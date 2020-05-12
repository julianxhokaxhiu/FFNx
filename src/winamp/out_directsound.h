#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include "plugin.h"

struct CustomOutPluginState {
	IDirectSoundBuffer* sound_buffer;
	DWORD sound_buffer_size;
	DWORD sound_write_pointer;
	DWORD bytes_written;
	DWORD prebuffer_size;
	DWORD start_t;
	DWORD offset_t;
	DWORD paused_t;
	DWORD last_pause_t;
	DWORD last_stop_t;
	WAVEFORMATEX sound_format;
	bool play_started;
	bool clear_done;
};

class CustomOutPlugin : public AbstractOutPlugin {
private:
	static WinampOutContext static_context;
	static CustomOutPluginState state;
	static CustomOutPluginState dup_state;
	static int last_pause;
	static int last_volume;

	static void FakeDialog(HWND hwndParent);
	static void Noop();
	static int Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms);
	static void Close();
	static bool playHelper();
	static int Write(char* buf, int len);
	static int canWriteHelper(DWORD& current_play_cursor);
	static int CanWrite();
	// More likely: 'in': Hey 'out' I finished my decoding, have you finished playing?
	static int IsPlaying();
	static int Pause(int pause);
	static void SetVolume(int volume);
	static void SetPan(int pan);
	static void Flush(int t);
	static int GetOutputTime();
	static int GetWrittenTime();
	static void Duplicate();
	static int Resume(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms);
	static int CancelDuplicate();
public:
	CustomOutPlugin();
	virtual ~CustomOutPlugin();
	// This method is not part of the winamp plugin
	void setTempo(int tempo);
};
