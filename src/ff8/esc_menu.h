/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2026 AxlRose                                            //
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

#include "../ff8.h"

void ff8_esc_menu_init();
void ff8_esc_menu_update(bool esc_down);
bool ff8_esc_menu_is_open();
bool ff8_esc_menu_restarting();
int ff8_esc_menu_pause_screen(int callback);
ff8_draw_menu_sprite_texture_infos *ff8_esc_menu_battle_pause_window(int a1, ff8_draw_menu_sprite_texture_infos *draw_infos);
