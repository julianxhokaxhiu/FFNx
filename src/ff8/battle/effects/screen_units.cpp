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

#include "screen_units.h"

#include "../../../ff8.h"
#include "../../../patch.h"
#include "../../../globals.h"

#include <array>
#include <utility>

// Call sites in the overlay renderer shared by Cure, Cura, Potion and Hi-Potion (projector pass)
#define OVERLAY_TRI_READ_SXY012 0x414
#define OVERLAY_TRI_LIGHT_INSERT 0x4E1
#define OVERLAY_TRI_MODEL_INSERT 0x541
#define OVERLAY_QUAD_READ_SXY012 0x729
#define OVERLAY_QUAD_READ_SXY2 0x74A
#define OVERLAY_QUAD_LIGHT_INSERT 0x831
#define OVERLAY_QUAD_MODEL_INSERT 0x898

// Ribbon draw functions patched at most (one edge routine hook each)
#define RIBBON_SLOTS 32

namespace ff8_battle_effects
{
	static int16_t *(*gte_read_sxy012)(int16_t *) = nullptr;
	static int16_t *(*gte_read_sxy2)(int16_t *) = nullptr;
	static void *(*insert_prim_auto_depth)(void *, void *) = nullptr;
	static void *(*insert_prim_depth_keys)(void *, void *, int, int, int, int) = nullptr;

	static void *pending_light_ot = nullptr;
	static void *pending_light = nullptr;

	void sxy_to_pixels(int16_t *sxy, int count)
	{
		for (int i = 0; i < count * 2; ++i) sxy[i] = int16_t(sxy[i] >> 3);
	}

	static int16_t *read_sxy012_pixels(int16_t *out)
	{
		gte_read_sxy012(out);
		sxy_to_pixels(out, 3);
		return out;
	}

	int16_t *read_sxy2_eighths(int16_t *out)
	{
		return gte_read_sxy2(out);
	}

	int16_t *read_sxy2_pixels(int16_t *out)
	{
		gte_read_sxy2(out);
		sxy_to_pixels(out, 1);
		return out;
	}

	// The light would take its keys from the projector pass, far behind the model: keep it for the model insert
	static void *defer_light_insert(void *ot_entry, void *prim)
	{
		if (pending_light != nullptr) insert_prim_auto_depth(pending_light_ot, pending_light);

		pending_light_ot = ot_entry;
		pending_light = prim;

		return nullptr;
	}

	// Model polygon first, then its light on top with the same depth keys
	static void *insert_model_then_light(void *ot_entry, void *prim, int z0, int z1, int z2, int z3)
	{
		void *ret = insert_prim_depth_keys(ot_entry, prim, z0, z1, z2, z3);

		if (pending_light != nullptr)
		{
			ret = insert_prim_depth_keys(ot_entry, pending_light, z0, z1, z2, z3);
			pending_light = nullptr;
			pending_light_ot = nullptr;
		}

		return ret;
	}

	// Cure and Potion: Init -> Tick -> spawn emitter -> Emitter -> spawn ring -> Ring -> draw -> render
	uint32_t cure_overlay_render(uint32_t effect)
	{
		uint32_t tick = get_absolute_value(effect, 0x2D);
		uint32_t spawn_emitter = get_absolute_value(tick, 0x4A);
		uint32_t emitter = get_absolute_value(spawn_emitter, 0x9);
		uint32_t spawn_ring = get_absolute_value(emitter, 0xD);
		uint32_t ring = get_absolute_value(spawn_ring, 0x9);
		uint32_t draw = get_relative_call(ring, 0x11A);

		return get_relative_call(draw, 0xA9);
	}

	// Cura and Hi-Potion: Init -> Tick -> spawn emitter -> Emitter -> spawn overlay -> Overlay -> draw -> render
	uint32_t cura_overlay_render(uint32_t effect)
	{
		uint32_t tick = get_absolute_value(effect, 0x28);
		uint32_t spawn_emitter = get_absolute_value(tick, 0x14);
		uint32_t emitter = get_absolute_value(spawn_emitter, 0x9);
		uint32_t spawn_overlay = get_absolute_value(emitter, 0x15);
		uint32_t overlay = get_absolute_value(spawn_overlay, 0x21);
		uint32_t draw = get_relative_call(overlay, 0xCE);

		return get_relative_call(draw, 0xA9);
	}

	void patch_overlay_render(uint32_t render)
	{
		replace_call(render + OVERLAY_TRI_READ_SXY012, read_sxy012_pixels);
		replace_call(render + OVERLAY_TRI_LIGHT_INSERT, defer_light_insert);
		replace_call(render + OVERLAY_TRI_MODEL_INSERT, insert_model_then_light);
		replace_call(render + OVERLAY_QUAD_READ_SXY012, read_sxy012_pixels);
		replace_call(render + OVERLAY_QUAD_READ_SXY2, read_sxy2_pixels);
		replace_call(render + OVERLAY_QUAD_LIGHT_INSERT, defer_light_insert);
		replace_call(render + OVERLAY_QUAD_MODEL_INSERT, insert_model_then_light);
	}

	uint32_t resolve_screen_units(const DWORD *effects)
	{
		const uint32_t cure = cure_overlay_render(effects[FF8BattleEffect::Cure]);

		gte_read_sxy012 = (int16_t *(*)(int16_t *))get_relative_call(cure, OVERLAY_TRI_READ_SXY012);
		gte_read_sxy2 = (int16_t *(*)(int16_t *))get_relative_call(cure, OVERLAY_QUAD_READ_SXY2);
		insert_prim_auto_depth = (void *(*)(void *, void *))get_relative_call(cure, OVERLAY_TRI_LIGHT_INSERT);
		insert_prim_depth_keys = (void *(*)(void *, void *, int, int, int, int))get_relative_call(cure, OVERLAY_TRI_MODEL_INSERT);

		return cure;
	}

	// Ribbons: each patched draw function gets its own hook slot, which knows its segment layout and edge routine
	typedef int (*ribbon_edge_fn)(uint8_t *, uint8_t *, int16_t *, int16_t *);

	static uint32_t ribbon_sx_offsets[RIBBON_SLOTS] = {};
	static ribbon_edge_fn ribbon_edges[RIBBON_SLOTS] = {};
	static size_t ribbon_slots_used = 0;

	static int ribbon_edges_in_pixels(size_t slot, uint8_t *segment, uint8_t *next, int16_t *previous_direction, int16_t *direction)
	{
		int16_t *point = (int16_t *)(segment + ribbon_sx_offsets[slot]);
		int16_t *outer = point + 4;
		int16_t *next_point = next != nullptr ? (int16_t *)(next + ribbon_sx_offsets[slot]) : nullptr;
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

		const int ret = ribbon_edges[slot](segment, next, previous_direction, direction);

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

	struct ribbon_resolution
	{
		uint32_t site, result;
		ribbon_step_kind kind;
	};

	static uint32_t ribbon_draw(const ribbon &ribbon, ribbon_resolution *resolved, size_t &resolved_count)
	{
		uint32_t address = ff8_externals.func_off_battle_effects_C81774[ribbon.effect];

		for (const ribbon_step &step : ribbon.path)
		{
			if (step.kind == RIBBON_END) break;

			const uint32_t site = address + step.offset;
			size_t i = 0;
			while (i < resolved_count && (resolved[i].site != site || resolved[i].kind != step.kind)) ++i;
			if (i < resolved_count)
			{
				address = resolved[i].result;
				continue;
			}

			if (step.kind == RIBBON_ABS) address = get_absolute_value(address, step.offset);
			else address = get_relative_call(address, step.offset);
			resolved[resolved_count++] = {site, address, step.kind};
		}

		return address;
	}

	void patch_ribbons(const ribbon *ribbons, size_t count)
	{
		static const std::array<ribbon_edge_fn, RIBBON_SLOTS> hooks = ribbon_edges_hooks(std::make_index_sequence<RIBBON_SLOTS>());
		uint32_t draws[RIBBON_SLOTS];
		ribbon_resolution resolved[RIBBON_SLOTS * 12];
		size_t resolved_count = 0;

		if (ribbon_slots_used + count > RIBBON_SLOTS) return;

		// Everything is resolved before anything is patched
		for (size_t i = 0; i < count; ++i)
		{
			const size_t slot = ribbon_slots_used + i;

			draws[i] = ribbon_draw(ribbons[i], resolved, resolved_count);
			ribbon_sx_offsets[slot] = ribbons[i].sx_offset;
			ribbon_edges[slot] = (ribbon_edge_fn)get_relative_call(draws[i], ribbons[i].edge_calls[0]);
		}

		for (size_t i = 0; i < count; ++i)
		{
			for (uint32_t call : ribbons[i].edge_calls) replace_call(draws[i] + call, (void *)hooks[ribbon_slots_used + i]);
		}

		ribbon_slots_used += count;
	}
}
