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

// Magic and Blue Magic effects

#include "screen_units.h"

namespace ff8_battle_effects
{
	static const ribbon magic_ribbons[] = {
		// Electrocute (Quistis) arcs, crawlers
		{FF8BattleEffect::Electrocute, 0x10, {0x17, 0x42, 0x5A}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x57), RIBBON_ABS_STEP(0x191), RIBBON_CALL_STEP(0x20A), RIBBON_ABS_STEP(0x20), RIBBON_CALL_STEP(0x422)}},
		{FF8BattleEffect::Electrocute, 0x10, {0xBA, 0xE5, 0xFD}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x57), RIBBON_ABS_STEP(0x191), RIBBON_CALL_STEP(0x79), RIBBON_CALL_STEP(0x63), RIBBON_ABS_STEP(0x56), RIBBON_CALL_STEP(0x174)}},
		// Acid (Quistis) streams
		{FF8BattleEffect::QuistisAcid, 0x10, {0x17, 0x42, 0x5A}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x4C), RIBBON_ABS_STEP(0xDB), RIBBON_CALL_STEP(0x12C), RIBBON_ABS_STEP(0x23), RIBBON_CALL_STEP(0x200)}}
	};

	void patch_magic(const DWORD *effects)
	{
		// Cure and Cura: the light on the target
		patch_overlay_render(cure_overlay_render(effects[FF8BattleEffect::Cure]));
		patch_overlay_render(cura_overlay_render(effects[FF8BattleEffect::Cura]));

		patch_ribbons(magic_ribbons, sizeof(magic_ribbons) / sizeof(magic_ribbons[0]));
	}
}
