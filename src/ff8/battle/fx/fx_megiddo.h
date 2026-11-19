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

// Effect 166: Megiddo Flame

#pragma once

#include "mag_common.h"

namespace ff8fx
{
namespace megiddo
{
	using namespace eng;
	using namespace magc;

	inline int32_t Normalize(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void MatVec(const Mat4x3 *m, const void *in, void *out) { fn<void (__cdecl *)(const Mat4x3 *, const void *, void *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void GteMatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void SplineSetup(int32_t n, const void *points, void *work) { fn<void (__cdecl *)(int32_t, const void *, void *)>(game_addr(game::Effect_SplineSetup))(n, points, work); }
	inline void SplineEval(int32_t n, void *work, void *out, int32_t t) { fn<void (__cdecl *)(int32_t, void *, void *, int32_t)>(game_addr(game::Effect_SplineEval))(n, work, out, t); }
	inline void CalculateCenterPosition(uint32_t mask, int16_t *out) { fn<void (__cdecl *)(uint32_t, int16_t *)>(game_addr(game::CalculateCenterPosition))(mask, out); }
	inline void QueueChainTransformation(uint8_t *entity, int32_t seq) { fn<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, seq); }
	inline void SetDrawEnvPrim(uint32_t prim, uint32_t env) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_SetDrawEnv))(prim, env); }
	inline void InsertPrimAltViewport(uint32_t bucket, uint32_t packet) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(bucket, packet); }
	inline void SetDrawAreaPrim(uint32_t prim, const void *rect) { fn<void (__cdecl *)(uint32_t, const void *)>(game_addr(game::GPU_SetDrawAreaPrim))(prim, rect); }
	inline void SetDrawOffsetPrim(uint32_t prim, const void *xy) { fn<void (__cdecl *)(uint32_t, const void *)>(game_addr(game::SSIGPU_SetDrawOffsetPrim))(prim, xy); }
	inline void GteReadScreenOffset(int32_t *x, int32_t *y) { fn<void (__cdecl *)(int32_t *, int32_t *)>(game_addr(game::GTE_ReadScreenOffset))(x, y); }
	inline void GteSetScreenOffset(int32_t x, int32_t y) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::Call_Bs_ParseCamera))(x, y); }
	inline void SetProjectionH(int32_t h) { fn<void (__cdecl *)(int32_t)>(game_addr(game::Battle_SetProjectionH))(h); }
	inline uint32_t RenderGeometry(uint32_t geom, void *hdr, uint32_t ot, int32_t shift, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(geom, hdr, ot, shift, cursor); }
	inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteSetFarColor2(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColorRGB))(r, g, b); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
	inline void GteLoadRGB012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadRGB012))(a, b, c); }
	inline void GteDPCT() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCT))(); }
	inline void GteStoreRGB012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_StoreRGB012))(a, b, c); }
	inline void GteLoadV012(const void *v0, const void *v1, const void *v2) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(v0, v1, v2); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteStoreOTZ(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteSetBackColorFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
	inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
	inline void GteTransFromMAC() { fn<void (__cdecl *)()>(game_addr(game::GTE_TransFromMAC))(); }
	inline uint32_t Mag151Render(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_151_CannonBlow_ScrollRender))(h, ot, mode, cursor); }
	inline uint32_t ColumnRender(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_166_MegiddoFlame_ColumnRender))(h, ot, mode, cursor); }
	inline uint32_t OT0() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t c;
		int16_t e;
	};
	struct Node24
	{
		TaskNode hdr;
		int16_t c;
		int16_t e;
		int16_t w10, w12, w14, w16;
		int16_t w18;
		int16_t w1A;
		int16_t w1C;
		int16_t w1E;
		int16_t w20;
		int16_t w22;
	};
	struct RenderHeader
	{
		uint32_t model;
		uint32_t verts;
		uint8_t far_rgb[4];
		int32_t fade;
		uint16_t u_off;
		uint16_t pad12;
		uint16_t v_off;
		uint16_t pad16;
		int32_t depth;
		uint32_t flags;
		uint32_t colour;
		uint32_t list;
		int32_t mac0;
		uint32_t pad2C;
		int32_t otz;
		uint32_t gte_flag;
		uint8_t pad38[0x20];
		uint32_t scale_rgb;
		uint32_t c[4];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node24) == 0x24 && sizeof(RenderHeader) == 0x6C, "Megiddo Flame layouts");

	template<typename T> static inline T &At(uint32_t base, int32_t off) { return *(T *)(base + off); }

	namespace rm
	{
	static inline bool OutX(uint32_t xy) { int16_t v = *(const int16_t *)xy; return v < 0 || v > 0xA00; }
	static inline bool OutY(uint32_t xy) { int16_t v = *(const int16_t *)(xy + 2); return v < 0 || v > 0x6C0; }

	static inline void UvOffsets(const RenderHeader *h, uint32_t pkt, uint32_t u_word, uint32_t v_word)
	{
		const uint32_t fl = h->flags;
		if (fl & 0x400) At<uint16_t>(pkt, u_word) = (uint16_t)(At<uint16_t>(pkt, u_word) + h->u_off);
		else if (fl & 0x100) At<uint16_t>(pkt, u_word) = h->u_off;
		if (fl & 0x800) At<uint16_t>(pkt, v_word) = (uint16_t)(At<uint16_t>(pkt, v_word) + h->v_off);
		else if (fl & 0x200) At<uint16_t>(pkt, v_word) = h->v_off;
	}

	static inline void ScaleColours(RenderHeader *h, int n)
	{
		h->scale_rgb = h->colour;
		uint8_t *s = (uint8_t *)&h->scale_rgb;
		for (int k = 0; k < n; k++)
		{
			uint8_t *c = (uint8_t *)&h->c[k];
			c[0] = (uint8_t)((uint32_t)c[0] * s[0] >> 7);
			c[1] = (uint8_t)((uint32_t)c[1] * s[1] >> 7);
			c[2] = (uint8_t)((uint32_t)c[2] * s[2] >> 7);
		}
	}

	static inline uint32_t Bucket(uint32_t ot, const RenderHeader *h, int32_t shift) { return ot + (uint32_t)(h->otz >> shift) * 4; }

	static inline void FlatDepthCue(RenderHeader *h, uint32_t pkt)
	{
		if (h->flags & 0x40)
		{
			GteLoadRGBC((const void *)(pkt + 4));
			GteSetIR0(h->fade);
			GteDPCS();
			GteStoreRGB2((void *)(pkt + 4));
		}
	}

	static uint32_t ListF3(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			const uint32_t code = h->colour | 0x20000000;
			const uint32_t fl = h->flags;
			At<uint32_t>(pkt, 0) = 0x4000000;
			At<uint32_t>(pkt, 4) = code;
			if (fl & 1) At<uint32_t>(pkt, 4) = code | 0x2000000;
			if (fl & 4) At<uint32_t>(pkt, 4) &= 0xFDFFFFFF;
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x10)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0xC), (void *)(pkt + 0x10));
					GteAVSZ3();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0xC)) clip |= 2;
					if (OutX(pkt + 0x10)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0xC)) clip |= 0x20;
					if (OutY(pkt + 0x10)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						GteStoreOTZ(&h->otz);
						FlatDepthCue(h, pkt);
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x14;
					}
				}
			}
			rec += 0xC;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListF4(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			const uint32_t code = h->colour | 0x28000000;
			const uint32_t fl = h->flags;
			At<uint32_t>(pkt, 0) = 0x5000000;
			At<uint32_t>(pkt, 4) = code;
			if (fl & 1) At<uint32_t>(pkt, 4) = code | 0x2000000;
			if (fl & 4) At<uint32_t>(pkt, 4) &= 0xFDFFFFFF;
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x10)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0xC), (void *)(pkt + 0x10));
					GteLoadV0((const void *)(vb + (uint32_t)At<uint16_t>(rec, 0xA) * 4));
					GteRTPS();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0xC)) clip |= 2;
					if (OutX(pkt + 0x10)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0xC)) clip |= 0x20;
					if (OutY(pkt + 0x10)) clip |= 0x40;
					GteReadSXY2((void *)(pkt + 0x14));
					GteAVSZ4();
					if (OutX(pkt + 0x14)) clip |= 8;
					if (OutY(pkt + 0x14)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						GteStoreOTZ(&h->otz);
						FlatDepthCue(h, pkt);
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x18;
					}
				}
			}
			rec += 0xC;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListFT3(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			uint32_t code = h->colour | 0x24000000;
			At<uint32_t>(pkt, 0) = 0x7000000;
			At<uint32_t>(pkt, 4) = code;
			if (h->flags & 1) At<uint32_t>(pkt, 4) = code | 0x2000000;
			if (h->flags & 4) At<uint32_t>(pkt, 4) &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			At<uint32_t>(pkt, 0x14) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x1C) = (At<uint32_t>(rec, 8) >> 16) + d;
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x16, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x10)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x10), (void *)(pkt + 0x18));
					GteAVSZ3();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x10)) clip |= 2;
					if (OutX(pkt + 0x18)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x10)) clip |= 0x20;
					if (OutY(pkt + 0x18)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						GteStoreOTZ(&h->otz);
						FlatDepthCue(h, pkt);
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x20;
					}
				}
			}
			rec += 0x14;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListFT4(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			uint32_t code = h->colour | 0x2C000000;
			At<uint32_t>(pkt, 0) = 0x9000000;
			At<uint32_t>(pkt, 4) = code;
			if (h->flags & 1) At<uint32_t>(pkt, 4) = code | 0x2000000;
			if (h->flags & 4) At<uint32_t>(pkt, 4) &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			const uint32_t uv23 = (d << 16) + d + At<uint32_t>(rec, 0x14);
			At<uint32_t>(pkt, 0x1C) = uv23;
			At<uint32_t>(pkt, 0x14) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x24) = uv23 >> 16;
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x16, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x10)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x10), (void *)(pkt + 0x18));
					GteLoadV0((const void *)(vb + (uint32_t)At<uint16_t>(rec, 0xA) * 4));
					GteRTPS();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x10)) clip |= 2;
					if (OutX(pkt + 0x18)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x10)) clip |= 0x20;
					if (OutY(pkt + 0x18)) clip |= 0x40;
					GteReadSXY2((void *)(pkt + 0x20));
					GteAVSZ4();
					if (OutX(pkt + 0x20)) clip |= 8;
					if (OutY(pkt + 0x20)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						GteStoreOTZ(&h->otz);
						FlatDepthCue(h, pkt);
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x28;
					}
				}
			}
			rec += 0x18;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListG3(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			const uint32_t fl = h->flags;
			const uint32_t code = At<uint32_t>(rec, 0);
			At<uint32_t>(pkt, 0) = 0x6000000;
			h->c[0] = code;
			if (fl & 2) h->c[0] = code | 0x2000000;
			if (fl & 8) h->c[0] &= 0xFDFFFFFF;
			h->c[1] = At<uint32_t>(rec, 0xC);
			h->c[2] = At<uint32_t>(rec, 0x10);
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x20)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x10), (void *)(pkt + 0x18));
					GteAVSZ3();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x10)) clip |= 2;
					if (OutX(pkt + 0x18)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x10)) clip |= 0x20;
					if (OutY(pkt + 0x18)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						GteStoreOTZ(&h->otz);
						ScaleColours(h, 3);
						if (h->flags & 0x80)
						{
							GteLoadRGB012(&h->c[1], &h->c[2], &h->c[0]);
							GteSetIR0(h->fade);
							GteDPCT();
							GteStoreRGB012((void *)(pkt + 0xC), (void *)(pkt + 0x14), (void *)(pkt + 4));
						}
						else
						{
							At<uint32_t>(pkt, 4) = h->c[0];
							At<uint32_t>(pkt, 0xC) = h->c[1];
							At<uint32_t>(pkt, 0x14) = h->c[2];
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x1C;
					}
				}
			}
			rec += 0x14;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListG4(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			const uint32_t fl = h->flags;
			const uint32_t code = At<uint32_t>(rec, 0);
			At<uint32_t>(pkt, 0) = 0x8000000;
			h->c[0] = code;
			if (fl & 2) h->c[0] = code | 0x2000000;
			if (fl & 8) h->c[0] &= 0xFDFFFFFF;
			h->c[1] = At<uint32_t>(rec, 0xC);
			h->c[2] = At<uint32_t>(rec, 0x10);
			h->c[3] = At<uint32_t>(rec, 0x14);
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x20)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x10), (void *)(pkt + 0x18));
					GteLoadV0((const void *)(vb + (uint32_t)At<uint16_t>(rec, 0xA) * 4));
					GteRTPS();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x10)) clip |= 2;
					if (OutX(pkt + 0x18)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x10)) clip |= 0x20;
					if (OutY(pkt + 0x18)) clip |= 0x40;
					GteReadSXY2((void *)(pkt + 0x20));
					GteAVSZ4();
					if (OutX(pkt + 0x20)) clip |= 8;
					if (OutY(pkt + 0x20)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						GteStoreOTZ(&h->otz);
						ScaleColours(h, 4);
						if (h->flags & 0x80)
						{
							GteLoadRGB012(&h->c[1], &h->c[2], &h->c[3]);
							GteSetIR0(h->fade);
							GteDPCT();
							GteStoreRGB012((void *)(pkt + 0xC), (void *)(pkt + 0x14), (void *)(pkt + 0x1C));
							GteLoadRGBC(&h->c[0]);
							GteDPCS();
							GteStoreRGB2((void *)(pkt + 4));
						}
						else
						{
							At<uint32_t>(pkt, 4) = h->c[0];
							At<uint32_t>(pkt, 0xC) = h->c[1];
							At<uint32_t>(pkt, 0x14) = h->c[2];
							At<uint32_t>(pkt, 0x1C) = h->c[3];
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x24;
					}
				}
			}
			rec += 0x18;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListGT3(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			const uint32_t code = At<uint32_t>(rec, 0);
			At<uint32_t>(pkt, 0) = 0x9000000;
			h->c[0] = code;
			if (h->flags & 2) h->c[0] = code | 0x2000000;
			if (h->flags & 8) h->c[0] &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			At<uint32_t>(pkt, 0x18) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x24) = (At<uint32_t>(rec, 8) >> 16) + d;
			h->c[1] = At<uint32_t>(rec, 0x14);
			h->c[2] = At<uint32_t>(rec, 0x18);
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x1A, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x20)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x14), (void *)(pkt + 0x20));
					GteAVSZ3();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x14)) clip |= 2;
					if (OutX(pkt + 0x20)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x14)) clip |= 0x20;
					if (OutY(pkt + 0x20)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						GteStoreOTZ(&h->otz);
						ScaleColours(h, 3);
						if (h->flags & 0x80)
						{
							GteLoadRGB012(&h->c[1], &h->c[2], &h->c[0]);
							GteSetIR0(h->fade);
							GteDPCT();
							GteStoreRGB012((void *)(pkt + 0x10), (void *)(pkt + 0x1C), (void *)(pkt + 4));
						}
						else
						{
							At<uint32_t>(pkt, 4) = h->c[0];
							At<uint32_t>(pkt, 0x10) = h->c[1];
							At<uint32_t>(pkt, 0x1C) = h->c[2];
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x28;
					}
				}
			}
			rec += 0x1C;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListGT4(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			const uint32_t code = At<uint32_t>(rec, 0);
			At<uint32_t>(pkt, 0) = 0xC000000;
			h->c[0] = code;
			if (h->flags & 2) h->c[0] = code | 0x2000000;
			if (h->flags & 8) h->c[0] &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			const uint32_t uv23 = (d << 16) + d + At<uint32_t>(rec, 0x14);
			At<uint32_t>(pkt, 0x18) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x24) = uv23;
			At<uint32_t>(pkt, 0x30) = uv23 >> 16;
			h->c[1] = At<uint32_t>(rec, 0x18);
			h->c[2] = At<uint32_t>(rec, 0x1C);
			h->c[3] = At<uint32_t>(rec, 0x20);
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x1A, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x20)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x14), (void *)(pkt + 0x20));
					GteLoadV0((const void *)(vb + (uint32_t)At<uint16_t>(rec, 0xA) * 4));
					GteRTPS();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x14)) clip |= 2;
					if (OutX(pkt + 0x20)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x14)) clip |= 0x20;
					if (OutY(pkt + 0x20)) clip |= 0x40;
					GteReadSXY2((void *)(pkt + 0x2C));
					GteAVSZ4();
					if (OutX(pkt + 0x2C)) clip |= 8;
					if (OutY(pkt + 0x2C)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						GteStoreOTZ(&h->otz);
						ScaleColours(h, 4);
						if (h->flags & 0x80)
						{
							GteLoadRGB012(&h->c[1], &h->c[2], &h->c[3]);
							GteSetIR0(h->fade);
							GteDPCT();
							GteStoreRGB012((void *)(pkt + 0x10), (void *)(pkt + 0x1C), (void *)(pkt + 0x28));
							GteLoadRGBC(&h->c[0]);
							GteDPCS();
							GteStoreRGB2((void *)(pkt + 4));
						}
						else
						{
							At<uint32_t>(pkt, 4) = h->c[0];
							At<uint32_t>(pkt, 0x10) = h->c[1];
							At<uint32_t>(pkt, 0x1C) = h->c[2];
							At<uint32_t>(pkt, 0x28) = h->c[3];
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x34;
					}
				}
			}
			rec += 0x24;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t RenderModel(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t fl = h->flags;
		if (!(fl & 0x2000)) h->verts = h->model + 8;
		h->list = At<uint32_t>(h->model, 0) + h->model;
		if (!(fl & 0x1000)) h->depth = 0;
		GteSetFarColor(h->far_rgb[0], h->far_rgb[1], h->far_rgb[2]);
		typedef uint32_t (*List)(RenderHeader *, uint32_t, int32_t, uint32_t);
		static const List lists[8] = { ListF3, ListF4, ListFT3, ListFT4, ListG3, ListG4, ListGT3, ListGT4 };
		for (int i = 0; i < 8; i++)
		{
			if (At<uint32_t>(h->list, 0) != 0) cursor = lists[i](h, ot, shift, cursor);
			else h->list += 4;
		}
		return cursor;
	}
	}

	template<class D>
	struct Megiddo
	{
		static uint32_t C(uint32_t a) { return a - D::code(); }
		static uint32_t G(uint32_t a) { return a - D::glob(); }
		static uint32_t X(uint32_t a) { return a - D::data(); }

		static int32_t &TargetSlot() { return var<int32_t>(G(game_addr(game::unk_23BF038))); }
		static CastContext *&Context() { return var<CastContext *>(G(game_addr(game::unk_23C3250))); }
		static int32_t &CasterSlot() { return var<int32_t>(G(game_addr(game::unk_23C3254))); }
		static uint32_t &TexFile() { return var<uint32_t>(G(game_addr(game::unk_23C3258))); }
		static uint32_t &TexBase() { return var<uint32_t>(G(game_addr(game::unk_23C3270))); }
		static uint32_t &PacketCursor() { return var<uint32_t>(G(game_addr(game::unk_23C3274))); }
		static uint32_t &FlashColour() { return var<uint32_t>(G(game_addr(game::unk_23C3268))); }
		static TaskQueue *QRoot() { return (TaskQueue *)G(game_addr(game::unk_23BF070)); }
		static TaskQueue *QMain() { return (TaskQueue *)G(game_addr(game::unk_23C0540)); }
		static int16_t *Centre() { return (int16_t *)G(game_addr(game::unk_23C3248)); }
		static int16_t *Point() { return (int16_t *)G(game_addr(game::unk_23C3260)); }
		static Mat4x3 *Frame() { return (Mat4x3 *)G(game_addr(game::unk_23C04E0)); }
		static uint32_t Bolt(int32_t e) { return G(game_addr(game::unk_23C1428)) + 0x334 * e; }
		static uint32_t Save(int32_t slot) { return G(game_addr(game::unk_23C1360)) + 0x1C * slot; }
		static constexpr game::Id POOL_A = game::unk_23BF080, POOL_A_END = game::unk_23BF9E0;
		static constexpr game::Id POOL_B = game::unk_23BF9E0, POOL_B_END = game::unk_23C04D0;
		static constexpr game::Id RIBBON_TABLE = game::unk_23C2DC8;

		static constexpr game::Id ORIG_Root = game::MAG_166_MegiddoFlame_Root, ORIG_Restore = game::MAG_166_sub_5E9410, ORIG_Director = game::MAG_166_MegiddoFlame_Director, ORIG_Glow = game::MAG_166_MegiddoFlame_Glow, ORIG_Spin = game::MAG_166_MegiddoFlame_Spin, ORIG_Ring = game::MAG_166_MegiddoFlame_Ring, ORIG_BoltDir = game::MAG_166_MegiddoFlame_BoltDirector, ORIG_Arc = game::MAG_166_MegiddoFlame_Arc, ORIG_Sheet = game::MAG_166_MegiddoFlame_Sheet, ORIG_Ribbon = game::MAG_166_MegiddoFlame_Ribbon, ORIG_Column = game::MAG_166_MegiddoFlame_Column, ORIG_ColumnFlash = game::MAG_166_MegiddoFlame_ColumnFlash, ORIG_Pillar = game::MAG_166_MegiddoFlame_Pillar, ORIG_Flame = game::MAG_166_MegiddoFlame_Flame, ORIG_Sparks = game::MAG_166_MegiddoFlame_Sparks, ORIG_Debris = game::MAG_166_MegiddoFlame_Debris, ORIG_Knock = game::MAG_166_MegiddoFlame_Knock, ORIG_Throw = game::MAG_166_MegiddoFlame_Throw, ORIG_Ring2 = game::MAG_166_MegiddoFlame_Ring2;

		static constexpr game::Id BONE_OFFSET = game::unk_D78048, REF_DIR = game::unk_D78050, SOUND_START = game::unk_D7803C, SOUND_BLAST = game::unk_D78040, SHEET_COLOURS = game::unk_D78060, DEBRIS_MODELS = game::unk_D78090, ALT_VIEWPORT = game::unk_D780A0, SEQ_SPARK = game::unk_D718AC, MODEL_GLOW = game::unk_D7688C, MODEL_SPIN = game::unk_D767BC, MODEL_RING = game::unk_D7712C, MODEL_ARC = game::unk_D719E4, MODEL_COLUMN = game::unk_D7398C, MODEL_COLUMN_FLASH = game::unk_D756A4, MODEL_PILLAR = game::unk_D722E4, MODEL_FLAME_A = game::unk_D7217C, MODEL_FLAME_B = game::unk_D72014;

		static Node24 *AddMain(uint32_t f) { return (Node24 *)AddTaskToQueue(QMain(), C(f)); }
		static inline uint32_t EndAt(int16_t c, int16_t limit) { return c >= limit ? TASK_END : 0; }

		struct PF { bool wf; int32_t fade; uint32_t flags; };
		static void ApplyPF(uint8_t *h, const PF &f, int flags_off)
		{
			if (f.wf) *(int32_t *)(h + 0xC) = f.fade;
			*(uint32_t *)(h + flags_off) = f.flags;
		}
		static PF GlowPF(int32_t c) { if (c == 0) return { true, 0x800, 0xF3 }; if (c >= 0xC) return { true, shl32(c - 0xC, 9), 0xF3 }; return { false, 0, 0x33 }; }
		static PF SpinPF(int32_t c) { if (c < 4) return { true, shl32(8 - c, 9) | 0xC0, 0x33 }; if (c >= 0x12) return { true, shl32(c - 0x12, 9), 0xF3 }; return { false, 0, 0x33 }; }
		static PF RingPF(int32_t c) { if (c >= 1) return { true, mul32(c - 1, 0x174), 0xF3 }; return { false, 0, 0x33 }; }
		static PF ArcPF(int32_t c) { if (c == 0) return { true, 0x800, 0xF3 }; if (c >= 0x10) return { true, shl32(c - 0x10, 9), 0xF3 }; return { false, 0, 0x33 }; }
		static PF ColumnFlashPF(int32_t c) { return { true, c < 8 ? shl32(0x10 - c, 8) : 0x800, 0xC3 }; }
		static PF PillarPF(int32_t c) { return { true, c >= 0 ? mul32(c, 0xCC) : 0, 0xF3 }; }
		static PF Ring2PF(int32_t c) { if (c >= 2) return { true, mul32(c - 2, 0x199), 0xF3 }; return { false, 0, 0x33 }; }
		static PF FlamePF(int32_t c) { if (c < 4) return { true, shl32(4 - c, 10), 0xF3 }; if (c >= 10) return { true, shl32(c - 10, 9), 0xF3 }; return { false, 0, 0x33 }; }

		static void BoneMatrix(uint32_t anim_header, int32_t bone, Mat4x3 *out)
		{
			const uint32_t sk = *(const uint32_t *)*(const uint32_t *)(anim_header + 4);
			uint32_t b = (uint32_t)bone;
			if (bone >= 0xF0)
			{
				b = *(const uint8_t *)(sk + bone - 0xEC);
				if (b & 0x80) b &= 0x7F;
			}
			ff8fx::copy<(0x20)>(out, (const void *)(sk + b * 0x30 + 0x20));
		}

		static void EffectPoint(int16_t *point)
		{
			const uint32_t ent = game_addr(game::BattleEntitySlotData) + 0x9C * (uint32_t)CasterSlot();
			Mat4x3 m;
			BoneMatrix(ent + 0x60, D::BONE, &m);
			ComposeAffineTransform((const Mat4x3 *)(ent + 0x40), &m, &m);
			MatVec(&m, (const void *)X(game_addr(BONE_OFFSET)), point);
			point[0] = (int16_t)(point[0] + (int16_t)m.t[0]);
			point[1] = (int16_t)(point[1] + (int16_t)m.t[1]);
			point[2] = (int16_t)(point[2] + (int16_t)m.t[2]);
		}
		static void FrameAt(const int16_t *point, Mat4x3 *frame)
		{
			int32_t v[3], axis[4];
			v[0] = Centre()[0] - point[0];
			v[1] = Centre()[1] - point[1];
			v[2] = Centre()[2] - point[2];
			Normalize(v, v);
			const int32_t angle = RotationBetweenVectors((const int32_t *)X(game_addr(REF_DIR)), v, axis);
			BuildAxisAngleRotationMatrix(angle, frame, axis);
			frame->t[1] = point[1];
			frame->t[0] = point[0];
			frame->t[2] = point[2];
		}

		static int32_t TargetIndex(int32_t slot)
		{
			const ActionData *a = Context()->actions;
			const int32_t n = a->target_count;
			for (int32_t i = 0; i < n; i++)
				if (slot == (int32_t)a->targets[i * TARGET_STRIDE]) return i;
			return -1;
		}

		static uint32_t __cdecl DirectorTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			EffectPoint(Point());
			FrameAt(Point(), Frame());
			if (DrawOnly()) return 0;
			if (n->c == 0)
			{
				CalculateCenterPosition(Context()->target_mask, Centre());
				FlashColour() = 0;
			}
			if (n->c == 0x2B) StageCapture(Centre());
			if (n->c == 1)
			{
				Node24 *t = AddMain(game_addr(ORIG_Glow));
				t->c = 0;
				t->w1C = 0x6800;
				t->w1E = 0x14CC;
			}
			if (n->c == 8)
			{
				Node24 *a = AddMain(game_addr(ORIG_Spin));
				a->c = 0;
				a->w18 = (int16_t)(CrtRand() % 0x1000);
				const int32_t r = CrtRand() % 0x28;
				a->w1C = 0x2000;
				a->w1A = (int16_t)(r + 0x14);
				Node24 *b = AddMain(game_addr(ORIG_Spin));
				b->c = 0;
				const int16_t ang = (int16_t)(CrtRand() % 0x1000);
				b->w18 = ang;
				b->w1A = (int16_t)-a->w18;
				b->w1C = 0x2000;
			}
			if (n->c >= 6 && n->c <= 0x1E && n->c % 6 == 2)
			{
				Node24 *t = AddMain(game_addr(ORIG_Ring));
				t->c = 0;
				t->w1E = 0x340;
				t->w1C = 0x340;
			}
			if (n->c == 8)
				for (int i = 0; i < 6; i++)
				{
					Node24 *t = AddMain(game_addr(ORIG_BoltDir));
					t->e = (int16_t)i;
					t->c = 0;
				}
			if (n->c == 0x2C)
			{
				Node24 *t = AddMain(game_addr(ORIG_ColumnFlash));
				*(uint32_t *)&t->w10 = *(const uint32_t *)Centre();
				t->c = 0;
				*(uint32_t *)&t->w14 = *(const uint32_t *)(Centre() + 2);
				t->w1C = 0;
				t->w1E = 0x200;
				if (n->c == 0x2C)
				{
					t = AddMain(game_addr(ORIG_Column));
					*(uint32_t *)&t->w10 = *(const uint32_t *)Centre();
					t->c = 0;
					*(uint32_t *)&t->w14 = *(const uint32_t *)(Centre() + 2);
					t->w1C = 0;
					t->w1E = 0x200;
				}
			}
			if (n->c == 6) ScreenFadeTask(2, 1, 0xA, 0xFF);
			if (n->c == 0x33) ScreenFadeTask(0, 2, 4, 0xFF);
			if (n->c == 0x36)
			{
				Node24 *t = AddMain(game_addr(ORIG_Pillar));
				*(uint32_t *)&t->w10 = *(const uint32_t *)Centre();
				*(uint32_t *)&t->w14 = *(const uint32_t *)(Centre() + 2);
				t->c = 0;
				t->w1C = 0x1000;
				t->w22 = 0x555;
				t->w20 = 0x555;
				if (n->c == 0x36)
					for (int i = 0; i < 0x10; i++)
					{
						Node24 *f = AddMain(game_addr(ORIG_Flame));
						f->c = 0;
						f->e = (int16_t)(CrtRand() % 4 + i);
						*(uint32_t *)&f->w10 = *(const uint32_t *)Centre();
						*(uint32_t *)&f->w14 = *(const uint32_t *)(Centre() + 2);
						f->w10 = (int16_t)(f->w10 + (int16_t)(CrtRand() % 0x15E0 - 0xAF0));
						f->w14 = (int16_t)(f->w14 + (int16_t)(CrtRand() % 0xAF0 - 0x578));
						const int32_t r = CrtRand() % 0x1000;
						f->w18 = (int16_t)r;
						f->w1A = (int16_t)(r & 1);
						f->w20 = 0x4000;
						f->w1C = (int16_t)(CrtRand() % 0x1800 + 0x1C00);
						f->w22 = (int16_t)(CrtRand() % 10 + 6);
					}
			}
			if (n->c == 0x37)
			{
				Node24 *t = AddMain(game_addr(ORIG_Sparks));
				*(uint32_t *)&t->w10 = *(const uint32_t *)Centre();
				*(uint32_t *)&t->w14 = *(const uint32_t *)(Centre() + 2);
				t->c = 0;
				if (n->c == 0x37)
				{
					t = AddMain(game_addr(ORIG_Debris));
					*(uint32_t *)&t->w10 = *(const uint32_t *)Centre();
					t->c = 0;
					*(uint32_t *)&t->w14 = *(const uint32_t *)(Centre() + 2);
				}
			}
			if (n->c == 0x2C)
				for (int32_t slot = 0; slot < 3; slot++)
				{
					if (!(*Entity(slot) & 2)) continue;
					Node24 *t = AddMain(game_addr(ORIG_Knock));
					t->e = (int16_t)slot;
					const int32_t idx = TargetIndex(slot);
					t->c = 0;
					t->w20 = 0x1F4;
					t->w1A = (int16_t)idx;
					if (idx < 0) t->w20 = 0xFA;
				}
			if (n->c == 0x36)
				for (int32_t slot = 0; slot < 3; slot++)
				{
					if (!(*Entity(slot) & 2)) continue;
					Node24 *t = AddMain(game_addr(ORIG_Throw));
					t->c = 0;
					t->e = (int16_t)slot;
					t->w18 = (int16_t)((CrtRand() % 3 + 1) << 12);
					const int16_t h = (int16_t)(-0xBB8 - CrtRand() % 0x5DC);
					t->w20 = h;
					const int32_t idx = TargetIndex(slot);
					t->w1A = (int16_t)idx;
					if (idx < 0) t->w20 = (int16_t)(h >> 1);
				}
			const int16_t c = n->c;
			if (c < 8) SetScreenFlash((uint32_t)shl32(c, 8), 0);
			else if (c >= 0x1B && c < 0x23)
			{
				const int32_t ir0 = shl32(c - 0x1B, 12) / 8;
				GteSetFarColor2(0x30, 0x10, 0x10);
				FlashColour() = 0;
				GteLoadRGBC(&FlashColour());
				GteSetIR0(ir0);
				GteDPCS();
				GteStoreRGB2(&FlashColour());
				SetScreenFlash((uint32_t)shl32(n->c - 0xB, 7), FlashColour());
			}
			else if (c >= 0x5A) SetScreenFlash((uint32_t)shl32((0x62 - c) * 3, 7), 0x101030);
			if (n->c == 1) BdPlaySE((const void *)X(game_addr(SOUND_START)), 0, 0x80);
			if (n->c == 0x30) BdPlaySE((const void *)X(game_addr(SOUND_BLAST)), 0, 0x80);
			n->c = (int16_t)(n->c + 1);
			return n->c > 0x62 ? TASK_END : 0;
		}

		static void StageCapture(const int16_t *centre)
		{
			int16_t rect[4] = { 0x180, 0x100, 0x100, 0x100 };
			int32_t ofx, ofy;
			int16_t angles[4];
			Mat4x3 m;
			int32_t q = (int32_t)(((int64_t)centre[0] * (int32_t)0xC71C71C7) >> 32) >> 3;
			m.t[0] = q + (int32_t)((uint32_t)q >> 31);
			q = (int32_t)(((int64_t)centre[2] * 0x2AAAAAAB) >> 32) >> 2;
			m.t[1] = q + (int32_t)((uint32_t)q >> 31);
			angles[0] = 0x400;
			angles[1] = 0;
			angles[2] = 0;
			m.t[2] = 0x7FFF;
			BuildRotationMatrixFromAngles(angles, &m);
			int32_t s[3];
			s[1] = 0x1000;
			s[0] = 0x1000000 / (0x2378000 / rect[2]);
			s[2] = 0x1000000 / (0x17A4000 / rect[3]);
			Scale3DMatrix(&m, s);
			const uint32_t env = game_addr(game::g_Battle_DrawEnv) + 0x5C * (uint32_t)(var<uint8_t>(game_addr(game::g_Battle_DisplayParity)) ^ 1);
			SetDrawEnvPrim(G(game_addr(game::unk_23C0500)), env);
			InsertPrimAutoDepth(OT0() + 0x44, (void *)G(game_addr(game::unk_23C0500)));
			GteReadScreenOffset(&ofx, &ofy);
			GteSetScreenOffset(rect[2] / 2, rect[3] / 2);
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
			for (uint32_t g = game_addr(game::unk_1D989BD); g >= game_addr(game::unk_1D98991); g -= 0x2C)
			{
				const uint8_t fl = *(const uint8_t *)g;
				if (!(fl & 1) || !(fl & 2)) continue;
				*(uint32_t *)(h + 0x1C) = *(const uint32_t *)(g + 0x27);
				*(uint16_t *)(h + 0x24) = *(const uint16_t *)(g + 1);
				BuildBoneMatricesFromPose((void *)(g + 0x13));
				ComputeBonesWorldMatrices((void *)(g + 0x13), &m);
				PacketCursor() = RenderGeometry(g + 3, h, OT0() + 0x44, 0x10, PacketCursor());
			}
			SetProjectionH(var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)));
			FieldFree(0x2C);
			GteSetScreenOffset(ofx, ofy);
			InsertPrimAltViewport(OT0() + 0x44, X(game_addr(ALT_VIEWPORT)));
			SetDrawAreaPrim(G(game_addr(game::unk_23BF040)), rect);
			InsertPrimAutoDepth(OT0() + 0x44, (void *)G(game_addr(game::unk_23BF040)));
			var<int16_t>(G(game_addr(game::unk_23C326C))) = rect[0];
			var<int16_t>(G(game_addr(game::unk_23C326E))) = rect[1];
			SetDrawOffsetPrim(G(game_addr(game::unk_23C04D0)), (const void *)G(game_addr(game::unk_23C326C)));
			InsertPrimAutoDepth(OT0() + 0x44, (void *)G(game_addr(game::unk_23C04D0)));
		}

		static uint8_t *PrimHeader(uint32_t model, int size)
		{
			uint8_t *h = AllocHeader(size);
			*(uint32_t *)h = model;
			*(uint32_t *)(h + 8) = 0;
			return h;
		}
		static void DrawPrim(uint8_t *h, int32_t mode, int size)
		{
			PacketCursor() = RenderPrimModel(h, OT0() + 0x44, mode, PacketCursor());
			FieldFree(size);
		}

		static void GlowDraw(const Node24 *n, const Mat4x3 *frame, const PF &f)
		{
			Mat4x3 m = *frame;
			int32_t s[3] = { n->w1C, n->w1C, n->w1C };
			Scale3DMatrix(&m, s);
			ComposeAffineTransform(&Camera(), &m, &m);
			GteSetRotMatrix(&m);
			GteSetTransVector(&m);
			uint8_t *h = PrimHeader(X(game_addr(MODEL_GLOW)), 0x58);
			ApplyPF(h, f, 0x1C);
			DrawPrim(h, D::RMODE, 0x58);
		}
		static uint32_t __cdecl GlowTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			GlowDraw(n, Frame(), GlowPF(n->c));
			if (DrawOnly()) return 0;
			const int16_t c = n->c;
			if (c < 8)
			{
				const int16_t r = n->w1E;
				n->w1C = (int16_t)(n->w1C - r);
				n->w1E = (int16_t)(r - r / 6);
			}
			n->c = (int16_t)(c + 1);
			return EndAt(n->c, 0x14);
		}

		static void SpinDraw(const Node24 *n, const Mat4x3 *frame, const PF &f)
		{
			int16_t ang[4] = { 0, 0, n->w18, 0 };
			Mat4x3 m;
			ComposeZYXRotationMatrix(ang, &m);
			GteMatrixMultiply(frame, &m);
			m.t[2] = frame->t[2];
			m.t[1] = frame->t[1];
			m.t[0] = frame->t[0];
			int32_t s[3] = { n->w1C, n->w1C, n->w1C };
			Scale3DMatrix(&m, s);
			ComposeAffineTransform(&Camera(), &m, &m);
			GteSetRotMatrix(&m);
			GteSetTransVector(&m);
			uint8_t *h = PrimHeader(X(game_addr(MODEL_SPIN)), 0x58);
			ApplyPF(h, f, 0x1C);
			DrawPrim(h, D::RMODE, 0x58);
		}
		static uint32_t __cdecl SpinTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			SpinDraw(n, Frame(), SpinPF(n->c));
			if (DrawOnly()) return 0;
			const int16_t r = n->w1A;
			n->w18 = (int16_t)(n->w18 + r);
			n->w1A = (int16_t)(r - r / 8);
			n->c = (int16_t)(n->c + 1);
			return EndAt(n->c, 0x1A);
		}

		static void RingDraw(const Node24 *n, const Mat4x3 *frame, const PF &f)
		{
			Mat4x3 m = *frame;
			int32_t s[3] = { n->w1C, n->w1C, n->w1C };
			Scale3DMatrix(&m, s);
			ComposeAffineTransform(&Camera(), &m, &m);
			GteSetRotMatrix(&m);
			GteSetTransVector(&m);
			uint8_t *h = PrimHeader(X(game_addr(MODEL_RING)), 0x58);
			ApplyPF(h, f, 0x1C);
			DrawPrim(h, D::RMODE, 0x58);
		}
		static uint32_t __cdecl RingTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			RingDraw(n, Frame(), RingPF(n->c));
			if (DrawOnly()) return 0;
			const int16_t r = n->w1E;
			n->w1C = (int16_t)(n->w1C + r);
			n->w1E = (int16_t)(r - r / 6);
			n->c = (int16_t)(n->c + 1);
			return EndAt(n->c, 0xC);
		}

		static uint32_t __cdecl BoltDirTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			const uint32_t rec = Bolt(n->e);
			if (DrawOnly()) return 0;
			if (n->c == 0)
			{
				const uint32_t c01 = *(const uint32_t *)Centre(), c23 = *(const uint32_t *)(Centre() + 2);
				At<uint32_t>(rec, 8) = c01;
				At<uint32_t>(rec, 0xC) = c23;
				At<uint32_t>(rec, 0) = c01;
				At<uint32_t>(rec, 4) = c23;
				At<int16_t>(rec, 0) = (int16_t)(At<int16_t>(rec, 0) + (int16_t)(CrtRand() % 0x1CE8 - 0xE74));
				At<int16_t>(rec, 4) = (int16_t)(At<int16_t>(rec, 4) + (int16_t)(CrtRand() % 0x1CE8 - 0xE74));
				At<int16_t>(rec, 8) = (int16_t)(At<int16_t>(rec, 8) + (int16_t)(CrtRand() % 0x1CE8 - 0xE74));
				At<int16_t>(rec, 0xC) = (int16_t)(At<int16_t>(rec, 0xC) + (int16_t)(CrtRand() % 0x1CE8 - 0xE74));
				const int32_t dx = At<int16_t>(rec, 8) - At<int16_t>(rec, 0);
				const int32_t dz = At<int16_t>(rec, 0xC) - At<int16_t>(rec, 4);
				const int32_t len = Sqrt(mul32(dz, dz) + mul32(dx, dx));
				At<uint32_t>(rec, 0x10) = At<uint32_t>(rec, 0);
				At<uint32_t>(rec, 0x14) = At<uint32_t>(rec, 4);
				At<uint32_t>(rec, 0x30) = At<uint32_t>(rec, 8);
				At<uint32_t>(rec, 0x34) = At<uint32_t>(rec, 0xC);
				At<int16_t>(rec, 0x2A) = 0;
				At<int16_t>(rec, 0x22) = 0;
				At<int16_t>(rec, 0x1A) = 0;
				At<int16_t>(rec, 0x20) = (int16_t)((At<int16_t>(rec, 0) + At<int16_t>(rec, 8)) >> 1);
				At<int16_t>(rec, 0x24) = (int16_t)((At<int16_t>(rec, 4) + At<int16_t>(rec, 0xC)) >> 1);
				At<int16_t>(rec, 0x18) = (int16_t)((At<int16_t>(rec, 0) + At<int16_t>(rec, 0x20)) >> 1);
				At<int16_t>(rec, 0x1C) = (int16_t)((At<int16_t>(rec, 4) + At<int16_t>(rec, 0x24)) >> 1);
				At<int16_t>(rec, 0x28) = (int16_t)((At<int16_t>(rec, 0x20) + At<int16_t>(rec, 8)) >> 1);
				At<int16_t>(rec, 0x2C) = (int16_t)((At<int16_t>(rec, 0xC) + At<int16_t>(rec, 0x24)) >> 1);
				const int32_t two = len + len;
				static const int offs[6] = { 0x18, 0x1C, 0x20, 0x24, 0x28, 0x2C };
				for (int k = 0; k < 6; k++)
					At<int16_t>(rec, offs[k]) = (int16_t)(At<int16_t>(rec, offs[k]) + (int16_t)(CrtRand() % two - len));
				void *work = FieldAlloc(0x190);
				SplineSetup(5, (const void *)(rec + 0x10), work);
				for (int32_t i = 0; i < 0x30; i++)
					SplineEval(5, work, (void *)(rec + 0x38 + 8 * i), shl32(i, 12) / 0x2F);
				FieldFree(0x190);
				Node24 *t = AddMain(game_addr(ORIG_Arc));
				t->c = 0;
				t->e = n->e;
				t->w18 = 0;
				t->w1A = 2;
				t->w1C = (int16_t)(CrtRand() % 0x200 + 0x200);
				t = AddMain(game_addr(ORIG_Sheet));
				t->c = 0;
				t->e = n->e;
				t->w18 = 0;
				t->w1A = 2;
				for (int k = 0; k < 3; k++) At<uint32_t>(rec, 0x1B8 + 0x14 * k) = 0;
				t = AddMain(game_addr(ORIG_Ribbon));
				t->w1A = 2;
				t->e = n->e;
				t->c = 0;
				t->w18 = 0;
			}
			n->c = (int16_t)(n->c + 1);
			return EndAt(n->c, 0x10);
		}

		static void ArcDraw(const Node24 *n, const int16_t *point, const int16_t *target, const PF &f)
		{
			int32_t v[3], axis[4];
			v[0] = target[0] - point[0];
			v[1] = target[1] - point[1];
			v[2] = target[2] - point[2];
			const int32_t len = Sqrt(Normalize(v, v));
			const int32_t angle = RotationBetweenVectors((const int32_t *)X(game_addr(REF_DIR)), v, axis);
			Mat4x3 r, m;
			BuildAxisAngleRotationMatrix(angle, &r, axis);
			int16_t ang[4] = { 0, 0, 0, 0 };
			ComposeZYXRotationMatrix(ang, &m);
			m.t[1] = point[1];
			m.t[0] = point[0];
			int32_t s[3];
			s[1] = n->w1C;
			s[0] = n->w1C;
			const int32_t q = (int32_t)(((int64_t)shl32(len, 12) * 0x61FF6D01) >> 32) >> 11;
			m.t[2] = point[2];
			s[2] = q + (int32_t)((uint32_t)q >> 31);
			Scale3DMatrix(&m, s);
			GteMatrixMultiply(&r, &m);
			ComposeAffineTransform(&Camera(), &m, &m);
			GteSetRotMatrix(&m);
			GteSetTransVector(&m);
			RenderHeader *h = (RenderHeader *)AllocHeader(0x6C);
			h->model = X(game_addr(MODEL_ARC));
			*(uint32_t *)h->far_rgb = 0;
			h->colour = 0x30307F;
			ApplyPF((uint8_t *)h, f, 0x1C);
			PacketCursor() = rm::RenderModel(h, OT0() + 0x44, 2, PacketCursor());
			FieldFree(0x6C);
		}
		static uint32_t __cdecl ArcTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			ArcDraw(n, Point(), (const int16_t *)(Bolt(n->e) + 0x38 + 8 * n->w18), ArcPF(n->c));
			if (DrawOnly()) return 0;
			const int16_t s = n->w1A;
			n->w18 = (int16_t)(n->w18 + s);
			if (s > 2) n->w1A = (int16_t)(s - 1);
			n->c = (int16_t)(n->c + 1);
			return n->w18 >= 0x30 ? TASK_END : 0;
		}

		static void SheetHistory(const Node24 *n, uint32_t rec, const int16_t *point)
		{
			const int32_t j = n->c % 3;
			At<uint32_t>(rec, 0x1B8 + 0x14 * j) = 1;
			At<uint32_t>(rec, 0x1BC + 0x14 * j) = *(const uint32_t *)point;
			At<uint32_t>(rec, 0x1C0 + 0x14 * j) = *(const uint32_t *)(point + 2);
			const uint32_t sp = rec + 0x38 + 8 * n->w18;
			At<uint32_t>(rec, 0x1C4 + 0x14 * j) = At<uint32_t>(sp, 0);
			At<uint32_t>(rec, 0x1C8 + 0x14 * j) = At<uint32_t>(sp, 4);
		}
		static int32_t SheetGather(const Node24 *n, uint32_t rec)
		{
			int32_t j = n->c % 3;
			const int32_t lim = n->c >= 0x15 ? 0x18 - n->c : 3;
			int32_t k = 0;
			for (; k < lim; k++)
			{
				if (At<uint32_t>(rec, 0x1B8 + 0x14 * j) == 0) break;
				At<uint32_t>(rec, 0x1F4 + 8 * k) = At<uint32_t>(rec, 0x1BC + 0x14 * j);
				At<uint32_t>(rec, 0x1F8 + 8 * k) = At<uint32_t>(rec, 0x1C0 + 0x14 * j);
				At<uint32_t>(rec, 0x214 + 8 * k) = At<uint32_t>(rec, 0x1C4 + 0x14 * j);
				At<uint32_t>(rec, 0x218 + 8 * k) = At<uint32_t>(rec, 0x1C8 + 0x14 * j);
				if (--j < 0) j = 2;
			}
			return k;
		}
		static void SheetCurves(uint32_t rec, int32_t k)
		{
			void *work = FieldAlloc(0x190);
			SplineSetup(k, (const void *)(rec + 0x1F4), work);
			for (int32_t i = 0; i < 0x10; i++) SplineEval(k, work, (void *)(rec + 0x234 + 8 * i), shl32(i, 12) / 15);
			SplineSetup(k, (const void *)(rec + 0x214), work);
			for (int32_t i = 0; i < 0x10; i++) SplineEval(k, work, (void *)(rec + 0x2B4 + 8 * i), shl32(i, 12) / 15);
			FieldFree(0x190);
		}
		static uint32_t SheetColour(int32_t c) { return *(const uint32_t *)(X(game_addr(SHEET_COLOURS)) + 4 * (c % 12)); }
		static void SheetDraw(uint32_t colour, uint32_t rec, uint8_t *blk)
		{
			GteSetRotMatrix(&Camera());
			GteSetTransVector(&Camera());
			GteSetFarColor2(0, 0, 0);
			uint32_t a = PacketCursor() + 4, b = PacketCursor() + 0x24;
			const uint32_t col = colour | 0x3A000000u;
			At<uint32_t>((uint32_t)blk, 0x14) = col;
			At<uint32_t>((uint32_t)blk, 0x1C) = col;
			uint32_t v = rec + 0x23C;
			for (int32_t t = 0; t < 0xE000; t += 0x1000, v += 8)
			{
				At<uint32_t>(a, -4) = 0x8000000;
				At<uint32_t>((uint32_t)blk, 0x10) = At<uint32_t>((uint32_t)blk, 0x14);
				GteLoadV012((const void *)(v - 8), (const void *)v, (const void *)(v + 0x78));
				GteRTPT();
				GteReadFLAG(blk + 0xC);
				if (At<uint32_t>((uint32_t)blk, 0xC) & 0x60000) continue;
				GteReadSXY012Split((void *)(a + 4), (void *)(a + 0xC), (void *)(a + 0x14));
				GteLoadV0((const void *)(v + 0x80));
				GteRTPS();
				GteReadSXY2((void *)(a + 0x1C));
				GteAVSZ4();
				GteStoreOTZ(blk + 8);
				GteSetIR0(t / 14);
				GteLoadRGBC(blk + 0x1C);
				GteDPCS();
				GteStoreRGB2(blk + 0x14);
				const uint32_t c0 = At<uint32_t>((uint32_t)blk, 0x10);
				At<uint32_t>(a, 0x10) = c0;
				At<uint32_t>(a, 0) = c0;
				const uint32_t c1 = At<uint32_t>((uint32_t)blk, 0x14);
				At<uint32_t>(a, 0x18) = c1;
				At<uint32_t>(a, 8) = c1;
				InsertPrimAutoDepth(OT0() + (uint32_t)(At<int32_t>((uint32_t)blk, 8) >> 2) * 4 + 0x44, (void *)(a - 4));
				At<uint32_t>(b, 0xC) = 0x3A000000;
				At<uint32_t>(b, 4) = 0x3A000000;
				At<uint32_t>(b, 0x1C) = At<uint32_t>((uint32_t)blk, 0x14);
				At<uint32_t>(b, 0x14) = At<uint32_t>((uint32_t)blk, 0x10);
				At<uint32_t>(b, 0x10) = At<uint32_t>(a, 0xC);
				At<uint32_t>(b, 8) = At<uint32_t>(a, 4);
				At<uint32_t>(b, 0x18) = At<uint32_t>(a, 0x14);
				At<uint32_t>(b, 0x20) = At<uint32_t>(a, 0x1C);
				At<uint32_t>(b, 0) = 0x8000000;
				InsertPrimAutoDepth(OT0() + (uint32_t)(At<int32_t>((uint32_t)blk, 8) >> 2) * 4 + 0x44, (void *)b);
				a += 0x48;
				b += 0x48;
			}
			PacketCursor() = b;
		}
		static uint32_t __cdecl SheetTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			const uint32_t rec = Bolt(n->e);
			uint8_t *blk = (uint8_t *)FieldAlloc(0x40);
			SheetHistory(n, rec, Point());
			const int32_t k = SheetGather(n, rec);
			if (k > 1)
			{
				SheetCurves(rec, k);
				SheetDraw(SheetColour(n->c), rec, blk);
			}
			FieldFree(0x40);
			if (DrawOnly()) return 0;
			const int16_t s = n->w1A;
			n->w18 = (int16_t)(n->w18 + s);
			if (s > 2) n->w1A = (int16_t)(s - 1);
			n->c = (int16_t)(n->c + 1);
			return n->w18 >= 0x30 ? TASK_END : 0;
		}

		static void RibbonTable(uint32_t rec, uint8_t *blk, int32_t cnt)
		{
			const uint32_t w = (uint32_t)blk;
			ff8fx::copy<(0x10)>(blk + 0x40, (const void *)X(game_addr(REF_DIR)));
			At<int16_t>(w, 0x30) = 0x5A;
			At<int16_t>(w, 0x32) = 0;
			At<int16_t>(w, 0x34) = 0;
			for (int32_t j = 0; j < cnt; j++)
			{
				if (j != 0)
				{
					At<uint32_t>(w, 0x20) = At<uint32_t>(rec, 0x38 + 8 * j);
					At<uint32_t>(w, 0x28) = At<uint32_t>(rec, 0x30 + 8 * j);
					At<uint32_t>(w, 0x24) = At<uint32_t>(rec, 0x3C + 8 * j);
					At<uint32_t>(w, 0x2C) = At<uint32_t>(rec, 0x34 + 8 * j);
					At<int32_t>(w, 0x50) = At<int16_t>(w, 0x20) - At<int16_t>(w, 0x28);
					At<int32_t>(w, 0x58) = At<int16_t>(w, 0x24) - At<int16_t>(w, 0x2C);
				}
				else
				{
					At<uint32_t>(w, 0x20) = At<uint32_t>(rec, 0x38);
					At<uint32_t>(w, 0x28) = At<uint32_t>(rec, 0x40);
					At<uint32_t>(w, 0x24) = At<uint32_t>(rec, 0x3C);
					At<uint32_t>(w, 0x2C) = At<uint32_t>(rec, 0x44);
					At<int32_t>(w, 0x50) = At<int16_t>(w, 0x28) - At<int16_t>(w, 0x20);
					At<int32_t>(w, 0x58) = At<int16_t>(w, 0x2C) - At<int16_t>(w, 0x24);
				}
				At<int32_t>(w, 0x54) = 0;
				Normalize((const int32_t *)(w + 0x50), (int32_t *)(w + 0x50));
				const int32_t angle = RotationBetweenVectors((const int32_t *)(w + 0x40), (const int32_t *)(w + 0x50), (int32_t *)(w + 0x60));
				BuildAxisAngleRotationMatrix(angle, (Mat4x3 *)(w + 0x70), (const int32_t *)(w + 0x60));
				MatVec((const Mat4x3 *)(w + 0x70), (const void *)(w + 0x30), (void *)(w + 0x38));
				const uint32_t t = G(game_addr(RIBBON_TABLE)) + 0x18 * j;
				At<uint32_t>(t, 0) = At<uint32_t>(w, 0x20);
				At<uint32_t>(t, 4) = At<uint32_t>(w, 0x24);
				At<int16_t>(t, 8) = (int16_t)(At<int16_t>(w, 0x38) + At<int16_t>(w, 0x20));
				At<int16_t>(t, 0x10) = (int16_t)(At<int16_t>(w, 0x20) - At<int16_t>(w, 0x38));
				At<int16_t>(t, 0xC) = (int16_t)(At<int16_t>(w, 0x3C) + At<int16_t>(w, 0x24));
				At<int16_t>(t, 0x14) = (int16_t)(At<int16_t>(w, 0x24) - At<int16_t>(w, 0x3C));
				At<int16_t>(t, 0x12) = 0;
				At<int16_t>(t, 0xA) = 0;
			}
		}
		static void RibbonDraw(bool fade, int32_t ir0, uint8_t *blk, int32_t cnt)
		{
			const uint32_t w = (uint32_t)blk;
			GteSetRotMatrix(&Camera());
			GteSetTransVector(&Camera());
			GteSetFarColor2(0, 0, 0);
			uint32_t pa = PacketCursor();
			uint32_t pb = PacketCursor() + 0x24;
			At<uint32_t>(w, 0x10) = 0x3A202080;
			At<uint32_t>(w, 0x14) = 0x3A040430;
			At<uint32_t>(w, 0x18) = 0x3A04040E;
			At<uint32_t>(w, 0x1C) = 0x3A000000;
			if (fade)
			{
				GteLoadRGB012(blk + 0x10, blk + 0x14, blk + 0x18);
				GteSetIR0(ir0);
				GteDPCT();
				GteStoreRGB012(blk + 0x10, blk + 0x14, blk + 0x18);
				At<uint32_t>(w, 0x10) |= 0x3A000000;
				At<uint32_t>(w, 0x14) |= 0x3A000000;
			}
			const int32_t segs = cnt - 1;
			const uint32_t T = G(game_addr(RIBBON_TABLE));
			for (int32_t k = 0; k < segs; k++)
			{
				const uint32_t tk = T + 0x18 * k;
				const uint32_t a10 = pa + 0x10;
				At<uint32_t>(pb, 0) = 0x8000000;
				At<uint32_t>(pa, 0) = 0x8000000;
				GteLoadV012((const void *)(tk + 0x18), (const void *)(tk + 8), (const void *)(tk + 0x10));
				GteRTPT();
				GteReadFLAG(blk + 0xC);
				if (At<uint32_t>(w, 0xC) & 0x60000) continue;
				GteReadSXY012Split((void *)a10, (void *)(a10 + 8), (void *)(pb + 0x20));
				At<uint32_t>(pb, 8) = At<uint32_t>(a10, 0);
				GteAVSZ3();
				GteStoreOTZ(blk + 8);
				GteLoadV012((const void *)tk, (const void *)(tk + 0x20), (const void *)(tk + 0x28));
				GteRTPT();
				GteReadSXY012Split((void *)(a10 + 0x10), (void *)(a10 - 8), (void *)(pb + 0x10));
				At<uint32_t>(pb, 0x18) = At<uint32_t>(a10, 0x10);
				if (k == 0)
				{
					At<uint32_t>(pb, 0xC) = At<uint32_t>(w, 0x14); At<uint32_t>(a10, -0xC) = At<uint32_t>(w, 0x14);
					At<uint32_t>(pb, 0x1C) = At<uint32_t>(w, 0x1C); At<uint32_t>(a10, 4) = At<uint32_t>(w, 0x1C);
					At<uint32_t>(pb, 4) = At<uint32_t>(w, 0x10); At<uint32_t>(a10, -4) = At<uint32_t>(w, 0x10);
					At<uint32_t>(pb, 0x14) = At<uint32_t>(w, 0x18); At<uint32_t>(a10, 0xC) = At<uint32_t>(w, 0x18);
				}
				else if (k == segs - 1)
				{
					At<uint32_t>(pb, 0xC) = At<uint32_t>(w, 0x1C); At<uint32_t>(a10, -0xC) = At<uint32_t>(w, 0x1C);
					At<uint32_t>(pb, 0x1C) = At<uint32_t>(w, 0x14); At<uint32_t>(a10, 4) = At<uint32_t>(w, 0x14);
					At<uint32_t>(pb, 4) = At<uint32_t>(w, 0x18); At<uint32_t>(a10, -4) = At<uint32_t>(w, 0x18);
					At<uint32_t>(pb, 0x14) = At<uint32_t>(w, 0x10); At<uint32_t>(a10, 0xC) = At<uint32_t>(w, 0x10);
				}
				else
				{
					const uint32_t c14 = At<uint32_t>(w, 0x14), c10 = At<uint32_t>(w, 0x10);
					At<uint32_t>(a10, 4) = c14;
					At<uint32_t>(a10, -0xC) = c14;
					At<uint32_t>(a10, 0xC) = c10;
					At<uint32_t>(a10, -4) = c10;
					At<uint32_t>(pb, 0x14) = c10;
					At<uint32_t>(pb, 4) = c10;
					At<uint32_t>(pb, 0x1C) = c14;
					At<uint32_t>(pb, 0xC) = c14;
				}
				InsertPrimAutoDepth(OT0() + (uint32_t)(At<int32_t>(w, 8) >> 2) * 4 + 0x44, (void *)pa);
				InsertPrimAutoDepth(OT0() + (uint32_t)(At<int32_t>(w, 8) >> 2) * 4 + 0x44, (void *)pb);
				pa += 0x48;
				pb += 0x48;
			}
			PacketCursor() = pb;
		}
		static int32_t RibbonFade(int32_t c) { return mul32(c - 0x18, 0x155); }
		static uint32_t __cdecl RibbonTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			const uint32_t rec = Bolt(n->e);
			uint8_t *blk = (uint8_t *)FieldAlloc(0x90);
			const int32_t cnt = n->w18 + 1;
			if (cnt >= 2)
			{
				RibbonTable(rec, blk, cnt);
				RibbonDraw(n->c >= 0x18, RibbonFade(n->c), blk, cnt);
			}
			FieldFree(0x90);
			if (DrawOnly()) return 0;
			const int16_t s = n->w1A;
			n->w18 = (int16_t)(n->w18 + s);
			if (n->w18 >= 0x30) n->w18 = 0x2F;
			if (s > 2) n->w1A = (int16_t)(s - 1);
			n->c = (int16_t)(n->c + 1);
			return EndAt(n->c, 0x24);
		}

		static void CentreMatrix(const Node24 *n, int16_t ax, int16_t ay, int32_t sx, int32_t sy, int32_t sz)
		{
			int16_t ang[4] = { ax, ay, 0, 0 };
			Mat4x3 m;
			ComposeZYXRotationMatrix(ang, &m);
			m.t[1] = n->w12;
			m.t[0] = n->w10;
			int32_t s[3];
			s[2] = sz;
			s[0] = sx;
			m.t[2] = n->w14;
			s[1] = sy;
			Scale3DMatrix(&m, s);
			ComposeAffineTransform(&Camera(), &m, &m);
			GteSetRotMatrix(&m);
			GteSetTransVector(&m);
		}
		static inline uint32_t StepScale(Node24 *n, int16_t limit)
		{
			const int16_t c = n->c;
			if (c >= 0 && c < 8) n->w1C = (int16_t)(n->w1C + n->w1E);
			n->c = (int16_t)(c + 1);
			return EndAt(n->c, limit);
		}

		static void ColumnDraw(const Node24 *n, int16_t sy)
		{
			CentreMatrix(n, 0, 0, 0x1000, sy, 0x1000);
			uint8_t *h = AllocHeader(0x60);
			*(uint32_t *)h = X(game_addr(MODEL_COLUMN));
			*(uint32_t *)(h + 8) = 0;
			*(uint32_t *)(h + 0x1C) = 0;
			*(uint32_t *)(h + 0x58) = 0xE1000020;
			*(uint32_t *)(h + 0x5C) = 0xE1000220;
			PacketCursor() = ColumnRender(h, OT0() + 0x44, 2, PacketCursor());
			FieldFree(0x60);
		}
		static uint32_t __cdecl ColumnTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			ColumnDraw(n, n->w1C);
			if (DrawOnly()) return 0;
			return StepScale(n, 8);
		}

		static void ColumnFlashDraw(const Node24 *n, int16_t sy, const PF &f)
		{
			CentreMatrix(n, 0, 0, 0x1000, sy, 0x1000);
			uint8_t *h = AllocHeader(0x58);
			*(uint32_t *)h = X(game_addr(MODEL_COLUMN_FLASH));
			*(uint32_t *)(h + 8) = 0;
			ApplyPF(h, f, 0x1C);
			DrawPrim(h, 2, 0x58);
		}
		static uint32_t __cdecl ColumnFlashTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			ColumnFlashDraw(n, n->w1C, ColumnFlashPF(n->c));
			if (DrawOnly()) return 0;
			return StepScale(n, 8);
		}

		static void PillarDraw(const Node24 *n, int16_t sx, int16_t sy, const PF &f)
		{
			CentreMatrix(n, 0, 0, sx, sy, sx);
			uint8_t *h = PrimHeader(X(game_addr(MODEL_PILLAR)), 0x58);
			ApplyPF(h, f, 0x1C);
			DrawPrim(h, 2, 0x58);
		}
		static uint32_t __cdecl PillarTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			PillarDraw(n, n->w1C, n->w20, PillarPF(n->c));
			if (DrawOnly()) return 0;
			const int16_t r = n->w22;
			n->w20 = (int16_t)(n->w20 + r);
			n->w22 = (int16_t)(r - r / 5);
			n->c = (int16_t)(n->c + 1);
			return EndAt(n->c, 0x14);
		}

		static void FlameDraw(const Node24 *n, int32_t scroll, const PF &f)
		{
			CentreMatrix(n, 0, n->w18, n->w1C, n->w20, n->w1C);
			uint8_t *h = AllocHeader(0x7C);
			*(int32_t *)(h + 0x14) = scroll;
			*(uint32_t *)(h + 8) = 0;
			*(uint32_t *)(h + 0x20) = 0x33;
			*(int16_t *)(h + 0x1A) = 0;
			*(int16_t *)(h + 0x1C) = 0x40;
			*(int16_t *)(h + 0x1E) = 0x80;
			*(uint32_t *)(h + 0x10) = 0;
			if (n->w1A != 0) { *(uint32_t *)h = X(game_addr(MODEL_FLAME_A)); *(int16_t *)(h + 0x18) = 0x40; }
			else { *(uint32_t *)h = X(game_addr(MODEL_FLAME_B)); *(int16_t *)(h + 0x18) = 0; }
			ApplyPF(h, f, 0x20);
			PacketCursor() = Mag151Render(h, OT0() + 0x44, 2, PacketCursor());
			FieldFree(0x7C);
		}
		static uint32_t __cdecl FlameTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			if (n->e > 0)
			{
				if (DrawOnly()) return 0;
				n->e = (int16_t)(n->e - 1);
				return 0;
			}
			FlameDraw(n, mul32(n->w22, n->c), FlamePF(n->c));
			if (DrawOnly()) return 0;
			n->c = (int16_t)(n->c + 1);
			return EndAt(n->c, 0x12);
		}

		static void Ring2Draw(const Node24 *n, int16_t s, const PF &f)
		{
			CentreMatrix(n, 0x400, 0, s, s, s);
			uint8_t *h = PrimHeader(X(game_addr(MODEL_RING)), 0x58);
			ApplyPF(h, f, 0x1C);
			DrawPrim(h, 2, 0x58);
		}
		static uint32_t __cdecl Ring2Task(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			Ring2Draw(n, n->w1C, Ring2PF(n->c));
			if (DrawOnly()) return 0;
			const int16_t r = n->w1E;
			n->w1C = (int16_t)(n->w1C + r);
			n->w1E = (int16_t)(r - r / 5);
			n->c = (int16_t)(n->c + 1);
			return EndAt(n->c, 0xC);
		}

		static int32_t FreeEntry(uint32_t pool, uint32_t end, uint32_t stride)
		{
			int32_t i = 0;
			for (uint32_t a = pool; *(const uint32_t *)a != 0; i++)
			{
				a += stride;
				if (a >= end) return -1;
			}
			return i >= 0x64 ? -1 : i;
		}

		static void SparksSpawn(const Node24 *n)
		{
			for (int k = 0; k < 2; k++)
			{
				const int32_t i = FreeEntry(G(game_addr(POOL_A)), G(game_addr(POOL_A_END)), 0x18);
				if (i < 0) return;
				const uint32_t e = G(game_addr(POOL_A)) + 0x18 * i;
				At<uint32_t>(e, 0) = 1;
				At<int16_t>(e, 4) = 0;
				const int32_t r = CrtRand() % 0xC00;
				At<int16_t>(e, 6) = (int16_t)(r + 0xA00);
				At<uint32_t>(e, 8) = *(const uint32_t *)&n->w10;
				At<uint32_t>(e, 0xC) = *(const uint32_t *)&n->w14;
				At<int16_t>(e, 8) = (int16_t)(At<int16_t>(e, 8) + (int16_t)(CrtRand() % 0x12C0 - 0x960));
				At<int16_t>(e, 0xA) = (int16_t)(At<int16_t>(e, 0xA) + (int16_t)(-0x190 - CrtRand() % 0x898));
				At<int16_t>(e, 0xC) = (int16_t)(At<int16_t>(e, 0xC) + (int16_t)(CrtRand() % 0x960 - 0x4B0));
			}
		}
		static int32_t SparksDraw(uint8_t *blk, uint32_t pool, uint32_t pool_end, bool update)
		{
			uint8_t *h = AllocHeader(0xB4);
			*(int16_t *)(h + 0x24) = 0;
			int32_t live = 0;
			ff8fx::copy<(0x20)>(blk + 8, &Camera());
			*(uint32_t *)h = X(game_addr(SEQ_SPARK));
			GteSetLightMatrix(blk + 8);
			GteSetBackColorFromTrans(blk + 8);
			for (uint32_t e = pool; e < pool_end; e += 0x18)
			{
				if (!(*(const uint8_t *)e & 1)) continue;
				GteLoadV0((const void *)(e + 8));
				GteMVMVA_LightV0Bk();
				GteSetRotScale(At<int16_t>(e, 6));
				*(int16_t *)(h + 4) = At<int16_t>(e, 4);
				GteTransFromMAC();
				PacketCursor() = InitEffectSequenceFromData(h, OT0() + 0x44, 2, PacketCursor());
				if (!update) continue;
				At<int16_t>(e, 4) = (int16_t)(At<int16_t>(e, 4) + 1);
				if (*(const int16_t *)(h + 0x28) < 0) At<uint32_t>(e, 0) = 0;
				else live++;
			}
			FieldFree(0xB4);
			return live;
		}
		static uint32_t __cdecl SparksTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			uint8_t *blk = (uint8_t *)FieldAlloc(0x48);
			if (!DrawOnly() && n->c <= 0x10) SparksSpawn(n);
			const int32_t live = SparksDraw(blk, G(game_addr(POOL_A)), G(game_addr(POOL_A_END)), !DrawOnly());
			FieldFree(0x48);
			if (DrawOnly()) return 0;
			n->c = (int16_t)(n->c + 1);
			return live != 0 ? 0 : TASK_END;
		}

		static void DebrisEntry(uint8_t *hdr, uint8_t *blk, uint32_t e, int16_t ax, int16_t az, int16_t s, int16_t x, int16_t y, int16_t z)
		{
			const uint32_t w = (uint32_t)blk;
			At<int16_t>(w, 0) = ax;
			*(uint32_t *)hdr = *(const uint32_t *)(X(game_addr(DEBRIS_MODELS)) + 4 * At<int32_t>(e, 8));
			At<int16_t>(w, 4) = az;
			ComposeZYXRotationMatrix((const int16_t *)w, (Mat4x3 *)(w + 8));
			At<int32_t>(w, 0x1C) = x;
			At<int32_t>(w, 0x24) = z;
			At<int32_t>(w, 0x20) = y;
			At<int32_t>(w, 0x50) = s;
			At<int32_t>(w, 0x4C) = s;
			At<int32_t>(w, 0x48) = s;
			Scale3DMatrix((Mat4x3 *)(w + 8), (const int32_t *)(w + 0x48));
			ComposeAffineTransform((const Mat4x3 *)(w + 0x28), (const Mat4x3 *)(w + 8), (Mat4x3 *)(w + 8));
			GteSetRotMatrix((const Mat4x3 *)(w + 8));
			GteSetTransVector((const Mat4x3 *)(w + 8));
			PacketCursor() = RenderPrimModel(hdr, OT0() + 0x44, 2, PacketCursor());
		}
		static void DebrisSpawn(const Node24 *n)
		{
			for (int k = 0; k < 3; k++)
			{
				const int32_t i = FreeEntry(G(game_addr(POOL_B)), G(game_addr(POOL_B_END)), 0x1C);
				if (i < 0) return;
				const uint32_t e = G(game_addr(POOL_B)) + 0x1C * i;
				At<uint32_t>(e, 0) = 2;
				At<int16_t>(e, 4) = 0;
				At<int16_t>(e, 6) = (int16_t)(CrtRand() % 0x3C0 + 0x280);
				const int32_t kind = CrtRand() % 3;
				At<int32_t>(e, 8) = kind;
				At<uint32_t>(e, 0xC) = *(const uint32_t *)&n->w10;
				At<uint32_t>(e, 0x10) = *(const uint32_t *)&n->w14;
				At<int16_t>(e, 0xC) = (int16_t)(At<int16_t>(e, 0xC) + (int16_t)(CrtRand() % 0x12C0 - 0x960));
				At<int16_t>(e, 0xE) = (int16_t)-(CrtRand() % 0x12C);
				At<int16_t>(e, 0x10) = (int16_t)(At<int16_t>(e, 0x10) + (int16_t)(CrtRand() % 0x960 - 0x4B0));
				At<int16_t>(e, 0x12) = (int16_t)(-0xB9 - CrtRand() % 0x186);
				int32_t v = CrtRand() % 0x78 + 0x3C;
				At<int16_t>(e, 0x14) = (int16_t)v;
				if (v & 1) At<int16_t>(e, 0x14) = (int16_t)-v;
				v = CrtRand() % 0x3C + 0x1E;
				At<int16_t>(e, 0x18) = (int16_t)v;
				if (v & 1) At<int16_t>(e, 0x18) = (int16_t)-v;
				const int16_t a = (int16_t)(CrtRand() % 0x1000);
				At<int16_t>(e, 0x16) = a;
				At<int16_t>(e, 0x1A) = (int16_t)(a >> 1);
			}
		}
		static uint32_t __cdecl DebrisTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			uint8_t *hdr = AllocHeader(0x58);
			uint8_t *blk = (uint8_t *)FieldAlloc(0x88);
			ff8fx::copy<(0x20)>(blk + 0x28, &Camera());
			*(uint32_t *)(hdr + 8) = 0;
			*(uint32_t *)(hdr + 0x1C) = 0;
			int32_t live = 0;
			*(int16_t *)(blk + 2) = 0;
			for (uint32_t e = G(game_addr(POOL_B)); e < G(game_addr(POOL_B_END)); e += 0x1C)
			{
				if (!(*(const uint8_t *)e & 2)) continue;
				DebrisEntry(hdr, blk, e, At<int16_t>(e, 0x16), At<int16_t>(e, 0x1A), At<int16_t>(e, 6), At<int16_t>(e, 0xC), At<int16_t>(e, 0xE), At<int16_t>(e, 0x10));
				if (DrawOnly()) continue;
				const int16_t life = (int16_t)(At<int16_t>(e, 4) + 1);
				At<int16_t>(e, 4) = life;
				if (life >= 0x10) { At<uint32_t>(e, 0) = 0; continue; }
				if (life >= 0xC)
				{
					const int16_t s = At<int16_t>(e, 6);
					At<int16_t>(e, 6) = (int16_t)(s - (int16_t)(s >> 3));
				}
				At<int16_t>(e, 0xE) = (int16_t)(At<int16_t>(e, 0xE) + At<int16_t>(e, 0x12));
				const int16_t vy = At<int16_t>(e, 0x12);
				At<int16_t>(e, 0x12) = (int16_t)(vy - (int16_t)(vy >> 3));
				At<int16_t>(e, 0x16) = (int16_t)(At<int16_t>(e, 0x16) + At<int16_t>(e, 0x14));
				At<int16_t>(e, 0x1A) = (int16_t)(At<int16_t>(e, 0x1A) + At<int16_t>(e, 0x18));
				live++;
			}
			FieldFree(0x88);
			FieldFree(0x58);
			if (DrawOnly()) return 0;
			if (n->c >= 0 && n->c <= 5) DebrisSpawn(n);
			n->c = (int16_t)(n->c + 1);
			if (n->c >= 4 && live == 0) return TASK_END;
			return 0;
		}

		static void SaveEntity(uint8_t *ent, uint32_t sv)
		{
			At<uint16_t>(sv, 0) = (uint16_t)(*(const uint16_t *)ent & 0x1000);
			At<uint32_t>(sv, 0xC) = *(const uint32_t *)(ent + 0x1C);
			At<uint32_t>(sv, 0x10) = *(const uint32_t *)(ent + 0x20);
			At<uint32_t>(sv, 4) = *(const uint32_t *)(ent + 0xC);
			At<uint32_t>(sv, 8) = *(const uint32_t *)(ent + 0x10);
		}
		static void RestoreEntity(uint8_t *ent, uint32_t sv)
		{
			*(uint16_t *)ent = (uint16_t)(At<uint16_t>(sv, 0) | (*(const uint16_t *)ent & 0xEFFF));
			*(uint32_t *)(ent + 0x1C) = At<uint32_t>(sv, 0xC);
			*(uint32_t *)(ent + 0x20) = At<uint32_t>(sv, 0x10);
			*(uint32_t *)(ent + 0xC) = At<uint32_t>(sv, 4);
			*(uint32_t *)(ent + 0x10) = At<uint32_t>(sv, 8);
		}

		static uint32_t __cdecl KnockTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			const int16_t c = n->c;
			const int32_t slot = n->e;
			uint8_t *ent = Entity(slot);
			const uint32_t sv = Save(slot);
			if (c == 0) SaveEntity(ent, sv);
			if (c < 8)
			{
				const int32_t v = mul32(n->w20, c) / 8;
				ent[1] |= 0x10;
				*(int16_t *)(ent + 0x1E) = (int16_t)(At<int16_t>(sv, 0xE) - (int16_t)v);
			}
			if (DrawOnly()) return 0;
			n->c = (int16_t)(c + 1);
			if (n->c < 9) return 0;
			RestoreEntity(ent, sv);
			return TASK_END;
		}

		static uint32_t __cdecl ThrowTask(TaskNode *tn)
		{
			Node24 *n = (Node24 *)tn;
			const int32_t slot = n->e;
			uint8_t *ent = Entity(slot);
			const uint32_t sv = Save(slot);
			if (n->c == 0)
			{
				GetDefaultEffectPosition(ent, &n->w10);
				SaveEntity(ent, sv);
			}
			const int16_t c = n->c;
			if (c <= 0x14)
			{
				const int32_t a = shl32(c, 11) / 20;
				ent[1] |= 0x10;
				const int32_t lift = mul32(ComputeSin(a), n->w20);
				*(int16_t *)(ent + 0xC) = (int16_t)(mul32(n->w18, n->c) / 20);
				*(int16_t *)(ent + 0xE) = (int16_t)(shl32(n->c, 13) / 20);
				int16_t d[4];
				d[0] = (int16_t)(n->w10 - At<int16_t>(sv, 0xC));
				d[1] = (int16_t)(n->w12 - At<int16_t>(sv, 0xE));
				d[2] = (int16_t)(n->w14 - At<int16_t>(sv, 0x10));
				Mat4x3 m;
				ComposeZYXRotationMatrix((const int16_t *)(ent + 0xC), &m);
				MatVec(&m, d, d);
				const int16_t x = (int16_t)(n->w10 - d[0]);
				int16_t y = (int16_t)(n->w12 - d[1]);
				const int16_t z = (int16_t)(n->w14 - d[2]);
				y = (int16_t)(y + (int16_t)(lift >> 12));
				*(int16_t *)(ent + 0x1C) = x;
				*(int16_t *)(ent + 0x1E) = y;
				*(int16_t *)(ent + 0x20) = z;
			}
			else if (c <= 0x1C)
			{
				const int32_t a = shl32(c - 0x14, 11) / 8;
				ent[1] |= 0x10;
				*(int16_t *)(ent + 0x1E) = (int16_t)(mul32(ComputeSin(a), n->w20 >> 3) >> 12);
			}
			if (DrawOnly()) return 0;
			if (n->c == 0x14)
			{
				Node24 *t = AddMain(game_addr(ORIG_Ring2));
				t->c = 0;
				t->w10 = *(const int16_t *)(ent + 0x1C);
				t->w12 = 0;
				t->w14 = *(const int16_t *)(ent + 0x20);
				const int32_t r = (CrtRand() % 0x400 + 0xC00) / 4;
				t->w1E = (int16_t)r;
				t->w1C = (int16_t)r;
				if (n->w1A >= 0) QueueChainTransformation(ent, 6);
			}
			n->c = (int16_t)(n->c + 1);
			if (n->c < 0x1C) return 0;
			const int16_t idx = n->w1A;
			if (idx >= 0 && **(const uint8_t **)(ent + 0x74) == 6) return 0;
			RestoreEntity(ent, sv);
			if (idx >= 0) ApplyActionResultToTarget(Context()->actions->targets + idx * TARGET_STRIDE);
			return TASK_END;
		}

		static uint32_t __cdecl RootTask(TaskNode *tn)
		{
			RootNode *n = (RootNode *)tn;
			if (n->c & 1) PacketCursor() = TexBase() + 0xC000;
			else PacketCursor() = TexBase();
			const int running = ExecuteTaskQueue(QMain());
			if (!running)
			{
				RootNode *t = (RootNode *)AddTaskToQueue(QRoot(), C(game_addr(ORIG_Restore)));
				t->c = 0;
				t->e = 0;
			}
			n->c = (int16_t)(n->c + 1);
			return running ? 0 : TASK_END;
		}
		static uint32_t __cdecl RestoreTask(TaskNode *tn)
		{
			RootNode *n = (RootNode *)tn;
			if (n->c == 1) TextureRestoreTask((void *)TexBase(), &n->e);
			const int16_t done = n->e;
			n->c = (int16_t)(n->c + 1);
			return done ? TASK_END : 0;
		}

		static void Register(const char *const names[19])
		{
			register_port(C(game_addr(ORIG_Root)), (void *)RootTask, names[0], D::EFFECT);
			register_port(C(game_addr(ORIG_Restore)), (void *)RestoreTask, names[1], D::EFFECT);
			register_port(C(game_addr(ORIG_Director)), (void *)DirectorTask, names[2], D::EFFECT);
			register_port(C(game_addr(ORIG_Glow)), (void *)GlowTask, names[3], D::EFFECT);
			register_port(C(game_addr(ORIG_Spin)), (void *)SpinTask, names[4], D::EFFECT);
			register_port(C(game_addr(ORIG_Ring)), (void *)RingTask, names[5], D::EFFECT);
			register_port(C(game_addr(ORIG_BoltDir)), (void *)BoltDirTask, names[6], D::EFFECT);
			register_port(C(game_addr(ORIG_Arc)), (void *)ArcTask, names[7], D::EFFECT);
			register_port(C(game_addr(ORIG_Sheet)), (void *)SheetTask, names[8], D::EFFECT);
			register_port(C(game_addr(ORIG_Ribbon)), (void *)RibbonTask, names[9], D::EFFECT);
			register_port(C(game_addr(ORIG_Column)), (void *)ColumnTask, names[10], D::EFFECT);
			register_port(C(game_addr(ORIG_ColumnFlash)), (void *)ColumnFlashTask, names[11], D::EFFECT);
			register_port(C(game_addr(ORIG_Pillar)), (void *)PillarTask, names[12], D::EFFECT);
			register_port(C(game_addr(ORIG_Flame)), (void *)FlameTask, names[13], D::EFFECT);
			register_port(C(game_addr(ORIG_Sparks)), (void *)SparksTask, names[14], D::EFFECT);
			register_port(C(game_addr(ORIG_Debris)), (void *)DebrisTask, names[15], D::EFFECT);
			register_port(C(game_addr(ORIG_Knock)), (void *)KnockTask, names[16], D::EFFECT);
			register_port(C(game_addr(ORIG_Throw)), (void *)ThrowTask, names[17], D::EFFECT);
			register_port(C(game_addr(ORIG_Ring2)), (void *)Ring2Task, names[18], D::EFFECT);
		}
	};
}
}
