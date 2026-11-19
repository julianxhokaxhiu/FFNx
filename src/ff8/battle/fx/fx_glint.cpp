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

// Effect library glint task, shared by the spells

#include "mag_common.h"

namespace ff8fx
{
namespace glint
{
	using namespace eng;
	using namespace magc;

#pragma pack(push, 1)
	struct FxNode
	{
		TaskNode hdr;
		CastContext *ctx;
		FxNode *root;
		FxNode *emitter;
		FxNode *parent;
		int16_t pos[4];
		int16_t counter;
		uint16_t flags;
		uint8_t children;
		int8_t phase;
		int8_t action;
		int8_t target;
		uint8_t attacker;
		uint8_t slot;
		uint8_t pad2E[2];
	};
	struct GlintNode
	{
		FxNode h;
		int16_t rot[3];
		int16_t pad36;
		int16_t w, hgt;
		int16_t grow_w, grow_h;
		int16_t shrink_w, shrink_h;
		uint8_t rgb_a[3];
		uint8_t pad47;
		uint8_t rgb_b[3];
		uint8_t pad4B;
		uint8_t d_a[3];
		uint8_t pad4F;
		uint8_t d_b[3];
		uint8_t pad53;
		int16_t left;
		int16_t steps;
		int16_t spin;
		int16_t pad5A;
	};
#pragma pack(pop)
	static_assert(sizeof(FxNode) == 0x30 && sizeof(GlintNode) == 0x5C, "glint node");

	static constexpr game::Id ORIG_Glint = game::Effect_Glint_Tick;
	inline uint32_t *&CursorPtr() { return var<uint32_t *>(game_addr(game::Effect_GlintPacketCursorPtr)); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	inline void ReleaseLinkedTask(void *n) { fn<void (__cdecl *)(void *)>(game_addr(game::Effect_ReleaseLinkedTask))(n); }
	inline void CopyMidpointFromSource(void *n, int16_t *out) { fn<void (__cdecl *)(void *, int16_t *)>(game_addr(game::Effect_CopyMidpointFromSource))(n, out); }
	inline int32_t BoundsCenterY(void *n) { return fn<int32_t (__cdecl *)(void *)>(game_addr(game::Effect_GetTargetBoundsCenterWorldY))(n); }
	inline void MatrixMulVector(const Mat4x3 *m, const int16_t *v, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, v, out); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteReadOTZWord(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline uint32_t GteSZ(int k) { return var<uint32_t>(game_addr(game::GTE_SZ0) + 4 * k); }
	inline void InsertPrimDepthKeys(uint32_t ot, void *prim, uint32_t a, uint32_t b, uint32_t c, uint32_t d) { fn<void (__cdecl *)(uint32_t, void *, uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::SSIGPU_InsertPrimDepthKeys))(ot, prim, a, b, c, d); }
	inline uint32_t GetTPage(int32_t tp, int32_t abr, int32_t x, int32_t y) { return fn<uint32_t (__cdecl *)(int32_t, int32_t, int32_t, int32_t)>(game_addr(game::sub_45C690))(tp, abr, x, y); }
	inline void SetDrawMode(void *p, int32_t dfe, int32_t dtd, uint32_t tpage, const void *tw) { fn<void (__cdecl *)(void *, int32_t, int32_t, uint32_t, const void *)>(game_addr(game::GPU_SetDrawMode))(p, dfe, dtd, tpage, tw); }

	template<typename T> static inline T &F(uint8_t *p, uint32_t off) { return *(T *)(p + off); }

	static void Step(GlintNode *g)
	{
		if (g->h.phase == 1)
		{
			g->w = (int16_t)(g->w + g->grow_w);
			g->hgt = (int16_t)(g->hgt + g->grow_h);
			for (int k = 0; k < 3; k++) g->rgb_a[k] = (uint8_t)(g->rgb_a[k] + g->d_a[k]);
			for (int k = 0; k < 3; k++) g->rgb_b[k] = (uint8_t)(g->rgb_b[k] + g->d_b[k]);
			g->left--;
			if (g->left <= 0)
			{
				g->left = g->steps;
				g->h.phase++;
			}
		}
		else if (g->h.phase == 2)
		{
			g->w = (int16_t)(g->w - g->shrink_w);
			g->hgt = (int16_t)(g->hgt - g->shrink_h);
			for (int k = 0; k < 3; k++) g->rgb_a[k] = (uint8_t)(g->rgb_a[k] - g->d_a[k]);
			for (int k = 0; k < 3; k++) g->rgb_b[k] = (uint8_t)(g->rgb_b[k] - g->d_b[k]);
			g->left--;
			if (g->left <= 0)
			{
				*(uint8_t *)&g->h.flags |= 5;
				g->h.phase++;
			}
		}
	}

	static void Draw(const GlintNode *g, uint32_t ot, uint32_t *cursor)
	{
		uint8_t *p = (uint8_t *)*cursor;
		Mat4x3 m;
		ComposeZYXRotationMatrix(g->rot, &m);
		const int16_t w = g->w, h = g->hgt;
		const int16_t hw = (int16_t)(w / 2), hh = (int16_t)(h / 2);
		int16_t in[5][4];
		in[0][0] = 0;
		in[0][1] = 0;
		in[0][2] = 0;
		in[1][0] = hw;
		in[1][1] = hh;
		in[1][2] = 0;
		in[2][0] = hw;
		in[2][1] = (int16_t)-hh;
		in[2][2] = 0;
		in[3][0] = w;
		in[3][1] = h;
		in[3][2] = 0;
		in[4][0] = w;
		in[4][1] = (int16_t)-h;
		in[4][2] = 0;
		int16_t out[5][4];
		for (int k = 0; k < 5; k++)
		{
			MatrixMulVector(&m, in[k], out[k]);
			out[k][0] = (int16_t)(out[k][0] + g->h.pos[0]);
			out[k][1] = (int16_t)(out[k][1] + g->h.pos[1]);
			out[k][2] = (int16_t)(out[k][2] + g->h.pos[2]);
		}
		GteSetTransVectorCtrl(&Camera());
		GteSetRotMatrixCtrl(&Camera());
		uint32_t sxy[5];
		GteLoadV012(out[0], out[1], out[2]);
		GteRTPT();
		GteReadSXY012(&sxy[0], &sxy[1], &sxy[2]);
		const uint32_t z0 = GteSZ(1), z1 = GteSZ(2);
		GteAVSZ3();
		int32_t otz;
		GteReadOTZWord(&otz);
		otz >>= 2;
		GteLoadV012(out[2], out[3], out[4]);
		GteRTPT();
		GteReadSXY012(&sxy[2], &sxy[3], &sxy[4]);
		const uint32_t z2 = GteSZ(1), z3 = GteSZ(2), z4 = GteSZ(3);
		p[4] = g->rgb_a[0];
		p[5] = g->rgb_a[1];
		p[6] = g->rgb_a[2];
		F<uint32_t>(p, 0) = 0x6000000;
		p[7] = 0x32;
		p[0xC] = g->rgb_b[0];
		p[0xD] = g->rgb_b[1];
		p[0xE] = g->rgb_b[2];
		p[0x14] = g->rgb_b[0];
		p[0x15] = g->rgb_b[1];
		p[0x16] = g->rgb_b[2];
		F<uint32_t>(p, 8) = sxy[0];
		F<uint32_t>(p, 0x10) = sxy[1];
		F<uint32_t>(p, 0x18) = sxy[2];
		const uint32_t bucket = ot + (uint32_t)otz * 4;
		InsertPrimDepthKeys(bucket, p, z0, z1, z2, 0);
		p += 0x1C;
		F<uint32_t>(p, 0) = 0x8000000;
		p[7] = 0x3A;
		p[4] = g->rgb_b[0];
		p[5] = g->rgb_b[1];
		p[6] = g->rgb_b[2];
		p[0xC] = g->rgb_b[0];
		p[0xD] = g->rgb_b[1];
		p[0xE] = g->rgb_b[2];
		p[0x1E] = 0;
		p[0x1D] = 0;
		p[0x1C] = 0;
		p[0x16] = 0;
		p[0x15] = 0;
		p[0x14] = 0;
		F<uint32_t>(p, 8) = sxy[1];
		F<uint32_t>(p, 0x10) = sxy[2];
		F<uint32_t>(p, 0x18) = sxy[3];
		F<uint32_t>(p, 0x20) = sxy[4];
		InsertPrimDepthKeys(bucket, p, z1, z2, z3, z4);
		p += 0x24;
		const uint32_t tpage = GetTPage(0, 1, 0x280, 0) & 0xFFFF;
		SetDrawMode(p, 0, 0, tpage, nullptr);
		InsertPrimAutoDepth(bucket, p);
		p += 0xC;
		*cursor = (uint32_t)p;
	}
}
}

namespace ff8fx
{
namespace glint
{
	static uint32_t __cdecl GlintTask(TaskNode *n)
	{
		GlintNode *g = (GlintNode *)n;
		const uint32_t ot = RenderOT(0x44);
		CopyMidpointFromSource(g, g->h.pos);
		g->h.pos[1] = (int16_t)BoundsCenterY(g);
		if (g->h.phase == 0)
		{
			g->left = g->steps;
			g->w = 0;
			g->hgt = 0;
			g->rot[1] = (int16_t)(CrtRand() & 0xFFF);
			g->rot[0] = (int16_t)(CrtRand() & 0xFFF);
			g->rot[2] = (int16_t)(CrtRand() & 0xFFF);
			g->rgb_b[2] = 0;
			g->rgb_b[1] = 0;
			g->rgb_b[0] = 0;
			g->rgb_a[2] = 0;
			g->rgb_a[1] = 0;
			g->rgb_a[0] = 0;
			g->h.phase++;
		}
		else Step(g);
		g->rot[1] = (int16_t)((uint16_t)(g->spin + g->rot[1]) & 0xFFF);
		if (!(g->h.flags & 4))
		{
			Draw(g, ot, CursorPtr());
		}
		g->h.counter++;
		if ((g->h.flags & 1) && g->h.children == 0)
		{
			ReleaseLinkedTask(g);
			return TASK_END;
		}
		return 0;
	}
}

	void register_fx_glint()
	{
		register_port(glint::ORIG_Glint, (void *)glint::GlintTask, "shared 8DCC20 Effect_Glint_Tick", 1);
	}
}

