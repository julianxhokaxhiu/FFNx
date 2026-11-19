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
namespace boomerang304
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g304_CasterEntity)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g304_MagicBufferBase)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g304_CastContext)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g304_PacketCursor)); }
	inline TaskQueue *QSparks() { return (TaskQueue *)game_addr(game::g304_SparkQueue); }
	inline TaskQueue *QHits() { return (TaskQueue *)game_addr(game::g304_HitQueue); }
	inline TaskQueue *QTrails() { return (TaskQueue *)game_addr(game::g304_TrailQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::g304_MainQueue); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_304_BoomerangSword_MasterTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_304_BoomerangSword_MainTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_304_BoomerangSword_SparkTask;
	static constexpr game::Id ORIG_HitTask = game::MAG_304_BoomerangSword_HitTask;
	static constexpr game::Id ORIG_TrailTask = game::MAG_304_BoomerangSword_TrailTask;
	static const uint32_t POOL_Trails = 0xAC;
	static const uint32_t POOL_Hits = 0x6AC;
	static const uint32_t POOL_Sparks = 0xC58;
	static const uint32_t MORPH = 0x1558;
	static const uint32_t ARENA_A = 0x16F0;
	static const uint32_t ARENA_B = 0x116F0;
	static constexpr game::Id MODEL_Sword = game::unk_102EA98;
	static constexpr game::Id MODEL_Trail = game::unk_102F2B4;
	static constexpr game::Id MODEL_Hit = game::unk_102F48C;
	static constexpr game::Id SEQ_Spark = game::unk_102E94C;
	static const void *SOUND_Throw() { return (const void *)game_addr(game::unk_10312C8); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline int32_t ISqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void RotX(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_063_RotationXMatrix))(angle, out); }
	inline void RotZ(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_065_RotationZMatrix))(angle, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MulMatrix3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void MatrixMulVector(const Mat4x3 *m, const int16_t *v, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, v, out); }
	inline void Apply3D(const Mat4x3 *m, const int16_t *in, int32_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int32_t *)>(game_addr(game::apply3DTransform))(m, in, out); }
	inline void BlendVectors(const int16_t *a, const int16_t *b, int32_t wa, int32_t wb, int16_t *out) { fn<void (__cdecl *)(const int16_t *, const int16_t *, int32_t, int32_t, int16_t *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }
	inline void GteLoadIRDelta(const int16_t *a, const int16_t *b) { fn<void (__cdecl *)(const int16_t *, const int16_t *)>(game_addr(game::GTE_LoadIRDelta))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline int32_t BuildOrthonormalBasis(int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }

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
		int16_t count;
		int16_t next;
		int16_t minx;
		int16_t maxx;
		int16_t meany;
		int16_t meanz;
		int16_t spin;
		int16_t tilt;
		int16_t pad1E;
		uint8_t *ents[3];
		int16_t tpos[3][4];
		int16_t path[9][4];
		int16_t spawn[4];
		int16_t cur[4];
		int16_t prev[4];
		uint8_t padA4[8];
	};
	struct TrailNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t fade;
		Mat4x3 m;
	};
	struct HitNode
	{
		TaskNode hdr;
		uint8_t *ent;
		int16_t pos[4];
		uint8_t layout[0x1CC];
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
		int16_t pos[4];
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(MainNode) == 0xAC && sizeof(TrailNode) == 0x30, "Boomerang nodes");
	static_assert(sizeof(HitNode) == 0x1E4 && sizeof(SparkNode) == 0x24, "Boomerang nodes");
	static_assert(sizeof(PrimArg) == 0x10, "Boomerang prim block");

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
		if (r->flags & 0x200)
		{
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)(int32_t)r->pos[0]);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)(int32_t)r->pos[1]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)(int32_t)r->pos[2]);
		}
		else
		{
			const int16_t off[4] = { (int16_t)(r->pos[0] + arg->pos[0]), (int16_t)(r->pos[1] + arg->pos[1]), (int16_t)(r->pos[2] + arg->pos[2]), 0 };
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			MatrixMultiply(&Camera(), &m);
		}
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (r->flags & 0x100)
			{
				Mat4x3 d = {};
				d.m[0][0] = r->scale[0];
				d.m[1][1] = r->scale[1];
				d.m[2][2] = r->scale[2];
				MulMatrix3(&m, &d);
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

	static void PathPoint(const MainNode *m, int32_t seg, int32_t t, int16_t *out)
	{
		const int16_t *P = m->path[seg], *A = m->path[seg + 1], *B = m->path[seg + 2], *C = m->path[seg + 3];
		int32_t sq;
		GteLoadIRDelta(A, B);
		GteSQR();
		GteSumMAC123(&sq);
		const int32_t len = ISqrt(sq);
		int16_t a[4] = {};
		a[0] = (int16_t)(A[0] - P[0]);
		a[1] = (int16_t)(A[1] - P[1]);
		a[2] = (int16_t)(A[2] - P[2]);
		GteLoadIR123(a);
		GteSQR();
		GteSumMAC123(&sq);
		int32_t l0 = ISqrt(sq);
		if (l0 > 0)
		{
			a[0] = (int16_t)(mul32(a[0], len) / l0 + A[0]);
			a[1] = (int16_t)(mul32(a[1], len) / l0 + A[1]);
			a[2] = (int16_t)(mul32(a[2], len) / l0 + A[2]);
			BlendVectors(A, a, 0x1000 - t, t, a);
		}
		else ff8fx::copy<(8)>(a, A);
		int16_t b[4] = {};
		b[0] = (int16_t)(B[0] - C[0]);
		b[1] = (int16_t)(B[1] - C[1]);
		b[2] = (int16_t)(B[2] - C[2]);
		GteLoadIR123(b);
		GteSQR();
		GteSumMAC123(&sq);
		l0 = ISqrt(sq);
		if (l0 > 0)
		{
			b[0] = (int16_t)(mul32(b[0], len) / l0 + B[0]);
			b[1] = (int16_t)(mul32(b[1], len) / l0 + B[1]);
			b[2] = (int16_t)(mul32(b[2], len) / l0 + B[2]);
			BlendVectors(B, b, t, 0x1000 - t, b);
		}
		else ff8fx::copy<(8)>(b, B);
		BlendVectors(a, b, 0x1000 - t, t, out);
	}

	static void SwordDraw(const int16_t *cur, int32_t *dir, int16_t spin, int16_t tilt, Mat4x3 *frame)
	{
		Mat4x3 rz, basis;
		RotX(spin, frame);
		RotZ(tilt, &rz);
		BuildOrthonormalBasis(dir, &basis);
		MatrixMultiply(&rz, frame);
		MatrixMultiply(&basis, frame);
		const int16_t raise[4] = { 0, 0, 0xA0, 0 };
		Apply3D(frame, raise, frame->t);
		frame->t[0] = (int32_t)((uint32_t)frame->t[0] + (uint32_t)(int32_t)cur[0]);
		frame->t[1] = (int32_t)((uint32_t)frame->t[1] + (uint32_t)(int32_t)cur[1]);
		frame->t[2] = (int32_t)((uint32_t)frame->t[2] + (uint32_t)(int32_t)cur[2]);
		uint8_t *h = AllocHeader(0x58);
		Mat4x3 cam;
		ComposeAffineTransform(&Camera(), frame, &cam);
		GteSetRotMatrix(&cam);
		GteSetTransVector(&cam);
		*(uint32_t *)(h + 0x1C) = 0;
		*(uint32_t *)h = game_addr(MODEL_Sword);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void TrailDraw(const TrailNode *t, int32_t angle)
	{
		uint8_t *h = AllocHeader(0x58);
		Mat4x3 cam;
		ComposeAffineTransform(&Camera(), &t->m, &cam);
		GteSetRotMatrix(&cam);
		GteSetTransVector(&cam);
		int32_t f = ComputeSin(angle);
		*(int32_t *)(h + 0xC) = f;
		if (t->fade < 0x1000)
		{
			f = 0x1000 - ((0x1000 - f) * t->fade >> 12);
			*(int32_t *)(h + 0xC) = f;
		}
		h[9] = 0;
		h[8] = 0;
		int32_t blue = (0x1000 - f) >> 4;
		if (blue > 0xFF) blue = 0xFF;
		h[0xA] = (uint8_t)blue;
		*(uint32_t *)(h + 0x1C) = 0xC0;
		*(uint32_t *)h = game_addr(MODEL_Trail);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void SpriteDraw(int16_t *pos, int16_t frame)
	{
		TransformCameraByShadowRotation(pos, 0x800, -0x200);
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Spark);
		*(int16_t *)(h + 4) = frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static int16_t PointResidue()
	{
		return 0;
	}
}
}

namespace ff8fx
{
namespace boomerang304
{
	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *s = (SparkNode *)n;
		SpriteDraw(s->pos, s->counter);
		s->counter++;
		if (s->counter >= 0xA) return TASK_END;
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
		s->vel[0] = (int16_t)(s->drag[0] + s->vel[0]);
		s->vel[1] = (int16_t)(s->drag[1] + s->vel[1]);
		s->vel[2] = (int16_t)(s->drag[2] + s->vel[2]);
		return 0;
	}

	static uint32_t __cdecl HitTask(TaskNode *n)
	{
		HitNode *h = (HitNode *)n;
		PrimArg arg;
		ff8fx::copy<(8)>(arg.pos, h->pos);
		arg.depth = -0x80;
		arg.morph = TexBase() + MORPH;
		return prim::play((prim::Layout *)h->layout, PrimCallback, (int)&arg, 0) ? 0 : TASK_END;
	}

	static uint32_t __cdecl TrailTask(TaskNode *n)
	{
		TrailNode *t = (TrailNode *)n;
		TrailDraw(t, (int32_t)t->counter * 0x400 / 8);
		t->counter++;
		return t->counter < 8 ? 0 : TASK_END;
	}

	static void ApplyResultsOf(const uint8_t *ent)
	{
		const int32_t slot = (int32_t)((uint32_t)ent - game_addr(game::BattleEntitySlotData)) / 0x9C;
		for (uint32_t i = 0; i < Ctx()->actions[0].target_count; i++)
		{
			uint8_t *rec = Ctx()->actions[0].targets + i * TARGET_STRIDE;
			if ((int32_t)rec[0] == slot) ApplyActionResultToTarget(rec);
		}
	}

	static void BuildPath(MainNode *m)
	{
		GetEffectSpawnPosition(Caster(), 0x17, 0, m->spawn);
		int16_t (*p)[4] = m->path;
		ff8fx::copy<(8)>(*p++, m->spawn);
		ff8fx::copy<(8)>(*p++, m->spawn);
		(*p)[0] = m->maxx;
		(*p)[1] = (int16_t)(m->meany - 1000);
		(*p)[2] = m->meanz;
		p++;
		for (int32_t i = 0; i < m->count; i++) ff8fx::copy<(8)>(*p++, m->tpos[i]);
		(*p)[0] = m->minx;
		(*p)[1] = m->meany;
		(*p)[2] = m->meanz;
		p++;
		(*p)[0] = (int16_t)(m->minx - 1000);
		(*p)[1] = (int16_t)(m->meany - 3000);
		(*p)[2] = m->meanz;
		p++;
		(*p)[0] = (int16_t)(m->minx - 1000);
		(*p)[1] = (int16_t)(m->meany - 3000);
		(*p)[2] = m->meanz;
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		if (m->counter == 4)
		{
			GetEffectSpawnPosition(Caster(), 0x17, 0, m->cur);
			ff8fx::copy<(8)>(m->prev, m->cur);
		}
		const int32_t k = m->counter - 5;
		int32_t seg = 0, t = 0;
		if ((uint32_t)k < 0x14)
		{
			if (k == 0) BuildPath(m);
			t = (int32_t)((uint32_t)(k << 12) / 20u) + 0x66;
			seg = 0;
			m->tilt = (int16_t)(m->tilt + 0x33);
		}
		else if ((uint32_t)(m->counter - 0x19) < 0xF)
		{
			const uint32_t v = (uint32_t)mul32(m->count + 1, m->counter - 0x19) << 12;
			const int32_t w = (int32_t)(v / 15u) + 0x88;
			m->tilt = 0x400;
			seg = (w >> 12) + 1;
			t = w & 0xFFF;
		}
		else if ((uint32_t)(m->counter - 0x28) < 0x14)
		{
			t = (int32_t)((uint32_t)((m->counter - 0x28) << 12) / 20u) + 0x66;
			seg = m->count + 2;
			m->tilt = (int16_t)(m->tilt - 0x33);
		}
		if ((uint32_t)k < 0x3C)
		{
			int16_t cur[4];
			PathPoint(m, seg, t, cur);
			cur[3] = PointResidue();
			if (m->cur[0] != cur[0] || m->cur[1] != cur[1] || m->cur[2] != cur[2]) ff8fx::copy<(8)>(m->prev, m->cur);
			m->spin = (int16_t)(m->spin + 0x400);
			ff8fx::copy<(8)>(m->cur, cur);
			*(uint32_t *)(Caster() + 0x7C) &= 0xFFFFFFFDu;
			int32_t dir[3] = { (int32_t)m->prev[0] - cur[0], 0, (int32_t)cur[2] - m->prev[2] };
			Mat4x3 frame;
			SwordDraw(cur, dir, m->spin, m->tilt, &frame);
			TrailNode *tr = (TrailNode *)AddTaskToQueue(QTrails(), game_addr(ORIG_TrailTask));
			if (tr)
			{
				tr->counter = 0;
				tr->m = frame;
				if ((uint32_t)k < 0x10) tr->fade = (int16_t)(k << 8);
				else if ((uint32_t)k > 0x2C) tr->fade = (int16_t)((0x3C - k) << 8);
				else tr->fade = 0x1000;
			}
			if (m->counter >= 0x19 && m->counter < 0x28 && m->next < m->count)
			{
				uint8_t *ent = m->ents[m->next];
				if (cur[0] < *(const int16_t *)(ent + 0x1C))
				{
					HitNode *h = (HitNode *)AddTaskToQueue(QHits(), game_addr(ORIG_HitTask));
					ApplyResultsOf(ent);
					if (h)
					{
						h->ent = ent;
						GetDefaultEffectPosition(ent, h->pos);
						DecodeModelPrimLayout(game_addr(MODEL_Hit), h->layout, 0x1CC);
					}
					int32_t angle = 0;
					Mat4x3 basis;
					BuildOrthonormalBasis(dir, &basis);
					cur[3] = 0;
					for (int i = 0; i < 0x15; i++)
					{
						SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
						if (!s) continue;
						ff8fx::copy<(4)>(s->pos, cur);
						ff8fx::copy<(4)>(&s->pos[2], &cur[2]);
						const int32_t speed = (CrtRand() & 0x3F) + 0x40;
						s->vel[0] = (int16_t)(mul32(ComputeSin(angle), speed) / 0x1000);
						s->vel[1] = (int16_t)(mul32(ComputeCos(angle), speed) / 0x1000);
						s->vel[2] = (int16_t)((CrtRand() & 0x3F) + 0x40);
						MatrixMulVector(&basis, s->vel, s->vel);
						s->drag[0] = (int16_t)((int32_t)s->vel[0] / -12);
						s->drag[1] = (int16_t)((int32_t)s->vel[1] / -12);
						s->drag[2] = (int16_t)((int32_t)s->vel[2] / -12);
						angle += 0xC3;
					}
					m->next++;
				}
			}
		}
		if (m->counter == 5) BdPlaySE(SOUND_Throw(), 0, 0x80);
		m->counter++;
		return m->counter < 0x3B ? 0 : TASK_END;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *ms = (MasterNode *)n;
		if (ms->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			ms->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			ms->arena = 1;
		}
		if (ms->counter == 1 && !ms->started)
		{
			ms->started = 1;
			InitTaskQueuePool(QMain(), (void *)TexBase(), 0xAC, 1);
			InitTaskQueuePool(QTrails(), (void *)(TexBase() + POOL_Trails), 0x30, 0x20);
			InitTaskQueuePool(QHits(), (void *)(TexBase() + POOL_Hits), 0x1E4, 3);
			InitTaskQueuePool(QSparks(), (void *)(TexBase() + POOL_Sparks), 0x24, 0x40);
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			if (m)
			{
				Memset32(&m->counter, 0, 0x28);
				const ActionData &a = Ctx()->actions[0];
				m->count = a.target_count;
				for (int32_t i = 0; i < m->count; i++) m->ents[i] = Entity(a.targets[i * TARGET_STRIDE]);
				for (int32_t i = 0; i + 1 < m->count; i++)
					for (int32_t j = i + 1; j < m->count; j++)
						if (*(const int16_t *)(m->ents[i] + 0x1C) < *(const int16_t *)(m->ents[j] + 0x1C))
						{
							uint8_t *e = m->ents[i];
							m->ents[i] = m->ents[j];
							m->ents[j] = e;
						}
				m->minx = -1000;
				m->maxx = 1000;
				m->meanz = 0;
				m->meany = 0;
				for (int32_t i = 0; i < m->count; i++)
				{
					GetDefaultEffectPosition(m->ents[i], m->tpos[i]);
					m->meany = (int16_t)(m->meany + m->tpos[i][1]);
					m->meanz = (int16_t)(m->meanz + m->tpos[i][2]);
					if (m->minx > m->tpos[i][0]) m->minx = m->tpos[i][0];
					if (m->maxx < m->tpos[i][0]) m->maxx = m->tpos[i][0];
				}
				const int32_t y = (int32_t)m->meany / m->count;
				m->minx = (int16_t)(m->minx - 500);
				m->maxx = (int16_t)(m->maxx + 500);
				m->meany = (int16_t)y;
				m->meanz = (int16_t)((int32_t)m->meanz / m->count - 1000);
			}
		}
		int a0, a1, a2;
		if (ms->started)
		{
			a0 = ExecuteTaskQueue(QMain());
			a1 = ExecuteTaskQueue(QTrails());
			a2 = ExecuteTaskQueue(QHits());
			ExecuteTaskQueue(QSparks());
		}
		else a0 = a1 = a2 = (int)n;
		if (ms->started && a0 == 0 && a1 == 0 && a2 == 0)
		{
			return TASK_END;
		}
		ms->counter++;
		return 0;
	}
}

	void register_mag304_boomerang_sword()
	{
		register_port(boomerang304::ORIG_MasterTask, (void *)boomerang304::MasterTask, "304 MasterTask", 304);
		register_port(boomerang304::ORIG_MainTask, (void *)boomerang304::MainTask, "304 MainTask", 304);
		register_port(boomerang304::ORIG_TrailTask, (void *)boomerang304::TrailTask, "304 TrailTask", 304);
		register_port(boomerang304::ORIG_HitTask, (void *)boomerang304::HitTask, "304 HitTask", 304);
		register_port(boomerang304::ORIG_SparkTask, (void *)boomerang304::SparkTask, "304 SparkTask", 304);
	}
}

