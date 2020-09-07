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

#include <soloud/miniaudio.h>
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

// PUBLIC

bool NxAudioEngine::init()
{
	if (_engine.init() == 0)
	{
		*common_externals.directsound = (IDirectSound*)((ma_device*)_engine.mBackendDevice)->dsound.pPlayback;

		return true;
	}

	return false;
}

void NxAudioEngine::cleanup()
{
	_engine.stopAll();
	_engine.deinit();
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

void NxAudioEngine::playMusic(uint32_t midi, char* name, bool crossfade, uint32_t time)
{
	SoLoud::VGMStream* music = new SoLoud::VGMStream();

	char filename[MAX_PATH];

	getMusicFilenameFullPath(filename, name);

	music->load(filename);

	// Fade out or stop the previous playing music
	if (_engine.isValidVoiceHandle(_musicHandle))
	{
		if (crossfade)
			_engine.fadeVolume(_musicHandle, 0, time);
		else
			_engine.stop(_musicHandle);
	}

	_musicHandle = _engine.playBackground(*music, crossfade ? 0.0f : 1.0f);
	if (crossfade) setMusicVolume(1.0f, time);
}

void NxAudioEngine::stopMusic()
{
	_engine.stop(_musicHandle);
}

void NxAudioEngine::pauseMusic()
{
	_engine.setPause(_musicHandle, true);
}

void NxAudioEngine::resumeMusic()
{
	_engine.setPause(_musicHandle, false);
}

bool NxAudioEngine::isMusicPlaying()
{
	return true;
}

void NxAudioEngine::setMusicMasterVolume(float _volume)
{
	_musicMasterVolume = _volume;
}

void NxAudioEngine::setMusicVolume(float _volume, size_t time)
{	
	float volume = 1.0f / ((_volume * _musicMasterVolume) / 100);

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
void NxAudioEngine::playVoice(char* name)
{
	SoLoud::VGMStream* voice = new SoLoud::VGMStream();

	char filename[MAX_PATH];

	getVoiceFilenameFullPath(filename, name);

	voice->load(filename);

	// Stop any previously playing voice
	if (_engine.isValidVoiceHandle(_voiceHandle)) _engine.stop(_voiceHandle);

	_voiceHandle = _engine.play(*voice);
}