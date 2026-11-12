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

// Enemy attack effects

#include "screen_units.h"

#include "../../../ff8.h"
#include "../../../patch.h"

#include <algorithm>
#include <cstdlib>

#define HYPNOTIZE_MAIN_READ_SXY2 0x4C
// Magic Summon: the vertex warp and what it calls
#define MAGIC_SUMMON_WARP_VERTICES 0x99
#define WARP_LOAD_VECTOR 0x32
#define WARP_PROJECT 0x37
#define WARP_READ_OTZ 0x50
#define WARP_DISTANCES 0xD4

namespace ff8_battle_effects
{
	static void (*gte_load_vector)(void *) = nullptr;
	static void (*gte_project)() = nullptr;
	static void (*gte_read_otz)(void *) = nullptr;
	static const uint8_t *warp_distances = nullptr;

#pragma pack(push, 1)
	// Magic Summon's warp work block
	struct magic_summon_warp
	{
		uint8_t unknown_0[0xC];
		int16_t clip_x0, clip_y0, clip_x1, clip_y1; // pixels
		uint8_t unknown_14[0x8];
		int16_t sx, sy;
		uint16_t otz;
		uint8_t clip_flags;
		uint8_t unknown_23[0x29];
		uint16_t *weights;
		int16_t radius; // pixels
		int16_t centre_x, centre_y; // the point on screen
		int16_t largest_offset; // pixels, the radius of the next frame
	};
#pragma pack(pop)

	// Magic Summon pushes the target's vertices away from a point on screen. Its offsets, their limit (128),
	// the distance table and the radius are PSX pixels: the offset from the point goes to pixels and the
	// pushed offset back to eighths. The rest is the game's code.
	static void magic_summon_warp_vertices(uint8_t **vertices, uint8_t **records, magic_summon_warp *warp)
	{
		uint8_t *vertex = *vertices;
		uint32_t *record = (uint32_t *)*records;
		int count = *(int16_t *)vertex;

		vertex += 2;

		for (; count > 0; --count, record += 2)
		{
			gte_load_vector(vertex);
			gte_project();
			vertex += 6;
			read_sxy2_eighths(&warp->sx);
			gte_read_otz(&warp->otz);

			if (warp->otz == 0)
			{
				((uint16_t *)record)[3] = 0x10;
				continue;
			}

			const int dx = (warp->sx - warp->centre_x) >> 3;
			const int dy = (warp->sy - warp->centre_y) >> 3;
			const int16_t abs_dx = int16_t((std::min)(std::abs(dx), 0x80));
			const int16_t abs_dy = int16_t((std::min)(std::abs(dy), 0x80));

			if (warp->largest_offset < abs_dx) warp->largest_offset = abs_dx;
			if (warp->largest_offset < abs_dy) warp->largest_offset = abs_dy;

			const int distance = warp_distances[(abs_dy << 7) + abs_dx];

			if (distance != 0 && distance < warp->radius)
			{
				const int weight = warp->weights[(distance << 7) / warp->radius];

				// (offset x weight) >> 12 in pixels, << 3 for eighths
				warp->sx = int16_t(warp->centre_x + ((dx * weight) >> 9));
				warp->sy = int16_t(warp->centre_y + ((dy * weight) >> 9));
			}

			if (warp->sx < warp->clip_x0 * 8) warp->clip_flags |= 1;
			else if (warp->sx >= warp->clip_x1 * 8) warp->clip_flags |= 2;

			if (warp->sy < warp->clip_y0 * 8) warp->clip_flags |= 4;
			else if (warp->sy >= warp->clip_y1 * 8) warp->clip_flags |= 8;

			record[0] = *(uint32_t *)&warp->sx;
			record[1] = *(uint32_t *)&warp->otz;
		}

		*vertices = vertex;
		*records = (uint8_t *)record;
	}

	static const ribbon enemy_ribbons[] = {
		// Breath branches, arcs
		{FF8BattleEffect::Breath, 0x4, {0xF3, 0x11E, 0x136}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x4C), RIBBON_ABS_STEP(0x15F), RIBBON_CALL_STEP(0x5F0), RIBBON_CALL_STEP(0x16), RIBBON_ABS_STEP(0x4A), RIBBON_CALL_STEP(0x132)}},
		{FF8BattleEffect::Breath, 0x0, {0x6C, 0x88, 0xA3}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x4C), RIBBON_ABS_STEP(0x15F), RIBBON_CALL_STEP(0x85), RIBBON_ABS_STEP(0x2), RIBBON_CALL_STEP(0x370)}},
		// Electric Discharge arcs, crawlers
		{FF8BattleEffect::ElectricDischarge, 0x10, {0x17, 0x42, 0x5A}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x53), RIBBON_ABS_STEP(0x163), RIBBON_CALL_STEP(0x25F), RIBBON_ABS_STEP(0x20), RIBBON_CALL_STEP(0x421)}},
		{FF8BattleEffect::ElectricDischarge, 0x10, {0xBA, 0xE5, 0xFD}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x53), RIBBON_ABS_STEP(0x163), RIBBON_CALL_STEP(0x80), RIBBON_CALL_STEP(0x63), RIBBON_ABS_STEP(0x56), RIBBON_CALL_STEP(0x174)}},
		// X-ATM092 death arcs
		{FF8BattleEffect::XATM092Death, 0x0, {0x6C, 0x88, 0xA3}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x5B), RIBBON_ABS_STEP(0x120), RIBBON_CALL_STEP(0x4CF), RIBBON_ABS_STEP(0x1), RIBBON_CALL_STEP(0x69), RIBBON_ABS_STEP(0xB1), RIBBON_CALL_STEP(0x328)}},
		// Mega Spark
		{FF8BattleEffect::MegaSpark, 0x10, {0x17, 0x42, 0x5A}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x56), RIBBON_ABS_STEP(0xFE), RIBBON_CALL_STEP(0x370), RIBBON_CALL_STEP(0xA3), RIBBON_CALL_STEP(0x454)}},
		{FF8BattleEffect::MegaSpark, 0x0, {0x6C, 0x88, 0xA3}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x56), RIBBON_ABS_STEP(0xFE), RIBBON_CALL_STEP(0x14E), RIBBON_ABS_STEP(0x1), RIBBON_CALL_STEP(0x346)}},
		// Mega Pulse Cannon
		{FF8BattleEffect::MegaPulseCannon, 0x0, {0x6C, 0x88, 0xA3}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x5F), RIBBON_ABS_STEP(0xE4), RIBBON_CALL_STEP(0xAC), RIBBON_ABS_STEP(0x1), RIBBON_CALL_STEP(0x346)}},
		// Saliva, Dissolving Acid, Gastric Juice streams
		{FF8BattleEffect::Saliva, 0x8, {0x17, 0x42, 0x5A}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x4C), RIBBON_ABS_STEP(0xD7), RIBBON_ABS_STEP(0x19E), RIBBON_CALL_STEP(0x31F)}},
		{FF8BattleEffect::DissolvingAcid, 0x8, {0x17, 0x42, 0x5A}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x4C), RIBBON_ABS_STEP(0x14F), RIBBON_CALL_STEP(0xC6), RIBBON_ABS_STEP(0x20), RIBBON_CALL_STEP(0x25C)}},
		{FF8BattleEffect::GastricJuice, 0x10, {0x17, 0x42, 0x5A}, {RIBBON_CALL_STEP(0x5), RIBBON_ABS_STEP(0x4C), RIBBON_ABS_STEP(0xDB), RIBBON_CALL_STEP(0x136), RIBBON_ABS_STEP(0x23), RIBBON_CALL_STEP(0x1FD)}},
		// Ultimecia junctioning to Griever, bolts
		{FF8BattleEffect::UltimeciaJunction, 0x0, {0xD0, 0xF2, 0x111}, {RIBBON_ABS_STEP(0x2D), RIBBON_ABS_STEP(0x4A), RIBBON_ABS_STEP(0x9), RIBBON_ABS_STEP(0xD), RIBBON_ABS_STEP(0xF), RIBBON_CALL_STEP(0x71), RIBBON_CALL_STEP(0x23), RIBBON_ABS_STEP(0x8C), RIBBON_CALL_STEP(0x79), RIBBON_CALL_STEP(0x0), RIBBON_CALL_STEP(0xD4)}}
	};

	void patch_enemy_attacks(const DWORD *effects)
	{
		// Hypnotize: the swirl is placed from a screen position in pixels
		const uint32_t hypnotize_master = get_absolute_value(get_relative_call(effects[FF8BattleEffect::Hypnotize], 0x5), 0x4C);
		const uint32_t hypnotize_main = get_absolute_value(hypnotize_master, 0xC8);

		replace_call(hypnotize_main + HYPNOTIZE_MAIN_READ_SXY2, read_sxy2_pixels);

		// Magic Summon (Sphinxara): the spikes thrown across the screen
		uint32_t magic_summon = get_relative_call(effects[FF8BattleEffect::MagicSummon], 0x5);
		magic_summon = get_absolute_value(get_absolute_value(magic_summon, 0x4C), 0x72);
		magic_summon = get_relative_call(get_relative_call(magic_summon, 0x1D7), 0xFC);
		const uint32_t warp_vertices = get_relative_call(magic_summon, MAGIC_SUMMON_WARP_VERTICES);

		gte_load_vector = (void (*)(void *))get_relative_call(warp_vertices, WARP_LOAD_VECTOR);
		gte_project = (void (*)())get_relative_call(warp_vertices, WARP_PROJECT);
		gte_read_otz = (void (*)(void *))get_relative_call(warp_vertices, WARP_READ_OTZ);
		warp_distances = (const uint8_t *)get_absolute_value(warp_vertices, WARP_DISTANCES);
		replace_function(warp_vertices, magic_summon_warp_vertices);

		patch_ribbons(enemy_ribbons, sizeof(enemy_ribbons) / sizeof(enemy_ribbons[0]));
	}
}
