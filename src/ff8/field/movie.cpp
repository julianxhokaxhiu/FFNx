/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2026 AxlRose                                            //
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

#include "movie.h"
#include "../../globals.h"
#include "../../movies.h"
#include "../../patch.h"

#include <algorithm>
#include <vector>

// During a movie, the field logic runs once per movie frame with timings made for 15 FPS movies:
// the MOVIE opcode doubles move and animation speeds, and the field loop runs the game time twice
// as often. With faster movies the logic keeps running once per 15 FPS frame, and every movie frame
// draws the models on their way between the last two logic steps.

static_assert(sizeof(ff8_field_state_other) == 0x264);
static_assert(sizeof(ff8_field_fade) == 0x18);

struct ff8_field_movie_model_state
{
	int32_t x, y, z;
	uint8_t direction;
	void *animation;
	int16_t animation_frame;
};

static std::vector<ff8_field_movie_model_state> step_start, step_scripts_end, step_end;
static int step_15fps_frame = -1;
static WORD step_field_id = 0;
static bool logic_runs = true;
static uint32_t keys_pressed = 0;

static bool is_faster_movie_playing()
{
	return ff8_externals.movie_object->movie_is_playing && movie_fps_ratio > 1;
}

static int get_15fps_frame()
{
	return ff8_externals.movie_object->movie_current_frame / movie_fps_ratio;
}

// Once per 15 FPS frame during faster movies, and right away on a new field
static bool is_logic_frame()
{
	return !is_faster_movie_playing() || get_15fps_frame() != step_15fps_frame || *common_externals.current_field_id != step_field_id;
}

static ff8_field_model_data *get_model(const ff8_field_state_other &entity)
{
	// Same condition the game uses before animating the model
	if (entity.common.execution_flags & 4) return nullptr;

	return (ff8_field_model_data *)ff8_externals.dword_1DCB340[entity.model_index];
}

static void save_model_states(const ff8_field_state_other *entities, std::vector<ff8_field_movie_model_state> &states)
{
	states.clear();

	for (int i = 0; i < *ff8_externals.field_state_other_count; ++i)
	{
		const ff8_field_model_data *model = get_model(entities[i]);

		states.push_back({ entities[i].position_x, entities[i].position_y, entities[i].position_z, entities[i].direction, model ? model->animation : nullptr, model ? model->animation_frame : int16_t(0) });
	}
}

static void set_model_state(ff8_field_state_other &entity, const ff8_field_movie_model_state &state)
{
	entity.position_x = state.x;
	entity.position_y = state.y;
	entity.position_z = state.z;
	entity.direction = state.direction;

	if (ff8_field_model_data *model = get_model(entity)) model->animation_frame = state.animation_frame;
}

// Where a model is drawn on the movie frame `step` (1 to movie_fps_ratio) of a logic step, the last one is where the logic left it
static ff8_field_movie_model_state get_model_state_between(const ff8_field_movie_model_state &start, const ff8_field_movie_model_state &scripts_end, const ff8_field_movie_model_state &end, int step)
{
	ff8_field_movie_model_state ret = end;
	auto between = [&](int from, int to) { return int(from + int64_t(to - from) * step / movie_fps_ratio); };

	// Positions and directions set by scripts (SET, DIR...) jump, only moves and turns are drawn in between
	if (scripts_end.x == start.x && scripts_end.y == start.y && scripts_end.z == start.z)
	{
		ret.x = between(start.x, end.x);
		ret.y = between(start.y, end.y);
		ret.z = between(start.z, end.z);
	}

	if (scripts_end.direction == start.direction)
	{
		ret.direction = uint8_t(start.direction + int8_t(end.direction - start.direction) * step / movie_fps_ratio);
	}

	// A new or looping animation jumps
	if (end.animation == start.animation && end.animation_frame > start.animation_frame)
	{
		ret.animation_frame = int16_t(between(start.animation_frame, end.animation_frame));
	}

	return ret;
}

// Moves the models to where they are drawn on this movie frame, until move_models_back
static bool move_models_for_drawing(ff8_field_state_other *entities)
{
	const size_t count = *ff8_externals.field_state_other_count;

	if (!is_faster_movie_playing() || step_start.size() != count || step_scripts_end.size() != count) return false;

	const int step = std::clamp(ff8_externals.movie_object->movie_current_frame - movie_fps_ratio * step_15fps_frame + 1, 1, int(movie_fps_ratio));

	save_model_states(entities, step_end);

	for (size_t i = 0; i < count; ++i) set_model_state(entities[i], get_model_state_between(step_start[i], step_scripts_end[i], step_end[i], step));

	return true;
}

static void move_models_back(ff8_field_state_other *entities)
{
	for (size_t i = 0; i < step_end.size(); ++i) set_model_state(entities[i], step_end[i]);
}

static void ff8_field_movie_update_entities(int drawbuf)
{
	logic_runs = is_logic_frame();

	// Scripts only see the keys pressed on their own frame (KEYON, closing messages), so they get all of them since the last step
	keys_pressed |= *ff8_externals.field_keys_pressed;

	if (!logic_runs) return;

	if (*common_externals.current_field_id == step_field_id) *ff8_externals.field_keys_pressed = keys_pressed;

	const bool faster_movie = is_faster_movie_playing();

	keys_pressed = 0;
	step_15fps_frame = faster_movie ? get_15fps_frame() : -1;
	step_field_id = *common_externals.current_field_id;

	if (faster_movie) save_model_states(*ff8_externals.field_state_others, step_start);
	else step_start.clear();

	((void(*)(int))common_externals.update_field_entities)(drawbuf);

	if (faster_movie) save_model_states(*ff8_externals.field_state_others, step_scripts_end);
}

static void ff8_field_movie_update_movement(ff8_field_state_other *entities, uint32_t keys)
{
	if (logic_runs) ((void(*)(ff8_field_state_other *, uint32_t))ff8_externals.sub_4789A0)(entities, keys);
}

static void ff8_field_movie_update_shake(void *shake)
{
	if (logic_runs) ((void(*)(void *))ff8_externals.field_shake_sub_4765C0)(shake);
}

static void ff8_field_movie_update_scroll()
{
	if (logic_runs) ((void(*)())ff8_externals.field_scroll_sub_475E60)();
}

static void ff8_field_movie_draw_models(ff8_field_state_other *entities, int drawbuf)
{
	const bool moved = move_models_for_drawing(entities);

	((void(*)(ff8_field_state_other *, int))ff8_externals.sub_472B30)(entities, drawbuf);

	if (moved) move_models_back(entities);
}

static void ff8_field_movie_draw_shadows(int drawbuf, int a2, int a3, int a4, ff8_field_state_other *entities)
{
	const bool moved = move_models_for_drawing(entities);

	((void(*)(int, int, int, int, ff8_field_state_other *))ff8_externals.field_draw_shadows_sub_472F20)(drawbuf, a2, a3, a4, entities);

	if (moved) move_models_back(entities);
}

static void ff8_field_movie_update_fade(int drawbuf)
{
	// It draws the fade too, so it runs every frame, but the fade only moves on with the logic
	const ff8_field_fade fade = *ff8_externals.field_fade;

	((void(*)(int))ff8_externals.field_fade_sub_47B980)(drawbuf);

	if (!logic_runs) *ff8_externals.field_fade = fade;
}

static void ff8_field_movie_update_game_time()
{
	// Called before update_field_entities sets logic_runs for this frame
	if (is_logic_frame()) ((void(*)())ff8_externals.update_game_time_sub_4701B0)();
}

void ff8_field_movie_init()
{
	replace_call(ff8_externals.sub_4767B0 + (JP_VERSION ? 0x14F : 0x14E), ff8_field_movie_update_entities);
	replace_call(ff8_externals.sub_4767B0 + (JP_VERSION ? 0x412 : 0x40F), ff8_field_movie_update_movement);
	replace_call(ff8_externals.sub_4767B0 + (JP_VERSION ? 0x449 : 0x446), ff8_field_movie_update_shake);
	replace_call(ff8_externals.sub_4767B0 + (JP_VERSION ? 0x453 : 0x450), ff8_field_movie_update_shake);
	replace_call(ff8_externals.sub_4767B0 + (JP_VERSION ? 0x460 : 0x45D), ff8_field_movie_update_scroll);
	replace_call(ff8_externals.sub_4767B0 + (JP_VERSION ? 0x4CC : 0x4C9), ff8_field_movie_draw_models);
	replace_call(ff8_externals.sub_4767B0 + (JP_VERSION ? 0x4F3 : 0x4F0), ff8_field_movie_draw_shadows);
	replace_call(ff8_externals.sub_4767B0 + (JP_VERSION ? 0x913 : 0x920), ff8_field_movie_update_fade);

	// The field loop calls it twice per frame, and 4 times during movies to keep its 15 FPS movie pace
	for (uint32_t offset : { 0x119, 0x11E, 0x12C, 0x131 }) replace_call(ff8_externals.field_main_loop + offset, ff8_field_movie_update_game_time);
}
