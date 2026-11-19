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
namespace sphinxara265
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace sphinxara265
{
	static void describe(Clone &c)
	{
		c.name = "265 Sphinxara";
		c.effect_id = 265;
		c.vm_table = game_addr(game::funcs_93796C);
		c.bone_table = game_addr(game::funcs_934547);
		c.prim_table = game_addr(game::funcs_92FEC4);
		c.prim_size = game_addr(game::unk_18565B8);
		c.spr_table = game_addr(game::unk_18565CC);
		c.ptype_table = game_addr(game::unk_18565F8);
		c.pop_table = game_addr(game::funcs_931F37);
		c.draw_table = game_addr(game::unk_185666C);
		c.attr_16A = game_addr(game::unk_18567D6);
		c.attr_184 = game_addr(game::unk_18567F0);
		c.attr_186 = game_addr(game::unk_18567F2);
		c.attr_194 = game_addr(game::unk_1856800);
		c.attr_19C = game_addr(game::unk_1856808);
		c.desc = game_addr(game::unk_1856304);
		c.ptr_block = game_addr(game::unk_1856458);
		c.queue = game_addr(game::unk_2795D30);
		c.file00 = game_addr(game::unk_2795D44);
		c.file01 = game_addr(game::unk_2795D40);
		c.tick = game_addr(game::MAG_265_sub_92D240);
		c.load_cb = game_addr(game::MAG_265_sub_933930);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

#include "gfc_melt.inc"

	static VoidFn g_save_vm[0x200], g_save_big[256], g_save_draw[256];

	static void apply_slots(Clone &c)
	{
		ff8fx::copy<(sizeof(g_save_vm))>(g_save_vm, c.vm);
		ff8fx::copy<(sizeof(g_save_big))>(g_save_big, c.bigtab);
		ff8fx::copy<(sizeof(g_save_draw))>(g_save_draw, c.draw);
		apply_shared_misc(c);
		const VoidFn d16 = c.draw[16];
		const VoidFn p16 = c.prim[16];
		ff8fx::copy<(sizeof(g_save_vm))>(c.vm, g_save_vm);
		ff8fx::copy<(sizeof(g_save_big))>(c.bigtab, g_save_big);
		ff8fx::copy<(sizeof(g_save_draw))>(c.draw, g_save_draw);
		c.draw[16] = d16;
		c.prim[16] = p16;
		c.draw[3] = dh_03_AnimatedModelWithShadow;
		c.draw[52] = dh_52_EntityMelt;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag265_sphinxara_265()
	{
		using namespace gfc;
		sphinxara265::describe(sphinxara265::g_c);
		init_clone(sphinxara265::g_c, nullptr, 0);
		sphinxara265::apply_slots(sphinxara265::g_c);
		register_port(game::MAG_265_sub_92D240, (void *)sphinxara265::SequenceTask, "MAG265 Sphinxara 265 SequenceTick", 265);
	}
}

