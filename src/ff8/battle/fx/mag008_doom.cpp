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

// Effect 8: Doom

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
namespace doom
{
	static const Mod MOD_008 = { "doom", 8, game::MAG_008_DOOM_FL, game::MAG_007_MEGA_PHOENIX_FL,
		{ {}, {}, game::p_Doom_PhaseData, {}, {}, game::unk_2758884, {}, {}, {}, {}, {}, {}, game::MAG_008_FilePtr, game::g_Doom_PacketCursor, game::MAG_008_DirectorQueue, game::unk_2758884, game::unk_2758888, game::MAG_008_SmokeQueue, {}, game::unk_275ABDC, game::MAG_008_PrimModelQueue, game::MAG_008_DustWobbleQueue, game::unk_275DF48, game::unk_275DF4C, game::MAG_008_ActorSystemQueue, game::MAG_008_ScreenTintQueue, game::MAG_008_EmitterQueue, game::unk_275E828, game::unk_275E82C, game::unk_275E830, game::MAG_008_AssetTablePtr, game::MAG_008_ReaperQueue, game::unk_275F3E8, {}, game::unk_275F5C8, game::unk_275F910, game::MAG_008_LoadScriptQueue, game::MAG_008_ScratchSP, game::unk_275F9D4, game::MAG_008_ActorStatePtr },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_008_DirectorSetup, game::MAG_008_ActSys_Step, game::MAG_008_ActSys_DrawPass, game::MAG_008_ActSys_DrawPrimActor, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_008_ActSys_DrawSpritePass, game::MAG_008_ActSys_UpdatePass, game::MAG_008_ActorStep, game::MAG_008_ActSys_UpdateEmitter, game::MAG_008_Particle_LifeCountdown, game::MAG_008_ActSys_EmitterSpawnStep, game::MAG_008_ActSys_SpawnParticles, game::MAG_008_RandVec3, game::MAG_008_JitterAngles3, game::MAG_008_ActSys_AllocParticleSlot, game::MAG_008_GetWord8Minus1, game::MAG_008_Particle_Init, game::MAG_008_SpawnSubEffects, game::MAG_008_Director_SpawnGroupEffects, {}, game::MAG_008_Master_State0_Advance, game::MAG_008_Master_State1_Advance, game::MAG_008_Master_CarveActorPools, game::MAG_008_Master_State3_SpawnEmitter, game::MAG_008_EmitterTask, game::MAG_008_Master_InitDirector, {}, game::MAG_008_LoadCb_UploadTim, game::MAG_008_LoadCb_SummonStream, game::MAG_008_LoadCb_ClearBusy, game::MAG_008_LoadCb_ClearBusy_2, game::MAG_008_LoadCb_ClearBusy_3, game::MAG_008_LoadCb_ClearBusy_4, game::MAG_008_LoadScript_RecordFile, {}, {}, game::MAG_008_LoadScript_TryArmStep, {}, {}, {}, {}, {}, {}, {}, game::MAG_008_DirSyncGate, {}, game::MAG_008_DirPhaseGE, game::MAG_008_PrimPlayer_DrawCallback, {}, {}, {}, {}, {}, game::MAG_008_ResetTrailHistory, game::MAG_008_Director_StartPositions, game::MAG_008_FillTrailRows, game::MAG_008_Director_Mode3Setup, game::MAG_008_Director_TargetPositions, game::MAG_008_RelocateActorData, game::MAG_008_ActSys_DrawActorSprite, game::MAG_008_ActorMotion, game::MAG_008_DampValue, game::MAG_008_DampVector, game::MAG_008_Instance_AdvanceTexFrame, game::MAG_008_Instance_BuildTransform, game::MAG_008_MorphVertexBlock, game::MAG_008_Instance_EndOfLifeTest, game::MAG_008_Instance_SpawnFollowUp, game::MAG_008_AllocParticle, game::MAG_008_RandRange, game::MAG_008_RandRangeWide, game::MAG_008_RandAngleJitter, game::MAG_008_Particle_AttachToOwner, game::MAG_008_Actor_InterpolatePosition, {}, game::MAG_008_ParticleList_Unlink, game::MAG_008_Particle_PlaceAtAnchorSet, game::MAG_008_Particle_PlaceAtPoint, game::MAG_008_SpriteStrip_DrawQuads, game::MAG_008_ActSys_Step, game::MAG_008_Emitter_State1_ApplyDamage, game::MAG_008_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_008_Emitter_State3_FinishReleaseRoot, game::MAG_008_Emitter_State4_Empty, game::MAG_008_Master_State4_NextTargetLoop, game::MAG_008_Master_State5_Advance, game::MAG_008_Master_State6_WaitQueuesEmpty, game::MAG_008_Master_State7_RequestBattleSync, game::MAG_008_Master_State8_WaitBattleSyncDone, game::MAG_008_Master_State9_Finish, game::nullsub_1833 } };

	static constexpr game::Id PACKET_CURSOR = game::g_Doom_PacketCursor;
	static constexpr game::Id ACTOR_STATE = game::MAG_008_ActorStatePtr;
	static constexpr game::Id SCRATCH_SP = game::MAG_008_ScratchSP;
	static constexpr game::Id PHASE_DATA = game::p_Doom_PhaseData;
	static constexpr game::Id CLOCK_BLOCK = game::p_Doom_ClockBlock;
	static constexpr game::Id REAPER_CTX = game::g_Doom_ReaperRenderCtx;
	static constexpr game::Id ORIG_Master = game::MAG_008_Master_Task;
	static constexpr game::Id ORIG_Reaper = game::MAG_008_Reaper_Task;
	static constexpr game::Id ORIG_Prim = game::MAG_008_Scythe_Task;
	static constexpr game::Id DUST_FLIPBOOK = game::unk_1634F28;

	namespace dx
	{
		static inline uint32_t sub_45E0B0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadIR123_u16))(a1); }
		static inline uint32_t sub_45E9D0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPF))(); }
		static inline uint32_t sub_45EBF0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPL))(); }
		static inline uint32_t GTE_SetLightMatrix(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_SetLightMatrix))(a1); }
		static inline uint32_t GTE_SetBackgroundVector(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetBackgroundVector))(a1, a2, a3); }
		static inline uint32_t GTE_MVMVA_LightV0_Bk() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
		static inline uint32_t GTE_StoreSXY012_PolyFT3(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreSXY012_PolyFT3))(a1); }
		static inline uint32_t GTE_StoreSXY012_PolyFT3_2(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreSXY012_PolyFT3_2))(a1); }
		static inline uint32_t GTE_LoadV012_Packed(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadV012Dwords))(a1); }
		static inline uint32_t GTE_StoreSXY012_PolyG4(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::sub_45E2C0))(a1); }
		static inline uint32_t Effect_BuildMatrixFromDirAndUp(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::Effect_BuildMatrixFromDirAndUp))(a1, a2, a3); }
	}
	using namespace dx;
	namespace gx = dx;

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
	static inline void next_state(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }
	static inline uint32_t t1_data() { return MEM<uint32_t>(game_addr(PHASE_DATA)); }
	static inline void t1_next_state(uint32_t node) { next_state(node); }
	static inline void t4_next_state(uint32_t node) { next_state(node); }
	static inline void b103_next_state(uint32_t node) { next_state(node); }
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

	uint32_t __cdecl dm_8C4B30(uint32_t a1);
	uint32_t __cdecl dm_8C4CE0(uint32_t a1);
	uint32_t __cdecl dm_8C4D20(void);
	uint32_t __cdecl dm_8C4E10(uint32_t a1);
	uint32_t __cdecl dm_8C4E60(void);
	uint32_t __cdecl dm_8C52C0(uint32_t a1);
	uint32_t __cdecl dm_8C5350(uint32_t a1);
	uint32_t __cdecl dm_8C5410(uint32_t a1);
	uint32_t __cdecl dm_8C5460(uint32_t a1);
	uint32_t __cdecl dm_8C54E0(uint32_t a1);
	uint32_t __cdecl dm_8C5610(uint32_t a1);
	uint32_t __cdecl dm_8C5650(uint32_t a1);
	uint32_t __cdecl dm_8C56A0(uint32_t a1);
	uint32_t __cdecl dm_8C5710(uint32_t a1);
	uint32_t __cdecl dm_8C5860(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl dm_8C5910(uint32_t a1);
	uint32_t __cdecl dm_8C5B10(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl dm_8C5DD0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dm_8C5F00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dm_8C6120(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dm_8C63B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dm_8C68A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dm_8C73C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dm_8C7920(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dm_8C7FF0(uint32_t a1);
	uint32_t __cdecl dm_8C80C0(uint32_t a1);
	uint32_t __cdecl dm_8C8120(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dm_8C8350(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dm_8C8400(uint32_t a1);
	uint32_t __cdecl dm_8C84A0(void);
	uint32_t __cdecl dm_8C8500(uint32_t a1);
	uint32_t __cdecl dm_8C87F0(uint32_t a1);
	uint32_t __cdecl dm_8C8840(uint32_t a1);
	uint32_t __cdecl dm_8C8870(uint32_t a1);
	uint32_t __cdecl dm_8C8890(uint32_t a1);
	uint32_t __cdecl dm_8C8930(uint32_t a1);
	uint32_t __cdecl dm_8C8B10(uint32_t a1);
	uint32_t __cdecl dm_8C8D90(void);
	uint32_t __cdecl dm_8C9440(uint32_t a1);
	uint32_t __cdecl dm_8C94D0(void);
	uint32_t __cdecl dm_8C9530(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dm_8C9690(void);
	uint32_t __cdecl dm_8C9750(uint32_t a1);
	uint32_t __cdecl dm_8C97D0(void);
	uint32_t __cdecl dm_8C9810(uint32_t a1);
	uint32_t __cdecl dm_8CA840(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dm_8CA950(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dm_8CA9D0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dm_8CAA00(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl dm_8CAAA0(uint32_t a1);
	uint32_t __cdecl dm_8CAB20(uint32_t a1);
	uint32_t __cdecl dm_8CABA0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl dm_8CB380(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dm_8CB5A0(uint32_t a1);
	uint32_t __cdecl dm_8CC090(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dm_8CC120(void);
	uint32_t __cdecl dm_8CC1B0(uint32_t a1);
	uint32_t __cdecl dm_8CC280(uint32_t a1);
	uint32_t __cdecl dm_8CC330(uint32_t a1);
	uint32_t __cdecl dm_8CC350(uint32_t a1);
	uint32_t __cdecl dm_8CC3A0(uint32_t a1);
	uint32_t __cdecl dm_8CC450(uint32_t a1);
	uint32_t __cdecl dm_8CC6B0(uint32_t a1);
	uint32_t __cdecl dm_8CC7B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dm_8CCF80(uint32_t a1);
	uint32_t __cdecl dm_8CCFE0(uint32_t a1);
	uint32_t __cdecl dm_8CD020(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl dm_8CD090(uint32_t a1);
	uint32_t __cdecl dm_8CD0E0(uint32_t a1);
	uint32_t __cdecl dm_8CD120(uint32_t a1);
	uint32_t __cdecl dm_8CD1A0(uint32_t a1);
	uint32_t __cdecl dm_8CD1D0(uint32_t a1);
	uint32_t __cdecl dm_8CD200(uint32_t a1);
	uint32_t __cdecl dm_8CD250(uint32_t a1);
	uint32_t __cdecl dm_8CD290(uint32_t a1);
	uint32_t __cdecl dm_8CD2B0(uint32_t a1);
	uint32_t __cdecl dm_8CD310(uint32_t a1, uint32_t a2);
	uint32_t __cdecl dm_8CD430(uint32_t a1);
	uint32_t __cdecl dm_8CD510(uint32_t a1);
	uint32_t __cdecl dm_8CD540(uint32_t a1);
	uint32_t __cdecl dm_8CD670(uint32_t a1);
	uint32_t __cdecl dm_8CD770(uint32_t a1);
	uint32_t __cdecl dm_8CDE40(uint32_t a1);
	uint32_t __cdecl dm_8CDE60(uint32_t a1);
	uint32_t __cdecl dm_8CDF30(uint32_t a1);
	uint32_t __cdecl dm_8CDF60(uint32_t a1);
	uint32_t __cdecl dm_8CDFB0(uint32_t a1);
	uint32_t __cdecl dm_8CE040(uint32_t a1);
	uint32_t __cdecl dm_8CE130(uint32_t a1);
	uint32_t __cdecl dm_8CE1B0(uint32_t a1);
	uint32_t __cdecl dm_8CE1E0(uint32_t a1);
	uint32_t __cdecl dm_8CE200(uint32_t a1);
	uint32_t __cdecl dm_8CE250(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace doom
{
	uint32_t __cdecl dm_8C4CE0(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::MAG_008_FileArenaCursor));
		MEM<uint32_t>(game_addr(game::MAG_008_ParticlePoolPtr)) = p;
		p += 0x438;
		MEM<uint32_t>(game_addr(game::MAG_008_ActorPoolPtr)) = p;
		p += 0x6900;
		MEM<uint32_t>(game_addr(game::MAG_008_FileArenaCursor)) = p;
		dm_8C4D20();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl dm_8C4D20(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_008_ParticlePoolPtr)), 0x438);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_008_ActorPoolPtr)), 0x6900);
		MEM<uint16_t>(game_addr(game::MAG_008_ParticleCursor)) = 0;
		MEM<uint16_t>(game_addr(game::MAG_008_ActorCursor)) = 0;
		MEM<uint16_t>(game_addr(game::unk_275F238)) = 0;
		MEM<uint16_t>(game_addr(game::unk_2758508)) = 0;
		return 0;
	}

	uint32_t __cdecl dm_8C4E10(uint32_t a1)
	{
		dm_8C52C0(a1);
		dm_8C4E60();
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_008_LoadScriptQueue), game_addr(game::MAG_008_FileLoadScriptTask), 0x30, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_008_DirectorQueue), game_addr(game::MAG_008_Director_Task), 0x48, a1);
		t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl dm_8C5410(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			dm_8C5710(a1);
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

	uint32_t __cdecl dm_8C54E0(uint32_t a1)
	{
		uint32_t r = a_73B7E0(1);
		if (r == 0) return 0;
		x::BdPlaySE(game_addr(game::unk_1634F24), 0, 0x80);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl dm_8C5610(uint32_t a1)
	{
		uint32_t blk = U32(game_addr(game::p_Doom_PhaseData), 0);
		if (S16(blk, 0x46) <= 0x2D) return blk;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl dm_8C5650(uint32_t a1)
	{
		if (a_73B7E0(5) != 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0x44) = 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl dm_8C5860(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_008_PrimModelQueue), a2, 0x458, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		U32(t, 0x78) = (uint32_t)(int32_t)(int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl dm_8C5DD0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		static const Draw lists[8] = { dm_8C5F00, dm_8C6120, dm_8C63B0, dm_8C68A0, a_73D770, a_73D9C0, dm_8C73C0, dm_8C7920 };
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

	uint32_t __cdecl dm_8C5F00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl dm_8C6120(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl dm_8C63B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl dm_8C68A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl dm_8C73C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl dm_8C7920(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl dm_8C8840(uint32_t a1)
	{
		const uint32_t r = a_73C280(a1);
		if (r == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl dm_8C8930(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			a_73F990(a1);
			dm_8C9440(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl dm_8C8B10(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_008_ActorStatePtr));
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

	uint32_t __cdecl dm_8C8D90(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_008_ActorStatePtr));
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

	uint32_t __cdecl dm_8C9440(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::MAG_008_ActorStatePtr), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			dm_8CC120();
			dm_8C97D0();
			dm_8C9690();
			dm_8C94D0();
			sys = U32(game_addr(game::MAG_008_ActorStatePtr), 0);
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
		U16(game_addr(game::unk_275F910), 0) = (uint16_t)(U16(game_addr(game::unk_275F910), 0) + c14);
		U16(game_addr(game::unk_275F3E8), 0) = (uint16_t)(U16(game_addr(game::unk_275F3E8), 0) + c16);
		return done;
	}

	uint32_t __cdecl dm_8C94D0(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_008_ActorStatePtr), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::MAG_008_ActorStatePtr), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					dm_8C9530(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl dm_8C9530(uint32_t a1, uint32_t a2)
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
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = dm_8C5DD0(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
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
				MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = dm_8C5DD0(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
			}
		}
		const uint32_t top = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 0x68;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = top;
		return top;
	}

	uint32_t __cdecl dm_8C9690(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::MAG_008_ActorStatePtr)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::MAG_008_ActorStatePtr));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						dm_8C9750(act);
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
							dm_8C9750(act);
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

	uint32_t __cdecl dm_8C9750(uint32_t a1)
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

	uint32_t __cdecl dm_8C97D0(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_008_ActorStatePtr), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				dm_8CAAA0(act);
			else if (type == 1)
				dm_8C9810(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl dm_8C9810(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::MAG_008_ActorStatePtr));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (dm_8CA950(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::MAG_008_ActorStatePtr));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl dm_8CA840(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::MAG_008_ActorStatePtr), 0);
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

	uint32_t __cdecl dm_8CA950(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				dm_8CA9D0(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			dm_8CA9D0(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			dm_8CA9D0(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl dm_8CA9D0(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return dm_8CAA00(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl dm_8CAA00(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_008_ParticlePoolPtr));
		int32_t idx = MEM<int16_t>(game_addr(game::MAG_008_ParticleCursor));
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
			if (idx >= 9)
				idx = 0;
			tries++;
		} while (tries < 0xA);
		idx++;
		if (idx < 9)
			MEM<uint16_t>(game_addr(game::MAG_008_ParticleCursor)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::MAG_008_ParticleCursor)) = 0;
		return slot;
	}

	uint32_t __cdecl dm_8CAAA0(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_008_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_008_Actor_InterpolatePosition), a1);
		dm_8CAB20(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl dm_8CAB20(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::MAG_008_ActorStatePtr), 0);
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
				dm_8CABA0(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			dm_8CABA0(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl dm_8CABA0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::MAG_008_ScratchSP), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::MAG_008_ScratchSP), 0) = sp;
		uint32_t node = dm_8CB380(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::MAG_008_ActorStatePtr), 0);
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
		dm_8CC090(node, desc);
		U32(game_addr(game::MAG_008_ScratchSP), 0) = U32(game_addr(game::MAG_008_ScratchSP), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl dm_8CB380(uint32_t a1, uint32_t a2)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_008_ActorPoolPtr));
		int32_t i = MEM<int16_t>(game_addr(game::MAG_008_ActorCursor));
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
			MEM<uint16_t>(game_addr(game::MAG_008_ActorCursor)) = 0;
		else
			MEM<uint16_t>(game_addr(game::MAG_008_ActorCursor)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl dm_8CB5A0(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::MAG_008_ActorStatePtr), 0);
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
				uint32_t g = U32(game_addr(game::MAG_008_ActorStatePtr), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::MAG_008_ActorStatePtr), 0);
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

	uint32_t __cdecl dm_8CC090(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			dm_8CAA00(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			dm_8CAA00(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			dm_8CAA00(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			dm_8CAA00(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl dm_8CC120(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::MAG_008_ActorStatePtr));
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
					dm_8CAA00(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::MAG_008_ActorStatePtr));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				dm_8CAA00(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::MAG_008_ActorStatePtr));
			}
		}
		return 0;
	}

	uint32_t __cdecl dm_8CC1B0(uint32_t a1)
	{
		if (dm_8C9440(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl dm_8CC280(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0xA0);
		if (S16(a1, 0x1C) >= 0x600)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0x600;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int i = 4; i != 0; i--, p += 0x2C)
			MEM<uint16_t>(p) = level;
		return 0;
	}

	uint32_t __cdecl dm_8CC350(uint32_t a1)
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

	uint32_t __cdecl dm_8CC450(uint32_t a1)
	{
		const uint16_t f = U16(a1, 0x26);
		if (f & 4)
			return 0;
		if (f & 8)
		{
			x::ComposeZYXRotationMatrix(a1 + 0x3C, a1 + 0x70);
			S32(a1, 0x84) = S16(a1, 0x4C);
			S32(a1, 0x88) = S16(a1, 0x4E);
			S32(a1, 0x8C) = S16(a1, 0x50);
			x::scale3DMatrix(a1 + 0x70, a1 + 0x114);
			return dm_8CC6B0(game_addr(REAPER_CTX));
		}
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = a_746C10(a1, game_addr(game::unk_275E838), MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		return 0;
	}

	uint32_t __cdecl dm_8CC6B0(uint32_t a1)
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
		dm_8CC7B0(U32(blk, 0x64), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, ctx);
		const uint32_t second = U32(blk, 0x78);
		if (second != 0)
			dm_8CC7B0(U32(second, 4), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, ctx);
		x::BattleModel_BuildBoneMatricesFromPose(anim);
		return 0;
	}

	uint32_t __cdecl dm_8CD020(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		x::MAG_007_sub_8DCC00(a1, 0x278);
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

	uint32_t __cdecl dm_8CD1D0(uint32_t a1)
	{
		uint32_t r = x::au_re_Battle_ReadAnimation_8(a1);
		if (r == 1)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 2);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl dm_8CD250(uint32_t a1)
	{
		if (x::au_re_Battle_ReadAnimation_8(a1) != 1)
			return 0;
		x::au_re_Battle_ReadAnimation_7(a1, 4);
		U16(a1, 0x134) = 7;
		b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl dm_8CD290(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x134) = (uint16_t)(U16(node, 0x134) - 1);
		if (S16(node, 0x134) <= 0)
			t4_next_state(node);
		return 0;
	}

	uint32_t __cdecl dm_8CD2B0(uint32_t a1)
	{
		U16(a1, 0x13A) = (uint16_t)(U16(a1, 0x13A) + 0x100);
		if (S16(a1, 0x13A) >= 0x1000)
		{
			U8(a1, 0x26) |= 5;
			U16(a1, 0x13A) = 0x1000;
			b103_next_state(a1);
		}
		U16(a1, 0x13C) = U16(a1, 0x13A);
		dm_8CD310(a1, 3);
		return dm_8CD430(a1);
	}

	uint32_t __cdecl dm_8CD310(uint32_t a1, uint32_t a2)
	{
		const uint32_t node = a1;
		a_733950(node + 0x30, U16(node, 0x13A), a2);
		return 0;
	}

	uint32_t __cdecl dm_8CD430(uint32_t a1)
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

	uint32_t __cdecl dm_8CD670(uint32_t a1)
	{
		uint32_t tab[3] = { game_addr(game::MAG_008_DustSparkle_State0_InitFlipbook), game_addr(game::MAG_008_DustSparkle_State1_Animate), game_addr(game::MAG_008_DustSparkle_State2_Empty) };
		callp(tab[S8(a1, 0x29)], a1);
		uint32_t v = a1 + 0x8E;
		for (int32_t n = 4; n != 0; n--)
		{
			int16_t c = S16(v, -2);
			U16(v, -2) = (uint16_t)((uint16_t)c - (int32_t)c / 16);
			c = S16(v, 0);
			U16(v, 0) = (uint16_t)((uint16_t)c - (int32_t)c / 16);
			c = S16(v, 2);
			U16(v, 2) = (uint16_t)((uint16_t)c - (int32_t)c / 16);
			U16(v, -0x22) = (uint16_t)(U16(v, -0x22) + (int32_t)S16(v, -2) / 16);
			U16(v, -0x20) = (uint16_t)(U16(v, -0x20) + (int32_t)S16(v, 0) / 16);
			U16(v, -0x1E) = (uint16_t)(U16(v, -0x1E) + (int32_t)S16(v, 2) / 16);
			U32(a1, 0x1C) = U32(v, -0x22);
			U32(a1, 0x20) = U32(v, -0x1E);
			dm_8CD770(a1);
			v += 8;
		}
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		if ((status & 1) != 0 && U8(a1, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(a1);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl dm_8CD770(uint32_t a1)
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
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::g_Doom_PacketCursor));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(blk, 0x24) = 0;
		U16(blk, 0xB4) = colour;
		MEM<uint32_t>(game_addr(game::g_Doom_PacketCursor)) = a_7435E0(blk, ot, 2, cursor);
		x::Field_Free(0xD8);
		return 0;
	}

	uint32_t __cdecl dm_8CDE40(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U32(a1, 0x4C) = game_addr(DUST_FLIPBOOK);
		U16(a1, 0x52) = 0x10;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl dm_8CDE60(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl dm_8CE040(uint32_t a1)
	{
		const uint32_t node = a1;
		if (U8(node, 0x26) & 4)
			return 0;
		uint32_t mbuf[8] = {};
		const uint32_t m = P(mbuf);
		x::MAG_022_sub_8DD770(m);
		x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(node, 0x44));
		x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0x48));
		x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(node, 0x46));
		int32_t tx = S16(node, 0x1C);
		int32_t ty = S16(node, 0x1E);
		int32_t tz = S16(node, 0x20);
		S32(m, 0x14) = tx;
		S32(m, 0x18) = ty;
		S32(m, 0x1C) = tz;
		x::scale3DMatrix(m, node + 0x30);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), m, m);
		x::GTE_SetRotMatrix_W(m);
		x::GTE_SetTransVector_W(m);
		uint32_t prm = x::Field_Alloc(0x58);
		uint32_t model = U32(node, 0x4C);
		uint32_t v40 = U32(node, 0x40);
		U32(prm, 0) = model;
		U32(prm, 8) = v40;
		int32_t v50 = S16(node, 0x50);
		uint32_t cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		S32(prm, 0xC) = v50;
		uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U32(prm, 0x1C) = 0xF0;
		uint32_t next = x::Effect_RenderPrimModel(prm, ot, 2, cursor);
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = next;
		x::Field_Free(0x58);
		return 0;
	}

	uint32_t __cdecl dm_8CE1E0(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x54) = (uint16_t)(U16(node, 0x54) - 1);
		if (S16(node, 0x54) <= 0)
			t4_next_state(node);
		return 0;
	}

	uint32_t __cdecl dm_8CE200(uint32_t a1)
	{
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0x400);
		if (S16(a1, 0x50) >= 0x1000)
		{
			int8_t st = S8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U16(a1, 0x50) = 0x1000;
			S8(a1, 0x29) = (int8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl dm_8CE250(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::p_Doom_PhaseData));
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

	uint32_t __cdecl dm_8C4B30(uint32_t a1)
	{
		g_mod = &MOD_008;

		const uint32_t node = a1;
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		MEM<uint32_t>(game_addr(game::MAG_008_ScratchSP)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_2758888)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		const uint32_t states[11] = { game_addr(game::MAG_008_Master_State0_Advance), game_addr(game::MAG_008_Master_State1_Advance), game_addr(game::MAG_008_Master_CarveActorPools), game_addr(game::MAG_008_Master_State3_SpawnEmitter), game_addr(game::MAG_008_Master_State4_NextTargetLoop), game_addr(game::MAG_008_Master_State5_Advance),
			game_addr(game::MAG_008_Master_State6_WaitQueuesEmpty), game_addr(game::MAG_008_Master_State7_RequestBattleSync), game_addr(game::MAG_008_Master_State8_WaitBattleSyncDone), game_addr(game::MAG_008_Master_State9_Finish), game_addr(game::nullsub_1833) };
		if ((U8(node, 0x5C) & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_275E82C));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_275DF4C));
			MEM<uint32_t>(game_addr(game::g_Doom_PacketCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_2758884)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_275E828));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_275DF48));
			MEM<uint32_t>(game_addr(game::g_Doom_PacketCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_2758884)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_275F910)) = 0;
		MEM<uint16_t>(game_addr(game::unk_275F3E8)) = 0;
		static constexpr game::Id queues[9] = { game::MAG_008_EmitterQueue, game::MAG_008_DirectorQueue, game::MAG_008_LoadScriptQueue, game::MAG_008_ReaperQueue, game::MAG_008_ActorSystemQueue, game::MAG_008_DustWobbleQueue, game::MAG_008_SmokeQueue, game::MAG_008_PrimModelQueue, game::MAG_008_ScreenTintQueue };
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

	uint32_t __cdecl dm_8C4E60(void)
	{
		MEM<uint32_t>(game_addr(game::unk_275E830)) = game_addr(game::unk_1636C78);
		MEM<uint32_t>(game_addr(game::MAG_008_AssetTablePtr)) = game_addr(game::MAG_008_AssetTable);
		x::MAG_007_sub_8DCC00(game_addr(game::MAG_008_AssetTable), 0x16C);
		const uint32_t t = MEM<uint32_t>(game_addr(game::MAG_008_AssetTablePtr));
		uint32_t c = MEM<uint32_t>(game_addr(game::MAG_008_FileArenaCursor));
		U32(t, 0x100) = c;
		c += 0xA000;
		U32(t, 0x114) = game_addr(game::unk_19EB018);
		MEM<uint32_t>(game_addr(game::MAG_008_FileArenaCursor)) = c;
		U32(t, 0x118) = game_addr(game::unk_19EB018);
		U32(t, 0x11C) = game_addr(game::unk_19EB018);
		U32(t, 0x120) = game_addr(game::unk_19EB018);
		U32(t, 0x104) = game_addr(game::unk_19F0324);
		U32(t, 0x108) = game_addr(game::unk_19F0324);
		U32(t, 0x10C) = game_addr(game::unk_19F0324);
		U32(t, 0x110) = game_addr(game::unk_19F0324);
		U32(t, 0x124) = game_addr(game::unk_19EB018);
		U32(t, 0x128) = game_addr(game::unk_19ECBB8);
		U32(t, 0x12C) = game_addr(game::unk_19ED83C);
		for (int32_t o = 0x130; o <= 0x160; o += 4)
			U32(t, o) = game_addr(game::unk_19F0324);
		U16(t, 0x164) = 0;
		U16(t, 0x166) = 0;
		return t;
	}

	uint32_t __cdecl dm_8C52C0(uint32_t a1)
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

	uint32_t __cdecl dm_8C5350(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t states[15] = { game_addr(game::MAG_008_UploadReaperTIMs), game_addr(game::MAG_008_Director_State1_WaitDirGate1), game_addr(game::MAG_008_Director_State2_PlaySoundAtCut1), game_addr(game::MAG_008_Director_State3_ArmLoadScriptStep), game_addr(game::MAG_008_Director_State4_WaitDirGate2), game_addr(game::MAG_008_Director_State5_WaitLoadScriptIdle),
			game_addr(game::MAG_008_Director_State6_WaitDirGate3), game_addr(game::MAG_008_Director_State7_WaitDirPhase4), game_addr(game::MAG_008_Director_State8_WaitCutFrame2D), game_addr(game::MAG_008_Director_State9_WaitDirGate5), game_addr(game::MAG_008_Director_State10_Mark5SetTimer), game_addr(game::MAG_008_Director_State11_CountdownTimer), game_addr(game::MAG_008_Director_State12_Mark6SetPhaseFlag), game_addr(game::MAG_008_Director_State13_Finish),
			game_addr(game::MAG_008_Director_State14_Empty) };
		dm_8C5410(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl dm_8C5460(uint32_t a1)
	{
		x::Battle_QueueTIMUpload_GetEOF(game_addr(game::MAG008_Reaper_BodyTIM));
		x::Battle_QueueTIMUpload_GetEOF(game_addr(game::MAG008_Reaper_WingsTIM));
		U16(a1, 0x44) = 0xC;
		next_state(a1);
		return a1;
	}

	uint32_t __cdecl dm_8C56A0(uint32_t a1)
	{
		if (a_73B640(6) == 0)
			return 0;
		U16(MEM<uint32_t>(game_addr(PHASE_DATA)), 0x48) = 1;
		next_state(a1);
		return a1;
	}

	uint32_t __cdecl dm_8C5710(uint32_t a1)
	{
		const int32_t k = S16(MEM<uint32_t>(game_addr(PHASE_DATA)), 0x40);
		if (k == 2)
		{
			a_73C100(a1, game_addr(game::MAG_008_Aura_Task), game_addr(game::unk_1818B78), 0, 0x2D, 2);
			const uint32_t reaper = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_008_ReaperQueue), game_addr(game::MAG_008_Reaper_Task), 0x140, a1);
			x::Effect_BindModelContainerSetAnim(reaper, game_addr(game::MAG008_Reaper_ModelContainer), 3);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_008_DustWobbleQueue), game_addr(game::MAG_008_DustTask), 0xB0, a1);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_008_SmokeQueue), game_addr(game::MAG_008_SmokeTask), 0x58, a1);
			return x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_008_DustWobbleQueue), game_addr(game::MAG_008_StageWobbleInTask), 0xB0, a1);
		}
		if (k == 4)
		{
			dm_8C5860(a1, game_addr(game::MAG_008_Scythe_Task), U32(MEM<uint32_t>(game_addr(game::MAG_008_AssetTablePtr)), 0x124), 0xC0, 0, 0);
			const uint32_t w = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_008_ScreenTintQueue), game_addr(game::Effect_ScreenTintTask), 0x40, a1);
			U32(w, 0x34) = game_addr(game::unk_163ACB4);
			U16(w, 0x38) = 0x1A;
			return x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_008_DustWobbleQueue), game_addr(game::MAG_008_StageWobbleOutTask), 0xB0, a1);
		}
		return (uint32_t)(k - 4);
	}

	uint32_t __cdecl dm_8C5910(uint32_t a1)
	{
		const uint32_t node = a1;
		if (U16(node, 0x24) != 0xB)
			return 0;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		const uint32_t d = MEM<uint32_t>(game_addr(PHASE_DATA));
		U16(node, 0x62) = U16(d, 0x4A);
		U32(node, 0x1C) = U32(d, 0);
		U32(node, 0x20) = U32(d, 4);
		alignas(4) uint8_t loc[0x30] = {};
		const uint32_t L = P(loc);
		x::MAG_022_sub_8DD770(L + 0x10);
		x::MAG_022_sub_8DD8A0(L + 0x10, (uint32_t)(int32_t)S16(node, 0x62));
		U16(L, 0) = 0xFFEF;
		U16(L, 2) = 0xFBCC;
		U16(L, 4) = 0xFE67;
		x::matrixMultiplyVector(L + 0x10, L, L + 8);
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + U16(L, 0xC));
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + U16(L, 8));
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + U16(L, 0xA));
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		U16(node, 0x60) = 0xF00;
		U16(node, 0x88) = 0xFFF0;
		a_73C280(node);
		next_state(node);
		return 0;
	}

	uint32_t __cdecl dm_8C5B10(uint32_t a1, uint32_t a2, uint32_t a3)
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
		const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		U32(hdr, 0x10) = (uint32_t)(int32_t)S16(blk, 0x58);
		U16(hdr, 0x22) = 0x100;
		U16(hdr, 0x20) = 0x100;
		U16(hdr, 0x2A) = 0x100;
		U16(hdr, 0x28) = 0x100;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = dm_8C5DD0(hdr, ot, 2, cursor);
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl dm_8C7FF0(uint32_t a1)
	{
		const uint32_t r = a_73C280(a1);
		if ((uint16_t)r == 0x13)
		{
			const uint32_t t = dm_8C5860(a1, game_addr(game::MAG_008_DoomClock_Task), U32(MEM<uint32_t>(game_addr(game::MAG_008_AssetTablePtr)), 0x128), 0x8C, 1, 0);
			U32(t, 0x1C) = U32(a1, 0x1C);
			U32(t, 0x20) = U32(a1, 0x20);
			return t;
		}
		if ((uint16_t)r == 0)
		{
			U8(a1, 0x26) |= 1;
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl dm_8C80C0(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		dm_8C8120(node + 0x1C, (uint32_t)U8(node, 0x2D));
		dm_8C8350(node + 0x1C, (uint32_t)U16(node, 0x454));
		a_73C280(node);
		next_state(node);
		return 0;
	}

	uint32_t __cdecl dm_8C8120(uint32_t a1, uint32_t a2)
	{
		alignas(4) uint8_t loc[0x30] = {};
		const uint32_t L = P(loc);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(CLOCK_BLOCK)), 0x58);
		uint32_t b = MEM<uint32_t>(game_addr(CLOCK_BLOCK));
		U8(b, 0x48) = 0x60;
		U8(b, 0x49) = 0;
		U8(b, 0x4A) = 0x20;
		U8(b, 0x44) = 0x30;
		U8(b, 0x45) = 0;
		U8(b, 0x46) = 0x10;
		U8(b, 0x40) = 0;
		U8(b, 0x41) = 0;
		U8(b, 0x42) = 0;
		U32(b, 0x30) = U32(a1, 0);
		U32(b, 0x34) = U32(a1, 4);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)((int32_t)(int16_t)a2 * 0x9C);
		x::GetEffectSpawnPosition(ent, 0xF1, 0, b + 0x38);
		b = MEM<uint32_t>(game_addr(CLOCK_BLOCK));
		const int32_t ddx = sub32(S16(b, 0x38), S16(b, 0x30));
		const int32_t ddy = sub32(S16(b, 0x3A), S16(b, 0x32));
		const int32_t ddz = sub32(S16(b, 0x3C), S16(b, 0x34));
		const int32_t sq = add32(add32(mul32(ddz, ddz), mul32(ddy, ddy)), mul32(ddx, ddx));
		U32(b, 0x4C) = x::Sqrt((uint32_t)sq);
		b = MEM<uint32_t>(game_addr(CLOCK_BLOCK));
		U16(L, 0) = 0;
		U32(b, 0x20) = 0x1000;
		U32(b, 0x24) = 0x1000;
		U16(L, 2) = 0x1000;
		U32(b, 0x28) = U32(b, 0x4C);
		U16(L, 4) = 0;
		U16(L, 8) = (uint16_t)(U16(b, 0x38) - U16(b, 0x30));
		U16(L, 0xA) = (uint16_t)(U16(b, 0x3A) - U16(b, 0x32));
		U16(L, 0xC) = (uint16_t)(U16(b, 0x3C) - U16(b, 0x34));
		Effect_BuildMatrixFromDirAndUp(b, L + 8, L);
		b = MEM<uint32_t>(game_addr(CLOCK_BLOCK));
		S32(b, 0x14) = S16(b, 0x30);
		S32(b, 0x18) = S16(b, 0x32);
		S32(b, 0x1C) = S16(b, 0x34);
		uint32_t p = game_addr(game::MAG_008_ClockRingTemplate);
		for (int32_t row = 0; row < 0x11; row++)
		{
			const uint16_t z = (uint16_t)(row * 0xF0);
			for (uint32_t s = game_addr(game::unk_1636C08); s < game_addr(game::unk_1636C10); s += 2)
			{
				U16(p, 0) = MEM<uint16_t>(s);
				U16(p, 2) = 0;
				U16(p, 4) = z;
				p += 8;
			}
		}
		const uint32_t M = L + 0x10;
		{
			x::MAG_022_sub_8DD770(M);
			x::sub_8DD960(M, 0);
			uint32_t src = game_addr(game::MAG_008_ClockRingTemplate), dst = game_addr(game::MAG_008_ClockRingA);
			for (uint32_t w = game_addr(game::unk_1636C10); w < game_addr(game::unk_1636C32); w += 2)
				for (int k = 4; k != 0; k--)
				{
					x::matrixMultiplyVector(M, src, dst);
					U16(dst, 2) = (uint16_t)(U16(dst, 2) + MEM<uint16_t>(w));
					src += 8;
					dst += 8;
				}
		}
		{
			x::MAG_022_sub_8DD770(M);
			x::sub_8DD960(M, 0x400);
			uint32_t src = game_addr(game::MAG_008_ClockRingTemplate), dst = game_addr(game::MAG_008_ClockRingB);
			for (uint32_t w = game_addr(game::unk_1636C10); w < game_addr(game::unk_1636C32); w += 2)
				for (int k = 4; k != 0; k--)
				{
					x::matrixMultiplyVector(M, src, dst);
					const int32_t sc = mul32(S32(MEM<uint32_t>(game_addr(CLOCK_BLOCK)), 0x28), (int32_t)MEM<int16_t>(w));
					src += 8;
					dst += 8;
					U16(dst, -6) = (uint16_t)(U16(dst, -6) + (uint16_t)(sc / 4096));
				}
		}
		return 0;
	}

	uint32_t __cdecl dm_8C8350(uint32_t a1, uint32_t a2)
	{
		alignas(4) uint8_t loc[0x28] = {};
		const uint32_t L = P(loc);
		const uint32_t b = MEM<uint32_t>(game_addr(CLOCK_BLOCK));
		U16(L, 0) = 0;
		const int32_t y = mul32((int32_t)MEM<int16_t>(game_addr(game::unk_1636C10) + (uint32_t)((int32_t)(int16_t)a2 * 2)), S32(b, 0x28)) / 4096;
		U16(L, 2) = (uint16_t)y;
		U16(L, 4) = (uint16_t)(a2 * 0xF0);
		x::MAG_022_sub_8DD770(L + 8);
		ff8fx::copy<(8 * 4)>((void *)(L + 8), (const void *)MEM<uint32_t>(game_addr(CLOCK_BLOCK)));
		x::scale3DMatrix(L + 8, MEM<uint32_t>(game_addr(CLOCK_BLOCK)) + 0x20);
		x::matrixMultiplyVector(L + 8, L, a1);
		const uint32_t b2 = MEM<uint32_t>(game_addr(CLOCK_BLOCK));
		U16(a1, 0) = (uint16_t)(U16(a1, 0) + U16(b2, 0x30));
		U16(a1, 2) = (uint16_t)(U16(a1, 2) + U16(b2, 0x32));
		U16(a1, 4) = (uint16_t)(U16(a1, 4) + U16(b2, 0x34));
		return 0;
	}

	uint32_t __cdecl dm_8C8400(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x456) = (uint16_t)(U16(node, 0x456) + 1);
		const uint16_t step = MEM<uint16_t>(game_addr(game::Doom_ClockHandSteps) + (uint32_t)((int32_t)S16(node, 0x456) * 2));
		U16(node, 0x454) = step;
		if ((int16_t)step >= 0x7F)
		{
			U32(node, 0x1C) = U32(node, 0x44C);
			const uint32_t layout = U32(MEM<uint32_t>(game_addr(game::MAG_008_AssetTablePtr)), 0x12C);
			U16(node, 0x454) = 0x11;
			U32(node, 0x20) = U32(node, 0x450);
			const uint32_t t = dm_8C5860(node, game_addr(game::MAG_008_Strike_Task), layout, 0x3B0, 2, 0);
			const uint32_t b = MEM<uint32_t>(game_addr(CLOCK_BLOCK));
			U32(t, 0x1C) = U32(b, 0x38);
			U32(t, 0x20) = U32(b, 0x3C);
			next_state(node);
		}
		dm_8C8350(node + 0x1C, (uint32_t)U16(node, 0x454));
		a_73C280(node);
		return dm_8C84A0();
	}

	uint32_t __cdecl dm_8C84A0(void)
	{
		uint32_t b = MEM<uint32_t>(game_addr(CLOCK_BLOCK));
		const uint16_t first = MEM<uint16_t>(game_addr(game::Doom_ClockRingSteps) + (uint32_t)((int32_t)S16(b, 0x54) * 4));
		if (first == 0x7F)
			return 0;
		U16(b, 0x50) = first;
		U16(b, 0x52) = MEM<uint16_t>(game_addr(game::unk_1636C42) + (uint32_t)((int32_t)S16(b, 0x54) * 4));
		dm_8C8500(game_addr(game::MAG_008_ClockRingA));
		dm_8C8500(game_addr(game::MAG_008_ClockRingB));
		b = MEM<uint32_t>(game_addr(CLOCK_BLOCK));
		U16(b, 0x54) = (uint16_t)(U16(b, 0x54) + 1);
		return 1;
	}

	uint32_t __cdecl dm_8C8500(uint32_t a1)
	{
		alignas(4) uint8_t loc[0x30] = {};
		const uint32_t L = P(loc);
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase));
		uint32_t b = MEM<uint32_t>(game_addr(CLOCK_BLOCK));
		const int32_t first = S16(b, 0x50);
		const int32_t last1 = S16(b, 0x52) + 1;
		uint32_t pkt = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		uint32_t col = (uint32_t)(first << 4) + game_addr(game::g_Doom_RingRowColours);
		U8(L, 0) = 0;
		U8(L, 1) = 0;
		U8(L, 2) = 0;
		U8(L, 3) = 0;
		if (first < last1)
		{
			for (int32_t n = last1 - first; n != 0; n--)
			{
				const uint32_t c = U32(L, 0);
				for (int32_t j = 0; j < 4; j++, col += 4)
				{
					if (j == 0 || j == 3)
					{
						U8(col, 0) = 0;
						U8(col, 1) = 0;
						U8(col, 2) = 0;
					}
					else
						U32(col, 0) = c;
				}
				U8(L, 0) = (uint8_t)(U8(L, 0) + 6);
				U8(L, 2) = (uint8_t)(U8(L, 2) + 2);
			}
		}
		const uint32_t M = L + 0x10;
		ff8fx::copy<(8 * 4)>((void *)M, (const void *)b);
		x::scale3DMatrix(M, b + 0x20);
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(M);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(M);
		x::GTE_LoadIRFromMatrixColumn(M + 2);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(M + 2);
		x::GTE_LoadIRFromMatrixColumn(M + 4);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(M + 4);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(M + 0x14);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(M + 0x14);
		x::GTE_SetRotMatrix(M);
		x::GTE_SetTransVector(M);
		b = MEM<uint32_t>(game_addr(CLOCK_BLOCK));
		int32_t row = S16(b, 0x50);
		uint32_t crow = (uint32_t)(row << 4) + game_addr(game::g_Doom_RingRowColours);
		uint32_t vrow = a1 + (uint32_t)(row << 5);
		if (row < S16(b, 0x52))
		{
			do
			{
				uint32_t v = vrow;
				uint32_t sxy = pkt + 0x20;
				uint32_t vn = vrow + 8;
				for (int32_t k = 3; k != 0; k--)
				{
					U32(pkt, 0) = 0x8000000;
					U8(sxy, -0x19) = 0x3A;
					x::GTE_LoadV012(v, vn, vn + 0x18);
					x::GTE_RTPT();
					GTE_StoreSXY012_PolyG4(pkt);
					x::GTE_LoadV0(vn + 0x20);
					x::GTE_RTPS();
					x::GTE_ReadSXY2(sxy);
					U8(sxy, -0x1C) = U8(crow, 0);
					U8(sxy, -0x1B) = U8(crow, 1);
					U8(sxy, -0x1A) = U8(crow, 2);
					U8(sxy, -0x14) = U8(crow, 4);
					U8(sxy, -0x13) = U8(crow, 5);
					U8(sxy, -0x12) = U8(crow, 6);
					U8(sxy, -0xC) = U8(crow, 0x10);
					U8(sxy, -0xB) = U8(crow, 0x11);
					U8(sxy, -0xA) = U8(crow, 0x12);
					U8(sxy, -4) = U8(crow, 0x14);
					U8(sxy, -3) = U8(crow, 0x15);
					U8(sxy, -2) = U8(crow, 0x16);
					x::GTE_AVSZ4();
					x::GTE_ReadOTZ(L + 4);
					S32(L, 4) = S32(L, 4) >> 2;
					x::SSIGPU_InsertPrimAutoDepth(ot + (uint32_t)S32(L, 4) * 4, pkt);
					pkt += 0x24;
					sxy += 0x24;
					v += 8;
					vn += 8;
					crow += 4;
				}
				crow += 4;
				vrow = v + 8;
				row++;
			} while (row < S16(MEM<uint32_t>(game_addr(CLOCK_BLOCK)), 0x52));
		}
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = pkt;
		return 0;
	}

	uint32_t __cdecl dm_8C87F0(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		U16(node, 0x88) = 0xFE00;
		a_73C280(node);
		next_state(node);
		return 0;
	}

	uint32_t __cdecl dm_8C8870(uint32_t a1)
	{
		const uint32_t r = a_73C280(a1);
		if (r == 0)
			next_state(a1);
		return dm_8C84A0();
	}

	uint32_t __cdecl dm_8C8890(uint32_t a1)
	{
		const uint32_t r = dm_8C84A0();
		if (r == 0)
		{
			U8(a1, 0x26) |= 1;
			next_state(a1);
		}
		return r;
	}

	uint32_t __cdecl dm_8CC330(uint32_t a1)
	{
		if (S16(a1, 0x24) > 0x2D)
		{
			U16(a1, 0x1C) = 0x600;
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl dm_8CC3A0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t states[11] = { game_addr(game::MAG_008_Reaper_Appear), game_addr(game::MAG_008_Reaper_Rise), game_addr(game::MAG_008_Reaper_Turn), game_addr(game::MAG_008_Reaper_State3_Mark4ResumePlayers), game_addr(game::MAG_008_Reaper_State4_Phase4PlayAnim1), game_addr(game::MAG_008_Reaper_State5_WaitModelAnimDone),
			game_addr(game::MAG_008_Reaper_State6_AnimUntilLoopThenAnim3), game_addr(game::MAG_008_Reaper_State7_AnimEndPlayAnim4), game_addr(game::MAG_008_Reaper_State8_CountdownTimer), game_addr(game::MAG_008_Reaper_FadeOut), game_addr(game::MAG_008_Reaper_State10_Empty) };
		callp(states[S8(node, 0x29)], node);
		if ((U8(node, 0x26) & 4) == 0)
		{
			dm_8CC450(node);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl dm_8CC7B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		(void)a3;
		const uint32_t ctx = a4;
		const uint32_t blk = U32(ctx, 0x48);
		uint32_t cursor = MEM<uint32_t>(U32(ctx, 0x4C));
		const uint32_t vbuf = U32(U32(ctx, 0x44), 4);
		uint32_t table = U32(a1, 4);
		const uint32_t bones = U32(a1, 0) + 0x10;
		const int32_t count = S32(table, 0);
		const uint32_t hdr = x::Field_Alloc(0x100);
		table += 4;
		const uint32_t colour = U32(blk, 0x28) & 0xFFFFFF;
		U32(hdr, 0x50) = U32(blk, 0x7C);
		U32(hdr, 0x48) = colour | 0x24000000;
		U32(hdr, 0x4C) = colour | 0x2C000000;
		U16(hdr, 0xF4) = U16(ctx, 0x54);
		U16(hdr, 0xFE) = 0;
		uint32_t face = ctx + 0x5C;
		uint8_t grey = 0;
		for (int32_t k = 0; k < 0x14; k++)
		{
			const uint32_t g = (uint32_t)grey;
			const uint32_t rgb = ((g << 8) + g) << 8 | 0;
			const uint32_t w = rgb + g;
			MEM<uint32_t>(game_addr(game::MAG_008_GreyTriCodes) + k * 4) = w;
			MEM<uint32_t>(game_addr(game::MAG_008_GreyQuadCodes) + k * 4) = w;
			if (k == 0x13)
			{
				MEM<uint32_t>(game_addr(game::unk_275F9D0)) |= 0x24000000;
				MEM<uint32_t>(game_addr(game::unk_275F97C)) = w | 0x2C000000;
			}
			else
			{
				MEM<uint32_t>(game_addr(game::MAG_008_GreyTriCodes) + k * 4) = w | 0x26000000;
				MEM<uint32_t>(game_addr(game::MAG_008_GreyQuadCodes) + k * 4) = w | 0x2E000000;
			}
			grey = (uint8_t)(grey + 8);
			if (grey > 0x80)
				grey = 0x80;
		}
		uint32_t ang = U16(ctx, 0x58);
		for (int32_t i = 0; i < 0x14; i++)
		{
			const uint32_t o = hdr + 0x74 + (uint32_t)i * 2;
			if (i < 0x10)
			{
				const int32_t k = 0xF - i;
				const int32_t radius = (int16_t)(k * 0x30);
				const int32_t s = mul32((int32_t)x::computeSin((uint32_t)(int32_t)(int16_t)ang), radius);
				U16(o, 0) = (uint16_t)(s / 4096);
				U16(o, 0x28) = (uint16_t)(k << 6);
				if (S16(o, 0x28) > 0)
					U16(o, 0x28) = 0;
				const int32_t c = mul32((int32_t)x::computeCosine((uint32_t)(int32_t)(int16_t)ang), radius);
				U16(o, 0x50) = (uint16_t)(c / 4096);
				ang = (uint32_t)(uint16_t)(U16(ctx, 0x5A) + (uint16_t)ang) & 0xFFF;
			}
			else
			{
				U16(o, 0) = 0;
				U16(o, 0x28) = 0;
				U16(o, 0x50) = 0;
			}
		}
		x::GTE_SetRotMatrix(ctx + 0x20);
		x::GTE_SetTransVector(ctx + 0x20);
		for (int32_t i = 0; i < count; i++)
		{
			uint32_t v = vbuf;
			const uint32_t off = U32(table, 0);
			table += 4;
			uint32_t obj = U32(a1, 4) + off;
			if (!((U32(hdr, 0x50) >> (i & 31)) & 1))
				continue;
			int32_t groups = S16(obj, 0);
			obj += 2;
			for (; groups > 0; groups--)
			{
				const int32_t bone = S16(obj, 0);
				obj += 2;
				const uint32_t m = bones + (uint32_t)(bone * 0x30) + 0x10;
				GTE_SetLightMatrix(m);
				GTE_SetBackgroundVector(U32(m, 0x14), U32(m, 0x18), U32(m, 0x1C));
				int32_t nv = S16(obj, 0);
				obj += 2;
				for (; nv > 0; nv--)
				{
					U16(hdr, 0x28) = U16(obj, 0);
					U16(hdr, 0x2A) = U16(obj, 2);
					U16(hdr, 0x2C) = U16(obj, 4);
					obj += 6;
					x::GTE_LoadV0(hdr + 0x28);
					GTE_MVMVA_LightV0_Bk();
					x::GTE_StoreIR123(hdr + 0x28);
					U32(v, 0) = U32(hdr, 0x28);
					U32(v, 4) = U32(hdr, 0x2C);
					v += 8;
				}
			}
			obj = (obj + 3) & ~3u;
			const int32_t n3 = S16(obj, 0);
			const int32_t n4 = S16(obj, 2);
			uint32_t rec = obj + 0xC;
			uint32_t pkt = cursor;
			for (int32_t k = n3; k > 0; k--, rec += 0x10)
			{
				U16(hdr, 0xFE) = (uint16_t)(U16(hdr, 0xFE) + 1);
				face++;
				const uint32_t i0 = U16(rec, 0) & 0xFFF, i1 = U16(rec, 2) & 0xFFF, i2 = U16(rec, 4) & 0xFFF;
				U32(hdr, 0x38) = i0;
				U32(hdr, 0x54) = U32(vbuf + i0 * 8, 0);
				U32(hdr, 0x58) = U32(vbuf + i0 * 8, 4);
				U32(hdr, 0x3C) = i1;
				U32(hdr, 0x5C) = U32(vbuf + i1 * 8, 0);
				U32(hdr, 0x60) = U32(vbuf + i1 * 8, 4);
				U32(hdr, 0x64) = U32(vbuf + i2 * 8, 0);
				U32(hdr, 0x40) = i2;
				U32(hdr, 0x68) = U32(vbuf + i2 * 8, 4);
				const int32_t mean = (S16(hdr, 0x56) + S16(hdr, 0x5E) + S16(hdr, 0x66)) / 3;
				U16(hdr, 0xF6) = (uint16_t)mean;
				if ((int16_t)mean >= S16(hdr, 0xF4) && U8(face, 0) < 0x13)
					U8(face, 0) = (uint8_t)(U8(face, 0) + 1);
				const uint8_t st = U8(face, 0);
				if (st == 0)
					continue;
				if (st != 0x13)
				{
					U16(hdr, 0xF8) = U16(hdr, 0x74 + st * 2);
					U16(hdr, 0xFA) = U16(hdr, 0x9C + st * 2);
					U16(hdr, 0xFC) = U16(hdr, 0xC4 + st * 2);
					if (U16(hdr, 0xFE) & 1)
					{
						U16(hdr, 0xF8) = (uint16_t)-U16(hdr, 0xF8);
						U16(hdr, 0xFC) = (uint16_t)-U16(hdr, 0xFC);
					}
					const uint16_t ox = U16(hdr, 0xF8), oy = U16(hdr, 0xFA), oz = U16(hdr, 0xFC);
					U16(hdr, 0x54) = (uint16_t)(U16(hdr, 0x54) + ox);
					U16(hdr, 0x5C) = (uint16_t)(U16(hdr, 0x5C) + ox);
					U16(hdr, 0x64) = (uint16_t)(U16(hdr, 0x64) + ox);
					U16(hdr, 0x56) = (uint16_t)(U16(hdr, 0x56) + oy);
					U16(hdr, 0x58) = (uint16_t)(U16(hdr, 0x58) + oz);
					U16(hdr, 0x60) = (uint16_t)(U16(hdr, 0x60) + oz);
					U16(hdr, 0x68) = (uint16_t)(U16(hdr, 0x68) + oz);
					U16(hdr, 0x5E) = (uint16_t)(U16(hdr, 0x5E) + oy);
					U16(hdr, 0x66) = (uint16_t)(U16(hdr, 0x66) + oy);
				}
				GTE_LoadV012_Packed(hdr + 0x54);
				x::GTE_RTPT();
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(hdr + 0xEC);
				if (S32(hdr, 0xEC) <= 0)
					continue;
				x::GTE_AVSZ3();
				x::GTE_ReadOTZ(hdr + 0xF0);
				U32(hdr, 0xF0) = U32(hdr, 0xF0) >> 2;
				GTE_StoreSXY012_PolyFT3(pkt);
				U32(pkt, 0) = 0x7000000;
				U32(pkt, 0x14) = U32(rec, 0xC);
				U32(pkt, 0xC) = U32(rec, 8);
				U16(pkt, 0x1C) = U16(rec, 6);
				U32(pkt, 4) = MEM<uint32_t>(game_addr(game::MAG_008_GreyTriCodes) + (uint32_t)U8(face, 0) * 4);
				x::SSIGPU_InsertPrimAutoDepth(a2 + U32(hdr, 0xF0) * 4, pkt);
				pkt += 0x20;
			}
			if (n4 > 0)
			{
				rec += 4;
				for (int32_t k = n4; k > 0; k--, rec += 0x14)
				{
					U16(hdr, 0xFE) = (uint16_t)(U16(hdr, 0xFE) + 1);
					face++;
					const uint32_t i0 = U16(rec, -4) & 0xFFF, i1 = U16(rec, -2) & 0xFFF;
					const uint32_t i2 = U16(rec, 0) & 0xFFF, i3 = U16(rec, 2) & 0xFFF;
					U32(hdr, 0x40) = i2;
					U32(hdr, 0x38) = i0;
					U32(hdr, 0x44) = i3;
					U32(hdr, 0x54) = U32(vbuf + i0 * 8, 0);
					U32(hdr, 0x58) = U32(vbuf + i0 * 8, 4);
					U32(hdr, 0x3C) = i1;
					U32(hdr, 0x5C) = U32(vbuf + i1 * 8, 0);
					U32(hdr, 0x60) = U32(vbuf + i1 * 8, 4);
					U32(hdr, 0x64) = U32(vbuf + i2 * 8, 0);
					U32(hdr, 0x68) = U32(vbuf + i2 * 8, 4);
					U32(hdr, 0x6C) = U32(vbuf + i3 * 8, 0);
					U32(hdr, 0x70) = U32(vbuf + i3 * 8, 4);
					const int32_t mean = (S16(hdr, 0x6E) + S16(hdr, 0x56) + S16(hdr, 0x5E) + S16(hdr, 0x66)) / 4;
					U16(hdr, 0xF6) = (uint16_t)mean;
					if ((int16_t)mean >= S16(hdr, 0xF4) && U8(face, 0) < 0x13)
						U8(face, 0) = (uint8_t)(U8(face, 0) + 1);
					const uint8_t st = U8(face, 0);
					if (st == 0)
						continue;
					if (st != 0x13)
					{
						U16(hdr, 0xF8) = U16(hdr, 0x74 + st * 2);
						U16(hdr, 0xFA) = U16(hdr, 0x9C + st * 2);
						U16(hdr, 0xFC) = U16(hdr, 0xC4 + st * 2);
						if (U16(hdr, 0xFE) & 1)
						{
							U16(hdr, 0xF8) = (uint16_t)-U16(hdr, 0xF8);
							U16(hdr, 0xFC) = (uint16_t)-U16(hdr, 0xFC);
						}
						const uint16_t ox = U16(hdr, 0xF8), oy = U16(hdr, 0xFA), oz = U16(hdr, 0xFC);
						U16(hdr, 0x54) = (uint16_t)(U16(hdr, 0x54) + ox);
						U16(hdr, 0x5C) = (uint16_t)(U16(hdr, 0x5C) + ox);
						U16(hdr, 0x64) = (uint16_t)(U16(hdr, 0x64) + ox);
						U16(hdr, 0x6C) = (uint16_t)(U16(hdr, 0x6C) + ox);
						U16(hdr, 0x5E) = (uint16_t)(U16(hdr, 0x5E) + oy);
						U16(hdr, 0x56) = (uint16_t)(U16(hdr, 0x56) + oy);
						U16(hdr, 0x58) = (uint16_t)(U16(hdr, 0x58) + oz);
						U16(hdr, 0x60) = (uint16_t)(U16(hdr, 0x60) + oz);
						U16(hdr, 0x68) = (uint16_t)(U16(hdr, 0x68) + oz);
						U16(hdr, 0x70) = (uint16_t)(U16(hdr, 0x70) + oz);
						U16(hdr, 0x66) = (uint16_t)(U16(hdr, 0x66) + oy);
						U16(hdr, 0x6E) = (uint16_t)(U16(hdr, 0x6E) + oy);
					}
					GTE_LoadV012_Packed(hdr + 0x54);
					x::GTE_RTPT();
					x::GTE_NCLIP();
					x::GTE_ReadMAC0(hdr + 0xEC);
					if (S32(hdr, 0xEC) <= 0)
						continue;
					GTE_StoreSXY012_PolyFT3_2(pkt);
					x::GTE_LoadV0(hdr + 0x6C);
					x::GTE_RTPS();
					x::GTE_ReadSXY2(pkt + 0x20);
					x::GTE_AVSZ4();
					x::GTE_ReadOTZ(hdr + 0xF0);
					const uint32_t z = U32(hdr, 0xF0) >> 2;
					U32(hdr, 0xF0) = z;
					U32(pkt, 0xC) = U32(rec, 4);
					U32(pkt, 0x14) = U32(rec, 8);
					U16(pkt, 0x1C) = U16(rec, 0xC);
					U16(pkt, 0x24) = U16(rec, 0xE);
					U32(pkt, 0) = 0x9000000;
					U32(pkt, 4) = MEM<uint32_t>(game_addr(game::MAG_008_GreyQuadCodes) + (uint32_t)U8(face, 0) * 4);
					x::SSIGPU_InsertPrimAutoDepth(a2 + z * 4, pkt);
					pkt += 0x28;
				}
			}
			cursor = pkt;
		}
		const uint32_t out = U32(ctx, 0x4C);
		MEM<uint32_t>(out) = cursor;
		return x::Field_Free(0x100);
	}

	uint32_t __cdecl dm_8CCF80(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t d = MEM<uint32_t>(game_addr(PHASE_DATA));
		U32(node, 0x4C) = U32(d, 0);
		const uint32_t w4 = U32(d, 4);
		U16(node, 0x3E) = U16(d, 0x4A);
		U32(node, 0x60) = node + 0x114;
		U32(node, 0x50) = w4;
		U32(node, 0x11C) = 0x1000;
		U32(node, 0x118) = 0x1000;
		U32(node, 0x114) = 0x1000;
		x::au_re_Battle_ReadAnimation_7(node, 0);
		dm_8CCFE0(node);
		next_state(node);
		return 0;
	}

	uint32_t __cdecl dm_8CCFE0(uint32_t a1)
	{
		dm_8CD020(game_addr(REAPER_CTX), game_addr(game::MAG_008_ReaperVertexBuffer), game_addr(game::MAG_008_ReaperObjectBlock), a1 + 0x30);
		U8(a1, 0x26) |= 8;
		MEM<uint16_t>(game_addr(game::g_Doom_RiseHeight)) = 0;
		MEM<uint16_t>(game_addr(game::MAG_008_SpiralStep)) = 0x100;
		return 0;
	}

	uint32_t __cdecl dm_8CD090(uint32_t a1)
	{
		const uint16_t a = (uint16_t)(MEM<uint16_t>(game_addr(game::g_Doom_SpiralAngle)) + MEM<uint16_t>(game_addr(game::MAG_008_SpiralStep)));
		const uint16_t h = (uint16_t)(MEM<uint16_t>(game_addr(game::g_Doom_RiseHeight)) - 0x48);
		MEM<uint16_t>(game_addr(game::g_Doom_RiseHeight)) = h;
		MEM<uint16_t>(game_addr(game::g_Doom_SpiralAngle)) = (uint16_t)(a & 0xFFF);
		if ((int16_t)h <= (int16_t)0xFB00)
		{
			U16(a1, 0x134) = 0x10;
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl dm_8CD0E0(uint32_t a1)
	{
		MEM<uint16_t>(game_addr(game::g_Doom_SpiralAngle)) = (uint16_t)((MEM<uint16_t>(game_addr(game::g_Doom_SpiralAngle)) + MEM<uint16_t>(game_addr(game::MAG_008_SpiralStep))) & 0xFFF);
		U16(a1, 0x134) = (uint16_t)(U16(a1, 0x134) - 1);
		if (S16(a1, 0x134) <= 0)
		{
			U8(a1, 0x26) &= 0xF7;
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl dm_8CD120(uint32_t a1)
	{
		if (a_73B640(4) != 0)
		{
			a_73BB60(0);
			next_state(a1);
			return a1;
		}
		return a_73BB60(1);
	}

	uint32_t __cdecl dm_8CD1A0(uint32_t a1)
	{
		if (a_73B7E0(4) == 0)
			return 0;
		x::au_re_Battle_ReadAnimation_7(a1, 1);
		next_state(a1);
		return 0;
	}

	uint32_t __cdecl dm_8CD200(uint32_t a1)
	{
		alignas(4) uint8_t loc[8] = {};
		x::GetEffectSpawnPosition(a1 + 0x30, 0x1B, 0, P(loc));
		x::sub_8DD1C0(a1);
		if (S16(a1, 0x138) >= 1)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 3);
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl dm_8CD510(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(PHASE_DATA));
		U32(a1, 0x1C) = U32(d, 0);
		U32(a1, 0x20) = U32(d, 4);
		U16(a1, 0x1E) = 0;
		next_state(a1);
		return 0;
	}
	uint32_t __cdecl dm_8CDF30(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(PHASE_DATA));
		U32(a1, 0x1C) = U32(d, 0);
		U32(a1, 0x20) = U32(d, 4);
		U16(a1, 0x1E) = 0;
		next_state(a1);
		return 0;
	}

	uint32_t __cdecl dm_8CD540(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint16_t h = (uint16_t)(MEM<uint32_t>(game_addr(game::g_Doom_RiseHeight)) + 0x100);
		U16(node, 0x1E) = h;
		if ((int16_t)h >= 0)
			U16(node, 0x1E) = 0;
		if ((U8(node, 0x24) & 1) == 0)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_008_DustWobbleQueue), game_addr(game::MAG_008_DustSparkle_Task), 0xB0, node);
			uint32_t p = t + 0x6C;
			for (int32_t n = 4; n != 0; n--)
			{
				const int32_t r1 = (int32_t)x::CrtRand();
				const int32_t radius = (int16_t)((r1 & 0xFF) + 0x100);
				const int32_t r2 = (int32_t)x::CrtRand();
				U32(p, 0) = U32(node, 0x1C);
				const int32_t angle = (int16_t)(r2 & 0xFFF);
				U32(p, 4) = U32(node, 0x20);
				const int32_t s = mul32((int32_t)x::computeSin((uint32_t)angle), radius);
				U16(p, 0) = (uint16_t)(U16(p, 0) + (uint16_t)(s / 4096));
				const int32_t r3 = (int32_t)x::CrtRand();
				U16(p, 2) = (uint16_t)(U16(p, 2) + (uint16_t)((r3 & 0xFF) - 0x80));
				const int32_t c = mul32((int32_t)x::computeCosine((uint32_t)angle), radius);
				U16(p, 4) = (uint16_t)(U16(p, 4) + (uint16_t)(c / 4096));
				const int32_t r4 = (int32_t)x::CrtRand();
				U16(p, 0x20) = (uint16_t)((r4 & 0x3FF) - 0x200);
				const int32_t r5 = (int32_t)x::CrtRand();
				U16(p, 0x22) = (uint16_t)((r5 & 0x3FF) - 0x200);
				const int32_t r6 = (int32_t)x::CrtRand();
				p += 8;
				U16(p, 0x1C) = (uint16_t)((r6 & 0x3FF) - 0x200);
			}
		}
		if (S16(node, 0x24) >= 0x28)
		{
			U8(node, 0x26) |= 1;
			next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl dm_8CDF60(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x1E) = (uint16_t)(MEM<uint32_t>(game_addr(game::g_Doom_RiseHeight)) + 0x100);
		if ((U8(node, 0x24) & 1) == 0)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_008_SmokeQueue), game_addr(game::MAG_008_SmokePuff_Task), 0x58, node);
			U32(t, 0x1C) = U32(node, 0x1C);
			U32(t, 0x20) = U32(node, 0x20);
		}
		if (S16(node, 0x24) >= 0x23)
		{
			U8(node, 0x26) |= 1;
			next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl dm_8CDFB0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t states[5] = { game_addr(game::MAG_008_SmokePuff_State0_Init), game_addr(game::MAG_008_SmokePuff_State1_FadeIn), game_addr(game::MAG_008_SmokePuff_State2_Wait), game_addr(game::MAG_008_SmokePuff_State3_FadeOut), game_addr(game::MAG_008_SmokePuff_State4_Empty) };
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x46) = (uint16_t)((U16(node, 0x46) + 0x80) & 0xFFF);
		dm_8CE040(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl dm_8CE130(uint32_t a1)
	{
		const uint32_t node = a1;
		U32(node, 0x4C) = game_addr(game::unk_16350F0);
		const int32_t r1 = (int32_t)x::CrtRand();
		U16(node, 0x50) = 0x1000;
		const int32_t sc = r1 % 0x3FF + 0xC00;
		U32(node, 0x38) = (uint32_t)sc;
		U32(node, 0x34) = (uint32_t)sc;
		U32(node, 0x30) = (uint32_t)sc;
		const int32_t r2 = (int32_t)x::CrtRand();
		U16(node, 0x46) = (uint16_t)(r2 & 0xFFF);
		const int32_t r3 = (int32_t)x::CrtRand();
		U16(node, 0x44) = (uint16_t)(r3 % 0x300 - 0x180);
		const int32_t r4 = (int32_t)x::CrtRand();
		U16(node, 0x48) = (uint16_t)(r4 % 0x300 - 0x180);
		next_state(node);
		return 0;
	}

	uint32_t __cdecl dm_8CE1B0(uint32_t a1)
	{
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0xFC00);
		if (S16(a1, 0x50) <= 0)
		{
			U16(a1, 0x50) = 0;
			U16(a1, 0x54) = 1;
			next_state(a1);
		}
		return 0;
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_008_Master_State0_Advance, (void *)a_73A0D0, "008 MAG_008_sub_8C4CC0" },
		{ game::MAG_008_Master_State1_Advance, (void *)a_73A0D0, "008 MAG_008_sub_8C4CD0" },
		{ game::MAG_008_Master_State3_SpawnEmitter, (void *)a_73A170, "008 MAG_008_sub_8C4D60" },
		{ game::MAG_008_EmitterTask, (void *)a_73A1A0, "008 MAG_008_sub_8C4D90" },
		{ game::MAG_008_FileLoadScriptTask, (void *)a_73A380, "008 MAG_008_sub_8C4F60" },
		{ game::MAG_008_LoadScript_Issue, (void *)a_73A3E0, "008 sub_8C4FC0" },
		{ game::MAG_008_LoadCb_UploadTim, (void *)a_73A580, "008 sub_8C5160" },
		{ game::MAG_008_LoadCb_SummonStream, (void *)a_73A5B0, "008 sub_8C5190" },
		{ game::MAG_008_LoadCb_ClearBusy, (void *)a_73A5E0, "008 sub_8C51C0" },
		{ game::MAG_008_LoadCb_ClearBusy_2, (void *)a_73A5E0, "008 sub_8C51D0" },
		{ game::MAG_008_LoadCb_ClearBusy_3, (void *)a_73A5E0, "008 sub_8C51E0" },
		{ game::MAG_008_LoadCb_ClearBusy_4, (void *)a_73A5E0, "008 sub_8C51F0" },
		{ game::MAG_008_LoadScript_RecordFile, (void *)a_73A620, "008 sub_8C5200" },
		{ game::MAG_008_FileLoadScript_State1_LoadScriptWait, (void *)a_73A660, "008 sub_8C5240" },
		{ game::MAG_008_FileLoadScript_State2_Empty, (void *)a_73A6D0, "008 nullsub_1834" },
		{ game::MAG_008_Director_State1_WaitDirGate1, (void *)a_73B790, "008 sub_8C5490" },
		{ game::MAG_008_DirSyncGate, (void *)a_73B640, "008 sub_8C54B0" },
		{ game::MAG_008_DirPhaseGE, (void *)a_73B7E0, "008 sub_8C5510" },
		{ game::MAG_008_Director_State3_ArmLoadScriptStep, (void *)a_73A930, "008 sub_8C5530" },
		{ game::MAG_008_LoadScript_TryArmStep, (void *)a_73A950, "008 sub_8C5550" },
		{ game::MAG_008_Director_State4_WaitDirGate2, (void *)a_73B8A0, "008 sub_8C5590" },
		{ game::MAG_008_Director_State5_WaitLoadScriptIdle, (void *)a_733AC0, "008 sub_8C55B0" },
		{ game::MAG_008_Director_State6_WaitDirGate3, (void *)a_733B70, "008 sub_8C55D0" },
		{ game::MAG_008_Director_State7_WaitDirPhase4, (void *)a_733760, "008 sub_8C55F0" },
		{ game::MAG_008_Director_State9_WaitDirGate5, (void *)a_73B620, "008 sub_8C5630" },
		{ game::MAG_008_Director_State11_CountdownTimer, (void *)a_73BC40, "008 sub_8C5680" },
		{ game::MAG_008_Director_State13_Finish, (void *)a_7475A0, "008 sub_8C56D0" },
		{ game::MAG_008_Director_State14_Empty, (void *)a_73A6D0, "008 nullsub_1835" },
		{ game::MAG_008_St_Empty, (void *)a_73A6D0, "008 nullsub_1836" },
		{ game::MAG_008_SpawnChildTask, (void *)a_73C100, "008 sub_8C5810" },
		{ game::MAG_008_Scythe_Task, (void *)a_73A380, "008 sub_8C58B0" },
		{ game::MAG_008_DrawPrimModel, (void *)a_73C280, "008 sub_8C59E0" },
		{ game::MAG_008_EmitG3, (void *)a_73D770, "008 sub_8C6E90" },
		{ game::MAG_008_EmitG4, (void *)a_73D9C0, "008 sub_8C70E0" },
		{ game::MAG_008_DoomClock_Task, (void *)a_73F110, "008 sub_8C8050" },
		{ game::MAG_008_Strike_Task, (void *)a_73A380, "008 sub_8C8790" },
		{ game::MAG_008_Strike_State2_Empty, (void *)a_73A6D0, "008 nullsub_1837" },
		{ game::MAG_008_DoomClock_State4_Empty, (void *)a_73A6D0, "008 nullsub_1838" },
		{ game::MAG_008_Scythe_State2_Empty, (void *)a_73A6D0, "008 nullsub_1839" },
		{ game::MAG_008_Aura_Task, (void *)a_73A380, "008 sub_8C88D0" },
		{ game::MAG_008_DirectorSetup, (void *)a_73F990, "008 sub_8C8960" },
		{ game::MAG_008_Director_StartPositions, (void *)a_73FC20, "008 sub_8C8BF0" },
		{ game::MAG_008_Director_Mode3Setup, (void *)a_73FE90, "008 sub_8C8E60" },
		{ game::MAG_008_Director_TargetPositions, (void *)a_73FFB0, "008 sub_8C8F80" },
		{ game::MAG_008_RelocateActorData, (void *)a_740210, "008 sub_8C91E0" },
		{ game::MAG_008_ActorMotion, (void *)a_740910, "008 sub_8C98A0" },
		{ game::MAG_008_DampValue, (void *)a_740F70, "008 sub_8C9F00" },
		{ game::MAG_008_DampVector, (void *)a_740FC0, "008 sub_8C9F50" },
		{ game::MAG_008_Instance_AdvanceTexFrame, (void *)a_741050, "008 sub_8C9FE0" },
		{ game::MAG_008_Instance_BuildTransform, (void *)a_741120, "008 sub_8CA0B0" },
		{ game::MAG_008_Particle_LifeCountdown, (void *)a_741B60, "008 sub_8CAAF0" },
		{ game::MAG_008_RandRange, (void *)a_742290, "008 au_re__rand_69" },
		{ game::MAG_008_RandRangeWide, (void *)a_7422C0, "008 sub_8CB250" },
		{ game::MAG_008_RandVec3, (void *)a_742300, "008 sub_8CB290" },
		{ game::MAG_008_JitterAngles3, (void *)a_742350, "008 sub_8CB2E0" },
		{ game::MAG_008_RandAngleJitter, (void *)a_7423A0, "008 au_re__rand_69_0" },
		{ game::MAG_008_GetWord8Minus1, (void *)a_7424B0, "008 sub_8CB440" },
		{ game::MAG_008_Particle_AttachToOwner, (void *)a_7424C0, "008 sub_8CB450" },
		{ game::MAG_008_ParticleList_Append, (void *)a_742CA0, "008 sub_8CBC30" },
		{ game::MAG_008_ParticleList_Unlink, (void *)a_742CD0, "008 sub_8CBC60" },
		{ game::MAG_008_Particle_Init, (void *)a_742D00, "008 sub_8CBC90" },
		{ game::MAG_008_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "008 sub_8CBE40" },
		{ game::MAG_008_Particle_PlaceAtPoint, (void *)a_742FF0, "008 sub_8CBF80" },
		{ game::MAG_008_Aura_State2_Empty, (void *)a_73A6D0, "008 nullsub_1840" },
		{ game::MAG_008_StageWobbleInTask, (void *)a_73A380, "008 sub_8CC1E0" },
		{ game::MAG_008_StageWobbleIn_State0_ResetEntityRecords, (void *)a_7335D0, "008 sub_8CC240" },
		{ game::MAG_008_StageWobbleIn_State2_Empty, (void *)a_73A6D0, "008 nullsub_1841" },
		{ game::MAG_008_StageWobbleOutTask, (void *)a_73A380, "008 sub_8CC2D0" },
		{ game::MAG_008_StageWobbleOut_State2_Empty, (void *)a_73A6D0, "008 nullsub_1842" },
		{ game::MAG_008_DrawBattleModel, (void *)a_746C10, "008 sub_8CC4D0" },
		{ game::MAG_008_SetPauseSwitch, (void *)a_73BB60, "008 sub_8CD150" },
		{ game::MAG_008_SetEntityColourTint, (void *)a_733950, "008 sub_8CD330" },
		{ game::MAG_008_Reaper_State10_Empty, (void *)a_73A6D0, "008 nullsub_1843" },
		{ game::MAG_008_DustTask, (void *)a_73A380, "008 sub_8CD4B0" },
		{ game::InitEffectSequenceFromData_c18, (void *)a_7435E0, "008 InitEffectSequenceFromData_c18" },
		{ game::MAG_008_SpriteStrip_DrawQuads, (void *)a_743720, "008 sub_8CD9D0" },
		{ game::MAG_008_StepCounterLimit, (void *)a_743C20, "008 sub_8CDE80" },
		{ game::MAG_008_DustSparkle_State2_Empty, (void *)a_73A6D0, "008 nullsub_1844" },
		{ game::MAG_008_Dust_State2_Empty, (void *)a_73A6D0, "008 nullsub_1845" },
		{ game::MAG_008_SmokeTask, (void *)a_73A380, "008 sub_8CDED0" },
		{ game::MAG_008_SmokePuff_State4_Empty, (void *)a_73A6D0, "008 nullsub_1846" },
		{ game::MAG_008_Smoke_State2_Empty, (void *)a_73A6D0, "008 nullsub_1847" },
		{ game::MAG_008_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "008 MAG_008_sub_8CE290" },
		{ game::MAG_008_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "008 MAG_008_sub_8CE2B0" },
		{ game::MAG_008_Emitter_State4_Empty, (void *)a_73A6D0, "008 nullsub_1848" },
		{ game::MAG_008_Master_State4_NextTargetLoop, (void *)a_747500, "008 MAG_008_sub_8CE2E0" },
		{ game::MAG_008_Master_State5_Advance, (void *)a_73A0D0, "008 MAG_008_sub_8CE320" },
		{ game::MAG_008_Master_State6_WaitQueuesEmpty, (void *)a_747550, "008 MAG_008_sub_8CE330" },
		{ game::MAG_008_Master_State7_RequestBattleSync, (void *)a_747560, "008 MAG_008_sub_8CE340" },
		{ game::MAG_008_Master_State8_WaitBattleSyncDone, (void *)a_747590, "008 MAG_008_sub_8CE370" },
		{ game::MAG_008_Master_State9_Finish, (void *)a_7475A0, "008 MAG_008_sub_8CE380" },
		{ game::nullsub_1833, (void *)a_73A6D0, "008 nullsub_1833" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_008_Master_Task, (void *)dm_8C4B30, "008 dm_8C4B30" },
		{ game::MAG_008_Master_CarveActorPools, (void *)dm_8C4CE0, "008 dm_8C4CE0" },
		{ game::MAG_008_Master_ClearActorPools, (void *)dm_8C4D20, "008 dm_8C4D20" },
		{ game::MAG_008_Master_InitDirector, (void *)dm_8C4E10, "008 dm_8C4E10" },
		{ game::MAG_008_InitAssetTable, (void *)dm_8C4E60, "008 dm_8C4E60" },
		{ game::MAG_008_InitPhaseData, (void *)dm_8C52C0, "008 dm_8C52C0" },
		{ game::MAG_008_Director_Task, (void *)dm_8C5350, "008 dm_8C5350" },
		{ game::MAG_008_Director_PhaseBookkeeping, (void *)dm_8C5410, "008 dm_8C5410" },
		{ game::MAG_008_UploadReaperTIMs, (void *)dm_8C5460, "008 dm_8C5460" },
		{ game::MAG_008_Director_State2_PlaySoundAtCut1, (void *)dm_8C54E0, "008 dm_8C54E0" },
		{ game::MAG_008_Director_State8_WaitCutFrame2D, (void *)dm_8C5610, "008 dm_8C5610" },
		{ game::MAG_008_Director_State10_Mark5SetTimer, (void *)dm_8C5650, "008 dm_8C5650" },
		{ game::MAG_008_Director_State12_Mark6SetPhaseFlag, (void *)dm_8C56A0, "008 dm_8C56A0" },
		{ game::MAG_008_SpawnReaperCreature, (void *)dm_8C5710, "008 dm_8C5710" },
		{ game::MAG_008_SpawnPrimModelTask, (void *)dm_8C5860, "008 dm_8C5860" },
		{ game::MAG_008_Scythe_Init, (void *)dm_8C5910, "008 dm_8C5910" },
		{ game::MAG_008_PrimPlayer_DrawCallback, (void *)dm_8C5B10, "008 dm_8C5B10" },
		{ game::MAG_008_RenderPrimModel, (void *)dm_8C5DD0, "008 dm_8C5DD0" },
		{ game::MAG_008_PrimList_FlatTri, (void *)dm_8C5F00, "008 dm_8C5F00" },
		{ game::MAG_008_PrimList_FlatQuad, (void *)dm_8C6120, "008 dm_8C6120" },
		{ game::MAG_008_PrimList_TexTri, (void *)dm_8C63B0, "008 dm_8C63B0" },
		{ game::MAG_008_PrimList_TexQuad, (void *)dm_8C68A0, "008 dm_8C68A0" },
		{ game::MAG_008_PrimList_GT3Scroll, (void *)dm_8C73C0, "008 dm_8C73C0" },
		{ game::MAG_008_PrimList_GT4Scroll, (void *)dm_8C7920, "008 dm_8C7920" },
		{ game::MAG_008_Scythe_SpawnClock, (void *)dm_8C7FF0, "008 dm_8C7FF0" },
		{ game::MAG_008_DoomClock_Init, (void *)dm_8C80C0, "008 dm_8C80C0" },
		{ game::MAG_008_DoomClock_BuildRings, (void *)dm_8C8120, "008 dm_8C8120" },
		{ game::MAG_008_DoomClock_HandPosition, (void *)dm_8C8350, "008 dm_8C8350" },
		{ game::MAG_008_DoomClock_StepHand, (void *)dm_8C8400, "008 dm_8C8400" },
		{ game::MAG_008_DoomClock_DrawRingStep, (void *)dm_8C84A0, "008 dm_8C84A0" },
		{ game::MAG_008_DoomClock_DrawRing, (void *)dm_8C8500, "008 dm_8C8500" },
		{ game::MAG_008_Strike_State0_Init, (void *)dm_8C87F0, "008 dm_8C87F0" },
		{ game::MAG_008_Strike_State1_PlayModel, (void *)dm_8C8840, "008 dm_8C8840" },
		{ game::MAG_008_DoomClock_State2_ModelStep, (void *)dm_8C8870, "008 dm_8C8870" },
		{ game::MAG_008_DoomClock_State3_RingSteps, (void *)dm_8C8890, "008 dm_8C8890" },
		{ game::MAG_008_Aura_State0_StartActorSystemAtFrame, (void *)dm_8C8930, "008 dm_8C8930" },
		{ game::MAG_008_ResetTrailHistory, (void *)dm_8C8B10, "008 dm_8C8B10" },
		{ game::MAG_008_FillTrailRows, (void *)dm_8C8D90, "008 dm_8C8D90" },
		{ game::MAG_008_ActSys_Step, (void *)dm_8C9440, "008 dm_8C9440" },
		{ game::MAG_008_ActSys_DrawPass, (void *)dm_8C94D0, "008 dm_8C94D0" },
		{ game::MAG_008_ActSys_DrawPrimActor, (void *)dm_8C9530, "008 dm_8C9530" },
		{ game::MAG_008_ActSys_DrawSpritePass, (void *)dm_8C9690, "008 dm_8C9690" },
		{ game::MAG_008_ActSys_DrawActorSprite, (void *)dm_8C9750, "008 dm_8C9750" },
		{ game::MAG_008_ActSys_UpdatePass, (void *)dm_8C97D0, "008 dm_8C97D0" },
		{ game::MAG_008_ActorStep, (void *)dm_8C9810, "008 dm_8C9810" },
		{ game::MAG_008_MorphVertexBlock, (void *)dm_8CA840, "008 dm_8CA840" },
		{ game::MAG_008_Instance_EndOfLifeTest, (void *)dm_8CA950, "008 dm_8CA950" },
		{ game::MAG_008_Instance_SpawnFollowUp, (void *)dm_8CA9D0, "008 dm_8CA9D0" },
		{ game::MAG_008_AllocParticle, (void *)dm_8CAA00, "008 dm_8CAA00" },
		{ game::MAG_008_ActSys_UpdateEmitter, (void *)dm_8CAAA0, "008 dm_8CAAA0" },
		{ game::MAG_008_ActSys_EmitterSpawnStep, (void *)dm_8CAB20, "008 dm_8CAB20" },
		{ game::MAG_008_ActSys_SpawnParticles, (void *)dm_8CABA0, "008 dm_8CABA0" },
		{ game::MAG_008_ActSys_AllocParticleSlot, (void *)dm_8CB380, "008 dm_8CB380" },
		{ game::MAG_008_Actor_InterpolatePosition, (void *)dm_8CB5A0, "008 dm_8CB5A0" },
		{ game::MAG_008_SpawnSubEffects, (void *)dm_8CC090, "008 dm_8CC090" },
		{ game::MAG_008_Director_SpawnGroupEffects, (void *)dm_8CC120, "008 dm_8CC120" },
		{ game::MAG_008_Aura_State1_StepActorSystem, (void *)dm_8CC1B0, "008 dm_8CC1B0" },
		{ game::MAG_008_StageWobbleIn_State1_RampUp, (void *)dm_8CC280, "008 dm_8CC280" },
		{ game::MAG_008_StageWobbleOut_State0_WaitTick45, (void *)dm_8CC330, "008 dm_8CC330" },
		{ game::MAG_008_StageWobbleOut_State1_FadeOut, (void *)dm_8CC350, "008 dm_8CC350" },
		{ game::MAG_008_Reaper_Task, (void *)dm_8CC3A0, "008 dm_8CC3A0" },
		{ game::MAG_008_Reaper_Draw, (void *)dm_8CC450, "008 dm_8CC450" },
		{ game::MAG_008_Reaper_DrawDark, (void *)dm_8CC6B0, "008 dm_8CC6B0" },
		{ game::MAG_008_Reaper_DarkRenderer, (void *)dm_8CC7B0, "008 dm_8CC7B0" },
		{ game::MAG_008_Reaper_Appear, (void *)dm_8CCF80, "008 dm_8CCF80" },
		{ game::MAG_008_Reaper_PrepareDarkDraw, (void *)dm_8CCFE0, "008 dm_8CCFE0" },
		{ game::MAG_008_Reaper_InitDarkRenderCtx, (void *)dm_8CD020, "008 dm_8CD020" },
		{ game::MAG_008_Reaper_Rise, (void *)dm_8CD090, "008 dm_8CD090" },
		{ game::MAG_008_Reaper_Turn, (void *)dm_8CD0E0, "008 dm_8CD0E0" },
		{ game::MAG_008_Reaper_State3_Mark4ResumePlayers, (void *)dm_8CD120, "008 dm_8CD120" },
		{ game::MAG_008_Reaper_State4_Phase4PlayAnim1, (void *)dm_8CD1A0, "008 dm_8CD1A0" },
		{ game::MAG_008_Reaper_State5_WaitModelAnimDone, (void *)dm_8CD1D0, "008 dm_8CD1D0" },
		{ game::MAG_008_Reaper_State6_AnimUntilLoopThenAnim3, (void *)dm_8CD200, "008 dm_8CD200" },
		{ game::MAG_008_Reaper_State7_AnimEndPlayAnim4, (void *)dm_8CD250, "008 dm_8CD250" },
		{ game::MAG_008_Reaper_State8_CountdownTimer, (void *)dm_8CD290, "008 dm_8CD290" },
		{ game::MAG_008_Reaper_FadeOut, (void *)dm_8CD2B0, "008 dm_8CD2B0" },
		{ game::MAG_008_Reaper_ApplyFadeTint, (void *)dm_8CD310, "008 dm_8CD310" },
		{ game::MAG_008_Reaper_SetAmbientColour, (void *)dm_8CD430, "008 dm_8CD430" },
		{ game::MAG_008_Dust_State0_PlaceAtPhasePos, (void *)dm_8CD510, "008 dm_8CD510" },
		{ game::MAG_008_Dust_Emit, (void *)dm_8CD540, "008 dm_8CD540" },
		{ game::MAG_008_DustSparkle_Task, (void *)dm_8CD670, "008 dm_8CD670" },
		{ game::MAG_008_DrawSpriteNode, (void *)dm_8CD770, "008 dm_8CD770" },
		{ game::MAG_008_DustSparkle_State0_InitFlipbook, (void *)dm_8CDE40, "008 dm_8CDE40" },
		{ game::MAG_008_DustSparkle_State1_Animate, (void *)dm_8CDE60, "008 dm_8CDE60" },
		{ game::MAG_008_Smoke_State0_PlaceAtPhasePos, (void *)dm_8CDF30, "008 dm_8CDF30" },
		{ game::MAG_008_Smoke_Emit, (void *)dm_8CDF60, "008 dm_8CDF60" },
		{ game::MAG_008_SmokePuff_Task, (void *)dm_8CDFB0, "008 dm_8CDFB0" },
		{ game::MAG_008_DrawNodePrimModel, (void *)dm_8CE040, "008 dm_8CE040" },
		{ game::MAG_008_SmokePuff_State0_Init, (void *)dm_8CE130, "008 dm_8CE130" },
		{ game::MAG_008_SmokePuff_State1_FadeIn, (void *)dm_8CE1B0, "008 dm_8CE1B0" },
		{ game::MAG_008_SmokePuff_State2_Wait, (void *)dm_8CE1E0, "008 dm_8CE1E0" },
		{ game::MAG_008_SmokePuff_State3_FadeOut, (void *)dm_8CE200, "008 dm_8CE200" },
		{ game::MAG_008_Emitter_State1_ApplyDamage, (void *)dm_8CE250, "008 dm_8CE250" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag008_doom()
	{
		act::register_module(8);
		for (const act::doom::ModPort *p = act::doom::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(8, p->addr, p->port, p->name);
		for (const act::doom::ModPort *p = act::doom::PORTS; p->addr; p++)
			act::register_module_port(8, p->addr, p->port, p->name);
	}
}

