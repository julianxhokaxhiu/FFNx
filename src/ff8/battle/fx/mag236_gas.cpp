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
namespace gas236
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace gas236
{
	static void describe(Clone &c)
	{
		c.name = "236 Gas";
		c.effect_id = 236;
		c.vm_table = game_addr(game::funcs_A3DE2C);
		c.bone_table = game_addr(game::funcs_A3AA07);
		c.prim_table = game_addr(game::funcs_A36654);
		c.prim_size = game_addr(game::unk_18667D0);
		c.spr_table = game_addr(game::unk_18667E4);
		c.ptype_table = game_addr(game::unk_1866810);
		c.pop_table = game_addr(game::funcs_A383F7);
		c.draw_table = game_addr(game::unk_1866884);
		c.attr_16A = game_addr(game::unk_18669EE);
		c.attr_184 = game_addr(game::unk_1866A08);
		c.attr_186 = game_addr(game::unk_1866A0A);
		c.attr_194 = game_addr(game::unk_1866A18);
		c.attr_19C = game_addr(game::unk_1866A20);
		c.desc = game_addr(game::unk_1866564);
		c.ptr_block = game_addr(game::unk_1866670);
		c.queue = game_addr(game::unk_2796978);
		c.file00 = game_addr(game::unk_279698C);
		c.file01 = game_addr(game::unk_2796988);
		c.tick = game_addr(game::MAG_236_sub_A348B0);
		c.load_cb = game_addr(game::MAG_236_sub_A39DF0);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

#include "gfc_deathclaw.inc"

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag236_gas()
	{
		using namespace gfc;
		gas236::describe(gas236::g_c);
		init_clone(gas236::g_c, nullptr, 0);
		gas236::g_c.bone[4] = gas236::bh_04_OrbitBetweenBones;
		gas236::g_c.bone[8] = gas236::bh_08_VertexAtHeight;
		gas236::g_c.prim[48] = gas236::pr_48_TintedFT3Abr;
		gas236::g_c.draw[31] = gas236::dh_31_MeshHeightCut;
		register_port(game::MAG_236_sub_A348B0, (void *)gas236::SequenceTask, "MAG236 Gas SequenceTick", 236);
	}
}

