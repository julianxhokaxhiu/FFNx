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

// The PC GTE returns SX/SY in eighths of a pixel, the PSX returned pixels.
// These effects compare them with pixel values, so they get pixels back here (or the limits go to eighths).

#include "effects_screen_units.h"
#include "effects.h"

#include "../../ff8.h"
#include "../../patch.h"
#include "../../globals.h"

#include <algorithm>
#include <cstdlib>
#include <stdint.h>

// Call sites in the overlay renderer shared by Cure, Cura, Potion and Hi-Potion (projector pass)
#define OVERLAY_TRI_READ_SXY012 0x414
#define OVERLAY_TRI_LIGHT_INSERT 0x4E1
#define OVERLAY_TRI_MODEL_INSERT 0x541
#define OVERLAY_QUAD_READ_SXY012 0x729
#define OVERLAY_QUAD_READ_SXY2 0x74A
#define OVERLAY_QUAD_LIGHT_INSERT 0x831
#define OVERLAY_QUAD_MODEL_INSERT 0x898
#define HYPNOTIZE_MAIN_READ_SXY2 0x4C

// Gilgamesh: the timeline task starts the attack task from a table (Zantetsuken, Masamune, Excaliber, Excalipoor)
#define GILGAMESH_ATTACK_TASKS 0x5B3
#define GILGAMESH_MASAMUNE_TASK 1
#define MASAMUNE_READ_SXY2 0xA3
// Eden: the draw functions are called from a table, number 53 draws the pixel particles
#define EDEN_DRAW_FUNCTIONS 0xA6
#define EDEN_PIXEL_PARTICLE_DRAW 53
#define EDEN_PIXEL_PARTICLE_PROJECT 0x5B0
// Magic Summon: the vertex warp and what it calls
#define MAGIC_SUMMON_WARP_VERTICES 0x99
#define WARP_LOAD_VECTOR 0x32
#define WARP_PROJECT 0x37
#define WARP_READ_OTZ 0x50
#define WARP_DISTANCES 0xD4

static int16_t *(*gte_read_sxy012)(int16_t *) = nullptr;
static int16_t *(*gte_read_sxy2)(int16_t *) = nullptr;
static void *(*insert_prim_auto_depth)(void *, void *) = nullptr;
static void *(*insert_prim_depth_keys)(void *, void *, int, int, int, int) = nullptr;
static int (*project_point)(void *, int16_t *, void *, void *) = nullptr;
static void (*gte_load_vector)(void *) = nullptr;
static void (*gte_project)() = nullptr;
static void (*gte_read_otz)(void *) = nullptr;
static const uint8_t *warp_distances = nullptr;

static void *pending_light_ot = nullptr;
static void *pending_light = nullptr;

static void sxy_to_pixels(int16_t *sxy, int count)
{
	for (int i = 0; i < count * 2; ++i) sxy[i] = int16_t(sxy[i] >> 3);
}

static int16_t *read_sxy012_pixels(int16_t *out)
{
	gte_read_sxy012(out);
	sxy_to_pixels(out, 3);
	return out;
}

static int16_t *read_sxy2_pixels(int16_t *out)
{
	gte_read_sxy2(out);
	sxy_to_pixels(out, 1);
	return out;
}

// Eden's pixel particles: the projected point goes into a dot placed in pixels
static int project_point_pixels(void *vector, int16_t *sxy, void *p, void *flag)
{
	int ret = project_point(vector, sxy, p, flag);
	sxy_to_pixels(sxy, 1);
	return ret;
}

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
		gte_read_sxy2(&warp->sx);
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
static uint32_t cure_overlay_render(uint32_t effect)
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
static uint32_t cura_overlay_render(uint32_t effect)
{
	uint32_t tick = get_absolute_value(effect, 0x28);
	uint32_t spawn_emitter = get_absolute_value(tick, 0x14);
	uint32_t emitter = get_absolute_value(spawn_emitter, 0x9);
	uint32_t spawn_overlay = get_absolute_value(emitter, 0x15);
	uint32_t overlay = get_absolute_value(spawn_overlay, 0x21);
	uint32_t draw = get_relative_call(overlay, 0xCE);

	return get_relative_call(draw, 0xA9);
}

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

static void patch_chocobocle_limits(uint32_t dispatch)
{
	for (const chocobocle_renderer &renderer : chocobocle_renderers)
	{
		const uint32_t render = get_relative_call(dispatch, renderer.call);

		for (uint32_t offset : renderer.x_limits) if (offset != 0) patch_code_word(render + offset, 0x140 * 8);
		for (uint32_t offset : renderer.y_limits) if (offset != 0) patch_code_word(render + offset, 0xD8 * 8);
	}
}

static void patch_overlay_render(uint32_t render)
{
	replace_call(render + OVERLAY_TRI_READ_SXY012, read_sxy012_pixels);
	replace_call(render + OVERLAY_TRI_LIGHT_INSERT, defer_light_insert);
	replace_call(render + OVERLAY_TRI_MODEL_INSERT, insert_model_then_light);
	replace_call(render + OVERLAY_QUAD_READ_SXY012, read_sxy012_pixels);
	replace_call(render + OVERLAY_QUAD_READ_SXY2, read_sxy2_pixels);
	replace_call(render + OVERLAY_QUAD_LIGHT_INSERT, defer_light_insert);
	replace_call(render + OVERLAY_QUAD_MODEL_INSERT, insert_model_then_light);
}

void ff8_battle_effects_screen_units_init()
{
	if (!FF8_US_VERSION) return;

	const DWORD *effects = ff8_externals.func_off_battle_effects_C81774;
	const uint32_t cure = cure_overlay_render(effects[FF8BattleEffect::Cure]);
	const uint32_t potion = cure_overlay_render(effects[FF8BattleEffect::Potion]);
	const uint32_t cura = cura_overlay_render(effects[FF8BattleEffect::Cura]);
	const uint32_t hi_potion = cura_overlay_render(effects[FF8BattleEffect::HiPotion]);
	const uint32_t hypnotize_master = get_absolute_value(get_relative_call(effects[FF8BattleEffect::Hypnotize], 0x5), 0x4C);
	const uint32_t hypnotize_main = get_absolute_value(hypnotize_master, 0xC8);
	const uint32_t chocobocle = chocobocle_polygon_dispatch(effects[FF8BattleEffect::BokoChocobocle]);
	const uint32_t gilgamesh_timeline = get_absolute_value(effects[FF8BattleEffect::GilgameshMasamune], 0x8F);
	const uint32_t masamune = ((uint32_t *)get_absolute_value(gilgamesh_timeline, GILGAMESH_ATTACK_TASKS))[GILGAMESH_MASAMUNE_TASK];
	const uint32_t eden_draw = get_relative_call(get_absolute_value(effects[FF8BattleEffect::Eden], 0x45), 0x1D7);
	const uint32_t eden_pixel_particles = ((uint32_t *)get_absolute_value(eden_draw, EDEN_DRAW_FUNCTIONS))[EDEN_PIXEL_PARTICLE_DRAW];
	uint32_t magic_summon = get_relative_call(effects[FF8BattleEffect::MagicSummon], 0x5);
	magic_summon = get_absolute_value(get_absolute_value(magic_summon, 0x4C), 0x72);
	magic_summon = get_relative_call(get_relative_call(magic_summon, 0x1D7), 0xFC);
	const uint32_t warp_vertices = get_relative_call(magic_summon, MAGIC_SUMMON_WARP_VERTICES);

	// Resolve the GTE and SSIGPU helpers from Cure before anything is patched
	gte_read_sxy012 = (int16_t *(*)(int16_t *))get_relative_call(cure, OVERLAY_TRI_READ_SXY012);
	gte_read_sxy2 = (int16_t *(*)(int16_t *))get_relative_call(cure, OVERLAY_QUAD_READ_SXY2);
	insert_prim_auto_depth = (void *(*)(void *, void *))get_relative_call(cure, OVERLAY_TRI_LIGHT_INSERT);
	insert_prim_depth_keys = (void *(*)(void *, void *, int, int, int, int))get_relative_call(cure, OVERLAY_TRI_MODEL_INSERT);
	project_point = (int (*)(void *, int16_t *, void *, void *))get_relative_call(eden_pixel_particles, EDEN_PIXEL_PARTICLE_PROJECT);
	gte_load_vector = (void (*)(void *))get_relative_call(warp_vertices, WARP_LOAD_VECTOR);
	gte_project = (void (*)())get_relative_call(warp_vertices, WARP_PROJECT);
	gte_read_otz = (void (*)(void *))get_relative_call(warp_vertices, WARP_READ_OTZ);
	warp_distances = (const uint8_t *)get_absolute_value(warp_vertices, WARP_DISTANCES);

	patch_overlay_render(cure);
	patch_overlay_render(potion);
	patch_overlay_render(cura);
	patch_overlay_render(hi_potion);

	replace_call(hypnotize_main + HYPNOTIZE_MAIN_READ_SXY2, read_sxy2_pixels);

	patch_chocobocle_limits(chocobocle);

	replace_call(masamune + MASAMUNE_READ_SXY2, read_sxy2_pixels);
	replace_call(eden_pixel_particles + EDEN_PIXEL_PARTICLE_PROJECT, project_point_pixels);
	replace_function(warp_vertices, magic_summon_warp_vertices);
}
