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

#include <algorithm>
#include <imgui.h>
#include <stdint.h>

#include "globals.h"
#include "common.h"
#include "patch.h"
#include "log.h"
#include "utils.h"
#include "gl.h"
#include "renderer.h"

#include "field.h"

#include "ff7/defs.h"
#include "ff7/field/kawai_eyetx.h"
#include "ff7/field/kawai.h"
#include "ff7/widescreen.h"
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
		if (trace_all || trace_opcodes)
			ffnx_trace("opcode[EYETX]: field_id=%u,entity=%u,model=%u,script_position=%u,left=%u,right=%u,mouth=%u,advanced=%d,movie=%d\n",
				*common_externals.current_field_id, entity_id, model_id,
				ff7_externals.field_curr_script_position[entity_id],
				get_field_parameter<byte>(2), get_field_parameter<byte>(3), get_field_parameter<byte>(4),
				ff7_advanced_blinking, ff7_externals.movie_object->is_playing);
		ff7::field::ff7_field_set_mouth_index(model_id, get_field_parameter<byte>(4));
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

static uint16_t (*field_fade_screen_original)();
static void (*field_draw_fade_original)(ff7_game_obj*);
static uint32_t* field_final_fade_color;
static uint32_t* field_fade_draw_flags;
static uint32_t* field_system_fade;
static uint32_t field_subtractive_fade_color;
static uint16_t field_subtractive_fade_field_id;

static uint32_t ff7_subtractive_fade_color(int16_t r, int16_t g, int16_t b, int16_t adjustment, bool inverse = true)
{
	const int progress = std::clamp<int>(adjustment, 0, 255);
	const auto channel = [progress, inverse](int16_t value) -> uint32_t {
		const int color = std::clamp<int>(value, 0, 255);
		return ((inverse ? 255 - color : color) * progress + 127) / 255;
	};
	const uint32_t fade_r = channel(r);
	const uint32_t fade_g = channel(g);
	const uint32_t fade_b = channel(b);

	return 0xFF000000 | (fade_r << 16) | (fade_g << 8) | fade_b;
}

static uint16_t field_fade_screen()
{
	const uint16_t fade_type = ff7_externals.modules_global_object->fade_type;
	const bool subtractive = fade_type == 2 || fade_type == 12 || (fade_type == 1 && !*field_system_fade);
	field_subtractive_fade_color = 0;
	const uint16_t result = field_fade_screen_original();

	if (subtractive)
	{
		const auto& fade = *ff7_externals.modules_global_object;
		// NFADE supplies the subtraction color directly and interpolates RGB, not opacity.
		field_subtractive_fade_color = ff7_subtractive_fade_color(fade.fade_r, fade.fade_g, fade.fade_b,
			fade_type == 12 ? 255 : static_cast<int16_t>(fade.fade_adjustment), fade_type != 12);
		field_subtractive_fade_field_id = *common_externals.current_field_id;
		// Keep native timing and state changes, but suppress both PC tinting paths.
		*field_final_fade_color = 0xFFFFFFFF;
		field_fade_draw_flags[0] = 0;
	}

	return result;
}

static void field_draw_fade(ff7_game_obj* game_object)
{
	field_draw_fade_original(game_object);
	if (*field_system_fade || field_subtractive_fade_field_id != *common_externals.current_field_id ||
		(field_subtractive_fade_color & 0xFFFFFF) == 0) return;

	// Drain only scene draws; dialogue has not been submitted at this point.
	gl_draw_sorted_deferred(true);

	struct driver_state saved_state;
	gl_save_state(&saved_state);
	const bool time_filter = newRenderer.isTimeFilterEnabled();
	newRenderer.setTimeFilterEnabled(false);
	gl_bind_texture_set(nullptr);
	current_state.kawai_effect = {};
	gl_set_blend_func(static_cast<uint32_t>(RendererBlendMode::BLEND_SUB));
	internal_set_renderstate(V_NOCULL, true, (game_obj*)game_object);
	internal_set_renderstate(V_DEPTHTEST, false, (game_obj*)game_object);
	internal_set_renderstate(V_DEPTHMASK, false, (game_obj*)game_object);
	internal_set_renderstate(V_ALPHATEST, false, (game_obj*)game_object);

	const float x = widescreen_enabled ? static_cast<float>(wide_viewport_x) : 0.0f;
	const float width = widescreen_enabled ? static_cast<float>(wide_viewport_width) : static_cast<float>(game_width);
	const float height = static_cast<float>(game_height);
	struct nvertex vertices[] = {
		{x, 0.0f, 0.0f, 1.0f, field_subtractive_fade_color, 0, 0.0f, 0.0f},
		{x, height, 0.0f, 1.0f, field_subtractive_fade_color, 0, 0.0f, 0.0f},
		{x + width, 0.0f, 0.0f, 1.0f, field_subtractive_fade_color, 0, 0.0f, 0.0f},
		{x + width, height, 0.0f, 1.0f, field_subtractive_fade_color, 0, 0.0f, 0.0f},
	};
	WORD indices[] = {0, 1, 2, 1, 3, 2};
	gl_draw_indexed_primitive(static_cast<uint32_t>(RendererPrimitiveType::PT_TRIANGLES), TLVERTEX,
		vertices, nullptr, 4, indices, 6, nullptr, nullptr, nullptr, false, false);

	gl_load_state(&saved_state);
	newRenderer.setTimeFilterEnabled(time_filter);
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

		field_fade_screen_original = (uint16_t(*)())ff7_externals.field_fade_screen_sub_63B84B;
		field_draw_fade_original = (void(*)(ff7_game_obj*))get_relative_call(ff7_externals.field_draw_everything, 0x369);
		field_final_fade_color = (uint32_t*)get_absolute_value(ff7_externals.field_calc_fade_color_sub_63AE66, 0x326);
		field_fade_draw_flags = (uint32_t*)get_absolute_value((uint32_t)field_draw_fade_original, 0xB);
		field_system_fade = (uint32_t*)get_absolute_value(ff7_externals.field_calc_fade_color_sub_63AE66, 0x35);
		replace_call_function(ff7_externals.field_loop_sub_63C17F + 0x634, field_fade_screen);
		replace_call_function(ff7_externals.field_draw_everything + 0x369, field_draw_fade);

		ff7::field::ff7_field_initialize_eyetx();
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
