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
namespace renzo343
{
	static constexpr game::Id PTS_A = game::unk_21DFEF0, PTS_B = game::unk_21DFF30, TARGET_SLOT = game::unk_21DFF70;
	static constexpr game::Id POOL_A = game::unk_21DFF78, POOL_A_END = game::unk_21E0158;
	static constexpr game::Id POOL_B = game::unk_21E0158, POOL_B_SPAWN_END = game::unk_21E03D8, POOL_B_END = game::unk_21E0518;
	static constexpr game::Id HIT_REC = game::unk_21E0518, MASTER_POOL = game::unk_21E0C68, Q_MASTER = game::unk_21E0C78;
	static constexpr game::Id SLOTS = game::unk_21E0C88, SLOTS_END = game::unk_21E1908;
	static constexpr game::Id POOL_C = game::unk_21E1908, POOL_C_END = game::unk_21E1C28;
	static constexpr game::Id SPL_A = game::unk_21E1C28, SPL_B = game::unk_21E1D28, SAVED_MAT = game::unk_21E1E28;
	static constexpr game::Id HIT_POS = game::unk_21E1E48, HIT_DIR = game::unk_21E1E50, Q_MAIN = game::unk_21E1E60, MAIN_POOL = game::unk_21E1E70;
	static constexpr game::Id PREV_HIT = game::unk_21E2C80, CONTEXT = game::unk_21E2C98, CASTER_SLOT = game::unk_21E2C9C, TEX_FILE = game::tim_blob;
	static constexpr game::Id RING = game::unk_21E2CA8, RING_END = game::unk_21E2D34, LENGTH = game::unk_21E2D34, FINISHER = game::unk_21E2D38;
	static constexpr game::Id TEX_BASE = game::unk_21E2D3C, PACKET_CURSOR = game::packet_cursor, HIT_OFFS = game::unk_21E2D48;
	static const uint32_t PARITY = 0xC000;
	static const void *SOUND() { return (const void *)game_addr(game::sound_block); }
	static constexpr game::Id TEXTURE_NAME = game::aMag333Tim, CAMERA = game::MAG343_RENZOKUKEN_VS_GRIEVER_cameraAnim, ANIM_SETS = game::unk_E426C0, BIND_OUT = game::unk_24FE948;
	static constexpr game::Id MODEL_SPARKLE = game::unk_E42150, SPARKLE_FADE = game::unk_E422D8, SPARKLE_DEPTH = game::unk_E422C8;
	static constexpr game::Id MODEL_SLASH = game::unk_C82B34, MODEL_STREAK = game::unk_C8351C;
	static constexpr game::Id SEQ_FLASH = game::unk_C825AC, SEQ_DEBRIS = game::unk_C8287C, SEQ_SPARK = game::unk_C82758, SPARK_FRAMES = game::unk_C89150;
	static constexpr game::Id SLASH_REF = game::unk_C891B0, HIT_LEAD = game::unk_C891C0;
	static constexpr game::Id SEQ_EMIT = game::unk_C82400, MODEL_FINAL = game::unk_C852AC;
	static constexpr game::Id ORIG_FileLoader = game::MAG_343_UNKNOWN_FL, ORIG_Setup = game::MAG_343_UNKNOWN;
	static constexpr game::Id ORIG_Controller = game::MAG_343_sub_575770, ORIG_HitSpark = game::MAG_343_sub_575BA0, ORIG_Sparkle = game::MAG_343_sub_575E30, ORIG_Trail = game::MAG_343_sub_576160, ORIG_SparkSpawn = game::MAG_343_sub_5766E0, ORIG_Hit = game::MAG_343_sub_576810, ORIG_Slash = game::MAG_343_sub_576BE0, ORIG_Emitter = game::MAG_343_sub_576EC0, ORIG_Final = game::MAG_343_sub_577240, ORIG_Master = game::task_function;

#include "renzo_boss_defs.inc"
}
}

namespace ff8fx
{
namespace renzo343
{
#define RENZO_ID 343
#define RENZO_IDS "343"
#define RENZO_HITSPARK_NULLCHECK
	static void Events(Node24 *n);
#include "renzo_boss.inc"
#undef RENZO_HITSPARK_NULLCHECK
#undef RENZO_IDS
#undef RENZO_ID

	static void Events(Node24 *n)
	{
		if (n->c == 0x24) AddRecordEmitter(game_addr(ORIG_Emitter), 4, 0x600, -0xB00, 0x500);
		if (n->c == 0x2C) AddRecordEmitter(game_addr(ORIG_Emitter), 8, -0x200, -0x800, 0x700);
		if (n->c == 0x37)
		{
			Node24 *t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_Final));
			ff8fx::copy<(8)>(&t->w10, (const void *)game_addr(HIT_POS));
			t->c = 0;
			const int16_t s = (int16_t)((CrtRand() % 0xC00 + 0x1C00) / 3);
			t->w1E = s;
			t->w1C = s;
			ff8fx::copy<(0x20)>((void *)game_addr(SAVED_MAT), Caster() + 0x40);
		}
	}

	static uint32_t __cdecl EmitterTask(TaskNode *tn) { return RecordEmitter(tn, 0xE, game_addr(SEQ_EMIT)); }

	static void FinalDraw(const Node24 *n, int32_t scale, int32_t fade)
	{
		int16_t ang[3] = { 0, 0, 0x400 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		MatrixMultiply((const Mat4x3 *)game_addr(SAVED_MAT), &m);
		m.t[0] = n->w10;
		m.t[1] = n->w12;
		int32_t sv[3];
		sv[2] = scale;
		sv[1] = scale;
		sv[0] = scale;
		m.t[2] = n->w14;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_FINAL);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (n->c >= 6)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}
	static int32_t FinalFade(int32_t c) { return mul32(c - 6, 341); }

	static uint32_t __cdecl FinalTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		FinalDraw(n, n->w1C, FinalFade(n->c));
		if (DrawOnly()) return 0;
		const int16_t rate = n->w1E;
		n->w1C = (int16_t)(n->w1C + rate);
		n->w1E = (int16_t)(rate - (int16_t)(rate / 4));
		n->c++;
		return n->c >= 0x12 ? TASK_END : 0;
	}
}

	void register_mag343_renzokuken_griever()
	{
		renzo343::RegisterCommon();
		register_port(renzo343::ORIG_Emitter, (void *)renzo343::EmitterTask, "343 Emitter", 343);
		register_port(renzo343::ORIG_Final, (void *)renzo343::FinalTask, "343 Final", 343);
	}
}

