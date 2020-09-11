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

#include "plugin.h"

class AbstractOutPlugin {
protected:
	WinampOutModule* mod;
	WinampOutContext* context;
public:
	AbstractOutPlugin() : mod(nullptr), context(nullptr) {}
	virtual ~AbstractOutPlugin() {}
	inline WinampOutModule* getModule() const {
		return mod;
	}
	inline WinampOutContext* getContext() const {
		return context;
	}
	// volume stuff
	virtual void setVolume(int volume);	// from 0 to 255.. usually just call outMod->SetVolume
	void setPan(int pan);	    // from -127 to 127.. usually just call outMod->SetPan
	int getOutputTime() const;
	int getWrittenTime() const;
	void pause() const;
	void unPause() const;
};

class BufferOutPlugin : public AbstractOutPlugin {
private:
	static BufferOutPlugin* _instance;
	static char* _buffer;
	static int _bufferLength;
	static int _readPosition;
	static int _writePosition;
	static bool _clearDone;
	static bool _finishedPlaying;
	static int _sampleRate;
	static int _numChannels;
	static int _bitsPerSample;
	static int _lastPause;
	static CRITICAL_SECTION _mutex;
	static void FakeDialog(HWND hwndParent);
	static void Noop();
	static int Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms);
	static void Close();
	static int Write(char* buf, int len);
	static int CanWrite();
	// More likely: 'in': Hey 'out' I finished my decoding, have you finished playing?
	static int IsPlaying();
	static int Pause(int pause);
	static void SetVolume(int volume);
	static void SetPan(int pan);
	static void Flush(int t);
	static int GetOutputTime();
	static int GetWrittenTime();
	BufferOutPlugin();
	virtual ~BufferOutPlugin();
public:
	static BufferOutPlugin* instance();
	static void destroyInstance();

	int sampleRate() const;
	int numChannels() const;
	int bitsPerSample() const;

	int read(char* buf, int maxLen);
	bool finishedPlaying() const;
};
