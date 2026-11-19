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

// Effect 37: Drink Magic: a copy of the actor/heal

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
namespace drinkmagic
{
	static const Mod MOD_037 = { "drink", 37, game::MAG_037_DRINK_MAGIC_FL, game::MAG_036_CONFUSE_FL,
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::unk_26A05EC, {}, {}, {}, {}, {}, {}, {}, game::MAG_037_ActorSystemQueue, {}, game::MAG_037_EmitterQueue, {}, {}, {}, {}, {}, game::unk_26A8D8A, {}, {}, game::unk_26A8D9A, {}, game::MAG_037_ScratchSP, {}, game::MAG_037_ActorStatePtr },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_037_DirectorSetup, game::MAG_037_ActSys_Step, game::MAG_037_ActSys_DrawPass, game::MAG_037_ActSys_DrawActorModel, game::MAG_037_RenderPrimModel, game::MAG_037_MeshEmitF3, game::MAG_037_MeshEmitF4, game::MAG_037_RenderPolyFT3List, game::MAG_037_RenderPolyFT4List, game::MAG_037_PrimList_G3, game::MAG_037_PrimList_G4, game::MAG_037_PrimList_GT3, game::MAG_037_PrimList_GT4, game::MAG_037_ActSys_DrawSpritePass, game::MAG_037_ActSys_UpdatePass, game::MAG_037_ActorStep, game::MAG_037_ActSys_UpdateEmitter, game::MAG_037_Particle_LifeCountdown, game::MAG_037_ActSys_EmitterSpawnStep, game::MAG_037_ActSys_SpawnParticles, game::MAG_037_RandVec3, game::MAG_037_JitterAngles3, game::MAG_037_ActSys_AllocParticleSlot, game::MAG_037_GetWord8Minus1, game::MAG_037_Particle_Init, game::MAG_037_SpawnSubEffects, game::MAG_037_Director_SpawnGroupEffects, {}, {}, {}, {}, {}, game::MAG_037_DrinkMagic_EmitterTask, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_037_ResetTrailHistory, game::MAG_037_Director_CasterRefPoints, game::MAG_037_FillTrailRows, game::MAG_037_Director_Mode3Setup, game::MAG_037_Director_TargetRefPoints, game::MAG_037_RelocateActorData, game::MAG_037_ActSys_DrawActorSprite, game::MAG_037_ActorMotion, game::MAG_037_DampValue, game::MAG_037_DampVector, game::MAG_037_Instance_AdvanceTexFrame, game::MAG_037_Instance_BuildTransform, game::MAG_037_MorphVertexBlock, game::MAG_037_Instance_EndOfLifeTest, game::MAG_037_Instance_SpawnFollowUp, game::MAG_037_AllocParticle, game::MAG_037_RandRange, game::MAG_037_RandRangeWide, game::MAG_037_RandAngleJitter, game::MAG_037_Particle_AttachToOwner, game::MAG_037_Actor_InterpolatePosition, {}, game::MAG_037_ParticleList_Unlink, game::MAG_037_Particle_PlaceAtAnchorSet, game::MAG_037_Particle_PlaceAtPoint, {}, game::MAG_037_StepCounterLimit, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::MAG_037_PacketCursor;
	static constexpr game::Id ACTOR_STATE = game::MAG_037_ActorStatePtr;
	static constexpr game::Id SCRATCH_SP = game::MAG_037_ScratchSP;
	static constexpr game::Id GATHER = game::g_DrinkMagic_GatherPoint;
	static constexpr game::Id Q_EMITTER = game::MAG_037_EmitterQueue, Q_ACTORS = game::MAG_037_ActorSystemQueue, Q_RIBBON = game::MAG_037_RibbonQueue, Q_ORB = game::MAG_037_OrbQueue, Q_STAGE = game::MAG_037_UnusedQueue;
	static constexpr game::Id ORIG_Master = game::MAG_037_DrinkMagic_MasterTask;
	static constexpr game::Id ORIG_Emitter = game::MAG_037_DrinkMagic_EmitterTask;
	static constexpr game::Id ORIG_ActorSystem = game::MAG_037_ActorSystemTask;
	static constexpr game::Id ORIG_Spawner = game::MAG_037_DrinkMagic_SpawnerTask;
	static constexpr game::Id ORIG_Streak = game::MAG_037_DrinkMagic_StreakTask;
	static constexpr game::Id ORIG_Orb = game::MAG_037_DrinkMagic_OrbTask;
	static constexpr game::Id ORIG_Ribbon = game::MAG_037_DrinkMagic_RibbonTask;
	static constexpr game::Id ORIG_Spark = game::MAG_037_DrinkMagic_SparkTask;
	static const uint32_t NODE_SIZE = 0x130;
	static constexpr game::Id ACTOR_DATA = game::unk_17C8420;
	static const void *SOUND_Drink() { return (const void *)game_addr(game::unk_15D62E8); }
	static constexpr game::Id RIBBON_FLIPBOOK = game::unk_15D92F4, SPARK_FLIPBOOK = game::unk_15D9504, STREAK_FLIPBOOK = game::unk_15D9624;
	static constexpr game::Id ORB_MODEL = game::unk_15D62EC, ORB_TEMPLATE = game::unk_15D860C;

	namespace gx
	{
		static inline uint32_t sub_45E0B0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadIR123_u16))(a1); }
		static inline uint32_t sub_45E9D0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPF))(); }
		static inline uint32_t sub_45EBF0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPL))(); }
		static inline uint32_t sub_45E220(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadSZ3))(a1); }
		static inline uint32_t BdPlaySy(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::BdPlaySy))(a1, a2, a3); }
		static inline uint32_t TransformCameraByShadowRotation(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::TransformCameraByShadowRotation))(a1, a2, a3); }
		static inline uint32_t InitEffectSequenceFromData(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::InitEffectSequenceFromData))(a1, a2, a3, a4); }
		static inline uint32_t Effect_CopyMidpointFromSource(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::Effect_CopyMidpointFromSource))(a1, a2); }
		static inline uint32_t MAG_037_render(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::MAG_037_PrimList_GT3Scroll))(a1, a2, a3, a4); }
		static inline uint32_t Effect_BuildMatrixFromDirAndUp(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::Effect_BuildMatrixFromDirAndUp))(a1, a2, a3); }
	}

	static inline void e6_copy16(uint32_t dst, uint32_t src)
	{
		U32(dst, 0) = U32(src, 0);
		U32(dst, 4) = U32(src, 4);
		U32(dst, 8) = U32(src, 8);
		U32(dst, 12) = U32(src, 12);
	}
	static inline void e6_rotate_zxy(uint32_t m, uint32_t ang)
	{
		x::MAG_022_sub_8DD770(m);
		if (U16(ang, 4) != 0) x::sub_8DD960(m, (uint32_t)(int32_t)S16(ang, 4));
		if (U16(ang, 0) != 0) x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(ang, 0));
		if (U16(ang, 2) != 0) x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(ang, 2));
	}
	static inline void e6_rotate_down_vector(uint32_t sp)
	{
		uint32_t v = sp + 0x20;
		U16(sp, 0x22) = 0xF000;
		U16(sp, 0x24) = 0;
		U16(v, 0) = 0;
		x::GTE_SetRotMatrix(sp);
		x::GTE_LoadV0(v);
		x::GTE_MVMVA_RotV0();
		x::GTE_StoreIR123(v);
	}
	static inline void e6_rand_mod(uint32_t sp, int32_t off)
	{
		if (U16(sp, off) != 0)
		{
			int32_t r = (int32_t)x::CrtRand();
			U16(sp, off) = (uint16_t)(r % (int32_t)S16(sp, off));
		}
	}
	static inline void b1_copy16(uint32_t dst, uint32_t src)
	{
		for (int i = 0; i < 16; i += 4)
			U32(dst, i) = U32(src, i);
	}
	static inline bool b1_out(int16_t v, int16_t hi) { return v < 0 || v > hi; }
	static inline bool b2_out_x(int16_t v) { return v < 0 || v > 0xA00; }
	static inline bool b2_out_y(int16_t v) { return v < 0 || v > 0x6C0; }
	static inline int32_t b2_fx(uint32_t v)
	{
		int32_t s = (int32_t)v;
		return add32(s, (s >> 31) & 0xFFFF) >> 16;
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

	static inline int32_t div3(int32_t v)
	{
		const int32_t hi = (int32_t)(((int64_t)v * 0x55555556LL) >> 32);
		return add32(hi, (int32_t)((uint32_t)hi >> 31));
	}
	static inline int32_t div10(int32_t v)
	{
		const int32_t hi = (int32_t)(((int64_t)v * 0x66666667LL) >> 32) >> 2;
		return add32(hi, (int32_t)((uint32_t)hi >> 31));
	}

	static inline bool t1_out_x(int16_t v) { return v < 0 || v > 0xA00; }
	static inline bool t1_out_y(int16_t v) { return v < 0 || v > 0x6C0; }
	static uint32_t t2_twin(uint32_t ctx, int32_t b)
	{
		uint32_t ecx = (uint32_t)(U8(ctx, b + 2) & 0xF8) | 0xFFFE2000u;
		uint32_t edx = (uint32_t)(U8(ctx, b) & 0xF8);
		ecx <<= 5;
		ecx |= edx;
		edx = U16(ctx, b + 6);
		ecx <<= 5;
		edx = ~(edx - 1) & 0xF8;
		ecx |= edx;
		edx = U16(ctx, b + 4);
		ecx <<= 2;
		edx = (uint32_t)((int32_t)~(edx - 1) >> 3) & 0x1F;
		ecx |= edx;
		return ecx;
	}
	static inline bool t2_outx(uint32_t p) { const int16_t v = S16(p, 0); return v < 0 || v > 0xA00; }
	static inline bool t2_outy(uint32_t p) { const int16_t v = S16(p, 0); return v < 0 || v > 0x6C0; }
	static inline void t2_depth_cue(uint32_t ctx, uint32_t pCode)
	{
		x::set_unk_1CA8A28(pCode);
		x::set_dword_1CA8A30(U32(ctx, 0xC));
		x::sub_45F270();
		x::set_param_with_dword_1CA8A68(pCode);
	}
	static inline uint32_t t2_ot(uint32_t ctx, uint32_t a2, uint32_t a3)
	{
		const int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
		S32(ctx, 0x38) = z;
		if (z < 0)
			S32(ctx, 0x38) = 0;
		return a2 + (uint32_t)(S32(ctx, 0x38) >> (a3 & 31)) * 4u;
	}
	static inline bool t3_out_x(int16_t v) { return v < 0 || v > 0xA00; }
	static inline bool t3_out_y(int16_t v) { return v < 0 || v > 0x6C0; }
	static inline uint32_t t3_texwin(uint32_t p)
	{
		uint32_t t = ((uint32_t)U8(p, 2) & 0xF8) | 0xFFFE2000u;
		t = (t << 5) | ((uint32_t)U8(p, 0) & 0xF8);
		t = (t << 5) | (~((uint32_t)U16(p, 6) - 1) & 0xF8);
		t = (t << 2) | ((~((uint32_t)U16(p, 4) - 1) >> 3) & 0x1F);
		return t;
	}
	static inline void t3_scroll(uint32_t pkt, const int32_t *off, int n, uint16_t d, uint8_t wrap)
	{
		uint32_t s[4];
		uint32_t any = 0;
		for (int i = 0; i < n; i++)
		{
			s[i] = (uint32_t)U8(pkt, off[i]) + (uint32_t)d;
			any |= s[i];
		}
		const bool over = (int32_t)any > 0xFF;
		for (int i = 0; i < n; i++)
			U8(pkt, off[i]) = over ? (uint8_t)(s[i] - wrap) : (uint8_t)s[i];
	}

	namespace
	{
		uint32_t s2_twin(uint32_t ctx, int32_t b)
		{
			uint32_t ecx = (uint32_t)(U8(ctx, b + 2) & 0xF8) | 0xFFFE2000u;
			uint32_t edx = (uint32_t)(U8(ctx, b) & 0xF8);
			ecx <<= 5;
			ecx |= edx;
			edx = U16(ctx, b + 6);
			ecx <<= 5;
			edx = ~(edx - 1) & 0xF8;
			ecx |= edx;
			edx = U16(ctx, b + 4);
			ecx <<= 2;
			edx = (uint32_t)((int32_t)~(edx - 1) >> 3) & 0x1F;
			ecx |= edx;
			return ecx;
		}
	}
	namespace
	{
		uint32_t s3_texwin(uint32_t ctx, int32_t o)
		{
			uint32_t c = ((uint32_t)U8(ctx, o + 2) & 0xF8) | 0xFFFE2000u;
			c = (c << 5) | ((uint32_t)U8(ctx, o) & 0xF8);
			c = (c << 5) | (~((uint32_t)U16(ctx, o + 6) - 1u) & 0xF8);
			c = (c << 2) | ((~((uint32_t)U16(ctx, o + 4) - 1u) >> 3) & 0x1F);
			return c;
		}

		void s3_scroll_uv(uint32_t pkt, int32_t o, uint32_t d, uint32_t ctx, int32_t wo)
		{
			int32_t a = (int32_t)(U8(pkt, o) + d);
			int32_t c = (int32_t)(U8(pkt, o + 0xC) + d);
			int32_t e = (int32_t)(U8(pkt, o + 0x18) + d);
			int32_t b = (int32_t)(U8(pkt, o + 0x24) + d);
			if ((b | e | c | a) > 0xFF)
			{
				uint8_t w = U8(ctx, wo);
				U8(pkt, o) = (uint8_t)(a - w);
				U8(pkt, o + 0xC) = (uint8_t)(c - w);
				U8(pkt, o + 0x18) = (uint8_t)(e - w);
				U8(pkt, o + 0x24) = (uint8_t)(b - w);
			}
			else
			{
				U8(pkt, o) = (uint8_t)a;
				U8(pkt, o + 0xC) = (uint8_t)c;
				U8(pkt, o + 0x18) = (uint8_t)e;
				U8(pkt, o + 0x24) = (uint8_t)b;
			}
		}

		inline bool s3_off(int16_t v, int16_t lim) { return v < 0 || v > lim; }

		uint32_t s3_quad(uint32_t ctx, uint32_t data, uint32_t vbase, uint32_t ot_base, uint32_t shift, uint32_t pkt)
		{
			x::GTE_LoadV012(vbase + ((uint32_t)U16(data, 4) << 2), vbase + ((uint32_t)U16(data, 6) << 2), vbase + ((uint32_t)U16(data, 8) << 2));
			x::GTE_RTPT();
			uint32_t flags = U32(ctx, 0x14);
			uint32_t col0 = U32(data, 0);
			U32(pkt, 0) = 0x0C000000;
			U32(pkt, 4) = col0;
			if (flags & 2)
			{
				col0 |= 0x2000000;
				U32(pkt, 4) = col0;
			}
			if (flags & 8)
				U32(pkt, 4) &= 0xFDFFFFFF;
			uint32_t uv0 = U32(data, 0xC);
			uint32_t uv1 = U32(data, 0x10);
			U32(pkt, 0xC) = uv0;
			uint32_t uv23 = U32(data, 0x14);
			U32(pkt, 0x24) = uv23;
			U32(pkt, 0x18) = uv1;
			U32(pkt, 0x30) = uv23 >> 16;
			x::GTE_ReadFLAG(ctx + 0x3C);
			if (U32(ctx, 0x3C) & 0x60000)
				return pkt;
			x::GTE_NCLIP();
			uint32_t clip = 0;
			x::GTE_ReadMAC0(ctx + 0x30);
			if (S32(ctx, 0x30) < 0 && !(U8(ctx, 0x14) & 0x20))
				return pkt;
			x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x14, pkt + 0x20);
			x::GTE_LoadV0(vbase + ((uint32_t)U16(data, 0xA) << 2));
			x::GTE_RTPS();
			if (s3_off(S16(pkt, 0x8), 0xA00)) clip = 1;
			if (s3_off(S16(pkt, 0x14), 0xA00)) clip |= 2;
			if (s3_off(S16(pkt, 0x20), 0xA00)) clip |= 4;
			if (s3_off(S16(pkt, 0xA), 0x6C0)) clip |= 0x10;
			if (s3_off(S16(pkt, 0x16), 0x6C0)) clip |= 0x20;
			if (s3_off(S16(pkt, 0x22), 0x6C0)) clip |= 0x40;
			x::GTE_ReadSXY2(pkt + 0x2C);
			x::GTE_AVSZ4();
			uint32_t c = clip;
			if (s3_off(S16(pkt, 0x2C), 0xA00)) c |= 8;
			if (s3_off(S16(pkt, 0x2E), 0x6C0)) c |= 0x80;
			if ((c & 0xF) == 0xF)
				return pkt;
			if ((c & 0xF0) == 0xF0)
				return pkt;
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x80)
			{
				x::sub_45E120(data + 0x18, data + 0x1C, data + 0x20);
				x::set_dword_1CA8A30(U32(ctx, 0xC));
				x::sub_45F4C0();
				x::sub_45E370(pkt + 0x10, pkt + 0x1C, pkt + 0x28);
				x::set_unk_1CA8A28(pkt + 4);
				x::sub_45F270();
				x::set_param_with_dword_1CA8A68(pkt + 4);
			}
			else
			{
				uint32_t c1 = U32(data, 0x18);
				uint32_t c2 = U32(data, 0x1C);
				U32(pkt, 0x10) = c1;
				uint32_t c3 = U32(data, 0x20);
				U32(pkt, 0x1C) = c2;
				U32(pkt, 0x28) = c3;
			}
			int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
			S32(ctx, 0x38) = z;
			if (z < 0)
				S32(ctx, 0x38) = 0;
			uint32_t ot = ot_base + ((uint32_t)(S32(ctx, 0x38) >> (shift & 31)) << 2);
			uint16_t du = U16(ctx, 0x18);
			uint16_t dv = U16(ctx, 0x1A);
			if ((uint16_t)(du | dv) == 0)
			{
				x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
				return pkt + 0x34;
			}
			if (du != 0)
				s3_scroll_uv(pkt, 0xC, du, ctx, 0x28);
			uint16_t dv2 = U16(ctx, 0x1A);
			if (dv2 != 0)
				s3_scroll_uv(pkt, 0xD, dv2, ctx, 0x2A);
			uint32_t prim = pkt;
			uint32_t twa = pkt + 0x34;
			U32(twa, 0) = 0x2000000;
			uint32_t w = (ctx + 0x1C != 0) ? s3_texwin(ctx, 0x1C) : 0;
			U32(twa, 4) = w;
			U32(twa, 8) = 0;
			x::SSIGPU_InsertPrimAutoDepth(ot, twa);
			x::SSIGPU_InsertPrimAutoDepth(ot, prim);
			uint32_t twb = pkt + 0x40;
			U32(twb, 0) = 0x2000000;
			w = (ctx + 0x24 != 0) ? s3_texwin(ctx, 0x24) : 0;
			U32(twb, 4) = w;
			U32(twb, 8) = 0;
			x::SSIGPU_InsertPrimAutoDepth(ot, twb);
			return pkt + 0x4C;
		}

		inline uint32_t s3_task_end(uint32_t n, uint8_t status)
		{
			if ((status & 1) && U8(n, 0x28) == 0)
			{
				x::Effect_ReleaseLinkedTask(n);
				return 2;
			}
			return 0;
		}
	}

	uint32_t __cdecl dk_856B40(uint32_t a1);
	uint32_t __cdecl dk_856CC0(uint32_t a1);
	uint32_t __cdecl dk_856D00(void);
	uint32_t __cdecl dk_856D70(uint32_t a1);
	uint32_t __cdecl dk_856E00(uint32_t a1);
	uint32_t __cdecl dk_856EF0(uint32_t a1);
	uint32_t __cdecl dk_856F20(uint32_t a1);
	uint32_t __cdecl dk_8570D0(uint32_t a1);
	uint32_t __cdecl dk_8571B0(void);
	uint32_t __cdecl dk_857310(void);
	uint32_t __cdecl dk_857500(void);
	uint32_t __cdecl dk_857970(uint32_t a1);
	uint32_t __cdecl dk_857CE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dk_857EF0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dk_858650(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dk_858880(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dk_858B40(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dk_858DA0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dk_859080(void);
	uint32_t __cdecl dk_859140(uint32_t a1);
	uint32_t __cdecl dk_8591C0(void);
	uint32_t __cdecl dk_859200(uint32_t a1);
	uint32_t __cdecl dk_85A230(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dk_85A340(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dk_85A3C0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dk_85A3F0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl dk_85A490(uint32_t a1);
	uint32_t __cdecl dk_85A510(uint32_t a1);
	uint32_t __cdecl dk_85A590(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl dk_85AD70(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dk_85AF90(uint32_t a1);
	uint32_t __cdecl dk_85BA80(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dk_85BB10(void);
	uint32_t __cdecl dk_85BBA0(uint32_t a1);
	uint32_t __cdecl dk_85BBD0(uint32_t a1);
	uint32_t __cdecl dk_85BC50(uint32_t a1);
	uint32_t __cdecl dk_85BC90(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dk_85BD50(uint32_t a1);
	uint32_t __cdecl dk_85BE30(uint32_t a1);
	uint32_t __cdecl dk_85BEA0(uint32_t a1);
	uint32_t __cdecl dk_85BEC0(uint32_t a1);
	uint32_t __cdecl dk_85BFD0(uint32_t a1);
	uint32_t __cdecl dk_85C0C0(uint32_t a1);
	uint32_t __cdecl dk_85C110(uint32_t a1);
	uint32_t __cdecl dk_85C1F0(uint32_t a1);
	uint32_t __cdecl dk_85C2C0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dk_85C350(uint32_t a1);
	uint32_t __cdecl dk_85C480(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dk_85C5B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dk_85C7D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dk_85CA60(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dk_85CEE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dk_85DE90(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dk_85E4E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dk_85E620(uint32_t a1);
	uint32_t __cdecl dk_85E640(uint32_t a1);
	uint32_t __cdecl dk_85E6C0(uint32_t a1);
	uint32_t __cdecl dk_85E780(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dk_85E820(uint32_t a1);
	uint32_t __cdecl dk_85E860(uint32_t a1);
	uint32_t __cdecl dk_85E890(uint32_t a1);
	uint32_t __cdecl dk_85E8E0(uint32_t a1);
	uint32_t __cdecl dk_85E930(uint32_t a1);
	uint32_t __cdecl dk_85EA30(uint32_t a1);
	uint32_t __cdecl dk_85EB60(uint32_t a1);
	uint32_t __cdecl dk_85EF20(uint32_t a1);
	uint32_t __cdecl dk_85F020(uint32_t a1);
	uint32_t __cdecl dk_85F060(uint32_t a1);
	uint32_t __cdecl dk_85F0D0(uint32_t a1);
	uint32_t __cdecl dk_85F1B0(uint32_t a1);
	uint32_t __cdecl dk_85F260(uint32_t a1);
	uint32_t __cdecl dk_85F2B0(uint32_t a1);
	uint32_t __cdecl dk_85F300(uint32_t a1);
	uint32_t __cdecl dk_85F3A0(uint32_t a1);
	uint32_t __cdecl dk_85F3C0(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace drinkmagic
{
	uint32_t __cdecl dk_856B40(uint32_t a1)
	{
		g_mod = &MOD_037;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_26A05EC)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_037_DrinkMagic_Master_State0_Advance);
		states[1] = game_addr(game::MAG_037_DrinkMagic_Master_State1_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[2] = game_addr(game::MAG_037_DrinkMagic_Master_State2_CarveActorPools);
		states[3] = game_addr(game::MAG_037_DrinkMagic_Master_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_037_DrinkMagic_Master_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_037_DrinkMagic_Master_State5_Advance);
		states[6] = game_addr(game::MAG_037_DrinkMagic_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_037_DrinkMagic_Master_State7_Advance);
		states[8] = game_addr(game::MAG_037_DrinkMagic_Master_State8_Advance);
		states[9] = game_addr(game::MAG_037_DrinkMagic_Master_State9_Finish);
		states[10] = game_addr(game::MAG_037_DrinkMagic_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_26A8D7C));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_26A8174));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_26A05E8)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_26A8D78));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_26A8170));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_26A05E8)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_26A8D9A)) = 0;
		MEM<uint16_t>(game_addr(game::unk_26A8D8A)) = 0;
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_EMITTER)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_ACTORS)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_RIBBON)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_ORB)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_STAGE)));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl dk_856CC0(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::MAG_037_FileArenaCursor));
		MEM<uint32_t>(game_addr(game::MAG_037_ParticlePoolPtr)) = p;
		p += 0x438;
		MEM<uint32_t>(game_addr(game::MAG_037_ActorPoolPtr)) = p;
		p += 0x6900;
		MEM<uint32_t>(game_addr(game::MAG_037_FileArenaCursor)) = p;
		dk_856D00();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl dk_856D00(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_037_ParticlePoolPtr)), 0x438);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_037_ActorPoolPtr)), 0x6900);
		MEM<uint16_t>(game_addr(game::MAG_037_ParticleCursor)) = 0;
		MEM<uint16_t>(game_addr(game::MAG_037_ActorCursor)) = 0;
		MEM<uint16_t>(game_addr(game::unk_26A8D88)) = 0;
		MEM<uint16_t>(game_addr(game::unk_26A05D0)) = 0;
		return 0;
	}

	uint32_t __cdecl dk_856D70(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_037_DrinkMagic_Emitter_State0_SpawnActorSystem);
		states[1] = game_addr(game::MAG_037_DrinkMagic_Emitter_State1_ReleaseMasterAt15);
		states[2] = game_addr(game::MAG_037_DrinkMagic_Emitter_State2_ApplyDamage);
		states[3] = game_addr(game::MAG_037_DrinkMagic_Emitter_State3_Empty);
		x::MAG_001_CURE_Emitter_UpdatePos(node);
		x::MAG_001_CURE_Emitter_ComputeModelBounds(node);
		callp(states[S8(node, 0x29)], node);
		if (U16(node, 0x24) == 0)
			x::BdPlaySE(P(SOUND_Drink()), 0, 0x80);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl dk_856EF0(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			dk_856F20(a1);
			dk_857970(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl dk_856F20(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t data = U32(node, 0x30);
		uint16_t w29a = U16(node, 0x29A);
		uint16_t w29c = U16(node, 0x29C);
		uint32_t st = node + 0x34;
		const uint32_t root = U32(node, 0x10);
		MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr)) = st;
		U16(st, 0x22) = w29a;
		uint16_t caster = (uint16_t)U8(node, 0x2C);
		U16(st, 0x24) = w29c;
		U32(st, 0x224) = data + 4;
		U32(st, 0x228) = data + 0x44;
		U32(st, 0x22C) = data + 0x84;
		uint32_t relocated = U32(data, 0);
		U32(st, 0x230) = data + 0xC4;
		if (relocated == 0)
		{
			a_740210(data);
			st = MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr));
			U32(data, 0) = 1;
		}
		uint16_t ntargets = U16(root, 0x5A);
		U16(st, 0x1E) = caster;
		U16(st, 0x1C) = ntargets;
		if ((int16_t)ntargets > 0)
		{
			int32_t i = 0;
			do
			{
				int32_t action = S8(node, 0x2A);
				uint32_t tab = U32(U32(node, 0x0C), 4);
				uint32_t rec = U32(tab + (uint32_t)mul32(action, 5) * 4, 8);
				U32(st, 4 + 4 * i) = U8(rec, 0x18 * i);
				i++;
			} while (i < S16(st, 0x1C));
		}
		U16(st, 0x1A) = U16(node, 0x29E);
		dk_8571B0();
		st = MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr));
		if (U16(st, 0x24) == 4)
			callp(game_addr(game::MAG_037_ResetTrailHistory), node);
		dk_857500();
		st = MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr));
		if (U16(st, 0x24) == 1)
		{
			callp(game_addr(game::MAG_037_FillTrailRows), node);
			st = MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr));
		}
		if (U16(st, 0x24) == 3)
		{
			a_73FE90(node);
			st = MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr));
		}
		int32_t s1 = S16(st, 0x1E);
		uint32_t e1 = game_addr(game::BattleEntitySlotData) + (uint32_t)mul32(s1, 0x9C);
		uint32_t d10 = U32(e1, 0x1C);
		uint32_t d14 = U32(e1, 0x20);
		uint32_t s2 = U32(st, 4);
		uint32_t e2 = game_addr(game::BattleEntitySlotData) + s2 * 0x9C;
		uint32_t d18 = U32(e2, 0x1C);
		int32_t dx = (int16_t)(uint16_t)((uint16_t)d10 - (uint16_t)d18);
		int32_t dy = (int16_t)(uint16_t)((uint16_t)(d10 >> 16) - (uint16_t)(d18 >> 16));
		int32_t dz = (int16_t)(uint16_t)((uint16_t)d14 - (uint16_t)U32(e2, 0x20));
		int32_t sq = add32(add32(mul32(dx, dx), mul32(dy, dy)), mul32(dz, dz));
		uint32_t dist = x::Sqrt((uint32_t)sq);
		st = MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr));
		U32(st, 0) = dist;
		return 0;
	}

	uint32_t __cdecl dk_8570D0(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr));
		const uint32_t v = arena + 0x1B4;
		U32(v, 0) = (uint32_t)shl32((int32_t)S16(a1, 0x290), 16);
		const int32_t y = S16(a1, 0x292);
		const int32_t z = S16(a1, 0x294);
		U32(arena, 0x1B8) = (uint32_t)shl32(y, 16);
		U32(arena, 0x1BC) = (uint32_t)shl32(z, 16);
		b1_copy16(arena + 0x204, v);
		b1_copy16(arena + 0x1F4, v);
		b1_copy16(arena + 0x1E4, v);
		b1_copy16(arena + 0x1D4, v);
		b1_copy16(arena + 0x1C4, v);
		return U32(v, 0xC);
	}

	uint32_t __cdecl dk_8571B0(void)
	{
		uint32_t st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		const int32_t slot = S16(st, 0x1E);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)mul32(slot, 0x9C);
		alignas(4) uint8_t pos[8] = {};
		const uint32_t pp = P(pos);
		if (U16(st, 0x24) != 2)
		{
			x::GetEffectSpawnPosition(ent, 0xF1, 0, pp);
			st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		}
		const int32_t px = S16(pp, 0);
		U16(pp, 2) = U16(ent, 0x24);
		S32(st, 0x1C4) = shl32(px, 16);
		S32(st, 0x1C8) = shl32(S16(pp, 2), 16);
		S32(st, 0x1CC) = shl32(S16(pp, 4), 16);
		U32(st, 0x1D4) = U32(st, 0x1C4);
		U32(st, 0x1D8) = 0;
		U32(st, 0x1DC) = U32(st, 0x1CC);
		if (U16(st, 0x24) != 2)
		{
			x::GetEffectSpawnPosition(ent, 0xF1, 0, pp);
			st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		}
		S32(st, 0x1E4) = shl32(S16(pp, 0), 16);
		S32(st, 0x1E8) = shl32(S16(pp, 2), 16);
		S32(st, 0x1EC) = shl32(S16(pp, 4), 16);
		if (U16(st, 0x24) != 2)
		{
			x::GetEffectSpawnPosition(ent, 0xF0, 0, pp);
			st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		}
		S32(st, 0x1F4) = shl32(S16(pp, 0), 16);
		S32(st, 0x1F8) = shl32(S16(pp, 2), 16);
		S32(st, 0x1FC) = shl32(S16(pp, 4), 16);
		const uint32_t sx = U32(st, 0x1C4);
		const int32_t h = shl32(S16(ent, 0x3C), 16);
		U32(st, 0x204) = sx;
		S32(st, 0x208) = h;
		U32(st, 0x20C) = U32(st, 0x1CC);
		return 0;
	}

	uint32_t __cdecl dk_857310(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr));
		int32_t i = 0;
		if (S16(arena, 0x1C) <= 0)
			return arena;
		const uint32_t src = arena + 0x194;
		uint32_t dst = arena + 0x94;
		do
		{
			i++;
			b1_copy16(dst - 0x40, src);
			b1_copy16(dst, src);
			b1_copy16(dst + 0x40, src);
			b1_copy16(dst + 0x80, src);
			b1_copy16(dst + 0xC0, src);
			dst += 0x10;
		} while (i < (int32_t)S16(arena, 0x1C));
		return dst;
	}

	uint32_t __cdecl dk_857500(void)
	{
		uint32_t st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		alignas(4) uint8_t pos[8] = {};
		const uint32_t pp = P(pos);
		if (S16(st, 0x1C) > 0)
		{
			uint32_t o = 0;
			for (int32_t i = 0; ; )
			{
				const uint32_t slot = U32(st, 4 + 4 * i);
				const uint32_t ent = game_addr(game::BattleEntitySlotData) + slot * 0x9C;
				if (U16(st, 0x24) != 2)
				{
					x::GetEffectSpawnPosition(ent, 0xF1, 0, pp);
					st = MEM<uint32_t>(game_addr(ACTOR_STATE));
				}
				const int32_t px = S16(pp, 0);
				U16(pp, 2) = U16(ent, 0x24);
				S32(st, o + 0x54) = shl32(px, 16);
				S32(st, o + 0x58) = shl32(S16(pp, 2), 16);
				S32(st, o + 0x5C) = shl32(S16(pp, 4), 16);
				U32(st, o + 0x94) = U32(st, o + 0x54);
				U32(st, o + 0x98) = 0;
				U32(st, o + 0x9C) = U32(st, o + 0x5C);
				if (U16(st, 0x24) != 2)
				{
					x::GetEffectSpawnPosition(ent, 0xF1, 0, pp);
					st = MEM<uint32_t>(game_addr(ACTOR_STATE));
				}
				S32(st, o + 0xD4) = shl32(S16(pp, 0), 16);
				S32(st, o + 0xD8) = shl32(S16(pp, 2), 16);
				S32(st, o + 0xDC) = shl32(S16(pp, 4), 16);
				if (U16(st, 0x24) != 2)
				{
					x::GetEffectSpawnPosition(ent, 0xF0, 0, pp);
					st = MEM<uint32_t>(game_addr(ACTOR_STATE));
				}
				S32(st, o + 0x114) = shl32(S16(pp, 0), 16);
				S32(st, o + 0x118) = shl32(S16(pp, 2), 16);
				S32(st, o + 0x11C) = shl32(S16(pp, 4), 16);
				U32(st, o + 0x154) = U32(st, o + 0x54);
				S32(st, o + 0x158) = shl32(S16(ent, 0x3C), 16);
				U32(st, o + 0x15C) = U32(st, o + 0x5C);
				o += 0x10;
				i++;
				if (i >= S16(st, 0x1C)) break;
			}
		}
		const int32_t n = S16(st, 0x1C);
		int32_t min_x = 0, max_x = 0, max_z = 0, min_z = 0;
		for (int32_t i = 0; i < n; i++)
		{
			const int32_t vx = S32(st, 0x54 + 0x10 * i);
			const int32_t vz = S32(st, 0x5C + 0x10 * i);
			if ((uint16_t)i == 0)
			{
				max_x = vx;
				max_z = vz;
				min_x = vx;
				min_z = vz;
				continue;
			}
			if (vx < min_x) min_x = vx;
			else if (vx > max_x) max_x = vx;
			if (vz < min_z) min_z = vz;
			else if (vz > max_z) max_z = vz;
		}
		S32(st, 0x194) = add32(min_x, max_x) / 2;
		U32(st, 0x198) = 0;
		S32(st, 0x19C) = add32(max_z, min_z) / 2;
		return 0;
	}

	uint32_t __cdecl dk_857970(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::MAG_037_ActorStatePtr), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			dk_85BB10();
			dk_8591C0();
			dk_859080();
			a_735440();
			sys = U32(game_addr(game::MAG_037_ActorStatePtr), 0);
			U16(sys, 0x18) = (uint16_t)(U16(sys, 0x18) + 1);
			if (S16(sys, 0x18) >= S16(sys, 0x1A) && U16(sys, 0x14) == 0 && U16(sys, 0x16) == 0)
				U16(sys, 0x20) = (uint16_t)(U16(sys, 0x20) + 1);
			break;
		case 2:
			done = 1;
			break;
		default:
			break;
		}
		uint16_t c14 = U16(sys, 0x14);
		uint16_t c16 = U16(sys, 0x16);
		U16(game_addr(game::unk_26A8D9A), 0) = (uint16_t)(U16(game_addr(game::unk_26A8D9A), 0) + c14);
		U16(game_addr(game::unk_26A8D8A), 0) = (uint16_t)(U16(game_addr(game::unk_26A8D8A), 0) + c16);
		return done;
	}

	uint32_t __cdecl dk_857CE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		const uint32_t cnt_ptr = U32(ctx, 0x20);
		const uint32_t count = U32(cnt_ptr, 0);
		uint32_t face = cnt_ptr + 4;
		U32(ctx, 0x20) = face;
		const uint32_t vbase = U32(ctx, 4);
		uint32_t cur = a4;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x20) = face;
			return cur;
		}
		uint32_t pk = cur + 0xC;
		uint32_t left = count;
		do
		{
			const uint32_t pcur = cur;
			const uint32_t v2 = vbase + (uint32_t)U16(face, 8) * 4;
			const uint32_t v1 = vbase + (uint32_t)U16(face, 6) * 4;
			const uint32_t v0 = vbase + (uint32_t)U16(face, 4) * 4;
			x::GTE_LoadV012(v0, v1, v2);
			x::GTE_RTPT();
			const uint32_t fl = U32(ctx, 0x1C);
			const uint32_t code = U32(face, 0);
			U32(pcur, 0) = 0x4000000;
			U32(pk, -8) = code;
			if ((fl & 1) != 0)
				U32(pk, -8) = code | 0x2000000;
			if ((fl & 4) != 0)
				U32(pk, -8) &= 0xFDFFFFFFu;
			x::GTE_ReadFLAG(ctx + 0x30);
			if ((U32(ctx, 0x30) & 0x60000) == 0)
			{
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x10) != 0)
				{
					x::GTE_ReadSXY012_Split(pk - 4, pk, pk + 4);
					x::GTE_AVSZ3();
					uint32_t clip = 0;
					if (b1_out(S16(pk, -4), 0xA00)) clip = 1;
					if (b1_out(S16(pk, 0), 0xA00)) clip |= 2;
					if (b1_out(S16(pk, 4), 0xA00)) clip |= 4;
					if (b1_out(S16(pk, -2), 0x6C0)) clip |= 0x10;
					if (b1_out(S16(pk, 2), 0x6C0)) clip |= 0x20;
					if (b1_out(S16(pk, 6), 0x6C0)) clip |= 0x40;
					if ((uint8_t)(clip & 7) != 7 && (uint8_t)(clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if ((U8(ctx, 0x1C) & 0x40) != 0)
						{
							x::set_unk_1CA8A28(pk - 8);
							x::set_dword_1CA8A30(U32(ctx, 0xC));
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pk - 8);
						}
						const int32_t otz = S32(ctx, 0x2C) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)shl32(otz, 2), pcur);
						cur = pcur + 0x14;
						pk += 0x14;
					}
				}
			}
			face += 0xC;
			left--;
		} while (left != 0);
		U32(ctx, 0x20) = face;
		return cur;
	}

	uint32_t __cdecl dk_857EF0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		const uint32_t cnt_ptr = U32(ctx, 0x20);
		const uint32_t count = U32(cnt_ptr, 0);
		uint32_t face = cnt_ptr + 4;
		U32(ctx, 0x20) = face;
		const uint32_t vbase = U32(ctx, 4);
		uint32_t cur = a4;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x20) = face;
			return cur;
		}
		uint32_t pk = cur + 0xC;
		uint32_t left = count;
		do
		{
			const uint32_t pcur = cur;
			const uint32_t v2 = vbase + (uint32_t)U16(face, 8) * 4;
			const uint32_t v1 = vbase + (uint32_t)U16(face, 6) * 4;
			const uint32_t v0 = vbase + (uint32_t)U16(face, 4) * 4;
			x::GTE_LoadV012(v0, v1, v2);
			x::GTE_RTPT();
			const uint32_t fl = U32(ctx, 0x1C);
			const uint32_t code = U32(face, 0);
			U32(pcur, 0) = 0x5000000;
			U32(pk, -8) = code;
			if ((fl & 1) != 0)
				U32(pk, -8) = code | 0x2000000;
			if ((fl & 4) != 0)
				U32(pk, -8) &= 0xFDFFFFFFu;
			x::GTE_ReadFLAG(ctx + 0x30);
			if ((U32(ctx, 0x30) & 0x60000) == 0)
			{
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x10) != 0)
				{
					x::GTE_ReadSXY012_Split(pk - 4, pk, pk + 4);
					const uint32_t v3 = vbase + (uint32_t)U16(face, 0xA) * 4;
					x::GTE_LoadV0(v3);
					x::GTE_RTPS();
					uint32_t clip = 0;
					if (b1_out(S16(pk, -4), 0xA00)) clip = 1;
					if (b1_out(S16(pk, 0), 0xA00)) clip |= 2;
					if (b1_out(S16(pk, 4), 0xA00)) clip |= 4;
					if (b1_out(S16(pk, -2), 0x6C0)) clip |= 0x10;
					if (b1_out(S16(pk, 2), 0x6C0)) clip |= 0x20;
					if (b1_out(S16(pk, 6), 0x6C0)) clip |= 0x40;
					x::GTE_ReadSXY2(pk + 8);
					x::GTE_AVSZ4();
					if (b1_out(S16(pk, 8), 0xA00)) clip |= 8;
					if (b1_out(S16(pk, 0xA), 0x6C0)) clip |= 0x80;
					if ((uint8_t)(clip & 0xF) != 0xF && (uint8_t)(clip & 0xF0) != 0xF0)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if ((U8(ctx, 0x1C) & 0x40) != 0)
						{
							x::set_unk_1CA8A28(pk - 8);
							x::set_dword_1CA8A30(U32(ctx, 0xC));
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pk - 8);
						}
						const int32_t otz = S32(ctx, 0x2C) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)shl32(otz, 2), pcur);
						cur = pcur + 0x18;
						pk += 0x18;
					}
				}
			}
			face += 0xC;
			left--;
		} while (left != 0);
		U32(ctx, 0x20) = face;
		return cur;
	}

	uint32_t __cdecl dk_858650(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t cursor = a4;
		uint32_t list = U32(ctx, 0x20);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x20) = rec;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x20) = rec;
			return a4;
		}
		uint32_t pkt = cursor;
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t flags = U32(ctx, 0x1C);
			uint32_t code = U32(rec, 0);
			U32(cursor, 0) = 0x6000000;
			U32(pkt, 4) = code;
			if (flags & 2)
			{
				code |= 0x2000000;
				U32(pkt, 4) = code;
			}
			if (flags & 8)
				U32(pkt, 4) &= 0xFDFFFFFF;
			x::GTE_ReadFLAG(ctx + 0x30);
			if (!(U32(ctx, 0x30) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					x::GTE_AVSZ3();
					if (b2_out_x(S16(pkt, 8))) clip = 1;
					if (b2_out_x(S16(pkt, 0x10))) clip |= 2;
					if (b2_out_x(S16(pkt, 0x18))) clip |= 4;
					if (b2_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (b2_out_y(S16(pkt, 0x12))) clip |= 0x20;
					if (b2_out_y(S16(pkt, 0x1A))) clip |= 0x40;
					if ((uint8_t)(clip & 7) != 7 && (uint8_t)(clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if (U8(ctx, 0x1C) & 0x80)
						{
							x::sub_45E120(rec + 0x0C, rec + 0x10, pkt + 4);
							x::set_dword_1CA8A30(U32(ctx, 0x0C));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0x0C, pkt + 0x14, pkt + 4);
						}
						else
						{
							uint32_t c1 = U32(rec, 0x0C), c2 = U32(rec, 0x10);
							U32(pkt, 0x0C) = c1;
							U32(pkt, 0x14) = c2;
						}
						int32_t z = S32(ctx, 0x2C) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)z * 4, cursor);
						cursor += 0x1C;
						pkt += 0x1C;
					}
				}
			}
			rec += 0x14;
		} while (--count);
		U32(ctx, 0x20) = rec;
		return cursor;
	}

	uint32_t __cdecl dk_858880(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t cursor = a4;
		uint32_t list = U32(ctx, 0x20);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x20) = rec;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x20) = rec;
			return a4;
		}
		uint32_t pkt = cursor;
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t flags = U32(ctx, 0x1C);
			uint32_t code = U32(rec, 0);
			U32(cursor, 0) = 0x8000000;
			U32(pkt, 4) = code;
			if (flags & 2)
			{
				code |= 0x2000000;
				U32(pkt, 4) = code;
			}
			if (flags & 8)
				U32(pkt, 4) &= 0xFDFFFFFF;
			x::GTE_ReadFLAG(ctx + 0x30);
			if (!(U32(ctx, 0x30) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0x0A) * 4);
					x::GTE_RTPS();
					if (b2_out_x(S16(pkt, 8))) clip = 1;
					if (b2_out_x(S16(pkt, 0x10))) clip |= 2;
					if (b2_out_x(S16(pkt, 0x18))) clip |= 4;
					if (b2_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (b2_out_y(S16(pkt, 0x12))) clip |= 0x20;
					if (b2_out_y(S16(pkt, 0x1A))) clip |= 0x40;
					x::GTE_ReadSXY2(pkt + 0x20);
					x::GTE_AVSZ4();
					if (b2_out_x(S16(pkt, 0x20))) clip |= 8;
					if (b2_out_y(S16(pkt, 0x22))) clip |= 0x80;
					if ((uint8_t)(clip & 0xF) != 0xF && (uint8_t)(clip & 0xF0) != 0xF0)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if (U8(ctx, 0x1C) & 0x80)
						{
							x::sub_45E120(rec + 0x0C, rec + 0x10, rec + 0x14);
							x::set_dword_1CA8A30(U32(ctx, 0x0C));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0x0C, pkt + 0x14, pkt + 0x1C);
							x::set_unk_1CA8A28(pkt + 4);
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pkt + 4);
						}
						else
						{
							uint32_t c1 = U32(rec, 0x0C), c2 = U32(rec, 0x10), c3 = U32(rec, 0x14);
							U32(pkt, 0x0C) = c1;
							U32(pkt, 0x14) = c2;
							U32(pkt, 0x1C) = c3;
						}
						int32_t z = S32(ctx, 0x2C) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)z * 4, cursor);
						cursor += 0x24;
						pkt += 0x24;
					}
				}
			}
			rec += 0x18;
		} while (--count);
		U32(ctx, 0x20) = rec;
		return cursor;
	}

	uint32_t __cdecl dk_858B40(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t list = U32(ctx, 0x20);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x20) = rec;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x20) = rec;
			return pkt;
		}
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t flags = U32(ctx, 0x1C);
			uint32_t code = U32(rec, 0);
			U32(pkt, 0) = 0x9000000;
			U32(pkt, 4) = code;
			if (flags & 2)
			{
				code |= 0x2000000;
				U32(pkt, 4) = code;
			}
			if (flags & 8)
				U32(pkt, 4) &= 0xFDFFFFFF;
			uint32_t toff = U32(ctx, 0x18);
			uint32_t uv0 = U32(rec, 0x0C);
			uint32_t uv1 = U32(rec, 0x10);
			U32(pkt, 0x0C) = uv0 + toff;
			uint32_t uv2 = U32(rec, 8) >> 16;
			U32(pkt, 0x18) = uv1 + toff;
			U32(pkt, 0x24) = uv2 + toff;
			x::GTE_ReadFLAG(ctx + 0x30);
			if (!(U32(ctx, 0x30) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t f2 = U32(ctx, 0x1C);
				if (f2 & 0x400)
					U16(pkt, 0x1A) = (uint16_t)(U16(pkt, 0x1A) + U16(ctx, 0x10));
				else if (f2 & 0x100)
					U16(pkt, 0x1A) = U16(ctx, 0x10);
				if (f2 & 0x800)
					U16(pkt, 0x0E) = (uint16_t)(U16(pkt, 0x0E) + U16(ctx, 0x14));
				else if (f2 & 0x200)
					U16(pkt, 0x0E) = U16(ctx, 0x14);
				uint32_t zero = 0;
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x14, pkt + 0x20);
					x::GTE_AVSZ3();
					uint32_t clip = b2_out_x(S16(pkt, 8)) ? 1 : zero;
					if (b2_out_x(S16(pkt, 0x14))) clip |= 2;
					if (b2_out_x(S16(pkt, 0x20))) clip |= 4;
					if (b2_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (b2_out_y(S16(pkt, 0x16))) clip |= 0x20;
					if (b2_out_y(S16(pkt, 0x22))) clip |= 0x40;
					if ((uint8_t)(clip & 7) != 7 && (uint8_t)(clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if (U8(ctx, 0x1C) & 0x80)
						{
							x::sub_45E120(rec + 0x14, rec + 0x18, pkt + 4);
							x::set_dword_1CA8A30(U32(ctx, 0x0C));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0x10, pkt + 0x1C, pkt + 4);
						}
						else
						{
							uint32_t c1 = U32(rec, 0x14), c2 = U32(rec, 0x18);
							U32(pkt, 0x10) = c1;
							U32(pkt, 0x1C) = c2;
						}
						int32_t z = S32(ctx, 0x2C) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)z * 4, pkt);
						pkt += 0x28;
					}
				}
			}
			rec += 0x1C;
		} while (--count);
		U32(ctx, 0x20) = rec;
		return pkt;
	}

	uint32_t __cdecl dk_858DA0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t list = U32(ctx, 0x20);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x20) = rec;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x20) = rec;
			return pkt;
		}
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t flags = U32(ctx, 0x1C);
			uint32_t code = U32(rec, 0);
			U32(pkt, 0) = 0xC000000;
			U32(pkt, 4) = code;
			if (flags & 2)
			{
				code |= 0x2000000;
				U32(pkt, 4) = code;
			}
			if (flags & 8)
				U32(pkt, 4) &= 0xFDFFFFFF;
			uint32_t toff = U32(ctx, 0x18);
			uint32_t uv0 = U32(rec, 0x0C);
			uint32_t uv1 = U32(rec, 0x10);
			U32(pkt, 0x0C) = uv0 + toff;
			uint32_t w23 = (toff << 16) + toff;
			uv1 += toff;
			w23 += U32(rec, 0x14);
			U32(pkt, 0x24) = w23;
			U32(pkt, 0x18) = uv1;
			U32(pkt, 0x30) = w23 >> 16;
			x::GTE_ReadFLAG(ctx + 0x30);
			if (!(U32(ctx, 0x30) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t f2 = U32(ctx, 0x1C);
				if (f2 & 0x400)
					U16(pkt, 0x1A) = (uint16_t)(U16(pkt, 0x1A) + U16(ctx, 0x10));
				else if (f2 & 0x100)
					U16(pkt, 0x1A) = U16(ctx, 0x10);
				if (f2 & 0x800)
					U16(pkt, 0x0E) = (uint16_t)(U16(pkt, 0x0E) + U16(ctx, 0x14));
				else if (f2 & 0x200)
					U16(pkt, 0x0E) = U16(ctx, 0x14);
				uint32_t zero = 0;
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x14, pkt + 0x20);
					x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0x0A) * 4);
					x::GTE_RTPS();
					uint32_t clip = b2_out_x(S16(pkt, 8)) ? 1 : zero;
					if (b2_out_x(S16(pkt, 0x14))) clip |= 2;
					if (b2_out_x(S16(pkt, 0x20))) clip |= 4;
					if (b2_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (b2_out_y(S16(pkt, 0x16))) clip |= 0x20;
					if (b2_out_y(S16(pkt, 0x22))) clip |= 0x40;
					x::GTE_ReadSXY2(pkt + 0x2C);
					x::GTE_AVSZ4();
					if (b2_out_x(S16(pkt, 0x2C))) clip |= 8;
					if (b2_out_y(S16(pkt, 0x2E))) clip |= 0x80;
					if ((uint8_t)(clip & 0xF) != 0xF && (uint8_t)(clip & 0xF0) != 0xF0)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if (U8(ctx, 0x1C) & 0x80)
						{
							x::sub_45E120(rec + 0x18, rec + 0x1C, rec + 0x20);
							x::set_dword_1CA8A30(U32(ctx, 0x0C));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0x10, pkt + 0x1C, pkt + 0x28);
							x::set_unk_1CA8A28(pkt + 4);
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pkt + 4);
						}
						else
						{
							uint32_t c1 = U32(rec, 0x18), c2 = U32(rec, 0x1C), c3 = U32(rec, 0x20);
							U32(pkt, 0x10) = c1;
							U32(pkt, 0x1C) = c2;
							U32(pkt, 0x28) = c3;
						}
						int32_t z = S32(ctx, 0x2C) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)z * 4, pkt);
						pkt += 0x34;
					}
				}
			}
			rec += 0x24;
		} while (--count);
		U32(ctx, 0x20) = rec;
		return pkt;
	}

	uint32_t __cdecl dk_859080(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						dk_859140(act);
					}
					else if (n > 0)
					{
						int32_t i = 0;
						uint32_t sub = act + 0x19E;
						do
						{
							S32(act, 0xC0) = S16(sub, -2);
							S32(act, 0xC4) = S16(sub, 0);
							S32(act, 0xC8) = S16(sub, 2);
							dk_859140(act);
							i++;
							sub += 8;
						} while (i < S8(act, 0x1D8));
					}
				}
			}
			act = U32(act, 4);
		} while (act != 0);
		return 0;
	}

	uint32_t __cdecl dk_859140(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t hdr = MEM<uint32_t>(game_addr(SCRATCH_SP)) - 0xB4;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = hdr;
		const uint32_t mtx = act + 0xAC;
		x::GTE_SetRotMatrix(mtx);
		x::GTE_SetTransVector(mtx);
		const uint32_t seq = U32(act, 0x170);
		const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		const int16_t variant = (int16_t)S8(act, 0x1D0);
		U32(hdr, 0) = seq;
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		S16(hdr, 4) = variant;
		U16(hdr, 0x24) = 0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = x::InitEffectSequenceFromData_c19(hdr, ot, 2, cursor);
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 0xB4;
		return 0;
	}

	uint32_t __cdecl dk_8591C0(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_037_ActorStatePtr), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				dk_85A490(act);
			else if (type == 1)
				dk_859200(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl dk_859200(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (dk_85A340(act, bone) == 0)
			{
				a_740910(act, bone);
				a_741120(act, bone);
				U16(act, 0x1CA) = (uint16_t)(U16(act, 0x1CA) + 1);
				return 0;
			}
			U16(act, 0x1CC) = (uint16_t)(U16(act, 0x1CC) + 1);
			U32(act, 0x170) = 0;
			return 0;
		}
		if (phase == 1)
		{
			a_742CD0(act);
			st = MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl dk_85A230(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::MAG_037_ActorStatePtr), 0);
		uint32_t dst_tab = U32(st, 0x22C);
		uint32_t src_tab = U32(st, 0x230);
		uint32_t dst = U32(dst_tab, i_dst * 4);
		uint32_t i_src1 = U8(U32(data, 0x130), frame);
		uint32_t src1 = U32(src_tab, i_src1 * 4);
		uint32_t i_src2 = U8(U32(data, 0x134), frame);
		uint32_t src2 = U32(src_tab, i_src2 * 4);
		if (src1 == 0 || src2 == 0)
			return 0;
		uint32_t w_blk = x::Field_Alloc(8);
		int32_t frame2 = S16(a1, 0x1CA);
		uint32_t n = U32(dst, 4);
		int32_t w = S16(U32(data, 0x138), frame2 * 2);
		U32(w_blk, 4) = (uint32_t)w;
		U32(w_blk, 0) = (uint32_t)sub32(0x1000, w);
		if ((int32_t)n > 0)
		{
			uint32_t out = dst + 8;
			uint32_t in2 = src2 + 8;
			uint32_t delta = src1 - src2;
			do
			{
				x::set_dword_1CA8A30(U32(w_blk, 0));
				gx::sub_45E0B0(in2 + delta);
				gx::sub_45E9D0();
				x::set_dword_1CA8A30(U32(w_blk, 4));
				gx::sub_45E0B0(in2);
				gx::sub_45EBF0();
				x::GTE_StoreIR123(out);
				out += 8;
				in2 += 8;
			} while (--n != 0);
		}
		x::Field_Free(8);
		return 0;
	}

	uint32_t __cdecl dk_85A340(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				dk_85A3C0(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			dk_85A3C0(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			dk_85A3C0(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl dk_85A3C0(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return dk_85A3F0(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl dk_85A3F0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_037_ParticlePoolPtr));
		int32_t idx = MEM<int16_t>(game_addr(game::MAG_037_ParticleCursor));
		uint32_t slot = 0;
		int32_t tries = 0;
		do
		{
			if (U8(pool + (uint32_t)(idx * 27) * 4, 0x69) == 0)
			{
				slot = pool + (uint32_t)(idx * 27) * 4;
				x::MAG_007_sub_8DCC00(slot, 0x6C);
				U8(slot, 0x6A) = (uint8_t)a2;
				const uint32_t dir = MEM<uint32_t>(game_addr(ACTOR_STATE));
				U8(slot, 0x69) = 1;
				U16(dir, 0x14) = (uint16_t)(U16(dir, 0x14) + 1);
				U32(slot, 0x5C) = a1;
				U8(slot, 0x6B) = (uint8_t)a3;
				a_742CA0(slot, 0);
				break;
			}
			idx++;
			if (idx >= 9)
				idx = 0;
			tries++;
		} while (tries < 0xA);
		idx++;
		if (idx < 9)
			MEM<uint16_t>(game_addr(game::MAG_037_ParticleCursor)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::MAG_037_ParticleCursor)) = 0;
		return slot;
	}

	uint32_t __cdecl dk_85A490(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_037_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_037_Actor_InterpolatePosition), a1);
		dk_85A510(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl dk_85A510(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::MAG_037_ActorStatePtr), 0);
		int32_t k = S8(a1, 0x6A);
		uint32_t def = U32(U32(sys, 0x224), k * 4);
		int16_t frame = S16(a1, 0x60);
		if (frame > 0x27)
			return 0;
		uint32_t total = U8(U32(def, 0xC8), (int32_t)frame);
		if (total == 0)
			return 0;
		int32_t batch = S8(def, 0x36);
		int32_t full = (int32_t)total / batch;
		int32_t rest = (int32_t)total % batch;
		if (full > 0)
		{
			do
			{
				int32_t b = S8(def, 0x36);
				dk_85A590(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			dk_85A590(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl dk_85A590(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::MAG_037_ScratchSP), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::MAG_037_ScratchSP), 0) = sp;
		uint32_t node = dk_85AD70(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::MAG_037_ActorStatePtr), 0);
			int32_t k = S8(a1, 0x6A);
			uint32_t v = U32(U32(ctx, 0x228), shl32(k, 2));
			U32(node, 0x170) = v;
			U8(node, 0x1D1) = (uint8_t)a_7424B0(v);
			if (U8(desc, 0x16) == 2)
			{
				uint8_t b17 = U8(desc, 0x17);
				uint8_t b18 = U8(desc, 0x18);
				int32_t s1a = S8(desc, 0x1A);
				U8(node, 0x1D3) = b17;
				int32_t s19 = S8(desc, 0x19);
				U8(node, 0x1D4) = b18;
				U8(node, 0x1D5) = (uint8_t)a_742290((uint32_t)s19, (uint32_t)s1a);
			}
		}
		U16(node, 0x1DA) = U16(desc, 0x64);
		if ((int32_t)a3 > 0)
		{
			uint32_t src = a1 + 0x4C;
			uint32_t emat = a1 + 0x2C;
			uint32_t out_speed = node + 0x184;
			uint32_t mat = node + 0x0C;
			uint32_t pos = node + 0x12C;
			for (int32_t n = (int32_t)a3; n != 0; n--)
			{
				e6_copy16(pos - 0x50, src);
				if (U8(desc, 0x22) == 0)
					e6_copy16(pos, src);
				uint8_t spread = U8(desc, 0x34);
				if (spread == 1)
				{
					U16(sp, 0x28) = (uint16_t)(x::CrtRand() & 0xFFF);
					U16(sp, 0x2A) = (uint16_t)(x::CrtRand() & 0xFFF);
					U16(sp, 0x2C) = (uint16_t)(x::CrtRand() & 0xFFF);
					e6_rotate_zxy(sp, sp + 0x28);
					int32_t i2 = shl32(S16(sp, 0x4E), 1);
					if (U8(desc, 0x33) != 0)
					{
						U16(sp, 0x48) = U16(U32(desc, 0xCC), i2);
						U16(sp, 0x4A) = U16(U32(desc, 0xD0), i2);
						U16(sp, 0x4C) = U16(U32(desc, 0xD4), i2);
					}
					else
					{
						U16(sp, 0x48) = U16(U32(desc, 0xCC), i2);
						U16(sp, 0x4A) = U16(U32(desc, 0xD0), i2);
						U16(sp, 0x4C) = U16(U32(desc, 0xD4), i2);
						e6_rand_mod(sp, 0x48);
						e6_rand_mod(sp, 0x4A);
						e6_rand_mod(sp, 0x4C);
					}
					e6_rotate_down_vector(sp);
					int32_t ex = mul32(S16(sp, 0x48), S16(sp, 0x20));
					int32_t ey = mul32(S16(sp, 0x4A), S16(sp, 0x22));
					int32_t ez = mul32(S16(sp, 0x4C), S16(sp, 0x24));
					int32_t old = S32(pos, 0);
					ex = shl32(ex, 4);
					S32(sp, 0x38) = ex;
					S32(pos, 0) = add32(old, ex);
					old = S32(pos, 4);
					ey = shl32(ey, 4);
					S32(sp, 0x3C) = ey;
					S32(pos, 4) = add32(old, ey);
					old = S32(pos, 8);
					ez = shl32(ez, 4);
					S32(sp, 0x40) = ez;
					S32(pos, 8) = add32(old, ez);
				}
				else if (spread == 2)
				{
					U16(sp, 0x28) = (uint16_t)(x::CrtRand() & 0xFFF);
					U16(sp, 0x2A) = 0;
					U16(sp, 0x2C) = 0x400;
					e6_rotate_zxy(sp, sp + 0x28);
					int32_t i2 = shl32(S16(sp, 0x4E), 1);
					if (S8(desc, 0x33) != 1)
					{
						U16(sp, 0x48) = U16(U32(desc, 0xCC), i2);
						U16(sp, 0x4A) = U16(U32(desc, 0xD0), i2);
						U16(sp, 0x4C) = U16(U32(desc, 0xD4), i2);
						e6_rand_mod(sp, 0x48);
						e6_rand_mod(sp, 0x4A);
						e6_rand_mod(sp, 0x4C);
					}
					else
					{
						U16(sp, 0x48) = U16(U32(desc, 0xCC), i2);
						U16(sp, 0x4A) = U16(U32(desc, 0xD0), i2);
						e6_rand_mod(sp, 0x4A);
						int32_t e = S16(sp, 0x4E);
						U16(sp, 0x4C) = U16(U32(desc, 0xD4), shl32(e, 1));
					}
					if ((x::CrtRand() & 1) != 0)
						U16(sp, 0x4A) = (uint16_t)(0 - U16(sp, 0x4A));
					e6_rotate_down_vector(sp);
					int32_t ex = mul32(S16(sp, 0x48), S16(sp, 0x20));
					int32_t ey = shl32(S16(sp, 0x4A), 16);
					int32_t oldx = S32(pos, 0);
					S32(sp, 0x3C) = ey;
					int32_t ez = mul32(S16(sp, 0x4C), S16(sp, 0x24));
					ex = shl32(ex, 4);
					S32(sp, 0x38) = ex;
					S32(pos, 0) = add32(oldx, ex);
					int32_t ey2 = S32(sp, 0x3C);
					S32(pos, 4) = add32(S32(pos, 4), ey2);
					int32_t oldz = S32(pos, 8);
					ez = shl32(ez, 4);
					S32(sp, 0x40) = ez;
					S32(pos, 8) = add32(oldz, ez);
				}
				int32_t rmode = S8(desc, 0x21);
				if (rmode == 0)
					a_742350(sp + 0x30, desc + 0x3C, desc + 0x44);
				else if (rmode == 1)
				{
					uint32_t c0 = U32(sp, 0x28);
					uint32_t c1 = U32(sp, 0x2C);
					U32(sp, 0x30) = c0;
					U32(sp, 0x34) = c1;
				}
				e6_rotate_zxy(mat, sp + 0x30);
				x::GTE_SetRotMatrix(emat);
				x::GTE_LoadIRFromMatrixColumn(mat);
				x::GTE_MVMVA_RotIR();
				x::GTE_StoreIRToMatrixColumn(mat);
				x::GTE_LoadIRFromMatrixColumn(mat + 2);
				x::GTE_MVMVA_RotIR();
				x::GTE_StoreIRToMatrixColumn(mat + 2);
				x::GTE_LoadIRFromMatrixColumn(mat + 4);
				x::GTE_MVMVA_RotIR();
				x::GTE_StoreIRToMatrixColumn(mat + 4);
				if (U8(desc, 0x27) == 0)
				{
					uint32_t r1 = a_7422C0(U32(desc, 0x4C), U32(desc, 0x50));
					U32(out_speed, -0x10) = r1;
					uint32_t r2 = a_7422C0(U32(desc, 0x54), U32(desc, 0x58));
					U32(out_speed, 0) = r2;
				}
				out_speed += 4;
				pos += 0x10;
				mat += 0x20;
			}
		}
		a_742350(node + 0x194, desc + 0x148, desc + 0x150);
		uint8_t m1e = U8(desc, 0x1E);
		if (m1e == 1 || m1e == 2)
		{
			int32_t s20 = S8(desc, 0x20);
			int32_t s1f = S8(desc, 0x1F);
			U16(node, 0x1C8) = (uint16_t)a_742290((uint32_t)s1f, (uint32_t)s20);
		}
		if (U8(desc, 0x27) == 0)
		{
			U32(node, 0x1DC) = a_7422C0(U32(desc, 0x5C), U32(desc, 0x60));
			if ((int32_t)a3 > 0)
			{
				uint32_t q = node + 0x220;
				for (int32_t n = (int32_t)a3; n != 0; n--)
				{
					a_742300(q - 0x40, desc + 0x68, desc + 0x78);
					a_742300(q, desc + 0x88, desc + 0x98);
					a_742300(q + 0x40, desc + 0xA8, desc + 0xB8);
					q += 0x10;
				}
			}
		}
		dk_85BA80(node, desc);
		U32(game_addr(game::MAG_037_ScratchSP), 0) = U32(game_addr(game::MAG_037_ScratchSP), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl dk_85AD70(uint32_t a1, uint32_t a2)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_037_ActorPoolPtr));
		int32_t i = MEM<int16_t>(game_addr(game::MAG_037_ActorCursor));
		uint32_t slot = 0;
		int32_t tries = 0;
		for (;;)
		{
			const uint32_t off = (uint32_t)mul32(i, 0x2A0);
			if (U8(off + pool, 0x1D7) == 0)
			{
				slot = (uint32_t)mul32(i, 0x2A0) + pool;
				x::MAG_007_sub_8DCC00(slot, 0x2A0);
				const uint8_t def_idx = (uint8_t)a2;
				U32(slot, 0x1BC) = a1;
				const uint8_t b6b = U8(a1, 0x6B);
				const uint32_t sys = MEM<uint32_t>(game_addr(ACTOR_STATE));
				U8(slot, 0x1D7) = 1;
				U16(sys, 0x16) = (uint16_t)(U16(sys, 0x16) + 1);
				U8(slot, 0x1D6) = def_idx;
				U8(slot, 0x1D9) = b6b;
				a_742CA0(slot, 1);
				break;
			}
			i++;
			if (i >= 0x27)
				i = 0;
			tries++;
			if (tries >= 0x28)
				break;
		}
		i++;
		if (i >= 0x27)
			MEM<uint16_t>(game_addr(game::MAG_037_ActorCursor)) = 0;
		else
			MEM<uint16_t>(game_addr(game::MAG_037_ActorCursor)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl dk_85AF90(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::MAG_037_ActorStatePtr), 0);
		int32_t aidx = S8(act, 0x6A);
		int32_t key = S16(act, 0x60);
		const uint32_t data = U32(U32(st, 0x224), aidx * 4);
		uint8_t sel = U8(U32(data, 0x108), key);
		uint8_t kind = U8(U32(data, 0x100), key);
		int16_t ax = 0, ay = 0, az = 0;
		uint32_t base = 0;
		bool have = false;
		if (kind == 0)
		{
			if (sel <= 3)
			{
				static const uint32_t offs0[4] = { 0x94, 0xD4, 0x114, 0x154 };
				base = st + (uint32_t)((int32_t)S8(act, 0x6B) << 4) + offs0[sel];
				have = true;
			}
		}
		else if (kind == 1)
		{
			if (sel <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				base = st + offs1[sel];
				have = true;
			}
		}
		if (have)
		{
			int32_t vx = b2_fx(U32(base, 0));
			int32_t vy = b2_fx(U32(base, 4));
			ay = (int16_t)vy;
			int32_t vz = b2_fx(U32(base, 8));
			az = (int16_t)vz;
			ax = (int16_t)vx;
		}
		int16_t v[4];
		int16_t ir[8] = {};
		v[0] = (int16_t)U16(U32(data, 0x110), key * 2);
		v[1] = (int16_t)U16(U32(data, 0x114), key * 2);
		v[2] = (int16_t)U16(U32(data, 0x118), key * 2);
		v[3] = 0;
		x::GTE_SetRotMatrix(act + 0x2C);
		x::GTE_LoadV0(P(v));
		x::GTE_MVMVA_RotV0();
		x::GTE_StoreIR123(P(ir));
		az = (int16_t)(az + ir[2]);
		ay = (int16_t)(ay + ir[1]);
		const int32_t key2 = S16(act, 0x60);
		uint8_t kind2 = U8(U32(data, 0x104), key2);
		uint8_t sel2 = U8(U32(data, 0x10C), key2);
		ax = (int16_t)(ax + ir[0]);
		int16_t bx, by, bz;
		uint32_t base2 = 0;
		bool have2 = false;
		if (kind2 == 0)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs0[4] = { 0x94, 0xD4, 0x114, 0x154 };
				uint32_t g = U32(game_addr(game::MAG_037_ActorStatePtr), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::MAG_037_ActorStatePtr), 0);
				base2 = g + offs1[sel2];
				have2 = true;
			}
		}
		if (have2)
		{
			int32_t vx = b2_fx(U32(base2, 0));
			int32_t vy = b2_fx(U32(base2, 4));
			int32_t vz = b2_fx(U32(base2, 8));
			bx = (int16_t)vx;
			by = (int16_t)vy;
			bz = (int16_t)vz;
		}
		else
		{
			bz = ir[2];
			by = ir[1];
			bx = ir[0];
		}
		v[0] = (int16_t)U16(U32(data, 0x11C), key2 * 2);
		v[1] = (int16_t)U16(U32(data, 0x120), key2 * 2);
		v[2] = (int16_t)U16(U32(data, 0x124), key2 * 2);
		x::GTE_SetRotMatrix(act + 0x2C);
		x::GTE_LoadV0(P(v));
		x::GTE_MVMVA_RotV0();
		x::GTE_StoreIR123(P(ir));
		bx = (int16_t)(bx + ir[0]);
		by = (int16_t)(by + ir[1]);
		const int32_t key3 = S16(act, 0x60);
		const uint32_t ttab = U32(data, 0x128);
		bz = (int16_t)(bz + ir[2]);
		const int32_t t = S16(ttab, key3 * 2);
		int32_t a = ax;
		S32(act, 0x4C) = shl32(add32(mul32(sub32(bx, a), t), shl32(a, 12)), 4);
		a = ay;
		S32(act, 0x50) = shl32(add32(mul32(sub32(by, a), t), shl32(a, 12)), 4);
		a = az;
		S32(act, 0x54) = shl32(add32(mul32(sub32(bz, a), t), shl32(a, 12)), 4);
		return 0;
	}

	uint32_t __cdecl dk_85BA80(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			dk_85A3F0(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			dk_85A3F0(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			dk_85A3F0(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			dk_85A3F0(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl dk_85BB10(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr));
		for (int32_t slot = 0; slot < 0x10; slot++)
		{
			uint32_t table = U32(dir, 0x224);
			uint32_t entry = U32(table, slot * 4);
			if (entry == 0)
				continue;
			uint32_t bit = (uint32_t)shl32(1, slot);
			uint32_t mask = U16(dir, 0x22);
			if (bit & mask)
				continue;
			uint8_t kind = U8(entry, 0x11);
			if (kind != 0 && kind != 1 && kind != 4)
				continue;
			int16_t group = (int16_t)S8(entry, 0x13);
			if (group != S16(dir, 0x18))
				continue;
			if (U8(entry, 0x37) == 1)
			{
				int32_t n = 0;
				if (S16(dir, 0x1C) <= 0)
					continue;
				do
				{
					dk_85A3F0(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				dk_85A3F0(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::MAG_037_ActorStatePtr));
			}
		}
		return 0;
	}

	uint32_t __cdecl dk_85BBA0(uint32_t a1)
	{
		if (dk_857970(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl dk_85BC90(uint32_t a1, uint32_t a2)
	{
		int32_t range = (int16_t)a2;
		if ((int16_t)a2 == 0)
			range = 1;
		const uint32_t r1 = x::CrtRand() & 0xFFF;
		const uint32_t r2 = x::CrtRand() & 0xFFF;
		alignas(4) uint8_t m[0x20];
		alignas(4) uint8_t v[8];
		alignas(4) uint8_t o[8];
		x::MAG_022_sub_8DD770(P(m));
		x::MAG_022_sub_8DD8A0(P(m), (uint32_t)(int32_t)(int16_t)r1);
		x::sub_8DD7E0(P(m), (uint32_t)(int32_t)(int16_t)r2);
		U16(P(v), 0) = 0;
		U16(P(v), 2) = 0;
		const int32_t r3 = (int32_t)x::CrtRand();
		U16(P(v), 4) = (uint16_t)(r3 % (int32_t)(int16_t)range);
		x::matrixMultiplyVector(P(m), P(v), P(o));
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + (uint16_t)(U32(P(o), 0) * 2));
		U16(a1, 0x1E) = (uint16_t)(U16(a1, 0x1E) + U16(P(o), 2));
		U16(a1, 0x20) = (uint16_t)(U16(a1, 0x20) + (uint16_t)(U32(P(o), 4) * 2));
		return a1;
	}

	uint32_t __cdecl dk_85BE30(uint32_t a1)
	{
		if ((U8(a1, 0x26) & 4) != 0)
			return 0;
		const uint32_t hdr = x::Field_Alloc(0xB4);
		gx::TransformCameraByShadowRotation(a1 + 0x1C, 0x1000, (uint32_t)(int32_t)S16(a1, 0x54));
		U32(hdr, 0) = U32(a1, 0x4C);
		const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		U16(hdr, 4) = U16(a1, 0x50);
		U16(hdr, 0x24) = 0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = gx::InitEffectSequenceFromData(hdr, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, cursor);
		x::Field_Free(0xB4);
		return 0;
	}

	uint32_t __cdecl dk_85BEA0(uint32_t a1)
	{
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 1);
		if (S16(a1, 0x50) > S16(a1, 0x52))
			U16(a1, 0x50) = 0;
		return a1;
	}

	uint32_t __cdecl dk_85C480(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t hdr = a1;
		if ((U32(hdr, 0x14) & 0x2000) == 0)
			U32(hdr, 4) = U32(hdr, 0) + 8;
		{
			const uint32_t model = U32(hdr, 0);
			const uint32_t b = U8(hdr, 0xA);
			U32(hdr, 0x2C) = U32(model, 0) + model;
			const uint32_t g = U8(hdr, 9);
			const uint32_t r = U8(hdr, 8);
			x::someCameraWork_45DD60(r, g, b);
		}
		uint32_t cursor = a4;
		typedef uint32_t (__cdecl *Draw)(uint32_t, uint32_t, uint32_t, uint32_t);
		static const Draw lists[8] = { dk_85C5B0, dk_85C7D0, dk_85CA60, dk_85CEE0, a_73D770, a_73D9C0, gx::MAG_037_render, dk_85DE90 };
		for (int i = 0; i < 8; i++)
		{
			const uint32_t list = U32(hdr, 0x2C);
			if (U32(list, 0) != 0)
				cursor = lists[i](hdr, a2, a3, cursor);
			else
				U32(hdr, 0x2C) = list + 4;
		}
		return cursor;
	}

	uint32_t __cdecl dk_85C5B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t cursor = a4;
		const uint32_t list = U32(ctx, 0x2C);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		U32(ctx, 0x2C) = rec;
		const uint32_t vbase = U32(ctx, 4);
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return a4;
		}
		uint32_t pkt = cursor + 0xC;
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			const uint32_t flags = U32(ctx, 0x14);
			uint32_t code = U32(rec, 0);
			U32(cursor, 0) = 0x4000000;
			U32(pkt, -8) = code;
			if (flags & 1)
			{
				code |= 0x2000000;
				U32(pkt, -8) = code;
			}
			if (flags & 4)
				U32(pkt, -8) &= 0xFDFFFFFF;
			x::GTE_ReadFLAG(ctx + 0x3C);
			if (!(U32(ctx, 0x3C) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) >= 0 || (U8(ctx, 0x14) & 0x10))
				{
					x::GTE_ReadSXY012_Split(pkt - 4, pkt, pkt + 4);
					x::GTE_AVSZ3();
					if (t1_out_x(S16(pkt, -4))) clip = 1;
					if (t1_out_x(S16(pkt, 0))) clip |= 2;
					if (t1_out_x(S16(pkt, 4))) clip |= 4;
					if (t1_out_y(S16(pkt, -2))) clip |= 0x10;
					if (t1_out_y(S16(pkt, 2))) clip |= 0x20;
					if (t1_out_y(S16(pkt, 6))) clip |= 0x40;
					if ((uint8_t)(clip & 7) != 7 && (uint8_t)(clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x38);
						if (U8(ctx, 0x14) & 0x40)
						{
							x::set_unk_1CA8A28(pkt - 8);
							x::set_dword_1CA8A30(U32(ctx, 0x0C));
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pkt - 8);
						}
						int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
						S32(ctx, 0x38) = z;
						if (z < 0)
							S32(ctx, 0x38) = 0;
						z = S32(ctx, 0x38) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)z * 4, cursor);
						cursor += 0x14;
						pkt += 0x14;
					}
				}
			}
			rec += 0xC;
		} while (--count);
		U32(ctx, 0x2C) = rec;
		return cursor;
	}

	uint32_t __cdecl dk_85C7D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t rec = U32(ctx, 0x2C);
		int32_t count = S32(rec, 0);
		rec += 4;
		U32(ctx, 0x2C) = rec;
		const uint32_t vbase = U32(ctx, 4);
		if (count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return pkt;
		}
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4u, vbase + (uint32_t)U16(rec, 6) * 4u,
				vbase + (uint32_t)U16(rec, 8) * 4u);
			x::GTE_RTPT();
			{
				const uint32_t flags = U32(ctx, 0x14);
				const uint32_t code = U32(rec, 0);
				U32(pkt, 0) = 0x5000000;
				U32(pkt, 4) = code;
				if (flags & 1)
					U32(pkt, 4) = code | 0x2000000;
				if (flags & 4)
					U32(pkt, 4) = U32(pkt, 4) & 0xFDFFFFFFu;
			}
			x::GTE_ReadFLAG(ctx + 0x3C);
			if ((U32(ctx, 0x3C) & 0x60000) != 0)
				goto next;
			x::GTE_NCLIP();
			{
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) < 0 && !(U8(ctx, 0x14) & 0x10))
					goto next;
				x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0xC, pkt + 0x10);
				x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0xA) * 4u);
				x::GTE_RTPS();
				if (t2_outx(pkt + 8))
					clip = 1;
				if (t2_outx(pkt + 0xC))
					clip |= 2;
				if (t2_outx(pkt + 0x10))
					clip |= 4;
				if (t2_outy(pkt + 0xA))
					clip |= 0x10;
				if (t2_outy(pkt + 0xE))
					clip |= 0x20;
				if (t2_outy(pkt + 0x12))
					clip |= 0x40;
				x::GTE_ReadSXY2(pkt + 0x14);
				x::GTE_AVSZ4();
				if (t2_outx(pkt + 0x14))
					clip |= 8;
				if (t2_outy(pkt + 0x16))
					clip |= 0x80;
				if ((clip & 0xF) == 0xF)
					goto next;
				if ((clip & 0xF0) == 0xF0)
					goto next;
			}
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x40)
				t2_depth_cue(ctx, pkt + 4);
			{
				const uint32_t ot = t2_ot(ctx, a2, a3);
				x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
				pkt += 0x18;
			}
		next:
			rec += 0xC;
			--count;
		} while (count != 0);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl dk_85CA60(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t rec = U32(ctx, 0x2C);
		int32_t count = S32(rec, 0);
		rec += 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = rec;
		if (count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return pkt;
		}
		uint32_t pSXY2 = pkt + 0x18;
		uint32_t pSXY1 = pkt + 0x10;
		uint32_t pCode = pkt + 4;
		uint32_t pSXY0 = pkt + 8;
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4u, vbase + (uint32_t)U16(rec, 6) * 4u,
				vbase + (uint32_t)U16(rec, 8) * 4u);
			x::GTE_RTPT();
			{
				const uint32_t flags = U32(ctx, 0x14);
				const uint32_t code = U32(rec, 0);
				U32(pkt, 0) = 0x7000000;
				U32(pCode, 0) = code;
				if (flags & 1)
					U32(pCode, 0) = code | 0x2000000;
				if (flags & 4)
					U32(pCode, 0) = U32(pCode, 0) & 0xFDFFFFFFu;
				const uint32_t uv2 = U32(rec, 8) >> 16;
				U32(pkt, 0xC) = U32(rec, 0xC);
				U32(pkt, 0x14) = U32(rec, 0x10);
				U32(pkt, 0x1C) = uv2;
			}
			x::GTE_ReadFLAG(ctx + 0x3C);
			if ((U32(ctx, 0x3C) & 0x60000) != 0)
				goto next;
			x::GTE_NCLIP();
			{
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) < 0 && !(U8(ctx, 0x14) & 0x10))
					goto next;
				x::GTE_ReadSXY012_Split(pSXY0, pSXY1, pSXY2);
				x::GTE_AVSZ3();
				int16_t v = S16(pSXY0, 0);
				if (v < 0 || v > 0xA00)
					clip = 1;
				v = S16(pSXY1, 0);
				if (v < 0 || v > 0xA00)
					clip |= 2;
				v = S16(pSXY2, 0);
				if (v < 0 || v > 0xA00)
					clip |= 4;
				v = S16(pkt, 0xA);
				if (v < 0 || v > 0x6C0)
					clip |= 0x10;
				v = S16(pkt, 0x12);
				if (v < 0 || v > 0x6C0)
					clip |= 0x20;
				v = S16(pkt, 0x1A);
				if (v < 0 || v > 0x6C0)
					clip |= 0x40;
				if ((clip & 7) == 7)
					goto next;
				if ((clip & 0x70) == 0x70)
					goto next;
			}
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x40)
			{
				x::set_unk_1CA8A28(pCode);
				x::set_dword_1CA8A30(U32(ctx, 0xC));
				x::sub_45F270();
				x::set_param_with_dword_1CA8A68(pCode);
			}
			{
				const int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
				S32(ctx, 0x38) = z;
				if (z < 0)
					S32(ctx, 0x38) = 0;
				const uint32_t ot = a2 + (uint32_t)(S32(ctx, 0x38) >> (a3 & 31)) * 4u;
				const uint16_t add0 = U16(ctx, 0x18);
				const uint16_t add1 = U16(ctx, 0x1A);
				if ((uint16_t)(add0 | add1) == 0)
				{
					x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
					pSXY0 += 0x20;
					pkt += 0x20;
					pSXY1 += 0x20;
					pSXY2 += 0x20;
					pCode += 0x20;
				}
				else
				{
					if (add0 != 0)
					{
						const uint32_t a = add0;
						const uint32_t c2 = U8(pkt, 0x1C) + a;
						const uint32_t c1 = U8(pkt, 0x14) + a;
						const uint32_t c0 = U8(pkt, 0xC) + a;
						if ((int32_t)(c2 | c1 | c0) > 0xFF)
						{
							const uint8_t s = U8(ctx, 0x28);
							U8(pkt, 0xC) = (uint8_t)(c0 - s);
							U8(pkt, 0x14) = (uint8_t)(c1 - s);
							U8(pkt, 0x1C) = (uint8_t)(c2 - s);
						}
						else
						{
							U8(pkt, 0xC) = (uint8_t)c0;
							U8(pkt, 0x14) = (uint8_t)c1;
							U8(pkt, 0x1C) = (uint8_t)c2;
						}
					}
					const uint16_t add1b = U16(ctx, 0x1A);
					if (add1b != 0)
					{
						const uint32_t a = add1b;
						const uint32_t c2 = U8(pkt, 0x1D) + a;
						const uint32_t c1 = U8(pkt, 0x15) + a;
						const uint32_t c0 = U8(pkt, 0xD) + a;
						if ((int32_t)(c2 | c1 | c0) > 0xFF)
						{
							const uint8_t s = U8(ctx, 0x2A);
							U8(pkt, 0xD) = (uint8_t)(c0 - s);
							U8(pkt, 0x15) = (uint8_t)(c1 - s);
							U8(pkt, 0x1D) = (uint8_t)(c2 - s);
						}
						else
						{
							U8(pkt, 0xD) = (uint8_t)c0;
							U8(pkt, 0x15) = (uint8_t)c1;
							U8(pkt, 0x1D) = (uint8_t)c2;
						}
					}
					const uint32_t poly = pkt;
					pSXY1 += 0x2C;
					pSXY2 += 0x2C;
					pCode += 0x2C;
					pSXY0 += 0x2C;
					uint32_t prim = pkt + 0x20;
					pkt += 0x2C;
					U32(prim, 0) = 0x2000000;
					uint32_t tw = 0;
					if (ctx + 0x1C != 0)
						tw = s2_twin(ctx, 0x1C);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
					pSXY0 += 0xC;
					pSXY1 += 0xC;
					prim = pkt;
					pkt += 0xC;
					pSXY2 += 0xC;
					pCode += 0xC;
					U32(prim, 0) = 0x2000000;
					tw = 0;
					if (ctx + 0x24 != 0)
						tw = s2_twin(ctx, 0x24);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
				}
			}
		next:
			rec += 0x14;
			--count;
		} while (count != 0);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl dk_85CEE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t rec = U32(ctx, 0x2C);
		int32_t count = S32(rec, 0);
		rec += 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = rec;
		if (count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return pkt;
		}
		uint32_t pSXY3 = pkt + 0x20;
		uint32_t pSXY2 = pkt + 0x18;
		uint32_t pSXY1 = pkt + 0x10;
		uint32_t pCode = pkt + 4;
		uint32_t pSXY0 = pkt + 8;
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4u, vbase + (uint32_t)U16(rec, 6) * 4u,
				vbase + (uint32_t)U16(rec, 8) * 4u);
			x::GTE_RTPT();
			{
				const uint32_t flags = U32(ctx, 0x14);
				const uint32_t code = U32(rec, 0);
				U32(pkt, 0) = 0x9000000;
				U32(pCode, 0) = code;
				if (flags & 1)
					U32(pCode, 0) = code | 0x2000000;
				if (flags & 4)
					U32(pCode, 0) = U32(pCode, 0) & 0xFDFFFFFFu;
				U32(pkt, 0xC) = U32(rec, 0xC);
				const uint32_t uv1 = U32(rec, 0x10);
				const uint32_t uv23 = U32(rec, 0x14);
				U32(pkt, 0x1C) = uv23;
				U32(pkt, 0x14) = uv1;
				U32(pkt, 0x24) = uv23 >> 16;
			}
			x::GTE_ReadFLAG(ctx + 0x3C);
			if ((U32(ctx, 0x3C) & 0x60000) != 0)
				goto next;
			x::GTE_NCLIP();
			{
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) < 0 && !(U8(ctx, 0x14) & 0x10))
					goto next;
				x::GTE_ReadSXY012_Split(pSXY0, pSXY1, pSXY2);
				x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0xA) * 4u);
				x::GTE_RTPS();
				int16_t v = S16(pSXY0, 0);
				if (v < 0 || v > 0xA00)
					clip = 1;
				v = S16(pSXY1, 0);
				if (v < 0 || v > 0xA00)
					clip |= 2;
				v = S16(pSXY2, 0);
				if (v < 0 || v > 0xA00)
					clip |= 4;
				v = S16(pkt, 0xA);
				if (v < 0 || v > 0x6C0)
					clip |= 0x10;
				v = S16(pkt, 0x12);
				if (v < 0 || v > 0x6C0)
					clip |= 0x20;
				v = S16(pkt, 0x1A);
				if (v < 0 || v > 0x6C0)
					clip |= 0x40;
				x::GTE_ReadSXY2(pSXY3);
				x::GTE_AVSZ4();
				v = S16(pSXY3, 0);
				if (v < 0 || v > 0xA00)
					clip |= 8;
				v = S16(pkt, 0x22);
				if (v < 0 || v > 0x6C0)
					clip |= 0x80;
				if ((clip & 0xF) == 0xF)
					goto next;
				if ((clip & 0xF0) == 0xF0)
					goto next;
			}
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x40)
			{
				x::set_unk_1CA8A28(pCode);
				x::set_dword_1CA8A30(U32(ctx, 0xC));
				x::sub_45F270();
				x::set_param_with_dword_1CA8A68(pCode);
			}
			{
				const int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
				S32(ctx, 0x38) = z;
				if (z < 0)
					S32(ctx, 0x38) = 0;
				const uint32_t ot = a2 + (uint32_t)(S32(ctx, 0x38) >> (a3 & 31)) * 4u;
				const uint16_t add0 = U16(ctx, 0x18);
				const uint16_t add1 = U16(ctx, 0x1A);
				if ((uint16_t)(add0 | add1) == 0)
				{
					x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
					pSXY0 += 0x28;
					pSXY1 += 0x28;
					pkt += 0x28;
					pSXY2 += 0x28;
					pSXY3 += 0x28;
					pCode += 0x28;
				}
				else
				{
					if (add0 != 0)
					{
						const uint32_t a = add0;
						const uint32_t c0 = U8(pkt, 0xC) + a;
						const uint32_t c1 = U8(pkt, 0x14) + a;
						const uint32_t c2 = U8(pkt, 0x1C) + a;
						const uint32_t c3 = U8(pkt, 0x24) + a;
						if ((int32_t)(c3 | c2 | c1 | c0) > 0xFF)
						{
							const uint8_t s = U8(ctx, 0x28);
							U8(pkt, 0xC) = (uint8_t)(c0 - s);
							U8(pkt, 0x14) = (uint8_t)(c1 - s);
							U8(pkt, 0x1C) = (uint8_t)(c2 - s);
							U8(pkt, 0x24) = (uint8_t)(c3 - s);
						}
						else
						{
							U8(pkt, 0x14) = (uint8_t)c1;
							U8(pkt, 0xC) = (uint8_t)c0;
							U8(pkt, 0x1C) = (uint8_t)c2;
							U8(pkt, 0x24) = (uint8_t)c3;
						}
					}
					const uint16_t add1b = U16(ctx, 0x1A);
					if (add1b != 0)
					{
						const uint32_t a = add1b;
						const uint32_t c0 = U8(pkt, 0xD) + a;
						const uint32_t c1 = U8(pkt, 0x15) + a;
						const uint32_t c2 = U8(pkt, 0x1D) + a;
						const uint32_t c3 = U8(pkt, 0x25) + a;
						if ((int32_t)(c3 | c2 | c1 | c0) > 0xFF)
						{
							const uint8_t s = U8(ctx, 0x2A);
							U8(pkt, 0xD) = (uint8_t)(c0 - s);
							U8(pkt, 0x15) = (uint8_t)(c1 - s);
							U8(pkt, 0x1D) = (uint8_t)(c2 - s);
							U8(pkt, 0x25) = (uint8_t)(c3 - s);
						}
						else
						{
							U8(pkt, 0x1D) = (uint8_t)c2;
							U8(pkt, 0xD) = (uint8_t)c0;
							U8(pkt, 0x15) = (uint8_t)c1;
							U8(pkt, 0x25) = (uint8_t)c3;
						}
					}
					pSXY0 += 0x34;
					pSXY3 += 0x34;
					pSXY2 += 0x34;
					const uint32_t poly = pkt;
					pCode += 0x34;
					pSXY1 += 0x34;
					uint32_t prim = pkt + 0x28;
					pkt += 0x34;
					U32(prim, 0) = 0x2000000;
					uint32_t tw = 0;
					if (ctx + 0x1C != 0)
						tw = s2_twin(ctx, 0x1C);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
					pSXY0 += 0xC;
					pSXY1 += 0xC;
					pSXY2 += 0xC;
					prim = pkt;
					pkt += 0xC;
					pSXY3 += 0xC;
					pCode += 0xC;
					U32(prim, 0) = 0x2000000;
					tw = 0;
					if (ctx + 0x24 != 0)
						tw = s2_twin(ctx, 0x24);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
				}
			}
		next:
			rec += 0x18;
			--count;
		} while (count != 0);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl dk_85DE90(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t data = U32(ctx, 0x2C);
		int32_t count = S32(data, 0);
		data += 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = data;
		if (count <= 0)
		{
			U32(ctx, 0x2C) = data;
			return pkt;
		}
		do
		{
			pkt = s3_quad(ctx, data, vbase, a2, a3, pkt);
			data += 0x24;
		} while (--count);
		U32(ctx, 0x2C) = data;
		return pkt;
	}

	uint32_t __cdecl dk_85E620(uint32_t a1)
	{
		const uint32_t node = a1;
		uint8_t st = U8(node, 0x29);
		U8(node, 0x26) |= 4;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl dk_85E930(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_037_DrinkMagic_Ribbon_Setup);
		states[1] = game_addr(game::MAG_037_DrinkMagic_Ribbon_State1_SpawnSpark);
		states[2] = game_addr(game::MAG_037_DrinkMagic_Ribbon_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		dk_85EA30(node);
		dk_85BE30(node);
		U16(node, 0x126) = (uint16_t)((U16(node, 0x126) + 0x180) & 0xFFF);
		dk_85EB60(node);
		U16(node, 0x124) = (uint16_t)(U16(node, 0x124) + 1);
		if (S16(node, 0x124) > 0xA)
			U16(node, 0x124) = 0xA;
		U16(node, 0x12A) = (uint16_t)(U16(node, 0x12A) + 1);
		const int16_t end = S16(node, 0x12A);
		if (end < 0xA)
		{
			const int16_t start = (int16_t)(end - 6);
			U16(node, 0x128) = (uint16_t)start;
			if (start < 1)
				U16(node, 0x128) = 1;
		}
		else
		{
			U16(node, 0x128) = (uint16_t)(U16(node, 0x128) + 1);
			U16(node, 0x12A) = 0xA;
			if (S16(node, 0x128) > 0xA)
				U16(node, 0x128) = 0xA;
		}
		dk_85BEA0(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl dk_85EA30(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint16_t c = U16(node, 0x124);
		const uint32_t z0 = U32(node, 0x8C);
		const int32_t steps = (int16_t)c;
		const int16_t angle = (int16_t)(uint16_t)((uint32_t)c * 51 * 4);
		U32(node, 0x1C) = U32(node, 0x88);
		U32(node, 0x20) = z0;
		const int32_t dx = div10(mul32(sub32(S16(node, 0x80), S16(node, 0x88)), steps));
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + (uint16_t)dx);
		const int32_t dy = div10(mul32(sub32(S16(node, 0x82), S16(node, 0x8A)), steps));
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + (uint16_t)dy);
		const int32_t dz = div10(mul32(sub32(S16(node, 0x84), S16(node, 0x8C)), steps));
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + (uint16_t)dz);
		alignas(4) uint8_t v[8];
		alignas(4) uint8_t o[8];
		U16(P(v), 0) = 0;
		const int32_t s = (int32_t)x::computeSin((uint32_t)(int32_t)angle);
		U16(P(v), 2) = (uint16_t)div3(s);
		U16(P(v), 4) = 0;
		x::matrixMultiplyVector(node + 0x60, P(v), P(o));
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + U16(P(o), 0));
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + U16(P(o), 2));
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + U16(P(o), 4));
		const int32_t k = S16(node, 0x124);
		U32(node, 0x90 + k * 8) = U32(node, 0x1C);
		U32(node, 0x94 + k * 8) = U32(node, 0x20);
		return 0;
	}

	uint32_t __cdecl dk_85EB60(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t bucket = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		uint32_t pkt = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		const int16_t last = S16(node, 0x12A);
		if (last == 0 || S16(node, 0x128) >= 0xA)
			return 0;
		const int32_t first = S16(node, 0x128);
		uint8_t alpha[0x10];
		{
			uint8_t a = 0;
			for (int32_t i = first; i < last + 1; i++)
			{
				alpha[i] = a;
				a = (uint8_t)(a + 0x20);
				if (a > 0x80) a = 0x80;
			}
		}
		alignas(4) uint8_t pts[22 * 8];
		alignas(4) uint8_t m[0x20];
		alignas(4) uint8_t v[8];
		alignas(4) uint8_t o[8];
		uint32_t roll = 0;
		for (int32_t i = first; i < S16(node, 0x12A) + 1; i++)
		{
			if (i == first) roll = (uint32_t)i * 0x120;
			ff8fx::copy<(0x20)>(m, (const void *)(node + 0x60));
			x::sub_8DD960(P(m), (U16(node, 0x126) + roll) & 0xFFF);
			const uint32_t h = node + 0x90 + (uint32_t)i * 8;
			const uint32_t pa = P(pts) + (uint32_t)i * 16, pb = pa + 8;
			const uint16_t hx = U16(h, 0);
			U16(pb, 0) = hx;
			U16(pa, 0) = hx;
			U16(P(v), 2) = 0xFF80;
			const uint16_t hy = U16(h, 2);
			U16(pb, 2) = hy;
			U16(pa, 2) = hy;
			const uint16_t hz = U16(h, 4);
			U16(pb, 4) = hz;
			U16(pa, 4) = hz;
			U16(P(v), 0) = 0;
			U16(P(v), 4) = 0;
			x::matrixMultiplyVector(P(m), P(v), P(o));
			U16(pa, 0) = (uint16_t)(U16(pa, 0) + U16(P(o), 0));
			U16(pa, 2) = (uint16_t)(U16(pa, 2) + U16(P(o), 2));
			U16(pa, 4) = (uint16_t)(U16(pa, 4) + U16(P(o), 4));
			U16(P(v), 0) = 0;
			U16(P(v), 4) = 0;
			U16(P(v), 2) = 0x80;
			x::matrixMultiplyVector(P(m), P(v), P(o));
			U16(pb, 0) = (uint16_t)(U16(pb, 0) + U16(P(o), 0));
			U16(pb, 2) = (uint16_t)(U16(pb, 2) + U16(P(o), 2));
			U16(pb, 4) = (uint16_t)(U16(pb, 4) + U16(P(o), 4));
			roll += 0x120;
		}
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		uint32_t sxy[22];
		int32_t sz[22];
		for (int32_t j = first * 2; j < S16(node, 0x12A) * 2 + 2; j++)
		{
			x::GTE_LoadV0(P(pts) + (uint32_t)j * 8);
			x::GTE_RTPS();
			x::GTE_ReadSXY2(P(&sxy[j]));
			gx::sub_45E220(P(&sz[j]));
		}
		for (int32_t i = first; i < S16(node, 0x12A); i++)
		{
			const int32_t j = i * 2;
			const uint8_t c0 = alpha[i], c1 = alpha[i + 1];
			U32(pkt, 0) = 0x9000000;
			U8(pkt, 7) = 0x36;
			U16(pkt, 0x1A) = 0xB9;
			U16(pkt, 0xE) = 0x3D94;
			int32_t z = div3(add32(add32(sz[j], sz[j + 2]), sz[j + 1])) / 4 >> 2;
			U32(pkt, 8) = sxy[j];
			U32(pkt, 0x14) = sxy[j + 2];
			U8(pkt, 0x12) = c1;
			U8(pkt, 0x11) = c1;
			U8(pkt, 0x10) = c1;
			U32(pkt, 0x20) = sxy[j + 1];
			U8(pkt, 0xC) = 0x60;
			U8(pkt, 0xD) = 0;
			U8(pkt, 0x18) = 0x7F;
			U8(pkt, 0x19) = 0;
			U8(pkt, 0x24) = 0x60;
			U8(pkt, 0x25) = 0x3F;
			U8(pkt, 6) = c0;
			U8(pkt, 5) = c0;
			U8(pkt, 4) = c0;
			U8(pkt, 0x1E) = c0;
			U8(pkt, 0x1D) = c0;
			U8(pkt, 0x1C) = c0;
			x::SSIGPU_InsertPrimAutoDepth(bucket + (uint32_t)(z * 4), pkt);
			pkt += 0x28;
			U32(pkt, 0) = 0x9000000;
			U8(pkt, 7) = 0x36;
			U16(pkt, 0x1A) = 0xB9;
			U16(pkt, 0xE) = 0x3D94;
			z = div3(add32(add32(sz[j + 3], sz[j + 2]), sz[j + 1])) / 4 >> 2;
			U32(pkt, 8) = sxy[j + 1];
			U32(pkt, 0x14) = sxy[j + 2];
			U32(pkt, 0x20) = sxy[j + 3];
			U8(pkt, 0xC) = 0x60;
			U8(pkt, 0xD) = 0x3F;
			U8(pkt, 0x18) = 0x7F;
			U8(pkt, 0x19) = 0;
			U8(pkt, 0x24) = 0x7F;
			U8(pkt, 0x25) = 0x3F;
			U8(pkt, 0x1E) = c1;
			U8(pkt, 0x1D) = c1;
			U8(pkt, 0x1C) = c1;
			U8(pkt, 0x12) = c1;
			U8(pkt, 0x11) = c1;
			U8(pkt, 0x10) = c1;
			U8(pkt, 6) = c0;
			U8(pkt, 5) = c0;
			U8(pkt, 4) = c0;
			x::SSIGPU_InsertPrimAutoDepth(bucket + (uint32_t)(z * 4), pkt);
			pkt += 0x28;
		}
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = pkt;
		return 0;
	}

	uint32_t __cdecl dk_85F020(uint32_t a1)
	{
		if (S16(a1, 0x124) < 0xA)
			return x::Effect_AddTaskAndInitFromCtx(game_addr(Q_RIBBON), game_addr(ORIG_Spark), NODE_SIZE, a1);
		U8(a1, 0x26) |= 5;
		U16(a1, 0x124) = 0xA;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl dk_85F060(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_037_DrinkMagic_Spark_State0_Setup);
		states[1] = game_addr(game::MAG_037_DrinkMagic_Spark_State1_CounterFinish);
		states[2] = game_addr(game::MAG_037_DrinkMagic_Spark_State2_Empty);
		const uint32_t owner = U32(node, 0x18);
		U32(node, 0x1C) = U32(owner, 0x1C);
		U32(node, 0x20) = U32(owner, 0x20);
		callp(states[S8(node, 0x29)], node);
		dk_85F0D0(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl dk_85F0D0(uint32_t a1)
	{
		if ((U8(a1, 0x26) & 4) != 0)
			return 0;
		alignas(4) uint8_t m[0x20];
		x::MAG_022_sub_8DD770(P(m));
		x::MAG_022_sub_8DD8A0(P(m), (uint32_t)(int32_t)S16(a1, 0x42));
		x::sub_8DD960(P(m), (uint32_t)(int32_t)S16(a1, 0x44));
		x::sub_8DD7E0(P(m), (uint32_t)(int32_t)S16(a1, 0x40));
		S32(P(m), 0x14) = S16(a1, 0x1C);
		S32(P(m), 0x18) = S16(a1, 0x1E);
		S32(P(m), 0x1C) = S16(a1, 0x20);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), P(m), P(m));
		x::GTE_SetRotMatrix_W(P(m));
		x::GTE_SetTransVector_W(P(m));
		const uint32_t hdr = x::Field_Alloc(0xB4);
		U32(hdr, 0) = U32(a1, 0x4C);
		const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		U16(hdr, 4) = U16(a1, 0x50);
		U16(hdr, 0x24) = 0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = gx::InitEffectSequenceFromData(hdr, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, cursor);
		x::Field_Free(0xB4);
		return 0;
	}

	uint32_t __cdecl dk_85F1B0(uint32_t a1)
	{
		U32(a1, 0x4C) = game_addr(SPARK_FLIPBOOK);
		U16(a1, 0x52) = 5;
		U16(a1, 0x54) = 0;
		U16(a1, 0x42) = (uint16_t)(x::CrtRand() & 0xFFF);
		U16(a1, 0x44) = (uint16_t)(x::CrtRand() & 0xFFF);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl dk_85F3A0(uint32_t a1)
	{
		if (U16(a1, 0x24) == 0xF)
		{
			U8(U32(a1, 0x10), 0x63) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return a1;
	}

	uint32_t __cdecl dk_85F3C0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 0xF)
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

	uint32_t __cdecl dk_856E00(uint32_t a1)
	{
		a_73C100(a1, game_addr(ORIG_ActorSystem), game_addr(ACTOR_DATA), 0x28, 0xF, 0);
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_RIBBON), game_addr(ORIG_Spawner), NODE_SIZE, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint16_t spawn_point_up(uint32_t ent, uint32_t a3, uint32_t pos)
	{
		x::GetEffectSpawnPosition(ent, 0xC, a3, pos);
		alignas(4) uint8_t v[8];
		alignas(4) uint8_t m[0x20];
		U16(P(v), 0) = 0;
		U16(P(v), 2) = 0xFF00;
		U16(P(v), 4) = 0;
		x::MAG_022_sub_8DD770(P(m));
		x::MAG_022_sub_8DD8A0(P(m), (uint32_t)(int32_t)S16(ent, 0xE));
		x::GTE_SetRotMatrix(P(m));
		x::GTE_LoadV0(P(v));
		x::GTE_MVMVA_RotV0();
		x::GTE_StoreIR123(P(v));
		U16(pos, 0) = (uint16_t)(U16(pos, 0) + U16(P(v), 0));
		U16(pos, 2) = (uint16_t)(U16(pos, 2) + U16(P(v), 2));
		U16(pos, 4) = (uint16_t)(U16(pos, 4) + U16(P(v), 4));
		return U16(P(m), 2);
	}

	uint32_t __cdecl dk_85C110(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t caster = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(U32(node, 0xC), 0) * 0x9C;
		spawn_point_up(caster, 0, game_addr(GATHER));
		gx::Effect_CopyMidpointFromSource(node, node + 0x1C);
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_ORB), game_addr(ORIG_Orb), 0x60, node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static uint32_t spawner_one(uint32_t a1, uint16_t tick, uint16_t number)
	{
		if (U16(a1, 0x24) != tick)
			return 0;
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_RIBBON), game_addr(ORIG_Ribbon), NODE_SIZE, a1);
		dk_85BC90(t, 0x80);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		U16(t, 0x120) = number;
		return 0;
	}
	uint32_t __cdecl dk_85E8E0(uint32_t a1) { return spawner_one(a1, 0x28, 0); }
	uint32_t __cdecl dk_85F260(uint32_t a1) { return spawner_one(a1, 0x2A, 1); }
	uint32_t __cdecl dk_85F2B0(uint32_t a1) { return spawner_one(a1, 0x2C, 2); }

	uint32_t __cdecl dk_85F300(uint32_t a1)
	{
		if (U16(a1, 0x24) != 0x2E)
			return 0;
		const uint32_t t1 = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_RIBBON), game_addr(ORIG_Ribbon), NODE_SIZE, a1);
		dk_85BC90(t1, 0x80);
		U16(t1, 0x120) = 3;
		const uint32_t t2 = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_RIBBON), game_addr(ORIG_Ribbon), NODE_SIZE, a1);
		dk_85BC90(t2, 0x80);
		U16(a1, 0x26) |= 1;
		U16(t2, 0x122) = 1;
		const uint8_t st = U8(a1, 0x29);
		U16(t2, 0x120) = 4;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl dk_85BC50(uint32_t a1)
	{
		const int16_t t = S16(a1, 0x24);
		if (t <= 0xD || t >= 0x3F)
			return 0;
		const uint32_t n = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_RIBBON), game_addr(ORIG_Streak), NODE_SIZE, a1);
		dk_85BC90(n, 0x80);
		return 0;
	}

	uint32_t __cdecl dk_85BBD0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[6];
		states[0] = game_addr(game::MAG_037_DrinkMagic_Spawner_GatherPointAndOrb);
		states[1] = game_addr(game::MAG_037_DrinkMagic_Spawner_State1_SpawnRibbon);
		states[2] = game_addr(game::MAG_037_DrinkMagic_Spawner_State2_SpawnRibbon);
		states[3] = game_addr(game::MAG_037_DrinkMagic_Spawner_State3_SpawnRibbon);
		states[4] = game_addr(game::MAG_037_DrinkMagic_Spawner_State4_SpawnRibbon);
		states[5] = game_addr(game::MAG_037_DrinkMagic_Spawner_State5_Empty);
		callp(states[S8(node, 0x29)], node);
		dk_85BC50(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl dk_85BFD0(uint32_t a1)
	{
		const uint32_t node = a1;
		U32(node, 0x4C) = game_addr(STREAK_FLIPBOOK);
		U16(node, 0x52) = 0xA;
		U16(node, 0x54) = 0;
		const uint32_t r = x::CrtRand();
		const uint32_t g0 = MEM<uint32_t>(game_addr(GATHER));
		const uint32_t z0 = U32(node, 0x20);
		U32(node, 0x80) = g0;
		U16(node, 0x126) = (uint16_t)(r & 0xFFF);
		U32(node, 0x88) = U32(node, 0x1C);
		const uint32_t g1 = MEM<uint32_t>(game_addr(GATHER) + 4);
		U32(node, 0x8C) = z0;
		U32(node, 0x84) = g1;
		alignas(4) uint8_t up[8];
		alignas(4) uint8_t dir[8];
		U16(P(dir), 0) = (uint16_t)(U16(node, 0x80) - U16(node, 0x88));
		U16(P(dir), 2) = (uint16_t)(U16(node, 0x82) - U16(node, 0x8A));
		U16(P(up), 0) = 0;
		U16(P(up), 4) = 0;
		U16(P(up), 2) = 0x1000;
		U16(P(dir), 4) = (uint16_t)(U16(node, 0x84) - U16(node, 0x8C));
		gx::Effect_BuildMatrixFromDirAndUp(node + 0x60, P(dir), P(up));
		const uint32_t roll = (x::CrtRand() & 0x7FF) + 0x400;
		x::sub_8DD960(node + 0x60, roll);
		U16(node, 0x12C) = (uint16_t)((x::CrtRand() & 0x1FF) + 0xA00);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl dk_85C0C0(uint32_t a1)
	{
		U16(a1, 0x12E) = (uint16_t)(U16(a1, 0x12E) + 0x30);
		U16(a1, 0x12C) = (uint16_t)(U16(a1, 0x12C) + U16(a1, 0x12E));
		if (S16(a1, 0x12C) >= 0x1000)
		{
			U8(a1, 0x26) |= 5;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return a1;
	}

	uint32_t __cdecl dk_85BEC0(uint32_t a1)
	{
		const uint32_t node = a1;
		const int32_t f = S16(node, 0x12C);
		const int32_t half = (int32_t)(((int64_t)f * 2048 + (((int64_t)f * 2048) < 0 ? 0xFFF : 0)) >> 12);
		const uint32_t z0 = U32(node, 0x8C);
		U32(node, 0x1C) = U32(node, 0x88);
		U32(node, 0x20) = z0;
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + (uint16_t)(mul32(sub32(S16(node, 0x80), S16(node, 0x88)), f) / 4096));
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + (uint16_t)(mul32(sub32(S16(node, 0x82), S16(node, 0x8A)), f) / 4096));
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + (uint16_t)(mul32(sub32(S16(node, 0x84), S16(node, 0x8C)), f) / 4096));
		alignas(4) uint8_t v[8];
		alignas(4) uint8_t o[8];
		U16(P(v), 0) = 0;
		const int32_t s = (int32_t)x::computeSin((uint32_t)(int32_t)(int16_t)half);
		U16(P(v), 2) = (uint16_t)div3(s);
		U16(P(v), 4) = 0;
		x::matrixMultiplyVector(node + 0x60, P(v), P(o));
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + U16(P(o), 0));
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + U16(P(o), 2));
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + U16(P(o), 4));
		return 0;
	}

	uint32_t __cdecl dk_85BD50(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_037_DrinkMagic_Streak_Setup);
		states[1] = game_addr(game::MAG_037_DrinkMagic_Streak_Accelerate);
		states[2] = game_addr(game::MAG_037_DrinkMagic_Streak_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		dk_85BEC0(node);
		dk_85BE30(node);
		U16(node, 0x124) = (uint16_t)(U16(node, 0x124) + 1);
		if (S16(node, 0x124) > 0xA)
			U16(node, 0x124) = 0xA;
		U16(node, 0x12A) = (uint16_t)(U16(node, 0x12A) + 1);
		const int16_t end = S16(node, 0x12A);
		if (end < 0xA)
		{
			const int16_t start = (int16_t)(end - 6);
			U16(node, 0x128) = (uint16_t)start;
			if (start < 1)
				U16(node, 0x128) = 1;
		}
		else
		{
			U16(node, 0x128) = (uint16_t)(U16(node, 0x128) + 1);
			U16(node, 0x12A) = 0xA;
			if (S16(node, 0x128) > 0xA)
				U16(node, 0x128) = 0xA;
		}
		dk_85BEA0(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl dk_85EF20(uint32_t a1)
	{
		const uint32_t node = a1;
		U32(node, 0x4C) = game_addr(RIBBON_FLIPBOOK);
		U16(node, 0x52) = 5;
		U16(node, 0x54) = 0;
		const uint32_t r = x::CrtRand();
		const uint32_t g0 = MEM<uint32_t>(game_addr(GATHER));
		const uint32_t z0 = U32(node, 0x20);
		U32(node, 0x80) = g0;
		U16(node, 0x126) = (uint16_t)(r & 0xFFF);
		U32(node, 0x88) = U32(node, 0x1C);
		const uint32_t g1 = MEM<uint32_t>(game_addr(GATHER) + 4);
		U32(node, 0x8C) = z0;
		U32(node, 0x84) = g1;
		alignas(4) uint8_t up[8];
		alignas(4) uint8_t dir[8];
		U16(P(dir), 0) = (uint16_t)(U16(node, 0x80) - U16(node, 0x88));
		U16(P(dir), 2) = (uint16_t)(U16(node, 0x82) - U16(node, 0x8A));
		U16(P(up), 0) = 0;
		U16(P(up), 4) = 0;
		U16(P(up), 2) = 0x1000;
		U16(P(dir), 4) = (uint16_t)(U16(node, 0x84) - U16(node, 0x8C));
		gx::Effect_BuildMatrixFromDirAndUp(node + 0x60, P(dir), P(up));
		uint32_t roll;
		if ((U8(node, 0x120) & 1) != 0)
			roll = (x::CrtRand() & 0x3FF) + 0x400;
		else
			roll = (x::CrtRand() & 0x3FF) + 0x800;
		x::sub_8DD960(node + 0x60, roll);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static uint16_t g_85E780_m2 = 0;
	uint32_t __cdecl dk_85E780(uint32_t a1, uint32_t a2)
	{
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)((int32_t)(int16_t)a2 * 0x9C);
		g_85E780_m2 = spawn_point_up(ent, 0x400, a1);
		return 0;
	}

	uint32_t __cdecl dk_85E6C0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2C) * 0x9C;
		int32_t best = 0;
		uint32_t act = U32(U32(node, 0xC), 4) + (uint32_t)((int32_t)S8(node, 0x2A) * 20);
		if (U8(act, 0x10) == 0)
			return 0;
		for (uint32_t i = 0; ; )
		{
			const uint32_t t = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(U32(act, 8), (int32_t)(i * 0x18)) * 0x9C;
			const int32_t dx = sub32(S16(t, 0x1C), S16(ent, 0x1C));
			const int32_t dz = sub32(S16(t, 0x20), S16(ent, 0x20));
			const int32_t d = (int32_t)x::Sqrt((uint32_t)add32(mul32(dz, dz), mul32(dx, dx)));
			if (best < d)
				best = d;
			i++;
			act = U32(U32(node, 0xC), 4) + (uint32_t)((int32_t)S8(node, 0x2A) * 20);
			if ((int32_t)i >= (int32_t)U8(act, 0x10))
				break;
		}
		return (uint32_t)best;
	}

	uint32_t __cdecl dk_85E640(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 0xF)
			return 0;
		U8(node, 0x26) &= 0xFB;
		U32(node, 0x4C) = game_addr(ORB_MODEL);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2C) * 0x9C;
		U32(node, 0x30) = 0x1400;
		U32(node, 0x34) = 0x1400;
		U32(node, 0x38) = dk_85E6C0(node);
		U16(node, 0x46) = U16(ent, 0xE);
		U16(node, 0x44) = 0xF00;
		dk_85E780(node + 0x1C, (uint32_t)U8(node, 0x2C));
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		U16(node, 0x50) = 0x1000;
		return 0;
	}

	uint32_t __cdecl dk_85E820(uint32_t a1)
	{
		dk_85E780(a1 + 0x1C, (uint32_t)U8(a1, 0x2C));
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0xFF00);
		if (S16(a1, 0x50) <= 0)
		{
			U16(a1, 0x50) = 0;
			U16(a1, 0x5C) = 0x14;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl dk_85E860(uint32_t a1)
	{
		dk_85E780(a1 + 0x1C, (uint32_t)U8(a1, 0x2C));
		U16(a1, 0x5C) = (uint16_t)(U16(a1, 0x5C) - 1);
		if (S16(a1, 0x5C) <= 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl dk_85E890(uint32_t a1)
	{
		dk_85E780(a1 + 0x1C, (uint32_t)U8(a1, 0x2C));
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0x100);
		if (S16(a1, 0x50) >= 0x1000)
		{
			U8(a1, 0x26) |= 1;
			U16(a1, 0x50) = 0x1000;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	static uint16_t ORB_VERTEX_PAD = 0xFF00;

	uint32_t __cdecl dk_85E4E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t src = a1 + 4;
		const int32_t count = S32(src, 0);
		alignas(4) uint8_t m[0x20];
		x::MAG_022_sub_8DD770(P(m));
		x::sub_8DD960(P(m), (uint32_t)(int32_t)(int16_t)a4);
		if (count <= 0)
			return 0;
		const uint32_t delta = a2 - src;
		uint32_t v = src + 4;
		for (int32_t n = count; n != 0; n--, v += 8)
		{
			const int32_t z = S16(v, 4);
			const uint32_t ang = (uint32_t)add32(z, (int16_t)a3) & 0xFFF;
			const uint32_t half = (uint32_t)((z - (z >> 31)) >> 1) & 0xFFF;
			const int32_t amp1 = (int32_t)(((int64_t)(z * 512) + ((z * 512) < 0 ? 0xFFF : 0)) >> 12);
			const int32_t amp2 = (int32_t)(((int64_t)(z * 3072) + ((z * 3072) < 0 ? 0xFFF : 0)) >> 12);
			alignas(4) uint8_t in[8];
			alignas(4) uint16_t out[4];
			out[3] = ORB_VERTEX_PAD;
			U32(P(in), 0) = U32(v, 0);
			U32(P(in), 4) = U32(v, 4);
			x::GTE_SetRotMatrix(P(m));
			x::GTE_LoadV0(P(in));
			x::GTE_MVMVA_RotV0();
			x::GTE_StoreIR123(P(out));
			const int32_t s1 = (int32_t)x::computeSin(ang);
			const int32_t p1 = mul32(s1, (int16_t)amp1);
			out[0] = (uint16_t)(out[0] + (uint16_t)(p1 / 4096));
			const int32_t s2 = (int32_t)x::computeSin(half >> 1);
			const int32_t p2 = mul32(s2, (int16_t)amp2);
			const uint32_t zw = U32(P(out), 4);
			out[1] = (uint16_t)(out[1] + (uint16_t)(-(p2 / 4096)));
			U32(v + delta, 4) = U32(P(out), 0);
			U32(v + delta, 8) = zw;
		}
		return 0;
	}

	uint32_t __cdecl dk_85C2C0(uint32_t a1, uint32_t a2)
	{
		const int32_t count = S32(a1, 4);
		uint32_t v = a1 + 8;
		U16(a2, 4) = 0x7000;
		U16(a2, 2) = 0x7000;
		U16(a2, 0) = 0x7000;
		U16(a2, 0xA) = 0x9000;
		U16(a2, 8) = 0x9000;
		U16(a2, 6) = 0x9000;
		for (int32_t n = count; n > 0; n--, v += 8)
			for (int k = 0; k < 3; k++)
			{
				const int16_t c = S16(v, 2 * k);
				if (S16(a2, 2 * k) > c) S16(a2, 2 * k) = c;
				if (S16(a2, 6 + 2 * k) < c) S16(a2, 6 + 2 * k) = c;
			}
		return a2;
	}

	uint32_t __cdecl dk_85C350(uint32_t a1)
	{
		const uint32_t node = a1;
		if ((U8(node, 0x26) & 4) != 0)
			return 0;
		alignas(4) uint8_t m[0x20];
		x::MAG_022_sub_8DD770(P(m));
		x::MAG_022_sub_8DD8A0(P(m), (uint32_t)(int32_t)S16(node, 0x46));
		x::sub_8DD7E0(P(m), (uint32_t)(int32_t)S16(node, 0x44));
		x::sub_8DD960(P(m), (uint32_t)(int32_t)S16(node, 0x48));
		S32(P(m), 0x14) = S16(node, 0x1C);
		S32(P(m), 0x18) = S16(node, 0x1E);
		S32(P(m), 0x1C) = S16(node, 0x20);
		x::scale3DMatrix(P(m), node + 0x30);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), P(m), P(m));
		x::GTE_SetRotMatrix_W(P(m));
		x::GTE_SetTransVector_W(P(m));
		const uint32_t hdr = x::Field_Alloc(0x68);
		U32(hdr, 0) = U32(node, 0x4C);
		U32(hdr, 8) = U32(node, 0x40);
		S32(hdr, 0xC) = S16(node, 0x50);
		U16(hdr, 0x18) = U16(node, 0x5E);
		U16(hdr, 0x1A) = 0;
		U16(hdr, 0x1E) = 0;
		U16(hdr, 0x1C) = 0;
		U16(hdr, 0x24) = 0;
		U16(hdr, 0x26) = 0;
		U16(hdr, 0x22) = 0x100;
		U16(hdr, 0x20) = 0x100;
		U16(hdr, 0x2A) = 0x100;
		S32(hdr, 0x10) = S16(node, 0x52);
		U32(hdr, 0x14) = 0xF0;
		U16(hdr, 0x28) = 0x80;
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = dk_85C480(hdr, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl dk_85C1F0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[6];
		states[0] = game_addr(game::MAG_037_DrinkMagic_Orb_State0_Finish);
		states[1] = game_addr(game::MAG_037_DrinkMagic_Orb_State1_ShowAtCaster);
		states[2] = game_addr(game::MAG_037_DrinkMagic_Orb_State2_FadeOutAtCaster);
		states[3] = game_addr(game::MAG_037_DrinkMagic_Orb_State3_HoldAtCaster);
		states[4] = game_addr(game::MAG_037_DrinkMagic_Orb_State4_FadeInFinish);
		states[5] = game_addr(game::MAG_037_DrinkMagic_Orb_State5_Empty);
		const int32_t st = S8(node, 0x29);
		callp(states[st], node);
		if ((U8(node, 0x26) & 4) == 0)
		{
			ORB_VERTEX_PAD = st == 1 ? g_85E780_m2 : 0xFF00;
			const uint32_t b = (uint32_t)(uint16_t)(U16(node, 0x58) - 0xA0) & 0xFFF;
			const uint32_t a = (uint32_t)(uint16_t)(U16(node, 0x54) + 0xA0) & 0xFFF;
			U16(node, 0x58) = (uint16_t)b;
			U16(node, 0x54) = (uint16_t)a;
			dk_85E4E0(game_addr(ORB_TEMPLATE), U32(node, 0x4C), a, b);
			U16(node, 0x5E) = (uint16_t)((uint8_t)(U8(node, 0x5E) + 8) & 0x7F);
			dk_85C350(node);
			alignas(4) uint8_t bounds[0xC];
			dk_85C2C0(U32(node, 0x4C), P(bounds));
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_037_DrinkMagic_Master_State0_Advance, (void *)a_73A0D0, "037 MAG_037_sub_856CA0" },
		{ game::MAG_037_DrinkMagic_Master_State1_Advance, (void *)a_73A0D0, "037 MAG_037_sub_856CB0" },
		{ game::MAG_037_DrinkMagic_Master_State3_SpawnEmitter, (void *)a_73A170, "037 MAG_037_sub_856D40" },
		{ game::MAG_037_SpawnChildTask, (void *)a_73C100, "037 MAG_037_sub_856E40" },
		{ game::MAG_037_ActorSystemTask, (void *)a_73A380, "037 MAG_037_sub_856E90" },
		{ game::MAG_037_Director_Mode3Setup, (void *)a_73FE90, "037 sub_8573E0" },
		{ game::MAG_037_RelocateActorData, (void *)a_740210, "037 sub_857710" },
		{ game::MAG_037_ActSys_DrawPass, (void *)a_735440, "037 sub_857A00" },
		{ game::MAG_037_ActSys_DrawActorModel, (void *)a_7354A0, "037 sub_857A60" },
		{ game::MAG_037_RenderPrimModel, (void *)a_7355E0, "037 sub_857BA0" },
		{ game::MAG_037_RenderPolyFT3List, (void *)a_735BB0, "037 sub_858170" },
		{ game::MAG_037_RenderPolyFT4List, (void *)a_735DF0, "037 sub_8583B0" },
		{ game::MAG_037_ActorMotion, (void *)a_740910, "037 sub_859290" },
		{ game::MAG_037_DampValue, (void *)a_740F70, "037 sub_8598F0" },
		{ game::MAG_037_DampVector, (void *)a_740FC0, "037 sub_859940" },
		{ game::MAG_037_Instance_AdvanceTexFrame, (void *)a_741050, "037 sub_8599D0" },
		{ game::MAG_037_Instance_BuildTransform, (void *)a_741120, "037 sub_859AA0" },
		{ game::MAG_037_Particle_LifeCountdown, (void *)a_741B60, "037 sub_85A4E0" },
		{ game::MAG_037_RandRange, (void *)a_742290, "037 au_re__rand_51" },
		{ game::MAG_037_RandRangeWide, (void *)a_7422C0, "037 sub_85AC40" },
		{ game::MAG_037_RandVec3, (void *)a_742300, "037 sub_85AC80" },
		{ game::MAG_037_JitterAngles3, (void *)a_742350, "037 sub_85ACD0" },
		{ game::MAG_037_RandAngleJitter, (void *)a_7423A0, "037 au_re__rand_51_0" },
		{ game::MAG_037_GetWord8Minus1, (void *)a_7424B0, "037 sub_85AE30" },
		{ game::MAG_037_Particle_AttachToOwner, (void *)a_7424C0, "037 sub_85AE40" },
		{ game::MAG_037_ParticleList_Append, (void *)a_742CA0, "037 sub_85B620" },
		{ game::MAG_037_ParticleList_Unlink, (void *)a_742CD0, "037 sub_85B650" },
		{ game::MAG_037_Particle_Init, (void *)a_742D00, "037 sub_85B680" },
		{ game::MAG_037_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "037 sub_85B830" },
		{ game::MAG_037_Particle_PlaceAtPoint, (void *)a_742FF0, "037 sub_85B970" },
		{ game::MAG_037_ActorSystem_State2_Empty, (void *)a_73A6D0, "037 nullsub_1636" },
		{ game::MAG_037_DrinkMagic_Streak_State2_Empty, (void *)a_73A6D0, "037 nullsub_1637" },
		{ game::MAG_037_EmitG3, (void *)a_73D770, "037 sub_85D460" },
		{ game::MAG_037_EmitG4, (void *)a_73D9C0, "037 sub_85D6B0" },
		{ game::MAG_037_DrinkMagic_Orb_State5_Empty, (void *)a_73A6D0, "037 nullsub_1638" },
		{ game::MAG_037_DrinkMagic_Spark_State1_CounterFinish, (void *)a_743C00, "037 sub_85F1F0" },
		{ game::MAG_037_StepCounterLimit, (void *)a_743C20, "037 sub_85F210" },
		{ game::MAG_037_DrinkMagic_Spark_State2_Empty, (void *)a_73A6D0, "037 nullsub_1639" },
		{ game::MAG_037_DrinkMagic_Ribbon_State2_Empty, (void *)a_73A6D0, "037 nullsub_1640" },
		{ game::MAG_037_DrinkMagic_Spawner_State5_Empty, (void *)a_73A6D0, "037 nullsub_1641" },
		{ game::MAG_037_DrinkMagic_Emitter_State3_Empty, (void *)a_73A6D0, "037 nullsub_1642" },
		{ game::MAG_037_DrinkMagic_Master_State4_NextTargetLoop, (void *)a_747500, "037 MAG_037_sub_85F410" },
		{ game::MAG_037_DrinkMagic_Master_State5_Advance, (void *)a_73A0D0, "037 MAG_037_sub_85F450" },
		{ game::MAG_037_DrinkMagic_Master_State6_WaitQueuesEmpty, (void *)a_747550, "037 MAG_037_sub_85F460" },
		{ game::MAG_037_DrinkMagic_Master_State7_Advance, (void *)a_73A0D0, "037 MAG_037_sub_85F470" },
		{ game::MAG_037_DrinkMagic_Master_State8_Advance, (void *)a_73A0D0, "037 MAG_037_sub_85F480" },
		{ game::MAG_037_DrinkMagic_Master_State9_Finish, (void *)a_7475A0, "037 MAG_037_sub_85F490" },
		{ game::MAG_037_DrinkMagic_Master_State10_Empty, (void *)a_73A6D0, "037 nullsub_1635" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_037_DrinkMagic_MasterTask, (void *)dk_856B40, "037 dk_856B40" },
		{ game::MAG_037_DrinkMagic_Master_State2_CarveActorPools, (void *)dk_856CC0, "037 dk_856CC0" },
		{ game::MAG_037_Master_ClearActorPools, (void *)dk_856D00, "037 dk_856D00" },
		{ game::MAG_037_DrinkMagic_EmitterTask, (void *)dk_856D70, "037 dk_856D70" },
		{ game::MAG_037_DrinkMagic_Emitter_State0_SpawnActorSystem, (void *)dk_856E00, "037 dk_856E00" },
		{ game::MAG_037_ActorSystem_State0_StartActorSystemAtFrame, (void *)dk_856EF0, "037 dk_856EF0" },
		{ game::MAG_037_DirectorSetup, (void *)dk_856F20, "037 dk_856F20" },
		{ game::MAG_037_ResetTrailHistory, (void *)dk_8570D0, "037 dk_8570D0" },
		{ game::MAG_037_Director_CasterRefPoints, (void *)dk_8571B0, "037 dk_8571B0" },
		{ game::MAG_037_FillTrailRows, (void *)dk_857310, "037 dk_857310" },
		{ game::MAG_037_Director_TargetRefPoints, (void *)dk_857500, "037 dk_857500" },
		{ game::MAG_037_ActSys_Step, (void *)dk_857970, "037 dk_857970" },
		{ game::MAG_037_MeshEmitF3, (void *)dk_857CE0, "037 dk_857CE0" },
		{ game::MAG_037_MeshEmitF4, (void *)dk_857EF0, "037 dk_857EF0" },
		{ game::MAG_037_PrimList_G3, (void *)dk_858650, "037 dk_858650" },
		{ game::MAG_037_PrimList_G4, (void *)dk_858880, "037 dk_858880" },
		{ game::MAG_037_PrimList_GT3, (void *)dk_858B40, "037 dk_858B40" },
		{ game::MAG_037_PrimList_GT4, (void *)dk_858DA0, "037 dk_858DA0" },
		{ game::MAG_037_ActSys_DrawSpritePass, (void *)dk_859080, "037 dk_859080" },
		{ game::MAG_037_ActSys_DrawActorSprite, (void *)dk_859140, "037 dk_859140" },
		{ game::MAG_037_ActSys_UpdatePass, (void *)dk_8591C0, "037 dk_8591C0" },
		{ game::MAG_037_ActorStep, (void *)dk_859200, "037 dk_859200" },
		{ game::MAG_037_MorphVertexBlock, (void *)dk_85A230, "037 dk_85A230" },
		{ game::MAG_037_Instance_EndOfLifeTest, (void *)dk_85A340, "037 dk_85A340" },
		{ game::MAG_037_Instance_SpawnFollowUp, (void *)dk_85A3C0, "037 dk_85A3C0" },
		{ game::MAG_037_AllocParticle, (void *)dk_85A3F0, "037 dk_85A3F0" },
		{ game::MAG_037_ActSys_UpdateEmitter, (void *)dk_85A490, "037 dk_85A490" },
		{ game::MAG_037_ActSys_EmitterSpawnStep, (void *)dk_85A510, "037 dk_85A510" },
		{ game::MAG_037_ActSys_SpawnParticles, (void *)dk_85A590, "037 dk_85A590" },
		{ game::MAG_037_ActSys_AllocParticleSlot, (void *)dk_85AD70, "037 dk_85AD70" },
		{ game::MAG_037_Actor_InterpolatePosition, (void *)dk_85AF90, "037 dk_85AF90" },
		{ game::MAG_037_SpawnSubEffects, (void *)dk_85BA80, "037 dk_85BA80" },
		{ game::MAG_037_Director_SpawnGroupEffects, (void *)dk_85BB10, "037 dk_85BB10" },
		{ game::MAG_037_ActorSystem_State1_StepActorSystem, (void *)dk_85BBA0, "037 dk_85BBA0" },
		{ game::MAG_037_DrinkMagic_SpawnerTask, (void *)dk_85BBD0, "037 dk_85BBD0" },
		{ game::MAG_037_DrinkMagic_Spawner_SpawnStreak, (void *)dk_85BC50, "037 dk_85BC50" },
		{ game::MAG_037_RandomOffsetPosition, (void *)dk_85BC90, "037 dk_85BC90" },
		{ game::MAG_037_DrinkMagic_StreakTask, (void *)dk_85BD50, "037 dk_85BD50" },
		{ game::MAG_037_DrawSprite, (void *)dk_85BE30, "037 dk_85BE30" },
		{ game::MAG_037_FlipbookStep, (void *)dk_85BEA0, "037 dk_85BEA0" },
		{ game::MAG_037_DrinkMagic_Streak_StepHead, (void *)dk_85BEC0, "037 dk_85BEC0" },
		{ game::MAG_037_DrinkMagic_Streak_Setup, (void *)dk_85BFD0, "037 dk_85BFD0" },
		{ game::MAG_037_DrinkMagic_Streak_Accelerate, (void *)dk_85C0C0, "037 dk_85C0C0" },
		{ game::MAG_037_DrinkMagic_Spawner_GatherPointAndOrb, (void *)dk_85C110, "037 dk_85C110" },
		{ game::MAG_037_DrinkMagic_OrbTask, (void *)dk_85C1F0, "037 dk_85C1F0" },
		{ game::MAG_037_DrinkMagic_ModelBounds, (void *)dk_85C2C0, "037 dk_85C2C0" },
		{ game::MAG_037_DrinkMagic_Orb_Draw, (void *)dk_85C350, "037 dk_85C350" },
		{ game::MAG_037_DrinkMagic_RenderPrimModel, (void *)dk_85C480, "037 dk_85C480" },
		{ game::MAG_037_PrimList_FlatTri, (void *)dk_85C5B0, "037 dk_85C5B0" },
		{ game::MAG_037_PrimList_FlatQuad, (void *)dk_85C7D0, "037 dk_85C7D0" },
		{ game::MAG_037_PrimList_TexTri, (void *)dk_85CA60, "037 dk_85CA60" },
		{ game::MAG_037_PrimList_TexQuad, (void *)dk_85CEE0, "037 dk_85CEE0" },
		{ game::MAG_037_PrimList_GT4Scroll, (void *)dk_85DE90, "037 dk_85DE90" },
		{ game::MAG_037_DrinkMagic_Orb_WarpModel, (void *)dk_85E4E0, "037 dk_85E4E0" },
		{ game::MAG_037_DrinkMagic_Orb_State0_Finish, (void *)dk_85E620, "037 dk_85E620" },
		{ game::MAG_037_DrinkMagic_Orb_State1_ShowAtCaster, (void *)dk_85E640, "037 dk_85E640" },
		{ game::MAG_037_DrinkMagic_FarthestTargetDistance, (void *)dk_85E6C0, "037 dk_85E6C0" },
		{ game::MAG_037_DrinkMagic_CasterPointUp, (void *)dk_85E780, "037 dk_85E780" },
		{ game::MAG_037_DrinkMagic_Orb_State2_FadeOutAtCaster, (void *)dk_85E820, "037 dk_85E820" },
		{ game::MAG_037_DrinkMagic_Orb_State3_HoldAtCaster, (void *)dk_85E860, "037 dk_85E860" },
		{ game::MAG_037_DrinkMagic_Orb_State4_FadeInFinish, (void *)dk_85E890, "037 dk_85E890" },
		{ game::MAG_037_DrinkMagic_Spawner_State1_SpawnRibbon, (void *)dk_85E8E0, "037 dk_85E8E0" },
		{ game::MAG_037_DrinkMagic_RibbonTask, (void *)dk_85E930, "037 dk_85E930" },
		{ game::MAG_037_Ribbon_StepHead, (void *)dk_85EA30, "037 dk_85EA30" },
		{ game::MAG_037_Ribbon_Draw, (void *)dk_85EB60, "037 dk_85EB60" },
		{ game::MAG_037_DrinkMagic_Ribbon_Setup, (void *)dk_85EF20, "037 dk_85EF20" },
		{ game::MAG_037_DrinkMagic_Ribbon_State1_SpawnSpark, (void *)dk_85F020, "037 dk_85F020" },
		{ game::MAG_037_DrinkMagic_SparkTask, (void *)dk_85F060, "037 dk_85F060" },
		{ game::MAG_037_Spark_Draw, (void *)dk_85F0D0, "037 dk_85F0D0" },
		{ game::MAG_037_DrinkMagic_Spark_State0_Setup, (void *)dk_85F1B0, "037 dk_85F1B0" },
		{ game::MAG_037_DrinkMagic_Spawner_State2_SpawnRibbon, (void *)dk_85F260, "037 dk_85F260" },
		{ game::MAG_037_DrinkMagic_Spawner_State3_SpawnRibbon, (void *)dk_85F2B0, "037 dk_85F2B0" },
		{ game::MAG_037_DrinkMagic_Spawner_State4_SpawnRibbon, (void *)dk_85F300, "037 dk_85F300" },
		{ game::MAG_037_DrinkMagic_Emitter_State1_ReleaseMasterAt15, (void *)dk_85F3A0, "037 dk_85F3A0" },
		{ game::MAG_037_DrinkMagic_Emitter_State2_ApplyDamage, (void *)dk_85F3C0, "037 dk_85F3C0" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag037_drink_magic()
	{
		act::register_module(37);
		for (const act::drinkmagic::ModPort *p = act::drinkmagic::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(37, p->addr, p->port, p->name);
		for (const act::drinkmagic::ModPort *p = act::drinkmagic::PORTS; p->addr; p++)
			act::register_module_port(37, p->addr, p->port, p->name);
	}
}

