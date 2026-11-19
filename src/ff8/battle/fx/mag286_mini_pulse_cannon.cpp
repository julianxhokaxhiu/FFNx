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

// Effect 286: Mini Pulse Cannon

#include "mag_common.h"

namespace ff8fx
{
namespace minipulse286
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_2507CAC)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::unk_2507CB0)); }
	inline TaskQueue *QSparks() { return (TaskQueue *)game_addr(game::unk_2507CB8); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::unk_2507CC8); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_2507CFC)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_2507D00)); }
	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	static constexpr game::Id ORIG_MasterTask = game::MAG_286_sub_679EC0;
	static constexpr game::Id ORIG_MainTask = game::MAG_286_sub_67A070;
	static constexpr game::Id ORIG_SparkTask = game::MAG_286_sub_67A7C0;
	static constexpr game::Id ORIG_PrimCallback = game::MAG_286_sub_67A4E0;
	static const uint32_t POOL_Main = 0x480;
	static const uint32_t MORPH = 0xB54;
	static const uint32_t ARENA_A = 0xFD4, ARENA_B = 0x10FD4;
	static constexpr game::Id MODEL_Muzzle = game::unk_107DD94;
	static constexpr game::Id MODEL_Beam = game::unk_10809EC;
	static constexpr game::Id MODEL_Impact = game::unk_1078BC8;
	static constexpr game::Id MODEL_Ground = game::unk_107D004;
	static constexpr game::Id SEQ_Spark = game::unk_10789A8;
	static const int16_t *MUZZLE() { return (const int16_t *)game_addr(game::unk_1084AE0); }
	static const void *SOUND_Shot() { return (const void *)game_addr(game::unk_10847AC); }

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrimSet(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_063_RenderPrimSet))(h, ot, mode, cursor); }
	inline int32_t BuildOrthonormalBasis(int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline void RotY(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint8_t *ent;
		int16_t pos[4];
		uint8_t muzzle[0x22C];
		uint8_t beam[0x158];
		uint8_t impact[0x2A4];
		uint8_t ground[0x90];
	};
	struct SparkNode
	{
		TaskNode hdr;
		uint32_t seq;
		int32_t shadow;
		int16_t pos[3];
		int16_t counter;
		int16_t vel[3];
		int16_t life;
	};
	struct PrimArg
	{
		Mat4x3 m;
		int16_t depth;
		int16_t scroll;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(MainNode) == 0x6D4 && sizeof(SparkNode) == 0x24, "Mini Pulse Cannon nodes");
	static_assert(sizeof(PrimArg) == 0x28, "Mini Pulse Cannon prim block");

	static void BonePoint(const int16_t *v, int32_t *out)
	{
		const uint8_t *bones = *(uint8_t *const *)*(uint8_t *const *)(Caster() + 0x64);
		const Mat4x3 *bm = (const Mat4x3 *)(bones + v[3] * 0x30 + 0x20);
		GteSetRotMatrixCtrl(bm);
		GteSetTransVectorCtrl(bm);
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		GteReadMAC123(out);
	}

	static void BeamVector(const MainNode *m, int32_t c, const int32_t P[3], int32_t v[3])
	{
		v[0] = (int32_t)m->pos[0] - P[0];
		v[2] = P[2] - (int32_t)m->pos[2];
		const int32_t dy = P[1] - (int32_t)*(const int16_t *)(m->ent + 0x24);
		if ((uint32_t)c < 0xE) v[1] = dy;
		else if ((uint32_t)c < 0x18) v[1] = mul32(dy, 0x18 - c) / 10;
		else v[1] = 0;
	}

	static int32_t BeamScale(int32_t c, int32_t len)
	{
		if ((uint32_t)c > 0x1A) return len >> 1;
		if ((uint32_t)c > 0xE) return len >> 2;
		return 0x1000;
	}

	static void ImpactFrame(const MainNode *m, const int32_t P[3], Mat4x3 *M)
	{
		int32_t v[3] = { P[0] - (int32_t)m->pos[0], 0, (int32_t)m->pos[2] - P[2] };
		BuildOrthonormalBasis(v, M);
		M->t[0] = m->pos[0];
		M->t[1] = m->pos[1];
		M->t[2] = m->pos[2];
	}

	static void SparkDraw(const SparkNode *s)
	{
		TransformCameraByShadowRotation(s->pos, (int16_t)s->shadow, -(s->shadow >> 2));
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = s->seq;
		*(int16_t *)(h + 4) = s->counter;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}
}
}

namespace ff8fx
{
namespace minipulse286
{
	static void __cdecl PrimCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PrimArg *arg = (const PrimArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = AllocHeader(0x6C);
		const int32_t object = (int16_t)r->flags_lo;
		uint8_t *model = l->data + *(const int32_t *)(l->data + object * 4 + 8);
		*(uint8_t **)h = model;
		const int16_t f0 = r->b0, f1 = r->b1;
		const int32_t nverts = *(const int32_t *)(model + 4);
		if (f0 == f1)
		{
			if (f0 == 0) *(uint8_t **)(h + 4) = model + 0xC;
			else *(uint8_t **)(h + 4) = model + (uint32_t)mul32(nverts, f0) * 8 + 0xC;
		}
		else
		{
			const int16_t t = r->b;
			if (t == 0)
			{
				if (f0 == 0) *(uint8_t **)(h + 4) = model + 0xC;
				else *(uint8_t **)(h + 4) = model + (uint32_t)mul32(nverts, f0) * 8 + 0xC;
			}
			else if (t == 0x1000)
			{
				if (f1 == 0) *(uint8_t **)(h + 4) = model + 0xC;
				else *(uint8_t **)(h + 4) = model + (uint32_t)mul32(nverts, f1) * 8 + 0xC;
			}
			else
			{
				BlendVertexFrames((uint32_t)model, f0, f1, t, arg->morph);
				*(uint32_t *)(h + 4) = arg->morph;
			}
		}
		Mat4x3 m = {};
		RotationFromAngles(r->rot, &m);
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		if (r->flags & 0x200)
		{
			m.t[0] = off[0];
			m.t[1] = off[1];
			m.t[2] = off[2];
		}
		else
		{
			GteSetRotMatrixCtrl(&arg->m);
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			if (!(r->flags & 0x8000)) MatrixMultiply(&arg->m, &m);
		}
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)arg->m.t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)arg->m.t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)arg->m.t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			const int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
			Scale3DMatrix(&m, v);
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)(h + 0x1C) = 0x2030;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) = 0x20F0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		if (arg->scroll != 0 && (int16_t)r->flags_lo <= 1)
		{
			*(uint16_t *)(h + 0x26) = 0;
			*(uint16_t *)(h + 0x24) = 0;
			*(uint16_t *)(h + 0x2A) = 0x100;
			*(uint16_t *)(h + 0x28) = 0x100;
			*(uint16_t *)(h + 0x30) = 0x100;
			*(uint16_t *)(h + 0x2C) = 0x100;
			if ((int16_t)r->flags_lo == 0)
			{
				*(uint16_t *)(h + 0x2E) = 0;
				*(uint16_t *)(h + 0x32) = 0x80;
			}
			else
			{
				*(uint16_t *)(h + 0x2E) = 0x80;
				*(uint16_t *)(h + 0x32) = 0x80;
			}
			*(int16_t *)(h + 0x22) = arg->scroll;
		}
		else *(int16_t *)(h + 0x22) = 0;
		*(int32_t *)(h + 0x18) = arg->depth;
		*(uint16_t *)(h + 0x20) = 0;
		PacketCursor() = RenderPrimSet(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x6C);
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *s = (SparkNode *)n;
		SparkDraw(s);
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
		s->counter++;
		return s->counter < s->life ? 0 : TASK_END;
	}

	static void SpawnSparks(int32_t c, const int32_t P[3])
	{
		int32_t count = 2;
		SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
		if (!s) return;
		int32_t base = shl32(c + 0x10, 8);
		for (;;)
		{
			s->seq = game_addr(SEQ_Spark);
			const int32_t angle = (CrtRand() & 0x3F) + base - 0x20;
			const int32_t radius = (CrtRand() & 0x3FF) + 0x200;
			s->pos[0] = (int16_t)(mul32(ComputeSin(angle), radius) >> 12);
			s->pos[1] = (int16_t)(mul32(ComputeCos(angle), radius) >> 12);
			s->pos[2] = (int16_t)((CrtRand() & 0xFF) - 0x80);
			GteLoadV0(s->pos);
			GteMVMVA_RotV0();
			GteStoreIR123(s->pos);
			const int16_t x = s->pos[0], y = s->pos[1], z = s->pos[2];
			s->life = 0xA;
			s->vel[0] = (int16_t)((int32_t)x / -10);
			s->vel[1] = (int16_t)((int32_t)y / -10);
			s->vel[2] = (int16_t)((int32_t)z / -10);
			s->pos[0] = (int16_t)(x + P[0]);
			s->pos[1] = (int16_t)(y + P[1]);
			s->pos[2] = (int16_t)(z + P[2]);
			s->counter = 0;
			s->shadow = 0x4B0;
			base -= 0x800;
			if (--count == 0) break;
			s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
			if (!s) break;
		}
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		const int32_t c = m->counter - 0x19;
		PrimArg arg;
		*(uint32_t *)&arg.depth = 0;
		arg.morph = 0;
		Mat4x3 M = {};
		int32_t P[3];
		if ((uint32_t)c < 0x2A)
		{
			BonePoint(MUZZLE(), P);
			M.t[0] = P[0];
			M.t[1] = P[1];
			M.t[2] = P[2];
			RotY(*(const int16_t *)(Caster() + 0xE), &M);
			if ((uint32_t)c < 8)
			{
				GteSetRotMatrix(&M);
				SpawnSparks(c, P);
			}
			ComposeAffineTransform(&Camera(), &M, &arg.m);
			arg.morph = TexBase() + MORPH;
			arg.depth = -0x20;
			arg.scroll = 0;
			prim::play((prim::Layout *)m->muzzle, PrimCallback, (int)&arg, 0);
			int32_t v[3];
			BeamVector(m, c, P, v);
			const int32_t len = BuildOrthonormalBasis(v, &M);
			const int32_t sv[3] = { 0x1000, 0x1000, BeamScale(c, len) };
			Scale3DMatrix(&M, sv);
			ComposeAffineTransform(&Camera(), &M, &arg.m);
			arg.scroll = (int16_t)((c & 3) << 5);
			prim::play((prim::Layout *)m->beam, PrimCallback, (int)&arg, 0);
		}
		if ((uint32_t)(m->counter - 0x31) < 0x14)
		{
			BonePoint(MUZZLE(), P);
			ImpactFrame(m, P, &M);
			ComposeAffineTransform(&Camera(), &M, &arg.m);
			prim::play((prim::Layout *)m->impact, PrimCallback, (int)&arg, 0);
			M.t[1] = *(const int16_t *)(m->ent + 0x24);
			ComposeAffineTransform(&Camera(), &M, &arg.m);
			arg.morph = TexBase() + MORPH;
			arg.depth = -0x80;
			arg.scroll = 0;
			prim::play((prim::Layout *)m->ground, PrimCallback, (int)&arg, 0);
		}
		if (m->counter == 0x18) BdPlaySE(SOUND_Shot(), 0, 0x80);
		if (m->counter == 0x35) ApplyActionResultToTarget(Ctx()->actions[0].targets);
		m->counter++;
		return m->counter < 0x45 ? 0 : TASK_END;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *mn = (MasterNode *)n;
		if (mn->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			mn->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			mn->arena = 1;
		}
		if (mn->counter == 1 && !mn->started)
		{
			mn->started = 1;
			InitTaskQueuePool(QMain(), (void *)(TexBase() + POOL_Main), 0x6D4, 1);
			InitTaskQueuePool(QSparks(), (void *)TexBase(), 0x24, 0x20);
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&m->counter, 0, 0x1B2);
			m->ent = Entity(Ctx()->actions[0].targets[0]);
			DecodeModelPrimLayout(game_addr(MODEL_Muzzle), m->muzzle, 0x22C);
			DecodeModelPrimLayout(game_addr(MODEL_Beam), m->beam, 0x158);
			DecodeModelPrimLayout(game_addr(MODEL_Impact), m->impact, 0x2A4);
			DecodeModelPrimLayout(game_addr(MODEL_Ground), m->ground, 0x90);
			GetDefaultEffectPosition(m->ent, m->pos);
		}
		int a0, a1;
		if (mn->started)
		{
			uint8_t *caster = Caster();
			ComputeBonesWorldMatrices(caster + 0x60, caster + 0x40);
			a0 = ExecuteTaskQueue(QMain());
			if (a0 == 0) *(uint16_t *)Caster() &= 0xFFF7;
			BuildBoneMatricesFromPose(Caster() + 0x60);
			a1 = ExecuteTaskQueue(QSparks());
		}
		else a0 = a1 = (int)n;
		if (mn->started && !a0 && !a1) return TASK_END;
		mn->counter++;
		return 0;
	}
}

	void register_mag286_mini_pulse_cannon()
	{
		register_port(minipulse286::ORIG_MasterTask, (void *)minipulse286::MasterTask, "286 MasterTask", 286);
		register_port(minipulse286::ORIG_MainTask, (void *)minipulse286::MainTask, "286 MainTask", 286);
		register_port(minipulse286::ORIG_SparkTask, (void *)minipulse286::SparkTask, "286 SparkTask", 286);
	}
}

