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
namespace ultima270
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace ultima270
{
	static void describe(Clone &c)
	{
		c.name = "270 Ultima Weapon Death";
		c.effect_id = 270;
		c.vm_table = game_addr(game::funcs_8F6CDC);
		c.bone_table = game_addr(game::funcs_8F38B7);
		c.prim_table = game_addr(game::funcs_8EF1D4);
		c.prim_size = game_addr(game::unk_185327C);
		c.spr_table = game_addr(game::unk_1853290);
		c.ptype_table = game_addr(game::unk_18532BC);
		c.pop_table = game_addr(game::funcs_8F1247);
		c.draw_table = game_addr(game::unk_1853330);
		c.attr_16A = game_addr(game::unk_185349A);
		c.attr_184 = game_addr(game::unk_18534B4);
		c.attr_186 = game_addr(game::unk_18534B6);
		c.attr_194 = game_addr(game::unk_18534C4);
		c.attr_19C = game_addr(game::unk_18534CC);
		c.desc = game_addr(game::unk_1852FD0);
		c.ptr_block = game_addr(game::unk_185311C);
		c.queue = game_addr(game::unk_2795C18);
		c.file00 = game_addr(game::unk_2795C3C);
		c.file01 = game_addr(game::unk_2795C38);
		c.tick = game_addr(game::MAG_270_sub_8EA2A0);
		c.load_cb = game_addr(game::resetByte_2798219);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static constexpr game::Id DIS_KEYS = game::unk_2795C40;
	static const uint16_t DIS_RET16 = 0xB57E;
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

	void register_mag270_ultima_weapon_death()
	{
		using namespace gfc;
		ultima270::describe(ultima270::g_c);
		init_clone(ultima270::g_c, nullptr, 0);
		ultima270::apply_slots(ultima270::g_c);
		register_port(game::MAG_270_sub_8EA2A0, (void *)ultima270::SequenceTask, "MAG270 Ultima Weapon Death SequenceTick", 270);
	}
}

