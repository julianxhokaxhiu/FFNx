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

// Effect 222: Water

#include "gfc_engine.h"

namespace ff8fx
{
namespace gfc
{
	void apply_brothers(Clone &c);

namespace water
{
	static Clone g_water = {};

	static void describe(Clone &c)
	{
		c.name = "222 Water";
		c.effect_id = 222;
		c.vm_table = game_addr(game::GF_222Water_VmOpcodeTable);
		c.bone_table = game_addr(game::GF_222Water_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_A9DC34);
		c.prim_size = game_addr(game::unk_186CAA4);
		c.spr_table = game_addr(game::unk_186CAB8);
		c.ptype_table = game_addr(game::unk_186CAE4);
		c.pop_table = game_addr(game::funcs_A9F457);
		c.draw_table = game_addr(game::GF_222Water_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_186CCC2);
		c.attr_184 = game_addr(game::unk_186CCDC);
		c.attr_186 = game_addr(game::unk_186CCDE);
		c.attr_194 = game_addr(game::unk_186CCEC);
		c.attr_19C = game_addr(game::unk_186CCF4);
		c.desc = game_addr(game::GF_222Water_ResourceDesc);
		c.ptr_block = game_addr(game::GF_222Water_PtrBlock);
		c.queue = game_addr(game::g_GF222Water_TaskQueue);
		c.file00 = game_addr(game::g_GF222Water_File00);
		c.file01 = game_addr(game::g_GF222Water_File01);
		c.tick = game_addr(game::MAG_222_WATER_CinematicRootTick);
		c.load_cb = game_addr(game::GF_222Water_LoadBattleFileDone);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static VoidFn g_save_vm[0x200], g_save_big[256], g_save_draw[256];

	static void apply_draw38(Clone &c)
	{
		ff8fx::copy<(sizeof(g_save_vm))>(g_save_vm, c.vm);
		ff8fx::copy<(sizeof(g_save_big))>(g_save_big, c.bigtab);
		ff8fx::copy<(sizeof(g_save_draw))>(g_save_draw, c.draw);
		apply_brothers(c);
		VoidFn d38 = c.draw[38];
		ff8fx::copy<(sizeof(g_save_vm))>(c.vm, g_save_vm);
		ff8fx::copy<(sizeof(g_save_big))>(c.bigtab, g_save_big);
		ff8fx::copy<(sizeof(g_save_draw))>(c.draw, g_save_draw);
		c.draw[38] = d38;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_water);
	}
}
}

	void register_mag222_water()
	{
		using namespace gfc;
		water::describe(water::g_water);
		init_clone(water::g_water, nullptr, 0);
		apply_shared_misc(water::g_water);
		water::apply_draw38(water::g_water);
		register_port(game::MAG_222_WATER_CinematicRootTick, (void *)water::SequenceTask, "MAG222 Water SequenceTick", 222);
	}
}

