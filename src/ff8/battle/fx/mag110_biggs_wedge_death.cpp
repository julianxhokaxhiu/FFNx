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
namespace bw110
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_110_g_TexBase)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_110_g_CastContext)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_110_g_PacketCursor)); }
	inline TaskQueue *QActors() { return (TaskQueue *)game_addr(game::MAG_110_g_ActorQueue); }
	inline TaskQueue *QDebris() { return (TaskQueue *)game_addr(game::MAG_110_g_DebrisQueue); }
	inline TaskQueue *QRings() { return (TaskQueue *)game_addr(game::MAG_110_g_RingQueue); }
	inline TaskQueue *QSprites() { return (TaskQueue *)game_addr(game::MAG_110_g_SpriteQueue); }
	inline TaskQueue *QStream() { return (TaskQueue *)game_addr(game::MAG_110_g_StreamQueue); }
	inline const Mat4x3 *WobbleTrans() { return (const Mat4x3 *)game_addr(game::unk_2521A44); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_110_BiggsWedgeDeath_RootTask;
	static constexpr game::Id ORIG_StreamTask = game::MAG_110_BiggsWedgeDeath_StreamTask;
	static constexpr game::Id ORIG_ActorTask = game::MAG_110_BiggsWedgeDeath_ActorTask;
	static constexpr game::Id ORIG_HitTask = game::MAG_110_BiggsWedgeDeath_HitTask;
	static constexpr game::Id ORIG_DebrisTask = game::MAG_110_BiggsWedgeDeath_DebrisTask;
	static constexpr game::Id ORIG_RingTask = game::MAG_110_BiggsWedgeDeath_RingTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_110_BiggsWedgeDeath_SparkTask;
	static constexpr game::Id MODEL_Stream = game::MAG_110_StreamModel;
	static constexpr game::Id STREAM_VERTS = game::unk_12E0658;
	static const int STREAM_NVERTS = 0x134;
	static constexpr game::Id MODEL_Debris = game::MAG_110_DebrisModel;
	static constexpr game::Id MODEL_Ring = game::MAG_110_RingModel;
	static constexpr game::Id SEQ_Spark = game::MAG_110_SparkSequence;
	static constexpr game::Id SEQ_Hit = game::MAG_110_HitSequence;
	static const void *SOUND_Wind() { return (const void *)game_addr(game::MAG_110_SoundWind); }
	static const int16_t *SOUND_Pos() { return (const int16_t *)game_addr(game::MAG_110_g_SoundPosition); }
	static const int16_t MOUTH_YZ = (int16_t)0xF060;

	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void BuildOrthonormalBasis(const int32_t *v, Mat4x3 *out) { fn<void (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void QueueChainTransformation(uint8_t *entity, int32_t chain) { fn<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, chain); }
	inline void FillDwords(void *dst, uint32_t value, uint32_t count) { fn<void (__cdecl *)(void *, uint32_t, uint32_t)>(game_addr(game::Effect_FillDwords))(dst, value, count); }
	inline void PrintMonsterText(int32_t slot, int32_t text) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::printMonsterRelatedText0))(slot, text); }
	inline int32_t CountMaskedVertices(uint32_t model, const uint32_t *mask) { return fn<int32_t (__cdecl *)(uint32_t, const uint32_t *)>(game_addr(game::Effect_CountMaskedModelVertices))(model, mask); }
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
	struct StreamNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t count;
		uint8_t *ent[3];
		int16_t cur;
		int16_t next;
		int16_t t;
		int16_t dt;
		int16_t a;
		int16_t b;
		int32_t dist;
		int32_t dx;
		int32_t dz;
		int16_t mouth[4];
		int16_t kept[4];
		Mat4x3 basis;
	};
	struct ActorNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t sparks;
		StreamNode *sn;
		int16_t flag_delay;
		int16_t vc;
		int16_t pad18[2];
		uint8_t *ent;
		int16_t idx;
		int16_t delay;
		int16_t vel[3];
		int16_t pad2A;
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
		StreamNode *sn;
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
	struct Ramp
	{
		int kind;
		int32_t s;
		int32_t h8;
		int32_t scroll;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(StreamNode) == 0x64 && sizeof(ActorNode) == 0x2C, "Biggs / Wedge death nodes");
	static_assert(sizeof(SpriteNode) == 0x20 && sizeof(RingNode) == 0x14 && sizeof(DebrisNode) == 0x30, "Biggs / Wedge death nodes");

	static int32_t Div12(int32_t x)
	{
		int32_t hi = (int32_t)(((int64_t)x * 0x2AAAAAAB) >> 32);
		hi >>= 1;
		return hi + (int32_t)((uint32_t)hi >> 31);
	}

	static int16_t SweepStep(int16_t count)
	{
		const int32_t x = shl32(count, 12);
		int32_t q = (int32_t)(((int64_t)x * (int32_t)0x88888889) >> 32) + x;
		q >>= 5;
		q += (int32_t)((uint32_t)q >> 31);
		return (int16_t)q;
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

	static int32_t FindMaskedVertex(uint32_t model, uint8_t *state)
	{
		const uint8_t *bones = *(uint8_t *const *)model + 0x10;
		const uint8_t *objs = *(uint8_t *const *)(model + 4);
		const int32_t nobj = *(const int32_t *)objs;
		const int32_t *offs = (const int32_t *)(objs + 4);
		for (int32_t o = 0; o < nobj; o++)
		{
			const uint8_t *p = *(uint8_t *const *)(model + 4) + offs[o];
			if (!(*(const uint32_t *)state & (1u << (o & 31)))) continue;
			const int32_t ngroups = *(const int16_t *)p;
			p += 2;
			for (int32_t g = 0; g < ngroups; g++)
			{
				const int32_t bone = *(const int16_t *)p;
				p += 4;
				const uint8_t *bm = bones + bone * 0x30 + 0x10;
				const int32_t nv = *(const int16_t *)(p - 2);
				int32_t n = *(const int32_t *)(state + 4) - nv;
				*(int32_t *)(state + 4) = n;
				if (n <= 0)
				{
					n = -n;
					*(int32_t *)(state + 4) = n;
					const uint8_t *v = p + n * 6;
					GteSetRotMatrix((const Mat4x3 *)bm);
					GteSetTransVector((const Mat4x3 *)bm);
					*(int16_t *)(state + 0x10) = *(const int16_t *)v;
					*(int16_t *)(state + 0x12) = *(const int16_t *)(v + 2);
					*(int16_t *)(state + 0x14) = *(const int16_t *)(v + 4);
					GteLoadV0(state + 0x10);
					GteMVMVA_RotV0Tr();
					GteStoreIR123(state + 8);
					return 1;
				}
				p += nv * 6;
			}
		}
		return 0;
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

	static void StreamWobble(int16_t a, int16_t b, uint8_t *out)
	{
		Mat4x3 rz;
		ZRotation(a, &rz);
		GteSetRotMatrix(&rz);
		GteSetTransVector(WobbleTrans());
		const uint8_t *src = (const uint8_t *)(TexBase() + 0x14FC);
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

	static void StreamAimKept(const int16_t *p, const int16_t *kept, int16_t *aim)
	{
		for (int k = 0; k < 3; k++) aim[k] = (int16_t)(((int32_t)p[k] + kept[k]) >> 1);
	}

	static Ramp StreamRamp(uint32_t c)
	{
		Ramp r;
		r.scroll = (int32_t)((c & 7) << 4);
		r.s = 0x1000;
		if (c < 8)
		{
			r.kind = 0;
			r.s = ComputeSin((int32_t)((c & 0x3FFFFF) << 7));
			r.h8 = 0x1000 - r.s;
		}
		else if (c > 0x3E)
		{
			r.kind = 1;
			r.h8 = ComputeSin((int32_t)(((c << 7) - 0x1E81) & 0x1FFFFF80));
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

	static void StreamDraw(uint8_t *h, StreamNode *s, uint32_t c, const Ramp *given, const int16_t *aim, Mat4x3 *m1, uint32_t verts)
	{
		s->mouth[0] = aim[0];
		const int32_t dy = (int32_t)aim[1] - s->mouth[1];
		const int32_t dz = (int32_t)aim[2] - s->mouth[2];
		s->dx = 0;
		s->dz = dz;
		s->dist = Sqrt(mul32(dz, dz) + mul32(dy, dy) + 0);
		int16_t zs = (int16_t)s->dist;
		const Ramp r = given ? *given : StreamRamp(c);
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
		const int32_t v[3] = { 0, (int32_t)(0u - (uint32_t)dy), dz };
		Mat4x3 sc;
		sc.m[0][0] = 0x1000; sc.m[0][1] = 0; sc.m[0][2] = 0;
		sc.m[1][0] = 0; sc.m[1][1] = 0x1000; sc.m[1][2] = 0;
		sc.m[2][0] = 0; sc.m[2][1] = 0; sc.m[2][2] = zs;
		BuildOrthonormalBasis(v, &s->basis);
		*m1 = s->basis;
		MatrixMultiply3(m1, &sc);
		m1->t[0] = s->mouth[0];
		m1->t[1] = s->mouth[1];
		m1->t[2] = s->mouth[2];
		Mat4x3 m3;
		ComposeAffineTransform(&Camera(), m1, &m3);
		GteSetRotMatrix(&m3);
		GteSetTransVector(&m3);
		*(int16_t *)(h + 0x16) = 0x100;
		*(int16_t *)(h + 0x14) = 0x100;
		*(int16_t *)(h + 0x1E) = 0x100;
		*(int16_t *)(h + 0x12) = 0;
		*(int16_t *)(h + 0x10) = 0;
		*(int16_t *)(h + 0x1A) = 0;
		*(int16_t *)(h + 0x18) = 0;
		*(int16_t *)(h + 0x1C) = 0x80;
		*(int16_t *)(h + 0xC) = (int16_t)r.scroll;
		*(uint32_t *)h = game_addr(MODEL_Stream);
		PacketCursor() = DrawStream(h, RenderOT(0x44), 2, PacketCursor(), verts);
		FieldFree(0x5C);
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

	static void RingDraw(const RingNode *g, const StreamNode *sn)
	{
		uint8_t *h = AllocHeader(0x58);
		const int16_t ang = (int16_t)(sn->b + g->e);
		const int32_t rad = (int16_t)(ComputeSin((int32_t)g->e >> 1) >> 3);
		int16_t v[4];
		v[0] = (int16_t)(mul32(ComputeCos(ang), rad) >> 12);
		v[1] = (int16_t)((int32_t)(0u - (uint32_t)mul32(ComputeSin(ang), rad)) >> 12);
		v[2] = (int16_t)(mul32(sn->dist, g->e) >> 12);
		v[3] = 0;
		GteSetRotMatrix(&sn->basis);
		Mat4x3 tm;
		tm.t[0] = sn->mouth[0];
		tm.t[1] = sn->mouth[1];
		tm.t[2] = sn->mouth[2];
		GteSetTransVector(&tm);
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		Mat4x3 m;
		GteReadMAC123(m.t);
		const int16_t s = (int16_t)(g->e + 0x400);
		m.m[0][0] = s; m.m[0][1] = 0; m.m[0][2] = 0;
		m.m[1][0] = 0; m.m[1][1] = s; m.m[1][2] = 0;
		m.m[2][0] = 0; m.m[2][1] = 0; m.m[2][2] = 0x1000;
		MatrixMultiply(&sn->basis, &m);
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
namespace bw110
{
	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SpriteNode *p = (SpriteNode *)n;
		SpriteDraw(p, game_addr(SEQ_Spark));
		return SpriteMove(p, 0xD);
	}

	static uint32_t __cdecl HitTask(TaskNode *n)
	{
		SpriteNode *p = (SpriteNode *)n;
		SpriteDraw(p, game_addr(SEQ_Hit));
		return SpriteMove(p, 8);
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
		RingDraw(g, g->sn);
		g->e = (int16_t)(g->e + 0x80);
		return g->e < 0x1000 ? 0 : TASK_END;
	}

	static void StartActor(StreamNode *s, int32_t idx)
	{
		ActorNode *a = (ActorNode *)AddTaskToQueue(QActors(), game_addr(ORIG_ActorTask));
		if (!a) return;
		FillDwords(&a->counter, 0, 8);
		a->sn = s;
		uint8_t *e = s->ent[idx];
		a->ent = e;
		a->vc = (int16_t)CountVertices(e);
		a->idx = (int16_t)idx;
		a->vel[2] = 8;
		if (idx == 0)
		{
			a->delay = 0xA;
			a->flag_delay = 0xF;
		}
		else if (idx == 1)
		{
			a->delay = 2;
			a->flag_delay = 0xF;
		}
	}

	static uint32_t __cdecl ActorTask(TaskNode *n)
	{
		ActorNode *a = (ActorNode *)n;
		if (a->delay != 0)
		{
			a->delay--;
			return 0;
		}
		if (a->counter == 0)
		{
			if (a->idx == 0 || a->idx == 1)
			{
				QueueChainTransformation(a->ent, 5);
				a->sparks = 0xA;
			}
			const int32_t off = (int32_t)((uint32_t)a->ent - game_addr(game::BattleEntitySlotData));
			int32_t slot = (int32_t)(((int64_t)off * (int32_t)0xD20D20D3) >> 32) + off;
			slot >>= 7;
			slot += (int32_t)((uint32_t)slot >> 31);
			if (slot == 3) PrintMonsterText(3, 0xA);
			else if (slot == 4) PrintMonsterText(4, 1);
		}
		if (a->flag_delay == 0) a->ent[0] |= 8;
		else a->flag_delay--;
		if (a->sparks == 0)
		{
			*(int16_t *)(a->ent + 0xC) = (int16_t)(*(const int16_t *)(a->ent + 0xC) + 0xA0);
		}
		else
		{
			int32_t v = mul32(CrtRand(), a->vc) >> 16;
			for (int k = 0; k < 2; k++)
			{
				SpriteNode *p = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_HitTask));
				if (!p) continue;
				FillDwords(&p->counter, 0, 5);
				VertexPosition(a->ent, v, p->pos);
				p->scale = (int16_t)((CrtRand() & 0x7FF) + 0xC00);
				p->vel[0] = (int16_t)(shl32(a->sn->dx, 7) / a->sn->dist);
				p->vel[1] = 0;
				p->vel[2] = (int16_t)(shl32(a->sn->dz, 7) / a->sn->dist);
				v += (int32_t)a->vc >> 1;
			}
			a->sparks--;
			if (a->sparks == 0)
			{
				a->vel[2] = 0x100;
				a->vel[1] = (int16_t)0xFF38;
				if (a->idx == 0) a->vel[0] = 0x64;
				else if (a->idx == 1) a->vel[0] = (int16_t)0xFF9C;
			}
		}
		*(int16_t *)(a->ent + 0x1C) = (int16_t)(*(const int16_t *)(a->ent + 0x1C) + a->vel[0]);
		*(int16_t *)(a->ent + 0x1E) = (int16_t)(*(const int16_t *)(a->ent + 0x1E) + a->vel[1]);
		*(int16_t *)(a->ent + 0x20) = (int16_t)(*(const int16_t *)(a->ent + 0x20) + a->vel[2]);
		a->ent[1] |= 0x10;
		a->counter++;
		if (a->counter < 0x32) return 0;
		a->ent[0] |= 4;
		*(uint16_t *)a->ent &= 0xEFFF;
		return TASK_END;
	}

	static uint32_t __cdecl StreamTask(TaskNode *n)
	{
		StreamNode *s = (StreamNode *)n;
		s->mouth[1] = MOUTH_YZ;
		s->mouth[2] = MOUTH_YZ;
		const uint32_t c = (uint32_t)(int32_t)s->counter;
		Mat4x3 m1;
		if (c < 0x46)
		{
			uint8_t *h = AllocHeader(0x5C);
			if (c == 0) BdPlaySE3D(SOUND_Wind(), 0, SOUND_Pos());
			StreamWobble(s->a, s->b, (uint8_t *)game_addr(STREAM_VERTS));
			if (c >= 0xF)
			{
				s->t = (int16_t)(s->t + s->dt);
				if (s->t >= 0x1000)
				{
					s->t = (int16_t)(s->t + 0xF000);
					if (s->cur != s->next)
					{
						ff8fx::copy<(8)>(s->kept, s->ent[s->next] + 0x1C);
						StartActor(s, s->next);
					}
					const int16_t nx = s->next;
					s->cur = nx;
					s->next = (int16_t)(nx + 1);
					if (s->next >= s->count) s->next = (int16_t)(s->count - 1);
				}
			}
			int16_t aim[3];
			if (s->cur == 1) StreamAimKept((const int16_t *)(s->ent[s->next] + 0x1C), s->kept, aim);
			else StreamAim((const int16_t *)(s->ent[s->cur] + 0x1C), (const int16_t *)(s->ent[s->next] + 0x1C), s->t, aim);
			StreamDraw(h, s, c, nullptr, aim, &m1, game_addr(STREAM_VERTS));
		}
		if (c - 5 < 0x3C)
		{
			const uint32_t sweep = c - 5;
			GteSetRotMatrix(&m1);
			GteSetTransVector(&m1);
			const uint8_t *vtx = (const uint8_t *)game_addr(STREAM_VERTS);
			for (int i = 0; i < STREAM_NVERTS; i++, vtx += 8)
			{
				int16_t w[4];
				GteLoadV0(vtx);
				GteMVMVA_RotV0Tr();
				GteStoreIR123(w);
				if (w[1] < 0) continue;
				if (CrtRand() < 0x800)
				{
					SpriteNode *p = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_SparkTask));
					if (p)
					{
						FillDwords(&p->counter, 0, 5);
						p->pos[0] = w[0];
						p->pos[1] = w[1];
						p->pos[2] = w[2];
						p->scale = (int16_t)((CrtRand() & 0x7FF) + 0x800);
						p->vel[1] = -0x32;
						p->vel[0] = (int16_t)(shl32(0, 7) / s->dist);
						p->vel[2] = (int16_t)(shl32(s->dz, 7) / s->dist);
					}
				}
				if (sweep >= 0x32) continue;
				if (CrtRand() >= 0x800) continue;
				DebrisNode *e = (DebrisNode *)AddTaskToQueue(QDebris(), game_addr(ORIG_DebrisTask));
				if (!e) continue;
				FillDwords(&e->counter, 0, 9);
				e->pos[0] = w[0];
				e->pos[1] = w[1];
				e->pos[2] = w[2];
				e->scale = (int16_t)((CrtRand() & 0x7FF) + 0x800);
				e->vel[0] = (int16_t)(shl32(0, 6) / s->dist);
				e->vel[1] = (int16_t)(-0x80 - (CrtRand() & 0x7F));
				e->vel[2] = (int16_t)(shl32(s->dz, 6) / s->dist);
				e->spin[0] = (int16_t)((CrtRand() & 0x3FF) - 0x200);
				e->spin[1] = (int16_t)((CrtRand() & 0x3FF) - 0x200);
				e->spin[2] = (int16_t)((CrtRand() & 0x3FF) - 0x200);
			}
		}
		if (c < 0x25 && (c & 3) == 0)
		{
			RingNode *g = (RingNode *)AddTaskToQueue(QRings(), game_addr(ORIG_RingTask));
			if (g)
			{
				FillDwords(&g->pad0C, 0, 2);
				g->sn = s;
			}
		}
		const int16_t cc = s->counter;
		s->a = (int16_t)(s->a - 100);
		s->b = (int16_t)(s->b + 100);
		if (cc >= 0x46) return TASK_END;
		s->counter = (int16_t)(cc + 1);
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0x1E9C;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x11E9C;
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
				uint8_t *dst = (uint8_t *)(TexBase() + 0x14FC);
				for (int i = 0; i < STREAM_NVERTS; i++, dst += 8)
				{
					ff8fx::copy<(8)>(dst, (const void *)(game_addr(STREAM_VERTS) + i * 8));
					*(int16_t *)(dst + 6) = (int16_t)(shl32(*(const int16_t *)(dst + 4), 12) / range);
				}
				InitTaskQueuePool(QStream(), (void *)(TexBase() + 0x1498), 0x64, 1);
				InitTaskQueuePool(QSprites(), (void *)(TexBase() + 0x498), 0x20, 0x80);
				InitTaskQueuePool(QRings(), (void *)(TexBase() + 0x358), 0x14, 0x10);
				InitTaskQueuePool(QDebris(), (void *)(TexBase() + 0x58), 0x30, 0x10);
				InitTaskQueuePool(QActors(), (void *)TexBase(), 0x2C, 2);
			}
			StreamNode *s = (StreamNode *)AddTaskToQueue(QStream(), game_addr(ORIG_StreamTask));
			if (s)
			{
				FillDwords(&s->counter, 0, 0x16);
				s->next = 1;
				s->count = (int16_t)Ctx()->actions[0].target_count;
				for (int i = 0; i < s->count; i++) s->ent[i] = Entity(Ctx()->actions[0].targets[i * TARGET_STRIDE]);
				s->dt = SweepStep(s->count);
				StartActor(s, 0);
			}
		}
		int a, b, c, d, e;
		if (r->started)
		{
			a = ExecuteTaskQueue(QStream());
			b = ExecuteTaskQueue(QSprites());
			c = ExecuteTaskQueue(QRings());
			d = ExecuteTaskQueue(QDebris());
			e = ExecuteTaskQueue(QActors());
		}
		else
		{
			a = b = c = d = e = (int)n;
		}
		if (r->started && a == 0 && b == 0 && c == 0 && d == 0 && e == 0) return TASK_END;
		r->counter++;
		if (r->counter >= 10) r->counter = 10;
		return 0;
	}
}

	void register_mag110_biggs_wedge_death()
	{
		register_port(bw110::ORIG_RootTask, (void *)bw110::RootTask, "110 RootTask", 110);
		register_port(bw110::ORIG_StreamTask, (void *)bw110::StreamTask, "110 StreamTask", 110);
		register_port(bw110::ORIG_ActorTask, (void *)bw110::ActorTask, "110 ActorTask", 110);
		register_port(bw110::ORIG_HitTask, (void *)bw110::HitTask, "110 HitTask", 110);
		register_port(bw110::ORIG_DebrisTask, (void *)bw110::DebrisTask, "110 DebrisTask", 110);
		register_port(bw110::ORIG_RingTask, (void *)bw110::RingTask, "110 RingTask", 110);
		register_port(bw110::ORIG_SparkTask, (void *)bw110::SparkTask, "110 SparkTask", 110);
	}
}

