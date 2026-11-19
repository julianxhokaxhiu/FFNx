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
namespace breath238
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace breath238
{
	static void describe(Clone &c)
	{
		c.name = "238 Breath (Ruby Dragon)";
		c.effect_id = 238;
		c.vm_table = game_addr(game::funcs_A296EC);
		c.bone_table = game_addr(game::funcs_A26247);
		c.prim_table = game_addr(game::funcs_A22194);
		c.prim_size = game_addr(game::unk_18653D8);
		c.spr_table = game_addr(game::unk_18653EC);
		c.ptype_table = game_addr(game::unk_1865418);
		c.pop_table = game_addr(game::funcs_A23C37);
		c.draw_table = game_addr(game::unk_186548C);
		c.attr_16A = game_addr(game::unk_18655F6);
		c.attr_184 = game_addr(game::unk_1865610);
		c.attr_186 = game_addr(game::unk_1865612);
		c.attr_194 = game_addr(game::unk_1865620);
		c.attr_19C = game_addr(game::unk_1865628);
		c.desc = game_addr(game::unk_1865154);
		c.ptr_block = game_addr(game::unk_1865278);
		c.queue = game_addr(game::unk_2796910);
		c.file00 = game_addr(game::unk_279690C);
		c.file01 = game_addr(game::unk_2796908);
		c.tick = game_addr(game::MAG_238_sub_A20430);
		c.load_cb = game_addr(game::MAG_238_sub_A25630);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static void __cdecl op_094_JumpIfBoneNear()
	{
		const uint8_t *b = blob::GetBone((int32_t)S16(STREAM(), 2));
		const int32_t bx = S16(b, 0x94);
		const int32_t dx = S16(STREAM(), 4), dz = S16(STREAM(), 6);
		const int32_t cx = S16(CUR(), 0x94);
		if (bx >= cx - dx && bx <= cx + dx)
		{
			const int32_t bz = S16(b, 0x98), cz = S16(CUR(), 0x98);
			if (bz >= cz - dz && bz <= cz + dz)
			{
				STREAM() += (int32_t)S16(STREAM(), 8);
				return;
			}
		}
		STREAM() += 0xA;
	}

	static VoidFn g_save_vm[0x200], g_save_big[256], g_save_draw[256];

	static void apply_slots(Clone &c)
	{
		ff8fx::copy<(sizeof(g_save_vm))>(g_save_vm, c.vm);
		ff8fx::copy<(sizeof(g_save_big))>(g_save_big, c.bigtab);
		ff8fx::copy<(sizeof(g_save_draw))>(g_save_draw, c.draw);
		apply_shared_misc(c);
		const VoidFn p16 = c.prim[16];
		ff8fx::copy<(sizeof(g_save_vm))>(c.vm, g_save_vm);
		ff8fx::copy<(sizeof(g_save_big))>(c.bigtab, g_save_big);
		ff8fx::copy<(sizeof(g_save_draw))>(c.draw, g_save_draw);
		c.prim[16] = p16;
		c.vm[0x094] = op_094_JumpIfBoneNear;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag238_breath()
	{
		using namespace gfc;
		breath238::describe(breath238::g_c);
		init_clone(breath238::g_c, nullptr, 0);
		breath238::apply_slots(breath238::g_c);
		register_port(game::MAG_238_sub_A20430, (void *)breath238::SequenceTask, "MAG238 Breath (Ruby Dragon) SequenceTick", 238);
	}
}

