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

// Effect 85: The End: a copy of

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
namespace theend
{
	static const Mod MOD_085 = { "theend", 85, game::MAG_085_THE_END_FL, game::MAG_084_ANGEL_WING_FL,
		{ {}, {}, game::unk_156C7F0, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG085_THE_END_timBuffer, {}, {}, {}, game::unk_25BC4A0, {}, {}, game::unk_25BC894, {}, {}, {}, {}, game::unk_25C1748, {}, game::unk_25C19E8, {}, {}, game::MAG085_THE_END_activeSubObject, game::unk_25C1C58, {}, {}, {}, game::unk_25C1D28, {}, {}, {}, game::unk_25C21D4, game::unk_25C21DC },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_085_THE_END_EmitterTask, game::MAG_085_THE_END_Master_InitDirector, {}, game::MAG_085_THE_END_RequeueTIMUpload, game::MAG_085_THE_END_LoadCb_SummonStream, game::MAG_085_THE_END_LoadCb_ClearBusy_784CC0, game::MAG_085_THE_END_LoadCb_ClearBusy_784CD0, game::MAG_085_THE_END_LoadCb_ClearBusy_784CE0, game::MAG_085_THE_END_LoadCb_ClearBusy_784CF0, game::MAG_085_THE_END_LoadScript_RecordFile, {}, {}, game::MAG_085_THE_END_LoadScript_TryArmStep, {}, {}, {}, {}, {}, {}, {}, game::MAG_085_THE_END_DirSyncGate, {}, game::MAG_085_THE_END_DirPhaseGE, game::MAG_085_THE_END_PrimPlay_ObjectCallback, {}, {}, {}, {}, {}, game::MAG_085_THE_END_ResetTrailHistory, game::MAG_085_THE_END_Director_StartPositions, game::MAG_085_THE_END_FillTrailRows, game::MAG_085_THE_END_Director_Mode3Setup, game::MAG_085_THE_END_Director_TargetPositions, game::MAG_085_THE_END_RelocateActorData, {}, {}, game::MAG_085_THE_END_DampValue, game::MAG_085_THE_END_DampVector, {}, {}, game::MAG_085_THE_END_MorphVertexBlock, {}, {}, {}, game::MAG_085_RandRange, game::MAG_085_THE_END_RandRangeWide, game::MAG_085_RandAngleJitter, game::MAG_085_THE_END_Particle_AttachToOwner, game::MAG_085_THE_END_Actor_InterpolatePosition, {}, game::MAG_085_THE_END_ParticleList_Unlink, game::MAG_085_THE_END_Particle_PlaceAtAnchorSet, game::MAG_085_THE_END_Particle_PlaceAtPoint, {}, {}, game::MAG_085_THE_END_Emitter_State1_ApplyActionResults, game::MAG_085_THE_END_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_085_THE_END_Emitter_State3_FinishReleaseRoot, game::MAG_085_THE_END_Nullsub_78D6B0, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id DIR_PTR = game::unk_156C7F0;
	static constexpr game::Id PHASE_DATA = DIR_PTR;
	static constexpr game::Id SPRITE_CURSOR = game::unk_25BC394;
	static constexpr game::Id PACKET_CURSOR = SPRITE_CURSOR;
	static constexpr game::Id FA_CURSOR = game::BATTLE_PACKET_WRITE_CURSOR;
	static constexpr game::Id SCRATCH_SP = game::unk_25C21CC;
	static constexpr game::Id ACTOR_STATE = game::unk_25C21DC;
	static constexpr game::Id LOADER_PTR = game::unk_25C1C58;
	static constexpr game::Id SCRIPT_CUR = game::MAG085_THE_END_activeSubObject;

	namespace gx
	{
		static inline uint32_t sub_4A8480(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::BattleMenu_SetVisible))(a1); }
		static inline uint32_t MAG_063_sub_7015B0(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(a1, a2); }
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

	static inline uint32_t task_end(uint32_t node) { const uint8_t st = U8(node, 0x26); U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1); return task_end(node, st); }

	static inline uint32_t te_texwin(uint32_t ctx, int32_t o)
	{
		uint32_t c = ((uint32_t)U8(ctx, o + 2) & 0xF8) | 0xFFFE2000u;
		c = (c << 5) | ((uint32_t)U8(ctx, o) & 0xF8);
		c = (c << 5) | (~((uint32_t)U16(ctx, o + 6) - 1u) & 0xF8);
		c = (c << 2) | ((~((uint32_t)U16(ctx, o + 4) - 1u) >> 3) & 0x1F);
		return c;
	}
	static inline bool te_offx(int16_t v) { return v < 0 || v > 0xA00; }
	static inline bool te_offy(int16_t v) { return v < 0 || v > 0x6C0; }
	static inline void te_scroll(uint32_t pk, int32_t o, uint32_t d, uint8_t w)
	{
		const uint32_t a = U8(pk, 0xC + o) + d, b = U8(pk, 0x18 + o) + d, c = U8(pk, 0x24 + o) + d, e = U8(pk, 0x30 + o) + d;
		if ((int32_t)(e | c | b | a) > 0xFF)
		{
			U8(pk, 0xC + o) = (uint8_t)(a - w);
			U8(pk, 0x18 + o) = (uint8_t)(b - w);
			U8(pk, 0x24 + o) = (uint8_t)(c - w);
			U8(pk, 0x30 + o) = (uint8_t)(e - w);
		}
		else
		{
			U8(pk, 0xC + o) = (uint8_t)a;
			U8(pk, 0x18 + o) = (uint8_t)b;
			U8(pk, 0x24 + o) = (uint8_t)c;
			U8(pk, 0x30 + o) = (uint8_t)e;
		}
	}

	uint32_t __cdecl te_784630(uint32_t a1);
	uint32_t __cdecl te_7847E0(uint32_t a1);
	uint32_t __cdecl te_784820();
	uint32_t __cdecl te_784910(uint32_t a1);
	uint32_t __cdecl te_784960();
	uint32_t __cdecl te_784DC0(uint32_t a1);
	uint32_t __cdecl te_784E30(uint32_t a1);
	uint32_t __cdecl te_784F30(uint32_t a1);
	uint32_t __cdecl te_784F80(uint32_t a1);
	uint32_t __cdecl te_784FD0(uint32_t a1);
	uint32_t __cdecl te_785080(uint32_t a1);
	uint32_t __cdecl te_785110(uint32_t a1);
	uint32_t __cdecl te_785170(uint32_t a1);
	uint32_t __cdecl te_785200(uint32_t a1);
	uint32_t __cdecl te_785230(uint32_t a1);
	uint32_t __cdecl te_785250(uint32_t a1);
	uint32_t __cdecl te_785290(uint32_t a1);
	uint32_t __cdecl te_7852D0(uint32_t a1);
	uint32_t __cdecl te_7852F0(uint32_t a1);
	uint32_t __cdecl te_785310(uint32_t a1);
	uint32_t __cdecl te_785330(uint32_t a1);
	uint32_t __cdecl te_785350(uint32_t a1);
	uint32_t __cdecl te_7853C0(uint32_t a1);
	uint32_t __cdecl te_785510(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl te_7855C0(uint32_t a1);
	uint32_t __cdecl te_785620();
	uint32_t __cdecl te_785790(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl te_785A90(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_785BC0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_785DE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_786070(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_7864F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_786FA0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_7874A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_787AF0(uint32_t a1);
	uint32_t __cdecl te_787B50(uint32_t a1);
	uint32_t __cdecl te_787BB0(uint32_t a1);
	uint32_t __cdecl te_787BE0(uint32_t a1);
	uint32_t __cdecl te_787C90(uint32_t a1);
	uint32_t __cdecl te_787E90(uint32_t a1);
	uint32_t __cdecl te_787ED0(uint32_t a1);
	uint32_t __cdecl te_787F30(uint32_t a1);
	uint32_t __cdecl te_787F80(uint32_t a1);
	uint32_t __cdecl te_788040(uint32_t a1);
	uint32_t __cdecl te_788060(uint32_t a1);
	uint32_t __cdecl te_7880A0(void);
	uint32_t __cdecl te_7880E0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_788220(uint32_t a1);
	uint32_t __cdecl te_788250(void);
	uint32_t __cdecl te_788290(uint32_t a1);
	uint32_t __cdecl te_788350(uint32_t a1);
	uint32_t __cdecl te_788370(uint32_t a1);
	uint32_t __cdecl te_7883B0(uint32_t a1);
	uint32_t __cdecl te_7883D0(uint32_t a1);
	uint32_t __cdecl te_788480(uint32_t a1);
	uint32_t __cdecl te_788490(uint32_t a1);
	uint32_t __cdecl te_788580(uint32_t a1);
	uint32_t __cdecl te_788700(uint32_t a1);
	uint32_t __cdecl te_788810(uint32_t a1);
	uint32_t __cdecl te_788960(uint32_t a1);
	uint32_t __cdecl te_788AF0(uint32_t a1);
	uint32_t __cdecl te_788B40(uint32_t a1);
	uint32_t __cdecl te_788BF0(uint32_t a1);
	uint32_t __cdecl te_788DD0(uint32_t a1);
	uint32_t __cdecl te_789050(void);
	uint32_t __cdecl te_789700(uint32_t a1);
	uint32_t __cdecl te_789790(void);
	uint32_t __cdecl te_7897F0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_789950(void);
	uint32_t __cdecl te_789A10(uint32_t a1);
	uint32_t __cdecl te_789A90(void);
	uint32_t __cdecl te_789AD0(uint32_t a1);
	uint32_t __cdecl te_78AB00(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_78AC10(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_78AC90(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_78ACC0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl te_78AD60(uint32_t a1);
	uint32_t __cdecl te_78ADE0(uint32_t a1);
	uint32_t __cdecl te_78AE60(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl te_78B640(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_78B860(uint32_t a1);
	uint32_t __cdecl te_78C350(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_78C3E0(void);
	uint32_t __cdecl te_78C470(uint32_t a1);
	uint32_t __cdecl te_78C4C0(uint32_t a1);
	uint32_t __cdecl te_78C530(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_78C660(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_78C6D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_78CE00(uint32_t a1);
	uint32_t __cdecl te_78CE30(uint32_t a1);
	uint32_t __cdecl te_78CE80(uint32_t a1);
	uint32_t __cdecl te_78CED0(uint32_t a1);
	uint32_t __cdecl te_78CFA0(uint32_t a1);
	uint32_t __cdecl te_78D150(uint32_t a1);
	uint32_t __cdecl te_78D180(uint32_t a1);
	uint32_t __cdecl te_78D1C0(uint32_t a1);
	uint32_t __cdecl te_78D1F0(uint32_t a1);
	uint32_t __cdecl te_78D250(uint32_t a1);
	uint32_t __cdecl te_78D2B0(uint32_t a1);
	uint32_t __cdecl te_78D2E0(uint32_t a1);
	uint32_t __cdecl te_78D3A0(uint32_t a1);
	uint32_t __cdecl te_78D3D0(uint32_t a1);
	uint32_t __cdecl te_78D480(uint32_t a1);
	uint32_t __cdecl te_78D4A0(uint32_t a1);
	uint32_t __cdecl te_78D4D0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_78D590(uint32_t a1);
	uint32_t __cdecl te_78D5C0(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace theend
{
	uint32_t __cdecl te_784630(uint32_t a1)
	{
		g_mod = &MOD_085;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::unk_25C21CC)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_25BC4A0)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_085_THE_END_Sequence_State0_Advance_7847C0);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_085_THE_END_Sequence_State0_Advance_7847D0);
		states[2] = game_addr(game::MAG_085_THE_END_Master_State2_CarvePools);
		states[3] = game_addr(game::MAG_085_THE_END_Sequence_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_085_THE_END_Sequence_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_085_THE_END_Sequence_State0_Advance_78D700);
		states[6] = game_addr(game::MAG_085_THE_END_Sequence_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_085_THE_END_Sequence_State7_RequestBattleSync);
		states[8] = game_addr(game::MAG_085_THE_END_Sequence_State8_WaitBattleSyncDone);
		states[9] = game_addr(game::MAG_085_THE_END_Sequence_State9_Finish_78D760);
		states[10] = game_addr(game::MAG_085_THE_END_Nullsub_78D780);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_25C1B5C));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_25C0F54));
			MEM<uint32_t>(game_addr(game::unk_25BC394)) = v1;
			MEM<uint32_t>(game_addr(game::unk_25BC49C)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_25C1B58));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_25C0F50));
			MEM<uint32_t>(game_addr(game::unk_25BC394)) = v1;
			MEM<uint32_t>(game_addr(game::unk_25BC49C)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_25C21B0)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25C1CB4)) = 0;
		static constexpr game::Id queues[8] = { game::unk_25C19E8, game::unk_25BC478, game::unk_25C21B8, game::unk_25C1748, game::unk_25BC580, game::unk_25BC898, game::unk_25C0F40, game::unk_25C19D8 };
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

	uint32_t __cdecl te_7847E0(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::unk_25C21D8));
		MEM<uint32_t>(game_addr(game::unk_25C1CB8)) = p;
		p += 0x798;
		MEM<uint32_t>(game_addr(game::unk_25C21D0)) = p;
		p += 0x9D80;
		MEM<uint32_t>(game_addr(game::unk_25C21D8)) = p;
		te_784820();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_784820()
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::unk_25C1CB8)), 0x798);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::unk_25C21D0)), 0x9D80);
		MEM<uint16_t>(game_addr(game::unk_25BC890)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25C1CC4)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25C1C54)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25BC398)) = 0;
		return 0;
	}

	uint32_t __cdecl te_784910(uint32_t a1)
	{
		te_784DC0(a1);
		te_784960();
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25C21B8), game_addr(game::MAG_085_THE_END_FileLoadScriptTask_784A60), 0x30, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25BC478), game_addr(game::MAG_085_THE_END_DirectorTask), 0x48, a1);
		t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_784960()
	{
		MEM<uint32_t>(game_addr(game::MAG085_THE_END_activeSubObject)) = game_addr(game::unk_1571E70);
		MEM<uint32_t>(game_addr(game::unk_25C1C58)) = game_addr(game::unk_25C21E0);
		x::MAG_007_sub_8DCC00(game_addr(game::unk_25C21E0), 0x16C);
		const uint32_t b = MEM<uint32_t>(game_addr(game::unk_25C1C58));
		uint32_t c = MEM<uint32_t>(game_addr(game::unk_25C21D8));
		U32(b, 0x100) = c;
		c += 0xA000;
		MEM<uint32_t>(game_addr(game::unk_25C21D8)) = c;
		U32(b, 0x114) = game_addr(game::unk_19EB018);
		for (int32_t o = 0x104; o <= 0x160; o += 4)
			if (o != 0x114 && o != 0x12C) U32(b, o) = game_addr(game::unk_19CF018);
		U32(b, 0x12C) = game_addr(game::unk_19D2560);
		U16(b, 0x164) = 0;
		U16(b, 0x166) = 0;
		return b;
	}

	uint32_t __cdecl te_784DC0(uint32_t a1)
	{
		(void)a1;
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::unk_156C7F0)), 0x54);
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_156C7F0));
		U16(d, 8) = 0;
		U16(d, 0xA) = 0;
		U16(d, 0xC) = 0;
		U16(d, 0) = 0;
		U16(d, 2) = 0;
		U16(d, 4) = 0x1000;
		const int16_t dx = (int16_t)(U16(d, 8) - U16(d, 0));
		const int16_t dz = (int16_t)(U16(d, 0xC) - 0x1000);
		const uint32_t a = x::CartesianToGameAngle((uint32_t)(int32_t)dx, (uint32_t)(int32_t)dz);
		U16(MEM<uint32_t>(game_addr(game::unk_156C7F0)), 0x4A) = (uint16_t)((a - 0x800) & 0xFFF);
		return 0;
	}

	uint32_t __cdecl te_784E30(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[23];
		states[0] = game_addr(game::MAG_085_THE_END_Director_State0_WaitCut1_BattleUIOff);
		states[1] = game_addr(game::MAG_085_THE_END_Director_State1_PlaySound1);
		states[2] = game_addr(game::MAG_085_THE_END_Director_State2_ArmLoadScriptStep);
		states[3] = game_addr(game::MAG_085_THE_END_Director_State3_WaitLoadDone);
		states[4] = game_addr(game::MAG_085_THE_END_Director_State4_WaitFrame4B);
		states[5] = game_addr(game::MAG_085_THE_END_Director_State5_WaitDirGate2);
		states[6] = game_addr(game::MAG_085_THE_END_Director_State6_WaitDirPhase2);
		states[7] = game_addr(game::MAG_085_THE_END_Director_State7_ScreenTint);
		states[8] = game_addr(game::MAG_085_THE_END_Director_State8_Countdown);
		states[9] = game_addr(game::MAG_085_THE_END_Director_State9_WaitDirGate3);
		states[10] = game_addr(game::MAG_085_THE_END_Director_State10_WaitCut3_PlaySound2);
		states[11] = game_addr(game::MAG_085_THE_END_Director_State11_WaitFrame3C);
		states[12] = game_addr(game::MAG_085_THE_END_Director_State12_WaitDirGate4);
		states[13] = game_addr(game::MAG_085_THE_END_Director_State13_WaitDirPhase4);
		states[14] = game_addr(game::MAG_085_THE_END_Director_State14_WaitFrame2D_A);
		states[15] = game_addr(game::MAG_085_THE_END_Director_State15_WaitDirGate5);
		states[16] = game_addr(game::MAG_085_THE_END_Director_State16_WaitDirPhase5);
		states[17] = game_addr(game::MAG_085_THE_END_Director_State17_WaitFrame2D_B);
		states[18] = game_addr(game::MAG_085_THE_END_Director_State18_WaitDirGate6);
		states[19] = game_addr(game::MAG_085_THE_END_Director_State19_WaitDirPhase6);
		states[20] = game_addr(game::MAG_085_THE_END_Director_State20_BattleUIOn);
		states[21] = game_addr(game::MAG_085_THE_END_Director_State21_Finish);
		states[22] = game_addr(game::MAG_085_THE_END_Director_State22_Nullsub);
		te_784F30(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl te_784F30(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			te_7853C0(a1);
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

	uint32_t __cdecl te_784F80(uint32_t a1)
	{
		gx::sub_4A8480(0);
		if (a_73B640(1) != 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_784FD0(uint32_t a1)
	{
		uint32_t r = a_73B7E0(1);
		if (r == 0) return 0;
		x::BdPlaySE(game_addr(game::unk_156C7F4), 0, 0x80);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl te_785080(uint32_t a1)
	{
		if (U16(MEM<uint32_t>(game_addr(game::unk_25C1C58)), 0x166) == 0)
		{
			a_73BB60(0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		}
		if (S16(MEM<uint32_t>(game_addr(game::unk_156C7F0)), 0x46) >= 0x4A)
			a_73BB60(1);
		return 0;
	}

	uint32_t __cdecl rs_824AE0(uint32_t a1)
	{
		if (S16(t1_data(), 0x46) >= 0x37)
			next_state(a1);
		return 0;
	}
	uint32_t __cdecl te_785110(uint32_t a1)
	{
		if (S16(t1_data(), 0x46) >= 0x4B)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_785170(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(game::unk_156C7F0)), 0x46) < 0xE)
			return 0;
		U16(a1, 0x44) = 6;
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25C19D8), game_addr(game::Effect_ScreenTintTask), 0x40, a1);
		U32(t, 0x34) = game_addr(game::unk_1571FE8);
		U16(t, 0x38) = 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_785200(uint32_t a1)
	{
		if (a_73B7E0(3) != 0)
		{
			x::BdPlaySE(game_addr(game::unk_156C7F8), 0, 0x80);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl te_785230(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(game::unk_156C7F0)), 0x46) >= 0x3C)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_785250(uint32_t a1)
	{
		uint32_t r = a_73B640(4);
		if (r == 0) return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl te_785290(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(PHASE_DATA)), 0x46) >= 0x2D)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_7852D0(uint32_t a1)
	{
		uint32_t r = a_73B7E0(5);
		if (r == 0)
			return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl te_7852F0(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(PHASE_DATA)), 0x46) >= 0x2D)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_785310(uint32_t a1)
	{
		const uint32_t r = a_73B640(6);
		if (r == 0)
			return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl te_785330(uint32_t a1)
	{
		const uint32_t r = a_73B7E0(6);
		if (r == 0)
			return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl te_785350(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_156C7F0));
		if (S16(d, 0x46) < 0xA)
			return 0;
		U16(d, 0x48) = 1;
		gx::sub_4A8480(1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_7853C0(uint32_t a1)
	{
		const int32_t cut = S16(MEM<uint32_t>(game_addr(game::unk_156C7F0)), 0x40);
		switch (cut - 1)
		{
		case 0:
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25BC580), game_addr(game::MAG_085_THE_END_ModelTaskC_787E20), 0x60, a1);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25BC580), game_addr(game::MAG_085_THE_END_ModelTaskC_7882E0), 0x60, a1);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25BC580), game_addr(game::MAG_085_THE_END_ModelTaskC_78D410), 0x60, a1);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25BC580), game_addr(game::MAG_085_THE_END_ModelTaskC_787FD0), 0x60, a1);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25BC898), game_addr(game::MAG_085_THE_END_FileLoadScriptTask_788420), 0xE4, a1);
			break;
		case 1:
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25BC898), game_addr(game::MAG_085_THE_END_GroundTask), 0xE4, a1);
			break;
		case 2:
		{
			te_785510(a1, game_addr(game::MAG_085_THE_END_FileLoadScriptTask_785560), game_addr(game::unk_1718EE4), 0x20, 1, 0);
			const uint32_t t1 = te_785510(a1, game_addr(game::MAG_085_THE_END_OrbitPlayTask), game_addr(game::unk_171C42C), 0x50, 2, 0);
			U16(t1, 0x8A) = 0;
			const uint32_t t2 = te_785510(a1, game_addr(game::MAG_085_THE_END_OrbitPlayTask), game_addr(game::unk_171C42C), 0x50, 2, 0);
			U16(t2, 0x8A) = 1;
			break;
		}
		case 3:
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25BC898), game_addr(game::MAG_085_THE_END_PetalEmitterTask), 0xE4, a1);
			break;
		case 4:
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25BC580), game_addr(game::MAG_085_THE_END_ScreenGlowTask), 0x60, a1);
			break;
		default:
			break;
		}
		return 0;
	}

	uint32_t __cdecl te_785510(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25C0F40), a2, 0x2DC, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		S32(t, 0x78) = (int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl te_7855C0(uint32_t a1)
	{
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		U16(a1, 0x1C) = 0;
		U16(a1, 0x1E) = 0;
		U16(a1, 0x20) = 0;
		S32(a1, 0x58) = 0x1000;
		S32(a1, 0x54) = 0x1000;
		S32(a1, 0x50) = 0x1000;
		U16(a1, 0x88) = 0x400;
		a_73C280(a1);
		te_785620();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_785620()
	{
		uint32_t cur = MEM<uint32_t>(game_addr(game::unk_25BC394));
		for (uint32_t e = game_addr(game::unk_1D97494); e < game_addr(game::CAMERA_FLAG_RELATED); e += 0x9C)
		{
			if (!(U8(e, 0) & 2)) continue;
			cur = x::sub_5088A0(e, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, cur);
			MEM<uint32_t>(game_addr(game::unk_25BC394)) = cur;
		}
		return cur;
	}

	uint32_t __cdecl te_785790(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t r = a2;
		if (((int32_t)S16(r, 0x1C) | S32(r, 0x18)) == 0)
			return 0;
		if (S16(r, 0x24) >= 0x1000 && U32(r, 0x20) == 0)
			return 0;
		uint8_t sp[0x48];
		const uint32_t s = P(sp);
		const uint32_t h = x::Field_Alloc(0x68);
		const uint32_t blk = a3;
		const uint32_t data = U32(a1, 0);
		const uint32_t model = data + U32(data, (uint32_t)(int32_t)S16(r, 2) * 4 + 8);
		const int16_t f1 = S16(r, 0x2A), f0 = S16(r, 0x28);
		U32(h, 0) = model;
		auto frame = [&](int16_t f) { U32(h, 4) = f == 0 ? model + 0xC : model + (uint32_t)mul32(S32(model, 4), f) * 8 + 0xC; };
		if (f0 == f1) frame(f0);
		else
		{
			const int16_t t = S16(r, 0x26);
			if (t == 0) frame(f0);
			else if (t == 0x1000) frame(f1);
			else
			{
				x::MAG_017_sub_701390(model, (uint32_t)(int32_t)f0, (uint32_t)(int32_t)f1, (uint32_t)(int32_t)t, U32(blk, 0x48));
				U32(h, 4) = U32(blk, 0x48);
			}
		}
		const uint32_t R = s + 0x28;
		ff8fx::fill<(0x20)>(sp + 0x28, 0);
		uint32_t flags = U32(r, 4);
		if (flags & 0x400) x::MAG_117_sub_701430(r + 0x10, R);
		else if (flags & 0x40000) gx::MAG_063_sub_7015B0(r + 0x10, R);
		else x::MAG_017_sub_701310(r + 0x10, R);
		flags = U32(r, 4);
		U16(s, 0) = U16(r, 8);
		U16(s, 2) = U16(r, 0xA);
		U16(s, 4) = U16(r, 0xC);
		int32_t tx, ty, tz;
		if (flags & 0x200)
		{
			tx = S16(s, 0);
			ty = S16(s, 2);
			tz = S16(s, 4);
		}
		else
		{
			x::GTE_SetRotMatrix(blk);
			x::GTE_LoadV0(s);
			x::GTE_MVMVA_RotV0();
			x::GTE_ReadMAC123(R + 0x14);
			x::GTE_MatrixMultiply(blk, R);
			tx = S32(R, 0x14);
			ty = S32(R, 0x18);
			tz = S32(R, 0x1C);
		}
		S32(R, 0x14) = add32(tx, S32(blk, 0x14));
		S32(R, 0x18) = add32(ty, S32(blk, 0x18));
		S32(R, 0x1C) = add32(tz, S32(blk, 0x1C));
		if (!(U32(r, 0x18) == 0x10001000 && S16(r, 0x1C) == 0x1000))
		{
			if (U32(r, 4) & 0x100)
			{
				const uint32_t D = s + 8;
				U16(D, 0) = U16(r, 0x18);
				U16(D, 8) = U16(r, 0x1A);
				U16(D, 0xA) = 0;
				U16(D, 4) = 0;
				U16(D, 0xE) = 0;
				U16(D, 2) = 0;
				U16(D, 0xC) = 0;
				U16(D, 6) = 0;
				U16(D, 0x10) = U16(r, 0x1C);
				x::sub_56C220(R, D);
			}
			else
			{
				S32(s, 8) = S16(r, 0x18);
				S32(s, 0xC) = S16(r, 0x1A);
				S32(s, 0x10) = S16(r, 0x1C);
				x::scale3DMatrix(R, s + 8);
			}
		}
		x::GTE_SetRotMatrix_W(R);
		x::GTE_SetTransVector_W(R);
		const uint32_t fl = U32(r, 4);
		U32(h, 0x14) = (fl & 0x4000) ? 0x2000u : 0x2030u;
		if (fl & 0x2000) U32(h, 0x14) |= 0xC;
		S32(h, 0xC) = S16(r, 0x24);
		if (S32(h, 0xC) != 0)
		{
			U32(h, 0x14) |= 0xC0;
			U32(h, 8) = U32(r, 0x20);
		}
		if (U16(blk, 0x56) == 0 && S16(r, 2) == 2) S32(h, 0x10) = S16(blk, 0x58) - 0x100;
		else S32(h, 0x10) = S16(blk, 0x58);
		U16(h, 0x18) = 0;
		U16(h, 0x1A) = 0;
		U16(h, 0x1E) = 0;
		U16(h, 0x1C) = 0;
		U16(h, 0x26) = 0;
		U16(h, 0x24) = 0;
		U16(h, 0x22) = 0x100;
		U16(h, 0x20) = 0x100;
		U16(h, 0x2A) = 0x100;
		U16(h, 0x28) = 0x100;
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = te_785A90(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl te_785A90(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
			cur = te_785BC0(ctx, a2, a3, a4);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = te_785DE0(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = te_786070(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = te_7864F0(ctx, a2, a3, cur);
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
			cur = te_786FA0(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			return te_7874A0(ctx, a2, a3, cur);
		U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		return cur;
	}

	uint32_t __cdecl te_785BC0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_785DE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_786070(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_7864F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_786FA0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_7874A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_787AF0(uint32_t a1)
	{
		if (a_73B7E0(6) != 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		}
		te_785620();
		if (a_73C280(a1) == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl te_787B50(uint32_t a1)
	{
		const uint32_t tab[4] = { game_addr(game::MAG_085_THE_END_OrbitPlay_State0_Wait), game_addr(game::MAG_085_THE_END_OrbitPlay_State1_Setup), game_addr(game::MAG_085_THE_END_OrbitPlay_State2_Orbit), game_addr(game::MAG_085_THE_END_OrbitPlay_State3_Nullsub) };
		callp(tab[S8(a1, 0x29)], a1);
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24)++;
		return task_end(a1, status);
	}

	uint32_t __cdecl te_787BB0(uint32_t a1)
	{
		if (U16(a1, 0x8A) == 0)
		{
			if (S16(a1, 0x24) >= 8) U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		else if (S16(a1, 0x24) >= 0xE)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_787BE0(uint32_t a1)
	{
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		S32(a1, 0x58) = 0x1000;
		S32(a1, 0x54) = 0x1000;
		S32(a1, 0x50) = 0x1000;
		U16(a1, 0x2CC) = 0xF500;
		U16(a1, 0x2CE) = 0xF400;
		U16(a1, 0x2D0) = 0;
		if (U16(a1, 0x8A) == 0)
		{
			U16(a1, 0x2D6) = 0;
			U16(a1, 0x2D8) = 0xE0;
			U16(a1, 0x2DA) = 0xFB00;
		}
		else
		{
			U16(a1, 0x2D6) = 0x800;
			U16(a1, 0x2D8) = 0x120;
			U16(a1, 0x2DA) = 0xFA40;
		}
		U16(a1, 0x2D4) = 0;
		U16(a1, 0x2C6) = 0xC00;
		U16(a1, 0x2C4) = 0x280;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_787C90(uint32_t a1)
	{
		uint8_t sp[0x38];
		const uint32_t s = P(sp);
		U16(a1, 0x2D4) = (uint16_t)(U16(a1, 0x2D4) + 1);
		if (S16(a1, 0x2D4) >= 0x1A)
		{
			U16(a1, 0x2C4) = (uint16_t)(U16(a1, 0x2C4) - 0x40);
			if (S16(a1, 0x2C4) <= 0) U16(a1, 0x2C4) = 0;
		}
		const uint16_t r = U16(a1, 0x2DA);
		U16(s, 0) = 0;
		U16(s, 2) = 0;
		U16(s, 4) = r;
		x::MAG_022_sub_8DD770(s + 0x18);
		x::MAG_022_sub_8DD8A0(s + 0x18, (uint32_t)(int32_t)S16(a1, 0x2C6));
		x::sub_8DD7E0(s + 0x18, (uint32_t)(int32_t)S16(a1, 0x2C4));
		x::matrixMultiplyVector(s + 0x18, s, a1 + 0x68);
		U16(s, 8) = 0;
		U16(s, 0xA) = 0xFF40;
		U16(s, 0xC) = 0;
		U16(a1, 0x2CC) = (uint16_t)(U16(a1, 0x2CC) + (uint16_t)(S16(a1, 0x68) / 16));
		U16(a1, 0x2CE) = (uint16_t)(U16(a1, 0x2CE) + (uint16_t)(S16(a1, 0x6A) / 16));
		U16(a1, 0x2D0) = (uint16_t)(U16(a1, 0x2D0) + (uint16_t)(S16(a1, 0x6C) / 16));
		const uint16_t ang = (uint16_t)((U16(a1, 0x2D8) + U16(a1, 0x2D6)) & 0xFFF);
		U16(a1, 0x2D6) = ang;
		x::sub_8DD960(s + 0x18, (uint32_t)(int32_t)(int16_t)ang);
		x::matrixMultiplyVector(s + 0x18, s + 8, s + 0x10);
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x2CC) + U16(s, 0x10));
		U16(a1, 0x1E) = (uint16_t)(U16(s, 0x12) + U16(a1, 0x2CE));
		U16(a1, 0x20) = (uint16_t)(U16(a1, 0x2D0) + U16(s, 0x14));
		U16(a1, 0x62) = U16(a1, 0x2C6);
		if (a_73B7E0(6) == 0)
		{
			if (a_73C280(a1) != 0)
				return 0;
		}
		U8(a1, 0x26) |= 1;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_787E90(uint32_t a1)
	{
		if (S16(a1, 0x24) < 0)
			return 0;
		U16(a1, 0x1C) = 0;
		uint32_t e = game_addr(game::unk_1D989BA);
		for (int k = 0; k < 4; k++, e += 0x2C)
		{
			U16(e, -0x28) = 0;
			U8(e, 0) = 0;
			U8(e, -1) = 0;
			U8(e, -2) = 0;
		}
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_787ED0(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0x100);
		if (S16(a1, 0x1C) >= 0x1000)
		{
			U16(a1, 0x1C) = 0x1000;
			a_746A10();
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		const uint16_t v = U16(a1, 0x1C);
		for (uint32_t e = game_addr(game::unk_1D98992), k = 0; k < 4; k++, e += 0x2C)
			MEM<uint16_t>(e) = v;
		return 0;
	}

	uint32_t __cdecl te_787F30(uint32_t a1)
	{
		if (a_73B7E0(6) != 0)
		{
			a_746AE0();
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl te_787F80(uint32_t a1)
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

	uint32_t __cdecl te_788040(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0)
		{
			U16(a1, 0x1C) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl te_788060(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0x200);
		if (S16(a1, 0x1C) >= 0x1000)
		{
			U16(a1, 0x1C) = 0x1000;
			te_7880A0();
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		te_7880E0((uint32_t)U16(a1, 0x1C), 1);
		return 0;
	}

	uint32_t __cdecl te_7880A0(void)
	{
		uint32_t save = MEM<uint32_t>(game_addr(game::unk_156C7F0)) + 0x20;
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

	uint32_t __cdecl te_7880E0(uint32_t a1, uint32_t a2)
	{
		for (uint32_t ent = game_addr(game::BattleEntitySlotData); ent < game_addr(game::unk_1D97494); ent += 0x9C)
		{
			uint16_t flags = U16(ent, 0);
			if ((flags & 2) != 0)
			{
				flags = (uint16_t)(flags | 0x800);
				U16(ent, 0) = flags;
				a_733950(ent, a1, a2);
			}
		}
		return 0;
	}

	uint32_t __cdecl te_788220(uint32_t a1)
	{
		if (a_73B7E0(6) != 0)
		{
			te_788250();
			U16(a1, 0x1C) = 0x1000;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl te_788250(void)
	{
		uint32_t save = MEM<uint32_t>(game_addr(game::unk_156C7F0)) + 0x20;
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

	uint32_t __cdecl te_788290(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0xFF00);
		if (S16(a1, 0x1C) <= 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			U16(a1, 0x1C) = 0;
		}
		te_7880E0((uint32_t)U16(a1, 0x1C), 1);
		return 0;
	}

	uint32_t __cdecl te_788350(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0)
		{
			U16(a1, 0x1C) = 0x7F;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl te_788370(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) - 8);
		if (S16(a1, 0x1C) <= 0x30)
		{
			U16(a1, 0x1C) = 0x30;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		x::Music_SetVolumeImmediate(0, (uint32_t)(int32_t)S16(a1, 0x1C));
		return 0;
	}

	uint32_t __cdecl te_7883B0(uint32_t a1)
	{
		const uint32_t r = a_73B7E0(6);
		if (r == 0)
			return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl te_7883D0(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 8);
		if (S16(a1, 0x1C) >= 0x7F)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0x7F;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		x::Music_SetVolumeImmediate(0, (uint32_t)(int32_t)S16(a1, 0x1C));
		return 0;
	}

	uint32_t __cdecl te_788480(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0x10)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl te_788490(uint32_t a1)
	{
		const int32_t n = S16(a1, 0xDC);
		uint16_t kind;
		if (n == 0) kind = 0;
		else if (n == 8) kind = 1;
		else if (n == 0xE) kind = 2;
		else
		{
			if (a_73B7E0(3) != 0)
			{
				U8(a1, 0x26) |= 1;
				U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
				U16(a1, 0xDC) = (uint16_t)(U16(a1, 0xDC) + 1);
				return 0;
			}
			kind = 0x7F;
		}
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25BC898), game_addr(game::MAG_085_THE_END_MeteorTask), 0xE4, a1);
		U16(a1, 0xDC) = (uint16_t)(U16(a1, 0xDC) + 1);
		U16(t, 0xDE) = kind;
		return 0;
	}

	uint32_t __cdecl te_788580(uint32_t a1)
	{
		uint8_t sp[0x2C];
		const uint32_t s = P(sp);
		uint32_t states[3];
		states[0] = game_addr(game::MAG_085_THE_END_Meteor_State0_Setup);
		states[1] = game_addr(game::MAG_085_THE_END_Meteor_State1_Fall);
		states[2] = game_addr(game::MAG_085_THE_END_Nullsub_78C4A0);
		callp(states[S8(a1, 0x29)], a1);
		const uint16_t sw = (uint16_t)((U16(a1, 0xB4) + 0x20) & 0xFFF);
		U16(a1, 0xB4) = sw;
		const int32_t sn = (int32_t)x::computeSin((uint32_t)(int32_t)(int16_t)sw);
		U16(s, 0) = 0;
		U16(s, 4) = 0;
		U16(a1, 0x94) = (uint16_t)(mul32(sn, 0x300) / 4096);
		U16(s, 2) = U16(a1, 0xD4);
		x::MAG_022_sub_8DD770(s + 0xC);
		x::MAG_022_sub_8DD8A0(s + 0xC, (uint32_t)(int32_t)S16(a1, 0x96));
		x::sub_8DD7E0(s + 0xC, (uint32_t)(int32_t)S16(a1, 0x94));
		x::matrixMultiplyVector(s + 0xC, s, a1 + 0x74);
		U16(a1, 0x54) = (uint16_t)(U16(a1, 0x54) + (uint16_t)(S16(a1, 0x74) / 16));
		U16(a1, 0x56) = (uint16_t)(U16(a1, 0x56) + (uint16_t)(S16(a1, 0x76) / 16));
		U16(a1, 0x58) = (uint16_t)(U16(a1, 0x58) + (uint16_t)(S16(a1, 0x78) / 16));
		U32(a1, 0x1C) = U32(a1, 0x54);
		U32(a1, 0x20) = U32(a1, 0x58);
		const int32_t y = S16(a1, 0x20);
		if (y >= 0)
		{
			S32(a1, 0x38) = 2 * y + 0x1000;
			if (S32(a1, 0x38) > 0x4000) S32(a1, 0x38) = 0x4000;
		}
		else
		{
			S32(a1, 0x38) = y + 0x1000;
			if (S32(a1, 0x38) < 0x800) S32(a1, 0x38) = 0x800;
		}
		S32(a1, 0x34) = S32(a1, 0x38);
		S32(a1, 0x30) = S32(a1, 0x38);
		te_788700(a1);
		const uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		return task_end(a1, status);
	}

	uint32_t __cdecl te_788700(uint32_t a1)
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
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = te_785A90(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl te_788810(uint32_t a1)
	{
		const int32_t kind = S16(a1, 0xDE);
		U32(a1, 0x4C) = game_addr(game::unk_156C7FC);
		S32(a1, 0x38) = 0x1000;
		S32(a1, 0x34) = 0x1000;
		S32(a1, 0x30) = 0x1000;
		U16(a1, 0x50) = 0x1000;
		if (kind == 0)
		{
			U16(a1, 0xB4) = 0;
			U16(a1, 0xD4) = 0x400;
			U16(a1, 0x96) = 0x400;
			U16(a1, 0x54) = 0xF900;
			U16(a1, 0x56) = 0xFA00;
			U16(a1, 0x58) = 0x200;
		}
		else if (kind == 1)
		{
			U16(a1, 0xD4) = 0x400;
			U16(a1, 0xB4) = 0;
			U16(a1, 0x96) = 0xC00;
			U16(a1, 0x54) = 0x500;
			U16(a1, 0x56) = 0xFA00;
			U16(a1, 0x58) = 0;
		}
		else if (kind == 2)
		{
			U16(a1, 0x54) = 0xFC80;
			U16(a1, 0xD4) = 0x400;
			U16(a1, 0xB4) = 0x400;
			U16(a1, 0x96) = 0x400;
			U16(a1, 0x56) = 0xFA00;
			U16(a1, 0x58) = 0;
		}
		else
		{
			U16(a1, 0xD4) = 0x400;
			U16(a1, 0xB4) = (uint16_t)(x::CrtRand() & 0xFFF);
			U16(a1, 0x96) = (uint16_t)(x::CrtRand() & 0xFFF);
			U16(a1, 0x54) = (uint16_t)((x::CrtRand() & 0x1FFF) - 0x1000);
			U16(a1, 0x56) = (uint16_t)(0xFFFFFC00u - (x::CrtRand() & 0xFFF));
			U16(a1, 0x58) = (uint16_t)((x::CrtRand() & 0x1FFF) - 0x1400);
		}
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_788960(uint32_t a1)
	{
		if (a_73B7E0(3) != 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		}
		if (S16(a1, 0x1E) >= 0)
		{
			const bool impact = S16(a1, 0xDE) <= 2;
			U16(a1, 0x1E) = 0;
			if (impact)
			{
				te_785510(a1, game_addr(game::MAG_085_THE_END_FileLoadScriptTask_788A90), game_addr(game::unk_1715D28), 0x230, 0, 0);
				const uint32_t t = a_73C100(a1, game_addr(game::MAG_085_THE_END_FileLoadScriptTask_788B90), game_addr(game::unk_17151EC), 0, 0xF, 3);
				U32(t, 0x288) = U32(a1, 0x1C);
				U32(t, 0x28C) = U32(a1, 0x20);
			}
			U8(a1, 0x26) |= 5;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		}
		if (S16(a1, 0x24) <= 0x20)
		{
			U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0xFE00);
			if (S16(a1, 0x50) < 0) U16(a1, 0x50) = 0;
			return 0;
		}
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0x200);
		if (S16(a1, 0x50) < 0x1000)
			return 0;
		U16(a1, 0x50) = 0x1000;
		U8(a1, 0x26) |= 5;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_788AF0(uint32_t a1)
	{
		if (S16(a1, 0x24) < 5)
			return 0;
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		S32(a1, 0x58) = 0x1000;
		S32(a1, 0x54) = 0x1000;
		S32(a1, 0x50) = 0x1000;
		a_73C280(a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_788B40(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_73B7E0(3) != 0)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(st + 1);
			return node;
		}
		const uint32_t r = a_73C280(node);
		if (r == 0)
		{
			const uint8_t st = (uint8_t)(U8(node, 0x29) + 1);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = st;
			return st;
		}
		return r;
	}

	uint32_t __cdecl te_788BF0(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			a_73F990(a1);
			te_789700(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl te_788DD0(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::unk_25C21DC));
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

	uint32_t __cdecl te_789050(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::unk_25C21DC));
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

	uint32_t __cdecl te_789700(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::unk_25C21DC), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			te_78C3E0();
			te_789A90();
			te_789950();
			te_789790();
			sys = U32(game_addr(game::unk_25C21DC), 0);
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
		U16(game_addr(game::unk_25C21B0), 0) = (uint16_t)(U16(game_addr(game::unk_25C21B0), 0) + c14);
		U16(game_addr(game::unk_25C1CB4), 0) = (uint16_t)(U16(game_addr(game::unk_25C1CB4), 0) + c16);
		return done;
	}

	uint32_t __cdecl te_789790(void)
	{
		uint32_t act = U32(U32(game_addr(game::unk_25C21DC), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::unk_25C21DC), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					te_7897F0(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl te_7897F0(uint32_t a1, uint32_t a2)
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
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = te_785A90(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
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
				MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = te_785A90(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
			}
		}
		const uint32_t top = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 0x68;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = top;
		return top;
	}

	uint32_t __cdecl te_789950(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::unk_25C21DC)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::unk_25C21DC));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						te_789A10(act);
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
							te_789A10(act);
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

	uint32_t __cdecl te_789A10(uint32_t a1)
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

	uint32_t __cdecl te_789A90(void)
	{
		uint32_t act = U32(U32(game_addr(game::unk_25C21DC), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				te_78AD60(act);
			else if (type == 1)
				te_789AD0(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl te_789AD0(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::unk_25C21DC));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (te_78AC10(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::unk_25C21DC));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl te_78AB00(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::unk_25C21DC), 0);
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

	uint32_t __cdecl te_78AC10(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				te_78AC90(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			te_78AC90(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			te_78AC90(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl te_78AC90(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return te_78ACC0(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl te_78ACC0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::unk_25C1CB8));
		int32_t i = MEM<int16_t>(game_addr(game::unk_25BC890));
		uint32_t slot = 0;
		for (int32_t k = 0; k < 0x12; k++)
		{
			const uint32_t r = pool + (uint32_t)i * 0x6C;
			if (U8(r, 0x69) == 0)
			{
				slot = r;
				x::MAG_007_sub_8DCC00(slot, 0x6C);
				U8(slot, 0x6A) = (uint8_t)a2;
				U8(slot, 0x69) = 1;
				U16(MEM<uint32_t>(game_addr(game::unk_25C21DC)), 0x14) = (uint16_t)(U16(MEM<uint32_t>(game_addr(game::unk_25C21DC)), 0x14) + 1);
				U32(slot, 0x5C) = a1;
				U8(slot, 0x6B) = (uint8_t)a3;
				a_742CA0(slot, 0);
				break;
			}
			i++;
			if (i >= 0x11) i = 0;
		}
		i++;
		if (i >= 0x11) MEM<uint16_t>(game_addr(game::unk_25BC890)) = 0;
		else MEM<uint16_t>(game_addr(game::unk_25BC890)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl te_78AD60(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_085_THE_END_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_085_THE_END_Actor_InterpolatePosition), a1);
		te_78ADE0(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl te_78ADE0(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::unk_25C21DC), 0);
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
				te_78AE60(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			te_78AE60(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl te_78AE60(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::unk_25C21CC), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::unk_25C21CC), 0) = sp;
		uint32_t node = te_78B640(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::unk_25C21DC), 0);
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
		te_78C350(node, desc);
		U32(game_addr(game::unk_25C21CC), 0) = U32(game_addr(game::unk_25C21CC), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl te_78B640(uint32_t a1, uint32_t a2)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::unk_25C21D0));
		int32_t i = MEM<int16_t>(game_addr(game::unk_25C1CC4));
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
			MEM<uint16_t>(game_addr(game::unk_25C1CC4)) = 0;
		else
			MEM<uint16_t>(game_addr(game::unk_25C1CC4)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl te_78B860(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::unk_25C21DC), 0);
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
				uint32_t g = U32(game_addr(game::unk_25C21DC), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::unk_25C21DC), 0);
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

	uint32_t __cdecl te_78C350(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			te_78ACC0(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			te_78ACC0(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			te_78ACC0(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			te_78ACC0(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl te_78C3E0(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::unk_25C21DC));
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
					te_78ACC0(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::unk_25C21DC));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				te_78ACC0(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::unk_25C21DC));
			}
		}
		return 0;
	}

	uint32_t __cdecl te_78C470(uint32_t a1)
	{
		if (te_789700(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl te_78C4C0(uint32_t a1)
	{
		uint32_t states[3];
		states[0] = game_addr(game::MAG_085_THE_END_Ground_State0_Setup);
		states[1] = game_addr(game::MAG_085_THE_END_Ground_State1_FadeWait);
		states[2] = game_addr(game::MAG_085_THE_END_Ground_State2_Nullsub);
		callp(states[S8(a1, 0x29)], a1);
		te_78C530(a1, (uint32_t)U16(a1, 0xE0));
		const uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		return task_end(a1, status);
	}

	uint32_t __cdecl te_78C530(uint32_t a1, uint32_t a2)
	{
		if (U8(a1, 0x26) & 4)
			return 0;
		uint8_t m[0x20];
		const uint32_t M = P(m);
		x::MAG_022_sub_8DD770(M);
		x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)S16(a1, 0x46));
		x::sub_8DD7E0(M, (uint32_t)(int32_t)S16(a1, 0x44));
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
		U16(h, 0x18) = 0;
		S32(h, 0x10) = S16(a1, 0x52);
		U16(h, 0x1A) = 0;
		U16(h, 0x1E) = 0;
		U16(h, 0x1C) = 0;
		U16(h, 0x26) = 0;
		U16(h, 0x24) = 0;
		U16(h, 0x64) = (uint16_t)a2;
		U32(h, 0x14) = 0xF0;
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = te_78C660(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl te_78C660(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		if (!(U32(a1, 0x14) & 0x2000))
			U32(a1, 4) = U32(a1, 0) + 8;
		U32(a1, 0x2C) = U32(a1, 0) + U32(U32(a1, 0), 0);
		x::someCameraWork_45DD60(U8(a1, 8), U8(a1, 9), U8(a1, 0xA));
		U32(a1, 0x2C) += 0x1C;
		if (U32(U32(a1, 0x2C), 0) != 0)
			return te_78C6D0(a1, a2, a3, a4);
		U32(a1, 0x2C) += 4;
		return a4;
	}

	uint32_t __cdecl te_78C6D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pk = a4;
		uint32_t rec = U32(ctx, 0x2C);
		const int32_t n = S32(rec, 0);
		rec += 4;
		U32(ctx, 0x2C) = rec;
		const uint32_t verts = U32(ctx, 4);
		if (n <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return pk;
		}
		for (int32_t k = 0; k < n; k++, rec += 0x24)
		{
			x::GTE_LoadV012(verts + ((uint32_t)U16(rec, 4) << 2), verts + ((uint32_t)U16(rec, 6) << 2), verts + ((uint32_t)U16(rec, 8) << 2));
			x::GTE_RTPT();
			const uint32_t flags = U32(ctx, 0x14);
			U32(pk, 0) = 0x0C000000;
			U32(pk, 4) = U32(rec, 0);
			if (flags & 2) U32(pk, 4) |= 0x2000000;
			if (flags & 8) U32(pk, 4) &= 0xFDFFFFFF;
			U32(pk, 0xC) = U32(rec, 0xC);
			const uint32_t uv23 = U32(rec, 0x14);
			U32(pk, 0x24) = uv23;
			U32(pk, 0x18) = U32(rec, 0x10);
			U32(pk, 0x30) = uv23 >> 16;
			x::GTE_ReadFLAG(ctx + 0x3C);
			if (U32(ctx, 0x3C) & 0x60000) continue;
			x::GTE_NCLIP();
			uint32_t clip = 0;
			x::GTE_ReadMAC0(ctx + 0x30);
			if (S32(ctx, 0x30) < 0 && !(U8(ctx, 0x14) & 0x20)) continue;
			x::GTE_ReadSXY012_Split(pk + 8, pk + 0x14, pk + 0x20);
			x::GTE_LoadV0(verts + ((uint32_t)U16(rec, 0xA) << 2));
			x::GTE_RTPS();
			if (te_offx(S16(pk, 8))) clip = 1;
			if (te_offx(S16(pk, 0x14))) clip |= 2;
			if (te_offx(S16(pk, 0x20))) clip |= 4;
			if (te_offy(S16(pk, 0xA))) clip |= 0x10;
			if (te_offy(S16(pk, 0x16))) clip |= 0x20;
			if (te_offy(S16(pk, 0x22))) clip |= 0x40;
			x::GTE_ReadSXY2(pk + 0x2C);
			x::GTE_AVSZ4();
			if (te_offx(S16(pk, 0x2C))) clip |= 8;
			if (te_offy(S16(pk, 0x2E))) clip |= 0x80;
			if ((clip & 0xF) == 0xF) continue;
			if ((clip & 0xF0) == 0xF0) continue;
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x80)
			{
				x::sub_45E120(rec + 0x18, rec + 0x1C, rec + 0x20);
				x::set_dword_1CA8A30(U32(ctx, 0xC));
				x::sub_45F4C0();
				x::sub_45E370(pk + 0x10, pk + 0x1C, pk + 0x28);
				x::set_unk_1CA8A28(pk + 4);
				x::sub_45F270();
				x::set_param_with_dword_1CA8A68(pk + 4);
			}
			else
			{
				U32(pk, 0x10) = U32(rec, 0x18);
				U32(pk, 0x1C) = U32(rec, 0x1C);
				U32(pk, 0x28) = U32(rec, 0x20);
			}
			S32(ctx, 0x38) = add32(S32(ctx, 0x38), S32(ctx, 0x10));
			if (S32(ctx, 0x38) < 0) S32(ctx, 0x38) = 0;
			const uint32_t bucket = a2 + (uint32_t)(S32(ctx, 0x38) >> (a3 & 31)) * 4u;
			const uint16_t us = U16(ctx, 0x18), vs = U16(ctx, 0x1A);
			if ((uint16_t)(us | vs) != 0)
			{
				if (us != 0) te_scroll(pk, 0, us, U8(ctx, 0x28));
				if (vs != 0) te_scroll(pk, 1, vs, U8(ctx, 0x2A));
				const uint32_t w1 = pk + 0x34;
				U32(w1, 0) = 0x2000000;
				U32(w1, 4) = te_texwin(ctx, 0x1C);
				U32(w1, 8) = 0;
				x::SSIGPU_InsertPrimAutoDepth(bucket, w1);
				x::SSIGPU_InsertPrimAutoDepth(bucket, pk);
				const uint32_t w2 = pk + 0x40;
				U32(w2, 0) = 0x2000000;
				U32(w2, 4) = te_texwin(ctx, 0x24);
				U32(w2, 8) = 0;
				x::SSIGPU_InsertPrimAutoDepth(bucket, w2);
				pk += 0x4C;
				continue;
			}
			const int16_t f = S16(ctx, 0x64);
			int16_t l0 = (int16_t)(U8(pk, 4) - f), l1 = (int16_t)(U8(pk, 0x10) - f), l2 = (int16_t)(U8(pk, 0x1C) - f), l3 = (int16_t)(U8(pk, 0x28) - f);
			if (l0 < 0 || l1 < 0 || l2 < 0 || l3 < 0) continue;
			if (l0 > 8) l0 = 8;
			if (l1 > 8) l1 = 8;
			if (l2 > 8) l2 = 8;
			if (l3 > 8) l3 = 8;
			const uint8_t g0 = (uint8_t)(l0 << 4), g1 = (uint8_t)(l1 << 4), g2 = (uint8_t)(l2 << 4), g3 = (uint8_t)(l3 << 4);
			U8(pk, 6) = g0; U8(pk, 5) = g0; U8(pk, 4) = g0;
			U8(pk, 0x12) = g1; U8(pk, 0x11) = g1; U8(pk, 0x10) = g1;
			U8(pk, 0x1E) = g2; U8(pk, 0x1D) = g2; U8(pk, 0x1C) = g2;
			U8(pk, 0x2A) = g3; U8(pk, 0x29) = g3; U8(pk, 0x28) = g3;
			x::SSIGPU_InsertPrimAutoDepth(bucket, pk);
			pk += 0x34;
		}
		U32(ctx, 0x2C) = rec;
		return pk;
	}

	uint32_t __cdecl te_78CE00(uint32_t a1)
	{
		S32(a1, 0x38) = 0x1000;
		S32(a1, 0x34) = 0x1000;
		S32(a1, 0x30) = 0x1000;
		U32(a1, 0x4C) = game_addr(game::unk_156C8BC);
		U16(a1, 0xE0) = 0x80;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_78CE30(uint32_t a1)
	{
		U16(a1, 0xE0) = (uint16_t)(U16(a1, 0xE0) - 1);
		if (S16(a1, 0xE0) < 0x6C) U16(a1, 0xE0) = 0x6C;
		if (a_73B7E0(3) != 0)
		{
			U8(a1, 0x26) |= 5;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl te_78CE80(uint32_t a1)
	{
		const uint32_t states[2] = { game_addr(game::MAG_085_THE_END_PetalEmitter_SpawnGroup), game_addr(game::MAG_085_THE_END_Nullsub_78D240) };
		callp(states[S8(a1, 0x29)], a1);
		return task_end(a1);
	}

	uint32_t __cdecl te_78CED0(uint32_t a1)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25BC898), game_addr(game::MAG_085_THE_END_PetalGroupTask), 0xE4, a1);
		uint32_t st = t + 0xD4, p = t + 0x56;
		for (int k = 0; k < 4; k++, st += 2, p += 8)
		{
			U16(p, -2) = (uint16_t)((x::CrtRand() & 0x7FFF) - 0x4000);
			const uint16_t y = (uint16_t)(0x400 - (x::CrtRand() & 0x1FFF));
			U16(p, 0) = y;
			if ((int16_t)y > 0) U16(p, 0) = 0;
			U16(p, 2) = (uint16_t)((x::CrtRand() & 0x7FFF) - 0x2000);
			U16(st, 0) = (uint16_t)((x::CrtRand() & 0xFFF) + 0x400);
			U16(p, 0x62) = (uint16_t)(x::CrtRand() & 0xFFF);
		}
		U16(a1, 0xDE) = (uint16_t)(U16(a1, 0xDE) + 1);
		if (a_73B7E0(6) != 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl te_78CFA0(uint32_t a1)
	{
		uint8_t sp[0x28];
		const uint32_t s = P(sp);
		uint32_t states[5];
		states[0] = game_addr(game::MAG_085_THE_END_PetalGroup_State0_Setup);
		states[1] = game_addr(game::MAG_085_THE_END_PetalGroup_State1_FadeOut);
		states[2] = game_addr(game::MAG_085_THE_END_PetalGroup_State2_Wait);
		states[3] = game_addr(game::MAG_085_THE_END_PetalGroup_State3_FadeIn);
		states[4] = game_addr(game::MAG_085_THE_END_PetalGroup_State4_Nullsub);
		callp(states[S8(a1, 0x29)], a1);
		uint32_t p = a1 + 0x98, st = a1 + 0xD4;
		for (int k = 0; k < 4; k++, st += 2, p += 8)
		{
			const uint16_t sw = (uint16_t)((U16(p, 0x20) + 0x40) & 0xFFF);
			U16(p, 0x20) = sw;
			const int32_t sn = (int32_t)x::computeSin((uint32_t)(int32_t)(int16_t)sw);
			U16(p, 0) = (uint16_t)(shl32(sn, 8) / 4096 - 0xC0);
			U16(s, 0) = U16(st, 0);
			U16(s, 2) = 0;
			U16(s, 4) = 0;
			x::MAG_022_sub_8DD770(s + 8);
			x::sub_8DD960(s + 8, (uint32_t)(int32_t)S16(p, 0));
			const uint32_t v = p - 0x24;
			x::matrixMultiplyVector(s + 8, s, v);
			U16(p, -0x44) = (uint16_t)(U16(p, -0x44) + (uint16_t)(S16(v, 0) / 16));
			U16(p, -0x42) = (uint16_t)(U16(p, -0x42) + (uint16_t)(S16(v, 2) / 16));
			U16(p, -0x40) = (uint16_t)(U16(p, -0x40) + (uint16_t)(S16(v, 4) / 16));
			if (S16(p, -0x42) >= 0) U16(p, -0x42) = 0;
		}
		uint32_t q = a1 + 0x54;
		for (int k = 0; k < 4; k++, q += 8)
		{
			U32(a1, 0x1C) = U32(q, 0);
			U32(a1, 0x20) = U32(q, 4);
			const int32_t y = S16(a1, 0x20);
			if (y >= 0)
			{
				S32(a1, 0x38) = y + 0x1000;
				if (S32(a1, 0x38) > 0x2000) S32(a1, 0x38) = 0x2000;
			}
			else
			{
				S32(a1, 0x38) = y + 0x1000;
				if (S32(a1, 0x38) < 0x400) S32(a1, 0x38) = 0x400;
			}
			S32(a1, 0x34) = S32(a1, 0x38);
			S32(a1, 0x30) = S32(a1, 0x38);
			te_788700(a1);
		}
		const uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		return task_end(a1, status);
	}

	uint32_t __cdecl te_78D150(uint32_t a1)
	{
		S32(a1, 0x38) = 0x1000;
		S32(a1, 0x34) = 0x1000;
		S32(a1, 0x30) = 0x1000;
		U16(a1, 0x50) = 0x1000;
		U32(a1, 0x4C) = game_addr(game::unk_156C85C);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_78D180(uint32_t a1)
	{
		if (a_73B7E0(6) != 0) { U8(a1, 0x26) |= 5; return 0; }
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0xFE00);
		if (S16(a1, 0x50) <= 0)
		{
			U16(a1, 0x50) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl te_78D1C0(uint32_t a1)
	{
		if (a_73B7E0(6) != 0) { U8(a1, 0x26) |= 5; return 0; }
		if (S16(a1, 0x24) >= 0x18) U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_78D1F0(uint32_t a1)
	{
		if (a_73B7E0(6) != 0) { U8(a1, 0x26) |= 5; return 0; }
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0x200);
		if (S16(a1, 0x50) >= 0x1000)
		{
			U8(a1, 0x26) |= 5;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			U16(a1, 0x50) = 0x1000;
		}
		return 0;
	}

	uint32_t __cdecl te_78D250(uint32_t a1)
	{
		const uint32_t tab[4] = { game_addr(game::MAG_085_THE_END_ScreenGlow_State0_Widen), game_addr(game::MAG_085_THE_END_ScreenGlow_State1_Rise), game_addr(game::MAG_085_THE_END_ScreenGlow_State2_WaitCut6), game_addr(game::MAG_085_THE_END_ScreenGlow_State3_Nullsub) };
		callp(tab[S8(a1, 0x29)], a1);
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24)++;
		return task_end(a1, status);
	}

	uint32_t __cdecl te_78D2B0(uint32_t a1)
	{
		U16(a1, 0x58) = (uint16_t)(U16(a1, 0x58) + 8);
		if (S16(a1, 0x58) >= 0x80)
		{
			U16(a1, 0x58) = 0x80;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		te_78D2E0(a1);
		return 0;
	}

	uint32_t __cdecl te_78D2E0(uint32_t a1)
	{
		const uint32_t pk = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		U8(pk, 0x24) = 0;
		U8(pk, 0xC) = 0;
		U32(pk, 0) = 0xC000000;
		U8(pk, 0x30) = U8(a1, 0x58);
		U8(pk, 0x18) = U8(a1, 0x58);
		U8(pk, 7) = 0x3E;
		U8(pk, 0x19) = 0xE0;
		U8(pk, 0xD) = 0xE0;
		U16(pk, 0x1A) = 0xB6;
		U8(pk, 0x31) = 0xFF;
		U8(pk, 0x25) = 0xFF;
		U16(pk, 0xE) = 0x3D54;
		const uint16_t x1 = (uint16_t)(U16(a1, 0x58) + 0xBA);
		U16(pk, 0x20) = 0xBA;
		U16(pk, 0x2C) = x1;
		U16(pk, 0x14) = x1;
		U16(pk, 8) = 0xBA;
		U16(pk, 0x16) = 0xA0;
		U16(pk, 0xA) = 0xA0;
		U16(pk, 0x2E) = 0xC0;
		U16(pk, 0x22) = 0xC0;
		U8(pk, 0x1E) = 0x80; U8(pk, 0x1D) = 0x80; U8(pk, 0x1C) = 0x80;
		U8(pk, 6) = 0x80; U8(pk, 5) = 0x80; U8(pk, 4) = 0x80;
		const uint8_t l = U8(a1, 0x5A);
		U8(pk, 0x2A) = l; U8(pk, 0x29) = l; U8(pk, 0x28) = l;
		U8(pk, 0x12) = l; U8(pk, 0x11) = l; U8(pk, 0x10) = l;
		x::SSIGPU_InsertPrimAltViewport(MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x14, pk);
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = pk + 0x34;
		return pk + 0x34;
	}

	uint32_t __cdecl te_78D3A0(uint32_t a1)
	{
		U16(a1, 0x5A) = (uint16_t)(U16(a1, 0x5A) + 0x10);
		if (S16(a1, 0x5A) >= 0x80)
		{
			U16(a1, 0x5A) = 0x80;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		te_78D2E0(a1);
		return 0;
	}

	uint32_t __cdecl te_78D3D0(uint32_t a1)
	{
		if (a_73B7E0(6) != 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		}
		te_78D2E0(a1);
		return 0;
	}

	uint32_t __cdecl te_78D480(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0x10)
		{
			U16(a1, 0x1E) = 0x18;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl te_78D4A0(uint32_t a1)
	{
		if (a_73B7E0(6) != 0)
		{
			U16(a1, 0x5C) = 4;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		te_78D4D0(a1, (uint32_t)U16(a1, 0x1E));
		return 0;
	}

	uint32_t __cdecl te_78D4D0(uint32_t a1, uint32_t a2)
	{
		(void)a1;
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x14;
		uint32_t pk = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		const uint16_t b = (uint16_t)a2;
		U32(pk, 0) = 0x5000000;
		U8(pk, 7) = 0x29;
		U16(pk, 0x10) = 0;
		U16(pk, 8) = 0;
		U16(pk, 0x14) = 0x140;
		U16(pk, 0xC) = 0x140;
		U16(pk, 0xE) = 0;
		U16(pk, 0xA) = 0;
		U16(pk, 0x16) = b;
		U16(pk, 0x12) = b;
		U8(pk, 6) = 0;
		U8(pk, 5) = 0;
		U8(pk, 4) = 0;
		x::SSIGPU_InsertPrimAltViewport(ot, pk);
		pk += 0x18;
		const uint16_t y0 = (uint16_t)(0xD8 - b);
		U16(pk, 0x14) = 0x140;
		U16(pk, 0xC) = 0x140;
		U32(pk, 0) = 0x5000000;
		U8(pk, 7) = 0x29;
		U16(pk, 0x10) = 0;
		U16(pk, 8) = 0;
		U16(pk, 0xE) = y0;
		U16(pk, 0xA) = y0;
		U16(pk, 0x16) = 0xD8;
		U16(pk, 0x12) = 0xD8;
		U8(pk, 6) = 0;
		U8(pk, 5) = 0;
		U8(pk, 4) = 0;
		x::SSIGPU_InsertPrimAltViewport(ot, pk);
		pk += 0x18;
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = pk;
		return pk;
	}

	uint32_t __cdecl te_78D590(uint32_t a1)
	{
		U16(a1, 0x5C) = (uint16_t)(U16(a1, 0x5C) - 1);
		if (S16(a1, 0x5C) <= 0) U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		te_78D4D0(a1, (uint32_t)U16(a1, 0x1E));
		return 0;
	}

	uint32_t __cdecl te_78D5C0(uint32_t a1)
	{
		U16(a1, 0x1E) = (uint16_t)(U16(a1, 0x1E) - 4);
		if (S16(a1, 0x1E) <= 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		}
		te_78D4D0(a1, (uint32_t)U16(a1, 0x1E));
		return 0;
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_085_THE_END_Sequence_State0_Advance_7847C0, (void *)a_73A0D0, "085 MAG_085_sub_7847C0" },
		{ game::MAG_085_THE_END_Sequence_State0_Advance_7847D0, (void *)a_73A0D0, "085 MAG_085_sub_7847D0" },
		{ game::MAG_085_THE_END_Sequence_State3_SpawnEmitter, (void *)a_73A170, "085 MAG_085_sub_784860" },
		{ game::MAG_085_THE_END_EmitterTask, (void *)a_73A1A0, "085 MAG_085_sub_784890" },
		{ game::MAG_085_THE_END_FileLoadScriptTask_784A60, (void *)a_73A380, "085 MAG_085_sub_784A60" },
		{ game::MAG_085_THE_END_LoadScript_Issue, (void *)a_73A3E0, "085 sub_784AC0" },
		{ game::MAG_085_THE_END_RequeueTIMUpload, (void *)a_73A580, "085 MAG_085_THE_END_RequeueTIMUpload" },
		{ game::MAG_085_THE_END_LoadCb_SummonStream, (void *)a_73A5B0, "085 sub_784C90" },
		{ game::MAG_085_THE_END_LoadCb_ClearBusy_784CC0, (void *)a_73A5E0, "085 sub_784CC0" },
		{ game::MAG_085_THE_END_LoadCb_ClearBusy_784CD0, (void *)a_73A5E0, "085 sub_784CD0" },
		{ game::MAG_085_THE_END_LoadCb_ClearBusy_784CE0, (void *)a_73A5E0, "085 sub_784CE0" },
		{ game::MAG_085_THE_END_LoadCb_ClearBusy_784CF0, (void *)a_73A5E0, "085 sub_784CF0" },
		{ game::MAG_085_THE_END_LoadScript_RecordFile, (void *)a_73A620, "085 sub_784D00" },
		{ game::MAG_085_THE_END_FileLoadScript_State1_LoadScriptWait, (void *)a_73A660, "085 sub_784D40" },
		{ game::MAG_085_THE_END_Nullsub_784DB0, (void *)a_73A6D0, "085 nullsub_1311" },
		{ game::MAG_085_THE_END_DirSyncGate, (void *)a_73B640, "085 sub_784FA0" },
		{ game::MAG_085_THE_END_DirPhaseGE, (void *)a_73B7E0, "085 sub_785000" },
		{ game::MAG_085_THE_END_Director_State2_ArmLoadScriptStep, (void *)a_73A930, "085 sub_785020" },
		{ game::MAG_085_THE_END_LoadScript_TryArmStep, (void *)a_73A950, "085 sub_785040" },
		{ game::MAG_085_THE_END_SetPauseSwitch, (void *)a_73BB60, "085 sub_7850C0" },
		{ game::MAG_085_THE_END_Director_State5_WaitDirGate2, (void *)a_73B8A0, "085 sub_785130" },
		{ game::MAG_085_THE_END_Director_State6_WaitDirPhase2, (void *)a_73B8C0, "085 sub_785150" },
		{ game::MAG_085_THE_END_Director_State8_Countdown, (void *)a_73BC40, "085 sub_7851C0" },
		{ game::MAG_085_THE_END_Director_State9_WaitDirGate3, (void *)a_733B70, "085 sub_7851E0" },
		{ game::MAG_085_THE_END_Director_State13_WaitDirPhase4, (void *)a_733760, "085 sub_785270" },
		{ game::MAG_085_THE_END_Director_State15_WaitDirGate5, (void *)a_73B620, "085 sub_7852B0" },
		{ game::MAG_085_THE_END_Director_State21_Finish, (void *)a_7475A0, "085 sub_785380" },
		{ game::MAG_085_THE_END_Director_State22_Nullsub, (void *)a_73A6D0, "085 nullsub_1312" },
		{ game::MAG_085_THE_END_Nullsub_7853B0, (void *)a_73A6D0, "085 nullsub_1313" },
		{ game::MAG_085_THE_END_FileLoadScriptTask_785560, (void *)a_73A380, "085 sub_785560" },
		{ game::MAG_085_THE_END_DrawPrimModel, (void *)a_73C280, "085 sub_785660" },
		{ game::MAG_085_THE_END_EmitG3, (void *)a_73D770, "085 sub_786A70" },
		{ game::MAG_085_THE_END_EmitG4, (void *)a_73D9C0, "085 sub_786CC0" },
		{ game::MAG_085_THE_END_PrimPlay_State2_Nullsub, (void *)a_73A6D0, "085 nullsub_1314" },
		{ game::MAG_085_THE_END_OrbitPlay_State3_Nullsub, (void *)a_73A6D0, "085 nullsub_1315" },
		{ game::MAG_085_THE_END_ModelTaskC_787E20, (void *)a_73F110, "085 sub_787E20" },
		{ game::MAG_085_THE_END_ClearEntityRecordBit1, (void *)a_746A10, "085 sub_787F10" },
		{ game::MAG_085_THE_END_RestoreEntityRecordBit1, (void *)a_746AE0, "085 sub_787F50" },
		{ game::MAG_085_THE_END_StageFade_State3_Nullsub, (void *)a_73A6D0, "085 nullsub_1316" },
		{ game::MAG_085_THE_END_ModelTaskC_787FD0, (void *)a_73F110, "085 sub_787FD0" },
		{ game::MAG_085_THE_END_SetEntityColourTint, (void *)a_733950, "085 sub_788120" },
		{ game::MAG_085_THE_END_Wobble_State4_Nullsub, (void *)a_73A6D0, "085 nullsub_1317" },
		{ game::MAG_085_THE_END_ModelTaskC_7882E0, (void *)a_73F110, "085 sub_7882E0" },
		{ game::MAG_085_THE_END_MusicFade_State4_Nullsub, (void *)a_73A6D0, "085 nullsub_1318" },
		{ game::MAG_085_THE_END_FileLoadScriptTask_788420, (void *)a_73A380, "085 sub_788420" },
		{ game::MAG_085_THE_END_SpawnChildTask, (void *)a_73C100, "085 sub_788A40" },
		{ game::MAG_085_THE_END_FileLoadScriptTask_788A90, (void *)a_73A380, "085 sub_788A90" },
		{ game::MAG_085_THE_END_ImpactPlay_State1_Nullsub, (void *)a_73A6D0, "085 nullsub_1326" },
		{ game::MAG_085_THE_END_FileLoadScriptTask_788B90, (void *)a_73A380, "085 sub_788B90" },
		{ game::MAG_085_THE_END_DirectorSetup, (void *)a_73F990, "085 sub_788C20" },
		{ game::MAG_085_THE_END_Director_StartPositions, (void *)a_73FC20, "085 sub_788EB0" },
		{ game::MAG_085_THE_END_Director_Mode3Setup, (void *)a_73FE90, "085 sub_789120" },
		{ game::MAG_085_THE_END_Director_TargetPositions, (void *)a_73FFB0, "085 sub_789240" },
		{ game::MAG_085_THE_END_RelocateActorData, (void *)a_740210, "085 sub_7894A0" },
		{ game::MAG_085_THE_END_ActorMotion, (void *)a_740910, "085 sub_789B60" },
		{ game::MAG_085_THE_END_DampValue, (void *)a_740F70, "085 sub_78A1C0" },
		{ game::MAG_085_THE_END_DampVector, (void *)a_740FC0, "085 sub_78A210" },
		{ game::MAG_085_THE_END_Instance_AdvanceTexFrame, (void *)a_741050, "085 sub_78A2A0" },
		{ game::MAG_085_THE_END_InstanceBuildTransform, (void *)a_741120, "085 sub_78A370" },
		{ game::MAG_085_THE_END_Particle_LifeCountdown, (void *)a_741B60, "085 sub_78ADB0" },
		{ game::MAG_085_RandRange, (void *)a_742290, "085 MAG_085_RandRange" },
		{ game::MAG_085_THE_END_RandRangeWide, (void *)a_7422C0, "085 sub_78B510" },
		{ game::MAG_085_THE_END_RandVec3, (void *)a_742300, "085 sub_78B550" },
		{ game::MAG_085_THE_END_JitterAngles3, (void *)a_742350, "085 sub_78B5A0" },
		{ game::MAG_085_RandAngleJitter, (void *)a_7423A0, "085 MAG_085_RandAngleJitter" },
		{ game::MAG_085_THE_END_GetWord8Minus1, (void *)a_7424B0, "085 sub_78B700" },
		{ game::MAG_085_THE_END_Particle_AttachToOwner, (void *)a_7424C0, "085 sub_78B710" },
		{ game::MAG_085_THE_END_ParticleList_Append, (void *)a_742CA0, "085 sub_78BEF0" },
		{ game::MAG_085_THE_END_ParticleList_Unlink, (void *)a_742CD0, "085 sub_78BF20" },
		{ game::MAG_085_THE_END_Particle_Init, (void *)a_742D00, "085 sub_78BF50" },
		{ game::MAG_085_THE_END_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "085 sub_78C100" },
		{ game::MAG_085_THE_END_Particle_PlaceAtPoint, (void *)a_742FF0, "085 sub_78C240" },
		{ game::MAG_085_THE_END_Nullsub_78C490, (void *)a_73A6D0, "085 nullsub_1327" },
		{ game::MAG_085_THE_END_Nullsub_78C4A0, (void *)a_73A6D0, "085 nullsub_1328" },
		{ game::MAG_085_THE_END_Nullsub_78C4B0, (void *)a_73A6D0, "085 nullsub_1319" },
		{ game::MAG_085_THE_END_Ground_State2_Nullsub, (void *)a_73A6D0, "085 nullsub_1320" },
		{ game::MAG_085_THE_END_PetalGroup_State4_Nullsub, (void *)a_73A6D0, "085 nullsub_1321" },
		{ game::MAG_085_THE_END_Nullsub_78D240, (void *)a_73A6D0, "085 nullsub_1322" },
		{ game::MAG_085_THE_END_ScreenGlow_State3_Nullsub, (void *)a_73A6D0, "085 nullsub_1323" },
		{ game::MAG_085_THE_END_ModelTaskC_78D410, (void *)a_73F110, "085 sub_78D410" },
		{ game::MAG_085_THE_END_Letterbox_State4_Nullsub, (void *)a_73A6D0, "085 nullsub_1324" },
		{ game::MAG_085_THE_END_Emitter_State1_ApplyActionResults, (void *)a_747440, "085 MAG_085_sub_78D600" },
		{ game::MAG_085_THE_END_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "085 MAG_085_sub_78D670" },
		{ game::MAG_085_THE_END_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "085 MAG_085_sub_78D690" },
		{ game::MAG_085_THE_END_Nullsub_78D6B0, (void *)a_73A6D0, "085 nullsub_1325" },
		{ game::MAG_085_THE_END_Sequence_State4_NextTargetLoop, (void *)a_747500, "085 MAG_085_sub_78D6C0" },
		{ game::MAG_085_THE_END_Sequence_State0_Advance_78D700, (void *)a_73A0D0, "085 MAG_085_sub_78D700" },
		{ game::MAG_085_THE_END_Sequence_State6_WaitQueuesEmpty, (void *)a_747550, "085 MAG_085_sub_78D710" },
		{ game::MAG_085_THE_END_Sequence_State7_RequestBattleSync, (void *)a_747560, "085 MAG_085_sub_78D720" },
		{ game::MAG_085_THE_END_Sequence_State8_WaitBattleSyncDone, (void *)a_747590, "085 MAG_085_sub_78D750" },
		{ game::MAG_085_THE_END_Sequence_State9_Finish_78D760, (void *)a_7475A0, "085 MAG_085_sub_78D760" },
		{ game::MAG_085_THE_END_Nullsub_78D780, (void *)a_73A6D0, "085 nullsub_1310" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_085_THE_END_Tick, (void *)te_784630, "085 te_784630" },
		{ game::MAG_085_THE_END_Master_State2_CarvePools, (void *)te_7847E0, "085 te_7847E0" },
		{ game::MAG_085_THE_END_ClearPools, (void *)te_784820, "085 te_784820" },
		{ game::MAG_085_THE_END_Master_InitDirector, (void *)te_784910, "085 te_784910" },
		{ game::MAG_085_THE_END_LoaderSetup, (void *)te_784960, "085 te_784960" },
		{ game::MAG_085_THE_END_DirectorDataSetup, (void *)te_784DC0, "085 te_784DC0" },
		{ game::MAG_085_THE_END_DirectorTask, (void *)te_784E30, "085 te_784E30" },
		{ game::MAG_085_THE_END_Director_CutBookkeeping, (void *)te_784F30, "085 te_784F30" },
		{ game::MAG_085_THE_END_Director_State0_WaitCut1_BattleUIOff, (void *)te_784F80, "085 te_784F80" },
		{ game::MAG_085_THE_END_Director_State1_PlaySound1, (void *)te_784FD0, "085 te_784FD0" },
		{ game::MAG_085_THE_END_Director_State3_WaitLoadDone, (void *)te_785080, "085 te_785080" },
		{ game::MAG_085_THE_END_Director_State4_WaitFrame4B, (void *)te_785110, "085 te_785110" },
		{ game::MAG_085_THE_END_Director_State7_ScreenTint, (void *)te_785170, "085 te_785170" },
		{ game::MAG_085_THE_END_Director_State10_WaitCut3_PlaySound2, (void *)te_785200, "085 te_785200" },
		{ game::MAG_085_THE_END_Director_State11_WaitFrame3C, (void *)te_785230, "085 te_785230" },
		{ game::MAG_085_THE_END_Director_State12_WaitDirGate4, (void *)te_785250, "085 te_785250" },
		{ game::MAG_085_THE_END_Director_State14_WaitFrame2D_A, (void *)te_785290, "085 te_785290" },
		{ game::MAG_085_THE_END_Director_State16_WaitDirPhase5, (void *)te_7852D0, "085 te_7852D0" },
		{ game::MAG_085_THE_END_Director_State17_WaitFrame2D_B, (void *)te_7852F0, "085 te_7852F0" },
		{ game::MAG_085_THE_END_Director_State18_WaitDirGate6, (void *)te_785310, "085 te_785310" },
		{ game::MAG_085_THE_END_Director_State19_WaitDirPhase6, (void *)te_785330, "085 te_785330" },
		{ game::MAG_085_THE_END_Director_State20_BattleUIOn, (void *)te_785350, "085 te_785350" },
		{ game::MAG_085_THE_END_Director_NewCut, (void *)te_7853C0, "085 te_7853C0" },
		{ game::MAG_085_THE_END_SpawnPrimPlayTask, (void *)te_785510, "085 te_785510" },
		{ game::MAG_085_THE_END_PrimPlay_State0_DecodeLayout, (void *)te_7855C0, "085 te_7855C0" },
		{ game::MAG_085_THE_END_DrawEntityShadows, (void *)te_785620, "085 te_785620" },
		{ game::MAG_085_THE_END_PrimPlay_ObjectCallback, (void *)te_785790, "085 te_785790" },
		{ game::MAG_085_THE_END_RenderPrimModel, (void *)te_785A90, "085 te_785A90" },
		{ game::MAG_085_THE_END_PrimList_FlatTri, (void *)te_785BC0, "085 te_785BC0" },
		{ game::MAG_085_THE_END_PrimList_FlatQuad, (void *)te_785DE0, "085 te_785DE0" },
		{ game::MAG_085_THE_END_PrimList_TexTri, (void *)te_786070, "085 te_786070" },
		{ game::MAG_085_THE_END_PrimList_TexQuad, (void *)te_7864F0, "085 te_7864F0" },
		{ game::MAG_085_THE_END_PrimList_GouraudTexTriScroll, (void *)te_786FA0, "085 te_786FA0" },
		{ game::MAG_085_THE_END_PrimList_GT4, (void *)te_7874A0, "085 te_7874A0" },
		{ game::MAG_085_THE_END_PrimPlay_State1_Play, (void *)te_787AF0, "085 te_787AF0" },
		{ game::MAG_085_THE_END_OrbitPlayTask, (void *)te_787B50, "085 te_787B50" },
		{ game::MAG_085_THE_END_OrbitPlay_State0_Wait, (void *)te_787BB0, "085 te_787BB0" },
		{ game::MAG_085_THE_END_OrbitPlay_State1_Setup, (void *)te_787BE0, "085 te_787BE0" },
		{ game::MAG_085_THE_END_OrbitPlay_State2_Orbit, (void *)te_787C90, "085 te_787C90" },
		{ game::MAG_085_THE_END_StageFade_State0_Init, (void *)te_787E90, "085 te_787E90" },
		{ game::MAG_085_THE_END_StageFade_State1_FadeIn, (void *)te_787ED0, "085 te_787ED0" },
		{ game::MAG_085_THE_END_StageFade_State2_WaitCut6, (void *)te_787F30, "085 te_787F30" },
		{ game::MAG_085_THE_END_StageFadeOut_Step, (void *)te_787F80, "085 te_787F80" },
		{ game::MAG_085_THE_END_Wobble_State0_Init, (void *)te_788040, "085 te_788040" },
		{ game::MAG_085_THE_END_Wobble_State1_Rise, (void *)te_788060, "085 te_788060" },
		{ game::MAG_085_THE_END_HidePartyModels, (void *)te_7880A0, "085 te_7880A0" },
		{ game::MAG_085_THE_END_ApplyEntityFadeLevel, (void *)te_7880E0, "085 te_7880E0" },
		{ game::MAG_085_THE_END_Wobble_State2_WaitCut6, (void *)te_788220, "085 te_788220" },
		{ game::MAG_085_THE_END_ShowPartyModels, (void *)te_788250, "085 te_788250" },
		{ game::MAG_085_THE_END_Wobble_State3_Fall, (void *)te_788290, "085 te_788290" },
		{ game::MAG_085_THE_END_MusicFade_State0_Init, (void *)te_788350, "085 te_788350" },
		{ game::MAG_085_THE_END_MusicFade_State1_FadeOut, (void *)te_788370, "085 te_788370" },
		{ game::MAG_085_THE_END_MusicFade_State2_WaitDirPhase6, (void *)te_7883B0, "085 te_7883B0" },
		{ game::MAG_085_THE_END_MusicFade_State3_FadeIn, (void *)te_7883D0, "085 te_7883D0" },
		{ game::MAG_085_THE_END_Heartbreak_Ring_Wait, (void *)te_788480, "085 te_788480" },
		{ game::MAG_085_THE_END_MeteorEmitter_SpawnMeteors, (void *)te_788490, "085 te_788490" },
		{ game::MAG_085_THE_END_MeteorTask, (void *)te_788580, "085 te_788580" },
		{ game::MAG_085_THE_END_DrawModel, (void *)te_788700, "085 te_788700" },
		{ game::MAG_085_THE_END_Meteor_State0_Setup, (void *)te_788810, "085 te_788810" },
		{ game::MAG_085_THE_END_Meteor_State1_Fall, (void *)te_788960, "085 te_788960" },
		{ game::MAG_085_THE_END_ImpactPlay_State0_Play, (void *)te_788AF0, "085 te_788AF0" },
		{ game::MAG_085_THE_END_Tonberry_Particle_State2_WaitCue3OrEnd, (void *)te_788B40, "085 te_788B40" },
		{ game::MAG_085_THE_END_ActorSystem_State0_StartActorSystemAtFrame, (void *)te_788BF0, "085 te_788BF0" },
		{ game::MAG_085_THE_END_ResetTrailHistory, (void *)te_788DD0, "085 te_788DD0" },
		{ game::MAG_085_THE_END_FillTrailRows, (void *)te_789050, "085 te_789050" },
		{ game::MAG_085_THE_END_ActSys_Step, (void *)te_789700, "085 te_789700" },
		{ game::MAG_085_THE_END_ActSys_DrawPass, (void *)te_789790, "085 te_789790" },
		{ game::MAG_085_THE_END_ActSys_DrawPrimActor, (void *)te_7897F0, "085 te_7897F0" },
		{ game::MAG_085_THE_END_ActSys_DrawSpritePass, (void *)te_789950, "085 te_789950" },
		{ game::MAG_085_THE_END_ActSys_DrawActorSprite, (void *)te_789A10, "085 te_789A10" },
		{ game::MAG_085_THE_END_ActSys_UpdatePass, (void *)te_789A90, "085 te_789A90" },
		{ game::MAG_085_THE_END_ActorStep, (void *)te_789AD0, "085 te_789AD0" },
		{ game::MAG_085_THE_END_MorphVertexBlock, (void *)te_78AB00, "085 te_78AB00" },
		{ game::MAG_085_THE_END_Instance_EndOfLifeTest, (void *)te_78AC10, "085 te_78AC10" },
		{ game::MAG_085_THE_END_Instance_SpawnFollowUp, (void *)te_78AC90, "085 te_78AC90" },
		{ game::MAG_085_THE_END_Actor_AllocSlot, (void *)te_78ACC0, "085 te_78ACC0" },
		{ game::MAG_085_THE_END_ActSys_UpdateEmitter, (void *)te_78AD60, "085 te_78AD60" },
		{ game::MAG_085_THE_END_ActSys_EmitterSpawnStep, (void *)te_78ADE0, "085 te_78ADE0" },
		{ game::MAG_085_THE_END_ActSys_SpawnParticles, (void *)te_78AE60, "085 te_78AE60" },
		{ game::MAG_085_THE_END_ActSys_AllocParticleSlot, (void *)te_78B640, "085 te_78B640" },
		{ game::MAG_085_THE_END_Actor_InterpolatePosition, (void *)te_78B860, "085 te_78B860" },
		{ game::MAG_085_THE_END_SpawnSubEffects, (void *)te_78C350, "085 te_78C350" },
		{ game::MAG_085_THE_END_Director_SpawnGroupEffects, (void *)te_78C3E0, "085 te_78C3E0" },
		{ game::MAG_085_THE_END_Tonberry_PrimModelActor_State3_CounterFinish, (void *)te_78C470, "085 te_78C470" },
		{ game::MAG_085_THE_END_GroundTask, (void *)te_78C4C0, "085 te_78C4C0" },
		{ game::MAG_085_THE_END_Ground_Draw, (void *)te_78C530, "085 te_78C530" },
		{ game::MAG_085_THE_END_Ground_Render, (void *)te_78C660, "085 te_78C660" },
		{ game::MAG_085_THE_END_Ground_DrawQuadsGT4, (void *)te_78C6D0, "085 te_78C6D0" },
		{ game::MAG_085_THE_END_Ground_State0_Setup, (void *)te_78CE00, "085 te_78CE00" },
		{ game::MAG_085_THE_END_Ground_State1_FadeWait, (void *)te_78CE30, "085 te_78CE30" },
		{ game::MAG_085_THE_END_PetalEmitterTask, (void *)te_78CE80, "085 te_78CE80" },
		{ game::MAG_085_THE_END_PetalEmitter_SpawnGroup, (void *)te_78CED0, "085 te_78CED0" },
		{ game::MAG_085_THE_END_PetalGroupTask, (void *)te_78CFA0, "085 te_78CFA0" },
		{ game::MAG_085_THE_END_PetalGroup_State0_Setup, (void *)te_78D150, "085 te_78D150" },
		{ game::MAG_085_THE_END_PetalGroup_State1_FadeOut, (void *)te_78D180, "085 te_78D180" },
		{ game::MAG_085_THE_END_PetalGroup_State2_Wait, (void *)te_78D1C0, "085 te_78D1C0" },
		{ game::MAG_085_THE_END_PetalGroup_State3_FadeIn, (void *)te_78D1F0, "085 te_78D1F0" },
		{ game::MAG_085_THE_END_ScreenGlowTask, (void *)te_78D250, "085 te_78D250" },
		{ game::MAG_085_THE_END_ScreenGlow_State0_Widen, (void *)te_78D2B0, "085 te_78D2B0" },
		{ game::MAG_085_THE_END_ScreenGlow_DrawQuad, (void *)te_78D2E0, "085 te_78D2E0" },
		{ game::MAG_085_THE_END_ScreenGlow_State1_Rise, (void *)te_78D3A0, "085 te_78D3A0" },
		{ game::MAG_085_THE_END_ScreenGlow_State2_WaitCut6, (void *)te_78D3D0, "085 te_78D3D0" },
		{ game::MAG_085_THE_END_Letterbox_State0_Init, (void *)te_78D480, "085 te_78D480" },
		{ game::MAG_085_THE_END_Letterbox_State1_Wait, (void *)te_78D4A0, "085 te_78D4A0" },
		{ game::MAG_085_THE_END_Letterbox_DrawBars, (void *)te_78D4D0, "085 te_78D4D0" },
		{ game::MAG_085_THE_END_Letterbox_State2_Hold, (void *)te_78D590, "085 te_78D590" },
		{ game::MAG_085_THE_END_Letterbox_State3_Shrink, (void *)te_78D5C0, "085 te_78D5C0" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag085_the_end()
	{
		act::register_module(85);
		for (const act::theend::ModPort *p = act::theend::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(85, p->addr, p->port, p->name);
		for (const act::theend::ModPort *p = act::theend::PORTS; p->addr; p++)
			act::register_module_port(85, p->addr, p->port, p->name);
	}
}

