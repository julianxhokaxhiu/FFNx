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
namespace bgh249
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace bgh249
{
	static void describe(Clone &c)
	{
		c.name = "249 Attack (BGH251F2)";
		c.effect_id = 249;
		c.vm_table = game_addr(game::funcs_9C038C);
		c.bone_table = game_addr(game::funcs_9BCF67);
		c.prim_table = game_addr(game::funcs_9B9644);
		c.prim_size = game_addr(game::unk_185E76C);
		c.spr_table = game_addr(game::unk_185E780);
		c.ptype_table = game_addr(game::unk_185E7AC);
		c.pop_table = game_addr(game::funcs_9BA957);
		c.draw_table = game_addr(game::unk_185E820);
		c.attr_16A = game_addr(game::unk_185E98A);
		c.attr_184 = game_addr(game::unk_185E9A4);
		c.attr_186 = game_addr(game::unk_185E9A6);
		c.attr_194 = game_addr(game::unk_185E9B4);
		c.attr_19C = game_addr(game::unk_185E9BC);
		c.desc = game_addr(game::unk_185E51C);
		c.ptr_block = game_addr(game::unk_185E60C);
		c.queue = game_addr(game::unk_27966A0);
		c.file00 = game_addr(game::unk_27966B0);
		c.file01 = game_addr(game::unk_27966B4);
		c.tick = game_addr(game::MAG_249_sub_9B79F0);
		c.load_cb = game_addr(game::MAG_249_sub_9BC350);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag249_bgh_attack()
	{
		using namespace gfc;
		bgh249::describe(bgh249::g_c);
		init_clone(bgh249::g_c, nullptr, 0);
		register_port(game::MAG_249_sub_9B79F0, (void *)bgh249::SequenceTask, "MAG249 Attack (BGH251F2) SequenceTick", 249);
	}
}

