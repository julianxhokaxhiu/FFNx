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

// Effect 230: Fire Breath

#include "gfc_engine.h"

namespace ff8fx
{
namespace gfc
{
namespace fire_breath_q
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace fire_breath_q
{
	static void describe(Clone &c)
	{
		c.name = "230 Fire Breath (Quistis)";
		c.effect_id = 230;
		c.vm_table = game_addr(game::funcs_A7A4DC);
		c.bone_table = game_addr(game::funcs_A77037);
		c.prim_table = game_addr(game::funcs_A72F84);
		c.prim_size = game_addr(game::unk_186A2B0);
		c.spr_table = game_addr(game::unk_186A2C4);
		c.ptype_table = game_addr(game::unk_186A2F0);
		c.pop_table = game_addr(game::funcs_A74A27);
		c.draw_table = game_addr(game::unk_186A364);
		c.attr_16A = game_addr(game::unk_186A4CE);
		c.attr_184 = game_addr(game::unk_186A4E8);
		c.attr_186 = game_addr(game::unk_186A4EA);
		c.attr_194 = game_addr(game::unk_186A4F8);
		c.attr_19C = game_addr(game::unk_186A500);
		c.desc = game_addr(game::unk_186A02C);
		c.ptr_block = game_addr(game::unk_186A150);
		c.queue = game_addr(game::unk_2796AC8);
		c.file00 = game_addr(game::unk_2796ADC);
		c.file01 = game_addr(game::unk_2796AD8);
		c.tick = game_addr(game::MAG_230_FIRE_BREATH_QUISTIS_SequenceTick);
		c.load_cb = game_addr(game::MAG_230_FIRE_BREATH_QUISTIS_LoadFileDoneCallback);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static inline void BindEntityModelFromComData(uint8_t *entity, uint8_t *out, uint32_t data)
	{
		x::f<void (__cdecl *)(uint8_t *, uint8_t *, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data);
	}
	static inline void QueueChainTransformation(uint8_t *entity, int32_t id)
	{
		x::f<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, id);
	}

	static constexpr game::Id COM_ANIM = game::MAG_230_FIRE_BREATH_AnimSet;

	static void __cdecl op_0AB_BindEntityAnimSet()
	{
		uint8_t *ent = PTR(SCENE(), 0x60 + 4 * (int32_t)U8(CUR(), 0x1B));
		uint8_t *blk = blob::ArenaAlloc(0x20);
		BindEntityModelFromComData(ent, blk, game_addr(COM_ANIM));
		QueueChainTransformation(ent, (int32_t)S16(STREAM(), 2));
		STREAM() += 4;
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
		c.vm[0x0AB] = op_0AB_BindEntityAnimSet;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag230_fire_breath_quistis()
	{
		using namespace gfc;
		fire_breath_q::describe(fire_breath_q::g_c);
		init_clone(fire_breath_q::g_c, nullptr, 0);
		fire_breath_q::apply_slots(fire_breath_q::g_c);
		register_port(game::MAG_230_FIRE_BREATH_QUISTIS_SequenceTick, (void *)fire_breath_q::SequenceTask, "MAG230 Fire Breath (Quistis) SequenceTick", 230);
	}
}

