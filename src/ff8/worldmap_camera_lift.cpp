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

// When terrain nearer than the player covers the player's spot on screen, the world map sets a flag and the camera
// tilts up until it sees over it (World_UpdateVehicleSteeringAndCamera). The terrain blocks test that spot as
// screen point (160, 180), in PSX pixels for the far blocks and PSX half pixels for the near blocks, against
// positions the PC GTE gives in eighths of a pixel: the test looks near the top-left corner of the screen
// (20, 22 or 40, 45 pixels) instead of the player. Here the limits go to 160 * 8 and 180 * 8.

#include "worldmap_camera_lift.h"

#include "../ff8.h"
#include "../patch.h"
#include "../globals.h"

#include <stdint.h>

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

void ff8_worldmap_camera_lift_init()
{
	const camera_lift_sites &sites = FF8_US_VERSION ? us_sites : other_sites;

	patch_code_dword(ff8_externals.worldmap_fog_filter_polygons_fallback + sites.far_x, LIFT_POINT_X);
	patch_code_dword(ff8_externals.worldmap_fog_filter_polygons_fallback + sites.far_y, LIFT_POINT_Y);

	patch_near_block(ff8_externals.worldmap_fog_filter_polygons_in_block_2, sites.fog_near_x, sites.fog_near_y);
	patch_near_block(ff8_externals.worldmap_no_fog_filter_polygons_in_block, sites.no_fog_near_x, sites.no_fog_near_y);
}
