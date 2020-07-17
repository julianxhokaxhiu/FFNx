/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

#include <stdint.h>

#include "../ff7.h"
#include "../log.h"
#include "../gamepad.h"

// MDEF fix
uint32_t get_equipment_stats(uint32_t party_index, uint32_t type)
{
	uint32_t character = ff7_externals.party_member_to_char_map[ff7_externals.savemap->party_members[party_index]];

	switch(type)
	{
		case 0:
			return ff7_externals.weapon_data_array[ff7_externals.savemap->chars[character].equipped_weapon].attack_stat;
			break;
		case 1:
			return ff7_externals.armor_data_array[ff7_externals.savemap->chars[character].equipped_armor].defense_stat;
			break;
		case 2:
			return 0;
			break;
		case 3:
			return mdef_fix ? ff7_externals.armor_data_array[ff7_externals.savemap->chars[character].equipped_armor].mdef_stat : 0;
			break;

		default: return 0;
	}
}

char *kernel2_sections[20];
uint32_t kernel2_section_counter;

void kernel2_reset_counters()
{
	uint32_t i;

	if(trace_all) trace("kernel2 reset\n");

	for(i = 0; i < kernel2_section_counter; i++) external_free(kernel2_sections[i]);

	kernel2_section_counter = 0;
}

char *kernel2_add_section(uint32_t size)
{
	char *ret = (char*)external_malloc(size);

	if(trace_all) trace("kernel2 add section %i (%i)\n", kernel2_section_counter, size);

	kernel2_sections[kernel2_section_counter++] = ret;

	return ret;
}

char *kernel2_get_text(uint32_t section_base, uint32_t string_id, uint32_t section_offset)
{
	char *section = kernel2_sections[section_base + section_offset];

	if(trace_all) trace("kernel2 get text (%i+%i:%i)\n", section_base, section_offset, string_id);
	
	return &section[((WORD *)section)[string_id]];
}

void ff7_wm_activateapp(bool hasFocus)
{

}

int ff7_get_gamepad()
{
	if (!gamepad.Refresh())
		return FALSE;

	return TRUE;
}

struct ff7_gamepad_status* ff7_update_gamepad_status()
{
	if (!gamepad.Refresh()) return 0;

	ff7_externals.gamepad_status->pos_x = gamepad.leftStickX;
	ff7_externals.gamepad_status->pos_y = gamepad.leftStickY;
	ff7_externals.gamepad_status->field_30 = (gamepad.leftStickY > 0.5f) || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_UP); // UP
	ff7_externals.gamepad_status->field_34 = (gamepad.leftStickY < -0.5f) || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_DOWN); // DOWN
	ff7_externals.gamepad_status->field_38 = (gamepad.leftStickX < -0.5f) || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_LEFT); // LEFT
	ff7_externals.gamepad_status->field_3C = (gamepad.leftStickX > 0.5f) || gamepad.IsPressed(XINPUT_GAMEPAD_DPAD_RIGHT); // RIGHT
	ff7_externals.gamepad_status->button1 = gamepad.IsPressed(XINPUT_GAMEPAD_X); // Square
	ff7_externals.gamepad_status->button2 = gamepad.IsPressed(XINPUT_GAMEPAD_A); // Cross
	ff7_externals.gamepad_status->button3 = gamepad.IsPressed(XINPUT_GAMEPAD_B); // Circle
	ff7_externals.gamepad_status->button4 = gamepad.IsPressed(XINPUT_GAMEPAD_Y); // Triangle
	ff7_externals.gamepad_status->button5 = gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_SHOULDER); // L1
	ff7_externals.gamepad_status->button6 = gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER); // R1
	ff7_externals.gamepad_status->button7 = gamepad.leftTrigger > 0.85f;
	ff7_externals.gamepad_status->button8 = gamepad.rightTrigger > 0.85f;
	ff7_externals.gamepad_status->button9 = gamepad.IsPressed(XINPUT_GAMEPAD_BACK); // SELECT
	ff7_externals.gamepad_status->button10 = gamepad.IsPressed(XINPUT_GAMEPAD_START); // START
	ff7_externals.gamepad_status->button11 = gamepad.IsPressed(XINPUT_GAMEPAD_LEFT_THUMB); // L3
	ff7_externals.gamepad_status->button12 = gamepad.IsPressed(XINPUT_GAMEPAD_RIGHT_THUMB); // R3
	ff7_externals.gamepad_status->button13 = gamepad.IsPressed(0x400); // PS Button
    
    return ff7_externals.gamepad_status;
}
