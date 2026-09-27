/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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
#include "../../cfg.h"
#include "../../common.h"
#include "../../globals.h"
#include "../../patch.h"
#include "../../log.h"

#include <string.h>
#include <windows.h>

// All addresses: FF8_EN.exe 1.2 (FF8 2000 / Steam English). See the FF8ModdingWiki page
// "Battle UI Timing and Input Sampling" for the battle frame anatomy.
//
// One battle frame (battle_cardgame_main_loop, 15 fps):
//   3 hidden UI ticks -> pad read (Input_ProcessInput) -> battle logic -> visible UI tick -> wait
// Every UI tick advances the battle pad ring from the engine pad mask (Input_GetPadState):
// the hidden ticks run before the frame's pad read and all see the previous frame's reading.
// The 3 extra readings are taken during the frame wait, so the next frame's hidden ticks get
// readings 16 ms apart and the visible tick gets the regular one: PlayStation pad timing.

#define FF8_BATTLE_UI_DISPLAY               0x4A84E0 // isBattle_HUDdisplay: UI tick part 2 (pad ring advance)
#define FF8_INPUT_PROCESS                   0x467D10 // Input_ProcessInput: keyboard/mouse/joystick read
#define FF8_BATTLE_GF_BOOST                 0x56DD70 // computeGFBoost_: GF Boost gauge, once per UI tick
#define FF8_READ_PAD_PRESSED_REMAPPED       0x4A8420 // press edges of the current UI tick

#define FF8_BATTLE_UI_MENU_RENDERING        (*(uint32_t *)0x1D6D4AC) // 0 on the hidden catch-up UI ticks
#define FF8_BATTLE_UI_CTX                   (*(uint8_t **)0x1D6D490) // battle UI context
#define FF8_INPUT_PAD_BLOCKS                0x1CD01F8u               // per pad (112 bytes): +0 edges, +4 current, +12 previous
#define FF8_INPUT_PAD_STRIDE                112
#define FF8_INPUT_PAD_CURRENT(pad)          (*(uint32_t *)(FF8_INPUT_PAD_BLOCKS + 4 + FF8_INPUT_PAD_STRIDE * (pad)))
#define FF8_INPUT_AUTOREPEAT_INTERVAL       (*(uint32_t *)0x1CD02F0) // 0 = engine auto-repeat off
#define FF8_INPUT_RAW_JOY_WORDS             0x1CD0394u               // edges / current / previous button words, 8 devices each
#define FF8_INPUT_RAW_JOY_WORDS_SIZE        (3 * 8 * 4)
#define FF8_BATTLE_GF_BOOST_VALUE           (*(uint16_t *)0x209CEF0) // current Boost (75..250, 0 = untouched = 100)
#define FF8_BATTLE_GF_BOOST_SAFE_PHASE      (*(uint8_t *)0x209CEF9)  // 1 = presses raise the Boost, 0 = they reset it
#define FF8_BATTLE_GF_BOOST_STATE           (*(uint8_t *)0x209CEFB)  // 4 = gauge running, 6 = done

#define PAD_SAMPLING_HIDDEN_TICKS 3

static bool pad_sampling_enabled = false;
static uint32_t pad_sampling_display_ri = 0, pad_sampling_boost_ri = 0;

// Readings taken during the last frame wait, for the next frame's hidden UI ticks
static struct { uint32_t pad[2]; bool valid; } pad_samples[PAD_SAMPLING_HIDDEN_TICKS];
static int pad_hidden_tick = 0;

bool ff8_battle_pad_sampling_enabled() { return pad_sampling_enabled; }

// Scope in which a function hooked with replace_function can call its original
struct pad_sampling_unhooked
{
	uint32_t ri;
	pad_sampling_unhooked(uint32_t replacement) : ri(replacement) { unreplace_function(ri); }
	~pad_sampling_unhooked() { rereplace_function(ri); }
};

int ff8_battle_pad_sampling_extra_reads(uint32_t driver_mode)
{
	return (pad_sampling_enabled && driver_mode == MODE_BATTLE) ? PAD_SAMPLING_HIDDEN_TICKS : 0;
}

// Takes a reading with the engine's own pad read, then puts back everything that read
// changed (pad masks and their press edges, raw joystick words; the engine auto-repeat is
// off meanwhile), so the frame's regular read still behaves exactly as in the original game.
void ff8_battle_pad_sampling_read(int index)
{
	if (index < 0 || index >= PAD_SAMPLING_HIDDEN_TICKS) return;

	pad_samples[index].valid = false;

	struct ff8_game_obj *game_object = (ff8_game_obj *)common_externals.get_game_object();

	if (!ff8_always_capture_input && game_object->hwnd != GetActiveWindow()) return;

	uint8_t pad_blocks[2 * FF8_INPUT_PAD_STRIDE], joy_words[FF8_INPUT_RAW_JOY_WORDS_SIZE];
	uint32_t autorepeat = FF8_INPUT_AUTOREPEAT_INTERVAL;

	memcpy(pad_blocks, (void *)FF8_INPUT_PAD_BLOCKS, sizeof(pad_blocks));
	memcpy(joy_words, (void *)FF8_INPUT_RAW_JOY_WORDS, sizeof(joy_words));
	FF8_INPUT_AUTOREPEAT_INTERVAL = 0;

	((void *(*)())FF8_INPUT_PROCESS)();

	pad_samples[index].pad[0] = FF8_INPUT_PAD_CURRENT(0);
	pad_samples[index].pad[1] = FF8_INPUT_PAD_CURRENT(1);
	pad_samples[index].valid = true;

	FF8_INPUT_AUTOREPEAT_INTERVAL = autorepeat;
	memcpy((void *)FF8_INPUT_RAW_JOY_WORDS, joy_words, sizeof(joy_words));
	memcpy((void *)FF8_INPUT_PAD_BLOCKS, pad_blocks, sizeof(pad_blocks));
}

// UI tick part 2: the n-th hidden tick of a frame advances the pad ring from the n-th reading
// of the last frame wait; the visible tick uses the frame's regular read and ends the frame.
static int __cdecl pad_sampling_display_hook()
{
	int r;

	if (FF8_BATTLE_UI_MENU_RENDERING == 0)
	{
		int index = pad_hidden_tick++;

		if (index < PAD_SAMPLING_HIDDEN_TICKS && pad_samples[index].valid)
		{
			uint32_t current[2] = { FF8_INPUT_PAD_CURRENT(0), FF8_INPUT_PAD_CURRENT(1) };

			FF8_INPUT_PAD_CURRENT(0) = pad_samples[index].pad[0];
			FF8_INPUT_PAD_CURRENT(1) = pad_samples[index].pad[1];
			{ pad_sampling_unhooked u(pad_sampling_display_ri); r = ((int (__cdecl *)())FF8_BATTLE_UI_DISPLAY)(); }
			FF8_INPUT_PAD_CURRENT(0) = current[0];
			FF8_INPUT_PAD_CURRENT(1) = current[1];

			return r;
		}
	}
	else
	{
		pad_hidden_tick = 0;
		for (auto &sample : pad_samples) sample.valid = false;
	}

	{ pad_sampling_unhooked u(pad_sampling_display_ri); r = ((int (__cdecl *)())FF8_BATTLE_UI_DISPLAY)(); }

	return r;
}

// One FFNx.log line per GF Boost: the Square presses the gauge saw, so the input rate a
// player (or a turbo button) actually gets through can be checked, and on which UI tick of
// the frame each press arrived (vanilla: always the last one, right after the frame's read).
static struct { uint32_t ticks, safe, danger, by_tick[PAD_SAMPLING_HIDDEN_TICKS + 1]; } pad_boost_stats;

static void __cdecl pad_sampling_boost_hook()
{
	uint8_t *ctx = FF8_BATTLE_UI_CTX;
	uint8_t state = FF8_BATTLE_GF_BOOST_STATE;

	if (state <= 1) memset(&pad_boost_stats, 0, sizeof(pad_boost_stats));
	if (state == 4 && ctx && (ctx[30] & 1)) // gauge running, Boost ability on
	{
		pad_boost_stats.ticks++;
		if (((int (__cdecl *)(int))FF8_READ_PAD_PRESSED_REMAPPED)(0) & 0x80) // Square press edge
		{
			if (FF8_BATTLE_GF_BOOST_SAFE_PHASE) pad_boost_stats.safe++;
			else pad_boost_stats.danger++;

			// hidden ticks are counted by the display hook before they run
			int tick = FF8_BATTLE_UI_MENU_RENDERING == 0 ? pad_hidden_tick - 1 : PAD_SAMPLING_HIDDEN_TICKS;
			if (tick >= 0 && tick <= PAD_SAMPLING_HIDDEN_TICKS) pad_boost_stats.by_tick[tick]++;
		}
	}

	{ pad_sampling_unhooked u(pad_sampling_boost_ri); ((void (__cdecl *)())FF8_BATTLE_GF_BOOST)(); }

	if (state != 6 && FF8_BATTLE_GF_BOOST_STATE == 6 && pad_boost_stats.ticks)
	{
		const auto &b = pad_boost_stats;
		double seconds = b.ticks / 60.0;
		uint16_t boost = FF8_BATTLE_GF_BOOST_VALUE ? FF8_BATTLE_GF_BOOST_VALUE : 100;

		ffnx_info("battle pad sampling: GF Boost: %u Square presses in %.1f s (%.1f per second: %u during safe phases, %u during danger phases), Boost %u; presses per UI tick: %u / %u / %u (extra reads) %u (regular read)\n",
			b.safe + b.danger, seconds, (b.safe + b.danger) / seconds, b.safe, b.danger, boost,
			b.by_tick[0], b.by_tick[1], b.by_tick[2], b.by_tick[3]);
	}
}

// Code and data references checked before anything is patched
struct pad_sampling_signature { uint32_t addr; uint8_t bytes[5]; };

static const pad_sampling_signature pad_sampling_signatures[] = {
	{ FF8_BATTLE_UI_DISPLAY,         { 0x57, 0x33, 0xFF, 0x57, 0xE8 } },
	{ FF8_INPUT_PROCESS,             { 0x83, 0xEC, 0x08, 0x53, 0x55 } },
	{ FF8_BATTLE_GF_BOOST,           { 0x83, 0xEC, 0x08, 0x8A, 0x0D } },
	{ FF8_READ_PAD_PRESSED_REMAPPED, { 0x8B, 0x44, 0x24, 0x04, 0x6A } },
	{ 0x468511,                      { 0x81, 0xFC, 0x01, 0xCD, 0x01 } }, // Input_GetPadState: current mask
	{ 0x467DCE,                      { 0x88, 0x04, 0x02, 0xCD, 0x01 } }, // Input_ProcessInput: previous mask
	{ 0x4681A7,                      { 0x90, 0xF8, 0x01, 0xCD, 0x01 } }, // Input_ProcessInput: press edges
	{ 0x4681AC,                      { 0xA1, 0xF0, 0x02, 0xCD, 0x01 } }, // Input_ProcessInput: auto-repeat interval
	{ 0x4687A0,                      { 0xB8, 0xB4, 0x03, 0xCD, 0x01 } }, // Input_UpdateRawJoyButtonWords
};

bool ff8_battle_pad_sampling_init()
{
	if (pad_sampling_enabled) return true;

	// The Steam executable is detected as the Nvidia variant: every English 1.2 build is
	// accepted, the code signatures decide
	bool supported = FF8_US_VERSION && !ff8_remastered_edition;

	for (const pad_sampling_signature &s : pad_sampling_signatures)
	{
		if (!supported) break;

		if (memcmp((const void *)s.addr, s.bytes, sizeof(s.bytes)) != 0)
		{
			ffnx_warning("battle pad sampling: unexpected code at 0x%X\n", s.addr);
			supported = false;
		}
	}

	if (!supported)
	{
		ffnx_warning("battle pad sampling: requires FF8 2000 / Steam English 1.2, not installed\n");
		return false;
	}

	pad_sampling_display_ri = replace_function(FF8_BATTLE_UI_DISPLAY, (void *)pad_sampling_display_hook);
	pad_sampling_boost_ri = replace_function(FF8_BATTLE_GF_BOOST, (void *)pad_sampling_boost_hook);

	pad_sampling_enabled = true;

	ffnx_info("battle pad sampling: pad read 60 times per second in battle (battle unchanged, 15 fps)\n");

	return true;
}
