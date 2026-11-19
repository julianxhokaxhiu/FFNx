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
namespace anchor263
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace anchor263
{
	static void describe(Clone &c)
	{
		c.name = "263 Massive Anchor";
		c.effect_id = 263;
		c.vm_table = game_addr(game::MAG263_VmOpcodeTable);
		c.bone_table = game_addr(game::MAG263_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_947C64);
		c.prim_size = game_addr(game::unk_1857A18);
		c.spr_table = game_addr(game::unk_1857A2C);
		c.ptype_table = game_addr(game::unk_1857A58);
		c.pop_table = game_addr(game::funcs_949A77);
		c.draw_table = game_addr(game::MAG263_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_1857C36);
		c.attr_184 = game_addr(game::unk_1857C50);
		c.attr_186 = game_addr(game::unk_1857C52);
		c.attr_194 = game_addr(game::unk_1857C60);
		c.attr_19C = game_addr(game::unk_1857C68);
		c.desc = game_addr(game::MAG263_ResourceDesc);
		c.ptr_block = game_addr(game::MAG263_PtrBlock);
		c.queue = game_addr(game::g_MAG263_RootQueue);
		c.file00 = game_addr(game::g_MAG263_File00);
		c.file01 = game_addr(game::g_MAG263_File01);
		c.tick = game_addr(game::MAG_263_SequenceTick);
		c.load_cb = game_addr(game::MAG_263_LoadCb_ClearBusy);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static constexpr game::Id COMDATA = game::MAG263_LimitAnimSetComData;

	template<typename F> static inline F fx(uint32_t a) { return (F)a; }
	static inline void xl_BindEntityModelFromComData(void *ent, void *out, uint32_t data) { fx<void (__cdecl *)(void *, void *, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(ent, out, data); }
	static inline void xl_QueueChainTransformation(void *ent, int32_t id) { fx<void (__cdecl *)(void *, int32_t)>(game_addr(game::QueueChainTransformation))(ent, id); }

	static inline uint8_t *SlotEntity() { return PTR(SCENE(), 0x60 + 4 * (int32_t)U8(CUR(), 0x1B)); }

	static void __cdecl op_0AB_BindLimitAnimSet()
	{
		uint8_t *ent = SlotEntity();
		uint8_t *blk = blob::ArenaAlloc(0x20);
		xl_BindEntityModelFromComData(ent, blk, game_addr(COMDATA));
		xl_QueueChainTransformation(ent, S16(STREAM(), 2));
		STREAM() += 4;
	}

	static void __cdecl dh_70_PlaceWeaponAtBone()
	{
		uint8_t *w = PTR(SlotEntity(), 0x78);
		S16(w, 0x1C) = S16(CUR(), 0x94);
		S16(w, 0x1E) = S16(CUR(), 0x96);
		S16(w, 0x20) = S16(CUR(), 0x98);
	}

	static void apply_slots(Clone &c)
	{
		c.vm[0x0AB] = op_0AB_BindLimitAnimSet;
		c.draw[70] = dh_70_PlaceWeaponAtBone;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag263_massive_anchor()
	{
		using namespace gfc;
		anchor263::describe(anchor263::g_c);
		init_clone(anchor263::g_c, nullptr, 0);
		anchor263::apply_slots(anchor263::g_c);
		register_port(game::MAG_263_SequenceTick, (void *)anchor263::SequenceTask, "MAG263 Massive Anchor SequenceTick", 263);
	}
}

