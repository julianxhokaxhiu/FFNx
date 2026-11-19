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

// Effect 62: Devour

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
namespace devour
{
	static Mod make_mod()
	{
		Mod m = {};
		m.name = "devour";
		m.effect_id = 62;
		m.lo = game::MAG_062_DEVOUR_FL;
		m.hi = game::MAG_061_REVIVE_FL;
		m.g[G_258BE30] = game::MAG_062_DEVOUR_EmitterQueue;
		m.f[F_73A1A0] = game::MAG_062_DEVOUR_Emitter;
		return m;
	}
	static const Mod MOD_062 = make_mod();

	static constexpr game::Id PACKET_CURSOR = game::MAG_062_DEVOUR_PacketCursor;
	static constexpr game::Id Q_EMITTER = game::MAG_062_DEVOUR_EmitterQueue;
	static constexpr game::Id Q_TASKS = game::MAG_062_DEVOUR_TaskQueue;
	static constexpr game::Id TIM_TITLE = game::MAG_062_DEVOUR_TimTitle;
	static constexpr game::Id TIM_PIC1 = game::MAG_062_DEVOUR_TimPicture1;
	static constexpr game::Id TIM_PIC2 = game::MAG_062_DEVOUR_TimPicture2;
	static constexpr game::Id TIM_PIC3 = game::MAG_062_DEVOUR_TimPicture3;
	static constexpr game::Id SOUND_TABLE = game::MAG_062_DEVOUR_OutcomeSoundTable;
	static constexpr game::Id FADE_LAYERS = game::BATTLE_STAGE_RENDER_GROUPS;
	static constexpr game::Id ENTITIES = game::BattleEntitySlotData;

	static constexpr game::Id ORIG_Master = game::MAG_062_DEVOUR_Master;
	static constexpr game::Id ORIG_Emitter = game::MAG_062_DEVOUR_Emitter;
	static constexpr game::Id ORIG_Cast = game::MAG_062_DEVOUR_Cast;
	static constexpr game::Id ORIG_Picture = game::MAG_062_DEVOUR_Picture;
	static constexpr game::Id ORIG_Darken = game::MAG_062_DEVOUR_Darken;
	static constexpr game::Id ORIG_Music = game::MAG_062_DEVOUR_MusicFade;

	static inline uint32_t task_end(uint32_t node, uint8_t status)
	{
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	static inline uint32_t first_target(uint32_t node)
	{
		const uint32_t acts = U32(U32(node, 0xC), 4);
		return U32(acts, 8);
	}
}
}
}

namespace ff8fx
{
namespace act
{
namespace devour
{
	static uint32_t __cdecl Master(uint32_t a1)
	{
		g_mod = &MOD_062;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::MAG_062_DEVOUR_CameraCopyPtr2)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::MAG_062_DEVOUR_CameraCopyPtr)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_062_DEVOUR_Master_NextState0);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_062_DEVOUR_Master_NextState1);
		states[2] = game_addr(game::MAG_062_DEVOUR_Master_WaitChildren);
		states[3] = game_addr(game::MAG_062_DEVOUR_Master_SpawnEmitter);
		states[4] = game_addr(game::MAG_062_DEVOUR_Master_ActionLoop);
		states[5] = game_addr(game::MAG_062_DEVOUR_Master_NextState5);
		states[6] = game_addr(game::MAG_062_DEVOUR_Master_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_062_DEVOUR_Master_NextState7);
		states[8] = game_addr(game::MAG_062_DEVOUR_Master_NextState8);
		states[9] = game_addr(game::MAG_062_DEVOUR_Master_Finish);
		states[10] = game_addr(game::MAG_062_DEVOUR_Master_Done);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::MAG_062_DEVOUR_ArenaOdd));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::MAG_062_DEVOUR_Arena2Odd));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::MAG_062_DEVOUR_PacketCursor2)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::MAG_062_DEVOUR_ArenaEven));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::MAG_062_DEVOUR_Arena2Even));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::MAG_062_DEVOUR_PacketCursor2)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_EMITTER)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_TASKS)));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl WaitChildren(uint32_t a1)
	{
		if (U8(a1, 0x28) == 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	static uint32_t __cdecl Emitter(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_062_DEVOUR_Emitter_StartCast);
		states[1] = game_addr(game::MAG_062_DEVOUR_Emitter_ReleaseMaster);
		states[2] = game_addr(game::MAG_062_DEVOUR_Emitter_ApplyResult);
		states[3] = game_addr(game::MAG_062_DEVOUR_Emitter_Done);
		x::MAG_001_CURE_Emitter_UpdatePos(node);
		x::MAG_001_CURE_Emitter_ComputeModelBounds(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl StartCast(uint32_t a1)
	{
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TASKS), game_addr(ORIG_Cast), 0x54, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl ReleaseMaster(uint32_t a1)
	{
		if (U16(a1, 0x24) == 0x4B)
		{
			U8(U32(a1, 0x10), 0x63) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return a1;
	}

	static uint32_t __cdecl Damage(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 0x53)
			return 0;
		const int32_t action = S8(node, 0x2A);
		const int32_t target = S8(node, 0x2B);
		const uint32_t acts = U32(U32(node, 0xC), 4);
		const uint32_t targets = U32(acts + (uint32_t)(action * 20), 8);
		x::ApplyActionResultToTarget(targets + (uint32_t)(target * 24));
		const uint8_t st = U8(node, 0x29);
		U8(node, 0x26) |= 1;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	static uint32_t __cdecl Cast(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_062_DEVOUR_Cast_LoadTextures);
		states[1] = game_addr(game::MAG_062_DEVOUR_Cast_StartParts);
		states[2] = game_addr(game::MAG_062_DEVOUR_Cast_PlayOutcomeSound);
		states[3] = game_addr(game::MAG_062_DEVOUR_Cast_Done);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl LoadTextures(uint32_t a1)
	{
		x::Battle_QueueTIMUpload_GetEOF(MEM<uint32_t>(game_addr(TIM_TITLE)));
		const int32_t r = (int32_t)x::CrtRand();
		const uint32_t node = a1;
		const int16_t pick = (int16_t)(r % 3);
		S16(node, 0x4E) = pick;
		switch (pick)
		{
		case 0:
			x::Battle_QueueTIMUpload_GetEOF(MEM<uint32_t>(game_addr(TIM_PIC1)));
			break;
		case 1:
			x::Battle_QueueTIMUpload_GetEOF(MEM<uint32_t>(game_addr(TIM_PIC2)));
			break;
		case 2:
			x::Battle_QueueTIMUpload_GetEOF(MEM<uint32_t>(game_addr(TIM_PIC3)));
			break;
		}
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl StartParts(uint32_t a1)
	{
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TASKS), game_addr(ORIG_Darken), 0x54, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TASKS), game_addr(ORIG_Music), 0x54, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TASKS), game_addr(ORIG_Picture), 0x54, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl Sound(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 0x23)
			return 0;
		int16_t k = S16(first_target(node), 6);
		if (k < 0)
			k = 0;
		else if (k > 8)
			k = 8;
		x::BdPlaySE(MEM<uint32_t>(game_addr(SOUND_TABLE) + (uint32_t)((int32_t)k * 4)), 0, 0x80);
		const uint8_t st = U8(node, 0x29);
		U8(node, 0x26) |= 1;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	static uint32_t Quad(uint32_t x, uint32_t y, uint32_t vx, uint32_t vy, uint32_t w, uint32_t h, uint32_t u, uint32_t v,
		uint32_t cx, uint32_t cy, uint32_t colour, uint32_t semi)
	{
		const uint32_t p = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		U8(p, 7) = 0x2C;
		U8(p, 7) = (uint8_t)((uint16_t)semi == 1 ? 0x2E : 0x2C);
		const int32_t vys = (int16_t)vy;
		const uint32_t tpage = ((((vx & 0x3C0) | 0x2800) >> 6) | ((uint32_t)(vys >> 4) & 0x10)) | ((uint32_t)(vys & 0x200) << 2);
		U32(p, 0) = 0x9000000;
		U16(p, 0x16) = (uint16_t)tpage;
		U16(p, 0xE) = (uint16_t)(((uint32_t)((int32_t)cx >> 4) & 0x3F) | (cy << 6));
		const uint8_t ub = (uint8_t)u;
		U8(p, 0x1C) = ub;
		U8(p, 0xC) = ub;
		if ((int32_t)(int16_t)w + (int32_t)(int16_t)u >= 0x100)
		{
			U8(p, 0x24) = 0xFF;
			U8(p, 0x14) = 0xFF;
		}
		else
		{
			const uint8_t u1 = (uint8_t)((uint8_t)w + ub);
			U8(p, 0x24) = u1;
			U8(p, 0x14) = u1;
		}
		const uint8_t vb = (uint8_t)v;
		U8(p, 0x15) = vb;
		U8(p, 0xD) = vb;
		if ((int32_t)(int16_t)h + (int32_t)(int16_t)v >= 0x100)
		{
			U8(p, 0x25) = 0xFF;
			U8(p, 0x1D) = 0xFF;
		}
		else
		{
			const uint8_t v1 = (uint8_t)((uint8_t)h + vb);
			U8(p, 0x25) = v1;
			U8(p, 0x1D) = v1;
		}
		U16(p, 0x18) = (uint16_t)x;
		U16(p, 8) = (uint16_t)x;
		U16(p, 0x20) = (uint16_t)(x + w);
		U16(p, 0x10) = (uint16_t)(x + w);
		U16(p, 0x12) = (uint16_t)y;
		U16(p, 0xA) = (uint16_t)y;
		U16(p, 0x22) = (uint16_t)(y + h);
		U16(p, 0x1A) = (uint16_t)(y + h);
		const uint8_t c = (uint8_t)colour;
		U8(p, 6) = c;
		U8(p, 5) = c;
		U8(p, 4) = c;
		x::SSIGPU_InsertPrimAltViewport(MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x14, p);
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = p + 0x28;
		return 0;
	}

	static uint32_t Picture(uint32_t node)
	{
		Quad(0, 0, 0x180, 0x100, 0x80, 0xD8, 0, 0, 0x140, 0xF0, U16(node, 0x50), 0);
		Quad(0x80, 0, 0x1C0, 0x100, 0xC0, 0xD8, 0, 0, 0x140, 0xF0, U16(node, 0x50), 0);
		return 0;
	}

	static uint32_t Strip(uint32_t node)
	{
		const uint16_t x = (uint16_t)(U16(node, 0x1C) + 0x60);
		Quad(x, 0x10, 0x180, 0x100, 0x100, 0x28, 0, 0xD8, 0x140, 0xF1, U16(node, 0x50), 1);
		Quad((uint16_t)(x + 0x100), 0x10, 0x200, 0x100, 0xC0, 0x28, 0, 0xD8, 0x140, 0xF1, U16(node, 0x50), 1);
		return 0;
	}

	static uint32_t __cdecl PictureTask(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_062_DEVOUR_Picture_Wait);
		states[1] = game_addr(game::MAG_062_DEVOUR_Picture_Scroll);
		states[2] = game_addr(game::MAG_062_DEVOUR_Picture_FadeOut);
		states[3] = game_addr(game::MAG_062_DEVOUR_Picture_Done);
		const int8_t state = S8(node, 0x29);
		callp(states[state], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		const uint32_t r = task_end(node, status);
		return r;
	}

	static uint32_t __cdecl PictureWait(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 7)
		{
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0x1C) = 0;
			U16(a1, 0x50) = 0x80;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return a1;
	}

	static uint32_t __cdecl PictureScroll(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) - 4);
		Strip(node);
		Picture(node);
		if (S16(node, 0x24) >= 0x4B)
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl PictureFade(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x50) = (uint16_t)(U16(node, 0x50) - 0x10);
		if (S16(node, 0x50) <= 0)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U16(node, 0x50) = 0;
			U8(node, 0x29) = (uint8_t)(st + 1);
			return 0;
		}
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) - 4);
		Strip(node);
		Picture(node);
		return 0;
	}

	static void TintAll(uint32_t level, uint32_t mode)
	{
		uint32_t e = game_addr(ENTITIES);
		for (int k = 7; k != 0; k--, e += 0x9C)
		{
			uint16_t f = U16(e, 0);
			if ((f & 2) != 0)
			{
				f = (uint16_t)(f | 0x800);
				U16(e, 0) = f;
				a_733950(e, level, mode);
			}
		}
	}

	static void HideTarget(uint32_t node)
	{
		if (U16(first_target(node), 6) == 8)
			return;
		const uint32_t slot = U8(node, 0x2D);
		U8(game_addr(ENTITIES) + slot * 0x9C, 0) |= 4;
	}

	static uint32_t __cdecl Darken(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[6];
		states[0] = game_addr(game::MAG_062_DEVOUR_Darken_SaveEntityFlags);
		states[1] = game_addr(game::MAG_062_DEVOUR_Darken_BlackoutHideTarget);
		states[2] = game_addr(game::MAG_062_DEVOUR_Darken_Wait);
		states[3] = game_addr(game::MAG_062_DEVOUR_Darken_FadeOut);
		states[4] = game_addr(game::MAG_062_DEVOUR_Darken_RestoreFlagsHideTarget);
		states[5] = game_addr(game::MAG_062_DEVOUR_Darken_Done);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl DarkenKeep(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t e = game_addr(ENTITIES);
		if (S16(node, 0x24) < 0xA)
			return node;
		for (int k = 0; k < 7; k++, e += 0x9C)
			U32(node, 0x30 + 4 * k) = U16(e, 0);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return node;
	}

	static uint32_t __cdecl DarkenIn(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x4C) = 0x1000;
		uint32_t p = game_addr(FADE_LAYERS) + 0x2D;
		for (int k = 4; k != 0; k--, p += 0x2C)
		{
			U8(p, 1) = 0;
			U8(p, 0) = 0;
			U8(p, -1) = 0;
			U16(p, -0x27) = 0x1000;
		}
		TintAll(0x1000, 0);
		HideTarget(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl DarkenWait(uint32_t a1)
	{
		if (S16(a1, 0x24) > 0x53)
		{
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0x4C) = 0x1000;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return a1;
	}

	static uint32_t __cdecl DarkenOut(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x4C) = (uint16_t)(U16(node, 0x4C) + 0xFE00);
		if (S16(node, 0x4C) <= 0)
		{
			const uint8_t st = U8(node, 0x29);
			U16(node, 0x4C) = 0;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(node, 0x4C);
		uint32_t p = game_addr(FADE_LAYERS) + 6;
		for (int k = 4; k != 0; k--, p += 0x2C)
			U16(p, 0) = level;
		TintAll(level, 0);
		return 0;
	}

	static uint32_t __cdecl DarkenRestore(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t e = game_addr(ENTITIES);
		uint32_t kept = node + 0x30;
		for (int k = 7; k != 0; k--, e += 0x9C, kept += 4)
		{
			const uint16_t f = U16(e, 0);
			if ((f & 2) != 0)
				U16(e, 0) = (uint16_t)(((U16(kept, 0) ^ f) & 0x804) ^ f);
		}
		HideTarget(node);
		const uint8_t st = U8(node, 0x29);
		U8(node, 0x26) |= 1;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	static uint32_t __cdecl Music(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[5];
		states[0] = game_addr(game::MAG_062_DEVOUR_MusicFade_Wait);
		states[1] = game_addr(game::MAG_062_DEVOUR_MusicFade_Down);
		states[2] = game_addr(game::MAG_062_DEVOUR_MusicFade_Wait2);
		states[3] = game_addr(game::MAG_062_DEVOUR_MusicFade_Up);
		states[4] = game_addr(game::MAG_062_DEVOUR_MusicFade_Done);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl MusicWait(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0xA)
		{
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0x4C) = 0x7F;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return a1;
	}

	static uint32_t __cdecl MusicDown(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x4C) = (uint16_t)(U16(node, 0x4C) - 8);
		if (S16(node, 0x4C) <= 0)
		{
			const uint8_t st = U8(node, 0x29);
			U16(node, 0x4C) = 0;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return x::Music_SetVolumeImmediate(0, (uint32_t)(int32_t)S16(node, 0x4C));
	}

	static uint32_t __cdecl MusicWait2(uint32_t a1)
	{
		if (S16(a1, 0x24) > 0x53)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	static uint32_t __cdecl MusicUp(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x4C) = (uint16_t)(U16(node, 0x4C) + 8);
		if (S16(node, 0x4C) >= 0x7F)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U16(node, 0x4C) = 0x7F;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return x::Music_SetVolumeImmediate(0, (uint32_t)(int32_t)S16(node, 0x4C));
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_062_DEVOUR_Master_NextState0, (void *)a_73A0D0, "062 MAG_062_sub_7E4D30" },
		{ game::MAG_062_DEVOUR_Master_NextState1, (void *)a_73A0D0, "062 MAG_062_sub_7E4D40" },
		{ game::MAG_062_DEVOUR_Master_SpawnEmitter, (void *)a_73A170, "062 MAG_062_sub_7E4D60" },
		{ game::MAG_062_DEVOUR_Picture_Done, (void *)a_73A6D0, "062 nullsub_1460" },
		{ game::MAG_062_DEVOUR_Darken_Done, (void *)a_73A6D0, "062 nullsub_1461" },
		{ game::MAG_062_DEVOUR_MusicFade_Done, (void *)a_73A6D0, "062 nullsub_1462" },
		{ game::MAG_062_DEVOUR_Cast_Done, (void *)a_73A6D0, "062 nullsub_1459" },
		{ game::MAG_062_DEVOUR_Emitter_Done, (void *)a_73A6D0, "062 nullsub_1458" },
		{ game::MAG_062_DEVOUR_Master_ActionLoop, (void *)a_747500, "062 MAG_062_sub_7E57F0" },
		{ game::MAG_062_DEVOUR_Master_NextState5, (void *)a_73A0D0, "062 MAG_062_sub_7E5830" },
		{ game::MAG_062_DEVOUR_Master_WaitQueuesEmpty, (void *)a_747550, "062 MAG_062_sub_7E5840" },
		{ game::MAG_062_DEVOUR_Master_NextState7, (void *)a_73A0D0, "062 MAG_062_sub_7E5850" },
		{ game::MAG_062_DEVOUR_Master_NextState8, (void *)a_73A0D0, "062 MAG_062_sub_7E5860" },
		{ game::MAG_062_DEVOUR_Master_Finish, (void *)a_7475A0, "062 MAG_062_sub_7E5870" },
		{ game::MAG_062_DEVOUR_Master_Done, (void *)a_73A6D0, "062 nullsub_1457" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ ORIG_Master, (void *)Master, "062 Devour Master" },
		{ game::MAG_062_DEVOUR_Master_WaitChildren, (void *)WaitChildren, "062 WaitChildren" },
		{ ORIG_Emitter, (void *)Emitter, "062 Emitter" },
		{ game::MAG_062_DEVOUR_Emitter_StartCast, (void *)StartCast, "062 StartCast" },
		{ game::MAG_062_DEVOUR_Emitter_ReleaseMaster, (void *)ReleaseMaster, "062 ReleaseMaster" },
		{ game::MAG_062_DEVOUR_Emitter_ApplyResult, (void *)Damage, "062 Damage" },
		{ ORIG_Cast, (void *)Cast, "062 Cast" },
		{ game::MAG_062_DEVOUR_Cast_LoadTextures, (void *)LoadTextures, "062 LoadTextures" },
		{ game::MAG_062_DEVOUR_Cast_StartParts, (void *)StartParts, "062 StartParts" },
		{ game::MAG_062_DEVOUR_Cast_PlayOutcomeSound, (void *)Sound, "062 Sound" },
		{ ORIG_Picture, (void *)PictureTask, "062 Picture" },
		{ game::MAG_062_DEVOUR_Picture_Wait, (void *)PictureWait, "062 PictureWait" },
		{ game::MAG_062_DEVOUR_Picture_Scroll, (void *)PictureScroll, "062 PictureScroll" },
		{ game::MAG_062_DEVOUR_Picture_FadeOut, (void *)PictureFade, "062 PictureFade" },
		{ ORIG_Darken, (void *)Darken, "062 Darken" },
		{ game::MAG_062_DEVOUR_Darken_SaveEntityFlags, (void *)DarkenKeep, "062 DarkenKeep" },
		{ game::MAG_062_DEVOUR_Darken_BlackoutHideTarget, (void *)DarkenIn, "062 DarkenIn" },
		{ game::MAG_062_DEVOUR_Darken_Wait, (void *)DarkenWait, "062 DarkenWait" },
		{ game::MAG_062_DEVOUR_Darken_FadeOut, (void *)DarkenOut, "062 DarkenOut" },
		{ game::MAG_062_DEVOUR_Darken_RestoreFlagsHideTarget, (void *)DarkenRestore, "062 DarkenRestore" },
		{ ORIG_Music, (void *)Music, "062 Music" },
		{ game::MAG_062_DEVOUR_MusicFade_Wait, (void *)MusicWait, "062 MusicWait" },
		{ game::MAG_062_DEVOUR_MusicFade_Down, (void *)MusicDown, "062 MusicDown" },
		{ game::MAG_062_DEVOUR_MusicFade_Wait2, (void *)MusicWait2, "062 MusicWait2" },
		{ game::MAG_062_DEVOUR_MusicFade_Up, (void *)MusicUp, "062 MusicUp" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag062_devour()
	{
		act::register_module(62);
		for (const act::devour::ModPort *p = act::devour::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(62, p->addr, p->port, p->name);
		for (const act::devour::ModPort *p = act::devour::PORTS; p->addr; p++)
			act::register_module_port(62, p->addr, p->port, p->name);
	}
}

