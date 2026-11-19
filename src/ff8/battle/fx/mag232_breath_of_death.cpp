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

// Effect 232: Breath of Death

#include "gfc_engine.h"

namespace ff8fx
{
namespace gfc
{
namespace death232
{
	static Clone g_c = {};

	static void describe(Clone &c)
	{
		c.name = "232 Breath of Death";
		c.effect_id = 232;
		c.vm_table = game_addr(game::GF_232BreathOfDeath_VmOpcodeTable);
		c.bone_table = game_addr(game::GF_232BreathOfDeath_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_A5EB84);
		c.prim_size = game_addr(game::unk_1868EC0);
		c.spr_table = game_addr(game::unk_1868ED4);
		c.ptype_table = game_addr(game::unk_1868F00);
		c.pop_table = game_addr(game::funcs_A604A7);
		c.draw_table = game_addr(game::GF_232BreathOfDeath_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_18690DE);
		c.attr_184 = game_addr(game::unk_18690F8);
		c.attr_186 = game_addr(game::unk_18690FA);
		c.attr_194 = game_addr(game::unk_1869108);
		c.attr_19C = game_addr(game::unk_1869110);
		c.desc = game_addr(game::GF_232BreathOfDeath_Desc);
		c.ptr_block = game_addr(game::GF_232BreathOfDeath_PtrBlock);
		c.queue = game_addr(game::GF_232BreathOfDeath_Queue);
		c.file00 = game_addr(game::unk_2796A6C);
		c.file01 = game_addr(game::unk_2796A68);
		c.tick = game_addr(game::GF_232BreathOfDeath_SequenceTick);
		c.load_cb = game_addr(game::GF_232BreathOfDeath_LoadCallback);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}
}
}
}

namespace ff8fx
{
namespace gfc
{
namespace death232
{
	inline uint32_t &GteData(int reg) { return MEM<uint32_t>(game_addr(game::GTE_VXY0) + 4 * reg); }

	static void __cdecl bh_10_ArcBetweenBones()
	{
		uint8_t *refs = PTR(CUR(), 0xA0);
		uint8_t *a = blob::GetBone(S16(refs, 0));
		uint8_t *b = blob::GetBone(S16(refs, 2));
		int32_t rad = S16(refs, 4);
		int32_t ang = blob::Blob_B66B30(S16(b, 0x94), S16(b, 0x98), S16(a, 0x94), S16(a, 0x98));
		int32_t pz = add32(mul32(x::Cos(ang), rad) >> 12, S16(b, 0x98));
		int32_t px = add32(mul32(x::Sin(ang), rad) >> 12, S16(b, 0x94));
		int32_t ax = S16(a, 0x94);
		uint8_t *bone = CUR();
		int32_t d = mul32(sub32(px, ax), S32(bone, 0x50) >> 16) / 256;
		S16(bone, 0x94) = (int16_t)add32(S32(bone, 0x5C) >> 16, add32(ax, d));
		int32_t ay = S16(a, 0x96);
		int32_t by = S16(b, 0x96);
		bone = CUR();
		d = mul32(sub32(by, ay), S32(bone, 0x54) >> 16) / 256;
		S16(bone, 0x96) = (int16_t)add32(S32(bone, 0x60) >> 16, add32(ay, d));
		int32_t az = S16(a, 0x98);
		bone = CUR();
		d = mul32(sub32(pz, az), S32(bone, 0x58) >> 16) / 256;
		S16(bone, 0x98) = (int16_t)add32(S32(bone, 0x64) >> 16, add32(az, d));
	}

	static inline int32_t DepthKey(uint32_t off)
	{
		uint32_t v = DEPTHARR()[(int32_t)off / 8] & 0xFFFF;
		return (int32_t)(int64_t)((double)v * (double)FLT_1877DA8());
	}

	static void __cdecl pr_58_TintedFT4Abr()
	{
		h_B29450((int32_t)U32(WS(), 0x84));
		uint8_t *ws = WS();
		uint8_t *pkt = PTR(ws, 0x60);
		uint8_t *rec = PTR(ws, 0x6C);
		int32_t count = S32(ws, 0x70);
		uint32_t ot = U32(ws, 0x5C);
		U32(ws, 0xF0) = 0;
		uint32_t nocull = U32(ws, 0x90) & 0x10;
		do
		{
			uint32_t o0 = U16(rec, 0xC);
			uint32_t o1 = U16(rec, 0xE);
			uint32_t o2 = U16(rec, 0x10);
			uint32_t o3 = U16(rec, 0x12);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			int32_t z3 = DepthKey(o3);
			uint8_t *vb = PTR(ws, 0x7C);
			uint8_t *p0 = vb + o0;
			uint8_t *p1 = vb + o1;
			uint8_t *p2 = vb + o2;
			uint8_t *p3 = vb + o3;
			uint32_t s0 = U32(p0, 0);
			uint32_t s1 = U32(p1, 0);
			uint32_t s2 = U32(p2, 0);
			uint32_t s3 = U32(p3, 0);
			U32(pkt, 8) = s0;
			U32(pkt, 0x18) = s2;
			U32(pkt, 0x20) = s3;
			U32(pkt, 0x10) = s1;
			if (nocull == 0)
			{
				GteData(12) = s0;
				GteData(13) = s1;
				GteData(14) = s2;
				x::GteNCLIP();
				if ((int32_t)GteData(24) < 0) goto next;
			}
			{
				uint32_t a0 = U32(p0, 4), a2 = U32(p2, 4), a1 = U32(p1, 4), a3 = U32(p3, 4);
				if ((a3 | a2 | a1 | a0) & 0x460000) goto next;
				U8(pkt, 3) = 0xA;
				U32(pkt, 0x28) = 0xE1000220;
				uint32_t slot = (((a3 + a2 + a1 + a0) >> 2) & 0x3FFC) + ot;
				U16(pkt, 0x16) = (uint16_t)(U32(ws, 0x50) | U16(rec, 0x16));
				U16(pkt, 0xE) = (uint16_t)(U16(rec, 0x14) + (uint16_t)U32(ws, 0x54));
				uint32_t uvofs = U16(ws, 0x98);
				uint32_t uv0 = U16(rec, 4) + uvofs;
				uint32_t uv1 = U16(rec, 6) + uvofs;
				uint32_t uv2 = U16(rec, 8) + uvofs;
				uint32_t uv3 = U16(rec, 0xA) + uvofs;
				U16(pkt, 0xC) = (uint16_t)uv0;
				U16(pkt, 0x24) = (uint16_t)uv3;
				U16(pkt, 0x14) = (uint16_t)uv1;
				U16(pkt, 0x1C) = (uint16_t)uv2;
				GteData(6) = U32(ws, 0x8C) | U32(rec, 0) | 0x2C000000;
				x::Gte_4601B0();
				x::InsertPrimDepthKeys(slot, pkt, z0, z1, z2, z3);
				U32(pkt, 4) = GteData(22);
				pkt += 0x2C;
			}
		next:
			rec += 0x18;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}

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

	static void SetupParentXformScaled()
	{
		Mat4x3 *m = blob::GetParentMatrix(U16(CUR(), 0x9C));
		x::GteSetRotMatrix(m);
		x::GteSetTransVector(m);
		x::GteLoadV0(CUR() + 0x94);
		x::GteMVMVA_RotV0Tr();
		x::Gte_45E580();
		S32(WS(), 0xD4) = shl32(S16(CUR(), 0x8C), 4);
		S32(WS(), 0xD8) = shl32(S16(CUR(), 0x8E), 4);
		S32(WS(), 0xDC) = shl32(S16(CUR(), 0x90), 4);
		uint8_t *d = WS() + 0xE0;
		for (int i = 0; i < 8; i++) U32(d, i * 4) = U32(m, i * 4);
		x::ScaleMatrix(d, WS() + 0xD4);
		x::GteSetRotMatrix(d);
	}

	static void __cdecl dh_02_MeshScaled()
	{
		U32(WS(), 0x4C) = U32(CUR(), 0xBC);
		S32(WS(), 0x48) = 0;
		S32(WS(), 0x44) = 0;
		S32(WS(), 0x40) = 0;
		SetupParentXformScaled();
		h_B27440(nullptr);
	}

	static inline uint32_t AddTint(uint32_t a, uint32_t b)
	{
		int32_t v = (int32_t)(a + b) - 0x80;
		if (v >= 0x100) return 0xFF;
		if (v < 0) return 0;
		return (uint32_t)v;
	}

	static void __cdecl dh_44_AnimatedEntityCopy()
	{
		if (U32(CUR(), 0xBC) == 0)
		{
			uint8_t *ent = PTR(SCENE(), 0x60 + 4 * (int32_t)U8(CUR(), 0x1B));
			PTR(CUR(), 0xBC) = CreateEntityModelCopy(ent);
			uint8_t *blk = PTR(CUR(), 0xBC);
			uint8_t *old = PTR(blk, 0x30);
			int32_t size = (int32_t)U8(old, 0) * 0x30 + 0x10;
			uint8_t *copy = blob::ArenaAlloc(size);
			PTR(blk, 0x30) = copy;
			if (size > 0)
			{
				uint32_t n = ((uint32_t)size + 3) >> 2;
				for (uint32_t k = 0; k < n; k++)
					U32(copy, 4 * k) = U32(old, 4 * k);
			}
			uint8_t *hdr = blk + 0x10;
			uint8_t *cmd = blk + 0x20;
			PTR(hdr, 4) = blk + 0x30;
			int32_t anim = S16(PTR(CUR(), 0xB8), 0);
			U8(blk, 9) = (uint8_t)anim;
			x::PreReadAnimation(hdr, cmd, anim);
		}
		uint8_t *blk = PTR(CUR(), 0xBC);
		SetupParentXformScaled();
		h_B26AD0(PTR(CUR(), 0xBC));
		uint32_t tint = U32(blk, 0x78);
		uint32_t col = U32(CUR(), 0xCC);
		uint32_t r = AddTint(tint & 0xFF, col & 0xFF);
		uint32_t g = AddTint((tint >> 8) & 0xFF, (col >> 8) & 0xFF);
		uint32_t b = AddTint((tint >> 16) & 0xFF, (col >> 16) & 0xFF);
		U32(blk, 0x5C) = (col & 0x2000000) | (b << 16) | r | (g << 8);
		uint8_t *m = PTR(CUR(), 0xBC);
		uint8_t *desc = m + 0x40;
		U32(desc, 4) = U32(CTX(), 0x74);
		uint32_t cursor = U32(CTX(), 0x7C);
		uint32_t ot = U32(RT(), 0x4C);
		U32(CTX(), 0x7C) = x::RenderGeometry(m + 0x30, desc, ot, 4, cursor);
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag232_breath_of_death()
	{
		using namespace gfc;
		death232::describe(death232::g_c);
		init_clone(death232::g_c, nullptr, 0);
		apply_shared_misc(death232::g_c);
		death232::g_c.draw[2] = death232::dh_02_MeshScaled;
		death232::g_c.bone[10] = death232::bh_10_ArcBetweenBones;
		death232::g_c.prim[58] = death232::pr_58_TintedFT4Abr;
		death232::g_c.draw[44] = death232::dh_44_AnimatedEntityCopy;
		register_port(game::GF_232BreathOfDeath_SequenceTick, (void *)death232::SequenceTask, "MAG232 Breath of Death SequenceTick", 232);
	}
}

