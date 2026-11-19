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

// Effect 231: Disease Breath

#include "gfc_engine.h"

namespace ff8fx
{
namespace gfc
{
namespace disease231
{
	static Clone g_c = {};

	static void describe(Clone &c)
	{
		c.name = "231 Disease Breath";
		c.effect_id = 231;
		c.vm_table = game_addr(game::GF_231DiseaseBreath_VmOpcodeTable);
		c.bone_table = game_addr(game::GF_231DiseaseBreath_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_A69594);
		c.prim_size = game_addr(game::unk_186989C);
		c.spr_table = game_addr(game::unk_18698B0);
		c.ptype_table = game_addr(game::unk_18698DC);
		c.pop_table = game_addr(game::funcs_A6ABF7);
		c.draw_table = game_addr(game::GF_231DiseaseBreath_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_1869ABA);
		c.attr_184 = game_addr(game::unk_1869AD4);
		c.attr_186 = game_addr(game::unk_1869AD6);
		c.attr_194 = game_addr(game::unk_1869AE4);
		c.attr_19C = game_addr(game::unk_1869AEC);
		c.desc = game_addr(game::unk_1869650);
		c.ptr_block = game_addr(game::unk_186973C);
		c.queue = game_addr(game::unk_2796A90);
		c.file00 = game_addr(game::g_MAG231_ScriptFile);
		c.file01 = game_addr(game::g_MAG231_CompanionFile);
		c.tick = game_addr(game::GF_231DiseaseBreath_SequenceTick);
		c.load_cb = game_addr(game::GF_231DiseaseBreath_LoadDoneCallback);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

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

	static void LightColourSetup(int32_t c)
	{
		U32(WS(), 0xE0) = 0x1000;
		U32(WS(), 0xE4) = 0x10000000;
		U32(WS(), 0xE8) = 0;
		U32(WS(), 0xEC) = 0x1000;
		U32(WS(), 0xF0) = 0;
		x::SetLightMatrix(WS() + 0xE0);
		U32(WS(), 0xE0) = (uint32_t)(c & 0xFF) << 5;
		U32(WS(), 0xE4) = (uint32_t)(c & 0xFF00) << 13;
		U32(WS(), 0xEC) = (uint32_t)((c >> 11) & 0x1FE0);
		x::SetColorMatrix(WS() + 0xE0);
		x::SetBackColor(0, 0, 0);
		U32(WS(), 0xE0) = 0x10001000;
		U32(WS(), 0xE4) = 0x1000;
		x::GteLoadV0(WS() + 0xE0);
	}

	static inline int32_t DepthKey(uint32_t off)
	{
		uint32_t v = DEPTHARR()[off >> 3] & 0xFFFF;
		return (int32_t)(int64_t)((double)v * (double)FLT_1877DA8());
	}

	inline uint32_t &GteData(int reg) { return MEM<uint32_t>(game_addr(game::GTE_VXY0) + 4 * reg); }

	static void __cdecl pr_58_TintedFT4Abr()
	{
		LightColourSetup((int32_t)U32(WS(), 0x84));
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

	static inline int32_t xl_NClip_56CB30(uint32_t s0, uint32_t s1, uint32_t s2) { return x::f<int32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_NormalClip3))(s0, s1, s2); }
	static inline void xl_ProjectVertexGroup_50FD40(uint8_t **stream, uint8_t **out) { x::f<void (__cdecl *)(uint8_t **, uint8_t **)>(game_addr(game::BattleModel_ProjectVerticesWithGTE))(stream, out); }
	static inline void xl_EntityBounds_5086F0(uint8_t *ent) { x::f<void (__cdecl *)(uint8_t *)>(game_addr(game::BattleEntity_ComputeModelBounds))(ent); }
	static inline int32_t xl_ISqrt(int32_t v) { return x::f<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	static inline uint32_t xl_Render_5088A0(void *ent, uint32_t ot, int32_t mode, uint32_t cursor) { return x::Render_5088A0(ent, ot, mode, cursor); }

	static void SyncEntityModelCopy(uint8_t *blk)
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

	static uint8_t *CreateEntityModelCopy(uint8_t *ent)
	{
		uint8_t *blk = blob::ArenaAlloc(0x80);
		PTR(blk, 0xC) = ent;
		SyncEntityModelCopy(blk);
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

	static int32_t DrawGlowPrims(uint8_t *blk, uint8_t *dist, int32_t k)
	{
		uint8_t *p = PTR(CTX(), 0x7C);
		int32_t off = S16(dist, 0x10 + 2 * k);
		uint8_t *rec = PTR(blk, 0x40);
		uint8_t *v = PTR(blk, 0x44);
		uint8_t *col = PTR(CTX(), 0x74) + off * 2;
		int32_t n = U16(blk, 0x48);
		for (; n > 0; n--)
		{
			uint32_t i0 = (U16(rec, 0) & 0xFFF) << 3;
			uint32_t i1 = (U16(rec, 2) & 0xFFF) << 3;
			uint32_t i2 = (U16(rec, 4) & 0xFFF) << 3;
			uint32_t s0 = U32(v, i0), s1 = U32(v, i1), s2 = U32(v, i2);
			if (xl_NClip_56CB30(s0, s1, s2) >= 0)
			{
				U32(p, 8) = s0;
				U32(p, 0x14) = s1;
				U32(p, 0x20) = s2;
				int32_t z0 = U16(v, i0 + 4);
				int32_t z1 = U16(v, i1 + 4);
				int32_t z2 = U16(v, i2 + 4);
				int32_t sum = z2 + z1 + z0;
				U32(p, 4) = U32(col, (i0 >> 3) * 4);
				U32(p, 0x10) = U32(col, (i1 >> 3) * 4);
				U8(p, 7) = 0x34;
				U32(p, 0x1C) = U32(col, (i2 >> 3) * 4);
				U8(p, 3) = 9;
				U16(p, 0x1A) = U16(rec, 0xE);
				U16(p, 0xE) = U16(rec, 0xA);
				U16(p, 0xC) = U16(rec, 8);
				U16(p, 0x18) = U16(rec, 0xC);
				U16(p, 0x24) = U16(rec, 6);
				x::InsertPrimDepthKeys(U32(RT(), 0x4C) + ((sum / 3 >> 2) & ~3), p, z0, z1, z2, 0);
				p += 0x28;
			}
			rec += 0x10;
		}
		n = U16(blk, 0x4A);
		if (n > 0)
		{
			rec += 4;
			for (; n > 0; n--)
			{
				uint32_t i1 = (U16(rec, -2) & 0xFFF) << 3;
				uint32_t i2 = (U16(rec, 0) & 0xFFF) << 3;
				uint32_t i0 = (U16(rec, -4) & 0xFFF) << 3;
				uint32_t i3 = (U16(rec, 2) & 0xFFF) << 3;
				uint32_t s1 = U32(v, i1), s2 = U32(v, i2), s0 = U32(v, i0), s3 = U32(v, i3);
				if (xl_NClip_56CB30(s0, s1, s2) >= 0)
				{
					U32(p, 8) = s0;
					U32(p, 0x14) = s1;
					U32(p, 0x20) = s2;
					U32(p, 0x2C) = s3;
					int32_t z0 = U16(v, i0 + 4);
					int32_t z1 = U16(v, i1 + 4);
					int32_t z2 = U16(v, i2 + 4);
					int32_t z3 = U16(v, i3 + 4);
					int32_t sum = z3 + z2 + z1 + z0;
					U32(p, 4) = U32(col, (i0 >> 3) * 4);
					U32(p, 0x10) = U32(col, (i1 >> 3) * 4);
					U32(p, 0x1C) = U32(col, (i2 >> 3) * 4);
					U8(p, 7) = 0x3C;
					U32(p, 0x28) = U32(col, (i3 >> 3) * 4);
					U8(p, 3) = 0xC;
					U16(p, 0x1A) = U16(rec, 0xA);
					U16(p, 0xE) = U16(rec, 6);
					U16(p, 0xC) = U16(rec, 4);
					U16(p, 0x18) = U16(rec, 8);
					U16(p, 0x24) = U16(rec, 0xC);
					U16(p, 0x30) = U16(rec, 0xE);
					x::InsertPrimDepthKeys(U32(RT(), 0x4C) + ((sum / 4 >> 2) & ~3), p, z0, z1, z2, z3);
					p += 0x34;
				}
				rec += 0x14;
			}
		}
		U32(CTX(), 0x7C) = (uint32_t)p;
		return 0;
	}

	static inline int32_t clamp255(int32_t v)
	{
		if (v >= 0x100) return 0xFF;
		if (v < 0) return 0;
		return v;
	}

	static inline uint8_t glow_channel(uint8_t ramp, int32_t f, int32_t base)
	{
		int32_t v = mul32((int32_t)ramp, f);
		v = (v < 0 ? v + 0xFFF : v) >> 12;
		v += base;
		if (v > 0xFF) v = 0xFF;
		return (uint8_t)v;
	}

	static void __cdecl dh_32_EntityGlow()
	{
		uint8_t *bone = CUR();
		uint8_t *ent = PTR(SCENE(), 0x60 + 4 * (int32_t)U8(bone, 0x1B));
		uint8_t *model = PTR(ent, 0x64);
		S32(WS(), 0x60) = S16(bone, 0x94);
		S32(WS(), 0x64) = S16(CUR(), 0x96);
		S32(WS(), 0x68) = S16(CUR(), 0x98);
		if (U32(CUR(), 0xBC) == 0)
		{
			U8(ent, 0) |= 4;
			PTR(CUR(), 0xBC) = CreateEntityModelCopy(ent);
			uint8_t *dist = PTR(CTX(), 0x74);
			PTR(CUR(), 0xC0) = dist;
			PTR(dist, 4) = dist + 0x200;
			uint8_t *hdr = PTR(CUR(), 0xBC) + 0x10;
			x::BuildBoneMatricesFromPose(hdr);
			x::ComputeBonesWorldMatrices(hdr, ent + 0x40);
			uint8_t *out = PTR(dist, 4);
			int32_t groups = 0;
			uint8_t *mats = PTR(model, 0) + 0x10;
			uint8_t *tab = PTR(model, 4);
			int32_t nobj = S32(tab, 0);
			tab += 4;
			for (int32_t o = nobj; o > 0; o--)
			{
				uint8_t *s = PTR(model, 4) + U32(tab, 0);
				tab += 4;
				int32_t ng = S16(s, 0);
				s += 2;
				if (ng > 0)
				{
					uint8_t *gt = dist + 2 * groups + 0x10;
					groups += ng;
					for (int32_t g = ng; g > 0; g--)
					{
						int32_t id = S16(s, 0);
						s += 2;
						uint8_t *m = mats + id * 48 + 0x10;
						x::SetRotMatrix(m);
						x::SetTransVector(m);
						U16(gt, 0) = (uint16_t)((uint16_t)(uint32_t)out - U16(dist, 4));
						gt += 2;
						int32_t nv = S16(s, 0);
						s += 2;
						for (; nv > 0; nv--)
						{
							U16(WS(), 0xD0) = U16(s, 0);
							U16(WS(), 0xD2) = U16(s, 2);
							U16(WS(), 0xD4) = U16(s, 4);
							uint8_t *w = WS();
							x::TransformToCamera(w + 0xD0, w + 0xF0, w + 0xFC);
							w = WS();
							int32_t dx = sub32(S32(w, 0xF0), S32(w, 0x60));
							int32_t dy = sub32(S32(w, 0xF4), S32(w, 0x64));
							int32_t r = xl_ISqrt(add32(mul32(dy, dy), mul32(dx, dx)));
							w = WS();
							int32_t dz = sub32(S32(w, 0xF8), S32(w, 0x68));
							U16(out, 0) = (uint16_t)xl_ISqrt(add32(mul32(dz, dz), mul32(r, r)));
							out += 2;
							s += 6;
						}
					}
				}
			}
			int32_t n = (int32_t)((uint32_t)out - U32(dist, 4));
			out = (uint8_t *)(((uint32_t)out + 4) & ~3u);
			U16(dist, 0) = (uint16_t)(n / 2);
			PTR(dist, 0xC) = out;
			uint8_t *prm = PTR(CUR(), 0xB8);
			U16(dist, 8) = U16(prm, 2);
			blob::ReadClut_B66560(S16(prm, 0));
			uint8_t *clut = PTR(WS(), 0xFC);
			for (int32_t k = 0; k < 0x200; k += 2)
			{
				int32_t c = S16(clut, k);
				out[0] = (uint8_t)(c << 3);
				out[1] = (uint8_t)((c >> 2) & 0xF8);
				out[2] = (uint8_t)((c >> 7) & 0xF8);
				out += 4;
			}
			blob::ArenaAlloc((int32_t)((uint32_t)out - U32(CUR(), 0xC0)));
		}
		uint8_t *dist = PTR(CUR(), 0xC0);
		uint8_t *ref = blob::GetBone(S16(dist, 8));
		int32_t inten = S16(CUR(), 0x8E);
		int32_t radius = S16(CUR(), 0x8C);
		int32_t phase = S16(CUR(), 0x90);
		uint32_t ecol = U32(ent, 0x28);
		int32_t br = clamp255(S16(ref, 0x94) + (int32_t)(ecol & 0xFF));
		int32_t bg = clamp255(S16(ref, 0x96) + (int32_t)((ecol >> 8) & 0xFF));
		int32_t bb = clamp255(S16(ref, 0x98) + (int32_t)((ecol >> 16) & 0xFF));
		PTR(WS(), 0x70) = PTR(CTX(), 0x74);
		uint8_t *o = PTR(WS(), 0x70);
		uint8_t *dp = PTR(dist, 4);
		uint8_t *ramp = PTR(dist, 0xC);
		for (int32_t n = S16(dist, 0); n > 0; n--)
		{
			int32_t d = S16(dp, 0);
			if (d >= radius) U32(o, 0) = 0;
			else
			{
				int32_t idx = ((d << 8) / radius + phase) & 0xFF;
				uint8_t *e = ramp + idx * 4;
				int32_t t = (d << 12) / radius;
				int32_t f = mul32(0x1000 - t, inten);
				f = (f < 0 ? f + 0xFF : f) >> 8;
				o[0] = glow_channel(e[0], f, br);
				o[1] = glow_channel(e[1], f, bg);
				o[2] = glow_channel(e[2], f, bb);
			}
			dp += 2;
			o += 4;
		}
		uint8_t *blk = PTR(CUR(), 0xBC);
		U32(blk, 0x44) = U32(CTX(), 0x74) + 4 * (int32_t)S16(dist, 0);
		SyncEntityModelCopy(PTR(CUR(), 0xBC));
		uint8_t *hdr = blk + 0x10;
		x::BuildBoneMatricesFromPose(hdr);
		xl_EntityBounds_5086F0(ent);
		x::ComposeAffine(NODEMAT(), ent + 0x40, WS() + 0xE0);
		x::ComputeBonesWorldMatrices(hdr, WS() + 0xE0);
		uint8_t *mats = PTR(blk, 0x30) + 0x10;
		uint8_t *tab = PTR(blk, 0x34);
		int32_t nobj = S32(tab, 0);
		x::GteSetBackColor3(U8(blk, 0x5C), U8(blk, 0x5D), U8(blk, 0x5E));
		tab += 4;
		int32_t drawn = 0;
		for (int32_t o2 = 0; o2 < nobj; o2++)
		{
			uint8_t *s = PTR(blk, 0x34) + U32(tab, 0);
			uint32_t bit = 1u << (o2 & 31);
			tab += 4;
			if (!(U32(blk, 0x60) & bit)) continue;
			uint8_t *vo = PTR(blk, 0x44);
			int32_t ng = S16(s, 0);
			s += 2;
			for (; ng > 0; ng--)
			{
				int32_t id = S16(s, 0);
				s += 2;
				uint8_t *m = mats + id * 48 + 0x10;
				x::SetRotMatrix(m);
				x::SetTransVector(m);
				xl_ProjectVertexGroup_50FD40(&s, &vo);
			}
			s = (uint8_t *)(((uint32_t)s + 3) & ~3u);
			U16(blk, 0x48) = U16(s, 0);
			s += 2;
			U16(blk, 0x4A) = U16(s, 0);
			s += 2;
			U16(blk, 0x4C) = U16(s, 0);
			s += 2;
			U16(blk, 0x4E) = U16(s, 0);
			s += 6;
			PTR(blk, 0x40) = s;
			DrawGlowPrims(PTR(CUR(), 0xBC), PTR(CUR(), 0xC0), drawn++);
		}
		U32(CTX(), 0x7C) = xl_Render_5088A0(ent, FRAMERL() + 0x4064, 0x1F, U32(CTX(), 0x7C));
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag231_disease_breath()
	{
		using namespace gfc;
		disease231::describe(disease231::g_c);
		init_clone(disease231::g_c, nullptr, 0);
		disease231::g_c.bone[4] = disease231::bh_04_OrbitBetweenBones;
		disease231::g_c.prim[58] = disease231::pr_58_TintedFT4Abr;
		disease231::g_c.draw[32] = disease231::dh_32_EntityGlow;
		register_port(game::GF_231DiseaseBreath_SequenceTick, (void *)disease231::SequenceTask, "MAG231 Disease Breath SequenceTick", 231);
	}
}

