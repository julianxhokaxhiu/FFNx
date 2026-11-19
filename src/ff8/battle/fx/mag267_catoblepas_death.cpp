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
namespace catoblepas267
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace catoblepas267
{
	static void describe(Clone &c)
	{
		c.name = "267 Catoblepas death";
		c.effect_id = 267;
		c.vm_table = game_addr(game::funcs_91F09C);
		c.bone_table = game_addr(game::funcs_91BC77);
		c.prim_table = game_addr(game::funcs_917594);
		c.prim_size = game_addr(game::unk_1855138);
		c.spr_table = game_addr(game::unk_185514C);
		c.ptype_table = game_addr(game::unk_1855178);
		c.pop_table = game_addr(game::funcs_919607);
		c.draw_table = game_addr(game::unk_18551EC);
		c.attr_16A = game_addr(game::unk_1855356);
		c.attr_184 = game_addr(game::unk_1855370);
		c.attr_186 = game_addr(game::unk_1855372);
		c.attr_194 = game_addr(game::unk_1855380);
		c.attr_19C = game_addr(game::unk_1855388);
		c.desc = game_addr(game::unk_1854E84);
		c.ptr_block = game_addr(game::unk_1854FD8);
		c.queue = game_addr(game::unk_2795CC0);
		c.file00 = game_addr(game::unk_2795CD4);
		c.file01 = game_addr(game::unk_2795CD0);
		c.tick = game_addr(game::MAG_267_sub_912660);
		c.load_cb = game_addr(game::MAG_267_sub_91B060);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static constexpr game::Id DIS_KEYS = game::unk_2795CE8;
	static const uint16_t DIS_RET16 = 0x393E;
#include "gfc_disintegrate.inc"

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
		c.vm[0x0AF] = op_0AF_SetInlineDescriptor;
		c.draw[22] = dh_22_EntityDisintegrate;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag267_catoblepas_death()
	{
		using namespace gfc;
		catoblepas267::describe(catoblepas267::g_c);
		init_clone(catoblepas267::g_c, nullptr, 0);
		catoblepas267::apply_slots(catoblepas267::g_c);
		register_port(game::MAG_267_sub_912660, (void *)catoblepas267::SequenceTask, "MAG267 Catoblepas death SequenceTick", 267);
	}
}

