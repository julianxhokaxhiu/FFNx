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

// Effect 339: Renzokuken vs Adel

#include "mag_common.h"

namespace ff8fx
{
namespace renzo339
{
	static constexpr game::Id PTS_A = game::unk_21EB8B0, PTS_B = game::unk_21EB8F0, TARGET_SLOT = game::unk_21EB930;
	static constexpr game::Id POOL_A = game::unk_21EB938, POOL_A_END = game::unk_21EBB18;
	static constexpr game::Id POOL_B = game::unk_21EBB18, POOL_B_SPAWN_END = game::unk_21EBD98, POOL_B_END = game::unk_21EBED8;
	static constexpr game::Id HIT_REC = game::unk_21EBED8, MASTER_POOL = game::unk_21EC628, Q_MASTER = game::unk_21EC638;
	static constexpr game::Id SLOTS = game::unk_21EC648, SLOTS_END = game::unk_21ED2C8;
	static constexpr game::Id POOL_C = game::unk_21ED2C8, POOL_C_END = game::unk_21ED5E8;
	static constexpr game::Id SPL_A = game::unk_21ED5E8, SPL_B = game::unk_21ED6E8;
	static constexpr game::Id HIT_POS = game::unk_21ED7E8, HIT_DIR = game::unk_21ED7F0, Q_MAIN = game::unk_21ED800, MAIN_POOL = game::unk_21ED810;
	static constexpr game::Id PREV_HIT = game::unk_21EE620, CONTEXT = game::unk_21EE638, CASTER_SLOT = game::unk_21EE63C, TEX_FILE = game::unk_21EE640;
	static constexpr game::Id RING = game::unk_21EE648, RING_END = game::unk_21EE6D4, LENGTH = game::unk_21EE6D4, FINISHER = game::unk_21EE6D8;
	static constexpr game::Id TEX_BASE = game::unk_21EE6DC, PACKET_CURSOR = game::unk_21EE6E0, HIT_OFFS = game::unk_21EE6E8;
	static const uint32_t PARITY = 0xC000;
	static const void *SOUND() { return (const void *)game_addr(game::unk_E55D74); }
	static constexpr game::Id TEXTURE_NAME = game::aMag333Tim_3, CAMERA = game::MAG339_RENZOKUKEN_VS_ADEL_cameraAnim, ANIM_SETS = game::unk_E56798, BIND_OUT = game::unk_24FEA08;
	static constexpr game::Id MODEL_SPARKLE = game::unk_E55D78, SPARKLE_FADE = game::unk_E55F00, SPARKLE_DEPTH = game::unk_E55EF0;
	static constexpr game::Id MODEL_SLASH = game::unk_C9E424, MODEL_STREAK = game::unk_C9EE0C;
	static constexpr game::Id SEQ_FLASH = game::unk_C9DE9C, SEQ_DEBRIS = game::unk_C9E16C, SEQ_SPARK = game::unk_C9E048, SPARK_FRAMES = game::unk_CA4A40;
	static constexpr game::Id SLASH_REF = game::unk_CA4AA0, HIT_LEAD = game::unk_CA4AB0;
	static constexpr game::Id SEQ_EMIT = game::unk_C9DCF0, RING_REF = game::unk_CA4AE0;
	static constexpr game::Id ORIG_FileLoader = game::MAG_339_UNKNOWN_FL, ORIG_Setup = game::MAG_339_UNKNOWN;
	static constexpr game::Id ORIG_Controller = game::MAG_339_sub_57E6F0, ORIG_HitSpark = game::MAG_339_sub_57EC70, ORIG_Sparkle = game::MAG_339_sub_57EF00, ORIG_Trail = game::MAG_339_sub_57F230, ORIG_SparkSpawn = game::MAG_339_sub_57F7B0, ORIG_Hit = game::MAG_339_sub_57F8E0, ORIG_Slash = game::MAG_339_sub_57FCB0, ORIG_Emitter = game::MAG_339_sub_57FF90, ORIG_RingA = game::MAG_339_sub_580310, ORIG_RingB = game::MAG_339_sub_5804A0, ORIG_Master = game::MAG_339_sub_580700;

#include "renzo_boss_defs.inc"
}
}

namespace ff8fx
{
namespace renzo339
{
#define RENZO_ID 339
#define RENZO_IDS "339"
#define RENZO_CUSTOM_START
	static void Events(Node24 *n);
	static void Start(Node24 *n);
#include "renzo_boss.inc"
#undef RENZO_CUSTOM_START
#undef RENZO_IDS
#undef RENZO_ID

	static const SlotModelDef SLOTMODELS[2] = {
		{ ORIG_RingA, game::unk_C9FA74, RING_REF, 3, 6, 409, 0x10, false },
		{ ORIG_RingB, game::unk_CA1C2C, RING_REF, 2, 6, 409, 0x10, true },
	};

	static void StartBoss(uint8_t *caster, uint8_t *boss, uint32_t out, uint32_t sets, int32_t seq)
	{
		(void)caster; (void)out; (void)sets; (void)seq;
		QueueChainTransformation(boss, 1);
	}

	static void Bind3(uint8_t *caster, uint8_t *boss, uint32_t out, uint32_t sets, int32_t seq)
	{
		uint32_t next = BindEntityModelFromComData(caster, out, sets + *(const uint32_t *)(sets + 4));
		next = BindEntityModelFromComData(boss, next, sets + *(const uint32_t *)(sets + 8));
		BindEntityModelFromComData(boss + 0x9C, next, sets + *(const uint32_t *)(sets + 0xC));
		QueueChainTransformation(caster, seq);
	}

	static void Start(Node24 *n)
	{
		if (n->c == 1) StartBoss(Caster(), Target(), game_addr(BIND_OUT), game_addr(ANIM_SETS), (Finisher() != 0) + 1);
		if (n->c == 2) Bind3(Caster(), Target(), game_addr(BIND_OUT), game_addr(ANIM_SETS), (Finisher() != 0) + 1);
		if (n->c == 3) ComputeLength();
		if (n->c == 2) StartTasks();
	}

	static void Events(Node24 *n)
	{
		if (n->c == 0x16) AddRecordEmitter(game_addr(ORIG_Emitter), 2, 0, -0xA00, 0x500);
		if (n->c == 0x1D) AddRecordEmitter(game_addr(ORIG_Emitter), 4, 0, -0x800, 0x600);
		if (n->c == 0x48)
		{
			AddRecordEmitter(game_addr(ORIG_Emitter), 8, 0, -0xB00, 0x400);
			Node24 *t = AddSlotTask(game_addr(ORIG_RingA));
			const int16_t s = (int16_t)((CrtRand() % 0x400 + 0xA00) / 3);
			t->w1E = s;
			t->w1C = s;
			HitPointRecord(t);
			t = AddSlotTask(game_addr(ORIG_RingB));
			t->w1C = 0x1600;
			const int16_t sy = (int16_t)((CrtRand() % 0x400 + 0xA00) / 3);
			t->w22 = sy;
			t->w20 = sy;
			HitPointRecord(t);
		}
	}

	static uint32_t __cdecl EmitterTask(TaskNode *tn) { return RecordEmitter(tn, 0x10, game_addr(SEQ_EMIT)); }
	static uint32_t __cdecl RingATask(TaskNode *tn) { return SlotModelTask(SLOTMODELS[0], 0, (Node24 *)tn); }
	static uint32_t __cdecl RingBTask(TaskNode *tn) { return SlotModelTask(SLOTMODELS[1], 1, (Node24 *)tn); }
}

	void register_mag339_renzokuken_adel()
	{
		renzo339::RegisterCommon();
		register_port(renzo339::ORIG_Emitter, (void *)renzo339::EmitterTask, "339 Emitter", 339);
		register_port(renzo339::ORIG_RingA, (void *)renzo339::RingATask, "339 RingA", 339);
		register_port(renzo339::ORIG_RingB, (void *)renzo339::RingBTask, "339 RingB", 339);
	}
}

