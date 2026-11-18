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

// Battle: monsters, AI, effect cameras and anim sets, menus, effects

#include "battle.h"

#include "battle/monsters.h"
#include "battle/ai.h"
#include "battle/effect_camera.h"
#include "battle/anim_set.h"
#include "battle/ui.h"
#include "battle/effects.h"

void ff8_battle_init()
{
	// Monster models c0m144-c0m199, their sounds and Scan camera framing
	ff8_battle_monsters_init();
	// Monster AI target values 228-248 -> fixed battle slot pairs
	ff8_battle_ai_init();
	// Effect cameras from <direct_mode_path>/exe/effect_NNN.bcam
	ff8_battle_effect_camera_init();
	// Anim sets (limit breaks, Blue Magic...) from <direct_mode_path>/exe/effect_NNN.animset
	ff8_battle_anim_set_init();
	ff8_battle_ui_init();
	ff8_battle_effects_init();
}
