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
namespace eff248
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace eff248
{
	static void describe(Clone &c)
	{
		c.name = "248 (no kernel.bin attack)";
		c.effect_id = 248;
		c.vm_table = game_addr(game::funcs_9C934C);
		c.bone_table = game_addr(game::funcs_9C5EA7);
		c.prim_table = game_addr(game::funcs_9C2B94);
		c.prim_size = game_addr(game::unk_185F10C);
		c.spr_table = game_addr(game::unk_185F120);
		c.ptype_table = game_addr(game::unk_185F14C);
		c.pop_table = game_addr(game::funcs_9C3847);
		c.draw_table = game_addr(game::unk_185F1C0);
		c.attr_16A = game_addr(game::unk_185F32A);
		c.attr_184 = game_addr(game::unk_185F344);
		c.attr_186 = game_addr(game::unk_185F346);
		c.attr_194 = game_addr(game::unk_185F354);
		c.attr_19C = game_addr(game::unk_185F35C);
		c.desc = game_addr(game::unk_185EEFC);
		c.ptr_block = game_addr(game::unk_185EFAC);
		c.queue = game_addr(game::unk_27966D8);
		c.file00 = game_addr(game::unk_27966E8);
		c.file01 = game_addr(game::unk_27966EC);
		c.tick = game_addr(game::MAG_248_sub_9C0F40);
		c.load_cb = game_addr(game::MAG_248_sub_9C5240);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static void __cdecl op_09A_SetTargetList()
	{
		const uint32_t n = (uint32_t)U16(RT(), 0x4A) >> 12;
		U8(SCENE(), 0x41) = (uint8_t)n;
		STREAM() += 2;
		for (uint32_t i = 0; i < n; i++)
		{
			U8(SCENE(), 0x48 + i) = U8(STREAM(), 0);
			STREAM() += 2;
		}
	}

	static VoidFn g_save_vm[0x200], g_save_big[256], g_save_draw[256];

	static void apply_slots(Clone &c)
	{
		ff8fx::copy<(sizeof(g_save_vm))>(g_save_vm, c.vm);
		ff8fx::copy<(sizeof(g_save_big))>(g_save_big, c.bigtab);
		ff8fx::copy<(sizeof(g_save_draw))>(g_save_draw, c.draw);
		apply_shared_misc(c);
		const VoidFn v042 = c.vm[0x042];
		ff8fx::copy<(sizeof(g_save_vm))>(c.vm, g_save_vm);
		ff8fx::copy<(sizeof(g_save_big))>(c.bigtab, g_save_big);
		ff8fx::copy<(sizeof(g_save_draw))>(c.draw, g_save_draw);
		c.vm[0x042] = v042;
		c.vm[0x09A] = op_09A_SetTargetList;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag248_unknown_248()
	{
		using namespace gfc;
		eff248::describe(eff248::g_c);
		init_clone(eff248::g_c, nullptr, 0);
		eff248::apply_slots(eff248::g_c);
		register_port(game::MAG_248_sub_9C0F40, (void *)eff248::SequenceTask, "MAG248 effect 248 SequenceTick", 248);
	}
}

