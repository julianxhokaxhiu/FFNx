/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

	static bool DSoundPlay(IDirectSoundBuffer* buffer);
	static bool DSoundStop(IDirectSoundBuffer* buffer);
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
