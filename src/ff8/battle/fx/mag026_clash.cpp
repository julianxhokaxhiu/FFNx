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

// Effect 26: Clash: built on the heal-spell effect library

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
namespace clash026
{
	static constexpr game::Id PACKET_CURSOR = game::MAG_026_PacketCursor;
	static constexpr game::Id ARENA_EVEN = game::MAG_026_ArenaA, ARENA_ODD = game::MAG_026_ArenaB;
	static constexpr game::Id Q_EMITTER = game::MAG_026_EmitterQueue;
	static constexpr game::Id Q_CHARGE = game::MAG_026_KnockBackQueue;
	static constexpr game::Id Q_SPRITE = game::MAG_026_SpriteQueue;
	static constexpr game::Id Q_PRIM = game::MAG_026_PrimModelQueue;
	static constexpr game::Id Q_FLASH = game::MAG_026_FlashQueue;
	static constexpr game::Id SOUND_Clash = game::unk_160F6C8;
	static constexpr game::Id FLASH_SCRIPT = game::unk_1610774;
	static constexpr game::Id FADE_TABLE = game::unk_161075C;
	static constexpr game::Id MODEL_Ring = game::unk_16104B4, MODEL_WaveA = game::unk_160FA94, MODEL_WaveB = game::unk_160FF3C;
	static constexpr game::Id SEQ_Spark = game::unk_160F7A8, SEQ_Dust = game::unk_160F6CC;

	static constexpr game::Id ORIG_Master = game::MAG_026_CLASH_Master;
	static constexpr game::Id ORIG_Emitter = game::MAG_026_Emitter;
	static constexpr game::Id ORIG_BurstRing = game::MAG_026_BurstRing;
	static constexpr game::Id ORIG_Spark = game::MAG_026_Spark;
	static constexpr game::Id ORIG_Flash = game::MAG_026_ScreenFlash;
	static constexpr game::Id ORIG_WaveA = game::MAG_026_WaveA;
	static constexpr game::Id ORIG_WaveB = game::MAG_026_WaveB;
	static constexpr game::Id ORIG_Dust = game::MAG_026_Dust;
	static constexpr game::Id ORIG_Charge = game::MAG_026_KnockBack;
	static constexpr game::Id ORIG_LandRing = game::MAG_026_LandRing;
	static constexpr game::Id ORIG_LandSpark = game::MAG_026_LandSpark;
	static constexpr game::Id ORIG_WalkDust = game::MAG_026_WalkDust;

	namespace cx
	{
		inline uint32_t CopyAnchorFromSource(uint32_t node, uint32_t out) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::Effect_CopyAnchorXZFromSource))(node, out); }
		inline uint32_t TransformCameraByShadowRotation(uint32_t pos, uint32_t a, uint32_t b) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::TransformCameraByShadowRotation))(pos, a, b); }
		inline uint32_t InitEffectSequenceFromData(uint32_t h, uint32_t ot, uint32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::InitEffectSequenceFromData))(h, ot, mode, cursor); }
		inline uint32_t BuildRotationMatrixFromAngles(uint32_t angles, uint32_t out) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::BuildRotationMatrixFromAngles))(angles, out); }
		inline uint32_t QueueChainTransformation(uint32_t entity, uint32_t id) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	}

	typedef uint32_t (__cdecl *StateFn)(uint32_t);

	static inline uint32_t task_end(uint32_t node, uint8_t status)
	{
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	static inline uint32_t RenderOT(uint32_t off) { return MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }
	static inline uint32_t EntityOf(uint32_t node) { return game_addr(game::BattleEntitySlotData) + 0x9C * (uint32_t)U8(node, 0x2D); }
	static uint32_t __cdecl Nop(uint32_t a1) { return a1; }
}
}
}

namespace ff8fx
{
namespace act
{
namespace clash026
{
	static void PrimDraw(uint32_t node)
	{
		if (U8(node, 0x26) & 4)
			return;
		Mat4x3 m = {};
		cx::BuildRotationMatrixFromAngles(node + 0x44, P(&m));
		m.t[0] = S16(node, 0x1C);
		m.t[1] = S16(node, 0x1E);
		m.t[2] = S16(node, 0x20);
		x::scale3DMatrix(P(&m), node + 0x30);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), P(&m), P(&m));
		x::GTE_SetRotMatrix_W(P(&m));
		x::GTE_SetTransVector_W(P(&m));
		const uint32_t h = x::Field_Alloc(0x58);
		U32(h, 0) = U32(node, 0x4C);
		U32(h, 8) = U32(node, 0x40);
		S32(h, 0xC) = S16(node, 0x50);
		U32(h, 0x1C) = 0xF0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = x::Effect_RenderPrimModel(h, RenderOT(0x44), 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0x58);
	}

	static void SpriteDraw(uint32_t node, uint32_t scale)
	{
		if (U8(node, 0x26) & 4)
		{
			return;
		}
		cx::TransformCameraByShadowRotation(node + 0x1C, scale, (uint32_t)(int32_t)S16(node, 0x54));
		const uint32_t h = x::Field_Alloc(0xB4);
		U32(h, 0) = U32(node, 0x4C);
		U16(h, 4) = U16(node, 0x50);
		U16(h, 0x24) = 0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = cx::InitEffectSequenceFromData(h, RenderOT(0x44), 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0xB4);
	}

	static void FlashDraw(uint32_t colour)
	{
		const uint32_t bucket = RenderOT(0x1C);
		uint32_t pkt = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		U32(pkt, 4) = colour;
		U32(pkt, 0) = 0x5000000;
		U8(pkt, 7) = 0x2A;
		U32(pkt, 8) = 0;
		U32(pkt, 0xC) = 0x140;
		U32(pkt, 0x10) = 0xD80000;
		U32(pkt, 0x14) = 0xD80140;
		x::SSIGPU_InsertPrimAltViewport(bucket, pkt);
		pkt += 0x18;
		const uint32_t tpage = x::sub_45C690(0, 1, 0x280, 0) & 0xFFFF;
		x::sub_45BFC0(pkt, 0, 0, tpage, 0);
		x::SSIGPU_InsertPrimAutoDepth(bucket, pkt);
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = pkt + 0xC;
	}

	static int16_t FadeAt(int16_t i)
	{
		if (i > 0xB) i = 0xB;
		else if (i < 0) i = 0;
		return MEM<int16_t>(game_addr(FADE_TABLE) + 2 * (int32_t)i);
	}

	static uint32_t FrameStep(uint32_t node)
	{
		U16(node, 0x50) = (uint16_t)(U16(node, 0x50) + 1);
		const int16_t f = S16(node, 0x50), last = S16(node, 0x52);
		if (f > last)
		{
			U8(node, 0x26) |= 4;
			U16(node, 0x50) = (uint16_t)last;
			return 1;
		}
		return 0;
	}

	static void RandomOffset(uint32_t node, uint32_t r, uint32_t angle)
	{
		const int32_t q = (int32_t)x::CrtRand();
		const int32_t d = (int16_t)(q % (int32_t)(int16_t)r);
		const int32_t a = (int16_t)angle;
		const int32_t s = (int32_t)x::computeSin((uint32_t)a);
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + (uint16_t)(mul32(s, d) / 4096));
		const int32_t c = (int32_t)x::computeCosine((uint32_t)a);
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + (uint16_t)(mul32(c, d) / 4096));
	}

	static void SparkStart(uint32_t node, int shift)
	{
		U32(node, 0x58) = U32(node, 0x1C);
		U32(node, 0x5C) = U32(node, 0x20);
		RandomOffset(node, U16(node, 0x6C), U16(node, 0x6A));
		const int32_t dx = S16(node, 0x1C) - S16(node, 0x58);
		U8(node, 0x26) |= 4;
		U32(node, 0x4C) = game_addr(SEQ_Spark);
		U16(node, 0x52) = 0xF;
		const int32_t dy = S16(node, 0x1E) - S16(node, 0x5A);
		const int32_t dz = S16(node, 0x20) - S16(node, 0x5C);
		const int32_t div = 1 << shift;
		const int16_t vx = (int16_t)(dx / div), vy = (int16_t)(dy / div), vz = (int16_t)(dz / div);
		U16(node, 0x58) = (uint16_t)vx;
		U16(node, 0x60) = (uint16_t)(-((int32_t)vx / 16));
		U16(node, 0x5A) = (uint16_t)vy;
		U16(node, 0x62) = (uint16_t)(-((int32_t)vy / 16));
		U16(node, 0x5C) = (uint16_t)vz;
		U16(node, 0x64) = (uint16_t)(-((int32_t)vz / 16));
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
	}

	static uint32_t __cdecl SparkStart8(uint32_t a1) { SparkStart(a1, 3); return 0; }

	static uint32_t __cdecl SparkStart16(uint32_t a1)
	{
		cx::CopyAnchorFromSource(a1, a1 + 0x1C);
		SparkStart(a1, 4);
		return 0;
	}

	static uint32_t __cdecl Delay(uint32_t a1)
	{
		U16(a1, 0x68) = (uint16_t)(U16(a1, 0x68) - 1);
		if (S16(a1, 0x68) <= 0)
		{
			U8(a1, 0x26) &= 0xFB;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	static uint32_t __cdecl SparkMove(uint32_t a1)
	{
		U16(a1, 0x58) = (uint16_t)(U16(a1, 0x58) + U16(a1, 0x60));
		U16(a1, 0x5A) = (uint16_t)(U16(a1, 0x5A) + U16(a1, 0x62));
		U16(a1, 0x5C) = (uint16_t)(U16(a1, 0x5C) + U16(a1, 0x64));
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + U16(a1, 0x58));
		U16(a1, 0x1E) = (uint16_t)(U16(a1, 0x1E) + U16(a1, 0x5A));
		U16(a1, 0x20) = (uint16_t)(U16(a1, 0x20) + U16(a1, 0x5C));
		if (FrameStep(a1))
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	static uint32_t __cdecl FramePlay(uint32_t a1)
	{
		if (FrameStep(a1))
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	static uint32_t __cdecl RingStart(uint32_t a1)
	{
		S32(a1, 0x38) = 0x100;
		S32(a1, 0x34) = 0x100;
		S32(a1, 0x30) = 0x100;
		U32(a1, 0x4C) = game_addr(MODEL_Ring);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}
	static uint32_t __cdecl LandRingStart(uint32_t a1)
	{
		cx::CopyAnchorFromSource(a1, a1 + 0x1C);
		return RingStart(a1);
	}

	static void RingGrow(uint32_t a1, int32_t step)
	{
		const int16_t fade = FadeAt((int16_t)(U16(a1, 0x24) - 3));
		S32(a1, 0x30) = add32(S32(a1, 0x30), step);
		U16(a1, 0x50) = (uint16_t)fade;
		S32(a1, 0x34) = add32(S32(a1, 0x34), step);
		S32(a1, 0x38) = add32(S32(a1, 0x38), step);
		if (fade >= 0x1000)
		{
			U8(a1, 0x26) |= 5;
			U16(a1, 0x50) = 0x1000;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
	}
	static uint32_t __cdecl RingGrow300(uint32_t a1) { RingGrow(a1, 0x300); return 0; }
	static uint32_t __cdecl RingGrow200(uint32_t a1) { RingGrow(a1, 0x200); return 0; }

	static void WaveStart(uint32_t a1, uint32_t model)
	{
		U16(a1, 0x1E) = (uint16_t)(U16(a1, 0x1E) + 0xFD00);
		S32(a1, 0x38) = 0x1000;
		S32(a1, 0x34) = 0x1000;
		S32(a1, 0x30) = 0x1000;
		U32(a1, 0x4C) = model;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
	}
	static uint32_t __cdecl WaveAStart(uint32_t a1) { WaveStart(a1, game_addr(MODEL_WaveA)); return 0; }
	static uint32_t __cdecl WaveBStart(uint32_t a1) { WaveStart(a1, game_addr(MODEL_WaveB)); return 0; }

	static uint32_t __cdecl WaveAGrow(uint32_t a1)
	{
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0x200);
		const int16_t fade = S16(a1, 0x50);
		S32(a1, 0x30) = add32(S32(a1, 0x30), 0x400);
		S32(a1, 0x34) = add32(S32(a1, 0x34), 0x400);
		S32(a1, 0x38) = add32(S32(a1, 0x38), 0x400);
		if (fade >= 0x1000)
		{
			U8(a1, 0x26) |= 5;
			U16(a1, 0x50) = 0x1000;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	static uint32_t __cdecl WaveBShrink(uint32_t a1)
	{
		const int32_t sx = sub32(S32(a1, 0x30), 0x200);
		S32(a1, 0x34) = sub32(S32(a1, 0x34), 0x200);
		S32(a1, 0x38) = sub32(S32(a1, 0x38), 0x200);
		S32(a1, 0x30) = sx;
		if (sx <= 0)
		{
			U8(a1, 0x26) |= 5;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			S32(a1, 0x38) = 0;
			S32(a1, 0x34) = 0;
			S32(a1, 0x30) = 0;
		}
		return 0;
	}

	static void DustOffset(uint32_t node, uint32_t r)
	{
		int32_t b = (int32_t)r;
		if ((int16_t)b == 0) b = 1;
		const int32_t yaw = (int32_t)(x::CrtRand() & 0xFFF);
		const int32_t roll = (int32_t)(x::CrtRand() & 0xFFF);
		Mat4x3 m = {};
		x::MAG_022_sub_8DD770(P(&m));
		x::MAG_022_sub_8DD8A0(P(&m), (uint32_t)(int32_t)(int16_t)yaw);
		x::sub_8DD7E0(P(&m), (uint32_t)(int32_t)(int16_t)roll);
		int16_t v[4] = { 0, 0, 0, 0 };
		const int32_t q = (int32_t)x::CrtRand();
		v[2] = (int16_t)(q % (int32_t)(int16_t)b);
		int16_t w[4] = { 0, 0, 0, 0 };
		x::matrixMultiplyVector(P(&m), P(v), P(w));
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + (uint16_t)w[0]);
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + (uint16_t)w[1]);
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + (uint16_t)w[2]);
	}

	static uint32_t __cdecl DustStart(uint32_t a1)
	{
		cx::CopyAnchorFromSource(a1, a1 + 0x1C);
		U16(a1, 0x1E) = (uint16_t)(U16(a1, 0x1E) + 0xFD00);
		DustOffset(a1, 0x180);
		U8(a1, 0x26) |= 4;
		U32(a1, 0x4C) = game_addr(SEQ_Dust);
		U16(a1, 0x52) = 7;
		U16(a1, 0x54) = 0xFE00;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl WalkDustStart(uint32_t a1)
	{
		cx::CopyAnchorFromSource(a1, a1 + 0x1C);
		U32(a1, 0x4C) = game_addr(SEQ_Spark);
		U16(a1, 0x52) = 0xF;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static const StateFn T_BurstRing[] = { RingStart, RingGrow300, Nop };
	static const StateFn T_Spark[] = { SparkStart8, Delay, SparkMove, Nop };
	static const StateFn T_WaveA[] = { WaveAStart, WaveAGrow, Nop };
	static const StateFn T_WaveB[] = { WaveBStart, WaveBShrink, Nop };
	static const StateFn T_Dust[] = { DustStart, Delay, FramePlay, Nop };
	static const StateFn T_LandRing[] = { LandRingStart, RingGrow200, Nop };
	static const StateFn T_LandSpark[] = { SparkStart16, Delay, SparkMove, Nop };
	static const StateFn T_WalkDust[] = { WalkDustStart, FramePlay, Nop };

	static inline uint32_t PrimTask(uint32_t node, const StateFn *t)
	{
		t[S8(node, 0x29)](node);
		PrimDraw(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}
	static inline uint32_t SpriteTask(uint32_t node, const StateFn *t)
	{
		t[S8(node, 0x29)](node);
		SpriteDraw(node, U16(node, 0x6E));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}
	static uint32_t __cdecl BurstRing(uint32_t a1) { return PrimTask(a1, T_BurstRing); }
	static uint32_t __cdecl WaveA(uint32_t a1) { return PrimTask(a1, T_WaveA); }
	static uint32_t __cdecl WaveB(uint32_t a1) { return PrimTask(a1, T_WaveB); }
	static uint32_t __cdecl LandRing(uint32_t a1) { return PrimTask(a1, T_LandRing); }
	static uint32_t __cdecl Spark(uint32_t a1) { return SpriteTask(a1, T_Spark); }
	static uint32_t __cdecl Dust(uint32_t a1) { return SpriteTask(a1, T_Dust); }
	static uint32_t __cdecl LandSpark(uint32_t a1) { return SpriteTask(a1, T_LandSpark); }
	static uint32_t __cdecl WalkDust(uint32_t a1) { return SpriteTask(a1, T_WalkDust); }

	static bool FlashStep(uint32_t node)
	{
		const uint16_t idx = U16(node, 0x38);
		const uint32_t script = U32(node, 0x30);
		const uint32_t entry = U32(script, (int32_t)(int16_t)idx * 4);
		const uint16_t hi = (uint16_t)(entry >> 16);
		if ((hi >> 8) == 0xFF)
		{
			U8(node, 0x26) |= 1;
			return false;
		}
		if ((hi >> 8) == 0xFE)
		{
			if (S16(node, 0x24) >= (int16_t)(hi & 0xFF))
			{
				const uint16_t next = (uint16_t)(idx + 1);
				U16(node, 0x38) = next;
				U32(node, 0x34) = U32(script, (int32_t)(int16_t)next * 4);
			}
		}
		else
		{
			U32(node, 0x34) = entry;
			U16(node, 0x38) = (uint16_t)(idx + 1);
		}
		return true;
	}

	static uint32_t __cdecl Flash(uint32_t a1)
	{
		const uint32_t node = a1;
		if (FlashStep(node))
		{
			FlashDraw(U32(node, 0x34));
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl ChargeStart(uint32_t a1)
	{
		const uint32_t e = EntityOf(a1);
		cx::QueueChainTransformation(e, 6);
		const uint32_t p0 = U32(e, 0x1C), p1 = U32(e, 0x20);
		U32(a1, 0x50) = p0;
		U32(a1, 0x54) = p1;
		const uint16_t w0 = U16(e, 0);
		U32(a1, 0x48) = p0;
		U16(e, 0) = (uint16_t)(w0 | 0x1000);
		const int16_t face = S16(e, 0xE);
		U32(a1, 0x4C) = p1;
		U32(a1, 0x58) = w0;
		U16(a1, 0x42) = (uint16_t)face;
		const int32_t s = (int32_t)x::computeSin((uint32_t)(int32_t)face);
		U16(a1, 0x30) = (uint16_t)(shl32(s, 8) / 4096);
		const int32_t c = (int32_t)x::computeCosine((uint32_t)(int32_t)S16(a1, 0x42));
		U16(a1, 0x34) = (uint16_t)(shl32(c, 8) / 4096);
		switch (U8(e, 4))
		{
		case 0:
			U16(a1, 0x32) = 0xFEB0;
			U16(a1, 0x3A) = 0x30;
			U16(a1, 0x5E) = 0xC;
			break;
		case 1:
			U16(a1, 0x32) = 0xFE80;
			U16(a1, 0x3A) = 0x30;
			U16(a1, 0x5E) = 0xE;
			break;
		default:
			U16(a1, 0x32) = 0xFE40;
			U16(a1, 0x3A) = 0x30;
			U16(a1, 0x5E) = 0x10;
			break;
		}
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static void LandSparkSpawn(uint32_t node, int16_t delay, uint16_t angle)
	{
		const uint32_t s = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_SPRITE), game_addr(ORIG_LandSpark), 0x70, node);
		U16(s, 0x68) = (uint16_t)delay;
		const uint32_t r = x::CrtRand() & 0x3FF;
		U16(s, 0x6C) = 0xC0;
		U16(s, 0x6E) = (uint16_t)(r + 0x600);
		U16(s, 0x6A) = angle;
		const uint32_t r2 = x::CrtRand() & 0x1FF;
		U16(s, 0x6A) = (uint16_t)(((uint16_t)(r2 + U16(s, 0x6A))) & 0xFFF);
	}

	static uint32_t __cdecl ChargeFly(uint32_t a1)
	{
		const uint32_t e = EntityOf(a1);
		const int32_t q = (int32_t)0xFFFFF000 / (int32_t)S16(a1, 0x5E);
		U16(a1, 0x40) = (uint16_t)(U16(a1, 0x40) + (uint16_t)q);
		const int16_t spin = S16(a1, 0x40);
		if (spin <= (int16_t)0xF000)
			U16(e, 0xC) = 0;
		else
			U16(e, 0xC) = (uint16_t)(spin & 0xFFF);
		U16(a1, 0x32) = (uint16_t)(U16(a1, 0x32) + U16(a1, 0x3A));
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + U16(a1, 0x30));
		U16(a1, 0x1E) = (uint16_t)(U16(a1, 0x1E) + U16(a1, 0x32));
		U16(a1, 0x20) = (uint16_t)(U16(a1, 0x20) + U16(a1, 0x34));
		const int16_t ground = S16(e, 0x24);
		if (S16(a1, 0x1E) >= ground)
		{
			U16(a1, 0x1E) = (uint16_t)ground;
			cx::QueueChainTransformation(e, 6);
			x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PRIM), game_addr(ORIG_LandRing), 0x6C, a1);
			const uint32_t base = x::CrtRand() & 0xFFF;
			LandSparkSpawn(a1, 0, (uint16_t)base);
			LandSparkSpawn(a1, 2, (uint16_t)(base + 0x500));
			LandSparkSpawn(a1, 4, (uint16_t)(base + 0xA00));
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		Mat4x3 m = {};
		x::ComposeZYXRotationMatrix(e + 0xC, P(&m));
		int16_t v[4] = { 0, 0x200, 0, 0 };
		int16_t w[4] = { 0, 0, 0, 0 };
		x::matrixMultiplyVector(P(&m), P(v), P(w));
		U16(e, 0x1C) = (uint16_t)(U16(a1, 0x1C) + (uint16_t)w[0]);
		U16(e, 0x1E) = (uint16_t)((uint16_t)(U16(a1, 0x1E) + (uint16_t)w[1]) - 0x200);
		U16(e, 0x20) = (uint16_t)(U16(a1, 0x20) + (uint16_t)w[2]);
		return 0;
	}

	static uint32_t __cdecl ChargeTurn(uint32_t a1)
	{
		const uint32_t e = EntityOf(a1);
		if (U8(U32(e, 0x74), 0) == 6)
			return 0;
		cx::QueueChainTransformation(e, 0x1A);
		U16(e, 0xE) = (uint16_t)((U16(e, 0xE) - 0x800) & 0xFFF);
		U16(a1, 0x5C) = 0x10;
		U16(a1, 0x30) = (uint16_t)((S16(a1, 0x48) - S16(e, 0x1C)) / 16);
		U16(a1, 0x34) = (uint16_t)((S16(a1, 0x4C) - S16(e, 0x20)) / 16);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl ChargeWalk(uint32_t a1)
	{
		const uint32_t e = EntityOf(a1);
		U16(e, 0x1C) = (uint16_t)(U16(e, 0x1C) + U16(a1, 0x30));
		U16(e, 0x20) = (uint16_t)(U16(e, 0x20) + U16(a1, 0x34));
		U16(a1, 0x5C) = (uint16_t)(U16(a1, 0x5C) - 1);
		if (S16(a1, 0x5C) <= 0)
		{
			const uint16_t bit = (uint16_t)((U16(a1, 0x58) ^ U16(e, 0)) & 0x1000);
			U16(e, 0) = (uint16_t)(U16(e, 0) ^ bit);
			cx::QueueChainTransformation(e, U8(U32(e, 0x74), 2));
			U16(e, 0xE) = (uint16_t)((U16(e, 0xE) - 0x800) & 0xFFF);
			U16(e, 0x1C) = U16(a1, 0x48);
			U16(e, 0x20) = U16(a1, 0x4C);
			const uint32_t actions = U32(U32(a1, 0xC), 4);
			const uint32_t targets = U32(actions + 20 * (int32_t)S8(a1, 0x2A), 8);
			x::ApplyActionResultToTarget(targets + 24 * (int32_t)S8(a1, 0x2B));
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		if ((U8(a1, 0x24) & 3) == 0)
		{
			const uint32_t s = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_SPRITE), game_addr(ORIG_WalkDust), 0x70, a1);
			U16(s, 0x6E) = (uint16_t)((x::CrtRand() & 0x1FF) + 0x400);
		}
		return 0;
	}

	static const StateFn T_Charge[] = { ChargeStart, ChargeFly, ChargeTurn, ChargeWalk, Nop };

	static uint32_t __cdecl Charge(uint32_t a1)
	{
		T_Charge[S8(a1, 0x29)](a1);
		const uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		return task_end(a1, status);
	}

	static void Burst(uint32_t node, int16_t bone, int16_t flag, uint16_t stale)
	{
		int16_t pos[4];
		pos[3] = (int16_t)stale;
		const uint32_t caster = game_addr(game::BattleEntitySlotData) + 0x9C * (uint32_t)U8(U32(node, 0xC), 0);
		x::GetEffectSpawnPosition(caster, (uint32_t)(int32_t)bone, 0x1800, P(pos));
		pos[1] = 0;
		if (flag == 1)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PRIM), game_addr(ORIG_BurstRing), 0x6C, node);
			U32(t, 0x1C) = U32(P(pos), 0);
			U32(t, 0x20) = U32(P(pos), 4);
		}
		uint32_t angle = x::CrtRand() & 0xFFF;
		for (int i = 0; i < 8; i++)
		{
			const uint32_t s = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_SPRITE), game_addr(ORIG_Spark), 0x70, node);
			U32(s, 0x1C) = U32(P(pos), 0);
			U32(s, 0x20) = U32(P(pos), 4);
			U16(s, 0x68) = (uint16_t)(x::CrtRand() & 3);
			const uint32_t r = x::CrtRand() & 0x3FF;
			U16(s, 0x6C) = 0x100;
			U16(s, 0x6A) = (uint16_t)angle;
			U16(s, 0x6E) = (uint16_t)(r + 0x600);
			const uint32_t r2 = x::CrtRand() & 0xFF;
			angle += 0x200;
			U16(s, 0x6A) = (uint16_t)(((uint32_t)(uint16_t)(r2 + U16(s, 0x6A)) - 0x80) & 0xFFF);
		}
	}

	static uint32_t __cdecl EmitterStart(uint32_t a1)
	{
		cx::CopyAnchorFromSource(a1, a1 + 0x1C);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl EmitterSpawn(uint32_t a1)
	{
		if (S16(a1, 0x24) < 4)
			return 0;
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PRIM), game_addr(ORIG_WaveA), 0x6C, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PRIM), game_addr(ORIG_WaveB), 0x6C, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_CHARGE), game_addr(ORIG_Charge), 0x60, a1);
		for (int i = 0; i < 4; i++)
		{
			const uint32_t d = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_SPRITE), game_addr(ORIG_Dust), 0x70, a1);
			U16(d, 0x68) = (uint16_t)(i * 2);
			U16(d, 0x6E) = (uint16_t)((x::CrtRand() & 0x3FF) + 0x600);
		}
		if (U8(a1, 0x2B) == 0)
		{
			const uint32_t f = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_FLASH), game_addr(ORIG_Flash), 0x3C, a1);
			U32(f, 0x30) = game_addr(FLASH_SCRIPT);
		}
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl EmitterWait(uint32_t a1)
	{
		if (S16(a1, 0x24) > 0x1E)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	static const StateFn T_Emitter[] = { EmitterStart, EmitterSpawn, EmitterWait, Nop };

	static uint32_t __cdecl Emitter(uint32_t a1)
	{
		const uint32_t node = a1;
		x::MAG_001_CURE_Emitter_UpdatePos(node);
		x::MAG_001_CURE_Emitter_ComputeModelBounds(node);
		T_Emitter[S8(node, 0x29)](node);
		if (U8(node, 0x2B) == 0)
		{
			if (U16(node, 0x24) == 4)
				x::BdPlaySE(game_addr(SOUND_Clash), 0, 0x80);
			if (U8(node, 0x2B) == 0)
			{
				const uint16_t c = U16(node, 0x24);
				const uint16_t queue_hi = (uint16_t)(game_addr(Q_EMITTER) >> 16);
				if (c == 4 || c == 0x28)
				{
					Burst(node, 0x34, 1, 0);
					Burst(node, 0x35, 1, queue_hi);
					Burst(node, 0x36, 1, queue_hi);
					Burst(node, 0x33, 1, queue_hi);
				}
				else if (c == 0x11)
					Burst(node, 0x36, 0, 0);
				else if (c == 0x14)
					Burst(node, 0x34, 0, 0);
			}
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl NextState(uint32_t a1)
	{
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	static uint32_t __cdecl SpawnEmitters(uint32_t a1)
	{
		for (int32_t i = 0; i < (int32_t)S16(a1, 0x5A); i++)
		{
			x::Effect_AddTaskAndInitFromCtx(game_addr(Q_EMITTER), game_addr(ORIG_Emitter), 0x58, a1);
			U8(a1, 0x2B) = (uint8_t)(U8(a1, 0x2B) + 1);
		}
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl WaitQueues(uint32_t a1)
	{
		if (U16(a1, 0x5E) == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return a1;
	}

	static const StateFn T_Master[] = { NextState, SpawnEmitters, NextState, WaitQueues, Nop };

	static uint32_t __cdecl Master(uint32_t a1)
	{
		const uint32_t node = a1;
		if (U8(node, 0x5C) & 1)
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = MEM<uint32_t>(game_addr(ARENA_ODD));
		else
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = MEM<uint32_t>(game_addr(ARENA_EVEN));
		x::Effect_UpdateTargetPosFromBones(node);
		T_Master[S8(node, 0x29)](node);
		U16(node, 0x5E) = (uint16_t)x::ExecuteTaskQueue(game_addr(Q_EMITTER));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_CHARGE)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_SPRITE)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_PRIM)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_FLASH)));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}
}
}

	void register_mag026_clash()
	{
		register_port(act::clash026::ORIG_Master, (void *)act::clash026::Master, "026 Clash Master", 26);
		register_port(act::clash026::ORIG_Emitter, (void *)act::clash026::Emitter, "026 Emitter", 26);
		register_port(act::clash026::ORIG_Charge, (void *)act::clash026::Charge, "026 KnockBack", 26);
		register_port(act::clash026::ORIG_BurstRing, (void *)act::clash026::BurstRing, "026 BurstRing", 26);
		register_port(act::clash026::ORIG_WaveA, (void *)act::clash026::WaveA, "026 WaveA", 26);
		register_port(act::clash026::ORIG_WaveB, (void *)act::clash026::WaveB, "026 WaveB", 26);
		register_port(act::clash026::ORIG_LandRing, (void *)act::clash026::LandRing, "026 LandRing", 26);
		register_port(act::clash026::ORIG_Spark, (void *)act::clash026::Spark, "026 Spark", 26);
		register_port(act::clash026::ORIG_Dust, (void *)act::clash026::Dust, "026 Dust", 26);
		register_port(act::clash026::ORIG_LandSpark, (void *)act::clash026::LandSpark, "026 LandSpark", 26);
		register_port(act::clash026::ORIG_WalkDust, (void *)act::clash026::WalkDust, "026 WalkDust", 26);
		register_port(act::clash026::ORIG_Flash, (void *)act::clash026::Flash, "026 Flash", 26);
	}
}

