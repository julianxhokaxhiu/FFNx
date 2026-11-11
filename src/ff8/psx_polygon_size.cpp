/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2026 HobbitDur                                          //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

// The PS1 GTE clamps screen positions to -1024..1023 pixels, and the PS1 GPU does not draw a triangle whose
// vertices are more than 1023 pixels apart horizontally or 511 vertically (a quad is drawn as two triangles, tested
// one by one). The PC port can give a vertex far outside that range, and its GPU layer draws the triangle: one
// triangle then covers the screen with a single texel of its texture (one frame of Eden's summon shows the pit and
// the sky in flat blue). The polygon handlers of both draw passes are wrapped: a triangle with a vertex outside the
// GTE range is skipped when, clamped like the GTE does, it is larger than the PS1 GPU draws.
// Triangles inside the GTE range are left alone: the PC port draws some large polygons (summon close-ups) that the
// PS1 version splits into smaller ones before they reach the GPU.

#include "psx_polygon_size.h"

#include "../ff8.h"
#include "../log.h"
#include "../globals.h"
#include "../patch.h"
#include <algorithm>

namespace
{
	constexpr uint8_t POLYGON_FIRST_CODE = 0x20;
	constexpr uint8_t POLYGON_LAST_CODE = 0x3F;
	constexpr uint8_t POLYGON_QUAD = 0x08;
	constexpr uint8_t POLYGON_TEXTURED = 0x04;
	constexpr uint8_t POLYGON_GOURAUD = 0x10;
	constexpr int PSX_MAX_WIDTH = 1023;
	constexpr int PSX_MAX_HEIGHT = 511;
	constexpr int GTE_MIN_POSITION = -1024;
	constexpr int GTE_MAX_POSITION = 1023;
	constexpr int EIGHTHS = 8;
	constexpr size_t POLYGON_CODES = POLYGON_LAST_CODE - POLYGON_FIRST_CODE + 1;

	typedef void (*polygon_handler)(uint8_t *packet);

	polygon_handler pass_1_handlers[POLYGON_CODES];
	polygon_handler pass_2_handlers[POLYGON_CODES];

	// Packet layout (the handler gets the command address - 4, the code is at +7): a vertex record holds
	// [gouraud color] screen xy [textured uv + clut / tpage]; records start after the color word of flat polygons.
	struct PolygonLayout
	{
		size_t first_record, record_size, xy, uv;
	};

	PolygonLayout layout_of(uint8_t code)
	{
		const bool gouraud = code & POLYGON_GOURAUD, textured = code & POLYGON_TEXTURED;
		PolygonLayout layout;
		layout.first_record = gouraud ? 4 : 8;
		layout.record_size = 4 + (gouraud ? 4 : 0) + (textured ? 4 : 0);
		layout.xy = gouraud ? 4 : 0;
		layout.uv = layout.xy + 4;
		return layout;
	}

	bool psx_skips_triangle(const uint8_t *packet, const PolygonLayout &layout, int first_vertex)
	{
		const int unit = *ff8_externals.ssigpu_packet_in_pixels_dword_1CA89FC ? 1 : EIGHTHS;
		int16_t x[3], y[3];
		for (int i = 0; i < 3; ++i)
		{
			const uint32_t xy = get_absolute_value(uint32_t(packet), layout.first_record + (first_vertex + i) * layout.record_size + layout.xy);
			x[i] = int16_t(xy & 0xFFFF);
			y[i] = int16_t(xy >> 16);
		}
		const int min_position = GTE_MIN_POSITION * unit, max_position = GTE_MAX_POSITION * unit;
		bool outside = false;
		int clamped_x[3], clamped_y[3];
		for (int i = 0; i < 3; ++i)
		{
			outside |= x[i] < min_position || x[i] > max_position || y[i] < min_position || y[i] > max_position;
			clamped_x[i] = std::clamp<int>(x[i], min_position, max_position);
			clamped_y[i] = std::clamp<int>(y[i], min_position, max_position);
		}
		if (!outside) return false;

		const int width = *std::max_element(clamped_x, clamped_x + 3) - *std::min_element(clamped_x, clamped_x + 3);
		const int height = *std::max_element(clamped_y, clamped_y + 3) - *std::min_element(clamped_y, clamped_y + 3);
		return width > PSX_MAX_WIDTH * unit || height > PSX_MAX_HEIGHT * unit;
	}

	void draw_polygon(polygon_handler handler, uint8_t *packet)
	{
		const uint8_t code = packet[7];
		const PolygonLayout layout = layout_of(code);

		if (!(code & POLYGON_QUAD))
		{
			if (psx_skips_triangle(packet, layout, 0))
			{
				if (trace_all) ffnx_trace("%s: skip triangle 0x%02X the PS1 would not draw\n", __func__, code);
				return;
			}
			handler(packet);
			return;
		}

		const bool first_skipped = psx_skips_triangle(packet, layout, 0);
		const bool second_skipped = psx_skips_triangle(packet, layout, 1);
		if (!first_skipped && !second_skipped)
		{
			handler(packet);
			return;
		}
		if (trace_all) ffnx_trace("%s: skip half of quad 0x%02X the PS1 would not draw (%d, %d)\n", __func__, code, first_skipped, second_skipped);

		const uint8_t triangle_code = code & ~POLYGON_QUAD;
		const polygon_handler triangle_handler = (code & 0x02 ? pass_2_handlers : pass_1_handlers)[triangle_code - POLYGON_FIRST_CODE];
		if (triangle_handler == nullptr) return;

		const size_t size = layout.first_record + 4 * layout.record_size;
		uint8_t triangle[4 + 12 * 4];

		if (!first_skipped)
		{
			// Vertices 0, 1, 2: the same packet read as a triangle
			memcpy_code(uint32_t(triangle), packet, size);
			triangle[7] = triangle_code;
			triangle_handler(triangle);
		}

		if (!second_skipped)
		{
			// Vertices 1, 2, 3: the records move down by one, the triangle keeps the quad's code, clut and tpage
			memcpy_code(uint32_t(triangle), packet, layout.first_record);
			memcpy_code(uint32_t(triangle + layout.first_record), packet + layout.first_record + layout.record_size, 3 * layout.record_size);
			if (code & POLYGON_TEXTURED)
			{
				for (int i = 0; i < 2; ++i)
				{
					const size_t clut_or_tpage = layout.first_record + i * layout.record_size + layout.uv + 2;
					memcpy_code(uint32_t(triangle + clut_or_tpage), packet + clut_or_tpage, sizeof(uint16_t));
				}
			}
			triangle[7] = triangle_code;
			triangle_handler(triangle);
		}
	}

	void pass_1_polygon(uint8_t *packet)
	{
		draw_polygon(pass_1_handlers[packet[7] - POLYGON_FIRST_CODE], packet);
	}

	void pass_2_polygon(uint8_t *packet)
	{
		draw_polygon(pass_2_handlers[packet[7] - POLYGON_FIRST_CODE], packet);
	}

	void wrap_polygon_handlers(uint32_t *callbacks, polygon_handler *handlers, polygon_handler wrapper)
	{
		for (size_t i = 0; i < POLYGON_CODES; ++i)
		{
			handlers[i] = reinterpret_cast<polygon_handler>(callbacks[POLYGON_FIRST_CODE + i]);
			if (handlers[i] != nullptr) callbacks[POLYGON_FIRST_CODE + i] = reinterpret_cast<uint32_t>(wrapper);
		}
	}
}

void ff8_psx_polygon_size_init()
{
	wrap_polygon_handlers(ff8_externals.ssigpu_callbacks_1, pass_1_handlers, pass_1_polygon);
	wrap_polygon_handlers(ff8_externals.ssigpu_callbacks_3, pass_2_handlers, pass_2_polygon);
}
