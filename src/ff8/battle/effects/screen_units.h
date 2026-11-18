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

#include "../effects.h"

#include <windows.h>
#include <stddef.h>
#include <stdint.h>

// The PC GTE returns SX/SY in eighths of a pixel, the PSX returned pixels. Many battle effects compare them with
// pixel values: these helpers give them pixels back (or the effects get their limits in eighths).
namespace ff8_battle_effects
{
	// Resolves the GTE and SSIGPU helpers from Cure's overlay renderer, before any effect is patched
	uint32_t resolve_screen_units(const DWORD *effects);

	void sxy_to_pixels(int16_t *sxy, int count);
	int16_t *read_sxy2_eighths(int16_t *out);
	int16_t *read_sxy2_pixels(int16_t *out);

	// Overlay renderer shared by Cure, Cura, Potion and Hi-Potion
	uint32_t cure_overlay_render(uint32_t effect);
	uint32_t cura_overlay_render(uint32_t effect);
	void patch_overlay_render(uint32_t render);

	// Lightning and acid streams are ribbons: a chain of projected points, each one widened into two edge points
	// along the normal of the chain. The edge routine takes the distance between two points from a 128 x 128 pixel
	// table and limits each axis to 127, so it gets its points in pixels and its edge offsets go back to eighths.
	enum ribbon_step_kind
	{
		RIBBON_END = 0,
		RIBBON_ABS,
		RIBBON_CALL
	};

	struct ribbon_step
	{
		ribbon_step_kind kind;
		uint32_t offset;
	};

	// The segment record holds x, y, depth at sx_offset and the second edge point at sx_offset + 8.
	// The path goes from the effect's entry to the draw function, which calls the edge routine 3 times.
	struct ribbon
	{
		FF8BattleEffect::Effect effect;
		uint32_t sx_offset;
		uint32_t edge_calls[3];
		ribbon_step path[12];
	};

	void patch_ribbons(const ribbon *ribbons, size_t count, uint32_t root = 0);

	// Topics
	void patch_magic(const DWORD *effects, uint32_t cure_render);
	void patch_summons(const DWORD *effects);
	void patch_enemy_attacks(const DWORD *effects);
	void patch_items(const DWORD *effects);
}

#define RIBBON_ABS_STEP(offset) {ff8_battle_effects::RIBBON_ABS, offset}
#define RIBBON_CALL_STEP(offset) {ff8_battle_effects::RIBBON_CALL, offset}
