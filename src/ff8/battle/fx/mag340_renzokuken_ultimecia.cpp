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
namespace renzo340
{
	static constexpr game::Id PTS_A = game::unk_21E8A78, PTS_B = game::unk_21E8AB8, TARGET_SLOT = game::unk_21E8AF8;
	static constexpr game::Id POOL_A = game::unk_21E8B00, POOL_A_END = game::unk_21E8CE0;
	static constexpr game::Id POOL_B = game::unk_21E8CE0, POOL_B_SPAWN_END = game::unk_21E8F60, POOL_B_END = game::unk_21E90A0;
	static constexpr game::Id HIT_REC = game::unk_21E90A0, MASTER_POOL = game::unk_21E97F0, Q_MASTER = game::unk_21E9800;
	static constexpr game::Id SLOTS = game::unk_21E9810, SLOTS_END = game::unk_21EA490;
	static constexpr game::Id POOL_C = game::unk_21EA490, POOL_C_END = game::unk_21EA7B0;
	static constexpr game::Id SPL_A = game::unk_21EA7B0, SPL_B = game::unk_21EA8B0;
	static constexpr game::Id HIT_POS = game::unk_21EA9B0, HIT_DIR = game::unk_21EA9B8, Q_MAIN = game::unk_21EA9C8, MAIN_POOL = game::unk_21EA9D8;
	static constexpr game::Id PREV_HIT = game::unk_21EB7E8, CONTEXT = game::unk_21EB800, CASTER_SLOT = game::unk_21EB804, TEX_FILE = game::unk_21EB808;
	static constexpr game::Id RING = game::unk_21EB810, RING_END = game::unk_21EB89C, LENGTH = game::unk_21EB89C, FINISHER = game::unk_21EB8A0;
	static constexpr game::Id TEX_BASE = game::unk_21EB8A4, PACKET_CURSOR = game::unk_21EB8A8;
	static const uint32_t PARITY = 0xC000;
	static const void *SOUND() { return (const void *)game_addr(game::unk_E50B78); }
	static constexpr game::Id TEXTURE_NAME = game::aMag333Tim_2, CAMERA = game::MAG340_RENZOKUKEN_VS_ULTIMECIA_FINAL_FORM_cameraAnim, ANIM_SETS = game::unk_E510FC, BIND_OUT = game::unk_24FE9D8;
	static constexpr game::Id MODEL_SPARKLE = game::unk_E50B7C, SPARKLE_FADE = game::unk_E50D04, SPARKLE_DEPTH = game::unk_E50CF4;
	static constexpr game::Id MODEL_SLASH = game::unk_C975B4, MODEL_STREAK = game::unk_C97F9C;
	static constexpr game::Id SEQ_FLASH = game::unk_C9702C, SEQ_DEBRIS = game::unk_C972FC, SEQ_SPARK = game::unk_C971D8, SPARK_FRAMES = game::unk_C9DBD0;
	static constexpr game::Id SLASH_REF = game::unk_C9DC30, HIT_LEAD = game::unk_C9DC40, HIT_OFFS = game::unk_C9DC70;
	static constexpr game::Id SEQ_EMIT = game::unk_C96E80;
	static constexpr game::Id ORIG_FileLoader = game::MAG_340_UNKNOWN_FL, ORIG_Setup = game::MAG_340_UNKNOWN;
	static constexpr game::Id ORIG_Controller = game::MAG_340_sub_57BD20, ORIG_HitSpark = game::MAG_340_sub_57C390, ORIG_Sparkle = game::MAG_340_sub_57C620, ORIG_Trail = game::MAG_340_sub_57C950, ORIG_SparkSpawn = game::MAG_340_sub_57CED0, ORIG_Hit = game::MAG_340_sub_57D000, ORIG_Slash = game::MAG_340_sub_57D3D0, ORIG_RingA = game::MAG_340_sub_57D6B0, ORIG_CasterModel = game::MAG_340_sub_57D840, ORIG_RingB = game::MAG_340_sub_57D980, ORIG_AngleModel = game::MAG_340_sub_57DB10, ORIG_BoneEmitter = game::MAG_340_sub_57DC60, ORIG_NodeEmitter = game::MAG_340_sub_57DF60, ORIG_Master = game::MAG_340_sub_57E340;

#include "renzo_boss_defs.inc"
}
}

namespace ff8fx
{
namespace renzo340
{
#define RENZO_ID 340
#define RENZO_IDS "340"
#define RENZO_CUSTOM_START
	static void Events(Node24 *n);
	static void Start(Node24 *n);
#include "renzo_boss.inc"
#undef RENZO_CUSTOM_START
#undef RENZO_IDS
#undef RENZO_ID

	static const SlotModelDef SLOTMODELS[2] = {
		{ ORIG_RingA, game::unk_C99D2C, game::unk_C9DC30, 3, 6, 409, 0x10, false },
		{ ORIG_RingB, game::unk_C98C04, game::unk_C9DCD0, 3, 6, 409, 0x10, false },
	};
	static const NodeModelDef NODEMODELS[2] = {
		{ ORIG_CasterModel, game::unk_C9B9EC, NM_CASTER, 6, 409, 4, 0x10, false, false },
		{ ORIG_AngleModel, game::unk_C9CF24, NM_YANGLE, 8, 409, 4, 0x12, false, true },
	};
	static const BoneEmitDef BONE_EMIT = { 0xE, 0x1D, 0, 0x880, 0x700, 0x64, 0x19, SEQ_EMIT };

	static void StartBoss(uint8_t *caster, uint8_t *boss, uint32_t out, uint32_t sets, int32_t seq)
	{
		(void)caster; (void)out; (void)sets; (void)seq;
		QueueChainTransformation(boss, 1);
	}

	static void Bind2(uint8_t *caster, uint8_t *boss, uint32_t out, uint32_t sets, int32_t seq)
	{
		const uint32_t next = BindEntityModelFromComData(caster, out, sets + *(const uint32_t *)(sets + 4));
		BindEntityModelFromComData(boss, next, sets + *(const uint32_t *)(sets + 8));
		QueueChainTransformation(caster, seq);
	}

	static void Start(Node24 *n)
	{
		if (n->c == 1) StartBoss(Caster(), Target(), game_addr(BIND_OUT), game_addr(ANIM_SETS), (Finisher() != 0) + 1);
		if (n->c == 2) Bind2(Caster(), Target(), game_addr(BIND_OUT), game_addr(ANIM_SETS), (Finisher() != 0) + 1);
		if (n->c == 3) ComputeLength();
		if (n->c == 2) StartTasks();
	}

	static void Events(Node24 *n)
	{
		if (n->c == 0x1C)
		{
			Node24 *t = AddSlotTask(game_addr(ORIG_RingA));
			const int16_t s = (int16_t)((CrtRand() % 0x900 + 0x1400) / 3);
			t->w1E = s;
			t->w1C = s;
			HitPointRecord(t);
		}
		if (n->c == 0x23)
		{
			Node24 *t = AddSlotTask(game_addr(ORIG_RingA));
			t->w10 = 0x258;
			t->w12 = 0x64;
			t->w14 = -0x190;
			MatVec((const Mat4x3 *)(Caster() + 0x40), &t->w10, &t->w10);
			const int16_t s = (int16_t)((CrtRand() % 0x900 + 0x1400) / 3);
			t->w1E = s;
			t->w1C = s;
			HitPointRecord(t);
			SlotRec *r = Slot(t->e);
			r->pos[0] = (int16_t)(r->pos[0] + t->w10);
			r->pos[1] = (int16_t)(r->pos[1] + t->w12);
			r->pos[2] = (int16_t)(r->pos[2] + t->w14);
		}
		if (n->c == 0x3B)
		{
			Node24 *t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_CasterModel));
			t->c = 0;
			GetEffectSpawnPosition(Target(), 0x1D, 0, &t->w10);
			const int16_t s = (int16_t)((CrtRand() % 0x800 + 0x2000) / 3);
			t->w1E = s;
			t->w1C = s;
		}
		if (n->c == 0x43)
		{
			Node24 *t = AddSlotTask(game_addr(ORIG_RingB));
			const int16_t s = (int16_t)((CrtRand() % 0x500 + 0xD00) / 3);
			t->w1E = s;
			t->w1C = s;
			HitPointRecord(t);
		}
		if (n->c == 0x44)
		{
			Node24 *t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_AngleModel));
			t->c = 0;
			t->w10 = var<int16_t>(game_addr(HIT_POS));
			t->w12 = 0x226;
			t->w14 = var<int16_t>(game_addr(HIT_POS) + 4);
			t->w18 = (int16_t)(CrtRand() % 0x1000);
			t->w1C = 0x3200;
			const int16_t sy = (int16_t)((CrtRand() % 0xC00 + 0x1400) / 3);
			t->w22 = sy;
			t->w20 = sy;
		}
		if (n->c == 0x3B) ((Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_BoneEmitter)))->c = 0;
		if (n->c == 0x43)
		{
			Node24 *t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_NodeEmitter));
			ff8fx::copy<(8)>(&t->w10, (const void *)game_addr(HIT_POS));
			t->w12 = (int16_t)(t->w12 - 0x12C);
			t->c = 0;
		}
	}

	static uint32_t __cdecl NodeEmitterTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		EmitWork *w = (EmitWork *)FieldAlloc(0x50);
		if (!DrawOnly() && n->c <= 0x10)
		{
			w->dir[0] = CrtRand() % 0x200 - 0x100;
			w->dir[1] = -0x900 - CrtRand() % 0x500;
			w->dir[2] = CrtRand() % 0x600 + 0x400;
			Normalize(w->dir, w->dir);
			TransformVectorBy3x3Matrix((const Mat4x3 *)(Caster() + 0x40), w->dir, w->dir);
			ff8fx::copy<(8)>(w->pos, &n->w10);
			for (int32_t j = 0; j < 3; j++)
			{
				const int32_t i = FreeRecord();
				if (i < 0) break;
				SlotRec *p = Slot(i);
				p->alive = 4;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x880 + 0x700);
				ff8fx::copy<(8)>(p->pos, w->pos);
				const int32_t sp = CrtRand() % 0x8C + 0x28;
				const int32_t rx = CrtRand() % 0x28;
				p->w10 = (int16_t)(rx + (mul32(w->dir[0], sp) >> 12) - 0x14);
				p->w12 = (int16_t)(mul32(w->dir[1], sp) >> 12);
				p->w14 = (int16_t)(mul32(w->dir[2], sp) >> 12);
			}
		}
		const int32_t alive = EmitterDraw(n, w, 4, game_addr(SEQ_EMIT));
		FieldFree(0x50);
		if (DrawOnly()) return 0;
		n->c++;
		return alive ? 0 : TASK_END;
	}

	static uint32_t __cdecl RingATask(TaskNode *tn) { return SlotModelTask(SLOTMODELS[0], 0, (Node24 *)tn); }
	static uint32_t __cdecl RingBTask(TaskNode *tn) { return SlotModelTask(SLOTMODELS[1], 1, (Node24 *)tn); }
	static uint32_t __cdecl CasterModelTask(TaskNode *tn) { return NodeModelTask(NODEMODELS[0], 0, (Node24 *)tn); }
	static uint32_t __cdecl AngleModelTask(TaskNode *tn) { return NodeModelTask(NODEMODELS[1], 1, (Node24 *)tn); }
	static uint32_t __cdecl BoneEmitterTask(TaskNode *tn) { return BoneEmitter(BONE_EMIT, (Node24 *)tn); }
}

	void register_mag340_renzokuken_ultimecia()
	{
		renzo340::RegisterCommon();
		register_port(renzo340::ORIG_RingA, (void *)renzo340::RingATask, "340 RingA", 340);
		register_port(renzo340::ORIG_CasterModel, (void *)renzo340::CasterModelTask, "340 CasterModel", 340);
		register_port(renzo340::ORIG_RingB, (void *)renzo340::RingBTask, "340 RingB", 340);
		register_port(renzo340::ORIG_AngleModel, (void *)renzo340::AngleModelTask, "340 AngleModel", 340);
		register_port(renzo340::ORIG_BoneEmitter, (void *)renzo340::BoneEmitterTask, "340 BoneEmitter", 340);
		register_port(renzo340::ORIG_NodeEmitter, (void *)renzo340::NodeEmitterTask, "340 NodeEmitter", 340);
	}
}

