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
namespace elvoret018
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace elvoret018
{
	static void describe(Clone &c)
	{
		c.name = "018 Elvoret Death";
		c.effect_id = 18;
		c.vm_table = game_addr(game::MAG_018_ElvoretDeath_VmOpcodeTable);
		c.bone_table = game_addr(game::MAG_018_ElvoretDeath_BoneHandlerTable);
		c.prim_table = game_addr(game::MAG_018_ElvoretDeath_PrimRendererTable);
		c.prim_size = game_addr(game::MAG_018_ElvoretDeath_PrimSizeTable);
		c.spr_table = game_addr(game::MAG_018_ElvoretDeath_SpriteOrientTable);
		c.ptype_table = game_addr(game::MAG_018_ElvoretDeath_ParticleTypeTable);
		c.pop_table = game_addr(game::MAG_018_ElvoretDeath_ParticleOpTable);
		c.draw_table = game_addr(game::MAG_018_ElvoretDeath_DrawHandlerTable);
		c.attr_16A = game_addr(game::MAG_018_ElvoretDeath_VmOpcodeAttribTable);
		c.attr_184 = game_addr(game::MAG_018_ElvoretDeath_VmFieldDescTable);
		c.attr_186 = game_addr(game::unk_1876D16);
		c.attr_194 = game_addr(game::unk_1876D24);
		c.attr_19C = game_addr(game::unk_1876D2C);
		c.desc = game_addr(game::MAG_018_ElvoretDeath_SectionDesc);
		c.ptr_block = game_addr(game::MAG_018_ElvoretDeath_SectionPtrBlock);
		c.queue = game_addr(game::MAG_018_ElvoretDeath_g_TaskQueue);
		c.file00 = game_addr(game::MAG_018_ElvoretDeath_g_File00);
		c.file01 = game_addr(game::MAG_018_ElvoretDeath_g_File01);
		c.tick = game_addr(game::MAG_018_ElvoretDeath_SequenceTick);
		c.load_cb = game_addr(game::MAG_018_ElvoretDeath_LoadFileDoneCallback);
		c.lit_dispatcher = true;
		c.bone_count = 18;
	}

	static inline uint8_t *xl_JointMatrix_B66B80(void *hdr, int32_t joint, int32_t scale, void *m)
	{
		return x::f<uint8_t *(__cdecl *)(void *, int32_t, int32_t, void *)>(game_addr(game::GfCinematic_ModelJointMatrix))(hdr, joint, scale, m);
	}
	static inline void xl_RotTrans(const void *v, void *out, void *flag) { x::f<void (__cdecl *)(const void *, void *, void *)>(game_addr(game::GTE_RotTrans))(v, out, flag); }
	static inline int32_t xl_RotTransPers(const void *v, void *sxy, void *p, void *flag) { return x::f<int32_t (__cdecl *)(const void *, void *, void *, void *)>(game_addr(game::TransformWorldCoordinateToProjectedSpace))(v, sxy, p, flag); }
	static inline void xl_ProjectVertexGroup_50FD40(uint8_t **stream, uint8_t **out) { x::f<void (__cdecl *)(uint8_t **, uint8_t **)>(game_addr(game::BattleModel_ProjectVerticesWithGTE))(stream, out); }
	static inline int32_t div3(int32_t v)
	{
		int32_t hi = (int32_t)(((int64_t)v * 0x55555556LL) >> 32);
		return hi + (int32_t)((uint32_t)hi >> 31);
	}

	inline int32_t &KEY70() { return MEM<int32_t>(game_addr(game::MAG_018_ElvoretDeath_g_DepthKey70)); }
	inline int32_t &KEY74() { return MEM<int32_t>(game_addr(game::MAG_018_ElvoretDeath_g_DepthKey74)); }
	inline int32_t &KEY78() { return MEM<int32_t>(game_addr(game::MAG_018_ElvoretDeath_g_DepthKey78)); }
	inline int32_t &KEY7C() { return MEM<int32_t>(game_addr(game::MAG_018_ElvoretDeath_g_DepthKey7C)); }

	static inline uint8_t *SlotEntity() { return PTR(SCENE(), 0x60 + 4 * (int32_t)U8(CUR(), 0x1B)); }

	static void SyncEntityModelCopy_B4C4E0(uint8_t *blk)
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

	static uint8_t *CreateEntityModelCopy_B4C480(uint8_t *ent)
	{
		uint8_t *blk = blob::ArenaAlloc(0x80);
		PTR(blk, 0xC) = ent;
		SyncEntityModelCopy_B4C4E0(blk);
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

	static int32_t CountModelPolygons_B4C090(uint8_t *mdl)
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

	static int32_t SumY3_B4CBD0(const uint8_t *prim)
	{
		const uint8_t *v = PTR(CTX(), 0x74);
		return S16(v, 8 * (U16(prim, 4) & 0xFFF) + 2) + S16(v, 8 * (U16(prim, 2) & 0xFFF) + 2) + S16(v, 8 * (U16(prim, 0) & 0xFFF) + 2);
	}
	static int32_t SumY4_B4CC20(const uint8_t *prim)
	{
		const uint8_t *v = PTR(CTX(), 0x74);
		int32_t s = S16(v, 8 * (U16(prim, 6) & 0xFFF) + 2) + S16(v, 8 * (U16(prim, 4) & 0xFFF) + 2);
		return s + S16(v, 8 * (U16(prim, 2) & 0xFFF) + 2) + S16(v, 8 * (U16(prim, 0) & 0xFFF) + 2);
	}

	static void PieceVelocity_B4D2D0(uint8_t *desc, uint8_t *p)
	{
		U16(p, 0x3C) = (uint16_t)(U16(desc, 0x12) + blob::Rand73(S16(desc, 0x14)));
		U16(p, 0x3E) = (uint16_t)(U16(desc, 0x16) + blob::Rand73(S16(desc, 0x18)));
		U16(p, 0x16) = (uint16_t)(U16(desc, 0x1A) + blob::Rand73(S16(desc, 0x1C)));
		U16(p, 0x1E) = (uint16_t)(U16(desc, 0x1E) + blob::Rand73(S16(desc, 0x20)));
		U16(p, 0x40) = (uint16_t)(U16(desc, 0x22) + blob::Rand73(S16(desc, 0x24)));
		U16(p, 0x42) = (uint16_t)(U16(desc, 0x26) + blob::Rand73(S16(desc, 0x28)));
	}

	static void SpawnDot_B4D3F0(uint8_t *p)
	{
		uint8_t *st = PTR(WS(), 0x84);
		uint8_t *desc = PTR(WS(), 0x90);
		int32_t c = U16(st, 0x1C);
		U16(st, 0x1C) = (uint16_t)(c + 1);
		int32_t idx = c % (int32_t)U16(desc, 0x2E);
		uint8_t *d = PTR(WS(), 0x8C) + idx * 0x1C;
		U8(d, 0x18) = 1;
		U8(d, 0x19) = U8(desc, 0x32);
		U32(d, 0) = U32(p, 4);
		U32(d, 4) = U32(p, 8);
		U32(d, 8) = U32(p, 0xC);
		U16(d, 0xC) = (uint16_t)(U16(desc, 0x34) + blob::Rand73(S16(desc, 0x36)));
		U16(d, 0xE) = (uint16_t)(U16(desc, 0x38) + blob::Rand73(S16(desc, 0x3A)));
		U16(d, 0x10) = (uint16_t)(U16(desc, 0x3C) + blob::Rand73(S16(desc, 0x3E)));
		U16(d, 0x12) = (uint16_t)(U16(desc, 0x40) + blob::Rand73(S16(desc, 0x42)));
		U16(d, 0x14) = (uint16_t)(U16(desc, 0x44) + blob::Rand73(S16(desc, 0x46)));
		U16(d, 0x16) = (uint16_t)(U16(desc, 0x48) + blob::Rand73(S16(desc, 0x4A)));
		if (U16(desc, 8) == 6)
		{
			int32_t a = blob::Blob_B66B30(0, 0, S32(d, 0) >> 8, S32(d, 8) >> 8);
			S32(d, 0) = shl32(S32(WS(), 0xFC), 8);
			S32(d, 8) = shl32(a, 4);
		}
	}

	static void SpawnParticle_B4D510(uint8_t *p)
	{
		uint8_t *desc = PTR(WS(), 0x90);
		uint8_t *st = PTR(WS(), 0x84);
		int32_t id = S16(desc, 0xE);
		if (id == 0) return;
		if (blob::Rand74(0x100) > S16(desc, 0x10)) return;
		uint8_t *b = blob::GetBone(id);
		uint8_t *pool = PTR(b, 0xB8);
		int32_t k = U16(pool, 2);
		U16(pool, 2) = (uint16_t)(k + 1);
		int32_t idx = k % (int32_t)U16(pool, 0);
		uint8_t *r = pool + idx * 0x50 + 0x10;
		U16(pool, 8) = 1;
		U16(r, 0) = 1;
		U16(r, 2) = 0;
		U32(r, 4) = U32(st, 0x10);
		U32(r, 0x14) = U32(p, 4);
		U32(r, 0x18) = U32(p, 8);
		U32(r, 0x1C) = U32(p, 0xC);
		U32(r, 0x20) = 0;
		U32(r, 0x24) = 0;
		U32(r, 0x28) = 0;
		U32(r, 0x2C) = 0;
		U16(r, 0x40) = U16(b, 0x92);
		U16(r, 0x48) = U16(b, 0x9A);
		U32(r, 0x3C) = 0;
		U32(r, 0x38) = U32(b, 0xCC) & 0x2FFFFFF;
		U8(r, 0x37) = 1;
	}

	static void LaunchPiece3_B4CC80(uint8_t *p, uint8_t *prim, uint8_t *stp)
	{
		uint8_t *desc = PTR(WS(), 0x90);
		U8(p, 0x26) = 0;
		PTR(p, 0) = prim;
		U8(stp, 0) = 1;
		U16(p, 0x30) = (uint16_t)(uint32_t)(stp - PTR(WS(), 0x88));
		U8(p, 0x2E) = U8(desc, 0xC);
		PieceVelocity_B4D2D0(desc, p);
		uint8_t *scr = PTR(CTX(), 0x74);
		static const int vo[3] = { 0xC0, 0xC8, 0xD0 };
		for (int k = 0; k < 3; k++)
		{
			uint8_t *v = scr + 8 * (U16(prim, 2 * k) & 0xFFF);
			U16(WS(), vo[k]) = U16(v, 0);
			U16(WS(), vo[k] + 2) = U16(v, 2);
			U16(WS(), vo[k] + 4) = U16(v, 4);
		}
		for (int a = 0; a < 3; a++)
		{
			uint8_t *w = WS();
			int32_t c = div3(S16(w, 0xD0 + 2 * a) + S16(w, 0xC8 + 2 * a) + S16(w, 0xC0 + 2 * a));
			S32(p, 4 + 4 * a) = shl32(c, 8);
			U16(p, 0x10 + 2 * a) = (uint16_t)(U16(WS(), 0xC0 + 2 * a) - (uint16_t)c);
			U16(p, 0x18 + 2 * a) = (uint16_t)(U16(WS(), 0xC8 + 2 * a) - (uint16_t)c);
			U16(p, 0x20 + 2 * a) = (uint16_t)(U16(WS(), 0xD0 + 2 * a) - (uint16_t)c);
		}
		U16(p, 0x32) = (uint16_t)blob::Blob_B66B30(0, 0, S32(p, 4) >> 8, S32(p, 0xC) >> 8);
		U16(p, 0x36) = U16(WS(), 0xFC);
		U16(p, 0x34) = (uint16_t)(U16(desc, 0x2A) + blob::Rand73(S16(desc, 0x2C)));
		for (int32_t n = U16(desc, 0x30); n > 0; n--) SpawnDot_B4D3F0(p);
		SpawnParticle_B4D510(p);
	}

	static void LaunchPiece4_B4CF60(uint8_t *p, uint8_t *prim, uint8_t *stp)
	{
		uint8_t *desc = PTR(WS(), 0x90);
		U8(p, 0x26) = 1;
		PTR(p, 0) = prim;
		U8(stp, 0) = 1;
		U16(p, 0x30) = (uint16_t)(uint32_t)(stp - PTR(WS(), 0x88));
		U8(p, 0x2E) = U8(desc, 0xC);
		PieceVelocity_B4D2D0(desc, p);
		uint8_t *scr = PTR(CTX(), 0x74);
		static const int vo[4] = { 0xC0, 0xC8, 0xD0, 0xD8 };
		for (int k = 0; k < 4; k++)
		{
			uint8_t *v = scr + 8 * (U16(prim, 2 * k) & 0xFFF);
			U16(WS(), vo[k]) = U16(v, 0);
			U16(WS(), vo[k] + 2) = U16(v, 2);
			U16(WS(), vo[k] + 4) = U16(v, 4);
		}
		for (int a = 0; a < 3; a++)
		{
			uint8_t *w = WS();
			int32_t s = S16(w, 0xD8 + 2 * a) + S16(w, 0xD0 + 2 * a);
			s += S16(w, 0xC8 + 2 * a);
			s += S16(w, 0xC0 + 2 * a);
			int32_t c = s / 4;
			S32(p, 4 + 4 * a) = shl32(c, 8);
			U16(p, 0x10 + 2 * a) = (uint16_t)(U16(WS(), 0xC0 + 2 * a) - (uint16_t)c);
			U16(p, 0x18 + 2 * a) = (uint16_t)(U16(WS(), 0xC8 + 2 * a) - (uint16_t)c);
			U16(p, 0x20 + 2 * a) = (uint16_t)(U16(WS(), 0xD0 + 2 * a) - (uint16_t)c);
			U16(p, 0x28 + 2 * a) = (uint16_t)(U16(WS(), 0xD8 + 2 * a) - (uint16_t)c);
		}
		U16(p, 0x32) = (uint16_t)blob::Blob_B66B30(0, 0, S32(p, 4) >> 8, S32(p, 0xC) >> 8);
		U16(p, 0x36) = U16(WS(), 0xFC);
		U16(p, 0x34) = (uint16_t)(U16(desc, 0x2A) + blob::Rand73(S16(desc, 0x2C)));
		for (int32_t n = U16(desc, 0x30); n > 0; n--) SpawnDot_B4D3F0(p);
		SpawnParticle_B4D510(p);
	}

	static int32_t LaunchByCounter_B4D360(uint8_t *p, uint8_t *prim, uint8_t *stp, int32_t quad)
	{
		uint8_t *desc = PTR(WS(), 0x90);
		if (U16(desc, 0x2E) == 0) return 0;
		S32(WS(), 0x44) = S32(WS(), 0x44) - 1;
		if (S32(WS(), 0x44) > 0) return 0;
		S32(WS(), 0x44) = S32(WS(), 0x40);
		if (quad == 0) LaunchPiece3_B4CC80(p, prim, stp);
		else LaunchPiece4_B4CF60(p, prim, stp);
		for (int32_t n = U16(desc, 0x30); n > 0; n--) SpawnDot_B4D3F0(p);
		return 1;
	}

	static inline uint8_t *FreeSlot(uint8_t *p)
	{
		while (U32(p, 0) != 0) p += 0x44;
		return p;
	}

	static void UpdatePolygons_B4C530()
	{
		S32(WS(), 0xE4) = S32(WS(), 0xF0);
		uint8_t *stp = PTR(WS(), 0x88);
		uint8_t *st = PTR(WS(), 0x84);
		uint8_t *per = PTR(st, 0x18);
		uint8_t *mdl = PTR(WS(), 0x80) + 0x30;
		uint8_t *bones = PTR(mdl, 0) + 0x10;
		uint8_t *tab = PTR(mdl, 4);
		int32_t nobj = S32(tab, 0);
		tab += 4;
		int32_t alive = 0;
		bool alive_set = false;
		for (int32_t o = 0; o < nobj; o++)
		{
			uint8_t *s = PTR(mdl, 4) + U32(tab, 0);
			tab += 4;
			uint8_t *out = PTR(CTX(), 0x74);
			int32_t ng = S16(s, 0);
			s += 2;
			uint8_t *v = WS() + 0xF0;
			for (; ng > 0; ng--)
			{
				uint8_t *m = bones + S16(s, 0) * 48 + 0x10;
				s += 2;
				x::SetRotMatrix(m);
				x::SetTransVector(m);
				int32_t nv = S16(s, 0);
				s += 2;
				for (; nv > 0; nv--)
				{
					U16(v, 0) = U16(s, 0);
					U16(v, 2) = U16(s, 2);
					U16(v, 4) = U16(s, 4);
					xl_RotTrans(v, out, WS() + 0xFC);
					out += 8;
					s += 6;
				}
			}
			uint8_t *c = (uint8_t *)(((uint32_t)s + 3) & ~3u);
			S32(WS(), 0xE8) = S16(c, 0);
			c += 2;
			S32(WS(), 0xEC) = S16(c, 0);
			c += 0xA;
			const uint32_t mode = U32(WS(), 0x50);
			if (mode > 7 || mode == 4) continue;
			uint8_t *prim = c;
			st = PTR(WS(), 0x84);
			uint8_t *p = st + 0x20;
			S32(WS(), 0xE0) = (int32_t)U16(st, 2) + 1;
			alive = U16(st, 6);
			alive_set = true;
			{
				int32_t lim = mode == 5 ? (int32_t)S16(CUR(), 0x96) : S32(WS(), 0xE4) * 3;
				for (int32_t n = S32(WS(), 0xE8); n > 0; n--)
				{
					if (U8(stp, 0) == 0)
					{
						bool take;
						if (mode == 5) take = S16(per, 2) > lim;
						else if (mode == 7) take = SumY3_B4CBD0(prim) > lim;
						else take = SumY3_B4CBD0(prim) < lim;
						if (alive >= S32(WS(), 0xE0)) goto done;
						p = FreeSlot(p);
						if (take)
						{
							LaunchPiece3_B4CC80(p, prim, stp);
							p += 0x44;
							alive++;
						}
						else if (LaunchByCounter_B4D360(p, prim, stp, 0) == 1)
						{
							p += 0x44;
							alive++;
						}
					}
					stp++;
					if (mode == 5) per += 4;
					prim += 0x10;
				}
			}
			U16(st, 6) = (uint16_t)alive;
			{
				int32_t lim = mode == 5 ? (int32_t)S16(CUR(), 0x96) : shl32(S32(WS(), 0xE4), 2);
				for (int32_t n = S32(WS(), 0xEC); n > 0; n--)
				{
					if (U8(stp, 0) == 0)
					{
						bool take;
						if (mode == 5) take = S16(per, 2) > lim;
						else if (mode == 7) take = SumY4_B4CC20(prim) > lim;
						else take = SumY4_B4CC20(prim) < lim;
						if (alive >= S32(WS(), 0xE0)) goto done;
						p = FreeSlot(p);
						if (take)
						{
							LaunchPiece4_B4CF60(p, prim, stp);
							p += 0x44;
							alive++;
						}
						else if (LaunchByCounter_B4D360(p, prim, stp, 1) == 1)
						{
							p += 0x44;
							alive++;
						}
					}
					stp++;
					if (mode == 5) per += 4;
					prim += 0x14;
				}
			}
			U16(st, 6) = (uint16_t)alive;
		}
	done:
		U16(st, 6) = alive_set ? (uint16_t)alive : (uint16_t)0xBE7E;
	}

	static void InsertPrim_B4DDD0(int32_t z, void *prim);

	static inline void xl_Cross_56BBF0(const void *a, const void *b, void *out) { x::f<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::FixedPointCrossProduct))(a, b, out); }
	static inline void xl_Normalize_56BD20(const void *in, void *out) { x::f<void (__cdecl *)(const void *, void *)>(game_addr(game::Math_NormalizeVec3_Q12))(in, out); }
	static inline int32_t xl_NClip_56CB30(uint32_t s0, uint32_t s1, uint32_t s2) { return x::f<int32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_NormalClip3))(s0, s1, s2); }
	static inline void xl_NormalColor_56CA30(const void *n, const void *col, void *out) { x::f<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::sub_56CA30))(n, col, out); }

	static void PolygonValues_B4D5F0(uint8_t *hdr, uint8_t *out)
	{
		x::BuildBoneMatricesFromPose(hdr);
		xl_JointMatrix_B66B80(hdr, 0xF1, 0x1000, BBMAT());
		uint8_t *w = WS();
		U16(w, 0xD0) = U16(w, 0xF0);
		U16(WS(), 0xD2) = U16(WS(), 0xF4);
		U16(WS(), 0xD4) = U16(WS(), 0xF8);
		uint8_t *mdl = PTR(hdr, 4);
		uint8_t *bones = PTR(mdl, 0) + 0x10;
		uint8_t *tab = PTR(mdl, 4);
		int32_t n = S32(tab, 0);
		tab += 4;
		for (; n > 0; n--)
		{
			uint8_t *s = PTR(mdl, 4) + U32(tab, 0);
			tab += 4;
			uint8_t *scr = PTR(CTX(), 0x74);
			int32_t ng = S16(s, 0);
			s += 2;
			uint8_t *v = WS() + 0xF0;
			for (; ng > 0; ng--)
			{
				uint8_t *m = bones + S16(s, 0) * 48 + 0x10;
				s += 2;
				x::SetRotMatrix(m);
				x::SetTransVector(m);
				int32_t nv = S16(s, 0);
				s += 2;
				for (; nv > 0; nv--)
				{
					U16(v, 0) = U16(s, 0);
					U16(v, 2) = U16(s, 2);
					U16(v, 4) = U16(s, 4);
					xl_RotTrans(v, scr, WS() + 0xFC);
					scr += 8;
					s += 6;
				}
			}
			uint8_t *q = (uint8_t *)(((uint32_t)s + 3) & ~3u);
			S32(WS(), 0xE8) = S16(q, 0);
			q += 2;
			S32(WS(), 0xEC) = S16(q, 0);
			q += 0xA;
			const uint8_t *sv = PTR(CTX(), 0x74);
			while (S32(WS(), 0xE8) > 0)
			{
				const uint8_t *v0 = sv + 8 * (U16(q, 0) & 0xFFF), *v1 = sv + 8 * (U16(q, 2) & 0xFFF), *v2 = sv + 8 * (U16(q, 4) & 0xFFF);
				const int32_t cx = div3(S16(v0, 0) + S16(v1, 0) + S16(v2, 0));
				const int32_t cz = div3(S16(v0, 4) + S16(v1, 4) + S16(v2, 4));
				w = WS();
				U16(out, 0) = (uint16_t)blob::Blob_B66B30(S16(w, 0xD0), S16(w, 0xD4), cx, cz);
				out += 4;
				q += 0x10;
				U16(out, -2) = U16(WS(), 0xFC);
				S32(WS(), 0xE8) = S32(WS(), 0xE8) - 1;
			}
			if (S32(WS(), 0xEC) > 0)
			{
				q += 4;
				while (S32(WS(), 0xEC) > 0)
				{
					const uint8_t *v0 = sv + 8 * (U16(q, -4) & 0xFFF), *v1 = sv + 8 * (U16(q, -2) & 0xFFF);
					const uint8_t *v2 = sv + 8 * (U16(q, 0) & 0xFFF), *v3 = sv + 8 * (U16(q, 2) & 0xFFF);
					const int32_t cx = (S16(v0, 0) + S16(v1, 0) + S16(v2, 0) + S16(v3, 0)) / 4;
					const int32_t cz = (S16(v0, 4) + S16(v1, 4) + S16(v2, 4) + S16(v3, 4)) / 4;
					w = WS();
					U16(out, 0) = (uint16_t)blob::Blob_B66B30(S16(w, 0xD0), S16(w, 0xD4), cx, cz);
					out += 4;
					q += 0x14;
					U16(out, -2) = U16(WS(), 0xFC);
					S32(WS(), 0xEC) = S32(WS(), 0xEC) - 1;
				}
			}
		}
	}

	static void PolygonNormals_B4C130(uint8_t *hdr, uint8_t *out)
	{
		x::BuildBoneMatricesFromPose(hdr);
		uint8_t *mdl = PTR(hdr, 4);
		const int32_t nvert = CountModelPolygons_B4C090(mdl);
		uint8_t *vbase = PTR(CTX(), 0x74) + 8 * nvert;
		uint8_t *tab = PTR(mdl, 4) + 4;
		uint8_t *bones = PTR(mdl, 0) + 0x10;
		int32_t n = S32(tab, -4);
		for (; n > 0; n--)
		{
			uint8_t *s = PTR(mdl, 4) + U32(tab, 0);
			tab += 4;
			uint8_t *o = vbase;
			int32_t ng = S16(s, 0);
			s += 2;
			for (; ng > 0; ng--)
			{
				uint8_t *m = bones + S16(s, 0) * 48 + 0x10;
				s += 2;
				x::SetRotMatrix(m);
				x::SetTransVector(m);
				int32_t nv = S16(s, 0);
				s += 2;
				for (; nv > 0; nv--)
				{
					x::GteWriteData((int32_t)(((uint32_t)U16(s, 2) << 16) | U16(s, 0)), 0);
					x::GteWriteData((int32_t)U16(s, 4), 1);
					x::GteMVMVA_RotV0Tr();
					U16(o, 0) = MEM<uint16_t>(game_addr(game::GTE_MAC123));
					U16(o, 2) = (uint16_t)MEM<uint32_t>(game_addr(game::unk_1CA8A78));
					U16(o, 4) = (uint16_t)MEM<uint32_t>(game_addr(game::unk_1CA8A7C));
					o += 8;
					s += 6;
				}
			}
			s = (uint8_t *)(((uint32_t)s + 3) & ~3u);
			const int32_t ntri = S16(s, 0);
			const int32_t nquad = S16(s, 2);
			s += 0xC;
			for (int pass = 0; pass < 2; pass++)
			{
				int32_t k = pass == 0 ? ntri : nquad;
				if (k <= 0) continue;
				if (pass == 1) s += 4;
				const int ofs = pass == 0 ? 0 : -4;
				for (; k > 0; k--)
				{
					const uint8_t *a = vbase + 8 * (U16(s, ofs) & 0xFFF);
					const uint8_t *b = vbase + 8 * (U16(s, ofs + 2) & 0xFFF);
					const uint8_t *c = vbase + 8 * (U16(s, ofs + 4) & 0xFFF);
					uint8_t *w = WS();
					S32(w, 0xE0) = S16(b, 0) - S16(a, 0);
					S32(WS(), 0xE4) = S16(b, 2) - S16(a, 2);
					S32(WS(), 0xE8) = S16(b, 4) - S16(a, 4);
					S32(WS(), 0xF0) = S16(c, 0) - S16(a, 0);
					S32(WS(), 0xF4) = S16(c, 2) - S16(a, 2);
					S32(WS(), 0xF8) = S16(c, 4) - S16(a, 4);
					w = WS();
					xl_Cross_56BBF0(w + 0xE0, w + 0xF0, w + 0xF0);
					xl_Normalize_56BD20(WS() + 0xF0, out);
					out += 8;
					s += pass == 0 ? 0x10 : 0x14;
				}
			}
		}
	}

	static uint8_t *DrawModelPolygonsLit_B4E770(uint8_t *nrm)
	{
		uint8_t *d = PTR(WS(), 0x80) + 0x40;
		uint8_t *stp = PTR(WS(), 0x88);
		uint8_t *pk = PTR(CTX(), 0x7C);
		uint8_t *vb = PTR(d, 4);
		uint8_t *q = PTR(d, 0);
		for (int32_t n = U16(d, 8); n > 0; n--, q += 0x10, nrm += 8, stp++)
		{
			if (U8(stp, 0) != 0) continue;
			uint8_t *v = vb + 8 * (U16(q, 0) & 0xFFF);
			U32(pk, 8) = U32(v, 0);
			KEY78() = U16(v, 4);
			int32_t sz = U16(v, 4);
			v = vb + 8 * (U16(q, 2) & 0xFFF);
			U32(pk, 0x10) = U32(v, 0);
			KEY7C() = U16(v, 4);
			sz += U16(v, 4);
			v = vb + 8 * (U16(q, 4) & 0xFFF);
			U32(pk, 0x18) = U32(v, 0);
			KEY74() = 0;
			KEY70() = U16(v, 4);
			sz += U16(v, 4);
			xl_NClip_56CB30(U32(pk, 8), U32(pk, 0x10), U32(pk, 0x18));
			U8(pk, 3) = 7;
			xl_NormalColor_56CA30(nrm, d + 0x1C, WS() + 0xFC);
			U32(pk, 4) = (U32(WS(), 0xFC) & 0xFFFFFF) | 0x24000000;
			U16(pk, 0xC) = U16(q, 8);
			U16(pk, 0x14) = U16(q, 0xC);
			U16(pk, 0x1C) = U16(q, 6);
			U16(pk, 0x16) = U16(q, 0xE);
			U16(pk, 0xE) = U16(q, 0xA);
			InsertPrim_B4DDD0(div3(sz) >> 4, pk);
			pk += 0x20;
		}
		for (int32_t n = U16(d, 0xA); n > 0; n--, q += 0x14, nrm += 8, stp++)
		{
			if (U8(stp, 0) != 0) continue;
			uint8_t *v = vb + 8 * (U16(q, 0) & 0xFFF);
			U32(pk, 8) = U32(v, 0);
			KEY78() = U16(v, 4);
			int32_t sz = U16(v, 4);
			v = vb + 8 * (U16(q, 2) & 0xFFF);
			U32(pk, 0x10) = U32(v, 0);
			KEY7C() = U16(v, 4);
			sz += U16(v, 4);
			v = vb + 8 * (U16(q, 4) & 0xFFF);
			U32(pk, 0x18) = U32(v, 0);
			KEY70() = U16(v, 4);
			sz += U16(v, 4);
			v = vb + 8 * (U16(q, 6) & 0xFFF);
			U32(pk, 0x20) = U32(v, 0);
			KEY74() = U16(v, 4);
			sz += U16(v, 4);
			xl_NClip_56CB30(U32(pk, 8), U32(pk, 0x10), U32(pk, 0x18));
			U8(pk, 3) = 9;
			xl_NormalColor_56CA30(nrm, d + 0x1C, WS() + 0xFC);
			U32(pk, 4) = (U32(WS(), 0xFC) & 0xFFFFFF) | 0x2C000000;
			U16(pk, 0xC) = U16(q, 8);
			U16(pk, 0x14) = U16(q, 0xC);
			U16(pk, 0x1C) = U16(q, 0x10);
			U16(pk, 0x24) = U16(q, 0x12);
			U16(pk, 0x16) = U16(q, 0xE);
			U16(pk, 0xE) = U16(q, 0xA);
			InsertPrim_B4DDD0((sz / 4) >> 4, pk);
			pk += 0x28;
		}
		PTR(CTX(), 0x7C) = pk;
		PTR(WS(), 0x88) = stp;
		return nrm;
	}

	static inline void StepAxis(uint8_t *p, int vel, int acc, int pos)
	{
		U16(p, vel) = (uint16_t)(U16(p, vel) + U16(p, acc));
		S32(p, pos) = add32(S32(p, pos), shl32(S16(p, vel), 4));
	}

	static void StepPieces_B4D990()
	{
		uint8_t *st = PTR(CUR(), 0xC0);
		uint8_t *desc = PTR(CUR(), 0xB8);
		int32_t n = (int32_t)U16(st, 2) + 1;
		const uint32_t mode = U16(desc, 8);
		uint8_t *p = st + 0x20;
		if (mode > 7 || mode == 4 || n <= 0) return;
		for (; n > 0; n--, p += 0x44)
		{
			if (U32(p, 0) == 0) continue;
			if (mode == 1 || mode == 3)
			{
				StepAxis(p, 0x3C, 0x3E, 4);
				StepAxis(p, 0x40, 0x42, 0xC);
				StepAxis(p, 0x16, 0x1E, 8);
				if (S32(p, 8) > 0)
				{
					U16(p, 0x1E) = 0;
					U16(p, 0x16) = 0;
					U16(p, 0x3E) = 0;
					U16(p, 0x3C) = 0;
					U16(p, 0x42) = 0;
					U16(p, 0x40) = 0;
				}
			}
			else if (mode == 2)
			{
				StepAxis(p, 0x3C, 0x3E, 4);
				StepAxis(p, 0x16, 0x1E, 8);
				StepAxis(p, 0x40, 0x42, 0xC);
			}
			else
			{
				StepAxis(p, 0x16, 0x1E, 8);
				U16(p, 0x32) = (uint16_t)(U16(p, 0x32) + U16(p, 0x34));
			}
			const int8_t life = S8(p, 0x2E);
			U8(p, 0x2E) = (uint8_t)(life - 1);
			if (life <= 0)
			{
				U32(p, 0) = 0;
				U16(st, 6) = (uint16_t)(U16(st, 6) - 1);
				U8(PTR(WS(), 0x88), U16(p, 0x30)) = 0xFF;
			}
		}
	}

	static void InsertPrim_B4DDD0(int32_t z, void *prim)
	{
		if (z >= 0xFFF) return;
		x::InsertPrimDepthKeys(U32(RT(), 0x4C) + 4 * (uint32_t)z, prim, KEY78(), KEY7C(), KEY70(), KEY74());
	}

	static void DrawModelPolygons_B4DFA0()
	{
		uint8_t *d = PTR(WS(), 0x80) + 0x40;
		uint8_t *stp = PTR(WS(), 0x88);
		uint8_t *pk = PTR(CTX(), 0x7C);
		uint8_t *vb = PTR(d, 4);
		uint8_t *q = PTR(d, 0);
		for (int32_t n = U16(d, 8); n > 0; n--, q += 0x10, stp++)
		{
			if (U8(stp, 0) != 0) continue;
			uint8_t *v = vb + 8 * (U16(q, 0) & 0xFFF);
			U32(pk, 8) = U32(v, 0);
			uint32_t fl = U16(v, 6) & 0x8000;
			KEY78() = U16(v, 4);
			int32_t sz = U16(v, 4);
			v = vb + 8 * (U16(q, 2) & 0xFFF);
			U32(pk, 0x10) = U32(v, 0);
			fl |= U16(v, 6) & 0x8000;
			KEY7C() = U16(v, 4);
			sz += U16(v, 4);
			v = vb + 8 * (U16(q, 4) & 0xFFF);
			U32(pk, 0x18) = U32(v, 0);
			fl |= U16(v, 6) & 0x8000;
			KEY74() = 0;
			KEY70() = U16(v, 4);
			sz += U16(v, 4);
			const int32_t y0 = S16(pk, 0xA);
			int32_t lo = y0, hi = y0;
			if (S16(pk, 0x12) < lo) lo = S16(pk, 0x12);
			if (S16(pk, 0x1A) < lo) lo = S16(pk, 0x1A);
			if (S16(pk, 0x12) > hi) hi = S16(pk, 0x12);
			if (S16(pk, 0x1A) > hi) hi = S16(pk, 0x1A);
			if (fl != 0) continue;
			if (lo < 0 && hi > 0xD2) continue;
			U8(pk, 3) = 7;
			U32(pk, 4) = (U32(d, 0x1C) & 0xFFFFFF) | 0x24000000;
			U16(pk, 0xC) = U16(q, 8);
			U16(pk, 0x14) = U16(q, 0xC);
			U16(pk, 0x1C) = U16(q, 6);
			U16(pk, 0x16) = U16(q, 0xE);
			U16(pk, 0xE) = U16(q, 0xA);
			InsertPrim_B4DDD0(div3(sz) >> 4, pk);
			pk += 0x20;
		}
		for (int32_t n = U16(d, 0xA); n > 0; n--, q += 0x14, stp++)
		{
			if (U8(stp, 0) != 0) continue;
			uint8_t *v = vb + 8 * (U16(q, 0) & 0xFFF);
			U32(pk, 8) = U32(v, 0);
			KEY78() = U16(v, 4);
			uint32_t fl = U16(v, 6) & 0x8000;
			int32_t sz = U16(v, 4);
			v = vb + 8 * (U16(q, 2) & 0xFFF);
			U32(pk, 0x10) = U32(v, 0);
			fl |= U16(v, 6) & 0x8000;
			KEY7C() = U16(v, 4);
			sz += U16(v, 4);
			v = vb + 8 * (U16(q, 4) & 0xFFF);
			U32(pk, 0x18) = U32(v, 0);
			fl |= U16(v, 6) & 0x8000;
			KEY70() = U16(v, 4);
			sz += U16(v, 4);
			v = vb + 8 * (U16(q, 6) & 0xFFF);
			U32(pk, 0x20) = U32(v, 0);
			fl |= U16(v, 6) & 0x8000;
			KEY74() = U16(v, 4);
			sz += U16(v, 4);
			if (fl != 0) continue;
			U8(pk, 3) = 9;
			U32(pk, 4) = (U32(d, 0x1C) & 0xFFFFFF) | 0x2C000000;
			U16(pk, 0xC) = U16(q, 8);
			U16(pk, 0x14) = U16(q, 0xC);
			U16(pk, 0x1C) = U16(q, 0x10);
			U16(pk, 0x24) = U16(q, 0x12);
			U16(pk, 0x16) = U16(q, 0xE);
			U16(pk, 0xE) = U16(q, 0xA);
			InsertPrim_B4DDD0((sz / 4) >> 4, pk);
			pk += 0x28;
		}
		PTR(CTX(), 0x7C) = pk;
		PTR(WS(), 0x88) = stp;
	}

	static void DrawPiece(uint8_t *&pk, const uint8_t *p, int32_t px, int32_t py, int32_t pz, int8_t life)
	{
		uint8_t *prim = PTR(p, 0);
		const int nv = U8(p, 0x26) ? 4 : 3;
		uint8_t *v = WS() + 0xF0;
		const int32_t x = px >> 8, y = py >> 8, z = pz >> 8;
		int32_t sum = 0;
		for (int k = 0; k < nv; k++)
		{
			U16(v, 0) = (uint16_t)(U16(p, 0x10 + 8 * k) + (uint16_t)x);
			U16(v, 2) = (uint16_t)(U16(p, 0x12 + 8 * k) + (uint16_t)y);
			U16(v, 4) = (uint16_t)(U16(p, 0x14 + 8 * k) + (uint16_t)z);
			uint8_t *w = WS() + 0xFC;
			sum += xl_RotTransPers(v, pk + 8 + 8 * k, w, w);
			const int32_t key = shl32(S32(WS(), 0xFC), 2);
			if (k == 0) KEY78() = key;
			else if (k == 1) KEY7C() = key;
			else if (k == 2)
			{
				if (nv == 3) KEY74() = 0;
				KEY70() = key;
			}
			else KEY74() = key;
		}
		if (nv == 4)
		{
			U8(pk, 3) = 9;
			U32(pk, 4) = U32(PTR(WS(), 0x48), 4 * (int32_t)life) | 0x2E000000;
			U16(pk, 0xC) = U16(prim, 8);
			U16(pk, 0x14) = U16(prim, 0xC);
			U16(pk, 0x1C) = U16(prim, 0x10);
			U16(pk, 0x24) = U16(prim, 0x12);
			U16(pk, 0x16) = (uint16_t)(U16(prim, 0xE) | 0x20);
			U16(pk, 0xE) = U16(prim, 0xA);
			InsertPrim_B4DDD0((sum / 4) >> 2, pk);
			pk += 0x28;
		}
		else
		{
			U8(pk, 3) = 7;
			U32(pk, 4) = U32(PTR(WS(), 0x48), 4 * (int32_t)life) | 0x26000000;
			U16(pk, 0xC) = U16(prim, 8);
			U16(pk, 0x14) = U16(prim, 0xC);
			U16(pk, 0x1C) = U16(prim, 6);
			U16(pk, 0x16) = (uint16_t)(U16(prim, 0xE) | 0x20);
			U16(pk, 0xE) = U16(prim, 0xA);
			InsertPrim_B4DDD0(div3(sum) >> 2, pk);
			pk += 0x20;
		}
	}

	static void DrawPieces_B4E300()
	{
		uint8_t *st = PTR(WS(), 0x84);
		uint8_t *pk = PTR(CTX(), 0x7C);
		uint8_t *p = st + 0x20;
		h_B27130();
		for (int32_t n = (int32_t)U16(st, 2) + 1; n > 0; n--, p += 0x44)
			if (PTR(p, 0)) DrawPiece(pk, p, S32(p, 4), S32(p, 8), S32(p, 0xC), S8(p, 0x2E));
		PTR(CTX(), 0x7C) = pk;
	}

	static void DrawModel_B4DE20()
	{
		uint8_t *st = PTR(WS(), 0x84);
		uint8_t *blk = PTR(WS(), 0x80);
		uint8_t *extra = PTR(st, 0x14);
		S32(WS(), 0x60) = 0;
		uint8_t *bones = PTR(blk, 0x30) + 0x10;
		uint8_t *tab = PTR(blk, 0x34) + 4;
		const int32_t nobj = S32(tab, -4);
		if (MEM<uint8_t>(game_addr(game::some_entity_slot_id)) != 0xFF)
		{
			U32(blk, 0x5C) = MEM<uint32_t>(game_addr(game::unk_B8BA00));
			U8(blk, 0x5F) = 0;
		}
		x::GteSetBackColor3(U8(blk, 0x5C), U8(blk, 0x5D), U8(blk, 0x5E));
		for (int32_t i = 0; i < nobj; i++)
		{
			uint8_t *o = PTR(blk, 0x34) + U32(tab, 0);
			tab += 4;
			if (!(U32(blk, 0x60) & (1u << i))) continue;
			uint8_t *out = PTR(blk, 0x44);
			int32_t ng = S16(o, 0);
			o += 2;
			for (; ng > 0; ng--)
			{
				uint8_t *m = bones + S16(o, 0) * 48 + 0x10;
				o += 2;
				x::SetRotMatrix(m);
				x::SetTransVector(m);
				xl_ProjectVertexGroup_50FD40(&o, &out);
			}
			o = (uint8_t *)(((uint32_t)o + 3) & ~3u);
			U16(blk, 0x48) = U16(o, 0);
			U16(blk, 0x4A) = U16(o, 2);
			U16(blk, 0x4C) = U16(o, 4);
			U16(blk, 0x4E) = U16(o, 6);
			PTR(blk, 0x40) = o + 0xC;
			if (U8(CUR(), 0xE1)) extra = DrawModelPolygonsLit_B4E770(extra);
			else DrawModelPolygons_B4DFA0();
			DrawPieces_B4E300();
		}
	}

	static void DrawDot(uint8_t *pk, const uint8_t *desc, int8_t life, int32_t dx, int32_t dy, int32_t dz)
	{
		U8(pk, 3) = 2;
		U32(pk, 4) = U32(PTR(WS(), 0x4C), 4 * (int32_t)life) | 0x6A000000;
		uint8_t *v = WS() + 0xF0;
		if (U16(desc, 8) == 6)
		{
			const int32_t a = dz >> 4;
			const int32_t r = dx >> 8;
			U16(v, 0) = (uint16_t)(mul32(x::Sin(a), r) >> 12);
			U16(v, 4) = (uint16_t)(mul32(x::Cos(a), r) >> 12);
			U16(v, 2) = (uint16_t)(dy >> 8);
		}
		else
		{
			U16(v, 0) = (uint16_t)(dx >> 8);
			U16(v, 2) = (uint16_t)(dy >> 8);
			U16(v, 4) = (uint16_t)(dz >> 8);
		}
		uint8_t *w = WS() + 0xFC;
		const int32_t z = xl_RotTransPers(v, pk + 8, w, w);
		const int32_t key = shl32(z, 2);
		KEY78() = key;
		KEY7C() = key;
		KEY70() = key;
		KEY74() = key;
		InsertPrim_B4DDD0(z >> 2, pk);
	}

	static void Dots_B4DB80()
	{
		uint8_t *d = PTR(WS(), 0x8C);
		uint8_t *desc = PTR(WS(), 0x90);
		uint8_t *pk = PTR(CTX(), 0xD8);
		for (int32_t n = U16(desc, 0x2E); n > 0; n--, d += 0x1C)
		{
			if (!U8(d, 0x18)) continue;
			const uint32_t mode = U16(desc, 8);
			if (mode <= 7 && mode != 4)
			{
				const int8_t life = (int8_t)(S8(d, 0x19) - 1);
				U8(d, 0x19) = (uint8_t)life;
				if (life < 0)
				{
					U8(d, 0x18) = 0;
					continue;
				}
				if (mode == 1 || mode == 3)
				{
					StepAxis(d, 0xC, 0xE, 0);
					StepAxis(d, 0x14, 0x16, 8);
					StepAxis(d, 0x10, 0x12, 4);
					if (S32(d, 4) > 0)
					{
						U16(d, 0x12) = 0;
						U16(d, 0x10) = 0;
					}
				}
				else
				{
					StepAxis(d, 0xC, 0xE, 0);
					StepAxis(d, 0x10, 0x12, 4);
					StepAxis(d, 0x14, 0x16, 8);
				}
			}
			DrawDot(pk, desc, S8(d, 0x19), S32(d, 0), S32(d, 4), S32(d, 8));
			pk += 0xC;
		}
		PTR(CTX(), 0xD8) = pk;
	}

	static void __cdecl dh_22_EntityDisintegrate()
	{
		if (U32(CUR(), 0xBC) == 0)
		{
			uint8_t *desc = PTR(CUR(), 0xB8);
			PTR(CUR(), 0xBC) = CreateEntityModelCopy_B4C480(SlotEntity());
			const int32_t slots = U16(desc, 0xA);
			PTR(CUR(), 0xC0) = blob::ArenaAlloc(slots * 0x44 + 0x20);
			uint8_t *st = PTR(CUR(), 0xC0);
			U16(st, 0) = 0;
			U16(st, 2) = (uint16_t)(slots - 1);
			U16(st, 6) = 0;
			PTR(st, 8) = desc + S16(desc, 0);
			PTR(st, 0xC) = desc + S16(desc, 2);
			PTR(st, 0x10) = desc + S16(desc, 4);
			if (U16(desc, 0x2E) != 0)
			{
				U16(st, 0x1C) = 0;
				const int32_t nd = U16(desc, 0x2E);
				PTR(CUR(), 0xA0) = blob::ArenaAlloc(nd * 0x1C);
				uint8_t *d = PTR(CUR(), 0xA0);
				for (int32_t k = nd; k > 0; k--, d += 0x1C) U8(d, 0x18) = 0;
			}
			uint8_t *blk = PTR(CUR(), 0xBC);
			CountModelPolygons_B4C090(blk + 0x30);
			const int32_t npoly = add32(S32(WS(), 0xFC), S32(WS(), 0xF8));
			U16(st, 4) = (uint16_t)npoly;
			const int32_t nb = add32(npoly, 4) & ~3;
			PTR(CUR(), 0xC4) = blob::ArenaAlloc(nb);
			uint8_t *sb = PTR(CUR(), 0xC4);
			if (nb > 0) fill_bytes(sb, 0, 4 * (size_t)((uint32_t)(nb + 3) >> 2));
			uint8_t *p = st + 0x20;
			for (int32_t k = (int32_t)U16(st, 2) + 1; k > 0; k--, p += 0x44) U32(p, 0) = 0;
			if (U16(desc, 8) == 5)
			{
				uint8_t *b = blob::ArenaAlloc(4 * (int32_t)U16(st, 4));
				PTR(st, 0x18) = b;
				PolygonValues_B4D5F0(blk + 0x10, b);
			}
			if (U8(CUR(), 0xE1))
			{
				uint8_t *b = blob::ArenaAlloc(8 * (int32_t)U16(st, 4));
				PTR(st, 0x14) = b;
				PolygonNormals_B4C130(blk + 0x10, b);
			}
			else
				PTR(st, 0x14) = nullptr;
			U8(SlotEntity(), 0) |= 4;
		}
		uint8_t *bone = CUR();
		PTR(WS(), 0x90) = PTR(bone, 0xB8);
		PTR(WS(), 0x80) = PTR(CUR(), 0xBC);
		PTR(WS(), 0x84) = PTR(CUR(), 0xC0);
		uint8_t *st = PTR(WS(), 0x84);
		PTR(WS(), 0x88) = PTR(CUR(), 0xC4);
		PTR(WS(), 0x8C) = PTR(CUR(), 0xA0);
		U32(WS(), 0x48) = U32(st, 8);
		U32(WS(), 0x4C) = U32(st, 0xC);
		SyncEntityModelCopy_B4C4E0(PTR(CUR(), 0xBC));
		x::BuildBoneMatricesFromPose(PTR(WS(), 0x80) + 0x10);
		if (U8(RT(), 0x45) == 0)
		{
			uint8_t *w = WS();
			st = PTR(w, 0x84);
			S32(w, 0x50) = U16(PTR(w, 0x90), 8);
			S32(WS(), 0x54) = S16(CUR(), 0x94);
			S32(WS(), 0x58) = S16(CUR(), 0x98);
			S32(WS(), 0x5C) = 0;
			w = WS();
			const int32_t zz = S16(CUR(), 0x98);
			int32_t per;
			if (zz != 0)
			{
				const uint8_t *sb = PTR(w, 0x88);
				int32_t left = 0;
				for (int32_t k = U16(st, 4); k > 0; k--, sb++)
					if (U8(sb, 0) == 0) left++;
				per = left / (zz + 1);
				if (per == 0) per = 1;
			}
			else
				per = 0x7FFF;
			S32(w, 0x44) = per;
			S32(WS(), 0x40) = per;
			S32(WS(), 0xF0) = S16(CUR(), 0x96);
			UpdatePolygons_B4C530();
			StepPieces_B4D990();
		}
		uint8_t *blk = PTR(WS(), 0x80);
		h_B27130();
		x::ComputeBonesWorldMatrices(blk + 0x10, WS() + 0xE0);
		U32(blk, 0x44) = U32(CTX(), 0x74);
		DrawModel_B4DE20();
		Dots_B4DB80();
		U16(blk, 0) = (uint16_t)(U16(blk, 0) + 1);
	}

	static VoidFn g_save_vm[0x200], g_save_big[256], g_save_draw[256];

	static void apply_slots(Clone &c)
	{
		ff8fx::copy<(sizeof(g_save_vm))>(g_save_vm, c.vm);
		ff8fx::copy<(sizeof(g_save_big))>(g_save_big, c.bigtab);
		ff8fx::copy<(sizeof(g_save_draw))>(g_save_draw, c.draw);
		apply_shared_misc(c);
		const VoidFn d16 = c.draw[16];
		const VoidFn p16 = c.prim[16];
		const VoidFn v042 = c.vm[0x042];
		ff8fx::copy<(sizeof(g_save_vm))>(c.vm, g_save_vm);
		ff8fx::copy<(sizeof(g_save_big))>(c.bigtab, g_save_big);
		ff8fx::copy<(sizeof(g_save_draw))>(c.draw, g_save_draw);
		c.draw[16] = d16;
		c.prim[16] = p16;
		c.vm[0x042] = v042;
		c.draw[22] = dh_22_EntityDisintegrate;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag018_elvoret_death()
	{
		using namespace gfc;
		elvoret018::describe(elvoret018::g_c);
		init_clone(elvoret018::g_c, nullptr, 0);
		elvoret018::apply_slots(elvoret018::g_c);
		register_port(game::MAG_018_ElvoretDeath_SequenceTick, (void *)elvoret018::SequenceTask, "MAG018 Elvoret Death SequenceTick", 18);
	}
}

