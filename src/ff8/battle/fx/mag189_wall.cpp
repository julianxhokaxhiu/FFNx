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

// Effect 189: Wall

#include "mag_common.h"

namespace ff8fx
{
namespace wall189
{
	using namespace eng;
	using namespace magc;

	inline int32_t &FirstTarget() { return var<int32_t>(game_addr(game::unk_2293F4C)); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::unk_2293F80); }
	inline TaskQueue *QEffects() { return (TaskQueue *)game_addr(game::unk_2295C00); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_2296A20)); }
	inline uint32_t &Cursor() { return var<uint32_t>(game_addr(game::unk_22B2A30)); }
	inline int16_t &CameraShakeY() { return var<int16_t>(game_addr(game::BATTLE_CAMERA_SHAKE_OFFSET_Y)); }
	inline uint8_t DisplayBuffer() { return var<uint8_t>(game_addr(game::g_Battle_DisplayParity)); }

	static constexpr game::Id ARENA_ODD = game::unk_22A4A30, ARENA_EVEN = game::unk_2296A30;
	static constexpr game::Id PKT_DrawEnv = game::unk_2295BC0;
	static constexpr game::Id PKT_DrawArea = game::unk_2293F50;
	static constexpr game::Id PKT_DrawOffset = game::unk_2294DA0;
	static constexpr game::Id OFFSET_XY = game::unk_2296A2C;
	static constexpr game::Id PKT_AltViewport = game::unk_D1B490;
	static constexpr game::Id DRAWENV = game::g_Battle_DrawEnv;
	static constexpr game::Id STAGE_Entry1 = game::unk_1D989BD;

	static constexpr game::Id ORIG_RootTask = game::MAG_189_WALL_RootTask;
	static constexpr game::Id ORIG_RestoreTask = game::MAG_189_WALL_RestoreTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_189_WALL_MainTask;
	static constexpr game::Id ORIG_WallTask = game::MAG_189_WALL_WallTask;
	static constexpr game::Id ORIG_ShardTask = game::MAG_189_WALL_ShardTask;
	static constexpr game::Id ORIG_DustTask = game::MAG_189_WALL_DustTask;
	static constexpr game::Id ORIG_DebrisTask = game::MAG_189_WALL_DebrisTask;
	static constexpr game::Id MODEL_Wall = game::unk_D19B74;
	static constexpr game::Id MODEL_Shard = game::unk_D19804;
	static constexpr game::Id MODEL_Debris = game::unk_D19ABC;
	static constexpr game::Id SEQ_Dust = game::unk_D19658;
	static constexpr game::Id CLIP_TABLE = game::unk_D1B3AC;
	static const void *SOUND_Wall() { return (const void *)game_addr(game::unk_D1B3A4); }
	static const int DUST_COUNT = 150, DEBRIS_COUNT = 100;
	static const int32_t WALL_TICKS = 0x3C, WALL_CLIPPED = 0x14, WALL_SHAKE = 0x1A, WALL_FADE = 0x30;
	static const int32_t SHARD_TICKS = 0x18, SHARD_GROW = 6, SHARD_FADE = 0x10;

	inline void CalculateCenterPosition(uint32_t mask, int16_t *out) { fn<void (__cdecl *)(uint32_t, int16_t *)>(game_addr(game::CalculateCenterPosition))(mask, out); }
	inline void MatVec(const void *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const void *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline void NormalizeVectorFixed(const int32_t *in, int32_t *out) { fn<void (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline void SetDrawEnvPrim(uint32_t prim, uint32_t env) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_SetDrawEnv))(prim, env); }
	inline void SetDrawAreaPrim(uint32_t prim, const int16_t *rect) { fn<void (__cdecl *)(uint32_t, const int16_t *)>(game_addr(game::GPU_SetDrawAreaPrim))(prim, rect); }
	inline void SetDrawOffsetPrim(uint32_t prim, uint32_t xy) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_SetDrawOffsetPrim))(prim, xy); }
	inline void InsertPrimAltViewport(uint32_t bucket, uint32_t prim) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(bucket, prim); }
	inline void GetScreenOffset(int32_t *x, int32_t *y) { fn<void (__cdecl *)(int32_t *, int32_t *)>(game_addr(game::GTE_ReadScreenOffset))(x, y); }
	inline void SetScreenOffset(int32_t x, int32_t y) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::Call_Bs_ParseCamera))(x, y); }
	inline void SetProjectionH(int32_t h) { fn<void (__cdecl *)(int32_t)>(game_addr(game::Battle_SetProjectionH))(h); }
	inline uint32_t RenderGeometry(uint32_t group, void *header, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(group, header, ot, mode, cursor); }
	inline uint32_t ColumnRender(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_166_MegiddoFlame_ColumnRender))(h, ot, mode, cursor); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZWord(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteWriteControlReg(uint32_t value, int32_t reg) { fn<void (__cdecl *)(uint32_t, int32_t)>(game_addr(game::GTE_WriteControlReg))(value, reg); }
	inline void GteLightV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteLightV1() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV1_Bk))(); }
	inline void GteLightV2() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV2_Bk))(); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t done;
	};
	struct Node
	{
		TaskNode hdr;
		int16_t counter;
		int16_t delay;
		int16_t x;
		int16_t y;
		int16_t z;
		int16_t w;
		int16_t yaw;
		int16_t pad1A;
		int16_t scale;
		int16_t pad1E;
		int16_t scale_y;
		int16_t step;
	};
	struct Dust
	{
		int32_t flag;
		int16_t frame;
		int16_t angle;
		int16_t pos[4];
		int16_t vel[3];
		int16_t pad16;
	};
	struct Debris
	{
		int32_t flag;
		int16_t life;
		int16_t scale;
		int16_t x, y, z;
		int16_t vy;
		int16_t spin_x;
		int16_t ang_x;
		int16_t spin_z;
		int16_t ang_z;
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
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node) == 0x24, "Wall nodes");
	static_assert(sizeof(Dust) == 0x18 && sizeof(Debris) == 0x18, "Wall particles");
	static_assert(sizeof(ClipHeader) == 0x1A8, "Wall clip header");

	inline Dust *Dusts() { return (Dust *)game_addr(game::unk_2294DB0); }
	inline Debris *Pieces() { return (Debris *)game_addr(game::unk_2293F90); }
	inline uint32_t rd32(const void *p) { return *(const uint32_t *)p; }
	inline Node *AddTask(uint32_t f) { return (Node *)AddTaskToQueue(QEffects(), f); }

	static int16_t WallPlane(int32_t c) { return (int16_t)(c * 2013 / 20 - 0x7DD); }
	static int32_t WallFade(int32_t c) { return c * 341 - 0x30 * 341; }
	static int32_t ShardFade(int32_t c) { return shl32(c - SHARD_FADE, 9); }
}
}

namespace ff8fx
{
namespace wall189
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

	static void WallDraw(const Node *n, int32_t c, int16_t plane_y, int32_t fade)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[1] = n->y;
		m.t[0] = n->x;
		const int32_t s = n->scale;
		const int32_t v[3] = { s, s, s };
		m.t[2] = n->z;
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		if (c < WALL_CLIPPED)
		{
			ClipHeader *h = (ClipHeader *)FieldAlloc(0x1A8);
			h->model = (uint8_t *)game_addr(MODEL_Wall);
			h->flags = 0;
			h->slot[0].v[0] = 0;
			h->slot[0].v[2] = 0;
			h->plane[0] = 0;
			h->plane[1] = (int16_t)0xF000;
			h->plane[2] = 0;
			h->slot[0].v[1] = plane_y;
			PlaneDistance(h->slot[0].v, h->plane);
			Cursor() = ClipRender(h, RenderOT(), 2, Cursor());
			FieldFree(0x1A8);
		}
		else
		{
			uint8_t *h = (uint8_t *)FieldAlloc(0x60);
			*(uint32_t *)h = game_addr(MODEL_Wall);
			*(uint32_t *)(h + 8) = 0;
			*(uint32_t *)(h + 0x1C) = 0;
			*(uint32_t *)(h + 0x58) = 0xE1000020;
			*(uint32_t *)(h + 0x5C) = 0xE1000220;
			if (c >= WALL_FADE)
			{
				*(uint32_t *)(h + 0x1C) = 0xF3;
				*(int32_t *)(h + 0xC) = fade;
			}
			Cursor() = ColumnRender(h, RenderOT(), 2, Cursor());
			FieldFree(0x60);
		}
	}

	static void ShardDraw(const Node *n, int32_t c, int32_t fade)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = n->yaw;
		ang[2] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = n->x;
		m.t[1] = n->y;
		const int32_t v[3] = { n->scale, n->scale_y, n->scale };
		m.t[2] = n->z;
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = game_addr(MODEL_Shard);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (c >= SHARD_FADE)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		Cursor() = RenderPrimModel(h, RenderOT(), 2, Cursor());
		FieldFree(0x58);
	}

	static void DustDraw(uint8_t *h, int16_t frame, const int16_t *pos, int16_t angle)
	{
		*(int16_t *)(h + 4) = frame;
		TransformCameraByShadowRotation(pos, angle, -((int32_t)angle >> 4));
		Cursor() = InitEffectSequenceFromData(h, RenderOT(), 2, Cursor());
	}

	static void DebrisDraw(uint8_t *h, uint8_t *w, const Debris *p)
	{
		*(int16_t *)w = p->ang_x;
		*(int16_t *)(w + 4) = p->ang_z;
		Mat4x3 *m = (Mat4x3 *)(w + 8);
		ComposeZYXRotationMatrix((const int16_t *)w, m);
		m->t[1] = p->y;
		m->t[2] = p->z;
		m->t[0] = p->x;
		int32_t *v = (int32_t *)(w + 0x48);
		v[2] = p->scale;
		v[1] = p->scale;
		v[0] = p->scale;
		Scale3DMatrix(m, v);
		ComposeAffineTransform((const Mat4x3 *)(w + 0x28), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		Cursor() = RenderPrimModel(h, RenderOT(), 2, Cursor());
	}

	static void Capture(const int16_t *point)
	{
		int16_t rect[4] = { 0x180, 0x100, 0x100, 0x100 };
		const int32_t tx = -1 - (int32_t)point[0] / 18;
		const int32_t ty = (int32_t)point[2] / 4 / 2 + 1;
		int16_t ang[4];
		ang[0] = 0x400;
		ang[1] = 0;
		ang[2] = 0;
		Mat4x3 m;
		m.t[0] = tx;
		m.t[1] = ty;
		m.t[2] = 0x7FFF;
		BuildRotationMatrixFromAngles(ang, &m);
		int32_t v[3];
		v[1] = 0x1000;
		v[0] = 0x1000000 / (0x1162000 / (int32_t)rect[2]);
		v[2] = 0x1000000 / (0xAF0000 / (int32_t)rect[3]);
		Scale3DMatrix(&m, v);
		SetDrawEnvPrim(game_addr(PKT_DrawEnv), game_addr(DRAWENV) + 0x5C * ((uint32_t)DisplayBuffer() ^ 1));
		InsertPrimAutoDepth(RenderOT(), (void *)game_addr(PKT_DrawEnv));
		int32_t ofx, ofy;
		GetScreenOffset(&ofx, &ofy);
		SetScreenOffset((int32_t)rect[2] / 2, (int32_t)rect[3] / 2);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x2C);
		*(int16_t *)(h + 0x14) = 0;
		*(int16_t *)(h + 0x16) = 0;
		*(int16_t *)(h + 0x18) = 0x140;
		*(int16_t *)(h + 0x1A) = 0xD8;
		*(uint32_t *)(h + 4) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		*(int32_t *)(h + 0x10) = -1;
		*(int32_t *)(h + 0x20) = -1;
		SetProjectionH(0x7FFF);
		for (uint32_t e = game_addr(STAGE_Entry1); e >= game_addr(STAGE_Entry1) - 0x2C; e -= 0x2C)
		{
			const uint8_t f = var<uint8_t>(e);
			if (!(f & 1) || !(f & 2)) continue;
			*(uint32_t *)(h + 0x1C) = var<uint32_t>(e + 0x27);
			*(uint16_t *)(h + 0x24) = var<uint16_t>(e + 1);
			BuildBoneMatricesFromPose((void *)(e + 0x13));
			ComputeBonesWorldMatrices((void *)(e + 0x13), &m);
			Cursor() = RenderGeometry(e + 3, h, RenderOT(), 0x10, Cursor());
		}
		SetProjectionH(var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)));
		FieldFree(0x2C);
		SetScreenOffset(ofx, ofy);
		InsertPrimAltViewport(RenderOT(), game_addr(PKT_AltViewport));
		SetDrawAreaPrim(game_addr(PKT_DrawArea), rect);
		InsertPrimAutoDepth(RenderOT(), (void *)game_addr(PKT_DrawArea));
		var<int16_t>(game_addr(OFFSET_XY)) = rect[0];
		var<int16_t>(game_addr(OFFSET_XY) + 2) = rect[1];
		SetDrawOffsetPrim(game_addr(PKT_DrawOffset), game_addr(OFFSET_XY));
		InsertPrimAutoDepth(RenderOT(), (void *)game_addr(PKT_DrawOffset));
	}

	static uint32_t __cdecl DebrisTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		uint8_t *w = (uint8_t *)FieldAlloc(0x88);
		ff8fx::copy<(0x20)>(w + 0x28, &Camera());
		*(uint32_t *)h = game_addr(MODEL_Debris);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0;
		int32_t alive = 0;
		*(int16_t *)(w + 2) = 0;
		for (Debris *p = Pieces(); p < Pieces() + DEBRIS_COUNT; p++)
		{
			if (!(*(const uint8_t *)&p->flag & 1)) continue;
			DebrisDraw(h, w, p);
			if (!(UpdateFlags() & 0x201))
			{
				p->life++;
				if (p->life >= 0x10) p->flag = 0;
				else
				{
					if (p->life >= 0xC) p->scale = (int16_t)(p->scale - (int16_t)(p->scale >> 3));
					p->y = (int16_t)(p->y + p->vy);
					p->vy = (int16_t)(p->vy - (int16_t)(p->vy >> 3));
					p->ang_x = (int16_t)(p->ang_x + p->spin_x);
					p->ang_z = (int16_t)(p->ang_z + p->spin_z);
					alive++;
				}
			}
		}
		FieldFree(0x88);
		FieldFree(0x58);
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter >= 0 && n->counter <= 0x18)
		{
			for (int k = 0; k < 3; k++)
			{
				int idx = 0;
				while (idx < DEBRIS_COUNT && Pieces()[idx].flag != 0) idx++;
				if (idx >= DEBRIS_COUNT) break;
				Debris *p = &Pieces()[idx];
				p->flag = 1;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x2C0 + 0x80);
				*(uint32_t *)&p->x = rd32(&n->x);
				*(uint32_t *)&p->z = rd32(&n->z);
				const int32_t a = shl32(CrtRand() % 24, 12) / 24;
				p->x = (int16_t)(p->x + (int16_t)(mul32(ComputeCos(a), 1800) >> 12));
				p->y = (int16_t)-(CrtRand() % 200);
				const int32_t s = mul32(ComputeSin(a), 1800) >> 12;
				p->z = (int16_t)(p->z + (int16_t)(mul32(s, 1280) >> 12));
				p->vy = (int16_t)(-25 - CrtRand() % 200);
				int16_t sx = (int16_t)(CrtRand() % 120 + 60);
				if (sx & 1) sx = (int16_t)-sx;
				p->spin_x = sx;
				int16_t sz = (int16_t)(CrtRand() % 60 + 30);
				if (sz & 1) sz = (int16_t)-sz;
				p->spin_z = sz;
				p->ang_x = (int16_t)(CrtRand() % 0x1000);
				p->ang_z = (int16_t)(p->ang_x >> 1);
			}
		}
		n->counter++;
		if (n->counter >= 4 && alive == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl DustTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t alive = 0;
		*(uint32_t *)h = game_addr(SEQ_Dust);
		*(int16_t *)(h + 0x24) = 8;
		for (Dust *p = Dusts(); p < Dusts() + DUST_COUNT; p++)
		{
			if (!(*(const uint8_t *)&p->flag & 1)) continue;
			DustDraw(h, p->frame, p->pos, p->angle);
			if (!(UpdateFlags() & 0x201))
			{
				p->frame++;
				if (*(const int16_t *)(h + 0x28) < 0) p->flag = 0;
				else
				{
					p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
					p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
					p->vel[0] = (int16_t)(p->vel[0] - (int16_t)(p->vel[0] >> 4));
					p->vel[2] = (int16_t)(p->vel[2] - (int16_t)(p->vel[2] >> 4));
					alive++;
				}
			}
		}
		FieldFree(0xB4);
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter >= 0 && n->counter <= 0x38)
		{
			for (int k = 0; k < 4; k++)
			{
				int idx = 0;
				while (idx < DUST_COUNT && Dusts()[idx].flag != 0) idx++;
				if (idx >= DUST_COUNT) break;
				Dust *p = &Dusts()[idx];
				p->flag = 1;
				p->frame = 0;
				p->angle = (int16_t)(CrtRand() % 0xA00 + 0xA00);
				*(uint32_t *)&p->pos[0] = rd32(&n->x);
				*(uint32_t *)&p->pos[2] = rd32(&n->z);
				const int32_t a = shl32(CrtRand() % 24, 12) / 24;
				p->pos[0] = (int16_t)(p->pos[0] + (int16_t)(mul32(ComputeCos(a), 1800) >> 12));
				const int32_t s = mul32(ComputeSin(a), 1800) >> 12;
				p->pos[2] = (int16_t)(p->pos[2] + (int16_t)(mul32(s, 1280) >> 12));
				int32_t v[3];
				v[1] = 0;
				v[0] = (int32_t)p->pos[0] - n->x;
				v[2] = (int32_t)p->pos[2] - n->z;
				NormalizeVectorFixed(v, v);
				const int32_t r = CrtRand() % 25 + 20;
				p->vel[0] = (int16_t)(mul32(v[0], r) >> 12);
				p->vel[1] = -100;
				p->vel[2] = (int16_t)(mul32(v[2], r) >> 12);
			}
		}
		n->counter++;
		if (n->counter >= 4 && alive == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl ShardTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		if (n->delay > 0)
		{
			if (UpdateFlags() & 0x201) return 0;
			n->delay--;
			return 0;
		}
		ShardDraw(n, n->counter, ShardFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter < SHARD_GROW) n->scale_y = (int16_t)(n->scale_y + n->step);
		n->counter++;
		return n->counter >= SHARD_TICKS ? TASK_END : 0;
	}

	static uint32_t __cdecl WallTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		WallDraw(n, n->counter, WallPlane(n->counter), WallFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t c = n->counter;
		if (c < WALL_CLIPPED) n->y = (int16_t)(n->y - n->step);
		else n->y = 0;
		if (c < WALL_SHAKE)
		{
			int32_t v = mul32(ComputeSin(shl32(c, 11) / 26), 40) >> 12;
			if (!(c & 1)) v = -v;
			CameraShakeY() = (int16_t)(CameraShakeY() + (int16_t)v);
		}
		n->counter = (int16_t)(c + 1);
		return n->counter >= WALL_TICKS ? TASK_END : 0;
	}

	static uint32_t __cdecl MainTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter == 0)
		{
			CalculateCenterPosition(Ctx()->target_mask, &n->x);
			int16_t v[4];
			v[0] = 0;
			v[1] = 0;
			v[2] = (int16_t)0xFA24;
			MatVec(Entity(FirstTarget()) + 0x40, v, v);
			n->x = (int16_t)(n->x + v[0]);
			n->y = (int16_t)(n->y + v[1]);
			n->z = (int16_t)(n->z + v[2]);
			Capture(&n->x);
		}
		if (n->counter == 1)
		{
			const int32_t base = CrtRand() % 0x1000;
			int32_t i = 0, acc = 0;
			for (int32_t k = 0x20; k > 8; k--)
			{
				Node *s = AddTask(game_addr(ORIG_ShardTask));
				s->counter = 0;
				const int32_t r = CrtRand();
				*(uint32_t *)&s->x = rd32(&n->x);
				s->delay = (int16_t)(r % k + i);
				*(uint32_t *)&s->z = rd32(&n->z);
				const int32_t a = (acc / 24 + base) & 0xFFF;
				s->x = (int16_t)(s->x + (int16_t)(mul32(ComputeCos(a), 2300) >> 12));
				const int32_t sn = mul32(ComputeSin(a), 2300) >> 12;
				s->yaw = (int16_t)(0xC00 - a);
				s->z = (int16_t)(s->z + (int16_t)(mul32(sn, 1280) >> 12));
				s->scale = (int16_t)(CrtRand() % 0x1000 + 0x800);
				s->scale_y = 0;
				s->step = 0x5AA;
				i++;
				acc += 0x1000;
			}
		}
		if (n->counter == 0xE)
		{
			Node *d = AddTask(game_addr(ORIG_DustTask));
			*(uint32_t *)&d->x = rd32(&n->x);
			d->counter = 0;
			*(uint32_t *)&d->z = rd32(&n->z);
			if (n->counter == 0xE)
			{
				Node *b = AddTask(game_addr(ORIG_DebrisTask));
				*(uint32_t *)&b->x = rd32(&n->x);
				b->counter = 0;
				*(uint32_t *)&b->z = rd32(&n->z);
			}
		}
		if (n->counter == 0x10)
		{
			Node *w = AddTask(game_addr(ORIG_WallTask));
			w->counter = 0;
			w->x = n->x;
			w->y = 0x7DD;
			w->z = n->z;
			w->scale = 0x1000;
			w->step = 0x64;
		}
		if (n->counter == 0x41) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		if (n->counter == 1) BdPlaySE(SOUND_Wall(), 0, 0x80);
		n->counter++;
		return n->counter > 0x46 ? TASK_END : 0;
	}

	static uint32_t __cdecl RestoreTask(TaskNode *n_)
	{
		RootNode *t = (RootNode *)n_;
		if (t->counter == 1) TextureRestoreTask((void *)game_addr(ARENA_EVEN), &t->done);
		const int16_t done = t->done;
		t->counter++;
		return done ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n_)
	{
		RootNode *r = (RootNode *)n_;
		Cursor() = game_addr(ARENA_ODD);
		if (!(*(const uint8_t *)&r->counter & 1)) Cursor() = game_addr(ARENA_EVEN);
		const int a = ExecuteTaskQueue(QEffects());
		if (a == 0)
		{
			RootNode *t = (RootNode *)AddTaskToQueue(QRoot(), game_addr(ORIG_RestoreTask));
			t->counter = 0;
			t->done = 0;
		}
		r->counter++;
		return a ? 0 : TASK_END;
	}
}

	void register_mag189_wall()
	{
		register_port(wall189::ORIG_RootTask, (void *)wall189::RootTask, "189 RootTask", 189);
		register_port(wall189::ORIG_RestoreTask, (void *)wall189::RestoreTask, "189 RestoreTask", 189);
		register_port(wall189::ORIG_MainTask, (void *)wall189::MainTask, "189 MainTask", 189);
		register_port(wall189::ORIG_WallTask, (void *)wall189::WallTask, "189 WallTask", 189);
		register_port(wall189::ORIG_ShardTask, (void *)wall189::ShardTask, "189 ShardTask", 189);
		register_port(wall189::ORIG_DustTask, (void *)wall189::DustTask, "189 DustTask", 189);
		register_port(wall189::ORIG_DebrisTask, (void *)wall189::DebrisTask, "189 DebrisTask", 189);
	}
}

