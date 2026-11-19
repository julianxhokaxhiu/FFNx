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
namespace curse
{
	static const Mod MOD_047 = { "curse", 47, game::MAG_047_CURSE_FL, game::MAG_046_SIGH_FL,
		{ {}, {}, game::unk_15B8028, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_047_FilePtr, {}, {}, {}, game::unk_264B4D8, {}, {}, game::unk_264D82C, {}, {}, {}, {}, game::MAG_047_ActorSystemQueue, {}, game::MAG_047_EmitterQueue, {}, {}, game::unk_264E6A0, game::MAG_047_AssetTablePtr, {}, game::unk_264E704, {}, game::unk_264E778, game::unk_264EAC0, {}, game::MAG_047_ScratchSP, game::unk_264EAE4, game::MAG_047_ActorStatePtr },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_047_EmitterTask, game::MAG_047_Emitter_StartDirector, {}, game::MAG_047_LoadCb_UploadTim, game::MAG_047_LoadCb_SummonStream, game::FFModuleHandlerInitSystem, game::FFModuleHandlerExitSystem, game::MAG_047_LoadCb_ClearBusy, game::MAG_047_LoadCb_ClearBusy_2, game::MAG_047_LoadScript_RecordFile, {}, {}, game::MAG_047_LoadScript_TryArmStep, {}, {}, {}, {}, {}, {}, {}, game::MAG_047_DirSyncGate, {}, game::MAG_047_DirPhaseGE, game::MAG_047_PrimPlayer_DrawCallback, {}, {}, {}, {}, {}, game::MAG_047_ResetTrailHistory, game::MAG_047_Director_StartPositions, game::MAG_047_FillTrailRows, game::MAG_047_Director_Mode3Setup, game::MAG_047_Director_TargetPositions, game::MAG_047_RelocateActorData, game::MAG_047_Actor_DrawSprite, {}, game::MAG_047_DampValue, game::MAG_047_DampVector, {}, {}, game::MAG_047_MorphVertexBlock, {}, {}, {}, game::MAG_047_RandRange, game::MAG_047_RandRangeWide, game::MAG_047_RandAngleJitter, game::MAG_047_Particle_AttachToOwner, game::MAG_047_Actor_InterpolatePosition, {}, game::MAG_047_ParticleList_Unlink, game::MAG_047_Particle_PlaceAtAnchorSet, game::MAG_047_Particle_PlaceAtPoint, {}, {}, game::MAG_047_Emitter_ApplyDamage, game::MAG_047_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_047_Emitter_State3_FinishReleaseRoot, game::MAG_047_Emitter_State4_Empty, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::MAG_047_PacketCursor;
	static constexpr game::Id FA_CURSOR = game::BATTLE_PACKET_WRITE_CURSOR;
	static constexpr game::Id ACTOR_STATE = game::MAG_047_ActorStatePtr;
	static constexpr game::Id SCRATCH_SP = game::MAG_047_ScratchSP;
	static constexpr game::Id PHASE_DATA = game::unk_15B8028;
	static constexpr game::Id DIR_PTR = PHASE_DATA;
	static constexpr game::Id ASSET_TABLE_PTR = game::MAG_047_AssetTablePtr;
	static constexpr game::Id Q_PRIM = game::MAG_047_PrimModelQueue, Q_STAGE = game::MAG_047_StageDarkenQueue;
	static constexpr game::Id ORIG_Master = game::MAG_047_CURSE_MasterTask;
	static constexpr game::Id PRIM_CB = game::MAG_047_PrimPlayer_DrawCallback;
	static inline uint32_t ENT(uint32_t slot) { return game_addr(game::BattleEntitySlotData) + slot * 0x9C; }

	namespace dx
	{
		static inline uint32_t sub_45E0B0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadIR123_u16))(a1); }
		static inline uint32_t sub_45E9D0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPF))(); }
		static inline uint32_t sub_45EBF0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPL))(); }
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

	static inline uint32_t t1_task_end(uint32_t node, uint8_t status) { return task_end(node, status); }

	uint32_t __cdecl cu_82A970(uint32_t a1);
	uint32_t __cdecl cu_82AB10(uint32_t a1);
	uint32_t __cdecl cu_82AB50(void);
	uint32_t __cdecl cu_82AC40(uint32_t a1);
	uint32_t __cdecl cu_82AC90(void);
	uint32_t __cdecl cu_82B0F0(uint32_t a1);
	uint32_t __cdecl cu_82B170(uint32_t a1);
	uint32_t __cdecl cu_82B220(uint32_t a1);
	uint32_t __cdecl cu_82B2C0(uint32_t a1);
	uint32_t __cdecl cu_82B370(uint32_t a1);
	uint32_t __cdecl cu_82B400(uint32_t a1);
	uint32_t __cdecl cu_82B460(uint32_t a1);
	uint32_t __cdecl cu_82B4A0(uint32_t a1);
	uint32_t __cdecl cu_82B500(uint32_t a1);
	uint32_t __cdecl cu_82B5F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl cu_82B6A0(uint32_t a1);
	uint32_t __cdecl cu_82B710(uint32_t a1);
	uint32_t __cdecl cu_82B870(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl cu_82BAB0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl cu_82BBE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl cu_82BE00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl cu_82C090(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl cu_82C580(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl cu_82D0A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl cu_82D600(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl cu_82DCD0(uint32_t a1);
	uint32_t __cdecl cu_82DD60(uint32_t a1);
	uint32_t __cdecl cu_82DDD0(uint32_t a1);
	uint32_t __cdecl cu_82DE60(uint32_t a1);
	uint32_t __cdecl cu_82DF30(uint32_t a1);
	uint32_t __cdecl cu_82DFC0(uint32_t a1);
	uint32_t __cdecl cu_82E1A0(uint32_t a1);
	uint32_t __cdecl cu_82E420(void);
	uint32_t __cdecl cu_82EAD0(uint32_t a1);
	uint32_t __cdecl cu_82EB60(void);
	uint32_t __cdecl cu_82EBC0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl cu_82EE60(void);
	uint32_t __cdecl cu_82EEA0(uint32_t a1);
	uint32_t __cdecl cu_82FED0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl cu_82FFE0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl cu_830060(uint32_t a1, uint32_t a2);
	uint32_t __cdecl cu_830090(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl cu_830130(uint32_t a1);
	uint32_t __cdecl cu_8301B0(uint32_t a1);
	uint32_t __cdecl cu_830230(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl cu_830A10(uint32_t a1, uint32_t a2);
	uint32_t __cdecl cu_830C30(uint32_t a1);
	uint32_t __cdecl cu_831720(uint32_t a1, uint32_t a2);
	uint32_t __cdecl cu_8317B0(void);
	uint32_t __cdecl cu_831840(uint32_t a1);
	uint32_t __cdecl cu_831920(uint32_t a1);
	uint32_t __cdecl cu_831960(uint32_t a1);
	uint32_t __cdecl cu_831980(uint32_t a1);
	uint32_t __cdecl cu_8319D0(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace curse
{
	uint32_t __cdecl cu_82A970(uint32_t a1)
	{
		g_mod = &MOD_047;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::MAG_047_ScratchSP)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_264B4D8)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_047_CURSE_Master_State0_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_047_CURSE_Master_State1_Advance);
		states[2] = game_addr(game::MAG_047_Master_CarveActorPools);
		states[3] = game_addr(game::MAG_047_CURSE_Master_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_047_CURSE_Master_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_047_CURSE_Master_State5_Advance);
		states[6] = game_addr(game::MAG_047_CURSE_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_047_CURSE_Master_State7_RequestBattleSync);
		states[8] = game_addr(game::MAG_047_CURSE_Master_State8_WaitBattleSyncDone);
		states[9] = game_addr(game::MAG_047_CURSE_Master_State9_Finish);
		states[10] = game_addr(game::MAG_047_CURSE_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_264E69C));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_264DFDC));
			MEM<uint32_t>(game_addr(game::MAG_047_PacketCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_264B4D4)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_264E698));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_264DFD8));
			MEM<uint32_t>(game_addr(game::MAG_047_PacketCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_264B4D4)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_264EAC0)) = 0;
		MEM<uint16_t>(game_addr(game::unk_264E704)) = 0;
		const uint32_t queues[7] = { game_addr(game::MAG_047_EmitterQueue), game_addr(game::MAG_047_DirectorQueue), game_addr(game::MAG_047_LoadScriptQueue), game_addr(game::MAG_047_ActorSystemQueue),
			game_addr(game::MAG_047_StageDarkenQueue), game_addr(game::MAG_047_PrimModelQueue), game_addr(game::MAG_047_UnusedQueue) };
		for (int i = 0; i < 7; i++)
		{
			const uint32_t n = x::ExecuteTaskQueue(queues[i]);
			U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)n);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl cu_82AB10(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::MAG_047_FileArenaCursor));
		MEM<uint32_t>(game_addr(game::MAG_047_ParticlePoolPtr)) = p;
		p += 0x438;
		MEM<uint32_t>(game_addr(game::MAG_047_ActorPoolPtr)) = p;
		p += 0xB7C0;
		MEM<uint32_t>(game_addr(game::MAG_047_FileArenaCursor)) = p;
		cu_82AB50();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl cu_82AB50(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_047_ParticlePoolPtr)), 0x438);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_047_ActorPoolPtr)), 0xB7C0);
		MEM<uint16_t>(game_addr(game::MAG_047_ParticleCursor)) = 0;
		MEM<uint16_t>(game_addr(game::MAG_047_ActorCursor)) = 0;
		MEM<uint16_t>(game_addr(game::unk_264E6A4)) = 0;
		MEM<uint16_t>(game_addr(game::unk_264B3D0)) = 0;
		return 0;
	}

	uint32_t __cdecl cu_82AC40(uint32_t a1)
	{
		cu_82B0F0(a1);
		cu_82AC90();
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_047_LoadScriptQueue), game_addr(game::MAG_047_FileLoadScriptTask), 0x30, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_047_DirectorQueue), game_addr(game::MAG_047_DirectorTask), 0x48, a1);
		t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl cu_82AC90(void)
	{
		MEM<uint32_t>(game_addr(game::unk_264E6A0)) = game_addr(game::unk_15B8230);
		MEM<uint32_t>(game_addr(game::MAG_047_AssetTablePtr)) = game_addr(game::unk_264EAF0);
		x::MAG_007_sub_8DCC00(game_addr(game::unk_264EAF0), 0x16C);
		const uint32_t t = MEM<uint32_t>(game_addr(game::MAG_047_AssetTablePtr));
		uint32_t c = MEM<uint32_t>(game_addr(game::MAG_047_FileArenaCursor));
		U32(t, 0x100) = c;
		c += 0xA000;
		U32(t, 0x114) = game_addr(game::unk_19EB018);
		MEM<uint32_t>(game_addr(game::MAG_047_FileArenaCursor)) = c;
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

	uint32_t __cdecl cu_82B0F0(uint32_t a1)
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

	uint32_t __cdecl cu_82B170(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[12];
		states[0] = game_addr(game::MAG_047_Director_State0_WaitDirGate1);
		states[1] = game_addr(game::MAG_047_Director_State1_PlaySoundAtCut1);
		states[2] = game_addr(game::MAG_047_Director_State2_ArmLoadScriptStep);
		states[3] = game_addr(game::MAG_047_Director_EffectLight);
		states[4] = game_addr(game::MAG_047_Director_State4_WaitPhaseTick36);
		states[5] = game_addr(game::MAG_047_Director_State5_WaitDirGate2);
		states[6] = game_addr(game::MAG_047_Director_State6_WaitDirPhase2);
		states[7] = game_addr(game::MAG_047_Director_DamageCue);
		states[8] = game_addr(game::MAG_047_Director_State8_WaitDirGate3);
		states[9] = game_addr(game::MAG_047_Director_State9_WaitDirPhase3);
		states[10] = game_addr(game::MAG_047_Director_State10_Finish);
		states[11] = game_addr(game::MAG_047_Director_State11_Empty);
		cu_82B220(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return t1_task_end(node, status);
	}

	uint32_t __cdecl cu_82B220(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			cu_82B500(a1);
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

	uint32_t __cdecl cu_82B2C0(uint32_t a1)
	{
		uint32_t r = a_73B7E0(1);
		if (r == 0) return 0;
		x::BdPlaySE(game_addr(game::unk_15B802C), 0, 0x80);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl cu_82B4A0(uint32_t a1)
	{
		if (a_73B7E0(3) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl cu_82BAB0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		static const Draw lists[8] = { cu_82BBE0, cu_82BE00, cu_82C090, cu_82C580, a_73D770, a_73D9C0, cu_82D0A0, cu_82D600 };
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

	uint32_t __cdecl cu_82BBE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl cu_82BE00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl cu_82C090(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl cu_82C580(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl cu_82D0A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl cu_82D600(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl cu_82DCD0(uint32_t a1)
	{
		const uint32_t r = a_73C280(a1);
		if (r == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl cu_82DDD0(uint32_t a1)
	{
		const uint32_t r = a_73C280(a1);
		if (r == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl cu_82DF30(uint32_t a1)
	{
		const uint32_t r = a_73C280(a1);
		if (r == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl cu_82DFC0(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			a_73F990(a1);
			cu_82EAD0(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl cu_82E1A0(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_047_ActorStatePtr));
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

	uint32_t __cdecl cu_82E420(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_047_ActorStatePtr));
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

	uint32_t __cdecl cu_82EAD0(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::MAG_047_ActorStatePtr), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			cu_8317B0();
			cu_82EE60();
			a_740700();
			cu_82EB60();
			sys = U32(game_addr(game::MAG_047_ActorStatePtr), 0);
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
		U16(game_addr(game::unk_264EAC0), 0) = (uint16_t)(U16(game_addr(game::unk_264EAC0), 0) + c14);
		U16(game_addr(game::unk_264E704), 0) = (uint16_t)(U16(game_addr(game::unk_264E704), 0) + c16);
		return done;
	}

	uint32_t __cdecl cu_82EB60(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_047_ActorStatePtr), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::MAG_047_ActorStatePtr), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					cu_82EBC0(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl cu_82EBC0(uint32_t a1, uint32_t a2)
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
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = cu_82BAB0(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
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
				MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = cu_82BAB0(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)));
			}
		}
		const uint32_t top = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 0x68;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = top;
		return top;
	}

	uint32_t __cdecl cu_82EE60(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_047_ActorStatePtr), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				cu_830130(act);
			else if (type == 1)
				cu_82EEA0(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl cu_82EEA0(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::MAG_047_ActorStatePtr));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (cu_82FFE0(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::MAG_047_ActorStatePtr));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl cu_82FED0(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::MAG_047_ActorStatePtr), 0);
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

	uint32_t __cdecl cu_82FFE0(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				cu_830060(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			cu_830060(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			cu_830060(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl cu_830060(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return cu_830090(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl cu_830090(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_047_ParticlePoolPtr));
		int32_t idx = MEM<int16_t>(game_addr(game::MAG_047_ParticleCursor));
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
			MEM<uint16_t>(game_addr(game::MAG_047_ParticleCursor)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::MAG_047_ParticleCursor)) = 0;
		return slot;
	}

	uint32_t __cdecl cu_830130(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_047_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_047_Actor_InterpolatePosition), a1);
		cu_8301B0(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl cu_8301B0(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::MAG_047_ActorStatePtr), 0);
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
				cu_830230(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			cu_830230(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl cu_830230(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::MAG_047_ScratchSP), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::MAG_047_ScratchSP), 0) = sp;
		uint32_t node = cu_830A10(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::MAG_047_ActorStatePtr), 0);
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
		cu_831720(node, desc);
		U32(game_addr(game::MAG_047_ScratchSP), 0) = U32(game_addr(game::MAG_047_ScratchSP), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl cu_830A10(uint32_t a1, uint32_t a2)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_047_ActorPoolPtr));
		int32_t i = MEM<int16_t>(game_addr(game::MAG_047_ActorCursor));
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
			MEM<uint16_t>(game_addr(game::MAG_047_ActorCursor)) = 0;
		else
			MEM<uint16_t>(game_addr(game::MAG_047_ActorCursor)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl cu_830C30(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::MAG_047_ActorStatePtr), 0);
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
				uint32_t g = U32(game_addr(game::MAG_047_ActorStatePtr), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::MAG_047_ActorStatePtr), 0);
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

	uint32_t __cdecl cu_831720(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			cu_830090(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			cu_830090(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			cu_830090(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			cu_830090(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl cu_8317B0(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::MAG_047_ActorStatePtr));
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
					cu_830090(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::MAG_047_ActorStatePtr));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				cu_830090(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::MAG_047_ActorStatePtr));
			}
		}
		return 0;
	}

	uint32_t __cdecl cu_831840(uint32_t a1)
	{
		if (cu_82EAD0(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl cu_831920(uint32_t a1)
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

	uint32_t __cdecl cu_831960(uint32_t a1)
	{
		if (a_73B7E0(3) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl cu_831980(uint32_t a1)
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

	uint32_t __cdecl cu_8319D0(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_15B8028));
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

	uint32_t __cdecl cu_82B370(uint32_t a1)
	{
		if (U16(MEM<uint32_t>(game_addr(ASSET_TABLE_PTR)), 0x166) == 0)
		{
			a_73BB60(0);
			next_state(a1);
			return a1;
		}
		if (S16(MEM<uint32_t>(game_addr(PHASE_DATA)), 0x46) >= 0x35)
			a_73BB60(1);
		return 0;
	}

	uint32_t __cdecl cu_82B400(uint32_t a1)
	{
		if (S16(MEM<uint32_t>(game_addr(PHASE_DATA)), 0x46) >= 0x36)
			next_state(a1);
		return 0;
	}

	uint32_t __cdecl cu_82B460(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(PHASE_DATA));
		if (S16(d, 0x46) > 0x32)
		{
			U16(d, 0x48) = 1;
			next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl cu_82B500(uint32_t a1)
	{
		const int32_t k = S16(MEM<uint32_t>(game_addr(PHASE_DATA)), 0x40);
		if (k == 1)
		{
			cu_82B5F0(a1, game_addr(game::MAG_047_PrimTaskRighty), game_addr(game::unk_17BB134), 0x154, 0, 0);
			cu_82B5F0(a1, game_addr(game::MAG_047_PrimTaskLefty), game_addr(game::unk_17BDF6C), 0x154, 1, 0);
			cu_82B5F0(a1, game_addr(game::MAG_047_PrimTaskCenter), game_addr(game::unk_17C0DA4), 0x1F4, 2, 0);
			return x::Effect_AddTaskAndInitFromCtx(game_addr(Q_STAGE), game_addr(game::MAG_047_StageDarkenTask), 0xB0, a1);
		}
		if (k == 2)
			return a_73C100(a1, game_addr(game::MAG_047_ActorSystemTask), game_addr(game::unk_17B97A4), 0, 0, 0);
		return (uint32_t)(k - 2);
	}

	uint32_t __cdecl cu_82B5F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PRIM), a2, 0x288, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		U32(t, 0x78) = (uint32_t)(int32_t)(int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl cu_82B6A0(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		const uint32_t e = cu_82B710(0x38);
		if (e != 0)
			x::GetEffectSpawnPosition(e, 0xF, 0, node + 0x1C);
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		U16(node, 0x88) = 0xFF00;
		a_73C280(node);
		next_state(node);
		return 0;
	}

	uint32_t __cdecl cu_82DD60(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		const uint32_t e = cu_82B710(0x37);
		if (e != 0)
			x::GetEffectSpawnPosition(e, 0xF, 0, node + 0x1C);
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		U16(node, 0x88) = 0xFF00;
		a_73C280(node);
		next_state(node);
		return 0;
	}

	uint32_t __cdecl cu_82B710(uint32_t a1)
	{
		for (uint32_t e = game_addr(game::BattleEntitySlotData); e < game_addr(game::CAMERA_FLAG_RELATED); e += 0x9C)
		{
			if ((U8(e, 0) & 2) != 0 && (uint16_t)U8(e, 4) == (uint16_t)a1)
				return e;
		}
		return 0;
	}

	uint32_t __cdecl cu_82DE60(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 0xF)
			return 0;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		alignas(4) uint8_t loc[0x10] = {};
		const uint32_t L = P(loc);
		const uint32_t e1 = cu_82B710(0x37);
		if (e1 != 0)
		{
			x::GetEffectSpawnPosition(e1, 0xF, 0, L + 8);
			const uint32_t e2 = cu_82B710(0x38);
			if (e2 != 0)
			{
				x::GetEffectSpawnPosition(e2, 0xF, 0, L);
				U16(node, 0x1C) = (uint16_t)(((int32_t)S16(L, 0) + (int32_t)S16(L, 8)) / 2);
				U16(node, 0x1E) = (uint16_t)(((int32_t)S16(L, 2) + (int32_t)S16(L, 0xA)) / 2);
				U16(node, 0x20) = (uint16_t)(((int32_t)S16(L, 4) + (int32_t)S16(L, 0xC)) / 2);
			}
		}
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		U16(node, 0x88) = 0xFF80;
		a_73C280(node);
		next_state(node);
		return 0;
	}

	uint32_t __cdecl cu_82B870(uint32_t a1, uint32_t a2, uint32_t a3)
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
		MEM<uint32_t>(game_addr(FA_CURSOR)) = cu_82BAB0(hdr, ot, 2, cursor);
		x::Field_Free(0x68);
		return 0;
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_047_CURSE_Master_State0_Advance, (void *)a_73A0D0, "047 MAG_047_sub_82AAF0" },
		{ game::MAG_047_CURSE_Master_State1_Advance, (void *)a_73A0D0, "047 MAG_047_sub_82AB00" },
		{ game::MAG_047_CURSE_Master_State3_SpawnEmitter, (void *)a_73A170, "047 MAG_014_DEATH_DEATH_STONE_0_sub_82AB90" },
		{ game::MAG_047_EmitterTask, (void *)a_73A1A0, "047 MAG_014_DEATH_DEATH_STONE_0_sub_82ABC0" },
		{ game::MAG_047_FileLoadScriptTask, (void *)a_73A380, "047 MAG_014_DEATH_DEATH_STONE_0_sub_82AD90" },
		{ game::MAG_047_LoadScript_Issue, (void *)a_73A3E0, "047 MAG_014_DEATH_DEATH_STONE_0_sub_82ADF0" },
		{ game::MAG_047_LoadCb_UploadTim, (void *)a_73A580, "047 sub_82AF90" },
		{ game::MAG_047_LoadCb_SummonStream, (void *)a_73A5B0, "047 sub_82AFC0" },
		{ game::FFModuleHandlerInitSystem, (void *)a_73A5E0, "047 FFModuleHandlerInitSystem" },
		{ game::FFModuleHandlerExitSystem, (void *)a_73A5E0, "047 FFModuleHandlerExitSystem" },
		{ game::MAG_047_LoadCb_ClearBusy, (void *)a_73A5E0, "047 sub_82B010" },
		{ game::MAG_047_LoadCb_ClearBusy_2, (void *)a_73A5E0, "047 sub_82B020" },
		{ game::MAG_047_LoadScript_RecordFile, (void *)a_73A620, "047 sub_82B030" },
		{ game::MAG_047_FileLoadScript_State1_LoadScriptWait, (void *)a_73A660, "047 sub_82B070" },
		{ game::MAG_047_FileLoadScript_State2_Empty, (void *)a_73A6D0, "047 nullsub_1559" },
		{ game::MAG_047_Director_State0_WaitDirGate1, (void *)a_73B790, "047 sub_82B270" },
		{ game::MAG_047_DirSyncGate, (void *)a_73B640, "047 sub_82B290" },
		{ game::MAG_047_DirPhaseGE, (void *)a_73B7E0, "047 sub_82B2F0" },
		{ game::MAG_047_Director_State2_ArmLoadScriptStep, (void *)a_73A930, "047 sub_82B310" },
		{ game::MAG_047_LoadScript_TryArmStep, (void *)a_73A950, "047 sub_82B330" },
		{ game::MAG_047_SetPauseSwitch, (void *)a_73BB60, "047 sub_82B3B0" },
		{ game::MAG_047_Director_State5_WaitDirGate2, (void *)a_73B8A0, "047 sub_82B420" },
		{ game::MAG_047_Director_State6_WaitDirPhase2, (void *)a_73B8C0, "047 sub_82B440" },
		{ game::MAG_047_Director_State8_WaitDirGate3, (void *)a_733B70, "047 sub_82B480" },
		{ game::MAG_047_Director_State10_Finish, (void *)a_7475A0, "047 sub_82B4C0" },
		{ game::MAG_047_Director_State11_Empty, (void *)a_73A6D0, "047 nullsub_1560" },
		{ game::MAG_047_St_Empty, (void *)a_73A6D0, "047 nullsub_1561" },
		{ game::MAG_047_SpawnChildTask, (void *)a_73C100, "047 sub_82B5A0" },
		{ game::MAG_047_PrimTaskRighty, (void *)a_73A380, "047 sub_82B640" },
		{ game::MAG_047_DrawPrimModel, (void *)a_73C280, "047 sub_82B740" },
		{ game::MAG_047_EmitG3, (void *)a_73D770, "047 sub_82CB70" },
		{ game::MAG_047_EmitG4, (void *)a_73D9C0, "047 sub_82CDC0" },
		{ game::MAG_047_PrimTaskRighty_State2_Empty, (void *)a_73A6D0, "047 nullsub_1562" },
		{ game::MAG_047_PrimTaskLefty, (void *)a_73A380, "047 sub_82DD00" },
		{ game::MAG_047_PrimTaskLefty_State2_Empty, (void *)a_73A6D0, "047 nullsub_1563" },
		{ game::MAG_047_PrimTaskCenter, (void *)a_73A380, "047 sub_82DE00" },
		{ game::MAG_047_PrimTaskCenter_State2_Empty, (void *)a_73A6D0, "047 nullsub_1564" },
		{ game::MAG_047_ActorSystemTask, (void *)a_73A380, "047 sub_82DF60" },
		{ game::MAG_047_DirectorSetup, (void *)a_73F990, "047 sub_82DFF0" },
		{ game::MAG_047_Director_StartPositions, (void *)a_73FC20, "047 sub_82E280" },
		{ game::MAG_047_Director_Mode3Setup, (void *)a_73FE90, "047 sub_82E4F0" },
		{ game::MAG_047_Director_TargetPositions, (void *)a_73FFB0, "047 sub_82E610" },
		{ game::MAG_047_RelocateActorData, (void *)a_740210, "047 sub_82E870" },
		{ game::MAG_047_ActSys_DrawSpritePass, (void *)a_740700, "047 sub_82ED20" },
		{ game::MAG_047_Actor_DrawSprite, (void *)a_7407C0, "047 sub_82EDE0" },
		{ game::MAG_047_ActorMotion, (void *)a_740910, "047 sub_82EF30" },
		{ game::MAG_047_DampValue, (void *)a_740F70, "047 sub_82F590" },
		{ game::MAG_047_DampVector, (void *)a_740FC0, "047 sub_82F5E0" },
		{ game::MAG_047_Instance_AdvanceTexFrame, (void *)a_741050, "047 sub_82F670" },
		{ game::MAG_047_Instance_BuildTransform, (void *)a_741120, "047 sub_82F740" },
		{ game::MAG_047_Particle_LifeCountdown, (void *)a_741B60, "047 sub_830180" },
		{ game::MAG_047_RandRange, (void *)a_742290, "047 au_re__rand_44" },
		{ game::MAG_047_RandRangeWide, (void *)a_7422C0, "047 sub_8308E0" },
		{ game::MAG_047_RandVec3, (void *)a_742300, "047 sub_830920" },
		{ game::MAG_047_JitterAngles3, (void *)a_742350, "047 sub_830970" },
		{ game::MAG_047_RandAngleJitter, (void *)a_7423A0, "047 au_re__rand_44_0" },
		{ game::MAG_047_GetWord8Minus1, (void *)a_7424B0, "047 sub_830AD0" },
		{ game::MAG_047_Particle_AttachToOwner, (void *)a_7424C0, "047 sub_830AE0" },
		{ game::MAG_047_ParticleList_Append, (void *)a_742CA0, "047 sub_8312C0" },
		{ game::MAG_047_ParticleList_Unlink, (void *)a_742CD0, "047 sub_8312F0" },
		{ game::MAG_047_Particle_Init, (void *)a_742D00, "047 sub_831320" },
		{ game::MAG_047_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "047 sub_8314D0" },
		{ game::MAG_047_Particle_PlaceAtPoint, (void *)a_742FF0, "047 sub_831610" },
		{ game::MAG_047_ActorSystem_State2_Empty, (void *)a_73A6D0, "047 nullsub_1565" },
		{ game::MAG_047_StageDarkenTask, (void *)a_73F110, "047 sub_831870" },
		{ game::MAG_047_StageDarken_State0_ResetEntityRecords, (void *)a_7335D0, "047 sub_8318E0" },
		{ game::MAG_047_StageDarken_State4_Empty, (void *)a_73A6D0, "047 nullsub_1566" },
		{ game::MAG_047_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "047 MAG_047_sub_831A10" },
		{ game::MAG_047_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "047 MAG_047_sub_831A30" },
		{ game::MAG_047_Emitter_State4_Empty, (void *)a_73A6D0, "047 nullsub_1567" },
		{ game::MAG_047_CURSE_Master_State4_NextTargetLoop, (void *)a_747500, "047 MAG_047_sub_831A60" },
		{ game::MAG_047_CURSE_Master_State5_Advance, (void *)a_73A0D0, "047 MAG_047_sub_831AA0" },
		{ game::MAG_047_CURSE_Master_State6_WaitQueuesEmpty, (void *)a_747550, "047 MAG_047_sub_831AB0" },
		{ game::MAG_047_CURSE_Master_State7_RequestBattleSync, (void *)a_747560, "047 MAG_047_sub_831AC0" },
		{ game::MAG_047_CURSE_Master_State8_WaitBattleSyncDone, (void *)a_747590, "047 MAG_047_sub_831AF0" },
		{ game::MAG_047_CURSE_Master_State9_Finish, (void *)a_7475A0, "047 MAG_047_sub_831B00" },
		{ game::MAG_047_CURSE_Master_State10_Empty, (void *)a_73A6D0, "047 nullsub_1558" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_047_CURSE_MasterTask, (void *)cu_82A970, "047 cu_82A970" },
		{ game::MAG_047_Master_CarveActorPools, (void *)cu_82AB10, "047 cu_82AB10" },
		{ game::MAG_047_ClearActorPools, (void *)cu_82AB50, "047 cu_82AB50" },
		{ game::MAG_047_Emitter_StartDirector, (void *)cu_82AC40, "047 cu_82AC40" },
		{ game::MAG_047_InitAssetTable, (void *)cu_82AC90, "047 cu_82AC90" },
		{ game::MAG_047_InitPhaseData, (void *)cu_82B0F0, "047 cu_82B0F0" },
		{ game::MAG_047_DirectorTask, (void *)cu_82B170, "047 cu_82B170" },
		{ game::MAG_047_Director_PhaseBookkeeping, (void *)cu_82B220, "047 cu_82B220" },
		{ game::MAG_047_Director_State1_PlaySoundAtCut1, (void *)cu_82B2C0, "047 cu_82B2C0" },
		{ game::MAG_047_Director_EffectLight, (void *)cu_82B370, "047 cu_82B370" },
		{ game::MAG_047_Director_State4_WaitPhaseTick36, (void *)cu_82B400, "047 cu_82B400" },
		{ game::MAG_047_Director_DamageCue, (void *)cu_82B460, "047 cu_82B460" },
		{ game::MAG_047_Director_State9_WaitDirPhase3, (void *)cu_82B4A0, "047 cu_82B4A0" },
		{ game::MAG_047_SpawnPhaseTasks, (void *)cu_82B500, "047 cu_82B500" },
		{ game::MAG_047_SpawnPrimModelTask, (void *)cu_82B5F0, "047 cu_82B5F0" },
		{ game::MAG_047_PrimTaskRighty_Start, (void *)cu_82B6A0, "047 cu_82B6A0" },
		{ game::MAG_047_FindEntityByMonsterId, (void *)cu_82B710, "047 cu_82B710" },
		{ game::MAG_047_PrimPlayer_DrawCallback, (void *)cu_82B870, "047 cu_82B870" },
		{ game::MAG_047_RenderPrimModel, (void *)cu_82BAB0, "047 cu_82BAB0" },
		{ game::MAG_047_PrimList_FlatTri, (void *)cu_82BBE0, "047 cu_82BBE0" },
		{ game::MAG_047_PrimList_FlatQuad, (void *)cu_82BE00, "047 cu_82BE00" },
		{ game::MAG_047_PrimList_TexTri, (void *)cu_82C090, "047 cu_82C090" },
		{ game::MAG_047_PrimList_TexQuad, (void *)cu_82C580, "047 cu_82C580" },
		{ game::MAG_047_PrimList_GT3Scroll, (void *)cu_82D0A0, "047 cu_82D0A0" },
		{ game::MAG_047_PrimList_GT4Scroll, (void *)cu_82D600, "047 cu_82D600" },
		{ game::MAG_047_PrimTaskRighty_State1_PlayModel, (void *)cu_82DCD0, "047 cu_82DCD0" },
		{ game::MAG_047_PrimTaskLefty_Start, (void *)cu_82DD60, "047 cu_82DD60" },
		{ game::MAG_047_PrimTaskLefty_State1_PlayModel, (void *)cu_82DDD0, "047 cu_82DDD0" },
		{ game::MAG_047_PrimTaskCenter_Start, (void *)cu_82DE60, "047 cu_82DE60" },
		{ game::MAG_047_PrimTaskCenter_State1_PlayModel, (void *)cu_82DF30, "047 cu_82DF30" },
		{ game::MAG_047_ActorSystem_State0_StartActorSystemAtFrame, (void *)cu_82DFC0, "047 cu_82DFC0" },
		{ game::MAG_047_ResetTrailHistory, (void *)cu_82E1A0, "047 cu_82E1A0" },
		{ game::MAG_047_FillTrailRows, (void *)cu_82E420, "047 cu_82E420" },
		{ game::MAG_047_ActSys_Step, (void *)cu_82EAD0, "047 cu_82EAD0" },
		{ game::MAG_047_ActSys_DrawPass, (void *)cu_82EB60, "047 cu_82EB60" },
		{ game::MAG_047_Actor_DrawModel, (void *)cu_82EBC0, "047 cu_82EBC0" },
		{ game::MAG_047_ActSys_UpdatePass, (void *)cu_82EE60, "047 cu_82EE60" },
		{ game::MAG_047_ActorStep, (void *)cu_82EEA0, "047 cu_82EEA0" },
		{ game::MAG_047_MorphVertexBlock, (void *)cu_82FED0, "047 cu_82FED0" },
		{ game::MAG_047_Instance_EndOfLifeTest, (void *)cu_82FFE0, "047 cu_82FFE0" },
		{ game::MAG_047_Instance_SpawnFollowUp, (void *)cu_830060, "047 cu_830060" },
		{ game::MAG_047_AllocParticle, (void *)cu_830090, "047 cu_830090" },
		{ game::MAG_047_ActSys_UpdateEmitter, (void *)cu_830130, "047 cu_830130" },
		{ game::MAG_047_ActSys_EmitterSpawnStep, (void *)cu_8301B0, "047 cu_8301B0" },
		{ game::MAG_047_ActSys_SpawnParticles, (void *)cu_830230, "047 cu_830230" },
		{ game::MAG_047_ActSys_AllocParticleSlot, (void *)cu_830A10, "047 cu_830A10" },
		{ game::MAG_047_Actor_InterpolatePosition, (void *)cu_830C30, "047 cu_830C30" },
		{ game::MAG_047_SpawnSubEffects, (void *)cu_831720, "047 cu_831720" },
		{ game::MAG_047_Director_SpawnGroupEffects, (void *)cu_8317B0, "047 cu_8317B0" },
		{ game::MAG_047_ActorSystem_State1_StepActorSystem, (void *)cu_831840, "047 cu_831840" },
		{ game::MAG_047_StageDarken_State1_RampUp600, (void *)cu_831920, "047 cu_831920" },
		{ game::MAG_047_StageDarken_State2_WaitDirPhase3, (void *)cu_831960, "047 cu_831960" },
		{ game::MAG_047_StageDarken_State3_FadeOut, (void *)cu_831980, "047 cu_831980" },
		{ game::MAG_047_Emitter_ApplyDamage, (void *)cu_8319D0, "047 cu_8319D0" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag047_curse()
	{
		act::register_module(47);
		for (const act::curse::ModPort *p = act::curse::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(47, p->addr, p->port, p->name);
		for (const act::curse::ModPort *p = act::curse::PORTS; p->addr; p++)
			act::register_module_port(47, p->addr, p->port, p->name);
	}
}

