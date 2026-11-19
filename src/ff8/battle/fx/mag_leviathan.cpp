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

// Effect 006: Leviathan

#include "gfc_engine.h"

namespace ff8fx
{
namespace gfc
{
namespace part_lev_a
{
	static inline int32_t xl_RotTransPers(const void *v, void *sxy, void *p, void *flag)
	{
		return x::f<int32_t (__cdecl *)(const void *, void *, void *, void *)>(game_addr(game::GTE_RotTransPersDiv8))(v, sxy, p, flag);
	}

	static inline void xl_OffscreenRender_B65810()
	{
		x::f<int32_t (__cdecl *)()>(game_addr(game::GfCinematic_RenderPartyModelsOffscreen))();
	}

	static inline int32_t Div3(int32_t v)
	{
		int32_t hi = (int32_t)(((int64_t)v * (int64_t)0x55555556) >> 32);
		return sub32(hi, v >> 31);
	}

	static inline int32_t DepthKey(uint32_t off)
	{
		uint32_t v = DEPTHARR()[off >> 3] & 0xFFFF;
		return (int32_t)(int64_t)((double)v * (double)FLT_1877DA8());
	}

	static inline int32_t DivPow2(int32_t v, int k)
	{
		uint32_t bias = (uint32_t)(v >> 31) & ((1u << k) - 1u);
		return (int32_t)((uint32_t)v + bias) >> k;
	}

	static inline uint8_t *PADD(uint8_t *p, int32_t n) { return (uint8_t *)((uint32_t)p + (uint32_t)n); }

	static void __cdecl dh_17_RippleTextureUpload()
	{
		uint8_t *arg = PTR(CUR(), 0xB8);
		blob::ReadAltTexture(S16(arg, 0));
		U32(WS(), 0x64) = U32(WS(), 0xFC);
		uint8_t *tex = PTR(WS(), 0xF0);
		if (U32(CUR(), 0xBC) == 0)
		{
			int32_t n = shl32(mul32(S16(tex, 6), S16(tex, 4)), 1);
			uint8_t *buf = blob::ArenaAlloc(n);
			PTR(CUR(), 0xBC) = buf;
		}
		U32(WS(), 0x60) = U32(CUR(), 0xBC);
		{
			uint8_t *w = WS();
			U32(w, 0x80) = U32(w, 0x60);
		}
		uint8_t *rect = blob::VramRectRing();
		PTR(WS(), 0x68) = rect;
		rect = PTR(WS(), 0x68);
		U16(rect, 0) = U16(arg, 2);
		U16(rect, 2) = U16(arg, 4);
		uint16_t wd = U16(tex, 4);
		U16(rect, 4) = wd;
		S32(WS(), 0x78) = (int16_t)wd;
		int32_t w32 = S32(WS(), 0x78);
		uint16_t ht = U16(tex, 6);
		U16(rect, 6) = ht;
		S32(WS(), 0x7C) = (int16_t)ht;
		int32_t mask = sub32(w32, 1);
		S32(WS(), 0x6C) = mask;
		S32(WS(), 0x70) = (int32_t)S16(CUR(), 0x94) & mask;
		S32(WS(), 0x84) = add32(w32, w32);
		S32(WS(), 0x74) = S16(CUR(), 0x8C);
		S32(WS(), 0x88) = S16(CUR(), 0x8E);
		S32(WS(), 0x8C) = S16(CUR(), 0x90);
		uint8_t *w = WS();
		if (S32(w, 0x7C) > 0)
		{
			do
			{
				uint8_t *dst = PTR(w, 0x60);
				uint8_t *src = PTR(w, 0x64);
				int32_t c = x::Cos(S32(w, 0x88));
				w = WS();
				int32_t off = add32(S32(w, 0x70), mul32(c, S32(w, 0x74)) >> 20);
				off &= S32(w, 0x6C);
				int32_t n = sub32(S32(w, 0x78), off);
				dst = PADD(dst, shl32(off, 1));
				if (n > 0)
				{
					do
					{
						U16(dst, 0) = U16(src, 0);
						src += 2;
						dst += 2;
					} while (--n != 0);
					w = WS();
				}
				uint8_t *d2 = PTR(w, 0x60);
				if (off > 0)
				{
					do
					{
						U16(d2, 0) = U16(src, 0);
						src += 2;
						d2 += 2;
					} while (--off != 0);
					w = WS();
				}
				int32_t step = S32(w, 0x84);
				S32(w, 0x60) = add32(S32(w, 0x60), step);
				w = WS();
				S32(w, 0x64) = add32(S32(w, 0x64), step);
				w = WS();
				S32(w, 0x88) = add32(S32(w, 0x88), S32(w, 0x8C));
				w = WS();
				S32(w, 0x7C) = S32(w, 0x7C) - 1;
				w = WS();
			} while (S32(w, 0x7C) > 0);
		}
		x::QueueVramUpload(PTR(w, 0x68), PTR(w, 0x80));
	}

	static void __cdecl dh_18_WaterColumn()
	{
		if (U32(CUR(), 0xBC) == 0)
		{
			uint8_t *nb = blob::ArenaAlloc(0xA0);
			PTR(CUR(), 0xBC) = nb;
			uint8_t *blk0 = PTR(CUR(), 0xBC);
			uint8_t *arg = PTR(CUR(), 0xB8);
			uint16_t ref = U16(arg, 6);
			U16(blk0, 6) = 0x20;
			U16(blk0, 4) = ref;
			uint32_t a0 = U16(arg, 0);
			uint32_t a2 = U16(arg, 2);
			uint8_t *cur = CUR();
			uint32_t b = U8(cur, 0x92) & 0x60u;
			uint32_t e = (a0 >> 2) & 0x3F0u;
			uint16_t dx = (uint16_t)(e | (a2 & 0x100u));
			dx = (uint16_t)((int16_t)dx >> 4);
			dx = (uint16_t)(dx | b);
			dx = (uint16_t)(dx | U16(arg, 4));
			U16(cur, 0x92) = dx;
		}
		uint8_t *blk = PTR(CUR(), 0xBC);
		uint8_t *bone2 = blob::GetBone(S16(blk, 4));
		uint8_t *rad16 = blk + 0x60;
		PTR(WS(), 0x80) = PTR(CTX(), 0x74);
		S32(WS(), 0x60) = S16(CUR(), 0x90);
		S32(WS(), 0x64) = S16(bone2, 0x98);
		S32(WS(), 0x68) = shl32(S16(CUR(), 0x8C), 8);
		S32(WS(), 0x6C) = shl32(S16(CUR(), 0x8E), 4);
		S32(WS(), 0x70) = shl32(S16(bone2, 0x8C), 8);
		S32(WS(), 0x74) = S16(bone2, 0x8E);
		S32(WS(), 0x78) = S16(bone2, 0x90);
		S32(WS(), 0x84) = S16(CUR(), 0x92);
		S32(WS(), 0x88) = S16(CUR(), 0x9A);
		U32(WS(), 0x8C) = (U32(CUR(), 0xCC) & 0x2FFFFFFu) | 0x3C000000u;

		{
			uint8_t *w = WS();
			int32_t k60 = S32(w, 0x60);
			int32_t stepC = mul32(S32(w, 0x64), k60);
			uint8_t *a = PTR(w, 0x80);
			int32_t stepB = shl32(k60, 8);
			int32_t accB = 0, accC = 0;
			for (int i = 0; i < 32; i++)
			{
				int32_t c = DivPow2(accC, 5);
				U16(rad16, 2 * i) = (uint16_t)DivPow2(c, 8);
				S32(a, 4 * i) = c;
				S32(a, 0x500 + 4 * i) = DivPow2(accB, 5);
				S32(a, 0x80 + 4 * i) = 0;
				S32(a, 0x580 + 4 * i) = 0;
				accB = add32(accB, stepB);
				accC = add32(accC, stepC);
			}
		}
		for (int k = 1; k <= 3; k++)
		{
			int32_t ang = k * 0x100;
			uint8_t *p = PTR(WS(), 0x80);
			for (int i = 0; i < 32; i++)
			{
				int32_t r = S32(p, 0);
				int32_t v = x::Cos(ang);
				S32(p, 0x100 * k) = mul32(v >> 4, r) >> 16;
				v = x::Sin(ang);
				int32_t t = mul32(v >> 4, r);
				r = S32(p, 0x500);
				S32(p, 0x100 * k + 0x80) = t >> 16;
				v = x::Cos(ang);
				S32(p, 0x500 + 0x100 * k) = mul32(v >> 4, r) >> 16;
				v = x::Sin(ang);
				S32(p, 0x580 + 0x100 * k) = mul32(v >> 4, r) >> 16;
				p += 4;
			}
		}
		{
			uint8_t *p = PTR(WS(), 0x80);
			for (int i = 0; i < 32; i++)
			{
				int32_t v = S32(p, 0);
				S32(p, 0x400) = 0;
				S32(p, 0x900) = 0;
				v = DivPow2(v, 8);
				S32(p, 0) = v;
				S32(p, 0x480) = v;
				v = DivPow2(S32(p, 0x500), 8);
				S32(p, 0x500) = v;
				S32(p, 0x980) = v;
				p += 4;
			}
		}
		{
			uint8_t *w = WS();
			uint8_t *p = PTR(w, 0x80) + 0xA00;
			int32_t stepAmp = S32(w, 0x6C);
			int32_t amp = S32(w, 0x68);
			int32_t vel = S32(w, 0x74);
			uint8_t *h16 = blk + 0x20;
			int32_t phase = S32(w, 0x70);
			int32_t accel = S32(w, 0x78);
			for (int i = 0; i < 32; i++)
			{
				int32_t s = x::Sin(phase >> 4);
				int32_t v = mul32(s, amp >> 4) >> 20;
				p += 4;
				U16(h16, 0) = (uint16_t)v;
				S32(p, -4) = (int16_t)v;
				amp = add32(amp, stepAmp);
				vel = add32(vel, accel);
				h16 += 2;
				phase = add32(phase, vel);
			}
		}

		h_B27000();
		int32_t eu = S16(bone2, 0x94);
		int32_t ev = S16(bone2, 0x96);
		uint8_t *pkt = PTR(CTX(), 0x7C);
		uint8_t *A = PTR(WS(), 0x80);
		U16(WS(), 0xD0) = 0;

		uint8_t *dep = A + 0xB00;
		U16(WS(), 0xD4) = U16(A, 0);
		U16(WS(), 0xD2) = U16(A, 0xA00);
		int32_t otz;
		{
			uint8_t *w = WS();
			otz = xl_RotTransPers(w + 0xD0, pkt + 8, w + 0xFC, w + 0xFC);
		}
		U8(pkt, 0xC) = (uint8_t)(U8(A, 0x500) + (uint8_t)eu);
		U8(pkt, 0xD) = (uint8_t)(U8(A, 0x580) + (uint8_t)ev);
		U16(dep, 0) = (uint16_t)otz;
		dep += 2;
		for (int j = 1; j < 32; j++)
		{
			uint8_t *pk = pkt + 0x34 * j;
			U16(WS(), 0xD4) = U16(A, 4 * j);
			U16(WS(), 0xD2) = U16(A, 0xA00 + 4 * j);
			{
				uint8_t *w = WS();
				otz = xl_RotTransPers(w + 0xD0, pk + 8, w + 0xFC, w + 0xFC);
			}
			U32(pk, -0x20) = U32(pk, 8);
			uint8_t u = (uint8_t)(U8(A, 0x500 + 4 * j) + (uint8_t)eu);
			U8(pk, 0xC) = u;
			U8(pk, -0x1C) = u;
			uint8_t v = (uint8_t)(U8(A, 0x580 + 4 * j) + (uint8_t)ev);
			U8(pk, 0xD) = v;
			U8(pk, -0x1B) = v;
			U16(dep, 0) = (uint16_t)otz;
			dep += 2;
		}

		uint8_t *row = pkt + 0x680;
		const uint8_t *tbl = (const uint8_t *)(C().draw_table - 440);
		S32(WS(), 0x7C) = 1;
		while (S32(WS(), 0x7C) < 0x11)
		{
			int32_t r = S32(WS(), 0x7C);
			uint32_t t = tbl[r];
			uint8_t *prev = row - 0x680;
			A = PTR(WS(), 0x80);
			uint32_t xy = U32(prev, 8);
			U32(row, 8) = xy;
			U32(prev, 0x20) = xy;
			t &= 7;
			int16_t uv0 = S16(prev, 0xC);
			U16(row, 0xC) = (uint16_t)uv0;
			U16(prev, 0x24) = (uint16_t)uv0;
			U16(dep, 0) = U16(dep, -0x20);
			uint8_t *ring = A + (t << 8) + 4;
			uint8_t *hgt = A + 0xA00 + 4;
			uint32_t q = ((uint32_t)tbl[S32(WS(), 0x7C)] >> 6) & 3;
			dep += 2;
			uint8_t *pk = row + 0x34;
			for (int cnt = 0x1F; cnt != 0; cnt--)
			{
				uint16_t zx = U16(ring, 0);
				U16(WS(), 0xD4) = (q & 1) ? (uint16_t)(0u - zx) : zx;
				U16(WS(), 0xD2) = U16(hgt, 0);
				uint16_t xx = U16(ring, 0x80);
				U16(WS(), 0xD0) = (q & 2) ? (uint16_t)(0u - xx) : xx;
				{
					uint8_t *w = WS();
					otz = xl_RotTransPers(w + 0xD0, w + 0xF0, w + 0xFC, w + 0xFC);
				}
				uint32_t sxy = U32(WS(), 0xF0);
				U32(pk, -0x688) = sxy;
				U32(pk, -0x660) = sxy;
				U32(pk, -0x20) = sxy;
				U32(pk, 8) = sxy;
				int32_t sv = S16(ring, 0x580), su = S16(ring, 0x500);
				int32_t vv = (q & 2) ? sub32(ev, sv) : add32(sv, ev);
				int32_t uu = (q & 1) ? sub32(eu, su) : add32(su, eu);
				uint16_t uv = (uint16_t)(shl32(vv, 8) | uu);
				U16(pk, -0x684) = uv;
				U16(pk, -0x65C) = uv;
				U16(pk, -0x1C) = uv;
				U16(pk, 0xC) = uv;
				U16(dep, 0) = (uint16_t)otz;
				uint8_t *quad = pk - 0x6B4;
				U8(quad, 3) = 0xC;
				U16(quad, 0x1A) = U16(WS(), 0x84);
				U16(quad, 0xE) = U16(WS(), 0x88);
				uint32_t col = U32(WS(), 0x8C);
				U32(quad, 0x28) = col;
				U32(quad, 0x1C) = col;
				U32(quad, 0x10) = col;
				U32(quad, 4) = col;
				int32_t sum = add32(add32(add32(S16(dep, -0x20), otz), S16(dep, -0x22)), S16(dep, -2));
				uint32_t slot = ((uint32_t)DivPow2(sum, 2) & 0xFFFFFFFCu) + U32(RT(), 0x4C);
				x::InsertPrimAltViewport(slot, quad);
				pk += 0x34;
				ring += 4;
				hgt += 4;
				dep += 2;
			}
			row = pk;
			U32(WS(), 0x20) = 0;
			U32(WS(), 0x30) = 0;
			U32(WS(), 0x40) = 0;
			U32(WS(), 0x50) = 0;
			S32(WS(), 0x7C) = S32(WS(), 0x7C) + 1;
		}
		PTR(CTX(), 0x7C) = row - 0x680;
	}

	static void __cdecl pr_02_NormalLitG3()
	{
		U32(WS(), 0x84) = 0x32000000;
		uint8_t *ws = WS();
		uint32_t ot = U32(ws, 0x5C);
		uint8_t *pkt = PTR(ws, 0x60);
		int32_t count = S32(ws, 0x70);
		uint8_t *rec = PTR(ws, 0x6C);
		uint32_t nocull = U32(ws, 0x90) & 0x10;
		uint8_t *c0 = pkt + 4;
		uint8_t *c2 = pkt + 0x14;
		uint8_t *c1 = pkt + 0xC;
		U32(ws, 0xF0) = 0;
		do
		{
			uint8_t *vb = PTR(ws, 0x7C);
			uint32_t o1 = U16(rec, 0x10);
			uint32_t o0 = U16(rec, 0xE);
			uint32_t o2 = U16(rec, 0x12);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			uint8_t *p1 = vb + o1;
			uint8_t *p0 = vb + o0;
			uint8_t *p2 = vb + o2;
			uint32_t s0 = U32(p0, 0);
			uint32_t s2 = U32(p2, 0);
			uint32_t s1 = U32(p1, 0);
			U32(pkt, 0x10) = s1;
			U32(pkt, 8) = s0;
			U32(pkt, 0x18) = s2;
			if (nocull == 0)
			{
				int32_t mac0;
				x::GteWriteData((int32_t)s0, 12);
				x::GteWriteData((int32_t)s1, 13);
				x::GteWriteData((int32_t)s2, 14);
				x::GteNCLIP();
				x::GteReadData(&mac0, 24);
				if (mac0 < 0) goto next;
			}
			{
				uint32_t a0 = U32(p0, 4);
				uint32_t a1 = U32(p1, 4);
				uint32_t a2 = U32(p2, 4);
				if (((a2 | a1 | a0) >> 16) & 0x46) goto next;
				int32_t sum = (int32_t)((a2 + a1 + a0) & 0xFFFF);
				U8(pkt, 3) = 6;
				uint8_t *n;
				x::GteWriteData((int32_t)(U32(rec, 0) | U32(ws, 0x84)), 6);
				n = PTR(ws, 0x78) + U16(rec, 0xC);
				x::GteWriteData2(0, (int32_t)U32(n, 0));
				x::GteWriteData2(1, (int32_t)U32(n, 4));
				x::Gte_4601B0();
				x::GteReadData2(0x16, c0);
				x::GteWriteData((int32_t)(U32(rec, 4) | U32(ws, 0x84)), 6);
				n = PTR(ws, 0x78) + U16(rec, 0xC);
				x::GteWriteData2(0, (int32_t)U32(n, 0));
				x::GteWriteData2(1, (int32_t)U32(n, 4));
				x::Gte_4601B0();
				x::GteReadData2(0x16, c1);
				x::GteWriteData((int32_t)(U32(rec, 8) | U32(ws, 0x84)), 6);
				n = PTR(ws, 0x78) + U16(rec, 0xC);
				x::GteWriteData2(0, (int32_t)U32(n, 0));
				x::GteWriteData2(1, (int32_t)U32(n, 4));
				x::Gte_4601B0();
				x::GteReadData2(0x16, c2);
				uint32_t slot = ((uint32_t)Div3(sum) & 0x3FFC) + ot;
				x::InsertPrimDepthKeys(slot, pkt, z0, z1, z2, 0);
				c2 += 0x1C;
				pkt += 0x1C;
				c0 += 0x1C;
				c1 += 0x1C;
				U32(pkt, 0) = 0x2000000;
				U32(c0, 0) = U32(ws, 0x50);
				U32(pkt, 8) = 0;
				x::InsertPrimDepthKeys(slot, pkt, z0, z1, z2, 0);
				c1 += 0xC;
				pkt += 0xC;
				c2 += 0xC;
				c0 += 0xC;
			}
		next:
			rec += 0x14;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}

	static void __cdecl op_04F_OffscreenPass()
	{
		if (U16(RCTX(), 0x30) != 0)
		{
			STREAM() += 8;
			return;
		}
		U16(WS(), 0xD4) = U16(STREAM(), 4);
		U16(WS(), 0xD6) = U16(STREAM(), 6);
		uint8_t *blk = PTR(CTX(), 0x74);
		uint32_t size = 0xEC;
		U32(blk, 0) = 0;
		U32(blk, 4) = 0;
		int32_t flags = S16(STREAM(), 2);
		if (flags & 1)
		{
			uint32_t p = U32(CTX(), 0x74) + 0xEC;
			size = 0x40EC;
			U32(blk, 4) = p;
		}
		if (flags & 2)
		{
			uint32_t p = U32(CTX(), 0x74) + size;
			size += 0x1000;
			U32(blk, 0) = p;
		}
		{
			uint32_t p = U32(CTX(), 0x74) + size;
			size += 0x5DC0;
			U32(blk, 8) = p;
			U32(blk, 0xC) = size;
			U32(CUR(), 0xC4) = size;
		}
		blob::ArenaAlloc((int32_t)U32(blk, 0xC));
		U32(blk, 0x14) = U32(CTX(), 0x74);
		h_B27000();
		{
			uint8_t *pos = WS() + 0xF0;
			U32(pos, 0) = U32(CUR(), 0x94);
			U16(pos, 4) = U16(CUR(), 0x98);
		}
		{
			uint8_t *w = WS();
			xl_RotTransPers(w + 0xF0, w + 0xD0, w + 0xFC, w + 0xFC);
		}
		PTR(WS(), 0xF0) = blk;
		xl_OffscreenRender_B65810();
		STREAM() += 8;
	}
}

	void apply_lev_a(Clone &c)
	{
		c.draw[17] = part_lev_a::dh_17_RippleTextureUpload;
		c.draw[18] = part_lev_a::dh_18_WaterColumn;
		c.prim[2] = part_lev_a::pr_02_NormalLitG3;
		c.vm[0x04F] = part_lev_a::op_04F_OffscreenPass;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace part_lev_b
{
	static int32_t xl_NormalClip_56CB30(int32_t a, int32_t b, int32_t c)
	{
		return x::f<int32_t (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_NormalClip3))(a, b, c);
	}

	static void MorphTableBuild_B5A760()
	{
		uint8_t *out = PTR(CTX(), 0x74);
		uint8_t *w = WS();
		int32_t num = S32(w, 0xF8);
		int32_t den = S32(w, 0xFC);
		U16(w, 0xD0) = U16(PTR(w, 0xF0), 0);
		U16(WS(), 0xD2) = 0;
		w = WS();
		if (S16(w, 0xD0) > S16(w, 0xD2))
		{
			do
			{
				int32_t i4 = shl32(S16(w, 0xD2), 2);
				uint8_t *ga = PTR(w, 0xF0);
				uint8_t *a = ga + U32(ga, i4 + 4);
				a += 2;
				uint8_t *gb = PTR(w, 0xF4);
				uint8_t *b = gb + U32(gb, i4 + 4);
				U16(w, 0xD4) = U16(a, -2);
				w = WS();
				b += 2;
				if (S16(w, 0xD4) > 0)
				{
					do
					{
						int32_t n = S16(a, 2);
						a += 4;
						b += 4;
						if (n > 0)
						{
							do
							{
								U16(out, 0) = U16(a, 0);
								a += 6;
								U16(out, 2) = U16(a, -4);
								U16(out, 4) = U16(a, -2);
								out += 0xC;
								S16(out, -6) = (int16_t)(mul32(S16(b, 0), num) / den);
								b += 6;
								S16(out, -4) = (int16_t)(mul32(S16(b, -4), num) / den);
								S16(out, -2) = (int16_t)(mul32(S16(b, -2), num) / den);
							} while (--n);
							w = WS();
						}
						U16(w, 0xD4) = (uint16_t)(U16(w, 0xD4) - 1);
						w = WS();
					} while (S16(w, 0xD4) > 0);
				}
				U16(w, 0xD2) = (uint16_t)(U16(w, 0xD2) + 1);
				w = WS();
			} while (S16(w, 0xD0) > S16(w, 0xD2));
		}
		int32_t size = (int32_t)((uint32_t)out - U32(CTX(), 0x74));
		uint8_t *t = blob::ArenaAlloc(size);
		PTR(WS(), 0xFC) = t;
		S32(WS(), 0xF8) = size;
	}

	static inline int32_t div256(int32_t v) { return add32(v, (v >> 31) & 0xFF) >> 8; }

	static void MorphApply_B5A8E0()
	{
		uint8_t *w = WS();
		int32_t wx = S32(w, 0xE0);
		uint8_t *t = PTR(w, 0xF8);
		int32_t wy = S32(w, 0xE4);
		int32_t wz = S32(w, 0xE8);
		U16(w, 0xD0) = U16(PTR(w, 0xF0), 0);
		U16(WS(), 0xD2) = 0;
		w = WS();
		int16_t si = S16(w, 0xD2);
		if (S16(w, 0xD0) > si)
		{
			do
			{
				uint8_t *g = PTR(w, 0xF0);
				uint8_t *v = g + U32(g, (int32_t)si * 4 + 4);
				U16(w, 0xD4) = U16(v, 0);
				v += 2;
				w = WS();
				if (S16(w, 0xD4) > 0)
				{
					do
					{
						int32_t n = S16(v, 2);
						v += 4;
						if (n > 0)
						{
							do
							{
								int32_t a = S16(t, 0);
								int32_t d = S16(t, 6) - a;
								v += 6;
								d = mul32(d, wx);
								t += 0xC;
								S16(v, -6) = (int16_t)add32(div256(d), a);
								a = S16(t, -0xA);
								d = mul32(S16(t, -4) - a, wy);
								S16(v, -4) = (int16_t)add32(div256(d), a);
								a = S16(t, -8);
								d = mul32(S16(t, -2) - a, wz);
								S16(v, -2) = (int16_t)add32(div256(d), a);
							} while (--n);
							w = WS();
						}
						U16(w, 0xD4) = (uint16_t)(U16(w, 0xD4) - 1);
						w = WS();
					} while (S16(w, 0xD4) > 0);
				}
				U16(w, 0xD2) = (uint16_t)(U16(w, 0xD2) + 1);
				w = WS();
				si = S16(w, 0xD2);
			} while (S16(w, 0xD0) > si);
		}
	}

	static void ProjectVertexGroup_B5B010(uint8_t *p, uint32_t cursor)
	{
		U32(WS(), 0xF0) = U16(p, 0);
		PTR(WS(), 0xF4) = p + 2;
		U32(WS(), 0xF8) = cursor;
		uint8_t *w = WS();
		int32_t cnt = S32(w, 0xF0);
		uint8_t *v = PTR(w, 0xF4);
		uint8_t *o = PTR(w, 0xF8);
		int32_t tmp = (int32_t)(((uint32_t)U16(v, 2) << 16) | U16(v, 0));
		int32_t z = U16(v, 4);
		x::GteWriteData(tmp, 0);
		x::GteWriteData(z, 1);
		x::GteRTPS();
		int32_t left = cnt - 1;
		v += 6;
		x::GteReadData2(0xE, o);
		x::GteReadData(&tmp, 0x13);
		o += 8;
		uint32_t *dp = DEPTHARR() + 1;
		DEPTHARR()[0] = (uint32_t)tmp & 0xFFFF;
		S32(o, 4) = tmp;
		if (left > 0)
		{
			do
			{
				tmp = (int32_t)(((uint32_t)U16(v, 2) << 16) | U16(v, 0));
				z = U16(v, 4);
				x::GteWriteData(tmp, 0);
				x::GteWriteData(z, 1);
				x::GteRTPS();
				x::GteReadData2(0xE, o);
				v += 6;
				x::GteReadData(&tmp, 0x13);
				o += 8;
				*dp = (uint32_t)tmp & 0xFFFF;
				S32(o, 4) = tmp;
				dp++;
			} while (--left);
		}
		PTR(w, 0xFC) = o;
		PTR(w, 0xF8) = v;
	}

	static void DrawScreenTexturedPrims_B5B170()
	{
		uint8_t *w = WS();
		uint32_t pk0 = U32(CTX(), 0x7C);
		uint8_t *b40 = PTR(w, 0x84) + 0x40;
		U32(w, 0xF4) = pk0;
		U32(WS(), 0xFC) = U8(CUR(), 0xDE);
		U32(WS(), 0xE0) = U32(RT(), 0x38);
		U32(WS(), 0xE8) = (U32(CUR(), 0xCC) & 0x2FFFFFFu) | 0x24000000u;
		U16(WS(), 0xD0) = U16(b40, 8);
		U16(WS(), 0xD2) = U16(b40, 0xA);
		uint8_t *rec = PTR(b40, 0);
		uint8_t *pk = PTR(CTX(), 0x7C);
		w = WS();
		int32_t ntri = S16(w, 0xD0);
		if (ntri > 0)
		{
			int32_t left = ntri;
			do
			{
				uint32_t o2 = ((uint32_t)U16(rec, 4) & 0xFFF) << 3;
				uint8_t *base = PTR(w, 0x68);
				uint32_t o1 = ((uint32_t)U16(rec, 2) & 0xFFF) << 3;
				uint32_t o0 = ((uint32_t)U16(rec, 0) & 0xFFF) << 3;
				int32_t s2 = S32(base, o2);
				int32_t s1 = S32(base, o1);
				int32_t s0 = S32(base, o0);
				if (xl_NormalClip_56CB30(s0, s1, s2) >= 0)
				{
					uint8_t *p2 = pk + 0x30;
					U8(p2, 3) = 7;
					U32(pk, 8) = 0;
					U32(pk, 0x2C) = 0;
					U8(pk, 3) = 0xB;
					U32(pk, 0x28) = 0xE2000000u;
					S32(pk, 0x10) = s0;
					S32(p2, 8) = s0;
					S32(pk, 0x18) = s1;
					S32(p2, 0x10) = s1;
					S32(pk, 0x20) = s2;
					S32(p2, 0x18) = s2;
					int32_t z0 = S16(base, o0 + 4);
					int32_t z1 = S16(base, o1 + 4);
					int32_t z2 = S16(base, o2 + 4);
					int32_t sum = z2 + z1 + z0;
					int32_t q = (int32_t)(((int64_t)sum * 0x55555556LL) >> 32);
					q += (int32_t)((uint32_t)q >> 31);
					q >>= 4;
					uint8_t *b2 = PTR(WS(), 0x6C);
					U8(pk, 0x14) = U8(b2 + o0, 0);
					U8(pk, 0x15) = U8(b2 + o0, 2);
					U8(pk, 0x1C) = U8(b2 + o1, 0);
					U8(pk, 0x1D) = U8(b2 + o1, 2);
					U8(pk, 0x24) = U8(b2 + o2, 0);
					U8(pk, 0x25) = U8(b2 + o2, 2);
					U16(p2, 0xC) = U16(rec, 8);
					U16(p2, 0x14) = U16(rec, 0xC);
					U16(p2, 0x1C) = U16(rec, 6);
					U16(p2, 0x16) = U16(rec, 0xE);
					U16(p2, 0xE) = U16(rec, 0xA);
					U32(p2, 4) = U32(WS(), 0xE8);
					U16(pk, 0x1E) = U16(WS(), 0x78);
					U16(pk, 0x16) = U16(WS(), 0x7C);
					U32(pk, 0xC) = U32(WS(), 0x80) | 0x24000000u;
					U32(pk, 4) = U32(WS(), 0x88);
					uint32_t d4 = (uint32_t)shl32(q, 2);
					x::InsertPrimDepthKeys(U32(RT(), 0x4C) + d4, pk, z0, z1, z2, 0);
					x::InsertPrimDepthKeys(U32(RT(), 0x4C) + d4, p2, z0, z1, z2, 0);
					pk = p2 + 0x20;
				}
				rec += 0x10;
				w = WS();
			} while (--left);
		}
		U32(w, 0xE8) = (U32(w, 0xE8) & 0x2FFFFFFu) | 0x2C000000u;
		w = WS();
		int32_t nq = S16(w, 0xD2);
		if (nq <= 0)
		{
			PTR(CTX(), 0x7C) = pk;
			return;
		}
		rec += 4;
		int32_t left = nq;
		do
		{
			w = WS();
			uint32_t o2 = ((uint32_t)U16(rec, 0) & 0xFFF) << 3;
			uint32_t o1 = ((uint32_t)U16(rec, -2) & 0xFFF) << 3;
			uint8_t *base = PTR(w, 0x68);
			uint32_t o0 = ((uint32_t)U16(rec, -4) & 0xFFF) << 3;
			int32_t s2 = S32(base, o2);
			int32_t s1 = S32(base, o1);
			int32_t s0 = S32(base, o0);
			if (xl_NormalClip_56CB30(s0, s1, s2) >= 0)
			{
				uint8_t *p2 = pk + 0x38;
				U8(p2, 3) = 9;
				U32(pk, 8) = 0;
				U32(pk, 0x34) = 0;
				U8(pk, 3) = 0xD;
				U32(pk, 0x30) = 0xE2000000u;
				uint32_t o3 = ((uint32_t)U16(rec, 2) & 0xFFF) << 3;
				int32_t s3 = S32(base, o3);
				S32(pk, 0x10) = s0;
				S32(p2, 8) = s0;
				S32(pk, 0x18) = s1;
				S32(p2, 0x10) = s1;
				S32(pk, 0x20) = s2;
				S32(p2, 0x18) = s2;
				S32(pk, 0x28) = s3;
				S32(p2, 0x20) = s3;
				int32_t z0 = S16(base, o0 + 4);
				int32_t z1 = S16(base, o1 + 4);
				int32_t z2 = S16(base, o2 + 4);
				int32_t z3 = S16(base, o3 + 4);
				int32_t sum = z3 + z2 + z1 + z0;
				int32_t q = (sum + ((sum >> 31) & 3)) >> 2;
				q >>= 4;
				uint8_t *b2 = PTR(WS(), 0x6C);
				U8(pk, 0x14) = U8(b2 + o0, 0);
				U8(pk, 0x15) = U8(b2 + o0, 2);
				U8(pk, 0x1C) = U8(b2 + o1, 0);
				U8(pk, 0x1D) = U8(b2 + o1, 2);
				U8(pk, 0x24) = U8(b2 + o2, 0);
				U8(pk, 0x25) = U8(b2 + o2, 2);
				U8(pk, 0x2C) = U8(b2 + o3, 0);
				U8(pk, 0x2D) = U8(b2 + o3, 2);
				U16(p2, 0xC) = U16(rec, 4);
				U16(p2, 0x14) = U16(rec, 8);
				U16(p2, 0x1C) = U16(rec, 0xC);
				U16(p2, 0x24) = U16(rec, 0xE);
				U16(p2, 0x16) = U16(rec, 0xA);
				U16(p2, 0xE) = U16(rec, 6);
				U32(p2, 4) = U32(WS(), 0xE8);
				U16(pk, 0x1E) = U16(WS(), 0x78);
				U16(pk, 0x16) = U16(WS(), 0x7C);
				U32(pk, 0xC) = U32(WS(), 0x80) | 0x2C000000u;
				U32(pk, 4) = U32(WS(), 0x88);
				uint32_t d4 = (uint32_t)shl32(q, 2);
				x::InsertPrimDepthKeys(U32(RT(), 0x4C) + d4, pk, z0, z1, z2, z3);
				x::InsertPrimDepthKeys(U32(RT(), 0x4C) + d4, p2, z0, z1, z2, z3);
				pk = p2 + 0x28;
			}
			rec += 0x14;
		} while (--left);
		PTR(CTX(), 0x7C) = pk;
	}

	static void BuildPose(uint8_t *blk)
	{
		x::BuildBoneMatricesFromPose(blk + 0x10);
	}

	static void DrawModelScreenTextured_B5AD50()
	{
		U32(WS(), 0x84) = U32(WS(), 0xF0);
		uint8_t *w = WS();
		int32_t ref = S32(w, 0xF4);
		uint8_t *blk = PTR(w, 0x84);
		uint8_t *bone2 = blob::GetBone(ref);
		blob::ReadAltTexture(S32(WS(), 0xF8));
		w = WS();
		uint8_t *r = PTR(w, 0xF0);
		int32_t e = shl32(S16(r, 2), 5) | S16(r, 0);
		uint32_t ebx = (S16(r, 0) < 0 ? 0xFFFF0000u : 0u) | U16(r, 4);
		uint32_t mx = (uint32_t)((int32_t)(~ebx + 1u) >> 3) & 0x1Fu;
		uint32_t ecx = ((uint32_t)r & 0xFFFF0000u) | U16(r, 6);
		uint32_t my = ((~ecx) * 4u + 4u) & 0x3E0u;
		e = shl32(e, 7);
		U32(w, 0x88) = my | ((uint32_t)e | mx) | 0xE2000000u;
		S32(WS(), 0x78) = S16(bone2, 0x92);
		S32(WS(), 0x7C) = S16(bone2, 0x9A);
		U32(WS(), 0x80) = U32(bone2, 0xCC);
		S32(WS(), 0x70) = S16(CUR(), 0x92);
		S32(WS(), 0x74) = S16(CUR(), 0x9A);
		U32(WS(), 0x68) = U32(CTX(), 0x74);
		uint8_t *bones = PTR(blk, 0x30) + 0x10;
		uint8_t *tab = PTR(blk, 0x34);
		int32_t nobj = S32(tab, 0);
		tab += 4;
		if (nobj <= 0) return;
		int32_t left = nobj;
		do
		{
			h_B269E0();
			BuildPose(blk);
			x::ComputeBonesWorldMatrices(blk + 0x10, WS() + 0xE0);
			uint8_t *obj = PTR(blk, 0x34) + U32(tab, 0);
			tab += 4;
			PTR(WS(), 0x8C) = obj;
			uint8_t *p = obj + 2;
			uint32_t cursor = U32(WS(), 0x68);
			int32_t ng = S16(p, -2);
			if (ng > 0)
			{
				do
				{
					uint8_t *m = bones + S16(p, 0) * 0x30 + 0x10;
					x::SetRotMatrix(m);
					x::SetTransVector(m);
					p += 2;
					ProjectVertexGroup_B5B010(p, cursor);
					cursor = U32(WS(), 0xFC);
					p = PTR(WS(), 0xF8);
				} while (--ng);
			}
			U32(WS(), 0x6C) = U32(WS(), 0xFC);
			uint8_t *save = CUR();
			CUR() = bone2;
			h_B269E0();
			CUR() = save;
			BuildPose(blk);
			x::ComputeBonesWorldMatrices(blk + 0x10, WS() + 0xE0);
			p = PTR(WS(), 0x8C);
			cursor = U32(WS(), 0x6C);
			ng = S16(p, 0);
			p += 2;
			if (ng > 0)
			{
				do
				{
					uint8_t *m = bones + S16(p, 0) * 0x30 + 0x10;
					x::SetRotMatrix(m);
					x::SetTransVector(m);
					p += 2;
					ProjectVertexGroup_B5B010(p, cursor);
					p = PTR(WS(), 0xF8);
					cursor = U32(WS(), 0xFC);
				} while (--ng);
			}
			uint8_t *q = (uint8_t *)(((uint32_t)p + 3) & ~3u);
			U16(blk, 0x48) = U16(q, 0);
			U16(blk, 0x4A) = U16(q, 2);
			U16(blk, 0x4C) = U16(q, 4);
			U16(blk, 0x4E) = U16(q, 6);
			PTR(blk, 0x40) = q + 0xC;
			DrawScreenTexturedPrims_B5B170();
		} while (--left);
	}

	static void __cdecl dh_20_MorphAnimModel()
	{
		uint8_t *cur = CUR();
		if (PTR(cur, 0xBC) == nullptr)
		{
			uint8_t *prm = PTR(cur, 0xB8);
			uint8_t *nb = h_B26860(S16(prm, 0));
			PTR(CUR(), 0xBC) = nb;
			uint8_t *blk = PTR(CUR(), 0xBC);
			uint8_t *obj = blob::ObjectPtr(S16(prm, 2));
			PTR(WS(), 0xF4) = obj + U32(obj, 8);
			U32(WS(), 0xF0) = U32(blk, 0x34);
			S32(WS(), 0xF8) = S16(prm, 4);
			S32(WS(), 0xFC) = S16(prm, 6);
			MorphTableBuild_B5A760();
			U32(blk, 4) = U32(WS(), 0xFC);
			cur = CUR();
		}
		uint8_t *blk = PTR(cur, 0xBC);
		uint8_t *hdr = blk + 0x10;
		uint8_t *cmd = blk + 0x20;
		if (!(U8(blk, 2) & 1))
		{
			if (U16(blk, 0) != 0)
			{
				{
					int32_t r = x::ReadAnimation(hdr, cmd);
					U16(blk, 0) = (uint16_t)(U16(blk, 0) + 1);
					if (r == 1) x::PreReadAnimation(hdr, cmd, 0);
				}
			}
			if (!(U8(blk, 2) & 1))
			{
				h_B269E0();
				x::ComputeBonesWorldMatrices(hdr, WS() + 0xE0);
			}
		}
		uint8_t *b40 = blk + 0x40;
		uint8_t *b30 = blk + 0x30;
		U32(b40, 4) = U32(CTX(), 0x74);
		uint8_t *wb = blob::GetBone(S16(PTR(CUR(), 0xB8), 8));
		U32(WS(), 0xF0) = U32(b30, 4);
		U32(WS(), 0xF8) = U32(blk, 4);
		S32(WS(), 0xE0) = S16(wb, 0x94);
		S32(WS(), 0xE4) = S16(wb, 0x96);
		S32(WS(), 0xE8) = S16(wb, 0x98);
		MorphApply_B5A8E0();
		uint32_t cursor = U32(CTX(), 0x7C);
		uint32_t ot = U32(RT(), 0x4C);
		U32(CTX(), 0x7C) = x::RenderGeometry(b30, b40, ot, 4, cursor);
	}

	static void __cdecl dh_24_AnimModelAltTex()
	{
		uint8_t *cur = CUR();
		if (PTR(cur, 0xBC) == nullptr)
		{
			uint8_t *prm = PTR(cur, 0xB8);
			uint8_t *nb = h_B26860(S16(prm, 0));
			PTR(CUR(), 0xBC) = nb;
			blob::ReadAltTexture(S16(prm, 2));
			U32(CUR(), 0xC0) = U32(WS(), 0xF0);
			cur = CUR();
			uint8_t *mp = PTR(cur, 0xC4);
			if (mp)
			{
				uint8_t *blk = PTR(cur, 0xBC);
				uint8_t *o = blob::ObjectPtr(S16(mp, 2));
				PTR(WS(), 0xF4) = o + U32(o, 8);
				o = blob::ObjectPtr(S16(mp, 0));
				PTR(WS(), 0xF0) = o + U32(o, 8);
				S32(WS(), 0xF8) = S16(mp, 4);
				S32(WS(), 0xFC) = S16(mp, 6);
				MorphTableBuild_B5A760();
				U32(blk, 4) = U32(WS(), 0xFC);
				cur = CUR();
			}
		}
		uint8_t *mp = PTR(cur, 0xC4);
		if (mp)
		{
			uint8_t *mb = PTR(cur, 0xBC);
			uint8_t *wb = blob::GetBone(S16(mp, 8));
			uint8_t *o = blob::ObjectPtr(S16(mp, 0));
			PTR(WS(), 0xF0) = o + U32(o, 8);
			U32(WS(), 0xF8) = U32(mb, 4);
			S32(WS(), 0xE0) = S16(wb, 0x94);
			S32(WS(), 0xE4) = S16(wb, 0x96);
			S32(WS(), 0xE8) = S16(wb, 0x98);
			MorphApply_B5A8E0();
		}
		h_B269E0();
		h_B26AD0(PTR(CUR(), 0xBC));
		cur = CUR();
		uint8_t *blk = PTR(cur, 0xBC);
		uint8_t *prm = PTR(cur, 0xB8);
		PTR(WS(), 0xF0) = blk;
		S32(WS(), 0xF4) = S16(prm, 4);
		S32(WS(), 0xF8) = S16(prm, 2);
		DrawModelScreenTextured_B5AD50();
		U16(blk, 0) = (uint16_t)(U16(blk, 0) + 1);
	}
}

	void apply_lev_b(Clone &c)
	{
		c.draw[20] = part_lev_b::dh_20_MorphAnimModel;
		c.draw[24] = part_lev_b::dh_24_AnimModelAltTex;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace leviathan
{
	static Clone g_c = {};

	static void describe(Clone &c)
	{
		c.name = "006 Leviathan";
		c.effect_id = 6;
		c.vm_table = game_addr(game::GF_006Leviathan_VmOpcodeTable);
		c.bone_table = game_addr(game::GF_006Leviathan_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_B5C6B4);
		c.prim_size = game_addr(game::unk_187760C);
		c.spr_table = game_addr(game::unk_1877620);
		c.ptype_table = game_addr(game::unk_187764C);
		c.pop_table = game_addr(game::funcs_B5E9F7);
		c.draw_table = game_addr(game::GF_006Leviathan_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_187782A);
		c.attr_184 = game_addr(game::unk_1877844);
		c.attr_186 = game_addr(game::unk_1877846);
		c.attr_194 = game_addr(game::unk_1877854);
		c.attr_19C = game_addr(game::unk_187785C);
		c.desc = game_addr(game::unk_187726C);
		c.ptr_block = game_addr(game::unk_18774AC);
		c.queue = game_addr(game::unk_2796EF8);
		c.file00 = game_addr(game::unk_2796F0C);
		c.file01 = game_addr(game::unk_2796F08);
		c.tick = game_addr(game::GF_006Leviathan_SequenceTick);
		c.load_cb = game_addr(game::MAG_006_sub_B60530);
		c.lit_dispatcher = true;
		c.bone_count = 18;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_gfc_leviathan()
	{
		using namespace gfc;
		leviathan::describe(leviathan::g_c);
		init_clone(leviathan::g_c, nullptr, 0);
		apply_lev_a(leviathan::g_c);
		apply_lev_b(leviathan::g_c);
		apply_shared_misc(leviathan::g_c);
		register_port(game::GF_006Leviathan_SequenceTick, (void *)leviathan::SequenceTask, "GFC006 Leviathan SequenceTick", 6);
	}
}

