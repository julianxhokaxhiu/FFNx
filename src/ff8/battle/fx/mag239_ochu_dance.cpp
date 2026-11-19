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
	void apply_breath_glow(Clone &c);

namespace ochu_dance
{
	static Clone g_c = {};

	static void describe(Clone &c)
	{
		c.name = "239 Ochu Dance";
		c.effect_id = 239;
		c.vm_table = game_addr(game::MAG_239_VmOpcodeTable);
		c.bone_table = game_addr(game::MAG_239_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_A18AE4);
		c.prim_size = game_addr(game::unk_18649C4);
		c.spr_table = game_addr(game::unk_18649D8);
		c.ptype_table = game_addr(game::unk_1864A04);
		c.pop_table = game_addr(game::funcs_A19E47);
		c.draw_table = game_addr(game::MAG_239_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_1864BE2);
		c.attr_184 = game_addr(game::unk_1864BFC);
		c.attr_186 = game_addr(game::unk_1864BFE);
		c.attr_194 = game_addr(game::unk_1864C0C);
		c.attr_19C = game_addr(game::unk_1864C14);
		c.desc = game_addr(game::MAG_239_Desc);
		c.ptr_block = game_addr(game::MAG_239_PtrBlock);
		c.queue = game_addr(game::MAG_239_Queue);
		c.file00 = game_addr(game::unk_27968D4);
		c.file01 = game_addr(game::unk_27968D0);
		c.tick = game_addr(game::MAG_239_OchuDance_SequenceTick);
		c.load_cb = game_addr(game::MAG_239_LoadBattleFile_Done);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag239_ochu_dance()
	{
		using namespace gfc;
		ochu_dance::describe(ochu_dance::g_c);
		init_clone(ochu_dance::g_c, nullptr, 0);
		apply_breath_glow(ochu_dance::g_c);
		register_port(game::MAG_239_OchuDance_SequenceTick, (void *)ochu_dance::SequenceTask, "MAG239 Ochu Dance SequenceTick", 239);
	}
}

