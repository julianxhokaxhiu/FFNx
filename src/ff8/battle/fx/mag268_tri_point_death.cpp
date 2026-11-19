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
namespace tripoint268
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace tripoint268
{
	static void describe(Clone &c)
	{
		c.name = "268 Tri-Point death";
		c.effect_id = 268;
		c.vm_table = game_addr(game::funcs_911AAC);
		c.bone_table = game_addr(game::funcs_90E687);
		c.prim_table = game_addr(game::funcs_909FA4);
		c.prim_size = game_addr(game::unk_18546F4);
		c.spr_table = game_addr(game::unk_1854708);
		c.ptype_table = game_addr(game::unk_1854734);
		c.pop_table = game_addr(game::funcs_90C017);
		c.draw_table = game_addr(game::unk_18547A8);
		c.attr_16A = game_addr(game::unk_1854912);
		c.attr_184 = game_addr(game::unk_185492C);
		c.attr_186 = game_addr(game::unk_185492E);
		c.attr_194 = game_addr(game::unk_185493C);
		c.attr_19C = game_addr(game::unk_1854944);
		c.desc = game_addr(game::unk_1854440);
		c.ptr_block = game_addr(game::unk_1854594);
		c.queue = game_addr(game::unk_2795C88);
		c.file00 = game_addr(game::unk_2795CAC);
		c.file01 = game_addr(game::unk_2795CA8);
		c.tick = game_addr(game::MAG_268_sub_905070);
		c.load_cb = game_addr(game::MAG_268_sub_90DA70);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static constexpr game::Id DIS_KEYS = game::unk_2795CB0;
	static const uint16_t DIS_RET16 = 0x634E;
#include "gfc_disintegrate.inc"

	static VoidFn g_save_vm[0x200], g_save_big[256], g_save_draw[256];

	static void apply_slots(Clone &c)
	{
		ff8fx::copy<(sizeof(g_save_vm))>(g_save_vm, c.vm);
		ff8fx::copy<(sizeof(g_save_big))>(g_save_big, c.bigtab);
		ff8fx::copy<(sizeof(g_save_draw))>(g_save_draw, c.draw);
		apply_shared_misc(c);
		const VoidFn d16 = c.draw[16];
		const VoidFn p16 = c.prim[16];
		ff8fx::copy<(sizeof(g_save_vm))>(c.vm, g_save_vm);
		ff8fx::copy<(sizeof(g_save_big))>(c.bigtab, g_save_big);
		ff8fx::copy<(sizeof(g_save_draw))>(c.draw, g_save_draw);
		c.draw[16] = d16;
		c.prim[16] = p16;
		c.vm[0x0AF] = op_0AF_SetInlineDescriptor;
		c.draw[22] = dh_22_EntityDisintegrate;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag268_tri_point_death()
	{
		using namespace gfc;
		tripoint268::describe(tripoint268::g_c);
		init_clone(tripoint268::g_c, nullptr, 0);
		tripoint268::apply_slots(tripoint268::g_c);
		register_port(game::MAG_268_sub_905070, (void *)tripoint268::SequenceTask, "MAG268 Tri-Point death SequenceTick", 268);
	}
}

