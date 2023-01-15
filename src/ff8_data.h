/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2020 Marcin Gomulak                                     //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
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

#include "patch.h"

// FF8 game mode definitions
static struct game_mode ff8_modes[] = {
	{FF8_MODE_CREDITS,       "MODE_CREDITS",       MODE_CREDITS,       true },
	{FF8_MODE_FIELD,         "MODE_FIELD",         MODE_FIELD,         true },
	{FF8_MODE_WORLDMAP,      "MODE_WORLDMAP",      MODE_WORLDMAP,      true },
	{FF8_MODE_SWIRL,         "MODE_SWIRL",         MODE_SWIRL,         true },
	{FF8_MODE_AFTER_BATTLE,  "MODE_AFTER_BATTLE",  MODE_AFTER_BATTLE,  true },
	{FF8_MODE_5,             "MODE_5",             MODE_UNKNOWN,       true },
	{FF8_MODE_MENU,          "MODE_MENU",          MODE_MENU,          true },
	{FF8_MODE_7,             "MODE_7",             MODE_UNKNOWN,       true },
	{FF8_MODE_CARDGAME,      "MODE_CARDGAME",      MODE_CARDGAME,      true },
	{FF8_MODE_9,             "MODE_9",             MODE_UNKNOWN,       true },
	{FF8_MODE_TUTO,          "MODE_TUTO",          MODE_UNKNOWN,       true },
	{FF8_MODE_11,            "MODE_11",            MODE_UNKNOWN,       true },
	{FF8_MODE_INTRO,         "MODE_INTRO",         MODE_INTRO,         true },
	{FF8_MODE_100,           "MODE_100",           MODE_UNKNOWN,       true },
	{FF8_MODE_BATTLE,        "MODE_BATTLE",        MODE_BATTLE,        true },
};

void ff8_set_main_loop(uint32_t driver_mode, uint32_t main_loop);
void ff8_find_externals();
void ff8_data();
