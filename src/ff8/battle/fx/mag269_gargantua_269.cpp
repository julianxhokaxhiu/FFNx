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
namespace gargantua269
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace gargantua269
{
	static void describe(Clone &c)
	{
		c.name = "269 Gargantua";
		c.effect_id = 269;
		c.vm_table = game_addr(game::funcs_9044BC);
		c.bone_table = game_addr(game::funcs_901017);
		c.prim_table = game_addr(game::funcs_8FC934);
		c.prim_size = game_addr(game::unk_1853CB0);
		c.spr_table = game_addr(game::unk_1853CC4);
		c.ptype_table = game_addr(game::unk_1853CF0);
		c.pop_table = game_addr(game::funcs_8FE9A7);
		c.draw_table = game_addr(game::unk_1853D64);
		c.attr_16A = game_addr(game::unk_1853ECE);
		c.attr_184 = game_addr(game::unk_1853EE8);
		c.attr_186 = game_addr(game::unk_1853EEA);
		c.attr_194 = game_addr(game::unk_1853EF8);
		c.attr_19C = game_addr(game::unk_1853F00);
		c.desc = game_addr(game::unk_1853A0C);
		c.ptr_block = game_addr(game::unk_1853B50);
		c.queue = game_addr(game::unk_2795C50);
		c.file00 = game_addr(game::unk_2795C74);
		c.file01 = game_addr(game::unk_2795C70);
		c.tick = game_addr(game::MAG_269_sub_8F7890);
		c.load_cb = game_addr(game::MAG_269_sub_900400);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static constexpr game::Id DIS_KEYS = game::unk_2795C78;
	static const uint16_t DIS_RET16 = 0x8B6E;
#include "gfc_disintegrate.inc"

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
		c.vm[0x0AF] = op_0AF_SetInlineDescriptor;
		c.vm[0x035] = op_035_VramReadback;
		c.draw[22] = dh_22_EntityDisintegrate;
		c.draw[47] = dh_47_PaletteHsvShift;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag269_gargantua_269()
	{
		using namespace gfc;
		gargantua269::describe(gargantua269::g_c);
		init_clone(gargantua269::g_c, nullptr, 0);
		gargantua269::apply_slots(gargantua269::g_c);
		register_port(game::MAG_269_sub_8F7890, (void *)gargantua269::SequenceTask, "MAG269 Gargantua 269 SequenceTick", 269);
	}
}

