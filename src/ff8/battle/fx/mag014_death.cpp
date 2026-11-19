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

// Effect 14: Death: a copy of the actor/heal effect library

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
namespace death
{
	static const Mod MOD_014 = { "death", 14, game::MAG_014_DEATH_DEATH_STONE_FL, game::MAG_013_DARK_MIST_POISON_MIST_FL,
		{ {}, {}, game::unk_162B860, {}, {}, game::unk_273AFE4, {}, {}, {}, {}, {}, {}, game::MAG_014_FilePtr, {}, {}, {}, game::unk_273AFE8, {}, {}, {}, {}, {}, {}, {}, game::MAG_014_ActorSystemQueue, {}, game::MAG_014_EmitterQueue, {}, {}, {}, {}, {}, game::unk_2742820, {}, {}, game::unk_274285C, {}, game::MAG_014_ScratchSP, {}, game::MAG_014_ActorStatePtr },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_014_DirectorSetup, game::MAG_014_ActSys_Step, game::MAG_014_ActSys_DrawPass, game::Death_DrawPrimActor, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_014_ActSys_DrawSpritePass, game::MAG_014_ActSys_UpdatePass, game::MAG_014_ActorStep, game::MAG_014_ActSys_UpdateEmitter, game::MAG_014_Particle_LifeCountdown, game::MAG_014_ActSys_EmitterSpawnStep, game::MAG_014_ActSys_SpawnParticles, game::MAG_014_RandVec3, game::MAG_014_JitterAngles3, game::MAG_014_ActSys_AllocParticleSlot, game::MAG_014_GetWord8Minus1, game::MAG_014_Particle_Init, game::MAG_014_SpawnSubEffects, game::MAG_014_Director_SpawnGroupEffects, {}, {}, {}, {}, {}, game::Death_EmitterTask, game::MAG_014_Death_Emitter_State0_SpawnDirector, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_014_ResetTrailHistory, game::MAG_014_Director_StartPositions, game::MAG_014_FillTrailRows, game::MAG_014_Director_Mode3Setup, game::MAG_014_Director_TargetPositions, game::MAG_014_RelocateActorData, game::MAG_014_ActSys_DrawActorSprite, game::MAG_014_ActorMotion, game::MAG_014_DampValue, game::MAG_014_DampVector, game::MAG_014_Instance_AdvanceTexFrame, game::MAG_014_Instance_BuildTransform, game::MAG_014_MorphVertexBlock, game::MAG_014_Instance_EndOfLifeTest, game::MAG_014_Instance_SpawnFollowUp, game::MAG_014_AllocParticle, game::MAG_014_RandRange, game::MAG_014_RandRangeWide, game::MAG_014_RandAngleJitter, game::MAG_014_Particle_AttachToOwner, game::MAG_014_Actor_InterpolatePosition, {}, game::MAG_014_ParticleList_Unlink, game::MAG_014_Particle_PlaceAtAnchorSet, game::MAG_014_Particle_PlaceAtPoint, {}, game::MAG_014_ActSys_Step, game::MAG_014_Death_Emitter_State1_ApplyDamage, game::MAG_014_Death_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_014_Death_Emitter_State3_FinishReleaseRoot, game::MAG_014_Death_Emitter_State4_Empty, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::MAG_014_PacketCursor;
	static constexpr game::Id ACTOR_STATE = game::MAG_014_ActorStatePtr;
	static constexpr game::Id SCRATCH_SP = game::MAG_014_ScratchSP;
	static constexpr game::Id REAPER_DATA = game::unk_162B860;
	static constexpr game::Id ORIG_Master = game::MAG_014_DEATH_Tick;
	static constexpr game::Id ORIG_Reaper = game::Death_ReaperTask;
	static constexpr game::Id ORIG_ActorSystem = game::MAG_014_AuraActorSystemTask;
	static constexpr game::Id ORIG_Spark = game::Death_SparkTask;
	static constexpr game::Id REAPER_CTX = game::MAG_014_ReaperRenderCtx;

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
	}
	using namespace dx;

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
	static inline void t4_next_state(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }

	static inline uint32_t task_end(uint32_t node, uint8_t status)
	{
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl d_8B6C20(void);
	uint32_t __cdecl d_8B6BE0(uint32_t a1);
	uint32_t __cdecl d_8B6D40(uint32_t a1);
	uint32_t __cdecl d_8B6D10(uint32_t a1);
	uint32_t __cdecl d_8B6EA0(uint32_t a1);
	uint32_t __cdecl d_8B6E30(uint32_t a1);
	uint32_t __cdecl d_8B6EF0(uint32_t a1);
	uint32_t __cdecl d_8B6F30(uint32_t a1);
	uint32_t __cdecl d_8B7390(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_8B7290(uint32_t a1);
	uint32_t __cdecl d_8B7030(uint32_t a1);
	uint32_t __cdecl d_8B6F70(uint32_t a1);
	uint32_t __cdecl d_8BD700(uint32_t a1);
	uint32_t __cdecl d_8BD6A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_8BD670(uint32_t a1);
	uint32_t __cdecl d_8B7780(uint32_t a1);
	uint32_t __cdecl d_8BD710(uint32_t a1);
	uint32_t __cdecl d_8BD760(uint32_t a1);
	uint32_t __cdecl d_8BDE90(uint32_t a1);
	uint32_t __cdecl d_8BDEF0(uint32_t a1);
	uint32_t __cdecl d_8BD5E0(uint32_t a1);
	uint32_t __cdecl d_8BD620(uint32_t a1);
	uint32_t __cdecl d_8BDFA0(uint32_t a1);
	uint32_t __cdecl d_8BD8A0(uint32_t a1);
	uint32_t __cdecl d_8BD8E0(uint32_t a1);
	uint32_t __cdecl d_8BDD20(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_8BDBD0(uint32_t a1);
	uint32_t __cdecl d_8BDE20(uint32_t a1);
	uint32_t __cdecl d_8BDE50(uint32_t a1);
	uint32_t __cdecl d_8B8520(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_8BA960(uint32_t a1);
	uint32_t __cdecl d_8BBC10(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl d_8BC590(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_8B6A40(uint32_t a1);
	uint32_t __cdecl d_8B7920(uint32_t a1);
	uint32_t __cdecl d_8B7B00(uint32_t a1);
	uint32_t __cdecl d_8B7D80(void);
	uint32_t __cdecl d_8B8430(uint32_t a1);
	uint32_t __cdecl d_8B84C0(void);
	uint32_t __cdecl d_8B8680(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_8B87B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_8B89D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_8B8C60(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_8B9150(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_8B9C70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_8BA1D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl d_8BA8A0(void);
	uint32_t __cdecl d_8BA9E0(void);
	uint32_t __cdecl d_8BAA20(uint32_t a1);
	uint32_t __cdecl d_8BBA50(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_8BBB60(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_8BBBE0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_8BBCB0(uint32_t a1);
	uint32_t __cdecl d_8BBD30(uint32_t a1);
	uint32_t __cdecl d_8BBDB0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl d_8BC7B0(uint32_t a1);
	uint32_t __cdecl d_8BD2A0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_8BD330(void);
	uint32_t __cdecl d_8BD3C0(uint32_t a1);
	uint32_t __cdecl d_8BD3F0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl d_8BD510(uint32_t a1);
	uint32_t __cdecl d_8BDFC0(uint32_t a1);
	uint32_t __cdecl d_8BE070(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace death
{
	uint32_t __cdecl d_8B6C20(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_014_ParticlePoolPtr)), 0x870);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_014_ActorPoolPtr)), 0x9D80);
		MEM<uint16_t>(game_addr(game::MAG_014_ParticleCursor)) = 0;
		MEM<uint16_t>(game_addr(game::MAG_014_ActorCursor)) = 0;
		MEM<uint16_t>(game_addr(game::unk_27423F0)) = 0;
		MEM<uint16_t>(game_addr(game::unk_273AE88)) = 0;
		return 0;
	}

	uint32_t __cdecl d_8B6BE0(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::MAG_014_FileArenaCursor));
		MEM<uint32_t>(game_addr(game::MAG_014_ParticlePoolPtr)) = p;
		p += 0x870;
		MEM<uint32_t>(game_addr(game::MAG_014_ActorPoolPtr)) = p;
		p += 0x9D80;
		MEM<uint32_t>(game_addr(game::MAG_014_FileArenaCursor)) = p;
		d_8B6C20();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl d_8B6D40(uint32_t a1)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(REAPER_DATA)), 0x54);
		const uint32_t slot0 = U8(a1, 0x2D);
		x::GetEffectSpawnPosition(game_addr(game::BattleEntitySlotData) + slot0 * 0x9C, 0xF1, 0, MEM<uint32_t>(game_addr(REAPER_DATA)) + 8);
		const uint8_t slot = U8(a1, 0x2D);
		const uint32_t d = MEM<uint32_t>(game_addr(REAPER_DATA));
		U16(d, 0xA) = 0;
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)slot * 0x9C;
		U32(d, 0) = U32(ent, 0x1C);
		U32(d, 4) = U32(ent, 0x20);
		U16(d, 2) = 0;
		const int32_t size125 = mul32(S16(ent, 0x26), 125);
		const int32_t off = shl32(size125, 4) >> 12;
		if (slot <= 2)
			U16(d, 4) = (uint16_t)(U16(d, 4) + (uint16_t)(-0x200 - off));
		else
			U16(d, 4) = (uint16_t)(U16(d, 4) + (uint16_t)(off + 0x200));
		const int16_t dz = (int16_t)(U16(d, 0xC) - U16(d, 4));
		const int16_t dxv = (int16_t)(U16(d, 8) - U16(d, 0));
		const uint32_t angle = x::CartesianToGameAngle((uint32_t)(int32_t)dxv, (uint32_t)(int32_t)dz);
		U16(MEM<uint32_t>(game_addr(REAPER_DATA)), 0x4A) = (uint16_t)((angle - 0x800) & 0xFFF);
		return 0;
	}

	uint32_t __cdecl d_8B6D10(uint32_t a1)
	{
		d_8B6D40(a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_014_DirectorQueue), game_addr(game::Death_DirectorTask), 0x48, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl d_8B6EA0(uint32_t a1)
	{
		uint32_t d = MEM<uint32_t>(game_addr(REAPER_DATA));
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			a_73A6D0();
			d = MEM<uint32_t>(game_addr(REAPER_DATA));
		}
		const uint16_t next = U16(d, 0x44);
		if (U16(d, 0x42) != next)
		{
			U16(d, 0x42) = next;
			a_73A6D0();
		}
		return a1;
	}

	uint32_t __cdecl d_8B6E30(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_014_UploadReaperTIMs);
		states[1] = game_addr(game::MAG_014_SpawnReaperCreature);
		states[2] = game_addr(game::MAG_014_Death_Director_State2_Finish);
		states[3] = game_addr(game::MAG_014_Death_Director_State3_Empty);
		d_8B6EA0(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl d_8B6EF0(uint32_t a1)
	{
		if (U8(a1, 0x2E) == 0)
		{
			x::Battle_QueueTIMUpload_GetEOF(game_addr(game::MAG014_Reaper_BodyTIM));
			x::Battle_QueueTIMUpload_GetEOF(game_addr(game::MAG014_Reaper_WingsTIM));
		}
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x44) = 1;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl d_8B6F30(uint32_t a1)
	{
		U16(a1, 0x44) = (uint16_t)(U16(a1, 0x44) - 1);
		if (S16(a1, 0x44) > 0)
			return 0;
		const uint32_t n = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_014_CreatureQueue), game_addr(ORIG_Reaper), 0x140, a1);
		x::Effect_BindModelContainerSetAnim(n, game_addr(game::MAG014_Reaper_ModelContainer), 3);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl d_8B7390(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		(void)a3;
		const uint32_t ctx = a4;
		const uint32_t blk = U32(ctx, 0x48);
		uint32_t cursor = MEM<uint32_t>(U32(ctx, 0x4C));
		const uint32_t vbuf = U32(U32(ctx, 0x44), 4);
		uint32_t table = U32(a1, 4);
		const uint32_t bones = U32(a1, 0) + 0x10;
		const int32_t count = S32(table, 0);
		const uint32_t hdr = x::Field_Alloc(0x5C);
		const uint32_t mask = U32(blk, 0x7C);
		const uint32_t colour = U32(blk, 0x28) & 0xFFFFFF;
		table += 4;
		U32(hdr, 0x50) = mask;
		U32(hdr, 0x4C) = colour | 0x2C000000;
		U32(hdr, 0x48) = colour | 0x24000000;
		x::GTE_SetRotMatrix(ctx + 0x20);
		x::GTE_SetTransVector(ctx + 0x20);
		for (int32_t i = 0; i < count; i++)
		{
			uint32_t v = vbuf;
			table += 4;
			uint32_t obj = U32(a1, 4) + U32(table, -4);
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
			int32_t n4 = S16(obj, 2);
			uint32_t rec = obj + 0xC;
			uint32_t pkt = cursor;
			for (int32_t k = n3; k > 0; k--, rec += 0x10)
			{
				const uint32_t i0 = U16(rec, 0) & 0xFFF, i1 = U16(rec, 2) & 0xFFF, i2 = U16(rec, 4) & 0xFFF;
				U32(hdr, 0x38) = i0;
				U32(hdr, 0x3C) = i1;
				U32(hdr, 0x40) = i2;
				if (S16(vbuf + i0 * 8, 2) > 0 || S16(vbuf + i1 * 8, 2) > 0 || S16(vbuf + i2 * 8, 2) > 0)
					continue;
				x::GTE_LoadV012(vbuf + i0 * 8, vbuf + i1 * 8, vbuf + i2 * 8);
				x::GTE_RTPT();
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(hdr + 0x54);
				if (S32(hdr, 0x54) <= 0)
					continue;
				x::GTE_AVSZ3();
				x::GTE_ReadOTZ(hdr + 0x58);
				U32(hdr, 0x58) = U32(hdr, 0x58) >> 2;
				GTE_StoreSXY012_PolyFT3(pkt);
				U32(pkt, 0xC) = U32(rec, 8);
				U32(pkt, 0x14) = U32(rec, 0xC);
				U16(pkt, 0x1C) = U16(rec, 6);
				U32(pkt, 4) = U32(hdr, 0x48);
				U32(pkt, 0) = 0x7000000;
				x::SSIGPU_InsertPrimAutoDepth(a2 + U32(hdr, 0x58) * 4, pkt);
				pkt += 0x20;
			}
			if (n4 > 0)
			{
				rec += 4;
				for (; n4 > 0; n4--, rec += 0x14)
				{
					const uint32_t i0 = U16(rec, -4) & 0xFFF, i1 = U16(rec, -2) & 0xFFF, i2 = U16(rec, 0) & 0xFFF;
					U32(hdr, 0x38) = i0;
					U32(hdr, 0x3C) = i1;
					U32(hdr, 0x40) = i2;
					if (S16(vbuf + i0 * 8, 2) > 0 || S16(vbuf + i1 * 8, 2) > 0 || S16(vbuf + i2 * 8, 2) > 0)
						continue;
					if (S16(vbuf + U32(hdr, 0x44) * 8, 2) > 0)
						continue;
					x::GTE_LoadV012(vbuf + i0 * 8, vbuf + i1 * 8, vbuf + i2 * 8);
					x::GTE_RTPT();
					x::GTE_NCLIP();
					x::GTE_ReadMAC0(hdr + 0x54);
					if (S32(hdr, 0x54) <= 0)
						continue;
					GTE_StoreSXY012_PolyFT3_2(pkt);
					const uint32_t i3 = U16(rec, 2) & 0xFFF;
					U32(hdr, 0x44) = i3;
					x::GTE_LoadV0(vbuf + i3 * 8);
					x::GTE_RTPS();
					x::GTE_ReadSXY2(pkt + 0x20);
					x::GTE_AVSZ4();
					x::GTE_ReadOTZ(hdr + 0x58);
					const uint32_t z = U32(hdr, 0x58) >> 2;
					U32(hdr, 0x58) = z;
					U32(pkt, 0xC) = U32(rec, 4);
					U16(pkt, 0x1C) = U16(rec, 0xC);
					U16(pkt, 0x24) = U16(rec, 0xE);
					U32(pkt, 0x14) = U32(rec, 8);
					U32(pkt, 0) = 0x9000000;
					U32(pkt, 4) = U32(hdr, 0x4C);
					x::SSIGPU_InsertPrimAutoDepth(a2 + z * 4, pkt);
					pkt += 0x28;
				}
			}
			cursor = pkt;
		}
		const uint32_t out = U32(ctx, 0x4C);
		x::Field_Free(0x5C);
		MEM<uint32_t>(out) = cursor;
		return out;
	}

	uint32_t __cdecl d_8B7290(uint32_t a1)
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
		d_8B7390(U32(blk, 0x64), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, ctx);
		const uint32_t second = U32(blk, 0x78);
		if (second != 0)
			d_8B7390(U32(second, 4), MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 4, ctx);
		x::BattleModel_BuildBoneMatricesFromPose(anim);
		return 0;
	}

	uint32_t __cdecl d_8B7030(uint32_t a1)
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
			return d_8B7290(game_addr(REAPER_CTX));
		}
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = a_746C10(a1, game_addr(game::unk_27419F0), MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		return 0;
	}

	uint32_t __cdecl d_8B6F70(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[6];
		states[0] = game_addr(game::Death_ReaperAppear);
		states[1] = game_addr(game::MAG_014_Death_Reaper_State1_AppearAnimDone);
		states[2] = game_addr(game::Death_ReaperStrike);
		states[3] = game_addr(game::Death_ReaperVanish);
		states[4] = game_addr(game::MAG_014_Death_Reaper_State4_StartWobbleOutFinish);
		states[5] = game_addr(game::MAG_014_Death_Reaper_State5_Empty);
		callp(states[S8(node, 0x29)], node);
		const int32_t pz = S16(node, 0x50);
		const int32_t py = S16(node, 0x4E);
		ff8fx::copy<(32)>((void *)game_addr(game::unk_1D99AB0), (const void *)(node + 0x70));
		const int32_t px = S16(node, 0x4C);
		S32(node, 0x8C) = pz;
		const uint8_t f = U8(node, 0x26);
		S32(node, 0x84) = px;
		S32(node, 0x88) = py;
		if (!(f & 4))
		{
			d_8B7030(node);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl d_8BD700(uint32_t a1)
	{
		(void)a1;
		U16(MEM<uint32_t>(game_addr(game::unk_16309A8)), 0x20) = 0;
		return 0;
	}

	uint32_t __cdecl d_8BD6A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		x::MAG_007_sub_8DCC00(a1, 0x54);
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

	uint32_t __cdecl d_8BD670(uint32_t a1)
	{
		d_8BD6A0(game_addr(REAPER_CTX), game_addr(game::MAG_014_ReaperVertexBuffer), game_addr(game::MAG_014_ReaperObjectBlock), a1 + 0x30);
		U8(a1, 0x26) |= 8;
		return 0;
	}

	uint32_t __cdecl d_8B7780(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(REAPER_DATA));
		U32(a1, 0x4C) = U32(d, 0);
		const uint32_t w4 = U32(d, 4);
		const uint16_t yaw = U16(d, 0x4A);
		U32(a1, 0x60) = a1 + 0x114;
		U32(a1, 0x50) = w4;
		U16(a1, 0x3E) = yaw;
		U32(a1, 0x11C) = 0x1000;
		U32(a1, 0x118) = 0x1000;
		U32(a1, 0x114) = 0x1000;
		d_8BD700(a1);
		if (U8(a1, 0x2E) == 0)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 5);
			d_8BD670(a1);
			a_73C100(a1, game_addr(ORIG_ActorSystem), game_addr(game::unk_1801FD0), 0, 0x14, 2);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_014_StageWobbleQueue), game_addr(game::MAG_014_StageWobbleInTask), 0x70, a1);
			x::BdPlaySE(game_addr(game::unk_162B864), 0, 0x80);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		}
		x::au_re_Battle_ReadAnimation_7(a1, 6);
		U16(a1, 0x13A) = 0x1000;
		U16(a1, 0x13C) = 0x1000;
		d_8BD3F0(a1, 1);
		d_8BD510(a1);
		x::BdPlaySE(game_addr(game::unk_162B868), 0, 0x80);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 2);
		return 0;
	}

	uint32_t __cdecl d_8BD710(uint32_t a1)
	{
		if (U16(a1, 0x136) == 0x14)
			U8(a1, 0x26) &= 0xF7;
		if (x::au_re_Battle_ReadAnimation_8(a1) == 1)
		{
			U8(a1, 0x26) &= 0xF7;
			x::au_re_Battle_ReadAnimation_7(a1, 6);
			x::BdPlaySE(game_addr(game::unk_162B868), 0, 0x80);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl d_8BD760(uint32_t a1)
	{
		U16(a1, 0x13A) = (uint16_t)(U16(a1, 0x13A) + 0xFC00);
		if (S16(a1, 0x13A) <= 0)
			U16(a1, 0x13A) = 0;
		U16(a1, 0x13C) = U16(a1, 0x13A);
		d_8BD3F0(a1, 1);
		d_8BD510(a1);
		if (U16(a1, 0x136) == 5)
		{
			const int32_t action = S8(a1, 0x2A);
			U16(MEM<uint32_t>(game_addr(REAPER_DATA)), 0x48) = 1;
			const uint32_t targets = U32(U32(U32(a1, 0xC), 4) + (uint32_t)(action * 20), 8);
			const int32_t target = S8(a1, 0x2B);
			if (!(U8(targets + (uint32_t)(target * 24), 3) & 4))
			{
				a_73C100(a1, game_addr(ORIG_ActorSystem), game_addr(game::unk_18069C4), 0, 0x14, 0);
				x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_014_DarkSparkQueue), game_addr(game::Death_SparkBurstTask), 0x118, a1);
			}
		}
		const uint32_t r = x::au_re_Battle_ReadAnimation_8(a1);
		if (U8(a1, 0x2E) == U8(a1, 0x2F))
		{
			if ((uint16_t)r == 1)
				U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		}
		if (S16(a1, 0x136) >= 0xB)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl d_8BDE90(uint32_t a1)
	{
		x::au_re_Battle_ReadAnimation_8(a1);
		U16(a1, 0x13A) = (uint16_t)(U16(a1, 0x13A) + 0x200);
		if (S16(a1, 0x13A) >= 0x1000)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 4;
			U16(a1, 0x13A) = 0x1000;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		U16(a1, 0x13C) = U16(a1, 0x13A);
		d_8BD3F0(a1, 1);
		d_8BD510(a1);
		return 0;
	}

	uint32_t __cdecl d_8BDEF0(uint32_t a1)
	{
		if (U8(a1, 0x2E) == U8(a1, 0x2F))
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_014_StageWobbleQueue), game_addr(game::MAG_014_StageWobbleOutTask), 0x70, a1);
		const uint32_t d = MEM<uint32_t>(game_addr(REAPER_DATA));
		U16(a1, 0x26) |= 1;
		U16(d, 0x4C) = 1;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl d_8BD5E0(uint32_t a1)
	{
		if (S16(a1, 0x24) < 0x1C)
			return 0;
		U16(a1, 0x1C) = 0;
		uint32_t p = game_addr(game::unk_1D989BA);
		for (int i = 4; i != 0; i--, p += 0x2C)
		{
			MEM<uint16_t>(p - 0x28) = 0;
			MEM<uint8_t>(p) = 0;
			MEM<uint8_t>(p - 1) = 0;
			MEM<uint8_t>(p - 2) = 0;
		}
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl d_8BD620(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0x200);
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

	uint32_t __cdecl d_8BDFA0(uint32_t a1)
	{
		if (S16(a1, 0x24) < 0)
			return a1;
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x1C) = 0x600;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl d_8BD8A0(uint32_t a1)
	{
		const uint32_t slot = U8(a1, 0x2D);
		x::GetEffectSpawnPosition(game_addr(game::BattleEntitySlotData) + slot * 0x9C, 0xF1, 0, a1 + 0x1C);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static void SparkGroup(uint32_t a1, uint32_t base, uint32_t m2, uint32_t pitch_mask, uint32_t pitch_add, int32_t pitch_sub)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_014_DarkSparkQueue), game_addr(ORIG_Spark), 0x118, a1);
		uint32_t vel = t + 0xF4;
		uint32_t mat = t + 0x54;
		for (int k = 4; k != 0; k--, mat += 0x20, vel += 8)
		{
			U32(vel, -0x20) = U32(a1, 0x1C);
			U32(vel, -0x1C) = U32(a1, 0x20);
			const int16_t yaw = (int16_t)(x::CrtRand() & 0xFFF);
			const uint32_t r = x::CrtRand();
			ff8fx::copy<(32)>((void *)m2, (const void *)base);
			const uint32_t pitch = ((r & pitch_mask) + (uint32_t)pitch_add - (uint32_t)pitch_sub) & 0xFFF;
			x::MAG_022_sub_8DD8A0(m2, (uint32_t)(int32_t)yaw);
			x::sub_8DD7E0(m2, (uint32_t)(int32_t)(int16_t)pitch);
			ff8fx::copy<(32)>((void *)mat, (const void *)m2);
			const uint32_t r3 = x::CrtRand() & 0x1FFF;
			U16(vel, 2) = (uint16_t)(0xFFFFF000u - r3);
			x::matrixMultiplyVector(m2, vel, vel);
		}
	}

	uint32_t __cdecl d_8BD8E0(uint32_t a1)
	{
		alignas(4) uint8_t base[32] = {}, m2[32] = {};
		const uint32_t slot = U8(a1, 0x2D);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + slot * 0x9C;
		x::MAG_022_sub_8DD770(P(base));
		x::MAG_022_sub_8DD8A0(P(base), (uint32_t)(int32_t)S16(ent, 0xE));
		x::sub_8DD960(P(base), 0x155);
		x::sub_8DD7E0(P(base), 0xEAB);
		const int16_t c = S16(a1, 0x24);
		int32_t n = 0;
		if (c == 1) n = 4;
		else if (c == 2) n = 3;
		else if (c == 3) n = 2;
		else if (c == 4) n = 1;
		for (; n > 0; n--)
			SparkGroup(a1, P(base), P(m2), 0x1FF, 0, 0x100);
		n = 0;
		if (c == 1 || c == 2) n = 2;
		else if (c == 3 || c == 4) n = 1;
		for (; n > 0; n--)
			SparkGroup(a1, P(base), P(m2), 0xFF, 0x380, 0);
		if (S16(a1, 0x24) >= 4)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl d_8BDD20(uint32_t a1, uint32_t a2)
	{
		if (U8(a1, 0x26) & 4)
			return 0;
		alignas(4) uint8_t mb[32];
		const uint32_t M = P(mb);
		ff8fx::copy<(32)>(mb, (const void *)a2);
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
		U32(h, 0xC) = (uint32_t)(int32_t)S16(a1, 0x50);
		U16(h, 0x22) = 0x100;
		U16(h, 0x20) = 0x100;
		U16(h, 0x2A) = 0x100;
		U16(h, 0x28) = 0x100;
		U32(h, 0x10) = (uint32_t)(int32_t)S16(a1, 0x52);
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(h, 0x18) = 0;
		U16(h, 0x1A) = 0;
		U16(h, 0x1E) = 0;
		U16(h, 0x1C) = 0;
		U16(h, 0x26) = 0;
		U16(h, 0x24) = 0;
		U32(h, 0x14) = 0xF0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = d_8B8680(h, ot, 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0x68);
		return 0;
	}

	static void SparkMove(uint32_t node)
	{
		uint32_t v = node + 0xF4;
		for (int k = 4; k != 0; k--, v += 8)
		{
			for (int j = 0; j < 3; j++)
			{
				const int16_t s = S16(v, 2 * j);
				S16(v, 2 * j) = (int16_t)(s - (int16_t)(s / 4));
			}
			for (int j = 0; j < 3; j++)
				S16(v, -0x20 + 2 * j) = (int16_t)(S16(v, -0x20 + 2 * j) + (int16_t)(S16(v, 2 * j) / 16));
		}
	}

	static void SparkDraw(uint32_t node)
	{
		const int32_t e = S16(node, 0x24) * 2;
		S32(node, 0x30) = MEM<int16_t>(game_addr(game::unk_1630928) + e);
		S32(node, 0x34) = MEM<int16_t>(game_addr(game::unk_1630948) + e);
		S32(node, 0x38) = MEM<int16_t>(game_addr(game::unk_1630968) + e);
		U16(node, 0x50) = MEM<uint16_t>(game_addr(game::unk_1630988) + e);
		uint32_t pos = node + 0xD4;
		uint32_t mat = node + 0x54;
		for (int k = 4; k != 0; k--, pos += 8, mat += 0x20)
		{
			U32(node, 0x1C) = U32(pos, 0);
			U32(node, 0x20) = U32(pos, 4);
			d_8BDD20(node, mat);
		}
	}

	uint32_t __cdecl d_8BDBD0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_014_Death_Spark_State0_Init);
		states[1] = game_addr(game::MAG_014_Death_Spark_State1_EndAtTick14);
		states[2] = game_addr(game::MAG_014_Death_Spark_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		SparkMove(node);
		SparkDraw(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl d_8BDE20(uint32_t a1)
	{
		U32(a1, 0x38) = 0x1000;
		U32(a1, 0x34) = 0x1000;
		U32(a1, 0x30) = 0x1000;
		const uint8_t st = U8(a1, 0x29);
		U32(a1, 0x4C) = game_addr(game::unk_162B86C);
		U8(a1, 0x40) = 0x28;
		U8(a1, 0x41) = 0;
		U8(a1, 0x42) = 0xA;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl d_8BDE50(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0xE)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl d_8B8520(uint32_t a1, uint32_t a2)
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
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = d_8B8680(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
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
				MEM<uint32_t>(game_addr(PACKET_CURSOR)) = d_8B8680(h, MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44, 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
			}
		}
		const uint32_t top = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 0x68;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = top;
		return top;
	}

	uint32_t __cdecl d_8BA960(uint32_t a1)
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

	uint32_t __cdecl d_8BBC10(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_014_ParticlePoolPtr));
		int32_t idx = MEM<int16_t>(game_addr(game::MAG_014_ParticleCursor));
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
			MEM<uint16_t>(game_addr(game::MAG_014_ParticleCursor)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::MAG_014_ParticleCursor)) = 0;
		return slot;
	}

	uint32_t __cdecl d_8BC590(uint32_t a1, uint32_t a2)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_014_ActorPoolPtr));
		int32_t i = MEM<int16_t>(game_addr(game::MAG_014_ActorCursor));
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
			MEM<uint16_t>(game_addr(game::MAG_014_ActorCursor)) = 0;
		else
			MEM<uint16_t>(game_addr(game::MAG_014_ActorCursor)) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl d_8B6A40(uint32_t a1)
	{
		g_mod = &MOD_014;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::MAG_014_ScratchSP)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_273AFE8)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_014_Death_Master_State0_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_014_Death_Master_State1_Advance);
		states[2] = game_addr(game::MAG_014_Death_Master_State2_CarveActorPools);
		states[3] = game_addr(game::MAG_014_Death_Master_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_014_Death_Master_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_014_Death_Master_State5_Advance);
		states[6] = game_addr(game::MAG_014_Death_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_014_Death_Master_State7_RequestBattleSync);
		states[8] = game_addr(game::MAG_014_Death_Master_State8_WaitBattleSyncDone);
		states[9] = game_addr(game::MAG_014_Death_Master_State9_Finish);
		states[10] = game_addr(game::MAG_014_Death_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_27419EC));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_2740B44));
			MEM<uint32_t>(game_addr(game::MAG_014_PacketCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_273AFE4)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_27419E8));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_2740B40));
			MEM<uint32_t>(game_addr(game::MAG_014_PacketCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_273AFE4)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_274285C)) = 0;
		MEM<uint16_t>(game_addr(game::unk_2742820)) = 0;
		const uint32_t queues[7] = { game_addr(game::MAG_014_EmitterQueue), game_addr(game::MAG_014_DirectorQueue), game_addr(game::MAG_014_CreatureQueue), game_addr(game::MAG_014_ActorSystemQueue),
			game_addr(game::MAG_014_StageWobbleQueue), game_addr(game::MAG_014_DarkSparkQueue), game_addr(game::MAG_014_UnusedQueue) };
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

	uint32_t __cdecl d_8B7920(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			a_73F990(a1);
			d_8B8430(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl d_8B7B00(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_014_ActorStatePtr));
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

	uint32_t __cdecl d_8B7D80(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_014_ActorStatePtr));
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

	uint32_t __cdecl d_8B8430(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::MAG_014_ActorStatePtr), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			d_8BD330();
			d_8BA9E0();
			d_8BA8A0();
			d_8B84C0();
			sys = U32(game_addr(game::MAG_014_ActorStatePtr), 0);
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
		U16(game_addr(game::unk_274285C), 0) = (uint16_t)(U16(game_addr(game::unk_274285C), 0) + c14);
		U16(game_addr(game::unk_2742820), 0) = (uint16_t)(U16(game_addr(game::unk_2742820), 0) + c16);
		return done;
	}

	uint32_t __cdecl d_8B84C0(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_014_ActorStatePtr), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::MAG_014_ActorStatePtr), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					d_8B8520(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl d_8B8680(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		static const Draw lists[8] = { d_8B87B0, d_8B89D0, d_8B8C60, d_8B9150, a_73D770, a_73D9C0, d_8B9C70, d_8BA1D0 };
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

	uint32_t __cdecl d_8B87B0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl d_8B89D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl d_8B8C60(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl d_8B9150(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl d_8B9C70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl d_8BA1D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl d_8BA8A0(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::MAG_014_ActorStatePtr)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::MAG_014_ActorStatePtr));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						d_8BA960(act);
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
							d_8BA960(act);
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

	uint32_t __cdecl d_8BA9E0(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_014_ActorStatePtr), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				d_8BBCB0(act);
			else if (type == 1)
				d_8BAA20(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl d_8BAA20(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::MAG_014_ActorStatePtr));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (d_8BBB60(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::MAG_014_ActorStatePtr));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl d_8BBA50(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::MAG_014_ActorStatePtr), 0);
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
				dx::sub_45E0B0(in2 + delta);
				dx::sub_45E9D0();
				x::set_dword_1CA8A30(U32(w_blk, 4));
				dx::sub_45E0B0(in2);
				dx::sub_45EBF0();
				x::GTE_StoreIR123(out);
				out += 8;
				in2 += 8;
			} while (--n != 0);
		}
		x::Field_Free(8);
		return 0;
	}

	uint32_t __cdecl d_8BBB60(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				d_8BBBE0(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			d_8BBBE0(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			d_8BBBE0(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl d_8BBBE0(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return d_8BBC10(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl d_8BBCB0(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_014_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_014_Actor_InterpolatePosition), a1);
		d_8BBD30(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl d_8BBD30(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::MAG_014_ActorStatePtr), 0);
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
				d_8BBDB0(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			d_8BBDB0(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl d_8BBDB0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::MAG_014_ScratchSP), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::MAG_014_ScratchSP), 0) = sp;
		uint32_t node = d_8BC590(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::MAG_014_ActorStatePtr), 0);
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
		d_8BD2A0(node, desc);
		U32(game_addr(game::MAG_014_ScratchSP), 0) = U32(game_addr(game::MAG_014_ScratchSP), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl d_8BC7B0(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::MAG_014_ActorStatePtr), 0);
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
				uint32_t g = U32(game_addr(game::MAG_014_ActorStatePtr), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::MAG_014_ActorStatePtr), 0);
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

	uint32_t __cdecl d_8BD2A0(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			d_8BBC10(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			d_8BBC10(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			d_8BBC10(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			d_8BBC10(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl d_8BD330(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::MAG_014_ActorStatePtr));
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
					d_8BBC10(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::MAG_014_ActorStatePtr));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				d_8BBC10(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::MAG_014_ActorStatePtr));
			}
		}
		return 0;
	}

	uint32_t __cdecl d_8BD3C0(uint32_t a1)
	{
		if (d_8B8430(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl d_8BD3F0(uint32_t a1, uint32_t a2)
	{
		const uint32_t node = a1;
		a_733950(node + 0x30, U16(node, 0x13A), a2);
		return 0;
	}

	uint32_t __cdecl d_8BD510(uint32_t a1)
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

	uint32_t __cdecl d_8BDFC0(uint32_t a1)
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

	uint32_t __cdecl d_8BE070(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_162B860));
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
		{ game::MAG_014_Death_Master_State0_Advance, (void *)a_73A0D0, "014 MAG_014_sub_8B6BC0" },
		{ game::MAG_014_Death_Master_State1_Advance, (void *)a_73A0D0, "014 MAG_014_sub_8B6BD0" },
		{ game::MAG_014_Death_Master_State3_SpawnEmitter, (void *)a_73A170, "014 MAG_014_sub_8B6C60" },
		{ game::Death_EmitterTask, (void *)a_73A1A0, "014 MAG_014_sub_8B6C90" },
		{ game::MAG_014_DrawBattleModel, (void *)a_746C10, "014 sub_8B70B0" },
		{ game::MAG_014_SpawnChildTask, (void *)a_73C100, "014 sub_8B7870" },
		{ game::MAG_014_AuraActorSystemTask, (void *)a_73A380, "014 sub_8B78C0" },
		{ game::MAG_014_DirectorSetup, (void *)a_73F990, "014 sub_8B7950" },
		{ game::MAG_014_Director_StartPositions, (void *)a_73FC20, "014 sub_8B7BE0" },
		{ game::MAG_014_Director_Mode3Setup, (void *)a_73FE90, "014 sub_8B7E50" },
		{ game::MAG_014_Director_TargetPositions, (void *)a_73FFB0, "014 sub_8B7F70" },
		{ game::MAG_014_RelocateActorData, (void *)a_740210, "014 sub_8B81D0" },
		{ game::MAG_014_EmitG3, (void *)a_73D770, "014 sub_8B9740" },
		{ game::MAG_014_EmitG4, (void *)a_73D9C0, "014 sub_8B9990" },
		{ game::MAG_014_ActorMotion, (void *)a_740910, "014 sub_8BAAB0" },
		{ game::MAG_014_DampValue, (void *)a_740F70, "014 sub_8BB110" },
		{ game::MAG_014_DampVector, (void *)a_740FC0, "014 sub_8BB160" },
		{ game::MAG_014_Instance_AdvanceTexFrame, (void *)a_741050, "014 sub_8BB1F0" },
		{ game::MAG_014_Instance_BuildTransform, (void *)a_741120, "014 sub_8BB2C0" },
		{ game::MAG_014_Particle_LifeCountdown, (void *)a_741B60, "014 sub_8BBD00" },
		{ game::MAG_014_RandRange, (void *)a_742290, "014 au_re__rand_68" },
		{ game::MAG_014_RandRangeWide, (void *)a_7422C0, "014 sub_8BC460" },
		{ game::MAG_014_RandVec3, (void *)a_742300, "014 sub_8BC4A0" },
		{ game::MAG_014_JitterAngles3, (void *)a_742350, "014 sub_8BC4F0" },
		{ game::MAG_014_RandAngleJitter, (void *)a_7423A0, "014 au_re__rand_68_0" },
		{ game::MAG_014_GetWord8Minus1, (void *)a_7424B0, "014 sub_8BC650" },
		{ game::MAG_014_Particle_AttachToOwner, (void *)a_7424C0, "014 sub_8BC660" },
		{ game::MAG_014_ParticleList_Append, (void *)a_742CA0, "014 sub_8BCE40" },
		{ game::MAG_014_ParticleList_Unlink, (void *)a_742CD0, "014 sub_8BCE70" },
		{ game::MAG_014_Particle_Init, (void *)a_742D00, "014 sub_8BCEA0" },
		{ game::MAG_014_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "014 sub_8BD050" },
		{ game::MAG_014_Particle_PlaceAtPoint, (void *)a_742FF0, "014 sub_8BD190" },
		{ game::MAG_014_AuraActorSystem_State2_Empty, (void *)a_73A6D0, "014 nullsub_1798" },
		{ game::MAG_014_SetEntityColourTint, (void *)a_733950, "014 sub_8BD410" },
		{ game::MAG_014_StageWobbleInTask, (void *)a_73A380, "014 sub_8BD580" },
		{ game::MAG_014_StageWobbleIn_State2_Empty, (void *)a_73A6D0, "014 nullsub_1799" },
		{ game::Death_SparkBurstTask, (void *)a_73A380, "014 sub_8BD840" },
		{ game::MAG_014_Death_Spark_State2_Empty, (void *)a_73A6D0, "014 nullsub_1800" },
		{ game::MAG_014_Death_SparkBurst_State2_Empty, (void *)a_73A6D0, "014 nullsub_1801" },
		{ game::MAG_014_StageWobbleOutTask, (void *)a_73A380, "014 sub_8BDF40" },
		{ game::MAG_014_StageWobbleOut_State2_Empty, (void *)a_73A6D0, "014 nullsub_1802" },
		{ game::MAG_014_Death_Reaper_State5_Empty, (void *)a_73A6D0, "014 nullsub_1803" },
		{ game::MAG_014_Death_Director_State2_Finish, (void *)a_7475A0, "014 sub_8BE020" },
		{ game::MAG_014_Death_Director_State3_Empty, (void *)a_73A6D0, "014 nullsub_1804" },
		{ game::MAG_014_St_Empty, (void *)a_73A6D0, "014 nullsub_1805" },
		{ game::MAG_014_St_Empty_2, (void *)a_73A6D0, "014 nullsub_1806" },
		{ game::MAG_014_Death_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "014 MAG_014_sub_8BE0B0" },
		{ game::MAG_014_Death_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "014 MAG_014_sub_8BE0D0" },
		{ game::MAG_014_Death_Emitter_State4_Empty, (void *)a_73A6D0, "014 nullsub_1807" },
		{ game::MAG_014_Death_Master_State4_NextTargetLoop, (void *)a_747500, "014 MAG_014_sub_8BE100" },
		{ game::MAG_014_Death_Master_State5_Advance, (void *)a_73A0D0, "014 MAG_014_sub_8BE140" },
		{ game::MAG_014_Death_Master_State6_WaitQueuesEmpty, (void *)a_747550, "014 MAG_014_sub_8BE150" },
		{ game::MAG_014_Death_Master_State7_RequestBattleSync, (void *)a_747560, "014 MAG_014_sub_8BE160" },
		{ game::MAG_014_Death_Master_State8_WaitBattleSyncDone, (void *)a_747590, "014 MAG_014_sub_8BE190" },
		{ game::MAG_014_Death_Master_State9_Finish, (void *)a_7475A0, "014 MAG_014_sub_8BE1A0" },
		{ game::MAG_014_Death_Master_State10_Empty, (void *)a_73A6D0, "014 nullsub_1797" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_014_Master_ClearActorPools, (void *)d_8B6C20, "d_8B6C20" },
		{ game::MAG_014_Death_Master_State2_CarveActorPools, (void *)d_8B6BE0, "d_8B6BE0" },
		{ game::MAG_014_Reaper_PlaceAndFaceTarget, (void *)d_8B6D40, "d_8B6D40" },
		{ game::MAG_014_Death_Emitter_State0_SpawnDirector, (void *)d_8B6D10, "d_8B6D10" },
		{ game::MAG_014_Director_PhaseBookkeeping, (void *)d_8B6EA0, "d_8B6EA0" },
		{ game::Death_DirectorTask, (void *)d_8B6E30, "d_8B6E30" },
		{ game::MAG_014_UploadReaperTIMs, (void *)d_8B6EF0, "d_8B6EF0" },
		{ game::MAG_014_SpawnReaperCreature, (void *)d_8B6F30, "d_8B6F30" },
		{ game::Death_RenderDarkModel, (void *)d_8B7390, "d_8B7390" },
		{ game::Death_ReaperDarkDraw, (void *)d_8B7290, "d_8B7290" },
		{ game::Death_ReaperDraw, (void *)d_8B7030, "d_8B7030" },
		{ game::Death_ReaperTask, (void *)d_8B6F70, "d_8B6F70" },
		{ game::MAG_014_Reaper_ClearDataFlag20, (void *)d_8BD700, "d_8BD700" },
		{ game::Death_InitDarkDrawContext, (void *)d_8BD6A0, "d_8BD6A0" },
		{ game::MAG_014_Reaper_InitDarkDraw, (void *)d_8BD670, "d_8BD670" },
		{ game::Death_ReaperAppear, (void *)d_8B7780, "d_8B7780" },
		{ game::MAG_014_Death_Reaper_State1_AppearAnimDone, (void *)d_8BD710, "d_8BD710" },
		{ game::Death_ReaperStrike, (void *)d_8BD760, "d_8BD760" },
		{ game::Death_ReaperVanish, (void *)d_8BDE90, "d_8BDE90" },
		{ game::MAG_014_Death_Reaper_State4_StartWobbleOutFinish, (void *)d_8BDEF0, "d_8BDEF0" },
		{ game::MAG_014_StageWobbleIn_State0_ClearAtTick1C, (void *)d_8BD5E0, "d_8BD5E0" },
		{ game::MAG_014_StageWobbleIn_State1_RampUp, (void *)d_8BD620, "d_8BD620" },
		{ game::MAG_014_StageWobbleOut_State0_SetLevel600, (void *)d_8BDFA0, "d_8BDFA0" },
		{ game::MAG_014_Death_SparkBurst_State0_AtTargetBone, (void *)d_8BD8A0, "d_8BD8A0" },
		{ game::MAG_014_Death_SparkBurst_State1_SpawnSpark, (void *)d_8BD8E0, "d_8BD8E0" },
		{ game::Death_DrawSpark, (void *)d_8BDD20, "d_8BDD20" },
		{ game::Death_SparkTask, (void *)d_8BDBD0, "d_8BDBD0" },
		{ game::MAG_014_Death_Spark_State0_Init, (void *)d_8BDE20, "d_8BDE20" },
		{ game::MAG_014_Death_Spark_State1_EndAtTick14, (void *)d_8BDE50, "d_8BDE50" },
		{ game::Death_DrawPrimActor, (void *)d_8B8520, "d_8B8520" },
		{ game::MAG_014_ActSys_DrawActorSprite, (void *)d_8BA960, "d_8BA960" },
		{ game::MAG_014_AllocParticle, (void *)d_8BBC10, "d_8BBC10" },
		{ game::MAG_014_ActSys_AllocParticleSlot, (void *)d_8BC590, "d_8BC590" },
		{ game::MAG_014_DEATH_Tick, (void *)d_8B6A40, "d_8B6A40" },
		{ game::MAG_014_AuraActorSystem_State0_StartActorSystemAtFrame, (void *)d_8B7920, "d_8B7920" },
		{ game::MAG_014_ResetTrailHistory, (void *)d_8B7B00, "d_8B7B00" },
		{ game::MAG_014_FillTrailRows, (void *)d_8B7D80, "d_8B7D80" },
		{ game::MAG_014_ActSys_Step, (void *)d_8B8430, "d_8B8430" },
		{ game::MAG_014_ActSys_DrawPass, (void *)d_8B84C0, "d_8B84C0" },
		{ game::Death_RenderPrimModel, (void *)d_8B8680, "d_8B8680" },
		{ game::MAG_014_PrimList_FlatTri, (void *)d_8B87B0, "d_8B87B0" },
		{ game::MAG_014_PrimList_FlatQuad, (void *)d_8B89D0, "d_8B89D0" },
		{ game::MAG_014_PrimList_TexTri, (void *)d_8B8C60, "d_8B8C60" },
		{ game::MAG_014_PrimList_TexQuad, (void *)d_8B9150, "d_8B9150" },
		{ game::MAG_014_PrimList_GT3Scroll, (void *)d_8B9C70, "d_8B9C70" },
		{ game::MAG_014_PrimList_GT4Scroll, (void *)d_8BA1D0, "d_8BA1D0" },
		{ game::MAG_014_ActSys_DrawSpritePass, (void *)d_8BA8A0, "d_8BA8A0" },
		{ game::MAG_014_ActSys_UpdatePass, (void *)d_8BA9E0, "d_8BA9E0" },
		{ game::MAG_014_ActorStep, (void *)d_8BAA20, "d_8BAA20" },
		{ game::MAG_014_MorphVertexBlock, (void *)d_8BBA50, "d_8BBA50" },
		{ game::MAG_014_Instance_EndOfLifeTest, (void *)d_8BBB60, "d_8BBB60" },
		{ game::MAG_014_Instance_SpawnFollowUp, (void *)d_8BBBE0, "d_8BBBE0" },
		{ game::MAG_014_ActSys_UpdateEmitter, (void *)d_8BBCB0, "d_8BBCB0" },
		{ game::MAG_014_ActSys_EmitterSpawnStep, (void *)d_8BBD30, "d_8BBD30" },
		{ game::MAG_014_ActSys_SpawnParticles, (void *)d_8BBDB0, "d_8BBDB0" },
		{ game::MAG_014_Actor_InterpolatePosition, (void *)d_8BC7B0, "d_8BC7B0" },
		{ game::MAG_014_SpawnSubEffects, (void *)d_8BD2A0, "d_8BD2A0" },
		{ game::MAG_014_Director_SpawnGroupEffects, (void *)d_8BD330, "d_8BD330" },
		{ game::MAG_014_AuraActorSystem_State1_StepActorSystem, (void *)d_8BD3C0, "d_8BD3C0" },
		{ game::MAG_014_Reaper_ApplyFadeTint, (void *)d_8BD3F0, "d_8BD3F0" },
		{ game::MAG_014_Reaper_SetAmbientColour, (void *)d_8BD510, "d_8BD510" },
		{ game::MAG_014_StageWobbleOut_State1_FadeOut, (void *)d_8BDFC0, "d_8BDFC0" },
		{ game::MAG_014_Death_Emitter_State1_ApplyDamage, (void *)d_8BE070, "d_8BE070" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag014_death()
	{
		act::register_module(14);
		for (const act::death::ModPort *p = act::death::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(14, p->addr, p->port, p->name);
		for (const act::death::ModPort *p = act::death::PORTS; p->addr; p++)
			act::register_module_port(14, p->addr, p->port, p->name);
	}
}

