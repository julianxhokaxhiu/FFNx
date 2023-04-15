/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
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

#include "uv_patch.h"

#include "../log.h"
#include "../patch.h"

struct TexCoord {
	uint8_t x, y;
};

struct MapBlockPolygon
{
	uint8_t vi[3];
	uint8_t ni[3];
	TexCoord pos[3];
	uint8_t texi, groundType;
	uint16_t flags;
};

struct SsigpuExecutionInstruction {
	uint32_t field_0;
	uint8_t r, g, b, func_id;
	uint32_t vertex_a;
	TexCoord tex_coord_a;
	uint16_t tex_pos_x6_y9;
	uint32_t field_10;
	uint32_t vertex_b;
	TexCoord tex_coord_b;
	uint16_t tex_header;
	uint32_t field_1C;
	uint32_t vertex_c;
	TexCoord tex_coord_c;
	uint16_t field_26;
};

int current_polygon = -1;
void *current_polygon_condition_old = nullptr;
bool old_dword_2045C90_value = false;
uint8_t *current_block_data_start = nullptr;
float *maybe_hundred_bak = nullptr;

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

void enrich_tex_coords_sub_45E3A0(SsigpuExecutionInstruction *a1)
{
	MapBlockPolygon *polygon = (MapBlockPolygon *)(current_block_data_start + 4 + current_polygon * sizeof(MapBlockPolygon));
	// Save the last bit of texture coordinates in field_26
	a1->field_26 = ((polygon->pos[0].x & 1) << 0) | ((polygon->pos[0].y & 1) << 1)
		| ((polygon->pos[1].x & 1) << 2) | ((polygon->pos[1].y & 1) << 3)
		| ((polygon->pos[2].x & 1) << 4) | ((polygon->pos[2].y & 1) << 5);

	((void(*)(SsigpuExecutionInstruction*))ff8_externals.sub_45E3A0)(a1);
}

void ssigpu_callback_sub_461E00(SsigpuExecutionInstruction *a1)
{
	uint8_t lost_bits = a1->field_26;

	((void(*)(SsigpuExecutionInstruction*))ff8_externals.sub_461E00)(a1);

	if (!(*(int *)ff8_externals.dword_1CA8848) && maybe_hundred_bak != nullptr) {
		// Not 512 to avoid space between textures
		maybe_hundred_bak[6] = double(uint32_t(a1->tex_coord_a.x) * 2 + ((lost_bits >> 0) & 1)) / 511.999;
		maybe_hundred_bak[7] = double(uint32_t(a1->tex_coord_a.y) * 2 + ((lost_bits >> 1) & 1)) / 511.999;
		maybe_hundred_bak[14] = double(uint32_t(a1->tex_coord_b.x) * 2 + ((lost_bits >> 2) & 1)) / 511.999;
		maybe_hundred_bak[15] = double(uint32_t(a1->tex_coord_b.y) * 2 + ((lost_bits >> 3) & 1)) / 511.999;
		maybe_hundred_bak[22] = double(uint32_t(a1->tex_coord_c.x) * 2 + ((lost_bits >> 4) & 1)) / 511.999;
		maybe_hundred_bak[23] = double(uint32_t(a1->tex_coord_c.y) * 2 + ((lost_bits >> 5) & 1)) / 511.999;

		maybe_hundred_bak = nullptr;
	}
}

void ssigpu_sub_461220(float *maybe_hundred)
{
	maybe_hundred_bak = maybe_hundred;

	((void(*)(float*))ff8_externals.sub_461220)(maybe_hundred);
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
	bool isUs = version == VERSION_FF8_12_US || version == VERSION_FF8_12_US_NV || version == VERSION_FF8_12_US_EIDOS || version == VERSION_FF8_12_US_EIDOS_NV;

	// Worldmap with Fog enabled
	replace_call(ff8_externals.sub_53BB90 + (isUs ? 0x42D : 0x43B), worldmap_fog_filter_polygons_in_block_1);
	replace_call(ff8_externals.sub_53BB90 + (isUs ? 0xADD : 0xB11), worldmap_fog_filter_polygons_in_block_1);
	replace_call(ff8_externals.sub_53BB90 + (isUs ? 0x442 : 0x450), worldmap_fog_filter_polygons_in_block_2);
	replace_call(ff8_externals.sub_53BB90 + (isUs ? 0xAFE : 0xB26), worldmap_fog_filter_polygons_in_block_2);
	replace_call(ff8_externals.worldmap_fog_filter_polygons_in_block_1 + (isUs ? 0x239 : 0x202), sub_45DF20);
	replace_call(ff8_externals.worldmap_fog_filter_polygons_in_block_1 + (isUs ? 0x5C8 : 0x5DC), enrich_tex_coords_sub_45E3A0);
	replace_call(ff8_externals.worldmap_fog_filter_polygons_in_block_2 + (isUs ? 0x281 : 0x241), sub_45DF20);
	replace_call(ff8_externals.worldmap_fog_filter_polygons_in_block_2 + (isUs ? 0x698 : 0x6CD), enrich_tex_coords_sub_45E3A0);
	replace_call(ff8_externals.sub_461E00 + 0x50, ssigpu_sub_461220);
	ff8_externals.ssigpu_callbacks_1[52] = uint32_t(ssigpu_callback_sub_461E00);
}
