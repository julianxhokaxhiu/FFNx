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

// Effect 335: Renzokuken vs NORG

#include "mag_common.h"

namespace ff8fx
{
namespace renzo335
{
	static constexpr game::Id PTS_A = game::unk_21F4418, PTS_B = game::unk_21F4458, TARGET_SLOT = game::unk_21F4498;
	static constexpr game::Id POOL_A = game::unk_21F44A0, POOL_A_END = game::unk_21F4680;
	static constexpr game::Id POOL_B = game::unk_21F4680, POOL_B_SPAWN_END = game::unk_21F4900, POOL_B_END = game::unk_21F4A40;
	static constexpr game::Id HIT_REC = game::unk_21F4A40, MASTER_POOL = game::unk_21F5190, Q_MASTER = game::unk_21F51A0;
	static constexpr game::Id SLOTS = game::unk_21F51B0, SLOTS_END = game::unk_21F5E30;
	static constexpr game::Id POOL_C = game::unk_21F5E30, POOL_C_END = game::unk_21F6150;
	static constexpr game::Id SPL_A = game::unk_21F6150, SPL_B = game::unk_21F6250;
	static constexpr game::Id HIT_POS = game::unk_21F6350, HIT_DIR = game::unk_21F6358, Q_MAIN = game::unk_21F6368, MAIN_POOL = game::unk_21F6378;
	static constexpr game::Id PREV_HIT = game::unk_21F7188, CONTEXT = game::unk_21F71A0, CASTER_SLOT = game::unk_21F71A4, TEX_FILE = game::unk_21F71A8;
	static constexpr game::Id RING = game::unk_21F71B0, RING_END = game::unk_21F723C, LENGTH = game::unk_21F723C, FINISHER = game::unk_21F7240;
	static constexpr game::Id TEX_BASE = game::unk_21F7244, PACKET_CURSOR = game::unk_21F7248;
	static const uint32_t PARITY = 0xC000;
	static const void *SOUND() { return (const void *)game_addr(game::unk_E6C48C); }
	static constexpr game::Id TEXTURE_NAME = game::aMag333Tim_6, CAMERA = game::MAG335_RENZOKUKEN_VS_NORG_cameraAnim, ANIM_SETS = game::unk_E6CC38, BIND_OUT = game::unk_24FEAA8;
	static constexpr game::Id MODEL_SPARKLE = game::unk_E6C490, SPARKLE_FADE = game::unk_E6C618, SPARKLE_DEPTH = game::unk_E6C608;
	static constexpr game::Id MODEL_SLASH = game::unk_CB2EA4, MODEL_STREAK = game::unk_CB388C;
	static constexpr game::Id SEQ_FLASH = game::unk_CB291C, SEQ_DEBRIS = game::unk_CB2BEC, SEQ_SPARK = game::unk_CB2AC8, SPARK_FRAMES = game::unk_CB94C0;
	static constexpr game::Id SLASH_REF = game::unk_CB9520, HIT_LEAD = game::unk_CB9530, HIT_OFFS = game::unk_CB9560;
	static constexpr game::Id SEQ_EMIT = game::unk_CB2770, RING_REF = game::unk_CB95D0;
	static constexpr game::Id ORIG_FileLoader = game::MAG_335_UNKNOWN_FL, ORIG_Setup = game::MAG_335_UNKNOWN;
	static constexpr game::Id ORIG_Controller = game::sub_5857D0, ORIG_HitSpark = game::sub_585C70, ORIG_Sparkle = game::sub_585F00, ORIG_Trail = game::sub_586230, ORIG_SparkSpawn = game::sub_5867B0, ORIG_Hit = game::sub_5868E0, ORIG_Slash = game::sub_586CB0, ORIG_BossEmitter = game::sub_586F90, ORIG_Ring = game::sub_5872C0, ORIG_Master = game::MAG_335_sub_587500;

#include "renzo_boss_defs.inc"
}
}

namespace ff8fx
{
namespace renzo335
{
#define RENZO_ID 335
#define RENZO_IDS "335"
	static void Events(Node24 *n);
#include "renzo_boss.inc"
#undef RENZO_IDS
#undef RENZO_ID

	static const SlotModelDef SLOTMODELS[1] = {
		{ ORIG_Ring, game::unk_CB44F4, RING_REF, 3, 6, 341, 0x12, false },
	};

	static void Events(Node24 *n)
	{
		if (n->c == 0xD)
		{
			int16_t p[4];
			GetDefaultEffectPosition(Target(), p);
			uint8_t *e = Caster();
			*(int16_t *)(e + 0x1C) = p[0];
			*(int16_t *)(e + 0x20) = (int16_t)(p[2] + 0xF0A);
		}
		if (n->c > 0xD && (int32_t)n->c == Length() - 5)
		{
			uint8_t *e = Caster();
			*(int16_t *)(e + 0x1C) = *(const int16_t *)(e + 0x14);
			*(int16_t *)(e + 0x20) = *(const int16_t *)(e + 0x18);
		}
		if (n->c == 0x28)
		{
			Node24 *t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_BossEmitter));
			ff8fx::copy<(8)>(&t->w10, (const void *)game_addr(HIT_POS));
			t->c = 0;
		}
		if (n->c == 0x3B)
		{
			Node24 *t = AddSlotTask(game_addr(ORIG_Ring));
			const int16_t s = (int16_t)((CrtRand() % 0x600 + 0x1000) / 3);
			t->w1E = s;
			t->w1C = s;
			HitPointRecord(t);
		}
	}

	static uint32_t __cdecl BossEmitterTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		EmitWork *w = (EmitWork *)FieldAlloc(0x50);
		if (!DrawOnly() && n->c <= 0x10)
		{
			GetEffectSpawnPosition(Target(), 0xF0, 0x1000, w->pos2);
			w->pos2[1] = (int16_t)(w->pos2[1] - 600);
			w->pos2[2] = (int16_t)(w->pos2[2] - 0x226);
			for (int32_t j = 0; j < 3; j++)
			{
				const int32_t i = FreeRecord();
				if (i < 0) break;
				SlotRec *p = Slot(i);
				p->alive = 2;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x600 + 0x800);
				ff8fx::copy<(8)>(p->pos, w->pos2);
				w->dir[0] = CrtRand() % 0x600 - 0x300;
				w->dir[1] = -0x900 - CrtRand() % 0x300;
				w->dir[2] = CrtRand() % 0x300 + 0x800;
				Normalize(w->dir, w->dir);
				const int32_t sp = CrtRand() % 0x12C + 0x5A;
				p->w10 = (int16_t)(mul32(w->dir[0], sp) >> 12);
				p->w12 = (int16_t)(mul32(w->dir[1], sp) >> 12);
				p->w14 = (int16_t)(mul32(w->dir[2], sp) >> 12);
			}
		}
		const int32_t alive = EmitterDraw(n, w, 2, game_addr(SEQ_EMIT));
		FieldFree(0x50);
		if (DrawOnly()) return 0;
		n->c++;
		return alive ? 0 : TASK_END;
	}

	static uint32_t __cdecl RingTask(TaskNode *tn) { return SlotModelTask(SLOTMODELS[0], 0, (Node24 *)tn); }
}

	void register_mag335_renzokuken_norg()
	{
		renzo335::RegisterCommon();
		register_port(renzo335::ORIG_BossEmitter, (void *)renzo335::BossEmitterTask, "335 BossEmitter", 335);
		register_port(renzo335::ORIG_Ring, (void *)renzo335::RingTask, "335 Ring", 335);
	}
}

