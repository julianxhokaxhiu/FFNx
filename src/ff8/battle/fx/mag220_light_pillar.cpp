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

namespace pillar220
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace pillar220
{
	static void describe(Clone &c)
	{
		c.name = "220 Light Pillar";
		c.effect_id = 220;
		c.vm_table = game_addr(game::funcs_ABCEDC);
		c.bone_table = game_addr(game::funcs_AB9AB7);
		c.prim_table = game_addr(game::funcs_AB56F4);
		c.prim_size = game_addr(game::unk_186DF40);
		c.spr_table = game_addr(game::unk_186DF54);
		c.ptype_table = game_addr(game::unk_186DF80);
		c.pop_table = game_addr(game::funcs_AB74A7);
		c.draw_table = game_addr(game::unk_186DFF4);
		c.attr_16A = game_addr(game::unk_186E15E);
		c.attr_184 = game_addr(game::unk_186E178);
		c.attr_186 = game_addr(game::unk_186E17A);
		c.attr_194 = game_addr(game::unk_186E188);
		c.attr_19C = game_addr(game::unk_186E190);
		c.desc = game_addr(game::unk_186DCEC);
		c.ptr_block = game_addr(game::unk_186DDE0);
		c.queue = game_addr(game::unk_2796C18);
		c.file00 = game_addr(game::unk_2796C2C);
		c.file01 = game_addr(game::unk_2796C28);
		c.tick = game_addr(game::MAG_220_sub_AB3990);
		c.load_cb = game_addr(game::MAG_220_sub_AB8EA0);
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
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag220_light_pillar()
	{
		using namespace gfc;
		pillar220::describe(pillar220::g_c);
		init_clone(pillar220::g_c, nullptr, 0);
		pillar220::apply_slots(pillar220::g_c);
		register_port(game::MAG_220_sub_AB3990, (void *)pillar220::SequenceTask, "MAG220 Light Pillar SequenceTick", 220);
	}
}

