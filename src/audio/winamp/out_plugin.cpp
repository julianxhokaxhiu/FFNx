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

constexpr auto AUDIO_BUFFER_SECONDS = 5;

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

BufferOutPlugin* BufferOutPlugin::_instance = nullptr;

char* BufferOutPlugin::_buffer = nullptr;
int BufferOutPlugin::_bufferLength = 0;
int BufferOutPlugin::_readPosition = 0;
int BufferOutPlugin::_writePosition = 0;

bool BufferOutPlugin::_clearDone = false;
bool BufferOutPlugin::_finishedPlaying = true;

int BufferOutPlugin::_sampleRate = -1;
int BufferOutPlugin::_numChannels = -1;
int BufferOutPlugin::_bitsPerSample = -1;
int BufferOutPlugin::_lastPause = 0;

CRITICAL_SECTION BufferOutPlugin::_mutex;

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

	if (trace_all || trace_music) trace("Open buffer out plugin %i %i %i\n", samplerate, numchannels, bitspersamp);

	Close();

	_bufferLength = AUDIO_BUFFER_SECONDS * samplerate * numchannels * bitspersamp / 8;
	_writePosition = 0;
	_readPosition = 0;
	_buffer = new char[_bufferLength]();

	_clearDone = false;
	_finishedPlaying = false;

	_sampleRate = samplerate;
	_numChannels = numchannels;
	_bitsPerSample = bitspersamp;

	return 0;
}

void BufferOutPlugin::Close()
{
	if (trace_all || trace_music) trace("Close buffer out plugin\n");

	_clearDone = true;
	_finishedPlaying = true;
	_sampleRate = -1;
	_numChannels = -1;
	_bitsPerSample = -1;

	if (_buffer != nullptr) {
		delete[] _buffer;
		_buffer = nullptr;
	}
}

int BufferOutPlugin::Write(char* buffer, int len)
{
	if (trace_all) trace("BufferOutPlugin::Write(%i) %i / %i\n", len, _readPosition, _writePosition);

	if (_buffer == nullptr) {
		return 1;
	}

	EnterCriticalSection(&_mutex);

	// Cyclic buffer
	if (_writePosition + len > _bufferLength) {
		char* ptr1, * ptr2;
		size_t bytes1, bytes2;

		ptr2 = _buffer;
		bytes2 = _writePosition + len - _bufferLength;
		ptr1 = _buffer + _writePosition;
		bytes1 = len - bytes2;
		memcpy(ptr1, buffer, bytes1);
		memcpy(ptr2, buffer + bytes1, bytes2);
	}
	else {
		memcpy(_buffer + _writePosition, buffer, len);
	}

	_writePosition = (_writePosition + len) % _bufferLength;

	LeaveCriticalSection(&_mutex);

	return 0;
}

int BufferOutPlugin::CanWrite()
{
	if (_buffer == nullptr) {
		return 0;
	}

	const int readPosition = _readPosition;
	if (trace_all) trace("BufferOutPlugin::CanWrite %i / %i bufferLength: %i\n", readPosition, _writePosition, _bufferLength);

	if (readPosition <= _writePosition) {
		return (_bufferLength - _writePosition) + readPosition;
	}

	return readPosition - _writePosition;
}

int BufferOutPlugin::IsPlaying()
{
	if (trace_all || trace_music) trace("BufferOutPlugin::IsPlaying\n");

	if (_buffer == nullptr) {
		return 0;
	}

	const int canWrite = CanWrite();
	const int clearDataSize = _bufferLength / AUDIO_BUFFER_SECONDS / 2; // ~500 ms

	// The buffer is circular, so we make precautions
	if (_clearDone) {
		if (_readPosition < _writePosition
			|| _readPosition > _writePosition + clearDataSize) {
			return 1;
		}

		_finishedPlaying = true;

		Close();

		return 0;
	}

	// Clear some data after the write pointer when it is possible (to remove artifacts)
	if (canWrite >= clearDataSize) {
		EnterCriticalSection(&_mutex);

		// Cyclic buffer
		if (_writePosition + clearDataSize > _bufferLength) {
			char* ptr1, * ptr2;
			size_t bytes1, bytes2;

			ptr2 = _buffer;
			bytes2 = _writePosition + clearDataSize - _bufferLength;
			ptr1 = _buffer + _writePosition;
			bytes1 = clearDataSize - bytes2;
			memset(ptr1, 0, bytes1);
			memset(ptr2, 0, bytes2);
		}
		else {
			memset(_buffer + _writePosition, 0, clearDataSize);
		}

		LeaveCriticalSection(&_mutex);

		_clearDone = true;
	}

	return 1;
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

	InitializeCriticalSection(&_mutex);
}

BufferOutPlugin::~BufferOutPlugin()
{
	DeleteCriticalSection(&_mutex);
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
	const int sleepMs = 50;
	const int maxWait = 5000 / sleepMs;
	int bufCur = 0;

	for (int i = 0; i < maxWait; ++i) {
		if (trace_all) trace("BufferOutPlugin::read(%i) %i %i\n", maxLen, bufCur, _readPosition);

		if (_buffer == nullptr) {
			return bufCur;
		}

		EnterCriticalSection(&_mutex);

		const int writePosition = _writePosition;
		const int bufferLength = _bufferLength;

		if (_readPosition != writePosition) {
			int toRead = maxLen - bufCur;
			int available = _readPosition < writePosition
				? writePosition - _readPosition
				: bufferLength - _readPosition;
			int bytes = toRead < available ? toRead : available;

			if (bytes > 0) {
				memcpy(buf + bufCur, _buffer + _readPosition, bytes);

				_readPosition = (_readPosition + bytes) % bufferLength;
				bufCur += bytes;

				if (_readPosition > writePosition) {
					toRead -= bytes;
					available = writePosition;
					bytes = toRead < available ? toRead : available;

					if (bytes > 0) {
						memcpy(buf + bufCur, _buffer, bytes);

						_readPosition = (_readPosition + bytes) % bufferLength;
						bufCur += bytes;
					}
				}

				if (bufCur >= maxLen) {
					LeaveCriticalSection(&_mutex);

					return bufCur;
				}
			}
		}

		LeaveCriticalSection(&_mutex);
		
		Sleep(sleepMs);
	}

	error("Timeout to read sound from buffer\n");

	return bufCur;
}
