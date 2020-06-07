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

#include "types.h"

void sfx_init();
uint sfx_operation_battle_swirl_stop_sound(uint type, uint param1, uint param2, uint param3, uint param4, uint param5);
uint sfx_operation_resume_music(uint type, uint param1, uint param2, uint param3, uint param4, uint param5);
void sfx_menu_force_channel_5_volume(uint volume, uint channel);
void sfx_menu_play_sound_down(uint id);
void sfx_menu_play_sound_up(uint id);
void sfx_update_volume(int modifier);
void sfx_clear_sound_locks();
