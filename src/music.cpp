/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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
#include "ff8/remaster.h"
#include "audio/vgmstream/zzzstreamfile.h"

#include "ff8/engine.h"

bool was_battle_gameover = false;
bool next_music_channel = 0;
bool next_music_is_skipped = false;
bool next_music_is_skipped_with_saved_offset = false;
std::unordered_map<uint32_t, bool> remember_musics;
bool hold_volume_for_channel[2] = { false, false };
bool next_music_is_not_multi = false;
double next_music_fade_time = 0.0;
bool next_music_is_battle = false;
bool next_music_is_world = false;
uint16_t next_battle_scene_id = 0;
bool ff8_music_intro_volume_changed = false;
char* eyes_on_me_track = "eyes_on_me";
bool eyes_on_me_is_playing = false;
uint8_t ff7_last_akao_call_type = 0;
uint32_t ff7_last_music_id = 0;
uint32_t ff7_last_region_id = -1;
int16_t ff7_next_field_music_relative_id = -1;

void handle_mainmenu_playback()
{
	struct game_mode *mode = getmode_cached();
	static bool is_main_menu = false;

	switch (mode->driver_mode)
	{
	case MODE_MAIN_MENU:
		if (!is_main_menu)
		{
			is_main_menu = true;

			nxAudioEngine.playMusic("main_menu", 0xFF, 0);
		}
		break;
	default:
		if (is_main_menu)
		{
			nxAudioEngine.stopMusic();
			is_main_menu = false;
		}
		break;
	}
}

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
	const char* truncated_name = midi_name;

	if (!ff8_external_music_force_original_filenames) {
		truncated_name = strchr(midi_name, '-');

		if (nullptr != truncated_name) {
			truncated_name += 1; // Remove "-"
		}
		else {
			truncated_name = midi_name;
		}
	}

	// Remove extension
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
	if (trace_all || trace_music) ffnx_trace("%s: music_id=%u, midi=%s\n", __func__, nxAudioEngine.currentMusicId(0), current_midi_name(0));

	nxAudioEngine.pauseMusic();
}

void restart_music()
{
	if (trace_all || trace_music) ffnx_trace("%s: music_id=%u, midi=%s\n", __func__, nxAudioEngine.currentMusicId(0), current_midi_name(0));

	nxAudioEngine.resumeMusic();
}

uint32_t ff7_use_midi(uint32_t midi)
{
	const char* name = common_externals.get_midi_name(midi);

	if (nxAudioEngine.canPlayMusic(name)) {
		return 1;
	}

	return strcmp(name, "HEART") != 0 && strcmp(name, "SATO") != 0 && strcmp(name, "SENSUI") != 0 && strcmp(name, "WIND") != 0;
}

bool play_music(const char* music_name, uint32_t music_id, int channel, NxAudioEngine::MusicOptions options = NxAudioEngine::MusicOptions())
{
	const struct game_mode* mode;
	bool playing = false;
	char new_music_name[50];

	if (nxAudioEngine.isMusicDisabled(music_name)) {
		ff7_next_field_music_relative_id = -1;

		return false;
	}

	if (ff8)
	{
		const char* current_party_leader = ff8_names[*(byte*)(ff8_externals.field_vars_stack_1CFE9B8 + 0xCB) == 62 ? 8 : 0].c_str();

		mode = getmode();

		// Attempt to override battle music
		if (next_music_is_battle)
		{
			if (next_battle_scene_id > 0)
			{
				sprintf(new_music_name, "%s_%s_%u", music_name, current_party_leader, next_battle_scene_id);

				// Attempt to load theme by party leader and battle id
				playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);

				if (!playing)
				{
					sprintf(new_music_name, "%s_%u", music_name, next_battle_scene_id);

					// Attempt to load theme by battle id
					playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);
				}
			}

			next_music_is_battle = false;
		}
		// Attempt to override field music
		else if (mode->driver_mode == MODE_FIELD)
		{
			sprintf(new_music_name, "%s_field_%s_%s", music_name, current_party_leader, get_current_field_name());

			// Attempt to load theme by party leader and map name
			playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);

			if (!playing)
			{
				sprintf(new_music_name, "%s_field_%s", music_name, get_current_field_name());

				// Attempt to load theme by map name
				playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);
			}
		}

		if (!playing)
		{
			sprintf(new_music_name, "%s_%s", music_name, current_party_leader);

			// Attempt to load current music name using the party leader in the name
			playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);
		}

		if (!playing) {
			playing = nxAudioEngine.playMusic(music_name, music_id, channel, options);
		}
	}
	else
	{
		const uint32_t main_theme_midi_id = 13; // The Main Theme is always resumed

		mode = getmode_cached();

		if (external_music_resume) {
			if (nxAudioEngine.currentMusicId(0) == main_theme_midi_id || channel == 1) {
				// Backup current state of the music
				nxAudioEngine.pauseMusic(0, options.fadetime == 0.0 ? (next_music_is_battle && !external_music_sync ? 0.2 : 1.0) : options.fadetime, true);
			}
			else if (channel == 0) {
				// Channel 1 is never resumed
				nxAudioEngine.stopMusic(1, options.fadetime == 0.0 ? 1.0 : options.fadetime);
			}
		}

		// Attempt to customize the battle theme flow
		if (next_music_is_battle)
		{
			uint16_t battle_id = next_battle_scene_id;

			if (mode->driver_mode == MODE_FIELD)
			{
				battle_id = ff7_externals.modules_global_object->battle_id;
			}

			if (battle_id > 0)
			{
				sprintf(new_music_name, "bat_%u_a%d", battle_id, ff7_externals.world_get_player_walkmap_region());

				// Attempt to load theme by Battle ID + WM region
				playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);

				if (!playing)
				{
					sprintf(new_music_name, "bat_%u", battle_id);

					// Attempt to load theme by Battle ID
					playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);
				}

				if (!playing && *common_externals._previous_mode == FF7_MODE_FIELD)
				{
					sprintf(new_music_name, "bat_%s", get_current_field_name());

					// Attempt to load theme by Field name
					playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);
				}

				if (!playing)
				{
					sprintf(new_music_name, "bat_a%d", ff7_externals.world_get_player_walkmap_region());

					// Attempt to load theme by Battle WM region
					playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);
				}
			}
			else
			{
				if (trace_all || trace_music) ffnx_warning("%s: Unknown battle_id\n", __func__);
			}
		}
		else if (next_music_is_world)
		{
			ff7_last_region_id = ff7_externals.world_get_player_walkmap_region();

			sprintf(new_music_name, "%s_a%d", music_name, ff7_last_region_id);

			// Since world music comes with the same ID, we need to stop manually the channel to allow the new per region file to load again
			if (nxAudioEngine.canPlayMusic(new_music_name)) nxAudioEngine.stopMusic(channel);

			playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);
		}
		// Attempt to override field music
		else if (mode->driver_mode == MODE_FIELD)
		{
			if (ff7_next_field_music_relative_id >= 0)
			{
				sprintf(new_music_name, "field_%s_%d", get_current_field_name(), ff7_next_field_music_relative_id);

				// Attempt to load theme by map name + relative field music id
				playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);
			}

			if (!playing)
			{
				sprintf(new_music_name, "field_%s", get_current_field_name());

				// Attempt to load theme by map name
				playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);
				if (!playing)
				{
					sprintf(new_music_name, "field_%d", *ff7_externals.field_id);

					// Attempt to load theme by Field ID
					playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);
				}
			}
		}
		else if (mode->driver_mode == MODE_CONDOR)
		{
			sprintf(new_music_name, "condor_%s", music_name);

			playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);
		}
		else if (mode->driver_mode == MODE_SNOWBOARD)
		{
			sprintf(new_music_name, "snowboard_%s", music_name);

			playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);
		}
		else if (mode->driver_mode == MODE_HIGHWAY)
		{
			sprintf(new_music_name, "highway_%s", music_name);

			playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);
		}
		else if (mode->driver_mode == MODE_CHOCOBO)
		{
			sprintf(new_music_name, "chocobo_%s", music_name);

			playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);
		}
		else if (mode->driver_mode == MODE_CREDITS)
		{
			sprintf(new_music_name, "credits_%s", music_name);

			playing = nxAudioEngine.playMusic(new_music_name, music_id, channel, options);
		}

		if (!playing)
		{
			// Nothing worked, switch back to default
			playing = nxAudioEngine.playMusic(music_name, music_id, channel, options);
		}

		ff7_next_field_music_relative_id = -1;
	}

	if (playing)
	{
		nxAudioEngine.setMusicLooping(!no_loop(music_id), channel);
	}

	return playing;
}

bool play_music_ff8(const char* music_name, uint32_t music_id, int channel, NxAudioEngine::MusicOptions options = NxAudioEngine::MusicOptions(), char* wav_fullpath = nullptr)
{
	bool playing = play_music(music_name, music_id, channel, options);

	options.useNameAsFullPath = true;

	if (!playing && wav_fullpath != nullptr) {
		if (trace_all || trace_music) ffnx_info("%s: back to wav %s\n", __func__, wav_fullpath);

		strcpy(options.format, "wav");
		playing = nxAudioEngine.playMusic(wav_fullpath, music_id, channel, options);
	}

	if (!playing && remastered_edition) {
		char file_name[MAX_PATH] = {};

		if (wav_fullpath != nullptr) {
			snprintf(file_name, sizeof(file_name), "zzz://%s", wav_fullpath);
			strcpy(options.format, "wav");
			playing = nxAudioEngine.playMusic(file_name, music_id, channel, options);
		} else {
			bool old_ff8_external_music_force_original_filenames = ff8_external_music_force_original_filenames;
			ff8_external_music_force_original_filenames = true;
			music_name = ff8_midi_name(music_id);
			ff8_external_music_force_original_filenames = old_ff8_external_music_force_original_filenames;

			snprintf(file_name, sizeof(file_name), "zzz://data\\music\\dmusic\\ogg\\%s.ogg", music_name);
			strcpy(options.format, "ogg");
			playing = nxAudioEngine.playMusic(file_name, music_id, channel, options);
		}
	}

	return playing;
}

void ff7_play_midi(uint32_t music_id)
{
	const int channel = external_music_resume ? next_music_channel : 0;

	if (nxAudioEngine.currentMusicId(0) != music_id && nxAudioEngine.currentMusicId(1) != music_id)
	{
		if (is_gameover(music_id)) music_flush();

		const char* midi_name = common_externals.get_midi_name(music_id);
		struct game_mode* mode = getmode_cached();

		// Avoid restarting the same music when transitioning from the battle gameover to the gameover screen
		if (mode->driver_mode == MODE_GAMEOVER && was_battle_gameover)
		{
			was_battle_gameover = false;
			return;
		}

		if (mode->driver_mode == MODE_BATTLE && is_gameover(music_id)) was_battle_gameover = true;

		play_music(midi_name, music_id, channel);

		if (trace_all || trace_music) ffnx_trace("%s: music_id=%u, midi=%s\n", __func__, nxAudioEngine.currentMusicId(channel), midi_name);
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

	if (trace_all || trace_music) ffnx_trace("%s: music_id=%u, midi=%s, channel=%d\n", __func__, nxAudioEngine.currentMusicId(channel), current_midi_name(channel), channel);

	nxAudioEngine.stopMusic(channel);

	if (ff8)
	{
		ff8_externals.current_music_ids[channel] = 0;
	}
}

void stop_music()
{
	if (trace_all || trace_music) ffnx_trace("%s: music_id=%u, midi=%s\n", __func__, nxAudioEngine.currentMusicId(0), current_midi_name(0));

	stop_music_for_channel(0);
	stop_music_for_channel(1);
}

void ff7_cross_fade_midi(uint32_t music_id, uint32_t steps)
{
	const char* midi_name = common_externals.get_midi_name(music_id);
	const int channel = external_music_resume ? next_music_channel : 0;

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
			NxAudioEngine::MusicOptions options = NxAudioEngine::MusicOptions();
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

	if (trace_all || trace_music) ffnx_trace("%s: music_id=%u, midi=%s, time=%fs\n", __func__, music_id, midi_name, time);
}

uint32_t music_status()
{
	if (trace_all || trace_music) ffnx_trace("%s: music_id=%u, midi=%s\n", __func__, nxAudioEngine.currentMusicId(0), current_midi_name(0));

	if (ff8) {
		// When the game asks for a music status, you know that it ends eventually
		nxAudioEngine.setMusicLooping(false, 0);
		nxAudioEngine.setMusicLooping(false, 1);
	}

	return nxAudioEngine.isMusicPlaying(0) || nxAudioEngine.isMusicPlaying(1);
}

void set_master_midi_volume(uint32_t volume)
{
	if (trace_all || trace_music) ffnx_trace("%s: volume=%u\n", __func__, volume);

	nxAudioEngine.setMusicMasterVolume(volume / 100.0f);
}

void set_music_volume(uint32_t volume)
{
	const int channel = 0;

	if (volume > 127) volume = 127;

	if (trace_all || trace_music) ffnx_trace("%s: volume=%u, channel=%u\n", __func__, volume, channel);

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

	if (trace_all || trace_music) ffnx_trace("%s: volume=%u, steps=%u (=> time=%fs), channel=%u\n", __func__, volume, steps, time, channel);

	set_volume_trans(channel, volume / 127.0, time);
}

void set_midi_tempo(int8_t tempo)
{
	const int channel = 0;

	if (trace_all || trace_music) ffnx_trace("%s: tempo=%d, channel=%u\n", __func__, tempo, channel);

	if (tempo == -128) {
		tempo = -127; // Prevent speed to be 0 (can crash with SoLoud)
	}

	float speed = float(tempo) / 128.0f + 1.0f;

	// FIXME: will change the pitch
	nxAudioEngine.setMusicSpeed(speed, channel);
}

uint32_t ff7_music_sound_operation_fix(uint32_t type, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4, uint32_t param5)
{
	if (trace_all || trace_music) ffnx_trace("%s: AKAO call type=%X params=(%i %i %i %i)\n", __func__, type, param1, param2, param3, param4, param5);

	type &= 0xFF; // The game does not always set this parameter as a 32-bit integer

	if (type == 0xDA) { // Assimilated to stop music (Cid speech in Highwind)
		return ((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))ff7_externals.sound_operation)(0xF0, 0, 0, 0, 0, 0);
	}

	// Play music (channel #2) and Play music with fade (channel #2)
	if (use_external_music && (type == 0x14 || type == 0x19)) {
		const uint32_t music_id = param1;
		if (music_id > 0 && music_id <= 0x62) {
			if (trace_all || trace_music) ffnx_trace("%s: set music channel to 1\n", __func__);
			next_music_channel = 1;
		}
	}

	uint32_t ret = ((uint32_t(*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))ff7_externals.sound_operation)(type, param1, param2, param3, param4, param5);
	next_music_channel = 0;
	return ret;
}

uint32_t ff7_battle_music(uint32_t type, uint32_t music_id, uint32_t fadetime, uint32_t param3, uint32_t param4, uint32_t param5)
{
	if (trace_all || trace_music) ffnx_trace("%s: music_id=%d\n", __func__, music_id);

	next_music_is_battle = true;

	uint32_t ret = ff7_music_sound_operation_fix(type, music_id, fadetime, param3, param4, param5);

	next_music_is_battle = false;

	return ret;
}

uint32_t ff7_battle_music_fanfare()
{
	if (trace_all || trace_music) ffnx_trace("%s: set music channel to 1\n", __func__);

	next_music_channel = 1;

	uint32_t ret = ff7_externals.play_battle_end_music();

	next_music_channel = 0;

	return ret;
}

uint32_t ff7_worldmap_music_change(uint32_t type, uint32_t music_id, uint32_t fadetime, uint32_t param3, uint32_t param4, uint32_t param5)
{
	if (trace_all || trace_music) ffnx_trace("%s\n", __func__);

	if (music_id == 7) { // Battle
		ffnx_trace("%s: force music type to channel 1\n", __func__);

		next_music_is_battle = true;

		switch (type) {
		case 0x10:
			type = 0x14;
			break;
		case 0x18:
			type = 0x19;
			break;
		}
	}
	else
		next_music_is_world = true;

	// Backup music operation type for custom battle music call
	ff7_last_akao_call_type = type;
	ff7_last_music_id = music_id;

	return 0;
}

void ff7_worldmap_on_loop()
{
	(*ff7_externals.world_dword_DE68FC)();

	// If we change region while moving in the worldmap, trigger a new music change
	if (ff7_last_region_id != ff7_externals.world_get_player_walkmap_region()) next_music_is_world = true;

	if (next_music_is_battle || next_music_is_world)
	{
		ff7_music_sound_operation_fix(ff7_last_akao_call_type, ff7_last_music_id, 4, 0, 0, 0);

		next_music_is_battle = false;
		next_music_is_world = false;
	}
}

void ff7_worldmap_play_custom_battle_music(DWORD* unk1, DWORD* unk2, DWORD* battle_id)
{
	ff7_externals.sub_767039(unk1, unk2, battle_id);

	if (*unk1)
	{
		next_battle_scene_id = *battle_id;
		next_music_is_battle = true;

		// Now we know the battle scene ID, so we can try to customize battle music in Worldmap too
		stop_music();
		// Play music
		ff7_music_sound_operation_fix(ff7_last_akao_call_type, ff7_last_music_id, 4, 0, 0, 0);

		next_battle_scene_id = 0;
		next_music_is_battle = false;
	}
}

uint32_t ff7_field_music_id_to_midi_id(int16_t field_music_id)
{
	if (trace_all || trace_music) ffnx_trace("%s: field_music_id=%d\n", __func__, field_music_id);

	ff7_next_field_music_relative_id = field_music_id;

	return ff7_externals.field_music_id_to_midi_id(field_music_id);
}

uint32_t ff8_remember_playing_time()
{
	if (trace_all || trace_music) ffnx_trace("%s\n", __func__);

	nxAudioEngine.pauseMusic(0, 0.0, true);
	// We never remember dualmusic, but battle and cardgame do not pause or stop channel 1 before playing the next music
	stop_music_for_channel(1);

	return 0;
}

uint32_t* ff8_load_music(uint32_t channel, uint32_t music_id, uint32_t data)
{
	if (trace_all || trace_music) ffnx_trace("%s: channel=%u, music_id=%u, data=%u\n", __func__, channel, music_id, data);

	// Do not apply volume changes for this channel between load_music and change_music/dual_music/replay_music instructions
	hold_volume_for_channel[channel] = true;

	return ((uint32_t * (*)(uint32_t, uint32_t, uint32_t))ff8_externals.music_load)(channel, music_id, data);
}

uint32_t ff8_play_midi(uint32_t music_id, int32_t volume, uint32_t unused1, uint32_t unused2)
{
	const int channel = next_music_channel;
	const uint32_t current_music_id = nxAudioEngine.currentMusicId(channel);

	if (current_music_id != music_id)
	{
		if (is_gameover(music_id)) music_flush();

		const char* music_name = ff8_midi_name(music_id);

		if (nullptr == music_name) {
			ffnx_error("%s: Cannot get music name from music_id %d\n", __func__, music_id);
			return 0; // Error
		}

		if (trace_all || trace_music) ffnx_trace("%s: music_id=%u, name=%s, channel=%d, volume=%d\n", __func__, music_id, music_name, channel, volume);

		SoLoud::time offset = 0;
		bool noIntro = false, backup_channel_1_after = false;

		if (remember_musics[current_music_id]) {
			remember_musics[current_music_id] = false;
			if (trace_all || trace_music) ffnx_trace("%s: remember music time for music_id %d\n", __func__, current_music_id);

			nxAudioEngine.pauseMusic(0, 0.2, true);
		}

		if (next_music_is_skipped_with_saved_offset) {
			nxAudioEngine.prioritizeMusicRestore(music_id);
		}
		else if (next_music_is_skipped) {
			noIntro = true;
		}

		NxAudioEngine::MusicOptions options = NxAudioEngine::MusicOptions();
		options.fadetime = next_music_fade_time;
		options.noIntro = noIntro;
		if (volume >= 0 && volume <= 127) {
			options.targetVolume = volume / 127.0f;
		}
		play_music_ff8(music_name, music_id, channel, options);

		if (backup_channel_1_after) {
			// Backup channel 1 music state
			nxAudioEngine.pauseMusic(1, 1.0, true);
		}

		if (music_id == 93) { // The Extreme
			nxAudioEngine.pauseMusic(0);
		}
	}
	else if (trace_all || trace_music) {
		ffnx_trace("%s: is already playing music_id=%u, channel=%d, volume=%d\n", __func__, music_id, channel, volume);
	}

	return 1; // Success
}

uint32_t ff8_play_wav(uint32_t zero, char* filename, uint32_t volume)
{
	int channel = next_music_channel;
	uint32_t music_id = ff8_externals.current_music_ids[channel];
	bool the_extreme_intro = false;

	if (strstr(filename, "field2.fs") != nullptr) { // The Extreme Intro
		channel = 1;
		music_id = 111; // Arbitrary
		the_extreme_intro = true;
	}

	if (the_extreme_intro || nxAudioEngine.currentMusicId(channel) != music_id)
	{
		if (is_gameover(music_id)) music_flush();

		char* music_name = nullptr;

		if (the_extreme_intro) {
			music_name = "lasbossintro";
		}
		else {
			music_name = strrchr(filename, '\\');
			if (music_name == nullptr) {
				music_name = filename;
			} else {
				music_name += 1;
			}
			music_name = ff8_format_midi_name(music_name);
		}

		if (nullptr == music_name) {
			ffnx_error("%s: Cannot get music name from filename %s\n", __func__, filename);
			return 0; // Error
		}

		if (trace_all || trace_music) ffnx_trace("%s: music_id=%u, name=%s, channel=%d, volume=%u\n", __func__, music_id, music_name, channel, volume);

		NxAudioEngine::MusicOptions options = NxAudioEngine::MusicOptions();
		options.fadetime = next_music_fade_time;
		if (volume >= 0 && volume < 127) {
			options.targetVolume = volume / 127.0f;
		}
		play_music_ff8(music_name, music_id, channel, options, filename);
	}
	else if (trace_all || trace_music) {
		ffnx_trace("%s: is already playing music_id=%u, filename=%s, channel=%d, volume=%d\n", __func__, music_id, filename, channel, volume);
	}

	return 1; // Success
}

uint32_t ff8_opcode_dualmusic_play_music(char* midi_data, uint32_t volume)
{
	uint32_t channel = 1;

	if (trace_all || trace_music) ffnx_trace("%s: channel=%u, volume=%u\n", __func__, channel, volume);

	next_music_channel = channel;
	channel = ff8_externals.sd_music_play(channel, midi_data, volume);
	next_music_channel = 0;
	return channel;
}

uint32_t ff8_cross_fade_midi(char* midi_data, uint32_t steps, uint32_t volume)
{
	uint32_t channel = 0;
	double time = steps / 50.0;

	if (trace_all || trace_music) ffnx_trace("%s: steps=%u (time=%fs), volume=%u\n", __func__, steps, time, volume);

	next_music_fade_time = time;
	channel = ff8_externals.sd_music_play(channel, midi_data, volume);
	next_music_fade_time = 0;
	return channel;
}

uint32_t ff8_play_music_to_channel_0(char* midi_data)
{
	uint32_t channel = 0;

	hold_volume_for_channel[channel] = false;

	if (trace_all || trace_music) ffnx_trace("%s\n", __func__);
	// Force known volume value
	return ff8_externals.sd_music_play(channel, midi_data, uint32_t(nxAudioEngine.getMusicVolume(channel) * 127));
}

uint32_t ff8_play_midi_at(char* midi_data, uint32_t offset)
{
	// We don't know what offset means in seconds
	if (trace_all || trace_music) ffnx_trace("%s: play midi at %d\n", __func__, offset);
	next_music_is_skipped = true;
	uint32_t channel = ff8_play_music_to_channel_0(midi_data);
	next_music_is_skipped = false;
	return channel;
}

uint32_t ff8_opcode_musicskip_play_midi_at(char* midi_data, uint32_t offset)
{
	uint32_t channel = 0;

	hold_volume_for_channel[channel] = false;

	if (trace_all || trace_music) ffnx_trace("%s: music skip, play midi at %d\n", __func__, offset);
	next_music_is_skipped_with_saved_offset = (offset & 0xFF) == 0xFF; // Special offset returned by ff8_opcode_getmusicoffset()
	channel = ff8_play_midi_at(midi_data, offset);
	next_music_is_skipped_with_saved_offset = false;
	return channel;
}

uint32_t ff8_opcode_getmusicoffset()
{
	const uint32_t channel = 0;
	const uint32_t musicId = nxAudioEngine.currentMusicId(channel);

	if (trace_all || trace_music) ffnx_trace("%s: save music %d\n", __func__, musicId);

	remember_musics[musicId] = true;

	return 0xFFFFFFFF; // Return a special offset to recognize it in ff8_opcode_musicskip_play_midi_at()
}

uint32_t ff8_field_pause_music(uint32_t a1)
{
	if (trace_all || trace_music) ffnx_trace("%s: a1=%d\n", __func__, a1);

	((uint32_t(*)(uint32_t))ff8_externals.pause_music_and_sfx)(0);

	return ff8_externals.check_game_is_paused(a1);
}

uint32_t ff8_field_restart_music(uint32_t a1)
{
	uint32_t ret = ff8_externals.check_game_is_paused(a1);

	if (ret == 0) { // Unpause
		if (trace_all || trace_music) ffnx_trace("%s: a1=%d\n", __func__, a1);

		((uint32_t(*)(uint32_t))ff8_externals.restart_music_and_sfx)(0);
	}

	return ret;
}

uint32_t set_music_volume_for_channel(int32_t channel, uint32_t volume)
{
	if (trace_all || trace_music) ffnx_trace("%s: channel=%d, volume=%d, hold=%d\n", __func__, channel, volume, hold_volume_for_channel[channel]);

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

	if (trace_all || trace_music) ffnx_trace("%s: channel=%d, volume=%u, steps=%u (=> time=%fs), hold=%d\n", __func__, channel, volume, steps, time, hold_volume_for_channel[channel]);

	if (hold_volume_for_channel[channel] || channel < 0 || channel > 1) {
		return 1;
	}

	if (volume > 127) volume = 127;

	nxAudioEngine.setMusicVolume(volume / 127.0f, channel, time);

	return 1;
}

uint32_t ff8_volume_fade(uint32_t channel, uint32_t steps, uint32_t volume1, uint32_t volume2)
{
	if (trace_all || trace_music) ffnx_trace("%s: channel=%d, volume1=%u, volume2=%u, steps=%u\n", __func__, channel, volume1, volume2, steps);

	set_music_volume_for_channel(channel, volume1);
	ff8_volume_trans(channel, steps, volume2);

	return 1;
}

uint32_t ff8_volume_sync(int a1)
{
	if (trace_all || trace_music) ffnx_trace("%s\n", __func__);

	if (nxAudioEngine.isMusicVolumeFadeFinished()) {
		return 2; // Continue
	}

	return 1; // Wait
}

uint32_t ff8_play_music_worldmap(char* midi_data)
{
	if (trace_all || trace_music) ffnx_trace("%s\n", __func__);

	nxAudioEngine.setMusicVolume(0.0f, 0, 0.5); // Fadeout: 500ms
	return ff8_cross_fade_midi(midi_data, 60, 127); // Fadein: ~1s
}

uint32_t ff8_load_cdrom()
{
	if (trace_all || trace_music) ffnx_trace("%s\n", __func__);

	if (eyes_on_me_is_playing) {
		return 1;
	}

	char fullpath[MAX_PATH];

	snprintf(fullpath, MAX_PATH, "%s..\\%s.wav", ff8_externals.music_path, eyes_on_me_track);

	NxAudioEngine::MusicOptions options = NxAudioEngine::MusicOptions();
	options.targetVolume = 1.0f;
	eyes_on_me_is_playing = play_music_ff8(eyes_on_me_track, 111, 0, options, fullpath);

	if (eyes_on_me_is_playing) {
		return 1;
	}

	// Fallback
	return ff8_externals.load_cdrom();
}

uint32_t ff8_play_cdrom(uint32_t trackStart, uint32_t trackEnd, uint32_t unknown)
{
	if (trace_all || trace_music) ffnx_trace("%s\n", __func__);

	if (eyes_on_me_is_playing) {
		return 1;
	}

	return ff8_externals.play_cdrom(trackStart, trackEnd, unknown);
}

uint32_t ff8_stop_cdrom()
{
	if (trace_all || trace_music) ffnx_trace("%s\n", __func__);

	if (eyes_on_me_is_playing) {
		nxAudioEngine.stopMusic(0);
		eyes_on_me_is_playing = false;
		return 1;
	}

	return ff8_externals.stop_cdrom();
}

std::vector<std::string> musics;

uint32_t ff8_opcode_choicemusic(uint32_t unused, uint32_t instruments)
{
	if (trace_all || trace_music) ffnx_trace("%s: Clear musics\n", __func__);

	musics.clear();

	return ((uint32_t(*)(uint32_t, uint32_t))ff8_externals.choice_music)(unused, instruments);
}

uint32_t ff8_load_midi_segment(void* directsound, const char* filename)
{
	const char* midi_name = ff8_format_midi_name(filename);

	hold_volume_for_channel[0] = false;

	if (next_music_is_not_multi) {
		next_music_is_not_multi = false;
		NxAudioEngine::MusicOptions options = NxAudioEngine::MusicOptions();
		options.suppressOpeningSilence = true;
		play_music_ff8(midi_name, 43, 0, options);
		return 1; // Success
	}

	if (trace_all || trace_music) ffnx_trace("%s: load music %s (%s)\n", __func__, midi_name, filename);

	musics.push_back(midi_name);

	return 1; // Success
}

uint32_t ff8_play_midi_segments()
{
	const int channel = 0;

	if (trace_all || trace_music) ffnx_trace("%s\n", __func__);

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

	if (trace_all || trace_music) ffnx_trace("%s %d %d\n", __func__, channel, volume);

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
	else
	{
		// Adding pause/resume music (and sfx) when pausing the game in field
		replace_call(ff8_externals.sub_4767B0 + (JP_VERSION ? 0x9BF : 0x9CC), ff8_field_pause_music);
		replace_call(ff8_externals.field_main_loop + 0x16C, ff8_field_restart_music);
	}

	if (use_external_music)
	{
		if (ff8)
		{
			/* Play & Stop */
			replace_call(ff8_externals.opcode_musicload + 0x8C, ff8_load_music);
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
			replace_function(uint32_t(ff8_externals.dmusicperf_set_volume_sub_46C6F0), set_music_volume);
			// Fix intro credits volume fadeout time
			replace_call(ff8_externals.load_credits_image + 0x5DF, ff8_set_music_volume_intro_credits);
			replace_call(ff8_externals.load_credits_image + 0x5C2, noop_a1);
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
			/* Eyes On Me */
			replace_call(ff8_externals.load_cdrom_call, ff8_load_cdrom);
			replace_call(ff8_externals.play_cdrom_call, ff8_play_cdrom);
			replace_call(ff8_externals.stop_cdrom_field_call, ff8_stop_cdrom);
			replace_call(ff8_externals.stop_cdrom_cleanup_call, ff8_stop_cdrom);
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

			// Allow custom worldmap battle musics
			replace_call_function(ff7_externals.world_mode_loop_sub_74DB8C + 0x613, ff7_worldmap_play_custom_battle_music);
			// Force channel detection (1) for battle music
			replace_call(ff7_externals.play_battle_music_call, ff7_battle_music);
			replace_call(ff7_externals.play_battle_music_win_call, ff7_battle_music_fanfare);
			replace_call(ff7_externals.wm_play_music_call, ff7_worldmap_music_change);
			// Introduce a custom hook to run music AKAO code
			patch_code_byte(ff7_externals.world_loop_74BE49 + 0x121, 0x90);
			replace_call_function(ff7_externals.world_loop_74BE49 + 0x122, ff7_worldmap_on_loop);

			replace_call(ff7_externals.field_music_id_to_midi_id_call1, ff7_field_music_id_to_midi_id);
			replace_call(ff7_externals.field_music_id_to_midi_id_call2, ff7_field_music_id_to_midi_id);
			replace_call(ff7_externals.field_music_id_to_midi_id_call3, ff7_field_music_id_to_midi_id);

			if (ff7_external_opening_music) {
				// Disable opening music part 2 (ob)
				replace_call(ff7_externals.opening_movie_play_midi_call, noop_a1);
				// Reenable opening music part 1 (oa)
				patch_code_byte(ff7_externals.field_music_helper + 0xD1 + 2, 0x00);
			}
		}
	}
}
