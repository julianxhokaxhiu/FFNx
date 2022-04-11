/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include "audio/openpsf/openpsf.h"

#include "audio.h"

#include "log.h"
#include "gamehacks.h"
#include "utils.h"

#if defined(__cplusplus)
extern "C" {
#endif

#include <libvgmstream/util/log.h>

#if defined(__cplusplus)
}
#endif

NxAudioEngine nxAudioEngine;

// PRIVATE

void NxAudioEngine::loadConfig()
{
	char _fullpath[MAX_PATH];

	for (int idx = NxAudioEngineLayer::NXAUDIOENGINE_SFX; idx <= NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT; idx++)
	{
		NxAudioEngineLayer type = NxAudioEngineLayer(idx);

		switch (type)
		{
		case NxAudioEngineLayer::NXAUDIOENGINE_SFX:
			sprintf(_fullpath, "%s/%s/config.toml", basedir, external_sfx_path.c_str());
			if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s: %s\n", __func__, _fullpath);
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_MUSIC:
			sprintf(_fullpath, "%s/%s/config.toml", basedir, external_music_path.c_str());
			if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: %s\n", __func__, _fullpath);
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_VOICE:
			sprintf(_fullpath, "%s/%s/config.toml", basedir, external_voice_path.c_str());
			if (trace_all || trace_voice) ffnx_trace("NxAudioEngine::%s: %s\n", __func__, _fullpath);
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT:
			sprintf(_fullpath, "%s/%s/config.toml", basedir, external_ambient_path.c_str());
			if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: %s\n", __func__, _fullpath);
			break;
		}

		if (fileExists(_fullpath)) {
			try
			{
				nxAudioEngineConfig[type] = toml::parse_file(_fullpath);
			}
			catch (const toml::parse_error &err)
			{
				ffnx_warning("Parse error while opening the file %s. Will continue with the default settings.\n", _fullpath);
				ffnx_warning("%s (Line %u Column %u)\n", err.what(), err.source().begin.line, err.source().begin.column);

				nxAudioEngineConfig[type] = toml::v3::ex::parse_result();
			}
		} else {
			nxAudioEngineConfig[type] = toml::v3::ex::parse_result();
		}
	}
}

bool NxAudioEngine::getFilenameFullPath(char *_out, const char* _key, NxAudioEngineLayer _type)
{
	std::vector<std::string> extensions;

	switch(_type)
	{
		case NxAudioEngineLayer::NXAUDIOENGINE_SFX:
			extensions = external_sfx_ext;
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_MUSIC:
			extensions = external_music_ext;
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_VOICE:
			extensions = external_voice_ext;
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT:
			extensions = external_ambient_ext;
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_MOVIE_AUDIO:
			extensions = external_movie_audio_ext;
			break;
	}

	for (const std::string &extension: extensions) {
		switch (_type)
		{
		case NxAudioEngineLayer::NXAUDIOENGINE_SFX:
			sprintf(_out, "%s/%s/%s.%s", basedir, external_sfx_path.c_str(), _key, extension.c_str());
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_MUSIC:
			sprintf(_out, "%s/%s/%s.%s", basedir, external_music_path.c_str(), _key, extension.c_str());
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_VOICE:
			sprintf(_out, "%s/%s/%s.%s", basedir, external_voice_path.c_str(), _key, extension.c_str());
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT:
			sprintf(_out, "%s/%s/%s.%s", basedir, external_ambient_path.c_str(), _key, extension.c_str());
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_MOVIE_AUDIO:
			sprintf(_out, "%s.%s", _key, extension.c_str());
			break;
		}

		if (fileExists(_out)) {
			return true;
		}
	}

	return false;
}

bool NxAudioEngine::fileExists(const char* filename)
{
	bool ret = ::fileExists(filename);

	if (!ret && (trace_all || trace_music || trace_sfx || trace_voice || trace_ambient))
		ffnx_warning("NxAudioEngine::%s: Could not find file %s\n", __func__, filename);

	return ret;
}

// PUBLIC

bool NxAudioEngine::init()
{
	if (_engine.init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::AUTO, external_audio_sample_rate, SoLoud::Soloud::AUTO, external_audio_number_of_channels) == 0)
	{
		_engineInitialized = true;

		ffnx_info("NxAudioEngine initialized: channels=%u,sample_rate=%u\n", _engine.getBackendChannels(), _engine.getBackendSamplerate());

		// 100 -> LOG_LEVEL_ALL: https://github.com/vgmstream/vgmstream/blob/4cda04d02595b381dc8cf98ec39e771c80987d18/src/util/log.c#L20
		if (trace_all || trace_ambient || trace_sfx || trace_music || trace_voice) vgm_log_set_callback(NULL, 100, 0, NxAudioEngineVgmstreamCallback);

		loadConfig();

		if (!he_bios_path.empty()) {
			char fullHeBiosPath[MAX_PATH];
			sprintf(fullHeBiosPath, "%s/%s", basedir, he_bios_path.c_str());

			if (!Psf::initialize_psx_core(fullHeBiosPath)) {
				ffnx_error("NxAudioEngine::%s couldn't load %s, please verify 'he_bios_path' or comment it\n", __func__, he_bios_path.c_str());
			}
			else {
				_openpsf_loaded = true;
				ffnx_info("NxAudioEngine::%s OpenPSF music plugin loaded using %s\n", __func__, he_bios_path.c_str());
			}
		}

		for (int channel = 0; channel < _sfxTotalChannels; channel++) _sfxChannels[channel] = NxAudioEngineSFX();

		return true;
	}

	return false;
}

void NxAudioEngine::flush()
{
	_engine.stopAll();

	for (int channel = 0; channel < 2; channel++)
	{
		_musics[channel] = NxAudioEngineMusic();
	}
	_musicStack = std::stack<NxAudioEngineMusic>();

	for (int channel = 0; channel < _sfxTotalChannels; channel++)
	{
		_sfxChannels[channel] = NxAudioEngineSFX();
	}

	for (int slot = 0; slot < _voiceMaxSlots; slot++)
	{
		_currentVoice[slot] = NxAudioEngineVoice();
	}

	_currentAmbient = NxAudioEngineAmbient();

	for (int slot = 0; slot < _movieAudioMaxSlots; slot++)
	{
		_currentMovieAudio[slot] = NxAudioEngineMovieAudio();
	}

	_currentStream = NxAudioEngineStreamAudio();
}

void NxAudioEngine::cleanup()
{
	_engine.deinit();
}

// SFX
SoLoud::VGMStream* NxAudioEngine::loadSFX(std::string id, bool loop)
{
	if (_engineInitialized)
	{
		char filename[MAX_PATH];

		bool exists = getFilenameFullPath(filename, id.c_str(), NxAudioEngineLayer::NXAUDIOENGINE_SFX);

		if (exists)
		{
			auto node = nxAudioEngineConfig[NxAudioEngineLayer::NXAUDIOENGINE_SFX][id];

			if (node)
			{
				int shouldLoop = node["loop"].value_or(-1);

				// Force loop if requested in the config
				if (shouldLoop != -1) loop = shouldLoop;
			}

			if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s: filename=%s,loop=%d\n", __func__, filename, loop);

			SoLoud::VGMStream* sfx = new SoLoud::VGMStream();

			sfx->setLooping(loop);

			SoLoud::result res = sfx->load(filename);
			if (res != SoLoud::SO_NO_ERROR) {
				ffnx_error("NxAudioEngine::%s: Cannot load %s with vgmstream ( SoLoud error: %u )\n", __func__, filename, res);
				delete sfx;
				return nullptr;
			}

			return sfx;
		}
	}

	return nullptr;
}

int NxAudioEngine::getSFXIdFromChannel(int channel)
{
	return _sfxChannels[channel].game_id;
}

void NxAudioEngine::unloadSFX(int id)
{
	if (_sfxEffectsHandler.count(id) > 0)
	{
		if (_sfxEffectsHandler[id] != nullptr)
		{
			delete _sfxEffectsHandler[id];

			_sfxEffectsHandler.erase(id);
		}
	}
}

void NxAudioEngine::unloadSFXChannel(int channel)
{
	if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s: channel=%d\n", __func__, channel);

	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	if (options->stream != nullptr)
	{
		delete options->stream;

		options->stream = nullptr;
	}
}

bool NxAudioEngine::playSFX(const char* name, int id, int channel, float panning, bool loop, float volume)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];
	int _curId = id;
	bool skipPlay = false;
	std::string _id(name);

	// If channel is known to be reusable
	if (channel <= _sfxReusableChannels)
	{
		// Stop the current channel is already used and the track to be played is different that the one currently playing
		if (options->stream != nullptr && options->id != id)
		{
			stopSFX(channel);
			unloadSFXChannel(channel);
		}
		// If the engine is asking us to play again the same id on the same channel, and is a looping effect...
		else if (options->stream != nullptr && options->id == id && loop)
		{
			// ...simply skip it since it's already playing
			skipPlay = true;
		}
	}
	// If channel is known to lazy unload what is currently playing, save the handler for later
	else if (std::find(_sfxLazyUnloadChannels.begin(), _sfxLazyUnloadChannels.end(), channel) != _sfxLazyUnloadChannels.end())
	{
		_sfxEffectsHandler[options->game_id] = options->stream;

		// invalidate the old channel stream in order to continue loading this new ID
		options->stream = nullptr;
	}
	// Otherwise just unload the channel and allow to load a new track immediately
	else if (channel <= _sfxTotalChannels)
	{
		unloadSFXChannel(channel);
	}

	// Reset state
	options->volume = volume;

	auto node = nxAudioEngineConfig[NxAudioEngineLayer::NXAUDIOENGINE_SFX][name];
	if (node)
	{
		// Shuffle SFX playback, if any entry found for the current id
		toml::array *shuffleIds = node["shuffle"].as_array();
		if (shuffleIds && !shuffleIds->empty() && shuffleIds->is_homogeneous(toml::node_type::integer))
		{
			auto _newId = shuffleIds->get(getRandomInt(0, shuffleIds->size() - 1));

			_curId = _newId->value_or(id);
			_id = std::to_string(_curId);
		}

		// Sequentially playback new SFX ids, if any entry found for the current id
		toml::array *sequentialIds = node["sequential"].as_array();
		if (sequentialIds && !sequentialIds->empty() && sequentialIds->is_homogeneous(toml::node_type::integer))
		{
			if (_sfxSequentialIndexes.find(name) == _sfxSequentialIndexes.end() || _sfxSequentialIndexes[name] >= sequentialIds->size())
				_sfxSequentialIndexes[name] = 0;

			auto _newId = sequentialIds->get(_sfxSequentialIndexes[name]);

			_sfxSequentialIndexes[name]++;

			_curId = _newId->value_or(id);
			_id = std::to_string(_curId);
		}

		// Should we skip playing the track?
		toml::node *shouldSkip = node["skip"].as_boolean();
		if (shouldSkip && shouldSkip->is_boolean()) {
			skipPlay = shouldSkip->value_or(false);
		}
	}

	// Try to load the new ID if it's not already cached
	if (options->stream == nullptr)
	{
		options->game_id = id;
		options->id = _curId;
		// Avoid loading a stream if it is meant to be skipped
		options->stream = skipPlay ? nullptr : loadSFX(_id, loop);
	}

	if (skipPlay)
	{
		// Make the game think everything went fine but instead just be silent
		return true;
	}

	if (external_sfx_always_centered)
	{
		panning = 0.0f;

		if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s: panning overridden because of external_sfx_always_centered\n", __func__);
	}

	if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s: name=%s,id=%d,channel=%d,panning=%f,volume=%f\n", __func__, name, options->id, channel, panning, options->volume);

	if (options->stream != nullptr)
	{
		options->handle = _engine.play(
			*options->stream,
			options->volume * getSFXMasterVolume(),
			panning
		);

		options->loop = _engine.getLooping(options->handle);

		return true;
	}

	return false;
}

void NxAudioEngine::stopSFX(int channel, double time)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s channel=%d\n", __func__, channel);

	if (time > 0.0)
	{
		_engine.fadeVolume(options->handle, 0.0f, time);
		_engine.scheduleStop(options->handle, time);
	}
	else
	{
		_engine.stop(options->handle);
	}

	options->id = 0;
	options->loop = false;
	options->volume = 1.0f;
}

void NxAudioEngine::pauseSFX(int channel)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s channel=%d\n", __func__, channel);

	_engine.setPause(options->handle, true);
}

void NxAudioEngine::resumeSFX(int channel)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s channel=%d\n", __func__, channel);

	_engine.setPause(options->handle, false);
}

bool NxAudioEngine::isSFXPlaying(int channel)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	return _engine.isValidVoiceHandle(options->handle) && !_engine.getPause(options->handle);
}

float NxAudioEngine::getSFXMasterVolume()
{
	return _sfxMasterVolume < 0.0f ? 1.0f : _sfxMasterVolume;
}

void NxAudioEngine::setSFXMasterVolume(float volume, double time)
{
	_sfxMasterVolume = volume;
}

void NxAudioEngine::setSFXVolume(int channel, float volume, double time)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	options->volume = volume;

	if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s: channel=%d,volume=%f\n", __func__, channel, volume);

	if (time > 0.0) {
		time /= gamehacks.getCurrentSpeedhack();
		_engine.fadeVolume(options->handle, volume * getSFXMasterVolume(), time);
	}
	else {
		_engine.setVolume(options->handle, volume * getSFXMasterVolume());
	}
}

void NxAudioEngine::setSFXSpeed(int channel, float speed, double time)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	if (time > 0.0) {
		time /= gamehacks.getCurrentSpeedhack();
		_engine.fadeRelativePlaySpeed(options->handle, speed, time);
	}
	else {
		_engine.setRelativePlaySpeed(options->handle, speed);
	}
}

void NxAudioEngine::setSFXPanning(int channel, float panning, double time)
{
	NxAudioEngineSFX *options = &_sfxChannels[channel - 1];

	if (external_sfx_always_centered)
	{
		panning = 0.0f;

		if (trace_all || trace_sfx) ffnx_trace("NxAudioEngine::%s: panning overridden because of external_sfx_always_centered\n", __func__);
	}

	if (time > 0.0) {
		time /= gamehacks.getCurrentSpeedhack();
		_engine.fadePan(options->handle, panning, time);
	}
	else {
		_engine.setPan(options->handle, panning);
	}
}

void NxAudioEngine::setSFXReusableChannels(short num)
{
	_sfxReusableChannels = num;
}

void NxAudioEngine::setSFXTotalChannels(short num)
{
	_sfxTotalChannels = num;
}

void NxAudioEngine::addSFXLazyUnloadChannel(int channel)
{
	_sfxLazyUnloadChannels.push_back(channel);
}

// Music
bool NxAudioEngine::canPlayMusic(const char* name)
{
	char filename[MAX_PATH];

	return getFilenameFullPath(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_MUSIC);
}

bool NxAudioEngine::isMusicDisabled(const char* name)
{
	char lowercaseName[MAX_PATH];

	// Name to lower case
	for (int i = 0; name[i]; i++) {
		lowercaseName[i] = tolower(name[i]);
	}

	toml::table config = nxAudioEngineConfig[NXAUDIOENGINE_MUSIC];
	std::optional<bool> disabled = config[lowercaseName]["disabled"].value<bool>();

	return disabled.has_value() && disabled;
}

void NxAudioEngine::cleanOldAudioSources()
{
	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: %d elements in the list before cleaning\n", __func__, _audioSourcesToDeleteLater.size());

	std::list<NxAudioEngineMusicAudioSource>::iterator it = _audioSourcesToDeleteLater.begin();
	while (it != _audioSourcesToDeleteLater.end()) {
		if (!_engine.isValidVoiceHandle((*it).handle) && !_engine.isVoiceGroup((*it).handle)) {
			delete (*it).audioSource;
			it = _audioSourcesToDeleteLater.erase(it);
		}
		else {
			++it;
		}
	}

	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: %d elements in the list after cleaning\n", __func__, _audioSourcesToDeleteLater.size());
}

SoLoud::AudioSource* NxAudioEngine::loadMusic(const char* name, bool isFullPath, const char* format, bool suppressOpeningSilence)
{
	SoLoud::AudioSource* music = nullptr;
	char filename[MAX_PATH];
	bool exists = false;

	if (isFullPath)
	{
		exists = true;
		strcpy(filename, name);
	}

	if (!exists)
	{
		exists = getFilenameFullPath(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_MUSIC);
	}

	if (exists)
	{
		if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: %s\n", __func__, filename);

		cleanOldAudioSources();

		if (_openpsf_loaded && SoLoud::OpenPsf::is_our_path(filename)) {
			SoLoud::OpenPsf* openpsf = new SoLoud::OpenPsf();
			music = openpsf;

			SoLoud::result res = openpsf->load(filename, suppressOpeningSilence);
			if (res != SoLoud::SO_NO_ERROR) {
				ffnx_error("NxAudioEngine::%s: Cannot load %s with openpsf ( SoLoud error: %u )\n", __func__, filename, res);
				delete openpsf;
				music = nullptr;
			}
		}

		if (music == nullptr) {
			SoLoud::VGMStream* vgmstream = new SoLoud::VGMStream();
			music = vgmstream;

			SoLoud::result res = vgmstream->load(filename, format);
			if (res != SoLoud::SO_NO_ERROR) {
				ffnx_error("NxAudioEngine::%s: Cannot load %s with vgmstream ( SoLoud error: %u )\n", __func__, filename, res);
				delete vgmstream;
				music = nullptr;
			}
		}
	}

	return music;
}

void NxAudioEngine::overloadPlayArgumentsFromConfig(char* name, uint32_t* id, MusicOptions* musicOptions)
{
	// Name to lower case
	for (int i = 0; name[i]; i++) {
		name[i] = tolower(name[i]);
	}

	toml::table config = nxAudioEngineConfig[NXAUDIOENGINE_MUSIC];
	std::optional<SoLoud::time> offset_seconds_opt = config[name]["offset_seconds"].value<SoLoud::time>();
	std::optional<std::string> no_intro_track_opt = config[name]["no_intro_track"].value<std::string>();
	std::optional<SoLoud::time> intro_seconds_opt = config[name]["intro_seconds"].value<SoLoud::time>();
	std::optional<float> relative_speed_opt = config[name]["relative_speed"].value<float>();

	if (offset_seconds_opt.has_value()) {
		musicOptions->offsetSeconds = *offset_seconds_opt;
	} else {
		std::optional<std::string> offset_special_opt = config[name]["offset_seconds"].value<std::string>();

		if (offset_special_opt.has_value() && offset_special_opt->compare("sync") == 0) {
			musicOptions->sync = true;
		}
	}

	if (musicOptions->noIntro) {
		if (no_intro_track_opt.has_value()) {
			std::string no_intro_track = *no_intro_track_opt;
			if (trace_all || trace_music) ffnx_info("%s: replaced by no intro track %s\n", __func__, no_intro_track.c_str());

			if (!no_intro_track.empty()) {
				memcpy(name, no_intro_track.c_str(), no_intro_track.size());
				name[no_intro_track.size()] = '\0';
			}
		}
		else if (intro_seconds_opt.has_value()) {
			musicOptions->offsetSeconds = *intro_seconds_opt;
		}
		else {
			ffnx_info("%s: cannot play no intro track, please configure it in %s/config.toml\n", __func__, external_music_path.c_str());
		}
	}

	if (relative_speed_opt.has_value() && *relative_speed_opt > 0.0f) {
		musicOptions->relativeSpeed = *relative_speed_opt;
	}

	// Shuffle Music playback, if any entry found for the current music name
	toml::array* shuffleNames = config[name]["shuffle"].as_array();
	if (shuffleNames && !shuffleNames->empty() && shuffleNames->is_homogeneous(toml::node_type::string)) {
		std::optional<std::string> _newName = shuffleNames->get(getRandomInt(0, shuffleNames->size() - 1))->value<std::string>();
		if (_newName.has_value()) {
			memcpy(name, (*_newName).c_str(), (*_newName).size());
			name[(*_newName).size()] = '\0';

			if (trace_all || trace_music) ffnx_info("%s: replaced by shuffle with %s\n", __func__, (*_newName).c_str());
		}
	}
}

bool NxAudioEngine::playMusic(const char* name, uint32_t id, int channel, MusicOptions options)
{
	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: %s (%d) on channel #%d\n", __func__, name, id, channel);

	char overloadedName[MAX_PATH];

	strncpy(overloadedName, name, MAX_PATH);

	if (!options.useNameAsFullPath) {
		overloadPlayArgumentsFromConfig(overloadedName, &id, &options);
	}
	// Same music is already playing on this channel
	if (isMusicPlaying(channel) && currentMusicId(channel) == id) {
		if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: %s is already playing on channel %d\n", __func__, overloadedName, channel);

		return false;
	}
	// Same music is paused on this channel or in backup channel
	bool restore = !_musicStack.empty() && _musicStack.top().id == id;
	if ((isChannelValid(channel) && currentMusicId(channel) == id) || restore) {
		resumeMusic(channel, options.fadetime == 0.0 ? 1.0 : options.fadetime, restore); // Slight fade

		return true;
	}

	SoLoud::AudioSource* audioSource = loadMusic(overloadedName, options.useNameAsFullPath, options.format, options.suppressOpeningSilence);

	if (audioSource != nullptr) {
		// Different music is playing on this channel
		if (isChannelValid(channel)) {
			stopMusic(channel, options.fadetime == 0.0 ? 0.2 : options.fadetime);
		}

		NxAudioEngineMusic& music = _musics[channel];
		SoLoud::time offsetSeconds = options.sync ? _musics[!channel].lastMusicOffset : options.offsetSeconds;

		if (options.targetVolume >= 0.0f) {
			music.wantedMusicVolume = options.targetVolume;
		}
		const float initialVolume = options.fadetime > 0.0 || offsetSeconds > 0.0 ? 0.0f : music.wantedMusicVolume * _musicMasterVolume;
		music.handle = _engine.playBackground(*audioSource, initialVolume, offsetSeconds > 0.0);
		music.id = id;
		music.sync = options.sync;

		// Keep audioSource pointer somewhere to delete it after musicHandle is stopped
		_audioSourcesToDeleteLater.push_back(NxAudioEngineMusicAudioSource(music.handle, audioSource));

		if (offsetSeconds > 0.0) {
			if (trace_all || trace_music) ffnx_info("NxAudioEngine::%s: seek to time %fs\n", __func__, offsetSeconds);
			_engine.seek(music.handle, offsetSeconds);
			resumeMusic(channel, options.fadetime == 0.0 ? 1.0 : options.fadetime); // Slight fade
		}
		else if (options.fadetime > 0.0) {
			setMusicVolume(music.wantedMusicVolume, channel, options.fadetime);
		}

		if (options.relativeSpeed > 0.0f && options.relativeSpeed != 1.0f) {
			setMusicSpeed(options.relativeSpeed, channel);
		}

		return true;
	}

	return false;
}

void NxAudioEngine::playSynchronizedMusics(const std::vector<std::string>& names, uint32_t id, MusicOptions options)
{
	const int channel = 0;

	if (_musics[channel].id == id) {
		if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: id %d is already playing\n", __func__, id);
		return; // Already playing
	}

	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: id %d\n", __func__, id);

	stopMusic(options.fadetime);

	SoLoud::handle groupHandle = _engine.createVoiceGroup();

	if (groupHandle == 0) {
		ffnx_error("NxAudioEngine::%s: cannot allocate voice group\n", __func__);
		return;
	}

	for (const std::string &name: names) {
		SoLoud::AudioSource* audioSource = loadMusic(name.c_str());

		if (audioSource != nullptr) {
			SoLoud::handle musicHandle = _engine.playBackground(*audioSource, _musicMasterVolume, true);
			_engine.addVoiceToGroup(groupHandle, musicHandle);
			// Keep audioSource pointer somewhere to delete it after musicHandle is stopped
			_audioSourcesToDeleteLater.push_back(NxAudioEngineMusicAudioSource(musicHandle, audioSource));
		}
	}

	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: handle=%X\n", __func__, groupHandle);

	if (!_engine.isVoiceGroupEmpty(groupHandle)) {
		_musics[channel].handle = groupHandle;
		_musics[channel].id = id;
		// Play synchronously
		_engine.setPause(groupHandle, false);
	}
	else {
		_engine.destroyVoiceGroup(groupHandle);
	}
}

/**
 * Put music on top of the music stack for backup/restore feature
 */
void NxAudioEngine::prioritizeMusicRestore(uint32_t id)
{
	std::stack<NxAudioEngineMusic> removedElements;
	NxAudioEngineMusic foundElement = NxAudioEngineMusic();

	// Search for id
	while (! _musicStack.empty()) {
		const NxAudioEngineMusic &backup = _musicStack.top();

		if (backup.id == id) {
			foundElement = backup;
			break;
		}

		removedElements.push(backup);

		_musicStack.pop();
	}

	// Rebuild stack
	while (!removedElements.empty()) {
		_musicStack.push(removedElements.top());
		removedElements.pop();
	}

	// Push music on top
	if (foundElement.id == id) {
		if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: found midi %d\n", __func__, id);

		_musicStack.push(foundElement);
	} else {
		if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: midi %d not found\n", __func__, id);
	}
}

void NxAudioEngine::stopMusic(double time)
{
	for (int channel = 0; channel < 2; ++channel) {
		stopMusic(channel, time);
	}
}

void NxAudioEngine::stopMusic(int channel, double time)
{
	NxAudioEngineMusic& music = _musics[channel];

	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: channel %d, midi %d, time %f\n", __func__, channel, music.id, time);

	if (external_music_sync) {
		music.lastMusicOffset = _engine.getStreamTime(_musics[channel].handle);
	}

	if (time > 0.0)
	{
		time /= gamehacks.getCurrentSpeedhack();
		_engine.fadeVolume(music.handle, 0.0f, time);
		_engine.scheduleStop(music.handle, time);
		_lastVolumeFadeEndTime = _engine.mStreamTime + time;
	}
	else
	{
		_engine.stop(music.handle);
	}

	if (_engine.isVoiceGroup(music.handle)) {
		_engine.destroyVoiceGroup(music.handle);
	}

	music.invalidate();
}

void NxAudioEngine::pauseMusic(double time)
{
	for (int channel = 0; channel < 2; ++channel) {
		pauseMusic(channel, time);
	}
}

void NxAudioEngine::pauseMusic(int channel, double time, bool backup)
{
	NxAudioEngineMusic& music = _musics[channel];
	bool syncMusic = external_music_sync && backup && music.sync;

	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: midi %d, time %f, sync %d\n", __func__, music.id, time, syncMusic);

	if (external_music_sync) {
		music.lastMusicOffset = _engine.getStreamTime(_musics[channel].handle);
	}

	if (time > 0.0)
	{
		time /= gamehacks.getCurrentSpeedhack();
		_engine.fadeVolume(music.handle, 0.0f, time);
		if (!syncMusic) {
			_engine.schedulePause(music.handle, time);
		}
		_lastVolumeFadeEndTime = _engine.mStreamTime + time;
	}
	else if (syncMusic)
	{
		_engine.setVolume(music.handle, 0.0f);
	}
	else
	{
		_engine.setPause(music.handle, true);
	}

	if (backup) {
		backupMusic(channel);
	}
}

void NxAudioEngine::backupMusic(int channelSource)
{
	if (!isChannelValid(channelSource)) {
		return;
	}

	NxAudioEngineMusic& music = _musics[channelSource];

	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: backup music %d for later usage\n", __func__, music.id);

	NxAudioEngineMusic backup = NxAudioEngineMusic();
	// Save for later usage
	backup.id = music.id;
	backup.handle = music.handle;
	backup.sync = music.sync;

	_musicStack.push(backup);

	// Invalidate the current handle
	music.invalidate();
}

void NxAudioEngine::restoreMusic(int channelDest, double stopTime)
{
	NxAudioEngineMusic& music = _musics[channelDest];

	if (_musicStack.empty()) {
		return;
	}

	stopMusic(channelDest, stopTime);

	const NxAudioEngineMusic &backup = _musicStack.top();

	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: restore music %d\n", __func__, backup.id);

	// Restore
	music.id = backup.id;
	music.handle = backup.handle;
	music.sync = backup.sync;

	_musicStack.pop();
}

void NxAudioEngine::resumeMusic(double time)
{
	for (int channel = 0; channel < 2; ++channel) {
		resumeMusic(channel, time);
	}
}

void NxAudioEngine::resumeMusic(int channel, double time, bool restore)
{
	if (restore) {
		restoreMusic(channel, time);
	}

	NxAudioEngineMusic& music = _musics[channel];

	time /= gamehacks.getCurrentSpeedhack();

	if (trace_all || trace_music) ffnx_trace("NxAudioEngine::%s: midi %d, time %f\n", __func__, music.id, time);

	// Play it again from where it was left off
	if (time > 0.0) {
		_engine.setVolume(music.handle, 0.0);
	}
	resetMusicVolume(channel, time);
	_engine.setPause(music.handle, false);
}

bool NxAudioEngine::isChannelValid(int channel)
{
	const NxAudioEngineMusic& music = _musics[channel];

	return (_engine.isValidVoiceHandle(music.handle)
		|| _engine.isVoiceGroup(music.handle));
}

bool NxAudioEngine::isMusicPlaying()
{
	for (int channel = 0; channel < 2; ++channel) {
		if (isMusicPlaying(channel)) return true;
	}

	return false;
}

bool NxAudioEngine::isMusicPlaying(int channel)
{
	const NxAudioEngineMusic& music = _musics[channel];

	return isChannelValid(channel) && !_engine.getPause(music.handle);
}

uint32_t NxAudioEngine::currentMusicId(int channel)
{
	return isMusicPlaying(channel) ? _musics[channel].id : 0;
}

void NxAudioEngine::setMusicMasterVolume(float volume, double time)
{
	_previousMusicMasterVolume = _musicMasterVolume;

	_musicMasterVolume = volume;

	resetMusicVolume(time);
}

void NxAudioEngine::restoreMusicMasterVolume(double time)
{
	if (_previousMusicMasterVolume != _musicMasterVolume && _previousMusicMasterVolume >= 0.0f)
	{
		_musicMasterVolume = _previousMusicMasterVolume;

		// Set them equally so if this API is called again, nothing will happen
		_previousMusicMasterVolume = _musicMasterVolume;

		resetMusicVolume(time);
	}
}

float NxAudioEngine::getMusicVolume(int channel)
{
	return _musics[channel].wantedMusicVolume;
}

bool NxAudioEngine::isMusicVolumeFadeFinished()
{
	return _engine.mStreamTime >= _lastVolumeFadeEndTime;
}

float NxAudioEngine::getMusicMasterVolume()
{
	return _musicMasterVolume < 0.0f ? 1.0f : _musicMasterVolume;
}

void NxAudioEngine::setMusicVolume(float volume, int channel, double time)
{
	_musics[channel].wantedMusicVolume = volume;

	resetMusicVolume(channel, time);
}

void NxAudioEngine::resetMusicVolume(double time)
{
	for (int channel = 0; channel < 2; ++channel) {
		resetMusicVolume(channel, time);
	}
}

void NxAudioEngine::resetMusicVolume(int channel, double time)
{
	const NxAudioEngineMusic& music = _musics[channel];
	const float volume = music.wantedMusicVolume * getMusicMasterVolume();

	if (time > 0.0) {
		time /= gamehacks.getCurrentSpeedhack();
		_engine.fadeVolume(music.handle, volume, time);
		_lastVolumeFadeEndTime = _engine.mStreamTime + time;
	}
	else {
		_engine.setVolume(music.handle, volume);
	}
}

void NxAudioEngine::setMusicSpeed(float speed, int channel)
{
	_engine.setRelativePlaySpeed(_musics[channel].handle, speed);
}

void NxAudioEngine::setMusicLooping(bool looping, int channel)
{
	_engine.setLooping(_musics[channel].handle, looping);
}

// Voice
bool NxAudioEngine::canPlayVoice(const char* name)
{
	char filename[MAX_PATH];

	return getFilenameFullPath(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_VOICE);
}

bool NxAudioEngine::playVoice(const char* name, int slot, float volume, int game_moment)
{
	char filename[MAX_PATH];

	bool exists = false;

	_currentVoice[slot].volume = volume * getVoiceMasterVolume();

	std::string _name(name);

	// TOML doesn't like the / char as key, replace it with - ( one of the valid accepted chars )
	replaceAll(_name, '/', '-');

	auto node = nxAudioEngineConfig[NxAudioEngineLayer::NXAUDIOENGINE_VOICE][_name];

  // Attempt to load a subnode based on the current game moment
  if (game_moment > -1)
  {
    std::string _gamemoment("gm-" + std::to_string(game_moment));
    auto subnode = node[_gamemoment];
    if (subnode) node = subnode;
  }

	if (node)
	{
		// Set volume for the current track
		toml::node *trackVolume = node["volume"].as_integer();
		if (trackVolume)
		{
			_currentVoice[slot].volume = (trackVolume->value_or(100) / 100.0f) * getVoiceMasterVolume();
		}

		// Shuffle Voice playback, if any entry found for the current id
		toml::array *shuffleNames = node["shuffle"].as_array();
		if (shuffleNames && !shuffleNames->empty() && shuffleNames->is_homogeneous(toml::node_type::string))
		{
			auto _newName = shuffleNames->get(getRandomInt(0, shuffleNames->size() - 1));

			exists = getFilenameFullPath(filename, _newName->value_or(name), NxAudioEngineLayer::NXAUDIOENGINE_VOICE);
		}

		// Sequentially playback new voice items, if any entry found for the current id
		toml::array *sequentialNames = node["sequential"].as_array();
		if (sequentialNames && !sequentialNames->empty() && sequentialNames->is_homogeneous(toml::node_type::string))
		{
			if (_voiceSequentialIndexes.find(name) == _voiceSequentialIndexes.end() || _voiceSequentialIndexes[name] >= sequentialNames->size())
				_voiceSequentialIndexes[name] = 0;

			auto _newName = sequentialNames->get(_voiceSequentialIndexes[name]);

			_voiceSequentialIndexes[name]++;

			exists = getFilenameFullPath(filename, _newName->value_or(name), NxAudioEngineLayer::NXAUDIOENGINE_VOICE);
		}
	}

	// If none of the previous configurations worked, load the default one as last tentative
	if (!exists) {
		exists = getFilenameFullPath(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_VOICE);
	}

	if (trace_all || trace_voice) ffnx_trace("NxAudioEngine::%s: slot[%d] %s exists=%d\n", __func__, slot, filename, exists);

	if (exists)
	{
		// Stop any previously playing voice
		if (_engine.isValidVoiceHandle(_currentVoice[slot].handle))
		{
			_engine.stop(_currentVoice[slot].handle);

			delete _currentVoice[slot].stream;

			_currentVoice[slot].handle = NXAUDIOENGINE_INVALID_HANDLE;
		}

		_currentVoice[slot].stream = new SoLoud::VGMStream();

		SoLoud::result res = _currentVoice[slot].stream->load(filename);
		if (res != SoLoud::SO_NO_ERROR) {
			ffnx_error("NxAudioEngine::%s: Cannot load %s with vgmstream ( SoLoud error: %u )\n", __func__, filename, res);
			delete _currentVoice[slot].stream;
			return false;
		}

		_currentVoice[slot].handle = _engine.play(*_currentVoice[slot].stream, _currentVoice[slot].volume);

		return _engine.isValidVoiceHandle(_currentVoice[slot].handle);
	}
	else
		return false;
}

void NxAudioEngine::stopVoice(int slot, double time)
{
	SoLoud::handle handle = _currentVoice[slot].handle;

	if (trace_all || trace_voice) ffnx_trace("NxAudioEngine::%s: slot=%d time=%lf handle=%X\n", __func__, slot, time, handle);

	if (!_engine.isValidVoiceHandle(handle))
	{
		return;
	}

	if (time > 0.0)
	{
		_engine.fadeVolume(handle, 0, time);
		_engine.scheduleStop(handle, time);
	}
	else
	{
		_engine.stop(handle);
	}
}

void NxAudioEngine::pauseVoice(int slot, double time)
{
	if (time > 0.0)
	{
		_engine.fadeVolume(_currentVoice[slot].handle, 0, time);
		_engine.schedulePause(_currentVoice[slot].handle, time);
	}
	else
	{
		_engine.setPause(_currentVoice[slot].handle, true);
	}
}

void NxAudioEngine::resumeVoice(int slot, double time)
{
	if (time > 0.0)
	{
		_engine.setPause(_currentVoice[slot].handle, false);
		_engine.fadeVolume(_currentVoice[slot].handle, _currentVoice[slot].volume, time);
	}
	else
	{
		_engine.setVolume(_currentVoice[slot].handle, _currentVoice[slot].volume);
		_engine.setPause(_currentVoice[slot].handle, false);
	}
}

bool NxAudioEngine::isVoicePlaying(int slot)
{
	return _engine.isValidVoiceHandle(_currentVoice[slot].handle) && !_engine.getPause(_currentVoice[slot].handle);
}

void NxAudioEngine::setVoiceMaxSlots(int slot)
{
	_voiceMaxSlots = slot;
}

float NxAudioEngine::getVoiceMasterVolume()
{
	return _voiceMasterVolume < 0.0f ? 1.0f : _voiceMasterVolume;
}

void NxAudioEngine::setVoiceMasterVolume(float volume, double time)
{
	_voiceMasterVolume = volume;
}

// Ambient
bool NxAudioEngine::canPlayAmbient(const char* name)
{
	char filename[MAX_PATH];

	return getFilenameFullPath(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT);
}

bool NxAudioEngine::playAmbient(const char* name, float volume, double time)
{
	char filename[MAX_PATH];
	bool exists = false;

	// Reset state
	_currentAmbient.fade_in = 0.0f;
	_currentAmbient.fade_out = 0.0f;
	_currentAmbient.volume = volume * getAmbientMasterVolume();

	auto node = nxAudioEngineConfig[NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT][name];
	if (node)
	{
		// Shuffle Ambient playback, if any entry found for the current id
		toml::array *shuffleIds = node["shuffle"].as_array();
		if (shuffleIds && !shuffleIds->empty() && shuffleIds->is_homogeneous(toml::node_type::string))
		{
			auto _newName = shuffleIds->get(getRandomInt(0, shuffleIds->size() - 1));

			exists = getFilenameFullPath(filename, _newName->value_or(""), NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT);
		}

		// Sequentially playback new Ambient ids, if any entry found for the current id
		toml::array *sequentialIds = node["sequential"].as_array();
		if (sequentialIds && !sequentialIds->empty() && sequentialIds->is_homogeneous(toml::node_type::string))
		{
			// If the key doesn't exist already, add it
			if (_ambientSequentialIndexes.count(name) == 0) _ambientSequentialIndexes[name] = NULL;

			if (_ambientSequentialIndexes.find(name) == _ambientSequentialIndexes.end() || _ambientSequentialIndexes[name] >= sequentialIds->size())
				_ambientSequentialIndexes[name] = 0;

			auto _newName = sequentialIds->get(_ambientSequentialIndexes[name]);

			_ambientSequentialIndexes[name]++;

			exists = getFilenameFullPath(filename, _newName->value_or(""), NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT);
		}

		// Fade In time for this track, if configured
		toml::node *fadeInTime = node["fade_in"].as_floating_point();
		if (fadeInTime)
		{
			_currentAmbient.fade_in = fadeInTime->value_or(0.0f);

			time = _currentAmbient.fade_in;
		}

		// Fade Out time for this track, if configured
		toml::node *fadeOutTime = node["fade_out"].as_floating_point();
		if (fadeOutTime)
		{
			_currentAmbient.fade_out = fadeOutTime->value_or(0.0f);
		}

		// Set volume for the current ambient
		toml::node *ambientVolume = node["volume"].as_integer();
		if (ambientVolume)
		{
			_currentAmbient.volume = (ambientVolume->value_or(100) / 100.0f) * getAmbientMasterVolume();
		}
	}

	// If none of the previous configurations worked, load the default one as last tentative
	if (!exists) {
		exists = getFilenameFullPath(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT);
	}

	if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: %s exists=%d handle=%X\n", __func__, filename, exists, _currentAmbient.handle);

	if (exists)
	{
		// Stop any previously playing ambient
		if (_engine.isValidVoiceHandle(_currentAmbient.handle))
		{
			_engine.stop(_currentAmbient.handle);

			delete _currentAmbient.stream;

			_currentAmbient.handle = NXAUDIOENGINE_INVALID_HANDLE;
		}

		_currentAmbient.stream = new SoLoud::VGMStream();

		SoLoud::result res = _currentAmbient.stream->load(filename);
		if (res != SoLoud::SO_NO_ERROR) {
			ffnx_error("NxAudioEngine::%s: Cannot load %s with vgmstream ( SoLoud error: %u )\n", __func__, filename, res);
			delete _currentAmbient.stream;
			return false;
		}

		_currentAmbient.handle = _engine.play(*_currentAmbient.stream, time > 0.0f ? 0.0f : _currentAmbient.volume, 0.0f, time > 0.0f);

		if (time > 0.0f) resumeAmbient(time);

		return _engine.isValidVoiceHandle(_currentAmbient.handle);
	}
	else
		return false;
}

void NxAudioEngine::stopAmbient(double time)
{
	if (_currentAmbient.fade_out > 0.0f)
	{
		time = _currentAmbient.fade_out;

		if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: time=%f ( overridden through config.toml )\n", __func__, time);
	}
	else if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: time=%f\n", __func__, time);

	if (time > 0.0)
	{
		_engine.fadeVolume(_currentAmbient.handle, 0, time);
		_engine.scheduleStop(_currentAmbient.handle, time);
	}
	else
	{
		_engine.stop(_currentAmbient.handle);
	}
}

void NxAudioEngine::pauseAmbient(double time)
{
	if (_currentAmbient.fade_out > 0.0f)
	{
		time = _currentAmbient.fade_out;

		if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: time=%f ( overridden through config.toml )\n", __func__, time);
	}
	else if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: time=%f\n", __func__, time);

	if (time > 0.0)
	{
		_engine.fadeVolume(_currentAmbient.handle, 0, time);
		_engine.schedulePause(_currentAmbient.handle, time);
	}
	else
	{
		_engine.setPause(_currentAmbient.handle, true);
	}
}

void NxAudioEngine::resumeAmbient(double time)
{
	if (_currentAmbient.fade_in > 0.0f)
	{
		time = _currentAmbient.fade_in;

		if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: time=%f ( overridden through config.toml )\n", __func__, time);
	}
	else if (trace_all || trace_ambient) ffnx_trace("NxAudioEngine::%s: time=%f\n", __func__, time);

	if (time > 0.0)
	{
		_engine.setPause(_currentAmbient.handle, false);
		_engine.fadeVolume(_currentAmbient.handle, _currentAmbient.volume, time);
	}
	else
	{
		_engine.setVolume(_currentAmbient.handle, _currentAmbient.volume);
		_engine.setPause(_currentAmbient.handle, false);
	}
}

bool NxAudioEngine::isAmbientPlaying()
{
	return _engine.isValidVoiceHandle(_currentAmbient.handle) && !_engine.getPause(_currentAmbient.handle);
}

float NxAudioEngine::getAmbientMasterVolume()
{
	return _ambientMasterVolume < 0.0f ? 1.0f : _ambientMasterVolume;
}

void NxAudioEngine::setAmbientMasterVolume(float volume, double time)
{
	_ambientMasterVolume = volume;
}

// Movie Audio
bool NxAudioEngine::canPlayMovieAudio(const char* nameWithPath)
{
	char filename[MAX_PATH];

	return getFilenameFullPath(filename, nameWithPath, NxAudioEngineLayer::NXAUDIOENGINE_MOVIE_AUDIO);
}

bool NxAudioEngine::playMovieAudio(const char* nameWithPath, int slot, float volume)
{
	char filename[MAX_PATH];
	bool exists = getFilenameFullPath(filename, nameWithPath, NxAudioEngineLayer::NXAUDIOENGINE_MOVIE_AUDIO);

	if (trace_all || trace_movies) ffnx_trace("NxAudioEngine::%s: %s exists=%d\n", __func__, filename, exists);

	// Stop any previously playing movie audio
	if (_engine.isValidVoiceHandle(_currentMovieAudio[slot].handle))
	{
		_engine.stop(_currentMovieAudio[slot].handle);

		delete _currentMovieAudio[slot].stream;

		_currentMovieAudio[slot].handle = NXAUDIOENGINE_INVALID_HANDLE;
	}

	if (exists)
	{
		_currentMovieAudio[slot].stream = new SoLoud::VGMStream();

		SoLoud::result res = _currentMovieAudio[slot].stream->load(filename);
		if (res != SoLoud::SO_NO_ERROR) {
			ffnx_error("NxAudioEngine::%s: Cannot load %s with vgmstream ( SoLoud error: %u )\n", __func__, filename, res);
			delete _currentMovieAudio[slot].stream;
			return false;
		}

		_currentMovieAudio[slot].handle = _engine.play(*_currentMovieAudio[slot].stream, volume * getMovieMasterVolume());

		return _engine.isValidVoiceHandle(_currentMovieAudio[slot].handle);
	}
	else
		return false;
}

void NxAudioEngine::stopMovieAudio(int slot)
{
	_engine.stop(_currentMovieAudio[slot].handle);
}

bool NxAudioEngine::isMovieAudioPlaying(int slot)
{
	return _engine.isValidVoiceHandle(_currentMovieAudio[slot].handle);
}

void NxAudioEngine::setMovieAudioMaxSlots(int slot)
{
	_movieAudioMaxSlots = slot;
}

float NxAudioEngine::getMovieMasterVolume()
{
	return _movieMasterVolume < 0.0f ? 1.0f : _movieMasterVolume;
}

void NxAudioEngine::setMovieMasterVolume(float volume, double time)
{
	_movieMasterVolume = volume;
}

// Stream Audio
void NxAudioEngine::initStream(float duration, float sample_rate, uint32_t channels)
{
	if (_currentStream.stream) delete _currentStream.stream;

	_currentStream.stream = new SoLoud::MemoryStream(sample_rate, ::ceil(duration) * sample_rate * channels, channels);
}

void NxAudioEngine::pushStreamData(uint8_t* data, uint32_t size)
{
	if (_currentStream.stream)
	{
		_currentStream.stream->push(data, size);
	}
}

bool NxAudioEngine::playStream(float volume)
{
	_currentStream.volume = volume * getStreamMasterVolume();
	_currentStream.handle = _engine.play(*_currentStream.stream, _currentStream.volume);

	return _engine.isValidVoiceHandle(_currentStream.handle);
}

void NxAudioEngine::stopStream(double time)
{
	if (time > 0.0)
	{
		_engine.fadeVolume(_currentStream.handle, 0, time);
		_engine.schedulePause(_currentStream.handle, time);
	}
	else
	{
		_engine.setPause(_currentStream.handle, true);
	}

	_currentStream.handle = NXAUDIOENGINE_INVALID_HANDLE;
}

bool NxAudioEngine::isStreamPlaying()
{
	return _engine.isValidVoiceHandle(_currentStream.handle);
}

void NxAudioEngine::pauseStream(double time)
{
	if (time > 0.0)
	{
		_engine.fadeVolume(_currentStream.handle, 0, time);
		_engine.schedulePause(_currentStream.handle, time);
	}
	else
	{
		_engine.setPause(_currentStream.handle, true);
	}
}

void NxAudioEngine::resumeStream(double time)
{
	if (time > 0.0)
	{
		_engine.setPause(_currentStream.handle, false);
		_engine.fadeVolume(_currentStream.handle, _currentStream.volume, time);
	}
	else
	{
		_engine.setVolume(_currentStream.handle, _currentStream.volume);
		_engine.setPause(_currentStream.handle, false);
	}
}

float NxAudioEngine::getStreamMasterVolume()
{
	return _streamMasterVolume < 0.0f ? 1.0f : _streamMasterVolume;
}

void NxAudioEngine::setStreamMasterVolume(float volume, double time)
{
	_streamMasterVolume = volume;
}
