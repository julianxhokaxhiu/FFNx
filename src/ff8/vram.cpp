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
#include "vram.h"

#include "../ff8.h"
#include "../patch.h"
#include "../macro.h"
#include "../image/tim.h"
#include "../utils.h"
#include "field/background.h"
#include "field/chara_one.h"
#include "world/chara_one.h"
#include "world/wmset.h"
#include "battle/stage.h"
#include "file.h"

#include <unordered_map>
#include <vector>
#include <filesystem>

TexturePacker texturePacker;

char next_texture_name[MAX_PATH] = "";
TexturePacker::TextureInfos texture_infos = TexturePacker::TextureInfos();
TexturePacker::TextureInfos palette_infos = TexturePacker::TextureInfos();
uint16_t *next_pal_data = nullptr;
int next_psxvram_x = -1;
int next_psxvram_y = -1;
int next_texl_id = 0;
int next_texture_count = -1;
int next_do_not_clear_old_texture = false;
Tim::Bpp next_bpp = Tim::Bpp16;
int last_CLUT = 0;

// Field background
uint8_t *mim_texture_buffer = nullptr;
// Field models
std::unordered_map<uint32_t, CharaOneModel> chara_one_models;
std::vector<uint32_t> chara_one_loaded_models;
int chara_one_current_pos = 0;
uint32_t chara_one_current_model = 0;
uint32_t chara_one_current_mch = 0;
uint32_t chara_one_current_texture = 0;
// World
std::vector<CharaOneModelTextures> chara_one_world_texture_offsets;
uint8_t *chara_one_world_data;
std::vector<WmsetSection17Texture> wm_wmset_wave_animations_textures;
std::unordered_map<uint32_t, WmsetSection41Texture> wm_wmset_palette_animations_textures;
// Battle
char battle_texture_name[MAX_PATH] = "";
int battle_texture_id = 0;
Stage stage;

uint8_t *ff8_vram_seek(int xBpp2, int y)
{
	return ff8_externals.psxvram_buffer + VRAM_DEPTH * (xBpp2 + y * VRAM_WIDTH);
}

bool ff8_vram_save(const char *fileName, Tim::Bpp bpp)
{
	uint16_t palette[256] = {};

	ff8_tim tim_infos = ff8_tim();

	tim_infos.img_data = ff8_externals.psxvram_buffer;
	tim_infos.img_w = VRAM_WIDTH;
	tim_infos.img_h = VRAM_HEIGHT;

	if (bpp < Tim::Bpp16)
	{
		tim_infos.pal_data = palette;
		tim_infos.pal_h = 1;
		tim_infos.pal_w = bpp == Tim::Bpp4 ? 16 : 256;

		// Greyscale palette
		for (int i = 0; i < tim_infos.pal_w; ++i)
		{
			uint8_t color = bpp == Tim::Bpp4 ? i * 16 : i;
			palette[i] = color | (color << 5) | (color << 10);
		}
	}

	return Tim(bpp, tim_infos).save(fileName, int(bpp), false);
}

void ff8_upload_vram(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	const int16_t x = pos_and_size[0];
	const int16_t y = pos_and_size[1];
	const int16_t w = pos_and_size[2];
	const int16_t h = pos_and_size[3];
	bool isPal = (next_pal_data != nullptr && (uint8_t *)next_pal_data == texture_buffer)
		|| (palette_infos.isValid() && palette_infos.x() == x && palette_infos.y() == y && palette_infos.w() == w && palette_infos.h() == h);

	if (trace_all || trace_vram) ffnx_trace("%s x=%d y=%d w=%d h=%d bpp=%d isPal=%d texture_buffer=0x%X\n", __func__, x, y, w, h, next_bpp, isPal, texture_buffer);

	uint8_t *vram = ff8_vram_seek(x, y);
	const int vramLineWidth = VRAM_DEPTH * VRAM_WIDTH;
	const int lineWidth = VRAM_DEPTH * w;

	for (int i = 0; i < h; ++i) {
		memcpy(vram, texture_buffer, lineWidth);

		texture_buffer += lineWidth;
		vram += vramLineWidth;
	}

	if (texture_infos.isValid() && palette_infos.isValid()) {
		texturePacker.setTexture(next_texture_name, texture_infos, palette_infos, next_texture_count, !next_do_not_clear_old_texture);

		texture_infos = TexturePacker::TextureInfos();
		palette_infos = TexturePacker::TextureInfos();
	} else if (!texture_infos.isValid() && !palette_infos.isValid() && !isPal) {
		texturePacker.setTexture(next_texture_name, TexturePacker::TextureInfos(x, y, w, h, next_bpp), TexturePacker::TextureInfos(), next_texture_count, !next_do_not_clear_old_texture);
	}

	ff8_externals.sub_464850(x, y, x + w - 1, h + y - 1);

	*next_texture_name = '\0';
	next_do_not_clear_old_texture = false;
	next_texture_count = -1;
}

void ff8_vram_copy_part(int x, int y, int w, int h, int target_x, int target_y)
{
	if (trace_all || trace_vram) ffnx_trace("%s x=%d y=%d w=%d h=%d target_x=%d target_y=%d\n", __func__, x, y, w, h, target_x, target_y);

	uint8_t *vram = ff8_vram_seek(x, y), *target = ff8_vram_seek(target_x, target_y);
	const int vramLineWidth = VRAM_DEPTH * VRAM_WIDTH;
	const int lineWidth = VRAM_DEPTH * w;

	for (int i = 0; i < h; ++i) {
		memcpy(target, vram, lineWidth);

		vram += vramLineWidth;
		target += vramLineWidth;
	}

	texturePacker.animateTextureByCopy(x, y, w, h, target_x, target_y);

	ff8_externals.sub_464850(x, y, x + w - 1, h + y - 1);
}

void ff8_copy_vram_part(int16_t *pos_and_size, int target_x, int target_y)
{
	const int16_t x = pos_and_size[0];
	const int16_t y = pos_and_size[1];
	const int16_t w = pos_and_size[2];
	const int16_t h = pos_and_size[3];

	ff8_vram_copy_part(x, y, w, h, target_x, target_y);
}

int read_vram_to_buffer_parent_call1(struc_50 *psxvram, texture_page *tex_page, int x, int y, int w, int h, int bpp, int rel_pos, int a9, uint8_t *target)
{
	if (trace_all || trace_vram) ffnx_trace("%s: x=%d y=%d w=%d h=%d bpp=%d rel_pos=(%d, %d) a9=%d target=%X\n", __func__, x, y, w, h, bpp, rel_pos & 0xF, rel_pos >> 4, a9, target);

	next_psxvram_x = (x >> (2 - bpp)) + ((rel_pos & 0xF) << 6);
	next_psxvram_y = y + (((rel_pos >> 4) & 1) << 8);

	int ret = ff8_externals.sub_464F70(psxvram, tex_page, x, y, w, h, bpp, rel_pos, a9, target);

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
	if (trace_all || trace_vram) ffnx_trace("%s: vram_pos=(%d, %d) target=0x%X target_w=%d w=%d h=%d bpp=%d\n", __func__, next_psxvram_x, next_psxvram_y, int(target), target_w, w, h, bpp);

	if (next_psxvram_x == -1)
	{
		ffnx_warning("%s: cannot detect VRAM position\n", __func__);
	}
	else
	{
		texturePacker.registerTiledTex(target, next_psxvram_x, next_psxvram_y, Tim::Bpp(bpp));
	}

	ff8_externals.read_vram_1(vram, vram_w_2048, target, target_w, w, h, bpp);
}

void get_vram_palette_pos_from_texture_pages(uint16_t &x, uint16_t &y)
{
	int vram_page = (next_psxvram_x / 64) + (next_psxvram_y / 256) * 16;
	uint16_t pos = ff8_externals.psx_texture_pages->struc_50_array[vram_page].vram_palette_pos;

	x = (pos & 0x3F) * 16;
	y = (pos >> 6) & 0x1FF;
}

void read_vram_to_buffer_with_palette1(uint8_t *vram, int vram_w_2048, uint8_t *target, int target_w, int w, int h, int bpp, uint16_t *vram_palette)
{
	if (trace_all || trace_vram) ffnx_trace("%s: vram_pos=(%d, %d) target=0x%X target_w=%d w=%d h=%d bpp=%d vram_palette=%X\n", __func__, next_psxvram_x, next_psxvram_y, int(target), target_w, w, h, bpp, int(vram_palette));

	if (next_psxvram_x == -1)
	{
		ffnx_warning("%s: cannot detect VRAM position\n", __func__);
	}
	else
	{
		uint16_t psxvram_pal_x, psxvram_pal_y;
		get_vram_palette_pos_from_texture_pages(psxvram_pal_x, psxvram_pal_y);
		texturePacker.registerTiledTex(target, next_psxvram_x, next_psxvram_y, Tim::Bpp(bpp), psxvram_pal_x, psxvram_pal_y);
	}

	ff8_externals.read_vram_2_paletted(vram, vram_w_2048, target, target_w, w, h, bpp, vram_palette);
}

void read_vram_to_buffer_with_palette2(uint8_t *vram, uint8_t *target, int w, int h, int bpp, uint16_t *vram_palette)
{
	if (trace_all || trace_vram) ffnx_trace("%s: vram_pos=(%d, %d) target=0x%X w=%d h=%d bpp=%d vram_palette=%X\n", __func__, next_psxvram_x, next_psxvram_y, int(target), w, h, bpp, int(vram_palette));

	if (next_psxvram_x == -1)
	{
		ffnx_warning("%s: cannot detect VRAM position\n", __func__);
	}
	else
	{
		uint16_t psxvram_pal_x, psxvram_pal_y;
		get_vram_palette_pos_from_texture_pages(psxvram_pal_x, psxvram_pal_y);
		texturePacker.registerTiledTex(target, next_psxvram_x, next_psxvram_y, Tim::Bpp(bpp), psxvram_pal_x, psxvram_pal_y);
	}

	ff8_externals.read_vram_3_paletted(vram, target, w, h, bpp, vram_palette);
}

void ff8_read_vram_palette(int CLUT, uint8_t *rgba, int size)
{
	if (trace_all || trace_vram) ffnx_trace("%s: CLUT=(%d, %d) size=%d\n", __func__, (CLUT & 0x3F) * 16, (CLUT >> 6) & 0x1FF, size);

	last_CLUT = CLUT;

	((void(*)(int,uint8_t*,int))ff8_externals.read_vram_palette_sub_467370)(CLUT, rgba, size);
}

int ff8_write_palette_to_driver(int source_offset, int size, uint32_t *source_rgba, int dest_offset, ff8_texture_set *texture_set)
{
	VOBJ(texture_set, texture_set, texture_set);
	VOBJ(tex_header, tex_header, VREF(texture_set, tex_header));

	uint16_t palette_x = (last_CLUT & 0x3F) * 16, palette_y = (last_CLUT >> 6) & 0x1FF;
	int pal_index = dest_offset / size / 2;

	texturePacker.registerPaletteWrite(VREF(tex_header, image_data), pal_index, palette_x, palette_y);

	return ((int(*)(int,int,uint32_t*,int,struct ff8_texture_set*))ff8_externals.write_palette_to_driver_sub_467310)(source_offset, size, source_rgba, dest_offset, texture_set);
}

uint32_t ff8_credits_open_texture(char *fileName, char *buffer)
{
	if (trace_all || trace_vram) ffnx_trace("%s: %s\n", __func__, fileName);

	// {name}.lzs
	strncpy(next_texture_name, strrchr(fileName, '\\') + 1, sizeof(next_texture_name));
	next_bpp = Tim::Bpp16;

	uint32_t ret = ff8_externals.credits_open_file(fileName, buffer);

	if (save_textures) Tim::fromLzsData((uint8_t *)buffer).save(next_texture_name);

	return ret;
}

void ff8_cdcheck_error_upload_vram(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	if (trace_all || trace_vram) ffnx_trace("%s\n", __func__);

	strncpy(next_texture_name, "discerr.lzs", sizeof(next_texture_name));
	next_bpp = Tim::Bpp16;

	if (save_textures) Tim::fromLzsData(texture_buffer - 8).save(next_texture_name);

	ff8_upload_vram(pos_and_size, texture_buffer);
}

void ff8_upload_vram_triple_triad_1(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	if (trace_all || trace_vram) ffnx_trace("%s %p\n", __func__, texture_buffer);

	if (texture_buffer == ff8_externals.cardgame_tim_texture_intro)
	{
		strncpy(next_texture_name, "cardgame/intro", sizeof(next_texture_name));
		next_bpp = Tim::Bpp16;
	}
	else if (texture_buffer == ff8_externals.cardgame_tim_texture_game)
	{
		strncpy(next_texture_name, "cardgame/game", sizeof(next_texture_name));
		next_bpp = Tim::Bpp16;
	}

	if (save_textures && *next_texture_name != '\0')
	{
		ff8_tim tim = ff8_tim();
		tim.img_w = pos_and_size[2];
		tim.img_h = pos_and_size[3];
		tim.img_data = texture_buffer;
		Tim(Tim::Bpp16, tim).save(next_texture_name);
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
			if (save_textures) Tim::fromTimData(texture_buffer - 20).saveMultiPaletteGrid(next_texture_name, 28, 4, 128, 2, true);
		}
		next_bpp = Tim::Bpp8;
	}
	else if (texture_buffer >= ff8_externals.cardgame_tim_texture_icons && texture_buffer < ff8_externals.cardgame_tim_texture_font)
	{
		strncpy(next_texture_name, "cardgame/icons", sizeof(next_texture_name));
		if (next_pal_data == (uint16_t *)texture_buffer)
		{
			if (save_textures) Tim::fromTimData(texture_buffer - 20).save(next_texture_name, 0, 0, true);
		}
		next_bpp = Tim::Bpp4;
	}
	else if (texture_buffer >= ff8_externals.cardgame_tim_texture_font)
	{
		strncpy(next_texture_name, "cardgame/font", sizeof(next_texture_name));
		Tim tim = Tim::fromTimData(texture_buffer - 20);

		palette_infos = TexturePacker::TextureInfos(tim.paletteX(), tim.paletteY(), tim.paletteWidth(), tim.paletteHeight(), Tim::Bpp16);
		texture_infos = TexturePacker::TextureInfos(tim.imageX(), tim.imageY(), tim.imageWidth(), tim.imageHeight(), tim.bpp());

		if (next_pal_data == (uint16_t *)texture_buffer)
		{
			if (save_textures) tim.save(next_texture_name, 0, 0, true);
		}
		next_bpp = tim.bpp();
	}
}

void ff8_upload_vram_triple_triad_2_palette(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	if (trace_all || trace_vram) ffnx_trace("%s %p\n", __func__, texture_buffer);

	next_pal_data = (uint16_t *)texture_buffer;
	ff8_upload_vram_triple_triad_2_texture_name(texture_buffer);
	next_bpp = Tim::Bpp16;

	ff8_upload_vram(pos_and_size, texture_buffer);
}

void ff8_upload_vram_triple_triad_2_data(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	if (trace_all || trace_vram) ffnx_trace("%s %p\n", __func__, texture_buffer);

	ff8_upload_vram_triple_triad_2_texture_name(texture_buffer);

	ff8_upload_vram(pos_and_size, texture_buffer);
}

int search_pos_in_toc(const uint32_t *toc, uint32_t searching_value)
{
	// Find tim id relative to the start of section 38
	for (const uint32_t *cur = toc; *cur != 0; ++cur) {
		if (*cur == searching_value) {
			return int(cur - toc);
		}
	}

	return -1;
}

void ff8_wm_section_17_set_texture(int texture_id)
{
	if (texture_id >= wm_wmset_wave_animations_textures.size())
	{
		return;
	}

	const WmsetSection17Texture &tex = wm_wmset_wave_animations_textures.at(texture_id);
	Tim tim = Tim::fromTimData(tex.textureFramePositions.at(0));
	TexturePacker::TextureInfos palette_infos(tim.paletteX(), tim.paletteY(), tim.paletteWidth(), tim.paletteHeight(), Tim::Bpp16);
	TexturePacker::TextureInfos texture_infos(tex.x, tex.y, tim.imageWidth(), tim.imageHeight(), tim.bpp());

	char texture_name[MAX_PATH] = {};

	snprintf(texture_name, sizeof(texture_name), "world/dat/wmset/section17/texture%d", texture_id);

	texturePacker.setTexture(texture_name, texture_infos, palette_infos, tex.textureFramePositions.size(), true);
}

uint32_t ff8_wm_section_38_prepare_texture_for_upload(uint8_t *tim_file_data, ff8_tim *tim_infos)
{
	uint8_t bpp = tim_file_data[4] & 0x3;
	uint32_t *wm_section_38_textures_pos = *ff8_externals.worldmap_section38_position;
	uint32_t searching_value = uint32_t(tim_file_data - (uint8_t *)wm_section_38_textures_pos);
	int timId = search_pos_in_toc(wm_section_38_textures_pos, searching_value);

	snprintf(next_texture_name, MAX_PATH, "world/dat/wmset/section38/texture%d", timId);

	next_bpp = Tim::Bpp(bpp);

	uint32_t ret = ff8_externals.worldmap_prepare_tim_for_upload(tim_file_data, tim_infos);

	if (timId >= 8)
	{
		texture_infos = TexturePacker::TextureInfos(tim_infos->img_x, tim_infos->img_y, tim_infos->img_w, tim_infos->img_h, next_bpp);
		palette_infos = TexturePacker::TextureInfos(tim_infos->pal_x, tim_infos->pal_y, tim_infos->pal_w, tim_infos->pal_h, Tim::Bpp16);
	}
	else
	{
		texture_infos = TexturePacker::TextureInfos();
		palette_infos = TexturePacker::TextureInfos();
	}

	next_do_not_clear_old_texture = timId >= 16 && timId <= 18 || timId == 20;

	// Open wave animation textures
	if (!*(ff8_externals.config_worldmap_textured_anim_disabled))
	{
		if (timId == 0)
		{
			// Open section 17
			uint8_t *wm_section_17_dword_2040068 = *(uint8_t **)ff8_externals.worldmap_section17_position,
				*wm_section_18_dword_2040638 = *(uint8_t **)ff8_externals.worldmap_section18_position;

			wm_wmset_wave_animations_textures = ff8_world_wmset_wave_animations_parse(wm_section_17_dword_2040068, wm_section_18_dword_2040638 - wm_section_17_dword_2040068);
		}
		else if (timId == 21)
		{
			if (save_textures)
			{
				// Save first texture of section 17, using the palette from section38/texture21
				ff8_world_wmset_wave_animations_save_texture(wm_wmset_wave_animations_textures, 0, "world/dat/wmset/section17", Tim::fromTimData(tim_file_data));
			}
		}
		else if (timId == 22)
		{
			if (save_textures)
			{
				// Save second and third textures of section 17, using the palette from section38/texture22
				Tim tim = Tim::fromTimData(tim_file_data);
				ff8_world_wmset_wave_animations_save_texture(wm_wmset_wave_animations_textures, 1, "world/dat/wmset/section17", tim);
				ff8_world_wmset_wave_animations_save_texture(wm_wmset_wave_animations_textures, 2, "world/dat/wmset/section17", tim);
			}
		}
		else if (timId == 34) // After uploads of section38/texture21 & 22
		{
			for (int i = 0; i < wm_wmset_wave_animations_textures.size(); ++i)
			{
				ff8_wm_section_17_set_texture(i);
			}
		}
	}

	if (!*(ff8_externals.config_worldmap_color_anim_disabled))
	{
		if (timId == 0)
		{
			// Open section 41
			uint8_t *wm_section_41_animation_palettes_dword_203FAF8 = (uint8_t *)*ff8_externals.worldmap_section41_position,
				*wm_section_42_dword_2040690 = (uint8_t *)*ff8_externals.worldmap_section42_position;

			wm_wmset_palette_animations_textures = ff8_world_wmset_palette_animations_parse(wm_section_41_animation_palettes_dword_203FAF8, wm_section_42_dword_2040690 - wm_section_41_animation_palettes_dword_203FAF8);
		}

		if (palette_infos.isValid() && wm_wmset_palette_animations_textures.contains(uint32_t(palette_infos.x()) | (uint32_t(palette_infos.y()) << 16)))
		{
			const WmsetSection41Texture &tex = wm_wmset_palette_animations_textures[uint32_t(palette_infos.x()) | (uint32_t(palette_infos.y()) << 16)];
			if (trace_all || trace_vram) ffnx_trace("%s: set animation palette source=(%d, %d)\n", __func__, tex.srcX, tex.srcY);
			next_texture_count = tex.height;
			TexturePacker::TextureInfos palette(tex.x, tex.y, 256, tex.height, Tim::Bpp16);
			texturePacker.setTexture(nullptr, palette, palette);

			if (save_textures)
			{
				ff8_world_wmset_palette_animations_save_texture(tex, next_texture_name, Tim::fromTimData(tim_file_data));
			}
		}
	}

	if (save_textures)
	{
		if (timId < 8)
		{
			Tim::fromTimData(tim_file_data).saveMultiPaletteGrid(next_texture_name, 8, 4, 0, 4, true);
		}
		else if (palette_infos.h() > 1)
		{
			for (int pal = 0; pal < palette_infos.h(); ++pal) {
				Tim::fromTimData(tim_file_data).save(next_texture_name, 0, pal, true);
			}
		}
		else
		{
			Tim::fromTimData(tim_file_data).save(next_texture_name, 0, 0, true);
		}
	}

	return ret;
}

void ff8_upload_vram_wm_section_38_palette(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	if (trace_all || trace_vram) ffnx_trace("%s %p\n", __func__, texture_buffer);

	next_pal_data = (uint16_t *)texture_buffer;

	ff8_upload_vram(pos_and_size, texture_buffer);

	next_pal_data = nullptr;
}

Tim ff8_wm_set_texture_name_from_section_position(uint8_t section_number, uint32_t *section_toc, uint8_t *tim_file_data)
{
	if (trace_all || trace_vram) ffnx_trace("%s: tim_file_data=%p section%d_pointer=%p\n", __func__, tim_file_data, section_number, section_toc);

	uint32_t searching_value = uint32_t(tim_file_data - (uint8_t *)section_toc);
	int timId = search_pos_in_toc(section_toc, searching_value);

	next_do_not_clear_old_texture = section_number == 40 && timId == 0;

	snprintf(next_texture_name, MAX_PATH, "world/dat/wmset/section%d/texture%d", section_number, timId);

	Tim tim = Tim::fromTimData(tim_file_data);

	next_bpp = tim.bpp();

	texture_infos = TexturePacker::TextureInfos(tim.imageX(), tim.imageY(), tim.imageWidth(), tim.imageHeight(), tim.bpp());
	if (tim.bpp() != Tim::Bpp16) {
		palette_infos = TexturePacker::TextureInfos(tim.paletteX(), tim.paletteY(), tim.paletteWidth(), tim.paletteHeight(), Tim::Bpp16);
		next_pal_data = tim.paletteData();
	}

	return tim;
}

void ff8_wm_section_17_upload(uint8_t *tim_file_data, int16_t x, int16_t y)
{
	if (trace_all || trace_vram) ffnx_trace("%s: pos=(%d, %d)\n", __func__, x, y);

	Tim tim = Tim::fromTimData(tim_file_data);

	next_bpp = tim.bpp();
	palette_infos = TexturePacker::TextureInfos(); // Invalid palette
	texture_infos = TexturePacker::TextureInfos(x, y, tim.imageWidth(), tim.imageHeight(), tim.bpp());

	int texture_id = 0;
	for (const WmsetSection17Texture &texture: wm_wmset_wave_animations_textures) {
		int frame_id = 0;
		for (const uint8_t *frameTexturePos: texture.textureFramePositions) {
			if (frameTexturePos == tim_file_data) {
				texturePacker.forceCurrentPalette(x, y, frame_id);

				((void(*)(uint8_t*,int16_t,int16_t))ff8_externals.sub_541AE0)(tim_file_data, x, y);

				return;
			}
			++frame_id;
		}

		++texture_id;
	}

	((void(*)(uint8_t*,int16_t,int16_t))ff8_externals.sub_541AE0)(tim_file_data, x, y);
}

void ff8_wm_section_39_upload(uint8_t *tim_file_data)
{
	Tim tim = ff8_wm_set_texture_name_from_section_position(39, *ff8_externals.worldmap_section39_position, tim_file_data);

	if (save_textures) {
		tim.save(next_texture_name, 0, 0, true);
	}

	((void(*)(uint8_t*))ff8_externals.worldmap_sub_541970_upload_tim)(tim_file_data);

	next_pal_data = nullptr;
}

void ff8_wm_section_40_upload(uint8_t *tim_file_data)
{
	Tim tim = ff8_wm_set_texture_name_from_section_position(40, *ff8_externals.worldmap_section40_position, tim_file_data);

	if (save_textures) {
		tim.saveMultiPaletteGrid(next_texture_name, 8, 4, 0, 4, true);
	}

	((void(*)(uint8_t*))ff8_externals.worldmap_sub_541970_upload_tim)(tim_file_data);

	next_pal_data = nullptr;
}

void ff8_wm_section_41_upload_palette(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	next_pal_data = (uint16_t *)texture_buffer;

	ff8_upload_vram(pos_and_size, texture_buffer);

	next_pal_data = nullptr;
}

void ff8_wm_section_42_upload(uint8_t *tim_file_data)
{
	Tim tim = ff8_wm_set_texture_name_from_section_position(42, *ff8_externals.worldmap_section42_position, tim_file_data);

	if (save_textures) {
		tim.save(next_texture_name, 0, 0, true);
	}

	((void(*)(uint8_t*))ff8_externals.worldmap_sub_541970_upload_tim)(tim_file_data);

	next_pal_data = nullptr;
}

int ff8_wm_chara_one_read_file(int fd, uint8_t *data, size_t size)
{
	if (trace_all || trace_vram) ffnx_trace("%s\n", __func__);

	int read = ((int(*)(int, uint8_t *, size_t))ff8_externals.chara_one_read_file)(fd, data, size);

	chara_one_world_texture_offsets = ff8_world_chara_one_parse_models(data, read);
	chara_one_world_data = data;

	if (save_textures) {
		char filename[MAX_PATH];
		snprintf(filename, sizeof(filename), "world/esk/chara_one/");
		ff8_world_chara_one_model_save_textures(chara_one_world_texture_offsets, data, filename);
	}

	return read;
}

int ff8_wm_chara_one_upload_texture_2(char *image_buffer, char bpp, char a3, int x, int16_t y, int w, int16_t h)
{
	if (trace_all || trace_vram) ffnx_trace("%s\n", __func__);

	int chara_one_offset = int(*(ff8_externals.chara_one_data_start) - chara_one_world_data);
	int model_id = 0;
	for (const CharaOneModelTextures &textures: chara_one_world_texture_offsets) {
		int texture_id = 0;
		for (const uint32_t texture_offset: textures) {
			if (texture_offset == chara_one_offset) {
				next_bpp = Tim::Bpp(bpp);
				snprintf(next_texture_name, MAX_PATH, "world/esk/chara_one/model%d-%d", model_id, texture_id);

				return ((int(*)(char *, char, char, int, __int16, int, __int16))ff8_externals.chara_one_upload_texture)(image_buffer, bpp, a3, x, y, w, h);
			}
			++texture_id;
		}

		++model_id;
	}

	return ((int(*)(char *, char, char, int, __int16, int, __int16))ff8_externals.chara_one_upload_texture)(image_buffer, bpp, a3, x, y, w, h);
}

void ff8_wm_chara_one_upload_palette_2(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	next_pal_data = (uint16_t *)texture_buffer;

	ff8_upload_vram(pos_and_size, texture_buffer);

	next_pal_data = nullptr;
}

int ff8_wm_open_data(const char *path, int32_t pos, uint32_t size, void *data)
{
	if (strstr(path, "texl.obj") != nullptr)
	{
		next_texl_id = pos / 0x12800;

		if (trace_all || trace_vram) ffnx_trace("Next texl ID: %d\n", next_texl_id);
	}

	return ff8_externals.open_file_world(path, pos, size, data);
}

void ff8_wm_texl_palette_upload_vram(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	Tim tim = Tim::fromTimData(texture_buffer - 20);

	if (trace_all || trace_vram) ffnx_trace("%s texl_id=%d pos=(%d, %d) palPos=(%d, %d)\n", __func__, next_texl_id, tim.imageX(), tim.imageY(), tim.paletteX(), tim.paletteY());

	next_bpp = Tim::Bpp8;

	ff8_upload_vram(pos_and_size, texture_buffer);

	// Worldmap texture fix

	uint16_t oldX = 16 * (next_texl_id - 2 * ((next_texl_id / 2) & 1) + (next_texl_id & 1)), oldY = ((next_texl_id / 2) & 1) ? 384 : 256;

	if (next_texl_id == 18 || next_texl_id == 19)
	{
		oldX = next_texl_id & 1 ? 96 : 64;
		oldY = 384;
	}

	uint16_t newX = 0;

	if (pos_and_size[0] == 320 && pos_and_size[1] == 224)
	{
		newX = 256;
	}
	else if (pos_and_size[0] == 320 && pos_and_size[1] == 240)
	{
		newX = 384;
	}
	else if (pos_and_size[0] == 576 && pos_and_size[1] == 224)
	{
		newX = 512;
	}
	else if (pos_and_size[0] == 576 && pos_and_size[1] == 240)
	{
		newX = 640;
	}

	if (next_texl_id == 16 || next_texl_id == 17 || next_texl_id > 19)
	{
		if (trace_all || trace_vram) ffnx_warning("%s: texl id not supported %d\n", __func__, next_texl_id);
		return; // TODO
	}

	TexturePacker::TextureInfos oldTexture(oldX, oldY, tim.imageWidth() / 4, tim.imageHeight() / 2, Tim::Bpp4),
		newTexture(newX, 256, tim.imageWidth(), tim.imageHeight(), Tim::Bpp(tim.bpp()));

	// Allow to mod via texl textures
	snprintf(next_texture_name, MAX_PATH, "world/dat/texl/texture%d", next_texl_id);

	if (save_textures)
	{
		tim.saveMultiPaletteGrid(next_texture_name, 4, 4, 0, 4, true);
	}

	if (! ff8_worldmap_internal_highres_textures)
	{
		return;
	}

	// Redirect internal texl textures
	uint32_t image_data_size = newTexture.pixelW() * newTexture.h() * 4;
	uint32_t *image = (uint32_t*)driver_malloc(image_data_size);

	if (image)
	{
		if (! tim.toRGBA32MultiPaletteGrid(image, 4, 4, 0, 4, true))
		{
			driver_free(image);
			return;
		}

		if (! texturePacker.setTextureRedirection(next_texture_name, oldTexture, newTexture, image))
		{
			driver_free(image);
		}
	}

	*next_texture_name = '\0';

	int section = 16 + oldX / 64;
	// Reload texture
	struc_50 *stru50 = ff8_externals.psx_texture_pages[0].struc_50_array + section;

	for (int page = 0; page < 8; ++page) {
		if ((stru50->initialized >> page) & 1) {
			ff8_texture_set *texture_set = (struct ff8_texture_set *)stru50->texture_page[page].tri_gfxobj->hundred_data->texture_set;

			common_unload_texture((struct texture_set *)texture_set);
			common_load_texture((struct texture_set *)texture_set, texture_set->tex_header, texture_set->texture_format);
		}
	}
}

void ff8_field_mim_palette_upload_vram(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	if (trace_all || trace_vram) ffnx_trace("%s\n", __func__);

	mim_texture_buffer = texture_buffer;

	ff8_upload_vram(pos_and_size, texture_buffer);
}

uint32_t ff8_field_read_map_data(char *filename, uint8_t *map_data)
{
	if (trace_all || trace_vram) ffnx_trace("%s %s\n", __func__, filename);

	uint32_t ret = ff8_externals.sm_pc_read(filename, map_data);

	char tex_directory[MAX_PATH] = {}, tex_filename[MAX_PATH] = {};

	snprintf(tex_directory, sizeof(tex_directory), "field/mapdata/%s", get_current_field_name());
	snprintf(tex_filename, sizeof(tex_filename), "%s/%s", tex_directory, get_current_field_name());

	if (save_textures_legacy) {
		ff8_background_save_textures_legacy(ff8_background_parse_tiles(map_data), mim_texture_buffer, tex_filename);

		return ret;
	} else if (save_textures) {
		ff8_background_save_textures(ff8_background_parse_tiles(map_data), mim_texture_buffer, tex_filename);

		return ret;
	}

	char tex_abs_directory[MAX_PATH] = {};
	snprintf(tex_abs_directory, sizeof(tex_abs_directory), "%s/%s", mod_path.c_str(), tex_directory);
	bool has_dir = dirExists(tex_abs_directory);

	if (!has_dir && (trace_all || trace_loaders || trace_vram)) {
		ffnx_warning("Directory does not exist, fallback to Tonberry compatibility layer: %s\n", tex_abs_directory);
	}

	std::vector<Tile> tiles;

	if (has_dir) {
		tiles = ff8_background_parse_tiles(map_data);
	}

	if (!has_dir || !texturePacker.setTextureBackground(tex_filename, 0, 256, VRAM_PAGE_MIM_MAX_COUNT * TEXTURE_WIDTH_BPP16, TEXTURE_HEIGHT, tiles)) {
		snprintf(tex_directory, MAX_PATH, "field/mapdata/%.2s/%s", get_current_field_name(), get_current_field_name());
		snprintf(tex_abs_directory, sizeof(tex_abs_directory), "%s/%s", mod_path.c_str(), tex_directory);

		if (!dirExists(tex_abs_directory)) {
			if (trace_all || trace_loaders || trace_vram) {
				ffnx_warning("Directory does not exist, abort looking for field textures: %s\n", tex_abs_directory);
			}

			return ret;
		}

		if (tiles.empty()) {
			tiles = ff8_background_parse_tiles(map_data);
		}

		char found_extension[MAX_PATH] = {};
		char *extension = nullptr;
		bool found_pages[VRAM_PAGE_MIM_MAX_COUNT] = {};

		snprintf(tex_directory, MAX_PATH, "%s/%s", tex_directory, get_current_field_name());

		// Compatibility with Tonberry, vram pages 13-25
		for (int i = 0; i < VRAM_PAGE_MIM_MAX_COUNT; ++i) {
			snprintf(tex_filename, MAX_PATH, "%s_%d", tex_directory, i + VRAM_PAGE_MIM_MAX_COUNT);

			if (texturePacker.setTextureBackground(tex_filename, i * TEXTURE_WIDTH_BPP16, 256, TEXTURE_WIDTH_BPP16, TEXTURE_HEIGHT, tiles, i, extension, found_extension)) {
				extension = found_extension;
				found_pages[i] = true;
			}
		}

		// Compatibility with Tonberry, vram pages 0-12
		for (int i = 0; i < VRAM_PAGE_MIM_MAX_COUNT; ++i) {
			snprintf(tex_filename, MAX_PATH, "%s_%d", tex_directory, i);

			if (!found_pages[i] && !texturePacker.setTextureBackground(tex_filename, i * TEXTURE_WIDTH_BPP16, 256, TEXTURE_WIDTH_BPP16, TEXTURE_HEIGHT, tiles, i, extension, found_extension)) {
				break;
			}

			extension = found_extension;
		}
	}

	return ret;
}

int ff8_field_chara_one_read_file_header(int fd, uint8_t *const data, size_t size)
{
	int read = ((int(*)(int,uint8_t*const,size_t))ff8_externals.chara_one_read_file)(fd, data, size);

	if (trace_all || trace_vram) ffnx_trace("%s: size=%d\n", __func__, size);

	chara_one_models = ff8_chara_one_parse_models(data, size);
	chara_one_loaded_models.clear();
	chara_one_current_model = 0;
	chara_one_current_mch = 0;
	chara_one_current_texture = 0;

	return read;
}

int ff8_field_chara_one_seek_to_model(int fd, int pos, int whence)
{
	if (trace_all || trace_vram) ffnx_trace("%s: pos=0x%X whence=%d\n", __func__, pos, whence);

	chara_one_current_pos = pos;

	return ((int(*)(int,int,int))ff8_externals.chara_one_seek_file)(fd, pos, whence);
}

int ff8_field_chara_one_read_model(int fd, uint8_t *const data, size_t size)
{
	int read = ((int(*)(int,uint8_t*const,size_t))ff8_externals.chara_one_read_file)(fd, data, size);

	if (trace_all || trace_vram) ffnx_trace("%s: size=%d\n", __func__, size);

	if (chara_one_models.contains(chara_one_current_pos)) {
		if (save_textures) {
			char filename[MAX_PATH];
			snprintf(filename, sizeof(filename), "field/model/second_chr");
			ff8_chara_one_model_save_textures(chara_one_models[chara_one_current_pos], data, filename);
		}

		chara_one_loaded_models.push_back(chara_one_current_pos);
	}

	return read;
}

int ff8_field_chara_one_read_mch(int fd, uint8_t *const data, size_t size)
{
	int read = ((int(*)(int,uint8_t*const,size_t))ff8_externals.chara_one_read_file)(fd, data, size);

	if (trace_all || trace_vram) ffnx_trace("%s: size=%d\n", __func__, size);

	int id = 0;
	for (uint32_t addr: chara_one_loaded_models) {
		if (chara_one_models[addr].isMch) {
			if (id == chara_one_current_mch) {
				ff8_mch_parse_model(chara_one_models[addr], data, size);
				if (save_textures) {
					char filename[MAX_PATH];
					snprintf(filename, sizeof(filename), "field/model/main_chr");
					ff8_chara_one_model_save_textures(chara_one_models[addr], data, filename);
				}

				break;
			}
			++id;
		}
	}

	++chara_one_current_mch;

	return read;
}

int ff8_field_texture_upload_one(char *image_buffer, char bpp, char a3, int x, int16_t y, int w, int16_t h)
{
	if (trace_all || trace_vram) ffnx_trace("%s bpp=%d a3=%d image_buffer=0x%X\n", __func__, bpp, a3, image_buffer);

	while (chara_one_current_model < chara_one_loaded_models.size()) {
		CharaOneModel model = chara_one_models[chara_one_loaded_models.at(chara_one_current_model)];

		if (chara_one_current_texture < model.texturesData.size()) {
			next_bpp = Tim::Bpp(bpp);
			snprintf(next_texture_name, MAX_PATH, "field/model/%s_chr/%s-%d", model.isMch ? "main" : "second", model.name, chara_one_current_texture);

			++chara_one_current_texture;
			break;
		}

		++chara_one_current_model;
		chara_one_current_texture = 0;
	}

	return ((int(*)(char*,char,char,int,int16_t,int,int16_t))ff8_externals.chara_one_upload_texture)(image_buffer, bpp, a3, x, y, w, h);
}

void ff8_field_effects_upload_vram1(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	Tim::Bpp bpp = Tim::Bpp4;
	char texture_name[MAX_PATH];

	snprintf(texture_name, sizeof(texture_name), "field/mapdata/%s/%s_pmp", get_current_field_name(), get_current_field_name());

	if (save_textures) {
		ff8_tim t = ff8_tim();
		t.pal_data = (uint16_t*)(texture_buffer - 512);
		t.pal_w = 256;
		t.pal_h = 1;
		t.img_data = texture_buffer;
		t.img_w = pos_and_size[2];
		t.img_h = pos_and_size[3] * 2; // This upload and the next one together
		Tim(bpp, t).save(texture_name);
	}

	ff8_upload_vram(pos_and_size, texture_buffer);

	// This upload and the next one together
	texturePacker.setTexture(texture_name, TexturePacker::TextureInfos(pos_and_size[0], pos_and_size[1], pos_and_size[2], pos_and_size[3] * 2, bpp));
}

int16_t ff8_battle_open_and_read_file(int fileId, void *data, int a3, int callback)
{
	if (trace_all || trace_vram) ffnx_trace("%s: %d\n", __func__, fileId);

	snprintf(battle_texture_name, sizeof(battle_texture_name), "battle/%s", ff8_externals.battle_filenames[fileId]);

	return ((int16_t(*)(int,void*,int,int))ff8_externals.battle_open_file)(fileId, data, a3, callback);
}

size_t ff8_battle_read_file(char *fileName, void *data)
{
	if (trace_all || trace_vram) ffnx_trace("%s: %s\n", __func__, fileName);

	battle_texture_id = 0;

	size_t file_size = ff8_externals.sm_pc_read(fileName, data);

	if (save_textures && StrStrIA(fileName, ".X") != nullptr) {
		ff8_battle_stage_parse_geometry((uint8_t *)data, file_size, stage);
	}

	return file_size;
}

void ff8_battle_upload_texture_palette(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	if (trace_all || trace_vram) ffnx_trace("%s: %s\n", __func__, battle_texture_name);

	Tim tim = Tim::fromTimData(texture_buffer - 20);

	ff8_upload_vram(pos_and_size, texture_buffer);

	next_bpp = tim.bpp();
	snprintf(next_texture_name, sizeof(next_texture_name), "%s-%d", battle_texture_name, battle_texture_id);

	++battle_texture_id;

	if (save_textures) {
		if (StrStrIA(battle_texture_name, ".X") != nullptr) {
			ff8_battle_state_save_texture(stage, tim, next_texture_name);
		} else {
			tim.save(next_texture_name, 0, 0, true);
		}
	}
}

void engine_set_init_time(double fps_adjust)
{
	texturePacker.clearTextures();

	((void(*)(double))ff8_externals.engine_set_init_time)(fps_adjust);
}

void clean_psxvram_pages()
{
	texturePacker.clearTiledTexs();

	((void(*)())ff8_externals.sub_4672C0)();
}

void vram_init()
{
	//---- Texture uploads identification

	replace_function(ff8_externals.upload_psx_vram, ff8_upload_vram);
	replace_function(ff8_externals.copy_psx_vram_part, ff8_copy_vram_part);

	// pubintro
	replace_call(ff8_externals.open_lzs_image + 0x72, ff8_credits_open_texture);
	// cdcheck
	replace_call(ff8_externals.cdcheck_sub_52F9E0 + 0x1DC, ff8_cdcheck_error_upload_vram);
	// Triple Triad
	replace_call(ff8_externals.sub_5391B0 + 0x49, ff8_upload_vram_triple_triad_1);
	replace_call(ff8_externals.sub_5391B0 + 0x1CC, ff8_upload_vram_triple_triad_2_palette);
	replace_call(ff8_externals.sub_5391B0 + 0x1E1, ff8_upload_vram_triple_triad_2_data);
	// worldmap
	replace_call(ff8_externals.sub_554940_call_130, ff8_wm_section_17_upload); // Waves
	replace_call(ff8_externals.worldmap_sub_53F310_call_2A9, ff8_wm_section_38_prepare_texture_for_upload);
	replace_call(ff8_externals.worldmap_sub_53F310_call_30D, ff8_upload_vram_wm_section_38_palette);
	replace_call(ff8_externals.worldmap_sub_53F310_call_330, ff8_wm_section_39_upload); // Rails/Roads
	replace_call(ff8_externals.worldmap_sub_53F310_call_366, ff8_wm_section_40_upload);
	replace_call(ff8_externals.worldmap_sub_554AA0_call_C2, ff8_wm_section_41_upload_palette); // Animated palettes
	replace_call(ff8_externals.worldmap_sub_548020 + 0x47, ff8_wm_section_42_upload); // Train/vehicles
	replace_call(ff8_externals.worldmap_chara_one + 0xCC, ff8_wm_chara_one_read_file);
	replace_call(ff8_externals.worldmap_chara_one + 0x4D1, ff8_wm_chara_one_upload_texture_2); // Characters/Chocobos/Ragnarok
	replace_call(ff8_externals.worldmap_chara_one + 0x566, ff8_wm_chara_one_upload_palette_2);
	// wm texl project
	replace_call(ff8_externals.upload_psxvram_texl_pal_call1, ff8_wm_texl_palette_upload_vram);
	replace_call(ff8_externals.upload_psxvram_texl_pal_call2, ff8_wm_texl_palette_upload_vram);
	replace_call(ff8_externals.open_file_world_sub_52D670_texl_call1, ff8_wm_open_data);
	replace_call(ff8_externals.open_file_world_sub_52D670_texl_call2, ff8_wm_open_data);
	// field: background
	replace_call(ff8_externals.upload_mim_file + 0x2E, ff8_field_mim_palette_upload_vram);
	replace_call(ff8_externals.read_field_data + (JP_VERSION ? 0x990 : 0x915), ff8_field_read_map_data);
	// field: characters
	replace_call(ff8_externals.load_field_models + 0x15F, ff8_field_chara_one_read_file_header);
	replace_call(ff8_externals.load_field_models + 0x582, ff8_field_chara_one_seek_to_model);
	replace_call(ff8_externals.load_field_models + 0x594, ff8_field_chara_one_read_model);
	replace_call(ff8_externals.load_field_models + 0x879, ff8_field_chara_one_read_mch);
	replace_call(ff8_externals.load_field_models + 0xB72, ff8_field_texture_upload_one);
	// field: effects
	replace_call(ff8_externals.upload_pmp_file + 0x7F, ff8_field_effects_upload_vram1);
	// battle
	replace_call(ff8_externals.battle_open_file_wrapper + 0x14, ff8_battle_open_and_read_file);
	replace_call(ff8_externals.battle_open_file + 0x8E, ff8_battle_read_file);
	replace_call(ff8_externals.battle_open_file + 0x1A2, ff8_battle_read_file);
	replace_call(ff8_externals.battle_upload_texture_to_vram + 0x45, ff8_battle_upload_texture_palette);

	//---- VRAM reads

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

	// Read palette from VRAM to Graphic driver
	replace_call(ff8_externals.write_palette_texture_set_sub_466190 + 0x2C, ff8_read_vram_palette);
	replace_call(ff8_externals.write_palette_texture_set_sub_466190 + 0x7E, ff8_write_palette_to_driver);

	//---- Misc

	// Fix missing textures in battle module by clearing custom textures
	replace_call(ff8_externals.battle_enter + 0x35, engine_set_init_time);
	// Clear texture_packer on every module exits
	replace_call(ff8_externals.psxvram_texture_pages_free + 0x5A, clean_psxvram_pages);
}
