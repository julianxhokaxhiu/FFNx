/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
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
ff7_channel_6_state sfx_channel_6_state;

constexpr auto FF8_MAX_CHANNEL_NUMBER = 0x1F;

//=============================================================================

void ff7_sfx_release(IDirectSoundBuffer *buffer)
{
	if (buffer) buffer->Release();
}

bool ff7_should_sfx_loop(int id)
{
	return ff7_externals.sfx_fmt_header[id-1].loop;
}

void ff7_sfx_stop_channel(int channel, double time = 0)
{
	nxAudioEngine.stopSFX(channel, time);

	sfx_state[channel-1].pan1 = 64;
	sfx_state[channel-1].sound_id = 0;
	sfx_state[channel-1].is_looped = false;
}

void ff8_sfx_stop_channel(int channel, double time = 0)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: channel=%d\n", __func__, channel);

	if (channel <= FF8_MAX_CHANNEL_NUMBER)
	{
		nxAudioEngine.stopSFX(channel, time);
	}
}

int ff7_sfx_load(int id, DWORD dsound_flag)
{
	//if (trace_all || trace_sfx) ffnx_trace("%s: id=%d\n", __func__, id);

	return true;
}

unsigned int ff8_sfx_load(unsigned int channel, unsigned int id, int is_eax)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: channel=%d id=%d is_eax=%d\n", __func__, channel, id, is_eax);

	return true;
}

void ff7_sfx_unload(int id, void* unk)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: id=%d\n", __func__, id);

	nxAudioEngine.unloadSFX(id);
}

void ff8_sfx_unload(int channel)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: channel=%d\n", __func__, channel);

	nxAudioEngine.unloadSFX(nxAudioEngine.getSFXIdFromChannel(channel));
}

void ff8_sfx_set_master_volume(uint32_t volume)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: volume=%d\n", __func__, volume);

	if (volume <= 100) {
		*common_externals.master_sfx_volume = volume;
		nxAudioEngine.setSFXMasterVolume(volume / 100.0);
	}
}

void ff7_sfx_set_volume_on_channel(byte volume, int channel)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: volume=%d,channel=%d\n", __func__, volume, channel);

	sfx_state[channel].volume1 = volume;

	nxAudioEngine.setSFXVolume(channel, volume / 127.0f);
}

void ff8_sfx_set_volume(uint32_t channel, uint32_t volume, int32_t time)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: channel=%d, volume=%d, time=%d\n", __func__, channel, volume, time);

	if (volume == 0)
		ff8_sfx_stop_channel(channel, time / 60.0f);
	else if (channel <= FF8_MAX_CHANNEL_NUMBER && volume <= 127)
		nxAudioEngine.setSFXVolume(channel, volume / 127.0f, time / 60.0f);
}

void ff7_sfx_set_volume_trans_on_channel(byte volume, int channel, int time)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: volume=%d,channel=%d,time=%d\n", __func__, volume, channel, time);

	if (volume == 0)
		ff7_sfx_stop_channel(channel, time / 60.0f);
	else
		nxAudioEngine.setSFXVolume(channel, volume / 127.0f, time / 60.0f);
}

void ff7_sfx_set_panning_on_channel(byte panning, int channel)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: panning=%d,channel=%d\n", __func__, panning, channel);

	sfx_state[channel].pan1 = panning;

	if (panning <= 127)
		nxAudioEngine.setSFXPanning(channel, panning == 64 ? 0.0f : panning * 2 / 127.0f - 1.0f);
}

void ff8_sfx_set_panning(uint32_t channel, uint32_t panning, int32_t time)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: channel=%d, panning=%d, time=%d\n", __func__, channel, panning, time);

	if (channel <= FF8_MAX_CHANNEL_NUMBER && panning <= 127)
	{
		// TODO: inverted panning option ((Reg.SoundOptions >> 20) & 1)
		nxAudioEngine.setSFXPanning(channel, panning == 64 ? 0.0f : panning * 2 / 127.0f - 1.0f);
		// TODO: 3D sfx
	}
}

void ff7_sfx_set_panning_trans_on_channel(byte panning, int channel, int time)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: panning=%d,channel=%d,time=%d\n", __func__, panning, channel, time);

	if (panning <= 127)
		nxAudioEngine.setSFXPanning(channel, panning == 64 ? 0.0f : panning * 2 / 127.0f - 1.0f, time / 60.0f);
}

void ff7_sfx_set_frequency_on_channel(byte speed, int channel)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: speed=%d,channel=%d\n", __func__, speed, channel);

	sfx_state[channel].frequency = speed;

	if (speed == -128) {
		speed = -127; // Prevent speed to be 0 (can crash with SoLoud)
	}

	nxAudioEngine.setSFXSpeed(channel, float(speed) / 128.0f + 1.0f);
}

void ff7_sfx_set_frequency_trans_on_channel(byte speed, int channel, int time)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: speed=%d,channel=%d,time=%d\n", __func__, speed, channel, time);

	if (speed == -128) {
		speed = -127; // Prevent speed to be 0 (can crash with SoLoud)
	}

	nxAudioEngine.setSFXSpeed(channel, float(speed) / 128.0f + 1.0f, time / 60.0f);
}

bool ff7_sfx_play_layered(float panning, int id, int channel)
{
	const struct game_mode* mode = getmode_cached();
	bool playing = false;
	char track_name[64];
	byte actor_id = 0xFF;

	switch(mode->driver_mode)
	{
	case MODE_FIELD:
		sprintf(track_name, "%s_%d_%d", get_current_field_name(), *ff7_externals.current_triangle_id, id);
		break;
	case MODE_MENU:
		sprintf(track_name, "menu_%d", id);
		break;
	case MODE_WORLDMAP:
		sprintf(track_name, "world_%d", id);
		break;
	case MODE_BATTLE:
		actor_id = ff7_externals.anim_event_queue[0].attackerID;
		if(actor_id >= 0 && actor_id <= 2)
			sprintf(track_name, "battle_char_%02X_%d", ff7_externals.battle_context->actor_vars[actor_id].index, id);
		else if(actor_id >= 4 && actor_id <= 9)
			sprintf(track_name, "battle_enemy_%04X_%d", ff7_externals.battle_context->actor_vars[actor_id].formationID, id);
		else
			sprintf(track_name, "%d", id);
		break;
	default:
		sprintf(track_name, "%d", id);
	}

	// If any overridden layer could not be played, fallback to default
	if (!(playing = nxAudioEngine.playSFX(track_name, id, channel, panning, ff7_should_sfx_loop(id))))
	{
		if (mode->driver_mode == MODE_FIELD)
		{
			sprintf(track_name, "%s_%d", get_current_field_name(), id);
			if (!(playing = nxAudioEngine.playSFX(track_name, id, channel, panning, ff7_should_sfx_loop(id))))
			{
				sprintf(track_name, "%d", id);
				playing = nxAudioEngine.playSFX(track_name, id, channel, panning, ff7_should_sfx_loop(id));
			}
		}
		else
		{
			sprintf(track_name, "%d", id);
			playing = nxAudioEngine.playSFX(track_name, id, channel, panning, ff7_should_sfx_loop(id));
		}
	}

	return playing;
}

bool ff8_sfx_play_layered(int channel, int id, int volume, float panning)
{
	const struct game_mode* mode = getmode_cached();
	bool playing = false;
	char track_name[64];
	float panningf = panning == 64 ? 0.0f : panning * 2 / 127.0f - 1.0f;
	float volumef = volume / 127.0;

	switch(mode->driver_mode)
	{
	case MODE_FIELD:
		sprintf(track_name, "%s_%d", get_current_field_name(), id);
		break;
	case MODE_MENU:
		sprintf(track_name, "menu_%d", id);
		break;
	case MODE_WORLDMAP:
		sprintf(track_name, "world_%d", id);
		break;
	case MODE_BATTLE:
		sprintf(track_name, "battle_%d", id);
		break;
	default:
		sprintf(track_name, "%d", id);
	}

	bool loop = false;

	// Get loop info from audio.fmt
	if (id <= *ff8_externals.sfx_sound_count) {
		loop = (*ff8_externals.sfx_audio_fmt)[id].loop;
	}

	// TODO: inverted panning option ((Reg.SoundOptions >> 20) & 1)
	nxAudioEngine.setSFXVolume(channel, volumef);

	// If any overridden layer could not be played, fallback to default
	if (!(playing = nxAudioEngine.playSFX(track_name, id, channel, panningf, loop)))
	{
		sprintf(track_name, "%d", id);
		playing = nxAudioEngine.playSFX(track_name, id, channel, panningf, loop);
	}

	return playing;
}

void ff7_sfx_play_on_channel(byte panning, int id, int channel)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: id=%d,channel=%d,panning=%d\n", __func__, id, channel, panning);

	ff7_field_sfx_state *currentState = &sfx_state[channel-1];

	if (id)
	{
		for (int chdx = 1; chdx <= 5; chdx++)
		{
			if(sfx_state[chdx-1].sound_id == id && sfx_state[chdx-1].is_looped && chdx != channel)
			{
				ff7_sfx_stop_channel(chdx);
				return;
			}
		}

		if (channel <= 5 && ((currentState->sound_id == id && !currentState->is_looped) || (currentState->sound_id != id))) ff7_sfx_stop_channel(channel);

		if (currentState->sound_id != id) currentState->is_looped = false;

		if (!currentState->is_looped)
		{
			ff7_sfx_play_layered(panning == 64 ? 0.0f : panning * 2 / 127.0f - 1.0f, id, channel);
		}

		currentState->pan1 = panning;
		currentState->sound_id = id;
		currentState->is_looped = ff7_should_sfx_loop(id);
	}
	else if ( channel < 6) // normally all sounds that are non-channel aware must never be stopped by the engine
	{
		ff7_sfx_stop_channel(channel);
	}
}

int ff8_sfx_play_channel(unsigned int channel, unsigned int id, unsigned int volume, unsigned int panning, int pitch)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: channel=%d, id=%d, volume=%d, panning=%d, pitch=%d\n", __func__, channel, id, volume, panning, pitch);

	if (channel > FF8_MAX_CHANNEL_NUMBER || volume > 127 || panning > 127)
	{
		return 0;
	}

	// pitch is always 100
	return ff8_sfx_play_layered(channel, id, volume, panning);
}

void ff7_sfx_play_on_channel_5(int id)
{
	ff7_sfx_play_on_channel(64, id, 5);
}

void ff7_sfx_load_and_play_with_speed(int id, byte panning, byte volume, byte speed)
{
	const int _channel = 6;

	if (trace_all || trace_sfx) ffnx_trace("%s: id=%d,volume=%d,panning=%d,speed=%d\n", __func__, id, volume, panning, speed);

	if (id)
	{
		ff7_sfx_set_volume_on_channel(volume, _channel);
		ff7_sfx_set_frequency_on_channel(speed, _channel);
		ff7_sfx_play_on_channel(panning, id, _channel);
	}
}

bool ff8_sfx_is_playing(int channel)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: channel=%d\n", __func__, channel);

	return channel <= FF8_MAX_CHANNEL_NUMBER ? nxAudioEngine.isSFXPlaying(channel) : false;
}

void ff7_sfx_pause()
{
	if (trace_all || trace_sfx) ffnx_trace("%s\n", __func__);

	for (short channel = 1; channel <= 6; channel++) nxAudioEngine.pauseSFX(channel);
}

void ff8_sfx_pause_channel(int channel)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: channel=%d\n", __func__, channel);

	if (channel <= FF8_MAX_CHANNEL_NUMBER)
	{
		nxAudioEngine.pauseSFX(channel);
	}
}

void ff7_sfx_resume()
{
	if (trace_all || trace_sfx) ffnx_trace("%s\n", __func__);

	for (short channel = 1; channel <= 6; channel++) nxAudioEngine.resumeSFX(channel);
}

void ff8_sfx_resume_channel(int channel)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: channel=%d\n", __func__, channel);

	if (channel <= FF8_MAX_CHANNEL_NUMBER)
	{
		nxAudioEngine.resumeSFX(channel);
	}
}

void ff7_sfx_stop()
{
	if (trace_all || trace_sfx) ffnx_trace("%s\n", __func__);

	for (short channel = 1; channel <= 6; channel++)
		ff7_sfx_stop_channel(channel);

	*ff7_externals.sfx_play_effects_id_channel_6 = 0;
}

// Channel 6 only

void ff7_sfx_set_frequency_on_channel_6(void* dsoundptr, DWORD frequency)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: frequency=%lu\n", __func__, frequency);

	// TODO: IMPLEMENT PITCH?
}

void ff7_sfx_set_panning_on_channel_6(void* dsoundptr, LONG panning)
{
	sfx_channel_6_state.panning = (panning / 10000.0f);

	if (trace_all || trace_sfx) ffnx_trace("%s: panning=%ld,calculated=%f\n", __func__, panning, sfx_channel_6_state.panning);
}

void ff7_sfx_set_volume_on_channel_6(void* dsoundptr, LONG volume)
{
	sfx_channel_6_state.volume = (volume / 10000.0f) + 1.0f;

	if (trace_all || trace_sfx) ffnx_trace("%s: volume=%ld,calculated=%f\n", __func__, volume, sfx_channel_6_state.volume);
}

void ff7_sfx_play_on_channel_6(void* dsoundptr, int unk)
{
	if (trace_all || trace_sfx) ffnx_trace("%s: id=%lu,panning=%f\n", __func__, *ff7_externals.sfx_play_effects_id_channel_6, sfx_channel_6_state.panning);

	nxAudioEngine.setSFXVolume(6, sfx_channel_6_state.volume);
	ff7_sfx_play_layered(sfx_channel_6_state.panning, *ff7_externals.sfx_play_effects_id_channel_6, 6);
}

void ff7_sfx_stop_channel_6()
{
	if (trace_all || trace_sfx) ffnx_trace("%s\n", __func__);

	nxAudioEngine.stopSFX(6);

	if (*ff7_externals.sfx_stop_channel_timer_handle) timeKillEvent(*ff7_externals.sfx_stop_channel_timer_handle);

	*ff7_externals.sfx_stop_channel_timer_handle = 0;
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
	if (trace_all || trace_sfx) ffnx_info("%s: Battle swirl stop sound\n", __func__);

	for (int i = 0; i < 4; ++i) {
		sfx_buffers[i] = ff7_field_sfx_state();
		sfx_buffers[i].buffer1 = nullptr;
		sfx_buffers[i].buffer2 = nullptr;

		if (use_external_sfx)
		{
			// Check which SFX effects are still playing on channels before saving their state. If not, avoid playing them back.
			if (!nxAudioEngine.isSFXPlaying(i+1)) sfx_state[i].sound_id = 0;
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
	if (trace_all || trace_sfx) ffnx_info("%s: Field resume music after battle\n", __func__);

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
	if (trace_all || trace_sfx) ffnx_info("%s: Remember SFX volumes (master: %i)\n", __func__, *common_externals.master_sfx_volume);

	for (int i = 0; i < 5; ++i) {

		sfx_volumes[i] = use_external_sfx ? sfx_state[i].volume1 : (sfx_state[i].buffer1 != nullptr ? sfx_state[i].volume1 : sfx_state[i].volume2);

		if (sfx_volumes[i] > 127) {
			sfx_volumes[i] = 127;
		}

		if (trace_all || trace_sfx) ffnx_info("%s: SFX volume channel #%i: %i\n", __func__, i, sfx_volumes[i]);
	}
}

void sfx_menu_force_channel_5_volume(uint32_t volume, uint32_t channel)
{
	if (trace_all || trace_sfx) ffnx_info("%s: %d\n", __func__, volume);
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
	if (trace_all || trace_sfx) ffnx_info("%s: Update SFX volumes %d\n", __func__, modifier);

	// Set master sfx volume
	BYTE** sfx_tmp_volume = (BYTE**)(ff7_externals.config_menu_sub + ff7_externals.call_menu_sound_slider_loop_sfx_down + 0xA);

	*common_externals.master_sfx_volume = **sfx_tmp_volume + modifier;

	if (use_external_sfx) nxAudioEngine.setSFXMasterVolume(*common_externals.master_sfx_volume / 100.0f);

	// Update sfx volume in real-time for all channel
	for (int channel = 1; channel <= 5; ++channel) {
		if (use_external_sfx)
			ff7_sfx_set_volume_on_channel(sfx_volumes[channel - 1], channel);
		else
			common_externals.set_sfx_volume_on_channel(sfx_volumes[channel - 1], channel);

		if (trace_all || trace_sfx) ffnx_info("%s: Set SFX volume for channel #%i: %i\n", __func__, channel, sfx_volumes[channel - 1]);
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
	if (trace_all || trace_sfx) ffnx_info("%s", log); // FF7 default log

	for (int i = 0; i < 5; ++i) {
		if (sfx_state[i].u1 == 0xFFFFFFFF) {
			if (trace_all || trace_sfx) ffnx_info("%s: SFX fix volume channel #%i: %i\n", __func__, i + 1, real_volume);

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
	((uint32_t(*)(int, int))common_externals.sfx_load)(0x188, dsound_buffer);

	// Original call (load sound 0x285)
	return ((uint32_t(*)(int, int))common_externals.sfx_load)(sound_id, dsound_buffer);
}

void sfx_fix_cait_sith_roulette(int sound_id)
{
	((uint32_t(*)(uint32_t, uint32_t, uint32_t))common_externals.play_sfx_on_channel)(64, sound_id, 4);
}

//=============================================================================

void sfx_process_footstep(bool is_player_moving)
{
	static time_t last_playback_time, current_playback_time;

	if (is_player_moving)
	{
		float pace = 0.5f;

		// If running change the pace
		if (*ff7_externals.input_run_button_status != 0 || gamepad_analogue_intent == INTENT_RUN) pace = 0.30f;

		qpc_get_time(&current_playback_time);
		if (qpc_diff_time(&current_playback_time, &last_playback_time, nullptr) >= ((ff7_game_obj*)common_externals.get_game_object())->countspersecond * pace)
		{
			if (use_external_sfx) ff7_sfx_play_layered(0.0f, 159, 7);
			else common_externals.play_sfx(159);
			qpc_get_time(&last_playback_time);
		}
	}
}

void sfx_process_wm_footstep(int player_model_id, int player_walkmap_type)
{
	static time_t last_playback_time, current_playback_time;
	float pace = 0.3f;
	constexpr int footstep_id = 159;
	bool playing;

	if(player_model_id == 4 || player_model_id == 19)
		pace = 0.5f;

	qpc_get_time(&current_playback_time);
	if (qpc_diff_time(&current_playback_time, &last_playback_time, nullptr) >= ((ff7_game_obj*)common_externals.get_game_object())->countspersecond * pace)
	{
		char track_name[64];
		if (use_external_sfx)
		{
			sprintf(track_name, "wm_footsteps_%d_%d_%d", player_model_id, player_walkmap_type, footstep_id);
			playing = nxAudioEngine.playSFX(track_name, footstep_id, 7, 0.0f);

			if(!playing)
			{
				sprintf(track_name, "wm_footsteps_%d_%d", player_walkmap_type, footstep_id);
				playing = nxAudioEngine.playSFX(track_name, footstep_id, 7, 0.0f);
			}
		}
		else
		{
			common_externals.play_sfx(footstep_id);
		}
		qpc_get_time(&last_playback_time);
	}
}

void sfx_process_wm_highwind(bool is_old_highwind, bool is_highwind_moving)
{
	static bool playing = false;

	constexpr int default_sfx_id = 493; // Tiny Bronco SFX
	constexpr int default_channel = 1;

	if (is_highwind_moving && !playing)
	{
		if (use_external_sfx)
		{
			char track_name[64];
			sprintf(track_name, "sfx_highwind_%d", (is_old_highwind) ? 0 : 1);
			playing = nxAudioEngine.playSFX(track_name, default_sfx_id, default_channel, 0.0f, true);
		}
		else
		{
			common_externals.play_sfx_effects(64, default_sfx_id, 0, 0, 0);
			playing = true;
		}
	}
	else if (!is_highwind_moving && playing)
	{
		for(int channel = 1; channel <= 5; channel++)
			common_externals.set_sfx_frequency_on_channel(0, channel);
		((void(*)())common_externals.sfx_stop)();
		playing = false;
	}
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
		replace_call(ff7_externals.config_menu_sub + ff7_externals.call_menu_sound_slider_loop_sfx_down, sfx_menu_play_sound_down);
		replace_call(ff7_externals.config_menu_sub + ff7_externals.call_menu_sound_slider_loop_sfx_up, sfx_menu_play_sound_up);
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
		// Cait Sith Roulette fix
		replace_call_function(ff7_externals.battle_menu_state_fn_table[26] + 0xC7, sfx_fix_cait_sith_roulette);
		// Comet2 fix
		patch_code_byte(ff7_externals.comet2_unload_sub_5A4359 + 0xF1, 0x5A);

		if (use_external_sfx)
		{
			replace_function(common_externals.sfx_load, ff7_sfx_load);
			replace_function(common_externals.sfx_unload, ff7_sfx_unload);
			replace_function(common_externals.play_sfx_on_channel, ff7_sfx_play_on_channel);
			replace_function((uint32_t)common_externals.play_sfx, ff7_sfx_play_on_channel_5);
			replace_function((uint32_t)common_externals.set_sfx_volume_on_channel, ff7_sfx_set_volume_on_channel);
			replace_function((uint32_t)common_externals.set_sfx_volume_trans_on_channel, ff7_sfx_set_volume_trans_on_channel);
			replace_function((uint32_t)common_externals.set_sfx_panning_on_channel, ff7_sfx_set_panning_on_channel);
			replace_function((uint32_t)common_externals.set_sfx_panning_trans_on_channel, ff7_sfx_set_panning_trans_on_channel);
			replace_function((uint32_t)common_externals.set_sfx_frequency_on_channel, ff7_sfx_set_frequency_on_channel);
			replace_function((uint32_t)common_externals.set_sfx_frequency_trans_on_channel, ff7_sfx_set_frequency_trans_on_channel);
			replace_function(ff7_externals.sfx_load_and_play_with_speed, ff7_sfx_load_and_play_with_speed);
			replace_function(ff7_externals.sfx_play_summon, ff7_sfx_play_on_channel_5);
			replace_function(common_externals.sfx_pause, ff7_sfx_pause);
			replace_function(common_externals.sfx_resume, ff7_sfx_resume);
			replace_function(common_externals.sfx_stop, ff7_sfx_stop);
			replace_function(common_externals.sfx_release, ff7_sfx_release);
			// Replace partially some calls in ff7_sfx_play_effects
			replace_call_function((uint32_t)common_externals.play_sfx_effects + 0x183, ff7_sfx_set_frequency_on_channel_6);
			replace_call_function((uint32_t)common_externals.play_sfx_effects + 0x1A9, ff7_sfx_set_panning_on_channel_6);
			replace_call_function((uint32_t)common_externals.play_sfx_effects + 0x1D9, ff7_sfx_set_volume_on_channel_6);
			replace_call_function((uint32_t)common_externals.play_sfx_effects + 0x1E9, ff7_sfx_play_on_channel_6);
			// Required to stop channel 6 effects when required by the engine
			replace_function(ff7_externals.sfx_stop_channel_6, ff7_sfx_stop_channel_6);

			sfx_state = new ff7_field_sfx_state[5]{0};
			for (short i = 0; i < 5; i++) sfx_state[i].volume1 = 127;

			nxAudioEngine.setSFXTotalChannels(7); // Allocate 7 channels in total
			nxAudioEngine.setSFXReusableChannels(5); // The engine by default although re-uses up to 5 channels
			nxAudioEngine.addSFXLazyUnloadChannel(6); // Channel 6 will be lazy unloaded by the game engine
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
	} else if (use_external_sfx) {
		replace_function(common_externals.sfx_load, ff8_sfx_load);
		replace_function(common_externals.sfx_unload, ff8_sfx_unload);
		replace_function(common_externals.play_sfx_on_channel, ff8_sfx_play_channel);
		replace_function(common_externals.sfx_stop, ff8_sfx_stop_channel);
		replace_function(common_externals.sfx_pause, ff8_sfx_pause_channel);
		replace_function(common_externals.sfx_resume, ff8_sfx_resume_channel);
		replace_function(uint32_t(ff8_externals.sfx_set_master_volume), ff8_sfx_set_master_volume);
		replace_function(ff8_externals.sfx_is_playing, ff8_sfx_is_playing);
		replace_function(ff8_externals.sfx_set_volume, ff8_sfx_set_volume);
		replace_function(ff8_externals.sfx_set_panning, ff8_sfx_set_panning);

		nxAudioEngine.setSFXTotalChannels(FF8_MAX_CHANNEL_NUMBER + 1); // Allocate 32 channels in total
		nxAudioEngine.setSFXReusableChannels(FF8_MAX_CHANNEL_NUMBER + 1); // The engine by default although re-uses up to 32 channels
	}
}
