/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 myst6re                                            //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Tang-Tang Zhou                                     //
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

int ff8_vibrate_capability(int port)
{
	bool ret = false;

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

	if (trace_all || trace_gamepad) ffnx_trace("%s left=%d right=%d vibrate_option_enabled=%d\n", __func__, left, right, ff8_externals.gamepad_vibration_states[port].vibrate_option_enabled);

	if (ff8_externals.gamepad_vibration_states[port].vibrate_option_enabled == 255)
	{
		nxVibrationEngine.setLeftMotorValue(left);
		nxVibrationEngine.setRightMotorValue(right);
		nxVibrationEngine.rumbleUpdate();
	}
}

void apply_vibrate_calc(char port, int left, int right)
{
	ff8_gamepad_vibration_state *gamepad_state = ff8_externals.gamepad_vibration_states;
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

size_t vibrate_data_estimate_size(const uint8_t *data)
{
	uint32_t *header = (uint32_t *)data;
	uint32_t max_pos = 0;

	while (*header && header - (uint32_t *)data < 64) {
		if (*header > max_pos) {
			max_pos = *header;
		}
		header++;
	}

	return max_pos + 4;
}

const char *vibrate_data_name(const uint8_t *data, XXH64_hash_t hash)
{
	if (trace_all || trace_gamepad) ffnx_trace("%s: hash=0x%llX data=0x%X\n", __func__, hash, data);

	size_t size = vibrate_data_estimate_size(data);

	switch (hash)
	{
	case 0x5839BE1A4D099886:
		return set_name("main");
	case 0x5ADC96A5804D9034:
		return set_name("field");
	case 0x48B2C92616FC193D:
		return set_name("world");
	}

	snprintf(vibrateName, sizeof(vibrateName), "%llX", hash);

	return vibrateName;
}

uint32_t ff8_set_vibration_replace_id = 0;

int ff8_set_vibration(const uint8_t *data, int set, int intensity)
{
	if (trace_all || trace_gamepad) ffnx_trace("%s: set=%d intensity=%d\n", __func__, set, intensity);

	size_t size = vibrate_data_estimate_size(data);
	XXH64_hash_t hash = XXH3_64bits(data, size);
	const char *name = vibrate_data_name(data, hash);

	const uint8_t *dataOverride = nxVibrationEngine.vibrateDataOverride(name);
	if (dataOverride != nullptr)
	{
		data = dataOverride;
	}
	else if (strcmp(name, "world") == 0) {
		// Disable vibration
		return 0;
	}

	unreplace_function(ff8_set_vibration_replace_id);
	int ret = ((int(*)(const uint8_t*,int,int))ff8_externals.set_vibration)(data, set, intensity);
	ff8_set_vibration_replace_id = replace_function(ff8_externals.set_vibration, ff8_set_vibration);

	return ret;
}

void vibration_init()
{
	replace_call(uint32_t(ff8_externals.check_game_is_paused) + 0x88, ff8_game_is_paused);
	replace_function(ff8_externals.get_vibration_capability, ff8_vibrate_capability);
	replace_function(ff8_externals.vibration_apply, apply_vibrate_calc);
	replace_function(ff8_externals.vibration_clear_intensity, noop_a1);
	ff8_set_vibration_replace_id = replace_function(ff8_externals.set_vibration, ff8_set_vibration);
}
