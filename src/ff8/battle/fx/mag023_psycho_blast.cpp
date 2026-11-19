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
namespace psycho
{
	static const Mod MOD_023 = { "psycho", 23, game::MAG_023_PSYCHO_BLAST_FL, game::MAG_022_BIO_FL,
		{ {}, {}, game::p_PsychoBlast_DirectorBlock, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::g_PsychoBlast_FilePtr, {}, {}, {}, game::unk_2701304, {}, {}, {}, {}, {}, {}, {}, game::MAG_023_ActorSystemQueue, {}, game::MAG_023_EmitterQueue, {}, {}, {}, {}, {}, game::unk_270420C, {}, {}, game::unk_27044C0, {}, game::g_PsychoBlast_ScratchSP, {}, game::g_PsychoBlast_ActorState },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_023_ActSys_DrawPass, game::MAG_023_ActSys_DrawPrimActor, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_023_ActSys_DrawSpritePass, game::MAG_023_ActSys_UpdatePass, game::MAG_023_ActorStep, game::MAG_023_ActSys_UpdateEmitter, game::MAG_023_Particle_LifeCountdown, game::MAG_023_ActSys_EmitterSpawnStep, game::MAG_023_ActSys_SpawnParticles, game::MAG_023_RandVec3, game::MAG_023_JitterAngles3, game::MAG_023_ActSys_AllocParticleSlot, game::MAG_023_GetWord8Minus1, game::MAG_023_Particle_Init, game::MAG_023_SpawnSubEffects, game::MAG_023_Director_SpawnGroupEffects, {}, {}, {}, {}, {}, game::MAG_023_PsychoBlast_EmitterTask, game::MAG_023_PsychoBlast_Emitter_StartDirector, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_023_DirSyncGate, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_023_ResetTrailHistory, game::MAG_023_Director_StartPositions, game::MAG_023_FillTrailRows, game::MAG_023_Director_Mode3Setup, game::MAG_023_Director_TargetPositions, game::MAG_023_RelocateActorData, game::MAG_023_ActSys_DrawActorSprite, game::MAG_023_ActorMotion, game::MAG_023_DampValue, game::MAG_023_DampVector, game::MAG_023_Instance_AdvanceTexFrame, game::MAG_023_Instance_BuildTransform, game::MAG_023_MorphVertexBlock, game::MAG_023_Instance_EndOfLifeTest, game::MAG_023_Instance_SpawnFollowUp, game::MAG_023_AllocParticle, game::MAG_023_RandRange, game::MAG_023_RandRangeWide, game::MAG_023_RandAngleJitter, game::MAG_023_Particle_AttachToOwner, game::MAG_023_Actor_InterpolatePosition, {}, game::MAG_023_ParticleList_Unlink, game::MAG_023_Particle_PlaceAtAnchorSet, game::MAG_023_Particle_PlaceAtPoint, game::MAG_023_SpriteStrip_DrawQuads, game::MAG_023_PsychoBlast_ActorSystem_Step, game::MAG_023_PsychoBlast_Emitter_ApplyDamage, game::MAG_023_PsychoBlast_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_023_PsychoBlast_Emitter_State3_FinishReleaseRoot, game::MAG_023_PsychoBlast_Emitter_State4_Empty, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::g_PsychoBlast_PacketCursor;
	static constexpr game::Id ACTOR_STATE = game::g_PsychoBlast_ActorState;
	static constexpr game::Id SCRATCH_SP = game::g_PsychoBlast_ScratchSP;
	static constexpr game::Id DIR_PTR = game::p_PsychoBlast_DirectorBlock;
	static constexpr game::Id GEO_PTR = game::p_PsychoBlast_GeometryBlock;
	static constexpr game::Id Q_EMITTER = game::MAG_023_EmitterQueue, Q_DIRECTOR = game::MAG_023_DirectorQueue, Q_ACTORS = game::MAG_023_ActorSystemQueue, Q_PRIM = game::MAG_023_PrimModelQueue, Q_DEBRIS = game::MAG_023_DebrisQueue, Q_STAGE = game::MAG_023_StageWobbleQueue;
	static constexpr game::Id ORIG_Master = game::MAG_023_PsychoBlast_MasterTask;
	static constexpr game::Id ORIG_Orb = game::MAG_023_PsychoBlast_OrbTask, ORIG_Ring = game::MAG_023_PsychoBlast_RingTask, ORIG_Burst = game::MAG_023_PsychoBlast_BurstTask;
	static constexpr game::Id ORIG_Spawner = game::MAG_023_PsychoBlast_DebrisSpawnerTask, ORIG_Debris = game::MAG_023_PsychoBlast_DebrisTask;
	static constexpr game::Id PRIM_CB = game::MAG_023_PsychoBlast_PrimModelCallback;
	static constexpr game::Id BLEND_BUF = game::g_PsychoBlast_VertexBlendBuffer;
	static constexpr game::Id SOUND_Blast = game::PsychoBlast_Sound;
	static inline uint32_t ENT(uint32_t slot) { return game_addr(game::BattleEntitySlotData) + slot * 0x9C; }

	namespace gx
	{
		static inline uint32_t sub_45E0B0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadIR123_u16))(a1); }
		static inline uint32_t sub_45E9D0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPF))(); }
		static inline uint32_t sub_45EBF0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPL))(); }
		static inline uint32_t sub_8DDAC0(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::sub_8DDAC0))(a1, a2, a3); }
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

	uint32_t __cdecl pb_893190(uint32_t a1);
	uint32_t __cdecl pb_893320(uint32_t a1);
	uint32_t __cdecl pb_893360(void);
	uint32_t __cdecl pb_893450(uint32_t a1);
	uint32_t __cdecl pb_893480(uint32_t a1);
	uint32_t __cdecl pb_893500(uint32_t a1);
	uint32_t __cdecl pb_893580(uint32_t a1);
	uint32_t __cdecl pb_8935D0(uint32_t a1);
	uint32_t __cdecl pb_893630(uint32_t a1);
	uint32_t __cdecl pb_893760(uint32_t a1);
	uint32_t __cdecl pb_8937A0(uint32_t a1);
	uint32_t __cdecl pb_893830(uint32_t a1);
	uint32_t __cdecl pb_893900(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl pb_893950(uint32_t a1);
	uint32_t __cdecl pb_8939B0(uint32_t a1);
	uint32_t __cdecl pb_893A30(uint32_t a1);
	uint32_t __cdecl pb_893B20(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl pb_893DE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl pb_893F10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl pb_894130(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl pb_8943C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl pb_894840(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl pb_8952F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl pb_8957F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl pb_895E40(uint32_t a1, uint32_t a2);
	uint32_t __cdecl pb_895ED0(uint32_t a1);
	uint32_t __cdecl pb_896050(uint32_t a1);
	uint32_t __cdecl pb_8960C0(uint32_t a1);
	uint32_t __cdecl pb_8960F0(uint32_t a1);
	uint32_t __cdecl pb_8961B0(uint32_t a1);
	uint32_t __cdecl pb_896220(uint32_t a1);
	uint32_t __cdecl pb_8962B0(uint32_t a1);
	uint32_t __cdecl pb_8964B0(uint32_t a1);
	uint32_t __cdecl pb_896730(void);
	uint32_t __cdecl pb_896DE0(uint32_t a1);
	uint32_t __cdecl pb_896E70(void);
	uint32_t __cdecl pb_896ED0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl pb_897030(void);
	uint32_t __cdecl pb_8970F0(uint32_t a1);
	uint32_t __cdecl pb_897170(void);
	uint32_t __cdecl pb_8971B0(uint32_t a1);
	uint32_t __cdecl pb_8981E0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl pb_8982F0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl pb_898370(uint32_t a1, uint32_t a2);
	uint32_t __cdecl pb_8983A0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl pb_898440(uint32_t a1);
	uint32_t __cdecl pb_8984C0(uint32_t a1);
	uint32_t __cdecl pb_898540(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl pb_898D20(uint32_t a1, uint32_t a2);
	uint32_t __cdecl pb_898F40(uint32_t a1);
	uint32_t __cdecl pb_899A30(uint32_t a1, uint32_t a2);
	uint32_t __cdecl pb_899AC0(void);
	uint32_t __cdecl pb_899B50(uint32_t a1);
	uint32_t __cdecl pb_899B80(uint32_t a1);
	uint32_t __cdecl pb_899BD0(uint32_t a1);
	uint32_t __cdecl pb_899CC0(uint32_t a1);
	uint32_t __cdecl pb_899DD0(uint32_t a1);
	uint32_t __cdecl pb_89A4A0(uint32_t a1);
	uint32_t __cdecl pb_89A4C0(uint32_t a1);
	uint32_t __cdecl pb_89A530(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace psycho
{
	uint32_t __cdecl pb_893320(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::MAG_023_FileArenaCursor));
		MEM<uint32_t>(game_addr(game::MAG_023_ParticlePoolPtr)) = p;
		p += 0x438;
		MEM<uint32_t>(game_addr(game::MAG_023_ActorPoolPtr)) = p;
		p += 0xD200;
		MEM<uint32_t>(game_addr(game::MAG_023_FileArenaCursor)) = p;
		pb_893360();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl pb_893360(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_023_ParticlePoolPtr)), 0x438);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_023_ActorPoolPtr)), 0xD200);
		MEM<uint16_t>(game_addr(game::MAG_023_ParticleCursor)) = 0;
		MEM<uint16_t>(game_addr(game::MAG_023_ActorCursor)) = 0;
		MEM<uint16_t>(game_addr(game::unk_27041B0)) = 0;
		MEM<uint16_t>(game_addr(game::unk_27011C0)) = 0;
		return 0;
	}

	uint32_t __cdecl pb_893450(uint32_t a1)
	{
		pb_893480(a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_023_DirectorQueue), game_addr(game::MAG_023_PsychoBlast_DirectorTask), 0x48, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl pb_893500(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[6];
		states[0] = game_addr(game::MAG_023_PsychoBlast_Director_Cue1_SetGeometry);
		states[1] = game_addr(game::MAG_023_PsychoBlast_Director_State1_WaitDirPhase1);
		states[2] = game_addr(game::MAG_023_PsychoBlast_Director_PlaySound);
		states[3] = game_addr(game::MAG_023_PsychoBlast_Director_State3_WaitDirGate2);
		states[4] = game_addr(game::MAG_023_PsychoBlast_Director_State4_Finish);
		states[5] = game_addr(game::MAG_023_PsychoBlast_Director_State5_Empty);
		pb_893580(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl pb_893580(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			pb_893830(a1);
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

	uint32_t __cdecl pb_8935D0(uint32_t a1)
	{
		if (a_73B640(1) != 0)
		{
			pb_893630(a1);
			t1_next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl pb_893760(uint32_t a1)
	{
		if (a_73B7E0(1) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl pb_893900(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PRIM), a2, 0x180, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		S32(t, 0x78) = (int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl pb_893950(uint32_t a1)
	{
		const uint32_t tab[4] = { game_addr(game::MAG_023_PsychoBlast_Orb_Appear), game_addr(game::MAG_023_PsychoBlast_Orb_Flight), game_addr(game::MAG_023_PsychoBlast_Orb_Flatten), game_addr(game::MAG_023_PsychoBlast_Orb_State3_Empty) };
		callp(tab[S8(a1, 0x29)], a1);
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24)++;
		return s3_task_end(a1, status);
	}

	uint32_t __cdecl pb_893DE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		static const Draw lists[8] = { pb_893F10, pb_894130, pb_8943C0, pb_894840, a_73D770, a_73D9C0, pb_8952F0, pb_8957F0 };
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

	uint32_t __cdecl pb_893F10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl pb_894130(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl pb_8943C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl pb_894840(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl pb_8952F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl pb_8957F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl pb_8960C0(uint32_t a1)
	{
		const uint32_t r = pb_893A30(a1);
		if (r == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl pb_896220(uint32_t a1)
	{
		const uint32_t r = pb_893A30(a1);
		if (r == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl pb_8964B0(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::g_PsychoBlast_ActorState));
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

	uint32_t __cdecl pb_896730(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::g_PsychoBlast_ActorState));
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

	uint32_t __cdecl pb_896DE0(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::g_PsychoBlast_ActorState), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			pb_899AC0();
			pb_897170();
			pb_897030();
			pb_896E70();
			sys = U32(game_addr(game::g_PsychoBlast_ActorState), 0);
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
		U16(game_addr(game::unk_27044C0), 0) = (uint16_t)(U16(game_addr(game::unk_27044C0), 0) + c14);
		U16(game_addr(game::unk_270420C), 0) = (uint16_t)(U16(game_addr(game::unk_270420C), 0) + c16);
		return done;
	}

	uint32_t __cdecl pb_896E70(void)
	{
		uint32_t act = U32(U32(game_addr(game::g_PsychoBlast_ActorState), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::g_PsychoBlast_ActorState), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					pb_896ED0(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl pb_896ED0(uint32_t a1, uint32_t a2)
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
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = pb_893DE0(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
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
				MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = pb_893DE0(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
			}
		}
		const uint32_t top = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 0x68;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = top;
		return top;
	}

	uint32_t __cdecl pb_897030(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::g_PsychoBlast_ActorState)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::g_PsychoBlast_ActorState));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						pb_8970F0(act);
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
							pb_8970F0(act);
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

	uint32_t __cdecl pb_8970F0(uint32_t a1)
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

	uint32_t __cdecl pb_897170(void)
	{
		uint32_t act = U32(U32(game_addr(game::g_PsychoBlast_ActorState), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				pb_898440(act);
			else if (type == 1)
				pb_8971B0(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl pb_8971B0(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::g_PsychoBlast_ActorState));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (pb_8982F0(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::g_PsychoBlast_ActorState));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl pb_8981E0(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::g_PsychoBlast_ActorState), 0);
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

	uint32_t __cdecl pb_8982F0(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				pb_898370(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			pb_898370(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			pb_898370(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl pb_898370(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return pb_8983A0(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl pb_8983A0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_023_ParticlePoolPtr));
		int32_t idx = MEM<int16_t>(game_addr(game::MAG_023_ParticleCursor));
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
			MEM<uint16_t>(game_addr(game::MAG_023_ParticleCursor)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::MAG_023_ParticleCursor)) = 0;
		return slot;
	}

	uint32_t __cdecl pb_898440(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_023_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_023_Actor_InterpolatePosition), a1);
		pb_8984C0(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl pb_8984C0(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::g_PsychoBlast_ActorState), 0);
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
				pb_898540(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			pb_898540(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl pb_898540(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::g_PsychoBlast_ScratchSP), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::g_PsychoBlast_ScratchSP), 0) = sp;
		uint32_t node = pb_898D20(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::g_PsychoBlast_ActorState), 0);
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
		pb_899A30(node, desc);
		U32(game_addr(game::g_PsychoBlast_ScratchSP), 0) = U32(game_addr(game::g_PsychoBlast_ScratchSP), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl pb_898D20(uint32_t a1, uint32_t a2)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_023_ActorPoolPtr));
		int32_t i = MEM<int16_t>(game_addr(game::MAG_023_ActorCursor));
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
			if (i >= 0x4F)
				i = 0;
			tries++;
			if (tries >= 0x50)
				break;
		}
		i++;
		if (i >= 0x4F)
			MEM<uint16_t>(game_addr(game::MAG_023_ActorCursor)) = 0;
		else
			MEM<uint16_t>(game_addr(game::MAG_023_ActorCursor)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl pb_898F40(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::g_PsychoBlast_ActorState), 0);
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
				uint32_t g = U32(game_addr(game::g_PsychoBlast_ActorState), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::g_PsychoBlast_ActorState), 0);
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

	uint32_t __cdecl pb_899A30(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			pb_8983A0(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			pb_8983A0(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			pb_8983A0(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			pb_8983A0(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl pb_899AC0(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::g_PsychoBlast_ActorState));
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
					pb_8983A0(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::g_PsychoBlast_ActorState));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				pb_8983A0(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::g_PsychoBlast_ActorState));
			}
		}
		return 0;
	}

	uint32_t __cdecl pb_899B50(uint32_t a1)
	{
		if (pb_896DE0(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl pb_899B80(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[2];
		states[0] = game_addr(game::MAG_023_PsychoBlast_DebrisSpawner_Spawn);
		states[1] = game_addr(game::MAG_023_PsychoBlast_DebrisSpawner_State1_Empty);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl pb_899DD0(uint32_t a1)
	{
		const uint32_t node = a1;
		if ((U8(node, 0x26) & 4) != 0)
			return 0;
		const uint32_t blk = x::Field_Alloc(0xD8);
		const uint32_t mat = blk + 0xB8;
		x::UnpackRotationMatrix(game_addr(game::unk_1D97778), mat);
		const int32_t px = S16(node, 0x1C);
		const int32_t py = S16(node, 0x1E);
		const int32_t pz = S16(node, 0x20);
		const uint32_t tr = blk + 0xCC;
		S32(blk, 0xD0) = py;
		S32(blk, 0xD4) = pz;
		S32(tr, 0) = px;
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(mat);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(mat);
		x::GTE_LoadIRFromMatrixColumn(blk + 0xBA);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(blk + 0xBA);
		x::GTE_LoadIRFromMatrixColumn(blk + 0xBC);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(blk + 0xBC);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(tr);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(tr);
		x::GTE_SetRotMatrix(mat);
		x::GTE_SetTransVector(mat);
		U32(blk, 0) = U32(node, 0x4C);
		const uint16_t frame = U16(node, 0x50);
		const uint16_t colour = U16(node, 0x56);
		U16(blk, 4) = frame;
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::g_PsychoBlast_PacketCursor));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(blk, 0x24) = 0;
		U16(blk, 0xB4) = colour;
		MEM<uint32_t>(game_addr(game::g_PsychoBlast_PacketCursor)) = a_7435E0(blk, ot, 2, cursor);
		x::Field_Free(0xD8);
		return 0;
	}

	uint32_t __cdecl pb_89A4A0(uint32_t a1)
	{
		uint32_t node = a1;
		uint8_t st = U8(node, 0x29);
		U32(node, 0x4c) = game_addr(game::unk_161428C);
		st++;
		U16(node, 0x56) = 0xFFC0;
		U16(node, 0x52) = 8;
		U8(node, 0x29) = st;
		return 0;
	}

	uint32_t __cdecl pb_89A4C0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl pb_89A530(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::p_PsychoBlast_DirectorBlock));
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

	uint32_t __cdecl pb_893190(uint32_t a1)
	{
		g_mod = &MOD_023;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_2701304)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_023_PsychoBlast_Master_State0_Advance);
		states[1] = game_addr(game::MAG_023_PsychoBlast_Master_State1_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[2] = game_addr(game::MAG_023_PsychoBlast_Master_CarveActorPools);
		states[3] = game_addr(game::MAG_023_PsychoBlast_Master_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_023_PsychoBlast_Master_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_023_PsychoBlast_Master_State5_Advance);
		states[6] = game_addr(game::MAG_023_PsychoBlast_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_023_PsychoBlast_Master_State7_RequestBattleSync);
		states[8] = game_addr(game::MAG_023_PsychoBlast_Master_State8_WaitBattleSyncDone);
		states[9] = game_addr(game::MAG_023_PsychoBlast_Master_State9_Finish);
		states[10] = game_addr(game::MAG_023_PsychoBlast_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_27041AC));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_2703AEC));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_2701300)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_27041A8));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_2703AE8));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_2701300)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_27044C0)) = 0;
		MEM<uint16_t>(game_addr(game::unk_270420C)) = 0;
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_EMITTER)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_DIRECTOR)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_ACTORS)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_DEBRIS)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_PRIM)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_STAGE)));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl pb_893480(uint32_t a1)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(DIR_PTR)), 0x54);
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		const uint32_t ent = ENT(U8(a1, 0x2D));
		U32(d, 8) = U32(ent, 0x1C);
		U32(d, 0xC) = U32(ent, 0x20);
		U16(d, 0) = 0;
		U16(d, 2) = 0;
		U16(d, 4) = 0;
		const int16_t dx = (int16_t)(U16(d, 8) - U16(d, 0));
		const uint32_t ang = x::CartesianToGameAngle((uint32_t)(int32_t)dx, (uint32_t)(int32_t)S16(d, 0xC));
		const uint32_t r = (ang - 0x800) & 0xFFF;
		U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x4A) = (uint16_t)r;
		return r;
	}

	uint32_t __cdecl pb_893630(uint32_t a1)
	{
		alignas(4) uint8_t loc[0x28] = {};
		const uint32_t V = P(loc);
		const uint32_t M = V + 8;
		const uint32_t caster = ENT(U8(a1, 0x2C));
		const uint32_t target = ENT(U8(a1, 0x2D));
		x::MAG_022_sub_8DD770(M);
		x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)S16(caster, 0xE));
		U16(V, 0) = 0;
		U16(V, 2) = 0xF200;
		U16(V, 4) = 0xFC00;
		x::matrixMultiplyVector(M, V, V);
		uint32_t b = MEM<uint32_t>(game_addr(GEO_PTR));
		U32(b, 0x20) = U32(caster, 0x1C);
		U32(b, 0x24) = U32(caster, 0x20);
		U16(b, 0x20) = (uint16_t)(U16(b, 0x20) + U16(V, 0));
		U16(b, 0x22) = (uint16_t)(U16(b, 0x22) + U16(V, 2));
		U16(b, 0x24) = (uint16_t)(U16(b, 0x24) + U16(V, 4));
		U16(V, 0) = 0;
		U16(V, 2) = 0xF200;
		U16(V, 4) = 0xF688;
		x::matrixMultiplyVector(M, V, V);
		b = MEM<uint32_t>(game_addr(GEO_PTR));
		U32(b, 0x28) = U32(caster, 0x1C);
		U32(b, 0x2C) = U32(caster, 0x20);
		U16(b, 0x28) = (uint16_t)(U16(b, 0x28) + U16(V, 0));
		U16(b, 0x2A) = (uint16_t)(U16(b, 0x2A) + U16(V, 2));
		U16(b, 0x2C) = (uint16_t)(U16(b, 0x2C) + U16(V, 4));
		x::GetEffectSpawnPosition(target, 0xF0, 0, b + 0x30);
		b = MEM<uint32_t>(game_addr(GEO_PTR));
		return gx::sub_8DDAC0(b, b + 0x28, b + 0x30);
	}

	uint32_t __cdecl pb_8937A0(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x46) < 4)
			return 0;
		x::BdPlaySE(game_addr(SOUND_Blast), 0, 0x80);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl pb_893830(uint32_t a1)
	{
		const int32_t ph = (int32_t)S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x40) - 1;
		if (ph != 0)
			return (uint32_t)ph;
		a_73C100(a1, game_addr(game::MAG_023_PsychoBlast_ActorSystemTask), game_addr(game::unk_17EA4F0), 0, 0x28, 4);
		pb_893900(a1, game_addr(ORIG_Ring), game_addr(game::unk_17EE254), 0xD8, 0, 0);
		pb_893900(a1, game_addr(ORIG_Orb), game_addr(game::unk_17F01B0), 0x58, 1, 0);
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_STAGE), game_addr(game::Effect_ScreenTintTask), 0x40, a1);
		U32(t, 0x34) = game_addr(game::PsychoBlast_TintScript);
		U16(t, 0x38) = 0x27;
		return t;
	}

	uint32_t __cdecl pb_8939B0(uint32_t a1)
	{
		const uint32_t n = a1;
		if (S16(n, 0x24) < 0x23)
			return 0;
		x::Effect_DecodeModelPrimLayout(U32(n, 0x74), n + 0x94, U32(n, 0x78));
		U32(n, 0x58) = 0x1000;
		U32(n, 0x54) = 0x1000;
		U32(n, 0x50) = 0x1000;
		const uint32_t b = MEM<uint32_t>(game_addr(GEO_PTR));
		const uint32_t w2c = U32(b, 0x2C);
		ff8fx::copy<(0x20)>((void *)(n + 0x30), (const void *)b);
		const uint32_t w28 = U32(b, 0x28);
		U32(n, 0x1C) = w28;
		U32(n, 0x16C) = w28;
		U32(n, 0x20) = w2c;
		U32(n, 0x170) = w2c;
		pb_895E40(n, n + 0x174);
		pb_893A30(n);
		U8(n, 0x29) = (uint8_t)(U8(n, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl pb_893A30(uint32_t a1)
	{
		alignas(4) uint8_t fr[0x5C] = {};
		const uint32_t L = P(fr);
		ff8fx::copy<(0x20)>((void *)L, (const void *)(a1 + 0x30));
		x::scale3DMatrix(L, a1 + 0x50);
		S32(L, 0x14) = S16(a1, 0x1C);
		S32(L, 0x18) = S16(a1, 0x1E);
		S32(L, 0x1C) = S16(a1, 0x20);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), L, L);
		U16(L, 0x54) = U16(a1, 0x86);
		U32(L, 0x44) = U32(a1, 0x7C);
		U32(L, 0x38) = U32(a1, 0x70);
		U16(L, 0x4C) = U16(a1, 0x8C);
		U16(L, 0x4E) = U16(a1, 0x8E);
		U16(L, 0x52) = U16(a1, 0x92);
		U16(L, 0x50) = U16(a1, 0x90);
		U32(L, 0x48) = game_addr(BLEND_BUF);
		U16(L, 0x56) = U16(a1, 0x80);
		U16(L, 0x58) = U16(a1, 0x88);
		return prim_play(a1 + 0x94, game_addr(PRIM_CB), L, 0);
	}

	uint32_t __cdecl pb_893B20(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		alignas(4) uint8_t loc[0x48] = {};
		const uint32_t L = P(loc);
		const uint32_t V = L;
		const uint32_t S = L + 8;
		const uint32_t M = L + 0x28;
		const uint32_t rec = a2;
		if (((uint32_t)(int32_t)S16(rec, 0x1C) | U32(rec, 0x18)) == 0)
			return 0;
		if (S16(rec, 0x24) >= 0x1000 && U32(rec, 0x20) == 0)
			return 0;

		const uint32_t hdr = x::Field_Alloc(0x68);
		const int32_t model = S16(rec, 2);
		const uint32_t data = U32(a1, 0);
		const uint32_t blk = a3;
		const uint32_t base = data + U32(data, model * 4 + 8);
		const int16_t f1 = S16(rec, 0x2A);
		const int16_t f0 = S16(rec, 0x28);
		U32(hdr, 0) = base;
		auto frame_ptr = [base](int16_t f) -> uint32_t {
			if (f == 0)
				return base + 0xC;
			const int32_t n = mul32(S32(base, 4), (int32_t)f);
			return base + (uint32_t)shl32(n, 3) + 0xC;
		};
		if (f0 == f1)
			U32(hdr, 4) = frame_ptr(f0);
		else
		{
			const int16_t t = S16(rec, 0x26);
			if (t == 0)
				U32(hdr, 4) = frame_ptr(f0);
			else if (t == 0x1000)
				U32(hdr, 4) = frame_ptr(f1);
			else
			{
				x::MAG_017_sub_701390(base, (uint32_t)(int32_t)f0, (uint32_t)(int32_t)f1, (uint32_t)(int32_t)t, U32(blk, 0x48));
				U32(hdr, 4) = U32(blk, 0x48);
			}
		}

		x::MAG_017_sub_701310(rec + 0x10, M);
		const uint32_t flags0 = U32(rec, 4);
		const uint16_t vx = U16(rec, 8);
		const uint16_t vy = U16(rec, 0xA);
		const uint16_t vz = U16(rec, 0xC);
		U16(V, 0) = vx;
		U16(V, 2) = vy;
		U16(V, 4) = vz;
		int32_t t0, t1, t2;
		if ((flags0 & 0x200) != 0)
		{
			t0 = (int16_t)vx;
			t1 = (int16_t)vy;
			t2 = (int16_t)vz;
		}
		else
		{
			x::GTE_SetRotMatrix(blk);
			x::GTE_LoadV0(V);
			x::GTE_MVMVA_RotV0();
			x::GTE_ReadMAC123(M + 0x14);
			x::GTE_MatrixMultiply(blk, M);
			t2 = S32(M, 0x1C);
			t1 = S32(M, 0x18);
			t0 = S32(M, 0x14);
		}
		S32(M, 0x14) = add32(t0, S32(blk, 0x14));
		S32(M, 0x18) = add32(t1, S32(blk, 0x18));
		S32(M, 0x1C) = add32(t2, S32(blk, 0x1C));
		if (U32(rec, 0x18) != 0x10001000 || U16(rec, 0x1C) != 0x1000)
		{
			if ((U32(rec, 4) & 0x100) != 0)
			{
				U16(S, 8) = U16(rec, 0x1A);
				U16(S, 0) = U16(rec, 0x18);
				U16(S, 0xA) = 0;
				U16(S, 4) = 0;
				U16(S, 0xE) = 0;
				U16(S, 2) = 0;
				U16(S, 0xC) = 0;
				U16(S, 6) = 0;
				U16(S, 0x10) = U16(rec, 0x1C);
				x::sub_56C220(M, S);
			}
			else
			{
				S32(S, 0) = S16(rec, 0x18);
				S32(S, 4) = S16(rec, 0x1A);
				S32(S, 8) = S16(rec, 0x1C);
				x::scale3DMatrix(M, S);
			}
		}
		x::GTE_SetRotMatrix_W(M);
		x::GTE_SetTransVector_W(M);
		const uint32_t flags = U32(rec, 4);
		U32(hdr, 0x14) = (flags & 0x4000) != 0 ? 0x2000u : 0x2030u;
		if ((flags & 0x2000) != 0)
			U32(hdr, 0x14) = U32(hdr, 0x14) | 0xC;
		const int32_t alpha = S16(rec, 0x24);
		U32(hdr, 0xC) = (uint32_t)alpha;
		if (alpha != 0)
		{
			const uint32_t fl = U32(hdr, 0x14);
			U32(hdr, 8) = U32(rec, 0x20);
			U32(hdr, 0x14) = fl | 0xC0;
		}
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(hdr, 0x18) = 0;
		U16(hdr, 0x1A) = 0;
		U16(hdr, 0x1E) = 0;
		U16(hdr, 0x1C) = 0;
		U16(hdr, 0x26) = 0;
		U16(hdr, 0x24) = 0;
		const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		U32(hdr, 0x10) = (uint32_t)(int32_t)S16(blk, 0x58);
		U16(hdr, 0x22) = 0x100;
		U16(hdr, 0x20) = 0x100;
		U16(hdr, 0x2A) = 0x100;
		U16(hdr, 0x28) = 0x100;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = pb_893DE0(hdr, ot, 2, cursor);
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl pb_895E40(uint32_t a1, uint32_t a2)
	{
		alignas(4) uint8_t loc[0x28] = {};
		const uint32_t V = P(loc);
		const uint32_t M = V + 8;
		U16(V, 0) = 0;
		U16(V, 2) = 0;
		ff8fx::copy<(0x20)>((void *)M, (const void *)(a1 + 0x30));
		U16(V, 4) = 0xFE00;
		x::scale3DMatrix(M, a1 + 0x50);
		x::matrixMultiplyVector(M, V, V);
		const uint32_t b = MEM<uint32_t>(game_addr(GEO_PTR));
		U16(a2, 0) = (uint16_t)(U16(b, 0x30) + U16(V, 0));
		const uint16_t y = (uint16_t)(U16(b, 0x32) + U16(V, 2));
		const uint16_t z = (uint16_t)(U16(b, 0x34) + U16(V, 4));
		U16(a2, 2) = y;
		U16(a2, 4) = z;
		return z;
	}

	uint32_t __cdecl pb_895ED0(uint32_t a1)
	{
		const uint32_t n = a1;
		U16(n, 0x17C) = (uint16_t)(U16(n, 0x17C) + 0x556);
		const int16_t t = S16(n, 0x17C);
		if (t >= 0x1000)
		{
			const uint32_t b = MEM<uint32_t>(game_addr(GEO_PTR));
			U32(n, 0x1C) = U32(b, 0x28);
			U16(n, 0x17C) = 0x1000;
			U32(n, 0x20) = U32(b, 0x2C);
			pb_895E40(n, n + 0x1C);
			const uint32_t burst = pb_893900(n, game_addr(ORIG_Burst), game_addr(game::unk_17F1744), 0xD8, 2, 0);
			const uint32_t w1c = U32(n, 0x1C);
			const uint32_t w20 = U32(n, 0x20);
			U32(burst, 0x1C) = w1c;
			U32(burst, 0x20) = w20;
			x::Effect_AddTaskAndInitFromCtx(game_addr(Q_DEBRIS), game_addr(ORIG_Spawner), 0xB0, n);
			U8(n, 0x29) = (uint8_t)(U8(n, 0x29) + 1);
			return pb_893A30(n);
		}
		for (int k = 0; k < 3; k++)
		{
			const int32_t s = S16(n, 0x16C + 2 * k);
			const int32_t e = S16(n, 0x174 + 2 * k);
			const int32_t p = mul32(e - s, (int32_t)t);
			U16(n, 0x1C + 2 * k) = (uint16_t)(p / 0x1000 + s);
		}
		return pb_893A30(n);
	}

	uint32_t __cdecl pb_896050(uint32_t a1)
	{
		const uint32_t n = a1;
		x::Effect_DecodeModelPrimLayout(U32(n, 0x74), n + 0x94, U32(n, 0x78));
		U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x48) = 1;
		U32(n, 0x58) = 0x1000;
		U32(n, 0x54) = 0x1000;
		U32(n, 0x50) = 0x1000;
		const uint32_t b = MEM<uint32_t>(game_addr(GEO_PTR));
		const uint32_t w34 = U32(b, 0x34);
		ff8fx::copy<(0x20)>((void *)(n + 0x30), (const void *)b);
		U32(n, 0x1C) = U32(b, 0x30);
		U32(n, 0x20) = w34;
		pb_893A30(n);
		U8(n, 0x29) = (uint8_t)(U8(n, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl pb_8960F0(uint32_t a1)
	{
		const uint32_t n = a1;
		const uint32_t sy = U32(n, 0x58);
		if (sy == 0x1000)
			U32(n, 0x58) = 0x800;
		else if (sy == 0x800)
			U32(n, 0x58) = 0x400;
		else if (sy == 0x400)
		{
			U8(n, 0x26) |= 1;
			U8(n, 0x29) = (uint8_t)(U8(n, 0x29) + 1);
		}
		pb_895E40(n, n + 0x1C);
		return pb_893A30(n);
	}

	uint32_t __cdecl pb_8961B0(uint32_t a1)
	{
		const uint32_t n = a1;
		if (S16(n, 0x24) < 0x23)
			return 0;
		x::Effect_DecodeModelPrimLayout(U32(n, 0x74), n + 0x94, U32(n, 0x78));
		U32(n, 0x58) = 0x1000;
		U32(n, 0x54) = 0x1000;
		U32(n, 0x50) = 0x1000;
		const uint32_t b = MEM<uint32_t>(game_addr(GEO_PTR));
		const uint32_t w2c = U32(b, 0x2C);
		ff8fx::copy<(0x20)>((void *)(n + 0x30), (const void *)b);
		U32(n, 0x1C) = U32(b, 0x28);
		U32(n, 0x20) = w2c;
		pb_893A30(n);
		U8(n, 0x29) = (uint8_t)(U8(n, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl pb_8962B0(uint32_t a1)
	{
		const uint32_t n = a1;
		if (S16(n, 0x24) < S16(n, 0x298))
			return 0;
		const uint32_t b = MEM<uint32_t>(game_addr(GEO_PTR));
		U32(n, 0x290) = U32(b, 0x20);
		U32(n, 0x294) = U32(b, 0x24);
		a_73F990(n);
		pb_896DE0(n);
		U8(n, 0x29) = (uint8_t)(U8(n, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl pb_899BD0(uint32_t a1)
	{
		const uint32_t n = a1;
		int32_t count = 0;
		const uint16_t tick = U16(n, 0x24);
		if (tick == 0)
			count = 8;
		else if (tick == 1)
		{
			U8(n, 0x26) |= 1;
			U8(n, 0x29) = (uint8_t)(U8(n, 0x29) + 1);
			count = 4;
		}
		if (count <= 0)
			return 0;
		alignas(4) uint8_t mbuf[0x20] = {};
		const uint32_t M = P(mbuf);
		do
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_DEBRIS), game_addr(ORIG_Debris), 0xB0, n);
			uint32_t v = t + 0x8C;
			for (int k = 4; k != 0; k--, v += 8)
			{
				const uint32_t b = MEM<uint32_t>(game_addr(GEO_PTR));
				U32(v, -0x20) = U32(b, 0x30);
				U32(v, -0x1C) = U32(b, 0x34);
				U16(v, 0) = 0;
				const uint32_t r = x::CrtRand();
				U16(v, 2) = (uint16_t)((r & 0xFFF) + 0x400);
				U16(v, 4) = 0;
				ff8fx::copy<(0x20)>((void *)M, (const void *)MEM<uint32_t>(game_addr(GEO_PTR)));
				const uint32_t r2 = x::CrtRand() & 0xFFF;
				x::sub_8DD960(M, (uint32_t)(int32_t)(int16_t)r2);
				x::matrixMultiplyVector(M, v, v);
			}
		} while (--count != 0);
		return 0;
	}

	uint32_t __cdecl pb_899CC0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_023_PsychoBlast_Debris_Init);
		states[1] = game_addr(game::MAG_023_PsychoBlast_Debris_Animate);
		states[2] = game_addr(game::MAG_023_PsychoBlast_Debris_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		for (int k = 0; k < 4; k++)
		{
			const uint32_t v = node + 0x8C + 8 * k;
			const uint32_t p = node + 0x6C + 8 * k;
			for (int c = 0; c < 3; c++)
			{
				const int16_t s = S16(v, 2 * c);
				U16(v, 2 * c) = (uint16_t)(s - s / 8);
			}
			for (int c = 0; c < 3; c++)
				U16(p, 2 * c) = (uint16_t)(U16(p, 2 * c) + (uint16_t)(S16(v, 2 * c) / 16));
		}
		for (int k = 0; k < 4; k++)
		{
			const uint32_t p = node + 0x6C + 8 * k;
			U32(node, 0x1C) = U32(p, 0);
			U32(node, 0x20) = U32(p, 4);
			pb_899DD0(node);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_023_PsychoBlast_Master_State0_Advance, (void *)a_73A0D0, "023 MAG_023_sub_893300" },
		{ game::MAG_023_PsychoBlast_Master_State1_Advance, (void *)a_73A0D0, "023 MAG_023_sub_893310" },
		{ game::MAG_023_PsychoBlast_Master_State3_SpawnEmitter, (void *)a_73A170, "023 MAG_023_sub_8933A0" },
		{ game::MAG_023_PsychoBlast_EmitterTask, (void *)a_73A1A0, "023 MAG_023_sub_8933D0" },
		{ game::MAG_023_DirSyncGate, (void *)a_73B640, "023 sub_893600" },
		{ game::MAG_023_DirPhaseGE, (void *)a_73B7E0, "023 sub_893780" },
		{ game::MAG_023_PsychoBlast_Director_State3_WaitDirGate2, (void *)a_73B8A0, "023 sub_8937D0" },
		{ game::MAG_023_PsychoBlast_Director_State4_Finish, (void *)a_7475A0, "023 sub_8937F0" },
		{ game::MAG_023_PsychoBlast_Director_State5_Empty, (void *)a_73A6D0, "023 nullsub_1734" },
		{ game::MAG_023_St_Empty, (void *)a_73A6D0, "023 nullsub_1735" },
		{ game::MAG_023_SpawnChildTask, (void *)a_73C100, "023 sub_8938B0" },
		{ game::MAG_023_EmitG3, (void *)a_73D770, "023 sub_894DC0" },
		{ game::MAG_023_EmitG4, (void *)a_73D9C0, "023 sub_895010" },
		{ game::MAG_023_PsychoBlast_BurstTask, (void *)a_73A380, "023 sub_895FF0" },
		{ game::MAG_023_PsychoBlast_Burst_State2_Empty, (void *)a_73A6D0, "023 nullsub_1736" },
		{ game::MAG_023_PsychoBlast_Orb_State3_Empty, (void *)a_73A6D0, "023 nullsub_1737" },
		{ game::MAG_023_PsychoBlast_RingTask, (void *)a_73A380, "023 sub_896150" },
		{ game::MAG_023_PsychoBlast_Ring_State2_Empty, (void *)a_73A6D0, "023 nullsub_1738" },
		{ game::MAG_023_PsychoBlast_ActorSystemTask, (void *)a_73A380, "023 sub_896250" },
		{ game::MAG_023_DirectorSetup, (void *)a_73F990, "023 sub_896300" },
		{ game::MAG_023_Director_StartPositions, (void *)a_73FC20, "023 sub_896590" },
		{ game::MAG_023_Director_Mode3Setup, (void *)a_73FE90, "023 sub_896800" },
		{ game::MAG_023_Director_TargetPositions, (void *)a_73FFB0, "023 sub_896920" },
		{ game::MAG_023_RelocateActorData, (void *)a_740210, "023 sub_896B80" },
		{ game::MAG_023_ActorMotion, (void *)a_740910, "023 sub_897240" },
		{ game::MAG_023_DampValue, (void *)a_740F70, "023 sub_8978A0" },
		{ game::MAG_023_DampVector, (void *)a_740FC0, "023 sub_8978F0" },
		{ game::MAG_023_Instance_AdvanceTexFrame, (void *)a_741050, "023 sub_897980" },
		{ game::MAG_023_Instance_BuildTransform, (void *)a_741120, "023 sub_897A50" },
		{ game::MAG_023_Particle_LifeCountdown, (void *)a_741B60, "023 sub_898490" },
		{ game::MAG_023_RandRange, (void *)a_742290, "023 au_re__rand_62" },
		{ game::MAG_023_RandRangeWide, (void *)a_7422C0, "023 sub_898BF0" },
		{ game::MAG_023_RandVec3, (void *)a_742300, "023 sub_898C30" },
		{ game::MAG_023_JitterAngles3, (void *)a_742350, "023 sub_898C80" },
		{ game::MAG_023_RandAngleJitter, (void *)a_7423A0, "023 au_re__rand_62_0" },
		{ game::MAG_023_GetWord8Minus1, (void *)a_7424B0, "023 sub_898DE0" },
		{ game::MAG_023_Particle_AttachToOwner, (void *)a_7424C0, "023 sub_898DF0" },
		{ game::MAG_023_ParticleList_Append, (void *)a_742CA0, "023 sub_8995D0" },
		{ game::MAG_023_ParticleList_Unlink, (void *)a_742CD0, "023 sub_899600" },
		{ game::MAG_023_Particle_Init, (void *)a_742D00, "023 sub_899630" },
		{ game::MAG_023_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "023 sub_8997E0" },
		{ game::MAG_023_Particle_PlaceAtPoint, (void *)a_742FF0, "023 sub_899920" },
		{ game::MAG_023_PsychoBlast_ActorSystem_State2_Empty, (void *)a_73A6D0, "023 nullsub_1739" },
		{ game::InitEffectSequenceFromData_c14, (void *)a_7435E0, "023 InitEffectSequenceFromData_c14" },
		{ game::MAG_023_SpriteStrip_DrawQuads, (void *)a_743720, "023 sub_89A030" },
		{ game::MAG_023_StepCounterLimit, (void *)a_743C20, "023 sub_89A4E0" },
		{ game::MAG_023_PsychoBlast_Debris_State2_Empty, (void *)a_73A6D0, "023 nullsub_1740" },
		{ game::MAG_023_PsychoBlast_DebrisSpawner_State1_Empty, (void *)a_73A6D0, "023 nullsub_1741" },
		{ game::MAG_023_PsychoBlast_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "023 MAG_023_sub_89A570" },
		{ game::MAG_023_PsychoBlast_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "023 MAG_023_sub_89A590" },
		{ game::MAG_023_PsychoBlast_Emitter_State4_Empty, (void *)a_73A6D0, "023 nullsub_1742" },
		{ game::MAG_023_PsychoBlast_Master_State4_NextTargetLoop, (void *)a_747500, "023 MAG_023_sub_89A5C0" },
		{ game::MAG_023_PsychoBlast_Master_State5_Advance, (void *)a_73A0D0, "023 MAG_023_sub_89A600" },
		{ game::MAG_023_PsychoBlast_Master_State6_WaitQueuesEmpty, (void *)a_747550, "023 MAG_023_sub_89A610" },
		{ game::MAG_023_PsychoBlast_Master_State7_RequestBattleSync, (void *)a_747560, "023 MAG_023_sub_89A620" },
		{ game::MAG_023_PsychoBlast_Master_State8_WaitBattleSyncDone, (void *)a_747590, "023 MAG_023_sub_89A650" },
		{ game::MAG_023_PsychoBlast_Master_State9_Finish, (void *)a_7475A0, "023 MAG_023_sub_89A660" },
		{ game::MAG_023_PsychoBlast_Master_State10_Empty, (void *)a_73A6D0, "023 nullsub_1733" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_023_PsychoBlast_MasterTask, (void *)pb_893190, "023 pb_893190" },
		{ game::MAG_023_PsychoBlast_Master_CarveActorPools, (void *)pb_893320, "023 pb_893320" },
		{ game::MAG_023_PsychoBlast_ClearActorPools, (void *)pb_893360, "023 pb_893360" },
		{ game::MAG_023_PsychoBlast_Emitter_StartDirector, (void *)pb_893450, "023 pb_893450" },
		{ game::MAG_023_PsychoBlast_Emitter_InitDirectorBlock, (void *)pb_893480, "023 pb_893480" },
		{ game::MAG_023_PsychoBlast_DirectorTask, (void *)pb_893500, "023 pb_893500" },
		{ game::MAG_023_PsychoBlast_Director_PhaseBookkeeping, (void *)pb_893580, "023 pb_893580" },
		{ game::MAG_023_PsychoBlast_Director_Cue1_SetGeometry, (void *)pb_8935D0, "023 pb_8935D0" },
		{ game::MAG_023_PsychoBlast_SetupGeometryBlock, (void *)pb_893630, "023 pb_893630" },
		{ game::MAG_023_PsychoBlast_Director_State1_WaitDirPhase1, (void *)pb_893760, "023 pb_893760" },
		{ game::MAG_023_PsychoBlast_Director_PlaySound, (void *)pb_8937A0, "023 pb_8937A0" },
		{ game::MAG_023_PsychoBlast_Director_StartPhase, (void *)pb_893830, "023 pb_893830" },
		{ game::MAG_023_PsychoBlast_SpawnPrimModelTask, (void *)pb_893900, "023 pb_893900" },
		{ game::MAG_023_PsychoBlast_OrbTask, (void *)pb_893950, "023 pb_893950" },
		{ game::MAG_023_PsychoBlast_Orb_Appear, (void *)pb_8939B0, "023 pb_8939B0" },
		{ game::MAG_023_PsychoBlast_DrawPrimModel, (void *)pb_893A30, "023 pb_893A30" },
		{ game::MAG_023_PsychoBlast_PrimModelCallback, (void *)pb_893B20, "023 pb_893B20" },
		{ game::MAG_023_PsychoBlast_RenderPrimModel, (void *)pb_893DE0, "023 pb_893DE0" },
		{ game::MAG_023_PsychoBlast_FlatTriList, (void *)pb_893F10, "023 pb_893F10" },
		{ game::MAG_023_PsychoBlast_FlatQuadList, (void *)pb_894130, "023 pb_894130" },
		{ game::MAG_023_PsychoBlast_TexTriList, (void *)pb_8943C0, "023 pb_8943C0" },
		{ game::MAG_023_PsychoBlast_TexQuadList, (void *)pb_894840, "023 pb_894840" },
		{ game::MAG_023_PsychoBlast_GouraudTexTriList, (void *)pb_8952F0, "023 pb_8952F0" },
		{ game::MAG_023_PsychoBlast_GouraudTexQuadList, (void *)pb_8957F0, "023 pb_8957F0" },
		{ game::MAG_023_PsychoBlast_Orb_ComputeEndPoint, (void *)pb_895E40, "023 pb_895E40" },
		{ game::MAG_023_PsychoBlast_Orb_Flight, (void *)pb_895ED0, "023 pb_895ED0" },
		{ game::MAG_023_PsychoBlast_Burst_Start, (void *)pb_896050, "023 pb_896050" },
		{ game::MAG_023_PsychoBlast_Burst_State1_PlayModel, (void *)pb_8960C0, "023 pb_8960C0" },
		{ game::MAG_023_PsychoBlast_Orb_Flatten, (void *)pb_8960F0, "023 pb_8960F0" },
		{ game::MAG_023_PsychoBlast_Ring_Appear, (void *)pb_8961B0, "023 pb_8961B0" },
		{ game::MAG_023_PsychoBlast_Ring_State1_PlayModel, (void *)pb_896220, "023 pb_896220" },
		{ game::MAG_023_PsychoBlast_ActorSystem_Start, (void *)pb_8962B0, "023 pb_8962B0" },
		{ game::MAG_023_ResetTrailHistory, (void *)pb_8964B0, "023 pb_8964B0" },
		{ game::MAG_023_FillTrailRows, (void *)pb_896730, "023 pb_896730" },
		{ game::MAG_023_PsychoBlast_ActorSystem_Step, (void *)pb_896DE0, "023 pb_896DE0" },
		{ game::MAG_023_ActSys_DrawPass, (void *)pb_896E70, "023 pb_896E70" },
		{ game::MAG_023_ActSys_DrawPrimActor, (void *)pb_896ED0, "023 pb_896ED0" },
		{ game::MAG_023_ActSys_DrawSpritePass, (void *)pb_897030, "023 pb_897030" },
		{ game::MAG_023_ActSys_DrawActorSprite, (void *)pb_8970F0, "023 pb_8970F0" },
		{ game::MAG_023_ActSys_UpdatePass, (void *)pb_897170, "023 pb_897170" },
		{ game::MAG_023_ActorStep, (void *)pb_8971B0, "023 pb_8971B0" },
		{ game::MAG_023_MorphVertexBlock, (void *)pb_8981E0, "023 pb_8981E0" },
		{ game::MAG_023_Instance_EndOfLifeTest, (void *)pb_8982F0, "023 pb_8982F0" },
		{ game::MAG_023_Instance_SpawnFollowUp, (void *)pb_898370, "023 pb_898370" },
		{ game::MAG_023_AllocParticle, (void *)pb_8983A0, "023 pb_8983A0" },
		{ game::MAG_023_ActSys_UpdateEmitter, (void *)pb_898440, "023 pb_898440" },
		{ game::MAG_023_ActSys_EmitterSpawnStep, (void *)pb_8984C0, "023 pb_8984C0" },
		{ game::MAG_023_ActSys_SpawnParticles, (void *)pb_898540, "023 pb_898540" },
		{ game::MAG_023_ActSys_AllocParticleSlot, (void *)pb_898D20, "023 pb_898D20" },
		{ game::MAG_023_Actor_InterpolatePosition, (void *)pb_898F40, "023 pb_898F40" },
		{ game::MAG_023_SpawnSubEffects, (void *)pb_899A30, "023 pb_899A30" },
		{ game::MAG_023_Director_SpawnGroupEffects, (void *)pb_899AC0, "023 pb_899AC0" },
		{ game::MAG_023_PsychoBlast_ActorSystem_State1_StepActorSystem, (void *)pb_899B50, "023 pb_899B50" },
		{ game::MAG_023_PsychoBlast_DebrisSpawnerTask, (void *)pb_899B80, "023 pb_899B80" },
		{ game::MAG_023_PsychoBlast_DebrisSpawner_Spawn, (void *)pb_899BD0, "023 pb_899BD0" },
		{ game::MAG_023_PsychoBlast_DebrisTask, (void *)pb_899CC0, "023 pb_899CC0" },
		{ game::MAG_023_PsychoBlast_Debris_DrawSprite, (void *)pb_899DD0, "023 pb_899DD0" },
		{ game::MAG_023_PsychoBlast_Debris_Init, (void *)pb_89A4A0, "023 pb_89A4A0" },
		{ game::MAG_023_PsychoBlast_Debris_Animate, (void *)pb_89A4C0, "023 pb_89A4C0" },
		{ game::MAG_023_PsychoBlast_Emitter_ApplyDamage, (void *)pb_89A530, "023 pb_89A530" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag023_psycho_blast()
	{
		act::register_module(23);
		for (const act::psycho::ModPort *p = act::psycho::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(23, p->addr, p->port, p->name);
		for (const act::psycho::ModPort *p = act::psycho::PORTS; p->addr; p++)
			act::register_module_port(23, p->addr, p->port, p->name);
	}
}

