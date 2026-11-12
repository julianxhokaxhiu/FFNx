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

#include "../globals.h"
#include "../ff8.h"
#include "../patch.h"
#include "world.h"

#include <array>
#include <stdint.h>

// World map fixes

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

static void patch_model_depth()
{
	const uint32_t draw_models = get_relative_call(ff8_externals.worldmap_with_fog_sub_53FAC0, FF8_US_VERSION ? 0x267 : 0x268);
	const uint32_t draw_model = get_relative_call(draw_models, FF8_US_VERSION ? 0x5E5 : 0x63A);
	const uint32_t draw_quads = get_relative_call(draw_model, FF8_US_VERSION ? 0x313 : 0x345);
	const uint32_t average_triangle_depth = get_relative_call(draw_model, FF8_US_VERSION ? 0x274 : 0x29B);
	const uint32_t average_quad_depth = get_relative_call(draw_quads, FF8_US_VERSION ? 0x105 : 0x107);
	ff8_worldmap_model_depths = reinterpret_cast<uint32_t *>(ff8_externals.dword_1CA8A50);
	ff8_worldmap_model_triangle_depth_scale = reinterpret_cast<int16_t *>(get_absolute_value(average_triangle_depth, 0x31));
	ff8_worldmap_model_quad_depth_scale = reinterpret_cast<int16_t *>(get_absolute_value(average_quad_depth, 0x35));
	ff8_worldmap_model_submit = reinterpret_cast<decltype(ff8_worldmap_model_submit)>(get_relative_call(draw_model, FF8_US_VERSION ? 0x2E0 : 0x30A));
	replace_call(draw_model + (FF8_US_VERSION ? 0x2E0 : 0x30A), ff8_worldmap_submit_model_polygon<3>);
	replace_call(draw_quads + (FF8_US_VERSION ? 0x181 : 0x187), ff8_worldmap_submit_model_polygon<4>);
}

// When terrain nearer than the player covers the player's spot on screen, the world map sets a flag and the camera
// tilts up until it sees over it (World_UpdateVehicleSteeringAndCamera). The terrain blocks test that spot as
// screen point (160, 180), in PSX pixels for the far blocks and PSX half pixels for the near blocks, against
// positions the PC GTE gives in eighths of a pixel: the test looks near the top-left corner of the screen
// (20, 22 or 40, 45 pixels) instead of the player. Here the limits go to 160 * 8 and 180 * 8.
#define LIFT_POINT_X (160 * 8)
#define LIFT_POINT_Y (180 * 8)

// Far block: mov edx, 0xA0 / mov edx, 0xB4 against the triangle's screen x / y.
// Near blocks: cmp word [vertex * 8 + screen x / y], 0x140 / 0x168, once per triangle corner.
// The US release has one layout, the FR, DE, IT, ES and JP releases share the other one.
struct camera_lift_sites
{
	uint32_t far_x, far_y;
	uint32_t fog_near_x[3], fog_near_y[3];
	uint32_t no_fog_near_x[3], no_fog_near_y[3];
};

static const camera_lift_sites us_sites = {0x529, 0x553, {0x3D0, 0x3E2, 0x3F7}, {0x414, 0x426, 0x43B}, {0x1F9, 0x208, 0x21D}, {0x235, 0x247, 0x256}};
static const camera_lift_sites other_sites = {0x54A, 0x572, {0x3D5, 0x3E8, 0x3FF}, {0x41E, 0x431, 0x448}, {0x1EC, 0x1FB, 0x212}, {0x22B, 0x23E, 0x24D}};

static void patch_near_block(uint32_t near_block, const uint32_t (&x)[3], const uint32_t (&y)[3])
{
	for (uint32_t offset : x) patch_code_word(near_block + offset, LIFT_POINT_X);
	for (uint32_t offset : y) patch_code_word(near_block + offset, LIFT_POINT_Y);
}

static void patch_camera_lift()
{
	const camera_lift_sites &sites = FF8_US_VERSION ? us_sites : other_sites;

	patch_code_dword(ff8_externals.worldmap_fog_filter_polygons_fallback + sites.far_x, LIFT_POINT_X);
	patch_code_dword(ff8_externals.worldmap_fog_filter_polygons_fallback + sites.far_y, LIFT_POINT_Y);

	patch_near_block(ff8_externals.worldmap_fog_filter_polygons_in_block_2, sites.fog_near_x, sites.fog_near_y);
	patch_near_block(ff8_externals.worldmap_no_fog_filter_polygons_in_block, sites.no_fog_near_x, sites.no_fog_near_y);
}

void ff8_world_init()
{
	patch_model_depth();
	patch_camera_lift();
}
