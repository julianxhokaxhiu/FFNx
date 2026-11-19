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

// Cinematic GF engine: melt

	static inline void xl_ModelShadow_B67040(void *hdr, void *root, int32_t ot, int32_t y, int32_t level)
	{
		x::f<int32_t (__cdecl *)(void *, void *, int32_t, int32_t, int32_t)>(game_addr(game::GfCinematic_DrawModelShadow))(hdr, root, ot, y, level);
	}
	static inline int32_t xl_NormalClip(int32_t a, int32_t b, int32_t c) { return x::f<int32_t (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_NormalClip3))(a, b, c); }
	static inline void xl_InsertPrimAutoDepth(uint32_t ot, void *prim) { x::f<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAutoDepth))(ot, prim); }
	static inline int32_t melt_div3(int32_t v)
	{
		int32_t hi = (int32_t)(((int64_t)v * 0x55555556LL) >> 32);
		return hi + (int32_t)((uint32_t)hi >> 31);
	}
	static inline uint8_t *MeltSlotEntity() { return PTR(SCENE(), 0x60 + 4 * (int32_t)U8(CUR(), 0x1B)); }

	static void SyncEntityModelCopy_92E7E0(uint8_t *blk)
	{
		uint8_t *src = PTR(blk, 0xC) + 0x60;
		uint8_t *d = blk + 0x10;
		uint8_t *d2 = blk + 0x30;
		U32(d, 0) = U32(src, 0);
		U32(d, 4) = U32(src, 4);
		U32(d, 8) = U32(src, 8);
		uint8_t *m = PTR(src, 4);
		U32(d2, 0) = U32(m, 0);
		U32(d2, 4) = U32(m, 4);
		U32(d2, 8) = U32(m, 8);
		U32(d2, 0xC) = U32(m, 0xC);
	}

	static uint8_t *CreateEntityModelCopy_92E780(uint8_t *ent)
	{
		uint8_t *blk = blob::ArenaAlloc(0x80);
		PTR(blk, 0xC) = ent;
		SyncEntityModelCopy_92E7E0(blk);
		U32(blk, 0x54) = 0;
		U32(blk, 0x58) = 0xD00140;
		uint32_t t = U32(ent, 0x28);
		U32(blk, 0x78) = t;
		U32(blk, 0x5C) = t;
		t = U32(ent, 0x7C);
		U16(blk, 0x64) = 0;
		U32(blk, 0x60) = t;
		U32(blk, 0x68) = 0;
		U32(blk, 0x50) = 0xFFFFFFFF;
		U16(blk, 0) = 0;
		U16(blk, 2) = 0;
		U8(blk, 0x21) = 0;
		return blk;
	}

	static int32_t CountModelPolygons_92E6E0(uint8_t *mdl)
	{
		S32(WS(), 0xFC) = 0;
		S32(WS(), 0xF8) = 0;
		int32_t nvert = 0;
		uint8_t *tab = PTR(mdl, 4);
		int32_t n = S32(tab, 0);
		tab += 4;
		for (; n > 0; n--)
		{
			uint8_t *o = PTR(mdl, 4) + U32(tab, 0);
			tab += 4;
			int32_t ng = S16(o, 0);
			o += 2;
			for (; ng > 0; ng--)
			{
				int32_t nv = S16(o, 2);
				o += 2;
				nvert += nv;
				o += nv * 6 + 2;
			}
			o = (uint8_t *)(((uint32_t)o + 3) & ~3u);
			S32(WS(), 0xFC) = add32(S32(WS(), 0xFC), S16(o, 0));
			S32(WS(), 0xF8) = add32(S32(WS(), 0xF8), S16(o, 2));
		}
		return nvert;
	}

	static void MeltPolygons_92EB10(uint8_t *s, int32_t ntri, int32_t nquad)
	{
		S32(WS(), 0xF0) = S16(PTR(WS(), 0x80), 4);
		uint8_t *vb = PTR(WS(), 0x64);
		uint8_t *fl = PTR(WS(), 0x8C);
		uint8_t *pk = PTR(CTX(), 0x7C);
		for (; ntri > 0; ntri--, s += 0x10, fl++)
		{
			const int32_t f = S8(fl, 0);
			if (f == 0) continue;
			const uint8_t *a = vb + 8 * (U16(s, 0) & 0xFFF);
			const uint8_t *b = vb + 8 * (U16(s, 2) & 0xFFF);
			const uint8_t *c = vb + 8 * (U16(s, 4) & 0xFFF);
			int32_t idx = f;
			bool fade = true;
			if (f & 0x80)
			{
				if ((U8(c, 6) | U8(b, 6) | U8(a, 6)) == 0) fade = false;
				else idx = S32(WS(), 0xF0);
			}
			if (fade)
			{
				U32(pk, 4) = U32(PTR(WS(), 0x6C), 4 * idx) | 0x26000000;
				U16(pk, 0x16) = U16(s, 0xE) | 0x20;
				U8(fl, 0) = (uint8_t)(idx - 1);
			}
			else
			{
				U32(pk, 4) = 0x24808080;
				U16(pk, 0x16) = U16(s, 0xE);
			}
			U32(pk, 8) = U32(a, 0);
			U32(pk, 0x10) = U32(b, 0);
			U32(pk, 0x18) = U32(c, 0);
			if (xl_NormalClip(S32(a, 0), S32(b, 0), S32(c, 0)) < 0) continue;
			U8(pk, 3) = 7;
			U16(pk, 0xE) = U16(s, 0xA);
			U16(pk, 0xC) = U16(s, 8);
			U16(pk, 0x14) = U16(s, 0xC);
			U16(pk, 0x1C) = U16(s, 6);
			const int32_t z = (int32_t)U16(c, 4) + (int32_t)U16(b, 4) + (int32_t)U16(a, 4);
			xl_InsertPrimAutoDepth(U32(RT(), 0x4C) + (uint32_t)((melt_div3(z) >> 2) & ~3), pk);
			pk += 0x20;
		}
		if (nquad > 0)
		{
			s += 4;
			for (; nquad > 0; nquad--, s += 0x14, fl++)
			{
				const int32_t f = S8(fl, 0);
				if (f == 0) continue;
				const uint8_t *v0 = vb + 8 * (U16(s, -4) & 0xFFF);
				const uint8_t *v1 = vb + 8 * (U16(s, -2) & 0xFFF);
				const uint8_t *v2 = vb + 8 * (U16(s, 0) & 0xFFF);
				const uint8_t *v3 = vb + 8 * (U16(s, 2) & 0xFFF);
				int32_t idx = f;
				bool fade = true;
				if (f & 0x80)
				{
					if ((U8(v3, 6) | U8(v2, 6) | U8(v1, 6) | U8(v0, 6)) == 0) fade = false;
					else idx = S32(WS(), 0xF0);
				}
				if (fade)
				{
					U32(pk, 4) = U32(PTR(WS(), 0x6C), 4 * idx) | 0x2E000000;
					U16(pk, 0x16) = U16(s, 0xA) | 0x20;
					U8(fl, 0) = (uint8_t)(idx - 1);
				}
				else
				{
					U32(pk, 4) = 0x2C808080;
					U16(pk, 0x16) = U16(s, 0xA);
				}
				U32(pk, 8) = U32(v0, 0);
				U32(pk, 0x10) = U32(v1, 0);
				U32(pk, 0x18) = U32(v2, 0);
				if (xl_NormalClip(S32(v0, 0), S32(v1, 0), S32(v2, 0)) < 0) continue;
				U32(pk, 0x20) = U32(v3, 0);
				const int32_t z = (int32_t)U16(v3, 4) + (int32_t)U16(v2, 4) + (int32_t)U16(v1, 4) + (int32_t)U16(v0, 4);
				U8(pk, 3) = 9;
				U16(pk, 0xE) = U16(s, 6);
				U16(pk, 0xC) = U16(s, 4);
				U16(pk, 0x14) = U16(s, 8);
				U16(pk, 0x1C) = U16(s, 0xC);
				U16(pk, 0x24) = U16(s, 0xE);
				xl_InsertPrimAutoDepth(U32(RT(), 0x4C) + (uint32_t)(((z / 4) >> 2) & ~3), pk);
				pk += 0x28;
			}
		}
		PTR(CTX(), 0x7C) = pk;
	}

	static int32_t MeltVertices_92E830(uint8_t *, uint8_t *out)
	{
		uint8_t *blk = PTR(WS(), 0x84);
		uint8_t *hdr = blk + 0x10;
		x::BuildBoneMatricesFromPose(hdr);
		uint8_t *ent = MeltSlotEntity();
		uint8_t *m = WS() + 0xE0;
		blob::CopyMatrix(m, ent + 0x40);
		U32(m, 0x1C) = 0;
		U32(m, 0x14) = 0;
		U32(m, 0x18) = U32(WS(), 0x74);
		x::ComputeBonesWorldMatrices(hdr, WS() + 0xE0);
		PTR(blk, 0x44) = out;
		uint8_t *bones = PTR(blk, 0x30) + 0x10;
		uint8_t *tab = PTR(blk, 0x34);
		int32_t nobj = S32(tab, 0);
		tab += 4;
		for (; nobj > 0; nobj--)
		{
			uint8_t *s = PTR(blk, 0x34) + U32(tab, 0);
			tab += 4;
			int32_t ng = S16(s, 0);
			s += 2;
			uint8_t *o = out;
			for (; ng > 0; ng--)
			{
				uint8_t *mm = bones + S16(s, 0) * 48 + 0x10;
				x::SetRotMatrix(mm);
				x::SetTransVector(mm);
				const int32_t nv = S16(s, 2);
				s += 4;
				S32(WS(), 0xF0) = nv;
				for (int32_t k = nv; k > 0; k--)
				{
					x::GteWriteData((int32_t)(((uint32_t)U16(s, 2) << 16) | U16(s, 0)), 0);
					x::GteWriteData((int32_t)U16(s, 4), 1);
					x::GteMVMVA_RotV0Tr();
					U16(o, 0) = MEM<uint16_t>(game_addr(game::GTE_MAC123));
					U16(o, 2) = (uint16_t)MEM<uint32_t>(game_addr(game::unk_1CA8A78));
					U16(o, 4) = (uint16_t)MEM<uint32_t>(game_addr(game::unk_1CA8A7C));
					s += 6;
					o += 8;
				}
				o += mul32(-S32(WS(), 0xF0), 8);
				x::SetRotMatrix(NODEMAT());
				x::SetTransVector(NODEMAT());
				x::TransformToCamera(ent + 0x1C, WS() + 0xF4, WS() + 0xE0);
				x::SetTransVector(WS() + 0xE0);
				const int32_t lo = S32(WS(), 0x70);
				uint8_t *tb = PTR(WS(), 0x60);
				for (int32_t k = S32(WS(), 0xF0); k > 0; k--)
				{
					int32_t y = S16(o, 2);
					U8(o, 6) = y > S32(WS(), 0x78) ? 1 : 0;
					bool wave;
					if (y > 0)
					{
						U16(o, 2) = 0;
						y = 0;
						wave = true;
					}
					else wave = y > lo;
					if (wave)
					{
						const int32_t amp = S32(tb, 4 * (y - lo)) >> 4;
						const int32_t a = blob::Blob_B66B30(0, 0, S16(o, 0), S16(o, 4));
						U16(o, 0) = (uint16_t)(U16(o, 0) + (uint16_t)(mul32(x::Sin(a) >> 4, amp) >> 16));
						U16(o, 4) = (uint16_t)(U16(o, 4) + (uint16_t)(mul32(x::Cos(a) >> 4, amp) >> 16));
					}
					x::GteWriteData((int32_t)(((uint32_t)U16(o, 2) << 16) | U16(o, 0)), 0);
					x::GteWriteData((int32_t)U16(o, 4), 1);
					x::GteRTPS();
					x::GteReadSXY2(o);
					U16(o, 4) = MEM<uint16_t>(game_addr(game::GTE_SZ3));
					o += 8;
				}
			}
			s = (uint8_t *)(((uint32_t)s + 3) & ~3u);
			const int32_t ntri = S16(s, 0);
			const int32_t nquad = S16(s, 2);
			s += 0xC;
			MeltPolygons_92EB10(s, ntri, nquad);
		}
		return 0;
	}

	static uint8_t *MeltFrame_92E430()
	{
		PTR(WS(), 0x80) = PTR(CUR(), 0xB8);
		PTR(WS(), 0x84) = PTR(CUR(), 0xBC);
		uint8_t *blk = PTR(WS(), 0x84);
		PTR(WS(), 0x88) = PTR(CUR(), 0xC0);
		uint8_t *st = PTR(WS(), 0x88);
		PTR(WS(), 0x8C) = PTR(CUR(), 0xC4);
		uint8_t *wb = blob::GetBone((int32_t)S16(PTR(WS(), 0x80), 2));
		SyncEntityModelCopy_92E7E0(blk);
		PTR(WS(), 0x60) = PTR(CTX(), 0x74);
		uint8_t *tbl = PTR(WS(), 0x60);
		S32(WS(), 0x78) = S16(CUR(), 0x98);
		S32(WS(), 0x74) = S16(CUR(), 0x96);
		S32(WS(), 0x70) = S16(CUR(), 0x94);
		int32_t amp = shl32(S16(wb, 0x8C), 12);
		int32_t vel = shl32(S16(wb, 0x8E), 4);
		int32_t ang = S16(wb, 0x94);
		const int32_t dang = S16(wb, 0x96);
		const int32_t acc = S16(wb, 0x90);
		for (int32_t n = 1 - S16(CUR(), 0x94); n > 0; n--)
		{
			S32(tbl, 0) = mul32(x::Sin(ang) >> 4, amp) >> 8;
			tbl += 4;
			vel = add32(vel, acc);
			ang = add32(ang, dang);
			amp = add32(amp, vel);
		}
		PTR(WS(), 0x6C) = PTR(st, 4);
		PTR(WS(), 0x64) = tbl;
		MeltVertices_92E830(PTR(WS(), 0x60), PTR(WS(), 0x64));
		return blk;
	}

	static void __cdecl dh_52_EntityMelt()
	{
		if (U32(CUR(), 0xBC) == 0)
		{
			uint8_t *desc = PTR(CUR(), 0xB8);
			uint8_t *ent = MeltSlotEntity();
			PTR(CUR(), 0xBC) = CreateEntityModelCopy_92E780(ent);
			uint8_t *blk = PTR(CUR(), 0xBC);
			PTR(CUR(), 0xC0) = blob::ArenaAlloc(0x10);
			uint8_t *st = PTR(CUR(), 0xC0);
			U16(st, 2) = (uint16_t)CountModelPolygons_92E6E0(blk + 0x30);
			const uint16_t np = (uint16_t)(U16(WS(), 0xFC) + U16(WS(), 0xF8));
			U16(st, 0) = np;
			const int32_t n = (int32_t)(((uint32_t)np + 4) & 0xFFC);
			PTR(CUR(), 0xC4) = blob::ArenaAlloc(n);
			if (n > 0) fill_bytes(PTR(CUR(), 0xC4), 0xFF, ((uint32_t)(n + 3) >> 2) * 4);
			PTR(st, 4) = desc + S16(desc, 0);
			U8(ent, 0) |= 4;
		}
		uint8_t *blk = MeltFrame_92E430();
		xl_ModelShadow_B67040(blk + 0x10, MeltSlotEntity() + 0x40, -1, 0, S16(CUR(), 0x8C));
	}

	static void __cdecl dh_03_AnimatedModelWithShadow()
	{
		h_B269E0();
		h_B26AD0(PTR(CUR(), 0xBC));
		uint8_t *blk = PTR(CUR(), 0xBC);
		U32(blk, 0x44) = U32(CTX(), 0x74);
		uint32_t cursor = U32(CTX(), 0x7C);
		uint32_t ot = U32(RT(), 0x4C);
		U32(CTX(), 0x7C) = x::RenderGeometry(blk + 0x30, blk + 0x40, ot, 4, cursor);
		if (!(U8(blk, 2) & 4)) return;
		uint8_t *par = blob::GetBone((int32_t)U16(CUR(), 0x9C));
		blob::RotMatrixOrder(par + 0x8C, 0);
		blob::CopyMatrix(WS() + 0x80, WS() + 0xE0);
		S32(WS(), 0x60) = shl32(S16(CUR(), 0x8C), 4);
		S32(WS(), 0x64) = shl32(S16(CUR(), 0x8E), 4);
		S32(WS(), 0x68) = shl32(S16(CUR(), 0x90), 4);
		x::ScaleMatrix(WS() + 0x80, WS() + 0x60);
		uint8_t *hdr = blk + 0x10;
		S32(WS(), 0x94) = S16(par, 0x94);
		S32(WS(), 0x98) = S16(par, 0x96);
		S32(WS(), 0x9C) = S16(par, 0x98);
		uint8_t *sp = PTR(hdr, 0x6C);
		int32_t y = S16(sp, 2);
		int32_t otspec = S16(sp, 0);
		xl_ModelShadow_B67040(hdr, WS() + 0x80, otspec, y, 0x80);
	}
