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
#include "../image/tim.h"
#include "field/background.h"
#include "field/chara_one.h"

#include <unordered_map>
#include <vector>

TexturePacker texturePacker;

char next_texture_name[MAX_PATH] = "";
uint16_t *next_pal_data = nullptr;
int next_psxvram_x = -1;
int next_psxvram_y = -1;
int next_psxvram_pal_x = -1;
int next_psxvram_pal_y = -1;
int next_texl_id = 0;
Tim::Bpp next_bpp = Tim::Bpp16;
uint8_t next_scale = 1;
int8_t texl_id_left = -1;
int8_t texl_id_right = -1;
// Field background
uint8_t *mim_texture_buffer = nullptr;
// Field models
std::unordered_map<uint32_t, CharaOneModel> chara_one_models;
std::vector<uint32_t> chara_one_loaded_models;
int chara_one_current_pos = 0;
uint32_t chara_one_current_model = 0;
uint32_t chara_one_current_mch = 0;
uint32_t chara_one_current_texture = 0;

void ff8_upload_vram(int16_t *pos_and_size, uint8_t *texture_buffer)
{
	const int x = pos_and_size[0];
	const int y = pos_and_size[1];
	const int w = pos_and_size[2];
	const int h = pos_and_size[3];
	bool isPal = next_pal_data != nullptr && (uint8_t *)next_pal_data == texture_buffer;

	if (trace_all || trace_vram) ffnx_trace("%s x=%d y=%d w=%d h=%d bpp=%d isPal=%d texture_buffer=0x%X\n", __func__, x, y, w, h, next_bpp, isPal, texture_buffer);

	texturePacker.uploadTexture(texture_buffer, x, y, w, h);
	texturePacker.setTexture(next_texture_name, x, y, w, h, next_bpp, isPal);

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

void read_vram_to_buffer_with_palette1(uint8_t *vram, int vram_w_2048, uint8_t *target, int target_w, int w, int h, int bpp, uint16_t *vram_palette)
{
	if (trace_all || trace_vram) ffnx_trace("%s: vram_pos=(%d, %d) target=0x%X target_w=%d w=%d h=%d bpp=%d vram_palette=%X\n", __func__, next_psxvram_x, next_psxvram_y, int(target), target_w, w, h, bpp, int(vram_palette));

	if (next_psxvram_x == -1)
	{
		ffnx_warning("%s: cannot detect VRAM position\n", __func__);
	}
	else
	{
		texturePacker.registerTiledTex(target, next_psxvram_x, next_psxvram_y, Tim::Bpp(bpp), next_psxvram_pal_x, next_psxvram_pal_y);
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
		texturePacker.registerTiledTex(target, next_psxvram_x, next_psxvram_y, Tim::Bpp(bpp), next_psxvram_pal_x, next_psxvram_pal_y);
	}

	ff8_externals.read_vram_3_paletted(vram, target, w, h, bpp, vram_palette);
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

uint32_t ff8_wm_section_38_prepare_texture_for_upload(uint8_t *tim_file_data, ff8_tim *tim_infos)
{
	uint8_t bpp = tim_file_data[4] & 0x3;
	uint32_t *wm_section_38_textures_pos = *ff8_externals.worldmap_section38_position;
	uint32_t searching_value = uint32_t(tim_file_data - (uint8_t *)wm_section_38_textures_pos);
	int timId = -1;

	// Find tim id relative to the start of section 38
	for (uint32_t *cur = wm_section_38_textures_pos; *cur != 0; ++cur) {
		if (*cur == searching_value) {
			timId = int(cur - wm_section_38_textures_pos);
			break;
		}
	}

	snprintf(next_texture_name, MAX_PATH, "world/dat/wmset/section38/texture%d", timId);

	next_bpp = Tim::Bpp(bpp);

	uint32_t ret = ff8_externals.worldmap_prepare_tim_for_upload(tim_file_data, tim_infos);

	next_pal_data = tim_infos->pal_data;

	if (save_textures)
	{
		if (timId < 8)
		{
			Tim::fromTimData(tim_file_data).saveMultiPaletteGrid(next_texture_name, 8, 4, 0, 4, true);
		}
		else
		{
			Tim::fromTimData(tim_file_data).save(next_texture_name);
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
	snprintf(next_texture_name, MAX_PATH, "world/dat/texl/texture%d", next_texl_id);

	Tim tim = Tim::fromTimData(texture_buffer - 20);

	if (trace_all || trace_vram) ffnx_trace("%s texl_id=%d pos=(%d, %d) palPos=(%d, %d)\n", __func__, next_texl_id, tim.imageX(), tim.imageY(), tim.paletteX(), tim.paletteY());

	if (save_textures) tim.saveMultiPaletteGrid(next_texture_name, 4, 4, 0, 4, true);

	next_bpp = Tim::Bpp8;

	ff8_upload_vram(pos_and_size, texture_buffer);

	if (! ff8_worldmap_internal_highres_textures)
	{
		return;
	}

	// Worldmap texture fix

	bool is_left = pos_and_size[1] == 224;
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

	TexturePacker::TextureInfos oldTexture(oldX, oldY, tim.imageWidth() / 8, tim.imageHeight() / 2, Tim::Bpp4),
		newTexture(newX, 256, tim.imageWidth() / 2, tim.imageHeight(), Tim::Bpp(tim.bpp()));

	uint32_t image_data_size = newTexture.pixelW() * newTexture.h() * 4;
	uint32_t *image = (uint32_t*)driver_malloc(image_data_size);

	if (image)
	{
		if (! tim.toRGBA32MultiPaletteGrid(image, 4, 4, 0, 4, true))
		{
			driver_free(image);
			return;
		}

		if (! texturePacker.setTextureRedirection(oldTexture, newTexture, image))
		{
			if (trace_all || trace_vram) ffnx_warning("%s: invalid redirection\n");
			driver_free(image);
		}
	}

	// Reload texture TODO: reload only relevant parts
	ff8_externals.psx_texture_pages[0].struc_50_array[16].vram_needs_reload = 0xFF;
	ff8_externals.psx_texture_pages[0].struc_50_array[17].vram_needs_reload = 0xFF;
	ff8_externals.psx_texture_pages[0].struc_50_array[18].vram_needs_reload = 0xFF;
	ff8_externals.psx_texture_pages[0].struc_50_array[19].vram_needs_reload = 0xFF;
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

	char tex_filename[MAX_PATH] = {};

	snprintf(tex_filename, MAX_PATH, "field/mapdata/%s/%s", get_current_field_name(), get_current_field_name());

	std::vector<Tile> tiles = ff8_background_parse_tiles(map_data);

	if (save_textures) {
		ff8_background_save_textures(tiles, mim_texture_buffer, tex_filename);
	}

	texturePacker.setTextureBackground(tex_filename, 0, 256, VRAM_PAGE_MIM_MAX_COUNT * TEXTURE_WIDTH_BPP16, TEXTURE_HEIGHT, tiles);

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

DWORD *create_graphics_object_load_texture_call2(int a1, int a2, char *path, void *data, ff8_game_obj *game_object)
{
	if (trace_all || trace_vram) ffnx_trace("%s path=%s\n", __func__, path == nullptr ? "(none)" : path);

	// Optimization: when the game creates 3D objects for field background, it does upload textures, but it does not display it
	texturePacker.disableDrawTexturesBackground(true);

	DWORD *ret = ((DWORD*(*)(int,int,char*,void*,ff8_game_obj*))ff8_externals.sub_4076B6)(a1, a2, path, data, game_object);

	texturePacker.disableDrawTexturesBackground(false);

	return ret;
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
	replace_call(ff8_externals.worldmap_sub_53F310_call_2A9, ff8_wm_section_38_prepare_texture_for_upload);
	replace_call(ff8_externals.worldmap_sub_53F310_call_30D, ff8_upload_vram_wm_section_38_palette);
	// wm texl project
	replace_call(ff8_externals.upload_psxvram_texl_pal_call1, ff8_wm_texl_palette_upload_vram);
	replace_call(ff8_externals.upload_psxvram_texl_pal_call2, ff8_wm_texl_palette_upload_vram);
	replace_call(ff8_externals.open_file_world_sub_52D670_texl_call1, ff8_wm_open_data);
	replace_call(ff8_externals.open_file_world_sub_52D670_texl_call2, ff8_wm_open_data);
	// field: mim/map
	replace_call(ff8_externals.upload_mim_file + 0x2E, ff8_field_mim_palette_upload_vram);
	replace_call(ff8_externals.read_field_data + (JP_VERSION ? 0x990 : 0x915), ff8_field_read_map_data);
	// field: chara.one
	replace_call(ff8_externals.load_field_models + 0x15F, ff8_field_chara_one_read_file_header);
	replace_call(ff8_externals.load_field_models + 0x582, ff8_field_chara_one_seek_to_model);
	replace_call(ff8_externals.load_field_models + 0x594, ff8_field_chara_one_read_model);
	replace_call(ff8_externals.load_field_models + 0x879, ff8_field_chara_one_read_mch);
	replace_call(ff8_externals.load_field_models + 0xB72, ff8_field_texture_upload_one);

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

	replace_call(ff8_externals._load_texture + 0x24E, create_graphics_object_load_texture_call2);

	// Not used?
	replace_call(ff8_externals.sub_4649A0 + 0x13F, read_vram_to_buffer_with_palette2);
}
