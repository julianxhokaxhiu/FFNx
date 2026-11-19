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
namespace moon88
{
	static const Mod MOD_088 = { "invinciblemoon", 88, game::MAG_088_INVINCIBLE_MOON_FL, game::MAG_087_ANGELO_STRIKE_LoadFile,
		{ {}, {}, game::unk_154ECE0, game::unk_154F918, {}, game::unk_25A4EA4, {}, {}, {}, {}, {}, {}, game::unk_25A4D98, game::unk_25A4D9C, game::unk_25A4E80, game::unk_25A4EA4, game::unk_25A4EA8, game::unk_25A6BC0, {}, game::unk_25A6BBC, game::unk_25AA9F0, game::unk_25A4F88, game::unk_25AAA00, game::unk_25AAA04, game::unk_25AB498, game::unk_25AB728, game::unk_25AB738, game::unk_25AB8A8, game::unk_25AB8AC, game::unk_25AB8B0, game::unk_25AC3A8, game::unk_25AC6C8, game::unk_25AC6D8, {}, game::unk_25AC750, game::unk_25ACCF0, game::unk_25ACCF8, game::unk_25ACD0C, game::unk_25ACD34, game::unk_25ACD3C },
		{ {}, game::MAG_088_INVINCIBLE_MOON_Camera_State0_Reset, game::MAG_088_INVINCIBLE_MOON_Camera_State1_StartAtSync1, game::MAG_088_INVINCIBLE_MOON_Camera_State2_Chain, game::MAG_088_INVINCIBLE_MOON_Camera_State3_ChainRandom, game::MAG_088_INVINCIBLE_MOON_Camera_State4_Chain, game::MAG_088_INVINCIBLE_MOON_Camera_State5_Chain, game::MAG_088_INVINCIBLE_MOON_Camera_State6_Chain, game::MAG_088_INVINCIBLE_MOON_Camera_State7_PartyCentreAtSync4, game::MAG_088_INVINCIBLE_MOON_Camera_State8_PartyCentreChain, game::MAG_088_INVINCIBLE_MOON_Camera_State9_StartAtSync5, game::MAG_088_INVINCIBLE_MOON_Camera_State10_Chain, game::MAG_088_INVINCIBLE_MOON_Camera_State11_Chain, game::MAG_088_INVINCIBLE_MOON_Camera_State12_StepUntilDone, game::MAG_088_INVINCIBLE_MOON_Director_State18_WaitDirGate6_769C10, game::MAG_088_INVINCIBLE_MOON_Camera_State13_End, game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_769C60, {}, {}, {}, {}, {}, game::MAG_088_INVINCIBLE_MOON_ModelList_FlatTri, game::MAG_088_INVINCIBLE_MOON_ModelList_FlatQuad, game::MAG_088_INVINCIBLE_MOON_PrimList_TexTri_76DA70, game::MAG_088_INVINCIBLE_MOON_PrimList_TexQuad_76DCB0, game::MAG_088_INVINCIBLE_MOON_ModelList_GouraudTri, game::MAG_088_INVINCIBLE_MOON_ModelList_GouraudQuad, game::MAG_088_INVINCIBLE_MOON_ModelList_GouraudTexTri, game::MAG_088_INVINCIBLE_MOON_ModelList_GouraudTexQuad, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_088_INVINCIBLE_MOON_Sequence_State0_Advance_768820, game::MAG_088_INVINCIBLE_MOON_Sequence_State0_Advance_768830, game::MAG_088_INVINCIBLE_MOON_Master_State2_CarvePools, game::MAG_088_INVINCIBLE_MOON_Sequence_State3_SpawnEmitter, game::MAG_088_INVINCIBLE_MOON_EmitterTask, game::MAG_088_INVINCIBLE_MOON_Master_State2_InitDirector, {}, game::MAG_088_INVINCIBLE_MOON_LoadCallback_QueueTIM, game::MAG_088_INVINCIBLE_MOON_LoadCb_SummonStream, game::MAG_088_INVINCIBLE_MOON_LoadCb_ClearBusy_768D20, game::MAG_088_INVINCIBLE_MOON_LoadCb_ClearBusy_768D30, game::MAG_088_INVINCIBLE_MOON_LoadCb_ClearBusy_768D40, game::MAG_088_INVINCIBLE_MOON_LoadCb_ClearBusy_768D50, game::MAG_088_INVINCIBLE_MOON_LoadScript_RecordFile, {}, {}, game::MAG_088_INVINCIBLE_MOON_LoadScript_TryArmStep, {}, {}, game::MAG_088_INVINCIBLE_MOON_Camera_StepScript, game::MAG_088_INVINCIBLE_MOON_Camera_WriteBattleCamera, {}, {}, {}, game::MAG_088_INVINCIBLE_MOON_DirSyncGate, game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_769C50, {}, game::MAG_088_INVINCIBLE_MOON_Play_ObjectCallback, {}, {}, {}, {}, {}, game::MAG_088_INVINCIBLE_MOON_ResetTrailHistory, game::MAG_088_INVINCIBLE_MOON_Director_StartPositions, game::MAG_088_INVINCIBLE_MOON_FillTrailRows, game::MAG_088_INVINCIBLE_MOON_Director_Mode3Setup, game::MAG_088_INVINCIBLE_MOON_Director_TargetPositions, game::MAG_088_INVINCIBLE_MOON_RelocateActorData, {}, {}, game::MAG_088_INVINCIBLE_MOON_DampValue, game::MAG_088_INVINCIBLE_MOON_DampVector, {}, {}, game::MAG_088_INVINCIBLE_MOON_MorphVertexBlock, {}, {}, {}, game::MAG_088_RandRange, game::MAG_088_INVINCIBLE_MOON_RandRangeWide, game::MAG_088_RandAngleJitter, game::MAG_088_INVINCIBLE_MOON_Particle_AttachToOwner, game::MAG_088_INVINCIBLE_MOON_Particle_KeyPositionPath, {}, game::MAG_088_INVINCIBLE_MOON_ParticleList_Unlink, game::MAG_088_INVINCIBLE_MOON_Particle_PlaceAtAnchorSet, game::MAG_088_INVINCIBLE_MOON_Particle_PlaceAtPoint, game::MAG_088_INVINCIBLE_MOON_Sequence_DrawSpriteStrip, {}, game::MAG_088_INVINCIBLE_MOON_Emitter_State1_ApplyActionResults, game::MAG_088_INVINCIBLE_MOON_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_088_INVINCIBLE_MOON_Emitter_State3_FinishReleaseRoot, game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_773770, game::MAG_088_INVINCIBLE_MOON_Sequence_State4_NextTargetLoop, game::MAG_088_INVINCIBLE_MOON_Sequence_State0_Advance_7737C0, game::MAG_088_INVINCIBLE_MOON_Sequence_State6_WaitQueuesEmpty, game::MAG_088_INVINCIBLE_MOON_Sequence_State7_RequestBattleSync, game::MAG_088_INVINCIBLE_MOON_Sequence_State8_WaitBattleSyncDone, game::MAG_088_INVINCIBLE_MOON_Sequence_State9_Finish, game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_773840 } };

	static constexpr game::Id DIR_PTR = game::unk_154ECE0;
	static constexpr game::Id PHASE_DATA = DIR_PTR;
	static constexpr game::Id PACKET_CURSOR = game::unk_25A4D9C;
	static constexpr game::Id SCRATCH_SP = game::unk_25ACD0C;
	static constexpr game::Id ACTOR_STATE = game::unk_25ACD3C;
	static constexpr game::Id FLASH_PAUSE = game::unk_25ACD34;
	static constexpr game::Id CREATURE_ARG = game::unk_25AB8B8;
	static constexpr game::Id STAGE_LIGHT = game::unk_1D98992;
	static inline uint32_t ENT(uint32_t slot) { return game_addr(game::BattleEntitySlotData) + slot * 0x9C; }
	static inline void b3_next_state(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }
	static inline void b103_next_state(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }

	namespace gx
	{
		static inline uint32_t BindEntityModelFromComData(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(a1, a2, a3); }
		static inline uint32_t QueueChainTransformation(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::QueueChainTransformation))(a1, a2); }
		static inline uint32_t BS_Camera_ArmReturnAfterEffect() { return fn<uint32_t (__cdecl *)()>(game_addr(game::BS_Camera_ArmReturnAfterEffect))(); }
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

	uint32_t __cdecl im_768690(uint32_t a1);
	uint32_t __cdecl im_768840(uint32_t a1);
	uint32_t __cdecl im_768880();
	uint32_t __cdecl im_768970(uint32_t a1);
	uint32_t __cdecl im_7689C0();
	uint32_t __cdecl im_768E20(uint32_t a1);
	uint32_t __cdecl im_768ED0(uint32_t a1);
	uint32_t __cdecl im_768F90(uint32_t a1);
	uint32_t __cdecl im_768FE0(uint32_t a1);
	uint32_t __cdecl im_769120(uint32_t a1);
	uint32_t __cdecl im_769150(uint32_t a1);
	uint32_t __cdecl im_769950(uint32_t a1);
	uint32_t __cdecl im_769980(uint32_t a1);
	uint32_t __cdecl im_7699C0(uint32_t a1);
	uint32_t __cdecl im_7699F0(uint32_t a1);
	uint32_t __cdecl im_769A20(uint32_t a1);
	uint32_t __cdecl im_769A50(uint32_t a1);
	uint32_t __cdecl im_769A90(uint32_t a1);
	uint32_t __cdecl im_769B20(uint32_t a1);
	uint32_t __cdecl im_769B60(uint32_t a1);
	uint32_t __cdecl im_769B90(uint32_t a1);
	uint32_t __cdecl im_769BC0(uint32_t a1);
	uint32_t __cdecl im_769C10(uint32_t a1);
	uint32_t __cdecl im_769C70(uint32_t a1);
	uint32_t __cdecl im_769D90(uint32_t a1);
	uint32_t __cdecl im_769DD0(void);
	uint32_t __cdecl im_769E10(uint32_t a1);
	uint32_t __cdecl im_769E40(uint32_t a1);
	uint32_t __cdecl im_769E90(void);
	uint32_t __cdecl im_769ED0(uint32_t a1);
	uint32_t __cdecl im_769EF0(uint32_t a1);
	uint32_t __cdecl im_769F20(uint32_t a1);
	uint32_t __cdecl im_769F80(uint32_t a1);
	uint32_t __cdecl im_769FA0(uint32_t a1);
	uint32_t __cdecl im_769FC0(uint32_t a1);
	uint32_t __cdecl im_76A000(uint32_t a1);
	uint32_t __cdecl im_76A130(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl im_76A180(uint32_t a1);
	uint32_t __cdecl im_76A1F0(uint32_t a1);
	uint32_t __cdecl im_76A380(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl im_76A650(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl im_76A780(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl im_76A9A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl im_76AC30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl im_76B0B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl im_76BB60(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl im_76C060(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl im_76C6B0(uint32_t a1);
	uint32_t __cdecl im_76C760(uint32_t a1);
	uint32_t __cdecl im_76C940(uint32_t a1);
	uint32_t __cdecl im_76CBC0(void);
	uint32_t __cdecl im_76D270(uint32_t a1);
	uint32_t __cdecl im_76D300(void);
	uint32_t __cdecl im_76D360(uint32_t a1, uint32_t a2);
	uint32_t __cdecl im_76D5E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl im_76D7F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl im_76DF50(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl im_76E180(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl im_76E440(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl im_76E6A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl im_76E980(void);
	uint32_t __cdecl im_76EA40(uint32_t a1);
	uint32_t __cdecl im_76EAC0(void);
	uint32_t __cdecl im_76EB00(uint32_t a1);
	uint32_t __cdecl im_76FB30(uint32_t a1, uint32_t a2);
	uint32_t __cdecl im_76FC40(uint32_t a1, uint32_t a2);
	uint32_t __cdecl im_76FCC0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl im_76FCF0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl im_76FD90(uint32_t a1);
	uint32_t __cdecl im_76FE10(uint32_t a1);
	uint32_t __cdecl im_76FE90(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl im_770670(uint32_t a1, uint32_t a2);
	uint32_t __cdecl im_770890(uint32_t a1);
	uint32_t __cdecl im_771380(uint32_t a1, uint32_t a2);
	uint32_t __cdecl im_771410(void);
	uint32_t __cdecl im_7714A0(uint32_t a1);
	uint32_t __cdecl im_7714D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl im_7715C0(uint32_t a1);
	uint32_t __cdecl im_771600(uint32_t a1);
	uint32_t __cdecl im_771610(uint32_t a1);
	uint32_t __cdecl im_771660(uint32_t a1);
	uint32_t __cdecl im_7716C0(uint32_t a1);
	uint32_t __cdecl im_7717A0(uint32_t a1);
	uint32_t __cdecl im_771880(uint32_t a1);
	uint32_t __cdecl im_7718D0(uint32_t a1);
	uint32_t __cdecl im_771B80(uint32_t a1);
	uint32_t __cdecl im_771BD0(uint32_t a1);
	uint32_t __cdecl im_771C00(uint32_t a1);
	uint32_t __cdecl im_771C30(uint32_t a1);
	uint32_t __cdecl im_771C60(uint32_t a1);
	uint32_t __cdecl im_771C90(uint32_t a1);
	uint32_t __cdecl im_771CC0(uint32_t a1);
	uint32_t __cdecl im_771CF0(uint32_t a1);
	uint32_t __cdecl im_771D20(uint32_t a1);
	uint32_t __cdecl im_771D50(uint32_t a1);
	uint32_t __cdecl im_771D70(uint32_t a1);
	uint32_t __cdecl im_771DB0(uint32_t a1);
	uint32_t __cdecl im_771F50(uint32_t a1);
	uint32_t __cdecl im_772060(uint32_t a1);
	uint32_t __cdecl im_7720C0(uint32_t a1);
	uint32_t __cdecl im_772790(uint32_t a1);
	uint32_t __cdecl im_7727B0(uint32_t a1);
	uint32_t __cdecl im_772880(uint32_t a1);
	uint32_t __cdecl im_772990(uint32_t a1);
	uint32_t __cdecl im_7729F0(uint32_t a1);
	uint32_t __cdecl im_772A10(uint32_t a1);
	uint32_t __cdecl im_772AB0(uint32_t a1);
	uint32_t __cdecl im_772B40(uint32_t a1);
	uint32_t __cdecl im_772B80(uint32_t a1);
	uint32_t __cdecl im_772BE0(uint32_t a1);
	uint32_t __cdecl im_772CB0(uint32_t a1);
	uint32_t __cdecl im_772D10(uint32_t a1);
	uint32_t __cdecl im_772D40(uint32_t a1);
	uint32_t __cdecl im_772D90(uint32_t a1);
	uint32_t __cdecl im_772E30(uint32_t a1);
	uint32_t __cdecl im_772F50(uint32_t a1);
	uint32_t __cdecl im_772FB0(uint32_t a1);
	uint32_t __cdecl im_772FE0(uint32_t a1);
	uint32_t __cdecl im_773010(uint32_t a1);
	uint32_t __cdecl im_773040(uint32_t a1);
	uint32_t __cdecl im_773090(uint32_t a1);
	uint32_t __cdecl im_773100(uint32_t a1);
	uint32_t __cdecl im_773130(uint32_t a1);
	uint32_t __cdecl im_7732F0(uint32_t a1);
	uint32_t __cdecl im_773320(uint32_t a1);
	uint32_t __cdecl im_7733B0(uint32_t a1);
	uint32_t __cdecl im_773400(uint32_t a1, uint32_t a2);
	uint32_t __cdecl im_773520(uint32_t a1);
	uint32_t __cdecl im_773560(uint32_t a1);
	uint32_t __cdecl im_773590(uint32_t a1);
	uint32_t __cdecl im_773600(uint32_t a1);
	uint32_t __cdecl im_773630(uint32_t a1);
	uint32_t __cdecl im_773670(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace moon88
{
	uint32_t __cdecl im_768690(uint32_t a1)
	{
		g_mod = &MOD_088;

		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		MEM<uint32_t>(G(G_258FB68)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(G(G_257F9B0)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		uint32_t states[11] = {
			F(F_73A0D0), F(F_73A0E0), F(F_73A0F0), F(F_73A170), F(F_747500), F(F_747540),
			F(F_747550), F(F_747560), F(F_747590), F(F_7475A0), F(F_7475C0),
		};
		if (U8(a1, 0x5C) & 1)
		{
			const uint32_t v1 = MEM<uint32_t>(G(G_258BFA4));
			const uint32_t v2 = MEM<uint32_t>(G(G_258A024));
			MEM<uint32_t>(G(G_257F8A4)) = v1;
			MEM<uint32_t>(G(G_257F9AC)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(G(G_258BFA0));
			const uint32_t v2 = MEM<uint32_t>(G(G_258A020));
			MEM<uint32_t>(G(G_257F8A4)) = v1;
			MEM<uint32_t>(G(G_257F9AC)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(a1);
		callp(states[S8(a1, 0x29)], a1);

		U16(a1, 0x5E) = 0;
		U16(G(G_258FB40), 0) = 0;
		U16(G(G_258EC50), 0) = 0;
		static const int queues[9] = { G_258BE30, G_257F988, G_258FB48, G_258EC40, G_258BA90,
			G_258A010, G_257FA90, G_2585EF0, G_258BE20 };
		for (int i = 0; i < 9; i++)
		{
			const uint32_t n = x::ExecuteTaskQueue(G(queues[i]));
			U16(a1, 0x5E) = (uint16_t)(U16(a1, 0x5E) + (uint16_t)n);
		}
		const uint8_t status = U8(a1, 0x26);
		U16(a1, 0x5C) = (uint16_t)(U16(a1, 0x5C) + 1);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		return task_end(a1, status);
	}

	uint32_t __cdecl im_768840(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::unk_25ACD38));
		MEM<uint32_t>(game_addr(game::unk_25AC6DC)) = p;
		p += 0x870;
		MEM<uint32_t>(game_addr(game::unk_25ACD30)) = p;
		p += 0xB7C0;
		MEM<uint32_t>(game_addr(game::unk_25ACD38)) = p;
		im_768880();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl im_768880()
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::unk_25AC6DC)), 0x870);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::unk_25ACD30)), 0xB7C0);
		MEM<uint16_t>(game_addr(game::unk_25A6BB8)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25AC6E8)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25AC3A4)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25A4DA0)) = 0;
		return 0;
	}

	uint32_t __cdecl im_768970(uint32_t a1)
	{
		im_768E20(a1);
		im_7689C0();
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25ACCF8), game_addr(game::MAG_088_INVINCIBLE_MOON_ActorSystemTask_768AC0), 0x30, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25A4E80), game_addr(game::MAG_088_INVINCIBLE_MOON_DirectorTask), 0x48, a1);
		t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl im_7689C0()
	{
		MEM<uint32_t>(game_addr(game::unk_25AB8B0)) = game_addr(game::unk_154F898);
		MEM<uint32_t>(game_addr(game::unk_25AC3A8)) = game_addr(game::unk_25ACD40);
		x::MAG_007_sub_8DCC00(game_addr(game::unk_25ACD40), 0x16C);
		const uint32_t m = MEM<uint32_t>(game_addr(game::unk_25AC3A8));
		const uint32_t cur = MEM<uint32_t>(game_addr(game::unk_25ACD38));
		U32(m, 0x100) = cur;
		MEM<uint32_t>(game_addr(game::unk_25ACD38)) = cur + 0xA000;
		for (uint32_t o = 0x104; o <= 0x160; o += 4)
			U32(m, o) = game_addr(game::unk_19ED018);
		U32(m, 0x128) = game_addr(game::unk_19F3A8C);
		U32(m, 0x144) = game_addr(game::unk_19EF0BC);
		U32(m, 0x148) = game_addr(game::unk_19F1B5C);
		U16(m, 0x164) = 0;
		U16(m, 0x166) = 0;
		return 0;
	}

	uint32_t __cdecl im_768E20(uint32_t a1)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(DIR_PTR)), 0x54);
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		const uint32_t t = ENT(U8(a1, 0x2D)), c = ENT(U8(a1, 0x2C));
		U32(d, 8) = U32(t, 0x1C);
		U32(d, 0xC) = U32(t, 0x20);
		U32(d, 0) = U32(c, 0x1C);
		U32(d, 4) = U32(c, 0x20);
		const int16_t dz = (int16_t)(U16(d, 0xC) - U16(d, 4));
		const int16_t dx = (int16_t)(U16(d, 8) - U16(d, 0));
		const uint32_t ang = x::CartesianToGameAngle((uint32_t)(int32_t)dx, (uint32_t)(int32_t)dz);
		const uint32_t d2 = MEM<uint32_t>(game_addr(DIR_PTR));
		U16(d2, 0x4A) = (uint16_t)((ang - 0x800) & 0xFFF);
		U16(d2, 0x4E) = U16(ENT(U8(a1, 0x2D)), 0x26);
		return 0;
	}

	uint32_t __cdecl im_768ED0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t states[15] = { game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State0_UploadTIMs_SpawnCamera), game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State1_Cue1), game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State2_ArmLoadScriptStep), game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State3_WaitIdle), game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State5_WaitDirGate2), game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State5_Cue3_HideParty),
			game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State6_Sync4), game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State7_Cue4_ShowParty), game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State8_DoneAtFrame23), game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State9_Sync5), game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State10_ShowMonsters), game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State11_WaitFrame14), game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State18_WaitDirGate6_769FA0), game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State13_FinishAtCue6),
			game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_769FE0) };
		im_768F90(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl im_768F90(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			im_76A000(a1);
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

	uint32_t __cdecl im_768FE0(uint32_t a1)
	{
		x::Battle_QueueTIMUpload_GetEOF(game_addr(game::MAG088_Angelo_BodyTIM));
		x::Battle_QueueTIMUpload_GetEOF(game_addr(game::unk_16FB420));
		gx::BS_Camera_ArmReturnAfterEffect();
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25A4F88), game_addr(game::MAG_088_INVINCIBLE_MOON_CameraTask), 0x90, a1);
		if (a_73B640(1) != 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static inline void im_cam_chain(uint32_t a1, uint32_t script)
	{
		if (a_73AE10() == 1)
		{
			a_73AB20(script, MEM<uint32_t>(game_addr(DIR_PTR)), 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
	}

	uint32_t __cdecl im_769120(uint32_t a1)
	{
		a_73AB00();
		t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl im_769150(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(1) != 0)
		{
			a_73AB20(game_addr(game::unk_1555D60), MEM<uint32_t>(game_addr(DIR_PTR)), 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_769950(uint32_t a1) { im_cam_chain(a1, game_addr(game::unk_1555DA8)); return 0; }

	uint32_t __cdecl im_769980(uint32_t a1)
	{
		if (a_73AE10() == 1)
		{
			const uint32_t r = x::CrtRand();
			a_73AB20((r & 1) ? game_addr(game::unk_1555DF0) : game_addr(game::unk_1555E38), MEM<uint32_t>(game_addr(DIR_PTR)), 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_7699C0(uint32_t a1) { im_cam_chain(a1, game_addr(game::unk_1555E80)); return 0; }

	uint32_t __cdecl im_7699F0(uint32_t a1) { im_cam_chain(a1, game_addr(game::unk_1555EC8)); return 0; }

	uint32_t __cdecl im_769A20(uint32_t a1)
	{
		a_73AE10();
		uint32_t r = a_73AAE0(3);
		if (r == 0) return 0;
		uint32_t blk = U32(game_addr(game::unk_154ECE0), 0);
		a_73AB20(game_addr(game::unk_1555F10), blk, 0);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl im_769A50(uint32_t a1)
	{
		alignas(4) uint8_t pt[8] = {};
		a_73AE10();
		if (a_73AAE0(4) != 0)
		{
			im_769A90(P(pt));
			a_73AB20(game_addr(game::unk_1555F58), P(pt), 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_769A90(uint32_t a1)
	{
		uint32_t count = 0;
		int16_t max_z = 0, min_z = 0, max_x = 0, min_x = 0;
		for (uint32_t e = game_addr(game::unk_1D972E0); e < game_addr(game::unk_1D974B4); e += 0x9C)
		{
			if (!(U8(e, -0x20) & 2))
				continue;
			if ((uint16_t)count == 0)
			{
				max_x = S16(e, -4);
				max_z = S16(e, 0);
				min_x = max_x;
				min_z = max_z;
			}
			else
			{
				const int16_t x = S16(e, -4);
				if (x < min_x) min_x = x;
				else if (x > max_x) max_x = x;
				const int16_t z = S16(e, 0);
				if (z < min_z) min_z = z;
				else if (z > max_z) max_z = z;
			}
			count++;
		}
		const int32_t cx = ((int32_t)max_x + (int32_t)min_x) / 2;
		U16(a1, 2) = 0;
		U16(a1, 0) = (uint16_t)cx;
		const int32_t cz = ((int32_t)max_z + (int32_t)min_z) / 2;
		U16(a1, 4) = (uint16_t)cz;
		return a1;
	}

	uint32_t __cdecl im_769B20(uint32_t a1)
	{
		alignas(4) uint8_t pt[8] = {};
		if (a_73AE10() == 1)
		{
			im_769A90(P(pt));
			a_73AB20(game_addr(game::unk_1555FA0), P(pt), 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_769B60(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(5) != 0)
		{
			a_73AB20(game_addr(game::unk_1555FE8), MEM<uint32_t>(game_addr(DIR_PTR)), 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_769B90(uint32_t a1) { im_cam_chain(a1, game_addr(game::unk_1556030)); return 0; }

	uint32_t __cdecl im_769BC0(uint32_t a1) { im_cam_chain(a1, game_addr(game::unk_1556078)); return 0; }

	uint32_t __cdecl im_769C10(uint32_t a1)
	{
		const uint32_t r = a_73AAE0(6);
		if (r == 0)
			return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl im_769C70(uint32_t a1)
	{
		if (a_73B7E0(1) == 0)
			return 0;
		a_73B9F0();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl im_769D90(uint32_t a1)
	{
		if (a_73B7E0(3) != 0)
		{
			a_746A10();
			im_769DD0();
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_769DD0(void)
	{
		uint32_t save = MEM<uint32_t>(game_addr(game::unk_154ECE0)) + 0x20;
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

	uint32_t __cdecl im_769E10(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x46) >= 0x15 && a_73B640(4) != 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl im_769E40(uint32_t a1)
	{
		if (a_73B7E0(4) != 0)
		{
			a_746AE0();
			im_769E90();
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_769E90(void)
	{
		uint32_t save = MEM<uint32_t>(game_addr(game::unk_154ECE0)) + 0x20;
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

	uint32_t __cdecl im_769ED0(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		if (S16(d, 0x46) >= 0x23)
		{
			U16(d, 0x48) = 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_769EF0(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x46) >= 0x34 && a_73B640(5) != 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl im_769F20(uint32_t a1)
	{
		uint32_t r = a_73B7E0(5);
		if (r == 0) return 0;
		a_73BA90();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl im_769F80(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x46) >= 0x14)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl im_769FA0(uint32_t a1)
	{
		const uint32_t r = a_73B640(6);
		if (r == 0)
			return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl im_769FC0(uint32_t a1)
	{
		if (a_73B7E0(6) != 0)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_76A000(uint32_t a1)
	{
		const int32_t phase = S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x40);
		if (phase == 1)
		{
			im_7714D0(a1, game_addr(game::MAG_088_INVINCIBLE_MOON_AngeloTask), game_addr(game::MAG088_Angelo_ModelContainer), 3);
			im_7714D0(a1, game_addr(game::MAG_088_INVINCIBLE_MOON_DoubleTask), game_addr(game::unk_155563C), 3);
		}
		else if (phase == 3)
		{
			for (uint32_t i = 0; i < 3; i++)
			{
				const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25A6BC0), game_addr(game::MAG_088_INVINCIBLE_MOON_MoonModelTask), 0x9C, a1);
				U16(t, 0x98) = (uint16_t)i;
			}
		}
		else if (phase == 4)
		{
			a_73C100(a1, game_addr(game::MAG_088_INVINCIBLE_MOON_ActorSystemTask_76C700), U32(MEM<uint32_t>(game_addr(game::unk_25AC3A8)), 0x104), 8, 0xF, 1);
			im_76A130(a1, game_addr(game::MAG_088_INVINCIBLE_MOON_MoonPlayTask), U32(MEM<uint32_t>(game_addr(game::unk_25AC3A8)), 0x128), 0x3C4, 1, 0);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25A4F88), game_addr(game::MAG_088_INVINCIBLE_MOON_StageLightTask), 0x90, a1);
		}
		return 0;
	}

	uint32_t __cdecl im_76A130(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25AA9F0), a2, 0x474, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		S32(t, 0x78) = (int32_t)(int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl im_76A180(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_088_INVINCIBLE_MOON_MoonPlay_State0_Setup);
		states[1] = game_addr(game::MAG_088_INVINCIBLE_MOON_MoonPlay_State1_PlayUntilCue5);
		states[2] = game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_76C6F0);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x8E) = (uint16_t)((uint8_t)(U8(node, 0x8E) - 2) & 0x7F);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl im_76A1F0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 4)
			return 0;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		im_769A90(node + 0x1C);
		U16(node, 0x1E) = 0x100;
		S32(node, 0x58) = 0x1000;
		S32(node, 0x54) = 0x1000;
		S32(node, 0x50) = 0x1000;
		U16(node, 0x88) = 0xF800;
		a_73C280(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl im_76A380(uint32_t a1, uint32_t a2, uint32_t a3)
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
			if ((U32(rec, 4) & 0x100) != 0)
			{
				for (uint32_t o = 0; o < 0x12; o += 2) U16(S, o) = 0;
				U16(S, 0) = U16(rec, 0x18);
				U16(S, 8) = U16(rec, 0x1A);
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
		U32(hdr, 0x14) = (U32(rec, 4) & 0x4000) != 0 ? 0x2000 : 0x2030;
		const int32_t alpha = S16(rec, 0x24);
		U32(hdr, 0xC) = (uint32_t)alpha;
		if (alpha != 0)
		{
			U32(hdr, 8) = U32(rec, 0x20);
			U8(hdr, 0x14) = (uint8_t)(U8(hdr, 0x14) | 0xC0);
		}
		U32(hdr, 0x10) = (uint32_t)(int32_t)S16(blk, 0x58);
		U16(hdr, 0x18) = 0;
		U16(hdr, 0x1E) = 0;
		U16(hdr, 0x1C) = 0;
		U16(hdr, 0x22) = 0x100;
		U16(hdr, 0x20) = 0x100;
		U16(hdr, 0x26) = 0;
		U16(hdr, 0x24) = 0;
		if (U16(blk, 0x56) == 1 && U16(rec, 2) == 0)
		{
			U16(hdr, 0x1A) = U16(blk, 0x4E);
			U16(hdr, 0x28) = 0x80;
			U16(hdr, 0x2A) = 0x80;
		}
		else
		{
			U16(hdr, 0x1A) = 0;
			U16(hdr, 0x2A) = 0x100;
			U16(hdr, 0x28) = 0x100;
		}
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = im_76A650(hdr, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl im_76A650(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
			cur = im_76A780(ctx, a2, a3, a4);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = im_76A9A0(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = im_76AC30(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = im_76B0B0(ctx, a2, a3, cur);
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
			cur = im_76BB60(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			return im_76C060(ctx, a2, a3, cur);
		U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		return cur;
	}

	uint32_t __cdecl im_76A780(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl im_76A9A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl im_76AC30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl im_76B0B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl im_76BB60(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl im_76C060(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl im_76C6B0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_73B7E0(5) == 0)
		{
			if (a_73C280(node) != 0)
				return 0;
		}
		U8(node, 0x26) = (uint8_t)(U8(node, 0x26) | 1);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl im_76C760(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			a_73F990(a1);
			im_76D270(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_76C940(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::unk_25ACD3C));
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

	uint32_t __cdecl im_76CBC0(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::unk_25ACD3C));
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

	uint32_t __cdecl im_76D270(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::unk_25ACD3C), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			im_771410();
			im_76EAC0();
			im_76E980();
			im_76D300();
			sys = U32(game_addr(game::unk_25ACD3C), 0);
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
		U16(game_addr(game::unk_25ACCF0), 0) = (uint16_t)(U16(game_addr(game::unk_25ACCF0), 0) + c14);
		U16(game_addr(game::unk_25AC6D8), 0) = (uint16_t)(U16(game_addr(game::unk_25AC6D8), 0) + c16);
		return done;
	}

	uint32_t __cdecl im_76D300(void)
	{
		uint32_t act = U32(U32(game_addr(game::unk_25ACD3C), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::unk_25ACD3C), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					im_76D360(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl im_76D360(uint32_t a1, uint32_t a2)
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

	uint32_t __cdecl im_76D5E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl im_76D7F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl im_76DF50(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl im_76E180(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl im_76E440(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl im_76E6A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl im_76E980(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::unk_25ACD3C)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::unk_25ACD3C));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						im_76EA40(act);
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
							im_76EA40(act);
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

	uint32_t __cdecl im_76EA40(uint32_t a1)
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

	uint32_t __cdecl im_76EAC0(void)
	{
		uint32_t act = U32(U32(game_addr(game::unk_25ACD3C), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				im_76FD90(act);
			else if (type == 1)
				im_76EB00(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl im_76EB00(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::unk_25ACD3C));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (im_76FC40(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::unk_25ACD3C));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl im_76FB30(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::unk_25ACD3C), 0);
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

	uint32_t __cdecl im_76FC40(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				im_76FCC0(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			im_76FCC0(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			im_76FCC0(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl im_76FCC0(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return im_76FCF0(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl im_76FCF0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::unk_25AC6DC));
		int32_t idx = MEM<int16_t>(game_addr(game::unk_25A6BB8));
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
			MEM<uint16_t>(game_addr(game::unk_25A6BB8)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::unk_25A6BB8)) = 0;
		return slot;
	}

	uint32_t __cdecl im_76FD90(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_088_INVINCIBLE_MOON_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_088_INVINCIBLE_MOON_Particle_KeyPositionPath), a1);
		im_76FE10(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl im_76FE10(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::unk_25ACD3C), 0);
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
				im_76FE90(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			im_76FE90(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl im_76FE90(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::unk_25ACD0C), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::unk_25ACD0C), 0) = sp;
		uint32_t node = im_770670(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::unk_25ACD3C), 0);
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
		im_771380(node, desc);
		U32(game_addr(game::unk_25ACD0C), 0) = U32(game_addr(game::unk_25ACD0C), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl im_770670(uint32_t a1, uint32_t a2)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::unk_25ACD30));
		int32_t i = MEM<int16_t>(game_addr(game::unk_25AC6E8));
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
			MEM<uint16_t>(game_addr(game::unk_25AC6E8)) = 0;
		else
			MEM<uint16_t>(game_addr(game::unk_25AC6E8)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl im_770890(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::unk_25ACD3C), 0);
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
				uint32_t g = U32(game_addr(game::unk_25ACD3C), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::unk_25ACD3C), 0);
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

	uint32_t __cdecl im_771380(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			im_76FCF0(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			im_76FCF0(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			im_76FCF0(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			im_76FCF0(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl im_771410(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::unk_25ACD3C));
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
					im_76FCF0(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::unk_25ACD3C));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				im_76FCF0(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::unk_25ACD3C));
			}
		}
		return 0;
	}

	uint32_t __cdecl im_7714A0(uint32_t a1)
	{
		if (im_76D270(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_7714D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25AC6C8), a2, 0x160, a1);
		x::Effect_BindModelContainerSetAnim(t, a3, a4);
		return t;
	}

	uint32_t __cdecl im_7715C0(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0x180);
		if (S16(a1, 0x1C) >= 0xA00)
		{
			U16(a1, 0x1C) = 0xA00;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		const uint16_t v = U16(a1, 0x1C);
		for (uint32_t k = 0; k < 4; k++)
			MEM<uint16_t>(game_addr(STAGE_LIGHT) + k * 0x2C) = v;
		return 0;
	}

	uint32_t __cdecl im_771600(uint32_t a1)
	{
		if (S16(a1, 0x24) > 0x1E)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl im_771610(uint32_t a1)
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

	uint32_t __cdecl im_771660(uint32_t a1)
	{
		const uint32_t states[3] = { game_addr(game::MAG_088_INVINCIBLE_MOON_MoonModel_State0_Setup), game_addr(game::MAG_088_INVINCIBLE_MOON_MoonModel_State1_DriftUntilCue4), game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_7718C0) };
		callp(states[S8(a1, 0x29)], a1);
		im_7716C0(a1);
		return task_end(a1);
	}

	uint32_t __cdecl im_7716C0(uint32_t a1)
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
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = im_76A650(hdr, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl im_7717A0(uint32_t a1)
	{
		switch (S16(a1, 0x98))
		{
		case 0:
			U16(a1, 0x52) = 0x800;
			U32(a1, 0x4C) = U32(MEM<uint32_t>(game_addr(game::unk_25AC3A8)), 0x148);
			U16(a1, 0x20) = (uint16_t)(U16(MEM<uint32_t>(game_addr(game::unk_154F918)), 0xE0) << 1);
			break;
		case 1:
			U32(a1, 0x4C) = game_addr(game::unk_154EF74);
			U16(a1, 0x52) = 0x700;
			U16(a1, 0x1C) = 0x60;
			U16(a1, 0x1E) = 0xFFA0;
			U16(a1, 0x20) = (uint16_t)(U16(MEM<uint32_t>(game_addr(game::unk_154F918)), 0xE0) << 1);
			U16(a1, 0x7C) = 0x14;
			break;
		case 2:
			U32(a1, 0x4C) = game_addr(game::unk_154F404);
			U16(a1, 0x52) = 0x600;
			U16(a1, 0x1C) = 0xFFA0;
			U16(a1, 0x1E) = 0x60;
			U16(a1, 0x20) = (uint16_t)(U16(MEM<uint32_t>(game_addr(game::unk_154F918)), 0xE0) << 1);
			U16(a1, 0x7C) = 0xFFE0;
			break;
		default:
			break;
		}
		U16(a1, 0x9A) = (uint16_t)(U16(a1, 0x1C) << 4);
		S32(a1, 0x34) = 0x1000;
		S32(a1, 0x30) = 0x1000;
		S32(a1, 0x38) = 0x1000;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl im_771880(uint32_t a1)
	{
		U16(a1, 0x9A) = (uint16_t)(U16(a1, 0x9A) + U16(a1, 0x7C));
		U16(a1, 0x1C) = (uint16_t)(S16(a1, 0x9A) / 16);
		if (a_73B7E0(4) != 0)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 5);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_7718D0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[12];
		states[0] = game_addr(game::MAG_088_INVINCIBLE_MOON_Double_State0_PlaceHidden);
		states[1] = game_addr(game::MAG_088_INVINCIBLE_MOON_Double_State1_ShowAtFrameC);
		states[2] = game_addr(game::MAG_088_INVINCIBLE_MOON_Double_State2_Anim1);
		states[3] = game_addr(game::MAG_088_INVINCIBLE_MOON_Double_State3_Anim2);
		states[4] = game_addr(game::MAG_088_INVINCIBLE_MOON_Double_State4_Anim5);
		states[5] = game_addr(game::MAG_088_INVINCIBLE_MOON_Double_State5_Cue3);
		states[6] = game_addr(game::MAG_088_INVINCIBLE_MOON_Double_State6_Anim3);
		states[7] = game_addr(game::MAG_088_INVINCIBLE_MOON_Double_State7_HideAtCue4);
		states[8] = game_addr(game::MAG_088_INVINCIBLE_MOON_Double_State8_ShowAtCue5);
		states[9] = game_addr(game::MAG_088_INVINCIBLE_MOON_Double_State9_WaitAnimEnd);
		states[10] = game_addr(game::MAG_088_INVINCIBLE_MOON_Double_State10_Finish);
		states[11] = game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_771DA0);
		callp(states[S8(node, 0x29)], node);
		if ((U8(node, 0x26) & 4) == 0)
		{
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = a_746C10(node, game_addr(CREATURE_ARG), MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl im_771B80(uint32_t a1)
	{
		const uint32_t e = ENT(U8(a1, 0x2C));
		U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 4);
		U32(a1, 0x4C) = U32(e, 0x1C);
		U32(a1, 0x11C) = 0x1000;
		U32(a1, 0x118) = 0x1000;
		U32(a1, 0x114) = 0x1000;
		U32(a1, 0x50) = U32(e, 0x20);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl im_771BD0(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0xC)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) & 0xFB);
			x::au_re_Battle_ReadAnimation_7(a1, 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_771C00(uint32_t a1)
	{
		if (x::au_re_Battle_ReadAnimation_8(a1) == 1)
		{
			U16(a1, 0x128) = 0;
			x::au_re_Battle_ReadAnimation_7(a1, 1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_771C30(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		if (S16(a1, 0x136) >= 0x14)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 2);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_771C60(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t r = x::au_re_Battle_ReadAnimation_8(node);
		if (r != 1)
			return r;
		x::au_re_Battle_ReadAnimation_7(node, 5);
		b3_next_state(node);
		return 0;
	}

	uint32_t __cdecl im_771C90(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		if (a_73B7E0(3) != 0)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 5);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_771CC0(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		if (S16(a1, 0x136) >= 6)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 3);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_771CF0(uint32_t a1)
	{
		x::au_re_Battle_ReadAnimation_8(a1);
		if (a_73B7E0(4) != 0)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 4);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_771D20(uint32_t a1)
	{
		if (a_73B7E0(5) != 0)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) & 0xFB);
			x::au_re_Battle_ReadAnimation_7(a1, 4);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_771D50(uint32_t a1)
	{
		if (x::au_re_Battle_ReadAnimation_8(a1) == 1)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl im_771D70(uint32_t a1)
	{
		if (x::au_re_Battle_ReadAnimation_8(a1) == 1)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 5);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_771DB0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[17];
		states[0] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State0_BindRinoaAnimSet);
		states[1] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State1_RinoaChain2);
		states[2] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State2_Anim1);
		states[3] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State3_SpawnHowlPlay);
		states[4] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State4_SpawnHalo);
		states[5] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State5_Anim8);
		states[6] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State6_SpawnFlash);
		states[7] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State7_Countdown);
		states[8] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State8_WaitSync3_Pause);
		states[9] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State9_Cue3_Glow);
		states[10] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State10_Anim4_Glow);
		states[11] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State11_HideAtCue4);
		states[12] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State12_ShowAtCue5_RinoaChain4);
		states[13] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State13_Anim6);
		states[14] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State14_Anim7_WalkOff);
		states[15] = game_addr(game::MAG_088_INVINCIBLE_MOON_Angelo_State15_Finish);
		states[16] = game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_7736B0);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x50) = (uint16_t)(U16(node, 0x50) + U16(node, 0x128));
		U16(node, 0x4C) = (uint16_t)(U16(node, 0x4C) + U16(node, 0x124));
		U16(node, 0x4E) = (uint16_t)(U16(node, 0x4E) + U16(node, 0x126));
		if ((U8(node, 0x26) & 4) == 0)
		{
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = a_746C10(node, game_addr(CREATURE_ARG), MEM<uint32_t>(game_addr(PACKET_CURSOR)));
			if ((U8(node, 0x26) & 4) == 0)
			{
				const uint8_t anim = U8(node, 0x9C), fr = U8(node, 0xA2);
				if (anim == 1)
				{
					if (fr >= 0xC && fr <= 0x11)
						im_771F50(node);
				}
				else if (anim == 3)
				{
					if (fr >= 2 && fr <= 5)
						im_772880(node);
				}
				else if (anim == 5)
				{
					if (fr >= 2 && fr <= 7)
						im_771F50(node);
				}
			}
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

	static inline void im_dust_rings(uint32_t a1, int n, uint32_t f, uint32_t mask)
	{
		alignas(4) uint8_t m[0x20];
		alignas(4) uint8_t v[8] = {};
		const uint32_t M = P(m), Vp = P(v);
		for (int i = 0; i < n; i++)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25A4F88), f, 0x90, a1);
			x::GetEffectSpawnPosition(a1 + 0x30, 0xF1, 0, t + 0x1C);
			U16(t, 0x1E) = 0;
			const uint32_t r1 = x::CrtRand() & 0xFFF;
			x::MAG_022_sub_8DD770(M);
			x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)(int16_t)r1);
			U16(Vp, 0) = 0;
			U16(Vp, 2) = 0;
			U16(Vp, 4) = (uint16_t)((x::CrtRand() & 0xFF) + 0x40);
			x::matrixMultiplyVector(M, Vp, Vp);
			U16(t, 0x1C) = (uint16_t)(U16(t, 0x1C) + U16(Vp, 0));
			U16(t, 0x20) = (uint16_t)(U16(t, 0x20) + U16(Vp, 4));
			const int32_t r4 = (int32_t)x::CrtRand();
			U16(t, 0x78) = 0;
			U16(t, 0x7A) = 0;
			U16(t, 0x82) = (uint16_t)(-(r4 % 0x60));
			U16(t, 0x7C) = (uint16_t)((x::CrtRand() & mask) + 0x200);
			x::matrixMultiplyVector(M, t + 0x78, t + 0x78);
		}
	}

	uint32_t __cdecl im_771F50(uint32_t a1) { im_dust_rings(a1, 4, game_addr(game::MAG_088_INVINCIBLE_MOON_DustRingTask), 0x3FF); return 0; }

	uint32_t __cdecl im_772060(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_088_INVINCIBLE_MOON_DustRing_State0_Setup);
		states[1] = game_addr(game::MAG_088_INVINCIBLE_MOON_DustRing_State1_Rise);
		states[2] = game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_772870);
		callp(states[S8(node, 0x29)], node);
		im_7720C0(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl im_7720C0(uint32_t a1)
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
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::unk_25A4D9C));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(blk, 0x24) = 0;
		U16(blk, 0xB4) = colour;
		MEM<uint32_t>(game_addr(game::unk_25A4D9C)) = a_7435E0(blk, ot, 2, cursor);
		x::Field_Free(0xD8);
		return 0;
	}

	uint32_t __cdecl im_772790(uint32_t a1)
	{
		U32(a1, 0x4C) = game_addr(game::unk_154ECEC);
		U16(a1, 0x56) = 0xFFC0;
		U16(a1, 0x52) = 7;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl im_7727B0(uint32_t a1)
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

	uint32_t __cdecl im_772880(uint32_t a1) { im_dust_rings(a1, 3, game_addr(game::MAG_088_INVINCIBLE_MOON_DustRingFastTask), 0x7FF); return 0; }

	uint32_t __cdecl im_772990(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_088_INVINCIBLE_MOON_DustRingFast_State0_Setup);
		states[1] = game_addr(game::MAG_088_INVINCIBLE_MOON_DustRingFast_State1_Rise);
		states[2] = game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_772AA0);
		callp(states[S8(node, 0x29)], node);
		im_7720C0(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl im_7729F0(uint32_t a1)
	{
		U32(a1, 0x4C) = game_addr(game::unk_154EE30);
		U16(a1, 0x56) = 0xFFC0;
		U16(a1, 0x52) = 7;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl im_772A10(uint32_t a1)
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

	uint32_t __cdecl im_772AB0(uint32_t a1)
	{
		const uint32_t e = ENT(U8(a1, 0x2C));
		U32(MEM<uint32_t>(game_addr(DIR_PTR)), 0x18) = a1;
		gx::BindEntityModelFromComData(e, game_addr(game::unk_25ACD10), game_addr(game::MAG_088_INVINCIBLE_MOON_AnimSet));
		gx::QueueChainTransformation(e, 1);
		U32(a1, 0x4C) = U32(e, 0x1C);
		U32(a1, 0x50) = U32(e, 0x20);
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0x1200);
		x::au_re_Battle_ReadAnimation_7(a1, 0);
		U32(a1, 0x11C) = 0x1000;
		U32(a1, 0x118) = 0x1000;
		U32(a1, 0x114) = 0x1000;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		U16(a1, 0x128) = 0xFF00;
		return 0;
	}

	uint32_t __cdecl im_772B40(uint32_t a1)
	{
		const uint32_t e = ENT(U8(a1, 0x2C));
		x::au_re_Battle_ReadAnimation_8(a1);
		if (S16(a1, 0x24) >= 0xB)
		{
			gx::QueueChainTransformation(e, 2);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_772B80(uint32_t a1)
	{
		const uint32_t e = ENT(U8(a1, 0x2C));
		if (x::au_re_Battle_ReadAnimation_8(a1) == 1)
		{
			U16(a1, 0x128) = 0;
			x::au_re_Battle_ReadAnimation_7(a1, 1);
			U32(a1, 0x4C) = U32(e, 0x1C);
			U32(a1, 0x50) = U32(e, 0x20);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_772BE0(uint32_t a1)
	{
		if (S16(a1, 0x136) == 8)
			x::BdPlaySE(game_addr(game::unk_154ECE4), 0, 0x80);
		if (x::au_re_Battle_ReadAnimation_8(a1) == 1)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 2);
			const uint32_t t = im_76A130(a1, game_addr(game::MAG_088_INVINCIBLE_MOON_HowlPlayTask), game_addr(game::unk_16F1F8C), 0xB0, 0, 0);
			U32(t, 0x458) = a1 + 0x30;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_772CB0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t mdl = U32(node, 0x458);
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		x::GetEffectSpawnPosition(mdl, 0xF1, 0, node + 0x1C);
		U16(node, 0x1E) = 0;
		S32(node, 0x58) = 0x1000;
		S32(node, 0x54) = 0x1000;
		S32(node, 0x50) = 0x1000;
		a_73C280(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl im_772D10(uint32_t a1)
	{
		const uint32_t r = a_73C280(a1);
		if (r == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl im_772D40(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		if (S16(a1, 0x136) >= 0x14)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 3);
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25A6BC0), game_addr(game::MAG_088_INVINCIBLE_MOON_HaloTask), 0x9C, a1);
			U32(t, 0x8C) = a1 + 0x30;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_772D90(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_088_INVINCIBLE_MOON_Halo_State0_Setup);
		states[1] = game_addr(game::MAG_088_INVINCIBLE_MOON_Halo_State1_FadeIn);
		states[2] = game_addr(game::MAG_088_INVINCIBLE_MOON_Halo_State2_HideAtCue3);
		states[3] = game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_773000);
		callp(states[S8(node, 0x29)], node);
		alignas(4) uint8_t pt[8] = {};
		U16(node, 0x46) = (uint16_t)((U16(node, 0x46) + 0x100) & 0xFFF);
		x::GetEffectSpawnPosition(U32(node, 0x8C), 0x1D, 0, P(pt));
		S32(node, 0x34) = -(int32_t)S16(P(pt), 2);
		im_772E30(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl im_772E30(uint32_t a1)
	{
		const uint32_t node = a1;
		if ((U8(node, 0x26) & 4) != 0)
			return 0;
		uint32_t mbuf[8] = {};
		const uint32_t m = P(mbuf);
		x::MAG_022_sub_8DD770(m);
		x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(node, 0x46));
		x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(node, 0x44));
		x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0x48));
		S32(m, 0x14) = S16(node, 0x1C);
		S32(m, 0x18) = S16(node, 0x1E);
		S32(m, 0x1C) = S16(node, 0x20);
		x::scale3DMatrix(m, node + 0x30);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), m, m);
		x::GTE_SetRotMatrix_W(m);
		x::GTE_SetTransVector_W(m);
		const uint32_t h = x::Field_Alloc(0x68);
		U32(h, 0) = U32(node, 0x4C);
		U32(h, 8) = U32(node, 0x40);
		S32(h, 0xC) = S16(node, 0x50);
		U16(h, 0x22) = 0x100;
		U16(h, 0x20) = 0x100;
		U16(h, 0x2A) = 0x100;
		U16(h, 0x28) = 0x100;
		S32(h, 0x10) = S16(node, 0x52);
		U16(h, 0x18) = 0;
		U16(h, 0x1A) = 0;
		U16(h, 0x1E) = 0;
		U16(h, 0x1C) = 0;
		U16(h, 0x26) = 0;
		U16(h, 0x24) = 0;
		U32(h, 0x14) = 0xF0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = im_76A650(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl im_772F50(uint32_t a1)
	{
		const uint32_t mdlp = U32(MEM<uint32_t>(game_addr(game::unk_25AC3A8)), 0x144);
		S32(a1, 0x30) = 0x800;
		S32(a1, 0x38) = 0x800;
		U32(a1, 0x4C) = mdlp;
		S32(a1, 0x34) = 0x1000;
		U16(a1, 0x52) = 0xFE00;
		U16(a1, 0x50) = 0x1000;
		x::GetEffectSpawnPosition(U32(a1, 0x8C), 0xF1, 0, a1 + 0x1C);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		U16(a1, 0x1E) = 0;
		return 0;
	}

	uint32_t __cdecl im_772FB0(uint32_t a1)
	{
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0xFC00);
		if (S16(a1, 0x50) <= 0)
		{
			U16(a1, 0x50) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_772FE0(uint32_t a1)
	{
		if (a_73B7E0(3) != 0)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 5);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_773010(uint32_t a1)
	{
		if (x::au_re_Battle_ReadAnimation_8(a1) == 1)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 8);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_773040(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		if (S16(a1, 0x136) >= 6)
		{
			U16(a1, 0x134) = 9;
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25AB728), game_addr(game::MAG_088_INVINCIBLE_MOON_FlashTask), 0x40, a1);
			U32(t, 0x34) = game_addr(game::unk_1557CAC);
			U16(t, 0x38) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_773090(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_088_INVINCIBLE_MOON_Flash_State0_Delay);
		states[1] = game_addr(game::MAG_088_INVINCIBLE_MOON_Flash_State1_StepDraw);
		states[2] = game_addr(game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_7732E0);
		callp(states[S8(node, 0x29)], node);
		if (MEM<uint32_t>(game_addr(FLASH_PAUSE)) == 0)
			U16(node, 0x3C) = (uint16_t)(U16(node, 0x3C) + 1);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl im_773100(uint32_t a1)
	{
		if (MEM<uint32_t>(game_addr(FLASH_PAUSE)) != 0)
			return 0;
		U16(a1, 0x38) = (uint16_t)(U16(a1, 0x38) - 1);
		if (S16(a1, 0x38) <= 0)
		{
			U16(a1, 0x38) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_773130(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint16_t idx = U16(node, 0x3A);
		const uint32_t script = U32(node, 0x34);
		const uint32_t ent = U32(script, (int32_t)(int16_t)idx * 4);
		const uint8_t op = (uint8_t)(ent >> 24), b2 = (uint8_t)(ent >> 16);
		if (op == 0xFF)
		{
			U8(node, 0x26) = (uint8_t)(U8(node, 0x26) | 1);
			return 0;
		}
		if (op == 0xFE)
		{
			if (S16(node, 0x3C) >= (int16_t)(uint16_t)b2 && MEM<uint32_t>(game_addr(FLASH_PAUSE)) == 0)
			{
				const uint16_t ni = (uint16_t)(idx + 1);
				U16(node, 0x3A) = ni;
				U32(node, 0x30) = U32(script, (int32_t)(int16_t)ni * 4);
			}
		}
		else
		{
			U32(node, 0x30) = ent;
			if (MEM<uint32_t>(game_addr(FLASH_PAUSE)) == 0)
				U16(node, 0x3A) = (uint16_t)(idx + 1);
		}
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x1C;
		uint32_t pkt = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		x::sub_45BFC0(pkt, 0, 0, x::sub_45C690(0, 2, 0x280, 0) & 0xFFFF, 0);
		x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
		pkt += 0xC;
		static const uint32_t xy[4][4] = {
			{ 0, 0xA0, 0x780000, 0x7800A0 }, { 0xA0, 0x140, 0x7800A0, 0x780140 },
			{ 0x780000, 0x7800A0, 0xF00000, 0xF000A0 }, { 0x7800A0, 0x780140, 0xF000A0, 0xF00140 } };
		for (int q = 0; q < 4; q++)
		{
			U32(pkt, 4) = U32(node, 0x30);
			U32(pkt, 0) = 0x5000000;
			U8(pkt, 7) = 0x2A;
			for (int v = 0; v < 4; v++)
				U32(pkt, 8 + v * 4) = xy[q][v];
			x::SSIGPU_InsertPrimAltViewport(ot, pkt);
			pkt += 0x18;
		}
		x::sub_45BFC0(pkt, 0, 0, x::sub_45C690(0, 2, 0x280, 0) & 0xFFFF, 0);
		x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = pkt + 0xC;
		return 0;
	}

	uint32_t __cdecl im_7732F0(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		U16(a1, 0x134) = (uint16_t)(U16(a1, 0x134) - 1);
		if (S16(a1, 0x134) <= 0)
			b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl im_773320(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		if (a_73B640(3) != 0)
		{
			a_73BB60(0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		else
			a_73BB60(1);
		return 0;
	}

	uint32_t __cdecl im_7733B0(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		if (a_73B7E0(3) != 0)
		{
			x::BdPlaySE(game_addr(game::unk_154ECE8), 1, 0x80);
			x::au_re_Battle_ReadAnimation_7(a1, 8);
			U16(a1, 0x13A) = 0x800;
			im_773400(a1, 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_773400(uint32_t a1, uint32_t a2)
	{
		const uint32_t node = a1;
		a_733950(node + 0x30, U16(node, 0x13A), a2);
		return 0;
	}

	uint32_t __cdecl im_773520(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		if (S16(a1, 0x136) >= 6)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 4);
			U16(a1, 0x13A) = 0x800;
			im_773400(a1, 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_773560(uint32_t a1)
	{
		x::au_re_Battle_ReadAnimation_8(a1);
		if (a_73B7E0(4) != 0)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 4);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_773590(uint32_t a1)
	{
		const uint32_t e = ENT(U8(a1, 0x2C));
		if (a_73B7E0(5) != 0)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) & 0xFB);
			U16(a1, 0x13A) = 0;
			im_773400(a1, 0);
			x::au_re_Battle_ReadAnimation_7(a1, 5);
			gx::QueueChainTransformation(e, 4);
			U32(a1, 0x4C) = U32(e, 0x1C);
			U32(a1, 0x50) = U32(e, 0x20);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_773600(uint32_t a1)
	{
		if (x::au_re_Battle_ReadAnimation_8(a1) == 1)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 6);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl im_773630(uint32_t a1)
	{
		if (x::au_re_Battle_ReadAnimation_8(a1) == 1)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 7);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			U16(a1, 0x128) = 0x100;
		}
		return 0;
	}

	uint32_t __cdecl im_773670(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		if (S16(a1, 0x136) >= 0x12)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 3);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 5);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_088_INVINCIBLE_MOON_Sequence_State0_Advance_768820, (void *)a_73A0D0, "088 MAG_088_sub_768820" },
		{ game::MAG_088_INVINCIBLE_MOON_Sequence_State0_Advance_768830, (void *)a_73A0D0, "088 MAG_088_sub_768830" },
		{ game::MAG_088_INVINCIBLE_MOON_Sequence_State3_SpawnEmitter, (void *)a_73A170, "088 MAG_088_sub_7688C0" },
		{ game::MAG_088_INVINCIBLE_MOON_EmitterTask, (void *)a_73A1A0, "088 MAG_088_sub_7688F0" },
		{ game::MAG_088_INVINCIBLE_MOON_ActorSystemTask_768AC0, (void *)a_73A380, "088 MAG_088_sub_768AC0" },
		{ game::MAG_088_INVINCIBLE_MOON_LoadScript_Issue, (void *)a_73A3E0, "088 sub_768B20" },
		{ game::MAG_088_INVINCIBLE_MOON_LoadCallback_QueueTIM, (void *)a_73A580, "088 sub_768CC0" },
		{ game::MAG_088_INVINCIBLE_MOON_LoadCb_SummonStream, (void *)a_73A5B0, "088 sub_768CF0" },
		{ game::MAG_088_INVINCIBLE_MOON_LoadCb_ClearBusy_768D20, (void *)a_73A5E0, "088 sub_768D20" },
		{ game::MAG_088_INVINCIBLE_MOON_LoadCb_ClearBusy_768D30, (void *)a_73A5E0, "088 sub_768D30" },
		{ game::MAG_088_INVINCIBLE_MOON_LoadCb_ClearBusy_768D40, (void *)a_73A5E0, "088 sub_768D40" },
		{ game::MAG_088_INVINCIBLE_MOON_LoadCb_ClearBusy_768D50, (void *)a_73A5E0, "088 sub_768D50" },
		{ game::MAG_088_INVINCIBLE_MOON_LoadScript_RecordFile, (void *)a_73A620, "088 sub_768D60" },
		{ game::MAG_088_INVINCIBLE_MOON_FileLoadScript_State1_LoadScriptWait, (void *)a_73A660, "088 sub_768DA0" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_768E10, (void *)a_73A6D0, "088 nullsub_1264" },
		{ game::MAG_088_INVINCIBLE_MOON_DirSyncGate, (void *)a_73B640, "088 sub_769030" },
		{ game::MAG_088_INVINCIBLE_MOON_CameraTask, (void *)a_732A00, "088 sub_769060" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_ResetBlock, (void *)a_73AB00, "088 sub_769130" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_StepReached, (void *)a_73AAE0, "088 sub_769180" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_StepScript, (void *)a_73AE10, "088 sub_7691A0" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_WriteBattleCamera, (void *)a_73B160, "088 sub_7694F0" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_StartScript, (void *)a_73AB20, "088 sub_769660" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_State12_StepUntilDone, (void *)a_73B2D0, "088 sub_769BF0" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_State13_End, (void *)a_73B750, "088 sub_769C30" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_769C50, (void *)a_73A6D0, "088 nullsub_1265" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_769C60, (void *)a_73A6D0, "088 nullsub_1266" },
		{ game::MAG_088_INVINCIBLE_MOON_DirPhaseGE, (void *)a_73B7E0, "088 sub_769C90" },
		{ game::MAG_088_INVINCIBLE_MOON_HideMonsters, (void *)a_73B9F0, "088 sub_769CB0" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State2_ArmLoadScriptStep, (void *)a_73A930, "088 sub_769CF0" },
		{ game::MAG_088_INVINCIBLE_MOON_LoadScript_TryArmStep, (void *)a_73A950, "088 sub_769D10" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State3_WaitIdle, (void *)a_733AC0, "088 sub_769D50" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_WaitDirGate2, (void *)a_73B8A0, "088 sub_769D70" },
		{ game::MAG_088_INVINCIBLE_MOON_ClearEntityRecordBit1, (void *)a_746A10, "088 sub_769DB0" },
		{ game::MAG_088_INVINCIBLE_MOON_RestoreEntityRecordBit1, (void *)a_746AE0, "088 sub_769E60" },
		{ game::MAG_088_INVINCIBLE_MOON_RestoreMonsters, (void *)a_73BA90, "088 sub_769F40" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_769FE0, (void *)a_73A6D0, "088 nullsub_1267" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_769FF0, (void *)a_73A6D0, "088 nullsub_1268" },
		{ game::MAG_088_INVINCIBLE_MOON_SpawnActorSystemTask, (void *)a_73C100, "088 sub_76A0E0" },
		{ game::MAG_088_INVINCIBLE_MOON_Play_Step, (void *)a_73C280, "088 sub_76A250" },
		{ game::MAG_088_INVINCIBLE_MOON_EmitG3, (void *)a_73D770, "088 sub_76B630" },
		{ game::MAG_088_INVINCIBLE_MOON_EmitG4, (void *)a_73D9C0, "088 sub_76B880" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_76C6F0, (void *)a_73A6D0, "088 nullsub_1269" },
		{ game::MAG_088_INVINCIBLE_MOON_ActorSystemTask_76C700, (void *)a_73A380, "088 sub_76C700" },
		{ game::MAG_088_INVINCIBLE_MOON_DirectorSetup, (void *)a_73F990, "088 sub_76C790" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_StartPositions, (void *)a_73FC20, "088 sub_76CA20" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_Mode3Setup, (void *)a_73FE90, "088 sub_76CC90" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_TargetPositions, (void *)a_73FFB0, "088 sub_76CDB0" },
		{ game::MAG_088_INVINCIBLE_MOON_RelocateActorData, (void *)a_740210, "088 sub_76D010" },
		{ game::MAG_088_INVINCIBLE_MOON_RenderPrimLists, (void *)a_7355E0, "088 sub_76D4A0" },
		{ game::MAG_088_INVINCIBLE_MOON_PrimList_TexTri_76DA70, (void *)a_735BB0, "088 sub_76DA70" },
		{ game::MAG_088_INVINCIBLE_MOON_PrimList_TexQuad_76DCB0, (void *)a_735DF0, "088 sub_76DCB0" },
		{ game::MAG_088_INVINCIBLE_MOON_ActorMotion, (void *)a_740910, "088 sub_76EB90" },
		{ game::MAG_088_INVINCIBLE_MOON_DampValue, (void *)a_740F70, "088 sub_76F1F0" },
		{ game::MAG_088_INVINCIBLE_MOON_DampVector, (void *)a_740FC0, "088 sub_76F240" },
		{ game::MAG_088_INVINCIBLE_MOON_Instance_AdvanceTexFrame, (void *)a_741050, "088 sub_76F2D0" },
		{ game::MAG_088_INVINCIBLE_MOON_InstanceBuildTransform, (void *)a_741120, "088 sub_76F3A0" },
		{ game::MAG_088_INVINCIBLE_MOON_Particle_LifeCountdown, (void *)a_741B60, "088 sub_76FDE0" },
		{ game::MAG_088_RandRange, (void *)a_742290, "088 MAG_088_RandRange" },
		{ game::MAG_088_INVINCIBLE_MOON_RandRangeWide, (void *)a_7422C0, "088 sub_770540" },
		{ game::MAG_088_INVINCIBLE_MOON_RandVec3, (void *)a_742300, "088 sub_770580" },
		{ game::MAG_088_INVINCIBLE_MOON_JitterAngles3, (void *)a_742350, "088 sub_7705D0" },
		{ game::MAG_088_RandAngleJitter, (void *)a_7423A0, "088 MAG_088_RandAngleJitter" },
		{ game::MAG_088_INVINCIBLE_MOON_GetWord8Minus1, (void *)a_7424B0, "088 sub_770730" },
		{ game::MAG_088_INVINCIBLE_MOON_Particle_AttachToOwner, (void *)a_7424C0, "088 sub_770740" },
		{ game::MAG_088_INVINCIBLE_MOON_ParticleList_Append, (void *)a_742CA0, "088 sub_770F20" },
		{ game::MAG_088_INVINCIBLE_MOON_ParticleList_Unlink, (void *)a_742CD0, "088 sub_770F50" },
		{ game::MAG_088_INVINCIBLE_MOON_Particle_Init, (void *)a_742D00, "088 sub_770F80" },
		{ game::MAG_088_INVINCIBLE_MOON_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "088 sub_771130" },
		{ game::MAG_088_INVINCIBLE_MOON_Particle_PlaceAtPoint, (void *)a_742FF0, "088 sub_771270" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_7714C0, (void *)a_73A6D0, "088 nullsub_1270" },
		{ game::MAG_088_INVINCIBLE_MOON_StageLightTask, (void *)a_73F110, "088 sub_771510" },
		{ game::MAG_088_INVINCIBLE_MOON_StageLight_Init, (void *)a_7335D0, "088 sub_771580" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_771650, (void *)a_73A6D0, "088 nullsub_1271" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_7718C0, (void *)a_73A6D0, "088 nullsub_1272" },
		{ game::MAG_088_INVINCIBLE_MOON_Creature_DrawModel, (void *)a_746C10, "088 sub_7719A0" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_771DA0, (void *)a_73A6D0, "088 nullsub_1273" },
		{ game::InitEffectSequenceFromData_c6, (void *)a_7435E0, "088 InitEffectSequenceFromData_c6" },
		{ game::MAG_088_INVINCIBLE_MOON_Sequence_DrawSpriteStrip, (void *)a_743720, "088 sub_772320" },
		{ game::MAG_088_INVINCIBLE_MOON_Sequence_StepFrame, (void *)a_743C20, "088 sub_772840" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_772870, (void *)a_73A6D0, "088 nullsub_1274" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_772AA0, (void *)a_73A6D0, "088 nullsub_1275" },
		{ game::MAG_088_INVINCIBLE_MOON_HowlPlayTask, (void *)a_73A380, "088 sub_772C50" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_772D30, (void *)a_73A6D0, "088 nullsub_1276" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_773000, (void *)a_73A6D0, "088 nullsub_1277" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_7732E0, (void *)a_73A6D0, "088 nullsub_1278" },
		{ game::MAG_088_INVINCIBLE_MOON_SetPauseSwitch, (void *)a_73BB60, "088 sub_773360" },
		{ game::MAG_088_INVINCIBLE_MOON_SetEntityColourTint, (void *)a_733950, "088 sub_773420" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_7736B0, (void *)a_73A6D0, "088 nullsub_1279" },
		{ game::MAG_088_INVINCIBLE_MOON_Emitter_State1_ApplyActionResults, (void *)a_747440, "088 MAG_088_sub_7736C0" },
		{ game::MAG_088_INVINCIBLE_MOON_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "088 MAG_088_sub_773730" },
		{ game::MAG_088_INVINCIBLE_MOON_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "088 MAG_088_sub_773750" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_773770, (void *)a_73A6D0, "088 nullsub_1280" },
		{ game::MAG_088_INVINCIBLE_MOON_Sequence_State4_NextTargetLoop, (void *)a_747500, "088 MAG_088_sub_773780" },
		{ game::MAG_088_INVINCIBLE_MOON_Sequence_State0_Advance_7737C0, (void *)a_73A0D0, "088 MAG_088_sub_7737C0" },
		{ game::MAG_088_INVINCIBLE_MOON_Sequence_State6_WaitQueuesEmpty, (void *)a_747550, "088 MAG_088_sub_7737D0" },
		{ game::MAG_088_INVINCIBLE_MOON_Sequence_State7_RequestBattleSync, (void *)a_747560, "088 MAG_088_sub_7737E0" },
		{ game::MAG_088_INVINCIBLE_MOON_Sequence_State8_WaitBattleSyncDone, (void *)a_747590, "088 MAG_088_sub_773810" },
		{ game::MAG_088_INVINCIBLE_MOON_Sequence_State9_Finish, (void *)a_7475A0, "088 MAG_088_sub_773820" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Nullsub_773840, (void *)a_73A6D0, "088 nullsub_1263" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_088_INVINCIBLE_MOON_MasterTask, (void *)im_768690, "088 im_768690" },
		{ game::MAG_088_INVINCIBLE_MOON_Master_State2_CarvePools, (void *)im_768840, "088 im_768840" },
		{ game::MAG_088_INVINCIBLE_MOON_ClearPools, (void *)im_768880, "088 im_768880" },
		{ game::MAG_088_INVINCIBLE_MOON_Master_State2_InitDirector, (void *)im_768970, "088 im_768970" },
		{ game::MAG_088_INVINCIBLE_MOON_ModuleDataSetup, (void *)im_7689C0, "088 im_7689C0" },
		{ game::MAG_088_INVINCIBLE_MOON_DirectorDataSetup, (void *)im_768E20, "088 im_768E20" },
		{ game::MAG_088_INVINCIBLE_MOON_DirectorTask, (void *)im_768ED0, "088 im_768ED0" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_PhaseBookkeeping, (void *)im_768F90, "088 im_768F90" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State0_UploadTIMs_SpawnCamera, (void *)im_768FE0, "088 im_768FE0" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_State0_Reset, (void *)im_769120, "088 im_769120" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_State1_StartAtSync1, (void *)im_769150, "088 im_769150" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_State2_Chain, (void *)im_769950, "088 im_769950" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_State3_ChainRandom, (void *)im_769980, "088 im_769980" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_State4_Chain, (void *)im_7699C0, "088 im_7699C0" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_State5_Chain, (void *)im_7699F0, "088 im_7699F0" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_State6_Chain, (void *)im_769A20, "088 im_769A20" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_State7_PartyCentreAtSync4, (void *)im_769A50, "088 im_769A50" },
		{ game::MAG_088_INVINCIBLE_MOON_PartyCentre, (void *)im_769A90, "088 im_769A90" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_State8_PartyCentreChain, (void *)im_769B20, "088 im_769B20" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_State9_StartAtSync5, (void *)im_769B60, "088 im_769B60" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_State10_Chain, (void *)im_769B90, "088 im_769B90" },
		{ game::MAG_088_INVINCIBLE_MOON_Camera_State11_Chain, (void *)im_769BC0, "088 im_769BC0" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State18_WaitDirGate6_769C10, (void *)im_769C10, "088 im_769C10" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State1_Cue1, (void *)im_769C70, "088 im_769C70" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State5_Cue3_HideParty, (void *)im_769D90, "088 im_769D90" },
		{ game::MAG_088_INVINCIBLE_MOON_HidePartyEntities, (void *)im_769DD0, "088 im_769DD0" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State6_Sync4, (void *)im_769E10, "088 im_769E10" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State7_Cue4_ShowParty, (void *)im_769E40, "088 im_769E40" },
		{ game::MAG_088_INVINCIBLE_MOON_RestorePartyEntities, (void *)im_769E90, "088 im_769E90" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State8_DoneAtFrame23, (void *)im_769ED0, "088 im_769ED0" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State9_Sync5, (void *)im_769EF0, "088 im_769EF0" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State10_ShowMonsters, (void *)im_769F20, "088 im_769F20" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State11_WaitFrame14, (void *)im_769F80, "088 im_769F80" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State18_WaitDirGate6_769FA0, (void *)im_769FA0, "088 im_769FA0" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_State13_FinishAtCue6, (void *)im_769FC0, "088 im_769FC0" },
		{ game::MAG_088_INVINCIBLE_MOON_SpawnPhaseTasks, (void *)im_76A000, "088 im_76A000" },
		{ game::MAG_088_INVINCIBLE_MOON_SpawnPlayTask, (void *)im_76A130, "088 im_76A130" },
		{ game::MAG_088_INVINCIBLE_MOON_MoonPlayTask, (void *)im_76A180, "088 im_76A180" },
		{ game::MAG_088_INVINCIBLE_MOON_MoonPlay_State0_Setup, (void *)im_76A1F0, "088 im_76A1F0" },
		{ game::MAG_088_INVINCIBLE_MOON_Play_ObjectCallback, (void *)im_76A380, "088 im_76A380" },
		{ game::MAG_088_INVINCIBLE_MOON_RenderModelPrims, (void *)im_76A650, "088 im_76A650" },
		{ game::MAG_088_INVINCIBLE_MOON_PrimList_FlatTri, (void *)im_76A780, "088 im_76A780" },
		{ game::MAG_088_INVINCIBLE_MOON_PrimList_FlatQuad, (void *)im_76A9A0, "088 im_76A9A0" },
		{ game::MAG_088_INVINCIBLE_MOON_PrimList_TexTri_76AC30, (void *)im_76AC30, "088 im_76AC30" },
		{ game::MAG_088_INVINCIBLE_MOON_PrimList_TexQuad_76B0B0, (void *)im_76B0B0, "088 im_76B0B0" },
		{ game::MAG_088_INVINCIBLE_MOON_PrimList_GouraudTexTriScroll, (void *)im_76BB60, "088 im_76BB60" },
		{ game::MAG_088_INVINCIBLE_MOON_PrimList_GT4, (void *)im_76C060, "088 im_76C060" },
		{ game::MAG_088_INVINCIBLE_MOON_MoonPlay_State1_PlayUntilCue5, (void *)im_76C6B0, "088 im_76C6B0" },
		{ game::MAG_088_INVINCIBLE_MOON_ActSys_State0_StartAtFrame, (void *)im_76C760, "088 im_76C760" },
		{ game::MAG_088_INVINCIBLE_MOON_ResetTrailHistory, (void *)im_76C940, "088 im_76C940" },
		{ game::MAG_088_INVINCIBLE_MOON_FillTrailRows, (void *)im_76CBC0, "088 im_76CBC0" },
		{ game::MAG_088_INVINCIBLE_MOON_ActSys_Step, (void *)im_76D270, "088 im_76D270" },
		{ game::MAG_088_INVINCIBLE_MOON_ActSys_DrawModelPass, (void *)im_76D300, "088 im_76D300" },
		{ game::MAG_088_INVINCIBLE_MOON_ActSys_DrawModelActor, (void *)im_76D360, "088 im_76D360" },
		{ game::MAG_088_INVINCIBLE_MOON_ModelList_FlatTri, (void *)im_76D5E0, "088 im_76D5E0" },
		{ game::MAG_088_INVINCIBLE_MOON_ModelList_FlatQuad, (void *)im_76D7F0, "088 im_76D7F0" },
		{ game::MAG_088_INVINCIBLE_MOON_ModelList_GouraudTri, (void *)im_76DF50, "088 im_76DF50" },
		{ game::MAG_088_INVINCIBLE_MOON_ModelList_GouraudQuad, (void *)im_76E180, "088 im_76E180" },
		{ game::MAG_088_INVINCIBLE_MOON_ModelList_GouraudTexTri, (void *)im_76E440, "088 im_76E440" },
		{ game::MAG_088_INVINCIBLE_MOON_ModelList_GouraudTexQuad, (void *)im_76E6A0, "088 im_76E6A0" },
		{ game::MAG_088_INVINCIBLE_MOON_ActSys_DrawSpritePass, (void *)im_76E980, "088 im_76E980" },
		{ game::MAG_088_INVINCIBLE_MOON_ActSys_DrawSprite, (void *)im_76EA40, "088 im_76EA40" },
		{ game::MAG_088_INVINCIBLE_MOON_ActSys_UpdatePass, (void *)im_76EAC0, "088 im_76EAC0" },
		{ game::MAG_088_INVINCIBLE_MOON_ActorStep, (void *)im_76EB00, "088 im_76EB00" },
		{ game::MAG_088_INVINCIBLE_MOON_MorphVertexBlock, (void *)im_76FB30, "088 im_76FB30" },
		{ game::MAG_088_INVINCIBLE_MOON_Instance_EndOfLifeTest, (void *)im_76FC40, "088 im_76FC40" },
		{ game::MAG_088_INVINCIBLE_MOON_Instance_SpawnFollowUp, (void *)im_76FCC0, "088 im_76FCC0" },
		{ game::MAG_088_INVINCIBLE_MOON_AllocParticle, (void *)im_76FCF0, "088 im_76FCF0" },
		{ game::MAG_088_INVINCIBLE_MOON_ActSys_UpdateEmitter, (void *)im_76FD90, "088 im_76FD90" },
		{ game::MAG_088_INVINCIBLE_MOON_ActSys_EmitterSpawnStep, (void *)im_76FE10, "088 im_76FE10" },
		{ game::MAG_088_INVINCIBLE_MOON_ActSys_SpawnParticles, (void *)im_76FE90, "088 im_76FE90" },
		{ game::MAG_088_INVINCIBLE_MOON_ActSys_AllocParticleSlot, (void *)im_770670, "088 im_770670" },
		{ game::MAG_088_INVINCIBLE_MOON_Particle_KeyPositionPath, (void *)im_770890, "088 im_770890" },
		{ game::MAG_088_INVINCIBLE_MOON_SpawnSubEffects, (void *)im_771380, "088 im_771380" },
		{ game::MAG_088_INVINCIBLE_MOON_Director_SpawnGroupEffects, (void *)im_771410, "088 im_771410" },
		{ game::MAG_088_INVINCIBLE_MOON_ActSys_State1_StepUntilDone, (void *)im_7714A0, "088 im_7714A0" },
		{ game::MAG_088_INVINCIBLE_MOON_SpawnCreature, (void *)im_7714D0, "088 im_7714D0" },
		{ game::MAG_088_INVINCIBLE_MOON_StageLight_State0_Rise, (void *)im_7715C0, "088 im_7715C0" },
		{ game::MAG_088_INVINCIBLE_MOON_StageLight_State1_Wait, (void *)im_771600, "088 im_771600" },
		{ game::MAG_088_INVINCIBLE_MOON_StageLight_State3_FadeOut, (void *)im_771610, "088 im_771610" },
		{ game::MAG_088_INVINCIBLE_MOON_MoonModelTask, (void *)im_771660, "088 im_771660" },
		{ game::MAG_088_INVINCIBLE_MOON_MoonModel_Draw, (void *)im_7716C0, "088 im_7716C0" },
		{ game::MAG_088_INVINCIBLE_MOON_MoonModel_State0_Setup, (void *)im_7717A0, "088 im_7717A0" },
		{ game::MAG_088_INVINCIBLE_MOON_MoonModel_State1_DriftUntilCue4, (void *)im_771880, "088 im_771880" },
		{ game::MAG_088_INVINCIBLE_MOON_DoubleTask, (void *)im_7718D0, "088 im_7718D0" },
		{ game::MAG_088_INVINCIBLE_MOON_Double_State0_PlaceHidden, (void *)im_771B80, "088 im_771B80" },
		{ game::MAG_088_INVINCIBLE_MOON_Double_State1_ShowAtFrameC, (void *)im_771BD0, "088 im_771BD0" },
		{ game::MAG_088_INVINCIBLE_MOON_Double_State2_Anim1, (void *)im_771C00, "088 im_771C00" },
		{ game::MAG_088_INVINCIBLE_MOON_Double_State3_Anim2, (void *)im_771C30, "088 im_771C30" },
		{ game::MAG_088_INVINCIBLE_MOON_Double_State4_Anim5, (void *)im_771C60, "088 im_771C60" },
		{ game::MAG_088_INVINCIBLE_MOON_Double_State5_Cue3, (void *)im_771C90, "088 im_771C90" },
		{ game::MAG_088_INVINCIBLE_MOON_Double_State6_Anim3, (void *)im_771CC0, "088 im_771CC0" },
		{ game::MAG_088_INVINCIBLE_MOON_Double_State7_HideAtCue4, (void *)im_771CF0, "088 im_771CF0" },
		{ game::MAG_088_INVINCIBLE_MOON_Double_State8_ShowAtCue5, (void *)im_771D20, "088 im_771D20" },
		{ game::MAG_088_INVINCIBLE_MOON_Double_State9_WaitAnimEnd, (void *)im_771D50, "088 im_771D50" },
		{ game::MAG_088_INVINCIBLE_MOON_Double_State10_Finish, (void *)im_771D70, "088 im_771D70" },
		{ game::MAG_088_INVINCIBLE_MOON_AngeloTask, (void *)im_771DB0, "088 im_771DB0" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_DustRings4, (void *)im_771F50, "088 im_771F50" },
		{ game::MAG_088_INVINCIBLE_MOON_DustRingTask, (void *)im_772060, "088 im_772060" },
		{ game::MAG_088_INVINCIBLE_MOON_DustRing_Draw, (void *)im_7720C0, "088 im_7720C0" },
		{ game::MAG_088_INVINCIBLE_MOON_DustRing_State0_Setup, (void *)im_772790, "088 im_772790" },
		{ game::MAG_088_INVINCIBLE_MOON_DustRing_State1_Rise, (void *)im_7727B0, "088 im_7727B0" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_DustRings3, (void *)im_772880, "088 im_772880" },
		{ game::MAG_088_INVINCIBLE_MOON_DustRingFastTask, (void *)im_772990, "088 im_772990" },
		{ game::MAG_088_INVINCIBLE_MOON_DustRingFast_State0_Setup, (void *)im_7729F0, "088 im_7729F0" },
		{ game::MAG_088_INVINCIBLE_MOON_DustRingFast_State1_Rise, (void *)im_772A10, "088 im_772A10" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State0_BindRinoaAnimSet, (void *)im_772AB0, "088 im_772AB0" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State1_RinoaChain2, (void *)im_772B40, "088 im_772B40" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State2_Anim1, (void *)im_772B80, "088 im_772B80" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State3_SpawnHowlPlay, (void *)im_772BE0, "088 im_772BE0" },
		{ game::MAG_088_INVINCIBLE_MOON_HowlPlay_State0_Setup, (void *)im_772CB0, "088 im_772CB0" },
		{ game::MAG_088_INVINCIBLE_MOON_HowlPlay_State1_Play, (void *)im_772D10, "088 im_772D10" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State4_SpawnHalo, (void *)im_772D40, "088 im_772D40" },
		{ game::MAG_088_INVINCIBLE_MOON_HaloTask, (void *)im_772D90, "088 im_772D90" },
		{ game::MAG_088_INVINCIBLE_MOON_Halo_Draw, (void *)im_772E30, "088 im_772E30" },
		{ game::MAG_088_INVINCIBLE_MOON_Halo_State0_Setup, (void *)im_772F50, "088 im_772F50" },
		{ game::MAG_088_INVINCIBLE_MOON_Halo_State1_FadeIn, (void *)im_772FB0, "088 im_772FB0" },
		{ game::MAG_088_INVINCIBLE_MOON_Halo_State2_HideAtCue3, (void *)im_772FE0, "088 im_772FE0" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State5_Anim8, (void *)im_773010, "088 im_773010" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State6_SpawnFlash, (void *)im_773040, "088 im_773040" },
		{ game::MAG_088_INVINCIBLE_MOON_FlashTask, (void *)im_773090, "088 im_773090" },
		{ game::MAG_088_INVINCIBLE_MOON_Flash_State0_Delay, (void *)im_773100, "088 im_773100" },
		{ game::MAG_088_INVINCIBLE_MOON_Flash_State1_StepDraw, (void *)im_773130, "088 im_773130" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State7_Countdown, (void *)im_7732F0, "088 im_7732F0" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State8_WaitSync3_Pause, (void *)im_773320, "088 im_773320" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State9_Cue3_Glow, (void *)im_7733B0, "088 im_7733B0" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_SetGlow, (void *)im_773400, "088 im_773400" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State10_Anim4_Glow, (void *)im_773520, "088 im_773520" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State11_HideAtCue4, (void *)im_773560, "088 im_773560" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State12_ShowAtCue5_RinoaChain4, (void *)im_773590, "088 im_773590" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State13_Anim6, (void *)im_773600, "088 im_773600" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State14_Anim7_WalkOff, (void *)im_773630, "088 im_773630" },
		{ game::MAG_088_INVINCIBLE_MOON_Angelo_State15_Finish, (void *)im_773670, "088 im_773670" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag088_invincible_moon()
	{
		act::register_module(88);
		for (const act::moon88::ModPort *p = act::moon88::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(88, p->addr, p->port, p->name);
		for (const act::moon88::ModPort *p = act::moon88::PORTS; p->addr; p++)
			act::register_module_port(88, p->addr, p->port, p->name);
	}
}

