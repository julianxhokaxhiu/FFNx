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
namespace laser255
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace laser255
{
	static void describe(Clone &c)
	{
		c.name = "255 Homing Laser (attack 282)";
		c.effect_id = 255;
		c.vm_table = game_addr(game::funcs_98CC6C);
		c.bone_table = game_addr(game::funcs_989767);
		c.prim_table = game_addr(game::funcs_985BA4);
		c.prim_size = game_addr(game::unk_185B660);
		c.spr_table = game_addr(game::unk_185B674);
		c.ptype_table = game_addr(game::unk_185B6A0);
		c.pop_table = game_addr(game::funcs_987157);
		c.draw_table = game_addr(game::unk_185B714);
		c.attr_16A = game_addr(game::unk_185B87E);
		c.attr_184 = game_addr(game::unk_185B898);
		c.attr_186 = game_addr(game::unk_185B89A);
		c.attr_194 = game_addr(game::unk_185B8A8);
		c.attr_19C = game_addr(game::unk_185B8B0);
		c.desc = game_addr(game::unk_185B410);
		c.ptr_block = game_addr(game::unk_185B500);
		c.queue = game_addr(game::unk_2796580);
		c.file00 = game_addr(game::unk_2796590);
		c.file01 = game_addr(game::unk_2796594);
		c.tick = game_addr(game::MAG_255_sub_982CC0);
		c.load_cb = game_addr(game::MAG_255_sub_988B50);
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
		const bool weapon = (U8(RT(), 0x4B) & 0x80) != 0;
		uint8_t *hdr = weapon ? PTR(ent, 0x78) : ent + 0x60;
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

	static void apply_slots(Clone &c)
	{
		apply_shared_ribbon(c);
		c.vm[0x0AD] = op_0AD_PosFromPosedEntityJoint;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag255_homing_laser_282()
	{
		using namespace gfc;
		laser255::describe(laser255::g_c);
		init_clone(laser255::g_c, nullptr, 0);
		laser255::apply_slots(laser255::g_c);
		register_port(game::MAG_255_sub_982CC0, (void *)laser255::SequenceTask, "MAG255 Homing Laser (attack 282) SequenceTick", 255);
	}
}

