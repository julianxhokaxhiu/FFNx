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
namespace ochu_quake
{
	static Clone g_c = {};

	static void describe(Clone &c)
	{
		c.name = "240 Ochu Earthquake";
		c.effect_id = 240;
		c.vm_table = game_addr(game::MAG_240_VmOpcodeTable);
		c.bone_table = game_addr(game::MAG_240_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_A0D8A4);
		c.prim_size = game_addr(game::unk_1863F80);
		c.spr_table = game_addr(game::unk_1863FE4);
		c.ptype_table = game_addr(game::unk_1864010);
		c.pop_table = game_addr(game::funcs_A0FAB7);
		c.draw_table = game_addr(game::MAG_240_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_18641EE);
		c.attr_184 = game_addr(game::unk_1864208);
		c.attr_186 = game_addr(game::unk_186420A);
		c.attr_194 = game_addr(game::unk_1864218);
		c.attr_19C = game_addr(game::unk_1864220);
		c.desc = game_addr(game::MAG_240_Desc);
		c.ptr_block = game_addr(game::MAG_240_PtrBlock);
		c.queue = game_addr(game::unk_27968A0);
		c.file00 = game_addr(game::unk_2796898);
		c.file01 = game_addr(game::unk_279689C);
		c.tick = game_addr(game::MAG_240_GfCinematic_SequenceTick);
		c.load_cb = game_addr(game::MAG_240_LoadBattleFile_Callback);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static constexpr game::Id PIECE_DRAW_TABLE = game::MAG_240_PieceDrawTable, PIECE_INIT_TABLE = game::MAG_240_PieceInitTable;
	static inline VoidFn *PieceDraw() { return g_c.bigtab + (game_addr(PIECE_DRAW_TABLE) - game_addr(game::MAG_240_BoneHandlerTable)) / 4; }
	static inline VoidFn *PieceInit() { return g_c.bigtab + (game_addr(PIECE_INIT_TABLE) - game_addr(game::MAG_240_BoneHandlerTable)) / 4; }

	static inline void xl_Nop_B65150() { blob::f<int32_t (__cdecl *)()>(game_addr(game::xorEAX_6))(); }
	static inline int32_t xl_RotTransPers(const void *v, void *sxy, void *p, void *flag) { return x::f<int32_t (__cdecl *)(const void *, void *, void *, void *)>(game_addr(game::TransformWorldCoordinateToProjectedSpace))(v, sxy, p, flag); }
	static inline void xl_NormalColor_56CA30(const void *n, const void *col, void *out) { x::f<void (__cdecl *)(const void *, const void *, void *)>(game_addr(game::sub_56CA30))(n, col, out); }
	static inline void xl_GteLoadV012Block(const void *v) { x::f<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV012Dwords))(v); }
	static inline void xl_InsertPrimAutoDepth(uint32_t ot, void *prim)
	{
		x::f<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAutoDepth))(ot, prim);
	}
	static inline int32_t GteOTZ() { return MEM<int32_t>(game_addr(game::GTE_OTZ)); }

	static inline int16_t Div3(int32_t s) { return (int16_t)(s / 3); }

	static void __cdecl pi_09_PieceCentroids()
	{
		uint8_t *ws = WS();
		uint8_t *hdr = PTR(ws, 0xF0);
		uint8_t *verts = PTR(ws, 0xF4);
		uint8_t *pc = PTR(ws, 0xF8);
		int32_t n = S16(hdr, 2);
		uint8_t *rec = hdr + 4;
		if (n <= 0)
		{
			PTR(ws, 0xF0) = rec;
			PTR(WS(), 0xF8) = pc;
			return;
		}
		do
		{
			uint8_t *v1 = verts + U16(rec, 0x14);
			uint8_t *v2 = verts + U16(rec, 0x16);
			uint8_t *v0 = verts + U16(rec, 0x12);
			pc += 0x10;
			S16(pc, -0x10) = Div3(add32(add32(S16(v2, 0), S16(v1, 0)), S16(v0, 0)));
			S16(pc, -0x0E) = Div3(add32(add32(S16(v2, 2), S16(v1, 2)), S16(v0, 2)));
			S16(pc, -0x0C) = Div3(add32(add32(S16(v2, 4), S16(v1, 4)), S16(v0, 4)));
			rec += 0x1C;
		} while (--n != 0);
		PTR(WS(), 0xF0) = rec;
		PTR(WS(), 0xF8) = pc;
	}

	static void __cdecl pd_09_DrawPiecesGT3()
	{
		uint8_t *ws = WS();
		uint8_t *verts = PTR(ws, 0xD4);
		uint8_t *hdr = PTR(ws, 0xD0);
		uint8_t *pc = PTR(ws, 0xD8);
		uint8_t *pkt = PTR(ws, 0x60);
		const uint8_t code = (uint8_t)((int32_t)((U32(CUR(), 0xCC) & 0x2000000u) | 0x34000000u) >> 24);
		uint8_t *rec = hdr + 4;
		int32_t n = S16(hdr, 2);
		if (n > 0)
		{
			do
			{
				uint8_t *r4 = rec + 4;
				x::SetRotMatrix(PTR(WS(), 0x38));
				x::SetTransVector(PTR(WS(), 0x38));
				ws = WS();
				U16(ws, 0xB0) = (uint16_t)((uint32_t)U16(ws, 0x5C) * U16(pc, 8));
				U16(WS(), 0xB2) = (uint16_t)((uint32_t)U16(WS(), 0x5C) * U16(pc, 0xA));
				U16(WS(), 0xB4) = (uint16_t)((uint32_t)U16(WS(), 0x5C) * U16(pc, 0xC));
				ws = WS();
				uint8_t *vb = PTR(ws, 0x7C);
				uint8_t *v0 = vb + U16(r4, 0xE);
				uint8_t *v1 = vb + U16(r4, 0x10);
				uint8_t *v2 = vb + U16(r4, 0x12);
				S16(WS(), 0xA8) = Div3(add32(add32(S16(v2, 0), S16(v1, 0)), S16(v0, 0)));
				S16(WS(), 0xAA) = Div3(add32(add32(S16(v2, 2), S16(v1, 2)), S16(v0, 2)));
				S16(WS(), 0xAC) = Div3(add32(add32(S16(v2, 4), S16(v1, 4)), S16(v0, 4)));
				ws = WS();
				U16(ws, 0xB8) = (uint16_t)(mul32(S16(ws, 0xA8), S32(ws, 0x58)) / 256 + U16(ws, 0xA0));
				U16(WS(), 0xBA) = (uint16_t)(mul32(S16(WS(), 0xAA), S32(WS(), 0x58)) / 256 + U16(WS(), 0xA2));
				U16(WS(), 0xBC) = (uint16_t)(mul32(S16(WS(), 0xAC), S32(WS(), 0x58)) / 256 + U16(WS(), 0xA4));
				x::GteLoadV0(WS() + 0xB8);
				x::GteMVMVA_RotV0Tr();
				x::Gte_45E580();
				x::RotMatrixFromAngles(WS() + 0xB0, WS() + 0xC0);
				x::MulMatrixInPlace(WS() + 0xC0, PTR(WS(), 0x38));
				x::SetRotMatrix(WS() + 0xC0);
				U8(pkt, 3) = 9;
				U16(pkt, 0x1A) = U16(WS(), 0x50);
				U16(pkt, 0xE) = (uint16_t)(U16(r4, 0x14) + U16(WS(), 0x54));
				xl_NormalColor_56CA30(WS() + 0xE0, rec, pkt + 4);
				xl_NormalColor_56CA30(WS() + 0xE0, r4, pkt + 0x10);
				xl_NormalColor_56CA30(WS() + 0xE0, rec + 8, pkt + 0x1C);
				U8(pkt, 7) = code;
				U16(pkt, 0xC) = U16(r4, 8);
				U16(pkt, 0x18) = U16(r4, 0xA);
				U16(pkt, 0x24) = U16(r4, 0xC);
				for (int k = 0; k < 3; k++)
				{
					uint8_t *v = verts + U16(r4, 0xE + 2 * k);
					uint8_t *o = WS() + 0xC0 + 8 * k;
					U16(o, 0) = (uint16_t)(U16(v, 0) - U16(WS(), 0xA8));
					U16(WS() + 0xC0 + 8 * k, 2) = (uint16_t)(U16(v, 2) - U16(WS(), 0xAA));
					U16(WS() + 0xC0 + 8 * k, 4) = (uint16_t)(U16(v, 4) - U16(WS(), 0xAC));
				}
				xl_GteLoadV012Block(WS() + 0xC0);
				x::GteRTPT();
				x::GteReadSXY012(pkt + 8, pkt + 0x14, pkt + 0x20);
				x::GteAVSZ3();
				xl_InsertPrimAutoDepth(U32(RT(), 0x4C) + (uint32_t)(GteOTZ() >> 2) * 4u, pkt);
				pkt += 0x28;
				rec += 0x1C;
				pc += 0x10;
			} while (--n != 0);
		}
		ws = WS();
		PTR(ws, 0x60) = pkt;
		PTR(WS(), 0xF0) = rec;
		PTR(WS(), 0xF8) = pc;
	}

	static void __cdecl dh_33_ShatterSpin()
	{
		if (U32(CUR(), 0xBC) == 0)
		{
			uint8_t *m = blob::ObjectPtr(S16(PTR(CUR(), 0xB8), 0));
			PTR(CUR(), 0xD8) = m;
			uint8_t *mesh = PTR(CUR(), 0xD8);
			uint8_t *hdr = mesh + U32(mesh, 8);
			PTR(WS(), 0x6C) = hdr;
			uint8_t *blk = blob::ArenaAlloc(shl32(add32(S16(hdr, 2), 1), 4));
			PTR(CUR(), 0xBC) = blk;
			U32(WS(), 0x90) = U32(CUR(), 0xBC);
			U32(WS(), 0x94) = U32(WS(), 0x90) + 0x10;
			U32(WS(), 0x74) = U32(mesh, 0x14) + (uint32_t)mesh;
			U32(WS(), 0xF0) = U32(WS(), 0x6C);
			uint8_t *h = PTR(WS(), 0xF0);
			U32(WS(), 0xF4) = U32(WS(), 0x74);
			U32(WS(), 0xF8) = U32(WS(), 0x94);
			PieceInit()[S16(h, 0)]();
			int32_t range = S16(PTR(CUR(), 0xB8), 4);
			uint8_t *pc = PTR(WS(), 0x94);
			int32_t n = S16(h, 2);
			if (n > 0)
			{
				uint8_t *q = pc + 0xA;
				do
				{
					S16(q, -2) = (int16_t)blob::Rand73(range);
					S16(q, 0) = (int16_t)blob::Rand73(range);
					S16(q, 2) = (int16_t)blob::Rand73(range);
					q += 0x10;
				} while (--n != 0);
			}
		}
		U32(WS(), 0x90) = U32(CUR(), 0xBC);
		U32(WS(), 0x94) = U32(WS(), 0x90) + 0x10;
		S32(WS(), 0x50) = S16(CUR(), 0x92);
		S32(WS(), 0x54) = S16(CUR(), 0x9A);
		S32(WS(), 0x58) = S16(CUR(), 0x8C);
		S32(WS(), 0x5C) = S16(CUR(), 0x8E);
		S32(WS(), 0x8C) = S16(CUR(), 0x90);
		U32(WS(), 0xA0) = U32(CUR(), 0x94);
		U16(WS(), 0xA4) = U16(CUR(), 0x98);
		PTR(WS(), 0x38) = (uint8_t *)blob::GetParentMatrix((int32_t)U16(CUR(), 0x9C));
		uint8_t *mesh = PTR(CUR(), 0xD8);
		U32(WS(), 0x6C) = U32(mesh, 8) + (uint32_t)mesh;
		U32(WS(), 0x74) = U32(mesh, 0x14) + (uint32_t)mesh;
		U32(WS(), 0x7C) = U32(CTX(), 0x74);
		int32_t n = S32(mesh, 0x18);
		uint8_t *s = PTR(WS(), 0x74);
		uint8_t *d = PTR(WS(), 0x7C);
		if (n > 0)
		{
			do
			{
				d += 8;
				s += 8;
				S16(d, -8) = (int16_t)(mul32(S32(WS(), 0x8C), S16(s, -8)) / 256);
				S16(d, -6) = (int16_t)(mul32(S16(s, -6), S32(WS(), 0x8C)) / 256);
				S16(d, -4) = (int16_t)(mul32(S16(s, -4), S32(WS(), 0x8C)) / 256);
			} while (--n != 0);
		}
		h_B29450((int32_t)U32(CUR(), 0xCC));
		U32(WS(), 0x60) = U32(CTX(), 0x7C);
		U32(WS(), 0xD0) = U32(WS(), 0x6C);
		U32(WS(), 0xD4) = U32(WS(), 0x7C);
		uint8_t *h = PTR(WS(), 0xD0);
		U32(WS(), 0xD8) = U32(WS(), 0x94);
		PieceDraw()[S16(h, 0)]();
		U32(CTX(), 0x7C) = U32(WS(), 0x60);
	}

	static void __cdecl dh_34_MeshOnNode()
	{
		uint8_t *cur = CUR();
		if (U32(cur, 0xBC) == 0)
		{
			uint8_t *dsc = PTR(cur, 0xB8);
			PTR(CUR(), 0xD8) = blob::ObjectPtr(S16(dsc, 2));
			S32(CUR(), 0xBC) = S16(dsc, 0);
			cur = CUR();
		}
		U32(WS(), 0x94) = U32(cur, 0xB8);
		U32(WS(), 0x60) = U32(CTX(), 0x7C);
		U32(WS(), 0x50) = (uint32_t)(int32_t)S16(CUR(), 0x92) | 0xE1000000u;
		S32(WS(), 0x54) = S16(CUR(), 0x9A);
		U32(WS(), 0x84) = U32(CUR(), 0xCC) & 0x2FFFFFFu;
		U32(WS(), 0x8C) = U32(WS(), 0x84) & 0x2000000u;
		U32(WS(), 0x5C) = U32(RT(), 0x4C);
		U32(WS(), 0x90) = U8(CUR(), 0xDE);
		U32(WS(), 0x98) = U16(CUR(), 0x9E);
		U32(WS(), 0x68) = U32(CUR(), 0xD8);
		uint8_t *mesh = PTR(WS(), 0x68);
		U32(WS(), 0x6C) = U32(mesh, 8) + (uint32_t)mesh;
		U32(WS(), 0x74) = U32(mesh, 0x14) + (uint32_t)mesh;
		U32(WS(), 0x7C) = U32(CTX(), 0x74);
		U32(WS(), 0x80) = U32(WS(), 0x7C) + U32(mesh, 0x18) * 8u;
		uint8_t *dsc = PTR(WS(), 0x94);
		S32(WS(), 0x78) = S16(dsc, 0);
		uint8_t *node = blob::GetBone(S16(dsc, 6));
		xl_Nop_B65150();
		Mat4x3 *r = blob::RotMatrixOrder(node + 0x8C, S32(WS(), 0x78) >> 8);
		S32(WS(), 0xF4) = shl32(S16(CUR(), 0x8C), 4);
		S32(WS(), 0xF8) = shl32(S16(CUR(), 0x8E), 4);
		S32(WS(), 0xFC) = shl32(S16(CUR(), 0x90), 4);
		x::ScaleMatrix(r, WS() + 0xF4);
		x::SetRotMatrix(r);
		int32_t px = S16(node, 0x94), pz = S16(node, 0x98), py = S16(node, 0x96);
		x::GteWriteCtrl(px, 5);
		x::GteWriteCtrl(py, 6);
		x::GteWriteCtrl(pz, 7);
		x::GteLoadV0(CUR() + 0x94);
		x::GteMVMVA_RotV0Tr();
		x::Gte_45E580();
		uint8_t *ws = WS();
		int32_t n = S32(mesh, 0x18);
		uint8_t *v = PTR(ws, 0x74);
		uint8_t *mac = PTR(ws, 0x80);
		if (n > 0)
		{
			do
			{
				x::TransformToCamera(v, mac, WS() + 0xFC);
				v += 8;
				mac += 0x10;
			} while (--n != 0);
		}
		ws = WS();
		uint8_t *a = PTR(ws, 0x7C);
		int32_t lim = S16(PTR(ws, 0x94), 4);
		uint8_t *s = PTR(ws, 0x80);
		int32_t cnt = S32(mesh, 0x18);
		if ((U8(ws, 0x78) & 8) && cnt > 0)
		{
			do
			{
				U16(a, 0) = U16(s, 0);
				U16(a, 4) = U16(s, 8);
				int32_t y = S16(s, 4);
				if (y > lim) y = lim;
				U16(a, 2) = (uint16_t)y;
				a += 8;
				s += 0x10;
			} while (--cnt != 0);
		}
		x::SetRotMatrix(NODEMAT());
		x::SetTransVector(NODEMAT());
		uint8_t *w = WS();
		int32_t k = S32(mesh, 0x18);
		uint8_t *p = PTR(w, 0x7C);
		if (k > 0)
		{
			do
			{
				int32_t z = xl_RotTransPers(p, p, WS() + 0xFC, WS() + 0xFC);
				S32(p, 4) = z;
				w = WS();
				p += 8;
			} while (--k != 0);
		}
		uint8_t *hdr = PTR(w, 0x6C);
		int32_t c = S16(hdr, 2);
		while (c != -1)
		{
			S32(w, 0x70) = c;
			U32(WS(), 0x6C) = U32(WS(), 0x6C) + 4;
			int32_t type = S16(hdr, 0);
			uint8_t *w2 = WS();
			uint32_t size = MEM<uint8_t>(C().prim_size + (uint32_t)type);
			U32(w2, 0x58) = (uint32_t)mul32((int32_t)size, S32(w2, 0x70)) + U32(w2, 0x6C);
			C().prim[type]();
			U32(WS(), 0x6C) = U32(WS(), 0x58);
			w = WS();
			hdr = PTR(w, 0x6C);
			c = S16(hdr, 2);
		}
		U32(CTX(), 0x7C) = U32(w, 0x60);
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_mag240_ochu_earthquake()
	{
		using namespace gfc;
		ochu_quake::describe(ochu_quake::g_c);
		init_clone(ochu_quake::g_c, nullptr, 0);
		ochu_quake::g_c.draw[33] = ochu_quake::dh_33_ShatterSpin;
		ochu_quake::g_c.draw[34] = ochu_quake::dh_34_MeshOnNode;
		ochu_quake::PieceDraw()[9] = ochu_quake::pd_09_DrawPiecesGT3;
		ochu_quake::PieceInit()[9] = ochu_quake::pi_09_PieceCentroids;
		register_port(game::MAG_240_GfCinematic_SequenceTick, (void *)ochu_quake::SequenceTask, "MAG240 Ochu Earthquake SequenceTick", 240);
	}
}

