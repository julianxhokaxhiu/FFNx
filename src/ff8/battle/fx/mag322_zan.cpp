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

// Effect 322: Zan

#include "mag_common.h"

namespace ff8fx
{
namespace zan322
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_322_Zan_CasterEntity)); }
	inline uint32_t &TexFile() { return var<uint32_t>(game_addr(game::MAG_322_Zan_TextureFile)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_322_Zan_CastContext)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_322_Zan_PacketCursor)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_322_Zan_MagicBufferBase)); }
	inline TaskQueue *QDust() { return (TaskQueue *)game_addr(game::MAG_322_Zan_DustQueue); }
	inline TaskQueue *QStreaks() { return (TaskQueue *)game_addr(game::MAG_322_Zan_StreaksQueue); }
	inline TaskQueue *QHit() { return (TaskQueue *)game_addr(game::MAG_322_Zan_HitQueue); }
	inline TaskQueue *QTrails() { return (TaskQueue *)game_addr(game::MAG_322_Zan_TrailsQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_322_Zan_MainQueue); }
	static const int16_t *ZERO_VECTOR() { return (const int16_t *)game_addr(game::MAG_322_Zan_ZeroVector); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_322_Zan_MasterTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_322_Zan_MainTask;
	static constexpr game::Id ORIG_DustTask = game::MAG_322_Zan_DustTask;
	static constexpr game::Id ORIG_StreakTask = game::MAG_322_Zan_StreakTask;
	static constexpr game::Id ORIG_HitTask = game::MAG_322_Zan_HitTask;
	static constexpr game::Id ORIG_TrailTask = game::MAG_322_Zan_TrailTask;
	static const uint32_t POOL_Trails = 0x7C;
	static const uint32_t POOL_Hit = 0x1FC;
	static const uint32_t POOL_Streaks = 0x424;
	static const uint32_t POOL_Dust = 0x9A4;
	static const uint32_t STREAM_STATE = 0xCA4;
	static const uint32_t ARENA_A = 0xCEC;
	static const uint32_t ARENA_B = 0xECEC;
	static const uint32_t STREAM_DEST = 0x1CCEC;
	static const uint32_t MORPH = 0x1CCF0;
	static constexpr game::Id MODEL_Blade = game::MAG_322_Zan_ModelBlade;
	static constexpr game::Id MODEL_Trail = game::MAG_322_Zan_ModelTrail;
	static constexpr game::Id MODEL_Hit = game::MAG_322_Zan_ModelHit;
	static constexpr game::Id SEQ_Streak = game::MAG_322_Zan_SeqStreak;
	static constexpr game::Id SEQ_Dust = game::MAG_322_Zan_SeqDust;
	static const void *SOUND_Throw() { return (const void *)game_addr(game::MAG_322_Zan_SoundThrow); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline int32_t ISqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline int32_t CartesianToGameAngle(int32_t a, int32_t b) { return fn<int32_t (__cdecl *)(int32_t, int32_t)>(game_addr(game::CartesianToGameAngle))(a, b); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void RotX(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_063_RotationXMatrix))(angle, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MulMatrix3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void MatrixMulVector(const Mat4x3 *m, const int16_t *v, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, v, out); }
	inline void BlendVectors(const int16_t *a, const int16_t *b, int32_t wa, int32_t wb, int16_t *out) { fn<void (__cdecl *)(const int16_t *, const int16_t *, int32_t, int32_t, int16_t *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }
	inline void GteLoadIRDelta(const int16_t *a, const int16_t *b) { fn<void (__cdecl *)(const int16_t *, const int16_t *)>(game_addr(game::GTE_LoadIRDelta))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline int32_t BuildOrthonormalBasis(int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline void StreamQueueLoad(int32_t id, uint32_t dst, int32_t kind) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, kind); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamPreload(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::BattleFile_PreLoad_IsLoaded))(a); }

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
		int16_t spin;
		uint8_t *target;
		int16_t tpos[4];
		int16_t path[7][4];
		uint8_t pad54[0x10];
		int16_t cur[4];
		int16_t prev[4];
		uint8_t pad74[8];
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
		uint8_t layout[0x210];
	};
	struct StreakNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t counter;
		int16_t vel[3];
		int16_t pad1A;
		int16_t drag[3];
		int16_t pad22;
		int16_t v0[4];
	};
	struct DustNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t counter;
		int16_t height;
		int16_t pad16;
	};
	struct PrimArg
	{
		int16_t pos[4];
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(MainNode) == 0x7C && sizeof(TrailNode) == 0x30, "Zan nodes");
	static_assert(sizeof(HitNode) == 0x228 && sizeof(StreakNode) == 0x2C && sizeof(DustNode) == 0x18, "Zan nodes");
	static_assert(sizeof(PrimArg) == 0x10, "Zan prim block");

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

	static void BladeDraw(const int16_t *cur, int32_t *dir, int16_t spin, Mat4x3 *frame)
	{
		Mat4x3 basis;
		RotX(spin, frame);
		BuildOrthonormalBasis(dir, &basis);
		MatrixMultiply(&basis, frame);
		frame->t[0] = cur[0];
		frame->t[1] = cur[1];
		frame->t[2] = cur[2];
		uint8_t *h = AllocHeader(0x58);
		Mat4x3 cam;
		ComposeAffineTransform(&Camera(), frame, &cam);
		GteSetRotMatrix(&cam);
		GteSetTransVector(&cam);
		*(uint32_t *)(h + 0x1C) = 0;
		*(uint32_t *)h = game_addr(MODEL_Blade);
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
		*(uint32_t *)(h + 0x1C) = 0xF0;
		*(uint32_t *)h = game_addr(MODEL_Trail);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void StreakDraw(const int16_t *pos, const int16_t *v0, int16_t frame)
	{
		uint8_t *h = AllocHeader(0xB4);
		int16_t d[4] = { (int16_t)(pos[0] - v0[0]), (int16_t)(pos[1] - v0[1]), (int16_t)(pos[2] - v0[2]), 0 };
		int16_t p1[4] = {}, p2[4] = {};
		TransformCameraByShadowRotation(d, 0x400, -0x100);
		GteLoadV0(ZERO_VECTOR());
		GteMVMVA_RotV0Tr();
		GteStoreIR123(p1);
		TransformCameraByShadowRotation(pos, 0x400, -0x100);
		GteLoadV0(v0);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(p2);
		*(int32_t *)(h + 8) = -CartesianToGameAngle((int32_t)p1[1] - p2[1], (int32_t)p2[0] - p1[0]);
		*(uint32_t *)h = game_addr(SEQ_Streak);
		*(int16_t *)(h + 0x24) = 1;
		*(int16_t *)(h + 4) = frame;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void DustDraw(const int16_t *pos, int16_t height, int16_t frame)
	{
		TransformCameraByShadowRotation(pos, height, -(height >> 2));
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Dust);
		*(int16_t *)(h + 4) = frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}
}
}

namespace ff8fx
{
namespace zan322
{
	static uint32_t __cdecl DustTask(TaskNode *n)
	{
		DustNode *d = (DustNode *)n;
		DustDraw(d->pos, d->height, d->counter);
		d->counter++;
		return d->counter < 0xC ? 0 : TASK_END;
	}

	static uint32_t __cdecl StreakTask(TaskNode *n)
	{
		StreakNode *s = (StreakNode *)n;
		StreakDraw(s->pos, s->v0, s->counter);
		s->counter++;
		if (s->counter >= 8) return TASK_END;
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

	static void BuildPath(MainNode *m)
	{
		int16_t bone[4] = {};
		GetEffectSpawnPosition(Caster(), 0x18, 0x800, bone);
		int32_t v[3] = { (int32_t)bone[0] - m->tpos[0], 0, (int32_t)m->tpos[2] - bone[2] };
		Mat4x3 basis;
		BuildOrthonormalBasis(v, &basis);
		basis.t[0] = m->tpos[0];
		basis.t[1] = m->tpos[1];
		basis.t[2] = m->tpos[2];
		ff8fx::copy<(8)>(m->path[0], bone);
		ff8fx::copy<(8)>(m->path[1], bone);
		GteSetRotMatrix(&basis);
		GteSetTransVector(&basis);
		int16_t off[4] = { 0, 0, -1000, 0 };
		GteLoadV0(off);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(m->path[2]);
		m->path[2][1] = (int16_t)(m->path[2][1] + 300);
		ff8fx::copy<(8)>(m->path[3], m->tpos);
		off[2] = 1000;
		GteLoadV0(off);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(m->path[4]);
		m->path[4][1] = (int16_t)(m->path[4][1] - 600);
		ff8fx::copy<(8)>(m->path[5], bone);
		ff8fx::copy<(8)>(m->path[6], bone);
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		if (m->counter == 0xD)
		{
			GetEffectSpawnPosition(Caster(), 0x18, 0x800, m->cur);
			ff8fx::copy<(8)>(m->prev, m->cur);
		}
		const int32_t k = m->counter - 0xE;
		int32_t seg = 0, t = 0;
		if ((uint32_t)k < 0x14)
		{
			if (k == 0) BuildPath(m);
			const int32_t u = (int32_t)((uint32_t)(k << 12) / 20u) + 0x66;
			t = u * u >> 12;
			seg = 0;
		}
		else if ((uint32_t)(m->counter - 0x22) < 4)
		{
			const int32_t w = (int32_t)((uint32_t)(m->counter - 0x22) << 11) + 0x200;
			seg = (w >> 12) + 1;
			t = w & 0xFFF;
		}
		else if ((uint32_t)(m->counter - 0x26) < 0x14)
		{
			t = (int32_t)((uint32_t)((m->counter - 0x26) << 12) / 20u) + 0x66;
			seg = 3;
		}
		if ((uint32_t)k < 0x3A)
		{
			int16_t cur[4] = {};
			PathPoint(m, seg, t, cur);
			if (m->cur[0] != cur[0] || m->cur[2] != cur[2]) ff8fx::copy<(8)>(m->prev, m->cur);
			m->spin = (int16_t)(m->spin + 0x400);
			ff8fx::copy<(8)>(m->cur, cur);
			*(uint32_t *)(Caster() + 0x7C) &= 0xFFFFFFFDu;
			int32_t dir[3] = { (int32_t)m->prev[0] - cur[0], 0, (int32_t)cur[2] - m->prev[2] };
			Mat4x3 frame;
			BladeDraw(cur, dir, m->spin, &frame);
			TrailNode *tr = (TrailNode *)AddTaskToQueue(QTrails(), game_addr(ORIG_TrailTask));
			if (tr)
			{
				tr->counter = 0;
				ff8fx::copy<(0x12)>(&tr->m, &frame);
				ff8fx::copy<(sizeof(frame.t))>(tr->m.t, frame.t);
				if ((uint32_t)k < 0x10) tr->fade = (int16_t)(k << 8);
				else if ((uint32_t)k > 0x24) tr->fade = (int16_t)((0x2C - k) << 9);
				else tr->fade = 0x1000;
			}
			if (cur[1] > -0x200)
			{
				const int16_t height = (int16_t)(cur[1] * 8 + 0x1800);
				DustNode *d = (DustNode *)AddTaskToQueue(QDust(), game_addr(ORIG_DustTask));
				if (d)
				{
					ff8fx::copy<(8)>(d->pos, cur);
					d->pos[1] = 0;
					d->counter = 0;
					d->height = height;
				}
				d = (DustNode *)AddTaskToQueue(QDust(), game_addr(ORIG_DustTask));
				if (d)
				{
					d->pos[0] = (int16_t)(((int32_t)m->prev[0] + cur[0]) >> 1);
					d->pos[1] = 0;
					d->pos[2] = (int16_t)(((int32_t)m->prev[2] + cur[2]) >> 1);
					d->counter = 0;
					d->height = height;
				}
			}
			if (m->counter == 0x24)
			{
				uint8_t *ent = m->target;
				HitNode *h = (HitNode *)AddTaskToQueue(QHit(), game_addr(ORIG_HitTask));
				if (h)
				{
					h->ent = ent;
					GetDefaultEffectPosition(ent, h->pos);
					DecodeModelPrimLayout(game_addr(MODEL_Hit), h->layout, 0x210);
				}
				Mat4x3 basis;
				BuildOrthonormalBasis(dir, &basis);
				int32_t angle = 0;
				cur[3] = 0;
				for (int i = 0; i < 0x20; i++)
				{
					StreakNode *s = (StreakNode *)AddTaskToQueue(QStreaks(), game_addr(ORIG_StreakTask));
					if (!s) continue;
					ff8fx::copy<(8)>(s->pos, cur);
					const int32_t speed = (CrtRand() & 0x3F) + 0x40;
					s->vel[0] = (int16_t)(mul32(ComputeSin(angle), speed) / 0x1000);
					s->vel[1] = (int16_t)(mul32(ComputeCos(angle), speed) / 0x1000);
					s->vel[2] = (int16_t)((CrtRand() & 0x3F) + 0x40);
					MatrixMulVector(&basis, s->vel, s->vel);
					ff8fx::copy<(8)>(s->v0, s->vel);
					s->drag[0] = (int16_t)((int32_t)s->vel[0] / -12);
					s->drag[1] = (int16_t)((int32_t)s->vel[1] / -12);
					s->drag[2] = (int16_t)((int32_t)s->vel[2] / -12);
					angle += 0x80;
				}
				ApplyActionResultToTarget(Ctx()->actions[0].targets);
			}
		}
		if (m->counter == 0xF) BdPlaySE(SOUND_Throw(), 0, 0x80);
		if (m->counter == 0x1E)
		{
			int16_t bone[4] = {};
			GetEffectSpawnPosition(Caster(), 0x18, 0x800, bone);
			ff8fx::copy<(8)>(m->path[5], bone);
			ff8fx::copy<(8)>(m->path[6], bone);
		}
		m->counter++;
		if (m->counter < 0x39) return 0;
		*(uint32_t *)(Caster() + 0x7C) |= 2;
		return TASK_END;
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
			InitTaskQueuePool(QMain(), (void *)TexBase(), 0x7C, 1);
			InitTaskQueuePool(QTrails(), (void *)(TexBase() + POOL_Trails), 0x30, 8);
			InitTaskQueuePool(QHit(), (void *)(TexBase() + POOL_Hit), 0x228, 1);
			InitTaskQueuePool(QStreaks(), (void *)(TexBase() + POOL_Streaks), 0x2C, 0x20);
			InitTaskQueuePool(QDust(), (void *)(TexBase() + POOL_Dust), 0x18, 0x20);
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			if (m)
			{
				Memset32(&m->counter, 0, 0x1C);
				m->target = Entity(Ctx()->actions[0].targets[0]);
				GetDefaultEffectPosition(m->target, m->tpos);
			}
			StreamStateInit((void *)(TexBase() + STREAM_STATE));
		}
		if (ms->started)
		{
			ExecuteTaskQueue(QMain());
			ExecuteTaskQueue(QTrails());
			ExecuteTaskQueue(QHit());
			ExecuteTaskQueue(QStreaks());
			ExecuteTaskQueue(QDust());
		}
		if (ms->counter == 0x3B) StreamQueueLoad(0x168, TexBase() + STREAM_DEST, 0);
		if (ms->started) StreamPump();
		if (ms->counter > 0x3B)
		{
			if (StreamPreload(0) == 0) return 0;
			return TASK_END;
		}
		ms->counter++;
		return 0;
	}
}

	void register_mag322_zan()
	{
		register_port(zan322::ORIG_MasterTask, (void *)zan322::MasterTask, "322 MasterTask", 322);
		register_port(zan322::ORIG_MainTask, (void *)zan322::MainTask, "322 MainTask", 322);
		register_port(zan322::ORIG_TrailTask, (void *)zan322::TrailTask, "322 TrailTask", 322);
		register_port(zan322::ORIG_HitTask, (void *)zan322::HitTask, "322 HitTask", 322);
		register_port(zan322::ORIG_StreakTask, (void *)zan322::StreakTask, "322 StreakTask", 322);
		register_port(zan322::ORIG_DustTask, (void *)zan322::DustTask, "322 DustTask", 322);
	}
}

