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
namespace turret245
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace turret245
{
	static void describe(Clone &c)
	{
		c.name = "245 BGH251F2 3rd turret explosion";
		c.effect_id = 245;
		c.vm_table = game_addr(game::funcs_9E52CC);
		c.bone_table = game_addr(game::funcs_9E1EA7);
		c.prim_table = game_addr(game::funcs_9DE854);
		c.prim_size = game_addr(game::unk_1860E8C);
		c.spr_table = game_addr(game::unk_1860EA0);
		c.ptype_table = game_addr(game::unk_1860ECC);
		c.pop_table = game_addr(game::funcs_9DF897);
		c.draw_table = game_addr(game::unk_1860F40);
		c.attr_16A = game_addr(game::unk_18610AA);
		c.attr_184 = game_addr(game::unk_18610C4);
		c.attr_186 = game_addr(game::unk_18610C6);
		c.attr_194 = game_addr(game::unk_18610D4);
		c.attr_19C = game_addr(game::unk_18610DC);
		c.desc = game_addr(game::unk_1860C58);
		c.ptr_block = game_addr(game::unk_1860D2C);
		c.queue = game_addr(game::unk_2796788);
		c.file00 = game_addr(game::unk_2796784);
		c.file01 = game_addr(game::unk_2796780);
		c.tick = game_addr(game::MAG_245_sub_9DCC00);
		c.load_cb = game_addr(game::MAG_245_sub_9E1290);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag245_bgh_turret3_explode()
	{
		using namespace gfc;
		turret245::describe(turret245::g_c);
		init_clone(turret245::g_c, nullptr, 0);
		register_port(game::MAG_245_sub_9DCC00, (void *)turret245::SequenceTask, "MAG245 BGH251F2 3rd turret explosion SequenceTick", 245);
	}
}

