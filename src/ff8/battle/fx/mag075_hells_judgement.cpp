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

// Effect 75: Hell's Judgement

#include "gfc_engine.h"

namespace ff8fx
{
namespace gfc
{
	void apply_eden_b(Clone &c);

namespace hells_judgement
{
	static Clone g_hj = {};

	static void describe(Clone &c)
	{
		c.name = "075 Hell's Judgement";
		c.effect_id = 75;
		c.vm_table = game_addr(game::GF_075HellsJudgement_VmOpcodeTable);
		c.bone_table = game_addr(game::GF_075HellsJudgement_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_B41A94);
		c.prim_size = game_addr(game::unk_18760E0);
		c.spr_table = game_addr(game::unk_18760F4);
		c.ptype_table = game_addr(game::unk_1876120);
		c.pop_table = game_addr(game::funcs_B445B7);
		c.draw_table = game_addr(game::GF_075HellsJudgement_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_18762FE);
		c.attr_184 = game_addr(game::unk_1876318);
		c.attr_186 = game_addr(game::unk_187631A);
		c.attr_194 = game_addr(game::unk_1876328);
		c.attr_19C = game_addr(game::unk_1876330);
		c.desc = game_addr(game::GF_075HellsJudgement_ResourceDesc);
		c.ptr_block = game_addr(game::GF_075HellsJudgement_PtrBlock);
		c.queue = game_addr(game::GF_075HellsJudgement_Queue);
		c.file00 = game_addr(game::GF_075HellsJudgement_File00);
		c.file01 = game_addr(game::GF_075HellsJudgement_File01);
		c.tick = game_addr(game::GF_075HellsJudgement_SequenceTick);
		c.load_cb = game_addr(game::GF_075HellsJudgement_LoadCallback);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static inline int32_t xl_RotTransPers3(const void *v0, const void *v1, const void *v2, void *s0, void *s1, void *s2, void *p, void *flag)
	{
		return x::f<int32_t (__cdecl *)(const void *, const void *, const void *, void *, void *, void *, void *, void *)>(game_addr(game::GTE_RotTransPers3))(v0, v1, v2, s0, s1, s2, p, flag);
	}
	static inline int32_t xl_RotTransPers4(const void *v0, const void *v1, const void *v2, const void *v3, void *s0, void *s1, void *s2, void *s3, void *p, void *flag)
	{
		return x::f<int32_t (__cdecl *)(const void *, const void *, const void *, const void *, void *, void *, void *, void *, void *, void *)>(game_addr(game::sub_56C970))(v0, v1, v2, v3, s0, s1, s2, s3, p, flag);
	}
	static inline int32_t xl_Sqrt(int32_t v) { return x::f<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	static inline void xl_InsertPrim(uint32_t ot, void *prim) { x::f<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAutoDepth))(ot, prim); }

	static inline int32_t div3(int32_t v)
	{
		int32_t hi = (int32_t)(((int64_t)v * 0x55555556LL) >> 32);
		return hi + (int32_t)((uint32_t)hi >> 31);
	}

	static int32_t ShatterInit(uint8_t *p, uint8_t *mesh, uint8_t *desc)
	{
		uint8_t *ws = WS();
		int32_t count = 0;
		uint8_t *lst = mesh + U32(mesh, 8);
		uint8_t *verts = mesh + U32(mesh, 0x14);
		S32(ws, 0xF0) = S16(desc, 8);
		S32(WS(), 0xF4) = S16(desc, 0xA);
		S32(WS(), 0xF8) = S16(desc, 0xC);
		int32_t type = S16(lst, 0);
		int32_t n = S16(lst, 2);
		if (type == 0xFFFF) return count;
		for (;;)
		{
			if (type == 8)
			{
				lst += 4;
				count = add32(count, n);
				for (int32_t k = n; k > 0; k--)
				{
					const uint8_t *a = verts + U16(lst, 0xA);
					int32_t sx = S16(a, 0), sy = S16(a, 2), sz = S16(a, 4);
					const uint8_t *b = verts + U16(lst, 0xC);
					sx += S16(b, 0);
					sy += S16(b, 2);
					sz += S16(b, 4);
					const uint8_t *c = verts + U16(lst, 0xE);
					int32_t cx = div3(S16(c, 0) + sx);
					int32_t cy = div3(S16(c, 2) + sy);
					int32_t cz = div3(S16(c, 4) + sz);
					U16(p, 0x24) = (uint16_t)cx;
					S32(p, 0) = shl32(cx, 8);
					U16(p, 0x26) = (uint16_t)cy;
					U16(p, 0x28) = (uint16_t)cz;
					S32(p, 4) = shl32(cy, 8);
					S32(p, 8) = shl32(cz, 8);
					uint8_t *w = WS();
					int32_t dx = sub32(cx, S32(w, 0xF0));
					int32_t dy = sub32(cy, S32(w, 0xF4));
					int32_t dz = sub32(cz, S32(w, 0xF8));
					int32_t r = xl_Sqrt(add32(mul32(dy, dy), mul32(dx, dx)));
					r = xl_Sqrt(add32(mul32(dz, dz), mul32(r, r)));
					U16(p, 0x2A) = (uint16_t)r;
					lst += 0x14;
					p += 0x30;
				}
			}
			else if (type == 0x12)
			{
				lst += 4;
				count = add32(count, n);
				for (int32_t k = n; k > 0; k--)
				{
					const uint8_t *a = verts + U16(lst, 0xC);
					int32_t sx = S16(a, 0), sy = S16(a, 2), sz = S16(a, 4);
					const uint8_t *b = verts + U16(lst, 0xE);
					sx += S16(b, 0);
					sy += S16(b, 2);
					sz += S16(b, 4);
					const uint8_t *c = verts + U16(lst, 0x10);
					sx += S16(c, 0);
					sy += S16(c, 2);
					sz += S16(c, 4);
					const uint8_t *d = verts + U16(lst, 0x12);
					int32_t cx = (S16(d, 0) + sx) / 4;
					int32_t cy = (S16(d, 2) + sy) / 4;
					int32_t cz = (S16(d, 4) + sz) / 4;
					U16(p, 0x24) = (uint16_t)cx;
					S32(p, 0) = shl32(cx, 8);
					U16(p, 0x26) = (uint16_t)cy;
					U16(p, 0x28) = (uint16_t)cz;
					S32(p, 4) = shl32(cy, 8);
					S32(p, 8) = shl32(cz, 8);
					uint8_t *w = WS();
					int32_t dx = sub32(cx, S32(w, 0xF0));
					int32_t dy = sub32(cy, S32(w, 0xF4));
					int32_t dz = sub32(cz, S32(w, 0xF8));
					int32_t r = xl_Sqrt(add32(mul32(dy, dy), mul32(dx, dx)));
					r = xl_Sqrt(add32(mul32(dz, dz), mul32(r, r)));
					U16(p, 0x2A) = (uint16_t)r;
					U16(p, 0x1C) = 0;
					U16(p, 0x1A) = 0;
					U16(p, 0x18) = 0;
					lst += 0x18;
					p += 0x30;
				}
			}
			else
				return count;
			type = S16(lst, 0);
			n = S16(lst, 2);
			if (type == 0xFFFF) return count;
		}
	}

	static void ShatterXformPos(int32_t px, int32_t py, int32_t pz)
	{
		uint8_t *w = WS();
		S16(w, 0xC0) = (int16_t)(px >> 8);
		S16(WS(), 0xC2) = (int16_t)(py >> 8);
		S16(WS(), 0xC4) = (int16_t)(pz >> 8);
		x::GteLoadV0(WS() + 0xC0);
		x::GteMVMVA_RotV0Tr();
		x::Gte_45E580();
	}

	static void ShatterXformRot(int16_t ax, int16_t ay, int16_t az)
	{
		S16(WS(), 0xC0) = ax;
		S16(WS(), 0xC2) = ay;
		S16(WS(), 0xC4) = az;
		x::RotMatrixFromAngles(WS() + 0xC0, WS() + 0xD0);
		x::MulMatrixInPlace(WS() + 0xD0, PTR(WS(), 0x60));
		pad_after_rot_mul(WS() + 0xD0, WS() + 0xC0);
		S32(WS(), 0xC0) = shl32(S16(CUR(), 0x8C), 4);
		S32(WS(), 0xC4) = shl32(S16(CUR(), 0x8E), 4);
		S32(WS(), 0xC8) = shl32(S16(CUR(), 0x90), 4);
		x::ScaleMatrix(WS() + 0xD0, WS() + 0xC0);
		x::SetRotMatrix(WS() + 0xD0);
	}

	static void ShatterStep(uint8_t *p)
	{
		x::SetRotMatrix(PTR(WS(), 0x60));
		x::SetTransVector(PTR(WS(), 0x60));
		U16(p, 0xC) = (uint16_t)(U16(p, 0xC) + U16(p, 0xE));
		U16(p, 0x10) = (uint16_t)(U16(p, 0x10) + U16(p, 0x12));
		int32_t px = add32(S32(p, 0), shl32(S16(p, 0xC), 4));
		int32_t py = add32(S32(p, 4), shl32(S16(p, 0x10), 4));
		U16(p, 0x14) = (uint16_t)(U16(p, 0x14) + U16(p, 0x16));
		S32(p, 0) = px;
		int32_t pz = add32(S32(p, 8), shl32(S16(p, 0x14), 4));
		S32(p, 4) = py;
		S32(p, 8) = pz;
		ShatterXformPos(px, py, pz);
		U16(p, 0x18) = (uint16_t)(U16(p, 0x18) + U16(p, 0x1E));
		U16(p, 0x1A) = (uint16_t)(U16(p, 0x1A) + U16(p, 0x20));
		U16(p, 0x1C) = (uint16_t)(U16(p, 0x1C) + U16(p, 0x22));
		ShatterXformRot((int16_t)(U16(p, 0x18) << 4), (int16_t)(S16(p, 0x1A) << 4), (int16_t)(S16(p, 0x1C) << 4));
	}

	static inline uint32_t BucketG3(int32_t v) { return (uint32_t)((div3(v) >> 2) & ~3) + U32(RT(), 0x4C); }
	static inline uint32_t BucketG4(int32_t v) { return (uint32_t)(((v / 4) >> 2) & ~3) + U32(RT(), 0x4C); }

	static uint8_t *ShatterTris(uint8_t *p, uint8_t *verts, uint8_t *mesh)
	{
		uint8_t *lst = PTR(WS(), 0xF0);
		uint8_t *pk = PTR(CTX(), 0x7C);
		S32(WS(), 0xF0) = S16(lst, 2);
		lst += 4;
		U32(WS(), 0xF4) = (U32(CUR(), 0xCC) & 0x2000000) | 0x24000000;
		U32(WS(), 0xF8) = U8(CUR(), 0x92) & 0x60;
		PTR(WS(), 0xEC) = mesh + U32(mesh, 0x14);
		uint8_t *start = pk;
		if (S32(WS(), 0xF0) > 0)
		{
			pk += 0x10;
			do
			{
				int8_t s = S8(p, 0x2C);
				if (s != 0)
				{
					int32_t z;
					if (s > 0)
					{
						ShatterStep(p);
						uint8_t *w = WS();
						const uint8_t *v = PTR(w, 0xEC) + U16(lst, 0xA);
						U16(w, 0xC0) = (uint16_t)(U16(v, 0) - U16(p, 0x24));
						U16(WS(), 0xC2) = (uint16_t)(U16(v, 2) - U16(p, 0x26));
						U16(WS(), 0xC4) = (uint16_t)(U16(v, 4) - U16(p, 0x28));
						v = PTR(WS(), 0xEC) + U16(lst, 0xC);
						U16(WS(), 0xC8) = (uint16_t)(U16(v, 0) - U16(p, 0x24));
						U16(WS(), 0xCA) = (uint16_t)(U16(v, 2) - U16(p, 0x26));
						U16(WS(), 0xCC) = (uint16_t)(U16(v, 4) - U16(p, 0x28));
						v = PTR(WS(), 0xEC) + U16(lst, 0xE);
						U16(WS(), 0xD0) = (uint16_t)(U16(v, 0) - U16(p, 0x24));
						U16(WS(), 0xD2) = (uint16_t)(U16(v, 2) - U16(p, 0x26));
						U16(WS(), 0xD4) = (uint16_t)(U16(v, 4) - U16(p, 0x28));
						w = WS();
						int32_t otz = xl_RotTransPers3(w + 0xC0, w + 0xC8, w + 0xD0, pk - 8, pk, pk + 8, w + 0xFC, w + 0xFC);
						z = shl32(mul32(otz, 3), 2);
						U32(pk, -0xC) = U32(PTR(WS(), 0x64), (int32_t)S8(p, 0x2C) * 4) | U32(WS(), 0xF4);
						U8(p, 0x2C) = (uint8_t)(U8(p, 0x2C) - 1);
					}
					else
					{
						U32(pk, -8) = U32(verts, U16(lst, 0xA));
						U32(pk, 0) = U32(verts, U16(lst, 0xC));
						U32(pk, 8) = U32(verts, U16(lst, 0xE));
						z = (int32_t)U16(verts, U16(lst, 0xC) + 4) + U16(verts, U16(lst, 0xA) + 4);
						z += U16(verts, U16(lst, 0xE) + 4);
						U32(pk, -0xC) = U32(WS(), 0xF4) | U32(lst, 0);
					}
					U8(pk, -0xD) = 7;
					U16(pk, 6) = (uint16_t)(U16(WS(), 0xF8) | U16(lst, 0x12));
					U16(pk, -2) = U16(lst, 0x10);
					U16(pk, -4) = U16(lst, 4);
					U16(pk, 4) = U16(lst, 6);
					U16(pk, 0xC) = U16(lst, 8);
					xl_InsertPrim(BucketG3(z), start);
					start += 0x20;
					pk += 0x20;
				}
				S32(WS(), 0xF0) = S32(WS(), 0xF0) - 1;
				lst += 0x14;
				p += 0x30;
			} while (S32(WS(), 0xF0) > 0);
		}
		PTR(CTX(), 0x7C) = start;
		PTR(WS(), 0xF0) = lst;
		return p;
	}

	static uint8_t *ShatterQuads(uint8_t *p, uint8_t *verts, uint8_t *mesh)
	{
		uint8_t *lst = PTR(WS(), 0xF0);
		uint8_t *pk = PTR(CTX(), 0x7C);
		S32(WS(), 0xF0) = S16(lst, 2);
		lst += 4;
		U32(WS(), 0xF4) = (U32(CUR(), 0xCC) & 0x2000000) | 0x2C000000;
		U32(WS(), 0xF8) = U8(CUR(), 0x92) & 0x60;
		PTR(WS(), 0xEC) = mesh + U32(mesh, 0x14);
		uint8_t *start = pk;
		if (S32(WS(), 0xF0) > 0)
		{
			pk += 0x10;
			do
			{
				int8_t s = S8(p, 0x2C);
				if (s != 0)
				{
					int32_t z;
					if (s > 0)
					{
						ShatterStep(p);
						uint8_t *w = WS();
						const uint8_t *v = PTR(w, 0xEC) + U16(lst, 0xC);
						U16(w, 0xC0) = (uint16_t)(U16(v, 0) - U16(p, 0x24));
						U16(WS(), 0xC2) = (uint16_t)(U16(v, 2) - U16(p, 0x26));
						U16(WS(), 0xC4) = (uint16_t)(U16(v, 4) - U16(p, 0x28));
						v = PTR(WS(), 0xEC) + U16(lst, 0xE);
						U16(WS(), 0xC8) = (uint16_t)(U16(v, 0) - U16(p, 0x24));
						U16(WS(), 0xCA) = (uint16_t)(U16(v, 2) - U16(p, 0x26));
						U16(WS(), 0xCC) = (uint16_t)(U16(v, 4) - U16(p, 0x28));
						v = PTR(WS(), 0xEC) + U16(lst, 0x10);
						U16(WS(), 0xD0) = (uint16_t)(U16(v, 0) - U16(p, 0x24));
						U16(WS(), 0xD2) = (uint16_t)(U16(v, 2) - U16(p, 0x26));
						U16(WS(), 0xD4) = (uint16_t)(U16(v, 4) - U16(p, 0x28));
						v = PTR(WS(), 0xEC) + U16(lst, 0x12);
						U16(WS(), 0xD8) = (uint16_t)(U16(v, 0) - U16(p, 0x24));
						U16(WS(), 0xDA) = (uint16_t)(U16(v, 2) - U16(p, 0x26));
						U16(WS(), 0xDC) = (uint16_t)(U16(v, 4) - U16(p, 0x28));
						w = WS();
						int32_t otz = xl_RotTransPers4(w + 0xC0, w + 0xC8, w + 0xD0, w + 0xD8, pk - 8, pk, pk + 8, pk + 0x10, w + 0xFC, w + 0xFC);
						z = shl32(otz, 4);
						U32(pk, -0xC) = U32(PTR(WS(), 0x64), (int32_t)S8(p, 0x2C) * 4) | U32(WS(), 0xF4);
						U8(p, 0x2C) = (uint8_t)(U8(p, 0x2C) - 1);
					}
					else
					{
						U32(pk, -8) = U32(verts, U16(lst, 0xC));
						U32(pk, 0) = U32(verts, U16(lst, 0xE));
						U32(pk, 8) = U32(verts, U16(lst, 0x10));
						U32(pk, 0x10) = U32(verts, U16(lst, 0x12));
						z = (int32_t)U16(verts, U16(lst, 0xE) + 4) + U16(verts, U16(lst, 0xC) + 4);
						z += U16(verts, U16(lst, 0x10) + 4);
						z += U16(verts, U16(lst, 0x12) + 4);
						U32(pk, -0xC) = U32(WS(), 0xF4) | U32(lst, 0);
					}
					U8(pk, -0xD) = 9;
					U16(pk, 6) = (uint16_t)(U16(WS(), 0xF8) | U16(lst, 0x16));
					U16(pk, -2) = U16(lst, 0x14);
					U16(pk, -4) = U16(lst, 4);
					U16(pk, 4) = U16(lst, 6);
					U16(pk, 0xC) = U16(lst, 8);
					U16(pk, 0x14) = U16(lst, 0xA);
					xl_InsertPrim(BucketG4(z), start);
					start += 0x28;
					pk += 0x28;
				}
				S32(WS(), 0xF0) = S32(WS(), 0xF0) - 1;
				lst += 0x18;
				p += 0x30;
			} while (S32(WS(), 0xF0) > 0);
		}
		PTR(CTX(), 0x7C) = start;
		PTR(WS(), 0xF0) = lst;
		return p;
	}

	static void __cdecl dh_66_ShatterMesh()
	{
		uint8_t *bone = CUR();
		if (PTR(bone, 0xBC) == nullptr)
		{
			uint8_t *desc = PTR(bone, 0xB8);
			uint8_t *blk = blob::ArenaAlloc(0x10);
			PTR(CUR(), 0xBC) = blk;
			blk = PTR(CUR(), 0xBC);
			uint8_t *mesh = blob::ObjectPtr(S16(desc, 4));
			PTR(blk, 8) = mesh;
			PTR(blk, 0) = desc + S16(desc, 0);
			S32(WS(), 0xF0) = S16(desc, 8);
			S32(WS(), 0xF4) = S16(desc, 0xA);
			S32(WS(), 0xF8) = S16(desc, 0xC);
			S32(WS(), 0xFC) = 0;
			uint8_t *top = PTR(CTX(), 0x74);
			PTR(blk, 4) = top;
			int32_t n = ShatterInit(top, mesh, desc);
			U16(blk, 0xC) = (uint16_t)n;
			n &= 0xFFFF;
			blob::ArenaAlloc(n * 0x30);
			uint8_t *p = PTR(blk, 4);
			for (int32_t k = n; k > 0; k--, p += 0x30) U8(p, 0x2C) = 0xFF;
			bone = CUR();
		}
		uint8_t *blk = PTR(bone, 0xBC);
		uint8_t *desc = PTR(bone, 0xB8);
		uint8_t *mesh = PTR(blk, 8);
		PTR(WS(), 0x60) = (uint8_t *)blob::GetParentMatrix(U16(bone, 0x9C));
		PTR(WS(), 0x64) = PTR(blk, 0);
		static const uint8_t PARAM[18][2] = {
			{ 0x0E, 0xA0 }, { 0x16, 0xA2 }, { 0x1E, 0xA4 }, { 0x10, 0xA8 }, { 0x18, 0xAA }, { 0x20, 0xAC },
			{ 0x12, 0xB0 }, { 0x1A, 0xB2 }, { 0x22, 0xB4 }, { 0x14, 0xB8 }, { 0x1C, 0xBA }, { 0x24, 0xBC },
			{ 0x26, 0xC0 }, { 0x2A, 0xC2 }, { 0x2E, 0xC4 }, { 0x28, 0xC8 }, { 0x2C, 0xCA }, { 0x30, 0xCC } };
		const int32_t ky = S16(CUR(), 0x96);
		for (int i = 0; i < 18; i++)
			S16(WS(), PARAM[i][1]) = (int16_t)((S16(desc, PARAM[i][0]) * ky) / 256);
		uint8_t *p = PTR(blk, 4);
		S32(WS(), 0xF0) = S16(CUR(), 0x94);
		S32(WS(), 0xF4) = S16(CUR(), 0x8C);
		S32(WS(), 0xF8) = S16(CUR(), 0x8E);
		S32(WS(), 0xFC) = S16(CUR(), 0x90);
		int32_t n = U16(blk, 0xC);
		for (; n > 0; n--, p += 0x30)
		{
			uint8_t *w = WS();
			if (S8(p, 0x2C) >= 0 || S16(p, 0x2A) >= S32(w, 0xF0)) continue;
			int32_t r;
			r = blob::Rand73(S16(WS(), 0xA8));
			U16(p, 0xC) = (uint16_t)(U16(WS(), 0xA0) + r);
			r = blob::Rand73(S16(WS(), 0xB8));
			U16(p, 0xE) = (uint16_t)(U16(WS(), 0xB0) + r);
			r = blob::Rand73(S16(WS(), 0xAA));
			U16(p, 0x10) = (uint16_t)(U16(WS(), 0xA2) + r);
			r = blob::Rand73(S16(WS(), 0xBA));
			U16(p, 0x12) = (uint16_t)(U16(WS(), 0xB2) + r);
			r = blob::Rand73(S16(WS(), 0xAC));
			U16(p, 0x14) = (uint16_t)(U16(WS(), 0xA4) + r);
			r = blob::Rand73(S16(WS(), 0xBC));
			U16(p, 0x16) = (uint16_t)(U16(WS(), 0xB4) + r);
			r = blob::Rand73(S16(WS(), 0xC8));
			U16(p, 0x1E) = (uint16_t)(U16(WS(), 0xC0) + r);
			r = blob::Rand73(S16(WS(), 0xCA));
			U16(p, 0x20) = (uint16_t)(U16(WS(), 0xC2) + r);
			r = blob::Rand73(S16(WS(), 0xCC));
			U16(p, 0x22) = (uint16_t)(U16(WS(), 0xC4) + r);
			S32(p, 0) = mul32(S32(WS(), 0xF4), S32(p, 0)) / 256;
			S32(p, 4) = mul32(S32(WS(), 0xF8), S32(p, 4)) / 256;
			S32(p, 8) = mul32(S32(WS(), 0xFC), S32(p, 8)) / 256;
			U8(p, 0x2C) = U8(desc, 6);
		}
		h_B27130();
		uint8_t *out = PTR(CTX(), 0x74);
		const uint8_t *v = mesh + U32(mesh, 0x14);
		for (int32_t k = S32(mesh, 0x18); k > 0; k--)
		{
			x::GteWriteData((int32_t)(((uint32_t)U16(v, 2) << 16) | U16(v, 0)), 0);
			x::GteWriteData((int32_t)U16(v, 4), 1);
			x::GteRTPS();
			x::GteReadSXY2(out);
			U16(out, 4) = MEM<uint16_t>(game_addr(game::GTE_SZ3));
			v += 8;
			out += 8;
		}
		uint8_t *lst = mesh + U32(mesh, 8);
		uint8_t *verts = PTR(CTX(), 0x74);
		p = PTR(blk, 4);
		int32_t type = S16(lst, 0);
		if (type == 0xFFFF) return;
		for (;;)
		{
			if (type == 8)
			{
				PTR(WS(), 0xF0) = lst;
				p = ShatterTris(p, verts, mesh);
			}
			else if (type == 0x12)
			{
				PTR(WS(), 0xF0) = lst;
				p = ShatterQuads(p, verts, mesh);
			}
			else
				return;
			lst = PTR(WS(), 0xF0);
			type = S16(lst, 0);
			if (type == 0xFFFF) return;
		}
	}

	static VoidFn g_save_vm[0x200], g_save_big[256], g_save_draw[256];

	static void apply_draw61(Clone &c)
	{
		ff8fx::copy<(sizeof(g_save_vm))>(g_save_vm, c.vm);
		ff8fx::copy<(sizeof(g_save_big))>(g_save_big, c.bigtab);
		ff8fx::copy<(sizeof(g_save_draw))>(g_save_draw, c.draw);
		apply_eden_b(c);
		VoidFn d61 = c.draw[61];
		ff8fx::copy<(sizeof(g_save_vm))>(c.vm, g_save_vm);
		ff8fx::copy<(sizeof(g_save_big))>(c.bigtab, g_save_big);
		ff8fx::copy<(sizeof(g_save_draw))>(c.draw, g_save_draw);
		c.draw[61] = d61;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_hj);
	}
}
}

	void register_mag075_hells_judgement()
	{
		using namespace gfc;
		hells_judgement::describe(hells_judgement::g_hj);
		init_clone(hells_judgement::g_hj, nullptr, 0);
		hells_judgement::apply_draw61(hells_judgement::g_hj);
		hells_judgement::g_hj.draw[66] = hells_judgement::dh_66_ShatterMesh;
		register_port(game::GF_075HellsJudgement_SequenceTick, (void *)hells_judgement::SequenceTask, "MAG075 Hell's Judgement SequenceTick", 75);
	}
}

