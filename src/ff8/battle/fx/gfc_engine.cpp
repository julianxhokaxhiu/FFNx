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

// Engine of the cinematic GFs

#include "gfc_engine.h"
#include <intrin.h>

namespace ff8fx
{
namespace gfc
{
	Clone *g_clone = nullptr;
	Generic g_generic;

namespace part_core
{
	static int32_t DebugPadRootControl()
	{
		uint8_t *ctx = CTX();
		if (U8(ctx, 0x4C) & 0x10)
		{
			uint32_t pad = U32(ctx, 0x40);
			if (pad & 0x8000) MEM<int16_t>(game_addr(game::g_GfCinematic_RootAngleY)) = (int16_t)(MEM<int16_t>(game_addr(game::g_GfCinematic_RootAngleY)) + 0x20);
			if (pad & 0x2000) MEM<int16_t>(game_addr(game::g_GfCinematic_RootAngleY)) = (int16_t)(MEM<int16_t>(game_addr(game::g_GfCinematic_RootAngleY)) - 0x20);
			if (pad & 0x1000) MEM<int16_t>(game_addr(game::g_GfCinematic_RootAngleX)) = (int16_t)(MEM<int16_t>(game_addr(game::g_GfCinematic_RootAngleX)) - 0x20);
			if (pad & 0x4000) MEM<int16_t>(game_addr(game::g_GfCinematic_RootAngleX)) = (int16_t)(MEM<int16_t>(game_addr(game::g_GfCinematic_RootAngleX)) + 0x20);
			if (pad & 4) MEM<int16_t>(game_addr(game::g_GfCinematic_RootAngleZ)) = (int16_t)(MEM<int16_t>(game_addr(game::g_GfCinematic_RootAngleZ)) - 0x20);
			if (pad & 8) MEM<int16_t>(game_addr(game::g_GfCinematic_RootAngleZ)) = (int16_t)(MEM<int16_t>(game_addr(game::g_GfCinematic_RootAngleZ)) + 0x20);
			if (pad & 1) MEM<int32_t>(game_addr(game::unk_2797778)) = sub32(MEM<int32_t>(game_addr(game::unk_2797778)), 0x20);
			if (pad & 2) MEM<int32_t>(game_addr(game::unk_2797778)) = add32(MEM<int32_t>(game_addr(game::unk_2797778)), 0x20);
		}
		if (U8(ctx, 0x4C) & 0x40)
		{
			uint32_t pad = U32(ctx, 0x40);
			if (pad & 0x8000) MEM<int32_t>(game_addr(game::unk_2797770)) = sub32(MEM<int32_t>(game_addr(game::unk_2797770)), 0x100);
			if (pad & 0x2000) MEM<int32_t>(game_addr(game::unk_2797770)) = add32(MEM<int32_t>(game_addr(game::unk_2797770)), 0x100);
			if (pad & 0x1000) MEM<int32_t>(game_addr(game::unk_2797774)) = sub32(MEM<int32_t>(game_addr(game::unk_2797774)), 0x100);
			if (pad & 0x4000) MEM<int32_t>(game_addr(game::unk_2797774)) = add32(MEM<int32_t>(game_addr(game::unk_2797774)), 0x100);
			if (pad & 1) MEM<int32_t>(game_addr(game::unk_2797778)) = sub32(MEM<int32_t>(game_addr(game::unk_2797778)), 0x100);
			if (pad & 2) MEM<int32_t>(game_addr(game::unk_2797778)) = add32(MEM<int32_t>(game_addr(game::unk_2797778)), 0x100);
			if (pad & 4) MEM<int32_t>(game_addr(game::unk_2797778)) = sub32(MEM<int32_t>(game_addr(game::unk_2797778)), 0x10);
			if (pad & 8) MEM<int32_t>(game_addr(game::unk_2797778)) = add32(MEM<int32_t>(game_addr(game::unk_2797778)), 0x10);
		}
		return 0;
	}

	static void __cdecl bh_00_AccumPos()
	{
		S16(CUR(), 0x94) = (int16_t)(S32(CUR(), 0x5C) >> 16);
		S16(CUR(), 0x96) = (int16_t)(S32(CUR(), 0x60) >> 16);
		S16(CUR(), 0x98) = (int16_t)(S32(CUR(), 0x64) >> 16);
	}

	static void __cdecl bh_05_CopyBone()
	{
		uint8_t *o = blob::GetBone(S16(PTR(CUR(), 0xA0), 0));
		S16(CUR(), 0x94) = S16(o, 0x94);
		S16(CUR(), 0x96) = S16(o, 0x96);
		S16(CUR(), 0x98) = S16(o, 0x98);
	}

	static void __cdecl bh_07_BonePlusAccum()
	{
		uint8_t *ref = PTR(CUR(), 0xA0);
		if (!ref) return;
		uint8_t *o = blob::GetBone(S16(ref, 0));
		S16(CUR(), 0x94) = (int16_t)((S32(CUR(), 0x5C) >> 16) + S16(o, 0x94));
		S16(CUR(), 0x96) = (int16_t)((S32(CUR(), 0x60) >> 16) + S16(o, 0x96));
		S16(CUR(), 0x98) = (int16_t)((S32(CUR(), 0x64) >> 16) + S16(o, 0x98));
	}

	static void __cdecl bh_03_Polar()
	{
		uint8_t *o = blob::GetBone(S16(CUR(), 0xB0));
		int32_t ang = S32(CUR(), 0x64) >> 16;
		int32_t c = x::Cos(ang);
		S16(CUR(), 0x98) = (int16_t)((mul32(S32(CUR(), 0x5C) >> 8, c >> 4) >> 16) + S16(o, 0x98));
		int32_t s = x::Sin(ang);
		S16(CUR(), 0x94) = (int16_t)((mul32(S32(CUR(), 0x5C) >> 8, s >> 4) >> 16) + S16(o, 0x94));
		S16(CUR(), 0x96) = (int16_t)((S32(CUR(), 0x60) >> 16) + S16(o, 0x96));
	}

	static void __cdecl bh_06_LerpBones()
	{
		const uint8_t *refs = PTR(CUR(), 0xA0);
		uint8_t *a = blob::GetBone(S16(refs, 0));
		uint8_t *b = blob::GetBone(S16(refs, 2));
		for (int k = 0; k < 3; k++)
		{
			uint8_t *bone = CUR();
			int16_t av = S16(a, 0x94 + 2 * k);
			int32_t d = mul32((int32_t)S16(b, 0x94 + 2 * k) - av, S32(bone, 0x50 + 4 * k) >> 16) / 256;
			S16(bone, 0x94 + 2 * k) = (int16_t)((S32(bone, 0x5C + 4 * k) >> 16) + av + d);
		}
	}

	static void __cdecl bh_09_EntityJoint()
	{
		uint8_t *bone = CUR();
		uint8_t *ent = PTR(SCENE(), 0x60 + 4 * U8(bone, 0x1B));
		int32_t px = S32(bone, 0x5C) >> 16;
		uint8_t *rec = PTR(PTR(ent, 0x64), 0);
		int32_t mask = S16(PTR(bone, 0xA0), 0);
		int32_t scale = S16(rec, 2);
		if (mask & 0x1000) S16(bone, 0x94) = (int16_t)(mul32(S16(rec, 8), scale) / 256 + px);
		else S16(bone, 0x94) = (int16_t)px;
		int32_t py = S32(bone, 0x60) >> 16;
		if (mask & 0x800) S16(bone, 0x96) = (int16_t)(mul32(S16(rec, 0xA), scale) / 256 + py);
		else S16(bone, 0x96) = (int16_t)py;
		int32_t pz = S32(bone, 0x64) >> 16;
		if (mask & 0x400) S16(bone, 0x98) = (int16_t)(mul32(S16(rec, 0xC), scale) / 256 + pz);
		else S16(bone, 0x98) = (int16_t)pz;
	}

	template<bool NEG>
	static void AnimChannels()
	{
		if (NEG)
		{
			U16(RT(), 0x3E) = 0;
			U32(RT(), 0x4C) = U32(RT(), 0x38);
		}
		else U16(RT(), 0x3E) = 0;
		U8(RT(), 0x44) = 0;
		uint32_t e = ORDER()[U8(RT(), 0x44)];
		if (e == 0xFF) return;
		for (;;)
		{
			uint8_t *rt = RT();
			if (U8(rt, 0x45) == 0 || (e & 0x80))
			{
				U8(rt, 0x42) = (uint8_t)(e & 0x7F);
				CUR() = PTR(CTX(), 0x90) + ((uint32_t)U8(RT(), 0x42) << 8);
				U8(RT(), 0x43) = 0;
				do
				{
					uint8_t *bone = CUR();
					int32_t sp = S32(bone, 4 * U8(RT(), 0x43));
					STREAM() = (uint8_t *)sp;
					if (NEG ? sp < 0 : sp > 0)
					{
						int16_t *wait = &S16(bone, 0xC + 2 * U8(RT(), 0x43));
						if (*wait > 0) *wait = (int16_t)(*wait - S16(bone, 0xC8));
						else
						{
							int16_t op = S16((uint8_t *)sp, 0);
							S16(RT(), 0x4A) = op;
							call_vm((uint32_t)(int32_t)op);
							while (S16(RT(), 0x3E) == 0)
							{
								op = S16(STREAM(), 0);
								S16(RT(), 0x4A) = op;
								call_vm((uint32_t)(int32_t)op);
							}
							uint8_t ch = U8(RT(), 0x43);
							int16_t req = S16(RT(), 0x3E);
							uint8_t *cb = CUR();
							req = (int16_t)(req - S16(cb, 0xC8));
							S16(cb, 0xC + 2 * ch) = (int16_t)(S16(cb, 0xC + 2 * ch) + req);
							U16(RT(), 0x3E) = 0;
							S32(CUR(), 4 * U8(RT(), 0x43)) = (int32_t)STREAM();
						}
					}
					U8(RT(), 0x43) = (uint8_t)(U8(RT(), 0x43) + 1);
				} while (U8(RT(), 0x43) < 3);
			}
			rt = RT();
			U8(rt, 0x44) = (uint8_t)(U8(rt, 0x44) + 1);
			e = ORDER()[U8(RT(), 0x44)];
			if (e == 0xFF) break;
		}
	}

	static void Integrator()
	{
		U8(RT(), 0x44) = 0;
		uint32_t e = ORDER()[U8(RT(), 0x44)];
		if (e == 0xFF) return;
		for (;;)
		{
			uint8_t *rt = RT();
			if (U8(rt, 0x45) == 0 || (e & 0x80))
			{
				U8(rt, 0x42) = (uint8_t)(e & 0x7F);
				uint8_t *b = PTR(CTX(), 0x90) + ((uint32_t)U8(RT(), 0x42) << 8);
				CUR() = b;
				uint8_t fl = U8(b, 0x1A);
				if (fl & 1)
				{
					S32(CUR(), 0x68) = add32(S32(CUR(), 0x68), shl32(S16(CUR(), 0x80), 12));
					S32(CUR(), 0x6C) = add32(S32(CUR(), 0x6C), shl32(S16(CUR(), 0x82), 12));
					S32(CUR(), 0x70) = add32(S32(CUR(), 0x70), shl32(S16(CUR(), 0x84), 12));
				}
				if (fl & 8)
				{
					S32(CUR(), 0x74) = add32(S32(CUR(), 0x74), shl32(S16(CUR(), 0x86), 12));
					S32(CUR(), 0x78) = add32(S32(CUR(), 0x78), shl32(S16(CUR(), 0x88), 12));
					S32(CUR(), 0x7C) = add32(S32(CUR(), 0x7C), shl32(S16(CUR(), 0x8A), 12));
				}
				for (int k = 0; k < 6; k++)
					S32(CUR(), 0x50 + 4 * k) = add32(S32(CUR(), 0x50 + 4 * k), S32(CUR(), 0x68 + 4 * k));
				S16(CUR(), 0x8C) = (int16_t)(S32(CUR(), 0x50) >> 16);
				S16(CUR(), 0x8E) = (int16_t)(S32(CUR(), 0x54) >> 16);
				S16(CUR(), 0x90) = (int16_t)(S32(CUR(), 0x58) >> 16);
				C().bone[U8(CUR(), 0x18)]();
			}
			rt = RT();
			U8(rt, 0x44) = (uint8_t)(U8(rt, 0x44) + 1);
			e = ORDER()[U8(RT(), 0x44)];
			if (e == 0xFF) break;
		}
	}

	static void Dispatch()
	{
		U32(WS(), 0) = 0;
		U32(WS(), 4) = U32(CTX(), 0x7C);
		uint8_t id = DRAWORDER()[U32(WS(), 0)];
		if (id == 0xFF) return;
		for (;;)
		{
			U8(RT(), 0x42) = id;
			uint8_t *rt = RT();
			uint8_t *b = PTR(CTX(), 0x90) + ((uint32_t)U8(rt, 0x42) << 8);
			CUR() = b;
			U32(rt, 0x4C) = U32(rt, 0x38) + 4 * (U16(b, 0x48) & 0xFFF);
			if (C().lit_dispatcher)
			{
				int32_t slot = S8(CUR(), 0xE1);
				if (slot != 0)
				{
					uint8_t *lt = LIGHTS() + 0x50 * (slot & 0xF);
					x::SetColorMatrix(lt + 0x20);
					x::SetBackColor(S32(lt, 0x40), S32(lt, 0x44), S32(lt, 0x48));
					S32(WS(), 0x80) = (S32(CUR(), 0xCC) & 0xFF) << 5;
					S32(WS(), 0x84) = (S32(CUR(), 0xCC) >> 3) & 0x1FE0;
					S32(WS(), 0x88) = (S32(CUR(), 0xCC) >> 11) & 0x1FE0;
					uint8_t *m = WS() + 0x20;
					if (slot & 0x80)
					{
						PTR(WS(), 0xFC) = (uint8_t *)blob::GetParentMatrix(U16(CUR(), 0x9C));
						x::MulMatrix3(lt, PTR(WS(), 0xFC), m);
						x::ScaleMatrix(m, WS() + 0x80);
						x::SetLightMatrix(m);
					}
					else
					{
						ff8fx::copy<(20)>(m, lt);
						x::ScaleMatrix(m, WS() + 0x80);
						x::SetLightMatrix(m);
					}
				}
			}
			if (U8(CUR(), 0x4C) & 1)
			{
				U32(WS(), 4) = U32(CTX(), 0x7C);
				U32(CTX(), 0x7C) = U32(CTX(), 0xD8);
				C().draw[U8(CUR(), 0x1C)]();
				U32(CTX(), 0xD8) = U32(CTX(), 0x7C);
				U32(CTX(), 0x7C) = U32(WS(), 4);
			}
			else C().draw[U8(CUR(), 0x1C)]();
			U32(WS(), 0) = U32(WS(), 0) + 1;
			id = DRAWORDER()[U32(WS(), 0)];
			if (id == 0xFF) break;
		}
	}
}

	void BindContext()
	{
		uint8_t *a = (uint8_t *)x::FieldAlloc(0x180);
		uint8_t *rt = a + 0x100;
		const uint32_t *pb = (const uint32_t *)C().ptr_block;
		PTR(rt, 0) = a;
		WS() = a;
		PTR(rt, 4) = rt;
		RT() = rt;
		U32(rt, 8) = pb[0];
		CTX() = (uint8_t *)pb[0];
		U32(rt, 0xC) = pb[1];
		PTR3() = (uint8_t *)pb[1];
		U32(rt, 0x10) = pb[2];
		SCENE() = (uint8_t *)pb[2];
		U32(rt, 0x14) = pb[3];
		RCTX() = (uint8_t *)pb[3];
		U32(rt, 0x18) = pb[4];
		SEQ() = (uint8_t *)pb[4];
	}

	void ReleaseContext() { x::FieldFree(0x180); }

	void AnimChannelsNeg() { part_core::AnimChannels<true>(); }
	void AnimChannelsPos() { part_core::AnimChannels<false>(); }
	void AnimIntegrator() { part_core::Integrator(); }
	void BuildMatricesAndDraw() { part_core::Dispatch(); }

	int32_t h_B2F8F0()
	{
		U32(SEQ(), 8) = 0x80000000;
		U32(CTX(), 0x74) = U32(CTX(), 0x70);
		uint8_t *f1 = RESFILE()[1];
		PTR(CTX(), 0x90) = f1 + U32(f1, 0x1C);
		uint8_t *bones = PTR(CTX(), 0x90);
		ff8fx::fill<(0x80)>(ORDER(), 0xFF);
		CUR() = bones;
		ff8fx::fill<(0x80)>(DRAWORDER(), 0xFF);
		ORDER()[0] = 0;
		U16(SCENE(), 2) = 0;
		U16(SCENE(), 0x18) = 1;
		U16(SCENE(), 0x1A) = 0;
		U16(SCENE(), 0) = 0x8000;
		U32(SCENE(), 0x40) = 0;
		U32(SEQ(), 0xC) = 0x4000080;
		U32(SCENE(), 0x14) = 0;
		U16(SCENE(), 0x38) = 8;
		U8(SCENE(), 0x3A) = 0;
		MEM<uint8_t>(game_addr(game::g_GfCinematic_FileLoadBusy)) = 0;
		MEM<uint8_t>(game_addr(game::unk_2798218)) = 0xFF;
		U8(RT(), 0x45) = 0;
		U8(CTX(), 0xA2) = 0;
		U32(SCENE(), 0x170) = 0;
		U32(SCENE(), 0x16C) = 0;
		U32(SCENE(), 0x168) = 0;
		uint8_t *sc = SCENE();
		ff8fx::fill<(0x80)>(NODEKEYS(), 0);
		uint8_t *b = CUR();
		if (U16(sc, 0x38) != 0)
		{
			uint32_t i = 0;
			do
			{
				U32(b, 0) = 0;
				b += 0x100;
				i++;
			} while (i < U16(SCENE(), 0x38));
		}
		blob::InitBone(0);
		U16(CUR(), 0x12) = 0;
		h_B2FC20();
		uint8_t *s0 = SCENE();
		for (int k = 0; k < 7; k++)
		{
			uint32_t e = game_addr(game::unk_1D97358) + 0x9C * k;
			uint8_t *s = SCENE();
			U32(s, 0x60 + 4 * k) = e - 0x98;
			U32(s0, 0x84 + 8 * k) = MEM<uint32_t>(e - 0x7C);
			U32(s0, 0x88 + 8 * k) = MEM<uint32_t>(e - 0x78);
			U32(s0, 0x120 + 8 * k) = MEM<uint32_t>(e - 4);
			U32(s0, 0x124 + 8 * k) = MEM<uint32_t>(e);
			U32(s0, 0xD8 + 8 * k) = MEM<uint32_t>(e - 0x8C);
			U32(s0, 0xDC + 8 * k) = MEM<uint32_t>(e - 0x88);
		}
		U32(CUR(), 0) = U32(CTX(), 0x94);
		return 0;
	}

	int32_t h_B2FC20()
	{
		uint8_t *ctx = CTX();
		uint8_t *cast = PTR(ctx, 0xC0);
		uint8_t *list = PTR(cast, 4);
		PTR(ctx, 0xCC) = list + 20 * (uint32_t)U8(ctx, 0xD0);
		uint8_t *act = PTR(CTX(), 0xCC);
		U8(SCENE(), 0x44) = U8(cast, 0);
		U8(SCENE(), 0x48) = U8(PTR(PTR(cast, 4), 8), 0);
		uint8_t *dst = SCENE() + 0x4E;
		for (int k = 0; k < 7; k++) U16(dst, 2 * k) = MEM<uint16_t>(game_addr(game::BattleEntitySlotData) + 0x9C * k);
		const uint8_t *t = PTR(act, 8);
		int32_t n = U8(act, 0x10);
		uint32_t mask = 0;
		uint32_t cnt = 0;
		if (n > 0)
		{
			for (int32_t i = n; i != 0; i--)
			{
				uint8_t id = t[0];
				uint32_t bit = 1u << (id & 31);
				if (!(bit & mask))
				{
					mask |= bit;
					U8(SCENE(), 0x48 + cnt) = id;
					U8(SCENE(), 0xCC + id) = (uint8_t)cnt;
					cnt++;
				}
				t += 0x18;
			}
		}
		U8(CUR(), 0x1B) = U8(SCENE(), 0x48);
		U8(SCENE(), 0x41) = (uint8_t)cnt;
		return 0;
	}

	uint32_t SequenceTick(Clone &c)
	{
		g_clone = &c;
		BindContext();
		S16(CTX(), 0x32) = (int16_t)(S16(CTX(), 0x32) + 1);
		U8(RT(), 0x41) = U8(CTX(), 0x32) & 1;
		x::Rand();
		if (U16(CTX(), 0) & 0x400)
		{
			U32(RCTX(), 0x2C) = FRAMERL() + 0xC;
			U16(RCTX(), 0x24) = 0;
		}
		U8(CTX(), 0x35) = (UPDFLAGS() & 1) ? 0xFF : 0;
		U8(RT(), 0x40) = U8(RT(), 0x41);
		U32(RT(), 0x38) = FRAMERL() + 0x44;
		{
			const uint32_t *v = (const uint32_t *)game_addr(game::unk_1D97778);
			uint32_t v0 = v[0], v1 = v[1], v2 = v[2], v3 = v[3], v4 = v[4] & 0xFFFF, v5 = v[5], v6 = v[6], v7 = v[7];
			uint32_t *m0 = (uint32_t *)NODEMAT(), *ma = (uint32_t *)CAMALT();
			m0[5] = v5; m0[0] = v0; ma[0] = v0; m0[1] = v1; m0[4] = v4; m0[6] = v6; ma[1] = v1; ma[4] = v4; ma[5] = v5;
			m0[2] = v2; m0[3] = v3; m0[7] = v7; ma[2] = v2; ma[3] = v3; ma[6] = v6; ma[7] = v7;
			U32(CTX(), 0x7C) = PKTCUR();
		}
		U32(CTX(), 0xD8) = U32(CTX(), 0xD4);
		U32(CTX(), 0x88) = add32(mul32(S8(RT(), 0x40), S32(CTX(), 0x8C)), S32(CTX(), 0x84));
		if (U8(CTX(), 1) & 0x80) x::ClearOT(U32(RT(), 0x38), 0x1000);
		if (U8(CTX(), 1) & 0x80) part_core::DebugPadRootControl();
		U8(CTX(), 0xD2) = 0;
		if (U8(CTX(), 0x35) == 0)
		{
			AnimChannelsNeg();
			AnimIntegrator();
			AnimChannelsPos();
		}
		U16(RCTX(), 4) = 0;
		U16(RT(), 0x46) = 0;
		BuildMatricesAndDraw();
		x::ParseCamera2(PROJH());
		PKTCUR() = U32(CTX(), 0x7C);
		ReleaseContext();
		return (~(uint32_t)U16(SEQ(), 0xA) >> 14) & 2;
	}

	static void __cdecl noop() {}

	void fill_core(Generic &g)
	{
		g.bone[0] = part_core::bh_00_AccumPos;
		g.bone[1] = part_core::bh_00_AccumPos;
		g.bone[3] = part_core::bh_03_Polar;
		g.bone[5] = part_core::bh_05_CopyBone;
		g.bone[6] = part_core::bh_06_LerpBones;
		g.bone[7] = part_core::bh_07_BonePlusAccum;
		g.bone[9] = part_core::bh_09_EntityJoint;
	}

	void fill_generic()
	{
		static bool done = false;
		if (done) return;
		done = true;
		ff8fx::fill<(sizeof(g_generic))>(&g_generic, 0);
		fill_core(g_generic);
		fill_vm_a(g_generic);
		fill_vm_b(g_generic);
		fill_vm_c(g_generic);
		fill_vm_d(g_generic);
		fill_draw_mesh(g_generic);
		fill_draw_prim(g_generic);
		fill_draw_sprite(g_generic);
	}

	static VoidFn port_of(uint32_t orig, VoidFn gen, int tid, int index, const Exception *ex, int nex)
	{
		if (orig == 0) return nullptr;
		if (!is_game_code(orig)) return (VoidFn)orig;
		if (*(const uint8_t *)orig == 0xC3) return noop;
		for (int k = 0; k < nex; k++)
			if (ex[k].table == tid && ex[k].index == index) return (VoidFn)orig;
		return gen ? gen : (VoidFn)orig;
	}

	static Clone *g_clones[16];
	static int g_nclones = 0;

	Clone *find_clone(int effect_id)
	{
		for (int i = 0; i < g_nclones; i++)
			if (g_clones[i]->effect_id == effect_id) return g_clones[i];
		return nullptr;
	}

	void init_clone(Clone &c, const Exception *ex, int nex)
	{
		fill_generic();
		if (!find_clone(c.effect_id) && g_nclones < 16) g_clones[g_nclones++] = &c;
		for (int i = 0; i < 0x200; i++)
			c.vm[i] = port_of(*(const uint32_t *)(c.vm_table + 4 * i), i < 0x147 ? g_generic.vm[i] : nullptr, 0, i, ex, nex);
		struct Sub { uint32_t base; int count; int tid; VoidFn const *gen; } subs[] = {
			{ c.bone_table, c.bone_count, 1, g_generic.bone },
			{ c.prim_table, 60, 2, g_generic.prim },
			{ c.spr_table, 11, 3, g_generic.spr },
			{ c.ptype_table, 5, 4, g_generic.ptype },
			{ c.pop_table, 24, 5, g_generic.pop },
			{ c.draw_table, 73, 6, g_generic.draw },
		};
		for (int j = 0; j < 256; j++)
		{
			uint32_t a = c.bone_table + 4 * j;
			VoidFn gen = nullptr;
			int tid = -1, index = 0;
			for (const Sub &sb : subs)
				if (a >= sb.base && a < sb.base + 4 * (uint32_t)sb.count)
				{
					index = (int)(a - sb.base) / 4;
					tid = sb.tid;
					gen = (tid == 1 && index >= 32) ? nullptr : sb.gen[index];
				}
			c.bigtab[j] = port_of(*(const uint32_t *)a, gen, tid, index, ex, nex);
		}
		c.bone = c.bigtab;
		c.prim = c.bigtab + (c.prim_table - c.bone_table) / 4;
		c.spr = c.bigtab + (c.spr_table - c.bone_table) / 4;
		c.ptype = c.bigtab + (c.ptype_table - c.bone_table) / 4;
		c.pop = c.bigtab + (c.pop_table - c.bone_table) / 4;
		for (int i = 0; i < 256; i++)
			c.draw[i] = port_of(*(const uint32_t *)(c.draw_table + 4 * i), i < 73 ? g_generic.draw[i] : nullptr, 6, i, ex, nex);
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace part_vm_a
{
	inline uint8_t &LoadBusy() { return MEM<uint8_t>(game_addr(game::g_GfCinematic_FileLoadBusy)); }
	inline uint8_t &LoadDone() { return MEM<uint8_t>(game_addr(game::unk_2798218)); }
	inline uint8_t &Byte1D96DC4() { return MEM<uint8_t>(game_addr(game::GF_CINEMATIC_PARTY_HINT)); }
	inline uint32_t &Ptr1D99A88() { return MEM<uint32_t>(game_addr(game::MAGIC_TEXTURE_BUFFER_PTR)); }
	inline uint32_t &CamWord32(int i) { return MEM<uint32_t>(game_addr(game::Battle_Camera_world_XZ_s16) + 4 * i); }
	inline uint16_t &Word1D977A0() { return MEM<uint16_t>(game_addr(game::Battle_Camera_ReturnView_Param1D8E038)); }
	inline uint16_t &Word1D977A2() { return MEM<uint16_t>(game_addr(game::g_BattleCam_Roll)); }
	inline uint16_t &Word1D9771C() { return MEM<uint16_t>(game_addr(game::Battle_Camera_ReturnView_Roll)); }
	inline uint16_t *CamShake() { return (uint16_t *)game_addr(game::BATTLE_CAMERA_SHAKE_OFFSET_X); }
	inline uint8_t *Entity(int32_t i) { return (uint8_t *)(game_addr(game::BattleEntitySlotData) + i * 0x9C); }
	inline uint8_t *SlotData(uint32_t slot) { return (uint8_t *)(game_addr(game::unk_1D27B1C) + slot * 0xD0); }

	[[maybe_unused]] static void __cdecl cb_B2BB40_LoadDone()
	{
		LoadBusy() = 0;
	}

	static uint8_t *StreamSource_B25DB0(int32_t slot)
	{
		uint8_t *ctx = CTX();
		uint32_t b = U8(PTR(ctx, 0xB0), slot);
		if (b == 0xFF) return PTR(ctx, 0xB8);
		uint8_t *f = RESFILE()[b];
		uint8_t *tab = f + U32(f, 0x18);
		return tab + U32(tab, slot * 4);
	}

	static int32_t StepModelFromVm_B269B0(uint8_t *blk)
	{
		h_B269E0();
		U16(blk, 2) &= 0xFFFD;
		h_B26AD0(blk);
		U8(blk, 2) |= 2;
		return 0;
	}

	static void EntityRestore_B2B700(int32_t start, int32_t count)
	{
		uint8_t *keep = SCENE() + start * 2 + 0x4E;
		uint8_t *ent = Entity(start);
		if (count <= 0) return;
		do
		{
			if (!(U8(keep, 0) & 4))
			{
				U16(ent, 0) &= 0xFFFB;
			}
			keep += 2;
			ent += 0x9C;
		} while (--count);
	}

	static void EntityHide(int32_t start, int32_t count)
	{
		uint8_t *ent = Entity(start);
		if (count <= 0) return;
		do
		{
			U16(ent, 0) |= 4;
			ent += 0x9C;
		} while (--count);
	}

	static void __cdecl op_002_Jump()
	{
		uint8_t *s = STREAM();
		STREAM() = s + S16(s, 2);
	}

	static void __cdecl op_003_Call()
	{
		uint32_t ch = U8(RT(), 0x43);
		uint8_t *sp = CUR() + ch + 0x44;
		uint8_t d8 = *sp;
		int32_t depth = (int8_t)d8;
		*sp = (uint8_t)(d8 + 1);
		uint8_t *s = STREAM();
		uint32_t ch2 = U8(RT(), 0x43);
		uint8_t *bone = CUR();
		int32_t off = S16(s, 2);
		int32_t idx = depth + (int32_t)ch2 * 2;
		PTR(bone, idx * 4 + 0x24) = s + 4;
		STREAM() = STREAM() + off;
	}

	static void __cdecl op_004_Return()
	{
		uint32_t ch = U8(RT(), 0x43);
		uint8_t *sp = CUR() + ch + 0x44;
		*sp = (uint8_t)(*sp - 1);
		ch = U8(RT(), 0x43);
		uint8_t *bone = CUR();
		int32_t depth = S8(bone, ch + 0x44);
		STREAM() = PTR(bone, (depth + (int32_t)ch * 2) * 4 + 0x24);
	}

	static void __cdecl op_01E_SetLoopCounter()
	{
		uint8_t v = U8(STREAM(), 2);
		uint32_t k = (uint32_t)U16(RT(), 0x4A) >> 14;
		U8(CUR(), k + 0x20) = v;
		STREAM() += 4;
	}

	static void __cdecl op_01F_LoopDecJump()
	{
		uint32_t k = (uint32_t)U16(RT(), 0x4A) >> 14;
		uint8_t *b = CUR();
		U8(b, k + 0x20) = (uint8_t)(U8(b, k + 0x20) - 1);
		uint8_t c = U8(CUR(), k + 0x20);
		uint8_t *s = STREAM();
		if (c == 0)
			STREAM() = s + 4;
		else
			STREAM() = s + S16(s, 2);
	}

	static void __cdecl op_0A7_SetCounter()
	{
		uint8_t *s = STREAM();
		int32_t i = S16(s, 2);
		uint8_t v = U8(s, 4);
		U8(SCENE(), i + 0x14) = v;
		STREAM() += 6;
	}

	static void __cdecl op_0A8_DecCounterJumpNZ()
	{
		int32_t i = S16(STREAM(), 2);
		uint8_t *sc = SCENE();
		U8(sc, i + 0x14) = (uint8_t)(U8(sc, i + 0x14) - 1);
		uint8_t c = U8(SCENE(), i + 0x14);
		uint8_t *s = STREAM();
		if (c == 0)
			STREAM() = s + 6;
		else
			STREAM() = s + S16(s, 4);
	}

	static void __cdecl op_0A1_RandomJump()
	{
		int32_t r = blob::Rand73(0x100);
		if (r < 0) r = (int32_t)(0u - (uint32_t)r);
		uint8_t *s = STREAM();
		if (r <= (int32_t)S16(s, 2))
			STREAM() = s + S16(s, 4);
		else
			STREAM() = s + 6;
	}

	static void __cdecl op_046_JumpIfTargetResult()
	{
		uint8_t *act = PTR(CTX(), 0xCC);
		uint32_t slot = U8(CUR(), 0x1B);
		uint8_t *rec = PTR(act, 8);
		int32_t n = U8(act, 0x10);
		while (n > 0)
		{
			if (U8(rec, 0) == slot) break;
			rec += 0x18;
			n--;
		}
		uint32_t sel = U16(RT(), 0x4A);
		uint8_t b3 = U8(rec, 3);
		bool jump;
		switch ((sel >> 9) - 1)
		{
		case 0: jump = (b3 & 4) != 0; break;
		case 1: jump = (U8(rec, 2) & 8) != 0; break;
		case 2: jump = (b3 & 2) != 0; break;
		case 3: jump = (U32(SlotData(slot), 0) & 0x2000) != 0; break;
		default: jump = (b3 & 4) == 0; break;
		}
		uint8_t *s = STREAM();
		if (jump)
			STREAM() = s + S16(s, 2);
		else
			STREAM() = s + 4;
	}

	static void __cdecl op_055_JumpIfSummonState()
	{
		uint8_t *ctx = CTX();
		uint8_t *rec = PTR(ctx, 0xC0);
		uint32_t sel = (uint32_t)U16(RT(), 0x4A) >> 9;
		bool jump;
		if (sel == 1)
			jump = U8(ctx, 0xD0) != U8(ctx, 0xD1);
		else if (sel == 2)
		{
			uint8_t c = U8(rec, 1);
			jump = (c & 3) && !(c & 2);
		}
		else if (sel == 3)
		{
			uint8_t c = U8(rec, 1);
			jump = (c & 3) && (c & 2);
		}
		else
			jump = U8(ctx, 0xD0) >= 1;
		uint8_t *s = STREAM();
		if (jump)
			STREAM() = s + S16(s, 2);
		else
			STREAM() = s + 4;
	}

	static void __cdecl op_057_Nop2() { STREAM() += 2; }
	static void __cdecl op_0A5_Nop4() { STREAM() += 4; }
	static void __cdecl op_0A6_Nop4() { STREAM() += 4; }
	static void __cdecl op_0B8_Nop8() { STREAM() += 8; }

	static void __cdecl op_02B_PlaySE()
	{
		uint8_t *s = STREAM();
		uint8_t *tab = PTR(CTX(), 0xC4);
		int32_t attr = S16(s, 2);
		uint32_t idx = (uint32_t)U16(RT(), 0x4A) >> 9;
		x::PlaySE(tab + U32(tab, idx * 4), attr, 0x80);
		STREAM() += 4;
	}

	static void __cdecl op_031_TransSummonStream()
	{
		if (LoadBusy() != 0)
		{
			U16(RT(), 0x3E) = U16(CUR(), 0xC8);
			return;
		}
		uint8_t *src = StreamSource_B25DB0(((uint32_t)U16(RT(), 0x4A) >> 9) & 0xF);
		uint8_t *ready = CTX() + 0xA3;
		if (U8(RT(), 0x4B) & 0x80)
		{
			Byte1D96DC4() = 0;
		}
		x::TransSummonStream(src, ready);
		STREAM() += 2;
	}

	static void __cdecl op_034_WaitStreamReady()
	{
		if (U8(CTX(), 0xA3) == 0)
		{
			U16(RT(), 0x3E) = U16(CUR(), 0xC8);
			return;
		}
		STREAM() += 2;
	}

	static void __cdecl op_030_PlaySummonStream()
	{
		uint32_t a = (uint32_t)U16(RT(), 0x4A) >> 9;
		x::PlaySummonStream(0x80, 1, (int32_t)a);
		STREAM() += 2;
	}

	static void __cdecl op_02C_StopSoundChannels()
	{
		uint32_t m = ((uint32_t)U16(RT(), 0x4A) >> 9) & 0xF;
		x::Snd_46B450(0, (int32_t)m);
		STREAM() += 2;
	}

	static void __cdecl op_053_MusicVolume()
	{
		uint8_t *rt = RT();
		int32_t v = S16(STREAM(), 2);
		if (U8(rt, 0x4B) & 0x80)
			x::MusicSetVolumeTrans(0, v, 0);
		else
			x::Music_47E3C0(v);
		STREAM() += 4;
	}

	static void __cdecl op_0C3_PauseResumeSounds()
	{
		if (U8(RT(), 0x4B) & 0x80)
		{
			x::Snd_46B3E0(2);
			x::Snd_46B3E0(3);
		}
		else
		{
			x::Snd_46B3A0(2);
			x::Snd_46B3A0(3);
		}
		STREAM() += 2;
	}

	static void __cdecl op_006_LoadFile()
	{
		if (LoadBusy() != 0)
		{
			U16(RT(), 0x3E) = U16(CUR(), 0xC8);
			return;
		}
		uint8_t *rt = RT();
		LoadBusy() = 0xFF;
		uint8_t *ctx = CTX();
		uint32_t w = U16(rt, 0x4A);
		uint32_t dst;
		uint32_t id;
		bool store = true;
		if (w & 0x8000)
		{
			dst = U32(ctx, 0xB8);
			id = (w >> 9) & 0x3F;
			if (id & 0x20)
			{
				id = (id & 0x1F) + 0x16B;
				store = false;
			}
		}
		else
		{
			uint32_t src = U16(STREAM(), 2);
			uint32_t lo = src & 0xFF;
			uint32_t hi = src >> 8;
			id = w >> 9;
			if (lo == 0xFF)
				dst = U32(ctx, 0xB8);
			else if (lo & 0x80)
				dst = (uint32_t)RESFILE()[lo & 0x7F] + (hi << 12);
			else
			{
				dst = (uint32_t)RESFILE()[lo];
				if ((hi & 0xFF) != 0xFF)
				{
					uint32_t d = C().desc;
					uint32_t t = d + U32((void *)d, 0x1C);
					t += U32((void *)t, lo * 4);
					dst += U32((void *)t, hi * 4);
				}
			}
		}
		if (store)
		{
			id += U8(ctx, 0xA2);
			RESFILE()[id] = (uint8_t *)dst;
			id += (uint32_t)(int32_t)S16(ctx, 0xA0);
		}
		U32(ctx, 0xB4) = dst;
		x::PreLoadBattleFile((int32_t)(id & 0xFFFF), (void *)dst, 0, C().load_cb);
		if (S16(RT(), 0x4A) <= 0)
			STREAM() += 2;
		else
			STREAM() += 4;
	}

	static void __cdecl op_008_WaitLoadDone()
	{
		if (LoadBusy() == 0 && LoadDone() != 0)
		{
			STREAM() += 2;
			return;
		}
		U16(RT(), 0x3E) = U16(CUR(), 0xC8);
	}

	static void __cdecl op_07B_WaitThenReloadA8defTim()
	{
		if (LoadDone() != 0)
		{
			x::Battle_508630(PTR(CTX(), 0xB8), &LoadDone());
			STREAM() += 2;
			return;
		}
		U16(RT(), 0x3E) = U16(CUR(), 0xC8);
	}

	static void __cdecl op_0B2_SetCtxByteA2()
	{
		U8(CTX(), 0xA2) = U8(STREAM(), 2);
		STREAM() += 4;
	}

	static void __cdecl op_021_SetDataPointer()
	{
		uint8_t *s = STREAM();
		uint32_t off = ((uint32_t)U16(s, 4) << 16) | U16(s, 2);
		uint32_t w = U16(RT(), 0x4A);
		uint32_t base;
		if (w & 0x8000)
		{
			uint8_t *f = RESFILE()[1];
			base = (uint32_t)f + U32(f, 0x1C);
		}
		else
			base = Ptr1D99A88();
		switch (((int32_t)w >> 12) & 7)
		{
		case 1:
			U32(CTX(), 0xB8) = base + off;
			break;
		case 2:
		{
			U32(CTX(), 0xD4) = base + off;
			int32_t t = shl32(S16(STREAM(), 6), 8);
			int32_t half = t / 2;
			S32(CTX(), 0xDC) = half;
			int32_t ping = S8(RT(), 0x41);
			S32(CTX(), 0xD8) = add32(mul32(ping, half), S32(CTX(), 0xD4));
			break;
		}
		default:
			U32(CTX(), 0x74) = base + off;
			U32(CTX(), 0x70) = U32(CTX(), 0x74);
			break;
		}
		STREAM() += 8;
	}

	static void __cdecl op_120_SetArenaTop()
	{
		int32_t v = S16(STREAM(), 2);
		uint8_t *ctx = CTX();
		S32(ctx, 0x74) = add32(shl32(v, 16), S32(ctx, 0x70));
		STREAM() += 4;
	}

	static void __cdecl op_033_SetBoneId()
	{
		U16(CUR(), 0x12) = U16(STREAM(), 2);
		STREAM() += 4;
	}

	static void __cdecl op_0CE_SetBoneIdFromSlot()
	{
		uint8_t *b = CUR();
		U16(b, 0x12) = (uint16_t)(U8(b, 0x1B) + U16(STREAM(), 2));
		STREAM() += 4;
	}

	static void __cdecl op_064_SetBoneSlot()
	{
		uint32_t w = U16(RT(), 0x4A);
		uint8_t v;
		if (w & 0x8000)
			v = U8(PTR(CTX(), 0xC0), 0);
		else if (w & 0x1000)
		{
			U8(CUR(), 0x1B) = U8(STREAM(), 2);
			STREAM() += 4;
			return;
		}
		else
			v = U8(SCENE(), 0x48);
		U8(CUR(), 0x1B) = v;
		STREAM() += 2;
	}

	static void __cdecl op_0B1_SetSeqStateByte6()
	{
		U8(SEQ(), 6) = U8(STREAM(), 2);
		STREAM() += 4;
	}

	static void __cdecl op_0CD_SetRenderListOffset()
	{
		U16(CUR(), 0x48) = U16(STREAM(), 2);
		STREAM() += 4;
	}

	static void __cdecl op_044_SetDrawHandlerInlineDataReset()
	{
		blob::SetDrawHandler(S16(STREAM(), 2));
		uint8_t *s = STREAM();
		int32_t n = S16(s, 4);
		PTR(CUR(), 0xB8) = s + 6;
		U32(CUR(), 0xBC) = 0;
		U32(CUR(), 0xC0) = 0;
		U32(CUR(), 0xC4) = 0;
		STREAM() = STREAM() + shl32(n, 1) + 6;
	}

	static void __cdecl op_043_SetDrawHandlerInlineData()
	{
		blob::SetDrawHandler(S16(STREAM(), 2));
		uint8_t *s = STREAM();
		int32_t n = S16(s, 4);
		PTR(CUR(), 0xB8) = s + 6;
		U32(CUR(), 0xBC) = 0;
		STREAM() = STREAM() + shl32(n, 1) + 6;
	}

	static void __cdecl op_04E_SetDescriptorInlineData()
	{
		uint8_t *s = STREAM();
		int32_t k = S16(s, 2);
		PTR(CUR(), k * 4 + 0xB8) = s + 6;
		s = STREAM();
		STREAM() = s + S16(s, 4) * 2 + 6;
	}

	static void __cdecl op_03B_SequenceFlags()
	{
		uint8_t *seq = SEQ();
		int32_t v = S16(STREAM(), 2);
		if (v & 0x8000)
			U16(seq, 8) &= (uint16_t)~(v & 0x7FFF);
		else
			U16(seq, 8) |= (uint16_t)(v & 0x7FFF);
		STREAM() += 4;
	}

	static void __cdecl op_007_SetFreezeOthers()
	{
		uint8_t *rt = RT();
		uint32_t w = U16(rt, 0x4A);
		if (w & 0x8000)
		{
			U8(rt, 0x45) = 0xFF;
			rt = RT();
		}
		if (w & 0x4000)
		{
			U8(rt, 0x45) = 0;
			rt = RT();
		}
		uint32_t i = U8(rt, 0x44);
		ORDER()[i] |= 0x80;
		STREAM() += 2;
	}

	static void __cdecl op_04D_SetupScreenTint()
	{
		U8(CUR(), 0x18) = 0;
		U16(CUR(), 0x48) = 0;
		blob::SetDrawHandler(4);
		STREAM() += 2;
	}

	static void __cdecl op_03F_CreateEmbeddedModel()
	{
		uint8_t *blk = h_B26860(S16(STREAM(), 2));
		PTR(CUR(), 0xBC) = blk;
		blob::SetDrawHandler(3);
		STREAM() += 4;
	}

	static void __cdecl op_041_EmbeddedModelFlags()
	{
		uint8_t *s = STREAM();
		uint8_t *blk = PTR(CUR(), 0xBC);
		uint8_t *rt = RT();
		uint8_t *arg = s + 2;
		uint32_t sub = (uint32_t)U16(rt, 0x4A) >> 12;
		int32_t v = S16(arg, 0);
		switch (sub)
		{
		case 0:
			U16(blk, 2) &= (uint16_t)~v;
			STREAM() += 4;
			break;
		case 1:
			U8(blk, 2) |= 4;
			PTR(blk, 0x7C) = arg;
			STREAM() += 6;
			break;
		case 8:
			U16(blk, 2) |= (uint16_t)v;
			STREAM() += 4;
			break;
		default:
			break;
		}
	}

	static void __cdecl op_089_StepEmbeddedModelAnim()
	{
		StepModelFromVm_B269B0(PTR(CUR(), 0xBC));
		STREAM() += 2;
	}

	static void __cdecl op_058_CreateParticlePool()
	{
		int32_t n = S16(STREAM(), 2);
		uint8_t *pool = blob::ArenaAlloc(n * 0x50 + 0x10);
		PTR(CUR(), 0xB8) = pool;
		uint8_t *p = pool + 0x10;
		U16(pool, 0) = (uint16_t)n;
		U16(pool, 2) = 0;
		U16(pool, 4) = 0;
		U16(pool, 6) = 0;
		for (int32_t i = n; i > 0; i--)
		{
			U16(p, 0) = 0;
			U32(p, 8) = 0;
			U32(p, 0x30) = 0;
			U32(p, 4) = 0;
			p += 0x50;
		}
		blob::SetDrawHandler(6);
		STREAM() += 4;
	}

	static void __cdecl op_059_EmitParticle()
	{
		uint8_t *bone;
		if (U8(RT(), 0x4B) & 0x10)
		{
			bone = blob::GetBone(S16(STREAM(), 2));
			if (bone == PTR(CTX(), 0x90))
			{
				STREAM() += 4;
				return;
			}
			STREAM() += 2;
		}
		else
			bone = CUR();
		uint8_t *pool = PTR(bone, 0xB8);
		uint32_t idx = U16(pool, 2);
		int32_t cnt = U16(pool, 0);
		U16(pool, 2) = (uint16_t)(idx + 1);
		int32_t slot = (int32_t)idx % cnt;
		U16(pool, 8) = U8(bone, 0x18);
		uint8_t *p = pool + slot * 0x50 + 0x10;
		U16(p, 0) = 1;
		U16(p, 2) = 0;
		uint8_t *s = STREAM();
		PTR(p, 4) = s + S16(s, 2);
		U32(WS(), 0x38) = 0;
		U32(WS(), 0x3C) = 0;
		U32(p, 0x20) = 0;
		U32(p, 0x24) = 0;
		U32(p, 0x28) = 0;
		U32(p, 0x2C) = 0;
		S32(p, 0x14) = shl32(S16(CUR(), 0x94), 8);
		S32(p, 0x18) = shl32(S16(CUR(), 0x96), 8);
		S32(p, 0x1C) = shl32(S16(CUR(), 0x98), 8);
		U16(p, 0x40) = U16(bone, 0x92);
		U16(p, 0x48) = U16(bone, 0x9A);
		uint32_t col = U32(bone, 0xCC) & 0x2FFFFFF;
		U32(p, 0x3C) = 0;
		U32(p, 0x38) = col;
		U8(p, 0x37) = 1;
		STREAM() += 4;
	}

	static void __cdecl op_022_ApplyActionResult()
	{
		uint32_t w = U16(RT(), 0x4A);
		uint8_t *act = PTR(CTX(), 0xCC);
		uint8_t *rec = PTR(act, 8);
		int32_t n = S8(act, 0x10);
		if ((w & 0xF000) == 0x8000)
		{
			x::ApplyActionResultToTargets((int32_t)rec, n);
			STREAM() += 2;
			return;
		}
		uint32_t slot = U8(CUR(), 0x1B);
		if (n > 0)
		{
			do
			{
				if (U8(rec, 0) == slot)
				{
					x::ApplyActionResultToTarget((int32_t)rec);
					STREAM() += 2;
					return;
				}
				rec += 0x18;
				n--;
			} while (n > 0);
		}
		STREAM() += 2;
	}

	static void __cdecl op_13D_BattleEntityVisibility()
	{
		int32_t side = 0;
		if (U8(PTR(CTX(), 0x90), 0x1B) < 3) side = -1;
		int32_t mode = (int32_t)S16(STREAM(), 2) - 1;
		switch ((uint32_t)mode)
		{
		case 0:
			if (side != 0) EntityHide(0, 3); else EntityHide(3, 4);
			break;
		case 1:
			if (side != 0) EntityRestore_B2B700(3, 4); else EntityRestore_B2B700(0, 3);
			break;
		case 2:
			if (side == 0) EntityHide(0, 3); else EntityHide(3, 4);
			break;
		case 3:
			EntityRestore_B2B700(0, 7);
			break;
		case 4:
			EntityHide(0, 7);
			break;
		default:
			if (side == 0) EntityRestore_B2B700(3, 4); else EntityRestore_B2B700(0, 3);
			break;
		}
		STREAM() += 4;
	}

	static void __cdecl op_047_CameraShakeFromOutPos()
	{
		uint8_t *b = CUR();
		CamShake()[0] = U16(b, 0x94);
		CamShake()[1] = U16(b, 0x96);
		CamShake()[2] = U16(b, 0x98);
		STREAM() += 2;
	}

	static void __cdecl op_039_CameraAndSound()
	{
		uint32_t sub = (uint32_t)U16(RT(), 0x4A) >> 12;
		switch (sub - 1)
		{
		case 0:
		{
			uint8_t *f = RESFILE()[0];
			x::PlayCameraAnimation(f + U32(f, 0x20));
			break;
		}
		case 1:
		{
			uint8_t *f = RESFILE()[0];
			uint8_t *s = STREAM();
			uint32_t slot = U8(s, 2);
			uint8_t *snd = f + U32(f, 0x24);
			int32_t n = (int32_t)U8(s, 3) + 1;
			uint32_t v = x::ClaimVoiceSlot(snd, n, 0x80);
			U32(SCENE(), slot * 4 + 0x168) = v;
			break;
		}
		case 2:
			for (int32_t off = 0x168; off < 0x174; off += 4)
			{
				uint32_t v = U32(SCENE(), off);
				if (v) x::Snd_4A2940((int32_t)v);
			}
			break;
		case 3:
		{
			uint32_t e0 = CamWord32(0), e1 = CamWord32(1), a0 = CamWord32(2);
			CamWord32(4) = e0;
			uint32_t a1 = CamWord32(3);
			CamWord32(5) = e1;
			uint16_t h = (uint16_t)PROJH();
			CamWord32(7) = a1;
			CamWord32(6) = a0;
			uint16_t w = Word1D977A2();
			Word1D977A0() = h;
			Word1D9771C() = w;
			break;
		}
		case 4:
		case 5:
			x::Camera_504270(1);
			break;
		case 6:
			x::Camera_504270(0);
			break;
		case 7:
			x::CameraArmReturn();
			break;
		default:
			if (!(U8(SEQ(), 9) & 0x20))
			{
				uint8_t *b = CUR();
				CamWord32(0) = U32(b, 0x94);
				CAMEYE()[2] = S16(b, 0x98);
				PROJH() = S16(b, 0x8C);
				Word1D977A2() = U16(b, 0x8E);
				uint8_t *t = blob::GetBone(S16(STREAM(), 2));
				CamWord32(2) = U32(t, 0x94);
				CAMAT()[2] = S16(t, 0x98);
			}
			break;
		}
		STREAM() += 4;
	}
}

	uint8_t *h_B26860(int32_t id)
	{
		uint8_t *obj = blob::ObjectPtr(id & 0xFF);
		uint8_t *b = blob::ArenaAlloc(0x80);
		uint8_t *hdr = b + 0x10;
		uint8_t *sec = b + 0x30;
		uint8_t *cmd = b + 0x20;
		U16(b, 8) = (uint16_t)id;
		PTR(hdr, 4) = sec;
		PTR(sec, 0) = obj + U32(obj, 4);
		PTR(sec, 4) = obj + U32(obj, 8);
		PTR(sec, 8) = obj + U32(obj, 0xC);
		U32(b, 0x54) = 0;
		U32(b, 0x58) = 0xD00140;
		U32(b, 0x5C) = 0x808080;
		U32(b, 0x60) = 0xFFFFFFFF;
		U16(b, 0x64) = 0;
		U32(b, 0x68) = 0;
		U32(b, 0x50) = 0xFFFFFFFF;
		U16(b, 0) = 0;
		U16(b, 2) = 0;
		U8(cmd, 1) = 0;
		U32(b, 0x78) = 0x808080;
		x::PreReadAnimation(hdr, cmd, U8(b, 9));
		return b;
	}

	void fill_vm_a(Generic &g)
	{
		g.vm[0x002] = part_vm_a::op_002_Jump;
		g.vm[0x003] = part_vm_a::op_003_Call;
		g.vm[0x004] = part_vm_a::op_004_Return;
		g.vm[0x006] = part_vm_a::op_006_LoadFile;
		g.vm[0x007] = part_vm_a::op_007_SetFreezeOthers;
		g.vm[0x008] = part_vm_a::op_008_WaitLoadDone;
		g.vm[0x01E] = part_vm_a::op_01E_SetLoopCounter;
		g.vm[0x01F] = part_vm_a::op_01F_LoopDecJump;
		g.vm[0x021] = part_vm_a::op_021_SetDataPointer;
		g.vm[0x022] = part_vm_a::op_022_ApplyActionResult;
		g.vm[0x02B] = part_vm_a::op_02B_PlaySE;
		g.vm[0x02C] = part_vm_a::op_02C_StopSoundChannels;
		g.vm[0x030] = part_vm_a::op_030_PlaySummonStream;
		g.vm[0x031] = part_vm_a::op_031_TransSummonStream;
		g.vm[0x033] = part_vm_a::op_033_SetBoneId;
		g.vm[0x034] = part_vm_a::op_034_WaitStreamReady;
		g.vm[0x039] = part_vm_a::op_039_CameraAndSound;
		g.vm[0x03B] = part_vm_a::op_03B_SequenceFlags;
		g.vm[0x03F] = part_vm_a::op_03F_CreateEmbeddedModel;
		g.vm[0x041] = part_vm_a::op_041_EmbeddedModelFlags;
		g.vm[0x043] = part_vm_a::op_043_SetDrawHandlerInlineData;
		g.vm[0x044] = part_vm_a::op_044_SetDrawHandlerInlineDataReset;
		g.vm[0x046] = part_vm_a::op_046_JumpIfTargetResult;
		g.vm[0x047] = part_vm_a::op_047_CameraShakeFromOutPos;
		g.vm[0x04D] = part_vm_a::op_04D_SetupScreenTint;
		g.vm[0x04E] = part_vm_a::op_04E_SetDescriptorInlineData;
		g.vm[0x053] = part_vm_a::op_053_MusicVolume;
		g.vm[0x055] = part_vm_a::op_055_JumpIfSummonState;
		g.vm[0x057] = part_vm_a::op_057_Nop2;
		g.vm[0x058] = part_vm_a::op_058_CreateParticlePool;
		g.vm[0x059] = part_vm_a::op_059_EmitParticle;
		g.vm[0x064] = part_vm_a::op_064_SetBoneSlot;
		g.vm[0x07B] = part_vm_a::op_07B_WaitThenReloadA8defTim;
		g.vm[0x089] = part_vm_a::op_089_StepEmbeddedModelAnim;
		g.vm[0x0A1] = part_vm_a::op_0A1_RandomJump;
		g.vm[0x0A5] = part_vm_a::op_0A5_Nop4;
		g.vm[0x0A6] = part_vm_a::op_0A6_Nop4;
		g.vm[0x0A7] = part_vm_a::op_0A7_SetCounter;
		g.vm[0x0A8] = part_vm_a::op_0A8_DecCounterJumpNZ;
		g.vm[0x0B1] = part_vm_a::op_0B1_SetSeqStateByte6;
		g.vm[0x0B2] = part_vm_a::op_0B2_SetCtxByteA2;
		g.vm[0x0B8] = part_vm_a::op_0B8_Nop8;
		g.vm[0x0C3] = part_vm_a::op_0C3_PauseResumeSounds;
		g.vm[0x0CD] = part_vm_a::op_0CD_SetRenderListOffset;
		g.vm[0x0CE] = part_vm_a::op_0CE_SetBoneIdFromSlot;
		g.vm[0x120] = part_vm_a::op_120_SetArenaTop;
		g.vm[0x13D] = part_vm_a::op_13D_BattleEntityVisibility;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace part_vm_b
{
	static inline int32_t shl_cl(int32_t v, int32_t c) { return (int32_t)((uint32_t)v << (c & 31)); }
	static inline int32_t sar_cl(int32_t v, int32_t c) { return v >> (c & 31); }
	static inline int16_t add16(int16_t a, int32_t b) { return (int16_t)(uint16_t)((uint16_t)a + (uint16_t)b); }
	static inline int32_t neg32(int32_t v) { return (int32_t)(0u - (uint32_t)v); }

	static inline uint8_t *Entity() { return PTR(SCENE(), 0x60 + 4 * (int32_t)U8(CUR(), 0x1B)); }

	static inline void CallBoneHandler() { (&C().bone[0])[U8(CUR(), 0x18)](); }

	static inline void Yield() { U16(RT(), 0x3E) = U16(CUR(), 0xC8); }

	static void __cdecl op_02D_SetCameraFlag()
	{
		uint8_t *s = STREAM();
		MEM<uint8_t>(game_addr(game::unk_1D97705)) |= 0x80;
		STREAM() = s + 2;
	}

	static void __cdecl op_045_EntityFlagBits()
	{
		int32_t slot = U8(CUR(), 0x1B);
		uint8_t *scene = SCENE();
		uint8_t *e = PTR(scene, 0x60 + 4 * slot);
		int32_t sw = S16(scene, 0x4E + 2 * slot);
		int32_t w = S16(STREAM(), 2);
		uint32_t sub = (uint32_t)U16(RT(), 0x4A) >> 12;
		if (sub == 0)
		{
			U16(e, 0) |= (uint16_t)w;
		}
		else if (sub == 1)
		{
			if ((w & sw) == 0)
			{
				U16(e, 0) &= (uint16_t)~w;
			}
		}
		else if (sub == 8)
		{
			U16(e, 0) &= (uint16_t)~w;
		}
		STREAM() += 4;
	}

	static void __cdecl op_049_JumpIfEntitySize()
	{
		uint8_t *e = Entity();
		uint8_t *s = STREAM();
		uint32_t op = U16(RT(), 0x4A);
		int32_t w = S16(s, 2);
		int32_t v = S16(e, 0x26);
		bool jump;
		if (op & 0xFFFFFE00) jump = v < w;
		else jump = !(v <= w);
		if (jump) STREAM() = s + S16(s, 4);
		else STREAM() = s + 6;
	}

	static void __cdecl op_06D_PartyRecordFlag()
	{
		bool clr = (U8(RT(), 0x4B) & 0x80) != 0;
		uint8_t *p = (uint8_t *)game_addr(game::unk_1D98991);
		for (int i = 0; i < 4; i++, p += 0x2C)
		{
			if (clr) *p &= 0xFD;
			else *p |= 2;
		}
		STREAM() += 2;
	}

	static void __cdecl op_01C_PokeBoneField()
	{
		uint8_t *s = STREAM();
		uint8_t *bone = CUR();
		int32_t off = S16(s, 2);
		uint32_t k = (uint32_t)U16(RT(), 0x4A) >> 9;
		if (k == 1)
		{
			U16(bone, off) = U16(s, 4);
		}
		else
		{
			U8(bone, off) = U8(s, 4);
		}
		STREAM() += 6;
	}

	static void __cdecl op_01B_Nop4() { STREAM() += 4; }

	static void __cdecl op_077_Nop6() { STREAM() += 6; }

	static void __cdecl op_079_CompareFieldJump()
	{
		uint32_t op = U16(RT(), 0x4A);
		uint8_t *s = STREAM();
		int32_t off = ((int32_t)op >> 10) & 0x1C;
		int32_t v = S32(CUR(), off + 0x50) >> 16;
		int32_t w = S16(s, 2);
		bool jump;
		if (op & 0x8000) jump = v <= w;
		else jump = !(v < w);
		if (jump) STREAM() = s + S16(s, 4);
		else STREAM() = s + 6;
	}

	static void __cdecl op_07D_SetBoneCount()
	{
		int32_t n = S16(STREAM(), 2);
		uint32_t cur = U16(SCENE(), 0x38);
		int32_t d = sub32(n, (int32_t)cur);
		if (d > 0)
		{
			uint8_t *b = PTR(CTX(), 0x90) + (cur << 8);
			do
			{
				U32(b, 0) = 0;
				b += 0x100;
			} while (--d != 0);
			S16(SCENE(), 0x38) = (int16_t)n;
		}
		STREAM() += 4;
	}

	static void __cdecl op_08A_BattleScreenFx()
	{
		uint32_t sub = (uint32_t)U16(RT(), 0x4A) >> 12;
		if (sub == 1)
		{
			x::Battle_501F30(S16(STREAM(), 2));
			STREAM() += 4;
		}
		else if (sub == 2)
		{
			x::Battle_501E40(S16(STREAM(), 2));
			STREAM() += 4;
		}
		else if (sub == 3)
		{
			x::Battle_4A8480(0);
			STREAM() += 2;
		}
		else
		{
			uint8_t *p = PTR(CTX(), 0xC0);
			U8(p, 0xB) = (uint8_t)(U8(STREAM(), 2) + 1);
			U8(p, 9) = U8(STREAM(), 4);
			STREAM() += 6;
		}
	}

	static void __cdecl op_08D_ModelFlagsSetClear()
	{
		uint8_t *e = Entity();
		int32_t w = S16(STREAM(), 2);
		if (U8(RT(), 0x4B) & 0x80) U32(e, 0x7C) &= ~(uint32_t)w;
		else U32(e, 0x7C) |= (uint32_t)w;
		STREAM() += 4;
	}

	static void __cdecl op_005_FlagOp()
	{
		uint8_t *s = STREAM();
		uint8_t *rt = RT();
		int32_t m = S16(s, 2);
		uint8_t *fw = PTR(rt, 0x10);
		uint32_t sub = (uint32_t)U16(rt, 0x4A) >> 12;
		int32_t f = S16(fw, 2);
		if (sub == 0)
		{
			U16(fw, 2) = (uint16_t)(m | f);
			STREAM() = s + 4;
		}
		else if (sub == 1)
		{
			U16(fw, 2) = (uint16_t)(~m & f);
			STREAM() = s + 4;
		}
		else if (sub == 2)
		{
			if ((m & f) == 0) STREAM() = s + 6;
			else STREAM() = s + S16(s, 4);
		}
		else if (sub == 3)
		{
			if ((m & f) != 0) STREAM() = s + 6;
			else STREAM() = s + S16(s, 4);
		}
		else if (sub == 4)
		{
			if ((m & f) == 0) STREAM() = s + 4;
			else
			{
				U16(rt, 0x3E) = U16(CUR(), 0xC8);
				STREAM() = s;
			}
		}
		else if (sub == 5)
		{
			if ((m & f) != 0) STREAM() = s + 4;
			else
			{
				U16(rt, 0x3E) = U16(CUR(), 0xC8);
				STREAM() = s;
			}
		}
		else
		{
			U16(fw, 2) = (uint16_t)(m | f);
			STREAM() = s + 4;
		}
	}

	static void __cdecl op_10C_BoneFlagOps()
	{
		uint8_t *rt = RT();
		uint8_t *bone = CUR();
		uint8_t *s = STREAM();
		uint32_t fl = U16(bone, 0x4A);
		int32_t m = S16(s, 2);
		uint32_t sub = (uint32_t)U16(rt, 0x4A) >> 12;
		switch (sub)
		{
		case 1:
			U16(bone, 0x4A) = (uint16_t)(~(uint32_t)m & fl);
			STREAM() += 4;
			return;
		case 2:
			if ((fl & (uint32_t)m) == 0) STREAM() = s + 6;
			else STREAM() = s + S16(s, 4);
			return;
		case 3:
			if ((fl & (uint32_t)m) != 0) STREAM() = s + 6;
			else STREAM() = s + S16(s, 4);
			return;
		case 4:
			if ((fl & (uint32_t)m) == 0) { STREAM() += 4; return; }
			U16(rt, 0x3E) = U16(bone, 0xC8);
			return;
		case 5:
			if ((fl & (uint32_t)m) != 0) { STREAM() += 4; return; }
			U16(rt, 0x3E) = U16(bone, 0xC8);
			return;
		default:
			U16(bone, 0x4A) = (uint16_t)((uint32_t)m | fl);
			STREAM() += 4;
			return;
		}
	}

	static void __cdecl op_10D_WaitOtherBoneFlags()
	{
		uint8_t *o = blob::GetBone(S16(STREAM(), 2));
		uint32_t fl = U16(o, 0x4A);
		uint8_t *rt = RT();
		uint32_t m = ((uint32_t)U16(STREAM(), 4) & fl) & 0x3FFF;
		bool wait;
		if (U8(rt, 0x4B) & 0x10) wait = m != 0;
		else wait = m == 0;
		if (wait) U16(rt, 0x3E) = U16(CUR(), 0xC8);
		else STREAM() += 6;
	}

	static void __cdecl op_0D7_Nop4() { STREAM() += 4; }

	static void __cdecl op_0E7_WaitUntilFieldGreater()
	{
		uint8_t *s = STREAM();
		uint8_t *bone = CUR();
		int32_t v = S16(bone, S16(s, 2) + 0x8C);
		if (v > (int32_t)S16(s, 4)) STREAM() = s + 6;
		else U16(RT(), 0x3E) = U16(bone, 0xC8);
	}

	static void __cdecl op_0FB_WaitUntilFieldLess()
	{
		uint8_t *s = STREAM();
		uint8_t *bone = CUR();
		int32_t v = S16(bone, S16(s, 2) + 0x8C);
		if (v < (int32_t)S16(s, 4)) STREAM() = s + 6;
		else U16(RT(), 0x3E) = U16(bone, 0xC8);
	}

	static void __cdecl op_106_WaitUntilFieldLessThanBone()
	{
		int32_t off = S16(STREAM(), 2);
		uint8_t *o = blob::GetBone(S16(STREAM(), 4));
		uint8_t *bone = CUR();
		if (S16(bone, off + 0x8C) < S16(o, off + 0x8C)) STREAM() += 6;
		else U16(RT(), 0x3E) = U16(bone, 0xC8);
	}

	static void __cdecl op_0FD_JumpIfFieldInRange()
	{
		uint8_t *s = STREAM();
		int32_t v = S16(CUR(), S16(s, 2) + 0x8C);
		if (v < (int32_t)S16(s, 4) || v > (int32_t)S16(s, 6)) STREAM() = s + 10;
		else STREAM() = s + S16(s, 8);
	}

	static void __cdecl op_074_DrawFlagBits()
	{
		bool clr = (U8(RT(), 0x4B) & 0x80) != 0;
		int32_t w = S16(STREAM(), 2);
		if (clr)
		{
			w = ~w;
			U16(CUR(), 0x4C) &= (uint16_t)w;
		}
		U16(CUR(), 0x4C) |= (uint16_t)w;
		STREAM() += 4;
	}

	static void __cdecl op_0C1_Nop() { STREAM() += 2; }

	static void __cdecl op_04C_SetBoneHandler()
	{
		U8(CUR(), 0x18) = U8(STREAM(), 2);
		CallBoneHandler();
		STREAM() += 4;
	}

	static void __cdecl op_037_ResetAccum()
	{
		uint8_t *bone = CUR();
		for (int i = 0; i < 6; i++) U32(bone, 0x50 + 4 * i) = 0;
		CallBoneHandler();
		h_B2C440();
		STREAM() += 2;
	}

	static void __cdecl op_00A_GenericWrite()
	{
		uint32_t mask = U16(RT(), 0x4A);
		uint32_t a = U8((const void *)C().attr_16A, (int32_t)(mask & 0x1FF));
		U32(WS(), 0xE4) = a;
		int32_t idx = (int32_t)(a & 0xF);
		int32_t off = U8((const void *)C().attr_184, idx * 4);
		int32_t width = U8((const void *)C().attr_186, idx * 4);
		uint8_t *field = off + CUR();
		int32_t shift = S8((const void *)C().attr_184, idx * 4 + 1);
		U32(WS(), 0xE0) = U8((const void *)C().attr_184, idx * 4 + 3);
		uint8_t *arg = STREAM() + 2;
		int32_t count = width * 6;
		int32_t mode = (int32_t)U32(WS(), 0xE4) >> 4;
		if (mode == 1)
		{
			int32_t v = S16(arg, 0);
			if (width == 2) v = sar_cl(v, shift);
			else v = shl_cl(v, shift);
			for (int32_t o = 0; o < count; o += width)
			{
				if ((int16_t)mask < 0)
				{
					if (width != 2) S32(field, o) = v;
					else S16(field, o) = (int16_t)v;
				}
				mask <<= 1;
			}
			arg += 2;
		}
		else if (mode == 2)
		{
			for (int32_t o = 0; o < count; o += width)
			{
				if ((int16_t)mask < 0)
				{
					int32_t v = S16(arg, 0);
					if (v != 0x7654)
					{
						if (width != 2) S32(field, o) = add32(S32(field, o), shl_cl(v, shift));
						else S16(field, o) = add16(S16(field, o), shl_cl(v, shift));
					}
					arg += 2;
				}
				mask <<= 1;
			}
		}
		else if (mode == 3)
		{
			for (int32_t o = 0; o < count; o += width)
			{
				if ((int16_t)mask < 0)
				{
					int32_t v = S16(arg, 0);
					if (v != 0)
					{
						int32_t r = blob::Rand73(v);
						if (width != 2) S32(field, o) = add32(S32(field, o), shl_cl(r, shift));
						else S16(field, o) = add16(S16(field, o), shl_cl(r, shift));
					}
					arg += 2;
				}
				mask <<= 1;
			}
		}
		else if (mode == 4)
		{
			int32_t r = blob::Rand73(S16(arg, 0));
			if (width == 2) r = sar_cl(r, shift);
			else r = shl_cl(r, shift);
			for (int32_t o = 0; o < count; o += width)
			{
				if ((int16_t)mask < 0)
				{
					if (width != 2) S32(field, o) = add32(S32(field, o), r);
					else S16(field, o) = add16(S16(field, o), r);
				}
				mask <<= 1;
			}
			arg += 2;
		}
		else if (mode == 5)
		{
			for (int32_t o = 0; o < count; o += width)
			{
				if ((int16_t)mask < 0)
				{
					int32_t base = S16(arg, 0);
					blob::Rand73(base);
					int32_t range = S16(arg, 2);
					if (range < 0) base = neg32(base);
					int32_t r = blob::Rand73(range);
					base = add32(base, r);
					if (width != 2) S32(field, o) = add32(S32(field, o), shl_cl(base, shift));
					else S16(field, o) = add16(S16(field, o), shl_cl(base, shift));
					arg += 4;
				}
				mask <<= 1;
			}
		}
		else
		{
			for (int32_t o = 0; o < count; o += width)
			{
				if ((int16_t)mask < 0)
				{
					int32_t v = S16(arg, 0);
					if (v != 0x7654)
					{
						if (width != 2) S32(field, o) = shl_cl(v, shift);
						else S16(field, o) = (int16_t)sar_cl(v, shift);
					}
					arg += 2;
				}
				mask <<= 1;
			}
		}
		STREAM() = arg;
		uint32_t post = U32(WS(), 0xE0);
		if (post == 1)
		{
			CallBoneHandler();
			h_B2C440();
		}
		else if (post == 2)
		{
			h_B2D460();
		}
	}

	static void __cdecl op_05F_PosFromEntityOffset()
	{
		uint8_t *bone = CUR();
		uint8_t *p = Entity() + 0x1C;
		int32_t w = S16(STREAM(), 2);
		if (w != 0x7654)
		{
			S32(bone, 0x5C) = shl32(add32(S16(p, 0), w), 16);
			bone = CUR();
		}
		w = S16(STREAM(), 4);
		if (w != 0x7654)
		{
			S32(bone, 0x60) = shl32(add32(S16(p, 2), w), 16);
			bone = CUR();
		}
		w = S16(STREAM(), 6);
		if (w != 0x7654)
		{
			S32(bone, 0x64) = shl32(add32(S16(p, 4), w), 16);
			bone = CUR();
		}
		U8(bone, 0x18) = 1;
		C().bone[1]();
		STREAM() += 8;
	}

	static void __cdecl op_072_RotFromEntity()
	{
		uint8_t *bone = CUR();
		uint8_t *p = Entity() + 0xC;
		S32(bone, 0x50) = shl32(S16(p, 0), 16);
		S32(CUR(), 0x54) = shl32(S16(p, 2), 16);
		S32(CUR(), 0x58) = shl32(S16(p, 4), 16);
		h_B2C440();
		STREAM() += 2;
	}

	static void __cdecl op_05E_PosFromSlotHome()
	{
		uint8_t *bone = CUR();
		uint8_t *p = SCENE() + 8 * (int32_t)U8(bone, 0x1B) + 0x84;
		S32(bone, 0x5C) = shl32(S16(p, 0), 16);
		S32(CUR(), 0x60) = shl32(S16(p, 2), 16);
		S32(CUR(), 0x64) = shl32(S16(p, 4), 16);
		CallBoneHandler();
		h_B2C440();
		STREAM() += 2;
	}

	static void __cdecl op_02F_StopMotion()
	{
		uint8_t *bone = CUR();
		for (int i = 0; i < 9; i++) U32(bone, 0x68 + 4 * i) = 0;
		h_B2D460();
		STREAM() += 2;
	}

	static void __cdecl op_082_VelocityTowardBone()
	{
		uint8_t *o = blob::GetBone(S16(STREAM(), 2));
		int32_t n = S16(STREAM(), 4);
		uint8_t *b = CUR();
		S32(b, 0x74) = sub32(S32(o, 0x5C), S32(b, 0x5C)) / n;
		b = CUR();
		S32(b, 0x78) = sub32(S32(o, 0x60), S32(b, 0x60)) / n;
		int32_t t = S32(o, 0x64);
		b = CUR();
		S32(b, 0x7C) = sub32(t, S32(b, 0x64)) / n;
		STREAM() += 6;
	}

	static void __cdecl op_0C0_VelPosTowardSpawner()
	{
		uint8_t *o = blob::GetBone((int32_t)U16(CUR(), 0x14));
		int32_t n = S16(STREAM(), 2);
		uint8_t *b = CUR();
		S32(b, 0x74) = sub32(S32(o, 0x5C), S32(b, 0x5C)) / n;
		b = CUR();
		S32(b, 0x78) = sub32(S32(o, 0x60), S32(b, 0x60)) / n;
		int32_t t = S32(o, 0x64);
		b = CUR();
		S32(b, 0x7C) = sub32(t, S32(b, 0x64)) / n;
		STREAM() += 4;
	}

	static void __cdecl op_048_OverrideEntityTransform()
	{
		uint8_t *e = Entity();
		U8(e, 1) |= 0x10;
		uint32_t k = ((uint32_t)U16(RT(), 0x4A) >> 9) & 0xF;
		if (k == 0)
		{
			uint8_t *p = CUR() + 0x94;
			U32(e, 0x1C) = U32(p, 0);
			U16(e, 0x20) = U16(p, 4);
			if (U8(RT(), 0x4B) & 0x80)
			{
				U16(e, 0x24) = U16(p, 2);
			}
		}
		else if (k == 1)
		{
			uint8_t *p = CUR() + 0x8C;
			U32(e, 0xC) = U32(p, 0);
			U16(e, 0x10) = U16(p, 4);
		}
		STREAM() += 2;
	}

	static void __cdecl op_0B9_NegateField()
	{
		int32_t i = S16(STREAM(), 2);
		uint8_t *bone = CUR();
		if (i > 0)
		{
			S32(bone, 0x50 + 4 * i) = neg32(S32(bone, 0x50 + 4 * i));
			if (i < 6)
			{
				if (i >= 3)
				{
					CallBoneHandler();
					STREAM() += 4;
					return;
				}
				h_B2C440();
			}
		}
		STREAM() += 4;
	}

	static void __cdecl op_0BA_NegateFieldNeg()
	{
		int32_t i = S16(STREAM(), 2);
		uint8_t *bone = CUR();
		if (i < 0)
		{
			S32(bone, 0x50 + 4 * i) = neg32(S32(bone, 0x50 + 4 * i));
			if (i < 6)
			{
				if (i >= 3)
				{
					CallBoneHandler();
					STREAM() += 4;
					return;
				}
				h_B2C440();
			}
		}
		STREAM() += 4;
	}

	static void __cdecl op_0D2_NegateMotionComponent()
	{
		int32_t i = S16(STREAM(), 2);
		uint8_t *bone = CUR();
		S32(bone, 0x50 + 4 * i) = neg32(S32(bone, 0x50 + 4 * i));
		CallBoneHandler();
		h_B2C440();
		STREAM() += 4;
	}

	static void __cdecl op_0CC_SetBoneHandler7()
	{
		U16(CUR(), 0xB0) = U16(STREAM(), 2);
		U8(CUR(), 0x18) = 7;
		CallBoneHandler();
		STREAM() += 4;
	}

	static void __cdecl op_0F2_BakeOutPosToAccum()
	{
		uint8_t *b = CUR();
		S32(b, 0x5C) = shl32(S16(b, 0x94), 16);
		b = CUR();
		S32(b, 0x60) = shl32(S16(b, 0x96), 16);
		b = CUR();
		S32(b, 0x64) = shl32(S16(b, 0x98), 16);
		U8(CUR(), 0x18) = 0;
		STREAM() += 2;
	}

	static void __cdecl op_110_PlaceAtMidpoint()
	{
		uint8_t *a = blob::GetBone(S16(STREAM(), 2));
		uint8_t *b = blob::GetBone(S16(STREAM(), 4));
		U8(CUR(), 0x18) = U8(a, 0x18);
		int32_t ax = S32(a, 0x5C);
		int32_t bx = S32(b, 0x5C);
		int32_t ay = S32(a, 0x60);
		int32_t az = S32(a, 0x64);
		S32(CUR(), 0x5C) = add32(sub32(bx, ax) / 2, ax);
		int32_t by = S32(b, 0x60);
		S32(CUR(), 0x60) = add32(sub32(by, ay) / 2, ay);
		int32_t bz = S32(b, 0x64);
		S32(CUR(), 0x64) = add32(sub32(bz, az) / 2, az);
		STREAM() += 6;
		CallBoneHandler();
	}

	static void __cdecl op_061_CopyAccumOrVelFromBone()
	{
		uint8_t *src = blob::GetBone(S16(STREAM(), 4));
		uint8_t *dst;
		if (U16(RT(), 0x4A) == 0x61)
		{
			dst = CUR() + 0x50;
			src += 0x50;
		}
		else
		{
			src += 0x68;
			dst = CUR() + 0x68;
		}
		int32_t m = S16(STREAM(), 2);
		int32_t bit = 1;
		for (int i = 0; i < 6; i++)
		{
			if (m & bit) U32(dst, 0) = U32(src, 0);
			bit <<= 1;
			dst += 4;
			src += 4;
		}
		h_B2C440();
		CallBoneHandler();
		STREAM() += 6;
	}

	static void __cdecl op_091_AccumFromBoneOutputs()
	{
		uint8_t *src = blob::GetBone(S16(STREAM(), 4)) + 0x8C;
		uint8_t *dst = CUR() + 0x50;
		int32_t m = S16(STREAM(), 2);
		int32_t bit = 1;
		for (int outer = 2; outer != 0; outer--)
		{
			for (int i = 0; i < 3; i++)
			{
				if (m & bit) S32(dst, 0) = shl32(S16(src, 0), 16);
				bit <<= 1;
				dst += 4;
				src += 2;
			}
			src += 2;
		}
		h_B2C440();
		CallBoneHandler();
		STREAM() += 6;
	}

	static void __cdecl op_060_PosFromEntityJoint()
	{
		uint8_t *e = Entity();
		int32_t joint = S16(STREAM(), 2);
		blob::Blob_B66B80(e + 0x60, joint, 0x1000, e + 0x40);
		if (U8(RT(), 0x4B) & 0x80)
		{
			uint8_t *s = STREAM();
			int32_t sz = S16(e, 0x26);
			int32_t ang = shl32(S16(s, 6), 4);
			int32_t mul = S16(s, 4);
			int32_t t = x::Cos(ang);
			t = mul32(t, sz) >> 4;
			t = mul32(t, mul) / 65536;
			S32(CUR(), 0x64) = shl32(add32(S32(WS(), 0xF8), t), 16);
			t = x::Sin(ang);
			t = mul32(t, sz) >> 4;
			t = mul32(t, mul) / 65536;
			S32(CUR(), 0x5C) = shl32(add32(S32(WS(), 0xF0), t), 16);
			S32(CUR(), 0x60) = shl32(S32(WS(), 0xF4), 16);
			STREAM() += 8;
			CallBoneHandler();
			return;
		}
		S32(CUR(), 0x5C) = shl32(S32(WS(), 0xF0), 16);
		S32(CUR(), 0x60) = shl32(S32(WS(), 0xF4), 16);
		S32(CUR(), 0x64) = shl32(S32(WS(), 0xF8), 16);
		STREAM() += 4;
		CallBoneHandler();
	}

	static void __cdecl op_06E_PosFromTargetsJointAvg()
	{
		int32_t joint = S16(STREAM(), 2);
		uint8_t *scene = SCENE();
		int32_t sy = 0, sx = 0, sz = 0;
		int32_t i = 0;
		if (S8(scene, 0x41) > 0)
		{
			do
			{
				uint8_t *e = PTR(scene, 0x60 + 4 * (int32_t)S8(scene, 0x48 + i));
				blob::Blob_B66B80(e + 0x60, joint, 0x1000, e + 0x40);
				uint8_t *ws = WS();
				int32_t fx = S32(ws, 0xF0), fy = S32(ws, 0xF4), fz = S32(ws, 0xF8);
				sx = add32(sx, fx);
				sy = add32(sy, fy);
				sz = add32(sz, fz);
				scene = SCENE();
				i++;
			} while (i < (int32_t)S8(scene, 0x41));
		}
		int32_t n = S8(scene, 0x41);
		uint32_t k = (uint32_t)U16(RT(), 0x4A) >> 9;
		if (k & 8) S32(CUR(), 0x5C) = shl32(sx / n, 16);
		if (k & 0x10) S32(CUR(), 0x60) = shl32(sy / n, 16);
		if (k & 0x20) S32(CUR(), 0x64) = shl32(sz / n, 16);
		CallBoneHandler();
		STREAM() += 4;
	}

	static void __cdecl op_01A_SetBoneHandlerParams()
	{
		int32_t w = S16(STREAM(), 2);
		int32_t n = w & 0xF;
		U8(CUR(), 0x18) = (uint8_t)(w >> 8);
		PTR(CUR(), 0xA0) = STREAM() + 4;
		STREAM() = STREAM() + n * 2 + 4;
	}

	static void __cdecl op_075_AccumFromEntitySize()
	{
		int32_t ops = S16(RT(), 0x4A);
		uint8_t *e = PTR(SCENE(), 0x60 + 4 * (int32_t)U8(CUR(), 0x1B));
		int32_t size = S16(e, 0x26);
		if (ops & 0x200)
		{
			blob::Blob_B66B80(e + 0x60, 0, 0x1000, e + 0x40);
			int32_t f4 = S32(WS(), 0xF4);
			int32_t a = sub32(f4, S16(e, 0x36));
			int32_t b = sub32(S16(e, 0x3C), f4);
			if (a < b) a = b;
			if (size < a) size = a;
		}
		int32_t v = shl32(mul32(S16(STREAM(), 2), size), 8);
		uint8_t *acc = CUR() + 0x50;
		uint32_t m = (uint32_t)ops;
		for (int32_t o = 0; o < 0x18; o += 4)
		{
			if (m & 0x8000) S32(acc, o) = v;
			m <<= 1;
		}
		CallBoneHandler();
		h_B2C440();
		STREAM() += 4;
	}

	static void __cdecl op_076_AccumAddRandom()
	{
		uint32_t m = (uint32_t)(int32_t)S16(RT(), 0x4A);
		uint8_t *src = blob::GetBone(S16(STREAM(), 2));
		uint8_t *acc = CUR() + 0x50;
		for (int32_t i = 0; i < 6; i++)
		{
			if (m & 0x8000)
			{
				int32_t so = U8((const void *)C().attr_19C, i);
				int32_t r = blob::Rand73(S16(src, so + 0x8C));
				int32_t d = U8((const void *)C().attr_194, i);
				S32(acc, d) = add32(S32(acc, d), shl32(r, 16));
			}
			m <<= 1;
		}
		CallBoneHandler();
		h_B2C440();
		STREAM() += 4;
	}

	static void __cdecl op_07C_RandomOffsetXZ()
	{
		int32_t a = blob::Rand73(0x1000);
		int32_t r = blob::Rand73(S16(STREAM(), 4));
		int32_t rad = add32(r, S16(STREAM(), 2));
		uint8_t *p = CUR() + 0x64;
		int32_t t = shl32(mul32(x::Cos(a), rad), 4);
		S32(p, 0) = add32(S32(p, 0), t);
		r = blob::Rand73(S16(STREAM(), 8));
		rad = add32(r, S16(STREAM(), 6));
		p = CUR() + 0x5C;
		t = shl32(mul32(x::Sin(a), rad), 4);
		S32(p, 0) = add32(S32(p, 0), t);
		STREAM() += 10;
	}

	static void __cdecl op_087_VelocityScaleAccum()
	{
		uint32_t m = (uint32_t)U16(RT(), 0x4A) << 16;
		uint8_t *vel = CUR() + 0x68;
		int32_t sc = S16(STREAM(), 2);
		int32_t fr = S16(STREAM(), 4);
		for (int i = 6; i != 0; i--)
		{
			if ((int32_t)m < 0)
			{
				int32_t a = S32(vel, -0x18) >> 16;
				int32_t v = shl32(mul32(add32(sc, -0x100), a), 8);
				S32(vel, 0) = v / fr;
			}
			vel += 4;
			m <<= 1;
		}
		STREAM() += 6;
	}
}

	void h_B2C440()
	{
		S16(CUR(), 0x8C) = (int16_t)(S32(CUR(), 0x50) >> 16);
		S16(CUR(), 0x8E) = (int16_t)(S32(CUR(), 0x54) >> 16);
		S16(CUR(), 0x90) = (int16_t)(S32(CUR(), 0x58) >> 16);
	}

	int32_t h_B2D460()
	{
		uint8_t *b = CUR();
		uint8_t f = 0;
		if (U16(b, 0x80) != 0) f = 1;
		if (U16(b, 0x82) != 0) f |= 1;
		if (U16(b, 0x84) != 0) f |= 1;
		if (U16(b, 0x86) != 0) f |= 8;
		if (U16(b, 0x88) != 0) f |= 8;
		if (U16(b, 0x8A) != 0) f |= 8;
		U8(b, 0x1A) = f;
		return 0;
	}

	void fill_vm_b(Generic &g)
	{
		using namespace part_vm_b;
		g.vm[0x02D] = op_02D_SetCameraFlag;
		g.vm[0x045] = op_045_EntityFlagBits;
		g.vm[0x049] = op_049_JumpIfEntitySize;
		g.vm[0x06D] = op_06D_PartyRecordFlag;
		g.vm[0x01C] = op_01C_PokeBoneField;
		g.vm[0x01B] = op_01B_Nop4;
		g.vm[0x077] = op_077_Nop6;
		g.vm[0x079] = op_079_CompareFieldJump;
		g.vm[0x07D] = op_07D_SetBoneCount;
		g.vm[0x08A] = op_08A_BattleScreenFx;
		g.vm[0x08D] = op_08D_ModelFlagsSetClear;
		g.vm[0x005] = op_005_FlagOp;
		g.vm[0x10C] = op_10C_BoneFlagOps;
		g.vm[0x10D] = op_10D_WaitOtherBoneFlags;
		g.vm[0x0D7] = op_0D7_Nop4;
		g.vm[0x0E7] = op_0E7_WaitUntilFieldGreater;
		g.vm[0x0FB] = op_0FB_WaitUntilFieldLess;
		g.vm[0x106] = op_106_WaitUntilFieldLessThanBone;
		g.vm[0x0FD] = op_0FD_JumpIfFieldInRange;
		g.vm[0x074] = op_074_DrawFlagBits;
		g.vm[0x0C1] = op_0C1_Nop;
		g.vm[0x04C] = op_04C_SetBoneHandler;
		g.vm[0x037] = op_037_ResetAccum;
		for (int i = 0x0A; i <= 0x17; i++)
			g.vm[i] = op_00A_GenericWrite;
		g.vm[0x05F] = op_05F_PosFromEntityOffset;
		g.vm[0x072] = op_072_RotFromEntity;
		g.vm[0x05E] = op_05E_PosFromSlotHome;
		g.vm[0x02F] = op_02F_StopMotion;
		g.vm[0x082] = op_082_VelocityTowardBone;
		g.vm[0x0C0] = op_0C0_VelPosTowardSpawner;
		g.vm[0x048] = op_048_OverrideEntityTransform;
		g.vm[0x0B9] = op_0B9_NegateField;
		g.vm[0x0BA] = op_0BA_NegateFieldNeg;
		g.vm[0x0D2] = op_0D2_NegateMotionComponent;
		g.vm[0x0CC] = op_0CC_SetBoneHandler7;
		g.vm[0x0F2] = op_0F2_BakeOutPosToAccum;
		g.vm[0x110] = op_110_PlaceAtMidpoint;
		g.vm[0x061] = op_061_CopyAccumOrVelFromBone;
		g.vm[0x062] = op_061_CopyAccumOrVelFromBone;
		g.vm[0x091] = op_091_AccumFromBoneOutputs;
		g.vm[0x060] = op_060_PosFromEntityJoint;
		g.vm[0x06E] = op_06E_PosFromTargetsJointAvg;
		g.vm[0x01A] = op_01A_SetBoneHandlerParams;
		g.vm[0x075] = op_075_AccumFromEntitySize;
		g.vm[0x076] = op_076_AccumAddRandom;
		g.vm[0x07C] = op_07C_RandomOffsetXZ;
		g.vm[0x087] = op_087_VelocityScaleAccum;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace part_vm_c
{
	inline uint8_t &LOADBUSY() { return MEM<uint8_t>(game_addr(game::g_GfCinematic_FileLoadBusy)); }
	static constexpr game::Id CLUTBUF = game::unk_279822C;
	inline uint32_t &MODELPALETTE() { return MEM<uint32_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE)); }
	static constexpr game::Id FADELAYERS = game::BATTLE_STAGE_RENDER_GROUPS;

	static uint32_t xl_Render_5088A0(void *a, uint32_t ot, int32_t mode, uint32_t cursor)
	{
		return x::Render_5088A0(a, ot, mode, cursor);
	}

	static int32_t ResetChannel_B2D8A0()
	{
		int32_t ch = S32(WS(), 0xF0);
		U16(CUR(), ch * 2 + 0xC) = 0;
		U8(CUR(), ch + 0x44) = 0;
		return 0;
	}

	static void SpawnBone_B2D610(uint32_t caller_ecx)
	{
		uint32_t n = U16(SCENE(), 0x38);
		if (MEM<uint8_t>(game_addr(game::unk_2797453) + n) != 0xFF)
		{
			U32(WS(), 0xFC) = caller_ecx;
			STREAM() += 4;
			return;
		}
		uint32_t pos = 0;
		if (ORDER()[0] != 0xFF)
		{
			uint8_t v;
			do
			{
				v = ORDER()[pos + 1];
				pos++;
			} while (v != 0xFF);
		}
		uint8_t *bone = PTR(CTX(), 0x90);
		int32_t id = 0;
		if (U32(bone, 0) != 0)
		{
			uint32_t next;
			do
			{
				next = U32(bone, 0x100);
				bone += 0x100;
				id++;
			} while (next != 0);
		}
		ORDER()[pos] = (uint8_t)id;
		uint8_t *st = STREAM();
		PTR(bone, 0) = st + S16(st, 2);
		U16(SCENE(), 0x18) = (uint16_t)(U16(SCENE(), 0x18) + 1);
		blob::InitBone(id);
		U32(bone, 0x5C) = U32(CUR(), 0x5C);
		U32(bone, 0x60) = U32(CUR(), 0x60);
		U32(bone, 0x64) = U32(CUR(), 0x64);
		U32(bone, 0x50) = U32(CUR(), 0x50);
		U32(bone, 0x54) = U32(CUR(), 0x54);
		U32(bone, 0x58) = U32(CUR(), 0x58);
		U8(bone, 0x18) = U8(CUR(), 0x18);
		U16(bone, 0x48) = U16(CUR(), 0x48);
		U8(bone, 0x1B) = U8(CUR(), 0x1B);
		{
			uint8_t *src = CUR() + 0xB0;
			uint8_t *dst = bone + 0xB0;
			GFC_TAINT(dst, src, 8);
			for (int i = 2; i != 0; i--)
			{
				U32(dst, 0) = U32(src, 0);
				src += 4;
				dst += 4;
			}
		}
		U16(bone, 0x14) = U16(CUR(), 0x12);
		U16(bone, 0x16) = U16(CUR(), 0x14);
		U16(bone, 0x12) = U16(SCENE(), 0);
		U16(SCENE(), 0) = (uint16_t)(U16(SCENE(), 0) + 1);
		uint8_t *saved = CUR();
		CUR() = bone;
		C().bone[U8(bone, 0x18)]();
		h_B2C440();
		CUR() = saved;
		PTR(WS(), 0xFC) = bone;
		STREAM() += 4;
	}

	static int32_t AltTextureLoader_B2E700()
	{
		blob::ReadAltTexture(S16(STREAM(), 2));
		int32_t r = blob::AltTextureUpload(PTR(WS(), 0xF0), PTR(WS(), 0xFC));
		if (r == 0)
		{
			STREAM() += 4;
			return r;
		}
		U16(RT(), 0x3E) = U16(CUR(), 0xC8);
		return r;
	}

	static int32_t UploadImage_B2E750(uint32_t caller_ecx)
	{
		if (LOADBUSY() == 0)
		{
			blob::ReadClut_B66560(S16(STREAM(), 2));
			int32_t r = blob::AltTextureUpload(PTR(WS(), 0xF0), PTR(WS(), 0xFC));
			if (r == 0)
			{
				STREAM() += 4;
				return r;
			}
			U16(RT(), 0x3E) = U16(CUR(), 0xC8);
			return r;
		}
		U16(RT(), 0x3E) = U16(CUR(), 0xC8);
		return (int32_t)caller_ecx;
	}

	static void __cdecl op_036_SetSprite()
	{
		blob::SetSpriteAnim(S16(STREAM(), 2));
		U8(CUR(), 0x1E) = 1;
		STREAM() += 4;
	}

	static void __cdecl op_0AC_AccelerateToTarget()
	{
		uint8_t *st = STREAM();
		int32_t imm = U16(RT(), 0x4A) & 0x200;
		int32_t n = S16(st, 2);
		int32_t div = shl32(mul32(n + 1, n), 1) / 2;
		uint8_t *tgt = nullptr;
		if (imm != 0)
		{
			STREAM() = st + 4;
		}
		else
		{
			tgt = blob::GetBone(S16(st, 4));
			STREAM() = STREAM() + 6;
		}
		uint8_t *cur = CUR();
		uint8_t *acc = cur + 0x80;
		uint8_t *vel = cur + 0x68;
		if (imm == 0) tgt += 0x50;
		uint32_t mask = U16(RT(), 0x4A);
		for (int k = 6; k != 0; k--)
		{
			if (mask & 0x8000)
			{
				int32_t v;
				if (imm != 0)
				{
					int32_t e = mul32(S32(vel, 0), n);
					int32_t a = sub32(shl32(S16(STREAM(), 0), 16), e);
					a = a / div;
					STREAM() += 2;
					v = a >> 7;
				}
				else
				{
					int32_t e = S32(vel, 0) >> 4;
					int32_t a = S32(tgt, 0) >> 4;
					e = mul32(e, n);
					a = sub32(a, e);
					e = S32(vel, -0x18) >> 4;
					a = sub32(a, e);
					a = a / div;
					v = a >> 3;
				}
				v = v >> 4;
				U16(acc, 0) = (uint16_t)v;
			}
			acc += 2;
			if (imm == 0) tgt += 4;
			vel += 4;
			mask <<= 1;
		}
		h_B2D460();
	}

	static void __cdecl op_026_StartChannel()
	{
		uint8_t *st = STREAM();
		uint8_t *cur = CUR();
		int32_t ch = S16(st, 2);
		if (ch == 0)
		{
			ch = 1;
			uint8_t *p = cur + 4;
			while (U32(p, 0) != 0)
			{
				ch++;
				p += 4;
				if (ch >= 4)
				{
					STREAM() = st + 6;
					return;
				}
			}
		}
		PTR(cur, ch * 4) = st + S16(st, 4);
		S32(WS(), 0xF0) = ch;
		ResetChannel_B2D8A0();
		STREAM() += 6;
	}

	static void __cdecl op_08B_StartChannelPos()
	{
		uint8_t *st = STREAM();
		uint8_t *cur = CUR();
		int32_t ch = S16(st, 2);
		if (ch == 0)
		{
			ch = 1;
			uint8_t *p = cur + 4;
			while (U32(p, 0) != 0)
			{
				ch++;
				p += 4;
				if (ch >= 4)
				{
					STREAM() = st + 6;
					return;
				}
			}
		}
		U32(cur, ch * 4) = ((uint32_t)st + (uint32_t)(int32_t)S16(st, 4)) & 0x7FFFFFFF;
		S32(WS(), 0xF0) = ch;
		ResetChannel_B2D8A0();
		STREAM() += 6;
	}

	static void __cdecl op_02A_StopChannels()
	{
		uint32_t ch = (uint32_t)U16(RT(), 0x4A) >> 14;
		if (ch != 0)
		{
			U32(CUR(), ch * 4) = 0;
			STREAM() += 2;
			return;
		}
		U32(CUR(), 8) = 0;
		U32(CUR(), 4) = 0;
		STREAM() += 2;
	}

	static void __cdecl op_02E_SetBoneIdShort()
	{
		U16(CUR(), 0x12) = U16(STREAM(), 2);
		STREAM() += 2;
	}

	static void __cdecl op_032_SpawnBone()
	{
		SpawnBone_B2D610((uint32_t)RT());
	}

	static void __cdecl op_135_SpawnBoneWithId()
	{
		SpawnBone_B2D610((uint32_t)RT());
		uint8_t *st = STREAM();
		U16(PTR(WS(), 0xFC), 0x12) = U16(st, 0);
		STREAM() += 2;
	}

	static void __cdecl op_05D_SpawnPerTarget()
	{
		int32_t i = 0;
		int32_t n = S8(SCENE(), 0x41);
		if (n <= 0)
		{
			STREAM() += 4;
			return;
		}
		uint32_t ecx = (uint32_t)RT();
		uint8_t *st;
		do
		{
			SpawnBone_B2D610(ecx);
			uint8_t *ws = WS();
			uint8_t *scene = SCENE();
			uint8_t slot = U8(scene, i + 0x48);
			ecx = ((uint32_t)ws & 0xFFFFFF00) | slot;
			U8(PTR(ws, 0xFC), 0x1B) = slot;
			st = STREAM() - 4;
			i++;
			n--;
			STREAM() = st;
		} while (n != 0);
		STREAM() = st + 4;
	}

	static void __cdecl op_09C_SpawnBoneChained()
	{
		SpawnBone_B2D610((uint32_t)RT());
		uint8_t *b = PTR(WS(), 0xFC);
		U16(b, 0x14) = U16(b, -0xEE);
		U16(b, 0x16) = U16(b, -0xEC);
	}

	static void __cdecl op_05A_SetBoneHandlerParam()
	{
		U8(CUR(), 0x18) = U8(STREAM(), 2);
		U16(CUR(), 0xB0) = U16(STREAM(), 4);
		C().bone[U8(CUR(), 0x18)]();
		STREAM() += 6;
	}

	static void __cdecl op_0A4_SetBoneHandler()
	{
		int32_t h = S16(STREAM(), 2);
		U8(CUR(), 0x18) = (uint8_t)h;
		int32_t ref = S16(STREAM(), 4);
		U16(CUR(), 0xB0) = (uint16_t)ref;
		uint8_t *rb = blob::GetBone(ref);
		if (h == 3)
		{
			uint8_t *cur = CUR();
			int32_t r = blob::Blob_B66B30(S16(rb, 0x94), S16(rb, 0x98), S16(cur, 0x94), S16(cur, 0x98));
			S32(CUR(), 0x64) = shl32(r, 16);
			S32(CUR(), 0x5C) = shl32(S32(WS(), 0xFC), 16);
			cur = CUR();
			S32(cur, 0x60) = shl32(sub32(S16(cur, 0x96), S16(rb, 0x96)), 16);
		}
		C().bone[U8(CUR(), 0x18)]();
		STREAM() += 6;
	}

	static void __cdecl op_0C7_InitSpriteAnimH22()
	{
		op_036_SetSprite();
		U8(CUR(), 0x1C) = 0x16;
	}

	static void __cdecl op_073_SetSpriteOrient2()
	{
		op_036_SetSprite();
		U8(CUR(), 0x1E) = 2;
		S32(CUR(), 0xB8) = S16(STREAM(), 0);
		STREAM() += 2;
	}

	static void __cdecl op_081_SetSpriteAnimOriented()
	{
		op_036_SetSprite();
		U8(CUR(), 0x1E) = (uint8_t)(U16(RT(), 0x4A) >> 12);
	}

	static void __cdecl op_025_SetSpriteOriented()
	{
		op_036_SetSprite();
		U8(CUR(), 0x1E) = 9;
	}

	static void __cdecl op_09B_ClearOptionBits()
	{
		uint32_t w = U16(RT(), 0x4A);
		uint8_t *cur = CUR();
		uint8_t m = (uint8_t)((w >> 9) & 0x7F);
		U8(cur, 0xDE) = (uint8_t)(U8(cur, 0xDE) & (uint8_t)~m);
		STREAM() += 2;
	}

	static void __cdecl op_024_SetOptionBits()
	{
		uint32_t v = (uint32_t)U16(RT(), 0x4A) >> 9;
		if (v & 3)
		{
			U8(CUR(), 0xDE) = (uint8_t)v;
			STREAM() += 2;
			return;
		}
		uint8_t *cur = CUR();
		U8(cur, 0xDE) = (uint8_t)(U8(cur, 0xDE) | (uint8_t)v);
		STREAM() += 2;
	}

	static void __cdecl op_051_SetOptionBitsAndC0()
	{
		uint16_t w = U16(RT(), 0x4A);
		uint8_t *cur = CUR();
		U8(cur, 0xDE) = (uint8_t)(U8(cur, 0xDE) | (uint8_t)(uint16_t)(w >> 9));
		U16(CUR(), 0xC0) = U16(STREAM(), 2);
		STREAM() += 4;
	}

	static void __cdecl op_06B_SetFlagCA()
	{
		U16(CUR(), 0xCA) |= 0x8000;
		STREAM() += 2;
	}

	static void __cdecl op_06C_ClearFlagCA()
	{
		U16(CUR(), 0xCA) &= 0x7FFF;
		STREAM() += 2;
	}

	static void __cdecl op_08E_RemoveFromDrawList()
	{
		blob::DrawListRemove(U8(RT(), 0x42));
		STREAM() += 2;
	}

	static void __cdecl op_0D0_RemoveFromDrawList()
	{
		blob::DrawListRemove(U8(RT(), 0x42));
		STREAM() += 2;
	}

	static void __cdecl op_0EB_AddToDrawList()
	{
		blob::DrawListAddId(U8(RT(), 0x42));
		STREAM() += 2;
	}

	static void __cdecl op_0BC_SetSpriteFrameSpeed()
	{
		U16(CUR(), 0xCA) = U16(STREAM(), 2);
		STREAM() += 4;
	}

	static void __cdecl op_071_DrawEntityInCinematic()
	{
		uint8_t *cur = CUR();
		uint8_t *scene = SCENE();
		uint32_t slot = U8(cur, 0x1B);
		bool clear = (U8(RT(), 0x4B) & 0x80) != 0;
		uint8_t *e = PTR(scene, slot * 4 + 0x60);
		if (clear)
		{
			U16(e, 0) &= 0xFFDF;
			STREAM() += 2;
			return;
		}
		U8(e, 0) |= 0x20;
		uint32_t r = xl_Render_5088A0(e, U32(RT(), 0x4C), 4, U32(CTX(), 0x7C));
		U32(CTX(), 0x7C) = r;
		STREAM() += 2;
	}

	static void __cdecl op_040_ColourFromBoneOutPos()
	{
		uint8_t *b = blob::GetBone(S16(STREAM(), 2));
		int32_t r = S16(b, 0x94);
		if (r > 0xFF) r = 0xFF; else if (r < 0) r = 0;
		int32_t g = S16(b, 0x96);
		if (g > 0xFF) g = 0xFF; else if (g < 0) g = 0;
		int32_t bl = S16(b, 0x98);
		if (bl > 0xFF) bl = 0xFF; else if (bl < 0) bl = 0;
		uint32_t c = (uint32_t)r | (((uint32_t)bl << 8 | (uint32_t)g) << 8);
		uint8_t *cur = CUR();
		U32(cur, 0xCC) = (U32(cur, 0xCC) & 0xFF000000) | c;
		STREAM() += 4;
	}

	static void __cdecl op_0D4_SetColor()
	{
		bool grey = (U8(RT(), 0x4B) & 0x80) != 0;
		uint8_t *st = STREAM();
		uint32_t r = (uint32_t)(int32_t)S16(st, 2);
		uint32_t g, b;
		if (grey)
		{
			g = (uint32_t)shl32((int32_t)r, 8);
			b = (uint32_t)shl32((int32_t)r, 16);
			st += 4;
		}
		else
		{
			g = (uint32_t)shl32(S16(st, 4), 8);
			b = (uint32_t)shl32(S16(st, 6), 16);
			st += 8;
		}
		b |= g;
		STREAM() = st;
		r |= b;
		uint8_t *cur = CUR();
		U32(cur, 0xCC) = (U32(cur, 0xCC) & 0xFF000000) | r;
	}

	static void __cdecl op_0D3_ColorFromBoneOutPos()
	{
		uint8_t *s = blob::GetBone(S16(STREAM(), 2));
		int32_t k = S16(s, 0x8C);
		int32_t r = (S16(s, 0x94) * k) / 256;
		int32_t g = (S16(s, 0x96) * k) / 256;
		int32_t b = (S16(s, 0x98) * k) / 256;
		if (r > 0xFF) r = 0xFF; else if (r < 0) r = 0;
		if (g > 0xFF) g = 0xFF; else if (g < 0) g = 0;
		if (b > 0xFF) b = 0xFF; else if (b < 0) b = 0;
		uint32_t c = (uint32_t)r | (((uint32_t)b << 8 | (uint32_t)g) << 8);
		uint8_t *cur = CUR();
		U32(cur, 0xCC) = (U32(cur, 0xCC) & 0xFF000000) | c;
		STREAM() += 4;
	}

	static void __cdecl op_083_ColorFromOutAngle()
	{
		uint8_t *cur = CUR();
		int32_t r = S16(cur, 0x8C);
		if (r > 0xFF) r = 0xFF; else if (r < 0) r = 0;
		int32_t g = S16(cur, 0x8E);
		if (g > 0xFF) g = 0xFF; else if (g < 0) g = 0;
		int32_t b = S16(cur, 0x90);
		if (b > 0xFF) b = 0xFF; else if (b < 0) b = 0;
		uint32_t c = (uint32_t)r | (((uint32_t)b << 8 | (uint32_t)g) << 8);
		U32(cur, 0xCC) = (U32(cur, 0xCC) & 0xFF000000) | c;
		STREAM() += 2;
	}

	static void __cdecl op_023_SetBlendMode()
	{
		int32_t v = (int32_t)((uint32_t)U16(RT(), 0x4A) >> 9);
		if (v <= 3)
		{
			U32(CUR(), 0xCC) |= 0x2000000;
			uint8_t *cur = CUR();
			U16(cur, 0x92) = (uint16_t)((U16(cur, 0x92) & 0x19F) | (v << 5));
			STREAM() += 2;
			return;
		}
		U32(CUR(), 0xCC) &= 0xFDFFFFFF;
		uint8_t *cur = CUR();
		U16(cur, 0x92) = (uint16_t)((U16(cur, 0x92) & 0xFFBF) | 0x20);
		STREAM() += 2;
	}

	static void __cdecl op_078_SetTexPage()
	{
		int32_t w = S16(STREAM(), 2);
		uint16_t v = (uint16_t)(((w & 0x1F0) << 2) + U16(SCENE(), 0x1C));
		v = (uint16_t)(v + (w & 0xF));
		U16(CUR(), 0x9A) = v;
		STREAM() += 4;
	}

	static void __cdecl op_04B_TintPaletteUpload()
	{
		int32_t w0 = S16(STREAM(), 2);
		blob::ReadClut_B66560(w0 & 0xFF);
		U32(WS(), 0xF0) = U32(WS(), 0xFC);
		U32(WS(), 0xF8) = (uint32_t)shl32(w0 >> 8, 9) + game_addr(CLUTBUF);
		S32(WS(), 0xFC) = S16(STREAM(), 4);
		uint8_t *ws = WS();
		uint32_t d = (U32(ws, 0xFC) & 0xFF) << 1;
		U32(ws, 0xF0) = U32(ws, 0xF0) + d;
		ws = WS();
		U32(ws, 0xF8) = U32(ws, 0xF8) + d;
		uint32_t mode = (uint32_t)U16(RT(), 0x4A) >> 12;
		if (mode == 1)
		{
			S32(WS(), 0xF4) = S16(STREAM(), 6);
			uint8_t *b = blob::GetBone(S16(STREAM(), 8));
			blob::ClutTint_B666F0(S16(b, 0x8C), S16(b, 0x8E), S16(b, 0x90));
			STREAM() += 0xA;
			return;
		}
		if (mode == 2)
		{
			S32(WS(), 0xF4) = S16(STREAM(), 6);
			uint8_t *cur = CUR();
			blob::ClutTint_B666F0(S16(cur, 0x8C), S16(cur, 0x8E), S16(cur, 0x90));
			STREAM() += 8;
			return;
		}
		S32(WS(), 0xF4) = 0x100;
		uint8_t *cur = CUR();
		blob::ClutTint_B666F0(S16(cur, 0x8C), S16(cur, 0x8E), S16(cur, 0x90));
		STREAM() += 6;
	}

	static void __cdecl op_038_SetTargetModelColor()
	{
		uint32_t pal = MODELPALETTE();
		uint8_t *src;
		if (U16(RT(), 0x4A) & 0xFFFFFE00)
			src = CUR() + 0x8C;
		else
			src = CUR() + 0x94;
		S32(WS(), 0xF0) = add32((int32_t)(pal & 0xFF), S16(src, 0) - 0x80);
		S32(WS(), 0xF4) = add32((int32_t)((pal >> 8) & 0xFF), S16(src, 2) - 0x80);
		S32(WS(), 0xF8) = add32((int32_t)((pal >> 16) & 0xFF), S16(src, 4) - 0x80);
		blob::Blob_B66A90();
		uint32_t slot = U8(CUR(), 0x1B);
		uint8_t *e = PTR(SCENE(), slot * 4 + 0x60);
		uint32_t c = U32(WS(), 0xFC);
		U32(e, 0x28) = c;
		STREAM() += 2;
	}

	static void __cdecl op_020_SetFadeLayers()
	{
		S32(WS(), 0xF0) = S16(CUR(), 0x94);
		S32(WS(), 0xF4) = S16(CUR(), 0x96);
		S32(WS(), 0xF8) = S16(CUR(), 0x98);
		blob::Blob_B66A90();
		int32_t k = shl32(S16(CUR(), 0x8C), 4);
		uint32_t c = U32(WS(), 0xFC);
		if (k < 0) k = 0;
		else if (k > 0x1000) k = 0x1000;
		uint32_t p = game_addr(FADELAYERS) + 0x2C;
		for (int i = 4; i != 0; i--)
		{
			MEM<uint16_t>(p - 0x26) = (uint16_t)k;
			MEM<uint32_t>(p) = c;
			p += 0x2C;
		}
		STREAM() += 2;
	}

	static void __cdecl op_0DF_SetupDrawHandler29()
	{
		S32(CUR(), 0xB8) = shl32(S16(STREAM(), 2), 1);
		S32(CUR(), 0xBC) = shl32(S16(STREAM(), 4), 1);
		int32_t w2 = S16(STREAM(), 6);
		U32(CUR(), 0xC0) = (uint32_t)shl32(w2, 1) + game_addr(CLUTBUF);
		S32(CUR(), 0xC4) = S16(STREAM(), 8);
		blob::SetDrawHandler(0x1D);
		STREAM() += 0xA;
	}

	static void __cdecl op_127_Nop6()
	{
		STREAM() += 6;
	}

	static void __cdecl op_029_VramUpload()
	{
		if (LOADBUSY() != 0)
		{
			U16(RT(), 0x3E) = U16(CUR(), 0xC8);
			return;
		}
		uint16_t w = U16(RT(), 0x4A);
		if (w & 0x8000)
		{
			uint8_t *ctx = CTX();
			uint8_t *st = STREAM();
			uint32_t data = U32(ctx, 0xB4);
			uint32_t rect = U32(ctx, 0x98) + (uint32_t)shl32(S16(st, 2), 3);
			x::QueueVramUpload((const void *)rect, (const void *)data);
			STREAM() += 4;
			return;
		}
		if (w & 0x4000)
		{
			uint8_t *st = STREAM();
			uint32_t f = U8(st, 4) & 0x7F;
			uint32_t data = (uint32_t)RESFILE()[f] + (uint32_t)shl32(S16(st, 6), 12);
			uint32_t rect = U32(CTX(), 0x98) + (uint32_t)shl32(S16(st, 2), 3);
			x::QueueVramUpload((const void *)rect, (const void *)data);
			STREAM() += 8;
			return;
		}
		if (w & 0x2000)
		{
			uint8_t *ctx = CTX();
			uint8_t *rect = STREAM() + 2;
			x::QueueVramUpload(rect, PTR(ctx, 0xB4));
			STREAM() += 0xA;
			return;
		}
		AltTextureLoader_B2E700();
	}

	static void __cdecl op_028_UploadImage()
	{
		UploadImage_B2E750((uint32_t)RT());
	}

	static void __cdecl op_027_UploadAltTexAndImage()
	{
		if (LOADBUSY() == 0)
		{
			if (AltTextureLoader_B2E700() == 0)
			{
				STREAM() -= 4;
				if (UploadImage_B2E750(0) == 0) return;
			}
		}
		U16(RT(), 0x3E) = U16(CUR(), 0xC8);
	}

	static void __cdecl op_04F_Nop8()
	{
		STREAM() += 8;
	}

	static void __cdecl op_06F_Nop6()
	{
		STREAM() += 6;
	}

	static void __cdecl op_090_Nop10()
	{
		STREAM() += 0xA;
	}

	static void __cdecl op_052_SetTexture()
	{
		int32_t t = S16(STREAM(), 2);
		if (t == -1)
		{
			U16(CUR(), 0x92) = 0;
			U16(CUR(), 0x9A) = 0;
			U16(CUR(), 0x9E) = 0;
			STREAM() += 4;
			return;
		}
		blob::TexInfo_B665C0(t);
		U16(CUR(), 0x92) = U16(WS(), 0xFC);
		U16(CUR(), 0x9E) = U16(WS(), 0xF8);
		int32_t r = blob::ClutWord_B66640(t);
		U16(CUR(), 0x9A) = (uint16_t)r;
		STREAM() += 4;
	}

	static void __cdecl op_092_SetField92And9A()
	{
		U16(CUR(), 0x92) = U16(STREAM(), 2);
		U16(CUR(), 0x9A) = U16(STREAM(), 4);
		STREAM() += 6;
	}
}

	void fill_vm_c(Generic &g)
	{
		g.vm[0x0AC] = part_vm_c::op_0AC_AccelerateToTarget;
		g.vm[0x026] = part_vm_c::op_026_StartChannel;
		g.vm[0x08B] = part_vm_c::op_08B_StartChannelPos;
		g.vm[0x02A] = part_vm_c::op_02A_StopChannels;
		g.vm[0x02E] = part_vm_c::op_02E_SetBoneIdShort;
		g.vm[0x032] = part_vm_c::op_032_SpawnBone;
		g.vm[0x135] = part_vm_c::op_135_SpawnBoneWithId;
		g.vm[0x05D] = part_vm_c::op_05D_SpawnPerTarget;
		g.vm[0x09C] = part_vm_c::op_09C_SpawnBoneChained;
		g.vm[0x05A] = part_vm_c::op_05A_SetBoneHandlerParam;
		g.vm[0x0A4] = part_vm_c::op_0A4_SetBoneHandler;
		g.vm[0x036] = part_vm_c::op_036_SetSprite;
		g.vm[0x0C7] = part_vm_c::op_0C7_InitSpriteAnimH22;
		g.vm[0x073] = part_vm_c::op_073_SetSpriteOrient2;
		g.vm[0x081] = part_vm_c::op_081_SetSpriteAnimOriented;
		g.vm[0x025] = part_vm_c::op_025_SetSpriteOriented;
		g.vm[0x09B] = part_vm_c::op_09B_ClearOptionBits;
		g.vm[0x024] = part_vm_c::op_024_SetOptionBits;
		g.vm[0x051] = part_vm_c::op_051_SetOptionBitsAndC0;
		g.vm[0x06B] = part_vm_c::op_06B_SetFlagCA;
		g.vm[0x06C] = part_vm_c::op_06C_ClearFlagCA;
		g.vm[0x08E] = part_vm_c::op_08E_RemoveFromDrawList;
		g.vm[0x0D0] = part_vm_c::op_0D0_RemoveFromDrawList;
		g.vm[0x0EB] = part_vm_c::op_0EB_AddToDrawList;
		g.vm[0x0BC] = part_vm_c::op_0BC_SetSpriteFrameSpeed;
		g.vm[0x071] = part_vm_c::op_071_DrawEntityInCinematic;
		g.vm[0x040] = part_vm_c::op_040_ColourFromBoneOutPos;
		g.vm[0x0D4] = part_vm_c::op_0D4_SetColor;
		g.vm[0x0D3] = part_vm_c::op_0D3_ColorFromBoneOutPos;
		g.vm[0x083] = part_vm_c::op_083_ColorFromOutAngle;
		g.vm[0x023] = part_vm_c::op_023_SetBlendMode;
		g.vm[0x078] = part_vm_c::op_078_SetTexPage;
		g.vm[0x04B] = part_vm_c::op_04B_TintPaletteUpload;
		g.vm[0x038] = part_vm_c::op_038_SetTargetModelColor;
		g.vm[0x020] = part_vm_c::op_020_SetFadeLayers;
		g.vm[0x0DF] = part_vm_c::op_0DF_SetupDrawHandler29;
		g.vm[0x127] = part_vm_c::op_127_Nop6;
		g.vm[0x029] = part_vm_c::op_029_VramUpload;
		g.vm[0x028] = part_vm_c::op_028_UploadImage;
		g.vm[0x027] = part_vm_c::op_027_UploadAltTexAndImage;
		g.vm[0x04F] = part_vm_c::op_04F_Nop8;
		g.vm[0x06F] = part_vm_c::op_06F_Nop6;
		g.vm[0x090] = part_vm_c::op_090_Nop10;
		g.vm[0x052] = part_vm_c::op_052_SetTexture;
		g.vm[0x092] = part_vm_c::op_092_SetField92And9A;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace part_vm_d
{
	inline int16_t *ROOTANG() { return (int16_t *)game_addr(game::g_GfCinematic_RootAngleX); }
	inline uint8_t *AUXMAT(int32_t slot) { return (uint8_t *)(0x2798B68u + (uint32_t)shl32(slot, 5)); }

	inline uint8_t *xl_B66B80(void *hdr, int32_t joint, int32_t scale, void *m)
	{
		return x::f<uint8_t *(__cdecl *)(void *, int32_t, int32_t, void *)>(game_addr(game::GfCinematic_ModelJointMatrix))(hdr, joint, scale, m);
	}

	static void __cdecl op_000_EndChannel();

	static void __cdecl op_065_NodeCamRotFromPos()
	{
		uint8_t *node = (uint8_t *)blob::NodeForCurBone();
		x::RotMatrixFromAngles(CUR() + 0x94, node);
		S32(node, 0x14) = MEM<int32_t>(game_addr(game::g_GfCinematic_CamMatMain_TransX));
		S32(node, 0x18) = MEM<int32_t>(game_addr(game::g_GfCinematic_CamMatMain_TransY));
		S32(node, 0x1C) = MEM<int32_t>(game_addr(game::g_GfCinematic_CamMatMain_TransZ));
		x::MulMatrixGte(NODEMAT(), node);
		pad_after_rot_mul(node, CUR() + 0x94);
		U16(node, 0x48) = 0;
		STREAM() += 4;
	}

	static void __cdecl op_066_NodeCamTranslate()
	{
		uint8_t *node = (uint8_t *)blob::NodeForCurBone();
		x::SetRotMatrix(NODEMAT());
		x::SetTransVector(NODEMAT());
		x::TransformToCamera(CUR() + 0x94, node + 0x14, WS() + 0xF0);
		uint8_t *cam = (uint8_t *)NODEMAT();
		U32(node, 0x00) = U32(cam, 0x00);
		U32(node, 0x04) = U32(cam, 0x04);
		U32(node, 0x08) = U32(cam, 0x08);
		U32(node, 0x0C) = U32(cam, 0x0C);
		U32(node, 0x10) = U32(cam, 0x10);
		U16(node, 0x12) = U16(CTX(), 2);
		STREAM() += 4;
	}

	static void __cdecl op_06A_NodeCamRotLocalPos()
	{
		uint8_t *node = (uint8_t *)blob::NodeForCurBone();
		Mat4x3 *r = blob::RotMatrixOrder(CUR() + 0x8C, (int32_t)((uint32_t)U16(RT(), 0x4A) >> 12));
		x::MulMatrix3(NODEMAT(), r, node);
		S32(node, 0x14) = S16(CUR(), 0x94);
		S32(node, 0x18) = S16(CUR(), 0x96);
		S32(node, 0x1C) = S16(CUR(), 0x98);
		U16(node, 0x12) = U16(CTX(), 2);
		STREAM() += 4;
	}

	static void __cdecl op_067_NodeCamRotTranslate()
	{
		uint8_t *node = (uint8_t *)blob::NodeForCurBone();
		x::SetRotMatrix(NODEMAT());
		x::SetTransVector(NODEMAT());
		x::TransformToCamera(CUR() + 0x94, node + 0x14, WS() + 0xF0);
		Mat4x3 *r = x::f<Mat4x3 *(__cdecl *)(void *, uint32_t)>(game_addr(game::GfCinematic_RotMatrixOrder))(CUR() + 0x8C, (uint32_t)U16(RT(), 0x4A) >> 12);
		x::MulMatrix3(NODEMAT(), r, node);
		U16(node, 0x12) = 0;
		STREAM() += 4;
	}

	static void __cdecl op_069_NodeChildOfParent()
	{
		uint8_t *node = (uint8_t *)blob::NodeForCurBone();
		Mat4x3 *parent = blob::GetParentMatrix(S16(STREAM(), 4));
		x::RotMatrixFromAngles(CUR() + 0x8C, node);
		x::MulMatrixInPlace(node, NODEMAT());
		x::MulMatrixInPlace(node, parent);
		S32(node, 0x14) = S16(CUR(), 0x94);
		S32(node, 0x18) = S16(CUR(), 0x96);
		S32(node, 0x1C) = S16(CUR(), 0x98);
		U16(node, 0x12) = 0;
		STREAM() += 6;
	}

	static void __cdecl op_0C4_NodeLocalNoCamera()
	{
		uint8_t *node = (uint8_t *)blob::NodeForCurBone();
		uint8_t *r = (uint8_t *)blob::RotMatrixOrder(CUR() + 0x8C, (int32_t)((uint32_t)U16(RT(), 0x4A) >> 12));
		U32(node, 0x00) = U32(r, 0x00);
		U32(node, 0x04) = U32(r, 0x04);
		U32(node, 0x08) = U32(r, 0x08);
		U32(node, 0x0C) = U32(r, 0x0C);
		U32(node, 0x10) = U32(r, 0x10);
		S32(node, 0x14) = S16(CUR(), 0x94);
		S32(node, 0x18) = S16(CUR(), 0x96);
		S32(node, 0x1C) = S16(CUR(), 0x98);
		U16(node, 0x12) = U16(CTX(), 2);
		STREAM() += 4;
	}

	static void __cdecl op_116_NodeCamPosLocalRot()
	{
		uint8_t *node = (uint8_t *)blob::NodeForCurBone();
		x::SetRotMatrix(NODEMAT());
		x::SetTransVector(NODEMAT());
		x::TransformToCamera(CUR() + 0x94, node + 0x14, WS() + 0xF0);
		x::RotMatrixFromAngles(CUR() + 0x8C, node);
		U16(node, 0x12) = 0;
		STREAM() += 4;
	}

	static void __cdecl op_095_Nop4()
	{
		STREAM() += 4;
	}

	static void __cdecl op_07A_NodeFromParentAndMatrix()
	{
		int32_t ref = S16(STREAM(), 2);
		uint8_t *other = blob::GetBone(ref);
		uint8_t *node = (uint8_t *)blob::NodeForCurBone();
		uint8_t *saved = CUR();
		CUR() = other;
		uint8_t *parent = (uint8_t *)NODEMAT();
		if (ref != 0) parent = (uint8_t *)blob::NodeForCurBone();
		CUR() = saved;
		int32_t ofs = S16(STREAM(), 4);
		uint8_t *m = (ofs == 0) ? node : saved + ofs;
		x::ComposeAffine(parent, m, node);
		if (U8(RT(), 0x4B) & 0x80)
			U16(node, 0x12) = U16(CTX(), 2);
		else
			U16(node, 0x12) = 0;
		STREAM() += 6;
	}

	static void __cdecl op_084_CopyMatrix()
	{
		uint8_t *other = blob::GetBone(S16(STREAM(), 2));
		uint8_t *s = STREAM();
		uint8_t *saved = CUR();
		CUR() = other;
		int32_t ofs = S16(s, 4);
		uint8_t *src = (ofs == 0) ? (uint8_t *)blob::NodeForCurBone() : other + ofs;
		CUR() = saved;
		int32_t dofs = S16(STREAM(), 6);
		uint8_t *dst = (dofs == 0) ? (uint8_t *)blob::NodeForCurBone() : saved + dofs;
		GFC_TAINT(dst, src, 0x20);
		blob::CopyMatrix(dst, src);
		STREAM() += 8;
	}

	static void __cdecl op_068_SetParentNode()
	{
		U16(CUR(), 0x9C) = U16(STREAM(), 2);
		STREAM() += 4;
	}

	static void __cdecl op_0CB_NodeLookAt()
	{
		int32_t a = S16(STREAM(), 2);
		uint8_t *eye = (a == 0) ? CUR() : blob::GetBone(a);
		int32_t b = S16(STREAM(), 4);
		uint8_t *tgt = (b == 0) ? CUR() : blob::GetBone(b);
		uint8_t *node = (uint8_t *)blob::NodeForCurBone();
		blob::LookAt_B66E50(eye + 0x94, tgt + 0x94, node);
		uint8_t *src = (U8(RT(), 0x4B) & 0x10) ? tgt : eye;
		S32(node, 0x14) = S16(src, 0x94);
		S32(node, 0x18) = S16(src, 0x96);
		S32(node, 0x1C) = S16(src, 0x98);
		x::ComposeAffine(NODEMAT(), node, node);
		U16(node, 0x12) = 0;
		STREAM() += 6;
	}

	static void __cdecl op_104_BuildBillboardSlot()
	{
		uint8_t *aux = AUXMAT(S16(STREAM(), 2));
		uint8_t *r = WS() + 0x20;
		x::RotMatrixFromAngles(CUR() + 0x8C, r);
		x::MulMatrix3(CAMALT(), r, aux);
		x::SetRotMatrix(CAMALT());
		x::SetTransVector(CAMALT());
		x::TransformToCamera(CUR() + 0x94, aux + 0x14, WS() + 0xF0);
		U16(aux, 0x12) = 0;
		STREAM() += 4;
	}

	static void __cdecl op_105_NodeFromBillboardSlot()
	{
		uint8_t *node = (uint8_t *)blob::NodeForCurBone();
		uint8_t *aux = AUXMAT(S16(STREAM(), 2));
		x::SetRotMatrix(aux);
		x::SetTransVector(aux);
		x::TransformToCamera(CUR() + 0x94, node + 0x14, WS() + 0xF0);
		Mat4x3 *r = blob::RotMatrixOrder(CUR() + 0x8C, (int32_t)((uint32_t)U16(RT(), 0x4A) >> 12));
		x::MulMatrix3(aux, r, node);
		U16(node, 0x12) = 0;
		STREAM() += 6;
	}

	static void __cdecl op_117_SetRotRelativeToRoot()
	{
		uint8_t *s = STREAM();
		int32_t rx = ROOTANG()[0];
		int32_t y = S16(s, 4);
		int32_t z = S16(s, 6);
		int32_t x0 = S16(s, 2);
		x0 = sub32(x0, rx);
		int32_t ry = ROOTANG()[1];
		int32_t rz = ROOTANG()[2];
		S32(CUR(), 0x50) = shl32(x0, 16);
		y = sub32(y, ry);
		z = sub32(z, rz);
		S32(CUR(), 0x54) = shl32(y, 16);
		S32(CUR(), 0x58) = shl32(z, 16);
		h_B2C440();
		STREAM() += 8;
	}

	static void __cdecl op_063_PosFromBoneViaParent()
	{
		uint8_t *saved = CUR();
		uint8_t *src = blob::GetBone(S16(STREAM(), 2));
		CUR() = src;
		uint8_t *par = blob::GetBone((int32_t)U16(src, 0x9C));
		Mat4x3 *r = blob::RotMatrixOrder(par + 0x8C, S16(STREAM(), 4));
		x::SetRotMatrix(r);
		int32_t px = S16(par, 0x94);
		int32_t pz = S16(par, 0x98);
		int32_t py = S16(par, 0x96);
		x::GteWriteCtrl(px, 5);
		x::GteWriteCtrl(py, 6);
		x::GteWriteCtrl(pz, 7);
		x::TransformToCamera(CUR() + 0x94, WS() + 0xF0, WS() + 0xFC);
		CUR() = saved;
		S32(saved, 0x5C) = shl32(S32(WS(), 0xF0), 16);
		S32(CUR(), 0x60) = shl32(S32(WS(), 0xF4), 16);
		S32(CUR(), 0x64) = shl32(S32(WS(), 0xF8), 16);
		C().bone[U8(CUR(), 0x18)]();
		STREAM() += 6;
	}

	static void __cdecl op_05B_AttachToModelJoint()
	{
		uint8_t *hdr;
		uint8_t *base;
		uint8_t *mbone = nullptr;
		int32_t nodeofs;
		int32_t joint, scale;
		if (U8(RT(), 0x4B) & 4)
		{
			uint8_t *e = PTR(SCENE(), 0x60 + U8(CUR(), 0x1B) * 4);
			hdr = e + 0x60;
			base = e + 0x40;
		}
		else
		{
			mbone = blob::GetBone(S16(STREAM(), 2));
			uint8_t *blk = PTR(mbone, 0xBC);
			if (blk == nullptr)
			{
				if (U8(RT(), 0x4B) & 8)
					STREAM() += 8;
				else
					STREAM() += 4;
				return;
			}
			hdr = blk + 0x10;
			base = nullptr;
		}
		if (U8(RT(), 0x4B) & 8)
		{
			uint8_t *s = STREAM();
			int32_t w1 = S16(s, 4);
			scale = S16(s, 6);
			nodeofs = (int32_t)(((uint32_t)w1 & 0xFFFF) >> 8);
			joint = w1 & 0xFF;
			scale = shl32(scale, 4);
			STREAM() = s + 8;
		}
		else
		{
			uint8_t *c = CUR();
			uint8_t *s = STREAM();
			nodeofs = 0;
			joint = S16(c, 0x8C);
			scale = 0x1000;
			STREAM() = s + 4;
		}
		if (base == nullptr)
		{
			uint8_t *w = WS() + 0x60;
			U32(w, 0x00) = MEM<uint32_t>(game_addr(game::unk_2797968));
			U32(w, 0x04) = MEM<uint32_t>(game_addr(game::unk_279796C));
			U32(w, 0x08) = MEM<uint32_t>(game_addr(game::unk_2797970));
			U32(w, 0x0C) = MEM<uint32_t>(game_addr(game::unk_2797974));
			U32(w, 0x10) = MEM<uint32_t>(game_addr(game::unk_2797978));
			S32(w, 0x14) = S16(CUR(), 0x94);
			S32(w, 0x18) = S16(CUR(), 0x96);
			S32(w, 0x1C) = S16(CUR(), 0x98);
			S32(w, 0x20) = shl32(S16(mbone, 0x8C), 4);
			S32(w, 0x24) = shl32(S16(mbone, 0x8E), 4);
			S32(w, 0x28) = shl32(S16(mbone, 0x90), 4);
			x::ScaleMatrix(w, w + 0x20);
			base = w;
		}
		x::BuildBoneMatricesFromPose(hdr);
		uint8_t *jm = xl_B66B80(hdr, joint, scale, base);
		switch ((uint32_t)U16(RT(), 0x4A) >> 12)
		{
		case 1:
		{
			uint8_t *node = (uint8_t *)blob::NodeForCurBone();
			blob::CopyMatrix(node, jm);
			S32(node, 0x14) = S32(WS(), 0xF0);
			S32(node, 0x18) = S32(WS(), 0xF4);
			S32(node, 0x1C) = S32(WS(), 0xF8);
			x::ComposeAffine(NODEMAT(), node, node);
			U16(node, 0x12) = 0;
			return;
		}
		case 2:
		{
			uint8_t *node = (nodeofs == 0) ? (uint8_t *)blob::NodeForCurBone() : CUR() + nodeofs;
			blob::CopyMatrix(node, jm);
			GFC_TAINT(node + 0x12, nullptr, 2);
			S32(node, 0x14) = S32(WS(), 0xF0);
			S32(node, 0x18) = S32(WS(), 0xF4);
			S32(node, 0x1C) = S32(WS(), 0xF8);
			return;
		}
		default:
			S32(CUR(), 0x5C) = shl32(S32(WS(), 0xF0), 16);
			S32(CUR(), 0x60) = shl32(S32(WS(), 0xF4), 16);
			S32(CUR(), 0x64) = shl32(S32(WS(), 0xF8), 16);
			C().bone[U8(CUR(), 0x18)]();
			return;
		}
	}

	static void __cdecl op_054_SetMesh()
	{
		U32(CUR(), 0xCC) = U32(CUR(), 0xCC) & 0x2FFFFFF;
		U32(CUR(), 0xBC) = 0;
		uint8_t *obj = blob::ObjectPtr(S16(STREAM(), 2));
		PTR(CUR(), 0xD8) = obj;
		blob::SetDrawHandler(1);
		STREAM() += 4;
	}

	static void __cdecl op_03C_SetMeshScaled()
	{
		op_054_SetMesh();
		U8(CUR(), 0x1C) = 2;
	}

	static void __cdecl op_056_SetClippedMesh()
	{
		op_054_SetMesh();
		U8(CUR(), 0x1C) = 9;
		U16(CUR(), 0xC2) = U16(STREAM(), 0);
		STREAM() += 2;
	}

	static void __cdecl op_03D_SetMeshTexParam()
	{
		U32(CUR(), 0xCC) = (U32(CUR(), 0xCC) & 0x2000000) | 0x808080;
		uint8_t *obj = blob::ObjectPtr(S16(STREAM(), 4));
		PTR(CUR(), 0xD8) = obj;
		S32(CUR(), 0xB8) = shl32(S16(STREAM(), 6), 4);
		U32(CUR(), 0xBC) = 0;
		blob::SetDrawHandler(S16(STREAM(), 2));
		STREAM() += 8;
	}

	static void __cdecl op_04A_SetMeshDrawHandler()
	{
		U32(CUR(), 0xCC) = U32(CUR(), 0xCC) & 0x2FFFFFF;
		U32(CUR(), 0xBC) = 0;
		uint8_t *obj = blob::ObjectPtr(S16(STREAM(), 2));
		PTR(CUR(), 0xD8) = obj;
		blob::SetDrawHandler((int32_t)((uint32_t)U16(RT(), 0x4A) >> 9));
		STREAM() += 4;
	}

	static void __cdecl op_03E_SetInlineDescriptor3()
	{
		PTR(CUR(), 0xBC) = STREAM() + 2;
		STREAM() += 8;
	}

	static void __cdecl op_093_BuildLightMatrices()
	{
		uint8_t *s = STREAM();
		int32_t slot = S16(s, 2);
		uint8_t *L = (uint8_t *)(game_addr(game::g_GfCinematic_BoneMatrixTable) + (uint32_t)mul32(slot, 0x50));
		uint8_t *m = WS() + 0x20;
		int32_t id = S16(s, 4);
		if (id != -1)
		{
			uint8_t *b = blob::GetBone(id);
			U16(m, 0x0) = (uint16_t)(U16(b, 0x94) << 4);
			U16(m, 0x2) = (uint16_t)(U16(b, 0x96) << 4);
			U16(m, 0x4) = (uint16_t)(U16(b, 0x98) << 4);
			U16(L, 0x20) = (uint16_t)(U16(b, 0x8C) << 4);
			U16(L, 0x26) = (uint16_t)(U16(b, 0x8E) << 4);
			U16(L, 0x2C) = (uint16_t)(U16(b, 0x90) << 4);
		}
		else
		{
			U16(m, 0x4) = 0;
			U16(m, 0x2) = 0;
			U16(m, 0x0) = 0;
		}
		id = S16(STREAM(), 6);
		if (id != -1)
		{
			uint8_t *b = blob::GetBone(id);
			U16(m, 0x6) = (uint16_t)(U16(b, 0x94) << 4);
			U16(m, 0x8) = (uint16_t)(U16(b, 0x96) << 4);
			U16(m, 0xA) = (uint16_t)(U16(b, 0x98) << 4);
			U16(L, 0x22) = (uint16_t)(U16(b, 0x8C) << 4);
			U16(L, 0x28) = (uint16_t)(U16(b, 0x8E) << 4);
			U16(L, 0x2E) = (uint16_t)(U16(b, 0x90) << 4);
		}
		else
		{
			U16(m, 0xA) = 0;
			U16(m, 0x8) = 0;
			U16(m, 0x6) = 0;
		}
		id = S16(STREAM(), 8);
		if (id != -1)
		{
			uint8_t *b = blob::GetBone(id);
			U16(m, 0xC) = (uint16_t)(U16(b, 0x94) << 4);
			U16(m, 0xE) = (uint16_t)(U16(b, 0x96) << 4);
			U16(m, 0x10) = (uint16_t)(U16(b, 0x98) << 4);
			U16(L, 0x24) = (uint16_t)(U16(b, 0x8C) << 4);
			U16(L, 0x2A) = (uint16_t)(U16(b, 0x8E) << 4);
			U16(L, 0x30) = (uint16_t)(U16(b, 0x90) << 4);
		}
		else
		{
			U16(m, 0x10) = 0;
			U16(m, 0xE) = 0;
			U16(m, 0xC) = 0;
		}
		uint8_t *ang = m + 0x30;
		U16(ang, 0) = (uint16_t)((uint32_t)U8(CUR(), 0x94) << 4);
		U16(m, 0x32) = (uint16_t)((uint32_t)U8(CUR(), 0x96) << 4);
		U16(m, 0x34) = (uint16_t)((uint32_t)U8(CUR(), 0x98) << 4);
		x::RotMatrixFromAngles(ang, L);
		x::MulMatrixGte(m, L);
		pad_after_rot_mul(L, ang);
		S32(L, 0x40) = shl32(S16(CUR(), 0x8C), 4);
		S32(L, 0x44) = shl32(S16(CUR(), 0x8E), 4);
		S32(L, 0x48) = shl32(S16(CUR(), 0x90), 4);
		STREAM() += 10;
	}

	static void __cdecl op_0AA_SetLightSlot()
	{
		int32_t v = S16(STREAM(), 2);
		if (v == -1)
		{
			U8(CUR(), 0xE1) = 0;
			STREAM() += 4;
			return;
		}
		U8(CUR(), 0xE1) = (uint8_t)v;
		STREAM() += 4;
	}

	static void __cdecl op_0C6_SetFogColorAndNear()
	{
		uint8_t *c = CUR();
		int32_t b = S16(c, 0x90), g = S16(c, 0x8E), r = S16(c, 0x8C);
		x::GteSetBackColor3(r, g, b);
		c = CUR();
		int32_t y = S16(c, 0x96), xx = S16(c, 0x94);
		x::Fog_56CCC0(xx, y);
		STREAM() += 2;
	}

	static void __cdecl op_11B_SetFogColorAndFar()
	{
		uint8_t *c = CUR();
		int32_t b = S16(c, 0x90), g = S16(c, 0x8E), r = S16(c, 0x8C);
		x::GteSetBackColor3(r, g, b);
		c = CUR();
		int32_t y = S16(c, 0x96), xx = S16(c, 0x94);
		x::Fog_56CCA0(xx, y);
		STREAM() += 2;
	}

	static void __cdecl op_0F1_Nop2()
	{
		STREAM() += 4;
	}

	static void __cdecl op_001_LoopSequenceOrFinish()
	{
		uint8_t *ctx = CTX();
		uint8_t cnt = U8(ctx, 0xD0);
		if (cnt >= U8(ctx, 0xD1))
		{
			ff8fx::fill<(0x80)>(ORDER(), 0xFF);
			U32(SEQ(), 8) = 0;
			U32(CUR(), 8) = 0;
			U32(CUR(), 4) = 0;
			U32(CUR(), 0) = 0;
			U16(RT(), 0x3E) = 0xFFFF;
			U16(SCENE(), 0x18) = 0;
			STREAM() = nullptr;
			U16(SCENE(), 0x1A) = 4;
			return;
		}
		uint8_t *saved = CUR();
		uint8_t *s = STREAM();
		U8(ctx, 0xD0) = (uint8_t)(cnt + 1);
		h_B2F8F0();
		STREAM() = s;
		CUR() = saved;
		STREAM() = s + S16(s, 2);
		U8(RT(), 0x44) = 0;
		U16(RT(), 0x3E) = 0;
	}

	static void __cdecl op_0AE_NextTargetOrJump()
	{
		uint8_t *ctx = CTX();
		uint8_t cnt = U8(ctx, 0xD0);
		if (cnt >= U8(ctx, 0xD1))
		{
			uint8_t *s = STREAM();
			STREAM() = s + S16(s, 2);
			return;
		}
		U8(ctx, 0xD0) = (uint8_t)(cnt + 1);
		h_B2FC20();
		STREAM() += 4;
	}

	static void __cdecl op_009_Wait()
	{
		uint8_t *rt = RT();
		uint16_t v = (uint16_t)(U16(rt, 0x4A) >> 9);
		U16(rt, 0x3E) = (uint16_t)((uint32_t)v << 7);
		STREAM() += 2;
	}

	static void __cdecl op_000_EndChannel()
	{
		uint8_t *rt = RT();
		if (U8(rt, 0x43) != 0)
		{
			STREAM() = nullptr;
			U16(rt, 0x3E) = 0x8000;
			return;
		}
		U32(CUR(), 0x0) = 0;
		U32(CUR(), 0x4) = 0;
		U32(CUR(), 0x8) = 0;
		U32(CUR(), 0xC) = 0;
		int32_t id = U8(RT(), 0x42);
		blob::OrderListRemove(id);
		blob::DrawListRemove(id);
		U16(SCENE(), 0x18) = (uint16_t)(U16(SCENE(), 0x18) - 1);
		U16(RT(), 0x3E) = U16(CUR(), 0xC8);
		STREAM() = nullptr;
		U8(RT(), 0x44) = (uint8_t)(U8(RT(), 0x44) - 1);
		uint32_t key = U16(CUR(), 0x12);
		uint16_t *k = NODEKEYS();
		for (int i = 0; i < 0x40; i++, k++)
		{
			if (key == (uint32_t)*k)
			{
				*k = 0;
				return;
			}
		}
	}

	static void __cdecl op_01D_WaitBoneGoneThenEnd()
	{
		uint8_t *b = blob::GetBone(S16(STREAM(), 2));
		if (b == PTR(CTX(), 0x90))
		{
			op_000_EndChannel();
			return;
		}
		U16(RT(), 0x3E) = 0x80;
	}

	static void __cdecl op_09E_JumpIfCtxFlag15Clear()
	{
		uint8_t f = U8(CTX(), 1);
		uint8_t *s = STREAM();
		if (f & 0x80)
			STREAM() = s + 4;
		else
			STREAM() = s + S16(s, 2);
	}

	static void __cdecl op_09F_JumpIfCtxFlag15Set()
	{
		uint8_t f = U8(CTX(), 1);
		uint8_t *s = STREAM();
		if (f & 0x80)
			STREAM() = s + S16(s, 2);
		else
			STREAM() = s + 4;
	}

	static void __cdecl op_0A2_Nop4()
	{
		STREAM() += 4;
	}

	static void __cdecl op_0A0_JumpIfCtxFlag13Clear()
	{
		uint16_t f = (uint16_t)(U16(CTX(), 0) & 0x2000);
		uint8_t *s = STREAM();
		if (f != 0)
			STREAM() = s + 4;
		else
			STREAM() = s + S16(s, 2);
	}

	static void __cdecl op_0B0_RandomWait()
	{
		int32_t r = blob::Rand74(S16(STREAM(), 2));
		uint8_t *s = STREAM();
		int32_t v = mul32(S16(CUR(), 0xC8), r);
		STREAM() = s + 4;
		if (v != 0) U16(RT(), 0x3E) = (uint16_t)v;
	}

	static void __cdecl op_140_WaitScaledBySlot()
	{
		uint32_t idx = U8(CUR(), 0x1B);
		uint16_t f = U8(SCENE(), 0xCC + idx);
		uint16_t p = (uint16_t)(f * U16(STREAM(), 2));
		U16(RT(), 0x3E) = (uint16_t)((uint32_t)p << 7);
		STREAM() += 4;
	}

	static void __cdecl op_050_FreeArenaBlock()
	{
		U32(WS(), 0xF0) = U32(CUR(), 0xC4);
		blob::ArenaFree_B65710();
		STREAM() += 2;
	}
}

	void fill_vm_d(Generic &g)
	{
		g.vm[0x065] = part_vm_d::op_065_NodeCamRotFromPos;
		g.vm[0x066] = part_vm_d::op_066_NodeCamTranslate;
		g.vm[0x06A] = part_vm_d::op_06A_NodeCamRotLocalPos;
		g.vm[0x067] = part_vm_d::op_067_NodeCamRotTranslate;
		g.vm[0x069] = part_vm_d::op_069_NodeChildOfParent;
		g.vm[0x0C4] = part_vm_d::op_0C4_NodeLocalNoCamera;
		g.vm[0x116] = part_vm_d::op_116_NodeCamPosLocalRot;
		g.vm[0x095] = part_vm_d::op_095_Nop4;
		g.vm[0x07A] = part_vm_d::op_07A_NodeFromParentAndMatrix;
		g.vm[0x084] = part_vm_d::op_084_CopyMatrix;
		g.vm[0x068] = part_vm_d::op_068_SetParentNode;
		g.vm[0x0CB] = part_vm_d::op_0CB_NodeLookAt;
		g.vm[0x104] = part_vm_d::op_104_BuildBillboardSlot;
		g.vm[0x105] = part_vm_d::op_105_NodeFromBillboardSlot;
		g.vm[0x117] = part_vm_d::op_117_SetRotRelativeToRoot;
		g.vm[0x063] = part_vm_d::op_063_PosFromBoneViaParent;
		g.vm[0x05B] = part_vm_d::op_05B_AttachToModelJoint;
		g.vm[0x054] = part_vm_d::op_054_SetMesh;
		g.vm[0x03C] = part_vm_d::op_03C_SetMeshScaled;
		g.vm[0x056] = part_vm_d::op_056_SetClippedMesh;
		g.vm[0x03D] = part_vm_d::op_03D_SetMeshTexParam;
		g.vm[0x04A] = part_vm_d::op_04A_SetMeshDrawHandler;
		g.vm[0x03E] = part_vm_d::op_03E_SetInlineDescriptor3;
		g.vm[0x093] = part_vm_d::op_093_BuildLightMatrices;
		g.vm[0x0AA] = part_vm_d::op_0AA_SetLightSlot;
		g.vm[0x0C6] = part_vm_d::op_0C6_SetFogColorAndNear;
		g.vm[0x11B] = part_vm_d::op_11B_SetFogColorAndFar;
		g.vm[0x0F1] = part_vm_d::op_0F1_Nop2;
		g.vm[0x001] = part_vm_d::op_001_LoopSequenceOrFinish;
		g.vm[0x0AE] = part_vm_d::op_0AE_NextTargetOrJump;
		g.vm[0x009] = part_vm_d::op_009_Wait;
		g.vm[0x000] = part_vm_d::op_000_EndChannel;
		g.vm[0x01D] = part_vm_d::op_01D_WaitBoneGoneThenEnd;
		g.vm[0x09E] = part_vm_d::op_09E_JumpIfCtxFlag15Clear;
		g.vm[0x09F] = part_vm_d::op_09F_JumpIfCtxFlag15Set;
		g.vm[0x0A2] = part_vm_d::op_0A2_Nop4;
		g.vm[0x0A0] = part_vm_d::op_0A0_JumpIfCtxFlag13Clear;
		g.vm[0x0B0] = part_vm_d::op_0B0_RandomWait;
		g.vm[0x140] = part_vm_d::op_140_WaitScaledBySlot;
		g.vm[0x050] = part_vm_d::op_050_FreeArenaBlock;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace part_draw_mesh
{
	static inline void *xl_MatrixMultiplyGte(const void *a, void *b) { return x::f<void *(__cdecl *)(const void *, void *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	static inline void xl_TransformV0ToMac(const void *v, void *mac, void *flag) { x::f<void (__cdecl *)(const void *, void *, void *)>(game_addr(game::TransformCoordinateToCameraSpace))(v, mac, flag); }

	static inline uint8_t *ZEROVEC() { return (uint8_t *)game_addr(game::unk_27971E4); }
	static inline uint32_t &GTEFLAG() { return MEM<uint32_t>(game_addr(game::GTE_FLAG)); }
	static const double DEPTH_SCALE_NUM = 4096.0;

	static void __cdecl dh_21_ScrollTextureUpload()
	{
		uint8_t *arg = PTR(CUR(), 0xB8);
		blob::ReadAltTexture(S16(arg, 0));
		U32(WS(), 0x64) = U32(WS(), 0xFC);
		uint8_t *w = WS();
		int32_t vx = S16(arg, 2);
		uint8_t *tex = PTR(w, 0xF0);
		int32_t vy = S16(arg, 4);
		S32(w, 0x78) = S16(tex, 4);
		S32(WS(), 0x7C) = S16(tex, 6);
		S32(WS(), 0x70) = S16(CUR(), 0x96);
		w = WS();
		S32(w, 0x70) = (int32_t)((uint32_t)S32(w, 0x70) & (uint32_t)sub32(S32(w, 0x7C), 1));
		uint8_t *rect = blob::VramRectRing();
		S16(rect, 0) = (int16_t)vx;
		S16(rect, 2) = (int16_t)vy;
		U16(rect, 4) = U16(WS(), 0x78);
		w = WS();
		U16(rect, 6) = (uint16_t)(U16(w, 0x7C) - U16(w, 0x70));
		w = WS();
		x::QueueVramUpload(rect, (const void *)(U32(w, 0x64) + (uint32_t)mul32(S32(w, 0x78), S32(w, 0x70)) * 2u));
		if (S32(WS(), 0x70) != 0)
		{
			rect = blob::VramRectRing();
			S16(rect, 0) = (int16_t)vx;
			w = WS();
			U16(rect, 2) = (uint16_t)((uint16_t)(U16(w, 0x7C) - U16(w, 0x70)) + (uint16_t)vy);
			U16(rect, 4) = U16(WS(), 0x78);
			U16(rect, 6) = U16(WS(), 0x70);
			x::QueueVramUpload(rect, (const void *)U32(WS(), 0x64));
		}
	}

	static void SetupParentXform()
	{
		Mat4x3 *m = blob::GetParentMatrix(U16(CUR(), 0x9C));
		x::GteSetRotMatrix(m);
		x::GteSetTransVector(m);
		x::GteLoadV0(CUR() + 0x94);
		x::GteMVMVA_RotV0Tr();
		x::Gte_45E580();
	}

	static void SetupParentXformScaledAtOrigin()
	{
		Mat4x3 *m = blob::GetParentMatrix(U16(CUR(), 0x9C));
		x::GteSetRotMatrix(m);
		x::GteSetTransVector(m);
		x::GteLoadV0(ZEROVEC());
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

	static void SetupBillboardXform(const void *pos, const void *angles, int32_t scale, int32_t order)
	{
		blob::RotMatrixOrder(angles, order);
		Mat4x3 *m = blob::GetParentMatrix(U16(CUR(), 0x9C));
		x::SetRotMatrix(m);
		x::SetTransVector(m);
		uint8_t *w = WS();
		xl_TransformV0ToMac(pos, w + 0xF4, w + 0xC0);
		xl_MatrixMultiplyGte(m, WS() + 0xE0);
		S32(WS(), 0xC8) = scale;
		S32(WS(), 0xC4) = scale;
		S32(WS(), 0xC0) = scale;
		w = WS();
		x::ScaleMatrix(w + 0xE0, w + 0xC0);
		x::SetRotMatrix(WS() + 0xE0);
		x::SetTransVector(WS() + 0xE0);
	}

	static void DrawMeshObject(const int16_t *clipArg)
	{
		int16_t local[4];
		const int16_t *clip = clipArg;
		if (clipArg != nullptr)
		{
			local[0] = (int16_t)(uint16_t)((uint32_t)(uint16_t)clipArg[0] << 3);
			local[1] = (int16_t)(uint16_t)((uint32_t)(uint16_t)clipArg[1] << 3);
			local[2] = clipArg[2];
			local[3] = clipArg[3];
			clip = local;
		}
		uint32_t pkt = U32(CTX(), 0x7C);
		U32(WS(), 0x60) = pkt;
		U32(WS(), 0x50) = (uint32_t)(int32_t)S16(CUR(), 0x92) | 0xE1000200u;
		S32(WS(), 0x54) = S16(CUR(), 0x9A);
		U32(WS(), 0x84) = U32(CUR(), 0xCC) & 0x2FFFFFFu;
		U32(WS(), 0x8C) = U32(WS(), 0x84) & 0x2000000u;
		U32(WS(), 0x5C) = U32(RT(), 0x4C);
		U32(WS(), 0x90) = U8(CUR(), 0xDE);
		S32(WS(), 0x94) = S16(CUR(), 0xC0);
		U32(WS(), 0x98) = U16(CUR(), 0x9E);
		U32(WS(), 0x68) = U32(CUR(), 0xD8);
		{
			uint8_t *mesh = PTR(WS(), 0x68);
			uint8_t *w = WS();
			U32(w, 0x6C) = U32(mesh, 0x08) + (uint32_t)mesh;
			U32(WS(), 0x74) = U32(mesh, 0x14) + (uint32_t)mesh;
			U32(WS(), 0x78) = U32(mesh, 0x1C) + (uint32_t)mesh;
			U32(WS(), 0x7C) = U32(CTX(), 0x74);
			w = WS();
			U32(w, 0x80) = U32(w, 0x7C) + U32(mesh, 0x18) * 8u;
			w = WS();
			U32(w, 0x88) = U32(w, 0x80) + U32(mesh, 0x20) * 8u;
		}

		uint8_t *w = WS();
		uint8_t *morph = PTR(w, 0x4C);
		if (morph != nullptr)
		{
			uint8_t *wbone = blob::GetBone(S16(morph, 0));
			uint8_t *objA = blob::ObjectPtr(S16(morph, 2));
			uint8_t *objB = blob::ObjectPtr(S16(morph, 4));
			S32(WS(), 0xE0) = S16(wbone, 0x94);
			S32(WS(), 0xE4) = S16(wbone, 0x96);
			S32(WS(), 0xE8) = S16(wbone, 0x98);
			uint32_t fl = U32(objA, 0) & U32(objB, 0);
			if (fl & 2)
			{
				w = WS();
				U32(w, 0x74) = U32(w, 0x7C);
				U32(WS(), 0xF0) = U32(objA, 0x14) + (uint32_t)objA;
				U32(WS(), 0xF4) = U32(objB, 0x14) + (uint32_t)objB;
				S32(WS(), 0xF8) = S32(objA, 0x18);
				w = WS();
				int32_t n = S32(w, 0xF8);
				uint8_t *a = PTR(w, 0xF0), *b = PTR(w, 0xF4), *d = PTR(w, 0x7C);
				do
				{
					int32_t a0 = S16(a, 0);
					int32_t d0 = sub32(S16(b, 0), a0);
					int32_t p0 = mul32(S16(w, 0xE0), d0);
					int32_t k1 = S16(w, 0xE4);
					int32_t a1 = S16(a, 2);
					int32_t d1 = sub32(S16(b, 2), a1);
					S16(d, 0) = (int16_t)add32(p0 >> 8, a0);
					int32_t p1 = mul32(k1, d1);
					int32_t a2 = S16(a, 4);
					int32_t d2 = sub32(S16(b, 4), a2);
					int32_t k2 = S16(w, 0xE8);
					S16(d, 2) = (int16_t)add32(p1 >> 8, a1);
					int32_t p2 = mul32(k2, d2);
					n--;
					a += 8;
					b += 8;
					S16(d, 4) = (int16_t)add32(p2 >> 8, a2);
					d += 8;
				} while (n > 0);
			}
			if (fl & 4)
			{
				w = WS();
				U32(w, 0x78) = U32(w, 0x80);
				uint8_t *a = objA + S32(objA, 0x1C);
				uint8_t *b = objB + S32(objB, 0x1C);
				uint8_t *d = PTR(WS(), 0x78);
				int32_t n = S32(objA, 0x20);
				if (n > 0)
				{
					do
					{
						w = WS();
						int16_t a0 = S16(a, 0);
						int32_t t = mul32(sub32(S16(b, 0), a0), S32(w, 0xE0));
						S16(d, 0) = (int16_t)add32(t / 256, a0);
						int16_t a1 = S16(a, 2);
						t = mul32(sub32(S16(b, 2), a1), S32(WS(), 0xE4));
						S16(d, 2) = (int16_t)add32(t / 256, a1);
						int16_t a2 = S16(a, 4);
						t = mul32(sub32(S16(b, 4), a2), S32(WS(), 0xE8));
						S16(d, 4) = (int16_t)add32(t / 256, a2);
						a += 8;
						b += 8;
						d += 8;
					} while (--n != 0);
				}
			}
			w = WS();
		}

		if (clip != nullptr)
		{
			int32_t cf = clip[3];
			if (cf & 0xC)
			{
				uint8_t *mesh = PTR(w, 0x68);
				int32_t lim = clip[1];
				int32_t n = S32(mesh, 0x18);
				uint8_t *d = PTR(w, 0x7C);
				uint8_t *s = PTR(w, 0x74);
				U32(w, 0x74) = U32(w, 0x7C);
				if (cf & 8)
				{
					while (n > 0)
					{
						int16_t y = S16(s, 2);
						if ((int32_t)y > lim) S16(d, 2) = (int16_t)lim;
						else S16(d, 2) = y;
						U16(d, 0) = U16(s, 0);
						s += 8;
						U16(d, 4) = U16(s, -4);
						d += 8;
						n--;
					}
				}
				else
				{
					while (n > 0)
					{
						int16_t y = S16(s, 2);
						if ((int32_t)y < lim) S16(d, 2) = (int16_t)lim;
						else S16(d, 2) = y;
						U16(d, 0) = U16(s, 0);
						s += 8;
						U16(d, 4) = U16(s, -4);
						d += 8;
						n--;
					}
				}
				w = WS();
			}
		}

		if ((U32(w, 0x48) | U32(w, 0x44) | U32(w, 0x40)) != 0)
		{
			FLT_1877DA8() = (float)(DEPTH_SCALE_NUM / (double)S32(w, 0xC0));
			uint32_t *depth = DEPTHARR();
			uint8_t *mesh = PTR(w, 0x68);
			uint8_t *dst = PTR(w, 0x7C);
			uint8_t *v = PTR(w, 0x74);
			int32_t n = S32(mesh, 0x18);
			int32_t sx = S32(w, 0x40);
			int32_t sy = S32(w, 0x44);
			int32_t sz = S32(w, 0x48);
			uint8_t *ir0 = PTR(w, 0x88);
			int32_t bias = S32(w, 0x94);
			int32_t mode = (int32_t)(U32(w, 0x90) & 0x60);
			do
			{
				int32_t vx = S16(v, 0);
				int32_t vy = S16(v, 2);
				int32_t lo = (mul32(vx, sx) >> 12) & 0xFFFF;
				int32_t hi = shl32(mul32(vy, sy) >> 12, 16);
				int32_t xy = lo | hi;
				x::GteWriteData(xy, 0);
				int32_t vz = S16(v, 4);
				int32_t z = mul32(vz, sz) >> 12;
				x::GteWriteData(z, 1);
				x::GteRTPS();
				n--;
				v += 8;
				x::GteReadData2(0xE, dst);
				int32_t loc8;
				x::GteReadData(&loc8, 0x13);
				*depth++ = (uint32_t)loc8;
				x::GteReadData2(8, ir0);
				x::GteReadCtrl(&sx, 0x1F);
				if (mode == 0) loc8 = loc8 >> 2;
				else
				{
					int32_t q = loc8 >> 2;
					if (mode & 0x20) loc8 = bias;
					else loc8 = add32(q, bias);
				}
				sx = shl32(sx, 4);
				S32(dst, 4) = sx;
				U16(dst, 4) = (uint16_t)loc8;
				ir0 += 8;
				dst += 8;
			} while (n > 0);
		}
		else if (U8(CUR(), 0x4C) & 2)
		{
			uint8_t *ir0 = PTR(w, 0x88);
			uint8_t *mesh = PTR(w, 0x68);
			uint8_t *dst = PTR(w, 0x7C);
			uint8_t *v = PTR(w, 0x74);
			int32_t n = S32(mesh, 0x18);
			int32_t bias = S32(w, 0x94);
			int32_t mode = (int32_t)(U32(w, 0x90) & 0x60);
			uint32_t *depth = DEPTHARR();
			do
			{
				int32_t loc8 = S32(v, 0);
				int32_t vz = U16(v, 4);
				x::GteWriteData(loc8, 0);
				x::GteWriteData(vz, 1);
				x::GteRTPS();
				n--;
				v += 8;
				x::GteReadData(&loc8, 0xE);
				x::GteReadData2(8, ir0);
				S32(dst, 0) = loc8;
				uint32_t code = 0;
				if (GTEFLAG() & 0x20000) code = 0x10;
				int32_t scx = (int16_t)loc8;
				if (scx < 0) code |= 8;
				else if (scx >= 0xA00) code |= 2;
				int32_t scy = loc8 >> 16;
				loc8 = scy;
				if (scy < 0) code |= 4;
				else if (scy >= 0x700) code |= 1;
				U16(dst, 6) = (uint16_t)code;
				x::GteReadData(&loc8, 0x13);
				*depth++ = (uint32_t)loc8;
				int32_t dz;
				if (mode == 0) dz = loc8 >> 2;
				else
				{
					dz = loc8 >> 2;
					if (mode & 0x20) dz = bias;
					else dz = add32(dz, bias);
				}
				U16(dst, 4) = (uint16_t)dz;
				dst += 8;
				ir0 += 8;
			} while (n > 0);
		}
		else
		{
			U32(&FLT_1877DA8(), 0) = 0x3F800000u;
			uint8_t *ir0 = PTR(w, 0x88);
			uint8_t *mesh = PTR(w, 0x68);
			uint8_t *dst = PTR(w, 0x7C);
			uint8_t *v = PTR(w, 0x74);
			int32_t n = S32(mesh, 0x18);
			int32_t bias = S32(w, 0x94);
			int32_t mode = (int32_t)(U32(w, 0x90) & 0x60);
			uint32_t *depth = DEPTHARR();
			do
			{
				int32_t loc8 = S32(v, 0);
				int32_t vz = U16(v, 4);
				x::GteWriteData(loc8, 0);
				x::GteWriteData(vz, 1);
				x::GteRTPS();
				n--;
				v += 8;
				x::GteReadData2(0xE, dst);
				x::GteReadData(&loc8, 0x13);
				*depth++ = (uint32_t)loc8;
				x::GteReadData2(8, ir0);
				int32_t flag;
				x::GteReadCtrl(&flag, 0x1F);
				if (mode == 0) loc8 = loc8 >> 2;
				else
				{
					int32_t q = loc8 >> 2;
					if (mode & 0x20) loc8 = bias;
					else loc8 = add32(q, bias);
				}
				dst += 8;
				flag = shl32(flag, 4);
				S32(dst, -4) = flag;
				U16(dst, -4) = (uint16_t)loc8;
				ir0 += 8;
			} while (n > 0);
		}

		if (U8(CUR(), 0xE1) != 0)
		{
			w = WS();
			U32(w, 0xF0) = (U32(w, 0x84) & 0xFF808080u) | 0x808080u;
			uint8_t *wb = WS();
			uint8_t *out = PTR(wb, 0x80);
			x::GteWriteData(S32(wb, 0xF0), 6);
			uint8_t *nrm = PTR(wb, 0x78);
			int32_t n = S32(PTR(wb, 0x68), 0x20);
			do
			{
				x::GteWriteData2(0, S32(nrm, 0));
				x::GteWriteData2(1, S32(nrm, 4));
				nrm += 8;
				x::Gte_4601B0();
				x::GteReadData2(0x16, out);
				n--;
				out += 8;
			} while (n > 0);
		}

		w = WS();
		uint8_t *hdr = PTR(w, 0x6C);
		int32_t cnt = S16(hdr, 2);
		while (cnt != -1)
		{
			S32(w, 0x70) = cnt;
			U32(WS(), 0x6C) = U32(WS(), 0x6C) + 4;
			int32_t type = S16(hdr, 0);
			uint8_t *w2 = WS();
			uint32_t size = MEM<uint8_t>(C().prim_size + (uint32_t)type);
			U32(w2, 0x58) = (uint32_t)mul32((int32_t)size, S32(w2, 0x70)) + U32(w2, 0x6C);
			uint16_t df = U16(CUR(), 0x4C);
			int32_t idx = type;
			if (df & 2) idx += 0x14;
			if (df & 4) idx += 0x28;
			C().prim[idx]();
			U32(WS(), 0x6C) = U32(WS(), 0x58);
			w = WS();
			hdr = PTR(w, 0x6C);
			cnt = S16(hdr, 2);
		}
		U32(CTX(), 0x7C) = U32(w, 0x60);
	}

	static void __cdecl dh_03_SkinnedBattleModel()
	{
		h_B269E0();
		h_B26AD0(PTR(CUR(), 0xBC));
		uint8_t *blk = PTR(CUR(), 0xBC);
		U32(blk, 0x44) = U32(CTX(), 0x74);
		uint32_t cursor = U32(CTX(), 0x7C);
		uint32_t ot = U32(RT(), 0x4C);
		U32(CTX(), 0x7C) = x::RenderGeometry(blk + 0x30, blk + 0x40, ot, 4, cursor);
	}

	static void __cdecl dh_04_ScreenTintQuad()
	{
		int32_t slot;
		if (U8(CUR(), 0xDE) & 0x60) slot = S32(CUR(), 0xC0);
		else slot = 0;
		U32(WS(), 0xF0) = 0x20;
		int32_t r = S16(CUR(), 0x94);
		int32_t g = S16(CUR(), 0x96);
		int32_t b = S16(CUR(), 0x98);
		if (r < 0) { r = -r; U32(WS(), 0xF0) = 0x40; }
		if (r >= 0x100) r = 0xFF;
		if (g < 0) { g = -g; U32(WS(), 0xF0) = 0x40; }
		if (g >= 0x100) g = 0xFF;
		if (b < 0) { b = -b; U32(WS(), 0xF0) = 0x40; }
		if (b >= 0x100) b = 0xFF;
		uint8_t *p = PTR(CTX(), 0x7C);
		uint32_t col = (((uint32_t)b << 8 | (uint32_t)g) << 8) | (uint32_t)r;
		U8(p, 3) = 5;
		if (col == 0) return;
		U32(p, 8) = 0;
		U32(p, 4) = col | 0x2A000000u;
		U32(p, 0xC) = 0x140;
		U32(p, 0x10) = 0x1000000;
		U32(p, 0x14) = 0x1000140;
		x::InsertPrimAltViewport(U32(RT(), 0x4C) + (uint32_t)slot * 4u, p);
		p += 0x18;
		x::SetDrawMode(p, 0, 0, S32(WS(), 0xF0), nullptr);
		x::InsertPrimAltViewport(U32(RT(), 0x4C), p);
		U32(CTX(), 0x7C) = (uint32_t)(p + 0xC);
	}

	static void __cdecl dh_01_MeshAtBonePos()
	{
		S32(WS(), 0x48) = 0;
		S32(WS(), 0x44) = 0;
		S32(WS(), 0x40) = 0;
		U32(WS(), 0x4C) = U32(CUR(), 0xBC);
		SetupParentXform();
		DrawMeshObject(nullptr);
	}

	static void __cdecl dh_02_MeshScaled()
	{
		U32(WS(), 0x4C) = U32(CUR(), 0xBC);
		S32(WS(), 0x48) = 0;
		S32(WS(), 0x44) = 0;
		S32(WS(), 0x40) = 0;
		h_B269E0();
		DrawMeshObject(nullptr);
	}

	static void __cdecl dh_09_MeshYClipped()
	{
		U32(WS(), 0x4C) = U32(CUR(), 0xBC);
		S32(WS(), 0x48) = 0;
		S32(WS(), 0x44) = 0;
		S32(WS(), 0x40) = 0;
		SetupParentXformScaledAtOrigin();
		uint8_t *c = WS() + 0x20;
		U16(c, 0) = U16(CUR(), 0x94);
		U16(c, 2) = U16(CUR(), 0x96);
		U16(c, 4) = U16(CUR(), 0x98);
		U16(c, 6) = U16(CUR(), 0xC2);
		DrawMeshObject((const int16_t *)c);
	}

	static void __cdecl dh_07_MeshCameraSpace()
	{
		x::SetRotMatrix(NODEMAT());
		x::SetTransVector(NODEMAT());
		x::GteLoadV0(CUR() + 0x94);
		x::GteMVMVA_RotV0Tr();
		x::Gte_45E580();
		uint8_t *m = WS() + 0x20;
		x::RotMatrixFromAngles(CUR() + 0x8C, m);
		xl_MatrixMultiplyGte(NODEMAT(), m);
		pad_after_rot_mul(m, CUR() + 0x8C);
		x::SetRotMatrix(m);
		U32(WS(), 0x48) = U32(CUR(), 0xB8);
		U32(WS(), 0x44) = U32(WS(), 0x48);
		U32(WS(), 0x40) = U32(WS(), 0x44);
		U32(WS(), 0x4C) = U32(CUR(), 0xBC);
		DrawMeshObject(nullptr);
	}

	static void __cdecl dh_27_MeshBillboardScaled()
	{
		uint8_t *ang = WS() + 0x90;
		uint8_t *pos = CUR() + 0x94;
		U16(ang, 0) = U16(CUR(), 0x8C);
		uint16_t ay = U16(CUR(), 0x8E);
		U16(ang, 4) = 0;
		U16(ang, 2) = ay;
		int32_t scale = shl32(S16(CUR(), 0x90), 4);
		SetupBillboardXform(pos, ang, scale, 0);
		S32(WS(), 0x48) = 0;
		S32(WS(), 0x44) = 0;
		S32(WS(), 0x40) = 0;
		U32(WS(), 0x4C) = U32(CUR(), 0xBC);
		DrawMeshObject(nullptr);
	}
}

	void h_B269E0()
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

	int32_t h_B26AD0(uint8_t *blk)
	{
		uint8_t *hdr = blk + 0x10;
		uint8_t *cmd = blk + 0x20;
		bool frozen = true;
		if (U8(RT(), 0x45) == 0)
		{
			uint16_t fl = U16(blk, 2);
			if (!(fl & 1))
			{
				frozen = false;
				if (U16(blk, 0) != 0)
				{
					if (fl & 2)
						x::BuildBoneMatricesFromPose(hdr);
					else
					{
						if (x::ReadAnimation(hdr, cmd) == 1) x::PreReadAnimation(hdr, cmd, U8(blk, 9));
						U16(blk, 0) = (uint16_t)(U16(blk, 0) + 1);
					}
				}
				x::ComputeBonesWorldMatrices(hdr, WS() + 0xE0);
			}
		}
		if (frozen)
		{
			x::BuildBoneMatricesFromPose(hdr);
			x::ComputeBonesWorldMatrices(hdr, WS() + 0xE0);
		}
		if (U16(blk, 0) == 0) U16(blk, 0) = 1;
		return 0;
	}

	void h_B27000() { part_draw_mesh::SetupParentXform(); }
	void h_B27130() { part_draw_mesh::SetupParentXformScaledAtOrigin(); }
	void h_B27360(const void *pos, const void *angles, int32_t scale, int32_t order) { part_draw_mesh::SetupBillboardXform(pos, angles, scale, order); }
	void h_B27440(const int16_t *clipArg) { part_draw_mesh::DrawMeshObject(clipArg); }

	void fill_draw_mesh(Generic &g)
	{
		g.draw[1] = part_draw_mesh::dh_01_MeshAtBonePos;
		g.draw[2] = part_draw_mesh::dh_02_MeshScaled;
		g.draw[3] = part_draw_mesh::dh_03_SkinnedBattleModel;
		g.draw[4] = part_draw_mesh::dh_04_ScreenTintQuad;
		g.draw[7] = part_draw_mesh::dh_07_MeshCameraSpace;
		g.draw[9] = part_draw_mesh::dh_09_MeshYClipped;
		g.draw[21] = part_draw_mesh::dh_21_ScrollTextureUpload;
		g.draw[27] = part_draw_mesh::dh_27_MeshBillboardScaled;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace part_draw_prim
{
	static inline int32_t DepthKey(uint32_t off)
	{
		uint32_t v = DEPTHARR()[off >> 3] & 0xFFFF;
		return (int32_t)(int64_t)((double)v * (double)FLT_1877DA8());
	}

	static inline int32_t Div3(int32_t v)
	{
		int32_t hi = (int32_t)(((int64_t)v * (int64_t)0x55555556) >> 32);
		return sub32(hi, v >> 31);
	}

	static void LightColourSetup(int32_t c)
	{
		U32(WS(), 0xE0) = 0x1000;
		U32(WS(), 0xE4) = 0x10000000;
		U32(WS(), 0xE8) = 0;
		U32(WS(), 0xEC) = 0x1000;
		U32(WS(), 0xF0) = 0;
		x::SetLightMatrix(WS() + 0xE0);
		U32(WS(), 0xE0) = (uint32_t)(c & 0xFF) << 5;
		U32(WS(), 0xE4) = (uint32_t)(c & 0xFF00) << 13;
		U32(WS(), 0xEC) = (uint32_t)((c >> 11) & 0x1FE0);
		x::SetColorMatrix(WS() + 0xE0);
		x::SetBackColor(0, 0, 0);
		U32(WS(), 0xE0) = 0x10001000;
		U32(WS(), 0xE4) = 0x1000;
		x::GteLoadV0(WS() + 0xE0);
	}

	static inline int32_t Nclip(uint32_t s0, uint32_t s1, uint32_t s2)
	{
		int32_t mac0;
		x::GteWriteData((int32_t)s0, 12);
		x::GteWriteData((int32_t)s1, 13);
		x::GteWriteData((int32_t)s2, 14);
		x::GteNCLIP();
		x::GteReadData(&mac0, 24);
		return mac0;
	}

	static void __cdecl pr_07_TintedG3()
	{
		LightColourSetup((int32_t)U32(WS(), 0x84));
		uint8_t *ws = WS();
		int32_t count = S32(ws, 0x70);
		uint8_t *pkt = PTR(ws, 0x60);
		uint8_t *rec = PTR(ws, 0x6C);
		uint32_t ot = U32(ws, 0x5C);
		uint32_t nocull = U32(ws, 0x90) & 0x10;
		U32(ws, 0xF0) = 0;
		do
		{
			uint8_t *vb = PTR(ws, 0x7C);
			uint32_t o0 = U16(rec, 0xC);
			uint32_t o1 = U16(rec, 0xE);
			uint32_t o2 = U16(rec, 0x10);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			uint8_t *p1 = vb + o1;
			uint8_t *p0 = vb + o0;
			uint8_t *p2 = vb + o2;
			uint32_t s1 = U32(p1, 0);
			uint32_t s0 = U32(p0, 0);
			uint32_t s2 = U32(p2, 0);
			U32(pkt, 0x18) = s1;
			U32(pkt, 0x10) = s0;
			U32(pkt, 0x20) = s2;
			if (nocull == 0 && Nclip(s0, s1, s2) < 0) goto next;
			{
				uint32_t a0 = U32(p0, 4), a1 = U32(p1, 4), a2 = U32(p2, 4);
				if (((a2 | a1 | a0) >> 16) & 0x46) goto next;
				int32_t sum = (int32_t)((a2 + a1 + a0) & 0xFFFF);
				U8(pkt, 3) = 8;
				U32(pkt, 8) = 0;
				x::GteWriteData((int32_t)(U32(ws, 0x8C) | U32(rec, 0) | 0x30000000), 6);
				uint32_t mode = U32(ws, 0x50);
				x::Gte_4601B0();
				U32(pkt, 4) = mode;
				x::GteReadData2(0x16, pkt + 0xC);
				x::GteWriteData2(6, (int32_t)U32(rec, 4));
				x::Gte_4601B0();
				x::GteReadData2(0x16, pkt + 0x14);
				x::GteWriteData2(6, (int32_t)U32(rec, 8));
				x::Gte_4601B0();
				x::GteReadData2(0x16, pkt + 0x1C);
				uint32_t key = (uint32_t)Div3(sum) & 0x3FFC;
				x::InsertPrimDepthKeys(ot + key, pkt, z0, z1, z2, 0);
				pkt += 0x24;
			}
		next:
			rec += 0x14;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}

	static void __cdecl pr_08_TintedFT3()
	{
		LightColourSetup((int32_t)U32(WS(), 0x84));
		uint8_t *ws = WS();
		int32_t count = S32(ws, 0x70);
		uint32_t ot = U32(ws, 0x5C);
		uint8_t *pkt = PTR(ws, 0x60);
		uint8_t *rec = PTR(ws, 0x6C);
		uint32_t nocull = U32(ws, 0x90) & 0x10;
		U32(ws, 0xF0) = 0;
		do
		{
			uint8_t *vb = PTR(ws, 0x7C);
			uint32_t o1 = U16(rec, 0xC);
			uint32_t o0 = U16(rec, 0xA);
			uint32_t o2 = U16(rec, 0xE);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			uint8_t *p1 = vb + o1;
			uint8_t *p0 = vb + o0;
			uint8_t *p2 = vb + o2;
			uint32_t s0 = U32(p0, 0);
			uint32_t s2 = U32(p2, 0);
			uint32_t s1 = U32(p1, 0);
			U32(pkt, 0x10) = s1;
			U32(pkt, 8) = s0;
			U32(pkt, 0x18) = s2;
			if (nocull == 0 && Nclip(s0, s1, s2) < 0) goto next;
			{
				uint32_t a0 = U32(p0, 4), a1 = U32(p1, 4), a2 = U32(p2, 4);
				if (((a2 | a1 | a0) >> 16) & 0x46) goto next;
				int32_t sum = (int32_t)((a2 + a1 + a0) & 0xFFFF);
				U8(pkt, 3) = 7;
				uint32_t tpage = U32(ws, 0x50) | U16(rec, 0x12);
				int32_t clut_add = S32(ws, 0x54);
				U16(pkt, 0x16) = (uint16_t)tpage;
				U16(pkt, 0xE) = (uint16_t)add32(U16(rec, 0x10), clut_add);
				uint32_t uvofs = U16(ws, 0x98);
				uint32_t uv1 = U16(rec, 6) + uvofs;
				uint32_t uv0 = U16(rec, 4) + uvofs;
				uint32_t uv2 = U16(rec, 8) + uvofs;
				U16(pkt, 0xC) = (uint16_t)uv0;
				U16(pkt, 0x14) = (uint16_t)uv1;
				U16(pkt, 0x1C) = (uint16_t)uv2;
				x::GteWriteData((int32_t)(U32(ws, 0x8C) | U32(rec, 0) | 0x24000000), 6);
				x::Gte_4601B0();
				x::GteReadData2(0x16, pkt + 4);
				uint32_t key = (uint32_t)Div3(sum) & 0x3FFC;
				x::InsertPrimDepthKeys(ot + key, pkt, z0, z1, z2, 0);
				pkt += 0x20;
			}
		next:
			rec += 0x14;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}

	static void __cdecl pr_09_TintedGT3()
	{
		LightColourSetup((int32_t)U32(WS(), 0x84));
		uint8_t *ws = WS();
		int32_t count = S32(ws, 0x70);
		uint32_t ot = U32(ws, 0x5C);
		uint8_t *pkt = PTR(ws, 0x60);
		uint8_t *rec = PTR(ws, 0x6C);
		uint32_t nocull = U32(ws, 0x90) & 0x10;
		U32(ws, 0xF0) = 0;
		do
		{
			uint8_t *vb = PTR(ws, 0x7C);
			uint32_t o1 = U16(rec, 0x14);
			uint32_t o0 = U16(rec, 0x12);
			uint32_t o2 = U16(rec, 0x16);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			uint8_t *p1 = vb + o1;
			uint8_t *p0 = vb + o0;
			uint8_t *p2 = vb + o2;
			uint32_t s0 = U32(p0, 0);
			uint32_t s2 = U32(p2, 0);
			uint32_t s1 = U32(p1, 0);
			U32(pkt, 0x14) = s1;
			U32(pkt, 8) = s0;
			U32(pkt, 0x20) = s2;
			if (nocull == 0 && Nclip(s0, s1, s2) < 0) goto next;
			{
				uint32_t a0 = U32(p0, 4), a1 = U32(p1, 4), a2 = U32(p2, 4);
				if (((a2 | a1 | a0) >> 16) & 0x46) goto next;
				int32_t sum = (int32_t)((a2 + a1 + a0) & 0xFFFF);
				U8(pkt, 3) = 9;
				uint32_t tpage = U32(ws, 0x50) | U16(rec, 0x1A);
				int32_t clut_add = S32(ws, 0x54);
				U16(pkt, 0x1A) = (uint16_t)tpage;
				U16(pkt, 0xE) = (uint16_t)add32(U16(rec, 0x18), clut_add);
				U16(pkt, 0xC) = U16(rec, 0xC);
				U16(pkt, 0x18) = U16(rec, 0xE);
				U16(pkt, 0x24) = U16(rec, 0x10);
				x::GteWriteData((int32_t)(U32(ws, 0x8C) | U32(rec, 0) | 0x34000000), 6);
				x::Gte_4601B0();
				x::GteReadData2(0x16, pkt + 4);
				x::GteWriteData2(6, (int32_t)U32(rec, 4));
				x::Gte_4601B0();
				x::GteReadData2(0x16, pkt + 0x10);
				x::GteWriteData2(6, (int32_t)U32(rec, 8));
				x::Gte_4601B0();
				x::GteReadData2(0x16, pkt + 0x1C);
				uint32_t key = (uint32_t)Div3(sum) & 0x3FFC;
				x::InsertPrimDepthKeys(ot + key, pkt, z0, z1, z2, 0);
				pkt += 0x28;
			}
		next:
			rec += 0x1C;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}

	static void __cdecl pr_12_NormalLitG4()
	{
		U32(WS(), 0x84) = 0x3A000000;
		uint8_t *ws = WS();
		int32_t count = S32(ws, 0x70);
		uint8_t *pkt = PTR(ws, 0x60);
		uint32_t ot = U32(ws, 0x5C);
		uint32_t nocull = U32(ws, 0x90) & 0x10;
		uint8_t *rec = PTR(ws, 0x6C);
		uint8_t *c0 = pkt + 4;
		uint8_t *c3 = pkt + 0x1C;
		uint8_t *c2 = pkt + 0x14;
		U32(ws, 0xF0) = 0;
		uint8_t *c1 = pkt + 0xC;
		do
		{
			uint8_t *vb = PTR(ws, 0x7C);
			uint32_t o0 = U16(rec, 0x14);
			uint32_t o1 = U16(rec, 0x16);
			uint32_t o2 = U16(rec, 0x18);
			uint32_t o3 = U16(rec, 0x1A);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			int32_t z3 = DepthKey(o3);
			uint8_t *p2 = vb + o2;
			uint8_t *p1 = vb + o1;
			uint8_t *p3 = vb + o3;
			uint32_t s1 = U32(p1, 0);
			uint8_t *p0 = vb + o0;
			uint32_t s2 = U32(p2, 0);
			uint32_t s0 = U32(p0, 0);
			uint32_t s3 = U32(p3, 0);
			U32(pkt, 0x10) = s1;
			U32(pkt, 0x20) = s3;
			U32(pkt, 8) = s0;
			U32(pkt, 0x18) = s2;
			if (nocull == 0 && Nclip(s0, s1, s2) < 0) goto next;
			{
				uint32_t a0 = U32(p0, 4), a1 = U32(p1, 4), a2 = U32(p2, 4), a3 = U32(p3, 4);
				if (((a3 | a2 | a1 | a0) >> 16) & 0x46) goto next;
				U8(pkt, 3) = 8;
				uint32_t slot = ot + (((a3 + a2 + a1 + a0) >> 2) & 0x3FFC);
				uint32_t mat = U32(ws, 0x84);
				uint8_t *normals = PTR(ws, 0x78);
				uint8_t *n;
				x::GteWriteData((int32_t)(U32(rec, 0) | mat), 6);
				n = normals + U16(rec, 0x10);
				x::GteWriteData2(0, (int32_t)U32(n, 0));
				x::GteWriteData2(1, (int32_t)U32(n, 4));
				x::Gte_4601B0();
				x::GteReadData2(0x16, c0);
				x::GteWriteData((int32_t)(mat | U32(rec, 4)), 6);
				n = normals + U16(rec, 0x10);
				x::GteWriteData2(0, (int32_t)U32(n, 0));
				x::GteWriteData2(1, (int32_t)U32(n, 4));
				uint32_t col2 = U32(rec, 8);
				x::Gte_4601B0();
				col2 |= mat;
				x::GteReadData2(0x16, c1);
				x::GteWriteData((int32_t)col2, 6);
				n = normals + U16(rec, 0x10);
				x::GteWriteData2(0, (int32_t)U32(n, 0));
				x::GteWriteData2(1, (int32_t)U32(n, 4));
				uint32_t col3 = U32(rec, 0xC);
				x::Gte_4601B0();
				x::GteReadData2(0x16, c2);
				x::GteWriteData((int32_t)(mat | col3), 6);
				n = normals + U16(rec, 0x10);
				x::GteWriteData2(0, (int32_t)U32(n, 0));
				x::GteWriteData2(1, (int32_t)U32(n, 4));
				uint32_t mode = U32(ws, 0x50);
				x::Gte_4601B0();
				x::GteReadData2(0x16, c3);
				x::InsertPrimDepthKeys(slot, pkt, z0, z1, z2, z3);
				c1 += 0x24;
				c3 += 0x24;
				pkt += 0x24;
				c2 += 0x24;
				c0 += 0x24;
				U32(pkt, 0) = 0x2000000;
				U32(c0, 0) = mode;
				U32(pkt, 8) = 0;
				x::InsertPrimDepthKeys(slot, pkt, z0, z1, z2, z3);
				c1 += 0xC;
				pkt += 0xC;
				c2 += 0xC;
				c3 += 0xC;
				c0 += 0xC;
			}
		next:
			rec += 0x1C;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}

	static void __cdecl pr_17_TintedG4()
	{
		LightColourSetup((int32_t)U32(WS(), 0x84));
		uint8_t *ws = WS();
		int32_t count = S32(ws, 0x70);
		uint8_t *pkt = PTR(ws, 0x60);
		uint8_t *rec = PTR(ws, 0x6C);
		uint32_t ot = U32(ws, 0x5C);
		uint32_t nocull = U32(ws, 0x90) & 0x10;
		U32(ws, 0xF0) = 0;
		do
		{
			uint8_t *vb = PTR(ws, 0x7C);
			uint32_t o0 = U16(rec, 0x10);
			uint32_t o1 = U16(rec, 0x12);
			uint32_t o2 = U16(rec, 0x14);
			uint32_t o3 = U16(rec, 0x16);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			int32_t z3 = DepthKey(o3);
			uint8_t *p2 = vb + o2;
			uint8_t *p1 = vb + o1;
			uint8_t *p3 = vb + o3;
			uint32_t s1 = U32(p1, 0);
			uint8_t *p0 = vb + o0;
			uint32_t s2 = U32(p2, 0);
			uint32_t s0 = U32(p0, 0);
			uint32_t s3 = U32(p3, 0);
			U32(pkt, 0x18) = s1;
			U32(pkt, 0x28) = s3;
			U32(pkt, 0x10) = s0;
			U32(pkt, 0x20) = s2;
			if (nocull == 0 && Nclip(s0, s1, s2) < 0) goto next;
			{
				uint32_t a0 = U32(p0, 4), a1 = U32(p1, 4), a2 = U32(p2, 4), a3 = U32(p3, 4);
				if (((a3 | a2 | a1 | a0) >> 16) & 0x46) goto next;
				U8(pkt, 3) = 0xA;
				U32(pkt, 8) = 0;
				uint32_t slot = ot + (((a3 + a2 + a1 + a0) >> 2) & 0x3FFC);
				x::GteWriteData((int32_t)(U32(ws, 0x8C) | U32(rec, 0) | 0x38000000), 6);
				uint32_t mode = U32(ws, 0x50);
				x::Gte_4601B0();
				U32(pkt, 4) = mode;
				x::GteReadData2(0x16, pkt + 0xC);
				x::GteWriteData2(6, (int32_t)U32(rec, 4));
				x::Gte_4601B0();
				x::GteReadData2(0x16, pkt + 0x14);
				x::GteWriteData2(6, (int32_t)U32(rec, 8));
				x::Gte_4601B0();
				x::GteReadData2(0x16, pkt + 0x1C);
				x::GteWriteData2(6, (int32_t)U32(rec, 0xC));
				x::Gte_4601B0();
				x::GteReadData2(0x16, pkt + 0x24);
				x::InsertPrimDepthKeys(slot, pkt, z0, z1, z2, z3);
				pkt += 0x2C;
			}
		next:
			rec += 0x18;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}

	static void __cdecl pr_18_TintedFT4()
	{
		LightColourSetup((int32_t)U32(WS(), 0x84));
		uint8_t *ws = WS();
		int32_t count = S32(ws, 0x70);
		uint8_t *pkt = PTR(ws, 0x60);
		uint8_t *rec = PTR(ws, 0x6C);
		uint32_t ot = U32(ws, 0x5C);
		uint32_t nocull = U32(ws, 0x90) & 0x10;
		U32(ws, 0xF0) = 0;
		do
		{
			uint8_t *vb = PTR(ws, 0x7C);
			uint32_t o0 = U16(rec, 0xC);
			uint32_t o1 = U16(rec, 0xE);
			uint32_t o2 = U16(rec, 0x10);
			uint32_t o3 = U16(rec, 0x12);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			int32_t z3 = DepthKey(o3);
			uint8_t *p2 = vb + o2;
			uint8_t *p1 = vb + o1;
			uint8_t *p3 = vb + o3;
			uint32_t s1 = U32(p1, 0);
			uint8_t *p0 = vb + o0;
			uint32_t s2 = U32(p2, 0);
			uint32_t s0 = U32(p0, 0);
			uint32_t s3 = U32(p3, 0);
			U32(pkt, 0x10) = s1;
			U32(pkt, 0x20) = s3;
			U32(pkt, 8) = s0;
			U32(pkt, 0x18) = s2;
			if (nocull == 0 && Nclip(s0, s1, s2) < 0) goto next;
			{
				uint32_t a0 = U32(p0, 4), a1 = U32(p1, 4), a2 = U32(p2, 4), a3 = U32(p3, 4);
				if (((a3 | a2 | a1 | a0) >> 16) & 0x46) goto next;
				U8(pkt, 3) = 9;
				uint32_t slot = ot + (((a3 + a2 + a1 + a0) >> 2) & 0x3FFC);
				uint32_t tpage = U32(ws, 0x50) | U16(rec, 0x16);
				int32_t clut_add = S32(ws, 0x54);
				U16(pkt, 0x16) = (uint16_t)tpage;
				U16(pkt, 0xE) = (uint16_t)add32(U16(rec, 0x14), clut_add);
				uint32_t uvofs = U16(ws, 0x98);
				uint32_t uv1 = U16(rec, 6) + uvofs;
				uint32_t uv0 = U16(rec, 4) + uvofs;
				uint32_t uv2 = U16(rec, 8) + uvofs;
				uint32_t uv3 = U16(rec, 0xA) + uvofs;
				U16(pkt, 0xC) = (uint16_t)uv0;
				U16(pkt, 0x24) = (uint16_t)uv3;
				U16(pkt, 0x14) = (uint16_t)uv1;
				U16(pkt, 0x1C) = (uint16_t)uv2;
				x::GteWriteData((int32_t)(U32(ws, 0x8C) | U32(rec, 0) | 0x2C000000), 6);
				x::Gte_4601B0();
				x::GteReadData2(0x16, pkt + 4);
				x::InsertPrimDepthKeys(slot, pkt, z0, z1, z2, z3);
				pkt += 0x28;
			}
		next:
			rec += 0x18;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}

	static void __cdecl pr_19_TintedGT4()
	{
		LightColourSetup((int32_t)U32(WS(), 0x84));
		uint8_t *ws = WS();
		int32_t count = S32(ws, 0x70);
		uint8_t *pkt = PTR(ws, 0x60);
		uint8_t *rec = PTR(ws, 0x6C);
		uint32_t ot = U32(ws, 0x5C);
		uint32_t nocull = U32(ws, 0x90) & 0x10;
		U32(ws, 0xF0) = 0;
		do
		{
			uint8_t *vb = PTR(ws, 0x7C);
			uint32_t o0 = U16(rec, 0x18);
			uint32_t o1 = U16(rec, 0x1A);
			uint32_t o2 = U16(rec, 0x1C);
			uint32_t o3 = U16(rec, 0x1E);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			int32_t z3 = DepthKey(o3);
			uint8_t *p2 = vb + o2;
			uint8_t *p1 = vb + o1;
			uint8_t *p3 = vb + o3;
			uint32_t s1 = U32(p1, 0);
			uint8_t *p0 = vb + o0;
			uint32_t s2 = U32(p2, 0);
			uint32_t s0 = U32(p0, 0);
			uint32_t s3 = U32(p3, 0);
			U32(pkt, 0x14) = s1;
			U32(pkt, 0x2C) = s3;
			U32(pkt, 8) = s0;
			U32(pkt, 0x20) = s2;
			if (nocull == 0 && Nclip(s0, s1, s2) < 0) goto next;
			{
				uint32_t a0 = U32(p0, 4), a1 = U32(p1, 4), a2 = U32(p2, 4), a3 = U32(p3, 4);
				if (((a3 | a2 | a1 | a0) >> 16) & 0x46) goto next;
				U8(pkt, 3) = 0xC;
				uint32_t slot = ot + (((a3 + a2 + a1 + a0) >> 2) & 0x3FFC);
				uint32_t tpage = U32(ws, 0x50) | U16(rec, 0x22);
				int32_t clut_add = S32(ws, 0x54);
				U16(pkt, 0x1A) = (uint16_t)tpage;
				U16(pkt, 0xE) = (uint16_t)add32(U16(rec, 0x20), clut_add);
				uint32_t uvofs = U16(ws, 0x98);
				uint32_t uv1 = U16(rec, 0x12) + uvofs;
				uint32_t uv0 = U16(rec, 0x10) + uvofs;
				uint32_t uv2 = U16(rec, 0x14) + uvofs;
				uint32_t uv3 = U16(rec, 0x16) + uvofs;
				U16(pkt, 0xC) = (uint16_t)uv0;
				U16(pkt, 0x30) = (uint16_t)uv3;
				U16(pkt, 0x18) = (uint16_t)uv1;
				U16(pkt, 0x24) = (uint16_t)uv2;
				x::GteWriteData((int32_t)(U32(ws, 0x8C) | U32(rec, 0) | 0x3C000000), 6);
				x::Gte_4601B0();
				x::GteReadData2(0x16, pkt + 4);
				x::GteWriteData2(6, (int32_t)U32(rec, 4));
				x::Gte_4601B0();
				x::GteReadData2(0x16, pkt + 0x10);
				x::GteWriteData2(6, (int32_t)U32(rec, 8));
				x::Gte_4601B0();
				x::GteReadData2(0x16, pkt + 0x1C);
				x::GteWriteData2(6, (int32_t)U32(rec, 0xC));
				x::Gte_4601B0();
				x::GteReadData2(0x16, pkt + 0x28);
				x::InsertPrimDepthKeys(slot, pkt, z0, z1, z2, z3);
				pkt += 0x34;
			}
		next:
			rec += 0x24;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}
}

	void h_B29450(int32_t c) { part_draw_prim::LightColourSetup(c); }

	void fill_draw_prim(Generic &g)
	{
		g.prim[7] = part_draw_prim::pr_07_TintedG3;
		g.prim[8] = part_draw_prim::pr_08_TintedFT3;
		g.prim[9] = part_draw_prim::pr_09_TintedGT3;
		g.prim[12] = part_draw_prim::pr_12_NormalLitG4;
		g.prim[17] = part_draw_prim::pr_17_TintedG4;
		g.prim[18] = part_draw_prim::pr_18_TintedFT4;
		g.prim[19] = part_draw_prim::pr_19_TintedGT4;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace part_draw_sprite
{
	inline void *xl_MatMulGte(const void *a, void *b) { return x::f<void *(__cdecl *)(const void *, void *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }

	inline int32_t GteSZ(int i) { return MEM<int32_t>(game_addr(game::GTE_SZ0) + 4 * i); }

	inline Mat4x3 *BillboardOfCurBone() { return BBMAT() + (U8(CUR(), 0xDE) & 3); }

	static void __cdecl so_01_Billboard()
	{
		x::SetRotMatrix(BillboardOfCurBone());
	}

	static void __cdecl so_02_BillboardScaled()
	{
		uint8_t *bb = (uint8_t *)BillboardOfCurBone();
		uint8_t *m = WS() + 0xD0;
		for (int i = 0; i < 20; i += 4) U32(m, i) = U32(bb, i);
		int32_t s = shl32(S32(CUR(), 0xB8), 4);
		S32(WS(), 0xF8) = s;
		S32(WS(), 0xF4) = s;
		S32(WS(), 0xF0) = s;
		x::ScaleMatrix(m, WS() + 0xF0);
		x::SetRotMatrix(m);
	}

	static void __cdecl so_07_ParentTiltedScaled()
	{
		uint8_t *a = WS() + 0xC0;
		Mat4x3 *par = blob::GetParentMatrix(U16(CUR(), 0x9C));
		uint8_t *sc = a + 0x10;
		S32(sc, 0) = shl32(S16(CUR(), 0x8C), 4);
		S32(a, 0x14) = shl32(S16(CUR(), 0x8E), 4);
		S32(a, 0x18) = 0x1000;
		U16(a, 0) = 0x400;
		U16(a, 2) = 0;
		U16(a, 4) = U16(CUR(), 0x90);
		uint8_t *m = WS() + 0xE0;
		x::RotMatrixFromAngles(a, m);
		xl_MatMulGte(par, m);
		pad_after_rot_mul(m, a);
		x::ScaleMatrix(m, sc);
		x::SetRotMatrix(m);
	}

	static void __cdecl so_08_ParentRotatedScaled()
	{
		uint8_t *a = WS() + 0x20;
		Mat4x3 *par = blob::GetParentMatrix(U16(CUR(), 0x9C));
		uint8_t *sc = a + 0x10;
		S32(sc, 0) = shl32(S16(CUR(), 0x8C), 4);
		S32(a, 0x14) = shl32(S16(CUR(), 0x8C), 4);
		S32(a, 0x18) = 0x1000;
		U16(a, 0) = 0;
		U16(a, 2) = U16(CUR(), 0x8E);
		U16(a, 4) = U16(CUR(), 0x90);
		Mat4x3 *m = blob::RotMatrixOrder(a, 1);
		xl_MatMulGte(par, m);
		x::ScaleMatrix(m, sc);
		x::SetRotMatrix(m);
	}

	static void __cdecl so_09_BillboardRolledScaled()
	{
		uint8_t *a = WS() + 0xC0;
		uint8_t *sc = a + 0x10;
		S32(sc, 0) = shl32(S16(CUR(), 0x8C), 4);
		S32(a, 0x14) = shl32(S16(CUR(), 0x8E), 4);
		S32(a, 0x18) = 0x1000;
		U16(a, 0) = 0;
		U16(a, 2) = 0;
		U16(a, 4) = U16(CUR(), 0x90);
		Mat4x3 *bb = BillboardOfCurBone();
		uint8_t *m = WS() + 0xE0;
		x::RotMatrixFromAngles(a, m);
		xl_MatMulGte(bb, m);
		pad_after_rot_mul(m, a);
		x::ScaleMatrix(m, sc);
		x::SetRotMatrix(m);
	}

	static int32_t EmitSpriteQuads()
	{
		uint8_t *pkt = PTR(CTX(), 0x7C);
		uint8_t *v = WS() + 0xE0;
		uint8_t *list = PTR(WS(), 0x90);
		uint32_t count = U32(list, 0);
		uint8_t *rec = list + 4;
		U16(v, 0xC) = 0;
		U16(v, 4) = 0;
		U16(v, 0x1C) = 0;
		U16(v, 0x14) = 0;
		if (count == 0)
		{
			PTR(CTX(), 0x7C) = pkt;
			return 0;
		}
		do
		{
			uint32_t bright = U8(rec, 0x10);
			if (bright == 0x80)
				U32(pkt, 4) = U32(WS(), 0x98) | 0x2C000000;
			else if (U8(CUR(), 0xDE) & 8)
				U32(pkt, 4) = U32(WS(), 0x98) | 0x2C000000;
			else
			{
				uint32_t abr = U32(CUR(), 0xCC) & 0x2000000;
				uint32_t c = bright | 0x2C00;
				c <<= 8;
				c |= bright;
				c <<= 8;
				c |= abr;
				c |= bright;
				U32(pkt, 4) = c;
			}
			U16(pkt, 0xE) = U16(rec, 0xE);
			uint32_t wh = U16(rec, 0xC);
			uint32_t uv = U16(rec, 0xA);
			uint32_t w = wh & 0xFF;
			uint32_t h = wh & 0xFF00;
			U16(pkt, 0xC) = (uint16_t)uv;
			U16(pkt, 0x14) = (uint16_t)(w + uv);
			U16(pkt, 0x1C) = (uint16_t)(h + uv);
			U16(pkt, 0x24) = (uint16_t)(h + w + uv);
			int32_t w16 = (int32_t)(w << 4);
			int32_t W = mul32(S16(rec, 4), w16) / 4096;
			int32_t x0 = sub32(w16 / 2, W / 2);
			x0 = add32(x0, S16(rec, 0));
			S16(v, 0x10) = (int16_t)x0;
			S16(v, 0) = (int16_t)x0;
			int32_t x1 = add32(x0, W);
			S16(v, 0x18) = (int16_t)x1;
			S16(v, 8) = (int16_t)x1;
			int32_t h16 = (int32_t)h >> 4;
			h16 &= (int32_t)0xFFFFFFF0;
			int32_t H = mul32(S16(rec, 6), h16) / 4096;
			int32_t y0 = sub32(S16(rec, 2), H / 2);
			y0 = add32(y0, h16 / 2);
			int32_t y1 = add32(H, y0);
			U16(v, 0x1C) = 0;
			U16(v, 0x14) = 0;
			U16(v, 0xC) = 0;
			U16(v, 4) = 0;
			S16(v, 0xA) = (int16_t)y0;
			S16(v, 2) = (int16_t)y0;
			S16(v, 0x1A) = (int16_t)y1;
			S16(v, 0x12) = (int16_t)y1;
			x::GteLoadV012(v, v + 8, v + 0x10);
			x::GteRTPT();
			x::GteReadSXY012(pkt + 8, pkt + 0x10, pkt + 0x18);
			x::GteAVSZ3();
			x::GteReadOTZ(WS() + 0xE0);
			x::GteLoadV0(v + 0x18);
			x::GteRTPS();
			x::GteReadSXY2(pkt + 0x20);
			x::GteReadOTZdiv4(WS() + 0xE4);
			int32_t sz3 = GteSZ(3), sz2 = GteSZ(2), sz1 = GteSZ(1), sz0 = GteSZ(0);
			int32_t z = add32(S32(WS(), 0xE4), S32(WS(), 0xE0)) / 2;
			uint32_t ot = U32(RT(), 0x4C) + ((uint32_t)(z >> 2) << 2);
			x::InsertPrimDepthKeys(ot, pkt, sz0, sz1, sz2, sz3);
			uint16_t tp;
			if (U8(CUR(), 0xDE) & 4) tp = U16(WS(), 0x94);
			else tp = U16(rec, 0x12);
			U16(pkt, 0x16) = tp;
			uint32_t size;
			if ((tp & 0x60) == 0x20)
			{
				U8(pkt, 3) = 9;
				size = 0x28;
			}
			else
			{
				U8(pkt, 3) = 0xA;
				U32(pkt, 0x28) = 0xE1000220;
				size = 0x2C;
			}
			pkt += size;
			rec += 0x14;
		} while (--count != 0);
		PTR(CTX(), 0x7C) = pkt;
		return 0;
	}

	static void __cdecl dh_05_AnimatedSprite()
	{
		uint8_t skip = U8(RT(), 0x45);
		uint8_t *b = CUR();
		if (skip == 0)
		{
			int32_t step = S16(b, 0xCA);
			if (step >= 0)
			{
				S16(b, 0xDC) = (int16_t)(S16(b, 0xDC) - step);
				b = CUR();
				if (S16(b, 0xDC) <= 0)
				{
					uint8_t *fr = PTR(b, 0xD0);
					U32(b, 0xD8) = U32(b, 0xE4) + U32(fr, 0);
					b = CUR();
					S16(b, 0xDC) = (int16_t)(S16(b, 0xDC) + U16(fr, 4));
					uint8_t *cb = CUR();
					int32_t ctl = S8(fr, 6);
					uint32_t next = (uint32_t)fr;
					if (ctl != 0)
					{
						if (ctl == 1)
						{
							U8(cb, 0x1C) = 0;
							blob::DrawListRemove(U8(RT(), 0x42));
							S32(WS(), 0) = sub32(S32(WS(), 0), 1);
							return;
						}
						if (ctl == 2) next = U32(cb, 0xD4) - 8;
					}
					U32(cb, 0xD0) = next + 8;
					b = CUR();
				}
			}
		}
		Mat4x3 *m = blob::GetParentMatrix(U16(b, 0x9C));
		x::SetRotMatrix(m);
		x::SetTransVector(m);
		x::GteLoadV0(CUR() + 0x94);
		x::GteMVMVA_RotV0Tr();
		x::Gte_45E580();
		C().spr[U8(CUR(), 0x1E)]();
		U32(WS(), 0x90) = U32(CUR(), 0xD8);
		S32(WS(), 0x94) = S16(CUR(), 0x92);
		U32(WS(), 0x98) = U32(CUR(), 0xCC);
		S32(WS(), 0x9C) = S16(CUR(), 0x9A);
		EmitSpriteQuads();
	}

	static void __cdecl pt_1_Cartesian()
	{
		uint8_t *w = WS();
		uint8_t *p = PTR(w, 0x44);
		S16(w, 0xA0) = (int16_t)((S32(p, 0x14) >> 8) - U16(w, 0x38));
		S16(WS(), 0xA2) = (int16_t)(S32(p, 0x18) >> 8);
		w = WS();
		S16(w, 0xA4) = (int16_t)((S32(p, 0x1C) >> 8) - U16(w, 0x3C));
	}

	static void __cdecl pt_2_Cylindrical()
	{
		uint8_t *p = PTR(WS(), 0x44);
		int32_t ang = (S32(p, 0x1C) >> 4) & 0xFFF;
		int32_t r = S32(p, 0x14) >> 4;
		int32_t c = x::Cos(ang);
		c = mul32(c, r) >> 16;
		int32_t s = x::Sin(ang);
		s = mul32(s, r) >> 16;
		uint8_t *w = WS();
		S16(w, 0xA0) = (int16_t)((uint16_t)(U16(p, 0x42) - U16(w, 0x38)) + s);
		S16(WS(), 0xA2) = (int16_t)((S32(p, 0x18) >> 8) + U16(p, 0x44));
		w = WS();
		S16(w, 0xA4) = (int16_t)((uint16_t)(U16(p, 0x46) - U16(w, 0x3C)) + c);
	}

	static void __cdecl pt_3_Ellipsoid()
	{
		uint8_t *p = PTR(WS(), 0x44);
		uint8_t *bn = blob::GetBone(S16(CUR(), 0xB6));
		int32_t a1 = (S32(p, 0x18) >> 4) & 0xFFF;
		int32_t t = x::Sin(a1);
		t = mul32(t, S16(bn, 0x98)) >> 12;
		S16(WS(), 0xA4) = (int16_t)(U16(p, 0x46) + t);
		int32_t R = S16(bn, 0x98);
		int32_t cr = x::Cos(a1);
		cr = mul32(cr, R);
		int32_t A = mul32(S16(bn, 0x94), cr) / R;
		A >>= 4;
		int32_t a2 = (S32(p, 0x1C) >> 4) & 0xFFF;
		int32_t xx = x::Cos(a2);
		xx = mul32(xx, A) >> 20;
		uint8_t *w = WS();
		S16(w, 0xA0) = (int16_t)((uint16_t)(U16(p, 0x42) - U16(w, 0x38)) + xx);
		int32_t B = mul32(S16(bn, 0x96), cr) / S16(bn, 0x98);
		B >>= 4;
		int32_t yy = x::Sin(a2);
		yy = mul32(yy, B) >> 20;
		w = WS();
		S16(w, 0xA2) = (int16_t)((uint16_t)(U16(p, 0x44) - U16(w, 0x3C)) + yy);
	}

	static void __cdecl po_00_End()
	{
		uint8_t *p = PTR(WS(), 0x44);
		U16(p, 0) = 0;
		U16(RT(), 0x3E) = U16(WS(), 0x24);
		STREAM() = nullptr;
		U32(p, 0x30) = 0;
		U32(p, 8) = 0;
	}

	static void __cdecl po_01_Wait()
	{
		uint8_t *rt = RT();
		U16(rt, 0x3E) = (uint16_t)((uint32_t)U8(rt, 0x4B) << 7);
		STREAM() += 2;
	}

	static void __cdecl po_02_Jump()
	{
		uint8_t *s = STREAM();
		STREAM() = s + S16(s, 2);
	}

	static void __cdecl po_03_AddRandomWait()
	{
		uint8_t *p = PTR(WS(), 0x44);
		int32_t r = blob::Rand73(U16(RT(), 0x4A) >> 8);
		U16(p, 2) = (uint16_t)(U16(p, 2) + (uint16_t)shl32(r, 7));
		STREAM() += 2;
	}

	static inline void SelVector(uint32_t op, uint8_t *p, uint8_t *&dst, int &esz, int &sh)
	{
		uint32_t sel = op & 0x300;
		if (sel == 0) { dst = p + 0x10; esz = 4; sh = 8; }
		else { dst = sel == 0x100 ? p + 0x20 : p + 0x28; esz = 2; sh = 0; }
	}

	static void __cdecl po_04_SetVector()
	{
		uint32_t mask = U16(RT(), 0x4A);
		uint8_t *p = PTR(WS(), 0x44);
		uint8_t *dst; int esz, sh;
		SelVector(mask, p, dst, esz, sh);
		uint8_t *s = STREAM() + 2;
		for (int off = 0; off < esz * 4; off += esz)
		{
			if ((int16_t)mask < 0)
			{
				int32_t v = S16(s, 0);
				if (v != 0x7654)
				{
					if (esz == 2) S16(dst, off) = (int16_t)shl32(v, sh);
					else S32(dst, off) = shl32(v, sh);
				}
				s += 2;
			}
			mask <<= 1;
		}
		STREAM() = s;
	}

	static void __cdecl po_05_AddRandomVector()
	{
		uint32_t mask = U16(RT(), 0x4A);
		uint8_t *p = PTR(WS(), 0x44);
		uint8_t *dst; int esz, sh;
		SelVector(mask, p, dst, esz, sh);
		uint8_t *s = STREAM() + 2;
		for (int off = 0; off < esz * 4; off += esz)
		{
			if ((int16_t)mask < 0)
			{
				int32_t v = S16(s, 0);
				if (v != 0)
				{
					int32_t r = blob::Rand73(v);
					if (esz == 2) S16(dst, off) = (int16_t)(S16(dst, off) + (int16_t)shl32(r, sh));
					else S32(dst, off) = add32(S32(dst, off), shl32(r, sh));
				}
				s += 2;
			}
			mask <<= 1;
		}
		STREAM() = s;
	}

	static void __cdecl po_06_AddPosition()
	{
		uint8_t *d = PTR(WS(), 0x44) + 0x10;
		uint16_t mask = U16(RT(), 0x4A);
		uint8_t *s = STREAM() + 2;
		for (int off = 0; off < 0x10; off += 4)
		{
			if ((int16_t)mask < 0)
			{
				int32_t v = S16(s, 0);
				if (v != 0x7654) S32(d, off) = add32(S32(d, off), shl32(v, 8));
				s += 2;
			}
			mask = (uint16_t)(mask << 1);
		}
		STREAM() = s;
	}

	static void __cdecl po_07_SetFrameList()
	{
		uint8_t *p = PTR(WS(), 0x44);
		uint32_t op = U16(RT(), 0x4A);
		U8(p, 0x4A) = (uint8_t)((uint8_t)op - 7);
		uint8_t *fl = blob::ObjectPtrWs((int32_t)op >> 8);
		uint32_t base = U32(WS(), 0xFC);
		PTR(p, 8) = fl;
		U32(p, 0x4C) = base;
		PTR(p, 0xC) = fl;
		U32(p, 0x30) = U32(WS(), 0xFC) + U32(fl, 0);
		U16(p, 0x34) = U16(CUR(), 0xCA);
		STREAM() += 2;
	}

	static void __cdecl po_09_SetOriginHere()
	{
		uint8_t *p = PTR(WS(), 0x44);
		int32_t t = S8(p, 0x37);
		U16(p, 0x42) = 0;
		U16(p, 0x44) = 0;
		U16(p, 0x46) = 0;
		C().ptype[t]();
		uint8_t *w = WS();
		U16(p, 0x42) = (uint16_t)(U16(w, 0xA0) + U16(w, 0x38));
		U16(p, 0x44) = U16(WS(), 0xA2);
		w = WS();
		U16(p, 0x46) = (uint16_t)(U16(w, 0xA4) + U16(w, 0x3C));
		STREAM() += 2;
	}

	static void __cdecl po_10_TypeCylindrical()
	{
		U8(PTR(WS(), 0x44), 0x37) = 2;
		STREAM() += 2;
	}

	static void __cdecl po_11_SetColor()
	{
		uint8_t *p = PTR(WS(), 0x44);
		U8(p, 0x38) = U8(RT(), 0x4B);
		uint32_t v = U16(STREAM(), 2);
		U8(p, 0x39) = (uint8_t)v;
		U8(p, 0x3A) = (uint8_t)(v >> 8);
		STREAM() += 4;
	}

	static void __cdecl po_12_SetColorDelta()
	{
		uint8_t *p = PTR(WS(), 0x44);
		U8(p, 0x3C) = U8(RT(), 0x4B);
		uint32_t v = U16(STREAM(), 2);
		U8(p, 0x3D) = (uint8_t)v;
		U8(p, 0x3E) = (uint8_t)(v >> 8);
		STREAM() += 4;
	}

	static void __cdecl po_13_TypeEllipsoid()
	{
		U8(PTR(WS(), 0x44), 0x37) = 3;
		U16(CUR(), 0xB6) = U16(STREAM(), 2);
		STREAM() += 4;
	}

	static void __cdecl po_14_AimAtBone()
	{
		uint8_t *p = PTR(WS(), 0x44);
		uint8_t *bn = blob::GetBone(U16(STREAM(), 2));
		int32_t tz = shl32(S16(bn, 0x98), 8);
		int32_t tx = shl32(S16(bn, 0x94), 8);
		int32_t r = blob::Blob_B66B30(S32(p, 0x14), S32(p, 0x1C), tx, tz);
		S32(p, 0x1C) = shl32(r, 4);
		STREAM() += 4;
	}

	static void __cdecl po_15_RandomBranch()
	{
		int32_t r = blob::Rand73(0x100);
		if (r < 0) r = (int32_t)(0u - (uint32_t)r);
		uint8_t *s = STREAM();
		if (r > S16(s, 2)) STREAM() = s + 6;
		else STREAM() = s + S16(s, 4);
	}

	static void __cdecl po_16_SetFrame()
	{
		uint8_t *p = PTR(WS(), 0x44);
		uint32_t v = (uint32_t)U16(RT(), 0x4A) >> 8;
		if (v & 0x80) v = (uint32_t)blob::Rand74((int32_t)(v & 0x7F));
		uint32_t f = U32(p, 0xC) + v * 8;
		U32(p, 8) = f;
		U32(p, 0x30) = U32(p, 0x4C) + U32((void *)f, 0);
		U16(p, 0x34) = U16((void *)f, 4);
		STREAM() += 2;
	}

	static void __cdecl po_17_HoldFrame()
	{
		U16(PTR(WS(), 0x44), 0x34) = 0x7F80;
		STREAM() += 2;
	}

	static void __cdecl po_18_RandomizeInBone()
	{
		uint8_t *bn = blob::GetBone(S16(STREAM(), 2));
		uint8_t *ext = bn + 0x94;
		uint16_t mask = (uint16_t)(U16(RT(), 0x4A) << 1);
		uint8_t *q = PTR(WS(), 0x44) + 0x14;
		for (int k = 3; k != 0; k--)
		{
			if ((int16_t)mask < 0)
			{
				int32_t v = S16(ext, 0);
				if (v != 0)
				{
					int32_t r = blob::Rand73(v);
					S32(q, 0) = add32(S32(q, 0), shl32(r, 8));
				}
			}
			q += 4;
			ext += 2;
			mask = (uint16_t)(mask << 1);
		}
		STREAM() += 4;
	}

	static void __cdecl po_19_SetTexture()
	{
		uint32_t id = (uint32_t)U16(RT(), 0x4A) >> 8;
		uint8_t *p = PTR(WS(), 0x44);
		blob::TexInfo_B665C0((int32_t)id);
		U16(p, 0x40) = U16(WS(), 0xFC);
		U16(p, 0x48) = (uint16_t)blob::ClutWord_B66640((int32_t)id);
		STREAM() += 2;
	}

	static void __cdecl po_20_CopyFromBone()
	{
		uint8_t *p = PTR(WS(), 0x44);
		uint8_t *bn = blob::GetBone(S16(STREAM(), 2));
		uint16_t mask = U16(RT(), 0x4A);
		uint8_t *dst, *src;
		if (mask & 0x8000) { dst = p + 0x10; src = bn + 0x8C; }
		else { dst = p + 0x14; src = bn + 0x94; mask = (uint16_t)(mask << 1); }
		for (int off = 0; off < 0x10; off += 4)
		{
			if ((int16_t)mask < 0) S32(dst, off) = shl32(S16(src, 0), 8);
			src += 2;
			mask = (uint16_t)(mask << 1);
		}
		STREAM() += 4;
	}

	static void __cdecl po_21_CopyVelFromBone()
	{
		uint8_t *q = PTR(WS(), 0x44) + 0x22;
		uint8_t *bn = blob::GetBone(S16(STREAM(), 2));
		uint8_t *src = bn + 0x94;
		uint16_t mask = (uint16_t)(U16(RT(), 0x4A) << 1);
		for (int k = 3; k != 0; k--)
		{
			if ((int16_t)mask < 0) U16(q, 0) = U16(src, 0);
			mask = (uint16_t)(mask << 1);
			q += 2;
			src += 2;
		}
		STREAM() += 4;
	}

	static void __cdecl dh_06_ParticleSystem()
	{
		U16(RT(), 0x3E) = 0;
		S32(WS(), 0x38) = 0;
		S32(WS(), 0x3C) = 0;
		S32(WS(), 0x24) = S16(CUR(), 0xC8);
		S32(WS(), 0x28) = S16(CUR(), 0xCA);
		Mat4x3 *par = blob::GetParentMatrix(U16(CUR(), 0x9C));
		PTR(WS(), 0x30) = (uint8_t *)par;
		uint8_t *pool = PTR(CUR(), 0xB8);
		uint8_t *p = pool + 0x10;
		PTR(WS(), 0x44) = p;
		S32(WS(), 0x20) = U16(pool, 0);
		S32(WS(), 0x34) = U16(pool, 8);
		if (S32(WS(), 0x20) <= 0) return;
		do
		{
			S32(WS(), 0x40) = 0;
			uint8_t *scr = PTR(p, 4);
			if (scr == nullptr) goto next;
			{
				STREAM() = scr;
				if (U8(RT(), 0x45) == 0)
				{
					uint32_t wait = U16(p, 2);
					if ((int32_t)wait > 0)
					{
						U16(p, 2) = (uint16_t)(wait - (uint32_t)S32(WS(), 0x24));
					}
					else
					{
						uint8_t *rt = RT();
						int32_t op = S16(scr, 0);
						S16(rt, 0x4A) = (int16_t)op;
						C().pop[op & 0xFF]();
						while (U16(RT(), 0x3E) == 0)
						{
							rt = RT();
							op = S16(STREAM(), 0);
							S16(rt, 0x4A) = (int16_t)op;
							C().pop[op & 0xFF]();
						}
						rt = RT();
						uint16_t d = (uint16_t)(U16(rt, 0x3E) - (uint16_t)S32(WS(), 0x24));
						U16(p, 2) = (uint16_t)(U16(p, 2) + d);
						U16(RT(), 0x3E) = 0;
						PTR(p, 4) = STREAM();
					}
					for (int k = 0; k < 4; k++)
					{
						int32_t s = S16(p, 0x28 + 2 * k) + S16(p, 0x20 + 2 * k);
						S16(p, 0x20 + 2 * k) = (int16_t)s;
						S32(p, 0x10 + 4 * k) = add32(S32(p, 0x10 + 4 * k), shl32(s, 4));
					}
					C().ptype[S8(p, 0x37)]();
					for (int k = 0; k < 3; k++)
					{
						int32_t c = S8(p, 0x3C + k) + U8(p, 0x38 + k);
						if (c >= 0x100) c = 0xFF;
						else if (c <= 0) c = 0;
						U8(p, 0x38 + k) = (uint8_t)c;
					}
					if (U32(p, 8) != 0)
					{
						int32_t t = S16(p, 0x34);
						bool step = true;
						if (t > 0)
						{
							t = sub32(t, S32(WS(), 0x28));
							S16(p, 0x34) = (int16_t)t;
							if (t > 0) step = false;
						}
						if (step)
						{
							uint32_t f = U32(p, 8);
							U32(p, 0x30) = U32(p, 0x4C) + U32((void *)f, 0);
							U16(p, 0x34) = (uint16_t)(U16(p, 0x34) + U16((void *)f, 4));
							int32_t ctl = S8((void *)f, 6);
							if (ctl == 1)
							{
								U32(p, 4) = 0;
								U32(p, 0x30) = 0;
								U32(p, 8) = f;
							}
							else
							{
								if (ctl == 2) f = U32(p, 0xC) - 8;
								U32(p, 8) = f + 8;
							}
						}
					}
				}
				else
				{
					if (U32(p, 0x30) == 0) goto next;
					C().ptype[S8(p, 0x37)]();
				}
				if (U32(p, 0x30) == 0) goto next;
				{
					uint8_t *m = PTR(WS(), 0x30);
					x::SetRotMatrix(m);
					x::SetTransVector(m);
					x::GteLoadV0(WS() + 0xA0);
					x::GteMVMVA_RotV0Tr();
					x::Gte_45E580();
					int32_t sc = S32(p, 0x10) >> 4;
					S32(WS(), 0xA8) = sc;
					S32(WS(), 0xA0) = sc;
					S32(WS(), 0xB0) = 0;
					S32(WS(), 0xAC) = 0;
					S32(WS(), 0xA4) = 0;
					x::SetRotMatrix(WS() + 0xA0);
					if (S32(WS(), 0x40) != 0) x::Matrix_56C2C0(BBMAT() + 1);
					U32(WS(), 0x90) = U32(p, 0x30);
					U32(WS(), 0x94) = U16(p, 0x40);
					U32(WS(), 0x98) = U32(p, 0x38);
					S32(WS(), 0x9C) = S16(p, 0x48);
					EmitSpriteQuads();
				}
			}
		next:
			p += 0x50;
			PTR(WS(), 0x44) = p;
			S32(WS(), 0x20) = sub32(S32(WS(), 0x20), 1);
		} while (S32(WS(), 0x20) > 0);
	}
}

	void fill_draw_sprite(Generic &g)
	{
		g.spr[1] = part_draw_sprite::so_01_Billboard;
		g.spr[2] = part_draw_sprite::so_02_BillboardScaled;
		g.spr[7] = part_draw_sprite::so_07_ParentTiltedScaled;
		g.spr[8] = part_draw_sprite::so_08_ParentRotatedScaled;
		g.spr[9] = part_draw_sprite::so_09_BillboardRolledScaled;
		g.draw[5] = part_draw_sprite::dh_05_AnimatedSprite;
		g.draw[6] = part_draw_sprite::dh_06_ParticleSystem;
		g.ptype[1] = part_draw_sprite::pt_1_Cartesian;
		g.ptype[2] = part_draw_sprite::pt_2_Cylindrical;
		g.ptype[3] = part_draw_sprite::pt_3_Ellipsoid;
		g.pop[0] = part_draw_sprite::po_00_End;
		g.pop[1] = part_draw_sprite::po_01_Wait;
		g.pop[2] = part_draw_sprite::po_02_Jump;
		g.pop[3] = part_draw_sprite::po_03_AddRandomWait;
		g.pop[4] = part_draw_sprite::po_04_SetVector;
		g.pop[5] = part_draw_sprite::po_05_AddRandomVector;
		g.pop[6] = part_draw_sprite::po_06_AddPosition;
		g.pop[7] = part_draw_sprite::po_07_SetFrameList;
		g.pop[8] = part_draw_sprite::po_07_SetFrameList;
		g.pop[9] = part_draw_sprite::po_09_SetOriginHere;
		g.pop[10] = part_draw_sprite::po_10_TypeCylindrical;
		g.pop[11] = part_draw_sprite::po_11_SetColor;
		g.pop[12] = part_draw_sprite::po_12_SetColorDelta;
		g.pop[13] = part_draw_sprite::po_13_TypeEllipsoid;
		g.pop[14] = part_draw_sprite::po_14_AimAtBone;
		g.pop[15] = part_draw_sprite::po_15_RandomBranch;
		g.pop[16] = part_draw_sprite::po_16_SetFrame;
		g.pop[17] = part_draw_sprite::po_17_HoldFrame;
		g.pop[18] = part_draw_sprite::po_18_RandomizeInBone;
		g.pop[19] = part_draw_sprite::po_19_SetTexture;
		g.pop[20] = part_draw_sprite::po_20_CopyFromBone;
		g.pop[21] = part_draw_sprite::po_21_CopyVelFromBone;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace part_shared_ribbon
{
	static inline int32_t xl_ISqrt(int32_t v) { return x::f<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	static inline void xl_RotTrans(const void *v, void *out, void *flag) { x::f<void (__cdecl *)(const void *, void *, void *)>(game_addr(game::GTE_RotTrans))(v, out, flag); }
	static inline int32_t xl_RotTransPers(const void *v, void *sxy, void *p, void *flag) { return x::f<int32_t (__cdecl *)(const void *, void *, void *, void *)>(game_addr(game::TransformWorldCoordinateToProjectedSpace))(v, sxy, p, flag); }
	static inline void xl_GteSetIR0(int32_t v) { x::f<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetIR0))(v); }
	static inline void xl_GteLoadRGB(const void *src) { x::f<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIR123FromBytes))(src); }
	static inline void xl_GteGPF() { x::f<void (__cdecl *)()>(game_addr(game::GTE_GPF))(); }
	static inline void xl_GteStoreRGB(void *dst) { x::f<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123LowBytes))(dst); }
	static inline int32_t xl_LookAt_B66E50(const void *a, const void *b, void *out) { return x::f<int32_t (__cdecl *)(const void *, const void *, void *)>(game_addr(game::sub_B66E50))(a, b, out); }

	static inline uint8_t *ZEROVEC() { return (uint8_t *)game_addr(game::unk_27971E4); }
	static inline uint16_t &GTE_SZ3_W() { return MEM<uint16_t>(game_addr(game::GTE_SZ3)); }

	static int32_t Ribbon_B1AF60_TurnToward(int32_t a, int32_t t, int32_t rate)
	{
		int32_t eax = a & 0xFFF;
		int32_t ecx = t & 0xFFF;
		int32_t edx = ecx;
		if (eax > ecx) edx = ecx + 0x1000;
		edx = edx - eax;
		if (edx < 0x800)
		{
			eax = add32(eax, rate) & 0xFFF;
			if (eax > ecx) ecx += 0x1000;
			edx = ecx - eax;
			if (edx < 0x800) return eax & 0xFFF;
			eax = ecx;
			return eax & 0xFFF;
		}
		eax = sub32(eax, rate) & 0xFFF;
		if (eax > ecx) ecx += 0x1000;
		edx = ecx - eax;
		if (edx < 0x800) eax = ecx;
		return eax & 0xFFF;
	}

	static int32_t Ribbon_B1AE60_SteerToward(const uint8_t *target, const uint8_t *prev, uint8_t *nw)
	{
		int32_t py = S16(prev, 2);
		int32_t pz = S16(prev, 4);
		int32_t ty = S16(target, 2);
		int32_t tx = S16(target, 0);
		int32_t px = S16(prev, 0);
		int32_t tz = S16(target, 4);
		int32_t yaw = blob::Blob_B66B30(px, pz, tx, tz);
		U16(nw, 6) = (uint16_t)Ribbon_B1AF60_TurnToward(S16(prev, 6), yaw, S16(CUR(), 0x8E));
		int32_t pitch = blob::Blob_B66B30(py, pz, ty, tz);
		int32_t np = Ribbon_B1AF60_TurnToward(S16(prev, 0xE), pitch, S16(CUR(), 0x8E));
		U16(nw, 0xE) = (uint16_t)np;
		uint8_t *w = WS();
		uint8_t *o = w + 0xF0;
		uint8_t *d = w + 0xE0;
		int32_t s = x::Sin((int16_t)np);
		s = shl32(s, 12) >> 12;
		U16(d, 2) = (uint16_t)s;
		U16(o, 2) = (uint16_t)add32(py, s);
		int32_t ny = S16(nw, 6);
		int32_t c = x::Cos(ny);
		c = shl32(c, 12) >> 12;
		pz = add32(pz, c);
		U16(d, 4) = (uint16_t)c;
		U16(o, 4) = (uint16_t)pz;
		s = x::Sin(ny);
		s = shl32(s, 12) >> 12;
		U16(d, 0) = (uint16_t)s;
		U16(o, 0) = (uint16_t)add32(px, s);
		return s;
	}

	static int32_t Ribbon_B1AFE0_FollowDelta(const uint8_t *pos, const uint8_t *prev)
	{
		int32_t pz = S16(prev, 4);
		int32_t py = S16(prev, 2);
		int32_t px = S16(prev, 0);
		uint8_t *w = WS();
		uint8_t *o = w + 0xF0;
		uint8_t *d = w + 0xE0;
		int32_t ax = S16(pos, 0), cx = S16(pos, 2), dx = S16(pos, 4);
		U16(o, 0) = (uint16_t)ax;
		U16(o, 2) = (uint16_t)cx;
		U16(o, 4) = (uint16_t)dx;
		U16(d, 0) = (uint16_t)sub32(ax, px);
		U16(d, 2) = (uint16_t)sub32(cx, py);
		U16(d, 4) = (uint16_t)sub32(dx, pz);
		int32_t ez = sub32(dx, pz), ey = sub32(cx, py), ex = sub32(ax, px);
		int32_t sq = add32(add32(mul32(ez, ez), mul32(ey, ey)), mul32(ex, ex));
		return xl_ISqrt(sq);
	}

	static void Ribbon_B1B080_BuildCrossSection(int32_t w, int32_t dep, uint8_t *pts, int32_t flip)
	{
		uint8_t *v = WS() + 0xF0;
		int32_t ebx = w;
		uint16_t save6 = U16(pts, 6);
		uint16_t saveE = U16(pts, 0xE);
		if (flip != 0) ebx = sub32(0, ebx);
		int32_t wfl = ebx;
		int32_t nd = sub32(0, dep);
		U16(v, 0) = 0;
		U16(v, 2) = 0;
		U16(v, 4) = (uint16_t)nd;
		xl_RotTrans(v, pts, WS() + 0xFC);
		U16(v, 0) = (uint16_t)ebx;
		U16(v, 2) = 0;
		U16(v, 4) = (uint16_t)nd;
		xl_RotTrans(v, pts + 8, WS() + 0xFC);
		int32_t nw = sub32(0, ebx);
		U16(v, 0) = (uint16_t)nw;
		xl_RotTrans(v, pts + 0x10, WS() + 0xFC);
		int32_t t = shl32(mul32(ebx, 3), 9);
		t = add32(t, (t >> 31) & 0xFFF);
		ebx = t >> 12;
		U16(v, 0) = (uint16_t)ebx;
		xl_RotTrans(v, pts + 0x18, WS() + 0xFC);
		int32_t n38 = sub32(0, ebx);
		U16(v, 0) = (uint16_t)n38;
		xl_RotTrans(v, pts + 0x20, WS() + 0xFC);
		U16(v, 0) = 0;
		U16(v, 2) = (uint16_t)wfl;
		U16(v, 4) = (uint16_t)nd;
		xl_RotTrans(v, pts + 0x28, WS() + 0xFC);
		U16(v, 2) = (uint16_t)nw;
		xl_RotTrans(v, pts + 0x30, WS() + 0xFC);
		U16(v, 2) = (uint16_t)ebx;
		xl_RotTrans(v, pts + 0x38, WS() + 0xFC);
		U16(v, 2) = (uint16_t)n38;
		xl_RotTrans(v, pts + 0x40, WS() + 0xFC);
		U16(pts, 6) = save6;
		U16(pts, 0xE) = saveE;
	}

	static void Ribbon_B1B220_BuildColourRamp(uint32_t a, uint32_t b, uint8_t *out, int32_t unused)
	{
		(void)unused;
		x::GteWriteData((int32_t)0x3A000000, 6);
		uint8_t *pa = WS() + 0xE0;
		U32(pa, 0) = (a & 0x2FFFFFFu) | 0x38000000u;
		uint8_t *pb = WS() + 0xE4;
		U32(pb, 0) = (b & 0x2FFFFFFu) | 0x38000000u;
		uint8_t *esi = out;
		uint8_t *w = WS();
		int32_t n = S32(w, 0xF0);
		if (n != 0)
		{
			int32_t step = 0x1000 / n;
			int32_t ir0 = 0;
			if (n > 0)
			{
				uint8_t *edi = esi + 4;
				for (int32_t cnt = n; cnt != 0; cnt--)
				{
					xl_GteSetIR0(ir0);
					xl_GteLoadRGB(pa);
					xl_GteGPF();
					xl_GteStoreRGB(esi);
					U8(edi, -1) = 0x3A;
					xl_GteSetIR0(ir0);
					xl_GteLoadRGB(pb);
					xl_GteGPF();
					xl_GteStoreRGB(edi);
					U8(edi, 3) = 0x3A;
					esi += 8;
					edi += 8;
					ir0 = add32(ir0, step);
				}
				w = WS();
			}
		}
		{
			int32_t m = sub32(S32(w, 0xF4), S32(w, 0xF0));
			uint32_t e4 = U32(w, 0xE4);
			uint32_t e0 = U32(w, 0xE0);
			if (m > 0)
			{
				for (; m != 0; m--)
				{
					U32(esi, 0) = e0;
					U32(esi, 4) = e4;
					esi += 8;
				}
				w = WS();
			}
		}
		n = sub32(S32(w, 0xF8), S32(w, 0xF4));
		if (n != 0)
		{
			int32_t step = 0x1000 / n;
			int32_t ir0 = 0x1000;
			if (n > 0)
			{
				uint8_t *edi = esi + 4;
				for (int32_t cnt = n; cnt != 0; cnt--)
				{
					xl_GteSetIR0(ir0);
					xl_GteLoadRGB(pa);
					xl_GteGPF();
					xl_GteStoreRGB(esi);
					U8(edi, -1) = 0x3A;
					xl_GteSetIR0(ir0);
					xl_GteLoadRGB(pb);
					xl_GteGPF();
					xl_GteStoreRGB(edi);
					U8(edi, 3) = 0x3A;
					esi += 8;
					edi += 8;
					ir0 = sub32(ir0, step);
				}
				w = WS();
			}
		}
		int32_t z = S32(w, 0xF8);
		for (int32_t cnt = z; cnt > 0; cnt--)
		{
			U32(esi, 0) = 0x3A000000u;
			U32(esi, 4) = 0x3A000000u;
			esi += 8;
		}
	}

	static int32_t Ribbon_B1ADD0_SetupParentXform()
	{
		Mat4x3 *m = blob::GetParentMatrix(U16(CUR(), 0x9C));
		x::GteSetRotMatrix(m);
		x::GteSetTransVector(m);
		x::GteWriteData(0, 1);
		x::GteWriteData(0, 1);
		x::GteMVMVA_RotV0Tr();
		x::Gte_45E580();
		return 0;
	}

	static void Ribbon_B1AE20_InsertQuad(int32_t sum, void *prim)
	{
		int32_t e = add32(sum, (sum >> 31) & 3) >> 2;
		e = e >> 2;
		e = (int32_t)((uint32_t)e & 0xFFFFFFFCu);
		e = add32(e, S32(RT(), 0x4C));
		x::InsertPrimDepthKeys((uint32_t)e, prim, 0, 0, 0, 0);
	}

	static uint8_t *QuadOuter(uint8_t *pkt, const uint8_t *col, const uint8_t *pp, const uint8_t *cc, int a, int b)
	{
		U8(pkt, 3) = 0xA;
		U32(pkt, 8) = 0;
		U32(pkt, 4) = U32(WS(), 0x64);
		U32(pkt, 0xC) = U32(WS(), 0x60);
		U32(pkt, 0x1C) = 0;
		U32(pkt, 0x14) = U32(col, 4);
		U32(pkt, 0x24) = U32(col, 0xC);
		U32(pkt, 0x10) = U32(pp, a);
		U32(pkt, 0x18) = U32(pp, b);
		U32(pkt, 0x20) = U32(cc, a);
		U32(pkt, 0x28) = U32(cc, b);
		int32_t sum = (int32_t)U16(pp, a + 4) + (int32_t)U16(cc, a + 4) + (int32_t)U16(pp, b + 4) + (int32_t)U16(cc, b + 4);
		Ribbon_B1AE20_InsertQuad(sum, pkt);
		return pkt + 0x2C;
	}
	static uint8_t *QuadInner(uint8_t *pkt, const uint8_t *col, const uint8_t *pp, const uint8_t *cc, int a)
	{
		U8(pkt, 3) = 0xA;
		U32(pkt, 8) = 0;
		U32(pkt, 4) = U32(WS(), 0x64);
		U32(pkt, 0xC) = U32(col, 4);
		U32(pkt, 0x14) = U32(col, 0);
		U32(pkt, 0x1C) = U32(col, 0xC);
		U32(pkt, 0x24) = U32(col, 8);
		U32(pkt, 0x10) = U32(pp, a);
		U32(pkt, 0x18) = U32(pp, 0);
		U32(pkt, 0x20) = U32(cc, a);
		U32(pkt, 0x28) = U32(cc, 0);
		int32_t sum = (int32_t)U16(pp, a + 4) + (int32_t)U16(pp, 4) + (int32_t)U16(cc, 4) + (int32_t)U16(cc, a + 4);
		Ribbon_B1AE20_InsertQuad(sum, pkt);
		return pkt + 0x2C;
	}

	static inline uint8_t *RingEntry(uint8_t *ring, int32_t idx) { return ring + mul32(idx, 72) + 0x20; }

	static void __cdecl dh_37_RibbonTrail()
	{
		uint8_t *cur = CUR();
		uint8_t *ring = PTR(cur, 0xBC);
		uint8_t *edi;
		if (ring == nullptr)
		{
			uint8_t *prm = PTR(cur, 0xB8);
			uint32_t cap = U16(prm, 8);
			uint8_t *blk = blob::ArenaAlloc((int32_t)(cap * 9 * 8 + 0x20));
			PTR(CUR(), 0xBC) = blk;
			edi = PTR(CUR(), 0xBC);
			U16(edi, 2) = (uint16_t)cap;
			U16(edi, 0) = 1;
			U16(edi, 4) = 1;
			U16(edi, 8) = U16(prm, 0xE);
			U16(edi, 0xA) = U16(prm, 0xA);
			U16(edi, 0xC) = U16(prm, 0xC);
			U16(edi, 0xE) = 0;
			U16(edi, 0x10) = U16(prm, 0x10);
			uint16_t tref = U16(prm, 0);
			U16(edi, 6) = tref;
			PTR(WS(), 0x98) = blob::GetBone(tref);
			uint8_t *e0 = edi + 0x20;
			uint8_t *c = CUR();
			int32_t x0 = S16(c, 0x94);
			int32_t y0 = S16(c, 0x96);
			int32_t z0 = S16(c, 0x98);
			U16(e0, 0) = (uint16_t)x0;
			U16(e0, 2) = (uint16_t)y0;
			U16(e0, 4) = (uint16_t)z0;
			int32_t ty, tz;
			if (U8(edi, 0x10) & 1)
			{
				uint8_t *b = CUR();
				int32_t dx = S32(b, 0x74) >> 16;
				int32_t dz = S32(b, 0x7C) >> 16;
				int32_t yaw = blob::Blob_B66B30(x0, z0, add32(dx, x0), add32(dz, z0)) & 0xFFF;
				U16(e0, 0x4E) = (uint16_t)yaw;
				U16(e0, 6) = (uint16_t)yaw;
				b = CUR();
				ty = add32(S32(b, 0x78) >> 16, y0);
				tz = S32(b, 0x7C) >> 16;
			}
			else
			{
				int32_t yaw = blob::Blob_B66B30(x0, z0, add32(S16(prm, 2), x0), add32(S16(prm, 6), z0)) & 0xFFF;
				U16(e0, 0x4E) = (uint16_t)yaw;
				U16(e0, 6) = (uint16_t)yaw;
				ty = add32(S16(prm, 4), y0);
				tz = S16(prm, 6);
			}
			tz = add32(tz, z0);
			int32_t pr = blob::Blob_B66B30(y0, z0, ty, tz);
			U16(e0, 0x56) = (uint16_t)(pr & 0xFFF);
			U16(e0, 0xE) = (uint16_t)(pr & 0xFFF);
			int32_t r = x::Sin(pr);
			U16(WS(), 0xD2) = (uint16_t)(shl32(r, 12) >> 12);
			int32_t yw = S16(e0, 6);
			r = x::Cos(yw);
			U16(WS(), 0xD4) = (uint16_t)(shl32(r, 12) >> 12);
			r = x::Sin(yw);
			U16(WS(), 0xD0) = (uint16_t)(shl32(r, 12) >> 12);
			uint8_t *w = WS();
			int32_t flip = xl_LookAt_B66E50(ZEROVEC(), w + 0xD0, w + 0xA0);
			x::SetRotMatrix(WS() + 0xA0);
			int32_t sx = S16(e0, 0), sy = S16(e0, 2), sz = S16(e0, 4);
			x::GteWriteCtrl(sx, 5);
			x::GteWriteCtrl(sy, 6);
			x::GteWriteCtrl(sz, 7);
			Ribbon_B1B080_BuildCrossSection(S16(CUR(), 0x90), 0, e0, flip);
			int32_t dep = (U8(edi, 0x10) & 1) ? 0 : S16(CUR(), 0x8C);
			uint8_t *e1 = e0 + 0x48;
			Ribbon_B1B080_BuildCrossSection(S16(CUR(), 0x90), dep, e1, flip);
			PTR(WS(), 0x68) = e0;
			PTR(WS(), 0x6C) = e1;
			S32(WS(), 0x80) = U16(edi, 0);
			S32(WS(), 0x78) = U16(edi, 4);
			PTR(WS(), 0x9C) = blob::GetBone(U16(edi, 8));
		}
		else
		{
			PTR(WS(), 0x98) = blob::GetBone(U16(ring, 6));
			PTR(WS(), 0x9C) = blob::GetBone(U16(ring, 8));
			if (U8(RT(), 0x45) == 0)
			{
				int16_t ax = S16(CUR(), 0x8C);
				if (ax < 0)
				{
					if (ax != -2)
					{
						U16(ring, 0xE) = (uint16_t)(U16(ring, 0xE) + 1);
						if (U16(ring, 0xE) >= U16(ring, 2)) return;
					}
					int32_t rr = (int32_t)U16(ring, 0) % (int32_t)U16(ring, 2);
					PTR(WS(), 0x6C) = RingEntry(PTR(CUR(), 0xBC), rr);
				}
				else
				{
					U16(ring, 0) = (uint16_t)(U16(ring, 0) + 1);
					int32_t len = (int32_t)U16(ring, 4) + 1;
					if (len < (int32_t)U16(ring, 2)) U16(ring, 4) = (uint16_t)len;
					edi = PTR(CUR(), 0xBC);
					int32_t head = U16(edi, 0);
					S32(WS(), 0x80) = head;
					int32_t cap = U16(edi, 2);
					PTR(WS(), 0x6C) = RingEntry(edi, head % cap);
					PTR(WS(), 0x68) = RingEntry(edi, sub32(head, 1) % cap);
					S32(WS(), 0x78) = U16(edi, 4);
					uint8_t *w = WS();
					uint8_t *tgt = PTR(w, 0x98);
					uint8_t *prev = PTR(w, 0x68);
					uint8_t *nw = PTR(w, 0x6C);
					if (U8(edi, 0x10) & 1)
						S32(WS(), 0x94) = Ribbon_B1AFE0_FollowDelta(CUR() + 0x94, prev);
					else
						Ribbon_B1AE60_SteerToward(tgt + 0x94, prev, nw);
					w = WS();
					int32_t flip = xl_LookAt_B66E50(ZEROVEC(), w + 0xE0, w + 0xA0);
					x::SetRotMatrix(WS() + 0xA0);
					int32_t py = S16(prev, 2), pz = S16(prev, 4), px = S16(prev, 0);
					x::GteWriteCtrl(px, 5);
					x::GteWriteCtrl(py, 6);
					x::GteWriteCtrl(pz, 7);
					if (U8(edi, 0x10) & 1)
					{
						Ribbon_B1B080_BuildCrossSection(S16(CUR(), 0x90), S32(WS(), 0x94), nw, flip);
						uint8_t *b = CUR();
						int32_t dx = sub32(S16(b, 0x94), S16(nw, 0));
						int32_t dy = sub32(S16(b, 0x96), S16(nw, 2));
						int32_t dz = sub32(S16(b, 0x98), S16(nw, 4));
						uint8_t *p = nw + 4;
						for (int n = 9; n != 0; n--)
						{
							U16(p, -4) = (uint16_t)(U16(p, -4) + (uint16_t)dx);
							U16(p, -2) = (uint16_t)(U16(p, -2) + (uint16_t)dy);
							U16(p, 0) = (uint16_t)(U16(p, 0) + (uint16_t)dz);
							p += 8;
						}
					}
					else
					{
						Ribbon_B1B080_BuildCrossSection(S16(CUR(), 0x90), S16(CUR(), 0x8C), nw, flip);
						int32_t nx = S16(nw, 0);
						U16(CUR(), 0x94) = (uint16_t)nx;
						S32(CUR(), 0x5C) = shl32(nx, 16);
						int32_t ny = S16(nw, 2);
						U16(CUR(), 0x96) = (uint16_t)ny;
						S32(CUR(), 0x60) = shl32(ny, 16);
						int32_t nz = S16(nw, 4);
						U16(CUR(), 0x98) = (uint16_t)nz;
						S32(CUR(), 0x64) = shl32(nz, 16);
						int32_t ex = sub32(S16(tgt, 0x94), nx);
						int32_t ey = sub32(S16(tgt, 0x96), ny);
						int32_t ez = sub32(S16(tgt, 0x98), nz);
						int32_t sq = add32(add32(mul32(ez, ez), mul32(ex, ex)), mul32(ey, ey));
						int32_t dist = xl_ISqrt(sq);
						int32_t rad = S16(tgt, 0x8E);
						if (dist > rad) U16(CUR(), 0x4A) = (uint16_t)(U16(CUR(), 0x4A) & 0xFFFE);
						else U8(CUR(), 0x4A) = (uint8_t)(U8(CUR(), 0x4A) | 1);
					}
				}
			}
		}

		uint8_t *c = CUR();
		edi = PTR(c, 0xBC);
		uint8_t *const rng = edi;
		int32_t v90;
		if (U8(edi, 0x10) & 1) v90 = shl32(S16(c, 0x8E), 4);
		else v90 = shl32(S16(PTR(WS(), 0x98), 0x8C), 4);
		S32(WS(), 0x90) = v90;
		uint8_t *w = WS();
		int32_t arg4 = S32(w, 0x90);
		uint8_t *cb = PTR(w, 0x9C);
		uint32_t colA = (uint32_t)(int32_t)S16(cb, 0x98) | 0x200u;
		colA = (colA << 8) | (uint32_t)(int32_t)S16(cb, 0x96);
		colA = (colA << 8) | (uint32_t)(int32_t)S16(cb, 0x94);
		uint32_t colB = (uint32_t)(int32_t)S16(cb, 0x90) | 0x200u;
		colB = (colB << 8) | (uint32_t)(int32_t)S16(cb, 0x8E);
		colB = (colB << 8) | (uint32_t)(int32_t)S16(cb, 0x8C);
		PTR(w, 0x84) = PTR(CTX(), 0x74) + 0x100;
		S32(WS(), 0xF0) = U16(edi, 0xA);
		S32(WS(), 0xF4) = U16(edi, 0xC);
		S32(WS(), 0xF8) = U16(edi, 2);

		Ribbon_B1B220_BuildColourRamp(colA, colB, PTR(WS(), 0x84), arg4);
		Ribbon_B1ADD0_SetupParentXform();

		uint8_t *pkt = PTR(CTX(), 0x7C);
		{
			int32_t rr = (int32_t)U16(edi, 0) % (int32_t)U16(edi, 2);
			PTR(WS(), 0x6C) = RingEntry(PTR(CUR(), 0xBC), rr);
			uint8_t *pt = PTR(WS(), 0x6C);
			uint8_t *out = PTR(CTX(), 0x74);
			for (int n = 9; n != 0; n--)
			{
				uint8_t *fl = WS() + 0xFC;
				S32(out, 4) = xl_RotTransPers(pt, out, fl, fl);
				out += 8;
				pt += 8;
			}
		}
		S32(WS(), 0x7C) = 0;
		S32(WS(), 0x60) = (int32_t)((U32(CUR(), 0xCC) & 0x2000000u) | 0x38000000u);
		S32(WS(), 0x64) = (int32_t)((uint32_t)(int32_t)S16(CUR(), 0x92) | 0xE1000000u);
		S32(WS(), 0x88) = U16(edi, 4);
		uint8_t *col = PTR(WS(), 0x84);
		{
			uint32_t tail = U16(edi, 0xE);
			if (tail >= 1)
			{
				S32(WS(), 0x88) = sub32(S32(WS(), 0x88), (int32_t)tail);
				col = col + tail * 8;
			}
		}
		S32(WS(), 0x7C) = add32(S32(WS(), 0x7C), 1);
		while (S32(WS(), 0x7C) <= S32(WS(), 0x88))
		{
			int32_t k = S32(WS(), 0x7C);
			int32_t rr = sub32((int32_t)U16(rng, 0), k) % (int32_t)U16(rng, 2);
			PTR(WS(), 0x68) = RingEntry(PTR(CUR(), 0xBC), rr);
			uint8_t *ws = WS();
			int32_t odd = S32(ws, 0x7C) & 1;
			uint8_t *src = PTR(ws, 0x68);
			uint8_t *base = PTR(CTX(), 0x74);
			uint8_t *dst = base + (odd << 7);
			if (odd == 0)
			{
				PTR(ws, 0x70) = base;
				PTR(WS(), 0x74) = PTR(WS(), 0x70) + 0x80;
			}
			else
			{
				PTR(ws, 0x74) = base;
				PTR(WS(), 0x70) = PTR(WS(), 0x74) + 0x80;
			}
			src += 2;
			for (int n = 9; n != 0; n--)
			{
				uint32_t z = U16(src, 2);
				uint32_t xx = U16(src, -2);
				uint32_t y = U16(src, 0);
				x::GteWriteData((int32_t)((y << 16) | xx), 0);
				x::GteWriteData((int32_t)z, 1);
				x::GteRTPS();
				x::GteReadSXY2(dst);
				U16(dst, 4) = GTE_SZ3_W();
				dst += 8;
				src += 8;
			}
			ws = WS();
			const uint8_t *cc = PTR(ws, 0x70);
			const uint8_t *pp = PTR(ws, 0x74);
			pkt = QuadOuter(pkt, col, pp, cc, 0x08, 0x18);
			pkt = QuadOuter(pkt, col, pp, cc, 0x10, 0x20);
			pkt = QuadInner(pkt, col, pp, cc, 0x18);
			pkt = QuadInner(pkt, col, pp, cc, 0x20);
			pkt = QuadOuter(pkt, col, pp, cc, 0x28, 0x38);
			pkt = QuadOuter(pkt, col, pp, cc, 0x30, 0x40);
			pkt = QuadInner(pkt, col, pp, cc, 0x38);
			pkt = QuadInner(pkt, col, pp, cc, 0x40);
			col += 8;
			S32(WS(), 0x7C) = add32(S32(WS(), 0x7C), 1);
		}
		PTR(CTX(), 0x7C) = pkt;
	}
}
	void apply_shared_ribbon(Clone &c)
	{
		c.draw[37] = part_shared_ribbon::dh_37_RibbonTrail;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace part_shared_misc
{
	static inline void xl_InsertPrimAutoDepth(uint32_t ot, void *prim) { x::f<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAutoDepth))(ot, prim); }
	static inline void xl_SetDrawStp(void *p, int32_t pbw) { x::f<void (__cdecl *)(void *, int32_t)>(game_addr(game::SetDrawStp))(p, pbw); }
	static inline void xl_SetDrawMove(void *p, const void *rect, int32_t xx, int32_t yy) { x::f<void (__cdecl *)(void *, const void *, int32_t, int32_t)>(game_addr(game::SetDrawMove))(p, rect, xx, yy); }
	static inline void xl_QueueChainTransformation(void *entity, int32_t id) { x::f<void (__cdecl *)(void *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	static inline uint8_t *xl_ChainMinEntity(void *entity) { return x::f<uint8_t *(__cdecl *)(void *)>(game_addr(game::Battle_EntityChainLowest))(entity); }
	static inline int32_t xl_OffscreenStageRender(void *desc, const void *pos, const void *angles)
	{
		return x::f<int32_t (__cdecl *)(void *, const void *, const void *)>(game_addr(game::GfCinematic_RenderBattleStageOffscreen))(desc, pos, angles);
	}

	static constexpr game::Id DISP_PARITY_1D96A80 = game::g_Battle_DisplayParity;
	static constexpr game::Id DISP_ENV_1D969C8 = game::g_Battle_DrawEnv;
	static constexpr game::Id BATTLE_ENTITIES_1D972C0 = game::BattleEntitySlotData;

	static inline int32_t DepthKey(uint32_t off)
	{
		uint32_t v = DEPTHARR()[off >> 3] & 0xFFFF;
		return (int32_t)(int64_t)((double)v * (double)FLT_1877DA8());
	}

	static inline int32_t Nclip(uint32_t s0, uint32_t s1, uint32_t s2)
	{
		int32_t mac0;
		x::GteWriteData((int32_t)s0, 12);
		x::GteWriteData((int32_t)s1, 13);
		x::GteWriteData((int32_t)s2, 14);
		x::GteNCLIP();
		x::GteReadData(&mac0, 24);
		return mac0;
	}

	static inline int32_t SDivPow2(int32_t v, int n)
	{
		return add32(v, (int32_t)((uint32_t)(v >> 31) & ((1u << n) - 1))) >> n;
	}

	static void __cdecl dh_16_ScrollTextureU()
	{
		uint8_t *arg = PTR(CUR(), 0xB8);
		blob::ReadAltTexture(S16(arg, 0));
		U32(WS(), 0x64) = U32(WS(), 0xFC);
		uint8_t *tex = PTR(WS(), 0xF0);
		if (U32(CUR(), 0xBC) == 0)
		{
			int32_t n = shl32(mul32(S16(tex, 6), S16(tex, 4)), 1);
			uint8_t *b = blob::ArenaAlloc(n);
			PTR(CUR(), 0xBC) = b;
		}
		U32(WS(), 0x60) = U32(CUR(), 0xBC);
		{
			uint8_t *w = WS();
			U32(w, 0x80) = U32(w, 0x60);
		}
		PTR(WS(), 0x68) = blob::VramRectRing();
		uint8_t *rect = PTR(WS(), 0x68);
		U16(rect, 0) = U16(arg, 2);
		U16(rect, 2) = U16(arg, 4);
		uint16_t wv = U16(tex, 4);
		U16(rect, 4) = wv;
		S32(WS(), 0x78) = (int16_t)wv;
		int32_t wd = S32(WS(), 0x78);
		uint16_t hv = U16(tex, 6);
		U16(rect, 6) = hv;
		S32(WS(), 0x7C) = (int16_t)hv;
		int32_t mask = sub32(wd, 1);
		int32_t stride = add32(wd, wd);
		S32(WS(), 0x6C) = mask;
		S32(WS(), 0x70) = (int32_t)S16(CUR(), 0x94) & mask;
		S32(WS(), 0x84) = stride;
		uint8_t *w = WS();
		if (S32(w, 0x7C) > 0)
		{
			do
			{
				int32_t sc = S32(w, 0x70);
				uint8_t *src = PTR(w, 0x64);
				uint8_t *dst = (uint8_t *)(U32(w, 0x60) + (uint32_t)sc * 2u);
				int32_t k = sub32(S32(w, 0x78), sc);
				if (k > 0)
				{
					do
					{
						U16(dst, 0) = U16(src, 0);
						src += 2;
						dst += 2;
					} while (--k != 0);
					w = WS();
				}
				k = S32(w, 0x70);
				dst = PTR(w, 0x60);
				if (k > 0)
				{
					do
					{
						U16(dst, 0) = U16(src, 0);
						src += 2;
						dst += 2;
					} while (--k != 0);
					w = WS();
				}
				int32_t st = S32(w, 0x84);
				U32(w, 0x60) = U32(w, 0x60) + (uint32_t)st;
				w = WS();
				U32(w, 0x64) = U32(w, 0x64) + (uint32_t)st;
				w = WS();
				S32(w, 0x7C) = sub32(S32(w, 0x7C), 1);
				w = WS();
			} while (S32(w, 0x7C) > 0);
		}
		x::QueueVramUpload(PTR(w, 0x68), PTR(w, 0x80));
	}

	static int32_t CaptureScreenStrips()
	{
		uint32_t par = MEM<uint8_t>(game_addr(DISP_PARITY_1D96A80));
		uint8_t *pkt = PTR(CTX(), 0x7C);
		uint32_t idx = par * 3u;
		idx <<= 3;
		idx -= par;
		uint8_t *env = (uint8_t *)(game_addr(DISP_ENV_1D969C8) + idx * 4u);
		xl_SetDrawStp(pkt, 0);
		xl_InsertPrimAutoDepth(U32(RT(), 0x4C) + 4, pkt);
		uint8_t *w = WS();
		U16(w, 0xF8) = U16(env, 0);
		pkt += 0xC;
		U16(w, 0xFA) = U16(env, 2);
		U32(WS(), 0xFC) = 0xE00040;
		w = WS();
		int32_t yy = S32(w, 0xF4);
		int32_t xx = S32(w, 0xF0);
		for (int k = 5; k != 0; k--)
		{
			xl_SetDrawMove(pkt, WS() + 0xF8, xx, yy);
			xl_InsertPrimAutoDepth(U32(RT(), 0x4C) + 4, pkt);
			w = WS();
			xx = add32(xx, 0x40);
			pkt += 0x18;
			U32(w, 0xF8) = U32(w, 0xF8) + 0x40;
		}
		xl_SetDrawStp(pkt, 1);
		xl_InsertPrimAutoDepth(U32(RT(), 0x4C) + 4, pkt);
		pkt += 0xC;
		PTR(CTX(), 0x7C) = pkt;
		return 0;
	}

	static void __cdecl dh_26_ScreenCaptureStrips()
	{
		uint8_t *arg = PTR(CUR(), 0xB8);
		S32(WS(), 0x90) = S16(arg, 0);
		S32(WS(), 0x94) = S16(arg, 2);
		uint8_t *b = CUR();
		if (U32(b, 0xC4) == 0)
		{
			U32(b, 0xC4) = 0xFFFFFFFF;
			U32(WS(), 0xF0) = U32(WS(), 0x90);
			U32(WS(), 0xF4) = U32(WS(), 0x94);
			CaptureScreenStrips();
			return;
		}
		uint32_t c = U32(b, 0xCC);
		uint32_t t = (uint32_t)((int32_t)c >> 8);
		if (((t ^ c) & 0xFF) != 0) return;
		if (((t ^ c) & 0xFF00) != 0) return;
		int32_t tp = S16(b, 0x92);
		uint8_t *pkt = PTR(CTX(), 0x7C);
		uint8_t *w = WS();
		int32_t di = 0;
		int32_t v = (S32(w, 0x90) >> 2) & 0xF0;
		v |= S32(w, 0x94) & 0x100;
		v >>= 4;
		v |= tp;
		v |= 0x100;
		S32(w, 0x80) = v;
		U32(WS(), 0x84) = U32(CUR(), 0xCC) | 0x2A000000;
		S32(WS(), 0x60) = S16(CUR(), 0x94);
		S32(WS(), 0x64) = S16(CUR(), 0x96);
		for (int k = 5; k != 0; k--)
		{
			U8(pkt, 3) = 5;
			U32(pkt, 4) = U32(WS(), 0x84);
			uint16_t x0 = (uint16_t)(U16(WS(), 0x60) + (uint16_t)di);
			U16(pkt, 0x10) = x0;
			U16(pkt, 8) = x0;
			uint16_t x1 = (uint16_t)((uint16_t)(U16(WS(), 0x60) + (uint16_t)di) + 0x40);
			U16(pkt, 0x14) = x1;
			U16(pkt, 0xC) = x1;
			uint16_t y0 = U16(WS(), 0x64);
			U16(pkt, 0xE) = y0;
			U16(pkt, 0xA) = y0;
			uint16_t y1 = (uint16_t)(U16(WS(), 0x64) + 0xE0);
			U16(pkt, 0x16) = y1;
			U16(pkt, 0x12) = y1;
			x::InsertPrimAltViewport(U32(RT(), 0x4C) + 8, pkt);
			di += 0x40;
			pkt += 0x18;
		}
		int16_t rect[4];
		rect[1] = 0;
		rect[0] = 0;
		rect[3] = 0x100;
		rect[2] = 0x100;
		x::SetDrawMode(pkt, 0, 0, 0, rect);
		x::InsertPrimAltViewport(U32(RT(), 0x4C) + 8, pkt);
		PTR(CTX(), 0x7C) = pkt + 0xC;
		U32(WS(), 0xF0) = U32(WS(), 0x90);
		U32(WS(), 0xF4) = U32(WS(), 0x94);
	}

	static void __cdecl dh_39_Starfield()
	{
		if (U32(CUR(), 0xBC) == 0)
		{
			uint8_t *prm = PTR(CUR(), 0xB8);
			int32_t n = S16(prm, 0);
			uint8_t *blk = blob::ArenaAlloc(add32(shl32(n, 4), 4));
			PTR(CUR(), 0xBC) = blk;
			uint8_t *s = PTR(CUR(), 0xBC);
			U16(s, 2) = (uint16_t)n;
			int32_t jit = S16(prm, 8);
			uint32_t col = U32(CUR(), 0xCC);
			s += 4;
			int32_t c0 = (int32_t)(col & 0xFF);
			int32_t c2 = (int32_t)((col >> 16) & 0xFF);
			int32_t c1 = (int32_t)((col >> 8) & 0xFF);
			int32_t rx = S16(prm, 2);
			int32_t ry = S16(prm, 4);
			int32_t rz = S16(prm, 6);
			S32(WS(), 0xF0) = S16(prm, 0xA);
			if (n > 0)
			{
				int32_t k = n;
				do
				{
					U16(s, 6) = 0;
					U16(s, 0xC) = 0;
					U16(s, 0) = (uint16_t)blob::Rand73(rx);
					U16(s, 2) = (uint16_t)blob::Rand73(ry);
					U16(s, 4) = (uint16_t)blob::Rand73(rz);
					int32_t a = add32(blob::Rand73(jit), c0);
					int32_t g = shl32(add32(blob::Rand73(jit), c1), 8);
					a |= g;
					int32_t bl = shl32(add32(blob::Rand73(jit), c2), 16);
					U32(s, 8) = (uint32_t)(bl | a | 0x68000000);
					int32_t r = blob::Rand74(0x100);
					if (r < S32(WS(), 0xF0))
					{
						U8(s, 6) |= 1;
						int32_t tt = blob::Rand73(0x40);
						if (tt > 0) U8(s, 0xC) = (uint8_t)tt;
						else U8(s, 0xD) = (uint8_t)~(uint8_t)tt;
					}
					s += 0x10;
				} while (--k != 0);
			}
		}
		uint8_t *blk = PTR(CUR(), 0xBC);
		uint8_t *stars = blk + 4;
		PTR(WS(), 0x60) = stars;
		U16(blk, 0) = (uint16_t)(U16(blk, 0) + 1);
		uint32_t otw = U16(CUR(), 0xC0);
		if (otw == 0)
			U32(WS(), 0x64) = 0;
		else
			U32(WS(), 0x64) = (otw & 0x3FFC) + U32(RT(), 0x4C);
		h_B27000();
		int32_t n = S16(blk, 2);
		uint8_t *scr0 = PTR(CTX(), 0x74);
		if (n > 0)
		{
			uint8_t *s = scr0;
			uint8_t *v = stars;
			int32_t k = n;
			do
			{
				x::GteLoadV0(v);
				x::GteRTPS();
				x::GteReadSXY2(s);
				x::GteReadOTZdiv4(s + 4);
				v += 0x10;
				s += 8;
			} while (--k != 0);
		}
		uint8_t *sc = PTR(CTX(), 0x74);
		uint8_t *pkt = PTR(CTX(), 0x7C);
		uint8_t *e = PTR(WS(), 0x60);
		n = S16(blk, 2);
		if (n > 0)
		{
			e += 0xC;
			for (int32_t i = 0; i < n; i++)
			{
				bool draw;
				if (U8(e, -6) & 1)
				{
					uint8_t al = U8(e, 0);
					if (al == 0)
					{
						al = (uint8_t)(U8(e, 1) - 1);
						U8(e, 1) = al;
						if (al == 0)
						{
							int32_t r = blob::Rand74(0x40);
							U8(e, 1) = 0;
							U8(e, 0) = (uint8_t)r;
						}
						draw = false;
					}
					else
					{
						al--;
						U8(e, 0) = al;
						if (al == 0)
						{
							int32_t r = blob::Rand74(0x40);
							U8(e, 0) = 0;
							U8(e, 1) = (uint8_t)r;
						}
						draw = true;
					}
				}
				else
					draw = true;
				if (draw)
				{
					U8(pkt, 3) = 2;
					U32(pkt, 4) = U32(e, -4);
					U32(pkt, 8) = U32(sc, 0);
					uint32_t ot = U32(WS(), 0x64);
					if (ot == 0) ot = (uint32_t)((int32_t)S16(sc, 4) & ~3) + U32(RT(), 0x4C);
					xl_InsertPrimAutoDepth(ot, pkt);
					pkt += 0xC;
				}
				sc += 8;
				e += 0x10;
			}
		}
		PTR(CTX(), 0x7C) = pkt;
	}

	static void __cdecl pr_16_TintedF4()
	{
		h_B29450((int32_t)U32(WS(), 0x84));
		uint8_t *ws = WS();
		int32_t count = S32(ws, 0x70);
		uint8_t *rec = PTR(ws, 0x6C);
		uint8_t *pkt = PTR(ws, 0x60);
		uint32_t ot = U32(ws, 0x5C);
		uint32_t nocull = U32(ws, 0x90) & 0x10;
		U32(ws, 0xF0) = 0;
		do
		{
			uint8_t *vb = PTR(ws, 0x7C);
			uint32_t o0 = U16(rec, 4);
			uint32_t o2 = U16(rec, 8);
			uint32_t o1 = U16(rec, 6);
			uint32_t o3 = U16(rec, 0xA);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			int32_t z3 = DepthKey(o3);
			uint8_t *p2 = vb + o2;
			uint8_t *p3 = vb + o3;
			uint32_t s2 = U32(p2, 0);
			uint32_t s1 = U32(vb + o1, 0);
			uint8_t *p0 = vb + o0;
			uint8_t *p1 = vb + o1;
			uint32_t s3 = U32(p3, 0);
			uint32_t s0 = U32(p0, 0);
			U32(pkt, 0x1C) = s3;
			U32(pkt, 0x14) = s1;
			U32(pkt, 0x10) = s0;
			U32(pkt, 0x18) = s2;
			if (nocull == 0 && Nclip(s0, s1, s2) < 0) goto next;
			{
				uint32_t a0 = U32(p0, 4), a1 = U32(p1, 4), a2 = U32(p2, 4), a3 = U32(p3, 4);
				if (((a3 | a2 | a1 | a0) >> 16) & 0x46) goto next;
				uint32_t slot = (((a3 + a2 + a1 + a0) >> 2) & 0x3FFC) + ot;
				U8(pkt, 3) = 7;
				U32(pkt, 8) = 0;
				x::GteWriteData((int32_t)(U32(ws, 0x8C) | U32(rec, 0) | 0x28000000), 6);
				x::Gte_4601B0();
				uint32_t mode = U32(ws, 0x50);
				x::GteReadData2(0x16, pkt + 0xC);
				U32(pkt, 4) = mode;
				x::InsertPrimDepthKeys(slot, pkt, z0, z1, z2, z3);
				pkt += 0x20;
			}
		next:
			rec += 0xC;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}

	static void __cdecl pr_29_GT3()
	{
		uint8_t *ws = WS();
		int32_t count = S32(ws, 0x70);
		uint8_t *pkt = PTR(ws, 0x60);
		uint32_t ot = U32(ws, 0x5C);
		uint8_t *rec = PTR(ws, 0x6C);
		do
		{
			uint8_t *vb = PTR(ws, 0x7C);
			uint32_t o0 = U16(rec, 0x12);
			uint32_t o1 = U16(rec, 0x14);
			uint32_t o2 = U16(rec, 0x16);
			int32_t z0 = DepthKey(o0);
			int32_t z1 = DepthKey(o1);
			int32_t z2 = DepthKey(o2);
			uint8_t *p2 = vb + o2;
			uint8_t *p0 = vb + o0;
			uint8_t *p1 = vb + o1;
			if (U16(p0, 6) & (uint16_t)(U16(p2, 6) & U16(p1, 6))) goto next;
			{
				uint32_t s0 = U32(p0, 0);
				uint32_t s1 = U32(p1, 0);
				uint32_t s2 = U32(p2, 0);
				U32(pkt, 8) = s0;
				U32(pkt, 0x14) = s1;
				U32(pkt, 0x20) = s2;
				if (Nclip(s0, s1, s2) < 0) goto next;
				int32_t sum = (int32_t)(U16(p2, 4) + U16(p1, 4) + U16(p0, 4));
				int32_t hi = (int32_t)(((int64_t)sum * (int64_t)0x55555556) >> 32);
				U8(pkt, 3) = 9;
				int32_t sgn = sum >> 31;
				uint32_t tp = U32(ws, 0x50) | U16(rec, 0x1A);
				int32_t clut_add = S32(ws, 0x54);
				U16(pkt, 0x1A) = (uint16_t)tp;
				U16(pkt, 0xE) = (uint16_t)add32(U16(rec, 0x18), clut_add);
				U16(pkt, 0xC) = U16(rec, 0xC);
				U16(pkt, 0x18) = U16(rec, 0xE);
				U16(pkt, 0x24) = U16(rec, 0x10);
				U32(pkt, 0x10) = U32(rec, 4);
				U32(pkt, 0x1C) = U32(rec, 8);
				uint32_t key = (uint32_t)sub32(hi, sgn) & 0x3FFC;
				U32(pkt, 4) = U32(rec, 0) | 0x34000000;
				x::InsertPrimDepthKeys(ot + key, pkt, z0, z1, z2, 0);
				pkt += 0x28;
			}
		next:
			rec += 0x1C;
		} while (--count > 0);
		PTR(ws, 0x60) = pkt;
	}

	static void __cdecl op_022_ApplyActionResultList()
	{
		uint8_t *act = PTR(CTX(), 0xCC);
		uint32_t sub = (uint32_t)U16(RT(), 0x4A) >> 12;
		uint8_t *rec = PTR(act, 8);
		int32_t n = S8(act, 0x10);
		if (sub == 1)
		{
			uint8_t *sc = SCENE();
			int32_t m = S8(sc, 0x17A);
			if (m != 0)
			{
				if (m > 0)
				{
					uint8_t *list = sc + 0x173;
					do
					{
						uint32_t slot = U8(list, m);
						uint8_t *r = PTR(act, 8);
						int32_t k = n;
						if (k > 0)
						{
							do
							{
								if (slot == U8(r, 0))
								{
									x::ApplyActionResultToTarget((int32_t)r);
									break;
								}
								r += 0x18;
								k--;
							} while (k > 0);
						}
						m--;
					} while (m > 0);
				}
				STREAM() += 2;
				return;
			}
		}
		else if (sub == 8)
		{
			x::ApplyActionResultToTargets((int32_t)rec, n);
			STREAM() += 2;
			return;
		}
		uint32_t slot = U8(CUR(), 0x1B);
		if (n > 0)
		{
			do
			{
				if (slot == U8(rec, 0))
				{
					x::ApplyActionResultToTarget((int32_t)rec);
					STREAM() += 2;
					return;
				}
				rec += 0x18;
				n--;
			} while (n > 0);
		}
		STREAM() += 2;
	}

	static void __cdecl op_042_EntityChainAnim()
	{
		uint32_t sub = (uint32_t)U16(RT(), 0x4A) >> 9;
		if (sub == 0)
		{
			int32_t id = S16(STREAM(), 2);
			uint32_t slot = U8(CUR(), 0x1B);
			uint8_t *e = PTR(SCENE(), 0x60 + (int32_t)slot * 4);
			xl_QueueChainTransformation(e, id);
			STREAM() += 4;
		}
		else if (sub == 2)
		{
			uint32_t slot = U8(CUR(), 0x1B);
			uint8_t *e = PTR(SCENE(), 0x60 + (int32_t)slot * 4);
			uint8_t a = U8(e, 0x72);
			uint8_t b = U8(e, 0x73);
			uint8_t *s = STREAM();
			if (a != b)
				STREAM() = s + 4;
			else
				STREAM() = s + S16(s, 2);
		}
	}

	static void __cdecl op_05C_AccumFromMeshVertex()
	{
		PTR(WS(), 0x60) = CUR();
		uint8_t *b = blob::GetBone(S16(STREAM(), 2));
		CUR() = b;
		int32_t ax, ay, az;
		uint8_t *next;
		if (U8(RT(), 0x4B) & 0x80)
		{
			uint8_t *p = blob::GetBone(U16(b, 0x9C));
			uint8_t *m = (uint8_t *)blob::RotMatrixOrder(p + 0x8C, S16(STREAM(), 6));
			S32(WS(), 0x80) = shl32(S16(CUR(), 0x8C), 4);
			S32(WS(), 0x84) = shl32(S16(CUR(), 0x8E), 4);
			S32(WS(), 0x88) = shl32(S16(CUR(), 0x90), 4);
			x::ScaleMatrix(m, WS() + 0x80);
			uint8_t *t = m + 0x14;
			S32(t, 0) = S16(p, 0x94);
			S32(m, 0x18) = S16(p, 0x96);
			S32(m, 0x1C) = S16(p, 0x98);
			x::SetRotMatrix(m);
			x::SetTransVector(m);
			x::TransformToCamera(CUR() + 0x94, t, WS() + 0x80);
			x::SetTransVector(m);
			uint8_t *mesh = PTR(CUR(), 0xD8);
			uint8_t *w = WS();
			int32_t vi = S16(STREAM(), 4);
			uint8_t *v = (uint8_t *)(uint32_t)add32(add32((int32_t)U32(mesh, 0x14), shl32(vi, 3)), (int32_t)(uint32_t)mesh);
			x::TransformToCamera(v, w + 0xF0, w + 0xFC);
			w = WS();
			next = STREAM();
			ax = shl32(S32(w, 0xF0), 16);
			ay = shl32(S32(w, 0xF4), 16);
			az = shl32(S32(w, 0xF8), 16);
			next += 8;
		}
		else
		{
			uint8_t *blk = PTR(b, 0xBC);
			uint8_t *wb = blob::GetBone(S16(blk, 0));
			uint8_t *oa = blob::ObjectPtr(S16(blk, 2));
			uint8_t *ob = blob::ObjectPtr(S16(blk, 4));
			uint8_t *vb = ob + U32(ob, 0x14);
			int32_t off = shl32(S16(STREAM(), 4), 3);
			uint8_t *va = oa + U32(oa, 0x14);
			vb += off;
			int32_t a = S16(va + off, 0);
			int32_t bv = S16(vb, 0);
			va += off;
			S32(WS(), 0xE0) = add32(SDivPow2(mul32(bv - a, S16(wb, 0x94)), 8), a);
			a = S16(va, 2);
			bv = S16(vb, 2);
			S32(WS(), 0xE4) = add32(SDivPow2(mul32(bv - a, S16(wb, 0x96)), 8), a);
			a = S16(va, 4);
			bv = S16(vb, 4);
			S32(WS(), 0xE8) = add32(SDivPow2(mul32(bv - a, S16(wb, 0x98)), 8), a);
			{
				uint8_t *w = WS();
				S32(w, 0xE0) = SDivPow2(shl32(mul32(S16(CUR(), 0x8C), S32(w, 0xE0)), 4), 12);
			}
			{
				uint8_t *w = WS();
				S32(w, 0xE4) = SDivPow2(shl32(mul32(S16(CUR(), 0x8E), S32(w, 0xE4)), 4), 12);
			}
			{
				uint8_t *w = WS();
				S32(w, 0xE8) = SDivPow2(shl32(mul32(S16(CUR(), 0x90), S32(w, 0xE8)), 4), 12);
			}
			uint8_t *par = blob::GetBone(U16(CUR(), 0x9C));
			uint8_t *w = WS();
			ax = add32(S16(par, 0x94), S32(w, 0xE0));
			ay = add32(S16(par, 0x96), S32(w, 0xE4));
			az = add32(S16(par, 0x98), S32(w, 0xE8));
			next = STREAM();
			ax = shl32(ax, 16);
			ay = shl32(ay, 16);
			az = shl32(az, 16);
			next += 6;
		}
		STREAM() = next;
		uint8_t *sv = PTR(WS(), 0x60);
		CUR() = sv;
		S32(sv, 0x5C) = ax;
		S32(CUR(), 0x60) = ay;
		S32(CUR(), 0x64) = az;
		C().bone[U8(CUR(), 0x18)]();
	}

	static void __cdecl op_090_OffscreenStageRender()
	{
		if (U16(RCTX(), 0x30) != 0)
		{
			STREAM() += 0xA;
			return;
		}
		U16(WS(), 0xD0) = U16(STREAM(), 2);
		U16(WS(), 0xD2) = U16(STREAM(), 4);
		U16(WS(), 0xD4) = U16(STREAM(), 6);
		U16(WS(), 0xD6) = U16(STREAM(), 8);
		uint8_t *d = PTR(CTX(), 0x74);
		U32(d, 0) = 0;
		U32(d, 4) = 0;
		U32(d, 0) = U32(CTX(), 0x74) + 0xEC;
		uint32_t sz = 0x5BEC;
		uint32_t pk = U32(CTX(), 0x74);
		U32(d, 0xC) = sz;
		U32(d, 8) = pk + 0x10EC;
		U32(CUR(), 0xC4) = sz;
		blob::ArenaAlloc(S32(d, 0xC));
		U32(d, 0x14) = U32(CTX(), 0x74);
		uint8_t *b = CUR();
		xl_OffscreenStageRender(d, b + 0x94, b + 0x8C);
		STREAM() += 0xA;
	}

	static void __cdecl op_096_CaptureScreenStrips()
	{
		S32(WS(), 0xF0) = S16(STREAM(), 2);
		S32(WS(), 0xF4) = S16(STREAM(), 4);
		CaptureScreenStrips();
		STREAM() += 6;
	}

	static void __cdecl op_0CF_TargetListFromChain()
	{
		uint32_t sub = (uint32_t)U16(RT(), 0x4A) >> 12;
		uint8_t *e;
		if (sub == 0)
		{
			uint8_t slot = U8(CUR(), 0x1B);
			uint8_t *sc = SCENE();
			e = PTR(sc, 0x60 + (int32_t)slot * 4);
			if (U32(e, 0x8C) == 0)
			{
				U8(sc, 0x17A) = 0;
				STREAM() += 2;
				return;
			}
			U8(sc, 0x174) = slot;
			U8(SCENE(), 0x17A) = 1;
		}
		else if (sub == 1)
		{
			uint32_t slot = U8(CUR(), 0x1B);
			uint8_t *sc = SCENE();
			e = PTR(sc, 0x60 + (int32_t)slot * 4);
			if (U32(e, 0x8C) == 0)
			{
				U8(sc, 0x17A) = 0;
				STREAM() += 2;
				return;
			}
			U8(sc, 0x17A) = U8(sc, 0x41);
			sc = SCENE();
			int32_t k = S8(sc, 0x17A);
			if (k > 0)
			{
				for (;;)
				{
					U8(sc, k + 0x173) = U8(sc, k + 0x47);
					k--;
					if (k <= 0) break;
					sc = SCENE();
				}
			}
		}
		else
			return;
		uint8_t *r = xl_ChainMinEntity(e);
		uint32_t d = (uint32_t)r - game_addr(BATTLE_ENTITIES_1D972C0);
		uint32_t hi = (uint32_t)(((uint64_t)d * 0xA41A41A5ull) >> 32);
		uint32_t q = (((d - hi) >> 1) + hi) >> 7;
		U8(CUR(), 0x1B) = (uint8_t)q;
		U8(SCENE(), 0x48) = (uint8_t)q;
		U8(SCENE(), 0x41) = 1;
		STREAM() += 2;
	}
}

	void apply_shared_misc(Clone &c)
	{
		auto live = [](uint32_t slot) -> bool
		{
			uint32_t orig = *(const uint32_t *)slot;
			return is_game_code(orig) && *(const uint8_t *)orig != 0xC3;
		};
		if (live(c.draw_table + 4 * 16)) c.draw[16] = part_shared_misc::dh_16_ScrollTextureU;
		if (live(c.draw_table + 4 * 26)) c.draw[26] = part_shared_misc::dh_26_ScreenCaptureStrips;
		if (live(c.draw_table + 4 * 39)) c.draw[39] = part_shared_misc::dh_39_Starfield;
		if (live(c.prim_table + 4 * 16)) c.prim[16] = part_shared_misc::pr_16_TintedF4;
		if (live(c.prim_table + 4 * 29)) c.prim[29] = part_shared_misc::pr_29_GT3;
		if (live(c.vm_table + 4 * 0x042)) c.vm[0x042] = part_shared_misc::op_042_EntityChainAnim;
		if (live(c.vm_table + 4 * 0x05C)) c.vm[0x05C] = part_shared_misc::op_05C_AccumFromMeshVertex;
		if (live(c.vm_table + 4 * 0x096)) c.vm[0x096] = part_shared_misc::op_096_CaptureScreenStrips;
		if (live(c.vm_table + 4 * 0x0CF)) c.vm[0x0CF] = part_shared_misc::op_0CF_TargetListFromChain;
		if ((c.effect_id == 205 || c.effect_id == 206 || c.effect_id == 222) && live(c.vm_table + 4 * 0x022))
			c.vm[0x022] = part_shared_misc::op_022_ApplyActionResultList;
		if ((c.effect_id == 202 || c.effect_id == 205) && live(c.vm_table + 4 * 0x090))
			c.vm[0x090] = part_shared_misc::op_090_OffscreenStageRender;
	}
}
}

