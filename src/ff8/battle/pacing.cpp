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

#include "pacing.h"

#include "../../ff8.h"
#include "../../patch.h"
#include "../../globals.h"
#include "../../cfg.h"
#include "../../common.h"
#include "../../log.h"

#include <string.h>

// The PC battle module runs at 15 fps and catches the battle UI up by running its UI tick 4
// times in a row per frame. Here the battle main loop runs at 60 fps: one frame in 4 is a
// "real tick", where every battle subsystem advances exactly like in one original frame; the
// 3 frames in between are "held": nothing advances, the last real tick is drawn again. The
// battle UI runs once per frame instead of 4 times in a row, so it keeps its 60 ticks per
// second, and the pad is read every frame. See docs/ff8/battle_60fps_ui.md

#define PACING_FRAMES_PER_TICK 4

// battle_main_loop: the 3 catch-up UI tick pairs (update, display) run before the frame's pad read
static const uint32_t pacing_catch_up_ui_calls[] = { 0x13D, 0x142, 0x147, 0x14C, 0x151, 0x156 };

// Battle UI context
#define PACING_UI_CTX_FRESH_INPUT 0x21   // 1 on the UI tick that latched a fresh pad snapshot
#define PACING_UI_CTX_BLINK_COUNTER 0x2C // HUD blink / pulse counter, incremented on each latch

// SSIGPU execution node arena (draw list), same limit as the engine inserts
#define PACING_SSIGPU_EXEC_LIMIT 0x60000
#define PACING_BATTLE_OT_BUCKETS 0x1122

static int pacing_phase = 0; // position of the current frame in its tick: 0 = real tick
static uint32_t pacing_frame = 0;
static bool pacing_new_battle = true;
static bool pacing_enabled = false;

// Calls the original of a function hooked with replace_function
struct pacing_unhooked
{
	uint32_t ri;
	pacing_unhooked(uint32_t replacement) : ri(replacement) { unreplace_function(ri); }
	~pacing_unhooked() { rereplace_function(ri); }
};

// Original targets of the redirected calls
static void (*pacing_status_visuals_orig)(uint8_t *) = nullptr;
static int (*pacing_entity_fades_orig)(uint8_t *) = nullptr;
static char (*pacing_text_management_orig)() = nullptr;
static void *(*pacing_stage_queue_orig)() = nullptr;
static void *(*pacing_stage_142_fade_orig)() = nullptr;

// ---------------------------------------------------------------------------
// Effects and hit effects: they tick on real ticks only, their draws are redrawn on held frames
// ---------------------------------------------------------------------------
// The effect tree (magic, GF, limit breaks, Draw) and the hit-effect queue both advance and
// draw in the same call. Their draws are 24-byte SSIGPU execution nodes linked into the battle
// ordering table: the nodes created during a real tick are exactly [cursor before, cursor
// after). Their packets are copied at the end of the real tick and linked again into every
// held frame's ordering table, so held frames show exactly what the real tick drew. Nothing
// runs twice: no sound, damage number or particle is triggered again.

#define PACING_REC_MAX_PRIMS 0x1000
#define PACING_REC_ARENA_WORDS 0x40000 // 1 MB of packet copies per queue

#pragma pack(push, 1)
struct pacing_exec_node
{
	uint32_t *pkt;   // packet (tag low 24 bits: previous bucket head, low part)
	int32_t k[4];    // depth keys
	uint16_t msk;    // bit 0: alternate viewport mode
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
	pacing_prim prim[PACING_REC_MAX_PRIMS];
	uint32_t arena[PACING_REC_ARENA_WORDS];
	int count;
	uint32_t used;
	bool valid;         // the last real tick's draws are recorded
	int last_ret;       // queue return value of the last real tick
	uint32_t rec_begin; // arena cursor before the real tick
	uint32_t *rec_ot;   // ordering table of the real tick
	void *faulted[0x10]; // queues whose copy/redraw faulted: not redrawn any more
	int faulted_count;
	bool overflow_logged;
};

static pacing_recorder pacing_rec_effect = { "effect" };
static pacing_recorder pacing_rec_hit = { "hit effect" };
static int16_t pacing_node_bucket[PACING_REC_MAX_PRIMS];

static uint32_t *pacing_current_ot()
{
	return (uint32_t *)(*ff8_externals.battle_render_list_base + 68);
}

// VRAM transfers (framebuffer copies of mirror/warp effects) are not redrawn: repeating them
// on a held frame would copy whatever the screen holds by then. Returns true if the packet
// has a VRAM transfer or an unknown GP0 command.
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
		else if (cmd >= 0x60 && cmd < 0x80) w += 2 + ((cmd & 0x04) ? 1 : 0) + (((cmd & 0x18) == 0) ? 1 : 0); // rectangle / sprite
		else if ((cmd >= 0xE1 && cmd <= 0xE6) || p[w] == 0) w++; // draw mode / area / offset / NOP
		else return true;
	}

	return false;
}

static bool pacing_rec_faulted(pacing_recorder &rec, void *ctx)
{
	for (int i = 0; i < rec.faulted_count; i++)
		if (rec.faulted[i] == ctx) return true;

	return false;
}

static void pacing_rec_add_faulted(pacing_recorder &rec, void *ctx)
{
	if (!pacing_rec_faulted(rec, ctx) && rec.faulted_count < 0x10) rec.faulted[rec.faulted_count++] = ctx;
}

// End of a real tick: copy every node the queue created, while the packets are fresh
static void pacing_rec_capture(pacing_recorder &rec)
{
	uint32_t begin = rec.rec_begin, end = *ff8_externals.ssigpu_exec_cur;

	if (end < begin || (end - begin) % sizeof(pacing_exec_node) != 0) return; // arena flushed during the tick

	int n = (end - begin) / sizeof(pacing_exec_node);

	if (n > PACING_REC_MAX_PRIMS)
	{
		if (!rec.overflow_logged && (trace_all || trace_battle_animation)) ffnx_trace("battle pacing: %d draws in one %s tick, held frames not redrawn\n", n, rec.name);
		rec.overflow_logged = true;
		return;
	}

	for (int i = 0; i < n; i++) pacing_node_bucket[i] = -1;

	// Bucket of each node: walk every bucket's chain while it stays inside the tick's nodes
	// (nothing else inserts during the tick, so the tick's nodes sit on top of each chain)
	for (int b = 0; b < PACING_BATTLE_OT_BUCKETS; b++)
	{
		uint32_t v = rec.rec_ot[b];

		while (v >= begin && v < end && (v - begin) % sizeof(pacing_exec_node) == 0)
		{
			int i = (v - begin) / sizeof(pacing_exec_node);
			if (pacing_node_bucket[i] >= 0) break;
			pacing_node_bucket[i] = int16_t(b);
			const pacing_exec_node *node = (const pacing_exec_node *)v;
			v = (node->pkt[0] & 0xFFFFFF) | (uint32_t(node->link_hi) << 24);
		}
	}

	for (int i = 0; i < n; i++)
	{
		const pacing_exec_node *node = (const pacing_exec_node *)(begin + i * sizeof(pacing_exec_node));

		if (pacing_node_bucket[i] < 0) continue; // not linked (discarded by the engine)

		uint32_t words = (node->pkt[0] >> 24) + 1;

		if (pacing_packet_has_transfer(node->pkt, words)) continue;
		if (rec.used + words > PACING_REC_ARENA_WORDS) return;

		pacing_prim &d = rec.prim[rec.count++];
		memcpy(d.k, node->k, sizeof(d.k));
		d.bucket = uint16_t(pacing_node_bucket[i]);
		d.msk = node->msk;
		d.off = rec.used;
		d.words = words;
		memcpy(&rec.arena[rec.used], node->pkt, words * 4);
		rec.used += words;
	}

	rec.valid = true;
}

// Held frame: link the recorded packets into this frame's ordering table, exactly like the
// engine's inserts do (node from the execution arena, packet tag relinked)
static void pacing_rec_redraw(pacing_recorder &rec)
{
	uint32_t *ot = pacing_current_ot();

	for (int i = 0; i < rec.count; i++)
	{
		if (*ff8_externals.ssigpu_exec_cur - uint32_t(ff8_externals.ssigpu_exec_start) >= PACING_SSIGPU_EXEC_LIMIT) break;

		const pacing_prim &e = rec.prim[i];
		uint32_t *pkt = &rec.arena[e.off];
		pacing_exec_node *node = (pacing_exec_node *)*ff8_externals.ssigpu_exec_cur;
		uint32_t *bucket = ot + e.bucket;
		uint32_t old_head = *bucket;

		node->pkt = pkt;
		memcpy(node->k, e.k, sizeof(node->k));
		node->msk = e.msk;
		node->link_hi = uint8_t(old_head >> 24);
		node->pad = 0;
		*bucket = uint32_t(node);
		pkt[0] = (pkt[0] & 0xFF000000) | (old_head & 0xFFFFFF);
		*ff8_externals.ssigpu_exec_cur += sizeof(pacing_exec_node);
	}
}

static int pacing_queue_tick(pacing_recorder &rec, void *ctx, int held_ret)
{
	int (*execute_task_queue)(void *) = (int (*)(void *))ff8_externals.battle_execute_task_queue_sub_508420;

	if (pacing_phase == 0)
	{
		rec.rec_begin = *ff8_externals.ssigpu_exec_cur;
		rec.rec_ot = pacing_current_ot();

		int r = execute_task_queue(ctx);

		rec.last_ret = r;
		rec.valid = false;
		rec.count = 0;
		rec.used = 0;

		// r == 0: queue empty, the effect is finished
		if (r != 0 && !pacing_rec_faulted(rec, ctx))
		{
			__try
			{
				pacing_rec_capture(rec);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				rec.valid = false;
				pacing_rec_add_faulted(rec, ctx);
				ffnx_warning("battle pacing: %s draws could not be copied, held frames not redrawn\n", rec.name);
			}
		}

		return r;
	}

	if (rec.valid && !pacing_rec_faulted(rec, ctx))
	{
		__try
		{
			pacing_rec_redraw(rec);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			pacing_rec_add_faulted(rec, ctx);
			ffnx_warning("battle pacing: %s draws could not be redrawn, held frames not redrawn\n", rec.name);
		}
	}

	return held_ret;
}

// Held frames report "still running" (the caller would otherwise drop the effect)
static int pacing_effect_tick(void *effect_ctx)
{
	return pacing_queue_tick(pacing_rec_effect, effect_ctx, 1);
}

static int pacing_hit_effect_tick(void *queue)
{
	return pacing_queue_tick(pacing_rec_hit, queue, pacing_rec_hit.last_ret);
}

// Screen feedback (Eden and others draw a ghost of the whole screen): an effect tick arms a
// one-frame request that the battle loop consumes after the frame. Effects tick on real ticks
// only, so the request of the last real tick is armed again on held frames.
static int pacing_feedback_request = 0;

static int pacing_feedback_request_hook(int mode)
{
	*ff8_externals.battle_screen_feedback_request = mode + 1;
	pacing_feedback_request = mode + 1;

	return mode + 1;
}

// ---------------------------------------------------------------------------
// Tasks that advance and draw in the same call
// ---------------------------------------------------------------------------
// Such a task draws with its state, then advances it. On a real tick its state is recorded
// before the call; a held frame puts that state back, calls the task (same draw as the real
// tick), then restores the current state. A task first seen on a held frame draws with its
// current state. The memo of a task that ended is dropped (its node can be reused).
// Task node data starts at +0xC; a task returns 0 to keep running, 2 to end.

#define PACING_TASK_MEMOS 0x20
#define PACING_TASK_MEMO_BYTES 0x20

static struct pacing_task_memo
{
	uint8_t *node;
	uint32_t frame; // frame the state was recorded on
	uint8_t data[PACING_TASK_MEMO_BYTES];
} pacing_task_memos[PACING_TASK_MEMOS];

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

static bool pacing_memo_is_fresh(uint32_t frame)
{
	return pacing_frame - frame < PACING_FRAMES_PER_TICK;
}

static DWORD pacing_task_redraw(uint8_t *node, uint32_t size, uint32_t ri, uint32_t task)
{
	uint8_t *data = node + 0xC;

	if (pacing_phase == 0)
	{
		pacing_task_memo *m = pacing_task_memo_find(node, true);

		if (m)
		{
			memcpy(m->data, data, size);
			m->frame = pacing_frame;
		}

		DWORD r;
		{ pacing_unhooked u(ri); r = ((DWORD (*)(uint8_t *))task)(node); }

		if (r == 2 && m) m->node = nullptr;

		return r;
	}

	uint8_t current[PACING_TASK_MEMO_BYTES];
	pacing_task_memo *m = pacing_task_memo_find(node, false);

	memcpy(current, data, size);
	if (m && pacing_memo_is_fresh(m->frame)) memcpy(data, m->data, size);
	{ pacing_unhooked u(ri); ((DWORD (*)(uint8_t *))task)(node); }
	memcpy(data, current, size);

	return 0; // a held frame never ends a task
}

static uint32_t pacing_damage_number_ri = 0, pacing_screen_fade_ri = 0;

static DWORD pacing_damage_number_task(uint8_t *node) { return pacing_task_redraw(node, 0x14, pacing_damage_number_ri, ff8_externals.battle_task_damage_number_sub_5069B0); }
static DWORD pacing_screen_fade_task(uint8_t *node) { return pacing_task_redraw(node, 0x8, pacing_screen_fade_ri, ff8_externals.battle_task_screen_fade_sub_501D10); }

// Tasks that only count: held frames skip them
static uint32_t pacing_wobble_ri = 0, pacing_texture_blink_ri = 0, pacing_footstep_ri = 0, pacing_camera_shake_ri = 0, pacing_move_ri = 0, pacing_camera_oscillation_ri = 0;

static DWORD pacing_counter_task(uint8_t *node, uint32_t ri, uint32_t task)
{
	if (pacing_phase != 0) return 0;

	pacing_unhooked u(ri);

	return ((DWORD (*)(uint8_t *))task)(node);
}

static DWORD pacing_wobble_task(uint8_t *node) { return pacing_counter_task(node, pacing_wobble_ri, ff8_externals.battle_task_84_wobble_sub_501F90); }
static DWORD pacing_texture_blink_task(uint8_t *node) { return pacing_counter_task(node, pacing_texture_blink_ri, ff8_externals.battle_task_9f_texture_blink_sub_5057D0); }
static DWORD pacing_footstep_task(uint8_t *node) { return pacing_counter_task(node, pacing_footstep_ri, ff8_externals.battle_task_99_footstep_sub_50F830); }
static DWORD pacing_camera_shake_task(uint8_t *node) { return pacing_counter_task(node, pacing_camera_shake_ri, ff8_externals.battle_task_96_camera_shake_sub_50F6C0); }
static DWORD pacing_move_task(uint8_t *node) { return pacing_counter_task(node, pacing_move_ri, ff8_externals.battle_task_9e_move_sub_50F750); }
static DWORD pacing_camera_oscillation_task(uint8_t *node) { return pacing_counter_task(node, pacing_camera_oscillation_ri, ff8_externals.battle_task_camera_oscillation_sub_509930); }

// Drag target to attacker bone: snaps the target to the bone every frame; only its frames-left
// counter (+0x18) counts at the original pace, and a held frame never ends the task
static uint32_t pacing_drag_ri = 0;

static DWORD pacing_drag_task(uint8_t *node)
{
	pacing_unhooked u(pacing_drag_ri);
	DWORD (*task)(uint8_t *) = (DWORD (*)(uint8_t *))ff8_externals.battle_task_ad_drag_sub_50F500;

	if (pacing_phase == 0) return task(node);

	int32_t left = *(int32_t *)(node + 0x18);

	if (left == 1) return 0;

	DWORD r = task(node);
	*(int32_t *)(node + 0x18) = left;

	return r;
}

// Restore model part: draws the fading part in its call, its fade counter (+0x13) only
// counts on real ticks
static uint32_t pacing_restore_part_ri = 0;

static DWORD pacing_restore_part_task(uint8_t *node)
{
	pacing_unhooked u(pacing_restore_part_ri);
	DWORD (*task)(uint8_t *) = (DWORD (*)(uint8_t *))ff8_externals.battle_task_81_restore_part_sub_50F0E0;

	if (pacing_phase == 0) return task(node);

	uint8_t state = node[0x12], counter = node[0x13];

	task(node);
	node[0x13] = (state == 0) ? 0 : counter; // first call: init only, counting starts on the next real tick

	return 0;
}

// Detached model part: draws, then integrates its physics (node +0xC..+0x2B and the saved
// matrix). A held frame draws the state the real tick drew (physics skipped through the battle
// "paused" flag for that call), then the current state is put back.
static uint32_t pacing_detach_part_ri = 0;
static struct { uint8_t *node; uint8_t state[0x20], matrix[0x20]; bool valid; } pacing_detach_part_memo;

static DWORD pacing_detach_part_task(uint8_t *node)
{
	pacing_unhooked u(pacing_detach_part_ri);
	DWORD (*task)(uint8_t *) = (DWORD (*)(uint8_t *))ff8_externals.battle_task_a6_detach_part_sub_50F2E0;
	auto &memo = pacing_detach_part_memo;

	if (pacing_phase == 0)
	{
		memo.node = node;
		memcpy(memo.state, node + 0xC, sizeof(memo.state));
		memcpy(memo.matrix, ff8_externals.battle_detached_part_matrix, sizeof(memo.matrix));
		memo.valid = node[0x12] != 0; // initialised (the first call copies the bone matrix)

		return task(node);
	}

	uint8_t node_current[0x20], matrix_current[0x20];
	uint32_t flags = *ff8_externals.battle_update_flags;

	memcpy(node_current, node + 0xC, sizeof(node_current));
	memcpy(matrix_current, ff8_externals.battle_detached_part_matrix, sizeof(matrix_current));

	if (memo.valid && memo.node == node && node[0x12] != 0)
	{
		memcpy(node + 0xC, memo.state, sizeof(memo.state));
		memcpy(ff8_externals.battle_detached_part_matrix, memo.matrix, sizeof(memo.matrix));
		*ff8_externals.battle_update_flags = flags | 1;
	}

	task(node);

	*ff8_externals.battle_update_flags = flags;
	memcpy(node + 0xC, node_current, sizeof(node_current));
	memcpy(ff8_externals.battle_detached_part_matrix, matrix_current, sizeof(matrix_current));

	return 0;
}

// Battle message: the task claims its text channel every frame (that is what displays it) and
// counts its display time (+0xE) down; held frames claim without counting
static uint32_t pacing_battle_text_ri = 0;

static DWORD pacing_battle_text_task(uint8_t *node)
{
	pacing_unhooked u(pacing_battle_text_ri);
	DWORD (*task)(uint8_t *) = (DWORD (*)(uint8_t *))ff8_externals.battle_task_text_sub_506F70;

	if (pacing_phase == 0) return task(node);

	uint8_t time_left = node[0xE];

	if (time_left == 0) node[0xE] = 1; // shown until the next real tick ends it
	task(node);
	node[0xE] = time_left;

	return 0;
}

// Stage 147 intro script (moves the party in, plays steps, holds the battle meanwhile): held
// frames only draw the stage and keep the battle held (node +0xC state, +0xE script step,
// +0x10 step counter)
static uint32_t pacing_stage_147_ri = 0;

static DWORD pacing_stage_147_task(uint8_t *node)
{
	if (pacing_phase == 0)
	{
		pacing_unhooked u(pacing_stage_147_ri);

		return ((DWORD (*)(uint8_t *))ff8_externals.battle_task_stage_147_sub_511EF0)(node);
	}

	if (*(uint16_t *)(node + 0xC) == 1)
	{
		((void (*)(int))ff8_externals.battle_stage_texture_animation_sub_51B0D0)(0);
		((void (*)(int))ff8_externals.battle_stage_texture_animation_sub_51B0D0)(1);
		((void (*)())ff8_externals.battle_stage_render_sub_500FD0)();

		uint16_t step = *(uint16_t *)(node + 0xE);

		if (step == 1 || (step == 5 && *(int16_t *)(node + 0x10) < 5)) *ff8_externals.battle_tasks_busy = 1;
	}

	return 0;
}

// ---------------------------------------------------------------------------
// Battle entities
// ---------------------------------------------------------------------------

// Status visuals (colour pulse, Float bob, spin, status sprites, Doom counter) advance their
// counters (32-byte block at entity +0x88) and draw in the same call
#define PACING_ENTITY_MEMOS 0x10

static struct pacing_entity_memo
{
	uint8_t *entity;
	uint32_t frame;
	uint8_t data[0x20];
} pacing_status_memos[PACING_ENTITY_MEMOS];

static pacing_entity_memo *pacing_entity_memo_find(pacing_entity_memo *memos, uint8_t *entity, bool create)
{
	pacing_entity_memo *free_slot = nullptr;

	for (int i = 0; i < PACING_ENTITY_MEMOS; i++)
	{
		if (memos[i].entity == entity) return &memos[i];
		if (!memos[i].entity && !free_slot) free_slot = &memos[i];
	}

	if (create && free_slot) free_slot->entity = entity;

	return create ? free_slot : nullptr;
}

static void pacing_status_visuals(uint8_t *entity)
{
	void (*status_visuals)(uint8_t *) = pacing_status_visuals_orig;
	uint8_t *block = *(uint8_t **)(entity + 0x88);

	if (!block)
	{
		if (pacing_phase == 0) status_visuals(entity);

		return;
	}

	pacing_entity_memo *memo = pacing_entity_memo_find(pacing_status_memos, entity, pacing_phase == 0);

	if (pacing_phase == 0)
	{
		if (memo)
		{
			memcpy(memo->data, block, sizeof(memo->data));
			memo->frame = pacing_frame;
		}
		status_visuals(entity);

		return;
	}

	uint8_t current[0x20];

	memcpy(current, block, sizeof(current));
	if (memo && pacing_memo_is_fresh(memo->frame)) memcpy(block, memo->data, sizeof(current));
	status_visuals(entity);
	memcpy(block, current, sizeof(current));
}

// Death / escape / appear / GF-summon fades (entity +5 fade kind, +6 fade counter). The
// per-entity task resets the model colours every frame (status visuals) and this call applies
// the fade to them. Some fade kinds also trigger the victory sequence, death flags or roll the
// battle RNG, so held frames do not run it again: they put back the colours the real tick
// produced (model and shadow palettes +0x28..+0x2F, shadow alpha +7). A fade starting on a
// held frame is drawn from its current state (side-effect-free kinds only, counter untouched).
static struct pacing_fade_memo
{
	uint8_t *entity;
	uint32_t frame;
	uint8_t palettes[8];
	uint8_t alpha;
} pacing_fade_memos[PACING_ENTITY_MEMOS];

static int pacing_entity_fades(uint8_t *entity)
{
	int (*entity_fades)(uint8_t *) = pacing_entity_fades_orig;
	pacing_fade_memo *memo = nullptr, *free_slot = nullptr;

	for (pacing_fade_memo &m : pacing_fade_memos)
	{
		if (m.entity == entity) { memo = &m; break; }
		if (!m.entity && !free_slot) free_slot = &m;
	}

	if (pacing_phase == 0)
	{
		int r = entity_fades(entity);

		if (!memo) memo = free_slot;
		if (memo)
		{
			memo->entity = entity;
			memo->frame = pacing_frame;
			memcpy(memo->palettes, entity + 0x28, sizeof(memo->palettes));
			memo->alpha = entity[7];
		}

		return r;
	}

	if (memo && pacing_memo_is_fresh(memo->frame))
	{
		memcpy(entity + 0x28, memo->palettes, sizeof(memo->palettes));
		entity[7] = memo->alpha;

		return 0; // 0 = draw the entity
	}

	switch (entity[5])
	{
	case 1: case 2: case 3: case 4: case 9: case 10: case 11: case 12:
	{
		uint8_t state[7];
		uint32_t flags = *ff8_externals.battle_update_flags;

		memcpy(state, entity, sizeof(state));
		*ff8_externals.battle_update_flags = flags | 1; // draw only: most fades do not count while set
		entity_fades(entity);
		*ff8_externals.battle_update_flags = flags;
		memcpy(entity, state, sizeof(state));
		break;
	}
	}

	return 0;
}

// ---------------------------------------------------------------------------
// Models: animation and choreography advance on real ticks only
// ---------------------------------------------------------------------------

static uint32_t pacing_read_animation_ri = 0, pacing_animseq_ri = 0;

// A fault while decoding an animation stream finishes that animation instead of crashing
static int pacing_read_animation_guarded(void *anim_header, uint8_t *anim_cmd)
{
	__try
	{
		return ((int (*)(void *, void *))ff8_externals.battle_read_animation_sub_508F90)(anim_header, anim_cmd);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		ffnx_warning("battle pacing: animation %u could not be read (frame %u/%u), animation finished\n", anim_cmd[0], anim_cmd[6], anim_cmd[7]);
		anim_cmd[6] = anim_cmd[7];

		return 1; // animation complete
	}
}

// Held frames rebuild the model geometry from the current pose without advancing it (the
// geometry is double buffered: skipping the call would draw a stale buffer).
// anim_cmd +6: current frame, +7: total frames
static int pacing_read_animation(void *anim_header, uint8_t *anim_cmd)
{
	void (*build_bone_matrices)(void *) = (void (*)(void *))ff8_externals.battle_build_bone_matrices_sub_508C90;

	// A completed animation returns without rebuilding: rebuild here as well
	if (anim_cmd[6] >= anim_cmd[7])
	{
		build_bone_matrices(anim_header);

		return 1;
	}

	// Frame 0 is the absolute base pose read right after the bones were zeroed: never held
	if (pacing_phase != 0 && anim_cmd[6] != 0)
	{
		build_bone_matrices(anim_header);

		return 0; // frame processed, animation not complete
	}

	pacing_unhooked u(pacing_read_animation_ri);

	return pacing_read_animation_guarded(anim_header, anim_cmd);
}

// The choreography VM (movement, animation changes, delays, sounds) runs on real ticks; held
// frames only let the animation rebuild the geometry
static int pacing_animseq_update_entity(void *entity)
{
	if (pacing_phase != 0)
	{
		((int (*)(void *))ff8_externals.battle_anim_advance_by_1_sub_5094F0)(entity);

		return 0; // the caller ignores the return value
	}

	pacing_unhooked u(pacing_animseq_ri);

	return ((int (*)(void *))ff8_externals.battle_animseq_update_entity_sub_504290)(entity);
}

// ---------------------------------------------------------------------------
// Camera
// ---------------------------------------------------------------------------

static uint32_t pacing_camera_animation_ri = 0, pacing_camera_sequence_ri = 0, pacing_camera_operations_ri = 0;
static int16_t pacing_camera_shake[3] = { 0, 0, 0 };

// Keyframe player: real ticks only (a smaller time step instead would hang its catch-up loop)
static int pacing_camera_animation(void *task)
{
	if (pacing_phase != 0) return 0;

	pacing_unhooked u(pacing_camera_animation_ri);

	return ((int (*)(void *))ff8_externals.battle_camera_animation_sub_5035E0)(task);
}

// Camera script VM: real ticks only (returns the current setting)
static void *pacing_camera_sequence()
{
	if (pacing_phase != 0) return *ff8_externals.battle_camera_setting;

	pacing_unhooked u(pacing_camera_sequence_ri);

	return ((void *(*)())ff8_externals.battle_camera_sequence_sub_509610)();
}

// The shake offsets are applied then cleared every frame; their producers only run on real
// ticks, so the last real values are applied again on held frames
static int pacing_camera_operations()
{
	int16_t *shake = ff8_externals.battle_camera_shake;

	if (pacing_phase == 0) memcpy(pacing_camera_shake, shake, sizeof(pacing_camera_shake));
	else if (shake[0] == 0 && shake[1] == 0 && shake[2] == 0) memcpy(shake, pacing_camera_shake, sizeof(pacing_camera_shake));

	pacing_unhooked u(pacing_camera_operations_ri);

	return ((int (*)())ff8_externals.battle_camera_operations_sub_5033E0)();
}

// ---------------------------------------------------------------------------
// Battle UI: one UI tick per frame
// ---------------------------------------------------------------------------
// The 3 catch-up UI ticks are skipped: the visible UI tick runs every frame, 60 per second.
// The fresh-input latch (UI ctx +0x21) is armed by BdLink, which runs every frame: the engine's
// UI ticks per frame constant becomes 1 so every frame gets its latch (BdLink also runs the menu
// tasks and text services that many times per call). Latches counted as time are brought back
// to the original pace (1 in 4): the HUD blink counter, GF Boost's phases and the Renzokuken
// trigger timeline.

static uint32_t pacing_ui_update_ri = 0, pacing_gf_boost_ri = 0, pacing_renzokuken_ri = 0;
static bool pacing_latch_counts = false;

static int pacing_skipped_ui_tick()
{
	return 0;
}

static int pacing_ui_update()
{
	int r;
	{ pacing_unhooked u(pacing_ui_update_ri); r = ((int (*)())ff8_externals.battle_ui_update_sub_4A8E30)(); }

	uint8_t *ctx = *ff8_externals.battle_ui_ctx;

	pacing_latch_counts = false;

	if (ctx && ctx[PACING_UI_CTX_FRESH_INPUT])
	{
		static uint32_t latches = 0;

		pacing_latch_counts = (latches++ % PACING_FRAMES_PER_TICK) == 0;
		// limit break arrow, blinking command text, list page arrows, active character pulse, countdown flash
		if (!pacing_latch_counts) ctx[PACING_UI_CTX_BLINK_COUNTER]--;
	}

	return r;
}

// Runs a UI function with the fresh-input latch only on the latches that count
static void pacing_with_counting_latch(uint32_t ri, uint32_t function)
{
	uint8_t *ctx = *ff8_externals.battle_ui_ctx;
	uint8_t latch = ctx ? ctx[PACING_UI_CTX_FRESH_INPUT] : 0;

	if (ctx && !pacing_latch_counts) ctx[PACING_UI_CTX_FRESH_INPUT] = 0;
	{ pacing_unhooked u(ri); ((void (*)())function)(); }
	if (ctx) ctx[PACING_UI_CTX_FRESH_INPUT] = latch;
}

// GF Boost counts its safe/danger phases and total window down on latches
static void pacing_gf_boost_update()
{
	pacing_with_counting_latch(pacing_gf_boost_ri, ff8_externals.gf_boost_update_sub_56DD70);
}

// Renzokuken adds 4 UI ticks to its trigger timeline on each latch
static void pacing_renzokuken_update()
{
	pacing_with_counting_latch(pacing_renzokuken_ri, ff8_externals.battle_renzokuken_update_sub_4BA6C0);
}

// Game time and battle countdown: called 4 times per battle loop iteration
static uint32_t pacing_game_time_ri = 0;

static int pacing_game_time_tick()
{
	if (getmode_cached()->driver_mode == MODE_BATTLE)
	{
		static uint32_t calls = 0;

		if ((calls++ % PACING_FRAMES_PER_TICK) != 0) return 0;
	}

	pacing_unhooked u(pacing_game_time_ri);

	return ((int (*)())ff8_externals.game_time_tick_sub_4701B0)();
}

// ---------------------------------------------------------------------------
// Battle loop: counters that advance once per loop iteration run on real ticks only
// ---------------------------------------------------------------------------

static uint32_t pacing_status_timers_ri = 0, pacing_gilgamesh_ri = 0;

static void pacing_status_timers()
{
	if (pacing_phase != 0) return;

	pacing_unhooked u(pacing_status_timers_ri);
	((void (*)())ff8_externals.battle_status_timers_sub_483470)();
}

static void pacing_gilgamesh_angelo()
{
	if (pacing_phase != 0) return;

	pacing_unhooked u(pacing_gilgamesh_ri);
	((void (*)())ff8_externals.battle_gilgamesh_angelo_sub_482F80)();
}

// AI text, AI wait timers and the end of battle fade trigger
static char pacing_text_management()
{
	if (pacing_phase != 0) return 0;

	return pacing_text_management_orig();
}

// Stage tasks draw the stage every frame and advance the sky rotation, texture animation and
// stage scripts: held frames set the engine's own stage freeze bits for that call
static void *pacing_stage_queue()
{
	void *(*stage_queue)() = pacing_stage_queue_orig;

	if (pacing_phase == 0) return stage_queue();

	uint8_t freeze = *ff8_externals.battle_stage_freeze;

	*ff8_externals.battle_stage_freeze = freeze | 0xB; // 1 sky, 2 texture animation, 8 stage scripts
	void *r = stage_queue();
	*ff8_externals.battle_stage_freeze = freeze;

	return r;
}

// Stage 142 palette fade
static void *pacing_stage_142_fade()
{
	if (pacing_phase != 0) return nullptr;

	return pacing_stage_142_fade_orig();
}

// ---------------------------------------------------------------------------
// Battle UI drawn once per battle frame
// ---------------------------------------------------------------------------
// The original battle frame runs 4 UI ticks and draws the UI once: some UI animations count
// per draw, others depend on the position of the UI tick in its frame.

// Cursor fingers: every UI tick records the fingers it shows in the slots of its tick phase
// (0 to 3), and the battle loop draws all the recorded slots once per frame, then resets the
// phase and the slots. The original frame so shows the fingers of its 4 UI ticks together (a
// hand on each target of a multiple target selection), and the finger blink toggles on the
// 4th UI tick of the frame. With one UI tick per frame, the phase keeps counting over 4 frames
// and every frame draws the fingers of the last 4 UI ticks.
#define PACING_FINGER_SLOT_SIZE 0x2C
#define PACING_FINGER_SLOTS 8

static void (*pacing_draw_cursor_fingers_orig)() = nullptr;

// Same reset as the engine's after drawing: inactive, no position, no target, no icon
static void pacing_clear_finger_slot(uint8_t *slot)
{
	slot[0] = 0;
	slot[1] = 0;
	memset(slot + 4, 0xFF, 5 * sizeof(uint32_t));
	slot[0x2B] = 0;
}

static void pacing_draw_cursor_fingers()
{
	uint8_t slots[PACING_FINGER_SLOTS * PACING_FINGER_SLOT_SIZE];
	uint32_t phase = *ff8_externals.battle_ui_tick_phase, phase_base = *ff8_externals.battle_ui_finger_slot_phase_base;

	memcpy(slots, ff8_externals.battle_ui_finger_slots, sizeof(slots));
	pacing_draw_cursor_fingers_orig();
	memcpy(ff8_externals.battle_ui_finger_slots, slots, sizeof(slots));
	*ff8_externals.battle_ui_tick_phase = phase;
	*ff8_externals.battle_ui_finger_slot_phase_base = phase_base;

	// The next UI tick records the fingers of this phase again
	for (int finger = 0; finger < 2; finger++) pacing_clear_finger_slot(ff8_externals.battle_ui_finger_slots + (2 * phase + finger) * PACING_FINGER_SLOT_SIZE);
}

// Active character marker (rotating triangle above the character whose turn it is): BdLink
// draws it, and every draw advances its rotation, colour and brightness. Held frames draw the
// state of the last real tick.
static void (*pacing_draw_active_chara_marker_orig)() = nullptr;

static struct
{
	uint32_t frame, color_table;
	uint16_t color_step, rotation;
	bool valid;
} pacing_marker_memo;

static void pacing_draw_active_chara_marker()
{
	auto &memo = pacing_marker_memo;

	if (pacing_phase == 0)
	{
		memo.frame = *ff8_externals.battle_active_chara_marker_frame;
		memo.color_table = *ff8_externals.battle_active_chara_marker_color_table;
		memo.color_step = *ff8_externals.battle_active_chara_marker_color_step;
		memo.rotation = *ff8_externals.battle_active_chara_marker_rotation;
		memo.valid = true;
	}
	else if (memo.valid)
	{
		*ff8_externals.battle_active_chara_marker_frame = memo.frame;
		*ff8_externals.battle_active_chara_marker_color_table = memo.color_table;
		*ff8_externals.battle_active_chara_marker_color_step = memo.color_step;
		*ff8_externals.battle_active_chara_marker_rotation = memo.rotation;
	}

	pacing_draw_active_chara_marker_orig();
}

// ---------------------------------------------------------------------------
// Frame phase
// ---------------------------------------------------------------------------

static uint32_t pacing_bdlink_ri = 0;
static int pacing_end_countdown = -1; // end of battle countdown when BdLink ran this frame

static void pacing_battle_start()
{
	for (pacing_recorder *rec : { &pacing_rec_effect, &pacing_rec_hit })
	{
		rec->valid = false;
		rec->faulted_count = 0;
		rec->last_ret = 0;
		rec->count = 0;
		rec->used = 0;
	}
	pacing_feedback_request = 0;
	memset(pacing_camera_shake, 0, sizeof(pacing_camera_shake));
	memset(&pacing_detach_part_memo, 0, sizeof(pacing_detach_part_memo));
	memset(pacing_task_memos, 0, sizeof(pacing_task_memos));
	memset(pacing_status_memos, 0, sizeof(pacing_status_memos));
	memset(pacing_fade_memos, 0, sizeof(pacing_fade_memos));
	memset(&pacing_marker_memo, 0, sizeof(pacing_marker_memo));
	for (int i = 0; i < PACING_FINGER_SLOTS; i++) pacing_clear_finger_slot(ff8_externals.battle_ui_finger_slots + i * PACING_FINGER_SLOT_SIZE);
	pacing_latch_counts = false;
}

static int pacing_bdlink()
{
	if (pacing_new_battle)
	{
		pacing_new_battle = false;
		pacing_battle_start();
	}

	if (pacing_phase == 0) pacing_feedback_request = 0; // armed again by the effect if it still wants it
	else if (pacing_feedback_request) *ff8_externals.battle_screen_feedback_request = pacing_feedback_request;

	pacing_end_countdown = *ff8_externals.battle_end_countdown;

	pacing_unhooked u(pacing_bdlink_ri);

	return ((int (*)())ff8_externals.battle_load_textures_sub_500900)();
}

void ff8_battle_pacing_frame_end(uint32_t driver_mode)
{
	if (!pacing_enabled) return;

	if (driver_mode != MODE_BATTLE)
	{
		pacing_new_battle = true;
		pacing_phase = 0; // the first battle frame is a real tick
		pacing_end_countdown = -1;

		return;
	}

	// Once the battle result is decided, the battle loop counts the end of battle down after
	// BdLink, once per iteration, before the battle unloads: held frames do not count
	uint8_t *countdown = ff8_externals.battle_end_countdown;

	if (pacing_phase != 0 && pacing_end_countdown > 0 && pacing_end_countdown != 0xFF && *countdown == pacing_end_countdown - 1) *countdown = uint8_t(pacing_end_countdown);
	pacing_end_countdown = -1;

	pacing_phase = (pacing_phase + 1) % PACING_FRAMES_PER_TICK;
	pacing_frame++;
}

void ff8_battle_pacing_init()
{
	uint32_t bdlink = ff8_externals.battle_load_textures_sub_500900;

	// Frame phase
	pacing_bdlink_ri = replace_function(bdlink, pacing_bdlink);

	// Battle UI: one UI tick per frame
	for (uint32_t offset : pacing_catch_up_ui_calls) replace_call(ff8_externals.battle_main_loop + offset, pacing_skipped_ui_tick);
	patch_code_dword(uint32_t(ff8_externals.battle_ui_ticks_per_frame), 1);
	pacing_ui_update_ri = replace_function(ff8_externals.battle_ui_update_sub_4A8E30, pacing_ui_update);
	pacing_gf_boost_ri = replace_function(ff8_externals.gf_boost_update_sub_56DD70, pacing_gf_boost_update);
	pacing_renzokuken_ri = replace_function(ff8_externals.battle_renzokuken_update_sub_4BA6C0, pacing_renzokuken_update);
	pacing_game_time_ri = replace_function(ff8_externals.game_time_tick_sub_4701B0, pacing_game_time_tick);

	// Models
	pacing_read_animation_ri = replace_function(ff8_externals.battle_read_animation_sub_508F90, pacing_read_animation);
	pacing_animseq_ri = replace_function(ff8_externals.battle_animseq_update_entity_sub_504290, pacing_animseq_update_entity);
	pacing_status_visuals_orig = (void (*)(uint8_t *))get_relative_call(ff8_externals.battle_task_entity_sub_502AB0, 0xAD);
	replace_call(ff8_externals.battle_task_entity_sub_502AB0 + 0xAD, pacing_status_visuals);
	pacing_entity_fades_orig = (int (*)(uint8_t *))get_relative_call(ff8_externals.battle_task_entity_sub_502AB0, 0xC4);
	replace_call(ff8_externals.battle_task_entity_sub_502AB0 + 0xC4, pacing_entity_fades);

	// Camera
	pacing_camera_animation_ri = replace_function(ff8_externals.battle_camera_animation_sub_5035E0, pacing_camera_animation);
	pacing_camera_sequence_ri = replace_function(ff8_externals.battle_camera_sequence_sub_509610, pacing_camera_sequence);
	pacing_camera_operations_ri = replace_function(ff8_externals.battle_camera_operations_sub_5033E0, pacing_camera_operations);

	// Effects and hit effects
	replace_call(bdlink + 0x3A, pacing_effect_tick);
	replace_call(bdlink + 0x23, pacing_hit_effect_tick);
	replace_function(ff8_externals.battle_request_screen_feedback_sub_47CF50, pacing_feedback_request_hook);

	// Tasks
	pacing_damage_number_ri = replace_function(ff8_externals.battle_task_damage_number_sub_5069B0, pacing_damage_number_task);
	pacing_screen_fade_ri = replace_function(ff8_externals.battle_task_screen_fade_sub_501D10, pacing_screen_fade_task);
	pacing_wobble_ri = replace_function(ff8_externals.battle_task_84_wobble_sub_501F90, pacing_wobble_task);
	pacing_texture_blink_ri = replace_function(ff8_externals.battle_task_9f_texture_blink_sub_5057D0, pacing_texture_blink_task);
	pacing_footstep_ri = replace_function(ff8_externals.battle_task_99_footstep_sub_50F830, pacing_footstep_task);
	pacing_camera_shake_ri = replace_function(ff8_externals.battle_task_96_camera_shake_sub_50F6C0, pacing_camera_shake_task);
	pacing_move_ri = replace_function(ff8_externals.battle_task_9e_move_sub_50F750, pacing_move_task);
	pacing_camera_oscillation_ri = replace_function(ff8_externals.battle_task_camera_oscillation_sub_509930, pacing_camera_oscillation_task);
	pacing_drag_ri = replace_function(ff8_externals.battle_task_ad_drag_sub_50F500, pacing_drag_task);
	pacing_restore_part_ri = replace_function(ff8_externals.battle_task_81_restore_part_sub_50F0E0, pacing_restore_part_task);
	pacing_detach_part_ri = replace_function(ff8_externals.battle_task_a6_detach_part_sub_50F2E0, pacing_detach_part_task);
	pacing_battle_text_ri = replace_function(ff8_externals.battle_task_text_sub_506F70, pacing_battle_text_task);
	pacing_stage_147_ri = replace_function(ff8_externals.battle_task_stage_147_sub_511EF0, pacing_stage_147_task);

	// Battle loop and stage
	pacing_status_timers_ri = replace_function(ff8_externals.battle_status_timers_sub_483470, pacing_status_timers);
	pacing_gilgamesh_ri = replace_function(ff8_externals.battle_gilgamesh_angelo_sub_482F80, pacing_gilgamesh_angelo);
	pacing_text_management_orig = (char (*)())get_relative_call(ff8_externals.sub_47CCB0, 0xB4B);
	replace_call(ff8_externals.sub_47CCB0 + 0xB4B, pacing_text_management);
	pacing_stage_queue_orig = (void *(*)())get_relative_call(bdlink, 0x7E);
	replace_call(bdlink + 0x7E, pacing_stage_queue);
	pacing_stage_142_fade_orig = (void *(*)())get_relative_call(ff8_externals.battle_task_stage_142_sub_512980, 0x27);
	replace_call(ff8_externals.battle_task_stage_142_sub_512980 + 0x27, pacing_stage_142_fade);

	// Battle UI drawn once per battle frame
	pacing_draw_cursor_fingers_orig = (void (*)())ff8_externals.battle_draw_cursor_fingers_sub_4A78E0;
	replace_call(ff8_externals.battle_main_loop + 0x1EB, pacing_draw_cursor_fingers);
	pacing_draw_active_chara_marker_orig = (void (*)())ff8_externals.battle_draw_active_chara_marker_sub_4BB090;
	replace_call(bdlink + 0x60, pacing_draw_active_chara_marker);

	pacing_enabled = true;

	if (trace_all || trace_battle_animation) ffnx_trace("battle pacing: battle at 60 fps, battle logic at 15 ticks per second\n");
}
