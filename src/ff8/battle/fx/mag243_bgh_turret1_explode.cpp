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
namespace turret243
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace turret243
{
	static void describe(Clone &c)
	{
		c.name = "243 BGH251F2 1st turret explosion";
		c.effect_id = 243;
		c.vm_table = game_addr(game::funcs_9F77CC);
		c.bone_table = game_addr(game::funcs_9F43A7);
		c.prim_table = game_addr(game::funcs_9F0D54);
		c.prim_size = game_addr(game::unk_1862214);
		c.spr_table = game_addr(game::unk_1862228);
		c.ptype_table = game_addr(game::unk_1862254);
		c.pop_table = game_addr(game::funcs_9F1D97);
		c.draw_table = game_addr(game::unk_18622C8);
		c.attr_16A = game_addr(game::unk_1862432);
		c.attr_184 = game_addr(game::unk_186244C);
		c.attr_186 = game_addr(game::unk_186244E);
		c.attr_194 = game_addr(game::unk_186245C);
		c.attr_19C = game_addr(game::unk_1862464);
		c.desc = game_addr(game::unk_1861FE0);
		c.ptr_block = game_addr(game::unk_18620B4);
		c.queue = game_addr(game::unk_27967F0);
		c.file00 = game_addr(game::unk_2796804);
		c.file01 = game_addr(game::unk_2796800);
		c.tick = game_addr(game::MAG_243_sub_9EF100);
		c.load_cb = game_addr(game::MAG_243_sub_9F3790);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag243_bgh_turret1_explode()
	{
		using namespace gfc;
		turret243::describe(turret243::g_c);
		init_clone(turret243::g_c, nullptr, 0);
		register_port(game::MAG_243_sub_9EF100, (void *)turret243::SequenceTask, "MAG243 BGH251F2 1st turret explosion SequenceTick", 243);
	}
}

