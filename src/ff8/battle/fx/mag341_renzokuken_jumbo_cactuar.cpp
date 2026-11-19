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
namespace renzo341
{
	static constexpr game::Id PTS_A = game::unk_21E5BE0, PTS_B = game::unk_21E5C20, TARGET_SLOT = game::unk_21E5C60;
	static constexpr game::Id POOL_A = game::unk_21E5C68, POOL_A_END = game::unk_21E5E48;
	static constexpr game::Id POOL_B = game::unk_21E5E48, POOL_B_SPAWN_END = game::unk_21E60C8, POOL_B_END = game::unk_21E6208;
	static constexpr game::Id HIT_REC = game::unk_21E6208, MASTER_POOL = game::unk_21E6958, Q_MASTER = game::unk_21E6968;
	static constexpr game::Id SLOTS = game::unk_21E6978, SLOTS_END = game::unk_21E75F8;
	static constexpr game::Id POOL_C = game::unk_21E75F8, POOL_C_END = game::unk_21E7918;
	static constexpr game::Id SPL_A = game::unk_21E7918, SPL_B = game::unk_21E7A18;
	static constexpr game::Id HIT_POS = game::unk_21E7B18, HIT_DIR = game::unk_21E7B20, Q_MAIN = game::unk_21E7B30, MAIN_POOL = game::unk_21E7B40;
	static constexpr game::Id PREV_HIT = game::unk_21E8950, CONTEXT = game::unk_21E8968, CASTER_SLOT = game::unk_21E896C, TEX_FILE = game::unk_21E8970;
	static constexpr game::Id RING = game::unk_21E8978, RING_END = game::unk_21E8A04, LENGTH = game::unk_21E8A04, FINISHER = game::unk_21E8A08;
	static constexpr game::Id TEX_BASE = game::unk_21E8A0C, PACKET_CURSOR = game::unk_21E8A10, HIT_OFFS = game::unk_21E8A18;
	static const uint32_t PARITY = 0xC000;
	static const void *SOUND() { return (const void *)game_addr(game::unk_E4EA18); }
	static constexpr game::Id TEXTURE_NAME = game::aMag333Tim_1, CAMERA = game::MAG341_RENZOKUKEN_VS_JUMBO_CACTUAR_cameraAnim, ANIM_SETS = game::unk_E4F038, BIND_OUT = game::unk_24FE9A8;
	static constexpr game::Id MODEL_SPARKLE = game::unk_E4EA1C, SPARKLE_FADE = game::unk_E4EBA4, SPARKLE_DEPTH = game::unk_E4EB94;
	static constexpr game::Id MODEL_SLASH = game::unk_C907A4, MODEL_STREAK = game::unk_C9118C;
	static constexpr game::Id SEQ_FLASH = game::unk_C9021C, SEQ_DEBRIS = game::unk_C904EC, SEQ_SPARK = game::unk_C903C8, SPARK_FRAMES = game::unk_C96DC0;
	static constexpr game::Id SLASH_REF = game::unk_C96E20, HIT_LEAD = game::unk_C96E30;
	static constexpr game::Id RING_REF = game::unk_C96E60;
	static constexpr game::Id ORIG_FileLoader = game::MAG_341_UNKNOWN_FL, ORIG_Setup = game::MAG_341_UNKNOWN;
	static constexpr game::Id ORIG_Controller = game::MAG_341_sub_579BA0, ORIG_HitSpark = game::MAG_341_sub_57A010, ORIG_Sparkle = game::MAG_341_sub_57A2A0, ORIG_Trail = game::MAG_341_sub_57A5D0, ORIG_SparkSpawn = game::MAG_341_sub_57AB50, ORIG_Hit = game::MAG_341_sub_57AC80, ORIG_Slash = game::MAG_341_sub_57B050, ORIG_Ring = game::MAG_341_sub_57B330, ORIG_Dust = game::MAG_341_sub_57B520, ORIG_Shake = game::MAG_341_sub_57B880, ORIG_Master = game::MAG_341_sub_57B970;

#include "renzo_boss_defs.inc"
}
}

namespace ff8fx
{
namespace renzo341
{
#define RENZO_ID 341
#define RENZO_IDS "341"
#define RENZO_LENGTH_EXTRA
#define RENZO_LATE_EVENTS
	static void LateEvents(Node24 *n);
	static void Events(Node24 *n);
	static int32_t LengthExtra() { return Finisher() != 0 ? 0x2D : 0xA; }
#include "renzo_boss.inc"
#undef RENZO_LENGTH_EXTRA
#undef RENZO_LATE_EVENTS
#undef RENZO_IDS
#undef RENZO_ID

	static const SlotModelDef SLOTMODELS[1] = {
		{ ORIG_Ring, game::unk_C91DF4, RING_REF, 3, 8, 409, 0x12, false },
	};

	static void LateEvents(Node24 *n)
	{
		if (Finisher() == 0) return;
		if (n->c == 0x5A) *Caster() = (uint8_t)(*Caster() | 8);
		if (n->c == 0x7D) *(uint16_t *)Caster() = (uint16_t)(*(uint16_t *)Caster() & 0xFFF7);
	}

	static void Events(Node24 *n)
	{
		if (n->c == 0x50)
		{
			Node24 *t = AddSlotTask(game_addr(ORIG_Ring));
			const int16_t s = (int16_t)((CrtRand() % 0x600 + 0xE00) / 3);
			t->w1E = s;
			t->w1C = s;
			HitPointRecord(t);
		}
		if (n->c == 0x57)
		{
			Node24 *t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_Dust));
			t->c = 0;
			t->e = 0xF;
			t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_Shake));
			t->c = 0;
			t->e = 0xF;
		}
	}

	static const DustDef DUST = { ORIG_Dust, true, 3, 0x10, 0xC00, 0x1800, 0x12C, 0x190, 0xA0, 0xFA };
	static uint32_t __cdecl DustTask(TaskNode *tn) { return DustRun(DUST, (Node24 *)tn); }
	static const ShakeDef SHAKE_DEF = { true, 10, 200, 0xA };
	static uint32_t __cdecl ShakeTask(TaskNode *tn) { return ShakeRun(SHAKE_DEF, (Node24 *)tn); }

	static uint32_t __cdecl RingTask(TaskNode *tn) { return SlotModelTask(SLOTMODELS[0], 0, (Node24 *)tn); }
}

	void register_mag341_renzokuken_jumbo_cactuar()
	{
		renzo341::RegisterCommon();
		register_port(renzo341::ORIG_Ring, (void *)renzo341::RingTask, "341 Ring", 341);
		register_port(renzo341::ORIG_Dust, (void *)renzo341::DustTask, "341 Dust", 341);
		register_port(renzo341::ORIG_Shake, (void *)renzo341::ShakeTask, "341 Shake", 341);
	}
}

