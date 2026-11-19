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

// Effect 11: Storm Breath

#include "mag_common.h"

namespace ff8fx
{
namespace storm011
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g_StormBreath_Caster)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_2556148)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_255614C)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_StormBreath_PacketCursor)); }
	inline TaskQueue *QDebris() { return (TaskQueue *)game_addr(game::g_StormBreath_DebrisQueue); }
	inline TaskQueue *QRings() { return (TaskQueue *)game_addr(game::g_StormBreath_RingQueue); }
	inline TaskQueue *QSprites() { return (TaskQueue *)game_addr(game::g_StormBreath_SpriteQueue); }
	inline TaskQueue *QTargets() { return (TaskQueue *)game_addr(game::g_StormBreath_TargetQueue); }
	inline const Mat4x3 *WobbleTrans() { return (const Mat4x3 *)game_addr(game::unk_25561A4); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_011_StormBreath_RootTask;
	static constexpr game::Id ORIG_TargetTask = game::MAG_011_StormBreath_TargetTask;
	static constexpr game::Id ORIG_DebrisTask = game::MAG_011_StormBreath_DebrisTask;
	static constexpr game::Id ORIG_RingTask = game::MAG_011_StormBreath_RingTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_011_StormBreath_SparkTask;
	static constexpr game::Id ORIG_PuffTask = game::MAG_011_StormBreath_PuffTask;
	static constexpr game::Id ORIG_MistTask = game::MAG_011_StormBreath_MistTask;
	static constexpr game::Id ORIG_HitTask = game::MAG_011_StormBreath_HitTask;
	static constexpr game::Id MODEL_Stream = game::StormBreath_StreamModel;
	static constexpr game::Id STREAM_VERTS = game::unk_1365728;
	static const int STREAM_NVERTS = 0x134;
	static constexpr game::Id MODEL_Mouth = game::StormBreath_MouthLayout;
	static constexpr game::Id MODEL_Cone = game::StormBreath_ConeLayout;
	static constexpr game::Id FLAGS_Mouth = game::unk_136BAFC;
	static constexpr game::Id FLAGS_Cone = game::unk_136BB24;
	static constexpr game::Id MODEL_Debris = game::StormBreath_DebrisModel;
	static constexpr game::Id MODEL_Ring = game::StormBreath_RingModel;
	static constexpr game::Id SEQ_Spark = game::StormBreath_SparkSeq;
	static constexpr game::Id SEQ_Puff = game::StormBreath_PuffSeq;
	static constexpr game::Id SEQ_Mist = game::StormBreath_MistSeq;
	static constexpr game::Id SEQ_Hit = game::StormBreath_HitSeq;
	static const void *SOUND_Breath() { return (const void *)game_addr(game::StormBreath_Sound); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void BuildOrthonormalBasis(const int32_t *v, Mat4x3 *out) { fn<void (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_MorphVertices))(model, f0, f1, t, out); }
	inline int32_t CountMaskedVertices(uint32_t model, const uint32_t *mask) { return fn<int32_t (__cdecl *)(uint32_t, const uint32_t *)>(game_addr(game::Effect_CountMaskedModelVertices))(model, mask); }
	inline int32_t FindMaskedVertex(uint32_t model, void *state) { return fn<int32_t (__cdecl *)(uint32_t, void *)>(game_addr(game::Effect_FindMaskedModelVertex))(model, state); }
	inline uint32_t DrawTriangleList(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::Effect_DrawModelTriangleList))(h, ot, mode, cursor); }
	inline uint32_t DrawQuadList(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::Effect_DrawModelQuadList))(h, ot, mode, cursor); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct TargetNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t count;
		uint8_t *ent[3];
		uint8_t order[4];
		int16_t cur;
		int16_t next;
		int16_t t;
		int16_t dt;
		int16_t a;
		int16_t b;
		int16_t vc;
		int16_t pad2E;
		int32_t dist;
		int16_t mouth[4];
		int16_t frozen[4];
		Mat4x3 basis;
		uint8_t layout[0x4A0];
	};
	struct SpriteNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[3];
		int16_t scale;
		int16_t vel[3];
		int16_t pad1E;
	};
	struct RingNode
	{
		TaskNode hdr;
		int16_t pad0C;
		int16_t e;
		TargetNode *tn;
	};
	struct DebrisNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[3];
		int16_t scale;
		int16_t vel[3];
		int16_t pad1E;
		int16_t ang[3];
		int16_t pad26;
		int16_t spin[3];
		int16_t pad2E;
	};
	struct PrimArg
	{
		const Mat4x3 *m;
		uint32_t flags;
		uint32_t morph;
	};
	struct Ramp
	{
		int kind;
		int32_t s;
		int32_t h8;
		int32_t scroll;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(TargetNode) == 0x504, "Storm Breath nodes");
	static_assert(sizeof(SpriteNode) == 0x20 && sizeof(RingNode) == 0x14 && sizeof(DebrisNode) == 0x30, "Storm Breath nodes");
	static_assert(sizeof(PrimArg) == 0xC, "Storm Breath prim block");

	static int32_t Div12(int32_t x)
	{
		int32_t hi = (int32_t)(((int64_t)x * 0x2AAAAAAB) >> 32);
		hi >>= 1;
		return hi + (int32_t)((uint32_t)hi >> 31);
	}

	static void ZRotation(int16_t angle, Mat4x3 *m)
	{
		const int32_t s = ComputeSin(angle);
		const int32_t c = ComputeCos(angle);
		m->m[0][1] = (int16_t)-s;
		m->m[1][0] = (int16_t)s;
		m->m[0][0] = (int16_t)c;
		m->m[0][2] = 0;
		m->m[1][1] = (int16_t)c;
		m->m[1][2] = 0;
		m->m[2][0] = 0;
		m->m[2][1] = 0;
		m->m[2][2] = 0x1000;
	}

	static void YawMatrix(int16_t yaw, Mat4x3 *m)
	{
		const int32_t sn = ComputeSin(yaw);
		const int32_t cs = ComputeCos(yaw);
		m->m[0][2] = (int16_t)sn;
		m->m[2][0] = (int16_t)-sn;
		m->m[0][0] = (int16_t)cs;
		m->m[0][1] = 0;
		m->m[1][0] = 0;
		m->m[1][1] = 0x1000;
		m->m[1][2] = 0;
		m->m[2][1] = 0;
		m->m[2][2] = (int16_t)cs;
	}

	static int32_t CountVertices(uint8_t *entity)
	{
		uint8_t *a = (uint8_t *)FieldAlloc(0x38);
		uint32_t *mask = (uint32_t *)(a + 0x20);
		*mask = *(const uint32_t *)(entity + 0x7C);
		int32_t n = CountMaskedVertices(*(const uint32_t *)(entity + 0x64), mask);
		uint8_t *weapon = *(uint8_t **)(entity + 0x78);
		if (weapon)
		{
			*mask = 0xFFFFFFFF;
			n += CountMaskedVertices(*(const uint32_t *)(weapon + 4), mask);
		}
		FieldFree(0x38);
		return n;
	}

	static void VertexPosition(uint8_t *entity, int32_t n, void *out)
	{
		uint8_t *a = (uint8_t *)FieldAlloc(0x38);
		*(int32_t *)(a + 0x24) = n;
		*(uint32_t *)(a + 0x20) = *(const uint32_t *)(entity + 0x7C);
		ComputeBonesWorldMatrices(entity + 0x60, entity + 0x40);
		const int32_t found = FindMaskedVertex(*(const uint32_t *)(entity + 0x64), a + 0x20);
		BuildBoneMatricesFromPose(entity + 0x60);
		if (found == 0)
		{
			uint8_t *weapon = *(uint8_t **)(entity + 0x78);
			if (weapon)
			{
				*(uint32_t *)(a + 0x20) = 0xFFFFFFFF;
				ComputeBonesWorldMatrices(weapon, entity + 0x40);
				FindMaskedVertex(*(const uint32_t *)(weapon + 4), a + 0x20);
				BuildBoneMatricesFromPose(weapon);
			}
		}
		ff8fx::copy<(8)>(out, a + 0x28);
		FieldFree(0x38);
	}

	static void __cdecl PartCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PrimArg *arg = (const PrimArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
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
		Mat4x3 m;
		ComposeZYXRotationMatrix(r->rot, &m);
		m.t[0] = r->pos[0];
		m.t[1] = r->pos[1];
		m.t[2] = r->pos[2];
		ComposeAffineTransform(arg->m, &m, &m);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			const int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
			Scale3DMatrix(&m, v);
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		const uint32_t f = (((const uint32_t *)arg->flags)[(int16_t)r->index] & 0xDFFF) | 0x2000;
		*(uint32_t *)(h + 0x1C) = f;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) = f | 0xC0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void StreamWobble(int16_t a, int16_t b, uint8_t *out)
	{
		Mat4x3 rz;
		ZRotation(a, &rz);
		GteSetRotMatrix(&rz);
		GteSetTransVector(WobbleTrans());
		const uint8_t *src = (const uint8_t *)(TexBase() + 0x1B18);
		for (int i = 0; i < STREAM_NVERTS; i++, src += 8, out += 8)
		{
			GteLoadV0(src);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(out);
			const int16_t w6 = *(const int16_t *)(src + 6);
			const int16_t ang = (int16_t)(b + w6);
			const int32_t r = ComputeSin((int32_t)w6 >> 1) >> 3;
			int16_t *o = (int16_t *)out;
			o[0] = (int16_t)(o[0] + (int16_t)(mul32(ComputeCos(ang), r) >> 12));
			o[1] = (int16_t)(o[1] - (int16_t)(mul32(ComputeSin(ang), r) >> 12));
		}
	}

	static void StreamAim(const int16_t *p0, const int16_t *p1, int16_t t, int16_t *aim)
	{
		for (int k = 0; k < 3; k++) aim[k] = (int16_t)((mul32(p1[k] - p0[k], t) >> 12) + p0[k]);
	}

	static Ramp StreamRamp(uint32_t phase)
	{
		Ramp r;
		r.scroll = (int32_t)((phase & 7) << 4);
		r.s = 0x1000;
		if (phase < 8)
		{
			r.kind = 0;
			r.s = ComputeSin((int32_t)((phase & 0x3FFFFF) << 7));
			r.h8 = 0x1000 - r.s;
		}
		else if (phase > 0x2A)
		{
			r.kind = 1;
			r.h8 = ComputeSin((int32_t)(((phase << 7) - 0x1481) & 0x1FFFFF80));
		}
		else
		{
			r.kind = 2;
			r.h8 = 0;
		}
		return r;
	}

	static uint32_t DrawStream(uint8_t *h, uint32_t ot, int32_t mode, uint32_t cursor, uint32_t verts)
	{
		const uint32_t model = *(const uint32_t *)h;
		*(uint32_t *)(h + 0x20) = verts;
		*(uint32_t *)(h + 0x24) = model + *(const uint32_t *)model + 0xC;
		cursor = DrawTriangleList(h, ot, mode, cursor);
		*(uint32_t *)(h + 0x24) += 0xC;
		return DrawQuadList(h, ot, mode, cursor);
	}

	static void StreamDraw(uint8_t *h, uint32_t phase, const Ramp *given, const int16_t *aim, const int16_t *mouth,
		int32_t *dist, Mat4x3 *basis, Mat4x3 *m1, int32_t d[3], uint32_t verts)
	{
		d[0] = (int32_t)aim[0] - mouth[0];
		d[2] = (int32_t)aim[2] - mouth[2];
		d[1] = (int32_t)aim[1] - mouth[1];
		*dist = Sqrt(mul32(d[2], d[2]) + mul32(d[1], d[1]) + mul32(d[0], d[0]));
		int16_t zs = (int16_t)*dist;
		const Ramp r = given ? *given : StreamRamp(phase);
		if (r.kind == 0)
		{
			zs = (int16_t)(mul32(zs, r.s) >> 12);
			*(int32_t *)(h + 8) = r.h8;
			*(int32_t *)(h + 4) = 0;
		}
		else if (r.kind == 1)
		{
			*(int32_t *)(h + 8) = r.h8;
			*(int32_t *)(h + 4) = 0;
		}
		else
		{
			*(int32_t *)(h + 8) = 0;
		}
		const int32_t v[3] = { (int32_t)(0u - (uint32_t)d[0]), (int32_t)(0u - (uint32_t)d[1]), d[2] };
		Mat4x3 sc;
		sc.m[0][0] = 0x1000; sc.m[0][1] = 0; sc.m[0][2] = 0;
		sc.m[1][0] = 0; sc.m[1][1] = 0x1000; sc.m[1][2] = 0;
		sc.m[2][0] = 0; sc.m[2][1] = 0; sc.m[2][2] = zs;
		BuildOrthonormalBasis(v, basis);
		*m1 = *basis;
		MatrixMultiply3(m1, &sc);
		m1->t[0] = mouth[0];
		m1->t[1] = mouth[1];
		m1->t[2] = mouth[2];
		Mat4x3 m3;
		ComposeAffineTransform(&Camera(), m1, &m3);
		GteSetRotMatrix(&m3);
		GteSetTransVector(&m3);
		*(int16_t *)(h + 0x12) = 0;
		*(int16_t *)(h + 0x10) = 0;
		*(int16_t *)(h + 0x1A) = 0;
		*(int16_t *)(h + 0x18) = 0;
		*(int16_t *)(h + 0x16) = 0x100;
		*(int16_t *)(h + 0x14) = 0x100;
		*(int16_t *)(h + 0x1C) = 0x80;
		*(int16_t *)(h + 0x1E) = 0x100;
		*(int16_t *)(h + 0xC) = (int16_t)r.scroll;
		*(uint32_t *)h = game_addr(MODEL_Stream);
		PacketCursor() = DrawStream(h, RenderOT(0x44), 2, PacketCursor(), verts);
		FieldFree(0x5C);
	}

	static void SweepStep(TargetNode *t)
	{
		t->t = (int16_t)(t->t + t->dt);
		if (t->t < 0x1000) return;
		ApplyActionResultToTarget(Ctx()->actions[0].targets + t->order[t->cur] * TARGET_STRIDE);
		if (t->cur != t->next) t->vc = (int16_t)CountVertices(t->ent[t->next]);
		t->t = (int16_t)(t->t + 0xF000);
		int16_t nx = t->next;
		t->cur = nx;
		nx = (int16_t)(nx + 1);
		t->next = nx;
		if (nx >= t->count) t->next = (int16_t)(t->count - 1);
	}

	static void SpriteDraw(const SpriteNode *p, uint32_t seq)
	{
		uint8_t *h = AllocHeader(0xB4);
		TransformCameraByShadowRotation(p->pos, p->scale, -((int32_t)p->scale >> 2));
		*(uint32_t *)h = seq;
		*(int16_t *)(h + 4) = p->counter;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t SpriteMove(SpriteNode *p, int16_t frames)
	{
		p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
		p->pos[1] = (int16_t)(p->pos[1] + p->vel[1]);
		p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
		p->counter++;
		return p->counter < frames ? 0 : TASK_END;
	}

	static void PuffDraw(const SpriteNode *p, int32_t w)
	{
		uint8_t *h = AllocHeader(0xB4);
		GteSetIR0(w);
		GteLoadIR123((const void *)(TexBase() + 0x1648));
		GteGPF();
		GteSetIR0(0x1000 - w);
		GteLoadIR123(p->pos);
		GteGPL();
		int16_t pos[4];
		pos[3] = 0;
		GteStoreIR123(pos);
		TransformCameraByShadowRotation(pos, p->scale, -((int32_t)p->scale >> 2));
		*(uint32_t *)h = game_addr(SEQ_Puff);
		*(int16_t *)(h + 4) = p->counter;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static int32_t PuffWeight(int16_t counter) { return Div12(shl32(counter, 12)); }

	static int32_t DebrisScale(int16_t counter) { return Div12(shl32(0x16 - counter, 12)) + 0x1000; }

	static void DebrisDraw(const DebrisNode *d, bool scaled, int32_t s)
	{
		uint8_t *h = AllocHeader(0x58);
		Mat4x3 m;
		BuildRotationMatrixFromAngles(d->ang, &m);
		if (scaled)
		{
			const int32_t v[3] = { s, s, s };
			Scale3DMatrix(&m, v);
		}
		m.t[0] = d->pos[0];
		m.t[1] = d->pos[1];
		m.t[2] = d->pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)(h + 0x1C) = 0;
		*(uint32_t *)h = game_addr(MODEL_Debris);
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t DebrisMove(DebrisNode *d)
	{
		d->vel[1] = (int16_t)(d->vel[1] + 0x14);
		d->pos[1] = (int16_t)(d->pos[1] + d->vel[1]);
		d->pos[0] = (int16_t)(d->pos[0] + d->vel[0]);
		d->pos[2] = (int16_t)(d->pos[2] + d->vel[2]);
		if (d->pos[1] >= 0 && d->vel[1] > 0)
		{
			d->vel[1] = (int16_t)(-0x14 - d->vel[1]);
			d->spin[0] = (int16_t)-d->spin[0];
			d->spin[1] = (int16_t)-d->spin[1];
			d->spin[2] = (int16_t)-d->spin[2];
		}
		d->ang[0] = (int16_t)(d->ang[0] + d->spin[0]);
		d->ang[1] = (int16_t)(d->ang[1] + d->spin[1]);
		d->ang[2] = (int16_t)(d->ang[2] + d->spin[2]);
		d->counter++;
		return d->counter < 0x1E ? 0 : TASK_END;
	}

	static void RingDraw(const RingNode *g, const TargetNode *tn)
	{
		uint8_t *h = AllocHeader(0x58);
		const int16_t ang = (int16_t)(tn->b + g->e);
		const int32_t rad = (int16_t)(ComputeSin((int32_t)g->e >> 1) >> 3);
		int16_t v[4];
		v[0] = (int16_t)(mul32(ComputeCos(ang), rad) >> 12);
		v[1] = (int16_t)((int32_t)(0u - (uint32_t)mul32(ComputeSin(ang), rad)) >> 12);
		v[2] = (int16_t)(mul32(tn->dist, g->e) >> 12);
		v[3] = 0;
		GteSetRotMatrix(&tn->basis);
		Mat4x3 tm;
		tm.t[0] = tn->mouth[0];
		tm.t[1] = tn->mouth[1];
		tm.t[2] = tn->mouth[2];
		GteSetTransVector(&tm);
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		Mat4x3 m;
		GteReadMAC123(m.t);
		const int16_t s = (int16_t)(g->e + 0x400);
		m.m[0][0] = s; m.m[0][1] = 0; m.m[0][2] = 0;
		m.m[1][0] = 0; m.m[1][1] = s; m.m[1][2] = 0;
		m.m[2][0] = 0; m.m[2][1] = 0; m.m[2][2] = 0x1000;
		MatrixMultiply(&tn->basis, &m);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)(h + 0x1C) = 0x30;
		*(uint32_t *)h = game_addr(MODEL_Ring);
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}
}
}

namespace ff8fx
{
namespace storm011
{
	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SpriteNode *p = (SpriteNode *)n;
		SpriteDraw(p, game_addr(SEQ_Spark));
		return SpriteMove(p, 0xD);
	}

	static uint32_t __cdecl MistTask(TaskNode *n)
	{
		SpriteNode *p = (SpriteNode *)n;
		SpriteDraw(p, game_addr(SEQ_Mist));
		return SpriteMove(p, 0xC);
	}

	static uint32_t __cdecl HitTask(TaskNode *n)
	{
		SpriteNode *p = (SpriteNode *)n;
		SpriteDraw(p, game_addr(SEQ_Hit));
		return SpriteMove(p, 8);
	}

	static uint32_t __cdecl PuffTask(TaskNode *n)
	{
		SpriteNode *p = (SpriteNode *)n;
		PuffDraw(p, PuffWeight(p->counter));
		p->counter++;
		return p->counter < 0xC ? 0 : TASK_END;
	}

	static uint32_t __cdecl DebrisTask(TaskNode *n)
	{
		DebrisNode *d = (DebrisNode *)n;
		const bool scaled = d->counter >= 0x16;
		DebrisDraw(d, scaled, scaled ? DebrisScale(d->counter) : 0);
		return DebrisMove(d);
	}

	static uint32_t __cdecl RingTask(TaskNode *n)
	{
		RingNode *g = (RingNode *)n;
		RingDraw(g, g->tn);
		g->e = (int16_t)(g->e + 0x80);
		return g->e < 0x1000 ? 0 : TASK_END;
	}

	static void SpawnMist(TargetNode *t, int16_t vz)
	{
		Mat4x3 m;
		YawMatrix(*(const int16_t *)(Caster() + 0xE), &m);
		m.t[0] = t->mouth[0];
		m.t[1] = t->mouth[1];
		m.t[2] = t->mouth[2];
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		int32_t angle = CrtRand();
		for (int k = 0; k < 2; k++)
		{
			SpriteNode *p = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_MistTask));
			if (!p) continue;
			Memset32(&p->counter, 0, 5);
			int16_t v[4];
			v[0] = (int16_t)(ComputeCos(angle) >> 2);
			v[1] = (int16_t)(ComputeSin(angle) >> 2);
			v[2] = vz;
			v[3] = 0;
			GteLoadV0(v);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(p->vel);
			v[0] = (int16_t)(v[0] >> 3);
			v[1] = (int16_t)(v[1] >> 3);
			v[2] = 0;
			GteLoadV0(v);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(p->pos);
			p->scale = (int16_t)((CrtRand() & 0x7FF) + 0x800);
			p->vel[0] = (int16_t)(((int32_t)p->pos[0] - p->vel[0]) >> 4);
			p->vel[1] = (int16_t)(((int32_t)p->pos[1] - p->vel[1]) >> 4);
			p->vel[2] = (int16_t)(((int32_t)p->pos[2] - p->vel[2]) >> 4);
			angle += 0x800;
		}
	}

	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		GetEffectSpawnPosition(Caster(), 0x1D, 0x400, t->mouth);
		if (t->counter == 8) BdPlaySE3D(SOUND_Breath(), 0, t->mouth);
		const uint32_t phase = (uint32_t)((int32_t)t->counter - 0x19);
		Mat4x3 m1;
		int32_t d[3];
		if (phase < 0x32)
		{
			uint8_t *h = AllocHeader(0x5C);
			StreamWobble(t->a, t->b, (uint8_t *)game_addr(STREAM_VERTS));
			int16_t p0[4], p1[4];
			GetEffectSpawnPosition(t->ent[t->cur], 0xF1, 0, p0);
			GetEffectSpawnPosition(t->ent[t->next], 0xF1, 0, p1);
			int16_t aim[3];
			StreamAim(p0, p1, t->t, aim);
			if (phase >= 5) SweepStep(t);
			StreamDraw(h, phase, nullptr, aim, t->mouth, &t->dist, &t->basis, &m1, d, game_addr(STREAM_VERTS));
		}
		if ((uint32_t)(int32_t)t->counter < 0x18)
		{
			if (t->counter == 0) DecodeModelPrimLayout(game_addr(MODEL_Mouth), t->layout, 0x4A0);
			const int16_t angles[3] = { (int16_t)0xFF80, (int16_t)(*(const int16_t *)(Caster() + 0xE) + 0x800), 0 };
			Mat4x3 m2, m3;
			BuildRotationMatrixFromAngles(angles, &m2);
			m2.t[0] = t->mouth[0];
			m2.t[1] = t->mouth[1];
			m2.t[2] = t->mouth[2];
			ComposeAffineTransform(&Camera(), &m2, &m3);
			PrimArg arg;
			arg.m = &m3;
			arg.flags = game_addr(FLAGS_Mouth);
			arg.morph = TexBase() + 0x24B8;
			prim::play((prim::Layout *)t->layout, PartCallback, (int)&arg, 0);
			CrtRand();
			int32_t angle = CrtRand();
			GteSetRotMatrix(&m2);
			GteSetTransVector(&m2);
			for (int k = 0; k < 2; k++)
			{
				SpriteNode *p = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_PuffTask));
				if (!p) continue;
				Memset32(&p->counter, 0, 5);
				int16_t v[4];
				v[0] = (int16_t)(ComputeCos(angle) >> 2);
				v[1] = (int16_t)(ComputeSin(angle) >> 2);
				v[2] = (int16_t)((CrtRand() & 0x3FF) + 0x400);
				v[3] = 0x0136;
				GteLoadV0(v);
				GteMVMVA_RotV0Tr();
				GteStoreIR123(p->pos);
				p->scale = (int16_t)((CrtRand() & 0x3FF) + 0x800);
				angle += 0x800;
			}
		}
		if (t->counter >= 0x1D)
		{
			Mat4x3 m2 = t->basis;
			if (t->counter == 0x1D)
			{
				ff8fx::copy<(8)>(t->frozen, t->mouth);
				DecodeModelPrimLayout(game_addr(MODEL_Cone), t->layout, 0x140);
			}
			m2.t[0] = t->frozen[0];
			m2.t[1] = t->frozen[1];
			m2.t[2] = t->frozen[2];
			ComposeAffineTransform(&Camera(), &m2, &m2);
			PrimArg arg;
			arg.m = &m2;
			arg.flags = game_addr(FLAGS_Cone);
			arg.morph = TexBase() + 0x24B8;
			prim::play((prim::Layout *)t->layout, PartCallback, (int)&arg, 0);
		}
		if ((uint32_t)((int32_t)t->counter - 0x23) < 0x1E)
		{
			int32_t v = CrtRand();
			v = mul32(v, t->vc) >> 16;
			for (int k = 0; k < 2; k++)
			{
				SpriteNode *p = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_HitTask));
				if (!p) continue;
				Memset32(&p->counter, 0, 5);
				VertexPosition(t->ent[t->cur], v, p->pos);
				p->scale = (int16_t)((CrtRand() & 0x7FF) + 0xC00);
				p->vel[1] = 0;
				p->vel[0] = (int16_t)(shl32(d[0], 7) / t->dist);
				p->vel[2] = (int16_t)(shl32(d[2], 7) / t->dist);
				v += (int32_t)t->vc >> 1;
			}
		}
		const uint32_t sweep = (uint32_t)((int32_t)t->counter - 0x1E);
		if (sweep < 0x28)
		{
			GteSetRotMatrix(&m1);
			GteSetTransVector(&m1);
			const uint8_t *vtx = (const uint8_t *)game_addr(STREAM_VERTS);
			for (int i = 0; i < STREAM_NVERTS; i++, vtx += 8)
			{
				int16_t w[3];
				GteLoadV0(vtx);
				GteMVMVA_RotV0Tr();
				GteStoreIR123(w);
				if (w[1] < 0) continue;
				if (CrtRand() < 0x800)
				{
					SpriteNode *p = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_SparkTask));
					if (p)
					{
						Memset32(&p->counter, 0, 5);
						p->pos[0] = w[0];
						p->pos[1] = w[1];
						p->pos[2] = w[2];
						p->scale = (int16_t)((CrtRand() & 0x7FF) + 0x800);
						p->vel[1] = -0x32;
						p->vel[0] = (int16_t)(shl32(d[0], 7) / t->dist);
						p->vel[2] = (int16_t)(shl32(d[2], 7) / t->dist);
					}
				}
				if (sweep >= 0x1E) continue;
				if (CrtRand() >= 0x800) continue;
				DebrisNode *e = (DebrisNode *)AddTaskToQueue(QDebris(), game_addr(ORIG_DebrisTask));
				if (!e) continue;
				Memset32(&e->counter, 0, 9);
				e->pos[0] = w[0];
				e->pos[1] = w[1];
				e->pos[2] = w[2];
				e->scale = (int16_t)((CrtRand() & 0x7FF) + 0x800);
				e->vel[0] = (int16_t)(shl32(d[0], 6) / t->dist);
				e->vel[1] = (int16_t)(-0x80 - (CrtRand() & 0x7F));
				e->vel[2] = (int16_t)(shl32(d[2], 6) / t->dist);
				e->spin[0] = (int16_t)((CrtRand() & 0x3FF) - 0x200);
				e->spin[1] = (int16_t)((CrtRand() & 0x3FF) - 0x200);
				e->spin[2] = (int16_t)((CrtRand() & 0x3FF) - 0x200);
			}
		}
		if ((uint32_t)((int32_t)t->counter - 0x14) < 5) SpawnMist(t, 0x100);
		if ((uint32_t)((int32_t)t->counter - 0x19) < 0x28) SpawnMist(t, 0x400);
		const uint32_t ring = (uint32_t)((int32_t)t->counter - 0x19);
		if (ring < 0x11 && (ring & 3) == 0)
		{
			RingNode *g = (RingNode *)AddTaskToQueue(QRings(), game_addr(ORIG_RingTask));
			if (g)
			{
				Memset32(&g->pad0C, 0, 2);
				g->tn = t;
			}
		}
		const int16_t c = t->counter;
		t->a = (int16_t)(t->a - 100);
		t->b = (int16_t)(t->b + 100);
		if (c >= 0x4B) return TASK_END;
		t->counter = (int16_t)(c + 1);
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0x26B8;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x126B8;
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				int32_t lo = *(const int16_t *)(game_addr(STREAM_VERTS) + 4), hi = lo;
				for (int i = 1; i < STREAM_NVERTS; i++)
				{
					const int32_t z = *(const int16_t *)(game_addr(STREAM_VERTS) + i * 8 + 4);
					if (lo > z) lo = z;
					else if (hi < z) hi = z;
				}
				const int32_t range = hi - lo;
				uint8_t *dst = (uint8_t *)(TexBase() + 0x1B18);
				for (int i = 0; i < STREAM_NVERTS; i++, dst += 8)
				{
					ff8fx::copy<(8)>(dst, (const void *)(game_addr(STREAM_VERTS) + i * 8));
					*(int16_t *)(dst + 6) = (int16_t)(shl32(*(const int16_t *)(dst + 4), 12) / range);
				}
				InitTaskQueuePool(QTargets(), (void *)(TexBase() + 0x1614), 0x504, 1);
				InitTaskQueuePool(QSprites(), (void *)(TexBase() + 0x614), 0x20, 0x80);
				InitTaskQueuePool(QRings(), (void *)(TexBase() + 0x4D4), 0x14, 0x10);
				InitTaskQueuePool(QDebris(), (void *)(TexBase() + 0x1D4), 0x30, 0x10);
			}
			TargetNode *t = (TargetNode *)AddTaskToQueue(QTargets(), game_addr(ORIG_TargetTask));
			if (t)
			{
				Memset32(&t->counter, 0, 0x13E);
				t->count = (int16_t)Ctx()->actions[0].target_count;
				for (int i = 0; i < t->count; i++)
				{
					t->order[i] = (uint8_t)i;
					t->ent[i] = Entity(Ctx()->actions[0].targets[i * TARGET_STRIDE]);
				}
				for (int i = 0; i < t->count - 1; i++)
				{
					for (int j = i + 1; j < t->count; j++)
					{
						uint8_t *ei = t->ent[i], *ej = t->ent[j];
						if (*(const int16_t *)(ei + 0x1C) >= *(const int16_t *)(ej + 0x1C)) continue;
						t->ent[i] = ej;
						t->ent[j] = ei;
						const uint8_t o = t->order[i];
						t->order[i] = t->order[j];
						t->order[j] = o;
					}
				}
				t->next = t->count > 1 ? 1 : 0;
				const int32_t x = shl32(t->count, 12);
				int32_t q = (int32_t)(((int64_t)x * 0x66666667) >> 32) >> 4;
				q += (int32_t)((uint32_t)q >> 31);
				t->dt = (int16_t)q;
				t->vc = (int16_t)CountVertices(t->ent[0]);
			}
		}
		int a, b, c, d;
		if (r->started)
		{
			a = ExecuteTaskQueue(QTargets());
			b = ExecuteTaskQueue(QSprites());
			c = ExecuteTaskQueue(QRings());
			d = ExecuteTaskQueue(QDebris());
		}
		else
		{
			a = b = c = d = (int)n;
		}
		if (r->started && a == 0 && b == 0 && c == 0 && d == 0) return TASK_END;
		r->counter++;
		if (r->counter >= 10) r->counter = 10;
		return 0;
	}
}

	void register_mag011_storm_breath()
	{
		register_port(storm011::ORIG_RootTask, (void *)storm011::RootTask, "S011 RootTask", 11);
		register_port(storm011::ORIG_TargetTask, (void *)storm011::TargetTask, "S011 TargetTask", 11);
		register_port(storm011::ORIG_DebrisTask, (void *)storm011::DebrisTask, "S011 DebrisTask", 11);
		register_port(storm011::ORIG_RingTask, (void *)storm011::RingTask, "S011 RingTask", 11);
		register_port(storm011::ORIG_SparkTask, (void *)storm011::SparkTask, "S011 SparkTask", 11);
		register_port(storm011::ORIG_PuffTask, (void *)storm011::PuffTask, "S011 PuffTask", 11);
		register_port(storm011::ORIG_MistTask, (void *)storm011::MistTask, "S011 MistTask", 11);
		register_port(storm011::ORIG_HitTask, (void *)storm011::HitTask, "S011 HitTask", 11);
	}
}

