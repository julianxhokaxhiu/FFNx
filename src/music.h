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

#include "types.h"

void music_init();
uint midi_init(uint unknown);
uint ff7_directsound_release();
void music_cleanup();
uint ff7_use_midi(uint midi);
void ff7_play_midi(uint midi);
uint ff8_play_midi(uint midi, uint volume, uint u1, uint u2);
void cross_fade_midi(uint midi, uint time);
void pause_midi();
void restart_midi();
void stop_midi();
uint ff8_stop_midi();
uint midi_status();
uint ff8_set_direct_volume(int volume);
void set_master_midi_volume(uint volume);
void set_midi_volume(uint volume);
void set_midi_volume_trans(uint volume, uint step);
void set_midi_tempo(unsigned char tempo);
uint remember_playing_time();
uint music_sound_operation_fix(uint type, uint param1, uint param2, uint param3, uint param4, uint param5);
bool needs_resume(uint old_mode, uint new_mode, char* old_midi, char* new_midi);
uint opcode_gameover_music_fix();
