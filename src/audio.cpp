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

#include "audio.h"

#include "log.h"

NxAudioEngine nxAudioEngine;

// PRIVATE

void NxAudioEngine::getMusicFilenameFullPath(char* _out, char* _name)
{
	sprintf(_out, "%s/%s/%s.%s", basedir, external_music_path, _name, external_music_ext);
}

void NxAudioEngine::getVoiceFilenameFullPath(char* _out, char* _name)
{
	sprintf(_out, "%s/%s/%s.%s", basedir, external_voice_path, _name, external_voice_ext);
}

bool NxAudioEngine::fileExists(char* filename)
{
	struct stat dummy;

	return (stat(filename, &dummy) == 0);
}

// PUBLIC

bool NxAudioEngine::init()
{
	if (_engine.init() == 0)
	{		
		if (nullptr != winamp_in_plugin)
		{
			_winampInPlugin = new WinampInPlugin(BufferOutPlugin::instance());

			if (!_winampInPlugin->open(winamp_in_plugin))
			{
				error("couldn't load %s, please verify 'winamp_in_plugin' or comment it\n", winamp_in_plugin);
				delete _winampInPlugin;
				_winampInPlugin = nullptr;
			}
			else {
				info("Winamp music plugin loaded using %s\n", winamp_in_plugin);
			}
		}
		else
		{
			_winampInPlugin = nullptr;
		}

		return true;
	}

	return false;
}

void NxAudioEngine::flush()
{
	_engine.stopAll();

	_musicStack.empty();
	_musicHandle = NXAUDIOENGINE_INVALID_HANDLE;
	
	_voiceHandle = NXAUDIOENGINE_INVALID_HANDLE;
}

void NxAudioEngine::cleanup()
{
	_engine.deinit();
	if (_winampInPlugin != nullptr) {
		delete _winampInPlugin;
		_winampInPlugin = nullptr;
	}
	BufferOutPlugin::destroyInstance();
}

// Audio

// Music
bool NxAudioEngine::canPlayMusic(char* name)
{
	struct stat dummy;

	char filename[MAX_PATH];

	getMusicFilenameFullPath(filename, name);

	return (stat(filename, &dummy) == 0);
}

void NxAudioEngine::playMusic(char* name, bool crossfade, uint32_t time)
{
	if (_engine.isValidVoiceHandle(_musicHandle)) stopMusic(crossfade ? time : 0);

	char filename[MAX_PATH];

	getMusicFilenameFullPath(filename, name);

	if (trace_all || trace_music) trace("NxAudioEngine::%s: %s\n", __func__, filename);

	if (fileExists(filename))
	{
		SoLoud::AudioSource* music = nullptr;

		if (_winampInPlugin != nullptr) {
			SoLoud::Winamp* winamp = new SoLoud::Winamp(_winampInPlugin, BufferOutPlugin::instance());
			music = dynamic_cast<SoLoud::AudioSource*>(winamp);

			if (winamp->load(filename) != SoLoud::SO_NO_ERROR) {
				error("Cannot load %s with winamp\n", filename);
				delete winamp;
				music = nullptr;
			}
		}

		if (music == nullptr) {
			SoLoud::VGMStream* vgmstream = new SoLoud::VGMStream();
			music = dynamic_cast<SoLoud::AudioSource*>(vgmstream);
			if (vgmstream->load(filename) != SoLoud::SO_NO_ERROR) {
				error("Cannot load %s with vgmstream\n", filename);
			}
		}

		_musicHandle = _engine.playBackground(*music, crossfade ? 0.0f : 1.0f);
		if (crossfade) setMusicVolume(1.0f, time);
	}
}

void NxAudioEngine::stopMusic(uint32_t time)
{
	if (time > 0)
	{
		_engine.fadeVolume(_musicHandle, 0, time);
		_engine.scheduleStop(_musicHandle, time);
	}
	else
	{
		_engine.stop(_musicHandle);
	}
}

void NxAudioEngine::pauseMusic()
{
	_engine.setPause(_musicHandle, true);

	// Save for later usage
	_musicStack.push(_musicHandle);

	// Invalidate the current handle
	_musicHandle = NXAUDIOENGINE_INVALID_HANDLE;
}

void NxAudioEngine::resumeMusic()
{
	// Whatever is currently playing, just stop it
	// If the handle is still invalid, nothing will happen
	_engine.stop(_musicHandle);

	// Restore the last known paused music
	_musicHandle = _musicStack.top();
	_musicStack.pop();

	// Play it again from where it was left off
	_engine.setPause(_musicHandle, false);
}

bool NxAudioEngine::isMusicPlaying()
{
	return _engine.isValidVoiceHandle(_musicHandle);
}

void NxAudioEngine::setMusicMasterVolume(float _volume)
{
	_musicMasterVolume = _volume;

	setMusicVolume(1.0f);
}

float NxAudioEngine::getMusicVolume()
{
	return _engine.getVolume(_musicHandle);
}

void NxAudioEngine::setMusicVolume(float _volume, size_t time)
{	
	float volume = (_volume * _musicMasterVolume) / 100.0f;

	if (time > 0)
		_engine.fadeVolume(_musicHandle, volume, time);
	else
		_engine.setVolume(_musicHandle, volume);
}

void NxAudioEngine::setMusicSpeed(float speed)
{
	_engine.setRelativePlaySpeed(_musicHandle, speed);
}

// Voice
bool NxAudioEngine::canPlayVoice(char* name)
{
	struct stat dummy;

	char filename[MAX_PATH];

	getVoiceFilenameFullPath(filename, name);

	return (stat(filename, &dummy) == 0);
}

void NxAudioEngine::playVoice(char* name)
{
	std::string strName = name;

	char filename[MAX_PATH];

	getVoiceFilenameFullPath(filename, name);

	if (trace_all || trace_voice) trace("NxAudioEngine::%s: %s\n", __func__, filename);

	if (fileExists(filename))
	{
		SoLoud::VGMStream* voice = new SoLoud::VGMStream();

		voice->load(filename);

		// Stop any previously playing voice
		if (_engine.isValidVoiceHandle(_voiceHandle)) _engine.stop(_voiceHandle);

		_voiceHandle = _engine.play(*voice);
	}
}

void NxAudioEngine::stopVoice(uint32_t time)
{
	if (time > 0)
	{
		_engine.fadeVolume(_voiceHandle, 0, time);
		_engine.scheduleStop(_voiceHandle, time);
	}
	else
	{
		_engine.stop(_voiceHandle);
	}
}
