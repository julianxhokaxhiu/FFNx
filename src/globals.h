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

#include <windows.h>

#include "ff7.h"
#include "ff8.h"

#define FFNX_API __declspec(dllexport)

extern HWND gameHwnd;

extern MEMORYSTATUSEX last_ram_state;
extern uint32_t version;
extern uint32_t steam_edition;
extern uint32_t estore_edition;
extern uint32_t ff7_japanese_edition;
extern uint32_t ff7_center_fields;
extern DWORD ff7_sfx_volume;
extern DWORD ff7_music_volume;

#define BASEDIR_LENGTH 512
extern char basedir[BASEDIR_LENGTH];

extern uint32_t game_width;
extern uint32_t game_height;
extern uint32_t x_offset;
extern uint32_t y_offset;

extern struct texture_format *texture_format;

extern struct ff7_externals ff7_externals;
extern struct ff8_externals ff8_externals;
extern struct common_externals common_externals;
extern struct driver_stats stats;

extern char popup_msg[];
extern uint32_t popup_ttl;
extern uint32_t popup_color;

extern struct game_mode modes[];
extern uint32_t num_modes;

extern uint32_t text_colors[];

extern uint32_t ff8;
extern uint32_t ff8_currentdisk;

extern uint32_t frame_counter;
