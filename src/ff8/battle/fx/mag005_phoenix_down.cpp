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

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
namespace phoenixdown
{
	static const Mod MOD_005 = { "phoenixdown", 5, game::MAG_005_PHOENIXDOWN_FL, game::MAG_004_DOUBLE_FL,
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_005_PHOENIXDOWN_StepCounterLimit, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::MAG_005_PHOENIXDOWN_PacketCursor;
	static constexpr game::Id ARENA_ODD = game::MAGICEFFECT05_TEX_VRAM_A;
	static constexpr game::Id ARENA_EVEN = game::MAGICEFFECT05_TEX_VRAM_B;
	static constexpr game::Id Q_EMITTER = game::MAG_005_PHOENIXDOWN_EmitterQueue;
	static constexpr game::Id Q_UNUSED = game::MAG_005_PHOENIXDOWN_UnusedQueue;
	static constexpr game::Id Q_SPRITE = game::MAG_005_PHOENIXDOWN_SpriteQueue;
	static constexpr game::Id Q_GLINT = game::MAG_005_PHOENIXDOWN_GlintQueue;
	static constexpr game::Id GLINT_CURSOR_PTR = game::Effect_GlintPacketCursorPtr;

	static constexpr game::Id ORIG_Master = game::MAG_005_PHOENIXDOWN_Tick;
	static constexpr game::Id ORIG_Emitter = game::MAG_005_PHOENIXDOWN_Emitter_Tick;
	static constexpr game::Id ORIG_Fall = game::MAG_005_PHOENIXDOWN_FallSparkle_Tick;
	static constexpr game::Id ORIG_Ring = game::MAG_005_PHOENIXDOWN_RingSparkle_Tick;
	static constexpr game::Id ORIG_Sparkle = game::MAG_005_PHOENIXDOWN_Sparkle_Tick;
	static constexpr game::Id ORIG_Glint = game::Effect_Glint_Tick;
	static const void *SOUND_PhoenixDown() { return (const void *)game_addr(game::MAG_005_PHOENIXDOWN_Sound); }
	static constexpr game::Id SEQ_Revive = game::MAG_005_PHOENIXDOWN_SeqRevive;
	static constexpr game::Id SEQ_RiseA = game::MAG_005_PHOENIXDOWN_SeqRiseA;
	static constexpr game::Id SEQ_RiseB = game::MAG_005_PHOENIXDOWN_SeqRiseB;
	static constexpr game::Id SEQ_Ring = game::MAG_005_PHOENIXDOWN_SeqRing;
	static constexpr game::Id SEQ_Fall = game::MAG_005_PHOENIXDOWN_SeqFall;

	inline uint32_t Effect_CopyAnchorXZFromSource(uint32_t node, uint32_t out) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::Effect_CopyAnchorXZFromSource))(node, out); }
	inline uint32_t TransformCameraByShadowRotation(uint32_t pos, uint32_t scale, uint32_t offset) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::TransformCameraByShadowRotation))(pos, scale, offset); }
	inline uint32_t InitEffectSequenceFromData(uint32_t hdr, uint32_t ot, uint32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::InitEffectSequenceFromData))(hdr, ot, mode, cursor); }

	static inline uint32_t task_end(uint32_t node, uint8_t status)
	{
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}
}
}
}

namespace ff8fx
{
namespace act
{
namespace phoenixdown
{
	static uint32_t __cdecl Master(uint32_t a1)
	{
		g_mod = &MOD_005;
		const uint32_t node = a1;
		uint32_t states[5];
		states[0] = game_addr(game::MAG_005_PHOENIXDOWN_Root_State0_Advance);
		states[1] = game_addr(game::MAG_005_PHOENIXDOWN_SpawnEmitter);
		states[2] = game_addr(game::MAG_005_PHOENIXDOWN_NextAction);
		states[3] = game_addr(game::MAG_005_PHOENIXDOWN_WaitChildren);
		states[4] = game_addr(game::MAG_005_PHOENIXDOWN_Root_State4_Empty);
		if ((U8(node, 0x5C) & 1) != 0)
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = MEM<uint32_t>(game_addr(ARENA_ODD));
		else
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = MEM<uint32_t>(game_addr(ARENA_EVEN));
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = (uint16_t)x::ExecuteTaskQueue(game_addr(Q_EMITTER));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_UNUSED)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_SPRITE)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_GLINT)));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl SpawnEmitter(uint32_t a1)
	{
		U8(a1, 0x63) = 1;
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_EMITTER), game_addr(ORIG_Emitter), 0x64, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl NextAction(uint32_t a1)
	{
		if (U8(a1, 0x63) != 0)
			return a1;
		const int8_t action = S8(a1, 0x2A);
		if ((int16_t)action < S16(a1, 0x58))
		{
			U8(a1, 0x2A) = (uint8_t)(action + 1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) - 1);
		}
		else
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	static uint32_t __cdecl WaitChildren(uint32_t a1)
	{
		if (U16(a1, 0x5E) == 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return a1;
	}

	static uint32_t SpawnSparkle(uint32_t parent, uint32_t seq, uint32_t last, uint32_t offset)
	{
		const uint32_t s = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_SPRITE), game_addr(ORIG_Sparkle), 0x70, parent);
		Effect_CopyAnchorXZFromSource(s, s + 0x1C);
		U16(s, 0x54) = (uint16_t)offset;
		U32(s, 0x4C) = seq;
		U16(s, 0x52) = (uint16_t)last;
		U16(s, 0x68) = 0x1000;
		return s;
	}

	static void SpawnRising(uint32_t node)
	{
		if (S16(node, 0x24) > 0x1E)
			return;
		int16_t count = S16(node, 0x5A);
		if (count == 0)
			count = 1;
		if (S16(node, 0x5C) <= 0)
			return;
		const int32_t d = count;
		int32_t i = 0;
		do
		{
			int16_t radius;
			if ((x::CrtRand() & 1) != 0)
				radius = (int16_t)(S16(node, 0x58) / 2);
			else
				radius = S16(node, 0x58);
			if (radius == 0)
				radius = 1;
			const uint32_t seq = (x::CrtRand() & 1) != 0 ? game_addr(SEQ_RiseA) : game_addr(SEQ_RiseB);
			const uint32_t s = SpawnSparkle(node, seq, 0xF, 0xFFFFFE00);
			const int32_t angle = (int32_t)(x::CrtRand() & 0xFFF);
			const int32_t dist = (int16_t)((int32_t)x::CrtRand() % (int32_t)radius);
			U16(s, 0x68) = U16(node, 0x5E);
			U16(s, 0x6A) = (uint16_t)((S16(node, 0x60) - S16(node, 0x5E)) / 16);
			U16(s, 0x1C) = (uint16_t)(U16(s, 0x1C) + (uint16_t)(mul32((int32_t)x::computeSin((uint32_t)angle), dist) / 4096));
			U16(s, 0x20) = (uint16_t)(U16(s, 0x20) + (uint16_t)(mul32((int32_t)x::computeCosine((uint32_t)angle), dist) / 4096));
			const int32_t rem = (int32_t)x::CrtRand() % d;
			U16(s, 0x1E) = (uint16_t)(U16(s, 0x1E) + (uint16_t)(-0x100 - rem));
			i++;
		} while (i < S16(node, 0x5C));
	}

	static void SpawnRevive(uint32_t node)
	{
		if (U16(node, 0x24) == 0)
			SpawnSparkle(node, game_addr(SEQ_Revive), 2, 0xFFFFFC00);
		const int16_t t = S16(node, 0x24);
		if (t <= 0xA || t >= 0x16 || (t & 1) != 0)
			return;
		const uint32_t s = SpawnSparkle(node, game_addr(SEQ_Revive), 2, 0xFFFFFC00);
		int16_t dx = (int16_t)((int32_t)(x::CrtRand() & 0x7FFF) % (S16(node, 0x58) / 2 + 1));
		const int16_t dy = (int16_t)((int32_t)(x::CrtRand() & 0x7FFF) % (S16(node, 0x5A) + 1));
		int16_t dz = (int16_t)((int32_t)(x::CrtRand() & 0x7FFF) % (S16(node, 0x58) / 2 + 1));
		if ((x::CrtRand() & 1) != 0)
			dx = (int16_t)-dx;
		if ((x::CrtRand() & 1) != 0)
			dz = (int16_t)-dz;
		U16(s, 0x1C) = (uint16_t)(U16(s, 0x1C) + (uint16_t)dx);
		U16(s, 0x1E) = (uint16_t)(U16(s, 0x1E) - (uint16_t)dy);
		U16(s, 0x20) = (uint16_t)(U16(s, 0x20) + (uint16_t)dz);
	}

	static void SpawnRing(uint32_t node)
	{
		const int16_t t = S16(node, 0x24);
		if (t <= 0xA || t >= 0x1E)
			return;
		const uint32_t s = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_SPRITE), game_addr(ORIG_Ring), 0x70, node);
		Effect_CopyAnchorXZFromSource(s, s + 0x1C);
		U16(s, 0x1E) = (uint16_t)(U16(s, 0x1E) + U16(node, 0x62));
		U32(s, 0x60) = U32(s, 0x1C);
		U32(s, 0x64) = U32(s, 0x20);
		U32(s, 0x4C) = game_addr(SEQ_Ring);
		U16(s, 0x52) = 0xF;
		U16(s, 0x68) = 0x1000;
		U16(s, 0x42) = (uint16_t)(x::CrtRand() & 0xFFF);
		U16(s, 0x6C) = (uint16_t)((S16(node, 0x24) - 0xA) / 3 + 8);
	}

	static void SpawnFall(uint32_t node)
	{
		const int16_t t = S16(node, 0x24);
		if (t <= 0x14 || t >= 0x25)
			return;
		const uint32_t s = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_SPRITE), game_addr(ORIG_Fall), 0x70, node);
		U32(s, 0x4C) = game_addr(SEQ_Fall);
		U16(s, 0x52) = 9;
		U16(s, 0x68) = 0x1000;
		U16(s, 0x42) = (uint16_t)(x::CrtRand() & 0xFFF);
		Effect_CopyAnchorXZFromSource(s, s + 0x1C);
		U16(s, 0x1C) = (uint16_t)(U16(s, 0x1C) + (uint16_t)((int32_t)x::computeSin((uint32_t)(int32_t)S16(s, 0x42)) / 8));
		U16(s, 0x20) = (uint16_t)(U16(s, 0x20) + (uint16_t)((int32_t)x::computeCosine((uint32_t)(int32_t)S16(s, 0x42)) / 8));
		U16(s, 0x1E) = (uint16_t)(U16(s, 0x1E) + (uint16_t)(-0x80 - (int32_t)(x::CrtRand() & 0x1FF)));
		U16(s, 0x5A) = (uint16_t)((x::CrtRand() & 0xF) + 0x10);
	}

	static void SpawnGlints(uint32_t node)
	{
		const int16_t t = S16(node, 0x24);
		if (t <= 3 || t >= 0x10)
			return;
		int32_t count;
		if (t > 0xE)
			count = 1;
		else if (t > 0xA)
			count = 3;
		else
			count = (t > 5 ? 1 : 0) + 1;
		if (count <= 0)
			return;
		do
		{
			MEM<uint32_t>(game_addr(GLINT_CURSOR_PTR)) = game_addr(PACKET_CURSOR);
			const uint32_t s = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_GLINT), game_addr(ORIG_Glint), 0x5C, node);
			U16(s, 0x56) = 8;
			U8(s, 0x4C) = 0x10;
			U8(s, 0x4D) = 0x10;
			U8(s, 0x4E) = 0x10;
			U8(s, 0x50) = 0x10;
			U8(s, 0x51) = 5;
			U8(s, 0x52) = 8;
			U16(s, 0x3C) = 0x100;
			U16(s, 0x3E) = 0x10;
			U16(s, 0x40) = 0x40;
			U16(s, 0x42) = 0x10;
			if ((x::CrtRand() & 1) != 0)
				U16(s, 0x58) = 0x40;
			else
				U16(s, 0x58) = 0xFFC0;
		} while (--count != 0);
	}

	static uint32_t __cdecl Emitter(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[5];
		states[0] = game_addr(game::MAG_005_PHOENIXDOWN_Emitter_Init);
		states[1] = game_addr(game::MAG_005_PHOENIXDOWN_Emitter_Grow);
		states[2] = game_addr(game::MAG_005_PHOENIXDOWN_Emitter_Spread);
		states[3] = game_addr(game::MAG_005_PHOENIXDOWN_Emitter_End);
		states[4] = game_addr(game::MAG_005_PHOENIXDOWN_Emitter_State4_Empty);
		x::MAG_001_CURE_Emitter_UpdatePos(node);
		x::MAG_001_CURE_Emitter_ComputeModelBounds(node);
		callp(states[S8(node, 0x29)], node);
		if ((U8(node, 0x26) & 2) != 0)
		{
			SpawnRising(node);
			SpawnRevive(node);
			SpawnRing(node);
			SpawnFall(node);
			SpawnGlints(node);
		}
		if (U16(node, 0x24) == 0)
			x::BdPlaySE(P(SOUND_PhoenixDown()), 0, 0x80);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl EmitterInit(uint32_t a1)
	{
		U8(a1, 0x26) |= 2;
		U16(a1, 0x58) = 0;
		U16(a1, 0x5A) = 0;
		U16(a1, 0x62) = 0;
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x5C) = 1;
		U16(a1, 0x5E) = 0x100;
		U16(a1, 0x60) = 0x1000;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return a1;
	}

	static uint32_t __cdecl EmitterGrow(uint32_t a1)
	{
		U16(a1, 0x5E) = (uint16_t)(U16(a1, 0x5E) + 0x180);
		U16(a1, 0x58) = (uint16_t)(U16(a1, 0x58) + 0x66);
		if (S16(a1, 0x24) > 0xA)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	static uint32_t __cdecl EmitterSpread(uint32_t a1)
	{
		U16(a1, 0x58) = (uint16_t)(U16(a1, 0x58) - 0x4C);
		U16(a1, 0x5A) = (uint16_t)(U16(a1, 0x5A) + 0x40);
		U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) - 0x26);
		if (S16(a1, 0x24) > 0x14)
		{
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0x5C) = 1;
			U16(a1, 0x60) = 0x10;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return a1;
	}

	static uint32_t __cdecl EmitterEnd(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + 0xFF01);
		if (S16(node, 0x24) <= 0x1E)
			return 0;
		U8(U32(node, 0x10), 0x63) = 0;
		const int32_t action = S8(node, 0x2A);
		const uint32_t acts = U32(U32(node, 0xC), 4);
		x::ApplyActionResultToTarget(U32(acts + (uint32_t)(action * 20), 8));
		const uint16_t flags = U16(node, 0x26);
		const uint8_t st = U8(node, 0x29);
		U16(node, 0x26) = (uint16_t)((flags & 0xFFFD) | 1);
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	static void DrawFall(uint32_t node)
	{
		if ((U8(node, 0x26) & 4) != 0)
			return;
		const uint32_t hdr = x::Field_Alloc(0xB4);
		TransformCameraByShadowRotation(node + 0x1C, 0x1000, (uint32_t)(int32_t)S16(node, 0x54));
		U32(hdr, 0) = U32(node, 0x4C);
		U16(hdr, 4) = U16(node, 0x50);
		U16(hdr, 0x24) = 0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = InitEffectSequenceFromData(hdr, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0xB4);
	}

	static void DrawScaled(uint32_t node, uint16_t scale)
	{
		if ((U8(node, 0x26) & 4) != 0)
			return;
		TransformCameraByShadowRotation(node + 0x1C, scale, (uint32_t)(int32_t)S16(node, 0x54));
		const uint32_t hdr = x::Field_Alloc(0xB4);
		U32(hdr, 0) = U32(node, 0x4C);
		U16(hdr, 4) = U16(node, 0x50);
		U16(hdr, 0x24) = 0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = InitEffectSequenceFromData(hdr, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0xB4);
	}

	static uint32_t __cdecl Fall(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_005_PHOENIXDOWN_FallSparkle_State0_Advance);
		states[1] = game_addr(game::MAG_005_PHOENIXDOWN_FallSparkle_Step);
		states[2] = game_addr(game::MAG_005_PHOENIXDOWN_FallSparkle_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		DrawFall(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl FallStep(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + U16(node, 0x5A));
		if (a_743C20(node) != 0)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	static uint32_t __cdecl Ring(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_005_PHOENIXDOWN_RingSparkle_State0_Advance);
		states[1] = game_addr(game::MAG_005_PHOENIXDOWN_RingSparkle_Orbit);
		states[2] = game_addr(game::MAG_005_PHOENIXDOWN_RingSparkle_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x68) = (uint16_t)(U16(node, 0x68) + U16(node, 0x6A));
		DrawScaled(node, U16(node, 0x68));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl RingStep(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint16_t angle = (uint16_t)((U16(node, 0x42) + 0x80) & 0xFFF);
		U16(node, 0x1C) = U16(node, 0x60);
		U16(node, 0x42) = angle;
		U16(node, 0x20) = U16(node, 0x64);
		const int32_t sx = (int32_t)x::computeSin((uint32_t)(int32_t)(int16_t)angle) / (int32_t)S16(node, 0x6C);
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + (uint16_t)sx);
		const int32_t cz = (int32_t)x::computeCosine((uint32_t)(int32_t)S16(node, 0x42)) / (int32_t)S16(node, 0x6C);
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) - 0x40);
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + (uint16_t)cz);
		if (a_743C20(node) != 0)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	static uint32_t __cdecl Sparkle(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_005_PHOENIXDOWN_Sparkle_State0_Advance);
		states[1] = game_addr(game::MAG_005_PHOENIXDOWN_Sparkle_State1_CounterFinish);
		states[2] = game_addr(game::MAG_005_PHOENIXDOWN_Sparkle_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x68) = (uint16_t)(U16(node, 0x68) + U16(node, 0x6A));
		DrawScaled(node, U16(node, 0x68));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_005_PHOENIXDOWN_Root_State0_Advance, (void *)a_73A0D0, "005 MAG_005_PHOENIXDOWN_Root_State0_Advance" },
		{ game::MAG_005_PHOENIXDOWN_FallSparkle_State0_Advance, (void *)a_73A0D0, "005 sub_8D4C70" },
		{ game::MAG_005_PHOENIXDOWN_StepCounterLimit, (void *)a_743C20, "005 sub_8D4CB0" },
		{ game::MAG_005_PHOENIXDOWN_FallSparkle_State2_Empty, (void *)a_73A6D0, "005 nullsub_1854" },
		{ game::MAG_005_PHOENIXDOWN_RingSparkle_State0_Advance, (void *)a_73A0D0, "005 sub_8D4DD0" },
		{ game::MAG_005_PHOENIXDOWN_RingSparkle_State2_Empty, (void *)a_73A6D0, "005 nullsub_1855" },
		{ game::MAG_005_PHOENIXDOWN_Sparkle_State0_Advance, (void *)a_73A0D0, "005 sub_8D4ED0" },
		{ game::MAG_005_PHOENIXDOWN_Sparkle_State1_CounterFinish, (void *)a_743C00, "005 sub_8D4EE0" },
		{ game::MAG_005_PHOENIXDOWN_Sparkle_State2_Empty, (void *)a_73A6D0, "005 nullsub_1856" },
		{ game::MAG_005_PHOENIXDOWN_Emitter_State4_Empty, (void *)a_73A6D0, "005 nullsub_1857" },
		{ game::MAG_005_PHOENIXDOWN_Root_State4_Empty, (void *)a_73A6D0, "005 nullsub_1858" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ ORIG_Master, (void *)Master, "005 Phoenix Down Master" },
		{ game::MAG_005_PHOENIXDOWN_SpawnEmitter, (void *)SpawnEmitter, "005 SpawnEmitter" },
		{ game::MAG_005_PHOENIXDOWN_NextAction, (void *)NextAction, "005 NextAction" },
		{ game::MAG_005_PHOENIXDOWN_WaitChildren, (void *)WaitChildren, "005 WaitChildren" },
		{ ORIG_Emitter, (void *)Emitter, "005 Emitter" },
		{ game::MAG_005_PHOENIXDOWN_Emitter_Init, (void *)EmitterInit, "005 EmitterInit" },
		{ game::MAG_005_PHOENIXDOWN_Emitter_Grow, (void *)EmitterGrow, "005 EmitterGrow" },
		{ game::MAG_005_PHOENIXDOWN_Emitter_Spread, (void *)EmitterSpread, "005 EmitterSpread" },
		{ game::MAG_005_PHOENIXDOWN_Emitter_End, (void *)EmitterEnd, "005 EmitterEnd" },
		{ ORIG_Fall, (void *)Fall, "005 Fall" },
		{ game::MAG_005_PHOENIXDOWN_FallSparkle_Step, (void *)FallStep, "005 FallStep" },
		{ ORIG_Ring, (void *)Ring, "005 Ring" },
		{ game::MAG_005_PHOENIXDOWN_RingSparkle_Orbit, (void *)RingStep, "005 RingStep" },
		{ ORIG_Sparkle, (void *)Sparkle, "005 Sparkle" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag005_phoenix_down()
	{
		act::register_module(5);
		for (const act::phoenixdown::ModPort *p = act::phoenixdown::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(5, p->addr, p->port, p->name);
		for (const act::phoenixdown::ModPort *p = act::phoenixdown::PORTS; p->addr; p++)
			act::register_module_port(5, p->addr, p->port, p->name);
	}
}

