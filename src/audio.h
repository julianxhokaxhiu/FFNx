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
		handle(NXAUDIOENGINE_INVALID_HANDLE),
		id(0),
		isResumable(false),
		wantedMusicVolume(1.0f) {}
	SoLoud::handle handle;
	uint32_t id;
	bool isResumable;
	float wantedMusicVolume;
};

class NxAudioEngine
{
public:
	enum PlayFlags {
		PlayFlagsNone = 0x0,
		PlayFlagsIsResumable = 0x1,
		PlayFlagsDoNotPause = 0x2
	};

	struct PlayOptions
	{
		PlayOptions() :
			offsetSeconds(0.0),
			flags(PlayFlagsNone),
			noIntro(false),
			fadetime(0.0),
			targetVolume(-1.0f),
			useNameAsFullPath(false)
		{}
		SoLoud::time offsetSeconds;
		PlayFlags flags;
		bool noIntro;
		SoLoud::time fadetime;
		float targetVolume;
		bool useNameAsFullPath;
	};
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
	NxAudioEngineMusic _musics[2];
	std::stack<NxAudioEngineMusic> _musicStack; // For resuming

	float _previousMusicMasterVolume = -1.0f;
	float _musicMasterVolume = -1.0f;
	SoLoud::time _lastVolumeFadeEndTime = 0.0;

	SoLoud::AudioSource* loadMusic(const char* name, bool isFullPath = false);
	void overloadPlayArgumentsFromConfig(char* name, uint32_t *id, PlayOptions *playOptions);
	void resetMusicVolume(int channel, double time = 0);

	// VOICE
	SoLoud::handle _voiceHandle = NXAUDIOENGINE_INVALID_HANDLE;

	// MISC
	// Returns false if the file does not exist
	template <class T>
	bool getFilenameFullPath(char *_out, T _key, NxAudioEngineLayer _type);

	bool fileExists(const char* filename);

	// CFG
	std::unordered_map<NxAudioEngineLayer,toml::parse_result> nxAudioEngineConfig;

	void loadConfig();

public:

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
	bool playMusic(char* name, uint32_t id, int channel, PlayOptions& playOptions = PlayOptions());
	void playSynchronizedMusics(const std::vector<std::string>& names, uint32_t id, PlayOptions& playOptions = PlayOptions());
	void stopMusic(int channel, double time = 0);
	void pauseMusic(int channel, double time = 0, bool push = false);
	void resumeMusic(int channel, double time = 0, bool pop = false, const PlayFlags &playFlags = PlayFlags());
	bool isMusicPlaying(int channel);
	uint32_t currentMusicId(int channel);
	void setMusicMasterVolume(float volume, double time = 0);
	void restoreMusicMasterVolume(double time = 0);
	float getMusicVolume(int channel);
	bool isMusicVolumeFadeFinished();
	float getMusicMasterVolume();
	void setMusicVolume(float volume, int channel, double time = 0);
	void setMusicSpeed(float speed, int channel);
	void setMusicLooping(bool looping, int channel);

	// Voice
	bool canPlayVoice(const char* name);
	bool playVoice(const char* name, float volume = 1.0f);
	void stopVoice(double time = 0);
	bool isVoicePlaying();
};

NxAudioEngine::PlayFlags operator|(NxAudioEngine::PlayFlags flags, NxAudioEngine::PlayFlags other) {
	return static_cast<NxAudioEngine::PlayFlags>(static_cast<int>(flags) | static_cast<int>(other));
}

extern NxAudioEngine nxAudioEngine;
