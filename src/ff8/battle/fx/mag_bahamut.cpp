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

// Effect 202: Bahamut

#include "gfc_engine.h"

namespace ff8fx
{
namespace gfc
{
namespace part_bahamut
{
	static inline void xl_SetFogNearFar_56CC50(int32_t a, int32_t b, int32_t h) { x::f<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFogNearFar))(a, b, h); }
	static inline int32_t xl_RotTransPers_56C880(const void *v, void *sxy, void *p, void *flag) { return x::f<int32_t (__cdecl *)(const void *, void *, void *, void *)>(game_addr(game::TransformWorldCoordinateToProjectedSpace))(v, sxy, p, flag); }
	static inline int32_t xl_RotTransPersDiv8_56C8C0(const void *v, void *sxy, void *p, void *flag) { return x::f<int32_t (__cdecl *)(const void *, void *, void *, void *)>(game_addr(game::GTE_RotTransPersDiv8))(v, sxy, p, flag); }
	static inline int32_t xl_RotTransPersNormalCol_56CAA0(const void *v, const void *normal, const void *rgbc, void *sxy, void *rgb, void *flag) { return x::f<int32_t (__cdecl *)(const void *, const void *, const void *, void *, void *, void *)>(game_addr(game::GTE_RotTransPersNormalColor))(v, normal, rgbc, sxy, rgb, flag); }
	static inline uint32_t xl_ScaleColour_B66A20(uint32_t colour, int32_t k) { return x::f<uint32_t (__cdecl *)(uint32_t, int32_t)>(game_addr(game::GfCinematic_ScaleColourGPF))(colour, k); }
	static inline void xl_InsertPrimAutoDepth_45C7A0(uint32_t ot, void *prim) { x::f<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAutoDepth))(ot, prim); }

	static inline uint8_t *ZEROVEC() { return (uint8_t *)game_addr(game::unk_27971E4); }

	static void __cdecl dh_40_LitScrollGrid()
	{
		uint8_t *arg = PTR(CUR(), 0xB8);
		S32(WS(), 0x60) = S16(arg, 4);
		S32(WS(), 0x64) = S16(arg, 6);
		int32_t tex = S16(arg, 2);
		blob::TexInfo_B665C0(tex);
		uint8_t *w = WS();
		U32(w, 0x68) = U32(w, 0xFC);
		int32_t clut = blob::ClutWord_B66640(tex);
		S32(WS(), 0x6C) = clut;
		w = WS();
		{
			int32_t half = S32(w, 0x60) / 2;
			uint32_t pg = ((uint32_t)U16(CUR(), 0x98) >> 4) & 0xF0;
			S32(w, 0x70) = (int32_t)(~(uint32_t)shl32(half, 4) - pg + 1u);
		}
		{
			uint32_t pg = ((uint32_t)U16(CUR(), 0x94) >> 4) & 0xF0;
			w = WS();
			int32_t half = S32(w, 0x64) / 2;
			S32(w, 0x74) = (int32_t)(~(uint32_t)shl32(half, 4) - pg + 1u);
		}
		w = WS();
		S32(w, 0x78) = mul32(add32(S32(w, 0x60), 1), 12);
		uint8_t *other = blob::GetBone(S16(arg, 0));
		if (U8(CUR(), 0xDE) & 0x20)
			U32(WS(), 0x7C) = U32(CUR(), 0xC0);
		else
			S32(WS(), 0x7C) = -1;
		int32_t saved8E = S16(CUR(), 0x8E);
		U16(CUR(), 0x8E) = 0x1000;
		h_B27130();
		U16(CUR(), 0x8E) = (uint16_t)saved8E;
		h_B29450((int32_t)U32(CUR(), 0xCC));
		xl_SetFogNearFar_56CC50(S16(arg, 8), S16(arg, 0xA), 0x200);
		{
			uint32_t bc = U32(other, 0xCC);
			x::GteSetBackColor3((int32_t)(bc & 0xFF), (int32_t)((bc >> 8) & 0xFF), (int32_t)((bc >> 16) & 0xFF));
		}
		uint8_t *vb = PTR(CTX(), 0x74);
		w = WS();
		U16(w, 0xC0) = (uint16_t)sub32(U8(CUR(), 0x94), shl32(S32(w, 0x64) / 2, 8));
		w = WS();
		int32_t z0 = sub32(U8(CUR(), 0x98), shl32(S32(w, 0x60) / 2, 8));
		U16(w, 0xC2) = 0;
		U16(WS(), 0xC8) = (uint16_t)(U16(CUR(), 0x96) << 4);
		U16(WS(), 0xCA) = 0;
		U16(WS(), 0xCC) = 0;
		U32(WS(), 0xE0) = 0x3C808080;
		uint32_t code = (U32(CUR(), 0xCC) & 0x2000000u) | 0x3C000000u;
		w = WS();
		if (S32(w, 0x64) >= 0)
		{
			int32_t rows = S32(w, 0x64) + 1;
			do
			{
				U16(w, 0xC4) = (uint16_t)z0;
				w = WS();
				int32_t cols = S32(w, 0x60);
				if (cols >= 0)
				{
					cols++;
					uint8_t *rgb = vb + 8;
					do
					{
						int32_t otz = xl_RotTransPersNormalCol_56CAA0(w + 0xC0, w + 0xC8, w + 0xE0, vb, rgb, w + 0xFC);
						uint32_t c = U32(rgb, 0);
						U16(rgb, -4) = (uint16_t)otz;
						U32(rgb, 0) = c | code;
						vb += 0xC;
						rgb += 0xC;
						U16(WS(), 0xC4) = (uint16_t)(U16(WS(), 0xC4) + 0x100);
						w = WS();
					} while (--cols != 0);
				}
				U16(w, 0xC0) = (uint16_t)(U16(w, 0xC0) + 0x100);
				w = WS();
			} while (--rows != 0);
		}
		uint8_t *ctx = CTX();
		int32_t v = S32(w, 0x74);
		int32_t rowstride = S32(w, 0x78);
		uint8_t *a = PTR(ctx, 0x74);
		int32_t rows = S32(w, 0x64);
		uint8_t *pkt = PTR(ctx, 0x7C);
		uint8_t *b = a + rowstride;
		if (rows > 0)
		{
			do
			{
				int32_t cols = S32(w, 0x60);
				int32_t u = S32(w, 0x70);
				if (cols > 0)
				{
					int32_t v2 = add32(v, 0x10);
					int32_t n = cols;
					for (;;)
					{
						U8(pkt, 3) = 0xC;
						U32(pkt, 4) = U32(a, 8);
						U32(pkt, 0x10) = U32(a, 0x14);
						U32(pkt, 0x1C) = U32(b, 8);
						U32(pkt, 0x28) = U32(b, 0x14);
						U16(pkt, 0x1A) = U16(WS(), 0x68);
						uint16_t cl = U16(WS(), 0x6C);
						U8(pkt, 0x24) = (uint8_t)u;
						U8(pkt, 0xC) = (uint8_t)u;
						u = add32(u, 0x10);
						uint8_t u2 = (u & 0xF0) ? (uint8_t)u : 0xFF;
						U16(pkt, 0xE) = cl;
						U8(pkt, 0x30) = u2;
						U8(pkt, 0x18) = u2;
						U8(pkt, 0x19) = (uint8_t)v;
						U8(pkt, 0xD) = (uint8_t)v;
						uint8_t vv2 = (v2 & 0xF0) ? (uint8_t)v2 : 0xFF;
						U8(pkt, 0x31) = vv2;
						U8(pkt, 0x25) = vv2;
						U32(pkt, 8) = U32(a, 0);
						U32(pkt, 0x14) = U32(a, 0xC);
						U32(pkt, 0x20) = U32(b, 0);
						U32(pkt, 0x2C) = U32(b, 0xC);
						int32_t z = S32(WS(), 0x7C);
						if (z < 0)
							z = (S16(b, 0x10) + S16(b, 4) + S16(a, 0x10) + S16(a, 4)) / 4;
						x::InsertPrimAltViewport(U32(RT(), 0x4C) + (uint32_t)(z & ~3), pkt);
						a += 0xC;
						b += 0xC;
						pkt += 0x34;
						if (--n == 0) break;
					}
					w = WS();
				}
				v = add32(v, 0x10);
				a += 0xC;
				b += 0xC;
			} while (--rows != 0);
		}
		U32(CTX(), 0x7C) = (uint32_t)pkt;
	}

	static void __cdecl dh_41_SineStrip()
	{
		int32_t tex = S16(PTR(CUR(), 0xB8), 0);
		blob::TexInfo_B665C0(tex);
		uint8_t *w = WS();
		U32(w, 0x68) = U32(w, 0xFC);
		int32_t clut = blob::ClutWord_B66640(tex);
		S32(WS(), 0x6C) = clut;
		if (U8(CUR(), 0xDE) & 0x20)
			U32(WS(), 0x64) = U32(CUR(), 0xC0);
		else
			S32(WS(), 0x64) = -1;
		h_B27130();
		uint8_t *vb = PTR(CTX(), 0x74);
		int32_t n = 0x7F;
		U16(WS(), 0xC4) = 0x800;
		uint8_t *cur = CUR();
		uint8_t *vm = vb + 8;
		int32_t phase = shl32(S16(cur, 0x96), 4);
		int32_t dphase = S16(cur, 0x98);
		int32_t amp = S16(cur, 0x94);
		do
		{
			int32_t s = x::Sin(phase);
			U16(WS(), 0xC2) = (uint16_t)(mul32(s, amp) >> 12);
			U16(WS(), 0xC0) = 0xF810;
			w = WS();
			U16(vm, -4) = (uint16_t)xl_RotTransPers_56C880(w + 0xC0, vb, w + 0xF8, w + 0xFC);
			U16(WS(), 0xC0) = 0;
			w = WS();
			U16(vm, 4) = (uint16_t)xl_RotTransPers_56C880(w + 0xC0, vm, w + 0xF8, w + 0xFC);
			U16(WS(), 0xC0) = 0x7F0;
			w = WS();
			U16(vm, 0xC) = (uint16_t)xl_RotTransPers_56C880(w + 0xC0, vm + 8, w + 0xF8, w + 0xFC);
			vb += 0x18;
			vm += 0x18;
			U16(WS(), 0xC4) = (uint16_t)(U16(WS(), 0xC4) - 0x20);
			phase = add32(phase, dphase);
		} while (--n != 0);

		uint8_t *ctx = CTX();
		cur = CUR();
		int32_t v = 1;
		n = 0x7E;
		uint8_t *pkt = PTR(ctx, 0x7C);
		uint8_t *r = PTR(ctx, 0x74);
		uint32_t col = U32(cur, 0xCC) | 0x2C000000u;
		do
		{
			U8(pkt, 3) = 9;
			U8(pkt, 0x2B) = 9;
			U32(pkt, 0x2C) = col;
			U32(pkt, 4) = col;
			uint16_t tp = U16(WS(), 0x68);
			U16(pkt, 0x3E) = tp;
			U16(pkt, 0x16) = tp;
			uint16_t cl = U16(WS(), 0x6C);
			U8(pkt, 0x1C) = 0;
			U16(pkt, 0x36) = cl;
			U16(pkt, 0xE) = cl;
			uint8_t v2 = (uint8_t)(v + 2);
			U8(pkt, 0xC) = 0;
			U8(pkt, 0x24) = 0x7F;
			U8(pkt, 0x14) = 0x7F;
			U8(pkt, 0x44) = 0;
			U8(pkt, 0x34) = 0;
			U8(pkt, 0x4C) = 0x7F;
			U8(pkt, 0x3C) = 0x7F;
			U8(pkt, 0x3D) = (uint8_t)v;
			U8(pkt, 0x35) = (uint8_t)v;
			U8(pkt, 0x15) = (uint8_t)v;
			U8(pkt, 0xD) = (uint8_t)v;
			U8(pkt, 0x4D) = v2;
			U8(pkt, 0x45) = v2;
			U8(pkt, 0x25) = v2;
			U8(pkt, 0x1D) = v2;
			U32(pkt, 8) = U32(r, 0);
			U32(pkt, 0x10) = U32(r, 8);
			U32(pkt, 0x18) = U32(r, 0x18);
			U32(pkt, 0x20) = U32(r, 0x20);
			U32(pkt, 0x30) = U32(r, 0x10);
			U32(pkt, 0x38) = U32(r, 8);
			U32(pkt, 0x40) = U32(r, 0x28);
			U32(pkt, 0x48) = U32(r, 0x20);
			int32_t z = S32(WS(), 0x64);
			if (z < 0)
			{
				int32_t z1 = (S16(r, 0x1C) + S16(r, 4) + S16(r, 0x24) + S16(r, 0xC)) / 4;
				xl_InsertPrimAutoDepth_45C7A0(U32(RT(), 0x4C) + (uint32_t)(z1 & ~3), pkt);
				int32_t z2 = (S16(r, 0x2C) + S16(r, 0x14) + S16(r, 0x24) + S16(r, 0xC)) / 4;
				xl_InsertPrimAutoDepth_45C7A0(U32(RT(), 0x4C) + (uint32_t)(z2 & ~3), pkt + 0x28);
			}
			else
			{
				uint32_t ot = (uint32_t)(z & ~3) + U32(RT(), 0x4C);
				xl_InsertPrimAutoDepth_45C7A0(ot, pkt);
				xl_InsertPrimAutoDepth_45C7A0(ot, pkt + 0x28);
			}
			pkt += 0x50;
			r += 0x18;
			v += 2;
		} while (--n != 0);
		U32(CTX(), 0x7C) = (uint32_t)pkt;
	}

	static int32_t RibbonBisector(int32_t a, int32_t b, int32_t prev)
	{
		int32_t lo = a & 0xFFF;
		int32_t hi = b & 0xFFF;
		if (lo > hi) { int32_t t = hi; hi = lo; lo = t; }
		int32_t d = hi - lo;
		int32_t m;
		if (d < 0x800) m = d / 2 + lo;
		else m = (0x1000 - d) / 2 + hi;
		int32_t p = prev;
		int32_t q = m & 0xFFF;
		if (p < q) { int32_t t = p; p = q; q = t; }
		p -= q;
		if (p >= 0x400 && p < 0xC00) m += 0x800;
		return m & 0xFFF;
	}

	static void BeamRibbon(int32_t count, uint8_t *blk, uint8_t *verts, int32_t vstep)
	{
		uint8_t *p = verts;
		PTR(WS(), 0xF0) = p;
		int32_t ang = blob::Blob_B66B30(S16(p, 2), S16(p, 0), S16(p, 0x22), S16(p, 0x20));
		int32_t perp = add32(ang, 0x400);
		int32_t wd = S16(p, 0x10);
		int32_t dx = mul32(x::Cos(perp), wd) >> 12;
		int32_t dy = mul32(x::Sin(perp), wd) >> 12;
		uint16_t px = U16(p, 0);
		U16(p, 8) = (uint16_t)(px + dx);
		uint16_t py = U16(p, 2);
		U16(p, 0xC) = (uint16_t)(px - dx);
		U16(p, 0xA) = (uint16_t)(py + dy);
		U16(p, 0xE) = (uint16_t)(py - dy);
		uint8_t *q = p + 0x20;
		int32_t segs = count - 1;
		if (segs > 1)
		{
			int32_t k = segs - 1;
			do
			{
				int32_t prev = ang;
				ang = blob::Blob_B66B30(S16(q, 2), S16(q, 0), S16(q, 0x22), S16(q, 0x20));
				perp = RibbonBisector(ang, add32(prev, 0x800), perp);
				wd = S16(q, 0x10);
				dx = mul32(x::Cos(perp), wd) >> 12;
				dy = mul32(x::Sin(perp), wd) >> 12;
				px = U16(q, 0);
				U16(q, 8) = (uint16_t)(px + dx);
				py = U16(q, 2);
				U16(q, 0xC) = (uint16_t)(px - dx);
				U16(q, 0xA) = (uint16_t)(py + dy);
				U16(q, 0xE) = (uint16_t)(py - dy);
				q += 0x20;
			} while (--k != 0);
		}
		wd = S16(q, 0x10);
		dx = mul32(x::Cos(perp), wd) >> 12;
		dy = mul32(x::Sin(perp), wd) >> 12;
		px = U16(q, 0);
		U16(q, 8) = (uint16_t)(px + dx);
		py = U16(q, 2);
		U16(q, 0xC) = (uint16_t)(px - dx);
		U16(q, 0xA) = (uint16_t)(py + dy);
		U16(q, 0xE) = (uint16_t)(py - dy);

		uint8_t *pkt = PTR(CTX(), 0x7C);
		S32(WS(), 0x68) = S16(blk, 8);
		int32_t v = vstep & 0xFF;
		S32(WS(), 0x6C) = S16(blk, 0xA);
		uint8_t *s = PTR(WS(), 0xF0);
		S32(WS(), 0xF4) = (int32_t)U8(blk, 0x1A) - 1;
		S32(WS(), 0xF8) = U8(blk, 0x1B);
		if (segs > 0)
		{
			s += 0x14;
			int32_t n = segs;
			do
			{
				uint8_t *next;
				if (U32(s, 0) == U32(s, 0x20))
				{
					U8(pkt, 3) = 9;
					U16(pkt, 0x16) = U16(WS(), 0x68);
					U16(pkt, 0xE) = U16(WS(), 0x6C);
					U32(pkt, 4) = U32(s, 0) | 0x2C000000u;
					U32(pkt, 8) = U32(s, -0xC);
					U32(pkt, 0x10) = U32(s, -8);
					U32(pkt, 0x18) = U32(s, 0x14);
					U32(pkt, 0x20) = U32(s, 0x18);
					U8(pkt, 0x1C) = 1;
					U8(pkt, 0xC) = 1;
					uint8_t ue = U8(WS(), 0xF4);
					U8(pkt, 0x15) = (uint8_t)v;
					U8(pkt, 0x24) = ue;
					U8(pkt, 0x14) = ue;
					U8(pkt, 0xD) = (uint8_t)v;
					v = add32(v, S32(WS(), 0xF8));
					if (v >= 0x100)
					{
						U8(pkt, 0x25) = 0xFF;
						U8(pkt, 0x1D) = 0xFF;
						v = 0;
					}
					else
					{
						U8(pkt, 0x25) = (uint8_t)v;
						U8(pkt, 0x1D) = (uint8_t)v;
					}
					next = pkt + 0x28;
				}
				else
				{
					U8(pkt, 3) = 0xC;
					U16(pkt, 0x1A) = U16(WS(), 0x68);
					U16(pkt, 0xE) = U16(WS(), 0x6C);
					uint32_t c0 = U32(s, 0) | 0x3C000000u;
					U32(pkt, 0x10) = c0;
					U32(pkt, 4) = c0;
					uint32_t c1 = U32(s, 0x20);
					U32(pkt, 0x28) = c1;
					U32(pkt, 0x1C) = c1;
					U32(pkt, 8) = U32(s, -0xC);
					U32(pkt, 0x14) = U32(s, -8);
					U32(pkt, 0x20) = U32(s, 0x14);
					U32(pkt, 0x2C) = U32(s, 0x18);
					U8(pkt, 0x24) = 1;
					U8(pkt, 0xC) = 1;
					uint8_t ue = U8(WS(), 0xF4);
					U8(pkt, 0x19) = (uint8_t)v;
					U8(pkt, 0x30) = ue;
					U8(pkt, 0x18) = ue;
					U8(pkt, 0xD) = (uint8_t)v;
					v = add32(v, S32(WS(), 0xF8));
					if (v >= 0x100)
					{
						U8(pkt, 0x31) = 0xFF;
						U8(pkt, 0x25) = 0xFF;
						v = 0;
					}
					else
					{
						U8(pkt, 0x31) = (uint8_t)v;
						U8(pkt, 0x25) = (uint8_t)v;
					}
					next = pkt + 0x34;
				}
				int32_t z = (S16(s, -0x10) + S16(s, 0x10)) / 2;
				x::InsertPrimDepthKeys(U32(RT(), 0x4C) + (uint32_t)(z & ~3), pkt, 0, 0, 0, 0);
				s += 0x20;
				pkt = next;
				S32(WS(), 0x70) = add32(S32(WS(), 0x70), 1);
			} while (--n != 0);
		}
		U32(CTX(), 0x7C) = (uint32_t)pkt;
	}

	static void __cdecl dh_43_Beam()
	{
		if (PTR(CUR(), 0xBC) == nullptr)
		{
			uint8_t *arg = PTR(CUR(), 0xB8);
			int32_t n = S16(arg, 2);
			uint8_t *m = blob::ArenaAlloc(add32(shl32(n, 3), 0x20));
			PTR(CUR(), 0xBC) = m;
			uint8_t *blk = PTR(CUR(), 0xBC);
			U16(blk, 4) = (uint16_t)n;
			int32_t tex = S16(arg, 0);
			blob::TexInfo_B665C0(tex);
			U16(blk, 8) = U16(WS(), 0xFC);
			U16(blk, 0xA) = (uint16_t)blob::ClutWord_B66640(tex);
			U16(blk, 0x18) = U16(arg, 8);
			uint8_t *o = blob::ObjectPtr(S16(arg, 4));
			U32(blk, 0xC) = U32(o, 0x14) + (uint32_t)o;
			U16(blk, 0x14) = U16(o, 0x18);
			o = blob::ObjectPtr(S16(arg, 6));
			U32(blk, 0x10) = U32(o, 0x14) + (uint32_t)o;
			U16(blk, 0x16) = U16(o, 0x18);
			S32(WS(), 0x74) = S16(arg, 0xE);
			U8(blk, 0x1A) = U8(arg, 0xA);
			U8(blk, 0x1B) = U8(arg, 0xC);
			U16(blk, 2) = 0;
			uint8_t *w = WS();
			if (U8(w, 0x74) & 1)
			{
				S32(w, 0x78) = sub32(0, shl32(S16(blk, 0x14), 8) / S16(blk, 4));
				S32(WS(), 0x7C) = sub32(0, shl32(S16(blk, 0x16), 8) / S16(blk, 4));
				S32(WS(), 0x80) = sub32(shl32(S16(blk, 0x14), 8), 0x100);
				S32(WS(), 0x84) = sub32(shl32(S16(blk, 0x16), 8), 0x100);
			}
			else
			{
				S32(w, 0x78) = shl32(S16(blk, 0x14), 8) / S16(blk, 4);
				S32(WS(), 0x7C) = shl32(S16(blk, 0x16), 8) / S16(blk, 4);
				S32(WS(), 0x80) = 0;
				S32(WS(), 0x84) = 0;
			}
			S32(WS(), 0x88) = 0;
			S32(WS(), 0x8C) = shl32(S16(CUR(), 0x96), 8) / S16(blk, 4);
			uint8_t *pt = blk + 0x20;
			uint8_t *la = PTR(blk, 0xC);
			uint8_t *lb = PTR(blk, 0x10);
			S32(WS(), 0xF0) = S16(blk, 0x18);
			int32_t wx = blob::Rand74(0x100);
			int32_t wy = blob::Rand74(0x100);
			int32_t wz = blob::Rand74(0x100);
			int32_t cnt = S16(blk, 4);
			if (cnt > 0)
			{
				do
				{
					w = WS();
					pt += 8;
					uint8_t *pa = (uint8_t *)((uint32_t)la + (uint32_t)shl32(S32(w, 0x80) >> 8, 3));
					uint8_t *pb = (uint8_t *)((uint32_t)lb + (uint32_t)shl32(S32(w, 0x84) >> 8, 3));
					int32_t c = S16(pa, 0);
					U16(pt, -8) = (uint16_t)add32(mul32(S16(pb, 0) - c, wx) / 256, c);
					c = S16(pa, 2);
					int32_t d = mul32(S16(pb, 2) - c, wy) / 256;
					int32_t yo = S32(WS(), 0x88) >> 8;
					U16(pt, -6) = (uint16_t)add32(yo, add32(c, d));
					c = S16(pa, 4);
					U16(pt, -4) = (uint16_t)add32(mul32(S16(pb, 4) - c, wz) / 256, c);
					U16(pt, -2) = U16(WS(), 0xF0);
					w = WS();
					S32(w, 0x80) = add32(S32(w, 0x80), S32(w, 0x78));
					w = WS();
					S32(w, 0x84) = add32(S32(w, 0x84), S32(w, 0x7C));
					w = WS();
					S32(w, 0x88) = add32(S32(w, 0x88), S32(w, 0x8C));
				} while (--cnt != 0);
			}
		}

		uint8_t *blk = PTR(CUR(), 0xBC);
		PTR(WS(), 0x60) = blk + 0x20;
		uint8_t *vs = PTR(CTX(), 0x74);
		h_B27130();
		int32_t left = S16(blk, 4);
		S32(WS(), 0x64) = left;
		U16(WS(), 0xC4) = 0;
		U16(WS(), 0xC2) = 0;
		int32_t idx = S16(blk, 4) - 1;
		int32_t lim = S16(CUR(), 0x94) + 1;
		if (lim > left) lim = left;
		if (left > 0)
		{
			int32_t head = lim - left;
			int32_t fade = shl32(left, 6) - 0x40;
			do
			{
				int32_t rem = idx % S16(blk, 4);
				uint8_t *w = WS();
				uint8_t *pt = PTR(w, 0x60) + rem * 8;
				x::SetRotMatrix(w + 0xE0);
				x::SetTransVector(WS() + 0xE0);
				w = WS();
				int32_t otz = xl_RotTransPers_56C880(pt, vs, w + 0x98, w + 0x9C);
				U16(vs, 4) = (uint16_t)otz;
				w = WS();
				x::TransformToCamera(pt, w + 0xD4, w + 0x9C);
				x::SetRotMatrix(BBMAT());
				x::SetTransVector(WS() + 0xC0);
				U16(WS(), 0xC0) = U16(pt, 6);
				w = WS();
				xl_RotTransPers_56C880(w + 0xC0, w + 0x94, w + 0x98, w + 0x9C);
				U16(vs, 0x10) = (uint16_t)(U16(WS(), 0x94) - U16(vs, 0));
				if (left <= 4)
					U32(vs, 0x14) = xl_ScaleColour_B66A20(U32(CUR(), 0xCC), fade);
				else
				{
					int32_t k = (left <= lim) ? head : otz;
					if (k < 4)
						U32(vs, 0x14) = xl_ScaleColour_B66A20(U32(CUR(), 0xCC), shl32(k, 6));
					else
						U32(vs, 0x14) = U32(CUR(), 0xCC);
				}
				idx--;
				vs += 0x20;
				head++;
				left--;
				fade = sub32(fade, 0x40);
			} while (left > 0);
		}
		int32_t cnt = S16(CUR(), 0x94) + 1;
		int32_t n = S16(blk, 4);
		if (cnt > n) cnt = n;
		int32_t vstep = mul32(S16(CUR(), 0x98), U8(blk, 0x1B));
		uint8_t *verts = (uint8_t *)((uint32_t)shl32(n - cnt, 5) + U32(CTX(), 0x74));
		BeamRibbon(cnt, blk, verts, vstep);
	}

	static void __cdecl dh_46_FlatQuad()
	{
		uint8_t *pkt = PTR(CTX(), 0x7C);
		uint8_t *arg = PTR(CUR(), 0xB8);
		U8(pkt, 3) = 9;
		U32(pkt, 4) = U32(CUR(), 0xCC) | 0x2C000000u;
		U16(pkt, 0x16) = U16(CUR(), 0x92);
		U16(pkt, 0xE) = U16(CUR(), 0x9A);
		h_B27000();
		uint8_t *w = WS();
		int32_t otz = xl_RotTransPersDiv8_56C8C0(ZEROVEC(), w + 0xD0, w + 0xFC, w + 0xFC);
		int32_t hw = S16(arg, 4) / 2;
		int32_t hh = S16(arg, 6) / 2;
		uint16_t xl = (uint16_t)(U16(WS(), 0xD0) - (uint16_t)hw);
		U16(pkt, 0x18) = xl;
		U16(pkt, 8) = xl;
		uint16_t xr = (uint16_t)(U16(WS(), 0xD0) + (uint16_t)hw);
		U16(pkt, 0x20) = xr;
		U16(pkt, 0x10) = xr;
		uint8_t uw = (uint8_t)((uint32_t)hw << 1);
		uint16_t yt = (uint16_t)(U16(WS(), 0xD2) - (uint16_t)hh);
		U16(pkt, 0x12) = yt;
		U16(pkt, 0xA) = yt;
		int32_t slot = (otz >> 2) & ~3;
		uint16_t yb = (uint16_t)(U16(WS(), 0xD2) + (uint16_t)hh);
		U16(pkt, 0x22) = yb;
		U16(pkt, 0x1A) = yb;
		uint8_t u = (uint8_t)S16(arg, 0);
		uint8_t v = (uint8_t)S16(arg, 2);
		U8(pkt, 0x15) = v;
		uint8_t vh = (uint8_t)((uint8_t)((uint32_t)hh << 1) + v);
		uw = (uint8_t)(uw + u);
		U8(pkt, 0x1C) = u;
		U8(pkt, 0xC) = u;
		U8(pkt, 0x24) = uw;
		U8(pkt, 0x14) = uw;
		U8(pkt, 0xD) = v;
		U8(pkt, 0x25) = vh;
		U8(pkt, 0x1D) = vh;
		x::InsertPrimAltViewport((uint32_t)slot + U32(RT(), 0x4C), pkt);
		U32(CTX(), 0x7C) = (uint32_t)(pkt + 0x28);
	}
}

	void apply_bahamut(Clone &c)
	{
		c.draw[40] = part_bahamut::dh_40_LitScrollGrid;
		c.draw[41] = part_bahamut::dh_41_SineStrip;
		c.draw[43] = part_bahamut::dh_43_Beam;
		c.draw[46] = part_bahamut::dh_46_FlatQuad;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace bahamut
{
	static Clone g_c = {};

	static void describe(Clone &c)
	{
		c.name = "202 Bahamut";
		c.effect_id = 202;
		c.vm_table = game_addr(game::GF_202Bahamut_VmOpcodeTable);
		c.bone_table = game_addr(game::GF_202Bahamut_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_B1D064);
		c.prim_size = game_addr(game::unk_1874104);
		c.spr_table = game_addr(game::unk_1874118);
		c.ptype_table = game_addr(game::unk_1874144);
		c.pop_table = game_addr(game::funcs_B1F697);
		c.draw_table = game_addr(game::GF_202Bahamut_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_1874322);
		c.attr_184 = game_addr(game::unk_187433C);
		c.attr_186 = game_addr(game::unk_187433E);
		c.attr_194 = game_addr(game::unk_187434C);
		c.attr_19C = game_addr(game::unk_1874354);
		c.desc = game_addr(game::unk_1873A60);
		c.ptr_block = game_addr(game::unk_1873FA4);
		c.queue = game_addr(game::unk_2796DE0);
		c.file00 = game_addr(game::unk_2796E14);
		c.file01 = game_addr(game::unk_2796E10);
		c.tick = game_addr(game::GF_202Bahamut_SequenceTick);
		c.load_cb = game_addr(game::MAG_202_sub_B21090);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_gfc_bahamut()
	{
		using namespace gfc;
		bahamut::describe(bahamut::g_c);
		init_clone(bahamut::g_c, nullptr, 0);
		apply_bahamut(bahamut::g_c);
		apply_shared_ribbon(bahamut::g_c);
		apply_shared_misc(bahamut::g_c);
		register_port(game::GF_202Bahamut_SequenceTick, (void *)bahamut::SequenceTask, "GFC202 Bahamut SequenceTick", 202);
	}
}

