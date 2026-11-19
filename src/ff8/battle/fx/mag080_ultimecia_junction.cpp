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
namespace ujunct
{
	static const Mod MOD_080 = { "ultijunction", 80, game::MAG_080_ULTIMECIA_JUNCTIONING_TO_GRIEVER_FL, game::MAG_079_GRIEVER_DEATH_FL,
		{ {}, {}, game::unk_157B2E0, {}, {}, {}, {}, {}, game::unk_25DF018, game::MAG_080_ActorPoolPtr, {}, {}, game::unk_25D4938, {}, {}, {}, game::unk_25DAF00, {}, {}, {}, {}, {}, {}, {}, game::unk_25DE010, {}, game::MAG_080_EmitterQueue, {}, {}, {}, {}, {}, game::unk_25DF008, {}, {}, game::unk_25DF2F0, {}, game::MAG_080_ScratchSP, {}, game::MAG_080_ActorStatePtr },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_080_EmitterTask, game::MAG_080_Emitter_State0_SetupDirectorData, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_080_DirSyncGate, {}, game::MAG_080_DirPhaseGE, {}, {}, {}, {}, {}, {}, game::MAG_080_ResetTrailHistory, game::MAG_080_Director_StartPositions, game::MAG_080_FillTrailRows, game::MAG_080_Director_Mode3Setup, game::MAG_080_Director_TargetPositions, game::MAG_080_RelocateActorData, {}, {}, game::MAG_080_DampValue, game::MAG_080_DampVector, {}, {}, game::MAG_080_MorphVertexBlock, {}, {}, {}, game::MAG_080_RandRange, game::MAG_080_RandRangeWide, game::MAG_080_RandAngleJitter, game::MAG_080_Particle_AttachToOwner, game::MAG_080_Actor_InterpolatePosition, game::MAG_080_ParticleList_Append, game::MAG_080_ParticleList_Unlink, game::MAG_080_Particle_PlaceAtAnchorSet, game::MAG_080_Particle_PlaceAtPoint, game::MAG_080_SpriteStrip_DrawQuads, {}, game::MAG_080_Emitter_State1_ApplyActionResults, game::MAG_080_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_080_Emitter_State3_FinishReleaseRoot, game::MAG_080_Emitter_State4_Empty, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id DIR_PTR = game::unk_157B2E0;
	static constexpr game::Id SPRITE_CURSOR = game::MAG_080_SpriteCursor;
	static constexpr game::Id MODEL_BLOCK = game::MAG_080_TargetModelBlock;
	static constexpr game::Id BOLT_BASE = game::unk_1580A6C;
	static constexpr game::Id Q_PRIM = game::MAG_080_PrimModelQueue;
	static constexpr game::Id BLEND_BUF = game::MAG_080_BlendBuffer;
	static constexpr game::Id PRIM_CB = game::MAG_080_PrimPlayer_ObjectCallback;
	static constexpr game::Id FA_CURSOR = game::BATTLE_PACKET_WRITE_CURSOR;
	static constexpr game::Id PHASE_DATA = DIR_PTR;
	static constexpr game::Id PACKET_CURSOR = SPRITE_CURSOR;
	static constexpr game::Id SCRATCH_SP = game::MAG_080_ScratchSP;
	static constexpr game::Id ACTOR_STATE = game::MAG_080_ActorStatePtr;

	namespace gx
	{
		static inline uint32_t GTE_MVMVA_ColorV0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_MVMVA_ColorV0))(); }
		static inline uint32_t GTE_MVMVA_LightV0_Bk() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
		static inline uint32_t GTE_SetBackgroundVector(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetBackgroundVector))(a1, a2, a3); }
		static inline uint32_t GTE_SetLightMatrix(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_SetLightMatrix))(a1); }
		static inline uint32_t sub_45DEA0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::sub_45DEA0))(a1); }
		static inline uint32_t sub_45E0E0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadIR123FromBytes))(a1); }
		static inline uint32_t sub_45E410(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreIR123LowBytes))(a1); }
		static inline uint32_t GTE_ReadOTZ_SZ3div4(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadOTZ_SZ3div4))(a1); }
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

	uint32_t __cdecl ud_7AB6F0(uint32_t a1);
	uint32_t __cdecl ud_7AB890(uint32_t a1);
	uint32_t __cdecl ud_7AB8F0(uint32_t a1);
	uint32_t __cdecl ud_7AB930(void);
	uint32_t __cdecl ud_7ABA20(uint32_t a1);
	uint32_t __cdecl ud_7ABA50(uint32_t a1);
	uint32_t __cdecl ud_7ABAF0(uint32_t a1);
	uint32_t __cdecl ud_7ABBA0(uint32_t a1);
	uint32_t __cdecl ud_7ABBF0(uint32_t a1);
	uint32_t __cdecl ud_7ABC70(uint32_t a1);
	uint32_t __cdecl ud_7ABCB0(void);
	uint32_t __cdecl ud_7ABCF0(uint32_t a1);
	uint32_t __cdecl ud_7ABD50(uint32_t a1);
	uint32_t __cdecl ud_7ABD90(uint32_t a1);
	uint32_t __cdecl ud_7ABDB0(uint32_t a1);
	uint32_t __cdecl ud_7ABDD0(uint32_t a1);
	uint32_t __cdecl ud_7ABE00(void);
	uint32_t __cdecl ud_7ABE80(uint32_t a1);
	uint32_t __cdecl ud_7ABF40(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl ud_7ABF90(uint32_t a1);
	uint32_t __cdecl ud_7AC000(uint32_t a1);
	uint32_t __cdecl ud_7AC060(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ud_7AC0B0(uint32_t a1);
	uint32_t __cdecl ud_7AC1E0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ud_7AC420(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7AC550(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7AC770(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7ACA00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7ACEF0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7ADA10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7ADF70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7AE640(uint32_t a1);
	uint32_t __cdecl ud_7AE6F0(uint32_t a1);
	uint32_t __cdecl ud_7AE710(uint32_t a1);
	uint32_t __cdecl ud_7AE740(uint32_t a1);
	uint32_t __cdecl ud_7AE7D0(uint32_t a1);
	uint32_t __cdecl ud_7AE840(uint32_t a1);
	uint32_t __cdecl ud_7AEF10(uint32_t a1);
	uint32_t __cdecl ud_7AEF30(uint32_t a1);
	uint32_t __cdecl ud_7AEFA0(uint32_t a1);
	uint32_t __cdecl ud_7AEFF0(uint32_t a1);
	uint32_t __cdecl ud_7AF030(uint32_t a1);
	uint32_t __cdecl ud_7AF130(uint32_t a1);
	uint32_t __cdecl ud_7AF190(uint32_t a1);
	uint32_t __cdecl ud_7AF270(uint32_t a1);
	uint32_t __cdecl ud_7AF320(uint32_t a1);
	uint32_t __cdecl ud_7AF340(uint32_t a1);
	uint32_t __cdecl ud_7AF390(uint32_t a1);
	uint32_t __cdecl ud_7AF4C0(void);
	uint32_t __cdecl ud_7AF4D0(void);
	uint32_t __cdecl ud_7AF5E0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ud_7AF860(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7AF9F0(void);
	uint32_t __cdecl ud_7AFD80(void);
	uint32_t __cdecl ud_7AFDD0(uint32_t a1);
	uint32_t __cdecl ud_7AFEA0(void);
	uint32_t __cdecl ud_7AFEC0(uint32_t a1);
	uint32_t __cdecl ud_7AFFB0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ud_7B06A0(uint32_t a1);
	uint32_t __cdecl ud_7B0840(uint32_t a1);
	uint32_t __cdecl ud_7B0860(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ud_7B09E0(uint32_t a1);
	uint32_t __cdecl ud_7B0B40(uint32_t a1);
	uint32_t __cdecl ud_7B0D20(uint32_t a1);
	uint32_t __cdecl ud_7B0FA0(void);
	uint32_t __cdecl ud_7B1650(uint32_t a1);
	uint32_t __cdecl ud_7B16E0(void);
	uint32_t __cdecl ud_7B1740(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_7B18A0(void);
	uint32_t __cdecl ud_7B1960(uint32_t a1);
	uint32_t __cdecl ud_7B19E0(void);
	uint32_t __cdecl ud_7B1A20(uint32_t a1);
	uint32_t __cdecl ud_7B2A50(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_7B2B60(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_7B2BE0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_7B2C10(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ud_7B2CB0(uint32_t a1);
	uint32_t __cdecl ud_7B2D30(uint32_t a1);
	uint32_t __cdecl ud_7B2DB0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ud_7B37B0(uint32_t a1);
	uint32_t __cdecl ud_7B42A0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_7B4330(void);
	uint32_t __cdecl ud_7B43C0(uint32_t a1);
	uint32_t __cdecl ud_7B43F0(void);
	uint32_t __cdecl ud_7B4400(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ud_7B4480(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
	uint32_t __cdecl ud_7B44F0(uint32_t a1);
	uint32_t __cdecl ud_7B4540(uint32_t a1);
	uint32_t __cdecl ud_7B45B0(uint32_t a1);
	uint32_t __cdecl ud_7B4620(uint32_t a1);
	uint32_t __cdecl ud_7B4650(uint32_t a1);
	uint32_t __cdecl ud_7B4670(uint32_t a1);
	uint32_t __cdecl ud_7B4750(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ud_7B4800(uint32_t a1);
	uint32_t __cdecl ud_7B4820(uint32_t a1);
	uint32_t __cdecl ud_7B4880(uint32_t a1);
	uint32_t __cdecl ud_7B49F0(uint32_t a1);
	uint32_t __cdecl ud_7B4A50(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace ujunct
{
	uint32_t __cdecl ud_7AB6F0(uint32_t a1)
	{
		g_mod = &MOD_080;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::MAG_080_ScratchSP)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_25DAF00)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_080_Master_State0_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_080_Master_State1_PutCasterAboveTarget);
		states[2] = game_addr(game::MAG_080_Master_State2_CarveActorPools);
		states[3] = game_addr(game::MAG_080_Master_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_080_Master_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_080_Master_State5_Advance);
		states[6] = game_addr(game::MAG_080_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_080_Master_State7_RequestBattleSync);
		states[8] = game_addr(game::MAG_080_Master_State8_WaitBattleSyncDone);
		states[9] = game_addr(game::MAG_080_Master_State9_Finish);
		states[10] = game_addr(game::MAG_080_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_25DE424));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_25DD81C));
			MEM<uint32_t>(game_addr(game::MAG_080_SpriteCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_25DAEFC)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_25DE420));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_25DD818));
			MEM<uint32_t>(game_addr(game::MAG_080_SpriteCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_25DAEFC)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_25DF2F0)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25DF008)) = 0;
		static constexpr game::Id queues[8] = { game::MAG_080_EmitterQueue, game::MAG_080_DirectorQueue, game::unk_25DEFF8, game::unk_25DE010, game::MAG_080_PrimModelQueue, game::MAG_080_TaskQueue, game::unk_25DCF38, game::unk_25DE2A0 };
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

	uint32_t __cdecl ud_7AB890(uint32_t a1)
	{
		const uint32_t c = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2C) * 0x9C;
		const uint32_t t = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2D) * 0x9C;
		U32(c, 0x1C) = U32(t, 0x1C);
		U32(c, 0x20) = U32(t, 0x20);
		U16(c, 0x1E) = (uint16_t)(U16(c, 0x1E) + 0xF918);
		S32(c, 0x54) = S16(c, 0x1C);
		S32(c, 0x58) = S16(c, 0x1E);
		S32(c, 0x5C) = S16(c, 0x20);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7AB8F0(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::MAG_080_FileArenaCursor));
		MEM<uint32_t>(game_addr(game::MAG_080_ParticlePoolPtr)) = p;
		p += 0x870;
		MEM<uint32_t>(game_addr(game::MAG_080_ActorPoolPtr)) = p;
		p += 0x10680;
		MEM<uint32_t>(game_addr(game::MAG_080_FileArenaCursor)) = p;
		ud_7AB930();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7AB930(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_080_ParticlePoolPtr)), 0x870);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_080_ActorPoolPtr)), 0x10680);
		MEM<uint16_t>(game_addr(game::MAG_080_ParticleCursor)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25DF018)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25DEE58)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25D4940)) = 0;
		return 0;
	}

	uint32_t __cdecl ud_7ABA20(uint32_t a1)
	{
		ud_7ABA50(a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_080_DirectorQueue), game_addr(game::MAG_080_Director), 0x48, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7ABA50(uint32_t a1)
	{
		const uint32_t t = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2D) * 0x9C;
		const uint32_t c = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2C) * 0x9C;
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(DIR_PTR)), 0x54);
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		U32(d, 0x1C) = t;
		U32(d, 8) = U32(t, 0x1C);
		U32(d, 0xC) = U32(t, 0x20);
		U32(d, 0) = U32(c, 0x1C);
		U32(d, 4) = U32(c, 0x20);
		const int16_t dz = (int16_t)(U16(d, 0xC) - U16(d, 4));
		const int16_t dx = (int16_t)(U16(d, 8) - U16(d, 0));
		const uint32_t a = x::CartesianToGameAngle((uint32_t)(int32_t)dx, (uint32_t)(int32_t)dz);
		U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x4A) = (uint16_t)((a - 0x800) & 0xFFF);
		return 0;
	}

	uint32_t __cdecl ud_7ABAF0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[13];
		states[0] = game_addr(game::MAG_080_Director_State0_ClaimVoiceSlot);
		states[1] = game_addr(game::MAG_080_Director_State1_WaitDirGate1);
		states[2] = game_addr(game::MAG_080_Director_State2_HideMonstersAtCut1);
		states[3] = game_addr(game::MAG_080_Director_State3_WaitCutFrame2D);
		states[4] = game_addr(game::MAG_080_Director_State4_WaitDirGate2);
		states[5] = game_addr(game::MAG_080_Director_State5_WaitDirPhase2);
		states[6] = game_addr(game::MAG_080_Director_State6_WaitCutFrame76);
		states[7] = game_addr(game::MAG_080_Director_State7_WaitDirGate3);
		states[8] = game_addr(game::MAG_080_Director_State8_WaitDirPhase3);
		states[9] = game_addr(game::MAG_080_Director_State9_DamageCueAtFrameA0);
		states[10] = game_addr(game::MAG_080_Director_State10_RestoreAndReleaseVoiceAtCut4);
		states[11] = game_addr(game::MAG_080_Director_State11_Finish);
		states[12] = game_addr(game::MAG_080_Director_State12_Empty);
		ud_7ABBA0(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_7ABBA0(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			ud_7ABE80(a1);
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

	uint32_t __cdecl ud_7ABBF0(uint32_t a1)
	{
		const uint32_t v = x::BdSound_ClaimVoiceSlot(game_addr(game::unk_175AAE0), 1, 0x80);
		U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x52) = (uint16_t)v;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7ABC70(uint32_t a1)
	{
		if (a_73B7E0(1) == 0)
			return 0;
		ud_7ABCB0();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl ud_7ABCB0(void)
	{
		uint32_t save = MEM<uint32_t>(game_addr(game::unk_157B2E0)) + 0x20;
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

	uint32_t __cdecl ud_7ABCF0(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(PHASE_DATA)), 0x46) >= 0x2D)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl ud_7ABD50(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(PHASE_DATA)), 0x46) >= 0x76)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl ud_7ABD90(uint32_t a1)
	{
		if (a_73B7E0(3) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl ud_7ABDB0(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_157B2E0));
		if (S16(d, 0x46) < 0xA0)
			return d;
		U16(d, 0x48) = 1;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl ud_7ABDD0(uint32_t a1)
	{
		if (a_73B640(4) == 0)
			return 0;
		ud_7ABE00();
		x::sub_4A2940((uint32_t)(int32_t)S16(MEM<uint32_t>(game_addr(game::unk_157B2E0)), 0x52));
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl ud_7ABE00(void)
	{
		uint32_t save = MEM<uint32_t>(game_addr(game::unk_157B2E0)) + 0x20;
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

	uint32_t __cdecl ud_7ABE80(uint32_t a1)
	{
		const int32_t cut = S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x40);
		if (cut == 1)
		{
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_080_TaskQueue), game_addr(game::MAG_080_Junction_Task), 0xA0, a1);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_080_TaskQueue), game_addr(game::MAG_080_StageWobbleInTask), 0xA0, a1);
		}
		else if (cut == 2)
		{
			ud_7ABF40(a1, game_addr(game::MAG_080_RisingModel_Task), game_addr(game::unk_1754B2C), 0xC4, 0, 1);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_080_TaskQueue), game_addr(game::MAG_080_ParticleEmitter_Task), 0xA0, a1);
		}
		else if (cut == 3)
		{
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_080_TaskQueue), game_addr(game::MAG_080_FlakeThrowerTask), 0xA0, a1);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_080_TaskQueue), game_addr(game::MAG_080_StageWobbleOutTask), 0xA0, a1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7ABF40(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PRIM), a2, 0x180, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		S32(t, 0x78) = (int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl ud_7ABF90(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_080_RisingModel_State0);
		states[1] = game_addr(game::MAG_080_RisingModel_State1);
		states[2] = game_addr(game::MAG_080_RisingModel_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		const uint32_t w0 = U32(node, 0x1C);
		const uint32_t w1 = U32(node, 0x20);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		U32(d, 0x10) = w0;
		U32(d, 0x14) = w1;
		const uint8_t status = U8(node, 0x26);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_7AC000(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		ud_7AC060(node, node + 0x1C, (uint32_t)U16(node, 0x84));
		U16(node, 0x88) = 0;
		ud_7AC0B0(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7AC060(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t c = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2C) * 0x9C;
		int16_t k = (int16_t)a3;
		if (k >= 0x78)
			k = 0x77;
		U32(a2, 0) = U32(c, 0x1C);
		U32(a2, 4) = U32(c, 0x20);
		U16(a2, 2) = (uint16_t)(U16(a2, 2) + MEM<uint16_t>(game_addr(game::unk_157B8C4) + (uint32_t)((int32_t)k * 2)));
		return a2;
	}

	uint32_t __cdecl ud_7AC0B0(uint32_t a1)
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

	uint32_t __cdecl ud_7AC1E0(uint32_t a1, uint32_t a2, uint32_t a3)
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
		const uint32_t cursor = MEM<uint32_t>(game_addr(FA_CURSOR));
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
		MEM<uint32_t>(game_addr(FA_CURSOR)) = ud_7AC420(hdr, ot, 2, cursor);
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl ud_7AC420(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		static const Draw lists[8] = { ud_7AC550, ud_7AC770, ud_7ACA00, ud_7ACEF0, a_73D770, a_73D9C0, ud_7ADA10, ud_7ADF70 };
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

	uint32_t __cdecl ud_7AC550(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_7AC770(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_7ACA00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_7ACEF0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_7ADA10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_7ADF70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ud_7AE640(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		U16(d, 0x50) = (uint16_t)(U16(d, 0x50) + 1);
		ud_7AC060(node, node + 0x1C, (uint32_t)U16(d, 0x50));
		if (ud_7AC0B0(node) == 0)
		{
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7AE6F0(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x9C) = 0x26;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return a1;
	}

	uint32_t __cdecl ud_7AE710(uint32_t a1)
	{
		ud_7AE740(a1);
		U16(a1, 0x9C) = (uint16_t)(U16(a1, 0x9C) - 1);
		if (S16(a1, 0x9C) <= 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7AE740(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t slot = U8(node, 0x2D);
		x::GetEffectSpawnPosition(game_addr(game::BattleEntitySlotData) + slot * 0x9C, 0xF1, 0, node + 0x1C);
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_080_TaskQueue), game_addr(game::MAG_080_Flake_Task), 0xA0, node);
		const uint32_t w1 = U32(node, 0x20);
		const uint32_t w0 = U32(node, 0x1C);
		U32(t, 0x5C) = w0;
		U32(t, 0x60) = w1;
		U16(t, 0x5C) = (uint16_t)(U16(t, 0x5C) + (uint16_t)((x::CrtRand() & 0x7FF) - 0x400));
		U16(t, 0x5E) = (uint16_t)(U16(t, 0x5E) + (uint16_t)((x::CrtRand() & 0x7FF) - 0x400));
		U16(t, 0x60) = (uint16_t)(U16(t, 0x60) + (uint16_t)((x::CrtRand() & 0x1FF) - 0x100));
		return 0;
	}

	uint32_t __cdecl ud_7AE7D0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_080_Flake_State0_InitSprite);
		states[1] = game_addr(game::MAG_080_Flake_State1_Animate);
		states[2] = game_addr(game::MAG_080_Flake_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		U32(node, 0x1C) = U32(node, 0x5C);
		U32(node, 0x20) = U32(node, 0x60);
		ud_7AE840(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_7AE840(uint32_t a1)
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
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::MAG_080_SpriteCursor));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(blk, 0x24) = 0;
		U16(blk, 0xB4) = colour;
		MEM<uint32_t>(game_addr(game::MAG_080_SpriteCursor)) = a_7435E0(blk, ot, 2, cursor);
		x::Field_Free(0xD8);
		return 0;
	}

	uint32_t __cdecl ud_7AEF10(uint32_t a1)
	{
		uint32_t node = a1;
		uint8_t st = U8(node, 0x29);
		U32(node, 0x4c) = game_addr(game::unk_157B4E8);
		st++;
		U16(node, 0x56) = 0xff80;
		U16(node, 0x52) = 0xB;
		U8(node, 0x29) = st;
		return 0;
	}

	uint32_t __cdecl ud_7AEF30(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl ud_7AEFA0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[2];
		states[0] = game_addr(game::MAG_080_ParticleEmitter_State0_SpawnOrbitParticle);
		states[1] = game_addr(game::MAG_080_ParticleEmitter_State1_Empty);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_7AEFF0(uint32_t a1)
	{
		const uint16_t t = U16(a1, 0x24);
		if ((int16_t)t >= 0x78)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		}
		if ((t & 1) == 0)
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_080_TaskQueue), game_addr(game::MAG_080_OrbitParticle_Task), 0xA0, a1);
		return 0;
	}

	uint32_t __cdecl ud_7AF030(uint32_t a1)
	{
		alignas(4) uint8_t loc[0x34] = {};
		const uint32_t L = P(loc);
		const uint32_t V = L;
		const uint32_t O = L + 8;
		const uint32_t M = L + 0x14;
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_080_OrbitParticle_Init);
		states[1] = game_addr(game::MAG_080_OrbitParticle_State1_Animate);
		states[2] = game_addr(game::MAG_080_OrbitParticle_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x42) = (uint16_t)(U16(node, 0x42) + 0x40);
		x::MAG_022_sub_8DD770(M);
		x::sub_8DD960(M, (uint32_t)(int32_t)S16(node, 0x44));
		x::sub_8DD7E0(M, (uint32_t)(int32_t)S16(node, 0x40));
		x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)S16(node, 0x42));
		U16(V, 4) = U16(node, 0x9E);
		U16(V, 0) = 0;
		U16(V, 2) = 0;
		x::matrixMultiplyVector(M, V, O);
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		U16(node, 0x1C) = (uint16_t)(U16(d, 0x10) + U16(O, 0));
		U16(node, 0x1E) = (uint16_t)(U16(d, 0x12) + U16(O, 2));
		U16(node, 0x20) = (uint16_t)(U16(d, 0x14) + U16(O, 4));
		ud_7AE840(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_7AF130(uint32_t a1)
	{
		const uint32_t node = a1;
		U32(node, 0x4C) = game_addr(game::unk_157B2EC);
		U16(node, 0x52) = 0x12;
		U16(node, 0x40) = (uint16_t)(x::CrtRand() & 0xFFF);
		U16(node, 0x42) = (uint16_t)(x::CrtRand() & 0xFFF);
		U16(node, 0x44) = (uint16_t)(x::CrtRand() & 0xFFF);
		U16(node, 0x9E) = (uint16_t)((x::CrtRand() & 0xFF) + 0x180);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7AF190(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl ud_7AF270(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0xA0);
		if (S16(a1, 0x1C) >= 0xA00)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0xA00;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int i = 4; i != 0; i--, p += 0x2C)
			MEM<uint16_t>(p) = level;
		return 0;
	}

	uint32_t __cdecl ud_7AF320(uint32_t a1)
	{
		if (S16(a1, 0x24) < 0x96)
			return a1;
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x1C) = 0xA00;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl ud_7AF340(uint32_t a1)
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

	uint32_t __cdecl ud_7AF390(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[10];
		states[0] = game_addr(game::MAG_080_Junction_State0_Setup);
		states[1] = game_addr(game::MAG_080_Junction_State1_SpawnModelAtCue3);
		const uint32_t t = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2D) * 0x9C;
		states[2] = game_addr(game::MAG_080_Junction_State2_Wait);
		states[3] = game_addr(game::MAG_080_Junction_State3_StartCopy);
		states[4] = game_addr(game::MAG_080_Junction_State4_Wait);
		states[5] = game_addr(game::MAG_080_Junction_State5_Grow);
		states[6] = game_addr(game::MAG_080_Junction_State6_ShakeMorph);
		states[7] = game_addr(game::MAG_080_Junction_State7_ShowTarget);
		states[8] = game_addr(game::MAG_080_Junction_State8_Finish);
		states[9] = game_addr(game::MAG_080_Junction_State9_Empty);
		callp(states[S8(node, 0x29)], node);
		ud_7AF4C0();
		if ((U8(node, 0x26) & 8) != 0)
		{
			const uint16_t x0 = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_25D49C0)) - MEM<uint16_t>(game_addr(game::unk_25D49B8)) + MEM<uint16_t>(game_addr(game::unk_25D49FC)));
			U16(t, 0x1C) = x0;
			S32(t, 0x54) = (int16_t)x0;
			const uint16_t y0 = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_25D49C2)) - MEM<uint16_t>(game_addr(game::unk_25D49BA)) + MEM<uint16_t>(game_addr(game::unk_25D49FE)));
			U16(t, 0x1E) = y0;
			S32(t, 0x58) = (int16_t)y0;
			const uint16_t z0 = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_25D49C4)) - MEM<uint16_t>(game_addr(game::unk_25D49BC)) + MEM<uint16_t>(game_addr(game::unk_25D4A00)));
			U16(t, 0x20) = z0;
			S32(t, 0x5C) = (int16_t)z0;
			ud_7AFEC0(game_addr(MODEL_BLOCK));
		}
		ud_7B06A0(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_7AF4C0(void)
	{
		ud_7AF4D0();
		return ud_7AF9F0();
	}

	uint32_t __cdecl ud_7AF4D0(void)
	{
		uint32_t c = MEM<uint32_t>(game_addr(BOLT_BASE)) + 0x3000;
		const uint32_t h = x::Field_Alloc(0x2B0);
		U32(h, 0) = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U32(h, 0x18) = 0x80F0F0;
		U32(h, 0x14) = 0x40F0;
		U32(h, 0x10) = 0x80F0F0;
		U32(h, 0xC) = 0;
		S32(h, 0x2C) = MEM<int16_t>(game_addr(game::unk_25DF322));
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		for (int n = 0x40; n != 0; n--, c += 0x30)
		{
			if (U32(c, 0) == 0 || S16(c, 0x2C) < 2)
				continue;
			const int16_t fade = S16(c, 0x2E);
			if (fade < 0x1000)
			{
				x::set_dword_1CA8A30((uint32_t)(int32_t)fade);
				uint32_t o = h + 0x28;
				for (int k = 4; k != 0; k--, o -= 4)
				{
					gx::sub_45E0E0(o - 0x10);
					gx::sub_45E9D0();
					gx::sub_45E410(o);
				}
			}
			else
			{
				uint32_t o = h + 0x28;
				for (int k = 4; k != 0; k--, o -= 4)
					U32(o, 0) = U32(o, -0x10);
			}
			ud_7AF5E0(h, U32(c, 0), (uint32_t)(int32_t)S16(c, 0x2C));
		}
		x::Field_Free(0x2B0);
		return 0;
	}

	uint32_t __cdecl ud_7AF5E0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t h = a1;
		const int32_t count = (int32_t)a3;
		const int32_t step = 0x800 / count * 2;
		uint32_t seg = a2;
		uint32_t wave = game_addr(game::unk_157FA68);
		uint32_t o = h + 0x30;
		do
		{
			x::GTE_LoadV0(seg);
			x::GTE_RTPS();
			x::GTE_ReadSXY2(o);
			gx::GTE_ReadOTZ_SZ3div4(o + 4);
			const int32_t w = mul32(S16(seg, 6), MEM<int16_t>(wave));
			wave += (uint32_t)step;
			const int32_t g = S16(seg, 0x10);
			const int32_t f = g & 0xFFF;
			U16(o, 6) = (uint16_t)(w >> 12);
			const int32_t i = g >> 12;
			x::set_dword_1CA8A30((uint32_t)(0x1000 - f));
			gx::sub_45E0E0(h + (uint32_t)(i * 4) + 0x1C);
			gx::sub_45E9D0();
			x::set_dword_1CA8A30((uint32_t)f);
			gx::sub_45E0E0(h + (uint32_t)(i * 4) + 0x20);
			gx::sub_45EBF0();
			gx::sub_45E410(o + 0x10);
			seg = U32(seg, 0x14);
			o += 0x14;
		} while (seg != 0);
		const uint32_t dir = h + 4;
		uint32_t s = h + 0x30;
		ud_7AF860(s, s + 0x14, 0, dir);
		s += 0x14;
		for (int32_t k = count - 2; k != 0; k--, s += 0x14)
			ud_7AF860(s, s + 0x14, dir, dir);
		ud_7AF860(s, 0, dir, 0);
		uint32_t a = h + 0x30;
		for (int32_t k = count - 1; k != 0; k--, a += 0x14)
		{
			const uint32_t b = a + 0x14;
			const int16_t za = S16(a, 4);
			if (za <= 0)
				continue;
			const int16_t zb = S16(b, 4);
			if (zb <= 0)
				continue;
			uint32_t p = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = p + 0x34;
			U32(p, 8) = U32(a, 0);
			U32(p, 0x14) = U32(a, 8);
			U32(p, 0x20) = U32(b, 0);
			U32(p, 0x2C) = U32(b, 8);
			U32(p, 0x10) = U32(a, 0x10);
			U32(p, 4) = U32(a, 0x10);
			U32(p, 0xC) = 0x3E54F808;
			U32(p, 0x28) = U32(b, 0x10);
			U32(p, 0x1C) = U32(b, 0x10);
			int32_t d = (((int32_t)za + (int32_t)zb) >> 3) + S32(h, 0x2C);
			U32(p, 0x18) = 0xB9F838;
			U16(p, 0x24) = 0xF808;
			U16(p, 0x30) = 0xF838;
			U32(p, 0) = 0xC000000;
			U8(p, 7) = 0x3E;
			if (d < 0)
				d = 0;
			x::SSIGPU_InsertPrimDepthKeys(U32(h, 0) + (uint32_t)d * 4, p, 0, 0, 0, 0);
			p = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = p + 0x34;
			U32(p, 0x14) = U32(a, 8);
			U32(p, 0x2C) = U32(b, 8);
			U32(p, 8) = U32(a, 0);
			U32(p, 0x10) = U32(a, 0x10);
			U32(p, 4) = U32(a, 0x10);
			U32(p, 0x20) = U32(b, 0);
			U32(p, 0x28) = U32(b, 0x10);
			U32(p, 0x1C) = U32(b, 0x10);
			d = (((int32_t)S16(a, 4) + (int32_t)S16(b, 4)) >> 3) + S32(h, 0x2C);
			U32(p, 0xC) = 0x3E54F908;
			U32(p, 0x18) = 0xD9F938;
			U16(p, 0x24) = 0xF908;
			U16(p, 0x30) = 0xF938;
			U32(p, 0) = 0xC000000;
			U8(p, 7) = 0x3E;
			if (d < 0)
				d = 0;
			x::SSIGPU_InsertPrimDepthKeys(U32(h, 0) + (uint32_t)d * 4, p, 0, 0, 0, 0);
		}
		return 0;
	}

	uint32_t __cdecl ud_7AF860(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t s = a1, n = a2, prev = a3, dir = a4;
		if (S16(s, 4) < 0)
			return 0;
		int32_t ox = 0, oy = 0;
		if (n != 0)
		{
			if (S16(n, 4) < 0)
				return 0;
			const int32_t dx = ScreenPx(S16(n, 0)) - ScreenPx(S16(s, 0));
			const int32_t ndy = ScreenPx(S16(s, 2)) - ScreenPx(S16(n, 2));
			int32_t adx = dx < 0 ? -dx : dx;
			if (adx >= 0x80)
				adx = 0x7F;
			int32_t ady = ndy < 0 ? -ndy : ndy;
			if (ady >= 0x80)
				ady = 0x7F;
			const int32_t len = MEM<uint8_t>(game_addr(game::unk_157BA68) + (uint32_t)(shl32(ady, 7) + adx));
			int32_t px, py;
			if (prev != 0)
			{
				if (len == 0)
				{
					const int16_t p0 = S16(prev, 0);
					const int16_t p1 = S16(prev, 2);
					U16(dir, 0) = (uint16_t)p0;
					U16(dir, 2) = (uint16_t)p1;
					px = p0;
					py = p1;
				}
				else
				{
					const int32_t cx = shl32(ndy, 12) / len;
					const int32_t cy = shl32(dx, 12) / len;
					int32_t sx = cx + S16(prev, 0);
					int32_t sy = cy + S16(prev, 2);
					U16(dir, 0) = (uint16_t)cx;
					U16(dir, 2) = (uint16_t)cy;
					if (sx == 0 && sy == 0)
					{
						sx = cx * 2;
						sy = cy * 2;
					}
					const int32_t sq = (int32_t)((uint32_t)mul32(sx, sx) + (uint32_t)mul32(sy, sy));
					const int32_t k = 0x4000 - (sq >> 13);
					px = mul32(k, sx) >> 13;
					py = mul32(k, sy) >> 13;
				}
			}
			else
			{
				if (len == 0)
				{
					U16(s, 4) = 0xFFFF;
					return 0;
				}
				px = shl32(ndy, 12) / len;
				py = shl32(dx, 12) / len;
				U16(dir, 0) = (uint16_t)px;
				U16(dir, 2) = (uint16_t)py;
			}
			const int32_t w = (int32_t)S16(n, 6) / ((int32_t)S16(s, 4) + 0x80);
			ox = ScreenEighths(mul32(w, px) >> 12);
			oy = ScreenEighths(mul32(w, py) >> 12);
		}
		const uint16_t x0 = U16(s, 0);
		U16(s, 0) = (uint16_t)(x0 - (uint16_t)ox);
		U16(s, 8) = (uint16_t)(x0 + (uint16_t)ox);
		const uint16_t y0 = U16(s, 2);
		U16(s, 0xA) = (uint16_t)(y0 + (uint16_t)oy);
		U16(s, 2) = (uint16_t)(y0 - (uint16_t)oy);
		return 0;
	}

	uint32_t __cdecl ud_7AF9F0(void)
	{
		alignas(4) uint8_t loc[0x40] = {};
		const uint32_t AT = P(loc);
		const uint32_t R = AT + 0x10 - 8;
		uint32_t c = MEM<uint32_t>(game_addr(BOLT_BASE)) + 0x3000;
		for (int n = 0x40; n != 0; n--, c += 0x30)
		{
			uint32_t s0 = U32(c, 0);
			if (s0 == 0)
				continue;
			U16(c, 0x2E) = (uint16_t)(U16(c, 0x2E) + 0xFF00);
			if (S16(c, 0x2E) <= 0)
			{
				uint32_t s = s0;
				do
				{
					MEM<uint32_t>(game_addr(game::unk_25DF314)) = s0;
					U16(s, 6) = 0xFFFF;
					s = U32(s, 0x14);
				} while (s != 0);
				U32(c, 0) = 0;
				MEM<uint32_t>(game_addr(game::unk_25DF318)) = c;
				continue;
			}
			if (S16(c, 0x16) < 0x1000)
			{
				const uint16_t prog = (uint16_t)(U16(c, 0x16) + U16(c, 0x1E));
				U16(c, 8) = (uint16_t)(U16(c, 8) + U16(c, 0x18));
				U16(c, 0xA) = (uint16_t)(U16(c, 0xA) + U16(c, 0x1A));
				U16(c, 0x16) = prog;
				U16(c, 0xC) = (uint16_t)(U16(c, 0xC) + U16(c, 0x1C));
				for (uint32_t o = 0x18; o <= 0x1C; o += 2)
				{
					const int32_t r = (int32_t)ud_7AFEA0();
					U16(c, o) = (uint16_t)(U16(c, o) + (uint16_t)((r >> (MEM<uint8_t>(game_addr(game::unk_25DF31C)) & 31)) - (int32_t)MEM<uint16_t>(game_addr(game::unk_25DF31E))));
				}
				if ((int32_t)ud_7AFEA0() < (int32_t)MEM<int16_t>(game_addr(game::unk_25DF320)))
				{
					for (uint32_t o = 8; o <= 0xC; o += 2)
					{
						const int32_t r = (int32_t)ud_7AFEA0();
						const uint32_t sh = (uint32_t)(MEM<uint8_t>(game_addr(game::unk_25DF31C)) - 2) & 31;
						U16(c, o) = (uint16_t)(U16(c, o) + (uint16_t)((r >> sh) - (int32_t)(uint16_t)(MEM<uint16_t>(game_addr(game::unk_25DF31E)) << 2)));
					}
				}
				if (S16(c, 0x16) >= 0x1000)
				{
					U16(c, 0x16) = 0x1000;
					U32(AT, 0) = U32(c, 0x10);
					U32(AT, 4) = U32(c, 0x14);
				}
				else
				{
					x::set_dword_1CA8A30((uint32_t)(0x1000 - (int32_t)S16(c, 0x16)));
					gx::sub_45E0B0(c + 8);
					gx::sub_45E9D0();
					x::set_dword_1CA8A30((uint32_t)(int32_t)S16(c, 0x16));
					gx::sub_45E0B0(c + 0x10);
					gx::sub_45EBF0();
					x::GTE_StoreIR123(AT);
				}
				const uint32_t s = ud_7AFD80();
				if (s == 0)
				{
					U16(c, 0x16) = 0x1000;
					return 0;
				}
				U32(s, 0x14) = 0;
				U32(s, 0) = U32(AT, 0);
				U32(s, 4) = U32(AT, 4);
				U16(s, 6) = U16(c, 0xE);
				U32(s, 0x10) = U32(c, 0x28);
				U32(U32(c, 4), 0x14) = s;
				const int32_t sn = MEM<int16_t>(game_addr(game::unk_157FA68) + (uint32_t)((S16(c, 0x16) >> 1) * 2));
				U32(c, 4) = s;
				x::set_dword_1CA8A30((uint32_t)sn);
				gx::sub_45E0B0(c + 0x20);
				gx::sub_45E9D0();
				U16(c, 0x2C) = (uint16_t)(U16(c, 0x2C) + 1);
				x::GTE_StoreIR123(s + 8);
				if (S16(c, 0x16) < 0xC00 && (int32_t)ud_7AFEA0() < shl32(S16(c, 0x26), 13))
				{
					U32(R, 8) = U32(AT, 0);
					U32(R, 0xC) = U32(AT, 4);
					U16(R, 0xE) = U16(c, 0xE);
					U16(R, 0x10) = U16(c, 0x10);
					U16(R, 0x12) = U16(c, 0x12);
					U16(R, 0x14) = U16(c, 0x14);
					U16(R, 0x16) = U16(c, 0x16);
					U16(R, 0x18) = (uint16_t)((ud_7AFEA0() & 0x1FF) + U16(c, 0x18) - 0x100);
					U16(R, 0x1A) = (uint16_t)((ud_7AFEA0() & 0x1FF) + U16(c, 0x1A) - 0x100);
					const uint32_t r = ud_7AFEA0();
					U16(R, 0x24) = U16(c, 0x24);
					U32(R, 0x20) = U32(c, 0x20);
					U16(R, 0x1C) = (uint16_t)((r & 0x1FF) + U16(c, 0x1C) - 0x100);
					U16(R, 0x1E) = U16(c, 0x1E);
					U16(R, 0x26) = 0;
					U32(R, 0x28) = U32(c, 0x28);
					U16(R, 0x2E) = U16(c, 0x2E);
					const uint32_t b = ud_7AFDD0(R);
					if (b != 0)
					{
						U32(b, 8) = U32(s, 8);
						U32(b, 0xC) = U32(s, 0xC);
					}
					U16(c, 0x26) = (uint16_t)(U16(c, 0x26) - 1);
				}
			}
			for (uint32_t s = U32(c, 0); s != 0; s = U32(s, 0x14))
			{
				U16(s, 0x10) = (uint16_t)(U16(s, 0x10) + U16(s, 0x12));
				if (S16(s, 0x10) < 0)
					U16(s, 0x10) = 0;
				U16(s, 0) = (uint16_t)(U16(s, 0) + U16(s, 8));
				U16(s, 2) = (uint16_t)(U16(s, 2) + U16(s, 0xA));
				U16(s, 4) = (uint16_t)(U16(s, 4) + U16(s, 0xC));
			}
			uint32_t s = U32(c, 0);
			if (S16(s, 0x10) > 0x400)
				continue;
			for (;;)
			{
				const uint32_t nx = U32(s, 0x14);
				if (S16(nx, 0x10) > 0x400)
					break;
				U16(c, 0x2C) = (uint16_t)(U16(c, 0x2C) - 1);
				MEM<uint32_t>(game_addr(game::unk_25DF314)) = s;
				U16(s, 6) = 0xFFFF;
				U32(c, 0) = nx;
				if (S16(c, 0x2C) < 2)
				{
					U32(c, 0) = 0;
					MEM<uint32_t>(game_addr(game::unk_25DF318)) = c;
					break;
				}
				s = nx;
				if (S16(s, 0x10) > 0x400)
					break;
			}
		}
		return 0;
	}

	uint32_t __cdecl ud_7AFD80(void)
	{
		uint32_t p = MEM<uint32_t>(game_addr(game::unk_25DF314));
		const uint32_t base = MEM<uint32_t>(game_addr(BOLT_BASE));
		if (S16(p, 6) != -1)
		{
			p = base;
			if (S16(base, 6) != -1)
			{
				int32_t n = 0x200;
				for (;;)
				{
					p += 0x18;
					if (--n == 0)
						return 0;
					if (S16(p, 6) == -1)
						break;
				}
			}
		}
		MEM<uint32_t>(game_addr(game::unk_25DF314)) = p >= base + 0x2FE8 ? base : p + 0x18;
		return p;
	}

	uint32_t __cdecl ud_7AFDD0(uint32_t a1)
	{
		const uint32_t base = MEM<uint32_t>(game_addr(BOLT_BASE));
		uint32_t c = MEM<uint32_t>(game_addr(game::unk_25DF318));
		if (U32(c, 0) != 0)
		{
			c = base + 0x3000;
			if (U32(c, 0) != 0)
			{
				int32_t n = 0x40;
				for (;;)
				{
					c += 0x30;
					if (--n == 0)
						return 0;
					if (U32(c, 0) == 0)
						break;
				}
			}
		}
		MEM<uint32_t>(game_addr(game::unk_25DF318)) = c >= base + 0x3BD0 ? base + 0x3000 : c + 0x30;
		const uint32_t s = ud_7AFD80();
		if (s == 0)
			return 0;
		const uint32_t rec = a1;
		U32(s, 0x14) = 0;
		U32(s, 8) = 0;
		U32(s, 0) = U32(rec, 8);
		U32(c, 8) = U32(rec, 8);
		U32(s, 0xC) = 0;
		U32(c, 0x10) = U32(rec, 0x10);
		U32(c, 0x14) = U32(rec, 0x14);
		U32(c, 0x18) = U32(rec, 0x18);
		U32(s, 4) = U32(rec, 0xC);
		U32(c, 0x1C) = U32(rec, 0x1C);
		U32(c, 0x20) = U32(rec, 0x20);
		U32(c, 0xC) = U32(rec, 0xC);
		U16(c, 0x2C) = 1;
		U32(c, 4) = s;
		U32(c, 0) = s;
		U32(c, 0x28) = U32(rec, 0x28);
		U32(c, 0x24) = U32(rec, 0x24);
		U16(c, 0x2E) = U16(rec, 0x2E);
		U32(s, 0x10) = U32(rec, 0x28);
		MEM<uint8_t>(game_addr(game::unk_25DF310)) = 2;
		return s;
	}

	uint32_t __cdecl ud_7AFEA0(void)
	{
		const uint32_t v = (MEM<uint32_t>(game_addr(game::unk_25DF324)) * 125u + 0xE) & 0x7FFF;
		MEM<uint32_t>(game_addr(game::unk_25DF324)) = v;
		return v;
	}

	uint32_t __cdecl ud_7AFEC0(uint32_t a1)
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
		ud_7AFFB0(U32(ent, 0x64), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, blk);
		const uint32_t part2 = U32(ent, 0x78);
		if (part2 != 0)
			ud_7AFFB0(U32(part2, 4), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, blk);
		x::BattleModel_BuildBoneMatricesFromPose(pose);
		return 0;
	}

	uint32_t __cdecl ud_7AFFB0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		U16(w, 0xA6) = U16(blk, 0xC0);
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

	uint32_t __cdecl ud_7B06A0(uint32_t a1)
	{
		alignas(4) uint8_t loc[0x38] = {};
		const uint32_t L = P(loc);
		const uint32_t V1 = L, V2 = L + 8, V3 = L + 0x10, M = L + 0x18;
		const uint32_t node = a1;
		const uint32_t c = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2C) * 0x9C;
		const int16_t t = S16(node, 0x24);
		if (t < 0x50 || t > 0x96)
			return 0;
		const int16_t k = (int16_t)(t - 0x52);
		if (MEM<int16_t>(game_addr(game::unk_1580A70) + (uint32_t)((int32_t)k * 2)) == 0)
			return 0;
		x::MAG_022_sub_8DD770(M);
		x::MAG_022_sub_8DD8A0(M, x::CrtRand() & 0xFFF);
		x::sub_8DD7E0(M, x::CrtRand() & 0xFFF);
		U16(V1, 0) = 0;
		U16(V1, 2) = 0;
		U16(V1, 4) = 0x200;
		x::matrixMultiplyVector(M, V1, V1);
		U16(V1, 0) = (uint16_t)(U16(V1, 0) + U16(c, 0x1C));
		U16(V1, 2) = (uint16_t)(U16(V1, 2) + U16(c, 0x1E));
		U16(V1, 4) = (uint16_t)(U16(V1, 4) + U16(c, 0x20));
		x::MAG_022_sub_8DD770(M);
		x::MAG_022_sub_8DD8A0(M, x::CrtRand() & 0xFFF);
		x::sub_8DD7E0(M, x::CrtRand() & 0xFFF);
		U16(V2, 0) = 0;
		U16(V2, 2) = 0;
		U16(V2, 4) = 0x100;
		x::matrixMultiplyVector(M, V2, V2);
		const int16_t hv = (int16_t)ud_7B0840((uint32_t)U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x50));
		const int32_t q = hv / 512;
		const uint16_t step = (uint16_t)(U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x50) + (uint16_t)q);
		ud_7AC060(node, V3, (uint32_t)step);
		U16(V3, 0) = (uint16_t)(U16(V3, 0) + U16(V2, 0));
		U16(V3, 2) = (uint16_t)(U16(V3, 2) + U16(V2, 2));
		U16(V3, 4) = (uint16_t)(U16(V3, 4) + U16(V2, 4));
		if ((int16_t)q > 0)
			ud_7B0860(V1, V3, (uint32_t)q);
		return 0;
	}

	uint32_t __cdecl ud_7B0840(uint32_t a1)
	{
		int16_t k = (int16_t)a1;
		if (k >= 0x78)
			k = 0x77;
		return (uint32_t)(uint16_t)(-(int32_t)MEM<int16_t>(game_addr(game::unk_157B8C4) + (uint32_t)((int32_t)k * 2)));
	}

	uint32_t __cdecl ud_7B0860(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		alignas(4) uint8_t rec[0x30] = {};
		const uint32_t R = P(rec);
		int16_t k = (int16_t)a3;
		if (k <= 0)
			k = 1;
		const int32_t step = 0x1000 / (int32_t)k;
		for (int i = 2; i != 0; i--)
		{
			const uint32_t ang = x::CrtRand();
			const int32_t rad = (int32_t)(x::CrtRand() & 0x3FF) + 0x400;
			int32_t sx = mul32((int32_t)x::computeCosine(ang), rad) / 4096;
			int32_t sz = mul32((int32_t)x::computeSin(ang), rad) / 4096;
			U16(R, 0xA) = U16(a1, 2);
			U16(R, 8) = U16(a1, 0);
			U16(R, 0xC) = U16(a1, 4);
			U16(R, 0x10) = U16(a2, 0);
			U16(R, 0x12) = U16(a2, 2);
			U16(R, 0x14) = U16(a2, 4);
			const int32_t m = shl32((int32_t)x::CrtRand(), 8) >> 15;
			sx = mul32(sx, m) >> 12;
			sz = mul32(sz, m) >> 12;
			U16(R, 0x18) = (uint16_t)sx;
			U16(R, 0x1A) = 0;
			U16(R, 0x1C) = (uint16_t)sz;
			U16(R, 0x24) = 0;
			U16(R, 0x22) = 0;
			U16(R, 0x20) = 0;
			U16(R, 0xE) = 0xBB8;
			U16(R, 0x16) = 0;
			U16(R, 0x1E) = (uint16_t)step;
			U16(R, 0x26) = 2;
			U16(R, 0x28) = 0x3000;
			U16(R, 0x2A) = 0xFE00;
			U16(R, 0x20) = (uint16_t)((x::CrtRand() & 0xFF) - 0x80);
			U16(R, 0x22) = (uint16_t)((x::CrtRand() & 0xFF) - 0x80);
			U16(R, 0x24) = (uint16_t)((x::CrtRand() & 0xFF) - 0x80);
			U16(R, 0x2E) = 0x1000;
			ud_7AFDD0(R);
		}
		return 0;
	}

	uint32_t __cdecl ud_7B09E0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint8_t ti = U8(node, 0x2D);
		const uint32_t t = game_addr(game::BattleEntitySlotData) + (uint32_t)ti * 0x9C;
		const uint32_t c = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2C) * 0x9C;
		ud_7B4480(game_addr(MODEL_BLOCK), game_addr(game::unk_25D4AF8), game_addr(game::unk_25DE428), t, (uint32_t)ti);
		U8(t, 0) |= 4;
		U16(c, 0) &= 0xFFFB;
		U8(t, 1) |= 8;
		U8(c, 1) |= 8;
		ud_7B43F0();
		const uint32_t k = a_73C100(node, game_addr(game::MAG_080_ActorSystemTask), game_addr(game::unk_1749C64), 0, 0xF, 4);
		U32(k, 0x290) = U32(c, 0x1C);
		U32(k, 0x294) = U32(c, 0x20);
		x::BdPlaySE(game_addr(game::unk_157B2E4), 0, 0x80);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7B0B40(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			a_73F990(a1);
			ud_7B1650(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7B0D20(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_080_ActorStatePtr));
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

	uint32_t __cdecl ud_7B0FA0(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_080_ActorStatePtr));
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

	uint32_t __cdecl ud_7B1650(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::MAG_080_ActorStatePtr), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			ud_7B4330();
			ud_7B19E0();
			ud_7B18A0();
			ud_7B16E0();
			sys = U32(game_addr(game::MAG_080_ActorStatePtr), 0);
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
		U16(game_addr(game::unk_25DF2F0), 0) = (uint16_t)(U16(game_addr(game::unk_25DF2F0), 0) + c14);
		U16(game_addr(game::unk_25DF008), 0) = (uint16_t)(U16(game_addr(game::unk_25DF008), 0) + c16);
		return done;
	}

	uint32_t __cdecl ud_7B16E0(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_080_ActorStatePtr), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::MAG_080_ActorStatePtr), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					ud_7B1740(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl ud_7B1740(uint32_t a1, uint32_t a2)
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
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = ud_7AC420(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
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
				MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = ud_7AC420(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
			}
		}
		const uint32_t top = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 0x68;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = top;
		return top;
	}

	uint32_t __cdecl ud_7B18A0(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::MAG_080_ActorStatePtr)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::MAG_080_ActorStatePtr));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						ud_7B1960(act);
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
							ud_7B1960(act);
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

	uint32_t __cdecl ud_7B1960(uint32_t a1)
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

	uint32_t __cdecl ud_7B19E0(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_080_ActorStatePtr), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				ud_7B2CB0(act);
			else if (type == 1)
				ud_7B1A20(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl ud_7B1A20(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::MAG_080_ActorStatePtr));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (ud_7B2B60(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::MAG_080_ActorStatePtr));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7B2A50(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::MAG_080_ActorStatePtr), 0);
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

	uint32_t __cdecl ud_7B2B60(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				ud_7B2BE0(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			ud_7B2BE0(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			ud_7B2BE0(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl ud_7B2BE0(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return ud_7B2C10(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl ud_7B2C10(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_080_ParticlePoolPtr));
		int32_t idx = MEM<int16_t>(game_addr(game::MAG_080_ParticleCursor));
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
			MEM<uint16_t>(game_addr(game::MAG_080_ParticleCursor)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::MAG_080_ParticleCursor)) = 0;
		return slot;
	}

	uint32_t __cdecl ud_7B2CB0(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_080_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_080_Actor_InterpolatePosition), a1);
		ud_7B2D30(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl ud_7B2D30(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::MAG_080_ActorStatePtr), 0);
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
				ud_7B2DB0(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			ud_7B2DB0(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl ud_7B2DB0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::MAG_080_ScratchSP), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::MAG_080_ScratchSP), 0) = sp;
		uint32_t node = a_7387B0(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::MAG_080_ActorStatePtr), 0);
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
		ud_7B42A0(node, desc);
		U32(game_addr(game::MAG_080_ScratchSP), 0) = U32(game_addr(game::MAG_080_ScratchSP), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl ud_7B37B0(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::MAG_080_ActorStatePtr), 0);
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
				uint32_t g = U32(game_addr(game::MAG_080_ActorStatePtr), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::MAG_080_ActorStatePtr), 0);
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

	uint32_t __cdecl ud_7B42A0(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			ud_7B2C10(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			ud_7B2C10(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			ud_7B2C10(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			ud_7B2C10(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl ud_7B4330(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::MAG_080_ActorStatePtr));
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
					ud_7B2C10(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::MAG_080_ActorStatePtr));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				ud_7B2C10(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::MAG_080_ActorStatePtr));
			}
		}
		return 0;
	}

	uint32_t __cdecl ud_7B43C0(uint32_t a1)
	{
		if (ud_7B1650(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl ud_7B43F0(void)
	{
		return ud_7B4400(5, 0, (uint32_t)-0x20);
	}

	uint32_t __cdecl ud_7B4400(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		MEM<uint8_t>(game_addr(game::unk_25DF310)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25DF31C)) = (uint16_t)(0xF - a1);
		const int32_t one = (int32_t)(1u << (a1 & 31));
		MEM<uint16_t>(game_addr(game::unk_25DF320)) = (uint16_t)a2;
		MEM<uint16_t>(game_addr(game::unk_25DF322)) = (uint16_t)a3;
		const uint32_t base = MEM<uint32_t>(game_addr(BOLT_BASE));
		MEM<uint16_t>(game_addr(game::unk_25DF31E)) = (uint16_t)((one - (one >> 31)) >> 1);
		MEM<uint32_t>(game_addr(game::unk_25DF314)) = base;
		uint32_t p = base + 6;
		for (int n = 0x200; n != 0; n--, p += 0x18)
			MEM<uint16_t>(p) = 0xFFFF;
		uint32_t c = base + 0x3000;
		MEM<uint32_t>(game_addr(game::unk_25DF318)) = c;
		for (int n = 0x40; n != 0; n--, c += 0x30)
			U32(c, 0) = 0;
		return c;
	}

	uint32_t __cdecl ud_7B4480(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
	{
		x::MAG_007_sub_8DCC00(a1, 0xC4);
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

	uint32_t __cdecl ud_7B44F0(uint32_t a1)
	{
		if (a_73B7E0(3) == 0)
			return 0;
		ud_7ABF40(a1, game_addr(game::MAG_080_JunctionModel_Task), game_addr(game::unk_1757B2C), 0xD8, 1, 0);
		U16(a1, 0x9C) = 2;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7B4540(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_080_JunctionModel_State0_Init);
		states[1] = game_addr(game::MAG_080_JunctionModel_State1_Play);
		states[2] = game_addr(game::MAG_080_JunctionModel_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		const uint32_t w0 = U32(node, 0x1C);
		const uint32_t w1 = U32(node, 0x20);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		U32(d, 0x10) = w0;
		U32(d, 0x14) = w1;
		const uint8_t status = U8(node, 0x26);
		return task_end(node, status);
	}

	uint32_t __cdecl ud_7B45B0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t c = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2C) * 0x9C;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		const uint32_t w0 = U32(c, 0x1C);
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		const uint32_t w1 = U32(c, 0x20);
		U32(node, 0x1C) = w0;
		U32(node, 0x20) = w1;
		U16(node, 0x88) = 0xFC00;
		ud_7AC0B0(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7B4620(uint32_t a1)
	{
		const uint32_t r = ud_7AC0B0(a1);
		if (r != 0)
			return r;
		U8(a1, 0x26) |= 1;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7B4650(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x9C) = (uint16_t)(U16(node, 0x9C) - 1);
		if (S16(node, 0x9C) <= 0)
			t4_next_state(node);
		return 0;
	}

	uint32_t __cdecl ud_7B4670(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t t = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2D) * 0x9C;
		const uint32_t c = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2C) * 0x9C;
		U8(node, 0x26) |= 8;
		U8(t, 0) |= 4;
		ud_7B4750(node, game_addr(MODEL_BLOCK));
		MEM<uint16_t>(game_addr(game::unk_25D49EC)) = 0x200;
		MEM<uint16_t>(game_addr(game::unk_25D49EE)) = 0x200;
		MEM<uint16_t>(game_addr(game::unk_25D49EA)) = 0x200;
		x::MAG_022_sub_8DD770(game_addr(MODEL_BLOCK));
		const uint32_t w0 = U32(c, 0x1C);
		const uint32_t w1 = U32(c, 0x20);
		U8(c, 0) |= 4;
		MEM<uint32_t>(game_addr(game::unk_25D49C0)) = w0;
		MEM<uint32_t>(game_addr(game::unk_25D49C4)) = w1;
		MEM<uint16_t>(game_addr(game::unk_25D4A0A)) = 0x1000;
		MEM<uint16_t>(game_addr(game::unk_25D49E8)) = 0x1000;
		const uint32_t k = a_73C100(node, game_addr(game::MAG_080_ActorSystemTask), game_addr(game::unk_174F0BC), 0xC, 0xF, 4);
		U32(k, 0x290) = U32(c, 0x1C);
		U32(k, 0x294) = U32(c, 0x20);
		x::BdPlaySE(game_addr(game::unk_157B2E8), 0, 0x80);
		U16(node, 0x9C) = 0x1E;
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7B4750(uint32_t a1, uint32_t a2)
	{
		const uint32_t e = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2D) * 0x9C;
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

	uint32_t __cdecl ud_7B4800(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x9C) = (uint16_t)(U16(node, 0x9C) - 1);
		if (S16(node, 0x9C) <= 0)
			t4_next_state(node);
		return 0;
	}

	uint32_t __cdecl ud_7B4820(uint32_t a1)
	{
		ud_7B4750(a1, game_addr(MODEL_BLOCK));
		uint16_t v = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_25D49EC)) + 0x40);
		MEM<uint16_t>(game_addr(game::unk_25D49EC)) = v;
		if ((int16_t)v >= 0x500)
		{
			v = 0x500;
			MEM<uint16_t>(game_addr(game::unk_25D49EC)) = v;
			U16(a1, 0x9C) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		MEM<uint16_t>(game_addr(game::unk_25D49EE)) = v;
		MEM<uint16_t>(game_addr(game::unk_25D49EA)) = v;
		return 0;
	}

	uint32_t __cdecl ud_7B4880(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t c = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2C) * 0x9C;
		const uint32_t t = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2D) * 0x9C;
		U16(node, 0x9C) = (uint16_t)(U16(node, 0x9C) + 1);
		const int16_t k = S16(node, 0x9C);
		if (k <= 0x2D)
			MEM<uint16_t>(game_addr(game::unk_25D49E0)) = (uint16_t)((MEM<uint16_t>(game_addr(game::unk_25D49E0)) + 0x5B) & 0xFFF);
		else
			MEM<uint16_t>(game_addr(game::unk_25D49E0)) = 0;
		if (k == 0x2D)
		{
			gx::QueueChainTransformation(t, 0x1A);
			const uint32_t q = a_73C100(node, game_addr(game::MAG_080_ActorSystemTask), game_addr(game::unk_1750660), 0xE, 0xF, 4);
			U32(q, 0x290) = U32(c, 0x1C);
			U32(q, 0x294) = U32(c, 0x20);
		}
		MEM<uint16_t>(game_addr(game::unk_25D49FC)) = (uint16_t)((x::CrtRand() & 0xFF) - 0x80);
		MEM<uint16_t>(game_addr(game::unk_25D4A00)) = (uint16_t)((x::CrtRand() & 0xFF) - 0x80);
		MEM<uint16_t>(game_addr(game::unk_25D49FE)) = (uint16_t)((x::CrtRand() & 0xFF) - 0x80);
		ud_7B4750(node, game_addr(MODEL_BLOCK));
		uint16_t v = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_25D49EC)) + 0x40);
		MEM<uint16_t>(game_addr(game::unk_25D49EC)) = v;
		if ((int16_t)v >= 0x600)
		{
			v = 0x600;
			MEM<uint16_t>(game_addr(game::unk_25D49EC)) = v;
		}
		MEM<uint16_t>(game_addr(game::unk_25D49EE)) = v;
		MEM<uint16_t>(game_addr(game::unk_25D49EA)) = v;
		const uint16_t m = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_25D4A0A)) + 0xFFC0);
		MEM<uint16_t>(game_addr(game::unk_25D4A0A)) = m;
		if ((int16_t)m <= 0)
		{
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			MEM<uint16_t>(game_addr(game::unk_25D4A0A)) = 0;
			MEM<uint16_t>(game_addr(game::unk_25D49E8)) = 0;
			MEM<uint16_t>(game_addr(game::unk_25D49FC)) = 0;
			MEM<uint16_t>(game_addr(game::unk_25D49FE)) = 0;
			MEM<uint16_t>(game_addr(game::unk_25D4A00)) = 0;
			MEM<uint16_t>(game_addr(game::unk_25D49E0)) = 0;
			return 0;
		}
		MEM<uint16_t>(game_addr(game::unk_25D49E8)) = m;
		return 0;
	}

	uint32_t __cdecl ud_7B49F0(uint32_t a1)
	{
		const uint32_t t = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2D) * 0x9C;
		U8(a1, 0x26) &= 0xF7;
		U16(t, 0) &= 0xFFFB;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ud_7B4A50(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_157B2E0));
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
		{ game::MAG_080_Master_State0_Advance, (void *)a_73A0D0, "080 MAG_080_sub_7AB880" },
		{ game::MAG_080_Master_State3_SpawnEmitter, (void *)a_73A170, "080 MAG_080_sub_7AB970" },
		{ game::MAG_080_EmitterTask, (void *)a_73A1A0, "080 MAG_080_sub_7AB9A0" },
		{ game::MAG_080_Director_State1_WaitDirGate1, (void *)a_73B790, "080 sub_7ABC20" },
		{ game::MAG_080_DirSyncGate, (void *)a_73B640, "080 sub_7ABC40" },
		{ game::MAG_080_DirPhaseGE, (void *)a_73B7E0, "080 sub_7ABC90" },
		{ game::MAG_080_Director_State4_WaitDirGate2, (void *)a_73B8A0, "080 sub_7ABD10" },
		{ game::MAG_080_Director_State5_WaitDirPhase2, (void *)a_73B8C0, "080 sub_7ABD30" },
		{ game::MAG_080_Director_State7_WaitDirGate3, (void *)a_733B70, "080 sub_7ABD70" },
		{ game::MAG_080_Director_State11_Finish, (void *)a_7475A0, "080 sub_7ABE40" },
		{ game::MAG_080_Director_State12_Empty, (void *)a_73A6D0, "080 nullsub_1363" },
		{ game::MAG_080_St_Empty, (void *)a_73A6D0, "080 nullsub_1364" },
		{ game::MAG_080_EmitG3, (void *)a_73D770, "080 sub_7AD4E0" },
		{ game::MAG_080_EmitG4, (void *)a_73D9C0, "080 sub_7AD730" },
		{ game::MAG_080_RisingModel_State2_Empty, (void *)a_73A6D0, "080 nullsub_1365" },
		{ game::MAG_080_FlakeThrowerTask, (void *)a_73A380, "080 sub_7AE690" },
		{ game::InitEffectSequenceFromData_c9, (void *)a_7435E0, "080 InitEffectSequenceFromData_c9" },
		{ game::MAG_080_SpriteStrip_DrawQuads, (void *)a_743720, "080 sub_7AEAA0" },
		{ game::MAG_080_StepCounterLimit, (void *)a_743C20, "080 sub_7AEF50" },
		{ game::MAG_080_Flake_State2_Empty, (void *)a_73A6D0, "080 nullsub_1366" },
		{ game::MAG_080_FlakeThrower_State2_Empty, (void *)a_73A6D0, "080 nullsub_1367" },
		{ game::MAG_080_OrbitParticle_State2_Empty, (void *)a_73A6D0, "080 nullsub_1375" },
		{ game::MAG_080_ParticleEmitter_State1_Empty, (void *)a_73A6D0, "080 nullsub_1368" },
		{ game::MAG_080_StageWobbleInTask, (void *)a_73A380, "080 sub_7AF1D0" },
		{ game::MAG_080_StageWobbleIn_State0_ResetEntityRecords, (void *)a_7335D0, "080 sub_7AF230" },
		{ game::MAG_080_StageWobbleIn_State2_Empty, (void *)a_73A6D0, "080 nullsub_1369" },
		{ game::MAG_080_StageWobbleOutTask, (void *)a_73A380, "080 sub_7AF2C0" },
		{ game::MAG_080_StageWobbleOut_State2_Empty, (void *)a_73A6D0, "080 nullsub_1370" },
		{ game::MAG_080_SpawnChildTask, (void *)a_73C100, "080 sub_7B0A90" },
		{ game::MAG_080_ActorSystemTask, (void *)a_73A380, "080 sub_7B0AE0" },
		{ game::MAG_080_DirectorSetup, (void *)a_73F990, "080 sub_7B0B70" },
		{ game::MAG_080_Director_StartPositions, (void *)a_73FC20, "080 sub_7B0E00" },
		{ game::MAG_080_Director_Mode3Setup, (void *)a_73FE90, "080 sub_7B1070" },
		{ game::MAG_080_Director_TargetPositions, (void *)a_73FFB0, "080 sub_7B1190" },
		{ game::MAG_080_RelocateActorData, (void *)a_740210, "080 sub_7B13F0" },
		{ game::MAG_080_ActorMotion, (void *)a_740910, "080 sub_7B1AB0" },
		{ game::MAG_080_DampValue, (void *)a_740F70, "080 sub_7B2110" },
		{ game::MAG_080_DampVector, (void *)a_740FC0, "080 sub_7B2160" },
		{ game::MAG_080_Instance_AdvanceTexFrame, (void *)a_741050, "080 sub_7B21F0" },
		{ game::MAG_080_Instance_BuildTransform, (void *)a_741120, "080 sub_7B22C0" },
		{ game::MAG_080_Particle_LifeCountdown, (void *)a_741B60, "080 sub_7B2D00" },
		{ game::MAG_080_RandRange, (void *)a_742290, "080 au_re__rand_25" },
		{ game::MAG_080_RandRangeWide, (void *)a_7422C0, "080 sub_7B3460" },
		{ game::MAG_080_RandVec3, (void *)a_742300, "080 sub_7B34A0" },
		{ game::MAG_080_JitterAngles3, (void *)a_742350, "080 sub_7B34F0" },
		{ game::MAG_080_RandAngleJitter, (void *)a_7423A0, "080 au_re__rand_25_0" },
		{ game::MAG_080_ActSys_AllocParticleSlot, (void *)a_7387B0, "080 sub_7B3590" },
		{ game::MAG_080_GetWord8Minus1, (void *)a_7424B0, "080 sub_7B3650" },
		{ game::MAG_080_Particle_AttachToOwner, (void *)a_7424C0, "080 sub_7B3660" },
		{ game::MAG_080_ParticleList_Append, (void *)a_742CA0, "080 sub_7B3E40" },
		{ game::MAG_080_ParticleList_Unlink, (void *)a_742CD0, "080 sub_7B3E70" },
		{ game::MAG_080_Particle_Init, (void *)a_742D00, "080 sub_7B3EA0" },
		{ game::MAG_080_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "080 sub_7B4050" },
		{ game::MAG_080_Particle_PlaceAtPoint, (void *)a_742FF0, "080 sub_7B4190" },
		{ game::MAG_080_ActorSystem_State2_Empty, (void *)a_73A6D0, "080 nullsub_1371" },
		{ game::MAG_080_JunctionModel_State2_Empty, (void *)a_73A6D0, "080 nullsub_1372" },
		{ game::MAG_080_Junction_State8_Finish, (void *)a_7475A0, "080 sub_7B4A20" },
		{ game::MAG_080_Junction_State9_Empty, (void *)a_73A6D0, "080 nullsub_1373" },
		{ game::MAG_080_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "080 MAG_080_sub_7B4A90" },
		{ game::MAG_080_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "080 MAG_080_sub_7B4AB0" },
		{ game::MAG_080_Emitter_State4_Empty, (void *)a_73A6D0, "080 nullsub_1374" },
		{ game::MAG_080_Master_State4_NextTargetLoop, (void *)a_747500, "080 MAG_080_sub_7B4AE0" },
		{ game::MAG_080_Master_State5_Advance, (void *)a_73A0D0, "080 MAG_080_sub_7B4B20" },
		{ game::MAG_080_Master_State6_WaitQueuesEmpty, (void *)a_747550, "080 MAG_080_sub_7B4B30" },
		{ game::MAG_080_Master_State7_RequestBattleSync, (void *)a_747560, "080 MAG_080_sub_7B4B40" },
		{ game::MAG_080_Master_State8_WaitBattleSyncDone, (void *)a_747590, "080 MAG_080_sub_7B4B70" },
		{ game::MAG_080_Master_State9_Finish, (void *)a_7475A0, "080 MAG_080_sub_7B4B80" },
		{ game::MAG_080_Master_State10_Empty, (void *)a_73A6D0, "080 nullsub_1362" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_080_Master, (void *)ud_7AB6F0, "080 ud_7AB6F0" },
		{ game::MAG_080_Master_State1_PutCasterAboveTarget, (void *)ud_7AB890, "080 ud_7AB890" },
		{ game::MAG_080_Master_State2_CarveActorPools, (void *)ud_7AB8F0, "080 ud_7AB8F0" },
		{ game::MAG_080_Master_ClearActorPools, (void *)ud_7AB930, "080 ud_7AB930" },
		{ game::MAG_080_Emitter_State0_SetupDirectorData, (void *)ud_7ABA20, "080 ud_7ABA20" },
		{ game::MAG_080_Director_InitData, (void *)ud_7ABA50, "080 ud_7ABA50" },
		{ game::MAG_080_Director, (void *)ud_7ABAF0, "080 ud_7ABAF0" },
		{ game::MAG_080_Director_PhaseBookkeeping, (void *)ud_7ABBA0, "080 ud_7ABBA0" },
		{ game::MAG_080_Director_State0_ClaimVoiceSlot, (void *)ud_7ABBF0, "080 ud_7ABBF0" },
		{ game::MAG_080_Director_State2_HideMonstersAtCut1, (void *)ud_7ABC70, "080 ud_7ABC70" },
		{ game::MAG_080_HideMonsters, (void *)ud_7ABCB0, "080 ud_7ABCB0" },
		{ game::MAG_080_Director_State3_WaitCutFrame2D, (void *)ud_7ABCF0, "080 ud_7ABCF0" },
		{ game::MAG_080_Director_State6_WaitCutFrame76, (void *)ud_7ABD50, "080 ud_7ABD50" },
		{ game::MAG_080_Director_State8_WaitDirPhase3, (void *)ud_7ABD90, "080 ud_7ABD90" },
		{ game::MAG_080_Director_State9_DamageCueAtFrameA0, (void *)ud_7ABDB0, "080 ud_7ABDB0" },
		{ game::MAG_080_Director_State10_RestoreAndReleaseVoiceAtCut4, (void *)ud_7ABDD0, "080 ud_7ABDD0" },
		{ game::MAG_080_RestoreMonsters, (void *)ud_7ABE00, "080 ud_7ABE00" },
		{ game::MAG_080_Director_OnCut, (void *)ud_7ABE80, "080 ud_7ABE80" },
		{ game::MAG_080_SpawnPrimSeqTask, (void *)ud_7ABF40, "080 ud_7ABF40" },
		{ game::MAG_080_RisingModel_Task, (void *)ud_7ABF90, "080 ud_7ABF90" },
		{ game::MAG_080_RisingModel_State0, (void *)ud_7AC000, "080 ud_7AC000" },
		{ game::MAG_080_CasterPosPlusHeight, (void *)ud_7AC060, "080 ud_7AC060" },
		{ game::MAG_080_DrawPrimModel, (void *)ud_7AC0B0, "080 ud_7AC0B0" },
		{ game::MAG_080_PrimPlayer_ObjectCallback, (void *)ud_7AC1E0, "080 ud_7AC1E0" },
		{ game::MAG_080_RenderPrimModel, (void *)ud_7AC420, "080 ud_7AC420" },
		{ game::MAG_080_PrimList_FlatTri, (void *)ud_7AC550, "080 ud_7AC550" },
		{ game::MAG_080_PrimList_FlatQuad, (void *)ud_7AC770, "080 ud_7AC770" },
		{ game::MAG_080_PrimList_TexTri, (void *)ud_7ACA00, "080 ud_7ACA00" },
		{ game::MAG_080_PrimList_TexQuad, (void *)ud_7ACEF0, "080 ud_7ACEF0" },
		{ game::MAG_080_PrimList_GT3Scroll, (void *)ud_7ADA10, "080 ud_7ADA10" },
		{ game::MAG_080_PrimList_GT4Scroll, (void *)ud_7ADF70, "080 ud_7ADF70" },
		{ game::MAG_080_RisingModel_State1, (void *)ud_7AE640, "080 ud_7AE640" },
		{ game::MAG_080_FlakeThrower_State0_SetCounter26, (void *)ud_7AE6F0, "080 ud_7AE6F0" },
		{ game::MAG_080_FlakeThrower_State1_FlakePerTick, (void *)ud_7AE710, "080 ud_7AE710" },
		{ game::MAG_080_FlakeThrower_SpawnFlake, (void *)ud_7AE740, "080 ud_7AE740" },
		{ game::MAG_080_Flake_Task, (void *)ud_7AE7D0, "080 ud_7AE7D0" },
		{ game::MAG_080_DrawSpriteNode, (void *)ud_7AE840, "080 ud_7AE840" },
		{ game::MAG_080_Flake_State0_InitSprite, (void *)ud_7AEF10, "080 ud_7AEF10" },
		{ game::MAG_080_Flake_State1_Animate, (void *)ud_7AEF30, "080 ud_7AEF30" },
		{ game::MAG_080_ParticleEmitter_Task, (void *)ud_7AEFA0, "080 ud_7AEFA0" },
		{ game::MAG_080_ParticleEmitter_State0_SpawnOrbitParticle, (void *)ud_7AEFF0, "080 ud_7AEFF0" },
		{ game::MAG_080_OrbitParticle_Task, (void *)ud_7AF030, "080 ud_7AF030" },
		{ game::MAG_080_OrbitParticle_Init, (void *)ud_7AF130, "080 ud_7AF130" },
		{ game::MAG_080_OrbitParticle_State1_Animate, (void *)ud_7AF190, "080 ud_7AF190" },
		{ game::MAG_080_StageWobbleIn_State1_RampUpA00, (void *)ud_7AF270, "080 ud_7AF270" },
		{ game::MAG_080_StageWobbleOut_State0_WaitTick150, (void *)ud_7AF320, "080 ud_7AF320" },
		{ game::MAG_080_StageWobbleOut_State1_FadeOut, (void *)ud_7AF340, "080 ud_7AF340" },
		{ game::MAG_080_Junction_Task, (void *)ud_7AF390, "080 ud_7AF390" },
		{ game::MAG_080_Bolts_DrawAndAdvance, (void *)ud_7AF4C0, "080 ud_7AF4C0" },
		{ game::MAG_080_Bolts_Draw, (void *)ud_7AF4D0, "080 ud_7AF4D0" },
		{ game::MAG_080_Bolts_DrawChain, (void *)ud_7AF5E0, "080 ud_7AF5E0" },
		{ game::MAG_080_Bolts_SegmentEdges, (void *)ud_7AF860, "080 ud_7AF860" },
		{ game::MAG_080_Bolts_Advance, (void *)ud_7AF9F0, "080 ud_7AF9F0" },
		{ game::MAG_080_Bolts_AllocSegment, (void *)ud_7AFD80, "080 ud_7AFD80" },
		{ game::MAG_080_Bolts_NewChain, (void *)ud_7AFDD0, "080 ud_7AFDD0" },
		{ game::MAG_080_Lcg, (void *)ud_7AFEA0, "080 ud_7AFEA0" },
		{ game::MAG_080_ModelCopy_Draw, (void *)ud_7AFEC0, "080 ud_7AFEC0" },
		{ game::MAG_080_ModelCopy_MorphRender, (void *)ud_7AFFB0, "080 ud_7AFFB0" },
		{ game::MAG_080_Bolts_Emitter, (void *)ud_7B06A0, "080 ud_7B06A0" },
		{ game::MAG_080_NegHeight, (void *)ud_7B0840, "080 ud_7B0840" },
		{ game::MAG_080_Bolts_SpawnPair, (void *)ud_7B0860, "080 ud_7B0860" },
		{ game::MAG_080_Junction_State0_Setup, (void *)ud_7B09E0, "080 ud_7B09E0" },
		{ game::MAG_080_ActorSystem_State0_StartActorSystemAtFrame, (void *)ud_7B0B40, "080 ud_7B0B40" },
		{ game::MAG_080_ResetTrailHistory, (void *)ud_7B0D20, "080 ud_7B0D20" },
		{ game::MAG_080_FillTrailRows, (void *)ud_7B0FA0, "080 ud_7B0FA0" },
		{ game::MAG_080_ActSys_Step, (void *)ud_7B1650, "080 ud_7B1650" },
		{ game::MAG_080_ActSys_DrawPass, (void *)ud_7B16E0, "080 ud_7B16E0" },
		{ game::MAG_080_ActSys_DrawPrimActor, (void *)ud_7B1740, "080 ud_7B1740" },
		{ game::MAG_080_ActSys_DrawSpritePass, (void *)ud_7B18A0, "080 ud_7B18A0" },
		{ game::MAG_080_ActSys_DrawActorSprite, (void *)ud_7B1960, "080 ud_7B1960" },
		{ game::MAG_080_ActSys_UpdatePass, (void *)ud_7B19E0, "080 ud_7B19E0" },
		{ game::MAG_080_ActorStep, (void *)ud_7B1A20, "080 ud_7B1A20" },
		{ game::MAG_080_MorphVertexBlock, (void *)ud_7B2A50, "080 ud_7B2A50" },
		{ game::MAG_080_Instance_EndOfLifeTest, (void *)ud_7B2B60, "080 ud_7B2B60" },
		{ game::MAG_080_Instance_SpawnFollowUp, (void *)ud_7B2BE0, "080 ud_7B2BE0" },
		{ game::MAG_080_AllocParticle, (void *)ud_7B2C10, "080 ud_7B2C10" },
		{ game::MAG_080_ActSys_UpdateEmitter, (void *)ud_7B2CB0, "080 ud_7B2CB0" },
		{ game::MAG_080_ActSys_EmitterSpawnStep, (void *)ud_7B2D30, "080 ud_7B2D30" },
		{ game::MAG_080_ActSys_SpawnParticles, (void *)ud_7B2DB0, "080 ud_7B2DB0" },
		{ game::MAG_080_Actor_InterpolatePosition, (void *)ud_7B37B0, "080 ud_7B37B0" },
		{ game::MAG_080_SpawnSubEffects, (void *)ud_7B42A0, "080 ud_7B42A0" },
		{ game::MAG_080_Director_SpawnGroupEffects, (void *)ud_7B4330, "080 ud_7B4330" },
		{ game::MAG_080_ActorSystem_State1_StepActorSystem, (void *)ud_7B43C0, "080 ud_7B43C0" },
		{ game::MAG_080_ResetBoltsShift5, (void *)ud_7B43F0, "080 ud_7B43F0" },
		{ game::MAG_080_Bolts_Init, (void *)ud_7B4400, "080 ud_7B4400" },
		{ game::MAG_080_ModelCopy_Init, (void *)ud_7B4480, "080 ud_7B4480" },
		{ game::MAG_080_Junction_State1_SpawnModelAtCue3, (void *)ud_7B44F0, "080 ud_7B44F0" },
		{ game::MAG_080_JunctionModel_Task, (void *)ud_7B4540, "080 ud_7B4540" },
		{ game::MAG_080_JunctionModel_State0_Init, (void *)ud_7B45B0, "080 ud_7B45B0" },
		{ game::MAG_080_JunctionModel_State1_Play, (void *)ud_7B4620, "080 ud_7B4620" },
		{ game::MAG_080_Junction_State2_Wait, (void *)ud_7B4650, "080 ud_7B4650" },
		{ game::MAG_080_Junction_State3_StartCopy, (void *)ud_7B4670, "080 ud_7B4670" },
		{ game::MAG_080_ModelCopy_BonePivot, (void *)ud_7B4750, "080 ud_7B4750" },
		{ game::MAG_080_Junction_State4_Wait, (void *)ud_7B4800, "080 ud_7B4800" },
		{ game::MAG_080_Junction_State5_Grow, (void *)ud_7B4820, "080 ud_7B4820" },
		{ game::MAG_080_Junction_State6_ShakeMorph, (void *)ud_7B4880, "080 ud_7B4880" },
		{ game::MAG_080_Junction_State7_ShowTarget, (void *)ud_7B49F0, "080 ud_7B49F0" },
		{ game::MAG_080_Emitter_State1_ApplyActionResults, (void *)ud_7B4A50, "080 ud_7B4A50" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag080_ultimecia_junction()
	{
		act::register_module(80);
		for (const act::ujunct::ModPort *p = act::ujunct::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(80, p->addr, p->port, p->name);
		for (const act::ujunct::ModPort *p = act::ujunct::PORTS; p->addr; p++)
			act::register_module_port(80, p->addr, p->port, p->name);
	}
}

