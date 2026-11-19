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
namespace strike87
{
	static const Mod MOD_087 = { "angelostrike", 87, game::MAG_087_ANGELO_STRIKE_LoadFile, game::MAG_086_ANGELO_CANNON_LoadFile,
		{ {}, {}, game::MAG_087_ANGELO_STRIKE_pDirector, game::MAG_087_ANGELO_STRIKE_pCameraBlock, {}, game::MAG_087_ANGELO_STRIKE_PacketCursor2, {}, {}, {}, {}, {}, {}, game::MAG_087_ANGELO_STRIKE_pFile, game::MAG_087_ANGELO_STRIKE_PacketCursor, game::MAG_087_ANGELO_STRIKE_DirectorQueue, game::MAG_087_ANGELO_STRIKE_PacketCursor2, game::MAG_087_ANGELO_STRIKE_pCameraCopy2, game::MAG_087_ANGELO_STRIKE_ModelQueue, {}, game::MAG_087_ANGELO_STRIKE_PlaysPause, game::MAG_087_ANGELO_STRIKE_PlayQueue, game::MAG_087_ANGELO_STRIKE_CameraRingQueue, game::MAG_087_ANGELO_STRIKE_PacketArena2_Even, game::MAG_087_ANGELO_STRIKE_PacketArena2_Odd, game::MAG_087_ANGELO_STRIKE_Queue5, game::MAG_087_ANGELO_STRIKE_Queue9, game::MAG_087_ANGELO_STRIKE_EmitterQueue, game::MAG_087_ANGELO_STRIKE_PacketArena_Even, game::MAG_087_ANGELO_STRIKE_PacketArena_Odd, game::MAG_087_ANGELO_STRIKE_pLoadScriptRecord, game::MAG_087_ANGELO_STRIKE_pModuleData, game::MAG_087_ANGELO_STRIKE_AngeloQueue, game::MAG_087_ANGELO_STRIKE_ActorCount16, {}, game::MAG_087_ANGELO_STRIKE_PlayVertexBlend, game::MAG_087_ANGELO_STRIKE_ActorCount14, game::MAG_087_ANGELO_STRIKE_LoadScriptQueue, game::MAG_087_ANGELO_STRIKE_pCameraCopy, game::MAG_087_ANGELO_STRIKE_PauseSwitch, game::MAG_087_ANGELO_STRIKE_pActorState },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_087_ANGELO_STRIKE_ActSys_DrawModelActor, game::MAG_087_ANGELO_STRIKE_RenderPrimLists, game::MAG_087_ANGELO_STRIKE_ModelList_FlatTri, game::MAG_087_ANGELO_STRIKE_ModelList_FlatQuad, game::MAG_087_ANGELO_STRIKE_PrimList_TexTri_77A410, game::MAG_087_ANGELO_STRIKE_PrimList_TexQuad_77A650, game::MAG_087_ANGELO_STRIKE_ModelList_GouraudTri, game::MAG_087_ANGELO_STRIKE_ModelList_GouraudQuad, game::MAG_087_ANGELO_STRIKE_ModelList_GouraudTexTri, game::MAG_087_ANGELO_STRIKE_ModelList_GouraudTexQuad, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_087_ANGELO_STRIKE_Sequence_State0_Advance_773BA0, game::MAG_087_ANGELO_STRIKE_Sequence_State0_Advance_773BB0, game::MAG_087_ANGELO_STRIKE_Master_State2_CarveActorPools, game::MAG_087_ANGELO_STRIKE_Sequence_State3_SpawnEmitter, game::MAG_087_ANGELO_STRIKE_EmitterTask, game::MAG_087_ANGELO_STRIKE_Master_State2_InitDirector, {}, game::MAG_087_ANGELO_STRIKE_LoadCallback_QueueTIM, game::MAG_087_ANGELO_STRIKE_LoadCallback_SummonStream, game::MAG_087_ANGELO_STRIKE_LoadCb_ClearBusy_774090, game::MAG_087_ANGELO_STRIKE_LoadCb_ClearBusy_7740A0, game::MAG_087_ANGELO_STRIKE_LoadCb_ClearBusy_7740B0, game::MAG_087_ANGELO_STRIKE_LoadCb_ClearBusy_7740C0, game::MAG_087_ANGELO_STRIKE_LoadScript_RecordFile, {}, {}, game::MAG_087_ANGELO_STRIKE_LoadScript_TryArmStep, {}, {}, game::MAG_087_ANGELO_STRIKE_Camera_StepScript, game::MAG_087_ANGELO_STRIKE_Camera_WriteBattleCamera, {}, {}, {}, game::MAG_087_ANGELO_STRIKE_Director_SyncPoint, game::MAG_087_ANGELO_STRIKE_Nullsub_774F50, game::MAG_087_ANGELO_STRIKE_Director_CueReached, game::MAG_087_ANGELO_STRIKE_Play_ObjectCallback, {}, {}, {}, {}, {}, game::MAG_087_ANGELO_STRIKE_ResetTrailHistory, game::MAG_087_ANGELO_STRIKE_Director_StartPositions, game::MAG_087_ANGELO_STRIKE_FillTrailRows, game::MAG_087_ANGELO_STRIKE_Director_Mode3Setup, game::MAG_087_ANGELO_STRIKE_Director_TargetPositions, game::MAG_087_ANGELO_STRIKE_RelocateActorData, {}, {}, game::MAG_087_ANGELO_STRIKE_DampValue, game::MAG_087_ANGELO_STRIKE_DampVector, {}, {}, game::MAG_087_ANGELO_STRIKE_MorphVertexBlock, {}, {}, {}, game::MAG_087_RandRange, game::MAG_087_ANGELO_STRIKE_RandRangeWide, game::MAG_087_RandAngleJitter, game::MAG_087_ANGELO_STRIKE_Particle_AttachToOwner, game::MAG_087_ANGELO_STRIKE_Actor_InterpolatePosition, {}, game::MAG_087_ANGELO_STRIKE_ParticleList_Unlink, game::MAG_087_ANGELO_STRIKE_Particle_PlaceAtAnchorSet, game::MAG_087_ANGELO_STRIKE_Particle_PlaceAtPoint, game::MAG_087_ANGELO_STRIKE_Sequence_DrawSpriteStrip, {}, game::MAG_087_ANGELO_STRIKE_DamageState_ApplyActionResult, game::MAG_087_ANGELO_STRIKE_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_087_ANGELO_STRIKE_Emitter_State3_FinishReleaseRoot, game::MAG_087_ANGELO_STRIKE_Nullsub_77F150, game::MAG_087_ANGELO_STRIKE_Sequence_State4_NextTargetLoop, game::MAG_087_ANGELO_STRIKE_Sequence_State0_Advance_77F1A0, game::MAG_087_ANGELO_STRIKE_Sequence_State6_WaitQueuesEmpty, game::MAG_087_ANGELO_STRIKE_Sequence_State7_RequestBattleSync, game::MAG_087_ANGELO_STRIKE_Sequence_State8_WaitBattleSyncDone, game::MAG_087_ANGELO_STRIKE_Sequence_State9_Finish, game::MAG_087_ANGELO_STRIKE_Nullsub_77F220 } };

	static constexpr game::Id DIR_PTR = game::MAG_087_ANGELO_STRIKE_pDirector;
	static constexpr game::Id PHASE_DATA = DIR_PTR;
	static constexpr game::Id PACKET_CURSOR = game::MAG_087_ANGELO_STRIKE_PacketCursor;
	static constexpr game::Id PLAY_PAUSE = game::MAG_087_ANGELO_STRIKE_PlaysPause;
	static constexpr game::Id PAUSE_SWITCH = game::MAG_087_ANGELO_STRIKE_PauseSwitch;
	static constexpr game::Id CREATURE_ARG = game::MAG_087_ANGELO_STRIKE_CreatureDrawState;
	static constexpr game::Id SCRATCH_SP = game::MAG_087_ANGELO_STRIKE_pCameraCopy;
	static constexpr game::Id ACTOR_STATE = game::MAG_087_ANGELO_STRIKE_pActorState;
	static inline uint32_t ENT(uint32_t slot) { return game_addr(game::BattleEntitySlotData) + slot * 0x9C; }
	static inline uint32_t b102_task_end(uint32_t node, uint8_t status)
	{
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	namespace gx
	{
		static inline uint32_t BindEntityModelFromComData(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(a1, a2, a3); }
		static inline uint32_t QueueChainTransformation(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::QueueChainTransformation))(a1, a2); }
		static inline uint32_t EntityChainLowest(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::Battle_EntityChainLowest))(a1); }
		static inline uint32_t BS_Camera_ArmReturnAfterEffect() { return fn<uint32_t (__cdecl *)()>(game_addr(game::BS_Camera_ArmReturnAfterEffect))(); }
		static inline uint32_t SSIGPU_SetDrawEnv(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_SetDrawEnv))(a1, a2); }
		static inline uint32_t GteReadScreenOffset(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::GTE_ReadScreenOffset))(a1, a2); }
		static inline uint32_t GteSetScreenOffset(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::Call_Bs_ParseCamera))(a1, a2); }
		static inline uint32_t SetDrawAreaPrim(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::GPU_SetDrawAreaPrim))(a1, a2); }
		static inline uint32_t SetDrawOffsetPrim(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_SetDrawOffsetPrim))(a1, a2); }
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

	uint32_t __cdecl ak_773A10(uint32_t a1);
	uint32_t __cdecl ak_773BC0(uint32_t a1);
	uint32_t __cdecl ak_773C00(void);
	uint32_t __cdecl ak_773CF0(uint32_t a1);
	uint32_t __cdecl ak_773D40();
	uint32_t __cdecl ak_774190(uint32_t a1);
	uint32_t __cdecl ak_774240(uint32_t a1);
	uint32_t __cdecl ak_7742E0(uint32_t a1);
	uint32_t __cdecl ak_774330(uint32_t a1);
	uint32_t __cdecl ak_7743A0(uint32_t a1);
	uint32_t __cdecl ak_774450(uint32_t a1);
	uint32_t __cdecl ak_7744B0(uint32_t a1);
	uint32_t __cdecl ak_774CB0(uint32_t a1);
	uint32_t __cdecl ak_774CE0(uint32_t a1);
	uint32_t __cdecl ak_774D40(uint32_t a1);
	uint32_t __cdecl ak_774D90(uint32_t a1);
	uint32_t __cdecl ak_774DE0(uint32_t a1);
	uint32_t __cdecl ak_774E20(uint32_t a1);
	uint32_t __cdecl ak_774E90(uint32_t a1);
	uint32_t __cdecl ak_774EC0(uint32_t a1);
	uint32_t __cdecl ak_774F10(uint32_t a1);
	uint32_t __cdecl ak_774F70(uint32_t a1);
	uint32_t __cdecl ak_775010(uint32_t a1);
	uint32_t __cdecl ak_7750E0(uint32_t a1);
	uint32_t __cdecl ak_775120(uint32_t a1);
	uint32_t __cdecl ak_775160(uint32_t a1);
	uint32_t __cdecl ak_7751A0(uint32_t a1);
	uint32_t __cdecl ak_775510(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ak_775720(uint32_t a1);
	uint32_t __cdecl ak_775780(uint32_t a1);
	uint32_t __cdecl ak_775E50(uint32_t a1);
	uint32_t __cdecl ak_775E70(uint32_t a1);
	uint32_t __cdecl ak_775F40(uint32_t a1);
	uint32_t __cdecl ak_776010(uint32_t a1);
	uint32_t __cdecl ak_7760E0(uint32_t a1);
	uint32_t __cdecl ak_776260(uint32_t a1);
	uint32_t __cdecl ak_7762D0(uint32_t a1);
	uint32_t __cdecl ak_7763B0(uint32_t a1);
	uint32_t __cdecl ak_776460(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl ak_776510(uint32_t a1);
	uint32_t __cdecl ak_7766B0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ak_776920(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ak_776A50(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ak_776C70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ak_776F00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ak_777380(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ak_777E30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ak_778330(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ak_778980(uint32_t a1);
	uint32_t __cdecl ak_7789B0(uint32_t a1);
	uint32_t __cdecl ak_778A20(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ak_778AB0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ak_778E90(uint32_t a1);
	uint32_t __cdecl ak_778F90(uint32_t a1);
	uint32_t __cdecl ak_7790B0(uint32_t a1);
	uint32_t __cdecl ak_7792E0(uint32_t a1);
	uint32_t __cdecl ak_779560(void);
	uint32_t __cdecl ak_779C10(uint32_t a1);
	uint32_t __cdecl ak_779F80(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ak_77A190(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ak_77A8F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ak_77AB20(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ak_77ADE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ak_77B040(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl ak_77B320(void);
	uint32_t __cdecl ak_77B3E0(uint32_t a1);
	uint32_t __cdecl ak_77B460(void);
	uint32_t __cdecl ak_77B4A0(uint32_t a1);
	uint32_t __cdecl ak_77C4D0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ak_77C5E0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ak_77C660(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ak_77C690(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ak_77C730(uint32_t a1);
	uint32_t __cdecl ak_77C7B0(uint32_t a1);
	uint32_t __cdecl ak_77C830(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl ak_77D010(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ak_77D230(uint32_t a1);
	uint32_t __cdecl ak_77DD20(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ak_77DDB0(void);
	uint32_t __cdecl ak_77DE40(uint32_t a1);
	uint32_t __cdecl ak_77DE70(uint32_t a1);
	uint32_t __cdecl ak_77DFC0(uint32_t a1);
	uint32_t __cdecl ak_77E060(uint32_t a1);
	uint32_t __cdecl ak_77E180(uint32_t a1);
	uint32_t __cdecl ak_77E1E0(uint32_t a1);
	uint32_t __cdecl ak_77E210(uint32_t a1);
	uint32_t __cdecl ak_77E220(uint32_t a1);
	uint32_t __cdecl ak_77E260(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ak_77E490(uint32_t a1);
	uint32_t __cdecl ak_77E4D0(uint32_t a1);
	uint32_t __cdecl ak_77E600(uint32_t a1);
	uint32_t __cdecl ak_77E720(uint32_t a1);
	uint32_t __cdecl ak_77E780(uint32_t a1);
	uint32_t __cdecl ak_77E7D0(uint32_t a1);
	uint32_t __cdecl ak_77E800(uint32_t a1);
	uint32_t __cdecl ak_77E850(uint32_t a1);
	uint32_t __cdecl ak_77EAB0(uint32_t a1);
	uint32_t __cdecl ak_77EB70(uint32_t a1);
	uint32_t __cdecl ak_77EBD0(uint32_t a1);
	uint32_t __cdecl ak_77EC00(uint32_t a1);
	uint32_t __cdecl ak_77ECC0(uint32_t a1);
	uint32_t __cdecl ak_77ED10(uint32_t a1);
	uint32_t __cdecl ak_77ED50(uint32_t a1);
	uint32_t __cdecl ak_77EDF0(uint32_t a1);
	uint32_t __cdecl ak_77EE50(uint32_t a1);
	uint32_t __cdecl ak_77EEC0(uint32_t a1);
	uint32_t __cdecl ak_77EF90(uint32_t a1);
	uint32_t __cdecl ak_77EFD0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl ak_77F030(uint32_t a1);
	uint32_t __cdecl ak_77F070(uint32_t a1);
	uint32_t __cdecl ak_77F0D0(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace strike87
{
	uint32_t __cdecl ak_773A10(uint32_t a1)
	{
		g_mod = &MOD_087;

		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pCameraCopy)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pCameraCopy2)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		uint32_t states[11];
		states[0] = game_addr(game::MAG_087_ANGELO_STRIKE_Sequence_State0_Advance_773BA0);
		states[1] = game_addr(game::MAG_087_ANGELO_STRIKE_Sequence_State0_Advance_773BB0);
		states[2] = game_addr(game::MAG_087_ANGELO_STRIKE_Master_State2_CarveActorPools);
		states[3] = game_addr(game::MAG_087_ANGELO_STRIKE_Sequence_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_087_ANGELO_STRIKE_Sequence_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_087_ANGELO_STRIKE_Sequence_State0_Advance_77F1A0);
		states[6] = game_addr(game::MAG_087_ANGELO_STRIKE_Sequence_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_087_ANGELO_STRIKE_Sequence_State7_RequestBattleSync);
		states[8] = game_addr(game::MAG_087_ANGELO_STRIKE_Sequence_State8_WaitBattleSyncDone);
		states[9] = game_addr(game::MAG_087_ANGELO_STRIKE_Sequence_State9_Finish);
		states[10] = game_addr(game::MAG_087_ANGELO_STRIKE_Nullsub_77F220);
		if ((U8(node, 0x5C) & 1) != 0)
		{
			MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_PacketCursor)) = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_PacketArena_Odd));
			MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_PacketCursor2)) = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_PacketArena2_Odd));
		}
		else
		{
			MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_PacketCursor)) = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_PacketArena_Even));
			MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_PacketCursor2)) = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_PacketArena2_Even));
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::MAG_087_ANGELO_STRIKE_ActorCount14)) = 0;
		MEM<uint16_t>(game_addr(game::MAG_087_ANGELO_STRIKE_ActorCount16)) = 0;
		static constexpr game::Id queues[9] = { game::MAG_087_ANGELO_STRIKE_EmitterQueue, game::MAG_087_ANGELO_STRIKE_DirectorQueue, game::MAG_087_ANGELO_STRIKE_LoadScriptQueue, game::MAG_087_ANGELO_STRIKE_AngeloQueue, game::MAG_087_ANGELO_STRIKE_Queue5, game::MAG_087_ANGELO_STRIKE_CameraRingQueue, game::MAG_087_ANGELO_STRIKE_ModelQueue, game::MAG_087_ANGELO_STRIKE_PlayQueue, game::MAG_087_ANGELO_STRIKE_Queue9 };
		for (int i = 0; i < 9; i++)
		{
			const uint32_t n = x::ExecuteTaskQueue(game_addr(queues[i]));
			U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)n);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ak_773BC0(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_FileArenaCursor));
		MEM<uint32_t>(game_addr(game::unk_25B4634)) = p;
		p += 0x870;
		MEM<uint32_t>(game_addr(game::unk_25B4B48)) = p;
		p += 0x8340;
		MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_FileArenaCursor)) = p;
		ak_773C00();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ak_773C00(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::unk_25B4634)), 0x870);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::unk_25B4B48)), 0x8340);
		MEM<uint16_t>(game_addr(game::unk_25AECD0)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25B4640)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25B4444)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25ACEB8)) = 0;
		return 0;
	}

	uint32_t __cdecl ak_773CF0(uint32_t a1)
	{
		ak_774190(a1);
		ak_773D40();
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_087_ANGELO_STRIKE_LoadScriptQueue), game_addr(game::MAG_087_ANGELO_STRIKE_LoadScriptTask), 0x30, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_087_ANGELO_STRIKE_DirectorQueue), game_addr(game::MAG_087_ANGELO_STRIKE_DirectorTask), 0x48, a1);
		t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl ak_773D40()
	{
		MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pLoadScriptRecord)) = game_addr(game::MAG_087_ANGELO_STRIKE_LoadScript);
		MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pModuleData)) = game_addr(game::MAG_087_ANGELO_STRIKE_ModuleData);
		x::MAG_007_sub_8DCC00(game_addr(game::MAG_087_ANGELO_STRIKE_ModuleData), 0x16C);
		const uint32_t b = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pModuleData));
		uint32_t c = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_FileArenaCursor));
		U32(b, 0x100) = c;
		c += 0xA000;
		MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_FileArenaCursor)) = c;
		U32(b, 0x114) = game_addr(game::unk_19EB018);
		for (int32_t o = 0x104; o <= 0x160; o += 4)
			if (o != 0x114) U32(b, o) = game_addr(game::unk_19CF018);
		U16(b, 0x164) = 0;
		U16(b, 0x166) = 0;
		return b;
	}

	uint32_t __cdecl ak_774190(uint32_t a1)
	{
		const uint32_t t = gx::EntityChainLowest(ENT(U8(a1, 0x2D)));
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(DIR_PTR)), 0x54);
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		U32(d, 8) = U32(t, 0x1C);
		const uint32_t c = ENT(U8(a1, 0x2C));
		U32(d, 0xC) = U32(t, 0x20);
		U32(d, 0) = U32(c, 0x1C);
		U32(d, 4) = U32(c, 0x20);
		const int16_t dz = (int16_t)(U16(d, 0xC) - U16(d, 4));
		const int16_t dx = (int16_t)(U16(d, 8) - U16(d, 0));
		const uint32_t ang = x::CartesianToGameAngle((uint32_t)(int32_t)dx, (uint32_t)(int32_t)dz);
		const uint16_t size = U16(t, 0x26);
		const uint32_t d2 = MEM<uint32_t>(game_addr(DIR_PTR));
		U16(d2, 0x4A) = (uint16_t)((ang - 0x800) & 0xFFF);
		U16(d2, 0x4E) = size;
		return d2;
	}

	uint32_t __cdecl ak_774240(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[10];
		states[0] = game_addr(game::MAG_087_ANGELO_STRIKE_Director_State0_UploadTIM_CameraTask_Sync1);
		states[1] = game_addr(game::MAG_087_ANGELO_STRIKE_Director_State1_Cue1);
		states[2] = game_addr(game::MAG_087_ANGELO_STRIKE_Director_State2_LoadStep);
		states[3] = game_addr(game::MAG_087_ANGELO_STRIKE_Director_State3_PauseWhileLoading);
		states[4] = game_addr(game::MAG_087_ANGELO_STRIKE_Director_State4_Sync2);
		states[5] = game_addr(game::MAG_087_ANGELO_STRIKE_Director_State5_Cue2);
		states[6] = game_addr(game::MAG_087_ANGELO_STRIKE_Director_State6_DoneAtFrame48);
		states[7] = game_addr(game::MAG_087_ANGELO_STRIKE_Director_State7_Sync3);
		states[8] = game_addr(game::MAG_087_ANGELO_STRIKE_Director_State8_FinishAtCue3);
		states[9] = game_addr(game::MAG_087_ANGELO_STRIKE_Nullsub_775140);
		ak_7742E0(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ak_7742E0(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			ak_775160(a1);
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

	uint32_t __cdecl ak_774330(uint32_t a1)
	{
		x::Battle_QueueTIMUpload_GetEOF(game_addr(game::MAG_087_ANGELO_STRIKE_AngeloTIM));
		gx::BS_Camera_ArmReturnAfterEffect();
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_087_ANGELO_STRIKE_CameraRingQueue), game_addr(game::MAG_087_ANGELO_STRIKE_CameraTask), 0x90, a1);
		if (a_73B640(1) != 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ak_7743A0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[14];
		states[0] = game_addr(game::MAG_087_ANGELO_STRIKE_Camera_State0_FaceCaster);
		states[1] = game_addr(game::MAG_087_ANGELO_STRIKE_Camera_State1_Sync1Script);
		states[2] = game_addr(game::MAG_087_ANGELO_STRIKE_Camera_State2_ChainUnlessPaused);
		states[3] = game_addr(game::MAG_087_ANGELO_STRIKE_Camera_State3_ChainRandomUnlessPaused);
		states[4] = game_addr(game::MAG_087_ANGELO_STRIKE_Camera_State4_ChainBySize);
		states[5] = game_addr(game::MAG_087_ANGELO_STRIKE_Camera_State5_ChainBySize);
		states[6] = game_addr(game::MAG_087_ANGELO_STRIKE_Camera_State6_ChainIfSmall);
		states[7] = game_addr(game::MAG_087_ANGELO_STRIKE_Camera_State7_ChainGroundPoint);
		states[8] = game_addr(game::MAG_087_ANGELO_STRIKE_Camera_State8_Chain);
		states[9] = game_addr(game::MAG_087_ANGELO_STRIKE_Camera_State9_Chain);
		states[10] = game_addr(game::MAG_087_ANGELO_STRIKE_Camera_State10_StepUntilDone);
		states[11] = game_addr(game::MAG_087_ANGELO_STRIKE_Camera_State11_WaitDirectorStep3);
		states[12] = game_addr(game::MAG_087_ANGELO_STRIKE_Camera_State12_End);
		states[13] = game_addr(game::MAG_087_ANGELO_STRIKE_Nullsub_774F60);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return b102_task_end(node, status);
	}

	uint32_t __cdecl ak_774450(uint32_t a1)
	{
		a_73AB00();
		const uint16_t ang = U16(ENT(U8(a1, 0x2C)), 0xE);
		U16(MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pCameraBlock)), 0xE8) = ang;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static inline void ak_cam_chain(uint32_t a1, uint32_t script, uint32_t from)
	{
		a_73AB20(script, from, 0);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
	}

	uint32_t __cdecl ak_7744B0(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(1) != 0)
		{
			a_73AB20(game_addr(game::MAG_087_ANGELO_STRIKE_CameraScript1), MEM<uint32_t>(game_addr(DIR_PTR)), 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_774CB0(uint32_t a1)
	{
		if (MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_PauseSwitch)) == 0 && a_73AE10() == 1)
			ak_cam_chain(a1, game_addr(game::unk_1560140), MEM<uint32_t>(game_addr(DIR_PTR)));
		return 0;
	}

	uint32_t __cdecl ak_774CE0(uint32_t a1)
	{
		if (MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_PauseSwitch)) == 0 && a_73AE10() == 1)
		{
			const uint32_t angelo = U32(MEM<uint32_t>(game_addr(DIR_PTR)), 0x18);
			const uint32_t r = x::CrtRand();
			ak_cam_chain(a1, (r & 1) ? game_addr(game::unk_1560188) : game_addr(game::unk_15601D0), angelo + 0x148);
		}
		return 0;
	}

	uint32_t __cdecl ak_774D40(uint32_t a1)
	{
		if (a_73AE10() == 1)
		{
			const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
			const uint32_t angelo = U32(d, 0x18);
			ak_cam_chain(a1, S16(d, 0x4E) < 0x1000 ? game_addr(game::unk_1560218) : game_addr(game::unk_1560260), angelo + 0x148);
		}
		return 0;
	}

	uint32_t __cdecl ak_774D90(uint32_t a1)
	{
		if (a_73AE10() == 1)
		{
			const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
			const uint32_t angelo = U32(d, 0x18);
			ak_cam_chain(a1, S16(d, 0x4E) < 0x1000 ? game_addr(game::unk_15602E0) : game_addr(game::unk_1560370), angelo + 0x148);
		}
		return 0;
	}

	uint32_t __cdecl ak_774DE0(uint32_t a1)
	{
		if (a_73AE10() == 1)
		{
			const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
			if (S16(d, 0x4E) < 0x1000)
				a_73AB20(game_addr(game::unk_1560328), U32(d, 0x18) + 0x148, 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_774E20(uint32_t a1)
	{
		if (a_73AE10() == 1)
		{
			const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
			const uint32_t angelo = U32(d, 0x18);
			alignas(4) uint8_t pt[8];
			U32(P(pt), 0) = U32(angelo, 0x148);
			U16(P(pt), 2) = 0;
			U32(P(pt), 4) = U32(angelo, 0x14C);
			ak_cam_chain(a1, S16(d, 0x4E) < 0x1000 ? game_addr(game::unk_15603B8) : game_addr(game::unk_1560400), P(pt));
		}
		return 0;
	}

	uint32_t __cdecl ak_774E90(uint32_t a1)
	{
		if (a_73AE10() == 1)
			ak_cam_chain(a1, game_addr(game::unk_1560468), MEM<uint32_t>(game_addr(DIR_PTR)));
		return 0;
	}

	uint32_t __cdecl ak_774EC0(uint32_t a1)
	{
		if (a_73AE10() == 1)
			ak_cam_chain(a1, game_addr(game::unk_15604F0), MEM<uint32_t>(game_addr(DIR_PTR)));
		return 0;
	}

	uint32_t __cdecl ak_774F10(uint32_t a1)
	{
		if (a_73AAE0(3) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl ak_774F70(uint32_t a1)
	{
		if (a_73B7E0(1) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl ak_775010(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x46) < 0x1C)
			return 0;
		if (U16(MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pModuleData)), 0x166) == 0)
		{
			a_73BB60(0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		else
			a_73BB60(1);
		return 0;
	}

	uint32_t __cdecl ak_7750E0(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		if (S16(d, 0x46) >= 0x48)
		{
			U16(d, 0x48) = 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_775120(uint32_t a1)
	{
		if (a_73B7E0(3) != 0)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_775160(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(DIR_PTR)), 0x40) == 1)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_087_ANGELO_STRIKE_AngeloQueue), game_addr(game::MAG_087_ANGELO_STRIKE_AngeloTask), 0x178, a1);
			x::Effect_BindModelContainerSetAnim(t, game_addr(game::MAG_087_ANGELO_STRIKE_AngeloModelContainer), 3);
		}
		return 0;
	}

	uint32_t __cdecl ak_7751A0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[17];
		states[0] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State0_BindRinoaAnimSet_FaceTarget);
		states[1] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State1_RunSetup);
		states[2] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State2_RunUnlessPaused);
		states[3] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State3_DustPlay_GroundCapture);
		states[4] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State4_LiftSetup);
		states[5] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State5_Lift_SpawnActorSystems);
		states[6] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State6_CarrySetup_Halo);
		states[7] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State7_Carry);
		states[8] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State8_Spin);
		states[9] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State9_Slam_ImpactPlay_Debris);
		states[10] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State10_JumpBackSetup);
		states[11] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State11_JumpBack);
		states[12] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State12_JumpBackLand);
		states[13] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State13_RestoreTarget_RinoaChain2);
		states[14] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State14_WalkOff);
		states[15] = game_addr(game::MAG_087_ANGELO_STRIKE_Angelo_State15_TurnWalkFinish);
		states[16] = game_addr(game::MAG_087_ANGELO_STRIKE_Nullsub_77F0C0);
		callp(states[S8(node, 0x29)], node);
		const uint16_t vz = U16(node, 0x128);
		const uint16_t vx = U16(node, 0x124);
		const uint16_t vy = U16(node, 0x126);
		U16(node, 0x50) = (uint16_t)(U16(node, 0x50) + vz);
		U16(node, 0x4C) = (uint16_t)(U16(node, 0x4C) + vx);
		U16(node, 0x4E) = (uint16_t)(U16(node, 0x4E) + vy);
		if ((U8(node, 0x26) & 4) == 0)
		{
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = a_746C10(node, game_addr(CREATURE_ARG), MEM<uint32_t>(game_addr(PACKET_CURSOR)));
			if ((U8(node, 0x26) & 4) == 0)
			{
				const uint8_t anim = U8(node, 0x9C), frame = U8(node, 0xA2);
				if (anim == 1)
				{
					if (frame <= 5)
						ak_775F40(node);
					else if (frame >= 0x10)
						ak_775510(node, (frame & 1) ? 2 : 3);
				}
				else if (anim == 8)
				{
					if (frame <= 5)
						ak_775F40(node);
				}
			}
		}
		const int32_t py = S16(node, 0x4E);
		const int32_t px = S16(node, 0x4C);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::unk_1D99AB0), (const void *)(node + 0x70));
		const int32_t pz = S16(node, 0x50);
		S32(node, 0x88) = py;
		const uint8_t status = U8(node, 0x26);
		S32(node, 0x84) = px;
		S32(node, 0x8C) = pz;
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl ak_775510(uint32_t a1, uint32_t a2)
	{
		static const uint32_t PTS[6] = { 0x1E, 0x21, 0x1C, 0x1D, 0x1C, 0x1D };
		const uint32_t mdl = a1 + 0x30;
		const int16_t k = (int16_t)a2;
		alignas(4) uint8_t m[0x20];
		const uint32_t M = P(m);
		const int n = k > 3 ? 2 : 4;
		for (int i = 0; i < n; i++)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_087_ANGELO_STRIKE_CameraRingQueue), game_addr(game::MAG_087_ANGELO_STRIKE_RingTask), 0x90, a1);
			if ((uint32_t)(int32_t)k <= 5)
				x::GetEffectSpawnPosition(mdl, PTS[k], 0, t + 0x1C);
			if (k <= 3)
			{
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
			}
			else
			{
				const int32_t r1 = (int32_t)x::CrtRand();
				U16(t, 0x1C) = (uint16_t)(U16(t, 0x1C) + (uint16_t)((r1 & 0x7F) - 0x40));
				const int32_t r2 = (int32_t)x::CrtRand();
				U16(t, 0x20) = (uint16_t)(U16(t, 0x20) + (uint16_t)((r2 & 0x7F) - 0x40));
				const int32_t r3 = (int32_t)x::CrtRand();
				const int32_t f = (int32_t)(int16_t)((r3 & 0x3F) + 0xC0);
				U16(t, 0x1E) = (uint16_t)((int32_t)S16(t, 0x1E) * f / 256);
				x::MAG_022_sub_8DD770(M);
				const int32_t r4 = (int32_t)x::CrtRand();
				x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)(int16_t)(((r4 & 0x3FF) - 0x200) & 0xFFF));
				const int32_t r5 = (int32_t)x::CrtRand();
				U16(t, 0x78) = 0;
				U16(t, 0x7A) = 0;
				U16(t, 0x7C) = 0;
				U16(t, 0x82) = (uint16_t)(-(r5 % 0x60));
			}
			x::matrixMultiplyVector(M, t + 0x78, t + 0x78);
		}
		return 0;
	}

	uint32_t __cdecl ak_775720(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_087_ANGELO_STRIKE_Ring_State0_Setup);
		states[1] = game_addr(game::MAG_087_ANGELO_STRIKE_Ring_State1_Rise);
		states[2] = game_addr(game::MAG_087_ANGELO_STRIKE_Nullsub_775F30);
		callp(states[S8(node, 0x29)], node);
		ak_775780(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl ak_775780(uint32_t a1)
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
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_PacketCursor));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(blk, 0x24) = 0;
		U16(blk, 0xB4) = colour;
		MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_PacketCursor)) = a_7435E0(blk, ot, 2, cursor);
		x::Field_Free(0xD8);
		return 0;
	}

	uint32_t __cdecl ak_775E50(uint32_t a1)
	{
		U32(a1, 0x4C) = game_addr(game::MAG_087_ANGELO_STRIKE_RingSequence);
		U16(a1, 0x56) = 0xFFC0;
		U16(a1, 0x52) = 7;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ak_775E70(uint32_t a1)
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

	uint32_t __cdecl ak_775F40(uint32_t a1)
	{
		alignas(4) uint8_t m[0x20];
		const uint32_t M = P(m);
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_087_ANGELO_STRIKE_CameraRingQueue), game_addr(game::MAG_087_ANGELO_STRIKE_RingTask), 0x90, a1);
		x::GetEffectSpawnPosition(a1 + 0x30, 0xF1, 0, t + 0x1C);
		U16(t, 0x1E) = 0;
		const int32_t r1 = (int32_t)x::CrtRand();
		U16(t, 0x1C) = (uint16_t)(U16(t, 0x1C) + (uint16_t)((r1 & 0xFF) - 0x80));
		const int32_t r2 = (int32_t)x::CrtRand();
		U16(t, 0x20) = (uint16_t)(U16(t, 0x20) + (uint16_t)((r2 & 0xFF) - 0x80));
		x::MAG_022_sub_8DD770(M);
		const int32_t r3 = (int32_t)x::CrtRand();
		x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)(int16_t)(r3 & 0xFFF));
		const int32_t r4 = (int32_t)x::CrtRand();
		U16(t, 0x78) = 0;
		U16(t, 0x7A) = 0;
		U16(t, 0x82) = (uint16_t)(-(r4 % 0x60));
		const int32_t r5 = (int32_t)x::CrtRand();
		U16(t, 0x7C) = (uint16_t)((r5 & 0x7FF) + 0x200);
		x::matrixMultiplyVector(M, t + 0x78, t + 0x78);
		return 0;
	}

	uint32_t __cdecl ak_776010(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t c = ENT(U8(node, 0x2C));
		const uint32_t t = gx::EntityChainLowest(ENT(U8(node, 0x2D)));
		U32(MEM<uint32_t>(game_addr(DIR_PTR)), 0x18) = node;
		gx::BindEntityModelFromComData(c, game_addr(game::MAG_087_ANGELO_STRIKE_RinoaAnimSetBlock), game_addr(game::MAG_087_ANGELO_STRIKE_RinoaAnimSetComData));
		gx::QueueChainTransformation(c, 1);
		const uint32_t d = MEM<uint32_t>(game_addr(DIR_PTR));
		U8(node, 0x26) = (uint8_t)(U8(node, 0x26) | 4);
		const uint32_t rot0 = U32(t, 0xC);
		U32(node, 0x4C) = U32(d, 0);
		const uint16_t face = U16(d, 0x4A);
		U16(node, 0x3E) = face;
		U32(node, 0x50) = U32(d, 4);
		const uint32_t rot1 = U32(t, 0x10);
		U16(t, 0xE) = (uint16_t)((face - 0x800) & 0xFFF);
		U32(node, 0x11C) = 0x1000;
		U32(node, 0x118) = 0x1000;
		U32(node, 0x114) = 0x1000;
		U32(node, 0x150) = rot0;
		U32(node, 0x154) = rot1;
		U32(node, 0x158) = U32(t, 0x1C);
		U32(node, 0x15C) = U32(t, 0x20);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ak_7760E0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t t = gx::EntityChainLowest(ENT(U8(node, 0x2D)));
		if (S16(node, 0x24) < 0xF)
			return 0;
		alignas(4) uint8_t loc[0x28];
		const uint32_t V = P(loc), M = V + 8;
		x::MAG_022_sub_8DD770(M);
		x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)S16(t, 0xE));
		U16(V, 4) = (uint16_t)((int32_t)S16(t, 0x38) / 2 - 0x400);
		U16(V, 0) = 0;
		U16(V, 2) = 0;
		x::matrixMultiplyVector(M, V, V);
		U32(node, 0x148) = U32(t, 0x1C);
		U32(node, 0x14C) = U32(t, 0x20);
		U16(node, 0x148) = (uint16_t)(U16(node, 0x148) + U16(V, 0));
		U16(node, 0x14C) = (uint16_t)(U16(node, 0x14C) + U16(V, 4));
		U16(node, 0x14A) = U16(V, 2);
		U16(V, 0) = 0;
		U16(V, 2) = 0;
		U16(V, 4) = 0xE000;
		x::matrixMultiplyVector(M, V, V);
		U32(node, 0x140) = U32(node, 0x148);
		U8(node, 0x26) = (uint8_t)(U8(node, 0x26) & 0xFB);
		U32(node, 0x144) = U32(node, 0x14C);
		U16(node, 0x140) = (uint16_t)(U16(node, 0x140) + U16(V, 0));
		U16(node, 0x142) = (uint16_t)(U16(node, 0x142) + U16(V, 2));
		U16(node, 0x144) = (uint16_t)(U16(node, 0x144) + U16(V, 4));
		U32(node, 0x4C) = U32(node, 0x140);
		U32(node, 0x50) = U32(node, 0x144);
		U16(node, 0x168) = 0;
		U16(node, 0x16A) = 0xBB;
		U16(node, 0x16C) = 0;
		U16(node, 0x16E) = 0;
		U16(node, 0x170) = 0xFE00;
		U16(node, 0x172) = 0x2E;
		x::au_re_Battle_ReadAnimation_7(node, 0);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static inline void ak_height_step(uint32_t node)
	{
		U16(node, 0x170) = (uint16_t)(U16(node, 0x170) + U16(node, 0x172));
		U16(node, 0x16E) = (uint16_t)(U16(node, 0x16E) + U16(node, 0x170));
		if (S16(node, 0x16E) > 0)
			U16(node, 0x16E) = 0;
	}

	uint32_t __cdecl ak_776260(uint32_t a1)
	{
		if (MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_PauseSwitch)) != 0)
			return 0;
		const uint32_t node = a1;
		ak_height_step(node);
		ak_7762D0(node);
		x::sub_8DD1C0(node);
		if (S16(node, 0x136) >= 0x14)
		{
			x::au_re_Battle_ReadAnimation_7(node, 0xA);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_7762D0(uint32_t a1)
	{
		const uint32_t n = a1;
		U16(n, 0x16A) = (uint16_t)(U16(n, 0x16A) + U16(n, 0x16C));
		U16(n, 0x168) = (uint16_t)(U16(n, 0x168) + U16(n, 0x16A));
		const int16_t f = S16(n, 0x168);
		if (f >= 0x1000)
		{
			U32(n, 0x4C) = U32(n, 0x148);
			U32(n, 0x50) = U32(n, 0x14C);
			U16(n, 0x168) = 0x1000;
			U16(n, 0x4E) = (uint16_t)(U16(n, 0x4E) + U16(n, 0x16E));
			return n;
		}
		const int32_t k = f;
		for (uint32_t o = 0; o < 6; o += 2)
		{
			const int16_t a = S16(n, 0x140 + o);
			U16(n, 0x4C + o) = (uint16_t)(((int32_t)S16(n, 0x148 + o) - a) * k / 4096 + a);
		}
		U16(n, 0x4E) = (uint16_t)(U16(n, 0x4E) + U16(n, 0x16E));
		return n;
	}

	uint32_t __cdecl ak_7763B0(uint32_t a1)
	{
		const uint32_t node = a1;
		ak_height_step(node);
		ak_7762D0(node);
		const uint32_t r = x::au_re_Battle_ReadAnimation_8(node);
		if (r == 1)
		{
			x::au_re_Battle_ReadAnimation_7(node, r);
			const uint32_t t = ak_776460(node, game_addr(game::MAG_087_ANGELO_STRIKE_DustPlayTask), game_addr(game::MAG_087_ANGELO_STRIKE_DustPlayLayout), 0xB0, 0, 0);
			U32(t, 0x240) = node + 0x30;
			U16(t, 0x8A) = 0;
			ak_7789B0(node);
			x::BdPlaySE(game_addr(game::MAG_087_ANGELO_STRIKE_SoundDig), 0, 0x80);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_776460(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_087_ANGELO_STRIKE_PlayQueue), a2, 0x25C, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		S32(t, 0x78) = (int32_t)(int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl ak_776510(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t mdl = U32(node, 0x240);
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		x::GetEffectSpawnPosition(mdl, 0xF1, 0, node + 0x1C);
		const int32_t s = U16(node, 0x8A) != 0 ? 0x1000 : 0xC00;
		U16(node, 0x1E) = 0;
		S32(node, 0x58) = s;
		S32(node, 0x54) = s;
		S32(node, 0x50) = s;
		a_73C280(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ak_7766B0(uint32_t a1, uint32_t a2, uint32_t a3)
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
		const uint16_t vx = U16(rec, 8);
		const uint16_t vy = U16(rec, 0xA);
		const uint16_t vz = U16(rec, 0xC);
		U16(V, 0) = vx;
		U16(V, 2) = vy;
		U16(V, 4) = vz;
		if ((U8(rec, 5) & 2) != 0)
		{
			S32(M, 0x14) = (int16_t)vx;
			S32(M, 0x18) = (int16_t)vy;
			S32(M, 0x1C) = (int16_t)vz;
		}
		else
		{
			x::GTE_SetRotMatrix(blk);
			x::GTE_LoadV0(V);
			x::GTE_MVMVA_RotV0();
			x::GTE_ReadMAC123(M + 0x14);
			x::GTE_MatrixMultiply(blk, M);
		}
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
		U32(hdr, 0x14) = (U32(rec, 4) & 0x4000) != 0 ? 0x2000 : 0x2030;
		const int32_t alpha = S16(rec, 0x24);
		U32(hdr, 0xC) = (uint32_t)alpha;
		if (alpha != 0)
		{
			U32(hdr, 0x14) = U32(hdr, 0x14) | 0xC0;
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
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = ak_776920(hdr, ot, 2, cursor);
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl ak_776920(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
			cur = ak_776A50(ctx, a2, a3, a4);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = ak_776C70(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = ak_776F00(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = ak_777380(ctx, a2, a3, cur);
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
			cur = ak_777E30(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			return ak_778330(ctx, a2, a3, cur);
		U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		return cur;
	}

	uint32_t __cdecl ak_776A50(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ak_776C70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ak_776F00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ak_777380(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ak_777E30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ak_778330(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ak_778980(uint32_t a1)
	{
		const uint32_t r = a_73C280(a1);
		if (r == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl ak_7789B0(uint32_t a1)
	{
		alignas(4) uint8_t loc[0x14];
		const uint32_t Q = P(loc), B = Q + 8;
		U16(B, 4) = 0x7FFF;
		U16(B, 2) = 0x7FFF;
		U16(B, 0) = 0x7FFF;
		U16(B, 0xA) = 0x8001;
		U16(B, 8) = 0x8001;
		U16(B, 6) = 0x8001;
		ak_778A20(game_addr(game::MAG_087_ANGELO_STRIKE_DebrisLargeModelA), B);
		U32(Q, 0) = U32(a1, 0x4C);
		U32(Q, 4) = U32(a1, 0x50);
		U16(Q, 2) = 0;
		return ak_778AB0(Q, B);
	}

	uint32_t __cdecl ak_778A20(uint32_t a1, uint32_t a2)
	{
		int32_t n = (int32_t)U32(a1, 4) - 1;
		uint32_t v = a1 + 0xC;
		const uint32_t b = a2;
		U16(b, 6) = U16(v, 0);
		U16(b, 0) = U16(v, 0);
		U16(b, 8) = U16(v, 2);
		U16(b, 2) = U16(v, 2);
		U16(b, 0xA) = U16(v, 4);
		U16(b, 4) = U16(v, 4);
		do
		{
			v += 8;
			for (uint32_t o = 0; o < 6; o += 2)
			{
				const int16_t c = S16(v, o);
				if (S16(b, o) > c)
					U16(b, o) = (uint16_t)c;
				else if (S16(b, 6 + o) < c)
					U16(b, 6 + o) = (uint16_t)c;
			}
		} while (--n != 0);
		return a2;
	}

	uint32_t __cdecl ak_778AB0(uint32_t a1, uint32_t a2)
	{
		const uint32_t box = a2;
		const int32_t minx = -(int32_t)S16(box, 0), maxx = S16(box, 6), minz = -(int32_t)S16(box, 4), maxz = S16(box, 0xA);
		const int32_t width = maxx + minx, depth = maxz + minz;
		alignas(4) int16_t rect[4] = { 0x300, 0x180, 0x40, 0x40 };
		int32_t sx = width / 64;
		int32_t sz = depth / 64;
		if (sx % 64) sx++;
		if (sz % 64) sz++;
		alignas(4) uint8_t mbuf[0x20];
		const uint32_t M = P(mbuf);
		alignas(4) int32_t sv[3];
		x::MAG_022_sub_8DD770(M);
		x::sub_8DD7E0(M, 0x400);
		S32(M, 0x14) = (minx - maxx) / 2 / sx - (int32_t)S16(a1, 0) / sx;
		const int32_t tz = (minz - maxz) / 2 / sz;
		const int32_t az = (int32_t)S16(a1, 4) / sz;
		S32(M, 0x1C) = (int32_t)MEM<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		sv[1] = 0;
		S32(M, 0x18) = az - tz;
		sv[0] = 0x1000000 / ((int32_t)shl32(width, 12) / rect[2]);
		sv[2] = 0x1000000 / ((int32_t)shl32(depth, 12) / rect[3]);
		x::scale3DMatrix(M, P(sv));
		const uint32_t env = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = env + 0x40;
		gx::SSIGPU_SetDrawEnv(env, game_addr(game::g_Battle_DrawEnv) + 0x5C * (uint32_t)(MEM<uint8_t>(game_addr(game::g_Battle_DisplayParity)) ^ 1));
		x::SSIGPU_InsertPrimAutoDepth(MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, env);
		alignas(4) int32_t ofs[2] = {};
		gx::GteReadScreenOffset(P(&ofs[1]), P(&ofs[0]));
		gx::GteSetScreenOffset((uint32_t)(int32_t)(rect[2] / 2), (uint32_t)(int32_t)(rect[3] / 2));
		x::GTE_SetRotMatrix_W(M);
		x::GTE_SetTransVector_W(M);
		if ((MEM<uint8_t>(game_addr(game::unk_1D989BD)) & 1) != 0)
		{
			const uint32_t h = x::Field_Alloc(0x2C);
			U16(h, 0x24) = MEM<uint16_t>(game_addr(game::unk_1D989BE));
			U16(h, 0x14) = 0;
			U16(h, 0x16) = 0;
			U16(h, 0x18) = 0x140;
			U16(h, 0x1A) = 0xD8;
			U32(h, 0x1C) = MEM<uint32_t>(game_addr(game::unk_1D989E4));
			U32(h, 4) = MEM<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
			U32(h, 0x20) = 0xFFFFFFFF;
			U32(h, 0x10) = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
			x::BattleModel_BuildBoneMatricesFromPose(game_addr(game::unk_1D989D0));
			x::BS_ComputeBonesWorldMatrices(game_addr(game::unk_1D989D0), M);
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = x::RenderGeometry(game_addr(game::unk_1D989C0), h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 0x10, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
			x::Field_Free(0x2C);
		}
		gx::GteSetScreenOffset((uint32_t)ofs[1], (uint32_t)ofs[0]);
		auto quad = [](uint8_t code) {
			const uint32_t pk = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = pk + 0x18;
			U32(pk, 0) = 0x5000000;
			U8(pk, 7) = code;
			U16(pk, 0x10) = 0;
			U16(pk, 8) = 0;
			U16(pk, 0x14) = 0x3F;
			U16(pk, 0xC) = 0x3F;
			U16(pk, 0xE) = 0;
			U16(pk, 0xA) = 0;
			U16(pk, 0x16) = 0x3F;
			U16(pk, 0x12) = 0x3F;
			U8(pk, 6) = 0;
			U8(pk, 5) = 0;
			U8(pk, 4) = 0;
			x::SSIGPU_InsertPrimAltViewport(MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, pk);
		};
		quad(0x2A);
		const uint32_t dm = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = dm + 0xC;
		x::sub_45BFC0(dm, 0, 0, x::sub_45C690(0, 1, 0, 0) & 0xFFFF, 0);
		x::SSIGPU_InsertPrimAutoDepth(MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, dm);
		quad(0x28);
		const uint32_t area = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = area + 0xC;
		gx::SetDrawAreaPrim(area, P(rect));
		x::SSIGPU_InsertPrimAutoDepth(MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, area);
		const uint32_t ofp = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = ofp + 0xC;
		alignas(4) int16_t xy[2] = { rect[0], rect[1] };
		gx::SetDrawOffsetPrim(ofp, P(xy));
		x::SSIGPU_InsertPrimAutoDepth(MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, ofp);
		return 0;
	}

	uint32_t __cdecl ak_778E90(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t t = gx::EntityChainLowest(ENT(U8(node, 0x2D)));
		if (x::au_re_Battle_ReadAnimation_8(node) != 1)
			return 0;
		x::au_re_Battle_ReadAnimation_7(node, 2);
		U32(node, 0x140) = U32(node, 0x4C);
		U32(node, 0x144) = U32(node, 0x50);
		x::GetEffectSpawnPosition(t, 0xF1, 0, node + 0x148);
		alignas(4) uint8_t loc[0x28];
		const uint32_t V = P(loc), M = V + 8;
		x::MAG_022_sub_8DD770(M);
		x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)S16(t, 0xE));
		U16(V, 4) = (uint16_t)((int32_t)S16(t, 0x38) / 2);
		U16(V, 0) = 0;
		U16(V, 2) = 0x200;
		x::matrixMultiplyVector(M, V, V);
		U16(node, 0x14A) = (uint16_t)(U16(node, 0x14A) + U16(V, 2));
		U16(node, 0x148) = (uint16_t)(U16(node, 0x148) + U16(V, 0));
		U16(node, 0x14C) = (uint16_t)(U16(node, 0x14C) + U16(V, 4));
		U16(node, 0x168) = 0;
		U16(node, 0x16A) = 0x801;
		U16(node, 0x16C) = 0;
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ak_778F90(uint32_t a1)
	{
		const uint32_t node = a1;
		ak_7762D0(node);
		if (U16(node, 0x136) == 1)
		{
			for (uint32_t i = 0; i < 4; i++)
			{
				const uint32_t t = a_73C100(node, game_addr(game::MAG_087_ANGELO_STRIKE_ActorSystemTask), game_addr(game::MAG_087_ANGELO_STRIKE_ActorSystemData), 0, 5, 4);
				U16(t, 0x2A0) = (uint16_t)i;
			}
		}
		if (x::au_re_Battle_ReadAnimation_8(node) == 1)
		{
			x::au_re_Battle_ReadAnimation_7(node, 3);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_7790B0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < S16(node, 0x298))
			return 0;
		const uint32_t mdl = U32(MEM<uint32_t>(game_addr(DIR_PTR)), 0x18) + 0x30;
		const int16_t k = S16(node, 0x2A0);
		const int32_t pt = k == 0 ? 0x1E : k == 1 ? 0x21 : (k != 2 ? 0x1D : 0x1C);
		x::GetEffectSpawnPosition(mdl, (uint32_t)pt, 0, node + 0x290);
		a_73F990(node);
		ak_779C10(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ak_7792E0(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState));
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

	uint32_t __cdecl ak_779560(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState));
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

	uint32_t __cdecl ak_779C10(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			ak_77DDB0();
			ak_77B460();
			ak_77B320();
			a_735440();
			sys = U32(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState), 0);
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
		U16(game_addr(game::MAG_087_ANGELO_STRIKE_ActorCount14), 0) = (uint16_t)(U16(game_addr(game::MAG_087_ANGELO_STRIKE_ActorCount14), 0) + c14);
		U16(game_addr(game::MAG_087_ANGELO_STRIKE_ActorCount16), 0) = (uint16_t)(U16(game_addr(game::MAG_087_ANGELO_STRIKE_ActorCount16), 0) + c16);
		return done;
	}

	uint32_t __cdecl ak_779F80(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ak_77A190(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ak_77A8F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ak_77AB20(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ak_77ADE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ak_77B040(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl ak_77B320(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						ak_77B3E0(act);
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
							ak_77B3E0(act);
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

	uint32_t __cdecl ak_77B3E0(uint32_t a1)
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

	uint32_t __cdecl ak_77B460(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				ak_77C730(act);
			else if (type == 1)
				ak_77B4A0(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl ak_77B4A0(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (ak_77C5E0(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_77C4D0(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState), 0);
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

	uint32_t __cdecl ak_77C5E0(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				ak_77C660(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			ak_77C660(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			ak_77C660(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl ak_77C660(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return ak_77C690(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl ak_77C690(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::unk_25B4634));
		int32_t idx = MEM<int16_t>(game_addr(game::unk_25AECD0));
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
			MEM<uint16_t>(game_addr(game::unk_25AECD0)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::unk_25AECD0)) = 0;
		return slot;
	}

	uint32_t __cdecl ak_77C730(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_087_ANGELO_STRIKE_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_087_ANGELO_STRIKE_Actor_InterpolatePosition), a1);
		ak_77C7B0(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl ak_77C7B0(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState), 0);
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
				ak_77C830(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			ak_77C830(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl ak_77C830(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::MAG_087_ANGELO_STRIKE_pCameraCopy), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::MAG_087_ANGELO_STRIKE_pCameraCopy), 0) = sp;
		uint32_t node = ak_77D010(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState), 0);
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
		ak_77DD20(node, desc);
		U32(game_addr(game::MAG_087_ANGELO_STRIKE_pCameraCopy), 0) = U32(game_addr(game::MAG_087_ANGELO_STRIKE_pCameraCopy), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl ak_77D010(uint32_t a1, uint32_t a2)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::unk_25B4B48));
		int32_t i = MEM<int16_t>(game_addr(game::unk_25B4640));
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
			if (i >= 0x31)
				i = 0;
			tries++;
			if (tries >= 0x32)
				break;
		}
		i++;
		if (i >= 0x31)
			MEM<uint16_t>(game_addr(game::unk_25B4640)) = 0;
		else
			MEM<uint16_t>(game_addr(game::unk_25B4640)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl ak_77D230(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState), 0);
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
				uint32_t g = U32(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState), 0);
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

	uint32_t __cdecl ak_77DD20(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			ak_77C690(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			ak_77C690(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			ak_77C690(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			ak_77C690(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl ak_77DDB0(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState));
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
					ak_77C690(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				ak_77C690(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pActorState));
			}
		}
		return 0;
	}

	uint32_t __cdecl ak_77DE40(uint32_t a1)
	{
		if (ak_779C10(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_77DE70(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t mdl = node + 0x30;
		const uint32_t t = gx::EntityChainLowest(ENT(U8(node, 0x2D)));
		if (x::au_re_Battle_ReadAnimation_8(node) != 1)
			return 0;
		x::au_re_Battle_ReadAnimation_7(node, 4);
		U16(node, 0x4E) = (uint16_t)(U16(node, 0x4E) + 0xFE00);
		const uint32_t pxy = U32(node, 0x4C), pz = U32(node, 0x50);
		const uint16_t py = U16(node, 0x4E);
		U32(node, 0x148) = pxy;
		U32(node, 0x140) = pxy;
		U32(node, 0x14C) = pz;
		U16(node, 0x168) = 0;
		U16(node, 0x174) = 0;
		const uint16_t ox = (uint16_t)(U16(t, 0x1C) - U16(node, 0x4C));
		U32(node, 0x144) = pz;
		const uint16_t oy = (uint16_t)(U16(t, 0x1E) - py);
		const uint16_t oz = (uint16_t)(U16(t, 0x20) - U16(node, 0x50));
		U16(node, 0x3C) = 0xC00;
		U16(node, 0x160) = ox;
		U16(node, 0x14A) = 0xE800;
		U16(node, 0x162) = oy;
		U16(node, 0x164) = oz;
		alignas(4) uint8_t m[0x20];
		const uint32_t M = P(m);
		x::MAG_022_sub_8DD770(M);
		x::MAG_022_sub_8DD8A0(M, (uint32_t)(0x1000 - (int32_t)S16(node, 0x3E)));
		x::matrixMultiplyVector(M, node + 0x160, node + 0x160);
		const uint32_t h = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_087_ANGELO_STRIKE_ModelQueue), game_addr(game::MAG_087_ANGELO_STRIKE_HaloTask), 0x98, node);
		U32(h, 0x8C) = mdl;
		const uint32_t p = ak_776460(node, game_addr(game::MAG_087_ANGELO_STRIKE_DustPlayTask), game_addr(game::MAG_087_ANGELO_STRIKE_DustPlayLayout), 0xB0, 0, 0);
		U32(p, 0x240) = mdl;
		U16(p, 0x8A) = 1;
		U16(node, 0x134) = 8;
		ak_77E260(node, 1);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ak_77DFC0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[5];
		states[0] = game_addr(game::MAG_087_ANGELO_STRIKE_Halo_State0_Setup);
		states[1] = game_addr(game::MAG_087_ANGELO_STRIKE_Halo_State1_FadeIn);
		states[2] = game_addr(game::MAG_087_ANGELO_STRIKE_Halo_State2_Wait);
		states[3] = game_addr(game::MAG_087_ANGELO_STRIKE_Halo_State3_FadeOut);
		states[4] = game_addr(game::MAG_087_ANGELO_STRIKE_Nullsub_77E250);
		callp(states[S8(node, 0x29)], node);
		alignas(4) uint8_t pt[8] = {};
		U16(node, 0x46) = (uint16_t)((U16(node, 0x46) + 0x100) & 0xFFF);
		x::GetEffectSpawnPosition(U32(node, 0x8C), 0x1D, 0, P(pt));
		S32(node, 0x34) = -(int32_t)S16(P(pt), 2);
		ak_77E060(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl ak_77E060(uint32_t a1)
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
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = ak_776920(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl ak_77E180(uint32_t a1)
	{
		S32(a1, 0x30) = 0x800;
		S32(a1, 0x38) = 0x800;
		S32(a1, 0x34) = 0x1000;
		U16(a1, 0x50) = 0x1000;
		U32(a1, 0x4C) = game_addr(game::MAG_087_ANGELO_STRIKE_HaloModel);
		U16(a1, 0x52) = 0xFE00;
		x::GetEffectSpawnPosition(U32(a1, 0x8C), 0xF1, 0, a1 + 0x1C);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		U16(a1, 0x1E) = 0;
		return 0;
	}

	uint32_t __cdecl ak_77E1E0(uint32_t a1)
	{
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0xFE00);
		if (S16(a1, 0x50) <= 0)
		{
			U16(a1, 0x50) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_77E210(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0xA)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ak_77E220(uint32_t a1)
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

	uint32_t __cdecl ak_77E260(uint32_t a1, uint32_t a2)
	{
		const uint32_t n = a1;
		const uint32_t t = gx::EntityChainLowest(ENT(U8(n, 0x2D)));
		if ((int16_t)a2 == 0)
		{
			U16(n, 0x16A) = (uint16_t)(U16(n, 0x16A) + U16(n, 0x16C));
			U16(n, 0x168) = (uint16_t)(U16(n, 0x168) + U16(n, 0x16A));
		}
		else
		{
			const int16_t i = S16(n, 0x174);
			const uint16_t f = MEM<uint16_t>(game_addr(game::MAG_087_ANGELO_STRIKE_CarryEaseTable) + (uint32_t)(int32_t)i * 2);
			U16(n, 0x168) = f;
			if (f != 0x1000)
				U16(n, 0x174) = (uint16_t)(i + 1);
		}
		const int16_t f = S16(n, 0x168);
		if (f >= 0x1000)
		{
			U32(n, 0x4C) = U32(n, 0x148);
			U16(n, 0x168) = 0x1000;
			U32(n, 0x50) = U32(n, 0x14C);
		}
		else
		{
			const int32_t k = f;
			for (uint32_t o = 0; o < 6; o += 2)
			{
				const int16_t a = S16(n, 0x140 + o);
				U16(n, 0x4C + o) = (uint16_t)(((int32_t)S16(n, 0x148 + o) - a) * k / 4096 + a);
			}
		}
		U16(n, 0x4E) = (uint16_t)(U16(n, 0x4E) + U16(n, 0x16E));
		U16(t, 0xC) = (uint16_t)((0xFC00 - U16(n, 0x3C)) & 0xFFF);
		alignas(4) uint8_t m[0x20];
		const uint32_t M = P(m);
		x::MAG_022_sub_8DD770(M);
		x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)S16(n, 0x3E));
		x::sub_8DD7E0(M, (uint32_t)((int32_t)S16(n, 0x3C) + 0x400));
		alignas(4) uint8_t v[8];
		U32(P(v), 0) = U32(n, 0x160);
		U32(P(v), 4) = U32(n, 0x164);
		x::matrixMultiplyVector(M, n + 0x160, P(v));
		const int32_t px = (int32_t)S16(n, 0x4C) + S16(P(v), 0);
		S32(t, 0x54) = px;
		U16(t, 0x1C) = (uint16_t)px;
		const int32_t py = (int32_t)S16(n, 0x4E) + S16(P(v), 2);
		S32(t, 0x58) = py;
		U16(t, 0x1E) = (uint16_t)py;
		const int32_t pz = (int32_t)S16(n, 0x50) + S16(P(v), 4);
		S32(t, 0x5C) = pz;
		U16(t, 0x20) = (uint16_t)pz;
		x::ComposeZYXRotationMatrix(t + 0xC, t + 0x40);
		uint32_t e = t;
		do
		{
			if ((U8(e, 0) & 2) != 0)
			{
				U32(e, 0x1C) = U32(t, 0x1C);
				U32(e, 0x20) = U32(t, 0x20);
				U32(e, 0xC) = U32(t, 0xC);
				U32(e, 0x10) = U32(t, 0x10);
				ff8fx::copy<(8 * 4)>((void *)(e + 0x40), (const void *)(t + 0x40));
			}
			e = U32(e, 0x8C);
		} while (e != t && e != 0);
		return 0;
	}

	uint32_t __cdecl ak_77E490(uint32_t a1)
	{
		const uint32_t node = a1;
		ak_77E260(node, 1);
		x::sub_8DD1C0(node);
		U16(node, 0x134) = (uint16_t)(U16(node, 0x134) - 1);
		if (S16(node, 0x134) <= 0)
		{
			U16(node, 0x134) = 0;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_77E4D0(uint32_t a1)
	{
		const uint32_t node = a1;
		gx::EntityChainLowest(ENT(U8(node, 0x2D)));
		U16(node, 0x176) = (uint16_t)(U16(node, 0x176) - 0x80);
		if (S16(node, 0x176) <= (int16_t)0xFD00)
			U16(node, 0x176) = 0xFD00;
		U16(node, 0x3C) = (uint16_t)(U16(node, 0x3C) + U16(node, 0x176));
		const uint16_t p = U16(node, 0x3C);
		if ((int16_t)p < 0)
		{
			U16(node, 0x134) = (uint16_t)(U16(node, 0x134) + 1);
			U16(node, 0x3C) = (uint16_t)(p & 0xFFF);
		}
		U16(node, 0x3C) = (uint16_t)(U16(node, 0x3C) & 0xFFF);
		if (S16(node, 0x134) >= 4)
		{
			U16(node, 0x3C) = 0;
			x::au_re_Battle_ReadAnimation_7(node, 5);
			const uint32_t e0 = U32(node, 0x148), s0 = U32(node, 0x140), s1 = U32(node, 0x144);
			U16(node, 0x164) = (uint16_t)(U16(node, 0x164) + 0x100);
			U32(node, 0x140) = e0;
			const uint32_t e1 = U32(node, 0x14C);
			U32(node, 0x148) = s0;
			U32(node, 0x14C) = s1;
			U32(node, 0x144) = e1;
			U16(node, 0x14A) = 0;
			U16(node, 0x168) = 0;
			U16(node, 0x16A) = 0;
			U16(node, 0x16C) = 0xC0;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		else
			ak_77E260(node, 1);
		x::sub_8DD1C0(node);
		a_73A6D0();
		return 0;
	}

	uint32_t __cdecl ak_77E600(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t mdl = node + 0x30;
		gx::EntityChainLowest(ENT(U8(node, 0x2D)));
		x::sub_8DD1C0(node);
		ak_77E260(node, 0);
		if (U16(node, 0x168) != 0x1000)
			return 0;
		x::au_re_Battle_ReadAnimation_7(node, 6);
		const uint32_t p = ak_776460(node, game_addr(game::MAG_087_ANGELO_STRIKE_ImpactPlayTask), U32(MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pModuleData)), 0x128), 0x1AC, 1, 0);
		U32(p, 0x240) = mdl;
		const uint32_t d = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_087_ANGELO_STRIKE_ModelQueue), game_addr(game::MAG_087_ANGELO_STRIKE_DebrisSpawnerTask), 0x98, node);
		U32(d, 0x1C) = U32(mdl, 0x1C);
		U32(d, 0x20) = U32(mdl, 0x20);
		U16(d, 0x1E) = 0;
		x::BdPlaySE(game_addr(game::MAG_087_ANGELO_STRIKE_SoundSlam), 0, 0x80);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ak_77E720(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t mdl = U32(node, 0x240);
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		x::GetEffectSpawnPosition(mdl, 0xF1, 0, node + 0x1C);
		U16(node, 0x1E) = 0;
		const int32_t s = (int16_t)ak_77E780(node);
		S32(node, 0x58) = s;
		S32(node, 0x54) = s;
		S32(node, 0x50) = s;
		a_73C280(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ak_77E780(uint32_t a1)
	{
		const uint32_t t = gx::EntityChainLowest(ENT(U8(a1, 0x2D)));
		const int16_t size = S16(t, 0x26);
		if (size < 0x800)
			return 0x1000;
		int32_t v = ((int32_t)size - 0x800) / 2 + 0x1000;
		if ((int16_t)v > 0x2000)
			v = 0x2000;
		return (uint32_t)v;
	}

	uint32_t __cdecl ak_77E7D0(uint32_t a1)
	{
		const uint32_t r = a_73C280(a1);
		if (r == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl ak_77E800(uint32_t a1)
	{
		const uint32_t states[2] = { game_addr(game::MAG_087_ANGELO_STRIKE_DebrisSpawner_State0_Spawn), game_addr(game::MAG_087_ANGELO_STRIKE_Nullsub_77ED40) };
		callp(states[S8(a1, 0x29)], a1);
		return task_end(a1);
	}

	uint32_t __cdecl ak_77E850(uint32_t a1)
	{
		const uint32_t node = a1;
		const int16_t f = S16(node, 0x24);
		if (f >= 6)
		{
			U8(node, 0x26) = (uint8_t)(U8(node, 0x26) | 1);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			return 0;
		}
		for (int kind = 0; kind < 2; kind++)
		{
			const int16_t fr = S16(node, 0x24);
			int32_t cnt = MEM<int16_t>((kind == 0 ? game_addr(game::MAG_087_ANGELO_STRIKE_DebrisLargeCounts) : game_addr(game::MAG_087_ANGELO_STRIKE_DebrisSmallCounts)) + (uint32_t)(int32_t)fr * 2);
			if (cnt <= 0)
				continue;
			do
			{
				const int32_t ang = (int32_t)(int16_t)(x::CrtRand() & 0xFFF);
				const int32_t dist = (int32_t)(int16_t)((x::CrtRand() & 0x3FF) + 0x100);
				const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_087_ANGELO_STRIKE_ModelQueue), kind == 0 ? game_addr(game::MAG_087_ANGELO_STRIKE_DebrisLargeTask) : game_addr(game::MAG_087_ANGELO_STRIKE_DebrisSmallTask), 0x98, node);
				U32(t, 0x1C) = U32(node, 0x1C);
				U32(t, 0x20) = U32(node, 0x20);
				U16(t, 0x1C) = (uint16_t)(U16(t, 0x1C) + (uint16_t)((int32_t)x::computeSin((uint32_t)ang) * dist / 4096));
				U16(t, 0x20) = (uint16_t)(U16(t, 0x20) + (uint16_t)((int32_t)x::computeCosine((uint32_t)ang) * dist / 4096));
				const int32_t speed = (int32_t)(int16_t)((x::CrtRand() & 0x3FF) + 0x300);
				const uint32_t up = x::CrtRand() & 0xFFF;
				U16(t, 0x7E) = (uint16_t)((kind == 0 ? 0xFFFFF200u : 0xFFFFF000u) - up);
				U16(t, 0x7C) = (uint16_t)((int32_t)x::computeSin((uint32_t)ang) * speed / 4096);
				U16(t, 0x46) = (uint16_t)ang;
				U16(t, 0x80) = (uint16_t)((int32_t)x::computeCosine((uint32_t)ang) * speed / 4096);
			} while (--cnt != 0);
		}
		return 0;
	}

	static inline uint32_t ak_debris(uint32_t node, const game::Id (&states)[3], uint16_t gravity)
	{
		callp(game_addr(states[S8(node, 0x29)]), node);
		const int32_t vx = S16(node, 0x7C);
		U16(node, 0x7E) = (uint16_t)(U16(node, 0x7E) + gravity);
		U16(node, 0x44) = (uint16_t)((U16(node, 0x96) + U16(node, 0x44)) & 0xFFF);
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + (uint16_t)(vx / 16));
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + (uint16_t)((int32_t)S16(node, 0x7E) / 16));
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + (uint16_t)((int32_t)S16(node, 0x80) / 16));
		ak_77E060(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl ak_77EAB0(uint32_t a1)
	{
		static constexpr game::Id states[3] = { game::MAG_087_ANGELO_STRIKE_DebrisLarge_State0_Setup, game::MAG_087_ANGELO_STRIKE_DebrisLarge_State1_Life, game::MAG_087_ANGELO_STRIKE_Nullsub_77EBF0 };
		return ak_debris(a1, states, 0x200);
	}

	uint32_t __cdecl ak_77EB70(uint32_t a1)
	{
		const uint32_t r1 = x::CrtRand();
		U32(a1, 0x4C) = (r1 & 1) ? game_addr(game::MAG_087_ANGELO_STRIKE_DebrisLargeModelA) : game_addr(game::MAG_087_ANGELO_STRIKE_DebrisLargeModelB);
		const int32_t s = (int32_t)x::CrtRand() % 0xFFF + 0x100;
		S32(a1, 0x38) = s;
		S32(a1, 0x30) = s;
		S32(a1, 0x34) = s;
		const uint32_t r3 = x::CrtRand();
		U16(a1, 0x96) = (uint16_t)(0xFFFFFFC0u - (r3 & 0x3F));
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ak_77EBD0(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0x19)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_77EC00(uint32_t a1)
	{
		static constexpr game::Id states[3] = { game::MAG_087_ANGELO_STRIKE_DebrisSmall_State0_Setup, game::MAG_087_ANGELO_STRIKE_DebrisSmall_State1_Life, game::MAG_087_ANGELO_STRIKE_Nullsub_77ED30 };
		return ak_debris(a1, states, 0x400);
	}

	uint32_t __cdecl ak_77ECC0(uint32_t a1)
	{
		U32(a1, 0x4C) = game_addr(game::MAG_087_ANGELO_STRIKE_DebrisSmallModel);
		const int32_t s = (int32_t)x::CrtRand() % 0x7FF + 0x800;
		S32(a1, 0x38) = s;
		S32(a1, 0x30) = s;
		S32(a1, 0x34) = s;
		const uint32_t r2 = x::CrtRand();
		U16(a1, 0x96) = (uint16_t)(0xFFFFFFC0u - (r2 & 0x3F));
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ak_77ED10(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0x10)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_77ED50(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t c = ENT(U8(node, 0x2C));
		if (x::au_re_Battle_ReadAnimation_8(node) != 1)
			return 0;
		x::au_re_Battle_ReadAnimation_7(node, 0xB);
		U32(node, 0x140) = U32(node, 0x4C);
		U32(node, 0x144) = U32(node, 0x50);
		U16(node, 0x168) = 0;
		U16(node, 0x16C) = 0;
		U16(node, 0x16E) = 0;
		U32(node, 0x148) = U32(c, 0x1C);
		U32(node, 0x14C) = U32(c, 0x20);
		U16(node, 0x16A) = 0x125;
		U16(node, 0x170) = 0xFE00;
		U16(node, 0x172) = 0x49;
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl ak_77EDF0(uint32_t a1)
	{
		const uint32_t node = a1;
		ak_height_step(node);
		ak_7762D0(node);
		if (x::au_re_Battle_ReadAnimation_8(node) == 1)
		{
			x::au_re_Battle_ReadAnimation_7(node, 7);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_77EE50(uint32_t a1)
	{
		const uint32_t node = a1;
		ak_height_step(node);
		ak_7762D0(node);
		x::sub_8DD1C0(node);
		if (S16(node, 0x136) >= 0xA)
		{
			x::au_re_Battle_ReadAnimation_7(node, 0xC);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_77EEC0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t c = ENT(U8(node, 0x2C));
		const uint32_t t = gx::EntityChainLowest(ENT(U8(node, 0x2D)));
		ak_height_step(node);
		ak_7762D0(node);
		if (x::au_re_Battle_ReadAnimation_8(node) == 1)
		{
			U32(t, 0xC) = U32(node, 0x150);
			U32(t, 0x1C) = U32(node, 0x158);
			U32(t, 0x10) = U32(node, 0x154);
			U32(t, 0x20) = U32(node, 0x15C);
			x::au_re_Battle_ReadAnimation_7(node, 8);
			gx::QueueChainTransformation(c, 2);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_77EF90(uint32_t a1)
	{
		const uint32_t node = a1;
		if (x::au_re_Battle_ReadAnimation_8(node) == 1)
		{
			x::au_re_Battle_ReadAnimation_7(node, 9);
			ak_77EFD0(node, 0x100);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_77EFD0(uint32_t a1, uint32_t a2)
	{
		alignas(4) uint8_t m[0x20];
		const uint32_t M = P(m);
		x::MAG_022_sub_8DD770(M);
		x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)S16(a1, 0x3E));
		U16(a1, 0x124) = 0;
		U16(a1, 0x126) = 0;
		U16(a1, 0x128) = (uint16_t)a2;
		return x::matrixMultiplyVector(M, a1 + 0x124, a1 + 0x124);
	}

	uint32_t __cdecl ak_77F030(uint32_t a1)
	{
		const uint32_t node = a1;
		ak_77F070(node);
		ak_77EFD0(node, 0x100);
		x::sub_8DD1C0(node);
		if (S16(node, 0x136) >= 0x12)
		{
			U8(node, 0x26) = (uint8_t)(U8(node, 0x26) | 5);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl ak_77F070(uint32_t a1)
	{
		U16(a1, 0x3E) = (uint16_t)(U16(a1, 0x3E) & 0xFFF);
		const int16_t f = S16(a1, 0x3E);
		if (f > 0x800)
		{
			const uint16_t v = (uint16_t)((f + 0x10) & 0xFFF);
			U16(a1, 0x3E) = v;
			if ((int16_t)v < 0x800)
				U16(a1, 0x3E) = 0;
		}
		else if (f < 0x800)
		{
			const uint16_t v = (uint16_t)((f - 0x10) & 0xFFF);
			U16(a1, 0x3E) = v;
			if ((int16_t)v > 0x800)
				U16(a1, 0x3E) = 0;
		}
		return a1;
	}

	uint32_t __cdecl ak_77F0D0(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::MAG_087_ANGELO_STRIKE_pDirector));
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
		{ game::MAG_087_ANGELO_STRIKE_Sequence_State0_Advance_773BA0, (void *)a_73A0D0, "087 MAG_087_sub_773BA0" },
		{ game::MAG_087_ANGELO_STRIKE_Sequence_State0_Advance_773BB0, (void *)a_73A0D0, "087 MAG_087_sub_773BB0" },
		{ game::MAG_087_ANGELO_STRIKE_Sequence_State3_SpawnEmitter, (void *)a_73A170, "087 MAG_087_sub_773C40" },
		{ game::MAG_087_ANGELO_STRIKE_EmitterTask, (void *)a_73A1A0, "087 MAG_087_sub_773C70" },
		{ game::MAG_087_ANGELO_STRIKE_LoadScriptTask, (void *)a_73A380, "087 MAG_087_sub_773E30" },
		{ game::MAG_087_ANGELO_STRIKE_LoadScript_State0_Command, (void *)a_73A3E0, "087 sub_773E90" },
		{ game::MAG_087_ANGELO_STRIKE_LoadCallback_QueueTIM, (void *)a_73A580, "087 sub_774030" },
		{ game::MAG_087_ANGELO_STRIKE_LoadCallback_SummonStream, (void *)a_73A5B0, "087 sub_774060" },
		{ game::MAG_087_ANGELO_STRIKE_LoadCb_ClearBusy_774090, (void *)a_73A5E0, "087 sub_774090" },
		{ game::MAG_087_ANGELO_STRIKE_LoadCb_ClearBusy_7740A0, (void *)a_73A5E0, "087 sub_7740A0" },
		{ game::MAG_087_ANGELO_STRIKE_LoadCb_ClearBusy_7740B0, (void *)a_73A5E0, "087 sub_7740B0" },
		{ game::MAG_087_ANGELO_STRIKE_LoadCb_ClearBusy_7740C0, (void *)a_73A5E0, "087 sub_7740C0" },
		{ game::MAG_087_ANGELO_STRIKE_LoadScript_RecordFile, (void *)a_73A620, "087 sub_7740D0" },
		{ game::MAG_087_ANGELO_STRIKE_LoadScript_State1_Wait, (void *)a_73A660, "087 sub_774110" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_774180, (void *)a_73A6D0, "087 nullsub_1282" },
		{ game::MAG_087_ANGELO_STRIKE_Director_SyncPoint, (void *)a_73B640, "087 sub_774370" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_ResetBlock, (void *)a_73AB00, "087 sub_774490" },
		{ game::MAG_087_ANGELO_STRIKE_Director_StepReached, (void *)a_73AAE0, "087 sub_7744E0" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_StepScript, (void *)a_73AE10, "087 sub_774500" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_WriteBattleCamera, (void *)a_73B160, "087 sub_774850" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_StartScript, (void *)a_73AB20, "087 sub_7749C0" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_State10_StepUntilDone, (void *)a_73B2D0, "087 sub_774EF0" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_State12_End, (void *)a_73B750, "087 sub_774F30" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_774F50, (void *)a_73A6D0, "087 nullsub_1283" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_774F60, (void *)a_73A6D0, "087 nullsub_1284" },
		{ game::MAG_087_ANGELO_STRIKE_Director_CueReached, (void *)a_73B7E0, "087 sub_774F90" },
		{ game::MAG_087_ANGELO_STRIKE_Director_State2_LoadStep, (void *)a_73A930, "087 sub_774FB0" },
		{ game::MAG_087_ANGELO_STRIKE_LoadScript_TryArmStep, (void *)a_73A950, "087 sub_774FD0" },
		{ game::MAG_087_ANGELO_STRIKE_SetModulePause, (void *)a_73BB60, "087 sub_775050" },
		{ game::MAG_087_ANGELO_STRIKE_Director_State4_Sync2, (void *)a_73B8A0, "087 sub_7750A0" },
		{ game::MAG_087_ANGELO_STRIKE_Director_State5_Cue2, (void *)a_73B8C0, "087 sub_7750C0" },
		{ game::MAG_087_ANGELO_STRIKE_Director_State7_Sync3, (void *)a_733B70, "087 sub_775100" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_775140, (void *)a_73A6D0, "087 nullsub_1285" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_775150, (void *)a_73A6D0, "087 nullsub_1286" },
		{ game::MAG_087_ANGELO_STRIKE_Creature_DrawModel, (void *)a_746C10, "087 sub_775330" },
		{ game::InitEffectSequenceFromData_c7, (void *)a_7435E0, "087 InitEffectSequenceFromData_c7" },
		{ game::MAG_087_ANGELO_STRIKE_Sequence_DrawSpriteStrip, (void *)a_743720, "087 sub_7759E0" },
		{ game::MAG_087_ANGELO_STRIKE_Sequence_StepFrame, (void *)a_743C20, "087 sub_775F00" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_775F30, (void *)a_73A6D0, "087 nullsub_1287" },
		{ game::MAG_087_ANGELO_STRIKE_DustPlayTask, (void *)a_73A380, "087 sub_7764B0" },
		{ game::MAG_087_ANGELO_STRIKE_Play_Step, (void *)a_73C280, "087 sub_776580" },
		{ game::MAG_087_ANGELO_STRIKE_EmitG3, (void *)a_73D770, "087 sub_777900" },
		{ game::MAG_087_ANGELO_STRIKE_EmitG4, (void *)a_73D9C0, "087 sub_777B50" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_7789A0, (void *)a_73A6D0, "087 nullsub_1288" },
		{ game::MAG_087_ANGELO_STRIKE_SpawnActorSystemTask, (void *)a_73C100, "087 sub_779000" },
		{ game::MAG_087_ANGELO_STRIKE_ActorSystemTask, (void *)a_73A380, "087 sub_779050" },
		{ game::MAG_087_ANGELO_STRIKE_DirectorSetup, (void *)a_73F990, "087 sub_779130" },
		{ game::MAG_087_ANGELO_STRIKE_Director_StartPositions, (void *)a_73FC20, "087 sub_7793C0" },
		{ game::MAG_087_ANGELO_STRIKE_Director_Mode3Setup, (void *)a_73FE90, "087 sub_779630" },
		{ game::MAG_087_ANGELO_STRIKE_Director_TargetPositions, (void *)a_73FFB0, "087 sub_779750" },
		{ game::MAG_087_ANGELO_STRIKE_RelocateActorData, (void *)a_740210, "087 sub_7799B0" },
		{ game::MAG_087_ANGELO_STRIKE_ActSys_DrawModelPass, (void *)a_735440, "087 sub_779CA0" },
		{ game::MAG_087_ANGELO_STRIKE_ActSys_DrawModelActor, (void *)a_7354A0, "087 sub_779D00" },
		{ game::MAG_087_ANGELO_STRIKE_RenderPrimLists, (void *)a_7355E0, "087 sub_779E40" },
		{ game::MAG_087_ANGELO_STRIKE_PrimList_TexTri_77A410, (void *)a_735BB0, "087 sub_77A410" },
		{ game::MAG_087_ANGELO_STRIKE_PrimList_TexQuad_77A650, (void *)a_735DF0, "087 sub_77A650" },
		{ game::MAG_087_ANGELO_STRIKE_ActorMotion, (void *)a_740910, "087 sub_77B530" },
		{ game::MAG_087_ANGELO_STRIKE_DampValue, (void *)a_740F70, "087 sub_77BB90" },
		{ game::MAG_087_ANGELO_STRIKE_DampVector, (void *)a_740FC0, "087 sub_77BBE0" },
		{ game::MAG_087_ANGELO_STRIKE_Instance_AdvanceTexFrame, (void *)a_741050, "087 sub_77BC70" },
		{ game::MAG_087_ANGELO_STRIKE_InstanceBuildTransform, (void *)a_741120, "087 sub_77BD40" },
		{ game::MAG_087_ANGELO_STRIKE_Particle_LifeCountdown, (void *)a_741B60, "087 sub_77C780" },
		{ game::MAG_087_RandRange, (void *)a_742290, "087 MAG_087_RandRange" },
		{ game::MAG_087_ANGELO_STRIKE_RandRangeWide, (void *)a_7422C0, "087 sub_77CEE0" },
		{ game::MAG_087_ANGELO_STRIKE_RandVec3, (void *)a_742300, "087 sub_77CF20" },
		{ game::MAG_087_ANGELO_STRIKE_JitterAngles3, (void *)a_742350, "087 sub_77CF70" },
		{ game::MAG_087_RandAngleJitter, (void *)a_7423A0, "087 MAG_087_RandAngleJitter" },
		{ game::MAG_087_ANGELO_STRIKE_GetWord8Minus1, (void *)a_7424B0, "087 sub_77D0D0" },
		{ game::MAG_087_ANGELO_STRIKE_Particle_AttachToOwner, (void *)a_7424C0, "087 sub_77D0E0" },
		{ game::MAG_087_ANGELO_STRIKE_ParticleList_Append, (void *)a_742CA0, "087 sub_77D8C0" },
		{ game::MAG_087_ANGELO_STRIKE_ParticleList_Unlink, (void *)a_742CD0, "087 sub_77D8F0" },
		{ game::MAG_087_ANGELO_STRIKE_Particle_Init, (void *)a_742D00, "087 sub_77D920" },
		{ game::MAG_087_ANGELO_STRIKE_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "087 sub_77DAD0" },
		{ game::MAG_087_ANGELO_STRIKE_Particle_PlaceAtPoint, (void *)a_742FF0, "087 sub_77DC10" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_77DE60, (void *)a_73A6D0, "087 nullsub_1297" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_77E250, (void *)a_73A6D0, "087 nullsub_1289" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_77E5F0, (void *)a_73A6D0, "087 nullsub_1290" },
		{ game::MAG_087_ANGELO_STRIKE_ImpactPlayTask, (void *)a_73A380, "087 sub_77E6C0" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_77E7F0, (void *)a_73A6D0, "087 nullsub_1291" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_77EBF0, (void *)a_73A6D0, "087 nullsub_1292" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_77ED30, (void *)a_73A6D0, "087 nullsub_1293" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_77ED40, (void *)a_73A6D0, "087 nullsub_1294" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_77F0C0, (void *)a_73A6D0, "087 nullsub_1295" },
		{ game::MAG_087_ANGELO_STRIKE_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "087 MAG_087_sub_77F110" },
		{ game::MAG_087_ANGELO_STRIKE_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "087 MAG_087_sub_77F130" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_77F150, (void *)a_73A6D0, "087 nullsub_1296" },
		{ game::MAG_087_ANGELO_STRIKE_Sequence_State4_NextTargetLoop, (void *)a_747500, "087 MAG_087_sub_77F160" },
		{ game::MAG_087_ANGELO_STRIKE_Sequence_State0_Advance_77F1A0, (void *)a_73A0D0, "087 MAG_087_sub_77F1A0" },
		{ game::MAG_087_ANGELO_STRIKE_Sequence_State6_WaitQueuesEmpty, (void *)a_747550, "087 MAG_087_sub_77F1B0" },
		{ game::MAG_087_ANGELO_STRIKE_Sequence_State7_RequestBattleSync, (void *)a_747560, "087 MAG_087_sub_77F1C0" },
		{ game::MAG_087_ANGELO_STRIKE_Sequence_State8_WaitBattleSyncDone, (void *)a_747590, "087 MAG_087_sub_77F1F0" },
		{ game::MAG_087_ANGELO_STRIKE_Sequence_State9_Finish, (void *)a_7475A0, "087 MAG_087_sub_77F200" },
		{ game::MAG_087_ANGELO_STRIKE_Nullsub_77F220, (void *)a_73A6D0, "087 nullsub_1281" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_087_ANGELO_STRIKE_MasterTask, (void *)ak_773A10, "087 ak_773A10" },
		{ game::MAG_087_ANGELO_STRIKE_Master_State2_CarveActorPools, (void *)ak_773BC0, "087 ak_773BC0" },
		{ game::MAG_087_ANGELO_STRIKE_ActorPools_Clear, (void *)ak_773C00, "087 ak_773C00" },
		{ game::MAG_087_ANGELO_STRIKE_Master_State2_InitDirector, (void *)ak_773CF0, "087 ak_773CF0" },
		{ game::MAG_087_ANGELO_STRIKE_ModuleDataSetup, (void *)ak_773D40, "087 ak_773D40" },
		{ game::MAG_087_ANGELO_STRIKE_DirectorDataSetup, (void *)ak_774190, "087 ak_774190" },
		{ game::MAG_087_ANGELO_STRIKE_DirectorTask, (void *)ak_774240, "087 ak_774240" },
		{ game::MAG_087_ANGELO_STRIKE_Director_PhaseBookkeeping, (void *)ak_7742E0, "087 ak_7742E0" },
		{ game::MAG_087_ANGELO_STRIKE_Director_State0_UploadTIM_CameraTask_Sync1, (void *)ak_774330, "087 ak_774330" },
		{ game::MAG_087_ANGELO_STRIKE_CameraTask, (void *)ak_7743A0, "087 ak_7743A0" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_State0_FaceCaster, (void *)ak_774450, "087 ak_774450" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_State1_Sync1Script, (void *)ak_7744B0, "087 ak_7744B0" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_State2_ChainUnlessPaused, (void *)ak_774CB0, "087 ak_774CB0" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_State3_ChainRandomUnlessPaused, (void *)ak_774CE0, "087 ak_774CE0" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_State4_ChainBySize, (void *)ak_774D40, "087 ak_774D40" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_State5_ChainBySize, (void *)ak_774D90, "087 ak_774D90" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_State6_ChainIfSmall, (void *)ak_774DE0, "087 ak_774DE0" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_State7_ChainGroundPoint, (void *)ak_774E20, "087 ak_774E20" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_State8_Chain, (void *)ak_774E90, "087 ak_774E90" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_State9_Chain, (void *)ak_774EC0, "087 ak_774EC0" },
		{ game::MAG_087_ANGELO_STRIKE_Camera_State11_WaitDirectorStep3, (void *)ak_774F10, "087 ak_774F10" },
		{ game::MAG_087_ANGELO_STRIKE_Director_State1_Cue1, (void *)ak_774F70, "087 ak_774F70" },
		{ game::MAG_087_ANGELO_STRIKE_Director_State3_PauseWhileLoading, (void *)ak_775010, "087 ak_775010" },
		{ game::MAG_087_ANGELO_STRIKE_Director_State6_DoneAtFrame48, (void *)ak_7750E0, "087 ak_7750E0" },
		{ game::MAG_087_ANGELO_STRIKE_Director_State8_FinishAtCue3, (void *)ak_775120, "087 ak_775120" },
		{ game::MAG_087_ANGELO_STRIKE_SpawnAngeloCreature, (void *)ak_775160, "087 ak_775160" },
		{ game::MAG_087_ANGELO_STRIKE_AngeloTask, (void *)ak_7751A0, "087 ak_7751A0" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_FootRings, (void *)ak_775510, "087 ak_775510" },
		{ game::MAG_087_ANGELO_STRIKE_RingTask, (void *)ak_775720, "087 ak_775720" },
		{ game::MAG_087_ANGELO_STRIKE_Ring_Draw, (void *)ak_775780, "087 ak_775780" },
		{ game::MAG_087_ANGELO_STRIKE_Ring_State0_Setup, (void *)ak_775E50, "087 ak_775E50" },
		{ game::MAG_087_ANGELO_STRIKE_Ring_State1_Rise, (void *)ak_775E70, "087 ak_775E70" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_RingAtF1, (void *)ak_775F40, "087 ak_775F40" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State0_BindRinoaAnimSet_FaceTarget, (void *)ak_776010, "087 ak_776010" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State1_RunSetup, (void *)ak_7760E0, "087 ak_7760E0" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State2_RunUnlessPaused, (void *)ak_776260, "087 ak_776260" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_MoveStep, (void *)ak_7762D0, "087 ak_7762D0" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State3_DustPlay_GroundCapture, (void *)ak_7763B0, "087 ak_7763B0" },
		{ game::MAG_087_ANGELO_STRIKE_SpawnPlayTask, (void *)ak_776460, "087 ak_776460" },
		{ game::MAG_087_ANGELO_STRIKE_DustPlay_State0_Setup, (void *)ak_776510, "087 ak_776510" },
		{ game::MAG_087_ANGELO_STRIKE_Play_ObjectCallback, (void *)ak_7766B0, "087 ak_7766B0" },
		{ game::MAG_087_ANGELO_STRIKE_RenderModelPrims, (void *)ak_776920, "087 ak_776920" },
		{ game::MAG_087_ANGELO_STRIKE_PrimList_FlatTri, (void *)ak_776A50, "087 ak_776A50" },
		{ game::MAG_087_ANGELO_STRIKE_PrimList_FlatQuad, (void *)ak_776C70, "087 ak_776C70" },
		{ game::MAG_087_ANGELO_STRIKE_PrimList_TexTri_776F00, (void *)ak_776F00, "087 ak_776F00" },
		{ game::MAG_087_ANGELO_STRIKE_PrimList_TexQuad_777380, (void *)ak_777380, "087 ak_777380" },
		{ game::MAG_087_ANGELO_STRIKE_PrimList_GouraudTexTriScroll, (void *)ak_777E30, "087 ak_777E30" },
		{ game::MAG_087_ANGELO_STRIKE_PrimList_GT4, (void *)ak_778330, "087 ak_778330" },
		{ game::MAG_087_ANGELO_STRIKE_DustPlay_State1_Play, (void *)ak_778980, "087 ak_778980" },
		{ game::MAG_087_ANGELO_STRIKE_GroundCapture, (void *)ak_7789B0, "087 ak_7789B0" },
		{ game::MAG_087_ANGELO_STRIKE_ModelBoundingBox, (void *)ak_778A20, "087 ak_778A20" },
		{ game::MAG_087_ANGELO_STRIKE_GroundCapture_Render, (void *)ak_778AB0, "087 ak_778AB0" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State4_LiftSetup, (void *)ak_778E90, "087 ak_778E90" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State5_Lift_SpawnActorSystems, (void *)ak_778F90, "087 ak_778F90" },
		{ game::MAG_087_ANGELO_STRIKE_ActorSystem_State0_FollowFootPoint, (void *)ak_7790B0, "087 ak_7790B0" },
		{ game::MAG_087_ANGELO_STRIKE_ResetTrailHistory, (void *)ak_7792E0, "087 ak_7792E0" },
		{ game::MAG_087_ANGELO_STRIKE_FillTrailRows, (void *)ak_779560, "087 ak_779560" },
		{ game::MAG_087_ANGELO_STRIKE_ActorSystem_Step, (void *)ak_779C10, "087 ak_779C10" },
		{ game::MAG_087_ANGELO_STRIKE_ModelList_FlatTri, (void *)ak_779F80, "087 ak_779F80" },
		{ game::MAG_087_ANGELO_STRIKE_ModelList_FlatQuad, (void *)ak_77A190, "087 ak_77A190" },
		{ game::MAG_087_ANGELO_STRIKE_ModelList_GouraudTri, (void *)ak_77A8F0, "087 ak_77A8F0" },
		{ game::MAG_087_ANGELO_STRIKE_ModelList_GouraudQuad, (void *)ak_77AB20, "087 ak_77AB20" },
		{ game::MAG_087_ANGELO_STRIKE_ModelList_GouraudTexTri, (void *)ak_77ADE0, "087 ak_77ADE0" },
		{ game::MAG_087_ANGELO_STRIKE_ModelList_GouraudTexQuad, (void *)ak_77B040, "087 ak_77B040" },
		{ game::MAG_087_ANGELO_STRIKE_ActSys_DrawSpritePass, (void *)ak_77B320, "087 ak_77B320" },
		{ game::MAG_087_ANGELO_STRIKE_ActSys_DrawActorSprite, (void *)ak_77B3E0, "087 ak_77B3E0" },
		{ game::MAG_087_ANGELO_STRIKE_ActSys_UpdatePass, (void *)ak_77B460, "087 ak_77B460" },
		{ game::MAG_087_ANGELO_STRIKE_ActorStep, (void *)ak_77B4A0, "087 ak_77B4A0" },
		{ game::MAG_087_ANGELO_STRIKE_MorphVertexBlock, (void *)ak_77C4D0, "087 ak_77C4D0" },
		{ game::MAG_087_ANGELO_STRIKE_Instance_EndOfLifeTest, (void *)ak_77C5E0, "087 ak_77C5E0" },
		{ game::MAG_087_ANGELO_STRIKE_Instance_SpawnFollowUp, (void *)ak_77C660, "087 ak_77C660" },
		{ game::MAG_087_ANGELO_STRIKE_AllocParticle, (void *)ak_77C690, "087 ak_77C690" },
		{ game::MAG_087_ANGELO_STRIKE_ActSys_UpdateEmitter, (void *)ak_77C730, "087 ak_77C730" },
		{ game::MAG_087_ANGELO_STRIKE_ActSys_EmitterSpawnStep, (void *)ak_77C7B0, "087 ak_77C7B0" },
		{ game::MAG_087_ANGELO_STRIKE_ActSys_SpawnParticles, (void *)ak_77C830, "087 ak_77C830" },
		{ game::MAG_087_ANGELO_STRIKE_ActSys_AllocActor, (void *)ak_77D010, "087 ak_77D010" },
		{ game::MAG_087_ANGELO_STRIKE_Actor_InterpolatePosition, (void *)ak_77D230, "087 ak_77D230" },
		{ game::MAG_087_ANGELO_STRIKE_SpawnSubEffects, (void *)ak_77DD20, "087 ak_77DD20" },
		{ game::MAG_087_ANGELO_STRIKE_Director_SpawnGroupEffects, (void *)ak_77DDB0, "087 ak_77DDB0" },
		{ game::MAG_087_ANGELO_STRIKE_ActorSystem_State1_Step, (void *)ak_77DE40, "087 ak_77DE40" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State6_CarrySetup_Halo, (void *)ak_77DE70, "087 ak_77DE70" },
		{ game::MAG_087_ANGELO_STRIKE_HaloTask, (void *)ak_77DFC0, "087 ak_77DFC0" },
		{ game::MAG_087_ANGELO_STRIKE_ModelPiece_Draw, (void *)ak_77E060, "087 ak_77E060" },
		{ game::MAG_087_ANGELO_STRIKE_Halo_State0_Setup, (void *)ak_77E180, "087 ak_77E180" },
		{ game::MAG_087_ANGELO_STRIKE_Halo_State1_FadeIn, (void *)ak_77E1E0, "087 ak_77E1E0" },
		{ game::MAG_087_ANGELO_STRIKE_Halo_State2_Wait, (void *)ak_77E210, "087 ak_77E210" },
		{ game::MAG_087_ANGELO_STRIKE_Halo_State3_FadeOut, (void *)ak_77E220, "087 ak_77E220" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_CarryStep_MoveTarget, (void *)ak_77E260, "087 ak_77E260" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State7_Carry, (void *)ak_77E490, "087 ak_77E490" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State8_Spin, (void *)ak_77E4D0, "087 ak_77E4D0" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State9_Slam_ImpactPlay_Debris, (void *)ak_77E600, "087 ak_77E600" },
		{ game::MAG_087_ANGELO_STRIKE_ImpactPlay_State0_Setup, (void *)ak_77E720, "087 ak_77E720" },
		{ game::MAG_087_ANGELO_STRIKE_ImpactScaleFromTargetSize, (void *)ak_77E780, "087 ak_77E780" },
		{ game::MAG_087_ANGELO_STRIKE_ImpactPlay_State1_Play, (void *)ak_77E7D0, "087 ak_77E7D0" },
		{ game::MAG_087_ANGELO_STRIKE_DebrisSpawnerTask, (void *)ak_77E800, "087 ak_77E800" },
		{ game::MAG_087_ANGELO_STRIKE_DebrisSpawner_State0_Spawn, (void *)ak_77E850, "087 ak_77E850" },
		{ game::MAG_087_ANGELO_STRIKE_DebrisLargeTask, (void *)ak_77EAB0, "087 ak_77EAB0" },
		{ game::MAG_087_ANGELO_STRIKE_DebrisLarge_State0_Setup, (void *)ak_77EB70, "087 ak_77EB70" },
		{ game::MAG_087_ANGELO_STRIKE_DebrisLarge_State1_Life, (void *)ak_77EBD0, "087 ak_77EBD0" },
		{ game::MAG_087_ANGELO_STRIKE_DebrisSmallTask, (void *)ak_77EC00, "087 ak_77EC00" },
		{ game::MAG_087_ANGELO_STRIKE_DebrisSmall_State0_Setup, (void *)ak_77ECC0, "087 ak_77ECC0" },
		{ game::MAG_087_ANGELO_STRIKE_DebrisSmall_State1_Life, (void *)ak_77ED10, "087 ak_77ED10" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State10_JumpBackSetup, (void *)ak_77ED50, "087 ak_77ED50" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State11_JumpBack, (void *)ak_77EDF0, "087 ak_77EDF0" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State12_JumpBackLand, (void *)ak_77EE50, "087 ak_77EE50" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State13_RestoreTarget_RinoaChain2, (void *)ak_77EEC0, "087 ak_77EEC0" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State14_WalkOff, (void *)ak_77EF90, "087 ak_77EF90" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_SetWalkVelocity, (void *)ak_77EFD0, "087 ak_77EFD0" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_State15_TurnWalkFinish, (void *)ak_77F030, "087 ak_77F030" },
		{ game::MAG_087_ANGELO_STRIKE_Angelo_TurnToFront, (void *)ak_77F070, "087 ak_77F070" },
		{ game::MAG_087_ANGELO_STRIKE_DamageState_ApplyActionResult, (void *)ak_77F0D0, "087 ak_77F0D0" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag087_angelo_strike()
	{
		act::register_module(87);
		for (const act::strike87::ModPort *p = act::strike87::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(87, p->addr, p->port, p->name);
		for (const act::strike87::ModPort *p = act::strike87::PORTS; p->addr; p++)
			act::register_module_port(87, p->addr, p->port, p->name);
	}
}

