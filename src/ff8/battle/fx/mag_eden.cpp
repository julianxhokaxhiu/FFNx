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

// Effect 206: Eden

#include "gfc_engine.h"

namespace ff8fx
{
namespace gfc
{
namespace part_eden_a
{
	static inline uint32_t xl_ColourScale_B66A20(uint32_t col, int32_t f) { return x::f<uint32_t (__cdecl *)(uint32_t, int32_t)>(game_addr(game::GfCinematic_ScaleColourGPF))(col, f); }
	static inline void xl_RotTransIR_56C850(const void *in, void *out, void *flag) { x::f<void (__cdecl *)(const void *, void *, void *)>(game_addr(game::GTE_RotTrans))(in, out, flag); }

	static inline uint16_t GTE_SZ3_LO() { return MEM<uint16_t>(game_addr(game::GTE_SZ3)); }
	static inline uint8_t *ZEROVEC() { return (uint8_t *)game_addr(game::unk_27971E4); }
	static inline const uint8_t *FADETAB() { return (const uint8_t *)(C().draw_table - 0x1C4); }

	static inline uint32_t LineOT(int32_t za, int32_t zb)
	{
		int32_t s = za + zb;
		return (uint32_t)(((s / 2) >> 2) & ~3) + U32(RT(), 0x4C);
	}

	static inline void EmitLineF2Ws(uint8_t *p, const uint8_t *v)
	{
		U8(p, 3) = 3;
		U32(p, 4) = U32(WS(), 0xF4);
		U32(p, 8) = U32(v, 0);
		U32(p, 0xC) = U32(v, 8);
		int32_t zb = U16(v, 0xC);
		int32_t za = U16(v, 4);
		x::InsertPrimDepthKeys(LineOT(za, zb), p, za, zb, 0, 0);
	}

	static inline void ProjectSV(const uint8_t *sv, uint8_t *out)
	{
		uint32_t y = U16(sv, 2);
		uint32_t z = U16(sv, 4);
		uint32_t xx = U16(sv, 0);
		x::GteWriteData((int32_t)((y << 16) | xx), 0);
		x::GteWriteData((int32_t)z, 1);
		x::GteRTPS();
		x::GteReadSXY2(out);
		U16(out, 4) = GTE_SZ3_LO();
	}

	static uint32_t FadeColour(uint32_t col, int32_t t, int32_t fv, bool frac)
	{
		return xl_ColourScale_B66A20(col, mul32(t, fv));
	}

	static int32_t LoadParentMatrixScaled_AE6000(const void *v, int32_t scale)
	{
		Mat4x3 *m = blob::GetParentMatrix((int32_t)U16(CUR(), 0x9C));
		x::SetRotMatrix(m);
		x::SetTransVector(m);
		x::TransformToCamera(v, WS() + 0xF4, WS() + 0xD4);
		uint8_t *d = WS() + 0xE0;
		U32(d, 0) = U32(m, 0);
		U32(d, 4) = U32(m, 4);
		U32(d, 8) = U32(m, 8);
		U32(d, 0xC) = U32(m, 0xC);
		U32(d, 0x10) = U32(m, 0x10);
		S32(WS(), 0xDC) = scale;
		S32(WS(), 0xD8) = scale;
		S32(WS(), 0xD4) = scale;
		x::ScaleMatrix(d, WS() + 0xD4);
		return 0;
	}

	static void FlatGridFadeIn_AE5AD0(uint8_t *blk, uint8_t *desc)
	{
		uint8_t *F = PTR(blk, 4);
		{
			int32_t n = S16(CUR(), 0x98);
			if (n >= 0x10) n = 0x10;
			if (n >= 1)
			{
				int32_t i = 1;
				uint8_t *e = F + 0xF;
				do
				{
					if (F[i + 0x10] == 0xFF) F[i + 0x10] = 0;
					if (*e == 0xFF) *e = 0;
					i++;
					e--;
				} while (i <= n);
				if (F[0x10] == 0xFF) F[0x10] = 0;
			}
			n = S16(CUR(), 0x94);
			if (n >= 0x20) n = 0x20;
			if (n >= 1)
			{
				int32_t i = 1;
				uint8_t *e = F + 0x5F;
				do
				{
					if (F[i + 0x60] == 0xFF) F[i + 0x60] = 0;
					if (*e == 0xFF) *e = 0;
					i++;
					e--;
				} while (i <= n);
				if (F[0x60] == 0xFF) F[0x60] = 0;
			}
			for (int32_t k = 0; k < 0x30; k++)
			{
				int32_t c = S8(F, k);
				if (c >= 0) c++;
				if (c >= 0x10) c = 0x10;
				F[k] = (uint8_t)c;
				c = S8(F, 0x40 + k);
				if (c >= 0) c++;
				if (c >= 0x10) c = 0x10;
				F[0x40 + k] = (uint8_t)c;
			}
		}

		LoadParentMatrixScaled_AE6000(ZEROVEC(), 0x1000);
		const uint8_t *tab = FADETAB();

		uint8_t *vo = PTR(CTX(), 0x74);
		uint8_t *sv = WS() + 0xC0;
		S32(WS(), 0xF0) = shl32(-(int32_t)U16(desc, 4), 5);
		U16(WS(), 0xC4) = (uint16_t)(U16(desc, 6) * 0xFFF0u);
		U16(WS(), 0xC2) = 0;
		int32_t zstep = shl32((int32_t)U16(desc, 4), 2);
		for (int32_t j = 0; j <= 0x20; j++)
		{
			U16(WS(), 0xC0) = (uint16_t)((uint32_t)(j - 0xF) * U16(desc, 6));
			int32_t z = shl32(-(int32_t)U16(desc, 4), 5);
			for (int32_t k = 13; k; k--)
			{
				U16(sv, 4) = (uint16_t)z;
				ProjectSV(sv, vo);
				z = add32(z, zstep);
				vo += 8;
			}
		}

		uint8_t *p = PTR(CTX(), 0x7C);
		uint8_t *v = PTR(CTX(), 0x74);
		for (int32_t i = 0; i < 0x20; i++)
		{
			int32_t fv = S8(F, i);
			if (fv == -1)
			{
				v += 0x68;
				continue;
			}
			U32(WS(), 0xF4) = FadeColour(U32(WS(), 0x60), (int32_t)tab[i], fv, true);
			for (int32_t k = 12; k; k--)
			{
				EmitLineF2Ws(p, v);
				p += 0x10;
				v += 8;
			}
			v += 8;
		}

		vo = PTR(CTX(), 0x74);
		sv = WS() + 0xC0;
		S32(WS(), 0xF0) = shl32(-(int32_t)U16(desc, 4), 5);
		U16(WS(), 0xC4) = (uint16_t)(U16(desc, 6) * 0xFFF0u);
		U16(WS(), 0xC2) = 0;
		int32_t xstep = shl32((int32_t)U16(desc, 6), 2);
		for (int32_t j = 0; j <= 0x30; j++)
		{
			U16(WS(), 0xC4) = (uint16_t)((uint32_t)(j - 0x1F) * U16(desc, 4));
			int32_t xx = shl32(-(int32_t)U16(desc, 6), 4);
			for (int32_t k = 9; k; k--)
			{
				U16(sv, 0) = (uint16_t)xx;
				ProjectSV(sv, vo);
				xx = add32(xx, xstep);
				vo += 8;
			}
		}
		U32(CTX(), 0x7C) = (uint32_t)p;

		uint8_t *q = PTR(CTX(), 0x7C);
		uint8_t *vb = PTR(CTX(), 0x74);
		for (int32_t i = 0; i < 0x30; i++)
		{
			int32_t e = S8(F, 0x40 + i);
			if (e == -1)
			{
				vb += 0x48;
				continue;
			}
			const int32_t fv = e;
			U32(WS(), 0xF4) = FadeColour(U32(WS(), 0x60), 0x10, fv, true);
			for (const uint8_t *tp = tab + 3; tp < tab + 0x23; tp += 4)
			{
				uint32_t a = tp[-3];
				uint32_t b = tp[0];
				int32_t sz;
				if (a == b)
				{
					U8(q, 3) = 3;
					U32(q, 8) = U32(vb, 0);
					U32(q, 0xC) = U32(vb, 8);
					sz = 0x10;
					U32(q, 4) = U32(WS(), 0xF4);
				}
				else
				{
					U8(q, 3) = 4;
					U32(q, 8) = U32(vb, 0);
					U32(q, 0x10) = U32(vb, 8);
					U32(q, 4) = FadeColour(U32(WS(), 0x64), (int32_t)a, e, e == fv);
					U32(q, 0xC) = FadeColour(U32(WS(), 0x64), (int32_t)b, e, e == fv);
					sz = 0x14;
				}
				int32_t za = U16(vb, 4);
				int32_t zb = U16(vb, 0xC);
				e = (int32_t)LineOT(za, zb);
				x::InsertPrimDepthKeys((uint32_t)e, q, za, zb, 0, 0);
				q += sz;
				vb += 8;
			}
			vb += 8;
		}
		U32(CTX(), 0x7C) = (uint32_t)q;
	}

	static uint8_t *RowsF2(uint8_t *p, uint8_t *v, int32_t segs)
	{
		const uint8_t *tab = FADETAB();
		for (int32_t i = 0; i < 0x20; i++)
		{
			U32(WS(), 0xF4) = xl_ColourScale_B66A20(U32(WS(), 0x60), shl32((int32_t)tab[i], 4));
			for (int32_t k = segs; k; k--)
			{
				EmitLineF2Ws(p, v);
				p += 0x10;
				v += 8;
			}
			v += 8;
		}
		return p;
	}

	static uint8_t *RowsPairs(uint8_t *p, uint8_t *v, int32_t rows)
	{
		const uint8_t *tab = FADETAB();
		for (int32_t r = rows; r; r--)
		{
			for (const uint8_t *tp = tab + 3; tp < tab + 0x23; tp += 4)
			{
				uint32_t a = tp[-3];
				uint32_t b = tp[0];
				int32_t sz;
				if (a == b)
				{
					U8(p, 3) = 3;
					sz = 0x10;
					U32(p, 4) = U32(WS(), 0x60);
					U32(p, 8) = U32(v, 0);
					U32(p, 0xC) = U32(v, 8);
				}
				else
				{
					U8(p, 3) = 4;
					U32(p, 8) = U32(v, 0);
					U32(p, 0x10) = U32(v, 8);
					U32(p, 4) = xl_ColourScale_B66A20(U32(WS(), 0x64), shl32((int32_t)a, 4));
					U32(p, 0xC) = xl_ColourScale_B66A20(U32(WS(), 0x64), shl32((int32_t)b, 4));
					sz = 0x14;
				}
				int32_t zb = U16(v, 0xC);
				int32_t za = U16(v, 4);
				x::InsertPrimDepthKeys(LineOT(za, zb), p, za, zb, 0, 0);
				p += sz;
				v += 8;
			}
			v += 8;
		}
		return p;
	}

	static void RolledGrid_AE60B0(uint8_t *blk, uint8_t *desc)
	{
		(void)blk;
		uint8_t *s = PTR(CTX(), 0x74);
		U32(s, 0) = 0;
		U16(s, 4) = 0;
		s += 0x10;
		S32(WS(), 0xB0) = 0;
		int32_t cz = (int32_t)U16(desc, 4);
		int32_t rem = (int32_t)S16(CUR(), 0x96) % cz;
		U16(WS(), 0xB4) = (uint16_t)(-rem);
		S32(WS(), 0xC0) = 0;
		U16(WS(), 0xC4) = (uint16_t)(0u - U16(desc, 4));
		S32(WS(), 0xD0) = 0;
		U16(WS(), 0xD4) = 0;
		x::SetTransVector(ZEROVEC());
		for (int32_t n = 0x21; n; n--)
		{
			U16(WS(), 0xD0) = (uint16_t)(U16(WS(), 0xD0) + U16(CUR(), 0x8E));
			x::RotMatrixFromAngles(WS() + 0xD0, WS() + 0xE0);
			x::SetRotMatrix(WS() + 0xE0);
			xl_RotTransIR_56C850(WS() + 0xC0, s, WS() + 0xFC);
			xl_RotTransIR_56C850(WS() + 0xB0, s - 8, WS() + 0xFC);
			int32_t tx = S16(s, 0);
			int32_t ty = S16(s, 2);
			int32_t tz = S16(s, 4);
			x::GteWriteCtrl(tx, 5);
			x::GteWriteCtrl(ty, 6);
			x::GteWriteCtrl(tz, 7);
			s += 0x10;
		}
		S32(WS(), 0x94) = S16(s, -0x1E);
		PTR(WS(), 0x90) = s;

		LoadParentMatrixScaled_AE6000(ZEROVEC(), 0x1000);
		uint8_t *vo = PTR(WS(), 0x90);
		U16(WS(), 0xC0) = (uint16_t)(U16(desc, 6) * 0xFFF0u);
		uint32_t xs = U16(desc, 6);
		for (int32_t m = 0x21; m; m--)
		{
			uint8_t *pp = PTR(CTX(), 0x74);
			U16(WS(), 0xC0) = (uint16_t)(U16(WS(), 0xC0) + xs);
			for (int32_t k = 0x21; k; k--)
			{
				U16(pp, 0) = U16(WS(), 0xC0);
				ProjectSV(pp, vo);
				pp += 0x10;
				vo += 8;
			}
		}
		U32(CTX(), 0x7C) = (uint32_t)RowsF2(PTR(CTX(), 0x7C), PTR(WS(), 0x90), 0x20);

		LoadParentMatrixScaled_AE6000(ZEROVEC(), 0x1000);
		vo = PTR(WS(), 0x90);
		uint8_t *pb = PTR(CTX(), 0x74) + 8;
		int32_t xstep = shl32((int32_t)U16(desc, 6), 2);
		for (int32_t r = 0x21; r; r--)
		{
			int32_t xx = shl32(-(int32_t)U16(desc, 6), 4);
			for (int32_t k = 9; k; k--)
			{
				U16(pb, 0) = (uint16_t)xx;
				ProjectSV(pb, vo);
				vo += 8;
				xx = add32(xx, xstep);
			}
			pb += 0x10;
		}
		U32(CTX(), 0x7C) = (uint32_t)RowsPairs(PTR(CTX(), 0x7C), PTR(WS(), 0x90), 0x20);

		if (U8(CUR(), 0x4A) & 1)
		{
			LoadParentMatrixScaled_AE6000(ZEROVEC(), 0x1000);
			uint8_t *vv = PTR(CTX(), 0x74);
			uint8_t *sv = WS() + 0xC0;
			U16(WS(), 0xC2) = 0;
			int32_t zstep = shl32((int32_t)U16(desc, 4), 2);
			for (int32_t j = 0; j <= 0x20; j++)
			{
				U16(WS(), 0xC0) = (uint16_t)((uint32_t)(j - 0xF) * U16(desc, 6));
				int32_t z = 0;
				for (int32_t k = 5; k; k--)
				{
					U16(WS(), 0xC4) = (uint16_t)z;
					ProjectSV(sv, vv);
					vv += 8;
					z = add32(z, zstep);
				}
			}
			U32(CTX(), 0x7C) = (uint32_t)RowsF2(PTR(CTX(), 0x7C), PTR(CTX(), 0x74), 4);

			vv = PTR(CTX(), 0x74);
			sv = WS() + 0xC0;
			U16(WS(), 0xC2) = 0;
			uint32_t c4 = U16(desc, 4);
			int32_t rm = (int32_t)S16(CUR(), 0x96) % (int32_t)c4;
			U16(WS(), 0xC4) = (uint16_t)(c4 - (uint32_t)rm);
			int32_t xst = shl32((int32_t)U16(desc, 6), 2);
			for (int32_t m = 0x11; m; m--)
			{
				int32_t xx = shl32(-(int32_t)U16(desc, 6), 4);
				for (int32_t k = 9; k; k--)
				{
					U16(WS(), 0xC0) = (uint16_t)xx;
					ProjectSV(sv, vv);
					vv += 8;
					xx = add32(xx, xst);
				}
				U16(WS(), 0xC4) = (uint16_t)(U16(WS(), 0xC4) + U16(desc, 4));
			}
			U32(CTX(), 0x7C) = (uint32_t)RowsPairs(PTR(CTX(), 0x7C), PTR(CTX(), 0x74), 0x10);
		}

		if (U8(CUR(), 0x4A) & 2)
		{
			uint8_t *vv = PTR(CTX(), 0x74);
			uint8_t *sv = WS() + 0xC0;
			U16(WS(), 0xC2) = U16(WS(), 0x94);
			int32_t zstep = shl32((int32_t)U16(desc, 4), 2);
			for (int32_t j = 0; j <= 0x20; j++)
			{
				U16(WS(), 0xC0) = (uint16_t)((uint32_t)(j - 0xF) * U16(desc, 6));
				int32_t z = (int32_t)U16(desc, 4);
				for (int32_t k = 5; k; k--)
				{
					U16(WS(), 0xC4) = (uint16_t)z;
					ProjectSV(sv, vv);
					vv += 8;
					z = add32(z, zstep);
				}
			}
			U32(CTX(), 0x7C) = (uint32_t)RowsF2(PTR(CTX(), 0x7C), PTR(CTX(), 0x74), 4);

			sv = WS() + 0xC0;
			vv = PTR(CTX(), 0x74);
			U16(WS(), 0xC2) = U16(WS(), 0x94);
			uint32_t c4 = U16(desc, 4);
			int32_t rm = (int32_t)S16(CUR(), 0x96) % (int32_t)c4;
			U16(WS(), 0xC4) = (uint16_t)((uint32_t)rm + c4);
			int32_t xst = shl32((int32_t)U16(desc, 6), 2);
			for (int32_t m = 0x11; m; m--)
			{
				int32_t xx = shl32(-(int32_t)U16(desc, 6), 4);
				for (int32_t k = 9; k; k--)
				{
					U16(WS(), 0xC0) = (uint16_t)xx;
					ProjectSV(sv, vv);
					vv += 8;
					xx = add32(xx, xst);
				}
				U16(WS(), 0xC4) = (uint16_t)(U16(WS(), 0xC4) + U16(desc, 4));
			}
			U32(CTX(), 0x7C) = (uint32_t)RowsPairs(PTR(CTX(), 0x7C), PTR(CTX(), 0x74), 0x10);
		}
	}

	static void __cdecl dh_59_WireGrid()
	{
		uint8_t *cur = CUR();
		if (PTR(cur, 0xBC) == nullptr)
		{
			uint8_t *desc = PTR(cur, 0xB8);
			uint8_t *b = blob::ArenaAlloc(0xC);
			PTR(CUR(), 0xBC) = b;
			uint8_t *blk = PTR(CUR(), 0xBC);
			PTR(blk, 0) = desc + S16(desc, 0);
			uint8_t *F = blob::ArenaAlloc(0x80);
			PTR(blk, 4) = F;
			for (int32_t i = 0; i < 0x20; i++) U32(F, i * 4) = 0xFFFFFFFFu;
			cur = CUR();
		}
		uint32_t col = U32(cur, 0xCC);
		uint8_t *desc = PTR(cur, 0xB8);
		uint8_t *blk = PTR(cur, 0xBC);
		U32(WS(), 0x60) = col | 0x40000000u;
		U32(WS(), 0x64) = U32(CUR(), 0xCC) | 0x50000000u;
		U32(WS(), 0x68) = U16(desc, 4);
		U32(WS(), 0x6C) = U16(desc, 6);
		if (U16(CUR(), 0x4A) == 0) FlatGridFadeIn_AE5AD0(blk, desc);
		if (U8(CUR(), 0x4A) & 3) RolledGrid_AE60B0(blk, desc);
	}

	static void __cdecl dh_60_PlaceModelOnCurlPath()
	{
		uint8_t *E = PTR(SCENE(), 0x60 + 4 * (int32_t)U8(CUR(), 0x1B));

		U8(E, 1) |= 0x10;
		S32(WS(), 0x84) = S16(PTR(CUR(), 0xB8), 2);
		uint8_t *pt = SCENE() + 8 * (int32_t)U8(CUR(), 0x1B) + 0x120;
		int32_t sc = S32(WS(), 0x84);
		U16(E, 0x94) = (uint16_t)(mul32(sc, S16(pt, 0)) >> 12);
		U16(E, 0x96) = (uint16_t)(mul32(S16(pt, 2), S32(WS(), 0x84)) >> 12);
		U16(E, 0x98) = (uint16_t)(mul32(S16(pt, 4), S32(WS(), 0x84)) >> 12);
		uint8_t *qp = SCENE() + 8 * (int32_t)U8(CUR(), 0x1B) + 0x84;
		U16(WS(), 0xA0) = (uint16_t)(mul32(S32(WS(), 0x84), S16(qp, 0)) >> 12);
		U16(WS(), 0xA2) = 0;
		U16(WS(), 0xA4) = (uint16_t)sub32(mul32(add32(S16(qp, 4), 0x800), S32(WS(), 0x84)) >> 12, 0x400);
		S32(WS(), 0x6C) = -(int32_t)S16(WS(), 0xA4);
		S32(WS(), 0x64) = shl32(S16(CUR(), 0x8E), 4);
		PTR(WS(), 0x80) = CUR();
		uint8_t *ref = blob::GetBone(S16(PTR(CUR(), 0xB8), 0));
		CUR() = ref;
		uint8_t *par = blob::GetBone((int32_t)U16(ref, 0x9C));
		S32(WS(), 0x60) = U16(PTR(CUR(), 0xB8), 4);
		S32(WS(), 0x68) = -(int32_t)S16(par, 0x98);
		int32_t d = add32(S32(WS(), 0x6C), S32(WS(), 0x64));
		int32_t st = S32(WS(), 0x68);
		if (d <= st)
		{
			U16(E, 0x1E) = 0;
			U16(E, 0x20) = (uint16_t)(0u - (uint32_t)d);
			uint16_t px = U16(WS(), 0xA0);
			U16(E, 0xC) = 0;
			U16(E, 0x1C) = px;
		}
		else if (d >= add32(shl32(S32(WS(), 0x60), 5), st))
		{
			S32(WS(), 0xC0) = 0;
			U16(WS(), 0xC4) = (uint16_t)(0u - U16(WS(), 0x60));
			S32(WS(), 0xD0) = 0;
			U16(WS(), 0xD4) = 0;
			S32(WS(), 0xE0) = 0;
			S32(WS(), 0xE4) = 0;
			for (int32_t n = 0x20; n; n--)
			{
				U16(WS(), 0xD0) = (uint16_t)(U16(WS(), 0xD0) + U16(CUR(), 0x8E));
				int32_t tx = S16(WS(), 0xE0);
				int32_t tz = S16(WS(), 0xE4);
				int32_t ty = S16(WS(), 0xE2);
				x::GteWriteCtrl(tx, 5);
				x::GteWriteCtrl(ty, 6);
				x::GteWriteCtrl(tz, 7);
				x::RotMatrixFromAngles(WS() + 0xD0, WS() + 0xE0);
				x::SetRotMatrix(WS() + 0xE0);
				xl_RotTransIR_56C850(WS() + 0xC0, WS() + 0xE0, WS() + 0xFC);
			}
			uint32_t e60 = U32(WS(), 0x60);
			uint32_t dx = U16(WS(), 0x60);
			dx = (uint16_t)(dx - (uint16_t)(e60 << 5));
			uint32_t e68 = U32(WS(), 0x68);
			dx = dx - e68;
			dx = (uint16_t)((uint16_t)dx - (uint16_t)e68);
			dx = dx + (uint32_t)d;
			U16(E, 0x20) = (uint16_t)dx;
			U16(E, 0x1E) = U16(WS(), 0xE2);
			uint16_t px = U16(WS(), 0xA0);
			U16(E, 0xC) = 0x800;
			U16(E, 0x1C) = px;
		}
		else
		{
			d = sub32(d, st);
			S32(WS(), 0xB0) = 0;
			int32_t r = d % S32(WS(), 0x60);
			U16(WS(), 0xB4) = (uint16_t)(0u - (uint32_t)r);
			int32_t nseg = d / S32(WS(), 0x60);
			S32(WS(), 0xC0) = 0;
			U16(WS(), 0xC4) = (uint16_t)(0u - U16(WS(), 0x60));
			S32(WS(), 0xD0) = 0;
			U16(WS(), 0xD4) = 0;
			S32(WS(), 0xE0) = 0;
			S32(WS(), 0xE4) = 0;
			if (nseg >= 0)
			{
				for (int32_t n = nseg + 1; n; n--)
				{
					U16(WS(), 0xD0) = (uint16_t)(U16(WS(), 0xD0) + U16(CUR(), 0x8E));
					int32_t ty = S16(WS(), 0xE2);
					int32_t tz = S16(WS(), 0xE4);
					int32_t tx = S16(WS(), 0xE0);
					x::GteWriteCtrl(tx, 5);
					x::GteWriteCtrl(ty, 6);
					x::GteWriteCtrl(tz, 7);
					x::RotMatrixFromAngles(WS() + 0xD0, WS() + 0xE0);
					x::SetRotMatrix(WS() + 0xE0);
					xl_RotTransIR_56C850(WS() + 0xC0, WS() + 0xE0, WS() + 0xFC);
				}
			}
			xl_RotTransIR_56C850(WS() + 0xB0, WS() + 0xC0, WS() + 0xFC);
			U16(E, 0x1E) = U16(WS(), 0xC2);
			uint32_t e68 = U32(WS(), 0x68);
			U16(E, 0x20) = (uint16_t)(U16(WS(), 0xC4) - (uint16_t)e68);
			U16(E, 0x1C) = U16(WS(), 0xA0);
			U16(E, 0xC) = (uint16_t)(0u - U16(WS(), 0xD0));
		}

		CUR() = PTR(WS(), 0x80);
		int32_t v = S16(E, 0x1C);
		U16(CUR(), 0x94) = (uint16_t)v;
		S32(CUR(), 0x5C) = shl32(v, 16);
		v = S16(E, 0x1E);
		U16(CUR(), 0x96) = (uint16_t)v;
		S32(CUR(), 0x60) = shl32(v, 16);
		v = S16(E, 0x20);
		U16(CUR(), 0x98) = (uint16_t)v;
		S32(CUR(), 0x64) = shl32(v, 16);
	}
}

	void apply_eden_a(Clone &c)
	{
		c.draw[59] = part_eden_a::dh_59_WireGrid;
		c.draw[60] = part_eden_a::dh_60_PlaceModelOnCurlPath;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace part_eden_b
{
	static inline int32_t xl_Sqrt_56BEC0(int32_t v) { return x::f<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	static inline int32_t xl_NormalClip_56CB30(uint32_t a, uint32_t b, uint32_t c) { return x::f<int32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_NormalClip3))(a, b, c); }
	static inline uint32_t xl_ColourScale_B66A20(uint32_t colour, int32_t intensity) { return x::f<uint32_t (__cdecl *)(uint32_t, int32_t)>(game_addr(game::GfCinematic_ScaleColourGPF))(colour, intensity); }
	static inline int32_t xl_Nop_B65150() { return x::f<int32_t (__cdecl *)()>(game_addr(game::xorEAX_6))(); }

	static inline uint8_t *ZEROVEC() { return (uint8_t *)game_addr(game::unk_27971E4); }
	static inline uint16_t GTE_SZ3_LO() { return MEM<uint16_t>(game_addr(game::GTE_SZ3)); }

	static inline int32_t TriKey(int32_t sum)
	{
		int32_t hi = (int32_t)(((int64_t)sum * (int64_t)0x55555556) >> 32);
		hi = add32(hi, (int32_t)((uint32_t)hi >> 31));
		return (int32_t)((uint32_t)(hi >> 2) & 0xFFFFFFFCu);
	}
	static inline int32_t QuadKey(int32_t sum)
	{
		int32_t q = sum / 4;
		return (int32_t)((uint32_t)(q >> 2) & 0xFFFFFFFCu);
	}

	static int32_t LoadParentMatrixScaled(const void *pos, int32_t scale)
	{
		Mat4x3 *m = blob::GetParentMatrix(U16(CUR(), 0x9C));
		x::SetRotMatrix(m);
		x::SetTransVector(m);
		uint8_t *w = WS();
		x::TransformToCamera(pos, w + 0xF4, w + 0xD4);
		uint8_t *d = WS() + 0xE0;
		U32(d, 0x00) = U32(m, 0x00);
		U32(d, 0x04) = U32(m, 0x04);
		U32(d, 0x08) = U32(m, 0x08);
		U32(d, 0x0C) = U32(m, 0x0C);
		U32(d, 0x10) = U32(m, 0x10);
		S32(WS(), 0xDC) = scale;
		S32(WS(), 0xD8) = scale;
		S32(WS(), 0xD4) = scale;
		x::ScaleMatrix(d, WS() + 0xD4);
		return 0;
	}

	static inline void FixedOtMasked()
	{
		int32_t b = S16(CUR(), 0xC0);
		if (b == 0) U32(WS(), 0xF8) = 0;
		else U32(WS(), 0xF8) = (uint32_t)(b & 0x3FFC) + U32(RT(), 0x4C);
	}

	static uint8_t *LW_Gt3Lit(uint8_t *blk, uint8_t *scr, uint8_t *col)
	{
		uint8_t *pkt = PTR(CTX(), 0x7C);
		uint32_t tp = U8(CUR(), 0x92) & 0x60u;
		int32_t n = S16(blk, 2);
		U32(WS(), 0xF0) = tp;
		blk += 4;
		U32(WS(), 0xF4) = (U32(CUR(), 0xCC) & 0x2000000u) | 0x34000000u;
		FixedOtMasked();
		if (n <= 0)
		{
			PTR(CTX(), 0x7C) = pkt;
			return blk;
		}
		do
		{
			uint32_t v2 = U32(scr, U16(blk, 0x16));
			uint32_t v1 = U32(scr, U16(blk, 0x14));
			uint32_t v0 = U32(scr, U16(blk, 0x12));
			if (xl_NormalClip_56CB30(v0, v1, v2) > 0)
			{
				U8(pkt, 3) = 9;
				U16(pkt, 0x1A) = (uint16_t)(U16(WS(), 0xF0) | U16(blk, 0x1A));
				U16(pkt, 0x0E) = U16(blk, 0x18);
				U32(pkt, 0x08) = v0;
				U32(pkt, 0x14) = v1;
				U32(pkt, 0x20) = v2;
				uint32_t c = xl_ColourScale_B66A20(U32(col, U16(blk, 0x12) >> 1), (int32_t)(U32(blk, 0) & 0xFF));
				U32(pkt, 0x04) = U32(WS(), 0xF4) | c;
				U32(pkt, 0x10) = xl_ColourScale_B66A20(U32(col, U16(blk, 0x14) >> 1), (int32_t)(U32(blk, 4) & 0xFF));
				U32(pkt, 0x1C) = xl_ColourScale_B66A20(U32(col, U16(blk, 0x16) >> 1), (int32_t)(U32(blk, 8) & 0xFF));
				U16(pkt, 0x0C) = U16(blk, 0x0C);
				U16(pkt, 0x18) = U16(blk, 0x0E);
				U16(pkt, 0x24) = U16(blk, 0x10);
				uint32_t ot = U32(WS(), 0xF8);
				int32_t z2 = U16(scr, U16(blk, 0x16) + 4);
				int32_t z1 = U16(scr, U16(blk, 0x14) + 4);
				int32_t z0 = U16(scr, U16(blk, 0x12) + 4);
				if (ot == 0) ot = (uint32_t)TriKey(z0 + z1 + z2) + U32(RT(), 0x4C);
				x::InsertPrimDepthKeys(ot, pkt, z0, z1, z2, 0);
				pkt += 0x28;
			}
			blk += 0x1C;
		} while (--n != 0);
		PTR(CTX(), 0x7C) = pkt;
		return blk;
	}

	static uint8_t *LW_Gt4Lit(uint8_t *blk, uint8_t *scr, uint8_t *col)
	{
		uint8_t *pkt = PTR(CTX(), 0x7C);
		uint32_t tp = U8(CUR(), 0x92) & 0x60u;
		int32_t n = S16(blk, 2);
		U32(WS(), 0xF0) = tp;
		blk += 4;
		U32(WS(), 0xF4) = (U32(CUR(), 0xCC) & 0x2000000u) | 0x3C000000u;
		FixedOtMasked();
		if (n <= 0)
		{
			PTR(CTX(), 0x7C) = pkt;
			return blk;
		}
		do
		{
			uint32_t v2 = U32(scr, U16(blk, 0x1C));
			uint32_t v1 = U32(scr, U16(blk, 0x1A));
			uint32_t v0 = U32(scr, U16(blk, 0x18));
			if (xl_NormalClip_56CB30(v0, v1, v2) > 0)
			{
				U8(pkt, 3) = 0x0C;
				U16(pkt, 0x1A) = (uint16_t)(U16(WS(), 0xF0) | U16(blk, 0x22));
				U16(pkt, 0x0E) = U16(blk, 0x20);
				U32(pkt, 0x08) = v0;
				U32(pkt, 0x14) = v1;
				U32(pkt, 0x20) = v2;
				U32(pkt, 0x2C) = U32(scr, U16(blk, 0x1E));
				uint32_t c = xl_ColourScale_B66A20(U32(col, U16(blk, 0x18) >> 1), (int32_t)(U32(blk, 0) & 0xFF));
				U32(pkt, 0x04) = U32(WS(), 0xF4) | c;
				U32(pkt, 0x10) = xl_ColourScale_B66A20(U32(col, U16(blk, 0x1A) >> 1), (int32_t)(U32(blk, 4) & 0xFF));
				U32(pkt, 0x1C) = xl_ColourScale_B66A20(U32(col, U16(blk, 0x1C) >> 1), (int32_t)(U32(blk, 8) & 0xFF));
				U32(pkt, 0x28) = xl_ColourScale_B66A20(U32(col, U16(blk, 0x1E) >> 1), (int32_t)(U32(blk, 0xC) & 0xFF));
				U16(pkt, 0x0C) = U16(blk, 0x10);
				U16(pkt, 0x18) = U16(blk, 0x12);
				U16(pkt, 0x24) = U16(blk, 0x14);
				U16(pkt, 0x30) = U16(blk, 0x16);
				uint32_t ot = U32(WS(), 0xF8);
				int32_t z3 = U16(scr, U16(blk, 0x1E) + 4);
				int32_t z2 = U16(scr, U16(blk, 0x1C) + 4);
				int32_t z1 = U16(scr, U16(blk, 0x1A) + 4);
				int32_t z0 = U16(scr, U16(blk, 0x18) + 4);
				if (ot == 0) ot = (uint32_t)QuadKey(z0 + z1 + z2 + z3) + U32(RT(), 0x4C);
				x::InsertPrimDepthKeys(ot, pkt, z0, z1, z2, z3);
				pkt += 0x34;
			}
			blk += 0x24;
		} while (--n != 0);
		PTR(CTX(), 0x7C) = pkt;
		return blk;
	}

	static uint8_t *LW_Gt3Plain(uint8_t *blk, uint8_t *scr, uint8_t *col)
	{
		uint8_t *pkt = PTR(CTX(), 0x7C);
		uint32_t tp = U8(CUR(), 0x92) & 0x60u;
		int32_t n = S16(blk, 2);
		U32(WS(), 0xF0) = tp;
		blk += 4;
		U32(WS(), 0xF4) = (U32(CUR(), 0xCC) & 0x2000000u) | 0x34000000u;
		FixedOtMasked();
		if (n <= 0)
		{
			PTR(CTX(), 0x7C) = pkt;
			return blk;
		}
		do
		{
			uint32_t v2 = U32(scr, U16(blk, 0x0E));
			uint32_t v1 = U32(scr, U16(blk, 0x0C));
			uint32_t v0 = U32(scr, U16(blk, 0x0A));
			if (xl_NormalClip_56CB30(v0, v1, v2) > 0)
			{
				U8(pkt, 3) = 9;
				U16(pkt, 0x1A) = (uint16_t)(U16(blk, 0x12) | U16(WS(), 0xF0));
				U16(pkt, 0x0E) = U16(blk, 0x10);
				U32(pkt, 0x08) = v0;
				U32(pkt, 0x14) = v1;
				U32(pkt, 0x20) = v2;
				U32(pkt, 0x04) = U32(col, U16(blk, 0x0A) >> 1) | U32(WS(), 0xF4);
				U32(pkt, 0x10) = U32(col, U16(blk, 0x0C) >> 1);
				U32(pkt, 0x1C) = U32(col, U16(blk, 0x0E) >> 1);
				U16(pkt, 0x0C) = U16(blk, 0x04);
				U16(pkt, 0x18) = U16(blk, 0x06);
				U16(pkt, 0x24) = U16(blk, 0x08);
				uint32_t ot = U32(WS(), 0xF8);
				int32_t z2 = U16(scr, U16(blk, 0x0E) + 4);
				int32_t z1 = U16(scr, U16(blk, 0x0C) + 4);
				int32_t z0 = U16(scr, U16(blk, 0x0A) + 4);
				if (ot == 0) ot = (uint32_t)TriKey(z0 + z2 + z1) + U32(RT(), 0x4C);
				x::InsertPrimDepthKeys(ot, pkt, z0, z1, z2, 0);
				pkt += 0x28;
			}
			blk += 0x14;
		} while (--n != 0);
		PTR(CTX(), 0x7C) = pkt;
		return blk;
	}

	static uint8_t *LW_Gt4Plain(uint8_t *blk, uint8_t *scr, uint8_t *col)
	{
		uint8_t *pkt = PTR(CTX(), 0x7C);
		uint32_t tp = U8(CUR(), 0x92) & 0x60u;
		int32_t n = S16(blk, 2);
		U32(WS(), 0xF0) = tp;
		blk += 4;
		U32(WS(), 0xF4) = (U32(CUR(), 0xCC) & 0x2000000u) | 0x3C000000u;
		FixedOtMasked();
		if (n <= 0)
		{
			PTR(CTX(), 0x7C) = pkt;
			return blk;
		}
		do
		{
			uint32_t v2 = U32(scr, U16(blk, 0x10));
			uint32_t v0 = U32(scr, U16(blk, 0x0C));
			uint32_t v1 = U32(scr, U16(blk, 0x0E));
			if ((U8(CUR(), 0xDE) & 0x10) != 0 || xl_NormalClip_56CB30(v0, v1, v2) > 0)
			{
				U8(pkt, 3) = 0x0C;
				U16(pkt, 0x1A) = (uint16_t)(U16(WS(), 0xF0) | U16(blk, 0x16));
				U32(pkt, 0x08) = v0;
				U32(pkt, 0x14) = v1;
				U16(pkt, 0x0E) = U16(blk, 0x14);
				U32(pkt, 0x20) = v2;
				U32(pkt, 0x2C) = U32(scr, U16(blk, 0x12));
				U32(pkt, 0x04) = U32(col, U16(blk, 0x0C) >> 1) | U32(WS(), 0xF4);
				U32(pkt, 0x10) = U32(col, U16(blk, 0x0E) >> 1);
				U32(pkt, 0x1C) = U32(col, U16(blk, 0x10) >> 1);
				U32(pkt, 0x28) = U32(col, U16(blk, 0x12) >> 1);
				U16(pkt, 0x0C) = U16(blk, 0x04);
				U16(pkt, 0x18) = U16(blk, 0x06);
				U16(pkt, 0x24) = U16(blk, 0x08);
				U16(pkt, 0x30) = U16(blk, 0x0A);
				uint32_t ot = U32(WS(), 0xF8);
				int32_t z3 = U16(scr, U16(blk, 0x12) + 4);
				int32_t z2 = U16(scr, U16(blk, 0x10) + 4);
				int32_t z1 = U16(scr, U16(blk, 0x0E) + 4);
				int32_t z0 = U16(scr, U16(blk, 0x0C) + 4);
				if (ot == 0) ot = (uint32_t)QuadKey(z0 + z1 + z2 + z3) + U32(RT(), 0x4C);
				x::InsertPrimDepthKeys(ot, pkt, z0, z1, z2, z3);
				pkt += 0x34;
			}
			blk += 0x18;
		} while (--n != 0);
		PTR(CTX(), 0x7C) = pkt;
		return blk;
	}

	static uint8_t *DK_Gt3(uint8_t *blk, uint8_t *scr, uint8_t *inten)
	{
		int32_t n = S16(blk, 2);
		uint8_t *rec = blk + 4;
		uint8_t *pkt = PTR(CTX(), 0x7C);
		U32(WS(), 0xF0) = U8(CUR(), 0x92) & 0x60u;
		U32(WS(), 0xF4) = (U32(CUR(), 0xCC) & 0x2000000u) | 0x34000000u;
		int32_t bias = S16(CUR(), 0xC0);
		U32(WS(), 0xF8) = (uint32_t)bias + U32(RT(), 0x4C);
		if (n <= 0)
		{
			PTR(CTX(), 0x7C) = pkt;
			return rec;
		}
		do
		{
			uint32_t v1 = U32(scr, U16(rec, 0x14));
			uint32_t v0 = U32(scr, U16(rec, 0x12));
			uint32_t v2 = U32(scr, U16(rec, 0x16));
			if (xl_NormalClip_56CB30(v0, v1, v2) > 0)
			{
				U8(pkt, 3) = 9;
				U16(pkt, 0x1A) = (uint16_t)(U16(rec, 0x1A) | U16(WS(), 0xF0));
				U16(pkt, 0x0E) = U16(rec, 0x18);
				U32(pkt, 0x08) = v0;
				U32(pkt, 0x14) = v1;
				U32(pkt, 0x20) = v2;
				uint32_t c = xl_ColourScale_B66A20(U32(rec, 0), S32(inten, U16(rec, 0x12) >> 1));
				U32(pkt, 0x04) = U32(WS(), 0xF4) | c;
				U32(pkt, 0x10) = xl_ColourScale_B66A20(U32(rec, 4), S32(inten, U16(rec, 0x14) >> 1));
				U32(pkt, 0x1C) = xl_ColourScale_B66A20(U32(rec, 8), S32(inten, U16(rec, 0x16) >> 1));
				U16(pkt, 0x0C) = U16(rec, 0x0C);
				U16(pkt, 0x18) = U16(rec, 0x0E);
				U16(pkt, 0x24) = U16(rec, 0x10);
				x::InsertPrimDepthKeys(U32(WS(), 0xF8), pkt, 0, 0, 0, 0);
				pkt += 0x28;
			}
			rec += 0x1C;
		} while (--n != 0);
		PTR(CTX(), 0x7C) = pkt;
		return rec;
	}

	static uint8_t *DK_Gt4(uint8_t *blk, uint8_t *scr, uint8_t *inten)
	{
		int32_t n = S16(blk, 2);
		uint8_t *rec = blk + 4;
		uint8_t *pkt = PTR(CTX(), 0x7C);
		U32(WS(), 0xF0) = U8(CUR(), 0x92) & 0x60u;
		U32(WS(), 0xF4) = (U32(CUR(), 0xCC) & 0x2000000u) | 0x3C000000u;
		int32_t bias = S16(CUR(), 0xC0);
		U32(WS(), 0xF8) = (uint32_t)bias + U32(RT(), 0x4C);
		if (n <= 0)
		{
			PTR(CTX(), 0x7C) = pkt;
			return rec;
		}
		do
		{
			uint32_t v2 = U32(scr, U16(rec, 0x1C));
			uint32_t v1 = U32(scr, U16(rec, 0x1A));
			uint32_t v0 = U32(scr, U16(rec, 0x18));
			if (xl_NormalClip_56CB30(v0, v1, v2) > 0)
			{
				U8(pkt, 3) = 0x0C;
				U16(pkt, 0x1A) = (uint16_t)(U16(WS(), 0xF0) | U16(rec, 0x22));
				U16(pkt, 0x0E) = U16(rec, 0x20);
				U32(pkt, 0x08) = v0;
				U32(pkt, 0x14) = v1;
				U32(pkt, 0x20) = v2;
				U32(pkt, 0x2C) = U32(scr, U16(rec, 0x1E));
				uint32_t c = xl_ColourScale_B66A20(U32(rec, 0), S32(inten, U16(rec, 0x18) >> 1));
				U32(pkt, 0x04) = U32(WS(), 0xF4) | c;
				U32(pkt, 0x10) = xl_ColourScale_B66A20(U32(rec, 4), S32(inten, U16(rec, 0x1A) >> 1));
				U32(pkt, 0x1C) = xl_ColourScale_B66A20(U32(rec, 8), S32(inten, U16(rec, 0x1C) >> 1));
				U32(pkt, 0x28) = xl_ColourScale_B66A20(U32(rec, 0xC), S32(inten, U16(rec, 0x1E) >> 1));
				U16(pkt, 0x0C) = U16(rec, 0x10);
				U16(pkt, 0x18) = U16(rec, 0x12);
				U16(pkt, 0x24) = U16(rec, 0x14);
				U16(pkt, 0x30) = U16(rec, 0x16);
				int32_t z3 = U16(scr, U16(rec, 0x1E) + 4);
				int32_t z2 = U16(scr, U16(rec, 0x1C) + 4);
				int32_t z1 = U16(scr, U16(rec, 0x1A) + 4);
				int32_t z0 = U16(scr, U16(rec, 0x18) + 4);
				x::InsertPrimDepthKeys(U32(WS(), 0xF8), pkt, z0, z1, z2, z3);
				pkt += 0x34;
			}
			rec += 0x24;
		} while (--n != 0);
		PTR(CTX(), 0x7C) = pkt;
		return rec;
	}

	static void ProjectVertices(uint8_t *v, int32_t n, uint8_t *scr)
	{
		if (n <= 0) return;
		do
		{
			uint32_t xy = ((uint32_t)U16(v, 2) << 16) | U16(v, 0);
			uint32_t z = U16(v, 4);
			x::GteWriteData((int32_t)xy, 0);
			x::GteWriteData((int32_t)z, 1);
			x::GteRTPS();
			x::GteReadSXY2(scr);
			U16(scr, 4) = GTE_SZ3_LO();
			v += 8;
			scr += 8;
		} while (--n != 0);
	}

	static inline int32_t VertexDistance(const uint8_t *v)
	{
		uint8_t *w = WS();
		int32_t dx = sub32(S16(v, 0), S16(w, 0xC0));
		int32_t dy = sub32(S16(v, 2), S16(w, 0xC2));
		int32_t r = xl_Sqrt_56BEC0(add32(mul32(dy, dy), mul32(dx, dx)));
		w = WS();
		int32_t dz = sub32(S16(v, 4), S16(w, 0xC4));
		return xl_Sqrt_56BEC0(add32(mul32(dz, dz), mul32(r, r)));
	}

	static void __cdecl dh_61_MeshRadialLightWave()
	{
		uint8_t *bone = CUR();
		if (PTR(bone, 0xBC) == nullptr)
		{
			uint8_t *desc = PTR(bone, 0xB8);
			uint8_t *obj = blob::ObjectPtr(U16(desc, 0));
			uint8_t *pool = blob::ArenaAlloc(0x10);
			PTR(CUR(), 0xBC) = pool;
			pool = PTR(CUR(), 0xBC);
			PTR(pool, 0) = obj;
			if (U16(desc, 4) == 0xFFFF)
			{
				U32(pool, 8) = 0;
			}
			else
			{
				uint8_t *pal = blob::ArenaAlloc((int32_t)U16(desc, 6) << 2);
				PTR(pool, 8) = pal;
				blob::ReadClut_B66560(U16(desc, 4));
				uint8_t *clut = PTR(WS(), 0xFC);
				int32_t len = U16(desc, 6);
				if (len > 0)
				{
					do
					{
						int32_t c = S16(clut, 0);
						U8(pal, 3) = 0;
						U8(pal, 0) = (uint8_t)((uint8_t)c << 3);
						U8(pal, 1) = (uint8_t)((c >> 2) & 0xF8);
						U8(pal, 2) = (uint8_t)((c >> 7) & 0xF8);
						pal += 4;
						clut += 2;
					} while (--len != 0);
				}
			}
			bone = CUR();
		}
		uint8_t *desc = PTR(bone, 0xB8);
		uint8_t *pool = PTR(bone, 0xBC);
		uint8_t *centre = blob::GetBone(U16(desc, 0x0A));
		uint8_t *cur = PTR(CTX(), 0x74);
		PTR(WS(), 0x80) = cur;
		uint8_t *mesh = PTR(pool, 0);
		uint8_t *pal = PTR(pool, 8);
		uint8_t *v = mesh + U32(mesh, 0x14);
		U16(WS(), 0xC0) = U16(centre, 0x94);
		U16(WS(), 0xC2) = U16(centre, 0x96);
		U16(WS(), 0xC4) = U16(centre, 0x98);
		S32(WS(), 0xF0) = S16(CUR(), 0x8C);
		S32(WS(), 0xF4) = S16(CUR(), 0x8E);
		S32(WS(), 0xF8) = S16(CUR(), 0x90);
		S32(WS(), 0xFC) = S16(CUR(), 0x98);
		if (S32(WS(), 0xFC) <= 0) S32(WS(), 0xFC) = 1;
		U32(WS(), 0xE0) = U16(desc, 6);
		U32(WS(), 0xE4) = U16(desc, 8);
		int32_t n = S32(mesh, 0x18);
		if (n > 0)
		{
			do
			{
				int32_t d = VertexDistance(v);
				uint8_t *w = WS();
				int32_t radius = S32(w, 0xF0);
				int32_t diff = sub32(radius, d);
				if (diff <= 0)
				{
					U32(cur, 0) = 0;
				}
				else
				{
					int32_t k = mul32(S32(w, 0xF4), diff) / radius;
					int32_t mx = S32(w, 0xE4);
					if (k > mx) k = mx;
					if (k < 0) k = 0;
					uint32_t colour;
					if (pal == nullptr)
					{
						colour = U32(CUR(), 0xCC) & 0xFFFFFFu;
					}
					else
					{
						int32_t idx = add32(d / S32(w, 0xFC), S32(w, 0xF8));
						if (idx < 0) idx = 0;
						colour = U32(pal, (idx % S32(w, 0xE0)) * 4);
					}
					U32(cur, 0) = xl_ColourScale_B66A20(colour, k);
				}
				v += 8;
				cur += 4;
			} while (--n != 0);
		}
		PTR(WS(), 0x84) = cur;
		LoadParentMatrixScaled(ZEROVEC(), (int32_t)U16(desc, 2) << 4);
		x::SetRotMatrix(WS() + 0xE0);
		v = mesh + U32(mesh, 0x14);
		uint8_t *scr = PTR(WS(), 0x84);
		ProjectVertices(v, S32(mesh, 0x18), scr);
		uint8_t *blk = mesh + U32(mesh, 8);
		int32_t t = S16(blk, 0);
		if (t == 0xFFFF) return;
		for (;;)
		{
			uint8_t *w = WS();
			if (t == 9) blk = LW_Gt3Lit(blk, PTR(w, 0x84), PTR(w, 0x80));
			else if (t == 0x13) blk = LW_Gt4Lit(blk, PTR(w, 0x84), PTR(w, 0x80));
			else if (t == 8) blk = LW_Gt3Plain(blk, PTR(w, 0x84), PTR(w, 0x80));
			else if (t == 0x12) blk = LW_Gt4Plain(blk, PTR(w, 0x84), PTR(w, 0x80));
			else return;
			t = S16(blk, 0);
			if (t == 0xFFFF) return;
		}
	}

	static void __cdecl dh_62_MeshRadialDarken()
	{
		uint8_t *bone = CUR();
		if (PTR(bone, 0xBC) == nullptr)
		{
			uint8_t *obj = blob::ObjectPtr(U16(PTR(bone, 0xB8), 0));
			uint8_t *pool = blob::ArenaAlloc(0x10);
			PTR(CUR(), 0xBC) = pool;
			PTR(PTR(CUR(), 0xBC), 0) = obj;
			bone = CUR();
		}
		uint8_t *desc = PTR(bone, 0xB8);
		uint8_t *pool = PTR(bone, 0xBC);
		uint8_t *centre = blob::GetBone(U16(desc, 6));
		uint8_t *cur = PTR(CTX(), 0x74);
		PTR(WS(), 0x80) = cur;
		uint8_t *mesh = PTR(pool, 0);
		U16(WS(), 0xC0) = U16(centre, 0x94);
		U16(WS(), 0xC2) = U16(centre, 0x96);
		uint8_t *v = mesh + U32(mesh, 0x14);
		U16(WS(), 0xC4) = U16(centre, 0x98);
		S32(WS(), 0xF0) = S16(CUR(), 0x8C);
		S32(WS(), 0xF4) = S16(CUR(), 0x8E);
		U32(WS(), 0xE4) = U16(desc, 4);
		xl_Nop_B65150();
		int32_t n = S32(mesh, 0x18);
		if (n > 0)
		{
			do
			{
				int32_t d = VertexDistance(v);
				uint8_t *w = WS();
				int32_t radius = S32(w, 0xF0);
				int32_t diff = sub32(radius, d);
				if (diff <= 0)
				{
					U32(cur, 0) = 0x100;
				}
				else
				{
					int32_t mx = S32(w, 0xE4);
					int32_t k = mul32(S32(w, 0xF4), diff) / radius;
					if (k > mx) k = mx;
					if (k < 0) k = 0;
					U32(cur, 0) = (uint32_t)sub32(0x100, k);
				}
				v += 8;
				cur += 4;
			} while (--n != 0);
		}
		PTR(WS(), 0x84) = cur;
		LoadParentMatrixScaled(ZEROVEC(), (int32_t)U16(desc, 2) << 4);
		x::SetRotMatrix(WS() + 0xE0);
		v = mesh + U32(mesh, 0x14);
		uint8_t *scr = PTR(WS(), 0x84);
		ProjectVertices(v, S32(mesh, 0x18), scr);
		uint8_t *blk = mesh + U32(mesh, 8);
		int32_t t = S16(blk, 0);
		if (t == 0xFFFF) return;
		for (;;)
		{
			uint8_t *w = WS();
			if (t == 9) blk = DK_Gt3(blk, PTR(w, 0x84), PTR(w, 0x80));
			else if (t == 0x13) blk = DK_Gt4(blk, PTR(w, 0x84), PTR(w, 0x80));
			else return;
			t = S16(blk, 0);
			if (t == 0xFFFF) return;
		}
	}
}
	void apply_eden_b(Clone &c)
	{
		c.draw[61] = part_eden_b::dh_61_MeshRadialLightWave;
		c.draw[62] = part_eden_b::dh_62_MeshRadialDarken;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace part_eden_c
{
	static inline int32_t xl_Sqrt(int32_t v) { return x::f<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	static inline int32_t xl_TransformToScreen(const void *v, void *sxy, void *p, void *flag) { return x::f<int32_t (__cdecl *)(const void *, void *, void *, void *)>(game_addr(game::TransformWorldCoordinateToProjectedSpace))(v, sxy, p, flag); }
	static inline void xl_SetDrawEnv(void *p, const void *env) { x::f<void (__cdecl *)(void *, const void *)>(game_addr(game::SSIGPU_SetDrawEnv))(p, env); }
	static inline void xl_InsertPrimAutoDepth(uint32_t ot, void *prim) { x::f<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAutoDepth))(ot, prim); }
	static inline uint32_t xl_ScaleColour_B66A20(uint32_t colour, int32_t scale) { return x::f<uint32_t (__cdecl *)(uint32_t, int32_t)>(game_addr(game::GfCinematic_ScaleColourGPF))(colour, scale); }
	static inline void xl_RequestScreenFeedback(int32_t a, int32_t b, int32_t c, int32_t d, int32_t e)
	{
		x::f<void (__cdecl *)(int32_t, int32_t, int32_t, int32_t, int32_t)>(game_addr(game::Battle_RequestScreenFeedback))(a, b, c, d, e);
	}

	static inline uint8_t FRAMEPARITY() { return MEM<uint8_t>(game_addr(game::g_Battle_DisplayParity)); }
	static inline uint8_t *DRAWENVS() { return (uint8_t *)game_addr(game::g_Battle_DrawEnv); }
	static inline uint8_t *ZEROVEC() { return (uint8_t *)game_addr(game::unk_27971E4); }

	static inline int32_t div256(int32_t v) { return v / 256; }

	static void __cdecl dh_53_PixelParticleEmitter()
	{
		if (U32(CUR(), 0xBC) == 0)
		{
			uint8_t *desc = PTR(CUR(), 0xB8);
			uint32_t n = U16(desc, 4);
			uint8_t *a = blob::ArenaAlloc((int32_t)(n * 28 + 0x10));
			PTR(CUR(), 0xBC) = a;
			uint8_t *pool = PTR(CUR(), 0xBC);
			if ((int32_t)n > 0)
			{
				uint8_t *q = pool + 0x28;
				do { *q = 0xFF; q += 0x1C; } while (--n != 0);
			}
			U16(pool, 2) = 0;
			PTR(pool, 4) = desc + S16(desc, 0);
		}
		uint8_t *desc = PTR(CUR(), 0xB8);
		uint8_t *pool = PTR(CUR(), 0xBC);
		uint8_t *ctrl = blob::GetBone(S16(desc, 6));
		int32_t ax = S16(ctrl, 0x8C);
		int32_t az = S16(ctrl, 0x90);
		int32_t ay = S16(ctrl, 0x8E);
		U16(WS(), 0xA0) = (uint16_t)div256(mul32(S16(desc, 0x0A), ax));
		U16(WS(), 0xA2) = (uint16_t)div256(mul32(S16(desc, 0x12), ay));
		U16(WS(), 0xA4) = (uint16_t)div256(mul32(S16(desc, 0x1A), az));
		U16(WS(), 0xA8) = (uint16_t)div256(mul32(S16(desc, 0x0C), ax));
		U16(WS(), 0xAA) = (uint16_t)div256(mul32(S16(desc, 0x14), ay));
		U16(WS(), 0xAC) = (uint16_t)div256(mul32(S16(desc, 0x1C), az));
		U16(WS(), 0xB0) = (uint16_t)div256(mul32(S16(desc, 0x0E), ax));
		U16(WS(), 0xB2) = (uint16_t)div256(mul32(S16(desc, 0x16), ay));
		U16(WS(), 0xB4) = (uint16_t)div256(mul32(S16(desc, 0x1E), az));
		U16(WS(), 0xB8) = (uint16_t)div256(mul32(S16(desc, 0x10), ax));
		U16(WS(), 0xBA) = (uint16_t)div256(mul32(S16(desc, 0x18), ay));
		U16(WS(), 0xBC) = (uint16_t)div256(mul32(S16(desc, 0x20), az));
		PTR(WS(), 0xE0) = pool + 0x10;
		int32_t widx = U16(pool, 2);
		int32_t count = S16(ctrl, 0x94);
		int32_t nring = U16(desc, 4);
		if (count > 0)
		{
			do
			{
				int32_t slot = widx % nring;
				uint8_t *p = PTR(WS(), 0xE0) + slot * 28;
				int32_t r = blob::Rand73(S16(CUR(), 0x8C));
				S32(p, 0) = shl32(add32(r, S16(CUR(), 0x94)), 8);
				r = blob::Rand73(S16(CUR(), 0x8E));
				S32(p, 4) = shl32(add32(r, S16(CUR(), 0x96)), 8);
				r = blob::Rand73(S16(CUR(), 0x90));
				S32(p, 8) = shl32(add32(r, S16(CUR(), 0x98)), 8);
				r = blob::Rand73(S16(WS(), 0xA8));
				U16(p, 0x0C) = (uint16_t)(U16(WS(), 0xA0) + (uint16_t)r);
				r = blob::Rand73(S16(WS(), 0xB8));
				U16(p, 0x0E) = (uint16_t)(U16(WS(), 0xB0) + (uint16_t)r);
				r = blob::Rand73(S16(WS(), 0xAA));
				U16(p, 0x10) = (uint16_t)(U16(WS(), 0xA2) + (uint16_t)r);
				r = blob::Rand73(S16(WS(), 0xBA));
				U16(p, 0x12) = (uint16_t)(U16(WS(), 0xB2) + (uint16_t)r);
				r = blob::Rand73(S16(WS(), 0xAC));
				U16(p, 0x14) = (uint16_t)(U16(WS(), 0xA4) + (uint16_t)r);
				r = blob::Rand73(S16(WS(), 0xBC));
				U16(p, 0x16) = (uint16_t)(U16(WS(), 0xB4) + (uint16_t)r);
				U8(p, 0x18) = U8(desc, 8);
				widx++;
			} while (--count != 0);
		}
		U16(pool, 2) = (uint16_t)widx;
		int32_t bias = S16(CUR(), 0xC0);
		if (bias == 0)
			S32(WS(), 0x64) = 0;
		else
			U32(WS(), 0x64) = ((uint32_t)bias & 0x3FFC) + U32(RT(), 0x4C);
		Mat4x3 *m = blob::GetParentMatrix(U16(CUR(), 0x9C));
		x::SetRotMatrix(m);
		x::SetTransVector(m);
		U32(WS(), 0xE4) = U32(pool, 4);
		uint8_t *p = PTR(WS(), 0xE0);
		int32_t n = U16(desc, 4);
		uint8_t *pkt = PTR(CTX(), 0x7C);
		if (n > 0)
		{
			do
			{
				if (!(U8(p, 0x18) & 0x80))
				{
					int32_t px, py, pz;
					int32_t cidx;
					U16(p, 0x0C) = (uint16_t)(U16(p, 0x0C) + U16(p, 0x0E));
					S32(p, 0) = add32(S32(p, 0), shl32(S16(p, 0x0C), 4));
					U16(p, 0x10) = (uint16_t)(U16(p, 0x10) + U16(p, 0x12));
					S32(p, 4) = add32(S32(p, 4), shl32(S16(p, 0x10), 4));
					U16(p, 0x14) = (uint16_t)(U16(p, 0x14) + U16(p, 0x16));
					S32(p, 8) = add32(S32(p, 8), shl32(S16(p, 0x14), 4));
					px = S32(p, 0); py = S32(p, 4); pz = S32(p, 8);
					cidx = S8(p, 0x18);
					uint8_t *v = WS() + 0xF0;
					if (U16(desc, 2) == 0)
					{
						U16(v, 0) = (uint16_t)(px >> 8);
						U16(v, 2) = (uint16_t)(py >> 8);
						U16(v, 4) = (uint16_t)(pz >> 8);
					}
					if (U16(desc, 2) == 1)
					{
						int32_t rad = px >> 8;
						U16(v, 2) = (uint16_t)(py >> 8);
						int32_t ang = (pz >> 4) & (int32_t)0xFFFFFFF0;
						int32_t s = x::Sin(ang);
						U16(v, 0) = (uint16_t)((uint16_t)(mul32(s, rad) >> 12) + U16(CUR(), 0x94));
						int32_t c = x::Cos(ang);
						U16(v, 4) = (uint16_t)((uint16_t)(mul32(c, rad) >> 12) + U16(CUR(), 0x98));
					}
					U8(pkt, 3) = 2;
					U32(pkt, 4) = U32(PTR(WS(), 0xE4), cidx * 4) | 0x6A000000;
					int32_t z = xl_TransformToScreen(WS() + 0xF0, pkt + 8, WS() + 0xFC, WS() + 0xFC);
					S16(pkt, 8) = PsxPixels(S16(pkt, 8));
					S16(pkt, 0xA) = PsxPixels(S16(pkt, 0xA));
					uint32_t ot = U32(WS(), 0x64);
					if (ot == 0) ot = U32(RT(), 0x4C) + ((uint32_t)z & ~3u);
					x::InsertPrimAltViewport(ot, pkt);
					U8(p, 0x18) = (uint8_t)(U8(p, 0x18) - 1);
					pkt += 0xC;
				}
				p += 0x1C;
			} while (--n != 0);
		}
		PTR(CTX(), 0x7C) = pkt;
	}

	static void __cdecl dh_54_DrawEnvClipWindow()
	{
		if (PTR(CUR(), 0xBC) == nullptr)
			PTR(CUR(), 0xBC) = blob::ArenaAlloc(0x100);
		uint32_t idx = (uint32_t)(uint8_t)(FRAMEPARITY() - 1) & 1;
		uint8_t *desc = PTR(CUR(), 0xB8);
		uint8_t *src = DRAWENVS() + idx * 92;
		uint8_t *env = PTR(CTX(), 0x74);
		for (int k = 0; k < 7; k++) U32(env, 4 * k) = U32(src, 4 * k);
		int32_t pp = S8(RT(), 0x41);
		uint8_t *buf = PTR(CUR(), 0xBC);
		U16(env, 8) = U16(desc, 2);
		U16(env, 0) = U16(desc, 2);
		U16(env, 0xA) = U16(desc, 4);
		U16(env, 2) = U16(desc, 4);
		uint8_t *pkt;
		pkt = buf + shl32(pp, 7);
		U16(env, 4) = U16(desc, 6);
		U16(env, 6) = 0xE0;
		U16(env, 0x16) = 0x101;
		U32(env, 0x18) = 1;
		xl_SetDrawEnv(pkt, env);
		uint8_t *pkt2 = pkt + 0x40;
		xl_SetDrawEnv(pkt2, src);
		xl_InsertPrimAutoDepth(((uint32_t)U16(desc, 0) & 0x3FFC) + U32(RT(), 0x4C), pkt2);
		xl_InsertPrimAutoDepth(U32(RT(), 0x4C) + 0x3FC0, pkt);
	}

	static void ScanBar(int32_t row, int32_t h, int32_t x0, uint32_t colour)
	{
		if (S8(PTR(CTX(), 0x74), row + 0x200) < 0) return;
		uint8_t *p = PTR(CTX(), 0x7C);
		U8(p, 3) = 9;
		U32(p, 4) = colour;
		U16(p, 0x16) = U16(WS(), 0x64);
		int32_t xa = sub32(x0, 0x10);
		U16(p, 0x18) = (uint16_t)xa;
		U16(p, 8) = (uint16_t)xa;
		uint16_t xb = (uint16_t)(U16(WS(), 0x60) + (uint16_t)xa);
		U16(p, 0x12) = (uint16_t)row;
		U16(p, 0x20) = xb;
		U16(p, 0x10) = xb;
		int32_t y1 = add32(h, row);
		uint8_t v1 = (uint8_t)((uint8_t)h + (uint8_t)row);
		U16(p, 0x22) = (uint16_t)y1;
		U16(p, 0x1A) = (uint16_t)y1;
		U16(p, 0xA) = (uint16_t)row;
		U8(p, 0x1C) = 0;
		U8(p, 0xC) = 0;
		U8(p, 0x24) = 0xFF;
		U8(p, 0x14) = 0xFF;
		U8(p, 0x15) = (uint8_t)row;
		U8(p, 0xD) = (uint8_t)row;
		U8(p, 0x25) = v1;
		U8(p, 0x1D) = v1;
		x::InsertPrimAltViewport(U32(RT(), 0x4C), p);
		PTR(CTX(), 0x7C) = p + 0x28;
	}

	static void __cdecl dh_55_ScanlineJitterScreen()
	{
		uint8_t *bone = CUR();
		if (U32(bone, 0xBC) == 0)
		{
			uint8_t *a = blob::ArenaAlloc(0x1B8);
			PTR(CUR(), 0xBC) = a;
			uint8_t *d = PTR(CUR(), 0xB8);
			uint8_t *pool = PTR(CUR(), 0xBC);
			int32_t range = S16(d, 4);
			for (int32_t i = 0; i < 0x1B0; i += 2)
				U16(pool, i + 8) = (uint16_t)blob::Rand73(range);
			U16(pool, 0) = (uint16_t)blob::Rand73(0xEA60);
			U16(pool, 2) = (uint16_t)blob::Rand73(0xEA60);
			U16(pool, 4) = (uint16_t)blob::Rand73(0xEA60);
			bone = CUR();
		}
		uint8_t *desc = PTR(bone, 0xB8);
		uint8_t *pool = PTR(bone, 0xBC);
		{
			int16_t amp = S16(bone, 0x98);
			uint8_t *s = PTR(CTX(), 0x74);
			for (int i = 0; i < 0xD8; i++)
				U16(s, 2 * i) = (uint16_t)(S16(pool, 8 + 2 * i) * amp);
		}
		{
			uint8_t *s = PTR(CTX(), 0x74);
			int32_t v = S16(CUR(), 0x96);
			if (v > 0)
			{
				int32_t acc = mul32(v, -108);
				for (int k = 0; k < 0xD8; k++)
				{
					U8(s, 0x200 + k) = 0;
					U8(s, 0x300 + k) = (uint8_t)((uint8_t)(acc >> 8) + 0x6C);
					acc = add32(acc, v);
				}
			}
			int32_t n = 0x6C - (mul32(S16(CUR(), 0x96), 108) >> 8);
			s = PTR(CTX(), 0x74);
			uint8_t *up = s + 0x200;
			uint8_t *dn = s + 0x2D7;
			if (n > 0)
			{
				do { *dn = 0x80; *up = 0x80; up++; dn--; } while (--n != 0);
			}
			if (S16(CUR(), 0x96) < 0)
			{
				s = PTR(CTX(), 0x74);
				for (int k = 0; k < 0xD8; k++)
				{
					U8(s, 0x200 + k) = 0x80;
					U8(s, 0x300 + k) = 0x6C;
				}
				U8(s, 0x200 + 0x6C) = 0;
			}
		}
		for (int k = 0; k < 3; k++)
		{
			uint16_t ph;
			{
				U16(pool, 2 * k) = (uint16_t)(U16(pool, 2 * k) + U16(desc, 6 + 2 * k));
				ph = U16(pool, 2 * k);
			}
			U32(WS(), 0x80 + 4 * k) = (uint32_t)ph >> 8;
		}
		{
			uint32_t t = (((uint32_t)U16(desc, 0) & 0xFC0) | 0x4000) >> 2;
			t |= (uint32_t)U16(desc, 2) & 0x100;
			uint32_t b92 = U8(CUR(), 0x92) & 0x60u;
			U32(WS(), 0x64) = (t >> 4) | b92;
		}
		U32(WS(), 0x6C) = U32(CUR(), 0xCC) & 0x2000000;
		U32(WS(), 0x70) = xl_ScaleColour_B66A20(U32(WS(), 0x6C) | 0x24804040, S16(CUR(), 0x8E));
		U32(WS(), 0x74) = xl_ScaleColour_B66A20(U32(WS(), 0x6C) | 0x24404060, S16(CUR(), 0x8E));
		{
			int32_t bx = S16(CUR(), 0x94);
			if (bx < 0xA0)
			{
				uint8_t *p = PTR(CTX(), 0x7C);
				int32_t w = sub32(0xA0, bx);
				if (w < 0) w = 0xA0;
				U8(p, 3) = 5;
				U32(p, 4) = 0x28000000;
				U32(p, 8) = 0;
				S32(p, 0xC) = w;
				U32(p, 0x10) = 0xD00000;
				S32(p, 0x14) = add32(w, 0xD00000);
				x::InsertPrimAltViewport(U32(RT(), 0x4C), p);
				p += 0x18;
				U8(p, 3) = 5;
				U32(p, 4) = 0x28000000;
				U32(p, 8) = 0x140;
				S32(p, 0xC) = sub32(0x140, w);
				U32(p, 0x10) = 0xD00140;
				S32(p, 0x14) = sub32(0xD00140, w);
				x::InsertPrimAltViewport(U32(RT(), 0x4C), p);
				PTR(CTX(), 0x7C) = p + 0x18;
			}
		}
		S32(WS(), 0x60) = S16(CUR(), 0x8C);
		int32_t x0 = -(sub32(S32(WS(), 0x60), 0x140) / 2);
		{
			uint8_t *s = PTR(CTX(), 0x74);
			uint8_t *flags = s + 0x200;
			uint8_t *map = s + 0x300;
			uint8_t *p = PTR(CTX(), 0x7C);
			uint8_t *jit = s;
			for (int32_t i = 0; i < 0xD8; i++, jit += 2)
			{
				uint32_t row = map[i];
				if (flags[row] != 0) continue;
				flags[row] = 1;
				uint32_t row1 = row + 1;
				U16(p, 0x12) = (uint16_t)row;
				U16(p, 0xA) = (uint16_t)row;
				U16(p, 0x1A) = (uint16_t)row1;
				U8(p, 3) = 7;
				U32(p, 4) = U32(WS(), 0x70);
				U16(p, 0x16) = U16(WS(), 0x64);
				int32_t xa = add32(S16(jit, 0) >> 8, x0);
				U16(p, 8) = (uint16_t)xa;
				uint16_t xb = (uint16_t)(U16(WS(), 0x60) + (uint16_t)xa);
				U8(p, 0xC) = 0;
				uint8_t v1 = (uint8_t)(i + 1);
				U16(p, 0x18) = xb;
				U16(p, 0x10) = xb;
				U8(p, 0x1C) = 0xFF;
				U8(p, 0x14) = 0xFF;
				U8(p, 0x15) = (uint8_t)i;
				U8(p, 0xD) = (uint8_t)i;
				U8(p, 0x1D) = v1;
				x::InsertPrimAltViewport(U32(RT(), 0x4C), p);
				p += 0x20;
				U8(p, 3) = 7;
				U32(p, 4) = U32(WS(), 0x74);
				U16(p, 0x16) = U16(WS(), 0x64);
				xa = add32(S16(jit, 0) >> 9, x0);
				U16(p, 8) = (uint16_t)xa;
				xb = (uint16_t)(U16(WS(), 0x60) + (uint16_t)xa);
				U16(p, 0x12) = (uint16_t)row;
				U16(p, 0x18) = xb;
				U16(p, 0x10) = xb;
				U16(p, 0xA) = (uint16_t)row;
				U16(p, 0x1A) = (uint16_t)row1;
				U8(p, 0xC) = 0;
				U8(p, 0x1C) = 0xFF;
				U8(p, 0x14) = 0xFF;
				U8(p, 0x15) = (uint8_t)i;
				U8(p, 0xD) = (uint8_t)i;
				U8(p, 0x1D) = v1;
				x::InsertPrimAltViewport(U32(RT(), 0x4C), p);
				p += 0x20;
			}
			PTR(CTX(), 0x7C) = p;
		}
		if (U16(desc, 6) != 0) ScanBar(S32(WS(), 0x80), 1, x0, U32(WS(), 0x6C) | 0x2C70B0B0);
		if (U16(desc, 8) != 0) ScanBar(S32(WS(), 0x84), 1, x0, U32(WS(), 0x6C) | 0x2E70C070);
		if (U16(desc, 0xA) != 0) ScanBar(S32(WS(), 0x88), 1, x0, U32(WS(), 0x6C) | 0x2EF0F0F0);
	}

	static void __cdecl dh_56_LineRingTunnel()
	{
		if (U32(CUR(), 0xBC) == 0)
		{
			uint8_t *d = PTR(CUR(), 0xB8);
			uint8_t *a = blob::ArenaAlloc(0x4C);
			PTR(CUR(), 0xBC) = a;
			uint8_t *pool = PTR(CUR(), 0xBC);
			PTR(pool, 0) = d + S16(d, 0);
			U16(pool, 4) = 1;
			uint8_t *obj = blob::ObjectPtr(U16(d, 4));
			uint8_t *verts = obj + U32(obj, 0x14);
			const uint8_t *ids = d + 0xA;
			for (int k = 0; k < 16; k++)
			{
				uint32_t id = U16(ids, 2 * k);
				U16(pool, 8 + 2 * k) = U16(verts, id * 8);
				U16(pool, 0x2A + 2 * k) = U16(verts, id * 8 + 4);
			}
			uint8_t *v0 = verts + (uint32_t)U16(ids, 0) * 8;
			U16(pool, 0x28) = U16(v0, 0);
			U16(pool, 0x4A) = U16(v0, 4);
		}
		PTR(WS(), 0x80) = PTR(CUR(), 0xB8);
		PTR(WS(), 0x84) = PTR(CUR(), 0xBC);
		uint8_t *desc = PTR(WS(), 0x80);
		uint8_t *pool = PTR(WS(), 0x84);
		S32(WS(), 0x70) = S16(CUR(), 0x8E);
		U32(WS(), 0x74) = (uint32_t)U16(CUR(), 0x96) & 0x7FFF;
		PTR(WS(), 0x64) = PTR(CTX(), 0x74);
		PTR(WS(), 0x60) = PTR(WS(), 0x64);
		int32_t mask = (int32_t)U16(desc, 8);
		int32_t lx = S16(CUR(), 0x94);
		int32_t q = S32(WS(), 0x74) / S32(WS(), 0x70);
		uint8_t *s = PTR(WS(), 0x60);
		mask = sub32(mask, 1);
		{
			int32_t r = 0;
			do
			{
				int32_t c = r;
				for (int k = 0; k < 17; k++)
				{
					if (c > lx)
						*s = 0x80;
					else
						*s = (uint8_t)(((uint8_t)c - (uint8_t)q + (uint8_t)lx) & (uint8_t)mask);
					s++;
					c++;
				}
				r++;
			} while (r <= (int32_t)U16(desc, 6));
		}
		s = (uint8_t *)(((uint32_t)s + 4) & ~3u);
		PTR(WS(), 0x68) = s;
		int32_t step, accel;
		{
			uint8_t *w = WS();
			int32_t sp = S32(w, 0x70);
			int32_t rem = S32(w, 0x74) % sp;
			int32_t half = mul32((int32_t)U16(desc, 6), sp) / 2;
			step = rem;
			U16(w, 0xD2) = (uint16_t)sub32(rem, half);
			accel = shl32(rem, 12) / S32(WS(), 0x70);
		}
		if (U16(desc, 2) == 0)
		{
			int32_t r0 = shl32(S16(CUR(), 0x8C), 12);
			S32(WS(), 0xF8) = r0;
			int32_t d = sub32(shl32(S16(CUR(), 0x90), 12), r0) / (int32_t)U16(desc, 6);
			int32_t f = mul32(accel, d);
			accel = 0;
			step = d;
			S32(WS(), 0xF8) = add32(S32(WS(), 0xF8), f >> 12);
		}
		if (U16(desc, 2) == 1)
		{
			S32(WS(), 0xF8) = shl32(S16(CUR(), 0x8C), 12);
			step = shl32(S16(CUR(), 0x90), 4);
			accel = shl32(S16(CUR(), 0x98), 4);
		}
		Mat4x3 *m = blob::GetParentMatrix(U16(CUR(), 0x9C));
		x::GteSetRotMatrix(m);
		x::GteSetTransVector(m);
		x::GteLoadV0(ZEROVEC());
		x::GteMVMVA_RotV0Tr();
		x::Gte_45E580();
		S32(WS(), 0xC4) = 0x1000;
		{
			int32_t r = 0;
			do
			{
				for (int k = 0; k < 5; k++) U32(WS(), 0xE0 + 4 * k) = U32(m, 4 * k);
				S32(WS(), 0xC8) = S32(WS(), 0xF8) >> 8;
				S32(WS(), 0xC0) = S32(WS(), 0xC8);
				x::ScaleMatrix(WS() + 0xE0, WS() + 0xC0);
				x::SetRotMatrix(WS() + 0xE0);
				for (int c = 0; c < 17; c++)
				{
					U16(WS(), 0xD0) = U16(pool, 8 + 2 * c);
					U16(WS(), 0xD4) = U16(pool, 0x2A + 2 * c);
					int32_t z = xl_TransformToScreen(WS() + 0xD0, s, WS() + 0xFC, WS() + 0xFC);
					U16(s, 4) = (uint16_t)z;
					s += 8;
				}
				step = add32(step, accel);
				S32(WS(), 0xF8) = add32(S32(WS(), 0xF8), step);
				U16(WS(), 0xD2) = (uint16_t)(U16(WS(), 0xD2) + U16(WS(), 0x70));
				r++;
			} while (r <= (int32_t)U16(desc, 6));
		}
		PTR(WS(), 0x6C) = s;
		{
			int32_t rows = U16(desc, 6);
			int32_t sp = S32(WS(), 0x70);
			int32_t rem = S32(WS(), 0x74) % sp;
			int32_t inv = 0x10000 / sp;
			int32_t a = sub32(shl32(rows, 6), mul32(rem, inv) >> 10);
			if (rows >= 0)
			{
				int32_t cnt = rows + 1;
				do
				{
					if (a < 0) a = 0;
					S32(s, 0) = a;
					s += 4;
					a = sub32(a, 0x40);
				} while (--cnt != 0);
			}
		}
		uint8_t *pkt = PTR(CTX(), 0x7C);
		S32(WS(), 0xF0) = sub32((int32_t)U16(desc, 8), 1);
		uint8_t *ci = PTR(WS(), 0x64);
		uint8_t *pv = PTR(WS(), 0x68);
		uint8_t *ctab = PTR(pool, 0);
		if (U16(desc, 6) != 0)
		{
			int32_t r = 0;
			do
			{
				int32_t a = S32(PTR(WS(), 0x6C), 4 * r);
				for (int c = 0; c < 16; c++)
				{
					int32_t idx = S8(ci, 0);
					if (idx >= 0)
					{
						uint32_t col = U32(ctab, 4 * idx);
						if (a < 0x100) col = xl_ScaleColour_B66A20(col, a);
						idx++;
						U32(pkt, 0xC) = col | 0x52000000;
						col = U32(ctab, 4 * (S32(WS(), 0xF0) & idx));
						if (a < 0x100) col = xl_ScaleColour_B66A20(col, a);
						U32(pkt, 0x14) = col;
						U32(pkt, 4) = 0xE1000220;
						U8(pkt, 3) = 6;
						U32(pkt, 8) = 0;
						U32(pkt, 0x10) = U32(pv, 0);
						U32(pkt, 0x18) = U32(pv, 8);
						int32_t d1 = U16(pv, 0xC), d0 = U16(pv, 4);
						uint32_t ot = ((uint32_t)(add32(d1, d0) / 2) & ~3u) + U32(RT(), 0x4C);
						x::InsertPrimDepthKeys(ot, pkt, d0, d1, 0, 0);
						pkt += 0x1C;
					}
					pv += 8;
					ci++;
				}
				pv += 8;
				ci++;
				r++;
			} while (r < (int32_t)U16(desc, 6));
		}
		S32(WS(), 0xF4) = 0x88;
		ci = PTR(WS(), 0x64);
		pv = PTR(WS(), 0x68);
		ctab = PTR(pool, 0);
		if (U16(desc, 6) != 0)
		{
			int32_t r = 0;
			do
			{
				uint8_t *ar = PTR(WS(), 0x6C) + 4 * r;
				for (int c = 0; c < 16; c++)
				{
					int32_t idx = S8(ci, 0);
					if (idx >= 0)
					{
						uint32_t col = U32(ctab, 4 * idx);
						int32_t a = S32(ar, 0);
						if (a < 0x100) col = xl_ScaleColour_B66A20(col, a);
						U32(pkt, 0xC) = col | 0x52000000;
						idx++;
						col = U32(ctab, 4 * (S32(WS(), 0xF0) & idx));
						a = S32(ar, 4);
						if (a < 0x100) col = xl_ScaleColour_B66A20(col, a);
						U32(pkt, 0x14) = col;
						U32(pkt, 4) = 0xE1000220;
						U8(pkt, 3) = 6;
						U32(pkt, 8) = 0;
						U32(pkt, 0x10) = U32(pv, 0);
						U32(pkt, 0x18) = U32(pv, S32(WS(), 0xF4));
						int32_t d1 = U16(pv, 0xC), d0 = U16(pv, 4);
						uint32_t ot = ((uint32_t)(add32(d1, d0) / 2) & ~3u) + U32(RT(), 0x4C);
						x::InsertPrimDepthKeys(ot, pkt, d0, d1, 0, 0);
						pkt += 0x1C;
					}
					ci++;
					pv += 8;
				}
				pv += 8;
				ci++;
				r++;
			} while (r < (int32_t)U16(desc, 6));
		}
		PTR(CTX(), 0x7C) = pkt;
	}

	static void __cdecl dh_58_RequestScreenFeedback()
	{
		xl_RequestScreenFeedback(1, 0xFF, 0xFF, 0xFF, 0x7F);
	}

	static void __cdecl dh_64_PlaceModelLerpBulge()
	{
		uint8_t *bone = CUR();
		uint8_t *desc = PTR(bone, 0xB8);
		uint8_t *e = PTR(SCENE(), 0x60 + 4 * (int32_t)U8(bone, 0x1B));
		U8(e, 1) |= 0x10;
		uint8_t *home = SCENE() + 0x84 + 8 * (int32_t)U8(CUR(), 0x1B);
		int32_t px = add32(div256(mul32(sub32(S16(home, 0), S16(desc, 6)), S16(desc, 4))), S16(desc, 6));
		int32_t py = add32(div256(mul32(sub32(S16(home, 2), S16(desc, 8)), S16(desc, 4))), S16(desc, 8));
		int32_t pz = add32(div256(mul32(sub32(S16(home, 4), S16(desc, 0xA)), S16(desc, 4))), S16(desc, 0xA));
		int32_t dx = sub32(px, S16(desc, 6));
		int32_t dy = sub32(py, S16(desc, 8));
		int32_t r1 = xl_Sqrt(add32(mul32(dy, dy), mul32(dx, dx)));
		int32_t dz = sub32(pz, S16(desc, 0xA));
		int32_t r = xl_Sqrt(add32(mul32(dz, dz), mul32(r1, r1)));
		int32_t h = S16(desc, 2);
		if (r < h)
		{
			int32_t k = mul32(sub32(h, r), S16(desc, 0)) / h;
			py = add32(py, div256(mul32(k, S16(CUR(), 0x94))));
		}
		U16(e, 0x1C) = (uint16_t)px;
		U16(e, 0x1E) = (uint16_t)py;
		U16(e, 0x20) = (uint16_t)pz;
		uint8_t *sc = SCENE() + 0x120 + 8 * (int32_t)U8(CUR(), 0x1B);
		int32_t t = S16(desc, 4);
		U16(e, 0x94) = (uint16_t)div256(mul32(S16(sc, 0), t));
		U16(e, 0x96) = (uint16_t)div256(mul32(S16(sc, 2), t));
		U16(e, 0x98) = (uint16_t)div256(mul32(S16(sc, 4), t));
	}

	static void __cdecl dh_65_BattleModelSyncTransform()
	{
		S32(WS(), 0xF0) = S16(PTR(CUR(), 0xB8), 0);
		uint8_t *e = PTR(SCENE(), 0x60 + 4 * (int32_t)U8(CUR(), 0x1B));
		U8(e, 1) |= 0x10;
		U16(e, 0x1C) = U16(CUR(), 0x94);
		U16(e, 0x1E) = U16(CUR(), 0x96);
		U16(e, 0x20) = U16(CUR(), 0x98);
		uint8_t *sc = SCENE() + 0x120 + 8 * (int32_t)U8(CUR(), 0x1B);
		U16(e, 0x94) = (uint16_t)div256(mul32(S32(WS(), 0xF0), S16(sc, 0)));
		U16(e, 0x96) = (uint16_t)div256(mul32(S16(sc, 2), S32(WS(), 0xF0)));
		U16(e, 0x98) = (uint16_t)div256(mul32(S16(CUR(), 0x8C), S16(sc, 4)));
		int32_t i = U8(CUR(), 0x1B);
		uint8_t *scene = SCENE();
		U16(e, 0x0C) = 0;
		uint8_t *rot = scene + 0xD8 + 8 * i;
		U16(e, 0x0E) = (uint16_t)((uint16_t)(U16(CUR(), 0x8E) << 4) + U16(rot, 2));
		U16(e, 0x10) = (uint16_t)((uint16_t)(U16(CUR(), 0x90) << 4) + U16(rot, 4));
	}
}

	void apply_eden_c(Clone &c)
	{
		c.draw[53] = part_eden_c::dh_53_PixelParticleEmitter;
		c.draw[54] = part_eden_c::dh_54_DrawEnvClipWindow;
		c.draw[55] = part_eden_c::dh_55_ScanlineJitterScreen;
		c.draw[56] = part_eden_c::dh_56_LineRingTunnel;
		c.draw[58] = part_eden_c::dh_58_RequestScreenFeedback;
		c.draw[64] = part_eden_c::dh_64_PlaceModelLerpBulge;
		c.draw[65] = part_eden_c::dh_65_BattleModelSyncTransform;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace part_eden_d
{
	static inline void *xl_ComposeZYXRotationMatrix(const void *angles, void *out) { return x::f<void *(__cdecl *)(const void *, void *)>(game_addr(game::ComposeZYXRotationMatrix))(angles, out); }
	static inline void xl_RotTransIR(const void *v, void *ir, void *flag) { x::f<void (__cdecl *)(const void *, void *, void *)>(game_addr(game::GTE_RotTrans))(v, ir, flag); }
	static inline void xl_Apply3DTransform(const void *m, const void *v, void *out) { x::f<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::apply3DTransform))(m, v, out); }
	static inline int32_t xl_RotTransPers(const void *v, void *sxy, void *p, void *flag) { return x::f<int32_t (__cdecl *)(const void *, void *, void *, void *)>(game_addr(game::TransformWorldCoordinateToProjectedSpace))(v, sxy, p, flag); }
	static inline int32_t xl_NormalClip(uint32_t s0, uint32_t s1, uint32_t s2) { return x::f<int32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_NormalClip3))(s0, s1, s2); }
	static inline void xl_QueueBlitCommand(const void *rect, int32_t xw, int32_t yw) { x::f<void (__cdecl *)(const void *, int32_t, int32_t)>(game_addr(game::QueueBlitCommand))(rect, xw, yw); }
	static inline int32_t xl_SdStreamingVolumeTranslation(int32_t a, int32_t b) { return x::f<int32_t (__cdecl *)(int32_t, int32_t)>(game_addr(game::SdStreamingVolumeTranslation))(a, b); }

	static inline uint8_t *ZEROVEC() { return (uint8_t *)game_addr(game::unk_27971E4); }
	static inline uint16_t GTE_SZ3_LO() { return MEM<uint16_t>(game_addr(game::GTE_SZ3)); }
	static constexpr game::Id BLITRING = game::g_GfCinematic_BlitRectRing;
	static constexpr game::Id BLITRING_IDX = game::g_GfCinematic_BlitRectRingIdx;

	static inline int32_t DepthKey(uint32_t off)
	{
		uint32_t v = DEPTHARR()[off >> 3] & 0xFFFF;
		return (int32_t)(int64_t)((double)v * (double)FLT_1877DA8());
	}

	static inline int32_t Div3(int32_t v)
	{
		int32_t hi = (int32_t)(((int64_t)v * (int64_t)0x55555556) >> 32);
		return sub32(hi, v >> 31);
	}

	static void __cdecl dh_57_MeshRotScaledOrder3()
	{
		uint8_t *ang = WS() + 0x90;
		uint8_t *pos = CUR() + 0x94;
		U16(ang, 0) = U16(CUR(), 0x8C);
		uint16_t ay = U16(CUR(), 0x8E);
		U16(ang, 4) = 0;
		U16(ang, 2) = ay;
		int32_t scale = shl32(S16(CUR(), 0x90), 4);
		h_B27360(pos, ang, scale, 3);
		S32(WS(), 0x48) = 0;
		S32(WS(), 0x44) = 0;
		S32(WS(), 0x40) = 0;
		U32(WS(), 0x4C) = U32(CUR(), 0xBC);
		h_B27440(nullptr);
	}

	static uint8_t *EmitLine_AE8B70(const uint8_t *a, const uint8_t *b, uint8_t *pkt)
	{
		U8(pkt, 3) = 3;
		U32(pkt, 4) = U32(WS(), 0xF0);
		U32(pkt, 8) = U32(a, 0);
		U32(pkt, 0xC) = U32(b, 0);
		uint32_t ot = U32(WS(), 0xF4);
		if (ot == 0)
		{
			int32_t zb = U16(b, 4);
			int32_t za = U16(a, 4);
			int32_t k = (zb + za) / 2;
			k >>= 2;
			uint32_t key = (uint32_t)k & 0xFFFFFFFCu;
			x::InsertPrimDepthKeys(key + U32(RT(), 0x4C), pkt, za, zb, 0, 0);
			return pkt + 0x10;
		}
		int32_t zb = U16(b, 4);
		int32_t za = U16(a, 4);
		x::InsertPrimDepthKeys(ot, pkt, za, zb, 0, 0);
		return pkt + 0x10;
	}

	static void EmitQuadEdges_AE8A30(uint8_t *base, uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3)
	{
		uint32_t f2 = U8(base + i2, 6);
		uint32_t f1 = U8(base + i1, 6);
		uint32_t f3 = U8(base + i3, 6);
		uint8_t f0 = U8(base + i0, 6);
		uint8_t *pkt = PTR(CTX(), 0x7C);
		if ((uint8_t)((f0 & 2) | (f1 & 1)) != 3)
		{
			U8(base + i0, 6) = (uint8_t)(f0 | 2);
			U8(base + i1, 6) |= 1;
			pkt = EmitLine_AE8B70(base + i0, base + i1, pkt);
		}
		if ((uint8_t)((f3 & 4) | (f2 & 8)) != 0xC)
		{
			U8(base + i2, 6) |= 8;
			U8(base + i3, 6) |= 4;
			pkt = EmitLine_AE8B70(base + i2, base + i3, pkt);
		}
		U8(base + i0, 6) |= 4;
		U8(base + i2, 6) |= 1;
		pkt = EmitLine_AE8B70(base + i0, base + i2, pkt);
		if ((uint8_t)((f3 & 2) | (f1 & 8)) != 0xA)
		{
			U8(base + i1, 6) |= 8;
			U8(base + i3, 6) |= 2;
			pkt = EmitLine_AE8B70(base + i1, base + i3, pkt);
		}
		PTR(CTX(), 0x7C) = pkt;
	}

	static void __cdecl dh_63_MorphWireframeLines()
	{
		uint8_t *desc = PTR(CUR(), 0xB8);
		uint8_t *objA = blob::ObjectPtr(S16(desc, 0));
		uint8_t *objB = blob::ObjectPtr(S16(desc, 2));
		uint8_t *objC = blob::ObjectPtr(S16(desc, 4));
		uint8_t *dst = objC + U32(objC, 0x14);
		int32_t f = S16(CUR(), 0x94);
		uint8_t *src = objB + U32(objB, 0x14);
		uint8_t *v = PTR(CTX(), 0x74);
		int32_t n = S32(objA, 0x18);
		if (n > 0)
		{
			do
			{
				int32_t d = sub32(S16(dst, 0), S16(src, 0));
				S16(v, 0) = (int16_t)add32(mul32(d, f) / 256, S16(src, 0));
				d = sub32(S16(dst, 2), S16(src, 2));
				S16(v, 2) = (int16_t)add32(mul32(d, f) / 256, S16(src, 2));
				d = sub32(S16(dst, 4), S16(src, 4));
				S16(v, 4) = (int16_t)add32(mul32(d, f) / 256, S16(src, 4));
				src += 8;
				dst += 8;
				v += 8;
			} while (--n != 0);
		}
		h_B27130();
		v = PTR(CTX(), 0x74);
		n = S32(objA, 0x18);
		if (n > 0)
		{
			do
			{
				uint32_t xy = ((uint32_t)U16(v, 2) << 16) | (uint32_t)U16(v, 0);
				uint32_t z = U16(v, 4);
				x::GteWriteData((int32_t)xy, 0);
				x::GteWriteData((int32_t)z, 1);
				x::GteRTPS();
				x::GteReadSXY2(v);
				uint16_t sz = GTE_SZ3_LO();
				U16(v, 6) = 0;
				U16(v, 4) = sz;
				v += 8;
			} while (--n != 0);
		}
		U32(WS(), 0xF0) = U32(CUR(), 0xCC) | 0x40000000u;
		int32_t bias = S16(CUR(), 0xC0);
		if (bias == 0)
			U32(WS(), 0xF4) = 0;
		else
			U32(WS(), 0xF4) = ((uint32_t)bias & 0x3FFC) + U32(RT(), 0x4C);
		uint8_t *p = objA + U32(objA, 8);
		uint8_t *base = PTR(CTX(), 0x74);
		int32_t t = S16(p, 0);
		if (t == (int32_t)0xFFFF) return;
		for (;;)
		{
			if (t == 0x10)
			{
				int32_t cnt = S16(p, 2);
				p += 4;
				if (cnt > 0)
				{
					do
					{
						EmitQuadEdges_AE8A30(base, U16(p, 4), U16(p, 6), U16(p, 8), U16(p, 0xA));
						p += 0xC;
					} while (--cnt != 0);
				}
			}
			else if (t == 0x12)
			{
				int32_t cnt = S16(p, 2);
				p += 4;
				if (cnt > 0)
				{
					do
					{
						EmitQuadEdges_AE8A30(base, U16(p, 0xC), U16(p, 0xE), U16(p, 0x10), U16(p, 0x12));
						p += 0x18;
					} while (--cnt != 0);
				}
			}
			else
				return;
			t = S16(p, 0);
			if (t == (int32_t)0xFFFF) return;
		}
	}

	static void ShadowOctagon_AE4900(uint8_t *E, void *mat, const uint8_t *pos, int32_t scale)
	{
		(void)scale;
		uint8_t *ws = WS();
		U8(E, 0) |= 0x20;
		uint8_t *hdr = E + 0x60;
		x::BuildBoneMatricesFromPose(hdr);
		x::ComputeBonesWorldMatrices(hdr, mat);
		uint8_t *sk = PTR(PTR(E, 0x64), 0);
		int32_t i = 1;
		int32_t minX = sub32(S32(sk, 0x34), 0x1F4);
		int32_t maxX = add32(S32(sk, 0x34), 0x1F4);
		int32_t minZ = sub32(S32(sk, 0x3C), 0x1F4);
		int32_t maxZ = add32(S32(sk, 0x3C), 0x1F4);
		U32(ws, 0xD0) = 0;
		if (U8(sk, 0) > 1)
		{
			uint8_t *m = sk + 0x50;
			do
			{
				x::SetRotMatrix(m);
				x::SetTransVector(m);
				U16(ws, 0xD4) = U16(m, -0xE);
				xl_RotTransIR(ws + 0xD0, ws + 0xD8, ws + 0xFC);
				int32_t vx = S16(ws, 0xD8);
				if (vx < minX) minX = vx;
				else if (vx > maxX) maxX = vx;
				int32_t vz = S16(ws, 0xDC);
				if (vz < minZ) minZ = vz;
				else if (vz > maxZ) maxZ = vz;
				m += 0x30;
				i++;
			} while (i < (int32_t)U8(sk, 0));
		}
		int32_t cx = add32(minX, maxX) / 2;
		U16(ws, 0xD0) = (uint16_t)(U16(pos, 0) + (uint16_t)cx);
		int32_t halfX = sub32(maxX, cx);
		U16(ws, 0xD2) = 0;
		int32_t cz = add32(minZ, maxZ) / 2;
		int32_t halfZ = sub32(maxZ, cz);
		U16(ws, 0xD4) = (uint16_t)(U16(pos, 4) + (uint16_t)cz);
		x::SetRotMatrix(NODEMAT());
		x::SetTransVector(NODEMAT());
		uint8_t *flag = ws + 0xFC;
		xl_RotTransIR(ws + 0xD0, ws + 0xD8, flag);
		int32_t tx = S16(ws, 0xD8);
		int32_t ty = S16(ws, 0xDA);
		int32_t tz = S16(ws, 0xDC);
		x::GteWriteCtrl(tx, 5);
		x::GteWriteCtrl(ty, 6);
		x::GteWriteCtrl(tz, 7);
		uint8_t *V = ws + 0xB0;
		int32_t c = x::Cos(0x200);
		int32_t dz = mul32(c, halfZ) >> 12;
		int32_t dx = mul32(c, halfX) >> 12;
		U16(V, 4) = (uint16_t)halfZ;
		U16(V, 0xC) = (uint16_t)dz;
		U16(V, 8) = (uint16_t)dx;
		U16(V, 0x18) = (uint16_t)dx;
		uint16_t ndz = (uint16_t)(0u - (uint32_t)dz);
		uint16_t nhz = (uint16_t)(0u - (uint32_t)halfZ);
		uint16_t ndx = (uint16_t)(0u - (uint32_t)dx);
		U16(V, 0) = 0;
		U16(V, 2) = 0;
		U16(V, 0xA) = 0;
		U16(V, 0x10) = (uint16_t)halfX;
		U16(V, 0x12) = 0;
		U16(V, 0x14) = 0;
		U16(V, 0x1A) = 0;
		U16(V, 0x1C) = ndz;
		U16(V, 0x20) = 0;
		U16(V, 0x22) = 0;
		U16(V, 0x24) = nhz;
		U16(V, 0x28) = ndx;
		uint16_t nhx = (uint16_t)(0u - (uint32_t)halfX);
		U16(V, 0x38) = ndx;
		U16(V, 0x30) = nhx;
		U16(V, 0x2A) = 0;
		U16(V, 0x2C) = ndz;
		U16(V, 0x32) = 0;
		U16(V, 0x34) = 0;
		U16(V, 0x3A) = 0;
		U16(V, 0x3C) = (uint16_t)dz;
		U16(V, 0x40) = 0;
		U16(V, 0x42) = 0;
		U16(V, 0x44) = 0;
		uint8_t *v = V;
		for (int k = 9; k != 0; k--)
		{
			xl_RotTransPers(v, v, flag, flag);
			v += 8;
		}
		if (xl_NormalClip(U32(V, 0), U32(V, 0x10), U32(V, 0x20)) > 0)
		{
			uint32_t ot = FRAMERL() + 0x4064;
			uint8_t *p = PTR(CTX(), 0x7C);
			uint32_t col = U32(E, 0x2C);
			int32_t k = 8;
			for (;;)
			{
				U8(p, 3) = 8;
				U32(p, 8) = 0;
				U32(p, 4) = 0xE1000040u;
				U32(p, 0xC) = col;
				U32(p, 0x14) = 0;
				U32(p, 0x1C) = 0;
				U32(p, 0x10) = U32(V, 0x40);
				U32(p, 0x18) = U32(V, k - 8);
				U32(p, 0x20) = U32(V, k & 0x38);
				x::InsertPrimDepthKeys(ot, p, 0, 0, 0, 0);
				k += 8;
				p += 0x24;
				if (k - 8 >= 0x40) break;
			}
			PTR(CTX(), 0x7C) = p;
		}
		x::BuildBoneMatricesFromPose(hdr);
	}

	static void BattleModelPivotAttach()
	{
		uint8_t *b = CUR();
		uint8_t *E = PTR(SCENE(), 0x60 + 4 * (int32_t)U8(b, 0x1B));
		if (S32(b, 0xC4) == 0)
		{
			U32(b, 0xC4) = 0xFFFFFFFFu;
			uint16_t ax = U16(E, 0x54);
			U8(E, 1) |= 0x10;
			ax = (uint16_t)(ax - U16(E, 0x1C));
			U16(WS(), 0xC0) = ax;
			U16(WS(), 0xC2) = (uint16_t)(U16(E, 0x58) - U16(E, 0x1E));
			U16(WS(), 0xC4) = (uint16_t)(U16(E, 0x5C) - U16(E, 0x20));
			uint8_t *a = SCENE() + 0xD8 + 8 * (int32_t)U8(CUR(), 0x1B);
			U16(WS(), 0xD0) = (uint16_t)(0u - U16(a, 0));
			U16(WS(), 0xD2) = (uint16_t)(0u - U16(a, 2));
			U16(WS(), 0xD4) = (uint16_t)(0u - U16(a, 4));
			xl_ComposeZYXRotationMatrix(WS() + 0xD0, WS() + 0x60);
			x::SetRotMatrix(WS() + 0x60);
			x::SetTransVector(ZEROVEC());
			uint8_t *w = WS();
			xl_RotTransIR(w + 0xC0, CUR() + 0xBC, w + 0xFC);
			b = CUR();
		}
		uint8_t *scene = SCENE();
		int32_t idx = U8(b, 0x1B);
		uint8_t *pivot = b + 0xBC;
		uint8_t *base = scene + 0xD8 + 8 * idx;
		xl_ComposeZYXRotationMatrix(base, WS() + 0x60);
		S32(WS(), 0xF8) = S16(CUR(), 0x90);
		S32(WS(), 0xF4) = S32(WS(), 0xF8);
		S32(WS(), 0xF0) = S32(WS(), 0xF4);
		x::ScaleMatrix(WS() + 0x60, WS() + 0xF0);
		xl_Apply3DTransform(WS() + 0x60, pivot, WS() + 0x74);
		uint8_t *hdr = E + 0x60;
		x::BuildBoneMatricesFromPose(hdr);
		blob::Blob_B66B80(hdr, 0xF1, 0x1000, WS() + 0x60);
		GFC_TAINT(WS() + 0xE2, nullptr, 2);
		uint8_t *w = WS();
		int32_t j0 = S32(w, 0xF0);
		int32_t j1 = S32(w, 0xF4);
		int32_t j2 = S32(w, 0xF8);
		U16(w, 0xD0) = (uint16_t)(U16(CUR(), 0x8C) + U16(base, 0));
		U16(E, 0xC) = U16(WS(), 0xD0);
		U16(WS(), 0xD2) = (uint16_t)(U16(CUR(), 0x8E) + U16(base, 2));
		U16(E, 0xE) = U16(WS(), 0xD2);
		U16(WS(), 0xD4) = U16(base, 4);
		U16(E, 0x10) = U16(WS(), 0xD4);
		xl_ComposeZYXRotationMatrix(WS() + 0xD0, WS() + 0x60);
		S32(WS(), 0xF8) = S16(CUR(), 0x90);
		S32(WS(), 0xF4) = S32(WS(), 0xF8);
		S32(WS(), 0xF0) = S32(WS(), 0xF4);
		x::ScaleMatrix(WS() + 0x60, WS() + 0xF0);
		xl_Apply3DTransform(WS() + 0x60, pivot, WS() + 0x74);
		blob::Blob_B66B80(hdr, 0xF1, 0x1000, WS() + 0x60);
		GFC_TAINT(WS() + 0xE2, nullptr, 2);
		w = WS();
		uint32_t d1 = (uint32_t)sub32(S32(w, 0xF4), j1);
		uint32_t d2 = (uint32_t)sub32(S32(w, 0xF8), j2);
		uint8_t *out = w + 0x50;
		uint16_t bx = (uint16_t)(U16(CUR(), 0x94) - (uint16_t)S32(w, 0xF0));
		uint16_t v = (uint16_t)((uint32_t)j0 + bx);
		U16(out, 0) = v;
		U16(E, 0x1C) = v;
		v = (uint16_t)(U16(CUR(), 0x96) - (uint16_t)d1);
		U16(out, 2) = v;
		U16(E, 0x1E) = v;
		v = (uint16_t)(U16(CUR(), 0x98) - (uint16_t)d2);
		U16(out, 4) = v;
		U16(E, 0x20) = v;
		uint8_t *cb = CUR();
		uint8_t *sc = SCENE() + 0x120 + 8 * (int32_t)U8(cb, 0x1B);
		int32_t s = S16(cb, 0x90);
		S16(E, 0x94) = (int16_t)(mul32(S16(sc, 0), s) / 4096);
		S16(E, 0x96) = (int16_t)(mul32(S16(sc, 2), s) / 4096);
		S16(E, 0x98) = (int16_t)(mul32(S16(sc, 4), s) / 4096);
		if (U8(CUR(), 0x4A) & 1)
			ShadowOctagon_AE4900(E, WS() + 0x60, out, 0x1000);
		else
			U16(E, 0) &= 0xEFFF;
	}

	static void __cdecl dh_72_BattleModelPivotAttach()
	{
		BattleModelPivotAttach();
	}

	static void __cdecl pr_06_TintedF3()
	{
		h_B29450((int32_t)U32(WS(), 0x84));
		uint8_t *ws = WS();
		int32_t count = S32(ws, 0x70);
		uint8_t *rec = PTR(ws, 0x6C);
		uint8_t *pkt = PTR(ws, 0x60);
		uint32_t ot = U32(ws, 0x5C);
		uint32_t nocull = U32(ws, 0x90) & 0x10;
		U32(ws, 0xF0) = 0;
		do
		{
			uint8_t *vb = PTR(ws, 0x7C);
			uint32_t o0 = U16(rec, 4);
			uint32_t o1 = U16(rec, 6);
			uint32_t o2 = U16(rec, 8);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			uint8_t *p1 = vb + o1;
			uint8_t *p0 = vb + o0;
			uint8_t *p2 = vb + o2;
			uint32_t s1 = U32(p1, 0);
			uint32_t s0 = U32(p0, 0);
			uint32_t s2 = U32(p2, 0);
			U32(pkt, 0x14) = s1;
			U32(pkt, 0x10) = s0;
			U32(pkt, 0x18) = s2;
			if (nocull == 0)
			{
				int32_t mac0;
				x::GteWriteData((int32_t)s0, 12);
				x::GteWriteData((int32_t)s1, 13);
				x::GteWriteData((int32_t)s2, 14);
				x::GteNCLIP();
				x::GteReadData(&mac0, 0x18);
				if (mac0 < 0) goto next;
			}
			{
				uint32_t a0 = U32(p0, 4), a1 = U32(p1, 4), a2 = U32(p2, 4);
				if (((a2 | a1 | a0) >> 16) & 0x46) goto next;
				int32_t sum = (int32_t)((a2 + a1 + a0) & 0xFFFF);
				int32_t q = Div3(sum);
				U8(pkt, 3) = 6;
				U32(pkt, 8) = 0;
				x::GteWriteData((int32_t)(U32(ws, 0x8C) | U32(rec, 0) | 0x20000000u), 6);
				x::Gte_4601B0();
				uint32_t mode = U32(ws, 0x50);
				x::GteReadData2(0x16, pkt + 0xC);
				uint32_t key = (uint32_t)q & 0x3FFC;
				U32(pkt, 4) = mode;
				x::InsertPrimDepthKeys(ot + key, pkt, z0, z1, z2, 0);
				pkt += 0x1C;
			}
		next:
			rec += 0xC;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}

	static void __cdecl op_0B6_QueueBlit()
	{
		uint8_t *s = STREAM();
		int32_t off = S16(s, 2);
		uint16_t w8 = U16(s + off, 8);
		uint8_t *p = s + off;
		uint16_t wA = U16(p, 0xA);
		xl_QueueBlitCommand(p, (int32_t)w8, (int32_t)(((uint32_t)s & 0xFFFF0000u) | wA));
		STREAM() += 4;
	}

	static void __cdecl op_0B7_StreamVolumeTranslation()
	{
		if (((uint32_t)U16(RT(), 0x4A) >> 12) - 1 != 0) return;
		uint8_t *s = STREAM();
		int32_t b = S16(s, 4);
		int32_t a = S16(s, 2);
		xl_SdStreamingVolumeTranslation(a, b);
		STREAM() += 6;
	}

	static void BlitRect_B66FE0(const uint8_t *src, int32_t a, int32_t b)
	{
		uint8_t *slot = (uint8_t *)(game_addr(BLITRING) + 8u * (MEM<uint8_t>(game_addr(BLITRING_IDX)) & 0xFu));
		U16(slot, 0) = U16(src, 0);
		U16(slot, 2) = U16(src, 2);
		U16(slot, 4) = U16(src, 4);
		U16(slot, 6) = U16(src, 6);
		xl_QueueBlitCommand(slot, a, b);
		MEM<uint8_t>(game_addr(BLITRING_IDX)) = (uint8_t)(MEM<uint8_t>(game_addr(BLITRING_IDX)) + 1);
	}

	static void __cdecl op_0B8_BlitFrameFromObject()
	{
		uint8_t *obj = blob::ObjectPtr(S16(STREAM(), 2));
		uint8_t *s = STREAM();
		uint8_t *table = obj + U32(obj, 0x10);
		int32_t index = S16(s, 4);
		int32_t divisor = S16(s, 6);
		uint8_t *rec = table + U8(table + index, 0);
		uint8_t *ws = WS();
		int32_t num = S16(CUR(), 0x94);
		int32_t f = num % divisor;
		uint32_t k = U8(rec, 0);
		const uint8_t *tab_x = (const uint8_t *)(C().draw_table - 476);
		const uint8_t *tab_y = (const uint8_t *)(C().draw_table - 464);
		uint32_t bx = U16(tab_x, 2 * (int32_t)k);
		uint32_t by = U16(tab_y, 2 * (int32_t)k);
		U16(ws, 0xA0) = (uint16_t)(U8(rec + 2 * f, 5) + bx);
		U16(WS(), 0xA2) = (uint16_t)(U8(rec + 2 * f + 5, 1) + by);
		U16(WS(), 0xA4) = U8(rec, 3);
		U16(WS(), 0xA6) = U8(rec, 4);
		int32_t b = (int32_t)(U8(rec, 2) + by);
		int32_t a = (int32_t)(U8(rec, 1) + bx);
		BlitRect_B66FE0(WS() + 0xA0, a, b);
		STREAM() += 8;
	}
}

	void apply_eden_d(Clone &c)
	{
		c.draw[57] = part_eden_d::dh_57_MeshRotScaledOrder3;
		c.draw[63] = part_eden_d::dh_63_MorphWireframeLines;
		c.draw[72] = part_eden_d::dh_72_BattleModelPivotAttach;
		c.prim[6] = part_eden_d::pr_06_TintedF3;
		c.vm[0x0B6] = part_eden_d::op_0B6_QueueBlit;
		c.vm[0x0B7] = part_eden_d::op_0B7_StreamVolumeTranslation;
		c.vm[0x0B8] = part_eden_d::op_0B8_BlitFrameFromObject;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace eden
{
	static Clone g_c = {};

	static void describe(Clone &c)
	{
		c.name = "206 Eden";
		c.effect_id = 206;
		c.vm_table = game_addr(game::GF_206Eden_VmOpcodeTable);
		c.bone_table = game_addr(game::GF_206Eden_BoneHandlerTable);
		c.prim_table = game_addr(game::GF_206Eden_PrimRendererTable);
		c.prim_size = game_addr(game::unk_18710E8);
		c.spr_table = game_addr(game::GF_206Eden_SpriteHandlerTable);
		c.ptype_table = game_addr(game::GF_206Eden_ParticleTypeTable);
		c.pop_table = game_addr(game::GF_206Eden_ParticlePopTable);
		c.draw_table = game_addr(game::GF_206Eden_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_1871306);
		c.attr_184 = game_addr(game::unk_1871320);
		c.attr_186 = game_addr(game::unk_1871322);
		c.attr_194 = game_addr(game::unk_1871330);
		c.attr_19C = game_addr(game::unk_1871338);
		c.desc = game_addr(game::unk_1870640);
		c.ptr_block = game_addr(game::unk_1870F50);
		c.queue = game_addr(game::unk_2796CF8);
		c.file00 = game_addr(game::unk_2796D2C);
		c.file01 = game_addr(game::unk_2796D28);
		c.tick = game_addr(game::GF_206Eden_SequenceTick);
		c.load_cb = game_addr(game::MAG_206_sub_AEFAE0);
		c.lit_dispatcher = false;
		c.bone_count = 32;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_gfc_eden()
	{
		using namespace gfc;
		eden::describe(eden::g_c);
		init_clone(eden::g_c, nullptr, 0);
		apply_eden_a(eden::g_c);
		apply_eden_b(eden::g_c);
		apply_eden_c(eden::g_c);
		apply_eden_d(eden::g_c);
		apply_shared_ribbon(eden::g_c);
		apply_shared_misc(eden::g_c);
		register_port(game::GF_206Eden_SequenceTick, (void *)eden::SequenceTask, "GFC206 Eden SequenceTick", 206);
	}
}

