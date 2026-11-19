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
	void apply_eden_d(Clone &c);

namespace beam242
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace beam242
{
	static void describe(Clone &c)
	{
		c.name = "242 Beam Cannon (BGH251F2)";
		c.effect_id = 242;
		c.vm_table = game_addr(game::funcs_A018CC);
		c.bone_table = game_addr(game::funcs_9FE4A7);
		c.prim_table = game_addr(game::funcs_9FA0E4);
		c.prim_size = game_addr(game::unk_1862BF8);
		c.spr_table = game_addr(game::unk_1862C0C);
		c.ptype_table = game_addr(game::unk_1862C38);
		c.pop_table = game_addr(game::funcs_9FBE97);
		c.draw_table = game_addr(game::unk_1862CAC);
		c.attr_16A = game_addr(game::unk_1862E16);
		c.attr_184 = game_addr(game::unk_1862E30);
		c.attr_186 = game_addr(game::unk_1862E32);
		c.attr_194 = game_addr(game::unk_1862E40);
		c.attr_19C = game_addr(game::unk_1862E48);
		c.desc = game_addr(game::unk_18629A4);
		c.ptr_block = game_addr(game::unk_1862A98);
		c.queue = game_addr(game::unk_2796828);
		c.file00 = game_addr(game::unk_279683C);
		c.file01 = game_addr(game::unk_2796838);
		c.tick = game_addr(game::MAG_242_sub_9F8380);
		c.load_cb = game_addr(game::MAG_242_sub_9FD890);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static VoidFn g_save_vm[0x200], g_save_big[256], g_save_draw[256];

	static void apply_slots(Clone &c)
	{
		ff8fx::copy<(sizeof(g_save_vm))>(g_save_vm, c.vm);
		ff8fx::copy<(sizeof(g_save_big))>(g_save_big, c.bigtab);
		ff8fx::copy<(sizeof(g_save_draw))>(g_save_draw, c.draw);
		apply_eden_d(c);
		const VoidFn p6 = c.prim[6];
		ff8fx::copy<(sizeof(g_save_vm))>(c.vm, g_save_vm);
		ff8fx::copy<(sizeof(g_save_big))>(c.bigtab, g_save_big);
		ff8fx::copy<(sizeof(g_save_draw))>(c.draw, g_save_draw);
		c.prim[6] = p6;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag242_bgh_beam_cannon()
	{
		using namespace gfc;
		beam242::describe(beam242::g_c);
		init_clone(beam242::g_c, nullptr, 0);
		beam242::apply_slots(beam242::g_c);
		register_port(game::MAG_242_sub_9F8380, (void *)beam242::SequenceTask, "MAG242 Beam Cannon (BGH251F2) SequenceTick", 242);
	}
}

