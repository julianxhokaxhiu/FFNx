/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 myst6re                                            //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Tang-Tang Zhou                                     //
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
#include "vram.h"

#include "../ff8.h"
#include "../patch.h"
#include "../image/tim.h"

char next_texture_name[MAX_PATH] = "";
uint16_t *next_pal_data = nullptr;
int next_psxvram_x = -1;
int next_psxvram_y = -1;
int next_psxvram_pal_x = -1;
int next_psxvram_pal_y = -1;
uint8_t next_bpp = 2;
uint8_t next_scale = 1;
int8_t texl_id_left = -1;
int8_t texl_id_right = -1;

void ff8_upload_vram(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	const int x = pos_and_size[0];
	const int y = pos_and_size[1];
	const int w = pos_and_size[2];
	const int h = pos_and_size[3];
	bool isPal = next_pal_data != nullptr && (uint8_t *)next_pal_data == texture_buffer;

	if (trace_all || trace_vram) ffnx_trace("%s x=%d y=%d w=%d h=%d bpp=%d isPal=%d\n", __func__, x, y, w, h, next_bpp, isPal);

	texturePacker.setTexture(next_texture_name, texture_buffer, x, y, w, h, next_bpp, isPal);

	ff8_externals.sub_464850(x, y, x + w - 1, h + y - 1);

	next_pal_data = nullptr;
	*next_texture_name = '\0';
}

int read_vram_to_buffer_parent_call1(int a1, int structure, int x, int y, int w, int h, int bpp, int rel_pos, int a9, uint8_t *target)
{
	if (trace_all || trace_vram) ffnx_trace("%s: x=%d y=%d w=%d h=%d bpp=%d rel_pos=(%d, %d) a9=%d target=%X\n", __func__, x, y, w, h, bpp, rel_pos & 0xF, rel_pos >> 4, a9, target);

	next_psxvram_x = (x >> (2 - bpp)) + ((rel_pos & 0xF) << 6);
	next_psxvram_y = y + (((rel_pos >> 4) & 1) << 8);

	int ret = ff8_externals.sub_464F70(a1, structure, x, y, w, h, bpp, rel_pos, a9, target);

	next_psxvram_x = -1;
	next_psxvram_y = -1;

	return ret;
}

int read_vram_to_buffer_parent_call2(texture_page *tex_page, int rel_pos, int a3)
{
	if (trace_all || trace_vram) ffnx_trace("%s: x=%d y=%d color_key=%d rel_pos=(%d, %d)\n", __func__, tex_page->x, tex_page->y, tex_page->color_key, rel_pos & 0xF, rel_pos >> 4);

	next_psxvram_x = (tex_page->x >> (2 - tex_page->color_key)) + ((rel_pos & 0xF) << 6);
	next_psxvram_y = tex_page->y + (((rel_pos >> 4) & 1) << 8);

	int ret = ((int(*)(texture_page *, int, int))ff8_externals.sub_4653B0)(tex_page, rel_pos, a3);

	next_psxvram_x = -1;
	next_psxvram_y = -1;

	return ret;
}

int read_vram_to_buffer_with_palette1_parent_call1(texture_page *tex_page, int rel_pos, struc_50 *psxvram_structure)
{
	if (trace_all || trace_vram) ffnx_trace("%s: x=%d y=%d color_key=%d rel_pos=(%d, %d)\n", __func__, tex_page->x, tex_page->y, tex_page->color_key, rel_pos & 0xF, rel_pos >> 4);

	next_psxvram_x = (tex_page->x >> (2 - tex_page->color_key)) + ((rel_pos & 0xF) << 6);
	next_psxvram_y = tex_page->y + (((rel_pos >> 4) & 1) << 8);

	int ret = ((int(*)(texture_page*,int,struc_50*))ff8_externals.sub_464DB0)(tex_page, rel_pos, psxvram_structure);

	next_psxvram_x = -1;
	next_psxvram_y = -1;

	return ret;
}

int read_vram_to_buffer_with_palette1_parent_call2(texture_page *tex_page, int rel_pos, struc_50 *psxvram_structure)
{
	if (trace_all || trace_vram) ffnx_trace("%s: x=%d y=%d color_key=%d rel_pos=(%d, %d)\n", __func__, tex_page->x, tex_page->y, tex_page->color_key, rel_pos & 0xF, rel_pos >> 4);

	next_psxvram_x = (tex_page->x >> (2 - tex_page->color_key)) + ((rel_pos & 0xF) << 6);
	next_psxvram_y = tex_page->y + (((rel_pos >> 4) & 1) << 8);

	int ret = ((int(*)(texture_page*,int,struc_50*))ff8_externals.sub_465720)(tex_page, rel_pos, psxvram_structure);

	next_psxvram_x = -1;
	next_psxvram_y = -1;

	return ret;
}

void read_vram_to_buffer(uint8_t *vram, int vram_w_2048, uint8_t *target, int target_w, signed int w, int h, int bpp)
{
	if (trace_all || trace_vram) ffnx_trace("%s: vram_pos=(%d, %d) target=%X target_w=%d w=%d h=%d bpp=%d\n", __func__, next_psxvram_x, next_psxvram_y, int(target), target_w, w, h, bpp);

	if (next_psxvram_x == -1)
	{
		ffnx_warning("%s: cannot detect VRAM position\n", __func__);
	}
	else
	{
		texturePacker.registerTiledTex(target, next_psxvram_x, next_psxvram_y, bpp);
	}

	ff8_externals.read_vram_1(vram, vram_w_2048, target, target_w, w, h, bpp);
}

void read_vram_to_buffer_with_palette1(uint8_t *vram, int vram_w_2048, uint8_t *target, int target_w, int w, int h, int bpp, uint16_t *vram_palette)
{
	if (trace_all || trace_vram) ffnx_trace("%s: vram_pos=(%d, %d) target=%X target_w=%d w=%d h=%d bpp=%d vram_palette=%X\n", __func__, next_psxvram_x, next_psxvram_y, int(target), target_w, w, h, bpp, int(vram_palette));

	if (next_psxvram_x == -1)
	{
		ffnx_warning("%s: cannot detect VRAM position\n", __func__);
	}
	else
	{
		texturePacker.registerTiledTex(target, next_psxvram_x, next_psxvram_y, bpp, next_psxvram_pal_x, next_psxvram_pal_y);
	}

	ff8_externals.read_vram_2_paletted(vram, vram_w_2048, target, target_w, w, h, bpp, vram_palette);
}

void read_vram_to_buffer_with_palette2(uint8_t *vram, uint8_t *target, int w, int h, int bpp, uint16_t *vram_palette)
{
	if (trace_all || trace_vram) ffnx_trace("%s: vram_pos=(%d, %d) target=%X w=%d h=%d bpp=%d vram_palette=%X\n", __func__, next_psxvram_x, next_psxvram_y, int(target), w, h, bpp, int(vram_palette));

	if (next_psxvram_x == -1)
	{
		ffnx_warning("%s: cannot detect VRAM position\n", __func__);
	}
	else
	{
		texturePacker.registerTiledTex(target, next_psxvram_x, next_psxvram_y, bpp, next_psxvram_pal_x, next_psxvram_pal_y);
	}

	ff8_externals.read_vram_3_paletted(vram, target, w, h, bpp, vram_palette);
}

uint32_t ff8_credits_open_texture(char *fileName, char *buffer)
{
	if (trace_all || trace_vram) ffnx_trace("%s: %s\n", __func__, fileName);

	// {name}.lzs
	strncpy(next_texture_name, strrchr(fileName, '\\') + 1, sizeof(next_texture_name));
	next_bpp = 2;

	uint32_t ret = ff8_externals.credits_open_file(fileName, buffer);

	if (save_textures) Tim::fromLzsData((uint8_t *)buffer).save(next_texture_name);

	return ret;
}

void ff8_cdcheck_error_upload_vram(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	if (trace_all || trace_vram) ffnx_trace("%s\n", __func__);

	strncpy(next_texture_name, "discerr.lzs", sizeof(next_texture_name));
	next_bpp = 2;

	if (save_textures) Tim::fromLzsData(texture_buffer - 8).save(next_texture_name);

	ff8_upload_vram(pos_and_size, texture_buffer);
}

void ff8_upload_vram_triple_triad_1(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	if (trace_all || trace_vram) ffnx_trace("%s %p\n", __func__, texture_buffer);

	if (texture_buffer == ff8_externals.cardgame_tim_texture_intro)
	{
		strncpy(next_texture_name, "cardgame/intro", sizeof(next_texture_name));
		next_bpp = 2;
	}
	else if (texture_buffer == ff8_externals.cardgame_tim_texture_game)
	{
		strncpy(next_texture_name, "cardgame/game", sizeof(next_texture_name));
		next_bpp = 2;
	}

	if (save_textures && *next_texture_name != '\0')
	{
		ff8_tim tim = ff8_tim();
		tim.img_w = pos_and_size[2];
		tim.img_h = pos_and_size[3];
		tim.img_data = texture_buffer;
		Tim(next_bpp, tim).save(next_texture_name);
	}

	ff8_upload_vram(pos_and_size, texture_buffer);
}

void ff8_upload_vram_triple_triad_2_texture_name(uint8_t *texture_buffer)
{
	if (texture_buffer >= ff8_externals.cardgame_tim_texture_cards && texture_buffer < ff8_externals.cardgame_tim_texture_game)
	{
		strncpy(next_texture_name, "cardgame/cards", sizeof(next_texture_name));
		if (next_pal_data == (uint16_t *)texture_buffer)
		{
			if (save_textures) Tim::fromTimData(texture_buffer - 20).saveMultiPaletteGrid(next_texture_name, 28, 4, 2, true);
		}
		next_bpp = 1;
	}
	else if (texture_buffer >= ff8_externals.cardgame_tim_texture_icons && texture_buffer < ff8_externals.cardgame_tim_texture_font)
	{
		strncpy(next_texture_name, "cardgame/icons", sizeof(next_texture_name));
		if (next_pal_data == (uint16_t *)texture_buffer)
		{
			if (save_textures) Tim::fromTimData(texture_buffer - 20).save(next_texture_name, 0, 0, true);
		}
		next_bpp = 0;
	}
}

void ff8_upload_vram_triple_triad_2_palette(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	if (trace_all || trace_vram) ffnx_trace("%s %p\n", __func__, texture_buffer);

	next_pal_data = (uint16_t *)texture_buffer;
	ff8_upload_vram_triple_triad_2_texture_name(texture_buffer);
	next_bpp = 2;

	ff8_upload_vram(pos_and_size, texture_buffer);
}

void ff8_upload_vram_triple_triad_2_data(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	if (trace_all || trace_vram) ffnx_trace("%s %p\n", __func__, texture_buffer);

	ff8_upload_vram_triple_triad_2_texture_name(texture_buffer);

	ff8_upload_vram(pos_and_size, texture_buffer);
}

void ff8_wm_open_pal1(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	if (trace_all || trace_vram) ffnx_trace("%s %p\n", __func__, texture_buffer);

	next_pal_data = (uint16_t *)texture_buffer;

	ff8_upload_vram(pos_and_size, texture_buffer);

	next_pal_data = nullptr;
}

uint32_t ff8_wm_open_texture1(uint8_t *tim_file_data, ff8_tim *tim_infos)
{
	uint8_t bpp = tim_file_data[4] & 0x3;
	int *wm_section_38_textures_pos = *((int **)0x2040014);
	int searching_value = int(tim_file_data - (uint8_t *)wm_section_38_textures_pos);
	int timId = -1;
	int *dword_C75DB8 = (int *)0xC75DB8;

	if (trace_all || trace_vram) ffnx_trace("%s C75DB8=%d\n", __func__, *dword_C75DB8);

	// Find tim id relative to the start of section 38
	for (int *cur = wm_section_38_textures_pos; *cur != 0; ++cur) {
		if (*cur == searching_value) {
			timId = int(cur - wm_section_38_textures_pos);
			break;
		}
	}

	snprintf(next_texture_name, MAX_PATH, "world/dat/wmset/section38/texture%d", timId);

	next_bpp = bpp;

	uint32_t ret = ((uint32_t(*)(uint8_t*,ff8_tim*))0x541740)(tim_file_data, tim_infos);

	next_pal_data = tim_infos->pal_data;

	if (save_textures) Tim::fromTimData(tim_file_data).save(next_texture_name, 0, 0, true);

	return ret;
}

void ff8_wm_texl_palette_upload_vram(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	int *dword_C75DB8 = (int *)0xC75DB8;
	int16_t *word_203688E = (int16_t *)0x203688E;
	int texl_id = (*dword_C75DB8 >> 8) & 0xFF;

	if ((*word_203688E & 1) != 0 && (*dword_C75DB8 == 1284 || *dword_C75DB8 == 1798))
	{
		texl_id += 12;
	}

	bool is_left = pos_and_size[0] == 320;

	if (is_left)
	{
		texl_id_left = texl_id;
	}
	else
	{
		texl_id_right = texl_id;
	}

	if (trace_all || trace_vram) ffnx_trace("%s texl_id=%d\n", __func__, texl_id);

	snprintf(next_texture_name, MAX_PATH, "world/dat/texl/texture%d", texl_id);

	next_bpp = 1;

	ff8_upload_vram(pos_and_size, texture_buffer);

	// Worldmap texture fix

	texturePacker.clearTextureRedirections();

	uint16_t oldX = 16 * (texl_id - 2 * ((texl_id / 2) & 1) + (texl_id & 1)), oldY = ((texl_id / 2) & 1) ? 384 : 256;

	if (texl_id == 18 || texl_id == 19)
	{
		oldX = texl_id & 1 ? 96 : 64;
		oldY = 384;
	}

	if (texl_id == 16 || texl_id == 17 || texl_id > 19)
	{
		return; // TODO
	}

	Tim tim = Tim::fromTimData(texture_buffer - 20);

	TexturePacker::TextureInfos oldTexture(oldX, oldY, pos_and_size[2] / 2, pos_and_size[3] / 2, 0),
		newTexture(tim.imageX(), tim.imageY(), pos_and_size[2], pos_and_size[3], 1),
		oldPal, // TODO
		newPal(tim.paletteX(), tim.paletteY(), tim.paletteWidth(), tim.paletteHeight(), 2);

	texturePacker.setTextureRedirection(
		oldTexture,
		newTexture,
		oldPal,
		newPal
	);
}

void read_psxvram_alpha1_ssigpu_select2_sub_467360(int CLUT, uint8_t *rgba, int size)
{
	if (trace_all || trace_vram) ffnx_trace("%s CLUT=(%d, %d) rgba=%p size=%d\n", __func__, 16 * (CLUT & 0x3F), (CLUT >> 6) & 0x1FF, rgba, size);

	((void(*)(int,uint8_t*,int))0x467360)(CLUT, rgba, size);
}

void read_psxvram_alpha2_ssigpu_select2_sub_467460(uint16_t CLUT, uint8_t *rgba, int size)
{
	if (trace_all || trace_vram) ffnx_trace("%s CLUT=(%d, %d) rgba=%p size=%d\n", __func__, 16 * (CLUT & 0x3F), (CLUT >> 6) & 0x1FF, rgba, size);

	((void(*)(uint16_t,uint8_t*,int))0x467460)(CLUT, rgba, size);
}

void ssigpu_tx_select_2_sub_465CD0_call1(uint32_t a1, uint32_t header_with_bit_depth, int16_t CLUT_pos_x6y9, int32_t *a4, int32_t *clut_pos_out, int32_t *a6)
{
	if (trace_all || trace_vram) ffnx_trace("%s pos=(%d, %d) %d bit_depth=%d CLUT=(%d, %d) image_flags=%X\n", __func__,
		(header_with_bit_depth & 0xF) << 6, 16 * (header_with_bit_depth & 0x10),
		header_with_bit_depth & 0x1F, (header_with_bit_depth >> 7) & 3,
		16 * (CLUT_pos_x6y9 & 0x3F), (CLUT_pos_x6y9 >> 6) & 0x1FF,
		(header_with_bit_depth >> 5) & 3);

	next_psxvram_pal_x = 16 * (CLUT_pos_x6y9 & 0x3F);
	next_psxvram_pal_y = (CLUT_pos_x6y9 >> 6) & 0x1FF;

	((void(*)(uint32_t,uint32_t,int16_t,int32_t*,int32_t*,int32_t*))0x465CD0)(a1, header_with_bit_depth, CLUT_pos_x6y9, a4, clut_pos_out, a6);
}

void ssigpu_tx_select_2_sub_465CD0_call2(uint32_t a1, uint32_t header_with_bit_depth, int16_t CLUT_pos_x6y9, int32_t *a4, int32_t *clut_pos_out, int32_t *a6)
{
	if (trace_all || trace_vram) ffnx_trace("%s pos=(%d, %d) %d bit_depth=%d CLUT=(%d, %d) image_flags=%X\n", __func__,
		(header_with_bit_depth & 0xF) << 6, 16 * (header_with_bit_depth & 0x10),
		header_with_bit_depth & 0x1F, (header_with_bit_depth >> 7) & 3,
		16 * (CLUT_pos_x6y9 & 0x3F), (CLUT_pos_x6y9 >> 6) & 0x1FF,
		(header_with_bit_depth >> 5) & 3);

	next_psxvram_pal_x = 16 * (CLUT_pos_x6y9 & 0x3F);
	next_psxvram_pal_y = (CLUT_pos_x6y9 >> 6) & 0x1FF;

	((void(*)(uint32_t,uint32_t,int16_t,int32_t*,int32_t*,int32_t*))0x465CD0)(a1, header_with_bit_depth, CLUT_pos_x6y9, a4, clut_pos_out, a6);
}

void vram_init()
{
	texturePacker.setVram((uint8_t *)ff8_externals.psxvram_buffer);

	// pubintro
	replace_call(ff8_externals.open_lzs_image + 0x72, ff8_credits_open_texture);
	// cdcheck
	replace_call(ff8_externals.cdcheck_sub_52F9E0 + 0x1DC, ff8_cdcheck_error_upload_vram);
	// Triple Triad
	replace_call(ff8_externals.sub_5391B0 + 0x49, ff8_upload_vram_triple_triad_1);
	replace_call(ff8_externals.sub_5391B0 + 0x1CC, ff8_upload_vram_triple_triad_2_palette);
	replace_call(ff8_externals.sub_5391B0 + 0x1E1, ff8_upload_vram_triple_triad_2_data);
	// worldmap
	replace_call(0x53F0EC, ff8_wm_open_texture1); // Section 38
	replace_call(0x53F165, ff8_wm_open_pal1); // Section 38
	// wm texl project
	replace_call(0x5533C0 + 0x2EC, ff8_wm_texl_palette_upload_vram);
	replace_call(0x5533C0 + 0x3F0, ff8_wm_texl_palette_upload_vram);

	replace_function(ff8_externals.upload_psx_vram, ff8_upload_vram);

	// read_vram_to_buffer_parent_calls
	replace_call(ff8_externals.sub_464BD0 + 0x53, read_vram_to_buffer_parent_call1);
	replace_call(ff8_externals.sub_464BD0 + 0xED, read_vram_to_buffer_parent_call1);
	replace_call(ff8_externals.sub_464BD0 + 0x1A1, read_vram_to_buffer_parent_call1);
	replace_call(ff8_externals.ssigpu_tx_select_2_sub_465CE0 + 0x281, read_vram_to_buffer_parent_call1);

	replace_call(ff8_externals.sub_464BD0 + 0x79, read_vram_to_buffer_parent_call2);
	replace_call(ff8_externals.sub_464BD0 + 0x1B5, read_vram_to_buffer_parent_call2);

	// read_vram_to_buffer_with_palette1_parent_calls
	replace_call(ff8_externals.sub_464BD0 + 0xFC, read_vram_to_buffer_with_palette1_parent_call1);
	replace_call(ff8_externals.ssigpu_tx_select_2_sub_465CE0 + 0x2CF, read_vram_to_buffer_with_palette1_parent_call1);

	replace_call(ff8_externals.sub_464BD0 + 0xAF, read_vram_to_buffer_with_palette1_parent_call2);

	replace_call(uint32_t(ff8_externals.sub_464F70) + 0x2C5, read_vram_to_buffer);
	replace_call(ff8_externals.sub_4653B0 + 0x9D, read_vram_to_buffer);

	replace_call(ff8_externals.sub_464DB0 + 0xEC, read_vram_to_buffer_with_palette1);
	replace_call(ff8_externals.sub_465720 + 0xA5, read_vram_to_buffer_with_palette1);

	// Not used?
	replace_call(ff8_externals.sub_4649A0 + 0x13F, read_vram_to_buffer_with_palette2);

	/* replace_call(0x4661AC, read_psxvram_alpha1_ssigpu_select2_sub_467360);
	replace_call(0x4661C1, read_psxvram_alpha2_ssigpu_select2_sub_467460); */

	replace_call(0x461A4C, ssigpu_tx_select_2_sub_465CD0_call1);
	replace_call(0x462E13, ssigpu_tx_select_2_sub_465CD0_call2);
}
