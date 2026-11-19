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
namespace megaflare257
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace megaflare257
{
	static void describe(Clone &c)
	{
		c.name = "257 Mega Flare (Bahamut)";
		c.effect_id = 257;
		c.vm_table = game_addr(game::funcs_978EBC);
		c.bone_table = game_addr(game::funcs_975A97);
		c.prim_table = game_addr(game::funcs_971664);
		c.prim_size = game_addr(game::unk_185A30C);
		c.spr_table = game_addr(game::unk_185A320);
		c.ptype_table = game_addr(game::unk_185A34C);
		c.pop_table = game_addr(game::funcs_973487);
		c.draw_table = game_addr(game::unk_185A3C0);
		c.attr_16A = game_addr(game::unk_185A52A);
		c.attr_184 = game_addr(game::unk_185A544);
		c.attr_186 = game_addr(game::unk_185A546);
		c.attr_194 = game_addr(game::unk_185A554);
		c.attr_19C = game_addr(game::unk_185A55C);
		c.desc = game_addr(game::unk_185A064);
		c.ptr_block = game_addr(game::unk_185A1AC);
		c.queue = game_addr(game::unk_2796510);
		c.file00 = game_addr(game::unk_279650C);
		c.file01 = game_addr(game::unk_2796508);
		c.tick = game_addr(game::MAG_257_sub_96F900);
		c.load_cb = game_addr(game::MAG_257_sub_974E80);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	inline uint32_t &GteData(int reg) { return MEM<uint32_t>(game_addr(game::GTE_VXY0) + 4 * reg); }

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

	static void apply_slots(Clone &c)
	{
		c.prim[58] = pr_58_TintedFT4Abr;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag257_mega_flare()
	{
		using namespace gfc;
		megaflare257::describe(megaflare257::g_c);
		init_clone(megaflare257::g_c, nullptr, 0);
		megaflare257::apply_slots(megaflare257::g_c);
		register_port(game::MAG_257_sub_96F900, (void *)megaflare257::SequenceTask, "MAG257 Mega Flare (Bahamut) SequenceTick", 257);
	}
}

