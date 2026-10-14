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

// Boss battle transition: the PC draws 4 identical additive copies of the screen
// captured at the battle start. On PS1 the copies are mirrored (kaleidoscope),
// slide apart and leave a short trail. Values matched against a PS1 capture.

#include "boss_transition.h"

#include "../../ff8.h"
#include "../../patch.h"
#include "../../globals.h"
#include "../../common.h"
#include "../../log.h"

#include <algorithm>
#include <math.h>
#include <stdint.h>

// Transformed and lit vertex, as written by the game in the capture graphics object
struct ff8_boss_transition_vertex
{
	float x, y, z, rhw;
	uint32_t color;
	uint32_t specular;
	float u, v;
};

constexpr int BOSS_TRANSITION_FRAMES = 80;
constexpr int BOSS_TRANSITION_ECHOES = 4;
constexpr float BOSS_TRANSITION_ECHO_DECAY = 0.6f;
constexpr float BOSS_TRANSITION_GAIN = 0.6f;

static float ff8_boss_transition_smooth(float x)
{
	x = std::clamp(x, 0.0f, 1.0f);

	return x * x * (3.0f - 2.0f * x);
}

static void ff8_boss_transition_draw(int frame, float t, ff8_game_obj *game_object)
{
	const float x0 = float(*ff8_externals.boss_battle_transition_x);
	const float y0 = float(*ff8_externals.boss_battle_transition_y);
	const float w = float(*ff8_externals.boss_battle_transition_w);
	const float h = float(*ff8_externals.boss_battle_transition_h);
	const float corners[4][2] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f} };

	float echo_total = 0.0f;
	for (int echo = 0; echo < BOSS_TRANSITION_ECHOES; ++echo) echo_total += powf(BOSS_TRANSITION_ECHO_DECAY, float(echo));

	ff8_graphics_object *graphics_object = ff8_externals.boss_battle_transition_get_graphics_object();

	ff8_externals.graphics_object_alloc_shapes(4 * BOSS_TRANSITION_ECHOES, graphics_object);

	// Oldest echo first, the current frame last
	for (int echo = BOSS_TRANSITION_ECHOES - 1; echo >= 0; --echo)
	{
		const float f = float(std::max(frame - echo, 0));
		// Copies slide apart (mirror twins end at 1/4 and 3/4 of the screen), zoom up to 2x
		const float spread = ff8_boss_transition_smooth((f - 12.0f) / 18.0f);
		const float dx = 0.5f * spread * w, dy = 0.2f * spread * h;
		const float zoom = std::max(1.0f + f / BOSS_TRANSITION_FRAMES, 1.0f + 0.5f * spread);
		// The mirrored copies fade in over the plain one
		const float mirror = ff8_boss_transition_smooth(f / 20.0f);
		const float weight = (1.0f + BOSS_TRANSITION_GAIN * t) * powf(BOSS_TRANSITION_ECHO_DECAY, float(echo)) / echo_total;

		const float zw = w * zoom, zh = h * zoom;
		const float left = x0 - (zw - w) / 2.0f - dx / 2.0f;
		const float top = y0 - (zh - h) / 2.0f - dy / 2.0f;

		for (int row = 0; row < 2; ++row)
		{
			for (int col = 0; col < 2; ++col)
			{
				const float share = (row == 0 && col == 0) ? 1.0f - 0.75f * mirror : 0.25f * mirror;
				const uint32_t level = uint32_t(std::min(weight * share * 255.0f + 0.5f, 255.0f));
				const uint32_t color = 0xFF000000 | (level << 16) | (level << 8) | level;
				const float qx = left + col * dx, qy = top + row * dy;
				ff8_boss_transition_vertex *vertices = (ff8_boss_transition_vertex *)graphics_object->field_74;

				for (int i = 0; i < 4; ++i)
				{
					vertices[i].x = qx + corners[i][0] * zw;
					vertices[i].y = qy + corners[i][1] * zh;
					vertices[i].z = 0.0015f;
					vertices[i].rhw = 1.0f;
					vertices[i].color = color;
					// Right copies are mirrored horizontally, bottom copies vertically
					vertices[i].u = col ? 1.0f - corners[i][0] : corners[i][0];
					vertices[i].v = row ? 1.0f - corners[i][1] : corners[i][1];
				}

				graphics_object->field_74 += graphics_object->vertex_offset;
			}
		}
	}

	ff8_externals.graphics_object_sub_41E752(1, game_object);
	ff8_externals.gfx_begin_end_scene_alternative(1, game_object);
	ff8_externals.gfx_set_renderstate(0xE, 1, game_object);
	ff8_externals.graphics_object_draw(graphics_object, game_object);

	if (trace_all) ffnx_trace("%s: frame=%d t=%f\n", __func__, frame, t);
}

void ff8_battle_boss_transition_init()
{
	replace_call(ff8_externals.boss_battle_transition + 0xC1, ff8_boss_transition_draw);
}
