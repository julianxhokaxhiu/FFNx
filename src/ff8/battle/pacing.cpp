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

#include "pacing.h"

#include "../../ff8.h"
#include "../../cfg.h"
#include "../../common.h"
#include "../../globals.h"
#include "../../patch.h"
#include "../../log.h"

#include <string.h>
#include <windows.h>

// All addresses: FF8_EN.exe 1.2 (FF8 2000 / Steam English). See the FF8ModdingWiki page
// "Battle UI Timing and Input Sampling" for the frame anatomy this module relies on.

// ---------------------------------------------------------------------------
// Engine addresses
// ---------------------------------------------------------------------------

// Functions
#define FF8_BATTLE_BDLINK                   0x500900 // per-frame battle tick: task queues, effects, models, camera, draw
#define FF8_BATTLE_UI_UPDATE                0x4A8E30 // isBattle_HUDupdate: UI tick part 1 (fresh-input latch)
#define FF8_BATTLE_UI_DISPLAY               0x4A84E0 // isBattle_HUDdisplay: UI tick part 2 (pad ring, windows, ATB)
#define FF8_BATTLE_GF_BOOST                 0x56DD70 // computeGFBoost_: GF Boost gauge, once per UI tick
#define FF8_SAVEMAP_TICK_GAME_TIME          0x4701B0 // game time + countdown, 4 calls per battle loop iteration
#define FF8_BATTLE_READ_ANIMATION           0x508F90 // Battle_ReadAnimation: decodes one animation frame
#define FF8_BATTLE_BUILD_BONE_MATRICES      0x508C90 // bone matrices + geometry from the current pose
#define FF8_BATTLE_ANIMSEQ_UPDATE_ENTITY    0x504290 // AnimSeq_UpdateEntityPerFrame: choreography VM + animation
#define FF8_BATTLE_ANIM_ADVANCE_BY_1        0x5094F0 // AdvanceAnimationBy1AndCheckCompletion
#define FF8_BATTLE_CAMERA_ANIMATION         0x5035E0 // camera keyframe player (camera task)
#define FF8_BATTLE_CAMERA_SEQUENCE          0x509610 // camera script VM
#define FF8_BATTLE_CAMERA_OPERATIONS        0x5033E0 // applies then clears the camera shake offsets
#define FF8_BATTLE_REQUEST_SCREEN_FEEDBACK  0x47CF50 // one-frame "ghost of the screen" request
#define FF8_BATTLE_STATUS_TIMERS            0x483470 // computeTimerStatus
#define FF8_BATTLE_GILGAMESH_ANGELO         0x482F80 // Gilgamesh / Angelo auto-action countdown
#define FF8_INPUT_PROCESS                   0x467D10 // DirectInput keyboard/joystick read
#define FF8_SSIGPU_CLEAR_OT                 0x45D530 // clears an ordering table
// AnimSeq-spawned task ticks (TASK_QUEUE_ANIM_SEQ, executed every host frame)
#define FF8_BATTLE_TASK_84_WOBBLE           0x501F90 // sine wobble of a stage group
#define FF8_BATTLE_TASK_9F_TEXTURE_BLINK    0x5057D0 // texture toggle timeline
#define FF8_BATTLE_TASK_99_B1_FOOTSTEP      0x50F830 // footstep dust + step sounds
#define FF8_BATTLE_TASK_96_CAMERA_SHAKE     0x50F6C0 // camera shake
#define FF8_BATTLE_TASK_9E_MOVE             0x50F750 // run-up / walk-back move (time-parameterized lerp)
#define FF8_BATTLE_TASK_AD_AE_DRAG          0x50F500 // drag target to attacker bone
#define FF8_BATTLE_TASK_81_RESTORE_PART     0x50F0E0 // restore detached model part (draws in its tick)
#define FF8_BATTLE_TASK_A6_DETACH_PART      0x50F2E0 // detached model part with physics (draws in its tick)
#define FF8_BATTLE_DAMAGE_NUMBER_TASK       0x5069B0 // damage number popup (10 frames, draws in its tick)
#define FF8_BATTLE_TEXT_TASK                0x506F70 // battle message box display time
#define FF8_BATTLE_SCREEN_FADE_TASK         0x501D10 // fade to / from black (battle start / end)
#define FF8_BATTLE_SCREEN_FLASH_TASK        0x501E80 // full-screen flash
#define FF8_BATTLE_CAMERA_OSCILLATION_TASK  0x509930 // camera return-to-view blend ramp
#define FF8_BATTLE_STAGE_147_TASK           0x511EF0 // stage 147 scripted intro
#define FF8_BATTLE_STAGE_TEXTURE_ANIMATION  0x51B0D0 // stage texture animation (checks the freeze byte)
#define FF8_BATTLE_STAGE_RENDER             0x500FD0 // BS_RenderRelated: stage parts animation + draw
// Call site inside the battle loop
#define FF8_BATTLE_CALL_TEXT_MANAGEMENT     0x47D7FB // executeTextManagementBattleAction: AI text / waits / end fade trigger
// Call sites inside the per-entity task
#define FF8_BATTLE_CALL_STATUS_VISUALS      0x502B5D // status colour pulse, Float bob, spin, status sprites
#define FF8_BATTLE_CALL_ENTITY_FADES        0x502B74 // computeCommandRank: death / escape / appear fades
// Call site inside the stage 142 task
#define FF8_BATTLE_CALL_STAGE_142_FADE      0x5129A7 // palette fade
// Call sites inside BdLink
#define FF8_BATTLE_CALL_HIT_EFFECT_QUEUE    0x500923 // ExecuteTaskQueue(hit-effect queue)
#define FF8_BATTLE_CALL_EFFECT_TICK         0x50093A // ExecuteTaskQueue(C3_28_GF_data_pointer): active effect tree
#define FF8_BATTLE_CALL_STAGE_QUEUE         0x50097E // stage task queue (sky, texture animation, stage scripts)

// Data
#define FF8_BATTLE_UI_CTX                   (*(uint8_t **)0x1D6D490) // battle UI context (88 bytes)
#define FF8_BATTLE_UI_MENU_RENDERING        (*(uint32_t *)0x1D6D4AC) // 0 on the hidden catch-up UI ticks
#define FF8_BATTLE_UI_TICKS_PER_FRAME       0xB8A3E4                 // CONST_BattleUI_TicksPerFrame (4)
#define FF8_BATTLE_RENZOKUKEN_LATCH_STEP    0x4BA9AC                 // imm8 of "add word_1D76798, 4" (Renzokuken timeline)
#define FF8_INPUT_AUTOREPEAT_ENABLED        (*(uint32_t *)0x1CD02F0)
#define FF8_BATTLE_SCREEN_FEEDBACK_REQUEST  (*(int *)0x1CFF6F4)
#define FF8_BATTLE_CAMERA_SETTING           (*(void **)0x1D99A34)    // CURRENT_CAMERA_SETTING_ADDR
#define FF8_BATTLE_CAMERA_SHAKE             ((int16_t *)0x1D97710)   // x, y, z
#define FF8_BATTLE_UPDATE_FLAGS             (*(uint32_t *)0x1D96A9C) // bit 0: battle paused (tasks draw only)
#define FF8_BATTLE_DETACHED_PART_MATRIX     ((uint8_t *)0x1D99BF8)   // 32 bytes, rebuilt every A6 tick
#define FF8_BATTLE_RENDER_LIST_BASE         (*(uint32_t *)0x1D8E04C) // ordering table at +68
#define FF8_SSIGPU_EXEC_START               0x1C48828u               // SSIGPU execution node arena
#define FF8_SSIGPU_EXEC_CUR                 (*(uint32_t *)0x1CA8828)
#define FF8_SSIGPU_EXEC_LIMIT               0x60000u
#define FF8_BATTLE_OT_BUCKETS               4386
#define FF8_BATTLE_STAGE_FREEZE             (*(uint8_t *)0x1D9898C)  // 1 sky rotation, 2 texture animation, 8 stage scripts
#define FF8_BATTLE_TASKS_BUSY               (*(uint8_t *)0x1D96A88)  // holds the next actions back while set

// UI context fields
#define FF8_BATTLE_UI_CTX_FRESH_INPUT       33   // 1 on the tick that latched a fresh pad snapshot
#define FF8_BATTLE_UI_CTX_BLINK_COUNTER     0x2C // HUD blink / pulse counter, ++ on each latch

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

static bool pacing_enabled = false;
static int pacing_n = 1;           // host frames per real tick
static int pacing_phase = 0;       // 0 = real tick
static uint32_t pacing_frame_no = 0;
static bool pacing_left_battle = true; // armed by any non-battle frame: next battle frame resets
static const ff8_battle_pacing_layer *pacing_layer = nullptr;

bool ff8_battle_pacing_enabled() { return pacing_enabled; }
int ff8_battle_pacing_frames_per_tick() { return pacing_n; }
int ff8_battle_pacing_phase() { return pacing_phase; }
bool ff8_battle_pacing_is_real_tick() { return pacing_phase == 0; }
uint32_t ff8_battle_pacing_frame_number() { return pacing_frame_no; }
void ff8_battle_pacing_set_layer(const ff8_battle_pacing_layer *layer) { pacing_layer = layer; }

// Scope in which a function hooked with replace_function can call its original
struct pacing_unhooked
{
	uint32_t ri;
	pacing_unhooked(uint32_t replacement) : ri(replacement) { unreplace_function(ri); }
	~pacing_unhooked() { rereplace_function(ri); }
};

// ---------------------------------------------------------------------------
// Frame limiter and pad reads
// ---------------------------------------------------------------------------

double ff8_battle_pacing_battle_framerate()
{
	return 15.0 * pacing_n;
}

// The engine reads the pad once per battle loop iteration, right before the visible UI
// tick. Below 60 fps the other UI ticks of the frame would only see that one read.
int ff8_battle_pacing_extra_pad_reads()
{
	return pacing_enabled ? 4 / pacing_n - 1 : 0;
}

void ff8_battle_pacing_read_pad()
{
	struct ff8_game_obj *game_object = (ff8_game_obj *)common_externals.get_game_object();

	if (!ff8_always_capture_input && game_object->hwnd != GetActiveWindow()) return;

	// The engine's field/menu auto-repeat must not tick twice per frame (battle uses its own
	// per-UI-tick repeat)
	uint32_t saved = FF8_INPUT_AUTOREPEAT_ENABLED;
	FF8_INPUT_AUTOREPEAT_ENABLED = 0;
	((void *(*)())FF8_INPUT_PROCESS)();
	FF8_INPUT_AUTOREPEAT_ENABLED = saved;
}

// Called at the end of every host frame: the phase of the next frame is decided here, so
// that every gate of a frame, before and inside BdLink, sees the same phase
void ff8_battle_pacing_on_host_frame(uint32_t driver_mode)
{
	if (!pacing_enabled) return;

	if (driver_mode != MODE_BATTLE)
	{
		pacing_left_battle = true;
		pacing_phase = 0; // the first battle frame is a real tick
		pacing_frame_no = 0;
		return;
	}

	pacing_phase = (pacing_phase + 1) % pacing_n;
	pacing_frame_no++;
}

// ---------------------------------------------------------------------------
// Effect queues: native-rate tick, draws recorded and redrawn on held frames
// ---------------------------------------------------------------------------
// BdLink ticks the active effect tree (magic, GF, limit breaks, Draw) and the hit-effect
// queue once per host frame; each tick both ADVANCES and DRAWS. They tick on real frames
// only. Their draws reach the frame as 24-byte nodes of the SSIGPU execution arena linked
// into the battle ordering table: the nodes created during the tick are exactly
// [arena cursor before, cursor after), each with its final depth keys and mask, and each
// one's bucket is found by walking the ordering table. Their packets are copied at the end
// of the real tick and linked again into every held frame's ordering table, so the held
// frames show exactly what the real tick drew. Nothing is run twice or rewound: no
// re-triggered sounds, damage numbers or particles.

#define PACING_REC_MAX_PRIMS  4096
#define PACING_REC_ARENA_WORDS (256 * 1024) // 1 MB of packet copies per queue

#pragma pack(push, 1)
struct pacing_exec_node // SSIGPU execution node
{
	uint32_t *pkt;   // packet (tag low 24 bits = previous bucket head, low part)
	int32_t k[4];    // depth keys
	uint16_t msk;    // bit 0 = alternate viewport mode
	uint8_t link_hi; // previous bucket head, high byte
	uint8_t pad;
};
#pragma pack(pop)
static_assert(sizeof(pacing_exec_node) == 24, "SSIGPU execution node is 24 bytes");

struct pacing_prim
{
	int32_t k[4];
	uint16_t bucket;
	uint16_t msk;
	uint32_t off;   // word offset of the packet copy in the arena
	uint32_t words; // packet size in words, tag included
};

struct pacing_recorder
{
	const char *name;
	ff8_battle_pacing_queue queue;
	pacing_prim prim[PACING_REC_MAX_PRIMS];
	uint32_t arena[PACING_REC_ARENA_WORDS];
	int count;
	uint32_t used;
	bool valid;         // the last real tick's draws are recorded
	int last_ret;       // queue return value of the last real tick
	uint32_t rec_begin; // arena cursor before the real tick
	uint32_t rec_ot;    // ordering table of the real tick
	bool ot_cleared;    // the effect clears the ordering table in its tick
	void *faulted[16];  // contexts whose copy/redraw faulted: not redrawn any more
	int faulted_n;
	bool overflow_logged;
};

static pacing_recorder pacing_rec_effect = { "effect", FF8_BATTLE_PACING_QUEUE_EFFECT };
static pacing_recorder pacing_rec_hit = { "hit-effect queue", FF8_BATTLE_PACING_QUEUE_HIT_EFFECT };
static int (__cdecl *pacing_effect_tick_orig)(void *) = nullptr;
static int (__cdecl *pacing_hit_tick_orig)(void *) = nullptr;
static int16_t pacing_node_bucket[PACING_REC_MAX_PRIMS];

static uint32_t *pacing_current_ot()
{
	return (uint32_t *)(FF8_BATTLE_RENDER_LIST_BASE + 68);
}

// VRAM transfers (GP0 0x80-0xDF: framebuffer copies used by mirror/warp effects) are not
// redrawn: repeating them on a held frame would copy whatever the screen holds by then.
// Walks the packet's GP0 commands; returns true if one of them is a VRAM transfer or an
// unknown command.
static bool pacing_packet_has_transfer(const uint32_t *p, uint32_t words)
{
	uint32_t w = 1;

	while (w < words)
	{
		uint32_t cmd = p[w] >> 24;

		if (cmd >= 0x20 && cmd < 0x40) // polygon
		{
			uint32_t nv = (cmd & 0x08) ? 4 : 3;
			bool tex = (cmd & 0x04) != 0, gouraud = (cmd & 0x10) != 0;
			w += 1 + nv + (tex ? nv : 0) + (gouraud ? nv - 1 : 0);
		}
		else if (cmd >= 0x40 && cmd < 0x60) // line / polyline
		{
			bool poly = (cmd & 0x08) != 0, gouraud = (cmd & 0x10) != 0;
			if (!poly) w += gouraud ? 4 : 3;
			else
			{
				w += 2;
				while (w < words && (p[w] & 0xF000F000) != 0x50005000) w++;
				w++; // terminator
			}
		}
		else if (cmd >= 0x60 && cmd < 0x80) // rectangle / sprite
			w += 2 + ((cmd & 0x04) ? 1 : 0) + (((cmd & 0x18) == 0) ? 1 : 0);
		else if ((cmd >= 0xE1 && cmd <= 0xE6) || p[w] == 0) // draw mode / area / offset / NOP
			w++;
		else
			return true;
	}

	return false;
}

static bool pacing_rec_is_faulted(pacing_recorder &rec, void *ctx)
{
	for (int i = 0; i < rec.faulted_n; i++)
		if (rec.faulted[i] == ctx) return true;
	return false;
}

static void pacing_rec_add_faulted(pacing_recorder &rec, void *ctx)
{
	if (!pacing_rec_is_faulted(rec, ctx) && rec.faulted_n < 16) rec.faulted[rec.faulted_n++] = ctx;
}

// End of a real tick: copy every node the queue created, while the packets are fresh
static void pacing_rec_capture_unsafe(pacing_recorder &rec)
{
	uint32_t begin = rec.rec_begin, end = FF8_SSIGPU_EXEC_CUR;

	if (end < begin || (end - begin) % sizeof(pacing_exec_node) != 0) return; // arena flushed during the tick

	int n = (int)((end - begin) / sizeof(pacing_exec_node));

	if (n > PACING_REC_MAX_PRIMS)
	{
		if (!rec.overflow_logged) ffnx_warning("battle pacing: %d draws in one %s tick (max %d), held frames not redrawn\n", n, rec.name, PACING_REC_MAX_PRIMS);
		rec.overflow_logged = true;
		return;
	}

	for (int i = 0; i < n; i++) pacing_node_bucket[i] = -1;

	// Bucket of each node: walk every bucket's chain while it stays inside the tick's nodes
	// (nothing else inserts during the tick, so the tick's nodes sit on top of each chain)
	uint32_t *ot = (uint32_t *)rec.rec_ot;

	for (int b = 0; b < FF8_BATTLE_OT_BUCKETS; b++)
	{
		uint32_t v = ot[b];

		while (v >= begin && v < end && (v - begin) % sizeof(pacing_exec_node) == 0)
		{
			int i = (int)((v - begin) / sizeof(pacing_exec_node));
			if (pacing_node_bucket[i] >= 0) break;
			pacing_node_bucket[i] = (int16_t)b;
			const pacing_exec_node *node = (const pacing_exec_node *)v;
			v = (node->pkt[0] & 0xFFFFFF) | ((uint32_t)node->link_hi << 24);
		}
	}

	for (int i = 0; i < n; i++)
	{
		const pacing_exec_node *node = (const pacing_exec_node *)(begin + i * sizeof(pacing_exec_node));

		if (pacing_node_bucket[i] < 0) continue; // not linked (the engine discarded it)

		uint32_t words = (node->pkt[0] >> 24) + 1;

		if (pacing_packet_has_transfer(node->pkt, words)) continue;
		if (rec.used + words > PACING_REC_ARENA_WORDS) return;

		pacing_prim &d = rec.prim[rec.count++];
		memcpy(d.k, node->k, sizeof(d.k));
		d.bucket = (uint16_t)pacing_node_bucket[i];
		d.msk = node->msk;
		d.off = rec.used;
		d.words = words;
		memcpy(&rec.arena[rec.used], node->pkt, words * 4);
		rec.used += words;
	}

	rec.valid = true;
}

// Held frame: link the recorded packets into this frame's ordering table, exactly like the
// engine's inserts do (node from the execution arena + packet tag relinked)
static void pacing_rec_redraw_unsafe(pacing_recorder &rec)
{
	uint32_t *ot = pacing_current_ot();

	for (int i = 0; i < rec.count; i++)
	{
		if (FF8_SSIGPU_EXEC_CUR - FF8_SSIGPU_EXEC_START >= FF8_SSIGPU_EXEC_LIMIT) break; // same limit as the engine

		const pacing_prim &e = rec.prim[i];
		uint32_t *pkt = &rec.arena[e.off];
		pacing_exec_node *node = (pacing_exec_node *)FF8_SSIGPU_EXEC_CUR;
		uint32_t *bucket = ot + e.bucket;
		uint32_t old_head = *bucket;

		node->pkt = pkt;
		memcpy(node->k, e.k, sizeof(node->k));
		node->msk = e.msk;
		node->link_hi = (uint8_t)(old_head >> 24);
		node->pad = 0;
		*bucket = (uint32_t)node;
		pkt[0] = (pkt[0] & 0xFF000000) | (old_head & 0xFFFFFF);
		FF8_SSIGPU_EXEC_CUR += sizeof(pacing_exec_node);
	}
}

static int pacing_queue_tick(pacing_recorder &rec, void *ctx, int (__cdecl *orig)(void *), int held_ret)
{
	if (pacing_phase == 0)
	{
		rec.rec_begin = FF8_SSIGPU_EXEC_CUR;
		rec.rec_ot = (uint32_t)pacing_current_ot();

		// Cinematic effects clear the ordering table at the start of their tick, discarding what
		// was inserted earlier in the frame: detected here and reproduced on held frames
		uint32_t *ot = (uint32_t *)rec.rec_ot;
		uint32_t probe1 = ot[1], probe4095 = ot[4095];

		int r = orig(ctx);

		bool had1 = probe1 != (uint32_t)&ot[0], had4095 = probe4095 != (uint32_t)&ot[4094];
		if ((had1 && ot[1] == (uint32_t)&ot[0]) || (had4095 && ot[4095] == (uint32_t)&ot[4094])) rec.ot_cleared = true;

		rec.last_ret = r;
		rec.valid = false;
		rec.count = 0;
		rec.used = 0;

		if (r == 0)
		{
			// queue empty = effect finished: never redraw past its end
			rec.ot_cleared = false;
		}
		else if (!pacing_rec_is_faulted(rec, ctx))
		{
			__try
			{
				pacing_rec_capture_unsafe(rec);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				rec.valid = false;
				pacing_rec_add_faulted(rec, ctx);
				ffnx_warning("battle pacing: %s %p: draw copy faulted, held frames not redrawn\n", rec.name, ctx);
			}
		}

		if (pacing_layer && pacing_layer->on_effect_tick) pacing_layer->on_effect_tick(rec.queue, ctx, r);

		return r;
	}

	if (rec.ot_cleared && rec.last_ret != 0)
		((void (__cdecl *)(void *, int))FF8_SSIGPU_CLEAR_OT)(pacing_current_ot(), 4096);

	if (pacing_layer && pacing_layer->effect_held_frame && pacing_layer->effect_held_frame(rec.queue, ctx, orig))
		return held_ret;

	if (rec.valid && !pacing_rec_is_faulted(rec, ctx))
	{
		__try
		{
			pacing_rec_redraw_unsafe(rec);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			pacing_rec_add_faulted(rec, ctx);
			ffnx_warning("battle pacing: %s %p: redraw faulted, held frames not redrawn\n", rec.name, ctx);
		}
	}

	return held_ret;
}

// Held frames report "still running" (the caller would otherwise drop the effect pointer)
static int __cdecl pacing_effect_tick_hook(void *effect_ctx)
{
	return pacing_queue_tick(pacing_rec_effect, effect_ctx, pacing_effect_tick_orig, 1);
}

static int __cdecl pacing_hit_tick_hook(void *queue)
{
	return pacing_queue_tick(pacing_rec_hit, queue, pacing_hit_tick_orig, pacing_rec_hit.last_ret);
}

// Screen feedback (Eden and others draw a ghost of the whole screen): an effect tick arms a
// one-frame request that the battle loop consumes and clears after the frame. Effects tick
// on real frames only, so the request of the last real tick is armed again on held frames.
static int pacing_feedback_request = 0;

static int __cdecl pacing_feedback_request_hook(int mode)
{
	FF8_BATTLE_SCREEN_FEEDBACK_REQUEST = mode + 1;
	pacing_feedback_request = mode + 1;
	return mode + 1;
}

// ---------------------------------------------------------------------------
// Held-frame redraw of tasks that advance and draw in the same call
// ---------------------------------------------------------------------------
// Such a task draws with its state, then advances it. On a real tick its state is recorded
// before the call; a held frame puts that state back, calls the task (same draw as the real
// tick), then restores the current state. A task first seen on a held frame draws with its
// current state. The memo of a task that ended is dropped (its node can be reused).

#define PACING_TASK_MEMOS 32
#define PACING_TASK_MEMO_BYTES 0x20

struct pacing_task_memo
{
	uint8_t *node;
	uint32_t tick; // real tick (host frame number) the state was recorded on
	uint8_t data[PACING_TASK_MEMO_BYTES];
};

static pacing_task_memo pacing_task_memos[PACING_TASK_MEMOS];

static pacing_task_memo *pacing_task_memo_find(uint8_t *node, bool create)
{
	pacing_task_memo *free_slot = nullptr;

	for (pacing_task_memo &m : pacing_task_memos)
	{
		if (m.node == node) return &m;
		if (!m.node && !free_slot) free_slot = &m;
	}

	if (create && free_slot) free_slot->node = node;

	return create ? free_slot : nullptr;
}

// data = node + 0x0C (task data), size bytes of it
static DWORD pacing_task_redraw(uint8_t *node, uint32_t size, DWORD (*call)(uint8_t *))
{
	uint8_t *data = node + 0x0C;

	if (pacing_phase == 0)
	{
		pacing_task_memo *m = pacing_task_memo_find(node, true);

		if (m)
		{
			memcpy(m->data, data, size);
			m->tick = pacing_frame_no;
		}

		DWORD r = call(node);

		if (r == 2 && m) m->node = nullptr;

		return r;
	}

	uint8_t current[PACING_TASK_MEMO_BYTES];
	pacing_task_memo *m = pacing_task_memo_find(node, false);

	memcpy(current, data, size);
	if (m && pacing_frame_no - m->tick < (uint32_t)pacing_n) memcpy(data, m->data, size);
	call(node);
	memcpy(data, current, size);

	return 0; // a held frame never ends a task
}

// Per-entity status visuals (sub_50A0A0: status colour pulse, Float bob, spin, status
// sprites and Doom counter): same scheme on the entity's 32-byte status animation block
#define PACING_STATUS_MEMOS 16

struct pacing_status_memo
{
	uint8_t *entity;
	uint32_t tick;
	uint8_t data[0x20];
};

static pacing_status_memo pacing_status_memos[PACING_STATUS_MEMOS];

// ---------------------------------------------------------------------------
// Frame phase driver
// ---------------------------------------------------------------------------

static uint32_t pacing_bdlink_ri = 0;
static int16_t pacing_shake_last[3] = { 0, 0, 0 };
static struct { uint8_t *node; uint8_t state[0x20], mtx[0x20]; bool valid; } pacing_a6_memo;
static bool pacing_ui_latch_counts = false;
static int pacing_ui_hidden_index = 0;
static bool pacing_ui_skip_tick = false;

static void pacing_battle_reset()
{
	for (pacing_recorder *rec : { &pacing_rec_effect, &pacing_rec_hit })
	{
		rec->valid = false;
		rec->ot_cleared = false;
		rec->faulted_n = 0;
		rec->last_ret = 0;
		rec->count = 0;
		rec->used = 0;
	}
	pacing_feedback_request = 0;
	memset(pacing_shake_last, 0, sizeof(pacing_shake_last));
	memset(&pacing_a6_memo, 0, sizeof(pacing_a6_memo));
	memset(pacing_task_memos, 0, sizeof(pacing_task_memos));
	memset(pacing_status_memos, 0, sizeof(pacing_status_memos));
	pacing_ui_latch_counts = false;
	pacing_ui_hidden_index = 0;
	pacing_ui_skip_tick = false;

	if (pacing_layer && pacing_layer->on_battle_start) pacing_layer->on_battle_start();
}

static int __cdecl pacing_bdlink_hook()
{
	if (pacing_left_battle)
	{
		pacing_left_battle = false;
		pacing_battle_reset();
	}

	if (pacing_phase == 0)
		pacing_feedback_request = 0; // re-armed by the effect if it still wants it this tick
	else if (pacing_feedback_request)
		FF8_BATTLE_SCREEN_FEEDBACK_REQUEST = pacing_feedback_request;

	if (pacing_layer && pacing_layer->on_frame_begin) pacing_layer->on_frame_begin(pacing_phase == 0);

	pacing_unhooked u(pacing_bdlink_ri);

	return ((int (__cdecl *)())FF8_BATTLE_BDLINK)();
}

// ---------------------------------------------------------------------------
// Battle UI: 4 / n UI ticks per host frame
// ---------------------------------------------------------------------------
// battle_cardgame_main_loop runs the UI tick pair 3 times with menu rendering disabled
// (hidden catch-up ticks), reads the pad, runs the battle logic, then runs the pair once more
// with rendering enabled. At 15 * n fps, 4 / n ticks per host frame keep the vanilla 60 UI
// ticks/s: the last (4 / n - 1) hidden ticks are kept with the visible one (none at 60 fps).
// The pair is always called update-then-display, so the decision made in the update hook is
// used by the display hook.
//
// The fresh-input latch (UI ctx+33) fires on the first tick after BdLink armed it, and at
// most once per CONST_BattleUI_TicksPerFrame ticks. BdLink also runs the menu tasks and text
// services that many times per call. BdLink runs every host frame, so that constant becomes
// 4 / n: both keep their vanilla 60 per second and every host frame gets a latch (the UI state
// machines advance on it). Latches counted as time are brought back to the vanilla pace:
// the HUD blink counter and GF Boost's phase countdown (1 latch in n), and the Renzokuken
// trigger timeline, which adds 4 UI ticks per latch (4 / n).

static uint32_t pacing_ui_update_ri = 0, pacing_ui_display_ri = 0, pacing_boost_ri = 0;

static int __cdecl pacing_ui_update_hook()
{
	if (FF8_BATTLE_UI_MENU_RENDERING == 0)
		pacing_ui_skip_tick = pacing_ui_hidden_index++ < 3 - (4 / pacing_n - 1);
	else
	{
		pacing_ui_hidden_index = 0;
		pacing_ui_skip_tick = false;
	}

	if (pacing_ui_skip_tick) return 0;

	int r;
	{ pacing_unhooked u(pacing_ui_update_ri); r = ((int (__cdecl *)())FF8_BATTLE_UI_UPDATE)(); }

	uint8_t *ctx = FF8_BATTLE_UI_CTX;
	pacing_ui_latch_counts = false;

	if (ctx && ctx[FF8_BATTLE_UI_CTX_FRESH_INPUT])
	{
		static uint32_t latches = 0;

		pacing_ui_latch_counts = (latches++ % pacing_n) == 0;
		// limit-break arrow, blinking command text, list page arrows, active character pulse,
		// countdown flash: 1 increment in n
		if (!pacing_ui_latch_counts) ctx[FF8_BATTLE_UI_CTX_BLINK_COUNTER]--;
	}

	return r;
}

static int __cdecl pacing_ui_display_hook()
{
	if (pacing_ui_skip_tick) return 0;

	{ pacing_unhooked u(pacing_ui_display_ri); return ((int (__cdecl *)())FF8_BATTLE_UI_DISPLAY)(); }
}

// GF Boost counts its safe/danger phases and total window down on latch ticks only (15 per
// second in vanilla, on the PlayStation too): only the latches that count reach it. Its
// 4-tick input budget is refilled by those latches, as in vanilla. Square presses are read
// on every UI tick, now fed by a pad read per tick.
static void __cdecl pacing_boost_hook()
{
	uint8_t *ctx = FF8_BATTLE_UI_CTX;
	uint8_t saved = ctx ? ctx[FF8_BATTLE_UI_CTX_FRESH_INPUT] : 0;

	if (ctx && !pacing_ui_latch_counts) ctx[FF8_BATTLE_UI_CTX_FRESH_INPUT] = 0;

	{ pacing_unhooked u(pacing_boost_ri); ((void (__cdecl *)())FF8_BATTLE_GF_BOOST)(); }

	if (ctx) ctx[FF8_BATTLE_UI_CTX_FRESH_INPUT] = saved;
}

// Game time / battle countdown: called 4 times per loop iteration whatever the frame rate
static uint32_t pacing_game_time_ri = 0;

static int __cdecl pacing_game_time_hook()
{
	if (getmode_cached()->driver_mode == MODE_BATTLE)
	{
		static uint32_t calls = 0;

		if ((calls++ % pacing_n) != 0) return 0;
	}

	{ pacing_unhooked u(pacing_game_time_ri); return ((int (__cdecl *)())FF8_SAVEMAP_TICK_GAME_TIME)(); }
}

// ---------------------------------------------------------------------------
// Models: animation and choreography advance on real ticks only
// ---------------------------------------------------------------------------

static uint32_t pacing_read_anim_ri = 0, pacing_animseq_ri = 0;

static void pacing_build_bone_matrices(void *anim_header)
{
	((void (__cdecl *)(void *))FF8_BATTLE_BUILD_BONE_MATRICES)(anim_header);
}

// A fault while decoding an animation stream finishes that animation instead of crashing
static int pacing_read_anim_guarded(void *anim_header, void *anim_cmd)
{
	__try
	{
		return ((int (__cdecl *)(void *, void *))FF8_BATTLE_READ_ANIMATION)(anim_header, anim_cmd);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		uint8_t *c = (uint8_t *)anim_cmd;
		ffnx_warning("battle pacing: animation read faulted (anim %u frame %u/%u), animation finished\n", c[0], c[6], c[7]);
		c[6] = c[7];
		return 1; // animation complete
	}
}

// Held frames rebuild the model geometry from the current pose without advancing it (the
// geometry is double buffered: skipping the call would draw a stale buffer). anim_cmd:
// +6 current frame, +7 total frames.
static int __cdecl pacing_read_anim_hook(void *anim_header, void *anim_cmd)
{
	uint8_t *cmd = (uint8_t *)anim_cmd;

	// A completed animation early-outs without rebuilding: rebuild here as well
	if (cmd[6] >= cmd[7])
	{
		pacing_build_bone_matrices(anim_header);
		return 1;
	}

	// Frame 0 is the absolute base pose read right after the bones were zeroed: never held
	if (pacing_phase != 0 && cmd[6] != 0)
	{
		if (!(pacing_layer && pacing_layer->anim_held_frame && pacing_layer->anim_held_frame(anim_header, anim_cmd)))
			pacing_build_bone_matrices(anim_header);

		return 0; // frame processed, animation not complete
	}

	unreplace_function(pacing_read_anim_ri);
	int r = pacing_read_anim_guarded(anim_header, anim_cmd);
	rereplace_function(pacing_read_anim_ri);

	return r;
}

// The choreography VM (movement, animation changes, delays, sounds) runs on real ticks;
// held frames only let the animation leaf rebuild the geometry
static int __cdecl pacing_animseq_hook(void *entity_slot)
{
	if (pacing_phase != 0)
	{
		((int (__cdecl *)(void *))FF8_BATTLE_ANIM_ADVANCE_BY_1)(entity_slot);
		return 0; // the caller ignores the return value
	}

	{ pacing_unhooked u(pacing_animseq_ri); return ((int (__cdecl *)(void *))FF8_BATTLE_ANIMSEQ_UPDATE_ENTITY)(entity_slot); }
}

// ---------------------------------------------------------------------------
// Camera
// ---------------------------------------------------------------------------

static uint32_t pacing_camera_anim_ri = 0, pacing_camera_seq_ri = 0, pacing_camera_ops_ri = 0;

// Keyframe player: real ticks only (halving its time step instead hangs its catch-up loop)
static int __cdecl pacing_camera_anim_hook(void *task)
{
	if (pacing_phase != 0) return 0;

	{ pacing_unhooked u(pacing_camera_anim_ri); return ((int (__cdecl *)(void *))FF8_BATTLE_CAMERA_ANIMATION)(task); }
}

// Camera script VM: real ticks only (returns the current setting pointer)
static void *__cdecl pacing_camera_seq_hook()
{
	if (pacing_phase != 0) return FF8_BATTLE_CAMERA_SETTING;

	{ pacing_unhooked u(pacing_camera_seq_ri); return ((void *(__cdecl *)())FF8_BATTLE_CAMERA_SEQUENCE)(); }
}

// The shake offsets are applied then cleared every frame; their producers only run on real
// ticks, so the last real values are applied again on held frames
static int __cdecl pacing_camera_ops_hook()
{
	int16_t *shake = FF8_BATTLE_CAMERA_SHAKE;

	if (pacing_phase == 0) memcpy(pacing_shake_last, shake, sizeof(pacing_shake_last));
	else if (shake[0] == 0 && shake[1] == 0 && shake[2] == 0) memcpy(shake, pacing_shake_last, sizeof(pacing_shake_last));

	{ pacing_unhooked u(pacing_camera_ops_ri); return ((int (__cdecl *)())FF8_BATTLE_CAMERA_OPERATIONS)(); }
}

// ---------------------------------------------------------------------------
// AnimSeq-spawned tasks: tick functions DWORD f(node), 0 = keep, 2 = remove, data at +0x0C
// ---------------------------------------------------------------------------

#define PACING_TASK_HOOK(name, addr) \
	static uint32_t pacing_##name##_ri = 0; \
	static DWORD pacing_##name##_call(uint8_t *node) \
	{ \
		pacing_unhooked u(pacing_##name##_ri); \
		return ((DWORD (__cdecl *)(uint8_t *))(addr))(node); \
	}

PACING_TASK_HOOK(t84, FF8_BATTLE_TASK_84_WOBBLE)
PACING_TASK_HOOK(t9f, FF8_BATTLE_TASK_9F_TEXTURE_BLINK)
PACING_TASK_HOOK(tstep, FF8_BATTLE_TASK_99_B1_FOOTSTEP)
PACING_TASK_HOOK(t96, FF8_BATTLE_TASK_96_CAMERA_SHAKE)
PACING_TASK_HOOK(t9e, FF8_BATTLE_TASK_9E_MOVE)
PACING_TASK_HOOK(tdrag, FF8_BATTLE_TASK_AD_AE_DRAG)
PACING_TASK_HOOK(t81, FF8_BATTLE_TASK_81_RESTORE_PART)
PACING_TASK_HOOK(ta6, FF8_BATTLE_TASK_A6_DETACH_PART)

// Pure counters with persistent results: held frames skip them
static DWORD __cdecl pacing_t84_hook(uint8_t *n) { return pacing_phase ? 0 : pacing_t84_call(n); }
static DWORD __cdecl pacing_t9f_hook(uint8_t *n) { return pacing_phase ? 0 : pacing_t9f_call(n); }
static DWORD __cdecl pacing_tstep_hook(uint8_t *n) { return pacing_phase ? 0 : pacing_tstep_call(n); }
static DWORD __cdecl pacing_t96_hook(uint8_t *n) { return pacing_phase ? 0 : pacing_t96_call(n); }
static DWORD __cdecl pacing_t9e_hook(uint8_t *n) { return pacing_phase ? 0 : pacing_t9e_call(n); }

// Snaps the target to the attacker's bone every tick; only its frames-left counter (+0x18)
// must count at the vanilla rate, and a held frame never ends the task
static DWORD __cdecl pacing_tdrag_hook(uint8_t *n)
{
	if (pacing_phase == 0) return pacing_tdrag_call(n);

	int32_t left = *(int32_t *)(n + 0x18);

	if (left == 1) return 0;

	DWORD r = pacing_tdrag_call(n);
	*(int32_t *)(n + 0x18) = left;

	return r;
}

// Draws the fading part inside its tick: runs every frame, its fade counter (+0x13) only
// advances on real ticks
static DWORD __cdecl pacing_t81_hook(uint8_t *n)
{
	if (pacing_phase == 0) return pacing_t81_call(n);

	uint8_t state = n[0x12], counter = n[0x13];

	pacing_t81_call(n);
	n[0x13] = (state == 0) ? 0 : counter; // first tick: init only, counting starts on the next real tick

	return 0;
}

// Draws the detached part, THEN integrates its physics (node +0x0C..+0x2B and the saved
// matrix). A held frame draws the state the real tick drew (physics skipped through the
// battle "paused" flag for that call), then the real state is put back.
static DWORD __cdecl pacing_ta6_hook(uint8_t *n)
{
	if (pacing_phase == 0)
	{
		pacing_a6_memo.node = n;
		memcpy(pacing_a6_memo.state, n + 0x0C, 0x20);
		memcpy(pacing_a6_memo.mtx, FF8_BATTLE_DETACHED_PART_MATRIX, 0x20);
		pacing_a6_memo.valid = n[0x12] != 0; // initialised (the first tick copies the bone matrix)

		return pacing_ta6_call(n);
	}

	uint8_t node_cur[0x20], mtx_cur[0x20];
	memcpy(node_cur, n + 0x0C, sizeof(node_cur));
	memcpy(mtx_cur, FF8_BATTLE_DETACHED_PART_MATRIX, sizeof(mtx_cur));

	uint32_t flags = FF8_BATTLE_UPDATE_FLAGS;

	if (pacing_a6_memo.valid && pacing_a6_memo.node == n && n[0x12] != 0)
	{
		memcpy(n + 0x0C, pacing_a6_memo.state, 0x20);
		memcpy(FF8_BATTLE_DETACHED_PART_MATRIX, pacing_a6_memo.mtx, 0x20);
		FF8_BATTLE_UPDATE_FLAGS = flags | 1;
	}

	pacing_ta6_call(n);

	FF8_BATTLE_UPDATE_FLAGS = flags;
	memcpy(n + 0x0C, node_cur, sizeof(node_cur));
	memcpy(FF8_BATTLE_DETACHED_PART_MATRIX, mtx_cur, sizeof(mtx_cur));

	return 0;
}

// ---------------------------------------------------------------------------
// Battle director: status timers and Gilgamesh / Angelo countdown run once per loop
// iteration; real ticks only
// ---------------------------------------------------------------------------

static uint32_t pacing_status_timers_ri = 0, pacing_gilgamesh_ri = 0;

static void __cdecl pacing_status_timers_hook()
{
	if (pacing_phase != 0) return;

	{ pacing_unhooked u(pacing_status_timers_ri); ((void (__cdecl *)())FF8_BATTLE_STATUS_TIMERS)(); }
}

static void __cdecl pacing_gilgamesh_hook()
{
	if (pacing_phase != 0) return;

	{ pacing_unhooked u(pacing_gilgamesh_ri); ((void (__cdecl *)())FF8_BATTLE_GILGAMESH_ANGELO)(); }
}

// ---------------------------------------------------------------------------
// Entities, messages, screen fades, stage: counters advanced by per-frame tasks
// ---------------------------------------------------------------------------

static int (__cdecl *pacing_entity_fades_orig)(void *) = nullptr;
static void (__cdecl *pacing_status_visuals_orig)(void *) = nullptr;
static char (__cdecl *pacing_text_management_orig)() = nullptr;
static void *(__cdecl *pacing_stage_queue_orig)() = nullptr;
static void *(__cdecl *pacing_stage_142_fade_orig)() = nullptr;

// Death / escape / appear fades: held frames keep the entity's colours (0 = draw it)
static int __cdecl pacing_entity_fades_hook(void *entity)
{
	if (pacing_phase != 0) return 0;

	return pacing_entity_fades_orig(entity);
}

// Status visuals advance their counters and draw the status sprites in the same call
static void __cdecl pacing_status_visuals_hook(void *entity)
{
	uint8_t *block = *(uint8_t **)((uint8_t *)entity + 0x88);

	if (!block)
	{
		if (pacing_phase == 0) pacing_status_visuals_orig(entity);
		return;
	}

	pacing_status_memo *memo = nullptr, *free_slot = nullptr;

	for (pacing_status_memo &m : pacing_status_memos)
	{
		if (m.entity == (uint8_t *)entity) { memo = &m; break; }
		if (!m.entity && !free_slot) free_slot = &m;
	}

	if (pacing_phase == 0)
	{
		if (!memo && free_slot)
		{
			memo = free_slot;
			memo->entity = (uint8_t *)entity;
		}
		if (memo)
		{
			memcpy(memo->data, block, sizeof(memo->data));
			memo->tick = pacing_frame_no;
		}
		pacing_status_visuals_orig(entity);
		return;
	}

	uint8_t current[0x20];
	memcpy(current, block, sizeof(current));
	if (memo && pacing_frame_no - memo->tick < (uint32_t)pacing_n) memcpy(block, memo->data, sizeof(current));
	pacing_status_visuals_orig(entity);
	memcpy(block, current, sizeof(current));
}

// Damage numbers, screen fade and flash: advance and draw in the same call
PACING_TASK_HOOK(tdamage, FF8_BATTLE_DAMAGE_NUMBER_TASK)
PACING_TASK_HOOK(tfade, FF8_BATTLE_SCREEN_FADE_TASK)
PACING_TASK_HOOK(tflash, FF8_BATTLE_SCREEN_FLASH_TASK)

static DWORD __cdecl pacing_tdamage_hook(uint8_t *n) { return pacing_task_redraw(n, 0x14, pacing_tdamage_call); }
static DWORD __cdecl pacing_tfade_hook(uint8_t *n) { return pacing_task_redraw(n, 0x08, pacing_tfade_call); }
static DWORD __cdecl pacing_tflash_hook(uint8_t *n) { return pacing_task_redraw(n, 0x08, pacing_tflash_call); }

// Battle message: the task claims its text channel every frame (that is what displays it)
// and counts its display time (+0x0E) down; held frames claim without counting
PACING_TASK_HOOK(ttext, FF8_BATTLE_TEXT_TASK)

static DWORD __cdecl pacing_ttext_hook(uint8_t *n)
{
	if (pacing_phase == 0) return pacing_ttext_call(n);

	uint8_t time_left = n[0x0E];

	if (time_left == 0) n[0x0E] = 1; // shown until the next real tick ends it
	pacing_ttext_call(n);
	n[0x0E] = time_left;

	return 0;
}

// Camera return-to-view blend ramp: the blend value it wrote stays in place
PACING_TASK_HOOK(toscillation, FF8_BATTLE_CAMERA_OSCILLATION_TASK)

static DWORD __cdecl pacing_toscillation_hook(uint8_t *n) { return pacing_phase ? 0 : pacing_toscillation_call(n); }

// AI text, AI wait timers and the end-of-battle fade trigger count down in this queue
static char __cdecl pacing_text_management_hook()
{
	if (pacing_phase != 0) return 0;

	return pacing_text_management_orig();
}

// Stage tasks draw the stage every frame and advance sky rotation, texture animation and
// per-stage scripts: held frames set the engine's own stage freeze bits for that call
static void *__cdecl pacing_stage_queue_hook()
{
	if (pacing_phase == 0) return pacing_stage_queue_orig();

	uint8_t freeze = FF8_BATTLE_STAGE_FREEZE;

	FF8_BATTLE_STAGE_FREEZE = freeze | 0x0B;
	void *r = pacing_stage_queue_orig();
	FF8_BATTLE_STAGE_FREEZE = freeze;

	return r;
}

static void *__cdecl pacing_stage_142_fade_hook()
{
	if (pacing_phase != 0) return nullptr; // return value unused

	return pacing_stage_142_fade_orig();
}

// Stage 147 intro script (moves the party in, plays steps, holds the battle meanwhile):
// held frames only draw the stage and keep the battle held (task data: +0x0C state,
// +0x0E script step, +0x10 step counter)
PACING_TASK_HOOK(tstage147, FF8_BATTLE_STAGE_147_TASK)

static DWORD __cdecl pacing_tstage147_hook(uint8_t *n)
{
	if (pacing_phase == 0) return pacing_tstage147_call(n);

	if (*(uint16_t *)(n + 0x0C) == 1)
	{
		((void (__cdecl *)(int))FF8_BATTLE_STAGE_TEXTURE_ANIMATION)(0);
		((void (__cdecl *)(int))FF8_BATTLE_STAGE_TEXTURE_ANIMATION)(1);
		((void (__cdecl *)())FF8_BATTLE_STAGE_RENDER)();

		uint16_t step = *(uint16_t *)(n + 0x0E);

		if (step == 1 || (step == 5 && *(int16_t *)(n + 0x10) < 5)) FF8_BATTLE_TASKS_BUSY = 1;
	}

	return 0;
}

// ---------------------------------------------------------------------------
// Install
// ---------------------------------------------------------------------------

// First bytes of every patched location in the supported executable
struct pacing_signature { uint32_t addr; uint8_t bytes[5]; };

static const pacing_signature pacing_signatures[] = {
	{ FF8_BATTLE_BDLINK,                  { 0xE8, 0xCB, 0xC7, 0xF8, 0xFF } },
	{ FF8_BATTLE_UI_UPDATE,               { 0x56, 0x6A, 0x00, 0xE8, 0xC8 } },
	{ FF8_BATTLE_UI_DISPLAY,              { 0x57, 0x33, 0xFF, 0x57, 0xE8 } },
	{ FF8_BATTLE_GF_BOOST,                { 0x83, 0xEC, 0x08, 0x8A, 0x0D } },
	{ FF8_SAVEMAP_TICK_GAME_TIME,         { 0xE8, 0x5B, 0xF3, 0x02, 0x00 } },
	{ FF8_BATTLE_READ_ANIMATION,          { 0x83, 0xEC, 0x08, 0x53, 0x8B } },
	{ FF8_BATTLE_BUILD_BONE_MATRICES,     { 0x81, 0xEC, 0x10, 0x0F, 0x00 } },
	{ FF8_BATTLE_ANIMSEQ_UPDATE_ENTITY,   { 0x83, 0xEC, 0x08, 0x53, 0x57 } },
	{ FF8_BATTLE_ANIM_ADVANCE_BY_1,       { 0x56, 0x8B, 0x74, 0x24, 0x08 } },
	{ FF8_BATTLE_CAMERA_ANIMATION,        { 0x51, 0x8B, 0x44, 0x24, 0x08 } },
	{ FF8_BATTLE_CAMERA_SEQUENCE,         { 0xA1, 0x34, 0x9A, 0xD9, 0x01 } },
	{ FF8_BATTLE_CAMERA_OPERATIONS,       { 0x55, 0x33, 0xED, 0x66, 0x39 } },
	{ FF8_BATTLE_REQUEST_SCREEN_FEEDBACK, { 0x8B, 0x44, 0x24, 0x04, 0x40 } },
	{ FF8_BATTLE_STATUS_TIMERS,           { 0x83, 0xEC, 0x0C, 0x53, 0x55 } },
	{ FF8_BATTLE_GILGAMESH_ANGELO,        { 0x66, 0x83, 0x3D, 0xE4, 0x8D } },
	{ FF8_INPUT_PROCESS,                  { 0x83, 0xEC, 0x08, 0x53, 0x55 } },
	{ FF8_SSIGPU_CLEAR_OT,                { 0x8B, 0x44, 0x24, 0x08, 0x56 } },
	{ FF8_BATTLE_TASK_84_WOBBLE,          { 0x56, 0x8B, 0x74, 0x24, 0x08 } },
	{ FF8_BATTLE_TASK_9F_TEXTURE_BLINK,   { 0x56, 0x8B, 0x74, 0x24, 0x08 } },
	{ FF8_BATTLE_TASK_99_B1_FOOTSTEP,     { 0x53, 0x56, 0x8B, 0x74, 0x24 } },
	{ FF8_BATTLE_TASK_96_CAMERA_SHAKE,    { 0x8B, 0x4C, 0x24, 0x04, 0x53 } },
	{ FF8_BATTLE_TASK_9E_MOVE,            { 0x8B, 0x4C, 0x24, 0x04, 0x53 } },
	{ FF8_BATTLE_TASK_AD_AE_DRAG,         { 0x53, 0x56, 0x57, 0x6A, 0x10 } },
	{ FF8_BATTLE_TASK_81_RESTORE_PART,    { 0x53, 0x56, 0x57, 0x6A, 0x4C } },
	{ FF8_BATTLE_TASK_A6_DETACH_PART,     { 0x53, 0x55, 0x56, 0x57, 0x6A } },
	{ FF8_BATTLE_CALL_HIT_EFFECT_QUEUE,   { 0xE8, 0xF8, 0x7A, 0x00, 0x00 } },
	{ FF8_BATTLE_CALL_EFFECT_TICK,        { 0xE8, 0xE1, 0x7A, 0x00, 0x00 } },
	{ FF8_BATTLE_DAMAGE_NUMBER_TASK,      { 0x53, 0x55, 0x56, 0x57, 0x6A } },
	{ FF8_BATTLE_TEXT_TASK,               { 0x8B, 0x44, 0x24, 0x04, 0x56 } },
	{ FF8_BATTLE_SCREEN_FADE_TASK,        { 0x53, 0x56, 0x57, 0x8B, 0x7C } },
	{ FF8_BATTLE_SCREEN_FLASH_TASK,       { 0x56, 0x57, 0x8B, 0x7C, 0x24 } },
	{ FF8_BATTLE_CAMERA_OSCILLATION_TASK, { 0x56, 0x8B, 0x74, 0x24, 0x08 } },
	{ FF8_BATTLE_STAGE_147_TASK,          { 0x53, 0x56, 0x8B, 0x74, 0x24 } },
	{ FF8_BATTLE_STAGE_TEXTURE_ANIMATION, { 0xF6, 0x05, 0x8C, 0x89, 0xD9 } },
	{ FF8_BATTLE_STAGE_RENDER,            { 0x53, 0x55, 0x56, 0x57, 0x6A } },
	{ FF8_BATTLE_CALL_TEXT_MANAGEMENT,    { 0xE8, 0x50, 0x55, 0x00, 0x00 } },
	{ FF8_BATTLE_CALL_STATUS_VISUALS,     { 0xE8, 0x3E, 0x75, 0x00, 0x00 } },
	{ FF8_BATTLE_CALL_ENTITY_FADES,       { 0xE8, 0x97, 0x95, 0x00, 0x00 } },
	{ FF8_BATTLE_CALL_STAGE_142_FADE,     { 0xE8, 0x54, 0x00, 0x00, 0x00 } },
	{ FF8_BATTLE_CALL_STAGE_QUEUE,        { 0xE8, 0xAD, 0x62, 0x00, 0x00 } },
	{ FF8_BATTLE_UI_TICKS_PER_FRAME,      { 0x04, 0x00, 0x00, 0x00, 0x80 } },
	{ FF8_BATTLE_RENZOKUKEN_LATCH_STEP - 4, { 0x98, 0x67, 0xD7, 0x01, 0x04 } },
};

static bool pacing_check_executable()
{
	// The Steam executable is detected as the Nvidia variant: every English 1.2 build is
	// accepted, the code signatures below decide
	if (!FF8_US_VERSION || ff8_remastered_edition) return false;

	for (const pacing_signature &s : pacing_signatures)
	{
		if (memcmp((const void *)s.addr, s.bytes, sizeof(s.bytes)) != 0)
		{
			ffnx_warning("battle pacing: unexpected code at 0x%X, not installed\n", s.addr);
			return false;
		}
	}

	return true;
}

bool ff8_battle_pacing_init(int host_frames_per_tick)
{
	if (pacing_enabled) return true;

	if (host_frames_per_tick != 2 && host_frames_per_tick != 4)
	{
		ffnx_warning("battle pacing: unsupported rate (%d host frames per tick)\n", host_frames_per_tick);
		return false;
	}

	if (!pacing_check_executable())
	{
		ffnx_warning("battle pacing: requires FF8 2000 / Steam English 1.2, not installed\n");
		return false;
	}

	pacing_n = host_frames_per_tick;

	// Frame phase driver
	pacing_bdlink_ri = replace_function(FF8_BATTLE_BDLINK, (void *)pacing_bdlink_hook);

	// Battle UI: 4 / n ticks per host frame, fresh input latched every host frame
	pacing_ui_update_ri = replace_function(FF8_BATTLE_UI_UPDATE, (void *)pacing_ui_update_hook);
	pacing_ui_display_ri = replace_function(FF8_BATTLE_UI_DISPLAY, (void *)pacing_ui_display_hook);
	patch_code_dword(FF8_BATTLE_UI_TICKS_PER_FRAME, 4 / pacing_n);
	patch_code_byte(FF8_BATTLE_RENZOKUKEN_LATCH_STEP, 4 / pacing_n);
	pacing_boost_ri = replace_function(FF8_BATTLE_GF_BOOST, (void *)pacing_boost_hook);
	pacing_game_time_ri = replace_function(FF8_SAVEMAP_TICK_GAME_TIME, (void *)pacing_game_time_hook);

	// Models
	pacing_read_anim_ri = replace_function(FF8_BATTLE_READ_ANIMATION, (void *)pacing_read_anim_hook);
	pacing_animseq_ri = replace_function(FF8_BATTLE_ANIMSEQ_UPDATE_ENTITY, (void *)pacing_animseq_hook);

	// Camera
	pacing_camera_anim_ri = replace_function(FF8_BATTLE_CAMERA_ANIMATION, (void *)pacing_camera_anim_hook);
	pacing_camera_seq_ri = replace_function(FF8_BATTLE_CAMERA_SEQUENCE, (void *)pacing_camera_seq_hook);
	pacing_camera_ops_ri = replace_function(FF8_BATTLE_CAMERA_OPERATIONS, (void *)pacing_camera_ops_hook);

	// Effects and hit effects: native-rate tick, recorded draws redrawn on held frames
	pacing_effect_tick_orig = (int (__cdecl *)(void *))get_relative_call(FF8_BATTLE_CALL_EFFECT_TICK, 0);
	replace_call(FF8_BATTLE_CALL_EFFECT_TICK, (void *)pacing_effect_tick_hook);
	pacing_hit_tick_orig = (int (__cdecl *)(void *))get_relative_call(FF8_BATTLE_CALL_HIT_EFFECT_QUEUE, 0);
	replace_call(FF8_BATTLE_CALL_HIT_EFFECT_QUEUE, (void *)pacing_hit_tick_hook);
	replace_function(FF8_BATTLE_REQUEST_SCREEN_FEEDBACK, (void *)pacing_feedback_request_hook);

	// AnimSeq-spawned tasks
	pacing_t84_ri = replace_function(FF8_BATTLE_TASK_84_WOBBLE, (void *)pacing_t84_hook);
	pacing_t9f_ri = replace_function(FF8_BATTLE_TASK_9F_TEXTURE_BLINK, (void *)pacing_t9f_hook);
	pacing_tstep_ri = replace_function(FF8_BATTLE_TASK_99_B1_FOOTSTEP, (void *)pacing_tstep_hook);
	pacing_t96_ri = replace_function(FF8_BATTLE_TASK_96_CAMERA_SHAKE, (void *)pacing_t96_hook);
	pacing_t9e_ri = replace_function(FF8_BATTLE_TASK_9E_MOVE, (void *)pacing_t9e_hook);
	pacing_tdrag_ri = replace_function(FF8_BATTLE_TASK_AD_AE_DRAG, (void *)pacing_tdrag_hook);
	pacing_t81_ri = replace_function(FF8_BATTLE_TASK_81_RESTORE_PART, (void *)pacing_t81_hook);
	pacing_ta6_ri = replace_function(FF8_BATTLE_TASK_A6_DETACH_PART, (void *)pacing_ta6_hook);

	// Entities, messages, screen fades, stage
	pacing_entity_fades_orig = (int (__cdecl *)(void *))get_relative_call(FF8_BATTLE_CALL_ENTITY_FADES, 0);
	replace_call(FF8_BATTLE_CALL_ENTITY_FADES, (void *)pacing_entity_fades_hook);
	pacing_status_visuals_orig = (void (__cdecl *)(void *))get_relative_call(FF8_BATTLE_CALL_STATUS_VISUALS, 0);
	replace_call(FF8_BATTLE_CALL_STATUS_VISUALS, (void *)pacing_status_visuals_hook);
	pacing_text_management_orig = (char (__cdecl *)())get_relative_call(FF8_BATTLE_CALL_TEXT_MANAGEMENT, 0);
	replace_call(FF8_BATTLE_CALL_TEXT_MANAGEMENT, (void *)pacing_text_management_hook);
	pacing_stage_queue_orig = (void *(__cdecl *)())get_relative_call(FF8_BATTLE_CALL_STAGE_QUEUE, 0);
	replace_call(FF8_BATTLE_CALL_STAGE_QUEUE, (void *)pacing_stage_queue_hook);
	pacing_stage_142_fade_orig = (void *(__cdecl *)())get_relative_call(FF8_BATTLE_CALL_STAGE_142_FADE, 0);
	replace_call(FF8_BATTLE_CALL_STAGE_142_FADE, (void *)pacing_stage_142_fade_hook);
	pacing_tdamage_ri = replace_function(FF8_BATTLE_DAMAGE_NUMBER_TASK, (void *)pacing_tdamage_hook);
	pacing_ttext_ri = replace_function(FF8_BATTLE_TEXT_TASK, (void *)pacing_ttext_hook);
	pacing_tfade_ri = replace_function(FF8_BATTLE_SCREEN_FADE_TASK, (void *)pacing_tfade_hook);
	pacing_tflash_ri = replace_function(FF8_BATTLE_SCREEN_FLASH_TASK, (void *)pacing_tflash_hook);
	pacing_toscillation_ri = replace_function(FF8_BATTLE_CAMERA_OSCILLATION_TASK, (void *)pacing_toscillation_hook);
	pacing_tstage147_ri = replace_function(FF8_BATTLE_STAGE_147_TASK, (void *)pacing_tstage147_hook);

	// Battle director counters
	pacing_status_timers_ri = replace_function(FF8_BATTLE_STATUS_TIMERS, (void *)pacing_status_timers_hook);
	pacing_gilgamesh_ri = replace_function(FF8_BATTLE_GILGAMESH_ANGELO, (void *)pacing_gilgamesh_hook);

	pacing_enabled = true;

	ffnx_info("battle pacing: battle at %d fps, logic at 15 ticks/s (1 real tick every %d frames), UI %d tick(s) per frame\n",
		15 * pacing_n, pacing_n, 4 / pacing_n);

	return true;
}
