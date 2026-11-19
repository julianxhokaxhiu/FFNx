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
namespace turret244
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace turret244
{
	static void describe(Clone &c)
	{
		c.name = "244 BGH251F2 turret explosion";
		c.effect_id = 244;
		c.vm_table = game_addr(game::funcs_9EE54C);
		c.bone_table = game_addr(game::funcs_9EB127);
		c.prim_table = game_addr(game::funcs_9E7AD4);
		c.prim_size = game_addr(game::unk_1861850);
		c.spr_table = game_addr(game::unk_1861864);
		c.ptype_table = game_addr(game::unk_1861890);
		c.pop_table = game_addr(game::funcs_9E8B17);
		c.draw_table = game_addr(game::unk_1861904);
		c.attr_16A = game_addr(game::unk_1861A6E);
		c.attr_184 = game_addr(game::unk_1861A88);
		c.attr_186 = game_addr(game::unk_1861A8A);
		c.attr_194 = game_addr(game::unk_1861A98);
		c.attr_19C = game_addr(game::unk_1861AA0);
		c.desc = game_addr(game::unk_186161C);
		c.ptr_block = game_addr(game::unk_18616F0);
		c.queue = game_addr(game::unk_27967C0);
		c.file00 = game_addr(game::unk_27967BC);
		c.file01 = game_addr(game::unk_27967B8);
		c.tick = game_addr(game::MAG_244_sub_9E5E80);
		c.load_cb = game_addr(game::MAG_244_sub_9EA510);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag244_bgh_turret_explode()
	{
		using namespace gfc;
		turret244::describe(turret244::g_c);
		init_clone(turret244::g_c, nullptr, 0);
		register_port(game::MAG_244_sub_9E5E80, (void *)turret244::SequenceTask, "MAG244 BGH251F2 turret explosion SequenceTick", 244);
	}
}

