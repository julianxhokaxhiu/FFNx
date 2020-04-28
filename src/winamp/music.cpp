/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU Lesser General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU Lesser General Public License for more details.
 *
 *	You should have received a copy of the GNU Lesser General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "music.h"
#include "plugin.h"
#include "cfg.h"

uint current_id = 0;
uint song_ended = true;

CustomOutPlugin* out = NULL;
WinampInPlugin* in = NULL;

int trans_step;
int trans_counter;
int trans_volume;

int master_volume;
int song_volume;

void winamp_apply_volume()
{
	trace("apply_volume %i\n", song_volume);

	if (in)
	{
		int volume = (((song_volume * 100) / 127) * master_volume) / 100;
		in->setVolume(volume * 255 / 100);
	}
}

void winamp_music_init()
{
	if (NULL != out) {
		delete out;
	}
	out = new CustomOutPlugin();
	in = new WinampInPlugin(out->getModule());

	if (in->open(in_plugin_dll_name)) {
		info("Winamp music plugin loaded\n");
	}
	else {
		error("couldn't load in_psf.dll\n");
		delete in;
		in = NULL;
		delete out;
		out = NULL;
	}
}

// start playing some music, <midi> is the name of the MIDI file without the .mid extension
void winamp_play_music(char *midi, uint id)
{
	trace("play music: %s\n", midi);

	if (in && (id != current_id || song_ended))
	{
		char tmp[MAX_PATH];
		sprintf(tmp, "%s/%s/%s.%s", basedir, external_music_path, midi, external_music_ext);

		song_ended = false;
		current_id = id;

		trace("play music 2: %s\n", tmp);

		winamp_apply_volume();

		trace("play music 3: %s\n", midi);

		int err = in->play(tmp);

		if (0 != err) {
			error("couldn't play music\n", err);
		}
	}
}

void winamp_stop_music()
{
	song_ended = true;

	if (in) {
		in->stop();
	}
}

// cross fade to a new song
void winamp_cross_fade_music(char *midi, uint id, uint time)
{
	int fade_time = time * 2;

	trace("cross fade music: %s (%i)\n", midi, time);

	if (id != current_id || song_ended)
	{
		winamp_stop_music();
		winamp_play_music(midi, id);

		// TODO: real fade
		/* if (!song_ended && fade_time)
		{
			trans_volume = 0;
			trans_counter = fade_time;
			trans_step = (trans_volume - song_volume) / fade_time;
		}
		else
		{
			trans_volume = 0;
			trans_counter = 1;
			trans_step = 0;
		}

		crossfade_time = fade_time;
		crossfade_id = id;
		crossfade_midi = midi; */
	}
}

void winamp_pause_music()
{
	if (in) {
		in->pause();
	}
}

void winamp_resume_music()
{
	if (in) {
		in->unPause();
	}
}

// return true if music is playing, false if it isn't
// it's important for some field scripts that this function returns true atleast once when a song has been requested
// 
// even if there's nothing to play because of errors/missing files you cannot return false every time
bool winamp_music_status()
{
	uint last_status = 0;
	uint status;

	status = !song_ended;

	if (!in)
	{
		last_status = !last_status;
		return !last_status;
	}

	last_status = status;
	return status;
}

void winamp_set_master_music_volume(uint volume)
{
	trace("set master volume: %i\n", volume);

	master_volume = volume;

	winamp_apply_volume();
}

void winamp_set_music_volume(uint volume)
{
	trace("set song volume: %i\n", volume);

	song_volume = volume;

	trans_volume = 0;
	trans_counter = 0;
	trans_step = 0;

	winamp_apply_volume();
}

// make a volume transition
void winamp_set_music_volume_trans(uint volume, uint step)
{
	trace("set volume trans: %i (%i)\n", volume, step);

	song_volume = volume;
	// TODO: transition
	winamp_apply_volume();

	/* step /= 4;

	if (step < 2)
	{
		trans_volume = 0;
		trans_counter = 0;
		trans_step = 0;
		song_volume = volume;
		winamp_apply_volume();
	}
	else
	{
		trans_volume = volume;
		trans_counter = step;
		trans_step = (trans_volume - song_volume) / step;
	} */
}

void winamp_set_music_tempo(unsigned char tempo)
{
	trace("set music tempo: %i\n", int(tempo));

	if (in) {
		in->setTempo(tempo);
	}
}

void winamp_music_cleanup()
{
	winamp_stop_music();

	if (in) {
		delete in;
		in = NULL;
	}

	if (out) {
		delete out;
		out = NULL;
	}
}