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

#include "mag_common.h"

namespace ff8fx
{
namespace micromissiles299
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_299_MICRO_MISSILES_QUISTIS_Context)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_299_MICRO_MISSILES_QUISTIS_Caster)); }
	inline TaskQueue *QSparks() { return (TaskQueue *)game_addr(game::MAG_299_MICRO_MISSILES_QUISTIS_SparkQueue); }
	inline TaskQueue *QHits() { return (TaskQueue *)game_addr(game::MAG_299_MICRO_MISSILES_QUISTIS_HitQueue); }
	inline TaskQueue *QMissiles() { return (TaskQueue *)game_addr(game::MAG_299_MICRO_MISSILES_QUISTIS_MissileQueue); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_299_MICRO_MISSILES_QUISTIS_PacketCursor)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_299_MICRO_MISSILES_QUISTIS_MagicBuffer)); }
	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	static constexpr game::Id ORIG_MasterTask = game::MAG_299_MICRO_MISSILES_QUISTIS_MasterTask;
	static constexpr game::Id ORIG_MissileTask = game::MAG_299_MICRO_MISSILES_QUISTIS_MissileTask;
	static constexpr game::Id ORIG_HitTask = game::MAG_299_MICRO_MISSILES_QUISTIS_HitTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_299_MICRO_MISSILES_QUISTIS_SparkTask;
	static constexpr game::Id ORIG_PrimCallback = game::MAG_299_MICRO_MISSILES_QUISTIS_PrimCallback;
	static const uint32_t POOL_Hits = 0xD80, POOL_Missiles = 0x1518;
	static const uint32_t BIND_Block = 0x4560;
	static const uint32_t MORPH = 0x4580;
	static const uint32_t ARENA_A = 0x4780, ARENA_B = 0x14780;
	static constexpr game::Id COM_Anim = game::MAG_299_MICRO_MISSILES_QUISTIS_CasterAnimSet;
	static constexpr game::Id CAMERA_Anim = game::MAG_299_MICRO_MISSILES_QUISTIS_CameraAnim;
	static constexpr game::Id MODEL_Missile = game::MAG_299_MICRO_MISSILES_QUISTIS_MissileLayout;
	static constexpr game::Id MODEL_Hit = game::MAG_299_MICRO_MISSILES_QUISTIS_HitLayout;
	static constexpr game::Id MODEL_Miss = game::MAG_299_MICRO_MISSILES_QUISTIS_MissLayout;
	static constexpr game::Id SEQ_Trail = game::MAG_299_MICRO_MISSILES_QUISTIS_TrailSequence;
	static constexpr game::Id SEQ_Flash = game::MAG_299_MICRO_MISSILES_QUISTIS_FlashSequence;
	static constexpr game::Id SEQ_Spark = game::MAG_299_MICRO_MISSILES_QUISTIS_SparkSequence;
	static const void *SOUND_Launch() { return (const void *)game_addr(game::MAG_299_MICRO_MISSILES_QUISTIS_Sound); }

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline int32_t BuildOrthonormalBasis(int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline void RotZ(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_065_RotationZMatrix))(angle, out); }
	inline void Matrix3x3MultiplyPSX(const Mat4x3 *a, const Mat4x3 *b, Mat4x3 *out) { fn<void (__cdecl *)(const Mat4x3 *, const Mat4x3 *, Mat4x3 *)>(game_addr(game::Matrix3x3_MultiplyPSX))(a, b, out); }
	inline void Apply3DTransform(const Mat4x3 *m, const int16_t *v, int32_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int32_t *)>(game_addr(game::apply3DTransform))(m, v, out); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline void BindEntityModelFromComData(uint8_t *entity, uint32_t out, uint32_t data) { fn<void (__cdecl *)(uint8_t *, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline void QueueChainTransformation(uint8_t *entity, int32_t id) { fn<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t done;
		uint32_t arena;
	};
	struct MissileNode
	{
		TaskNode hdr;
		int16_t rot[9];
		int16_t pad1E;
		int16_t dist;
		int16_t type;
		int16_t start[3];
		int16_t t;
		int16_t cur[3];
		int16_t step;
		uint8_t layout[0x7D8];
	};
	struct HitNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		Mat4x3 m;
		uint8_t layout[0x114];
	};
	struct SparkNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t counter;
		int16_t vel[3];
		int16_t pad1A;
		int16_t drag[3];
		int16_t pad22;
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(MissileNode) == 0x80C && sizeof(HitNode) == 0x144 && sizeof(SparkNode) == 0x24, "Micro Missiles nodes");
	static_assert(sizeof(PrimArg) == 0x28, "Micro Missiles prim block");

	inline Mat4x3 *Frame(MissileNode *m) { return (Mat4x3 *)m->rot; }
	inline const Mat4x3 *Frame(const MissileNode *m) { return (const Mat4x3 *)m->rot; }

	static void ArcOffset(int32_t a, int32_t r, int16_t *out)
	{
		out[0] = (int16_t)(mul32(ComputeSin(a), r) >> 12);
		out[2] = (int16_t)((int32_t)(0u - (uint32_t)mul32(ComputeCos(a), r)) >> 12);
	}

	static int RandomBonePoint(uint8_t *ent, int32_t idx, int16_t *out)
	{
		int32_t *blk = (int32_t *)FieldAlloc(0x14);
		const uint8_t *skel = **(uint8_t *const *const *)(ent + 0x64);
		if (idx >= 0xF0) idx = skel[idx - 0xEC];
		const uint8_t *bone = skel + 0x10 + idx * 0x30;
		if (*(const int16_t *)bone < 0)
		{
			FieldFree(0x14);
			return 1;
		}
		TransformVectorBy3x3Matrix((const Mat4x3 *)(ent + 0x40), (const int32_t *)(bone + 0x24), blk);
		out[0] = (int16_t)(*(const int16_t *)(ent + 0x54) + (int16_t)blk[0]);
		out[1] = (int16_t)(*(const int16_t *)(ent + 0x58) + (int16_t)blk[1]);
		out[2] = (int16_t)(*(const int16_t *)(ent + 0x5C) + (int16_t)blk[2]);
		FieldFree(0x14);
		return 0;
	}

	static void Direction(int32_t a, int32_t b, int16_t *out)
	{
		const int32_t c = ComputeCos(a);
		out[1] = (int16_t)(0 - ComputeSin(a));
		out[0] = (int16_t)(mul32(ComputeSin(b), c) >> 12);
		out[2] = (int16_t)((int32_t)(0u - (uint32_t)mul32(ComputeCos(b), c)) >> 12);
	}

	static void PathOffset(const MissileNode *m, int32_t s, int16_t v[4])
	{
		const int32_t d = mul32(m->dist, s) >> 12;
		const int32_t k = 0x1000 - ComputeCos(s >> 2);
		const int32_t a = (int32_t)(0u - (uint32_t)mul32(ComputeSin(s >> 1), d)) >> 12;
		v[1] = (int16_t)(mul32(k, a) >> 12);
		v[2] = (int16_t)(mul32(d, s) >> 12);
		v[0] = 0;
		v[3] = 0;
	}

	static void TrailPoint(const MissileNode *m, int32_t s, int32_t p[3])
	{
		int16_t v[4];
		PathOffset(m, s, v);
		GteSetRotMatrixCtrl(Frame(m));
		GteLoadV0(v);
		GteMVMVA_RotV0();
		GteReadMAC123(p);
		p[1] = (int32_t)((uint32_t)p[1] + (uint32_t)(int32_t)m->start[1]);
		p[0] = (int32_t)((uint32_t)p[0] + (uint32_t)(int32_t)m->start[0]);
		p[2] = (int32_t)((uint32_t)p[2] + (uint32_t)(int32_t)m->start[2]);
	}

	static void TrailDraw(uint8_t *h, const int32_t p[3], int16_t frame)
	{
		Mat4x3 view = {};
		view.m[0][0] = 0x1000;
		view.m[1][1] = 0x1000;
		view.m[2][2] = 0x1000;
		TransformVectorBy3x3Matrix(&Camera(), p, view.t);
		view.t[0] = (int32_t)((uint32_t)view.t[0] + (uint32_t)Camera().t[0]);
		view.t[1] = (int32_t)((uint32_t)view.t[1] + (uint32_t)Camera().t[1]);
		view.t[2] = (int32_t)((uint32_t)view.t[2] + (uint32_t)Camera().t[2]);
		GteSetRotMatrixCtrl(&view);
		GteSetTransVectorCtrl(&view);
		*(uint32_t *)h = game_addr(SEQ_Trail);
		*(int16_t *)(h + 4) = frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
	}

	static void HeadPoint(const MissileNode *m, int32_t t, int32_t p[3])
	{
		int16_t v[4];
		PathOffset(m, t, v);
		int32_t o[3];
		Apply3DTransform(Frame(m), v, o);
		p[0] = (int32_t)((uint32_t)o[0] + (uint32_t)(int32_t)m->start[0]);
		p[1] = (int32_t)((uint32_t)o[1] + (uint32_t)(int32_t)m->start[1]);
		p[2] = (int32_t)((uint32_t)o[2] + (uint32_t)(int32_t)m->start[2]);
	}

	static void HeadStep(const int16_t cur[3], const int32_t p[3], int32_t v[3])
	{
		v[0] = (int32_t)((uint32_t)p[0] - (uint32_t)(int32_t)cur[0]);
		v[2] = (int32_t)((uint32_t)(int32_t)cur[2] - (uint32_t)p[2]);
		v[1] = v[2] > 0 ? (int32_t)((uint32_t)p[1] - (uint32_t)(int32_t)cur[1]) : (int32_t)((uint32_t)(int32_t)cur[1] - (uint32_t)p[1]);
	}

	static void HeadArg(const int32_t p[3], int32_t v[3], PrimArg *arg)
	{
		BuildOrthonormalBasis(v, &arg->m);
		arg->m.t[0] = p[0];
		arg->m.t[1] = p[1];
		arg->m.t[2] = p[2];
		ComposeAffineTransform(&Camera(), &arg->m, &arg->m);
		arg->depth = 0;
		arg->morph = TexBase() + MORPH;
	}

	static void HitArg(const HitNode *h, PrimArg *arg)
	{
		ComposeAffineTransform(&Camera(), &h->m, &arg->m);
		arg->depth = -0x80;
		arg->morph = TexBase() + MORPH;
	}

	static void FlashDraw(const PrimArg *arg, int16_t frame)
	{
		Mat4x3 m = arg->m;
		m.m[0][0] = 0x1800; m.m[0][1] = 0; m.m[0][2] = 0;
		m.m[1][0] = 0; m.m[1][1] = 0x1800; m.m[1][2] = 0;
		m.m[2][0] = 0; m.m[2][1] = 0; m.m[2][2] = 0x1800;
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Flash);
		*(int16_t *)(h + 4) = frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void SparkDraw(const SparkNode *s)
	{
		TransformCameraByShadowRotation(s->pos, 0x400, -0x100);
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Spark);
		*(int16_t *)(h + 4) = s->counter;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}
}
}

namespace ff8fx
{
namespace micromissiles299
{
	static void __cdecl PrimCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PrimArg *arg = (const PrimArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = AllocHeader(0x58);
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
		if (r->flags & 0x2000) *(uint32_t *)(h + 0x1C) = 0x203C;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) |= 0xC0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		*(int32_t *)(h + 0x18) = arg->depth;
		PacketCursor() = RenderPrim2(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *s = (SparkNode *)n;
		SparkDraw(s);
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
		const int16_t vx = (int16_t)(s->drag[0] + s->vel[0]);
		const int16_t vy = (int16_t)(s->drag[1] + s->vel[1]);
		const int16_t vz = (int16_t)(s->drag[2] + s->vel[2]);
		s->counter++;
		s->vel[0] = vx;
		s->vel[1] = vy;
		s->vel[2] = vz;
		return s->counter < 0xC ? 0 : TASK_END;
	}

	static uint32_t __cdecl HitTask(TaskNode *n)
	{
		HitNode *h = (HitNode *)n;
		PrimArg arg;
		HitArg(h, &arg);
		const int left = prim::play((prim::Layout *)h->layout, PrimCallback, (int)&arg, 0);
		if (h->counter < 0xE)
		{
			FlashDraw(&arg, h->counter);
			h->counter++;
			return 0;
		}
		if (left)
		{
			h->counter++;
			return 0;
		}
		return TASK_END;
	}

	static uint32_t __cdecl MissileTask(TaskNode *n)
	{
		MissileNode *m = (MissileNode *)n;
		uint8_t *h = AllocHeader(0xB4);
		int32_t s = m->t - 0x990;
		for (int16_t frame = 0xB; ; frame--)
		{
			if (s >= 0 && s < 0x1000)
			{
				int32_t p[3];
				TrailPoint(m, s, p);
				TrailDraw(h, p, frame);
			}
			s += 0xCC;
			if (s >= 0x1000 || frame == 0) break;
		}
		FieldFree(0xB4);
		int32_t v[3] = {};
		const int32_t t = m->t;
		if (t <= 0x1000)
		{
			int32_t p[3];
			HeadPoint(m, t, p);
			HeadStep(m->cur, p, v);
			m->cur[0] = (int16_t)p[0];
			m->cur[2] = (int16_t)p[2];
			m->cur[1] = (int16_t)p[1];
			PrimArg arg;
			HeadArg(p, v, &arg);
			prim::play((prim::Layout *)m->layout, PrimCallback, (int)&arg, 0);
		}
		m->t = (int16_t)(m->t + m->step);
		if (m->t >= 0x1000 && m->type != -1)
		{
			HitNode *hit = (HitNode *)AddTaskToQueue(QHits(), game_addr(ORIG_HitTask));
			if (hit)
			{
				hit->counter = 0;
				v[1] = 0;
				BuildOrthonormalBasis(v, &hit->m);
				hit->m.t[0] = m->cur[0];
				hit->m.t[1] = m->cur[1];
				hit->m.t[2] = m->cur[2];
				if (m->type <= 1)
				{
					DecodeModelPrimLayout(game_addr(MODEL_Hit), hit->layout, 0xA0);
					if (m->type == 0) QueueChainTransformation(Entity(Ctx()->actions[0].targets[0]), 5);
					else ApplyActionResultToTarget(Ctx()->actions[0].targets);
				}
				else DecodeModelPrimLayout(game_addr(MODEL_Miss), hit->layout, 0x114);
			}
			for (int k = 0; k < 0x10; k++)
			{
				SparkNode *sp = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
				if (!sp) continue;
				ff8fx::copy<(4)>(sp->pos, m->cur);
				ff8fx::copy<(4)>(&sp->pos[2], &m->cur[2]);
				sp->counter = 0;
				const int32_t b = CrtRand();
				const int32_t a = CrtRand();
				Direction(a, b, sp->vel);
				GteSetIR0((CrtRand() & 0x3F) + 0x80);
				GteLoadIR123(sp->vel);
				GteGPF();
				GteStoreIR123(sp->vel);
				sp->drag[0] = (int16_t)((int32_t)(0 - (int32_t)sp->vel[0]) >> 4);
				sp->drag[1] = (int16_t)((int32_t)(0 - (int32_t)sp->vel[1]) >> 4);
				sp->drag[2] = (int16_t)((int32_t)(0 - (int32_t)sp->vel[2]) >> 4);
			}
			m->type = -1;
		}
		return m->t < 0x1990 ? 0 : TASK_END;
	}

	static void SpawnMissile(int32_t k)
	{
		uint8_t *target = Entity(Ctx()->actions[0].targets[0]);
		uint8_t *caster = Caster();
		int16_t pos[4];
		GetDefaultEffectPosition(caster, pos);
		int16_t off[4];
		ArcOffset(-(int32_t)*(const int16_t *)(caster + 0xE), 0x12C, off);
		pos[0] = (int16_t)(pos[0] + off[0]);
		pos[2] = (int16_t)(pos[2] + off[2]);
		pos[1] = (int16_t)(*(const int16_t *)(caster + 0x24) - 0x26C);
		MissileNode *m = (MissileNode *)AddTaskToQueue(QMissiles(), game_addr(ORIG_MissileTask));
		ff8fx::copy<(4)>(m->cur, pos);
		ff8fx::copy<(4)>(&m->cur[2], &pos[2]);
		ff8fx::copy<(4)>(m->start, pos);
		ff8fx::copy<(4)>(&m->start[2], &pos[2]);
		m->step = 0xCC;
		m->t = 0xCC;
		int16_t aim[4];
		while (RandomBonePoint(target, mul32(CrtRand(), ***(const uint8_t *const *const *)(target + 0x64)) >> 15, aim)) {}
		if (k % 3 == 1)
		{
			aim[0] = (int16_t)(aim[0] + (CrtRand() & 0x7FF) - 0x400);
			aim[2] = (int16_t)(aim[2] + (CrtRand() & 0x7FF) - 0x400);
			aim[1] = *(const int16_t *)(target + 0x24);
			m->type = 2;
		}
		else if (k == 5) m->type = 1;
		else m->type = 0;
		int32_t v[3] = { (int32_t)pos[0] - aim[0], (int32_t)aim[1] - pos[1], (int32_t)aim[2] - pos[2] };
		Mat4x3 basis;
		m->dist = (int16_t)BuildOrthonormalBasis(v, &basis);
		DecodeModelPrimLayout(game_addr(MODEL_Missile), m->layout, 0x7D8);
		RotZ(shl32(k, 8) - 0x280, Frame(m));
		Matrix3x3MultiplyPSX(&basis, Frame(m), Frame(m));
		m->pad1E = (int16_t)((uint32_t)m >> 16);
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
		if (mn->counter == 1)
		{
			InitTaskQueuePool(QMissiles(), (void *)(TexBase() + POOL_Missiles), 0x80C, 6);
			InitTaskQueuePool(QHits(), (void *)(TexBase() + POOL_Hits), 0x144, 6);
			InitTaskQueuePool(QSparks(), (void *)TexBase(), 0x24, 0x60);
			BindEntityModelFromComData(Caster(), TexBase() + BIND_Block, game_addr(COM_Anim));
			QueueChainTransformation(Caster(), 2);
		}
		const int32_t c = mn->counter - 0xA;
		if ((uint32_t)c < 0x18 && !(c & 3)) SpawnMissile(c >> 2);
		int a0, a1, a2;
		if (mn->counter >= 0xA)
		{
			a0 = ExecuteTaskQueue(QMissiles());
			if (a0 == 0 && mn->done == 0) mn->done = 1;
			a1 = ExecuteTaskQueue(QHits());
			a2 = ExecuteTaskQueue(QSparks());
			if (mn->counter == 0xA) BdPlaySE(SOUND_Launch(), 0, 0x80);
		}
		else a0 = a1 = a2 = (int)n;
		if (mn->done && !a0 && !a1 && !a2) return TASK_END;
		mn->counter++;
		return 0;
	}
}

	void register_mag299_micro_missiles_quistis()
	{
		register_port(micromissiles299::ORIG_MasterTask, (void *)micromissiles299::MasterTask, "299 MasterTask", 299);
		register_port(micromissiles299::ORIG_MissileTask, (void *)micromissiles299::MissileTask, "299 MissileTask", 299);
		register_port(micromissiles299::ORIG_HitTask, (void *)micromissiles299::HitTask, "299 HitTask", 299);
		register_port(micromissiles299::ORIG_SparkTask, (void *)micromissiles299::SparkTask, "299 SparkTask", 299);
	}
}

