/* 
 * FFNx - Complete OpenGL replacement of the Direct3D renderer used in 
 * the original ports of Final Fantasy VII and Final Fantasy VIII for the PC.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * music.c - replacements routines for music player
 */

#include <windows.h>

#include "music.h"
#include "types.h"
#include "globals.h"
#include "cfg.h"
#include "log.h"
#include "patch.h"
#include "vgmstream/music.h"
#include "ff7music/music.h"
#include "winamp/music.h"

void music_init()
{
	if (ff8) {
		return;
	}

	if (use_external_music > FFNX_MUSIC_NONE && use_external_music <= FFNX_MUSIC_WINAMP)
	{
		replace_function(common_externals.midi_init, midi_init);
		replace_function(common_externals.play_midi, play_midi);
		replace_function(common_externals.cross_fade_midi, cross_fade_midi);
		replace_function(common_externals.pause_midi, pause_midi);
		replace_function(common_externals.restart_midi, restart_midi);
		replace_function(common_externals.stop_midi, stop_midi);
		replace_function(common_externals.midi_status, midi_status);
		replace_function(common_externals.set_master_midi_volume, set_master_midi_volume);
		replace_function(common_externals.set_midi_volume, set_midi_volume);
		replace_function(common_externals.set_midi_volume_trans, set_midi_volume_trans);
		replace_function(common_externals.set_midi_tempo, set_midi_tempo);

		switch (use_external_music)
		{
		case FFNX_MUSIC_VGMSTREAM:
			vgm_music_init();
			break;
		case FFNX_MUSIC_FF7MUSIC:
			break;
		case FFNX_MUSIC_WINAMP:
			winamp_music_init();
			break;
		}
	}
	else {
		error("Unknown use_external_music value\n");
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

void play_midi(uint midi)
{
	switch (use_external_music)
	{
	case FFNX_MUSIC_VGMSTREAM:
		vgm_play_music(common_externals.get_midi_name(midi), midi);
		break;
	case FFNX_MUSIC_FF7MUSIC:
		ff7music_play_music(common_externals.get_midi_name(midi), midi);
		break;
	case FFNX_MUSIC_WINAMP:
		winamp_play_music(common_externals.get_midi_name(midi), midi);
		break;
	}
}

void cross_fade_midi(uint midi, uint time)
{
	switch (use_external_music)
	{
	case FFNX_MUSIC_VGMSTREAM:
		vgm_cross_fade_music(common_externals.get_midi_name(midi), midi, time);
		break;
	case FFNX_MUSIC_FF7MUSIC:
		ff7music_cross_fade_music(common_externals.get_midi_name(midi), midi, time);
		break;
	case FFNX_MUSIC_WINAMP:
		winamp_cross_fade_music(common_externals.get_midi_name(midi), midi, time);
		break;
	}
}

void pause_midi()
{
	switch (use_external_music)
	{
	case FFNX_MUSIC_VGMSTREAM:
		vgm_pause_music();
		break;
	case FFNX_MUSIC_FF7MUSIC:
		ff7music_pause_music();
		break;
	case FFNX_MUSIC_WINAMP:
		winamp_pause_music();
		break;
	}
}

void restart_midi()
{
	switch (use_external_music)
	{
	case FFNX_MUSIC_VGMSTREAM:
		vgm_resume_music();
		break;
	case FFNX_MUSIC_FF7MUSIC:
		ff7music_resume_music();
		break;
	case FFNX_MUSIC_WINAMP:
		winamp_resume_music();
		break;
	}
}

void stop_midi()
{
	switch (use_external_music)
	{
	case FFNX_MUSIC_VGMSTREAM:
		vgm_stop_music();
		break;
	case FFNX_MUSIC_FF7MUSIC:
		ff7music_stop_music();
		break;
	case FFNX_MUSIC_WINAMP:
		winamp_stop_music();
		break;
	}
}

uint midi_status()
{
	switch (use_external_music)
	{
	case FFNX_MUSIC_VGMSTREAM:
		return vgm_music_status();
		break;
	case FFNX_MUSIC_FF7MUSIC:
		return ff7music_music_status();
		break;
	case FFNX_MUSIC_WINAMP:
		return winamp_music_status();
		break;
	}
}

void set_master_midi_volume(uint volume)
{
	switch (use_external_music)
	{
	case FFNX_MUSIC_VGMSTREAM:
		vgm_set_master_music_volume(volume);
		break;
	case FFNX_MUSIC_FF7MUSIC:
		ff7music_set_master_music_volume(volume);
		break;
	case FFNX_MUSIC_WINAMP:
		winamp_set_master_music_volume(volume);
		break;
	}
}

void set_midi_volume(uint volume)
{
	switch (use_external_music)
	{
	case FFNX_MUSIC_VGMSTREAM:
		vgm_set_music_volume(volume);
		break;
	case FFNX_MUSIC_FF7MUSIC:
		ff7music_set_music_volume(volume);
		break;
	case FFNX_MUSIC_WINAMP:
		winamp_set_music_volume(volume);
		break;
	}
}

void set_midi_volume_trans(uint volume, uint step)
{
	switch (use_external_music)
	{
	case FFNX_MUSIC_VGMSTREAM:
		vgm_set_music_volume_trans(volume, step);
		break;
	case FFNX_MUSIC_FF7MUSIC:
		ff7music_set_music_volume_trans(volume, step);
		break;
	case FFNX_MUSIC_WINAMP:
		winamp_set_music_volume_trans(volume, step);
		break;
	}
}

void set_midi_tempo(unsigned char tempo)
{
	switch (use_external_music)
	{
	case FFNX_MUSIC_VGMSTREAM:
		vgm_set_music_tempo(tempo);
		break;
	case FFNX_MUSIC_FF7MUSIC:
		ff7music_set_music_tempo(tempo);
		break;
	case FFNX_MUSIC_WINAMP:
		winamp_set_music_tempo(tempo);
		break;
	}
}

void music_cleanup()
{
	switch (use_external_music)
	{
	case FFNX_MUSIC_VGMSTREAM:
		vgm_music_cleanup();
		break;
	case FFNX_MUSIC_FF7MUSIC:
		ff7music_music_cleanup();
		break;
	case FFNX_MUSIC_WINAMP:
		winamp_music_cleanup();
		break;
	}
}

bool ff7_is_wm_theme(char* midi)
{
	return midi != nullptr && (strcmp(midi, "TA") == 0 || strcmp(midi, "TB") == 0 || strcmp(midi, "KITA") == 0);
}

bool ff7_needs_resume(uint old_mode, uint new_mode, char* old_midi, char* new_midi)
{
	/*
	 * BATTLE -> FIELD or WM
	 * FIELD  -> WM
	 */
	return (new_mode == MODE_WORLDMAP && !ff7_is_wm_theme(old_midi) && ff7_is_wm_theme(new_midi))
		|| (old_mode == MODE_BATTLE || old_mode == MODE_SWIRL) && new_mode == MODE_FIELD;
}
