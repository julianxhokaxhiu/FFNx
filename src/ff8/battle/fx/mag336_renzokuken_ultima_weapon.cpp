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
namespace renzo336
{
	static constexpr game::Id PTS_A = game::unk_21F15E0, PTS_B = game::unk_21F1620, TARGET_SLOT = game::unk_21F1660;
	static constexpr game::Id POOL_A = game::unk_21F1668, POOL_A_END = game::unk_21F1848;
	static constexpr game::Id POOL_B = game::unk_21F1848, POOL_B_SPAWN_END = game::unk_21F1AC8, POOL_B_END = game::unk_21F1C08;
	static constexpr game::Id HIT_REC = game::unk_21F1C08, MASTER_POOL = game::unk_21F2358, Q_MASTER = game::unk_21F2368;
	static constexpr game::Id SLOTS = game::unk_21F2378, SLOTS_END = game::unk_21F2FF8;
	static constexpr game::Id POOL_C = game::unk_21F2FF8, POOL_C_END = game::unk_21F3318;
	static constexpr game::Id SPL_A = game::unk_21F3318, SPL_B = game::unk_21F3418;
	static constexpr game::Id HIT_POS = game::unk_21F3518, HIT_DIR = game::unk_21F3520, Q_MAIN = game::unk_21F3530, MAIN_POOL = game::unk_21F3540;
	static constexpr game::Id PREV_HIT = game::unk_21F4350, CONTEXT = game::unk_21F4368, CASTER_SLOT = game::unk_21F436C, TEX_FILE = game::unk_21F4370;
	static constexpr game::Id RING = game::unk_21F4378, RING_END = game::unk_21F4404, LENGTH = game::unk_21F4404, FINISHER = game::unk_21F4408;
	static constexpr game::Id TEX_BASE = game::unk_21F440C, PACKET_CURSOR = game::unk_21F4410;
	static const uint32_t PARITY = 0xC000;
	static const void *SOUND() { return (const void *)game_addr(game::unk_E63B38); }
	static constexpr game::Id TEXTURE_NAME = game::aMag333Tim_5, CAMERA = game::MAG336_RENZOKUKEN_VS_ULTIMA_WEAPON_cameraAnim, ANIM_SETS = game::unk_E640EC, BIND_OUT = game::unk_24FEA78;
	static constexpr game::Id MODEL_SPARKLE = game::unk_E63B3C, SPARKLE_FADE = game::unk_E63CC4, SPARKLE_DEPTH = game::unk_E63CB4;
	static constexpr game::Id MODEL_SLASH = game::unk_CAC044, MODEL_STREAK = game::unk_CACA2C;
	static constexpr game::Id SEQ_FLASH = game::unk_CABABC, SEQ_DEBRIS = game::unk_CABD8C, SEQ_SPARK = game::unk_CABC68, SPARK_FRAMES = game::unk_CB2660;
	static constexpr game::Id SLASH_REF = game::unk_CB26C0, HIT_LEAD = game::unk_CB26D0, HIT_OFFS = game::unk_CB2700;
	static constexpr game::Id ORIG_FileLoader = game::MAG_336_UNKNOWN_FL, ORIG_Setup = game::MAG_336_UNKNOWN;
	static constexpr game::Id ORIG_Controller = game::MAG_336_sub_583160, ORIG_HitSpark = game::MAG_336_sub_5835D0, ORIG_Sparkle = game::MAG_336_sub_583860, ORIG_Trail = game::MAG_336_sub_583B90, ORIG_SparkSpawn = game::MAG_336_sub_584110, ORIG_Hit = game::MAG_336_sub_584240, ORIG_Slash = game::MAG_336_sub_584610, ORIG_CasterModelA = game::MAG_336_sub_5848F0, ORIG_BoneEmitter = game::MAG_336_sub_584A30, ORIG_CasterModelB = game::MAG_336_sub_584DE0, ORIG_Dust = game::MAG_336_sub_584F20, ORIG_GroundModel = game::MAG_336_sub_585220, ORIG_Shake = game::MAG_336_sub_585360, ORIG_Master = game::MAG_336_sub_585420;

#include "renzo_boss_defs.inc"
}
}

namespace ff8fx
{
namespace renzo336
{
#define RENZO_ID 336
#define RENZO_IDS "336"
	static void Events(Node24 *n);
#include "renzo_boss.inc"
#undef RENZO_IDS
#undef RENZO_ID

	static const NodeModelDef NODEMODELS[3] = {
		{ ORIG_CasterModelA, game::unk_CAE7BC, NM_CASTER, 6, 512, 5, 0xE, false },
		{ ORIG_CasterModelB, game::unk_CB047C, NM_CASTER, 6, 341, 4, 0x12, false },
		{ ORIG_GroundModel, game::unk_CACCAC, NM_GROUND, 6, 409, 5, 0x10, false },
	};
	static const BoneEmitDef BONE_EMIT = { 0x18, 0x11, 0x25, 0x600, 0x800, 0x6E, 0x28, game::unk_CAB910 };
	static const DustDef DUST = { ORIG_Dust, false, 2, 0xE, 0xE00, 0x1800, 0xC8, 0x1F4, 0x96, 0xDC };
	static const ShakeDef SHAKE_DEF = { false, 8, 100, 8 };

	static void AddCasterModel(uint32_t orig, int32_t s0)
	{
		Node24 *t = (Node24 *)AddTaskToQueue(QMain(), orig);
		ff8fx::copy<(8)>(&t->w10, (const void *)game_addr(HIT_POS));
		t->c = 0;
		const int16_t s = (int16_t)((CrtRand() % 0x800 + s0) / 3);
		t->w1E = s;
		t->w1C = s;
	}

	static void Events(Node24 *n)
	{
		if (n->c == 0x3C) AddCasterModel(game_addr(ORIG_CasterModelA), 0x1600);
		if (n->c == 0x5B) ((Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_BoneEmitter)))->c = 0;
		if (n->c == 0x69) AddCasterModel(game_addr(ORIG_CasterModelB), 0x2000);
		if (n->c == 0x78)
		{
			Node24 *d = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_Dust));
			d->c = 0;
			GetEffectSpawnPosition(Target(), 0xF1, 0, &d->w10);
			Node24 *t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_GroundModel));
			ff8fx::copy<(8)>(&t->w10, &d->w10);
			t->c = 0;
			const int16_t s = (int16_t)((CrtRand() % 0xA00 + 0x1E00) / 3);
			t->w1E = s;
			t->w1C = s;
			((Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_Shake)))->c = 0;
		}
	}

	static uint32_t __cdecl CasterModelATask(TaskNode *tn) { return NodeModelTask(NODEMODELS[0], 0, (Node24 *)tn); }
	static uint32_t __cdecl BoneEmitterTask(TaskNode *tn) { return BoneEmitter(BONE_EMIT, (Node24 *)tn); }
	static uint32_t __cdecl CasterModelBTask(TaskNode *tn) { return NodeModelTask(NODEMODELS[1], 1, (Node24 *)tn); }
	static uint32_t __cdecl DustTask(TaskNode *tn) { return DustRun(DUST, (Node24 *)tn); }
	static uint32_t __cdecl GroundModelTask(TaskNode *tn) { return NodeModelTask(NODEMODELS[2], 2, (Node24 *)tn); }
	static uint32_t __cdecl ShakeTask(TaskNode *tn) { return ShakeRun(SHAKE_DEF, (Node24 *)tn); }
}

	void register_mag336_renzokuken_ultima_weapon()
	{
		renzo336::RegisterCommon();
		register_port(renzo336::ORIG_CasterModelA, (void *)renzo336::CasterModelATask, "336 CasterModelA", 336);
		register_port(renzo336::ORIG_BoneEmitter, (void *)renzo336::BoneEmitterTask, "336 BoneEmitter", 336);
		register_port(renzo336::ORIG_CasterModelB, (void *)renzo336::CasterModelBTask, "336 CasterModelB", 336);
		register_port(renzo336::ORIG_Dust, (void *)renzo336::DustTask, "336 Dust", 336);
		register_port(renzo336::ORIG_GroundModel, (void *)renzo336::GroundModelTask, "336 GroundModel", 336);
		register_port(renzo336::ORIG_Shake, (void *)renzo336::ShakeTask, "336 Shake", 336);
	}
}

