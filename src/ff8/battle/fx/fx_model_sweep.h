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

// Model sweep shared by several effects

#pragma once

#include "mag_common.h"

namespace ff8fx
{
namespace msweep
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &FrameArena() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t OT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	inline void MusicVolumeFade(int32_t v) { fn<void (__cdecl *)(int32_t)>(game_addr(game::Battle_SetMusicVolumeUnlessEncounterFlag2))(v); }
	inline uint32_t DrawShadow(uint8_t *entity, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, mode, cursor); }
	inline void ReleaseVoice(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
	inline void ColourLerp(void *a, uint32_t b, int32_t wa, int32_t wb, void *out) { fn<void (__cdecl *)(void *, uint32_t, int32_t, int32_t, void *)>(game_addr(game::ColourLerp))(a, b, wa, wb, out); }
	inline void GteLoadV0u16(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
	inline void GteStoreIR123Minus8(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123_Minus8))(p); }
	inline void GteSetSXY012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSXY012))(a, b, c); }
	inline void GteSetSZ123(int32_t a, int32_t b, int32_t c) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetSZ123))(a, b, c); }
	inline void GteSetSZ0123(int32_t a, int32_t b, int32_t c, int32_t d) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetSZ0123))(a, b, c, d); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZWord(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteLoadIRFromMatrixColumn(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIRFromMatrixColumn))(m); }
	inline void GteMVMVA_RotIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotIR))(); }
	inline void GteStoreIRToMatrixColumn(void *m) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIRToMatrixColumn))(m); }
	inline void GteLoadV0Dwords(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0FromDwords))(v); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }

	template<typename T> static inline T &F(uint8_t *p, uint32_t off) { return *(T *)(p + off); }
	template<typename T> static inline T F(const uint8_t *p, uint32_t off) { return *(const T *)(p + off); }
	static inline uint32_t Grey(uint8_t c) { return (uint32_t)c | ((uint32_t)c << 8) | ((uint32_t)c << 16); }
	static void ShowPolys(uint8_t *p)
	{
		uint8_t *poly = *(uint8_t **)p;
		int16_t c = *(int16_t *)(p + 4);
		*(int16_t *)(p + 4) = (int16_t)(c - 1);
		while (c != 0)
		{
			*(uint16_t *)(poly + 0xE) &= 0x7FFF;
			poly += 0x10;
			c = *(int16_t *)(p + 4);
			*(int16_t *)(p + 4) = (int16_t)(c - 1);
		}
		c = *(int16_t *)(p + 6);
		*(int16_t *)(p + 6) = (int16_t)(c - 1);
		if (c != 0)
		{
			poly += 0xE;
			do
			{
				*(uint16_t *)poly &= 0x7FFF;
				c = *(int16_t *)(p + 6);
				poly += 0x14;
				*(int16_t *)(p + 6) = (int16_t)(c - 1);
			} while (c != 0);
		}
	}

	static void ShowModelPolys(const uint8_t *model, uint8_t *p)
	{
		const uint32_t *tab = *(const uint32_t *const *)(model + 4);
		const int32_t count = (int32_t)*tab++;
		if (count <= 0) return;
		for (int32_t n = count; n != 0; n--)
		{
			uint8_t *o = (uint8_t *)(*tab++ + *(const uint32_t *)(model + 4));
			const int32_t groups = *(const int16_t *)o;
			o += 2;
			if (groups > 0)
			{
				for (int32_t g = groups; g != 0; g--)
				{
					const int32_t nv = *(const int16_t *)(o + 2);
					o += 2;
					o = o + nv * 6 + 2;
				}
			}
			o = (uint8_t *)(((uint32_t)o + 3) & ~3u);
			*(uint16_t *)(p + 4) = *(const uint16_t *)o;
			o += 2;
			*(uint16_t *)(p + 6) = *(const uint16_t *)o;
			o += 0xA;
			*(uint8_t **)p = o;
			ShowPolys(p);
		}
	}

	static void ShowAllPolys(uint8_t *entity)
	{
		uint8_t *p = (uint8_t *)FieldAlloc(8);
		ShowModelPolys(*(const uint8_t *const *)(entity + 0x64), p);
		FieldFree(8);
	}

	static uint8_t GreyOf(int16_t y, int16_t level)
	{
		int32_t g = (((int32_t)y - (int32_t)level) >> 2) + 0x40;
		if ((int16_t)g < 0) g = 0;
		else if ((int16_t)g > 0x80) g = 0x80;
		return (uint8_t)g;
	}

	static void ProjectA(uint8_t *h)
	{
		const uint8_t *data = F<uint8_t *>(h, 0x54);
		uint8_t *out = F<uint8_t *>(h, 0x58);
		const int32_t n = *(const int16_t *)data;
		data += 2;
		if (n != 0)
		{
			for (int32_t k = n; k != 0; k--)
			{
				GteLoadV0u16(data);
				GteMVMVA_RotV0Tr();
				out += 8;
				data += 6;
				GteStoreIR123Minus8(out);
			}
		}
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		out = F<uint8_t *>(h, 0x58);
		if (n != 0)
		{
			for (int32_t k = n; k != 0; k--)
			{
				GteLoadV0(out);
				GteRTPS();
				const uint8_t g = GreyOf(*(const int16_t *)(out + 2), F<int16_t>(h, 0x4C));
				GteReadSXY2(h + 0x1C);
				GteReadOTZ(h + 0x20);
				if (F<int16_t>(h, 0x20) > 0)
				{
					h[0x22] = g;
					*(uint32_t *)out = F<uint32_t>(h, 0x1C);
					*(uint32_t *)(out + 4) = F<uint32_t>(h, 0x20);
				}
				else *(uint16_t *)(out + 4) = 0;
				out += 8;
			}
		}
		F<const uint8_t *>(h, 0x54) = data;
		F<uint8_t *>(h, 0x58) = out;
	}

	static void PolysA(uint8_t *h, uint32_t ot, int32_t shift)
	{
		const uint8_t *verts = F<const uint8_t *>(h, 4);
		const uint8_t *p = F<const uint8_t *>(h, 0);
		uint8_t *pk = F<uint8_t *>(h, 0x5C);
		for (uint32_t i = 0; i < F<uint16_t>(h, 8); i++, p += 0x10)
		{
			const uint32_t i0 = *(const uint16_t *)p & 0xFFF, i1 = *(const uint16_t *)(p + 2) & 0xFFF, i2 = *(const uint16_t *)(p + 4) & 0xFFF;
			F<uint32_t>(h, 0x1C) = *(const uint32_t *)(verts + i0 * 8);
			F<uint32_t>(h, 0x20) = *(const uint32_t *)(verts + i0 * 8 + 4);
			F<uint32_t>(h, 0x24) = *(const uint32_t *)(verts + i1 * 8);
			F<uint32_t>(h, 0x28) = *(const uint32_t *)(verts + i1 * 8 + 4);
			F<uint32_t>(h, 0x2C) = *(const uint32_t *)(verts + i2 * 8);
			F<uint32_t>(h, 0x30) = *(const uint32_t *)(verts + i2 * 8 + 4);
			if ((int16_t)(F<uint16_t>(h, 0x28) | F<uint16_t>(h, 0x30) | F<uint16_t>(h, 0x20)) <= 0) continue;
			GteSetSXY012(F<uint32_t>(h, 0x1C), F<uint32_t>(h, 0x24), F<uint32_t>(h, 0x2C));
			GteNCLIP();
			GteReadMAC0(h + 0x3C);
			if (F<int32_t>(h, 0x3C) < 0) continue;
			GteSetSZ123(F<int16_t>(h, 0x20), F<int16_t>(h, 0x28), F<int16_t>(h, 0x30));
			GteAVSZ3();
			GteReadOTZWord(h + 0x44);
			const uint32_t bucket = ot + (uint32_t)(F<int32_t>(h, 0x44) >> (shift & 31)) * 4;
			const uint8_t c0 = h[0x22], c1 = h[0x2A], c2 = h[0x32];
			if ((uint8_t)(c0 | c1 | c2) == 0 || (uint8_t)(c0 & c1 & c2) == 0x80)
			{
				F<uint32_t>(pk, 0) = 0x7000000;
				F<uint32_t>(pk, 4) = F<uint32_t>(h, 0x14) | 0x24000000;
				F<uint32_t>(pk, 8) = F<uint32_t>(h, 0x1C);
				F<uint32_t>(pk, 0x10) = F<uint32_t>(h, 0x24);
				F<uint32_t>(pk, 0x18) = F<uint32_t>(h, 0x2C);
				F<uint32_t>(pk, 0xC) = *(const uint32_t *)(p + 8);
				F<uint32_t>(pk, 0x14) = *(const uint32_t *)(p + 0xC);
				F<uint16_t>(pk, 0x1C) = *(const uint16_t *)(p + 6);
				if ((uint8_t)(c0 | c1 | c2) != 0)
				{
					F<int16_t>(pk, 0x16) = (int16_t)(F<int16_t>(pk, 0x16) + F<int16_t>(h, 0x4E));
					F<int16_t>(pk, 0xE) = (int16_t)(F<int16_t>(pk, 0xE) + F<int16_t>(h, 0x50));
				}
				InsertPrimAutoDepth(bucket, pk);
				pk += 0x20;
				continue;
			}
			uint8_t *q = F<uint8_t *>(h, 0x60);
			uint8_t *g = pk;
			pk += 0x28;
			F<uint8_t *>(h, 0x60) = q + 0x28;
			F<uint32_t>(q, 0) = 0x9000000;
			F<uint32_t>(g, 0) = 0x9000000;
			F<uint32_t>(q, 8) = F<uint32_t>(g, 8) = F<uint32_t>(h, 0x1C);
			F<uint32_t>(q, 0x14) = F<uint32_t>(g, 0x14) = F<uint32_t>(h, 0x24);
			F<uint32_t>(q, 0x20) = F<uint32_t>(g, 0x20) = F<uint32_t>(h, 0x2C);
			F<uint32_t>(q, 0xC) = F<uint32_t>(g, 0xC) = *(const uint32_t *)(p + 8);
			F<uint32_t>(q, 0x18) = F<uint32_t>(g, 0x18) = *(const uint32_t *)(p + 0xC);
			F<uint16_t>(q, 0x24) = F<uint16_t>(g, 0x24) = *(const uint16_t *)(p + 6);
			F<uint32_t>(g, 4) = Grey(c0) | 0x34000000;
			F<uint32_t>(q, 4) = 0x36808080 - Grey(c0);
			F<uint32_t>(g, 0x10) = Grey(c1);
			F<uint32_t>(q, 0x10) = 0x808080 - Grey(c1);
			F<uint32_t>(g, 0x1C) = Grey(c2);
			F<uint32_t>(q, 0x1C) = 0x808080 - Grey(c2);
			InsertPrimAutoDepth(bucket, q);
			F<int16_t>(g, 0x1A) = (int16_t)(F<int16_t>(g, 0x1A) + F<int16_t>(h, 0x4E));
			F<int16_t>(g, 0xE) = (int16_t)(F<int16_t>(g, 0xE) + F<int16_t>(h, 0x50));
			InsertPrimAutoDepth(bucket, g);
		}
		p += 8;
		for (uint32_t i = 0; i < F<uint16_t>(h, 0xA); i++, p += 0x14)
		{
			const uint32_t i0 = *(const uint16_t *)(p - 8) & 0xFFF, i1 = *(const uint16_t *)(p - 6) & 0xFFF;
			const uint32_t i2 = *(const uint16_t *)(p - 4) & 0xFFF, i3 = *(const uint16_t *)(p - 2) & 0xFFF;
			F<uint32_t>(h, 0x1C) = *(const uint32_t *)(verts + i0 * 8);
			F<uint32_t>(h, 0x20) = *(const uint32_t *)(verts + i0 * 8 + 4);
			F<uint32_t>(h, 0x24) = *(const uint32_t *)(verts + i1 * 8);
			F<uint32_t>(h, 0x28) = *(const uint32_t *)(verts + i1 * 8 + 4);
			F<uint32_t>(h, 0x2C) = *(const uint32_t *)(verts + i2 * 8);
			F<uint32_t>(h, 0x30) = *(const uint32_t *)(verts + i2 * 8 + 4);
			F<uint32_t>(h, 0x34) = *(const uint32_t *)(verts + i3 * 8);
			F<uint32_t>(h, 0x38) = *(const uint32_t *)(verts + i3 * 8 + 4);
			if ((int16_t)(F<uint16_t>(h, 0x28) | F<uint16_t>(h, 0x38) | F<uint16_t>(h, 0x30) | F<uint16_t>(h, 0x20)) <= 0) continue;
			GteSetSXY012(F<uint32_t>(h, 0x1C), F<uint32_t>(h, 0x24), F<uint32_t>(h, 0x2C));
			GteNCLIP();
			GteReadMAC0(h + 0x3C);
			if (F<int32_t>(h, 0x3C) < 0) continue;
			GteSetSZ0123(F<int16_t>(h, 0x20), F<int16_t>(h, 0x28), F<int16_t>(h, 0x30), F<int16_t>(h, 0x38));
			GteAVSZ4();
			GteReadOTZWord(h + 0x44);
			const uint32_t bucket = ot + (uint32_t)(F<int32_t>(h, 0x44) >> (shift & 31)) * 4;
			const uint8_t c0 = h[0x22], c1 = h[0x2A], c2 = h[0x32];
			if ((uint8_t)(c0 | c1 | c2) == 0 || (uint8_t)(c0 & c1 & c2) == 0x80)
			{
				F<uint32_t>(pk, 0) = 0x9000000;
				F<uint32_t>(pk, 4) = F<uint32_t>(h, 0x14) | 0x2C000000;
				F<uint32_t>(pk, 8) = F<uint32_t>(h, 0x1C);
				F<uint32_t>(pk, 0x10) = F<uint32_t>(h, 0x24);
				F<uint32_t>(pk, 0x18) = F<uint32_t>(h, 0x2C);
				F<uint32_t>(pk, 0x20) = F<uint32_t>(h, 0x34);
				F<uint32_t>(pk, 0xC) = *(const uint32_t *)p;
				F<uint32_t>(pk, 0x14) = *(const uint32_t *)(p + 4);
				F<uint16_t>(pk, 0x1C) = *(const uint16_t *)(p + 8);
				F<uint16_t>(pk, 0x24) = *(const uint16_t *)(p + 0xA);
				if ((uint8_t)(c0 | c1 | c2) != 0)
				{
					F<int16_t>(pk, 0x16) = (int16_t)(F<int16_t>(pk, 0x16) + F<int16_t>(h, 0x4E));
					F<int16_t>(pk, 0xE) = (int16_t)(F<int16_t>(pk, 0xE) + F<int16_t>(h, 0x50));
				}
				InsertPrimAutoDepth(bucket, pk);
				pk += 0x28;
				continue;
			}
			uint8_t *q = F<uint8_t *>(h, 0x60);
			uint8_t *g = pk;
			pk += 0x34;
			F<uint8_t *>(h, 0x60) = q + 0x34;
			F<uint32_t>(q, 0) = 0xC000000;
			F<uint32_t>(g, 0) = 0xC000000;
			F<uint32_t>(q, 8) = F<uint32_t>(g, 8) = F<uint32_t>(h, 0x1C);
			F<uint32_t>(q, 0x14) = F<uint32_t>(g, 0x14) = F<uint32_t>(h, 0x24);
			F<uint32_t>(q, 0x20) = F<uint32_t>(g, 0x20) = F<uint32_t>(h, 0x2C);
			F<uint32_t>(q, 0x2C) = F<uint32_t>(g, 0x2C) = F<uint32_t>(h, 0x34);
			F<uint32_t>(q, 0xC) = F<uint32_t>(g, 0xC) = *(const uint32_t *)p;
			F<uint32_t>(q, 0x18) = F<uint32_t>(g, 0x18) = *(const uint32_t *)(p + 4);
			F<uint16_t>(q, 0x24) = F<uint16_t>(g, 0x24) = *(const uint16_t *)(p + 8);
			F<uint16_t>(q, 0x30) = F<uint16_t>(g, 0x30) = *(const uint16_t *)(p + 0xA);
			F<uint32_t>(g, 4) = Grey(c0) | 0x3C000000;
			F<uint32_t>(q, 4) = 0x3E808080 - Grey(c0);
			F<uint32_t>(g, 0x10) = Grey(c1);
			F<uint32_t>(q, 0x10) = 0x808080 - Grey(c1);
			F<uint32_t>(g, 0x1C) = Grey(c2);
			F<uint32_t>(q, 0x1C) = 0x808080 - Grey(c2);
			F<uint32_t>(g, 0x28) = Grey(h[0x3A]);
			F<uint32_t>(q, 0x28) = 0x808080 - Grey(h[0x3A]);
			InsertPrimAutoDepth(bucket, q);
			F<int16_t>(g, 0x1A) = (int16_t)(F<int16_t>(g, 0x1A) + F<int16_t>(h, 0x4E));
			F<int16_t>(g, 0xE) = (int16_t)(F<int16_t>(g, 0xE) + F<int16_t>(h, 0x50));
			InsertPrimAutoDepth(bucket, g);
		}
		F<uint8_t *>(h, 0x5C) = pk;
	}

	static void ModelA(const uint8_t *model, uint8_t *h, uint32_t ot, int32_t shift)
	{
		const uint32_t *objs = *(const uint32_t *const *)(model + 4);
		const uint8_t *bones = *(const uint8_t *const *)model + 0x10;
		const int32_t count = (int32_t)objs[0];
		for (int32_t i = 0; i < count; i++)
		{
			F<const uint8_t *>(h, 0x54) = *(const uint8_t *const *)(model + 4) + objs[1 + i];
			if (!(F<uint32_t>(h, 0x18) & (1u << (i & 31)))) continue;
			F<uint32_t>(h, 0x58) = F<uint32_t>(h, 4);
			const uint8_t *p = F<const uint8_t *>(h, 0x54);
			const int32_t groups = *(const int16_t *)p;
			F<const uint8_t *>(h, 0x54) = p + 2;
			if (groups > 0)
			{
				for (int32_t g = groups; g != 0; g--)
				{
					const uint8_t *q = F<const uint8_t *>(h, 0x54);
					const int32_t bone = *(const int16_t *)q;
					F<const uint8_t *>(h, 0x54) = q + 2;
					const Mat4x3 *bm = (const Mat4x3 *)(bones + mul32(bone, 0x30) + 0x10);
					GteSetRotMatrixCtrl(bm);
					GteSetTransVectorCtrl(bm);
					ProjectA(h);
				}
			}
			p = (const uint8_t *)((F<uint32_t>(h, 0x54) + 3) & ~3u);
			F<uint16_t>(h, 8) = *(const uint16_t *)p;
			F<uint16_t>(h, 0xA) = *(const uint16_t *)(p + 2);
			F<const uint8_t *>(h, 0x54) = p + 0xC;
			F<const uint8_t *>(h, 0) = p + 0xC;
			PolysA(h, ot, shift);
		}
	}

	static void SweepA(uint8_t *ent, int16_t level, int16_t page, int16_t clut, uint32_t &cursor)
	{
		FrameArena() = DrawShadow(ent, OT(0x4040), 0x10, FrameArena());
		uint8_t *b = (uint8_t *)FieldAlloc(0x84);
		uint8_t *anim = ent + 0x60;
		F<int16_t>(b, 0x6C) = level;
		F<int16_t>(b, 0x6E) = page;
		F<int16_t>(b, 0x70) = clut;
		ComputeBonesWorldMatrices(anim, ent + 0x40);
		F<int16_t>(b, 0x30) = 0x140;
		F<uint32_t>(b, 0x24) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		F<int16_t>(b, 0x2C) = 0;
		F<int16_t>(b, 0x2E) = 0;
		F<int16_t>(b, 0x32) = 0xD8;
		F<uint32_t>(b, 0x34) = F<uint32_t>(ent, 0x28);
		F<uint32_t>(b, 0x38) = F<uint32_t>(ent, 0x7C);
		F<uint32_t>(b, 0x7C) = FrameArena();
		F<uint32_t>(b, 0x80) = cursor;
		ModelA(F<const uint8_t *>(anim, 4), b + 0x20, OT(0x44), 0);
		FrameArena() = F<uint32_t>(b, 0x7C);
		cursor = F<uint32_t>(b, 0x80);
		BuildBoneMatricesFromPose(anim);
		FieldFree(0x84);
	}

	static uint32_t DrawSprites(uint8_t *h, uint32_t ot, int32_t slot, uint32_t cursor, bool raw_colour)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const int32_t count = F<int32_t>(h, 0x30);
		const uint8_t *f = F<const uint8_t *>(h, 0x2C);
		if (count <= 0) return cursor;
		for (int32_t n = count; n != 0; n--, f += 0x14)
		{
			F<int16_t>(h, 0x72) = 0;
			F<int16_t>(h, 0x70) = 0;
			F<int16_t>(h, 0x6E) = 0;
			F<int16_t>(h, 0x6A) = 0;
			F<int16_t>(h, 0x68) = 0;
			F<int16_t>(h, 0x66) = 0;
			F<int16_t>(h, 0x74) = 0x1000;
			F<uint16_t>(h, 0x64) = F<uint16_t>(f, 0x10);
			const int32_t page = F<int16_t>(f, 0xE);
			F<int16_t>(h, 0x6A) = 0;
			F<int16_t>(h, 0x66) = 0;
			F<uint16_t>(h, 0x6C) = F<uint16_t>(f, 0x12);
			const int32_t w = f[0];
			const int32_t ht = f[2];
			const int32_t ex = w << 3, ey = ht << 3;
			F<int16_t>(h, 0x54) = F<int16_t>(h, 0x44) = (int16_t)-ex;
			F<int16_t>(h, 0x5C) = F<int16_t>(h, 0x4C) = (int16_t)ex;
			F<int16_t>(h, 0x4E) = F<int16_t>(h, 0x46) = (int16_t)-ey;
			F<int16_t>(h, 0x5E) = F<int16_t>(h, 0x56) = (int16_t)ey;
			F<int32_t>(h, 0x78) = shl32(F<int16_t>(f, 8), 4) + ex;
			F<int32_t>(h, 0x80) = 0;
			F<int32_t>(h, 0x7C) = shl32(F<int16_t>(f, 0xA), 4) + ey;
			GteSetRotMatrixCtrl((const Mat4x3 *)(h + 0x84));
			GteLoadIRFromMatrixColumn(h + 0x64);
			GteMVMVA_RotIR();
			F<uint32_t>(pk, 0) = 0x9000000;
			if (raw_colour) F<uint32_t>(pk, 4) = F<uint32_t>(f, 0);
			else
			{
				pk[4] = f[1];
				pk[5] = f[1];
				pk[6] = f[1];
				pk[7] = f[3];
			}
			GteStoreIRToMatrixColumn(h + 0x64);
			GteLoadIRFromMatrixColumn(h + 0x66);
			GteMVMVA_RotIR();
			F<uint32_t>(pk, 0xC) = F<uint32_t>(f, 4);
			GteStoreIRToMatrixColumn(h + 0x66);
			GteLoadIRFromMatrixColumn(h + 0x68);
			GteMVMVA_RotIR();
			pk[0x1C] = f[4];
			GteStoreIRToMatrixColumn(h + 0x68);
			GteSetTransVectorCtrl((const Mat4x3 *)(h + 0x84));
			GteLoadV0Dwords(h + 0x78);
			GteMVMVA_RotV0Tr();
			pk[0x15] = f[5];
			GteReadMAC123((int32_t *)(h + 0x78));
			GteSetRotMatrixCtrl((const Mat4x3 *)(h + 0x64));
			GteSetTransVectorCtrl((const Mat4x3 *)(h + 0x64));
			GteLoadV012(h + 0x44, h + 0x4C, h + 0x54);
			GteRTPT();
			int32_t u = (int32_t)f[4] + w;
			if (u >= 0x100) u = 0xFF;
			pk[0x24] = (uint8_t)u;
			pk[0x14] = (uint8_t)u;
			GteReadFLAG(h + 0x40);
			if (F<uint32_t>(h, 0x40) & 0x60000) continue;
			F<uint16_t>(pk, 0x16) = (uint16_t)(page & 0x1FF);
			GteReadSXY012Split(pk + 8, pk + 0x10, pk + 0x18);
			GteLoadV0(h + 0x5C);
			GteRTPS();
			int32_t v = (int32_t)f[5] + ht;
			if (v >= 0x100) v = 0xFF;
			pk[0x25] = (uint8_t)v;
			pk[0x1D] = (uint8_t)v;
			GteReadSXY2(pk + 0x20);
			InsertPrimAutoDepth(ot + (uint32_t)slot * 4, pk);
			pk += 0x28;
		}
		return (uint32_t)pk;
	}
}
}
