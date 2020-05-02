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

AbstractOutPlugin* out = nullptr;
AbstractInPlugin* in = nullptr;

CRITICAL_SECTION winamp_mutex;
HANDLE winampRenderHandle = nullptr;
unsigned winampRenderThreadID;
bool winamp_stop_thread = false;

uint winamp_current_id = 0;
char* winamp_current_midi = nullptr;
bool winamp_song_ended = true; // Song reaches end (but it can still loop)
bool winamp_song_paused = false;

int winamp_trans_step = 0;
int winamp_trans_counter = 0;
int winamp_trans_volume = 0;

int winamp_crossfade_time = 0;
uint winamp_crossfade_id = 0;
char* winamp_crossfade_midi;

int winamp_master_volume = 100;
int winamp_song_volume = 127;

uint winamp_paused_midi_id = 0;
int winamp_paused_midi_ms = 0;
uint winamp_current_mode = uint(-1);

void winamp_apply_volume()
{
	if (out)
	{
		int volume = (((winamp_song_volume * 100) / 127) * winamp_master_volume) / 100;
		out->setVolume(volume * 255 / 100);
	}
}

void winamp_load_song(char* midi, uint id)
{
	char tmp[512];

	if (!in)
	{
		return;
	}
	
	uint winamp_previous_paused_midi_id = winamp_paused_midi_id;
	int winamp_previous_paused_midi_ms = winamp_paused_midi_ms;
	uint winamp_previous_mode = winamp_current_mode;
	char* winamp_previous_midi = winamp_current_midi;
	uint mode = getmode_cached()->driver_mode;

	if (winamp_current_id && ff7_needs_resume(mode, winamp_previous_mode, midi, winamp_previous_midi)) {
		winamp_paused_midi_id = winamp_current_id;
		winamp_paused_midi_ms = in->getOutputTime() % in->getLength();

		info("Saved midi time ms: %i\n", winamp_paused_midi_ms);
	}
	
	in->stop();

	if (!id)
	{
		winamp_song_ended = true;
		winamp_current_id = 0;
		return;
	}
	
	sprintf(tmp, "%s/%s/%s.%s", basedir, external_music_path, midi, external_music_ext);

	winamp_song_ended = false;
	winamp_current_id = id;
	winamp_current_midi = midi;
	winamp_current_mode = mode;

	winamp_apply_volume();

	bool seek = winamp_previous_paused_midi_id == id && ff7_needs_resume(winamp_previous_mode, mode, winamp_previous_midi, midi);

	if (seek) {
		in->pause();
	}

	int err = in->play(tmp);

	if (-1 == err) {
		error("couldn't play music (file not found)\n");
	} else if (0 != err) {
		error("couldn't play music (%i)\n", err);
	}

	if (seek) {
		info("Resume midi time ms: %i\n", winamp_previous_paused_midi_ms);
		in->setOutputTime(winamp_previous_paused_midi_ms);
		winamp_paused_midi_id = 0;
		winamp_paused_midi_ms = 0;
		in->unPause();
	}
}

ULONG(__stdcall* real_dsound_release)(IDirectSound*);

ULONG __stdcall dsound_release_hook(IDirectSound* me)
{
	trace("directsound release\n");

	EnterCriticalSection(&winamp_mutex);

	return real_dsound_release(me);
}

unsigned __stdcall winamp_render_thread(void* parameter)
{
	const int flush_debug_at = 20;
	// For safety we detect if output time does not change for a given amount of time
	const int stop_silence_ms = 1000;
	const DWORD sleep_ms = 50;
	const int stop_silence_loop_count = stop_silence_ms / sleep_ms;
	int silence_detected = 0;
	int cur = 0, previous_output_time = -1;

	while (!winamp_stop_thread)
	{
		Sleep(sleep_ms);

		EnterCriticalSection(&winamp_mutex);

		if (*common_externals.directsound)
		{
			bool start_next_song = false;

			if (winamp_trans_counter > 0)
			{
				if (winamp_current_id > 0)
				{
					winamp_song_volume += winamp_trans_step;

					winamp_apply_volume();

					winamp_trans_counter--;

					if (!winamp_trans_counter)
					{
						start_next_song = true;
					}
				}
				else
				{
					winamp_trans_counter = 0;

					start_next_song = true;
				}
			}
			
			if (start_next_song)
			{
				winamp_song_volume = winamp_trans_volume;

				winamp_apply_volume();

				if (winamp_crossfade_midi)
				{
					if (!winamp_stop_thread)
					{
						winamp_load_song(winamp_crossfade_midi, winamp_crossfade_id);
					}

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

		if (in && winamp_current_id > 0)
		{
			int output_time = in->getOutputTime();

			if (!winamp_song_ended && output_time >= in->getLength())
			{
				winamp_song_ended = true;

				trace("song ended at %i ms\n", output_time);
			}

			if (!winamp_song_paused && previous_output_time >= 0 && output_time == previous_output_time)
			{
				silence_detected += 1;
			}

			previous_output_time = output_time;

			if (silence_detected > stop_silence_loop_count)
			{
				winamp_song_ended = true;
				winamp_current_id = 0;
				silence_detected = 0;

				trace("silence detected at %i ms\n", output_time);
			}
		}
		else {
			silence_detected = 0;
		}

		cur += 1;

		if (cur % flush_debug_at == 0 && in)
		{
			trace("Output time %i, Length %i, Song Ended %i, Current Id %i\n", in->getOutputTime(), in->getLength(), winamp_song_ended, winamp_current_id);
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

	// Force volume for MM (out_wave fix)
	for (int i = 0; i < waveInGetNumDevs(); ++i) {
		waveOutSetVolume(HWAVEOUT(i), 0xFFFFFFFF);
	}

	char* out_type = "FFNx out implementation",
		* in_type = "FFNx in implementation";
	
	if (nullptr != winamp_out_plugin) {
		WinampOutPlugin* winamp_out = new WinampOutPlugin();
		if (winamp_out->open(winamp_out_plugin)) {
			out = winamp_out;
			out_type = winamp_out_plugin;
		}
		else {
			error("couldn't load %s, please verify 'winamp_out_plugin' or comment it\n", winamp_out_plugin);
			delete winamp_out;
		}
	}

	if (nullptr == out) {
		out = new CustomOutPlugin();
	}

	if (nullptr != winamp_in_plugin) {
		WinampInPlugin* winamp_in = new WinampInPlugin(out);
		if (winamp_in->open(winamp_in_plugin)) {
			in = winamp_in;
			in_type = winamp_in_plugin;
		}
		else {
			error("couldn't load %s, please verify 'winamp_in_plugin' or comment it\n", winamp_in_plugin);
			delete winamp_in;
		}
	}
	
	if (nullptr == in) {
		in = new VgmstreamInPlugin(out);
	}

	InitializeCriticalSection(&winamp_mutex);

	winampRenderHandle = (HANDLE)_beginthreadex(nullptr, 0, &winamp_render_thread, nullptr, 0, &winampRenderThreadID);

	info("Winamp music plugin loaded using %s and %s\n", in_type, out_type);
}

// start playing some music, <midi> is the name of the MIDI file without the .mid extension
void winamp_play_music(char *midi, uint id)
{
	trace("[%s] play music: %s:%i (current=%i, ended=%i)\n", getmode_cached()->name, midi, id, winamp_current_id, winamp_song_ended);

	EnterCriticalSection(&winamp_mutex);

	if (id != winamp_current_id)
	{
		winamp_load_song(midi, id);
	}

	LeaveCriticalSection(&winamp_mutex);
}

void winamp_stop_music()
{
	trace("stop music\n");

	EnterCriticalSection(&winamp_mutex);
	
	if (in) {
		in->stop();
	}

	winamp_song_ended = true;
	winamp_current_id = 0;

	LeaveCriticalSection(&winamp_mutex);
}

// cross fade to a new song
void winamp_cross_fade_music(char *midi, uint id, int time)
{
	int fade_time = time * 2;

	trace("[%s] cross fade music: %s:%i (%i)\n", getmode_cached()->name, midi, id, time);

	EnterCriticalSection(&winamp_mutex);

	if (id != winamp_current_id)
	{
		if (winamp_current_id != 0 && fade_time)
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
	winamp_song_paused = true;

	LeaveCriticalSection(&winamp_mutex);
}

void winamp_resume_music()
{
	EnterCriticalSection(&winamp_mutex);

	if (in) {
		in->unPause();
	}
	winamp_song_paused = false;

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

	if (out) {
		out->setTempo(tempo);
	}

	LeaveCriticalSection(&winamp_mutex);
}

void winamp_music_cleanup()
{
	winamp_stop_thread = true;
	
	winamp_stop_music();

	EnterCriticalSection(&winamp_mutex);

	if (in) {
		delete in;
		in = nullptr;
	}

	if (out) {
		delete out;
		out = nullptr;
	}

	LeaveCriticalSection(&winamp_mutex);

	WaitForSingleObject(winampRenderHandle, INFINITE);

	CloseHandle(winampRenderHandle);

	DeleteCriticalSection(&winamp_mutex);
}