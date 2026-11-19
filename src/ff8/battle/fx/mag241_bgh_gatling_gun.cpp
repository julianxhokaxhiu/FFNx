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
namespace gatling241
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace gatling241
{
	static void describe(Clone &c)
	{
		c.name = "241 Gatling Gun (BGH251F2)";
		c.effect_id = 241;
		c.vm_table = game_addr(game::funcs_A0AE1C);
		c.bone_table = game_addr(game::funcs_A079F7);
		c.prim_table = game_addr(game::funcs_A040D4);
		c.prim_size = game_addr(game::unk_18635D8);
		c.spr_table = game_addr(game::unk_18635EC);
		c.ptype_table = game_addr(game::unk_1863618);
		c.pop_table = game_addr(game::funcs_A053E7);
		c.draw_table = game_addr(game::unk_186368C);
		c.attr_16A = game_addr(game::unk_18637F6);
		c.attr_184 = game_addr(game::unk_1863810);
		c.attr_186 = game_addr(game::unk_1863812);
		c.attr_194 = game_addr(game::unk_1863820);
		c.attr_19C = game_addr(game::unk_1863828);
		c.desc = game_addr(game::unk_1863388);
		c.ptr_block = game_addr(game::unk_1863478);
		c.queue = game_addr(game::unk_2796860);
		c.file00 = game_addr(game::unk_2796874);
		c.file01 = game_addr(game::unk_2796870);
		c.tick = game_addr(game::MAG_241_sub_A02480);
		c.load_cb = game_addr(game::MAG_241_sub_A06DE0);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag241_bgh_gatling_gun()
	{
		using namespace gfc;
		gatling241::describe(gatling241::g_c);
		init_clone(gatling241::g_c, nullptr, 0);
		register_port(game::MAG_241_sub_A02480, (void *)gatling241::SequenceTask, "MAG241 Gatling Gun (BGH251F2) SequenceTick", 241);
	}
}

