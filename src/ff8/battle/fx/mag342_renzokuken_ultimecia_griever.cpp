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

#include "mag_common.h"

namespace ff8fx
{
namespace renzo342
{
	static constexpr game::Id PTS_A = game::unk_21E2DA8, PTS_B = game::unk_21E2DE8, TARGET_SLOT = game::unk_21E2E28;
	static constexpr game::Id POOL_A = game::unk_21E2E30, POOL_A_END = game::unk_21E3010;
	static constexpr game::Id POOL_B = game::unk_21E3010, POOL_B_SPAWN_END = game::unk_21E3290, POOL_B_END = game::unk_21E33D0;
	static constexpr game::Id HIT_REC = game::unk_21E33D0, MASTER_POOL = game::unk_21E3B20, Q_MASTER = game::unk_21E3B30;
	static constexpr game::Id SLOTS = game::unk_21E3B40, SLOTS_END = game::unk_21E47C0;
	static constexpr game::Id POOL_C = game::unk_21E47C0, POOL_C_END = game::unk_21E4AE0;
	static constexpr game::Id SPL_A = game::unk_21E4AE0, SPL_B = game::unk_21E4BE0;
	static constexpr game::Id HIT_POS = game::unk_21E4CE0, HIT_DIR = game::unk_21E4CE8, Q_MAIN = game::unk_21E4CF8, MAIN_POOL = game::unk_21E4D08;
	static constexpr game::Id PREV_HIT = game::unk_21E5B18, CONTEXT = game::unk_21E5B30, CASTER_SLOT = game::unk_21E5B34, TEX_FILE = game::unk_21E5B38;
	static constexpr game::Id RING = game::unk_21E5B40, RING_END = game::unk_21E5BCC, LENGTH = game::unk_21E5BCC, FINISHER = game::unk_21E5BD0;
	static constexpr game::Id TEX_BASE = game::unk_21E5BD4, PACKET_CURSOR = game::unk_21E5BD8;
	static const uint32_t PARITY = 0xC000;
	static const void *SOUND() { return (const void *)game_addr(game::unk_E46FA8); }
	static constexpr game::Id TEXTURE_NAME = game::aMag333Tim_0, CAMERA = game::MAG342_RENZOKUKEN_VS_GRIEVER_ULTIMECIA_cameraAnim, ANIM_SETS = game::unk_E476F4, BIND_OUT = game::unk_24FE978;
	static constexpr game::Id MODEL_SPARKLE = game::unk_E46FAC, SPARKLE_FADE = game::unk_E47134, SPARKLE_DEPTH = game::unk_E47124;
	static constexpr game::Id MODEL_SLASH = game::unk_C89934, MODEL_STREAK = game::unk_C8A31C;
	static constexpr game::Id SEQ_FLASH = game::unk_C893AC, SEQ_DEBRIS = game::unk_C8967C, SEQ_SPARK = game::unk_C89558, SPARK_FRAMES = game::unk_C8FF50;
	static constexpr game::Id SLASH_REF = game::unk_C8FFB0, HIT_LEAD = game::unk_C8FFC0, HIT_OFFS = game::unk_C8FFF0;
	static constexpr game::Id SEQ_HIT = game::unk_C89200, MODEL_FACING = game::unk_C8AF84;
	static constexpr game::Id ORIG_FileLoader = game::MAG_342_UNKNOWN_FL, ORIG_Setup = game::MAG_342_UNKNOWN;
	static constexpr game::Id ORIG_Controller = game::MAG_342_sub_5777F0, ORIG_HitSpark = game::MAG_342_sub_577D90, ORIG_Sparkle = game::MAG_342_sub_578020, ORIG_Trail = game::MAG_342_sub_578350, ORIG_SparkSpawn = game::MAG_342_sub_5788D0, ORIG_Hit = game::MAG_342_sub_578A00, ORIG_Slash = game::MAG_342_sub_578DF0, ORIG_HitEmitter = game::MAG_342_sub_5790D0, ORIG_FacingA = game::MAG_342_sub_579440, ORIG_FacingB = game::MAG_342_sub_5795C0, ORIG_Master = game::MAG_342_sub_5797F0;

#include "renzo_boss_defs.inc"
}
}

namespace ff8fx
{
namespace renzo342
{
#define RENZO_ID 342
#define RENZO_IDS "342"
#define RENZO_HIT_EXTRA
#define RENZO_LENGTH_EXTRA
	static int32_t LengthExtra() { return 0x23; }
	static void Events(Node24 *n);
	static void HitExtra(int32_t b);
#include "renzo_boss.inc"
#undef RENZO_HIT_EXTRA
#undef RENZO_LENGTH_EXTRA
#undef RENZO_IDS
#undef RENZO_ID

	static void HitExtra(int32_t b)
	{
		if (b <= 0 || b >= 7) return;
		Node24 *t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_HitEmitter));
		t->c = 0;
		t->e = (int16_t)b;
	}

	static void FacingAhead(Node24 *t, int32_t k)
	{
		int32_t v[3];
		TransformVectorBy3x3Matrix((const Mat4x3 *)(Caster() + 0x40), (const int32_t *)game_addr(SLASH_REF), v);
		SlotRec *r = Slot(t->e);
		r->pos[0] = (int16_t)((mul32(v[0], -k) >> 12) + var<int32_t>(game_addr(HIT_POS)));
		r->pos[1] = (int16_t)((mul32(v[1], -k) >> 12) + var<int16_t>(game_addr(HIT_POS) + 2));
		r->pos[2] = (int16_t)((mul32(v[2], -k) >> 12) + var<int32_t>(game_addr(HIT_POS) + 4));
		r->w18 = var<int16_t>(game_addr(HIT_DIR));
		r->w1A = var<int16_t>(game_addr(HIT_DIR) + 4);
		r->w1C = var<int16_t>(game_addr(HIT_DIR) + 8);
	}

	static void Events(Node24 *n)
	{
		if (n->c == 0x16)
		{
			Node24 *t = AddSlotTask(game_addr(ORIG_FacingA));
			const int16_t s = (int16_t)((CrtRand() % 0x400 + 0x900) / 3);
			t->w1E = s;
			t->w1C = s;
			HitPointRecord(t);
		}
		if (n->c == 0x69)
		{
			Node24 *t = AddSlotTask(game_addr(ORIG_FacingB));
			t->w1A = 0x14;
			const int16_t s = (int16_t)((CrtRand() % 0x300 + 0xA00) / 3);
			t->w1E = s;
			t->w1C = s;
			FacingAhead(t, 100);
		}
		if (n->c == 0x6A)
		{
			Node24 *t = AddSlotTask(game_addr(ORIG_FacingB));
			t->w1A = 0x16;
			const int16_t s = (int16_t)((CrtRand() % 0x580 + 0xD00) / 3);
			t->w1E = s;
			t->w1C = s;
			FacingAhead(t, 800);
		}
	}

	static uint32_t __cdecl HitEmitterTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		EmitWork *w = (EmitWork *)FieldAlloc(0x50);
		if (!DrawOnly() && n->c <= 0xE)
		{
			const HitRec *hr = (const HitRec *)(game_addr(HIT_REC) + 0x9C * n->e);
			ff8fx::copy<(16)>(w->dir, hr->dir);
			ff8fx::copy<(8)>(w->pos, hr->pos);
			for (int32_t j = 0; j < 2; j++)
			{
				const int32_t i = FreeRecord();
				if (i < 0) break;
				SlotRec *p = Slot(i);
				p->alive = n->e;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x980 + 0x700);
				ff8fx::copy<(8)>(p->pos, w->pos);
				p->pos[0] = (int16_t)(p->pos[0] + (int16_t)(CrtRand() % 0x8C - 0x46));
				p->pos[1] = (int16_t)(p->pos[1] + (int16_t)(CrtRand() % 0x8C - 0x46));
				p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(CrtRand() % 0x8C - 0x46));
				const int32_t sp = CrtRand() % 0xAA + 0x19;
				const int32_t rx = CrtRand() % 0x50;
				p->w10 = (int16_t)(rx + (mul32(w->dir[0], sp) >> 12) - 0x28);
				const int32_t ry = CrtRand() % 0x3C;
				p->w12 = (int16_t)(ry + (mul32(w->dir[1], sp) >> 12) - 0x1E);
				const int32_t rz = CrtRand() % 0x50;
				p->w14 = (int16_t)(rz + (mul32(w->dir[2], sp) >> 12) - 0x28);
			}
		}
		const int32_t alive = EmitterDraw(n, w, n->e, game_addr(SEQ_HIT), 5);
		FieldFree(0x50);
		if (DrawOnly()) return 0;
		n->c++;
		return alive ? 0 : TASK_END;
	}

	struct FacingDef { game::Id orig; int16_t ax; int32_t mode, fade_mul; bool node_ticks; };
	static const FacingDef FACINGS[2] = {
		{ ORIG_FacingA, -0x400, 3, 409, false },
		{ ORIG_FacingB, 0x400, 2, 341, true },
	};
	static int32_t FacingFrom(const FacingDef &d, const Node24 *n) { return d.node_ticks ? n->w1A - 0xC : 6; }
	static int32_t FacingTicks(const FacingDef &d, const Node24 *n) { return d.node_ticks ? n->w1A : 0x10; }
	static int32_t FacingFade(const FacingDef &d, const Node24 *n, int32_t c) { return mul32(c - FacingFrom(d, n), d.fade_mul); }

	static void FacingDraw(const FacingDef &d, const Node24 *n, const SlotRec *r, int32_t scale, int32_t fade)
	{
		int16_t ang[3];
		ang[0] = d.ax;
		ang[2] = 0;
		ang[1] = *(const int16_t *)(Caster() + 0xE);
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[1] = r->pos[1];
		m.t[0] = r->pos[0];
		int32_t sv[3];
		sv[2] = scale;
		sv[1] = scale;
		sv[0] = scale;
		m.t[2] = r->pos[2];
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_FACING);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (n->c >= FacingFrom(d, n))
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), d.mode, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t FacingTask(int kind, Node24 *n)
	{
		const FacingDef &d = FACINGS[kind];
		FacingDraw(d, n, Slot(n->e), n->w1C, FacingFade(d, n, n->c));
		if (DrawOnly()) return 0;
		const int16_t rate = n->w1E;
		n->w1C = (int16_t)(n->w1C + rate);
		n->w1E = (int16_t)(rate - (int16_t)(rate / 4));
		n->c++;
		if (n->c < FacingTicks(d, n)) return 0;
		Slot(n->e)->alive = 0;
		return TASK_END;
	}
	static uint32_t __cdecl FacingATask(TaskNode *tn) { return FacingTask(0, (Node24 *)tn); }
	static uint32_t __cdecl FacingBTask(TaskNode *tn) { return FacingTask(1, (Node24 *)tn); }
}

	void register_mag342_renzokuken_ultimecia_griever()
	{
		renzo342::RegisterCommon();
		register_port(renzo342::ORIG_HitEmitter, (void *)renzo342::HitEmitterTask, "342 HitEmitter", 342);
		register_port(renzo342::ORIG_FacingA, (void *)renzo342::FacingATask, "342 FacingA", 342);
		register_port(renzo342::ORIG_FacingB, (void *)renzo342::FacingBTask, "342 FacingB", 342);
	}
}

