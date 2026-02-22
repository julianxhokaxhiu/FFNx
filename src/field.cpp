/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include "ff7/field/model.h"

// Data for debug map jumps
int target_triangle = 0;
int target_field = 0;
byte map_patch_storage[7] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // Place to store the original bytes so that we can patch back after a map jump
bool map_changing = false;

// FF7 only
int (*opcode_old_kawai)();
int (*opcode_old_pc)();

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

int opcode_kawai() {
	byte byte_size = get_field_parameter<byte>(0);
	byte subcode = get_field_parameter<byte>(1);

	if (trace_all || trace_opcodes) ffnx_trace("opcode[KAWAI]: byte_size=%u,subcode=0x%x\n", byte_size, subcode);

	field_event_data* event_data = (*ff7_externals.field_event_data_ptr);
	field_animation_data* animation_data = *ff7_externals.field_animation_data_ptr;

	byte curr_entity_id = *ff7_externals.current_entity_id;
	byte curr_model_id = ff7_externals.field_model_id_array[curr_entity_id];

	if (subcode == 0x0) // EYETX
	{
		byte left_eye_index = get_field_parameter<byte>(2);
		byte right_eye_index = get_field_parameter<byte>(3);
		byte mouth_index = get_field_parameter<byte>(4);

		byte curr_eye_index = animation_data[curr_model_id].eye_texture_idx;

		ff7::field::ff7_model_data[curr_model_id].current_mouth_idx = mouth_index;

		if (trace_all || trace_opcodes)
		{
			ffnx_trace("subcode[EYETX]: left_eye_index=%u,right_eye_index=%u,mouth_index=%u,curr_entity_id=%u,curr_model_id=%u,curr_eye_index=%u\n", left_eye_index, right_eye_index, mouth_index, curr_entity_id, curr_model_id, curr_eye_index);
		}
	}
	else if (subcode == 0xD) // SHINE
	{
		ff7::field::ff7_model_data[curr_model_id].is_kawai_active = false;

		if (trace_all || trace_opcodes)
		{
			ffnx_trace("subcode[SHINE]: curr_model_id=%u\n", curr_model_id);
		}
	}

	int ret = opcode_old_kawai();

	if (subcode == 0x1) // TRNSP
	{
		if (event_data[curr_model_id].opcode_params->param_1 == 0)
		{
			ff7::field::ff7_model_data[curr_model_id].is_kawai_active = false;
			ff7::field::ff7_model_data[curr_model_id].do_kawai_repeat = false;
		}
		else
		{
			if (ff7::field::ff7_model_data[curr_model_id].init_kawai_opcode != 0x2)
			{
				ff7::field::ff7_model_data[curr_model_id].init_kawai_opcode = 0x0;
				ff7::field::ff7_model_data[curr_model_id].init_kawai_params = nullptr;
			}

			ff7::field::ff7_model_data[curr_model_id].exec_kawai_opcode = subcode;
			ff7::field::ff7_model_data[curr_model_id].exec_kawai_params = event_data[curr_model_id].opcode_params;
		}

		if (trace_all || trace_opcodes)
		{
			ffnx_trace("subcode[TRNSP]: curr_model_id=%u,activate=%u,opcode_params=0x%X\n", curr_model_id, event_data[curr_model_id].opcode_params->param_1, event_data[curr_model_id].opcode_params);
		}
	}
	else if (subcode == 0x2) // AMBNT
	{
		ff7::field::ff7_model_data[curr_model_id].is_kawai_active = true;
		ff7::field::ff7_model_data[curr_model_id].do_kawai_repeat = true;

		ff7::field::ff7_model_data[curr_model_id].init_kawai_opcode = subcode;
		ff7::field::ff7_model_data[curr_model_id].init_kawai_params = event_data[curr_model_id].opcode_params;

		ff7::field::ff7_model_data[curr_model_id].exec_kawai_opcode = 0x0;
		ff7::field::ff7_model_data[curr_model_id].exec_kawai_params = nullptr;

		if (trace_all || trace_opcodes)
		{
			ffnx_trace("subcode[AMBNT]: curr_model_id=%u,opcode_params=0x%X\n", curr_model_id, event_data[curr_model_id].opcode_params);
		}
	}
	else if (subcode == 0x6) // LIGHT
	{
		if (event_data[curr_model_id].opcode_params->param_1 == 0)
		{
			ff7::field::ff7_model_data[curr_model_id].is_kawai_active = true;
			ff7::field::ff7_model_data[curr_model_id].do_kawai_repeat = event_data[curr_model_id].opcode_params->param_F == 1 && event_data[curr_model_id].opcode_params->param_11 == 1 && event_data[curr_model_id].opcode_params->param_13 == 1;
			ff7::field::ff7_model_data[curr_model_id].init_kawai_opcode = subcode;
			ff7::field::ff7_model_data[curr_model_id].init_kawai_params = event_data[curr_model_id].opcode_params;
		}
		else
		{
			ff7::field::ff7_model_data[curr_model_id].exec_kawai_opcode = subcode;
			ff7::field::ff7_model_data[curr_model_id].exec_kawai_params = event_data[curr_model_id].opcode_params;
		}

		if (trace_all || trace_opcodes)
		{
			ffnx_trace("subcode[LIGHT]: curr_model_id=%u,activate=%u,opcode_params=0x%X,opcode_repeats=%u\n", curr_model_id, event_data[curr_model_id].opcode_params->param_1, event_data[curr_model_id].opcode_params, ff7::field::ff7_model_data[curr_model_id].do_kawai_repeat);
		}
	}
	else if (subcode == 0x7) // UNKNOWN7
	{
		if (event_data[curr_model_id].opcode_params->param_1 == 0)
		{
			ff7::field::ff7_model_data[curr_model_id].is_kawai_active = true;
			ff7::field::ff7_model_data[curr_model_id].do_kawai_repeat = true;
			ff7::field::ff7_model_data[curr_model_id].init_kawai_opcode = subcode;
			ff7::field::ff7_model_data[curr_model_id].init_kawai_params = event_data[curr_model_id].opcode_params;
		}
		else
		{
			ff7::field::ff7_model_data[curr_model_id].exec_kawai_opcode = subcode;
			ff7::field::ff7_model_data[curr_model_id].exec_kawai_params = event_data[curr_model_id].opcode_params;
		}

		if (trace_all || trace_opcodes)
		{
			ffnx_trace("subcode[UNKNOWN7]: curr_model_id=%u,activate=%u,opcode_params=0x%X\n", curr_model_id, event_data[curr_model_id].opcode_params->param_1, event_data[curr_model_id].opcode_params);
		}
	}
	else if (subcode == 0x8 || subcode == 0x9) // UNKNOWN9
	{
		if (event_data[curr_model_id].opcode_params->param_1 == 0)
		{
			ff7::field::ff7_model_data[curr_model_id].is_kawai_active = true;
			ff7::field::ff7_model_data[curr_model_id].do_kawai_repeat = true;
			ff7::field::ff7_model_data[curr_model_id].init_kawai_opcode = subcode;
			ff7::field::ff7_model_data[curr_model_id].init_kawai_params = event_data[curr_model_id].opcode_params;
		}
		else
		{
			ff7::field::ff7_model_data[curr_model_id].exec_kawai_opcode = subcode;
			ff7::field::ff7_model_data[curr_model_id].exec_kawai_params = event_data[curr_model_id].opcode_params;
		}

		if (trace_all || trace_opcodes)
		{
			ffnx_trace("subcode[UNKNOWN9]: curr_model_id=%u,activate=%u,opcode_params=0x%X\n", curr_model_id, event_data[curr_model_id].opcode_params->param_1, event_data[curr_model_id].opcode_params);
		}
	}

	return ret;
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

int ff7_calc_opcode_type_2_fade_color(int16_t r, int16_t g, int16_t b)
{
	uint8_t fade_b = (256 - b) * (256 - ff7_externals.modules_global_object->fade_adjustment / 2) / 256;
	uint8_t fade_g = (256 - g) * (256 - ff7_externals.modules_global_object->fade_adjustment / 2) / 256;
	uint8_t fade_r = (256 - r) * (256 - ff7_externals.modules_global_object->fade_adjustment / 2) / 256;

	return 0xFF000000 | (fade_r << 16) | (fade_g << 8) | fade_b;
}

int ff8_field_init_from_file(int unk1, int unk2, int unk3, int unk4)
{
	int ret = ff8_externals.field_scripts_init(unk1, unk2, unk3, unk4);

	// Current triangle id address changes on each field on FF8
	// Loop through objects until we find the one that has a valid triangle ID
	for(int i = 0; i < MAXBYTE; i++)
	{
		common_externals.current_triangle_id = (int16_t*)(*(uint32_t *)ff8_externals.field_state_others + 0x264 * i + 0x1FA);
		if (*common_externals.current_triangle_id != 0) break;
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
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0x28], (DWORD)&opcode_kawai);

		// Proxy the window calculation formula so we can offset windows vertically
		replace_call_function(common_externals.execute_opcode_table[0x50] + 0x174, field_calc_window_pos);

		// Proxy FADE opcode color calculation
		byte opcode_fade_patch[] = {0x0F, 0xBF, 0x45, 0x10, 0x50, 0x0F, 0xBF, 0x45, 0x0C, 0x50, 0x0F, 0xBF, 0x45, 0x08, 0x50, 0xE8, 0xFF, 0xFF, 0xFF, 0xFF, 0x83, 0xC4, 0x0C};
		memcpy_code(ff7_externals.field_calc_fade_color_sub_63AE66 + 0x244, opcode_fade_patch, sizeof(opcode_fade_patch));
		memset_code(ff7_externals.field_calc_fade_color_sub_63AE66 + 0x244 + sizeof(opcode_fade_patch), 0x90, 0xE1 - sizeof(opcode_fade_patch));
		replace_call_function(ff7_externals.field_calc_fade_color_sub_63AE66 + 0x244 + 0xF, ff7_calc_opcode_type_2_fade_color);

		// Init custom eyes and mouths structs
		for(int i = 0; i < FF7_MAX_NUM_MODEL_ENTITIES; i++)
		{
			ff7::field::ff7_model_data[i].left_eye_tex_filename = (char*)external_calloc(sizeof(char), sizeof(basedir) + 1024);
			ff7::field::ff7_model_data[i].right_eye_tex_filename = (char*)external_calloc(sizeof(char), sizeof(basedir) + 1024);
			ff7::field::ff7_model_data[i].current_mouth_idx = 0;
			ff7::field::ff7_model_data[i].mouth_tex_filename = (char*)external_calloc(sizeof(char), sizeof(basedir) + 1024);
			ff7::field::ff7_model_data[i].mouth_tex = NULL;
		}
	}
	else
	{
		// Proxy the field init file read
		replace_call_function(ff8_externals.read_field_data + (JP_VERSION ? 0xEDC : 0xE49), ff8_field_init_from_file);
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
	if (common_externals.current_triangle_id != 0) ImGui::Text("Current triangle ID: %d", *common_externals.current_triangle_id);
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
