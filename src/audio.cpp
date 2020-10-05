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

void NxAudioEngine::loadConfig()
{
	char _fullpath[MAX_PATH];

	for (int idx = NxAudioEngineLayer::NXAUDIOENGINE_SFX; idx != NxAudioEngineLayer::NXAUDIOENGINE_VOICE; idx++)
	{
		NxAudioEngineLayer type = NxAudioEngineLayer(idx);

		switch (type)
		{
		case NxAudioEngineLayer::NXAUDIOENGINE_SFX:
			sprintf(_fullpath, "%s/%s/config.toml", basedir, external_sfx_path.c_str());
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_MUSIC:
			sprintf(_fullpath, "%s/%s/config.toml", basedir, external_music_path.c_str());
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_VOICE:
			sprintf(_fullpath, "%s/%s/config.toml", basedir, external_voice_path.c_str());
			break;
		}

		try
		{
			nxAudioEngineConfig[type] = toml::parse_file(_fullpath);
		}
		catch (const toml::parse_error &err)
		{
			nxAudioEngineConfig[type] = toml::parse("");
		}
	}
}

template <class T>
void NxAudioEngine::getFilenameFullPath(char *_out, T _key, NxAudioEngineLayer _type)
{
	switch(_type)
	{
		case NxAudioEngineLayer::NXAUDIOENGINE_SFX:
			sprintf(_out, "%s/%s/%d.%s", basedir, external_sfx_path.c_str(), _key, external_sfx_ext.c_str());
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_MUSIC:
			sprintf(_out, "%s/%s/%s.%s", basedir, external_music_path.c_str(), _key, external_music_ext.c_str());
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_VOICE:
			sprintf(_out, "%s/%s/%s.%s", basedir, external_voice_path.c_str(), _key, external_voice_ext.c_str());
			break;
	}
}

bool NxAudioEngine::fileExists(const char* filename)
{
	struct stat dummy;

	bool ret = (stat(filename, &dummy) == 0);

	if (!ret && (trace_all || trace_music)) warning("NxAudioEngine::%s: Could not find file %s\n", __func__, filename);

	return ret;
}

// PUBLIC

bool NxAudioEngine::init()
{
	if (_engine.init() == 0)
	{
		_engineInitialized = true;

		loadConfig();

		if (!he_bios_path.empty()) {
			if (!Psf::initialize_psx_core(he_bios_path.c_str())) {
				error("NxAudioEngine::%s couldn't load %s, please verify 'he_bios_path' or comment it\n", __func__, he_bios_path.c_str());
			}
			else {
				_openpsf_loaded = true;
				info("NxAudioEngine::%s OpenPSF music plugin loaded using %s\n", __func__, he_bios_path.c_str());
			}
		}

		_sfxVolumePerChannels.resize(10, 1.0f);
		_sfxTempoPerChannels.resize(10, 1.0f);
		_sfxChannelsHandle.resize(10, NXAUDIOENGINE_INVALID_HANDLE);
		_sfxStreams.resize(1000, nullptr);

		while (!_sfxStack.empty())
		{
			loadSFX(_sfxStack.top());
			_sfxStack.pop();
		}

		return true;
	}

	return false;
}

void NxAudioEngine::flush()
{
	_engine.stopAll();

	_musicStack.empty();
	_music.handle = NXAUDIOENGINE_INVALID_HANDLE;

	_voiceHandle = NXAUDIOENGINE_INVALID_HANDLE;
}

void NxAudioEngine::cleanup()
{
	_engine.deinit();
}

// SFX
bool NxAudioEngine::canPlaySFX(int id)
{
	struct stat dummy;

	char filename[MAX_PATH];

	getFilenameFullPath<int>(filename, id, NxAudioEngineLayer::NXAUDIOENGINE_SFX);

	return (stat(filename, &dummy) == 0);
}

void NxAudioEngine::loadSFX(int id)
{
	int _curId = id - 1;

	if (_engineInitialized)
	{
		if (_sfxStreams[_curId] == nullptr)
		{
			char filename[MAX_PATH];

			getFilenameFullPath<int>(filename, id, NxAudioEngineLayer::NXAUDIOENGINE_SFX);

			if (trace_all || trace_sfx) trace("NxAudioEngine::%s: %s\n", __func__, filename);

			if (fileExists(filename))
			{
				SoLoud::Wav* sfx = new SoLoud::Wav();

				sfx->load(filename);

				_sfxStreams[_curId] = sfx;
			}
		}
	}
	else
		_sfxStack.push(id);
}

void NxAudioEngine::unloadSFX(int id)
{
	int _curId = id - 1;

	if (_sfxStreams[_curId] != nullptr)
	{
		delete _sfxStreams[_curId];

		_sfxStreams[_curId] = nullptr;
	}
}

void NxAudioEngine::playSFX(int id, int channel, float panning)
{
	int _curId = id - 1;

	std::string _id = std::to_string(id);
	if (auto node = nxAudioEngineConfig[NxAudioEngineLayer::NXAUDIOENGINE_SFX][_id])
	{
		// Shuffle SFX playback, if any entry found for the current id
		toml::array *shuffleIds = node["shuffle"].as_array();
		if (!shuffleIds->empty() && shuffleIds->is_homogeneous(toml::node_type::integer))
		{
			auto _newId = shuffleIds->get(getRandomInt(0, shuffleIds->size() - 1));

			_curId = _newId->value_or(id) - 1;
		}
	}

	if (trace_all || trace_sfx) trace("NxAudioEngine::%s: id=%d,channel=%d,panning:%f\n", __func__, _curId + 1, channel, panning);

	if (_sfxStreams[_curId] != nullptr)
	{
		SoLoud::handle _handle = _engine.play(
			*_sfxStreams[_curId],
			_sfxVolumePerChannels[channel - 1],
			panning
		);

		_sfxChannelsHandle[channel - 1] = _handle;

		_engine.setRelativePlaySpeed(_handle, _sfxTempoPerChannels[channel - 1]);
	}
}

void NxAudioEngine::pauseSFX()
{
	for (auto _handle : _sfxChannelsHandle) _engine.setPause(_handle, true);
}

void NxAudioEngine::resumeSFX()
{
	for (auto _handle : _sfxChannelsHandle) _engine.setPause(_handle, false);
}

void NxAudioEngine::setSFXVolume(float volume, int channel)
{
	_sfxVolumePerChannels[channel - 1] = volume;
}

void NxAudioEngine::setSFXSpeed(float speed, int channel)
{
	_sfxTempoPerChannels[channel - 1] = speed;
}

// Music
bool NxAudioEngine::canPlayMusic(const char* name)
{
	struct stat dummy;

	char filename[MAX_PATH];

	getFilenameFullPath<const char*>(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_MUSIC);

	return (stat(filename, &dummy) == 0);
}

SoLoud::AudioSource* NxAudioEngine::loadMusic(const char* name)
{
	SoLoud::AudioSource* music = nullptr;
	char filename[MAX_PATH];

	getFilenameFullPath<const char*>(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_MUSIC);

	if (trace_all || trace_music) trace("NxAudioEngine::%s: %s\n", __func__, filename);

	if (fileExists(filename))
	{
		if (_openpsf_loaded) {
			SoLoud::OpenPsf* openpsf = new SoLoud::OpenPsf();
			music = openpsf;

			if (openpsf->load(filename) != SoLoud::SO_NO_ERROR) {
				error("NxAudioEngine::%s: Cannot load %s with openpsf\n", __func__, filename);
				delete openpsf;
				music = nullptr;
			}
		}

		if (music == nullptr) {
			SoLoud::VGMStream* vgmstream = new SoLoud::VGMStream();
			music = vgmstream;
			if (vgmstream->load(filename) != SoLoud::SO_NO_ERROR) {
				error("NxAudioEngine::%s: Cannot load %s with vgmstream\n", __func__, filename);
			}
		}
	}

	return music;
}

void NxAudioEngine::playMusic(const char* name, uint32_t id, uint32_t time, PlayFlags flags)
{
	if (!_musicStack.empty() && _musicStack.top().id == id) {
		resumeMusic(time < 20 ? 20 : time, true); // Slight fade
		return;
	}

	if (_engine.isValidVoiceHandle(_music.handle)) {
		if (_music.id == id) {
			if (trace_all || trace_music) trace("NxAudioEngine::%s: %s is already playing\n", __func__, name);
			return; // Already playing
		}

		if (!(flags & PlayFlagsDoNotPause) && _music.isResumable) {
			pauseMusic(time, true);
		}
		else {
			stopMusic(time);
		}
	}

	SoLoud::AudioSource* music = loadMusic(name);

	if (music != nullptr) {
		_music.handle = _engine.playBackground(*music, time > 0 ? 0.0f : 1.0f);
		_music.id = id;
		_music.isResumable = flags & PlayFlagsIsResumable;

		if (time > 0) setMusicVolume(1.0f, time);
	}
}

void NxAudioEngine::playMusics(const std::vector<std::string>& names, uint32_t id, uint32_t time)
{
	if (_music.id == id) {
		if (trace_all || trace_music) trace("NxAudioEngine::%s: id %d is already playing\n", __func__, id);
		return; // Already playing
	}

	stopMusic(time);

	SoLoud::handle groupHandle = _engine.createVoiceGroup();

	if (trace_all || trace_music) trace("NxAudioEngine::%s\n", __func__);

	for (const std::string &name: names) {
		SoLoud::AudioSource* music = loadMusic(name.c_str());
		if (music != nullptr) {
			SoLoud::handle musicHandle = _engine.playBackground(*music, 1.0f, true);
			_musicSegmentsHandle.push_back(musicHandle);
			_engine.addVoiceToGroup(groupHandle, musicHandle);
		}
	}

	if (!_engine.isVoiceGroupEmpty(groupHandle)) {
		_music.handle = NXAUDIOENGINE_INVALID_HANDLE;
		_music.id = id;
		_music.isResumable = false;

		_engine.setProtectVoice(groupHandle, true);
		// Play synchronously
		_engine.setPause(groupHandle, false);
	}

	_engine.destroyVoiceGroup(groupHandle);
}

void NxAudioEngine::stopMusic(uint32_t time)
{
	if (trace_all || trace_music) trace("NxAudioEngine::%s: midi %d, time %d\n", __func__, _music.id, time);

	if (!_musicSegmentsHandle.empty())
	{
		for (SoLoud::handle handle : _musicSegmentsHandle) _engine.stop(handle);
		_musicSegmentsHandle.clear();
		return;
	}

	if (!_engine.isValidVoiceHandle(_music.handle)) return;

	if (time > 0)
	{
		_engine.fadeVolume(_music.handle, 0, time);
		_engine.scheduleStop(_music.handle, time);
	}
	else
	{
		_engine.stop(_music.handle);
	}
}

void NxAudioEngine::pauseMusic(uint32_t time, bool push)
{
	if (trace_all || trace_music) trace("NxAudioEngine::%s: midi %d, time %d\n", __func__, _music.id, time);

	if (!_musicSegmentsHandle.empty())
	{
		for (SoLoud::handle handle : _musicSegmentsHandle) _engine.setPause(handle, true);
		return;
	}

	if (!_engine.isValidVoiceHandle(_music.handle)) return;

	if (time > 0)
	{
		_engine.fadeVolume(_music.handle, 0, time);
		_engine.schedulePause(_music.handle, time);
	}
	else
	{
		_engine.setPause(_music.handle, true);
	}

	if (push) {
		if (trace_all || trace_music) trace("NxAudioEngine::%s: push music onto the stack for later usage\n", __func__);

		// Save for later usage
		_musicStack.push(_music);

		// Invalidate the current handle
		_music.handle = NXAUDIOENGINE_INVALID_HANDLE;
	}
}

void NxAudioEngine::resumeMusic(uint32_t time, bool pop)
{
	if (trace_all || trace_music) trace("NxAudioEngine::%s: midi %d, time %d\n", __func__, _music.id, time);

	if (!_musicSegmentsHandle.empty())
	{
		for (SoLoud::handle handle : _musicSegmentsHandle) _engine.setPause(handle, false);
		return;
	}

	if (pop) {
		// Whatever is currently playing, just stop it
		// If the handle is still invalid, nothing will happen
		_engine.stop(_music.handle);

		// Restore the last known paused music
		_music = _musicStack.top();
		_musicStack.pop();
	}

	// Play it again from where it was left off
	resetMusicVolume(time);
	_engine.setPause(_music.handle, false);
}

bool NxAudioEngine::isMusicPlaying()
{
	return _engine.isValidVoiceHandle(_music.handle) && !_engine.getPause(_music.handle);
}

uint32_t NxAudioEngine::currentMusicId()
{
	return _music.id;
}

void NxAudioEngine::setMusicMasterVolume(float volume, size_t time)
{
	_previousMusicMasterVolume = _musicMasterVolume;

	_musicMasterVolume = volume;

	resetMusicVolume(time);
}

void NxAudioEngine::restoreMusicMasterVolume(size_t time)
{
	if (_previousMusicMasterVolume != _musicMasterVolume)
	{
		_musicMasterVolume = _previousMusicMasterVolume;

		// Set them equally so if this API is called again, nothing will happen
		_previousMusicMasterVolume = _musicMasterVolume;

		resetMusicVolume(time);
	}
}

float NxAudioEngine::getMusicVolume()
{
	return _engine.getVolume(_music.handle);
}

void NxAudioEngine::setMusicVolume(float volume, size_t time)
{
	_wantedMusicVolume = volume;

	resetMusicVolume(time);
}

void NxAudioEngine::resetMusicVolume(size_t time)
{
	float volume = _wantedMusicVolume * _musicMasterVolume;

	if (!_musicSegmentsHandle.empty())
	{
		for (SoLoud::handle handle : _musicSegmentsHandle) _engine.setVolume(handle, volume);
		return;
	}

	if (time > 0)
		_engine.fadeVolume(_music.handle, volume, time);
	else
		_engine.setVolume(_music.handle, volume);
}

void NxAudioEngine::setMusicSpeed(float speed)
{
	if (!_musicSegmentsHandle.empty())
	{
		for (SoLoud::handle handle : _musicSegmentsHandle) _engine.setRelativePlaySpeed(handle, speed);
		return;
	}

	_engine.setRelativePlaySpeed(_music.handle, speed);
}

void NxAudioEngine::setMusicLooping(bool looping)
{
	if (!_musicSegmentsHandle.empty())
	{
		for (SoLoud::handle handle : _musicSegmentsHandle) _engine.setLooping(handle, looping);
		return;
	}

	_engine.setLooping(_music.handle, looping);
}

// Voice
bool NxAudioEngine::canPlayVoice(const char* name)
{
	struct stat dummy;

	char filename[MAX_PATH];

	getFilenameFullPath<const char*>(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_VOICE);

	return (stat(filename, &dummy) == 0);
}

void NxAudioEngine::playVoice(const char* name)
{
	char filename[MAX_PATH];

	getFilenameFullPath<const char *>(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_VOICE);

	if (trace_all || trace_voice) trace("NxAudioEngine::%s: %s\n", __func__, filename);

	if (fileExists(filename))
	{
		SoLoud::WavStream* voice = new SoLoud::WavStream();

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
