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

#include "../globals.h"
#include "../patch.h"
#include "../log.h"

#include "uv_patch.h"

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

void worldmap_fog_filter_polygons_in_block_leave()
{
	*(bool *)ff8_externals.worldmap_has_polygon_condition_2045C90 = old_dword_2045C90_value;
	*(void **)ff8_externals.worldmap_polygon_condition_2045C8C = current_polygon_condition_old;
}

void worldmap_fog_filter_polygons_in_block_1(uint8_t *block, int a2, void *out)
{
	current_block_data_start = block;

	((void(*)(uint8_t*,int,void*))ff8_externals.worldmap_fog_filter_polygons_in_block_1)(block, a2, out);

	worldmap_fog_filter_polygons_in_block_leave();
}

void worldmap_fog_filter_polygons_in_block_2(uint8_t *block, int a2, void *out)
{
	current_block_data_start = block;

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

	old_dword_2045C90_value = *(bool *)ff8_externals.worldmap_has_polygon_condition_2045C90;

	if (!old_dword_2045C90_value) {
		*(bool *)ff8_externals.worldmap_has_polygon_condition_2045C90 = true;
		current_polygon_condition_old = *current_polygon_condition_dword_2045964;
		*current_polygon_condition_dword_2045964 = current_polygon_condition;
	}

	((void(*)(int,int,int))ff8_externals.worldmap_sub_45DF20)(a1, a2, a3);
}

void enrich_tex_coords_sub_45E3A0(SsigpuExecutionInstructionTriangle52 *a1)
{
	MapBlockPolygon *polygon = (MapBlockPolygon *)(current_block_data_start + 4 + current_polygon * sizeof(MapBlockPolygon));
	// Save the last bit of texture coordinates in field_26
	a1->field_26 = ((polygon->pos[0].u & 1) << 0) | ((polygon->pos[0].v & 1) << 1)
		| ((polygon->pos[1].u & 1) << 2) | ((polygon->pos[1].v & 1) << 3)
		| ((polygon->pos[2].u & 1) << 4) | ((polygon->pos[2].v & 1) << 5);

	((void(*)(SsigpuExecutionInstructionTriangle52*))ff8_externals.sub_45E3A0)(a1);
}

void ssigpu_callback_sub_461E00(SsigpuExecutionInstructionTriangle52 *a1)
{
	uint8_t lost_bits = a1->field_26;

	((void(*)(SsigpuExecutionInstructionTriangle52*))ff8_externals.sub_461E00)(a1);

	if (!(*(int *)ff8_externals.dword_1CA8848) && maybe_hundred_bak != nullptr) {
		maybe_hundred_bak[6] = psx_floats512[uint32_t(a1->tex_coord_a.u) * 2 + ((lost_bits >> 0) & 1)];
		maybe_hundred_bak[7] = psx_floats512[uint32_t(a1->tex_coord_a.v) * 2 + ((lost_bits >> 1) & 1)];
		maybe_hundred_bak[14] = psx_floats512[uint32_t(a1->tex_coord_b.u) * 2 + ((lost_bits >> 2) & 1)];
		maybe_hundred_bak[15] = psx_floats512[uint32_t(a1->tex_coord_b.v) * 2 + ((lost_bits >> 3) & 1)];
		maybe_hundred_bak[22] = psx_floats512[uint32_t(a1->tex_coord_c.u) * 2 + ((lost_bits >> 4) & 1)];
		maybe_hundred_bak[23] = psx_floats512[uint32_t(a1->tex_coord_c.v) * 2 + ((lost_bits >> 5) & 1)];

		maybe_hundred_bak = nullptr;
	}
}

void ssigpu_sub_461220(float *maybe_hundred)
{
	maybe_hundred_bak = maybe_hundred;

	((void(*)(float*))ff8_externals.sub_461220)(maybe_hundred);
}

int pubintro_psxvram_buffer_init_sub_45B310()
{
	if (trace_all) ffnx_trace("%s\n", __func__);

	int ret = ((int(*)())ff8_externals.sub_45B310)();

	// Divide by 255 instead of 256, and don't alter values of psx_floats1[0] and psx_floats1[255]
	for (int i = 0; i < 256; ++i) {
		ff8_externals.psx_floats1[i] = float(double(i) / 255.0);
	}

	for (int i = 0; i < 512; ++i) {
		psx_floats512[i] = float(double(i) / 511.0);
	}

	return ret;
}

void uv_patch_init()
{
	/* We hook a lot of stuff here to bring back full precision in texture UVs
	 * - We need the UVs directly from the game data. To obtain this,
	 *   we get the current block data and the current polygon from the game.
	 * - Then we put this information into the SSIGPU instruction, the game will
	 *   only stores the UVs divided by 2, so we need to remember the forgotten bits at least.
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

	replace_call(ff8_externals.pubintro_init + 0x91, pubintro_psxvram_buffer_init_sub_45B310);
}
