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
namespace u083
{
	static const Mod MOD_083 = { "absorbed83", 83, game::MAG_083_ABSORBED_INTO_TIME_FL, game::MAG_082_ULTIMECIA_BLOW_AWAY_MAGIC_FL,
		{ {}, {}, game::MAG_083_DirectorPtr, {}, {}, game::unk_25CA07C, game::unk_25C9F78, game::unk_25CF878, game::unk_25CFA38, game::unk_25CFA44, game::unk_25CFA4C, {}, game::unk_25C9F70, {}, {}, {}, game::unk_25CA080, {}, game::unk_25CA850, {}, {}, {}, {}, {}, game::MAG_083_ActorSystemQueue, {}, game::MAG_083_EmitterQueue, {}, {}, {}, {}, {}, game::unk_25CFA28, game::unk_25CFA2C, {}, game::unk_25CFA3A, {}, game::MAG_083_ScratchSP, {}, game::MAG_083_ActorStatePtr },
		{ game::MAG_083_ClearWorkBuffers, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_083_ActorStep, game::MAG_083_ActSys_UpdateEmitter, game::MAG_083_Particle_LifeCountdown, game::MAG_083_ActSys_EmitterSpawnStep, game::MAG_083_ActSys_SpawnParticles, game::MAG_083_RandVec3, game::MAG_083_JitterAngles3, game::MAG_083_ActSys_AllocParticleSlot, game::MAG_083_GetWord8Minus1, game::MAG_083_Particle_Init, game::MAG_083_SpawnSubEffects, {}, {}, {}, {}, {}, {}, game::MAG_083_EmitterTask, game::MAG_083_Emitter_State0_PlaceAndStartDirector, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_083_DirSyncGate, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_083_ResetTrailHistory, game::MAG_083_Director_StartPositions, game::MAG_083_FillTrailRows, game::MAG_083_Director_Mode3Setup, game::MAG_083_Director_TargetPositions, game::MAG_083_RelocateActorData, {}, game::MAG_083_ActorMotion, game::MAG_083_DampValue, game::MAG_083_DampVector, game::MAG_083_Instance_AdvanceTexFrame, game::MAG_083_Instance_BuildTransform, game::MAG_083_MorphVertexBlock, game::MAG_083_Instance_EndOfLifeTest, game::MAG_083_Instance_SpawnFollowUp, game::MAG_083_AllocParticle, game::MAG_083_RandRange, game::MAG_083_RandRangeWide, game::MAG_083_RandAngleJitter, game::MAG_083_Particle_AttachToOwner, game::MAG_083_Actor_InterpolatePosition, game::MAG_083_ParticleList_Append, game::MAG_083_ParticleList_Unlink, game::MAG_083_Particle_PlaceAtAnchorSet, game::MAG_083_Particle_PlaceAtPoint, {}, {}, game::MAG_083_Emitter_ApplyDamageOnCue, game::MAG_083_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_083_Emitter_State3_FinishReleaseRoot, game::MAG_083_Emitter_State4_Empty, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id DIR_PTR = game::MAG_083_DirectorPtr;
	static constexpr game::Id PACKET_CURSOR = game::MAG_083_SpritePacketCursor;
	static constexpr game::Id SCRATCH_SP = game::MAG_083_ScratchSP;
	static constexpr game::Id ACTOR_STATE = game::MAG_083_ActorStatePtr;
	static constexpr game::Id STAGE_LIGHT = game::unk_1D98992;
	static constexpr game::Id Q_EMITTER = game::MAG_083_EmitterQueue;
	static constexpr game::Id Q_DIRECTOR = game::MAG_083_DirectorQueue;
	static constexpr game::Id Q_CREATURE = game::MAG_083_CreatureQueue;
	static constexpr game::Id Q_ACTORS = game::MAG_083_ActorSystemQueue;
	static constexpr game::Id Q_LIGHT = game::MAG_083_StageLightQueue;
	static constexpr game::Id Q_ABSORB = game::MAG_083_AbsorbQueue;
	static constexpr game::Id Q_LAST = game::MAG_083_UnusedQueue;
	static inline uint32_t ENT(uint32_t slot) { return game_addr(game::BattleEntitySlotData) + slot * 0x9C; }

	namespace gx
	{
		static inline uint32_t GTE_SetBackgroundVector(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetBackgroundVector))(a1, a2, a3); }
		static inline uint32_t GTE_SetLightMatrix(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_SetLightMatrix))(a1); }
		static inline uint32_t GTE_LoadV012Dwords(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadV012Dwords))(a1); }
		static inline uint32_t GTE_StoreSXY012_PolyFT3(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreSXY012_PolyFT3))(a1); }
		static inline uint32_t GTE_StoreSXY012_PolyFT3_2(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreSXY012_PolyFT3_2))(a1); }
		static inline uint32_t GTE_MVMVA_LightV0_Bk() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
		static inline uint32_t sub_45E0B0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadIR123_u16))(a1); }
		static inline uint32_t sub_45E9D0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPF))(); }
		static inline uint32_t sub_45EBF0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPL))(); }
		static inline uint32_t sub_571B70() { return fn<uint32_t (__cdecl *)()>(game_addr(game::sub_571B70))(); }
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
	static inline void next_state(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }

	static inline void absorb_step(uint32_t s, uint32_t st, bool quad)
	{
		const uint16_t phase = U16(st, 6);
		if (phase == 0)
		{
			const uint16_t x0 = U16(s, 0x54), y0 = U16(s, 0x56), z0 = U16(s, 0x58);
			U16(st, 0xA) = (uint16_t)(U16(s, 0x5C) - x0);
			U16(st, 0x10) = (uint16_t)(U16(s, 0x5E) - y0);
			U16(st, 0) = y0;
			U16(st, 0x16) = (uint16_t)(U16(s, 0x60) - z0);
			U16(st, 0xC) = (uint16_t)(U16(s, 0x64) - x0);
			U16(st, 8) = (uint16_t)(U16(st, 8) + 1);
			U16(st, 0x12) = (uint16_t)(U16(s, 0x66) - y0);
			U16(st, 0x18) = (uint16_t)(U16(s, 0x68) - z0);
			const int32_t x = S16(s, 0x54), z = S16(s, 0x58);
			U16(st, 4) = (uint16_t)(x::CartesianToGameAngle((uint32_t)x, (uint32_t)z) & 0xFFF);
			const uint32_t r2 = (uint32_t)(x * x) + (uint32_t)(z * z);
			U32(s, 0x88) = r2;
			const uint32_t r = x::Sqrt(r2);
			U16(st, 6) = (uint16_t)(U16(st, 6) + 1);
			U16(st, 2) = (uint16_t)r;
			U16(st, 0x1C) = 0x1000;
			U16(st, 8) = 0;
			return;
		}
		if (phase == 1)
		{
			if (S16(st, 2) >= S16(s, 0x90))
			{
				U16(st, 8) = 0;
				U16(st, 6) = 2;
			}
			return;
		}
		U16(st, 8) = (uint16_t)(U16(st, 8) + 1);
		U16(st, 0x1E) = (uint16_t)(U16(st, 0x1E) + 0x10);
		const int16_t cnt = S16(st, 8);
		if (S16(st, 0x1E) > 0x100)
			U16(st, 0x1E) = 0x100;
		const uint32_t ang = (uint32_t)(uint16_t)(U16(st, 4) + U16(st, 0x1E)) & 0xFFF;
		U16(st, 4) = (uint16_t)ang;
		if (cnt >= 0xF)
		{
			U16(st, 0x1C) = (uint16_t)(U16(st, 0x1C) - 0x20);
			if (S16(st, 0x1C) < 0x100)
				U16(st, 0x1C) = 0x100;
			const int32_t sc = S16(st, 0x1C);
			U16(s, 0x8E) = (uint16_t)sc;
			static const int32_t OFF[9] = { 0xA, 0x10, 0x16, 0xC, 0x12, 0x18, 0xE, 0x14, 0x1A };
			for (int k = 0; k < (quad ? 9 : 6); k++)
				S16(st, OFF[k]) = (int16_t)((S16(st, OFF[k]) * sc) / 4096);
		}
		const int32_t r = S16(st, 2);
		S32(s, 0x7C) = (int32_t)x::computeSin(ang) * r / 4096;
		const int32_t cs = (int32_t)x::computeCosine((uint32_t)(int32_t)S16(st, 4));
		U16(st, 0) = (uint16_t)(U16(st, 0) - 0x30);
		S32(s, 0x84) = cs * r / 4096;
		const int32_t y = S16(st, 0);
		S32(s, 0x80) = y;
		const uint16_t px = U16(s, 0x7C), py = (uint16_t)y, pz = U16(s, 0x84);
		U16(s, 0x54) = px;
		U16(s, 0x56) = py;
		U16(s, 0x58) = pz;
		U16(s, 0x5C) = (uint16_t)(U16(st, 0xA) + px);
		U16(s, 0x5E) = (uint16_t)(U16(st, 0x10) + py);
		U16(s, 0x60) = (uint16_t)(U16(st, 0x16) + pz);
		U16(s, 0x64) = (uint16_t)(U16(st, 0xC) + px);
		U16(s, 0x66) = (uint16_t)(U16(st, 0x12) + py);
		U16(s, 0x68) = (uint16_t)(U16(st, 0x18) + pz);
		if (quad)
		{
			U16(s, 0x6C) = (uint16_t)(U16(st, 0xE) + px);
			U16(s, 0x6E) = (uint16_t)(U16(st, 0x14) + py);
			U16(s, 0x70) = (uint16_t)(U16(st, 0x1A) + pz);
		}
	}

	uint32_t __cdecl ud_797840(uint32_t a1);
	uint32_t __cdecl ud_797B10(uint32_t a1);
	uint32_t __cdecl ud_797B40(uint32_t a1);
	uint32_t __cdecl ud_797BF0(uint32_t a1);
	uint32_t __cdecl ud_797C70(uint32_t a1);
	uint32_t __cdecl ud_797CC0(uint32_t a1);
	uint32_t __cdecl ud_797D30(uint32_t a1);
	uint32_t __cdecl ud_797D80(uint32_t a1);
	uint32_t __cdecl ud_797E00(uint32_t a1);
	uint32_t __cdecl ud_797F30(uint32_t a1);
	uint32_t __cdecl ud_798110(uint32_t a1);
	uint32_t __cdecl ud_798390(void);
	uint32_t __cdecl ud_798A40(uint32_t a1);
	uint32_t __cdecl ud_798AD0(void);
	uint32_t __cdecl ud_798B30(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_798CB0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_798DE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_799000(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_799290(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_799710(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_79A1C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_79A6C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_79AD10(void);
	uint32_t __cdecl ud_79ADD0(uint32_t a1);
	uint32_t __cdecl ud_79BEC0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_79CC20(uint32_t a1);
	uint32_t __cdecl ud_79D830(uint32_t a1);
	uint32_t __cdecl ud_79D910(uint32_t a1);
	uint32_t __cdecl ud_79D950(uint32_t a1);
	uint32_t __cdecl ud_79D960(uint32_t a1);
	uint32_t __cdecl ud_79D9B0(uint32_t a1);
	uint32_t __cdecl ud_79DA20(uint32_t a1);
	uint32_t __cdecl ud_79DB00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_79E530(uint32_t a1);
	uint32_t __cdecl ud_79E590(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_79E5F0(void);
	uint32_t __cdecl ud_79E610(uint32_t a1);
	uint32_t __cdecl ud_79E690(uint32_t a1);
	uint32_t __cdecl ud_79E960(uint32_t a1);
	uint32_t __cdecl ud_79E9E0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_79EB00(uint32_t a1);
	uint32_t __cdecl ud_79EB70(uint32_t a1);
	uint32_t __cdecl ud_79EBE0(uint32_t a1);
	uint32_t __cdecl ud_79EC20(uint32_t a1);
	uint32_t __cdecl ud_79EC40(uint32_t a1);
	uint32_t __cdecl ud_79ECB0(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace u083
{
	uint32_t __cdecl ud_797840(uint32_t a1)
	{
		g_mod = &MOD_083;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_25CA080)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_083_Master_State0_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_083_Master_State1_Advance);
		states[2] = game_addr(game::MAG_083_Master_State2_CarveWorkBuffers);
		states[3] = game_addr(game::MAG_083_Master_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_083_Master_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_083_Master_State5_Advance);
		states[6] = game_addr(game::MAG_083_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_083_Master_State7_RequestBattleSync);
		states[8] = game_addr(game::MAG_083_Master_State8_WaitBattleSyncDone);
		states[9] = game_addr(game::MAG_083_Master_State9_Finish);
		states[10] = game_addr(game::MAG_083_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_25CF3EC));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_25CED2C));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_25CA07C)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_25CF3E8));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_25CED28));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_25CA07C)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_25CFA3A)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25CFA28)) = 0;
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_EMITTER)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_DIRECTOR)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_CREATURE)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_ACTORS)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_LIGHT)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_ABSORB)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_LAST)));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_797B10(uint32_t a1)
	{
		ud_797B40(a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_083_DirectorQueue), game_addr(game::MAG_083_Director), 0x48, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_797B40(uint32_t a1)
	{
		const uint32_t ent = ENT(U8(a1, 0x2D));
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(DIR_PTR)), 0x54);
		x::GetEffectSpawnPosition(ent, 0xF1, 0, MEM<uint32_t>(game_addr(DIR_PTR)) + 8);
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		U16(d, 0xA) = 0;
		U16(d, 0) = 0;
		U16(d, 2) = 0;
		U16(d, 4) = 0;
		const int16_t dx = (int16_t)(U16(d, 8) - U16(d, 0));
		const uint32_t ang = x::CartesianToGameAngle((uint32_t)(int32_t)dx, (uint32_t)(int32_t)S16(d, 0xC));
		U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x4A) = (uint16_t)((ang - 0x800) & 0xFFF);
		x::MAG_022_sub_8DD770(game_addr(game::unk_1D99AB0));
		x::MAG_022_sub_8DD8A0(game_addr(game::unk_1D99AB0), (uint32_t)(int32_t)S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x4A));
		return 0;
	}

	uint32_t __cdecl ud_797BF0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[7];
		states[0] = game_addr(game::MAG_083_UploadCherubTIM);
		states[1] = game_addr(game::MAG_083_Director_State1_WaitDirGate1);
		states[2] = game_addr(game::MAG_083_Director_SoundAtCut1);
		states[3] = game_addr(game::MAG_083_Director_DamageCueAtFrame6E);
		states[4] = game_addr(game::MAG_083_Director_State4_WaitDirGate2);
		states[5] = game_addr(game::MAG_083_Director_State5_Finish);
		states[6] = game_addr(game::MAG_083_Director_State6_Empty);
		ud_797C70(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_797C70(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			ud_797E00(a1);
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

	uint32_t __cdecl ud_797CC0(uint32_t a1)
	{
		x::Battle_QueueTIMUpload_GetEOF(game_addr(game::MAG083_Cherub_BodyTIM));
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl ud_797D30(uint32_t a1)
	{
		uint32_t r = a_73B7E0(1);
		if (r == 0) return 0;
		x::BdPlaySE(game_addr(game::unk_1578440), 0, 0x80);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl ud_797D80(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		if (S16(d, 0x46) < 0x6E)
			return d;
		U16(d, 0x48) = 1;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl ud_797E00(uint32_t a1)
	{
		const int32_t cut = S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x40);
		if (cut - 1 != 0)
			return (uint32_t)(cut - 1);
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_CREATURE), game_addr(game::MAG_083_CherubTask), 0x140, a1);
		x::Effect_BindModelContainerSetAnim(t, game_addr(game::MAG083_Cherub_ModelContainer), 3);
		a_73C100(a1, game_addr(game::MAG_083_ActorSystemTask), game_addr(game::MAG_083_ActorSequence), 0, 0xF, 0);
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_ABSORB), game_addr(game::MAG_083_AbsorbTargetTask), 0xC8, a1);
		return x::Effect_AddTaskAndInitFromCtx(game_addr(Q_LIGHT), game_addr(game::MAG_083_StageLightTask), 0xB0, a1);
	}

	uint32_t __cdecl ud_797F30(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			a_73F990(a1);
			ud_798A40(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_798110(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_083_ActorStatePtr));
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

	uint32_t __cdecl ud_798390(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_083_ActorStatePtr));
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

	uint32_t __cdecl ud_798A40(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::MAG_083_ActorStatePtr), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			a_743190();
			a_736C00();
			ud_79AD10();
			ud_798AD0();
			sys = U32(game_addr(game::MAG_083_ActorStatePtr), 0);
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
		U16(game_addr(game::unk_25CFA3A), 0) = (uint16_t)(U16(game_addr(game::unk_25CFA3A), 0) + c14);
		U16(game_addr(game::unk_25CFA28), 0) = (uint16_t)(U16(game_addr(game::unk_25CFA28), 0) + c16);
		return done;
	}

	uint32_t __cdecl ud_798AD0(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_083_ActorStatePtr), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::MAG_083_ActorStatePtr), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					ud_798B30(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl ud_798B30(uint32_t a1, uint32_t a2)
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
		const uint8_t k = U8(a1, 0x1D6);
		if (k == 4 || k == 0xA)
			U32(h, 0x10) = 0x100;
		else
			U32(h, 0x10) = 0;
		const int8_t copies = S8(a1, 0x1D8);
		U16(h, 0x1E) = 0;
		U16(h, 0x1C) = 0;
		U16(h, 0x22) = 0x100;
		U16(h, 0x20) = 0x100;
		U16(h, 0x18) = 0;
		U16(h, 0x1A) = 0;
		U16(h, 0x26) = 0;
		U16(h, 0x24) = 0;
		U16(h, 0x2A) = 0x100;
		U16(h, 0x28) = 0x100;
		if (copies == 1)
		{
			x::GTE_SetRotMatrix(a1 + 0xAC);
			x::GTE_SetTransVector(a1 + 0xAC);
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = ud_798CB0(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
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
				MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = ud_798CB0(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
			}
		}
		const uint32_t top = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 0x68;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = top;
		return top;
	}

	uint32_t __cdecl ud_798CB0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		if (!(U32(ctx, 0x14) & 0x2000))
			U32(ctx, 4) = U32(ctx, 0) + 8;
		const uint32_t model = U32(ctx, 0);
		const uint32_t fb = U8(ctx, 0xA);
		const uint32_t lists = U32(model, 0) + model;
		const uint32_t fg = U8(ctx, 9);
		U32(ctx, 0x2C) = lists;
		const uint32_t fr = U8(ctx, 8);
		x::someCameraWork_45DD60(fr, fg, fb);

		uint32_t cur = a4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = ud_798DE0(ctx, a2, a3, a4);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = ud_799000(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = ud_799290(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = ud_799710(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = a_73D770(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = a_73D9C0(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = ud_79A1C0(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			return ud_79A6C0(ctx, a2, a3, cur);
		U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		return cur;
	}

	uint32_t __cdecl ud_798DE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_799000(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_799290(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_799710(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_79A1C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_79A6C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_79AD10(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::MAG_083_ActorStatePtr)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::MAG_083_ActorStatePtr));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						ud_79ADD0(act);
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
							ud_79ADD0(act);
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

	uint32_t __cdecl ud_79ADD0(uint32_t a1)
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

	uint32_t __cdecl ud_79BEC0(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::MAG_083_ActorStatePtr), 0);
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

	uint32_t __cdecl ud_79CC20(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::MAG_083_ActorStatePtr), 0);
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
				uint32_t g = U32(game_addr(game::MAG_083_ActorStatePtr), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::MAG_083_ActorStatePtr), 0);
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

	uint32_t __cdecl ud_79D830(uint32_t a1)
	{
		if (ud_798A40(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_79D910(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0x200);
		if (S16(a1, 0x1C) >= 0x800)
		{
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0x1C) = 0x800;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		uint32_t p = game_addr(STAGE_LIGHT);
		for (int i = 4; i != 0; i--, p += 0x2C)
			MEM<uint16_t>(p) = level;
		return p;
	}

	uint32_t __cdecl ud_79D950(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0x50)
			U8(a1, 0x29)++;
		return 0;
	}

	uint32_t __cdecl ud_79D960(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0xFF00);
		if (S16(a1, 0x1C) <= 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int i = 4; i != 0; i--, p += 0x2C)
			MEM<uint16_t>(p) = level;
		return 0;
	}

	uint32_t __cdecl ud_79D9B0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_083_AbsorbTarget_State0_Start);
		states[1] = game_addr(game::MAG_083_AbsorbTarget_State1_Countdown);
		states[2] = game_addr(game::MAG_083_AbsorbTarget_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		if ((U8(node, 0x26) & 8) != 0)
		{
			ud_79DA20(node + 0x6C);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_79DA20(uint32_t a1)
	{
		const uint32_t m = a1;
		const uint32_t ent = U32(m, 0x48);
		ff8fx::copy<(8 * 4)>((void *)m, (const void *)(ent + 0x40));
		x::BattleModel_BuildBoneMatricesFromPose(ent + 0x60);
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(m);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(m + 0x20);
		x::GTE_LoadIRFromMatrixColumn(m + 2);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(m + 0x22);
		x::GTE_LoadIRFromMatrixColumn(m + 4);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(m + 0x24);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(m + 0x14);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(m + 0x34);
		ud_79DB00(U32(ent, 0x64), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, m);
		const uint32_t weapon = U32(ent, 0x78);
		if (weapon != 0)
			ud_79DB00(U32(weapon, 4), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, m);
		return x::BattleModel_BuildBoneMatricesFromPose(ent + 0x60);
	}

	uint32_t __cdecl ud_79DB00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		(void)a3;
		const uint32_t m = a4;
		uint32_t cursor = MEM<uint32_t>(U32(m, 0x4C));
		const uint32_t verts = U32(U32(m, 0x44), 4);
		const uint32_t ent = U32(m, 0x48);
		const uint32_t table = U32(a1, 4);
		const uint32_t bones = U32(a1, 0) + 0x10;
		const int32_t count = S32(table, 0);
		const uint32_t s = x::Field_Alloc(0x94);
		uint32_t offs = table + 4;
		const uint32_t col = U32(ent, 0x28) & 0xFFFFFF;
		const uint32_t mask = U32(ent, 0x7C);
		uint32_t st = MEM<uint32_t>(game_addr(game::MAG_083_PolygonStatesPtr));
		U32(s, 0x48) = col | 0x24000000;
		U16(s, 0x90) = U16(m, 0x54);
		U32(s, 0x50) = mask;
		U32(s, 0x4C) = col | 0x2C000000;
		U16(s, 0x8C) = 0;
		x::GTE_SetRotMatrix(m + 0x20);
		x::GTE_SetTransVector(m + 0x20);
		for (int32_t i = 0; i < count; i++)
		{
			uint32_t dst = verts;
			uint32_t p = U32(a1, 4) + U32(offs, 0);
			offs += 4;
			if (((U32(s, 0x50) >> (i & 31)) & 1) == 0)
				continue;
			const int32_t groups = S16(p, 0);
			p += 2;
			for (int32_t g = groups; g > 0; g--)
			{
				const int32_t bone = S16(p, 0);
				p += 2;
				const uint32_t bm = (uint32_t)(bone * 0x30) + bones + 0x10;
				gx::GTE_SetLightMatrix(bm);
				gx::GTE_SetBackgroundVector(U32(bm, 0x14), U32(bm, 0x18), U32(bm, 0x1C));
				const int32_t nv = S16(p, 0);
				p += 2;
				for (int32_t k = nv; k > 0; k--)
				{
					U16(s, 0x28) = U16(p, 0);
					U16(s, 0x2A) = U16(p, 2);
					U16(s, 0x2C) = U16(p, 4);
					p += 6;
					x::GTE_LoadV0(s + 0x28);
					gx::GTE_MVMVA_LightV0_Bk();
					x::GTE_StoreIR123(s + 0x28);
					U32(dst, 0) = U32(s, 0x28);
					U32(dst, 4) = U32(s, 0x2C);
					dst += 0x10;
				}
			}
			p = (p + 3) & ~3u;
			const int32_t tris = S16(p, 0);
			const int32_t quads = S16(p, 2);
			uint32_t poly = p + 0xC;
			for (int32_t t = tris; t > 0; t--, poly += 0x10)
			{
				const uint32_t i0 = U16(poly, 0) & 0xFFF;
				const uint32_t i1 = U16(poly, 2) & 0xFFF;
				const uint32_t i2 = U16(poly, 4) & 0xFFF;
				U32(s, 0x38) = i0;
				U32(s, 0x3C) = i1;
				U32(s, 0x40) = i2;
				U32(s, 0x54) = U32(verts + i0 * 16, 0);
				U32(s, 0x58) = U32(verts + i0 * 16, 4);
				U32(s, 0x5C) = U32(verts + i1 * 16, 0);
				U16(s, 0x8C) = (uint16_t)(U16(s, 0x8C) + 1);
				U32(s, 0x60) = U32(verts + i1 * 16, 4);
				st += 0x20;
				U32(s, 0x64) = U32(verts + i2 * 16, 0);
				U32(s, 0x68) = U32(verts + i2 * 16, 4);
				absorb_step(s, st, false);
				gx::GTE_LoadV012Dwords(s + 0x54);
				x::GTE_RTPT();
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(s + 0x74);
				if (S32(s, 0x74) <= 0)
					continue;
				x::GTE_AVSZ3();
				x::GTE_ReadOTZ(s + 0x78);
				const uint32_t z = U32(s, 0x78) >> 2;
				U32(s, 0x78) = z;
				gx::GTE_StoreSXY012_PolyFT3(cursor);
				U32(cursor, 0) = 0x7000000;
				U32(cursor, 0xC) = U32(poly, 8);
				U32(cursor, 0x14) = U32(poly, 0xC);
				U16(cursor, 0x1C) = U16(poly, 6);
				U32(cursor, 4) = U32(s, 0x48);
				if ((U8(poly, 0xF) & 2) != 0)
					U8(cursor, 7) |= 2;
				if (S16(st, 6) >= 2)
					U8(cursor, 7) |= 2;
				x::SSIGPU_InsertPrimAutoDepth(a2 + z * 4, cursor);
				cursor += 0x20;
			}
			uint32_t pq = poly + 4;
			for (int32_t q = quads; q > 0; q--, pq += 0x14)
			{
				const uint32_t i0 = U16(pq, -4) & 0xFFF;
				const uint32_t i1 = U16(pq, -2) & 0xFFF;
				const uint32_t i2 = U16(pq, 0) & 0xFFF;
				const uint32_t i3 = U16(pq, 2) & 0xFFF;
				U32(s, 0x38) = i0;
				U32(s, 0x3C) = i1;
				U32(s, 0x54) = U32(verts + i0 * 16, 0);
				U32(s, 0x40) = i2;
				U32(s, 0x58) = U32(verts + i0 * 16, 4);
				U32(s, 0x5C) = U32(verts + i1 * 16, 0);
				U16(s, 0x8C) = (uint16_t)(U16(s, 0x8C) + 1);
				U32(s, 0x64) = U32(verts + i2 * 16, 0);
				U32(s, 0x60) = U32(verts + i1 * 16, 4);
				const uint32_t v2hi = U32(verts + i2 * 16, 4);
				st += 0x20;
				U32(s, 0x44) = i3;
				U32(s, 0x6C) = U32(verts + i3 * 16, 0);
				U32(s, 0x70) = U32(verts + i3 * 16, 4);
				U32(s, 0x68) = v2hi;
				absorb_step(s, st, true);
				gx::GTE_LoadV012Dwords(s + 0x54);
				x::GTE_RTPT();
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(s + 0x74);
				if (S32(s, 0x74) <= 0)
					continue;
				gx::GTE_StoreSXY012_PolyFT3_2(cursor);
				x::GTE_LoadV0(s + 0x6C);
				x::GTE_RTPS();
				x::GTE_ReadSXY2(cursor + 0x20);
				x::GTE_AVSZ4();
				x::GTE_ReadOTZ(s + 0x78);
				const uint32_t z = U32(s, 0x78) >> 2;
				U32(s, 0x78) = z;
				U32(cursor, 0) = 0x9000000;
				U32(cursor, 0xC) = U32(pq, 4);
				U32(cursor, 0x14) = U32(pq, 8);
				U16(cursor, 0x1C) = U16(pq, 0xC);
				U16(cursor, 0x24) = U16(pq, 0xE);
				U32(cursor, 4) = U32(s, 0x4C);
				if ((U8(pq, 0xB) & 2) != 0)
					U8(cursor, 7) |= 2;
				if (S16(st, 6) >= 2)
					U8(cursor, 7) |= 2;
				x::SSIGPU_InsertPrimAutoDepth(a2 + z * 4, cursor);
				cursor += 0x28;
			}
		}
		MEM<uint32_t>(U32(m, 0x4C)) = cursor;
		return x::Field_Free(0x94);
	}

	uint32_t __cdecl ud_79E530(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t m = node + 0x6C;
		const uint32_t ent = ENT(U8(node, 0x2D));
		if (S16(node, 0x24) < 0x37)
			return 0;
		ud_79E590(m, game_addr(game::MAG_083_AbsorbVertexBuffer), game_addr(game::unk_25CFA58), ent);
		ud_79E5F0();
		U8(node, 0x26) = (uint8_t)(U8(node, 0x26) | 8);
		U8(ent, 0) = (uint8_t)(U8(ent, 0) | 4);
		U16(m, 0x54) = 0x300;
		U16(node, 0xC4) = 0x5A;
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_79E590(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		x::MAG_007_sub_8DCC00(a1, 0x58);
		U32(a1, 0x48) = a4;
		U32(a1, 0x40) = a2;
		U32(a1, 0x4C) = game_addr(PACKET_CURSOR);
		U32(a1, 0x50) = game_addr(PACKET_CURSOR);
		U32(a3, 4) = a2;
		U32(a1, 0x44) = a3;
		U16(a3, 0x14) = 0;
		U16(a3, 0x16) = 0;
		U16(a3, 0x18) = 0x140;
		U16(a3, 0x1A) = 0;
		U8(a3, 0x1C) = 0x80;
		U8(a3, 0x1D) = 0x80;
		U8(a3, 0x1E) = 0x80;
		U32(a3, 0x20) = 0xFFFFFFFF;
		U16(a3, 0x24) = 0;
		return a3;
	}

	uint32_t __cdecl ud_79E5F0(void)
	{
		const uint32_t b = gx::sub_571B70();
		MEM<uint32_t>(game_addr(game::MAG_083_PolygonStatesPtr)) = b;
		return x::MAG_007_sub_8DCC00(b, 0x7D00);
	}

	uint32_t __cdecl ud_79E610(uint32_t a1)
	{
		const uint32_t ent = ENT(U8(a1, 0x2D));
		U16(a1, 0xC0) = (uint16_t)(U16(a1, 0xC0) - 0x10);
		if (S16(a1, 0xC0) < 0)
			U16(a1, 0xC0) = 0;
		U16(a1, 0xC4) = (uint16_t)(U16(a1, 0xC4) - 1);
		const int16_t c = S16(a1, 0xC4);
		if (c == 0x3C)
		{
			U8(ent, 0) = (uint8_t)(U8(ent, 0) | 0x20);
			return 0;
		}
		if (c > 0)
			return 0;
		U16(ent, 0) = (uint16_t)(U16(ent, 0) & 0xFFFB);
		U16(a1, 0x26) = (uint16_t)((U16(a1, 0x26) & 0xFFF7) | 1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_79E690(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[6];
		states[0] = game_addr(game::MAG_083_Cherub_State0_Appear);
		states[1] = game_addr(game::MAG_083_Cherub_State1_FadeIn);
		states[2] = game_addr(game::MAG_083_Cherub_State2_Anim1);
		states[3] = game_addr(game::MAG_083_Cherub_State3_Anim2);
		states[4] = game_addr(game::MAG_083_Cherub_State4_FadeOut);
		states[5] = game_addr(game::MAG_083_Cherub_State5_Empty);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x50) = (uint16_t)(U16(node, 0x50) + U16(node, 0x128));
		U16(node, 0x4C) = (uint16_t)(U16(node, 0x4C) + U16(node, 0x124));
		U16(node, 0x4E) = (uint16_t)(U16(node, 0x4E) + U16(node, 0x126));
		if ((U8(node, 0x26) & 4) == 0)
		{
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = a_746C10(node, game_addr(game::unk_25CF3F8), MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		}
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::unk_1D99AB0), (const void *)(node + 0x70));
		S32(node, 0x88) = S16(node, 0x4E);
		S32(node, 0x84) = S16(node, 0x4C);
		const uint8_t status = U8(node, 0x26);
		S32(node, 0x8C) = S16(node, 0x50);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_79E960(uint32_t a1)
	{
		x::au_re_Battle_ReadAnimation_7(a1, 0);
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		const uint32_t xz = U32(d, 8);
		const uint32_t zz = U32(d, 0xC);
		U32(a1, 0x4C) = xz;
		U16(a1, 0x30) = (uint16_t)(U16(a1, 0x30) | 0x20);
		U32(a1, 0x50) = zz;
		U16(a1, 0x4E) = 0xFA80;
		U16(a1, 0x126) = 0x20;
		U32(a1, 0x11C) = 0x1000;
		U32(a1, 0x118) = 0x1000;
		U32(a1, 0x114) = 0x1000;
		U16(a1, 0x13A) = 0x1000;
		U16(a1, 0x13C) = 0x1000;
		ud_79E9E0(a1, 3);
		ud_79EB00(a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_79E9E0(uint32_t a1, uint32_t a2)
	{
		const uint32_t node = a1;
		a_733950(node + 0x30, U16(node, 0x13A), a2);
		return 0;
	}

	uint32_t __cdecl ud_79EB00(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t amb = MEM<uint32_t>(game_addr(game::CURRENT_BS_SHADOW_PALETTE));
		const int32_t level = S16(node, 0x13C);
		const uint8_t c0 = (uint8_t)(amb & 0xFF);
		const uint8_t c1 = (uint8_t)((amb >> 8) & 0xFF);
		const int32_t d0 = mul32((int32_t)c0, level) / 4096;
		U8(node, 0x5C) = (uint8_t)(c0 - (uint8_t)d0);
		const int32_t d1 = mul32((int32_t)c1, level) / 4096;
		U8(node, 0x5D) = (uint8_t)(c1 - (uint8_t)d1);
		const uint8_t c2 = MEM<uint8_t>(game_addr(game::unk_B8B9AA));
		const int32_t d2 = mul32((int32_t)c2, level) / 4096;
		U8(node, 0x5E) = (uint8_t)(c2 - (uint8_t)d2);
		return 0;
	}

	uint32_t __cdecl ud_79EB70(uint32_t a1)
	{
		U16(a1, 0x13A) = (uint16_t)(U16(a1, 0x13A) + 0xFF00);
		if (S16(a1, 0x13A) <= 0)
			U16(a1, 0x13A) = 0;
		U16(a1, 0x13C) = U16(a1, 0x13A);
		ud_79E9E0(a1, 3);
		ud_79EB00(a1);
		const uint32_t r = x::au_re_Battle_ReadAnimation_8(a1);
		if (r == 1)
		{
			x::au_re_Battle_ReadAnimation_7(a1, r);
			U16(a1, 0x126) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_79EBE0(uint32_t a1)
	{
		if (x::au_re_Battle_ReadAnimation_8(a1) == 1)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 2);
			U16(a1, 0x126) = 0xFFE0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_79EC20(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		if (S16(a1, 0x136) >= 0x15)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_79EC40(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		U16(a1, 0x13A) = (uint16_t)(U16(a1, 0x13A) + 0x100);
		if (S16(a1, 0x13A) >= 0x1000)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 5);
			U16(a1, 0x13A) = 0x1000;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		U16(a1, 0x13C) = U16(a1, 0x13A);
		ud_79E9E0(a1, 3);
		ud_79EB00(a1);
		return 0;
	}

	uint32_t __cdecl ud_79ECB0(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::MAG_083_DirectorPtr));
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

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_083_Master_State0_Advance, (void *)a_73A0D0, "083 MAG_083_sub_7979C0" },
		{ game::MAG_083_Master_State1_Advance, (void *)a_73A0D0, "083 MAG_083_sub_7979D0" },
		{ game::MAG_083_Master_State2_CarveWorkBuffers, (void *)a_732120, "083 MAG_083_sub_7979E0" },
		{ game::MAG_083_ClearWorkBuffers, (void *)a_732160, "083 MAG_083_sub_797A20" },
		{ game::MAG_083_Master_State3_SpawnEmitter, (void *)a_73A170, "083 MAG_083_sub_797A60" },
		{ game::MAG_083_EmitterTask, (void *)a_73A1A0, "083 MAG_083_sub_797A90" },
		{ game::MAG_083_Director_State1_WaitDirGate1, (void *)a_73B790, "083 sub_797CE0" },
		{ game::MAG_083_DirSyncGate, (void *)a_73B640, "083 sub_797D00" },
		{ game::MAG_083_DirPhaseGE, (void *)a_73B7E0, "083 sub_797D60" },
		{ game::MAG_083_Director_State4_WaitDirGate2, (void *)a_73B8A0, "083 sub_797DA0" },
		{ game::MAG_083_Director_State5_Finish, (void *)a_7475A0, "083 sub_797DC0" },
		{ game::MAG_083_Director_State6_Empty, (void *)a_73A6D0, "083 nullsub_1343" },
		{ game::MAG_083_St_Empty, (void *)a_73A6D0, "083 nullsub_1344" },
		{ game::MAG_083_SpawnChildTask, (void *)a_73C100, "083 sub_797E80" },
		{ game::MAG_083_ActorSystemTask, (void *)a_73A380, "083 sub_797ED0" },
		{ game::MAG_083_DirectorSetup, (void *)a_73F990, "083 sub_797F60" },
		{ game::MAG_083_Director_StartPositions, (void *)a_73FC20, "083 sub_7981F0" },
		{ game::MAG_083_Director_Mode3Setup, (void *)a_73FE90, "083 sub_798460" },
		{ game::MAG_083_Director_TargetPositions, (void *)a_73FFB0, "083 sub_798580" },
		{ game::MAG_083_RelocateActorData, (void *)a_740210, "083 sub_7987E0" },
		{ game::MAG_083_EmitG3, (void *)a_73D770, "083 sub_799C90" },
		{ game::MAG_083_EmitG4, (void *)a_73D9C0, "083 sub_799EE0" },
		{ game::MAG_083_ActSys_UpdatePass, (void *)a_736C00, "083 sub_79AE50" },
		{ game::MAG_083_ActorStep, (void *)a_740880, "083 sub_79AE90" },
		{ game::MAG_083_ActorMotion, (void *)a_740910, "083 sub_79AF20" },
		{ game::MAG_083_DampValue, (void *)a_740F70, "083 sub_79B580" },
		{ game::MAG_083_DampVector, (void *)a_740FC0, "083 sub_79B5D0" },
		{ game::MAG_083_Instance_AdvanceTexFrame, (void *)a_741050, "083 sub_79B660" },
		{ game::MAG_083_Instance_BuildTransform, (void *)a_741120, "083 sub_79B730" },
		{ game::MAG_083_Instance_EndOfLifeTest, (void *)a_7419C0, "083 sub_79BFD0" },
		{ game::MAG_083_Instance_SpawnFollowUp, (void *)a_741A40, "083 sub_79C050" },
		{ game::MAG_083_AllocParticle, (void *)a_741A70, "083 sub_79C080" },
		{ game::MAG_083_ActSys_UpdateEmitter, (void *)a_737ED0, "083 sub_79C120" },
		{ game::MAG_083_Particle_LifeCountdown, (void *)a_741B60, "083 sub_79C170" },
		{ game::MAG_083_ActSys_EmitterSpawnStep, (void *)a_737F50, "083 sub_79C1A0" },
		{ game::MAG_083_ActSys_SpawnParticles, (void *)a_737FD0, "083 sub_79C220" },
		{ game::MAG_083_RandRange, (void *)a_742290, "083 au_re__rand_22" },
		{ game::MAG_083_RandRangeWide, (void *)a_7422C0, "083 sub_79C8D0" },
		{ game::MAG_083_RandVec3, (void *)a_742300, "083 sub_79C910" },
		{ game::MAG_083_JitterAngles3, (void *)a_742350, "083 sub_79C960" },
		{ game::MAG_083_RandAngleJitter, (void *)a_7423A0, "083 au_re__rand_22_0" },
		{ game::MAG_083_ActSys_AllocParticleSlot, (void *)a_7387B0, "083 sub_79CA00" },
		{ game::MAG_083_GetWord8Minus1, (void *)a_7424B0, "083 sub_79CAC0" },
		{ game::MAG_083_Particle_AttachToOwner, (void *)a_7424C0, "083 sub_79CAD0" },
		{ game::MAG_083_ParticleList_Append, (void *)a_742CA0, "083 sub_79D2B0" },
		{ game::MAG_083_ParticleList_Unlink, (void *)a_742CD0, "083 sub_79D2E0" },
		{ game::MAG_083_Particle_Init, (void *)a_742D00, "083 sub_79D310" },
		{ game::MAG_083_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "083 sub_79D4C0" },
		{ game::MAG_083_Particle_PlaceAtPoint, (void *)a_742FF0, "083 sub_79D600" },
		{ game::MAG_083_SpawnSubEffects, (void *)a_743100, "083 sub_79D710" },
		{ game::MAG_083_Director_SpawnGroupEffects, (void *)a_743190, "083 sub_79D7A0" },
		{ game::MAG_083_ActorSystem_State2_Empty, (void *)a_73A6D0, "083 nullsub_1345" },
		{ game::MAG_083_StageLightTask, (void *)a_73F110, "083 sub_79D860" },
		{ game::MAG_083_StageLight_State0_ResetEntityRecords, (void *)a_7335D0, "083 sub_79D8D0" },
		{ game::MAG_083_StageLight_State4_Empty, (void *)a_73A6D0, "083 nullsub_1346" },
		{ game::MAG_083_AbsorbTarget_State2_Empty, (void *)a_73A6D0, "083 nullsub_1347" },
		{ game::MAG_083_DrawBattleModel, (void *)a_746C10, "083 sub_79E780" },
		{ game::MAG_083_SetEntityColourTint, (void *)a_733950, "083 sub_79EA00" },
		{ game::MAG_083_Cherub_State5_Empty, (void *)a_73A6D0, "083 nullsub_1348" },
		{ game::MAG_083_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "083 MAG_083_sub_79ECF0" },
		{ game::MAG_083_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "083 MAG_083_sub_79ED10" },
		{ game::MAG_083_Emitter_State4_Empty, (void *)a_73A6D0, "083 nullsub_1349" },
		{ game::MAG_083_Master_State4_NextTargetLoop, (void *)a_747500, "083 MAG_083_sub_79ED40" },
		{ game::MAG_083_Master_State5_Advance, (void *)a_73A0D0, "083 MAG_083_sub_79ED80" },
		{ game::MAG_083_Master_State6_WaitQueuesEmpty, (void *)a_747550, "083 MAG_083_sub_79ED90" },
		{ game::MAG_083_Master_State7_RequestBattleSync, (void *)a_747560, "083 MAG_083_sub_79EDA0" },
		{ game::MAG_083_Master_State8_WaitBattleSyncDone, (void *)a_747590, "083 MAG_083_sub_79EDD0" },
		{ game::MAG_083_Master_State9_Finish, (void *)a_7475A0, "083 MAG_083_sub_79EDE0" },
		{ game::MAG_083_Master_State10_Empty, (void *)a_73A6D0, "083 nullsub_1342" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_083_Master, (void *)ud_797840, "083 ud_797840" },
		{ game::MAG_083_Emitter_State0_PlaceAndStartDirector, (void *)ud_797B10, "083 ud_797B10" },
		{ game::MAG_083_PlaceDirectorAtTarget, (void *)ud_797B40, "083 ud_797B40" },
		{ game::MAG_083_Director, (void *)ud_797BF0, "083 ud_797BF0" },
		{ game::MAG_083_Director_CutBookkeeping, (void *)ud_797C70, "083 ud_797C70" },
		{ game::MAG_083_UploadCherubTIM, (void *)ud_797CC0, "083 ud_797CC0" },
		{ game::MAG_083_Director_SoundAtCut1, (void *)ud_797D30, "083 ud_797D30" },
		{ game::MAG_083_Director_DamageCueAtFrame6E, (void *)ud_797D80, "083 ud_797D80" },
		{ game::MAG_083_SpawnCherubCreature, (void *)ud_797E00, "083 ud_797E00" },
		{ game::MAG_083_ActorSystem_State0_StartActorSystemAtFrame, (void *)ud_797F30, "083 ud_797F30" },
		{ game::MAG_083_ResetTrailHistory, (void *)ud_798110, "083 ud_798110" },
		{ game::MAG_083_FillTrailRows, (void *)ud_798390, "083 ud_798390" },
		{ game::MAG_083_ActorSystem_Step, (void *)ud_798A40, "083 ud_798A40" },
		{ game::MAG_083_ActorSystem_DrawModels, (void *)ud_798AD0, "083 ud_798AD0" },
		{ game::MAG_083_ActorSystem_DrawModelActor, (void *)ud_798B30, "083 ud_798B30" },
		{ game::MAG_083_RenderPrimModel, (void *)ud_798CB0, "083 ud_798CB0" },
		{ game::MAG_083_PrimList_FlatTri, (void *)ud_798DE0, "083 ud_798DE0" },
		{ game::MAG_083_PrimList_FlatQuad, (void *)ud_799000, "083 ud_799000" },
		{ game::MAG_083_PrimList_TexTri, (void *)ud_799290, "083 ud_799290" },
		{ game::MAG_083_PrimList_TexQuad, (void *)ud_799710, "083 ud_799710" },
		{ game::MAG_083_PrimList_GT3Scroll, (void *)ud_79A1C0, "083 ud_79A1C0" },
		{ game::MAG_083_PrimList_GT4, (void *)ud_79A6C0, "083 ud_79A6C0" },
		{ game::MAG_083_ActSys_DrawSpritePass, (void *)ud_79AD10, "083 ud_79AD10" },
		{ game::MAG_083_ActSys_DrawActorSprite, (void *)ud_79ADD0, "083 ud_79ADD0" },
		{ game::MAG_083_MorphVertexBlock, (void *)ud_79BEC0, "083 ud_79BEC0" },
		{ game::MAG_083_Actor_InterpolatePosition, (void *)ud_79CC20, "083 ud_79CC20" },
		{ game::MAG_083_ActorSystem_State1_StepActorSystem, (void *)ud_79D830, "083 ud_79D830" },
		{ game::MAG_083_StageLight_RampUp, (void *)ud_79D910, "083 ud_79D910" },
		{ game::MAG_083_StageLight_Hold, (void *)ud_79D950, "083 ud_79D950" },
		{ game::MAG_083_StageLight_RampDown, (void *)ud_79D960, "083 ud_79D960" },
		{ game::MAG_083_AbsorbTargetTask, (void *)ud_79D9B0, "083 ud_79D9B0" },
		{ game::MAG_083_AbsorbTarget_Draw, (void *)ud_79DA20, "083 ud_79DA20" },
		{ game::MAG_083_AbsorbTarget_DrawObjectPulledApart, (void *)ud_79DB00, "083 ud_79DB00" },
		{ game::MAG_083_AbsorbTarget_State0_Start, (void *)ud_79E530, "083 ud_79E530" },
		{ game::MAG_083_AbsorbTarget_InitDrawBlock, (void *)ud_79E590, "083 ud_79E590" },
		{ game::MAG_083_AbsorbTarget_ClearPolygonStates, (void *)ud_79E5F0, "083 ud_79E5F0" },
		{ game::MAG_083_AbsorbTarget_State1_Countdown, (void *)ud_79E610, "083 ud_79E610" },
		{ game::MAG_083_CherubTask, (void *)ud_79E690, "083 ud_79E690" },
		{ game::MAG_083_Cherub_State0_Appear, (void *)ud_79E960, "083 ud_79E960" },
		{ game::MAG_083_Cherub_ApplyFade, (void *)ud_79E9E0, "083 ud_79E9E0" },
		{ game::MAG_083_Cherub_ApplyAmbientColour, (void *)ud_79EB00, "083 ud_79EB00" },
		{ game::MAG_083_Cherub_State1_FadeIn, (void *)ud_79EB70, "083 ud_79EB70" },
		{ game::MAG_083_Cherub_State2_Anim1, (void *)ud_79EBE0, "083 ud_79EBE0" },
		{ game::MAG_083_Cherub_State3_Anim2, (void *)ud_79EC20, "083 ud_79EC20" },
		{ game::MAG_083_Cherub_State4_FadeOut, (void *)ud_79EC40, "083 ud_79EC40" },
		{ game::MAG_083_Emitter_ApplyDamageOnCue, (void *)ud_79ECB0, "083 ud_79ECB0" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag083_ultimecia_absorb()
	{
		act::register_module(83);
		for (const act::u083::ModPort *p = act::u083::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(83, p->addr, p->port, p->name);
		for (const act::u083::ModPort *p = act::u083::PORTS; p->addr; p++)
			act::register_module_port(83, p->addr, p->port, p->name);
	}
}

