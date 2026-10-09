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

// Boss battle transition: the PC draws 4 additive copies of a single screen
// capture, the PS1 draws them from the previous frame, leaving trails.
// We capture the screen again after each frame to get the PS1 feedback back.
// Parameters stack frame after frame, so they are per-frame increments.

#include "boss_transition.h"

#include "../../ff8.h"
#include "../../patch.h"
#include "../../globals.h"
#include "../../common.h"
#include "../../log.h"

#include <algorithm>
#include <math.h>
#include <stdint.h>

// FFNx replacement of the game's screen capture (ff8_opengl.cpp)
void swirl_sub_56D390(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

// Transformed and lit vertex, as written by the game in the capture graphics object
struct ff8_boss_transition_vertex
{
	float x, y, z, rhw;
	uint32_t color;
	uint32_t specular;
	float u, v;
};

constexpr int BOSS_TRANSITION_LAST_DRAWN_FRAME = 80;
constexpr float BOSS_TRANSITION_SPLIT_FACTOR = 0.1f;
constexpr float BOSS_TRANSITION_GAIN = 0.03f;

static void ff8_boss_transition_draw(int frame, float t, ff8_game_obj *game_object)
{
	const float x0 = float(*ff8_externals.boss_battle_transition_x);
	const float y0 = float(*ff8_externals.boss_battle_transition_y);
	const float w = float(*ff8_externals.boss_battle_transition_w);
	const float h = float(*ff8_externals.boss_battle_transition_h);

	// Split between the copies this frame (fraction of the screen size)
	const float split = sinf(3.14159265f * t) * t / 3.0f * BOSS_TRANSITION_SPLIT_FACTOR;
	// Zoom this frame: cover the split, and reach at least the PC 1.5x over the whole transition
	const float zoom = 1.0f + std::max(split, logf(1.5f) / float(BOSS_TRANSITION_LAST_DRAWN_FRAME));
	const uint32_t level = uint32_t(255.0f / 4.0f * (1.0f + BOSS_TRANSITION_GAIN * t) + 0.5f);
	const uint32_t color = 0xFF000000 | (level << 16) | (level << 8) | level;

	const float zw = w * zoom, zh = h * zoom;
	const float dx = split * w, dy = split * h;
	const float left = x0 - (zw - w) / 2.0f - dx / 2.0f;
	const float top = y0 - (zh - h) / 2.0f - dy / 2.0f;

	ff8_graphics_object *graphics_object = ff8_externals.boss_battle_transition_get_graphics_object();

	ff8_externals.graphics_object_alloc_shapes(4, graphics_object);

	for (int row = 0; row < 2; ++row)
	{
		for (int col = 0; col < 2; ++col)
		{
			const float qx = left + col * dx, qy = top + row * dy;
			ff8_boss_transition_vertex *vertices = (ff8_boss_transition_vertex *)graphics_object->field_74;
			const float corners[4][2] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f} };

			for (int i = 0; i < 4; ++i)
			{
				vertices[i].x = qx + corners[i][0] * zw;
				vertices[i].y = qy + corners[i][1] * zh;
				vertices[i].z = 0.0015f;
				vertices[i].rhw = 1.0f;
				vertices[i].color = color;
				vertices[i].u = corners[i][0];
				vertices[i].v = corners[i][1];
			}

			graphics_object->field_74 += graphics_object->vertex_offset;
		}
	}

	ff8_externals.graphics_object_sub_41E752(1, game_object);
	ff8_externals.gfx_begin_end_scene_alternative(1, game_object);
	ff8_externals.gfx_set_renderstate(0xE, 1, game_object);
	ff8_externals.graphics_object_draw(graphics_object, game_object);

	// Feedback: the next frame samples what was just drawn
	swirl_sub_56D390(uint32_t(x0), uint32_t(y0), uint32_t(w), uint32_t(h));

	if (trace_all) ffnx_trace("%s: frame=%d t=%f split=%f zoom=%f level=0x%X\n", __func__, frame, t, split, zoom, level);
}

void ff8_battle_boss_transition_init()
{
	replace_call(ff8_externals.boss_battle_transition + 0xC1, ff8_boss_transition_draw);
}
