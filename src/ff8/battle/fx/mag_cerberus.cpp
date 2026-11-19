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

// Effect 203: Cerberus

#include "gfc_engine.h"

namespace ff8fx
{
namespace gfc
{
namespace part_cerberus
{
	static inline void xl_Nop_B65150() { blob::f<int32_t (__cdecl *)()>(game_addr(game::xorEAX_6))(); }

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

	static inline int32_t Nclip(uint32_t s0, uint32_t s1, uint32_t s2)
	{
		int32_t mac0;
		x::GteWriteData((int32_t)s0, 12);
		x::GteWriteData((int32_t)s1, 13);
		x::GteWriteData((int32_t)s2, 14);
		x::GteNCLIP();
		x::GteReadData(&mac0, 24);
		return mac0;
	}

	static void __cdecl dh_45_ScreenMelt()
	{
		uint8_t *cur = CUR();
		if (PTR(cur, 0xBC) == nullptr)
		{
			uint8_t *prm = PTR(cur, 0xB8);
			uint32_t t = U16(prm, 0);
			uint32_t d = U16(prm, 2);
			t &= 0xFC0;
			d &= 0x100;
			t |= 0x4000;
			t >>= 2;
			t |= d;
			t >>= 4;
			U32(cur, 0xC0) = t;
			uint8_t *blk = blob::ArenaAlloc(0x2810);
			PTR(CUR(), 0xBC) = blk;
			uint8_t *col = PTR(CUR(), 0xBC);
			U16(col, 0) = 0;
			int32_t angStep = U16(prm, 0xA);
			int32_t spdStep = U16(prm, 6);
			int32_t angle = U16(prm, 8);
			int32_t rndSpd = U16(prm, 0xC);
			int32_t spd = U16(prm, 4);
			int32_t accBase = U16(prm, 0xE);
			int32_t rndAcc = U16(prm, 0x10);
			int32_t spdOfs = U16(prm, 0x12);
			col += 0x10;
			int32_t n = 0x140;
			do
			{
				U16(col, 0) = 0;
				int32_t s = x::Sin(angle);
				int32_t w = mul32(s, spd) >> 12;
				int32_t r = blob::Rand73(rndSpd);
				r = add32(r, spdOfs);
				r = add32(r, spd);
				r = add32(r, w);
				U16(col, 4) = (uint16_t)r;
				int32_t r2 = blob::Rand73(rndAcc);
				r2 = add32(r2, accBase);
				U16(col, 6) = (uint16_t)r2;
				angle = add32(angle, angStep);
				U32(col, 0x10) = 0;
				U32(col, 0xC) = 0;
				U32(col, 8) = 0xE000;
				spd = add32(spd, spdStep);
				col += 0x20;
			} while (--n != 0);
		}
		xl_Nop_B65150();
		cur = CUR();
		uint8_t *blk = PTR(cur, 0xBC);
		U32(WS(), 0x60) = (uint32_t)U16(PTR(cur, 0xB8), 0x14) << 8;
		U32(WS(), 0x64) = U32(CUR(), 0xC0);
		U32(WS(), 0x68) = ((uint32_t)U8(CUR(), 0x92) & 0x20) | 0xE1000040;
		int32_t fade = S16(blk, 0);
		if (fade >= 0xF) fade = 0xF;
		uint32_t e8 = (uint32_t)fade * 8;
		uint32_t a = 0x80u - e8;
		uint32_t c = e8 + 0x80u;
		uint32_t rgb = a | 0x3C00;
		rgb <<= 8;
		rgb |= a;
		rgb <<= 8;
		rgb |= c;
		uint8_t *col = blk + 0x10;
		U32(WS(), 0x70) = rgb;
		U32(WS(), 0x74) = 0x808080;
		uint8_t *pkt = PTR(CTX(), 0x7C);
		int32_t strip = 0;
		int32_t base = 0;
		do
		{
			int32_t i = 0;
			do
			{
				if (U8(RT(), 0x45) == 0)
				{
					U16(col, 4) = (uint16_t)(U16(col, 4) + U16(col, 6));
					int32_t y = add32(S32(col, 8), S16(col, 4));
					S32(col, 8) = y;
					if (y >= S32(WS(), 0x60))
					{
						U16(col, 0) = 1;
						U16(col, 0xC) = U16(col, 4);
						if (y >= add32(S32(WS(), 0x60), 0x10000)) U16(col, 0) |= 0x8000;
					}
					U16(col, 0xC) = (uint16_t)(U16(col, 0xC) + U16(col, 0xE));
					S32(col, 0x10) = add32(S32(col, 0x10), S16(col, 0xC));
				}
				if (!(U8(col, 1) & 0x80))
				{
					U8(pkt, 3) = 0xC;
					U16(pkt, 0x1A) = (uint16_t)(U16(WS(), 0x64) + (uint16_t)strip);
					uint32_t cc = U32(WS(), 0x70);
					U32(pkt, 0x10) = cc;
					U32(pkt, 4) = cc;
					cc = U32(WS(), 0x74);
					U32(pkt, 0x28) = cc;
					U32(pkt, 0x1C) = cc;
					int32_t xx = add32(base, i);
					U16(pkt, 0x20) = (uint16_t)xx;
					U16(pkt, 8) = (uint16_t)xx;
					U16(pkt, 0x2C) = (uint16_t)add32(xx, 1);
					U16(pkt, 0x14) = (uint16_t)add32(xx, 1);
					int32_t top = S32(col, 0x10) >> 8;
					U16(pkt, 0x16) = (uint16_t)top;
					U16(pkt, 0xA) = (uint16_t)top;
					int32_t bot = S32(col, 8) >> 8;
					U8(pkt, 0x24) = (uint8_t)i;
					U16(pkt, 0x2E) = (uint16_t)bot;
					U16(pkt, 0x22) = (uint16_t)bot;
					U8(pkt, 0xC) = (uint8_t)i;
					U8(pkt, 0x19) = 0;
					U8(pkt, 0x30) = (uint8_t)(i + 1);
					U8(pkt, 0x18) = (uint8_t)(i + 1);
					U8(pkt, 0xD) = 0;
					U8(pkt, 0x31) = 0xE0;
					U8(pkt, 0x25) = 0xE0;
					x::InsertPrimAltViewport(U32(RT(), 0x4C), pkt);
					pkt += 0x34;
					if (U8(col, 0) & 1)
					{
						U8(pkt, 3) = 6;
						U32(pkt, 8) = 0;
						uint32_t mode = U32(WS(), 0x68);
						U32(pkt, 0xC) = 0x52000000;
						U32(pkt, 4) = mode;
						U32(pkt, 0x14) = 0xFFFF00;
						U16(pkt, 0x18) = (uint16_t)xx;
						U16(pkt, 0x10) = (uint16_t)xx;
						int32_t ty = S32(col, 0x10) >> 8;
						U16(pkt, 0x1A) = (uint16_t)ty;
						U16(pkt, 0x12) = (uint16_t)sub32(ty, 0x80);
						x::InsertPrimAltViewport(U32(RT(), 0x4C), pkt);
						pkt += 0x1C;
					}
				}
				col += 0x20;
			} while (++i < 0x40);
			base += 0x40;
			strip++;
		} while (base < 0x140);
		PTR(CTX(), 0x7C) = pkt;
		if (U8(RT(), 0x45) == 0) U16(blk, 0) = (uint16_t)(U16(blk, 0) + 1);
	}

	static void __cdecl pr_28_RawFT3()
	{
		uint8_t *ws = WS();
		int32_t count = S32(ws, 0x70);
		uint8_t *pkt = PTR(ws, 0x60);
		uint32_t ot = U32(ws, 0x5C);
		uint8_t *rec = PTR(ws, 0x6C);
		do
		{
			uint8_t *vb = PTR(ws, 0x7C);
			uint32_t o0 = U16(rec, 0xA);
			uint32_t o1 = U16(rec, 0xC);
			uint32_t o2 = U16(rec, 0xE);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			uint8_t *p2 = vb + o2;
			uint8_t *p0 = vb + o0;
			uint8_t *p1 = vb + o1;
			if ((uint16_t)(U16(p2, 6) & U16(p1, 6)) & U16(p0, 6)) goto next;
			{
				uint32_t s0 = U32(p0, 0);
				uint32_t s1 = U32(p1, 0);
				uint32_t s2 = U32(p2, 0);
				U32(pkt, 8) = s0;
				U32(pkt, 0x10) = s1;
				U32(pkt, 0x18) = s2;
				if (Nclip(s0, s1, s2) < 0) goto next;
				int32_t sum = (int32_t)((uint32_t)U16(p2, 4) + U16(p1, 4) + U16(p0, 4));
				int32_t key = Div3(sum);
				U8(pkt, 3) = 7;
				U16(pkt, 0x16) = (uint16_t)(U32(ws, 0x50) | U16(rec, 0x12));
				U16(pkt, 0xE) = (uint16_t)add32(U16(rec, 0x10), S32(ws, 0x54));
				U16(pkt, 0xC) = U16(rec, 4);
				U16(pkt, 0x14) = U16(rec, 6);
				U16(pkt, 0x1C) = U16(rec, 8);
				U32(pkt, 4) = U32(rec, 0) | 0x24000000;
				x::InsertPrimDepthKeys(ot + ((uint32_t)key & 0x3FFC), pkt, z0, z1, z2, 0);
				pkt += 0x20;
			}
		next:
			rec += 0x14;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}

	static void __cdecl pr_38_RawFT4()
	{
		uint8_t *ws = WS();
		int32_t count = S32(ws, 0x70);
		uint32_t ot = U32(ws, 0x5C);
		uint8_t *pkt = PTR(ws, 0x60);
		uint8_t *rec = PTR(ws, 0x6C);
		do
		{
			uint8_t *vb = PTR(ws, 0x7C);
			uint32_t o0 = U16(rec, 0xC);
			uint32_t o1 = U16(rec, 0xE);
			uint32_t o2 = U16(rec, 0x10);
			uint32_t o3 = U16(rec, 0x12);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			int32_t z3 = DepthKey(o3);
			uint8_t *p2 = vb + o2;
			uint8_t *p0 = vb + o0;
			uint8_t *p3 = vb + o3;
			uint8_t *p1 = vb + o1;
			if ((uint16_t)(U16(p3, 6) & U16(p2, 6) & U16(p1, 6)) & U16(p0, 6)) goto next;
			{
				uint32_t s2 = U32(p2, 0);
				uint32_t s1 = U32(p1, 0);
				uint32_t s0 = U32(p0, 0);
				uint32_t s3 = U32(p3, 0);
				U32(pkt, 0x10) = s1;
				U32(pkt, 8) = s0;
				U32(pkt, 0x18) = s2;
				U32(pkt, 0x20) = s3;
				if (Nclip(s0, s1, s2) < 0) goto next;
				uint32_t sum = (uint32_t)U16(p3, 4) + U16(p2, 4) + U16(p1, 4) + U16(p0, 4);
				uint32_t key = ((sum >> 2) & 0x3FFC) + ot;
				U8(pkt, 3) = 9;
				U16(pkt, 0x16) = (uint16_t)(U32(ws, 0x50) | U16(rec, 0x16));
				U16(pkt, 0xE) = (uint16_t)add32(U16(rec, 0x14), S32(ws, 0x54));
				U16(pkt, 0xC) = U16(rec, 4);
				U16(pkt, 0x14) = U16(rec, 6);
				U16(pkt, 0x1C) = U16(rec, 8);
				U16(pkt, 0x24) = U16(rec, 0xA);
				U32(pkt, 4) = U32(rec, 0) | 0x2C000000;
				x::InsertPrimDepthKeys(key, pkt, z0, z1, z2, z3);
				pkt += 0x28;
			}
		next:
			rec += 0x18;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}

	static void __cdecl pr_39_RawGT4()
	{
		uint8_t *ws = WS();
		int32_t count = S32(ws, 0x70);
		uint32_t ot = U32(ws, 0x5C);
		uint8_t *pkt = PTR(ws, 0x60);
		uint8_t *rec = PTR(ws, 0x6C);
		do
		{
			uint8_t *vb = PTR(ws, 0x7C);
			uint32_t o0 = U16(rec, 0x18);
			uint32_t o1 = U16(rec, 0x1A);
			uint32_t o2 = U16(rec, 0x1C);
			uint32_t o3 = U16(rec, 0x1E);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			int32_t z3 = DepthKey(o3);
			uint8_t *p2 = vb + o2;
			uint8_t *p0 = vb + o0;
			uint8_t *p3 = vb + o3;
			uint8_t *p1 = vb + o1;
			if ((uint16_t)(U16(p3, 6) & U16(p2, 6) & U16(p1, 6)) & U16(p0, 6)) goto next;
			{
				uint32_t s2 = U32(p2, 0);
				uint32_t s1 = U32(p1, 0);
				uint32_t s0 = U32(p0, 0);
				uint32_t s3 = U32(p3, 0);
				U32(pkt, 0x14) = s1;
				U32(pkt, 8) = s0;
				U32(pkt, 0x20) = s2;
				U32(pkt, 0x2C) = s3;
				if (Nclip(s0, s1, s2) < 0) goto next;
				uint32_t sum = (uint32_t)U16(p3, 4) + U16(p2, 4) + U16(p1, 4) + U16(p0, 4);
				uint32_t key = ((sum >> 2) & 0x3FFC) + ot;
				U8(pkt, 3) = 0xC;
				U16(pkt, 0x1A) = (uint16_t)(U32(ws, 0x50) | U16(rec, 0x22));
				U16(pkt, 0xE) = (uint16_t)add32(U16(rec, 0x20), S32(ws, 0x54));
				uint16_t uv2 = U16(rec, 0x14);
				uint16_t uv3 = U16(rec, 0x16);
				U16(pkt, 0x18) = U16(rec, 0x12);
				U16(pkt, 0xC) = U16(rec, 0x10);
				U16(pkt, 0x24) = uv2;
				U16(pkt, 0x30) = uv3;
				uint32_t c3 = U32(rec, 0xC);
				uint32_t c2 = U32(rec, 8);
				uint32_t c1 = U32(rec, 4);
				uint32_t c0 = U32(rec, 0);
				U32(pkt, 0x1C) = c2;
				U32(pkt, 0x10) = c1;
				U32(pkt, 0x28) = c3;
				U32(pkt, 4) = c0 | 0x3C000000;
				x::InsertPrimDepthKeys(key, pkt, z0, z1, z2, z3);
				pkt += 0x34;
			}
		next:
			rec += 0x24;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}
}
	void apply_cerberus(Clone &c)
	{
		c.draw[45] = part_cerberus::dh_45_ScreenMelt;
		c.prim[28] = part_cerberus::pr_28_RawFT3;
		c.prim[38] = part_cerberus::pr_38_RawFT4;
		c.prim[39] = part_cerberus::pr_39_RawGT4;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace cerberus
{
	static Clone g_c = {};

	static void describe(Clone &c)
	{
		c.name = "203 Cerberus";
		c.effect_id = 203;
		c.vm_table = game_addr(game::GF_203Cerberus_VmOpcodeTable);
		c.bone_table = game_addr(game::GF_203Cerberus_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_B0FCB4);
		c.prim_size = game_addr(game::unk_18732D0);
		c.spr_table = game_addr(game::unk_18732E4);
		c.ptype_table = game_addr(game::unk_1873310);
		c.pop_table = game_addr(game::funcs_B129F7);
		c.draw_table = game_addr(game::GF_203Cerberus_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_18734EE);
		c.attr_184 = game_addr(game::unk_1873508);
		c.attr_186 = game_addr(game::unk_187350A);
		c.attr_194 = game_addr(game::unk_1873518);
		c.attr_19C = game_addr(game::unk_1873520);
		c.desc = game_addr(game::unk_1872EF8);
		c.ptr_block = game_addr(game::unk_1873170);
		c.queue = game_addr(game::unk_2796DA8);
		c.file00 = game_addr(game::unk_2796DDC);
		c.file01 = game_addr(game::unk_2796DD8);
		c.tick = game_addr(game::GF_203Cerberus_SequenceTick);
		c.load_cb = game_addr(game::MAG_203_sub_B143F0);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_gfc_cerberus()
	{
		using namespace gfc;
		cerberus::describe(cerberus::g_c);
		init_clone(cerberus::g_c, nullptr, 0);
		apply_cerberus(cerberus::g_c);
		apply_shared_ribbon(cerberus::g_c);
		apply_shared_misc(cerberus::g_c);
		register_port(game::GF_203Cerberus_SequenceTick, (void *)cerberus::SequenceTask, "GFC203 Cerberus SequenceTick", 203);
	}
}

