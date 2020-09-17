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
#include <vector>
#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>
#include "audio/vgmstream/vgmstream.h"
#include "audio/openpsf/openpsf.h"

#define NXAUDIOENGINE_INVALID_HANDLE 0xfffff000

class NxAudioEngine
{
private:
	bool _engineInitialized = false;
	SoLoud::Soloud _engine;
	bool _openpsf_loaded = false;

	// SFX
	std::stack<int> _sfxStack;
	std::vector<float> _sfxVolumePerChannels;
	std::vector<float> _sfxTempoPerChannels;
	std::vector<SoLoud::Wav*> _sfxStreams;

	void getSFXFilenameFullPath(char* _out, int _id);

	// MUSIC
	SoLoud::handle _musicHandle = NXAUDIOENGINE_INVALID_HANDLE;

	float _previousMusicMasterVolume = 1.0f;
	float _musicMasterVolume = 1.0f;

	float _wantedMusicVolume = 1.0f;
	std::stack<SoLoud::handle> _musicStack;

	void getMusicFilenameFullPath(char* _out, char* _name);

	// VOICE
	SoLoud::handle _voiceHandle = NXAUDIOENGINE_INVALID_HANDLE;

	void getVoiceFilenameFullPath(char* _out, char* _name);

	// MISC
	bool fileExists(char* filename);

public:
	bool init();
	void flush();
	void cleanup();

	// SFX
	bool canPlaySFX(int id);
	void loadSFX(int id);
	void unloadSFX(int id);
	void playSFX(int id, int channel, float panning);
	void setSFXVolume(float volume, int channel);
	void setSFXSpeed(float speed, int channel);

	// Music
	bool canPlayMusic(char* name);
	void playMusic(char* name, bool crossfade = false, uint32_t time = 0);
	void stopMusic(uint32_t time = 0);
	void pauseMusic();
	void resumeMusic();
	bool isMusicPlaying();
	void setMusicMasterVolume(float volume, size_t time = 0);
	void restoreMusicMasterVolume(size_t time = 0);
	float getMusicVolume();
	void setMusicVolume(float volume, size_t time = 0);
	void resetMusicVolume(size_t time = 0);
	void setMusicSpeed(float speed);
	void setMusicLooping(bool looping);

	// Voice
	bool canPlayVoice(char* name);
	void playVoice(char* name);
	void stopVoice(uint32_t time = 0);
};

extern NxAudioEngine nxAudioEngine;