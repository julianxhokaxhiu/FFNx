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
namespace sorceress208
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace sorceress208
{
	static void describe(Clone &c)
	{
		c.name = "208 Final Sorceress Death";
		c.effect_id = 208;
		c.vm_table = game_addr(game::funcs_AE288C);
		c.bone_table = game_addr(game::funcs_ADF3E7);
		c.prim_table = game_addr(game::funcs_ADAD64);
		c.prim_size = game_addr(game::unk_186FEB0);
		c.spr_table = game_addr(game::unk_186FEC4);
		c.ptype_table = game_addr(game::unk_186FEF0);
		c.pop_table = game_addr(game::funcs_ADCDD7);
		c.draw_table = game_addr(game::unk_186FF64);
		c.attr_16A = game_addr(game::unk_18700CE);
		c.attr_184 = game_addr(game::unk_18700E8);
		c.attr_186 = game_addr(game::unk_18700EA);
		c.attr_194 = game_addr(game::unk_18700F8);
		c.attr_19C = game_addr(game::unk_1870100);
		c.desc = game_addr(game::unk_186FBEC);
		c.ptr_block = game_addr(game::unk_186FD50);
		c.queue = game_addr(game::unk_2796CC0);
		c.file00 = game_addr(game::unk_2796CF0);
		c.file01 = game_addr(game::unk_2796CF4);
		c.tick = game_addr(game::MAG_208_sub_AD7F70);
		c.load_cb = game_addr(game::MAG_208_sub_ADE7D0);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

#include "gfc_melt.inc"

#include "gfc_palshift.inc"

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
		c.vm[0x035] = op_035_VramReadback;
		c.draw[47] = dh_47_PaletteHsvShift;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag208_final_sorceress_death()
	{
		using namespace gfc;
		sorceress208::describe(sorceress208::g_c);
		init_clone(sorceress208::g_c, nullptr, 0);
		sorceress208::apply_slots(sorceress208::g_c);
		register_port(game::MAG_208_sub_AD7F70, (void *)sorceress208::SequenceTask, "MAG208 Final Sorceress Death SequenceTick", 208);
	}
}

