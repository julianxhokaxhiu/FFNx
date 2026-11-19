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

// Effect 201: Ifrit - Hell Fire

#include "gfc_engine.h"

namespace ff8fx
{
namespace gfc
{
namespace ifrit
{
	static Clone g_ifrit = {};

	static void describe(Clone &c)
	{
		c.name = "201 Ifrit";
		c.effect_id = 201;
		c.vm_table = game_addr(game::GF_201Ifrit_VmOpcodeTable);
		c.bone_table = game_addr(game::GF_201Ifrit_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_B27DD4);
		c.prim_size = game_addr(game::unk_1874CB8);
		c.spr_table = game_addr(game::unk_1874CCC);
		c.ptype_table = game_addr(game::unk_1874CF8);
		c.pop_table = game_addr(game::funcs_B2A007);
		c.draw_table = game_addr(game::GF_201Ifrit_DrawHandlerTable);
		c.attr_16A = game_addr(game::GF_201Ifrit_VmOpcodeAttribTable);
		c.attr_184 = game_addr(game::GF_201Ifrit_VmFieldDescTable);
		c.attr_186 = game_addr(game::unk_1874EF2);
		c.attr_194 = game_addr(game::unk_1874F00);
		c.attr_19C = game_addr(game::unk_1874F08);
		c.desc = game_addr(game::unk_1874894);
		c.ptr_block = game_addr(game::unk_1874B58);
		c.queue = game_addr(game::unk_2796E18);
		c.file00 = game_addr(game::unk_2796E4C);
		c.file01 = game_addr(game::unk_2796E48);
		c.tick = game_addr(game::GF_Ifrit_seqBDlink);
		c.load_cb = game_addr(game::MAG_201_LoadCb_ClearBusy);
		c.lit_dispatcher = true;
		c.bone_count = 18;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_ifrit);
	}
}
}

	void register_gfc_ifrit()
	{
		using namespace gfc;
		ifrit::describe(ifrit::g_ifrit);
		init_clone(ifrit::g_ifrit, nullptr, 0);
		register_port(game::GF_Ifrit_seqBDlink, (void *)ifrit::SequenceTask, "GFC201 Ifrit SequenceTick", 201);
	}
}

