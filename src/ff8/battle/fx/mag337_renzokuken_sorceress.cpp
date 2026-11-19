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
namespace renzo337
{
	static constexpr game::Id PTS_A = game::unk_21EE748, PTS_B = game::unk_21EE788, TARGET_SLOT = game::unk_21EE7C8;
	static constexpr game::Id POOL_A = game::unk_21EE7D0, POOL_A_END = game::unk_21EE9B0;
	static constexpr game::Id POOL_B = game::unk_21EE9B0, POOL_B_SPAWN_END = game::unk_21EEC30, POOL_B_END = game::unk_21EED70;
	static constexpr game::Id HIT_REC = game::unk_21EED70, MASTER_POOL = game::unk_21EF4C0, Q_MASTER = game::unk_21EF4D0;
	static constexpr game::Id SLOTS = game::unk_21EF4E0, SLOTS_END = game::unk_21F0160;
	static constexpr game::Id POOL_C = game::unk_21F0160, POOL_C_END = game::unk_21F0480;
	static constexpr game::Id SPL_A = game::unk_21F0480, SPL_B = game::unk_21F0580;
	static constexpr game::Id HIT_POS = game::unk_21F0680, HIT_DIR = game::unk_21F0688, Q_MAIN = game::unk_21F0698, MAIN_POOL = game::unk_21F06A8;
	static constexpr game::Id PREV_HIT = game::unk_21F14B8, CONTEXT = game::unk_21F14D0, CASTER_SLOT = game::unk_21F14D4, TEX_FILE = game::unk_21F14D8;
	static constexpr game::Id RING = game::unk_21F14E0, RING_END = game::unk_21F156C, LENGTH = game::unk_21F156C, FINISHER = game::unk_21F1570;
	static constexpr game::Id TEX_BASE = game::unk_21F1574, PACKET_CURSOR = game::unk_21F1578, HIT_OFFS = game::unk_21F1580;
	static const uint32_t PARITY = 0xC000;
	static const void *SOUND() { return (const void *)game_addr(game::unk_E5D80C); }
	static constexpr game::Id TEXTURE_NAME = game::aMag333Tim_4, CAMERA = game::MAG337_RENZOKUKEN_VS_FINAL_SORCERESS_cameraAnim, ANIM_SETS = game::unk_E5DF6C, BIND_OUT = game::unk_24FEA48;
	static constexpr game::Id MODEL_SPARKLE = game::unk_E5D810, SPARKLE_FADE = game::unk_E5D998, SPARKLE_DEPTH = game::unk_E5D988;
	static constexpr game::Id MODEL_SLASH = game::unk_CA5234, MODEL_STREAK = game::unk_CA5C1C;
	static constexpr game::Id SEQ_FLASH = game::unk_CA4CAC, SEQ_DEBRIS = game::unk_CA4F7C, SEQ_SPARK = game::unk_CA4E58, SPARK_FRAMES = game::unk_CAB850;
	static constexpr game::Id SLASH_REF = game::unk_CAB8B0, HIT_LEAD = game::unk_CAB8C0;
	static constexpr game::Id RING_REF = game::unk_CAB8F0;
	static constexpr game::Id ORIG_FileLoader = game::MAG_337_UNKNOWN_FL, ORIG_Setup = game::MAG_337_UNKNOWN;
	static constexpr game::Id ORIG_Controller = game::MAG_337_sub_580AB0, ORIG_HitSpark = game::MAG_337_sub_580F50, ORIG_Sparkle = game::MAG_337_sub_5811E0, ORIG_Trail = game::MAG_337_sub_581510, ORIG_SparkSpawn = game::MAG_337_sub_581A90, ORIG_Hit = game::MAG_337_sub_581BC0, ORIG_Slash = game::MAG_337_sub_581F90, ORIG_CasterModel = game::MAG_337_sub_582270, ORIG_BoneEmitter = game::MAG_337_sub_5823B0, ORIG_Ring = game::MAG_337_sub_5826C0, ORIG_Dust = game::MAG_337_sub_582850, ORIG_GroundModel = game::MAG_337_sub_582B50, ORIG_Shake = game::MAG_337_sub_582C90, ORIG_Master = game::MAG_337_sub_582DB0;

#include "renzo_boss_defs.inc"
}
}

namespace ff8fx
{
namespace renzo337
{
#define RENZO_ID 337
#define RENZO_IDS "337"
	static void Events(Node24 *n);
#include "renzo_boss.inc"
#undef RENZO_IDS
#undef RENZO_ID

	static const SlotModelDef SLOTMODELS[1] = {
		{ ORIG_Ring, game::unk_CA6884, RING_REF, 3, 6, 341, 0x12, false },
	};
	static const NodeModelDef NODEMODELS[2] = {
		{ ORIG_CasterModel, game::unk_CA79AC, NM_CASTER, 6, 512, 5, 0xE, false },
		{ ORIG_GroundModel, game::unk_CA5E9C, NM_GROUND, 6, 409, 5, 0x10, false },
	};
	static const BoneEmitDef BONE_EMIT = { 0x16, 0xF1, 0, 0x880, 0x700, 0x64, 0x19, game::unk_CA4B00 };
	static const DustDef DUST = { ORIG_Dust, false, 2, 0xE, 0x800, 0x1200, 0xFA, 0x190, 0xA0, 0xBE };
	static const ShakeDef SHAKE_DEF = { false, 8, 100, 8 };

	static void Events(Node24 *n)
	{
		if (n->c == 0x46)
		{
			Node24 *t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_CasterModel));
			ff8fx::copy<(8)>(&t->w10, (const void *)game_addr(HIT_POS));
			t->c = 0;
			const int16_t s = (int16_t)((CrtRand() % 0x600 + 0x1600) / 3);
			t->w1E = s;
			t->w1C = s;
		}
		if (n->c == 0x47) ((Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_BoneEmitter)))->c = 0;
		if (n->c == 0x54)
		{
			Node24 *t = AddSlotTask(game_addr(ORIG_Ring));
			const int16_t s = (int16_t)((CrtRand() % 0x400 + 0xC00) / 3);
			t->w1E = s;
			t->w1C = s;
			HitPointRecord(t);
		}
		if (n->c == 0x59)
		{
			Node24 *d = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_Dust));
			d->c = 0;
			GetEffectSpawnPosition(Target(), 0xF1, 0, &d->w10);
			Node24 *t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_GroundModel));
			ff8fx::copy<(8)>(&t->w10, &d->w10);
			t->c = 0;
			const int16_t s = (int16_t)((CrtRand() % 0x800 + 0x1800) / 3);
			t->w1E = s;
			t->w1C = s;
			((Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_Shake)))->c = 0;
		}
	}

	static uint32_t __cdecl CasterModelTask(TaskNode *tn) { return NodeModelTask(NODEMODELS[0], 0, (Node24 *)tn); }
	static uint32_t __cdecl BoneEmitterTask(TaskNode *tn) { return BoneEmitter(BONE_EMIT, (Node24 *)tn); }
	static uint32_t __cdecl RingTask(TaskNode *tn) { return SlotModelTask(SLOTMODELS[0], 0, (Node24 *)tn); }
	static uint32_t __cdecl DustTask(TaskNode *tn) { return DustRun(DUST, (Node24 *)tn); }
	static uint32_t __cdecl GroundModelTask(TaskNode *tn) { return NodeModelTask(NODEMODELS[1], 1, (Node24 *)tn); }
	static uint32_t __cdecl ShakeTask(TaskNode *tn) { return ShakeRun(SHAKE_DEF, (Node24 *)tn); }
}

	void register_mag337_renzokuken_sorceress()
	{
		renzo337::RegisterCommon();
		register_port(renzo337::ORIG_CasterModel, (void *)renzo337::CasterModelTask, "337 CasterModel", 337);
		register_port(renzo337::ORIG_BoneEmitter, (void *)renzo337::BoneEmitterTask, "337 BoneEmitter", 337);
		register_port(renzo337::ORIG_Ring, (void *)renzo337::RingTask, "337 Ring", 337);
		register_port(renzo337::ORIG_Dust, (void *)renzo337::DustTask, "337 Dust", 337);
		register_port(renzo337::ORIG_GroundModel, (void *)renzo337::GroundModelTask, "337 GroundModel", 337);
		register_port(renzo337::ORIG_Shake, (void *)renzo337::ShakeTask, "337 Shake", 337);
	}
}

