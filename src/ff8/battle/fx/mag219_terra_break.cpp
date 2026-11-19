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
	void apply_eden_c(Clone &c);
	void apply_eden_d(Clone &c);

namespace terra219
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace terra219
{
	static void describe(Clone &c)
	{
		c.name = "219 Terra Break";
		c.effect_id = 219;
		c.vm_table = game_addr(game::funcs_AC9BDC);
		c.bone_table = game_addr(game::funcs_AC6567);
		c.prim_table = game_addr(game::funcs_AC0C34);
		c.prim_size = game_addr(game::unk_186EA20);
		c.spr_table = game_addr(game::unk_186EA34);
		c.ptype_table = game_addr(game::unk_186EA60);
		c.pop_table = game_addr(game::funcs_AC3F27);
		c.draw_table = game_addr(game::unk_186EAD4);
		c.attr_16A = game_addr(game::unk_186EC3E);
		c.attr_184 = game_addr(game::unk_186EC58);
		c.attr_186 = game_addr(game::unk_186EC5A);
		c.attr_194 = game_addr(game::unk_186EC68);
		c.attr_19C = game_addr(game::unk_186EC70);
		c.desc = game_addr(game::unk_186E6D0);
		c.ptr_block = game_addr(game::unk_186E8C0);
		c.queue = game_addr(game::unk_2796C50);
		c.file00 = game_addr(game::unk_2796C64);
		c.file01 = game_addr(game::unk_2796C60);
		c.tick = game_addr(game::MAG_219_sub_ABDA90);
		c.load_cb = game_addr(game::MAG_219_sub_AC5950);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static inline uint8_t *xl_JointMatrix_B66B80(void *hdr, int32_t joint, int32_t scale, void *m)
	{
		return x::f<uint8_t *(__cdecl *)(void *, int32_t, int32_t, void *)>(game_addr(game::GfCinematic_ModelJointMatrix))(hdr, joint, scale, m);
	}
	static inline int32_t xl_Steer_B67A60(const void *p, const void *t, int32_t speed, int32_t turn, int32_t reach, int32_t a, int32_t b)
	{
		return x::f<int32_t (__cdecl *)(const void *, const void *, int32_t, int32_t, int32_t, int32_t, int32_t)>(game_addr(game::GFHelper_SteerPointToward))(p, t, speed, turn, reach, a, b);
	}
	static inline uint8_t *SlotEntity() { return PTR(SCENE(), 0x60 + 4 * (int32_t)U8(CUR(), 0x1B)); }

	static void __cdecl op_0AD_PosFromPosedEntityJoint()
	{
		uint8_t *ent = SlotEntity();
		const bool weapon = (U8(RT(), 0x4B) & 0x80) != 0;
		uint8_t *hdr = weapon ? PTR(ent, 0x78) : ent + 0x60;
		x::BuildBoneMatricesFromPose(hdr);
		uint8_t *m = xl_JointMatrix_B66B80(hdr, S16(STREAM(), 2), 0x1000, ent + 0x40);
		x::SetRotMatrix(m);
		x::SetTransVector(m);
		uint8_t *w = WS();
		x::TransformToCamera(CUR() + 0x94, w + 0xF0, w + 0xFC);
		S32(CUR(), 0x5C) = shl32(S32(WS(), 0xF0), 16);
		S32(CUR(), 0x60) = shl32(S32(WS(), 0xF4), 16);
		S32(CUR(), 0x64) = shl32(S32(WS(), 0xF8), 16);
		C().bone[U8(CUR(), 0x18)]();
		STREAM() += 4;
	}

	static void __cdecl op_12E_ApplyNextTargetResult()
	{
		uint8_t *list = PTR(CTX(), 0xCC);
		const int32_t i = U8(SCENE(), 0x3A);
		if (S8(list, 0x10) > i)
		{
			x::ApplyActionResultToTarget((int32_t)(uint32_t)(PTR(list, 8) + 0x18 * i));
			U8(SCENE(), 0x3A) = (uint8_t)(U8(SCENE(), 0x3A) + 1);
		}
		STREAM() += 2;
	}

	static void __cdecl op_0B3_SteerTowardBone()
	{
		uint8_t *t = blob::GetBone(S16(STREAM(), 2));
		uint8_t *cur = CUR();
		uint8_t *st = cur + 0xA0;
		if (U8(RT(), 0x4B) & 0x80)
		{
			const int16_t x0 = S16(cur, 0x94), y0 = S16(cur, 0x96), z0 = S16(cur, 0x98);
			S16(st, 0) = x0;
			S16(st, 2) = y0;
			S16(st, 4) = z0;
			const int32_t tx = S16(t, 0x94), ty = S16(t, 0x96), tz = S16(t, 0x98);
			S16(st, 6) = (int16_t)blob::Blob_B66B30(x0, z0, tx, tz);
			S16(st, 8) = (int16_t)blob::Blob_B66B30(y0, z0, ty, tz);
			STREAM() += 4;
			return;
		}
		const int32_t r = xl_Steer_B67A60(st, t + 0x94, S16(cur, 0x94), S16(cur, 0x96), S16(cur, 0x98), S16(st, 6), S16(st, 8));
		if (r == 0) U16(CUR(), 0x4A) &= 0xFFFE;
		else U8(CUR(), 0x4A) |= 1;
		uint8_t *ws = WS();
		U16(st, 0) = U16(ws, 0xF0);
		U16(CUR(), 0x94) = U16(ws, 0xF0);
		U16(st, 2) = U16(ws, 0xF2);
		U16(CUR(), 0x96) = U16(ws, 0xF2);
		U16(st, 4) = U16(ws, 0xF4);
		U16(CUR(), 0x98) = U16(ws, 0xF4);
		U16(st, 6) = U16(WS(), 0xE0);
		U16(st, 8) = U16(WS(), 0xE4);
		STREAM() += 4;
	}

	static inline void xl_SetFogNearFar_56CC50(int32_t a, int32_t b, int32_t h) { x::f<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFogNearFar))(a, b, h); }
	static inline int32_t xl_RotTransPersNormalColor_56CAA0(const void *v, const void *n, const void *c, void *sxy, void *rgb, void *flag)
	{
		return x::f<int32_t (__cdecl *)(const void *, const void *, const void *, void *, void *, void *)>(game_addr(game::GTE_RotTransPersNormalColor))(v, n, c, sxy, rgb, flag);
	}

	static void __cdecl dh_67_FoggedGridSheet()
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
			const int32_t e = ((int32_t)U16(CUR(), 0x98) >> 4) & 0xE0;
			S32(ws, 0x70) = add32(sub32((int32_t)~(uint32_t)a, e), 1);
		}
		ws = WS();
		{
			const int32_t a = shl32(S32(ws, 0x64) / 2, 5);
			const int32_t e = ((int32_t)U16(CUR(), 0x94) >> 4) & 0x1E0;
			S32(ws, 0x74) = add32(sub32((int32_t)~(uint32_t)a, e), 1);
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
		int32_t vv = S32(ws, 0x74);
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
					u &= 0x60;
					U16(pk, 0x1A) = U16(WS(), 0x68);
					U16(pk, 0xE) = U16(WS(), 0x6C);
					U8(pk, 0x24) = (uint8_t)u;
					U8(pk, 0xC) = (uint8_t)u;
					u += 0x20;
					const uint8_t u2 = (u & 0x70) ? (uint8_t)u : 0x7F;
					U8(pk, 0x30) = u2;
					U8(pk, 0x18) = u2;
					U8(pk, 0x19) = (uint8_t)vv;
					U8(pk, 0xD) = (uint8_t)vv;
					const uint8_t vb = (v2 & 0xF0) ? (uint8_t)v2 : 0xFF;
					U8(pk, 0x31) = vb;
					U8(pk, 0x25) = vb;
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
			vv += 0x20;
			row0 += 0xC;
			row1 += 0xC;
		}
		PTR(CTX(), 0x7C) = pk;
	}

	static inline int32_t xl_NClip_56CB30(uint32_t s0, uint32_t s1, uint32_t s2) { return x::f<int32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_NormalClip3))(s0, s1, s2); }
	static inline void xl_InsertPrimAutoDepth(uint32_t ot, void *prim) { x::f<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAutoDepth))(ot, prim); }
	static inline uint32_t &TbGte(int reg) { return MEM<uint32_t>(game_addr(game::GTE_VXY0) + 4 * reg); }
	static inline int32_t TbDiv3(int32_t v)
	{
		const int32_t hi = (int32_t)(((int64_t)v * 0x55555556LL) >> 32);
		return hi + (int32_t)((uint32_t)hi >> 31);
	}

	static void Launch_AC2890(uint8_t *e, uint8_t *pool)
	{
		if (!(U8(e, 1) & 1)) return;
		uint8_t *ws = WS();
		x::BuildBoneMatricesFromPose(e + 0x14);
		uint8_t *mats = PTR(e, 4) + 0x10;
		const uint8_t *tab = PTR(e, 8);
		int32_t nobj = S32(tab, 0);
		tab += 4;
		for (; nobj > 0; nobj--)
		{
			const uint8_t *obj = PTR(e, 8) + U32(tab, 0) + 2;
			tab += 4;
			uint8_t *out = PTR(CTX(), 0x74);
			PTR(ws, 0x88) = out;
			for (int32_t g = S16(obj, -2); g > 0; g--)
			{
				uint8_t *m = mats + 0x30 * S16(obj, 0) + 0x10;
				x::SetRotMatrix(m);
				x::SetTransVector(m);
				int32_t nv = S16(obj, 2);
				obj += 4;
				for (; nv > 0; nv--)
				{
					x::GteWriteData((int32_t)(((uint32_t)U16(obj, 2) << 16) | U16(obj, 0)), 0);
					x::GteWriteData((int32_t)U16(obj, 4), 1);
					x::GteMVMVA_RotV0Tr();
					U16(out, 0) = (uint16_t)TbGte(25);
					U16(out, 2) = (uint16_t)TbGte(26);
					U16(out, 4) = (uint16_t)TbGte(27);
					obj += 6;
					out += 8;
				}
			}
			const uint8_t *t = (const uint8_t *)(((uintptr_t)obj + 3) & ~(uintptr_t)3);
			S16(ws, 0xD0) = S16(t, 4);
			S16(ws, 0xD2) = S16(t, 6);
			const uint8_t *p = t + 0xC;
			const uint8_t *vb = PTR(ws, 0x88);
			for (; S16(ws, 0xD0) > 0; S16(ws, 0xD0) = (int16_t)(S16(ws, 0xD0) - 1), pool += 0x2C, p += 0x14)
			{
				if (U8(pool, 6) != 0) continue;
				const uint8_t *v0 = vb + 8 * (U16(p, 0) & 0xFFF), *v1 = vb + 8 * (U16(p, 2) & 0xFFF), *v2 = vb + 8 * (U16(p, 4) & 0xFFF);
				const int32_t cx = TbDiv3(S16(v1, 0) + S16(v0, 0) + S16(v2, 0)) - S32(ws, 0x60);
				const int32_t cy = TbDiv3(S16(v2, 2) + S16(v1, 2) + S16(v0, 2)) - S32(ws, 0x64);
				const int32_t cz = TbDiv3(S16(v1, 4) + S16(v2, 4) + S16(v0, 4)) - S32(ws, 0x68);
				if (add32(add32(mul32(cy, cy), mul32(cx, cx)), mul32(cz, cz)) >= S32(WS(), 0x80)) continue;
				U8(pool, 6) = 1;
				S16(pool, 0) = (int16_t)cx;
				S16(pool, 2) = (int16_t)cy;
				S16(pool, 4) = (int16_t)cz;
				for (int k = 0; k < 3; k++)
				{
					const uint8_t *v = vb + 8 * (U16(p, 2 * k) & 0xFFF);
					S16(pool, 0xC + 8 * k) = (int16_t)(S16(v, 0) - cx);
					S16(pool, 0xE + 8 * k) = (int16_t)(S16(v, 2) - cy);
					S16(pool, 0x10 + 8 * k) = (int16_t)(S16(v, 4) - cz);
				}
				U32(pool, 8) = 0;
				U16(pool, 0x22) = 0;
				U16(pool, 0x1A) = 0;
				U16(pool, 0x12) = 0;
				const int32_t base = S32(ws, 0x90), range = S32(ws, 0x94);
				U8(pool, 0x13) = (uint8_t)(blob::Rand73(range) + base);
				U8(pool, 0x1B) = (uint8_t)(blob::Rand73(range) + base);
				U8(pool, 0x23) = (uint8_t)(blob::Rand73(range) + base);
				U8(pool, 9) = (uint8_t)(add32(S32(ws, 0x70), blob::Rand73(S32(ws, 0x74))) >> 4);
			}
			if (S16(ws, 0xD2) > 0)
			{
				p += 4;
				for (; S16(ws, 0xD2) > 0; S16(ws, 0xD2) = (int16_t)(S16(ws, 0xD2) - 1), pool += 0x2C, p += 0x18)
				{
					if (U8(pool, 6) != 0) continue;
					const uint8_t *v0 = vb + 8 * (U16(p, -4) & 0xFFF), *v1 = vb + 8 * (U16(p, -2) & 0xFFF);
					const uint8_t *v2 = vb + 8 * (U16(p, 0) & 0xFFF), *v3 = vb + 8 * (U16(p, 2) & 0xFFF);
					const int32_t cx = (S16(v1, 0) + S16(v0, 0) + S16(v2, 0) + S16(v3, 0)) / 4 - S32(ws, 0x60);
					const int32_t cy = (S16(v3, 2) + S16(v2, 2) + S16(v1, 2) + S16(v0, 2)) / 4 - S32(ws, 0x64);
					const int32_t cz = (S16(v3, 4) + S16(v2, 4) + S16(v1, 4) + S16(v0, 4)) / 4 - S32(ws, 0x68);
					if (add32(add32(mul32(cy, cy), mul32(cx, cx)), mul32(cz, cz)) >= S32(WS(), 0x80)) continue;
					S16(pool, 4) = (int16_t)cz;
					U8(pool, 6) = 1;
					S16(pool, 0) = (int16_t)cx;
					S16(pool, 2) = (int16_t)cy;
					for (int k = 0; k < 4; k++)
					{
						const uint8_t *v = vb + 8 * (U16(p, 2 * k - 4) & 0xFFF);
						S16(pool, 0xC + 8 * k) = (int16_t)(S16(v, 0) - cx);
						S16(pool, 0xE + 8 * k) = (int16_t)(S16(v, 2) - cy);
						S16(pool, 0x10 + 8 * k) = (int16_t)(S16(v, 4) - cz);
					}
					U32(pool, 8) = 0;
					U16(pool, 0x22) = 0;
					U16(pool, 0x1A) = 0;
					U16(pool, 0x12) = 0;
					const int32_t base = S32(ws, 0x90), range = S32(ws, 0x94);
					U8(pool, 0x13) = (uint8_t)(blob::Rand73(range) + base);
					U8(pool, 0x1B) = (uint8_t)(blob::Rand73(range) + base);
					U8(pool, 0x23) = (uint8_t)(blob::Rand73(range) + base);
					U8(pool, 9) = (uint8_t)(add32(S32(ws, 0x70), blob::Rand73(S32(ws, 0x74))) >> 4);
				}
			}
		}
	}

	static void ShardStep_AC3460(uint8_t *s)
	{
		uint8_t *ws = WS();
		if (U8(RT(), 0x45) == 0)
		{
			U8(s, 7) = (uint8_t)(U8(s, 7) - 1);
			S16(s, 2) = (int16_t)(S16(s, 2) + (int16_t)(S8(s, 9) << 4));
			U8(s, 0x12) = (uint8_t)(U8(s, 0x12) + U8(s, 0x13));
			U8(s, 0x1A) = (uint8_t)(U8(s, 0x1A) + U8(s, 0x1B));
			U8(s, 0x22) = (uint8_t)(U8(s, 0x22) + U8(s, 0x23));
		}
		int16_t ang[3] = { (int16_t)(S8(s, 0x12) << 4), (int16_t)(S8(s, 0x1A) << 4), (int16_t)(S8(s, 0x22) << 4) };
		int16_t pos[3] = { S16(s, 0), S16(s, 2), S16(s, 4) };
		S16(ws, 0xC0) = ang[0];
		S16(ws, 0xC2) = ang[1];
		S16(ws, 0xC4) = ang[2];
		x::RotMatrixFromAngles(ws + 0xC0, ws + 0xE0);
		S32(ws, 0xE0 + 0x14) = pos[0];
		S32(ws, 0xE0 + 0x18) = pos[1];
		S32(ws, 0xE0 + 0x1C) = pos[2];
		x::ComposeAffine(PTR(ws, 0x84), ws + 0xE0, ws + 0xE0);
		x::SetRotMatrix(ws + 0xE0);
		x::SetTransVector(ws + 0xE0);
	}

	static uint8_t *Draw_AC2EA0(uint8_t *e, uint8_t *pool)
	{
		if (!(U8(e, 1) & 1)) return nullptr;
		uint8_t *ws = WS();
		x::BuildBoneMatricesFromPose(e + 0x14);
		Mat4x3 *par = blob::GetParentMatrix(0);
		PTR(WS(), 0x84) = (uint8_t *)par;
		x::ComputeBonesWorldMatrices(e + 0x14, par);
		x::GteSetBackColor3(U8(e, 0x28), U8(e, 0x29), U8(e, 0x2A));
		uint8_t *mats = PTR(e, 4) + 0x10;
		const uint8_t *tab = PTR(e, 8);
		int32_t nobj = S32(tab, 0);
		tab += 4;
		for (; nobj > 0; nobj--)
		{
			const uint8_t *obj = PTR(e, 8) + U32(tab, 0) + 2;
			tab += 4;
			uint8_t *out = PTR(CTX(), 0x74);
			PTR(ws, 0x88) = out;
			for (int32_t g = S16(obj, -2); g > 0; g--)
			{
				uint8_t *m = mats + 0x30 * S16(obj, 0) + 0x10;
				x::SetRotMatrix(m);
				x::SetTransVector(m);
				int32_t nv = S16(obj, 2);
				obj += 4;
				for (; nv > 0; nv--)
				{
					x::GteWriteData((int32_t)(((uint32_t)U16(obj, 2) << 16) | U16(obj, 0)), 0);
					x::GteWriteData((int32_t)U16(obj, 4), 1);
					x::GteRTPS();
					x::GteReadSXY2(out);
					U16(out, 4) = (uint16_t)TbGte(19);
					obj += 6;
					out += 8;
				}
			}
			const uint8_t *t = (const uint8_t *)(((uintptr_t)obj + 3) & ~(uintptr_t)3);
			const uint8_t *rec = t + 0xC;
			S16(ws, 0xD0) = S16(t, 4);
			S16(ws, 0xD2) = S16(t, 6);
			uint8_t *pk = PTR(CTX(), 0x7C);
			const uint8_t *vb = PTR(ws, 0x88);
			if (S16(ws, 0xD0) > 0)
			{
				for (; S16(ws, 0xD0) > 0; S16(ws, 0xD0) = (int16_t)(S16(ws, 0xD0) - 1), rec += 0x14, pool += 0x2C)
				{
					U8(pk, 3) = 7;
					U16(pk, 0x16) = U16(rec, 0xE);
					U16(pk, 0xE) = U16(rec, 0xA);
					U16(pk, 0xC) = U16(rec, 8);
					U16(pk, 0x14) = U16(rec, 0xC);
					U16(pk, 0x1C) = U16(rec, 6);
					if (U8(pool, 6) == 0)
					{
						U32(pk, 4) = U32(rec, 0x10);
						const uint8_t *p0 = vb + 8 * (U16(rec, 0) & 0xFFF), *p1 = vb + 8 * (U16(rec, 2) & 0xFFF), *p2 = vb + 8 * (U16(rec, 4) & 0xFFF);
						const uint32_t s0 = U32(p0, 0), s1 = U32(p1, 0), s2 = U32(p2, 0);
						if (xl_NClip_56CB30(s0, s1, s2) <= 0) continue;
						U32(pk, 8) = s0;
						U32(pk, 0x10) = s1;
						U32(pk, 0x18) = s2;
						const int32_t z = TbDiv3((int32_t)U16(p2, 4) + U16(p1, 4) + U16(p0, 4));
						xl_InsertPrimAutoDepth(((uint32_t)(z >> 2) & 0xFFFFFFFCu) + U32(RT(), 0x4C), pk);
					}
					else
					{
						int8_t life = S8(pool, 7);
						if (life < 0) continue;
						U32(pk, 4) = AT<uint32_t>(PTR(WS(), 0x98), 4 * life) | 0x26000000;
						U8(pk, 0x16) |= 0x20;
						ShardStep_AC3460(pool);
						x::GteLoadV012(pool + 0xC, pool + 0x14, pool + 0x1C);
						x::GteRTPT();
						x::GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
						const uint16_t sz = (uint16_t)TbGte(19);
						U16(ws, 0xFC) = sz;
						xl_InsertPrimAutoDepth((((uint32_t)sz >> 2) & 0xFFFFFFFCu) + U32(RT(), 0x4C), pk);
					}
					pk += 0x20;
				}
			}
			if (S16(ws, 0xD2) > 0)
			{
				rec += 0xA;
				for (; S16(ws, 0xD2) > 0; S16(ws, 0xD2) = (int16_t)(S16(ws, 0xD2) - 1), rec += 0x18, pool += 0x2C)
				{
					U8(pk, 3) = 9;
					U16(pk, 0x16) = U16(rec, 4);
					U16(pk, 0xE) = U16(rec, 0);
					U16(pk, 0xC) = U16(rec, -2);
					U16(pk, 0x14) = U16(rec, 2);
					U16(pk, 0x1C) = U16(rec, 6);
					U16(pk, 0x24) = U16(rec, 8);
					if (U8(pool, 6) == 0)
					{
						U32(pk, 4) = U32(rec, 0xA);
						const uint8_t *p0 = vb + 8 * (U16(rec, -0xA) & 0xFFF), *p1 = vb + 8 * (U16(rec, -8) & 0xFFF);
						const uint8_t *p2 = vb + 8 * (U16(rec, -6) & 0xFFF), *p3 = vb + 8 * (U16(rec, -4) & 0xFFF);
						const uint32_t s0 = U32(p0, 0), s1 = U32(p1, 0), s2 = U32(p2, 0);
						if (xl_NClip_56CB30(s0, s1, s2) <= 0) continue;
						U32(pk, 8) = s0;
						U32(pk, 0x10) = s1;
						U32(pk, 0x18) = s2;
						U32(pk, 0x20) = U32(p3, 0);
						const int32_t z = ((int32_t)S16(p3, 4) + U16(p2, 4) + U16(p1, 4) + U16(p0, 4)) / 4;
						xl_InsertPrimAutoDepth(((uint32_t)(z >> 2) & 0xFFFFFFFCu) + U32(RT(), 0x4C), pk);
					}
					else
					{
						int8_t life = S8(pool, 7);
						if (life < 0) continue;
						U8(pk, 0x16) |= 0x20;
						U32(pk, 4) = AT<uint32_t>(PTR(WS(), 0x98), 4 * life) | 0x2E000000;
						ShardStep_AC3460(pool);
						x::GteLoadV0(pool + 0x24);
						x::GteRTPS();
						x::GteReadSXY2(pk + 0x20);
						x::GteLoadV012(pool + 0xC, pool + 0x14, pool + 0x1C);
						x::GteRTPT();
						x::GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
						const uint16_t sz = (uint16_t)TbGte(19);
						U16(ws, 0xFC) = sz;
						xl_InsertPrimAutoDepth((((uint32_t)sz >> 2) & 0xFFFFFFFCu) + U32(RT(), 0x4C), pk);
					}
					pk += 0x28;
				}
			}
			PTR(CTX(), 0x7C) = pk;
		}
		return pool;
	}

	static void __cdecl dh_49_StageShatter()
	{
		if (U32(CUR(), 0xBC) == 0)
		{
			const uint8_t *d = PTR(CUR(), 0xB8);
			PTR(CUR(), 0xBC) = blob::ArenaAlloc(0xB000);
			const uint16_t life = (uint16_t)shl32(S16(d, 2), 8);
			uint8_t *p = PTR(CUR(), 0xBC) + 6;
			for (int i = 0; i < 0x400; i++, p += 0x2C)
			{
				U16(p, 0) = life;
				MEM<uint8_t>(game_addr(game::unk_1D98991)) &= 0xFD;
				MEM<uint8_t>(game_addr(game::unk_1D989BD)) &= 0xFD;
				MEM<uint8_t>(game_addr(game::unk_1D989E9)) &= 0xFD;
				MEM<uint8_t>(game_addr(game::unk_1D98A15)) |= 2;
			}
		}
		const uint8_t *d = PTR(CUR(), 0xB8);
		uint8_t *ref = blob::GetBone(S16(d, 4));
		PTR(WS(), 0x98) = (uint8_t *)d + S16(d, 0);
		S32(WS(), 0x9C) = S16(d, 2);
		S32(WS(), 0x60) = S16(CUR(), 0x94);
		S32(WS(), 0x64) = S16(CUR(), 0x96);
		S32(WS(), 0x68) = S16(CUR(), 0x98);
		S32(WS(), 0x6C) = S16(CUR(), 0x8C);
		S32(WS(), 0x80) = mul32(S32(WS(), 0x6C), S32(WS(), 0x6C));
		S32(WS(), 0x70) = S16(ref, 0x96);
		S32(WS(), 0x74) = S16(ref, 0x8E);
		S32(WS(), 0x90) = S16(ref, 0x98);
		S32(WS(), 0x94) = S16(ref, 0x90);
		uint8_t *pool = PTR(CUR(), 0xBC);
		static uint8_t *const ENT[3] = { (uint8_t *)game_addr(game::unk_1D98990), (uint8_t *)game_addr(game::unk_1D989BC), (uint8_t *)game_addr(game::unk_1D989E8) };
		for (int k = 0; k < 3; k++)
		{
			Launch_AC2890(ENT[k], pool);
			pool = Draw_AC2EA0(ENT[k], pool);
		}
	}

	static VoidFn g_save_vm[0x200], g_save_big[256], g_save_draw[256];

	static void apply_slots(Clone &c)
	{
		ff8fx::copy<(sizeof(g_save_vm))>(g_save_vm, c.vm);
		ff8fx::copy<(sizeof(g_save_big))>(g_save_big, c.bigtab);
		ff8fx::copy<(sizeof(g_save_draw))>(g_save_draw, c.draw);
		apply_shared_misc(c);
		const VoidFn d16 = c.draw[16];
		apply_eden_c(c);
		const VoidFn d58 = c.draw[58];
		apply_eden_d(c);
		const VoidFn p6 = c.prim[6];
		ff8fx::copy<(sizeof(g_save_vm))>(c.vm, g_save_vm);
		ff8fx::copy<(sizeof(g_save_big))>(c.bigtab, g_save_big);
		ff8fx::copy<(sizeof(g_save_draw))>(c.draw, g_save_draw);
		c.draw[16] = d16;
		c.draw[58] = d58;
		c.prim[6] = p6;
		apply_shared_ribbon(c);
		c.vm[0x0AD] = op_0AD_PosFromPosedEntityJoint;
		c.vm[0x0B3] = op_0B3_SteerTowardBone;
		c.vm[0x12E] = op_12E_ApplyNextTargetResult;
		c.draw[49] = dh_49_StageShatter;
		c.draw[67] = dh_67_FoggedGridSheet;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag219_terra_break()
	{
		using namespace gfc;
		terra219::describe(terra219::g_c);
		init_clone(terra219::g_c, nullptr, 0);
		terra219::apply_slots(terra219::g_c);
		register_port(game::MAG_219_sub_ABDA90, (void *)terra219::SequenceTask, "MAG219 Terra Break SequenceTick", 219);
	}
}

