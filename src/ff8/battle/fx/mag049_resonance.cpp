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
namespace resonance
{
	static const Mod MOD_049 = { "resonance", 49, game::MAG_049_RESONANCE_FL, game::MAG_048_MAGMA_BREATH_FL,
		{ {}, {}, game::p_Resonance_DirectorBlock, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_049_FilePtr, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_049_EmitterQueue, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_049_Resonance_EmitterTask, game::MAG_049_Resonance_EmitterSetup, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_049_DirSyncGate, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_049_Resonance_EmitterWaitDamageCue, game::MAG_049_Resonance_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_049_Resonance_Emitter_State3_FinishReleaseRoot, game::MAG_049_Resonance_Emitter_State4_Empty, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::g_Resonance_PacketCursor;
	static constexpr game::Id FRAME_CURSOR = game::BATTLE_PACKET_WRITE_CURSOR;
	static constexpr game::Id DIR_PTR = game::p_Resonance_DirectorBlock;
	static constexpr game::Id WARP_PTR = game::p_Resonance_WarpBlock;
	static constexpr game::Id SOUND = game::unk_15ADAF4;
	static constexpr game::Id BLEND_BUF = game::g_Resonance_BlendBuffer;
	static constexpr game::Id PRIM_CB = game::MAG_049_Resonance_PrimDrawCallback;
	static constexpr game::Id GRID = game::g_Resonance_WarpGrid;
	static constexpr game::Id XWAVE = game::g_Resonance_WaveX, YWAVE = game::g_Resonance_WaveY;
	static constexpr game::Id Q_EMITTER = game::MAG_049_EmitterQueue, Q_DIRECTOR = game::MAG_049_DirectorQueue, Q_MISC = game::MAG_049_WarpHitQueue, Q_PRIM = game::MAG_049_PrimModelQueue, Q_TINT = game::MAG_049_UnusedQueue;
	static constexpr game::Id ORIG_Master = game::MAG_049_Resonance_MasterTask;
	static constexpr game::Id ORIG_Warp = game::MAG_049_Resonance_WarpTask;
	static constexpr game::Id ORIG_Hit = game::MAG_049_Resonance_HitTask;
	static constexpr game::Id ORIG_Prim1 = game::MAG_049_Resonance_PrimTask1, ORIG_Prim2 = game::MAG_049_Resonance_PrimTask2;
	static inline uint32_t ENT(uint32_t slot) { return game_addr(game::BattleEntitySlotData) + slot * 0x9C; }

	namespace rx
	{
		static inline uint32_t SetDrawEnv(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_SetDrawEnv))(a1, a2); }
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

	uint32_t __cdecl rs_821690(uint32_t a1);
	uint32_t __cdecl rs_821800(uint32_t a1);
	uint32_t __cdecl rs_8218C0(uint32_t a1);
	uint32_t __cdecl rs_8218F0(uint32_t a1);
	uint32_t __cdecl rs_821970(uint32_t a1);
	uint32_t __cdecl rs_8219E0(uint32_t a1);
	uint32_t __cdecl rs_821A80(uint32_t a1);
	uint32_t __cdecl rs_821AC0(uint32_t a1);
	uint32_t __cdecl rs_821AF0(uint32_t a1);
	uint32_t __cdecl rs_821B30(uint32_t a1);
	uint32_t __cdecl rs_821C00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl rs_821CB0(uint32_t a1);
	uint32_t __cdecl rs_821D40(uint32_t a1);
	uint32_t __cdecl rs_821E70(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl rs_8220B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl rs_8221E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl rs_822400(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl rs_822690(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl rs_822B10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl rs_8235C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl rs_823AC0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl rs_824110(uint32_t a1);
	uint32_t __cdecl rs_8241A0(uint32_t a1);
	uint32_t __cdecl rs_824230(uint32_t a1);
	uint32_t __cdecl rs_824260(uint32_t a1);
	uint32_t __cdecl rs_8242E0(uint32_t a1);
	uint32_t __cdecl rs_824300();
	uint32_t __cdecl rs_8243B0(uint32_t a1);
	uint32_t __cdecl rs_824410();
	uint32_t __cdecl rs_8244E0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl rs_824520(uint32_t a1);
	uint32_t __cdecl rs_8245B0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl rs_8246D0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl rs_8248A0(uint32_t a1);
	uint32_t __cdecl rs_8248E0(uint32_t a1);
	uint32_t __cdecl rs_824900(uint32_t a1);
	uint32_t __cdecl rs_824970(uint32_t a1);
	uint32_t __cdecl rs_8249E0(uint32_t a1);
	uint32_t __cdecl rs_824A10(uint32_t a1);
	uint32_t __cdecl rs_824A70(uint32_t a1);
	uint32_t __cdecl rs_824AB0(uint32_t a1);
	uint32_t __cdecl rs_824AE0(uint32_t a1);
	uint32_t __cdecl rs_824B00(uint32_t a1);
	uint32_t __cdecl rs_824B20(uint32_t a1);
	uint32_t __cdecl rs_824B90(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace resonance
{
	uint32_t __cdecl rs_821690(uint32_t a1)
	{
		g_mod = &MOD_049;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::unk_26480E4)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_26465E0)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_049_Resonance_Master_State0_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_049_Resonance_Master_State1_Advance);
		states[2] = game_addr(game::MAG_049_Resonance_MasterWaitChildren);
		states[3] = game_addr(game::MAG_049_Resonance_Master_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_049_Resonance_Master_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_049_Resonance_Master_State5_Advance);
		states[6] = game_addr(game::MAG_049_Resonance_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_049_Resonance_Master_State7_RequestBattleSync);
		states[8] = game_addr(game::MAG_049_Resonance_Master_State8_WaitBattleSyncDone);
		states[9] = game_addr(game::MAG_049_Resonance_Master_State9_Finish);
		states[10] = game_addr(game::MAG_049_Resonance_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_264781C));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_26472D4));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::g_Resonance_PacketCursor2)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_2647818));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_26472D0));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::g_Resonance_PacketCursor2)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_EMITTER)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_DIRECTOR)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_MISC)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_PRIM)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_TINT)));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl rs_821AC0(uint32_t a1)
	{
		if (S16(t1_data(), 0x46) >= 0xE)
		{
			x::BdPlaySE(game_addr(SOUND), 0, 0x80);
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl rs_821AF0(uint32_t a1)
	{
		U16(t1_data(), 0x48) = 1;
		U16(a1, 0x26) = (uint16_t)(U16(a1, 0x26) | 1);
		next_state(a1);
		return 0;
	}

	uint32_t __cdecl rs_821B30(uint32_t a1)
	{
		if ((int32_t)S16(t1_data(), 0x40) - 1 != 0)
			return 0;
		const uint32_t node = a1;
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_MISC), game_addr(ORIG_Warp), 0x80, node);
		const uint32_t p1 = rs_821C00(node, game_addr(ORIG_Prim1), game_addr(game::unk_17B329C), 0x2D4, 0, 0);
		U8(p1, 0x2C) = 3;
		const uint32_t p2 = rs_821C00(node, game_addr(ORIG_Prim2), game_addr(game::unk_17B656C), 0x2C8, 1, 0);
		U8(p2, 0x2C) = 4;
		const uint32_t act0 = U32(U32(node, 0xC), 4) + (uint32_t)((int32_t)S8(node, 0x2A) * 20);
		if (U8(act0, 0x10) == 0)
			return 0;
		int32_t k = 0;
		uint32_t act;
		do
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_MISC), game_addr(ORIG_Hit), 0x80, node);
			act = U32(U32(node, 0xC), 4) + (uint32_t)((int32_t)S8(node, 0x2A) * 20);
			const uint8_t slot = U8(U32(act, 8), k * 0x18);
			U8(t, 0x2B) = (uint8_t)k;
			U8(t, 0x2D) = slot;
			k++;
		} while (k < (int32_t)U8(act, 0x10));
		return 0;
	}

	uint32_t __cdecl rs_821C00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PRIM), a2, 0x368, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		S32(t, 0x78) = (int32_t)(int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	static uint32_t rs_place_and_play(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 0x14)
			return 0;
		const uint32_t ent = ENT(U8(node, 0x2C));
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		x::GetEffectSpawnPosition(ent, 0xF0, 0x1000, node + 0x1C);
		const uint16_t facing = U16(ent, 0xE);
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		U16(node, 0x88) = 0xFF80;
		U16(node, 0x62) = facing;
		rs_821D40(node);
		next_state(node);
		return 0;
	}
	uint32_t __cdecl rs_821CB0(uint32_t a1) { return rs_place_and_play(a1); }
	uint32_t __cdecl rs_8241A0(uint32_t a1) { return rs_place_and_play(a1); }

	uint32_t __cdecl rs_821E70(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		alignas(4) uint8_t loc[0x40] = {};
		const uint32_t L = P(loc);
		const uint32_t V = L + 8;
		const uint32_t S = L + 0x10;
		const uint32_t M = L + 0x20;
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
		if ((U32(rec, 4) & 0x8000) == 0)
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
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = rs_8220B0(hdr, ot, 2, cursor);
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl rs_824260(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[7];
		states[0] = game_addr(game::MAG_049_Resonance_WarpWait);
		states[1] = game_addr(game::MAG_049_Resonance_WarpStart);
		states[2] = game_addr(game::MAG_049_Resonance_WarpRampUp);
		states[3] = game_addr(game::MAG_049_Resonance_WarpHold);
		states[4] = game_addr(game::MAG_049_Resonance_WarpRampDown);
		states[5] = game_addr(game::MAG_049_Resonance_Warp_State5_Finish);
		states[6] = game_addr(game::MAG_049_Resonance_Warp_State6_Empty);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl rs_8242E0(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0x20)
		{
			rs_824300();
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl rs_824300()
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(WARP_PTR)), 0x10);
		const uint32_t r0 = x::CrtRand();
		U16(MEM<uint32_t>(game_addr(WARP_PTR)), 6) = (uint16_t)((r0 & 0x3F) + 0x20);
		const uint32_t r1 = x::CrtRand();
		U16(MEM<uint32_t>(game_addr(WARP_PTR)), 8) = (uint16_t)(r1 & 0xFFF);
		const uint32_t r2 = x::CrtRand();
		U16(MEM<uint32_t>(game_addr(WARP_PTR)), 0xC) = (uint16_t)(r2 & 0xFFF);
		uint32_t e = game_addr(GRID) + 4;
		for (uint32_t i = 0; e < game_addr(GRID) + 4 + 88 * 12; i++)
		{
			const uint8_t u0 = (uint8_t)(i << 5);
			const uint8_t u1 = (uint8_t)(u0 + 0x20);
			for (uint32_t j = 0; j < 8; j++, e += 0xC)
			{
				const uint8_t v0 = (uint8_t)(j << 5);
				U8(e, 0) = u0;
				U8(e, 2) = u1;
				if (u1 == 0)
					U8(e, 2) = 0xFF;
				U8(e, 4) = U8(e, 0);
				U8(e, 6) = U8(e, 2);
				U8(e, 3) = v0;
				U8(e, 1) = v0;
				const uint8_t v1 = (uint8_t)(v0 + 0x20);
				U8(e, 7) = v1;
				U8(e, 5) = v1;
			}
		}
		return 0;
	}

	uint32_t __cdecl rs_8243B0(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x64) = 0x200;
		U16(node, 0x62) = 0x200;
		U16(node, 0x60) = 0x200;
		U16(node, 0x74) = 0x200;
		U16(node, 0x72) = 0x200;
		U16(node, 0x70) = 0x200;
		U16(node, 0x58) = 0;
		U16(node, 0x5A) = 0x400;
		U16(node, 0x5C) = 0x800;
		U8(node, 0x7C) = 0x80;
		U8(node, 0x7D) = 0x80;
		U8(node, 0x7E) = 0x80;
		rs_8244E0(node, 5);
		rs_824410();
		next_state(node);
		return 0;
	}

	uint32_t __cdecl rs_824410()
	{
		const uint32_t k = (uint32_t)(MEM<uint8_t>(game_addr(game::g_Battle_DisplayParity)) - 1) & 1;
		const uint32_t pkt_scene = game_addr(game::g_Resonance_DrawEnvScenePkt) + (k << 6);
		const uint32_t pkt_back = game_addr(game::g_Resonance_DrawEnvBackPkt) + (k << 6);
		const uint32_t env_back = game_addr(game::g_Resonance_DrawEnvBack) + k * 0x5C;
		const uint32_t env_disp = game_addr(game::g_Battle_DrawEnv) + k * 0x5C;
		const uint32_t env_scene = game_addr(game::g_Resonance_DrawEnvScene) + k * 0x5C;
		ff8fx::copy<(0x5C)>((void *)env_back, (const void *)env_disp);
		ff8fx::copy<(0x5C)>((void *)env_scene, (const void *)env_disp);
		U16(env_scene, 0) = 0x240;
		U16(env_scene, 2) = 0x100;
		U16(env_scene, 4) = 0x140;
		U16(env_scene, 6) = 0xF0;
		U16(env_scene, 8) = 0x240;
		U16(env_scene, 0xA) = 0x100;
		U8(env_scene, 0x18) = 1;
		U8(env_scene, 0x19) = 0;
		U8(env_scene, 0x1A) = 0;
		U8(env_scene, 0x1B) = 0;
		U8(env_back, 0x18) = 0;
		rx::SetDrawEnv(pkt_scene, env_scene);
		x::SSIGPU_InsertPrimAutoDepth(MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4484, pkt_scene);
		rx::SetDrawEnv(pkt_back, env_back);
		x::SSIGPU_InsertPrimAutoDepth(MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x14, pkt_back);
		return 0;
	}

	uint32_t __cdecl rs_8244E0(uint32_t a1, uint32_t a2)
	{
		rs_8246D0(a1, a2);
		rs_824520(1);
		rs_8245B0(a1, a2, 1);
		rs_824520(0);
		rs_8245B0(a1, a2, 0);
		return 0;
	}

	uint32_t __cdecl rs_824520(uint32_t a1)
	{
		const uint32_t r = a1;
		for (uint32_t i = 0; i < 0xB; i++)
		{
			const uint16_t bx = (uint16_t)(i * 0x20);
			uint16_t by = 0;
			for (uint32_t j = 0; j < 8; j++)
			{
				const uint32_t e = game_addr(GRID) + (j + i * 8) * 0xC;
				if (i == 0)
					U16(e, 0) = 0;
				else if (i == 0xA)
					U16(e, 0) = 0x140;
				else
					U16(e, 0) = (uint16_t)(U16(game_addr(XWAVE) + (r + j * 2) * 2, 0) + bx);
				if (j == 0)
					U16(e, 2) = 0;
				else if (j == 7)
					U16(e, 2) = 0xF0;
				else
					U16(e, 2) = (uint16_t)(U16(game_addr(YWAVE) + (r + i * 2) * 2, 0) + by);
				by = (uint16_t)(by + 0x20);
			}
		}
		return 0;
	}

	uint32_t __cdecl rs_8245B0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		(void)a1;
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + a2 * 4;
		uint32_t pkt = MEM<uint32_t>(game_addr(FRAME_CURSOR));
		const uint8_t code = ((uint16_t)a3 == 1) ? 0x2F : 0x2D;
		uint16_t tpage = 0x119;
		for (uint32_t i = 0; i < 0xA; i++)
		{
			if (i == 8)
				tpage = 0x11D;
			uint32_t e = game_addr(GRID) + i * 0x60 + 6;
			for (int j = 0; j < 7; j++)
			{
				U8(pkt, 7) = code;
				U32(pkt, 0) = 0x9000000;
				U16(pkt, 0x14) = U16(e, 0);
				U32(pkt, 8) = U32(e, -6);
				U16(pkt, 0xC) = U16(e, -2);
				U16(pkt, 0x1C) = U16(e, 2);
				U16(pkt, 0x24) = U16(e, 4);
				U32(pkt, 0x10) = U32(e, 0x5A);
				U32(pkt, 0x20) = U32(e, 0x66);
				U32(pkt, 0x18) = U32(e, 6);
				U16(pkt, 0x16) = tpage;
				const uint32_t mode = pkt + 0x28;
				const uint32_t tp = x::sub_45C690(0, 1, 0, 0) & 0xFFFF;
				x::sub_45BFC0(mode, 0, 0, tp, 0);
				x::SSIGPU_InsertPrimAutoDepth(ot, mode);
				x::SSIGPU_InsertPrimAltViewport(ot, pkt);
				e += 0xC;
				pkt = mode + 0xC;
			}
		}
		MEM<uint32_t>(game_addr(FRAME_CURSOR)) = pkt;
		return 0;
	}

	uint32_t __cdecl rs_8246D0(uint32_t a1, uint32_t a2)
	{
		(void)a2;
		const uint32_t node = a1;
		static const int32_t PH[6] = { 0x58, 0x5A, 0x5C, 0x68, 0x6A, 0x6C };
		for (int k = 0; k < 6; k++)
		{
			const uint32_t r = x::CrtRand();
			U16(node, PH[k]) = (uint16_t)((U16(node, PH[k]) + (r & 0xFF) + 0x100) & 0xFFF);
		}
		for (uint32_t row = 0; row < 2; row++)
		{
			const uint16_t step = U16(node, 0x60 + row * 2);
			uint32_t ph = U16(node, 0x58 + row * 2);
			for (uint32_t k = 0; k < 8; k++)
			{
				const int32_t s = (int32_t)x::computeSin((uint32_t)(int32_t)(int16_t)ph);
				const int32_t v = mul32(s, (int32_t)S16(MEM<uint32_t>(game_addr(WARP_PTR)), 0));
				U16(game_addr(XWAVE) + row * 2 + k * 4, 0) = (uint16_t)(add32(v, (v >> 31) & 0x1FFFFF) >> 21);
				ph = (ph + step) & 0xFFF;
			}
		}
		for (uint32_t row = 0; row < 2; row++)
		{
			uint32_t ph = U16(node, 0x68 + row * 2);
			const uint16_t step = U16(node, 0x70 + row * 2);
			for (uint32_t k = 0; k < 0xB; k++)
			{
				const int32_t s = (int32_t)x::computeSin((uint32_t)(int32_t)(int16_t)ph);
				ph = (ph + step) & 0xFFF;
				const int32_t v = mul32(s, (int32_t)S16(MEM<uint32_t>(game_addr(WARP_PTR)), 0));
				U16(game_addr(YWAVE) + row * 2 + k * 4, 0) = (uint16_t)(add32(v, (v >> 31) & 0x1FFFFF) >> 21);
			}
		}
		return 0;
	}

	uint32_t __cdecl rs_8248A0(uint32_t a1)
	{
		const uint32_t w = MEM<uint32_t>(game_addr(WARP_PTR));
		U16(w, 0) = (uint16_t)(U16(w, 0) + 0x100);
		if (S16(w, 0) >= 0x1000)
		{
			U16(w, 0) = 0x1000;
			next_state(a1);
		}
		rs_8244E0(a1, 5);
		return rs_824410();
	}

	uint32_t __cdecl rs_8248E0(uint32_t a1)
	{
		if (S16(a1, 0x24) > 0x38)
			next_state(a1);
		rs_8244E0(a1, 5);
		return rs_824410();
	}

	uint32_t __cdecl rs_824900(uint32_t a1)
	{
		const uint32_t w = MEM<uint32_t>(game_addr(WARP_PTR));
		U16(w, 0) = (uint16_t)(U16(w, 0) - 0x80);
		if (S16(w, 0) <= 0)
		{
			U16(w, 0) = 0;
			next_state(a1);
		}
		rs_8244E0(a1, 5);
		return rs_824410();
	}

	uint32_t __cdecl rs_8249E0(uint32_t a1)
	{
		if (S16(t1_data(), 0x46) >= 0x1C)
		{
			rs_824A10(a1);
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl rs_824A10(uint32_t a1)
	{
		const uint32_t ent = ENT(U8(a1, 0x2D));
		const uint32_t r = x::CrtRand() & 1;
		if (r == 0)
			U16(a1, 0x78) = 4;
		else
			U16(a1, 0x78) = 5;
		return x::queueChainTransformationConditional(ent, (uint32_t)(int32_t)S16(a1, 0x78));
	}

	uint32_t __cdecl rs_824A70(uint32_t a1)
	{
		const uint32_t ent = ENT(U8(a1, 0x2D));
		if (U16(a1, 0x78) != (uint16_t)MEM<uint8_t>(U32(ent, 0x74)))
		{
			rs_824AB0(a1);
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl rs_824AB0(uint32_t a1)
	{
		const uint32_t slot = U8(a1, 0x2D);
		U16(a1, 0x78) = 6;
		return x::queueChainTransformationConditional(ENT(slot), 6);
	}

	uint32_t __cdecl rs_824AE0(uint32_t a1)
	{
		if (S16(t1_data(), 0x46) >= 0x37)
			next_state(a1);
		return 0;
	}
	uint32_t __cdecl rs_824B00(uint32_t a1)
	{
		if (S16(t1_data(), 0x46) >= 0x4B)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl rs_824B20(uint32_t a1)
	{
		const uint32_t ent = ENT(U8(a1, 0x2D));
		if (U16(a1, 0x78) == (uint16_t)MEM<uint8_t>(U32(ent, 0x74)))
			return 0;
		const uint32_t act = U32(U32(a1, 0xC), 4) + (uint32_t)((int32_t)S8(a1, 0x2A) * 20);
		x::ApplyActionResultToTarget(U32(act, 8) + (uint32_t)((int32_t)S8(a1, 0x2B) * 24));
		U8(a1, 0x26) |= 1;
		next_state(a1);
		return 0;
	}

	uint32_t __cdecl rs_824B90(uint32_t a1)
	{
		if (U16(t1_data(), 0x48) == 1)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl rs_821800(uint32_t a1)
	{
		if (U8(a1, 0x28) == 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl rs_8218C0(uint32_t a1)
	{
		rs_8218F0(a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_049_DirectorQueue), game_addr(game::MAG_049_Resonance_DirectorTask), 0x48, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl rs_8218F0(uint32_t a1)
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

	uint32_t __cdecl rs_821970(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[5];
		states[0] = game_addr(game::MAG_049_Resonance_Director_State0_WaitDirGate1);
		states[1] = game_addr(game::MAG_049_Resonance_DirectorWaitCue1);
		states[2] = game_addr(game::MAG_049_Resonance_DirectorPlaySound);
		states[3] = game_addr(game::MAG_049_Resonance_DirectorDamageCue);
		states[4] = game_addr(game::MAG_049_Resonance_Director_State4_Empty);
		rs_8219E0(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl rs_8219E0(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			rs_821B30(a1);
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

	uint32_t __cdecl rs_821A80(uint32_t a1)
	{
		if (a_73B7E0(1) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl rs_821D40(uint32_t a1)
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

	uint32_t __cdecl rs_8220B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		static const Draw lists[8] = { rs_8221E0, rs_822400, rs_822690, rs_822B10, a_73D770, a_73D9C0, rs_8235C0, rs_823AC0 };
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

	uint32_t __cdecl rs_8221E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl rs_822400(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl rs_822690(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl rs_822B10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl rs_8235C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl rs_823AC0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl rs_824110(uint32_t a1)
	{
		const uint32_t r = rs_821D40(a1);
		if (r != 0)
			return r;
		U8(a1, 0x26) |= 1;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl rs_824230(uint32_t a1)
	{
		const uint32_t r = rs_821D40(a1);
		if (r != 0)
			return r;
		U8(a1, 0x26) |= 1;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl rs_824970(uint32_t a1)
	{
		const uint32_t tab[6] = { game_addr(game::MAG_049_Resonance_HitWait), game_addr(game::MAG_049_Resonance_HitWaitReaction), game_addr(game::MAG_049_Resonance_HitWait37), game_addr(game::MAG_049_Resonance_HitWait4B), game_addr(game::MAG_049_Resonance_HitApplyDamage), game_addr(game::MAG_049_Resonance_Hit_State5_Empty) };
		callp(tab[S8(a1, 0x29)], a1);
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24)++;
		return task_end(a1, status);
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_049_Resonance_Master_State0_Advance, (void *)a_73A0D0, "049 MAG_049_sub_8217E0" },
		{ game::MAG_049_Resonance_Master_State1_Advance, (void *)a_73A0D0, "049 MAG_049_sub_8217F0" },
		{ game::MAG_049_Resonance_Master_State3_SpawnEmitter, (void *)a_73A170, "049 MAG_049_sub_821810" },
		{ game::MAG_049_Resonance_EmitterTask, (void *)a_73A1A0, "049 MAG_049_sub_821840" },
		{ game::MAG_049_Resonance_Director_State0_WaitDirGate1, (void *)a_73B790, "049 sub_821A30" },
		{ game::MAG_049_DirSyncGate, (void *)a_73B640, "049 sub_821A50" },
		{ game::MAG_049_DirPhaseGE, (void *)a_73B7E0, "049 sub_821AA0" },
		{ game::MAG_049_Resonance_Director_State4_Empty, (void *)a_73A6D0, "049 nullsub_1545" },
		{ game::MAG_049_St_Empty, (void *)a_73A6D0, "049 nullsub_1546" },
		{ game::MAG_049_Resonance_PrimTask1, (void *)a_73A380, "049 sub_821C50" },
		{ game::MAG_049_EmitG3, (void *)a_73D770, "049 sub_823090" },
		{ game::MAG_049_EmitG4, (void *)a_73D9C0, "049 sub_8232E0" },
		{ game::MAG_049_Resonance_PrimTask1_State2_Empty, (void *)a_73A6D0, "049 nullsub_1547" },
		{ game::MAG_049_Resonance_PrimTask2, (void *)a_73A380, "049 sub_824140" },
		{ game::MAG_049_Resonance_PrimTask2_State2_Empty, (void *)a_73A6D0, "049 nullsub_1548" },
		{ game::MAG_049_Resonance_Warp_State5_Finish, (void *)a_7475A0, "049 sub_824940" },
		{ game::MAG_049_Resonance_Warp_State6_Empty, (void *)a_73A6D0, "049 nullsub_1549" },
		{ game::MAG_049_Resonance_Hit_State5_Empty, (void *)a_73A6D0, "049 nullsub_1550" },
		{ game::MAG_049_Resonance_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "049 MAG_049_sub_824BB0" },
		{ game::MAG_049_Resonance_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "049 MAG_049_sub_824BD0" },
		{ game::MAG_049_Resonance_Emitter_State4_Empty, (void *)a_73A6D0, "049 nullsub_1551" },
		{ game::MAG_049_Resonance_Master_State4_NextTargetLoop, (void *)a_747500, "049 MAG_049_sub_824C00" },
		{ game::MAG_049_Resonance_Master_State5_Advance, (void *)a_73A0D0, "049 MAG_049_sub_824C40" },
		{ game::MAG_049_Resonance_Master_State6_WaitQueuesEmpty, (void *)a_747550, "049 MAG_049_sub_824C50" },
		{ game::MAG_049_Resonance_Master_State7_RequestBattleSync, (void *)a_747560, "049 MAG_049_sub_824C60" },
		{ game::MAG_049_Resonance_Master_State8_WaitBattleSyncDone, (void *)a_747590, "049 MAG_049_sub_824C90" },
		{ game::MAG_049_Resonance_Master_State9_Finish, (void *)a_7475A0, "049 MAG_049_sub_824CA0" },
		{ game::MAG_049_Resonance_Master_State10_Empty, (void *)a_73A6D0, "049 nullsub_1544" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_049_Resonance_MasterTask, (void *)rs_821690, "049 rs_821690" },
		{ game::MAG_049_Resonance_MasterWaitChildren, (void *)rs_821800, "049 rs_821800" },
		{ game::MAG_049_Resonance_EmitterSetup, (void *)rs_8218C0, "049 rs_8218C0" },
		{ game::MAG_049_Resonance_InitDirectorBlock, (void *)rs_8218F0, "049 rs_8218F0" },
		{ game::MAG_049_Resonance_DirectorTask, (void *)rs_821970, "049 rs_821970" },
		{ game::MAG_049_Resonance_DirectorPhaseStep, (void *)rs_8219E0, "049 rs_8219E0" },
		{ game::MAG_049_Resonance_DirectorWaitCue1, (void *)rs_821A80, "049 rs_821A80" },
		{ game::MAG_049_Resonance_DirectorPlaySound, (void *)rs_821AC0, "049 rs_821AC0" },
		{ game::MAG_049_Resonance_DirectorDamageCue, (void *)rs_821AF0, "049 rs_821AF0" },
		{ game::MAG_049_Resonance_SpawnPhaseTasks, (void *)rs_821B30, "049 rs_821B30" },
		{ game::MAG_049_Resonance_SpawnPrimTask, (void *)rs_821C00, "049 rs_821C00" },
		{ game::MAG_049_Resonance_PrimTask1_Place, (void *)rs_821CB0, "049 rs_821CB0" },
		{ game::MAG_049_Resonance_PrimPlay, (void *)rs_821D40, "049 rs_821D40" },
		{ game::MAG_049_Resonance_PrimDrawCallback, (void *)rs_821E70, "049 rs_821E70" },
		{ game::MAG_049_Resonance_RenderPrimLists, (void *)rs_8220B0, "049 rs_8220B0" },
		{ game::MAG_049_PrimList_FlatTri, (void *)rs_8221E0, "049 rs_8221E0" },
		{ game::MAG_049_PrimList_FlatQuad, (void *)rs_822400, "049 rs_822400" },
		{ game::MAG_049_PrimList_TexTri, (void *)rs_822690, "049 rs_822690" },
		{ game::MAG_049_PrimList_TexQuad, (void *)rs_822B10, "049 rs_822B10" },
		{ game::MAG_049_PrimList_GT3Scroll, (void *)rs_8235C0, "049 rs_8235C0" },
		{ game::MAG_049_PrimList_GT4, (void *)rs_823AC0, "049 rs_823AC0" },
		{ game::MAG_049_Resonance_PrimTask1_Play, (void *)rs_824110, "049 rs_824110" },
		{ game::MAG_049_Resonance_PrimTask2_Place, (void *)rs_8241A0, "049 rs_8241A0" },
		{ game::MAG_049_Resonance_PrimTask2_Play, (void *)rs_824230, "049 rs_824230" },
		{ game::MAG_049_Resonance_WarpTask, (void *)rs_824260, "049 rs_824260" },
		{ game::MAG_049_Resonance_WarpWait, (void *)rs_8242E0, "049 rs_8242E0" },
		{ game::MAG_049_Resonance_WarpInit, (void *)rs_824300, "049 rs_824300" },
		{ game::MAG_049_Resonance_WarpStart, (void *)rs_8243B0, "049 rs_8243B0" },
		{ game::MAG_049_Resonance_WarpSetDrawEnvs, (void *)rs_824410, "049 rs_824410" },
		{ game::MAG_049_Resonance_WarpDraw, (void *)rs_8244E0, "049 rs_8244E0" },
		{ game::MAG_049_Resonance_WarpBuildGrid, (void *)rs_824520, "049 rs_824520" },
		{ game::MAG_049_Resonance_WarpDrawQuads, (void *)rs_8245B0, "049 rs_8245B0" },
		{ game::MAG_049_Resonance_WarpStepWaves, (void *)rs_8246D0, "049 rs_8246D0" },
		{ game::MAG_049_Resonance_WarpRampUp, (void *)rs_8248A0, "049 rs_8248A0" },
		{ game::MAG_049_Resonance_WarpHold, (void *)rs_8248E0, "049 rs_8248E0" },
		{ game::MAG_049_Resonance_WarpRampDown, (void *)rs_824900, "049 rs_824900" },
		{ game::MAG_049_Resonance_HitTask, (void *)rs_824970, "049 rs_824970" },
		{ game::MAG_049_Resonance_HitWait, (void *)rs_8249E0, "049 rs_8249E0" },
		{ game::MAG_049_Resonance_HitPlayReaction, (void *)rs_824A10, "049 rs_824A10" },
		{ game::MAG_049_Resonance_HitWaitReaction, (void *)rs_824A70, "049 rs_824A70" },
		{ game::MAG_049_Resonance_HitPlayAnim6, (void *)rs_824AB0, "049 rs_824AB0" },
		{ game::MAG_049_Resonance_HitWait37, (void *)rs_824AE0, "049 rs_824AE0" },
		{ game::MAG_049_Resonance_HitWait4B, (void *)rs_824B00, "049 rs_824B00" },
		{ game::MAG_049_Resonance_HitApplyDamage, (void *)rs_824B20, "049 rs_824B20" },
		{ game::MAG_049_Resonance_EmitterWaitDamageCue, (void *)rs_824B90, "049 rs_824B90" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag049_resonance()
	{
		act::register_module(49);
		for (const act::resonance::ModPort *p = act::resonance::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(49, p->addr, p->port, p->name);
		for (const act::resonance::ModPort *p = act::resonance::PORTS; p->addr; p++)
			act::register_module_port(49, p->addr, p->port, p->name);
	}
}

