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

// Effect 46: Sigh

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
namespace sigh
{
	static const Mod MOD_046 = { "sigh", 46, game::MAG_046_SIGH_FL, game::MAG_045_STARE_FL,
		{ {}, {}, {}, {}, {}, {}, {}, {}, game::g_Sigh_ActorCursor, game::g_Sigh_ActorPool, {}, {}, game::g_Sigh_FilePtr, {}, {}, {}, game::unk_264EC84, {}, {}, {}, {}, {}, {}, {}, game::MAG_046_ActorSystemQueue, {}, game::MAG_046_EmitterQueue, {}, {}, {}, {}, {}, game::unk_265278A, {}, {}, game::unk_265279A, {}, game::g_Sigh_ScratchSP, {}, game::g_Sigh_ActorState },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_046_DirectorSetup, game::MAG_046_ActSys_Step, game::MAG_046_ActSys_DrawPass, game::MAG_046_Sigh_PrimActorRenderStub, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_046_ActSys_DrawSpritePass, game::MAG_046_ActSys_UpdatePass, game::MAG_046_ActorStep, game::MAG_046_ActSys_UpdateEmitter, game::MAG_046_Particle_LifeCountdown, game::MAG_046_ActSys_EmitterSpawnStep, game::MAG_046_ActSys_SpawnParticles, game::MAG_046_RandVec3, game::MAG_046_JitterAngles3, game::MAG_046_ActSys_AllocParticleSlot, game::MAG_046_GetWord8Minus1, game::MAG_046_Particle_Init, game::MAG_046_SpawnSubEffects, game::MAG_046_Director_SpawnGroupEffects, {}, {}, {}, {}, {}, game::MAG_046_Sigh_EmitterTask, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_046_ResetTrailHistory, game::MAG_046_Director_CasterRefPoints, game::MAG_046_FillTrailRows, game::MAG_046_Director_Mode3Setup, game::MAG_046_Director_TargetRefPoints, game::MAG_046_RelocateActorData, game::MAG_046_Sigh_DrawSpriteActor, game::MAG_046_ActorMotion, game::MAG_046_DampValue, game::MAG_046_DampVector, game::MAG_046_Instance_AdvanceTexFrame, game::MAG_046_Instance_BuildTransform, game::MAG_046_MorphVertexBlock, game::MAG_046_Instance_EndOfLifeTest, game::MAG_046_Instance_SpawnFollowUp, game::MAG_046_AllocParticle, game::MAG_046_RandRange, game::MAG_046_RandRangeWide, game::MAG_046_RandAngleJitter, game::MAG_046_Particle_AttachToOwner, game::MAG_046_Actor_InterpolatePosition, game::MAG_046_ParticleList_Append, game::MAG_046_ParticleList_Unlink, game::MAG_046_Particle_PlaceAtAnchorSet, game::MAG_046_Particle_PlaceAtPoint, {}, game::MAG_046_ActSys_Step, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::g_Sigh_PacketCursor;
	static constexpr game::Id ACTOR_STATE = game::g_Sigh_ActorState;
	static constexpr game::Id SCRATCH_SP = game::g_Sigh_ScratchSP;
	static constexpr game::Id Q_EMITTER = game::MAG_046_EmitterQueue, Q_ACTORS = game::MAG_046_ActorSystemQueue, Q_SIGH = game::MAG_046_SighQueue, Q_PART = game::MAG_046_BreathQueue, Q_STAGE = game::MAG_046_StageQueue;
	static constexpr game::Id ORIG_Master = game::MAG_046_Sigh_MasterTask;
	static constexpr game::Id ORIG_Emitter = game::MAG_046_Sigh_EmitterTask;
	static constexpr game::Id ORIG_ActorSystem = game::MAG_046_Sigh_ActorSystemTask;
	static constexpr game::Id ORIG_Sigh = game::MAG_046_Sigh_SighTask;
	static constexpr game::Id ORIG_Spawner = game::MAG_046_Sigh_BreathSpawnerTask;
	static constexpr game::Id ORIG_Breath = game::MAG_046_Sigh_BreathPuffTask;
	static constexpr game::Id ACTOR_DATA = game::Sigh_ActorData;
	static constexpr game::Id MODEL_A = game::Sigh_PrimModelA, MODEL_B = game::Sigh_PrimModelB;
	static constexpr game::Id COLOR_TABLE_A = game::Sigh_ColorFramesA, COLOR_TABLE_B = game::Sigh_ColorFramesB;
	static constexpr game::Id BURST_TABLE = game::Sigh_BurstTable;
	static constexpr game::Id FADE_TABLE = game::Sigh_FadeTable;
	static constexpr game::Id FLIPBOOK_0 = game::unk_15B82A4, FLIPBOOK_1 = game::unk_15B8438, FLIPBOOK_2 = game::unk_15B8BA4;
	static const void *SOUND_Sigh() { return (const void *)game_addr(game::Sigh_Sound); }
	static inline uint32_t ENT(uint32_t slot) { return game_addr(game::BattleEntitySlotData) + slot * 0x9C; }

	namespace gx
	{
		static inline uint32_t sub_45E0B0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadIR123_u16))(a1); }
		static inline uint32_t sub_45E9D0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPF))(); }
		static inline uint32_t sub_45EBF0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPL))(); }
		static inline uint32_t TransformCameraByShadowRotation(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::TransformCameraByShadowRotation))(a1, a2, a3); }
		static inline uint32_t InitEffectSequenceFromData(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::InitEffectSequenceFromData))(a1, a2, a3, a4); }
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

	uint32_t __cdecl sg_831CB0(uint32_t a1);
	uint32_t __cdecl sg_831E30(uint32_t a1);
	uint32_t __cdecl sg_831E80(void);
	uint32_t __cdecl sg_831F60(uint32_t a1);
	uint32_t __cdecl sg_831FE0(uint32_t a1);
	uint32_t __cdecl sg_832070(uint32_t a1);
	uint32_t __cdecl sg_832160(uint32_t a1);
	uint32_t __cdecl sg_832190(uint32_t a1);
	uint32_t __cdecl sg_832340(uint32_t a1);
	uint32_t __cdecl sg_832420(void);
	uint32_t __cdecl sg_832580(void);
	uint32_t __cdecl sg_832770(void);
	uint32_t __cdecl sg_832BE0(uint32_t a1);
	uint32_t __cdecl sg_832C70(void);
	uint32_t __cdecl sg_832CD0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl sg_832E10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl sg_832E70(void);
	uint32_t __cdecl sg_832F30(uint32_t a1);
	uint32_t __cdecl sg_832FB0(void);
	uint32_t __cdecl sg_832FF0(uint32_t a1);
	uint32_t __cdecl sg_834020(uint32_t a1, uint32_t a2);
	uint32_t __cdecl sg_834130(uint32_t a1, uint32_t a2);
	uint32_t __cdecl sg_8341B0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl sg_8341E0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl sg_834280(uint32_t a1);
	uint32_t __cdecl sg_834300(uint32_t a1);
	uint32_t __cdecl sg_834380(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl sg_834D80(uint32_t a1);
	uint32_t __cdecl sg_835870(uint32_t a1, uint32_t a2);
	uint32_t __cdecl sg_835900(void);
	uint32_t __cdecl sg_835990(uint32_t a1);
	uint32_t __cdecl sg_8359C0(uint32_t a1);
	uint32_t __cdecl sg_835B30(uint32_t a1);
	uint32_t __cdecl sg_835C20(uint32_t a1, uint32_t a2);
	uint32_t __cdecl sg_835C70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7);
	uint32_t __cdecl sg_835CF0(uint32_t a1);
	uint32_t __cdecl sg_835E30(uint32_t a1);
	uint32_t __cdecl sg_835E90(uint32_t a1);
	uint32_t __cdecl sg_835F00(uint32_t a1);
	uint32_t __cdecl sg_835F20(uint32_t a1);
	uint32_t __cdecl sg_836090(uint32_t a1);
	uint32_t __cdecl sg_8360F0(uint32_t a1);
	uint32_t __cdecl sg_836160(uint32_t a1);
	uint32_t __cdecl sg_8361C0(uint32_t a1);
	uint32_t __cdecl sg_8362A0(uint32_t a1);
	uint32_t __cdecl sg_8362C0(uint32_t a1);
	uint32_t __cdecl sg_836310(uint32_t a1);
	uint32_t __cdecl sg_836330(uint32_t a1);
	uint32_t __cdecl sg_8363B0(uint32_t a1);
	uint32_t __cdecl sg_8363F0(uint32_t a1);
	uint32_t __cdecl sg_836480(uint32_t a1);
	uint32_t __cdecl sg_8364A0(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace sigh
{
	uint32_t __cdecl sg_831CB0(uint32_t a1)
	{
		g_mod = &MOD_046;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_264EC84)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_046_Sigh_Master_State0_Advance);
		states[1] = game_addr(game::MAG_046_Sigh_Master_State1_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[2] = game_addr(game::MAG_046_Sigh_MasterReserveActorPools);
		states[3] = game_addr(game::MAG_046_Sigh_MasterSpawnEmitters);
		states[4] = game_addr(game::MAG_046_Sigh_MasterNextAction);
		states[5] = game_addr(game::MAG_046_Sigh_MasterWaitEnd);
		states[6] = game_addr(game::MAG_046_Sigh_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_046_Sigh_Master_State7_RequestBattleSync);
		states[8] = game_addr(game::MAG_046_Sigh_Master_State8_WaitBattleSyncDone);
		states[9] = game_addr(game::MAG_046_Sigh_Master_State9_Finish);
		states[10] = game_addr(game::MAG_046_Sigh_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_2652784));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_2651B7C));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_264EC80)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_2652780));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_2651B78));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_264EC80)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_265279A)) = 0;
		MEM<uint16_t>(game_addr(game::unk_265278A)) = 0;
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_EMITTER)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_ACTORS)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_SIGH)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_PART)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_STAGE)));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl sg_831E30(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::MAG_046_FileArenaCursor));
		MEM<uint32_t>(game_addr(game::g_Sigh_ParticlePool)) = p;
		p += 0x870;
		MEM<uint32_t>(game_addr(game::g_Sigh_ActorPool)) = p;
		p += 0x10680;
		MEM<uint32_t>(game_addr(game::MAG_046_FileArenaCursor)) = p;
		sg_831E80();
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_STAGE), game_addr(game::MAG_046_Sigh_StageWobbleInTask), 0x40, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl sg_831E80(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::g_Sigh_ParticlePool)), 0x870);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::g_Sigh_ActorPool)), 0x10680);
		MEM<uint16_t>(game_addr(game::g_Sigh_ParticleCursor)) = 0;
		MEM<uint16_t>(game_addr(game::g_Sigh_ActorCursor)) = 0;
		MEM<uint16_t>(game_addr(game::unk_2652788)) = 0;
		MEM<uint16_t>(game_addr(game::unk_264EC68)) = 0;
		return 0;
	}

	uint32_t __cdecl sg_831F60(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0x100);
		if (S16(a1, 0x1C) >= 0x400)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0x400;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int i = 4; i != 0; i--, p += 0x2C)
			MEM<uint16_t>(p) = level;
		return 0;
	}

	uint32_t __cdecl sg_831FE0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_046_Sigh_EmitterSpawnActorSystemAndSigh);
		states[1] = game_addr(game::MAG_046_Sigh_EmitterReleaseMaster);
		states[2] = game_addr(game::MAG_046_Sigh_EmitterApplyDamage);
		states[3] = game_addr(game::MAG_046_Sigh_Emitter_State3_Empty);
		x::MAG_001_CURE_Emitter_UpdatePos(node);
		x::MAG_001_CURE_Emitter_ComputeModelBounds(node);
		callp(states[S8(node, 0x29)], node);
		if (U16(node, 0x24) == 0)
			x::BdPlaySE(P(SOUND_Sigh()), 0, 0x80);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl sg_832160(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			sg_832190(a1);
			sg_832BE0(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl sg_832190(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t data = U32(node, 0x30);
		uint16_t w29a = U16(node, 0x29A);
		uint16_t w29c = U16(node, 0x29C);
		uint32_t st = node + 0x34;
		const uint32_t root = U32(node, 0x10);
		MEM<uint32_t>(game_addr(game::g_Sigh_ActorState)) = st;
		U16(st, 0x22) = w29a;
		uint16_t caster = (uint16_t)U8(node, 0x2C);
		U16(st, 0x24) = w29c;
		U32(st, 0x224) = data + 4;
		U32(st, 0x228) = data + 0x44;
		U32(st, 0x22C) = data + 0x84;
		uint32_t relocated = U32(data, 0);
		U32(st, 0x230) = data + 0xC4;
		if (relocated == 0)
		{
			a_740210(data);
			st = MEM<uint32_t>(game_addr(game::g_Sigh_ActorState));
			U32(data, 0) = 1;
		}
		uint16_t ntargets = U16(root, 0x5A);
		U16(st, 0x1E) = caster;
		U16(st, 0x1C) = ntargets;
		if ((int16_t)ntargets > 0)
		{
			int32_t i = 0;
			do
			{
				int32_t action = S8(node, 0x2A);
				uint32_t tab = U32(U32(node, 0x0C), 4);
				uint32_t rec = U32(tab + (uint32_t)mul32(action, 5) * 4, 8);
				U32(st, 4 + 4 * i) = U8(rec, 0x18 * i);
				i++;
			} while (i < S16(st, 0x1C));
		}
		U16(st, 0x1A) = U16(node, 0x29E);
		sg_832420();
		st = MEM<uint32_t>(game_addr(game::g_Sigh_ActorState));
		if (U16(st, 0x24) == 4)
			callp(game_addr(game::MAG_046_ResetTrailHistory), node);
		sg_832770();
		st = MEM<uint32_t>(game_addr(game::g_Sigh_ActorState));
		if (U16(st, 0x24) == 1)
		{
			callp(game_addr(game::MAG_046_FillTrailRows), node);
			st = MEM<uint32_t>(game_addr(game::g_Sigh_ActorState));
		}
		if (U16(st, 0x24) == 3)
		{
			a_73FE90(node);
			st = MEM<uint32_t>(game_addr(game::g_Sigh_ActorState));
		}
		int32_t s1 = S16(st, 0x1E);
		uint32_t e1 = game_addr(game::BattleEntitySlotData) + (uint32_t)mul32(s1, 0x9C);
		uint32_t d10 = U32(e1, 0x1C);
		uint32_t d14 = U32(e1, 0x20);
		uint32_t s2 = U32(st, 4);
		uint32_t e2 = game_addr(game::BattleEntitySlotData) + s2 * 0x9C;
		uint32_t d18 = U32(e2, 0x1C);
		int32_t dx = (int16_t)(uint16_t)((uint16_t)d10 - (uint16_t)d18);
		int32_t dy = (int16_t)(uint16_t)((uint16_t)(d10 >> 16) - (uint16_t)(d18 >> 16));
		int32_t dz = (int16_t)(uint16_t)((uint16_t)d14 - (uint16_t)U32(e2, 0x20));
		int32_t sq = add32(add32(mul32(dx, dx), mul32(dy, dy)), mul32(dz, dz));
		uint32_t dist = x::Sqrt((uint32_t)sq);
		st = MEM<uint32_t>(game_addr(game::g_Sigh_ActorState));
		U32(st, 0) = dist;
		return 0;
	}

	uint32_t __cdecl sg_832340(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::g_Sigh_ActorState));
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

	uint32_t __cdecl sg_832420(void)
	{
		uint32_t st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		const int32_t slot = S16(st, 0x1E);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)mul32(slot, 0x9C);
		alignas(4) uint8_t pos[8] = {};
		const uint32_t pp = P(pos);
		if (U16(st, 0x24) != 2)
		{
			x::GetEffectSpawnPosition(ent, 0xF1, 0, pp);
			st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		}
		const int32_t px = S16(pp, 0);
		U16(pp, 2) = U16(ent, 0x24);
		S32(st, 0x1C4) = shl32(px, 16);
		S32(st, 0x1C8) = shl32(S16(pp, 2), 16);
		S32(st, 0x1CC) = shl32(S16(pp, 4), 16);
		U32(st, 0x1D4) = U32(st, 0x1C4);
		U32(st, 0x1D8) = 0;
		U32(st, 0x1DC) = U32(st, 0x1CC);
		if (U16(st, 0x24) != 2)
		{
			x::GetEffectSpawnPosition(ent, 0xF1, 0, pp);
			st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		}
		S32(st, 0x1E4) = shl32(S16(pp, 0), 16);
		S32(st, 0x1E8) = shl32(S16(pp, 2), 16);
		S32(st, 0x1EC) = shl32(S16(pp, 4), 16);
		if (U16(st, 0x24) != 2)
		{
			x::GetEffectSpawnPosition(ent, 0xF0, 0, pp);
			st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		}
		S32(st, 0x1F4) = shl32(S16(pp, 0), 16);
		S32(st, 0x1F8) = shl32(S16(pp, 2), 16);
		S32(st, 0x1FC) = shl32(S16(pp, 4), 16);
		const uint32_t sx = U32(st, 0x1C4);
		const int32_t h = shl32(S16(ent, 0x3C), 16);
		U32(st, 0x204) = sx;
		S32(st, 0x208) = h;
		U32(st, 0x20C) = U32(st, 0x1CC);
		return 0;
	}

	uint32_t __cdecl sg_832580(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::g_Sigh_ActorState));
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

	uint32_t __cdecl sg_832770(void)
	{
		uint32_t st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		alignas(4) uint8_t pos[8] = {};
		const uint32_t pp = P(pos);
		if (S16(st, 0x1C) > 0)
		{
			uint32_t o = 0;
			for (int32_t i = 0; ; )
			{
				const uint32_t slot = U32(st, 4 + 4 * i);
				const uint32_t ent = game_addr(game::BattleEntitySlotData) + slot * 0x9C;
				if (U16(st, 0x24) != 2)
				{
					x::GetEffectSpawnPosition(ent, 0xF1, 0, pp);
					st = MEM<uint32_t>(game_addr(ACTOR_STATE));
				}
				const int32_t px = S16(pp, 0);
				U16(pp, 2) = U16(ent, 0x24);
				S32(st, o + 0x54) = shl32(px, 16);
				S32(st, o + 0x58) = shl32(S16(pp, 2), 16);
				S32(st, o + 0x5C) = shl32(S16(pp, 4), 16);
				U32(st, o + 0x94) = U32(st, o + 0x54);
				U32(st, o + 0x98) = 0;
				U32(st, o + 0x9C) = U32(st, o + 0x5C);
				if (U16(st, 0x24) != 2)
				{
					x::GetEffectSpawnPosition(ent, 0xF1, 0, pp);
					st = MEM<uint32_t>(game_addr(ACTOR_STATE));
				}
				S32(st, o + 0xD4) = shl32(S16(pp, 0), 16);
				S32(st, o + 0xD8) = shl32(S16(pp, 2), 16);
				S32(st, o + 0xDC) = shl32(S16(pp, 4), 16);
				if (U16(st, 0x24) != 2)
				{
					x::GetEffectSpawnPosition(ent, 0xF0, 0, pp);
					st = MEM<uint32_t>(game_addr(ACTOR_STATE));
				}
				S32(st, o + 0x114) = shl32(S16(pp, 0), 16);
				S32(st, o + 0x118) = shl32(S16(pp, 2), 16);
				S32(st, o + 0x11C) = shl32(S16(pp, 4), 16);
				U32(st, o + 0x154) = U32(st, o + 0x54);
				S32(st, o + 0x158) = shl32(S16(ent, 0x3C), 16);
				U32(st, o + 0x15C) = U32(st, o + 0x5C);
				o += 0x10;
				i++;
				if (i >= S16(st, 0x1C)) break;
			}
		}
		const int32_t n = S16(st, 0x1C);
		int32_t min_x = 0, max_x = 0, max_z = 0, min_z = 0;
		for (int32_t i = 0; i < n; i++)
		{
			const int32_t vx = S32(st, 0x54 + 0x10 * i);
			const int32_t vz = S32(st, 0x5C + 0x10 * i);
			if ((uint16_t)i == 0)
			{
				max_x = vx;
				max_z = vz;
				min_x = vx;
				min_z = vz;
				continue;
			}
			if (vx < min_x) min_x = vx;
			else if (vx > max_x) max_x = vx;
			if (vz < min_z) min_z = vz;
			else if (vz > max_z) max_z = vz;
		}
		S32(st, 0x194) = add32(min_x, max_x) / 2;
		U32(st, 0x198) = 0;
		S32(st, 0x19C) = add32(max_z, min_z) / 2;
		return 0;
	}

	uint32_t __cdecl sg_832BE0(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::g_Sigh_ActorState), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			sg_835900();
			sg_832FB0();
			sg_832E70();
			sg_832C70();
			sys = U32(game_addr(game::g_Sigh_ActorState), 0);
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
		U16(game_addr(game::unk_265279A), 0) = (uint16_t)(U16(game_addr(game::unk_265279A), 0) + c14);
		U16(game_addr(game::unk_265278A), 0) = (uint16_t)(U16(game_addr(game::unk_265278A), 0) + c16);
		return done;
	}

	uint32_t __cdecl sg_832C70(void)
	{
		uint32_t act = U32(U32(game_addr(game::g_Sigh_ActorState), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::g_Sigh_ActorState), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					sg_832CD0(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl sg_832CD0(uint32_t a1, uint32_t a2)
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
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = sg_832E10(ctx, ot, 2, cursor);
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
				const uint32_t r = sg_832E10(saved_ctx, ot, 2, cursor);
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

	uint32_t __cdecl sg_832E70(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::g_Sigh_ActorState)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::g_Sigh_ActorState));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						sg_832F30(act);
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
							sg_832F30(act);
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

	uint32_t __cdecl sg_832F30(uint32_t a1)
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

	uint32_t __cdecl sg_832FB0(void)
	{
		uint32_t act = U32(U32(game_addr(game::g_Sigh_ActorState), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				sg_834280(act);
			else if (type == 1)
				sg_832FF0(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl sg_832FF0(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::g_Sigh_ActorState));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (sg_834130(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::g_Sigh_ActorState));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl sg_834020(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::g_Sigh_ActorState), 0);
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

	uint32_t __cdecl sg_834130(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				sg_8341B0(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			sg_8341B0(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			sg_8341B0(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl sg_8341B0(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return sg_8341E0(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl sg_8341E0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::g_Sigh_ParticlePool));
		int32_t idx = MEM<int16_t>(game_addr(game::g_Sigh_ParticleCursor));
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
			MEM<uint16_t>(game_addr(game::g_Sigh_ParticleCursor)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::g_Sigh_ParticleCursor)) = 0;
		return slot;
	}

	uint32_t __cdecl sg_834280(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_046_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_046_Actor_InterpolatePosition), a1);
		sg_834300(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl sg_834300(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::g_Sigh_ActorState), 0);
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
				sg_834380(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			sg_834380(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl sg_834380(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::g_Sigh_ScratchSP), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::g_Sigh_ScratchSP), 0) = sp;
		uint32_t node = a_7387B0(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::g_Sigh_ActorState), 0);
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
		sg_835870(node, desc);
		U32(game_addr(game::g_Sigh_ScratchSP), 0) = U32(game_addr(game::g_Sigh_ScratchSP), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl sg_834D80(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::g_Sigh_ActorState), 0);
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
				uint32_t g = U32(game_addr(game::g_Sigh_ActorState), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::g_Sigh_ActorState), 0);
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

	uint32_t __cdecl sg_835870(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			sg_8341E0(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			sg_8341E0(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			sg_8341E0(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			sg_8341E0(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl sg_835900(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::g_Sigh_ActorState));
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
					sg_8341E0(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::g_Sigh_ActorState));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				sg_8341E0(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::g_Sigh_ActorState));
			}
		}
		return 0;
	}

	uint32_t __cdecl sg_835990(uint32_t a1)
	{
		if (sg_832BE0(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl sg_835E90(uint32_t a1)
	{
		const uint32_t tab[4] = { game_addr(game::MAG_046_Sigh_BreathSpawner_State0_Advance), game_addr(game::MAG_046_Sigh_BreathSpawnerWait), game_addr(game::MAG_046_Sigh_BreathSpawnerBurst), game_addr(game::MAG_046_Sigh_BreathSpawner_State3_Empty) };
		callp(tab[S8(a1, 0x29)], a1);
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24)++;
		return task_end(a1, status);
	}

	uint32_t __cdecl sg_836090(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_046_Sigh_BreathPuffInit);
		states[1] = game_addr(game::MAG_046_Sigh_BreathPuffMove);
		states[2] = game_addr(game::MAG_046_Sigh_BreathPuff_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		sg_8360F0(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl sg_8360F0(uint32_t a1)
	{
		if ((U8(a1, 0x26) & 4) != 0)
			return 0;
		const uint32_t hdr = x::Field_Alloc(0xB4);
		gx::TransformCameraByShadowRotation(a1 + 0x1C, 0x1000, (uint32_t)(int32_t)S16(a1, 0x54));
		U32(hdr, 0) = U32(a1, 0x4C);
		const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		U16(hdr, 4) = U16(a1, 0x50);
		U16(hdr, 0x24) = 0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = gx::InitEffectSequenceFromData(hdr, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, cursor);
		x::Field_Free(0xB4);
		return 0;
	}

	uint32_t __cdecl sg_8363F0(uint32_t a1)
	{
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) - 1);
		if (S16(a1, 0x60) > 0)
			return 0;
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_STAGE), game_addr(game::MAG_046_Sigh_StageWobbleOutTask), 0x40, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl sg_836480(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x1C) = 0x400;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return a1;
	}

	uint32_t __cdecl sg_8364A0(uint32_t a1)
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

	uint32_t __cdecl sg_832070(uint32_t a1)
	{
		a_73C100(a1, game_addr(ORIG_ActorSystem), game_addr(ACTOR_DATA), 0, 0x2D, 0);
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_SIGH), game_addr(ORIG_Sigh), 0x8C, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl sg_832E10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		(void)a2;
		(void)a3;
		const uint32_t flags = U32(a1, 0x1C);
		if ((flags & 0x2000) == 0)
			U32(a1, 4) = U32(a1, 0) + 8;
		const uint32_t model = U32(a1, 0);
		U32(a1, 0x20) = U32(model, 0) + model;
		if ((flags & 0x1000) == 0)
			U32(a1, 0x18) = 0;
		x::someCameraWork_45DD60(U8(a1, 8), U8(a1, 9), U8(a1, 0xA));
		U32(a1, 0x20) = U32(a1, 0x20) + 0x20;
		return a4;
	}

	uint32_t __cdecl sg_8359C0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[5];
		states[0] = game_addr(game::MAG_046_Sigh_SighSetup);
		states[1] = game_addr(game::MAG_046_Sigh_SighSpawnBreath);
		states[2] = game_addr(game::MAG_046_Sigh_SighWaitFade);
		states[3] = game_addr(game::MAG_046_Sigh_SighFade);
		states[4] = game_addr(game::MAG_046_Sigh_Sigh_State4_Empty);
		callp(states[S8(node, 0x29)], node);
		if ((U8(node, 0x26) & 4) == 0)
		{
			const uint32_t s1 = (uint8_t)(U8(node, 0x84) + 8);
			U16(node, 0x84) = (uint16_t)s1;
			sg_835C70(0x2C0, 0x100, 0x40, 0x100, 0x200, 0x100, s1);
			const uint32_t s2 = (uint8_t)(U8(node, 0x86) + 6);
			U16(node, 0x86) = (uint16_t)s2;
			sg_835C70(0x300, 0x100, 0x20, 0x100, 0x2A0, 0x100, s2);
			sg_835C20(U32(node, 0x74), MEM<uint32_t>(game_addr(COLOR_TABLE_A) + (uint32_t)(int32_t)S16(node, 0x82) * 4));
			sg_835C20(U32(node, 0x78), MEM<uint32_t>(game_addr(COLOR_TABLE_B) + (uint32_t)(int32_t)S16(node, 0x82) * 4));
			uint32_t ang = node + 0x7C;
			for (int k = 3; k != 0; k--, ang += 2)
			{
				U16(node, 0x46) = U16(ang, 0);
				U32(node, 0x4C) = U32(node, 0x74);
				sg_835B30(node);
				U32(node, 0x4C) = U32(node, 0x78);
				sg_835B30(node);
			}
			U16(node, 0x82) = (uint16_t)(U16(node, 0x82) + 1);
			if (S16(node, 0x82) > 0x10)
				U16(node, 0x82) = 0x10;
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl sg_835B30(uint32_t a1)
	{
		if ((U8(a1, 0x26) & 4) != 0)
			return 0;
		uint32_t mbuf[8];
		const uint32_t m = P(mbuf);
		x::MAG_022_sub_8DD770(m);
		x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(a1, 0x46));
		x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(a1, 0x44));
		x::sub_8DD960(m, (uint32_t)(int32_t)S16(a1, 0x48));
		S32(m, 0x14) = S16(a1, 0x1C);
		S32(m, 0x18) = S16(a1, 0x1E);
		S32(m, 0x1C) = S16(a1, 0x20);
		x::scale3DMatrix(m, a1 + 0x30);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), m, m);
		x::GTE_SetRotMatrix_W(m);
		x::GTE_SetTransVector_W(m);
		const uint32_t hdr = x::Field_Alloc(0x58);
		U32(hdr, 0) = U32(a1, 0x4C);
		U32(hdr, 8) = U32(a1, 0x40);
		U32(hdr, 0xC) = (uint32_t)(int32_t)S16(a1, 0x50);
		const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U32(hdr, 0x1C) = 0xF0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = x::Effect_RenderPrimModel(hdr, ot, 2, cursor);
		x::Field_Free(0x58);
		return 0;
	}

	uint32_t __cdecl sg_835C20(uint32_t a1, uint32_t a2)
	{
		const int32_t idx = S32(a1, 0) / 4;
		int32_t n = S32(a1, 0x1C + idx * 4);
		uint32_t rec = a1 + 0x1C + (uint32_t)idx * 4 + 4;
		if (n > 0)
		{
			uint32_t src = a2;
			do
			{
				U32(rec, 0) = U32(src, 0);
				rec += 0x24;
				U32(rec, -0xC) = U32(src, 4);
				U32(rec, -8) = U32(src, 8);
				U32(rec, -4) = U32(src, 0xC);
				src += 0x10;
			} while (--n != 0);
		}
		return 0;
	}

	uint32_t __cdecl sg_835C70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7)
	{
		uint16_t rect[4];
		rect[0] = (uint16_t)a1;
		rect[2] = (uint16_t)a3;
		rect[3] = (uint16_t)(a4 - a7);
		rect[1] = (uint16_t)a2;
		x::QueueBlitCommand(P(rect), a5, a7 + a6);
		if ((uint16_t)a7 != 0)
		{
			rect[1] = (uint16_t)(a2 - a7 + a4);
			rect[3] = (uint16_t)a7;
			x::QueueBlitCommand(P(rect), a5, a6);
		}
		return 0;
	}

	uint32_t __cdecl sg_835CF0(uint32_t a1)
	{
		const uint32_t ent = ENT(U8(a1, 0x2C));
		const uint32_t m = a1 + 0x54;
		U32(a1, 0x1C) = U32(ent, 0x1C);
		U32(a1, 0x20) = U32(ent, 0x20);
		x::MAG_022_sub_8DD770(m);
		if (U16(ent, 0xE) != 0)
			x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(ent, 0xE));
		uint16_t vbuf[4];
		const uint32_t v = P(vbuf);
		U16(v, 0) = 0;
		U16(v, 2) = 0;
		U16(v, 4) = 0xFD00;
		x::matrixMultiplyVector(m, v, v);
		U16(a1, 0x20) = (uint16_t)(U16(a1, 0x20) + U16(v, 4));
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + U16(v, 0));
		U16(a1, 0x1E) = (uint16_t)(U16(a1, 0x1E) + U16(v, 2));
		S32(a1, 0x30) = 0x1000;
		S32(a1, 0x34) = 0x1000;
		S32(a1, 0x38) = 0x1000;
		const uint16_t face = U16(ent, 0xE);
		U16(a1, 0x80) = face;
		U32(a1, 0x74) = game_addr(MODEL_A);
		U32(a1, 0x78) = game_addr(MODEL_B);
		U16(a1, 0x7C) = (uint16_t)(face - 0x155);
		U16(a1, 0x7E) = (uint16_t)(face + 0x155);
		uint16_t rect[4];
		rect[0] = 0x200;
		rect[1] = 0x100;
		rect[2] = 0x40;
		rect[3] = 0x100;
		x::QueueBlitCommand(P(rect), 0x2C0, 0x100);
		rect[0] = 0x2A0;
		rect[1] = 0x100;
		rect[2] = 0x40;
		rect[3] = 0x100;
		x::QueueBlitCommand(P(rect), 0x300, 0x100);
		U8(a1, 0x26) |= 4;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl sg_835E30(uint32_t a1)
	{
		if (S16(a1, 0x24) < 4)
			return 0;
		U8(a1, 0x26) &= 0xFB;
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PART), game_addr(ORIG_Spawner), 0x8C, a1);
		const uint16_t px = U16(a1, 0x1C);
		const uint16_t pz = U16(a1, 0x20);
		U16(t, 0x1C) = px;
		U16(t, 0x1E) = 0;
		U16(t, 0x20) = pz;
		ff8fx::copy<(0x20)>((void *)(t + 0x58), (const void *)(a1 + 0x54));
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl sg_835F00(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 8)
		{
			U16(a1, 0x88) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl sg_835F20(uint32_t a1)
	{
		const uint32_t node = a1;
		uint16_t vbuf[4];
		const uint32_t v = P(vbuf);
		U16(v, 0) = 0;
		U16(v, 2) = 0;
		U16(v, 4) = 0xF400;
		if (MEM<int16_t>(game_addr(BURST_TABLE) + (uint32_t)(int32_t)S16(node, 0x88) * 2) > 0)
		{
			int32_t i = 0;
			do
			{
				uint32_t mbuf[8];
				const uint32_t m = P(mbuf);
				ff8fx::copy<(0x20)>((void *)m, (const void *)(node + 0x58));
				int32_t r = (int32_t)x::CrtRand();
				r = r % 0x400;
				r = (r - 0x200) & 0xFFF;
				x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)(int16_t)r);
				const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PART), game_addr(ORIG_Breath), 0x8C, node);
				const uint32_t pz = U32(node, 0x20);
				U32(t, 0x1C) = U32(node, 0x1C);
				U32(t, 0x20) = pz;
				uint16_t obuf[4];
				const uint32_t o = P(obuf);
				x::matrixMultiplyVector(m, v, o);
				U16(t, 0x1C) = (uint16_t)(U16(t, 0x1C) + U16(o, 0));
				U16(t, 0x1E) = (uint16_t)(U16(t, 0x1E) + U16(o, 2));
				U16(t, 0x20) = (uint16_t)(U16(t, 0x20) + U16(o, 4));
				r = (int32_t)x::CrtRand();
				const int32_t sp = (int16_t)((r & 0x7FF) + 0x400);
				U16(t, 0x78) = (uint16_t)((S16(o, 0) * sp) / 0x1000);
				U16(t, 0x7C) = (uint16_t)((S16(o, 4) * sp) / 0x1000);
				r = (int32_t)x::CrtRand();
				U16(t, 0x7A) = (uint16_t)(-(r & 0x3FF));
				r = (int32_t)x::CrtRand();
				U16(t, 0x82) = (uint16_t)(-16 - (r & 0xF));
				i++;
			} while (i < MEM<int16_t>(game_addr(BURST_TABLE) + (uint32_t)(int32_t)S16(node, 0x88) * 2));
		}
		U16(node, 0x88) = (uint16_t)(U16(node, 0x88) + 1);
		if (MEM<uint16_t>(game_addr(BURST_TABLE) + (uint32_t)(int32_t)S16(node, 0x88) * 2) == 0)
		{
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl sg_836160(uint32_t a1)
	{
		const int32_t r = (int32_t)x::CrtRand() % 3;
		if (r == 0)
			U32(a1, 0x4C) = game_addr(FLIPBOOK_0);
		else if (r == 1)
			U32(a1, 0x4C) = game_addr(FLIPBOOK_1);
		else if (r == 2)
			U32(a1, 0x4C) = game_addr(FLIPBOOK_2);
		U16(a1, 0x52) = 0xE;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl sg_8361C0(uint32_t a1)
	{
		U16(a1, 0x7A) = (uint16_t)(U16(a1, 0x7A) + U16(a1, 0x82));
		int16_t vx = S16(a1, 0x78);
		int16_t vz = S16(a1, 0x7C);
		const int16_t vy = S16(a1, 0x7A);
		vx = (int16_t)(vx - vx / 16);
		U16(a1, 0x78) = (uint16_t)vx;
		vz = (int16_t)(vz - vz / 16);
		U16(a1, 0x7C) = (uint16_t)vz;
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + (uint16_t)(vx / 16));
		U16(a1, 0x1E) = (uint16_t)(U16(a1, 0x1E) + (uint16_t)(vy / 16));
		U16(a1, 0x20) = (uint16_t)(U16(a1, 0x20) + (uint16_t)(vz / 16));
		if (a_743C20(a1) != 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl sg_8362A0(uint32_t a1)
	{
		if (S16(a1, 0x24) > 0x16)
		{
			U16(a1, 0x88) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl sg_8362C0(uint32_t a1)
	{
		U16(a1, 0x88) = (uint16_t)(U16(a1, 0x88) + 1);
		const uint16_t f = MEM<uint16_t>(game_addr(FADE_TABLE) + (uint32_t)(int32_t)S16(a1, 0x88) * 2);
		U16(a1, 0x50) = f;
		if ((int16_t)f >= 0x1000)
		{
			U8(a1, 0x26) |= 1;
			U16(a1, 0x50) = 0x1000;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl sg_836310(uint32_t a1)
	{
		if (U16(a1, 0x24) == 0x28)
		{
			U8(U32(a1, 0x10), 0x63) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl sg_836330(uint32_t a1)
	{
		if (S16(a1, 0x24) < 0x28)
			return 0;
		uint32_t act = U32(U32(a1, 0xC), 4) + (uint32_t)(int32_t)S8(a1, 0x2A) * 20;
		if (U8(act, 0x10) != 0)
		{
			int32_t i = 0;
			uint32_t off = 0;
			do
			{
				x::ApplyActionResultToTarget(U32(act, 8) + off);
				i++;
				off += 0x18;
				act = U32(U32(a1, 0xC), 4) + (uint32_t)(int32_t)S8(a1, 0x2A) * 20;
			} while (i < (int32_t)U8(act, 0x10));
		}
		U8(a1, 0x26) |= 1;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl sg_8363B0(uint32_t a1)
	{
		if (U8(a1, 0x63) != 0)
			return 0;
		const uint8_t k = U8(a1, 0x2A);
		if ((int16_t)(int8_t)k < S16(a1, 0x58))
		{
			U8(a1, 0x2A) = (uint8_t)(k + 1);
			U8(a1, 0x2E) = (uint8_t)(U8(a1, 0x2E) + 1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) - 1);
			return 0;
		}
		U16(a1, 0x60) = 5;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_046_Sigh_Master_State0_Advance, (void *)a_73A0D0, "046 MAG_046_sub_831E10" },
		{ game::MAG_046_Sigh_Master_State1_Advance, (void *)a_73A0D0, "046 MAG_046_sub_831E20" },
		{ game::MAG_046_Sigh_StageWobbleInTask, (void *)a_73A380, "046 MAG_046_sub_831EC0" },
		{ game::MAG_046_Sigh_StageWobbleIn_State0_ResetEntityRecords, (void *)a_7335D0, "046 MAG_046_sub_831F20" },
		{ game::MAG_046_Sigh_StageWobbleIn_State2_Empty, (void *)a_73A6D0, "046 nullsub_1568" },
		{ game::MAG_046_Sigh_MasterSpawnEmitters, (void *)a_73A170, "046 MAG_046_sub_831FB0" },
		{ game::MAG_046_Sigh_SpawnActorSystem, (void *)a_73C100, "046 MAG_046_sub_8320B0" },
		{ game::MAG_046_Sigh_ActorSystemTask, (void *)a_73A380, "046 MAG_046_sub_832100" },
		{ game::MAG_046_Director_Mode3Setup, (void *)a_73FE90, "046 sub_832650" },
		{ game::MAG_046_RelocateActorData, (void *)a_740210, "046 sub_832980" },
		{ game::MAG_046_ActorMotion, (void *)a_740910, "046 sub_833080" },
		{ game::MAG_046_DampValue, (void *)a_740F70, "046 sub_8336E0" },
		{ game::MAG_046_DampVector, (void *)a_740FC0, "046 sub_833730" },
		{ game::MAG_046_Instance_AdvanceTexFrame, (void *)a_741050, "046 sub_8337C0" },
		{ game::MAG_046_Instance_BuildTransform, (void *)a_741120, "046 sub_833890" },
		{ game::MAG_046_Particle_LifeCountdown, (void *)a_741B60, "046 sub_8342D0" },
		{ game::MAG_046_RandRange, (void *)a_742290, "046 au_re__rand_45" },
		{ game::MAG_046_RandRangeWide, (void *)a_7422C0, "046 sub_834A30" },
		{ game::MAG_046_RandVec3, (void *)a_742300, "046 sub_834A70" },
		{ game::MAG_046_JitterAngles3, (void *)a_742350, "046 sub_834AC0" },
		{ game::MAG_046_RandAngleJitter, (void *)a_7423A0, "046 au_re__rand_45_0" },
		{ game::MAG_046_ActSys_AllocParticleSlot, (void *)a_7387B0, "046 sub_834B60" },
		{ game::MAG_046_GetWord8Minus1, (void *)a_7424B0, "046 sub_834C20" },
		{ game::MAG_046_Particle_AttachToOwner, (void *)a_7424C0, "046 sub_834C30" },
		{ game::MAG_046_ParticleList_Append, (void *)a_742CA0, "046 sub_835410" },
		{ game::MAG_046_ParticleList_Unlink, (void *)a_742CD0, "046 sub_835440" },
		{ game::MAG_046_Particle_Init, (void *)a_742D00, "046 sub_835470" },
		{ game::MAG_046_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "046 sub_835620" },
		{ game::MAG_046_Particle_PlaceAtPoint, (void *)a_742FF0, "046 sub_835760" },
		{ game::MAG_046_Sigh_ActorSystem_State2_Empty, (void *)a_73A6D0, "046 nullsub_1569" },
		{ game::MAG_046_Sigh_BreathSpawner_State0_Advance, (void *)a_73A0D0, "046 sub_835EF0" },
		{ game::MAG_046_Sigh_StepFlipbook, (void *)a_743C20, "046 sub_836250" },
		{ game::MAG_046_Sigh_BreathPuff_State2_Empty, (void *)a_73A6D0, "046 nullsub_1570" },
		{ game::MAG_046_Sigh_BreathSpawner_State3_Empty, (void *)a_73A6D0, "046 nullsub_1571" },
		{ game::MAG_046_Sigh_Sigh_State4_Empty, (void *)a_73A6D0, "046 nullsub_1572" },
		{ game::MAG_046_Sigh_Emitter_State3_Empty, (void *)a_73A6D0, "046 nullsub_1573" },
		{ game::MAG_046_Sigh_StageWobbleOutTask, (void *)a_73A380, "046 MAG_046_sub_836420" },
		{ game::MAG_046_Sigh_StageWobbleOut_State2_Empty, (void *)a_73A6D0, "046 nullsub_1575" },
		{ game::MAG_046_Sigh_Master_State6_WaitQueuesEmpty, (void *)a_747550, "046 MAG_046_sub_8364F0" },
		{ game::MAG_046_Sigh_Master_State7_RequestBattleSync, (void *)a_747560, "046 MAG_046_sub_836500" },
		{ game::MAG_046_Sigh_Master_State8_WaitBattleSyncDone, (void *)a_747590, "046 MAG_046_sub_836530" },
		{ game::MAG_046_Sigh_Master_State9_Finish, (void *)a_7475A0, "046 MAG_046_sub_836540" },
		{ game::MAG_046_Sigh_Master_State10_Empty, (void *)a_73A6D0, "046 nullsub_1574" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_046_Sigh_MasterTask, (void *)sg_831CB0, "046 sg_831CB0" },
		{ game::MAG_046_Sigh_MasterReserveActorPools, (void *)sg_831E30, "046 sg_831E30" },
		{ game::MAG_046_Sigh_ClearActorPools, (void *)sg_831E80, "046 sg_831E80" },
		{ game::MAG_046_Sigh_StageWobbleRamp, (void *)sg_831F60, "046 sg_831F60" },
		{ game::MAG_046_Sigh_EmitterTask, (void *)sg_831FE0, "046 sg_831FE0" },
		{ game::MAG_046_Sigh_EmitterSpawnActorSystemAndSigh, (void *)sg_832070, "046 sg_832070" },
		{ game::MAG_046_Sigh_ActorSystem_State0_StartActorSystemAtFrame, (void *)sg_832160, "046 sg_832160" },
		{ game::MAG_046_DirectorSetup, (void *)sg_832190, "046 sg_832190" },
		{ game::MAG_046_ResetTrailHistory, (void *)sg_832340, "046 sg_832340" },
		{ game::MAG_046_Director_CasterRefPoints, (void *)sg_832420, "046 sg_832420" },
		{ game::MAG_046_FillTrailRows, (void *)sg_832580, "046 sg_832580" },
		{ game::MAG_046_Director_TargetRefPoints, (void *)sg_832770, "046 sg_832770" },
		{ game::MAG_046_ActSys_Step, (void *)sg_832BE0, "046 sg_832BE0" },
		{ game::MAG_046_ActSys_DrawPass, (void *)sg_832C70, "046 sg_832C70" },
		{ game::MAG_046_Sigh_PrimActorRenderStub, (void *)sg_832CD0, "046 sg_832CD0" },
		{ game::MAG_046_Sigh_PrimActorPrepareHeader, (void *)sg_832E10, "046 sg_832E10" },
		{ game::MAG_046_ActSys_DrawSpritePass, (void *)sg_832E70, "046 sg_832E70" },
		{ game::MAG_046_Sigh_DrawSpriteActor, (void *)sg_832F30, "046 sg_832F30" },
		{ game::MAG_046_ActSys_UpdatePass, (void *)sg_832FB0, "046 sg_832FB0" },
		{ game::MAG_046_ActorStep, (void *)sg_832FF0, "046 sg_832FF0" },
		{ game::MAG_046_MorphVertexBlock, (void *)sg_834020, "046 sg_834020" },
		{ game::MAG_046_Instance_EndOfLifeTest, (void *)sg_834130, "046 sg_834130" },
		{ game::MAG_046_Instance_SpawnFollowUp, (void *)sg_8341B0, "046 sg_8341B0" },
		{ game::MAG_046_AllocParticle, (void *)sg_8341E0, "046 sg_8341E0" },
		{ game::MAG_046_ActSys_UpdateEmitter, (void *)sg_834280, "046 sg_834280" },
		{ game::MAG_046_ActSys_EmitterSpawnStep, (void *)sg_834300, "046 sg_834300" },
		{ game::MAG_046_ActSys_SpawnParticles, (void *)sg_834380, "046 sg_834380" },
		{ game::MAG_046_Actor_InterpolatePosition, (void *)sg_834D80, "046 sg_834D80" },
		{ game::MAG_046_SpawnSubEffects, (void *)sg_835870, "046 sg_835870" },
		{ game::MAG_046_Director_SpawnGroupEffects, (void *)sg_835900, "046 sg_835900" },
		{ game::MAG_046_Sigh_ActorSystem_State1_StepActorSystem, (void *)sg_835990, "046 sg_835990" },
		{ game::MAG_046_Sigh_SighTask, (void *)sg_8359C0, "046 sg_8359C0" },
		{ game::MAG_046_Sigh_DrawPrimModel, (void *)sg_835B30, "046 sg_835B30" },
		{ game::MAG_046_Sigh_ApplyColorFrame, (void *)sg_835C20, "046 sg_835C20" },
		{ game::MAG_046_Sigh_TextureScrollBlit, (void *)sg_835C70, "046 sg_835C70" },
		{ game::MAG_046_Sigh_SighSetup, (void *)sg_835CF0, "046 sg_835CF0" },
		{ game::MAG_046_Sigh_SighSpawnBreath, (void *)sg_835E30, "046 sg_835E30" },
		{ game::MAG_046_Sigh_BreathSpawnerTask, (void *)sg_835E90, "046 sg_835E90" },
		{ game::MAG_046_Sigh_BreathSpawnerWait, (void *)sg_835F00, "046 sg_835F00" },
		{ game::MAG_046_Sigh_BreathSpawnerBurst, (void *)sg_835F20, "046 sg_835F20" },
		{ game::MAG_046_Sigh_BreathPuffTask, (void *)sg_836090, "046 sg_836090" },
		{ game::MAG_046_Sigh_DrawBreathPuff, (void *)sg_8360F0, "046 sg_8360F0" },
		{ game::MAG_046_Sigh_BreathPuffInit, (void *)sg_836160, "046 sg_836160" },
		{ game::MAG_046_Sigh_BreathPuffMove, (void *)sg_8361C0, "046 sg_8361C0" },
		{ game::MAG_046_Sigh_SighWaitFade, (void *)sg_8362A0, "046 sg_8362A0" },
		{ game::MAG_046_Sigh_SighFade, (void *)sg_8362C0, "046 sg_8362C0" },
		{ game::MAG_046_Sigh_EmitterReleaseMaster, (void *)sg_836310, "046 sg_836310" },
		{ game::MAG_046_Sigh_EmitterApplyDamage, (void *)sg_836330, "046 sg_836330" },
		{ game::MAG_046_Sigh_MasterNextAction, (void *)sg_8363B0, "046 sg_8363B0" },
		{ game::MAG_046_Sigh_MasterWaitEnd, (void *)sg_8363F0, "046 sg_8363F0" },
		{ game::MAG_046_Sigh_StageWobbleOut_State0_SetAmplitude400, (void *)sg_836480, "046 sg_836480" },
		{ game::MAG_046_Sigh_StageWobbleOut_State1_FadeOut, (void *)sg_8364A0, "046 sg_8364A0" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag046_sigh()
	{
		act::register_module(46);
		for (const act::sigh::ModPort *p = act::sigh::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(46, p->addr, p->port, p->name);
		for (const act::sigh::ModPort *p = act::sigh::PORTS; p->addr; p++)
			act::register_module_port(46, p->addr, p->port, p->name);
	}
}

