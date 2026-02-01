/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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
#include "../patch.h"
#include "../vibration.h"
#include "battle/effects.h"

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

	if (data == ff8_externals.vibrate_data_field)
	{
		return set_name("field");
	}

	if (getmode_cached()->driver_mode != MODE_BATTLE)
	{
		return nullptr;
	}

	switch (FF8BattleEffect::Effect(*ff8_externals.battle_magic_id))
	{
	case FF8BattleEffect::Quezacotl:
		return set_name("battle_quezacotl");
	case FF8BattleEffect::Shiva:
		return set_name("battle_shiva");
	case FF8BattleEffect::Ifrit:
		return set_name("battle_ifrit");
	case FF8BattleEffect::Siren:
		return set_name("battle_siren");
	case FF8BattleEffect::Brothers:
		return set_name("battle_brothers");
	case FF8BattleEffect::Diablos:
		return set_name("battle_diablos");
	case FF8BattleEffect::Carbuncle:
		return set_name("battle_carbuncle");
	case FF8BattleEffect::Leviathan:
		return set_name("battle_leviathan");
	case FF8BattleEffect::Pandemona:
		return set_name("battle_pandemona");
	case FF8BattleEffect::Cerberus:
		return set_name("battle_cerberus");
	case FF8BattleEffect::Alexander:
		return set_name("battle_alexander");
	case FF8BattleEffect::Doomtrain:
		return set_name("battle_doomtrain");
	case FF8BattleEffect::Bahamut:
		return set_name("battle_bahamut");
	case FF8BattleEffect::Cactuar:
		return set_name("battle_cactuar");
	case FF8BattleEffect::Tonberry:
		return set_name("battle_tonberry");
	case FF8BattleEffect::Eden:
		return set_name("battle_eden");
	case FF8BattleEffect::Odin:
		return set_name("battle_odin");
	case FF8BattleEffect::GilgameshZantetsuken:
		return set_name("battle_gilgamesh_zantetsuken");
	case FF8BattleEffect::GilgameshMasamune:
		return set_name("battle_gilgamesh_masamune");
	case FF8BattleEffect::GilgameshExcaliber:
		return set_name("battle_gilgamesh_excaliber");
	case FF8BattleEffect::GilgameshExcalipoor:
		return set_name("battle_gilgamesh_excalipoor");
	case FF8BattleEffect::Phoenix:
		return set_name("battle_phoenix");
	case FF8BattleEffect::Moomba:
		return set_name("battle_moomba");
	case FF8BattleEffect::Minimog:
		return set_name("battle_minimog");
	case FF8BattleEffect::BokoChocofire:
		return set_name("battle_boko_chocofire");
	case FF8BattleEffect::BokoChocoflare:
		return set_name("battle_boko_chocoflare");
	case FF8BattleEffect::BokoChocometeor:
		return set_name("battle_boko_chocometeor");
	case FF8BattleEffect::BokoChocobocle:
		return set_name("battle_boko_chocobocle");
	case FF8BattleEffect::Meteor:
		return set_name("battle_meteor");
	case FF8BattleEffect::Apocalypse:
		return set_name("battle_apocalypse");
	case FF8BattleEffect::Ultima:
		return set_name("battle_ultima");
	}

	snprintf(vibrateName, sizeof(vibrateName), "battle_effect_%d", *ff8_externals.battle_magic_id);

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
