/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
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

#include <windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <dsound.h>
#include <process.h>

#include "../log.h"
#include "../music.h"

#include "plugin.h"
#include "out_directsound.h"

void winamp_music_init();
bool winamp_can_play(char* midi);
void winamp_play_music(char* midi, uint id);
void winamp_stop_music();
void winamp_cross_fade_music(char* midi, uint id, int time);
void winamp_pause_music();
void winamp_resume_music();
bool winamp_music_status();
void winamp_set_direct_volume(int volume);
void winamp_set_master_music_volume(int volume);
void winamp_set_music_volume(int volume);
void winamp_set_music_volume_trans(int volume, int frames);
void winamp_set_music_tempo(unsigned char tempo);
void winamp_remember_playing_time();
void winamp_music_cleanup();
