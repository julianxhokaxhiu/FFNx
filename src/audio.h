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

#include <vector>
#include <soloud/soloud.h>
#include <soloud/soloud_wavstream.h>

class NxAudioEngine
{
private:
	SoLoud::Soloud _engine;

	// MUSIC
	SoLoud::handle _musicHandle = 0xfffff000; // SoLoud Invalid Handle

	float _musicMasterVolume = 100.0f;

	void getMusicFilenameFullPath(char* _out, char* _name);

	// AUDIO
	SoLoud::handle _voiceHandle = 0xfffff000; // SoLoud Invalid Handle

	float _voiceMasterVolume = 100.0f;

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
	void setMusicVolume(float _volume, size_t time = 0);
	void setMusicSpeed(float speed);

	// Voice
	void playVoice(char* name);
};

extern NxAudioEngine nxAudioEngine;