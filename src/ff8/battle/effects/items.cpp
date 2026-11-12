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

// Item effects (Boko comes from Gysahl Greens)

#include "screen_units.h"

#include "../../../ff8.h"
#include "../../../patch.h"

namespace ff8_battle_effects
{
	// ChocoBocle: Init -> Tick -> ... -> particle draw -> polygon list dispatcher
	static uint32_t chocobocle_polygon_dispatch(uint32_t effect)
	{
		uint32_t address = effect;

		for (uint32_t offset : {0x2D, 0x4A, 0x9, 0xD, 0x26, 0xD, 0x45, 0xD}) address = get_absolute_value(address, offset);

		uint32_t particle_task = get_relative_call(address, 0x38);
		uint32_t particle_draw = get_absolute_value(particle_task, 0x101);

		return get_relative_call(particle_draw, 0x2C7);
	}

	// ChocoBocle's polygon renderers skip a polygon when its corners are all past x 320 or all past y 216
	// (PSX pixels): the limits go to 320 * 8 and 216 * 8. Call offset in the dispatcher, then the limits.
	struct chocobocle_renderer
	{
		uint32_t call;
		uint32_t x_limits[4];
		uint32_t y_limits[4];
	};

	static const chocobocle_renderer chocobocle_renderers[] = {
		{0x65, {0xF5, 0x10E, 0x120}, {0x132, 0x144, 0x156}},
		{0x86, {0x108, 0x11E, 0x138, 0x1AD}, {0x151, 0x16A, 0x183, 0x1C9}},
		{0xA3, {0x148, 0x162, 0x174}, {0x186, 0x198, 0x1AA}},
		{0xC0, {0x163, 0x17E, 0x190, 0x1EA}, {0x1A2, 0x1B4, 0x1C6, 0x1FC}},
		{0xDD, {0xF4, 0x10D, 0x11E}, {0x130, 0x142, 0x154}},
		{0xFA, {0x10A, 0x120, 0x139, 0x1AF}, {0x152, 0x16B, 0x184, 0x1CB}},
		{0x117, {0x145, 0x15F, 0x171}, {0x183, 0x195, 0x1A7}},
		{0x134, {0x164, 0x17E, 0x190, 0x1E9}, {0x1A2, 0x1B4, 0x1C6, 0x1FB}}
	};

	void patch_items(const DWORD *effects)
	{
		// Potion and Hi-Potion: the light on the target
		patch_overlay_render(cure_overlay_render(effects[FF8BattleEffect::Potion]));
		patch_overlay_render(cura_overlay_render(effects[FF8BattleEffect::HiPotion]));

		// Boko's ChocoBocle: the polygons past the middle of the screen
		const uint32_t chocobocle = chocobocle_polygon_dispatch(effects[FF8BattleEffect::BokoChocobocle]);

		for (const chocobocle_renderer &renderer : chocobocle_renderers)
		{
			const uint32_t render = get_relative_call(chocobocle, renderer.call);

			for (uint32_t offset : renderer.x_limits) if (offset != 0) patch_code_word(render + offset, 0x140 * 8);
			for (uint32_t offset : renderer.y_limits) if (offset != 0) patch_code_word(render + offset, 0xD8 * 8);
		}
	}
}
