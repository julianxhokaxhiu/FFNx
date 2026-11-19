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

// Effect 41: Dribble: a copy of

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
namespace dribble
{
	static const Mod MOD_041 = { "dribble", 41, game::MAG_041_DRIBBLE_FL, game::MAG_040_SCAN_FL,
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::unk_268F1B8, {}, game::MAG_041_ParticleCursor, {}, {}, {}, {}, {}, game::MAG_041_ActorSystemQueue, {}, game::MAG_041_EmitterQueue, {}, {}, {}, {}, {}, game::unk_2698A8E, game::MAG_041_ParticlePoolPtr, {}, game::unk_2698A9E, {}, game::g_Dribble_ScratchSP, {}, game::g_Dribble_ActorState },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_041_DirectorSetup, game::MAG_041_Dribble_ActorSystem_Step, game::MAG_041_ActSys_DrawPass, game::MAG_041_ActSys_DrawActorModel, {}, game::MAG_041_MeshEmitF3, game::MAG_041_MeshEmitF4, game::MAG_041_RenderPolyFT3List, game::MAG_041_RenderPolyFT4List, game::MAG_041_PrimList_G3, game::MAG_041_PrimList_G4, game::MAG_041_PrimList_GT3, game::MAG_041_PrimList_GT4, game::MAG_041_ActSys_DrawSpritePass, game::MAG_041_ActSys_UpdatePass, game::MAG_041_ActorStep, game::MAG_041_ActSys_UpdateEmitter, game::MAG_041_Particle_LifeCountdown, game::MAG_041_ActSys_EmitterSpawnStep, game::MAG_041_ActSys_SpawnParticles, game::MAG_041_RandVec3, game::MAG_041_JitterAngles3, game::MAG_041_ActSys_AllocParticleSlot, game::MAG_041_GetWord8Minus1, game::MAG_041_Particle_Init, game::MAG_041_SpawnSubEffects, game::MAG_041_Director_SpawnGroupEffects, {}, {}, {}, {}, {}, game::MAG_041_Dribble_EmitterTask, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_041_ResetTrailHistory, game::MAG_041_Director_CasterRefPoints, game::MAG_041_FillTrailRows, game::MAG_041_Director_Mode3Setup, game::MAG_041_Director_TargetRefPoints, game::MAG_041_RelocateActorData, game::MAG_041_ActSys_DrawActorSprite, game::MAG_041_ActorMotion, game::MAG_041_DampValue, game::MAG_041_DampVector, game::MAG_041_Instance_AdvanceTexFrame, game::MAG_041_Instance_BuildTransform, game::MAG_041_MorphVertexBlock, game::MAG_041_Instance_EndOfLifeTest, game::MAG_041_Instance_SpawnFollowUp, game::MAG_041_AllocParticle, game::MAG_041_RandRange, game::MAG_041_RandRangeWide, game::MAG_041_RandAngleJitter, game::MAG_041_Particle_AttachToOwner, game::MAG_041_Actor_InterpolatePosition, game::MAG_041_ParticleList_Append, game::MAG_041_ParticleList_Unlink, game::MAG_041_Particle_PlaceAtAnchorSet, game::MAG_041_Particle_PlaceAtPoint, {}, game::MAG_041_Dribble_ActorSystem_Step, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::g_Dribble_PacketCursor;
	static constexpr game::Id ACTOR_STATE = game::g_Dribble_ActorState;
	static constexpr game::Id SCRATCH_SP = game::g_Dribble_ScratchSP;
	static constexpr game::Id BLK = game::g_Dribble_BallBlock;
	static constexpr game::Id Q_EMITTER = game::MAG_041_EmitterQueue, Q_ACTORS = game::MAG_041_ActorSystemQueue, Q_TASKS = game::MAG_041_DribbleQueue;
	static constexpr game::Id ORIG_Master = game::MAG_041_Dribble_MasterTask;
	static constexpr game::Id ORIG_Emitter = game::MAG_041_Dribble_EmitterTask;
	static constexpr game::Id ORIG_ActorSystem = game::MAG_041_Dribble_ActorSystemTask;
	static constexpr game::Id ORIG_Spawner = game::MAG_041_Dribble_SpawnerTask;
	static constexpr game::Id ORIG_Dribble = game::MAG_041_Dribble_BallTask;
	static constexpr game::Id ORIG_DustSpawner = game::MAG_041_Dribble_DustSpawnerTask;
	static constexpr game::Id ORIG_Dust = game::MAG_041_Dribble_DustTask;
	static const void *SOUND_Grab() { return (const void *)game_addr(game::unk_15CA5A8); }
	static const void *SOUND_Land() { return (const void *)game_addr(game::unk_15CA5AC); }
	static constexpr game::Id ACTOR_DATA = game::unk_15CD8B0;
	static constexpr game::Id RADIUS_TABLE = game::Dribble_BallRadiusTable;
	static constexpr game::Id DUST_FLIPBOOK = game::unk_15CA5B0;

	namespace gx
	{
		static inline uint32_t sub_45E0B0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadIR123_u16))(a1); }
		static inline uint32_t sub_45E9D0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPF))(); }
		static inline uint32_t sub_45EBF0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPL))(); }
		static inline uint32_t sub_45DEA0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::sub_45DEA0))(a1); }
		static inline uint32_t GTE_SetLightMatrix(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_SetLightMatrix))(a1); }
		static inline uint32_t GTE_SetBackgroundVector(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetBackgroundVector))(a1, a2, a3); }
		static inline uint32_t sub_45E160(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSXY012))(a1, a2, a3); }
		static inline uint32_t sub_45E220(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadSZ3))(a1); }
		static inline uint32_t GTE_MVMVA_ColorV0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_MVMVA_ColorV0))(); }
		static inline uint32_t GTE_MVMVA_LightV0_Bk() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
		static inline uint32_t QueueChainTransformation(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::QueueChainTransformation))(a1, a2); }
		static inline uint32_t sub_56BDE0(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::VectorNormalS16))(a1, a2); }
		static inline uint32_t TransformCameraByShadowRotation(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::TransformCameraByShadowRotation))(a1, a2, a3); }
		static inline uint32_t InitEffectSequenceFromData(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::InitEffectSequenceFromData))(a1, a2, a3, a4); }
		static inline uint32_t sub_8DDB90(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::sub_8DDB90))(a1, a2, a3); }
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

	uint32_t __cdecl db_8466F0(uint32_t a1);
	uint32_t __cdecl db_846850(uint32_t a1);
	uint32_t __cdecl db_846890(void);
	uint32_t __cdecl db_846900(uint32_t a1);
	uint32_t __cdecl db_846990(uint32_t a1);
	uint32_t __cdecl db_846A80(uint32_t a1);
	uint32_t __cdecl db_846AB0(uint32_t a1);
	uint32_t __cdecl db_846C60(uint32_t a1);
	uint32_t __cdecl db_846D40(void);
	uint32_t __cdecl db_846EA0(void);
	uint32_t __cdecl db_847090(void);
	uint32_t __cdecl db_847500(uint32_t a1);
	uint32_t __cdecl db_8476F0(void);
	uint32_t __cdecl db_847750(uint32_t a1, uint32_t a2);
	uint32_t __cdecl db_8479D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl db_847BE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl db_848340(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl db_848570(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl db_848830(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl db_848A90(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl db_848D70(void);
	uint32_t __cdecl db_848E30(uint32_t a1);
	uint32_t __cdecl db_848EB0(void);
	uint32_t __cdecl db_848EF0(uint32_t a1);
	uint32_t __cdecl db_849F20(uint32_t a1, uint32_t a2);
	uint32_t __cdecl db_84A030(uint32_t a1, uint32_t a2);
	uint32_t __cdecl db_84A0B0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl db_84A0E0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl db_84A180(uint32_t a1);
	uint32_t __cdecl db_84A200(uint32_t a1);
	uint32_t __cdecl db_84A280(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl db_84AA60(uint32_t a1, uint32_t a2);
	uint32_t __cdecl db_84AC80(uint32_t a1);
	uint32_t __cdecl db_84B770(uint32_t a1, uint32_t a2);
	uint32_t __cdecl db_84B800(void);
	uint32_t __cdecl db_84B890(uint32_t a1);
	uint32_t __cdecl db_84B920(uint32_t a1);
	uint32_t __cdecl db_84B950(uint32_t a1);
	uint32_t __cdecl db_84BA60(uint32_t a1);
	uint32_t __cdecl db_84BB70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl db_84C220(uint32_t a1);
	uint32_t __cdecl db_84C2D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl db_84C350(uint32_t a1, uint32_t a2);
	uint32_t __cdecl db_84C3A0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl db_84C460(uint32_t a1, uint32_t a2);
	uint32_t __cdecl db_84C610(uint32_t a1);
	uint32_t __cdecl db_84C650(uint32_t a1);
	uint32_t __cdecl db_84C6B0(uint32_t a1);
	uint32_t __cdecl db_84C6F0(uint32_t a1);
	uint32_t __cdecl db_84C7A0(uint32_t a1);
	uint32_t __cdecl db_84C940(uint32_t a1);
	uint32_t __cdecl db_84CA70(uint32_t a1);
	uint32_t __cdecl db_84CB00(uint32_t a1);
	uint32_t __cdecl db_84CB70(uint32_t a1);
	uint32_t __cdecl db_84CB90(uint32_t a1);
	uint32_t __cdecl db_84CCE0(uint32_t a1);
	uint32_t __cdecl db_84CD20(uint32_t a1);
	uint32_t __cdecl db_84CDE0(uint32_t a1);
	uint32_t __cdecl db_84CE70(uint32_t a1);
	uint32_t __cdecl db_84CEF0(uint32_t a1);
	uint32_t __cdecl db_84CF10(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace dribble
{
	uint32_t __cdecl db_8466F0(uint32_t a1)
	{
		g_mod = &MOD_041;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_268F1B8)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_041_Dribble_Master_State0_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_041_Dribble_Master_State1_Advance);
		states[2] = game_addr(game::MAG_041_Dribble_Master_AllocActorPools);
		states[3] = game_addr(game::MAG_041_Dribble_Master_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_041_Dribble_Master_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_041_Dribble_Master_State5_Advance);
		states[6] = game_addr(game::MAG_041_Dribble_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_041_Dribble_Master_State7_Advance);
		states[8] = game_addr(game::MAG_041_Dribble_Master_State8_Advance);
		states[9] = game_addr(game::MAG_041_Dribble_Master_State9_Finish);
		states[10] = game_addr(game::MAG_041_Dribble_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_2698A5C));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_26903E4));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_268F1B4)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_2698A58));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_26903E0));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_268F1B4)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_2698A9E)) = 0;
		MEM<uint16_t>(game_addr(game::unk_2698A8E)) = 0;
		static constexpr game::Id queues[3] = { Q_EMITTER, Q_ACTORS, Q_TASKS };
		for (int i = 0; i < 3; i++)
			U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(queues[i])));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl db_846890(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_041_ParticlePoolPtr)), 0x870);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_041_ActorPoolPtr)), 0x8340);
		MEM<uint16_t>(game_addr(game::MAG_041_ParticleCursor)) = 0;
		MEM<uint16_t>(game_addr(game::MAG_041_ActorCursor)) = 0;
		MEM<uint16_t>(game_addr(game::unk_2698A8C)) = 0;
		MEM<uint16_t>(game_addr(game::unk_267B860)) = 0;
		return 0;
	}

	uint32_t __cdecl db_846A80(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			db_846AB0(a1);
			db_847500(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl db_846AB0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t data = U32(node, 0x30);
		uint16_t w29a = U16(node, 0x29A);
		uint16_t w29c = U16(node, 0x29C);
		uint32_t st = node + 0x34;
		const uint32_t root = U32(node, 0x10);
		MEM<uint32_t>(game_addr(game::g_Dribble_ActorState)) = st;
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
			st = MEM<uint32_t>(game_addr(game::g_Dribble_ActorState));
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
		db_846D40();
		st = MEM<uint32_t>(game_addr(game::g_Dribble_ActorState));
		if (U16(st, 0x24) == 4)
			callp(game_addr(game::MAG_041_ResetTrailHistory), node);
		db_847090();
		st = MEM<uint32_t>(game_addr(game::g_Dribble_ActorState));
		if (U16(st, 0x24) == 1)
		{
			callp(game_addr(game::MAG_041_FillTrailRows), node);
			st = MEM<uint32_t>(game_addr(game::g_Dribble_ActorState));
		}
		if (U16(st, 0x24) == 3)
		{
			a_73FE90(node);
			st = MEM<uint32_t>(game_addr(game::g_Dribble_ActorState));
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
		st = MEM<uint32_t>(game_addr(game::g_Dribble_ActorState));
		U32(st, 0) = dist;
		return 0;
	}

	uint32_t __cdecl db_846C60(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::g_Dribble_ActorState));
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

	uint32_t __cdecl db_846D40(void)
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

	uint32_t __cdecl db_846EA0(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::g_Dribble_ActorState));
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

	uint32_t __cdecl db_847090(void)
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

	uint32_t __cdecl db_8476F0(void)
	{
		uint32_t act = U32(U32(game_addr(game::g_Dribble_ActorState), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::g_Dribble_ActorState), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					db_847750(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl db_847750(uint32_t a1, uint32_t a2)
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

	uint32_t __cdecl db_8479D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl db_847BE0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl db_848340(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl db_848570(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl db_848830(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl db_848A90(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl db_848D70(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::g_Dribble_ActorState)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::g_Dribble_ActorState));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						db_848E30(act);
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
							db_848E30(act);
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

	uint32_t __cdecl db_848E30(uint32_t a1)
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

	uint32_t __cdecl db_848EB0(void)
	{
		uint32_t act = U32(U32(game_addr(game::g_Dribble_ActorState), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				db_84A180(act);
			else if (type == 1)
				db_848EF0(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl db_848EF0(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::g_Dribble_ActorState));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (db_84A030(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::g_Dribble_ActorState));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl db_849F20(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::g_Dribble_ActorState), 0);
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

	uint32_t __cdecl db_84A030(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				db_84A0B0(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			db_84A0B0(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			db_84A0B0(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl db_84A0B0(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return db_84A0E0(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl db_84A0E0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_041_ParticlePoolPtr));
		int32_t idx = MEM<int16_t>(game_addr(game::MAG_041_ParticleCursor));
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
			MEM<uint16_t>(game_addr(game::MAG_041_ParticleCursor)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::MAG_041_ParticleCursor)) = 0;
		return slot;
	}

	uint32_t __cdecl db_84A180(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_041_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_041_Actor_InterpolatePosition), a1);
		db_84A200(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl db_84A200(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::g_Dribble_ActorState), 0);
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
				db_84A280(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			db_84A280(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl db_84A280(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::g_Dribble_ScratchSP), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::g_Dribble_ScratchSP), 0) = sp;
		uint32_t node = db_84AA60(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::g_Dribble_ActorState), 0);
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
		db_84B770(node, desc);
		U32(game_addr(game::g_Dribble_ScratchSP), 0) = U32(game_addr(game::g_Dribble_ScratchSP), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl db_84AA60(uint32_t a1, uint32_t a2)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_041_ActorPoolPtr));
		int32_t i = MEM<int16_t>(game_addr(game::MAG_041_ActorCursor));
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
			MEM<uint16_t>(game_addr(game::MAG_041_ActorCursor)) = 0;
		else
			MEM<uint16_t>(game_addr(game::MAG_041_ActorCursor)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl db_84AC80(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::g_Dribble_ActorState), 0);
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
				uint32_t g = U32(game_addr(game::g_Dribble_ActorState), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::g_Dribble_ActorState), 0);
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

	uint32_t __cdecl db_84B770(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			db_84A0E0(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			db_84A0E0(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			db_84A0E0(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			db_84A0E0(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl db_84B800(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::g_Dribble_ActorState));
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
					db_84A0E0(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::g_Dribble_ActorState));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				db_84A0E0(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::g_Dribble_ActorState));
			}
		}
		return 0;
	}

	uint32_t __cdecl db_84B890(uint32_t a1)
	{
		if (db_847500(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl db_84CB00(uint32_t a1)
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

	uint32_t __cdecl db_846850(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::MAG_041_FileArenaCursor));
		MEM<uint32_t>(game_addr(game::MAG_041_ParticlePoolPtr)) = p;
		p += 0x870;
		MEM<uint32_t>(game_addr(game::MAG_041_ActorPoolPtr)) = p;
		p += 0x8340;
		MEM<uint32_t>(game_addr(game::MAG_041_FileArenaCursor)) = p;
		db_846890();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl db_846900(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_041_Dribble_Emitter_SpawnActorsAndDribble);
		states[1] = game_addr(game::MAG_041_Dribble_Emitter_ReleaseMaster);
		states[2] = game_addr(game::MAG_041_Dribble_Emitter_ApplyDamage);
		states[3] = game_addr(game::MAG_041_Dribble_Emitter_State3_Empty);
		x::MAG_001_CURE_Emitter_UpdatePos(node);
		x::MAG_001_CURE_Emitter_ComputeModelBounds(node);
		callp(states[S8(node, 0x29)], node);
		if (U16(node, 0x24) == 0x11)
			x::BdPlaySE(P(SOUND_Grab()), 0, 0x80);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl db_846990(uint32_t a1)
	{
		a_73C100(a1, game_addr(ORIG_ActorSystem), game_addr(ACTOR_DATA), 0xF, 0x2D, 0);
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TASKS), game_addr(ORIG_Spawner), 0xDC, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl db_84CEF0(uint32_t a1)
	{
		if (U16(a1, 0x24) == 0x74)
		{
			U8(U32(a1, 0x10), 0x63) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl db_84CF10(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 0x74)
			return 0;
		const uint32_t act = U32(U32(node, 0xC), 4) + (uint32_t)(S8(node, 0x2A) * 20);
		x::ApplyActionResultToTarget(U32(act, 8) + (uint32_t)(S8(node, 0x2B) * 24));
		const uint8_t st = U8(node, 0x29);
		U8(node, 0x26) |= 1;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl db_847500(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		MEM<uint32_t>(game_addr(ACTOR_STATE)) = sys;
		int16_t pos[4] = { 0, 0, 0, 0 };
		const uint32_t pp = P(pos);
		if (U16(sys, 0x24) != 2)
		{
			x::GetEffectSpawnPosition(game_addr(game::BattleEntitySlotData) + (uint32_t)((int32_t)S16(sys, 0x1E) * 0x9C), 0x18, 0, pp);
			sys = MEM<uint32_t>(game_addr(ACTOR_STATE));
		}
		S16(pp, 4) = (int16_t)(U16(pp, 4) + 0x100);
		S32(sys, 0x114) = shl32(S16(pp, 0), 16);
		S32(sys, 0x118) = shl32(S16(pp, 2), 16);
		S32(sys, 0x11C) = shl32(S16(pp, 4), 16);
		if (U16(sys, 0x24) != 2)
		{
			x::GetEffectSpawnPosition(game_addr(game::BattleEntitySlotData) + (uint32_t)((int32_t)S16(sys, 0x1E) * 0x9C), 0x15, 0, pp);
			sys = MEM<uint32_t>(game_addr(ACTOR_STATE));
		}
		S16(pp, 4) = (int16_t)(U16(pp, 4) + 0x100);
		S32(sys, 0x154) = shl32(S16(pp, 0), 16);
		S32(sys, 0x158) = shl32(S16(pp, 2), 16);
		S32(sys, 0x15C) = shl32(S16(pp, 4), 16);
		S32(sys, 0xD4) = add32(S32(sys, 0x114), S32(sys, 0x154)) / 2;
		S32(sys, 0xD8) = add32(S32(sys, 0x118), S32(sys, 0x158)) / 2;
		S32(sys, 0xDC) = add32(S32(sys, 0x11C), S32(sys, 0x15C)) / 2;
		if (U16(sys, 0x24) != 2)
		{
			x::GetEffectSpawnPosition(game_addr(game::BattleEntitySlotData) + (uint32_t)((int32_t)S16(sys, 0x1E) * 0x9C), 0x18, 0, pp);
			sys = MEM<uint32_t>(game_addr(ACTOR_STATE));
		}
		S16(pp, 4) = (int16_t)(U16(pp, 4) + 0x100);
		S32(sys, 0x204) = shl32(S16(pp, 0), 16);
		S32(sys, 0x208) = 0;
		S32(sys, 0x20C) = shl32(S16(pp, 4), 16);
		const uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			db_84B800();
			db_848EB0();
			db_848D70();
			db_8476F0();
			sys = MEM<uint32_t>(game_addr(ACTOR_STATE));
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
		const uint16_t c14 = U16(sys, 0x14);
		const uint16_t c16 = U16(sys, 0x16);
		MEM<uint16_t>(game_addr(game::unk_2698A9E)) = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_2698A9E)) + c14);
		MEM<uint16_t>(game_addr(game::unk_2698A8E)) = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_2698A8E)) + c16);
		return done;
	}

	uint32_t __cdecl db_84B920(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 8)
		{
			x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TASKS), game_addr(ORIG_Dribble), 0xDC, a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl db_84B950(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[11];
		states[0] = game_addr(game::MAG_041_Dribble_Ball_Setup);
		states[1] = game_addr(game::MAG_041_Dribble_Ball_Grab);
		states[2] = game_addr(game::MAG_041_Dribble_Ball_Morph);
		states[3] = game_addr(game::MAG_041_Dribble_Ball_Hold);
		states[4] = game_addr(game::MAG_041_Dribble_Ball_Bounce);
		states[5] = game_addr(game::MAG_041_Dribble_Ball_Flight);
		states[6] = game_addr(game::MAG_041_Dribble_Ball_GiveBackTarget);
		states[7] = game_addr(game::MAG_041_Dribble_Target_TurnAndWalkBack);
		states[8] = game_addr(game::MAG_041_Dribble_Target_WalkBack);
		states[9] = game_addr(game::MAG_041_Dribble_Ball_End);
		states[10] = game_addr(game::MAG_041_Dribble_Ball_State10_Empty);
		callp(states[S8(node, 0x29)], node);
		if ((U8(node, 0x26) & 8) != 0)
		{
			const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2D) * 0x9C;
			const int16_t px = (int16_t)(MEM<uint16_t>(game_addr(BLK) + 0x7C) - MEM<uint16_t>(game_addr(BLK) + 0x74));
			S16(ent, 0x1C) = px;
			S32(ent, 0x54) = px;
			const int16_t py = (int16_t)(MEM<uint16_t>(game_addr(BLK) + 0x7E) - MEM<uint16_t>(game_addr(BLK) + 0x76));
			S16(ent, 0x1E) = py;
			S32(ent, 0x58) = py;
			const int16_t pz = (int16_t)(MEM<uint16_t>(game_addr(BLK) + 0x80) - MEM<uint16_t>(game_addr(BLK) + 0x78));
			S16(ent, 0x20) = pz;
			S32(ent, 0x5C) = pz;
			db_84BA60(game_addr(BLK));
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl db_84BA60(uint32_t a1)
	{
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)((int32_t)S16(a1, 0xB0) * 0x9C);
		ff8fx::copy<(0x20)>((void *)(a1 + 0x20), (const void *)(ent + 0x40));
		const uint32_t pose = ent + 0x60;
		x::BattleModel_BuildBoneMatricesFromPose(pose);
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(a1 + 0x20);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(a1 + 0x40);
		x::GTE_LoadIRFromMatrixColumn(a1 + 0x22);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(a1 + 0x42);
		x::GTE_LoadIRFromMatrixColumn(a1 + 0x24);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(a1 + 0x44);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(a1 + 0x34);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(a1 + 0x54);
		uint32_t cp = U32(a1, 0x6C);
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4040;
		const uint32_t r = x::sub_5088A0(ent, ot, 0x10, U32(cp, 0));
		cp = U32(a1, 0x6C);
		const uint32_t ot44 = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U32(cp, 0) = r;
		db_84BB70(U32(ent, 0x64), ot44, 4, a1);
		const uint32_t wpn = U32(ent, 0x78);
		if (wpn != 0)
			db_84BB70(U32(wpn, 4), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, a1);
		x::BattleModel_BuildBoneMatricesFromPose(pose);
		return 0;
	}

	uint32_t __cdecl db_84BB70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t blk = a4;
		const uint32_t verts = U32(U32(blk, 0x64), 4);
		const uint32_t ent = U32(blk, 0x68);
		uint32_t cursor = U32(U32(blk, 0x6C), 0);
		const uint32_t bones = U32(a1, 0) + 0x10;
		uint32_t tab = U32(a1, 4);
		const int32_t count = S32(tab, 0);
		const uint32_t w = x::Field_Alloc(0xA8);
		tab += 4;
		U16(w, 0x9E) = U16(blk, 0xA8);
		U16(w, 0x9C) = (uint16_t)(0x1000 - U16(blk, 0xA8));
		U32(w, 0x5C) = U32(blk, 0x74);
		U32(w, 0x60) = U32(blk, 0x78);
		U16(w, 0xA0) = U16(blk, 0xAA);
		U16(w, 0xA4) = U16(blk, 0xAE);
		U32(w, 0x98) = U32(ent, 0x7C);
		const uint32_t rgb = U32(ent, 0x28) & 0xFFFFFF;
		ff8fx::copy<(0x20)>((void *)w, (const void *)blk);
		U16(w, 0xA2) = U16(blk, 0xAC);
		U32(w, 0x90) = rgb | 0x24000000;
		U32(w, 0x94) = rgb | 0x2C000000;
		if (U16(blk, 0x9C) != 0)
			x::sub_8DD7E0(w, (uint32_t)(int32_t)S16(blk, 0x9C));
		if (U16(blk, 0xA0) != 0)
			x::sub_8DD960(w, (uint32_t)(int32_t)S16(blk, 0xA0));
		if (U16(blk, 0x9E) != 0)
			x::MAG_022_sub_8DD8A0(w, (uint32_t)(int32_t)S16(blk, 0x9E));
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
					wv += 0x14;
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
					vp += 0x14;
					zp += 0x14;
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
				const uint32_t s2 = U32(verts + i2 * 20, 8);
				const uint32_t s1 = U32(verts + i1 * 20, 8);
				const uint32_t s0 = U32(verts + i0 * 20, 8);
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
				const int32_t z2 = S32(verts + U32(w, 0x88) * 20, 0xC);
				const int32_t z1 = S32(verts + U32(w, 0x84) * 20, 0xC);
				const int32_t z0 = S32(verts + U32(w, 0x80) * 20, 0xC);
				const int32_t sum = add32(add32(z1, z0), z2);
				int32_t hq = (int32_t)(((int64_t)sum * 0x55555556LL) >> 32);
				hq = add32(hq, (int32_t)((uint32_t)hq >> 31));
				const int32_t key = hq >> (a3 & 31);
				U32(w, 0x78) = (uint32_t)key;
				x::SSIGPU_InsertPrimDepthKeys(a2 + (uint32_t)key * 4, pk, (uint32_t)z0, (uint32_t)z1, (uint32_t)z2, 0);
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
					const uint32_t s2 = U32(verts + i2 * 20, 8);
					const uint32_t s1 = U32(verts + i1 * 20, 8);
					const uint32_t s0 = U32(verts + i0 * 20, 8);
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
					U32(pk, 0) = 0x9000000;
					const uint32_t v3 = verts + i3 * 20;
					const uint32_t s3 = U32(v3, 8);
					U32(w, 0x30) = s3;
					U32(pk, 0x20) = s3;
					U32(pk, 0x18) = U32(w, 0x2C);
					U32(pk, 0xC) = U32(f, 4);
					U32(pk, 0x14) = U32(f, 8);
					U16(pk, 0x1C) = U16(f, 0xC);
					U16(pk, 0x24) = U16(f, 0xE);
					U32(pk, 4) = U32(w, 0x94);
					if (U8(f, 0xB) & 2)
						U8(pk, 7) |= 2;
					const int32_t z3 = S32(v3, 0xC);
					const int32_t z2 = S32(verts + U32(w, 0x88) * 20, 0xC);
					const int32_t z1 = S32(verts + U32(w, 0x84) * 20, 0xC);
					const int32_t z0 = S32(verts + U32(w, 0x80) * 20, 0xC);
					const int32_t sum = add32(add32(add32(z0, z3), z1), z2);
					const int32_t key = sum >> ((a3 + 2) & 31);
					U32(w, 0x78) = (uint32_t)key;
					x::SSIGPU_InsertPrimDepthKeys(a2 + (uint32_t)key * 4, pk, (uint32_t)z0, (uint32_t)z1, (uint32_t)z2, (uint32_t)z3);
					pk += 0x28;
				}
			}
			cursor = pk;
		}
		U32(U32(blk, 0x6C), 0) = cursor;
		x::Field_Free(0xA8);
		return 0;
	}

	uint32_t __cdecl db_84C220(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t slot = U8(node, 0x2D);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + slot * 0x9C;
		db_84C2D0(game_addr(BLK), game_addr(game::unk_267B920), game_addr(game::unk_2698A60), game_addr(game::unk_2690BE8), ent, slot);
		U8(ent, 0) |= 4;
		const uint32_t p0 = U32(ent, 0x1C);
		const uint32_t p1 = U32(ent, 0x20);
		U8(node, 0x26) |= 8;
		MEM<uint32_t>(game_addr(game::unk_267B8F4)) = p0;
		MEM<uint32_t>(game_addr(game::unk_267B8F8)) = p1;
		if ((uint8_t)x::CrtRand() == 0)
		{
			MEM<uint16_t>(game_addr(BLK) + 0xB4) = 1;
			MEM<uint16_t>(game_addr(BLK) + 0x9E) = 0x800;
		}
		db_84C3A0(node, game_addr(BLK));
		db_84C350(node, game_addr(BLK));
		db_84C460(node, game_addr(BLK));
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl db_84C2D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		x::MAG_007_sub_8DCC00(a1, 0xB8);
		U32(a1, 0x68) = a5;
		U32(a1, 0x60) = a2;
		U32(a1, 0x6C) = game_addr(game::BATTLE_PACKET_WRITE_CURSOR);
		U32(a1, 0x70) = game_addr(game::BATTLE_PACKET_WRITE_CURSOR);
		U16(a1, 0xB0) = (uint16_t)a6;
		U32(a3, 4) = a4;
		U32(a1, 0x64) = a3;
		U16(a3, 0x14) = 0;
		U16(a3, 0x16) = 0;
		U16(a3, 0x18) = 0x140;
		U16(a3, 0x1A) = 0;
		U8(a3, 0x1C) = 0x80;
		U8(a3, 0x1D) = 0x80;
		U8(a3, 0x1E) = 0x80;
		U32(a3, 0x20) = 0xFFFFFFFF;
		U16(a3, 0x24) = 0;
		return a1;
	}

	uint32_t __cdecl db_84C350(uint32_t a1, uint32_t a2)
	{
		const int16_t t = S16(a1, 0x24);
		if (t > 0x38)
			return 0;
		const uint16_t ry = MEM<uint16_t>(game_addr(RADIUS_TABLE) + (uint32_t)((int32_t)t * 6));
		const uint16_t rxz = (uint16_t)(0x200 - ry);
		U16(a2, 0xAC) = ry;
		U16(a2, 0xAE) = rxz;
		U16(a2, 0xAA) = rxz;
		return 0;
	}

	uint32_t __cdecl db_84C3A0(uint32_t a1, uint32_t a2)
	{
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2D) * 0x9C;
		const uint32_t save1c = U32(ent, 0x1C);
		uint32_t mtx[8];
		ff8fx::copy<(0x20)>(mtx, (const void *)(ent + 0x40));
		const uint32_t save20 = U32(ent, 0x20);
		U16(ent, 0x1C) = 0;
		U16(ent, 0x1E) = 0;
		U16(ent, 0x20) = 0;
		x::MAG_022_sub_8DD770(ent + 0x40);
		const uint32_t pv = a2 + 0x74;
		x::GetEffectSpawnPosition(ent, U16(a2, 0xB4) == 1 ? 0xF0 : 0xF1, 0, pv);
		U16(pv, 0) = (uint16_t)(U16(pv, 0) - U16(ent, 0x1C));
		U16(a2, 0x76) = (uint16_t)(U16(a2, 0x76) - U16(ent, 0x1E));
		U16(a2, 0x78) = (uint16_t)(U16(a2, 0x78) - U16(ent, 0x20));
		ff8fx::copy<(0x20)>((void *)(ent + 0x40), mtx);
		U32(ent, 0x1C) = save1c;
		U32(ent, 0x20) = save20;
		return 0;
	}

	uint32_t __cdecl db_84C460(uint32_t a1, uint32_t a2)
	{
		const uint32_t caster = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(U32(a1, 0xC), 0) * 0x9C;
		const uint32_t blk = a2;
		alignas(4) uint8_t loc[0x58];
		ff8fx::fill<(sizeof(loc))>(loc, 0);
		const uint32_t L = P(loc);
		if (S16(a1, 0x24) <= 0x1E)
		{
			x::GetEffectSpawnPosition(caster, 0x18, 0, L + 0x28);
			x::GetEffectSpawnPosition(caster, 0x15, 0, L + 0x20);
			U16(L, 0x18) = (uint16_t)(U16(L, 0x20) - U16(L, 0x28));
			U16(L, 0x1A) = (uint16_t)(U16(L, 0x22) - U16(L, 0x2A));
			U16(L, 0x1C) = (uint16_t)(U16(L, 0x24) - U16(L, 0x2C));
			U16(L, 0x30) = 0;
			U16(L, 0x32) = 0;
			U16(L, 0x34) = 0x1000;
			gx::sub_56BDE0(L + 0x18, L + 0x18);
			gx::sub_8DDB90(blk, L + 0x18, L + 0x30);
		}
		else
			x::MAG_022_sub_8DD770(blk);
		ff8fx::copy<(0x20)>((void *)(L + 0x38), (const void *)blk);
		const uint32_t pos = blk + 0x7C;
		x::GetEffectSpawnPosition(caster, 0x18, 0, pos);
		const uint16_t base = (uint16_t)(U16(blk, 0x76) + 0x400);
		U16(L, 0x10) = 0;
		U16(L, 0x14) = 0;
		const int32_t d = mul32(sub32(S16(blk, 0xAC), (int16_t)base), S16(blk, 0xA8)) / 4096;
		U16(L, 0x12) = (uint16_t)(d + base);
		if (U16(blk, 0xA0) != 0)
			x::sub_8DD960(L + 0x38, (uint32_t)(int32_t)S16(blk, 0xA0));
		if (U16(blk, 0x9C) != 0)
			x::sub_8DD7E0(L + 0x38, (uint32_t)(int32_t)S16(blk, 0x9C));
		if (U16(blk, 0x9E) != 0)
			x::MAG_022_sub_8DD8A0(L + 0x38, (uint32_t)(int32_t)S16(blk, 0x9E));
		x::matrixMultiplyVector(L + 0x38, L + 0x10, L + 0x10);
		U16(pos, 0) = (uint16_t)(U16(pos, 0) + U16(L, 0x10));
		U16(blk, 0x7E) = (uint16_t)(U16(blk, 0x7E) + U16(L, 0x12));
		U16(blk, 0x80) = (uint16_t)(U16(blk, 0x80) + U16(L, 0x14));
		return 0;
	}

	uint32_t __cdecl db_84C610(uint32_t a1)
	{
		db_84C3A0(a1, game_addr(BLK));
		if (S16(a1, 0x24) >= 0x13)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		db_84C350(a1, game_addr(BLK));
		db_84C460(a1, game_addr(BLK));
		return 0;
	}

	uint32_t __cdecl db_84C650(uint32_t a1)
	{
		db_84C3A0(a1, game_addr(BLK));
		const uint16_t m = (uint16_t)(MEM<uint16_t>(game_addr(BLK) + 0xA8) + 0x200);
		MEM<uint16_t>(game_addr(BLK) + 0xA8) = m;
		if ((int16_t)m >= 0x1000)
		{
			MEM<uint16_t>(game_addr(BLK) + 0xA8) = 0x1000;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		db_84C350(a1, game_addr(BLK));
		db_84C460(a1, game_addr(BLK));
		return 0;
	}

	uint32_t __cdecl db_84C6B0(uint32_t a1)
	{
		db_84C3A0(a1, game_addr(BLK));
		db_84C350(a1, game_addr(BLK));
		db_84C460(a1, game_addr(BLK));
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl db_84C6F0(uint32_t a1)
	{
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2D) * 0x9C;
		db_84C3A0(a1, game_addr(BLK));
		if (S16(a1, 0x24) >= 0x33)
		{
			gx::QueueChainTransformation(ent, 6);
			const int32_t f = S16(ent, 0xE);
			MEM<uint16_t>(game_addr(BLK) + 0x96) = 0xFE00;
			const int32_t s = (int32_t)x::computeSin((uint32_t)f);
			MEM<uint16_t>(game_addr(BLK) + 0x94) = (uint16_t)(shl32(s, 7) / 4096);
			const int32_t c = (int32_t)x::computeCosine((uint32_t)(int32_t)S16(ent, 0xE));
			MEM<uint16_t>(game_addr(BLK) + 0xB2) = 0;
			MEM<uint16_t>(game_addr(BLK) + 0x98) = (uint16_t)(shl32(c, 7) / 4096);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		db_84C350(a1, game_addr(BLK));
		db_84C460(a1, game_addr(BLK));
		return 0;
	}

	uint32_t __cdecl db_84C7A0(uint32_t a1)
	{
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2D) * 0x9C;
		db_84C3A0(a1, game_addr(BLK));
		const uint16_t t = MEM<uint16_t>(game_addr(BLK) + 0xB2);
		if ((int16_t)t <= 0x10)
		{
			MEM<uint16_t>(game_addr(BLK) + 0x9C) = (uint16_t)((uint16_t)(t * 0xFF00u) & 0xFFF);
			if (MEM<uint16_t>(game_addr(BLK) + 0xB4) == 1)
				MEM<uint16_t>(game_addr(BLK) + 0x9E) = (uint16_t)((uint16_t)(0x10 - t) << 7);
		}
		const uint16_t m = (uint16_t)(MEM<uint16_t>(game_addr(BLK) + 0xA8) + 0xFF00);
		MEM<uint16_t>(game_addr(BLK) + 0xA8) = m;
		if ((int16_t)m <= 0)
			MEM<uint16_t>(game_addr(BLK) + 0xA8) = 0;
		const uint16_t vx = MEM<uint16_t>(game_addr(BLK) + 0x94);
		const uint16_t vz = MEM<uint16_t>(game_addr(BLK) + 0x98);
		MEM<uint16_t>(game_addr(BLK) + 0x7C) = (uint16_t)(MEM<uint16_t>(game_addr(BLK) + 0x7C) + vx);
		const uint16_t vy = (uint16_t)(MEM<uint16_t>(game_addr(BLK) + 0x96) + 0x30);
		MEM<uint16_t>(game_addr(BLK) + 0x80) = (uint16_t)(MEM<uint16_t>(game_addr(BLK) + 0x80) + vz);
		MEM<uint16_t>(game_addr(BLK) + 0x7E) = (uint16_t)(MEM<uint16_t>(game_addr(BLK) + 0x7E) + vy);
		MEM<uint16_t>(game_addr(BLK) + 0x96) = vy;
		if ((int16_t)vy > 0 && (int16_t)MEM<uint16_t>(game_addr(BLK) + 0x7E) >= (int16_t)MEM<uint16_t>(game_addr(BLK) + 0x76))
		{
			gx::QueueChainTransformation(ent, 6);
			const uint16_t ground = MEM<uint16_t>(game_addr(BLK) + 0x76);
			MEM<uint16_t>(game_addr(BLK) + 0x7E) = ground;
			const uint32_t n = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TASKS), game_addr(ORIG_DustSpawner), 0xDC, a1);
			U32(n, 0x1C) = U32(ent, 0x1C);
			U32(n, 0x20) = U32(ent, 0x20);
			U16(n, 0x1E) = 0;
			x::BdPlaySE(P(SOUND_Land()), 1, 0x80);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		MEM<uint16_t>(game_addr(BLK) + 0xB2) = (uint16_t)(MEM<uint16_t>(game_addr(BLK) + 0xB2) + 1);
		db_84C350(a1, game_addr(BLK));
		return 0;
	}

	uint32_t __cdecl db_84CCE0(uint32_t a1)
	{
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2D) * 0x9C;
		U8(a1, 0x26) &= 0xF7;
		U16(ent, 0) &= 0xFFFB;
		U16(ent, 0x1E) = U16(ent, 0x24);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl db_84CD20(uint32_t a1)
	{
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2D) * 0x9C;
		const uint16_t fl = U16(ent, 0);
		MEM<uint32_t>(game_addr(game::unk_267B90C)) = fl;
		U16(ent, 0) = (uint16_t)(fl | 0x1000);
		if (U8(U32(ent, 0x74), 0) == 6)
			return 0;
		gx::QueueChainTransformation(ent, 0x1A);
		const uint16_t facing = (uint16_t)((U16(ent, 0xE) - 0x800) & 0xFFF);
		MEM<uint16_t>(game_addr(BLK) + 0xB2) = 0xA;
		U16(ent, 0xE) = facing;
		MEM<uint16_t>(game_addr(BLK) + 0x94) = (uint16_t)(sub32(MEM<int16_t>(game_addr(game::unk_267B8F4)), S16(ent, 0x1C)) / 10);
		MEM<uint16_t>(game_addr(BLK) + 0x98) = (uint16_t)(sub32(MEM<int16_t>(game_addr(game::unk_267B8F8)), S16(ent, 0x20)) / 10);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl db_84CDE0(uint32_t a1)
	{
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2D) * 0x9C;
		const uint16_t vx = MEM<uint16_t>(game_addr(BLK) + 0x94);
		const uint16_t vz = MEM<uint16_t>(game_addr(BLK) + 0x98);
		U16(ent, 0x1C) = (uint16_t)(U16(ent, 0x1C) + vx);
		U16(ent, 0x20) = (uint16_t)(U16(ent, 0x20) + vz);
		const uint16_t t = (uint16_t)(MEM<uint16_t>(game_addr(BLK) + 0xB2) - 1);
		MEM<uint16_t>(game_addr(BLK) + 0xB2) = t;
		if ((int16_t)t > 0)
			return 0;
		gx::QueueChainTransformation(ent, U8(U32(ent, 0x74), 2));
		U32(ent, 0x1C) = MEM<uint32_t>(game_addr(game::unk_267B8F4));
		U32(ent, 0x20) = MEM<uint32_t>(game_addr(game::unk_267B8F8));
		U16(ent, 0xE) = (uint16_t)((U16(ent, 0xE) - 0x800) & 0xFFF);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl db_84CE70(uint32_t a1)
	{
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2D) * 0x9C;
		U8(a1, 0x26) |= 1;
		const uint16_t d = (uint16_t)((U16(ent, 0) ^ MEM<uint16_t>(game_addr(game::unk_267B90C))) & 0x1000);
		U16(ent, 0) ^= d;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl db_84C940(uint32_t a1)
	{
		const uint32_t n = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_TASKS), game_addr(ORIG_Dust), 0xDC, a1);
		U16(n, 0xD8) = 8;
		uint32_t vel = n + 0x98;
		for (int32_t i = 0; i < S16(n, 0xD8); i++, vel += 8)
		{
			U32(vel, -0x40) = U32(a1, 0x1C);
			U32(vel, -0x3C) = U32(a1, 0x20);
			int32_t ang = shl32(i, 9);
			if (U16(a1, 0x24) != 0)
				ang += 0x100;
			const int32_t r1 = (int32_t)x::CrtRand();
			ang = (int16_t)ang;
			const int32_t d = (int16_t)(r1 & 0x1FF);
			const int32_t s1 = (int32_t)x::computeSin((uint32_t)ang);
			U16(n, 0x1C) = (uint16_t)(U16(n, 0x1C) + (uint16_t)(mul32(s1, d) / 4096));
			const int32_t c1 = (int32_t)x::computeCosine((uint32_t)ang);
			U16(n, 0x20) = (uint16_t)(U16(n, 0x20) + (uint16_t)(mul32(c1, d) / 4096));
			const int32_t r2 = (int32_t)x::CrtRand();
			const int32_t sp = (int16_t)((r2 & 0x7FF) + 0x400);
			const int32_t s2 = (int32_t)x::computeSin((uint32_t)ang);
			U16(vel, 0) = (uint16_t)(mul32(s2, sp) / 4096);
			const int32_t c2 = (int32_t)x::computeCosine((uint32_t)ang);
			U16(vel, 4) = (uint16_t)(mul32(c2, sp) / 4096);
			const int32_t r3 = (int32_t)x::CrtRand();
			U16(vel, 2) = (uint16_t)(-0x80 - (r3 & 0x3FF));
		}
		if (S16(a1, 0x24) >= 1)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl db_84CA70(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_041_Dribble_Dust_Init);
		states[1] = game_addr(game::MAG_041_Dribble_Dust_Move);
		states[2] = game_addr(game::MAG_041_Dribble_Dust_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		for (int32_t i = 0; i < S16(node, 0xD8); i++)
		{
			U32(node, 0x1C) = U32(node, 0x58 + 8 * i);
			U32(node, 0x20) = U32(node, 0x5C + 8 * i);
			db_84CB00(node);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl db_84CB70(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U32(a1, 0x4C) = game_addr(DUST_FLIPBOOK);
		U16(a1, 0x52) = 0xF;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl db_84CB90(uint32_t a1)
	{
		for (int32_t i = 0; i < S16(a1, 0xD8); i++)
		{
			const uint32_t v = a1 + 0x98 + 8 * i;
			for (int k = 0; k < 3; k++)
			{
				const int16_t s = S16(v, 2 * k);
				S16(v, 2 * k) = (int16_t)(s - (int16_t)(shl32(s * 3, 8) / 4096));
			}
			for (int k = 0; k < 3; k++)
				U16(v, 2 * k - 0x40) = (uint16_t)(U16(v, 2 * k - 0x40) + (uint16_t)(S16(v, 2 * k) / 16));
		}
		if (a_743C20(a1) != 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_041_Dribble_Master_State0_Advance, (void *)a_73A0D0, "041 MAG_041_sub_846830" },
		{ game::MAG_041_Dribble_Master_State1_Advance, (void *)a_73A0D0, "041 MAG_041_sub_846840" },
		{ game::MAG_041_Dribble_Master_State3_SpawnEmitter, (void *)a_73A170, "041 MAG_041_sub_8468D0" },
		{ game::MAG_041_SpawnChildTask, (void *)a_73C100, "041 MAG_041_sub_8469D0" },
		{ game::MAG_041_Dribble_ActorSystemTask, (void *)a_73A380, "041 MAG_041_sub_846A20" },
		{ game::MAG_041_Director_Mode3Setup, (void *)a_73FE90, "041 sub_846F70" },
		{ game::MAG_041_RelocateActorData, (void *)a_740210, "041 sub_8472A0" },
		{ game::MAG_041_RenderPrimModel, (void *)a_7355E0, "041 sub_847890" },
		{ game::MAG_041_RenderPolyFT3List, (void *)a_735BB0, "041 sub_847E60" },
		{ game::MAG_041_RenderPolyFT4List, (void *)a_735DF0, "041 sub_8480A0" },
		{ game::MAG_041_ActorMotion, (void *)a_740910, "041 sub_848F80" },
		{ game::MAG_041_DampValue, (void *)a_740F70, "041 sub_8495E0" },
		{ game::MAG_041_DampVector, (void *)a_740FC0, "041 sub_849630" },
		{ game::MAG_041_Instance_AdvanceTexFrame, (void *)a_741050, "041 sub_8496C0" },
		{ game::MAG_041_Instance_BuildTransform, (void *)a_741120, "041 sub_849790" },
		{ game::MAG_041_Particle_LifeCountdown, (void *)a_741B60, "041 sub_84A1D0" },
		{ game::MAG_041_RandRange, (void *)a_742290, "041 au_re__rand_49" },
		{ game::MAG_041_RandRangeWide, (void *)a_7422C0, "041 sub_84A930" },
		{ game::MAG_041_RandVec3, (void *)a_742300, "041 sub_84A970" },
		{ game::MAG_041_JitterAngles3, (void *)a_742350, "041 sub_84A9C0" },
		{ game::MAG_041_RandAngleJitter, (void *)a_7423A0, "041 au_re__rand_49_0" },
		{ game::MAG_041_GetWord8Minus1, (void *)a_7424B0, "041 sub_84AB20" },
		{ game::MAG_041_Particle_AttachToOwner, (void *)a_7424C0, "041 sub_84AB30" },
		{ game::MAG_041_ParticleList_Append, (void *)a_742CA0, "041 sub_84B310" },
		{ game::MAG_041_ParticleList_Unlink, (void *)a_742CD0, "041 sub_84B340" },
		{ game::MAG_041_Particle_Init, (void *)a_742D00, "041 sub_84B370" },
		{ game::MAG_041_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "041 sub_84B520" },
		{ game::MAG_041_Particle_PlaceAtPoint, (void *)a_742FF0, "041 sub_84B660" },
		{ game::MAG_041_Dribble_ActorSystem_State2_Empty, (void *)a_73A6D0, "041 nullsub_1599" },
		{ game::MAG_041_Dribble_SpawnerTask, (void *)a_73A380, "041 MAG_041_sub_84B8C0" },
		{ game::MAG_041_Dribble_DustSpawnerTask, (void *)a_73A380, "041 sub_84C8E0" },
		{ game::MAG_041_StepCounterLimit, (void *)a_743C20, "041 sub_84CC70" },
		{ game::MAG_041_Dribble_Dust_State2_Empty, (void *)a_73A6D0, "041 nullsub_1600" },
		{ game::MAG_041_Dribble_DustSpawner_State1_Finish, (void *)a_7475A0, "041 sub_84CCB0" },
		{ game::MAG_041_Dribble_DustSpawner_State2_Empty, (void *)a_73A6D0, "041 nullsub_1601" },
		{ game::MAG_041_Dribble_Ball_State10_Empty, (void *)a_73A6D0, "041 nullsub_1602" },
		{ game::MAG_041_Dribble_Spawner_State1_Finish, (void *)a_7475A0, "041 sub_84CEC0" },
		{ game::MAG_041_Dribble_Spawner_State2_Empty, (void *)a_73A6D0, "041 nullsub_1603" },
		{ game::MAG_041_Dribble_Emitter_State3_Empty, (void *)a_73A6D0, "041 nullsub_1604" },
		{ game::MAG_041_Dribble_Master_State4_NextTargetLoop, (void *)a_747500, "041 MAG_041_sub_84CF60" },
		{ game::MAG_041_Dribble_Master_State5_Advance, (void *)a_73A0D0, "041 MAG_041_sub_84CFA0" },
		{ game::MAG_041_Dribble_Master_State6_WaitQueuesEmpty, (void *)a_747550, "041 MAG_041_sub_84CFB0" },
		{ game::MAG_041_Dribble_Master_State7_Advance, (void *)a_73A0D0, "041 MAG_041_sub_84CFC0" },
		{ game::MAG_041_Dribble_Master_State8_Advance, (void *)a_73A0D0, "041 MAG_041_sub_84CFD0" },
		{ game::MAG_041_Dribble_Master_State9_Finish, (void *)a_7475A0, "041 MAG_041_sub_84CFE0" },
		{ game::MAG_041_Dribble_Master_State10_Empty, (void *)a_73A6D0, "041 nullsub_1598" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_041_Dribble_MasterTask, (void *)db_8466F0, "041 db_8466F0" },
		{ game::MAG_041_Dribble_Master_AllocActorPools, (void *)db_846850, "041 db_846850" },
		{ game::MAG_041_Dribble_ClearActorPools, (void *)db_846890, "041 db_846890" },
		{ game::MAG_041_Dribble_EmitterTask, (void *)db_846900, "041 db_846900" },
		{ game::MAG_041_Dribble_Emitter_SpawnActorsAndDribble, (void *)db_846990, "041 db_846990" },
		{ game::MAG_041_Dribble_ActorSystem_State0_StartActorSystemAtFrame, (void *)db_846A80, "041 db_846A80" },
		{ game::MAG_041_DirectorSetup, (void *)db_846AB0, "041 db_846AB0" },
		{ game::MAG_041_ResetTrailHistory, (void *)db_846C60, "041 db_846C60" },
		{ game::MAG_041_Director_CasterRefPoints, (void *)db_846D40, "041 db_846D40" },
		{ game::MAG_041_FillTrailRows, (void *)db_846EA0, "041 db_846EA0" },
		{ game::MAG_041_Director_TargetRefPoints, (void *)db_847090, "041 db_847090" },
		{ game::MAG_041_Dribble_ActorSystem_Step, (void *)db_847500, "041 db_847500" },
		{ game::MAG_041_ActSys_DrawPass, (void *)db_8476F0, "041 db_8476F0" },
		{ game::MAG_041_ActSys_DrawActorModel, (void *)db_847750, "041 db_847750" },
		{ game::MAG_041_MeshEmitF3, (void *)db_8479D0, "041 db_8479D0" },
		{ game::MAG_041_MeshEmitF4, (void *)db_847BE0, "041 db_847BE0" },
		{ game::MAG_041_PrimList_G3, (void *)db_848340, "041 db_848340" },
		{ game::MAG_041_PrimList_G4, (void *)db_848570, "041 db_848570" },
		{ game::MAG_041_PrimList_GT3, (void *)db_848830, "041 db_848830" },
		{ game::MAG_041_PrimList_GT4, (void *)db_848A90, "041 db_848A90" },
		{ game::MAG_041_ActSys_DrawSpritePass, (void *)db_848D70, "041 db_848D70" },
		{ game::MAG_041_ActSys_DrawActorSprite, (void *)db_848E30, "041 db_848E30" },
		{ game::MAG_041_ActSys_UpdatePass, (void *)db_848EB0, "041 db_848EB0" },
		{ game::MAG_041_ActorStep, (void *)db_848EF0, "041 db_848EF0" },
		{ game::MAG_041_MorphVertexBlock, (void *)db_849F20, "041 db_849F20" },
		{ game::MAG_041_Instance_EndOfLifeTest, (void *)db_84A030, "041 db_84A030" },
		{ game::MAG_041_Instance_SpawnFollowUp, (void *)db_84A0B0, "041 db_84A0B0" },
		{ game::MAG_041_AllocParticle, (void *)db_84A0E0, "041 db_84A0E0" },
		{ game::MAG_041_ActSys_UpdateEmitter, (void *)db_84A180, "041 db_84A180" },
		{ game::MAG_041_ActSys_EmitterSpawnStep, (void *)db_84A200, "041 db_84A200" },
		{ game::MAG_041_ActSys_SpawnParticles, (void *)db_84A280, "041 db_84A280" },
		{ game::MAG_041_ActSys_AllocParticleSlot, (void *)db_84AA60, "041 db_84AA60" },
		{ game::MAG_041_Actor_InterpolatePosition, (void *)db_84AC80, "041 db_84AC80" },
		{ game::MAG_041_SpawnSubEffects, (void *)db_84B770, "041 db_84B770" },
		{ game::MAG_041_Director_SpawnGroupEffects, (void *)db_84B800, "041 db_84B800" },
		{ game::MAG_041_Dribble_ActorSystem_State1_StepActorSystem, (void *)db_84B890, "041 db_84B890" },
		{ game::MAG_041_Dribble_Spawner_SpawnDribble, (void *)db_84B920, "041 db_84B920" },
		{ game::MAG_041_Dribble_BallTask, (void *)db_84B950, "041 db_84B950" },
		{ game::MAG_041_Dribble_DrawTargetAsBall, (void *)db_84BA60, "041 db_84BA60" },
		{ game::MAG_041_Dribble_RenderBallModel, (void *)db_84BB70, "041 db_84BB70" },
		{ game::MAG_041_Dribble_Ball_Setup, (void *)db_84C220, "041 db_84C220" },
		{ game::MAG_041_Dribble_InitBallBlock, (void *)db_84C2D0, "041 db_84C2D0" },
		{ game::MAG_041_Dribble_Ball_RadiiFromTable, (void *)db_84C350, "041 db_84C350" },
		{ game::MAG_041_Dribble_Ball_TargetPivot, (void *)db_84C3A0, "041 db_84C3A0" },
		{ game::MAG_041_Dribble_Ball_PositionInHand, (void *)db_84C460, "041 db_84C460" },
		{ game::MAG_041_Dribble_Ball_Grab, (void *)db_84C610, "041 db_84C610" },
		{ game::MAG_041_Dribble_Ball_Morph, (void *)db_84C650, "041 db_84C650" },
		{ game::MAG_041_Dribble_Ball_Hold, (void *)db_84C6B0, "041 db_84C6B0" },
		{ game::MAG_041_Dribble_Ball_Bounce, (void *)db_84C6F0, "041 db_84C6F0" },
		{ game::MAG_041_Dribble_Ball_Flight, (void *)db_84C7A0, "041 db_84C7A0" },
		{ game::MAG_041_Dribble_SpawnDust, (void *)db_84C940, "041 db_84C940" },
		{ game::MAG_041_Dribble_DustTask, (void *)db_84CA70, "041 db_84CA70" },
		{ game::MAG_041_Dribble_DrawDustPuff, (void *)db_84CB00, "041 db_84CB00" },
		{ game::MAG_041_Dribble_Dust_Init, (void *)db_84CB70, "041 db_84CB70" },
		{ game::MAG_041_Dribble_Dust_Move, (void *)db_84CB90, "041 db_84CB90" },
		{ game::MAG_041_Dribble_Ball_GiveBackTarget, (void *)db_84CCE0, "041 db_84CCE0" },
		{ game::MAG_041_Dribble_Target_TurnAndWalkBack, (void *)db_84CD20, "041 db_84CD20" },
		{ game::MAG_041_Dribble_Target_WalkBack, (void *)db_84CDE0, "041 db_84CDE0" },
		{ game::MAG_041_Dribble_Ball_End, (void *)db_84CE70, "041 db_84CE70" },
		{ game::MAG_041_Dribble_Emitter_ReleaseMaster, (void *)db_84CEF0, "041 db_84CEF0" },
		{ game::MAG_041_Dribble_Emitter_ApplyDamage, (void *)db_84CF10, "041 db_84CF10" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag041_dribble()
	{
		act::register_module(41);
		for (const act::dribble::ModPort *p = act::dribble::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(41, p->addr, p->port, p->name);
		for (const act::dribble::ModPort *p = act::dribble::PORTS; p->addr; p++)
			act::register_module_port(41, p->addr, p->port, p->name);
	}
}

