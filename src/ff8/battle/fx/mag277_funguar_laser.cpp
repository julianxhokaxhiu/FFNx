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

// Effect 277: Funguar's laser

#include "mag_common.h"

namespace ff8fx
{
namespace laser277
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g277_Caster)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g277_TexBase)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g277_Ctx)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g277_PacketCursor)); }
	inline TaskQueue *QHits() { return (TaskQueue *)game_addr(game::g277_QHits); }
	inline TaskQueue *QSparks() { return (TaskQueue *)game_addr(game::g277_QSparks); }
	inline TaskQueue *QGlows() { return (TaskQueue *)game_addr(game::g277_QGlows); }
	inline TaskQueue *QBeam() { return (TaskQueue *)game_addr(game::g277_QBeam); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_277_FunguarLaser_MasterTask;
	static constexpr game::Id ORIG_BeamTask = game::MAG_277_FunguarLaser_BeamTask;
	static constexpr game::Id ORIG_HitTask = game::MAG_277_FunguarLaser_HitTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_277_FunguarLaser_SparkTask;
	static constexpr game::Id ORIG_GlowTask = game::MAG_277_FunguarLaser_GlowTask;
	static const uint32_t MORPH = 0xF24;
	static const uint32_t ARENA_A = 0x10AC;
	static const uint32_t ARENA_B = 0x110AC;
	static const uint32_t POOL_Beam = 0x8C0;
	static constexpr game::Id MODEL_Muzzle = game::unk_10CE318;
	static constexpr game::Id MODEL_Ray = game::unk_10D0ED4;
	static constexpr game::Id MODEL_Impact = game::unk_10CD0AC;
	static constexpr game::Id MODEL_Hit = game::unk_10D2134;
	static constexpr game::Id PRIM_Beam = game::unk_10CB9E4;
	static constexpr game::Id SEQ_Spark = game::unk_10CB838;
	static constexpr game::Id SEQ_Glow = game::unk_10CB628;
	static const void *SOUND_Beam() { return (const void *)game_addr(game::unk_10D2E9C); }
	static const int16_t P_OBJECT = 0, P_VERTEX = 0x7B;

	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MulMatrix3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteLoadV0u(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline int32_t BuildOrthonormalBasis(const int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t index;
		uint8_t started;
		uint32_t arena;
	};
	struct BeamNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t index;
		uint8_t *ent;
		int16_t spawn[4];
		uint8_t muzzle[0x3B0];
		uint8_t ray[0xE8];
		uint8_t impact[0x1B0];
	};
	struct HitNode
	{
		TaskNode hdr;
		Mat4x3 frame;
		uint8_t layout[0x7C];
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
	struct GlowNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t counter;
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(BeamNode) == 0x664 && sizeof(HitNode) == 0xA8, "Funguar laser nodes");
	static_assert(sizeof(SparkNode) == 0x24 && sizeof(GlowNode) == 0x14, "Funguar laser nodes");
	static_assert(sizeof(PrimArg) == 0x28, "Funguar laser prim block");

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
		if (r->flags & 0x40000) RotationFromAnglesB(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
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
			if (r->flags & 0x100)
			{
				int16_t s[9] = { r->scale[0], 0, 0, 0, r->scale[1], 0, 0, 0, r->scale[2] };
				MulMatrix3(&m, s);
			}
			else
			{
				const int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
				Scale3DMatrix(&m, v);
			}
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
		*(int32_t *)(h + 0x18) = arg->depth;
		PacketCursor() = RenderPrim2(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void SeekVertex(uint8_t *blk, uint8_t *anim)
	{
		uint8_t *model = *(uint8_t **)(anim + 4);
		uint8_t *bones = *(uint8_t **)model + 0x10;
		uint8_t *table = *(uint8_t **)(model + 4);
		uint8_t *p = table + *(const int32_t *)(table + *(int16_t *)(blk + 0xC) * 4 + 4);
		int32_t groups = *(const int16_t *)p;
		p += 2;
		*(uint8_t **)(blk + 4) = p;
		while (groups-- != 0)
		{
			*(uint8_t **)(blk + 8) = bones + *(const int16_t *)p * 0x30 + 0x10;
			p += 2;
			const int16_t count = *(const int16_t *)p;
			p += 2;
			*(uint8_t **)(blk + 4) = p;
			const int16_t idx = *(int16_t *)(blk + 0xE);
			if (idx < count)
			{
				*(uint8_t **)(blk + 4) = p + idx * 6;
				return;
			}
			*(int16_t *)(blk + 0xE) = (int16_t)(idx - count);
			p += count * 6;
			*(uint8_t **)(blk + 4) = p;
		}
	}

	static void VertexPoint(uint8_t *ent, int16_t object, int16_t vertex, int16_t *out)
	{
		uint8_t *blk = (uint8_t *)FieldAlloc(0x10);
		*(int16_t *)(blk + 0xC) = object;
		*(int16_t *)(blk + 0xE) = vertex;
		uint8_t *anim = ent + 0x60;
		ComputeBonesWorldMatrices(anim, ent + 0x40);
		SeekVertex(blk, anim);
		GteSetRotMatrix(*(const Mat4x3 **)(blk + 8));
		GteSetTransVector(*(const Mat4x3 **)(blk + 8));
		GteLoadV0u(*(const void **)(blk + 4));
		GteMVMVA_RotV0Tr();
		GteStoreIR123(out);
		BuildBoneMatricesFromPose(anim);
		FieldFree(0x10);
	}

	static void PointFrame(const Mat4x3 *M, const int16_t *P, int16_t *v, PrimArg *arg)
	{
		GteSetRotMatrixCtrl(M);
		GteLoadV0(v);
		GteMVMVA_RotV0();
		GteStoreIR123(v);
		v[0] = (int16_t)(v[0] + P[0]);
		v[1] = (int16_t)(v[1] + P[1]);
		v[2] = (int16_t)(v[2] + P[2]);
		GteSetRotMatrixCtrl(&Camera());
		GteLoadV0(v);
		GteMVMVA_RotV0();
		int32_t mac[3];
		GteReadMAC123(mac);
		ff8fx::copy<(sizeof(arg->m.m))>(&arg->m.m, &Camera().m);
		arg->m.t[0] = (int32_t)((uint32_t)mac[0] + (uint32_t)Camera().t[0]);
		arg->m.t[1] = (int32_t)((uint32_t)mac[1] + (uint32_t)Camera().t[1]);
		arg->m.t[2] = (int32_t)((uint32_t)mac[2] + (uint32_t)Camera().t[2]);
		arg->depth = 0;
		arg->morph = TexBase() + MORPH;
	}

	struct BeamFrame
	{
		int16_t P[4];
		int16_t T[4];
		int32_t v[3];
		Mat4x3 M;
		Mat4x3 R;
	};
	static void BuildFrame(const BeamNode *b, BeamFrame *f)
	{
		VertexPoint(Caster(), P_OBJECT, P_VERTEX, f->P);
		GetDefaultEffectPosition(b->ent, f->T);
		f->v[0] = (int32_t)f->T[0] - f->P[0];
		f->v[1] = 0;
		f->v[2] = (int32_t)f->P[2] - f->T[2];
		BuildOrthonormalBasis(f->v, &f->M);
		for (int k = 0; k < 3; k++)
		{
			f->R.m[0][k] = (int16_t)(0 - f->M.m[0][k]);
			f->R.m[1][k] = f->M.m[1][k];
			f->R.m[2][k] = (int16_t)(0 - f->M.m[2][k]);
		}
	}

	struct Ease
	{
		int16_t lift;
		int32_t width;
		int32_t fade;
	};
	static Ease BeamEase(int32_t counter, int16_t size)
	{
		Ease r;
		const uint32_t e = (uint32_t)(counter - 15);
		if (e < 0x14)
		{
			r.width = 0x1000 - ComputeCos((int32_t)((e << 10) / 20u));
			r.fade = 0;
			r.lift = (int16_t)((int32_t)size * 2 - (mul32(size, r.width) >> 10));
		}
		else if (e < 0x28)
		{
			r.fade = (int32_t)((e - 0x14) << 9);
			r.lift = (int16_t)((int32_t)size * 2 - (shl32(mul32((int32_t)e + 0x2C, size), 2) >> 6));
			r.width = 0x380 - (int32_t)(e << 5);
		}
		else
		{
			r.fade = 0;
			r.width = 0;
			r.lift = (int16_t)(size * 2);
		}
		return r;
	}

	static void FloorHit(const int16_t *P, int16_t *T)
	{
		const int32_t dy = (int32_t)T[1] - P[1];
		const int32_t ax = mul32((int32_t)P[0] - T[0], P[1]);
		const int32_t az = mul32((int32_t)P[2] - T[2], P[1]);
		T[0] = (int16_t)(ax / dy + P[0]);
		T[2] = (int16_t)(az / dy + P[2]);
		T[1] = 0;
	}

	struct BeamDraw
	{
		Mat4x3 M2;
		int32_t len;
		int kind;
	};
	static void BeamShape(const BeamNode *b, int32_t counter, const int16_t *P, const int16_t *T, BeamDraw *d)
	{
		const int32_t u[3] = { (int32_t)T[0] - P[0], (int32_t)P[1] - T[1], (int32_t)P[2] - T[2] };
		int32_t len = BuildOrthonormalBasis(u, &d->M2);
		d->kind = 0;
		const int16_t top = *(const int16_t *)(b->ent + 0x92);
		if (T[1] < top) len <<= 1;
		else if (T[1] >= 0) d->kind = 1;
		else if ((int32_t)T[1] >= (int32_t)top - ((int32_t)top >> 3) && !(counter & 1)) d->kind = 2;
		d->len = len;
	}
	static int16_t BeamWidth(int32_t angle) { return (int16_t)(ComputeSin(angle >> 1) >> 3); }
	static void BeamRender(Mat4x3 *M2, int32_t len, int16_t w, int32_t fade)
	{
		Mat4x3 s;
		Memset32(&s, 0, 8);
		s.m[0][0] = w;
		s.m[1][1] = w;
		s.m[2][2] = (int16_t)len;
		MulMatrix3(M2, &s);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(PRIM_Beam);
		if (fade == 0) *(uint32_t *)(h + 0x1C) = 0x30;
		else
		{
			h[0xA] = 0;
			h[9] = 0;
			h[8] = 0;
			*(int32_t *)(h + 0xC) = fade;
			*(uint32_t *)(h + 0x1C) = 0xF0;
		}
		ComposeAffineTransform(&Camera(), M2, M2);
		GteSetRotMatrix(M2);
		GteSetTransVector(M2);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void ImpactFrame(const BeamNode *b, Mat4x3 *R)
	{
		R->t[0] = b->spawn[0];
		R->t[1] = *(const int16_t *)(b->ent + 0x92) / 2;
		R->t[2] = b->spawn[2];
	}

	static void SpriteDraw(const int16_t *pos, int16_t frame, uint32_t seq, int32_t a, int32_t b)
	{
		uint8_t *h = AllocHeader(0xB4);
		TransformCameraByShadowRotation(pos, a, b);
		*(uint32_t *)h = seq;
		*(int16_t *)(h + 4) = frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}
}
}

namespace ff8fx
{
namespace laser277
{
	static uint32_t __cdecl GlowTask(TaskNode *n)
	{
		GlowNode *g = (GlowNode *)n;
		SpriteDraw(g->pos, g->counter, game_addr(SEQ_Glow), 0xAF0, -0x400);
		g->counter++;
		return g->counter < 0x10 ? 0 : TASK_END;
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *s = (SparkNode *)n;
		SpriteDraw(s->pos, s->counter, game_addr(SEQ_Spark), 0xC00, -0x300);
		s->counter++;
		if (s->counter >= 0x10) return TASK_END;
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
		if (s->counter < 0xC)
		{
			s->vel[0] = (int16_t)(s->drag[0] + s->vel[0]);
			s->vel[1] = (int16_t)(s->drag[1] + s->vel[1]);
			s->vel[2] = (int16_t)(s->drag[2] + s->vel[2]);
		}
		return 0;
	}

	static uint32_t __cdecl HitTask(TaskNode *n)
	{
		HitNode *h = (HitNode *)n;
		PrimArg arg;
		ComposeAffineTransform(&Camera(), &h->frame, &arg.m);
		arg.depth = 0;
		arg.morph = TexBase() + MORPH;
		return prim::play((prim::Layout *)h->layout, PrimCallback, (int)&arg, 0) ? 0 : TASK_END;
	}

	static uint32_t __cdecl BeamTask(TaskNode *n)
	{
		BeamNode *b = (BeamNode *)n;
		BeamFrame f;
		BuildFrame(b, &f);
		if (b->counter == 0) BdPlaySE(SOUND_Beam(), 0, 0x80);
		const int32_t c = b->counter;
		PrimArg arg;
		if ((uint32_t)(c - 5) < 0x28)
		{
			Mat4x3 M;
			BuildOrthonormalBasis(f.v, &M);
			int16_t v[4] = {};
			PointFrame(&M, f.P, v, &arg);
			prim::play((prim::Layout *)b->muzzle, PrimCallback, (int)&arg, 0);
			const Ease ez = BeamEase(c, *(const int16_t *)(b->ent + 0x26));
			int16_t T[4] = { f.T[0], (int16_t)(f.T[1] + ez.lift), f.T[2], f.T[3] };
			BeamDraw d;
			{
				const int32_t u[3] = { (int32_t)T[0] - f.P[0], (int32_t)f.P[1] - T[1], (int32_t)f.P[2] - T[2] };
				BuildOrthonormalBasis(u, &d.M2);
			}
			d.M2.t[0] = v[0];
			d.M2.t[1] = v[1];
			d.M2.t[2] = v[2];
			PrimArg ray;
			ComposeAffineTransform(&Camera(), &d.M2, &ray.m);
			ray.depth = 0;
			ray.morph = TexBase() + MORPH;
			prim::play((prim::Layout *)b->ray, PrimCallback, (int)&ray, 0);
			if ((uint32_t)(c - 15) < 0x28)
			{
				if (T[1] >= 0) FloorHit(f.P, T);
				BeamShape(b, c, f.P, T, &d);
				if (d.kind == 2)
				{
					GlowNode *g = (GlowNode *)AddTaskToQueue(QGlows(), game_addr(ORIG_GlowTask));
					if (g)
					{
						g->pos[0] = T[0];
						g->pos[1] = T[1];
						g->pos[2] = T[2];
						g->counter = 0;
					}
				}
				else if (d.kind == 1)
				{
					HitNode *h = (HitNode *)AddTaskToQueue(QHits(), game_addr(ORIG_HitTask));
					if (h)
					{
						ff8fx::copy<(sizeof(h->frame.m))>(&h->frame.m, &f.R.m);
						h->frame.t[0] = T[0];
						h->frame.t[1] = T[1];
						h->frame.t[2] = T[2];
						DecodeModelPrimLayout(game_addr(MODEL_Hit), h->layout, 0x7C);
					}
				}
				BeamRender(&d.M2, d.len, BeamWidth(ez.width), ez.fade);
			}
		}
		if ((uint32_t)(c - 0x21) < 0x18)
		{
			ImpactFrame(b, &f.R);
			ComposeAffineTransform(&Camera(), &f.R, &arg.m);
			arg.depth = -0x80;
			arg.morph = TexBase() + MORPH;
			prim::play((prim::Layout *)b->impact, PrimCallback, (int)&arg, 0);
		}
		if ((uint32_t)(c - 0x24) < 2)
		{
			for (int k = 0; k < 8; k++)
			{
				SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
				if (!s) continue;
				ff8fx::copy<(4)>(s->pos, b->spawn);
				*(uint32_t *)&s->pos[2] = *(const uint32_t *)&b->spawn[2];
				s->counter = 0;
				s->vel[0] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
				s->vel[1] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
				s->vel[2] = (int16_t)((CrtRand() & 0x1FF) - 0x100);
				s->drag[0] = (int16_t)(s->vel[0] / -12);
				s->drag[1] = (int16_t)(s->vel[1] / -12);
				s->drag[2] = (int16_t)(s->vel[2] / -12);
			}
		}
		if (b->counter == 0x2B) ApplyActionResultToTarget(Ctx()->actions[0].targets);
		if (b->counter >= 0x32) return TASK_END;
		b->counter++;
		return 0;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *m = (MasterNode *)n;
		if (m->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			m->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			m->arena = 1;
		}
		if (m->counter == 1)
		{
			if (!m->started)
			{
				m->started = 1;
				InitTaskQueuePool(QBeam(), (void *)(TexBase() + POOL_Beam), 0x664, 1);
				InitTaskQueuePool(QGlows(), (void *)(TexBase() + 0x780), 0x14, 0x10);
				InitTaskQueuePool(QSparks(), (void *)(TexBase() + 0x540), 0x24, 0x10);
				InitTaskQueuePool(QHits(), (void *)TexBase(), 0xA8, 8);
			}
			if (m->index <= Ctx()->actions[0].last_action)
			{
				uint8_t *ent = Entity(Ctx()->actions[m->index].targets[0]);
				const BeamNode *live = (const BeamNode *)(TexBase() + POOL_Beam);
				if ((live->hdr.flags & 1) && live->ent == ent) m->counter = 0;
				else
				{
					BeamNode *b = (BeamNode *)AddTaskToQueue(QBeam(), game_addr(ORIG_BeamTask));
					if (b)
					{
						Memset32(&b->counter, 0, 0x196);
						b->index = m->index;
						b->ent = ent;
						GetEffectSpawnPosition(ent, 0xF1, 0, b->spawn);
						m->index++;
						DecodeModelPrimLayout(game_addr(MODEL_Muzzle), b->muzzle, 0x3B0);
						DecodeModelPrimLayout(game_addr(MODEL_Ray), b->ray, 0xE8);
						DecodeModelPrimLayout(game_addr(MODEL_Impact), b->impact, 0x1B0);
					}
				}
			}
		}
		int a0, a1, a2, a3;
		if (m->started)
		{
			a0 = ExecuteTaskQueue(QBeam());
			a1 = ExecuteTaskQueue(QGlows());
			a2 = ExecuteTaskQueue(QSparks());
			a3 = ExecuteTaskQueue(QHits());
		}
		else a0 = a1 = a2 = a3 = (int)n;
		if (m->started && a0 == 0 && a1 == 0 && a2 == 0 && a3 == 0) return TASK_END;
		m->counter++;
		return 0;
	}
}

	void register_mag277_funguar_laser()
	{
		register_port(laser277::ORIG_MasterTask, (void *)laser277::MasterTask, "277 MasterTask", 277);
		register_port(laser277::ORIG_BeamTask, (void *)laser277::BeamTask, "277 BeamTask", 277);
		register_port(laser277::ORIG_HitTask, (void *)laser277::HitTask, "277 HitTask", 277);
		register_port(laser277::ORIG_SparkTask, (void *)laser277::SparkTask, "277 SparkTask", 277);
		register_port(laser277::ORIG_GlowTask, (void *)laser277::GlowTask, "277 GlowTask", 277);
	}
}

