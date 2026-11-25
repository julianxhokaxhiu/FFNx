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

// Cinematic GF engine: death claw

	static inline void xl_InsertPrimAutoDepth(uint32_t ot, void *prim) { x::f<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAutoDepth))(ot, prim); }
	static inline void xl_NormalColorCol_56CA30(const void *v, const void *rgbc, void *out) { x::f<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::sub_56CA30))(v, rgbc, out); }
	static inline int32_t xl_RotTransPers_56C880(const void *v, void *sxy, void *p, void *flag) { return x::f<int32_t (__cdecl *)(const void *, void *, void *, void *)>(game_addr(game::TransformWorldCoordinateToProjectedSpace))(v, sxy, p, flag); }
	static inline uint32_t &DcGteData(int reg) { return MEM<uint32_t>(game_addr(game::GTE_VXY0) + 4 * reg); }

	static void __cdecl bh_04_OrbitBetweenBones()
	{
		uint8_t *refs = PTR(CUR(), 0xA0);
		if (!refs) return;
		uint8_t *a = blob::GetBone(S16(refs, 0));
		uint8_t *b = blob::GetBone(S16(refs, 2));
		int32_t ang = blob::Blob_B66B30(S16(a, 0x94), S16(a, 0x98), S16(b, 0x94), S16(b, 0x98));
		uint8_t *bone = CUR();
		int16_t ay = S16(a, 0x96);
		int32_t e = add32(S32(bone, 0x50) >> 16, ang);
		int32_t k = S32(bone, 0x64) >> 16;
		int32_t d = mul32((int32_t)S16(b, 0x96) - (int32_t)ay, k) / 256;
		bone = CUR();
		S16(bone, 0x96) = (int16_t)((S32(bone, 0x60) >> 16) + ay + d);
		int32_t r = mul32(S32(WS(), 0xFC), k) / 256;
		int32_t z = mul32(x::Cos(e), r) >> 12;
		int32_t xx = mul32(x::Sin(e), r) >> 12;
		bone = CUR();
		e = add32(e, S32(bone, 0x54) >> 16);
		int32_t r2 = S32(bone, 0x5C) >> 16;
		z = add32(z, mul32(x::Cos(e), r2) >> 12);
		xx = add32(xx, mul32(x::Sin(e), r2) >> 12);
		S16(CUR(), 0x94) = (int16_t)(S16(a, 0x94) + xx);
		S16(CUR(), 0x98) = (int16_t)(S16(a, 0x98) + z);
	}

	static void __cdecl bh_08_VertexAtHeight()
	{
		uint8_t *ref = blob::GetBone(S16(PTR(CUR(), 0xA0), 0));
		int32_t lo = 0x7FFF;
		S32(WS(), 0x58) = S32(CUR(), 0x64) >> 16;
		uint8_t *m = PTR(ref, 0xD8);
		PTR(WS(), 0x74) = m + U32(m, 0x14);
		S32(WS(), 0x48) = S32(m, 0x18);
		uint8_t *ws = WS();
		const int32_t n = S32(ws, 0x48);
		const uint8_t *tab = PTR(ws, 0x74);
		int32_t hi = 0, ihi = 0, ilo = 0;
		for (int32_t i = 0; i < n; i++)
		{
			const int32_t z = S16(tab, 8 * i + 4);
			if (hi < z) { hi = z; ihi = i; }
			if (lo > z) { lo = z; ilo = i; }
		}
		S32(ws, 0x44) = hi;
		S32(WS(), 0x50) = ihi;
		S32(WS(), 0x54) = ilo;
		ws = WS();
		const int32_t k = S32(ws, 0x58);
		const uint8_t *t = PTR(ws, 0x74);
		int32_t v;
		if (k <= 0) v = S32(ws, 0x54);
		else if (k >= 0x1000) v = S32(ws, 0x50);
		else
		{
			const int32_t target = mul32(S32(ws, 0x44), k) / 4096;
			const int32_t cnt = S32(ws, 0x48);
			int32_t best = 0x7FFF;
			v = 0;
			for (int32_t i = 0; i < cnt; i++)
			{
				int32_t d = sub32(target, S16(t, 8 * i + 4));
				if (d < 0) d = (int32_t)(0u - (uint32_t)d);
				if (d < best) { best = d; v = i; }
			}
		}
		const int32_t vx = S16(t, 8 * v), vy = S16(t, 8 * v + 2), vz = S16(t, 8 * v + 4);
		S16(CUR(), 0x94) = (int16_t)(mul32(S16(ref, 0x8C), vx) / 256);
		S16(CUR(), 0x96) = (int16_t)(mul32(S16(ref, 0x8E), vy) / 256);
		S16(CUR(), 0x98) = (int16_t)(mul32(S16(ref, 0x90), vz) / 256);
	}

	static inline int32_t DcDepthKey(uint32_t off)
	{
		uint32_t v = DEPTHARR()[off >> 3] & 0xFFFF;
		return (int32_t)(int64_t)((double)v * (double)FLT_1877DA8());
	}

	static uint32_t Prim48Residue()
	{
		uint8_t *ws = WS();
		uint8_t *bone = CUR();
		uint8_t *mesh = PTR(ws, 0x68);
		const uint8_t *b = mesh + U32(mesh, 8);
		const uint8_t *cur = PTR(ws, 0x6C) - 4;
		if (b == cur)
		{
			if (U8(bone, 0xE1)) return S32(mesh, 0x20) >= 2 ? 0x16u : 1u;
			if (S32(ws, 0x40) | S32(ws, 0x44) | S32(ws, 0x48)) return 0;
			if (U16(bone, 0x4C) & 2) return 0;
			return 0xE;
		}
		const uint8_t *prev = nullptr;
		for (int n = 0; b != cur && S16(b, 2) != -1 && n < 4096; n++)
		{
			prev = b;
			b = b + 4 + (uint32_t)MEM<uint8_t>(C().prim_size + (int32_t)S16(b, 0)) * (uint32_t)S16(b, 2);
		}
		if (prev == nullptr || b != cur) return 0;
		int32_t v = S16(prev, 0);
		if (U16(bone, 0x4C) & 2) v += 0x14;
		if (U16(bone, 0x4C) & 4) v += 0x28;
		return v == 48 ? (U32(ws, 0x90) & 0x10) : 0u;
	}

	static void __cdecl pr_48_TintedFT3Abr()
	{
		h_B29450((int32_t)U32(WS(), 0x84));
		const uint32_t f0 = Prim48Residue();
		uint8_t *ws = WS();
		const uint32_t ot = U32(ws, 0x5C);
		int32_t count = S32(ws, 0x70);
		uint8_t *pkt = PTR(ws, 0x60);
		uint8_t *rec = PTR(ws, 0x6C);
		const uint32_t nocull = U32(ws, 0x90) & 0x10;
		U32(ws, 0xF0) = f0;
		do
		{
			const uint32_t o0 = U16(rec, 0xA), o1 = U16(rec, 0xC), o2 = U16(rec, 0xE);
			const int32_t z0 = DcDepthKey(o0);
			const int32_t z1 = DcDepthKey(o1);
			const int32_t z2 = DcDepthKey(o2);
			uint8_t *vb = PTR(ws, 0x7C);
			uint8_t *p0 = vb + o0, *p1 = vb + o1, *p2 = vb + o2;
			const uint32_t s0 = U32(p0, 0), s2 = U32(p2, 0), s1 = U32(p1, 0);
			U32(pkt, 8) = s0;
			U32(pkt, 0x10) = s1;
			U32(pkt, 0x18) = s2;
			if (nocull == 0)
			{
				DcGteData(12) = s0;
				DcGteData(13) = s1;
				DcGteData(14) = s2;
				x::GteNCLIP();
				if ((int32_t)DcGteData(24) < 0) goto next;
			}
			{
				const uint32_t a0 = U32(p0, 4), a1 = U32(p1, 4), a2 = U32(p2, 4);
				if ((a2 | a1 | a0) & 0x460000) goto next;
				const int32_t sum = (int32_t)((a2 + a1 + a0) & 0xFFFF);
				const int32_t hi = (int32_t)(((int64_t)sum * 0x55555556LL) >> 32);
				U8(pkt, 3) = 8;
				U32(pkt, 0x20) = 0xE1000220;
				U16(pkt, 0x16) = (uint16_t)(U16(ws, 0x50) | U16(rec, 0x12));
				U16(pkt, 0xE) = (uint16_t)(U16(rec, 0x10) + U16(ws, 0x54));
				const uint32_t uvofs = U16(ws, 0x98);
				U16(pkt, 0xC) = (uint16_t)(U16(rec, 4) + uvofs);
				U16(pkt, 0x14) = (uint16_t)(U16(rec, 6) + uvofs);
				U16(pkt, 0x1C) = (uint16_t)(U16(rec, 8) + uvofs);
				DcGteData(6) = U32(ws, 0x8C) | U32(rec, 0) | 0x24000000;
				x::Gte_4601B0();
				U32(pkt, 4) = DcGteData(22);
				x::InsertPrimDepthKeys(((uint32_t)sub32(hi, sum >> 31) & 0x3FFC) + ot, pkt, z0, z1, z2, 0);
				pkt += 0x24;
			}
		next:
			rec += 0x14;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}

	static void DrawHeightCutQuads_A37590()
	{
		h_B29450((int32_t)U32(CUR(), 0xCC));
		uint8_t *ws = WS();
		uint8_t *bone = CUR();
		const uint8_t *fl = PTR(ws, 0x40);
		uint8_t *vb = PTR(ws, 0x7C);
		const uint8_t code = (uint8_t)((U32(bone, 0xCC) | 0x38000000) >> 24);
		uint8_t *pkt = PTR(ws, 0x60);
		const int32_t abr = U8(bone, 0x92) & 0x60;
		uint8_t *rec = PTR(ws, 0x6C);
		int32_t cnt = S32(ws, 0x70);
		if (cnt > 0)
		{
			do
			{
				const int32_t f = (int8_t)fl[U16(rec, 0x10) >> 3] | (int8_t)fl[U16(rec, 0x12) >> 3] | (int8_t)fl[U16(rec, 0x14) >> 3] | (int8_t)fl[U16(rec, 0x16) >> 3];
				if (f == 0)
				{
					U8(pkt, 3) = 8;
					const uint8_t *p = vb + U16(rec, 0x10);
					U32(pkt, 8) = U32(p, 0);
					int32_t sum = S16(p, 4);
					p = vb + U16(rec, 0x12);
					U32(pkt, 0x10) = U32(p, 0);
					sum += S16(p, 4);
					p = vb + U16(rec, 0x14);
					U32(pkt, 0x18) = U32(p, 0);
					sum += S16(p, 4);
					p = vb + U16(rec, 0x16);
					U32(pkt, 0x20) = U32(p, 0);
					sum += S16(p, 4);
					xl_NormalColorCol_56CA30(WS() + 0xE0, rec, pkt + 4);
					xl_NormalColorCol_56CA30(WS() + 0xE0, rec + 4, pkt + 0xC);
					xl_NormalColorCol_56CA30(WS() + 0xE0, rec + 8, pkt + 0x14);
					xl_NormalColorCol_56CA30(WS() + 0xE0, rec + 0xC, pkt + 0x1C);
					U8(pkt, 7) = code;
					const uint32_t slot = ((uint32_t)(sum / 4) & 0xFFFFFFFCu) + U32(RT(), 0x4C);
					xl_InsertPrimAutoDepth(slot, pkt);
					pkt += 0x24;
					x::SetDrawMode(pkt, 0, 0, abr, nullptr);
					xl_InsertPrimAutoDepth(slot, pkt);
					pkt += 0xC;
				}
				rec += 0x18;
			} while (--cnt != 0);
		}
		PTR(WS(), 0x60) = pkt;
	}

	static void __cdecl dh_31_MeshHeightCut()
	{
		if (U32(CUR(), 0xBC) == 0)
		{
			U32(CUR(), 0xBC) = 0xFFFFFFFFu;
			PTR(CUR(), 0xD8) = blob::ObjectPtr(S16(PTR(CUR(), 0xB8), 2));
			U32(CUR(), 0xCC) &= 0x2FFFFFFu;
		}
		PTR(WS(), 0x60) = PTR(CTX(), 0x7C);
		PTR(WS(), 0x68) = PTR(CUR(), 0xD8);
		uint8_t *m = PTR(WS(), 0x68);
		PTR(WS(), 0x6C) = m + U32(m, 8);
		PTR(WS(), 0x74) = m + U32(m, 0x14);
		PTR(WS(), 0x78) = m + U32(m, 0x1C);
		PTR(WS(), 0x7C) = PTR(CTX(), 0x74);
		U32(WS(), 0x48) = U32(m, 0x18);
		PTR(WS(), 0x80) = PTR(WS(), 0x7C) + 8 * U32(m, 0x18);
		PTR(WS(), 0x40) = PTR(WS(), 0x80) + 8 * U32(m, 0x20);
		uint8_t *ws = WS();
		int32_t hi = 0;
		{
			const uint8_t *v = PTR(ws, 0x74);
			for (int32_t n = S32(ws, 0x48); n > 0; n--, v += 8)
				if (hi < S16(v, 4)) hi = S16(v, 4);
		}
		S32(ws, 0x44) = hi;
		const int32_t thr = mul32(S16(CUR(), 0x98), hi) / 4096;
		ws = WS();
		{
			const uint8_t *v = PTR(ws, 0x74);
			uint8_t *f = PTR(ws, 0x40);
			for (int32_t n = S32(ws, 0x48); n > 0; n--, v += 8, f++)
				*f = (uint8_t)((thr < S16(v, 4) ? 1 : 0) - 1);
		}
		h_B27130();
		ws = WS();
		{
			int32_t n = S32(ws, 0x48);
			const uint8_t *v = PTR(ws, 0x74);
			const uint8_t *f = PTR(ws, 0x40);
			uint8_t *o = PTR(ws, 0x7C);
			for (; n > 0; n--, v += 8, f++, o += 8)
			{
				if (*f != 0) continue;
				U16(o, 4) = (uint16_t)xl_RotTransPers_56C880(v, o, ws + 0xFC, ws + 0xFC);
				ws = WS();
			}
		}
		while (S16(PTR(ws, 0x6C), 2) != -1)
		{
			const uint8_t *b = PTR(ws, 0x6C);
			S32(ws, 0x70) = S16(b, 2);
			PTR(WS(), 0x6C) = PTR(WS(), 0x6C) + 4;
			const uint32_t size = MEM<uint8_t>(C().prim_size + (int32_t)S16(b, 0));
			ws = WS();
			PTR(ws, 0x58) = PTR(ws, 0x6C) + mul32((int32_t)size, S32(ws, 0x70));
			DrawHeightCutQuads_A37590();
			ws = WS();
			PTR(ws, 0x6C) = PTR(ws, 0x58);
			ws = WS();
		}
		PTR(CTX(), 0x7C) = PTR(ws, 0x60);
	}
