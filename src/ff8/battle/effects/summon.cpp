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

// GF summon effects

#include "screen_units.h"

#include "../../../ff8.h"
#include "../../../patch.h"

// Gilgamesh: the timeline task starts the attack task from a table (Zantetsuken, Masamune, Excaliber, Excalipoor)
#define GILGAMESH_ATTACK_TASKS 0x5B3
#define GILGAMESH_MASAMUNE_TASK 1
#define MASAMUNE_READ_SXY2 0xA3
// Eden: the draw functions are called from a table, number 53 draws the pixel particles
#define EDEN_DRAW_FUNCTIONS 0xA6
#define EDEN_PIXEL_PARTICLE_DRAW 53
#define EDEN_PIXEL_PARTICLE_PROJECT 0x5B0

namespace ff8_battle_effects
{
	static int (*project_point)(void *, int16_t *, void *, void *) = nullptr;

	// Eden's pixel particles: the projected point goes into a dot placed in pixels
	static int project_point_pixels(void *vector, int16_t *sxy, void *p, void *flag)
	{
		int ret = project_point(vector, sxy, p, flag);
		sxy_to_pixels(sxy, 1);
		return ret;
	}

	static const ribbon summon_ribbons[] = {
		// Quezacotl branches, bolts, arcs
		{FF8BattleEffect::Quezacotl, 0x4, {0x49, 0x74, 0x8C}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x8B), RIBBON_ABS_STEP(0x88), RIBBON_CALL_STEP(0xC82), RIBBON_CALL_STEP(0x16), RIBBON_ABS_STEP(0x4A), RIBBON_CALL_STEP(0x13B)}},
		{FF8BattleEffect::Quezacotl, 0x10, {0x17, 0x42, 0x5A}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x8B), RIBBON_ABS_STEP(0x88), RIBBON_CALL_STEP(0x7D5), RIBBON_ABS_STEP(0x23), RIBBON_CALL_STEP(0x503)}},
		{FF8BattleEffect::Quezacotl, 0x0, {0x6C, 0x88, 0xA3}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x8B), RIBBON_ABS_STEP(0x88), RIBBON_CALL_STEP(0x6E9), RIBBON_ABS_STEP(0x2), RIBBON_CALL_STEP(0x370)}}
	};

	void patch_summons(const DWORD *effects)
	{
		// Gilgamesh's Masamune: the slash is placed from a screen position in pixels
		const uint32_t gilgamesh_timeline = get_absolute_value(effects[FF8BattleEffect::GilgameshMasamune], 0x8F);
		const uint32_t masamune = ((uint32_t *)get_absolute_value(gilgamesh_timeline, GILGAMESH_ATTACK_TASKS))[GILGAMESH_MASAMUNE_TASK];

		replace_call(masamune + MASAMUNE_READ_SXY2, read_sxy2_pixels);

		// Eden's sparkles
		const uint32_t eden_draw = get_relative_call(get_absolute_value(effects[FF8BattleEffect::Eden], 0x45), 0x1D7);
		const uint32_t eden_pixel_particles = ((uint32_t *)get_absolute_value(eden_draw, EDEN_DRAW_FUNCTIONS))[EDEN_PIXEL_PARTICLE_DRAW];

		project_point = (int (*)(void *, int16_t *, void *, void *))get_relative_call(eden_pixel_particles, EDEN_PIXEL_PARTICLE_PROJECT);
		replace_call(eden_pixel_particles + EDEN_PIXEL_PARTICLE_PROJECT, project_point_pixels);

		patch_ribbons(summon_ribbons, sizeof(summon_ribbons) / sizeof(summon_ribbons[0]));
	}
}
