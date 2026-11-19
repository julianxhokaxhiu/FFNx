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
namespace angelo91
{
	static const Mod MOD_091 = { "angelorush", 91, game::MAG_091_ANGELO_RUSH_FL, game::MAG_090_TONBERRY_SUMMON_CHEFS_KNIFE_FL,
		{ {}, {}, game::unk_15430E0, {}, {}, game::unk_2599034, {}, {}, {}, {}, {}, {}, game::unk_2598F28, {}, {}, {}, game::unk_2599038, {}, {}, {}, {}, {}, {}, {}, game::unk_259D8F0, {}, game::unk_259DB90, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::unk_259EE90, {}, game::unk_259EEA0 },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_091_ANGELO_RUSH_ActSys_DrawModelActor, game::MAG_091_ANGELO_RUSH_ActSys_RenderPrimModel, game::MAG_091_ANGELO_RUSH_ModelList_FlatTri, game::MAG_091_ANGELO_RUSH_ModelList_FlatQuad, game::MAG_091_ANGELO_RUSH_ModelList_TexTri, game::MAG_091_ANGELO_RUSH_ModelList_TexQuad, game::MAG_091_ANGELO_RUSH_ModelList_GouraudTri, game::MAG_091_ANGELO_RUSH_ModelList_GouraudQuad, game::MAG_091_ANGELO_RUSH_ModelList_GouraudTexTri, game::MAG_091_ANGELO_RUSH_ModelList_GouraudTexQuad, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_091_ANGELO_RUSH_EmitterTask, game::MAG_091_ANGELO_RUSH_Emitter_State0_SpawnDirector, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_091_ANGELO_RUSH_DirSyncGate, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_091_ANGELO_RUSH_ResetTrailHistory, game::MAG_091_ANGELO_RUSH_Director_StartPositions, game::MAG_091_ANGELO_RUSH_FillTrailRows, game::MAG_091_ANGELO_RUSH_Director_Mode3Setup, game::MAG_091_ANGELO_RUSH_Director_TargetPositions, game::MAG_091_ANGELO_RUSH_RelocateActorData, {}, {}, game::MAG_091_ANGELO_RUSH_DampValue, game::MAG_091_ANGELO_RUSH_DampVector, {}, {}, game::MAG_091_ANGELO_RUSH_MorphVertexBlock, {}, {}, {}, game::MAG_091_RandRange, game::MAG_091_ANGELO_RUSH_RandRangeWide, game::MAG_091_RandAngleJitter, game::MAG_091_ANGELO_RUSH_Particle_AttachToOwner, game::MAG_091_ANGELO_RUSH_Actor_InterpolatePosition, {}, game::MAG_091_ANGELO_RUSH_ParticleList_Unlink, game::MAG_091_ANGELO_RUSH_Particle_PlaceAtAnchorSet, game::MAG_091_ANGELO_RUSH_Particle_PlaceAtPoint, game::MAG_091_ANGELO_RUSH_Sequence_DrawSpriteStrip, {}, game::MAG_091_ANGELO_RUSH_DamageState_ApplyActionResult, game::MAG_091_ANGELO_RUSH_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_091_ANGELO_RUSH_Emitter_State3_FinishReleaseRoot, game::MAG_091_ANGELO_RUSH_Director_State5_Nullsub_762240, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id DIR_PTR = game::unk_15430E0;
	static constexpr game::Id PHASE_DATA = DIR_PTR;
	static constexpr game::Id SPRITE_CURSOR = game::unk_2598F2C;
	static constexpr game::Id PACKET_CURSOR = SPRITE_CURSOR;
	static constexpr game::Id FA_CURSOR = game::BATTLE_PACKET_WRITE_CURSOR;
	static constexpr game::Id SCRATCH_SP = game::unk_259EE90;
	static constexpr game::Id ACTOR_STATE = game::unk_259EEA0;
	static constexpr game::Id BLEND_BUF = game::unk_259E8E8;
	static constexpr game::Id PRIM_CB = game::MAG_091_ANGELO_RUSH_DashPlay_ObjectCallback;
	static constexpr game::Id STAGE_LIGHT = game::unk_1D98992;
	static inline uint32_t ENT(uint32_t slot) { return game_addr(game::BattleEntitySlotData) + slot * 0x9C; }

	namespace gx
	{
		static inline uint32_t BdPlayStream(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::BdPlayStream))(a1, a2, a3, a4); }
		static inline uint32_t sub_45E0B0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadIR123_u16))(a1); }
		static inline uint32_t sub_45E9D0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPF))(); }
		static inline uint32_t sub_45EBF0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPL))(); }
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

	static inline uint32_t task_end(uint32_t node) { const uint8_t st = U8(node, 0x26); U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1); return task_end(node, st); }

	uint32_t __cdecl ru_759510(uint32_t a1);
	uint32_t __cdecl ru_7596C0(uint32_t a1);
	uint32_t __cdecl ru_759700();
	uint32_t __cdecl ru_7597F0(uint32_t a1);
	uint32_t __cdecl ru_759820(uint32_t a1);
	uint32_t __cdecl ru_7598C0(uint32_t a1);
	uint32_t __cdecl ru_759930(uint32_t a1);
	uint32_t __cdecl ru_759980(uint32_t a1);
	uint32_t __cdecl ru_7599F0(uint32_t a1);
	uint32_t __cdecl ru_759A30(uint32_t a1);
	uint32_t __cdecl ru_759A70(uint32_t a1);
	uint32_t __cdecl ru_759BA0(uint32_t a1);
	uint32_t __cdecl ru_759D80(uint32_t a1);
	uint32_t __cdecl ru_75A000(void);
	uint32_t __cdecl ru_75A6B0(uint32_t a1);
	uint32_t __cdecl ru_75AA20(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ru_75AC30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ru_75B390(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ru_75B5C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ru_75B880(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ru_75BAE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ru_75BDC0(void);
	uint32_t __cdecl ru_75BE80(uint32_t a1);
	uint32_t __cdecl ru_75BF00(void);
	uint32_t __cdecl ru_75BF40(uint32_t a1);
	uint32_t __cdecl ru_75CF70(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ru_75D080(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ru_75D100(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ru_75D130(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ru_75D1D0(uint32_t a1);
	uint32_t __cdecl ru_75D250(uint32_t a1);
	uint32_t __cdecl ru_75D2D0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ru_75DAB0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ru_75DCD0(uint32_t a1);
	uint32_t __cdecl ru_75E7C0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ru_75E850(void);
	uint32_t __cdecl ru_75E8E0(uint32_t a1);
	uint32_t __cdecl ru_75E910(uint32_t a1);
	uint32_t __cdecl ru_75EC80(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ru_75EEC0(uint32_t a1);
	uint32_t __cdecl ru_75EF20(uint32_t a1);
	uint32_t __cdecl ru_75F030(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ru_75F160(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ru_75F380(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ru_75F610(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ru_75FA90(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ru_760540(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ru_760A40(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ru_761090(uint32_t a1);
	uint32_t __cdecl ru_7610D0(uint32_t a1);
	uint32_t __cdecl ru_7611B0(uint32_t a1);
	uint32_t __cdecl ru_761210(uint32_t a1);
	uint32_t __cdecl ru_7618E0(uint32_t a1);
	uint32_t __cdecl ru_761900(uint32_t a1);
	uint32_t __cdecl ru_7619D0(uint32_t a1);
	uint32_t __cdecl ru_761A60(uint32_t a1);
	uint32_t __cdecl ru_761AB0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl ru_761B60(uint32_t a1);
	uint32_t __cdecl ru_761BB0(uint32_t a1);
	uint32_t __cdecl ru_761CE0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ru_761F10(uint32_t a1);
	uint32_t __cdecl ru_761F40(uint32_t a1);
	uint32_t __cdecl ru_761F90(uint32_t a1);
	uint32_t __cdecl ru_761FC0(uint32_t a1);
	uint32_t __cdecl ru_761FF0(uint32_t a1);
	uint32_t __cdecl ru_762030(uint32_t a1);
	uint32_t __cdecl ru_762080(uint32_t a1);
	uint32_t __cdecl ru_7620B0(uint32_t a1);
	uint32_t __cdecl ru_7620F0(uint32_t a1);
	uint32_t __cdecl ru_762180(uint32_t a1);
	uint32_t __cdecl ru_7621C0(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace angelo91
{
	uint32_t __cdecl ru_759510(uint32_t a1)
	{
		g_mod = &MOD_091;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::unk_259EE90)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_2599038)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_091_ANGELO_RUSH_Sequence_State0_Advance_7596A0);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_091_ANGELO_RUSH_Sequence_State0_Advance_7596B0);
		states[2] = game_addr(game::MAG_091_ANGELO_RUSH_Master_State2_CarvePools);
		states[3] = game_addr(game::MAG_091_ANGELO_RUSH_Sequence_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_091_ANGELO_RUSH_Sequence_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_091_ANGELO_RUSH_Sequence_State0_Advance_762290);
		states[6] = game_addr(game::MAG_091_ANGELO_RUSH_Sequence_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_091_ANGELO_RUSH_Sequence_State7_RequestBattleSync);
		states[8] = game_addr(game::MAG_091_ANGELO_RUSH_Sequence_State8_WaitBattleSyncDone);
		states[9] = game_addr(game::MAG_091_ANGELO_RUSH_Sequence_State9_Finish);
		states[10] = game_addr(game::MAG_091_ANGELO_RUSH_Director_State5_Nullsub_762310);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_259DD04));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_259D644));
			MEM<uint32_t>(game_addr(game::unk_2598F2C)) = v1;
			MEM<uint32_t>(game_addr(game::unk_2599034)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_259DD00));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_259D640));
			MEM<uint32_t>(game_addr(game::unk_2598F2C)) = v1;
			MEM<uint32_t>(game_addr(game::unk_2599034)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_259EE88)) = 0;
		MEM<uint16_t>(game_addr(game::unk_259E8D0)) = 0;
		static constexpr game::Id queues[8] = { game::unk_259DB90, game::unk_2599010, game::unk_259E8C0, game::unk_259D8F0, game::unk_2599118, game::unk_259BE30, game::unk_259D630, game::unk_259DB80 };
		for (int i = 0; i < 8; i++)
		{
			const uint32_t n = x::ExecuteTaskQueue(game_addr(queues[i]));
			U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)n);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ru_7596C0(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::unk_259EE9C));
		MEM<uint32_t>(game_addr(game::unk_259E8D4)) = p;
		p += 0x870;
		MEM<uint32_t>(game_addr(game::unk_259EE94)) = p;
		p += 0xB7C0;
		MEM<uint32_t>(game_addr(game::unk_259EE9C)) = p;
		ru_759700();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ru_759700()
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::unk_259E8D4)), 0x870);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::unk_259EE94)), 0xB7C0);
		MEM<uint16_t>(game_addr(game::unk_259BE28)) = 0;
		MEM<uint16_t>(game_addr(game::unk_259E8E0)) = 0;
		MEM<uint16_t>(game_addr(game::unk_259E708)) = 0;
		MEM<uint16_t>(game_addr(game::unk_2598F30)) = 0;
		return 0;
	}

	uint32_t __cdecl ru_7597F0(uint32_t a1)
	{
		ru_759820(a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2599010), game_addr(game::MAG_091_ANGELO_RUSH_DirectorTask), 0x48, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ru_759820(uint32_t a1)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(DIR_PTR)), 0x54);
		x::GetEffectSpawnPosition(ENT(U8(a1, 0x2D)), 0xF1, 0, MEM<uint32_t>(game_addr(DIR_PTR)) + 8);
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		if (S16(d, 0xA) > -0x300)
			U16(d, 0xA) = 0xFD00;
		U16(d, 0) = 0;
		U16(d, 2) = 0;
		U16(d, 4) = 0;
		const int16_t dx = (int16_t)(U16(d, 8) - U16(d, 0));
		const uint32_t ang = x::CartesianToGameAngle((uint32_t)(int32_t)dx, (uint32_t)(int32_t)S16(d, 0xC));
		const uint32_t r = (ang - 0x800) & 0xFFF;
		U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x4A) = (uint16_t)r;
		return r;
	}

	uint32_t __cdecl ru_7598C0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[5];
		states[0] = game_addr(game::MAG_091_UploadAngeloTIM);
		states[1] = game_addr(game::MAG_091_ANGELO_RUSH_Director_State1_WaitGate1);
		states[2] = game_addr(game::MAG_091_ANGELO_RUSH_Director_State2_WaitCue1);
		states[3] = game_addr(game::MAG_091_ANGELO_RUSH_Director_State4_FinishAtPhase2);
		states[4] = game_addr(game::MAG_091_ANGELO_RUSH_Director_State5_Nullsub_759A50);
		ru_759930(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ru_759930(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			ru_759A70(a1);
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

	uint32_t __cdecl ru_759980(uint32_t a1)
	{
		x::Battle_QueueTIMUpload_GetEOF(game_addr(game::MAG091_Angelo_BodyTIM));
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl ru_7599F0(uint32_t a1)
	{
		if (a_73B7E0(1) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl ru_759A30(uint32_t a1)
	{
		if (a_73B7E0(2) != 0)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ru_759A70(uint32_t a1)
	{
		const int32_t phase = S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x40);
		if (phase == 1)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_259E8C0), game_addr(game::MAG_091_ANGELO_RUSH_AngeloTask), 0x158, a1);
			x::Effect_BindModelContainerSetAnim(t, game_addr(game::MAG091_Angelo_ModelContainer), 3);
		}
		else if (phase == 2)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_259DB80), game_addr(game::sub_8DDEC0), 0x40, a1);
			U32(t, 0x34) = game_addr(game::unk_1547144);
			U16(t, 0x38) = 0;
			a_73C100(a1, game_addr(game::MAG_091_ANGELO_RUSH_ActorSystemTask), game_addr(game::unk_16CB3FC), 0, 0xF, 0);
		}
		return 0;
	}

	uint32_t __cdecl ru_759BA0(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			a_73F990(a1);
			ru_75A6B0(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ru_759D80(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::unk_259EEA0));
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

	uint32_t __cdecl ru_75A000(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::unk_259EEA0));
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

	uint32_t __cdecl ru_75A6B0(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::unk_259EEA0), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			ru_75E850();
			ru_75BF00();
			ru_75BDC0();
			a_735440();
			sys = U32(game_addr(game::unk_259EEA0), 0);
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
		U16(game_addr(game::unk_259EE88), 0) = (uint16_t)(U16(game_addr(game::unk_259EE88), 0) + c14);
		U16(game_addr(game::unk_259E8D0), 0) = (uint16_t)(U16(game_addr(game::unk_259E8D0), 0) + c16);
		return done;
	}

	uint32_t __cdecl ru_75AA20(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ru_75AC30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ru_75B390(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ru_75B5C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ru_75B880(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ru_75BAE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ru_75BDC0(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::unk_259EEA0)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::unk_259EEA0));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						ru_75BE80(act);
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
							ru_75BE80(act);
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

	uint32_t __cdecl ru_75BE80(uint32_t a1)
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

	uint32_t __cdecl ru_75BF00(void)
	{
		uint32_t act = U32(U32(game_addr(game::unk_259EEA0), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				ru_75D1D0(act);
			else if (type == 1)
				ru_75BF40(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl ru_75BF40(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::unk_259EEA0));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (ru_75D080(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::unk_259EEA0));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl ru_75CF70(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::unk_259EEA0), 0);
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

	uint32_t __cdecl ru_75D080(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				ru_75D100(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			ru_75D100(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			ru_75D100(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl ru_75D100(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return ru_75D130(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl ru_75D130(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::unk_259E8D4));
		int32_t idx = MEM<int16_t>(game_addr(game::unk_259BE28));
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
			if (idx >= 0x13)
				idx = 0;
			tries++;
		} while (tries < 0x14);
		idx++;
		if (idx < 0x13)
			MEM<uint16_t>(game_addr(game::unk_259BE28)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::unk_259BE28)) = 0;
		return slot;
	}

	uint32_t __cdecl ru_75D1D0(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_091_ANGELO_RUSH_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_091_ANGELO_RUSH_Actor_InterpolatePosition), a1);
		ru_75D250(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl ru_75D250(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::unk_259EEA0), 0);
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
				ru_75D2D0(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			ru_75D2D0(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl ru_75D2D0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::unk_259EE90), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::unk_259EE90), 0) = sp;
		uint32_t node = ru_75DAB0(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::unk_259EEA0), 0);
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
		ru_75E7C0(node, desc);
		U32(game_addr(game::unk_259EE90), 0) = U32(game_addr(game::unk_259EE90), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl ru_75DAB0(uint32_t a1, uint32_t a2)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::unk_259EE94));
		int32_t i = MEM<int16_t>(game_addr(game::unk_259E8E0));
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
			MEM<uint16_t>(game_addr(game::unk_259E8E0)) = 0;
		else
			MEM<uint16_t>(game_addr(game::unk_259E8E0)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl ru_75DCD0(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::unk_259EEA0), 0);
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
				uint32_t g = U32(game_addr(game::unk_259EEA0), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::unk_259EEA0), 0);
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

	uint32_t __cdecl ru_75E7C0(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			ru_75D130(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			ru_75D130(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			ru_75D130(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			ru_75D130(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl ru_75E850(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::unk_259EEA0));
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
					ru_75D130(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::unk_259EEA0));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				ru_75D130(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::unk_259EEA0));
			}
		}
		return 0;
	}

	uint32_t __cdecl ru_75E8E0(uint32_t a1)
	{
		if (ru_75A6B0(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl ru_75E910(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[10];
		states[0] = game_addr(game::MAG_091_ANGELO_RUSH_Angelo_State0_Place);
		states[1] = game_addr(game::MAG_091_ANGELO_RUSH_Angelo_State1_Run_SpawnDash);
		states[2] = game_addr(game::MAG_091_ANGELO_RUSH_Angelo_State2_WaitFrame18);
		states[3] = game_addr(game::MAG_091_ANGELO_RUSH_Angelo_State3_WaitAnimEnd);
		states[4] = game_addr(game::MAG_091_ANGELO_RUSH_Angelo_State4_Jump);
		states[5] = game_addr(game::MAG_091_ANGELO_RUSH_Angelo_State5_Land);
		states[6] = game_addr(game::MAG_091_ANGELO_RUSH_Angelo_State6_SyncPoint2);
		states[7] = game_addr(game::MAG_091_ANGELO_RUSH_Angelo_State7_FallAtPhase2);
		states[8] = game_addr(game::MAG_091_ANGELO_RUSH_Angelo_State8_Vanish);
		states[9] = game_addr(game::MAG_091_ANGELO_RUSH_Angelo_State9_Nullsub);
		callp(states[S8(node, 0x29)], node);
		const uint16_t g = U16(node, 0x152);
		const uint16_t vz = U16(node, 0x128);
		U16(node, 0x126) = (uint16_t)(U16(node, 0x126) + g);
		U16(node, 0x4E) = (uint16_t)(U16(node, 0x4E) + U16(node, 0x126));
		const int16_t y = S16(node, 0x4E);
		U16(node, 0x50) = (uint16_t)(U16(node, 0x50) + vz);
		if (y > 0)
			U16(node, 0x4E) = 0;
		if ((U8(node, 0x26) & 4) == 0)
		{
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = a_746C10(node, game_addr(game::unk_259DD08), MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		}
		const uint8_t anim = U8(node, 0x9C), frame = U8(node, 0xA2);
		if (anim == 0)
		{
			if (frame == 1)
				ru_75EC80(node, 2);
			else if (frame == 2)
			{
				ru_75EC80(node, 1);
				ru_75EC80(node, 3);
			}
			else if (frame == 4)
				ru_75EC80(node, 0);
		}
		else if (anim == 1)
		{
			if (frame == 1)
			{
				ru_75EC80(node, 2);
				ru_75EC80(node, 1);
			}
			else if (frame == 2)
			{
				ru_75EC80(node, 3);
				ru_75EC80(node, 0);
			}
		}
		const int32_t pz = S16(node, 0x50);
		const int32_t py = S16(node, 0x4E);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::unk_1D99AB0), (const void *)(node + 0x70));
		const int32_t px = S16(node, 0x4C);
		S32(node, 0x8C) = pz;
		S32(node, 0x84) = px;
		S32(node, 0x88) = py;
		if ((U8(node, 0x26) & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl ru_75EC80(uint32_t a1, uint32_t a2)
	{
		static const uint32_t PTS[4] = { 0x1E, 0x21, 0x1C, 0x1D };
		const uint32_t mdl = a1 + 0x30;
		const int32_t k = (int32_t)(int16_t)a2;
		alignas(4) uint8_t m[0x20];
		const uint32_t M = P(m);
		for (int i = 0; i < 4; i++)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2599118), game_addr(game::MAG_091_ANGELO_RUSH_RingParticleTask), 0x90, a1);
			if ((uint32_t)k <= 3)
				x::GetEffectSpawnPosition(mdl, PTS[k], 0, t + 0x1C);
			const int32_t r1 = (int32_t)x::CrtRand();
			U16(t, 0x1C) = (uint16_t)(U16(t, 0x1C) + (uint16_t)((r1 & 0xFF) - 0x80));
			const int32_t r2 = (int32_t)x::CrtRand();
			U16(t, 0x1E) = 0;
			U16(t, 0x20) = (uint16_t)(U16(t, 0x20) + (uint16_t)((r2 & 0xFF) - 0x80));
			x::MAG_022_sub_8DD770(M);
			const int32_t r3 = (int32_t)x::CrtRand();
			x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)(int16_t)(((r3 & 0x3FF) - 0x200) & 0xFFF));
			const int32_t r4 = (int32_t)x::CrtRand();
			U16(t, 0x78) = 0;
			U16(t, 0x7A) = 0;
			U16(t, 0x82) = (uint16_t)(-(r4 % 0x60));
			const int32_t r5 = (int32_t)x::CrtRand();
			U16(t, 0x7C) = (uint16_t)((r5 & 0xFFF) + 0x200);
			x::matrixMultiplyVector(M, t + 0x78, t + 0x78);
		}
		for (int i = 0; i < 2; i++)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_259BE30), game_addr(game::MAG_091_ANGELO_RUSH_DustPuffTask), 0x8C, a1);
			if ((uint32_t)k <= 3)
				x::GetEffectSpawnPosition(mdl, PTS[k], 0, t + 0x1C);
			U16(t, 0x1E) = 0;
			x::MAG_022_sub_8DD770(M);
			const int32_t r1 = (int32_t)x::CrtRand();
			x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)(int16_t)(((r1 & 0x1FF) - 0x100) & 0xFFF));
			const int32_t r2 = (int32_t)x::CrtRand();
			x::sub_8DD7E0(M, (uint32_t)(int32_t)(int16_t)(((r2 % 0x180) + 0x100) & 0xFFF));
			U16(t, 0x76) = 0;
			U16(t, 0x74) = 0;
			const int32_t r3 = (int32_t)x::CrtRand();
			U16(t, 0x78) = (uint16_t)((r3 & 0x7FF) + 0x200);
			x::matrixMultiplyVector(M, t + 0x74, t + 0x74);
		}
		return 0;
	}

	uint32_t __cdecl ru_75EEC0(uint32_t a1)
	{
		const uint32_t states[3] = { game_addr(game::MAG_091_ANGELO_RUSH_DustPuff_State0_Setup), game_addr(game::MAG_091_ANGELO_RUSH_DustPuff_State1_DriftShrink), game_addr(game::MAG_091_ANGELO_RUSH_DustPuff_State2_Nullsub) };
		callp(states[S8(a1, 0x29)], a1);
		ru_75EF20(a1);
		return task_end(a1);
	}

	uint32_t __cdecl ru_75EF20(uint32_t a1)
	{
		if (U8(a1, 0x26) & 4)
			return 0;
		uint8_t m[0x20];
		const uint32_t M = P(m);
		x::UnpackRotationMatrix(game_addr(game::unk_1D97778), M);
		x::sub_8DD960(M, (uint32_t)(int32_t)S16(a1, 0x48));
		S32(M, 0x14) = S16(a1, 0x1C);
		S32(M, 0x18) = S16(a1, 0x1E);
		S32(M, 0x1C) = S16(a1, 0x20);
		x::scale3DMatrix(M, a1 + 0x30);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), M, M);
		x::GTE_SetRotMatrix_W(M);
		x::GTE_SetTransVector_W(M);
		const uint32_t h = x::Field_Alloc(0x68);
		U32(h, 0) = U32(a1, 0x4C);
		U32(h, 8) = U32(a1, 0x40);
		S32(h, 0xC) = S16(a1, 0x50);
		U16(h, 0x22) = 0x100;
		U16(h, 0x20) = 0x100;
		U16(h, 0x2A) = 0x100;
		U16(h, 0x28) = 0x100;
		S32(h, 0x10) = S16(a1, 0x52);
		U16(h, 0x18) = 0;
		U16(h, 0x1A) = 0;
		U16(h, 0x1E) = 0;
		U16(h, 0x1C) = 0;
		U16(h, 0x26) = 0;
		U16(h, 0x24) = 0;
		U32(h, 0x14) = 0xF0;
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = ru_75F030(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl ru_75F030(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
			cur = ru_75F160(ctx, a2, a3, a4);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = ru_75F380(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = ru_75F610(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = ru_75FA90(ctx, a2, a3, cur);
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
			cur = ru_760540(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			return ru_760A40(ctx, a2, a3, cur);
		U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		return cur;
	}

	uint32_t __cdecl ru_75F160(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ru_75F380(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ru_75F610(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ru_75FA90(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ru_760540(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ru_760A40(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ru_761090(uint32_t a1)
	{
		U32(a1, 0x4C) = game_addr(game::unk_154322C);
		S32(a1, 0x38) = 0x1000;
		S32(a1, 0x34) = 0x1000;
		S32(a1, 0x30) = 0x1000;
		U16(a1, 0x48) = (uint16_t)(x::CrtRand() & 0xFFF);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ru_7610D0(uint32_t a1)
	{
		const uint32_t c = a1;
		U16(c, 0x48) = (uint16_t)(U16(c, 0x48) + 0x40);
		if (S16(c, 0x1E) >= 0)
			U16(c, 0x1E) = 0;
		if (S16(c, 0x24) >= 0xA)
		{
			const int32_t s = S32(c, 0x38) - 0x400;
			S32(c, 0x38) = s;
			if (s <= 0)
			{
				U8(c, 0x26) = (uint8_t)(U8(c, 0x26) | 1);
				U8(c, 0x29) = (uint8_t)(U8(c, 0x29) + 1);
				S32(c, 0x38) = 0x10;
			}
			S32(c, 0x34) = S32(c, 0x38);
			S32(c, 0x30) = S32(c, 0x38);
		}
		int16_t vx = S16(c, 0x74);
		int16_t vy = (int16_t)(S16(c, 0x76) + 0x80);
		int16_t vz = S16(c, 0x78);
		vx = (int16_t)(vx - vx / 16);
		U16(c, 0x74) = (uint16_t)vx;
		vy = (int16_t)(vy - vy / 16);
		U16(c, 0x76) = (uint16_t)vy;
		vz = (int16_t)(vz - vz / 16);
		U16(c, 0x78) = (uint16_t)vz;
		U16(c, 0x1C) = (uint16_t)(U16(c, 0x1C) + (uint16_t)(vx / 16));
		U16(c, 0x1E) = (uint16_t)(U16(c, 0x1E) + (uint16_t)(vy / 16));
		U16(c, 0x20) = (uint16_t)(U16(c, 0x20) + (uint16_t)(vz / 16));
		return 0;
	}

	uint32_t __cdecl ru_7611B0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_091_ANGELO_RUSH_RingParticle_State0_Setup);
		states[1] = game_addr(game::MAG_091_ANGELO_RUSH_RingParticle_State1_Rise);
		states[2] = game_addr(game::MAG_091_ANGELO_RUSH_RingParticle_State2_Nullsub);
		callp(states[S8(node, 0x29)], node);
		ru_761210(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ru_761210(uint32_t a1)
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
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::unk_2598F2C));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(blk, 0x24) = 0;
		U16(blk, 0xB4) = colour;
		MEM<uint32_t>(game_addr(game::unk_2598F2C)) = a_7435E0(blk, ot, 2, cursor);
		x::Field_Free(0xD8);
		return 0;
	}

	uint32_t __cdecl ru_7618E0(uint32_t a1)
	{
		U32(a1, 0x4C) = game_addr(game::unk_15430E8);
		U16(a1, 0x56) = 0xFFC0;
		U16(a1, 0x52) = 7;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ru_761900(uint32_t a1)
	{
		const uint32_t n = a1;
		if (a_743C20(n) != 0)
		{
			U8(n, 0x26) = (uint8_t)(U8(n, 0x26) | 5);
			U8(n, 0x29) = (uint8_t)(U8(n, 0x29) + 1);
		}
		U16(n, 0x7A) = (uint16_t)(U16(n, 0x7A) + U16(n, 0x82));
		int16_t vx = S16(n, 0x78);
		int16_t vz = S16(n, 0x7C);
		const int16_t vy = S16(n, 0x7A);
		vx = (int16_t)(vx - vx / 8);
		U16(n, 0x78) = (uint16_t)vx;
		vz = (int16_t)(vz - vz / 8);
		U16(n, 0x7C) = (uint16_t)vz;
		U16(n, 0x1C) = (uint16_t)(U16(n, 0x1C) + (uint16_t)(vx / 16));
		U16(n, 0x1E) = (uint16_t)(U16(n, 0x1E) + (uint16_t)(vy / 16));
		U16(n, 0x20) = (uint16_t)(U16(n, 0x20) + (uint16_t)(vz / 16));
		return 0;
	}

	uint32_t __cdecl ru_7619D0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		const uint32_t p8 = U32(d, 8), pc = U32(d, 0xC);
		U32(node, 0x4C) = p8;
		U32(node, 0x50) = pc;
		U16(node, 0x4E) = 0;
		U16(node, 0x128) = 0xFF00;
		const int32_t h = S16(ENT(U8(node, 0x2D)), 0x26);
		const int32_t dz = (mul32(h, 125) * 16 >> 12) + 0x2400;
		U32(node, 0x11C) = 0x1000;
		U16(node, 0x50) = (uint16_t)(U16(node, 0x50) + (uint16_t)dz);
		U32(node, 0x118) = 0x1000;
		U32(node, 0x114) = 0x1000;
		x::au_re_Battle_ReadAnimation_7(node, 0);
		x::BdPlaySE(game_addr(game::unk_15430E4), 0, 0x80);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ru_761A60(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		if (S16(node, 0x136) >= 0xC)
		{
			const uint32_t t = ru_761AB0(node, game_addr(game::MAG_091_ANGELO_RUSH_DashPlayTask), game_addr(game::unk_16CC2EC), 0x70, 0, 0);
			U32(t, 0x104) = node + 0x30;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ru_761AB0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_259D630), a2, 0x108, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		S32(t, 0x78) = (int32_t)(int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl ru_761B60(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		S32(node, 0x58) = 0x1000;
		S32(node, 0x54) = 0x1000;
		S32(node, 0x50) = 0x1000;
		U16(node, 0x86) = 0xFF00;
		ru_761F10(node);
		ru_761BB0(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ru_761BB0(uint32_t a1)
	{
		alignas(4) uint8_t blk[0x5C] = {};
		const uint32_t L = P(blk);
		const uint32_t node = a1;
		x::MAG_022_sub_8DD770(L);
		int16_t ang = S16(node, 0x62);
		if (ang != 0)
			x::MAG_022_sub_8DD8A0(L, (uint32_t)(int32_t)ang);
		ang = S16(node, 0x60);
		if (ang != 0)
			x::sub_8DD7E0(L, (uint32_t)(int32_t)ang);
		ang = S16(node, 0x64);
		if (ang != 0)
			x::sub_8DD960(L, (uint32_t)(int32_t)ang);
		x::scale3DMatrix(L, node + 0x50);
		S32(L, 0x14) = S16(node, 0x1C);
		S32(L, 0x18) = S16(node, 0x1E);
		S32(L, 0x1C) = S16(node, 0x20);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), L, L);
		U16(L, 0x54) = U16(node, 0x86);
		U32(L, 0x44) = U32(node, 0x7C);
		U32(L, 0x38) = U32(node, 0x70);
		U16(L, 0x4C) = U16(node, 0x8C);
		U16(L, 0x52) = U16(node, 0x92);
		U16(L, 0x4E) = U16(node, 0x8E);
		U16(L, 0x50) = U16(node, 0x90);
		U32(L, 0x48) = game_addr(BLEND_BUF);
		U16(L, 0x56) = U16(node, 0x80);
		U16(L, 0x58) = U16(node, 0x88);
		return prim_play(node + 0x94, game_addr(PRIM_CB), L, 0);
	}

	uint32_t __cdecl ru_761CE0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		alignas(4) uint8_t loc[0x38] = {};
		const uint32_t L = P(loc);
		const uint32_t V = L;
		const uint32_t S = L + 8;
		const uint32_t M = L + 0x18;
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
		U16(V, 0) = U16(rec, 8);
		U16(V, 2) = U16(rec, 0xA);
		U16(V, 4) = U16(rec, 0xC);
		x::GTE_SetRotMatrix(blk);
		x::GTE_LoadV0(V);
		x::GTE_MVMVA_RotV0();
		x::GTE_ReadMAC123(M + 0x14);
		x::GTE_MatrixMultiply(blk, M);
		S32(M, 0x14) = add32(S32(M, 0x14), S32(blk, 0x14));
		S32(M, 0x18) = add32(S32(M, 0x18), S32(blk, 0x18));
		S32(M, 0x1C) = add32(S32(M, 0x1C), S32(blk, 0x1C));
		if (U32(rec, 0x18) != 0x10001000 || U16(rec, 0x1C) != 0x1000)
		{
			S32(S, 0) = S16(rec, 0x18);
			S32(S, 4) = S16(rec, 0x1A);
			S32(S, 8) = S16(rec, 0x1C);
			x::scale3DMatrix(M, S);
		}
		x::GTE_SetRotMatrix_W(M);
		x::GTE_SetTransVector_W(M);
		U32(hdr, 0x14) = 0x2030;
		const int32_t alpha = S16(rec, 0x24);
		U32(hdr, 0xC) = (uint32_t)alpha;
		if (alpha != 0)
		{
			U32(hdr, 0x14) = 0x20F0;
			U32(hdr, 8) = U32(rec, 0x20);
		}
		const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		U16(hdr, 0x22) = 0x100;
		U16(hdr, 0x20) = 0x100;
		U16(hdr, 0x2A) = 0x100;
		U16(hdr, 0x28) = 0x100;
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U32(hdr, 0x10) = (uint32_t)(int32_t)S16(blk, 0x58);
		U16(hdr, 0x18) = 0;
		U16(hdr, 0x1A) = 0;
		U16(hdr, 0x1E) = 0;
		U16(hdr, 0x1C) = 0;
		U16(hdr, 0x26) = 0;
		U16(hdr, 0x24) = 0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = ru_75F030(hdr, ot, 2, cursor);
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl ru_761F10(uint32_t a1)
	{
		x::GetEffectSpawnPosition(U32(a1, 0x104), 0xF1, 0, a1 + 0x1C);
		U16(a1, 0x20) = (uint16_t)(U16(a1, 0x20) + 0xFF00);
		return 0;
	}

	uint32_t __cdecl ru_761F40(uint32_t a1)
	{
		ru_761F10(a1);
		if (a_73B7E0(2) != 0 || ru_761BB0(a1) == 0)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ru_761F90(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		if (S16(node, 0x136) >= 0x18)
		{
			x::au_re_Battle_ReadAnimation_7(node, 1);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ru_761FC0(uint32_t a1)
	{
		uint32_t r = x::au_re_Battle_ReadAnimation_8(a1);
		if (r == 1)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 2);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ru_761FF0(uint32_t a1)
	{
		const uint32_t node = a1;
		ru_762030(node);
		if (x::au_re_Battle_ReadAnimation_8(node) == 1)
		{
			U16(node, 0x50) = (uint16_t)(U16(node, 0x50) + 0xFE00);
			x::au_re_Battle_ReadAnimation_7(node, 3);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ru_762030(uint32_t a1)
	{
		const uint32_t n = a1;
		const uint16_t i = U16(n, 0x154);
		const uint16_t f = MEM<uint16_t>(game_addr(game::unk_1547134) + (uint32_t)((int32_t)(int16_t)i * 2));
		U16(n, 0x150) = f;
		if (f != 0x1000)
			U16(n, 0x154) = (uint16_t)(i + 1);
		const int32_t v = mul32(S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0xA), S16(n, 0x150));
		U16(n, 0x4E) = (uint16_t)(v / 4096);
		return 0;
	}

	uint32_t __cdecl ru_762080(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		ru_762030(node);
		if (S16(node, 0x136) >= 0)
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ru_7620B0(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		ru_762030(node);
		if (a_73B640(2) != 0)
		{
			U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x48) = 1;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ru_7620F0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t e = ENT(U8(node, 0x2D));
		x::sub_8DD1C0(node);
		ru_762030(node);
		if (a_73B7E0(2) != 0)
		{
			const int32_t h = S16(e, 0x26);
			U16(node, 0x152) = 8;
			U16(node, 0x134) = 0xF;
			const int32_t dz = mul32(h, 125) * 16 >> 12;
			U16(node, 0x50) = (uint16_t)(U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0xC) - (uint16_t)dz);
			const int32_t vz = S16(node, 0x128);
			U16(node, 0x128) = (uint16_t)(vz / 2);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ru_762180(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		U16(node, 0x134) = (uint16_t)(U16(node, 0x134) - 1);
		if (S16(node, 0x134) <= 0)
		{
			U8(node, 0x26) = (uint8_t)(U8(node, 0x26) | 5);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ru_7621C0(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_15430E0));
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
		{ game::MAG_091_ANGELO_RUSH_Sequence_State0_Advance_7596A0, (void *)a_73A0D0, "091 MAG_091_sub_7596A0" },
		{ game::MAG_091_ANGELO_RUSH_Sequence_State0_Advance_7596B0, (void *)a_73A0D0, "091 MAG_091_sub_7596B0" },
		{ game::MAG_091_ANGELO_RUSH_Sequence_State3_SpawnEmitter, (void *)a_73A170, "091 MAG_091_sub_759740" },
		{ game::MAG_091_ANGELO_RUSH_EmitterTask, (void *)a_73A1A0, "091 MAG_091_sub_759770" },
		{ game::MAG_091_ANGELO_RUSH_Director_State1_WaitGate1, (void *)a_73B790, "091 sub_7599A0" },
		{ game::MAG_091_ANGELO_RUSH_DirSyncGate, (void *)a_73B640, "091 sub_7599C0" },
		{ game::MAG_091_ANGELO_RUSH_DirPhaseGE, (void *)a_73B7E0, "091 sub_759A10" },
		{ game::MAG_091_ANGELO_RUSH_Director_State5_Nullsub_759A50, (void *)a_73A6D0, "091 nullsub_1237" },
		{ game::MAG_091_ANGELO_RUSH_Director_State5_Nullsub_759A60, (void *)a_73A6D0, "091 nullsub_1238" },
		{ game::MAG_091_ANGELO_RUSH_SpawnActorSystemTask, (void *)a_73C100, "091 sub_759AF0" },
		{ game::MAG_091_ANGELO_RUSH_ActorSystemTask, (void *)a_73A380, "091 sub_759B40" },
		{ game::MAG_091_ANGELO_RUSH_DirectorSetup, (void *)a_73F990, "091 sub_759BD0" },
		{ game::MAG_091_ANGELO_RUSH_Director_StartPositions, (void *)a_73FC20, "091 sub_759E60" },
		{ game::MAG_091_ANGELO_RUSH_Director_Mode3Setup, (void *)a_73FE90, "091 sub_75A0D0" },
		{ game::MAG_091_ANGELO_RUSH_Director_TargetPositions, (void *)a_73FFB0, "091 sub_75A1F0" },
		{ game::MAG_091_ANGELO_RUSH_RelocateActorData, (void *)a_740210, "091 sub_75A450" },
		{ game::MAG_091_ANGELO_RUSH_ActSys_DrawModelPass, (void *)a_735440, "091 sub_75A740" },
		{ game::MAG_091_ANGELO_RUSH_ActSys_DrawModelActor, (void *)a_7354A0, "091 sub_75A7A0" },
		{ game::MAG_091_ANGELO_RUSH_ActSys_RenderPrimModel, (void *)a_7355E0, "091 sub_75A8E0" },
		{ game::MAG_091_ANGELO_RUSH_ModelList_TexTri, (void *)a_735BB0, "091 sub_75AEB0" },
		{ game::MAG_091_ANGELO_RUSH_ModelList_TexQuad, (void *)a_735DF0, "091 sub_75B0F0" },
		{ game::MAG_091_ANGELO_RUSH_ActorMotion, (void *)a_740910, "091 sub_75BFD0" },
		{ game::MAG_091_ANGELO_RUSH_DampValue, (void *)a_740F70, "091 sub_75C630" },
		{ game::MAG_091_ANGELO_RUSH_DampVector, (void *)a_740FC0, "091 sub_75C680" },
		{ game::MAG_091_ANGELO_RUSH_Instance_AdvanceTexFrame, (void *)a_741050, "091 sub_75C710" },
		{ game::MAG_091_ANGELO_RUSH_InstanceBuildTransform, (void *)a_741120, "091 sub_75C7E0" },
		{ game::MAG_091_ANGELO_RUSH_Particle_LifeCountdown, (void *)a_741B60, "091 sub_75D220" },
		{ game::MAG_091_RandRange, (void *)a_742290, "091 MAG_091_RandRange" },
		{ game::MAG_091_ANGELO_RUSH_RandRangeWide, (void *)a_7422C0, "091 sub_75D980" },
		{ game::MAG_091_ANGELO_RUSH_RandVec3, (void *)a_742300, "091 sub_75D9C0" },
		{ game::MAG_091_ANGELO_RUSH_JitterAngles3, (void *)a_742350, "091 sub_75DA10" },
		{ game::MAG_091_RandAngleJitter, (void *)a_7423A0, "091 MAG_091_RandAngleJitter" },
		{ game::MAG_091_ANGELO_RUSH_GetWord8Minus1, (void *)a_7424B0, "091 sub_75DB70" },
		{ game::MAG_091_ANGELO_RUSH_Particle_AttachToOwner, (void *)a_7424C0, "091 sub_75DB80" },
		{ game::MAG_091_ANGELO_RUSH_ParticleList_Append, (void *)a_742CA0, "091 sub_75E360" },
		{ game::MAG_091_ANGELO_RUSH_ParticleList_Unlink, (void *)a_742CD0, "091 sub_75E390" },
		{ game::MAG_091_ANGELO_RUSH_Particle_Init, (void *)a_742D00, "091 sub_75E3C0" },
		{ game::MAG_091_ANGELO_RUSH_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "091 sub_75E570" },
		{ game::MAG_091_ANGELO_RUSH_Particle_PlaceAtPoint, (void *)a_742FF0, "091 sub_75E6B0" },
		{ game::MAG_091_ANGELO_RUSH_ActorSystem_State2_Nullsub, (void *)a_73A6D0, "091 nullsub_1239" },
		{ game::MAG_091_ANGELO_RUSH_Angelo_DrawModel, (void *)a_746C10, "091 sub_75EAA0" },
		{ game::MAG_091_ANGELO_RUSH_EmitG3, (void *)a_73D770, "091 sub_760010" },
		{ game::MAG_091_ANGELO_RUSH_EmitG4, (void *)a_73D9C0, "091 sub_760260" },
		{ game::MAG_091_ANGELO_RUSH_DustPuff_State2_Nullsub, (void *)a_73A6D0, "091 nullsub_1240" },
		{ game::InitEffectSequenceFromData_c4, (void *)a_7435E0, "091 InitEffectSequenceFromData_c4" },
		{ game::MAG_091_ANGELO_RUSH_Sequence_DrawSpriteStrip, (void *)a_743720, "091 sub_761470" },
		{ game::MAG_091_ANGELO_RUSH_Sequence_StepFrame, (void *)a_743C20, "091 sub_761990" },
		{ game::MAG_091_ANGELO_RUSH_RingParticle_State2_Nullsub, (void *)a_73A6D0, "091 nullsub_1241" },
		{ game::MAG_091_ANGELO_RUSH_DashPlayTask, (void *)a_73A380, "091 sub_761B00" },
		{ game::MAG_091_ANGELO_RUSH_DashPlay_State2_Nullsub, (void *)a_73A6D0, "091 nullsub_1245" },
		{ game::MAG_091_ANGELO_RUSH_Angelo_State9_Nullsub, (void *)a_73A6D0, "091 nullsub_1242" },
		{ game::MAG_091_ANGELO_RUSH_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "091 MAG_091_sub_762200" },
		{ game::MAG_091_ANGELO_RUSH_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "091 MAG_091_sub_762220" },
		{ game::MAG_091_ANGELO_RUSH_Director_State5_Nullsub_762240, (void *)a_73A6D0, "091 nullsub_1243" },
		{ game::MAG_091_ANGELO_RUSH_Sequence_State4_NextTargetLoop, (void *)a_747500, "091 MAG_091_sub_762250" },
		{ game::MAG_091_ANGELO_RUSH_Sequence_State0_Advance_762290, (void *)a_73A0D0, "091 MAG_091_sub_762290" },
		{ game::MAG_091_ANGELO_RUSH_Sequence_State6_WaitQueuesEmpty, (void *)a_747550, "091 MAG_091_sub_7622A0" },
		{ game::MAG_091_ANGELO_RUSH_Sequence_State7_RequestBattleSync, (void *)a_747560, "091 MAG_091_sub_7622B0" },
		{ game::MAG_091_ANGELO_RUSH_Sequence_State8_WaitBattleSyncDone, (void *)a_747590, "091 MAG_091_sub_7622E0" },
		{ game::MAG_091_ANGELO_RUSH_Sequence_State9_Finish, (void *)a_7475A0, "091 MAG_091_sub_7622F0" },
		{ game::MAG_091_ANGELO_RUSH_Director_State5_Nullsub_762310, (void *)a_73A6D0, "091 nullsub_1236" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_091_ANGELO_RUSH_MasterTask, (void *)ru_759510, "091 ru_759510" },
		{ game::MAG_091_ANGELO_RUSH_Master_State2_CarvePools, (void *)ru_7596C0, "091 ru_7596C0" },
		{ game::MAG_091_ANGELO_RUSH_ClearPools, (void *)ru_759700, "091 ru_759700" },
		{ game::MAG_091_ANGELO_RUSH_Emitter_State0_SpawnDirector, (void *)ru_7597F0, "091 ru_7597F0" },
		{ game::MAG_091_ANGELO_RUSH_DirectorDataSetup, (void *)ru_759820, "091 ru_759820" },
		{ game::MAG_091_ANGELO_RUSH_DirectorTask, (void *)ru_7598C0, "091 ru_7598C0" },
		{ game::MAG_091_ANGELO_RUSH_Director_PhaseBookkeeping, (void *)ru_759930, "091 ru_759930" },
		{ game::MAG_091_UploadAngeloTIM, (void *)ru_759980, "091 ru_759980" },
		{ game::MAG_091_ANGELO_RUSH_Director_State2_WaitCue1, (void *)ru_7599F0, "091 ru_7599F0" },
		{ game::MAG_091_ANGELO_RUSH_Director_State4_FinishAtPhase2, (void *)ru_759A30, "091 ru_759A30" },
		{ game::MAG_091_ANGELO_RUSH_Director_SpawnPhaseTasks, (void *)ru_759A70, "091 ru_759A70" },
		{ game::MAG_091_ANGELO_RUSH_ActorSystem_State0_StartAtFrame, (void *)ru_759BA0, "091 ru_759BA0" },
		{ game::MAG_091_ANGELO_RUSH_ResetTrailHistory, (void *)ru_759D80, "091 ru_759D80" },
		{ game::MAG_091_ANGELO_RUSH_FillTrailRows, (void *)ru_75A000, "091 ru_75A000" },
		{ game::MAG_091_ANGELO_RUSH_ActSys_Step, (void *)ru_75A6B0, "091 ru_75A6B0" },
		{ game::MAG_091_ANGELO_RUSH_ModelList_FlatTri, (void *)ru_75AA20, "091 ru_75AA20" },
		{ game::MAG_091_ANGELO_RUSH_ModelList_FlatQuad, (void *)ru_75AC30, "091 ru_75AC30" },
		{ game::MAG_091_ANGELO_RUSH_ModelList_GouraudTri, (void *)ru_75B390, "091 ru_75B390" },
		{ game::MAG_091_ANGELO_RUSH_ModelList_GouraudQuad, (void *)ru_75B5C0, "091 ru_75B5C0" },
		{ game::MAG_091_ANGELO_RUSH_ModelList_GouraudTexTri, (void *)ru_75B880, "091 ru_75B880" },
		{ game::MAG_091_ANGELO_RUSH_ModelList_GouraudTexQuad, (void *)ru_75BAE0, "091 ru_75BAE0" },
		{ game::MAG_091_ANGELO_RUSH_ActSys_DrawSpritePass, (void *)ru_75BDC0, "091 ru_75BDC0" },
		{ game::MAG_091_ANGELO_RUSH_ActSys_DrawActorSprite, (void *)ru_75BE80, "091 ru_75BE80" },
		{ game::MAG_091_ANGELO_RUSH_ActSys_UpdatePass, (void *)ru_75BF00, "091 ru_75BF00" },
		{ game::MAG_091_ANGELO_RUSH_ActorStep, (void *)ru_75BF40, "091 ru_75BF40" },
		{ game::MAG_091_ANGELO_RUSH_MorphVertexBlock, (void *)ru_75CF70, "091 ru_75CF70" },
		{ game::MAG_091_ANGELO_RUSH_Instance_EndOfLifeTest, (void *)ru_75D080, "091 ru_75D080" },
		{ game::MAG_091_ANGELO_RUSH_Instance_SpawnFollowUp, (void *)ru_75D100, "091 ru_75D100" },
		{ game::MAG_091_ANGELO_RUSH_AllocParticle, (void *)ru_75D130, "091 ru_75D130" },
		{ game::MAG_091_ANGELO_RUSH_ActSys_UpdateEmitter, (void *)ru_75D1D0, "091 ru_75D1D0" },
		{ game::MAG_091_ANGELO_RUSH_ActSys_EmitterSpawnStep, (void *)ru_75D250, "091 ru_75D250" },
		{ game::MAG_091_ANGELO_RUSH_ActSys_SpawnParticles, (void *)ru_75D2D0, "091 ru_75D2D0" },
		{ game::MAG_091_ANGELO_RUSH_ActSys_AllocParticleSlot, (void *)ru_75DAB0, "091 ru_75DAB0" },
		{ game::MAG_091_ANGELO_RUSH_Actor_InterpolatePosition, (void *)ru_75DCD0, "091 ru_75DCD0" },
		{ game::MAG_091_ANGELO_RUSH_SpawnSubEffects, (void *)ru_75E7C0, "091 ru_75E7C0" },
		{ game::MAG_091_ANGELO_RUSH_Director_SpawnGroupEffects, (void *)ru_75E850, "091 ru_75E850" },
		{ game::MAG_091_ANGELO_RUSH_ActorSystem_State1_StepUntilDone, (void *)ru_75E8E0, "091 ru_75E8E0" },
		{ game::MAG_091_ANGELO_RUSH_AngeloTask, (void *)ru_75E910, "091 ru_75E910" },
		{ game::MAG_091_ANGELO_RUSH_Angelo_SpawnDustBurst, (void *)ru_75EC80, "091 ru_75EC80" },
		{ game::MAG_091_ANGELO_RUSH_DustPuffTask, (void *)ru_75EEC0, "091 ru_75EEC0" },
		{ game::MAG_091_ANGELO_RUSH_DustPuff_DrawModel, (void *)ru_75EF20, "091 ru_75EF20" },
		{ game::MAG_091_ANGELO_RUSH_RenderPrimModel, (void *)ru_75F030, "091 ru_75F030" },
		{ game::MAG_091_ANGELO_RUSH_PrimList_FlatTri, (void *)ru_75F160, "091 ru_75F160" },
		{ game::MAG_091_ANGELO_RUSH_PrimList_FlatQuad, (void *)ru_75F380, "091 ru_75F380" },
		{ game::MAG_091_ANGELO_RUSH_PrimList_TexTri, (void *)ru_75F610, "091 ru_75F610" },
		{ game::MAG_091_ANGELO_RUSH_PrimList_TexQuad, (void *)ru_75FA90, "091 ru_75FA90" },
		{ game::MAG_091_ANGELO_RUSH_PrimList_GouraudTexTriScroll, (void *)ru_760540, "091 ru_760540" },
		{ game::MAG_091_ANGELO_RUSH_PrimList_GT4, (void *)ru_760A40, "091 ru_760A40" },
		{ game::MAG_091_ANGELO_RUSH_DustPuff_State0_Setup, (void *)ru_761090, "091 ru_761090" },
		{ game::MAG_091_ANGELO_RUSH_DustPuff_State1_DriftShrink, (void *)ru_7610D0, "091 ru_7610D0" },
		{ game::MAG_091_ANGELO_RUSH_RingParticleTask, (void *)ru_7611B0, "091 ru_7611B0" },
		{ game::MAG_091_ANGELO_RUSH_RingParticle_Draw, (void *)ru_761210, "091 ru_761210" },
		{ game::MAG_091_ANGELO_RUSH_RingParticle_State0_Setup, (void *)ru_7618E0, "091 ru_7618E0" },
		{ game::MAG_091_ANGELO_RUSH_RingParticle_State1_Rise, (void *)ru_761900, "091 ru_761900" },
		{ game::MAG_091_ANGELO_RUSH_Angelo_State0_Place, (void *)ru_7619D0, "091 ru_7619D0" },
		{ game::MAG_091_ANGELO_RUSH_Angelo_State1_Run_SpawnDash, (void *)ru_761A60, "091 ru_761A60" },
		{ game::MAG_091_ANGELO_RUSH_SpawnDashPlay, (void *)ru_761AB0, "091 ru_761AB0" },
		{ game::MAG_091_ANGELO_RUSH_DashPlay_State0_Setup, (void *)ru_761B60, "091 ru_761B60" },
		{ game::MAG_091_ANGELO_RUSH_DashPlay_PlayStep, (void *)ru_761BB0, "091 ru_761BB0" },
		{ game::MAG_091_ANGELO_RUSH_DashPlay_ObjectCallback, (void *)ru_761CE0, "091 ru_761CE0" },
		{ game::MAG_091_ANGELO_RUSH_DashPlay_FollowAngeloBone, (void *)ru_761F10, "091 ru_761F10" },
		{ game::MAG_091_ANGELO_RUSH_DashPlay_State1_Follow, (void *)ru_761F40, "091 ru_761F40" },
		{ game::MAG_091_ANGELO_RUSH_Angelo_State2_WaitFrame18, (void *)ru_761F90, "091 ru_761F90" },
		{ game::MAG_091_ANGELO_RUSH_Angelo_State3_WaitAnimEnd, (void *)ru_761FC0, "091 ru_761FC0" },
		{ game::MAG_091_ANGELO_RUSH_Angelo_State4_Jump, (void *)ru_761FF0, "091 ru_761FF0" },
		{ game::MAG_091_ANGELO_RUSH_Angelo_JumpHeight, (void *)ru_762030, "091 ru_762030" },
		{ game::MAG_091_ANGELO_RUSH_Angelo_State5_Land, (void *)ru_762080, "091 ru_762080" },
		{ game::MAG_091_ANGELO_RUSH_Angelo_State6_SyncPoint2, (void *)ru_7620B0, "091 ru_7620B0" },
		{ game::MAG_091_ANGELO_RUSH_Angelo_State7_FallAtPhase2, (void *)ru_7620F0, "091 ru_7620F0" },
		{ game::MAG_091_ANGELO_RUSH_Angelo_State8_Vanish, (void *)ru_762180, "091 ru_762180" },
		{ game::MAG_091_ANGELO_RUSH_DamageState_ApplyActionResult, (void *)ru_7621C0, "091 ru_7621C0" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag091_angelo_rush()
	{
		act::register_module(91);
		for (const act::angelo91::ModPort *p = act::angelo91::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(91, p->addr, p->port, p->name);
		for (const act::angelo91::ModPort *p = act::angelo91::PORTS; p->addr; p++)
			act::register_module_port(91, p->addr, p->port, p->name);
	}
}

