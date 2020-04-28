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
#include <process.h>

AbstractOutPlugin* out = nullptr;
WinampInPlugin* in = nullptr;

CRITICAL_SECTION winamp_mutex;
HANDLE winampRenderHandle = nullptr;
unsigned winampRenderThreadID;
bool winamp_stop_thread = false;

uint winamp_current_id = 0;
uint winamp_song_ended = true;

int winamp_trans_step = 0;
int winamp_trans_counter = 0;
int winamp_trans_volume = 0;

int winamp_crossfade_time = 0;
uint winamp_crossfade_id = 0;
char* winamp_crossfade_midi;

int winamp_master_volume = 100;
int winamp_song_volume = 127;

void winamp_apply_volume()
{
	if (in)
	{
		int volume = (((winamp_song_volume * 100) / 127) * winamp_master_volume) / 100;
		in->setVolume(volume * 255 / 100);
	}
}

void winamp_load_song(char* midi, uint id)
{
	char tmp[512];

	if (!in)
	{
		return;
	}

	in->stop();

	if (!id)
	{
		winamp_current_id = 0;
		return;
	}
	
	sprintf(tmp, "%s/%s/%s.%s", basedir, external_music_path, midi, external_music_ext);

	winamp_song_ended = false;
	winamp_current_id = id;

	winamp_apply_volume();

	int err = in->play(tmp);

	if (0 != err) {
		error("couldn't play music\n", err);
	}
}

unsigned __stdcall winamp_render_thread(void* parameter)
{
	while (!winamp_stop_thread)
	{
		Sleep(50);

		EnterCriticalSection(&winamp_mutex);

		if (*common_externals.directsound)
		{
			if (winamp_trans_counter > 0)
			{
				winamp_song_volume += winamp_trans_step;

				winamp_apply_volume();

				winamp_trans_counter--;

				if (!winamp_trans_counter)
				{
					winamp_song_volume = winamp_trans_volume;

					winamp_apply_volume();

					if (winamp_crossfade_midi)
					{
						winamp_load_song(winamp_crossfade_midi, winamp_crossfade_id);

						if (winamp_crossfade_time)
						{
							winamp_trans_volume = 127;
							winamp_trans_counter = winamp_crossfade_time;
							winamp_trans_step = (winamp_trans_volume - winamp_song_volume) / winamp_crossfade_time;
						}
						else
						{
							winamp_song_volume = 127;
							winamp_apply_volume();
						}

						winamp_crossfade_time = 0;
						winamp_crossfade_id = 0;
						winamp_crossfade_midi = 0;
					}
				}
			}

			if (out && *common_externals.directsound && !out->isPlaying())
			{
				winamp_song_ended = true;
			}
		}

		LeaveCriticalSection(&winamp_mutex);
	}

	_endthreadex(0);

	return 0;
}

void winamp_music_init()
{
	if (nullptr != out) {
		delete out;
		out = nullptr;
	}

	if (nullptr != winamp_out_plugin) {
		WinampOutPlugin *winamp_out = new WinampOutPlugin();
		if (winamp_out->open(winamp_out_plugin)) {
			out = winamp_out;
		}
		else {
			error("couldn't load %s, please verify 'winamp_out_plugin' or comment it\n", winamp_out_plugin);
		}
	}

	if (nullptr == out) {
		out = new CustomOutPlugin();
	}
	
	in = new WinampInPlugin(out);

	InitializeCriticalSection(&winamp_mutex);

	winampRenderHandle = (HANDLE)_beginthreadex(nullptr, 0, &winamp_render_thread, nullptr, 0, &winampRenderThreadID);

	if (in->open(winamp_in_plugin)) {
		info("Winamp music plugin loaded\n");
	}
	else {
		error("couldn't load %s\n", winamp_in_plugin);
		delete in;
		in = nullptr;
		delete out;
		out = nullptr;
	}
}

// start playing some music, <midi> is the name of the MIDI file without the .mid extension
void winamp_play_music(char *midi, uint id)
{
	trace("play music: %s\n", midi);

	EnterCriticalSection(&winamp_mutex);

	if (id != winamp_current_id || winamp_song_ended)
	{
		winamp_load_song(midi, id);
	}

	LeaveCriticalSection(&winamp_mutex);
}

void winamp_stop_music()
{
	EnterCriticalSection(&winamp_mutex);
	
	if (in) {
		in->stop();
	}

	winamp_song_ended = true;

	LeaveCriticalSection(&winamp_mutex);
}

// cross fade to a new song
void winamp_cross_fade_music(char *midi, uint id, int time)
{
	int fade_time = time * 2;

	trace("cross fade music: %s (%i)\n", midi, time);

	EnterCriticalSection(&winamp_mutex);

	if (id != winamp_current_id || winamp_song_ended)
	{
		if (!winamp_song_ended && fade_time)
		{
			winamp_trans_volume = 0;
			winamp_trans_counter = fade_time;
			winamp_trans_step = (winamp_trans_volume - winamp_song_volume) / fade_time;
		}
		else
		{
			winamp_trans_volume = 0;
			winamp_trans_counter = 1;
			winamp_trans_step = 0;
		}

		winamp_crossfade_time = fade_time;
		winamp_crossfade_id = id;
		winamp_crossfade_midi = midi;
	}

	LeaveCriticalSection(&winamp_mutex);
}

void winamp_pause_music()
{
	EnterCriticalSection(&winamp_mutex);
	
	if (in) {
		in->pause();
	}

	LeaveCriticalSection(&winamp_mutex);
}

void winamp_resume_music()
{
	EnterCriticalSection(&winamp_mutex);

	if (in) {
		in->unPause();
	}

	LeaveCriticalSection(&winamp_mutex);
}

// return true if music is playing, false if it isn't
// it's important for some field scripts that this function returns true atleast once when a song has been requested
// 
// even if there's nothing to play because of errors/missing files you cannot return false every time
bool winamp_music_status()
{
	uint last_status = 0;
	uint status;

	EnterCriticalSection(&winamp_mutex);

	status = !winamp_song_ended;

	if (!in)
	{
		last_status = !last_status;
		return !last_status;
	}

	LeaveCriticalSection(&winamp_mutex);

	last_status = status;
	return status;
}

void winamp_set_master_music_volume(int volume)
{
	trace("set master volume: %i\n", volume);

	EnterCriticalSection(&winamp_mutex);

	winamp_master_volume = volume;

	winamp_apply_volume();

	LeaveCriticalSection(&winamp_mutex);
}

void winamp_set_music_volume(int volume)
{
	trace("set song volume: %i\n", volume);

	EnterCriticalSection(&winamp_mutex);

	winamp_song_volume = volume;

	winamp_trans_volume = 0;
	winamp_trans_counter = 0;
	winamp_trans_step = 0;

	winamp_apply_volume();

	LeaveCriticalSection(&winamp_mutex);
}

// make a volume transition
void winamp_set_music_volume_trans(int volume, int step)
{
	trace("set volume trans: %i (%i)\n", volume, step);

	step /= 4;

	EnterCriticalSection(&winamp_mutex);
	
	if (step < 2)
	{
		winamp_trans_volume = 0;
		winamp_trans_counter = 0;
		winamp_trans_step = 0;
		winamp_song_volume = volume;
		winamp_apply_volume();
	}
	else
	{
		winamp_trans_volume = volume;
		winamp_trans_counter = step;
		winamp_trans_step = (winamp_trans_volume - winamp_song_volume) / step;
	}

	LeaveCriticalSection(&winamp_mutex);
}

void winamp_set_music_tempo(unsigned char tempo)
{
	trace("set music tempo: %i\n", int(tempo));

	EnterCriticalSection(&winamp_mutex);

	if (in) {
		in->setTempo(tempo);
	}

	EnterCriticalSection(&winamp_mutex);
}

void winamp_music_cleanup()
{
	winamp_stop_thread = true;
	
	winamp_stop_music();

	WaitForSingleObject(winampRenderHandle, INFINITE);

	CloseHandle(winampRenderHandle);

	DeleteCriticalSection(&winamp_mutex);

	if (in) {
		delete in;
		in = nullptr;
	}

	if (out) {
		delete out;
		out = nullptr;
	}
}