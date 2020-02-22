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
#include "vgmstream_music/music.h"

void music_init()
{
	if (use_external_music && !ff8)
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

		vgm_music_init();
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
	vgm_play_music(common_externals.get_midi_name(midi), midi);
}

void cross_fade_midi(uint midi, uint time)
{
	vgm_cross_fade_music(common_externals.get_midi_name(midi), midi, time);
}

void pause_midi()
{
	vgm_pause_music();
}

void restart_midi()
{
	vgm_resume_music();
}

void stop_midi()
{
	vgm_stop_music();
}

uint midi_status()
{
	return vgm_music_status();
}

void set_master_midi_volume(uint volume)
{
	vgm_set_master_music_volume(volume);
}

void set_midi_volume(uint volume)
{
	vgm_set_music_volume(volume);
}

void set_midi_volume_trans(uint volume, uint step)
{
	vgm_set_music_volume_trans(volume, step);
}

void set_midi_tempo(unsigned char tempo)
{
	vgm_set_music_tempo(tempo);
}

void music_cleanup()
{
	vgm_music_cleanup();
}