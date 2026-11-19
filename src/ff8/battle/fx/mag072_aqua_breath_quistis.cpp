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

// Effect 72: Aqua Breath

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
namespace aqua72
{
	static const Mod MOD_072 = { "aqua72", 72, game::_keep_, game::MAG_071_LASER_EYE_QUISTIS_FL,
		{ {}, {}, game::MAG_072_AQUA_BREATH_QUISTIS_pDirectorBlock, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_072_AQUA_BREATH_QUISTIS_EmitterQueue, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_072_AQUA_BREATH_QUISTIS_EmitterTask, game::MAG_072_AQUA_BREATH_QUISTIS_Emitter_State0_SpawnDirector, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_072_AQUA_BREATH_QUISTIS_Director_RequestPhase, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_072_AQUA_BREATH_QUISTIS_DrawSpriteQuads, {}, game::MAG_072_AQUA_BREATH_QUISTIS_Emitter_State1_ApplyDamageOnCue, game::MAG_072_AQUA_BREATH_QUISTIS_Emitter_State2_WaitChildren, game::MAG_072_AQUA_BREATH_QUISTIS_Emitter_State3_Finish, game::MAG_072_AQUA_BREATH_QUISTIS_Emitter_State4_Nullsub, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::MAG_072_AQUA_BREATH_QUISTIS_PacketCursor;
	static constexpr game::Id DIR_PTR = game::MAG_072_AQUA_BREATH_QUISTIS_pDirectorBlock;
	static constexpr game::Id PHASE_DATA = DIR_PTR;
	static constexpr game::Id Q_EMITTER = game::MAG_072_AQUA_BREATH_QUISTIS_EmitterQueue, Q_DIRECTOR = game::MAG_072_AQUA_BREATH_QUISTIS_DirectorQueue, Q_TASKS = game::MAG_072_AQUA_BREATH_QUISTIS_TaskQueue, Q_ORB = game::MAG_072_AQUA_BREATH_QUISTIS_OrbQueue, Q_SPARE = game::MAG_072_AQUA_BREATH_QUISTIS_SpareQueue;
	static constexpr game::Id ORIG_Master = game::MAG_072_AQUA_BREATH_QUISTIS_MasterTask;
	static constexpr game::Id ORIG_Darken = game::MAG_072_AQUA_BREATH_QUISTIS_StageDarkenTask;
	static constexpr game::Id ORIG_Orb = game::MAG_072_AQUA_BREATH_QUISTIS_OrbTask;
	static constexpr game::Id ORIG_Stream = game::MAG_072_AQUA_BREATH_QUISTIS_StreamTask;
	static constexpr game::Id ORIG_Drops = game::MAG_072_AQUA_BREATH_QUISTIS_StreamDropsTask;
	static constexpr game::Id ORIG_Splash = game::MAG_072_AQUA_BREATH_QUISTIS_SplashTask;
	static constexpr game::Id ORIG_SplashDrops = game::MAG_072_AQUA_BREATH_QUISTIS_SplashDropsTask;
	static constexpr game::Id ORIG_Rain = game::MAG_072_AQUA_BREATH_QUISTIS_RainTask;
	static constexpr game::Id ORIG_RainDrops = game::MAG_072_AQUA_BREATH_QUISTIS_RainDropsTask;
	static constexpr game::Id ORB_MODEL = game::MAG_072_AQUA_BREATH_QUISTIS_OrbModel;
	static constexpr game::Id ORB_TEMPLATE = game::MAG_072_AQUA_BREATH_QUISTIS_OrbModelTemplate;
	static constexpr game::Id DROP_SPRITES = game::MAG_072_AQUA_BREATH_QUISTIS_StreamDropSprites;
	static constexpr game::Id RAIN_SPRITES = game::MAG_072_AQUA_BREATH_QUISTIS_RainDropSprites;
	static inline uint32_t ENT(uint32_t slot) { return game_addr(game::BattleEntitySlotData) + slot * 0x9C; }
	static constexpr game::Id BIND_BLOCK = game::MAG_072_AQUA_BREATH_QUISTIS_BindBlock;
	static constexpr game::Id COM_ANIM = game::MAG_072_AQUA_BREATH_QUISTIS_ComAnimData;
	static inline void BindEntityModelFromComData(uint32_t entity, uint32_t out, uint32_t data) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	static inline void QueueChainTransformation(uint32_t entity, int32_t id) { fn<void (__cdecl *)(uint32_t, int32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }

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

	static constexpr game::Id FALLOC_PTR = game::GLOBAL_MEMORY_POOL;
	static uint16_t g_pad_residue = 0;
	static uint16_t ORB_VERTEX_PAD = 0;
	static inline uint16_t orb_vertex_pad(uint32_t node, int32_t state)
	{
		if (state <= 2)
			return (uint16_t)(node >> 16);
		return g_pad_residue;
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

	uint32_t __cdecl ab_7D1FD0(uint32_t a1);
	uint32_t __cdecl ab_7D2140(uint32_t a1);
	uint32_t __cdecl ab_7D2200(uint32_t a1);
	uint32_t __cdecl ab_7D2230(uint32_t a1);
	uint32_t __cdecl ab_7D2370(uint32_t a1);
	uint32_t __cdecl ab_7D2430(uint32_t a1);
	uint32_t __cdecl ab_7D24B0(uint32_t a1);
	uint32_t __cdecl ab_7D2500(uint32_t a1);
	uint32_t __cdecl ab_7D2620(uint32_t a1);
	uint32_t __cdecl ab_7D2660(uint32_t a1);
	uint32_t __cdecl ab_7D2670(uint32_t a1);
	uint32_t __cdecl ab_7D26C0(uint32_t a1);
	uint32_t __cdecl ab_7D2720(uint32_t a1);
	uint32_t __cdecl ab_7D2770(uint32_t a1);
	uint32_t __cdecl ab_7D27F0(uint32_t a1);
	uint32_t __cdecl ab_7D28F0(uint32_t a1);
	uint32_t __cdecl ab_7D29A0(uint32_t a1);
	uint32_t __cdecl ab_7D2AD0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ab_7D2C00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ab_7D2E20(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ab_7D30B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ab_7D3530(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ab_7D3FE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ab_7D44E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ab_7D4B30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ab_7D4C20(uint32_t a1);
	uint32_t __cdecl ab_7D4C80(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ab_7D4D20(uint32_t a1);
	uint32_t __cdecl ab_7D4D60(uint32_t a1);
	uint32_t __cdecl ab_7D4D90(uint32_t a1);
	uint32_t __cdecl ab_7D4E30(uint32_t a1);
	uint32_t __cdecl ab_7D4E70(uint32_t a1);
	uint32_t __cdecl ab_7D4F80(uint32_t a1);
	uint32_t __cdecl ab_7D50D0(uint32_t a1);
	uint32_t __cdecl ab_7D57A0(uint32_t a1);
	uint32_t __cdecl ab_7D57E0(uint32_t a1);
	uint32_t __cdecl ab_7D58B0(uint32_t a1);
	uint32_t __cdecl ab_7D5940(uint32_t a1);
	uint32_t __cdecl ab_7D5A70(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ab_7D5AD0(uint32_t a1);
	uint32_t __cdecl ab_7D5BE0(uint32_t a1);
	uint32_t __cdecl ab_7D5C00(uint32_t a1);
	uint32_t __cdecl ab_7D5CA0(uint32_t a1);
	uint32_t __cdecl ab_7D5CF0(uint32_t a1);
	uint32_t __cdecl ab_7D5E50(uint32_t a1);
	uint32_t __cdecl ab_7D5F60(uint32_t a1);
	uint32_t __cdecl ab_7D5F80(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace aqua72
{
	uint32_t __cdecl ab_7D2230(uint32_t a1)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(DIR_PTR)), 0x54);
		a_73A720(MEM<uint32_t>(game_addr(DIR_PTR)) + 8);
		const uint32_t node = a1;
		const uint32_t ent = ENT(U8(node, 0x2C));
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		U32(d, 0) = U32(ent, 0x1C);
		U32(d, 4) = U32(ent, 0x20);
		const int16_t dz = (int16_t)(U16(d, 0xC) - U16(d, 4));
		const int16_t dx = (int16_t)(U16(d, 8) - U16(d, 0));
		const uint32_t ang = x::CartesianToGameAngle((uint32_t)(int32_t)dx, (uint32_t)(int32_t)dz);
		U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x4A) = (uint16_t)((ang - 0x800) & 0xFFF);
		const uint32_t dist = ab_7D2370(node);
		const uint32_t d2 = MEM<uint32_t>(game_addr(DIR_PTR));
		U32(d2, 0x3C) = dist;
		if ((int32_t)dist < 0x1000)
			U32(d2, 0x3C) = 0x1000;
		else if ((int32_t)dist > 0x2000)
			U32(d2, 0x3C) = 0x2000;
		return dist;
	}

	uint32_t __cdecl ab_7D2430(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[7];
		states[0] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Director_State0_DarkenAndCastAnim);
		states[1] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Director_State1_CuePhase1);
		states[2] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Director_State2_PlaySound);
		states[3] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Director_State3_DamageCue);
		states[4] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Director_State4_CuePhase2);
		states[5] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Director_State5_Finish);
		states[6] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Director_State6_Nullsub);
		ab_7D24B0(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ab_7D2500(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t ent = ENT(U8(node, 0x2C));
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TASKS), game_addr(ORIG_Darken), 0x154, node);
		BindEntityModelFromComData(ent, game_addr(BIND_BLOCK), game_addr(COM_ANIM));
		QueueChainTransformation(ent, 1);
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		U16(node, 0x44) = 4;
		U16(ent, 0xE) = U16(d, 0x4A);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ab_7D26C0(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x44) = (uint16_t)(U16(node, 0x44) - 1);
		if (S16(node, 0x44) > 0)
			return 0;
		if (a_73B640(1) != 0)
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ab_7D27F0(uint32_t a1)
	{
		if ((int32_t)S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x40) - 1 != 0)
			return 0;
		const uint32_t node = a1;
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_ORB), game_addr(ORIG_Orb), 0x60, node);
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TASKS), game_addr(ORIG_Stream), 0x154, node);
		uint32_t act = U32(U32(node, 0xC), 4) + (uint32_t)((int32_t)S8(node, 0x2A) * 20);
		if (U8(act, 0x10) != 0)
		{
			int32_t k = 0;
			do
			{
				const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TASKS), game_addr(ORIG_Splash), 0x154, node);
				act = U32(U32(node, 0xC), 4) + (uint32_t)((int32_t)S8(node, 0x2A) * 20);
				U8(t, 0x2D) = U8(U32(act, 8), k * 0x18);
				k++;
			} while (k < (int32_t)U8(act, 0x10));
		}
		act = U32(U32(node, 0xC), 4) + (uint32_t)((int32_t)S8(node, 0x2A) * 20);
		if (U8(act, 0x10) != 0)
		{
			int32_t k = 0;
			do
			{
				const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TASKS), game_addr(ORIG_Rain), 0x154, node);
				act = U32(U32(node, 0xC), 4) + (uint32_t)((int32_t)S8(node, 0x2A) * 20);
				U8(t, 0x2D) = U8(U32(act, 8), k * 0x18);
				k++;
			} while (k < (int32_t)U8(act, 0x10));
		}
		return 0;
	}

	uint32_t __cdecl ab_7D28F0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[5];
		states[0] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Orb_State0_Appear);
		states[1] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Orb_State1_FadeIn);
		states[2] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Orb_State2_Hold);
		states[3] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Orb_State3_FadeOut);
		states[4] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Orb_State4_Nullsub);
		const int32_t st = S8(node, 0x29);
		callp(states[st], node);
		ORB_VERTEX_PAD = orb_vertex_pad(node, st);
		const uint32_t b = (uint32_t)(uint16_t)(U16(node, 0x58) + 0xA0) & 0xFFF;
		const uint32_t a = (uint32_t)(uint16_t)(U16(node, 0x54) + 0xA0) & 0xFFF;
		U16(node, 0x58) = (uint16_t)b;
		U16(node, 0x54) = (uint16_t)a;
		ab_7D4B30(game_addr(ORB_TEMPLATE), U32(node, 0x4C), a, b);
		U16(node, 0x5E) = (uint16_t)((uint8_t)(U8(node, 0x5E) + 8) & 0x7F);
		ab_7D29A0(node);
		if ((U8(node, 0x26) & 4) == 0)
			g_pad_residue = 0;
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ab_7D4B30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
			const uint32_t ang = (uint32_t)add32((z - (z >> 31)) >> 1, (int16_t)a3) & 0xFFF;
			const int32_t z5 = z * 0x500;
			const int16_t amp = (int16_t)((z5 + ((z5 >> 31) & 0xFFF)) >> 12);
			alignas(4) uint8_t in[8];
			alignas(4) uint16_t out[4];
			out[3] = ORB_VERTEX_PAD;
			U32(P(in), 0) = U32(v, 0);
			U32(P(in), 4) = U32(v, 4);
			x::GTE_SetRotMatrix(P(m));
			x::GTE_LoadV0(P(in));
			x::GTE_MVMVA_RotV0();
			x::GTE_StoreIR123(P(out));
			const int32_t s = (int32_t)x::computeSin(ang);
			const int32_t p = mul32(s, amp);
			const uint32_t zw = U32(P(out), 4);
			out[0] = (uint16_t)(out[0] + (uint16_t)((p + ((p >> 31) & 0xFFF)) >> 12));
			U32(v + delta, 4) = U32(P(out), 0);
			U32(v + delta, 8) = zw;
		}
		return 0;
	}

	uint32_t __cdecl ab_7D4C20(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t s = U32(MEM<uint32_t>(game_addr(DIR_PTR)), 0x3C);
		const uint8_t slot = U8(node, 0x2C);
		U32(node, 0x38) = s;
		U32(node, 0x34) = s;
		U32(node, 0x30) = s;
		U32(node, 0x4C) = game_addr(ORB_MODEL);
		U16(node, 0x46) = U16(ENT(slot), 0xE);
		ab_7D4C80(node + 0x1C, slot);
		const uint8_t st = U8(node, 0x29);
		U16(node, 0x50) = 0x1000;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl ab_7D4C80(uint32_t a1, uint32_t a2)
	{
		const uint32_t pos = a1;
		const uint32_t ent = ENT((uint32_t)(int32_t)(int16_t)a2);
		x::GetEffectSpawnPosition(ent, 0xF0, 0, pos);
		alignas(4) uint16_t v[4];
		alignas(4) uint8_t m[0x20];
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		v[3] = 0;
		x::MAG_022_sub_8DD770(P(m));
		x::MAG_022_sub_8DD8A0(P(m), (uint32_t)(int32_t)S16(ent, 0xE));
		x::GTE_SetRotMatrix(P(m));
		x::GTE_LoadV0(P(v));
		x::GTE_MVMVA_RotV0();
		x::GTE_StoreIR123(P(v));
		U16(pos, 0) = (uint16_t)(U16(pos, 0) + v[0]);
		U16(pos, 2) = (uint16_t)(U16(pos, 2) + v[1]);
		U16(pos, 4) = (uint16_t)(U16(pos, 4) + v[2]);
		return 0;
	}

	uint32_t __cdecl ab_7D4D20(uint32_t a1)
	{
		ab_7D4C80(a1 + 0x1C, (uint32_t)U8(a1, 0x2C));
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0xFE00);
		if (S16(a1, 0x50) <= 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0x50) = 0;
			U16(a1, 0x5C) = 0xF;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl ab_7D4E30(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t ent = ENT(U8(node, 0x2C));
		x::MAG_022_sub_8DD770(node + 0x58);
		x::MAG_022_sub_8DD8A0(node + 0x58, (uint32_t)(int32_t)S16(ent, 0xE));
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ab_7D4E70(uint32_t a1)
	{
		const uint32_t node = a1;
		ab_7D4C80(node + 0x1C, (uint32_t)U8(node, 0x2C));
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TASKS), game_addr(ORIG_Drops), 0x154, node);
		uint32_t spin = t + 0x148;
		uint32_t mt = t + 0xC0;
		for (int32_t n = 4; n != 0; n--)
		{
			const int32_t r1 = (int32_t)x::CrtRand();
			const int32_t yaw = r1 % 0x300 - 0x180;
			const int32_t r2 = (int32_t)x::CrtRand();
			const int32_t pitch = r2 % 0x180;
			ff8fx::copy<(0x20)>((void *)mt, (const void *)(node + 0x58));
			const int16_t pitch16 = (int16_t)(pitch - 0xC0);
			U32(t, 0x78) = U32(node, 0x1C);
			U32(t, 0x7C) = U32(node, 0x20);
			x::MAG_022_sub_8DD8A0(mt, (uint32_t)(int32_t)(int16_t)yaw);
			x::sub_8DD7E0(mt, (uint32_t)(int32_t)pitch16);
			const uint32_t r3 = x::CrtRand();
			mt += 0x20;
			U16(spin, 0) = (uint16_t)(r3 & 0xFFF);
			spin += 2;
		}
		if (S16(node, 0x24) > 0xF)
		{
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ab_7D4F80(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_StreamDrops_State0_Init);
		states[1] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_StreamDrops_State1_Animate);
		states[2] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_StreamDrops_State2_Nullsub);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x150) = (uint16_t)(U16(node, 0x150) + 0x10);
		uint32_t mt = node + 0xC0;
		uint32_t spin = node + 0x148;
		for (uint32_t tbl = game_addr(DROP_SPRITES); tbl < game_addr(DROP_SPRITES) + 0x10; tbl += 4, mt += 0x20, spin += 2)
		{
			alignas(4) uint16_t v[4];
			alignas(4) uint8_t m[0x20];
			const uint16_t s = (uint16_t)((U16(spin, 0) + 0x100) & 0xFFF);
			v[0] = 0;
			U16(spin, 0) = s;
			U16(spin, -8) = (uint16_t)(U16(spin, -8) + U16(node, 0x152));
			v[2] = U16(spin, -8);
			v[1] = U16(node, 0x150);
			v[3] = 0;
			ff8fx::copy<(0x20)>(m, (const void *)mt);
			x::sub_8DD960(P(m), (uint32_t)(int32_t)(int16_t)s);
			x::GTE_SetRotMatrix(P(m));
			x::GTE_LoadV0(P(v));
			x::GTE_MVMVA_RotV0();
			x::GTE_StoreIR123(node + 0x1C);
			U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + U16(node, 0x78));
			U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + U16(node, 0x7A));
			U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + U16(node, 0x7C));
			U32(node, 0x4C) = MEM<uint32_t>(tbl);
			ab_7D50D0(node);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ab_7D57A0(uint32_t a1)
	{
		const uint32_t node = a1;
		const int32_t v = S32(MEM<uint32_t>(game_addr(DIR_PTR)), 0x3C);
		int32_t hi = (int32_t)(((int64_t)v * (int64_t)(int32_t)0xD5555555) >> 32);
		hi >>= 1;
		const int32_t q = add32(hi, (int32_t)((uint32_t)hi >> 31));
		U32(node, 0x4C) = game_addr(game::unk_1589D90);
		const uint8_t st = U8(node, 0x29);
		U16(node, 0x56) = 0xFFF0;
		U16(node, 0x52) = 0xC;
		U16(node, 0x152) = (uint16_t)q;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl ab_7D58B0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 4)
			return 0;
		const uint32_t caster = ENT(U8(node, 0x2C));
		const uint32_t target = ENT(U8(node, 0x2D));
		x::MAG_022_sub_8DD770(node + 0x58);
		x::MAG_022_sub_8DD8A0(node + 0x58, (uint32_t)(int32_t)S16(caster, 0xE));
		const int32_t r = (int32_t)x::CrtRand() % 3;
		if (r == 0)
			x::queueChainTransformationConditional(target, 4);
		else if (r == 1)
			x::queueChainTransformationConditional(target, 5);
		else if (r == 2)
			x::queueChainTransformationConditional(target, 6);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ab_7D5940(uint32_t a1)
	{
		const uint32_t node = a1;
		if (((U8(node, 0x2D) + U8(node, 0x24)) & 1) != 0)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TASKS), game_addr(ORIG_SplashDrops), 0x154, node);
			ab_7D5A70(ENT(U8(node, 0x2D)), t + 0x80, 2);
			uint32_t vel = t + 0xA0;
			for (int32_t n = 2; n != 0; n--, vel += 8)
			{
				const uint32_t r1 = x::CrtRand();
				const int32_t pitch = (int32_t)(r1 & 0x1FF) + 0xE00;
				const uint32_t r2 = x::CrtRand();
				const int16_t yaw = (int16_t)((r2 & 0x3FF) - 0x200);
				alignas(4) uint8_t m[0x20];
				alignas(4) uint16_t v[4];
				ff8fx::copy<(0x20)>(m, (const void *)(node + 0x58));
				x::MAG_022_sub_8DD8A0(P(m), (uint32_t)(int32_t)yaw);
				x::sub_8DD7E0(P(m), (uint32_t)(int32_t)(int16_t)pitch);
				v[0] = 0;
				v[1] = 0;
				const uint32_t r3 = x::CrtRand();
				v[2] = (uint16_t)(0xFFFFFC00u - (r3 & 0x7FF));
				v[3] = 0;
				x::GTE_SetRotMatrix(P(m));
				x::GTE_LoadV0(P(v));
				x::GTE_MVMVA_RotV0();
				x::GTE_StoreIR123(vel);
			}
		}
		if (S16(node, 0x24) >= 0x12)
		{
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ab_7D5A70(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		int32_t n = (int16_t)a3;
		const int32_t bones = (int16_t)(uint16_t)U8(U32(U32(a1, 0x64), 0), 0);
		if (n <= 0)
			return 0;
		uint32_t out = a2;
		for (; n != 0; n--, out += 8)
		{
			const int32_t r1 = (int32_t)x::CrtRand();
			const int32_t bone = r1 % bones;
			const uint32_t r2 = x::CrtRand();
			x::GetEffectSpawnPosition(a1, (uint32_t)(int32_t)(int16_t)bone, (uint32_t)(int32_t)(int16_t)(r2 & 0xFFF), out);
		}
		return 0;
	}

	static inline void drop_step(uint32_t vel)
	{
		for (int k = 0; k < 3; k++)
		{
			const int16_t c = S16(vel, 2 * k);
			const int32_t e = c;
			U16(vel, 2 * k) = (uint16_t)(c - (uint16_t)((e + ((e >> 31) & 7)) >> 3));
		}
		for (int k = 0; k < 3; k++)
		{
			const int32_t e = S16(vel, 2 * k);
			U16(vel, -0x20 + 2 * k) = (uint16_t)(U16(vel, -0x20 + 2 * k) + (uint16_t)((e + ((e >> 31) & 0xF)) >> 4));
		}
	}

	uint32_t __cdecl ab_7D5AD0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_SplashDrops_State0_Init);
		states[1] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_SplashDrops_State1_Animate);
		states[2] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_SplashDrops_State2_Nullsub);
		callp(states[S8(node, 0x29)], node);
		uint32_t vel = node + 0xA0;
		for (int32_t n = 2; n != 0; n--, vel += 8)
		{
			U32(node, 0x1C) = U32(vel, -0x20);
			U32(node, 0x20) = U32(vel, -0x1C);
			ab_7D50D0(node);
			if ((U8(node, 0x26) & 4) == 0)
				g_pad_residue = (uint16_t)((MEM<uint32_t>(game_addr(FALLOC_PTR)) + 0xCC) >> 16);
			U16(vel, 2) = (uint16_t)(U16(vel, 2) + 0x50);
			drop_step(vel);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ab_7D5BE0(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U32(a1, 0x4C) = game_addr(game::unk_158A1B0);
		U16(a1, 0x56) = 0xFFC0;
		U16(a1, 0x52) = 0xF;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl ab_7D5CA0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 0x14)
			return 0;
		const uint32_t ent = ENT(U8(node, 0x2C));
		x::MAG_022_sub_8DD770(node + 0x58);
		x::MAG_022_sub_8DD8A0(node + 0x58, (uint32_t)(int32_t)S16(ent, 0xE));
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ab_7D5CF0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (((U8(node, 0x2D) + U8(node, 0x24)) & 1) != 0)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TASKS), game_addr(ORIG_RainDrops), 0x154, node);
			ab_7D5A70(ENT(U8(node, 0x2D)), t + 0x80, 4);
			uint32_t pos = t + 0x80;
			for (int32_t n = 4; n != 0; n--, pos += 8)
			{
				const uint32_t r1 = x::CrtRand();
				U16(pos, 0) = (uint16_t)(U16(pos, 0) + (uint16_t)((r1 & 0x3FF) - 0x200));
				const uint32_t r2 = x::CrtRand();
				U16(pos, 2) = (uint16_t)(U16(pos, 2) + (uint16_t)((r2 & 0x3FF) - 0x300));
				const uint32_t r3 = x::CrtRand();
				U16(pos, 4) = (uint16_t)(U16(pos, 4) + (uint16_t)((r3 & 0x1FF) - 0x100));
				const uint32_t r4 = x::CrtRand();
				const int16_t pitch = (int16_t)(r4 & 0xFFF);
				const uint32_t r5 = x::CrtRand();
				const int16_t yaw = (int16_t)(r5 & 0xFFF);
				alignas(4) uint8_t m[0x20];
				alignas(4) uint16_t v[4];
				ff8fx::copy<(0x20)>(m, (const void *)(node + 0x58));
				x::MAG_022_sub_8DD8A0(P(m), (uint32_t)(int32_t)yaw);
				x::sub_8DD7E0(P(m), (uint32_t)(int32_t)pitch);
				v[0] = 0;
				v[1] = 0;
				const uint32_t r6 = x::CrtRand();
				v[2] = (uint16_t)(0u - (r6 & 0x3FF));
				v[3] = 0;
				x::GTE_SetRotMatrix(P(m));
				x::GTE_LoadV0(P(v));
				x::GTE_MVMVA_RotV0();
				x::GTE_StoreIR123(pos + 0x20);
			}
		}
		if (S16(node, 0x24) >= 0x1C)
		{
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ab_7D5E50(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_RainDrops_State0_Init);
		states[1] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_RainDrops_State1_Animate);
		states[2] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_RainDrops_State2_Nullsub);
		callp(states[S8(node, 0x29)], node);
		uint32_t vel = node + 0xA0;
		for (uint32_t tbl = game_addr(RAIN_SPRITES); tbl < game_addr(RAIN_SPRITES) + 0x10; tbl += 4, vel += 8)
		{
			U32(node, 0x1C) = U32(vel, -0x20);
			U32(node, 0x20) = U32(vel, -0x1C);
			U32(node, 0x4C) = MEM<uint32_t>(tbl);
			ab_7D50D0(node);
			if ((U8(node, 0x26) & 4) == 0)
				g_pad_residue = (uint16_t)((MEM<uint32_t>(game_addr(FALLOC_PTR)) + 0xCC) >> 16);
			drop_step(vel);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ab_7D5F60(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U32(a1, 0x4C) = game_addr(game::unk_1589EF0);
		U16(a1, 0x56) = 0xFFF0;
		U16(a1, 0x52) = 0xC;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl ab_7D1FD0(uint32_t a1)
	{
		g_mod = &MOD_072;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_CameraCopyPtr)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_CameraCopyPtr2)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Master_State0_Next);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Master_State1_Next);
		states[2] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Master_State2_WaitChildren);
		states[3] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Master_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Master_State4_NextAction);
		states[5] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Master_State5_Next);
		states[6] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Master_State6_WaitTasks);
		states[7] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Master_State7_Next);
		states[8] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Master_State8_Next);
		states[9] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Master_State9_Finish);
		states[10] = game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Master_State10_Nullsub);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_ArenaBaseOdd));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Arena2BaseEven));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_PacketCursor2)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_ArenaBaseEven));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_Arena2BaseOdd));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_PacketCursor2)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_EMITTER)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_DIRECTOR)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_TASKS)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_ORB)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_SPARE)));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ab_7D2140(uint32_t a1)
	{
		if (U8(a1, 0x28) == 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl ab_7D2200(uint32_t a1)
	{
		ab_7D2230(a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_DirectorQueue), game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_DirectorTask), 0x48, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ab_7D2370(uint32_t a1)
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

	uint32_t __cdecl ab_7D24B0(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			ab_7D27F0(a1);
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

	uint32_t __cdecl ab_7D2620(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0x200);
		if (S16(a1, 0x1C) >= 0x600)
		{
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0x1C) = 0x600;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int i = 4; i != 0; i--, p += 0x2C)
			MEM<uint16_t>(p) = level;
		return 0;
	}

	uint32_t __cdecl ab_7D2660(uint32_t a1)
	{
		if (S16(a1, 0x24) > 0x28)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl ab_7D2670(uint32_t a1)
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

	uint32_t __cdecl ab_7D2720(uint32_t a1)
	{
		uint32_t r = a_73B7E0(1);
		if (r == 0) return 0;
		x::BdPlaySE(game_addr(game::MAG_072_AQUA_BREATH_QUISTIS_SoundData), 0, 0x80);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl ab_7D2770(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(PHASE_DATA));
		if (S16(d, 0x46) > 0x23)
		{
			U16(d, 0x48) = 1;
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl ab_7D29A0(uint32_t a1)
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
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = ab_7D2AD0(hdr, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl ab_7D2AD0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		static const Draw lists[8] = { ab_7D2C00, ab_7D2E20, ab_7D30B0, ab_7D3530, a_73D770, a_73D9C0, ab_7D3FE0, ab_7D44E0 };
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

	uint32_t __cdecl ab_7D2C00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ab_7D2E20(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ab_7D30B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ab_7D3530(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ab_7D3FE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ab_7D44E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ab_7D4D60(uint32_t a1)
	{
		ab_7D4C80(a1 + 0x1C, (uint32_t)U8(a1, 0x2C));
		U16(a1, 0x5C) = (uint16_t)(U16(a1, 0x5C) - 1);
		if (S16(a1, 0x5C) <= 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ab_7D4D90(uint32_t a1)
	{
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0x200);
		if (S16(a1, 0x50) >= 0x1000)
		{
			int8_t st = S8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U16(a1, 0x50) = 0x1000;
			S8(a1, 0x29) = (int8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl ab_7D50D0(uint32_t a1)
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

	uint32_t __cdecl ab_7D57E0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl ab_7D5C00(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl ab_7D5F80(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Master_State0_Next, (void *)a_73A0D0, "072 MAG_072_sub_7D2120" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Master_State1_Next, (void *)a_73A0D0, "072 MAG_072_sub_7D2130" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Master_State3_SpawnEmitter, (void *)a_73A170, "072 MAG_072_sub_7D2150" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_EmitterTask, (void *)a_73A1A0, "072 MAG_072_sub_7D2180" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_GetMonstersCentre, (void *)a_73A720, "072 sub_7D22E0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_StageDarkenTask, (void *)a_73F110, "072 sub_7D2570" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_StageDarken_State0_Clear, (void *)a_7335D0, "072 sub_7D25E0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_StageDarken_State4_Nullsub, (void *)a_73A6D0, "072 nullsub_1417" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Director_RequestPhase, (void *)a_73B640, "072 sub_7D26F0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Director_PhaseReached, (void *)a_73B7E0, "072 sub_7D2750" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Director_State4_CuePhase2, (void *)a_73B8A0, "072 sub_7D2790" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Director_State5_Finish, (void *)a_7475A0, "072 sub_7D27B0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Director_State6_Nullsub, (void *)a_73A6D0, "072 nullsub_1418" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Nullsub_7D27E0, (void *)a_73A6D0, "072 nullsub_1419" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_RenderListG3, (void *)a_73D770, "072 sub_7D3AB0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_RenderListG4, (void *)a_73D9C0, "072 sub_7D3D00" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Orb_State4_Nullsub, (void *)a_73A6D0, "072 nullsub_1420" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_StreamTask, (void *)a_73A380, "072 sub_7D4DD0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_InitSpriteSequence, (void *)a_7435E0, "072 InitEffectSequenceFromData_c11" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_DrawSpriteQuads, (void *)a_743720, "072 sub_7D5330" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_StepSpriteFrame, (void *)a_743C20, "072 sub_7D5800" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_StreamDrops_State2_Nullsub, (void *)a_73A6D0, "072 nullsub_1421" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Nullsub_7D5840, (void *)a_73A6D0, "072 nullsub_1422" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_SplashTask, (void *)a_73A380, "072 sub_7D5850" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_SplashDrops_State2_Nullsub, (void *)a_73A6D0, "072 nullsub_1423" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Nullsub_7D5C30, (void *)a_73A6D0, "072 nullsub_1424" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_RainTask, (void *)a_73A380, "072 sub_7D5C40" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_RainDrops_State2_Nullsub, (void *)a_73A6D0, "072 nullsub_1425" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Nullsub_7D5FB0, (void *)a_73A6D0, "072 nullsub_1426" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Emitter_State1_ApplyDamageOnCue, (void *)a_747440, "072 MAG_072_sub_7D5FC0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Emitter_State2_WaitChildren, (void *)a_7474B0, "072 MAG_072_sub_7D6030" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Emitter_State3_Finish, (void *)a_7474D0, "072 MAG_072_sub_7D6050" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Emitter_State4_Nullsub, (void *)a_73A6D0, "072 nullsub_1427" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Master_State4_NextAction, (void *)a_747500, "072 MAG_072_sub_7D6080" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Master_State5_Next, (void *)a_73A0D0, "072 MAG_072_sub_7D60C0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Master_State6_WaitTasks, (void *)a_747550, "072 MAG_072_sub_7D60D0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Master_State7_Next, (void *)a_73A0D0, "072 MAG_072_sub_7D60E0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Master_State8_Next, (void *)a_73A0D0, "072 MAG_072_sub_7D60F0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Master_State9_Finish, (void *)a_7475A0, "072 MAG_072_sub_7D6100" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Master_State10_Nullsub, (void *)a_73A6D0, "072 nullsub_1416" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_072_AQUA_BREATH_QUISTIS_MasterTask, (void *)ab_7D1FD0, "072 ab_7D1FD0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Master_State2_WaitChildren, (void *)ab_7D2140, "072 ab_7D2140" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Emitter_State0_SpawnDirector, (void *)ab_7D2200, "072 ab_7D2200" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_InitDirectorBlock, (void *)ab_7D2230, "072 ab_7D2230" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_GetFarthestTargetDistance, (void *)ab_7D2370, "072 ab_7D2370" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_DirectorTask, (void *)ab_7D2430, "072 ab_7D2430" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Director_PhaseBookkeeping, (void *)ab_7D24B0, "072 ab_7D24B0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Director_State0_DarkenAndCastAnim, (void *)ab_7D2500, "072 ab_7D2500" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_StageDarken_State1_RampUp, (void *)ab_7D2620, "072 ab_7D2620" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_StageDarken_State2_Hold, (void *)ab_7D2660, "072 ab_7D2660" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_StageDarken_State3_RampDown, (void *)ab_7D2670, "072 ab_7D2670" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Director_State1_CuePhase1, (void *)ab_7D26C0, "072 ab_7D26C0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Director_State2_PlaySound, (void *)ab_7D2720, "072 ab_7D2720" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Director_State3_DamageCue, (void *)ab_7D2770, "072 ab_7D2770" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_SpawnPhaseTasks, (void *)ab_7D27F0, "072 ab_7D27F0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_OrbTask, (void *)ab_7D28F0, "072 ab_7D28F0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_DrawOrb, (void *)ab_7D29A0, "072 ab_7D29A0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_RenderPrimModel, (void *)ab_7D2AD0, "072 ab_7D2AD0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_RenderListF3, (void *)ab_7D2C00, "072 ab_7D2C00" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_RenderListF4, (void *)ab_7D2E20, "072 ab_7D2E20" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_RenderListFT3, (void *)ab_7D30B0, "072 ab_7D30B0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_RenderListFT4, (void *)ab_7D3530, "072 ab_7D3530" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_RenderListGT3, (void *)ab_7D3FE0, "072 ab_7D3FE0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_RenderListGT4, (void *)ab_7D44E0, "072 ab_7D44E0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_OrbWaveVertices, (void *)ab_7D4B30, "072 ab_7D4B30" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Orb_State0_Appear, (void *)ab_7D4C20, "072 ab_7D4C20" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_GetCasterAnchor, (void *)ab_7D4C80, "072 ab_7D4C80" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Orb_State1_FadeIn, (void *)ab_7D4D20, "072 ab_7D4D20" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Orb_State2_Hold, (void *)ab_7D4D60, "072 ab_7D4D60" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Orb_State3_FadeOut, (void *)ab_7D4D90, "072 ab_7D4D90" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Stream_State0_Facing, (void *)ab_7D4E30, "072 ab_7D4E30" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Stream_State1_SpawnDrops, (void *)ab_7D4E70, "072 ab_7D4E70" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_StreamDropsTask, (void *)ab_7D4F80, "072 ab_7D4F80" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_DrawDropSprite, (void *)ab_7D50D0, "072 ab_7D50D0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_StreamDrops_State0_Init, (void *)ab_7D57A0, "072 ab_7D57A0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_StreamDrops_State1_Animate, (void *)ab_7D57E0, "072 ab_7D57E0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Splash_State0_HitAnim, (void *)ab_7D58B0, "072 ab_7D58B0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Splash_State1_SpawnDrops, (void *)ab_7D5940, "072 ab_7D5940" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_GetRandomBonePositions, (void *)ab_7D5A70, "072 ab_7D5A70" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_SplashDropsTask, (void *)ab_7D5AD0, "072 ab_7D5AD0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_SplashDrops_State0_Init, (void *)ab_7D5BE0, "072 ab_7D5BE0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_SplashDrops_State1_Animate, (void *)ab_7D5C00, "072 ab_7D5C00" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Rain_State0_Init, (void *)ab_7D5CA0, "072 ab_7D5CA0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_Rain_State1_SpawnDrops, (void *)ab_7D5CF0, "072 ab_7D5CF0" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_RainDropsTask, (void *)ab_7D5E50, "072 ab_7D5E50" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_RainDrops_State0_Init, (void *)ab_7D5F60, "072 ab_7D5F60" },
		{ game::MAG_072_AQUA_BREATH_QUISTIS_RainDrops_State1_Animate, (void *)ab_7D5F80, "072 ab_7D5F80" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag072_aqua_breath_quistis()
	{
		act::register_module(72);
		for (const act::aqua72::ModPort *p = act::aqua72::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(72, p->addr, p->port, p->name);
		for (const act::aqua72::ModPort *p = act::aqua72::PORTS; p->addr; p++)
			act::register_module_port(72, p->addr, p->port, p->name);
	}
}

