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
namespace adel211
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace adel211
{
	static void describe(Clone &c)
	{
		c.name = "211 Adel Death";
		c.effect_id = 211;
		c.vm_table = game_addr(game::funcs_AD73BC);
		c.bone_table = game_addr(game::funcs_AD3F17);
		c.prim_table = game_addr(game::funcs_ACF834);
		c.prim_size = game_addr(game::unk_186F45C);
		c.spr_table = game_addr(game::unk_186F470);
		c.ptype_table = game_addr(game::unk_186F49C);
		c.pop_table = game_addr(game::funcs_AD18A7);
		c.draw_table = game_addr(game::unk_186F510);
		c.attr_16A = game_addr(game::unk_186F67A);
		c.attr_184 = game_addr(game::unk_186F694);
		c.attr_186 = game_addr(game::unk_186F696);
		c.attr_194 = game_addr(game::unk_186F6A4);
		c.attr_19C = game_addr(game::unk_186F6AC);
		c.desc = game_addr(game::unk_186F1B0);
		c.ptr_block = game_addr(game::unk_186F2FC);
		c.queue = game_addr(game::unk_2796C88);
		c.file00 = game_addr(game::unk_2796CBC);
		c.file01 = game_addr(game::unk_2796CB8);
		c.tick = game_addr(game::MAG_211_sub_ACA790);
		c.load_cb = game_addr(game::MAG_211_sub_AD3300);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static constexpr game::Id DIS_KEYS = game::unk_2796CA8;
	static const uint16_t DIS_RET16 = 0xBA6E;
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

	void register_mag211_adel_death()
	{
		using namespace gfc;
		adel211::describe(adel211::g_c);
		init_clone(adel211::g_c, nullptr, 0);
		adel211::apply_slots(adel211::g_c);
		register_port(game::MAG_211_sub_ACA790, (void *)adel211::SequenceTask, "MAG211 Adel Death SequenceTick", 211);
	}
}

