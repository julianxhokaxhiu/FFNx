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
namespace twinlaser253
{
	static Clone g_c = {};
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace twinlaser253
{
	static void describe(Clone &c)
	{
		c.name = "253 Twin Homing Laser";
		c.effect_id = 253;
		c.vm_table = game_addr(game::funcs_9A232C);
		c.bone_table = game_addr(game::funcs_99EE27);
		c.prim_table = game_addr(game::funcs_99B264);
		c.prim_size = game_addr(game::unk_185CA30);
		c.spr_table = game_addr(game::unk_185CA44);
		c.ptype_table = game_addr(game::unk_185CA70);
		c.pop_table = game_addr(game::funcs_99C817);
		c.draw_table = game_addr(game::unk_185CAE4);
		c.attr_16A = game_addr(game::unk_185CC4E);
		c.attr_184 = game_addr(game::unk_185CC68);
		c.attr_186 = game_addr(game::unk_185CC6A);
		c.attr_194 = game_addr(game::unk_185CC78);
		c.attr_19C = game_addr(game::unk_185CC80);
		c.desc = game_addr(game::unk_185C7D0);
		c.ptr_block = game_addr(game::unk_185C8D0);
		c.queue = game_addr(game::unk_27965F8);
		c.file00 = game_addr(game::unk_27965F0);
		c.file01 = game_addr(game::unk_27965F4);
		c.tick = game_addr(game::MAG_253_sub_998380);
		c.load_cb = game_addr(game::MAG_253_sub_99E210);
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

	void register_mag253_twin_homing_laser()
	{
		using namespace gfc;
		twinlaser253::describe(twinlaser253::g_c);
		init_clone(twinlaser253::g_c, nullptr, 0);
		twinlaser253::apply_slots(twinlaser253::g_c);
		register_port(game::MAG_253_sub_998380, (void *)twinlaser253::SequenceTask, "MAG253 Twin Homing Laser SequenceTick", 253);
	}
}

