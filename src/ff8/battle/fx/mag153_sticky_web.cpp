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

// Effect 153: Sticky Web

#include "mag_common.h"

namespace ff8fx
{
namespace web153
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &TargetSlot() { return var<uint32_t>(game_addr(game::unk_243A53C)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_243BDE8)); }
	inline uint32_t &CasterSlot() { return var<uint32_t>(game_addr(game::unk_243BDEC)); }
	inline int16_t *TargetPos() { return (int16_t *)game_addr(game::g_Web153_TargetPos); }
	inline int16_t *CasterPos() { return (int16_t *)game_addr(game::g_Web153_CasterPos); }
	inline int16_t *Anchor() { return (int16_t *)game_addr(game::g_Web153_Anchor); }
	inline uint32_t &Cursor() { return var<uint32_t>(game_addr(game::g_Web153_PacketCursor)); }
	inline TaskQueue *QEffects() { return (TaskQueue *)game_addr(game::g_Web153_EffectQueue); }
	static constexpr game::Id PTS_A = game::g_Web153_SplinePointsA, PTS_B = game::g_Web153_SplinePointsB;
	static constexpr game::Id SPL_A = game::g_Web153_SplineSamplesA, SPL_B = game::g_Web153_SplineSamplesB;
	static constexpr game::Id ARENA_A = game::unk_2443E6C, ARENA_B = game::unk_243BE6C;

	static constexpr game::Id ORIG_RootTask = game::MAG_153_STICKY_WEB_RootTask;
	static constexpr game::Id ORIG_MasterTask = game::MAG_153_STICKY_WEB_MasterTask;
	static constexpr game::Id ORIG_ThreadTask = game::MAG_153_STICKY_WEB_ThreadTask;
	static constexpr game::Id ORIG_WebTask = game::MAG_153_STICKY_WEB_WebTask;
	static constexpr game::Id ORIG_Particles1Task = game::MAG_153_STICKY_WEB_Particles1Task;
	static constexpr game::Id ORIG_Particles2Task = game::MAG_153_STICKY_WEB_Particles2Task;
	static constexpr game::Id ORIG_BurstTask = game::MAG_153_STICKY_WEB_BurstTask;
	static constexpr game::Id MODEL_Web = game::unk_DBB33C;
	static constexpr game::Id MODEL_Burst = game::unk_DBDBF4;
	static constexpr game::Id SEQ_Particle = game::unk_DBB190;
	static constexpr game::Id CLIP_TABLE = game::unk_DBE284;
	static const void *SOUND_Web() { return (const void *)game_addr(game::unk_DBE27C); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void MatrixMulVector(const Mat4x3 *m, const int16_t *v, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, v, out); }
	inline void SplineSetup(int32_t n, uint32_t points, void *work) { fn<void (__cdecl *)(int32_t, uint32_t, void *)>(game_addr(game::Effect_SplineSetup))(n, points, work); }
	inline void SplineEval(int32_t n, void *work, uint32_t out, int32_t t) { fn<void (__cdecl *)(int32_t, void *, uint32_t, int32_t)>(game_addr(game::Effect_SplineEval))(n, work, out, t); }
	inline void GteSetFarColorB(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColorRGB))(r, g, b); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZWord(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *c) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(c); }
	inline void GteWriteControlReg(uint32_t value, int32_t reg) { fn<void (__cdecl *)(uint32_t, int32_t)>(game_addr(game::GTE_WriteControlReg))(value, reg); }
	inline void GteLightV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteLightV1() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV1_Bk))(); }
	inline void GteLightV2() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV2_Bk))(); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad;
	};
	struct Node
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t f10;
		int16_t f12;
		int16_t f14;
		int16_t f16;
		int16_t f18;
		int16_t pad1A;
		int16_t f1C;
		int16_t f1E;
		uint8_t pad20[4];
	};
	struct Particle
	{
		int32_t type;
		int16_t frame;
		int16_t angle;
		int16_t pos[4];
		int16_t vel[3];
		int16_t pad;
	};
	struct RingEntry
	{
		int32_t valid;
		uint32_t p0, p1;
		uint32_t q0, q1;
	};
	struct Slot
	{
		int16_t v[4];
		int32_t flag;
		int32_t d;
	};
	struct ClipHeader
	{
		uint8_t pad00[0x1C];
		uint8_t *model;
		uint8_t *verts;
		uint32_t flags;
		uint8_t *list;
		uint32_t mask;
		uint32_t code;
		uint32_t uv0;
		uint32_t uv1;
		uint32_t ot;
		int32_t shift;
		uint32_t cursor;
		uint8_t pad48[8];
		int32_t otz;
		int32_t flag;
		Slot slot[6];
		int16_t plane[4];
		uint8_t padC0[8];
		Slot *ptr[56];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node) == 0x24, "Sticky Web nodes");
	static_assert(sizeof(Particle) == 0x18 && sizeof(RingEntry) == 0x14, "Sticky Web records");
	static_assert(sizeof(ClipHeader) == 0x1A8, "Sticky Web clip header");

	inline Particle *Parts() { return (Particle *)game_addr(game::g_Web153_Particles); }
	inline RingEntry *Ring() { return (RingEntry *)game_addr(game::g_Web153_ThreadRing); }
	inline uint32_t rd32(const void *p) { return *(const uint32_t *)p; }
	inline Node *AddTask(uint32_t fn) { return (Node *)AddTaskToQueue(QEffects(), fn); }
}
}

namespace ff8fx
{
namespace web153
{
	static void PlaneDistance(const int16_t *a, int16_t *n)
	{
		uint32_t s = (uint32_t)((int32_t)a[2] * n[2]);
		s += (uint32_t)((int32_t)a[1] * n[1]);
		s += (uint32_t)((int32_t)a[0] * n[0]);
		n[3] = (int16_t)(-((int32_t)s >> 12));
	}

	static void Intersect(const int16_t *a, const int16_t *b, const int16_t *n, int16_t *out)
	{
		const int32_t dx = (int32_t)b[0] - a[0];
		const int32_t dy = (int32_t)b[1] - a[1];
		const int32_t dz = (int32_t)b[2] - a[2];
		int32_t den = (int32_t)((uint32_t)mul32(n[0], dx) + (uint32_t)mul32(n[1], dy) + (uint32_t)mul32(n[2], dz)) >> 12;
		if (den == 0) den = 1;
		const int32_t num = (int32_t)((uint32_t)shl32(n[3], 12) + (uint32_t)((int32_t)n[2] * a[2]) + (uint32_t)((int32_t)n[1] * a[1]) + (uint32_t)((int32_t)n[0] * a[0]));
		const int32_t t = -(num / den);
		out[0] = (int16_t)((mul32(t, dx) >> 12) + a[0]);
		out[1] = (int16_t)((mul32(t, dy) >> 12) + a[1]);
		out[2] = (int16_t)((mul32(t, dz) >> 12) + a[2]);
		const int32_t ua = (uint16_t)a[3], ub = (uint16_t)b[3];
		const int32_t lo = (mul32((ub & 0xFF) - (ua & 0xFF), t) >> 12) + (ua & 0xFF);
		const int32_t hi = (mul32((ub & 0xFF00) - (ua & 0xFF00), t) >> 12) + (ua & 0xFF00);
		out[3] = (int16_t)(uint16_t)((uint32_t)lo | ((uint32_t)hi & 0xFF00));
	}

	static void EmitTri(ClipHeader *h, const Slot *a, const Slot *b, const Slot *c)
	{
		uint32_t *pk = (uint32_t *)h->cursor;
		GteLoadV012(a, b, c);
		GteRTPT();
		pk[1] = h->code | 0x24000000;
		pk[3] = h->uv0;
		pk[5] = h->uv1;
		pk[0] = 0x07000000;
		GteReadSXY012(&pk[2], &pk[4], &pk[6]);
		GteAVSZ3();
		*(int16_t *)&pk[5] = b->v[3];
		*(int16_t *)&pk[3] = a->v[3];
		*(int16_t *)&pk[7] = c->v[3];
		GteReadOTZWord(&h->otz);
		InsertPrimAutoDepth(h->ot + (uint32_t)(h->otz >> (h->shift & 31)) * 4, pk);
		h->cursor = (uint32_t)pk + 0x20;
	}

	static void EmitQuad(ClipHeader *h, const Slot *a, const Slot *b, const Slot *c, const Slot *d)
	{
		uint32_t *pk = (uint32_t *)h->cursor;
		GteLoadV012(a, b, c);
		GteRTPT();
		pk[1] = h->code | 0x2C000000;
		pk[3] = h->uv0;
		pk[5] = h->uv1;
		pk[0] = 0x09000000;
		GteReadSXY012(&pk[2], &pk[4], &pk[6]);
		GteLoadV0(d);
		GteRTPS();
		*(int16_t *)&pk[5] = b->v[3];
		*(int16_t *)&pk[3] = a->v[3];
		*(int16_t *)&pk[7] = c->v[3];
		*(int16_t *)&pk[9] = d->v[3];
		GteReadSXY2(&pk[8]);
		GteAVSZ4();
		GteReadOTZWord(&h->otz);
		InsertPrimAutoDepth(h->ot + (uint32_t)(h->otz >> (h->shift & 31)) * 4, pk);
		h->cursor = (uint32_t)pk + 0x28;
	}

	static void ClipTri(ClipHeader *h)
	{
		if (h->slot[3].flag) Intersect(h->slot[0].v, h->slot[1].v, h->plane, h->slot[3].v);
		if (h->slot[4].flag) Intersect(h->slot[1].v, h->slot[2].v, h->plane, h->slot[4].v);
		if (h->slot[5].flag) Intersect(h->slot[2].v, h->slot[0].v, h->plane, h->slot[5].v);
		const uint32_t m = h->mask;
		Slot **p = &h->ptr[m * 8];
		switch (m)
		{
		case 1: case 2: case 4:
			if (p[3]->d > 0) EmitTri(h, p[0], p[1], p[2]);
			else EmitTri(h, p[4], p[5], p[6]);
			break;
		case 3: case 5: case 6:
			if (p[3]->d > 0) EmitTri(h, p[0], p[1], p[2]);
			else EmitQuad(h, p[4], p[5], p[6], p[7]);
			break;
		default:
			break;
		}
	}

	static void ClassifyAndDraw(ClipHeader *h)
	{
		GteLightV0();
		h->slot[3].flag = 0;
		GteReadDataReg(9, &h->slot[3].d);
		GteLightV1();
		h->slot[4].flag = 0;
		GteReadDataReg(9, &h->slot[4].d);
		GteLightV2();
		h->slot[5].flag = 0;
		GteReadDataReg(9, &h->slot[5].d);
		const int32_t d0 = h->slot[3].d, d1 = h->slot[4].d, d2 = h->slot[5].d;
		if (d0 != 0 && (d1 ^ d0) < 0) h->slot[3].flag = 1;
		if (d1 != 0 && (d1 ^ d2) < 0) h->slot[4].flag = 2;
		if (d2 != 0 && (d2 ^ d0) < 0) h->slot[5].flag = 4;
		h->mask = h->slot[5].flag | h->slot[3].flag | h->slot[4].flag;
		if (h->mask) ClipTri(h);
		else if ((d2 | d1 | d0) > 0) EmitTri(h, &h->slot[0], &h->slot[1], &h->slot[2]);
	}

	static void LoadVertex(ClipHeader *h, int slot, uint16_t index)
	{
		const uint32_t *v = (const uint32_t *)(h->verts + (uint32_t)index * 4);
		uint32_t *s = (uint32_t *)h->slot[slot].v;
		s[0] = v[0];
		s[1] = v[1];
	}

	static void TriList(ClipHeader *h)
	{
		const int32_t count0 = *(const int32_t *)h->list;
		uint8_t *rec = h->list + 4;
		h->list = rec;
		if (count0 <= 0) { h->list = rec; return; }
		int32_t count = count0;
		do
		{
			LoadVertex(h, 0, *(const uint16_t *)(rec + 4));
			LoadVertex(h, 1, *(const uint16_t *)(rec + 6));
			LoadVertex(h, 2, *(const uint16_t *)(rec + 8));
			GteLoadV012(&h->slot[0], &h->slot[1], &h->slot[2]);
			GteRTPT();
			h->code = rd32(rec) & 0x2FFFFFF;
			h->uv0 = rd32(rec + 0xC);
			h->uv1 = rd32(rec + 0x10);
			h->slot[0].v[3] = (int16_t)h->uv0;
			h->slot[1].v[3] = (int16_t)h->uv1;
			h->slot[2].v[3] = (int16_t)(rd32(rec + 8) >> 16);
			GteReadFLAG(&h->flag);
			if (!(h->flag & 0x60000)) ClassifyAndDraw(h);
			rec += 0x14;
		} while (--count);
		h->list = rec;
	}

	static void QuadList(ClipHeader *h)
	{
		const int32_t count0 = *(const int32_t *)h->list;
		uint8_t *rec = h->list + 4;
		h->list = rec;
		if (count0 <= 0) { h->list = rec; return; }
		int32_t count = count0;
		do
		{
			LoadVertex(h, 0, *(const uint16_t *)(rec + 4));
			LoadVertex(h, 1, *(const uint16_t *)(rec + 6));
			LoadVertex(h, 2, *(const uint16_t *)(rec + 8));
			GteLoadV012(&h->slot[0], &h->slot[1], &h->slot[2]);
			GteRTPT();
			h->code = rd32(rec) & 0x2FFFFFF;
			const uint32_t uv0 = rd32(rec + 0xC);
			h->slot[0].v[3] = (int16_t)uv0;
			const uint32_t uv23 = rd32(rec + 0x14);
			h->slot[2].v[3] = (int16_t)uv23;
			h->uv0 = uv0;
			const uint32_t uv1 = rd32(rec + 0x10);
			h->uv1 = uv1;
			h->slot[1].v[3] = (int16_t)uv1;
			GteReadFLAG(&h->flag);
			if (!(h->flag & 0x60000))
			{
				ClassifyAndDraw(h);
				LoadVertex(h, 0, *(const uint16_t *)(rec + 0xA));
				LoadVertex(h, 1, *(const uint16_t *)(rec + 6));
				LoadVertex(h, 2, *(const uint16_t *)(rec + 8));
				GteLoadV012(&h->slot[0], &h->slot[1], &h->slot[2]);
				GteRTPT();
				h->slot[2].v[3] = (int16_t)uv23;
				h->slot[1].v[3] = (int16_t)h->uv1;
				h->slot[0].v[3] = (int16_t)(uv23 >> 16);
				GteReadFLAG(&h->flag);
				ClassifyAndDraw(h);
			}
			rec += 0x18;
		} while (--count);
		h->list = rec;
	}

	static uint32_t ClipRender(ClipHeader *h, uint32_t ot, int32_t mode, uint32_t cursor)
	{
		if (!(h->flags & 0x2000)) h->verts = h->model + 8;
		h->list = h->model + rd32(h->model);
		const uint32_t w = rd32(&h->plane[2]);
		GteWriteControlReg(rd32(&h->plane[0]), 8);
		GteWriteControlReg(w, 9);
		GteWriteControlReg((uint32_t)((int32_t)w >> 16), 0xD);
		for (int k = 0; k < 56; k++) h->ptr[k] = (Slot *)((uint8_t *)h->slot + (var<uint32_t>(game_addr(CLIP_TABLE) + 4 * k) << 4));
		h->cursor = cursor;
		h->ot = ot;
		h->shift = mode;
		h->list += 8;
		if (*(const int32_t *)h->list != 0) TriList(h);
		else h->list += 4;
		if (*(const int32_t *)h->list != 0)
		{
			QuadList(h);
			return h->cursor;
		}
		h->list += 4;
		return h->cursor;
	}

	static void DrawLine(const int16_t *a, const int16_t *b, uint32_t colour)
	{
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		uint32_t *pk = (uint32_t *)Cursor();
		GteLoadV012(a, b, b);
		GteRTPT();
		pk[1] = colour;
		pk[3] = colour;
		pk[0] = 0x04000000;
		((uint8_t *)pk)[7] = 0x52;
		GteReadSXY012(&pk[2], &pk[4], &pk[4]);
		GteAVSZ3();
		int32_t otz;
		GteReadOTZWord(&otz);
		InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + (uint32_t)(otz >> 2) * 4 + 0x44, pk);
		Cursor() = (uint32_t)pk + 0x14;
	}

	static void DrawStrips(uint8_t *h, const uint8_t *a, const uint8_t *b)
	{
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteSetFarColorB(0, 0, 0);
		uint32_t *q = (uint32_t *)Cursor();
		*(uint32_t *)(h + 0x14) = 0x3A203040;
		*(uint32_t *)(h + 0x1C) = 0x3A203040;
		uint32_t *p2 = q + 9;
		q += 1;
		int32_t acc = 0;
		for (int32_t off = 0; off < 0x70; off += 8, acc += 0x1000)
		{
			*(uint32_t *)(h + 0x10) = *(uint32_t *)(h + 0x14);
			q[-1] = 0x08000000;
			GteLoadV012(a + off, a + off + 8, b + off);
			GteRTPT();
			GteReadFLAG(h + 0xC);
			if (*(uint32_t *)(h + 0xC) & 0x60000) continue;
			GteReadSXY012(&q[1], &q[3], &q[5]);
			GteLoadV0(b + off + 8);
			GteRTPS();
			GteReadSXY2(&q[7]);
			GteAVSZ4();
			GteReadOTZWord(h + 8);
			GteSetIR0(acc / 14);
			GteLoadRGBC(h + 0x1C);
			GteDPCS();
			GteStoreRGB2(h + 0x14);
			const uint32_t c0 = *(uint32_t *)(h + 0x10), c1 = *(uint32_t *)(h + 0x14);
			q[4] = c0;
			q[0] = c0;
			q[6] = c1;
			q[2] = c1;
			const uint32_t bucket = var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + (uint32_t)(*(int32_t *)(h + 8) >> 2) * 4 + 0x44;
			InsertPrimAutoDepth(bucket, q - 1);
			p2[5] = c0;
			p2[3] = 0x3A000000;
			p2[1] = 0x3A000000;
			p2[7] = c1;
			p2[4] = q[3];
			p2[2] = q[1];
			p2[6] = q[5];
			p2[8] = q[7];
			p2[0] = 0x08000000;
			InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + (uint32_t)(*(int32_t *)(h + 8) >> 2) * 4 + 0x44, p2);
			q += 0x12;
			p2 += 0x12;
		}
		Cursor() = (uint32_t)p2;
	}

	static void ModelMatrix(const Node *n)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = n->f18;
		ang[2] = 0;
		ang[3] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = n->f10;
		m.t[1] = n->f12;
		const int32_t s = n->f1C;
		const int32_t v[3] = { s, s, s };
		m.t[2] = n->f14;
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}

	static void WebDraw(const Node *n, int32_t c, int16_t anchor_y, int32_t fade)
	{
		ModelMatrix(n);
		if (c < 0x1E)
		{
			ClipHeader *h = (ClipHeader *)FieldAlloc(0x1A8);
			const int32_t y = n->f12;
			h->flags = 0;
			const int32_t s = n->f1C;
			h->slot[0].v[0] = 0;
			h->model = (uint8_t *)game_addr(MODEL_Web);
			const int32_t d = (int32_t)anchor_y - y;
			const int32_t inv = 0x1000000 / s;
			h->plane[1] = 0x1000;
			h->slot[0].v[2] = 0;
			h->plane[0] = 0;
			h->plane[2] = 0;
			h->slot[0].v[1] = (int16_t)(mul32(d, inv) >> 12);
			PlaneDistance(h->slot[0].v, h->plane);
			Cursor() = ClipRender(h, RenderOT(), 2, Cursor());
			FieldFree(0x1A8);
		}
		else
		{
			uint8_t *h = (uint8_t *)FieldAlloc(0x58);
			*(uint32_t *)h = game_addr(MODEL_Web);
			*(uint32_t *)(h + 8) = 0;
			*(uint32_t *)(h + 0x1C) = 0;
			if (c >= 0x25)
			{
				*(uint32_t *)(h + 0x1C) = 0xF3;
				*(int32_t *)(h + 0xC) = fade;
			}
			Cursor() = RenderPrimModel(h, RenderOT(), 2, Cursor());
			FieldFree(0x58);
		}
	}
	static int32_t WebFade(int32_t c) { return shl32(c - 0x25, 9); }

	static void BurstDraw(const Node *n, int32_t c, int32_t fade)
	{
		ModelMatrix(n);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = game_addr(MODEL_Burst);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (c >= 6)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		Cursor() = RenderPrimModel(h, RenderOT(), 2, Cursor());
		FieldFree(0x58);
	}
	static int32_t BurstFade(int32_t c) { return shl32(c - 6, 9); }

	static void ParticleDraw(uint8_t *h, int16_t frame, const int16_t *pos, int16_t angle)
	{
		*(int16_t *)(h + 4) = frame;
		TransformCameraByShadowRotation(pos, angle, -((int32_t)angle >> 4));
		Cursor() = InitEffectSequenceFromData(h, RenderOT(), 2, Cursor());
	}

	static uint32_t __cdecl Particles1Task(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t alive = 0;
		*(uint32_t *)h = game_addr(SEQ_Particle);
		*(int16_t *)(h + 0x24) = 8;
		for (Particle *e = Parts(); e < Parts() + 100; e++)
		{
			if (!(*(const uint8_t *)&e->type & 1)) continue;
			ParticleDraw(h, e->frame, e->pos, e->angle);
			if (!(UpdateFlags() & 0x201))
			{
				e->frame++;
				if (*(const int16_t *)(h + 0x28) < 0) e->type = 0;
				else
				{
					e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
					e->pos[1] = (int16_t)(e->pos[1] + e->vel[1]);
					e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
					alive++;
				}
			}
		}
		FieldFree(0xB4);
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter >= 0 && n->counter <= 0x16)
		{
			int32_t v[3];
			v[0] = (int32_t)TargetPos()[0] - CasterPos()[0];
			v[1] = 0;
			v[2] = (int32_t)TargetPos()[2] - CasterPos()[2];
			NormalizeVector(v, v);
			for (int k = 0; k < 1; k++)
			{
				int idx = 0;
				while (idx < 100 && Parts()[idx].type != 0) idx++;
				if (idx >= 100) break;
				Particle *e = &Parts()[idx];
				e->type = 1;
				e->frame = 0;
				e->angle = (int16_t)(CrtRand() % 0x500 + 0xB00);
				*(uint32_t *)&e->pos[0] = rd32(&CasterPos()[0]);
				*(uint32_t *)&e->pos[2] = rd32(&CasterPos()[2]);
				e->pos[0] = (int16_t)(e->pos[0] + (CrtRand() % 50 - 25));
				e->pos[1] = (int16_t)(e->pos[1] + (CrtRand() % 50 - 25));
				e->pos[2] = (int16_t)(e->pos[2] + (CrtRand() % 50 - 25));
				const int32_t r = CrtRand() % 5 + 5;
				e->vel[0] = (int16_t)(mul32(r, v[0]) >> 12);
				e->vel[1] = (int16_t)(mul32(r, v[1]) >> 12);
				e->vel[2] = (int16_t)(mul32(r, v[2]) >> 12);
				e->vel[1] = (int16_t)(e->vel[1] + (CrtRand() % 20 - 10));
			}
		}
		n->counter++;
		if (n->counter >= 4 && alive == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl Particles2Task(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t alive = 0;
		*(uint32_t *)h = game_addr(SEQ_Particle);
		*(int16_t *)(h + 0x24) = 8;
		for (Particle *e = Parts(); e < Parts() + 100; e++)
		{
			if (!(*(const uint8_t *)&e->type & 2)) continue;
			ParticleDraw(h, e->frame, e->pos, e->angle);
			if (!(UpdateFlags() & 0x201))
			{
				e->frame++;
				if (*(const int16_t *)(h + 0x28) < 0) e->type = 0;
				else
				{
					e->pos[1] = (int16_t)(e->pos[1] + e->vel[1]);
					e->vel[1] = (int16_t)(e->vel[1] - (int16_t)(e->vel[1] >> 4));
					alive++;
				}
			}
		}
		FieldFree(0xB4);
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter >= 0 && n->counter <= 8)
		{
			for (int k = 0; k < 8; k++)
			{
				int idx = 0;
				while (idx < 100 && Parts()[idx].type != 0) idx++;
				if (idx >= 100) break;
				Particle *e = &Parts()[idx];
				e->type = 2;
				e->frame = 0;
				e->angle = (int16_t)(CrtRand() % 0x900 + 0xD00);
				*(uint32_t *)&e->pos[0] = rd32(&TargetPos()[0]);
				*(uint32_t *)&e->pos[2] = rd32(&TargetPos()[2]);
				e->pos[0] = (int16_t)(e->pos[0] + (CrtRand() % 1000 - 500));
				e->pos[1] = (int16_t)(e->pos[1] + (CrtRand() % 1000 - 500));
				e->pos[2] = (int16_t)(e->pos[2] + (CrtRand() % 1000 - 500));
				e->vel[1] = (int16_t)(-10 - CrtRand() % 60);
			}
		}
		n->counter++;
		if (n->counter >= 4 && alive == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl BurstTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		BurstDraw(n, n->counter, BurstFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int32_t)step / 7);
		n->counter++;
		return n->counter >= 0xE ? TASK_END : 0;
	}

	static uint32_t __cdecl WebTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		WebDraw(n, n->counter, Anchor()[1], WebFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return n->counter >= 0x2D ? TASK_END : 0;
	}

	static uint32_t __cdecl ThreadTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		uint8_t *h = (uint8_t *)FieldAlloc(0x40);
		int16_t *v = (int16_t *)(h + 0x20);
		v[1] = 0;
		v[2] = 0;
		v[0] = (int16_t)0xF000;
		MatrixMulVector((const Mat4x3 *)(Entity(CasterSlot()) + 0x40), v, v);
		const int32_t k = mul32(ComputeSin(n->f10), 600) >> 12;
		Anchor()[0] = (int16_t)((mul32(v[0], k) >> 12) + TargetPos()[0]);
		Anchor()[1] = n->f12;
		Anchor()[2] = (int16_t)((mul32(v[2], k) >> 12) + TargetPos()[2]);
		int32_t slot = n->counter % 5;
		RingEntry &r = Ring()[slot];
		r.valid = 1;
		r.p0 = rd32(&CasterPos()[0]);
		r.p1 = rd32(&CasterPos()[2]);
		r.q0 = rd32(&Anchor()[0]);
		r.q1 = rd32(&Anchor()[2]);
		DrawLine(CasterPos(), Anchor(), 0x808080);
		const int32_t cnt = n->counter >= 0x19 ? 0x1E - n->counter : 5;
		int32_t i = 0;
		while (i < cnt)
		{
			const RingEntry &e = Ring()[slot];
			if (!e.valid) break;
			((uint32_t *)game_addr(PTS_A))[i * 2] = e.p0;
			((uint32_t *)game_addr(PTS_A))[i * 2 + 1] = e.p1;
			((uint32_t *)game_addr(PTS_B))[i * 2] = e.q0;
			((uint32_t *)game_addr(PTS_B))[i * 2 + 1] = e.q1;
			slot--;
			if (slot < 0) slot = 4;
			i++;
		}
		if (i > 1)
		{
			void *work = FieldAlloc(0x190);
			SplineSetup(i, game_addr(PTS_A), work);
			for (int32_t j = 0, t = 0; j < 16; j++, t += 0x1000) SplineEval(i, work, game_addr(SPL_A) + j * 8, t / 15);
			SplineSetup(i, game_addr(PTS_B), work);
			for (int32_t j = 0, t = 0; j < 16; j++, t += 0x1000) SplineEval(i, work, game_addr(SPL_B) + j * 8, t / 15);
			FieldFree(0x190);
			DrawStrips(h, (const uint8_t *)game_addr(SPL_A), (const uint8_t *)game_addr(SPL_B));
		}
		FieldFree(0x40);
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter < 0x19)
		{
			n->f10 = (int16_t)(n->f10 + n->f14);
			n->f12 = (int16_t)(n->f12 + n->f16);
		}
		n->counter++;
		return n->counter >= 0x1E ? TASK_END : 0;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		GetEffectSpawnPosition(Entity(CasterSlot()), 0xE, 0x1000, CasterPos());
		CasterPos()[1] = (int16_t)(CasterPos()[1] - 100);
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter == 0)
		{
			GetDefaultEffectPosition(Entity(TargetSlot()), TargetPos());
			const uint8_t *e = Entity(TargetSlot());
			TargetPos()[1] = (int16_t)(((int32_t)*(const int16_t *)(e + 0x3C) + *(const int16_t *)(e + 0x36)) >> 1);
		}
		if (n->counter == 0xD)
		{
			Node *t = AddTask(game_addr(ORIG_ThreadTask));
			t->counter = 0;
			t->f10 = 0;
			t->f14 = 0x380;
			t->f12 = (int16_t)(TargetPos()[1] + 0x2EE);
			t->f16 = (int16_t)0xFFC4;
			for (int k = 0; k < 5; k++) Ring()[k].valid = 0;
			if (n->counter == 0xD)
			{
				Node *w = AddTask(game_addr(ORIG_WebTask));
				*(uint32_t *)&w->f10 = rd32(&TargetPos()[0]);
				w->counter = 0;
				*(uint32_t *)&w->f14 = rd32(&TargetPos()[2]);
				w->f18 = (int16_t)(CrtRand() % 0x1000);
				w->f1C = 0xC00;
			}
		}
		if (n->counter == 0xA) AddTask(game_addr(ORIG_Particles1Task))->counter = 0;
		if (n->counter == 0x30) AddTask(game_addr(ORIG_Particles2Task))->counter = 0;
		if (n->counter == 0x32)
		{
			Node *b = AddTask(game_addr(ORIG_BurstTask));
			*(uint32_t *)&b->f10 = rd32(&TargetPos()[0]);
			b->counter = 0;
			*(uint32_t *)&b->f14 = rd32(&TargetPos()[2]);
			b->f18 = (int16_t)(CrtRand() % 0x1000);
			const int32_t r = CrtRand();
			const int16_t s = (int16_t)((r % 0x500 + 0x980) / 4);
			b->f1E = s;
			b->f1C = s;
		}
		if (n->counter == 0x37) ApplyActionResultToTarget(Ctx()->actions[0].targets);
		if (n->counter == 1) BdPlaySE(SOUND_Web(), 0, 0x80);
		n->counter++;
		return n->counter > 0x3C ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n_)
	{
		RootNode *r = (RootNode *)n_;
		Cursor() = game_addr(ARENA_A);
		if (!(*(const uint8_t *)&r->counter & 1)) Cursor() = game_addr(ARENA_B);
		const int a = ExecuteTaskQueue(QEffects());
		r->counter++;
		return a ? 0 : TASK_END;
	}
}

	void register_mag153_sticky_web()
	{
		register_port(web153::ORIG_RootTask, (void *)web153::RootTask, "W153 RootTask", 153);
		register_port(web153::ORIG_MasterTask, (void *)web153::MasterTask, "W153 MasterTask", 153);
		register_port(web153::ORIG_ThreadTask, (void *)web153::ThreadTask, "W153 ThreadTask", 153);
		register_port(web153::ORIG_WebTask, (void *)web153::WebTask, "W153 WebTask", 153);
		register_port(web153::ORIG_Particles1Task, (void *)web153::Particles1Task, "W153 Particles1Task", 153);
		register_port(web153::ORIG_Particles2Task, (void *)web153::Particles2Task, "W153 Particles2Task", 153);
		register_port(web153::ORIG_BurstTask, (void *)web153::BurstTask, "W153 BurstTask", 153);
	}
}

