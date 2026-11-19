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
namespace angelo92
{
	static const Mod MOD_092 = { "angelosearch", 92, game::MAG_092_ANGELO_SEARCH_FL, game::MAG_091_ANGELO_RUSH_FL,
		{ {}, {}, game::unk_153D1D8, {}, {}, game::unk_2594E9C, {}, {}, {}, {}, {}, {}, game::unk_2594D98, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::unk_25981A8, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_092_ANGELO_SEARCH_EmitterTask, game::MAG_092_ANGELO_SEARCH_Emitter_State0_SpawnDirector, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_092_ANGELO_SEARCH_DirSyncGate, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_092_ANGELO_SEARCH_Sequence_DrawSpriteStrip, {}, game::MAG_092_ANGELO_SEARCH_DamageState_ApplyActionResult, game::MAG_092_ANGELO_SEARCH_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_092_ANGELO_SEARCH_Emitter_State3_FinishReleaseRoot, game::MAG_092_ANGELO_SEARCH_Director_State5_Nullsub_759270, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id DIR_PTR = game::unk_153D1D8;
	static constexpr game::Id PHASE_DATA = DIR_PTR;
	static constexpr game::Id PACKET_CURSOR = game::unk_2594D9C;
	static inline uint32_t ENT(uint32_t slot) { return game_addr(game::BattleEntitySlotData) + slot * 0x9C; }

	namespace gx
	{
		static inline uint32_t BdPlayStream(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::BdPlayStream))(a1, a2, a3, a4); }
		static inline uint32_t BindEntityModelFromComData(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(a1, a2, a3); }
		static inline uint32_t QueueChainTransformation(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::QueueChainTransformation))(a1, a2); }
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

	uint32_t __cdecl as_7552B0(uint32_t a1);
	uint32_t __cdecl as_755440(uint32_t a1);
	uint32_t __cdecl as_755500(uint32_t a1);
	uint32_t __cdecl as_755530(uint32_t a1);
	uint32_t __cdecl as_7555B0(uint32_t a1);
	uint32_t __cdecl as_755620(uint32_t a1);
	uint32_t __cdecl as_755670(uint32_t a1);
	uint32_t __cdecl as_7556E0(uint32_t a1);
	uint32_t __cdecl as_755720(uint32_t a1);
	uint32_t __cdecl as_755770(uint32_t a1);
	uint32_t __cdecl as_7557B0(uint32_t a1);
	uint32_t __cdecl as_755A80(uint32_t a1);
	uint32_t __cdecl as_755B70(uint32_t a1);
	uint32_t __cdecl as_755CD0(uint32_t a1);
	uint32_t __cdecl as_755CE0(uint32_t a1);
	uint32_t __cdecl as_755EE0(uint32_t a1);
	uint32_t __cdecl as_755FC0(uint32_t a1);
	uint32_t __cdecl as_7560D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl as_756200(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl as_756420(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl as_7566B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl as_756B30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl as_7575E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl as_757AE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl as_758130(uint32_t a1);
	uint32_t __cdecl as_758170(uint32_t a1);
	uint32_t __cdecl as_7581D0(uint32_t a1);
	uint32_t __cdecl as_7582A0(uint32_t a1);
	uint32_t __cdecl as_758970(uint32_t a1);
	uint32_t __cdecl as_758990(uint32_t a1);
	uint32_t __cdecl as_758A00(uint32_t a1);
	uint32_t __cdecl as_758AA0(uint32_t a1);
	uint32_t __cdecl as_758BB0(uint32_t a1);
	uint32_t __cdecl as_758C60(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl as_758D10(uint32_t a1);
	uint32_t __cdecl as_758D70(uint32_t a1);
	uint32_t __cdecl as_758EA0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl as_7590E0(uint32_t a1);
	uint32_t __cdecl as_759110(uint32_t a1);
	uint32_t __cdecl as_7591F0(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace angelo92
{
	uint32_t __cdecl as_7552B0(uint32_t a1)
	{
		g_mod = &MOD_092;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::unk_2598EF0)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_2594EA0)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_092_ANGELO_SEARCH_Sequence_State0_Advance_755420);
		states[1] = game_addr(game::MAG_092_ANGELO_SEARCH_Sequence_State0_Advance_755430);
		states[2] = game_addr(game::MAG_092_ANGELO_SEARCH_Master_State2_WaitChildren);
		states[3] = game_addr(game::MAG_092_ANGELO_SEARCH_Sequence_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_092_ANGELO_SEARCH_Sequence_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_092_ANGELO_SEARCH_Sequence_State0_Advance_7592C0);
		states[6] = game_addr(game::MAG_092_ANGELO_SEARCH_Sequence_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_092_ANGELO_SEARCH_Sequence_State7_RequestBattleSync);
		states[8] = game_addr(game::MAG_092_ANGELO_SEARCH_Sequence_State8_WaitBattleSyncDone);
		states[9] = game_addr(game::MAG_092_ANGELO_SEARCH_Sequence_State9_Finish);
		states[10] = game_addr(game::MAG_092_ANGELO_SEARCH_Director_State5_Nullsub_759340);
		if ((U8(node, 0x5C) & 1) != 0)
		{
			MEM<uint32_t>(game_addr(game::unk_2594D9C)) = MEM<uint32_t>(game_addr(game::unk_259831C));
			MEM<uint32_t>(game_addr(game::unk_2594E9C)) = MEM<uint32_t>(game_addr(game::unk_2597F14));
		}
		else
		{
			MEM<uint32_t>(game_addr(game::unk_2594D9C)) = MEM<uint32_t>(game_addr(game::unk_2598318));
			MEM<uint32_t>(game_addr(game::unk_2594E9C)) = MEM<uint32_t>(game_addr(game::unk_2597F10));
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		static constexpr game::Id queues[7] = { game::unk_25981A8, game::unk_2594E78, game::unk_2598ED0, game::unk_2594F80, game::unk_2596618, game::unk_2597F00, game::unk_2598198 };
		for (int i = 0; i < 7; i++)
		{
			const uint32_t n = x::ExecuteTaskQueue(game_addr(queues[i]));
			U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)n);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl as_755440(uint32_t a1)
	{
		if (U8(a1, 0x28) == 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl as_755500(uint32_t a1)
	{
		as_755530(a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2594E78), game_addr(game::MAG_092_ANGELO_SEARCH_DirectorTask), 0x48, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl as_755530(uint32_t a1)
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

	uint32_t __cdecl as_7555B0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[5];
		states[0] = game_addr(game::MAG_092_ANGELO_SEARCH_Director_State0_UploadTIM);
		states[1] = game_addr(game::MAG_092_ANGELO_SEARCH_Director_State1_WaitGate1);
		states[2] = game_addr(game::MAG_092_ANGELO_SEARCH_Director_State2_WaitCue1);
		states[3] = game_addr(game::MAG_092_ANGELO_SEARCH_Director_State3_FinishAtFrame9B);
		states[4] = game_addr(game::MAG_092_ANGELO_SEARCH_Director_State5_Nullsub_755750);
		as_755620(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl as_755620(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			as_755770(a1);
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

	uint32_t __cdecl as_755670(uint32_t a1)
	{
		x::Battle_QueueTIMUpload_GetEOF(game_addr(game::MAG092_Angelo_BodyTIM));
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl as_7556E0(uint32_t a1)
	{
		if (a_73B7E0(1) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl as_755720(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		if (S16(d, 0x46) >= 0x9B)
		{
			U16(d, 0x48) = 1;
			U16(a1, 0x26) = (uint16_t)(U16(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl as_755770(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x40) == 1)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2598ED0), game_addr(game::MAG_092_ANGELO_SEARCH_AngeloTask), 0x154, a1);
			x::Effect_BindModelContainerSetAnim(t, game_addr(game::MAG092_Angelo_ModelContainer), 3);
		}
		return 0;
	}

	uint32_t __cdecl as_7557B0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[7];
		states[0] = game_addr(game::MAG_092_ANGELO_SEARCH_Angelo_State0_BindRinoaAnimSet);
		states[1] = game_addr(game::MAG_092_ANGELO_SEARCH_Angelo_State1_WalkIn_SpawnSniff);
		states[2] = game_addr(game::MAG_092_ANGELO_SEARCH_Angelo_State2_TurnToRinoa);
		states[3] = game_addr(game::MAG_092_ANGELO_SEARCH_Angelo_State3_WalkToRinoa);
		states[4] = game_addr(game::MAG_092_ANGELO_SEARCH_Angelo_State4_ItemPlay_WalkOff);
		states[5] = game_addr(game::MAG_092_ANGELO_SEARCH_Angelo_State5_WalkOff_Hide);
		states[6] = game_addr(game::MAG_092_ANGELO_SEARCH_Angelo_State6_Nullsub);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x50) = (uint16_t)(U16(node, 0x50) + U16(node, 0x128));
		U16(node, 0x4C) = (uint16_t)(U16(node, 0x4C) + U16(node, 0x124));
		U16(node, 0x4E) = (uint16_t)(U16(node, 0x4E) + U16(node, 0x126));
		if ((U8(node, 0x26) & 4) == 0)
		{
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = a_746C10(node, game_addr(game::unk_2598320), MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		}
		const int32_t py = S16(node, 0x4E);
		const int32_t px = S16(node, 0x4C);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::unk_1D99AB0), (const void *)(node + 0x70));
		const int32_t pz = S16(node, 0x50);
		S32(node, 0x88) = py;
		S32(node, 0x84) = px;
		S32(node, 0x8C) = pz;
		if ((U8(node, 0x26) & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl as_755A80(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t e = ENT(U8(node, 0x2D));
		gx::BindEntityModelFromComData(e, game_addr(game::unk_2598EF8), game_addr(game::MAG_092_ANGELO_SEARCH_AnimSet));
		gx::QueueChainTransformation(e, 1);
		U32(node, 0x148) = U32(e, 0x1C);
		U32(node, 0x14C) = U32(e, 0x20);
		U16(node, 0x148) = (uint16_t)(U16(node, 0x148) + 0xFE80);
		U16(node, 0x14C) = (uint16_t)(U16(node, 0x14C) + 0xFA00);
		U16(node, 0x14A) = 0;
		U32(node, 0x140) = U32(node, 0x148);
		U32(node, 0x144) = U32(node, 0x14C);
		U16(node, 0x144) = (uint16_t)(U16(node, 0x144) + 0x1000);
		U32(node, 0x4C) = U32(node, 0x140);
		U32(node, 0x50) = U32(node, 0x144);
		U32(node, 0x11C) = 0x1000;
		U32(node, 0x118) = 0x1000;
		U32(node, 0x114) = 0x1000;
		x::au_re_Battle_ReadAnimation_7(node, 0);
		x::BdPlaySE(game_addr(game::unk_153D1DC), 0, 0x80);
		gx::BdPlayStream(game_addr(game::unk_16CB3F4), 0x80, 1, 0x7F);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static inline void as_walk(uint32_t node, int32_t t)
	{
		for (uint32_t k = 0; k < 6; k += 2)
		{
			const int16_t a = S16(node, 0x140 + k);
			U16(node, 0x4C + k) = (uint16_t)((int32_t)(S16(node, 0x148 + k) - a) * t / 4096 + a);
		}
	}

	uint32_t __cdecl as_755B70(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t mdl = node + 0x30;
		x::sub_8DD1C0(node);
		U16(node, 0x150) = (uint16_t)(U16(node, 0x150) + 0x100);
		const int16_t t = S16(node, 0x150);
		if (t >= 0x1000)
		{
			U32(mdl, 0x1C) = U32(node, 0x148);
			U16(node, 0x150) = 0x1000;
			U32(mdl, 0x20) = U32(node, 0x14C);
			x::au_re_Battle_ReadAnimation_7(node, 1);
			const uint32_t s = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2594F80), game_addr(game::MAG_092_ANGELO_SEARCH_SniffTask), 0x90, node);
			U32(s, 0x1C) = U32(mdl, 0x1C);
			U32(s, 0x20) = U32(mdl, 0x20);
			U32(s, 0x88) = mdl;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			return 0;
		}
		as_walk(node, t);
		return 0;
	}

	uint32_t __cdecl as_755CD0(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0x25)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl as_755CE0(uint32_t a1)
	{
		const uint32_t mdl = U32(a1, 0x88);
		alignas(4) uint8_t m[0x20];
		const uint32_t M = P(m);
		for (int i = 0; i < 2; i++)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2594F80), game_addr(game::MAG_092_ANGELO_SEARCH_RingParticleTask), 0x90, a1);
			x::GetEffectSpawnPosition(mdl, i == 0 ? 0x1E : 0x21, 0, t + 0x1C);
			const int32_t r1 = (int32_t)x::CrtRand();
			U16(t, 0x1C) = (uint16_t)(U16(t, 0x1C) + (uint16_t)((r1 & 0x7F) - 0x40));
			const int32_t r2 = (int32_t)x::CrtRand();
			U16(t, 0x20) = (uint16_t)(U16(t, 0x20) + (uint16_t)((r2 & 0x7F) - 0x40));
			x::MAG_022_sub_8DD770(M);
			const int32_t r3 = (int32_t)x::CrtRand();
			x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)(int16_t)(((r3 & 0x1FF) - 0x100) & 0xFFF));
			const int32_t r4 = (int32_t)x::CrtRand();
			U16(t, 0x78) = 0;
			U16(t, 0x7A) = 0;
			U16(t, 0x82) = (uint16_t)(-(r4 % 0x60));
			const int32_t r5 = (int32_t)x::CrtRand();
			U16(t, 0x7C) = (uint16_t)(r5 % 0xA00 + 0x200);
			x::matrixMultiplyVector(M, t + 0x78, t + 0x78);
		}
		for (int i = 0; i < 2; i++)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2596618), game_addr(game::MAG_092_ANGELO_SEARCH_DustPuffTask), 0x8C, a1);
			x::GetEffectSpawnPosition(mdl, i == 0 ? 0x1E : 0x21, 0, t + 0x1C);
			const int32_t r1 = (int32_t)x::CrtRand();
			U16(t, 0x1C) = (uint16_t)(U16(t, 0x1C) + (uint16_t)((r1 & 0x7F) - 0x40));
			const int32_t r2 = (int32_t)x::CrtRand();
			U16(t, 0x20) = (uint16_t)(U16(t, 0x20) + (uint16_t)((r2 & 0x7F) - 0x40));
			x::MAG_022_sub_8DD770(M);
			const int32_t r3 = (int32_t)x::CrtRand();
			x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)(int16_t)(((r3 & 0x1FF) - 0x100) & 0xFFF));
			const int32_t r4 = (int32_t)x::CrtRand();
			x::sub_8DD7E0(M, (uint32_t)(int32_t)(int16_t)(((r4 % 0x180) + 0x100) & 0xFFF));
			U16(t, 0x76) = 0;
			U16(t, 0x74) = 0;
			const int32_t r5 = (int32_t)x::CrtRand();
			U16(t, 0x78) = (uint16_t)(r5 % 0x800 + 0x200);
			x::matrixMultiplyVector(M, t + 0x74, t + 0x74);
		}
		if (S16(a1, 0x24) >= 0x32)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl as_755EE0(uint32_t a1)
	{
		const uint32_t c = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_092_ANGELO_SEARCH_DustPuff_State0_Setup);
		states[1] = game_addr(game::MAG_092_ANGELO_SEARCH_DustPuff_State1_RollShrink);
		states[2] = game_addr(game::MAG_092_ANGELO_SEARCH_DustPuff_State2_Nullsub);
		callp(states[S8(c, 0x29)], c);
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
		as_755FC0(c);
		U16(c, 0x24) = (uint16_t)(U16(c, 0x24) + 1);
		if ((U8(c, 0x26) & 1) != 0 && U8(c, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(c);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl as_755FC0(uint32_t a1)
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
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = as_7560D0(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl as_7560D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
			cur = as_756200(ctx, a2, a3, a4);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = as_756420(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = as_7566B0(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = as_756B30(ctx, a2, a3, cur);
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
			cur = as_7575E0(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			return as_757AE0(ctx, a2, a3, cur);
		U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		return cur;
	}

	uint32_t __cdecl as_756200(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl as_756420(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl as_7566B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl as_756B30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl as_7575E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl as_757AE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl as_758130(uint32_t a1)
	{
		U32(a1, 0x4C) = game_addr(game::unk_153D2C4);
		S32(a1, 0x38) = 0x1000;
		S32(a1, 0x34) = 0x1000;
		S32(a1, 0x30) = 0x1000;
		U16(a1, 0x48) = (uint16_t)(x::CrtRand() & 0xFFF);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl as_758170(uint32_t a1)
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
		return 0;
	}

	uint32_t __cdecl as_7581D0(uint32_t a1)
	{
		const uint32_t n = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_092_ANGELO_SEARCH_RingParticle_State0_Setup);
		states[1] = game_addr(game::MAG_092_ANGELO_SEARCH_RingParticle_State1_SequenceStep);
		states[2] = game_addr(game::MAG_092_ANGELO_SEARCH_RingParticle_State2_Nullsub);
		callp(states[S8(n, 0x29)], n);
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
		as_7582A0(n);
		U16(n, 0x24) = (uint16_t)(U16(n, 0x24) + 1);
		if ((U8(n, 0x26) & 1) != 0 && U8(n, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(n);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl as_7582A0(uint32_t a1)
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
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::unk_2594D9C));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(blk, 0x24) = 0;
		U16(blk, 0xB4) = colour;
		MEM<uint32_t>(game_addr(game::unk_2594D9C)) = a_7435E0(blk, ot, 2, cursor);
		x::Field_Free(0xD8);
		return 0;
	}

	uint32_t __cdecl as_758970(uint32_t a1)
	{
		U32(a1, 0x4C) = game_addr(game::unk_153D1E8);
		U16(a1, 0x56) = 0xFFC0;
		U16(a1, 0x52) = 7;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl as_758990(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl as_758A00(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t e = ENT(U8(node, 0x2D));
		if (S16(node, 0x136) == 6)
			x::BdPlaySE(game_addr(game::unk_153D1E0), 1, 0x80);
		if (x::au_re_Battle_ReadAnimation_8(node) == 1)
		{
			x::au_re_Battle_ReadAnimation_7(node, 2);
			U32(node, 0x140) = U32(node, 0x4C);
			U32(node, 0x144) = U32(node, 0x50);
			U32(node, 0x148) = U32(e, 0x1C);
			U32(node, 0x14C) = U32(e, 0x20);
			U16(node, 0x14C) = (uint16_t)(U16(node, 0x14C) + 0xFD80);
			U16(node, 0x3E) = 0xA0;
			U16(node, 0x150) = 0;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl as_758AA0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t e = ENT(U8(node, 0x2D));
		x::sub_8DD1C0(node);
		U16(node, 0x150) = (uint16_t)(U16(node, 0x150) + 0x2AB);
		const int16_t t = S16(node, 0x150);
		if (t >= 0x1000)
		{
			U16(node, 0x150) = 0x1000;
			U32(node, 0x4C) = U32(e, 0x1C);
			U32(node, 0x50) = U32(e, 0x20);
			gx::QueueChainTransformation(e, 2);
			x::au_re_Battle_ReadAnimation_7(node, 3);
			U16(node, 0x3E) = 0;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			return 0;
		}
		as_walk(node, t);
		return 0;
	}

	uint32_t __cdecl as_758BB0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (x::au_re_Battle_ReadAnimation_8(node) == 1)
		{
			x::au_re_Battle_ReadAnimation_7(node, 4);
			gx::BdPlayStream(game_addr(game::unk_16CB3F8), 0x80, 1, 0x7F);
			const uint32_t p = U32(node, 0x4C), q = U32(node, 0x50);
			U32(node, 0x140) = p;
			U32(node, 0x148) = p;
			U32(node, 0x14C) = q;
			U16(node, 0x14C) = (uint16_t)(U16(node, 0x14C) + 0x1000);
			U32(node, 0x144) = q;
			U16(node, 0x150) = 0;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		if (S16(node, 0x136) == 0xB)
		{
			const uint32_t t = as_758C60(node, game_addr(game::MAG_092_ANGELO_SEARCH_ItemPlayTask), game_addr(game::unk_16C0DB8), 0xE0, 0, 0);
			x::GetEffectSpawnPosition(node + 0x30, 0x18, 0, t + 0x1C);
			U16(t, 0x1E) = 0;
		}
		return 0;
	}

	uint32_t __cdecl as_758C60(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2597F00), a2, 0x17C, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		S32(t, 0x78) = (int32_t)(int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl as_758D10(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 8)
			return 0;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		S32(node, 0x58) = 0x1000;
		S32(node, 0x54) = 0x1000;
		S32(node, 0x50) = 0x1000;
		U16(node, 0x86) = 0xFF00;
		as_758D70(node);
		x::BdPlaySE(game_addr(game::unk_153D1E4), 1, 0x80);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl as_758D70(uint32_t a1)
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
		U32(L, 0x48) = MEM<uint32_t>(game_addr(game::unk_2598EE8));
		U32(L, 0x44) = U32(node, 0x7C);
		U16(L, 0x4C) = U16(node, 0x8C);
		U32(L, 0x38) = U32(node, 0x70);
		U16(L, 0x52) = U16(node, 0x92);
		U16(L, 0x4E) = U16(node, 0x8E);
		U16(L, 0x50) = U16(node, 0x90);
		U16(L, 0x56) = U16(node, 0x80);
		U16(L, 0x58) = U16(node, 0x88);
		return prim_play(node + 0x94, game_addr(game::MAG_092_ANGELO_SEARCH_ItemPlay_ObjectCallback), L, 0);
	}

	uint32_t __cdecl as_758EA0(uint32_t a1, uint32_t a2, uint32_t a3)
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
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = as_7560D0(hdr, ot, 2, cursor);
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl as_7590E0(uint32_t a1)
	{
		const uint32_t r = as_758D70(a1);
		if (r == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl as_759110(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		U16(node, 0x150) = (uint16_t)(U16(node, 0x150) + 0x100);
		const int16_t t = S16(node, 0x150);
		if (t >= 0x1000)
		{
			U32(node, 0x4C) = U32(node, 0x148);
			U8(node, 0x26) = (uint8_t)(U8(node, 0x26) | 5);
			U32(node, 0x50) = U32(node, 0x14C);
			U16(node, 0x150) = 0x1000;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			return 0;
		}
		as_walk(node, t);
		return 0;
	}

	uint32_t __cdecl as_7591F0(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_153D1D8));
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
		{ game::MAG_092_ANGELO_SEARCH_Sequence_State0_Advance_755420, (void *)a_73A0D0, "092 MAG_092_sub_755420" },
		{ game::MAG_092_ANGELO_SEARCH_Sequence_State0_Advance_755430, (void *)a_73A0D0, "092 MAG_092_sub_755430" },
		{ game::MAG_092_ANGELO_SEARCH_Sequence_State3_SpawnEmitter, (void *)a_73A170, "092 MAG_092_sub_755450" },
		{ game::MAG_092_ANGELO_SEARCH_EmitterTask, (void *)a_73A1A0, "092 MAG_092_sub_755480" },
		{ game::MAG_092_ANGELO_SEARCH_Director_State1_WaitGate1, (void *)a_73B790, "092 sub_755690" },
		{ game::MAG_092_ANGELO_SEARCH_DirSyncGate, (void *)a_73B640, "092 sub_7556B0" },
		{ game::MAG_092_ANGELO_SEARCH_DirPhaseGE, (void *)a_73B7E0, "092 sub_755700" },
		{ game::MAG_092_ANGELO_SEARCH_Director_State5_Nullsub_755750, (void *)a_73A6D0, "092 nullsub_1228" },
		{ game::MAG_092_ANGELO_SEARCH_Director_State5_Nullsub_755760, (void *)a_73A6D0, "092 nullsub_1229" },
		{ game::MAG_092_ANGELO_SEARCH_Angelo_DrawModel, (void *)a_746C10, "092 sub_7558A0" },
		{ game::MAG_092_ANGELO_SEARCH_SniffTask, (void *)a_73A380, "092 sub_755C70" },
		{ game::MAG_092_ANGELO_SEARCH_EmitG3, (void *)a_73D770, "092 sub_7570B0" },
		{ game::MAG_092_ANGELO_SEARCH_EmitG4, (void *)a_73D9C0, "092 sub_757300" },
		{ game::MAG_092_ANGELO_SEARCH_DustPuff_State2_Nullsub, (void *)a_73A6D0, "092 nullsub_1230" },
		{ game::InitEffectSequenceFromData_c3, (void *)a_7435E0, "092 InitEffectSequenceFromData_c3" },
		{ game::MAG_092_ANGELO_SEARCH_Sequence_DrawSpriteStrip, (void *)a_743720, "092 sub_758500" },
		{ game::MAG_092_ANGELO_SEARCH_Sequence_StepFrame, (void *)a_743C20, "092 sub_7589B0" },
		{ game::MAG_092_ANGELO_SEARCH_RingParticle_State2_Nullsub, (void *)a_73A6D0, "092 nullsub_1231" },
		{ game::MAG_092_ANGELO_SEARCH_Sniff_State2_Nullsub, (void *)a_73A6D0, "092 nullsub_1232" },
		{ game::MAG_092_ANGELO_SEARCH_ItemPlayTask, (void *)a_73A380, "092 sub_758CB0" },
		{ game::MAG_092_ANGELO_SEARCH_ItemPlay_State2_Nullsub, (void *)a_73A6D0, "092 nullsub_1233" },
		{ game::MAG_092_ANGELO_SEARCH_Angelo_State6_Nullsub, (void *)a_73A6D0, "092 nullsub_1234" },
		{ game::MAG_092_ANGELO_SEARCH_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "092 MAG_092_sub_759230" },
		{ game::MAG_092_ANGELO_SEARCH_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "092 MAG_092_sub_759250" },
		{ game::MAG_092_ANGELO_SEARCH_Director_State5_Nullsub_759270, (void *)a_73A6D0, "092 nullsub_1235" },
		{ game::MAG_092_ANGELO_SEARCH_Sequence_State4_NextTargetLoop, (void *)a_747500, "092 MAG_092_sub_759280" },
		{ game::MAG_092_ANGELO_SEARCH_Sequence_State0_Advance_7592C0, (void *)a_73A0D0, "092 MAG_092_sub_7592C0" },
		{ game::MAG_092_ANGELO_SEARCH_Sequence_State6_WaitQueuesEmpty, (void *)a_747550, "092 MAG_092_sub_7592D0" },
		{ game::MAG_092_ANGELO_SEARCH_Sequence_State7_RequestBattleSync, (void *)a_747560, "092 MAG_092_sub_7592E0" },
		{ game::MAG_092_ANGELO_SEARCH_Sequence_State8_WaitBattleSyncDone, (void *)a_747590, "092 MAG_092_sub_759310" },
		{ game::MAG_092_ANGELO_SEARCH_Sequence_State9_Finish, (void *)a_7475A0, "092 MAG_092_sub_759320" },
		{ game::MAG_092_ANGELO_SEARCH_Director_State5_Nullsub_759340, (void *)a_73A6D0, "092 nullsub_1227" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_092_ANGELO_SEARCH_MasterTask, (void *)as_7552B0, "092 as_7552B0" },
		{ game::MAG_092_ANGELO_SEARCH_Master_State2_WaitChildren, (void *)as_755440, "092 as_755440" },
		{ game::MAG_092_ANGELO_SEARCH_Emitter_State0_SpawnDirector, (void *)as_755500, "092 as_755500" },
		{ game::MAG_092_ANGELO_SEARCH_DirectorDataSetup, (void *)as_755530, "092 as_755530" },
		{ game::MAG_092_ANGELO_SEARCH_DirectorTask, (void *)as_7555B0, "092 as_7555B0" },
		{ game::MAG_092_ANGELO_SEARCH_Director_PhaseBookkeeping, (void *)as_755620, "092 as_755620" },
		{ game::MAG_092_ANGELO_SEARCH_Director_State0_UploadTIM, (void *)as_755670, "092 as_755670" },
		{ game::MAG_092_ANGELO_SEARCH_Director_State2_WaitCue1, (void *)as_7556E0, "092 as_7556E0" },
		{ game::MAG_092_ANGELO_SEARCH_Director_State3_FinishAtFrame9B, (void *)as_755720, "092 as_755720" },
		{ game::MAG_092_ANGELO_SEARCH_SpawnAngeloCreature, (void *)as_755770, "092 as_755770" },
		{ game::MAG_092_ANGELO_SEARCH_AngeloTask, (void *)as_7557B0, "092 as_7557B0" },
		{ game::MAG_092_ANGELO_SEARCH_Angelo_State0_BindRinoaAnimSet, (void *)as_755A80, "092 as_755A80" },
		{ game::MAG_092_ANGELO_SEARCH_Angelo_State1_WalkIn_SpawnSniff, (void *)as_755B70, "092 as_755B70" },
		{ game::MAG_092_ANGELO_SEARCH_Sniff_State0_WaitFrame25, (void *)as_755CD0, "092 as_755CD0" },
		{ game::MAG_092_ANGELO_SEARCH_Sniff_State1_BurstRingsDust, (void *)as_755CE0, "092 as_755CE0" },
		{ game::MAG_092_ANGELO_SEARCH_DustPuffTask, (void *)as_755EE0, "092 as_755EE0" },
		{ game::MAG_092_ANGELO_SEARCH_DustPuff_DrawModel, (void *)as_755FC0, "092 as_755FC0" },
		{ game::MAG_092_ANGELO_SEARCH_RenderPrimModel, (void *)as_7560D0, "092 as_7560D0" },
		{ game::MAG_092_ANGELO_SEARCH_PrimList_FlatTri, (void *)as_756200, "092 as_756200" },
		{ game::MAG_092_ANGELO_SEARCH_PrimList_FlatQuad, (void *)as_756420, "092 as_756420" },
		{ game::MAG_092_ANGELO_SEARCH_PrimList_TexTri, (void *)as_7566B0, "092 as_7566B0" },
		{ game::MAG_092_ANGELO_SEARCH_PrimList_TexQuad, (void *)as_756B30, "092 as_756B30" },
		{ game::MAG_092_ANGELO_SEARCH_PrimList_GouraudTexTriScroll, (void *)as_7575E0, "092 as_7575E0" },
		{ game::MAG_092_ANGELO_SEARCH_PrimList_GT4, (void *)as_757AE0, "092 as_757AE0" },
		{ game::MAG_092_ANGELO_SEARCH_DustPuff_State0_Setup, (void *)as_758130, "092 as_758130" },
		{ game::MAG_092_ANGELO_SEARCH_DustPuff_State1_RollShrink, (void *)as_758170, "092 as_758170" },
		{ game::MAG_092_ANGELO_SEARCH_RingParticleTask, (void *)as_7581D0, "092 as_7581D0" },
		{ game::MAG_092_ANGELO_SEARCH_RingParticle_Draw, (void *)as_7582A0, "092 as_7582A0" },
		{ game::MAG_092_ANGELO_SEARCH_RingParticle_State0_Setup, (void *)as_758970, "092 as_758970" },
		{ game::MAG_092_ANGELO_SEARCH_RingParticle_State1_SequenceStep, (void *)as_758990, "092 as_758990" },
		{ game::MAG_092_ANGELO_SEARCH_Angelo_State2_TurnToRinoa, (void *)as_758A00, "092 as_758A00" },
		{ game::MAG_092_ANGELO_SEARCH_Angelo_State3_WalkToRinoa, (void *)as_758AA0, "092 as_758AA0" },
		{ game::MAG_092_ANGELO_SEARCH_Angelo_State4_ItemPlay_WalkOff, (void *)as_758BB0, "092 as_758BB0" },
		{ game::MAG_092_ANGELO_SEARCH_SpawnItemPlay, (void *)as_758C60, "092 as_758C60" },
		{ game::MAG_092_ANGELO_SEARCH_ItemPlay_State0_Decode, (void *)as_758D10, "092 as_758D10" },
		{ game::MAG_092_ANGELO_SEARCH_ItemPlay_PlayStep, (void *)as_758D70, "092 as_758D70" },
		{ game::MAG_092_ANGELO_SEARCH_ItemPlay_ObjectCallback, (void *)as_758EA0, "092 as_758EA0" },
		{ game::MAG_092_ANGELO_SEARCH_ItemPlay_State1_Play, (void *)as_7590E0, "092 as_7590E0" },
		{ game::MAG_092_ANGELO_SEARCH_Angelo_State5_WalkOff_Hide, (void *)as_759110, "092 as_759110" },
		{ game::MAG_092_ANGELO_SEARCH_DamageState_ApplyActionResult, (void *)as_7591F0, "092 as_7591F0" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag092_angelo_search()
	{
		act::register_module(92);
		for (const act::angelo92::ModPort *p = act::angelo92::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(92, p->addr, p->port, p->name);
		for (const act::angelo92::ModPort *p = act::angelo92::PORTS; p->addr; p++)
			act::register_module_port(92, p->addr, p->port, p->name);
	}
}

