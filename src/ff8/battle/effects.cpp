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

#include "effects.h"
#include "effects/screen_units.h"

#include "../../ff8.h"
#include "../../globals.h"

void ff8_battle_effects_init()
{
	const DWORD *effects = ff8_externals.func_off_battle_effects_C81774;

	// The shared helpers are read from Cure's renderer, before the magic patches it
	ff8_battle_effects::resolve_screen_units(effects);

	ff8_battle_effects::patch_magic(effects);
	ff8_battle_effects::patch_summons(effects);
	ff8_battle_effects::patch_enemy_attacks(effects);
	ff8_battle_effects::patch_items(effects);
}
