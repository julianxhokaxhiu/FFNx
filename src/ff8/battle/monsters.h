/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2026 HobbitDur                                          //
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

// Unlock the unused battle monster models c0m144..c0m199 so encounters can
// reference them. Call once from ff8_init_hooks(), after ff8_externals is
// resolved.
void ff8_battle_monsters_init();

// Seven world sound IDs per actor: character rows 0-15, monster rows c0m# + 16.
constexpr unsigned FF8_BATTLE_ACTOR_SOUND_ROWS = 216; // through c0m199
constexpr unsigned FF8_BATTLE_ACTOR_SOUND_SLOTS = 7;
extern uint32_t (*ff8_battle_actor_sounds)[FF8_BATTLE_ACTOR_SOUND_SLOTS];
