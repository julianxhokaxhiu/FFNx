/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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

/*
 * This file contains the changes necessary to support subtractive and 25%
 * blending modes in field backgrounds. Texture pages where these blending
 * modes are used are duplicated and the tile data modified to point to these
 * new pages which have the correct blending mode set.
 */

#include "../ff7.h"
#include "../log.h"
#include "../field.h"
#include "../patch.h"
#include "../sfx.h"
#include "../movies.h"
#include "defs.h"
#include <set>
#include <cmath>

// model movement and animations
constexpr byte CANIM1 = 0xB0;
constexpr byte CANM1 = 0xB1;
constexpr byte CANIM2 = 0xBB;
constexpr byte CANM2 = 0xBC;
constexpr byte PTURA = 0x35;
constexpr byte TURA = 0xAB;
constexpr byte TURNGEN = 0xB4;
constexpr byte TURN = 0xB5;
constexpr byte JUMP = 0xC0;

// background opcodes
constexpr byte BGSCR = 0x2D;

// camera movement and others
constexpr byte NFADE = 0x25;
constexpr byte FADE = 0x6B;
constexpr byte SCRLC = 0x62;
constexpr byte SCRLA = 0x63;
constexpr byte SCR2DC = 0x66;
constexpr byte SCR2DL = 0x68;
constexpr byte SCRLP = 0x6F;
constexpr byte VWOTF = 0x6A;

struct external_field_model_data
{
	int moveFrameIndex;
	vector3<int> initialPosition;
	vector3<int> finalPosition;
	int wasNotCollidingWithTarget;
	int updateMovementReturnValue;

	int rotationMoveFrameIndex;
};

struct field_bank_address
{
	byte bank;
	byte address;

	uint32_t getID() const
	{
		return bank * 256 + address;
	}

	bool operator< (const field_bank_address& f) const
	{
		return this->getID() < f.getID();
	}

	bool operator== (const field_bank_address& f) const
	{
		return this->getID() == f.getID();
	}
};

constexpr int MAX_FIELD_MODELS = 32;
std::array<external_field_model_data, MAX_FIELD_MODELS> external_model_data;
std::array<uint32_t, 256> original_opcode_table {0};
std::set<field_bank_address> field_bank_address_to_be_fixed = {{14, 6}};
field_bank_address mvief_bank_address;

constexpr float INVALID_VALUE = -1000000;
vector2<float> field_3d_world_pos = {INVALID_VALUE, INVALID_VALUE};
vector2<float> bg_main_layer_pos = {INVALID_VALUE, INVALID_VALUE};
vector2<float> bg_layer3_pos = {INVALID_VALUE, INVALID_VALUE};
vector2<float> bg_layer4_pos = {INVALID_VALUE, INVALID_VALUE};

int call_original_opcode_function(byte opcode)
{
	if(original_opcode_table[opcode])
		return ((int(*)())original_opcode_table[opcode])();
	else
	{
		ffnx_error("Initialization error: original opcode table empty in position %d\n", opcode);
		return 0;
	}
}

// helper function initializes page dst, copies texture from src and applies
// blend_mode
void field_load_textures_helper(struct ff7_game_obj *game_object, struct struc_3 *struc_3, uint32_t src, uint32_t dst, uint32_t blend_mode)
{
	struct ff7_tex_header *tex_header;

	ff7_externals.make_struc3(blend_mode, struc_3);

	tex_header = (struct ff7_tex_header *)common_externals.create_tex_header();

	ff7_externals.field_layers[dst]->tex_header = tex_header;

	if(ff7_externals.field_layers[src]->type == 1) ff7_externals.make_field_tex_header_pal(tex_header);
	if(ff7_externals.field_layers[src]->type == 2) ff7_externals.make_field_tex_header(tex_header);

	struc_3->tex_header = tex_header;

	if(src != dst)
	{
		ff7_externals.field_layers[dst]->image_data = external_malloc(256 * 256);
		memcpy(ff7_externals.field_layers[dst]->image_data, ff7_externals.field_layers[src]->image_data, 256 * 256);
	}

	tex_header->image_data = (unsigned char*)ff7_externals.field_layers[dst]->image_data;

	tex_header->file.pc_name = (char*)external_malloc(1024);
	sprintf(tex_header->file.pc_name, "field/%s/%s_%02i", strchr(ff7_externals.field_file_name, '\\') + 1, strchr(ff7_externals.field_file_name, '\\') + 1, src);

	ff7_externals.field_layers[dst]->graphics_object = ff7_externals._load_texture(1, PT_S2D, struc_3, 0, game_object->dx_sfx_something);
	ff7_externals.field_layers[dst]->present = true;
}

void field_load_textures(struct ff7_game_obj *game_object, struct struc_3 *struc_3)
{
	uint32_t i;

	ff7_externals.field_convert_type2_layers();

	for(i = 0; i < 29; i++)
	{
		uint32_t blend_mode = 4;

		if(!ff7_externals.field_layers[i]->present) continue;

		if(ff7_externals.field_layers[i]->type == 1)
		{
			if(i >= 24) blend_mode = 0;
			else if(i >= 15) blend_mode = 1;
		}
		else if(ff7_externals.field_layers[i]->type == 2)
		{
			if(i >= 40) blend_mode = 0;
			else if(i >= 33) blend_mode = 1;
		}
		else ffnx_glitch("unknown field layer type %i\n", ff7_externals.field_layers[i]->type);

		field_load_textures_helper(game_object, struc_3, i, i, blend_mode);

		// these magic numbers have been gleaned from original source data
		// the missing blend modes in question are used in exactly these pages
		// and copying them in this manner does not risk overwriting any other
		// data
		if(i >= 15 && i <= 18 && ff7_externals.field_layers[i]->type == 1) field_load_textures_helper(game_object, struc_3, i, i + 14, 2);
		if(i >= 15 && i <= 20 && ff7_externals.field_layers[i]->type == 1) field_load_textures_helper(game_object, struc_3, i, i + 18, 3);
	}

	*ff7_externals.layer2_end_page += 18;
}

void field_layer1_pick_tiles(short bg_position_x, short bg_position_y)
{
	int field_bg_multiplier = *ff7_externals.field_bg_multiplier;
	vector2<float> bg_position, initial_pos, tile_position;
	field_tile* layer1_tiles = *ff7_externals.field_layer1_tiles;

	bg_position.x = bg_position_x;
	bg_position.y = bg_position_y;
	if(ff7_fps_limiter >= FF7_LIMITER_30FPS)
	{
		if(bg_main_layer_pos.x != INVALID_VALUE && bg_main_layer_pos.y != INVALID_VALUE)
		{
			bg_position.x = bg_main_layer_pos.x;
			bg_position.y = bg_main_layer_pos.y;
		}
	}

	initial_pos.x = field_bg_multiplier * (320 - bg_position.x);
	initial_pos.y = field_bg_multiplier * ((ff7_center_fields ? 232 : 224) - bg_position.y);

	if(*ff7_externals.field_special_y_offset > 0 && bg_position.y <= 6)
		initial_pos.y -= field_bg_multiplier * (*ff7_externals.field_special_y_offset);

	for(int i = 0; i < *ff7_externals.field_layer1_tiles_num; i++)
	{
		uint32_t tile_index = (*ff7_externals.field_layer1_palette_sort)[i];
		layer1_tiles[tile_index].field_1044 = 1;

		// Add all tiles even if they are not inside the screen space
		tile_position.x = (initial_pos.x + field_bg_multiplier * layer1_tiles[tile_index].x);
		tile_position.y = (initial_pos.y + field_bg_multiplier * layer1_tiles[tile_index].y);
		ff7_externals.add_page_tile(tile_position.x, tile_position.y, 0.9997, layer1_tiles[tile_index].u,
									layer1_tiles[tile_index].v, layer1_tiles[tile_index].palette_index, layer1_tiles[tile_index].page);
	}
}

void field_layer2_pick_tiles(short bg_position_x, short bg_position_y)
{
	int field_bg_multiplier = *ff7_externals.field_bg_multiplier;
	field_tile *layer2_tiles = *ff7_externals.field_layer2_tiles;
	vector2<float> bg_position, initial_pos;

	bg_position.x = bg_position_x;
	bg_position.y = bg_position_y;
	if(ff7_fps_limiter >= FF7_LIMITER_30FPS)
	{
		if(bg_main_layer_pos.x != INVALID_VALUE && bg_main_layer_pos.y != INVALID_VALUE)
		{
			bg_position.x = bg_main_layer_pos.x;
			bg_position.y = bg_main_layer_pos.y;
		}
	}

	initial_pos.x = (320 - bg_position.x) * field_bg_multiplier;
	initial_pos.y = ((ff7_center_fields ? 232 : 224) - bg_position.y) * field_bg_multiplier;
	if(*ff7_externals.field_special_y_offset > 0 && bg_position.y <= 8)
		initial_pos.y -= (*ff7_externals.field_special_y_offset) * field_bg_multiplier;

	for(int i = 0; i < *ff7_externals.field_layer2_tiles_num; i++)
	{
		uint32_t tile_index = (*ff7_externals.field_layer2_palette_sort)[i];
		vector2<float> tile_position;

		char anim_group = layer2_tiles[tile_index].anim_group;
		if(anim_group && !(ff7_externals.modules_global_object->background_sprite_layer[anim_group] & layer2_tiles[tile_index].anim_bitmask))
			continue;

		layer2_tiles[tile_index].field_1040 = 1;

		tile_position.x = layer2_tiles[tile_index].x * field_bg_multiplier + initial_pos.x;
		tile_position.y = layer2_tiles[tile_index].y * field_bg_multiplier + initial_pos.y;

		uint32_t page = (layer2_tiles[tile_index].use_fx_page) ? layer2_tiles[tile_index].fx_page : layer2_tiles[tile_index].page;

		if(layer2_tiles[tile_index].use_fx_page && layer2_tiles[tile_index].blend_mode == 2) page += 14;
		if(layer2_tiles[tile_index].use_fx_page && layer2_tiles[tile_index].blend_mode == 3) page += 18;

		ff7_externals.add_page_tile(tile_position.x, tile_position.y, layer2_tiles[tile_index].z, layer2_tiles[tile_index].u,
									layer2_tiles[tile_index].v, layer2_tiles[tile_index].palette_index, page);
	}
}

void field_layer3_pick_tiles(short bg_position_x, short bg_position_y)
{
	if(*ff7_externals.do_draw_layer3_CFFE3C)
	{
		float z_value;
		int field_bg_multiplier = *ff7_externals.field_bg_multiplier;
		field_tile *layer3_tiles = *ff7_externals.field_layer3_tiles;
		vector2<float> bg_position, initial_pos;

		bg_position.x = bg_position_x;
		bg_position.y = bg_position_y;
		if(ff7_fps_limiter >= FF7_LIMITER_30FPS)
		{
			if(bg_layer3_pos.x != INVALID_VALUE && bg_layer3_pos.y != INVALID_VALUE)
			{
				bg_position.x = bg_layer3_pos.x;
				bg_position.y = bg_layer3_pos.y;
			}
		}

		initial_pos.x = (320 - bg_position.x) * field_bg_multiplier;
		initial_pos.y = ((ff7_center_fields ? 232 : 224) - bg_position.y) * field_bg_multiplier;
		if(ff7_externals.modules_global_object->field_B0 < 0xFFF)
			z_value = ff7_externals.field_layer_sub_C23C0F(ff7_externals.field_camera_CFF3D8, ff7_externals.modules_global_object->field_B0, 0, 0);
		else
			z_value = 0.9998;

		for(int i = 0; i < *ff7_externals.field_layer3_tiles_num; i++)
		{
			uint32_t tile_index = (*ff7_externals.field_layer3_palette_sort)[i];
			vector2<float> tile_position;
			tile_position.x = layer3_tiles[tile_index].x;
			tile_position.y = layer3_tiles[tile_index].y;

			if(tile_position.x <= bg_position.x - 352 || tile_position.x >= bg_position.x)
			{
				short width = (*ff7_externals.field_triggers_header)->bg3_width;
				tile_position.x += (tile_position.x >= bg_position.x - 160) ? -width : width;
			}
			if(tile_position.y <= bg_position.y - 256 || tile_position.y >= bg_position.y)
			{
				short height = (*ff7_externals.field_triggers_header)->bg3_height;
				tile_position.y += (tile_position.y >= bg_position.y - 112) ? -height : height;
			}

			char anim_group = layer3_tiles[tile_index].anim_group;
			if(anim_group && !(ff7_externals.modules_global_object->background_sprite_layer[anim_group] & layer3_tiles[tile_index].anim_bitmask))
				continue;

			layer3_tiles[tile_index].field_1040 = 1;
			tile_position.x = tile_position.x * field_bg_multiplier + initial_pos.x;
			tile_position.y = tile_position.y * field_bg_multiplier + initial_pos.y;

			uint32_t page = (layer3_tiles[tile_index].use_fx_page) ? layer3_tiles[tile_index].fx_page : layer3_tiles[tile_index].page;

			ff7_externals.add_page_tile(tile_position.x, tile_position.y, z_value, layer3_tiles[tile_index].u,
										layer3_tiles[tile_index].v, layer3_tiles[tile_index].palette_index, page);
		}
		*ff7_externals.field_layer3_flag_CFFE40 = 1;
	}
}

void field_layer4_pick_tiles(short bg_position_x, short bg_position_y)
{
	if(*ff7_externals.do_draw_layer4_CFFEA4)
	{
		int field_bg_multiplier = *ff7_externals.field_bg_multiplier;
		field_tile *layer4_tiles = *ff7_externals.field_layer4_tiles;
		vector2<float> bg_position, initial_pos;

		bg_position.x = bg_position_x;
		bg_position.y = bg_position_y;
		if(ff7_fps_limiter >= FF7_LIMITER_30FPS)
		{
			if(bg_layer4_pos.x != INVALID_VALUE && bg_layer4_pos.y != INVALID_VALUE)
			{
				bg_position.x = bg_layer4_pos.x;
				bg_position.y = bg_layer4_pos.y;
			}
		}

		initial_pos.x = (320 - bg_position.x) * field_bg_multiplier;
		initial_pos.y = ((ff7_center_fields ? 232 : 224) - bg_position.y) * field_bg_multiplier;
		float z_value = ff7_externals.field_layer_sub_C23C0F(ff7_externals.field_camera_CFF3D8, ff7_externals.modules_global_object->field_AE, 0, 0);

		for(int i = 0; i < *ff7_externals.field_layer4_tiles_num; i++)
		{
			uint32_t tile_index = (*ff7_externals.field_layer4_palette_sort)[i];
			vector2<float> tile_position;
			tile_position.x = layer4_tiles[tile_index].x;
			tile_position.y = layer4_tiles[tile_index].y;

			if(tile_position.x <= bg_position.x - 352 || tile_position.x >= bg_position.x)
			{
				short width = (*ff7_externals.field_triggers_header)->bg4_width;
				tile_position.x += (tile_position.x >= bg_position.x - 160) ? -width : width;
			}
			if(tile_position.y <= bg_position.y - 256 || tile_position.y >= bg_position.y)
			{
				short height = (*ff7_externals.field_triggers_header)->bg4_height;
				tile_position.y += (tile_position.y >= bg_position.y) ? -height : height;
			}

			char anim_group = layer4_tiles[tile_index].anim_group;
			if(tile_position.x < bg_position.x - 352 || tile_position.x > bg_position.x || anim_group && !(ff7_externals.modules_global_object->background_sprite_layer[anim_group] & layer4_tiles[tile_index].anim_bitmask))
				continue;

			layer4_tiles[tile_index].field_1040 = 1;
			tile_position.x = tile_position.x * field_bg_multiplier + initial_pos.x;
			tile_position.y = tile_position.y * field_bg_multiplier + initial_pos.y;

			if(!*ff7_externals.field_layer_CFF1D8 || layer4_tiles[tile_index].palette_index != (*ff7_externals.field_palette_D00088) + 1)
			{
				uint32_t page = (layer4_tiles[tile_index].use_fx_page) ? layer4_tiles[tile_index].fx_page : layer4_tiles[tile_index].page;
				ff7_externals.add_page_tile(tile_position.x, tile_position.y, z_value, layer4_tiles[tile_index].u,
											layer4_tiles[tile_index].v, layer4_tiles[tile_index].palette_index, page);
			}
		}
		*ff7_externals.field_layer4_flag_CFFEA8 = 1;
	}
}

void ff7_field_engine_sub_661B23(int field_world_x, int field_world_y)
{
	ff7_externals.engine_sub_661B23(field_world_x, field_world_y);

	// Override field_9A8 and field_9AC values with accurate field 3d world position when possible
	ff7_game_obj* game_obj = (ff7_game_obj*)common_externals.get_game_object();
	if(game_obj)
	{
		if(ff7_fps_limiter >= FF7_LIMITER_30FPS)
		{
			if(field_3d_world_pos.x != INVALID_VALUE && field_3d_world_pos.y != INVALID_VALUE)
			{
				*(float*)&game_obj->field_9A8 = field_3d_world_pos.x;
				*(float*)&game_obj->field_9AC = field_3d_world_pos.y;
			}
		}
	}
}

void ff7_field_sub_661B68(int field_world_x, int field_world_y)
{
	ff7_game_obj* game_obj = (ff7_game_obj*)common_externals.get_game_object();
	if(game_obj)
	{
		ff7_field_engine_sub_661B23(field_world_x, field_world_y);
		ff7_externals.engine_sub_67CCDE(*(float*)&game_obj->field_99C, *(float*)&game_obj->field_9A0, *(float*)&game_obj->field_9A4,
			*(float*)&game_obj->field_9A8, *(float*)&game_obj->field_9AC, (float)(int)game_obj->_res_w, (float)(int)game_obj->_res_h, game_obj);
	}
}

void ff7_field_set_world_position_sub_640EB7()
{
	if(field_3d_world_pos.x != INVALID_VALUE || *ff7_externals.field_world_pos_x != *ff7_externals.field_prev_world_pos_x || *ff7_externals.field_world_pos_y != *ff7_externals.field_prev_world_pos_y)
	{
		*ff7_externals.field_prev_world_pos_x = *ff7_externals.field_world_pos_x;
		*ff7_externals.field_prev_world_pos_y = *ff7_externals.field_world_pos_y;
		ff7_field_sub_661B68((*ff7_externals.field_bg_multiplier) * (*ff7_externals.field_world_pos_x), (*ff7_externals.field_bg_multiplier) * (*ff7_externals.field_world_pos_y));
	}
}

void float_sub_643628(field_trigger_header *trigger_header, vector2<float> *delta_position)
{
	if (trigger_header->field_14[0] == 1)
	{
		float diff_top_bottom = trigger_header->camera_range.top - 120 - (trigger_header->camera_range.bottom + 120);
		float diff_right_left = trigger_header->camera_range.right - 160 - (trigger_header->camera_range.left + 160);
		float temp_1 = -(diff_top_bottom * (trigger_header->camera_range.bottom + 120 - delta_position->y) + diff_right_left * (trigger_header->camera_range.left + 160 - delta_position->x));
		float temp_square_value = (diff_top_bottom * diff_top_bottom + diff_right_left * diff_right_left) / 256.f;
		delta_position->x = ((diff_right_left * temp_1 / temp_square_value) / 256.f) + trigger_header->camera_range.left + 160;
		delta_position->y = ((diff_top_bottom * temp_1 / temp_square_value) / 256.f) + trigger_header->camera_range.bottom + 120;
	}
	if (trigger_header->field_14[0] == 2)
	{
		float diff_bottom_top = trigger_header->camera_range.bottom + 120 - (trigger_header->camera_range.top - 120);
		float diff_right_left = trigger_header->camera_range.right - 160 - (trigger_header->camera_range.left + 160);
		float temp_1 = -((diff_bottom_top) * (trigger_header->camera_range.top - 120 - delta_position->y) + diff_right_left * (trigger_header->camera_range.left + 160 - delta_position->x));
		float temp_square_value = (diff_bottom_top * diff_bottom_top + diff_right_left * diff_right_left) / 256.f;
		delta_position->x = ((diff_right_left * temp_1 / temp_square_value) / 256.f) + trigger_header->camera_range.left + 160;
		delta_position->y = ((diff_bottom_top * temp_1 / temp_square_value) / 256.f) + trigger_header->camera_range.top - 120;
	}
}

int float_sub_66307D(vector3<float> *position, vector2<float> *delta_position, int *param_3, int *param_4)
{
	int z_value;
	float matrix[16];
	vector3<float> temp_point, temp_point_1;
	vector3<float> position_copy = *position;
	byte* global_game_data = *(byte**)ff7_externals.global_game_data_90AAF0;

	ff7_externals.engine_sub_661465((short*)&global_game_data[16], matrix);
	ff7_externals.engine_sub_66CF7E(matrix, &position_copy, &temp_point);
	temp_point_1.x = (double)*(int*)&global_game_data[34] + temp_point.x;
	temp_point_1.y = (double)*(int*)&global_game_data[38] + temp_point.y;
	temp_point_1.z = (double)*(int*)&global_game_data[42] + temp_point.z;
	if (temp_point_1.z == 0.f)
	{
		z_value = 0;
	}
	else
	{
		delta_position->x = (temp_point_1.x * (*(float*)global_game_data) / temp_point_1.z + (*(float*)&global_game_data[56]));
		delta_position->y = (temp_point_1.y * (*(float*)global_game_data) / temp_point_1.z + (*(float*)&global_game_data[60]));
		z_value = (temp_point_1.z * 0.25f);
	}
	*param_3 = 0;
	*param_4 = 0;
	return z_value;
}

int float_sub_64314F(vector3<float> *position, vector2<float> *bg_delta_position)
{
	int dummy_1, dummy_2;
	int camera_pos_z;

	ff7_externals.engine_sub_663673((WORD*)ff7_externals.field_camera_CFF3D8);
	ff7_externals.engine_sub_663707((DWORD*)ff7_externals.field_camera_CFF3D8);
	ff7_externals.engine_sub_661976(ff7_externals.field_vector2_CFF204->x, ff7_externals.field_vector2_CFF204->y);
	camera_pos_z = float_sub_66307D(position, bg_delta_position, &dummy_1, &dummy_2);
	ff7_externals.engine_sub_661976(ff7_externals.field_vector2_CFF1F4->x, ff7_externals.field_vector2_CFF1F4->y);
	return camera_pos_z;
}

void ff7_field_update_background()
{
	ff7_externals.field_update_background_positions();

	if ( *ff7_externals.word_CC1638 && !ff7_externals.modules_global_object->BGMOVIE_flag)
	{
		field_3d_world_pos = {INVALID_VALUE, INVALID_VALUE};
	}
	else if(*ff7_externals.field_bg_flag_CC15E4)
	{
		field_3d_world_pos = {INVALID_VALUE, INVALID_VALUE};
		bg_main_layer_pos = {INVALID_VALUE, INVALID_VALUE};
		bg_layer3_pos = {INVALID_VALUE, INVALID_VALUE};
		bg_layer4_pos = {INVALID_VALUE, INVALID_VALUE};
	}
	else
	{
		field_trigger_header* field_triggers_header_ptr = *ff7_externals.field_triggers_header;
		field_event_data* field_event_data_ptr = *ff7_externals.field_event_data_ptr;
		vector2<float> bg_delta_position;
		vector3<float> player_position;
		int field_bg_multiplier = *ff7_externals.field_bg_multiplier;

		player_position.x = field_event_data_ptr[*ff7_externals.field_player_model_id].model_pos.x >> 12;
		player_position.y = field_event_data_ptr[*ff7_externals.field_player_model_id].model_pos.y >> 12;
		player_position.z = ff7_externals.modules_global_object->field_16 + (field_event_data_ptr[*ff7_externals.field_player_model_id].model_pos.z >> 12);
		float_sub_64314F(&player_position, &bg_delta_position);
		bg_delta_position.x = bg_delta_position.x - ff7_externals.field_vector2_CFF204->x;
		bg_delta_position.y = bg_delta_position.y - ff7_externals.field_vector2_CFF204->y;
		if (bg_delta_position.x > field_triggers_header_ptr->camera_range.right - 160)
			bg_delta_position.x = field_triggers_header_ptr->camera_range.right - 160;
		if (bg_delta_position.x < field_triggers_header_ptr->camera_range.left + 160)
			bg_delta_position.x = field_triggers_header_ptr->camera_range.left + 160;
		if (bg_delta_position.y > field_triggers_header_ptr->camera_range.top - 120)
			bg_delta_position.y = field_triggers_header_ptr->camera_range.top - 120;
		if (bg_delta_position.y < field_triggers_header_ptr->camera_range.bottom + 120)
			bg_delta_position.y = field_triggers_header_ptr->camera_range.bottom + 120;
		float_sub_643628(field_triggers_header_ptr, &bg_delta_position);

		field_3d_world_pos.x = (ff7_externals.modules_global_object->shake_bg_x.shake_curr_value + ff7_externals.field_bg_offset->x - bg_delta_position.x - 160) * field_bg_multiplier;
		field_3d_world_pos.y = (ff7_externals.modules_global_object->shake_bg_y.shake_curr_value + ff7_externals.field_bg_offset->y - bg_delta_position.y - 120) * field_bg_multiplier;
		bg_main_layer_pos.x = bg_delta_position.x + 320 - ff7_externals.field_bg_offset->x - ff7_externals.modules_global_object->shake_bg_x.shake_curr_value;
		bg_main_layer_pos.y = bg_delta_position.y + 232 - ff7_externals.field_bg_offset->y - ff7_externals.modules_global_object->shake_bg_y.shake_curr_value;
		bg_layer3_pos.x = (field_triggers_header_ptr->bg3_pos_x / 16.f) + ((field_triggers_header_ptr->bg3_speed_x * bg_delta_position.x) / 256.f);
		bg_layer3_pos.x = remainder(bg_layer3_pos.x, field_triggers_header_ptr->bg3_width);
		bg_layer3_pos.x = bg_layer3_pos.x + 320 - ff7_externals.field_bg_offset->x - ff7_externals.modules_global_object->shake_bg_x.shake_curr_value;
		bg_layer3_pos.y = (field_triggers_header_ptr->bg3_pos_y / 16.f) + ((field_triggers_header_ptr->bg3_speed_y * bg_delta_position.y) / 256.f);
		bg_layer3_pos.y = remainder(bg_layer3_pos.y, field_triggers_header_ptr->bg3_height);
		bg_layer3_pos.y = bg_layer3_pos.y + 232 - ff7_externals.field_bg_offset->y - ff7_externals.modules_global_object->shake_bg_y.shake_curr_value;
		bg_layer4_pos.x = (field_triggers_header_ptr->bg4_pos_x / 16.f) + ((field_triggers_header_ptr->bg4_speed_x * bg_delta_position.x) / 256.f);
		bg_layer4_pos.x = remainder(bg_layer4_pos.x, field_triggers_header_ptr->bg4_width);
		bg_layer4_pos.x = bg_layer4_pos.x + 320 - ff7_externals.field_bg_offset->x - ff7_externals.modules_global_object->shake_bg_x.shake_curr_value;
		bg_layer4_pos.y = (field_triggers_header_ptr->bg4_pos_y / 16.f) + ((field_triggers_header_ptr->bg4_speed_y * bg_delta_position.y) / 256.f);
		bg_layer4_pos.y = remainder(bg_layer4_pos.y, field_triggers_header_ptr->bg4_height);
		bg_layer4_pos.y = bg_layer4_pos.y + 232 - ff7_externals.field_bg_offset->y - ff7_externals.modules_global_object->shake_bg_y.shake_curr_value;
	}
}

uint32_t field_open_flevel_siz()
{
	struct lgp_file *f = lgp_open_file("flevel.siz", 1);

	if (0 == f) {
		return 0;
	}

	uint32_t size = lgp_get_filesize(f, 1);
	char* buffer = new char[size];

	lgp_read_file(f, 1, buffer, size);

	// Increase from 787 (field map count) to 1200
	const uint32_t max_map_count = 1200;
	uint32_t* uncompressed_sizes = reinterpret_cast<uint32_t*>(buffer);
	uint32_t count = size / sizeof(uint32_t);
	uint32_t* flevel_sizes = reinterpret_cast<uint32_t*>(ff7_externals.field_map_infos + 0xBC);

	if (count > max_map_count) {
		count = max_map_count;
	}

	for (uint32_t i = 0; i < count; ++i) {
		flevel_sizes[i * 0x34] = uncompressed_sizes[i] + 4000000; // +2 MB compared to the original implementation
	}

	// Force a value if not specified by the flevel.siz
	for (uint32_t i = count; i < max_map_count; ++i) {
		flevel_sizes[i * 0x34] = 4000000;
	}

	delete[] buffer;

	return 1;
}

bool is_fps_running_more_than_original()
{
	if(is_overlapping_movie_playing())
		return movie_fps_ratio > 1;
	else
		return ff7_fps_limiter == FF7_LIMITER_60FPS;
}

int get_frame_multiplier()
{
	if(is_overlapping_movie_playing())
		return movie_fps_ratio;
	else
		return common_frame_multiplier;
}

void ff7_field_initialize_variables()
{
	((void(*)())ff7_externals.field_initialize_variables)();

	field_3d_world_pos = {INVALID_VALUE, INVALID_VALUE};
	bg_main_layer_pos = {INVALID_VALUE, INVALID_VALUE};
	bg_layer3_pos = {INVALID_VALUE, INVALID_VALUE};
	bg_layer4_pos = {INVALID_VALUE, INVALID_VALUE};

	// reset movement frame index for all models
	for(auto &external_data : external_model_data){
		external_data.moveFrameIndex = 0;
		external_data.rotationMoveFrameIndex = 0;
	}
}

void ff7_field_update_models_position(int key_input_status)
{
	((void(*)(int))ff7_externals.field_update_models_positions)(key_input_status);

	for(int model_idx = 0; model_idx < (int)(*ff7_externals.field_n_models); model_idx++)
	{
		// Reset movement frame index for all models if they are not walking/running
		if((*ff7_externals.field_event_data_ptr)[model_idx].movement_type != 1)
		{
			external_model_data[model_idx].moveFrameIndex = 0;
		}

		// Reset rotation movement frame index for all models if they are not rotating
		byte rotation_type = (*ff7_externals.field_event_data_ptr)[model_idx].rotation_steps_type;
		if(rotation_type == 0 || rotation_type == 3)
		{
			external_model_data[model_idx].rotationMoveFrameIndex = 0;
		}
	}
}

int ff7_field_update_player_model_position(short model_id)
{
	field_event_data* field_event_data_array = (*ff7_externals.field_event_data_ptr);
	int original_movement_speed = field_event_data_array[model_id].movement_speed;
	int frame_multiplier = get_frame_multiplier();
	if(is_fps_running_more_than_original())
	{
		field_event_data_array[model_id].movement_speed = original_movement_speed / frame_multiplier;
	}

	int is_player_moving = ff7_externals.field_update_single_model_position(model_id);
	field_event_data_array[model_id].movement_speed = original_movement_speed;

	// Allow footsteps to be detected correctly
	if(ff7_footsteps)
		sfx_process_footstep(is_player_moving);

	return is_player_moving;
}

int ff7_field_update_single_model_position(short model_id)
{
	int ret;
	int frame_multiplier = get_frame_multiplier();
	field_event_data* field_event_data_array = (*ff7_externals.field_event_data_ptr);

	if(is_fps_running_more_than_original())
	{
		int interpolationStep = external_model_data[model_id].moveFrameIndex + 1;
		if(external_model_data[model_id].moveFrameIndex == 0)
		{
			external_model_data[model_id].initialPosition = field_event_data_array[model_id].model_pos;
			ret = ff7_externals.field_update_single_model_position(model_id);
			external_model_data[model_id].updateMovementReturnValue = ret;
			external_model_data[model_id].finalPosition = field_event_data_array[model_id].model_pos;
			field_event_data_array[model_id].model_pos.x = external_model_data[model_id].initialPosition.x + ((external_model_data[model_id].finalPosition.x - external_model_data[model_id].initialPosition.x) * interpolationStep) / frame_multiplier;
			field_event_data_array[model_id].model_pos.y = external_model_data[model_id].initialPosition.y + ((external_model_data[model_id].finalPosition.y - external_model_data[model_id].initialPosition.y) * interpolationStep) / frame_multiplier;
			field_event_data_array[model_id].model_pos.z = external_model_data[model_id].initialPosition.z + ((external_model_data[model_id].finalPosition.z - external_model_data[model_id].initialPosition.z) * interpolationStep) / frame_multiplier;
		}
		else
		{
			ret = external_model_data[model_id].updateMovementReturnValue;
			field_event_data_array[model_id].model_pos.x = external_model_data[model_id].initialPosition.x + ((external_model_data[model_id].finalPosition.x - external_model_data[model_id].initialPosition.x) * interpolationStep) / frame_multiplier;
			field_event_data_array[model_id].model_pos.y = external_model_data[model_id].initialPosition.y + ((external_model_data[model_id].finalPosition.y - external_model_data[model_id].initialPosition.y) * interpolationStep) / frame_multiplier;
			field_event_data_array[model_id].model_pos.z = external_model_data[model_id].initialPosition.z + ((external_model_data[model_id].finalPosition.z - external_model_data[model_id].initialPosition.z) * interpolationStep) / frame_multiplier;
		}
		external_model_data[model_id].moveFrameIndex = (external_model_data[model_id].moveFrameIndex + 1) % frame_multiplier;
	}
	else
	{
		ret = ff7_externals.field_update_single_model_position(model_id);
	}

	return ret;
}

int ff7_field_check_collision_with_target(field_event_data* field_event_model, short target_collision_radius)
{
	int ret;
	int frame_multiplier = get_frame_multiplier();
	int model_id = std::distance(*ff7_externals.field_event_data_ptr, field_event_model);

	if(is_fps_running_more_than_original())
	{
		if(external_model_data[model_id].moveFrameIndex == 0)
		{
			ret = ff7_externals.field_check_collision_with_target(field_event_model, target_collision_radius);
			external_model_data[model_id].wasNotCollidingWithTarget = ret;
		}
		else
		{
			ret = external_model_data[model_id].wasNotCollidingWithTarget;
		}
	}
	else
	{
		ret = ff7_externals.field_check_collision_with_target(field_event_model, target_collision_radius);
	}

	return ret;
}

void ff7_field_update_models_rotation_new()
{
	for(int model_idx = 0; model_idx < *ff7_externals.field_n_models; model_idx++)
	{
		auto &field_event_data = (*ff7_externals.field_event_data_ptr)[model_idx];
		byte rotation_type = field_event_data.rotation_steps_type;
		if(rotation_type)
		{
			// Legacy code still works, but when in 60FPS, the steps index and number of steps are modified
			uint32_t rotation_n_steps = field_event_data.rotation_n_steps;
			uint32_t rotation_steps_idx = field_event_data.rotation_step_idx;

			if(is_fps_running_more_than_original())
			{
				rotation_n_steps *= get_frame_multiplier();
				rotation_steps_idx = rotation_steps_idx * get_frame_multiplier() + external_model_data[model_idx].rotationMoveFrameIndex;
			}

			if(rotation_type == 1)
			{
				field_event_data.rotation_curr_value = ff7_externals.field_get_linear_interpolated_value(
					field_event_data.rotation_initial,
					field_event_data.rotation_final,
					rotation_n_steps,
					rotation_steps_idx
				);

				if(field_event_data.rotation_step_idx == field_event_data.rotation_n_steps)
					field_event_data.rotation_steps_type = 3;
				else
				{
					if(is_fps_running_more_than_original())
					{
						external_model_data[model_idx].rotationMoveFrameIndex = (external_model_data[model_idx].rotationMoveFrameIndex + 1) % get_frame_multiplier();
						if(external_model_data[model_idx].rotationMoveFrameIndex == 0)
							field_event_data.rotation_step_idx++;
					}
					else
					{
						field_event_data.rotation_step_idx++;
					}
				}
			}
			else if(rotation_type == 2)
			{
				field_event_data.rotation_curr_value = ff7_externals.field_get_smooth_interpolated_value(
					field_event_data.rotation_initial,
					field_event_data.rotation_final,
					rotation_n_steps,
					rotation_steps_idx
				);

				if(field_event_data.rotation_step_idx == field_event_data.rotation_n_steps)
					field_event_data.rotation_steps_type = 3;
				else
				{
					if(is_fps_running_more_than_original())
					{
						external_model_data[model_idx].rotationMoveFrameIndex = (external_model_data[model_idx].rotationMoveFrameIndex + 1) % get_frame_multiplier();
						if(external_model_data[model_idx].rotationMoveFrameIndex == 0)
							field_event_data.rotation_step_idx++;
					}
					else
					{
						field_event_data.rotation_step_idx++;
					}
				}
			}
		}
	}
}

short ff7_opcode_multiply_get_bank_value(short bank, short address)
{
	int16_t ret = ff7_externals.get_bank_value(bank, address);
	if(is_fps_running_more_than_original())
		ret *= get_frame_multiplier();
	return ret;
}

short ff7_opcode_divide_get_bank_value(short bank, short address)
{
	int16_t ret = ff7_externals.get_bank_value(bank, address);
	if(is_fps_running_more_than_original())
	{
		if(abs(ret) >= get_frame_multiplier())
			ret /= get_frame_multiplier();
	}
	return ret;
}

int opcode_script_partial_animation_wrapper()
{
	int frame_multiplier = get_frame_multiplier();

	byte curr_opcode = get_field_parameter<byte>(-1);
	WORD total_number_of_frames = -1;
	byte curr_model_id = ff7_externals.field_model_id_array[*ff7_externals.current_entity_id];
	byte speed = get_field_parameter<byte>(3);
	WORD first_frame = 16 * get_field_parameter<byte>(1) * frame_multiplier / ((curr_opcode == CANIM1 || curr_opcode == CANIM2) ? speed : 1);
	WORD last_frame = (get_field_parameter<byte>(2) * frame_multiplier + 1) / speed;
	field_event_data* event_data = *ff7_externals.field_event_data_ptr;
	field_animation_data* animation_data = *ff7_externals.field_animation_data_ptr;
	char animation_type = ff7_externals.animation_type_array[curr_model_id];

	int ret = call_original_opcode_function(curr_opcode);

	if(curr_model_id != 255)
	{
		switch(animation_type)
		{
		case 0:
		case 1:
		case 3:
			if(animation_data)
				total_number_of_frames = animation_data[curr_model_id].anim_frame_object[2] - 1;

			if(last_frame > total_number_of_frames)
				last_frame = total_number_of_frames;

			event_data[curr_model_id].firstFrame = first_frame;
			event_data[curr_model_id].lastFrame = last_frame;
			break;
		default:
			break;
		}
	}

	return ret;
}

int ff7_opcode_script_SHAKE()
{
	byte type = get_field_parameter<byte>(2);
	auto *field_global_data_ptr = *ff7_externals.field_global_object_ptr;
	if ( (type & 1) != 0 )
	{
		field_global_data_ptr->shake_bg_x.do_shake = 1;
		field_global_data_ptr->shake_bg_x.shake_amplitude = ff7_externals.get_char_bank_value(1, 4);
		field_global_data_ptr->shake_bg_x.shake_n_steps = ff7_externals.get_char_bank_value(2, 5);

		if(is_fps_running_more_than_original())
			field_global_data_ptr->shake_bg_x.shake_n_steps *= get_frame_multiplier();
	}
	else
	{
		field_global_data_ptr->shake_bg_x.do_shake = 0;
	}
	if ( (type & 2) != 0 )
	{
		field_global_data_ptr->shake_bg_y.do_shake = 1;
		field_global_data_ptr->shake_bg_y.shake_amplitude = ff7_externals.get_char_bank_value(3, 6);
		field_global_data_ptr->shake_bg_y.shake_n_steps = ff7_externals.get_char_bank_value(4, 7);

		if(is_fps_running_more_than_original())
			field_global_data_ptr->shake_bg_y.shake_n_steps *= get_frame_multiplier();
	}
	else
	{
		field_global_data_ptr->shake_bg_y.do_shake = 0;
	}
	ff7_externals.field_curr_script_position[*ff7_externals.current_entity_id] += 8;
	return 0;
}

int script_WAIT()
{
	int result = 0;

	WORD frames_left = ff7_externals.wait_frames_ptr[*ff7_externals.current_entity_id];
	if (frames_left)
	{
		if (frames_left == 1)
		{
			ff7_externals.wait_frames_ptr[*ff7_externals.current_entity_id] = 0;
			ff7_externals.field_curr_script_position[*ff7_externals.current_entity_id] += 3;
			result = 0;
		}
		else
		{
			--ff7_externals.wait_frames_ptr[*ff7_externals.current_entity_id];
			result = 1;
		}
	}
	else
	{
		ff7_externals.wait_frames_ptr[*ff7_externals.current_entity_id] = get_field_parameter<WORD>(0);

		if(is_fps_running_more_than_original())
			ff7_externals.wait_frames_ptr[*ff7_externals.current_entity_id] *= get_frame_multiplier();

		if (!ff7_externals.wait_frames_ptr[*ff7_externals.current_entity_id])
			ff7_externals.field_curr_script_position[*ff7_externals.current_entity_id] += 3;
		result = 1;
	}

	return result;
}

int script_MVIEF()
{
	mvief_bank_address = {get_field_parameter<byte>(0), get_field_parameter<byte>(1)};

	return call_original_opcode_function(0xFA);
}

int script_BGMOVIE()
{
	is_movie_bgfield = get_field_parameter<byte>(0);

	return call_original_opcode_function(0x27);
}

uint8_t ff7_compare_ifsw()
{
	int16_t left_value = ff7_externals.get_bank_value(1, 2);
	int16_t right_value = ff7_externals.get_bank_value(2, 4);
	byte compare_type = get_field_parameter<byte>(5);

	field_bank_address current_mvief_bank_address = {get_field_bank_value(0), (byte)get_field_parameter<WORD>(1)};

	// Movie fix
	if (is_overlapping_movie_playing() && movie_fps_ratio > 1)
	{
		if (current_mvief_bank_address == mvief_bank_address)
			right_value *= movie_fps_ratio;
	}
	else
	{
		if(ff7_fps_limiter == FF7_LIMITER_60FPS)
		{
			if(field_bank_address_to_be_fixed.contains(current_mvief_bank_address))
				right_value *= common_frame_multiplier;
		}
	}

	switch(compare_type)
	{
	case 0:
		return (left_value == right_value);
	case 1:
		return (left_value != right_value);
	case 2:
		return (left_value > right_value);
	case 3:
		return (left_value < right_value);
	case 4:
		return (left_value >= right_value);
	case 5:
		return (left_value <= right_value);
	case 6:
		return (right_value & left_value);
	case 7:
		return (right_value ^ left_value);
	case 8:
		return (right_value | left_value);
	case 9:
		return ((1 << right_value) & left_value);
	case 10:
		return ((uint8_t)((1 << right_value) & left_value) == 0);
	default:
		return 0;
	}
}

int ff7_opcode_script_FADE_wrapper()
{
	int ret = ((int(*)())ff7_externals.opcode_fade)();

	if(is_fps_running_more_than_original())
	{
		if((*ff7_externals.field_global_object_ptr)->fade_speed >= get_frame_multiplier())
			(*ff7_externals.field_global_object_ptr)->fade_speed /= get_frame_multiplier();
	}

	return ret;
}

void ff7_field_update_model_animation_frame(short model_id)
{
	field_event_data &model_event_data = (*ff7_externals.field_event_data_ptr)[model_id];
	const int original_animation_speed = model_event_data.animation_speed;
	if (is_overlapping_movie_playing())
	{
		if(movie_fps_ratio == 1 && ff7_fps_limiter == FF7_LIMITER_60FPS)
			model_event_data.animation_speed *= common_frame_multiplier;
		else if(movie_fps_ratio > 1 && ff7_fps_limiter < FF7_LIMITER_60FPS)
			model_event_data.animation_speed /= movie_fps_ratio;
		else if(movie_fps_ratio > 2 && ff7_fps_limiter == FF7_LIMITER_60FPS)
			model_event_data.animation_speed /= (movie_fps_ratio / 2);
	}

	ff7_externals.field_update_model_animation_frame(model_id);

	model_event_data.animation_speed = original_animation_speed;
}

void ff7_field_evaluate_encounter_rate()
{
	field_event_data* field_event_data_array = (*ff7_externals.field_event_data_ptr);
	int original_movement_speed = field_event_data_array[*ff7_externals.field_player_model_id].movement_speed;
	field_event_data_array[*ff7_externals.field_player_model_id].movement_speed = original_movement_speed / common_frame_multiplier;
	ff7_externals.field_evaluate_encounter_rate_60B2C6();
	field_event_data_array[*ff7_externals.field_player_model_id].movement_speed = original_movement_speed;
}

void ff7_field_hook_init()
{
	std::copy(common_externals.execute_opcode_table, &common_externals.execute_opcode_table[0xFF], &original_opcode_table[0]);

	// Init stuff
	replace_call_function(ff7_externals.field_sub_60DCED + 0x178, ff7_field_initialize_variables);

	// Model movement (walk, run) fps fix + allow footstep sfx
	replace_call_function(ff7_externals.field_loop_sub_63C17F + 0x5DD, ff7_field_update_models_position);
	replace_call_function(ff7_externals.field_update_models_positions + 0x8BC, ff7_field_update_player_model_position);
	replace_call_function(ff7_externals.field_update_models_positions + 0x9E8, ff7_field_update_single_model_position);
	replace_call_function(ff7_externals.field_update_models_positions + 0x9AA, ff7_field_check_collision_with_target);
	replace_call_function(common_externals.execute_opcode_table[0xC3] + 0x46, ff7_opcode_multiply_get_bank_value);

	// Model rotation
	byte jump_to_OFST_update[] = {0xE9, 0xE6, 0x01, 0x00, 0x00};
	replace_call_function(ff7_externals.field_update_models_positions + 0x7C, ff7_field_update_models_rotation_new);
	memcpy_code(ff7_externals.field_update_models_positions + 0x81, jump_to_OFST_update, sizeof(jump_to_OFST_update));

	if(ff7_fps_limiter >= FF7_LIMITER_30FPS)
	{
		if(ff7_fps_limiter == FF7_LIMITER_60FPS)
		{
			// Model movement fps fix for ladder and jump
			patch_code_byte(ff7_externals.field_update_models_positions + 0x1041, 0x2 - common_frame_multiplier / 2);
			patch_code_byte(ff7_externals.field_update_models_positions + 0x189A, 0x2 - common_frame_multiplier / 2);
			replace_call_function(common_externals.execute_opcode_table[JUMP] + 0x1F1, ff7_opcode_multiply_get_bank_value);
			patch_divide_code<int>(ff7_externals.field_update_models_positions + 0xC89, common_frame_multiplier * 2);
			patch_divide_code<int>(ff7_externals.field_update_models_positions + 0xE48, common_frame_multiplier * 2);

			// Partial animation fps fix
			patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANM1], (DWORD)&opcode_script_partial_animation_wrapper);
			patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANM2], (DWORD)&opcode_script_partial_animation_wrapper);
			patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANIM1], (DWORD)&opcode_script_partial_animation_wrapper);
			patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANIM2], (DWORD)&opcode_script_partial_animation_wrapper);

			// Encounter rate fix
			replace_call_function(ff7_externals.field_update_models_positions + 0x90F, ff7_field_evaluate_encounter_rate);

			// Message speed fix
			patch_code_byte(ff7_externals.sub_631945 + 0xFD, 0x5 + common_frame_multiplier / 2);
			patch_divide_code<byte>(ff7_externals.sub_631945 + 0x100, common_frame_multiplier);
			patch_divide_code<WORD>(ff7_externals.sub_631945 + 0x111, common_frame_multiplier);
			patch_code_byte(ff7_externals.sub_631945 + 0x141, 0x4 + common_frame_multiplier / 2);
		}

		// Smooth background movement for both 30 fps mode and 60 fps mode
		replace_call_function(ff7_externals.field_draw_everything + 0x34, ff7_field_set_world_position_sub_640EB7);
		replace_call_function(ff7_externals.field_loop_sub_63C17F + 0x1A6, ff7_field_update_background);
	}

	// Background scroll fps fix
	replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x34, ff7_opcode_divide_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x4D, ff7_opcode_divide_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x68, ff7_opcode_divide_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x81, ff7_opcode_divide_get_bank_value);
	replace_function(ff7_externals.opcode_shake, ff7_opcode_script_SHAKE);

	// Camera fps fix
	replace_call_function(common_externals.execute_opcode_table[SCRLC] + 0x3B, ff7_opcode_multiply_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[SCRLA] + 0x72, ff7_opcode_multiply_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[SCR2DC] + 0x3C, ff7_opcode_multiply_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[SCR2DL] + 0x3C, ff7_opcode_multiply_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[SCRLP] + 0xA7, ff7_opcode_multiply_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[NFADE] + 0x89, ff7_opcode_divide_get_bank_value);
	replace_call_function(common_externals.execute_opcode_table[VWOTF] + 0xCC, ff7_opcode_multiply_get_bank_value);
	patch_code_dword((uint32_t)&common_externals.execute_opcode_table[FADE], (DWORD)&ff7_opcode_script_FADE_wrapper);

	// Movie model animation fps fix
	replace_call_function(ff7_externals.field_update_models_positions + 0x68D, ff7_field_update_model_animation_frame);
	replace_call_function(ff7_externals.field_update_models_positions + 0x919, ff7_field_update_model_animation_frame);
	replace_call_function(ff7_externals.field_update_models_positions + 0xA2B, ff7_field_update_model_animation_frame);
	replace_call_function(ff7_externals.field_update_models_positions + 0xE8C, ff7_field_update_model_animation_frame);

	// Movie fps fix
	patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0xFA], (DWORD)&script_MVIEF);
	patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x27], (DWORD)&script_BGMOVIE);

	// Others fps fix
	patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x24], (DWORD)&script_WAIT);
	replace_function(ff7_externals.sub_611BAE, ff7_compare_ifsw);
}