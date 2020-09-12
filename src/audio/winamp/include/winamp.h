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

#include "windows.h"

#define OUT_VER 0x10
#define IN_UNICODE 0x0F000000
#define IN_VER 0x100
#define IN_VER_UNICODE (IN_UNICODE | IN_VER)

struct WinampOutModule
{
	int version;				// module version (OUT_VER)
	char* description;			// description of module, with version string
	intptr_t id;						// module id. each input module gets its own. non-nullsoft modules should
								// be >= 65536.

	HWND hMainWindow;			// winamp's main window (filled in by winamp)
	HINSTANCE hDllInstance;		// DLL instance handle (filled in by winamp)

	void (*Config)(HWND hwndParent); // configuration dialog
	void (*About)(HWND hwndParent);  // about dialog

	void (*Init)();				// called when loaded
	void (*Quit)();				// called when unloaded

	int (*Open)(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms);
	// returns >=0 on success, <0 on failure

	// NOTENOTENOTE: bufferlenms and prebufferms are ignored in most if not all output plug-ins.
	//    ... so don't expect the max latency returned to be what you asked for.
	// returns max latency in ms (0 for diskwriters, etc)
	// bufferlenms and prebufferms must be in ms. 0 to use defaults.
	// prebufferms must be <= bufferlenms
	// pass bufferlenms==-666 to tell the output plugin that it's clock is going to be used to sync video
	//   out_ds turns off silence-eating when -666 is passed

	void (*Close)();	// close the ol' output device.

	int (*Write)(char* buf, int len);
	// 0 on success. Len == bytes to write (<= 8192 always). buf is straight audio data.
	// 1 returns not able to write (yet). Non-blocking, always.

	int (*CanWrite)();	// returns number of bytes possible to write at a given time.
						// Never will decrease unless you call Write (or Close, heh)

	int (*IsPlaying)(); // non0 if output is still going or if data in buffers waiting to be
						// written (i.e. closing while IsPlaying() returns 1 would truncate the song

	int (*Pause)(int pause); // returns previous pause state

	void (*SetVolume)(int volume); // volume is 0-255
	void (*SetPan)(int pan); // pan is -128 to 128

	void (*Flush)(int t);	// flushes buffers and restarts output at time t (in ms)
							// (used for seeking)

	int (*GetOutputTime)(); // returns played time in MS
	int (*GetWrittenTime)(); // returns time written in MS (used for synching up vis stuff)
};

struct WinampInModuleA
{
	int version;				// module type (IN_VER)
	char* description;			// description of module, with version string

	HWND hMainWindow;			// winamp's main window (filled in by winamp)
	HINSTANCE hDllInstance;		// DLL instance handle (Also filled in by winamp)

	char* FileExtensions;		// "mp3\0Layer 3 MPEG\0mp2\0Layer 2 MPEG\0mpg\0Layer 1 MPEG\0"
								// May be altered from Config, so the user can select what they want

	int is_seekable;			// is this stream seekable?
	int UsesOutputPlug;			// does this plug-in use the output plug-ins? (musn't ever change, ever :)
													// note that this has turned into a "flags" field
													// see IN_MODULE_FLAG_*

	void (*Config)(HWND hwndParent); // configuration dialog
	void (*About)(HWND hwndParent);  // about dialog

	void (*Init)();				// called at program init
	void (*Quit)();				// called at program quit

#define GETFILEINFO_TITLE_LENGTH 2048
	void (*GetFileInfo)(const char* file, char* title, int* length_in_ms); // if file == NULL, current playing is used

#define INFOBOX_EDITED 0
#define INFOBOX_UNCHANGED 1
	int (*InfoBox)(const char* file, HWND hwndParent);

	int (*IsOurFile)(const char* fn);	// called before extension checks, to allow detection of mms://, etc
	// playback stuff
	int (*Play)(const char* fn);		// return zero on success, -1 on file-not-found, some other value on other (stopping winamp) error
	void (*Pause)();			// pause stream
	void (*UnPause)();			// unpause stream
	int (*IsPaused)();			// ispaused? return 1 if paused, 0 if not
	void (*Stop)();				// stop (unload) stream

	// time stuff
	int (*GetLength)();			// get length in ms
	int (*GetOutputTime)();		// returns current output time in ms. (usually returns outMod->GetOutputTime()
	void (*SetOutputTime)(int time_in_ms);	// seeks to point in stream (in ms). Usually you signal your thread to seek, which seeks and calls outMod->Flush()..

	// volume stuff
	void (*SetVolume)(int volume);	// from 0 to 255.. usually just call outMod->SetVolume
	void (*SetPan)(int pan);	// from -127 to 127.. usually just call outMod->SetPan

	// in-window builtin vis stuff

	void (*SAVSAInit)(int maxlatency_in_ms, int srate);		// call once in Play(). maxlatency_in_ms should be the value returned from outMod->Open()
	// call after opening audio device with max latency in ms and samplerate
	void (*SAVSADeInit)();	// call in Stop()


	// simple vis supplying mode
	void (*SAAddPCMData)(void* PCMData, int nch, int bps, int timestamp);
	// sets the spec data directly from PCM data
	// quick and easy way to get vis working :)
	// needs at least 576 samples :)

// advanced vis supplying mode, only use if you're cool. Use SAAddPCMData for most stuff.
	int (*SAGetMode)();		// gets csa (the current type (4=ws,2=osc,1=spec))
							// use when calling SAAdd()
	int (*SAAdd)(void* data, int timestamp, int csa); // sets the spec data, filled in by winamp


	// vis stuff (plug-in)
	// simple vis supplying mode
	void (*VSAAddPCMData)(void* PCMData, int nch, int bps, int timestamp); // sets the vis data directly from PCM data
											// quick and easy way to get vis working :)
											// needs at least 576 samples :)

	// advanced vis supplying mode, only use if you're cool. Use VSAAddPCMData for most stuff.
	int (*VSAGetMode)(int* specNch, int* waveNch); // use to figure out what to give to VSAAdd
	int (*VSAAdd)(void* data, int timestamp); // filled in by winamp, called by plug-in


	// call this in Play() to tell the vis plug-ins the current output params.
	void (*VSASetInfo)(int srate, int nch); // <-- Correct (benski, dec 2005).. old declaration had the params backwards


	// dsp plug-in processing:
	// (filled in by winamp, calld by input plug)

	// returns 1 if active (which means that the number of samples returned by dsp_dosamples
	// could be greater than went in.. Use it to estimate if you'll have enough room in the
	// output buffer
	int (*dsp_isactive)();

	// returns number of samples to output. This can be as much as twice numsamples.
	// be sure to allocate enough buffer for samples, then.
	int (*dsp_dosamples)(short int* samples, int numsamples, int bps, int nch, int srate);


	// eq stuff
	void (*EQSet)(int on, char data[10], int preamp); // 0-64 each, 31 is +0, 0 is +12, 63 is -12. Do nothing to ignore.

	// info setting (filled in by winamp)
	void (*SetInfo)(int bitrate, int srate, int stereo, int synched); // if -1, changes ignored? :)

	WinampOutModule* outMod; // filled in by winamp, optionally used :)
};

struct WinampInModuleW
{
	int version;				// module type (IN_VER_UNICODE)
	char* description;			// description of module, with version string

	HWND hMainWindow;			// winamp's main window (filled in by winamp)
	HINSTANCE hDllInstance;		// DLL instance handle (Also filled in by winamp)

	char* FileExtensions;		// "mp3\0Layer 3 MPEG\0mp2\0Layer 2 MPEG\0mpg\0Layer 1 MPEG\0"
								// May be altered from Config, so the user can select what they want

	int is_seekable;			// is this stream seekable?
	int UsesOutputPlug;			// does this plug-in use the output plug-ins? (musn't ever change, ever :)
													// note that this has turned into a "flags" field
													// see IN_MODULE_FLAG_*

	void (*Config)(HWND hwndParent); // configuration dialog
	void (*About)(HWND hwndParent);  // about dialog

	void (*Init)();				// called at program init
	void (*Quit)();				// called at program quit

	void (*GetFileInfo)(const wchar_t* file, wchar_t* title, int* length_in_ms); // if file == NULL, current playing is used

	int (*InfoBox)(const wchar_t* file, HWND hwndParent);

	int (*IsOurFile)(const wchar_t* fn);	// called before extension checks, to allow detection of mms://, etc
	// playback stuff
	int (*Play)(const wchar_t* fn);		// return zero on success, -1 on file-not-found, some other value on other (stopping winamp) error
	void (*Pause)();			// pause stream
	void (*UnPause)();			// unpause stream
	int (*IsPaused)();			// ispaused? return 1 if paused, 0 if not
	void (*Stop)();				// stop (unload) stream

	// time stuff
	int (*GetLength)();			// get length in ms
	int (*GetOutputTime)();		// returns current output time in ms. (usually returns outMod->GetOutputTime()
	void (*SetOutputTime)(int time_in_ms);	// seeks to point in stream (in ms). Usually you signal your thread to seek, which seeks and calls outMod->Flush()..

	// volume stuff
	void (*SetVolume)(int volume);	// from 0 to 255.. usually just call outMod->SetVolume
	void (*SetPan)(int pan);	// from -127 to 127.. usually just call outMod->SetPan

	// in-window builtin vis stuff

	void (*SAVSAInit)(int maxlatency_in_ms, int srate);		// call once in Play(). maxlatency_in_ms should be the value returned from outMod->Open()
	// call after opening audio device with max latency in ms and samplerate
	void (*SAVSADeInit)();	// call in Stop()


	// simple vis supplying mode
	void (*SAAddPCMData)(void* PCMData, int nch, int bps, int timestamp);
	// sets the spec data directly from PCM data
	// quick and easy way to get vis working :)
	// needs at least 576 samples :)

// advanced vis supplying mode, only use if you're cool. Use SAAddPCMData for most stuff.
	int (*SAGetMode)();		// gets csa (the current type (4=ws,2=osc,1=spec))
							// use when calling SAAdd()
	int (*SAAdd)(void* data, int timestamp, int csa); // sets the spec data, filled in by winamp


	// vis stuff (plug-in)
	// simple vis supplying mode
	void (*VSAAddPCMData)(void* PCMData, int nch, int bps, int timestamp); // sets the vis data directly from PCM data
											// quick and easy way to get vis working :)
											// needs at least 576 samples :)

	// advanced vis supplying mode, only use if you're cool. Use VSAAddPCMData for most stuff.
	int (*VSAGetMode)(int* specNch, int* waveNch); // use to figure out what to give to VSAAdd
	int (*VSAAdd)(void* data, int timestamp); // filled in by winamp, called by plug-in


	// call this in Play() to tell the vis plug-ins the current output params.
	void (*VSASetInfo)(int srate, int nch); // <-- Correct (benski, dec 2005).. old declaration had the params backwards


	// dsp plug-in processing:
	// (filled in by winamp, calld by input plug)

	// returns 1 if active (which means that the number of samples returned by dsp_dosamples
	// could be greater than went in.. Use it to estimate if you'll have enough room in the
	// output buffer
	int (*dsp_isactive)();

	// returns number of samples to output. This can be as much as twice numsamples.
	// be sure to allocate enough buffer for samples, then.
	int (*dsp_dosamples)(short int* samples, int numsamples, int bps, int nch, int srate);


	// eq stuff
	void (*EQSet)(int on, char data[10], int preamp); // 0-64 each, 31 is +0, 0 is +12, 63 is -12. Do nothing to ignore.

	// info setting (filled in by winamp)
	void (*SetInfo)(int bitrate, int srate, int stereo, int synched); // if -1, changes ignored? :)

	WinampOutModule* outMod; // filled in by winamp, optionally used :)
};

union WinampInModule {
	WinampInModuleA standard;
	WinampInModuleW wide;
};

typedef WinampOutModule* (CALLBACK* winampGetOutModule)();
typedef WinampInModule* (CALLBACK* winampGetInModule2)();
typedef int (CALLBACK* winampGetExtendedFileInfo)(char* filename, char* metadata, char* ret, int retlen);
