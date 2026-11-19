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
namespace explosion237
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace explosion237
{
	static void describe(Clone &c)
	{
		c.name = "237 Explosion";
		c.effect_id = 237;
		c.vm_table = game_addr(game::funcs_A33CFC);
		c.bone_table = game_addr(game::funcs_A30697);
		c.prim_table = game_addr(game::funcs_A2C044);
		c.prim_size = game_addr(game::unk_1865DD4);
		c.spr_table = game_addr(game::unk_1865DE8);
		c.ptype_table = game_addr(game::unk_1865E14);
		c.pop_table = game_addr(game::funcs_A2E087);
		c.draw_table = game_addr(game::unk_1865E88);
		c.attr_16A = game_addr(game::unk_1865FF2);
		c.attr_184 = game_addr(game::unk_186600C);
		c.attr_186 = game_addr(game::unk_186600E);
		c.attr_194 = game_addr(game::unk_186601C);
		c.attr_19C = game_addr(game::unk_1866024);
		c.desc = game_addr(game::unk_1865B68);
		c.ptr_block = game_addr(game::unk_1865C74);
		c.queue = game_addr(game::unk_2796940);
		c.file00 = game_addr(game::unk_2796954);
		c.file01 = game_addr(game::unk_2796950);
		c.tick = game_addr(game::MAG_237_sub_A2A2A0);
		c.load_cb = game_addr(game::MAG_237_sub_A2FA80);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static const uint32_t PRIM57_F0 = 0;
#include "gfc_deathclaw.inc"

	static inline int32_t xl_ISqrt(int32_t v) { return x::f<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }

	static void __cdecl op_112_SetSpeedFromDistance()
	{
		uint8_t *a = blob::GetBone(S16(STREAM(), 4));
		uint8_t *b = blob::GetBone(S16(STREAM(), 6));
		const int32_t dx = S16(b, 0x94) - S16(a, 0x94);
		const int32_t dy = S16(b, 0x96) - S16(a, 0x96);
		const int32_t dz = S16(b, 0x98) - S16(a, 0x98);
		if (!(U8(RT(), 0x4B) & 0x80))
		{
			const int32_t h = xl_ISqrt(add32(mul32(dy, dy), mul32(dx, dx)));
			int32_t r = xl_ISqrt(add32(mul32(h, h), mul32(dz, dz)));
			if (r != 0) r = shl32(r, 16) / S16(STREAM(), 8);
			const int32_t w = S16(STREAM(), 2);
			S32(CUR(), 0x50 + 4 * w) = shl32(r, 8);
		}
		const int32_t w = S16(STREAM(), 2);
		int32_t sq = 0;
		if (w & 0x100) sq = mul32(dx, dx);
		if (w & 0x200) sq = add32(sq, mul32(dy, dy));
		if (w & 0x400) sq = add32(sq, mul32(dz, dz));
		const int32_t v = shl32(xl_ISqrt(sq), 16) / S16(STREAM(), 8);
		S32(CUR(), 0x50 + 4 * (w & 7)) = shl32(v, 8);
		h_B2C440();
		STREAM() = STREAM() + 10;
	}

	static void __cdecl op_113_SetAngleTowardBone()
	{
		uint8_t *a = blob::GetBone(S16(STREAM(), 4));
		uint8_t *b = blob::GetBone(S16(STREAM(), 6));
		const int32_t ang = blob::Blob_B66B30(S16(a, 0x94), S16(a, 0x98), S16(b, 0x94), S16(b, 0x98));
		const int32_t off = shl32(S16(STREAM(), 2), 2);
		S32(CUR(), 0x50 + off) = shl32(ang, 16);
		if (off >= 0xC) C().bone[U8(CUR(), 0x18)]();
		else h_B2C440();
		STREAM() = STREAM() + 8;
	}

	static void __cdecl pr_57_TintedG4Abr()
	{
		h_B29450((int32_t)U32(WS(), 0x84));
		uint8_t *ws = WS();
		uint8_t *pkt = PTR(ws, 0x60);
		uint8_t *rec = PTR(ws, 0x6C);
		int32_t count = S32(ws, 0x70);
		U32(ws, 0xF0) = PRIM57_F0;
		const uint32_t nocull = U32(ws, 0x90) & 0x10;
		do
		{
			const uint32_t o0 = U16(rec, 0x10), o1 = U16(rec, 0x12), o2 = U16(rec, 0x14), o3 = U16(rec, 0x16);
			const int32_t z0 = DcDepthKey(o0);
			const int32_t z1 = DcDepthKey(o1);
			const int32_t z2 = DcDepthKey(o2);
			const int32_t z3 = DcDepthKey(o3);
			uint8_t *vb = PTR(ws, 0x7C);
			uint8_t *p0 = vb + o0, *p1 = vb + o1, *p2 = vb + o2, *p3 = vb + o3;
			const uint32_t s0 = U32(p0, 0), s1 = U32(p1, 0), s2 = U32(p2, 0), s3 = U32(p3, 0);
			U32(pkt, 0xC) = s0;
			U32(pkt, 0x1C) = s2;
			U32(pkt, 0x24) = s3;
			U32(pkt, 0x14) = s1;
			if (nocull == 0)
			{
				DcGteData(12) = s0;
				DcGteData(13) = s1;
				DcGteData(14) = s2;
				x::GteNCLIP();
				if ((int32_t)DcGteData(24) < 0) goto next;
			}
			{
				const uint32_t a0 = U32(p0, 4), a1 = U32(p1, 4), a2 = U32(p2, 4), a3 = U32(p3, 4);
				if ((a3 | a2 | a1 | a0) & 0x460000) goto next;
				U8(pkt, 3) = 0xA;
				U32(pkt, 0x28) = 0xE1000220;
				const uint32_t slot = (((a3 + a2 + a1 + a0) >> 2) & 0x3FFC) + 0xC;
				DcGteData(6) = U32(ws, 0x8C) | U32(rec, 0) | 0x38000000;
				const uint32_t dm = U32(ws, 0x50);
				x::Gte_4601B0();
				U32(pkt, 4) = dm;
				U32(pkt, 8) = DcGteData(22);
				DcGteData(6) = (uint32_t)(uintptr_t)(rec + 4);
				x::Gte_4601B0();
				U32(pkt, 0x10) = DcGteData(22);
				DcGteData(6) = (uint32_t)(uintptr_t)(rec + 8);
				x::Gte_4601B0();
				U32(pkt, 0x18) = DcGteData(22);
				DcGteData(6) = 0xC;
				x::Gte_4601B0();
				U32(pkt, 0x20) = DcGteData(22);
				x::InsertPrimDepthKeys(slot, pkt, z0, z1, z2, z3);
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
		const VoidFn v042 = c.vm[0x042];
		ff8fx::copy<(sizeof(g_save_vm))>(c.vm, g_save_vm);
		ff8fx::copy<(sizeof(g_save_big))>(c.bigtab, g_save_big);
		ff8fx::copy<(sizeof(g_save_draw))>(c.draw, g_save_draw);
		c.vm[0x042] = v042;
		c.vm[0x112] = op_112_SetSpeedFromDistance;
		c.vm[0x113] = op_113_SetAngleTowardBone;
		c.prim[57] = pr_57_TintedG4Abr;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag237_explosion()
	{
		using namespace gfc;
		explosion237::describe(explosion237::g_c);
		init_clone(explosion237::g_c, nullptr, 0);
		explosion237::g_c.bone[4] = explosion237::bh_04_OrbitBetweenBones;
		explosion237::g_c.bone[8] = explosion237::bh_08_VertexAtHeight;
		explosion237::g_c.prim[48] = explosion237::pr_48_TintedFT3Abr;
		explosion237::g_c.draw[31] = explosion237::dh_31_MeshHeightCut;
		explosion237::apply_slots(explosion237::g_c);
		register_port(game::MAG_237_sub_A2A2A0, (void *)explosion237::SequenceTask, "MAG237 Explosion SequenceTick", 237);
	}
}

