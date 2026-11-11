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

// World_UpdateAndDrawTerrain_Fog / _NoFog -> the block drawers
#define TERRAIN_FOG_FAR_BLOCK 0x466
#define TERRAIN_FOG_NEAR_BLOCK 0x442
#define TERRAIN_NO_FOG_NEAR_BLOCK 0x439
// Far blocks: mov edx, 0xA0 / mov edx, 0xB4 against the triangle's screen x / y
#define FAR_BLOCK_X_LIMIT 0x529
#define FAR_BLOCK_Y_LIMIT 0x553

#define LIFT_POINT_X (160 * 8)
#define LIFT_POINT_Y (180 * 8)

// Near blocks: cmp word [vertex * 8 + screen x / y], 0x140 / 0x168, once per triangle corner
struct near_block_limits
{
	uint32_t x[3];
	uint32_t y[3];
};

static const near_block_limits fog_near_block = {{0x3D0, 0x3E2, 0x3F7}, {0x414, 0x426, 0x43B}};
static const near_block_limits no_fog_near_block = {{0x1F9, 0x208, 0x21D}, {0x235, 0x247, 0x256}};

static void patch_near_block(uint32_t near_block, const near_block_limits &limits)
{
	for (uint32_t offset : limits.x) patch_code_word(near_block + offset, LIFT_POINT_X);
	for (uint32_t offset : limits.y) patch_code_word(near_block + offset, LIFT_POINT_Y);
}

void ff8_worldmap_camera_lift_init()
{
	if (!FF8_US_VERSION) return;

	const uint32_t far_block = get_relative_call(ff8_externals.sub_53BB90, TERRAIN_FOG_FAR_BLOCK);

	patch_code_dword(far_block + FAR_BLOCK_X_LIMIT, LIFT_POINT_X);
	patch_code_dword(far_block + FAR_BLOCK_Y_LIMIT, LIFT_POINT_Y);

	patch_near_block(get_relative_call(ff8_externals.sub_53BB90, TERRAIN_FOG_NEAR_BLOCK), fog_near_block);
	patch_near_block(get_relative_call(ff8_externals.sub_53C750, TERRAIN_NO_FOG_NEAR_BLOCK), no_fog_near_block);
}
