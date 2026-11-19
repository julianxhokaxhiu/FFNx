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
namespace udeath
{
	static const Mod MOD_079 = { "ultideath", 79, game::MAG_079_GRIEVER_DEATH_FL, game::MAG_078_MIGHTY_GUARD_FL,
		{ {}, {}, game::unk_1580B20, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_079_FilePtr, {}, {}, {}, game::unk_25E5024, {}, {}, game::unk_25E8954, {}, {}, {}, {}, game::unk_25E9F30, {}, game::MAG_079_EmitterQueue, {}, {}, game::unk_25EA348, game::unk_25EAD84, {}, game::unk_25EAF30, {}, game::unk_25EAFA8, game::unk_25EB390, {}, game::MAG_079_ScratchSP, game::unk_25EB3B4, game::MAG_079_ActorStatePtr },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_079_EmitterTask, game::MAG_079_Emitter_State0_StartLoadScriptAndDirector, {}, game::MAG_079_LoadCb_UploadTim, game::MAG_079_LoadCb_SummonStream, game::MAG_079_LoadCb_ClearBusy, game::MAG_079_LoadCb_ClearBusy_2, game::MAG_079_LoadCb_ClearBusy_3, game::MAG_079_LoadCb_ClearBusy_4, game::MAG_079_LoadScript_RecordFile, {}, {}, game::MAG_079_LoadScript_TryArmStep, {}, {}, {}, {}, {}, {}, {}, game::MAG_079_DirSyncGate, {}, game::MAG_079_DirPhaseGE, game::MAG_079_PrimSeq_DrawCallback, {}, {}, {}, {}, {}, game::MAG_079_ResetTrailHistory, game::MAG_079_Director_StartPositions, game::MAG_079_FillTrailRows, game::MAG_079_Director_Mode3Setup, game::MAG_079_Director_TargetPositions, game::MAG_079_RelocateActorData, {}, {}, game::MAG_079_DampValue, game::MAG_079_DampVector, {}, {}, game::MAG_079_MorphVertexBlock, {}, {}, {}, game::MAG_079_RandRange, game::MAG_079_RandRangeWide, game::MAG_079_RandAngleJitter, {}, {}, {}, game::MAG_079_ParticleList_Unlink, {}, {}, game::MAG_079_SpriteStrip_DrawQuads, {}, game::MAG_079_Emitter_State1_ApplyActionResults, game::MAG_079_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_079_Emitter_State3_FinishReleaseRoot, game::MAG_079_Emitter_State4_Empty, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id DIR_PTR = game::unk_1580B20;
	static constexpr game::Id SPRITE_CURSOR = game::MAG_079_SpriteCursor;
	static constexpr game::Id MODEL_BLOCK = game::MAG_079_CasterModelBlock;
	static constexpr game::Id PHASE_DATA = DIR_PTR;
	static constexpr game::Id PACKET_CURSOR = SPRITE_CURSOR;
	static constexpr game::Id SCRATCH_SP = game::MAG_079_ScratchSP;
	static constexpr game::Id ACTOR_STATE = game::MAG_079_ActorStatePtr;

	struct Residue
	{
		uint32_t node;
		uint16_t stamp;
		int kind;
		uint32_t w[5];
	};
	static Residue g_res = {};

	static void res_rotation(uint32_t node, uint32_t L)
	{
		uint32_t prev = 0;
		for (uint32_t q = MEM<uint32_t>(game_addr(game::MAG_079_TaskQueue)); q != 0 && q != node; q = U32(q, 4))
			prev = q;
		if (g_res.kind == 0 || prev == 0 || prev != g_res.node || U16(prev, 0x24) != g_res.stamp)
			return;
		uint32_t w[5];
		ff8fx::copy<(sizeof(w))>(w, g_res.w);
		if (g_res.kind == 1)
		{
			const uint32_t te = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2D) * 0x9C;
			const uint32_t m = U32(te, 0x64);
			const uint32_t sk = m ? U32(m, 0) : 0;
			w[0] = (sk && U8(sk, 0) >= 2) ? sk + 0x50 + 0x30 * ((uint32_t)U8(sk, 0) - 2) : 0;
		}
		ff8fx::copy<(0x12)>((void *)L, w);
	}

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
		static inline uint32_t Music_SetVolumeTrans(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::Music_SetVolumeTrans))(a1, a2, a3); }
		static inline uint32_t QueueChainTransformation(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::QueueChainTransformation))(a1, a2); }
		static inline uint32_t BS_R0win_QueueVictoryFanfare(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::BS_R0win_QueueVictoryFanfare))(a1, a2); }
		static inline uint32_t BS_Music_SetChannelVolumeCommand(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::BS_Music_SetChannelVolumeCommand))(a1, a2); }
		static inline uint32_t sub_8DE8F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::Effect_GetBoneCameraPosition))(a1, a2, a3, a4); }
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

	uint32_t __cdecl ud_7B4D80(uint32_t a1);
	uint32_t __cdecl ud_7B4F40(uint32_t a1);
	uint32_t __cdecl ud_7B4F80(void);
	uint32_t __cdecl ud_7B5070(uint32_t a1);
	uint32_t __cdecl ud_7B50C0(void);
	uint32_t __cdecl ud_7B5520(uint32_t a1);
	uint32_t __cdecl ud_7B55B0(uint32_t a1);
	uint32_t __cdecl ud_7B5650(uint32_t a1);
	uint32_t __cdecl ud_7B56A0(uint32_t a1);
	uint32_t __cdecl ud_7B5710(uint32_t a1);
	uint32_t __cdecl ud_7B5750(void);
	uint32_t __cdecl ud_7B57F0(uint32_t a1);
	uint32_t __cdecl ud_7B5880(uint32_t a1);
	uint32_t __cdecl ud_7B58E0(uint32_t a1);
	uint32_t __cdecl ud_7B5900(uint32_t a1);
	uint32_t __cdecl ud_7B5930(void);
	uint32_t __cdecl ud_7B59B0(uint32_t a1);
	uint32_t __cdecl ud_7B5B30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl ud_7B5BE0(uint32_t a1);
	uint32_t __cdecl ud_7B5D80(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ud_7B6020(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7B6150(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7B6370(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7B6600(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7B6A80(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7B7530(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7B7A30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7B8080(uint32_t a1);
	uint32_t __cdecl ud_7B8110(uint32_t a1);
	uint32_t __cdecl ud_7B8170(uint32_t a1);
	uint32_t __cdecl ud_7B8220(uint32_t a1);
	uint32_t __cdecl ud_7B8290(uint32_t a1);
	uint32_t __cdecl ud_7B8320(uint32_t a1);
	uint32_t __cdecl ud_7B8500(uint32_t a1);
	uint32_t __cdecl ud_7B8780(void);
	uint32_t __cdecl ud_7B8E30(uint32_t a1);
	uint32_t __cdecl ud_7B8EC0(void);
	uint32_t __cdecl ud_7B8F20(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_7B9080(void);
	uint32_t __cdecl ud_7B9140(uint32_t a1);
	uint32_t __cdecl ud_7B91C0(void);
	uint32_t __cdecl ud_7B9200(uint32_t a1);
	uint32_t __cdecl ud_7BA230(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_7BA340(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_7BA3C0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_7BA3F0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ud_7BA490(uint32_t a1);
	uint32_t __cdecl ud_7BA510(uint32_t a1);
	uint32_t __cdecl ud_7BA590(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ud_7BAD70(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_7BAF90(uint32_t a1);
	uint32_t __cdecl ud_7BB680(uint32_t a1);
	uint32_t __cdecl ud_7BB970(uint32_t a1);
	uint32_t __cdecl ud_7BBAD0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_7BBB60(void);
	uint32_t __cdecl ud_7BBBF0(uint32_t a1);
	uint32_t __cdecl ud_7BBC80(uint32_t a1);
	uint32_t __cdecl ud_7BBCA0(uint32_t a1);
	uint32_t __cdecl ud_7BBCD0(uint32_t a1);
	uint32_t __cdecl ud_7BBDF0(uint32_t a1);
	uint32_t __cdecl ud_7BBEB0(uint32_t a1);
	uint32_t __cdecl ud_7BC580(uint32_t a1);
	uint32_t __cdecl ud_7BC5A0(uint32_t a1);
	uint32_t __cdecl ud_7BC610(uint32_t a1);
	uint32_t __cdecl ud_7BC670(uint32_t a1);
	uint32_t __cdecl ud_7BC6B0(uint32_t a1);
	uint32_t __cdecl ud_7BC720(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ud_7BC840(uint32_t a1);
	uint32_t __cdecl ud_7BC950(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_7BCAA0(uint32_t a1);
	uint32_t __cdecl ud_7BCAC0(uint32_t a1);
	uint32_t __cdecl ud_7BCAF0(uint32_t a1);
	uint32_t __cdecl ud_7BCBA0(uint32_t a1);
	uint32_t __cdecl ud_7BCBC0(uint32_t a1);
	uint32_t __cdecl ud_7BCBF0(uint32_t a1);
	uint32_t __cdecl ud_7BCC80(uint32_t a1);
	uint32_t __cdecl ud_7BCCF0(uint32_t a1);
	uint32_t __cdecl ud_7BCD10(uint32_t a1);
	uint32_t __cdecl ud_7BCDF0(uint32_t a1);
	uint32_t __cdecl ud_7BCEA0(uint32_t a1);
	uint32_t __cdecl ud_7BCEC0(uint32_t a1);
	uint32_t __cdecl ud_7BCF10(uint32_t a1);
	uint32_t __cdecl ud_7BD020(uint32_t a1);
	uint32_t __cdecl ud_7BD110(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7BD800(uint32_t a1);
	uint32_t __cdecl ud_7BD890(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
	uint32_t __cdecl ud_7BD900(uint32_t a1);
	uint32_t __cdecl ud_7BD9B0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_7BDA60(uint32_t a1);
	uint32_t __cdecl ud_7BDB30(uint32_t a1);
	uint32_t __cdecl ud_7BDB90(uint32_t a1);
	uint32_t __cdecl ud_7BDBE0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_7BDCB0(uint32_t a1);
	uint32_t __cdecl ud_7BDD00(uint32_t a1);
	uint32_t __cdecl ud_7BDD40(uint32_t a1);
	uint32_t __cdecl ud_7BDD70(uint32_t a1);
	uint32_t __cdecl ud_7BDEF0(uint32_t a1);
	uint32_t __cdecl ud_7BDF90(uint32_t a1);
	uint32_t __cdecl ud_7BE050(uint32_t a1);
	uint32_t __cdecl ud_7BE0C0(uint32_t a1);
	uint32_t __cdecl ud_7BE1A0(uint32_t a1);
	uint32_t __cdecl ud_7BE1E0(void);
	uint32_t __cdecl ud_7BE1F0(uint32_t a1);
	uint32_t __cdecl ud_7BE220(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace udeath
{
	uint32_t __cdecl ud_7B4D80(uint32_t a1)
	{
		g_mod = &MOD_079;
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		MEM<uint32_t>(game_addr(game::MAG_079_ScratchSP)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_25E5024)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		uint32_t states[13] = {
			game_addr(game::MAG_079_Master_State0_Advance), game_addr(game::MAG_079_Master_State1_Advance), game_addr(game::MAG_079_Master_State2_CarveActorPools), game_addr(game::MAG_079_Master_State3_SpawnEmitter), game_addr(game::MAG_079_Master_State4_NextTargetLoop), game_addr(game::MAG_079_Master_State5_Advance), game_addr(game::MAG_079_Master_LoadFinalModel),
			game_addr(game::MAG_079_Master_HandOverFinalModel), game_addr(game::MAG_079_Master_State8_MusicVolumeWhenLoaded), game_addr(game::MAG_079_Master_State9_RequestBattleSync), game_addr(game::MAG_079_Master_State10_WaitBattleSyncDone), game_addr(game::MAG_079_Master_State11_Finish), game_addr(game::MAG_079_Master_State12_Empty),
		};
		if (U8(a1, 0x5C) & 1)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::MAG_079_ArenaB));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_25E973C));
			MEM<uint32_t>(game_addr(game::MAG_079_SpriteCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_25E5020)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::MAG_079_ArenaA));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_25E9738));
			MEM<uint32_t>(game_addr(game::MAG_079_SpriteCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_25E5020)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(a1);
		callp(states[S8(a1, 0x29)], a1);

		U16(a1, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_25EB390)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25EAF30)) = 0;
		static constexpr game::Id queues[9] = { game::MAG_079_EmitterQueue, game::MAG_079_DirectorQueue, game::MAG_079_LoadScriptQueue, game::unk_25EAF20, game::unk_25E9F30, game::MAG_079_TaskQueue, game::unk_25E8958, game::unk_25E9728, game::unk_25EA1C0 };
		for (int i = 0; i < 9; i++)
		{
			const uint32_t n = x::ExecuteTaskQueue(game_addr(queues[i]));
			U16(a1, 0x5E) = (uint16_t)(U16(a1, 0x5E) + (uint16_t)n);
		}
		const uint8_t status = U8(a1, 0x26);
		U16(a1, 0x5C) = (uint16_t)(U16(a1, 0x5C) + 1);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		if ((status & 1) && U8(a1, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(a1);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl ud_7B4F40(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::MAG_079_FileArenaCursor));
		MEM<uint32_t>(game_addr(game::MAG_079_ParticlePoolPtr)) = p;
		p += 0x870;
		MEM<uint32_t>(game_addr(game::MAG_079_ActorPoolPtr)) = p;
		p += 0x9D80;
		MEM<uint32_t>(game_addr(game::MAG_079_FileArenaCursor)) = p;
		ud_7B4F80();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7B4F80(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_079_ParticlePoolPtr)), 0x870);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_079_ActorPoolPtr)), 0x9D80);
		MEM<uint16_t>(game_addr(game::MAG_079_ParticleCursor)) = 0;
		MEM<uint16_t>(game_addr(game::MAG_079_ActorCursor)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25EAD80)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25DF342)) = 0;
		return 0;
	}

	uint32_t __cdecl ud_7B5070(uint32_t a1)
	{
		ud_7B5520(a1);
		ud_7B50C0();
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_079_LoadScriptQueue), game_addr(game::MAG_079_FileLoadScriptTask), 0x30, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_079_DirectorQueue), game_addr(game::MAG_079_DirectorTask), 0x48, a1);
		t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl ud_7B50C0(void)
	{
		MEM<uint32_t>(game_addr(game::unk_25EA348)) = game_addr(game::unk_1581F18);
		MEM<uint32_t>(game_addr(game::unk_25EAD84)) = game_addr(game::unk_25EB3C0);
		x::MAG_007_sub_8DCC00(game_addr(game::unk_25EB3C0), 0x16C);
		const uint32_t c = MEM<uint32_t>(game_addr(game::unk_25EAD84));
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_079_FileArenaCursor));
		U32(c, 0x100) = arena;
		MEM<uint32_t>(game_addr(game::MAG_079_FileArenaCursor)) = arena + 0xA000;
		const uint32_t dflt = game_addr(game::unk_19F1018);
		U32(c, 0x114) = dflt;
		U32(c, 0x118) = dflt;
		U32(c, 0x11C) = dflt;
		U32(c, 0x120) = dflt;
		U32(c, 0x104) = dflt;
		U32(c, 0x108) = dflt;
		U32(c, 0x10C) = dflt;
		U32(c, 0x110) = dflt;
		U32(c, 0x124) = dflt;
		U32(c, 0x128) = game_addr(game::unk_19F24C4);
		U32(c, 0x12C) = game_addr(game::unk_19F36C0);
		U32(c, 0x130) = game_addr(game::unk_19F4880);
		for (int32_t o = 0x134; o <= 0x160; o += 4)
			U32(c, o) = dflt;
		U16(c, 0x164) = 0;
		U16(c, 0x166) = 0;
		return c;
	}

	uint32_t __cdecl ud_7B5520(uint32_t a1)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(PHASE_DATA)), 0x54);
		const uint32_t d = MEM<uint32_t>(game_addr(PHASE_DATA));
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2D) * 0x9Cu;
		U32(d, 8) = U32(ent, 0x1C);
		U32(d, 0xC) = U32(ent, 0x20);
		U16(d, 0) = 0;
		U16(d, 2) = 0;
		U16(d, 4) = 0;
		const int16_t dx_ = (int16_t)(U16(d, 8) - U16(d, 0));
		const uint32_t ang = x::CartesianToGameAngle((uint32_t)(int32_t)dx_, (uint32_t)(int32_t)S16(d, 0xC));
		const uint32_t d2 = MEM<uint32_t>(game_addr(PHASE_DATA));
		U16(d2, 0x4A) = (uint16_t)((ang - 0x800) & 0xFFF);
		x::MAG_022_sub_8DD770(game_addr(game::unk_1D99AB0));
		return x::MAG_022_sub_8DD8A0(game_addr(game::unk_1D99AB0), (uint32_t)(int32_t)S16(MEM<uint32_t>(game_addr(PHASE_DATA)), 0x4A));
	}

	uint32_t __cdecl ud_7B55B0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[11];
		states[0] = game_addr(game::MAG_079_Director_State0_ClaimVoiceMusicFadeAtCut1);
		states[1] = game_addr(game::MAG_079_Director_State1_HideMonstersAtCut1);
		states[2] = game_addr(game::MAG_079_Director_State2_ArmLoadScriptStep);
		states[3] = game_addr(game::MAG_079_Director_State3_ToggleAtCutFrame62);
		states[4] = game_addr(game::MAG_079_Director_State4_WaitCutFrame63);
		states[5] = game_addr(game::MAG_079_Director_State5_WaitDirGate2);
		states[6] = game_addr(game::MAG_079_Director_State6_WaitDirPhase2);
		states[7] = game_addr(game::MAG_079_Director_State7_DamageCueAtFrame5A);
		states[8] = game_addr(game::MAG_079_Director_State8_RestoreAndReleaseVoiceAtCut3);
		states[9] = game_addr(game::MAG_079_Director_State9_Finish);
		states[10] = game_addr(game::MAG_079_Director_State10_Empty);
		ud_7B5650(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_7B5650(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			ud_7B59B0(a1);
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

	uint32_t __cdecl ud_7B56A0(uint32_t a1)
	{
		if (a_73B640(1) == 0)
			return 0;
		const uint32_t v = x::BdSound_ClaimVoiceSlot(game_addr(game::unk_176E064), 1, 0x80);
		U16(MEM<uint32_t>(game_addr(game::unk_1580B20)), 0x52) = (uint16_t)v;
		gx::Music_SetVolumeTrans(0, 0x5A, 0);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl ud_7B5710(uint32_t a1)
	{
		if (a_73B7E0(1) == 0)
			return 0;
		ud_7B5750();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl ud_7B5750(void)
	{
		uint32_t save = MEM<uint32_t>(game_addr(game::unk_1580B20)) + 0x20;
		for (uint32_t e = game_addr(game::BattleEntitySlotData); e < game_addr(game::unk_1D97494); e += 0x9C, save += 4)
		{
			const uint16_t f = U16(e, 0);
			if (f & 2)
			{
				U32(save, 0) = (uint32_t)f;
				U16(e, 0) = (uint16_t)(f | 4);
			}
		}
		return 0;
	}

	uint32_t __cdecl ud_7B57F0(uint32_t a1)
	{
		const uint32_t tab = MEM<uint32_t>(game_addr(game::unk_25EAD84));
		if (U16(tab, 0x166) == 0)
		{
			a_73BB60(0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return a1;
		}
		const uint32_t blk = MEM<uint32_t>(game_addr(game::unk_1580B20));
		if (S16(blk, 0x46) < 0x62)
			return blk;
		return a_73BB60(1);
	}

	uint32_t __cdecl ud_7B5880(uint32_t a1)
	{
		const uint32_t blk = MEM<uint32_t>(game_addr(game::unk_1580B20));
		if (S16(blk, 0x46) < 0x63)
			return blk;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl ud_7B58E0(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_1580B20));
		if (S16(d, 0x46) < 0x5A)
			return d;
		U16(d, 0x48) = 1;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl ud_7B5900(uint32_t a1)
	{
		if (a_73B640(3) == 0)
			return 0;
		ud_7B5930();
		x::sub_4A2940((uint32_t)(int32_t)S16(MEM<uint32_t>(game_addr(game::unk_1580B20)), 0x52));
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl ud_7B5930(void)
	{
		uint32_t save = MEM<uint32_t>(game_addr(game::unk_1580B20)) + 0x20;
		for (uint32_t e = game_addr(game::BattleEntitySlotData); e < game_addr(game::unk_1D97494); e += 0x9C, save += 4)
		{
			const uint16_t f = U16(e, 0);
			if (f & 2)
			{
				const uint32_t v = ((uint32_t)(uint8_t)(U8(save, 0) ^ (uint8_t)f) & 4) ^ (uint32_t)f;
				U16(e, 0) = (uint16_t)v;
			}
		}
		return 0;
	}

	uint32_t __cdecl ud_7B59B0(uint32_t a1)
	{
		const int32_t cut = (int32_t)S16(MEM<uint32_t>(game_addr(game::unk_1580B20)), 0x40);
		if (cut == 1)
		{
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_079_TaskQueue), game_addr(game::MAG_079_CasterModelTask), 0xB4, a1);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_079_TaskQueue), game_addr(game::MAG_079_DebrisThrowerTask), 0xB4, a1);
			return x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_079_TaskQueue), game_addr(game::MAG_079_LightFadeIn), 0xB4, a1);
		}
		if (cut != 2)
			return 0;
		a_73C100(a1, game_addr(game::MAG_079_ActorSystemTask), game_addr(game::unk_176A958), 0, 0xF, 0);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_079_TaskQueue), game_addr(game::MAG_079_FlakeThrowerTask), 0xB4, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_079_TaskQueue), game_addr(game::MAG_079_SparkThrowerTask), 0xB4, a1);
		ud_7B5B30(a1, game_addr(game::MAG_079_PrimSeqTask0), U32(MEM<uint32_t>(game_addr(game::unk_25EAD84)), 0x124), 0xEC, 0, 0);
		ud_7B5B30(a1, game_addr(game::MAG_079_PrimSeqTask1), U32(MEM<uint32_t>(game_addr(game::unk_25EAD84)), 0x128), 0x134, 1, 0);
		ud_7B5B30(a1, game_addr(game::MAG_079_PrimSeqTask2), U32(MEM<uint32_t>(game_addr(game::unk_25EAD84)), 0x12C), 0x134, 2, 0);
		return x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_079_TaskQueue), game_addr(game::MAG_079_LightFadeOut), 0xB4, a1);
	}

	uint32_t __cdecl ud_7B5B30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25E9728), a2, 0x2C0, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		S32(t, 0x78) = (int32_t)(int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl ud_7B5BE0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t slot = U8(node, 0x2C);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + slot * 0x9C;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		const uint32_t w1c = U32(ent, 0x1C);
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		const uint32_t w20 = U32(ent, 0x20);
		U32(node, 0x1C) = w1c;
		U32(node, 0x20) = w20;
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + 0xF800);
		U16(node, 0x88) = 0xFE00;
		a_73C280(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7B5D80(uint32_t a1, uint32_t a2, uint32_t a3)
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
		const int32_t alpha = S16(rec, 0x24);
		U32(hdr, 0x14) = 0x2030;
		U32(hdr, 0xC) = (uint32_t)alpha;
		if (alpha != 0)
		{
			U32(hdr, 0x14) = 0x20F0;
			U32(hdr, 8) = U32(rec, 0x20);
		}
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(hdr, 0x18) = 0;
		U16(hdr, 0x1A) = 0;
		U16(hdr, 0x1E) = 0;
		U16(hdr, 0x1C) = 0;
		U16(hdr, 0x26) = 0;
		U16(hdr, 0x24) = 0;
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		U32(hdr, 0x10) = (uint32_t)(int32_t)S16(blk, 0x58);
		U16(hdr, 0x22) = 0x100;
		U16(hdr, 0x20) = 0x100;
		U16(hdr, 0x2A) = 0x100;
		U16(hdr, 0x28) = 0x100;
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = ud_7B6020(hdr, ot, 2, cursor);
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl ud_7B6020(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
			cur = ud_7B6150(ctx, a2, a3, a4);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = ud_7B6370(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = ud_7B6600(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = ud_7B6A80(ctx, a2, a3, cur);
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
			cur = ud_7B7530(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			return ud_7B7A30(ctx, a2, a3, cur);
		U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		return cur;
	}

	uint32_t __cdecl ud_7B6150(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_7B6370(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_7B6600(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_7B6A80(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_7B7530(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_7B7A30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_7B8080(uint32_t a1)
	{
		const uint32_t r = a_73C280(a1);
		if (r == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl ud_7B8110(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 4)
			return 0;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		const uint32_t w20 = MEM<uint32_t>(game_addr(game::unk_25DF3C4));
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		U32(node, 0x1C) = MEM<uint32_t>(game_addr(game::unk_25DF3C0));
		U32(node, 0x20) = w20;
		U16(node, 0x88) = 0xFE00;
		a_73C280(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7B8170(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t w1c = MEM<uint32_t>(game_addr(game::unk_25DF3C0));
		const uint32_t w20 = MEM<uint32_t>(game_addr(game::unk_25DF3C4));
		U32(node, 0x1C) = w1c;
		U32(node, 0x20) = w20;
		const uint32_t r = a_73C280(node);
		if (r == 0)
		{
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7B8220(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t slot = U8(node, 0x2C);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + slot * 0x9C;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		const uint32_t w1c = U32(ent, 0x1C);
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		const uint32_t w20 = U32(ent, 0x20);
		U32(node, 0x1C) = w1c;
		U32(node, 0x20) = w20;
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + 0xF800);
		U16(node, 0x88) = 0xFE00;
		a_73C280(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7B8290(uint32_t a1)
	{
		const uint32_t r = a_73C280(a1);
		if (r == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl ud_7B8320(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			a_73F990(a1);
			ud_7B8E30(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7B8500(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_079_ActorStatePtr));
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

	uint32_t __cdecl ud_7B8780(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_079_ActorStatePtr));
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

	uint32_t __cdecl ud_7B8E30(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::MAG_079_ActorStatePtr), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			ud_7BBB60();
			ud_7B91C0();
			ud_7B9080();
			ud_7B8EC0();
			sys = U32(game_addr(game::MAG_079_ActorStatePtr), 0);
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
		U16(game_addr(game::unk_25EB390), 0) = (uint16_t)(U16(game_addr(game::unk_25EB390), 0) + c14);
		U16(game_addr(game::unk_25EAF30), 0) = (uint16_t)(U16(game_addr(game::unk_25EAF30), 0) + c16);
		return done;
	}

	uint32_t __cdecl ud_7B8EC0(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_079_ActorStatePtr), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::MAG_079_ActorStatePtr), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					ud_7B8F20(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl ud_7B8F20(uint32_t a1, uint32_t a2)
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
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = ud_7B6020(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
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
				MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = ud_7B6020(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
			}
		}
		const uint32_t top = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 0x68;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = top;
		return top;
	}

	uint32_t __cdecl ud_7B9080(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::MAG_079_ActorStatePtr)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::MAG_079_ActorStatePtr));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						ud_7B9140(act);
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
							ud_7B9140(act);
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

	uint32_t __cdecl ud_7B9140(uint32_t a1)
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

	uint32_t __cdecl ud_7B91C0(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_079_ActorStatePtr), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				ud_7BA490(act);
			else if (type == 1)
				ud_7B9200(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl ud_7B9200(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::MAG_079_ActorStatePtr));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (ud_7BA340(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::MAG_079_ActorStatePtr));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BA230(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::MAG_079_ActorStatePtr), 0);
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

	uint32_t __cdecl ud_7BA340(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				ud_7BA3C0(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			ud_7BA3C0(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			ud_7BA3C0(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl ud_7BA3C0(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return ud_7BA3F0(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl ud_7BA3F0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_079_ParticlePoolPtr));
		int32_t idx = MEM<int16_t>(game_addr(game::MAG_079_ParticleCursor));
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
			MEM<uint16_t>(game_addr(game::MAG_079_ParticleCursor)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::MAG_079_ParticleCursor)) = 0;
		return slot;
	}

	uint32_t __cdecl ud_7BA490(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			ud_7BB680(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_079_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_079_Actor_InterpolatePosition), a1);
		ud_7BA510(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl ud_7BA510(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::MAG_079_ActorStatePtr), 0);
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
				ud_7BA590(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			ud_7BA590(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl ud_7BA590(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::MAG_079_ScratchSP), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::MAG_079_ScratchSP), 0) = sp;
		uint32_t node = ud_7BAD70(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::MAG_079_ActorStatePtr), 0);
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
		ud_7BBAD0(node, desc);
		U32(game_addr(game::MAG_079_ScratchSP), 0) = U32(game_addr(game::MAG_079_ScratchSP), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl ud_7BAD70(uint32_t a1, uint32_t a2)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_079_ActorPoolPtr));
		int32_t i = MEM<int16_t>(game_addr(game::MAG_079_ActorCursor));
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
			if (i >= 0x3B)
				i = 0;
			tries++;
			if (tries >= 0x3C)
				break;
		}
		i++;
		if (i >= 0x3B)
			MEM<uint16_t>(game_addr(game::MAG_079_ActorCursor)) = 0;
		else
			MEM<uint16_t>(game_addr(game::MAG_079_ActorCursor)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl ud_7BAF90(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::MAG_079_ActorStatePtr), 0);
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
				uint32_t g = U32(game_addr(game::MAG_079_ActorStatePtr), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::MAG_079_ActorStatePtr), 0);
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

	uint32_t __cdecl ud_7BB680(uint32_t a1)
	{
		uint32_t dir = U32(game_addr(game::MAG_079_ActorStatePtr), 0);
		uint32_t table = U32(dir, 0x224);
		int32_t di = (int32_t)S8(a1, 0x6A);
		uint32_t owner = U32(a1, 0x5C);
		uint32_t desc = U32(table + di * 4, 0);
		U8(a1, 0x68) = U8(desc, 0x11);
		uint32_t m = a1 + 0x2C;
		switch ((uint32_t)(int32_t)S8(desc, 0x1C))
		{
			case 0:
				x::MAG_022_sub_8DD770(m);
				break;
			case 1:
			{
				x::MAG_022_sub_8DD770(m);
				int32_t k = shl32((int32_t)S8(a1, 0x6B), 4);
				uint32_t d = U32(game_addr(game::MAG_079_ActorStatePtr), 0);
				uint32_t e = (uint32_t)k + d;
				int32_t dx = sub32(S32(d, 0x1C4), S32(e, 0x54));
				int32_t dz = sub32(S32(d, 0x1CC), S32(e, 0x5C));
				uint32_t ang = x::CartesianToGameAngle((uint32_t)dx, (uint32_t)dz);
				if (ang != 0)
					x::MAG_022_sub_8DD8A0(m, ang);
				break;
			}
			case 2:
			{
				x::MAG_022_sub_8DD770(m);
				int32_t k = shl32((int32_t)S8(a1, 0x6B), 4);
				uint32_t d = U32(game_addr(game::MAG_079_ActorStatePtr), 0);
				int32_t dx = sub32(S32((uint32_t)k + d, 0x54), S32(d, 0x1C4));
				int32_t dz = sub32(S32((uint32_t)k + d, 0x5C), S32(d, 0x1CC));
				uint32_t ang = x::CartesianToGameAngle((uint32_t)dx, (uint32_t)dz);
				if (ang != 0)
					x::MAG_022_sub_8DD8A0(m, ang);
				break;
			}
			case 3:
				ff8fx::copy<(32)>((void *)m, (void *)(owner + 0x8C));
				break;
			case 4:
			{
				x::MAG_022_sub_8DD770(m);
				uint32_t d = U32(game_addr(game::MAG_079_ActorStatePtr), 0);
				int32_t ent = (int32_t)S16(d, 0x1E);
				int32_t yaw = (int32_t)S16(game_addr(game::unk_1D972CE) + (uint32_t)(ent * 39) * 4, 0);
				if (yaw != 0)
					x::MAG_022_sub_8DD8A0(m, (uint32_t)yaw);
				break;
			}
			default:
				break;
		}
		switch ((uint32_t)(int32_t)S8(a1, 0x68))
		{
			case 0: a_742EB0(a1); break;
			case 1: ud_7BB970(a1); break;
			case 2:
			case 3: a_7424C0(a1); break;
			case 4: callp(game_addr(game::MAG_079_Actor_InterpolatePosition), a1); break;
			default: break;
		}
		U16(a1, 0x64) = (uint16_t)(int16_t)S8(desc, 0x12);
		return 0;
	}

	uint32_t __cdecl ud_7BB970(uint32_t a1)
	{
		alignas(4) uint8_t loc[0x18] = {};
		const uint32_t L = P(loc);
		const uint32_t dir = MEM<uint32_t>(game_addr(game::MAG_079_ActorStatePtr));
		const uint32_t desc = U32(U32(dir, 0x224) + (uint32_t)((int32_t)S8(a1, 0x6A) * 4), 0);
		uint32_t base = 0;
		switch ((uint32_t)(int32_t)S8(desc, 0x25))
		{
			case 0: base = 0x1D4; break;
			case 1: base = 0x1E4; break;
			case 3: base = 0x204; break;
			case 2:
			{
				const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)((int32_t)S16(dir, 0x1E) * 0x9C);
				const int32_t r = (int32_t)x::CrtRand();
				const int32_t bone = S16(game_addr(game::unk_1581EF0) + (uint32_t)((r % 0x12) * 2), 0);
				x::GetEffectSpawnPosition(ent, (uint32_t)bone, 0, L);
				U32(a1, 0x4C) = (uint32_t)shl32(S16(L, 0), 16);
				U32(a1, 0x50) = (uint32_t)shl32(S16(L, 2), 16);
				U32(a1, 0x54) = (uint32_t)shl32(S16(L, 4), 16);
				break;
			}
			default: break;
		}
		if (base != 0)
		{
			const uint32_t src = dir + base;
			U32(a1, 0x4C) = U32(src, 0);
			U32(a1, 0x50) = U32(src, 4);
			const uint32_t s8 = U32(src, 8);
			const uint32_t sc = U32(src, 0xC);
			U32(a1, 0x54) = s8;
			U32(a1, 0x58) = sc;
		}
		U16(L, 8) = (uint16_t)(S32(desc, 0) / 65536);
		U16(L, 0xA) = (uint16_t)(S32(desc, 4) / 65536);
		U16(L, 0xC) = (uint16_t)(S32(desc, 8) / 65536);
		x::GTE_SetRotMatrix(a1 + 0x2C);
		x::GTE_LoadV0(L + 8);
		x::GTE_MVMVA_RotV0();
		x::GTE_StoreIR123(L + 0x10);
		const int32_t ir1 = S16(L, 0x10);
		const int32_t ir2 = S16(L, 0x12);
		const int32_t ir3 = S16(L, 0x14);
		U32(a1, 0x4C) = (uint32_t)add32(S32(a1, 0x4C), shl32(ir1, 16));
		U32(a1, 0x50) = (uint32_t)add32(S32(a1, 0x50), shl32(ir2, 16));
		U32(a1, 0x54) = (uint32_t)add32(S32(a1, 0x54), shl32(ir3, 16));
		return 0;
	}

	uint32_t __cdecl ud_7BBAD0(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			ud_7BA3F0(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			ud_7BA3F0(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			ud_7BA3F0(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			ud_7BA3F0(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BBB60(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::MAG_079_ActorStatePtr));
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
					ud_7BA3F0(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::MAG_079_ActorStatePtr));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				ud_7BA3F0(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::MAG_079_ActorStatePtr));
			}
		}
		return 0;
	}

	uint32_t __cdecl ud_7BBBF0(uint32_t a1)
	{
		if (ud_7B8E30(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BBC80(uint32_t a1)
	{
		U16(a1, 0xB0) = 0x32;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7BBCA0(uint32_t a1)
	{
		ud_7BBCD0(a1);
		U16(a1, 0xB0) = (uint16_t)(U16(a1, 0xB0) - 1);
		if (S16(a1, 0xB0) <= 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BBCD0(uint32_t a1)
	{
		alignas(4) uint8_t loc[0x28] = {};
		const uint32_t L = P(loc);
		const uint32_t node = a1;
		const uint32_t slot = U8(node, 0x2C);
		x::GetEffectSpawnPosition(game_addr(game::BattleEntitySlotData) + slot * 0x9C, 0x31, 0, node + 0x1C);
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_079_TaskQueue), game_addr(game::MAG_079_SparkPairTask), 0xB4, node);
		uint32_t s = t + 0x6C;
		for (int i = 0; i < 2; i++, s += 8)
		{
			x::MAG_022_sub_8DD770(L + 8);
			const uint32_t ry = x::CrtRand() & 0xFFF;
			x::MAG_022_sub_8DD8A0(L + 8, ry);
			const uint32_t rx = x::CrtRand() & 0xFFF;
			x::sub_8DD7E0(L + 8, rx);
			U16(L, 0) = 0;
			U16(L, 2) = 0;
			U16(L, 4) = (uint16_t)((x::CrtRand() & 0x1FF) + 0x200);
			x::matrixMultiplyVector(L + 8, L, L);
			U32(s, 0) = U32(node, 0x1C);
			U32(s, 4) = U32(node, 0x20);
			U16(s, 0) = (uint16_t)(U16(s, 0) + U16(L, 0));
			U16(s, 2) = (uint16_t)(U16(s, 2) + U16(L, 2));
			U16(s, 4) = (uint16_t)(U16(s, 4) + U16(L, 4));
			U16(L, 0) = 0;
			U16(L, 2) = 0;
			U16(L, 4) = (uint16_t)((x::CrtRand() & 0x1FF) + 0x200);
			x::matrixMultiplyVector(L + 8, L, s + 0x20);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BBDF0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_079_SparkPair_State0_InitSprite);
		states[1] = game_addr(game::MAG_079_SparkPair_State1_Animate);
		states[2] = game_addr(game::MAG_079_SparkPair_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		uint32_t c = node + 0x6C;
		for (int i = 0; i < 2; i++)
		{
			const int32_t vx = S16(c, 0x20);
			c += 8;
			U16(c, -8) = (uint16_t)(U16(c, -8) + (uint16_t)(vx / 16));
			const int32_t vy = S16(c, 0x1A);
			U16(c, -6) = (uint16_t)(U16(c, -6) + (uint16_t)(vy / 16));
			const int32_t vz = S16(c, 0x1C);
			U16(c, -4) = (uint16_t)(U16(c, -4) + (uint16_t)(vz / 16));
		}
		uint32_t e = node + 0x6C;
		for (int i = 0; i < 2; i++, e += 8)
		{
			U32(node, 0x1C) = U32(e, 0);
			U32(node, 0x20) = U32(e, 4);
			ud_7BBEB0(node);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_7BBEB0(uint32_t a1)
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
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::MAG_079_SpriteCursor));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(blk, 0x24) = 0;
		U16(blk, 0xB4) = colour;
		MEM<uint32_t>(game_addr(game::MAG_079_SpriteCursor)) = a_7435E0(blk, ot, 2, cursor);
		x::Field_Free(0xD8);
		return 0;
	}

	uint32_t __cdecl ud_7BC580(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U32(a1, 0x4C) = game_addr(game::unk_1580E54);
		U16(a1, 0x56) = 0xFF80;
		U16(a1, 0x52) = 0xF;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl ud_7BC5A0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BC610(uint32_t a1)
	{
		const uint32_t tab[4] = { game_addr(game::au_re_BdPlaySE_2), game_addr(game::MAG_079_DebrisThrower_State1_TwoBurstsPerTick), game_addr(game::MAG_079_DebrisThrower_State2_BurstOffBone), game_addr(game::MAG_079_DebrisThrower_State3_Empty) };
		callp(tab[S8(a1, 0x29)], a1);
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24)++;
		return task_end(a1, status);
	}

	uint32_t __cdecl ud_7BC670(uint32_t a1)
	{
		if (S16(a1, 0x24) < 0x5A)
			return 0;
		U16(a1, 0xB0) = 8;
		x::BdPlaySE(game_addr(game::unk_1580B28), 0, 0x80);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7BC6B0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t ang = (uint32_t)(uint16_t)(U16(node, 0xB2) - 0x40) & 0xFFF;
		U16(node, 0xB2) = (uint16_t)ang;
		ud_7BC720(node, 1, ang);
		ang = (uint32_t)(uint16_t)(U16(node, 0xB2) - 0x40) & 0xFFF;
		U16(node, 0xB2) = (uint16_t)ang;
		ud_7BC720(node, 1, ang);
		U16(node, 0xB0) = (uint16_t)(U16(node, 0xB0) - 1);
		if (S16(node, 0xB0) <= 0)
		{
			U16(node, 0xB0) = 0x34;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BC720(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		alignas(4) uint8_t loc[0x20] = {};
		const uint32_t L = P(loc);
		const uint32_t node = a1;
		const uint32_t slot = U8(node, 0x2C);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + slot * 0x9C;
		const uint32_t pos = node + 0x1C;
		if ((uint16_t)a2 == 1)
		{
			x::MAG_022_sub_8DD770(L);
			x::MAG_022_sub_8DD8A0(L, 0x800);
			x::sub_8DD7E0(L, (uint32_t)(int32_t)(int16_t)a3);
			x::GetEffectSpawnPosition(ent, 0x31, 0, pos);
		}
		else
		{
			res_rotation(node, L);
			gx::sub_8DE8F0(ent, 0x31, 0, L);
			const uint16_t tx = U16(L, 0x14);
			const uint16_t ty = U16(L, 0x18);
			const uint16_t tz = U16(L, 0x1C);
			U16(node, 0x1E) = ty;
			U16(pos, 0) = tx;
			U16(node, 0x20) = tz;
		}
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_079_TaskQueue), game_addr(game::MAG_079_DebrisTask), 0xB4, node);
		uint32_t v = t + 0x8C;
		for (int i = 0; i < 2; i++, v += 8)
		{
			const uint32_t w0 = U32(pos, 0);
			const uint32_t w1 = U32(pos, 4);
			U32(v, -0x20) = w0;
			U32(v, -0x1C) = w1;
			U16(v, 0) = (uint16_t)((x::CrtRand() & 0x7FF) - 0x400);
			U16(v, 2) = (uint16_t)((x::CrtRand() & 0x7FF) - 0x400);
			U16(v, 4) = (uint16_t)(0xFFFFF000u - (x::CrtRand() & 0x7FF));
			x::matrixMultiplyVector(L, v, v);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BC840(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_079_Debris_State0_InitSprite);
		states[1] = game_addr(game::MAG_079_Debris_State1_Animate);
		states[2] = game_addr(game::MAG_079_Debris_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		uint32_t c = node + 0x8E;
		for (int i = 0; i < 2; i++)
		{
			U16(c, 0) = (uint16_t)(U16(c, 0) + 0x100);
			int16_t v = S16(c, -2);
			v = (int16_t)(v - (int16_t)((int32_t)v / 8));
			c += 8;
			U16(c, -0xA) = (uint16_t)v;
			v = S16(c, -8);
			v = (int16_t)(v - (int16_t)((int32_t)v / 8));
			U16(c, -8) = (uint16_t)v;
			v = S16(c, -6);
			v = (int16_t)(v - (int16_t)((int32_t)v / 8));
			U16(c, -6) = (uint16_t)v;
			U16(c, -0x2A) = (uint16_t)(U16(c, -0x2A) + (uint16_t)((int32_t)S16(c, -0xA) / 16));
			U16(c, -0x28) = (uint16_t)(U16(c, -0x28) + (uint16_t)((int32_t)S16(c, -8) / 16));
			U16(c, -0x26) = (uint16_t)(U16(c, -0x26) + (uint16_t)((int32_t)S16(c, -6) / 16));
		}
		uint32_t e = node + 0x6C;
		for (int i = 0; i < 2; i++, e += 8)
		{
			U32(node, 0x1C) = U32(e, 0);
			U32(node, 0x20) = U32(e, 4);
			ud_7BC950(node, 0x2000);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_7BC950(uint32_t a1, uint32_t a2)
	{
		const uint32_t node = a1;
		if ((U8(node, 0x26) & 4) != 0)
			return 0;
		const uint32_t s = x::Field_Alloc(0xE8);
		const uint32_t mat = s + 0xB8;
		x::UnpackRotationMatrix(game_addr(game::unk_1D97778), mat);
		const uint16_t scale = (uint16_t)a2;
		if (scale != 0x1000)
		{
			const int32_t sc = (int16_t)scale;
			U32(s, 0xE0) = (uint32_t)sc;
			U32(s, 0xDC) = (uint32_t)sc;
			U32(s, 0xD8) = (uint32_t)sc;
			x::scale3DMatrix(mat, s + 0xD8);
		}
		const int32_t px = S16(node, 0x1C);
		const int32_t py = S16(node, 0x1E);
		const int32_t pz = S16(node, 0x20);
		const uint32_t pos = s + 0xCC;
		U32(s, 0xD0) = (uint32_t)py;
		U32(s, 0xD4) = (uint32_t)pz;
		U32(pos, 0) = (uint32_t)px;
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(mat);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(mat);
		x::GTE_LoadIRFromMatrixColumn(s + 0xBA);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(s + 0xBA);
		x::GTE_LoadIRFromMatrixColumn(s + 0xBC);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(s + 0xBC);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(pos);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(pos);
		x::GTE_SetRotMatrix(mat);
		x::GTE_SetTransVector(mat);
		const uint32_t seq = U32(node, 0x4C);
		const uint16_t frame = U16(node, 0x50);
		const uint16_t w56 = U16(node, 0x56);
		U32(s, 0) = seq;
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::MAG_079_SpriteCursor));
		U16(s, 4) = frame;
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(s, 0x24) = 0;
		U16(s, 0xB4) = w56;
		MEM<uint32_t>(game_addr(game::MAG_079_SpriteCursor)) = a_7435E0(s, ot, 2, cursor);
		x::Field_Free(0xE8);
		return 0;
	}

	uint32_t __cdecl ud_7BCAA0(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U32(a1, 0x4C) = game_addr(game::unk_1580B30);
		U16(a1, 0x56) = 0xFFC0;
		U16(a1, 0x52) = 0x15;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl ud_7BCAC0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BCAF0(uint32_t a1)
	{
		ud_7BC720(a1, 0, 0);
		U16(a1, 0xB0) = (uint16_t)(U16(a1, 0xB0) - 1);
		if (S16(a1, 0xB0) <= 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BCBA0(uint32_t a1)
	{
		U16(a1, 0xB0) = 0x26;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7BCBC0(uint32_t a1)
	{
		ud_7BCBF0(a1);
		U16(a1, 0xB0) = (uint16_t)(U16(a1, 0xB0) - 1);
		if (S16(a1, 0xB0) <= 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BCBF0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t slot = U8(node, 0x2C);
		x::GetEffectSpawnPosition(game_addr(game::BattleEntitySlotData) + slot * 0x9C, 0xF1, 0, node + 0x1C);
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_079_TaskQueue), game_addr(game::MAG_079_FlakeTask), 0xB4, node);
		const uint32_t w1 = U32(node, 0x20);
		const uint32_t w0 = U32(node, 0x1C);
		U32(t, 0x6C) = w0;
		U32(t, 0x70) = w1;
		U16(t, 0x6C) = (uint16_t)(U16(t, 0x6C) + (uint16_t)((x::CrtRand() & 0x7FF) - 0x400));
		U16(t, 0x6E) = (uint16_t)(U16(t, 0x6E) + (uint16_t)((x::CrtRand() & 0x7FF) - 0x400));
		U16(t, 0x70) = (uint16_t)(U16(t, 0x70) + (uint16_t)((x::CrtRand() & 0x1FF) - 0x100));
		return 0;
	}

	uint32_t __cdecl ud_7BCC80(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_079_Flake_State0_InitSprite);
		states[1] = game_addr(game::MAG_079_Flake_State1_Animate);
		states[2] = game_addr(game::MAG_079_Flake_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		U32(node, 0x1C) = U32(node, 0x6C);
		U32(node, 0x20) = U32(node, 0x70);
		ud_7BBEB0(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_7BCCF0(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U32(a1, 0x4C) = game_addr(game::unk_1580D78);
		U16(a1, 0x56) = 0xFF80;
		U16(a1, 0x52) = 7;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl ud_7BCD10(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BCDF0(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0xA0);
		if (S16(a1, 0x1C) >= 0x800)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0x800;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t v = U16(a1, 0x1C);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int i = 0; i < 4; i++, p += 0x2C)
			U16(p, 0) = v;
		return 0;
	}

	uint32_t __cdecl ud_7BCEA0(uint32_t a1)
	{
		if (S16(a1, 0x24) <= 0x5A)
			return 0;
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x1C) = 0x800;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl ud_7BCEC0(uint32_t a1)
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

	uint32_t __cdecl ud_7BCF10(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[11];
		states[0] = game_addr(game::MAG_079_CasterModel_State0_SetupModelBlock);
		states[1] = game_addr(game::MAG_079_CasterModel_State1_ShowAtCut2);
		states[2] = game_addr(game::MAG_079_CasterModel_State2_ScaleThenSpawnSeq);
		states[3] = game_addr(game::MAG_079_CasterModel_State3_Morph);
		states[4] = game_addr(game::MAG_079_CasterModel_State4_Rise);
		states[5] = game_addr(game::MAG_079_CasterModel_State5_Apex);
		states[6] = game_addr(game::MAG_079_CasterModel_State6_ActorSystemAndScale);
		states[7] = game_addr(game::MAG_079_PlaceTargetAtAnchor);
		states[8] = game_addr(game::MAG_079_CasterModel_State8_FadeCasterGrow);
		states[9] = game_addr(game::MAG_079_CasterModel_State9_FadeTarget);
		states[10] = game_addr(game::MAG_079_CasterModel_State10_Empty);
		const uint32_t slot = U8(node, 0x2C);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + slot * 0x9C;
		const int8_t st0 = S8(node, 0x29);
		callp(states[st0], node);
		const uint8_t fl = U8(node, 0x26);
		g_res.kind = 0;
		if (st0 == 1 && S8(node, 0x29) == 1)
		{
			g_res.kind = 1;
			g_res.w[1] = game_addr(game::sub_7BD921);
			g_res.w[2] = 2;
			g_res.w[3] = node;
			g_res.w[4] = ent;
		}
		if ((fl & 8) != 0 && (fl & 4) == 0)
		{
			uint32_t live = 0;
			for (uint32_t q = MEM<uint32_t>(game_addr(game::MAG_079_TaskQueue)); q != 0 && q != node; q = U32(q, 4))
				live++;
			g_res.kind = 2;
			g_res.w[0] = game_addr(game::BattleEntitySlotData) + (uint32_t)((int32_t)S16(game_addr(game::MAG_079_CasterModelBlock), 0xA8) * 0x9C) + 0x60;
			g_res.w[1] = node;
			g_res.w[2] = ent;
			g_res.w[3] = game_addr(game::MAG_079_TaskQueue);
			g_res.w[4] = live;
			const uint32_t x0 = MEM<uint32_t>(game_addr(game::unk_25DF3C0)) - MEM<uint32_t>(game_addr(game::unk_25DF3B8));
			U16(ent, 0x1C) = (uint16_t)x0;
			S32(ent, 0x54) = (int32_t)(int16_t)x0;
			const uint16_t y0 = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_25DF3C2)) - MEM<uint16_t>(game_addr(game::unk_25DF3BA)));
			U16(ent, 0x1E) = y0;
			S32(ent, 0x58) = (int32_t)(int16_t)y0;
			const uint32_t z0 = MEM<uint32_t>(game_addr(game::unk_25DF3C4)) - MEM<uint32_t>(game_addr(game::unk_25DF3BC));
			U16(ent, 0x20) = (uint16_t)z0;
			S32(ent, 0x5C) = (int32_t)(int16_t)z0;
			ud_7BD020(game_addr(game::MAG_079_CasterModelBlock));
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		g_res.node = node;
		g_res.stamp = U16(node, 0x24);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_7BD020(uint32_t a1)
	{
		const uint32_t blk = a1;
		const int32_t idx = S16(blk, 0xA8);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)(idx * 0x9C);
		ff8fx::copy<(0x20)>((void *)(blk + 0x20), (const void *)(ent + 0x40));
		const uint32_t pose = ent + 0x60;
		x::BattleModel_BuildBoneMatricesFromPose(pose);
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(blk + 0x20);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(blk + 0x40);
		x::GTE_LoadIRFromMatrixColumn(blk + 0x22);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(blk + 0x42);
		x::GTE_LoadIRFromMatrixColumn(blk + 0x24);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(blk + 0x44);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(blk + 0x34);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(blk + 0x54);
		ud_7BD110(U32(ent, 0x64), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, blk);
		const uint32_t part2 = U32(ent, 0x78);
		if (part2 != 0)
			ud_7BD110(U32(part2, 4), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, blk);
		x::BattleModel_BuildBoneMatricesFromPose(pose);
		return 0;
	}

	uint32_t __cdecl ud_7BD110(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t blk = a4;
		const uint32_t ent = U32(blk, 0x64);
		const uint32_t verts = U32(U32(blk, 0x60), 4);
		uint32_t cursor = U32(U32(blk, 0x68), 0);
		const uint32_t bones = U32(a1, 0) + 0x10;
		uint32_t tab = U32(a1, 4);
		const int32_t count = S32(tab, 0);
		const uint32_t w = x::Field_Alloc(0xA8);
		tab += 4;
		U16(w, 0x9E) = U16(blk, 0xA0);
		U16(w, 0x9C) = (uint16_t)(0x1000 - U16(blk, 0xA0));
		U32(w, 0x5C) = U32(blk, 0x70);
		U32(w, 0x60) = U32(blk, 0x74);
		U16(w, 0xA4) = U16(blk, 0xA6);
		U16(w, 0xA0) = U16(blk, 0xA2);
		U16(w, 0xA6) = U16(blk, 0xD0);
		const uint32_t rgb = U32(ent, 0x28) & 0xFFFFFF;
		U16(w, 0xA2) = U16(blk, 0xA4);
		U32(w, 0x94) = rgb | 0x2C000000;
		ff8fx::copy<(0x20)>((void *)w, (const void *)blk);
		U32(w, 0x90) = rgb | 0x24000000;
		U32(w, 0x98) = U32(ent, 0x7C);
		if (U16(blk, 0x98) != 0)
			x::sub_8DD7E0(w, (uint32_t)(int32_t)S16(blk, 0x98));
		if (U16(blk, 0x9C) != 0)
			x::sub_8DD960(w, (uint32_t)(int32_t)S16(blk, 0x9C));
		if (U16(blk, 0x9A) != 0)
			x::MAG_022_sub_8DD8A0(w, (uint32_t)(int32_t)S16(blk, 0x9A));
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
					wv += 0x10;
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
					vp += 0x10;
					zp += 0x10;
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
				const uint32_t s2 = U32(verts + i2 * 16, 8);
				const uint32_t s1 = U32(verts + i1 * 16, 8);
				const uint32_t s0 = U32(verts + i0 * 16, 8);
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
				const int32_t z1 = S32(verts + U32(w, 0x84) * 16, 0xC);
				const int32_t z0 = S32(verts + U32(w, 0x80) * 16, 0xC);
				const int32_t z2 = S32(verts + U32(w, 0x88) * 16, 0xC);
				const int32_t sum = add32(add32(z1, z0), z2);
				int32_t hq = (int32_t)(((int64_t)sum * 0x55555556LL) >> 32);
				hq = add32(hq, (int32_t)((uint32_t)hq >> 31));
				int32_t key = add32(hq >> (a3 & 31), (int32_t)S16(w, 0xA6));
				U32(w, 0x78) = (uint32_t)key;
				if (key < 0)
					U32(w, 0x78) = 0;
				x::SSIGPU_InsertPrimDepthKeys(a2 + U32(w, 0x78) * 4, pk, (uint32_t)z0, (uint32_t)z1, (uint32_t)z2, 0);
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
					const uint32_t s2 = U32(verts + i2 * 16, 8);
					const uint32_t s1 = U32(verts + i1 * 16, 8);
					const uint32_t s0 = U32(verts + i0 * 16, 8);
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
					U32(pk, 0x18) = U32(w, 0x2C);
					U32(pk, 0) = 0x9000000;
					const uint32_t v3 = verts + i3 * 16;
					const uint32_t s3 = U32(v3, 8);
					U32(pk, 0x14) = U32(f, 8);
					U32(w, 0x30) = s3;
					U32(pk, 0x20) = s3;
					U32(pk, 0xC) = U32(f, 4);
					U16(pk, 0x1C) = U16(f, 0xC);
					U16(pk, 0x24) = U16(f, 0xE);
					U32(pk, 4) = U32(w, 0x94);
					if (U8(f, 0xB) & 2)
						U8(pk, 7) |= 2;
					const int32_t z3 = S32(v3, 0xC);
					const int32_t z2 = S32(verts + U32(w, 0x88) * 16, 0xC);
					const int32_t z1 = S32(verts + U32(w, 0x84) * 16, 0xC);
					const int32_t z0 = S32(verts + U32(w, 0x80) * 16, 0xC);
					const int32_t sum = add32(add32(add32(z0, z3), z1), z2);
					int32_t key = add32(sum >> ((a3 + 2) & 31), (int32_t)S16(w, 0xA6));
					U32(w, 0x78) = (uint32_t)key;
					if (key < 0)
						U32(w, 0x78) = 0;
					x::SSIGPU_InsertPrimDepthKeys(a2 + U32(w, 0x78) * 4, pk, (uint32_t)z0, (uint32_t)z1, (uint32_t)z2, (uint32_t)z3);
					pk += 0x28;
				}
			}
			cursor = pk;
		}
		U32(U32(blk, 0x68), 0) = cursor;
		x::Field_Free(0xA8);
		return 0;
	}

	uint32_t __cdecl ud_7BD800(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t slot = U8(node, 0x2C);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + slot * 0x9C;
		gx::QueueChainTransformation(ent, 0x11);
		ud_7BD890(game_addr(game::MAG_079_CasterModelBlock), game_addr(game::unk_25DF508), game_addr(game::unk_25EA350), ent, (uint32_t)(uint16_t)U8(node, 0x2C));
		const uint32_t w1c = U32(ent, 0x1C);
		const uint32_t w20 = U32(ent, 0x20);
		MEM<uint32_t>(game_addr(game::unk_25DF404)) = w1c;
		MEM<uint32_t>(game_addr(game::unk_25DF408)) = w20;
		a_73C100(node, game_addr(game::MAG_079_ActorSystemTask), game_addr(game::unk_1762E40), 0, 0xF, 0);
		x::BdPlaySE(game_addr(game::unk_1580B24), 0, 0x80);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7BD890(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
	{
		x::MAG_007_sub_8DCC00(a1, 0xD4);
		U32(a1, 0x64) = a4;
		U32(a1, 0x68) = game_addr(game::BATTLE_PACKET_WRITE_CURSOR);
		U32(a1, 0x6C) = game_addr(game::BATTLE_PACKET_WRITE_CURSOR);
		U16(a1, 0xA8) = (uint16_t)a5;
		U32(a3, 4) = a2;
		U32(a1, 0x60) = a3;
		U16(a3, 0x14) = 0;
		U16(a3, 0x16) = 0;
		U16(a3, 0x18) = 0x140;
		U16(a3, 0x1A) = 0;
		U8(a3, 0x1C) = 0x80;
		U8(a3, 0x1D) = 0x80;
		U8(a3, 0x1E) = 0x80;
		U32(a3, 0x20) = 0xFFFFFFFFu;
		U16(a3, 0x24) = 0;
		return 0;
	}

	uint32_t __cdecl ud_7BD900(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t slot = U8(node, 0x2C);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + slot * 0x9C;
		if (a_73B7E0(2) == 0)
			return 0;
		U8(node, 0x26) |= 8;
		U8(ent, 0) |= 4;
		MEM<uint16_t>(game_addr(game::unk_25DF3DA)) = 0xD000;
		MEM<uint16_t>(game_addr(game::unk_25DF3DC)) = 0xE000;
		MEM<uint16_t>(game_addr(game::unk_25DF40E)) = 0x200;
		ud_7BD9B0(node, game_addr(game::MAG_079_CasterModelBlock));
		MEM<uint16_t>(game_addr(game::unk_25DF3EC)) = 0x1F6;
		MEM<uint16_t>(game_addr(game::unk_25DF3EE)) = 0x1F6;
		MEM<uint16_t>(game_addr(game::unk_25DF3EA)) = 0x1F6;
		x::MAG_022_sub_8DD770(game_addr(game::MAG_079_CasterModelBlock));
		const uint32_t w20 = U32(ent, 0x20);
		const uint32_t w1c = U32(ent, 0x1C);
		const uint16_t py = MEM<uint16_t>(game_addr(game::unk_25DF3BA));
		MEM<uint32_t>(game_addr(game::unk_25DF3C4)) = w20;
		MEM<uint32_t>(game_addr(game::unk_25DF3C0)) = w1c;
		MEM<uint16_t>(game_addr(game::unk_25DF3C2)) = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_25DF3C2)) + py);
		U16(node, 0xB0) = 0x10;
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7BD9B0(uint32_t a1, uint32_t a2)
	{
		const uint32_t e = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2C) * 0x9C;
		uint32_t mat[8];
		const uint32_t xy = U32(e, 0x1C);
		ff8fx::copy<(0x20)>(mat, (const void *)(e + 0x40));
		const uint32_t z = U32(e, 0x20);
		U16(e, 0x1C) = 0;
		U16(e, 0x1E) = 0;
		U16(e, 0x20) = 0;
		x::MAG_022_sub_8DD770(e + 0x40);
		const uint32_t blk = a2;
		x::GetEffectSpawnPosition(e, 0xF1, 0, blk + 0x70);
		const uint16_t ex = U16(e, 0x1C), ey = U16(e, 0x1E), ez = U16(e, 0x20);
		U16(blk, 0x70) = (uint16_t)(U16(blk, 0x70) - ex);
		U16(blk, 0x72) = (uint16_t)(U16(blk, 0x72) - ey);
		U16(blk, 0x74) = (uint16_t)(U16(blk, 0x74) - ez);
		ff8fx::copy<(0x20)>((void *)(e + 0x40), mat);
		U32(e, 0x1C) = xy;
		U32(e, 0x20) = z;
		return 0;
	}

	uint32_t __cdecl ud_7BDA60(uint32_t a1)
	{
		const uint32_t node = a1;
		ud_7BDBE0(node, S16(node, 0xB0) < 0xC ? 1u : 0u);
		ud_7BD9B0(node, game_addr(game::MAG_079_CasterModelBlock));
		U16(node, 0xB0) = (uint16_t)(U16(node, 0xB0) - 1);
		if (S16(node, 0xB0) <= 0)
		{
			ud_7B5B30(node, game_addr(game::MAG_079_PrimSeqTask3), U32(MEM<uint32_t>(game_addr(game::unk_25EAD84)), 0x130), 0x218, 3, 0);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BDB30(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		const uint32_t w20 = MEM<uint32_t>(game_addr(game::unk_25DF3C4));
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		U32(node, 0x1C) = MEM<uint32_t>(game_addr(game::unk_25DF3C0));
		U32(node, 0x20) = w20;
		U16(node, 0x88) = 0xFE00;
		a_73C280(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7BDB90(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t w1c = MEM<uint32_t>(game_addr(game::unk_25DF3C0));
		const uint32_t w20 = MEM<uint32_t>(game_addr(game::unk_25DF3C4));
		U32(node, 0x1C) = w1c;
		U32(node, 0x20) = w20;
		const uint32_t r = a_73C280(node);
		if (r == 0)
		{
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BDBE0(uint32_t a1, uint32_t a2)
	{
		(void)a1;
		int16_t vy = (int16_t)(MEM<uint16_t>(game_addr(game::unk_25DF3DA)) + MEM<uint16_t>(game_addr(game::unk_25DF40E)));
		MEM<uint16_t>(game_addr(game::unk_25DF3DA)) = (uint16_t)vy;
		if ((uint16_t)a2 == 1)
		{
			int16_t v = (int16_t)MEM<uint16_t>(game_addr(game::unk_25DF3D8));
			v = (int16_t)(v + (int16_t)(-((int32_t)v / 16)));
			MEM<uint16_t>(game_addr(game::unk_25DF3D8)) = (uint16_t)v;
			vy = (int16_t)(vy + (int16_t)(-((int32_t)vy / 16)));
			MEM<uint16_t>(game_addr(game::unk_25DF3DA)) = (uint16_t)vy;
			v = (int16_t)MEM<uint16_t>(game_addr(game::unk_25DF3DC));
			v = (int16_t)(v + (int16_t)(-((int32_t)v / 16)));
			MEM<uint16_t>(game_addr(game::unk_25DF3DC)) = (uint16_t)v;
		}
		MEM<uint16_t>(game_addr(game::unk_25DF3C0)) = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_25DF3C0)) + (uint16_t)((int32_t)(int16_t)MEM<uint16_t>(game_addr(game::unk_25DF3D8)) / 16));
		MEM<uint16_t>(game_addr(game::unk_25DF3C2)) = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_25DF3C2)) + (uint16_t)((int32_t)vy / 16));
		MEM<uint16_t>(game_addr(game::unk_25DF3C4)) = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_25DF3C4)) + (uint16_t)((int32_t)(int16_t)MEM<uint16_t>(game_addr(game::unk_25DF3DC)) / 16));
		MEM<uint16_t>(game_addr(game::unk_25DF3E0)) = (uint16_t)((MEM<uint16_t>(game_addr(game::unk_25DF3E0)) + 0xFF80) & 0xFFF);
		return 0;
	}

	uint32_t __cdecl ud_7BDCB0(uint32_t a1)
	{
		const uint32_t node = a1;
		ud_7BDBE0(node, 1);
		ud_7BD9B0(node, game_addr(game::MAG_079_CasterModelBlock));
		const uint16_t m = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_25DF3E8)) + 0x80);
		MEM<uint16_t>(game_addr(game::unk_25DF3E8)) = m;
		if ((int16_t)m >= 0x1000)
		{
			MEM<uint16_t>(game_addr(game::unk_25DF3E8)) = 0x1000;
			MEM<uint16_t>(game_addr(game::unk_25DF40E)) = 0xFE00;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BDD00(uint32_t a1)
	{
		const uint32_t node = a1;
		ud_7BDBE0(node, 1);
		ud_7BD9B0(node, game_addr(game::MAG_079_CasterModelBlock));
		if ((int16_t)MEM<uint16_t>(game_addr(game::unk_25DF3DA)) <= -0x800)
		{
			MEM<uint16_t>(game_addr(game::unk_25DF40E)) = 0x100;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7BDD40(uint32_t a1)
	{
		const uint32_t node = a1;
		ud_7BDBE0(node, 1);
		ud_7BD9B0(node, game_addr(game::MAG_079_CasterModelBlock));
		if ((int16_t)MEM<uint16_t>(game_addr(game::unk_25DF3DA)) >= 0)
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7BDD70(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2D) * 0x9C;
		ud_7BD9B0(node, game_addr(game::MAG_079_CasterModelBlock));
		a_73C100(node, game_addr(game::MAG_079_ActorSystemTask), game_addr(game::unk_176C45C), 0, 0xF, 0);
		const uint32_t lvl = (uint32_t)U16(node, 0xAC);
		MEM<uint32_t>(game_addr(game::unk_25DF3FC)) = 0x800;
		MEM<uint32_t>(game_addr(game::unk_25DF3F8)) = 0x800;
		MEM<uint32_t>(game_addr(game::unk_25DF3F4)) = 0x800;
		U32(ent, 0x30) = game_addr(game::unk_25DF3F4);
		a_733950(ent, lvl, 0);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7BDEF0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t ax0 = MEM<uint32_t>(game_addr(game::unk_25DF3C0));
		U16(node, 0xAC) = 0x1000;
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2D) * 0x9C;
		const uint32_t az0 = MEM<uint32_t>(game_addr(game::unk_25DF3C4));
		U32(ent, 0x1C) = ax0;
		U32(ent, 0x20) = az0;
		U16(ent, 0x1C) = (uint16_t)(U16(ent, 0x1C) + 0x80);
		U16(ent, 0x1E) = (uint16_t)(U16(ent, 0x1E) - 0x20);
		U16(ent, 0x20) = (uint16_t)(U16(ent, 0x20) + 0xFEB0);
		S32(ent, 0x54) = S16(ent, 0x1C);
		S32(ent, 0x58) = S16(ent, 0x1E);
		S32(ent, 0x5C) = S16(ent, 0x20);
		U16(ent, 0) = (uint16_t)((U16(ent, 0) & 0xFFFB) | 0x800);
		a_733950(ent, 0x1000, 0);
		x::BdPlaySE(game_addr(game::unk_1580B2C), 0, 0x80);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7BDF90(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0xAE) = (uint16_t)(U16(node, 0xAE) + 0x200);
		const uint32_t tgt = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2D) * 0x9C;
		const uint32_t cst = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2C) * 0x9C;
		if (S16(node, 0xAE) >= 0x1000)
			U16(node, 0xAE) = 0x1000;
		a_733950(cst, (uint32_t)U16(node, 0xAE), 0);
		const uint16_t sx = MEM<uint16_t>(game_addr(game::unk_25DF3EA));
		MEM<uint16_t>(game_addr(game::unk_25DF3EE)) = sx;
		MEM<uint16_t>(game_addr(game::unk_25DF3EC)) = sx;
		int32_t s = (int32_t)(MEM<uint32_t>(game_addr(game::unk_25DF3FC)) + 0x100);
		MEM<uint32_t>(game_addr(game::unk_25DF3FC)) = (uint32_t)s;
		if (s >= 0x1000)
		{
			U8(node, 0x26) |= 4;
			s = 0x1000;
			MEM<uint32_t>(game_addr(game::unk_25DF3FC)) = 0x1000;
			U32(tgt, 0x30) = 0;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		MEM<uint32_t>(game_addr(game::unk_25DF3F8)) = (uint32_t)s;
		MEM<uint32_t>(game_addr(game::unk_25DF3F4)) = (uint32_t)s;
		return 0;
	}

	uint32_t __cdecl ud_7BE050(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0xAC) = (uint16_t)(U16(node, 0xAC) + 0xFE00);
		const uint32_t tgt = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2D) * 0x9C;
		if (S16(node, 0xAC) <= 0)
		{
			U8(node, 0x26) |= 1;
			U16(node, 0xAC) = 0;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		a_733950(tgt, (uint32_t)U16(node, 0xAC), 0);
		return 0;
	}

	uint32_t __cdecl ud_7BE0C0(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_1580B20));
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

	uint32_t __cdecl ud_7BE1A0(uint32_t a1)
	{
		if (U16(a1, 0x5E) != 0)
			return 0;
		MEM<uint8_t>(game_addr(game::unk_25DF340)) = 0;
		x::pre_LoadBattleFile(0x191, MEM<uint32_t>(game_addr(game::MAG_079_FilePtr)), 0, game_addr(game::MAG_079_LoadCb_SetLoadFlag));
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7BE1E0(void)
	{
		MEM<uint8_t>(game_addr(game::unk_25DF340)) = 1;
		return 0;
	}

	uint32_t __cdecl ud_7BE1F0(uint32_t a1)
	{
		if (MEM<uint8_t>(game_addr(game::unk_25DF340)) != 1)
			return 0;
		MEM<uint8_t>(game_addr(game::unk_25DF340)) = 0;
		gx::BS_R0win_QueueVictoryFanfare(MEM<uint32_t>(game_addr(game::MAG_079_FilePtr)), game_addr(game::unk_25DF340));
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl ud_7BE220(uint32_t a1)
	{
		if (MEM<uint8_t>(game_addr(game::unk_25DF340)) == 0)
			return 0;
		gx::BS_Music_SetChannelVolumeCommand(0, 0x7F);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_079_Master_State0_Advance, (void *)a_73A0D0, "079 MAG_079_sub_7B4F20" },
		{ game::MAG_079_Master_State1_Advance, (void *)a_73A0D0, "079 MAG_079_sub_7B4F30" },
		{ game::MAG_079_Master_State3_SpawnEmitter, (void *)a_73A170, "079 MAG_079_sub_7B4FC0" },
		{ game::MAG_079_EmitterTask, (void *)a_73A1A0, "079 MAG_079_sub_7B4FF0" },
		{ game::MAG_079_FileLoadScriptTask, (void *)a_73A380, "079 MAG_079_sub_7B51C0" },
		{ game::MAG_079_LoadScript_Issue, (void *)a_73A3E0, "079 sub_7B5220" },
		{ game::MAG_079_LoadCb_UploadTim, (void *)a_73A580, "079 sub_7B53C0" },
		{ game::MAG_079_LoadCb_SummonStream, (void *)a_73A5B0, "079 sub_7B53F0" },
		{ game::MAG_079_LoadCb_ClearBusy, (void *)a_73A5E0, "079 sub_7B5420" },
		{ game::MAG_079_LoadCb_ClearBusy_2, (void *)a_73A5E0, "079 sub_7B5430" },
		{ game::MAG_079_LoadCb_ClearBusy_3, (void *)a_73A5E0, "079 sub_7B5440" },
		{ game::MAG_079_LoadCb_ClearBusy_4, (void *)a_73A5E0, "079 sub_7B5450" },
		{ game::MAG_079_LoadScript_RecordFile, (void *)a_73A620, "079 sub_7B5460" },
		{ game::MAG_079_FileLoadScript_State1_LoadScriptWait, (void *)a_73A660, "079 sub_7B54A0" },
		{ game::MAG_079_FileLoadScript_State2_Empty, (void *)a_73A6D0, "079 nullsub_1377" },
		{ game::MAG_079_DirSyncGate, (void *)a_73B640, "079 sub_7B56E0" },
		{ game::MAG_079_DirPhaseGE, (void *)a_73B7E0, "079 sub_7B5730" },
		{ game::MAG_079_Director_State2_ArmLoadScriptStep, (void *)a_73A930, "079 sub_7B5790" },
		{ game::MAG_079_LoadScript_TryArmStep, (void *)a_73A950, "079 sub_7B57B0" },
		{ game::MAG_079_SetPauseSwitch, (void *)a_73BB60, "079 sub_7B5830" },
		{ game::MAG_079_Director_State5_WaitDirGate2, (void *)a_73B8A0, "079 sub_7B58A0" },
		{ game::MAG_079_Director_State6_WaitDirPhase2, (void *)a_73B8C0, "079 sub_7B58C0" },
		{ game::MAG_079_Director_State9_Finish, (void *)a_7475A0, "079 sub_7B5970" },
		{ game::MAG_079_Director_State10_Empty, (void *)a_73A6D0, "079 nullsub_1378" },
		{ game::MAG_079_St_Empty, (void *)a_73A6D0, "079 nullsub_1379" },
		{ game::MAG_079_SpawnChildTask, (void *)a_73C100, "079 sub_7B5AE0" },
		{ game::MAG_079_PrimSeqTask0, (void *)a_73A380, "079 sub_7B5B80" },
		{ game::MAG_079_DrawPrimModel, (void *)a_73C280, "079 sub_7B5C50" },
		{ game::MAG_079_EmitG3, (void *)a_73D770, "079 sub_7B7000" },
		{ game::MAG_079_EmitG4, (void *)a_73D9C0, "079 sub_7B7250" },
		{ game::MAG_079_PrimSeq0_State2_Empty, (void *)a_73A6D0, "079 nullsub_1380" },
		{ game::MAG_079_PrimSeqTask1, (void *)a_73A380, "079 sub_7B80B0" },
		{ game::MAG_079_PrimSeq1_State2_Empty, (void *)a_73A6D0, "079 nullsub_1381" },
		{ game::MAG_079_PrimSeqTask2, (void *)a_73A380, "079 sub_7B81C0" },
		{ game::MAG_079_PrimSeq2_State2_Empty, (void *)a_73A6D0, "079 nullsub_1382" },
		{ game::MAG_079_ActorSystemTask, (void *)a_73A380, "079 sub_7B82C0" },
		{ game::MAG_079_DirectorSetup, (void *)a_73F990, "079 sub_7B8350" },
		{ game::MAG_079_Director_StartPositions, (void *)a_73FC20, "079 sub_7B85E0" },
		{ game::MAG_079_Director_Mode3Setup, (void *)a_73FE90, "079 sub_7B8850" },
		{ game::MAG_079_Director_TargetPositions, (void *)a_73FFB0, "079 sub_7B8970" },
		{ game::MAG_079_RelocateActorData, (void *)a_740210, "079 sub_7B8BD0" },
		{ game::MAG_079_ActorMotion, (void *)a_740910, "079 sub_7B9290" },
		{ game::MAG_079_DampValue, (void *)a_740F70, "079 sub_7B98F0" },
		{ game::MAG_079_DampVector, (void *)a_740FC0, "079 sub_7B9940" },
		{ game::MAG_079_Instance_AdvanceTexFrame, (void *)a_741050, "079 sub_7B99D0" },
		{ game::MAG_079_Instance_BuildTransform, (void *)a_741120, "079 sub_7B9AA0" },
		{ game::MAG_079_Particle_LifeCountdown, (void *)a_741B60, "079 sub_7BA4E0" },
		{ game::MAG_079_RandRange, (void *)a_742290, "079 au_re__rand_26" },
		{ game::MAG_079_RandRangeWide, (void *)a_7422C0, "079 sub_7BAC40" },
		{ game::MAG_079_RandVec3, (void *)a_742300, "079 sub_7BAC80" },
		{ game::MAG_079_JitterAngles3, (void *)a_742350, "079 sub_7BACD0" },
		{ game::MAG_079_RandAngleJitter, (void *)a_7423A0, "079 au_re__rand_26_0" },
		{ game::MAG_079_GetWord8Minus1, (void *)a_7424B0, "079 sub_7BAE30" },
		{ game::MAG_079_Particle_AttachToOwner, (void *)a_7424C0, "079 sub_7BAE40" },
		{ game::MAG_079_ParticleList_Append, (void *)a_742CA0, "079 sub_7BB620" },
		{ game::MAG_079_ParticleList_Unlink, (void *)a_742CD0, "079 sub_7BB650" },
		{ game::MAG_079_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "079 sub_7BB830" },
		{ game::MAG_079_ActorSystem_State2_Empty, (void *)a_73A6D0, "079 nullsub_1383" },
		{ game::MAG_079_SparkThrowerTask, (void *)a_73A380, "079 sub_7BBC20" },
		{ game::InitEffectSequenceFromData_c10, (void *)a_7435E0, "079 InitEffectSequenceFromData_c10" },
		{ game::MAG_079_SpriteStrip_DrawQuads, (void *)a_743720, "079 sub_7BC110" },
		{ game::MAG_079_StepCounterLimit, (void *)a_743C20, "079 sub_7BC5C0" },
		{ game::MAG_079_SparkPair_State2_Empty, (void *)a_73A6D0, "079 nullsub_1384" },
		{ game::MAG_079_SparkThrower_State2_Empty, (void *)a_73A6D0, "079 nullsub_1385" },
		{ game::MAG_079_Debris_State2_Empty, (void *)a_73A6D0, "079 nullsub_1386" },
		{ game::MAG_079_DebrisThrower_State3_Empty, (void *)a_73A6D0, "079 nullsub_1387" },
		{ game::MAG_079_FlakeThrowerTask, (void *)a_73A380, "079 sub_7BCB40" },
		{ game::MAG_079_Flake_State2_Empty, (void *)a_73A6D0, "079 nullsub_1388" },
		{ game::MAG_079_FlakeThrower_State2_Empty, (void *)a_73A6D0, "079 nullsub_1389" },
		{ game::MAG_079_LightFadeIn, (void *)a_73A380, "079 sub_7BCD50" },
		{ game::MAG_079_LightFadeIn_State0_ResetEntityRecords, (void *)a_7335D0, "079 sub_7BCDB0" },
		{ game::MAG_079_LightFadeIn_State2_Empty, (void *)a_73A6D0, "079 nullsub_1390" },
		{ game::MAG_079_LightFadeOut, (void *)a_73A380, "079 sub_7BCE40" },
		{ game::MAG_079_LightFadeOut_State2_Empty, (void *)a_73A6D0, "079 nullsub_1391" },
		{ game::MAG_079_PrimSeqTask3, (void *)a_73A380, "079 sub_7BDAD0" },
		{ game::MAG_079_PrimSeq3_State2_Empty, (void *)a_73A6D0, "079 nullsub_1392" },
		{ game::MAG_079_SetEntityColourTint, (void *)a_733950, "079 sub_7BDDF0" },
		{ game::MAG_079_CasterModel_State10_Empty, (void *)a_73A6D0, "079 nullsub_1393" },
		{ game::MAG_079_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "079 MAG_079_sub_7BE100" },
		{ game::MAG_079_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "079 MAG_079_sub_7BE120" },
		{ game::MAG_079_Emitter_State4_Empty, (void *)a_73A6D0, "079 nullsub_1394" },
		{ game::MAG_079_Master_State4_NextTargetLoop, (void *)a_747500, "079 MAG_079_sub_7BE150" },
		{ game::MAG_079_Master_State5_Advance, (void *)a_73A0D0, "079 MAG_079_sub_7BE190" },
		{ game::MAG_079_Master_State9_RequestBattleSync, (void *)a_747560, "079 MAG_079_sub_7BE240" },
		{ game::MAG_079_Master_State10_WaitBattleSyncDone, (void *)a_747590, "079 MAG_079_sub_7BE270" },
		{ game::MAG_079_Master_State11_Finish, (void *)a_7475A0, "079 MAG_079_sub_7BE280" },
		{ game::MAG_079_Master_State12_Empty, (void *)a_73A6D0, "079 nullsub_1376" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_079_MasterTask, (void *)ud_7B4D80, "079 ud_7B4D80" },
		{ game::MAG_079_Master_State2_CarveActorPools, (void *)ud_7B4F40, "079 ud_7B4F40" },
		{ game::MAG_079_Master_ClearActorPools, (void *)ud_7B4F80, "079 ud_7B4F80" },
		{ game::MAG_079_Emitter_State0_StartLoadScriptAndDirector, (void *)ud_7B5070, "079 ud_7B5070" },
		{ game::MAG_079_InitLoadContext, (void *)ud_7B50C0, "079 ud_7B50C0" },
		{ game::MAG_079_InitPhaseData, (void *)ud_7B5520, "079 ud_7B5520" },
		{ game::MAG_079_DirectorTask, (void *)ud_7B55B0, "079 ud_7B55B0" },
		{ game::MAG_079_Director_PhaseBookkeeping, (void *)ud_7B5650, "079 ud_7B5650" },
		{ game::MAG_079_Director_State0_ClaimVoiceMusicFadeAtCut1, (void *)ud_7B56A0, "079 ud_7B56A0" },
		{ game::MAG_079_Director_State1_HideMonstersAtCut1, (void *)ud_7B5710, "079 ud_7B5710" },
		{ game::MAG_079_HidePartyModels, (void *)ud_7B5750, "079 ud_7B5750" },
		{ game::MAG_079_Director_State3_ToggleAtCutFrame62, (void *)ud_7B57F0, "079 ud_7B57F0" },
		{ game::MAG_079_Director_State4_WaitCutFrame63, (void *)ud_7B5880, "079 ud_7B5880" },
		{ game::MAG_079_Director_State7_DamageCueAtFrame5A, (void *)ud_7B58E0, "079 ud_7B58E0" },
		{ game::MAG_079_Director_State8_RestoreAndReleaseVoiceAtCut3, (void *)ud_7B5900, "079 ud_7B5900" },
		{ game::MAG_079_ShowPartyModels, (void *)ud_7B5930, "079 ud_7B5930" },
		{ game::MAG_079_Director_OnCutChange, (void *)ud_7B59B0, "079 ud_7B59B0" },
		{ game::MAG_079_SpawnPrimSeqTask, (void *)ud_7B5B30, "079 ud_7B5B30" },
		{ game::MAG_079_PrimSeq0_State0_Init, (void *)ud_7B5BE0, "079 ud_7B5BE0" },
		{ game::MAG_079_PrimSeq_DrawCallback, (void *)ud_7B5D80, "079 ud_7B5D80" },
		{ game::MAG_079_RenderPrimLists, (void *)ud_7B6020, "079 ud_7B6020" },
		{ game::MAG_079_PrimList_FlatTri, (void *)ud_7B6150, "079 ud_7B6150" },
		{ game::MAG_079_PrimList_FlatQuad, (void *)ud_7B6370, "079 ud_7B6370" },
		{ game::MAG_079_PrimList_TexTri, (void *)ud_7B6600, "079 ud_7B6600" },
		{ game::MAG_079_PrimList_TexQuad, (void *)ud_7B6A80, "079 ud_7B6A80" },
		{ game::MAG_079_PrimList_GT3Scroll, (void *)ud_7B7530, "079 ud_7B7530" },
		{ game::MAG_079_PrimList_GT4, (void *)ud_7B7A30, "079 ud_7B7A30" },
		{ game::MAG_079_PrimSeq0_State1_Play, (void *)ud_7B8080, "079 ud_7B8080" },
		{ game::MAG_079_PrimSeq1_State0_InitFromTick4, (void *)ud_7B8110, "079 ud_7B8110" },
		{ game::MAG_079_PrimSeq1_State1_FollowAnchor, (void *)ud_7B8170, "079 ud_7B8170" },
		{ game::MAG_079_PrimSeq2_State0_Init, (void *)ud_7B8220, "079 ud_7B8220" },
		{ game::MAG_079_PrimSeq2_State1_Play, (void *)ud_7B8290, "079 ud_7B8290" },
		{ game::MAG_079_ActorSystem_State0_StartActorSystemAtFrame, (void *)ud_7B8320, "079 ud_7B8320" },
		{ game::MAG_079_ResetTrailHistory, (void *)ud_7B8500, "079 ud_7B8500" },
		{ game::MAG_079_FillTrailRows, (void *)ud_7B8780, "079 ud_7B8780" },
		{ game::MAG_079_ActSys_Step, (void *)ud_7B8E30, "079 ud_7B8E30" },
		{ game::MAG_079_ActSys_DrawPass, (void *)ud_7B8EC0, "079 ud_7B8EC0" },
		{ game::MAG_079_ActSys_DrawPrimActor, (void *)ud_7B8F20, "079 ud_7B8F20" },
		{ game::MAG_079_ActSys_DrawSpritePass, (void *)ud_7B9080, "079 ud_7B9080" },
		{ game::MAG_079_ActSys_DrawActorSprite, (void *)ud_7B9140, "079 ud_7B9140" },
		{ game::MAG_079_ActSys_UpdatePass, (void *)ud_7B91C0, "079 ud_7B91C0" },
		{ game::MAG_079_ActorStep, (void *)ud_7B9200, "079 ud_7B9200" },
		{ game::MAG_079_MorphVertexBlock, (void *)ud_7BA230, "079 ud_7BA230" },
		{ game::MAG_079_Instance_EndOfLifeTest, (void *)ud_7BA340, "079 ud_7BA340" },
		{ game::MAG_079_Instance_SpawnFollowUp, (void *)ud_7BA3C0, "079 ud_7BA3C0" },
		{ game::MAG_079_AllocParticle, (void *)ud_7BA3F0, "079 ud_7BA3F0" },
		{ game::MAG_079_ActSys_UpdateEmitter, (void *)ud_7BA490, "079 ud_7BA490" },
		{ game::MAG_079_ActSys_EmitterSpawnStep, (void *)ud_7BA510, "079 ud_7BA510" },
		{ game::MAG_079_ActSys_SpawnParticles, (void *)ud_7BA590, "079 ud_7BA590" },
		{ game::MAG_079_ActSys_AllocParticleSlot, (void *)ud_7BAD70, "079 ud_7BAD70" },
		{ game::MAG_079_Actor_InterpolatePosition, (void *)ud_7BAF90, "079 ud_7BAF90" },
		{ game::MAG_079_Particle_Init, (void *)ud_7BB680, "079 ud_7BB680" },
		{ game::MAG_079_Actor_PlaceAtPointOrRandomBone, (void *)ud_7BB970, "079 ud_7BB970" },
		{ game::MAG_079_SpawnSubEffects, (void *)ud_7BBAD0, "079 ud_7BBAD0" },
		{ game::MAG_079_Director_SpawnGroupEffects, (void *)ud_7BBB60, "079 ud_7BBB60" },
		{ game::MAG_079_ActorSystem_State1_StepActorSystem, (void *)ud_7BBBF0, "079 ud_7BBBF0" },
		{ game::MAG_079_SparkThrower_State0_SetCounter32, (void *)ud_7BBC80, "079 ud_7BBC80" },
		{ game::MAG_079_SparkThrower_State1_BurstPerTick, (void *)ud_7BBCA0, "079 ud_7BBCA0" },
		{ game::MAG_079_SpawnSparkPair, (void *)ud_7BBCD0, "079 ud_7BBCD0" },
		{ game::MAG_079_SparkPairTask, (void *)ud_7BBDF0, "079 ud_7BBDF0" },
		{ game::MAG_079_DrawSpriteNode, (void *)ud_7BBEB0, "079 ud_7BBEB0" },
		{ game::MAG_079_SparkPair_State0_InitSprite, (void *)ud_7BC580, "079 ud_7BC580" },
		{ game::MAG_079_SparkPair_State1_Animate, (void *)ud_7BC5A0, "079 ud_7BC5A0" },
		{ game::MAG_079_DebrisThrowerTask, (void *)ud_7BC610, "079 ud_7BC610" },
		{ game::au_re_BdPlaySE_2, (void *)ud_7BC670, "079 ud_7BC670" },
		{ game::MAG_079_DebrisThrower_State1_TwoBurstsPerTick, (void *)ud_7BC6B0, "079 ud_7BC6B0" },
		{ game::MAG_079_SpawnDebrisBurst, (void *)ud_7BC720, "079 ud_7BC720" },
		{ game::MAG_079_DebrisTask, (void *)ud_7BC840, "079 ud_7BC840" },
		{ game::MAG_079_DrawScaledSpriteNode, (void *)ud_7BC950, "079 ud_7BC950" },
		{ game::MAG_079_Debris_State0_InitSprite, (void *)ud_7BCAA0, "079 ud_7BCAA0" },
		{ game::MAG_079_Debris_State1_Animate, (void *)ud_7BCAC0, "079 ud_7BCAC0" },
		{ game::MAG_079_DebrisThrower_State2_BurstOffBone, (void *)ud_7BCAF0, "079 ud_7BCAF0" },
		{ game::MAG_079_FlakeThrower_State0_SetCounter26, (void *)ud_7BCBA0, "079 ud_7BCBA0" },
		{ game::MAG_079_FlakeThrower_State1_FlakePerTick, (void *)ud_7BCBC0, "079 ud_7BCBC0" },
		{ game::MAG_079_SpawnFlake, (void *)ud_7BCBF0, "079 ud_7BCBF0" },
		{ game::MAG_079_FlakeTask, (void *)ud_7BCC80, "079 ud_7BCC80" },
		{ game::MAG_079_Flake_State0_InitSprite, (void *)ud_7BCCF0, "079 ud_7BCCF0" },
		{ game::MAG_079_Flake_State1_Animate, (void *)ud_7BCD10, "079 ud_7BCD10" },
		{ game::MAG_079_LightFadeIn_State1_RampUp, (void *)ud_7BCDF0, "079 ud_7BCDF0" },
		{ game::MAG_079_LightFadeOut_State0_WaitTick90, (void *)ud_7BCEA0, "079 ud_7BCEA0" },
		{ game::MAG_079_LightFadeOut_State1_FadeOut, (void *)ud_7BCEC0, "079 ud_7BCEC0" },
		{ game::MAG_079_CasterModelTask, (void *)ud_7BCF10, "079 ud_7BCF10" },
		{ game::MAG_079_DrawCasterModelCopy, (void *)ud_7BD020, "079 ud_7BD020" },
		{ game::MAG_079_RenderMorphModel, (void *)ud_7BD110, "079 ud_7BD110" },
		{ game::MAG_079_CasterModel_State0_SetupModelBlock, (void *)ud_7BD800, "079 ud_7BD800" },
		{ game::MAG_079_InitModelBlock, (void *)ud_7BD890, "079 ud_7BD890" },
		{ game::MAG_079_CasterModel_State1_ShowAtCut2, (void *)ud_7BD900, "079 ud_7BD900" },
		{ game::MAG_079_ComputeBonePivot, (void *)ud_7BD9B0, "079 ud_7BD9B0" },
		{ game::MAG_079_CasterModel_State2_ScaleThenSpawnSeq, (void *)ud_7BDA60, "079 ud_7BDA60" },
		{ game::MAG_079_PrimSeq3_State0_Init, (void *)ud_7BDB30, "079 ud_7BDB30" },
		{ game::MAG_079_PrimSeq3_State1_FollowAnchor, (void *)ud_7BDB90, "079 ud_7BDB90" },
		{ game::MAG_079_ModelBlock_MotionStep, (void *)ud_7BDBE0, "079 ud_7BDBE0" },
		{ game::MAG_079_CasterModel_State3_Morph, (void *)ud_7BDCB0, "079 ud_7BDCB0" },
		{ game::MAG_079_CasterModel_State4_Rise, (void *)ud_7BDD00, "079 ud_7BDD00" },
		{ game::MAG_079_CasterModel_State5_Apex, (void *)ud_7BDD40, "079 ud_7BDD40" },
		{ game::MAG_079_CasterModel_State6_ActorSystemAndScale, (void *)ud_7BDD70, "079 ud_7BDD70" },
		{ game::MAG_079_PlaceTargetAtAnchor, (void *)ud_7BDEF0, "079 ud_7BDEF0" },
		{ game::MAG_079_CasterModel_State8_FadeCasterGrow, (void *)ud_7BDF90, "079 ud_7BDF90" },
		{ game::MAG_079_CasterModel_State9_FadeTarget, (void *)ud_7BE050, "079 ud_7BE050" },
		{ game::MAG_079_Emitter_State1_ApplyActionResults, (void *)ud_7BE0C0, "079 ud_7BE0C0" },
		{ game::MAG_079_Master_LoadFinalModel, (void *)ud_7BE1A0, "079 ud_7BE1A0" },
		{ game::MAG_079_LoadCb_SetLoadFlag, (void *)ud_7BE1E0, "079 ud_7BE1E0" },
		{ game::MAG_079_Master_HandOverFinalModel, (void *)ud_7BE1F0, "079 ud_7BE1F0" },
		{ game::MAG_079_Master_State8_MusicVolumeWhenLoaded, (void *)ud_7BE220, "079 ud_7BE220" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag079_ultimecia_final_death()
	{
		act::register_module(79);
		for (const act::udeath::ModPort *p = act::udeath::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(79, p->addr, p->port, p->name);
		for (const act::udeath::ModPort *p = act::udeath::PORTS; p->addr; p++)
			act::register_module_port(79, p->addr, p->port, p->name);
	}
}

