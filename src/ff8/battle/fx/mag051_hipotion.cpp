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

// Effect 51: Hi-Potion

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
namespace hipotion
{
	static const Mod MOD_051 = { "hipotion", 51, game::MAG_051_HIPOTION_FL, game::MAG_050_POTION_POTION_FL,
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_051_HIPOTION_EmitterQueue, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_051_HIPOTION_EmitterTask, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_051_HIPOTION_StepCounterLimit, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::MAG_051_HIPOTION_PacketCursor;
	static constexpr game::Id FRAME_CURSOR = game::BATTLE_PACKET_WRITE_CURSOR;
	static constexpr game::Id Q_EMITTER = game::MAG_051_HIPOTION_EmitterQueue;
	static constexpr game::Id Q_PLAYER = game::MAG_051_HIPOTION_PlayerQueue;
	static constexpr game::Id Q_SPRITE = game::MAG_051_HIPOTION_SpriteQueue;
	static constexpr game::Id ARENA_EVEN = game::MAG_051_HIPOTION_ArenaEven, ARENA_ODD = game::MAG_051_HIPOTION_ArenaOdd;
	static constexpr game::Id BLEND_BUFFER = game::MAG_051_HIPOTION_BlendBufferPtr;
	static constexpr game::Id OVL_STATE = game::MAG_051_HIPOTION_OverlayState;
	static constexpr game::Id OVL_LIGHT = game::MAG_051_HIPOTION_OverlayLight;
	static constexpr game::Id OVL_FIRST = game::MAG_051_HIPOTION_OverlayFirstWord;
	static constexpr game::Id LIT_VERTICES = game::MAG_051_HIPOTION_OverlayLitVertices;
	static constexpr game::Id LIGHT_WORDS = game::unk_1D98992;
	static constexpr game::Id TAB_RINGS = game::MAG_051_HIPOTION_TabRings, TAB_SPARKLES = game::MAG_051_HIPOTION_TabSparkles, TAB_PARTICLES = game::MAG_051_HIPOTION_TabParticles;
	static constexpr game::Id LAYOUT_Aura = game::MAG_051_HIPOTION_LayoutAura, LAYOUT_Ring = game::MAG_051_HIPOTION_LayoutRing;
	static constexpr game::Id SEQ_Glow = game::MAG_051_HIPOTION_SeqGlow, SEQ_Swirl = game::MAG_051_HIPOTION_SeqSwirl, SEQ_SwirlEnd = game::MAG_051_HIPOTION_SeqSwirlEnd, SEQ_Mote = game::MAG_051_HIPOTION_SeqMote;
	static constexpr game::Id SEQ_SparkleA = game::MAG_051_HIPOTION_SeqSparkleA, SEQ_SparkleA2 = game::MAG_051_HIPOTION_SeqSparkleA2, SEQ_SparkleB = game::MAG_051_HIPOTION_SeqSparkleB, SEQ_SparkleB2 = game::MAG_051_HIPOTION_SeqSparkleB2;
	static constexpr game::Id SOUND_HiPotion = game::MAG_051_HIPOTION_SoundCtx;

	static constexpr game::Id ORIG_Master = game::MAG_051_HIPOTION_Tick;
	static constexpr game::Id ORIG_Emitter = game::MAG_051_HIPOTION_EmitterTask;
	static constexpr game::Id ORIG_Glow = game::MAG_051_HIPOTION_Glow_Tick;
	static constexpr game::Id ORIG_Swirl = game::MAG_051_HIPOTION_Swirl_Tick;
	static constexpr game::Id ORIG_Mote = game::MAG_051_HIPOTION_Mote_Tick;
	static constexpr game::Id ORIG_Aura = game::MAG_051_HIPOTION_AuraTask;
	static constexpr game::Id ORIG_LightTask = game::MAG_051_HIPOTION_LightTask;
	static constexpr game::Id ORIG_RingSpawner = game::MAG_051_HIPOTION_RingSpawner;
	static constexpr game::Id ORIG_Ring = game::MAG_051_HIPOTION_Ring_Tick;
	static constexpr game::Id ORIG_SparkleSpawner = game::MAG_051_HIPOTION_SparkleSpawner;
	static constexpr game::Id ORIG_Sparkle = game::MAG_051_HIPOTION_Sparkle_Tick;
	static constexpr game::Id ORIG_Overlay = game::MAG_051_HIPOTION_Overlay_Tick;
	static constexpr game::Id ORIG_ParticleEmitter = game::MAG_051_HIPOTION_ParticleEmitter_Tick;
	static constexpr game::Id ORIG_Particle = game::MAG_051_HIPOTION_Particle_Tick;
	static constexpr game::Id ORIG_DrawCallback = game::MAG_051_HIPOTION_PrimDrawCallback;

	namespace cx
	{
		inline uint32_t CopyAnchorFromSource(uint32_t node, uint32_t out) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::Effect_CopyAnchorXZFromSource))(node, out); }
		inline uint32_t CopyMidpointFromSource(uint32_t node, uint32_t out) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::Effect_CopyMidpointFromSource))(node, out); }
		inline uint32_t BoundsBaseY(uint32_t node) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::sub_8DCAF0))(node); }
		inline uint32_t BuildMatrixFromDirAndUp(uint32_t out, uint32_t dir, uint32_t up) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::Effect_BuildMatrixFromDirAndUp))(out, dir, up); }
		inline uint32_t NormalizeVec3(uint32_t in, uint32_t out) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::Math_NormalizeVec3_Q12))(in, out); }
		inline uint32_t TransformCameraByShadowRotation(uint32_t pos, uint32_t a, uint32_t b) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::TransformCameraByShadowRotation))(pos, a, b); }
		inline uint32_t InitEffectSequenceFromData(uint32_t h, uint32_t ot, uint32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::InitEffectSequenceFromData))(h, ot, mode, cursor); }
		inline uint32_t BattleReadAnimation(uint32_t hdr, uint32_t cmd) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::Battle_ReadAnimation))(hdr, cmd); }
		inline uint32_t PreBattleReadAnimation(uint32_t hdr, uint32_t cmd, uint32_t anim) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::pre_Battle_ReadAnimation))(hdr, cmd, anim); }
		inline uint32_t GteSetLightMatrix(uint32_t m) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_SetLightMatrix))(m); }
		inline uint32_t GteSetBackgroundVector(uint32_t r, uint32_t g, uint32_t b) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetBackgroundVector))(r, g, b); }
		inline uint32_t GteMVMVA_LightV0Bk() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
		inline uint32_t GteStoreSXY012_PolyGT3(uint32_t p) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreSXY012_PolyGT3))(p); }
		inline uint32_t GteStoreSXY012_PolyFT3(uint32_t p) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreSXY012_PolyFT3))(p); }
		inline uint32_t GteStoreSXY012_PolyFT3_2(uint32_t p) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreSXY012_PolyFT3_2))(p); }
		inline uint32_t GteReadSXY012(uint32_t p) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadSXY012))(p); }
		inline uint32_t GteLoadIR123(uint32_t v) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadIR123_u16))(v); }
		inline uint32_t GteGPF() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPF))(); }
		inline uint32_t GteGPL() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPL))(); }
	}

	static inline uint32_t task_end(uint32_t node, uint8_t status)
	{
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	static inline uint32_t RenderOT44() { return MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	static inline uint32_t ArgSlotColour(uint32_t node, uint8_t c)
	{
		return (node & 0xFF000000u) | ((uint32_t)c << 16) | ((uint32_t)c << 8) | c;
	}

	static inline int32_t Div18(int32_t x)
	{
		int32_t hi = (int32_t)(((int64_t)x * 0x38E38E39) >> 32) >> 2;
		return hi + (int32_t)((uint32_t)hi >> 31);
	}
	static inline int32_t Div20(int32_t x)
	{
		int32_t hi = (int32_t)(((int64_t)x * 0x66666667) >> 32) >> 3;
		return hi + (int32_t)((uint32_t)hi >> 31);
	}

	static const uint32_t FR_FLAGS = 0x00, FR_BLOCK = 0x04, FR_MATRIX = 0x18, FR_SIZE = 0x38;
}
}
}

namespace ff8fx
{
namespace act
{
namespace hipotion
{
	static void DrawSprite(uint32_t node)
	{
		if (U8(node, 0x26) & 4)
			return;
		const uint32_t h = x::Field_Alloc(0xB4);
		cx::TransformCameraByShadowRotation(node + 0x1C, 0x1000, (uint32_t)(int32_t)S16(node, 0x54));
		U32(h, 0) = U32(node, 0x4C);
		U16(h, 4) = U16(node, 0x50);
		U16(h, 0x24) = 0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = cx::InitEffectSequenceFromData(h, RenderOT44(), 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0xB4);
	}

	static void DrawGroundSprite(uint32_t node, uint32_t seq, uint32_t colour)
	{
		if (U8(node, 0x26) & 4)
			return;
		Mat4x3 m = {};
		x::MAG_022_sub_8DD770(P(&m));
		x::sub_8DD7E0(P(&m), 0x400);
		m.t[0] = S16(node, 0x1C);
		m.t[1] = 0;
		m.t[2] = S16(node, 0x20);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), P(&m), P(&m));
		x::GTE_SetRotMatrix_W(P(&m));
		x::GTE_SetTransVector_W(P(&m));
		const uint32_t h = x::Field_Alloc(0xB4);
		U32(h, 0) = seq;
		U16(h, 4) = U16(node, 0x50);
		U16(h, 0x24) = 4;
		U32(h, 0x1C) = U32(colour, 0);
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = cx::InitEffectSequenceFromData(h, RenderOT44(), 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0xB4);
	}

	static void DrawSpinSprite(uint32_t node, uint32_t seq, uint32_t colour, uint32_t angle, uint32_t scale)
	{
		if (U8(node, 0x26) & 4)
			return;
		const int32_t s = (int16_t)scale;
		int32_t sv[3] = { s, s, s };
		Mat4x3 m = {};
		x::MAG_022_sub_8DD770(P(&m));
		x::MAG_022_sub_8DD8A0(P(&m), (uint32_t)(int32_t)(int16_t)angle);
		x::sub_8DD7E0(P(&m), 0x400);
		x::scale3DMatrix(P(&m), P(sv));
		m.t[0] = S16(node, 0x1C);
		m.t[1] = 0;
		m.t[2] = S16(node, 0x20);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), P(&m), P(&m));
		x::GTE_SetRotMatrix_W(P(&m));
		x::GTE_SetTransVector_W(P(&m));
		const uint32_t h = x::Field_Alloc(0xB4);
		U32(h, 0) = seq;
		U16(h, 4) = U16(node, 0x50);
		U16(h, 0x24) = 4;
		U32(h, 0x1C) = U32(colour, 0);
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = cx::InitEffectSequenceFromData(h, RenderOT44(), 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0xB4);
	}

	static void RandomOffset(uint32_t node, uint32_t n)
	{
		int32_t d = (int16_t)n;
		if ((uint16_t)n == 0)
			d = 1;
		const int32_t yaw = x::CrtRand() & 0xFFF;
		const int32_t pitch = x::CrtRand() & 0xFFF;
		Mat4x3 m = {};
		x::MAG_022_sub_8DD770(P(&m));
		x::MAG_022_sub_8DD8A0(P(&m), (uint32_t)(int32_t)(int16_t)yaw);
		x::sub_8DD7E0(P(&m), (uint32_t)(int32_t)(int16_t)pitch);
		int16_t v[4] = {};
		const int32_t r = (int32_t)x::CrtRand();
		v[2] = (int16_t)(r % d);
		int16_t out[4] = {};
		x::matrixMultiplyVector(P(&m), P(v), P(out));
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + (uint16_t)out[0]);
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + (uint16_t)out[1]);
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + (uint16_t)out[2]);
	}

	static void Memset32(uint32_t dst, uint32_t value, uint32_t count)
	{
		for (uint32_t i = 0; i < count; i++)
			U32(dst, (int32_t)(i * 4)) = value;
	}

	static void DecodeLayout(uint32_t data, uint32_t out, int32_t size)
	{
		Memset32(out, 0, (uint32_t)(size >> 2));
		U32(out, 0) = data;
		uint32_t c = out + 8;
		const uint32_t tab = data + U32(data, 4);
		int32_t n = S32(tab, 0);
		uint32_t p = tab + 8;
		for (; n > 0; n--)
		{
			const uint32_t off = U32(p, 0);
			p += 4;
			const uint32_t f = U32(tab + off, 4);
			if (f & 4) c += 0x24;
			else if (f & 2) c += 0x18;
			else if (f & 1) c += 0xC;
			if (f & 0x20) c += 0x24;
			else if (f & 0x10) c += 0x18;
			else if (f & 8) c += 0xC;
			if (f & 0x1C0)
			{
				U32(c, 8) = 0x10000000;
				U32(c, 4) = 0x10000000;
				U32(c, 0) = 0x10000000;
			}
			if (f & 0x100) c += 0x24;
			else if (f & 0x80) c += 0x18;
			else if (f & 0x40) c += 0xC;
			if (f & 0x800) c += 0x18;
			else if (f & 0x400) c += 0x10;
			else if (f & 0x200) c += 8;
			if (f & 0x1000) c += 0xC;
			if (f & 0x2000) c += 0x10;
		}
	}

	static void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out)
	{
		const int32_t w0 = 0x1000 - t;
		uint32_t a = f0 == 0 ? model + 0xC : model + (uint32_t)mul32(S32(model, 4), f0) * 8 + 0xC;
		uint32_t b = f1 == 0 ? model + 0xC : model + (uint32_t)mul32(S32(model, 4), f1) * 8 + 0xC;
		uint32_t n = U32(model, 4);
		if (n == 0)
			return;
		for (; n != 0; n--)
		{
			x::set_dword_1CA8A30((uint32_t)w0);
			cx::GteLoadIR123(a);
			cx::GteGPF();
			x::set_dword_1CA8A30((uint32_t)t);
			cx::GteLoadIR123(b);
			cx::GteGPL();
			x::GTE_StoreIR123(out);
			out += 8;
			a += 8;
			b += 8;
		}
	}

	static void ScrollModelV(uint32_t model, int32_t v)
	{
		const int32_t q = S32(model, 0) / 4;
		uint32_t c = model + (uint32_t)q * 4 + 0x1C;
		const uint32_t sh = (uint32_t)shl32(v, 8);
		int32_t count = S32(c, 0);
		c += 4;
		if (count <= 0)
			return;
		c += 0x10;
		for (; count != 0; count--, c += 0x24)
		{
			const uint32_t w1 = U32(c, 0), w0 = U32(c, -4), w2 = U32(c, 4);
			uint32_t e0 = (w0 & 0xFF00) + sh;
			uint32_t e1 = (w1 & 0xFF00) + sh;
			uint32_t e2 = (w2 & 0xFF00) + sh;
			uint32_t e3 = ((w2 >> 24) << 8) + sh;
			if (e0 > 0xFF00 || e1 > 0xFF00 || e2 > 0xFF00 || e3 > 0xFF00)
			{
				e0 -= 0x8000;
				e1 -= 0x8000;
				e2 -= 0x8000;
				e3 -= 0x8000;
			}
			const uint32_t n0 = (w0 & 0xFFFF00FF) | (e0 & 0xFF00);
			const uint32_t n1 = (w1 & 0xFFFF00FF) | (e1 & 0xFF00);
			const uint32_t n2 = (w2 & 0x00FF00FF) | (e2 & 0xFF00) | ((e3 & 0xFFFFFF00) << 16);
			U32(c, 0) = n1;
			U32(c, -4) = n0;
			U32(c, 4) = n2;
		}
	}

	static void __cdecl DrawCallback(prim::Layout *lp, prim::Record *rp, int arg)
	{
		const uint32_t l = P(lp), rec = P(rp), blk = (uint32_t)arg;
		if (((uint32_t)(int32_t)S16(rec, 0x1C) | U32(rec, 0x18)) == 0)
			return;
		if (S16(rec, 0x24) >= 0x1000 && U32(rec, 0x20) == 0)
			return;
		const uint32_t h = x::Field_Alloc(0x58);
		const int32_t object = S16(rec, 2);
		const uint32_t data = U32(l, 0);
		const uint32_t model = data + U32(data, object * 4 + 8);
		U32(h, 0) = model;
		const int16_t f1 = S16(rec, 0x2A);
		const int16_t f0 = S16(rec, 0x28);
		auto frame_ptr = [model](int16_t f) -> uint32_t {
			if (f == 0)
				return model + 0xC;
			return model + (uint32_t)mul32(S32(model, 4), (int32_t)f) * 8 + 0xC;
		};
		if (f0 == f1)
			U32(h, 4) = frame_ptr(f0);
		else
		{
			const int16_t t = S16(rec, 0x26);
			if (t == 0)
				U32(h, 4) = frame_ptr(f0);
			else if (t == 0x1000)
				U32(h, 4) = frame_ptr(f1);
			else
			{
				BlendVertexFrames(model, f0, f1, t, U32(blk, 8));
				U32(h, 4) = U32(blk, 8);
			}
		}
		Mat4x3 mo = {};
		x::ComposeZYXRotationMatrix(rec + 0x10, P(&mo));
		mo.t[0] = S16(rec, 8);
		mo.t[1] = S16(rec, 0xA);
		mo.t[2] = S16(rec, 0xC);
		const int32_t index = S16(rec, 0);
		const uint32_t m = U32(blk, 0);
		if (U32(U32(blk, 4), index * 4) & 0x10000000)
		{
			x::TransformVectorBy3x3Matrix(m, P(mo.t), P(mo.t));
			mo.t[0] = add32(mo.t[0], S32(m, 0x14));
			mo.t[1] = add32(mo.t[1], S32(m, 0x18));
			mo.t[2] = add32(mo.t[2], S32(m, 0x1C));
		}
		else
			x::ComposeAffineTransform(m, P(&mo), P(&mo));
		if (U32(rec, 0x18) != 0x10001000 || U16(rec, 0x1C) != 0x1000)
		{
			int32_t sv[3] = { S16(rec, 0x18), S16(rec, 0x1A), S16(rec, 0x1C) };
			x::scale3DMatrix(P(&mo), P(sv));
		}
		x::GTE_SetRotMatrix_W(P(&mo));
		x::GTE_SetTransVector_W(P(&mo));
		const uint32_t fp = U32(blk, 4) + (uint32_t)(index * 4);
		U32(h, 0x1C) = (U32(fp, 0) & 0xDFFF) | 0x2000;
		const int32_t a = S16(rec, 0x24);
		S32(h, 0xC) = a;
		if (U32(fp, 0) & 0x40000000)
		{
			const int32_t base = S16(blk, 0x10);
			S32(h, 0xC) = mul32(base, 0x1000 - a) / 4096 + a;
		}
		if (S32(h, 0xC) != 0)
		{
			U32(h, 0x1C) |= 0xC0;
			U32(h, 8) = U32(rec, 0x20);
		}
		if (U32(fp, 0) & 0x20000000)
			ScrollModelV(U32(h, 0), S16(U32(blk, 0xC), index * 2));
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = x::Effect_RenderPrimModel(h, RenderOT44(), 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0x58);
	}

	static void OverlayInit(uint32_t st, uint32_t first, uint32_t light, uint32_t lit, uint32_t entity, uint32_t slot)
	{
		U32(st, 8) = entity;
		U32(st, 0) = first;
		U16(st, 0xDA) = (uint16_t)slot;
		U32(st, 4) = light;
		U32(st, 0xC) = game_addr(FRAME_CURSOR);
		U32(st, 0x10) = game_addr(PACKET_CURSOR);
		U16(st, 0xB4) = 0;
		U16(st, 0xB6) = 0;
		U16(st, 0xB8) = 0;
		U8(st, 0xC4) = 0x80;
		U8(st, 0xC5) = 0x80;
		U8(st, 0xC6) = 0x80;
		U32(st, 0xC8) = 0x2000;
		U16(st, 0xD2) = 1;
		U16(st, 0xCE) = 0;
		U16(st, 0xF2) = 0;
		U16(st, 0xA4) = 0;
		U16(st, 0xA6) = 0;
		U16(st, 0xA8) = 0;
		U16(st, 0xAC) = 0;
		U16(st, 0xAE) = 0;
		U16(st, 0xB0) = 0;
		U16(st, 0xBC) = 0;
		U16(st, 0xBE) = 0;
		U16(st, 0xC0) = 0;
		U8(light, 0x1C) = 0x80;
		U8(light, 0x1D) = 0x80;
		U8(light, 0x1E) = 0x80;
		U32(light, 4) = lit;
		U16(light, 0x14) = 0;
		U16(light, 0x16) = 0;
		U16(light, 0x18) = 0x140;
		U16(light, 0x1A) = 0;
		U32(light, 0x20) = 0xFFFFFFFF;
		U16(light, 0x24) = 0;
	}

	static void OverlayParams(uint32_t st, uint32_t pos, int32_t tx, int32_t ty, int32_t cx_, int32_t cy, int32_t w, int32_t hgt)
	{
		U16(st, 0xDC) = U16(pos, 0);
		U16(st, 0xDE) = U16(pos, 2);
		U16(st, 0xE0) = U16(pos, 4);
		const int32_t y = (int16_t)ty;
		uint32_t tp = ((uint32_t)tx & 0x3C0) | 0x2800;
		tp = (uint32_t)(uint16_t)((int16_t)(uint16_t)tp >> 6);
		tp |= (uint32_t)(y >> 4) & 0x10;
		tp |= ((uint32_t)y & 0x200) << 2;
		U16(st, 0xEC) = (uint16_t)tp;
		const uint32_t clut = (((uint32_t)(cx_ >> 4)) & 0x3F) | (uint32_t)shl32(cy, 6);
		U16(st, 0xEE) = (uint16_t)clut;
		U16(st, 0xE4) = (uint16_t)w;
		U16(st, 0xEA) = (uint16_t)(ty & 0xFF);
		U16(st, 0xE8) = (uint16_t)(((uint32_t)tx - ((uint32_t)tx & 0xFFFFFFC0u)) << 1);
		U16(st, 0xE6) = (uint16_t)hgt;
	}

	static void OverlayMatrices(uint32_t st, uint32_t entity)
	{
		ff8fx::copy<(0x20)>((void *)(st + 0x14), (const void *)(entity + 0x40));
		ff8fx::copy<(0x20)>((void *)(st + 0x54), (const void *)(entity + 0x40));
		const int32_t mode = S16(st, 0xD2);
		if (mode == 0)
		{
			int16_t out[4] = {};
			x::matrixMultiplyVector(st + 0x54, st + 0xB4, P(out));
			S32(st, 0x68) = add32(S32(st, 0x68), out[0]);
			S32(st, 0x6C) = add32(S32(st, 0x6C), out[1]);
			S32(st, 0x70) = add32(S32(st, 0x70), out[2]);
		}
		else if (mode == 1)
		{
			S32(st, 0x68) = add32(S32(st, 0x68), S16(st, 0xB4));
			S32(st, 0x6C) = add32(S32(st, 0x6C), S16(st, 0xB6));
			S32(st, 0x70) = add32(S32(st, 0x70), S16(st, 0xB8));
		}
	}

	static void OverlayLight(uint32_t st)
	{
		const uint32_t L = x::Field_Alloc(0x70);
		const uint16_t w68 = U16(st, 0x68), w6c = U16(st, 0x6C), w70 = U16(st, 0x70);
		U16(L, 0x68) = w68;
		U16(L, 0x6A) = w6c;
		U16(L, 0x6C) = w70;
		const int32_t dx = (int16_t)w68 - S16(st, 0xDC);
		const int32_t dy = (int16_t)w6c - S16(st, 0xDE);
		const int32_t dz = (int16_t)w70 - S16(st, 0xE0);
		S32(L, 4) = dy;
		S32(L, 8) = dz;
		S32(L, 0) = dx;
		const int32_t sq = add32(add32(mul32(dz, dz), mul32(dy, dy)), mul32(dx, dx));
		S32(L, 0x60) = sq;
		U32(L, 0x64) = x::Sqrt((uint32_t)sq);
		U16(L, 0x12) = 0x1000;
		U16(L, 0x10) = 0;
		U16(L, 0x14) = 0;
		cx::NormalizeVec3(L, L + 0x18);
		cx::BuildMatrixFromDirAndUp(L + 0x20, L + 0x18, L + 0x10);
		x::UnpackRotationMatrix(L + 0x20, L + 0x40);
		U32(L, 0x5C) = U32(st, 0xC8);
		ff8fx::copy<(0x20)>((void *)(L + 0x20), (const void *)(st + 0x14));
		U32(L, 0x54) = 0;
		U32(L, 0x58) = 0;
		S32(L, 0x38) = sub32(S32(st, 0x2C), S32(st, 0x6C));
		S32(L, 0x34) = sub32(S32(st, 0x28), S32(st, 0x68));
		S32(L, 0x3C) = sub32(S32(st, 0x30), S32(st, 0x70));
		x::GTE_SetRotMatrix(L + 0x40);
		x::GTE_LoadIRFromMatrixColumn(L + 0x20);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(st + 0x74);
		x::GTE_LoadIRFromMatrixColumn(L + 0x22);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(st + 0x76);
		x::GTE_LoadIRFromMatrixColumn(L + 0x24);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(st + 0x78);
		x::GTE_SetTransVector(L + 0x40);
		x::GTE_LoadV0FromDwords(L + 0x34);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(st + 0x88);
		x::Field_Free(0x70);
	}

	static void RenderModel(uint32_t part, uint32_t ot, uint32_t st)
	{
		uint32_t frame_cur = U32(U32(st, 0xC), 0);
		uint32_t mod_cur = U32(U32(st, 0x10), 0);
		const uint32_t lit = U32(U32(st, 4), 4);
		const uint32_t bones = U32(part, 0) + 0x10;
		const uint32_t objtab = U32(part, 4);
		const int32_t nobj = S32(objtab, 0);
		const uint32_t entity = U32(st, 8);
		const uint32_t h = x::Field_Alloc(0xD0);
		U16(h, 0xCE) = U16(st, 0xF4);
		U32(h, 0xB8) = U32(entity, 0x7C);
		U16(h, 0xBC) = U16(st, 0xEC);
		U16(h, 0xBE) = U16(st, 0xEE);
		const int32_t hw = S16(st, 0xE4) / 2;
		const int32_t hh = S16(st, 0xE6) / 2;
		U16(h, 0xC2) = (uint16_t)(hw + 0x9F);
		U16(h, 0xC0) = (uint16_t)(0xA0 - hw);
		U16(h, 0xC6) = (uint16_t)(hh + 0x77);
		U16(h, 0xCA) = (uint16_t)(U16(st, 0xEA) + (uint16_t)hh - 0x78);
		U16(h, 0xC4) = (uint16_t)(0x78 - hh);
		U16(h, 0xC8) = (uint16_t)(U16(st, 0xE8) + (uint16_t)hw - 0xA0);
		const uint32_t ovl = U32(st, 0xC4) & 0xFFFFFF;
		U32(h, 0xA8) = ovl | 0x36000000;
		U32(h, 0xAC) = ovl | 0x3E000000;
		const uint32_t tint = U32(entity, 0x28) & 0xFFFFFF;
		U32(h, 0xB0) = tint | 0x24000000;
		U32(h, 0xB4) = tint | 0x2C000000;
		ff8fx::copy<(0x20)>((void *)h, (const void *)(st + 0x34));
		ff8fx::copy<(0x20)>((void *)(h + 0x20), (const void *)(st + 0x74));
		U16(h, 0xA4) = U16(st, 0xCE);
		U16(h, 0xA6) = U16(st, 0xCC);
		x::GTE_SetRotMatrix(h);
		x::GTE_SetTransVector(h);
		auto vtx = [lit](uint32_t i) -> uint32_t { return lit + 4 + (i << 4); };
		uint32_t offp = objtab + 4;
		for (int32_t k = 0; k < nobj; k++)
		{
			uint32_t litc = lit;
			uint32_t obj = U32(part, 4) + U32(offp, 0);
			offp += 4;
			if (((U32(h, 0xB8) >> (k & 31)) & 1) == 0)
				continue;
			const int32_t groups = S16(obj, 0);
			obj += 2;
			for (int32_t g = groups; g > 0; g--)
			{
				const int32_t bone = S16(obj, 0);
				obj += 2;
				const uint32_t m = (uint32_t)mul32(bone, 0x30) + bones + 0x10;
				cx::GteSetLightMatrix(m);
				cx::GteSetBackgroundVector(U32(m, 0x14), U32(m, 0x18), U32(m, 0x1C));
				const int32_t nv = S16(obj, 0);
				obj += 2;
				if (nv <= 0)
					continue;
				uint32_t out = litc + 4;
				litc += (uint32_t)shl32(nv, 4);
				for (int32_t v = nv; v != 0; v--)
				{
					U16(h, 0x70) = U16(obj, 0);
					U16(h, 0x72) = U16(obj, 2);
					U16(h, 0x74) = U16(obj, 4);
					obj += 6;
					x::GTE_LoadV0(h + 0x70);
					cx::GteMVMVA_LightV0Bk();
					x::GTE_StoreIR123(out);
					out += 0x10;
				}
			}
			obj = (obj + 3) & ~3u;
			uint32_t ft = frame_cur;
			uint32_t pk = mod_cur;
			const int32_t ntri = S16(obj, 0);
			const int32_t nquad = S16(obj, 2);
			uint32_t rec = obj + 0xC;
			for (int32_t c = ntri; c > 0; c--, rec += 0x10)
			{
				uint32_t light = 0;
				U32(h, 0x9C) = U16(rec, 4) & 0xFFF;
				U32(h, 0x98) = U16(rec, 2) & 0xFFF;
				U32(h, 0x94) = U16(rec, 0) & 0xFFF;
				x::GTE_LoadV012(vtx(U32(h, 0x94)), vtx(U32(h, 0x98)), vtx(U32(h, 0x9C)));
				x::GTE_RTPT();
				const uint32_t sz1 = MEM<uint32_t>(game_addr(game::GTE_SZ1)), sz2 = MEM<uint32_t>(game_addr(game::GTE_SZ2)), sz3 = MEM<uint32_t>(game_addr(game::GTE_SZ3));
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(h + 0x8C);
				if (S32(h, 0x8C) <= 0)
					continue;
				x::GTE_AVSZ3();
				x::GTE_ReadOTZ(h + 0x90);
				U32(h, 0x90) = U32(h, 0x90) >> 2;
				cx::GteStoreSXY012_PolyGT3(pk);
				cx::GteStoreSXY012_PolyFT3(ft);
				x::GTE_SetRotMatrix(h + 0x20);
				x::GTE_SetTransVector(h + 0x20);
				x::GTE_LoadV012(vtx(U32(h, 0x94)), vtx(U32(h, 0x98)), vtx(U32(h, 0x9C)));
				x::GTE_RTPT();
				U16(h, 0xCC) = 0;
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(h + 0x8C);
				if (U16(h, 0xCE) == 1 && S32(h, 0x8C) <= 0)
					U16(h, 0xCC) = 1;
				if (U16(h, 0xCC) == 0)
				{
					cx::GteReadSXY012(h + 0x40);
					for (int i = 0; i < 3; i++)
					{
						const int16_t sx = PsxPixels(S16(h, 0x40 + 4 * i)), sy = PsxPixels(S16(h, 0x42 + 4 * i));
						if (sx < S16(h, 0xC0) || sx > S16(h, 0xC2) || sy < S16(h, 0xC4) || sy > S16(h, 0xC6))
						{
							U16(h, 0xCC) = 1;
							break;
						}
						U16(h, 0x40 + 4 * i) = (uint16_t)(U16(h, 0xC8) + (uint16_t)sx);
						U16(h, 0x42 + 4 * i) = (uint16_t)((uint16_t)sy + U16(h, 0xCA));
					}
					if (U16(h, 0xCC) == 0)
					{
						U8(pk, 0xC) = U8(h, 0x40);
						U8(pk, 0xD) = U8(h, 0x42);
						U8(pk, 0x18) = U8(h, 0x44);
						U8(pk, 0x19) = U8(h, 0x46);
						U16(pk, 0x1A) = U16(h, 0xBC);
						U8(pk, 0x24) = U8(h, 0x48);
						U8(pk, 0x25) = U8(h, 0x4A);
						U16(pk, 0xE) = U16(h, 0xBE);
						U32(pk, 0x1C) = U32(h, 0xA8);
						U32(pk, 0x10) = U32(h, 0xA8);
						U32(pk, 4) = U32(h, 0xA8);
						U32(pk, 0) = 0x9000000;
						light = pk;
						pk += 0x28;
					}
				}
				U32(ft, 0xC) = U32(rec, 8);
				U32(ft, 0x14) = U32(rec, 0xC);
				U32(ft, 0) = 0x7000000;
				U16(ft, 0x1C) = U16(rec, 6);
				U32(ft, 4) = U32(h, 0xB0);
				if (U8(rec, 0xF) & 2)
					U8(ft, 7) |= 2;
				x::SSIGPU_InsertPrimDepthKeys(ot + U32(h, 0x90) * 4, ft, sz1, sz2, sz3, 0);
				if (light) x::SSIGPU_InsertPrimDepthKeys(ot + U32(h, 0x90) * 4, light, sz1, sz2, sz3, 0);
				ft += 0x20;
				x::GTE_SetRotMatrix(h);
				x::GTE_SetTransVector(h);
			}
			for (int32_t c = nquad; c > 0; c--, rec += 0x14)
			{
				uint32_t light = 0;
				U32(h, 0x9C) = U16(rec, 4) & 0xFFF;
				U32(h, 0x98) = U16(rec, 2) & 0xFFF;
				U32(h, 0x94) = U16(rec, 0) & 0xFFF;
				x::GTE_LoadV012(vtx(U32(h, 0x94)), vtx(U32(h, 0x98)), vtx(U32(h, 0x9C)));
				x::GTE_RTPT();
				const uint32_t sz1 = MEM<uint32_t>(game_addr(game::GTE_SZ1)), sz2 = MEM<uint32_t>(game_addr(game::GTE_SZ2)), sz3 = MEM<uint32_t>(game_addr(game::GTE_SZ3));
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(h + 0x8C);
				if (S32(h, 0x8C) <= 0)
					continue;
				x::GTE_StoreSXY012_PolyGT3_2(pk);
				cx::GteStoreSXY012_PolyFT3_2(ft);
				U32(h, 0xA0) = U16(rec, 6) & 0xFFF;
				x::GTE_LoadV0(vtx(U32(h, 0xA0)));
				x::GTE_RTPS();
				const uint32_t sz4 = MEM<uint32_t>(game_addr(game::GTE_SZ3));
				x::GTE_ReadSXY2(pk + 0x2C);
				x::GTE_ReadSXY2(ft + 0x20);
				x::GTE_AVSZ4();
				x::GTE_ReadOTZ(h + 0x90);
				U32(h, 0x90) = U32(h, 0x90) >> 2;
				x::GTE_SetRotMatrix(h + 0x20);
				x::GTE_SetTransVector(h + 0x20);
				x::GTE_LoadV012(vtx(U32(h, 0x94)), vtx(U32(h, 0x98)), vtx(U32(h, 0x9C)));
				x::GTE_RTPT();
				U16(h, 0xCC) = 0;
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(h + 0x8C);
				if (U16(h, 0xCE) == 1 && S32(h, 0x8C) <= 0)
					U16(h, 0xCC) = 1;
				if (U16(h, 0xCC) == 0)
				{
					cx::GteReadSXY012(h + 0x40);
					x::GTE_LoadV0(vtx(U32(h, 0xA0)));
					x::GTE_RTPS();
					x::GTE_ReadSXY2(h + 0x4C);
					for (int i = 0; i < 4; i++)
					{
						const int16_t sx = PsxPixels(S16(h, 0x40 + 4 * i)), sy = PsxPixels(S16(h, 0x42 + 4 * i));
						if (sx < S16(h, 0xC0) || sx > S16(h, 0xC2) || sy < S16(h, 0xC4) || sy > S16(h, 0xC6))
						{
							U16(h, 0xCC) = 1;
							break;
						}
						U16(h, 0x40 + 4 * i) = (uint16_t)(U16(h, 0xC8) + (uint16_t)sx);
						U16(h, 0x42 + 4 * i) = (uint16_t)((uint16_t)sy + U16(h, 0xCA));
					}
					if (U16(h, 0xCC) == 0)
					{
						U8(pk, 0xD) = U8(h, 0x42);
						U8(pk, 0xC) = U8(h, 0x40);
						U8(pk, 0x18) = U8(h, 0x44);
						U8(pk, 0x24) = U8(h, 0x48);
						U8(pk, 0x19) = U8(h, 0x46);
						U8(pk, 0x25) = U8(h, 0x4A);
						U8(pk, 0x31) = U8(h, 0x4E);
						U8(pk, 0x30) = U8(h, 0x4C);
						U16(pk, 0x1A) = U16(h, 0xBC);
						U32(pk, 0x28) = U32(h, 0xAC);
						U32(pk, 0x1C) = U32(h, 0xAC);
						U32(pk, 0x10) = U32(h, 0xAC);
						U32(pk, 4) = U32(h, 0xAC);
						U16(pk, 0xE) = U16(h, 0xBE);
						U32(pk, 0) = 0xC000000;
						light = pk;
						pk += 0x34;
					}
				}
				U32(ft, 0xC) = U32(rec, 8);
				U32(ft, 0x14) = U32(rec, 0xC);
				U16(ft, 0x1C) = U16(rec, 0x10);
				U32(ft, 0) = 0x9000000;
				U16(ft, 0x24) = U16(rec, 0x12);
				U32(ft, 4) = U32(h, 0xB4);
				if (U8(rec, 0xF) & 2)
					U8(ft, 7) |= 2;
				x::SSIGPU_InsertPrimDepthKeys(ot + U32(h, 0x90) * 4, ft, sz1, sz2, sz3, sz4);
				if (light) x::SSIGPU_InsertPrimDepthKeys(ot + U32(h, 0x90) * 4, light, sz1, sz2, sz3, sz4);
				ft += 0x28;
				x::GTE_SetRotMatrix(h);
				x::GTE_SetTransVector(h);
			}
			frame_cur = ft;
			mod_cur = pk;
		}
		U32(U32(st, 0xC), 0) = frame_cur;
		U32(U32(st, 0x10), 0) = mod_cur;
		x::Field_Free(0xD0);
	}

	static void DrawOverlay(uint32_t st)
	{
		const uint32_t entity = U32(st, 8);
		OverlayMatrices(st, entity);
		const uint32_t anim = entity + 0x60;
		if (U16(st, 0xDA) != 0xFF)
			x::BattleModel_BuildBoneMatricesFromPose(anim);
		else
		{
			const uint32_t cmd = entity + 0x6C;
			const uint32_t r = cx::BattleReadAnimation(anim, cmd);
			if (r != 0)
				cx::PreBattleReadAnimation(anim, cmd, (r & 0xFFFF0000u) | U16(st, 0xD8));
		}
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), st + 0x14, st + 0x34);
		if ((U8(entity, 0) & 0x20) == 0)
			U32(U32(st, 0xC), 0) = x::sub_5088A0(entity, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4040, 0x10, U32(U32(st, 0xC), 0));
		OverlayLight(st);
		RenderModel(U32(entity, 0x64), RenderOT44(), st);
		const uint32_t second = U32(entity, 0x78);
		if (second != 0)
			RenderModel(U32(second, 4), RenderOT44(), st);
		if (U16(st, 0xDA) != 0xFF)
			x::BattleModel_BuildBoneMatricesFromPose(anim);
	}

	static uint32_t __cdecl Master(uint32_t a1)
	{
		g_mod = &MOD_051;
		const uint32_t node = a1;
		uint32_t states[5];
		states[0] = game_addr(game::MAG_051_HIPOTION_Root_State0_Advance);
		states[1] = game_addr(game::MAG_051_HIPOTION_Root_State1_SpawnEmitter);
		states[2] = game_addr(game::MAG_051_HIPOTION_State2_NextActionLoop);
		states[3] = game_addr(game::MAG_051_HIPOTION_Root_State3_WaitQueues);
		states[4] = game_addr(game::MAG_051_HIPOTION_Root_State4_Empty);
		if (U8(node, 0x5C) & 1)
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = MEM<uint32_t>(game_addr(ARENA_ODD));
		else
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = MEM<uint32_t>(game_addr(ARENA_EVEN));
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = (uint16_t)x::ExecuteTaskQueue(game_addr(Q_EMITTER));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_PLAYER)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_SPRITE)));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl NextAction(uint32_t a1)
	{
		if (U8(a1, 0x63) == 0)
		{
			const uint8_t action = U8(a1, 0x2A);
			if ((int16_t)(int8_t)action < S16(a1, 0x58))
			{
				U8(a1, 0x2A) = (uint8_t)(action + 1);
				U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) - 1);
			}
			else
				U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return a1;
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

	static uint32_t __cdecl Emitter(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[7];
		states[0] = game_addr(game::MAG_051_HIPOTION_Emitter_State0_SpawnGlow);
		states[1] = game_addr(game::MAG_051_HIPOTION_Emitter_State1_SpawnOverlay);
		states[2] = game_addr(game::MAG_051_HIPOTION_Emitter_State2_SpawnAuraC00);
		states[3] = game_addr(game::MAG_051_HIPOTION_Emitter_State3_SpawnAuraE00);
		states[4] = game_addr(game::MAG_051_HIPOTION_Emitter_State4_SpawnParticleEmitter);
		states[5] = game_addr(game::MAG_051_HIPOTION_Emitter_State5_Heal);
		states[6] = game_addr(game::MAG_051_HIPOTION_Emitter_State6_Empty);
		x::MAG_001_CURE_Emitter_UpdatePos(node);
		x::MAG_001_CURE_Emitter_ComputeModelBounds(node);
		callp(states[S8(node, 0x29)], node);
		if (U16(node, 0x24) == 0)
			x::BdPlaySE(game_addr(SOUND_HiPotion), 0, 0x80);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl SpawnFirst(uint32_t a1)
	{
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_SPRITE), game_addr(ORIG_Glow), 0x84, a1);
		const uint32_t aura = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PLAYER), game_addr(ORIG_Aura), 0xC8, a1);
		U16(aura, 0xC4) = 0;
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PLAYER), game_addr(ORIG_RingSpawner), 0xC8, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PLAYER), game_addr(ORIG_SparkleSpawner), 0xC8, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PLAYER), game_addr(ORIG_LightTask), 0xC8, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl SpawnOverlay(uint32_t a1)
	{
		const uint32_t aura = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PLAYER), game_addr(ORIG_Aura), 0xC8, a1);
		U16(aura, 0xC4) = 0x800;
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PLAYER), game_addr(ORIG_Overlay), 0xC8, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl SpawnAuraC00(uint32_t a1)
	{
		const uint32_t aura = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PLAYER), game_addr(ORIG_Aura), 0xC8, a1);
		U16(aura, 0xC4) = 0xC00;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}
	static uint32_t __cdecl SpawnAuraE00(uint32_t a1)
	{
		const uint32_t aura = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PLAYER), game_addr(ORIG_Aura), 0xC8, a1);
		U16(aura, 0xC4) = 0xE00;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl SpawnParticleEmitter(uint32_t a1)
	{
		if (S16(a1, 0x24) > 0xB)
		{
			x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PLAYER), game_addr(ORIG_ParticleEmitter), 0xC8, a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return a1;
	}

	static uint32_t __cdecl Heal(uint32_t a1)
	{
		if (S16(a1, 0x24) > 0x1E)
		{
			const int32_t action = S8(a1, 0x2A);
			const uint32_t acts = U32(U32(a1, 0xC), 4);
			x::ApplyActionResultToTarget(U32(acts + (uint32_t)(action * 20), 8));
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return a1;
	}

	static uint32_t __cdecl Glow(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_051_HIPOTION_Glow_State0_SpawnSwirl);
		states[1] = game_addr(game::MAG_051_HIPOTION_Glow_State1_CounterFinish);
		states[2] = game_addr(game::MAG_051_HIPOTION_Glow_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		DrawSprite(node);
		const uint32_t colour = ArgSlotColour(node, 0x20);
		DrawGroundSprite(node, U32(node, 0x4C), P(&colour));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl GlowStart(uint32_t a1)
	{
		const uint32_t node = a1;
		cx::CopyAnchorFromSource(node, node + 0x1C);
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + 0xFA00);
		U32(node, 0x4C) = game_addr(SEQ_Glow);
		U16(node, 0x52) = 3;
		uint32_t angle = x::CrtRand() & 0xFFF;
		for (int i = 8; i != 0; i--)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_SPRITE), game_addr(ORIG_Swirl), 0x84, node);
			S32(t, 0x30) = 0x1000;
			S32(t, 0x34) = 0x1000;
			S32(t, 0x38) = 0x1000;
			const uint32_t r = x::CrtRand();
			U16(t, 0x42) = (uint16_t)((r & 0x7F) + angle);
			angle = (angle + 0x200) & 0xFFF;
		}
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl Swirl(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_051_HIPOTION_Swirl_State0_Start);
		states[1] = game_addr(game::MAG_051_HIPOTION_Swirl_State1_Loop);
		states[2] = game_addr(game::MAG_051_HIPOTION_Swirl_State2_CounterFinish);
		states[3] = game_addr(game::MAG_051_HIPOTION_Swirl_State3_Empty);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x7C) = (uint16_t)(U16(node, 0x7C) + U16(node, 0x7E));
		if (S16(node, 0x7C) < 0)
			U16(node, 0x7C) = 0;
		const int32_t s = mul32((int32_t)x::computeSin((uint32_t)(int32_t)S16(node, 0x42)), S16(node, 0x7C));
		U16(node, 0x68) = (uint16_t)(U16(node, 0x68) + (uint16_t)(s / 4096));
		const int32_t c = mul32((int32_t)x::computeCosine((uint32_t)(int32_t)S16(node, 0x42)), S16(node, 0x7C));
		U16(node, 0x5A) = (uint16_t)(U16(node, 0x5A) + 0xA);
		const uint16_t xw = U16(node, 0x68);
		U16(node, 0x6C) = (uint16_t)(U16(node, 0x6C) + (uint16_t)(c / 4096));
		U16(node, 0x20) = U16(node, 0x6C);
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + U16(node, 0x5A));
		U16(node, 0x1C) = xw;
		if ((U8(node, 0x26) & 0x10) == 0 && (U8(node, 0x24) & 1) != 0)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_SPRITE), game_addr(ORIG_Mote), 0x84, node);
			RandomOffset(t, 0x80);
			U16(t, 0x1E) = U16(node, 0x1E);
			S32(t, 0x30) = 0x1000;
			S32(t, 0x34) = 0x1000;
			S32(t, 0x38) = 0x1000;
		}
		if (S16(node, 0x1E) > 0)
			U8(node, 0x26) |= 5;
		DrawSprite(node);
		const uint32_t colour = ArgSlotColour(node, 0x50);
		DrawGroundSprite(node, U32(node, 0x4C), P(&colour));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl SwirlStart(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t r1 = x::CrtRand();
		const uint32_t w20 = U32(node, 0x20);
		U16(node, 0x78) = (uint16_t)(r1 & 0xFFF);
		const uint32_t w1c = U32(node, 0x1C);
		U32(node, 0x4C) = game_addr(SEQ_Swirl);
		U32(node, 0x68) = w1c;
		U32(node, 0x6C) = w20;
		U16(node, 0x52) = 1;
		const uint32_t r2 = x::CrtRand();
		U16(node, 0x5A) = (uint16_t)((r2 & 0x3F) - 0x30);
		const uint32_t r3 = x::CrtRand();
		const int16_t radius = (int16_t)((r3 & 0x1F) + 0x50);
		U16(node, 0x7C) = (uint16_t)radius;
		U16(node, 0x7E) = (uint16_t)(-((int32_t)radius / 16));
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl SwirlLoop(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x50) = (uint16_t)(U16(node, 0x50) + 1);
		if (S16(node, 0x50) > S16(node, 0x52))
			U16(node, 0x50) = 0;
		if (S16(node, 0x24) >= 0x10)
		{
			U16(node, 0x26) |= 0x10;
			U32(node, 0x4C) = game_addr(SEQ_SwirlEnd);
			U16(node, 0x50) = 0;
			U16(node, 0x52) = 6;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	static uint32_t __cdecl Mote(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_051_HIPOTION_Mote_State0_Start);
		states[1] = game_addr(game::MAG_051_HIPOTION_Mote_State1_Fall);
		states[2] = game_addr(game::MAG_051_HIPOTION_Mote_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		if (S16(node, 0x1E) > 0)
			U8(node, 0x26) |= 5;
		DrawSprite(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl MoteStart(uint32_t a1)
	{
		const uint32_t node = a1;
		U32(node, 0x4C) = game_addr(SEQ_Mote);
		U16(node, 0x50) = 0;
		U16(node, 0x52) = 0xA;
		const uint32_t r = x::CrtRand();
		U16(node, 0x62) = 0xFFFD;
		U16(node, 0x5A) = (uint16_t)((r & 7) + 0x20);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl MoteFall(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x5A) = (uint16_t)(U16(node, 0x5A) + U16(node, 0x62));
		if (S16(node, 0x5A) < 2)
			U16(node, 0x5A) = 2;
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + U16(node, 0x5A));
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	static uint32_t __cdecl AuraStart(uint32_t a1)
	{
		S32(a1, 0x30) = 0x1000;
		S32(a1, 0x34) = 0x1000;
		S32(a1, 0x38) = 0x1000;
		DecodeLayout(game_addr(LAYOUT_Aura), a1 + 0x60, 0x64);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static void FrameLinks(uint32_t fr)
	{
		U32(fr, FR_BLOCK) = fr + FR_MATRIX;
		U32(fr, FR_BLOCK + 4) = fr + FR_FLAGS;
		U32(fr, FR_BLOCK + 8) = MEM<uint32_t>(game_addr(BLEND_BUFFER));
	}

	static uint32_t __cdecl AuraPlay(uint32_t a1)
	{
		const uint32_t node = a1;
		alignas(4) uint8_t frame[FR_SIZE] = {};
		const uint32_t fr = P(frame);
		U32(fr, FR_FLAGS) = 0x40000030;
		cx::CopyAnchorFromSource(node, node + 0x1C);
		U16(fr, FR_BLOCK + 0x10) = U16(node, 0xC4);
		const uint32_t m = fr + FR_MATRIX;
		x::MAG_022_sub_8DD770(m);
		S32(m, 0x14) = S16(node, 0x1C);
		S32(m, 0x18) = S16(node, 0x1E);
		S32(m, 0x1C) = S16(node, 0x20);
		x::scale3DMatrix(m, node + 0x30);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), m, m);
		FrameLinks(fr);
		const int left = prim::play((prim::Layout *)(node + 0x60), DrawCallback, (int)(fr + FR_BLOCK), 0);
		if (left == 0)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	static uint32_t __cdecl RingSpawn(uint32_t a1)
	{
		const uint32_t node = a1;
		int32_t i = 0;
		if (S16(game_addr(TAB_RINGS), S16(node, 0x24) * 2) > 0)
		{
			do
			{
				const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PLAYER), game_addr(ORIG_Ring), 0xC8, node);
				S32(t, 0x30) = 0x1000;
				S32(t, 0x34) = 0x1000;
				S32(t, 0x38) = 0x1000;
				U16(t, 0x50) = (uint16_t)(x::CrtRand() & 0xFFF);
				U16(t, 0x54) = (uint16_t)(x::CrtRand() & 0xFFF);
				U16(t, 0x58) = (uint16_t)((x::CrtRand() & 0x3F) + 0x20);
				U16(t, 0x5C) = (uint16_t)((x::CrtRand() & 0x3F) + 0x20);
				if (x::CrtRand() & 1)
					U16(t, 0x58) = (uint16_t)(0x1000 - U16(t, 0x58));
				if (x::CrtRand() & 1)
					U16(t, 0x5C) = (uint16_t)(0x1000 - U16(t, 0x5C));
				i++;
			} while (i < S16(game_addr(TAB_RINGS), S16(node, 0x24) * 2));
		}
		if (S16(node, 0x24) >= 0x12)
		{
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	static uint32_t __cdecl RingSpawnerStart(uint32_t a1)
	{
		cx::CopyMidpointFromSource(a1, a1 + 0x1C);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl Ring(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_051_HIPOTION_Ring_State0_Start);
		states[1] = game_addr(game::MAG_051_HIPOTION_Ring_State1_Play);
		states[2] = game_addr(game::MAG_051_HIPOTION_Ring_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		const uint16_t a50 = (uint16_t)(U16(node, 0x58) + U16(node, 0x50));
		const uint16_t a54 = (uint16_t)(U16(node, 0x5C) + U16(node, 0x54));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		U16(node, 0x50) = (uint16_t)(a50 & 0xFFF);
		U16(node, 0x54) = (uint16_t)(a54 & 0xFFF);
		return task_end(node, status);
	}

	static uint32_t __cdecl RingStart(uint32_t a1)
	{
		DecodeLayout(game_addr(LAYOUT_Ring), a1 + 0x60, 0x34);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl RingPlay(uint32_t a1)
	{
		const uint32_t node = a1;
		alignas(4) uint8_t frame[FR_SIZE] = {};
		const uint32_t fr = P(frame);
		U32(fr, FR_FLAGS) = 0x30;
		cx::CopyMidpointFromSource(node, node + 0x1C);
		const uint32_t m = fr + FR_MATRIX;
		x::ComposeZYXRotationMatrix(node + 0x50, m);
		S32(m, 0x14) = S16(node, 0x1C);
		S32(m, 0x18) = S16(node, 0x1E);
		S32(m, 0x1C) = S16(node, 0x20);
		x::scale3DMatrix(m, node + 0x30);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), m, m);
		FrameLinks(fr);
		const int left = prim::play((prim::Layout *)(node + 0x60), DrawCallback, (int)(fr + FR_BLOCK), 0);
		if (left == 0)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	static uint32_t __cdecl SparkleSpawnerStart(uint32_t a1)
	{
		const uint32_t node = a1;
		cx::CopyAnchorFromSource(node, node + 0x1C);
		const int16_t y = (int16_t)cx::BoundsBaseY(node);
		U16(node, 0x1E) = (uint16_t)y;
		U16(node, 0x42) = (uint16_t)Div18(0x100 - (int32_t)y);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl SparkleSpawn(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + U16(node, 0x42));
		for (int32_t i = 0; i < S16(game_addr(TAB_SPARKLES), S16(node, 0x24) * 2); i++)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_SPRITE), game_addr(ORIG_Sparkle), 0x84, node);
			RandomOffset(t, 0x200);
			U16(t, 0x1E) = U16(node, 0x1E);
			S32(t, 0x30) = 0x1000;
			S32(t, 0x34) = 0x1000;
			S32(t, 0x38) = 0x1000;
		}
		if (S16(node, 0x24) >= 0x12)
		{
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	static uint32_t __cdecl Sparkle(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_051_HIPOTION_Sparkle_State0_Start);
		states[1] = game_addr(game::MAG_051_HIPOTION_Sparkle_State1_CounterFinish);
		states[2] = game_addr(game::MAG_051_HIPOTION_Sparkle_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		DrawSprite(node);
		const uint32_t angle = (uint32_t)(U16(node, 0x7A) + 0x40) & 0xFFF;
		const uint32_t seq2 = U32(node, 0x70);
		const uint32_t colour = ArgSlotColour(node, 0x30);
		U16(node, 0x7A) = (uint16_t)angle;
		DrawSpinSprite(node, seq2, P(&colour), angle, 0x2000);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl SparkleStart(uint32_t a1)
	{
		const uint32_t node = a1;
		if (x::CrtRand() & 1)
		{
			U32(node, 0x4C) = game_addr(SEQ_SparkleA);
			U32(node, 0x70) = game_addr(SEQ_SparkleA2);
		}
		else
		{
			U32(node, 0x4C) = game_addr(SEQ_SparkleB);
			U32(node, 0x70) = game_addr(SEQ_SparkleB2);
		}
		const uint32_t r = x::CrtRand();
		U16(node, 0x52) = 0xF;
		U16(node, 0x7A) = (uint16_t)(r & 0xFFF);
		U16(node, 0x54) = 0xFE00;
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl ParticleEmitterStart(uint32_t a1)
	{
		const uint32_t node = a1;
		cx::CopyAnchorFromSource(node, node + 0x1C);
		const int16_t y = (int16_t)cx::BoundsBaseY(node);
		U16(node, 0x1E) = (uint16_t)y;
		U16(node, 0x42) = (uint16_t)Div20(0x100 - (int32_t)y);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl ParticleSpawn(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + U16(node, 0x42));
		for (int32_t i = 0; i < S16(game_addr(TAB_PARTICLES), S16(node, 0x24) * 2); i++)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_SPRITE), game_addr(ORIG_Particle), 0x84, node);
			U16(t, 0x1E) = (uint16_t)(U16(node, 0x1E) - 0x180);
			RandomOffset(t, 0x200);
		}
		if (S16(node, 0x24) >= 0x14)
		{
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	static uint32_t __cdecl Particle(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_051_HIPOTION_Particle_State0_Init);
		states[1] = game_addr(game::MAG_051_HIPOTION_Particle_State1_Rise);
		states[2] = game_addr(game::MAG_051_HIPOTION_Particle_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		if (S16(node, 0x1E) > 0)
			U8(node, 0x26) |= 5;
		DrawSprite(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl ParticleStart(uint32_t a1)
	{
		const uint32_t node = a1;
		U32(node, 0x4C) = game_addr(SEQ_Mote);
		U16(node, 0x52) = 0xA;
		U16(node, 0x54) = 0xFC00;
		const uint32_t r = x::CrtRand();
		U16(node, 0x5A) = (uint16_t)((r & 7) + 4);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl ParticleMove(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + U16(node, 0x5A));
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	static void SetLightWords(uint16_t v)
	{
		for (int k = 0; k < 4; k++)
			U16(game_addr(LIGHT_WORDS), k * 0x2C) = v;
	}

	static uint32_t __cdecl LightUp(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0xC0);
		if (S16(a1, 0x1C) >= 0x600)
		{
			U16(a1, 0x1C) = 0x600;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		SetLightWords(U16(a1, 0x1C));
		return 0;
	}

	static uint32_t __cdecl LightHold(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0x14)
		{
			U16(a1, 0x1E) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return a1;
	}

	static uint32_t __cdecl LightDown(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0xFF40);
		if (S16(a1, 0x1C) <= 0)
		{
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		SetLightWords(U16(a1, 0x1C));
		return 0;
	}

	static uint32_t __cdecl Overlay(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_051_HIPOTION_Overlay_State0_Start);
		states[1] = game_addr(game::MAG_051_HIPOTION_Overlay_State1_Rise);
		states[2] = game_addr(game::MAG_051_HIPOTION_Overlay_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		if (U8(node, 0x26) & 8)
		{
			alignas(4) uint8_t anchor[8] = {};
			cx::CopyAnchorFromSource(node, P(anchor));
			const uint32_t yaw = x::sub_8DD7B0(game_addr(game::unk_1D97778));
			U16(node, 0x52) = (uint16_t)yaw;
			const int32_t s = shl32((int32_t)x::computeSin((uint32_t)(int32_t)(int16_t)yaw), 0xA);
			U16(node, 0x1C) = (uint16_t)(s / 4096 + S32(P(anchor), 0));
			const int32_t c = shl32((int32_t)x::computeCosine((uint32_t)(int32_t)S16(node, 0x52)), 0xA);
			U16(node, 0x20) = (uint16_t)(c / 4096 + S32(P(anchor), 4));
			U16(game_addr(OVL_STATE), 0xB6) = U16(node, 0x1E);
			OverlayParams(game_addr(OVL_STATE), node + 0x1C, 0x280, 0x1C0, 0x140, 0xF4, 0x80, 0x40);
			DrawOverlay(game_addr(OVL_STATE));
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl OverlayStart(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t entity = U8(node, 0x2D) * 0x9Cu + game_addr(game::BattleEntitySlotData);
		cx::CopyAnchorFromSource(node, node + 0x1C);
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + 0xFA00);
		U16(node, 0x42) = (uint16_t)Div18(0x100 - (int32_t)S16(node, 0x1E));
		OverlayInit(game_addr(OVL_STATE), game_addr(OVL_FIRST), game_addr(OVL_LIGHT), game_addr(LIT_VERTICES), entity, U8(node, 0x2D));
		U8(node, 0x26) |= 8;
		U8(entity, 0) |= 4;
		U8(game_addr(OVL_STATE), 0xC4) = 0x40;
		U8(game_addr(OVL_STATE), 0xC5) = 0x40;
		U8(game_addr(OVL_STATE), 0xC6) = 0x40;
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl OverlayRise(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + U16(node, 0x42));
		if (S16(node, 0x24) >= 0x12)
		{
			const uint32_t entity = U8(node, 0x2D) * 0x9Cu + game_addr(game::BattleEntitySlotData);
			U16(entity, 0) &= 0xFFFB;
			U8(U32(node, 0x10), 0x63) = 0;
			U16(node, 0x26) = (uint16_t)((U16(node, 0x26) & 0xFFF7) | 1);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_051_HIPOTION_Root_State0_Advance, (void *)a_73A0D0, "051 MAG_051_HIPOTION_Root_State0_Advance" },
		{ game::MAG_051_HIPOTION_Root_State1_SpawnEmitter, (void *)a_73A170, "051 MAG_051_HIPOTION_Root_State1_SpawnEmitter" },
		{ game::MAG_051_HIPOTION_StepCounterLimit, (void *)a_743C20, "051 MAG_051_HIPOTION_StepCounterLimit" },
		{ game::MAG_051_HIPOTION_Mote_State2_Empty, (void *)a_73A6D0, "051 MAG_051_HIPOTION_Mote_State2_Empty" },
		{ game::MAG_051_HIPOTION_Swirl_State2_CounterFinish, (void *)a_743C00, "051 MAG_051_HIPOTION_Swirl_State2_CounterFinish" },
		{ game::MAG_051_HIPOTION_Swirl_State3_Empty, (void *)a_73A6D0, "051 MAG_051_HIPOTION_Swirl_State3_Empty" },
		{ game::MAG_051_HIPOTION_Glow_State1_CounterFinish, (void *)a_743C00, "051 MAG_051_HIPOTION_Glow_State1_CounterFinish" },
		{ game::MAG_051_HIPOTION_Glow_State2_Empty, (void *)a_73A6D0, "051 MAG_051_HIPOTION_Glow_State2_Empty" },
		{ game::MAG_051_HIPOTION_AuraTask, (void *)a_73A380, "051 MAG_051_HIPOTION_AuraTask" },
		{ game::MAG_051_HIPOTION_Aura_State2_Empty, (void *)a_73A6D0, "051 MAG_051_HIPOTION_Aura_State2_Empty" },
		{ game::MAG_051_HIPOTION_LightTask, (void *)a_73F110, "051 MAG_051_HIPOTION_LightTask" },
		{ game::MAG_051_HIPOTION_Light_State0_ResetEntityRecords, (void *)a_7335D0, "051 MAG_051_HIPOTION_Light_State0_ResetEntityRecords" },
		{ game::MAG_051_HIPOTION_Light_State4_Empty, (void *)a_73A6D0, "051 MAG_051_HIPOTION_Light_State4_Empty" },
		{ game::MAG_051_HIPOTION_RingSpawner, (void *)a_73A380, "051 MAG_051_HIPOTION_RingSpawner" },
		{ game::MAG_051_HIPOTION_Ring_State2_Empty, (void *)a_73A6D0, "051 MAG_051_HIPOTION_Ring_State2_Empty" },
		{ game::MAG_051_HIPOTION_RingSpawner_State2_Empty, (void *)a_73A6D0, "051 MAG_051_HIPOTION_RingSpawner_State2_Empty" },
		{ game::MAG_051_HIPOTION_SparkleSpawner, (void *)a_73A380, "051 MAG_051_HIPOTION_SparkleSpawner" },
		{ game::MAG_051_HIPOTION_Sparkle_State1_CounterFinish, (void *)a_743C00, "051 MAG_051_HIPOTION_Sparkle_State1_CounterFinish" },
		{ game::MAG_051_HIPOTION_Sparkle_State2_Empty, (void *)a_73A6D0, "051 MAG_051_HIPOTION_Sparkle_State2_Empty" },
		{ game::MAG_051_HIPOTION_SparkleSpawner_State2_Empty, (void *)a_73A6D0, "051 MAG_051_HIPOTION_SparkleSpawner_State2_Empty" },
		{ game::MAG_051_HIPOTION_Overlay_State2_Empty, (void *)a_73A6D0, "051 MAG_051_HIPOTION_Overlay_State2_Empty" },
		{ game::MAG_051_HIPOTION_ParticleEmitter_Tick, (void *)a_73A380, "051 MAG_051_HIPOTION_ParticleEmitter_Tick" },
		{ game::MAG_051_HIPOTION_Particle_State2_Empty, (void *)a_73A6D0, "051 MAG_051_HIPOTION_Particle_State2_Empty" },
		{ game::MAG_051_HIPOTION_ParticleEmitter_State2_Empty, (void *)a_73A6D0, "051 MAG_051_HIPOTION_ParticleEmitter_State2_Empty" },
		{ game::MAG_051_HIPOTION_Emitter_State6_Empty, (void *)a_73A6D0, "051 MAG_051_HIPOTION_Emitter_State6_Empty" },
		{ game::MAG_051_HIPOTION_Root_State4_Empty, (void *)a_73A6D0, "051 MAG_051_HIPOTION_Root_State4_Empty" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ ORIG_Master, (void *)Master, "051 Hi-Potion Master" },
		{ game::MAG_051_HIPOTION_State2_NextActionLoop, (void *)NextAction, "051 NextAction" },
		{ game::MAG_051_HIPOTION_Root_State3_WaitQueues, (void *)WaitQueues, "051 WaitQueues" },
		{ ORIG_Emitter, (void *)Emitter, "051 Emitter" },
		{ game::MAG_051_HIPOTION_Emitter_State0_SpawnGlow, (void *)SpawnFirst, "051 SpawnFirst" },
		{ game::MAG_051_HIPOTION_Emitter_State1_SpawnOverlay, (void *)SpawnOverlay, "051 SpawnOverlay" },
		{ game::MAG_051_HIPOTION_Emitter_State2_SpawnAuraC00, (void *)SpawnAuraC00, "051 SpawnAuraC00" },
		{ game::MAG_051_HIPOTION_Emitter_State3_SpawnAuraE00, (void *)SpawnAuraE00, "051 SpawnAuraE00" },
		{ game::MAG_051_HIPOTION_Emitter_State4_SpawnParticleEmitter, (void *)SpawnParticleEmitter, "051 SpawnParticleEmitter" },
		{ game::MAG_051_HIPOTION_Emitter_State5_Heal, (void *)Heal, "051 Heal" },
		{ ORIG_Glow, (void *)Glow, "051 Glow" },
		{ game::MAG_051_HIPOTION_Glow_State0_SpawnSwirl, (void *)GlowStart, "051 GlowStart" },
		{ ORIG_Swirl, (void *)Swirl, "051 Swirl" },
		{ game::MAG_051_HIPOTION_Swirl_State0_Start, (void *)SwirlStart, "051 SwirlStart" },
		{ game::MAG_051_HIPOTION_Swirl_State1_Loop, (void *)SwirlLoop, "051 SwirlLoop" },
		{ ORIG_Mote, (void *)Mote, "051 Mote" },
		{ game::MAG_051_HIPOTION_Mote_State0_Start, (void *)MoteStart, "051 MoteStart" },
		{ game::MAG_051_HIPOTION_Mote_State1_Fall, (void *)MoteFall, "051 MoteFall" },
		{ game::MAG_051_HIPOTION_Aura_State0_Start, (void *)AuraStart, "051 AuraStart" },
		{ game::MAG_051_HIPOTION_Aura_State1_Play, (void *)AuraPlay, "051 AuraPlay" },
		{ game::MAG_051_HIPOTION_RingSpawner_State0_Start, (void *)RingSpawnerStart, "051 RingSpawnerStart" },
		{ game::MAG_051_HIPOTION_RingSpawner_State1_SpawnRing, (void *)RingSpawn, "051 RingSpawn" },
		{ ORIG_Ring, (void *)Ring, "051 Ring" },
		{ game::MAG_051_HIPOTION_Ring_State0_Start, (void *)RingStart, "051 RingStart" },
		{ game::MAG_051_HIPOTION_Ring_State1_Play, (void *)RingPlay, "051 RingPlay" },
		{ game::MAG_051_HIPOTION_SparkleSpawner_State0_Start, (void *)SparkleSpawnerStart, "051 SparkleSpawnerStart" },
		{ game::MAG_051_HIPOTION_SparkleSpawner_State1_SpawnSparkle, (void *)SparkleSpawn, "051 SparkleSpawn" },
		{ ORIG_Sparkle, (void *)Sparkle, "051 Sparkle" },
		{ game::MAG_051_HIPOTION_Sparkle_State0_Start, (void *)SparkleStart, "051 SparkleStart" },
		{ game::MAG_051_HIPOTION_ParticleEmitter_State0_AnchorAndRate, (void *)ParticleEmitterStart, "051 ParticleEmitterStart" },
		{ game::MAG_051_HIPOTION_ParticleEmitter_State1_RiseAndSpawn, (void *)ParticleSpawn, "051 ParticleSpawn" },
		{ ORIG_Particle, (void *)Particle, "051 Particle" },
		{ game::MAG_051_HIPOTION_Particle_State0_Init, (void *)ParticleStart, "051 ParticleStart" },
		{ game::MAG_051_HIPOTION_Particle_State1_Rise, (void *)ParticleMove, "051 ParticleMove" },
		{ game::MAG_051_HIPOTION_Light_State1_Up, (void *)LightUp, "051 LightUp" },
		{ game::MAG_051_HIPOTION_Light_State2_WaitTick20, (void *)LightHold, "051 LightHold" },
		{ game::MAG_051_HIPOTION_Light_State3_Down, (void *)LightDown, "051 LightDown" },
		{ ORIG_Overlay, (void *)Overlay, "051 Overlay" },
		{ game::MAG_051_HIPOTION_Overlay_State0_Start, (void *)OverlayStart, "051 OverlayStart" },
		{ game::MAG_051_HIPOTION_Overlay_State1_Rise, (void *)OverlayRise, "051 OverlayRise" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag051_hipotion()
	{
		act::register_module(51);
		for (const act::hipotion::ModPort *p = act::hipotion::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(51, p->addr, p->port, p->name);
		for (const act::hipotion::ModPort *p = act::hipotion::PORTS; p->addr; p++)
			act::register_module_port(51, p->addr, p->port, p->name);
	}
}

