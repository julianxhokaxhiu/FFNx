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
namespace krysta264
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace krysta264
{
	static void describe(Clone &c)
	{
		c.name = "264 Krysta";
		c.effect_id = 264;
		c.vm_table = game_addr(game::funcs_9452CC);
		c.bone_table = game_addr(game::funcs_941C77);
		c.prim_table = game_addr(game::funcs_93D454);
		c.prim_size = game_addr(game::unk_1856FEC);
		c.spr_table = game_addr(game::unk_1857000);
		c.ptype_table = game_addr(game::unk_185702C);
		c.pop_table = game_addr(game::funcs_93F4C7);
		c.draw_table = game_addr(game::unk_18570A0);
		c.attr_16A = game_addr(game::unk_185720A);
		c.attr_184 = game_addr(game::unk_1857224);
		c.attr_186 = game_addr(game::unk_1857226);
		c.attr_194 = game_addr(game::unk_1857234);
		c.attr_19C = game_addr(game::unk_185723C);
		c.desc = game_addr(game::unk_1856D48);
		c.ptr_block = game_addr(game::unk_1856E8C);
		c.queue = game_addr(game::unk_2795D68);
		c.file00 = game_addr(game::unk_2795D8C);
		c.file01 = game_addr(game::unk_2795D88);
		c.tick = game_addr(game::MAG_264_sub_938520);
		c.load_cb = game_addr(game::MAG_264_sub_941060);
		c.lit_dispatcher = true;
		c.bone_count = 18;
	}

	static constexpr game::Id DIS_KEYS = game::unk_2795D90;
	static const uint16_t DIS_RET16 = 0x97FE;
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

	void register_mag264_krysta_264()
	{
		using namespace gfc;
		krysta264::describe(krysta264::g_c);
		init_clone(krysta264::g_c, nullptr, 0);
		krysta264::apply_slots(krysta264::g_c);
		register_port(game::MAG_264_sub_938520, (void *)krysta264::SequenceTask, "MAG264 Krysta 264 SequenceTick", 264);
	}
}

