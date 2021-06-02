/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2021 Julian Xhokaxhiu                                   //
//    Copyright (C) 2021 Cosmos                                             //
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

#include "field.h"

// Data for debug map jumps
int target_triangle = 0;
int target_field = 0;
byte map_patch_storage[7] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // Place to store the original bytes so that we can patch back after a map jump
bool map_changing = false;

// FF7 only
int (*old_pc)();
short* pending_x;
short* pending_y;
short* pending_triangle;

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

int script_PC_map_change() {
	if (map_changing)
	{
		byte* level_data = *ff7_externals.field_level_data_pointer;
		uint32_t walkmesh_offset = *(uint32_t*)(level_data + 0x16);
		vertex_3s* triangle_data = (vertex_3s*)(level_data + walkmesh_offset + 8 + 24 * target_triangle);

		// Calculates the centroid of the walkmesh triangle
		int x = (triangle_data[0].x + triangle_data[1].x + triangle_data[2].x) / 3;
		int y = (triangle_data[0].y + triangle_data[1].y + triangle_data[2].y) / 3;

		*pending_x = x;
		*pending_y = y;
		*pending_triangle = target_triangle;
		map_changing = false;
	}

	return old_pc();
}

int field_calc_window_pos(int16_t WINDOW_ID, int16_t X, int16_t Y, int16_t W, int16_t H)
{
	return ff7_externals.sub_630C48(WINDOW_ID, X, ff7_center_fields ? Y + 8 : Y, W, H);
}

void field_init()
{
	if (!ff8)
	{
		// Proxies the PC field opcode to reposition the player after a forced map change
		old_pc = (int (*)())common_externals.execute_opcode_table[0xA0];
		patch_code_dword((uint32_t)&common_externals.execute_opcode_table[0xA0], (DWORD)&script_PC_map_change);

		// Proxy the window calculation formula so we can offset windows vertically
		replace_call_function(common_externals.execute_opcode_table[0x50] + 0x174, field_calc_window_pos);

		pending_x = (short*)get_absolute_value(ff7_externals.sub_408074, 0x5D); // 0xCC0D8C
		pending_y = pending_x + 1; // 0xCC0D8E
		pending_triangle = pending_x + 15; // 0xCC0DAA
	}
}

int map_jump_ff7()
{
	// Restores the original field update code
	memcpy_code(common_externals.update_entities_call, map_patch_storage, 7);

	byte* current_executing_code = (byte*)(ff7_externals.field_array_1[*ff7_externals.current_entity_id] + *ff7_externals.field_ptr_1);

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

	ImGui::Text("Current field ID: %d", *common_externals.current_field_id);
	ImGui::Text("Previous field ID: %d", *common_externals.previous_field_id);
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
