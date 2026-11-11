/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Tang-Tang Zhou                                     //
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

#include <stdint.h>
#include <unordered_map>

#include "../globals.h"
#include "../patch.h"
#include "../log.h"

#include "uv_patch.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <map>

// Polygon UVs sit this far (in texels) inside their texel, see pubintro_psxvram_buffer_init_sub_45B310
constexpr double polygon_uv_inset = 1.0 / 64.0;

bool ff8_worldmap_adjust_strip_uvs(const nvertex *drawn_vertices, uint32_t vertexcount,
	const uint16_t *indices, uint32_t count, FF8WorldMapStrip strip, std::vector<nvertex> &adjusted)
{
	if (strip == FF8WorldMapStrip::None) return false;
	if (!drawn_vertices || !indices || count % 3 != 0)
	{
		ffnx_unexpected_once("Invalid FF8 world-map strip triangle list\n");
		return false;
	}

	// Strip pieces are found on their whole-texel UVs: take the polygon table's inset back out
	const auto texel_uv = [](float uv)
	{
		const float texel = std::floor(uv * 256.0f);
		return uv * 256.0f - texel == float(polygon_uv_inset) ? texel / 256.0f : uv;
	};
	std::vector<nvertex> texel_vertices(drawn_vertices, drawn_vertices + vertexcount);
	for (nvertex &vertex : texel_vertices)
	{
		vertex.u = texel_uv(vertex.u);
		vertex.v = texel_uv(vertex.v);
	}
	const nvertex *vertices = texel_vertices.data();

	const float min_u = strip == FF8WorldMapStrip::Road ? 32.0f : 0.0f;
	std::vector<uint8_t> usage(vertexcount);
	for (uint32_t triangle = 0; triangle < count; triangle += 3)
	{
		bool selected = true;
		for (uint32_t corner = 0; corner < 3; ++corner)
		{
			const uint16_t index = indices[triangle + corner];
			if (index >= vertexcount)
			{
				ffnx_unexpected_once("Invalid FF8 world-map strip vertex index\n");
				return false;
			}
			const float u = vertices[index].u * 256.0f;
			const float v = vertices[index].v * 256.0f;
			// Roads and rails occupy separate tiles and use different palettes.
			selected &= u >= min_u && u <= min_u + 31.0f && v >= 0.0f && v <= 15.0f &&
				u == std::trunc(u) && v == std::trunc(v);
		}
		for (uint32_t corner = 0; corner < 3; ++corner)
			usage[indices[triangle + corner]] |= selected ? 1 : 2;
	}

	using Position = std::array<float, 4>;
	std::map<Position, std::array<float, 2>> joins;
	for (uint32_t index = 0; index < vertexcount; ++index)
	{
		if (!(usage[index] & 1)) continue;
		const nvertex &vertex = vertices[index];
		if (usage[index] == 3 || !std::isfinite(vertex._.x) || !std::isfinite(vertex._.y) ||
			!std::isfinite(vertex._.z) || !std::isfinite(vertex.color.w))
		{
			ffnx_unexpected_once("Unexpected FF8 world-map strip vertex sharing or position\n");
			return false;
		}
		const float v = vertex.v * 256.0f;
		const Position position = {vertex._.x, vertex._.y, vertex._.z, vertex.color.w};
		auto join = joins.try_emplace(position, std::array<float, 2>{v, v}).first;
		join->second[0] = std::min(join->second[0], v);
		join->second[1] = std::max(join->second[1], v);
	}
	if (joins.empty()) return false;

	adjusted.assign(drawn_vertices, drawn_vertices + vertexcount);
	for (uint32_t index = 0; index < vertexcount; ++index)
	{
		if (usage[index] != 1) continue;
		nvertex &vertex = adjusted[index];
		const float v = vertices[index].v * 256.0f;
		const Position position = {vertex._.x, vertex._.y, vertex._.z, vertex.color.w};
		const auto &join = joins.at(position);
		float inset = 0.0f;
		// Flipped pieces must agree under V -> 16 - V. Some authored cuts already do.
		if (v == 0.0f || v == 15.0f ||
			(join[0] < 8.0f && join[1] > 8.0f && join[0] + join[1] == 15.0f))
			inset = 0.5f;
		vertex.u = vertices[index].u + 0.5f / 256.0f;
		vertex.v = (v + inset) / 256.0f;
	}

	if (strip == FF8WorldMapStrip::Rail)
	{
		using Endpoint = std::array<float, 6>;
		using Edge = std::array<Endpoint, 2>;
		struct RailEdge
		{
			uint32_t triangle;
			uint16_t a, b;
			uint32_t uses;
		};
		std::map<Edge, RailEdge> edges;
		std::vector<uint32_t> components(count / 3);
		for (uint32_t triangle = 0; triangle < count / 3; ++triangle)
			components[triangle] = triangle;
		for (uint32_t triangle = 0; triangle < count / 3; ++triangle)
		{
			if (usage[indices[triangle * 3]] != 1) continue;
			for (uint32_t corner = 0; corner < 3; ++corner)
			{
				const uint16_t a = indices[triangle * 3 + corner];
				const uint16_t b = indices[triangle * 3 + (corner + 1) % 3];
				const auto endpoint = [](const nvertex &vertex)
				{
					return Endpoint{vertex._.x, vertex._.y, vertex._.z, vertex.color.w, vertex.u, vertex.v};
				};
				Edge edge = {endpoint(vertices[a]), endpoint(vertices[b])};
				if (edge[1] < edge[0]) std::swap(edge[0], edge[1]);
				auto [entry, inserted] = edges.try_emplace(edge, RailEdge{triangle, a, b, 1});
				if (inserted) continue;
				++entry->second.uses;
				const uint32_t from = components[triangle];
				const uint32_t to = components[entry->second.triangle];
				if (from != to)
					for (auto &component : components)
						if (component == from) component = to;
			}
		}

		std::map<uint32_t, std::vector<std::array<uint16_t, 2>>> boundaries;
		for (const auto &[key, edge] : edges)
		{
			const nvertex &a = vertices[edge.a], &b = vertices[edge.b];
			if (edge.uses == 1 && a.v == b.v && (a.v == 0.0f || a.v == 15.0f / 256.0f))
				boundaries[components[edge.triangle]].push_back({edge.a, edge.b});
		}
		for (uint32_t triangle = 0; triangle < count / 3; ++triangle)
		{
			if (usage[indices[triangle * 3]] != 1) continue;
			const auto boundary = boundaries.find(components[triangle]);
			if (boundary == boundaries.end()) continue;
			for (uint32_t corner = 0; corner < 3; ++corner)
			{
				const uint16_t index = indices[triangle * 3 + corner];
				const nvertex &vertex = vertices[index];
				if (vertex.v == 0.0f || vertex.v == 15.0f / 256.0f || vertex.color.w <= 0.0f) continue;
				std::array<std::array<float, 3>, 2> points = {};
				std::array<uint32_t, 2> matches = {};
				for (const auto &edge : boundary->second)
				{
					const nvertex &a = vertices[edge[0]], &b = vertices[edge[1]];
					if (a.u == b.u || vertex.u < std::min(a.u, b.u) || vertex.u > std::max(a.u, b.u) ||
						a.color.w <= 0.0f || b.color.w <= 0.0f) continue;
					const uint32_t side = a.v == 0.0f ? 0 : 1;
					std::array<float, 3> point;
					if (vertex.u == a.u) point = {a._.x, a._.y, a.color.w};
					else if (vertex.u == b.u) point = {b._.x, b._.y, b.color.w};
					else
					{
						// TL vertices carry reciprocal W; interpolate in homogeneous space.
						const float t = (vertex.u - a.u) / (b.u - a.u);
						const float w = (1.0f - t) / a.color.w + t / b.color.w;
						const float s = (t / b.color.w) / w;
						point = {a._.x + s * (b._.x - a._.x), a._.y + s * (b._.y - a._.y), 1.0f / w};
					}
					if (matches[side] == 0)
					{
						points[side] = point;
						matches[side] = 1;
					}
					else if (points[side] != point) matches[side] = 2;
				}
				// Junctions or incomplete strips do not define a unique cross section.
				if (matches[0] != 1 || matches[1] != 1) continue;
				const float dx = points[1][0] - points[0][0], dy = points[1][1] - points[0][1];
				const float length2 = dx * dx + dy * dy;
				if (length2 == 0.0f) continue;
				const float s = ((vertex._.x - points[0][0]) * dx + (vertex._.y - points[0][1]) * dy) / length2;
				if (s <= 0.0f || s >= 1.0f) continue;
				const float t = s * points[1][2] / ((1.0f - s) * points[0][2] + s * points[1][2]);
				adjusted[index].v = (0.5f + 15.0f * t) / 256.0f;
			}
		}
	}
	return true;
}

struct MapBlockPolygon
{
	uint8_t vi[3];
	uint8_t ni[3];
	TexCoord pos[3];
	uint8_t texi, groundType;
	uint16_t flags;
};

int current_polygon = -1;
void *current_polygon_condition_old = nullptr;
bool old_dword_2045C90_value = false;
uint8_t *current_block_data_start = nullptr;
float *maybe_hundred_bak = nullptr;
float psx_floats512[512] = {};
static std::unordered_map<const SsigpuExecutionInstructionTriangle52 *, uint8_t> worldmap_uv_bits;

void worldmap_fog_filter_polygons_in_block_enter(uint8_t *block)
{
	current_block_data_start = block;
	current_polygon = -1;
	old_dword_2045C90_value = *(bool *)ff8_externals.worldmap_has_polygon_condition_2045C90;
	current_polygon_condition_old = *(void **)ff8_externals.worldmap_polygon_condition_2045C8C;
}

void worldmap_fog_filter_polygons_in_block_leave()
{
	*(bool *)ff8_externals.worldmap_has_polygon_condition_2045C90 = old_dword_2045C90_value;
	*(void **)ff8_externals.worldmap_polygon_condition_2045C8C = current_polygon_condition_old;
	current_block_data_start = nullptr;
	current_polygon = -1;
}

void worldmap_fog_filter_polygons_in_block_1(uint8_t *block, int a2, void *out)
{
	worldmap_fog_filter_polygons_in_block_enter(block);

	((void(*)(uint8_t*,int,void*))ff8_externals.worldmap_fog_filter_polygons_in_block_1)(block, a2, out);

	worldmap_fog_filter_polygons_in_block_leave();
}

void worldmap_fog_filter_polygons_in_block_2(uint8_t *block, int a2, void *out)
{
	worldmap_fog_filter_polygons_in_block_enter(block);

	((void(*)(uint8_t*,int,void*))ff8_externals.worldmap_fog_filter_polygons_in_block_2)(block, a2, out);

	worldmap_fog_filter_polygons_in_block_leave();
}

bool current_polygon_condition(int polygon_id)
{
	current_polygon = polygon_id;

	if (!old_dword_2045C90_value) {
		return true;
	}

	return ((bool(*)(int))current_polygon_condition_old)(polygon_id);
}

void sub_45DF20(int a1, int a2, int a3)
{
	void **current_polygon_condition_dword_2045964 = (void **)ff8_externals.worldmap_polygon_condition_2045C8C;

	*(bool *)ff8_externals.worldmap_has_polygon_condition_2045C90 = true;
	*current_polygon_condition_dword_2045964 = current_polygon_condition;

	((void(*)(int,int,int))ff8_externals.worldmap_sub_45DF20)(a1, a2, a3);
}

SsigpuExecutionInstructionTriangle52 *enrich_tex_coords_sub_45E3A0(SsigpuExecutionInstructionTriangle52 *a1)
{
	if (current_block_data_start == nullptr || current_polygon < 0)
	{
		worldmap_uv_bits.erase(a1);
		ffnx_warning("%s: missing world-map polygon for UV recovery.\n", __func__);
	}
	else
	{
		MapBlockPolygon *polygon = (MapBlockPolygon *)(current_block_data_start + 4 + current_polygon * sizeof(MapBlockPolygon));
		// Packet padding is also present in unrelated GT3 draws and may contain stale data.
		worldmap_uv_bits[a1] = ((polygon->pos[0].u & 1) << 0) | ((polygon->pos[0].v & 1) << 1)
			| ((polygon->pos[1].u & 1) << 2) | ((polygon->pos[1].v & 1) << 3)
			| ((polygon->pos[2].u & 1) << 4) | ((polygon->pos[2].v & 1) << 5);
	}

	return ((SsigpuExecutionInstructionTriangle52*(*)(SsigpuExecutionInstructionTriangle52*))ff8_externals.sub_45E3A0)(a1);
}

ff8_graphics_object *ssigpu_callback_sub_461E00(SsigpuExecutionInstructionTriangle52 *a1)
{
	maybe_hundred_bak = nullptr;
	ff8_graphics_object *ret = ((ff8_graphics_object*(*)(SsigpuExecutionInstructionTriangle52*))ff8_externals.sub_461E00)(a1);
	float *vertices = maybe_hundred_bak;
	maybe_hundred_bak = nullptr;

	const auto uv_bits = worldmap_uv_bits.find(a1);
	if (uv_bits != worldmap_uv_bits.end() && !(*(int *)ff8_externals.dword_1CA8848) && vertices != nullptr) {
		const uint8_t lost_bits = uv_bits->second;
		vertices[6] = psx_floats512[uint32_t(a1->tex_coord_a.u) * 2 + ((lost_bits >> 0) & 1)];
		vertices[7] = psx_floats512[uint32_t(a1->tex_coord_a.v) * 2 + ((lost_bits >> 1) & 1)];
		vertices[14] = psx_floats512[uint32_t(a1->tex_coord_b.u) * 2 + ((lost_bits >> 2) & 1)];
		vertices[15] = psx_floats512[uint32_t(a1->tex_coord_b.v) * 2 + ((lost_bits >> 3) & 1)];
		vertices[22] = psx_floats512[uint32_t(a1->tex_coord_c.u) * 2 + ((lost_bits >> 4) & 1)];
		vertices[23] = psx_floats512[uint32_t(a1->tex_coord_c.v) * 2 + ((lost_bits >> 5) & 1)];
	}

	return ret;
}

float *ssigpu_sub_461220(float *maybe_hundred)
{
	maybe_hundred_bak = maybe_hundred;

	return ((float*(*)(float*))ff8_externals.sub_461220)(maybe_hundred);
}

int ssigpu_render_packets(int ordering_table)
{
	int ret = ((int(*)(int))ff8_externals.sub_45D080)(ordering_table);

	// Native sorting keeps the packet addresses until this batch has finished drawing.
	worldmap_uv_bits.clear();
	maybe_hundred_bak = nullptr;

	return ret;
}

int pubintro_psxvram_buffer_init_sub_45B310()
{
	if (trace_all) ffnx_trace("%s\n", __func__);

	int ret = ((int(*)())ff8_externals.sub_45B310)();

	// Texel 255 is inside a 256-wide page, not its exclusive outer boundary.
	// Each UV sits inside its texel: exactly on the edge, nearest sampling can take the previous texel.
	for (int i = 0; i < 256; ++i) {
		ff8_externals.psx_floats1[i] = float((double(i) + polygon_uv_inset) / 256.0);
	}

	for (int i = 0; i < 512; ++i) {
		psx_floats512[i] = float(double(i) / 512.0);
	}

	return ret;
}

void uv_patch_init()
{
	/* We hook a lot of stuff here to bring back full precision in texture UVs
	 * - We need the UVs directly from the game data. To obtain this,
	 *   we get the current block data and the current polygon from the game.
	 * - The game stores the UVs divided by 2, so we remember the forgotten bits
	 *   by packet address until the SSIGPU batch has finished drawing.
	 * - And when the game uses the UVs from the SSIGPU instruction, we alter
	 *   the computation of U and V using the forgotten bits.
	 */

	// Worldmap with Fog enabled
	replace_call(ff8_externals.sub_53BB90 + (FF8_US_VERSION ? 0x42D : 0x43B), worldmap_fog_filter_polygons_in_block_1);
	replace_call(ff8_externals.sub_53BB90 + (FF8_US_VERSION ? 0xADD : 0xB11), worldmap_fog_filter_polygons_in_block_1);
	replace_call(ff8_externals.sub_53BB90 + (FF8_US_VERSION ? 0x442 : 0x450), worldmap_fog_filter_polygons_in_block_2);
	replace_call(ff8_externals.sub_53BB90 + (FF8_US_VERSION ? 0xAF2 : 0xB26), worldmap_fog_filter_polygons_in_block_2);
	replace_call(ff8_externals.worldmap_fog_filter_polygons_in_block_1 + (FF8_US_VERSION ? 0x1FC : 0x202), sub_45DF20);
	replace_call(ff8_externals.worldmap_fog_filter_polygons_in_block_1 + (FF8_US_VERSION ? 0x5C8 : 0x5DC), enrich_tex_coords_sub_45E3A0);
	replace_call(ff8_externals.worldmap_fog_filter_polygons_in_block_2 + (FF8_US_VERSION ? 0x242 : 0x241), sub_45DF20);
	replace_call(ff8_externals.worldmap_fog_filter_polygons_in_block_2 + (FF8_US_VERSION ? 0x698 : 0x6CD), enrich_tex_coords_sub_45E3A0);
	replace_call(ff8_externals.sub_461E00 + 0x50, ssigpu_sub_461220);
	ff8_externals.ssigpu_callbacks_1[52] = uint32_t(ssigpu_callback_sub_461E00);

	replace_call(ff8_externals.sub_45D610 + 0x5, ssigpu_render_packets);
	replace_call(ff8_externals.pubintro_init + 0x91, pubintro_psxvram_buffer_init_sub_45B310);
}
