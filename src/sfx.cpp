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

#include "audio.h"
#include "patch.h"
#include "ff7.h"

#include "sfx.h"

uint32_t sfx_volumes[5];
ff7_field_sfx_state sfx_buffers[4];
uint32_t real_volume;
ff7_field_sfx_state* sfx_state = nullptr;

//=============================================================================

bool ff7_should_sfx_loop(int id)
{
	return *(BYTE *)(ff7_externals.sfx_fmt_header + (28 * (id - 1)));
}

int ff7_sfx_load(int id, DWORD dsound_flag)
{
	bool shouldCurrentSfxLoop = ff7_should_sfx_loop(id);

	if (trace_all || trace_sfx) trace("%s: id=%d,loop=%x\n", __func__, id, shouldCurrentSfxLoop);

	if (id) nxAudioEngine.loadSFX(id, shouldCurrentSfxLoop);

	return true;
}

void ff7_sfx_unload(int id, int unk)
{
	if (trace_all || trace_sfx) trace("%s: id=%d\n", __func__, id);

	if (id) nxAudioEngine.unloadSFX(id);
}

void ff7_sfx_set_volume_on_channel(byte volume, int channel)
{
	if (trace_all || trace_sfx) trace("%s: volume=%d,channel=%d\n", __func__, volume, channel);

	nxAudioEngine.setSFXVolume(volume / 127.0f, channel);
}

void ff7_sfx_set_volume_trans_on_channel(byte volume, int channel, int time)
{
	if (trace_all || trace_sfx) trace("%s: volume=%d,channel=%d,time=%d\n", __func__, volume, channel, time);

	nxAudioEngine.setSFXVolume(channel, volume / 127.0f, time / 60.0f);
}

void ff7_sfx_set_panning_on_channel(byte panning, int channel)
{
	if (trace_all || trace_sfx) trace("%s: panning=%d,channel=%d\n", __func__, panning, channel);

	if (panning <= 127)
		nxAudioEngine.setSFXPanning(channel, panning == 64 ? 0.0f : panning * 2 / 127.0f - 1.0f);
}

void ff7_sfx_set_frequency_on_channel(byte speed, int channel)
{
	if (trace_all || trace_sfx) trace("%s: speed=%d,channel=%d\n", __func__, speed, channel);

	if (speed == -128) {
		speed = -127; // Prevent speed to be 0 (can crash with SoLoud)
	}

	nxAudioEngine.setSFXSpeed(channel, float(speed) / 128.0f + 1.0f);
}

void ff7_sfx_play_on_channel(byte panning, int id, int channel)
{
	if (trace_all || trace_sfx) trace("%s: id=%d,channel=%d,panning=%d\n", __func__, id, channel, panning);

	ff7_field_sfx_state *currentState = &sfx_state[channel-1];

	if (id)
	{
		if (currentState->sound_id == id && !currentState->is_looped) nxAudioEngine.stopSFX(channel);

		if (!currentState->is_looped)
		{
			ff7_sfx_load(id, 0);
			nxAudioEngine.playSFX(id, channel, panning == 64 ? 0.0f : panning * 2 / 127.0f - 1.0f);
		}
	}
	else
	{
		nxAudioEngine.stopSFX(channel);
	}

	currentState->pan1 = panning;
	currentState->sound_id = id;
	currentState->is_looped = ff7_should_sfx_loop(id);
}

void ff7_sfx_play_on_channel_5(int id)
{
	ff7_sfx_play_on_channel(64, id, 5);
}

void ff7_sfx_load_and_play_with_speed(int id, byte panning, byte volume, byte speed)
{
	const int _channel = 5;

	if (trace_all || trace_sfx) trace("%s: id=%d,volume=%d,panning=%d,speed=%d\n", __func__, id, volume, panning, speed);

	if (id)
	{
		ff7_sfx_load(id, 0);
		ff7_sfx_set_volume_on_channel(volume, _channel);
		ff7_sfx_set_frequency_on_channel(speed, _channel);
	}

	ff7_sfx_play_on_channel(panning, id, _channel);
}

void ff7_sfx_pause()
{
	nxAudioEngine.pauseSFX();
}

void ff7_sfx_resume()
{
	nxAudioEngine.resumeSFX();
}

void ff7_sfx_stop()
{
	for (short channel = 1; channel < 5; channel++) nxAudioEngine.stopSFX(channel);
}

//=============================================================================

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

uint32_t sfx_operation_battle_swirl_stop_sound(uint32_t type, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4, uint32_t param5)
{
	if (trace_all || trace_sfx) info("%s: Battle swirl stop sound\n", __func__);

	for (int i = 0; i < 4; ++i) {
		sfx_buffers[i] = ff7_field_sfx_state();
		sfx_buffers[i].buffer1 = nullptr;
		sfx_buffers[i].buffer2 = nullptr;

		if (use_external_sfx)
		{
			// Check which SFX effects are still playing on channels before saving their state. If not, avoid playing them back.
			if (!nxAudioEngine.isSFXPlaying(i)) sfx_state[i].sound_id = 0;
		}

		// Save sfx state for looped sounds in channel 1 -> 4 (not channel 5)
		if (sfx_buffer_is_looped(sfx_state[i].buffer1) || sfx_buffer_is_looped(sfx_state[i].buffer2) || use_external_sfx) {
			memcpy(&sfx_buffers[i], &sfx_state[i], sizeof(ff7_field_sfx_state));
		}
	}

	return ((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))ff7_externals.sound_operation)(type, param1, param2, param3, param4, param5);
}

uint32_t sfx_operation_resume_music(uint32_t type, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4, uint32_t param5)
{
	if (trace_all || trace_sfx) info("%s: Field resume music after battle\n", __func__);

	for (int i = 0; i < 4; ++i) {
		if (use_external_sfx)
		{
			ff7_sfx_play_on_channel(sfx_buffers[i].pan1, sfx_buffers[i].sound_id, i + 1);
		}
		else
		{
			if (sfx_buffers[i].buffer1 != nullptr || sfx_buffers[i].buffer2 != nullptr) {
				uint32_t pan;
				if (sfx_buffers[i].buffer1 != nullptr) {
					pan = sfx_buffers[i].pan1;
				}
				else {
					pan = sfx_buffers[i].pan2;
				}

				((uint32_t(*)(uint32_t, uint32_t, uint32_t))common_externals.play_sfx_on_channel)(pan, sfx_buffers[i].sound_id, i + 1);

				sfx_buffers[i] = ff7_field_sfx_state();
				sfx_buffers[i].buffer1 = nullptr;
				sfx_buffers[i].buffer2 = nullptr;
			}
		}
	}

	return ((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))ff7_externals.sound_operation)(type, param1, param2, param3, param4, param5);
}

void sfx_remember_volumes()
{
	if (trace_all || trace_sfx) info("%s: Remember SFX volumes (master: %i)\n", __func__, *common_externals.master_sfx_volume);

	for (int i = 0; i < 5; ++i) {
		sfx_volumes[i] = sfx_state[i].buffer1 != nullptr ? sfx_state[i].volume1 : sfx_state[i].volume2;

		if (sfx_volumes[i] > 127) {
			sfx_volumes[i] = 127;
		}

		if (trace_all || trace_sfx) info("%s: SFX volume channel #%i: %i\n", __func__, i, sfx_volumes[i]);
	}
}

void sfx_menu_force_channel_5_volume(uint32_t volume, uint32_t channel)
{
	if (trace_all || trace_sfx) info("%s: %d\n", __func__, volume);
	// Original call (set channel 5 volume to maximum)
	if (use_external_sfx)
		ff7_sfx_set_volume_on_channel(volume, channel);
	else
		common_externals.set_sfx_volume_on_channel(volume, channel);
	// Added by FFNx
	sfx_remember_volumes();
}

void sfx_update_volume(int modifier)
{
	if (trace_all || trace_sfx) info("%s: Update SFX volumes %d\n", __func__, modifier);

	// Set master sfx volume
	BYTE** sfx_tmp_volume = (BYTE**)(ff7_externals.menu_sound_slider_loop + ff7_externals.call_menu_sound_slider_loop_sfx_down + 0xA);

	*common_externals.master_sfx_volume = **sfx_tmp_volume + modifier;

	// Update sfx volume in real-time for all channel
	for (int channel = 1; channel <= 5; ++channel) {
		if (use_external_sfx)
			ff7_sfx_set_volume_on_channel(sfx_volumes[channel - 1], channel);
		else
			common_externals.set_sfx_volume_on_channel(sfx_volumes[channel - 1], channel);

		if (trace_all || trace_sfx) info("%s: Set SFX volume for channel #%i: %i\n", __func__, channel, sfx_volumes[channel - 1]);
	}
}

void sfx_menu_play_sound_down(uint32_t id)
{
	// Added by FFNx
	sfx_update_volume(-1);

	// Original call (cursor sound)
	if (use_external_sfx)
		ff7_sfx_play_on_channel_5(id);
	else
		common_externals.play_sfx(id);
}

void sfx_menu_play_sound_up(uint32_t id)
{
	// Added by FFNx
	sfx_update_volume(1);

	// Original call (cursor sound)
	if (use_external_sfx)
		ff7_sfx_play_on_channel_5(id);
	else
		common_externals.play_sfx(id);
}

void sfx_clear_sound_locks()
{
	uint32_t** flags = (uint32_t**)(ff7_externals.battle_clear_sound_flags + 5);
	// The last uint32_t wasn't reset by the original sub
	memset((void*)*flags, 0, 5 * sizeof(uint32_t));
}

void sfx_fix_volume_values(char* log)
{
	if (trace_all || trace_sfx) info("%s", log); // FF7 default log

	for (int i = 0; i < 5; ++i) {
		if (sfx_state[i].u1 == 0xFFFFFFFF) {
			if (trace_all || trace_sfx) info("%s: SFX fix volume channel #%i: %i\n", __func__, i + 1, real_volume);

			sfx_state[i].volume1 = real_volume;
			sfx_state[i].volume2 = real_volume;
			sfx_state[i].u1 = 0; // Back to the correct value
		}
	}
}

int sfx_play_battle_specific(IDirectSoundBuffer* buffer, uint32_t flags)
{
	if (buffer == nullptr) {
		return 0;
	}

	// Added by FFNx: set buffer volume according to master_sfx_volume
	unsigned char volume = 127 * (*common_externals.master_sfx_volume) / 100;

	buffer->SetVolume(common_externals.dsound_volume_table[volume]);

	// Original behavior
	HRESULT res = buffer->Play(0, 0, flags);

	if (DSERR_BUFFERLOST == res) {
		res = buffer->Restore();

		return -1;
	}

	return res == DS_OK;
}

uint32_t sfx_fix_omnislash_sound_loading(int sound_id, int dsound_buffer)
{
	// Added by FFNx: Load sound 0x188
	((uint32_t(*)(int, int))ff7_externals.sfx_fill_buffer_from_audio_dat)(0x188, dsound_buffer);

	// Original call (load sound 0x285)
	return ((uint32_t(*)(int, int))ff7_externals.sfx_fill_buffer_from_audio_dat)(sound_id, dsound_buffer);
}

//=============================================================================

void sfx_init()
{
	// Add Global Focus flag to DirectSound Secondary Buffers
	patch_code_byte(common_externals.directsound_buffer_flags_1 + 0x4, 0x80); // DSBCAPS_GLOBALFOCUS & 0x0000FF00

	// SFX Patches
	if (!ff8) {
		// On volume change in main menu initialization
		replace_call(ff7_externals.menu_start + 0x17, sfx_menu_force_channel_5_volume);
		// On SFX volume change in config menu
		replace_call(ff7_externals.menu_sound_slider_loop + ff7_externals.call_menu_sound_slider_loop_sfx_down, sfx_menu_play_sound_down);
		replace_call(ff7_externals.menu_sound_slider_loop + ff7_externals.call_menu_sound_slider_loop_sfx_up, sfx_menu_play_sound_up);
		// Fix escape sound not played more than once
		replace_function(ff7_externals.battle_clear_sound_flags, sfx_clear_sound_locks);
		// On stop sound in battle swirl
		replace_call(ff7_externals.swirl_sound_effect + 0x26, sfx_operation_battle_swirl_stop_sound);
		// On resume music after a battle
		replace_call(ff7_externals.field_initialize_variables + 0xEB, sfx_operation_resume_music);

		// Leviathan fix
		patch_code_byte(ff7_externals.battle_summon_leviathan_loop + 0x3FA + 1, 0x2A);
		// Omnislash fix
		replace_call(ff7_externals.battle_limit_omnislash_loop + 0x5A, sfx_fix_omnislash_sound_loading);

		if (use_external_sfx)
		{
			replace_function(common_externals.sfx_load, ff7_sfx_load);
			replace_function(common_externals.sfx_unload, ff7_sfx_unload);
			replace_function(common_externals.play_sfx_on_channel, ff7_sfx_play_on_channel);
			replace_function((uint32_t)common_externals.play_sfx, ff7_sfx_play_on_channel_5);
			replace_function((uint32_t)common_externals.set_sfx_volume_on_channel, ff7_sfx_set_volume_on_channel);
			replace_function((uint32_t)common_externals.set_sfx_volume_trans_on_channel, ff7_sfx_set_volume_trans_on_channel);
			replace_function((uint32_t)common_externals.set_sfx_panning_on_channel, ff7_sfx_set_panning_on_channel);
			replace_function((uint32_t)common_externals.set_sfx_frequency_on_channel, ff7_sfx_set_frequency_on_channel);
			replace_function(ff7_externals.sfx_load_and_play_with_speed, ff7_sfx_load_and_play_with_speed);
			replace_function(ff7_externals.sfx_play_summon, ff7_sfx_play_on_channel_5);
			replace_function(common_externals.sfx_pause, ff7_sfx_pause);
			replace_function(common_externals.sfx_resume, ff7_sfx_resume);
			replace_function(common_externals.sfx_stop, ff7_sfx_stop);

			sfx_state = new ff7_field_sfx_state[5]{0};
		}
		else
		{
			/*
			* Set sound volume on channel changes
			* When this sub is called, it set two fields of sfx_state,
			* but with the wrong value (computed with sfx_master_volume)
			*/

			// Replace a useless "volume & 0xFF" to "real_volume <- volume; nop"
			patch_code_byte(uint32_t(common_externals.set_sfx_volume_on_channel) + 0x48, 0xA3); // mov
			patch_code_uint(uint32_t(common_externals.set_sfx_volume_on_channel) + 0x48 + 1, uint32_t(&real_volume));
			patch_code_byte(uint32_t(common_externals.set_sfx_volume_on_channel) + 0x48 + 5, 0x90); // nop
			// Use a field of sfx_state to flag the current channel
			patch_code_uint(uint32_t(common_externals.set_sfx_volume_on_channel) + 0x70, 0xFFFFFFFF);
			// Replace log call to fix sfx_state volume values
			replace_call(uint32_t(common_externals.set_sfx_volume_on_channel) + 0x183, sfx_fix_volume_values);

			// Fix volume on specific SFX
			replace_call(ff7_externals.sfx_play_summon + 0xA2, sfx_play_battle_specific);
			replace_call(ff7_externals.sfx_play_summon + 0xF2, sfx_play_battle_specific);

			// Store pointer to the SFX states
			sfx_state = ff7_externals.sound_states;
		}
	}
}
