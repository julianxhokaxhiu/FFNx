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

// Effect 204: Alexander

#include "gfc_engine.h"

namespace ff8fx
{
namespace gfc
{
namespace part_alexander
{
	static constexpr game::Id BATTLE_DRAWENV_1D969C8 = game::g_Battle_DrawEnv;
	static inline uint8_t DISP_PARITY_1D96A80() { return MEM<uint8_t>(game_addr(game::g_Battle_DisplayParity)); }

	static inline void xl_SetDrawEnv_45C0F0(void *prim, const void *env) { x::f<void (__cdecl *)(void *, const void *)>(game_addr(game::SSIGPU_SetDrawEnv))(prim, env); }
	static inline void xl_InsertPrimAutoDepth_45C7A0(uint32_t ot, void *prim) { x::f<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAutoDepth))(ot, prim); }
	static inline void xl_Nop_B65150() { blob::f<int32_t (__cdecl *)()>(game_addr(game::xorEAX_6))(); }
	static inline void xl_ScaleColour_B66840() { blob::f<int32_t (__cdecl *)()>(game_addr(game::GfBlob_ScaleColourWsF0))(); }

	static inline int32_t div256(int32_t v) { return add32(v, (v >> 31) & 0xFF) >> 8; }
	static inline int32_t neg32(int32_t v) { return (int32_t)(0u - (uint32_t)v); }

	static void __cdecl dh_35_RenderToTexture()
	{
		if (PTR(CUR(), 0xBC) == nullptr)
		{
			uint8_t *b = blob::ArenaAlloc(0x100);
			PTR(CUR(), 0xBC) = b;
		}
		uint8_t *prm = PTR(CUR(), 0xB8);
		S32(WS(), 0x60) = S16(prm, 0);
		S32(WS(), 0x64) = S16(prm, 2);
		uint32_t idx = ((uint32_t)DISP_PARITY_1D96A80() - 1u) & 1u;
		uint8_t *env = PTR(CTX(), 0x74);
		const uint8_t *src = (const uint8_t *)(game_addr(BATTLE_DRAWENV_1D969C8) + idx * 23u * 4u);
		U32(env, 0) = U32(src, 0);
		U32(env, 4) = U32(src, 4);
		U32(env, 8) = U32(src, 8);
		U32(env, 0xC) = U32(src, 0xC);
		U32(env, 0x10) = U32(src, 0x10);
		U32(env, 0x14) = U32(src, 0x14);
		U32(env, 0x18) = U32(src, 0x18);
		uint8_t *prim;
		prim = PTR(CUR(), 0xBC) + shl32(S8(RT(), 0x41), 7);
		uint16_t vx = U16(WS(), 0x60);
		U16(env, 8) = vx;
		U16(env, 0) = vx;
		uint16_t vy = U16(WS(), 0x64);
		U16(env, 0xA) = vy;
		U16(env, 2) = vy;
		U32(env, 4) = 0xE00140;
		U32(env, 0x18) = 0x1000000;
		xl_SetDrawEnv_45C0F0(prim, env);
		uint8_t *back = prim + 0x40;
		xl_SetDrawEnv_45C0F0(back, src);
		uint32_t ot = U32(RT(), 0x4C) + 0x10;
		xl_InsertPrimAutoDepth_45C7A0(ot, back);
		ot += 0x4430;
		xl_InsertPrimAutoDepth_45C7A0(ot, prim);

		uint8_t *ws = WS();
		int32_t tp = (S32(ws, 0x60) >> 2) & 0xF0;
		tp |= S32(ws, 0x64) & 0x100;
		tp >>= 4;
		tp |= S16(CUR(), 0x92);
		tp |= 0x100;
		S32(ws, 0x80) = tp;
		uint8_t *pkt = PTR(CTX(), 0x7C);
		uint8_t *cur = CUR();
		int32_t x0 = S16(cur, 0x94);
		int32_t y0 = S16(cur, 0x96);
		S32(WS(), 0x68) = S16(cur, 0x8C);
		S32(WS(), 0x6C) = S16(CUR(), 0x8E);
		if (S32(WS(), 0x68) == 0x7654) return;
		int32_t i = 0;
		do
		{
			U8(pkt, 3) = 9;
			int32_t n = i + 1;
			U32(pkt, 4) = U32(CUR(), 0xCC) | 0x2C000000;
			U16(pkt, 0x16) = U16(WS(), 0x80);
			int32_t xa = add32(shl32(mul32(S32(WS(), 0x68), i), 6) >> 8, x0);
			U16(pkt, 0x18) = (uint16_t)xa;
			U16(pkt, 8) = (uint16_t)xa;
			int32_t w = S32(WS(), 0x68);
			U16(pkt, 0x12) = (uint16_t)y0;
			int32_t xb = shl32(mul32(w, n), 6) >> 8;
			U16(pkt, 0xA) = (uint16_t)y0;
			xb = add32(xb, x0);
			U16(pkt, 0x20) = (uint16_t)xb;
			U16(pkt, 0x10) = (uint16_t)xb;
			int32_t h = S32(WS(), 0x6C);
			U16(pkt, 0xC) = 0;
			U16(pkt, 0x14) = 0x40;
			U16(pkt, 0x1C) = 0xE000;
			int32_t yb = mul32(h, 8);
			U16(pkt, 0x24) = 0xE040;
			yb = sub32(yb, h);
			yb = shl32(yb, 5) >> 8;
			yb = add32(yb, y0);
			U16(pkt, 0x22) = (uint16_t)yb;
			U16(pkt, 0x1A) = (uint16_t)yb;
			xl_InsertPrimAutoDepth_45C7A0(U32(RT(), 0x4C) + 8, pkt);
			pkt += 0x28;
			U32(WS(), 0x80) = U32(WS(), 0x80) + 1;
			i = n;
		} while (i < 5);
		PTR(CTX(), 0x7C) = pkt;
	}

	static inline void Insert36(uint8_t *pkt) { xl_InsertPrimAutoDepth_45C7A0(U32(RT(), 0x4C) + 8, pkt); }

	static inline void Tail36_A(uint8_t *pkt, const uint8_t *e)
	{
		uint8_t v = (uint8_t)(U32(WS(), 0x6C) & 0xFF);
		U8(pkt, 0xD) = v;
		U8(pkt, 0x19) = (uint8_t)(U8(e, 0x90) + v);
		U8(pkt, 0x25) = (uint8_t)(U8(e, 0x92) + v);
		uint16_t x = U16(WS(), 0x68);
		U16(pkt, 8) = x;
		U16(pkt, 0x14) = (uint16_t)(U16(e, 0) + x);
		U16(pkt, 0x20) = (uint16_t)(x + U16(e, 2));
		uint16_t y = U16(WS(), 0x6C);
		U16(pkt, 0xA) = y;
		U16(pkt, 0x16) = (uint16_t)(U16(e, 0x30) + y);
		U16(pkt, 0x22) = (uint16_t)(U16(e, 0x32) + y);
		Insert36(pkt);
	}

	static inline void FullTriHead36(uint8_t *pkt, const uint8_t *e)
	{
		U8(pkt, 3) = 9;
		uint32_t col = U32(WS(), 0x8C) | 0x34000000;
		U32(pkt, 0x1C) = 0;
		U32(pkt, 0x10) = 0;
		U32(pkt, 4) = col;
		uint16_t x = U16(WS(), 0x68);
		U16(pkt, 8) = x;
		U16(pkt, 0x14) = (uint16_t)(U16(e, 0) + x);
		U16(pkt, 0x20) = (uint16_t)(x + U16(e, 2));
		uint16_t y = U16(WS(), 0x6C);
		U16(pkt, 0xA) = y;
		U16(pkt, 0x16) = (uint16_t)(U16(e, 0x30) + y);
		U16(pkt, 0x22) = (uint16_t)(U16(e, 0x32) + y);
	}

	static inline void Tail36_UV(uint8_t *pkt, const uint8_t *e, uint8_t ub)
	{
		U8(pkt, 0xC) = ub;
		U8(pkt, 0x18) = (uint8_t)(U8(e, 0x60) + ub);
		U8(pkt, 0x24) = (uint8_t)(U8(e, 0x62) + ub);
		uint8_t v = (uint8_t)U32(WS(), 0x94);
		U8(pkt, 0xD) = v;
		U8(pkt, 0x19) = (uint8_t)(U8(e, 0x90) + v);
		U8(pkt, 0x25) = (uint8_t)(U8(e, 0x92) + v);
		Insert36(pkt);
	}

	static inline void Tail36_B(uint8_t *pkt, uint8_t u)
	{
		U8(pkt, 0xC) = u;
		U16(pkt, 0xA) = U16(WS(), 0x6C);
		U16(pkt, 8) = U16(WS(), 0x68);
		Insert36(pkt);
	}

	static void __cdecl dh_36_ScreenLensDisc()
	{
		uint8_t *prm = PTR(CUR(), 0xB8);
		S32(WS(), 0x60) = S16(prm, 0);
		S32(WS(), 0x64) = S16(prm, 2);
		S32(WS(), 0x68) = S16(CUR(), 0x94);
		S32(WS(), 0x6C) = S16(CUR(), 0x96);
		S32(WS(), 0x70) = S16(CUR(), 0x8C);
		S32(WS(), 0x74) = S16(CUR(), 0x8E);
		uint8_t *ws = WS();
		S32(ws, 0x78) = mul32(S32(ws, 0x74), S32(ws, 0x70)) >> 8;
		U32(WS(), 0x8C) = U32(CUR(), 0xCC) & 0x2FFFFFF;
		ws = WS();
		U32(ws, 0x90) = U32(ws, 0x68) & 0x3F;
		ws = WS();
		U32(ws, 0x94) = U32(ws, 0x6C) & 0xFF;
		ws = WS();
		{
			int32_t tp = (add32(S32(ws, 0x68), S32(ws, 0x60)) >> 2) & 0xF0;
			tp |= S32(ws, 0x64) & 0x100;
			tp >>= 4;
			tp |= S16(CUR(), 0x92);
			tp |= 0x100;
			S32(ws, 0x80) = tp;
		}
		ws = WS();
		{
			int32_t a = add32(S32(ws, 0x68), S32(ws, 0x60));
			int32_t b = sub32(a, S32(ws, 0x78)) & 0xFFC0;
			a &= 0xFFC0;
			int32_t tp;
			if (a == b) tp = S32(ws, 0x80);
			else tp = sub32(S32(ws, 0x80), sub32(a, b) >> 6);
			S32(ws, 0x84) = tp;
		}
		xl_Nop_B65150();
		ws = WS();
		{
			int32_t d = sub32(S32(ws, 0x68), S32(ws, 0x78));
			uint32_t u = (uint32_t)d;
			if (d >= 0)
				u = 0u - (u & 0xFFC0);
			else
			{
				if ((u & 0x3F) == 0) u++;
				u = 0u - u;
				u &= 0xFFC0;
				u += 0x40;
			}
			U32(ws, 0x88) = u;
		}
		uint8_t *e = PTR(CTX(), 0x74);
		ws = WS();
		int32_t r1 = S32(ws, 0x70);
		int32_t r2 = mul32(S32(ws, 0x74), r1) >> 8;
		for (int32_t ang = 0; ang <= 0x1000; ang += 0x100)
		{
			S16(e, 0) = (int16_t)(mul32(x::Cos(ang), r1) >> 12);
			S16(e, 0x30) = (int16_t)(mul32(x::Sin(ang), r1) >> 12);
			S16(e, 0x60) = (int16_t)(mul32(x::Cos(ang), r2) >> 12);
			S16(e, 0x90) = (int16_t)(mul32(x::Sin(ang), r2) >> 12);
			e += 2;
		}
		uint8_t *pkt = PTR(CTX(), 0x7C);
		e = PTR(CTX(), 0x74);
		ws = WS();
		U32(ws, 0xF0) = U32(ws, 0x8C);
		for (int32_t i = 0; i < 0x10; i++, e += 2)
		{
			if (i < 4)
			{
				U8(pkt, 3) = 9;
				uint32_t col = U32(WS(), 0x8C) | 0x34000000;
				U32(pkt, 0x1C) = 0;
				U32(pkt, 0x10) = 0;
				U32(pkt, 4) = col;
				U16(pkt, 0x1A) = U16(WS(), 0x80);
				uint8_t u = (uint8_t)(U32(WS(), 0x68) & 0x3F);
				U8(pkt, 0xC) = u;
				U8(pkt, 0x18) = (uint8_t)(U8(e, 0x60) + u);
				U8(pkt, 0x24) = (uint8_t)(U8(e, 0x62) + u);
				Tail36_A(pkt, e);
				pkt += 0x28;
			}
			else if (i < 8)
			{
				U8(pkt, 3) = 9;
				uint32_t col = U32(WS(), 0x8C) | 0x34000000;
				U32(pkt, 0x1C) = 0;
				U32(pkt, 0x10) = 0;
				U32(pkt, 4) = col;
				U16(pkt, 0x1A) = U16(WS(), 0x84);
				ws = WS();
				uint8_t xl = (uint8_t)U32(ws, 0x68);
				U8(pkt, 0xC) = (uint8_t)(U8(ws, 0x88) + xl);
				U8(pkt, 0x18) = (uint8_t)(U8(WS(), 0x88) + U8(e, 0x60) + xl);
				U8(pkt, 0x24) = (uint8_t)(U8(WS(), 0x88) + U8(e, 0x62) + xl);
				Tail36_A(pkt, e);
				pkt += 0x28;
			}
			else if (i < 12)
			{
				U16(pkt, 0x1A) = U16(WS(), 0x84);
				ws = WS();
				int32_t v0 = S32(ws, 0x94);
				int32_t vB = add32(S16(e, 0x92), v0);
				if (vB >= 0)
				{
					FullTriHead36(pkt, e);
					ws = WS();
					Tail36_UV(pkt, e, (uint8_t)add32(S32(ws, 0x88), S32(ws, 0x68)));
					pkt += 0x28;
					continue;
				}
				int32_t vA = add32(S16(e, 0x90), v0);
				if (vA >= 0)
				{
					U8(pkt, 3) = 0xC;
					uint32_t col = U32(WS(), 0x8C);
					U8(pkt, 0x25) = 0;
					col |= 0x3C000000;
					int32_t den = sub32(v0, vB);
					int32_t num = shl32(v0, 8);
					U32(pkt, 4) = col;
					int32_t t = num / den;
					int32_t q = div256(mul32(S16(e, 0x62), t));
					ws = WS();
					U8(pkt, 0x24) = (uint8_t)((uint8_t)q + U8(ws, 0x88) + U8(ws, 0x68));
					q = div256(mul32(neg32(S16(e, 0x32)), t));
					U16(pkt, 0x22) = (uint16_t)(U16(WS(), 0x6C) - (uint16_t)q);
					q = div256(mul32(S16(e, 2), t));
					U16(pkt, 0x20) = (uint16_t)((uint16_t)q + U16(WS(), 0x68));
					S32(WS(), 0xF4) = sub32(0x100, t);
					xl_ScaleColour_B66840();
					U32(pkt, 0x1C) = U32(WS(), 0xFC);
					ws = WS();
					int32_t y2b = S16(e, 0x92);
					int32_t v0b = S32(ws, 0x94);
					int32_t vA2 = add32(S16(e, 0x90), v0b);
					U8(pkt, 0x19) = (uint8_t)vA2;
					int32_t den2 = sub32(sub32(vA2, y2b), v0b);
					int32_t t2 = shl32(vA2, 8) / den2;
					U8(pkt, 0x31) = 0;
					int32_t x2a = S16(e, 0x60);
					q = div256(mul32(sub32(S16(e, 0x62), x2a), t2));
					ws = WS();
					int32_t ua = add32(x2a, add32(S32(ws, 0x88), S32(ws, 0x68)));
					U8(pkt, 0x18) = (uint8_t)ua;
					U8(pkt, 0x30) = (uint8_t)((uint8_t)q + (uint8_t)ua);
					ws = WS();
					int32_t y1a = S16(e, 0x30);
					int32_t y1b = S16(e, 0x32);
					int32_t ya = add32(S32(ws, 0x6C), y1a);
					q = div256(mul32(sub32(y1a, y1b), t2));
					U16(pkt, 0x16) = (uint16_t)ya;
					U16(pkt, 0x2E) = (uint16_t)sub32(ya, q);
					int32_t x1a = S16(e, 0);
					q = div256(mul32(sub32(S16(e, 2), x1a), t2));
					int32_t xa = add32(x1a, S32(WS(), 0x68));
					U32(pkt, 0x28) = 0;
					U32(pkt, 0x10) = 0;
					U16(pkt, 0x14) = (uint16_t)xa;
					U16(pkt, 0x2C) = (uint16_t)add32(q, xa);
					U8(pkt, 0xD) = U8(WS(), 0x94);
					ws = WS();
					U8(pkt, 0xC) = (uint8_t)(U8(ws, 0x88) + U8(ws, 0x68));
					U16(pkt, 0xA) = U16(WS(), 0x6C);
					U16(pkt, 8) = U16(WS(), 0x68);
					Insert36(pkt);
					pkt += 0x34;
				}
				else
				{
					U8(pkt, 3) = 9;
					uint32_t col = U32(WS(), 0x8C);
					U8(pkt, 0x25) = 0;
					col |= 0x34000000;
					int32_t den = sub32(v0, vB);
					U32(pkt, 4) = col;
					int32_t num = shl32(v0, 8);
					int32_t t = num / den;
					int32_t q = div256(mul32(neg32(S16(e, 0x62)), t));
					uint8_t b = (uint8_t)(U8(WS(), 0x88) - (uint8_t)q);
					b = (uint8_t)(b + U8(WS(), 0x68));
					U8(pkt, 0x24) = b;
					q = div256(mul32(neg32(S16(e, 0x32)), t));
					U16(pkt, 0x22) = (uint16_t)(U16(WS(), 0x6C) - (uint16_t)q);
					q = div256(mul32(neg32(S16(e, 2)), t));
					U16(pkt, 0x20) = (uint16_t)(U16(WS(), 0x68) - (uint16_t)q);
					S32(WS(), 0xF4) = sub32(0x100, t);
					xl_ScaleColour_B66840();
					uint32_t c2 = U32(WS(), 0xFC);
					U8(pkt, 0x19) = 0;
					U32(pkt, 0x1C) = c2;
					den = sub32(v0, vA);
					t = num / den;
					q = div256(mul32(neg32(S16(e, 0x60)), t));
					b = (uint8_t)(U8(WS(), 0x88) - (uint8_t)q);
					b = (uint8_t)(b + U8(WS(), 0x68));
					U8(pkt, 0x18) = b;
					q = div256(mul32(neg32(S16(e, 0x30)), t));
					U16(pkt, 0x16) = (uint16_t)(U16(WS(), 0x6C) - (uint16_t)q);
					q = div256(mul32(neg32(S16(e, 0)), t));
					U16(pkt, 0x14) = (uint16_t)(U16(WS(), 0x68) - (uint16_t)q);
					S32(WS(), 0xF4) = sub32(0x100, t);
					xl_ScaleColour_B66840();
					U32(pkt, 0x10) = U32(WS(), 0xFC);
					U8(pkt, 0xD) = U8(WS(), 0x94);
					ws = WS();
					Tail36_B(pkt, (uint8_t)(U8(ws, 0x88) + U8(ws, 0x68)));
					pkt += 0x28;
				}
			}
			else
			{
				U16(pkt, 0x1A) = U16(WS(), 0x80);
				ws = WS();
				int32_t v0 = S32(ws, 0x94);
				int32_t vA = add32(S16(e, 0x90), v0);
				if (vA >= 0)
				{
					FullTriHead36(pkt, e);
					Tail36_UV(pkt, e, (uint8_t)U32(WS(), 0x90));
					pkt += 0x28;
					continue;
				}
				int32_t vB = add32(S16(e, 0x92), v0);
				if (vB >= 0)
				{
					U8(pkt, 3) = 0xC;
					uint32_t col = U32(WS(), 0x8C);
					U8(pkt, 0x19) = 0;
					col |= 0x3C000000;
					int32_t den = sub32(v0, vA);
					int32_t num = shl32(v0, 8);
					U32(pkt, 4) = col;
					int32_t t = num / den;
					int32_t q = div256(mul32(S16(e, 0x60), t));
					U8(pkt, 0x18) = (uint8_t)((uint8_t)q + U8(WS(), 0x90));
					q = div256(mul32(neg32(S16(e, 0x30)), t));
					U16(pkt, 0x16) = (uint16_t)(U16(WS(), 0x6C) - (uint16_t)q);
					q = div256(mul32(S16(e, 0), t));
					U16(pkt, 0x14) = (uint16_t)((uint16_t)q + U16(WS(), 0x68));
					S32(WS(), 0xF4) = sub32(0x100, t);
					xl_ScaleColour_B66840();
					U32(pkt, 0x10) = U32(WS(), 0xFC);
					ws = WS();
					int32_t y2b = S16(e, 0x92);
					int32_t v0b = S32(ws, 0x94);
					int32_t y2a = S16(e, 0x90);
					int32_t vB2 = add32(y2b, v0b);
					int32_t den2 = sub32(sub32(vB2, y2a), v0b);
					U8(pkt, 0x31) = 0;
					int32_t t2 = shl32(vB2, 8) / den2;
					U8(pkt, 0x25) = (uint8_t)vB2;
					ws = WS();
					int32_t x2a = S16(e, 0x60);
					int32_t ub = add32(S32(ws, 0x90), S16(e, 0x62));
					q = div256(mul32(sub32(S16(e, 0x62), x2a), t2));
					U8(pkt, 0x30) = (uint8_t)((uint8_t)ub - (uint8_t)q);
					U8(pkt, 0x24) = (uint8_t)ub;
					ws = WS();
					int32_t y1b = S16(e, 0x32);
					int32_t y1a = S16(e, 0x30);
					int32_t yb = add32(S32(ws, 0x6C), y1b);
					q = div256(mul32(sub32(y1b, y1a), t2));
					U16(pkt, 0x22) = (uint16_t)yb;
					U16(pkt, 0x2E) = (uint16_t)sub32(yb, q);
					ws = WS();
					int32_t x1b = S16(e, 2);
					int32_t x1a = S16(e, 0);
					int32_t xb = add32(S32(ws, 0x68), x1b);
					U32(pkt, 0x28) = 0;
					q = div256(mul32(sub32(x1b, x1a), t2));
					U16(pkt, 0x20) = (uint16_t)xb;
					U32(pkt, 0x1C) = 0;
					U16(pkt, 0x2C) = (uint16_t)sub32(xb, q);
					U8(pkt, 0xD) = U8(WS(), 0x94);
					U8(pkt, 0xC) = U8(WS(), 0x90);
					U16(pkt, 0xA) = U16(WS(), 0x6C);
					U16(pkt, 8) = U16(WS(), 0x68);
					Insert36(pkt);
					pkt += 0x34;
				}
				else
				{
					U8(pkt, 3) = 9;
					uint32_t col = U32(WS(), 0x8C);
					U8(pkt, 0x19) = 0;
					col |= 0x34000000;
					int32_t den = sub32(v0, vA);
					U32(pkt, 4) = col;
					int32_t num = shl32(v0, 8);
					int32_t t = num / den;
					int32_t q = div256(mul32(S16(e, 0x60), t));
					U8(pkt, 0x18) = (uint8_t)((uint8_t)q + U8(WS(), 0x90));
					q = div256(mul32(neg32(S16(e, 0x30)), t));
					U16(pkt, 0x16) = (uint16_t)(U16(WS(), 0x6C) - (uint16_t)q);
					q = div256(mul32(S16(e, 0), t));
					U16(pkt, 0x14) = (uint16_t)((uint16_t)q + U16(WS(), 0x68));
					S32(WS(), 0xF4) = sub32(0x100, t);
					xl_ScaleColour_B66840();
					uint32_t c1 = U32(WS(), 0xFC);
					U8(pkt, 0x25) = 0;
					U32(pkt, 0x10) = c1;
					den = sub32(v0, vB);
					t = num / den;
					q = div256(mul32(S16(e, 0x62), t));
					U8(pkt, 0x24) = (uint8_t)((uint8_t)q + U8(WS(), 0x90));
					q = div256(mul32(neg32(S16(e, 0x32)), t));
					U16(pkt, 0x22) = (uint16_t)(U16(WS(), 0x6C) - (uint16_t)q);
					q = div256(mul32(S16(e, 2), t));
					U16(pkt, 0x20) = (uint16_t)((uint16_t)q + U16(WS(), 0x68));
					S32(WS(), 0xF4) = sub32(0x100, t);
					xl_ScaleColour_B66840();
					U32(pkt, 0x1C) = U32(WS(), 0xFC);
					U8(pkt, 0xD) = U8(WS(), 0x94);
					Tail36_B(pkt, U8(WS(), 0x90));
					pkt += 0x28;
				}
			}
		}
		PTR(CTX(), 0x7C) = pkt;
	}
}
	void apply_alexander(Clone &c)
	{
		c.draw[35] = part_alexander::dh_35_RenderToTexture;
		c.draw[36] = part_alexander::dh_36_ScreenLensDisc;
	}
}
}

namespace ff8fx
{
namespace gfc
{
namespace alexander
{
	static Clone g_c = {};

	static void describe(Clone &c)
	{
		c.name = "204 Alexander";
		c.effect_id = 204;
		c.vm_table = game_addr(game::GF_204Alexander_VmOpcodeTable);
		c.bone_table = game_addr(game::GF_204Alexander_BoneHandlerTable);
		c.prim_table = game_addr(game::funcs_B04474);
		c.prim_size = game_addr(game::unk_1872768);
		c.spr_table = game_addr(game::unk_187277C);
		c.ptype_table = game_addr(game::unk_18727A8);
		c.pop_table = game_addr(game::funcs_B06227);
		c.draw_table = game_addr(game::GF_204Alexander_DrawHandlerTable);
		c.attr_16A = game_addr(game::unk_1872986);
		c.attr_184 = game_addr(game::unk_18729A0);
		c.attr_186 = game_addr(game::unk_18729A2);
		c.attr_194 = game_addr(game::unk_18729B0);
		c.attr_19C = game_addr(game::unk_18729B8);
		c.desc = game_addr(game::unk_1872384);
		c.ptr_block = game_addr(game::unk_1872608);
		c.queue = game_addr(game::unk_2796D70);
		c.file00 = game_addr(game::unk_2796DA4);
		c.file01 = game_addr(game::unk_2796DA0);
		c.tick = game_addr(game::GF_204Alexander_SequenceTick);
		c.load_cb = game_addr(game::MAG_204_sub_B07C20);
		c.lit_dispatcher = false;
		c.bone_count = 18;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *)
	{
		return SequenceTick(g_c);
	}
}
}

	void register_gfc_alexander()
	{
		using namespace gfc;
		alexander::describe(alexander::g_c);
		init_clone(alexander::g_c, nullptr, 0);
		apply_alexander(alexander::g_c);
		apply_shared_ribbon(alexander::g_c);
		apply_shared_misc(alexander::g_c);
		register_port(game::GF_204Alexander_SequenceTick, (void *)alexander::SequenceTask, "GFC204 Alexander SequenceTick", 204);
	}
}

