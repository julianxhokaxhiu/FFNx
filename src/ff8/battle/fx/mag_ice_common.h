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

// Code shared by Blizzara and Blizzaga

#pragma once

#include "mag_common.h"

namespace ff8fx
{
namespace icec
{
	using namespace eng;

	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZReg(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadRGB012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadRGB012))(a, b, c); }
	inline void GteDPCT() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCT))(); }
	inline void GteStoreRGB012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_StoreRGB012))(a, b, c); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline int32_t NormalizeVector16(int16_t *in, int16_t *out) { return fn<int32_t (__cdecl *)(int16_t *, int16_t *)>(game_addr(game::VectorNormalS16))(in, out); }

	static inline void FillDwords(void *dst, uint32_t value, uint32_t count)
	{
		uint32_t *d = (uint32_t *)dst;
		for (uint32_t i = 0; i < count; i++) d[i] = value;
	}
	static inline void CopyDwords(void *dst, const void *src, uint32_t count)
	{
		uint32_t *d = (uint32_t *)dst;
		const uint32_t *s = (const uint32_t *)src;
		for (uint32_t i = 0; i < count; i++) d[i] = s[i];
	}

	static uint8_t *ModelList(uint32_t n, uint8_t *model, uint32_t *count)
	{
		static const uint8_t size[7] = { 12, 12, 20, 24, 20, 24, 28 };
		uint8_t *p = model + *(uint32_t *)model;
		uint32_t c = *(uint32_t *)p;
		p += 4;
		for (uint32_t i = 0; i < 8; i++)
		{
			if (n == i)
			{
				if (count) *count = c;
				return p;
			}
			if (i == 7) break;
			p += c * size[i];
			c = *(uint32_t *)p;
			p += 4;
		}
		if (count) *count = 0;
		return nullptr;
	}

	inline uint8_t *&Cursor(uint8_t *h, int off) { return *(uint8_t **)(h + off); }

	static inline uint32_t SkipList(uint8_t *h, int off, uint32_t record, uint32_t ret)
	{
		uint8_t *p = Cursor(h, off);
		Cursor(h, off) = p + *(uint32_t *)p * record + 4;
		return ret;
	}

	static int16_t *PreparePolys(uint8_t *h, int16_t *base, int16_t *cursor)
	{
		uint8_t *prim = Cursor(h, 0x20);
		int32_t count = *(int32_t *)prim;
		prim += 4;
		Cursor(h, 0x20) = prim;
		if (count <= 0) return cursor;
		int16_t *v = cursor;
		for (int32_t left = count; left; left--, prim += 0x1C)
		{
			const uint8_t *verts = Cursor(h, 4);
			for (int k = 0; k < 3; k++)
			{
				uint16_t idx = *(uint16_t *)(prim + 4 + 2 * k);
				ff8fx::copy<(8)>(v, verts + 4 * idx);
				*(uint16_t *)(prim + 4 + 2 * k) = (uint16_t)(((uint8_t *)v - (uint8_t *)base) >> 2);
				v += 4;
			}
			if (h[0x1C] & 1) *(uint32_t *)prim |= 0x02000000;
			if (h[0x1C] & 4) *(uint32_t *)prim &= 0xFDFFFFFF;
			if (h[0x1C] & 0x80)
			{
				GteSetFarColor(h[8], h[9], h[0xA]);
				GteLoadRGB012(prim + 0x14, prim + 0x18, prim);
				GteSetIR0(*(int32_t *)(h + 0xC));
				GteDPCT();
				GteStoreRGB012(prim + 0x14, prim + 0x18, prim);
			}
		}
		Cursor(h, 0x20) = prim;
		return v;
	}

	static int16_t *PrepareModel(uint8_t *h, uint8_t *model, int16_t *vbuf)
	{
		Cursor(h, 4) = model + 8;
		uint8_t *m = Cursor(h, 0);
		Cursor(h, 0x20) = m + *(uint32_t *)m;
		static const uint8_t size[8] = { 12, 12, 20, 24, 20, 24, 0, 36 };
		int16_t *cur = vbuf;
		for (int i = 0; i < 8; i++)
		{
			uint8_t *p = Cursor(h, 0x20);
			if (*(uint32_t *)p == 0)
			{
				Cursor(h, 0x20) = p + 4;
				continue;
			}
			if (i == 6) cur = PreparePolys(h, vbuf, cur);
			else SkipList(h, 0x20, size[i], 0);
		}
		return cur;
	}

	static inline uint32_t TextureWindow(const uint8_t *w)
	{
		uint32_t c = ((uint32_t)w[2] & 0xF8) | 0xFFFE2000;
		c = (c << 5) | ((uint32_t)w[0] & 0xF8);
		c = (c << 5) | ((uint32_t)-(int32_t)*(const uint16_t *)(w + 6) & 0xF8);
		c = (c << 2) | ((uint32_t)(-(int32_t)*(const uint16_t *)(w + 4) >> 3) & 0x1F);
		return c;
	}

	static uint32_t RenderPolys(uint8_t *h, uint32_t ot, int shift, uint32_t cursor, bool scroll_u)
	{
		uint8_t *ext = Cursor(h, 0x10) + 4;
		Cursor(h, 0x10) = ext;
		uint8_t *prim = Cursor(h, 0xC);
		int32_t count = *(int32_t *)prim;
		prim += 4;
		Cursor(h, 0xC) = prim;
		uint8_t *p = (uint8_t *)cursor;
		if (count > 0)
		{
			uint32_t flags = 0;
			for (int32_t left = count; left; left--, prim += 0x1C, ext += 0x14)
			{
				const uint8_t *vb = Cursor(h, 8);
				GteLoadV012(vb + 4 * *(uint16_t *)(prim + 4), vb + 4 * *(uint16_t *)(prim + 6), vb + 4 * *(uint16_t *)(prim + 8));
				GteRTPT();
				uint32_t uvofs = *(uint32_t *)(h + 0x14);
				*(uint32_t *)p = 0x09000000;
				*(uint32_t *)(p + 4) = *(uint32_t *)prim;
				*(uint32_t *)(p + 0xC) = *(uint32_t *)(prim + 0xC) + uvofs;
				*(uint32_t *)(p + 0x18) = *(uint32_t *)(prim + 0x10) + uvofs;
				*(uint32_t *)(p + 0x24) = (*(uint32_t *)(prim + 8) >> 16) + uvofs;
				GteReadFLAG(h + 0x3C);
				if (*(uint32_t *)(h + 0x3C) & 0x60000) continue;
				GteNCLIP();
				uint32_t clip = 0;
				GteReadMAC0(h + 0x30);
				if (*(int32_t *)(h + 0x30) < 0 && !(h[0x18] & 0x20) && prim[0x17] == 0) continue;
				GteReadSXY012(p + 8, p + 0x14, p + 0x20);
				GteAVSZ3();
				int16_t x;
				x = *(int16_t *)(p + 8);
				clip = (x < 0 || x > 0xA00) ? 1 : clip;
				x = *(int16_t *)(p + 0x14);
				if (x < 0 || x > 0xA00) clip |= 2;
				x = *(int16_t *)(p + 0x20);
				if (x < 0 || x > 0xA00) clip |= 4;
				x = *(int16_t *)(p + 0xA);
				if (x < 0 || x > 0x6C0) clip |= 0x10;
				x = *(int16_t *)(p + 0x16);
				if (x < 0 || x > 0x6C0) clip |= 0x20;
				x = *(int16_t *)(p + 0x22);
				if (x < 0 || x > 0x6C0) clip |= 0x40;
				if ((clip & 7) == 7 || (clip & 0x70) == 0x70) continue;
				GteReadOTZReg(h + 0x38);
				*(uint32_t *)(p + 0x10) = *(uint32_t *)(prim + 0x14);
				*(uint32_t *)(p + 0x1C) = *(uint32_t *)(prim + 0x18);
				uint32_t otp = ot + 4 * (*(int32_t *)(h + 0x38) >> shift);
				uint8_t *main = p;
				p += 0x28;
				flags = *(uint32_t *)(h + 0x18);
				if ((flags & 0x2000) && prim[0x1B])
				{
					uint8_t *q = p;
					p += 0x20;
					*(uint32_t *)(q + 8) = *(uint32_t *)(main + 8);
					*(uint32_t *)(q + 0x10) = *(uint32_t *)(main + 0x14);
					*(uint32_t *)(q + 0x18) = *(uint32_t *)(main + 0x20);
					uint8_t g = prim[0x1B];
					q[6] = g;
					q[5] = g;
					q[4] = g;
					*(uint32_t *)(q + 0xC) = *(uint32_t *)(ext + 0xC);
					*(uint32_t *)(q + 0x14) = *(uint32_t *)(ext + 0x10);
					*(uint32_t *)(q + 0x1C) = *(uint32_t *)(ext + 8) >> 16;
					if (*(uint32_t *)(h + 0x18) & 0x20000)
					{
						uint8_t *tw = p;
						p += 0xC;
						*(uint32_t *)tw = 0x02000000;
						*(uint32_t *)(tw + 4) = TextureWindow(h + 0x20);
						*(uint32_t *)(tw + 8) = 0;
						InsertPrimAutoDepth(otp, tw);
						if (scroll_u)
						{
							uint32_t su = *(uint16_t *)(h + 0x1C);
							uint32_t u2 = q[0x1C] + su, u0 = q[0xC] + su, u1 = q[0x14] + su;
							if ((u2 | u1 | u0) > 0xFF)
							{
								q[0xC] = (uint8_t)(u0 - h[0x2C]);
								q[0x14] = (uint8_t)(u1 - h[0x2C]);
								q[0x1C] = (uint8_t)(u2 - h[0x2C]);
							}
							else
							{
								q[0xC] = (uint8_t)u0;
								q[0x14] = (uint8_t)u1;
								q[0x1C] = (uint8_t)u2;
							}
						}
						uint32_t sv = *(uint16_t *)(h + 0x1E);
						uint32_t v2 = q[0x1D] + sv, v0 = q[0xD] + sv, v1 = q[0x15] + sv;
						if ((v2 | v1 | v0) > 0xFF)
						{
							q[0xD] = (uint8_t)(v0 - h[0x2E]);
							q[0x15] = (uint8_t)(v1 - h[0x2E]);
							v2 = v2 - h[0x2E];
						}
						else
						{
							q[0xD] = (uint8_t)v0;
							q[0x15] = (uint8_t)v1;
						}
						q[0x1D] = (uint8_t)v2;
						*(uint32_t *)q = 0x07000000;
						q[7] = 0x26;
						InsertPrimAutoDepth(otp, q);
						uint8_t *tw2 = p;
						p += 0xC;
						*(uint32_t *)tw2 = 0x02000000;
						*(uint32_t *)(tw2 + 4) = TextureWindow(h + 0x28);
						*(uint32_t *)(tw2 + 8) = 0;
						InsertPrimAutoDepth(otp, tw2);
					}
					else
					{
						*(uint32_t *)q = 0x07000000;
						q[7] = 0x26;
						InsertPrimAutoDepth(otp, q);
					}
				}
				InsertPrimAutoDepth(otp, main);
				if ((h[0x18] & 0x40) && prim[0x17])
				{
					uint8_t *g = p;
					p += 0x14;
					*(uint32_t *)(g + 8) = *(uint32_t *)(main + 8);
					*(uint32_t *)(g + 0xC) = *(uint32_t *)(main + 0x14);
					*(uint32_t *)(g + 0x10) = *(uint32_t *)(main + 0x20);
					uint8_t level = prim[0x17];
					g[6] = level;
					g[5] = level;
					g[4] = level;
					*(uint32_t *)g = 0x04000000;
					g[7] = 0x22;
					InsertPrimAutoDepth(otp, g);
					uint8_t *m = p;
					p += 8;
					*(uint32_t *)m = 0x01000000;
					*(uint32_t *)(m + 4) = 0xE1000240;
					InsertPrimAutoDepth(otp, m);
				}
			}
			(void)flags;
		}
		Cursor(h, 0xC) = prim;
		Cursor(h, 0x10) = ext;
		return (uint32_t)p;
	}

	typedef uint32_t (*PolyRenderer)(uint8_t *h, uint32_t ot, int shift, uint32_t cursor);

	static uint32_t RenderModel(uint8_t *h, uint32_t ot, int shift, uint32_t cursor, PolyRenderer polys)
	{
		uint8_t *m = Cursor(h, 0);
		Cursor(h, 0xC) = m + *(uint32_t *)m;
		uint8_t *m2 = Cursor(h, 4);
		Cursor(h, 0x10) = m2 + *(uint32_t *)m2 + 8;
		if (!(*(uint32_t *)(h + 0x18) & 0x1000)) *(uint32_t *)(h + 0x14) = 0;
		static const uint8_t size[8] = { 12, 12, 20, 24, 20, 24, 0, 36 };
		uint32_t r = cursor;
		for (int i = 0; i < 8; i++)
		{
			uint8_t *p = Cursor(h, 0xC);
			if (*(uint32_t *)p == 0)
			{
				Cursor(h, 0xC) = p + 4;
				continue;
			}
			if (i == 6) r = polys(h, ot, shift, r);
			else if (i == 2)
			{
				SkipList(h, 0xC, 20, 0);
				SkipList(h, 0x10, 20, 0);
			}
			else SkipList(h, 0xC, size[i], 0);
		}
		return r;
	}
}
}
