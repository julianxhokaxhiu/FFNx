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
#include <unordered_map>
#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>
#include <soloud/soloud_wavstream.h>
#include "audio/vgmstream/vgmstream.h"
#include "audio/openpsf/openpsf.h"

#define NXAUDIOENGINE_INVALID_HANDLE 0xfffff000

struct NxAudioEngineMusic
{
	NxAudioEngineMusic() :
		handle(NXAUDIOENGINE_INVALID_HANDLE), id(0), isResumable(false) {}
	SoLoud::handle handle;
	uint32_t id;
	bool isResumable;
};

class NxAudioEngine
{
private:
	enum NxAudioEngineLayer
	{
		NXAUDIOENGINE_SFX,
		NXAUDIOENGINE_MUSIC,
		NXAUDIOENGINE_VOICE
	};

	bool _engineInitialized = false;
	SoLoud::Soloud _engine;
	bool _openpsf_loaded = false;

	// SFX
	std::stack<int> _sfxStack;
	std::vector<float> _sfxVolumePerChannels;
	std::vector<float> _sfxTempoPerChannels;
	std::vector<SoLoud::Wav*> _sfxStreams;
	std::vector<SoLoud::handle> _sfxChannelsHandle;

	// MUSIC
	NxAudioEngineMusic _music = NxAudioEngineMusic();
	std::vector<SoLoud::handle> _musicSegmentsHandle;
	std::stack<NxAudioEngineMusic> _musicStack;

	float _previousMusicMasterVolume = 1.0f;
	float _musicMasterVolume = 1.0f;

	float _wantedMusicVolume = 1.0f;

	SoLoud::AudioSource* loadMusic(const char* name);

	// VOICE
	SoLoud::handle _voiceHandle = NXAUDIOENGINE_INVALID_HANDLE;

	// MISC
	template <class T>
	void getFilenameFullPath(char *_out, T _key, NxAudioEngineLayer _type);

	bool fileExists(const char* filename);

	// CFG
	std::unordered_map<NxAudioEngineLayer,toml::parse_result> nxAudioEngineConfig;

	void loadConfig();

public:
	enum PlayFlags {
		PlayFlagsNone = 0x0,
		PlayFlagsIsResumable = 0x1,
		PlayFlagsDoNotPause = 0x2
	};

	bool init();
	void flush();
	void cleanup();

	// SFX
	bool canPlaySFX(int id);
	void loadSFX(int id);
	void unloadSFX(int id);
	void playSFX(int id, int channel, float panning);
	void pauseSFX();
	void resumeSFX();
	void setSFXVolume(float volume, int channel);
	void setSFXSpeed(float speed, int channel);

	// Music
	bool canPlayMusic(const char* name);
	void playMusic(const char* name, uint32_t id, uint32_t fadetime = 0, PlayFlags flags = PlayFlagsNone, uint32_t offsetSeconds = 0);
	void playMusics(const std::vector<std::string>& names, uint32_t id, uint32_t time = 0);
	void stopMusic(uint32_t time = 0);
	void pauseMusic(uint32_t time = 0, bool push = false);
	void resumeMusic(uint32_t time = 0, bool pop = false);
	bool isMusicPlaying();
	uint32_t currentMusicId();
	void setMusicMasterVolume(float volume, size_t time = 0);
	void restoreMusicMasterVolume(size_t time = 0);
	float getMusicVolume();
	void setMusicVolume(float volume, size_t time = 0);
	void resetMusicVolume(size_t time = 0);
	void setMusicSpeed(float speed);
	void setMusicLooping(bool looping);

	// Voice
	bool canPlayVoice(const char* name);
	void playVoice(const char* name);
	void stopVoice(uint32_t time = 0);
};

NxAudioEngine::PlayFlags operator|(NxAudioEngine::PlayFlags flags, NxAudioEngine::PlayFlags other) {
	return static_cast<NxAudioEngine::PlayFlags>(static_cast<int>(flags) | static_cast<int>(other));
}

extern NxAudioEngine nxAudioEngine;
