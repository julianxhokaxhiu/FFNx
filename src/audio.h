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

#include <stack>
#include <string>
#include <soloud/soloud.h>
#include "audio/vgmstream/vgmstream.h"
#include "audio/winamp/winamp.h"

#define NXAUDIOENGINE_INVALID_HANDLE 0xfffff000;

class NxAudioEngine
{
private:
	SoLoud::Soloud _engine;
	WinampInPlugin* _winampInPlugin;

	// MUSIC
	SoLoud::handle _musicHandle = NXAUDIOENGINE_INVALID_HANDLE;

	float _musicMasterVolume = 100.0f;

	std::stack<SoLoud::handle> _musicStack;

	std::string _lastMusicName;

	void getMusicFilenameFullPath(char* _out, char* _name);

	// VOICE
	SoLoud::handle _voiceHandle = NXAUDIOENGINE_INVALID_HANDLE;

	float _voiceMasterVolume = 100.0f;

	std::string _lastVoiceName;

	void getVoiceFilenameFullPath(char* _out, char* _name);

public:
	bool init();
	void cleanup();

	// Audio
	// TODO

	// Music
	bool canPlayMusic(char* name);
	void playMusic(uint32_t midi, char* name, bool crossfade = false, uint32_t time = 0);
	void stopMusic();
	void pauseMusic();
	void resumeMusic();
	bool isMusicPlaying();
	void setMusicMasterVolume(float _volume);
	float getMusicVolume();
	void setMusicVolume(float _volume, size_t time = 0);
	void setMusicSpeed(float speed);

	// Voice
	void playVoice(char* name);
};

extern NxAudioEngine nxAudioEngine;