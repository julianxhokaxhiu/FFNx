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

#include "types.h"
#include "ff7.h"
#include "ff8.h"

#define FFNX_API __declspec(dllexport)

extern HWND gameHwnd;

extern MEMORYSTATUSEX last_ram_state;
extern uint version;
extern uint steam_edition;
extern uint estore_edition;
extern uint ff7_japanese_edition;
extern uint ff7_center_fields;
extern DWORD ff7_sfx_volume;
extern DWORD ff7_music_volume;

#define BASEDIR_LENGTH 512
extern char basedir[BASEDIR_LENGTH];

extern uint game_width;
extern uint game_height;
extern uint x_offset;
extern uint y_offset;

extern struct texture_format *texture_format;

extern struct ff7_externals ff7_externals;
extern struct ff8_externals ff8_externals;
extern struct common_externals common_externals;
extern struct driver_stats stats;

extern char popup_msg[];
extern uint popup_ttl;
extern uint popup_color;

extern struct game_mode modes[];
extern uint num_modes;

extern uint text_colors[];

extern uint ff8;
extern uint ff8_currentdisk;

extern uint frame_counter;
