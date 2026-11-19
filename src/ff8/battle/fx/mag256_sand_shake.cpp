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
namespace sand_shake
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace sand_shake
{
	static void describe(Clone &c)
	{
		c.name = "256 Sand Shake";
		c.effect_id = 256;
		c.vm_table = game_addr(game::MAG_256_VmOpcodeTable);
		c.bone_table = game_addr(game::MAG_256_BoneHandlerTable);
		c.prim_table = game_addr(game::MAG_256_PrimHandlerTable);
		c.prim_size = game_addr(game::MAG_256_PrimSizeTable);
		c.spr_table = game_addr(game::MAG_256_SpriteOrientTable);
		c.ptype_table = game_addr(game::MAG_256_ParticleTypeTable);
		c.pop_table = game_addr(game::MAG_256_ParticleOpTable);
		c.draw_table = game_addr(game::MAG_256_DrawHandlerTable);
		c.attr_16A = game_addr(game::MAG_256_Attr16A);
		c.attr_184 = game_addr(game::MAG_256_Attr184);
		c.attr_186 = game_addr(game::MAG_256_Attr186);
		c.attr_194 = game_addr(game::MAG_256_Attr194);
		c.attr_19C = game_addr(game::MAG_256_Attr19C);
		c.desc = game_addr(game::MAG_256_DescBlock);
		c.ptr_block = game_addr(game::MAG_256_PtrBlock);
		c.queue = game_addr(game::MAG_256_TaskQueue);
		c.file00 = game_addr(game::MAG_256_File00);
		c.file01 = game_addr(game::MAG_256_File01);
		c.tick = game_addr(game::MAG_256_SandShake_SequenceTick);
		c.load_cb = game_addr(game::MAG_256_LoadFileDoneCallback);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag256_sand_shake()
	{
		using namespace gfc;
		sand_shake::describe(sand_shake::g_c);
		init_clone(sand_shake::g_c, nullptr, 0);
		apply_shared_misc(sand_shake::g_c);
		register_port(game::MAG_256_SandShake_SequenceTick, (void *)sand_shake::SequenceTask, "MAG256 Sand Shake SequenceTick", 256);
	}
}

