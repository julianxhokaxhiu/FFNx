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

#include <stdint.h>

void music_init();
uint32_t midi_init(uint32_t unknown);
uint32_t ff7_directsound_release();
void music_cleanup();
uint32_t ff7_use_midi(uint32_t midi);
void ff7_play_midi(uint32_t midi);
uint32_t ff8_play_midi(uint32_t midi, uint32_t volume, uint32_t u1, uint32_t u2);
void cross_fade_midi(uint32_t midi, uint32_t time);
void pause_midi();
void restart_midi();
void stop_midi();
uint32_t ff8_stop_midi();
uint32_t midi_status();
uint32_t ff8_set_direct_volume(int volume);
void set_master_midi_volume(uint32_t volume);
void set_midi_volume(uint32_t volume);
void set_midi_volume_trans(uint32_t volume, uint32_t step);
void set_midi_tempo(unsigned char tempo);
uint32_t remember_playing_time();
uint32_t music_sound_operation_fix(uint32_t type, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4, uint32_t param5);
bool needs_resume(uint32_t old_mode, uint32_t new_mode, char* old_midi, char* new_midi);
int engine_create_dsound(void* unk, LPGUID guid);
