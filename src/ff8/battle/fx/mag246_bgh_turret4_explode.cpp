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
namespace turret246
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace turret246
{
	static void describe(Clone &c)
	{
		c.name = "246 BGH251F2 4th turret explosion";
		c.effect_id = 246;
		c.vm_table = game_addr(game::funcs_9DC04C);
		c.bone_table = game_addr(game::funcs_9D8C27);
		c.prim_table = game_addr(game::funcs_9D55D4);
		c.prim_size = game_addr(game::unk_18604C8);
		c.spr_table = game_addr(game::unk_18604DC);
		c.ptype_table = game_addr(game::unk_1860508);
		c.pop_table = game_addr(game::funcs_9D6617);
		c.draw_table = game_addr(game::unk_186057C);
		c.attr_16A = game_addr(game::unk_18606E6);
		c.attr_184 = game_addr(game::unk_1860700);
		c.attr_186 = game_addr(game::unk_1860702);
		c.attr_194 = game_addr(game::unk_1860710);
		c.attr_19C = game_addr(game::unk_1860718);
		c.desc = game_addr(game::unk_1860294);
		c.ptr_block = game_addr(game::unk_1860368);
		c.queue = game_addr(game::unk_2796750);
		c.file00 = game_addr(game::unk_2796748);
		c.file01 = game_addr(game::unk_279674C);
		c.tick = game_addr(game::MAG_246_sub_9D3980);
		c.load_cb = game_addr(game::MAG_246_sub_9D8010);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag246_bgh_turret4_explode()
	{
		using namespace gfc;
		turret246::describe(turret246::g_c);
		init_clone(turret246::g_c, nullptr, 0);
		register_port(game::MAG_246_sub_9D3980, (void *)turret246::SequenceTask, "MAG246 BGH251F2 4th turret explosion SequenceTick", 246);
	}
}

