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
#include <stddef.h>

namespace ff8fx
{
namespace act
{
namespace angelwing
{
	static const Mod MOD_084 = { "angelwing", 84, game::MAG_084_ANGEL_WING_FL, game::MAG_083_ABSORBED_INTO_TIME_FL,
		{ {}, {}, game::MAG_084_ANGEL_WING_pDirectorBlock, game::MAG_084_ANGEL_WING_pCameraBlock, game::unk_1575FD8, game::unk_25C24B4, {}, {}, {}, {}, {}, {}, game::MAG_084_ANGEL_WING_pTextureFile, {}, {}, {}, game::unk_25C24B8, {}, {}, {}, {}, {}, {}, {}, game::unk_25C8E20, {}, game::unk_25C90C0, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_084_ANGEL_WING_CurActorState },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_084_ANGEL_WING_MeshEmitF3, game::MAG_084_ANGEL_WING_MeshEmitF4, game::MAG_084_ANGEL_WING_RenderPolyFT3List, game::MAG_084_ANGEL_WING_RenderPolyFT4List, game::MAG_084_ANGEL_WING_PrimList_G3, game::MAG_084_ANGEL_WING_PrimList_G4, game::MAG_084_ANGEL_WING_PrimList_GT3, game::MAG_084_ANGEL_WING_PrimList_GT4_793360, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_084_ANGEL_WING_EmitterTask, game::MAG_084_ANGEL_WING_Emitter_State0_SpawnDirector, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_084_ANGEL_WING_DirStepGE, game::MAG_084_ANGEL_WING_Camera_StartKeyMove, game::MAG_084_ANGEL_WING_Camera_KeyStep, game::MAG_084_ANGEL_WING_Camera_WriteBattleCamera, {}, {}, {}, game::MAG_084_ANGEL_WING_DirSyncGate, game::MAG_084_ANGEL_WING_Nullsub_78E830, game::MAG_084_ANGEL_WING_DirPhaseGE, {}, {}, {}, {}, {}, {}, game::MAG_084_ANGEL_WING_ResetTrailHistory, game::MAG_084_ANGEL_WING_Director_StartPositions, game::MAG_084_ANGEL_WING_FillTrailRows, game::MAG_084_ANGEL_WING_Director_Mode3Setup, game::MAG_084_ANGEL_WING_Director_TargetPositions, game::MAG_084_ANGEL_WING_RelocateActorData, {}, {}, game::MAG_084_ANGEL_WING_DampValue, game::MAG_084_ANGEL_WING_DampVector, {}, {}, game::MAG_084_ANGEL_WING_MorphVertexBlock, {}, {}, {}, game::MAG_084_ANGEL_WING_RandRange, game::MAG_084_ANGEL_WING_RandRangeWide, game::MAG_084_ANGEL_WING_RandRange2, game::MAG_084_ANGEL_WING_Particle_AttachToOwner, game::MAG_084_ANGEL_WING_Actor_InterpolatePosition, {}, game::MAG_084_ANGEL_WING_ParticleList_Unlink, game::MAG_084_ANGEL_WING_Particle_PlaceAtAnchorSet, game::MAG_084_ANGEL_WING_Particle_PlaceAtPoint, {}, {}, game::MAG_084_ANGEL_WING_Emitter_State1_ApplyDamage, game::MAG_084_ANGEL_WING_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_084_ANGEL_WING_Emitter_State3_FinishReleaseRoot, game::MAG_084_ANGEL_WING_Nullsub_7975C0, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id DIR_PTR = game::MAG_084_ANGEL_WING_pDirectorBlock;
	static constexpr game::Id PHASE_DATA = DIR_PTR;
	static constexpr game::Id SPRITE_CURSOR = game::MAG_084_ANGEL_WING_PacketCursor;
	static constexpr game::Id PACKET_CURSOR = SPRITE_CURSOR;
	static constexpr game::Id FA_CURSOR = game::BATTLE_PACKET_WRITE_CURSOR;
	static constexpr game::Id SCRATCH_SP = game::MAG_084_ANGEL_WING_ScratchSP;
	static constexpr game::Id ACTOR_STATE = game::MAG_084_ANGEL_WING_CurActorState;

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
		static inline uint32_t BS_Camera_ArmReturnAfterEffect() { return fn<uint32_t (__cdecl *)()>(game_addr(game::BS_Camera_ArmReturnAfterEffect))(); }
		static inline uint32_t BS_BindEntityModelFromComData(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(a1, a2, a3); }
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

	static inline uint32_t t1_data() { return MEM<uint32_t>(G(G_1533010)); }
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

	static constexpr game::Id REAPER_CTX = game::MAG_084_ANGEL_WING_RevealDrawContext;
	static inline uint32_t ENT(uint32_t slot) { return game_addr(game::BattleEntitySlotData) + slot * 0x9C; }
	static inline void ak_cam_chain(uint32_t a1, uint32_t script, uint32_t from)
	{
		a_73AB20(script, from, 0);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
	}

	uint32_t __cdecl te_78D940(uint32_t a1);
	uint32_t __cdecl te_78DAF0(uint32_t a1);
	uint32_t __cdecl te_78DB30();
	uint32_t __cdecl te_78DC20(uint32_t a1);
	uint32_t __cdecl te_78DC50(uint32_t a1);
	uint32_t __cdecl te_78DCE0(uint32_t a1);
	uint32_t __cdecl te_78DDB0(uint32_t a1);
	uint32_t __cdecl te_78DE00(uint32_t a1);
	uint32_t __cdecl te_78DE40(uint32_t a1);
	uint32_t __cdecl te_78DED0(uint32_t a1);
	uint32_t __cdecl te_78DF00(uint32_t a1);
	uint32_t __cdecl te_78E700(uint32_t a1);
	uint32_t __cdecl te_78E730(uint32_t a1);
	uint32_t __cdecl te_78E760(uint32_t a1);
	uint32_t __cdecl te_78E790(uint32_t a1);
	uint32_t __cdecl te_78E7F0(uint32_t a1);
	uint32_t __cdecl te_78E8A0(uint32_t a1);
	uint32_t __cdecl te_78E8F0(uint32_t a1);
	uint32_t __cdecl te_78E940(uint32_t a1);
	uint32_t __cdecl te_78E9A0(uint32_t a1);
	uint32_t __cdecl te_78E9E0(uint32_t a1);
	uint32_t __cdecl te_78EA00(uint32_t a1);
	uint32_t __cdecl te_78EA20(uint32_t a1);
	uint32_t __cdecl te_78EA40(uint32_t a1);
	uint32_t __cdecl te_78EA70(uint32_t a1);
	uint32_t __cdecl te_78EAC0(uint32_t a1);
	uint32_t __cdecl te_78EAE0(uint32_t a1);
	uint32_t __cdecl te_78EB50(uint32_t a1);
	uint32_t __cdecl te_78EC80(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl te_78ED30(uint32_t a1);
	uint32_t __cdecl te_78EDB0(uint32_t a1);
	uint32_t __cdecl te_78EEE0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl te_78F1B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_78F2E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_78F500(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_78F790(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_78FC10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_7906C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_790BC0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_791210(uint32_t a1);
	uint32_t __cdecl te_7912C0(uint32_t a1);
	uint32_t __cdecl te_791350(uint32_t a1);
	uint32_t __cdecl te_791420(uint32_t a1);
	uint32_t __cdecl te_791600(uint32_t a1);
	uint32_t __cdecl te_791880(void);
	uint32_t __cdecl te_791F30(uint32_t a1);
	uint32_t __cdecl te_791FC0(void);
	uint32_t __cdecl te_792020(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_7922A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_7924B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_792C10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_792E40(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_793100(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_793360(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_793640(void);
	uint32_t __cdecl te_793700(uint32_t a1);
	uint32_t __cdecl te_793780(void);
	uint32_t __cdecl te_7937C0(uint32_t a1);
	uint32_t __cdecl te_7947F0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_794900(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_794980(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_7949B0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl te_794A50(uint32_t a1);
	uint32_t __cdecl te_794AD0(uint32_t a1);
	uint32_t __cdecl te_794B50(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl te_795380(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_7955A0(uint32_t a1);
	uint32_t __cdecl te_796090(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_796120(void);
	uint32_t __cdecl te_7961B0(uint32_t a1);
	uint32_t __cdecl te_7961E0(uint32_t a1);
	uint32_t __cdecl te_7962F0(uint32_t a1);
	uint32_t __cdecl te_796360(uint32_t a1);
	uint32_t __cdecl te_796390(uint32_t a1);
	uint32_t __cdecl te_796620(uint32_t a1);
	uint32_t __cdecl te_796720(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_796BF0(uint32_t a1);
	uint32_t __cdecl te_796C70(uint32_t a1);
	uint32_t __cdecl te_796CA0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl te_796D00(uint32_t a1);
	uint32_t __cdecl te_796DB0(uint32_t a1);
	uint32_t __cdecl te_796DD0(uint32_t a1);
	uint32_t __cdecl te_796E30(uint32_t a1, uint32_t a2);
	uint32_t __cdecl te_796F50(uint32_t a1);
	uint32_t __cdecl te_796FD0(uint32_t a1);
	uint32_t __cdecl te_797080(uint32_t a1);
	uint32_t __cdecl te_797190(uint32_t a1);
	uint32_t __cdecl te_7971B0(uint32_t a1);
	uint32_t __cdecl te_7971F0(uint32_t a1);
	uint32_t __cdecl te_797220(uint32_t a1);
	uint32_t __cdecl te_797250(uint32_t a1);
	uint32_t __cdecl te_797270(uint32_t a1);
	uint32_t __cdecl te_7972B0(uint32_t a1);
	uint32_t __cdecl te_797330(uint32_t a1);
	uint32_t __cdecl te_797410(uint32_t a1);
	uint32_t __cdecl te_797430(uint32_t a1);
	uint32_t __cdecl te_797490(uint32_t a1);
	uint32_t __cdecl te_7974C0(uint32_t a1);
	uint32_t __cdecl te_7974E0(uint32_t a1);
	uint32_t __cdecl te_797500(uint32_t a1);
	uint32_t __cdecl te_797540(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace angelwing
{
	uint32_t __cdecl te_78D940(uint32_t a1)
	{
		g_mod = &MOD_084;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_ScratchSP)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_25C24B8)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_084_ANGEL_WING_Sequence_State0_Advance_78DAD0);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_084_ANGEL_WING_Sequence_State0_Advance_78DAE0);
		states[2] = game_addr(game::MAG_084_ANGEL_WING_Master_State2_CarvePools);
		states[3] = game_addr(game::MAG_084_ANGEL_WING_Sequence_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_084_ANGEL_WING_Sequence_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_084_ANGEL_WING_Sequence_State0_Advance_797610);
		states[6] = game_addr(game::MAG_084_ANGEL_WING_Sequence_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_084_ANGEL_WING_Sequence_State7_RequestBattleSync);
		states[8] = game_addr(game::MAG_084_ANGEL_WING_Sequence_State8_WaitBattleSyncDone);
		states[9] = game_addr(game::MAG_084_ANGEL_WING_Sequence_State9_Finish);
		states[10] = game_addr(game::MAG_084_ANGEL_WING_Nullsub_797690);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_25C9234));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_25C838C));
			MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_PacketCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_25C24B4)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_25C9230));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_25C8388));
			MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_PacketCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_25C24B4)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_25C9F00)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25C9EE8)) = 0;
		const uint32_t queues[8] = { game_addr(game::unk_25C90C0), game_addr(game::unk_25C2490), game_addr(game::unk_25C9ED8), game_addr(game::unk_25C8E20), game_addr(game::unk_25C2598), game_addr(game::unk_25C3938), game_addr(game::unk_25C8378), game_addr(game::unk_25C90B0) };
		for (int i = 0; i < 8; i++)
		{
			const uint32_t n = x::ExecuteTaskQueue(queues[i]);
			U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)n);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl te_78DAF0(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_FileArenaCursor));
		MEM<uint32_t>(game_addr(game::unk_25C9EEC)) = p;
		p += 0x1B0;
		MEM<uint32_t>(game_addr(game::unk_25C9F30)) = p;
		p += 0x6900;
		MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_FileArenaCursor)) = p;
		te_78DB30();
		next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_78DB30()
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::unk_25C9EEC)), 0x1B0);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::unk_25C9F30)), 0x6900);
		MEM<uint16_t>(game_addr(game::unk_25C3930)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25C9EF8)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25C9D24)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25C2358)) = 0;
		return 0;
	}

	uint32_t __cdecl te_78DC20(uint32_t a1)
	{
		te_78DC50(a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25C2490), game_addr(game::MAG_084_ANGEL_WING_DirectorTask), 0x48, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_78DC50(uint32_t a1)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_pDirectorBlock)), 0x54);
		const uint32_t d = MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_pDirectorBlock));
		const uint32_t tgt = ENT(U8(a1, 0x2D));
		U32(d, 8) = U32(tgt, 0x1C);
		U32(d, 0xC) = U32(tgt, 0x20);
		const uint32_t cst = ENT(U8(a1, 0x2C));
		U32(d, 0) = U32(cst, 0x1C);
		U32(d, 4) = U32(cst, 0x20);
		const int16_t dz = (int16_t)(U16(d, 0xC) - U16(d, 4));
		const int16_t dx = (int16_t)(U16(d, 8) - U16(d, 0));
		const uint32_t ang = x::CartesianToGameAngle((uint32_t)(int32_t)dx, (uint32_t)(int32_t)dz);
		U16(MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_pDirectorBlock)), 0x4A) = (uint16_t)((ang - 0x800) & 0xFFF);
		return 0;
	}

	uint32_t __cdecl te_78DCE0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[16];
		states[0] = game_addr(game::MAG_084_ANGEL_WING_Director_State0_UploadTIM_SpawnCameraTask);
		states[1] = game_addr(game::MAG_084_ANGEL_WING_Director_State1_WaitDirGate1);
		states[2] = game_addr(game::MAG_084_ANGEL_WING_Director_State2_HideEntitiesAtCut1);
		states[3] = game_addr(game::MAG_084_ANGEL_WING_Director_State3_WaitFrame36);
		states[4] = game_addr(game::MAG_084_ANGEL_WING_Director_State4_WaitDirGate2);
		states[5] = game_addr(game::MAG_084_ANGEL_WING_Director_State5_WaitDirPhase2);
		states[6] = game_addr(game::MAG_084_ANGEL_WING_Director_State6_WaitFrame19);
		states[7] = game_addr(game::MAG_084_ANGEL_WING_Director_State7_WaitDirGate3);
		states[8] = game_addr(game::MAG_084_ANGEL_WING_Director_State8_WaitCut3);
		states[9] = game_addr(game::MAG_084_ANGEL_WING_Director_State9_WaitFrame41);
		states[10] = game_addr(game::MAG_084_ANGEL_WING_Director_State10_RequestCut4);
		states[11] = game_addr(game::MAG_084_ANGEL_WING_Director_State11_ShowEntitiesAtCut4);
		states[12] = game_addr(game::MAG_084_ANGEL_WING_Director_State12_WaitFrame3C);
		states[13] = game_addr(game::MAG_084_ANGEL_WING_Director_State13_DoneAtCut5);
		states[14] = game_addr(game::MAG_084_ANGEL_WING_Director_State14_Finish);
		states[15] = game_addr(game::MAG_084_ANGEL_WING_Nullsub_78EB30);
		te_78DDB0(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl te_78DDB0(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			te_78EB50(a1);
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

	uint32_t __cdecl te_78DE00(uint32_t a1)
	{
		x::Battle_QueueTIMUpload_GetEOF(game_addr(game::MAG084_AngelWings_TIM));
		gx::BS_Camera_ArmReturnAfterEffect();
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25C2598), game_addr(game::MAG_084_ANGEL_WING_CameraTask), 0x64, a1);
		next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_78DE40(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[10];
		states[0] = game_addr(game::MAG_084_ANGEL_WING_Camera_State0_ResetBlock);
		states[1] = game_addr(game::MAG_084_ANGEL_WING_Camera_State1_StartScriptAtCut1);
		states[2] = game_addr(game::MAG_084_ANGEL_WING_Camera_State2_ScriptChain_1575E28);
		states[3] = game_addr(game::MAG_084_ANGEL_WING_Camera_State3_StepNextAtCue2);
		states[4] = game_addr(game::MAG_084_ANGEL_WING_Camera_State4_StepNewPathAtCut3);
		states[5] = game_addr(game::MAG_084_ANGEL_WING_Camera_State5_ScriptChain_1575F90);
		states[6] = game_addr(game::MAG_084_ANGEL_WING_Camera_State6_KeyStepThenKeyAtStep4);
		states[7] = game_addr(game::MAG_084_ANGEL_WING_Camera_State7_StepUntilCut5);
		states[8] = game_addr(game::MAG_084_ANGEL_WING_Camera_State8_FinishNode);
		states[9] = game_addr(game::MAG_084_ANGEL_WING_Nullsub_78E840);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl te_78DED0(uint32_t a1)
	{
		a_73AB00();
		t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_78DF00(uint32_t a1)
	{
		if (a_73AAE0(1) != 0)
		{
			a_73AB20(game_addr(game::unk_1575DE0), MEM<uint32_t>(game_addr(DIR_PTR)), 0);
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl te_78E700(uint32_t a1)
	{
		if (a_73AE10() == 1)
			ak_cam_chain(a1, game_addr(game::unk_1575E28), MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_pDirectorBlock)));
		return 0;
	}

	uint32_t __cdecl te_78E730(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(2) != 0)
		{
			a_73AB20(game_addr(game::unk_1575E70), MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_pDirectorBlock)), 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl te_78E760(uint32_t a1)
	{
		a_73AE10();
		uint32_t r = a_73AAE0(3);
		if (r == 0) return 0;
		uint32_t blk = MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_pDirectorBlock));
		a_73AB20(game_addr(game::unk_1575F20), blk, 0);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl te_78E790(uint32_t a1)
	{
		if (a_73AE10() == 1)
			ak_cam_chain(a1, game_addr(game::unk_1575F90), MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_pDirectorBlock)));
		return 0;
	}

	uint32_t __cdecl te_78E7F0(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(5) != 0)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_78E8A0(uint32_t a1)
	{
		if (a_73B7E0(1) != 0)
		{
			te_78E8F0((uint32_t)U8(a1, 0x2C));
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl te_78E8F0(uint32_t a1)
	{
		const int32_t keep = (int16_t)(uint16_t)a1;
		uint32_t saved = MEM<uint32_t>(game_addr(DIR_PTR)) + 0x20;
		int32_t i = 0;
		for (uint32_t e = game_addr(game::BattleEntitySlotData); e < game_addr(game::CAMERA_FLAG_RELATED); e += 0x9C, i++, saved += 4)
		{
			if (i == keep)
				continue;
			const uint16_t w = U16(e, 0);
			if ((w & 2) == 0)
				continue;
			U32(saved, 0) = (uint32_t)w;
			U16(e, 0) = (uint16_t)(w | 4);
		}
		return 0;
	}

	uint32_t __cdecl te_78E940(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_pDirectorBlock)), 0x46) >= 0x2D)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_78E9A0(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x46) >= 0x19)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_78E9E0(uint32_t a1)
	{
		if (a_73B7E0(3) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_78EA00(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x46) >= 0x41)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_78EA20(uint32_t a1)
	{
		uint32_t r = a_73B640(4);
		if (r == 0) return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl te_78EA40(uint32_t a1)
	{
		if (a_73B7E0(4) != 0)
		{
			te_78EA70((uint32_t)U8(a1, 0x2C));
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl te_78EA70(uint32_t a1)
	{
		const int32_t keep = (int16_t)(uint16_t)a1;
		uint32_t saved = MEM<uint32_t>(game_addr(DIR_PTR)) + 0x20;
		int32_t i = 0;
		for (uint32_t e = game_addr(game::BattleEntitySlotData); e < game_addr(game::CAMERA_FLAG_RELATED); e += 0x9C, i++, saved += 4)
		{
			if (i == keep)
				continue;
			const uint16_t w = U16(e, 0);
			if ((w & 2) == 0)
				continue;
			U16(e, 0) = (uint16_t)(w ^ ((uint16_t)(U8(saved, 0) ^ (uint8_t)w) & 4));
		}
		return 0;
	}

	uint32_t __cdecl te_78EAC0(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_pDirectorBlock)), 0x46) >= 0x3C)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl te_78EAE0(uint32_t a1)
	{
		if (a_73B640(5) != 0)
		{
			U16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x48) = 1;
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl te_78EB50(uint32_t a1)
	{
		const int32_t cut = S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x40);
		if (cut == 1)
		{
			const uint32_t n = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25C9ED8), game_addr(game::MAG_084_ANGEL_WING_ModelContainerTask), 0x154, a1);
			x::Effect_BindModelContainerSetAnim(n, game_addr(game::MAG_084_ANGEL_WING_ModelContainerData), 3);
			te_78EC80(a1, game_addr(game::MAG_084_ANGEL_WING_PlayA_Task), game_addr(game::unk_171F2B0), 0x4A0, 0, 0);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25C3938), game_addr(game::MAG_084_ANGEL_WING_GlowA_Task), 0x58, a1);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25C3938), game_addr(game::MAG_084_ANGEL_WING_GlowB_Task), 0x58, a1);
		}
		else if (cut == 2)
			a_73C100(a1, game_addr(game::MAG_084_ANGEL_WING_FeatherActorSystemTask), game_addr(game::unk_171DE1C), 8, 0xF, 0);
		else if (cut == 3)
		{
			te_78EC80(a1, game_addr(game::MAG_084_ANGEL_WING_PlayB_Task), game_addr(game::unk_17217A8), 0x788, 2, 0);
			a_73C100(a1, game_addr(game::MAG_084_ANGEL_WING_FeatherActorSystemTask), game_addr(game::unk_171DE1C), 8, 0xF, 0);
		}
		return 0;
	}

	uint32_t __cdecl te_78EC80(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t n = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25C8378), a2, 0x830, a1);
		U16(n, 0x80) = (uint16_t)a5;
		U32(n, 0x74) = a3;
		S32(n, 0x78) = (int16_t)(uint16_t)a4;
		U16(n, 0x82) = (uint16_t)a6;
		return n;
	}

	uint32_t __cdecl te_78ED30(uint32_t a1)
	{
		if (S16(a1, 0x24) < 0x18)
			return 0;
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2C) * 0x9C;
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		U32(a1, 0x1C) = U32(ent, 0x1C);
		U32(a1, 0x20) = U32(ent, 0x20);
		U16(a1, 0x1E) = (uint16_t)(U16(a1, 0x1E) + 0xFC00);
		U16(a1, 0x20) = (uint16_t)(U16(a1, 0x20) + 0x200);
		S32(a1, 0x58) = 0x1000;
		S32(a1, 0x54) = 0x1000;
		S32(a1, 0x50) = 0x1000;
		U16(a1, 0x88) = 0xFE00;
		te_78EDB0(a1);
		next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_78EDB0(uint32_t a1)
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
		U32(L, 0x48) = MEM<uint32_t>(game_addr(game::unk_25C9EFC));
		U32(L, 0x44) = U32(node, 0x7C);
		U16(L, 0x4C) = U16(node, 0x8C);
		U32(L, 0x38) = U32(node, 0x70);
		U16(L, 0x52) = U16(node, 0x92);
		U16(L, 0x4E) = U16(node, 0x8E);
		U16(L, 0x50) = U16(node, 0x90);
		U16(L, 0x56) = U16(node, 0x80);
		U16(L, 0x58) = U16(node, 0x88);
		return prim_play(node + 0x94, game_addr(game::MAG_084_ANGEL_WING_PrimObjectCallback), L, 0);
	}

	uint32_t __cdecl te_78EEE0(uint32_t a1, uint32_t a2, uint32_t a3)
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

		if ((U32(rec, 4) & 0x40000) != 0)
			gx::MAG_063_sub_7015B0(rec + 0x10, M);
		else
			x::MAG_017_sub_701310(rec + 0x10, M);
		const uint16_t vx = U16(rec, 8);
		const uint16_t vy = U16(rec, 0xA);
		const uint16_t vz = U16(rec, 0xC);
		U16(V, 0) = vx;
		U16(V, 2) = vy;
		U16(V, 4) = vz;
		x::GTE_SetRotMatrix(blk);
		x::GTE_LoadV0(V);
		x::GTE_MVMVA_RotV0();
		x::GTE_ReadMAC123(M + 0x14);
		x::GTE_MatrixMultiply(blk, M);
		const int32_t t2 = S32(M, 0x1C), t1 = S32(M, 0x18), t0 = S32(M, 0x14);
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
		const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		if (S16(blk, 0x56) == 2 && S16(rec, 2) == 1)
			S32(hdr, 0x10) = S16(blk, 0x58) - 0x7000;
		else
			S32(hdr, 0x10) = S16(blk, 0x58);
		U16(hdr, 0x22) = 0x100;
		U16(hdr, 0x20) = 0x100;
		U16(hdr, 0x2A) = 0x100;
		U16(hdr, 0x28) = 0x100;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = te_78F1B0(hdr, ot, 2, cursor);
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl te_78F1B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
			cur = te_78F2E0(ctx, a2, a3, a4);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = te_78F500(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = te_78F790(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = te_78FC10(ctx, a2, a3, cur);
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
			cur = te_7906C0(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			return te_790BC0(ctx, a2, a3, cur);
		U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		return cur;
	}

	uint32_t __cdecl te_78F2E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_78F500(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_78F790(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_78FC10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_7906C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_790BC0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_791210(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_73B7E0(3) != 0)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(st + 1);
			return node;
		}
		const uint32_t r = te_78EDB0(node);
		if (r == 0)
		{
			const uint8_t st = (uint8_t)(U8(node, 0x29) + 1);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = st;
			return st;
		}
		return r;
	}

	uint32_t __cdecl te_7912C0(uint32_t a1)
	{
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2C) * 0x9C;
		a_746A10();
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		U32(a1, 0x1C) = U32(ent, 0x1C);
		U32(a1, 0x20) = U32(ent, 0x20);
		S32(a1, 0x58) = 0x1000;
		S32(a1, 0x54) = 0x1000;
		S32(a1, 0x50) = 0x1000;
		U16(a1, 0x60) = 0xFF80;
		te_78EDB0(a1);
		next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_791350(uint32_t a1)
	{
		if (a_73B7E0(4) != 0)
		{
			a_746AE0();
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			next_state(a1);
			return 0;
		}
		te_78EDB0(a1);
		return 0;
	}

	uint32_t __cdecl te_791420(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			a_73F990(a1);
			te_791F30(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl te_791600(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_CurActorState));
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

	uint32_t __cdecl te_791880(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_CurActorState));
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

	uint32_t __cdecl te_791F30(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::MAG_084_ANGEL_WING_CurActorState), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			te_796120();
			te_793780();
			te_793640();
			te_791FC0();
			sys = U32(game_addr(game::MAG_084_ANGEL_WING_CurActorState), 0);
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
		U16(game_addr(game::unk_25C9F00), 0) = (uint16_t)(U16(game_addr(game::unk_25C9F00), 0) + c14);
		U16(game_addr(game::unk_25C9EE8), 0) = (uint16_t)(U16(game_addr(game::unk_25C9EE8), 0) + c16);
		return done;
	}

	uint32_t __cdecl te_791FC0(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_084_ANGEL_WING_CurActorState), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::MAG_084_ANGEL_WING_CurActorState), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					te_792020(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl te_792020(uint32_t a1, uint32_t a2)
	{
		uint32_t ctx = MEM<uint32_t>(game_addr(SCRATCH_SP)) - 0x58;
		const bool def3a = U16(a2, 0x3A) != 0;
		const uint32_t model = U32(a1, 0x170);
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = ctx;
		const uint32_t saved_ctx = ctx;
		U32(ctx, 0) = model;
		U32(ctx, 0x1C) = 0;
		if (!def3a)
			U32(ctx, 0x1C) = 0x30;
		const uint16_t col = U16(a1, 0x1CE);
		if (col != 0)
		{
			U32(ctx, 8) = U32(a1, 0x16C);
			const uint32_t fl = U32(ctx, 0x1C);
			U32(ctx, 0xC) = (uint32_t)(int32_t)(int16_t)col;
			U32(ctx, 0x1C) = fl | 0xC0;
		}
		const int8_t copies = S8(a1, 0x1D8);
		if (copies == 1)
		{
			const uint32_t m = a1 + 0xAC;
			x::GTE_SetRotMatrix(m);
			x::GTE_SetTransVector(m);
			const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
			const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = a_7355E0(ctx, ot, 2, cursor);
		}
		else if (copies > 0)
		{
			const uint32_t m = a1 + 0xAC;
			uint32_t pos = a1 + 0x19E;
			int32_t i = 0;
			int32_t n;
			do
			{
				S32(a1, 0xC0) = S16(pos, -2);
				S32(a1, 0xC4) = S16(pos, 0);
				S32(a1, 0xC8) = S16(pos, 2);
				x::GTE_SetRotMatrix(m);
				x::GTE_SetTransVector(m);
				const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
				const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
				const uint32_t r = a_7355E0(saved_ctx, ot, 2, cursor);
				n = S8(a1, 0x1D8);
				i++;
				pos += 8;
				MEM<uint32_t>(game_addr(PACKET_CURSOR)) = r;
			} while (i < n);
		}
		const uint32_t top = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 0x58;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = top;
		return top;
	}

	uint32_t __cdecl te_7922A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_7924B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_792C10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_792E40(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_793100(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_793360(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_793640(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_CurActorState)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_CurActorState));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						te_793700(act);
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
							te_793700(act);
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

	uint32_t __cdecl te_793700(uint32_t a1)
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

	uint32_t __cdecl te_793780(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_084_ANGEL_WING_CurActorState), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				te_794A50(act);
			else if (type == 1)
				te_7937C0(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl te_7937C0(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_CurActorState));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (te_794900(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_CurActorState));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl te_7947F0(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::MAG_084_ANGEL_WING_CurActorState), 0);
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

	uint32_t __cdecl te_794900(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				te_794980(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			te_794980(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			te_794980(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl te_794980(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return te_7949B0(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl te_7949B0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::unk_25C9EEC));
		int32_t idx = MEM<int16_t>(game_addr(game::unk_25C3930));
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
			if (idx >= 3)
				idx = 0;
			tries++;
		} while (tries < 4);
		idx++;
		if (idx < 3)
			MEM<uint16_t>(game_addr(game::unk_25C3930)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::unk_25C3930)) = 0;
		return slot;
	}

	uint32_t __cdecl te_794A50(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_084_ANGEL_WING_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_084_ANGEL_WING_Actor_InterpolatePosition), a1);
		te_794AD0(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl te_794AD0(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::MAG_084_ANGEL_WING_CurActorState), 0);
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
				te_794B50(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			te_794B50(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl te_794B50(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(SCRATCH_SP), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(SCRATCH_SP), 0) = sp;
		uint32_t node = te_795380(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(ACTOR_STATE), 0);
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
				alignas(4) uint8_t pt[8] = {};
				const uint32_t mdl = U32(MEM<uint32_t>(game_addr(DIR_PTR)), 0x18) + 0x30;
				const int32_t r = (int32_t)x::CrtRand();
				x::GetEffectSpawnPosition(mdl, (uint32_t)(int32_t)MEM<int16_t>(game_addr(game::MAG_084_ANGEL_WING_FeatherBoneTable) + (uint32_t)(r % 14) * 2), 0, P(pt));
				S32(a1, 0x4C) = shl32(S16(P(pt), 0), 16);
				S32(a1, 0x50) = shl32(S16(P(pt), 2), 16);
				S32(a1, 0x54) = shl32(S16(P(pt), 4), 16);
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
		te_796090(node, desc);
		U32(game_addr(SCRATCH_SP), 0) = U32(game_addr(SCRATCH_SP), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl te_795380(uint32_t a1, uint32_t a2)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::unk_25C9F30));
		int32_t i = MEM<int16_t>(game_addr(game::unk_25C9EF8));
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
			if (i >= 0x27)
				i = 0;
			tries++;
			if (tries >= 0x28)
				break;
		}
		i++;
		if (i >= 0x27)
			MEM<uint16_t>(game_addr(game::unk_25C9EF8)) = 0;
		else
			MEM<uint16_t>(game_addr(game::unk_25C9EF8)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl te_7955A0(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::MAG_084_ANGEL_WING_CurActorState), 0);
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
				uint32_t g = U32(game_addr(game::MAG_084_ANGEL_WING_CurActorState), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::MAG_084_ANGEL_WING_CurActorState), 0);
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

	uint32_t __cdecl te_796090(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			te_7949B0(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			te_7949B0(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			te_7949B0(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			te_7949B0(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl te_796120(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_CurActorState));
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
					te_7949B0(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_CurActorState));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				te_7949B0(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_CurActorState));
			}
		}
		return 0;
	}

	uint32_t __cdecl te_7961B0(uint32_t a1)
	{
		if (te_791F30(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl te_7961E0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[6];
		states[0] = game_addr(game::MAG_084_ANGEL_WING_ModelContainer_State0_SetupSwapCasterModel);
		states[1] = game_addr(game::MAG_084_ANGEL_WING_ModelContainer_State1_RevealGrow);
		states[2] = game_addr(game::MAG_084_ANGEL_WING_ModelContainer_State2_WaitAnimDone);
		states[3] = game_addr(game::MAG_084_ANGEL_WING_ModelContainer_State3_WaitCut4);
		states[4] = game_addr(game::MAG_084_ANGEL_WING_ModelContainer_State4_FadeOut);
		states[5] = game_addr(game::MAG_084_ANGEL_WING_Nullsub_796FC0);
		callp(states[S8(node, 0x29)], node);
		if ((U8(node, 0x26) & 4) == 0)
			te_796390(node);
		const uint16_t t = U16(node, 0x24);
		if (t == 0x32 || t == 0x64)
			te_78EC80(node, game_addr(game::MAG_084_ANGEL_WING_PlayC_Task), game_addr(game::unk_1720570), 0x158, 1, 0);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl te_7962F0(uint32_t a1)
	{
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2C) * 0x9C;
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		U32(a1, 0x1C) = U32(ent, 0x1C);
		U32(a1, 0x20) = U32(ent, 0x20);
		U16(a1, 0x1E) = 0;
		S32(a1, 0x58) = 0x1000;
		S32(a1, 0x54) = 0x1000;
		S32(a1, 0x50) = 0x1000;
		te_78EDB0(a1);
		next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_796360(uint32_t a1)
	{
		const uint32_t r = te_78EDB0(a1);
		if (r == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl te_796390(uint32_t a1)
	{
		const uint16_t fl = U16(a1, 0x26);
		if ((fl & 4) != 0)
			return 0;
		if ((fl & 8) != 0)
		{
			alignas(4) uint8_t pt[8] = {};
			U32(MEM<uint32_t>(game_addr(DIR_PTR)), 0x18) = a1;
			const uint32_t mdl = a1 + 0x30;
			const uint32_t m = mdl + 0x40;
			x::ComposeZYXRotationMatrix(mdl + 0xC, m);
			S32(mdl, 0x54) = S16(mdl, 0x1C);
			S32(mdl, 0x58) = S16(mdl, 0x1E);
			S32(mdl, 0x5C) = S16(mdl, 0x20);
			x::scale3DMatrix(m, a1 + 0x114);
			x::GetEffectSpawnPosition(mdl, 0xF1, 0, P(pt));
			MEM<uint16_t>(game_addr(game::MAG_084_ANGEL_WING_RevealHeight)) = U16(P(pt), 2);
			te_796620(game_addr(game::MAG_084_ANGEL_WING_RevealDrawContext));
			return 0;
		}
		MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_PacketCursor)) = a_746C10(a1, game_addr(game::unk_25C9238), MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_PacketCursor)));
		return 0;
	}

	uint32_t __cdecl te_796620(uint32_t a1)
	{
		const uint32_t ctx = a1;
		const uint32_t blk = U32(ctx, 0x48);
		ff8fx::copy<(32)>((void *)ctx, (const void *)(blk + 0x40));
		const uint32_t anim = blk + 0x60;
		x::BattleModel_BuildBoneMatricesFromPose(anim);
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
		const uint32_t shadow = x::sub_5088A0(blk, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4040, 0x10, MEM<uint32_t>(U32(ctx, 0x4C)));
		MEM<uint32_t>(U32(ctx, 0x4C)) = shadow;
		te_796720(U32(blk, 0x64), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, ctx);
		const uint32_t second = U32(blk, 0x78);
		if (second != 0)
			te_796720(U32(second, 4), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, ctx);
		x::BattleModel_BuildBoneMatricesFromPose(anim);
		return 0;
	}

	uint32_t __cdecl te_796720(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		(void)a3;
		const uint32_t ctx = a4;
		const uint32_t blk = U32(ctx, 0x48);
		uint32_t pkt = MEM<uint32_t>(U32(ctx, 0x4C));
		const uint32_t vbuf = U32(U32(ctx, 0x44), 4);
		uint32_t table = U32(a1, 4);
		const uint32_t bones = U32(a1, 0) + 0x10;
		const int32_t count = S32(table, 0);
		const uint32_t hdr = x::Field_Alloc(0x74);
		table += 4;
		const uint32_t colour = U32(blk, 0x28) & 0xFFFFFF;
		U32(hdr, 0x50) = U32(blk, 0x7C);
		U32(hdr, 0x48) = colour | 0x24000000;
		U32(hdr, 0x4C) = colour | 0x2C000000;
		U16(hdr, 0x70) = U16(ctx, 0x54);
		U16(hdr, 0x72) = U16(ctx, 0x56);
		x::GTE_SetRotMatrix(ctx + 0x20);
		x::GTE_SetTransVector(ctx + 0x20);
		for (int32_t i = 0; i < count; i++)
		{
			const uint32_t off = U32(table, 0);
			table += 4;
			uint32_t obj = U32(a1, 4) + off;
			if (((U32(hdr, 0x50) >> (i & 31)) & 1) == 0)
				continue;
			uint32_t v = vbuf;
			int32_t groups = S16(obj, 0);
			obj += 2;
			for (; groups > 0; groups--)
			{
				const int32_t bone = S16(obj, 0);
				obj += 2;
				const uint32_t m = bones + (uint32_t)(bone * 0x30) + 0x10;
				gx::GTE_SetLightMatrix(m);
				gx::GTE_SetBackgroundVector(U32(m, 0x14), U32(m, 0x18), U32(m, 0x1C));
				int32_t nv = S16(obj, 0);
				obj += 2;
				for (; nv > 0; nv--)
				{
					U16(hdr, 0x28) = U16(obj, 0);
					U16(hdr, 0x2A) = U16(obj, 2);
					U16(hdr, 0x2C) = U16(obj, 4);
					obj += 6;
					x::GTE_LoadV0(hdr + 0x28);
					gx::GTE_MVMVA_LightV0_Bk();
					x::GTE_StoreIR123(hdr + 0x28);
					const int32_t dx = S16(hdr, 0x28);
					const int32_t dy = sub32(S16(hdr, 0x2A), S16(hdr, 0x72));
					S32(hdr, 0x54) = dx;
					S32(hdr, 0x58) = dy;
					S32(hdr, 0x5C) = 0;
					const int32_t sq = add32(mul32(dx, dx), mul32(dy, dy));
					S32(hdr, 0x64) = sq;
					const int32_t dist = (int32_t)x::Sqrt((uint32_t)sq);
					S32(hdr, 0x64) = dist;
					const int32_t radius = S16(hdr, 0x70);
					if (dist <= radius - 0x80)
						S32(v, 8) = 0x80;
					else if (dist <= radius)
						S32(v, 8) = radius - dist;
					else
						S32(v, 8) = 0;
					S32(v, 8) = div3(shl32(S32(v, 8), 1));
					U32(v, 0) = U32(hdr, 0x28);
					U32(v, 4) = U32(hdr, 0x2C);
					v += 0xC;
				}
			}
			obj = (obj + 3) & ~3u;
			const int32_t n3 = S16(obj, 0);
			const int32_t n4 = S16(obj, 2);
			uint32_t rec = obj + 0xC;
			for (int32_t k = n3; k > 0; k--, rec += 0x10)
			{
				const uint32_t i2 = U16(rec, 4) & 0xFFF, i1 = U16(rec, 2) & 0xFFF, i0 = U16(rec, 0) & 0xFFF;
				U32(hdr, 0x40) = i2;
				U32(hdr, 0x3C) = i1;
				U32(hdr, 0x38) = i0;
				x::GTE_LoadV012(vbuf + i0 * 0xC, vbuf + i1 * 0xC, vbuf + i2 * 0xC);
				x::GTE_RTPT();
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(hdr + 0x68);
				if (S32(hdr, 0x68) <= 0)
					continue;
				U32(pkt, 0) = 0x9000000;
				U8(pkt, 7) = 0x36;
				x::GTE_AVSZ3();
				x::GTE_ReadOTZ(hdr + 0x6C);
				U32(hdr, 0x6C) = U32(hdr, 0x6C) >> 2;
				gx::GTE_StoreSXY012_PolyGT3(pkt);
				U32(pkt, 0x18) = U32(rec, 0xC);
				U32(pkt, 0xC) = U32(rec, 8);
				U16(pkt, 0x24) = U16(rec, 6);
				uint8_t g = U8(vbuf + U32(hdr, 0x38) * 0xC, 8);
				U8(pkt, 6) = g;
				U8(pkt, 5) = g;
				U8(pkt, 4) = g;
				g = U8(vbuf + U32(hdr, 0x3C) * 0xC, 8);
				U8(pkt, 0x12) = g;
				U8(pkt, 0x11) = g;
				U8(pkt, 0x10) = g;
				g = U8(vbuf + U32(hdr, 0x40) * 0xC, 8);
				U8(pkt, 0x1E) = g;
				U8(pkt, 0x1D) = g;
				U8(pkt, 0x1C) = g;
				x::SSIGPU_InsertPrimAutoDepth(a2 + U32(hdr, 0x6C) * 4, pkt);
				pkt += 0x28;
			}
			if (n4 > 0)
			{
				rec += 4;
				for (int32_t k = n4; k > 0; k--, rec += 0x14)
				{
					const uint32_t i2 = U16(rec, 0) & 0xFFF, i1 = U16(rec, -2) & 0xFFF, i0 = U16(rec, -4) & 0xFFF;
					U32(hdr, 0x40) = i2;
					U32(hdr, 0x3C) = i1;
					U32(hdr, 0x38) = i0;
					x::GTE_LoadV012(vbuf + i0 * 0xC, vbuf + i1 * 0xC, vbuf + i2 * 0xC);
					x::GTE_RTPT();
					x::GTE_NCLIP();
					x::GTE_ReadMAC0(hdr + 0x68);
					if (S32(hdr, 0x68) <= 0)
						continue;
					U8(pkt, 7) = 0x3E;
					U32(pkt, 0) = 0xC000000;
					x::GTE_StoreSXY012_PolyGT3_2(pkt);
					const uint32_t i3 = U16(rec, 2) & 0xFFF;
					U32(hdr, 0x44) = i3;
					x::GTE_LoadV0(vbuf + i3 * 0xC);
					x::GTE_RTPS();
					x::GTE_ReadSXY2(pkt + 0x2C);
					x::GTE_AVSZ4();
					x::GTE_ReadOTZ(hdr + 0x6C);
					U32(pkt, 0x18) = U32(rec, 8);
					U32(pkt, 0xC) = U32(rec, 4);
					U16(pkt, 0x30) = U16(rec, 0xE);
					U16(pkt, 0x24) = U16(rec, 0xC);
					const uint32_t z = U32(hdr, 0x6C) >> 2;
					U32(hdr, 0x6C) = z;
					uint8_t g = U8(vbuf + U32(hdr, 0x38) * 0xC, 8);
					U8(pkt, 6) = g;
					U8(pkt, 5) = g;
					U8(pkt, 4) = g;
					g = U8(vbuf + U32(hdr, 0x3C) * 0xC, 8);
					U8(pkt, 0x12) = g;
					U8(pkt, 0x11) = g;
					U8(pkt, 0x10) = g;
					g = U8(vbuf + U32(hdr, 0x40) * 0xC, 8);
					U8(pkt, 0x1E) = g;
					U8(pkt, 0x1D) = g;
					U8(pkt, 0x1C) = g;
					g = U8(vbuf + U32(hdr, 0x44) * 0xC, 8);
					U8(pkt, 0x2A) = g;
					U8(pkt, 0x29) = g;
					U8(pkt, 0x28) = g;
					x::SSIGPU_InsertPrimAutoDepth(a2 + z * 4, pkt);
					pkt += 0x34;
				}
			}
		}
		MEM<uint32_t>(U32(ctx, 0x4C)) = pkt;
		return x::Field_Free(0x74);
	}

	uint32_t __cdecl te_796BF0(uint32_t a1)
	{
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2C) * 0x9C;
		U32(a1, 0x60) = a1 + 0x114;
		U32(a1, 0x4C) = U32(ent, 0x1C);
		U32(a1, 0x50) = U32(ent, 0x20);
		S32(a1, 0x11C) = 0x1000;
		S32(a1, 0x118) = 0x1000;
		S32(a1, 0x114) = 0x1000;
		x::au_re_Battle_ReadAnimation_7(a1, 0);
		te_796C70(a1);
		gx::BS_BindEntityModelFromComData(ent, game_addr(game::unk_25C9F10), game_addr(game::MAG_084_ANGEL_WING_AnimSet));
		gx::QueueChainTransformation(ent, 1);
		next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_796C70(uint32_t a1)
	{
		te_796CA0(game_addr(REAPER_CTX), game_addr(game::unk_25C3948), game_addr(game::unk_25C9F40), a1 + 0x30);
		U8(a1, 0x26) |= 8;
		return 0;
	}

	uint32_t __cdecl te_796CA0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl te_796D00(uint32_t a1)
	{
		if (S16(a1, 0x136) >= 0x15)
		{
			x::BdPlaySE(game_addr(game::unk_15720AC), 0, 0x80);
			if (S16(a1, 0x136) >= 0x15)
			{
				const uint16_t r = (uint16_t)(MEM<uint16_t>(game_addr(game::MAG_084_ANGEL_WING_RevealRadius)) + 0x20);
				MEM<uint16_t>(game_addr(game::MAG_084_ANGEL_WING_RevealRadius)) = r;
				if ((int16_t)r >= 0x500)
					MEM<uint16_t>(game_addr(game::MAG_084_ANGEL_WING_RevealRadius)) = 0x500;
			}
		}
		if (a_73B7E0(2) != 0)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) & 0xF7);
			next_state(a1);
		}
		x::au_re_Battle_ReadAnimation_8(a1);
		return 0;
	}

	uint32_t __cdecl te_796DB0(uint32_t a1)
	{
		if (a_73B7E0(4) != 0)
			next_state(a1);
		x::au_re_Battle_ReadAnimation_8(a1);
		return 0;
	}

	uint32_t __cdecl te_796DD0(uint32_t a1)
	{
		x::au_re_Battle_ReadAnimation_8(a1);
		U16(a1, 0x13A) = (uint16_t)(U16(a1, 0x13A) + 0x100);
		if (S16(a1, 0x13A) >= 0x1000)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 5);
			U16(a1, 0x13A) = 0x1000;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		U16(a1, 0x13C) = U16(a1, 0x13A);
		te_796E30(a1, 3);
		te_796F50(a1);
		return 0;
	}

	uint32_t __cdecl te_796E30(uint32_t a1, uint32_t a2)
	{
		const uint32_t node = a1;
		a_733950(node + 0x30, U16(node, 0x13A), a2);
		return 0;
	}

	uint32_t __cdecl te_796F50(uint32_t a1)
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

	uint32_t __cdecl te_796FD0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[7];
		states[0] = game_addr(game::MAG_084_ANGEL_WING_GlowA_State0_Hide);
		states[1] = game_addr(game::MAG_084_ANGEL_WING_GlowA_State1_ShowAtFrame20);
		states[2] = game_addr(game::MAG_084_ANGEL_WING_GlowA_State2_FadeOut);
		states[3] = game_addr(game::MAG_084_ANGEL_WING_GlowA_State3_WaitCut4);
		states[4] = game_addr(game::MAG_084_ANGEL_WING_GlowA_State4_Count);
		states[5] = game_addr(game::MAG_084_ANGEL_WING_GlowA_State5_FadeIn);
		states[6] = game_addr(game::MAG_084_ANGEL_WING_Nullsub_7972A0);
		callp(states[S8(node, 0x29)], node);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2C) * 0x9C;
		x::GetEffectSpawnPosition(ent, 0xF1, 0, node + 0x1C);
		te_797080(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl te_797080(uint32_t a1)
	{
		if ((U8(a1, 0x26) & 4) != 0)
			return 0;
		alignas(4) uint8_t loc[0x20] = {};
		const uint32_t M = P(loc);
		x::UnpackRotationMatrix(game_addr(game::unk_1D97778), M);
		x::sub_8DD960(M, (uint32_t)(int32_t)S16(a1, 0x48));
		S32(M, 0x14) = S16(a1, 0x1C);
		S32(M, 0x18) = S16(a1, 0x1E);
		x::scale3DMatrix(M, a1 + 0x30);
		S32(M, 0x1C) = S16(a1, 0x20);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), M, M);
		x::GTE_SetRotMatrix_W(M);
		x::GTE_SetTransVector_W(M);
		const uint32_t hdr = x::Field_Alloc(0x68);
		U32(hdr, 0) = U32(a1, 0x4C);
		U32(hdr, 8) = U32(a1, 0x40);
		S32(hdr, 0xC) = S16(a1, 0x50);
		U16(hdr, 0x22) = 0x100;
		U16(hdr, 0x20) = 0x100;
		U16(hdr, 0x2A) = 0x100;
		U16(hdr, 0x28) = 0x100;
		S32(hdr, 0x10) = S16(a1, 0x52);
		U16(hdr, 0x18) = 0;
		U16(hdr, 0x1A) = 0;
		U16(hdr, 0x1E) = 0;
		U16(hdr, 0x1C) = 0;
		U16(hdr, 0x26) = 0;
		U16(hdr, 0x24) = 0;
		U32(hdr, 0x14) = 0xF0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = te_78F1B0(hdr, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl te_797190(uint32_t a1)
	{
		const uint32_t node = a1;
		uint8_t st = U8(node, 0x29);
		U8(node, 0x26) |= 4;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl te_7971B0(uint32_t a1)
	{
		if (S16(a1, 0x24) < 0x20)
			return 0;
		U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) & 0xFB);
		S32(a1, 0x38) = 0x1000;
		S32(a1, 0x34) = 0x1000;
		S32(a1, 0x30) = 0x1000;
		U16(a1, 0x50) = 0x1000;
		U32(a1, 0x4C) = game_addr(game::unk_15720B0);
		U16(a1, 0x52) = 0xFE00;
		next_state(a1);
		return 0;
	}

	uint32_t __cdecl te_7971F0(uint32_t a1)
	{
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) - 0x80);
		if (S16(a1, 0x50) <= 0)
		{
			U16(a1, 0x50) = 0;
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl te_797220(uint32_t a1)
	{
		if (a_73B7E0(4) != 0)
		{
			U16(a1, 0x54) = 0x18;
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl te_797250(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x54) = (uint16_t)(U16(node, 0x54) - 1);
		if (S16(node, 0x54) <= 0)
			t4_next_state(node);
		return 0;
	}

	uint32_t __cdecl te_797270(uint32_t a1)
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

	uint32_t __cdecl te_7972B0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[7];
		states[0] = game_addr(game::MAG_084_ANGEL_WING_GlowB_State0_Hide);
		states[1] = game_addr(game::MAG_084_ANGEL_WING_GlowB_State1_ShowAtFrame20);
		states[2] = game_addr(game::MAG_084_ANGEL_WING_GlowB_State2_FadeOut);
		states[3] = game_addr(game::MAG_084_ANGEL_WING_GlowB_State3_HideAtCut3);
		states[4] = game_addr(game::MAG_084_ANGEL_WING_GlowB_State4_ShowAtCut4);
		states[5] = game_addr(game::MAG_084_ANGEL_WING_GlowB_State5_FadeIn);
		states[6] = game_addr(game::MAG_084_ANGEL_WING_Nullsub_797530);
		callp(states[S8(node, 0x29)], node);
		te_797330(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl te_797330(uint32_t a1)
	{
		if ((U8(a1, 0x26) & 4) != 0)
			return 0;
		alignas(4) uint8_t m[0x20];
		const uint32_t M = P(m);
		x::MAG_022_sub_8DD770(M);
		S32(M, 0x14) = S16(a1, 0x1C);
		S32(M, 0x18) = S16(a1, 0x1E);
		S32(M, 0x1C) = S16(a1, 0x20);
		x::scale3DMatrix(M, a1 + 0x30);
		x::GTE_SetRotMatrix_W(M);
		x::GTE_SetTransVector_W(M);
		const uint32_t hdr = x::Field_Alloc(0x68);
		U32(hdr, 0) = U32(a1, 0x4C);
		U32(hdr, 8) = U32(a1, 0x40);
		U32(hdr, 0xC) = (uint32_t)(int32_t)S16(a1, 0x50);
		U16(hdr, 0x18) = 0;
		U16(hdr, 0x1A) = 0;
		U16(hdr, 0x1E) = 0;
		U16(hdr, 0x1C) = 0;
		U16(hdr, 0x26) = 0;
		U16(hdr, 0x24) = 0;
		U32(hdr, 0x10) = (uint32_t)(int32_t)S16(a1, 0x52);
		U16(hdr, 0x22) = 0x100;
		U16(hdr, 0x20) = 0x100;
		U16(hdr, 0x2A) = 0x100;
		U16(hdr, 0x28) = 0x100;
		U32(hdr, 0x14) = 0xF0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = te_78F1B0(hdr, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl te_797410(uint32_t a1)
	{
		const uint32_t node = a1;
		uint8_t st = U8(node, 0x29);
		U8(node, 0x26) |= 4;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl te_797430(uint32_t a1)
	{
		if (S16(a1, 0x24) < 0x20)
			return 0;
		U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) & 0xFB);
		U16(a1, 0x1C) = 0;
		U16(a1, 0x1E) = 0;
		const uint32_t cam = MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_pCameraBlock));
		S32(a1, 0x38) = 0x1000;
		S32(a1, 0x34) = 0x1000;
		S32(a1, 0x30) = 0x1000;
		const uint16_t h = U16(cam, 0xE0);
		U16(a1, 0x50) = 0x1000;
		next_state(a1);
		U32(a1, 0x4C) = game_addr(game::unk_1572810);
		U16(a1, 0x20) = h;
		U16(a1, 0x52) = 0xFE00;
		return 0;
	}

	uint32_t __cdecl te_797490(uint32_t a1)
	{
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) - 0x80);
		if (S16(a1, 0x50) <= 0)
		{
			U16(a1, 0x50) = 0;
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl te_7974C0(uint32_t a1)
	{
		if (a_73B7E0(3) != 0)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 4);
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl te_7974E0(uint32_t a1)
	{
		if (a_73B7E0(4) != 0)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) & 0xFB);
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl te_797500(uint32_t a1)
	{
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0x80);
		if (S16(a1, 0x50) >= 0x1000)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			next_state(a1);
			U16(a1, 0x50) = 0x1000;
		}
		return 0;
	}

	uint32_t __cdecl te_797540(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::MAG_084_ANGEL_WING_pDirectorBlock));
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
		{ game::MAG_084_ANGEL_WING_Sequence_State0_Advance_78DAD0, (void *)a_73A0D0, "084 MAG_084_sub_78DAD0" },
		{ game::MAG_084_ANGEL_WING_Sequence_State0_Advance_78DAE0, (void *)a_73A0D0, "084 MAG_084_sub_78DAE0" },
		{ game::MAG_084_ANGEL_WING_Sequence_State3_SpawnEmitter, (void *)a_73A170, "084 MAG_084_sub_78DB70" },
		{ game::MAG_084_ANGEL_WING_EmitterTask, (void *)a_73A1A0, "084 MAG_084_sub_78DBA0" },
		{ game::MAG_084_ANGEL_WING_Camera_ResetBlock, (void *)a_73AB00, "084 sub_78DEE0" },
		{ game::MAG_084_ANGEL_WING_DirStepGE, (void *)a_73AAE0, "084 sub_78DF30" },
		{ game::MAG_084_ANGEL_WING_Camera_StartKeyMove, (void *)a_73AB20, "084 sub_78DF50" },
		{ game::MAG_084_ANGEL_WING_Camera_KeyStep, (void *)a_73AE10, "084 sub_78E240" },
		{ game::MAG_084_ANGEL_WING_Camera_WriteBattleCamera, (void *)a_73B160, "084 sub_78E590" },
		{ game::MAG_084_ANGEL_WING_Camera_State6_KeyStepThenKeyAtStep4, (void *)a_73B350, "084 sub_78E7C0" },
		{ game::MAG_084_ANGEL_WING_Camera_State8_FinishNode, (void *)a_73B750, "084 sub_78E810" },
		{ game::MAG_084_ANGEL_WING_Nullsub_78E830, (void *)a_73A6D0, "084 nullsub_1330" },
		{ game::MAG_084_ANGEL_WING_Nullsub_78E840, (void *)a_73A6D0, "084 nullsub_1331" },
		{ game::MAG_084_ANGEL_WING_Director_State1_WaitDirGate1, (void *)a_73B790, "084 sub_78E850" },
		{ game::MAG_084_ANGEL_WING_DirSyncGate, (void *)a_73B640, "084 sub_78E870" },
		{ game::MAG_084_ANGEL_WING_DirPhaseGE, (void *)a_73B7E0, "084 sub_78E8D0" },
		{ game::MAG_084_ANGEL_WING_Director_State4_WaitDirGate2, (void *)a_73B8A0, "084 sub_78E960" },
		{ game::MAG_084_ANGEL_WING_Director_State5_WaitDirPhase2, (void *)a_73B8C0, "084 sub_78E980" },
		{ game::MAG_084_ANGEL_WING_Director_State7_WaitDirGate3, (void *)a_733B70, "084 sub_78E9C0" },
		{ game::MAG_084_ANGEL_WING_Director_State14_Finish, (void *)a_7475A0, "084 sub_78EB10" },
		{ game::MAG_084_ANGEL_WING_Nullsub_78EB30, (void *)a_73A6D0, "084 nullsub_1332" },
		{ game::MAG_084_ANGEL_WING_Nullsub_78EB40, (void *)a_73A6D0, "084 nullsub_1333" },
		{ game::MAG_084_ANGEL_WING_SpawnChildTask, (void *)a_73C100, "084 sub_78EC30" },
		{ game::MAG_084_ANGEL_WING_PlayA_Task, (void *)a_73A380, "084 sub_78ECD0" },
		{ game::MAG_084_ANGEL_WING_EmitG3, (void *)a_73D770, "084 sub_790190" },
		{ game::MAG_084_ANGEL_WING_EmitG4, (void *)a_73D9C0, "084 sub_7903E0" },
		{ game::MAG_084_ANGEL_WING_Nullsub_791250, (void *)a_73A6D0, "084 nullsub_1334" },
		{ game::MAG_084_ANGEL_WING_PlayB_Task, (void *)a_73A380, "084 sub_791260" },
		{ game::MAG_084_ANGEL_WING_ClearEntityRecordBit1, (void *)a_746A10, "084 sub_791330" },
		{ game::MAG_084_ANGEL_WING_RestoreEntityRecordBit1, (void *)a_746AE0, "084 sub_791380" },
		{ game::MAG_084_ANGEL_WING_Nullsub_7913B0, (void *)a_73A6D0, "084 nullsub_1335" },
		{ game::MAG_084_ANGEL_WING_FeatherActorSystemTask, (void *)a_73A380, "084 sub_7913C0" },
		{ game::MAG_084_ANGEL_WING_ActSys_Setup, (void *)a_73F990, "084 sub_791450" },
		{ game::MAG_084_ANGEL_WING_Director_StartPositions, (void *)a_73FC20, "084 sub_7916E0" },
		{ game::MAG_084_ANGEL_WING_Director_Mode3Setup, (void *)a_73FE90, "084 sub_791950" },
		{ game::MAG_084_ANGEL_WING_Director_TargetPositions, (void *)a_73FFB0, "084 sub_791A70" },
		{ game::MAG_084_ANGEL_WING_RelocateActorData, (void *)a_740210, "084 sub_791CD0" },
		{ game::MAG_084_ANGEL_WING_RenderPrimModel_792160, (void *)a_7355E0, "084 sub_792160" },
		{ game::MAG_084_ANGEL_WING_RenderPolyFT3List, (void *)a_735BB0, "084 sub_792730" },
		{ game::MAG_084_ANGEL_WING_RenderPolyFT4List, (void *)a_735DF0, "084 sub_792970" },
		{ game::MAG_084_ANGEL_WING_ActorMotion, (void *)a_740910, "084 sub_793850" },
		{ game::MAG_084_ANGEL_WING_DampValue, (void *)a_740F70, "084 sub_793EB0" },
		{ game::MAG_084_ANGEL_WING_DampVector, (void *)a_740FC0, "084 sub_793F00" },
		{ game::MAG_084_ANGEL_WING_Instance_AdvanceTexFrame, (void *)a_741050, "084 sub_793F90" },
		{ game::MAG_084_ANGEL_WING_InstanceBuildTransform, (void *)a_741120, "084 sub_794060" },
		{ game::MAG_084_ANGEL_WING_Particle_LifeCountdown, (void *)a_741B60, "084 sub_794AA0" },
		{ game::MAG_084_ANGEL_WING_RandRange, (void *)a_742290, "084 au_re__rand_21" },
		{ game::MAG_084_ANGEL_WING_RandRangeWide, (void *)a_7422C0, "084 sub_795250" },
		{ game::MAG_084_ANGEL_WING_RandVec3, (void *)a_742300, "084 sub_795290" },
		{ game::MAG_084_ANGEL_WING_JitterAngles3, (void *)a_742350, "084 sub_7952E0" },
		{ game::MAG_084_ANGEL_WING_RandRange2, (void *)a_7423A0, "084 au_re__rand_21_0" },
		{ game::MAG_084_ANGEL_WING_GetWord8Minus1, (void *)a_7424B0, "084 sub_795440" },
		{ game::MAG_084_ANGEL_WING_Particle_AttachToOwner, (void *)a_7424C0, "084 sub_795450" },
		{ game::MAG_084_ANGEL_WING_ParticleList_Append, (void *)a_742CA0, "084 sub_795C30" },
		{ game::MAG_084_ANGEL_WING_ParticleList_Unlink, (void *)a_742CD0, "084 sub_795C60" },
		{ game::MAG_084_ANGEL_WING_Particle_Init, (void *)a_742D00, "084 sub_795C90" },
		{ game::MAG_084_ANGEL_WING_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "084 sub_795E40" },
		{ game::MAG_084_ANGEL_WING_Particle_PlaceAtPoint, (void *)a_742FF0, "084 sub_795F80" },
		{ game::MAG_084_ANGEL_WING_Nullsub_7961D0, (void *)a_73A6D0, "084 nullsub_1336" },
		{ game::MAG_084_ANGEL_WING_PlayC_Task, (void *)a_73A380, "084 sub_796290" },
		{ game::MAG_084_ANGEL_WING_Nullsub_796380, (void *)a_73A6D0, "084 nullsub_1337" },
		{ game::MAG_084_ANGEL_WING_DrawModel, (void *)a_746C10, "084 sub_796440" },
		{ game::MAG_084_ANGEL_WING_ModelContainer_State2_WaitAnimDone, (void *)a_739B40, "084 sub_796D80" },
		{ game::MAG_084_ANGEL_WING_SetEntityColourTint, (void *)a_733950, "084 sub_796E50" },
		{ game::MAG_084_ANGEL_WING_Nullsub_796FC0, (void *)a_73A6D0, "084 nullsub_1338" },
		{ game::MAG_084_ANGEL_WING_Nullsub_7972A0, (void *)a_73A6D0, "084 nullsub_1339" },
		{ game::MAG_084_ANGEL_WING_Nullsub_797530, (void *)a_73A6D0, "084 nullsub_1340" },
		{ game::MAG_084_ANGEL_WING_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "084 MAG_084_sub_797580" },
		{ game::MAG_084_ANGEL_WING_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "084 MAG_084_sub_7975A0" },
		{ game::MAG_084_ANGEL_WING_Nullsub_7975C0, (void *)a_73A6D0, "084 nullsub_1341" },
		{ game::MAG_084_ANGEL_WING_Sequence_State4_NextTargetLoop, (void *)a_747500, "084 MAG_084_sub_7975D0" },
		{ game::MAG_084_ANGEL_WING_Sequence_State0_Advance_797610, (void *)a_73A0D0, "084 MAG_084_sub_797610" },
		{ game::MAG_084_ANGEL_WING_Sequence_State6_WaitQueuesEmpty, (void *)a_747550, "084 MAG_084_sub_797620" },
		{ game::MAG_084_ANGEL_WING_Sequence_State7_RequestBattleSync, (void *)a_747560, "084 MAG_084_sub_797630" },
		{ game::MAG_084_ANGEL_WING_Sequence_State8_WaitBattleSyncDone, (void *)a_747590, "084 MAG_084_sub_797660" },
		{ game::MAG_084_ANGEL_WING_Sequence_State9_Finish, (void *)a_7475A0, "084 MAG_084_sub_797670" },
		{ game::MAG_084_ANGEL_WING_Nullsub_797690, (void *)a_73A6D0, "084 nullsub_1329" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_084_ANGEL_WING_Master, (void *)te_78D940, "084 te_78D940" },
		{ game::MAG_084_ANGEL_WING_Master_State2_CarvePools, (void *)te_78DAF0, "084 te_78DAF0" },
		{ game::MAG_084_ANGEL_WING_ClearPools, (void *)te_78DB30, "084 te_78DB30" },
		{ game::MAG_084_ANGEL_WING_Emitter_State0_SpawnDirector, (void *)te_78DC20, "084 te_78DC20" },
		{ game::MAG_084_ANGEL_WING_DirectorDataSetup, (void *)te_78DC50, "084 te_78DC50" },
		{ game::MAG_084_ANGEL_WING_DirectorTask, (void *)te_78DCE0, "084 te_78DCE0" },
		{ game::MAG_084_ANGEL_WING_Director_CutBookkeeping, (void *)te_78DDB0, "084 te_78DDB0" },
		{ game::MAG_084_ANGEL_WING_Director_State0_UploadTIM_SpawnCameraTask, (void *)te_78DE00, "084 te_78DE00" },
		{ game::MAG_084_ANGEL_WING_CameraTask, (void *)te_78DE40, "084 te_78DE40" },
		{ game::MAG_084_ANGEL_WING_Camera_State0_ResetBlock, (void *)te_78DED0, "084 te_78DED0" },
		{ game::MAG_084_ANGEL_WING_Camera_State1_StartScriptAtCut1, (void *)te_78DF00, "084 te_78DF00" },
		{ game::MAG_084_ANGEL_WING_Camera_State2_ScriptChain_1575E28, (void *)te_78E700, "084 te_78E700" },
		{ game::MAG_084_ANGEL_WING_Camera_State3_StepNextAtCue2, (void *)te_78E730, "084 te_78E730" },
		{ game::MAG_084_ANGEL_WING_Camera_State4_StepNewPathAtCut3, (void *)te_78E760, "084 te_78E760" },
		{ game::MAG_084_ANGEL_WING_Camera_State5_ScriptChain_1575F90, (void *)te_78E790, "084 te_78E790" },
		{ game::MAG_084_ANGEL_WING_Camera_State7_StepUntilCut5, (void *)te_78E7F0, "084 te_78E7F0" },
		{ game::MAG_084_ANGEL_WING_Director_State2_HideEntitiesAtCut1, (void *)te_78E8A0, "084 te_78E8A0" },
		{ game::MAG_084_ANGEL_WING_HideOtherEntities, (void *)te_78E8F0, "084 te_78E8F0" },
		{ game::MAG_084_ANGEL_WING_Director_State3_WaitFrame36, (void *)te_78E940, "084 te_78E940" },
		{ game::MAG_084_ANGEL_WING_Director_State6_WaitFrame19, (void *)te_78E9A0, "084 te_78E9A0" },
		{ game::MAG_084_ANGEL_WING_Director_State8_WaitCut3, (void *)te_78E9E0, "084 te_78E9E0" },
		{ game::MAG_084_ANGEL_WING_Director_State9_WaitFrame41, (void *)te_78EA00, "084 te_78EA00" },
		{ game::MAG_084_ANGEL_WING_Director_State10_RequestCut4, (void *)te_78EA20, "084 te_78EA20" },
		{ game::MAG_084_ANGEL_WING_Director_State11_ShowEntitiesAtCut4, (void *)te_78EA40, "084 te_78EA40" },
		{ game::MAG_084_ANGEL_WING_RestoreOtherEntities, (void *)te_78EA70, "084 te_78EA70" },
		{ game::MAG_084_ANGEL_WING_Director_State12_WaitFrame3C, (void *)te_78EAC0, "084 te_78EAC0" },
		{ game::MAG_084_ANGEL_WING_Director_State13_DoneAtCut5, (void *)te_78EAE0, "084 te_78EAE0" },
		{ game::MAG_084_ANGEL_WING_SpawnCutParts, (void *)te_78EB50, "084 te_78EB50" },
		{ game::MAG_084_ANGEL_WING_SpawnPrimPlayTask, (void *)te_78EC80, "084 te_78EC80" },
		{ game::MAG_084_ANGEL_WING_PlayA_State0_Init, (void *)te_78ED30, "084 te_78ED30" },
		{ game::MAG_084_ANGEL_WING_PlayStep, (void *)te_78EDB0, "084 te_78EDB0" },
		{ game::MAG_084_ANGEL_WING_PrimObjectCallback, (void *)te_78EEE0, "084 te_78EEE0" },
		{ game::MAG_084_ANGEL_WING_RenderPrimModel, (void *)te_78F1B0, "084 te_78F1B0" },
		{ game::MAG_084_ANGEL_WING_PrimList_FlatTri, (void *)te_78F2E0, "084 te_78F2E0" },
		{ game::MAG_084_ANGEL_WING_PrimList_FlatQuad, (void *)te_78F500, "084 te_78F500" },
		{ game::MAG_084_ANGEL_WING_PrimList_TexTri, (void *)te_78F790, "084 te_78F790" },
		{ game::MAG_084_ANGEL_WING_PrimList_TexQuad, (void *)te_78FC10, "084 te_78FC10" },
		{ game::MAG_084_ANGEL_WING_PrimList_GouraudTexTriScroll, (void *)te_7906C0, "084 te_7906C0" },
		{ game::MAG_084_ANGEL_WING_PrimList_GT4, (void *)te_790BC0, "084 te_790BC0" },
		{ game::MAG_084_ANGEL_WING_PlayA_State1_PlayUntilCut3, (void *)te_791210, "084 te_791210" },
		{ game::MAG_084_ANGEL_WING_PlayB_State0_Init, (void *)te_7912C0, "084 te_7912C0" },
		{ game::MAG_084_ANGEL_WING_PlayB_State1_PlayUntilCut4, (void *)te_791350, "084 te_791350" },
		{ game::MAG_084_ANGEL_WING_ActorSystem_State0_Start, (void *)te_791420, "084 te_791420" },
		{ game::MAG_084_ANGEL_WING_ResetTrailHistory, (void *)te_791600, "084 te_791600" },
		{ game::MAG_084_ANGEL_WING_FillTrailRows, (void *)te_791880, "084 te_791880" },
		{ game::MAG_084_ANGEL_WING_ActSys_Step, (void *)te_791F30, "084 te_791F30" },
		{ game::MAG_084_ANGEL_WING_ActSys_DrawPass, (void *)te_791FC0, "084 te_791FC0" },
		{ game::MAG_084_ANGEL_WING_ActSys_DrawActorModel, (void *)te_792020, "084 te_792020" },
		{ game::MAG_084_ANGEL_WING_MeshEmitF3, (void *)te_7922A0, "084 te_7922A0" },
		{ game::MAG_084_ANGEL_WING_MeshEmitF4, (void *)te_7924B0, "084 te_7924B0" },
		{ game::MAG_084_ANGEL_WING_PrimList_G3, (void *)te_792C10, "084 te_792C10" },
		{ game::MAG_084_ANGEL_WING_PrimList_G4, (void *)te_792E40, "084 te_792E40" },
		{ game::MAG_084_ANGEL_WING_PrimList_GT3, (void *)te_793100, "084 te_793100" },
		{ game::MAG_084_ANGEL_WING_PrimList_GT4_793360, (void *)te_793360, "084 te_793360" },
		{ game::MAG_084_ANGEL_WING_ActSys_DrawSpritePass, (void *)te_793640, "084 te_793640" },
		{ game::MAG_084_ANGEL_WING_ActSys_DrawActorSprite, (void *)te_793700, "084 te_793700" },
		{ game::MAG_084_ANGEL_WING_ActSys_UpdatePass, (void *)te_793780, "084 te_793780" },
		{ game::MAG_084_ANGEL_WING_ActorStep, (void *)te_7937C0, "084 te_7937C0" },
		{ game::MAG_084_ANGEL_WING_MorphVertexBlock, (void *)te_7947F0, "084 te_7947F0" },
		{ game::MAG_084_ANGEL_WING_Instance_EndOfLifeTest, (void *)te_794900, "084 te_794900" },
		{ game::MAG_084_ANGEL_WING_Instance_SpawnFollowUp, (void *)te_794980, "084 te_794980" },
		{ game::MAG_084_ANGEL_WING_ActSys_AllocEmitterSlot, (void *)te_7949B0, "084 te_7949B0" },
		{ game::MAG_084_ANGEL_WING_ActSys_UpdateEmitter, (void *)te_794A50, "084 te_794A50" },
		{ game::MAG_084_ANGEL_WING_ActSys_EmitterSpawnStep, (void *)te_794AD0, "084 te_794AD0" },
		{ game::MAG_084_ANGEL_WING_ActSys_SpawnParticlesAtModelBones, (void *)te_794B50, "084 te_794B50" },
		{ game::MAG_084_ANGEL_WING_ActSys_AllocParticleSlot, (void *)te_795380, "084 te_795380" },
		{ game::MAG_084_ANGEL_WING_Actor_InterpolatePosition, (void *)te_7955A0, "084 te_7955A0" },
		{ game::MAG_084_ANGEL_WING_SpawnSubEffects, (void *)te_796090, "084 te_796090" },
		{ game::MAG_084_ANGEL_WING_Director_SpawnGroupEffects, (void *)te_796120, "084 te_796120" },
		{ game::MAG_084_ANGEL_WING_ActorSystem_State1_Step, (void *)te_7961B0, "084 te_7961B0" },
		{ game::MAG_084_ANGEL_WING_ModelContainerTask, (void *)te_7961E0, "084 te_7961E0" },
		{ game::MAG_084_ANGEL_WING_PlayC_State0_Init, (void *)te_7962F0, "084 te_7962F0" },
		{ game::MAG_084_ANGEL_WING_PlayC_State1_Play, (void *)te_796360, "084 te_796360" },
		{ game::MAG_084_ANGEL_WING_ModelContainer_Draw, (void *)te_796390, "084 te_796390" },
		{ game::MAG_084_ANGEL_WING_Reveal_DrawModel, (void *)te_796620, "084 te_796620" },
		{ game::MAG_084_ANGEL_WING_Reveal_RenderModel, (void *)te_796720, "084 te_796720" },
		{ game::MAG_084_ANGEL_WING_ModelContainer_State0_SetupSwapCasterModel, (void *)te_796BF0, "084 te_796BF0" },
		{ game::MAG_084_ANGEL_WING_Reveal_InitDrawContext, (void *)te_796C70, "084 te_796C70" },
		{ game::MAG_084_ANGEL_WING_InitDrawBlock, (void *)te_796CA0, "084 te_796CA0" },
		{ game::MAG_084_ANGEL_WING_ModelContainer_State1_RevealGrow, (void *)te_796D00, "084 te_796D00" },
		{ game::MAG_084_ANGEL_WING_ModelContainer_State3_WaitCut4, (void *)te_796DB0, "084 te_796DB0" },
		{ game::MAG_084_ANGEL_WING_ModelContainer_State4_FadeOut, (void *)te_796DD0, "084 te_796DD0" },
		{ game::MAG_084_ANGEL_WING_ModelContainer_ApplyFadeTint, (void *)te_796E30, "084 te_796E30" },
		{ game::MAG_084_ANGEL_WING_ModelContainer_SetAmbientColour, (void *)te_796F50, "084 te_796F50" },
		{ game::MAG_084_ANGEL_WING_GlowA_Task, (void *)te_796FD0, "084 te_796FD0" },
		{ game::MAG_084_ANGEL_WING_GlowA_Draw, (void *)te_797080, "084 te_797080" },
		{ game::MAG_084_ANGEL_WING_GlowA_State0_Hide, (void *)te_797190, "084 te_797190" },
		{ game::MAG_084_ANGEL_WING_GlowA_State1_ShowAtFrame20, (void *)te_7971B0, "084 te_7971B0" },
		{ game::MAG_084_ANGEL_WING_GlowA_State2_FadeOut, (void *)te_7971F0, "084 te_7971F0" },
		{ game::MAG_084_ANGEL_WING_GlowA_State3_WaitCut4, (void *)te_797220, "084 te_797220" },
		{ game::MAG_084_ANGEL_WING_GlowA_State4_Count, (void *)te_797250, "084 te_797250" },
		{ game::MAG_084_ANGEL_WING_GlowA_State5_FadeIn, (void *)te_797270, "084 te_797270" },
		{ game::MAG_084_ANGEL_WING_GlowB_Task, (void *)te_7972B0, "084 te_7972B0" },
		{ game::MAG_084_ANGEL_WING_GlowB_Draw, (void *)te_797330, "084 te_797330" },
		{ game::MAG_084_ANGEL_WING_GlowB_State0_Hide, (void *)te_797410, "084 te_797410" },
		{ game::MAG_084_ANGEL_WING_GlowB_State1_ShowAtFrame20, (void *)te_797430, "084 te_797430" },
		{ game::MAG_084_ANGEL_WING_GlowB_State2_FadeOut, (void *)te_797490, "084 te_797490" },
		{ game::MAG_084_ANGEL_WING_GlowB_State3_HideAtCut3, (void *)te_7974C0, "084 te_7974C0" },
		{ game::MAG_084_ANGEL_WING_GlowB_State4_ShowAtCut4, (void *)te_7974E0, "084 te_7974E0" },
		{ game::MAG_084_ANGEL_WING_GlowB_State5_FadeIn, (void *)te_797500, "084 te_797500" },
		{ game::MAG_084_ANGEL_WING_Emitter_State1_ApplyDamage, (void *)te_797540, "084 te_797540" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag084_angel_wing()
	{
		act::register_module(84);
		for (const act::angelwing::ModPort *p = act::angelwing::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(84, p->addr, p->port, p->name);
		for (const act::angelwing::ModPort *p = act::angelwing::PORTS; p->addr; p++)
			act::register_module_port(84, p->addr, p->port, p->name);
	}
}

