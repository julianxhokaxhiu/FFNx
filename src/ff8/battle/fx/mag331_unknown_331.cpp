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
	void apply_brothers(Clone &c);

namespace eff331
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace eff331
{
	static void describe(Clone &c)
	{
		c.name = "331 (attack 313)";
		c.effect_id = 331;
		c.vm_table = game_addr(game::funcs_8E96EC);
		c.bone_table = game_addr(game::funcs_8E61E7);
		c.prim_table = game_addr(game::funcs_8E28C4);
		c.prim_size = game_addr(game::unk_1852840);
		c.spr_table = game_addr(game::unk_1852854);
		c.ptype_table = game_addr(game::unk_1852880);
		c.pop_table = game_addr(game::funcs_8E3BB7);
		c.draw_table = game_addr(game::unk_18528F4);
		c.attr_16A = game_addr(game::unk_1852A5E);
		c.attr_184 = game_addr(game::unk_1852A78);
		c.attr_186 = game_addr(game::unk_1852A7A);
		c.attr_194 = game_addr(game::unk_1852A88);
		c.attr_19C = game_addr(game::unk_1852A90);
		c.desc = game_addr(game::unk_185260C);
		c.ptr_block = game_addr(game::unk_18526E0);
		c.queue = game_addr(game::unk_2795BE0);
		c.file00 = game_addr(game::unk_2795BF4);
		c.file01 = game_addr(game::unk_2795BF0);
		c.tick = game_addr(game::MAG_331_sub_8E0610);
		c.load_cb = game_addr(game::MAG_331_sub_8E55D0);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static inline uint8_t *xl_JointMatrix_B66B80(void *hdr, int32_t joint, int32_t scale, void *m)
	{
		return x::f<uint8_t *(__cdecl *)(void *, int32_t, int32_t, void *)>(game_addr(game::GfCinematic_ModelJointMatrix))(hdr, joint, scale, m);
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

	static void __cdecl op_0B5_JumpIfSlot()
	{
		const uint16_t slot = (uint16_t)U8(CUR(), 0x1B);
		uint8_t *s = STREAM();
		if (U16(s, 2) == slot) STREAM() = s + (int32_t)S16(s, 4);
		else STREAM() = s + 6;
	}

	static VoidFn g_save_vm[0x200], g_save_big[256], g_save_draw[256];

	static void apply_slots(Clone &c)
	{
		ff8fx::copy<(sizeof(g_save_vm))>(g_save_vm, c.vm);
		ff8fx::copy<(sizeof(g_save_big))>(g_save_big, c.bigtab);
		ff8fx::copy<(sizeof(g_save_draw))>(g_save_draw, c.draw);
		apply_brothers(c);
		const VoidFn d38 = c.draw[38];
		ff8fx::copy<(sizeof(g_save_vm))>(c.vm, g_save_vm);
		ff8fx::copy<(sizeof(g_save_big))>(c.bigtab, g_save_big);
		ff8fx::copy<(sizeof(g_save_draw))>(c.draw, g_save_draw);
		c.draw[38] = d38;
		c.vm[0x0AD] = op_0AD_PosFromPosedEntityJoint;
		c.vm[0x0B5] = op_0B5_JumpIfSlot;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag331_unknown_331()
	{
		using namespace gfc;
		eff331::describe(eff331::g_c);
		init_clone(eff331::g_c, nullptr, 0);
		eff331::apply_slots(eff331::g_c);
		register_port(game::MAG_331_sub_8E0610, (void *)eff331::SequenceTask, "MAG331 effect 331 (attack 313) SequenceTick", 331);
	}
}

