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

const DWORD thread_sleep_ms = 33;

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

int winamp_trans_steps = 0;
int winamp_trans_counter = 0;
int winamp_trans_target_volume = 0;
int winamp_trans_prev_volume = 0;

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

	if (!in || !out)
	{
		return;
	}
	
	uint winamp_previous_paused_midi_id = winamp_paused_midi_id;
	int winamp_previous_paused_midi_ms = winamp_paused_midi_ms;
	uint winamp_previous_mode = winamp_current_mode;
	char* winamp_previous_midi = winamp_current_midi;
	uint mode = getmode_cached()->driver_mode;

	if (winamp_current_id && needs_resume(mode, winamp_previous_mode, midi, winamp_previous_midi)) {
		winamp_paused_midi_id = winamp_current_id;
		winamp_paused_midi_ms = out->getOutputTime() % in->getLength();

		if (winamp_paused_midi_ms < 0) {
			winamp_paused_midi_ms = 0;
		}

		info("Saved midi time ms: %i\n", winamp_paused_midi_ms);
	}

	if (!id)
	{
		in->stop();
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

	bool seek = winamp_previous_paused_midi_id == id && needs_resume(winamp_previous_mode, mode, winamp_previous_midi, midi);

	if (seek) {
		in->pause();
		out->pause();
	}

	in->stop();
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
		out->unPause();
	}
}

ULONG(__stdcall* real_dsound_release)(IDirectSound*);

ULONG __stdcall dsound_release_hook(IDirectSound* me)
{
	trace("directsound release\n");

	EnterCriticalSection(&winamp_mutex);

	return real_dsound_release(me);
}

// Apply x^3 function
uint winamp_volume_fn()
{
	uint vol_min, vol_max, step;
	if (winamp_trans_prev_volume > winamp_trans_target_volume) {
		vol_min = winamp_trans_target_volume;
		vol_max = winamp_trans_prev_volume;
		step = winamp_trans_counter;
	}
	else {
		vol_min = winamp_trans_prev_volume;
		vol_max = winamp_trans_target_volume;
		step = winamp_trans_steps - winamp_trans_counter;
	}
	double st = step / double(winamp_trans_steps);
	return vol_min + (st * st * st) * double(vol_max - vol_min);
}

unsigned __stdcall winamp_render_thread(void* parameter)
{
	int activity_detected_count = 0;
	int previous_written_time = -1;
	bool next_music_started = false;

	while (!winamp_stop_thread)
	{
		Sleep(thread_sleep_ms);

		EnterCriticalSection(&winamp_mutex);

		if (*common_externals.directsound && in && out)
		{
			bool start_next_song = false;

			// Reset next_music_started if another song is requested
			if (next_music_started && winamp_crossfade_midi)
			{
				next_music_started = false;
			}

			// Volume trans
			if (winamp_trans_counter > 0)
			{
				winamp_song_volume = winamp_volume_fn();

				winamp_apply_volume();

				winamp_trans_counter -= 1;

				if (!winamp_trans_counter)
				{
					// Apply final volume
					winamp_song_volume = winamp_trans_target_volume;

					winamp_apply_volume();

					// Play crossfade music
					if (winamp_crossfade_midi)
					{
						winamp_load_song(winamp_crossfade_midi, winamp_crossfade_id);

						// Request volume fade-in (later, when the song is really played)
						next_music_started = true;
						activity_detected_count = 0;
						winamp_crossfade_id = 0;
						winamp_crossfade_midi = 0;
					}
				}
			}

			if (winamp_current_id > 0)
			{
				int output_time = out->getOutputTime(),
					written_time = out->getWrittenTime();

				if (!winamp_song_ended && output_time >= in->getLength())
				{
					winamp_song_ended = true;
				}

				if (next_music_started && !winamp_song_paused
					&& previous_written_time >= 0)
				{
					if (written_time > previous_written_time)
					{
						activity_detected_count += 1;
					}
					else
					{
						activity_detected_count = 0;
					}
				}

				previous_written_time = written_time;

				if (next_music_started && activity_detected_count > 3)
				{
					trace("Music activity detected at %i ms\n", output_time);

					// Start volume fade-in
					if (winamp_crossfade_time)
					{
						winamp_trans_target_volume = 127;
						winamp_trans_counter = winamp_crossfade_time;
						winamp_trans_prev_volume = winamp_song_volume;
						winamp_trans_steps = winamp_trans_counter;
					}
					else
					{
						winamp_song_volume = 127;
						winamp_apply_volume();
					}

					winamp_crossfade_time = 0;
					activity_detected_count = 0;
					next_music_started = false;
				}
			}
			else {
				activity_detected_count = 0;
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

	if (id != winamp_current_id || winamp_song_ended)
	{
		winamp_load_song(midi, id);
	}

	LeaveCriticalSection(&winamp_mutex);
}

void winamp_stop_music()
{
	trace("[%s] stop music\n", getmode_cached()->name);

	EnterCriticalSection(&winamp_mutex);
	
	if (in) {
		uint mode = getmode_cached()->driver_mode;

		if (ff8 && out && winamp_current_id
				&& needs_resume(mode, winamp_current_mode, nullptr, winamp_current_midi)) {
			winamp_paused_midi_id = winamp_current_id;
			winamp_paused_midi_ms = out->getOutputTime() % in->getLength();

			if (winamp_paused_midi_ms < 0) {
				winamp_paused_midi_ms = 0;
			}

			info("Saved midi time ms: %i\n", winamp_paused_midi_ms);
		}

		in->stop();
	}
	
	winamp_song_ended = true;
	winamp_current_id = 0;

	LeaveCriticalSection(&winamp_mutex);
}

// cross fade to a new song
void winamp_cross_fade_music(char *midi, uint id, int time)
{
	int fade_time = time * 4;

	trace("[%s] cross fade music: %s:%i (%i)\n", getmode_cached()->name, midi, id, time);

	EnterCriticalSection(&winamp_mutex);

	if (id != winamp_current_id)
	{
		if (winamp_current_id != 0 && fade_time)
		{
			winamp_trans_counter = fade_time;
		}
		else
		{
			winamp_trans_counter = 1;
		}

		winamp_trans_target_volume = 0;
		winamp_trans_prev_volume = winamp_song_volume;
		winamp_trans_steps = winamp_trans_counter;

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

void winamp_set_direct_volume(int volume)
{
	trace("set direct volume: %i\n", volume);

	if (out)
	{
		out->setVolume(volume);
	}
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

	winamp_trans_target_volume = 0;
	winamp_trans_counter = 0;
	winamp_trans_prev_volume = 0;
	winamp_trans_steps = winamp_trans_counter;

	winamp_apply_volume();

	LeaveCriticalSection(&winamp_mutex);
}

// make a volume transition
void winamp_set_music_volume_trans(int volume, int frames)
{
	trace("set volume trans: %i (%i)\n", volume, frames);

	frames /= 2; // 60 FPS on the original game, our thread has ~30 ticks per second

	EnterCriticalSection(&winamp_mutex);
	
	if (frames < 2)
	{
		winamp_trans_target_volume = 0;
		winamp_trans_counter = 0;
		winamp_trans_prev_volume = volume;
		winamp_trans_steps = winamp_trans_counter;

		winamp_song_volume = volume;
		winamp_apply_volume();
	}
	else
	{
		winamp_trans_target_volume = volume;
		winamp_trans_counter = frames;
		winamp_trans_prev_volume = winamp_song_volume;
		winamp_trans_steps = winamp_trans_counter;
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