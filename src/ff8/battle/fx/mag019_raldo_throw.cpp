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
namespace raldo
{
	static const Mod MOD_019 = { "granaldo", 19, game::MAG_019_UNK0_FL, game::MAG_016_RECOVER_FL,
		{ {}, {}, game::p_RaldoThrow_Director, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::unk_272B8F8, {}, {}, {}, {}, {}, {}, {}, game::MAG_019_BallActorSystemQueue, {}, game::MAG_019_EmitterQueue, {}, {}, {}, {}, {}, game::unk_273046C, {}, {}, game::unk_273047E, {}, game::g_RaldoThrow_ScratchSP, {}, game::g_RaldoThrow_ActorState },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_019_ActSys_DrawPass, game::MAG_019_ActSys_DrawPrimActor, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_019_ActSys_DrawSpritePass, game::MAG_019_ActSys_UpdatePass, game::MAG_019_ActorStep, game::MAG_019_ActSys_UpdateEmitter, game::MAG_019_Particle_LifeCountdown, game::MAG_019_ActSys_EmitterSpawnStep, game::MAG_019_ActSys_SpawnParticles, game::MAG_019_RandVec3, game::MAG_019_JitterAngles3, game::MAG_019_ActSys_AllocParticleSlot, game::MAG_019_GetWord8Minus1, game::MAG_019_Particle_Init, game::MAG_019_SpawnSubEffects, game::MAG_019_Director_SpawnGroupEffects, {}, {}, {}, {}, {}, game::MAG_019_RaldoThrow_EmitterTask, game::MAG_019_RaldoThrow_Emitter_State0_PlaceAndSpawnDirector, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_019_DirSyncGate, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_019_ResetTrailHistory, game::MAG_019_Director_StartPositions, game::MAG_019_FillTrailRows, game::MAG_019_Director_Mode3Setup, game::MAG_019_Director_TargetPositions, game::MAG_019_RelocateActorData, game::MAG_019_ActSys_DrawActorSprite, game::MAG_019_ActorMotion, game::MAG_019_DampValue, game::MAG_019_DampVector, game::MAG_019_Instance_AdvanceTexFrame, game::MAG_019_Instance_BuildTransform, game::MAG_019_MorphVertexBlock, game::MAG_019_Instance_EndOfLifeTest, game::MAG_019_Instance_SpawnFollowUp, game::MAG_019_AllocParticle, game::MAG_019_RandRange, game::MAG_019_RandRangeWide, game::MAG_019_RandAngleJitter, game::MAG_019_Particle_AttachToOwner, game::MAG_019_Actor_InterpolatePosition, {}, game::MAG_019_ParticleList_Unlink, game::MAG_019_Particle_PlaceAtAnchorSet, game::MAG_019_Particle_PlaceAtPoint, game::MAG_019_SpriteStrip_DrawQuads, game::MAG_019_ActSys_Step, game::MAG_019_RaldoThrow_Emitter_ApplyDamage, game::MAG_019_RaldoThrow_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_019_RaldoThrow_Emitter_State3_FinishReleaseRoot, game::MAG_019_RaldoThrow_Emitter_State4_Empty, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::g_RaldoThrow_PacketCursor;
	static constexpr game::Id ACTOR_STATE = game::g_RaldoThrow_ActorState;
	static constexpr game::Id SCRATCH_SP = game::g_RaldoThrow_ScratchSP;
	static constexpr game::Id DIR_PTR = game::p_RaldoThrow_Director;
	static constexpr game::Id BLK = game::g_RaldoThrow_BallBlock;
	static constexpr game::Id Q_EMITTER = game::MAG_019_EmitterQueue, Q_DIRECTOR = game::MAG_019_DirectorQueue, Q_BALL = game::MAG_019_BallActorSystemQueue, Q_RALDO = game::MAG_019_RaldoDebrisQueue, Q_STAGE = game::MAG_019_StageWobbleQueue;
	static constexpr game::Id ORIG_Master = game::MAG_019_RaldoThrow_MasterTask;
	static constexpr game::Id ORIG_Ball = game::MAG_019_RaldoThrow_BallActorSystemTask;
	static constexpr game::Id ORIG_Raldo = game::MAG_019_RaldoThrow_RaldoTask;
	static constexpr game::Id ORIG_DebrisHigh = game::MAG_019_RaldoThrow_DebrisHighTask, ORIG_DebrisLow = game::MAG_019_RaldoThrow_DebrisLowTask;
	static constexpr game::Id BALL_DATA = game::unk_17F37BC;
	static constexpr game::Id SOUND_Throw = game::unk_1629FAC;
	static inline uint32_t ENT(uint32_t slot) { return game_addr(game::BattleEntitySlotData) + slot * 0x9C; }

	namespace gx
	{
		static inline uint32_t GTE_MVMVA_ColorV0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_MVMVA_ColorV0))(); }
		static inline uint32_t GTE_MVMVA_LightV0_Bk() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
		static inline uint32_t GTE_SetBackgroundVector(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetBackgroundVector))(a1, a2, a3); }
		static inline uint32_t GTE_SetLightMatrix(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_SetLightMatrix))(a1); }
		static inline uint32_t sub_45DEA0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::sub_45DEA0))(a1); }
		static inline uint32_t sub_45E0B0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadIR123_u16))(a1); }
		static inline uint32_t sub_45E160(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSXY012))(a1, a2, a3); }
		static inline uint32_t sub_45E220(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadSZ3))(a1); }
		static inline uint32_t sub_45E9D0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPF))(); }
		static inline uint32_t sub_45EBF0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPL))(); }
		static inline uint32_t sub_56BDE0(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::VectorNormalS16))(a1, a2); }
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

	static inline uint32_t t1_data() { return MEM<uint32_t>(game_addr(DIR_PTR)); }
	static inline void t1_next_state(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }
	static inline void t4_next_state(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }

	uint32_t __cdecl rt_8A33F0(uint32_t a1);
	uint32_t __cdecl rt_8A3570(uint32_t a1);
	uint32_t __cdecl rt_8A35B0(void);
	uint32_t __cdecl rt_8A36A0(uint32_t a1);
	uint32_t __cdecl rt_8A36D0(uint32_t a1);
	uint32_t __cdecl rt_8A3760(uint32_t a1);
	uint32_t __cdecl rt_8A37E0(uint32_t a1);
	uint32_t __cdecl rt_8A3880(uint32_t a1);
	uint32_t __cdecl rt_8A38C0(uint32_t a1);
	uint32_t __cdecl rt_8A38E0(uint32_t a1);
	uint32_t __cdecl rt_8A3940(uint32_t a1);
	uint32_t __cdecl rt_8A3A90(uint32_t a1);
	uint32_t __cdecl rt_8A3CB0(uint32_t a1);
	uint32_t __cdecl rt_8A3F30(void);
	uint32_t __cdecl rt_8A45E0(uint32_t a1);
	uint32_t __cdecl rt_8A4670(void);
	uint32_t __cdecl rt_8A46D0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl rt_8A4830(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl rt_8A4960(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl rt_8A4B80(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl rt_8A4E10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl rt_8A5290(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl rt_8A5D40(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl rt_8A6240(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl rt_8A69D0(void);
	uint32_t __cdecl rt_8A6A10(uint32_t a1);
	uint32_t __cdecl rt_8A7A40(uint32_t a1, uint32_t a2);
	uint32_t __cdecl rt_8A7B50(uint32_t a1, uint32_t a2);
	uint32_t __cdecl rt_8A7BD0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl rt_8A7C00(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl rt_8A7CA0(uint32_t a1);
	uint32_t __cdecl rt_8A7D20(uint32_t a1);
	uint32_t __cdecl rt_8A7DA0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl rt_8A8580(uint32_t a1, uint32_t a2);
	uint32_t __cdecl rt_8A87A0(uint32_t a1);
	uint32_t __cdecl rt_8A9290(uint32_t a1, uint32_t a2);
	uint32_t __cdecl rt_8A9320(void);
	uint32_t __cdecl rt_8A93B0(uint32_t a1);
	uint32_t __cdecl rt_8A93E0(uint32_t a1);
	uint32_t __cdecl rt_8A94E0(uint32_t a1);
	uint32_t __cdecl rt_8A95F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl rt_8A9CA0(uint32_t a1);
	uint32_t __cdecl rt_8A9D20(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl rt_8A9DA0(uint32_t a1);
	uint32_t __cdecl rt_8A9DD0(uint32_t a1);
	uint32_t __cdecl rt_8A9EC0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl rt_8A9F10(uint32_t a1);
	uint32_t __cdecl rt_8A9F60(uint32_t a1);
	uint32_t __cdecl rt_8AA1D0(uint32_t a1);
	uint32_t __cdecl rt_8AA2E0(uint32_t a1);
	uint32_t __cdecl rt_8AA9B0(uint32_t a1);
	uint32_t __cdecl rt_8AA9D0(uint32_t a1);
	uint32_t __cdecl rt_8AAA30(uint32_t a1);
	uint32_t __cdecl rt_8AAB40(uint32_t a1);
	uint32_t __cdecl rt_8AAB60(uint32_t a1);
	uint32_t __cdecl rt_8AABA0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl rt_8AAC50(uint32_t a1);
	uint32_t __cdecl rt_8AAD90(uint32_t a1);
	uint32_t __cdecl rt_8AAEC0(uint32_t a1);
	uint32_t __cdecl rt_8AAF00(uint32_t a1);
	uint32_t __cdecl rt_8AAF60(uint32_t a1);
	uint32_t __cdecl rt_8AAFE0(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace raldo
{
	uint32_t __cdecl rt_8A33F0(uint32_t a1)
	{
		g_mod = &MOD_019;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_272B8F8)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_019_RaldoThrow_Master_State0_Advance);
		states[1] = game_addr(game::MAG_019_RaldoThrow_Master_State1_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[2] = game_addr(game::MAG_019_RaldoThrow_CarveActorPools);
		states[3] = game_addr(game::MAG_019_RaldoThrow_Master_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_019_RaldoThrow_Master_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_019_RaldoThrow_Master_State5_Advance);
		states[6] = game_addr(game::MAG_019_RaldoThrow_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_019_RaldoThrow_Master_State7_Advance);
		states[8] = game_addr(game::MAG_019_RaldoThrow_Master_State8_Advance);
		states[9] = game_addr(game::MAG_019_RaldoThrow_Master_State9_Finish);
		states[10] = game_addr(game::MAG_019_RaldoThrow_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_27303E4));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_272DDE4));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_272B8F4)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_27303E0));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_272DDE0));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_272B8F4)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_273047E)) = 0;
		MEM<uint16_t>(game_addr(game::unk_273046C)) = 0;
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_EMITTER)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_DIRECTOR)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_BALL)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_RALDO)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_STAGE)));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl rt_8A3570(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::MAG_019_FileArenaCursor));
		MEM<uint32_t>(game_addr(game::MAG_019_ParticlePoolPtr)) = p;
		p += 0x438;
		MEM<uint32_t>(game_addr(game::MAG_019_ActorPoolPtr)) = p;
		p += 0xB7C0;
		MEM<uint32_t>(game_addr(game::MAG_019_FileArenaCursor)) = p;
		rt_8A35B0();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl rt_8A35B0(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_019_ParticlePoolPtr)), 0x438);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_019_ActorPoolPtr)), 0xB7C0);
		MEM<uint16_t>(game_addr(game::MAG_019_ParticleCursor)) = 0;
		MEM<uint16_t>(game_addr(game::MAG_019_ActorCursor)) = 0;
		MEM<uint16_t>(game_addr(game::unk_2730414)) = 0;
		MEM<uint16_t>(game_addr(game::unk_2726908)) = 0;
		return 0;
	}

	uint32_t __cdecl rt_8A36A0(uint32_t a1)
	{
		rt_8A36D0(a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_019_DirectorQueue), game_addr(game::MAG_019_RaldoThrow_DirectorTask), 0x48, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl rt_8A37E0(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			rt_8A3940(a1);
			d = t1_data();
		}
		const uint16_t nxt = U16(d, 0x44);
		if (U16(d, 0x42) != nxt)
		{
			U16(d, 0x42) = nxt;
			a_73A6D0();
		}
		return 0;
	}

	uint32_t __cdecl rt_8A3880(uint32_t a1)
	{
		if (a_73B7E0(1) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl rt_8A38C0(uint32_t a1)
	{
		uint32_t blk = U32(game_addr(game::p_RaldoThrow_Director), 0);
		if (S16(blk, 0x46) < 4) return blk;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl rt_8A38E0(uint32_t a1)
	{
		uint32_t blk = U32(game_addr(game::p_RaldoThrow_Director), 0);
		if (S16(blk, 0x46) <= 0x2D) return blk;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl rt_8A3CB0(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::g_RaldoThrow_ActorState));
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

	uint32_t __cdecl rt_8A3F30(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::g_RaldoThrow_ActorState));
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

	uint32_t __cdecl rt_8A45E0(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::g_RaldoThrow_ActorState), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			rt_8A9320();
			rt_8A69D0();
			a_740700();
			rt_8A4670();
			sys = U32(game_addr(game::g_RaldoThrow_ActorState), 0);
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
		U16(game_addr(game::unk_273047E), 0) = (uint16_t)(U16(game_addr(game::unk_273047E), 0) + c14);
		U16(game_addr(game::unk_273046C), 0) = (uint16_t)(U16(game_addr(game::unk_273046C), 0) + c16);
		return done;
	}

	uint32_t __cdecl rt_8A4670(void)
	{
		uint32_t act = U32(U32(game_addr(game::g_RaldoThrow_ActorState), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::g_RaldoThrow_ActorState), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					rt_8A46D0(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl rt_8A46D0(uint32_t a1, uint32_t a2)
	{
		const uint32_t h = MEM<uint32_t>(game_addr(SCRATCH_SP)) - 0x68;
		U32(h, 0) = U32(a1, 0x170);
		const bool def3a = U16(a2, 0x3A) != 0;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = h;
		U32(h, 0x14) = 0;
		if (!def3a)
			U32(h, 0x14) = 0x30;
		const uint16_t fade = U16(a1, 0x1CE);
		if (fade != 0)
		{
			U32(h, 8) = U32(a1, 0x16C);
			const uint32_t fl = U32(h, 0x14);
			U32(h, 0xC) = (uint32_t)(int32_t)(int16_t)fade;
			U32(h, 0x14) = fl | 0xC0;
		}
		U32(h, 0x10) = 0;
		U16(h, 0x22) = 0x100;
		U16(h, 0x20) = 0x100;
		U16(h, 0x2A) = 0x100;
		U16(h, 0x28) = 0x100;
		const int8_t copies = S8(a1, 0x1D8);
		U16(h, 0x1E) = 0;
		U16(h, 0x1C) = 0;
		U16(h, 0x18) = 0;
		U16(h, 0x1A) = 0;
		U16(h, 0x26) = 0;
		U16(h, 0x24) = 0;
		if (copies == 1)
		{
			x::GTE_SetRotMatrix(a1 + 0xAC);
			x::GTE_SetTransVector(a1 + 0xAC);
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = rt_8A4830(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
		}
		else if (copies > 0)
		{
			uint32_t pos = a1 + 0x19E;
			for (int32_t i = 0; i < S8(a1, 0x1D8); i++, pos += 8)
			{
				S32(a1, 0xC0) = S16(pos, -2);
				S32(a1, 0xC4) = S16(pos, 0);
				S32(a1, 0xC8) = S16(pos, 2);
				x::GTE_SetRotMatrix(a1 + 0xAC);
				x::GTE_SetTransVector(a1 + 0xAC);
				MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = rt_8A4830(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
			}
		}
		const uint32_t top = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 0x68;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = top;
		return top;
	}

	uint32_t __cdecl rt_8A4830(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		static const Draw lists[8] = { rt_8A4960, rt_8A4B80, rt_8A4E10, rt_8A5290, a_73D770, a_73D9C0, rt_8A5D40, rt_8A6240 };
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

	uint32_t __cdecl rt_8A4960(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl rt_8A4B80(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl rt_8A4E10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl rt_8A5290(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl rt_8A6240(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl rt_8A69D0(void)
	{
		uint32_t act = U32(U32(game_addr(game::g_RaldoThrow_ActorState), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				rt_8A7CA0(act);
			else if (type == 1)
				rt_8A6A10(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl rt_8A6A10(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::g_RaldoThrow_ActorState));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (rt_8A7B50(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::g_RaldoThrow_ActorState));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl rt_8A7A40(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::g_RaldoThrow_ActorState), 0);
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

	uint32_t __cdecl rt_8A7B50(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				rt_8A7BD0(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			rt_8A7BD0(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			rt_8A7BD0(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl rt_8A7BD0(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return rt_8A7C00(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl rt_8A7C00(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_019_ParticlePoolPtr));
		int32_t idx = MEM<int16_t>(game_addr(game::MAG_019_ParticleCursor));
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
			MEM<uint16_t>(game_addr(game::MAG_019_ParticleCursor)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::MAG_019_ParticleCursor)) = 0;
		return slot;
	}

	uint32_t __cdecl rt_8A7CA0(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_019_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_019_Actor_InterpolatePosition), a1);
		rt_8A7D20(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl rt_8A7D20(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::g_RaldoThrow_ActorState), 0);
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
				rt_8A7DA0(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			rt_8A7DA0(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl rt_8A7DA0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::g_RaldoThrow_ScratchSP), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::g_RaldoThrow_ScratchSP), 0) = sp;
		uint32_t node = rt_8A8580(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::g_RaldoThrow_ActorState), 0);
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
		rt_8A9290(node, desc);
		U32(game_addr(game::g_RaldoThrow_ScratchSP), 0) = U32(game_addr(game::g_RaldoThrow_ScratchSP), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl rt_8A8580(uint32_t a1, uint32_t a2)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_019_ActorPoolPtr));
		int32_t i = MEM<int16_t>(game_addr(game::MAG_019_ActorCursor));
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
			if (i >= 0x45)
				i = 0;
			tries++;
			if (tries >= 0x46)
				break;
		}
		i++;
		if (i >= 0x45)
			MEM<uint16_t>(game_addr(game::MAG_019_ActorCursor)) = 0;
		else
			MEM<uint16_t>(game_addr(game::MAG_019_ActorCursor)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl rt_8A87A0(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::g_RaldoThrow_ActorState), 0);
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
				uint32_t g = U32(game_addr(game::g_RaldoThrow_ActorState), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::g_RaldoThrow_ActorState), 0);
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

	uint32_t __cdecl rt_8A9290(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			rt_8A7C00(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			rt_8A7C00(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			rt_8A7C00(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			rt_8A7C00(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl rt_8A9320(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::g_RaldoThrow_ActorState));
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
					rt_8A7C00(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::g_RaldoThrow_ActorState));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				rt_8A7C00(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::g_RaldoThrow_ActorState));
			}
		}
		return 0;
	}

	uint32_t __cdecl rt_8A93B0(uint32_t a1)
	{
		if (rt_8A45E0(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl rt_8A94E0(uint32_t a1)
	{
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)((int32_t)S16(a1, 0xAC) * 0x9C);
		ff8fx::copy<(0x20)>((void *)(a1 + 0x20), (const void *)(ent + 0x40));
		const uint32_t pose = ent + 0x60;
		x::BattleModel_BuildBoneMatricesFromPose(pose);
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(a1 + 0x20);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(a1 + 0x40);
		x::GTE_LoadIRFromMatrixColumn(a1 + 0x22);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(a1 + 0x42);
		x::GTE_LoadIRFromMatrixColumn(a1 + 0x24);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(a1 + 0x44);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(a1 + 0x34);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(a1 + 0x54);
		uint32_t cp = U32(a1, 0x6C);
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4040;
		const uint32_t r = x::sub_5088A0(ent, ot, 0x10, U32(cp, 0));
		cp = U32(a1, 0x6C);
		const uint32_t ot44 = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U32(cp, 0) = r;
		rt_8A95F0(U32(ent, 0x64), ot44, 4, a1);
		const uint32_t wpn = U32(ent, 0x78);
		if (wpn != 0)
			rt_8A95F0(U32(wpn, 4), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, a1);
		x::BattleModel_BuildBoneMatricesFromPose(pose);
		return 0;
	}

	uint32_t __cdecl rt_8A95F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t blk = a4;
		const uint32_t verts = U32(U32(blk, 0x64), 4);
		const uint32_t ent = U32(blk, 0x68);
		uint32_t cursor = U32(U32(blk, 0x6C), 0);
		const uint32_t bones = U32(a1, 0) + 0x10;
		uint32_t tab = U32(a1, 4);
		const int32_t count = S32(tab, 0);
		const uint32_t w = x::Field_Alloc(0xA8);
		tab += 4;
		U16(w, 0x9E) = U16(blk, 0xA4);
		U16(w, 0x9C) = (uint16_t)(0x1000 - U16(blk, 0xA4));
		U32(w, 0x5C) = U32(blk, 0x74);
		U32(w, 0x60) = U32(blk, 0x78);
		U16(w, 0xA0) = U16(blk, 0xA6);
		U16(w, 0xA4) = U16(blk, 0xAA);
		U32(w, 0x98) = U32(ent, 0x7C);
		const uint32_t rgb = U32(ent, 0x28) & 0xFFFFFF;
		ff8fx::copy<(0x20)>((void *)w, (const void *)blk);
		U16(w, 0xA2) = U16(blk, 0xA8);
		U32(w, 0x90) = rgb | 0x24000000;
		U32(w, 0x94) = rgb | 0x2C000000;
		if (U16(blk, 0x9C) != 0)
			x::sub_8DD7E0(w, (uint32_t)(int32_t)S16(blk, 0x9C));
		if (U16(blk, 0xA0) != 0)
			x::sub_8DD960(w, (uint32_t)(int32_t)S16(blk, 0xA0));
		if (U16(blk, 0x9E) != 0)
			x::MAG_022_sub_8DD8A0(w, (uint32_t)(int32_t)S16(blk, 0x9E));
		gx::sub_45DEA0(w);
		x::GTE_SetRotMatrix(blk + 0x40);
		x::GTE_SetTransVector(blk + 0x40);
		for (int32_t i = 0; i < count; i++)
		{
			const uint32_t obj = U32(a1, 4) + U32(tab, 0);
			tab += 4;
			if (((U32(w, 0x98) >> (i & 31)) & 1) == 0)
				continue;
			const uint32_t groups = obj + 2;
			uint32_t p = groups;
			uint32_t wv = verts;
			for (int32_t g = (int32_t)S16(obj, 0); g > 0; g--)
			{
				const int32_t bone = (int32_t)S16(p, 0);
				p += 2;
				const uint32_t m = (uint32_t)(bone * 3 * 16) + bones + 0x10;
				gx::GTE_SetLightMatrix(m);
				gx::GTE_SetBackgroundVector(U32(m, 0x14), U32(m, 0x18), U32(m, 0x1C));
				const int32_t nv = (int32_t)S16(p, 0);
				p += 2;
				const uint32_t v = w + 0x4C;
				for (int32_t k = nv; k > 0; k--)
				{
					const uint16_t vx = U16(p, 0);
					const uint16_t vy = U16(p, 2);
					p += 4;
					U16(v, 0) = vx;
					U16(w, 0x4E) = vy;
					U16(w, 0x50) = U16(p, 0);
					p += 2;
					x::GTE_LoadV0(v);
					gx::GTE_MVMVA_LightV0_Bk();
					x::GTE_StoreIR123(v);
					U16(v, 0) = (uint16_t)(U16(v, 0) - U16(w, 0x5C));
					U16(w, 0x4E) = (uint16_t)(U16(w, 0x4E) - U16(w, 0x5E));
					U16(w, 0x50) = (uint16_t)(U16(w, 0x50) - U16(w, 0x60));
					U32(w, 0x54) = U32(v, 0);
					U32(w, 0x58) = U32(v, 4);
					gx::sub_56BDE0(v, v);
					S16(v, 0) = (int16_t)(mul32(S16(w, 0xA0), S16(v, 0)) / 4096);
					S16(w, 0x4E) = (int16_t)(mul32(S16(w, 0xA2), S16(w, 0x4E)) / 4096);
					S16(w, 0x50) = (int16_t)(mul32(S16(w, 0xA4), S16(w, 0x50)) / 4096);
					x::set_dword_1CA8A30((uint32_t)(int32_t)S16(w, 0x9C));
					gx::sub_45E0B0(w + 0x54);
					gx::sub_45E9D0();
					x::set_dword_1CA8A30((uint32_t)(int32_t)S16(w, 0x9E));
					gx::sub_45E0B0(v);
					gx::sub_45EBF0();
					x::GTE_StoreIR123(v);
					x::GTE_LoadV0(v);
					gx::GTE_MVMVA_ColorV0();
					x::GTE_StoreIR123(v);
					U16(v, 0) = (uint16_t)(U16(v, 0) + U16(w, 0x5C));
					U16(w, 0x4E) = (uint16_t)(U16(w, 0x4E) + U16(w, 0x5E));
					U16(w, 0x50) = (uint16_t)(U16(w, 0x50) + U16(w, 0x60));
					U32(wv, 0) = U32(v, 0);
					U32(wv, 4) = U32(v, 4);
					wv += 0x14;
				}
			}
			uint32_t q = groups;
			uint32_t vp = verts;
			for (int32_t g = (int32_t)S16(obj, 0); g > 0; g--)
			{
				const int32_t nv = (int32_t)S16(q, 2);
				q += 4;
				if (nv <= 0)
					continue;
				const uint32_t next = q + (uint32_t)(nv * 3 * 2);
				uint32_t zp = vp + 0xC;
				for (int32_t k = nv; k > 0; k--)
				{
					U32(w, 0x34) = U32(vp, 0);
					U32(w, 0x38) = U32(vp, 4);
					x::GTE_LoadV0(w + 0x34);
					x::GTE_RTPS();
					x::GTE_ReadSXY2(zp - 4);
					gx::sub_45E220(zp);
					vp += 0x14;
					zp += 0x14;
				}
				q = next;
			}
			uint32_t f = (q + 3) & 0xFFFFFFFCu;
			const int32_t ntri = (int32_t)S16(f, 0);
			const int32_t nquad = (int32_t)S16(f, 2);
			f += 0xC;
			uint32_t pk = cursor;
			for (int32_t k = ntri; k > 0; k--, f += 0x10)
			{
				const uint32_t i2 = U16(f, 4) & 0xFFF;
				const uint32_t i1 = U16(f, 2) & 0xFFF;
				const uint32_t i0 = U16(f, 0) & 0xFFF;
				U32(w, 0x88) = i2;
				U32(w, 0x84) = i1;
				U32(w, 0x80) = i0;
				const uint32_t s2 = U32(verts + i2 * 20, 8);
				const uint32_t s1 = U32(verts + i1 * 20, 8);
				const uint32_t s0 = U32(verts + i0 * 20, 8);
				U32(w, 0x24) = s0;
				U32(w, 0x28) = s1;
				U32(w, 0x2C) = s2;
				gx::sub_45E160(s0, s1, s2);
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(w + 0x7C);
				if (S32(w, 0x7C) <= 0)
					continue;
				U32(pk, 8) = U32(w, 0x24);
				U32(pk, 0x10) = U32(w, 0x28);
				U32(pk, 0x18) = U32(w, 0x2C);
				U32(pk, 0xC) = U32(f, 8);
				U32(pk, 0) = 0x7000000;
				U32(pk, 0x14) = U32(f, 0xC);
				U16(pk, 0x1C) = U16(f, 6);
				U32(pk, 4) = U32(w, 0x90);
				if (U8(f, 0xF) & 2)
					U8(pk, 7) |= 2;
				const int32_t z2 = S32(verts + U32(w, 0x88) * 20, 0xC);
				const int32_t z1 = S32(verts + U32(w, 0x84) * 20, 0xC);
				const int32_t z0 = S32(verts + U32(w, 0x80) * 20, 0xC);
				const int32_t sum = add32(add32(z1, z0), z2);
				int32_t hq = (int32_t)(((int64_t)sum * 0x55555556LL) >> 32);
				hq = add32(hq, (int32_t)((uint32_t)hq >> 31));
				const int32_t key = hq >> (a3 & 31);
				U32(w, 0x78) = (uint32_t)key;
				x::SSIGPU_InsertPrimDepthKeys(a2 + (uint32_t)key * 4, pk, (uint32_t)z0, (uint32_t)z1, (uint32_t)z2, 0);
				pk += 0x20;
			}
			if (nquad > 0)
			{
				f += 4;
				for (int32_t k = nquad; k > 0; k--, f += 0x14)
				{
					const uint32_t i2 = U16(f, 0) & 0xFFF;
					const uint32_t i1 = U16(f, -2) & 0xFFF;
					const uint32_t i0 = U16(f, -4) & 0xFFF;
					U32(w, 0x88) = i2;
					U32(w, 0x84) = i1;
					U32(w, 0x80) = i0;
					const uint32_t s2 = U32(verts + i2 * 20, 8);
					const uint32_t s1 = U32(verts + i1 * 20, 8);
					const uint32_t s0 = U32(verts + i0 * 20, 8);
					U32(w, 0x24) = s0;
					U32(w, 0x28) = s1;
					U32(w, 0x2C) = s2;
					gx::sub_45E160(s0, s1, s2);
					x::GTE_NCLIP();
					x::GTE_ReadMAC0(w + 0x7C);
					if (S32(w, 0x7C) <= 0)
						continue;
					const uint32_t i3 = U16(f, 2) & 0xFFF;
					U32(pk, 8) = U32(w, 0x24);
					U32(w, 0x8C) = i3;
					U32(pk, 0x10) = U32(w, 0x28);
					U32(pk, 0) = 0x9000000;
					const uint32_t v3 = verts + i3 * 20;
					const uint32_t s3 = U32(v3, 8);
					U32(w, 0x30) = s3;
					U32(pk, 0x20) = s3;
					U32(pk, 0x18) = U32(w, 0x2C);
					U32(pk, 0xC) = U32(f, 4);
					U32(pk, 0x14) = U32(f, 8);
					U16(pk, 0x1C) = U16(f, 0xC);
					U16(pk, 0x24) = U16(f, 0xE);
					U32(pk, 4) = U32(w, 0x94);
					if (U8(f, 0xB) & 2)
						U8(pk, 7) |= 2;
					const int32_t z3 = S32(v3, 0xC);
					const int32_t z2 = S32(verts + U32(w, 0x88) * 20, 0xC);
					const int32_t z1 = S32(verts + U32(w, 0x84) * 20, 0xC);
					const int32_t z0 = S32(verts + U32(w, 0x80) * 20, 0xC);
					const int32_t sum = add32(add32(add32(z0, z3), z1), z2);
					const int32_t key = sum >> ((a3 + 2) & 31);
					U32(w, 0x78) = (uint32_t)key;
					x::SSIGPU_InsertPrimDepthKeys(a2 + (uint32_t)key * 4, pk, (uint32_t)z0, (uint32_t)z1, (uint32_t)z2, (uint32_t)z3);
					pk += 0x28;
				}
			}
			cursor = pk;
		}
		U32(U32(blk, 0x6C), 0) = cursor;
		x::Field_Free(0xA8);
		return 0;
	}

	uint32_t __cdecl rt_8A9D20(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		x::MAG_007_sub_8DCC00(a1, 0xC4);
		U32(a1, 0x68) = a5;
		U32(a1, 0x60) = a2;
		U32(a1, 0x6C) = game_addr(game::BATTLE_PACKET_WRITE_CURSOR);
		U32(a1, 0x70) = game_addr(game::BATTLE_PACKET_WRITE_CURSOR);
		U16(a1, 0xAC) = (uint16_t)a6;
		U32(a3, 4) = a4;
		U32(a1, 0x64) = a3;
		U16(a3, 0x14) = 0;
		U16(a3, 0x16) = 0;
		U16(a3, 0x18) = 0x140;
		U16(a3, 0x1A) = 0;
		U8(a3, 0x1C) = 0x80;
		U8(a3, 0x1D) = 0x80;
		U8(a3, 0x1E) = 0x80;
		U32(a3, 0x20) = 0xFFFFFFFF;
		U16(a3, 0x24) = 0;
		return a1;
	}

	uint32_t __cdecl rt_8A9F10(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[2];
		states[0] = game_addr(game::MAG_019_RaldoThrow_DebrisBurst);
		states[1] = game_addr(game::MAG_019_RaldoThrow_DebrisSpawner_State1_Empty);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl rt_8AA2E0(uint32_t a1)
	{
		if ((U8(a1, 0x26) & 4) != 0)
			return 0;
		uint32_t hdr = x::Field_Alloc(0xD8);
		uint32_t m = hdr + 0xB8;
		x::UnpackRotationMatrix(game_addr(game::unk_1D97778), m);
		int32_t px = S16(a1, 0x1C);
		int32_t py = S16(a1, 0x1E);
		int32_t pz = S16(a1, 0x20);
		uint32_t vec = hdr + 0xCC;
		S32(hdr, 0xD0) = py;
		S32(hdr, 0xD4) = pz;
		S32(vec, 0) = px;
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(m);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(m);
		x::GTE_LoadIRFromMatrixColumn(hdr + 0xBA);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(hdr + 0xBA);
		x::GTE_LoadIRFromMatrixColumn(hdr + 0xBC);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(hdr + 0xBC);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(vec);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(vec);
		x::GTE_SetRotMatrix(m);
		x::GTE_SetTransVector(m);
		uint32_t data = U32(a1, 0x4C);
		uint16_t frame = U16(a1, 0x50);
		uint16_t depth = U16(a1, 0x56);
		U32(hdr, 0) = data;
		uint32_t cursor = U32(game_addr(game::BATTLE_PACKET_WRITE_CURSOR), 0);
		U16(hdr, 4) = frame;
		uint32_t ot = U32(game_addr(game::g_Battle_FrameRenderListBase), 0) + 0x44;
		U16(hdr, 0x24) = 0;
		U16(hdr, 0xB4) = depth;
		U32(game_addr(game::BATTLE_PACKET_WRITE_CURSOR), 0) = a_7435E0(hdr, ot, 2, cursor);
		x::Field_Free(0xD8);
		return 0;
	}

	uint32_t __cdecl rt_8AA9B0(uint32_t a1)
	{
		uint32_t node = a1;
		uint8_t st = U8(node, 0x29);
		U32(node, 0x4c) = game_addr(game::unk_1629FB0);
		st++;
		U16(node, 0x56) = 0xfff0;
		U16(node, 0x52) = 9;
		U8(node, 0x29) = st;
		return 0;
	}

	uint32_t __cdecl rt_8AA9D0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl rt_8AAB40(uint32_t a1)
	{
		uint32_t node = a1;
		uint8_t st = U8(node, 0x29);
		U32(node, 0x4c) = game_addr(game::unk_162A0C0);
		st++;
		U16(node, 0x56) = 0xfff0;
		U16(node, 0x52) = 0xB;
		U8(node, 0x29) = st;
		return 0;
	}

	uint32_t __cdecl rt_8AAB60(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl rt_8AAFE0(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::p_RaldoThrow_Director));
		if (U16(d, 0x48) != 1)
			return 0;
		const uint32_t node = a1;
		const int32_t act = S8(node, 0x2A);
		const uint32_t ctx = U32(node, 0xC);
		const int32_t sub = S8(node, 0x2B);
		const uint32_t results = U32(ctx, 4);
		const uint32_t arr = U32(results + (uint32_t)(act * 20), 8);
		x::ApplyActionResultToTarget(arr + (uint32_t)(sub * 24));
		t4_next_state(node);
		return 0;
	}

	uint32_t __cdecl rt_8A36D0(uint32_t a1)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(DIR_PTR)), 0x54);
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		const uint32_t tgt = ENT(U8(a1, 0x2D));
		U32(d, 8) = U32(tgt, 0x1C);
		U32(d, 0xC) = U32(tgt, 0x20);
		const uint32_t cst = ENT(U8(a1, 0x2C));
		U32(d, 0) = U32(cst, 0x1C);
		U32(d, 4) = U32(cst, 0x20);
		const int16_t dz = (int16_t)(U16(d, 0xC) - U16(d, 4));
		const int16_t dx = (int16_t)(U16(d, 8) - U16(d, 0));
		const uint32_t ang = x::CartesianToGameAngle((uint32_t)(int32_t)dx, (uint32_t)(int32_t)dz);
		U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x4A) = (uint16_t)((ang - 0x800) & 0xFFF);
		return 0;
	}

	uint32_t __cdecl rt_8A3760(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[6];
		states[0] = game_addr(game::MAG_019_RaldoThrow_Director_State0_WaitDirGate1);
		states[1] = game_addr(game::MAG_019_RaldoThrow_Director_State1_WaitDirPhase1);
		states[2] = game_addr(game::MAG_019_RaldoThrow_Director_State2_WaitCutFrame4);
		states[3] = game_addr(game::MAG_019_RaldoThrow_Director_State3_WaitCutFrame2D);
		states[4] = game_addr(game::MAG_019_RaldoThrow_Director_State4_Finish);
		states[5] = game_addr(game::MAG_019_RaldoThrow_Director_State5_Empty);
		rt_8A37E0(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl rt_8A3940(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x40) != 1)
			return 0;
		const uint32_t node = a1;
		a_73C100(node, game_addr(ORIG_Ball), game_addr(BALL_DATA), 0x1C, 0, 4);
		const uint32_t cst = ENT(U8(node, 0x2C));
		const uint32_t raldo = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_RALDO), game_addr(ORIG_Raldo), 0xB8, node);
		uint8_t slot = 0;
		for (uint32_t e = ENT(0); e < ENT(7); e += 0x9C, slot++)
		{
			if ((U8(e, 0) & 2) == 0 || U8(e, 4) != 0x70)
				continue;
			if (U16(e, 0x1C) != U16(cst, 0x1C) || U16(e, 0x1E) != U16(cst, 0x1E) || U16(e, 0x20) != U16(cst, 0x20))
				continue;
			U8(raldo, 0x2C) = slot;
		}
		return 0;
	}

	uint32_t __cdecl rt_8A3A90(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < S16(node, 0x298))
			return 0;
		const uint32_t e = ENT(U8(node, 0x2C));
		const uint32_t xy = (U32(e, 0x1C) & 0xFFFFu) | 0xFD000000u;
		const uint32_t z = U32(e, 0x20);
		U32(node, 0x290) = xy;
		U32(node, 0x294) = z;
		a_73F990(node);
		rt_8A45E0(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl rt_8A5D40(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		static const int32_t U_OFF[3] = { 0x0C, 0x18, 0x24 };
		static const int32_t V_OFF[3] = { 0x0D, 0x19, 0x25 };
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		const uint32_t list = U32(ctx, 0x2C);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = rec;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return pkt;
		}
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			const uint32_t flags = U32(ctx, 0x14);
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
			const uint32_t uv0 = U32(rec, 0x0C);
			const uint32_t w8 = U32(rec, 8);
			const uint32_t uv1 = U32(rec, 0x10);
			U32(pkt, 0x0C) = uv0;
			U32(pkt, 0x18) = uv1;
			U32(pkt, 0x24) = w8 >> 16;
			x::GTE_ReadFLAG(ctx + 0x3C);
			if (!(U32(ctx, 0x3C) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) >= 0 || (U8(ctx, 0x14) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x14, pkt + 0x20);
					x::GTE_AVSZ3();
					if (t3_out_x(S16(pkt, 8))) clip = 1;
					if (t3_out_x(S16(pkt, 0x14))) clip |= 2;
					if (t3_out_x(S16(pkt, 0x20))) clip |= 4;
					if (t3_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (t3_out_y(S16(pkt, 0x16))) clip |= 0x20;
					if (t3_out_y(S16(pkt, 0x22))) clip |= 0x40;
					if ((uint8_t)(clip & 7) != 7 && (uint8_t)(clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x38);
						if (U8(ctx, 0x14) & 0x80)
						{
							x::sub_45E120(rec + 0x14, rec + 0x18, pkt + 4);
							x::set_dword_1CA8A30(U32(ctx, 0x0C));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0x10, pkt + 0x1C, pkt + 4);
						}
						else
						{
							const uint32_t c1 = U32(rec, 0x14), c2 = U32(rec, 0x18);
							U32(pkt, 0x10) = c1;
							U32(pkt, 0x1C) = c2;
						}
						int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
						S32(ctx, 0x38) = z;
						if (z < 0)
							S32(ctx, 0x38) = 0;
						z = S32(ctx, 0x38) >> (a3 & 31);
						const uint32_t ot = a2 + (uint32_t)z * 4;
						const uint16_t du = U16(ctx, 0x18);
						const uint16_t dv = U16(ctx, 0x1A);
						if ((uint16_t)(du | dv) != 0)
						{
							if (du != 0)
								t3_scroll(pkt, U_OFF, 3, du, U8(ctx, 0x28));
							const uint16_t dv2 = U16(ctx, 0x1A);
							if (dv2 != 0)
								t3_scroll(pkt, V_OFF, 3, dv2, U8(ctx, 0x2A));
							const uint32_t w1 = pkt + 0x28;
							U32(w1, 0) = 0x2000000;
							const uint32_t t1 = (ctx + 0x1C) != 0 ? t3_texwin(ctx + 0x1C) : 0;
							U32(w1, 4) = t1;
							U32(w1, 8) = 0;
							x::SSIGPU_InsertPrimAutoDepth(ot, w1);
							x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
							const uint32_t w2 = pkt + 0x34;
							U32(w2, 0) = 0x2000000;
							const uint32_t t2 = (ctx + 0x24) != 0 ? t3_texwin(ctx + 0x24) : 0;
							U32(w2, 4) = t2;
							U32(w2, 8) = 0;
							x::SSIGPU_InsertPrimAutoDepth(ot, w2);
							pkt += 0x40;
						}
						else
						{
							x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
							pkt += 0x28;
						}
					}
				}
			}
			rec += 0x1C;
		} while (--count);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl rt_8A93E0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[10];
		states[0] = game_addr(game::MAG_019_RaldoThrow_Raldo_Setup);
		states[1] = game_addr(game::MAG_019_RaldoThrow_Raldo_Sound);
		states[2] = game_addr(game::MAG_019_RaldoThrow_Raldo_Take);
		states[3] = game_addr(game::MAG_019_RaldoThrow_Raldo_Throw);
		states[4] = game_addr(game::MAG_019_RaldoThrow_Raldo_Rebound);
		states[5] = game_addr(game::MAG_019_RaldoThrow_Raldo_Unmorph);
		states[6] = game_addr(game::MAG_019_RaldoThrow_Raldo_GiveBack);
		states[7] = game_addr(game::MAG_019_RaldoThrow_Raldo_RestoreRotation);
		states[8] = game_addr(game::MAG_019_RaldoThrow_Raldo_State8_Finish);
		states[9] = game_addr(game::MAG_019_RaldoThrow_Raldo_State9_Empty);
		callp(states[S8(node, 0x29)], node);
		if ((U8(node, 0x26) & 8) != 0)
		{
			const uint32_t ent = ENT(U8(node, 0x2C));
			const int16_t px = (int16_t)(MEM<uint16_t>(game_addr(BLK) + 0x7C) - MEM<uint16_t>(game_addr(BLK) + 0x74));
			S16(ent, 0x1C) = px;
			S32(ent, 0x54) = px;
			const int16_t py = (int16_t)(MEM<uint16_t>(game_addr(BLK) + 0x7E) - MEM<uint16_t>(game_addr(BLK) + 0x76));
			S16(ent, 0x1E) = py;
			S32(ent, 0x58) = py;
			const int16_t pz = (int16_t)(MEM<uint16_t>(game_addr(BLK) + 0x80) - MEM<uint16_t>(game_addr(BLK) + 0x78));
			S16(ent, 0x20) = pz;
			S32(ent, 0x5C) = pz;
			rt_8A94E0(game_addr(BLK));
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl rt_8A9CA0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint8_t slot = U8(node, 0x2C);
		const uint32_t e = ENT(slot);
		rt_8A9D20(game_addr(BLK), game_addr(game::unk_2726AC0), game_addr(game::MAG_019_GeometryDescriptor), game_addr(game::g_RaldoThrow_BallVertexScratch), e, slot);
		const uint32_t z = U32(e, 0x20);
		const uint32_t r0 = U32(e, 0xC);
		const uint32_t xy = U32(e, 0x1C);
		MEM<uint32_t>(game_addr(game::unk_27269C4)) = z;
		MEM<uint32_t>(game_addr(game::unk_27269C8)) = r0;
		MEM<uint32_t>(game_addr(game::unk_27269C0)) = xy;
		const uint32_t r1 = U32(e, 0x10);
		U16(e, 0xE) = U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x4A);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		MEM<uint32_t>(game_addr(game::unk_27269CC)) = r1;
		return 0;
	}

	uint32_t __cdecl rt_8A9DA0(uint32_t a1)
	{
		if (S16(a1, 0x24) < 0x1C)
			return 0;
		x::BdPlaySE(game_addr(SOUND_Throw), 0, 0x80);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl rt_8A9DD0(uint32_t a1)
	{
		const uint32_t node = a1;
		U8(node, 0x26) = (uint8_t)(U8(node, 0x26) | 8);
		const uint32_t e = ENT(U8(node, 0x2C));
		U8(e, 0) = (uint8_t)(U8(e, 0) | 4);
		rt_8AABA0(node, game_addr(BLK));
		MEM<uint16_t>(game_addr(game::unk_27269B8)) = 0x1C0;
		MEM<uint16_t>(game_addr(game::unk_27269BA)) = 0x1C0;
		MEM<uint16_t>(game_addr(game::unk_27269B6)) = 0x1C0;
		x::MAG_022_sub_8DD770(game_addr(BLK));
		const uint32_t xy = U32(e, 0x1C);
		const uint16_t py = MEM<uint16_t>(game_addr(game::unk_2726986));
		MEM<uint32_t>(game_addr(game::unk_272698C)) = xy;
		MEM<uint16_t>(game_addr(game::unk_272698E)) = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_272698E)) + py);
		const uint32_t z = U32(e, 0x20);
		const uint32_t tgt = ENT(U8(node, 0x2D));
		const uint32_t pos = MEM<uint32_t>(game_addr(game::unk_272698C));
		MEM<uint32_t>(game_addr(game::unk_2726990)) = z;
		MEM<uint32_t>(game_addr(game::unk_272699C)) = pos;
		MEM<uint32_t>(game_addr(game::unk_27269A0)) = z;
		x::GetEffectSpawnPosition(tgt, 0xF1, 0, game_addr(game::unk_2726994));
		MEM<uint16_t>(game_addr(game::unk_27269A6)) = 0xFE80;
		MEM<uint16_t>(game_addr(game::unk_27269D2)) = 0;
		rt_8A9EC0(node, 2);
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_STAGE), game_addr(game::Effect_ScreenTintTask), 0x40, node);
		U32(t, 0x34) = game_addr(game::unk_162A470);
		U16(t, 0x38) = 9;
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl rt_8A9EC0(uint32_t a1, uint32_t a2)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_RALDO), game_addr(game::MAG_019_RaldoThrow_DebrisSpawnerTask), 0xB8, a1);
		const uint16_t n = (uint16_t)a2;
		const uint32_t xy = MEM<uint32_t>(game_addr(game::unk_272698C));
		U16(t, 0xB6) = n;
		const uint32_t z = MEM<uint32_t>(game_addr(game::unk_2726990));
		U32(t, 0x1C) = xy;
		U32(t, 0x20) = z;
		U16(t, 0x1E) = 0;
		return t;
	}

	static inline void rt_debris_burst(uint32_t t, uint32_t src, int32_t r0, int32_t v0, bool up)
	{
		uint32_t g = t + 0xAC;
		uint32_t p = t + 0x6C;
		for (int k = 4; k != 0; k--)
		{
			const int32_t r1 = (int32_t)x::CrtRand();
			const int32_t ang = (int32_t)(int16_t)(r1 % 0xFFF);
			const int32_t r2 = (int32_t)x::CrtRand();
			const int32_t rad = (int32_t)(int16_t)((r2 & 0xFF) + r0);
			const int32_t r3 = (int32_t)x::CrtRand();
			const int32_t spd = (int32_t)(int16_t)((r3 & 0xFFF) + v0);
			U32(p, 0) = U32(src, 0x1C);
			U32(p, 4) = U32(src, 0x20);
			U16(p, 0) = (uint16_t)(U16(p, 0) + (uint16_t)(mul32((int32_t)x::computeSin((uint32_t)ang), rad) / 4096));
			U16(p, 4) = (uint16_t)(U16(p, 4) + (uint16_t)(mul32((int32_t)x::computeCosine((uint32_t)ang), rad) / 4096));
			U16(p, 0x20) = (uint16_t)(mul32((int32_t)x::computeSin((uint32_t)ang), spd) / 4096);
			U16(p, 0x24) = (uint16_t)(mul32((int32_t)x::computeCosine((uint32_t)ang), spd) / 4096);
			if (up)
			{
				const int32_t r4 = (int32_t)x::CrtRand();
				U16(p, 0x22) = (uint16_t)(-0x800 - (r4 & 0xFFF));
				const int32_t r5 = (int32_t)x::CrtRand();
				U16(g, 0) = (uint16_t)((r5 & 0x7F) + 0xF0);
			}
			else
			{
				const int32_t r4 = (int32_t)x::CrtRand();
				U16(g, 0) = (uint16_t)(-0x30 - (r4 & 0x1F));
			}
			g += 2;
			p += 8;
		}
	}

	uint32_t __cdecl rt_8A9F60(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t t1 = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_RALDO), game_addr(game::MAG_019_RaldoThrow_DebrisLowTask), 0xB8, node);
		rt_debris_burst(t1, node, 0x40, 0x100, false);
		const uint32_t t2 = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_RALDO), game_addr(game::MAG_019_RaldoThrow_DebrisHighTask), 0xB8, node);
		rt_debris_burst(t2, node, 0x80, 0x800, true);
		if ((int32_t)S16(node, 0x24) >= (int32_t)S16(node, 0xB6) - 1)
		{
			U8(node, 0x26) = (uint8_t)(U8(node, 0x26) | 1);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	static inline void rt_debris_move(uint32_t node, int k)
	{
		const uint32_t v = node + 0x8C + 8 * k;
		const uint32_t p = node + 0x6C + 8 * k;
		U16(v, 2) = (uint16_t)(U16(v, 2) + U16(node, 0xAC + 2 * k));
		for (int j = 0; j < 3; j++)
		{
			const int16_t c = S16(v, 2 * j);
			S16(v, 2 * j) = (int16_t)(c - c / 4);
		}
		for (int j = 0; j < 3; j++)
			U16(p, 2 * j) = (uint16_t)(U16(p, 2 * j) + (uint16_t)(S16(v, 2 * j) / 16));
	}

	uint32_t __cdecl rt_8AA1D0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_019_RaldoThrow_DebrisHigh_State0_InitSprite);
		states[1] = game_addr(game::MAG_019_RaldoThrow_DebrisHigh_State1_Animate);
		states[2] = game_addr(game::MAG_019_RaldoThrow_DebrisHigh_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		for (int k = 0; k < 4; k++)
		{
			U32(node, 0x1C) = U32(node, 0x6C + 8 * k);
			U32(node, 0x20) = U32(node, 0x70 + 8 * k);
			rt_8AA2E0(node);
			rt_debris_move(node, k);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl rt_8AAA30(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_019_RaldoThrow_DebrisLow_State0_InitSprite);
		states[1] = game_addr(game::MAG_019_RaldoThrow_DebrisLow_State1_Animate);
		states[2] = game_addr(game::MAG_019_RaldoThrow_DebrisLow_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		for (int k = 0; k < 4; k++)
		{
			U32(node, 0x1C) = U32(node, 0x6C + 8 * k);
			U32(node, 0x20) = U32(node, 0x70 + 8 * k);
			rt_8AA2E0(node);
			rt_debris_move(node, k);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl rt_8AABA0(uint32_t a1, uint32_t a2)
	{
		const uint32_t e = ENT(U8(a1, 0x2C));
		uint32_t mat[8];
		const uint32_t xy = U32(e, 0x1C);
		ff8fx::copy<(0x20)>(mat, (const void *)(e + 0x40));
		const uint32_t z = U32(e, 0x20);
		U16(e, 0x1C) = 0;
		U16(e, 0x1E) = 0;
		U16(e, 0x20) = 0;
		x::MAG_022_sub_8DD770(e + 0x40);
		const uint32_t blk = a2;
		x::GetEffectSpawnPosition(e, 0xF1, 0, blk + 0x74);
		const uint16_t ex = U16(e, 0x1C), ey = U16(e, 0x1E), ez = U16(e, 0x20);
		U16(blk, 0x74) = (uint16_t)(U16(blk, 0x74) - ex);
		U16(blk, 0x76) = (uint16_t)(U16(blk, 0x76) - ey);
		U16(blk, 0x78) = (uint16_t)(U16(blk, 0x78) - ez);
		ff8fx::copy<(0x20)>((void *)(e + 0x40), mat);
		U32(e, 0x1C) = xy;
		U32(e, 0x20) = z;
		return 0;
	}

	static inline void rt_ball_xz(int32_t t)
	{
		const int32_t dx = mul32((int32_t)MEM<int16_t>(game_addr(game::unk_2726994)) - (int32_t)MEM<int16_t>(game_addr(game::unk_272699C)), t) / 4096;
		MEM<uint16_t>(game_addr(game::unk_272698C)) = (uint16_t)add32(dx, (int32_t)MEM<uint32_t>(game_addr(game::unk_272699C)));
		const int32_t dz = mul32((int32_t)MEM<int16_t>(game_addr(game::unk_2726998)) - (int32_t)MEM<int16_t>(game_addr(game::unk_27269A0)), t) / 4096;
		MEM<uint16_t>(game_addr(game::unk_2726990)) = (uint16_t)add32(dz, (int32_t)MEM<uint32_t>(game_addr(game::unk_27269A0)));
	}

	uint32_t __cdecl rt_8AAC50(uint32_t a1)
	{
		const uint32_t node = a1;
		rt_8AABA0(node, game_addr(BLK));
		uint16_t m = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_27269B4)) + 0x200);
		MEM<uint16_t>(game_addr(game::unk_27269B4)) = m;
		if ((int16_t)m >= 0x1000)
			MEM<uint16_t>(game_addr(game::unk_27269B4)) = 0x1000;
		uint16_t t = MEM<uint16_t>(game_addr(game::unk_27269D0));
		MEM<uint16_t>(game_addr(game::unk_27269AC)) = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_27269AC)) + 0x200);
		MEM<uint16_t>(game_addr(game::unk_27269B0)) = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_27269B0)) & 0xFFF);
		t = (uint16_t)(t + 0x200);
		MEM<uint16_t>(game_addr(game::unk_27269D0)) = t;
		uint16_t vy;
		if ((int16_t)t >= 0x1000)
		{
			t = 0x1000;
			MEM<uint16_t>(game_addr(game::unk_27269D0)) = t;
			vy = 0xFDA0;
			U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x48) = 1;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		else
			vy = MEM<uint16_t>(game_addr(game::unk_27269A6));
		vy = (uint16_t)(vy + 0x80);
		MEM<uint16_t>(game_addr(game::unk_27269A6)) = vy;
		rt_ball_xz((int32_t)(int16_t)t);
		const uint16_t ground = MEM<uint16_t>(game_addr(game::unk_2726986));
		const uint16_t y = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_272698E)) + vy);
		MEM<uint16_t>(game_addr(game::unk_272698E)) = y;
		if ((int16_t)y >= (int16_t)ground)
		{
			MEM<uint16_t>(game_addr(game::unk_272698E)) = ground;
			if (MEM<uint16_t>(game_addr(game::unk_27269D2)) == 0)
				MEM<uint16_t>(game_addr(game::unk_27269D2)) = 1;
			rt_8A9EC0(node, 1);
		}
		return 0;
	}

	uint32_t __cdecl rt_8AAD90(uint32_t a1)
	{
		const uint32_t node = a1;
		rt_8AABA0(node, game_addr(BLK));
		uint16_t t = MEM<uint16_t>(game_addr(game::unk_27269D0));
		MEM<uint16_t>(game_addr(game::unk_27269AC)) = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_27269AC)) + 0xFE00);
		MEM<uint16_t>(game_addr(game::unk_27269B0)) = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_27269B0)) & 0xFFF);
		t = (uint16_t)(t + 0xFF00);
		MEM<uint16_t>(game_addr(game::unk_27269D0)) = t;
		if (!((int16_t)t > 0))
		{
			t = 0;
			MEM<uint16_t>(game_addr(game::unk_27269D0)) = t;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		rt_ball_xz((int32_t)(int16_t)t);
		const uint16_t vy = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_27269A6)) + 0x80);
		MEM<uint16_t>(game_addr(game::unk_27269A6)) = vy;
		const uint16_t y = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_272698E)) + vy);
		const uint16_t ground = MEM<uint16_t>(game_addr(game::unk_2726986));
		MEM<uint16_t>(game_addr(game::unk_272698E)) = y;
		if ((int16_t)y >= (int16_t)ground)
		{
			MEM<uint16_t>(game_addr(game::unk_272698E)) = ground;
			const uint16_t b = MEM<uint16_t>(game_addr(game::unk_27269D2));
			if (b == 1)
			{
				MEM<uint16_t>(game_addr(game::unk_27269D2)) = 2;
				MEM<uint16_t>(game_addr(game::unk_27269A6)) = 0xFE80;
				rt_8A9EC0(node, 4);
				return 0;
			}
			if (b == 2)
				MEM<uint16_t>(game_addr(game::unk_27269D2)) = 3;
			rt_8A9EC0(node, 1);
		}
		return 0;
	}

	uint32_t __cdecl rt_8AAEC0(uint32_t a1)
	{
		rt_8AABA0(a1, game_addr(BLK));
		const uint16_t m = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_27269B4)) + 0xFD00);
		MEM<uint16_t>(game_addr(game::unk_27269B4)) = m;
		if (!((int16_t)m > 0))
		{
			MEM<uint16_t>(game_addr(game::unk_27269B4)) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl rt_8AAF00(uint32_t a1)
	{
		const uint32_t node = a1;
		U8(node, 0x26) = (uint8_t)(U8(node, 0x26) & 0xF7);
		U16(node, 0xB4) = 8;
		const uint32_t e = ENT(U8(node, 0x2C));
		U16(e, 0) = (uint16_t)(U16(e, 0) & 0xFFFB);
		U16(e, 0x1E) = U16(e, 0x24);
		U32(e, 0x1C) = MEM<uint32_t>(game_addr(game::unk_27269C0));
		U32(e, 0x20) = MEM<uint32_t>(game_addr(game::unk_27269C4));
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl rt_8AAF60(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0xB4) = (uint16_t)(U16(node, 0xB4) - 1);
		if (S16(node, 0xB4) > 0)
			return 0;
		const uint32_t e = ENT(U8(node, 0x2C));
		U32(e, 0xC) = MEM<uint32_t>(game_addr(game::unk_27269C8));
		U32(e, 0x10) = MEM<uint32_t>(game_addr(game::unk_27269CC));
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_019_RaldoThrow_Master_State0_Advance, (void *)a_73A0D0, "019 MAG_019_sub_8A3550" },
		{ game::MAG_019_RaldoThrow_Master_State1_Advance, (void *)a_73A0D0, "019 MAG_019_sub_8A3560" },
		{ game::MAG_019_RaldoThrow_Master_State3_SpawnEmitter, (void *)a_73A170, "019 MAG_019_sub_8A35F0" },
		{ game::MAG_019_RaldoThrow_EmitterTask, (void *)a_73A1A0, "019 MAG_019_sub_8A3620" },
		{ game::MAG_019_RaldoThrow_Director_State0_WaitDirGate1, (void *)a_73B790, "019 sub_8A3830" },
		{ game::MAG_019_DirSyncGate, (void *)a_73B640, "019 sub_8A3850" },
		{ game::MAG_019_DirPhaseGE, (void *)a_73B7E0, "019 sub_8A38A0" },
		{ game::MAG_019_RaldoThrow_Director_State4_Finish, (void *)a_7475A0, "019 sub_8A3900" },
		{ game::MAG_019_RaldoThrow_Director_State5_Empty, (void *)a_73A6D0, "019 nullsub_1777" },
		{ game::MAG_019_St_Empty, (void *)a_73A6D0, "019 nullsub_1778" },
		{ game::MAG_019_SpawnChildTask, (void *)a_73C100, "019 sub_8A39E0" },
		{ game::MAG_019_RaldoThrow_BallActorSystemTask, (void *)a_73A380, "019 sub_8A3A30" },
		{ game::MAG_019_DirectorSetup, (void *)a_73F990, "019 sub_8A3B00" },
		{ game::MAG_019_Director_StartPositions, (void *)a_73FC20, "019 sub_8A3D90" },
		{ game::MAG_019_Director_Mode3Setup, (void *)a_73FE90, "019 sub_8A4000" },
		{ game::MAG_019_Director_TargetPositions, (void *)a_73FFB0, "019 sub_8A4120" },
		{ game::MAG_019_RelocateActorData, (void *)a_740210, "019 sub_8A4380" },
		{ game::MAG_019_EmitG3, (void *)a_73D770, "019 sub_8A5810" },
		{ game::MAG_019_EmitG4, (void *)a_73D9C0, "019 sub_8A5A60" },
		{ game::MAG_019_ActSys_DrawSpritePass, (void *)a_740700, "019 sub_8A6890" },
		{ game::MAG_019_ActSys_DrawActorSprite, (void *)a_7407C0, "019 sub_8A6950" },
		{ game::MAG_019_ActorMotion, (void *)a_740910, "019 sub_8A6AA0" },
		{ game::MAG_019_DampValue, (void *)a_740F70, "019 sub_8A7100" },
		{ game::MAG_019_DampVector, (void *)a_740FC0, "019 sub_8A7150" },
		{ game::MAG_019_Instance_AdvanceTexFrame, (void *)a_741050, "019 sub_8A71E0" },
		{ game::MAG_019_Instance_BuildTransform, (void *)a_741120, "019 sub_8A72B0" },
		{ game::MAG_019_Particle_LifeCountdown, (void *)a_741B60, "019 sub_8A7CF0" },
		{ game::MAG_019_RandRange, (void *)a_742290, "019 au_re__rand_65" },
		{ game::MAG_019_RandRangeWide, (void *)a_7422C0, "019 sub_8A8450" },
		{ game::MAG_019_RandVec3, (void *)a_742300, "019 sub_8A8490" },
		{ game::MAG_019_JitterAngles3, (void *)a_742350, "019 sub_8A84E0" },
		{ game::MAG_019_RandAngleJitter, (void *)a_7423A0, "019 au_re__rand_70_0" },
		{ game::MAG_019_GetWord8Minus1, (void *)a_7424B0, "019 sub_8A8640" },
		{ game::MAG_019_Particle_AttachToOwner, (void *)a_7424C0, "019 sub_8A8650" },
		{ game::MAG_019_ParticleList_Append, (void *)a_742CA0, "019 sub_8A8E30" },
		{ game::MAG_019_ParticleList_Unlink, (void *)a_742CD0, "019 sub_8A8E60" },
		{ game::MAG_019_Particle_Init, (void *)a_742D00, "019 sub_8A8E90" },
		{ game::MAG_019_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "019 sub_8A9040" },
		{ game::MAG_019_Particle_PlaceAtPoint, (void *)a_742FF0, "019 sub_8A9180" },
		{ game::MAG_019_RaldoThrow_BallActorSystem_State2_Empty, (void *)a_73A6D0, "019 nullsub_1779" },
		{ game::InitEffectSequenceFromData_c16, (void *)a_7435E0, "019 InitEffectSequenceFromData_c16" },
		{ game::MAG_019_SpriteStrip_DrawQuads, (void *)a_743720, "019 sub_8AA540" },
		{ game::MAG_019_StepCounterLimit, (void *)a_743C20, "019 sub_8AA9F0" },
		{ game::MAG_019_RaldoThrow_DebrisHigh_State2_Empty, (void *)a_73A6D0, "019 nullsub_1780" },
		{ game::MAG_019_RaldoThrow_DebrisLow_State2_Empty, (void *)a_73A6D0, "019 nullsub_1781" },
		{ game::MAG_019_RaldoThrow_DebrisSpawner_State1_Empty, (void *)a_73A6D0, "019 nullsub_1782" },
		{ game::MAG_019_RaldoThrow_Raldo_State8_Finish, (void *)a_7475A0, "019 sub_8AAFB0" },
		{ game::MAG_019_RaldoThrow_Raldo_State9_Empty, (void *)a_73A6D0, "019 nullsub_1783" },
		{ game::MAG_019_RaldoThrow_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "019 MAG_019_sub_8AB020" },
		{ game::MAG_019_RaldoThrow_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "019 MAG_019_sub_8AB040" },
		{ game::MAG_019_RaldoThrow_Emitter_State4_Empty, (void *)a_73A6D0, "019 nullsub_1784" },
		{ game::MAG_019_RaldoThrow_Master_State4_NextTargetLoop, (void *)a_747500, "019 MAG_019_sub_8AB070" },
		{ game::MAG_019_RaldoThrow_Master_State5_Advance, (void *)a_73A0D0, "019 MAG_019_sub_8AB0B0" },
		{ game::MAG_019_RaldoThrow_Master_State6_WaitQueuesEmpty, (void *)a_747550, "019 MAG_019_sub_8AB0C0" },
		{ game::MAG_019_RaldoThrow_Master_State7_Advance, (void *)a_73A0D0, "019 MAG_019_sub_8AB0D0" },
		{ game::MAG_019_RaldoThrow_Master_State8_Advance, (void *)a_73A0D0, "019 MAG_019_sub_8AB0E0" },
		{ game::MAG_019_RaldoThrow_Master_State9_Finish, (void *)a_7475A0, "019 MAG_019_sub_8AB0F0" },
		{ game::MAG_019_RaldoThrow_Master_State10_Empty, (void *)a_73A6D0, "019 nullsub_1776" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_019_RaldoThrow_MasterTask, (void *)rt_8A33F0, "019 rt_8A33F0" },
		{ game::MAG_019_RaldoThrow_CarveActorPools, (void *)rt_8A3570, "019 rt_8A3570" },
		{ game::MAG_019_RaldoThrow_ClearActorPools, (void *)rt_8A35B0, "019 rt_8A35B0" },
		{ game::MAG_019_RaldoThrow_Emitter_State0_PlaceAndSpawnDirector, (void *)rt_8A36A0, "019 rt_8A36A0" },
		{ game::MAG_019_RaldoThrow_Director_InitFacing, (void *)rt_8A36D0, "019 rt_8A36D0" },
		{ game::MAG_019_RaldoThrow_DirectorTask, (void *)rt_8A3760, "019 rt_8A3760" },
		{ game::MAG_019_RaldoThrow_Director_PhaseBookkeeping, (void *)rt_8A37E0, "019 rt_8A37E0" },
		{ game::MAG_019_RaldoThrow_Director_State1_WaitDirPhase1, (void *)rt_8A3880, "019 rt_8A3880" },
		{ game::MAG_019_RaldoThrow_Director_State2_WaitCutFrame4, (void *)rt_8A38C0, "019 rt_8A38C0" },
		{ game::MAG_019_RaldoThrow_Director_State3_WaitCutFrame2D, (void *)rt_8A38E0, "019 rt_8A38E0" },
		{ game::MAG_019_RaldoThrow_Director_StartThrow, (void *)rt_8A3940, "019 rt_8A3940" },
		{ game::MAG_019_RaldoThrow_BallActorSystem_Start, (void *)rt_8A3A90, "019 rt_8A3A90" },
		{ game::MAG_019_ResetTrailHistory, (void *)rt_8A3CB0, "019 rt_8A3CB0" },
		{ game::MAG_019_FillTrailRows, (void *)rt_8A3F30, "019 rt_8A3F30" },
		{ game::MAG_019_ActSys_Step, (void *)rt_8A45E0, "019 rt_8A45E0" },
		{ game::MAG_019_ActSys_DrawPass, (void *)rt_8A4670, "019 rt_8A4670" },
		{ game::MAG_019_ActSys_DrawPrimActor, (void *)rt_8A46D0, "019 rt_8A46D0" },
		{ game::MAG_019_RaldoThrow_RenderPrimModel, (void *)rt_8A4830, "019 rt_8A4830" },
		{ game::MAG_019_PrimList_FlatTri, (void *)rt_8A4960, "019 rt_8A4960" },
		{ game::MAG_019_PrimList_FlatQuad, (void *)rt_8A4B80, "019 rt_8A4B80" },
		{ game::MAG_019_PrimList_TexTri, (void *)rt_8A4E10, "019 rt_8A4E10" },
		{ game::MAG_019_PrimList_TexQuad, (void *)rt_8A5290, "019 rt_8A5290" },
		{ game::MAG_019_RaldoThrow_PrimList_GouraudTexTriScroll, (void *)rt_8A5D40, "019 rt_8A5D40" },
		{ game::MAG_019_PrimList_GT4, (void *)rt_8A6240, "019 rt_8A6240" },
		{ game::MAG_019_ActSys_UpdatePass, (void *)rt_8A69D0, "019 rt_8A69D0" },
		{ game::MAG_019_ActorStep, (void *)rt_8A6A10, "019 rt_8A6A10" },
		{ game::MAG_019_MorphVertexBlock, (void *)rt_8A7A40, "019 rt_8A7A40" },
		{ game::MAG_019_Instance_EndOfLifeTest, (void *)rt_8A7B50, "019 rt_8A7B50" },
		{ game::MAG_019_Instance_SpawnFollowUp, (void *)rt_8A7BD0, "019 rt_8A7BD0" },
		{ game::MAG_019_AllocParticle, (void *)rt_8A7C00, "019 rt_8A7C00" },
		{ game::MAG_019_ActSys_UpdateEmitter, (void *)rt_8A7CA0, "019 rt_8A7CA0" },
		{ game::MAG_019_ActSys_EmitterSpawnStep, (void *)rt_8A7D20, "019 rt_8A7D20" },
		{ game::MAG_019_ActSys_SpawnParticles, (void *)rt_8A7DA0, "019 rt_8A7DA0" },
		{ game::MAG_019_ActSys_AllocParticleSlot, (void *)rt_8A8580, "019 rt_8A8580" },
		{ game::MAG_019_Actor_InterpolatePosition, (void *)rt_8A87A0, "019 rt_8A87A0" },
		{ game::MAG_019_SpawnSubEffects, (void *)rt_8A9290, "019 rt_8A9290" },
		{ game::MAG_019_Director_SpawnGroupEffects, (void *)rt_8A9320, "019 rt_8A9320" },
		{ game::MAG_019_RaldoThrow_BallActorSystem_State1_StepActorSystem, (void *)rt_8A93B0, "019 rt_8A93B0" },
		{ game::MAG_019_RaldoThrow_RaldoTask, (void *)rt_8A93E0, "019 rt_8A93E0" },
		{ game::MAG_019_RaldoThrow_DrawRaldoBall, (void *)rt_8A94E0, "019 rt_8A94E0" },
		{ game::MAG_019_RaldoThrow_BallRenderer, (void *)rt_8A95F0, "019 rt_8A95F0" },
		{ game::MAG_019_RaldoThrow_Raldo_Setup, (void *)rt_8A9CA0, "019 rt_8A9CA0" },
		{ game::MAG_019_RaldoThrow_InitBallBlock, (void *)rt_8A9D20, "019 rt_8A9D20" },
		{ game::MAG_019_RaldoThrow_Raldo_Sound, (void *)rt_8A9DA0, "019 rt_8A9DA0" },
		{ game::MAG_019_RaldoThrow_Raldo_Take, (void *)rt_8A9DD0, "019 rt_8A9DD0" },
		{ game::MAG_019_RaldoThrow_SpawnDebrisSpawner, (void *)rt_8A9EC0, "019 rt_8A9EC0" },
		{ game::MAG_019_RaldoThrow_DebrisSpawnerTask, (void *)rt_8A9F10, "019 rt_8A9F10" },
		{ game::MAG_019_RaldoThrow_DebrisBurst, (void *)rt_8A9F60, "019 rt_8A9F60" },
		{ game::MAG_019_RaldoThrow_DebrisHighTask, (void *)rt_8AA1D0, "019 rt_8AA1D0" },
		{ game::MAG_019_RaldoThrow_Debris_DrawSprite, (void *)rt_8AA2E0, "019 rt_8AA2E0" },
		{ game::MAG_019_RaldoThrow_DebrisHigh_State0_InitSprite, (void *)rt_8AA9B0, "019 rt_8AA9B0" },
		{ game::MAG_019_RaldoThrow_DebrisHigh_State1_Animate, (void *)rt_8AA9D0, "019 rt_8AA9D0" },
		{ game::MAG_019_RaldoThrow_DebrisLowTask, (void *)rt_8AAA30, "019 rt_8AAA30" },
		{ game::MAG_019_RaldoThrow_DebrisLow_State0_InitSprite, (void *)rt_8AAB40, "019 rt_8AAB40" },
		{ game::MAG_019_RaldoThrow_DebrisLow_State1_Animate, (void *)rt_8AAB60, "019 rt_8AAB60" },
		{ game::MAG_019_RaldoThrow_Raldo_UpdatePivot, (void *)rt_8AABA0, "019 rt_8AABA0" },
		{ game::MAG_019_RaldoThrow_Raldo_Throw, (void *)rt_8AAC50, "019 rt_8AAC50" },
		{ game::MAG_019_RaldoThrow_Raldo_Rebound, (void *)rt_8AAD90, "019 rt_8AAD90" },
		{ game::MAG_019_RaldoThrow_Raldo_Unmorph, (void *)rt_8AAEC0, "019 rt_8AAEC0" },
		{ game::MAG_019_RaldoThrow_Raldo_GiveBack, (void *)rt_8AAF00, "019 rt_8AAF00" },
		{ game::MAG_019_RaldoThrow_Raldo_RestoreRotation, (void *)rt_8AAF60, "019 rt_8AAF60" },
		{ game::MAG_019_RaldoThrow_Emitter_ApplyDamage, (void *)rt_8AAFE0, "019 rt_8AAFE0" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag019_raldo_throw()
	{
		act::register_module(19);
		for (const act::raldo::ModPort *p = act::raldo::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(19, p->addr, p->port, p->name);
		for (const act::raldo::ModPort *p = act::raldo::PORTS; p->addr; p++)
			act::register_module_port(19, p->addr, p->port, p->name);
	}
}

