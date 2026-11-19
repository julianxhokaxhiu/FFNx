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

// Effect 71: Laser Eye: Counter Laser-Eye 30

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
namespace laser_eye_quistis
{
	static const Mod MOD_071 = { "laser_eye_quistis", 71, game::MAG_071_LASER_EYE_QUISTIS_FL, game::MAG_068_SCAN_FL,
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::unk_25FA42C, {}, game::MAG_071_LASER_EYE_QUISTIS_g_ParticleCursor, {}, {}, {}, {}, {}, game::MAG_071_LASER_EYE_QUISTIS_g_ActorSystemQueue, {}, game::MAG_071_LASER_EYE_QUISTIS_g_EmitterQueue, {}, {}, {}, {}, {}, game::unk_25FCD8A, game::MAG_071_LASER_EYE_QUISTIS_g_ParticlePool, {}, game::unk_25FCD9A, {}, game::MAG_071_LASER_EYE_QUISTIS_g_ScratchSP, {}, game::MAG_071_LASER_EYE_QUISTIS_g_ActorState },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_071_LASER_EYE_QUISTIS_DirectorSetup, game::MAG_071_LASER_EYE_QUISTIS_ActSys_Step, game::MAG_071_LASER_EYE_QUISTIS_ActSys_DrawPass, game::MAG_071_LASER_EYE_QUISTIS_ActSys_DrawActorModel, {}, game::MAG_071_LASER_EYE_QUISTIS_MeshEmitF3, game::MAG_071_LASER_EYE_QUISTIS_MeshEmitF4, game::MAG_071_LASER_EYE_QUISTIS_RenderPolyFT3List, game::MAG_071_LASER_EYE_QUISTIS_RenderPolyFT4List, game::MAG_071_LASER_EYE_QUISTIS_PrimList_G3, game::MAG_071_LASER_EYE_QUISTIS_PrimList_G4, game::MAG_071_LASER_EYE_QUISTIS_PrimList_GT3, game::MAG_071_LASER_EYE_QUISTIS_PrimList_GT4, game::MAG_071_LASER_EYE_QUISTIS_ActSys_DrawSpritePass, game::MAG_071_LASER_EYE_QUISTIS_ActSys_UpdatePass, game::MAG_071_LASER_EYE_QUISTIS_ActorStep, game::MAG_071_LASER_EYE_QUISTIS_ActSys_UpdateEmitter, game::MAG_071_LASER_EYE_QUISTIS_Particle_LifeCountdown, game::MAG_071_LASER_EYE_QUISTIS_ActSys_EmitterSpawnStep, game::MAG_071_LASER_EYE_QUISTIS_ActSys_SpawnParticles, game::MAG_071_LASER_EYE_QUISTIS_RandVec3, game::MAG_071_LASER_EYE_QUISTIS_JitterAngles3, game::MAG_071_LASER_EYE_QUISTIS_ActSys_AllocParticleSlot, game::MAG_071_LASER_EYE_QUISTIS_GetWord8Minus1, game::MAG_071_LASER_EYE_QUISTIS_Actor_InitPlacement, game::MAG_071_LASER_EYE_QUISTIS_SpawnSubEffects, game::MAG_071_LASER_EYE_QUISTIS_Director_SpawnGroupEffects, {}, {}, {}, {}, {}, game::MAG_071_LASER_EYE_QUISTIS_EmitterTask, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_071_LASER_EYE_QUISTIS_ResetTrailHistory, game::MAG_071_LASER_EYE_QUISTIS_Director_CasterRefPoints, game::MAG_071_LASER_EYE_QUISTIS_FillTrailRows, game::MAG_071_LASER_EYE_QUISTIS_Director_Mode3Setup, game::MAG_071_LASER_EYE_QUISTIS_Director_TargetRefPoints, game::MAG_071_LASER_EYE_QUISTIS_RelocateActorData, game::MAG_071_LASER_EYE_QUISTIS_ActSys_DrawActorSprite, game::MAG_071_LASER_EYE_QUISTIS_ActorMotion, game::MAG_071_LASER_EYE_QUISTIS_DampValue, game::MAG_071_LASER_EYE_QUISTIS_DampVector, game::MAG_071_LASER_EYE_QUISTIS_Instance_AdvanceTexFrame, game::MAG_071_LASER_EYE_QUISTIS_Instance_BuildTransform, game::MAG_071_LASER_EYE_QUISTIS_MorphVertexBlock, game::MAG_071_LASER_EYE_QUISTIS_Instance_EndOfLifeTest, game::MAG_071_LASER_EYE_QUISTIS_Instance_SpawnFollowUp, game::MAG_071_LASER_EYE_QUISTIS_AllocParticle, game::MAG_071_LASER_EYE_QUISTIS_RandRange, game::MAG_071_LASER_EYE_QUISTIS_RandRangeWide, game::MAG_071_LASER_EYE_QUISTIS_RandAngleJitter, game::MAG_071_LASER_EYE_QUISTIS_Particle_AttachToOwner, game::MAG_071_LASER_EYE_QUISTIS_Actor_InterpolatePosition, game::MAG_071_LASER_EYE_QUISTIS_ParticleList_Append, game::MAG_071_LASER_EYE_QUISTIS_ParticleList_Unlink, game::MAG_071_LASER_EYE_QUISTIS_Particle_PlaceAtAnchorSet, game::MAG_071_LASER_EYE_QUISTIS_Particle_PlaceAtPoint, {}, game::MAG_071_LASER_EYE_QUISTIS_ActSys_Step, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::MAG_071_LASER_EYE_QUISTIS_g_PacketCursor;
	static constexpr game::Id ACTOR_STATE = game::MAG_071_LASER_EYE_QUISTIS_g_ActorState;
	static constexpr game::Id SCRATCH_SP = game::MAG_071_LASER_EYE_QUISTIS_g_ScratchSP;
	static constexpr game::Id Q_EMITTER = game::MAG_071_LASER_EYE_QUISTIS_g_EmitterQueue, Q_ACTORS = game::MAG_071_LASER_EYE_QUISTIS_g_ActorSystemQueue, Q_LASER = game::MAG_071_LASER_EYE_QUISTIS_g_LaserQueue;
	static constexpr game::Id Q_SPARK = game::MAG_071_LASER_EYE_QUISTIS_g_SparkQueue, Q_STAGE = game::MAG_071_LASER_EYE_QUISTIS_g_StageLightQueue;
	static constexpr game::Id ORIG_Master = game::MAG_071_LASER_EYE_QUISTIS_MasterTask;
	static constexpr game::Id ORIG_Emitter = game::MAG_071_LASER_EYE_QUISTIS_EmitterTask;
	static constexpr game::Id ORIG_ActorSystem = game::MAG_071_LASER_EYE_QUISTIS_ActorSystemTask;
	static constexpr game::Id ORIG_Caster = game::MAG_071_LASER_EYE_QUISTIS_CasterTask;
	static constexpr game::Id ORIG_Laser = game::MAG_071_LASER_EYE_QUISTIS_LaserTask;
	static constexpr game::Id ORIG_Spark = game::MAG_071_LASER_EYE_QUISTIS_SparkTask;
	static const void *SOUND_LaserEye() { return (const void *)game_addr(game::MAG_071_LASER_EYE_QUISTIS_SndLaser); }
	static constexpr game::Id BIND_Block = game::MAG_071_LASER_EYE_QUISTIS_g_CasterBindBlock;
	static constexpr game::Id COM_Anim = game::MAG_071_LASER_EYE_QUISTIS_ComAnimSet;
	static constexpr game::Id ACTOR_DATA = game::MAG_071_LASER_EYE_QUISTIS_ActorData;
	static constexpr game::Id SPARK_TABLE = game::MAG_071_LASER_EYE_QUISTIS_SparkTable;

	namespace gx
	{
		static inline uint32_t BS_BindEntityModelFromComData(uint32_t entity, uint32_t out, uint32_t data) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
		static inline uint32_t QueueChainTransformation(uint32_t entity, uint32_t id) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
		static inline uint32_t sub_45E0B0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadIR123_u16))(a1); }
		static inline uint32_t sub_45E9D0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPF))(); }
		static inline uint32_t sub_45EBF0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPL))(); }
		static inline uint32_t TransformCameraByShadowRotation(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::TransformCameraByShadowRotation))(a1, a2, a3); }
		static inline uint32_t InitEffectSequenceFromData(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::InitEffectSequenceFromData))(a1, a2, a3, a4); }
		static inline uint32_t Effect_BuildMatrixFromDirAndUp(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::Effect_BuildMatrixFromDirAndUp))(a1, a2, a3); }
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
	static inline int32_t div4864(int32_t v)
	{
		const int32_t hi = (int32_t)(((int64_t)v * 0x6BCA1AF3) >> 32);
		const int32_t q = hi >> 11;
		return q + (int32_t)((uint32_t)q >> 31);
	}
	static inline uint32_t sq3(uint32_t v)
	{
		const int32_t x = S16(v, 0), y = S16(v, 2), z = S16(v, 4);
		return (uint32_t)add32(add32(mul32(x, x), mul32(y, y)), mul32(z, z));
	}

	uint32_t __cdecl cl_7D62A0(uint32_t a1);
	uint32_t __cdecl cl_7D6420(uint32_t a1);
	uint32_t __cdecl cl_7D6470(void);
	uint32_t __cdecl cl_7D65D0(uint32_t a1);
	uint32_t __cdecl cl_7D6640(uint32_t a1);
	uint32_t __cdecl cl_7DBF70(uint32_t a1);
	uint32_t __cdecl cl_7DBF90(uint32_t a1);
	uint32_t __cdecl cl_7DAEC0(uint32_t a1);
	uint32_t __cdecl cl_7DB470(uint32_t a1);
	uint32_t __cdecl cl_7DB4D0(uint32_t a1);
	uint32_t __cdecl cl_7DB530(uint32_t a1);
	uint32_t __cdecl cl_7DB560(uint32_t a1);
	uint32_t __cdecl cl_7DB5C0(uint32_t a1);
	uint32_t __cdecl cl_7DB9B0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl cl_7DBA70(uint32_t a1);
	uint32_t __cdecl cl_7DBB60(uint32_t a1, uint32_t a2);
	uint32_t __cdecl cl_7DBE30(uint32_t a1);
	uint32_t __cdecl cl_7DBE70(uint32_t a1);
	uint32_t __cdecl cl_7DBEA0(uint32_t a1);
	uint32_t __cdecl cl_7DBEE0(uint32_t a1);
	uint32_t __cdecl cl_7DBF10(uint32_t a1);
	uint32_t __cdecl cl_7DBC70(uint32_t a1);
	uint32_t __cdecl cl_7DBCE0(uint32_t a1);
	uint32_t __cdecl cl_7DBD40(uint32_t a1);
	uint32_t __cdecl cl_7D6550(uint32_t a1);
	uint32_t __cdecl cl_7D6730(uint32_t a1);
	uint32_t __cdecl cl_7D6760(uint32_t a1);
	uint32_t __cdecl cl_7D6910(uint32_t a1);
	uint32_t __cdecl cl_7D69F0(void);
	uint32_t __cdecl cl_7D6B50(void);
	uint32_t __cdecl cl_7D6D40(void);
	uint32_t __cdecl cl_7D71B0(uint32_t a1);
	uint32_t __cdecl cl_7D7240(void);
	uint32_t __cdecl cl_7D72A0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl cl_7D7520(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl cl_7D7730(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl cl_7D7E90(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl cl_7D80C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl cl_7D8380(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl cl_7D85E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl cl_7D88C0(void);
	uint32_t __cdecl cl_7D8980(uint32_t a1);
	uint32_t __cdecl cl_7D8A00(void);
	uint32_t __cdecl cl_7D8A40(uint32_t a1);
	uint32_t __cdecl cl_7D9A70(uint32_t a1, uint32_t a2);
	uint32_t __cdecl cl_7D9B80(uint32_t a1, uint32_t a2);
	uint32_t __cdecl cl_7D9C00(uint32_t a1, uint32_t a2);
	uint32_t __cdecl cl_7D9C30(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl cl_7D9CD0(uint32_t a1);
	uint32_t __cdecl cl_7D9D50(uint32_t a1);
	uint32_t __cdecl cl_7D9DD0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl cl_7DA5B0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl cl_7DA7D0(uint32_t a1);
	uint32_t __cdecl cl_7DB320(uint32_t a1, uint32_t a2);
	uint32_t __cdecl cl_7DB3B0(void);
	uint32_t __cdecl cl_7DB440(uint32_t a1);
	uint32_t __cdecl cl_7DBC10(uint32_t a1);
	uint32_t __cdecl cl_7DBFE0(uint32_t a1);
	uint32_t __cdecl cl_7DC020(uint32_t a1);
	uint32_t __cdecl cl_7DC0B0(uint32_t a1);
	uint32_t __cdecl cl_7DC0D0(uint32_t a1);
	static void laser_update(uint32_t node);
}
}
}

namespace ff8fx
{
namespace act
{
namespace laser_eye_quistis
{
	uint32_t __cdecl cl_7D62A0(uint32_t a1)
	{
		g_mod = &MOD_071;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_25FA42C)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Master_State0_Advance);
		states[1] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Master_State1_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[2] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Master_AllocActorPools);
		states[3] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Master_SpawnEmitter);
		states[4] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Master_State4_NextActionLoop);
		states[5] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Master_WaitThenLightDown);
		states[6] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Master_State7_Advance);
		states[8] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Master_State8_Advance);
		states[9] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Master_State9_Finish);
		states[10] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ArenaA1));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_25FC17C));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_25FA428)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ArenaA0));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_25FC178));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_25FA428)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_25FCD9A)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25FCD8A)) = 0;
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_EMITTER)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_ACTORS)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_LASER)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_SPARK)));
		U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(Q_STAGE)));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl cl_7D6420(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ArenaCursor));
		MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ParticlePool)) = p;
		p += 0x870;
		MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorPool)) = p;
		p += 0x8340;
		MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ArenaCursor)) = p;
		cl_7D6470();
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_STAGE), game_addr(game::MAG_071_LASER_EYE_QUISTIS_LightRampUpTask), 0x40, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl cl_7D6470(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ParticlePool)), 0x870);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorPool)), 0x8340);
		MEM<uint16_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ParticleCursor)) = 0;
		MEM<uint16_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorCursor)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25FCD88)) = 0;
		MEM<uint16_t>(game_addr(game::unk_25FA410)) = 0;
		return 0;
	}

	uint32_t __cdecl cl_7D65D0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Emitter_SpawnActorsAndCasterTask);
		states[1] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Emitter_ReleaseMaster);
		states[2] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Emitter_State2_DamageFromTick25);
		states[3] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Emitter_State3_Empty);
		x::MAG_001_CURE_Emitter_UpdatePos(node);
		x::MAG_001_CURE_Emitter_ComputeModelBounds(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl cl_7D6640(uint32_t a1)
	{
		a_73C100(a1, game_addr(ORIG_ActorSystem), game_addr(ACTOR_DATA), 0x12, 0x2D, 0);
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_LASER), game_addr(ORIG_Caster), 0x94, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl cl_7DBF70(uint32_t a1)
	{
		if (U16(a1, 0x24) == 0xA)
		{
			U8(U32(a1, 0x10), 0x63) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return a1;
	}

	uint32_t __cdecl cl_7DBF90(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 0x19)
			return 0;
		const int32_t ai = S8(node, 0x2A);
		const int32_t ti = S8(node, 0x2B);
		const uint32_t acts = U32(U32(node, 0xC), 4);
		const uint32_t recs = U32(acts + (uint32_t)(ai * 5) * 4, 8);
		x::ApplyActionResultToTarget(recs + (uint32_t)(ti * 3) * 8);
		const uint8_t st = U8(node, 0x29);
		U8(node, 0x26) |= 1;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl cl_7DAEC0(uint32_t a1)
	{
		alignas(4) uint8_t frame[0x10] = {};
		const uint32_t f = P(frame);
		uint32_t dir = MEM<uint32_t>(game_addr(ACTOR_STATE));
		const uint32_t table = U32(dir, 0x224);
		const int32_t di = (int32_t)S8(a1, 0x6A);
		const uint32_t owner = U32(a1, 0x5C);
		const uint32_t desc = U32(table + di * 4, 0);
		U8(a1, 0x68) = U8(desc, 0x11);
		const uint32_t m = a1 + 0x2C;
		switch ((uint32_t)(int32_t)S8(desc, 0x1C))
		{
			case 0:
				x::MAG_022_sub_8DD770(m);
				break;
			case 1:
			{
				x::MAG_022_sub_8DD770(m);
				const int32_t k = shl32((int32_t)S8(a1, 0x6B), 4);
				const uint32_t d = MEM<uint32_t>(game_addr(ACTOR_STATE));
				const uint32_t e = (uint32_t)k + d;
				const int32_t dx = sub32(S32(d, 0x1C4), S32(e, 0x54));
				const int32_t dz = sub32(S32(d, 0x1CC), S32(e, 0x5C));
				const uint32_t ang = x::CartesianToGameAngle((uint32_t)dx, (uint32_t)dz);
				if (ang != 0)
					x::MAG_022_sub_8DD8A0(m, ang);
				break;
			}
			case 2:
			{
				S16(f, 2) = 0x1000;
				S16(f, 0) = 0;
				S16(f, 4) = 0;
				const int32_t k = shl32((int32_t)S8(a1, 0x6B), 4);
				const uint32_t e = (uint32_t)k + dir;
				S16(f, 8) = (int16_t)b2_fx((uint32_t)sub32(S32(e, 0xD4), S32(dir, 0x1F4)));
				S16(f, 0xA) = (int16_t)b2_fx((uint32_t)sub32(S32(e, 0xD8), S32(dir, 0x1F8)));
				S16(f, 0xC) = (int16_t)b2_fx((uint32_t)sub32(S32(e, 0xDC), S32(dir, 0x1FC)));
				gx::Effect_BuildMatrixFromDirAndUp(m, f + 8, f);
				break;
			}
			case 3:
				ff8fx::copy<(32)>((void *)m, (void *)(owner + 0x8C));
				break;
			case 4:
			{
				x::MAG_022_sub_8DD770(m);
				dir = MEM<uint32_t>(game_addr(ACTOR_STATE));
				const int32_t ent = (int32_t)S16(dir, 0x1E);
				const int32_t yaw = (int32_t)S16(game_addr(game::unk_1D972CE) + (uint32_t)(ent * 39) * 4, 0);
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
			case 1: a_742FF0(a1); break;
			case 2:
			case 3: a_7424C0(a1); break;
			case 4: cl_7DA7D0(a1); break;
			default: break;
		}
		U16(a1, 0x64) = (uint16_t)(int16_t)S8(desc, 0x12);
		return 0;
	}

	uint32_t __cdecl cl_7DB470(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Caster_State0_BindCastAnimation);
		states[1] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Caster_State1_SoundAtTick14);
		states[2] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Caster_State2_SpawnTwoLasers);
		states[3] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Caster_State3_Empty);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl cl_7DB4D0(uint32_t a1)
	{
		const uint32_t entity = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2C) * 0x9C;
		gx::BS_BindEntityModelFromComData(entity, game_addr(BIND_Block), game_addr(COM_Anim));
		gx::QueueChainTransformation(game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2C) * 0x9C, 3);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl cl_7DB530(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0xE)
		{
			x::BdPlaySE(P(SOUND_LaserEye()), 0, 0x80);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl cl_7DB560(uint32_t a1)
	{
		if (S16(a1, 0x24) < 0x12)
			return 0;
		const uint32_t l0 = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_LASER), game_addr(ORIG_Laser), 0x94, a1);
		U16(l0, 0x90) = 0x40;
		const uint32_t l1 = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_LASER), game_addr(ORIG_Laser), 0x94, a1);
		U8(a1, 0x26) |= 1;
		U16(l1, 0x90) = 0xFFC0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static void laser_update(uint32_t node)
	{
		alignas(4) uint8_t frame[0x48] = {};
		const uint32_t f = P(frame);
		U32(f, 0x28) = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Laser_State0_Appear);
		U32(f, 0x2C) = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Laser_StateShow);
		U32(f, 0x30) = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Laser_StateGrow);
		U32(f, 0x34) = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Laser_StateHold);
		U32(f, 0x38) = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Laser_StateShrink);
		U32(f, 0x3C) = game_addr(game::MAG_071_LASER_EYE_QUISTIS_LaserBeam_State5_Empty);
		const uint32_t caster_m = game_addr(game::unk_1D97300) + (uint32_t)U8(U32(node, 0xC), 0) * 0x9C;
		if (U16(node, 0x24) == 0)
		{
			x::GetEffectSpawnPosition(game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2D) * 0x9C, 0xF1, 0, node + 0x7C);
			U16(f, 8) = U16(node, 0x90);
			S16(f, 0xA) = 0;
			S16(f, 0xC) = 0;
			x::matrixMultiplyVector(caster_m, f + 8, f + 0x10);
			U16(node, 0x7C) = (uint16_t)(U16(node, 0x7C) + U16(f, 0x10));
			U16(node, 0x7E) = (uint16_t)(U16(node, 0x7E) + U16(f, 0x12));
			U16(node, 0x80) = (uint16_t)(U16(node, 0x80) + U16(f, 0x14));
		}
		x::GetEffectSpawnPosition(game_addr(game::BattleEntitySlotData) + (uint32_t)U8(U32(node, 0xC), 0) * 0x9C, 0xF0, 0, node + 0x74);
		U16(f, 0x10) = U16(node, 0x90);
		S16(f, 0x12) = 0;
		S16(f, 0x14) = 0;
		x::matrixMultiplyVector(caster_m, f + 0x10, f + 8);
		U16(node, 0x74) = (uint16_t)(U16(node, 0x74) + U16(f, 8));
		const uint16_t ex = U16(node, 0x74);
		const uint16_t tx = U16(node, 0x7C);
		U16(node, 0x76) = (uint16_t)(U16(node, 0x76) + U16(f, 0xA));
		const uint16_t ey = U16(node, 0x76);
		U16(node, 0x78) = (uint16_t)(U16(node, 0x78) + U16(f, 0xC));
		const uint16_t ez = U16(node, 0x78);
		U16(f, 2) = (uint16_t)(U16(node, 0x7E) - ey);
		U16(f, 0) = (uint16_t)(tx - ex);
		S16(f, 0x18) = 0;
		S16(f, 0x1A) = 0x1000;
		S16(f, 0x1C) = 0;
		U16(f, 4) = (uint16_t)(U16(node, 0x80) - ez);
		gx::Effect_BuildMatrixFromDirAndUp(node + 0x54, f, f + 0x18);
		U16(node, 0x8E) = (uint16_t)x::Sqrt(sq3(f));
		callp(U32(f, 0x28 + 4 * S8(node, 0x29)), node);
		x::MAG_022_sub_8DD8A0(node + 0x54, (uint32_t)(int32_t)S16(node, 0x46));
		const int32_t w = S16(node, 0x8C);
		U32(node, 0x4C) = game_addr(game::MAG_071_LASER_EYE_QUISTIS_BeamPrimModel);
		S16(f, 0x18) = 0;
		S16(f, 0x1A) = 0;
		S32(node, 0x38) = div4864(mul32(w, S16(node, 0x8E)));
		S16(f, 0x1C) = (int16_t)(-0x80 - mul32(w, 0x1300) / 4096);
		x::matrixMultiplyVector(node + 0x54, f + 0x18, f + 0x10);
		U16(node, 0x84) = (uint16_t)(U16(f, 0x10) + U16(node, 0x1C));
		U16(node, 0x86) = (uint16_t)(U16(node, 0x1E) + U16(f, 0x12));
		U16(node, 0x88) = (uint16_t)(U16(node, 0x20) + U16(f, 0x14));
		cl_7DB9B0(node, node + 0x54);
		const uint16_t dx = (uint16_t)(U16(node, 0x7C) - U16(node, 0x74));
		const uint16_t dz = (uint16_t)(U16(node, 0x80) - U16(node, 0x78));
		S16(f, 0x18) = 0;
		U16(f, 0) = dx;
		S16(f, 0x1A) = 0x1000;
		S16(f, 0x1C) = 0;
		S16(f, 2) = 0;
		U16(f, 4) = dz;
		gx::Effect_BuildMatrixFromDirAndUp(node + 0x54, f, f + 0x18);
		x::MAG_022_sub_8DD8A0(node + 0x54, (uint32_t)(int32_t)S16(node, 0x46));
		const uint16_t len = (uint16_t)x::Sqrt(sq3(f));
		U16(node, 0x8E) = len;
		U16(node, 0x1E) = 0;
		const int32_t w2 = S16(node, 0x8C);
		S32(node, 0x38) = div4864(mul32((int16_t)len, w2));
		U32(node, 0x4C) = game_addr(game::MAG_071_LASER_EYE_QUISTIS_HeadPrimModel);
		cl_7DB9B0(node, node + 0x54);
		if (U16(node, 0x8C) != 0x1000)
		{
			U32(node, 0x4C) = game_addr(game::MAG_071_LASER_EYE_QUISTIS_FlashPrimModel);
			const uint32_t s0 = U32(node, 0x30);
			U32(f, 0x1C) = U32(node, 0x34);
			U32(f, 0x20) = U32(node, 0x38);
			const uint32_t p0 = U32(node, 0x84);
			const uint32_t s3 = U32(node, 0x3C);
			U32(node, 0x38) = 0x1000;
			U32(node, 0x34) = 0x1000;
			U32(node, 0x30) = 0x1000;
			U32(f, 0x24) = s3;
			const uint32_t p1 = U32(node, 0x88);
			U32(node, 0x1C) = p0;
			U32(node, 0x20) = p1;
			x::UnpackRotationMatrix(game_addr(game::unk_1D97778), f + 0x28);
			cl_7DB9B0(node, f + 0x28);
			U32(node, 0x30) = s0;
			U32(node, 0x34) = U32(f, 0x1C);
			U32(node, 0x38) = U32(f, 0x20);
			U32(node, 0x3C) = U32(f, 0x24);
		}
	}

	uint32_t __cdecl cl_7DB5C0(uint32_t a1)
	{
		const uint32_t node = a1;
		laser_update(node);
		cl_7DBA70(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl cl_7DB9B0(uint32_t a1, uint32_t a2)
	{
		if ((U8(a1, 0x26) & 4) != 0)
			return 0;
		alignas(4) uint8_t mtx[0x20];
		const uint32_t m = P(mtx);
		const int32_t px = S16(a1, 0x1C);
		const int32_t pz = S16(a1, 0x20);
		ff8fx::copy<(0x20)>(mtx, (const void *)a2);
		const int32_t py = S16(a1, 0x1E);
		S32(m, 0x14) = px;
		S32(m, 0x18) = py;
		S32(m, 0x1C) = pz;
		x::scale3DMatrix(m, a1 + 0x30);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), m, m);
		x::GTE_SetRotMatrix_W(m);
		x::GTE_SetTransVector_W(m);
		const uint32_t hdr = x::Field_Alloc(0x58);
		U32(hdr, 0) = U32(a1, 0x4C);
		U32(hdr, 8) = U32(a1, 0x40);
		S32(hdr, 0xC) = S16(a1, 0x50);
		const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U32(hdr, 0x1C) = 0xF0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = x::Effect_RenderPrimModel(hdr, ot, 2, cursor);
		x::Field_Free(0x58);
		return 0;
	}

	uint32_t __cdecl cl_7DBA70(uint32_t a1)
	{
		const int16_t tick = S16(a1, 0x24);
		if (tick > 0x12)
			return 0;
		const uint32_t rec = game_addr(SPARK_TABLE) + (uint32_t)((int32_t)tick * 8);
		int32_t count = S16(rec, 4);
		const uint32_t sp = MEM<uint32_t>(game_addr(SCRATCH_SP)) - 8;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = sp;
		if (count > 0)
		{
			const uint32_t pt = sp + 4;
			do
			{
				const int32_t r = (int32_t)x::CrtRand();
				const int32_t t = S16(rec, 0) + r % (int32_t)S16(rec, 2);
				S32(pt, 0) = t;
				S32(sp, 0) = 0x1000 - t;
				const uint32_t spark = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_SPARK), game_addr(ORIG_Spark), 0x60, a1);
				x::set_dword_1CA8A30(U32(sp, 0));
				gx::sub_45E0B0(a1 + 0x74);
				gx::sub_45E9D0();
				x::set_dword_1CA8A30(U32(pt, 0));
				gx::sub_45E0B0(a1 + 0x7C);
				gx::sub_45EBF0();
				x::GTE_StoreIR123(spark + 0x1C);
				cl_7DBB60(spark, 0x40);
				count--;
			} while (count != 0);
		}
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 8;
		return 0;
	}

	uint32_t __cdecl cl_7DBB60(uint32_t a1, uint32_t a2)
	{
		alignas(4) uint8_t frame[0x30] = {};
		const uint32_t f = P(frame);
		uint32_t range = a2;
		if ((uint16_t)range == 0)
			range = 1;
		const uint32_t yaw = x::CrtRand() & 0xFFF;
		const uint32_t pitch = x::CrtRand() & 0xFFF;
		x::MAG_022_sub_8DD770(f + 0x10);
		x::MAG_022_sub_8DD8A0(f + 0x10, (uint32_t)(int32_t)(int16_t)yaw);
		x::sub_8DD7E0(f + 0x10, (uint32_t)(int32_t)(int16_t)pitch);
		S16(f, 0) = 0;
		S16(f, 2) = 0;
		const int32_t r = (int32_t)x::CrtRand();
		S16(f, 4) = (int16_t)(r % (int32_t)(int16_t)range);
		x::matrixMultiplyVector(f + 0x10, f, f + 8);
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + U16(f, 8));
		U16(a1, 0x1E) = (uint16_t)(U16(a1, 0x1E) + U16(f, 0xA));
		U16(a1, 0x20) = (uint16_t)(U16(a1, 0x20) + U16(f, 0xC));
		return 0;
	}

	uint32_t __cdecl cl_7DBE30(uint32_t a1)
	{
		const uint32_t e0 = U32(a1, 0x74);
		U8(a1, 0x26) |= 4;
		const uint32_t e1 = U32(a1, 0x78);
		U32(a1, 0x1C) = e0;
		U16(a1, 0x46) = 0x800;
		U32(a1, 0x30) = 0x400;
		U32(a1, 0x34) = 0x400;
		const uint8_t st = U8(a1, 0x29);
		U32(a1, 0x20) = e1;
		U32(a1, 0x4C) = game_addr(game::MAG_071_LASER_EYE_QUISTIS_BeamPrimModel);
		U32(a1, 0x38) = 0x1000;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return a1;
	}

	uint32_t __cdecl cl_7DBE70(uint32_t a1)
	{
		const uint32_t e0 = U32(a1, 0x74);
		const uint32_t e1 = U32(a1, 0x78);
		const int16_t tick = S16(a1, 0x24);
		U32(a1, 0x1C) = e0;
		U16(a1, 0x46) = 0x800;
		U32(a1, 0x20) = e1;
		if (tick >= 4)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) &= 0xFB;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return a1;
	}

	uint32_t __cdecl cl_7DBEA0(uint32_t a1)
	{
		const uint32_t e0 = U32(a1, 0x74);
		U16(a1, 0x8C) = (uint16_t)(U16(a1, 0x8C) + 0x400);
		const uint32_t e1 = U32(a1, 0x78);
		U32(a1, 0x1C) = e0;
		const int16_t w = S16(a1, 0x8C);
		U16(a1, 0x46) = 0x800;
		U32(a1, 0x20) = e1;
		if (w >= 0x1000)
		{
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0x8C) = 0x1000;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return a1;
	}

	uint32_t __cdecl cl_7DBEE0(uint32_t a1)
	{
		const uint32_t e0 = U32(a1, 0x74);
		const uint32_t e1 = U32(a1, 0x78);
		const int16_t tick = S16(a1, 0x24);
		U32(a1, 0x1C) = e0;
		U16(a1, 0x46) = 0x800;
		U32(a1, 0x20) = e1;
		if (tick >= 0xE)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl cl_7DBF10(uint32_t a1)
	{
		const uint32_t t0 = U32(a1, 0x7C);
		U16(a1, 0x8C) = (uint16_t)(U16(a1, 0x8C) + 0xF800);
		const int16_t w = S16(a1, 0x8C);
		U32(a1, 0x1C) = t0;
		const uint32_t t1 = U32(a1, 0x80);
		U16(a1, 0x46) = 0;
		U32(a1, 0x20) = t1;
		if (w <= 0)
		{
			U8(a1, 0x26) |= 5;
			U16(a1, 0x8C) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return a1;
	}

	uint32_t __cdecl cl_7DBC70(uint32_t a1)
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

	uint32_t __cdecl cl_7DBCE0(uint32_t a1)
	{
		U32(a1, 0x4C) = game_addr(game::MAG_071_LASER_EYE_QUISTIS_SparkFlipbook);
		U16(a1, 0x52) = 6;
		U16(a1, 0x58) = (uint16_t)((x::CrtRand() & 0x3FF) - 0x200);
		U16(a1, 0x5A) = (uint16_t)((x::CrtRand() & 0x3FF) - 0x200);
		U16(a1, 0x5C) = (uint16_t)((x::CrtRand() & 0x3FF) - 0x200);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl cl_7DBD40(uint32_t a1)
	{
		int16_t vx = S16(a1, 0x58);
		int16_t vy = S16(a1, 0x5A);
		int16_t vz = S16(a1, 0x5C);
		vx = (int16_t)(vx - (int16_t)(((int32_t)vx << 8) / 4096));
		vy = (int16_t)(vy - (int16_t)(((int32_t)vy << 8) / 4096));
		S16(a1, 0x58) = vx;
		vz = (int16_t)(vz - (int16_t)(((int32_t)vz << 8) / 4096));
		S16(a1, 0x5A) = vy;
		S16(a1, 0x5C) = vz;
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + (uint16_t)((int32_t)vx / 16));
		U16(a1, 0x1E) = (uint16_t)(U16(a1, 0x1E) + (uint16_t)((int32_t)vy / 16));
		U16(a1, 0x20) = (uint16_t)(U16(a1, 0x20) + (uint16_t)((int32_t)vz / 16));
		if (a_743C20(a1) != 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl cl_7D6550(uint32_t a1)
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

	uint32_t __cdecl cl_7D6730(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			cl_7D6760(a1);
			cl_7D71B0(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl cl_7D6760(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t data = U32(node, 0x30);
		uint16_t w29a = U16(node, 0x29A);
		uint16_t w29c = U16(node, 0x29C);
		uint32_t st = node + 0x34;
		const uint32_t root = U32(node, 0x10);
		MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState)) = st;
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
			st = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState));
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
		cl_7D69F0();
		st = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState));
		if (U16(st, 0x24) == 4)
			callp(game_addr(game::MAG_071_LASER_EYE_QUISTIS_ResetTrailHistory), node);
		cl_7D6D40();
		st = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState));
		if (U16(st, 0x24) == 1)
		{
			callp(game_addr(game::MAG_071_LASER_EYE_QUISTIS_FillTrailRows), node);
			st = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState));
		}
		if (U16(st, 0x24) == 3)
		{
			a_73FE90(node);
			st = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState));
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
		st = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState));
		U32(st, 0) = dist;
		return 0;
	}

	uint32_t __cdecl cl_7D6910(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState));
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

	uint32_t __cdecl cl_7D69F0(void)
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

	uint32_t __cdecl cl_7D6B50(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState));
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

	uint32_t __cdecl cl_7D6D40(void)
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

	uint32_t __cdecl cl_7D71B0(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			cl_7DB3B0();
			cl_7D8A00();
			cl_7D88C0();
			cl_7D7240();
			sys = U32(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState), 0);
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
		U16(game_addr(game::unk_25FCD9A), 0) = (uint16_t)(U16(game_addr(game::unk_25FCD9A), 0) + c14);
		U16(game_addr(game::unk_25FCD8A), 0) = (uint16_t)(U16(game_addr(game::unk_25FCD8A), 0) + c16);
		return done;
	}

	uint32_t __cdecl cl_7D7240(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					cl_7D72A0(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl cl_7D72A0(uint32_t a1, uint32_t a2)
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

	uint32_t __cdecl cl_7D7520(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl cl_7D7730(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl cl_7D7E90(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl cl_7D80C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl cl_7D8380(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl cl_7D85E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl cl_7D88C0(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						cl_7D8980(act);
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
							cl_7D8980(act);
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

	uint32_t __cdecl cl_7D8980(uint32_t a1)
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

	uint32_t __cdecl cl_7D8A00(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				cl_7D9CD0(act);
			else if (type == 1)
				cl_7D8A40(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl cl_7D8A40(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (cl_7D9B80(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl cl_7D9A70(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState), 0);
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

	uint32_t __cdecl cl_7D9B80(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				cl_7D9C00(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			cl_7D9C00(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			cl_7D9C00(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl cl_7D9C00(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return cl_7D9C30(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl cl_7D9C30(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ParticlePool));
		int32_t idx = MEM<int16_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ParticleCursor));
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
			MEM<uint16_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ParticleCursor)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ParticleCursor)) = 0;
		return slot;
	}

	uint32_t __cdecl cl_7D9CD0(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			cl_7DAEC0(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_071_LASER_EYE_QUISTIS_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_071_LASER_EYE_QUISTIS_Actor_InterpolatePosition), a1);
		cl_7D9D50(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl cl_7D9D50(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState), 0);
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
				cl_7D9DD0(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			cl_7D9DD0(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl cl_7D9DD0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ScratchSP), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ScratchSP), 0) = sp;
		uint32_t node = cl_7DA5B0(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState), 0);
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
		cl_7DB320(node, desc);
		U32(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ScratchSP), 0) = U32(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ScratchSP), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl cl_7DA5B0(uint32_t a1, uint32_t a2)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorPool));
		int32_t i = MEM<int16_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorCursor));
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
			MEM<uint16_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorCursor)) = 0;
		else
			MEM<uint16_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorCursor)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl cl_7DA7D0(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState), 0);
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
				uint32_t g = U32(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState), 0);
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

	uint32_t __cdecl cl_7DB320(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			cl_7D9C30(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			cl_7D9C30(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			cl_7D9C30(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			cl_7D9C30(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl cl_7DB3B0(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState));
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
					cl_7D9C30(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				cl_7D9C30(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::MAG_071_LASER_EYE_QUISTIS_g_ActorState));
			}
		}
		return 0;
	}

	uint32_t __cdecl cl_7DB440(uint32_t a1)
	{
		if (cl_7D71B0(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl cl_7DBC10(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Spark_Init);
		states[1] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Spark_Move);
		states[2] = game_addr(game::MAG_071_LASER_EYE_QUISTIS_Spark_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		cl_7DBC70(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl cl_7DBFE0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (U8(node, 0x63) != 0)
			return a1;
		const uint8_t idx = U8(node, 0x2A);
		if ((int16_t)(int8_t)idx < S16(node, 0x58))
		{
			const uint8_t loops = U8(node, 0x2E);
			U8(node, 0x2A) = (uint8_t)(idx + 1);
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x2E) = (uint8_t)(loops + 1);
			U8(node, 0x29) = (uint8_t)(st - 1);
			return a1;
		}
		const uint8_t st = U8(node, 0x29);
		U16(node, 0x60) = 0xF;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return a1;
	}

	uint32_t __cdecl cl_7DC020(uint32_t a1)
	{
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) - 1);
		if (S16(a1, 0x60) > 0)
			return 0;
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_STAGE), game_addr(game::MAG_071_LASER_EYE_QUISTIS_LightRampDownTask), 0x40, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl cl_7DC0B0(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x1C) = 0x400;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return a1;
	}

	uint32_t __cdecl cl_7DC0D0(uint32_t a1)
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

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_071_LASER_EYE_QUISTIS_Master_State0_Advance, (void *)a_73A0D0, "071 MAG_071_sub_7D6400" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Master_State1_Advance, (void *)a_73A0D0, "071 MAG_071_sub_7D6410" },
		{ game::MAG_071_LASER_EYE_QUISTIS_LightRampUpTask, (void *)a_73A380, "071 MAG_071_sub_7D64B0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_LightRampUp_State0_ResetEntityRecords, (void *)a_7335D0, "071 MAG_071_sub_7D6510" },
		{ game::MAG_071_LASER_EYE_QUISTIS_LightRampUp_State2_Empty, (void *)a_73A6D0, "071 nullsub_1428" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Master_SpawnEmitter, (void *)a_73A170, "071 MAG_071_sub_7D65A0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_SpawnChildTask, (void *)a_73C100, "071 MAG_071_sub_7D6680" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActorSystemTask, (void *)a_73A380, "071 MAG_071_sub_7D66D0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Director_Mode3Setup, (void *)a_73FE90, "071 sub_7D6C20" },
		{ game::MAG_071_LASER_EYE_QUISTIS_RelocateActorData, (void *)a_740210, "071 sub_7D6F50" },
		{ game::MAG_071_LASER_EYE_QUISTIS_RenderPrimModel, (void *)a_7355E0, "071 sub_7D73E0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_RenderPolyFT3List, (void *)a_735BB0, "071 sub_7D79B0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_RenderPolyFT4List, (void *)a_735DF0, "071 sub_7D7BF0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActorMotion, (void *)a_740910, "071 sub_7D8AD0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_DampValue, (void *)a_740F70, "071 sub_7D9130" },
		{ game::MAG_071_LASER_EYE_QUISTIS_DampVector, (void *)a_740FC0, "071 sub_7D9180" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Instance_AdvanceTexFrame, (void *)a_741050, "071 sub_7D9210" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Instance_BuildTransform, (void *)a_741120, "071 sub_7D92E0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Particle_LifeCountdown, (void *)a_741B60, "071 sub_7D9D20" },
		{ game::MAG_071_LASER_EYE_QUISTIS_RandRange, (void *)a_742290, "071 MAG_071_RandRange" },
		{ game::MAG_071_LASER_EYE_QUISTIS_RandRangeWide, (void *)a_7422C0, "071 sub_7DA480" },
		{ game::MAG_071_LASER_EYE_QUISTIS_RandVec3, (void *)a_742300, "071 sub_7DA4C0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_JitterAngles3, (void *)a_742350, "071 sub_7DA510" },
		{ game::MAG_071_LASER_EYE_QUISTIS_RandAngleJitter, (void *)a_7423A0, "071 MAG_071_RandAngleJitter" },
		{ game::MAG_071_LASER_EYE_QUISTIS_GetWord8Minus1, (void *)a_7424B0, "071 sub_7DA670" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Particle_AttachToOwner, (void *)a_7424C0, "071 sub_7DA680" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ParticleList_Append, (void *)a_742CA0, "071 sub_7DAE60" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ParticleList_Unlink, (void *)a_742CD0, "071 sub_7DAE90" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "071 sub_7DB0D0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Particle_PlaceAtPoint, (void *)a_742FF0, "071 sub_7DB210" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActorSystem_State2_Empty, (void *)a_73A6D0, "071 nullsub_1431" },
		{ game::MAG_071_LASER_EYE_QUISTIS_StepCounterLimit, (void *)a_743C20, "071 sub_7DBDF0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Spark_State2_Empty, (void *)a_73A6D0, "071 nullsub_1432" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Caster_State3_Empty, (void *)a_73A6D0, "071 nullsub_1434" },
		{ game::MAG_071_LASER_EYE_QUISTIS_LaserBeam_State5_Empty, (void *)a_73A6D0, "071 nullsub_1433" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Emitter_State3_Empty, (void *)a_73A6D0, "071 nullsub_1429" },
		{ game::MAG_071_LASER_EYE_QUISTIS_LightRampDownTask, (void *)a_73A380, "071 MAG_071_sub_7DC050" },
		{ game::MAG_071_LASER_EYE_QUISTIS_LightRampDown_State2_Empty, (void *)a_73A6D0, "071 nullsub_1435" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Master_State6_WaitQueuesEmpty, (void *)a_747550, "071 MAG_071_sub_7DC120" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Master_State7_Advance, (void *)a_73A0D0, "071 MAG_071_sub_7DC130" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Master_State8_Advance, (void *)a_73A0D0, "071 MAG_071_sub_7DC140" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Master_State9_Finish, (void *)a_7475A0, "071 MAG_071_sub_7DC150" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Master_State10_Empty, (void *)a_73A6D0, "071 nullsub_1430" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_071_LASER_EYE_QUISTIS_MasterTask, (void *)cl_7D62A0, "071 cl_7D62A0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Master_AllocActorPools, (void *)cl_7D6420, "071 cl_7D6420" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ClearActorPools, (void *)cl_7D6470, "071 cl_7D6470" },
		{ game::MAG_071_LASER_EYE_QUISTIS_LightRampUp_Step, (void *)cl_7D6550, "071 cl_7D6550" },
		{ game::MAG_071_LASER_EYE_QUISTIS_EmitterTask, (void *)cl_7D65D0, "071 cl_7D65D0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Emitter_SpawnActorsAndCasterTask, (void *)cl_7D6640, "071 cl_7D6640" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActorSystem_State0_StartActorSystemAtFrame, (void *)cl_7D6730, "071 cl_7D6730" },
		{ game::MAG_071_LASER_EYE_QUISTIS_DirectorSetup, (void *)cl_7D6760, "071 cl_7D6760" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ResetTrailHistory, (void *)cl_7D6910, "071 cl_7D6910" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Director_CasterRefPoints, (void *)cl_7D69F0, "071 cl_7D69F0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_FillTrailRows, (void *)cl_7D6B50, "071 cl_7D6B50" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Director_TargetRefPoints, (void *)cl_7D6D40, "071 cl_7D6D40" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActSys_Step, (void *)cl_7D71B0, "071 cl_7D71B0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActSys_DrawPass, (void *)cl_7D7240, "071 cl_7D7240" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActSys_DrawActorModel, (void *)cl_7D72A0, "071 cl_7D72A0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_MeshEmitF3, (void *)cl_7D7520, "071 cl_7D7520" },
		{ game::MAG_071_LASER_EYE_QUISTIS_MeshEmitF4, (void *)cl_7D7730, "071 cl_7D7730" },
		{ game::MAG_071_LASER_EYE_QUISTIS_PrimList_G3, (void *)cl_7D7E90, "071 cl_7D7E90" },
		{ game::MAG_071_LASER_EYE_QUISTIS_PrimList_G4, (void *)cl_7D80C0, "071 cl_7D80C0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_PrimList_GT3, (void *)cl_7D8380, "071 cl_7D8380" },
		{ game::MAG_071_LASER_EYE_QUISTIS_PrimList_GT4, (void *)cl_7D85E0, "071 cl_7D85E0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActSys_DrawSpritePass, (void *)cl_7D88C0, "071 cl_7D88C0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActSys_DrawActorSprite, (void *)cl_7D8980, "071 cl_7D8980" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActSys_UpdatePass, (void *)cl_7D8A00, "071 cl_7D8A00" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActorStep, (void *)cl_7D8A40, "071 cl_7D8A40" },
		{ game::MAG_071_LASER_EYE_QUISTIS_MorphVertexBlock, (void *)cl_7D9A70, "071 cl_7D9A70" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Instance_EndOfLifeTest, (void *)cl_7D9B80, "071 cl_7D9B80" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Instance_SpawnFollowUp, (void *)cl_7D9C00, "071 cl_7D9C00" },
		{ game::MAG_071_LASER_EYE_QUISTIS_AllocParticle, (void *)cl_7D9C30, "071 cl_7D9C30" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActSys_UpdateEmitter, (void *)cl_7D9CD0, "071 cl_7D9CD0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActSys_EmitterSpawnStep, (void *)cl_7D9D50, "071 cl_7D9D50" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActSys_SpawnParticles, (void *)cl_7D9DD0, "071 cl_7D9DD0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActSys_AllocParticleSlot, (void *)cl_7DA5B0, "071 cl_7DA5B0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Actor_InterpolatePosition, (void *)cl_7DA7D0, "071 cl_7DA7D0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Actor_InitPlacement, (void *)cl_7DAEC0, "071 cl_7DAEC0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_SpawnSubEffects, (void *)cl_7DB320, "071 cl_7DB320" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Director_SpawnGroupEffects, (void *)cl_7DB3B0, "071 cl_7DB3B0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_ActorSystem_State1_StepActorSystem, (void *)cl_7DB440, "071 cl_7DB440" },
		{ game::MAG_071_LASER_EYE_QUISTIS_CasterTask, (void *)cl_7DB470, "071 cl_7DB470" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Caster_State0_BindCastAnimation, (void *)cl_7DB4D0, "071 cl_7DB4D0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Caster_State1_SoundAtTick14, (void *)cl_7DB530, "071 cl_7DB530" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Caster_State2_SpawnTwoLasers, (void *)cl_7DB560, "071 cl_7DB560" },
		{ game::MAG_071_LASER_EYE_QUISTIS_LaserTask, (void *)cl_7DB5C0, "071 cl_7DB5C0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_LaserBeam_DrawModel, (void *)cl_7DB9B0, "071 cl_7DB9B0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Laser_SpawnSparks, (void *)cl_7DBA70, "071 cl_7DBA70" },
		{ game::MAG_071_LASER_EYE_QUISTIS_RandomOffset, (void *)cl_7DBB60, "071 cl_7DBB60" },
		{ game::MAG_071_LASER_EYE_QUISTIS_SparkTask, (void *)cl_7DBC10, "071 cl_7DBC10" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Spark_DrawSprite, (void *)cl_7DBC70, "071 cl_7DBC70" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Spark_Init, (void *)cl_7DBCE0, "071 cl_7DBCE0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Spark_Move, (void *)cl_7DBD40, "071 cl_7DBD40" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Laser_State0_Appear, (void *)cl_7DBE30, "071 cl_7DBE30" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Laser_StateShow, (void *)cl_7DBE70, "071 cl_7DBE70" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Laser_StateGrow, (void *)cl_7DBEA0, "071 cl_7DBEA0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Laser_StateHold, (void *)cl_7DBEE0, "071 cl_7DBEE0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Laser_StateShrink, (void *)cl_7DBF10, "071 cl_7DBF10" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Emitter_ReleaseMaster, (void *)cl_7DBF70, "071 cl_7DBF70" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Emitter_State2_DamageFromTick25, (void *)cl_7DBF90, "071 cl_7DBF90" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Master_State4_NextActionLoop, (void *)cl_7DBFE0, "071 cl_7DBFE0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_Master_WaitThenLightDown, (void *)cl_7DC020, "071 cl_7DC020" },
		{ game::MAG_071_LASER_EYE_QUISTIS_LightRampDown_State0_SetAmplitude400, (void *)cl_7DC0B0, "071 cl_7DC0B0" },
		{ game::MAG_071_LASER_EYE_QUISTIS_LightRampDown_Step, (void *)cl_7DC0D0, "071 cl_7DC0D0" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag071_laser_eye_quistis()
	{
		act::register_module(71);
		for (const act::laser_eye_quistis::ModPort *p = act::laser_eye_quistis::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(71, p->addr, p->port, p->name);
		for (const act::laser_eye_quistis::ModPort *p = act::laser_eye_quistis::PORTS; p->addr; p++)
			act::register_module_port(71, p->addr, p->port, p->name);
	}
}

