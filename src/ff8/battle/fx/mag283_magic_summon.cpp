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

// Effect 283: Magic Summon

#include "mag_common.h"

namespace ff8fx
{
namespace magicsummon283
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_283_MagicSummon_CastContext)); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_283_MagicSummon_MainQueue); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_283_MagicSummon_PacketCursor)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_283_MagicSummon_MagicBufferBase)); }
	static const Mat4x3 *MATRIX_Flag1000() { return (const Mat4x3 *)game_addr(game::MAG_283_MagicSummon_Matrix); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t &BoneWorkspace() { return var<uint32_t>(game_addr(game::g_BattleVertexWorkspace)); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_MasterTask = game::MAG_283_MagicSummon_MasterTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_283_MagicSummon_MainTask;
	static constexpr game::Id ORIG_PrimCallback = game::MAG_283_MagicSummon_PrimCallback;
	static const uint32_t MORPH = 0x648;
	static const uint32_t ARENA_A = 0x650, ARENA_B = 0x10650;
	static constexpr game::Id MODEL_A = game::MAG_283_MagicSummon_ModelA;
	static constexpr game::Id MODEL_B = game::MAG_283_MagicSummon_ModelB;
	static constexpr game::Id FLASH_Colours = game::unk_109968C;
	static constexpr game::Id WARP_Weights = game::unk_10996AC;
	static constexpr game::Id WARP_Distance = game::MAG_283_WarpDistanceTable;
	static const void *SOUND_Summon() { return (const void *)game_addr(game::MAG_283_MagicSummon_SoundSummon); }

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrimSet(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_063_RenderPrimSet))(h, ot, mode, cursor); }
	inline void BlendColour(const void *a, const void *b, int32_t wa, int32_t wb, void *out) { fn<void (__cdecl *)(const void *, const void *, int32_t, int32_t, void *)>(game_addr(game::ColourLerp))(a, b, wa, wb, out); }
	inline void GteLoadIRBytes(const void *src) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIR123FromBytes))(src); }
	inline void GteStoreIRBytes(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123LowBytes))(dst); }
	inline uint32_t DrawShadow(uint8_t *entity, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, mode, cursor); }
	inline void GteLoadV0u(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
	inline void GteSetSXY012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSXY012))(a, b, c); }
	inline void GteSetSZ123(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSZ123))(a, b, c); }
	inline void GteSetSZ0123(uint32_t a, uint32_t b, uint32_t c, uint32_t d) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSZ0123))(a, b, c, d); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZWord(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t radius;
		int32_t size;
		uint32_t colour;
		int16_t anchor[4];
		int16_t pos[4];
		uint8_t *ent;
		uint8_t layoutA[0x4BC];
		uint8_t layoutB[0x160];
	};
	struct PrimArg
	{
		int16_t anchor[4];
		int32_t scale[3];
		int32_t scaled;
		int16_t depth;
		int16_t scroll;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(MainNode) == 0x648, "Magic Summon nodes");
	static_assert(sizeof(PrimArg) == 0x20, "Magic Summon prim block");

	template<typename T> static inline T &F(uint8_t *s, uint32_t off) { return *(T *)(s + off); }

	static void WarpVertices(uint8_t **objp, uint8_t **outp, uint8_t *s)
	{
		uint8_t *p = *objp;
		uint8_t *out = *outp;
		const int32_t n = *(const int16_t *)p;
		p += 2;
		if (n == 0)
		{
			*objp = p;
			*outp = out;
			return;
		}
		uint32_t left = (uint32_t)n;
		do
		{
			GteLoadV0u(p);
			GteRTPS();
			p += 6;
			GteReadSXY2(s + 0x1C);
			GteReadOTZ(s + 0x20);
			if (F<uint16_t>(s, 0x20) == 0)
				*(uint16_t *)(out + 6) = 0x10;
			else
			{
				const int16_t cx = F<int16_t>(s, 0x52);
				const int16_t cy = F<int16_t>(s, 0x54);
				const int32_t dx = (int32_t)(int16_t)(F<int16_t>(s, 0x1C) - cx) >> 3;
				const int32_t dy = (int32_t)(int16_t)(F<int16_t>(s, 0x1E) - cy) >> 3;
				int32_t adx = dx;
				if ((int16_t)dx < 0) adx = -adx;
				if ((int16_t)adx >= 0x80) adx = 0x80;
				int32_t ady = dy;
				if ((int16_t)dy < 0) ady = -ady;
				if ((int16_t)ady >= 0x80) ady = 0x80;
				if (F<int16_t>(s, 0x56) < (int16_t)adx) F<int16_t>(s, 0x56) = (int16_t)adx;
				if (F<int16_t>(s, 0x56) < (int16_t)ady) F<int16_t>(s, 0x56) = (int16_t)ady;
				const uint32_t d = *(const uint8_t *)(game_addr(WARP_Distance) + (int32_t)(int16_t)adx + shl32((int16_t)ady, 7));
				if (d != 0)
				{
					const int32_t radius = F<int16_t>(s, 0x50);
					if ((int32_t)d < radius)
					{
						const int32_t q = (int32_t)(d << 7) / radius;
						const uint32_t w = *(const uint16_t *)(F<uint32_t>(s, 0x4C) + q * 2);
						F<int16_t>(s, 0x1C) = (int16_t)((mul32(dx, (int32_t)w) >> 9) + cx);
						F<int16_t>(s, 0x1E) = (int16_t)((mul32(dy, (int32_t)w) >> 9) + cy);
					}
				}
				const int32_t vx = F<int16_t>(s, 0x1C);
				if (vx < shl32(F<int16_t>(s, 0xC), 3)) s[0x22] |= 1;
				else if (vx >= shl32(F<int16_t>(s, 0x10), 3)) s[0x22] |= 2;
				const int32_t vy = F<int16_t>(s, 0x1E);
				if (vy < shl32(F<int16_t>(s, 0xE), 3)) s[0x22] |= 4;
				else if (vy >= shl32(F<int16_t>(s, 0x12), 3)) s[0x22] |= 8;
				*(uint32_t *)out = F<uint32_t>(s, 0x1C);
				*(uint32_t *)(out + 4) = F<uint32_t>(s, 0x20);
			}
			out += 8;
		} while (--left != 0);
		*objp = p;
		*outp = out;
	}

	static inline void CopyVertex(uint8_t *s, uint32_t off, const uint8_t *verts, uint16_t index)
	{
		const uint8_t *v = verts + (uint32_t)(index & 0xFFF) * 8;
		F<uint32_t>(s, off) = *(const uint32_t *)v;
		F<uint32_t>(s, off + 4) = *(const uint32_t *)(v + 4);
	}

	static uint32_t WarpPolygons(uint8_t *s, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const uint8_t *verts = F<uint8_t *>(s, 4);
		const uint8_t *p = F<uint8_t *>(s, 0);
		uint16_t k = F<uint16_t>(s, 8);
		F<uint16_t>(s, 8) = (uint16_t)(k - 1);
		if (k != 0)
		{
			do
			{
				CopyVertex(s, 0x1C, verts, *(const uint16_t *)p);
				CopyVertex(s, 0x24, verts, *(const uint16_t *)(p + 2));
				CopyVertex(s, 0x2C, verts, *(const uint16_t *)(p + 4));
				const uint8_t a = s[0x22], b = s[0x2A], c = s[0x32];
				if ((uint8_t)(a | b | c) < 0x10 && !(c & (a & b)))
				{
					GteSetSXY012(F<uint32_t>(s, 0x1C), F<uint32_t>(s, 0x24), F<uint32_t>(s, 0x2C));
					GteNCLIP();
					GteReadMAC0(s + 0x3C);
					if (F<int32_t>(s, 0x3C) >= 0)
					{
						GteSetSZ123(F<uint16_t>(s, 0x20), F<uint16_t>(s, 0x28), F<uint16_t>(s, 0x30));
						GteAVSZ3();
						*(uint32_t *)pk = 0x7000000;
						*(uint32_t *)(pk + 4) = ((((uint32_t)*(const uint16_t *)(p + 0xE) & 0x200) | 0x2400) << 16) | F<uint32_t>(s, 0x14);
						*(uint32_t *)(pk + 8) = F<uint32_t>(s, 0x1C);
						*(uint32_t *)(pk + 0x10) = F<uint32_t>(s, 0x24);
						*(uint32_t *)(pk + 0x18) = F<uint32_t>(s, 0x2C);
						*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 8);
						*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(p + 0xC);
						*(uint16_t *)(pk + 0x16) &= 0xFDFF;
						*(uint16_t *)(pk + 0x1C) = *(const uint16_t *)(p + 6);
						GteReadOTZWord(s + 0x44);
						const uint32_t bucket = ot + (uint32_t)(F<int32_t>(s, 0x44) >> shift) * 4;
						InsertPrimAutoDepth(bucket, pk);
						uint8_t *flat = pk + 0x20;
						*(uint32_t *)flat = 0x4000000;
						*(uint32_t *)(flat + 4) = F<uint32_t>(s, 0x14) | 0x22000000;
						*(uint32_t *)(flat + 8) = F<uint32_t>(s, 0x1C);
						*(uint32_t *)(flat + 0xC) = F<uint32_t>(s, 0x24);
						*(uint32_t *)(flat + 0x10) = F<uint32_t>(s, 0x2C);
						InsertPrimAutoDepth(bucket, flat);
						uint8_t *mode = pk + 0x34;
						*(uint32_t *)mode = 0x1000000;
						*(uint32_t *)(mode + 4) = 0xE1000240;
						InsertPrimAutoDepth(bucket, mode);
						pk += 0x3C;
					}
				}
				k = F<uint16_t>(s, 8);
				p += 0x10;
				F<uint16_t>(s, 8) = (uint16_t)(k - 1);
			} while (k != 0);
		}
		k = F<uint16_t>(s, 0xA);
		F<uint16_t>(s, 0xA) = (uint16_t)(k - 1);
		if (k != 0)
		{
			p += 4;
			do
			{
				CopyVertex(s, 0x1C, verts, *(const uint16_t *)(p - 4));
				CopyVertex(s, 0x24, verts, *(const uint16_t *)(p - 2));
				CopyVertex(s, 0x2C, verts, *(const uint16_t *)p);
				CopyVertex(s, 0x34, verts, *(const uint16_t *)(p + 2));
				const uint8_t d = s[0x3A], a = s[0x22], b = s[0x2A], c = s[0x32];
				if ((uint8_t)(d | a | b | c) < 0x10 && !(c & (d & a & b)))
				{
					GteSetSXY012(F<uint32_t>(s, 0x1C), F<uint32_t>(s, 0x24), F<uint32_t>(s, 0x2C));
					GteNCLIP();
					GteReadMAC0(s + 0x3C);
					if (F<int32_t>(s, 0x3C) >= 0)
					{
						GteSetSZ0123(F<uint16_t>(s, 0x20), F<uint16_t>(s, 0x28), F<uint16_t>(s, 0x30), F<uint16_t>(s, 0x38));
						GteAVSZ4();
						*(uint32_t *)pk = 0x9000000;
						*(uint32_t *)(pk + 4) = ((((uint32_t)*(const uint16_t *)(p + 0xA) & 0x200) | 0x2C00) << 16) | F<uint32_t>(s, 0x14);
						*(uint32_t *)(pk + 8) = F<uint32_t>(s, 0x1C);
						*(uint32_t *)(pk + 0x10) = F<uint32_t>(s, 0x24);
						*(uint32_t *)(pk + 0x18) = F<uint32_t>(s, 0x2C);
						*(uint32_t *)(pk + 0x20) = F<uint32_t>(s, 0x34);
						*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 4);
						*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(p + 8);
						*(uint16_t *)(pk + 0x1C) = *(const uint16_t *)(p + 0xC);
						*(uint16_t *)(pk + 0x16) &= 0xFDFF;
						*(uint16_t *)(pk + 0x24) = *(const uint16_t *)(p + 0xE);
						GteReadOTZWord(s + 0x44);
						const uint32_t bucket = ot + (uint32_t)(F<int32_t>(s, 0x44) >> shift) * 4;
						InsertPrimAutoDepth(bucket, pk);
						uint8_t *flat = pk + 0x28;
						*(uint32_t *)flat = 0x5000000;
						*(uint32_t *)(flat + 4) = F<uint32_t>(s, 0x14) | 0x2A000000;
						*(uint32_t *)(flat + 8) = F<uint32_t>(s, 0x1C);
						*(uint32_t *)(flat + 0xC) = F<uint32_t>(s, 0x24);
						*(uint32_t *)(flat + 0x10) = F<uint32_t>(s, 0x2C);
						*(uint32_t *)(flat + 0x14) = F<uint32_t>(s, 0x34);
						InsertPrimAutoDepth(bucket, flat);
						uint8_t *mode = pk + 0x40;
						*(uint32_t *)mode = 0x1000000;
						*(uint32_t *)(mode + 4) = 0xE1000240;
						InsertPrimAutoDepth(bucket, mode);
						pk += 0x48;
					}
				}
				k = F<uint16_t>(s, 0xA);
				p += 0x14;
				F<uint16_t>(s, 0xA) = (uint16_t)(k - 1);
			} while (k != 0);
		}
		return (uint32_t)pk;
	}

	static uint8_t *WarpGroups(const uint8_t *bones, uint8_t **objp, uint8_t *out, uint8_t *s)
	{
		uint8_t *obj = *objp;
		int32_t groups = *(const int16_t *)obj;
		obj += 2;
		if (groups > 0)
		{
			do
			{
				const int32_t bone = *(const int16_t *)obj;
				obj += 2;
				const Mat4x3 *bm = (const Mat4x3 *)(bones + bone * 0x30 + 0x10);
				GteSetRotMatrixCtrl(bm);
				GteSetTransVectorCtrl(bm);
				WarpVertices(&obj, &out, s);
			} while (--groups != 0);
		}
		*objp = obj;
		return out;
	}

	static void WarpCounts(uint8_t *obj, uint8_t *s)
	{
		obj = (uint8_t *)(((uintptr_t)obj + 3) & ~(uintptr_t)3);
		F<uint16_t>(s, 8) = *(const uint16_t *)obj;
		F<uint16_t>(s, 0xA) = *(const uint16_t *)(obj + 2);
		F<uint8_t *>(s, 0) = obj + 0xC;
	}

	static uint32_t WarpModel(const uint8_t *model, uint8_t *s, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint8_t *table = *(uint8_t *const *)(model + 4);
		const uint8_t *bones = *(uint8_t *const *)model + 0x10;
		const int32_t count = *(const int32_t *)table;
		const uint32_t *offs = (const uint32_t *)(table + 4);
		for (int32_t i = 0; i < count; i++)
		{
			uint8_t *obj = (uint8_t *)(*(uint8_t *const *)(model + 4) + offs[i]);
			if (!(F<uint32_t>(s, 0x18) & (1u << (i & 31)))) continue;
			WarpGroups(bones, &obj, F<uint8_t *>(s, 4), s);
			WarpCounts(obj, s);
			cursor = WarpPolygons(s, ot, shift, cursor);
		}
		return cursor;
	}

	static uint8_t *WarpBegin(uint8_t *entity, int16_t sx, int16_t sy, int16_t radius, int32_t step, int32_t fade)
	{
		uint8_t *c = (uint8_t *)FieldAlloc(0x7C);
		F<int16_t>(c, 0x72) = sx;
		F<int16_t>(c, 0x74) = sy;
		F<int32_t>(c, 0x78) = fade;
		F<int16_t>(c, 0x70) = radius;
		F<uint32_t>(c, 0x6C) = game_addr(WARP_Weights) + (uint32_t)shl32(step, 8);
		F<int16_t>(c, 0x76) = 0;
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(entity + 0x40), (Mat4x3 *)c);
		FrameCursor() = DrawShadow(entity, RenderOT(0x4040), 0x10, FrameCursor());
		ComputeBonesWorldMatrices(entity + 0x60, c);
		F<int16_t>(c, 0x2C) = 0;
		F<int16_t>(c, 0x2E) = 0;
		F<uint32_t>(c, 0x24) = BoneWorkspace();
		F<int16_t>(c, 0x30) = 0x140;
		F<int16_t>(c, 0x32) = 0xD8;
		const int32_t w = 0x1000 - fade;
		GteSetIR0(w);
		GteLoadIRBytes(entity + 0x28);
		GteGPF();
		GteStoreIRBytes(c + 0x34);
		c[0x37] = w == 0x1000 ? 0 : 2;
		F<uint32_t>(c, 0x38) = *(const uint32_t *)(entity + 0x7C);
		return c;
	}

	static int32_t DrawWarped(uint8_t *entity, int16_t sx, int16_t sy, int16_t radius, int32_t step, int32_t fade)
	{
		uint8_t *c = WarpBegin(entity, sx, sy, radius, step, fade);
		uint8_t *s = c + 0x20;
		FrameCursor() = WarpModel(*(const uint8_t *const *)(entity + 0x64), s, RenderOT(0x44), 0, FrameCursor());
		BuildBoneMatricesFromPose(entity + 0x60);
		uint8_t *weapon = *(uint8_t *const *)(entity + 0x78);
		if (weapon)
		{
			F<uint32_t>(c, 0x38) = 0xFFFFFFFF;
			ComputeBonesWorldMatrices(weapon, c);
			FrameCursor() = WarpModel(*(const uint8_t *const *)(weapon + 4), s, RenderOT(0x44), 0, FrameCursor());
			BuildBoneMatricesFromPose(weapon);
		}
		FieldFree(0x7C);
		return F<int16_t>(c, 0x76);
	}

	static int32_t WarpFade(int32_t step) { return ComputeSin(shl32(step, 10) / 20); }

	static bool WarpCentre(const int16_t *pos, int16_t sxy[2])
	{
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteLoadV0(pos);
		GteRTPS();
		uint32_t otz;
		GteReadSXY2(sxy);
		GteReadOTZ(&otz);
		return (uint16_t)otz != 0;
	}

	static void SetBlock(PrimArg *arg, const MainNode *m, const int16_t *point, int16_t scroll)
	{
		ff8fx::copy<(sizeof(arg->anchor))>(arg->anchor, point);
		arg->scale[0] = m->size;
		arg->scale[1] = m->size;
		arg->scale[2] = m->size;
		arg->scaled = 1;
		arg->depth = -0x200;
		arg->scroll = scroll;
		arg->morph = TexBase() + MORPH;
	}
}
}

namespace ff8fx
{
namespace magicsummon283
{
	static void __cdecl PrimCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PrimArg *arg = (const PrimArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = AllocHeader(0x6C);
		const int32_t object = (int16_t)r->flags_lo;
		uint8_t *model = l->data + *(const int32_t *)(l->data + object * 4 + 8);
		*(uint8_t **)h = model;
		const int16_t f0 = r->b0, f1 = r->b1;
		const int32_t nverts = *(const int32_t *)(model + 4);
		if (f0 == f1)
		{
			if (f0 == 0) *(uint8_t **)(h + 4) = model + 0xC;
			else *(uint8_t **)(h + 4) = model + (uint32_t)mul32(nverts, f0) * 8 + 0xC;
		}
		else
		{
			const int16_t t = r->b;
			if (t == 0)
			{
				if (f0 == 0) *(uint8_t **)(h + 4) = model + 0xC;
				else *(uint8_t **)(h + 4) = model + (uint32_t)mul32(nverts, f0) * 8 + 0xC;
			}
			else if (t == 0x1000)
			{
				if (f1 == 0) *(uint8_t **)(h + 4) = model + 0xC;
				else *(uint8_t **)(h + 4) = model + (uint32_t)mul32(nverts, f1) * 8 + 0xC;
			}
			else
			{
				BlendVertexFrames((uint32_t)model, f0, f1, t, arg->morph);
				*(uint32_t *)(h + 4) = arg->morph;
			}
		}
		Mat4x3 m = {};
		RotationFromAngles(r->rot, &m);
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		if (arg->scaled)
		{
			off[0] = (int16_t)(mul32(r->pos[0], arg->scale[0]) >> 12);
			off[1] = (int16_t)(mul32(r->pos[1], arg->scale[1]) >> 12);
			off[2] = (int16_t)(mul32(r->pos[2], arg->scale[2]) >> 12);
			Scale3DMatrix(&m, arg->scale);
		}
		if (r->flags & 0x1000)
		{
			int32_t v[3];
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->anchor);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			GteSetRotMatrixCtrl(MATRIX_Flag1000());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(v);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)v[0]);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)v[1]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)v[2]);
			if (!(r->flags & 0x8000)) MatrixMultiply(MATRIX_Flag1000(), &m);
		}
		else
		{
			off[0] = (int16_t)(off[0] + arg->anchor[0]);
			off[1] = (int16_t)(off[1] + arg->anchor[1]);
			off[2] = (int16_t)(off[2] + arg->anchor[2]);
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			if (!(r->flags & 0x8000)) MatrixMultiply(&Camera(), &m);
		}
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			const int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
			Scale3DMatrix(&m, v);
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)(h + 0x1C) = 0x2030;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) = 0x20F0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		if (arg->scroll != 0 && r->flags_lo == 1)
		{
			*(uint16_t *)(h + 0x26) = 0;
			*(uint16_t *)(h + 0x24) = 0;
			*(uint16_t *)(h + 0x2A) = 0x100;
			*(uint16_t *)(h + 0x28) = 0x100;
			*(uint16_t *)(h + 0x2E) = 0;
			*(uint16_t *)(h + 0x2C) = 0;
			*(uint16_t *)(h + 0x32) = 0x80;
			*(uint16_t *)(h + 0x30) = 0x100;
			*(int16_t *)(h + 0x22) = arg->scroll;
		}
		else *(int16_t *)(h + 0x22) = 0;
		*(int32_t *)(h + 0x18) = arg->depth;
		*(uint16_t *)(h + 0x20) = 0;
		PacketCursor() = RenderPrimSet(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x6C);
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		uint32_t colour = (uint32_t)(uintptr_t)n;
		const int32_t t = shl32(m->counter * 7, 12) / 60;
		const int32_t i = t >> 12, frac = t & 0xFFF;
		BlendColour((const void *)(game_addr(FLASH_Colours) + i * 4), (const void *)(game_addr(FLASH_Colours) + 4 + i * 4), 0x1000 - frac, frac, &colour);
		int32_t level;
		if (m->counter < 8 || m->counter >= 0x34)
		{
			const int32_t c = m->counter < 8 ? m->counter : 0x3C - m->counter;
			level = shl32(c * 275, 3) >> 3;
			colour = 0;
		}
		else level = 0x898;
		SetScreenFlash((uint32_t)level, colour);
		PrimArg arg;
		if ((uint32_t)(int32_t)m->counter < 0x3C)
		{
			SetBlock(&arg, m, m->anchor, (int16_t)((m->counter & 0xF) << 3));
			prim::play((prim::Layout *)m->layoutA, PrimCallback, (int)&arg, 0);
		}
		const int32_t e = m->counter - 0x18;
		if ((uint32_t)e < 0x24)
		{
			if ((uint32_t)e < 0x14)
			{
				const int32_t step = 0x14 - e;
				const int32_t fade = WarpFade(step);
				GteSetIR0(0x1000 - fade);
				GteLoadIRBytes(&m->colour);
				GteGPF();
				GteStoreIRBytes(m->ent + 0x2C);
				m->ent[0x2F] = 0x32;
				int16_t sxy[2];
				if (WarpCentre(m->pos, sxy))
				{
					m->radius = (int16_t)DrawWarped(m->ent, sxy[0], sxy[1], m->radius, step, fade);
				}
			}
			if (e == 0x14)
			{
				*(uint16_t *)m->ent &= 0xFFFB;
				*(uint32_t *)(m->ent + 0x2C) = m->colour;
			}
			SetBlock(&arg, m, m->pos, 0);
			prim::play((prim::Layout *)m->layoutB, PrimCallback, (int)&arg, 0);
		}
		if (m->counter == 0xA) BdPlaySE(SOUND_Summon(), 0, 0x80);
		m->counter++;
		return m->counter < 0x3C ? 0 : TASK_END;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *mn = (MasterNode *)n;
		if (mn->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			mn->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			mn->arena = 1;
		}
		if (mn->counter == 1 && !mn->started)
		{
			mn->started = 1;
			InitTaskQueuePool(QMain(), (void *)TexBase(), 0x648, 1);
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&m->counter, 0, 0x18F);
			m->ent = Entity(Ctx()->actions[0].targets[0]);
			m->ent[0] |= 4;
			DecodeModelPrimLayout(game_addr(MODEL_A), m->layoutA, 0x4BC);
			DecodeModelPrimLayout(game_addr(MODEL_B), m->layoutB, 0x160);
			GetDefaultEffectPosition(m->ent, m->pos);
			m->anchor[0] = m->pos[0];
			m->anchor[2] = m->pos[2];
			m->anchor[1] = *(const int16_t *)(m->ent + 0x24);
			m->colour = *(const uint32_t *)(m->ent + 0x2C);
			m->size = *(const int16_t *)(m->ent + 0x26);
		}
		const int a = mn->started ? ExecuteTaskQueue(QMain()) : (int)n;
		if (mn->started && a == 0)
		{
			SetScreenFlash(0, 0);
			return TASK_END;
		}
		mn->counter++;
		return 0;
	}
}

	void register_mag283_magic_summon()
	{
		register_port(magicsummon283::ORIG_MasterTask, (void *)magicsummon283::MasterTask, "283 MasterTask", 283);
		register_port(magicsummon283::ORIG_MainTask, (void *)magicsummon283::MainTask, "283 MainTask", 283);
	}
}

