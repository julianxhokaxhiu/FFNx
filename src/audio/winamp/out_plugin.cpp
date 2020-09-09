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

#include "out_plugin.h"

void AbstractOutPlugin::setVolume(int volume)
{
	if (nullptr != this->mod) {
		this->mod->SetVolume(volume);
	}
}

void AbstractOutPlugin::setPan(int pan)
{
	if (nullptr != this->mod) {
		this->mod->SetPan(pan);
	}
}

int AbstractOutPlugin::getOutputTime() const
{
	if (nullptr != this->mod) {
		return this->mod->GetOutputTime();
	}

	return 0;
}

int AbstractOutPlugin::getWrittenTime() const
{
	if (nullptr != this->mod) {
		return this->mod->GetWrittenTime();
	}

	return 0;
}

void AbstractOutPlugin::pause() const
{
	if (nullptr != this->mod) {
		this->mod->Pause(1);
	}
}

void AbstractOutPlugin::unPause() const
{
	if (nullptr != this->mod) {
		this->mod->Pause(0);
	}
}

WinampOutPlugin::WinampOutPlugin() :
	WinampPlugin(), AbstractOutPlugin()
{
}

WinampOutPlugin::~WinampOutPlugin()
{
	closeModule();
	close();
}

bool WinampOutPlugin::openModule(FARPROC procAddress)
{
	winampGetOutModule f = (winampGetOutModule)procAddress;
	this->mod = f();

	if (nullptr == this->mod) {
		error("couldn't call function %s in external library\n", procName());
		return false;
	}

	// Set fields
	this->mod->hMainWindow = nullptr;
	this->mod->hDllInstance = getHandle();
	// Initialize module
	if (nullptr != this->mod->Init) {
		this->mod->Init();
	}

	return true;
}

void WinampOutPlugin::closeModule()
{
	if (nullptr != this->mod) {
		if (nullptr != this->mod->Quit) {
			this->mod->Quit();
		}
		this->mod = nullptr;
	}
}

void WinampOutPlugin::setVolume(int volume)
{
	if (volume == 255) {
		// Force volume for MM (out_wave fix)
		for (int i = 0; i < waveInGetNumDevs(); ++i) {
			waveOutSetVolume(HWAVEOUT(i), 0xFFFFFFFF);
		}
	}
	AbstractOutPlugin::setVolume(volume);
}

BufferOutPlugin* BufferOutPlugin::_instance = nullptr;

char* BufferOutPlugin::_buffer = nullptr;
int BufferOutPlugin::_bufferLength = 0;
int BufferOutPlugin::_writePosition = 0;
int BufferOutPlugin::_toWriteLength = 0;

bool BufferOutPlugin::_finishedPlaying = true;

int BufferOutPlugin::_sampleRate = -1;
int BufferOutPlugin::_numChannels = -1;
int BufferOutPlugin::_bitsPerSample = -1;
int BufferOutPlugin::_lastPause = 0;

BufferOutPlugin* BufferOutPlugin::instance()
{
	if (_instance == nullptr) {
		_instance = new BufferOutPlugin();

	}

	return _instance;
}

void BufferOutPlugin::destroyInstance()
{
	if (_instance != nullptr) {
		delete _instance;
		_instance = nullptr;
	}
}

void BufferOutPlugin::FakeDialog(HWND hwndParent)
{
	UNUSED_PARAM(hwndParent);
	// Nothing to do
}

void BufferOutPlugin::Noop()
{
	// Nothing to do
}

int BufferOutPlugin::Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms)
{
	UNUSED_PARAM(bufferlenms);
	UNUSED_PARAM(prebufferms);

	if (trace_all || trace_music) trace("Open directsound %i %i %i\n", samplerate, numchannels, bitspersamp);

	Close();

	_finishedPlaying = false;
	_sampleRate = samplerate;
	_numChannels = numchannels;
	_bitsPerSample = bitspersamp;

	return 0;
}

void BufferOutPlugin::Close()
{
	if (trace_all || trace_music) trace("Close directsound (%i)\n", *common_externals.directsound ? 1 : 0);

	_finishedPlaying = true;
	_sampleRate = -1;
	_numChannels = -1;
	_bitsPerSample = -1;
}

int BufferOutPlugin::Write(char* buffer, int len)
{
	if (_writePosition + len > _toWriteLength) {
		return 0;
	}

	memcpy(_buffer + _writePosition, buffer, len);

	_writePosition += len;

	return len;
}

int BufferOutPlugin::CanWrite()
{
	if (_writePosition >= _toWriteLength) {
		return 0;
	}

	return _toWriteLength - _writePosition;
}

int BufferOutPlugin::IsPlaying()
{
	_finishedPlaying = true;
	return 0;
}

int BufferOutPlugin::Pause(int pause)
{
	int t = _lastPause;

	_lastPause = pause;

	return t;
}

void BufferOutPlugin::SetVolume(int volume)
{
	UNUSED_PARAM(volume);
}

void BufferOutPlugin::SetPan(int pan)
{
	UNUSED_PARAM(pan);
}

void BufferOutPlugin::Flush(int t)
{
	UNUSED_PARAM(t);
}

int BufferOutPlugin::GetOutputTime()
{
	return 0;
}

int BufferOutPlugin::GetWrittenTime()
{
	return 0;
}

BufferOutPlugin::BufferOutPlugin()
{
	mod = new WinampOutModule();
	mod->version = OUT_VER;
	mod->description = "FFNx Winamp Output Plugin";
	mod->id = 42; // Random id
	mod->hMainWindow = nullptr;
	mod->hDllInstance = nullptr;
	mod->Config = FakeDialog;
	mod->About = FakeDialog;
	mod->Init = Noop;
	mod->Quit = Noop;
	mod->Open = Open;
	mod->Close = Close;
	mod->Write = Write;
	mod->CanWrite = CanWrite;
	mod->IsPlaying = IsPlaying;
	mod->Pause = Pause;
	mod->SetVolume = SetVolume;
	mod->SetPan = SetPan;
	mod->Flush = Flush;
	mod->GetOutputTime = GetOutputTime;
	mod->GetWrittenTime = GetWrittenTime;
}

BufferOutPlugin::~BufferOutPlugin()
{
}

int BufferOutPlugin::sampleRate() const
{
	return _sampleRate;
}

int BufferOutPlugin::numChannels() const
{
	return _numChannels;
}

int BufferOutPlugin::bitsPerSample() const
{
	return _bitsPerSample;
}

bool BufferOutPlugin::finishedPlaying() const
{
	return _finishedPlaying;
}

int BufferOutPlugin::read(char* buf, int maxLen)
{
	_buffer = buf;
	_writePosition = 0;
	_toWriteLength = maxLen;

	const int sleepMs = 50;
	const int maxWait = 5000 / sleepMs;

	for (int i = 0; i < maxWait; ++i) {
		Sleep(sleepMs);

		if (_finishedPlaying || _writePosition >= maxLen) {
			return _writePosition;
		}
	}

	error("Timeout to write sound in buffer\n");

	return _writePosition;
}
