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

#include "ff7/defs.h"
#include "ff7/field/model.h"
#include "ff7/field/kawai.h"
#include "ff8/field_moments.h"

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
	const byte byte_size = get_field_parameter<byte>(0);
	const byte subcode = get_field_parameter<byte>(1);
	const byte entity_id = *ff7_externals.current_entity_id;
	const byte model_id = ff7_externals.field_model_id_array[entity_id];

	if (trace_all || trace_opcodes) ffnx_trace("opcode[KAWAI]: byte_size=%u,subcode=0x%x\n", byte_size, subcode);

	const size_t minimum_size = subcode == 0 ? 6 : (subcode == 1 ? 4 : 3);
	if (byte_size < minimum_size || (model_id != MAXBYTE && model_id >= FF7_MAX_NUM_MODEL_ENTITIES))
	{
		ffnx_error("KAWAI: invalid command size %u or model id %u for opcode %02X\n", byte_size, model_id, subcode);
		auto& position = ff7_externals.field_curr_script_position[entity_id];
		position = static_cast<uint16_t>(position + std::max<int>(byte_size, 3));
		return 0;
	}
	if (model_id == MAXBYTE)
	{
		ff7_externals.field_curr_script_position[entity_id] += byte_size;
		return 0;
	}

	std::array<uint8_t, 32> kawai_params{};
	const size_t parameter_count = ff7::field::kawai::handles(subcode) ?
		std::min<size_t>(byte_size - 3, kawai_params.size()) : 0;
	for (size_t i = 0; i < parameter_count; ++i) kawai_params[i] = get_field_parameter<byte>(2 + i);

	if (subcode == 0) // The native EYETX handler immediately invokes advanced blinking.
	{
		ff7::field::ff7_model_data[model_id].current_mouth_idx = get_field_parameter<byte>(4);
	}

	const int ret = opcode_old_kawai();
	ff7::field::queue_kawai_effect(model_id, subcode, std::span(kawai_params).first(parameter_count));
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
	field_autosize_window_geometry_changed(WINDOW_ID, W, H);
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

static void field_progress_debug()
{
	static int target_game_moment = *common_externals.field_game_moment;
	static ImGuiTextFilter moment_filter;
	if (ImGui::IsWindowAppearing())
		target_game_moment = *common_externals.field_game_moment;

	ImGui::TextUnformatted(ff8 ? "Set Game Moment" : "Set Game Progress");
	ImGui::SetNextItemWidth(100);
	ImGui::InputInt(ff8 ? "Moment ID" : "Progress ID", &target_game_moment);

	if (ff8)
	{
		char preview[128];
		snprintf(preview, sizeof(preview), "%d (Custom)", target_game_moment);
		for (const auto &moment : ff8_field_moments)
		{
			if (moment.id == target_game_moment)
			{
				snprintf(preview, sizeof(preview), "%u - %s", moment.id, moment.description);
				break;
			}
		}

		ImGui::SetNextItemWidth(400);
		if (ImGui::BeginCombo("Known moments", preview, ImGuiComboFlags_HeightLarge))
		{
			moment_filter.Draw("Filter (ID, disc, location)", 300);
			ImGui::BeginChild("Moment list", ImVec2(0, 250));
			for (const auto &moment : ff8_field_moments)
			{
				char label[128];
				snprintf(label, sizeof(label), "%u - %s", moment.id, moment.description);
				if (!moment_filter.PassFilter(label)) continue;

				bool selected = moment.id == target_game_moment;
				if (ImGui::Selectable(label, selected))
				{
					target_game_moment = moment.id;
					ImGui::CloseCurrentPopup();
				}
				if (selected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndChild();
			ImGui::EndCombo();
		}
	}

	bool valid_moment = target_game_moment >= 0 && target_game_moment <= UINT16_MAX;
	if (!valid_moment) ImGui::TextUnformatted(ff8 ? "Moment ID must be between 0 and 65535." : "Progress ID must be between 0 and 65535.");
	ImGui::BeginDisabled(!valid_moment);
	if (ImGui::Button(ff8 ? "Set Moment" : "Set Progress"))
	{
		*common_externals.field_game_moment = static_cast<WORD>(target_game_moment);
		ffnx_info("Field Debug: set %s to %u.\n", ff8 ? "FF8 game moment" : "FF7 game progress", *common_externals.field_game_moment);
	}
	ImGui::EndDisabled();
	ImGui::TextWrapped("Changes only story progression, not the field, disc or other story flags.");
	ImGui::Separator();
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

	ImGui::Text(ff8 ? "Game Moment: %u" : "Game Progress: %u", *common_externals.field_game_moment);
	ImGui::Text("Current field ID: %u", *common_externals.current_field_id);
	if (common_externals.current_triangle_id != 0) ImGui::Text("Current triangle ID: %d", *common_externals.current_triangle_id);
	ImGui::Text("Previous field ID: %u", *common_externals.previous_field_id);
	ImGui::Separator();

	field_progress_debug();

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
