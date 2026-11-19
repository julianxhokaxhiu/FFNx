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
namespace bghdeath247
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace bghdeath247
{
	static void describe(Clone &c)
	{
		c.name = "247 BGH251F2 Death";
		c.effect_id = 247;
		c.vm_table = game_addr(game::funcs_9D2DCC);
		c.bone_table = game_addr(game::funcs_9CF9A7);
		c.prim_table = game_addr(game::funcs_9CBB54);
		c.prim_size = game_addr(game::unk_185FB04);
		c.spr_table = game_addr(game::unk_185FB18);
		c.ptype_table = game_addr(game::unk_185FB44);
		c.pop_table = game_addr(game::funcs_9CD397);
		c.draw_table = game_addr(game::unk_185FBB8);
		c.attr_16A = game_addr(game::unk_185FD22);
		c.attr_184 = game_addr(game::unk_185FD3C);
		c.attr_186 = game_addr(game::unk_185FD3E);
		c.attr_194 = game_addr(game::unk_185FD4C);
		c.attr_19C = game_addr(game::unk_185FD54);
		c.desc = game_addr(game::unk_185F89C);
		c.ptr_block = game_addr(game::unk_185F9A4);
		c.queue = game_addr(game::unk_2796718);
		c.file00 = game_addr(game::unk_2796710);
		c.file01 = game_addr(game::unk_2796714);
		c.tick = game_addr(game::MAG_247_sub_9C9F00);
		c.load_cb = game_addr(game::MAG_247_sub_9CED90);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag247_bgh_death()
	{
		using namespace gfc;
		bghdeath247::describe(bghdeath247::g_c);
		init_clone(bghdeath247::g_c, nullptr, 0);
		register_port(game::MAG_247_sub_9C9F00, (void *)bghdeath247::SequenceTask, "MAG247 BGH251F2 Death SequenceTick", 247);
	}
}

