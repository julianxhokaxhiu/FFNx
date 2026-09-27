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

#include "pad_sampling.h"

#include "../../ff8.h"
#include "../../patch.h"
#include "../../globals.h"
#include "../../cfg.h"
#include "../../common.h"
#include "../../log.h"

#include <string.h>

// One battle frame (battle main loop, 15 fps):
//   3 catch-up UI ticks -> pad read -> battle logic -> visible UI tick -> frame wait
// Every UI tick advances the battle pad ring from the pad mask of the last pad read, so the
// 3 catch-up ticks, which run before the frame's pad read, all see the previous frame's
// reading. The pad is read 3 more times during the frame wait, at 1/4, 2/4 and 3/4 of the
// frame, and the next frame's catch-up ticks each use one of those readings.

#define PAD_SAMPLING_EXTRA_READS 3
#define PAD_SAMPLING_PAD_STRIDE 28 // DWORDs between the 2 pads' input state blocks

// Offsets in battle_main_loop of the 3 catch-up UI ticks (the "display" half of the pair)
static const uint32_t pad_sampling_catch_up_ticks[PAD_SAMPLING_EXTRA_READS] = { 0x142, 0x14C, 0x156 };

static struct
{
	DWORD pad[2];
	bool valid;
} pad_samples[PAD_SAMPLING_EXTRA_READS];

static bool pad_sampling_enabled = false;

// Statistics for trace_gamepad, reported once per battle
static struct
{
	uint32_t frames, presses[PAD_SAMPLING_EXTRA_READS + 1], reads[PAD_SAMPLING_EXTRA_READS];
	double work_ms, read_ms[PAD_SAMPLING_EXTRA_READS];
	DWORD last_read;
} pad_sampling_stats;

static int pad_sampling_count_presses(DWORD before, DWORD after)
{
	int count = 0;

	for (DWORD pressed = after & ~before; pressed; pressed &= pressed - 1) count++;

	return count;
}

int ff8_battle_pad_sampling_frame_end(uint32_t driver_mode, double frame_ms)
{
	if (!pad_sampling_enabled) return 0;

	auto &stats = pad_sampling_stats;

	if (driver_mode != MODE_BATTLE)
	{
		if (stats.frames && stats.reads[PAD_SAMPLING_EXTRA_READS - 1] && (trace_all || trace_gamepad))
		{
			ffnx_trace("battle pad sampling: %u presses this battle: %u / %u / %u from the extra reads, %u from the game's own read; frame work %.1f ms, extra reads at %.1f / %.1f / %.1f ms\n", stats.presses[0] + stats.presses[1] + stats.presses[2] + stats.presses[3], stats.presses[0], stats.presses[1], stats.presses[2], stats.presses[3], stats.work_ms / stats.frames, stats.read_ms[0] / stats.reads[0], stats.read_ms[1] / stats.reads[1], stats.read_ms[2] / stats.reads[2]);
		}

		memset(&stats, 0, sizeof(stats));
		for (auto &sample : pad_samples) sample.valid = false;

		return 0;
	}

	if (trace_all || trace_gamepad)
	{
		// Readings of this frame in time order: the extra reads of the last frame wait, then the
		// game's own read (still in the engine pad mask at the end of the frame)
		DWORD read = ff8_externals.engine_mapped_buttons[0];

		if (pad_samples[PAD_SAMPLING_EXTRA_READS - 1].valid)
		{
			DWORD before = stats.last_read;

			for (int i = 0; i < PAD_SAMPLING_EXTRA_READS; i++)
			{
				stats.presses[i] += pad_sampling_count_presses(before, pad_samples[i].pad[0]);
				before = pad_samples[i].pad[0];
			}
			stats.presses[PAD_SAMPLING_EXTRA_READS] += pad_sampling_count_presses(before, read);
			stats.work_ms += frame_ms;
			stats.frames++;
		}

		stats.last_read = read;
	}

	return PAD_SAMPLING_EXTRA_READS;
}

void ff8_battle_pad_sampling_read(int index, double frame_ms)
{
	if (index < 0 || index >= PAD_SAMPLING_EXTRA_READS) return;

	pad_samples[index].valid = false;

	struct ff8_game_obj *game_object = (ff8_game_obj *)common_externals.get_game_object();

	if (!ff8_always_capture_input && game_object->hwnd != GetActiveWindow()) return;

	// The game's pad read also updates the press edges and the previous state of each pad, the
	// raw gamepad button words and the auto-repeat: all of it is put back after the reading, so
	// the frame's own read still behaves exactly like in the original game
	DWORD *pad_state = ff8_externals.engine_mapped_buttons - 1; // edges, current, -, previous...
	DWORD *raw_buttons = ff8_externals.engine_raw_gamepad_buttons - 8; // edges[8], current[8], previous[8]
	DWORD saved_pad_state[2 * PAD_SAMPLING_PAD_STRIDE], saved_raw_buttons[3 * 8];
	DWORD autorepeat = *ff8_externals.engine_input_autorepeat_interval;

	memcpy(saved_pad_state, pad_state, sizeof(saved_pad_state));
	memcpy(saved_raw_buttons, raw_buttons, sizeof(saved_raw_buttons));
	*ff8_externals.engine_input_autorepeat_interval = 0;

	ff8_externals.engine_eval_keyboard_gamepad_input();

	pad_samples[index].pad[0] = ff8_externals.engine_mapped_buttons[0];
	pad_samples[index].pad[1] = ff8_externals.engine_mapped_buttons[PAD_SAMPLING_PAD_STRIDE];
	pad_samples[index].valid = true;

	*ff8_externals.engine_input_autorepeat_interval = autorepeat;
	memcpy(raw_buttons, saved_raw_buttons, sizeof(saved_raw_buttons));
	memcpy(pad_state, saved_pad_state, sizeof(saved_pad_state));

	pad_sampling_stats.read_ms[index] += frame_ms;
	pad_sampling_stats.reads[index]++;
}

// A catch-up UI tick, with the pad mask of its reading while it runs
static int pad_sampling_catch_up_tick(int index)
{
	int (*battle_ui_tick)() = (int (*)())ff8_externals.sub_4A84E0;

	if (!pad_samples[index].valid) return battle_ui_tick();

	DWORD *pad0 = &ff8_externals.engine_mapped_buttons[0], *pad1 = &ff8_externals.engine_mapped_buttons[PAD_SAMPLING_PAD_STRIDE];
	DWORD current0 = *pad0, current1 = *pad1;

	*pad0 = pad_samples[index].pad[0];
	*pad1 = pad_samples[index].pad[1];
	int ret = battle_ui_tick();
	*pad0 = current0;
	*pad1 = current1;

	return ret;
}

static int pad_sampling_catch_up_tick_1() { return pad_sampling_catch_up_tick(0); }
static int pad_sampling_catch_up_tick_2() { return pad_sampling_catch_up_tick(1); }
static int pad_sampling_catch_up_tick_3() { return pad_sampling_catch_up_tick(2); }

void ff8_battle_pad_sampling_init()
{
	if (ff8_remastered_edition)
	{
		ffnx_warning("battle pad sampling: not supported on the remastered edition, not enabled\n");
		return;
	}

	replace_call(ff8_externals.battle_main_loop + pad_sampling_catch_up_ticks[0], (void *)pad_sampling_catch_up_tick_1);
	replace_call(ff8_externals.battle_main_loop + pad_sampling_catch_up_ticks[1], (void *)pad_sampling_catch_up_tick_2);
	replace_call(ff8_externals.battle_main_loop + pad_sampling_catch_up_ticks[2], (void *)pad_sampling_catch_up_tick_3);

	pad_sampling_enabled = true;

	ffnx_info("battle pad sampling: pad read 60 times per second in battle\n");
}
