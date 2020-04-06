#include "music.h"

CRITICAL_SECTION mutex;

#define AUDIO_BUFFER_SIZE 5

IDirectSoundBuffer* vgm_sound_buffer;
uint vgm_sound_buffer_size;
uint vgm_sound_write_pointer;

HANDLE musicRenderHandle;
unsigned musicRenderThreadID;

VGMSTREAM *vgmstream[100];
uint current_id;
uint bytes_written;
uint bytespersample;
uint end_pos;

uint song_ended = true;

int trans_step;
int trans_counter;
int trans_volume;

int crossfade_time;
uint crossfade_id;
char *crossfade_midi;

int master_volume;
int song_volume;

bool stop_thread = false;

void apply_volume()
{
	if(vgm_sound_buffer && *common_externals.directsound)
	{
		int volume = (((song_volume * 100) / 127) * master_volume) / 100;
		float decibel = 20.0f * log10f(volume / 100.0f);

		IDirectSoundBuffer_SetVolume(vgm_sound_buffer, volume ? (int)(decibel * 100.0f) : DSBVOLUME_MIN);
	}
}

void buffer_bytes(uint bytes)
{
	if(vgm_sound_buffer && bytes)
	{
		LPVOID ptr1;
		LPVOID ptr2;
		DWORD bytes1;
		DWORD bytes2;
		sample_t *buffer = (sample_t*)driver_malloc(bytes);

		if(vgmstream[current_id]->loop_flag) render_vgmstream(buffer, bytes / bytespersample, vgmstream[current_id]);
		else
		{
			uint render_bytes = (vgmstream[current_id]->num_samples - vgmstream[current_id]->current_sample) * bytespersample;

			if(render_bytes >= bytes) render_vgmstream(buffer, bytes / bytespersample, vgmstream[current_id]);
			if(render_bytes < bytes)
			{
				render_vgmstream(buffer, render_bytes / bytespersample, vgmstream[current_id]);

				memset(&buffer[render_bytes / sizeof(sample_t)], 0, bytes - render_bytes);
			}
		}

		if(IDirectSoundBuffer_Lock(vgm_sound_buffer, vgm_sound_write_pointer, bytes, &ptr1, &bytes1, &ptr2, &bytes2, 0)) error("couldn't lock sound buffer\n");

		memcpy(ptr1, buffer, bytes1);
		memcpy(ptr2, &buffer[bytes1 / sizeof(sample_t)], bytes2);

		if(IDirectSoundBuffer_Unlock(vgm_sound_buffer, ptr1, bytes1, ptr2, bytes2)) error("couldn't unlock sound buffer\n");

		vgm_sound_write_pointer = (vgm_sound_write_pointer + bytes1 + bytes2) % vgm_sound_buffer_size;
		bytes_written += bytes1 + bytes2;

		driver_free(buffer);
	}
}

void cleanup()
{
	if(vgm_sound_buffer && *common_externals.directsound) IDirectSoundBuffer_Release(vgm_sound_buffer);

	vgm_sound_buffer = 0;
}

void load_song(char *midi, uint id)
{
	char tmp[512];
	WAVEFORMATEX sound_format;
	DSBUFFERDESC1 sbdesc;

	cleanup();

	if(!id)
	{
		current_id = 0;
		return;
	}

	sprintf(tmp, "%s/%s/%s.ogg", basedir, external_music_path, midi);

	if(!vgmstream[id])
	{
		vgmstream[id] = init_vgmstream(tmp);

		if(!vgmstream[id])
		{
			error("Couldn't open music file: %s\n", tmp);
			return;
		}
	}

	sound_format.cbSize = sizeof(sound_format);
	sound_format.wBitsPerSample = 16;
	sound_format.nChannels = vgmstream[id]->channels;
	sound_format.nSamplesPerSec = vgmstream[id]->sample_rate;
	sound_format.nBlockAlign = sound_format.nChannels * sound_format.wBitsPerSample / 8;
	sound_format.nAvgBytesPerSec = sound_format.nSamplesPerSec * sound_format.nBlockAlign;
	sound_format.wFormatTag = WAVE_FORMAT_PCM;

	vgm_sound_buffer_size = sound_format.nAvgBytesPerSec * AUDIO_BUFFER_SIZE;

	sbdesc.dwSize = sizeof(sbdesc);
	sbdesc.lpwfxFormat = &sound_format;
	sbdesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	sbdesc.dwReserved = 0;
	sbdesc.dwBufferBytes = vgm_sound_buffer_size;

	if(IDirectSound_CreateSoundBuffer(*common_externals.directsound, (LPCDSBUFFERDESC)&sbdesc, &vgm_sound_buffer, 0))
	{
		error("couldn't create sound buffer (%i, %i)\n", vgmstream[id]->channels, vgmstream[id]->sample_rate);
		vgm_sound_buffer = 0;

		return;
	}

	bytespersample = vgmstream[id]->channels * sizeof(sample_t);
	vgm_sound_write_pointer = 0;
	bytes_written = 0;
	song_ended = false;

	if(!vgmstream[id]->loop_flag) end_pos = vgmstream[id]->num_samples * bytespersample;

	current_id = id;

	buffer_bytes(vgm_sound_buffer_size);

	apply_volume();

	if(IDirectSoundBuffer_Play(vgm_sound_buffer, 0, 0, DSBPLAY_LOOPING)) error("couldn't play sound buffer\n");
}

ULONG (__stdcall *real_dsound_release)(IDirectSound *);

ULONG __stdcall dsound_release_hook(IDirectSound *me)
{
	trace("directsound release\n");

	EnterCriticalSection(&mutex);

	return real_dsound_release(me);
}

unsigned __stdcall render_thread(void *parameter)
{
	while(!stop_thread)
	{
		Sleep(50);

		EnterCriticalSection(&mutex);

		if(*common_externals.directsound)
		{
			if(trans_counter > 0)
			{
				song_volume += trans_step;

				apply_volume();

				trans_counter--;

				if(!trans_counter)
				{
					song_volume = trans_volume;

					apply_volume();

					if(crossfade_midi)
					{
						load_song(crossfade_midi, crossfade_id);

						if(crossfade_time)
						{
							trans_volume = 127;
							trans_counter = crossfade_time;
							trans_step = (trans_volume - song_volume) / crossfade_time;
						}
						else
						{
							song_volume = 127;
							apply_volume();
						}

						crossfade_time = 0;
						crossfade_id = 0;
						crossfade_midi = 0;
					}
				}
			}

			if(vgm_sound_buffer && *common_externals.directsound)
			{
				DWORD play_cursor;
				uint bytes_to_write = 0;

				IDirectSoundBuffer_GetCurrentPosition(vgm_sound_buffer, &play_cursor, 0);

				if(!vgmstream[current_id]->loop_flag)
				{
					uint play_pos = ((bytes_written - vgm_sound_write_pointer) - vgm_sound_buffer_size) + play_cursor;

					if(play_pos > end_pos && !song_ended)
					{
						song_ended = true;

						trace("song ended at %i (%i)\n", play_pos, play_pos / bytespersample);

						IDirectSoundBuffer_Stop(vgm_sound_buffer);
					}
				}

				if(vgm_sound_write_pointer < play_cursor) bytes_to_write = play_cursor - vgm_sound_write_pointer;
				else if(vgm_sound_write_pointer > play_cursor) bytes_to_write = (vgm_sound_buffer_size - vgm_sound_write_pointer) + play_cursor;
				
				buffer_bytes(bytes_to_write);
			}
		}

		LeaveCriticalSection(&mutex);
	}

	return 0;
}

// called once just after the plugin has been loaded, <plugin_directsound> is a pointer to FF7s own directsound pointer
void vgm_music_init()
{
	InitializeCriticalSection(&mutex);

	musicRenderHandle = (HANDLE)_beginthreadex(NULL, 0, &render_thread, NULL, 0, &musicRenderThreadID);

	info("VGMStream music plugin loaded\n");
}

// start playing some music, <midi> is the name of the MIDI file without the .mid extension
void vgm_play_music(char *midi, uint id)
{
	trace("play music: %s\n", midi);

	EnterCriticalSection(&mutex);
	
	if(id != current_id || song_ended)
	{
		close_vgmstream(vgmstream[id]);
		vgmstream[id] = 0;

		load_song(midi, id);
	}

	LeaveCriticalSection(&mutex);
}

void vgm_stop_music()
{
	EnterCriticalSection(&mutex);

	cleanup();

	song_ended = true;

	LeaveCriticalSection(&mutex);
}

// cross fade to a new song
void vgm_cross_fade_music(char *midi, uint id, int time)
{
	int fade_time = time * 2;

	trace("cross fade music: %s (%i)\n", midi, time);

	EnterCriticalSection(&mutex);

	if(id != current_id || song_ended)
	{
		if(!song_ended && fade_time)
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
		crossfade_midi = midi;
	}

	LeaveCriticalSection(&mutex);
}

void vgm_pause_music()
{
	EnterCriticalSection(&mutex);

	if(vgm_sound_buffer) IDirectSoundBuffer_Stop(vgm_sound_buffer);

	LeaveCriticalSection(&mutex);
}

void vgm_resume_music()
{
	EnterCriticalSection(&mutex);

	if(vgm_sound_buffer && !song_ended) IDirectSoundBuffer_Play(vgm_sound_buffer, 0, 0, DSBPLAY_LOOPING);

	LeaveCriticalSection(&mutex);
}

// return true if music is playing, false if it isn't
// it's important for some field scripts that this function returns true atleast once when a song has been requested
// 
// even if there's nothing to play because of errors/missing files you cannot return false every time
uint vgm_music_status()
{
	uint last_status = 0;
	uint status;

	EnterCriticalSection(&mutex);

	status = !song_ended;

	if(!vgm_sound_buffer)
	{
		last_status = !last_status;
		return !last_status;
	}

	LeaveCriticalSection(&mutex);

	last_status = status;
	return status;
}

void vgm_set_master_music_volume(int volume)
{
	EnterCriticalSection(&mutex);

	master_volume = volume;

	apply_volume();

	LeaveCriticalSection(&mutex);
}

void vgm_set_music_volume(int volume)
{
	EnterCriticalSection(&mutex);

	song_volume = volume;

	trans_volume = 0;
	trans_counter = 0;
	trans_step = 0;

	apply_volume();

	LeaveCriticalSection(&mutex);
}

// make a volume transition
void vgm_set_music_volume_trans(int volume, int step)
{
	trace("set volume trans: %i (%i)\n", volume, step);

	step /= 4;

	EnterCriticalSection(&mutex);

	if(step < 2)
	{
		trans_volume = 0;
		trans_counter = 0;
		trans_step = 0;
		song_volume = volume;
		apply_volume();
	}
	else
	{
		trans_volume = volume;
		trans_counter = step;
		trans_step = (trans_volume - song_volume) / step;
	}

	LeaveCriticalSection(&mutex);
}

void vgm_set_music_tempo(unsigned char tempo)
{
	uint dstempo;

	EnterCriticalSection(&mutex);

	if(vgm_sound_buffer)
	{
		dstempo = (vgmstream[current_id]->sample_rate * (tempo + 480)) / 512;

		IDirectSoundBuffer_SetFrequency(vgm_sound_buffer, dstempo);
	}

	LeaveCriticalSection(&mutex);
}

void vgm_music_cleanup()
{
	stop_thread = true;

	vgm_stop_music();

	WaitForSingleObject(musicRenderHandle, INFINITE);

	CloseHandle(musicRenderHandle);

	DeleteCriticalSection(&mutex);
}