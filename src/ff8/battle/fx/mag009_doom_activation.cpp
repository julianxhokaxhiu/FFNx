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
namespace doomact
{
	static const Mod MOD_009 = { "doomactivation", 9, game::MAG_009_DOOM_ACTIVATION_FL, game::MAG_008_DOOM_FL,
		{ {}, {}, game::unk_1634B7C, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_009_DoomActivation_TextureFile, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::unk_2757450, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_009_EmitterTask, game::MAG_009_Emitter_State0_InitDirector, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_009_DirSyncGate, {}, game::MAG_009_DirPhaseGE, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_009_SpriteStrip_DrawQuads, {}, game::MAG_009_Emitter_State1_ApplyDamage, game::MAG_009_Emitter_State2_WaitChildren, game::MAG_009_Emitter_State3_Finish, game::MAG_009_Emitter_State4_Empty, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id DIR_PTR = game::unk_1634B7C;
	static constexpr game::Id PHASE_DATA = DIR_PTR;
	static constexpr game::Id PACKET_CURSOR = game::MAG_009_DoomActivation_PacketCursors;
	static constexpr game::Id FRAME_CURSOR = game::BATTLE_PACKET_WRITE_CURSOR;
	static constexpr game::Id SOUND = game::unk_1634B80;
	static constexpr game::Id BLEND_BUF = game::MAG_009_DoomActivation_BlendBuffer;
	static constexpr game::Id PRIM_CB = game::MAG_009_PrimPlayer_DrawCallback;
	static constexpr game::Id ANCHOR = game::MAG_009_DoomActivation_SinkingAnchor;
	static constexpr game::Id BODY_CTX = game::MAG_009_DoomActivation_RenderContext;
	static constexpr game::Id BODY_VERTS = game::MAG_009_DoomActivation_VertexScratch;
	static constexpr game::Id BODY_BLOCK = game::MAG_009_DoomActivation_RenderBlock;
	static constexpr game::Id Q_EMITTER = game::unk_2757450, Q_DIRECTOR = game::unk_274F128, Q_PHASE = game::unk_274F230, Q_PARTICLE = game::unk_274F628, Q_PRIM = game::unk_27571A0, Q_TINT = game::unk_2757440;
	static constexpr game::Id ORIG_Master = game::MAG_009_Master_Task;
	static constexpr game::Id ORIG_DarkenIn = game::MAG_009_StageDarkenIn_Task;
	static constexpr game::Id ORIG_DoomModel = game::MAG_009_DoomModel_Task;
	static constexpr game::Id ORIG_Sink = game::MAG_009_Sink_Task;
	static constexpr game::Id ORIG_DarkenOut = game::MAG_009_StageDarkenOut_Task;
	static constexpr game::Id ORIG_SinkModel = game::MAG_009_SinkModel_Task;
	static constexpr game::Id ORIG_Spawner = game::MAG_009_ParticleSpawner_Task;
	static constexpr game::Id ORIG_Particle = game::MAG_009_Particle_Task;
	static inline uint32_t ENT(uint32_t slot) { return game_addr(game::BattleEntitySlotData) + slot * 0x9C; }

	namespace gx
	{
		static inline uint32_t GTE_SetLightMatrix(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_SetLightMatrix))(a1); }
		static inline uint32_t GTE_SetBackgroundVector(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetBackgroundVector))(a1, a2, a3); }
		static inline uint32_t GTE_MVMVA_LightV0_Bk() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
		static inline uint32_t GTE_LoadV012Dwords(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadV012Dwords))(a1); }
		static inline uint32_t GTE_StoreSXY012_PolyGT3(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreSXY012_PolyGT3))(a1); }
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

	static inline void da_sink_step(uint32_t node)
	{
		const uint16_t v = U16(node, 0x6E);
		MEM<uint16_t>(game_addr(BODY_CTX) + 0x54) = (uint16_t)(MEM<uint16_t>(game_addr(BODY_CTX) + 0x54) + v);
		MEM<uint16_t>(game_addr(ANCHOR) + 2) = (uint16_t)(MEM<uint16_t>(game_addr(ANCHOR) + 2) + v);
	}
	static inline void da_particle_place(uint32_t node)
	{
		alignas(4) uint8_t loc[0x38] = {};
		const uint32_t L = P(loc);
		const uint32_t M = L + 0x18, vin = L + 4, vout = L + 0xC;
		x::MAG_022_sub_8DD770(M);
		x::sub_8DD960(M, (uint32_t)(int32_t)S16(node, 0x44));
		x::sub_8DD7E0(M, (uint32_t)(int32_t)S16(node, 0x40));
		x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)S16(node, 0x42));
		U16(vin, 0) = 0;
		U16(vin, 2) = 0;
		U16(vin, 4) = 0xC0;
		x::matrixMultiplyVector(M, vin, vout);
		U16(node, 0x1C) = (uint16_t)(MEM<uint16_t>(game_addr(ANCHOR)) + U16(vout, 0));
		U16(node, 0x1E) = (uint16_t)(MEM<uint16_t>(game_addr(ANCHOR) + 2) + U16(vout, 2));
		U16(node, 0x20) = (uint16_t)(MEM<uint16_t>(game_addr(ANCHOR) + 4) + U16(vout, 4));
	}

	uint32_t __cdecl da_8C0500(uint32_t a1);
	uint32_t __cdecl da_8C0690(uint32_t a1);
	uint32_t __cdecl da_8C0750(uint32_t a1);
	uint32_t __cdecl da_8C0780(uint32_t a1);
	uint32_t __cdecl da_8C0800(uint32_t a1);
	uint32_t __cdecl da_8C08A0(uint32_t a1);
	uint32_t __cdecl da_8C0940(uint32_t a1);
	uint32_t __cdecl da_8C0980(uint32_t a1);
	uint32_t __cdecl da_8C09D0(uint32_t a1);
	uint32_t __cdecl da_8C0A10(uint32_t a1);
	uint32_t __cdecl da_8C0AB0(uint32_t a1);
	uint32_t __cdecl da_8C0B70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl da_8C0C20(uint32_t a1);
	uint32_t __cdecl da_8C0C90(uint32_t a1);
	uint32_t __cdecl da_8C0DC0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl da_8C1080(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl da_8C11B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl da_8C13D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl da_8C1660(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl da_8C1B50(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl da_8C2670(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl da_8C2BD0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl da_8C32A0(uint32_t a1);
	uint32_t __cdecl da_8C3370(uint32_t a1);
	uint32_t __cdecl da_8C3420(uint32_t a1);
	uint32_t __cdecl da_8C3440(uint32_t a1);
	uint32_t __cdecl da_8C3490(uint32_t a1);
	uint32_t __cdecl da_8C3520(uint32_t a1);
	uint32_t __cdecl da_8C3620(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl da_8C3B30(uint32_t a1);
	uint32_t __cdecl da_8C3B80(uint32_t a1);
	uint32_t __cdecl da_8C3C90(uint32_t a1);
	uint32_t __cdecl da_8C3CF0(uint32_t a1);
	uint32_t __cdecl da_8C3D40(uint32_t a1);
	uint32_t __cdecl da_8C3D90(uint32_t a1);
	uint32_t __cdecl da_8C3E00(uint32_t a1);
	uint32_t __cdecl da_8C3F00(uint32_t a1);
	uint32_t __cdecl da_8C45D0(uint32_t a1);
	uint32_t __cdecl da_8C4620(uint32_t a1);
	uint32_t __cdecl da_8C4690(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl da_8C46F0(uint32_t a1);
	uint32_t __cdecl da_8C4720(uint32_t a1);
	uint32_t __cdecl da_8C47A0(uint32_t a1);
	uint32_t __cdecl da_8C47D0(uint32_t a1);
	uint32_t __cdecl da_8C4800(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace doomact
{
	uint32_t __cdecl da_8C0500(uint32_t a1)
	{
		g_mod = &MOD_009;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::unk_27584C4)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_274F150)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_009_Master_State0_Advance);
		states[1] = game_addr(game::MAG_009_Master_State1_Advance);
		states[2] = game_addr(game::MAG_009_Master_State2_WaitNoChildren);
		states[3] = game_addr(game::MAG_009_Master_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_009_Master_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_009_Master_State5_Advance);
		states[6] = game_addr(game::MAG_009_Master_State6);
		states[7] = game_addr(game::MAG_009_Master_State7_Request);
		states[8] = game_addr(game::MAG_009_Master_State8_WaitRequest);
		states[9] = game_addr(game::MAG_009_Master_State9_Finish);
		states[10] = game_addr(game::MAG_009_Master_State10_Empty);
		if ((U8(node, 0x5C) & 1) != 0)
		{
			MEM<uint32_t>(game_addr(game::MAG_009_DoomActivation_PacketCursors)) = MEM<uint32_t>(game_addr(game::unk_27575C4));
			MEM<uint32_t>(game_addr(game::unk_274F14C)) = MEM<uint32_t>(game_addr(game::unk_27571B4));
		}
		else
		{
			MEM<uint32_t>(game_addr(game::MAG_009_DoomActivation_PacketCursors)) = MEM<uint32_t>(game_addr(game::unk_27575C0));
			MEM<uint32_t>(game_addr(game::unk_274F14C)) = MEM<uint32_t>(game_addr(game::unk_27571B0));
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		static constexpr game::Id queues[7] = { game::unk_2757450, game::unk_274F128, game::unk_2758160, game::unk_274F230, game::unk_274F628, game::unk_27571A0, game::unk_2757440 };
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

	uint32_t __cdecl da_8C0690(uint32_t a1)
	{
		if (U8(a1, 0x28) == 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl da_8C0750(uint32_t a1)
	{
		da_8C0780(a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_274F128), game_addr(game::MAG_009_DirectorTask), 0x48, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl da_8C0780(uint32_t a1)
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

	uint32_t __cdecl da_8C0800(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[11];
		states[0] = game_addr(game::MAG_009_Director_State0_RequestPhase1);
		states[1] = game_addr(game::MAG_009_Director_State1_WaitFrame5);
		states[2] = game_addr(game::MAG_009_Director_State2_RequestPhase2);
		states[3] = game_addr(game::MAG_009_Director_State3_PlaySoundAtPhase2);
		states[4] = game_addr(game::MAG_009_Director_State4_WaitFrame20);
		states[5] = game_addr(game::MAG_009_Director_State5_RequestPhase3);
		states[6] = game_addr(game::MAG_009_Director_State6_WaitPhase3);
		states[7] = game_addr(game::MAG_009_Director_State7_WaitPhase4);
		states[8] = game_addr(game::MAG_009_Director_State8_RequestPhase5);
		states[9] = game_addr(game::MAG_009_Director_State9_Finish);
		states[10] = game_addr(game::MAG_009_Director_State10_Empty);
		da_8C08A0(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl da_8C08A0(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			da_8C0AB0(a1);
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

	uint32_t __cdecl da_8C0940(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x46) >= 5)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl da_8C0980(uint32_t a1)
	{
		if (a_73B7E0(2) == 0)
			return 0;
		x::BdPlaySE(game_addr(SOUND), 0, 0x80);
		next_state(a1);
		return 0;
	}

	uint32_t __cdecl da_8C09D0(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x46) >= 0x20)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl da_8C0A10(uint32_t a1)
	{
		if (a_73B7E0(3) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl da_8C0AB0(uint32_t a1)
	{
		const int32_t ph = (int32_t)S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x40) - 1;
		switch (ph)
		{
		case 0:
			return x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PHASE), game_addr(ORIG_DarkenIn), 0x7C, a1);
		case 1:
		{
			da_8C0B70(a1, game_addr(ORIG_DoomModel), game_addr(game::unk_18155F0), 0x348, 0, 0);
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TINT), game_addr(game::Effect_ScreenTintTask), 0x40, a1);
			U32(t, 0x34) = game_addr(game::unk_1634F00);
			U16(t, 0x38) = 0x11;
			return t;
		}
		case 2:
			return x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PHASE), game_addr(ORIG_Sink), 0x7C, a1);
		case 3:
			return x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PHASE), game_addr(ORIG_DarkenOut), 0x7C, a1);
		default:
			return (uint32_t)ph;
		}
	}

	uint32_t __cdecl da_8C0B70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PRIM), a2, 0x3E4, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		U32(t, 0x78) = (uint32_t)(int32_t)(int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl da_8C0C20(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		x::GetEffectSpawnPosition(ENT(U8(node, 0x2D)), 0xF1, 0, node + 0x1C);
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		U16(node, 0x88) = 0xFE00;
		da_8C0C90(node);
		next_state(node);
		return 0;
	}

	uint32_t __cdecl da_8C0C90(uint32_t a1)
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

	uint32_t __cdecl da_8C0DC0(uint32_t a1, uint32_t a2, uint32_t a3)
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

		if ((U32(rec, 4) & 0x400) != 0)
			x::MAG_117_sub_701430(rec + 0x10, M);
		else
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
				U16(S, 0) = U16(rec, 0x18);
				U16(S, 8) = U16(rec, 0x1A);
				U16(S, 0x10) = U16(rec, 0x1C);
				U16(S, 0xA) = 0;
				U16(S, 4) = 0;
				U16(S, 0xE) = 0;
				U16(S, 2) = 0;
				U16(S, 0xC) = 0;
				U16(S, 6) = 0;
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
		const uint32_t cursor = MEM<uint32_t>(game_addr(FRAME_CURSOR));
		U32(hdr, 0x10) = (uint32_t)(int32_t)S16(blk, 0x58);
		U16(hdr, 0x22) = 0x100;
		U16(hdr, 0x20) = 0x100;
		U16(hdr, 0x2A) = 0x100;
		U16(hdr, 0x28) = 0x100;
		MEM<uint32_t>(game_addr(FRAME_CURSOR)) = da_8C1080(hdr, ot, 2, cursor);
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl da_8C1080(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		static const Draw lists[8] = { da_8C11B0, da_8C13D0, da_8C1660, da_8C1B50, a_73D770, a_73D9C0, da_8C2670, da_8C2BD0 };
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

	uint32_t __cdecl da_8C11B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl da_8C13D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl da_8C1660(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4u, vbase + (uint32_t)U16(rec, 6) * 4u,
				vbase + (uint32_t)U16(rec, 8) * 4u);
			x::GTE_RTPT();
			{
				const uint32_t flags = U32(ctx, 0x14);
				const uint32_t code = U32(rec, 0);
				U32(pkt, 0) = 0x7000000;
				U32(pkt, 4) = code;
				if (flags & 1)
					U32(pkt, 4) = code | 0x2000000;
				if (flags & 4)
					U32(pkt, 4) = U32(pkt, 4) & 0xFDFFFFFFu;
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
				x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
				x::GTE_AVSZ3();
				if (t2_outx(pkt + 8))
					clip = 1;
				if (t2_outx(pkt + 0x10))
					clip |= 2;
				if (t2_outx(pkt + 0x18))
					clip |= 4;
				if (t2_outy(pkt + 0xA))
					clip |= 0x10;
				if (t2_outy(pkt + 0x12))
					clip |= 0x20;
				if (t2_outy(pkt + 0x1A))
					clip |= 0x40;
				if ((clip & 7) == 7)
					goto next;
				if ((clip & 0x70) == 0x70)
					goto next;
			}
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x40)
				t2_depth_cue(ctx, pkt + 4);
			{
				const uint32_t ot = t2_ot(ctx, a2, a3);
				const uint16_t add0 = U16(ctx, 0x18);
				const uint16_t add1 = U16(ctx, 0x1A);
				if ((uint16_t)(add0 | add1) != 0)
				{
					if (add0 != 0)
					{
						const uint32_t a = add0;
						const uint32_t c2 = U8(pkt, 0x1C) + a;
						const uint32_t c0 = U8(pkt, 0xC) + a;
						const uint32_t c1 = U8(pkt, 0x14) + a;
						uint8_t s = 0;
						if ((int32_t)(c2 | c1 | c0) > 0xFF)
							s = U8(ctx, 0x28);
						U8(pkt, 0xC) = (uint8_t)(c0 - s);
						U8(pkt, 0x14) = (uint8_t)(c1 - s);
						U8(pkt, 0x1C) = (uint8_t)(c2 - s);
					}
					const uint16_t add1b = U16(ctx, 0x1A);
					if (add1b != 0)
					{
						const uint32_t a = add1b;
						const uint32_t c2 = U8(pkt, 0x1D) + a;
						const uint32_t c0 = U8(pkt, 0xD) + a;
						const uint32_t c1 = U8(pkt, 0x15) + a;
						uint8_t s = 0;
						if ((int32_t)(c2 | c1 | c0) > 0xFF)
							s = U8(ctx, 0x2A);
						U8(pkt, 0xD) = (uint8_t)(c0 - s);
						U8(pkt, 0x15) = (uint8_t)(c1 - s);
						U8(pkt, 0x1D) = (uint8_t)(c2 - s);
					}
					const uint32_t poly = pkt;
					uint32_t prim = pkt + 0x20;
					pkt += 0x2C;
					U32(prim, 0) = 0x2000000;
					uint32_t tw = 0;
					if (ctx + 0x1C != 0)
						tw = t2_twin(ctx, 0x1C);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
					prim = pkt;
					pkt += 0xC;
					U32(prim, 0) = 0x2000000;
					tw = 0;
					if (ctx + 0x24 != 0)
						tw = t2_twin(ctx, 0x24);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
				}
				else
				{
					const uint32_t poly = pkt;
					const uint32_t mode = pkt + 0x20;
					pkt += 0x2C;
					const uint32_t tpage = x::sub_45C690(0, 1, 0, 0);
					x::sub_45BFC0(mode, 0, 0, tpage & 0xFFFF, 0);
					x::SSIGPU_InsertPrimAutoDepth(ot, mode);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
				}
			}
		next:
			rec += 0x14;
			--count;
		} while (count != 0);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl da_8C1B50(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4u, vbase + (uint32_t)U16(rec, 6) * 4u,
				vbase + (uint32_t)U16(rec, 8) * 4u);
			x::GTE_RTPT();
			{
				const uint32_t flags = U32(ctx, 0x14);
				const uint32_t code = U32(rec, 0);
				U32(pkt, 0) = 0x9000000;
				U32(pkt, 4) = code;
				if (flags & 1)
					U32(pkt, 4) = code | 0x2000000;
				if (flags & 4)
					U32(pkt, 4) = U32(pkt, 4) & 0xFDFFFFFFu;
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
				x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
				x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0xA) * 4u);
				x::GTE_RTPS();
				if (t2_outx(pkt + 8))
					clip = 1;
				if (t2_outx(pkt + 0x10))
					clip |= 2;
				if (t2_outx(pkt + 0x18))
					clip |= 4;
				if (t2_outy(pkt + 0xA))
					clip |= 0x10;
				if (t2_outy(pkt + 0x12))
					clip |= 0x20;
				if (t2_outy(pkt + 0x1A))
					clip |= 0x40;
				x::GTE_ReadSXY2(pkt + 0x20);
				x::GTE_AVSZ4();
				if (t2_outx(pkt + 0x20))
					clip |= 8;
				if (t2_outy(pkt + 0x22))
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
				const uint16_t add0 = U16(ctx, 0x18);
				const uint16_t add1 = U16(ctx, 0x1A);
				if ((uint16_t)(add0 | add1) != 0)
				{
					if (add0 != 0)
					{
						const uint32_t a = add0;
						const uint32_t c0 = U8(pkt, 0xC) + a;
						const uint32_t c1 = U8(pkt, 0x14) + a;
						const uint32_t c2 = U8(pkt, 0x1C) + a;
						const uint32_t c3 = U8(pkt, 0x24) + a;
						uint8_t s = 0;
						if ((int32_t)(c3 | c2 | c1 | c0) > 0xFF)
							s = U8(ctx, 0x28);
						U8(pkt, 0xC) = (uint8_t)(c0 - s);
						U8(pkt, 0x14) = (uint8_t)(c1 - s);
						U8(pkt, 0x1C) = (uint8_t)(c2 - s);
						U8(pkt, 0x24) = (uint8_t)(c3 - s);
					}
					const uint16_t add1b = U16(ctx, 0x1A);
					if (add1b != 0)
					{
						const uint32_t a = add1b;
						const uint32_t c0 = U8(pkt, 0xD) + a;
						const uint32_t c1 = U8(pkt, 0x15) + a;
						const uint32_t c2 = U8(pkt, 0x1D) + a;
						const uint32_t c3 = U8(pkt, 0x25) + a;
						uint8_t s = 0;
						if ((int32_t)(c3 | c2 | c1 | c0) > 0xFF)
							s = U8(ctx, 0x2A);
						U8(pkt, 0xD) = (uint8_t)(c0 - s);
						U8(pkt, 0x15) = (uint8_t)(c1 - s);
						U8(pkt, 0x1D) = (uint8_t)(c2 - s);
						U8(pkt, 0x25) = (uint8_t)(c3 - s);
					}
					const uint32_t poly = pkt;
					uint32_t prim = pkt + 0x28;
					pkt += 0x34;
					U32(prim, 0) = 0x2000000;
					uint32_t tw = 0;
					if (ctx + 0x1C != 0)
						tw = t2_twin(ctx, 0x1C);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
					prim = pkt;
					pkt += 0xC;
					U32(prim, 0) = 0x2000000;
					tw = 0;
					if (ctx + 0x24 != 0)
						tw = t2_twin(ctx, 0x24);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
				}
				else
				{
					const uint32_t poly = pkt;
					const uint32_t mode = pkt + 0x28;
					pkt += 0x34;
					const uint32_t tpage = x::sub_45C690(0, 1, 0, 0);
					x::sub_45BFC0(mode, 0, 0, tpage & 0xFFFF, 0);
					x::SSIGPU_InsertPrimAutoDepth(ot, mode);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
				}
			}
		next:
			rec += 0x18;
			--count;
		} while (count != 0);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl da_8C2670(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
							const uint32_t dm = pkt + 0x28;
							const uint32_t tp = x::sub_45C690(0, 1, 0, 0);
							x::sub_45BFC0(dm, 0, 0, tp & 0xFFFF, 0);
							x::SSIGPU_InsertPrimAutoDepth(ot, dm);
							x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
							pkt += 0x34;
						}
					}
				}
			}
			rec += 0x1C;
		} while (--count);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl da_8C2BD0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		static const int32_t U_OFF[4] = { 0x0C, 0x18, 0x24, 0x30 };
		static const int32_t V_OFF[4] = { 0x0D, 0x19, 0x25, 0x31 };
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
			U32(pkt, 0) = 0xC000000;
			U32(pkt, 4) = code;
			if (flags & 2)
			{
				code |= 0x2000000;
				U32(pkt, 4) = code;
			}
			if (flags & 8)
				U32(pkt, 4) &= 0xFDFFFFFF;
			const uint32_t uv0 = U32(rec, 0x0C);
			const uint32_t uv1 = U32(rec, 0x10);
			const uint32_t uv23 = U32(rec, 0x14);
			U32(pkt, 0x0C) = uv0;
			U32(pkt, 0x24) = uv23;
			U32(pkt, 0x18) = uv1;
			U32(pkt, 0x30) = uv23 >> 16;
			x::GTE_ReadFLAG(ctx + 0x3C);
			if (!(U32(ctx, 0x3C) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) >= 0 || (U8(ctx, 0x14) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x14, pkt + 0x20);
					x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0x0A) * 4);
					x::GTE_RTPS();
					if (t3_out_x(S16(pkt, 8))) clip = 1;
					if (t3_out_x(S16(pkt, 0x14))) clip |= 2;
					if (t3_out_x(S16(pkt, 0x20))) clip |= 4;
					if (t3_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (t3_out_y(S16(pkt, 0x16))) clip |= 0x20;
					if (t3_out_y(S16(pkt, 0x22))) clip |= 0x40;
					x::GTE_ReadSXY2(pkt + 0x2C);
					x::GTE_AVSZ4();
					if (t3_out_x(S16(pkt, 0x2C))) clip |= 8;
					if (t3_out_y(S16(pkt, 0x2E))) clip |= 0x80;
					if ((uint8_t)(clip & 0xF) != 0xF && (uint8_t)(clip & 0xF0) != 0xF0)
					{
						x::GTE_ReadOTZ(ctx + 0x38);
						if (U8(ctx, 0x14) & 0x80)
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
							const uint32_t c1 = U32(rec, 0x18), c2 = U32(rec, 0x1C), c3 = U32(rec, 0x20);
							U32(pkt, 0x10) = c1;
							U32(pkt, 0x1C) = c2;
							U32(pkt, 0x28) = c3;
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
								t3_scroll(pkt, U_OFF, 4, du, U8(ctx, 0x28));
							const uint16_t dv2 = U16(ctx, 0x1A);
							if (dv2 != 0)
								t3_scroll(pkt, V_OFF, 4, dv2, U8(ctx, 0x2A));
							const uint32_t w1 = pkt + 0x34;
							U32(w1, 0) = 0x2000000;
							const uint32_t t1 = (ctx + 0x1C) != 0 ? t3_texwin(ctx + 0x1C) : 0;
							U32(w1, 4) = t1;
							U32(w1, 8) = 0;
							x::SSIGPU_InsertPrimAutoDepth(ot, w1);
							x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
							const uint32_t w2 = pkt + 0x40;
							U32(w2, 0) = 0x2000000;
							const uint32_t t2 = (ctx + 0x24) != 0 ? t3_texwin(ctx + 0x24) : 0;
							U32(w2, 4) = t2;
							U32(w2, 8) = 0;
							x::SSIGPU_InsertPrimAutoDepth(ot, w2);
							pkt += 0x4C;
						}
						else
						{
							const uint32_t dm = pkt + 0x34;
							const uint32_t tp = x::sub_45C690(0, 1, 0, 0);
							x::sub_45BFC0(dm, 0, 0, tp & 0xFFFF, 0);
							x::SSIGPU_InsertPrimAutoDepth(ot, dm);
							x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
							pkt += 0x40;
						}
					}
				}
			}
			rec += 0x24;
		} while (--count);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl da_8C32A0(uint32_t a1)
	{
		const uint32_t r = da_8C0C90(a1);
		if (r != 0)
			return r;
		U8(a1, 0x26) |= 1;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl da_8C3370(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0x180);
		if (S16(a1, 0x1C) >= 0x800)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0x800;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int i = 4; i != 0; i--, p += 0x2C)
			MEM<uint16_t>(p) = level;
		return 0;
	}

	uint32_t __cdecl da_8C3420(uint32_t a1)
	{
		if (S16(a1, 0x24) > 0x18)
		{
			U16(a1, 0x1C) = 0x800;
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl da_8C3440(uint32_t a1)
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

	uint32_t __cdecl da_8C3490(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t states[7] = { game_addr(game::MAG_009_Sink_State0_HideTarget), game_addr(game::MAG_009_Sink_State1_Setup), game_addr(game::MAG_009_Sink_State2_Sink), game_addr(game::MAG_009_Sink_State3_SinkFadeOut), game_addr(game::MAG_009_Sink_State4_SinkOn), game_addr(game::MAG_009_Sink_State5_RequestPhase4), game_addr(game::MAG_009_Sink_State6_Empty) };
		callp(states[S8(node, 0x29)], node);
		if ((U8(node, 0x26) & 8) != 0)
		{
			da_8C3520(game_addr(BODY_CTX));
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl da_8C3520(uint32_t a1)
	{
		const uint32_t ctx = a1;
		const uint32_t ent = U32(ctx, 0x48);
		const int32_t dy = S16(ctx, 0x54);
		ff8fx::copy<(0x20)>((void *)ctx, (const void *)(ent + 0x40));
		S32(ctx, 0x18) = add32(S32(ctx, 0x18), dy);
		const uint32_t pose = ent + 0x60;
		x::BattleModel_BuildBoneMatricesFromPose(pose);
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(ctx);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(ctx + 0x20);
		x::GTE_LoadIRFromMatrixColumn(ctx + 2);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(ctx + 0x22);
		x::GTE_LoadIRFromMatrixColumn(ctx + 4);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(ctx + 0x24);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(ctx + 0x14);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(ctx + 0x34);
		const uint32_t cur = U32(ctx, 0x4C);
		const uint32_t ot_shadow = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4040;
		U32(cur, 0) = x::sub_5088A0(ent, ot_shadow, 0x10, U32(cur, 0));
		da_8C3620(U32(ent, 0x64), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, ctx);
		const uint32_t part2 = U32(ent, 0x78);
		if (part2 != 0)
			da_8C3620(U32(part2, 4), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, ctx);
		x::BattleModel_BuildBoneMatricesFromPose(pose);
		return 0;
	}

	uint32_t __cdecl da_8C3620(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		(void)a3;
		const uint32_t ctx = a4;
		uint32_t cursor = U32(U32(ctx, 0x4C), 0);
		const uint32_t verts = U32(U32(ctx, 0x44), 4);
		const uint32_t bones = U32(a1, 0) + 0x10;
		uint32_t tab = U32(a1, 4);
		const int32_t count = S32(tab, 0);
		const uint32_t ent = U32(ctx, 0x48);
		const uint32_t w = x::Field_Alloc(0x84);
		U32(w, 0x48) = U32(ent, 0x7C);
		tab += 4;
		const uint16_t lo = U16(ctx, 0x58);
		const uint16_t range = (uint16_t)(U16(ctx, 0x5A) - lo);
		const uint16_t level = U16(ctx, 0x56);
		U16(w, 0x7C) = lo;
		U16(w, 0x7A) = level;
		U8(w, 0x81) = 0x3E;
		U8(w, 0x80) = 0x36;
		const int32_t lv = (int16_t)level;
		U16(w, 0x7E) = range;
		U32(w, 0x74) = (uint32_t)((lv * 0x100 + lv) * 0x100 + lv);
		if ((int16_t)range <= 0)
			U16(w, 0x7E) = 1;
		x::GTE_SetRotMatrix(ctx + 0x20);
		x::GTE_SetTransVector(ctx + 0x20);
		for (int32_t i = 0; i < count; i++)
		{
			uint32_t obj = U32(a1, 4) + U32(tab, 0);
			tab += 4;
			if (((U32(w, 0x48) >> (i & 31)) & 1) == 0)
				continue;
			uint32_t wv = verts;
			int32_t groups = S16(obj, 0);
			obj += 2;
			for (; groups > 0; groups--)
			{
				const int32_t bone = S16(obj, 0);
				obj += 2;
				const uint32_t m = (uint32_t)(bone * 0x30) + bones + 0x10;
				gx::GTE_SetLightMatrix(m);
				gx::GTE_SetBackgroundVector(U32(m, 0x14), U32(m, 0x18), U32(m, 0x1C));
				int32_t nv = S16(obj, 0);
				obj += 2;
				for (; nv > 0; nv--)
				{
					const uint16_t vx = U16(obj, 0);
					const uint16_t vy = U16(obj, 2);
					obj += 4;
					U16(w, 0x28) = vx;
					U16(w, 0x2A) = vy;
					U16(w, 0x2C) = U16(obj, 0);
					obj += 2;
					x::GTE_LoadV0(w + 0x28);
					gx::GTE_MVMVA_LightV0_Bk();
					x::GTE_StoreIR123(w + 0x28);
					const int16_t y = S16(w, 0x2A);
					const int16_t thr = S16(w, 0x7C);
					if (y > thr)
					{
						const int16_t c = S16(w, 0x7A);
						const int32_t t = mul32((int32_t)y - (int32_t)thr, (int32_t)c) / (int32_t)S16(w, 0x7E);
						U16(w, 0x78) = (uint16_t)t;
						if ((int16_t)t > c)
							U16(w, 0x78) = (uint16_t)c;
						else if ((int16_t)t < 0)
							U16(w, 0x78) = 0;
						const int16_t g = (int16_t)(c - S16(w, 0x78));
						U16(w, 0x78) = (uint16_t)g;
						const int32_t gi = g;
						U32(wv, 8) = (uint32_t)((gi * 0x100 + gi) * 0x100 + gi);
					}
					else
						U32(wv, 8) = U32(w, 0x74);
					U32(wv, 0) = U32(w, 0x28);
					U32(wv, 4) = U32(w, 0x2C);
					wv += 0xC;
				}
			}
			obj = (obj + 3) & ~3u;
			const int32_t ntri = S16(obj, 0);
			const int32_t nquad = S16(obj, 2);
			uint32_t rec = obj + 0xC;
			for (int32_t k = ntri; k > 0; k--, rec += 0x10)
			{
				const uint32_t i0 = U16(rec, 0) & 0xFFF, i1 = U16(rec, 2) & 0xFFF, i2 = U16(rec, 4) & 0xFFF;
				U32(w, 0x3C) = i1;
				U32(w, 0x38) = i0;
				U32(w, 0x40) = i2;
				U32(w, 0x4C) = U32(verts + i0 * 12, 0);
				U32(w, 0x50) = U32(verts + i0 * 12, 4);
				U32(w, 0x54) = U32(verts + i1 * 12, 0);
				U32(w, 0x58) = U32(verts + i1 * 12, 4);
				U32(w, 0x5C) = U32(verts + i2 * 12, 0);
				U32(w, 0x60) = U32(verts + i2 * 12, 4);
				gx::GTE_LoadV012Dwords(w + 0x4C);
				x::GTE_RTPT();
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(w + 0x6C);
				if (S32(w, 0x6C) <= 0)
					continue;
				x::GTE_AVSZ3();
				x::GTE_ReadOTZ(w + 0x70);
				U32(w, 0x70) = U32(w, 0x70) >> 2;
				gx::GTE_StoreSXY012_PolyGT3(cursor);
				U32(cursor, 0x18) = U32(rec, 0xC);
				U32(cursor, 0xC) = U32(rec, 8);
				U16(cursor, 0x24) = U16(rec, 6);
				U32(cursor, 0) = 0x09000000;
				U32(cursor, 4) = U32(verts + U32(w, 0x38) * 12, 8);
				U32(cursor, 0x10) = U32(verts + U32(w, 0x3C) * 12, 8);
				U32(cursor, 0x1C) = U32(verts + U32(w, 0x40) * 12, 8);
				U8(cursor, 7) = U8(w, 0x80);
				x::SSIGPU_InsertPrimAutoDepth(a2 + U32(w, 0x70) * 4, cursor);
				cursor += 0x28;
			}
			rec += 4;
			for (int32_t k = nquad; k > 0; k--, rec += 0x14)
			{
				const uint32_t i0 = U16(rec, -4) & 0xFFF, i1 = U16(rec, -2) & 0xFFF, i2 = U16(rec, 0) & 0xFFF;
				U32(w, 0x3C) = i1;
				U32(w, 0x38) = i0;
				U32(w, 0x40) = i2;
				U32(w, 0x4C) = U32(verts + i0 * 12, 0);
				U32(w, 0x50) = U32(verts + i0 * 12, 4);
				U32(w, 0x54) = U32(verts + i1 * 12, 0);
				U32(w, 0x58) = U32(verts + i1 * 12, 4);
				U32(w, 0x5C) = U32(verts + i2 * 12, 0);
				U32(w, 0x60) = U32(verts + i2 * 12, 4);
				const uint32_t p3 = verts + U32(w, 0x44) * 12;
				U32(w, 0x64) = U32(p3, 0);
				U32(w, 0x68) = U32(p3, 4);
				gx::GTE_LoadV012Dwords(w + 0x4C);
				x::GTE_RTPT();
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(w + 0x6C);
				if (S32(w, 0x6C) <= 0)
					continue;
				x::GTE_StoreSXY012_PolyGT3_2(cursor);
				const uint32_t i3 = U16(rec, 2) & 0xFFF;
				U32(w, 0x44) = i3;
				x::GTE_LoadV0(verts + i3 * 12);
				x::GTE_RTPS();
				x::GTE_ReadSXY2(cursor + 0x2C);
				x::GTE_AVSZ4();
				x::GTE_ReadOTZ(w + 0x70);
				U32(cursor, 0xC) = U32(rec, 4);
				U16(cursor, 0x24) = U16(rec, 0xC);
				U32(cursor, 0x18) = U32(rec, 8);
				U16(cursor, 0x30) = U16(rec, 0xE);
				U32(cursor, 4) = U32(verts + U32(w, 0x38) * 12, 8);
				U32(cursor, 0) = 0x0C000000;
				U32(cursor, 0x10) = U32(verts + U32(w, 0x3C) * 12, 8);
				U32(w, 0x70) = U32(w, 0x70) >> 2;
				U32(cursor, 0x1C) = U32(verts + U32(w, 0x40) * 12, 8);
				U32(cursor, 0x28) = U32(verts + U32(w, 0x44) * 12, 8);
				U8(cursor, 7) = U8(w, 0x81);
				x::SSIGPU_InsertPrimAutoDepth(a2 + U32(w, 0x70) * 4, cursor);
				cursor += 0x34;
			}
		}
		U32(U32(ctx, 0x4C), 0) = cursor;
		return x::Field_Free(0x84);
	}

	uint32_t __cdecl da_8C3B30(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t ent = ENT(U8(node, 0x2D));
		const uint16_t w = U16(ent, 0);
		U32(node, 0x74) = w;
		U16(ent, 0) = (uint16_t)(w | 8);
		x::GetEffectSpawnPosition(ent, 0xF1, 0, game_addr(ANCHOR));
		next_state(node);
		return 0;
	}

	uint32_t __cdecl da_8C3B80(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t ent = ENT(U8(node, 0x2D));
		da_8C4690(game_addr(BODY_CTX), game_addr(BODY_VERTS), game_addr(BODY_BLOCK), ent);
		const uint16_t top = U16(ent, 0x3C);
		U8(node, 0x26) |= 8;
		const int32_t sum = (int32_t)S16(ent, 0x36) + (int32_t)(int16_t)top;
		MEM<uint16_t>(game_addr(BODY_CTX) + 0x5A) = (uint16_t)(top - U16(ent, 0x1E));
		MEM<uint16_t>(game_addr(BODY_CTX) + 0x58) = (uint16_t)(sum / 2);
		MEM<uint16_t>(game_addr(BODY_CTX) + 0x56) = 0x40;
		U16(node, 0x6E) = 0xFFD8;
		da_8C0B70(node, game_addr(ORIG_SinkModel), game_addr(game::unk_1817EE8), 0x80, 0, 0);
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PARTICLE), game_addr(ORIG_Spawner), 0x5C, node);
		U16(node, 0x78) = 0x10;
		next_state(node);
		return 0;
	}

	uint32_t __cdecl da_8C3C90(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		U32(node, 0x1C) = U32(node, 0x3DC);
		U32(node, 0x20) = U32(node, 0x3E0);
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		U16(node, 0x88) = 0xFE00;
		da_8C0C90(node);
		next_state(node);
		return 0;
	}

	uint32_t __cdecl da_8C3CF0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t w1c = MEM<uint32_t>(game_addr(game::MAG_009_DoomActivation_SinkingAnchor));
		const uint32_t w20 = MEM<uint32_t>(game_addr(game::unk_27571BC));
		U32(node, 0x1C) = w1c;
		U32(node, 0x20) = w20;
		const uint32_t r = da_8C0C90(node);
		if (r == 0)
		{
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl da_8C3D40(uint32_t a1)
	{
		const uint32_t states[2] = { game_addr(game::MAG_009_ParticleSpawner_State0_Emit), game_addr(game::MAG_009_ParticleSpawner_State1_Empty) };
		callp(states[S8(a1, 0x29)], a1);
		return task_end(a1);
	}

	uint32_t __cdecl da_8C3D90(uint32_t a1)
	{
		const uint32_t node = a1;
		int32_t n = MEM<uint8_t>(game_addr(game::unk_1634EF4) + (uint32_t)(int32_t)S16(node, 0x24));
		if (a_73B7E0(4) != 0 || n == 0xFF)
		{
			U8(node, 0x26) |= 1;
			next_state(node);
			return 0;
		}
		for (; n > 0; n--)
			x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PARTICLE), game_addr(ORIG_Particle), 0x5C, node);
		return 0;
	}

	uint32_t __cdecl da_8C3E00(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t states[3] = { game_addr(game::MAG_009_Particle_State0_Init), game_addr(game::MAG_009_Particle_State1_Animate), game_addr(game::MAG_009_Particle_State2_Empty) };
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x42) = (uint16_t)(U16(node, 0x42) + 0x100);
		da_particle_place(node);
		da_8C3F00(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl da_8C3F00(uint32_t a1)
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

	uint32_t __cdecl da_8C45D0(uint32_t a1)
	{
		const uint32_t node = a1;
		U32(node, 0x4C) = game_addr(game::unk_1634B84);
		U16(node, 0x52) = 0xA;
		U16(node, 0x40) = (uint16_t)(x::CrtRand() & 0xFFF);
		U16(node, 0x42) = (uint16_t)(x::CrtRand() & 0xFFF);
		U16(node, 0x44) = (uint16_t)(x::CrtRand() & 0xFFF);
		next_state(node);
		return 0;
	}

	uint32_t __cdecl da_8C4620(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl da_8C4690(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		x::MAG_007_sub_8DCC00(ctx, 0x5C);
		U32(ctx, 0x48) = a4;
		U32(ctx, 0x40) = a2;
		U32(ctx, 0x4C) = game_addr(PACKET_CURSOR);
		U32(ctx, 0x50) = game_addr(PACKET_CURSOR);
		U32(a3, 4) = a2;
		U32(ctx, 0x44) = a3;
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

	uint32_t __cdecl da_8C46F0(uint32_t a1)
	{
		da_sink_step(a1);
		U16(a1, 0x78) = (uint16_t)(U16(a1, 0x78) - 1);
		if (S16(a1, 0x78) <= 0)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl da_8C4720(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t ent = ENT(U8(node, 0x2D));
		da_sink_step(node);
		const int16_t level = (int16_t)(MEM<uint16_t>(game_addr(BODY_CTX) + 0x56) - 0x10);
		MEM<uint16_t>(game_addr(BODY_CTX) + 0x56) = (uint16_t)level;
		if (level > 0)
			return 0;
		const uint16_t keep = (uint16_t)((U8(node, 0x74) ^ U8(ent, 0)) & 8);
		U8(node, 0x26) &= 0xF7;
		MEM<uint16_t>(game_addr(BODY_CTX) + 0x56) = 0;
		U16(node, 0x78) = 0xA;
		U16(ent, 0) = (uint16_t)(U16(ent, 0) ^ keep);
		next_state(node);
		U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x48) = 1;
		return 0;
	}

	uint32_t __cdecl da_8C47A0(uint32_t a1)
	{
		da_sink_step(a1);
		U16(a1, 0x78) = (uint16_t)(U16(a1, 0x78) - 1);
		if (S16(a1, 0x78) <= 0)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl da_8C47D0(uint32_t a1)
	{
		if (a_73B640(4) != 0)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl da_8C4800(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_1634B7C));
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
		{ game::MAG_009_Master_State0_Advance, (void *)a_73A0D0, "009 MAG_009_sub_8C0670" },
		{ game::MAG_009_Master_State1_Advance, (void *)a_73A0D0, "009 MAG_009_sub_8C0680" },
		{ game::MAG_009_Master_State3_SpawnEmitter, (void *)a_73A170, "009 MAG_009_sub_8C06A0" },
		{ game::MAG_009_EmitterTask, (void *)a_73A1A0, "009 MAG_009_sub_8C06D0" },
		{ game::MAG_009_Director_State0_RequestPhase1, (void *)a_73B790, "009 sub_8C08F0" },
		{ game::MAG_009_DirSyncGate, (void *)a_73B640, "009 sub_8C0910" },
		{ game::MAG_009_Director_State2_RequestPhase2, (void *)a_73B8A0, "009 sub_8C0960" },
		{ game::MAG_009_DirPhaseGE, (void *)a_73B7E0, "009 sub_8C09B0" },
		{ game::MAG_009_Director_State5_RequestPhase3, (void *)a_733B70, "009 sub_8C09F0" },
		{ game::MAG_009_Director_State7_WaitPhase4, (void *)a_733760, "009 sub_8C0A30" },
		{ game::MAG_009_Director_State8_RequestPhase5, (void *)a_73B620, "009 sub_8C0A50" },
		{ game::MAG_009_Director_State9_Finish, (void *)a_7475A0, "009 sub_8C0A70" },
		{ game::MAG_009_Director_State10_Empty, (void *)a_73A6D0, "009 nullsub_1823" },
		{ game::MAG_009_St_Empty, (void *)a_73A6D0, "009 nullsub_1824" },
		{ game::MAG_009_DoomModel_Task, (void *)a_73A380, "009 sub_8C0BC0" },
		{ game::MAG_009_EmitG3, (void *)a_73D770, "009 sub_8C2140" },
		{ game::MAG_009_EmitG4, (void *)a_73D9C0, "009 sub_8C2390" },
		{ game::MAG_009_PrimModel_State2_Empty, (void *)a_73A6D0, "009 nullsub_1825" },
		{ game::MAG_009_StageDarkenIn_Task, (void *)a_73A380, "009 sub_8C32D0" },
		{ game::MAG_009_StageDarkenIn_State0_Clear, (void *)a_7335D0, "009 sub_8C3330" },
		{ game::MAG_009_StageDarkenIn_State2_Empty, (void *)a_73A6D0, "009 nullsub_1826" },
		{ game::MAG_009_StageDarkenOut_Task, (void *)a_73A380, "009 sub_8C33C0" },
		{ game::MAG_009_StageDarkenOut_State2_Empty, (void *)a_73A6D0, "009 nullsub_1827" },
		{ game::MAG_009_SinkModel_Task, (void *)a_73A380, "009 sub_8C3C30" },
		{ game::MAG_009_SinkModel_State2_Empty, (void *)a_73A6D0, "009 nullsub_1828" },
		{ game::InitEffectSequenceFromData_c17, (void *)a_7435E0, "009 InitEffectSequenceFromData_c17" },
		{ game::MAG_009_SpriteStrip_DrawQuads, (void *)a_743720, "009 sub_8C4160" },
		{ game::MAG_009_StepCounterLimit, (void *)a_743C20, "009 sub_8C4640" },
		{ game::MAG_009_Particle_State2_Empty, (void *)a_73A6D0, "009 nullsub_1829" },
		{ game::MAG_009_ParticleSpawner_State1_Empty, (void *)a_73A6D0, "009 nullsub_1830" },
		{ game::MAG_009_Sink_State6_Empty, (void *)a_73A6D0, "009 nullsub_1831" },
		{ game::MAG_009_Emitter_State2_WaitChildren, (void *)a_7474B0, "009 MAG_009_sub_8C4840" },
		{ game::MAG_009_Emitter_State3_Finish, (void *)a_7474D0, "009 MAG_009_sub_8C4860" },
		{ game::MAG_009_Emitter_State4_Empty, (void *)a_73A6D0, "009 nullsub_1832" },
		{ game::MAG_009_Master_State4_NextTargetLoop, (void *)a_747500, "009 MAG_009_sub_8C4890" },
		{ game::MAG_009_Master_State5_Advance, (void *)a_73A0D0, "009 MAG_009_sub_8C48D0" },
		{ game::MAG_009_Master_State6, (void *)a_747550, "009 MAG_009_sub_8C48E0" },
		{ game::MAG_009_Master_State7_Request, (void *)a_747560, "009 MAG_009_sub_8C48F0" },
		{ game::MAG_009_Master_State8_WaitRequest, (void *)a_747590, "009 MAG_009_sub_8C4920" },
		{ game::MAG_009_Master_State9_Finish, (void *)a_7475A0, "009 MAG_009_sub_8C4930" },
		{ game::MAG_009_Master_State10_Empty, (void *)a_73A6D0, "009 nullsub_1822" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_009_Master_Task, (void *)da_8C0500, "009 da_8C0500" },
		{ game::MAG_009_Master_State2_WaitNoChildren, (void *)da_8C0690, "009 da_8C0690" },
		{ game::MAG_009_Emitter_State0_InitDirector, (void *)da_8C0750, "009 da_8C0750" },
		{ game::MAG_009_InitDirectorBlock, (void *)da_8C0780, "009 da_8C0780" },
		{ game::MAG_009_DirectorTask, (void *)da_8C0800, "009 da_8C0800" },
		{ game::MAG_009_Director_PhaseBookkeeping, (void *)da_8C08A0, "009 da_8C08A0" },
		{ game::MAG_009_Director_State1_WaitFrame5, (void *)da_8C0940, "009 da_8C0940" },
		{ game::MAG_009_Director_State3_PlaySoundAtPhase2, (void *)da_8C0980, "009 da_8C0980" },
		{ game::MAG_009_Director_State4_WaitFrame20, (void *)da_8C09D0, "009 da_8C09D0" },
		{ game::MAG_009_Director_State6_WaitPhase3, (void *)da_8C0A10, "009 da_8C0A10" },
		{ game::MAG_009_Director_StartPhase, (void *)da_8C0AB0, "009 da_8C0AB0" },
		{ game::MAG_009_SpawnPrimModelTask, (void *)da_8C0B70, "009 da_8C0B70" },
		{ game::MAG_009_DoomModel_State0_Init, (void *)da_8C0C20, "009 da_8C0C20" },
		{ game::MAG_009_PlayPrimModel, (void *)da_8C0C90, "009 da_8C0C90" },
		{ game::MAG_009_PrimPlayer_DrawCallback, (void *)da_8C0DC0, "009 da_8C0DC0" },
		{ game::MAG_009_RenderPrimModel, (void *)da_8C1080, "009 da_8C1080" },
		{ game::MAG_009_PrimList_FlatTri, (void *)da_8C11B0, "009 da_8C11B0" },
		{ game::MAG_009_PrimList_FlatQuad, (void *)da_8C13D0, "009 da_8C13D0" },
		{ game::MAG_009_PrimList_TexTri, (void *)da_8C1660, "009 da_8C1660" },
		{ game::MAG_009_PrimList_TexQuad, (void *)da_8C1B50, "009 da_8C1B50" },
		{ game::MAG_009_PrimList_GT3Scroll, (void *)da_8C2670, "009 da_8C2670" },
		{ game::MAG_009_PrimList_GT4Scroll, (void *)da_8C2BD0, "009 da_8C2BD0" },
		{ game::MAG_009_PrimModel_State1_Play, (void *)da_8C32A0, "009 da_8C32A0" },
		{ game::MAG_009_StageDarkenIn_State1_RampUp, (void *)da_8C3370, "009 da_8C3370" },
		{ game::MAG_009_StageDarkenOut_State0_Hold, (void *)da_8C3420, "009 da_8C3420" },
		{ game::MAG_009_StageDarkenOut_State1_FadeOut, (void *)da_8C3440, "009 da_8C3440" },
		{ game::MAG_009_Sink_Task, (void *)da_8C3490, "009 da_8C3490" },
		{ game::MAG_009_Sink_DrawTargetModel, (void *)da_8C3520, "009 da_8C3520" },
		{ game::MAG_009_Sink_RenderModelPart, (void *)da_8C3620, "009 da_8C3620" },
		{ game::MAG_009_Sink_State0_HideTarget, (void *)da_8C3B30, "009 da_8C3B30" },
		{ game::MAG_009_Sink_State1_Setup, (void *)da_8C3B80, "009 da_8C3B80" },
		{ game::MAG_009_SinkModel_State0_Init, (void *)da_8C3C90, "009 da_8C3C90" },
		{ game::MAG_009_SinkModel_State1_FollowAnchor, (void *)da_8C3CF0, "009 da_8C3CF0" },
		{ game::MAG_009_ParticleSpawner_Task, (void *)da_8C3D40, "009 da_8C3D40" },
		{ game::MAG_009_ParticleSpawner_State0_Emit, (void *)da_8C3D90, "009 da_8C3D90" },
		{ game::MAG_009_Particle_Task, (void *)da_8C3E00, "009 da_8C3E00" },
		{ game::MAG_009_Particle_DrawSprite, (void *)da_8C3F00, "009 da_8C3F00" },
		{ game::MAG_009_Particle_State0_Init, (void *)da_8C45D0, "009 da_8C45D0" },
		{ game::MAG_009_Particle_State1_Animate, (void *)da_8C4620, "009 da_8C4620" },
		{ game::MAG_009_Sink_InitRenderContext, (void *)da_8C4690, "009 da_8C4690" },
		{ game::MAG_009_Sink_State2_Sink, (void *)da_8C46F0, "009 da_8C46F0" },
		{ game::MAG_009_Sink_State3_SinkFadeOut, (void *)da_8C4720, "009 da_8C4720" },
		{ game::MAG_009_Sink_State4_SinkOn, (void *)da_8C47A0, "009 da_8C47A0" },
		{ game::MAG_009_Sink_State5_RequestPhase4, (void *)da_8C47D0, "009 da_8C47D0" },
		{ game::MAG_009_Emitter_State1_ApplyDamage, (void *)da_8C4800, "009 da_8C4800" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag009_doom_activation()
	{
		act::register_module(9);
		for (const act::doomact::ModPort *p = act::doomact::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(9, p->addr, p->port, p->name);
		for (const act::doomact::ModPort *p = act::doomact::PORTS; p->addr; p++)
			act::register_module_port(9, p->addr, p->port, p->name);
	}
}

