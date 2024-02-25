/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
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
#include "vibration.h"
#include "../ff8.h"
#include "../log.h"
#include "../gamepad.h"
#include "../joystick.h"
#include "../patch.h"
#include "../vibration.h"
#include <xxhash.h>

char vibrateName[32] = "";
ff8_menu_config_input_keymap menu_options_keymap_desc[11];

int ff8_vibrate_capability(int port)
{
	if (trace_all) ffnx_trace("%s port=%d\n", __func__, port);

	return nxVibrationEngine.canRumble();
}

int ff8_game_is_paused(int callback)
{
	if (trace_all) ffnx_trace("%s callback=%p\n", __func__, callback);

	if (nxVibrationEngine.canRumble())
	{
		// Reroute to the vibrate pause menu
		int ret = ff8_externals.pause_menu_with_vibration(callback);

		int keyon = ff8_externals.get_keyon(0, 0);
		// Press pause again
		if ((keyon & 0x800) != 0) {
			return 1;
		}

		return ret;
	}

	return ff8_externals.pause_menu(callback);
}

void vibrate(int left, int right)
{
	const int port = 0;

	if (trace_all || trace_gamepad) ffnx_trace("%s left=%d right=%d vibrate_option_enabled=%d\n", __func__, left, right, ff8_externals.gamepad_states->state_by_port[port].vibrate_option_enabled);

	if (ff8_externals.gamepad_states->state_by_port[port].vibrate_option_enabled == 255)
	{
		nxVibrationEngine.setLeftMotorValue(left);
		nxVibrationEngine.setRightMotorValue(right);
		nxVibrationEngine.rumbleUpdate();
	}
}

void apply_vibrate_calc(char port, int left, int right)
{
	ff8_gamepad_vibration_state *gamepad_state = ff8_externals.gamepad_states->state_by_port;
	if (left < 0) left = 0;
	if (right < 0) right = 0;

	// Keep the game implementation
	gamepad_state[port & 0x1].vibration_active = 1;
	gamepad_state[port & 0x1].left_motor_speed = left;
	gamepad_state[port & 0x1].right_motor_speed = right;

	vibrate(left, right);
}

const char *set_name(const char *name)
{
	return strncpy(vibrateName, name, sizeof(vibrateName));
}

size_t vibrate_data_size(const uint8_t *data)
{
	const uint32_t *header = (const uint32_t *)data;
	uint32_t max_pos = 0;

	while (header - (const uint32_t *)data < 64) {
		if (*header > max_pos) {
			max_pos = *header;
		}
		header++;
	}

	if (max_pos == 0) {
		return 0;
	}

	const uint16_t *it = (const uint16_t *)(data + max_pos);
	uint16_t left_size = it[0], right_size = it[1];

	return max_pos + 4 + left_size + right_size;
}

const char *vibrate_data_name(const uint8_t *data)
{
	if (trace_all || trace_gamepad) ffnx_trace("%s: data=0x%X\n", __func__, data);

	if (data == *ff8_externals.vibrate_data_battle)
	{
		return set_name("battle");
	}

	if (data == *ff8_externals.vibrate_data_main)
	{
		return set_name("main");
	}

	if (getmode_cached()->driver_mode == MODE_WORLDMAP)
	{
		return set_name("world");
	}

	size_t size = vibrate_data_size(data);
	if (size == 0)
	{
		return nullptr;
	}

	XXH64_hash_t hash = XXH3_64bits(data, size);

	if (trace_all || trace_gamepad) ffnx_trace("%s: hash=0x%llX\n", __func__, hash);

	switch (hash)
	{
	case 0x3A8B11844E20E005:
		return set_name("field");
	case 0x12ED0CB959EE1D06:
		return set_name("battle_quezacotl");
	case 0x5609D2CFA36FAA0A:
		return set_name("battle_shiva");
	case 0xCEFDFB6A96824726:
		return set_name("battle_ifrit");
	case 0x453F6DFFE0C9349F:
		return set_name("battle_siren");
	case 0xEBD9E3F6260E3959:
		return set_name("battle_brothers");
	case 0x9BFBB2F64D8D7481:
		return set_name("battle_diablos");
	case 0xBB77D755D1E18AAE:
		return set_name("battle_carbuncle");
	case 0x6CE5937D628A4A8C:
		return set_name("battle_leviathan");
	case 0x9FC2EA78F38A8DB2:
		return set_name("battle_pandemona");
	case 0xA0CB223A3095EF8E:
		return set_name("battle_cerberus");
	case 0x4618E0051D8EB26A:
		return set_name("battle_alexander");
	case 0x8226A772A7F38BE3:
		return set_name("battle_helltrain");
	case 0xB0A0606DC821934F:
		return set_name("battle_bahamut");
	case 0x9B9C719D7B06C9F6:
		return set_name("battle_cactuar");
	case 0xC5C7EEC0C8CA65F0:
		return set_name("battle_tonberry");
	case 0x52E0D8894C20D609:
		return set_name("battle_eden");
	case 0x928D553922FE8248:
		return set_name("battle_odin");
	case 0x79AC9002E7CD9699:
		return set_name("battle_gilgamesh_zantetsuken");
	case 0x50BE9269B62CFD2E:
		return set_name("battle_gilgamesh_excalipoor");
	case 0x182DC6DE431B0B59:
		return set_name("battle_phoenix");
	case 0xFCF682172921F687:
		return set_name("battle_moomba");
	case 0x2870C502E6C869CA:
		return set_name("battle_minimog");
	case 0x34A36AEB4BDB2873:
		return set_name("battle_boko_chocofire");
	case 0x71C550B41E196BF5:
		return set_name("battle_boko_chocoflare");
	case 0x58D0E469C7393C42:
		return set_name("battle_boko_chocometeor");
	case 0x711807BEA87AE308:
		return set_name("battle_boko_chocobocle");
	case 0x66A9AC8656CDA67E:
		return set_name("battle_meteor");
	case 0x8CB04A5C704B20BB:
		return set_name("battle_apocalypse");
	case 0xE443180422A17638:
		return set_name("battle_ultima");
	}

	snprintf(vibrateName, sizeof(vibrateName), "%llX", hash);

	return vibrateName;
}

uint32_t ff8_set_vibration_replace_id = 0;

int ff8_set_vibration(const uint8_t *data, int set, int intensity)
{
	if (trace_all || trace_gamepad) ffnx_trace("%s: set=%d intensity=%d\n", __func__, set, intensity);

	const char *name = vibrate_data_name(data);

	if (name != nullptr)
	{
		const uint8_t *dataOverride = nxVibrationEngine.vibrateDataOverride(name);
		if (dataOverride != nullptr)
		{
			data = dataOverride;
		}
		else if (data != *ff8_externals.vibrate_data_main && getmode_cached()->driver_mode == MODE_WORLDMAP)
		{
			// Disable vibration
			return 0;
		}
	}

	unreplace_function(ff8_set_vibration_replace_id);
	int ret = ((int(*)(const uint8_t*,int,int))ff8_externals.set_vibration)(data, set, intensity);
	rereplace_function(ff8_set_vibration_replace_id);

	return ret;
}

ff8_draw_menu_sprite_texture_infos *alternate_pause_battle(int a1, ff8_draw_menu_sprite_texture_infos *draw_infos)
{
	if (nxVibrationEngine.canRumble())
	{
		uint16_t keyon = *(uint16_t *)(*(int *)ff8_externals.battle_menu_state + 0x14);
		uint8_t port = 0;
		int vibrate_state = ((int(*)(uint8_t))ff8_externals.vibration_get_is_enabled)(port);

		if ((keyon & 0xF060) != 0)
		{
			vibrate_state = uint8_t(-(vibrate_state != 255));
			((void(*)(uint8_t,int))ff8_externals.vibration_set_is_enabled)(port, vibrate_state);
		}

		*ff8_externals.is_alternative_pause_menu = 1;
		int previous_value = *ff8_externals.pause_menu_option_state;
		*ff8_externals.pause_menu_option_state = vibrate_state;

		ff8_draw_menu_sprite_texture_infos *ret = ((ff8_draw_menu_sprite_texture_infos*(*)(int,ff8_draw_menu_sprite_texture_infos*))ff8_externals.battle_pause_window_sub_4CD350)(a1, draw_infos);

		// Reset values to prevent any issues outside the pause menu
		*ff8_externals.is_alternative_pause_menu = 0;
		*ff8_externals.pause_menu_option_state = previous_value;

		return ret;
	}

	return ((ff8_draw_menu_sprite_texture_infos*(*)(int,ff8_draw_menu_sprite_texture_infos*))ff8_externals.battle_pause_window_sub_4CD350)(a1, draw_infos);
}

ff8_draw_menu_sprite_texture_infos *battle_pause_show_quit_game_text(int a1, ff8_draw_menu_sprite_texture_infos *draw_infos, int x, int y, uint8_t *text_data, int a6)
{
	// Replace to "Vibration"
	return ((ff8_draw_menu_sprite_texture_infos*(*)(int,ff8_draw_menu_sprite_texture_infos*,int,int))ff8_externals.sub_4A7210)(a1, draw_infos, x, y);
}

void vibration_init()
{
	replace_function(ff8_externals.get_vibration_capability, ff8_vibrate_capability);
	replace_function(ff8_externals.vibration_apply, apply_vibrate_calc);
	replace_function(ff8_externals.vibration_clear_intensity, noop_a1);
	ff8_set_vibration_replace_id = replace_function(ff8_externals.set_vibration, ff8_set_vibration);

	// Replace pause menus
	replace_call(uint32_t(ff8_externals.check_game_is_paused) + 0x88, ff8_game_is_paused);
	replace_call(ff8_externals.battle_pause_sub_4CD140 + (JP_VERSION ? 0x1F1 : 0x225), alternate_pause_battle);
	replace_call(ff8_externals.battle_pause_window_sub_4CD350 + 0x8D, battle_pause_show_quit_game_text);

	// Relocate menu_options_keymap_desc to make room for a new input line in menu_options_desc which is located right before
	memcpy(menu_options_keymap_desc, ff8_externals.menu_config_input_desc_keymap, sizeof(menu_options_keymap_desc));
	patch_code_dword(uint32_t(ff8_externals.menu_callbacks[8].func) + 0x110, DWORD(menu_options_keymap_desc));
	patch_code_dword(uint32_t(ff8_externals.menu_callbacks[8].func) + 0x12E, DWORD(menu_options_keymap_desc));
	patch_code_dword(ff8_externals.menu_config_controller + 0x84, DWORD(menu_options_keymap_desc));
	patch_code_dword(ff8_externals.menu_config_controller + 0x92, DWORD(menu_options_keymap_desc));
	patch_code_dword(ff8_externals.menu_config_controller + 0xD7, DWORD(menu_options_keymap_desc) + 4);
	patch_code_dword(ff8_externals.menu_config_controller + 0x109, DWORD(menu_options_keymap_desc) + 3);
	patch_code_dword(ff8_externals.menu_config_controller + 0x155, DWORD(menu_options_keymap_desc) + 1);
	patch_code_dword(ff8_externals.menu_config_controller + 0x53A, DWORD(menu_options_keymap_desc) + 3);
	patch_code_dword(ff8_externals.menu_config_controller + 0x5B1, DWORD(menu_options_keymap_desc) + 3);
	patch_code_dword(ff8_externals.menu_config_controller + 0x67B, DWORD(menu_options_keymap_desc));
	patch_code_dword(ff8_externals.menu_config_controller + 0x7A0, DWORD(menu_options_keymap_desc) + 3);
	patch_code_dword(ff8_externals.menu_config_render_submenu + 0x13, DWORD(menu_options_keymap_desc));

	// Add Vibration configuration line in main menu
	ff8_externals.menu_config_input_desc[9].text_id_name = 0x0; // "Vibration"
	ff8_externals.menu_config_input_desc[9].text_id_value1 = 0x2; // "OFF"
	ff8_externals.menu_config_input_desc[9].text_id_value2 = 0x1; // "ON"
	ff8_externals.menu_config_input_desc[9].type = 2; // Toggle switch (Vibration option specific)
	ff8_externals.menu_config_input_desc[9].mask = 0x40; // Bit mask in the savemap entry
	ff8_externals.menu_config_input_desc[9].value = 0; // Always initialized to zero, will change at runtime
	ff8_externals.menu_config_input_desc[9].callback_change_state = (void(*)())ff8_externals.sub_4C2FF0; // Update structures
	// End of configuration lines
	ff8_externals.menu_config_input_desc[10] = ff8_menu_config_input();
	ff8_externals.menu_config_input_desc[10].text_id_name = -1;
}
