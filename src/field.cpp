/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
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

#include <imgui.h>
#include <stdint.h>

#include "globals.h"
#include "common.h"
#include "patch.h"
#include "log.h"
#include "utils.h"

#include "field.h"

// Data for debug map jumps
int target_triangle = 0;
int target_field = 0;
byte map_patch_storage[7] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // Place to store the original bytes so that we can patch back after a map jump
bool map_changing = false;

// FF7 only
int (*opcode_old_kawai)();
int (*opcode_old_pc)();

ff7_polygon_set* ff7_head = NULL;
ff7_model_eye_texture_data ff7_eyes[9];
ff7_model_mouth_data ff7_mouths[10];
byte ff7_curr_eye_index = NULL;


byte get_field_bank_value(int16_t bank)
{
	switch(bank)
	{
	case 0:
		return (get_field_parameter<byte>(0) >> 4) & 0xF;
	case 1:
		return get_field_parameter<byte>(0) & 0xF;
	case 2:
		return (get_field_parameter<byte>(1) >> 4) & 0xF;
	case 3:
		return get_field_parameter<byte>(1) & 0xF;
	case 4:
		return (get_field_parameter<byte>(2) >> 4) & 0xF;
	case 5:
		return get_field_parameter<byte>(2) & 0xF;
	default:
		return 0;
	}
}

int opcode_kawai_eye_texture() {
	byte num_params = get_field_parameter<byte>(0);
	byte subcode = get_field_parameter<byte>(1);

	if (subcode == 0x0) // EYETX
	{
		byte left_eye_index = get_field_parameter<byte>(2);
		byte right_eye_index = get_field_parameter<byte>(3);
		byte mouth_index = get_field_parameter<byte>(4);

		field_animation_data* animation_data = *ff7_externals.field_animation_data_ptr;
		byte curr_model_id = ff7_externals.field_model_id_array[*ff7_externals.current_entity_id];
		ff7_curr_eye_index = animation_data[curr_model_id].eye_texture_idx;
		char curr_model_name[10];

		_splitpath((const char*)(*ff7_externals.field_models_data + (10380 * curr_model_id) + 512), NULL, NULL, curr_model_name, NULL);

		if (trace_all || trace_opcodes)
		{
			ffnx_trace("opcode[KAWAI]: num_params=%u,subcode=0x%x,left_eye_index=%u,right_eye_index=%u,mouth_index=%u\n", num_params, subcode, left_eye_index, right_eye_index, mouth_index);
			ffnx_trace("subcode[EYETX]: curr_model_id=%u,curr_eye_index=%u,curr_model_name=%s\n", curr_model_id, ff7_curr_eye_index, curr_model_name);
		}

		if (ff7_curr_eye_index < 10)
		{
			char directpath[MAX_PATH + sizeof(basedir)];
			char filename[10];
			char ext[4];
			bool ext_left_eye_found = false, ext_right_eye_found = false;

			// NPCs always default on Cloud eyes/mouth
			if (ff7_curr_eye_index == 9) ff7_curr_eye_index = 0;

			// LEFT EYE
			_splitpath(ff7_eyes[ff7_curr_eye_index].static_left_eye_filename, NULL, NULL, filename, ext);

			_snprintf(directpath, sizeof(directpath), "%s/%s/flevel/eye_%s_%d.TEX", basedir, direct_mode_path.c_str(), curr_model_name, left_eye_index);
			if (ext_left_eye_found = fileExists(directpath))
				_snprintf(ff7_externals.field_models_eye_blink_buffer[ff7_curr_eye_index].static_left_eye_filename, 1024, "eye_%s_%d%s", curr_model_name, left_eye_index, ext);
			else
			{
				if (left_eye_index > 2 && (trace_all || trace_direct || trace_opcodes)) ffnx_trace("subcode[EYETX]: Custom left eye texture not found: %s\n", directpath);

				_snprintf(directpath, sizeof(directpath), "%s/%s/flevel/%s_%d.TEX", basedir, direct_mode_path.c_str(), filename, left_eye_index);
				if (ext_left_eye_found = fileExists(directpath))
					_snprintf(ff7_externals.field_models_eye_blink_buffer[ff7_curr_eye_index].static_left_eye_filename, 1024, "%s_%d%s", filename, left_eye_index, ext);
				else
				{
					if (left_eye_index > 2 && (trace_all || trace_direct || trace_opcodes)) ffnx_trace("subcode[EYETX]: Custom left eye texture not found: %s\n", directpath);

					_snprintf(ff7_externals.field_models_eye_blink_buffer[ff7_curr_eye_index].static_left_eye_filename, 1024, "%s%s", filename, ext);
				}
			}

			// RIGHT EYE
			_splitpath(ff7_eyes[ff7_curr_eye_index].static_right_eye_filename, NULL, NULL, filename, ext);

			_snprintf(directpath, sizeof(directpath), "%s/%s/flevel/eye_%sr_%d.TEX", basedir, direct_mode_path.c_str(), curr_model_name, right_eye_index);
			if (ext_right_eye_found = fileExists(directpath))
				_snprintf(ff7_externals.field_models_eye_blink_buffer[ff7_curr_eye_index].static_right_eye_filename, 1024, "eye_%sr_%d%s", curr_model_name, right_eye_index, ext);
			else
			{
				if (right_eye_index > 2 && (trace_all || trace_direct || trace_opcodes)) ffnx_trace("subcode[EYETX]: Custom right eye texture not found: %s\n", directpath);

				_snprintf(directpath, sizeof(directpath), "%s/%s/flevel/%s_%d.TEX", basedir, direct_mode_path.c_str(), filename, right_eye_index);
				if (ext_right_eye_found = fileExists(directpath))
					_snprintf(ff7_externals.field_models_eye_blink_buffer[ff7_curr_eye_index].static_right_eye_filename, 1024, "%s_%d%s", filename, right_eye_index, ext);
				else
				{
					if (right_eye_index > 2 && (trace_all || trace_direct || trace_opcodes)) ffnx_trace("subcode[EYETX]: Custom right eye texture not found: %s\n", directpath);
					_snprintf(ff7_externals.field_models_eye_blink_buffer[ff7_curr_eye_index].static_right_eye_filename, 1024, "%s%s", filename, ext);
				}
			}

			// Reload TEX data in memory
			if(animation_data[curr_model_id].static_left_eye_tex) ff7_externals.field_unload_model_tex(animation_data[curr_model_id].static_left_eye_tex);
			if(animation_data[curr_model_id].static_right_eye_tex) ff7_externals.field_unload_model_tex(animation_data[curr_model_id].static_right_eye_tex);
			ff7_externals.field_load_model_eye_tex(&ff7_externals.field_models_eye_blink_buffer[ff7_curr_eye_index], &animation_data[curr_model_id]);

			// Restore original curr_eye_index
			ff7_curr_eye_index = animation_data[curr_model_id].eye_texture_idx;

			// MOUTH
			if (ff7_curr_eye_index < 9)
			{
				char* char_name = strtok(filename, "_");

				_snprintf(directpath, sizeof(directpath), "%s/%s/flevel/mouth_%s_%d.TEX", basedir, direct_mode_path.c_str(), curr_model_name, mouth_index);
				if (ff7_mouths[ff7_curr_eye_index].has_mouth = fileExists(directpath))
					_snprintf(ff7_mouths[ff7_curr_eye_index].mouth_tex_filename, 1024, "mouth_%s_%d%s", curr_model_name, mouth_index, ext);
				else
				{
					if (mouth_index > 0 && (trace_all || trace_direct || trace_opcodes)) ffnx_trace("subcode[EYETX]: Custom mouth texture not found: %s\n", directpath);

					_snprintf(directpath, sizeof(directpath), "%s/%s/flevel/%s_mouth_%d.TEX", basedir, direct_mode_path.c_str(), filename, mouth_index);
					if (ff7_mouths[ff7_curr_eye_index].has_mouth = fileExists(directpath))
						_snprintf(ff7_mouths[ff7_curr_eye_index].mouth_tex_filename, 1024, "%s_mouth_%d%s", char_name, mouth_index, ext);
					else if (mouth_index > 0 && (trace_all || trace_direct || trace_opcodes)) ffnx_trace("subcode[EYETX]: Custom mouth texture not found: %s\n", directpath);
				}

				// Prepare mouth tex object
				if (ff7_mouths[ff7_curr_eye_index].has_mouth)
				{
					struc_3 tex_mouth_info;
					ff7_externals.create_struc_3_info_sub_67455E(&tex_mouth_info);
					tex_mouth_info.base_directory = (uint32_t)ff7_externals.field_unk_909288;
					tex_mouth_info.file_context.use_lgp = 1;
					tex_mouth_info.file_context.lgp_num = 1;
					tex_mouth_info.file_context.name_mangler = 0;
					ff7_mouths[ff7_curr_eye_index].mouth_tex = ff7_externals.field_load_model_tex(0, 0, ff7_mouths[ff7_curr_eye_index].mouth_tex_filename, &tex_mouth_info, common_externals.get_game_object());
				}
				else
				{
					if (ff7_mouths[ff7_curr_eye_index].mouth_tex) ff7_externals.field_unload_model_tex(ff7_mouths[ff7_curr_eye_index].mouth_tex);
					ff7_mouths[ff7_curr_eye_index].mouth_tex = NULL;
				}
				ff7_mouths[ff7_curr_eye_index].current_mouth_idx = mouth_index;
			}

			// Index is also treated as blink mode, if higher than 2 then "fake a closed eyes" in order to reload textures
			if (left_eye_index <= 2 || right_eye_index <= 2)
			{
				ff7_externals.field_model_blink_data_D000C8->blink_left_eye_mode = left_eye_index;
				ff7_externals.field_model_blink_data_D000C8->blink_right_eye_mode = right_eye_index;
			}
			else if (ext_left_eye_found || ext_right_eye_found)
			{
				ff7_externals.field_model_blink_data_D000C8->blink_left_eye_mode = 2;
				ff7_externals.field_model_blink_data_D000C8->blink_right_eye_mode = 2;
			}
			ff7_externals.field_model_blink_data_D000C8->unknown = 0;
			ff7_externals.field_model_blink_data_D000C8->model_id = curr_model_id;
			ff7_externals.field_blink_3d_model_649B50(&animation_data[curr_model_id], ff7_externals.field_model_blink_data_D000C8);

			// Required to force reload the mouth texture
			if (ff7_curr_eye_index < 9 && ff7_head) ff7_head->per_group_hundreds = 1;
		}
	}

	return opcode_old_kawai();
}

int opcode_pc_map_change() {
	if (map_changing)
	{
		byte* level_data = *ff7_externals.field_level_data_pointer;
		uint32_t walkmesh_offset = *(uint32_t*)(level_data + 0x16);
		vertex_3s* triangle_data = (vertex_3s*)(level_data + walkmesh_offset + 8 + 24 * target_triangle);

		// Calculates the centroid of the walkmesh triangle
		int x = (triangle_data[0].x + triangle_data[1].x + triangle_data[2].x) / 3;
		int y = (triangle_data[0].y + triangle_data[1].y + triangle_data[2].y) / 3;

		ff7_externals.modules_global_object->field_model_pos_x = x;
		ff7_externals.modules_global_object->field_model_pos_y = y;
		ff7_externals.modules_global_object->field_model_triangle_id = target_triangle;
		map_changing = false;
	}

	return opcode_old_pc();
}

int field_calc_window_pos(int16_t WINDOW_ID, int16_t X, int16_t Y, int16_t W, int16_t H)
{
	return ff7_externals.sub_630C48(WINDOW_ID, X, ff7_field_center ? Y + 8 : Y, W, H);
}

int field_load_mouth(ff7_polygon_set *polygon_set)
{
	int ret = ff7_externals.field_sub_6A2736(polygon_set);

	if (ff7_curr_eye_index < 9 && polygon_set && ff7_mouths[ff7_curr_eye_index].mouth_tex)
	{
		polygon_set->hundred_data_group_array[polygon_set->numgroups - 1] = ff7_mouths[ff7_curr_eye_index].mouth_tex;
		ff7_head = polygon_set;
	}

	return ret;
}

void field_init()
{
	if (!ff8)
	{
		// Proxies the PC field opcode to reposition the player after a forced map change
		opcode_old_pc = (int (*)())ff7_externals.opcode_pc;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0xA0], (DWORD)&opcode_pc_map_change);

		opcode_old_kawai = (int (*)())ff7_externals.opcode_kawai;
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x28], (DWORD)&opcode_kawai_eye_texture);

		// Proxy the window calculation formula so we can offset windows vertically
		replace_call_function(common_externals.execute_opcode_table[0x50] + 0x174, field_calc_window_pos);

		// Proxy the function to easily obtain the current polygon set for mouth replacement
		replace_call_function((uint32_t)ff7_externals.field_blink_3d_model_649B50 + 0xC4, field_load_mouth);

		// ################################
		// save static eyes names for later
		// ################################
		for(int i = 0; i < 10; i++)
		{
			if (i < 9)
			{
				if (ff7_externals.field_models_eye_blink_buffer[i].has_eyes)
				{
					if (ff7_externals.field_models_eye_blink_buffer[i].static_left_eye_filename)
					{
						ff7_eyes[i].static_left_eye_filename = ff7_externals.field_models_eye_blink_buffer[i].static_left_eye_filename;
						ff7_externals.field_models_eye_blink_buffer[i].static_left_eye_filename = (char*)external_malloc(1024);
						strcpy(ff7_externals.field_models_eye_blink_buffer[i].static_left_eye_filename, ff7_eyes[i].static_left_eye_filename);
					}

					if (ff7_externals.field_models_eye_blink_buffer[i].static_right_eye_filename)
					{
						ff7_eyes[i].static_right_eye_filename = ff7_externals.field_models_eye_blink_buffer[i].static_right_eye_filename;
						ff7_externals.field_models_eye_blink_buffer[i].static_right_eye_filename = (char*)external_malloc(1024);
						strcpy(ff7_externals.field_models_eye_blink_buffer[i].static_right_eye_filename, ff7_eyes[i].static_right_eye_filename);
					}
				}
			}

			ff7_mouths[i].current_mouth_idx = 0;
			ff7_mouths[i].mouth_tex_filename = (char*)external_calloc(sizeof(char), sizeof(basedir) + 1024);
			ff7_mouths[i].mouth_tex = NULL;
		}
	}
}

int map_jump_ff7()
{
	// Restores the original field update code
	memcpy_code(common_externals.update_entities_call, map_patch_storage, 7);

	byte* current_executing_code = get_field_parameter_address<byte>(-1);

	// Inject MAPJUMP coordinates
	memset(current_executing_code, 0, 10);
	*(current_executing_code) = 0x60; // MAPJUMP
	*(WORD*)(current_executing_code + 1) = target_field;

	int (*mapjump)() = (int (*)())common_externals.execute_opcode_table[0x60];
	return mapjump();
}

int map_jump_ff8(byte* entity, int arg)
{
	// Forces the entity to be able to trigger a map jump
	entity[0x175] |= 0x01;

	// Restores the original field update code
	memcpy_code(common_externals.update_entities_call, map_patch_storage, 7);
	map_changing = false;

	// Executes the field script to change map
	int (**field_functions)(byte*, uint32_t) = (int (**)(byte*, uint32_t))common_externals.execute_opcode_table;
	field_functions[0xB9](entity, 0); // KILLTIMER
	field_functions[0x07](entity, target_field); // PSHN_L
	field_functions[0x07](entity, target_triangle); // PSHN_L
	return field_functions[0x5C](entity, 0); // MAPJUMP
}

void field_debug(bool *isOpen)
{
	if (!ImGui::Begin("Field Debug", isOpen, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	if (getmode_cached()->driver_mode != MODE_FIELD)
	{
		ImGui::Text("Not currently on a field.");
		ImGui::End();
		return;
	}

	ImGui::Text("Game Moment: %u", *common_externals.field_game_moment);
	ImGui::Text("Current field ID: %u", *common_externals.current_field_id);
	ImGui::Text("Current triangle ID: %d", *common_externals.current_triangle_id);
	ImGui::Text("Previous field ID: %u", *common_externals.previous_field_id);
	ImGui::Separator();

	// Inputs for changing field map
	ImGui::Text("Switch Field"); ImGui::SetNextItemWidth(100);
	ImGui::InputInt("Field ID", &target_field); ImGui::SameLine(200); ImGui::SetNextItemWidth(100);
	ImGui::InputInt("Triangle ID", &target_triangle);

	if (ImGui::Button("Change") && !map_changing) {
		// Injects a call into where the field entities are checked
		memcpy(map_patch_storage, (void*)common_externals.update_entities_call, 7); // Make a copy of the existing CALL
		patch_code_dword(common_externals.update_entities_call, 0x00E89090); // Places 2 NOPs and a CALL
		replace_call(common_externals.update_entities_call + 2, ff8 ? (void*)&map_jump_ff8 : (void*)&map_jump_ff7);
		map_changing = true;
	}
	ImGui::End();
}
