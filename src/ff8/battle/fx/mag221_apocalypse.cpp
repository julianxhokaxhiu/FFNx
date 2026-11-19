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

#include "gfc_engine.h"

namespace ff8fx
{
namespace gfc
{
namespace apocalypse221
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
	void apply_eden_b(Clone &c);

namespace apocalypse221
{
	static void describe(Clone &c)
	{
		c.name = "221 Apocalypse";
		c.effect_id = 221;
		c.vm_table = game_addr(game::funcs_AB2DDC);
		c.bone_table = game_addr(game::funcs_AAF657);
		c.prim_table = game_addr(game::funcs_AA9C74);
		c.prim_size = game_addr(game::unk_186D4DC);
		c.spr_table = game_addr(game::unk_186D570);
		c.ptype_table = game_addr(game::unk_186D59C);
		c.pop_table = game_addr(game::funcs_AAD047);
		c.draw_table = game_addr(game::unk_186D610);
		c.attr_16A = game_addr(game::unk_186D77A);
		c.attr_184 = game_addr(game::unk_186D794);
		c.attr_186 = game_addr(game::unk_186D796);
		c.attr_194 = game_addr(game::unk_186D7A4);
		c.attr_19C = game_addr(game::unk_186D7AC);
		c.desc = game_addr(game::unk_186D234);
		c.ptr_block = game_addr(game::unk_186D37C);
		c.queue = game_addr(game::unk_2796BE0);
		c.file00 = game_addr(game::unk_2796C10);
		c.file01 = game_addr(game::unk_2796C14);
		c.tick = game_addr(game::MAG_221_sub_AA5BD0);
		c.load_cb = game_addr(game::MAG_221_sub_AAEA40);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static inline uint8_t RowV_186D4FC(int32_t i) { return MEM<uint8_t>(game_addr(game::unk_186D4FC) + i); }
	static inline uint32_t PieceColour_186D510(int32_t i) { return MEM<uint32_t>(game_addr(game::unk_186D510) + 4 * i); }

	inline uint32_t &GteData(int reg) { return MEM<uint32_t>(game_addr(game::GTE_VXY0) + 4 * reg); }
	static inline void xl_SetFogNearFar_56CC50(int32_t a, int32_t b, int32_t h) { x::f<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFogNearFar))(a, b, h); }
	static inline int32_t xl_RotTransPersNormalColor_56CAA0(const void *v, const void *n, const void *c, void *sxy, void *rgb, void *flag)
	{
		return x::f<int32_t (__cdecl *)(const void *, const void *, const void *, void *, void *, void *)>(game_addr(game::GTE_RotTransPersNormalColor))(v, n, c, sxy, rgb, flag);
	}
	static inline void xl_NormalColor_56CA30(const void *n, const void *col, void *out) { x::f<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::sub_56CA30))(n, col, out); }
	static inline void xl_RotTrans(const void *v, void *out, void *flag) { x::f<void (__cdecl *)(const void *, void *, void *)>(game_addr(game::GTE_RotTrans))(v, out, flag); }
	static inline void xl_InsertPrimAutoDepth(uint32_t ot, void *prim) { x::f<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAutoDepth))(ot, prim); }
	static inline int32_t div3(int32_t v)
	{
		int32_t hi = (int32_t)(((int64_t)v * 0x55555556LL) >> 32);
		return hi + (int32_t)((uint32_t)hi >> 31);
	}

	static void __cdecl bh_11_PolarXY()
	{
		uint8_t *o = blob::GetBone(S16(CUR(), 0xB0));
		const int32_t ang = S32(CUR(), 0x60) >> 16;
		const int32_t c = x::Cos(ang);
		S16(CUR(), 0x94) = (int16_t)((uint16_t)(mul32(S32(CUR(), 0x5C) >> 8, c >> 4) >> 16) + U16(o, 0x94));
		const int32_t s = x::Sin(ang);
		S16(CUR(), 0x96) = (int16_t)((uint16_t)(mul32(S32(CUR(), 0x5C) >> 8, s >> 4) >> 16) + U16(o, 0x96));
		S16(CUR(), 0x98) = (int16_t)((uint16_t)(S32(CUR(), 0x64) >> 16) + U16(o, 0x98));
	}

	static void EntityTint_AA6770()
	{
		uint8_t *sc = SCENE();
		int32_t s, n;
		if (S8(sc, 0x48) > 2) { s = 0; n = 3; }
		else { s = 3; n = 4; }
		uint8_t *cur = CUR();
		const int32_t v = (int32_t)S16(cur, 0x98) | (int32_t)S16(cur, 0x96) | (int32_t)S16(cur, 0x94);
		if (v & 0x8000)
		{
			for (int32_t k = 0; k < n; k++)
			{
				uint8_t *e = PTR(SCENE(), 0x60 + 4 * (s + k));
				if (e) U16(e, 0) = U16(SCENE(), 0x4E + 2 * (s + k));
			}
			if (v < 0) return;
		}
		if (v == 0)
		{
			for (int32_t k = 0; k < n; k++)
			{
				uint8_t *e = PTR(SCENE(), 0x60 + 4 * (s + k));
				if (e) U8(e, 0) |= 0xC;
			}
			return;
		}
		if (v <= 0) return;
		const uint32_t col = (((uint32_t)U8(CUR(), 0x98) | 0x200) << 16) | ((uint32_t)U8(cur, 0x96) << 8) | U8(CUR(), 0x94);
		for (int32_t k = 0; k < n; k++)
		{
			uint8_t *e = PTR(SCENE(), 0x60 + 4 * (s + k));
			if (!e) continue;
			U32(e, 0x28) = col;
			U16(e, 0) = (uint16_t)(U16(SCENE(), 0x4E + 2 * (s + k)) | 0x800);
		}
	}

	static void __cdecl dh_69_EntityTint()
	{
		EntityTint_AA6770();
	}

	static void GridSheet(bool rowtab)
	{
		const uint8_t *d = PTR(CUR(), 0xB8);
		S32(WS(), 0x60) = S16(d, 4);
		S32(WS(), 0x64) = S16(d, 6);
		const int32_t tex = S16(d, 2);
		blob::TexInfo_B665C0(tex);
		U32(WS(), 0x68) = U32(WS(), 0xFC);
		S32(WS(), 0x6C) = blob::ClutWord_B66640(tex);
		uint8_t *ws = WS();
		{
			const int32_t a = shl32(S32(ws, 0x60) / 2, 5);
			const int32_t e = ((int32_t)U16(CUR(), 0x98) >> 4) & 0x1E0;
			S32(ws, 0x70) = add32(sub32((int32_t)~(uint32_t)a, e), 1);
		}
		ws = WS();
		{
			const int32_t a = shl32(S32(ws, 0x64) / 2, 5);
			if (!rowtab)
			{
				const int32_t e = ((int32_t)U16(CUR(), 0x94) >> 4) & 0x1E0;
				S32(ws, 0x74) = add32(sub32((int32_t)~(uint32_t)a, e), 1);
			}
			else
			{
				const int32_t e = ((int32_t)U16(CUR(), 0x94) >> 4) & 0x3E0;
				S32(ws, 0x74) = add32(sub32((int32_t)~(uint32_t)a, e), 1) >> 5;
			}
		}
		ws = WS();
		S32(ws, 0x78) = mul32(S32(ws, 0x60) + 1, 12);
		uint8_t *ref = blob::GetBone(S16(d, 0));
		if (U8(CUR(), 0xDE) & 0x20) U32(WS(), 0x7C) = U32(CUR(), 0xC0);
		else U32(WS(), 0x7C) = 0xFFFFFFFFu;
		const int16_t angy = S16(CUR(), 0x8E);
		S16(CUR(), 0x8E) = 0x1000;
		h_B27130();
		S16(CUR(), 0x8E) = angy;
		h_B29450((int32_t)U32(CUR(), 0xCC));
		xl_SetFogNearFar_56CC50(S16(d, 8), S16(d, 0xA), 0x200);
		const uint32_t rc = U32(ref, 0xCC);
		x::GteSetBackColor3((int32_t)(rc & 0xFF), (int32_t)((rc >> 8) & 0xFF), (int32_t)((rc >> 16) & 0xFF));
		uint8_t *v = PTR(CTX(), 0x74);
		ws = WS();
		S16(ws, 0xC0) = (int16_t)((U16(CUR(), 0x94) & 0x1FF) - shl32(S32(ws, 0x64) / 2, 9));
		ws = WS();
		const int32_t z0 = (int32_t)(U16(CUR(), 0x98) & 0x1FF) - shl32(S32(ws, 0x60) / 2, 9);
		S16(ws, 0xC2) = 0;
		S16(WS(), 0xC8) = (int16_t)(U16(CUR(), 0x96) << 4);
		S16(WS(), 0xCA) = 0;
		S16(WS(), 0xCC) = 0;
		U32(WS(), 0xE0) = 0x3C808080;
		const uint32_t code = (U32(CUR(), 0xCC) & 0x2000000) | 0x3C000000;
		ws = WS();
		if (S32(ws, 0x64) >= 0)
		{
			for (int32_t r = S32(ws, 0x64) + 1; r > 0; r--)
			{
				S16(ws, 0xC4) = (int16_t)z0;
				ws = WS();
				if (S32(ws, 0x60) >= 0)
				{
					for (int32_t c = S32(ws, 0x60) + 1; c > 0; c--)
					{
						U16(v, 4) = (uint16_t)xl_RotTransPersNormalColor_56CAA0(ws + 0xC0, ws + 0xC8, ws + 0xE0, v, v + 8, ws + 0xFC);
						U32(v, 8) |= code;
						v += 0xC;
						S16(WS(), 0xC4) = (int16_t)(S16(WS(), 0xC4) + 0x200);
						ws = WS();
					}
				}
				S16(ws, 0xC0) = (int16_t)(S16(ws, 0xC0) + 0x200);
				ws = WS();
			}
		}
		uint8_t *row0 = PTR(CTX(), 0x74);
		uint8_t *row1 = row0 + S32(ws, 0x78);
		uint8_t *pk = PTR(CTX(), 0x7C);
		int32_t vv = rowtab ? (S32(ws, 0x74) & 0xF) : S32(ws, 0x74);
		for (int32_t r = S32(ws, 0x64); r > 0; r--)
		{
			int32_t u = S32(ws, 0x70);
			const int32_t w = S32(ws, 0x60);
			if (w > 0)
			{
				const int32_t v2 = vv + 0x20;
				for (int32_t c = w; c > 0; c--)
				{
					U8(pk, 3) = 0xC;
					U32(pk, 4) = U32(row0, 8);
					U32(pk, 0x10) = U32(row0, 0x14);
					U32(pk, 0x1C) = U32(row1, 8);
					U32(pk, 0x28) = U32(row1, 0x14);
					U16(pk, 0x1A) = U16(WS(), 0x68);
					U16(pk, 0xE) = U16(WS(), 0x6C);
					U8(pk, 0x24) = (uint8_t)u;
					U8(pk, 0xC) = (uint8_t)u;
					u += 0x20;
					const uint8_t u2 = (u & 0xF0) ? (uint8_t)u : 0xFF;
					U8(pk, 0x30) = u2;
					U8(pk, 0x18) = u2;
					if (!rowtab)
					{
						U8(pk, 0x19) = (uint8_t)vv;
						U8(pk, 0xD) = (uint8_t)vv;
						const uint8_t vb = (v2 & 0xF0) ? (uint8_t)v2 : 0xFF;
						U8(pk, 0x31) = vb;
						U8(pk, 0x25) = vb;
					}
					else
					{
						const uint8_t va = RowV_186D4FC(vv);
						U8(pk, 0x19) = va;
						U8(pk, 0xD) = va;
						const uint8_t vb = RowV_186D4FC(vv + 1);
						U8(pk, 0x31) = vb;
						U8(pk, 0x25) = vb;
					}
					U32(pk, 8) = U32(row0, 0);
					U32(pk, 0x14) = U32(row0, 0xC);
					U32(pk, 0x20) = U32(row1, 0);
					U32(pk, 0x2C) = U32(row1, 0xC);
					int32_t z = S32(WS(), 0x7C);
					if (z < 0) z = (S16(row1, 0x10) + S16(row1, 4) + S16(row0, 0x10) + S16(row0, 4)) / 4;
					x::InsertPrimAltViewport(((uint32_t)z & 0xFFFFFFFCu) + U32(RT(), 0x4C), pk);
					row0 += 0xC;
					row1 += 0xC;
					pk += 0x34;
				}
				ws = WS();
			}
			vv = rowtab ? ((vv + 1) & 0xF) : vv + 0x20;
			row0 += 0xC;
			row1 += 0xC;
		}
		PTR(CTX(), 0x7C) = pk;
	}
	static void __cdecl dh_51_FoggedGridSheet() { GridSheet(false); }
	static void __cdecl dh_68_FoggedGridSheetRows() { GridSheet(true); }

	static void PieceStep_AAC5A0(uint8_t *p)
	{
		uint8_t *ws = WS();
		if (U8(RT(), 0x45) != 0) return;
		const uint8_t st = U8(p, 0);
		U16(p, 2) = (uint16_t)(U16(p, 2) + 1);
		const int32_t age = S16(p, 2);
		if (st & 2)
		{
			const uint16_t ta = U16(p, 0xA);
			if ((int32_t)(int16_t)ta < S32(ws, 0x80))
			{
				U16(p, 0xE) = (uint16_t)(U16(p, 0xE) + U16(p, 0x10));
				U16(p, 0xC) = (uint16_t)(U16(p, 0xC) + U16(p, 0xE));
				U16(p, 0x14) = (uint16_t)(U16(p, 0x14) + U16(p, 0x16));
				U16(p, 0xA) = (uint16_t)(ta + 1);
			}
		}
		else if (age < S32(ws, 0x7C))
		{
			U16(p, 0xE) = (uint16_t)(U16(p, 0xE) + U16(p, 0x10));
			U16(p, 0xC) = (uint16_t)(U16(p, 0xC) + U16(p, 0xE));
			U16(p, 0x14) = (uint16_t)(U16(p, 0x14) + U16(p, 0x16));
			if (age >= S32(ws, 0x90)) U16(p, 0x20) = (uint16_t)(U16(p, 0x20) + U16(p, 0x22));
		}
		U16(p, 0x12) = (uint16_t)(U16(p, 0x12) + U16(p, 0x14));
		U8(p, 0x18) = (uint8_t)(U8(p, 0x18) + U8(p, 0x1B));
		U8(p, 0x19) = (uint8_t)(U8(p, 0x19) + U8(p, 0x1C));
		U8(p, 0x1A) = (uint8_t)(U8(p, 0x1A) + U8(p, 0x1D));
	}

	static void PieceRelease_AABFF0(uint8_t *blk, int32_t h)
	{
		uint8_t *ws = WS();
		uint8_t *p0 = blk + 0x14;
		uint8_t *ref = PTR(ws, 0x88);
		for (int32_t n = S16(blk, 0xA), k = 0; k < n; k++)
		{
			uint8_t *p = p0 + 0x24 * k;
			if (U16(p, 0) == 0 && S16(p, 0xC) < h) U32(p, 0) = 1;
		}
		if (!(U8(CUR(), 0x4A) & 2)) return;
		const int32_t m = S32(ws, 0x80);
		S32(ws, 0xF0) = shl32(mul32(m + 1, m), 4) / 2;
		const int32_t a = S16(ref, 0x90);
		int32_t lo1 = 0x400 - a;
		int32_t hi1 = a + 0x400;
		const int32_t lo2 = 0xC00 - a;
		const int32_t hi2 = a + 0xC00;
		uint8_t *p = p0;
		for (int32_t n = S16(blk, 0xA); n > 0; n--, p += 0x24)
		{
			const uint16_t sv = U16(p, 0);
			if (sv & 2) continue;
			const int32_t ang = U16(p, 0x12) & 0xFFF;
			if (!((ang >= lo1 && ang <= hi1) || (ang >= lo2 && ang <= hi2))) continue;
			U16(p, 0) = (uint16_t)(sv | 2);
			U16(p, 0xA) = 0;
			const int32_t r = S16(p, 0xC);
			const int32_t t = add32(blob::Rand73(S16(ref, 0x8C)), S16(ref, 0x94));
			lo1 = t;
			hi1 = sub32(t, r);
			U16(p, 0x10) = 0;
			U16(p, 0xE) = (uint16_t)(hi1 / S32(ws, 0x80));
		}
	}

	static void DrawPieces_AAC130(uint8_t *blk, uint8_t *verts)
	{
		uint8_t *ws = WS();
		uint8_t *obj = PTR(blk, 0xC);
		uint8_t *p = blk + 0x14;
		h_B269E0();
		blob::CopyMatrix(WS() + 0x40, WS() + 0xE0);
		PTR(ws, 0x78) = WS() + 0x40;
		uint8_t *o = PTR(CTX(), 0x74);
		const uint8_t *v = verts;
		for (int32_t n = S32(obj, 0x18); n > 0; n--, v += 8, o += 8)
		{
			x::GteWriteData((int32_t)(((uint32_t)U16(v, 2) << 16) | U16(v, 0)), 0);
			x::GteWriteData((int32_t)U16(v, 4), 1);
			x::GteRTPS();
			x::GteReadSXY2(o);
			U16(o, 4) = MEM<uint16_t>(game_addr(game::GTE_SZ3));
		}
		h_B29450((int32_t)U32(CUR(), 0xCC));
		const uint8_t *tri = obj + U32(obj, 8) + 4;
		uint8_t *pk = PTR(CTX(), 0x7C);
		uint8_t *vb = PTR(CTX(), 0x74);
		for (int32_t n = S16(blk, 0xA); n > 0; n--, pk += 0x20, tri += 0x14, p += 0x24)
		{
			U8(pk, 3) = 7;
			U16(pk, 0x16) = (uint16_t)(U16(tri, 0x12) | 0x20);
			U16(pk, 0xE) = U16(tri, 0x10);
			U16(pk, 0xC) = U16(tri, 4);
			U16(pk, 0x14) = U16(tri, 6);
			U16(pk, 0x1C) = U16(tri, 8);
			if (U16(p, 0) == 0)
			{
				const uint8_t *a = vb + U16(tri, 0xA);
				const uint8_t *b = vb + U16(tri, 0xC);
				const uint8_t *c = vb + U16(tri, 0xE);
				U32(pk, 8) = U32(a, 0);
				U32(pk, 0x10) = U32(b, 0);
				U32(pk, 0x18) = U32(c, 0);
				xl_NormalColor_56CA30(WS() + 0xE0, tri, pk + 4);
				U8(pk, 7) = 0x26;
				const int32_t z = (int32_t)U16(c, 4) + (int32_t)U16(b, 4) + (int32_t)U16(a, 4);
				xl_InsertPrimAutoDepth(U32(RT(), 0x4C) + (uint32_t)((div3(z) >> 2) & ~3), pk);
				continue;
			}
			U32(pk, 4) = PieceColour_186D510(S16(p, 2) % 0x18);
			uint8_t *q = p;
			PieceStep_AAC5A0(q);
			uint8_t *m = ws + 0xC0;
			U16(ws, 0xB0) = (uint16_t)((uint32_t)U8(q, 0x18) << 4);
			U16(ws, 0xB2) = (uint16_t)((uint32_t)U8(q, 0x19) << 4);
			U16(ws, 0xB4) = (uint16_t)((uint32_t)U8(q, 0x1A) << 4);
			x::RotMatrixFromAngles(ws + 0xB0, m);
			const uint32_t sc = U16(q, 0x20);
			U32(ws, 0xB8) = sc;
			U32(ws, 0xB4) = sc;
			U32(ws, 0xB0) = sc;
			x::ScaleMatrix(m, ws + 0xB0);
			S32(ws, 0xD4) = mul32(x::Sin(S16(q, 0x12)), S16(q, 0xC)) >> 12;
			S32(ws, 0xD8) = 0;
			S32(ws, 0xDC) = mul32(x::Cos(S16(q, 0x12)), S16(q, 0xC)) >> 12;
			x::ComposeAffine(PTR(ws, 0x78), m, m);
			x::SetRotMatrix(m);
			x::SetTransVector(m);
			const uint8_t *a = verts + U16(tri, 0xA);
			const uint8_t *b = verts + U16(tri, 0xC);
			const uint8_t *c = verts + U16(tri, 0xE);
			const uint16_t cx = U16(q, 4), cy = U16(q, 6), cz = U16(q, 8);
			U16(m, 0) = (uint16_t)(U16(a, 0) - cx);
			U16(ws, 0xC2) = (uint16_t)(U16(a, 2) - cy);
			U16(ws, 0xC4) = (uint16_t)(U16(a, 4) - cz);
			U16(ws, 0xC8) = (uint16_t)(U16(b, 0) - cx);
			U16(ws, 0xCA) = (uint16_t)(U16(b, 2) - cy);
			U16(ws, 0xCC) = (uint16_t)(U16(b, 4) - cz);
			U16(ws, 0xD0) = (uint16_t)(U16(c, 0) - cx);
			U16(ws, 0xD2) = (uint16_t)(U16(c, 2) - cy);
			U16(ws, 0xD4) = (uint16_t)(U16(c, 4) - cz);
			x::GteLoadV012(m, ws + 0xC8, ws + 0xD0);
			x::GteRTPT();
			x::GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			const uint16_t sz = MEM<uint16_t>(game_addr(game::GTE_SZ3));
			U16(m, 0) = sz;
			xl_InsertPrimAutoDepth(U32(RT(), 0x4C) + (((uint32_t)sz >> 2) & ~3u), pk);
		}
		PTR(CTX(), 0x7C) = pk;
	}

	static void __cdecl dh_50_ObjectShatter()
	{
		uint8_t *w = WS();
		if (U32(CUR(), 0xBC) == 0)
		{
			const uint8_t *desc = PTR(CUR(), 0xB8);
			uint8_t *ref = blob::GetBone(S16(desc, 0));
			uint8_t *obj = blob::ObjectPtr(S16(desc, 2));
			const int32_t n = S16(obj + U32(obj, 8), 2);
			PTR(CUR(), 0xBC) = blob::ArenaAlloc(n * 0x24 + 0x14);
			uint8_t *blk = PTR(CUR(), 0xBC);
			U16(blk, 0xA) = (uint16_t)n;
			PTR(blk, 0xC) = obj;
			U16(blk, 6) = U16(desc, 0);
			U16(blk, 0) = U16(desc, 6);
			U16(blk, 2) = 0;
			U16(blk, 4) = U16(desc, 4);
			U16(blk, 0x10) = U16(ref, 0x96);
			U16(blk, 0x12) = U16(ref, 0x8E);
			blob::CopyMatrix(w + 0xC0, BBMAT());
			S32(w, 0xE0) = shl32(S16(CUR(), 0x8C), 4);
			S32(w, 0xE4) = shl32(S16(CUR(), 0x8E), 4);
			S32(w, 0xE8) = shl32(S16(CUR(), 0x90), 4);
			x::ScaleMatrix(w + 0xC0, w + 0xE0);
			x::SetRotMatrix(w + 0xC0);
			const int32_t tx = S16(CUR(), 0x94), ty = S16(CUR(), 0x96), tz = S16(CUR(), 0x98);
			x::GteWriteCtrl(tx, 5);
			x::GteWriteCtrl(ty, 6);
			x::GteWriteCtrl(tz, 7);
			const uint8_t *src = obj + U32(obj, 0x14);
			PTR(CUR(), 0xC0) = blob::ArenaAlloc(n * 8);
			uint8_t *dst = PTR(CUR(), 0xC0);
			for (int32_t k = n; k > 0; k--, src += 8, dst += 8) xl_RotTrans(src, dst, w + 0xFC);
			const int32_t steps = S16(desc, 8);
			S32(w, 0xF0) = shl32(mul32(steps + 1, steps), 4) / 2;
			PTR(w, 0xF4) = PTR(CUR(), 0xC0);
			S32(w, 0xF8) = ((int32_t)S16(ref, 0x96) - 0x1000) / ((int32_t)S16(desc, 8) - (int32_t)S16(ref, 0x8E));
			S32(w, 0x70) = S16(ref, 0x94);
			S32(w, 0x74) = S16(ref, 0x8C);
			const uint8_t *rec = obj + U32(obj, 8) + 4 + 0xC;
			const int32_t ox = S16(blk, 0), oz = S16(blk, 4);
			uint8_t *p = blk + 0x14;
			for (int32_t k = n; k > 0; k--, p += 0x24, rec += 0x14)
			{
				U32(p, 0) = 0;
				U32(p, 0x18) = 0;
				U32(p, 0x1C) = 0;
				const uint8_t *a = PTR(w, 0xF4) + U16(rec, -2);
				const uint8_t *b = PTR(w, 0xF4) + U16(rec, 0);
				const uint8_t *c = PTR(w, 0xF4) + U16(rec, 2);
				const int16_t cx = (int16_t)div3(S16(a, 0) + S16(c, 0) + S16(b, 0));
				U16(p, 4) = (uint16_t)cx;
				const int16_t cy = (int16_t)div3(S16(c, 2) + S16(b, 2) + S16(a, 2));
				U16(p, 6) = (uint16_t)cy;
				const int16_t cz = (int16_t)div3(S16(c, 4) + S16(b, 4) + S16(a, 4));
				U16(p, 0x14) = 0;
				U16(p, 8) = (uint16_t)cz;
				U16(p, 0x16) = (uint16_t)(blob::Rand73(S16(desc, 0x10)) + U16(desc, 0xE));
				U16(p, 0x12) = (uint16_t)blob::Blob_B66B30(ox, oz, cx, cz);
				const int16_t r = S16(w, 0xFC);
				U16(p, 0xC) = (uint16_t)r;
				const int32_t t = add32(blob::Rand73(S32(w, 0x74)), S32(w, 0x70) - r);
				const int16_t v = (int16_t)((t / S16(desc, 8)) * 2);
				U16(p, 0xE) = (uint16_t)v;
				U16(p, 0x10) = (uint16_t)(shl32(sub32(t, mul32(S16(desc, 8), v)), 4) / S32(w, 0xF0));
				const int32_t spin = S16(desc, 0xC);
				U8(p, 0x1B) = (uint8_t)blob::Rand73(spin);
				U8(p, 0x1C) = (uint8_t)blob::Rand73(spin);
				U8(p, 0x1D) = (uint8_t)blob::Rand73(spin);
				U16(p, 0x20) = 0x1000;
				U16(p, 0x22) = U16(w, 0xF8);
			}
		}
		uint8_t *blk = PTR(CUR(), 0xBC);
		const uint8_t *desc = PTR(CUR(), 0xB8);
		uint8_t *ref = blob::GetBone(S16(blk, 6));
		PTR(w, 0x88) = ref;
		S32(w, 0x7C) = S16(desc, 8);
		S32(w, 0x80) = S16(desc, 0xA);
		S32(w, 0x90) = S16(blk, 0x12);
		PieceRelease_AABFF0(blk, S16(ref, 0x98));
		DrawPieces_AAC130(blk, PTR(CUR(), 0xC0));
	}

	static inline int32_t DepthKey(uint32_t off)
	{
		uint32_t v = DEPTHARR()[(int32_t)off / 8] & 0xFFFF;
		return (int32_t)(int64_t)((double)v * (double)FLT_1877DA8());
	}

	static void __cdecl pr_58_TintedFT4Abr()
	{
		h_B29450((int32_t)U32(WS(), 0x84));
		uint8_t *ws = WS();
		uint8_t *pkt = PTR(ws, 0x60);
		uint8_t *rec = PTR(ws, 0x6C);
		int32_t count = S32(ws, 0x70);
		uint32_t ot = U32(ws, 0x5C);
		U32(ws, 0xF0) = 0;
		uint32_t nocull = U32(ws, 0x90) & 0x10;
		do
		{
			uint32_t o0 = U16(rec, 0xC);
			uint32_t o1 = U16(rec, 0xE);
			uint32_t o2 = U16(rec, 0x10);
			uint32_t o3 = U16(rec, 0x12);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			int32_t z3 = DepthKey(o3);
			uint8_t *vb = PTR(ws, 0x7C);
			uint8_t *p0 = vb + o0;
			uint8_t *p1 = vb + o1;
			uint8_t *p2 = vb + o2;
			uint8_t *p3 = vb + o3;
			uint32_t s0 = U32(p0, 0);
			uint32_t s1 = U32(p1, 0);
			uint32_t s2 = U32(p2, 0);
			uint32_t s3 = U32(p3, 0);
			U32(pkt, 8) = s0;
			U32(pkt, 0x18) = s2;
			U32(pkt, 0x20) = s3;
			U32(pkt, 0x10) = s1;
			if (nocull == 0)
			{
				GteData(12) = s0;
				GteData(13) = s1;
				GteData(14) = s2;
				x::GteNCLIP();
				if ((int32_t)GteData(24) < 0) goto next;
			}
			{
				uint32_t a0 = U32(p0, 4), a2 = U32(p2, 4), a1 = U32(p1, 4), a3 = U32(p3, 4);
				if ((a3 | a2 | a1 | a0) & 0x460000) goto next;
				U8(pkt, 3) = 0xA;
				U32(pkt, 0x28) = 0xE1000220;
				uint32_t slot = (((a3 + a2 + a1 + a0) >> 2) & 0x3FFC) + ot;
				U16(pkt, 0x16) = (uint16_t)(U32(ws, 0x50) | U16(rec, 0x16));
				U16(pkt, 0xE) = (uint16_t)(U16(rec, 0x14) + (uint16_t)U32(ws, 0x54));
				uint32_t uvofs = U16(ws, 0x98);
				uint32_t uv0 = U16(rec, 4) + uvofs;
				uint32_t uv1 = U16(rec, 6) + uvofs;
				uint32_t uv2 = U16(rec, 8) + uvofs;
				uint32_t uv3 = U16(rec, 0xA) + uvofs;
				U16(pkt, 0xC) = (uint16_t)uv0;
				U16(pkt, 0x24) = (uint16_t)uv3;
				U16(pkt, 0x14) = (uint16_t)uv1;
				U16(pkt, 0x1C) = (uint16_t)uv2;
				GteData(6) = U32(ws, 0x8C) | U32(rec, 0) | 0x2C000000;
				x::Gte_4601B0();
				x::InsertPrimDepthKeys(slot, pkt, z0, z1, z2, z3);
				U32(pkt, 4) = GteData(22);
				pkt += 0x2C;
			}
		next:
			rec += 0x18;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}

	static VoidFn g_save_vm[0x200], g_save_big[256], g_save_draw[256];

	static void apply_slots(Clone &c)
	{
		ff8fx::copy<(sizeof(g_save_vm))>(g_save_vm, c.vm);
		ff8fx::copy<(sizeof(g_save_big))>(g_save_big, c.bigtab);
		ff8fx::copy<(sizeof(g_save_draw))>(g_save_draw, c.draw);
		apply_shared_misc(c);
		apply_shared_ribbon(c);
		apply_eden_b(c);
		const VoidFn v5c = c.vm[0x05C];
		const VoidFn p16 = c.prim[16];
		const VoidFn d37 = c.draw[37];
		const VoidFn d61 = c.draw[61];
		ff8fx::copy<(sizeof(g_save_vm))>(c.vm, g_save_vm);
		ff8fx::copy<(sizeof(g_save_big))>(c.bigtab, g_save_big);
		ff8fx::copy<(sizeof(g_save_draw))>(c.draw, g_save_draw);
		c.vm[0x05C] = v5c;
		c.prim[16] = p16;
		c.draw[37] = d37;
		c.draw[61] = d61;
		c.bone[11] = bh_11_PolarXY;
		c.prim[58] = pr_58_TintedFT4Abr;
		c.draw[50] = dh_50_ObjectShatter;
		c.draw[51] = dh_51_FoggedGridSheet;
		c.draw[68] = dh_68_FoggedGridSheetRows;
		c.draw[69] = dh_69_EntityTint;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag221_apocalypse()
	{
		using namespace gfc;
		apocalypse221::describe(apocalypse221::g_c);
		init_clone(apocalypse221::g_c, nullptr, 0);
		apocalypse221::apply_slots(apocalypse221::g_c);
		register_port(game::MAG_221_sub_AA5BD0, (void *)apocalypse221::SequenceTask, "MAG221 Apocalypse SequenceTick", 221);
	}
}

