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
namespace mguard_quistis
{
	static const Mod MOD_073 = { "mightyguard_quistis", 73, game::MAG_073_MIGHTY_GUARD_QUISTIS_FL, game::_keep_,
		{ {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_073_MIGHTY_GUARD_QUISTIS_g_PoolCounter3, game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorPool, {}, {}, {}, {}, {}, {}, game::MAG_073_MIGHTY_GUARD_QUISTIS_g_CameraPtr, {}, {}, {}, {}, {}, {}, {}, game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorSystemQueue, {}, game::MAG_073_MIGHTY_GUARD_QUISTIS_g_EmitterQueue, {}, {}, {}, {}, {}, game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorCount16, {}, {}, game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorCount14, {}, game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ScratchSP, {}, game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorState_Setup, game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorSystem_Step, game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorSystem_DrawModels, game::MAG_073_MIGHTY_GUARD_QUISTIS_Actor_DrawModel, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorSystem_DrawSprites, game::MAG_073_MIGHTY_GUARD_QUISTIS_ActSys_UpdatePass, game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorStep, game::MAG_073_MIGHTY_GUARD_QUISTIS_ActSys_UpdateEmitter, game::MAG_073_MIGHTY_GUARD_QUISTIS_Particle_LifeCountdown, game::MAG_073_MIGHTY_GUARD_QUISTIS_ActSys_EmitterSpawnStep, {}, game::MAG_073_MIGHTY_GUARD_QUISTIS_RandVec3, game::MAG_073_MIGHTY_GUARD_QUISTIS_JitterAngles3, game::MAG_073_MIGHTY_GUARD_QUISTIS_ActSys_AllocParticleSlot, game::MAG_073_MIGHTY_GUARD_QUISTIS_GetWord8Minus1, game::MAG_073_MIGHTY_GUARD_QUISTIS_Particle_Init, game::MAG_073_MIGHTY_GUARD_QUISTIS_SpawnSubEffects, game::MAG_073_MIGHTY_GUARD_QUISTIS_Director_SpawnGroupEffects, {}, {}, {}, {}, {}, game::MAG_073_MIGHTY_GUARD_QUISTIS_EmitterTask, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_073_MIGHTY_GUARD_QUISTIS_Actor_DrawSprite, game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorMotion, game::MAG_073_MIGHTY_GUARD_QUISTIS_DampValue, game::MAG_073_MIGHTY_GUARD_QUISTIS_DampVector, game::MAG_073_MIGHTY_GUARD_QUISTIS_Instance_AdvanceTexFrame, game::MAG_073_MIGHTY_GUARD_QUISTIS_Actor_InstanceTransform, game::MAG_073_MIGHTY_GUARD_QUISTIS_MorphVertexBlock, game::MAG_073_MIGHTY_GUARD_QUISTIS_Instance_EndOfLifeTest, game::MAG_073_MIGHTY_GUARD_QUISTIS_Instance_SpawnFollowUp, game::MAG_073_MIGHTY_GUARD_QUISTIS_ParticlePool_Alloc, game::MAG_073_MIGHTY_GUARD_QUISTIS_RandRange, game::MAG_073_MIGHTY_GUARD_QUISTIS_RandRangeWide, game::MAG_073_MIGHTY_GUARD_QUISTIS_RandRange2, game::MAG_073_MIGHTY_GUARD_QUISTIS_Particle_AttachToOwner, game::MAG_073_MIGHTY_GUARD_QUISTIS_Actor_InterpolatePosition, game::MAG_073_MIGHTY_GUARD_QUISTIS_ParticleList_Append, game::MAG_073_MIGHTY_GUARD_QUISTIS_ParticleList_Unlink, game::MAG_073_MIGHTY_GUARD_QUISTIS_Particle_PlaceAnchor, game::MAG_073_MIGHTY_GUARD_QUISTIS_Particle_PlaceAtPoint, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::MAG_073_MIGHTY_GUARD_QUISTIS_g_PacketCursor;
	static constexpr game::Id SCRATCH_SP = game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ScratchSP;
	static constexpr game::Id ACTOR_STATE = game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState;
	static constexpr game::Id BIND_Block = game::MAG_073_MIGHTY_GUARD_QUISTIS_g_CasterBindBlock;
	static constexpr game::Id COM_Anim = game::MAG_073_MIGHTY_GUARD_QUISTIS_ComAnimSet;
	static constexpr game::Id CAMERA_PTR = game::MAG_073_MIGHTY_GUARD_QUISTIS_g_CameraPtr;
	static constexpr game::Id ACTOR_DATA = game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorData;
	static const void *SOUND() { return (const void *)game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Sound); }
	static constexpr game::Id Q_EMITTER = game::MAG_073_MIGHTY_GUARD_QUISTIS_g_EmitterQueue, Q_ACTORS = game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorSystemQueue, Q_STAGE = game::MAG_073_MIGHTY_GUARD_QUISTIS_g_StageQueue;
	static constexpr game::Id ORIG_Master = game::MAG_073_MIGHTY_GUARD_QUISTIS_MasterTask;
	static constexpr game::Id ORIG_Emitter = game::MAG_073_MIGHTY_GUARD_QUISTIS_EmitterTask;
	static constexpr game::Id ORIG_ActorSystem = game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorSystemTask;
	static constexpr game::Id ORIG_Darken = game::MAG_073_MIGHTY_GUARD_QUISTIS_StageDarkenInTask;
	static constexpr game::Id ORIG_Lighten = game::MAG_073_MIGHTY_GUARD_QUISTIS_StageDarkenOutTask;

	namespace gx
	{
		static inline uint32_t BS_BindEntityModelFromComData(uint32_t entity, uint32_t out, uint32_t data) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
		static inline uint32_t QueueChainTransformation(uint32_t entity, uint32_t id) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
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

	static inline void t1_next_state(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }
	static inline void t4_next_state(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }
	static inline void next_state(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }
	static inline int32_t h4_div65536(int32_t v) { return v / 65536; }
	static void h4_world_to_view(uint32_t node, uint32_t src)
	{
		U32(node, 0xA0) = (uint32_t)h4_div65536(S32(src, 0));
		U32(node, 0xA4) = (uint32_t)h4_div65536(S32(src, 4));
		U32(node, 0xA8) = (uint32_t)h4_div65536(S32(src, 8));
		x::GTE_SetRotMatrix(MEM<uint32_t>(game_addr(CAMERA_PTR)));
		x::GTE_LoadIRFromMatrixColumn(node + 0x8C);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(node + 0xAC);
		x::GTE_LoadIRFromMatrixColumn(node + 0x8E);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(node + 0xAE);
		x::GTE_LoadIRFromMatrixColumn(node + 0x90);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(node + 0xB0);
		x::GTE_SetTransVector(MEM<uint32_t>(game_addr(CAMERA_PTR)));
		x::GTE_LoadV0FromDwords(node + 0xA0);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(node + 0xC0);
	}
	static void h4_build_rot(uint32_t node, int32_t sub, uint32_t parent)
	{
		uint32_t m = node + 0x8C;
		switch ((uint32_t)sub)
		{
			case 0:
				if (parent != 0)
					ff8fx::copy<(32)>((void *)m, (void *)(parent + 0x2C));
				else
					x::MAG_022_sub_8DD770(m);
				if (U16(node, 0xD0) != 0) x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0xD0));
				if (U16(node, 0xCC) != 0) x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(node, 0xCC));
				if (U16(node, 0xCE) != 0) x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(node, 0xCE));
				break;
			case 1:
				if (parent != 0)
					ff8fx::copy<(32)>((void *)m, (void *)(parent + 0x2C));
				else
					x::MAG_022_sub_8DD770(m);
				if (U16(node, 0xCE) != 0) x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(node, 0xCE));
				if (U16(node, 0xCC) != 0) x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(node, 0xCC));
				if (U16(node, 0xD0) != 0) x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0xD0));
				break;
			case 2:
				x::UnpackRotationMatrix(MEM<uint32_t>(game_addr(CAMERA_PTR)), m);
				if (U16(node, 0xD0) != 0) x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0xD0));
				if (U16(node, 0xCC) != 0) x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(node, 0xCC));
				if (U16(node, 0xCE) != 0) x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(node, 0xCE));
				break;
			case 3:
				x::UnpackRotationMatrix(MEM<uint32_t>(game_addr(CAMERA_PTR)), m);
				if (U16(node, 0xCE) != 0) x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(node, 0xCE));
				if (U16(node, 0xCC) != 0) x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(node, 0xCC));
				if (U16(node, 0xD0) != 0) x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0xD0));
				break;
			default:
				break;
		}
	}

	uint32_t __cdecl mq_7CBEE0(uint32_t a1);
	uint32_t __cdecl mq_7CC040(uint32_t a1);
	uint32_t __cdecl mq_7CC0C0(void);
	uint32_t __cdecl mq_7CC1A0(uint32_t a1);
	uint32_t __cdecl mq_7CC220(uint32_t a1);
	uint32_t __cdecl mq_7CC2B0(uint32_t a1);
	uint32_t __cdecl mq_7CC390(uint32_t a1);
	uint32_t __cdecl mq_7CC3C0(uint32_t a1);
	uint32_t __cdecl mq_7CC550(uint32_t a1);
	uint32_t __cdecl mq_7CC630(void);
	uint32_t __cdecl mq_7CC790(void);
	uint32_t __cdecl mq_7CC980(void);
	uint32_t __cdecl mq_7CCDF0(uint32_t a1);
	uint32_t __cdecl mq_7CCE80(void);
	uint32_t __cdecl mq_7CCEE0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl mq_7CD090(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl mq_7CD1C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl mq_7CD3E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl mq_7CD670(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl mq_7CDAF0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl mq_7CE5A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl mq_7CEAA0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl mq_7CF0F0(void);
	uint32_t __cdecl mq_7CF1B0(uint32_t a1);
	uint32_t __cdecl mq_7CF230(void);
	uint32_t __cdecl mq_7CF270(uint32_t a1);
	uint32_t __cdecl mq_7D02A0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl mq_7D03B0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl mq_7D0430(uint32_t a1, uint32_t a2);
	uint32_t __cdecl mq_7D0460(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl mq_7D0500(uint32_t a1);
	uint32_t __cdecl mq_7D0580(uint32_t a1);
	uint32_t __cdecl mq_7D0600(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl mq_7D1000(uint32_t a1);
	uint32_t __cdecl mq_7D1AF0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl mq_7D1B80(void);
	uint32_t __cdecl mq_7D1C10(uint32_t a1);
	uint32_t __cdecl mq_7D1C40(uint32_t a1);
	uint32_t __cdecl mq_7D1C60(uint32_t a1);
	uint32_t __cdecl mq_7D1CB0(uint32_t a1);
	uint32_t __cdecl mq_7D1D00(uint32_t a1);
	uint32_t __cdecl mq_7D1D90(uint32_t a1);
	uint32_t __cdecl mq_7D1DB0(uint32_t a1);
}
}
}

namespace ff8fx
{
namespace act
{
namespace mguard_quistis
{
	uint32_t __cdecl mq_7CC040(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		uint32_t p = MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ArenaCursor));
		MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ParticlePool)) = p;
		p += 0xEC4;
		MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorPool)) = p;
		p += 0x10680;
		MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ArenaCursor)) = p;
		mq_7CC0C0();
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_STAGE), game_addr(ORIG_Darken), 0x40, a1);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)U8(a1, 0x2C) * 0x9C;
		gx::BS_BindEntityModelFromComData(ent, game_addr(BIND_Block), game_addr(COM_Anim));
		gx::QueueChainTransformation(ent, 1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl mq_7CC0C0(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ParticlePool)), 0xEC4);
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorPool)), 0x10680);
		MEM<uint16_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_PoolCounter1)) = 0;
		MEM<uint16_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_PoolCounter3)) = 0;
		MEM<uint16_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_PoolCounter2)) = 0;
		MEM<uint16_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_PoolCounter0)) = 0;
		return 0;
	}

	uint32_t __cdecl mq_7CC220(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Emitter_SpawnActorSystem);
		states[1] = game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Emitter_ReleaseMaster);
		states[2] = game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Emitter_ApplyResult);
		states[3] = game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Emitter_State3_Empty);
		x::MAG_001_CURE_Emitter_UpdatePos(node);
		x::MAG_001_CURE_Emitter_ComputeModelBounds(node);
		callp(states[S8(node, 0x29)], node);
		if (U16(node, 0x24) == 0x10)
			x::BdPlaySE(P(SOUND()), 0, 0x80);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl mq_7D1C60(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 0x2D)
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

	uint32_t __cdecl mq_7CC390(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < S16(node, 0x298))
			return 0;
		mq_7CC3C0(node);
		mq_7CCDF0(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl mq_7CC3C0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t data = U32(node, 0x30);
		const uint16_t w29a = U16(node, 0x29A);
		const uint16_t w29c = U16(node, 0x29C);
		uint32_t st = node + 0x34;
		const uint16_t caster = (uint16_t)U8(node, 0x2C);
		U16(st, 0x22) = w29a;
		U16(st, 0x24) = w29c;
		U32(st, 0x224) = data + 4;
		U32(st, 0x228) = data + 0x44;
		U32(st, 0x22C) = data + 0x84;
		const uint32_t relocated = U32(data, 0);
		MEM<uint32_t>(game_addr(ACTOR_STATE)) = st;
		U32(st, 0x230) = data + 0xC4;
		if (relocated == 0)
		{
			a_740210(data);
			st = MEM<uint32_t>(game_addr(ACTOR_STATE));
			U32(data, 0) = 1;
		}
		U16(st, 0x1E) = caster;
		U16(st, 0x1C) = 1;
		{
			const int32_t action = S8(node, 0x2A);
			const uint32_t tab = U32(U32(node, 0x0C), 4);
			const uint32_t rec = U32(tab + (uint32_t)(action * 5) * 4, 8);
			const int32_t ti = S8(node, 0x2B);
			U32(st, 4) = U8(rec, ti * 0x18);
		}
		U16(st, 0x1A) = U16(node, 0x29E);
		mq_7CC630();
		st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		if (U16(st, 0x24) == 4)
			mq_7CC550(node);
		mq_7CC980();
		st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		if (U16(st, 0x24) == 1)
		{
			mq_7CC790();
			st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		}
		if (U16(st, 0x24) == 3)
		{
			a_73FE90(node);
			st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		}
		const uint32_t e1 = game_addr(game::BattleEntitySlotData) + (uint32_t)mul32(S16(st, 0x1E), 0x9C);
		const uint32_t d0c = U32(e1, 0x1C);
		const uint32_t d10 = U32(e1, 0x20);
		const uint32_t e2 = game_addr(game::BattleEntitySlotData) + (uint32_t)mul32((int32_t)U32(st, 4), 0x9C);
		const uint32_t d14 = U32(e2, 0x1C);
		const int32_t dx = (int16_t)(uint16_t)((uint16_t)d0c - (uint16_t)d14);
		const int32_t dy = (int16_t)(uint16_t)((uint16_t)(d0c >> 16) - (uint16_t)(d14 >> 16));
		const int32_t dz = (int16_t)(uint16_t)((uint16_t)d10 - (uint16_t)U32(e2, 0x20));
		const int32_t sq = add32(add32(mul32(dx, dx), mul32(dz, dz)), mul32(dy, dy));
		const uint32_t dist = x::Sqrt((uint32_t)sq);
		st = MEM<uint32_t>(game_addr(ACTOR_STATE));
		U32(st, 0) = dist;
		return 0;
	}

	uint32_t __cdecl mq_7CCEE0(uint32_t a1, uint32_t a2)
	{
		const uint32_t act = a1;
		const uint32_t ctx = MEM<uint32_t>(game_addr(SCRATCH_SP)) - 0x68;
		U32(ctx, 0) = U32(act, 0x170);
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = ctx;
		U32(ctx, 0x14) = 0;
		if (U16(a2, 0x3A) == 0)
			U32(ctx, 0x14) = 0x30;
		const uint16_t col = U16(act, 0x1CE);
		if (col != 0)
		{
			U32(ctx, 8) = U32(act, 0x16C);
			const uint32_t fl = U32(ctx, 0x14);
			U32(ctx, 0xC) = (uint32_t)(int32_t)(int16_t)col;
			U32(ctx, 0x14) = fl | 0xC0;
		}
		const uint8_t def_idx = U8(act, 0x1D6);
		U32(ctx, 0x10) = 0;
		U16(ctx, 0x1E) = 0;
		U16(ctx, 0x1C) = 0;
		U16(ctx, 0x22) = 0x100;
		U16(ctx, 0x20) = 0x100;
		if (def_idx == 4 || def_idx == 5)
		{
			U16(ctx, 0x2A) = 0x100;
			const uint32_t st = MEM<uint32_t>(game_addr(ACTOR_STATE));
			U16(ctx, 0x24) = 0;
			U16(ctx, 0x26) = 0;
			U16(ctx, 0x28) = 0x80;
			const uint16_t scroll = (uint16_t)((uint8_t)(U8(st, 0x28) + 2) & 0x7F);
			U16(ctx, 0x1A) = 0;
			U16(st, 0x28) = scroll;
			U16(ctx, 0x18) = scroll;
		}
		else
		{
			U16(ctx, 0x18) = 0;
			U16(ctx, 0x1A) = 0;
			U16(ctx, 0x26) = 0;
			U16(ctx, 0x24) = 0;
			U16(ctx, 0x2A) = 0x100;
			U16(ctx, 0x28) = 0x100;
		}
		const int8_t copies = S8(act, 0x1D8);
		if (copies == 1)
		{
			const uint32_t m = act + 0xAC;
			x::GTE_SetRotMatrix(m);
			x::GTE_SetTransVector(m);
			const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
			const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = mq_7CD090(ctx, ot, 2, cursor);
		}
		else if (copies > 0)
		{
			const uint32_t m = act + 0xAC;
			uint32_t pos = act + 0x19E;
			int32_t i = 0;
			do
			{
				S32(act, 0xC0) = S16(pos, -2);
				S32(act, 0xC4) = S16(pos, 0);
				S32(act, 0xC8) = S16(pos, 2);
				x::GTE_SetRotMatrix(m);
				x::GTE_SetTransVector(m);
				const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
				const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
				const uint32_t r = mq_7CD090(ctx, ot, 2, cursor);
				i++;
				pos += 8;
				MEM<uint32_t>(game_addr(PACKET_CURSOR)) = r;
			} while (i < S8(act, 0x1D8));
		}
		const uint32_t top = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 0x68;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = top;
		return top;
	}

	uint32_t __cdecl mq_7D0460(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t pool = MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ParticlePool));
		int32_t idx = (int32_t)MEM<int16_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_PoolCounter1));
		uint32_t slot = 0;
		int32_t tries = 0;
		do
		{
			if (U8(pool + (uint32_t)(idx * 27) * 4, 0x69) == 0)
			{
				slot = pool + (uint32_t)(idx * 27) * 4;
				x::MAG_007_sub_8DCC00(slot, 0x6C);
				U8(slot, 0x6A) = (uint8_t)a2;
				const uint32_t st = MEM<uint32_t>(game_addr(ACTOR_STATE));
				U8(slot, 0x69) = 1;
				U16(st, 0x14) = (uint16_t)(U16(st, 0x14) + 1);
				U32(slot, 0x5C) = a1;
				U8(slot, 0x6B) = (uint8_t)a3;
				a_742CA0(slot, 0);
				break;
			}
			idx++;
			if (idx >= 0x22)
				idx = 0;
			tries++;
		} while (tries < 0x23);
		idx++;
		if (idx < 0x22)
			MEM<uint16_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_PoolCounter1)) = (uint16_t)idx;
		else
			MEM<uint16_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_PoolCounter1)) = 0;
		return slot;
	}

	uint32_t __cdecl mq_7D1CB0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (U8(node, 0x63) != 0)
			return node;
		const int8_t t = S8(node, 0x2B);
		if ((int32_t)t < (int32_t)S16(node, 0x5A) - 1)
		{
			U8(node, 0x2B) = (uint8_t)(t + 1);
			U8(node, 0x2E) = (uint8_t)(U8(node, 0x2E) + 1);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) - 1);
			return node;
		}
		U16(node, 0x60) = 0x23;
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return node;
	}

	uint32_t __cdecl mq_7CBEE0(uint32_t a1)
	{
		g_mod = &MOD_073;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_CameraPtr)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_State0_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_State1_Advance);
		states[2] = game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_AllocActorPools_BindCasterAnim);
		states[3] = game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_SpawnEmitter);
		states[4] = game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_TargetLoop);
		states[5] = game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_StartDarkenOut);
		states[6] = game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_State7_Advance);
		states[8] = game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_State8_Advance);
		states[9] = game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_State9_Finish);
		states[10] = game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ArenaB1));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_PacketArena1));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_FrameArenaCursor)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ArenaB0));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_PacketArena0));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_FrameArenaCursor)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorCount14)) = 0;
		MEM<uint16_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorCount16)) = 0;
		static constexpr game::Id queues[3] = { Q_EMITTER, Q_ACTORS, Q_STAGE };
		for (int i = 0; i < 3; i++)
			U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(queues[i])));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	uint32_t __cdecl mq_7CC1A0(uint32_t a1)
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

	uint32_t __cdecl mq_7CC2B0(uint32_t a1)
	{
		a_73C100(a1, game_addr(ORIG_ActorSystem), game_addr(ACTOR_DATA), 0xC, 0x2D, 0);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl mq_7CC550(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState));
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

	uint32_t __cdecl mq_7CC630(void)
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

	uint32_t __cdecl mq_7CC790(void)
	{
		const uint32_t arena = MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState));
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

	uint32_t __cdecl mq_7CC980(void)
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

	uint32_t __cdecl mq_7CCDF0(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			mq_7D1B80();
			mq_7CF230();
			mq_7CF0F0();
			mq_7CCE80();
			sys = U32(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState), 0);
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
		U16(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorCount14), 0) = (uint16_t)(U16(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorCount14), 0) + c14);
		U16(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorCount16), 0) = (uint16_t)(U16(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorCount16), 0) + c16);
		return done;
	}

	uint32_t __cdecl mq_7CCE80(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					mq_7CCEE0(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl mq_7CD090(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
			cur = mq_7CD1C0(ctx, a2, a3, a4);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = mq_7CD3E0(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = mq_7CD670(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = mq_7CDAF0(ctx, a2, a3, cur);
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
			cur = mq_7CE5A0(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			return mq_7CEAA0(ctx, a2, a3, cur);
		U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		return cur;
	}

	uint32_t __cdecl mq_7CD1C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl mq_7CD3E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl mq_7CD670(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl mq_7CDAF0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl mq_7CE5A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl mq_7CEAA0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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

	uint32_t __cdecl mq_7CF0F0(void)
	{
		uint32_t act = U32(MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						mq_7CF1B0(act);
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
							mq_7CF1B0(act);
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

	uint32_t __cdecl mq_7CF1B0(uint32_t a1)
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

	uint32_t __cdecl mq_7CF230(void)
	{
		uint32_t act = U32(U32(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				mq_7D0500(act);
			else if (type == 1)
				mq_7CF270(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl mq_7CF270(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (mq_7D03B0(act, bone) == 0)
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
			st = MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl mq_7D02A0(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState), 0);
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

	uint32_t __cdecl mq_7D03B0(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				mq_7D0430(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			mq_7D0430(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			mq_7D0430(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl mq_7D0430(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return mq_7D0460(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl mq_7D0500(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Particle_AttachToOwner), a1);
		else if (mode == 4)
			callp(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_Actor_InterpolatePosition), a1);
		mq_7D0580(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl mq_7D0580(uint32_t a1)
	{
		uint32_t sys = U32(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState), 0);
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
				mq_7D0600(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			mq_7D0600(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl mq_7D0600(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = MEM<uint32_t>(game_addr(SCRATCH_SP)) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = sp;
		uint32_t node = a_7387B0(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = MEM<uint32_t>(game_addr(ACTOR_STATE));
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
		mq_7D1AF0(node, desc);
		MEM<uint32_t>(game_addr(SCRATCH_SP)) = MEM<uint32_t>(game_addr(SCRATCH_SP)) + 0x50;
		return 0;
	}

	uint32_t __cdecl mq_7D1000(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState), 0);
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
				uint32_t g = U32(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState), 0);
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

	uint32_t __cdecl mq_7D1AF0(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			mq_7D0460(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			mq_7D0460(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			mq_7D0460(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			mq_7D0460(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl mq_7D1B80(void)
	{
		uint32_t dir = MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState));
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
					mq_7D0460(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				mq_7D0460(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_g_ActorState));
			}
		}
		return 0;
	}

	uint32_t __cdecl mq_7D1C10(uint32_t a1)
	{
		if (mq_7CCDF0(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl mq_7D1C40(uint32_t a1)
	{
		if (U16(a1, 0x24) == 0x14)
		{
			U8(U32(a1, 0x10), 0x63) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return a1;
	}

	uint32_t __cdecl mq_7D1D00(uint32_t a1)
	{
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) - 1);
		if (S16(a1, 0x60) > 0)
			return 0;
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_STAGE), game_addr(game::MAG_073_MIGHTY_GUARD_QUISTIS_StageDarkenOutTask), 0x40, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl mq_7D1D90(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x1C) = 0x400;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return a1;
	}

	uint32_t __cdecl mq_7D1DB0(uint32_t a1)
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
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_State0_Advance, (void *)a_73A0D0, "073 a_73A0D0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_State1_Advance, (void *)a_73A0D0, "073 a_73A0D0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_StageDarkenInTask, (void *)a_73A380, "073 a_73A380" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_StageDarkenIn_State0_ResetEntityRecords, (void *)a_7335D0, "073 a_7335D0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_StageDarkenIn_State2_Empty, (void *)a_73A6D0, "073 a_73A6D0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_SpawnEmitter, (void *)a_73A170, "073 a_73A170" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_SpawnActorSystemTask, (void *)a_73C100, "073 a_73C100" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorSystemTask, (void *)a_73A380, "073 a_73A380" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Director_Mode3Setup, (void *)a_73FE90, "073 a_73FE90" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorData_Relocate, (void *)a_740210, "073 a_740210" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_EmitG3, (void *)a_73D770, "073 a_73D770" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_EmitG4, (void *)a_73D9C0, "073 a_73D9C0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorMotion, (void *)a_740910, "073 a_740910" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_DampValue, (void *)a_740F70, "073 a_740F70" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_DampVector, (void *)a_740FC0, "073 a_740FC0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Instance_AdvanceTexFrame, (void *)a_741050, "073 a_741050" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Actor_InstanceTransform, (void *)a_741120, "073 a_741120" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Particle_LifeCountdown, (void *)a_741B60, "073 a_741B60" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_RandRange, (void *)a_742290, "073 a_742290" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_RandRangeWide, (void *)a_7422C0, "073 a_7422C0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_RandVec3, (void *)a_742300, "073 a_742300" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_JitterAngles3, (void *)a_742350, "073 a_742350" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_RandRange2, (void *)a_7423A0, "073 a_7423A0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActSys_AllocParticleSlot, (void *)a_7387B0, "073 a_7387B0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_GetWord8Minus1, (void *)a_7424B0, "073 a_7424B0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Particle_AttachToOwner, (void *)a_7424C0, "073 a_7424C0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ParticleList_Append, (void *)a_742CA0, "073 a_742CA0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ParticleList_Unlink, (void *)a_742CD0, "073 a_742CD0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Particle_Init, (void *)a_742D00, "073 a_742D00" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Particle_PlaceAnchor, (void *)a_742EB0, "073 a_742EB0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Particle_PlaceAtPoint, (void *)a_742FF0, "073 a_742FF0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorSystem_State2_Empty, (void *)a_73A6D0, "073 a_73A6D0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Emitter_State3_Empty, (void *)a_73A6D0, "073 a_73A6D0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_StageDarkenOutTask, (void *)a_73A380, "073 a_73A380" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_StageDarkenOut_State2_Empty, (void *)a_73A6D0, "073 a_73A6D0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_State6_WaitQueuesEmpty, (void *)a_747550, "073 a_747550" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_State7_Advance, (void *)a_73A0D0, "073 a_73A0D0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_State8_Advance, (void *)a_73A0D0, "073 a_73A0D0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_State9_Finish, (void *)a_7475A0, "073 a_7475A0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_State10_Empty, (void *)a_73A6D0, "073 a_73A6D0" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_MasterTask, (void *)mq_7CBEE0, "073 mq_7CBEE0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_AllocActorPools_BindCasterAnim, (void *)mq_7CC040, "073 mq_7CC040" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ClearActorPools, (void *)mq_7CC0C0, "073 mq_7CC0C0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_StageDarkenIn_Step, (void *)mq_7CC1A0, "073 mq_7CC1A0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_EmitterTask, (void *)mq_7CC220, "073 mq_7CC220" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Emitter_SpawnActorSystem, (void *)mq_7CC2B0, "073 mq_7CC2B0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorSystem_State0_Start, (void *)mq_7CC390, "073 mq_7CC390" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorState_Setup, (void *)mq_7CC3C0, "073 mq_7CC3C0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ResetTrailHistory, (void *)mq_7CC550, "073 mq_7CC550" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorState_CasterPoints, (void *)mq_7CC630, "073 mq_7CC630" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_FillTrailRows, (void *)mq_7CC790, "073 mq_7CC790" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorState_TargetPoints, (void *)mq_7CC980, "073 mq_7CC980" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorSystem_Step, (void *)mq_7CCDF0, "073 mq_7CCDF0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorSystem_DrawModels, (void *)mq_7CCE80, "073 mq_7CCE80" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Actor_DrawModel, (void *)mq_7CCEE0, "073 mq_7CCEE0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_PrimList_Dispatch, (void *)mq_7CD090, "073 mq_7CD090" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_PrimList_FlatTri, (void *)mq_7CD1C0, "073 mq_7CD1C0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_PrimList_FlatQuad, (void *)mq_7CD3E0, "073 mq_7CD3E0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_PrimList_TexTri, (void *)mq_7CD670, "073 mq_7CD670" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_PrimList_TexQuad, (void *)mq_7CDAF0, "073 mq_7CDAF0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_PrimList_GT3Scroll, (void *)mq_7CE5A0, "073 mq_7CE5A0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_PrimList_GT4, (void *)mq_7CEAA0, "073 mq_7CEAA0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorSystem_DrawSprites, (void *)mq_7CF0F0, "073 mq_7CF0F0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Actor_DrawSprite, (void *)mq_7CF1B0, "073 mq_7CF1B0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActSys_UpdatePass, (void *)mq_7CF230, "073 mq_7CF230" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorStep, (void *)mq_7CF270, "073 mq_7CF270" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_MorphVertexBlock, (void *)mq_7D02A0, "073 mq_7D02A0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Instance_EndOfLifeTest, (void *)mq_7D03B0, "073 mq_7D03B0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Instance_SpawnFollowUp, (void *)mq_7D0430, "073 mq_7D0430" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ParticlePool_Alloc, (void *)mq_7D0460, "073 mq_7D0460" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActSys_UpdateEmitter, (void *)mq_7D0500, "073 mq_7D0500" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActSys_EmitterSpawnStep, (void *)mq_7D0580, "073 mq_7D0580" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Emitter_SpawnParticles, (void *)mq_7D0600, "073 mq_7D0600" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Actor_InterpolatePosition, (void *)mq_7D1000, "073 mq_7D1000" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_SpawnSubEffects, (void *)mq_7D1AF0, "073 mq_7D1AF0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Director_SpawnGroupEffects, (void *)mq_7D1B80, "073 mq_7D1B80" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_ActorSystem_State1_StepActorSystem, (void *)mq_7D1C10, "073 mq_7D1C10" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Emitter_ReleaseMaster, (void *)mq_7D1C40, "073 mq_7D1C40" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Emitter_ApplyResult, (void *)mq_7D1C60, "073 mq_7D1C60" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_TargetLoop, (void *)mq_7D1CB0, "073 mq_7D1CB0" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_Master_StartDarkenOut, (void *)mq_7D1D00, "073 mq_7D1D00" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_StageDarkenOut_State0_SetAmplitude400, (void *)mq_7D1D90, "073 mq_7D1D90" },
		{ game::MAG_073_MIGHTY_GUARD_QUISTIS_StageDarkenOut_State1_FadeOut, (void *)mq_7D1DB0, "073 mq_7D1DB0" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag073_mighty_guard_quistis()
	{
		act::register_module(73);
		for (const act::mguard_quistis::ModPort *p = act::mguard_quistis::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(73, p->addr, p->port, p->name);
		for (const act::mguard_quistis::ModPort *p = act::mguard_quistis::PORTS; p->addr; p++)
			act::register_module_port(73, p->addr, p->port, p->name);
	}
}

