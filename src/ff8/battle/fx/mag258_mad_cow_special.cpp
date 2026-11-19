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
namespace mad_cow_special
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
	void apply_bahamut(Clone &c);
	void apply_eden_c(Clone &c);

namespace mad_cow_special
{
	static void describe(Clone &c)
	{
		c.name = "258 Mad Cow Special";
		c.effect_id = 258;
		c.vm_table = game_addr(game::MAG_258_VmOpcodeTable);
		c.bone_table = game_addr(game::MAG_258_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_967924);
		c.prim_size = game_addr(game::unk_18598D4);
		c.spr_table = game_addr(game::unk_18598E8);
		c.ptype_table = game_addr(game::unk_1859914);
		c.pop_table = game_addr(game::funcs_969207);
		c.draw_table = game_addr(game::MAG_258_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_1859AF2);
		c.attr_184 = game_addr(game::unk_1859B0C);
		c.attr_186 = game_addr(game::unk_1859B0E);
		c.attr_194 = game_addr(game::unk_1859B1C);
		c.attr_19C = game_addr(game::unk_1859B24);
		c.desc = game_addr(game::MAG_258_ResourceDesc);
		c.ptr_block = game_addr(game::unk_1859774);
		c.queue = game_addr(game::MAG_258_TaskQueue);
		c.file00 = game_addr(game::MAG_258_File00);
		c.file01 = game_addr(game::MAG_258_File01);
		c.tick = game_addr(game::MAG_258_SequenceTick);
		c.load_cb = game_addr(game::MAG_258_LoadBattleFileDone);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static inline uint8_t *xl_JointMatrix_B66B80(void *hdr, int32_t joint, int32_t scale, void *m)
	{
		return x::f<uint8_t *(__cdecl *)(void *, int32_t, int32_t, void *)>(game_addr(game::GfCinematic_ModelJointMatrix))(hdr, joint, scale, m);
	}

	static inline uint8_t *SlotEntity() { return PTR(SCENE(), 0x60 + 4 * (int32_t)U8(CUR(), 0x1B)); }

	static void __cdecl op_0AD_PosFromPosedEntityJoint()
	{
		uint8_t *ent = SlotEntity();
		uint8_t *hdr = (U8(RT(), 0x4B) & 0x80) ? PTR(ent, 0x78) : ent + 0x60;
		x::BuildBoneMatricesFromPose(hdr);
		uint8_t *m = xl_JointMatrix_B66B80(hdr, S16(STREAM(), 2), 0x1000, ent + 0x40);
		x::SetRotMatrix(m);
		x::SetTransVector(m);
		uint8_t *w = WS();
		x::TransformToCamera(CUR() + 0x94, w + 0xF0, w + 0xFC);
		S32(CUR(), 0x5C) = shl32(S32(WS(), 0xF0), 16);
		S32(CUR(), 0x60) = shl32(S32(WS(), 0xF4), 16);
		S32(CUR(), 0x64) = shl32(S32(WS(), 0xF8), 16);
		C().bone[U8(CUR(), 0x18)]();
		STREAM() += 4;
	}

	static VoidFn g_save_vm[0x200], g_save_big[256], g_save_draw[256];

	static void save_tables(const Clone &c)
	{
		ff8fx::copy<(sizeof(g_save_vm))>(g_save_vm, c.vm);
		ff8fx::copy<(sizeof(g_save_big))>(g_save_big, c.bigtab);
		ff8fx::copy<(sizeof(g_save_draw))>(g_save_draw, c.draw);
	}
	static void restore_tables(Clone &c)
	{
		ff8fx::copy<(sizeof(g_save_vm))>(c.vm, g_save_vm);
		ff8fx::copy<(sizeof(g_save_big))>(c.bigtab, g_save_big);
		ff8fx::copy<(sizeof(g_save_draw))>(c.draw, g_save_draw);
	}

	static void apply_slots(Clone &c)
	{
		save_tables(c);
		apply_bahamut(c);
		const VoidFn d43 = c.draw[43];
		restore_tables(c);
		apply_eden_c(c);
		const VoidFn d53 = c.draw[53];
		restore_tables(c);
		apply_shared_misc(c);
		c.draw[43] = d43;
		c.draw[53] = d53;
		c.vm[0x0AD] = op_0AD_PosFromPosedEntityJoint;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag258_mad_cow_special()
	{
		using namespace gfc;
		mad_cow_special::describe(mad_cow_special::g_c);
		init_clone(mad_cow_special::g_c, nullptr, 0);
		mad_cow_special::apply_slots(mad_cow_special::g_c);
		register_port(game::MAG_258_SequenceTick, (void *)mad_cow_special::SequenceTask, "MAG258 Mad Cow Special SequenceTick", 258);
	}
}

