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

namespace beam250
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace beam250
{
	static void describe(Clone &c)
	{
		c.name = "250 Beam Cannon";
		c.effect_id = 250;
		c.vm_table = game_addr(game::funcs_9B6E3C);
		c.bone_table = game_addr(game::funcs_9B3A17);
		c.prim_table = game_addr(game::funcs_9AF654);
		c.prim_size = game_addr(game::unk_185DD8C);
		c.spr_table = game_addr(game::unk_185DDA0);
		c.ptype_table = game_addr(game::unk_185DDCC);
		c.pop_table = game_addr(game::funcs_9B1407);
		c.draw_table = game_addr(game::unk_185DE40);
		c.attr_16A = game_addr(game::unk_185DFAA);
		c.attr_184 = game_addr(game::unk_185DFC4);
		c.attr_186 = game_addr(game::unk_185DFC6);
		c.attr_194 = game_addr(game::unk_185DFD4);
		c.attr_19C = game_addr(game::unk_185DFDC);
		c.desc = game_addr(game::unk_185DB38);
		c.ptr_block = game_addr(game::unk_185DC2C);
		c.queue = game_addr(game::unk_2796668);
		c.file00 = game_addr(game::unk_2796678);
		c.file01 = game_addr(game::unk_2796660);
		c.tick = game_addr(game::MAG_250_sub_9AC660);
		c.load_cb = game_addr(game::MAG_250_sub_9B2E00);
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
		apply_shared_ribbon(c);
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag250_beam_cannon()
	{
		using namespace gfc;
		beam250::describe(beam250::g_c);
		init_clone(beam250::g_c, nullptr, 0);
		beam250::apply_slots(beam250::g_c);
		register_port(game::MAG_250_sub_9AC660, (void *)beam250::SequenceTask, "MAG250 Beam Cannon SequenceTick", 250);
	}
}

