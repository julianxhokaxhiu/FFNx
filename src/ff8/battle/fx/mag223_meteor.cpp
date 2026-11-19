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

// Effect 223: Meteor

#include "gfc_engine.h"

namespace ff8fx
{
namespace gfc
{
	void apply_eden_d(Clone &c);

namespace meteor
{
	static Clone g_meteor = {};

	static void describe(Clone &c)
	{
		c.name = "223 Meteor";
		c.effect_id = 223;
		c.vm_table = game_addr(game::GF_223Meteor_VmOpcodeTable);
		c.bone_table = game_addr(game::GF_223Meteor_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_A930B4);
		c.prim_size = game_addr(game::unk_186C0BC);
		c.spr_table = game_addr(game::unk_186C0D0);
		c.ptype_table = game_addr(game::unk_186C0FC);
		c.pop_table = game_addr(game::funcs_A950F7);
		c.draw_table = game_addr(game::GF_223Meteor_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_186C2DA);
		c.attr_184 = game_addr(game::unk_186C2F4);
		c.attr_186 = game_addr(game::unk_186C2F6);
		c.attr_194 = game_addr(game::unk_186C304);
		c.attr_19C = game_addr(game::unk_186C30C);
		c.desc = game_addr(game::GF_223Meteor_ResourceDesc);
		c.ptr_block = game_addr(game::GF_223Meteor_PtrBlock);
		c.queue = game_addr(game::g_GF223Meteor_TaskQueue);
		c.file00 = game_addr(game::g_GF223Meteor_File00);
		c.file01 = game_addr(game::g_GF223Meteor_File01);
		c.tick = game_addr(game::GF_223Meteor_SequenceTick);
		c.load_cb = game_addr(game::GF_223Meteor_LoadBattleFileDone);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static VoidFn g_save_vm[0x200], g_save_big[256], g_save_draw[256];

	static void apply_prim6(Clone &c)
	{
		ff8fx::copy<(sizeof(g_save_vm))>(g_save_vm, c.vm);
		ff8fx::copy<(sizeof(g_save_big))>(g_save_big, c.bigtab);
		ff8fx::copy<(sizeof(g_save_draw))>(g_save_draw, c.draw);
		apply_eden_d(c);
		VoidFn p6 = c.prim[6];
		ff8fx::copy<(sizeof(g_save_vm))>(c.vm, g_save_vm);
		ff8fx::copy<(sizeof(g_save_big))>(c.bigtab, g_save_big);
		ff8fx::copy<(sizeof(g_save_draw))>(c.draw, g_save_draw);
		c.prim[6] = p6;
	}

	static constexpr game::Id RECTRING = game::g_GfCinematic_BlitRectRing;
	static constexpr game::Id RECTRING_IDX = game::g_GfCinematic_BlitRectRingIdx;

	static inline void xl_QueueVramReadback(const void *rect, uint32_t dst) { x::f<void (__cdecl *)(const void *, uint32_t)>(game_addr(game::Battle_QueueVramReadback_Type2))(rect, dst); }
	static inline void xl_TexWindow_B66670(int32_t id) { x::f<int32_t (__cdecl *)(int32_t)>(game_addr(game::GfCinematic_ReadAltTextureWindow))(id); }
	static inline int32_t xl_RotTransPers_56C8C0(const void *v, void *sxy, void *p, void *flag) { return x::f<int32_t (__cdecl *)(const void *, void *, void *, void *)>(game_addr(game::GTE_RotTransPersDiv8))(v, sxy, p, flag); }

	static void StoreRect_B66F90(const uint8_t *src, uint32_t dst)
	{
		uint8_t *slot = (uint8_t *)(game_addr(RECTRING) + 8u * (MEM<uint8_t>(game_addr(RECTRING_IDX)) & 0xFu));
		U16(slot, 0) = U16(src, 0);
		U16(slot, 2) = U16(src, 2);
		U16(slot, 4) = U16(src, 4);
		U16(slot, 6) = U16(src, 6);
		xl_QueueVramReadback(slot, dst);
		MEM<uint8_t>(game_addr(RECTRING_IDX)) = (uint8_t)(MEM<uint8_t>(game_addr(RECTRING_IDX)) + 1);
	}

	static void LoadRect_B66F40(const uint8_t *src, uint32_t data)
	{
		uint8_t *slot = (uint8_t *)(game_addr(RECTRING) + 8u * (MEM<uint8_t>(game_addr(RECTRING_IDX)) & 0xFu));
		U16(slot, 0) = U16(src, 0);
		U16(slot, 2) = U16(src, 2);
		U16(slot, 4) = U16(src, 4);
		U16(slot, 6) = U16(src, 6);
		x::QueueVramUpload(slot, (const void *)data);
		MEM<uint8_t>(game_addr(RECTRING_IDX)) = (uint8_t)(MEM<uint8_t>(game_addr(RECTRING_IDX)) + 1);
	}

	static inline uint32_t MagicTexBuf() { return MEM<uint32_t>(game_addr(game::MAGIC_TEXTURE_BUFFER_PTR)); }

	static void __cdecl op_035_VramReadback()
	{
		uint8_t *s = STREAM();
		if (U8(RT(), 0x4B) & 0x80)
		{
			int32_t slot = S16(s, 4);
			uint8_t *src = s + S16(s, 2);
			uint32_t dst = MagicTexBuf() + ((uint32_t)slot << 4);
			STREAM() = s + 6;
			StoreRect_B66F90(src, dst);
			return;
		}
		uint8_t *src = s + 2;
		int32_t n = shl32(mul32(S16(s, 8), S16(s, 6)), 1);
		uint8_t *blk = blob::ArenaAlloc(n);
		PTR(CUR(), 0xC4) = blk;
		uint32_t dst = U32(CUR(), 0xC4);
		STREAM() = STREAM() + 10;
		StoreRect_B66F90(src, dst);
	}

	static void __cdecl op_0BD_VramUpload()
	{
		if (!(U8(RT(), 0x4B) & 0x80))
		{
			x::f<VoidFn>(game_addr(game::GF_223Meteor_VmOp0BD_VramUpload))();
			return;
		}
		uint8_t *s = STREAM();
		uint32_t data = MagicTexBuf() + ((uint32_t)S16(s, 4) << 4);
		uint8_t *rect = s + S16(s, 2);
		STREAM() = s + 6;
		LoadRect_B66F40(rect, data);
	}

	static void __cdecl op_0BB_CopyAltTextureToBuffer()
	{
		blob::ReadAltTexture(S16(STREAM(), 2));
		uint8_t *w = WS();
		uint32_t dst = MagicTexBuf() + ((uint32_t)S16(STREAM(), 4) << 4);
		uint32_t src = U32(w, 0xFC);
		uint8_t *r = PTR(w, 0xF0);
		int32_t n = shl32(mul32(S16(r, 6), S16(r, 4)), 1);
		bool copy = true;
		if (copy)
		{
			if ((int32_t)src < (int32_t)dst)
			{
				n -= 4;
				if (n > 0)
				{
					uint32_t d = (uint32_t)n + dst;
					uint32_t k = ((uint32_t)n + 3) >> 2;
					uint32_t off = src - dst;
					do
					{
						MEM<uint32_t>(d) = MEM<uint32_t>(off + d);
						d -= 4;
					} while (--k != 0);
				}
			}
			else if (n > 0)
			{
				uint32_t k = ((uint32_t)n + 3) >> 2;
				uint32_t off = src - dst;
				uint32_t d = dst;
				do
				{
					MEM<uint32_t>(d) = MEM<uint32_t>(d + off);
					d += 4;
				} while (--k != 0);
			}
		}
		STREAM() = STREAM() + 6;
	}

	static void __cdecl dh_28_SpreadActionResults()
	{
		uint8_t *b = CUR();
		if (U32(b, 0xBC) == 0)
		{
			U32(b, 0xB8) = (uint32_t)(int32_t)S16(PTR(b, 0xB8), 0);
			U32(CUR(), 0xC0) = 0;
			uint8_t *act = PTR(CTX(), 0xCC);
			U32(CUR(), 0xC4) = U8(act, 0x10);
			b = CUR();
		}
		int32_t n = S32(b, 0xC4);
		int32_t i = S32(b, 0xC0);
		if (i >= n) return;
		int32_t q = mul32(i + 1, S32(b, 0xB8)) / (n + 1);
		if (q <= S32(b, 0xBC))
		{
			uint8_t *act = PTR(CTX(), 0xCC);
			x::ApplyActionResultToTarget((int32_t)(U32(act, 8) + (uint32_t)(i * 3) * 8u));
			U32(CUR(), 0xC0) = U32(CUR(), 0xC0) + 1;
			b = CUR();
		}
		U32(b, 0xBC) = U32(b, 0xBC) + 1;
	}

	static void SetupRefXform_A91790(int32_t ref)
	{
		uint8_t *b = blob::GetBone(ref);
		Mat4x3 *m = blob::GetParentMatrix((int32_t)U16(CUR(), 0x9C));
		x::GteSetRotMatrix(m);
		x::GteSetTransVector(m);
		x::GteLoadV0(b + 0x94);
		x::GteMVMVA_RotV0Tr();
		x::Gte_45E580();
	}

	static inline int32_t divz256(int32_t v) { return (v + ((v >> 31) & 0xFF)) >> 8; }

	static uint32_t scale_rgb(uint32_t c, int32_t f)
	{
		uint32_t r = (uint32_t)shl32(divz256(mul32((int32_t)((c >> 16) & 0xFF), f)), 8);
		r |= (uint32_t)divz256(mul32((int32_t)((c >> 8) & 0xFF), f));
		r <<= 8;
		r |= (uint32_t)divz256(mul32((int32_t)(c & 0xFF), f));
		return r | (c & 0xFF000000u);
	}

	static void VertexColour_A917F0()
	{
		uint8_t *w = WS();
		U32(w, 0xBC) = U32(w, 0x58);
		uint32_t c = U32(w, 0x58);
		uint32_t hi = c & 0xFF000000u;
		if (U16(w, 0xB6) != 0 && (int32_t)S16(w, 0xB4) <= S32(w, 0x5C))
		{
			int16_t f = S16(w, 0xBA);
			if (f <= 0) U32(w, 0xBC) = hi;
			else U32(w, 0xBC) = scale_rgb(c, f);
			U16(w, 0xBA) = (uint16_t)(U16(w, 0xBA) + U16(w, 0xB6));
			return;
		}
		if (U16(w, 0xB2) == 0) return;
		if ((int32_t)S16(w, 0xB0) >= S32(w, 0x5C))
		{
			U32(w, 0xBC) = hi;
			return;
		}
		int16_t f = S16(w, 0xB8);
		if (f >= 0x100) U32(w, 0xBC) = c;
		else U32(w, 0xBC) = scale_rgb(c, f);
		U16(w, 0xB8) = (uint16_t)(U16(w, 0xB8) + U16(w, 0xB2));
	}

	static void GridVertexFirstRow(int32_t a, int32_t v4, int32_t vC, bool wave)
	{
		uint8_t *w = WS();
		U16(w, 0xC0) = (uint16_t)(mul32(x::Cos(a), v4) >> 16);
		U16(WS(), 0xC2) = (uint16_t)(mul32(x::Sin(a), v4) >> 16);
		w = WS();
		if (!wave)
		{
			U16(w, 0xC4) = (uint16_t)(vC >> 16);
			S32(PTR(w, 0x40), 0) = S16(w, 0xC4);
		}
		else
		{
			int32_t s = x::Sin(shl32(S16(WS(), 0xA6), 4));
			w = WS();
			U16(w, 0xC4) = (uint16_t)(add32(vC, mul32(s, S16(w, 0xA0)) >> 4) >> 16);
			S32(PTR(w, 0x40), 0) = S16(w, 0xC4);
			U16(w, 0xA2) = (uint16_t)(U16(w, 0xA2) + U16(w, 0xA4));
			U16(w, 0xA0) = (uint16_t)(U16(w, 0xA0) + U16(w, 0xA2));
			U16(w, 0xA8) = (uint16_t)(U16(w, 0xA8) + U16(w, 0xAA));
			U16(w, 0xA6) = (uint16_t)(U16(w, 0xA6) + U16(w, 0xA8));
		}
		if (U8(w, 0xAC) & 0x10)
		{
			uint16_t d = U16(w, 0xC4);
			int16_t cc = S16(w, 0xC2);
			U16(w, 0xC2) = d;
			U16(w, 0xC4) = (uint16_t)cc;
		}
		U32(w, 0x40) = U32(w, 0x40) + 4;
	}

	static void GridVertexNextRow(int32_t a, int32_t v4)
	{
		uint8_t *w = WS();
		U16(w, 0xC0) = (uint16_t)(mul32(x::Cos(a), v4) >> 16);
		U16(WS(), 0xC2) = (uint16_t)(mul32(x::Sin(a), v4) >> 16);
		w = WS();
		U16(w, 0xC4) = U16(PTR(w, 0x40), 0);
		if (U8(w, 0xAC) & 0x10)
		{
			uint16_t d = U16(w, 0xC4);
			int16_t cc = S16(w, 0xC2);
			U16(w, 0xC2) = d;
			U16(w, 0xC4) = (uint16_t)cc;
		}
		U32(w, 0x40) = U32(w, 0x40) + 4;
	}

	static void __cdecl dh_25_TexturedRibbonGrid()
	{
		uint8_t *b = CUR();
		uint32_t v20 = 0;
		int32_t v30 = 0;
		if (U32(b, 0xC4) == 0)
		{
			U32(b, 0xC4) = 0xFFFFFFFF;
			b = CUR();
			if (U32(b, 0xBC) == 0)
			{
				U32(b, 0xBC) = game_addr(game::unk_27971E4);
				b = CUR();
			}
		}
		uint8_t *prm = PTR(b, 0xB8);
		uint8_t *fade = PTR(b, 0xBC);
		SetupRefXform_A91790(S16(prm, 6));
		uint8_t *ref = blob::GetBone(S16(prm, 6));
		S32(WS(), 0xF0) = S16(prm, 8);
		uint8_t *ref2 = nullptr;
		if (S32(WS(), 0xF0) != 0) ref2 = blob::GetBone(S32(WS(), 0xF0));
		U32(WS(), 0xE0) = U32(CTX(), 0x74) + 0x400;
		xl_TexWindow_B66670(S16(prm, 0xA));
		uint8_t *w = WS();
		U32(w, 0xE4) = U32(w, 0xF8);
		{
			uint8_t *r = PTR(w, 0xF0);
			int32_t tw = S16(r, 4);
			if (U8(w, 0xF4) & 0x80) tw += tw;
			S32(w, 0x30) = tw;
			S32(WS(), 0x34) = S16(r, 6);
		}
		w = WS();
		S32(w, 0x50) = S16(CUR(), 0x92);
		S32(w, 0x54) = S16(CUR(), 0x9A);
		U32(w, 0x58) = U32(CUR(), 0xCC) | 0x3C000000u;
		U32(w, 0x5C) = 0;
		U32(w, 0x60) = U32(CTX(), 0x7C);
		U32(w, 0x98) = 0;
		U32(w, 0x9C) = 0;
		S32(w, 0x64) = S16(prm, 0);
		S32(w, 0x68) = shl32(S16(ref, 0x90), 16);
		S32(w, 0x6C) = 0x10000000 / S32(w, 0x64);
		S32(w, 0x70) = S16(prm, 4);
		S32(w, 0x74) = S16(CUR(), 0x8E);
		S32(w, 0x7C) = shl32(S16(CUR(), 0x90), 8);
		S32(w, 0x78) = sub32(shl32(S16(CUR(), 0x94), 8), S32(w, 0x7C)) / S32(w, 0x70);
		S32(w, 0x20) = shl32(S16(CUR(), 0x8C), 16);
		S32(w, 0x24) = shl32(S32(w, 0x30), 16) / S32(w, 0x70);
		S32(w, 0x28) = 0;
		S32(w, 0x2C) = S16(prm, 2);
		S32(w, 0x84) = shl32(S16(CUR(), 0x96), 8);
		S32(w, 0x88) = shl32(S16(CUR(), 0x98), 8);
		U16(w, 0xAC) = U16(prm, 0xC);
		U16(w, 0xB0) = U16(fade, 0);
		U16(w, 0xB2) = U16(fade, 2);
		U16(w, 0xB4) = U16(fade, 4);
		U16(w, 0xB6) = U16(fade, 6);
		U16(w, 0xB8) = 0;
		U16(w, 0xBA) = 0x100;
		if (ref2 == nullptr)
		{
			U32(w, 0xA0) = 0;
			U32(w, 0xA4) = 0;
			U32(w, 0xA8) = 0;
		}
		else
		{
			U16(w, 0xA0) = (uint16_t)((uint32_t)U8(ref2, 0x94) << 8);
			U16(w, 0xA2) = U16(ref2, 0x96);
			U16(w, 0xA4) = U16(ref2, 0x98);
			U16(w, 0xA6) = U16(ref2, 0x8C);
			U16(w, 0xA8) = U16(ref2, 0x8E);
			U16(w, 0xAA) = U16(ref2, 0x90);
		}
		S32(w, 0x90) = shl32(S16(ref, 0x8C), 16);
		S32(w, 0x94) = shl32(S16(ref, 0x8E), 12);
		int32_t flag;
		if (!(U8(w, 0xAC) & 1))
		{
			S32(w, 0x8C) = shl32(S16(ref, 0x98), 16);
			flag = v30;
		}
		else
		{
			flag = S32(w, 0x70);
			int32_t st = S32(w, 0x94);
			int32_t e = S32(w, 0x90);
			int32_t sum = 0;
			v20 = (uint32_t)st;
			if (flag > 1)
			{
				int32_t k = flag - 1;
				flag = 1;
				do
				{
					e = add32(e, st);
					sum = add32(sum, e);
				} while (--k != 0);
			}
			S32(w, 0x8C) = sub32(shl32(S16(ref, 0x98), 16), sum);
		}
		U32(w, 0x40) = U32(w, 0xE0);
		int32_t v1c = S32(w, 0x84);
		int32_t v18 = S32(w, 0x78);
		int32_t v10 = S32(w, 0x7C);
		uint8_t *pk = PTR(w, 0x60);
		int32_t vC = S32(w, 0x8C);
		int32_t v28 = S32(w, 0x90);
		int32_t v14 = S32(w, 0x68);
		int32_t ebx = S32(w, 0x20);
		uint8_t *v8 = flag == 0 ? PTR(CTX(), 0x74) : PTR(CTX(), 0x74) + 0x200;
		int32_t v2c = 0;
		GridVertexFirstRow(v14 >> 16, v10 >> 4, vC, ref2 != nullptr);
		w = WS();
		U16(v8, 0) = (uint16_t)xl_RotTransPers_56C8C0(w + 0xC0, pk + 0x10, w + 0xFC, w + 0xFC);
		w = WS();
		{
			int32_t r = (ebx >> 16) % S32(w, 0x30);
			uint8_t u = (uint8_t)add32(r, S32(w, 0x98));
			U8(pk, 0x2C) = u;
			U8(pk, 0x14) = u;
			uint8_t v = (uint8_t)(U8(w, 0x9C) + U8(w, 0x28));
			U8(pk, 0x21) = v;
			U8(pk, 0x15) = v;
		}
		VertexColour_A917F0();
		U32(pk, 0x24) = U32(WS(), 0xBC);
		U32(pk, 0x0C) = U32(pk, 0x24);
		S32(WS(), 0x5C) = S32(WS(), 0x5C) + 1;
		if (S32(WS(), 0x70) > 0)
		{
			uint8_t *nx = pk + 0x54;
			v30 = S32(WS(), 0x70);
			for (;;)
			{
				w = WS();
				v18 = add32(v18, S32(w, 0x74));
				v10 = add32(v10, v18);
				v1c = add32(v1c, S32(w, 0x88));
				v14 = add32(v14, v1c);
				v28 = add32(v28, S32(w, 0x94));
				vC = add32(vC, v28);
				ebx = add32(ebx, S32(w, 0x24));
				v2c = ebx;
				GridVertexFirstRow(v14 >> 16, v10 >> 4, vC, ref2 != nullptr);
				w = WS();
				U16(v8, 0) = (uint16_t)xl_RotTransPers_56C8C0(w + 0xC0, nx, w + 0xFC, w + 0xFC);
				U32(pk, 0x1C) = U32(nx, 0);
				VertexColour_A917F0();
				U32(pk, 0x30) = U32(WS(), 0xBC);
				U32(pk, 0x18) = U32(pk, 0x30);
				U32(nx, 0x14) = U32(WS(), 0xBC);
				U32(nx, -4) = U32(nx, 0x14);
				w = WS();
				int32_t r = (v2c >> 16) % S32(w, 0x30);
				if (r != 0)
				{
					int32_t e = add32(r, S32(w, 0x98));
					int32_t a0 = U8(pk, 0x14);
					v20 = (uint32_t)a0;
					if (a0 < e)
					{
						U8(pk, 0x38) = (uint8_t)e;
						U8(pk, 0x20) = (uint8_t)e;
						U8(nx, 0x1C) = (uint8_t)e;
						U8(nx, 4) = (uint8_t)e;
					}
					else
					{
						U8(nx, 0x1C) = (uint8_t)e;
						U8(nx, 4) = (uint8_t)e;
						uint8_t s = (uint8_t)(U8(WS(), 0x30) + (uint8_t)e);
						U8(pk, 0x38) = s;
						U8(pk, 0x20) = s;
					}
				}
				else
				{
					U8(nx, 0x1C) = U8(w, 0x98);
					U8(nx, 4) = U8(w, 0x98);
					uint8_t s = (uint8_t)(U8(WS(), 0x98) + U8(WS(), 0x30));
					U8(pk, 0x38) = s;
					U8(pk, 0x20) = s;
				}
				uint8_t v15 = U8(pk, 0x15);
				pk += 0x44;
				U8(nx, 0x11) = v15;
				U8(nx, 5) = v15;
				nx += 0x44;
				S32(WS(), 0x5C) = S32(WS(), 0x5C) + 1;
				if (--v30 == 0) break;
			}
		}
		w = WS();
		if (S32(w, 0x64) > 0)
		{
			do
			{
				w = WS();
				U32(w, 0x40) = U32(w, 0xE0);
				uint8_t *prev = pk - (uint32_t)S32(w, 0x70) * 0x44u;
				S32(w, 0x28) = add32(S32(w, 0x28), S32(w, 0x2C));
				v18 = S32(w, 0x78);
				v10 = S32(w, 0x7C);
				S32(w, 0x68) = add32(S32(w, 0x68), S32(w, 0x6C));
				v1c = S32(w, 0x84);
				v14 = S32(w, 0x68);
				uint8_t *base = PTR(CTX(), 0x74);
				uint8_t *dA, *dB;
				if (!(U8(w, 0x64) & 1)) { dA = base + 0x200; dB = base; }
				else { dA = base; dB = base + 0x200; }
				GridVertexNextRow(v14 >> 16, v10 >> 4);
				uint8_t *sx = pk + 0x10;
				w = WS();
				U16(dA, 0) = (uint16_t)xl_RotTransPers_56C8C0(w + 0xC0, sx, w + 0xFC, w + 0xFC);
				{
					uint32_t s0 = U32(sx, 0);
					uint32_t c0 = U32(prev, 0xC);
					U32(prev, 0x28) = s0;
					U32(pk, 0x24) = c0;
					U32(pk, 0x0C) = c0;
					uint32_t c1 = U32(prev, 0x18);
					U32(pk, 0x30) = c1;
					U32(pk, 0x18) = c1;
					uint8_t u = U8(prev, 0x14);
					U8(pk, 0x2C) = u;
					U8(pk, 0x14) = u;
					u = U8(prev, 0x20);
					U8(pk, 0x38) = u;
					U8(pk, 0x20) = u;
				}
				w = WS();
				int32_t r = S32(w, 0x28) % S32(w, 0x34);
				if (r != 0)
				{
					int32_t e = add32(r, S32(w, 0x9C));
					U8(prev, 0x39) = (uint8_t)e;
					U8(prev, 0x2D) = (uint8_t)e;
					U8(pk, 0x21) = (uint8_t)e;
					U8(pk, 0x15) = (uint8_t)e;
					S32(WS(), 0x3C) = e;
					S32(WS(), 0x38) = e;
				}
				else
				{
					U8(pk, 0x21) = U8(w, 0x9C);
					U8(pk, 0x15) = U8(w, 0x9C);
					uint8_t s = (uint8_t)(U8(WS(), 0x9C) + U8(WS(), 0x34));
					U8(prev, 0x39) = s;
					U8(prev, 0x2D) = s;
					S32(WS(), 0x38) = S32(WS(), 0x9C);
					S32(WS(), 0x3C) = add32(S32(WS(), 0x9C), S32(WS(), 0x34));
				}
				w = WS();
				if (S32(w, 0x70) > 0)
				{
					v30 = S32(w, 0x70);
					do
					{
						U8(prev, 3) = 0x10;
						U32(prev, 8) = 0;
						U32(prev, 0x3C) = 0xE2000000u;
						U32(prev, 0x40) = 0;
						sx += 0x44;
						U16(prev, 0x22) = U16(WS(), 0x50);
						U16(prev, 0x16) = U16(WS(), 0x54);
						U32(prev, 4) = U32(WS(), 0xE4);
						w = WS();
						v18 = add32(v18, S32(w, 0x74));
						v10 = add32(v10, v18);
						v1c = add32(v1c, S32(w, 0x88));
						v14 = add32(v14, v1c);
						GridVertexNextRow(v14 >> 16, v10 >> 4);
						w = WS();
						int32_t z = xl_RotTransPers_56C8C0(w + 0xC0, sx, w + 0xFC, w + 0xFC);
						int32_t sum = (int32_t)S16(dB, 0) + (int32_t)S16(dA, 0);
						v20 = (uint32_t)prev;
						dA += 2;
						prev += 0x44;
						dB += 2;
						pk += 0x44;
						U16(dA, 0) = (uint16_t)z;
						int32_t t = add32((int32_t)S16(dB, 0) + sum, z);
						uint32_t s0 = U32(sx, 0);
						U32(pk, -0x28) = s0;
						U32(prev, -0x10) = s0;
						t = (t + ((t >> 31) & 3)) >> 2;
						uint32_t m = U32(prev, -0x38);
						U32(pk, -0x20) = m;
						U32(pk, -0x38) = m;
						m = U32(prev, -0x2C);
						U32(pk, -0x14) = m;
						U32(pk, -0x2C) = m;
						t >>= 2;
						uint8_t d = U8(WS(), 0x38);
						U8(sx, 0x11) = d;
						U8(sx, 5) = d;
						d = U8(WS(), 0x3C);
						U8(prev, -0xB) = d;
						U8(prev, -0x17) = d;
						d = U8(prev, 0x14);
						U8(sx, 0x1C) = d;
						U8(sx, 4) = d;
						U32(prev, 0x28) = s0;
						d = U8(prev, 0x20);
						U8(sx, 0x28) = d;
						U8(sx, 0x10) = d;
						if (t > 0 && t < 0xFFF) x::InsertPrimAltViewport(U32(RT(), 0x4C) + (uint32_t)t * 4u, (void *)v20);
					} while (--v30 != 0);
				}
				w = WS();
				S32(w, 0x64) = S32(w, 0x64) - 1;
			} while (S32(WS(), 0x64) > 0);
		}
		U32(CTX(), 0x7C) = v20 + 0x44;
	}

	static void apply_meteor_ports(Clone &c)
	{
		c.vm[0x035] = op_035_VramReadback;
		c.vm[0x0BB] = op_0BB_CopyAltTextureToBuffer;
		c.vm[0x0BD] = op_0BD_VramUpload;
		c.draw[25] = dh_25_TexturedRibbonGrid;
		c.draw[28] = dh_28_SpreadActionResults;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_meteor);
	}
}
}

	void register_mag223_meteor()
	{
		using namespace gfc;
		meteor::describe(meteor::g_meteor);
		init_clone(meteor::g_meteor, nullptr, 0);
		apply_shared_misc(meteor::g_meteor);
		meteor::apply_prim6(meteor::g_meteor);
		meteor::apply_meteor_ports(meteor::g_meteor);
		register_port(game::GF_223Meteor_SequenceTick, (void *)meteor::SequenceTask, "MAG223 Meteor SequenceTick", 223);
	}
}

