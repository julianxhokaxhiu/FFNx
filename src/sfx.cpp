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

#include "sfx.h"
#include "patch.h"

uint sfx_volumes[5];

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
	}
}

void sfx_remember_volumes()
{
	for (int i = 0; i <= 4; ++i) {
		uint* current_channel_volume = (uint*)0xDBDC24;
		sfx_volumes[i] = current_channel_volume[i * 0x15];
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
	info("sfx_update_volume\n");

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
