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
namespace fart
{
	static Clone g_c = {};

	static void describe(Clone &c)
	{
		c.name = "234 Fart";
		c.effect_id = 234;
		c.vm_table = game_addr(game::GF_234Fart_VmOpcodeTable);
		c.bone_table = game_addr(game::GF_234Fart_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_A4B314);
		c.prim_size = game_addr(game::unk_1867B68);
		c.spr_table = game_addr(game::unk_1867B7C);
		c.ptype_table = game_addr(game::unk_1867BA8);
		c.pop_table = game_addr(game::funcs_A4BFC7);
		c.draw_table = game_addr(game::GF_234Fart_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_1867D86);
		c.attr_184 = game_addr(game::unk_1867DA0);
		c.attr_186 = game_addr(game::unk_1867DA2);
		c.attr_194 = game_addr(game::unk_1867DB0);
		c.attr_19C = game_addr(game::unk_1867DB8);
		c.desc = game_addr(game::GF_234Fart_ResourceDesc);
		c.ptr_block = game_addr(game::unk_1867A08);
		c.queue = game_addr(game::unk_27969E8);
		c.file00 = game_addr(game::unk_27969FC);
		c.file01 = game_addr(game::unk_27969F8);
		c.tick = game_addr(game::GF_234Fart_SequenceTick);
		c.load_cb = game_addr(game::GF_234Fart_LoadCallback);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static inline void xl_RotTransIR_56C850(const void *v, void *out, void *flag) { x::f<void (__cdecl *)(const void *, void *, void *)>(game_addr(game::GTE_RotTrans))(v, out, flag); }
	static inline int32_t xl_Project_56C880(const void *v, void *sxy, void *p, void *flag) { return x::f<int32_t (__cdecl *)(const void *, void *, void *, void *)>(game_addr(game::TransformWorldCoordinateToProjectedSpace))(v, sxy, p, flag); }
	static inline uint32_t xl_RenderProjectedObject_5106E0(void *desc, uint32_t ot, int32_t mode, uint32_t cursor) { return x::f<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleModel_RenderProjectedObject))(desc, ot, mode, cursor); }
	static inline uint32_t xl_Render_5088A0(void *ent, uint32_t ot, int32_t mode, uint32_t cursor) { return x::Render_5088A0(ent, ot, mode, cursor); }

	static void SyncEntityModelCopy(uint8_t *blk)
	{
		uint8_t *src = PTR(blk, 0xC) + 0x60;
		uint8_t *d = blk + 0x10;
		uint8_t *d2 = blk + 0x30;
		U32(d, 0) = U32(src, 0);
		U32(d, 4) = U32(src, 4);
		U32(d, 8) = U32(src, 8);
		uint8_t *m = PTR(src, 4);
		U32(d2, 0) = U32(m, 0);
		U32(d2, 4) = U32(m, 4);
		U32(d2, 8) = U32(m, 8);
		U32(d2, 0xC) = U32(m, 0xC);
	}

	static uint8_t *CreateEntityModelCopy(uint8_t *ent)
	{
		uint8_t *blk = blob::ArenaAlloc(0x80);
		PTR(blk, 0xC) = ent;
		SyncEntityModelCopy(blk);
		U32(blk, 0x54) = 0;
		U32(blk, 0x58) = 0xD00140;
		uint32_t t = U32(ent, 0x28);
		U32(blk, 0x78) = t;
		U32(blk, 0x5C) = t;
		t = U32(ent, 0x7C);
		U16(blk, 0x64) = 0;
		U32(blk, 0x60) = t;
		U32(blk, 0x68) = 0;
		U32(blk, 0x50) = 0xFFFFFFFF;
		U16(blk, 0) = 0;
		U16(blk, 2) = 0;
		U8(blk, 0x21) = 0;
		return blk;
	}

	static void ProjectWobbleGroup(uint8_t *s, uint8_t *out, uint8_t *table)
	{
		uint8_t *rec = out;
		int32_t n = S16(s, 0);
		s += 2;
		if (n > 0)
		{
			for (int32_t k = n; k > 0; k--)
			{
				U16(WS(), 0xD0) = U16(s, 0);
				U16(WS(), 0xD2) = U16(s, 2);
				U16(WS(), 0xD4) = U16(s, 4);
				uint8_t *w = WS();
				xl_RotTransIR_56C850(w + 0xD0, out, w + 0xFC);
				uint32_t i = ((uint32_t)U16(out, 2) >> 2) & 0xFF;
				s += 6;
				out += 8;
				U16(out, -8) = (uint16_t)(U16(out, -8) + U16(table, i * 2));
				U16(out, -4) = (uint16_t)(U16(out, -4) + U16(table, i * 2 + 0x400));
			}
		}
		x::SetRotMatrix(BBMAT());
		x::SetTransVector((const void *)game_addr(game::unk_27971E4));
		if (n > 0)
		{
			for (int32_t k = n; k > 0; k--)
			{
				uint8_t *flag = WS() + 0xFC;
				int32_t z = xl_Project_56C880(rec, rec, flag, flag);
				S32(rec, 4) = (int32_t)((uint32_t)z << 2);
				rec += 8;
			}
		}
		PTR(WS(), 0xFC) = rec;
		PTR(WS(), 0xF8) = s;
	}

	static void DrawWobbleModel(uint8_t *model, uint8_t *desc)
	{
		PTR(desc, 4) = PTR(CTX(), 0x74) + 0x800;
		uint8_t *mats = PTR(model, 0) + 0x10;
		uint8_t *tab = PTR(model, 4);
		int32_t nobj = S32(tab, 0);
		tab += 4;
		x::GteSetBackColor3(U8(desc, 0x1C), U8(desc, 0x1D), U8(desc, 0x1E));
		for (int32_t o = 0; o < nobj; o++)
		{
			uint8_t *s = PTR(model, 4) + U32(tab, 0);
			tab += 4;
			if (!(U32(desc, 0x20) & (1u << (o & 31)))) continue;
			int32_t ng = S16(s, 0);
			s += 2;
			uint8_t *out = PTR(desc, 4);
			for (; ng > 0; ng--)
			{
				uint8_t *m = mats + S16(s, 0) * 48 + 0x10;
				x::SetRotMatrix(m);
				x::SetTransVector(m);
				s += 2;
				ProjectWobbleGroup(s, out, PTR(CTX(), 0x74));
				s = PTR(WS(), 0xF8);
				out = PTR(WS(), 0xFC);
			}
			s = (uint8_t *)(((uint32_t)s + 3) & ~3u);
			U16(desc, 8) = U16(s, 0);
			U16(desc, 0xA) = U16(s, 2);
			U16(desc, 0xC) = U16(s, 4);
			U16(desc, 0xE) = U16(s, 6);
			PTR(desc, 0) = s + 0xC;
			U32(CTX(), 0x7C) = xl_RenderProjectedObject_5106E0(desc, U32(RT(), 0x4C), 4, U32(CTX(), 0x7C));
		}
	}

	static void __cdecl dh_30_WobbleEntity()
	{
		if (U32(CUR(), 0xBC) == 0)
		{
			uint8_t *prm = PTR(CUR(), 0xB8);
			PTR(CUR(), 0xC0) = blob::ArenaAlloc(0x10);
			uint8_t *d = PTR(CUR(), 0xC0);
			U16(d, 0) = 0;
			uint16_t fl = U16(prm, 0);
			U16(d, 2) = fl;
			U16(d, 6) = U16(prm, 4);
			if (fl & 1)
				PTR(CUR(), 0xBC) = h_B26860(S16(prm, 2));
			else
			{
				uint8_t *ent = PTR(SCENE(), 0x60 + 4 * (int32_t)U8(CUR(), 0x1B));
				PTR(CUR(), 0xBC) = CreateEntityModelCopy(ent);
				U8(ent, 0) |= 4;
			}
		}
		uint8_t *blk = PTR(CUR(), 0xBC);
		uint8_t *d = PTR(CUR(), 0xC0);
		uint8_t *ent = PTR(blk, 0xC);
		uint8_t *model = blk + 0x30;
		uint8_t *desc = blk + 0x40;
		uint8_t *hdr = blk + 0x10;
		uint8_t *weapon = nullptr;
		if (U8(d, 2) & 1)
		{
			h_B269E0();
			h_B26AD0(PTR(CUR(), 0xBC));
		}
		else
		{
			SyncEntityModelCopy(blk);
			weapon = PTR(ent, 0x78);
			x::ComposeAffine(NODEMAT(), ent + 0x40, WS() + 0xE0);
			if (weapon) x::ComputeBonesWorldMatrices(weapon, WS() + 0xE0);
			x::ComputeBonesWorldMatrices(hdr, WS() + 0xE0);
			U32(CTX(), 0x7C) = xl_Render_5088A0(ent, FRAMERL() + 0x4064, 0x1F, U32(CTX(), 0x7C));
		}
		uint8_t *tab = PTR(CTX(), 0x74);
		uint8_t *node = blob::GetBone(S16(d, 6));
		int32_t step = S16(node, 0x98);
		int32_t amp = S16(node, 0x94);
		int32_t phase = S16(node, 0x96);
		for (int32_t k = 0x100; k != 0; k--)
		{
			U16(tab, 0) = (uint16_t)(mul32(x::Sin(phase), amp) >> 12);
			U16(tab, 0x400) = (uint16_t)(mul32(x::Cos(phase), amp) >> 12);
			phase = add32(phase, step);
			tab += 2;
		}
		DrawWobbleModel(model, desc);
		if (weapon) DrawWobbleModel(PTR(weapon, 4), desc);
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag234_fart()
	{
		using namespace gfc;
		fart::describe(fart::g_c);
		init_clone(fart::g_c, nullptr, 0);
		fart::g_c.draw[30] = fart::dh_30_WobbleEntity;
		register_port(game::GF_234Fart_SequenceTick, (void *)fart::SequenceTask, "MAG234 Fart SequenceTick", 234);
	}
}

