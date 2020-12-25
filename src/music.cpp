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

#include <windows.h>
#include <unordered_map>

#include "audio.h"
#include "music.h"
#include "patch.h"

bool was_battle_gameover = false;
bool current_music_is_field_resumable = false;
bool next_music_channel = 0;
bool next_music_is_skipped = false;
bool next_music_is_skipped_with_saved_offset = false;
std::unordered_map<uint32_t, bool> remember_musics;
float hold_volume_for_channel[2];
bool next_music_is_not_multi = false;
double next_music_fade_time = 0.0;
bool ff8_music_intro_volume_changed = false;

static uint32_t noop() { return 0; }
static uint32_t noop_a1(uint32_t a1) { return 0; }
static uint32_t noop_a2(uint32_t a1, uint32_t a2) { return 0; }
static uint32_t noop_a3(uint32_t a1, uint32_t a2, uint32_t a3) { return 0; }

void music_flush()
{
	nxAudioEngine.flush();
}

bool is_gameover(uint32_t midi)
{
	bool ret = false;

	if (ff8)
	{
		switch (midi)
		{
		case 0: // Lose
			ret = true;
		}
	}
	else
	{
		switch (midi)
		{
		case 58: // OVER2
			ret = true;
		}
	}

	return ret;
}

uint32_t music_mode(uint32_t midi)
{
	if (ff8)
	{
		switch (midi)
		{
		case 41: // FIELD (Worldmap theme)
			return MODE_WORLDMAP;
		}
	}
	else
	{
		switch (midi)
		{
		case 13: // TA (Worldmap theme 1)
		case 71: // KITA (Worldmap theme 2)
			return MODE_WORLDMAP;
		}
	}

	return getmode_cached()->driver_mode == MODE_FIELD ? MODE_FIELD : MODE_EXIT;
}

NxAudioEngine::PlayFlags needs_resume(uint32_t midi)
{
	NxAudioEngine::PlayFlags ret = NxAudioEngine::PlayFlagsNone;

	if (external_music_resume)
	{
		uint32_t mode = music_mode(midi);

		switch (mode)
		{
		case MODE_FIELD:
			if (next_music_channel == 0) {
				ret = NxAudioEngine::PlayFlagsIsResumable;

				// Field channels are exclusive
				if (current_music_is_field_resumable) {
					ret = ret | NxAudioEngine::PlayFlagsDoNotPause;
				}
			}
			break;
		case MODE_WORLDMAP:
			if (!ff8) {
				ret = NxAudioEngine::PlayFlagsIsResumable;
			}

			// Come from field
			if (current_music_is_field_resumable) {
				ret = ret | NxAudioEngine::PlayFlagsDoNotPause;
			}
			break;
		}
	}

	return ret;
}

uint32_t ff7_no_loop_ids[11] = {
	5, // FANFARE
	14, // TB
	22, // WALZ
	48, // CANNON
	57, // YADO
	89, // RO
	90, // JYRO
	92, // RIKU
	93, // SI
	94, // MOGU
	98, // ROLL
};

bool no_loop(uint32_t midi)
{
	if (ff8) {
		return false; // TODO
	}

	for (int i = 0; i < 11; ++i) {
		if (ff7_no_loop_ids[i] == midi) {
			return true;
		}
	}

	return false;
}

uint32_t ff7_midi_init(uint32_t unknown)
{
	// without this there will be no volume control for music in the config menu
	*ff7_externals.midi_volume_control = true;

	// enable fade function
	*ff7_externals.midi_initialized = true;

	return true;
}

char ff8_midi[32];

char* ff8_format_midi_name(const char* midi_name)
{
	// midi_name format: {num}{type}-{name}.sgt or {name}.sgt or _Missing.sgt
	const char* truncated_name = strchr(midi_name, '-');

	if (nullptr != truncated_name) {
		truncated_name += 1; // Remove "-"
	}
	else {
		truncated_name = midi_name;
	}

	const char* max_midi_name = strchr(truncated_name, '.');

	if (nullptr != max_midi_name) {
		size_t len = max_midi_name - truncated_name;

		if (len < 32) {
			memcpy(ff8_midi, truncated_name, len);
			ff8_midi[len] = '\0';

			return ff8_midi;
		}
	}

	return nullptr;
}

char* ff8_midi_name(uint32_t musicId)
{
	if (musicId != UINT_MAX)
	{
		const char* midi_name = common_externals.get_midi_name(musicId);
		return ff8_format_midi_name(midi_name);
	}

	return nullptr;
}

char* current_midi_name(int channel)
{
	const uint32_t musicId = nxAudioEngine.currentMusicId(channel);
	return ff8 ? ff8_midi_name(musicId) : common_externals.get_midi_name(musicId);
}

void pause_music()
{
	if (trace_all || trace_music) trace("%s: music_id=%u, midi=%s\n", __func__, nxAudioEngine.currentMusicId(0), current_midi_name(0));

	nxAudioEngine.pauseMusic(0);
	nxAudioEngine.pauseMusic(1);
}

void restart_music()
{
	if (trace_all || trace_music) trace("%s: music_id=%u, midi=%s\n", __func__, nxAudioEngine.currentMusicId(0), current_midi_name(0));

	nxAudioEngine.resumeMusic(0);
	nxAudioEngine.resumeMusic(1);
}

uint32_t ff7_use_midi(uint32_t midi)
{
	const char* name = common_externals.get_midi_name(midi);

	if (nxAudioEngine.canPlayMusic(name)) {
		return 1;
	}

	return strcmp(name, "HEART") != 0 && strcmp(name, "SATO") != 0 && strcmp(name, "SENSUI") != 0 && strcmp(name, "WIND") != 0;
}

bool play_music(char* music_name, uint32_t music_id, int channel, NxAudioEngine::PlayOptions options = NxAudioEngine::PlayOptions(), const char* fullname = nullptr)
{
	struct game_mode* mode = getmode_cached();

	bool playing = false;

	options.flags = needs_resume(music_id);

	if (ff8)
	{
		if (nxAudioEngine.canPlayMusic(music_name))
		{
			playing = nxAudioEngine.playMusic(music_name, music_id, channel, options);
		}
		else if (fullname != nullptr)
		{
			char path[MAX_PATH];

			sprintf(path, "%s%s", ff8_externals.music_path, fullname);

			if (trace_all || trace_music) info("%s: back to wav %s\n", __func__, path);

			options.useNameAsFullPath = true;
			playing = nxAudioEngine.playMusic(path, music_id, channel, options);
		}
	}
	else
	{
		// Attempt to customize the battle theme flow
		if (strcmp(music_name, "BAT") == 0)
		{
			// Do only in fields for now
			if (*common_externals._previous_mode == FF7_MODE_FIELD && mode->driver_mode == MODE_SWIRL)
			{
				char battle_name[50];

				sprintf(battle_name, "bat_%d", *ff7_externals.battle_id);

				// Attempt to load theme by Battle ID
				if (!(playing = nxAudioEngine.playMusic(battle_name, music_id, channel, options)))
				{
					sprintf(battle_name, "bat_%s", get_current_field_name());

					// Attempt to load theme by Field name
					if (!(playing = nxAudioEngine.playMusic(battle_name, music_id, channel, options)))
						// Nothing worked, switch back to default
						playing = nxAudioEngine.playMusic(music_name, music_id, channel, options);
				}
			}
			else
				playing = nxAudioEngine.playMusic(music_name, music_id, channel, options);
		}
		// Attempt to override field music
		else if (mode->driver_mode == MODE_FIELD)
		{
			char field_name[50];

			sprintf(field_name, "field_%d", *ff7_externals.field_id);

			// Attempt to load theme by Field ID
			if (!(playing = nxAudioEngine.playMusic(field_name, music_id, channel, options)))
				// Nothing worked, switch back to default
				playing = nxAudioEngine.playMusic(music_name, music_id, channel, options);
		}
		else
			playing = nxAudioEngine.playMusic(music_name, music_id, channel, options);
	}

	if (playing)
	{
		nxAudioEngine.setMusicLooping(!no_loop(music_id), channel);

		if (music_mode(music_id) == MODE_FIELD) {
			current_music_is_field_resumable = next_music_channel == 0;
		}
		else {
			current_music_is_field_resumable = false;
		}
	}

	return playing;
}

void ff7_play_midi(uint32_t music_id)
{
	const int channel = 0;

	if (nxAudioEngine.currentMusicId(channel) != music_id)
	{
		if (is_gameover(music_id)) music_flush();

		char* midi_name = common_externals.get_midi_name(music_id);
		struct game_mode* mode = getmode_cached();

		// Avoid restarting the same music when transitioning from the battle gameover to the gameover screen
		if (mode->driver_mode == MODE_GAMEOVER && was_battle_gameover)
		{
			was_battle_gameover = false;
			return;
		}

		if (mode->driver_mode == MODE_BATTLE && music_id == 58) was_battle_gameover = true;

		play_music(midi_name, music_id, channel);

		if (trace_all || trace_music) trace("%s: music_id=%u, midi=%s\n", __func__, nxAudioEngine.currentMusicId(channel), midi_name);
	}
}

void stop_music_for_channel(int channel)
{
	if (!ff8)
	{
		struct game_mode* mode = getmode_cached();

		// Do not stop the gameover music if coming from a battle
		if (mode->driver_mode == MODE_GAMEOVER && was_battle_gameover) return;
	}

	if (trace_all || trace_music) trace("%s: music_id=%u, midi=%s, channel=%d\n", __func__, nxAudioEngine.currentMusicId(channel), current_midi_name(channel), channel);

	nxAudioEngine.stopMusic(channel);

	if (ff8)
	{
		ff8_externals.current_music_ids[channel] = 0;
	}
}

void stop_music()
{
	if (trace_all || trace_music) trace("%s: music_id=%u, midi=%s\n", __func__, nxAudioEngine.currentMusicId(0), current_midi_name(0));

	stop_music_for_channel(0);
	stop_music_for_channel(1);
}

void ff7_cross_fade_midi(uint32_t music_id, uint32_t steps)
{
	char* midi_name = common_externals.get_midi_name(music_id);
	const int channel = 0;

	/* FIXME: the game uses cross_fade_midi only in two places,
	 * with steps = 4 everytime. In the PS version, theses transitions
	 * last 1-2 seconds fade in / out, no more.
	 * Therefore steps value is clearly wrong here, so the formula
	 * to get the correct time is different than the one in set_midi_volume_trans.
	 */
	double time = (steps & 0xFF) / 4.0;

	if (music_id != 0)
	{
		if (music_id == 1)
		{
			music_id = (nxAudioEngine.currentMusicId(channel) == 2) + 1;
		}
		else if (music_id == 2)
		{
			music_id = (nxAudioEngine.currentMusicId(channel) != 1) + 1;
		}

		if (nxAudioEngine.currentMusicId(channel) != music_id)
		{
			NxAudioEngine::PlayOptions options = NxAudioEngine::PlayOptions();
			options.fadetime = time;
			options.targetVolume = 1.0f;
			play_music(midi_name, music_id, channel, options);
		}
		else
		{
			nxAudioEngine.setMusicVolume(1.0f, channel, time);
		}
	}
	else
	{
		stop_music();
	}

	if (trace_all || trace_music) trace("%s: music_id=%u, midi=%s, time=%fs\n", __func__, music_id, midi_name, time);
}

uint32_t music_status()
{
	if (trace_all || trace_music) trace("%s: music_id=%u, midi=%s\n", __func__, nxAudioEngine.currentMusicId(0), current_midi_name(0));

	// When the game asks for a music status, you know that it ends eventually
	nxAudioEngine.setMusicLooping(false, 0);
	nxAudioEngine.setMusicLooping(false, 1);

	return nxAudioEngine.isMusicPlaying(0) || nxAudioEngine.isMusicPlaying(1);
}

void set_master_midi_volume(uint32_t volume)
{
	if (trace_all || trace_music) trace("%s: volume=%u\n", __func__, volume);

	nxAudioEngine.setMusicMasterVolume(volume / 100.0f);
}

void set_music_volume(uint32_t volume)
{
	const int channel = 0;

	if (volume > 127) volume = 127;

	if (trace_all || trace_music) trace("%s: volume=%u, channel=%u\n", __func__, volume, channel);

	nxAudioEngine.setMusicVolume(volume / 127.0f, channel);
}

void set_volume_trans(int channel, double volume, double time)
{
	if (time != 0.0)
	{
		if (volume == 0.0)
		{
			nxAudioEngine.stopMusic(channel, time);
		}
		else
		{
			nxAudioEngine.setMusicVolume(volume, channel, time);
		}
	}
	else if (volume != 0.0)
	{
		nxAudioEngine.setMusicVolume(volume, channel);
	}
	else
	{
		stop_music_for_channel(channel);
	}
}

void ff7_volume_trans(uint32_t volume, uint32_t steps)
{
	const int channel = 0;

	if (volume > 127) volume = 127;

	double time = (steps & 0xFF) / 64.0;

	if (trace_all || trace_music) trace("%s: volume=%u, steps=%u (=> time=%fs), channel=%u\n", __func__, volume, steps, time, channel);

	set_volume_trans(channel, volume / 127.0, time);
}

void set_midi_tempo(int8_t tempo)
{
	const int channel = 0;

	if (trace_all || trace_music) trace("%s: tempo=%d, channel=%u\n", __func__, tempo, channel);

	if (tempo == -128) {
		tempo = -127; // Prevent speed to be 0 (can crash with SoLoud)
	}

	float speed = float(tempo) / 128.0f + 1.0f;

	// FIXME: will change the pitch
	nxAudioEngine.setMusicSpeed(speed, channel);
}

uint32_t ff7_music_sound_operation_fix(uint32_t type, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4, uint32_t param5)
{
	if (trace_all || trace_music) trace("%s: AKAO call type=%X params=(%i %i %i %i)\n", __func__, type, param1, param2, param3, param4, param5);

	type &= 0xFF; // The game does not always set this parameter as a 32-bit integer

	if (type == 0xDA) { // Assimilated to stop music (Cid speech in Highwind)
		return ((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))ff7_externals.sound_operation)(0xF0, 0, 0, 0, 0, 0);
	}

	// Play music (channel #2) and Play music with fade (channel #2)
	if (use_external_music && (type == 0x14 || type == 0x19)) {
		const uint32_t music_id = param1;
		if (music_id > 0 && music_id <= 0x62) {
			if (trace_all || trace_music) trace("%s: set field music channel to 1\n", __func__);
			next_music_channel = 1;
		}
	}

	uint32_t ret = ((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))ff7_externals.sound_operation)(type, param1, param2, param3, param4, param5);
	next_music_channel = 0;
	return ret;
}

uint32_t ff8_remember_playing_time()
{
	if (trace_all || trace_music) trace("%s\n", __func__);

	nxAudioEngine.pauseMusic(0, 0.0, true);
	// We never remember dualmusic, but battle and cardgame do not pause or stop channel 1 before playing the next music
	stop_music_for_channel(1);

	return 0;
}

uint32_t* ff8_load_music(uint32_t channel, uint32_t music_id, uint32_t data)
{
	if (trace_all || trace_music) trace("%s: channel=%u, music_id=%u, data=%u\n", __func__, channel, music_id, data);

	// Do not apply volume changes for this channel between load_music and change_music/dual_music/replay_music instructions
	hold_volume_for_channel[channel] = true;

	return ((uint32_t * (*)(uint32_t, uint32_t, uint32_t))ff8_externals.music_load)(channel, music_id, data);
}

uint32_t ff8_play_midi(uint32_t music_id, int32_t volume, uint32_t unused1, uint32_t unused2)
{
	const int channel = next_music_channel;

	if (nxAudioEngine.currentMusicId(channel) != music_id)
	{
		if (is_gameover(music_id)) music_flush();

		char* music_name = ff8_midi_name(music_id);

		if (nullptr == music_name) {
			error("%s: Cannot get music name from music_id %d\n", __func__, music_id);
			return 0; // Error
		}

		if (trace_all || trace_music) trace("%s: music_id=%u, name=%s, channel=%d, volume=%d\n", __func__, music_id, music_name, channel, volume);

		SoLoud::time offset = 0;
		bool noIntro = false;

		if (next_music_is_skipped_with_saved_offset && remember_musics[music_id]) {
			remember_musics[music_id] = false;
			if (trace_all || trace_music) trace("%s: use remembered music time\n", __func__);
		}
		else if (next_music_is_skipped) {
			noIntro = true;
		}
		else if (remember_musics[music_id]) {
			if (trace_all || trace_music) trace("%s: discard remembered music\n", __func__);
			remember_musics[music_id] = false;
		}

		NxAudioEngine::PlayOptions options = NxAudioEngine::PlayOptions();
		options.fadetime = next_music_fade_time;
		options.noIntro = noIntro;
		if (volume >= 0 && volume <= 127) {
			options.targetVolume = volume / 127.0f;
		}
		play_music(music_name, music_id, channel, options);
	}
	else if (trace_all || trace_music) {
		trace("%s: is already playing music_id=%u, channel=%d, volume=%d\n", __func__, music_id, channel, volume);
	}

	return 1; // Success
}

uint32_t ff8_play_wav(const char* filename, uint32_t volume)
{
	const int channel = next_music_channel;
	uint32_t music_id = ff8_externals.current_music_ids[channel];

	if (nxAudioEngine.currentMusicId(channel) != music_id)
	{
		if (is_gameover(music_id)) music_flush();

		char* music_name = ff8_format_midi_name(filename);

		if (nullptr == music_name) {
			error("%s: Cannot get music name from filename %s\n", __func__, filename);
			return 0; // Error
		}

		if (trace_all || trace_music) trace("%s: music_id=%u, name=%s, channel=%d, volume=%u\n", __func__, music_id, music_name, channel, volume);

		NxAudioEngine::PlayOptions options = NxAudioEngine::PlayOptions();
		options.fadetime = next_music_fade_time;
		if (volume >= 0 && volume < 127) {
			options.targetVolume = volume / 127.0f;
		}
		play_music(music_name, music_id, channel, options, filename);
	}
	else if (trace_all || trace_music) {
		trace("%s: is already playing music_id=%u, filename=%s, channel=%d, volume=%d\n", __func__, music_id, filename, channel, volume);
	}

	return 1; // Success
}

uint32_t ff8_opcode_dualmusic_play_music(char* midi_data, uint32_t volume)
{
	uint32_t channel = 1;

	if (trace_all || trace_music) trace("%s: channel=%u, volume=%u\n", __func__, channel, volume);

	next_music_channel = channel;
	channel = ff8_externals.sd_music_play(channel, midi_data, volume);
	next_music_channel = 0;
	return channel;
}

uint32_t ff8_cross_fade_midi(char* midi_data, uint32_t steps, uint32_t volume)
{
	uint32_t channel = 0;
	double time = steps / 50.0;

	if (trace_all || trace_music) trace("%s: steps=%u (time=%fs), volume=%u\n", __func__, steps, time, volume);

	next_music_fade_time = time;
	channel = ff8_externals.sd_music_play(channel, midi_data, volume);
	next_music_fade_time = 0;
	return channel;
}

uint32_t ff8_play_music_to_channel_0(char* midi_data)
{
	uint32_t channel = 0;

	hold_volume_for_channel[channel] = false;

	if (trace_all || trace_music) trace("%s\n", __func__);
	// Force known volume value
	return ff8_externals.sd_music_play(channel, midi_data, uint32_t(nxAudioEngine.getMusicVolume(channel) * 127));
}

uint32_t ff8_play_midi_at(char* midi_data, uint32_t offset)
{
	// We don't know what offset means in seconds
	if (trace_all || trace_music) trace("%s: play midi at %d\n", __func__, offset);
	next_music_is_skipped = true;
	uint32_t channel = ff8_play_music_to_channel_0(midi_data);
	next_music_is_skipped = false;
	return channel;
}

uint32_t ff8_opcode_musicskip_play_midi_at(char* midi_data, uint32_t offset)
{
	uint32_t channel = 0;

	hold_volume_for_channel[channel] = false;

	if (trace_all || trace_music) trace("%s: music skip, play midi at %d\n", __func__, offset);
	next_music_is_skipped_with_saved_offset = offset & 0xFF == 0xFF; // Special offset returned by ff8_opcode_getmusicoffset()
	channel = ff8_play_midi_at(midi_data, offset);
	next_music_is_skipped_with_saved_offset = false;
	return channel;
}

uint32_t ff8_opcode_getmusicoffset()
{
	const uint32_t channel = 0;
	const uint32_t musicId = nxAudioEngine.currentMusicId(channel);

	if (trace_all || trace_music) trace("%s: save music %d\n", __func__, musicId);

	remember_musics[musicId] = true;
	// Force current music to be pushed onto the stack
	current_music_is_field_resumable = false;

	return 0xFFFFFFFF; // Return a special offset to recognize it in ff8_opcode_musicskip_play_midi_at()
}

uint32_t set_music_volume_for_channel(int32_t channel, uint32_t volume)
{
	if (trace_all || trace_music) trace("%s: channel=%d, volume=%d, hold=%d\n", __func__, channel, volume, hold_volume_for_channel[channel]);

	if (hold_volume_for_channel[channel] || channel < 0 || channel > 1) {
		return 1;
	}

	if (volume > 127) volume = 127;

	nxAudioEngine.setMusicVolume(volume / 127.0f, channel);

	return 1; // Success
}

uint32_t ff8_volume_trans(int32_t channel, uint32_t steps, uint32_t volume)
{
	double time = steps / 50.0;

	if (trace_all || trace_music) trace("%s: channel=%d, volume=%u, steps=%u (=> time=%fs), hold=%d\n", __func__, channel, volume, steps, time, hold_volume_for_channel[channel]);

	if (hold_volume_for_channel[channel] || channel < 0 || channel > 1) {
		return 1;
	}

	if (volume > 127) volume = 127;

	nxAudioEngine.setMusicVolume(volume / 127.0f, channel, time);

	return 1;
}

uint32_t ff8_volume_fade(uint32_t channel, uint32_t steps, uint32_t volume1, uint32_t volume2)
{
	if (trace_all || trace_music) trace("%s: channel=%d, volume1=%u, volume2=%u, steps=%u\n", __func__, channel, volume1, volume2, steps);

	set_music_volume_for_channel(channel, volume1);
	ff8_volume_trans(channel, steps, volume2);

	return 1;
}

uint32_t ff8_volume_sync()
{
	if (trace_all || trace_music) trace("%s\n", __func__);

	if (nxAudioEngine.isMusicVolumeFadeFinished()) {
		return 2; // Continue
	}

	return 1; // Wait
}

uint32_t ff8_play_music_worldmap(char* midi_data)
{
	if (trace_all || trace_music) trace("%s\n", __func__);

	nxAudioEngine.setMusicVolume(0.0f, 0, 0.5); // Fadeout: 500ms
	return ff8_cross_fade_midi(midi_data, 60, 127); // Fadein: ~1s
}

std::vector<std::string> musics;

uint32_t ff8_opcode_choicemusic(uint32_t unused, uint32_t instruments)
{
	if (trace_all || trace_music) trace("%s: Clear musics\n", __func__);

	musics.clear();

	return ((uint32_t(*)(uint32_t, uint32_t))ff8_externals.choice_music)(unused, instruments);
}

uint32_t ff8_load_midi_segment(void* directsound, const char* filename)
{
	char* midi_name = ff8_format_midi_name(filename);

	if (next_music_is_not_multi) {
		next_music_is_not_multi = false;
		play_music(midi_name, 43, 0);
		return 1; // Success
	}

	if (trace_all || trace_music) trace("%s: load music %s (%s)\n", __func__, midi_name, filename);

	musics.push_back(midi_name);

	return 1; // Success
}

uint32_t ff8_play_midi_segments()
{
	const int channel = 0;

	if (trace_all || trace_music) trace("%s\n", __func__);

	nxAudioEngine.playSynchronizedMusics(musics, 43);
	nxAudioEngine.setMusicLooping(true, channel);

	return 0;
}

uint32_t ff8_load_and_play_one_midi_segment(uint32_t segment_id)
{
	// In this call we know that we play only one segment
	next_music_is_not_multi = true;
	((uint32_t(*)(uint32_t))ff8_externals.load_midi_segment_from_id)(segment_id);
	next_music_is_not_multi = false;

	return 0; // Fail (to prevent execution of the game's code)
}

uint32_t ff8_set_music_volume_intro_credits(uint32_t channel, uint32_t volume)
{
	if (ff8_music_intro_volume_changed) {
		if (nxAudioEngine.isMusicVolumeFadeFinished()) {
			*ff8_externals.credits_counter = 255; // Stop intro credits
			ff8_music_intro_volume_changed = false;
		}

		return 1;
	}

	if (trace_all || trace_music) trace("%s %d %d\n", __func__, channel, volume);

	if (nxAudioEngine.isMusicPlaying(channel)) {
		nxAudioEngine.stopMusic(channel, 2.0);
		ff8_music_intro_volume_changed = true;
	}
	else {
		*ff8_externals.credits_counter = 255; // Stop intro credits
	}

	return 1;
}

void music_init()
{
	if (!ff8)
	{
		// Fix music stop issue in FF7
		patch_code_dword(ff7_externals.music_lock_clear_fix + 2, 0xCC195C);
		// Fix Cid speech music stop + music channel detection (field only)
		replace_call(ff7_externals.opcode_akao + 0xEA, ff7_music_sound_operation_fix);
		replace_call(ff7_externals.opcode_akao2 + 0xE8, ff7_music_sound_operation_fix);
		replace_call(ff7_externals.field_music_helper_sound_op_call, ff7_music_sound_operation_fix);
	}

	if (use_external_music)
	{
		if (ff8)
		{
			/* Play & Stop */
			replace_function(common_externals.play_midi, ff8_play_midi);
			replace_function(common_externals.play_wav, ff8_play_wav);
			// Removing stop_midi call from sd_music_play
			replace_call(uint32_t(ff8_externals.sd_music_play) + 0x1B1, noop_a1);
			replace_function(ff8_externals.sdmusicplay, ff8_play_music_to_channel_0);
			replace_function(common_externals.pause_midi, pause_music);
			replace_function(common_externals.pause_wav, noop_a1);
			replace_function(common_externals.restart_midi, restart_music);
			replace_function(common_externals.restart_wav, noop_a1);
			replace_function(ff8_externals.stop_music, stop_music_for_channel);
			// Called by game credits
			replace_function(common_externals.stop_midi, stop_music);
			replace_function(common_externals.midi_status, music_status);
			// Music channel detection
			replace_call(ff8_externals.opcode_dualmusic + 0x58, ff8_opcode_dualmusic_play_music);
			/* Volume */
			replace_function(common_externals.cross_fade_midi, ff8_cross_fade_midi);
			replace_function(common_externals.set_midi_volume_trans, ff8_volume_trans);
			replace_function(common_externals.set_midi_volume_fade, ff8_volume_fade);
			replace_function(common_externals.set_midi_volume, set_music_volume_for_channel);
			// Not implemented by the game
			replace_function(ff8_externals.opcode_musicvolsync, ff8_volume_sync);
			// Called by game credits
			replace_function(ff8_externals.set_midi_volume, set_music_volume);
			// Fix intro credits volume fadeout time
			replace_call(ff8_externals.sub_52F300 + 0x5DF, ff8_set_music_volume_intro_credits);
			replace_call(ff8_externals.sub_52F300 + 0x5C2, noop_a1);
			// Worldmap: Replace stop and play music by a cross fade on leaving cities
			replace_call(ff8_externals.worldmap_sub_53F310_loc_53F7EE + 0x2, noop_a2); // set volume to 0
			replace_call(ff8_externals.worldmap_sub_53F310_loc_53F7EE + 0x8, noop_a1); // stop music
			replace_call(ff8_externals.worldmap_sub_53F310_loc_53F7EE + 0x28, ff8_play_music_worldmap); // play music
			replace_call(ff8_externals.worldmap_sub_53F310_loc_53F7EE + 0x3B, noop_a3); // volume fade (channel=0, steps=60, volume=127)
			/* Remember time and resume music */
			replace_function(ff8_externals.sd_music_play_at, ff8_play_midi_at);
			replace_function(common_externals.remember_midi_playing_time, ff8_remember_playing_time);
			replace_call(ff8_externals.opcode_musicskip + 0x46, ff8_opcode_musicskip_play_midi_at);
			// getmusicoffset opcode is not implemented, but could be used to skip music with musicskip opcode
			replace_call(ff8_externals.opcode_getmusicoffset, ff8_opcode_getmusicoffset);
			/* MIDI segments (Fisherman's Horizon concert instruments) */
			// Initialization
			replace_call(ff8_externals.opcode_choicemusic + 0x5D, ff8_opcode_choicemusic);
			replace_function(ff8_externals.load_midi_segment, ff8_load_midi_segment);
			replace_function(ff8_externals.play_midi_segments, ff8_play_midi_segments);
			// The next played song will stop the previous one
			replace_function(ff8_externals.stop_midi_segments, noop);
			// Detect solo play_midi
			replace_call(ff8_externals.load_and_play_midi_segment + 0x41, ff8_load_and_play_one_midi_segment);
			/* Nullify MIDI subs */
			replace_function(ff8_externals.volume_music_update, noop_a1);
			replace_function(ff8_externals.dmusic_segment_connect_to_dls, noop_a2);
			replace_function(common_externals.midi_cleanup, noop);
			replace_function(common_externals.wav_cleanup, noop);
			replace_call(ff8_externals.opcode_musicload + 0x8C, ff8_load_music);
		}
		else
		{
			replace_function(common_externals.midi_init, ff7_midi_init);
			replace_function(common_externals.use_midi, ff7_use_midi);
			replace_function(common_externals.play_midi, ff7_play_midi);
			replace_function(common_externals.cross_fade_midi, ff7_cross_fade_midi);
			replace_function(common_externals.pause_midi, pause_music);
			replace_function(common_externals.restart_midi, restart_music);
			replace_function(common_externals.stop_midi, stop_music);
			replace_function(common_externals.midi_status, music_status);
			replace_function(common_externals.set_master_midi_volume, set_master_midi_volume);
			replace_function(common_externals.set_midi_volume, set_music_volume);
			replace_function(common_externals.set_midi_volume_trans, ff7_volume_trans);
			replace_function(common_externals.set_midi_tempo, set_midi_tempo);
			replace_function(common_externals.midi_cleanup, noop);
		}
	}
}
