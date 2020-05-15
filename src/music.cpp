#include <windows.h>

#include "music.h"
#include "patch.h"
#include "directmusic.h"
#include "winamp/music.h"

void music_init()
{
	// Add Global Focus flag to DirectSound Secondary Buffers
	patch_code_byte(common_externals.directsound_buffer_flags_1 + 0x4, 0x80); // DSBCAPS_GLOBALFOCUS & 0x0000FF00
	patch_code_dword(ff7_externals.music_lock_clear_fix + 2, 0xCC195C);

	if (use_external_music)
	{
		if (ff8) {
			replace_function(common_externals.play_midi, ff8_play_midi);
			replace_function(common_externals.pause_midi, pause_midi);
			replace_function(common_externals.restart_midi, restart_midi);
			replace_function(common_externals.stop_midi, ff8_stop_midi);
			replace_function(common_externals.midi_status, midi_status);
			replace_function(common_externals.set_midi_volume, ff8_set_direct_volume);
			replace_function(common_externals.remember_midi_playing_time, remember_playing_time);
		}
		else {
			replace_function(common_externals.midi_init, midi_init);
			replace_function(common_externals.use_midi, ff7_use_midi);
			replace_function(common_externals.play_midi, ff7_play_midi);
			replace_function(common_externals.cross_fade_midi, cross_fade_midi);
			replace_function(common_externals.pause_midi, pause_midi);
			replace_function(common_externals.restart_midi, restart_midi);
			replace_function(common_externals.stop_midi, stop_midi);
			replace_function(common_externals.midi_status, midi_status);
			replace_function(common_externals.set_master_midi_volume, set_master_midi_volume);
			replace_function(common_externals.set_midi_volume, set_midi_volume);
			replace_function(common_externals.set_midi_volume_trans, set_midi_volume_trans);
			replace_function(common_externals.set_midi_tempo, set_midi_tempo);
			replace_function(common_externals.directsound_release, ff7_directsound_release);
		}
		winamp_music_init();
	}
}

uint midi_init(uint unknown)
{
	// without this there will be no volume control for music in the config menu
	*ff7_externals.midi_volume_control = true;

	// enable fade function
	*ff7_externals.midi_initialized = true;

	return true;
}

char ff8_midi[32];

char* ff8_midi_name(uint midi)
{
	// midi_name format: {num}{type}-{name}.sgt or {name}.sgt or _Missing.sgt
	char* midi_name = common_externals.get_midi_name(midi),
		* truncated_name;

	truncated_name = strchr(midi_name, '-');

	if (nullptr != truncated_name) {
		truncated_name += 1; // Remove "-"
	}
	else {
		truncated_name = midi_name;
	}

	char* max_midi_name = strchr(truncated_name, '.');

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

uint ff8_play_midi(uint midi, uint volume, uint u1, uint u2)
{
	info("FF8 midi play %i %i %i %i\n", midi, volume, u1, u2);

	char* midi_name = ff8_midi_name(midi);

	if (nullptr == midi_name) {
		return 0; // Error
	}

	winamp_set_music_volume(volume);
	winamp_play_music(midi_name, midi);

	return 1; // Success
}

uint ff7_use_midi(uint midi)
{
	char* name = common_externals.get_midi_name(midi);

	if (winamp_can_play(name)) {
		return 1;
	}

	return strcmp(name, "HEART") != 0 && strcmp(name, "SATO") != 0 && strcmp(name, "SENSUI") != 0 && strcmp(name, "WIND") != 0;
}

void ff7_play_midi(uint midi)
{
	winamp_play_music(common_externals.get_midi_name(midi), midi);
}

void cross_fade_midi(uint midi, uint time)
{
	winamp_cross_fade_music(common_externals.get_midi_name(midi), midi, time);
}

void pause_midi()
{
	winamp_pause_music();
}

void restart_midi()
{
	winamp_resume_music();
}

void stop_midi()
{
	winamp_stop_music();
}

uint ff8_stop_midi()
{
	if (trace_all || trace_music) info("FF8 stop midi\n");

	// Stop game midi for horizon concert instruments
	if (nullptr != *ff8_externals.directmusic_performance) {
		(*ff8_externals.directmusic_performance)->Stop(nullptr, nullptr, 0, DMUS_SEGF_DEFAULT);
	}

	stop_midi();

	return 0;
}

uint midi_status()
{
	return winamp_music_status();
}

uint ff8_set_direct_volume(int volume)
{
	if (trace_all || trace_music) info("FF8 set direct volume %i\n", volume);

	// Set game volume for horizon concert instruments
	if (nullptr != *ff8_externals.directmusic_performance) {
		(*ff8_externals.directmusic_performance)->SetGlobalParam(
			*(ff8_externals.GUID_PerfMasterVolume), &volume, sizeof(volume)
		);
	}

	if (volume == DSBVOLUME_MIN) {
		volume = 0;
	}
	else {
		volume = (pow(10, (volume + 2000) / 2000.0f) / 10.0f) * 255.0f;
	}
	
	winamp_set_direct_volume(volume);
	return 1; // Success
}

void set_master_midi_volume(uint volume)
{
	winamp_set_master_music_volume(volume);
}

void set_midi_volume(uint volume)
{
	winamp_set_music_volume(volume);
}

void set_midi_volume_trans(uint volume, uint step)
{
	winamp_set_music_volume_trans(volume, step);
}

void set_midi_tempo(unsigned char tempo)
{
	winamp_set_music_tempo(tempo);
}

uint ff7_directsound_release()
{
	if (nullptr == *common_externals.directsound) {
		return 0;
	}

	music_cleanup();
	(*common_externals.directsound)->Release();
	*common_externals.directsound = nullptr;

	return 0;
}

void music_cleanup()
{
	winamp_music_cleanup();
}

uint remember_playing_time()
{
	winamp_remember_playing_time();
	return 0;
}

bool is_wm_theme(char* midi)
{
	if (nullptr == midi)
	{
		return false;
	}

	if (ff8)
	{
		return strcmp(midi, "field") == 0;
	}
	
	return strcmp(midi, "TA") == 0 || strcmp(midi, "TB") == 0 || strcmp(midi, "KITA") == 0;
}

bool needs_resume(uint old_mode, uint new_mode, char* old_midi, char* new_midi)
{
	/*
	 * BATTLE -> FIELD or WM
	 * FIELD  -> WM
	 */
	return ((new_mode == MODE_WORLDMAP || new_mode == MODE_AFTER_BATTLE) && !is_wm_theme(old_midi) && is_wm_theme(new_midi))
		|| ((old_mode == MODE_BATTLE || old_mode == MODE_SWIRL) && (new_mode == MODE_FIELD || new_mode == MODE_AFTER_BATTLE))
		|| new_mode == MODE_CARDGAME;
}
