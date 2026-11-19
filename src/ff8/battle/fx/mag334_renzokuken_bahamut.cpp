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
namespace renzo334
{
	static constexpr game::Id PTS_A = game::unk_21F7250, PTS_B = game::unk_21F7290, TARGET_SLOT = game::unk_21F72D0;
	static constexpr game::Id POOL_A = game::unk_21F72D8, POOL_A_END = game::unk_21F74B8;
	static constexpr game::Id POOL_B = game::unk_21F74B8, POOL_B_SPAWN_END = game::unk_21F7738, POOL_B_END = game::unk_21F7878;
	static constexpr game::Id HIT_REC = game::unk_21F7878, MASTER_POOL = game::unk_21F7FC8, Q_MASTER = game::unk_21F7FD8;
	static constexpr game::Id SLOTS = game::unk_21F7FE8, SLOTS_END = game::unk_21F8C68;
	static constexpr game::Id POOL_C = game::unk_21F8C68, POOL_C_END = game::unk_21F8F88;
	static constexpr game::Id SPL_A = game::unk_21F8F88, SPL_B = game::unk_21F9088;
	static constexpr game::Id HIT_POS = game::unk_21F9188, HIT_DIR = game::unk_21F9190, Q_MAIN = game::unk_21F91A0, MAIN_POOL = game::unk_21F91B0;
	static constexpr game::Id PREV_HIT = game::unk_21F9FC0, CONTEXT = game::unk_21F9FD8, CASTER_SLOT = game::unk_21F9FDC, TEX_FILE = game::unk_21F9FE0;
	static constexpr game::Id RING = game::unk_21F9FE8, RING_END = game::unk_21FA074, LENGTH = game::unk_21FA074, FINISHER = game::unk_21FA078;
	static constexpr game::Id TEX_BASE = game::unk_21FA07C, PACKET_CURSOR = game::unk_21FA080, HIT_OFFS = game::unk_21FA088;
	static const uint32_t PARITY = 0x8000;
	static const void *SOUND() { return (const void *)game_addr(game::unk_E70D58); }
	static constexpr game::Id TEXTURE_NAME = game::aMag333Tim_7, CAMERA = game::MAG334_RENZOKUKEN_VS_BAHAMUT_cameraAnim, ANIM_SETS = game::unk_E71854, BIND_OUT = game::unk_24FEAD8;
	static constexpr game::Id MODEL_SPARKLE = game::unk_E70D5C, SPARKLE_FADE = game::unk_E70EE4, SPARKLE_DEPTH = game::unk_E70ED4;
	static constexpr game::Id MODEL_SLASH = game::unk_CB9D24, MODEL_STREAK = game::unk_CBA70C;
	static constexpr game::Id SEQ_FLASH = game::unk_CB979C, SEQ_DEBRIS = game::unk_CB9A6C, SEQ_SPARK = game::unk_CB9948, SPARK_FRAMES = game::unk_CC0340;
	static constexpr game::Id SLASH_REF = game::unk_CC03A0, HIT_LEAD = game::unk_CC03B0;
	static constexpr game::Id MODEL_POOL_RING = game::unk_CBD48C, POOL_RING_REF = game::unk_CC03E0;
	static constexpr game::Id ORIG_FileLoader = game::MAG_334_UNKNOWN_FL, ORIG_Setup = game::MAG_334_UNKNOWN;
	static constexpr game::Id ORIG_Controller = game::MAG_334_sub_5878B0, ORIG_HitSpark = game::MAG_334_sub_587CA0, ORIG_Sparkle = game::MAG_334_sub_587F30, ORIG_Trail = game::MAG_334_sub_588260, ORIG_SparkSpawn = game::MAG_334_sub_5887E0, ORIG_Hit = game::MAG_334_sub_588910, ORIG_Slash = game::MAG_334_sub_588CE0, ORIG_RingPool = game::MAG_334_sub_588FC0, ORIG_AngleModel = game::MAG_334_sub_5892B0, ORIG_BoneEmitter = game::MAG_334_sub_589400, ORIG_CasterModel = game::MAG_334_sub_589710, ORIG_Master = game::MAG_334_sub_589940;

#include "renzo_boss_defs.inc"
}
}

namespace ff8fx
{
namespace renzo334
{
#define RENZO_ID 334
#define RENZO_IDS "334"
	static void Events(Node24 *n);
#include "renzo_boss.inc"
#undef RENZO_IDS
#undef RENZO_ID

	static const NodeModelDef NODEMODELS[2] = {
		{ ORIG_AngleModel, game::unk_CBB374, NM_YANGLE, 6, 512, 4, 0xE, true },
		{ ORIG_CasterModel, game::unk_CBE15C, NM_CASTER_OFS, 4, 341, 4, 0x10, false },
	};
	static const BoneEmitDef BONE_EMIT = { 0x20, 0xF0, 0, 0x600, 0x800, 0x5A, 0x1E, game::unk_CB95F0 };

	static void Events(Node24 *n)
	{
		if (n->c == 0x39) ((Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_RingPool)))->c = 0;
		if (n->c == 0x65)
		{
			Node24 *t = AddSlotTask(game_addr(ORIG_AngleModel));
			ff8fx::copy<(8)>(&t->w10, (const void *)game_addr(HIT_POS));
			t->w18 = (int16_t)(CrtRand() % 0x1000);
			const int16_t s = (int16_t)((CrtRand() % 0x300 + 0xA00) / 3);
			t->w1E = s;
			t->w1C = s;
			((Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_BoneEmitter)))->c = 0;
		}
		if (n->c == 0x8E)
		{
			Node24 *t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_CasterModel));
			ff8fx::copy<(8)>(&t->w10, (const void *)game_addr(HIT_POS));
			t->c = 0;
			const int16_t s = (int16_t)((CrtRand() % 0x800 + 0x2400) / 3);
			t->w1E = s;
			t->w1C = s;
		}
	}

#pragma pack(push, 1)
	struct PoolWork
	{
		uint8_t pad00[8];
		Mat4x3 cam;
		Mat4x3 m;
		int32_t v[4];
		int32_t axis[4];
	};
#pragma pack(pop)
	static_assert(sizeof(PoolWork) == 0x68, "Renzokuken 334 ring pool block");

	static void PoolRing(SlotRec *r, const int16_t *p, int16_t dx, int16_t dz)
	{
		r->alive = 1;
		r->life = 0;
		r->pos[0] = p[0];
		r->pos[1] = p[1];
		r->pos[2] = p[2];
		r->pos[3] = p[3];
		r->pos[1] = (int16_t)(r->pos[1] + 600);
		r->w16 = 0x800;
		r->w12 = 0x800;
		r->w18 = dx;
		r->w1A = 0;
		r->w1C = dz;
		NormalizeS16(&r->w18, &r->w18);
	}

	static void PoolRingDraw(PoolWork *w, uint8_t *h, const SlotRec *r, int32_t scale, int32_t fade)
	{
		w->v[1] = 0;
		w->v[0] = r->w18;
		w->v[2] = r->w1C;
		const int32_t angle = RotationBetweenVectors((const int32_t *)game_addr(POOL_RING_REF), w->v, w->axis);
		BuildAxisAngleRotationMatrix(angle, &w->m, w->axis);
		w->m.t[0] = r->pos[0];
		w->m.t[1] = r->pos[1];
		w->v[1] = scale;
		w->m.t[2] = r->pos[2];
		Scale3DMatrix(&w->m, w->v);
		ComposeAffineTransform(&w->cam, &w->m, &w->m);
		GteSetRotMatrix(&w->m);
		GteSetTransVector(&w->m);
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (r->life >= 4)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
	}
	static int32_t PoolRingFade(int32_t life) { return shl32(life - 4, 9); }

	static uint32_t __cdecl RingPoolTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		PoolWork *w = (PoolWork *)FieldAlloc(0x68);
		if (!DrawOnly() && n->c <= 8)
		{
			const int32_t i = FreeRecord();
			if (i >= 0)
			{
				PoolRing(Slot(i), (const int16_t *)game_addr(HIT_POS), var<int16_t>(game_addr(HIT_DIR)), var<int16_t>(game_addr(HIT_DIR) + 8));
				int16_t mid[4];
				mid[0] = (int16_t)(((int32_t)var<int16_t>(game_addr(PREV_HIT)) + var<int16_t>(game_addr(HIT_POS))) >> 1);
				mid[1] = (int16_t)(((int32_t)var<int16_t>(game_addr(PREV_HIT) + 2) + var<int16_t>(game_addr(HIT_POS) + 2)) >> 1);
				mid[2] = (int16_t)(((int32_t)var<int16_t>(game_addr(PREV_HIT) + 4) + var<int16_t>(game_addr(HIT_POS) + 4)) >> 1);
				SlotRec *r = Slot(i + 1);
				mid[3] = r->pos[3];
				PoolRing(r, mid, var<int16_t>(game_addr(PREV_HIT) + 8), var<int16_t>(game_addr(PREV_HIT) + 0x10));
			}
		}
		uint8_t *h = AllocHeader(0x58);
		ff8fx::copy<(sizeof(Mat4x3))>(&w->cam, &Camera());
		*(uint32_t *)h = game_addr(MODEL_POOL_RING);
		*(uint32_t *)(h + 8) = 0;
		int32_t alive = 0;
		w->v[0] = 0;
		w->v[2] = 0x300;
		for (uint32_t a = game_addr(SLOTS); a < game_addr(SLOTS_END); a += 0x20)
		{
			SlotRec *r = (SlotRec *)a;
			if (!(r->alive & 1)) continue;
			PoolRingDraw(w, h, r, r->w12, PoolRingFade(r->life));
			if (DrawOnly()) continue;
			if (r->life >= 0xC)
			{
				r->alive = 0;
				continue;
			}
			r->life++;
			r->w12 = (int16_t)(r->w12 + r->w16);
			r->w16 = (int16_t)(r->w16 - (int16_t)(r->w16 / 4));
			alive++;
		}
		FieldFree(0x58);
		FieldFree(0x68);
		if (DrawOnly()) return 0;
		n->c++;
		return alive ? 0 : TASK_END;
	}

	static uint32_t __cdecl AngleModelTask(TaskNode *tn) { return NodeModelTask(NODEMODELS[0], 0, (Node24 *)tn); }
	static uint32_t __cdecl BoneEmitterTask(TaskNode *tn) { return BoneEmitter(BONE_EMIT, (Node24 *)tn); }
	static uint32_t __cdecl CasterModelTask(TaskNode *tn) { return NodeModelTask(NODEMODELS[1], 1, (Node24 *)tn); }
}

	void register_mag334_renzokuken_bahamut()
	{
		renzo334::RegisterCommon();
		register_port(renzo334::ORIG_RingPool, (void *)renzo334::RingPoolTask, "334 RingPool", 334);
		register_port(renzo334::ORIG_AngleModel, (void *)renzo334::AngleModelTask, "334 AngleModel", 334);
		register_port(renzo334::ORIG_BoneEmitter, (void *)renzo334::BoneEmitterTask, "334 BoneEmitter", 334);
		register_port(renzo334::ORIG_CasterModel, (void *)renzo334::CasterModelTask, "334 CasterModel", 334);
	}
}

