/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include "../globals.h"
#include "../ff8.h"
#include "../patch.h"
#include "world.h"

#include <array>

static uint32_t *ff8_worldmap_model_depths;
static int16_t *ff8_worldmap_model_triangle_depth_scale;
static int16_t *ff8_worldmap_model_quad_depth_scale;
static uint32_t (*ff8_worldmap_model_submit)(int *, uint32_t *, int, int, int, int);

template<int vertex_count>
static uint32_t ff8_worldmap_submit_model_polygon(int *ordering_table, uint32_t *packet, int, int, int, int)
{
	std::array<int, 4> depths = {};
	const uint32_t depth_scale = static_cast<uint32_t>(vertex_count == 3
		? *ff8_worldmap_model_triangle_depth_scale
		: *ff8_worldmap_model_quad_depth_scale);

	// AVSZ3 uses SZ1-SZ3; AVSZ4 uses SZ0-SZ3. Match the native scale for each packet type.
	for (int vertex = 0; vertex < vertex_count; ++vertex)
	{
		const uint32_t raw_depth = static_cast<uint16_t>(ff8_worldmap_model_depths[4 - vertex_count + vertex]);
		depths[vertex] = (vertex_count * raw_depth * depth_scale) >> 13;
	}

	// The caller has already culled the polygon and chosen its native average-depth sorting bucket.
	return ff8_worldmap_model_submit(ordering_table, packet, depths[0], depths[1], depths[2], depths[3]);
}

void ff8_worldmap_model_depth_init()
{
	const uint32_t draw_models = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, FF8_US_VERSION ? 0x267 : 0x268);
	const uint32_t draw_model = get_relative_call(draw_models, FF8_US_VERSION ? 0x5E5 : 0x63A);
	const uint32_t draw_quads = get_relative_call(draw_model, FF8_US_VERSION ? 0x313 : 0x345);
	const uint32_t set_quad_depths = get_relative_call(draw_quads, FF8_US_VERSION ? 0x100 : 0x102);
	const uint32_t average_triangle_depth = get_relative_call(draw_model, FF8_US_VERSION ? 0x274 : 0x29B);
	const uint32_t average_quad_depth = get_relative_call(draw_quads, FF8_US_VERSION ? 0x105 : 0x107);
	ff8_worldmap_model_depths = reinterpret_cast<uint32_t *>(get_absolute_value(set_quad_depths, 0xD));
	ff8_worldmap_model_triangle_depth_scale = reinterpret_cast<int16_t *>(get_absolute_value(average_triangle_depth, 0x31));
	ff8_worldmap_model_quad_depth_scale = reinterpret_cast<int16_t *>(get_absolute_value(average_quad_depth, 0x35));
	ff8_worldmap_model_submit = reinterpret_cast<decltype(ff8_worldmap_model_submit)>(get_relative_call(draw_model, FF8_US_VERSION ? 0x2E0 : 0x30A));
	replace_call(draw_model + (FF8_US_VERSION ? 0x2E0 : 0x30A), ff8_worldmap_submit_model_polygon<3>);
	replace_call(draw_quads + (FF8_US_VERSION ? 0x181 : 0x187), ff8_worldmap_submit_model_polygon<4>);
}
