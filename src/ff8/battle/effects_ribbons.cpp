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

// Lightning and acid streams are ribbons: a chain of projected points, each one widened into two edge
// points along the normal of the chain. The edge routine takes the distance between two points from a
// 128 x 128 pixel table and limits each axis to 127, but the PC GTE gives the points in eighths of a pixel:
// the ribbons come out a few pixels wide or with crossed edges. The developers fixed it for Diablos only
// (points divided by 8 before the edges, the result multiplied by 8).
// Here the edge routine gets its points in pixels and its edge offsets are multiplied by 8 afterwards.

#include "effects_ribbons.h"
#include "effects.h"

#include "../../ff8.h"
#include "../../patch.h"
#include "../../globals.h"

#include <array>
#include <stdint.h>
#include <utility>

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

#define ABS(offset) {RIBBON_ABS, offset}
#define CALL(offset) {RIBBON_CALL, offset}

// The segment record holds x, y, depth at sx_offset and the second edge point at sx_offset + 8.
// The path goes from the effect's entry to the draw function, which calls the edge routine 3 times.
struct ribbon
{
	FF8BattleEffect::Effect effect;
	uint32_t sx_offset;
	uint32_t edge_calls[3];
	ribbon_step path[12];
};

static const ribbon ribbons[] = {
	// Quezacotl branches, bolts, arcs
	{FF8BattleEffect::Quezacotl, 0x4, {0x49, 0x74, 0x8C}, {CALL(0x5), ABS(0x8B), ABS(0x88), CALL(0xC82), CALL(0x16), ABS(0x4A), CALL(0x13B)}},
	{FF8BattleEffect::Quezacotl, 0x10, {0x17, 0x42, 0x5A}, {CALL(0x5), ABS(0x8B), ABS(0x88), CALL(0x7D5), ABS(0x23), CALL(0x503)}},
	{FF8BattleEffect::Quezacotl, 0x0, {0x6C, 0x88, 0xA3}, {CALL(0x5), ABS(0x8B), ABS(0x88), CALL(0x6E9), ABS(0x2), CALL(0x370)}},
	// Breath branches, arcs
	{FF8BattleEffect::Breath, 0x4, {0xF3, 0x11E, 0x136}, {CALL(0x5), ABS(0x4C), ABS(0x15F), CALL(0x5F0), CALL(0x16), ABS(0x4A), CALL(0x132)}},
	{FF8BattleEffect::Breath, 0x0, {0x6C, 0x88, 0xA3}, {CALL(0x5), ABS(0x4C), ABS(0x15F), CALL(0x85), ABS(0x2), CALL(0x370)}},
	// Electric Discharge arcs, crawlers
	{FF8BattleEffect::ElectricDischarge, 0x10, {0x17, 0x42, 0x5A}, {CALL(0x5), ABS(0x53), ABS(0x163), CALL(0x25F), ABS(0x20), CALL(0x421)}},
	{FF8BattleEffect::ElectricDischarge, 0x10, {0xBA, 0xE5, 0xFD}, {CALL(0x5), ABS(0x53), ABS(0x163), CALL(0x80), CALL(0x63), ABS(0x56), CALL(0x174)}},
	// Electrocute (Quistis) arcs, crawlers
	{FF8BattleEffect::Electrocute, 0x10, {0x17, 0x42, 0x5A}, {CALL(0x5), ABS(0x57), ABS(0x191), CALL(0x20A), ABS(0x20), CALL(0x422)}},
	{FF8BattleEffect::Electrocute, 0x10, {0xBA, 0xE5, 0xFD}, {CALL(0x5), ABS(0x57), ABS(0x191), CALL(0x79), CALL(0x63), ABS(0x56), CALL(0x174)}},
	// X-ATM092 death arcs
	{FF8BattleEffect::XATM092Death, 0x0, {0x6C, 0x88, 0xA3}, {CALL(0x5), ABS(0x5B), ABS(0x120), CALL(0x4CF), ABS(0x1), CALL(0x69), ABS(0xB1), CALL(0x328)}},
	// Mega Spark
	{FF8BattleEffect::MegaSpark, 0x10, {0x17, 0x42, 0x5A}, {CALL(0x5), ABS(0x56), ABS(0xFE), CALL(0x370), CALL(0xA3), CALL(0x454)}},
	{FF8BattleEffect::MegaSpark, 0x0, {0x6C, 0x88, 0xA3}, {CALL(0x5), ABS(0x56), ABS(0xFE), CALL(0x14E), ABS(0x1), CALL(0x346)}},
	// Mega Pulse Cannon
	{FF8BattleEffect::MegaPulseCannon, 0x0, {0x6C, 0x88, 0xA3}, {CALL(0x5), ABS(0x5F), ABS(0xE4), CALL(0xAC), ABS(0x1), CALL(0x346)}},
	// Saliva, Dissolving Acid, Gastric Juice, Acid (Quistis) streams
	{FF8BattleEffect::Saliva, 0x8, {0x17, 0x42, 0x5A}, {CALL(0x5), ABS(0x4C), ABS(0xD7), ABS(0x19E), CALL(0x31F)}},
	{FF8BattleEffect::DissolvingAcid, 0x8, {0x17, 0x42, 0x5A}, {CALL(0x5), ABS(0x4C), ABS(0x14F), CALL(0xC6), ABS(0x20), CALL(0x25C)}},
	{FF8BattleEffect::GastricJuice, 0x10, {0x17, 0x42, 0x5A}, {CALL(0x5), ABS(0x4C), ABS(0xDB), CALL(0x136), ABS(0x23), CALL(0x1FD)}},
	{FF8BattleEffect::QuistisAcid, 0x10, {0x17, 0x42, 0x5A}, {CALL(0x5), ABS(0x4C), ABS(0xDB), CALL(0x12C), ABS(0x23), CALL(0x200)}},
	// Ultimecia junctioning to Griever, bolts
	{FF8BattleEffect::UltimeciaJunction, 0x0, {0xD0, 0xF2, 0x111}, {ABS(0x2D), ABS(0x4A), ABS(0x9), ABS(0xD), ABS(0xF), CALL(0x71), CALL(0x23), ABS(0x8C), CALL(0x79), CALL(0x0), CALL(0xD4)}}
};

static constexpr size_t ribbon_count = sizeof(ribbons) / sizeof(ribbons[0]);

typedef int (*ribbon_edge_fn)(uint8_t *, uint8_t *, int16_t *, int16_t *);

static ribbon_edge_fn ribbon_edges[ribbon_count] = {};

static int ribbon_edges_in_pixels(size_t index, uint8_t *segment, uint8_t *next, int16_t *previous_direction, int16_t *direction)
{
	int16_t *point = (int16_t *)(segment + ribbons[index].sx_offset);
	int16_t *outer = point + 4;
	int16_t *next_point = next != nullptr ? (int16_t *)(next + ribbons[index].sx_offset) : nullptr;
	const int16_t x = point[0], y = point[1], outer_x = outer[0], outer_y = outer[1];
	const int16_t next_x = next_point != nullptr ? next_point[0] : 0, next_y = next_point != nullptr ? next_point[1] : 0;

	point[0] = int16_t(x >> 3);
	point[1] = int16_t(y >> 3);
	if (next_point != nullptr)
	{
		next_point[0] = int16_t(next_x >> 3);
		next_point[1] = int16_t(next_y >> 3);
	}
	// Stays as is when the routine leaves the segment without edges (behind the camera)
	outer[0] = INT16_MAX;

	const int ret = ribbon_edges[index](segment, next, previous_direction, direction);

	if (next_point != nullptr)
	{
		next_point[0] = next_x;
		next_point[1] = next_y;
	}

	if (outer[0] == INT16_MAX)
	{
		point[0] = x;
		point[1] = y;
		outer[0] = outer_x;
		outer[1] = outer_y;

		return ret;
	}

	// The routine wrote the point minus the offset and the point plus the offset, in pixels
	const int offset_x = (x >> 3) - point[0], offset_y = (y >> 3) - point[1];

	point[0] = int16_t(x - offset_x * 8);
	point[1] = int16_t(y - offset_y * 8);
	outer[0] = int16_t(x + offset_x * 8);
	outer[1] = int16_t(y + offset_y * 8);

	return ret;
}

template <size_t I>
static int ribbon_edges_hook(uint8_t *segment, uint8_t *next, int16_t *previous_direction, int16_t *direction)
{
	return ribbon_edges_in_pixels(I, segment, next, previous_direction, direction);
}

template <size_t... I>
static std::array<ribbon_edge_fn, sizeof...(I)> ribbon_edges_hooks(std::index_sequence<I...>)
{
	return {ribbon_edges_hook<I>...};
}

static uint32_t ribbon_draw(const ribbon &ribbon)
{
	uint32_t address = ff8_externals.func_off_battle_effects_C81774[ribbon.effect];

	for (const ribbon_step &step : ribbon.path)
	{
		if (step.kind == RIBBON_ABS) address = get_absolute_value(address, step.offset);
		else if (step.kind == RIBBON_CALL) address = get_relative_call(address, step.offset);
	}

	return address;
}

void ff8_battle_effects_ribbons_init()
{
	static const std::array<ribbon_edge_fn, ribbon_count> hooks = ribbon_edges_hooks(std::make_index_sequence<ribbon_count>());
	uint32_t draws[ribbon_count];

	// Everything is resolved before anything is patched
	for (size_t i = 0; i < ribbon_count; ++i)
	{
		draws[i] = ribbon_draw(ribbons[i]);
		ribbon_edges[i] = (ribbon_edge_fn)get_relative_call(draws[i], ribbons[i].edge_calls[0]);
	}

	for (size_t i = 0; i < ribbon_count; ++i)
	{
		for (uint32_t call : ribbons[i].edge_calls) replace_call(draws[i] + call, (void *)hooks[i]);
	}
}
