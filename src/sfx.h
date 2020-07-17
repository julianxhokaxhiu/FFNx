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

#include <math.h>
#include <stdint.h>

void sfx_init();
uint32_t sfx_operation_battle_swirl_stop_sound(uint32_t type, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4, uint32_t param5);
uint32_t sfx_operation_resume_music(uint32_t type, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4, uint32_t param5);
void sfx_menu_force_channel_5_volume(uint32_t volume, uint32_t channel);
void sfx_menu_play_sound_down(uint32_t id);
void sfx_menu_play_sound_up(uint32_t id);
void sfx_clear_sound_locks();
void sfx_fix_volume_values(char* log);
int sfx_play_battle_specific(IDirectSoundBuffer* buffer, uint32_t flags);
uint32_t sfx_fix_omnislash_sound_loading(int sound_id, int dsound_buffer);
