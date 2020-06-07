/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
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

#include <windows.h>
#include <DSound.h>

#include "sfx.h"
#include "patch.h"
#include "ff7.h"

uint sfx_volumes[5];
IDirectSoundBuffer* sfx_buffers[5];

void sfx_init()
{
	// Add Global Focus flag to DirectSound Secondary Buffers
	patch_code_byte(common_externals.directsound_buffer_flags_1 + 0x4, 0x80); // DSBCAPS_GLOBALFOCUS & 0x0000FF00

	if (!ff8) {
		// On volume change in main menu initialization
		replace_call(0x758215, sfx_menu_force_channel_5_volume);
		// On SFX volume change in config menu
		replace_call(0x74DF4A, sfx_menu_play_sound_down);
		replace_call(0x74DF7F, sfx_menu_play_sound_up);
		// Fix escape sound not played more than once
		replace_function(0x41FA80, sfx_clear_sound_locks);
		// On stop sound in battle swirl
		replace_call(0x40805D, sfx_operation_battle_swirl_stop_sound);
		// On resume music after a battle
		replace_call(0x63BE33, sfx_operation_resume_music);
	}
}

bool sfx_buffer_is_looped(IDirectSoundBuffer* buffer)
{
	if (buffer == nullptr) {
		return false;
	}

	DWORD status;
	buffer->GetStatus(&status);

	if (status & (DSBSTATUS_LOOPING | DSBSTATUS_PLAYING)) {
		return true;
	}

	return false;
}

uint sfx_operation_battle_swirl_stop_sound(uint type, uint param1, uint param2, uint param3, uint param4, uint param5)
{
	if (trace_all || trace_music) info("Battle swirl stop sound\n");

	ff7_field_sfx_state* sfx_state = (ff7_field_sfx_state*)0xDBDC20;

	for (int i = 0; i < 5; ++i) {
		sfx_buffers[i] = nullptr;
		if (sfx_buffer_is_looped(sfx_state[i].buffer1)) {
			sfx_buffers[i] = sfx_state[i].buffer1;
		}
		else if (sfx_buffer_is_looped(sfx_state[i].buffer2)) {
			sfx_buffers[i] = sfx_state[i].buffer2;
		}
	}

	return ff7_externals.sound_operation(type, param1, param2, param3, param4, param5);
}

uint sfx_operation_resume_music(uint type, uint param1, uint param2, uint param3, uint param4, uint param5)
{
	if (trace_all || trace_music) info("Field resume music after battle\n");

	ff7_field_sfx_state* sfx_state = (ff7_field_sfx_state*)0xDBDC20;

	for (int i = 0; i < 5; ++i) {
		if (sfx_buffers[i] != nullptr) {
			if (sfx_state[i].buffer1 == sfx_buffers[i]) {
				// Play sound on channel
				((uint(*)(uint, uint, uint))(0x6E19E0))(sfx_state[i].pan1, sfx_state[i].sound_id, i + 1);
			}
			else if (sfx_state[i].buffer2 == sfx_buffers[i]) {
				((uint(*)(uint, uint, uint))(0x6E19E0))(sfx_state[i].pan2, sfx_state[i].sound_id, i + 1);
			}

			sfx_buffers[i] = nullptr;
		}
	}

	return ff7_externals.sound_operation(type, param1, param2, param3, param4, param5);
}

void sfx_remember_volumes()
{
	ff7_field_sfx_state* sfx_state = (ff7_field_sfx_state*)0xDBDC20;

	for (int i = 0; i <= 4; ++i) {
		sfx_volumes[i] = sfx_state[i].buffer1 != nullptr ? sfx_state[i].volume1 : sfx_state[i].volume2;
	}
}

void sfx_menu_force_channel_5_volume(uint volume, uint channel)
{
	// Original call (set channel 5 volume to maximum)
	common_externals.set_sfx_volume(volume, channel);
	// Added by FFNx
	sfx_remember_volumes();
}

void sfx_menu_play_sound_down(uint id)
{
	// Added by FFNx
	sfx_update_volume(-1);
	// Original call (curor sound)
	common_externals.play_sfx(id);
}

void sfx_menu_play_sound_up(uint id)
{
	// Added by FFNx
	sfx_update_volume(1);
	// Original call (curor sound)
	common_externals.play_sfx(id);
}

void sfx_update_volume(int modifier)
{
	// Set master sfx volume
	int* sfx_volume = (int*)0xDC349C;
	int* sfx_tmp_volume = (int*)0xF3AE28;

	*sfx_volume = *sfx_tmp_volume + modifier;

	// Update sfx volume in real-time for all channel
	for (int channel = 1; channel <= 5; ++channel) {
		common_externals.set_sfx_volume(sfx_volumes[channel - 1], channel);
	}
}

void sfx_clear_sound_locks()
{
	// The last uint wasn't reset by the original sub
	memset((void*)0x9AFCE8, 0, 5 * sizeof(uint));
}
