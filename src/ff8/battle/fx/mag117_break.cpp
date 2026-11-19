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

// Effect 117: Break

#include "mag_common.h"

namespace ff8fx
{
namespace break117
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_2521654)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_2521658)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_2521674)); }
	inline TaskQueue *QSparks() { return (TaskQueue *)game_addr(game::unk_2521678); }
	inline TaskQueue *QDebris() { return (TaskQueue *)game_addr(game::unk_2521688); }
	inline TaskQueue *QTargets() { return (TaskQueue *)game_addr(game::unk_2521698); }
	inline Mat4x3 *EffectCamera() { return (Mat4x3 *)game_addr(game::g_Mag117_EffectCameraMatrix); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t &BoneWorkspace() { return var<uint32_t>(game_addr(game::g_BattleVertexWorkspace)); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_117_BREAK_RootTask;
	static constexpr game::Id ORIG_TargetTask = game::MAG_117_BREAK_MainVisual_Petrify;
	static constexpr game::Id ORIG_SparkTask = game::MAG_117_SparkTask;
	static constexpr game::Id ORIG_DebrisTask = game::MAG_117_DebrisTask;
	static constexpr game::Id MODEL_Shell = game::unk_128E2D0;
	static constexpr game::Id MODEL_Debris = game::unk_1297928;
	static constexpr game::Id SEQ_Spark = game::unk_1297B00;
	static const void *SOUND_Break() { return (const void *)game_addr(game::unk_1298C58); }
	static const uint32_t STONE_TPage = 0xB8, STONE_Clut = 0x3D54;
	static const uint32_t BUF_Morph = 0x4AF0, BUF_Proj = 0x4CF0, BUF_Dither = 0x5CF0;

	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesC(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_117_RotationFromAnglesZYX))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline uint32_t DrawShadow(uint8_t *entity, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, mode, cursor); }
	inline void InsertPrimDepthKeys(uint32_t ot, void *prim, int32_t a, int32_t b, int32_t c, int32_t d) { fn<void (__cdecl *)(uint32_t, void *, int32_t, int32_t, int32_t, int32_t)>(game_addr(game::SSIGPU_InsertPrimDepthKeys))(ot, prim, a, b, c, d); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
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
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t action;
		uint8_t started;
		uint32_t arena;
	};
	struct TargetNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		int16_t pos[4];
		uint8_t *entity;
		uint8_t layout[0x1554];
	};
	struct DebrisNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[3];
		int16_t pad16;
		int16_t vel[3];
		int16_t pad1E;
		int16_t rot[3];
		int16_t pad26;
		int16_t spin[3];
		int16_t pad2E;
	};
	struct SparkNode
	{
		TaskNode hdr;
		int16_t frame;
		int16_t pad0E;
		int16_t pos[3];
		int16_t pad16;
		int16_t vel[3];
		int16_t pad1E;
		int16_t acc[3];
		int16_t pad26;
	};
	struct PrimArg
	{
		int16_t pos[4];
		int32_t scale[3];
		int32_t scaled;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(TargetNode) == 0x1570 && sizeof(DebrisNode) == 0x30
		&& sizeof(SparkNode) == 0x28, "Break nodes");
	static_assert(sizeof(PrimArg) == 0x1C, "Break prim block");

	template<typename T> static inline T &F(uint8_t *s, uint32_t off) { return *(T *)(s + off); }

	static uint32_t StoneLevel(uint32_t e)
	{
		if (e < 9) return (uint32_t)(((uint64_t)(e << 15) * 0x38E38E39u) >> 32) >> 1;
		if (e >= 0x30) return (uint32_t)(((uint64_t)(0x1E0000u - (e << 15)) * 0xAAAAAAABu) >> 32) >> 3;
		return 0x8000;
	}

	static void ProjectVertices(uint8_t **objp, uint8_t **outp, uint8_t *s)
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
			GteReadSXY2(s + 0x24);
			GteReadOTZ(s + 0x28);
			if (F<uint16_t>(s, 0x28) == 0)
				*(uint16_t *)(out + 6) = 0x10;
			else
			{
				const int32_t sx = F<int16_t>(s, 0x24);
				if (sx < shl32(F<int16_t>(s, 0x14), 3)) s[0x2A] |= 1;
				else if (sx >= shl32(F<int16_t>(s, 0x18), 3)) s[0x2A] |= 2;
				const int32_t sy = F<int16_t>(s, 0x26);
				if (sy < shl32(F<int16_t>(s, 0x16), 3)) s[0x2A] |= 4;
				else if (sy >= shl32(F<int16_t>(s, 0x1A), 3)) s[0x2A] |= 8;
				*(uint32_t *)out = F<uint32_t>(s, 0x24);
				*(uint32_t *)(out + 4) = F<uint32_t>(s, 0x28);
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

	static inline void StoneTexture(uint8_t *s, uint8_t *pk)
	{
		const uint16_t dither = *(const uint16_t *)(TexBase() + BUF_Dither + (uint32_t)s[0x5A] * 2);
		if (dither > F<uint16_t>(s, 0x58)) *(uint16_t *)(pk + 0x16) &= 0xFDFF;
		else
		{
			*(uint16_t *)(pk + 0xE) = F<uint16_t>(s, 0x66);
			*(uint16_t *)(pk + 0x16) = F<uint16_t>(s, 0x64);
		}
	}

	static uint32_t StonePolygons(uint8_t *s, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *pk = (uint8_t *)cursor;
		const uint8_t *verts = F<uint8_t *>(s, 4);
		const uint8_t *p = F<uint8_t *>(s, 0);
		if (F<uint16_t>(s, 8) != 0)
		{
			int32_t j = 0;
			do
			{
				CopyVertex(s, 0x24, verts, *(const uint16_t *)p);
				CopyVertex(s, 0x2C, verts, *(const uint16_t *)(p + 2));
				CopyVertex(s, 0x34, verts, *(const uint16_t *)(p + 4));
				const uint8_t a = s[0x2A], b = s[0x32], c = s[0x3A];
				if ((uint8_t)(a | b | c) < 0x10 && !(c & (a & b)))
				{
					GteSetSXY012(F<uint32_t>(s, 0x24), F<uint32_t>(s, 0x2C), F<uint32_t>(s, 0x34));
					GteNCLIP();
					GteReadMAC0(s + 0x44);
					if (F<int32_t>(s, 0x44) >= 0)
					{
						const uint32_t z0 = F<uint16_t>(s, 0x28), z1 = F<uint16_t>(s, 0x30), z2 = F<uint16_t>(s, 0x38);
						GteSetSZ123(z0, z1, z2);
						GteAVSZ3();
						*(uint32_t *)pk = 0x7000000;
						*(uint32_t *)(pk + 4) = F<uint32_t>(s, 0x1C) | 0x24000000;
						*(uint32_t *)(pk + 8) = F<uint32_t>(s, 0x24);
						*(uint32_t *)(pk + 0x10) = F<uint32_t>(s, 0x2C);
						*(uint32_t *)(pk + 0x18) = F<uint32_t>(s, 0x34);
						*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 8);
						*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(p + 0xC);
						*(uint16_t *)(pk + 0x1C) = *(const uint16_t *)(p + 6);
						StoneTexture(s, pk);
						GteReadOTZWord(s + 0x4C);
						InsertPrimDepthKeys(ot + (uint32_t)(F<int32_t>(s, 0x4C) >> shift) * 4, pk, (int32_t)(z0 * 4), (int32_t)(z1 * 4), (int32_t)(z2 * 4), 0);
						pk += 0x20;
					}
				}
				j++;
				p += 0x10;
				s[0x5A]++;
			} while (j < (int32_t)F<uint16_t>(s, 8));
		}
		if (F<uint16_t>(s, 0xA) != 0)
		{
			int32_t j = 0;
			p += 4;
			do
			{
				CopyVertex(s, 0x24, verts, *(const uint16_t *)(p - 4));
				CopyVertex(s, 0x2C, verts, *(const uint16_t *)(p - 2));
				CopyVertex(s, 0x34, verts, *(const uint16_t *)p);
				CopyVertex(s, 0x3C, verts, *(const uint16_t *)(p + 2));
				const uint8_t d = s[0x42], a = s[0x2A], b = s[0x32], c = s[0x3A];
				if ((uint8_t)(d | a | b | c) < 0x10 && !(c & (d & a & b)))
				{
					GteSetSXY012(F<uint32_t>(s, 0x24), F<uint32_t>(s, 0x2C), F<uint32_t>(s, 0x34));
					GteNCLIP();
					GteReadMAC0(s + 0x44);
					if (F<int32_t>(s, 0x44) >= 0)
					{
						const uint32_t z0 = F<uint16_t>(s, 0x28), z1 = F<uint16_t>(s, 0x30), z2 = F<uint16_t>(s, 0x38), z3 = F<uint16_t>(s, 0x40);
						GteSetSZ0123(z0, z1, z2, z3);
						GteAVSZ4();
						*(uint32_t *)pk = 0x9000000;
						*(uint32_t *)(pk + 4) = F<uint32_t>(s, 0x1C) | 0x2C000000;
						*(uint32_t *)(pk + 8) = F<uint32_t>(s, 0x24);
						*(uint32_t *)(pk + 0x10) = F<uint32_t>(s, 0x2C);
						*(uint32_t *)(pk + 0x18) = F<uint32_t>(s, 0x34);
						*(uint32_t *)(pk + 0x20) = F<uint32_t>(s, 0x3C);
						*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 4);
						*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(p + 8);
						*(uint16_t *)(pk + 0x1C) = *(const uint16_t *)(p + 0xC);
						*(uint16_t *)(pk + 0x24) = *(const uint16_t *)(p + 0xE);
						StoneTexture(s, pk);
						GteReadOTZWord(s + 0x4C);
						InsertPrimDepthKeys(ot + (uint32_t)(F<int32_t>(s, 0x4C) >> shift) * 4, pk, (int32_t)(z0 * 4), (int32_t)(z1 * 4), (int32_t)(z2 * 4), (int32_t)(z3 * 4));
						pk += 0x28;
					}
				}
				j++;
				p += 0x14;
				s[0x5A]++;
			} while (j < (int32_t)F<uint16_t>(s, 0xA));
		}
		return (uint32_t)pk;
	}

	static uint32_t DrawModelObjects(const uint8_t *model, uint8_t *s, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint8_t *table = *(uint8_t *const *)(model + 4);
		const uint8_t *bones = *(uint8_t *const *)model + 0x10;
		const int32_t count = *(const int32_t *)table;
		const uint32_t *offs = (const uint32_t *)(table + 4);
		for (int32_t i = 0; i < count; i++)
		{
			uint8_t *obj = (uint8_t *)(*(uint8_t *const *)(model + 4) + offs[i]);
			if (!(F<uint32_t>(s, 0x20) & (1u << (i & 31)))) continue;
			uint8_t *out = F<uint8_t *>(s, 4);
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
					ProjectVertices(&obj, &out, s);
				} while (--groups != 0);
			}
			obj = (uint8_t *)(((uintptr_t)obj + 3) & ~(uintptr_t)3);
			for (int w = 0; w < 6; w++)
			{
				F<uint16_t>(s, 8 + 2 * w) = *(const uint16_t *)obj;
				obj += 2;
			}
			F<uint8_t *>(s, 0) = obj;
			cursor = StonePolygons(s, ot, shift, cursor);
		}
		return cursor;
	}

	static void DrawStoneTarget(uint8_t *entity, int32_t level, const uint32_t *unused, uint16_t tpage, uint16_t clut)
	{
		uint8_t *c = (uint8_t *)FieldAlloc(0x88);
		F<uint32_t>(c, 0x7C) = unused[0];
		F<uint32_t>(c, 0x80) = unused[1];
		F<uint16_t>(c, 0x84) = tpage;
		F<uint16_t>(c, 0x86) = clut;
		F<int16_t>(c, 0x78) = (int16_t)level;
		c[0x7A] = 0;
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(entity + 0x40), (Mat4x3 *)c);
		FrameCursor() = DrawShadow(entity, RenderOT(0x4040), 0x10, FrameCursor());
		ComputeBonesWorldMatrices(entity + 0x60, c);
		F<int16_t>(c, 0x38) = 0x140;
		F<uint32_t>(c, 0x24) = BoneWorkspace();
		F<int16_t>(c, 0x34) = 0;
		F<int16_t>(c, 0x36) = 0;
		F<int16_t>(c, 0x3A) = 0xD8;
		F<uint32_t>(c, 0x3C) = *(const uint32_t *)(entity + 0x28);
		F<uint32_t>(c, 0x40) = *(const uint32_t *)(entity + 0x7C);
		FrameCursor() = DrawModelObjects(*(const uint8_t *const *)(entity + 0x64), c + 0x20, RenderOT(0x44), 0, FrameCursor());
		BuildBoneMatricesFromPose(entity + 0x60);
		uint8_t *second = *(uint8_t *const *)(entity + 0x78);
		if (second)
		{
			F<uint32_t>(c, 0x40) = 0xFFFFFFFF;
			ComputeBonesWorldMatrices(second, c);
			FrameCursor() = DrawModelObjects(*(const uint8_t *const *)(second + 4), c + 0x20, RenderOT(0x44), 0, FrameCursor());
			BuildBoneMatricesFromPose(second);
		}
		FieldFree(0x88);
	}

	static void ProjectCut(const uint8_t *v, uint8_t *out, int32_t n, int32_t cut)
	{
		if (n == 0) return;
		uint32_t left = (uint32_t)n;
		do
		{
			GteLoadV0u(v);
			GteRTPS();
			GteReadSXY2(out);
			GteReadOTZ(out + 4);
			if (*(const int16_t *)(out + 4) <= 0) out[6] = 0x10;
			else
			{
				const int16_t sx = *(const int16_t *)out;
				if (sx < 0) out[6] = 1;
				else if (sx >= 0xA00) out[6] = 2;
				else out[6] = 0;
				const int16_t sy = *(const int16_t *)(out + 2);
				if (sy < 0) out[6] |= 4;
				else if (sy >= 0x6C0) out[6] |= 8;
				out[7] = (int32_t)*(const int16_t *)(v + 2) <= cut ? 1 : 2;
			}
			v += 8;
			out += 8;
		} while (--left != 0);
	}

	static void CutEdge(const uint8_t *a, const uint8_t *b, uint8_t *out, int32_t cut, bool gouraud)
	{
		const int32_t dy = (int32_t)*(const int16_t *)(b + 2) - *(const int16_t *)(a + 2);
		if (dy == 0)
		{
			*(uint32_t *)(out + 8) = *(const uint32_t *)(a + 8);
			*(uint32_t *)(out + 0xC) = *(const uint32_t *)(a + 0xC);
			*(uint16_t *)(out + 0x10) = *(const uint16_t *)(a + 0x10);
			if (gouraud) *(uint32_t *)(out + 0x14) = *(const uint32_t *)(a + 0x14);
			return;
		}
		*(int16_t *)(out + 2) = (int16_t)cut;
		const int32_t t = shl32(cut - *(const int16_t *)(a + 2), 16) / dy;
		*(int16_t *)out = (int16_t)((mul32((int32_t)*(const int16_t *)b - *(const int16_t *)a, t) >> 16) + *(const int16_t *)a);
		*(int16_t *)(out + 4) = (int16_t)((mul32((int32_t)*(const int16_t *)(b + 4) - *(const int16_t *)(a + 4), t) >> 16) + *(const int16_t *)(a + 4));
		GteLoadV0u(out);
		GteRTPS();
		out[0x10] = (uint8_t)((mul32((int32_t)b[0x10] - a[0x10], t) >> 16) + a[0x10]);
		out[0x11] = (uint8_t)((mul32((int32_t)b[0x11] - a[0x11], t) >> 16) + a[0x11]);
		if (gouraud)
		{
			out[0x14] = (uint8_t)((mul32((int32_t)b[0x14] - a[0x14], t) >> 16) + a[0x14]);
			out[0x15] = (uint8_t)((mul32((int32_t)b[0x15] - a[0x15], t) >> 16) + a[0x15]);
			out[0x16] = (uint8_t)((mul32((int32_t)b[0x16] - a[0x16], t) >> 16) + a[0x16]);
		}
		GteReadSXY2(out + 8);
	}

	static inline void ListVertex(uint8_t *&dst, const uint8_t *verts, uint16_t index, const uint8_t *proj, uint16_t uv)
	{
		*(uint32_t *)dst = *(const uint32_t *)(verts + (uint32_t)index * 4);
		*(uint32_t *)(dst + 4) = *(const uint32_t *)(verts + (uint32_t)index * 4 + 4);
		if (proj)
		{
			*(uint32_t *)(dst + 8) = *(const uint32_t *)proj;
			*(uint32_t *)(dst + 0xC) = *(const uint32_t *)(proj + 4);
		}
		*(uint16_t *)(dst + 0x10) = uv;
	}

	static int TriangleSetup(uint8_t *h, const uint8_t *p0, const uint8_t *p1, const uint8_t *p2)
	{
		const uint8_t a = p0[6], b = p1[6], c = p2[6];
		if ((uint8_t)(a | b | c) >= 0x10) return 0;
		if (c & (a & b)) return 0;
		const uint8_t side = (uint8_t)(p1[7] | p2[7] | p0[7]);
		if (side == 2) return 0;
		GteSetSXY012(*(const uint32_t *)p0, *(const uint32_t *)p1, *(const uint32_t *)p2);
		GteNCLIP();
		GteReadMAC0(h + 0x24);
		if (*(const int32_t *)(h + 0x24) < 0) return 0;
		GteSetSZ123((uint32_t)(int32_t)*(const int16_t *)(p0 + 4), (uint32_t)(int32_t)*(const int16_t *)(p1 + 4), (uint32_t)(int32_t)*(const int16_t *)(p2 + 4));
		GteAVSZ3();
		return side == 3 ? 3 : 1;
	}

	static inline uint32_t Bucket(uint8_t *h, uint32_t ot, int32_t shift) { return ot + (uint32_t)(*(const int32_t *)(h + 0x24) >> shift) * 4; }

	static uint32_t CutTrianglesFT(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor, const uint8_t *proj)
	{
		uint8_t *rec = *(uint8_t **)(h + 0x20);
		const int32_t count = *(const int32_t *)rec;
		rec += 4;
		*(uint8_t **)(h + 0x20) = rec;
		uint8_t *pk = (uint8_t *)cursor;
		if (count == 0)
		{
			*(uint8_t **)(h + 0x20) = rec;
			return (uint32_t)pk;
		}
		uint32_t left = (uint32_t)count;
		do
		{
			const uint8_t *p0 = proj + (uint32_t)*(const uint16_t *)(rec + 4) * 4;
			const uint8_t *p1 = proj + (uint32_t)*(const uint16_t *)(rec + 6) * 4;
			const uint8_t *p2 = proj + (uint32_t)*(const uint16_t *)(rec + 8) * 4;
			const int side = TriangleSetup(h, p0, p1, p2);
			if (side == 3)
			{
				const uint8_t *verts = *(uint8_t **)(h + 4);
				uint8_t *A = h + 0x30, *B = h + 0x90;
				if (p0[7] == 1) { ListVertex(A, verts, *(const uint16_t *)(rec + 4), p0, *(const uint16_t *)(rec + 0xC)); A += 0x18; }
				else { ListVertex(B, verts, *(const uint16_t *)(rec + 4), nullptr, *(const uint16_t *)(rec + 0xC)); B += 0x18; }
				if (p1[7] == 1) { ListVertex(A, verts, *(const uint16_t *)(rec + 6), p1, *(const uint16_t *)(rec + 0x10)); A += 0x18; }
				else { ListVertex(B, verts, *(const uint16_t *)(rec + 6), nullptr, *(const uint16_t *)(rec + 0x10)); B += 0x18; }
				if (p2[7] == 1) { ListVertex(A, verts, *(const uint16_t *)(rec + 8), p2, *(const uint16_t *)(rec + 0xA)); A += 0x18; }
				else ListVertex(B, verts, *(const uint16_t *)(rec + 8), nullptr, *(const uint16_t *)(rec + 0xA));
				GteReadOTZWord(h + 0x24);
				if (A == h + 0x48)
				{
					CutEdge(h + 0x30, h + 0x90, h + 0x48, *(const int32_t *)(h + 0x2C), false);
					CutEdge(h + 0x30, h + 0xA8, h + 0x60, *(const int32_t *)(h + 0x2C), false);
					*(uint32_t *)pk = 0x7000000;
					*(uint32_t *)(pk + 4) = *(const uint32_t *)rec;
					*(uint16_t *)(pk + 0xE) = *(const uint16_t *)(rec + 0xE);
					*(uint16_t *)(pk + 0x16) = *(const uint16_t *)(rec + 0x12);
					*(uint16_t *)(pk + 0xC) = F<uint16_t>(h, 0x40);
					*(uint16_t *)(pk + 0x14) = F<uint16_t>(h, 0x58);
					*(uint16_t *)(pk + 0x1C) = F<uint16_t>(h, 0x70);
					*(uint32_t *)(pk + 8) = F<uint32_t>(h, 0x38);
					*(uint32_t *)(pk + 0x10) = F<uint32_t>(h, 0x50);
					*(uint32_t *)(pk + 0x18) = F<uint32_t>(h, 0x68);
				}
				else
				{
					CutEdge(h + 0x30, h + 0x90, h + 0x60, *(const int32_t *)(h + 0x2C), false);
					CutEdge(h + 0x48, h + 0x90, h + 0x78, *(const int32_t *)(h + 0x2C), false);
					*(uint32_t *)pk = 0x7000000;
					*(uint32_t *)(pk + 4) = *(const uint32_t *)rec;
					*(uint16_t *)(pk + 0xE) = *(const uint16_t *)(rec + 0xE);
					*(uint16_t *)(pk + 0x16) = *(const uint16_t *)(rec + 0x12);
					*(uint16_t *)(pk + 0xC) = F<uint16_t>(h, 0x40);
					*(uint16_t *)(pk + 0x14) = F<uint16_t>(h, 0x58);
					*(uint16_t *)(pk + 0x1C) = F<uint16_t>(h, 0x70);
					*(uint16_t *)(pk + 0x24) = F<uint16_t>(h, 0x88);
					*(uint32_t *)(pk + 8) = F<uint32_t>(h, 0x38);
					*(uint32_t *)(pk + 0x10) = F<uint32_t>(h, 0x50);
					*(uint32_t *)(pk + 0x18) = F<uint32_t>(h, 0x68);
					*(uint32_t *)(pk + 0x20) = F<uint32_t>(h, 0x80);
					const int32_t z0 = shl32(*(const int16_t *)(p0 + 4), 2);
					InsertPrimDepthKeys(Bucket(h, ot, shift), pk, z0, shl32(*(const int16_t *)(p1 + 4), 2), shl32(*(const int16_t *)(p2 + 4), 2), z0);
					pk += 0x28;
					rec += 0x14;
					continue;
				}
			}
			else if (side == 1)
			{
				*(uint32_t *)pk = 0x7000000;
				*(uint32_t *)(pk + 4) = *(const uint32_t *)rec;
				*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(rec + 0xC);
				*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(rec + 0x10);
				*(uint32_t *)(pk + 0x1C) = *(const uint32_t *)(rec + 8) >> 16;
				*(uint32_t *)(pk + 8) = *(const uint32_t *)p0;
				*(uint32_t *)(pk + 0x10) = *(const uint32_t *)p1;
				*(uint32_t *)(pk + 0x18) = *(const uint32_t *)p2;
				GteReadOTZWord(h + 0x24);
			}
			if (side)
			{
				InsertPrimDepthKeys(Bucket(h, ot, shift), pk, shl32(*(const int16_t *)(p0 + 4), 2), shl32(*(const int16_t *)(p1 + 4), 2), shl32(*(const int16_t *)(p2 + 4), 2), 0);
				pk += 0x20;
			}
			rec += 0x14;
		} while (--left != 0);
		*(uint8_t **)(h + 0x20) = rec;
		return (uint32_t)pk;
	}

	static uint32_t CutTrianglesGT(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor, const uint8_t *proj)
	{
		uint8_t *rec = *(uint8_t **)(h + 0x20);
		const int32_t count = *(const int32_t *)rec;
		rec += 4;
		*(uint8_t **)(h + 0x20) = rec;
		uint8_t *pk = (uint8_t *)cursor;
		if (count == 0)
		{
			*(uint8_t **)(h + 0x20) = rec;
			return (uint32_t)pk;
		}
		uint32_t left = (uint32_t)count;
		do
		{
			const uint8_t *p0 = proj + (uint32_t)*(const uint16_t *)(rec + 4) * 4;
			const uint8_t *p1 = proj + (uint32_t)*(const uint16_t *)(rec + 6) * 4;
			const uint8_t *p2 = proj + (uint32_t)*(const uint16_t *)(rec + 8) * 4;
			const int side = TriangleSetup(h, p0, p1, p2);
			if (side == 3)
			{
				const uint8_t *verts = *(uint8_t **)(h + 4);
				uint8_t *A = h + 0x30, *B = h + 0x90;
				if (p0[7] == 1) { ListVertex(A, verts, *(const uint16_t *)(rec + 4), p0, *(const uint16_t *)(rec + 0xC)); *(uint32_t *)(A + 0x14) = *(const uint32_t *)rec; A += 0x18; }
				else { ListVertex(B, verts, *(const uint16_t *)(rec + 4), nullptr, *(const uint16_t *)(rec + 0xC)); *(uint32_t *)(B + 0x14) = *(const uint32_t *)rec; B += 0x18; }
				if (p1[7] == 1) { ListVertex(A, verts, *(const uint16_t *)(rec + 6), p1, *(const uint16_t *)(rec + 0x10)); *(uint32_t *)(A + 0x14) = *(const uint32_t *)(rec + 0x14); A += 0x18; }
				else { ListVertex(B, verts, *(const uint16_t *)(rec + 6), nullptr, *(const uint16_t *)(rec + 0x10)); *(uint32_t *)(B + 0x14) = *(const uint32_t *)(rec + 0x14); B += 0x18; }
				if (p2[7] == 1) { ListVertex(A, verts, *(const uint16_t *)(rec + 8), p2, *(const uint16_t *)(rec + 0xA)); *(uint32_t *)(A + 0x14) = *(const uint32_t *)(rec + 0x18); A += 0x18; }
				else { ListVertex(B, verts, *(const uint16_t *)(rec + 8), nullptr, *(const uint16_t *)(rec + 0xA)); *(uint32_t *)(B + 0x14) = *(const uint32_t *)(rec + 0x18); }
				GteReadOTZWord(h + 0x24);
				if (A == h + 0x48)
				{
					CutEdge(h + 0x30, h + 0x90, h + 0x48, *(const int32_t *)(h + 0x2C), true);
					CutEdge(h + 0x30, h + 0xA8, h + 0x60, *(const int32_t *)(h + 0x2C), true);
					*(uint32_t *)(pk + 4) = F<uint32_t>(h, 0x44);
					*(uint32_t *)(pk + 0x10) = F<uint32_t>(h, 0x5C);
					*(uint32_t *)(pk + 0x1C) = F<uint32_t>(h, 0x74);
					*(uint16_t *)(pk + 0xE) = *(const uint16_t *)(rec + 0xE);
					*(uint16_t *)(pk + 0x1A) = *(const uint16_t *)(rec + 0x12);
					*(uint16_t *)(pk + 0xC) = F<uint16_t>(h, 0x40);
					*(uint16_t *)(pk + 0x18) = F<uint16_t>(h, 0x58);
					*(uint16_t *)(pk + 0x24) = F<uint16_t>(h, 0x70);
					*(uint32_t *)(pk + 8) = F<uint32_t>(h, 0x38);
					*(uint32_t *)(pk + 0x14) = F<uint32_t>(h, 0x50);
					*(uint32_t *)(pk + 0x20) = F<uint32_t>(h, 0x68);
					*(uint32_t *)pk = 0x9000000;
					pk[7] = 0x34;
				}
				else
				{
					CutEdge(h + 0x30, h + 0x90, h + 0x60, *(const int32_t *)(h + 0x2C), true);
					CutEdge(h + 0x48, h + 0x90, h + 0x78, *(const int32_t *)(h + 0x2C), true);
					*(uint32_t *)(pk + 4) = F<uint32_t>(h, 0x44);
					*(uint32_t *)(pk + 0x10) = F<uint32_t>(h, 0x5C);
					*(uint32_t *)(pk + 0x1C) = F<uint32_t>(h, 0x74);
					*(uint32_t *)(pk + 0x28) = F<uint32_t>(h, 0x8C);
					*(uint16_t *)(pk + 0xE) = *(const uint16_t *)(rec + 0xE);
					*(uint16_t *)(pk + 0x1A) = *(const uint16_t *)(rec + 0x12);
					*(uint16_t *)(pk + 0xC) = F<uint16_t>(h, 0x40);
					*(uint16_t *)(pk + 0x18) = F<uint16_t>(h, 0x58);
					*(uint16_t *)(pk + 0x24) = F<uint16_t>(h, 0x70);
					*(uint16_t *)(pk + 0x30) = F<uint16_t>(h, 0x88);
					*(uint32_t *)(pk + 8) = F<uint32_t>(h, 0x38);
					*(uint32_t *)(pk + 0x14) = F<uint32_t>(h, 0x50);
					*(uint32_t *)(pk + 0x20) = F<uint32_t>(h, 0x68);
					*(uint32_t *)pk = 0xC000000;
					*(uint32_t *)(pk + 0x2C) = F<uint32_t>(h, 0x80);
					pk[7] = 0x3C;
					InsertPrimAutoDepth(Bucket(h, ot, shift), pk);
					pk += 0x34;
					rec += 0x1C;
					continue;
				}
			}
			else if (side == 1)
			{
				*(uint32_t *)pk = 0x9000000;
				*(uint32_t *)(pk + 4) = *(const uint32_t *)rec;
				*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(rec + 0xC);
				*(uint32_t *)(pk + 0x18) = *(const uint32_t *)(rec + 0x10);
				*(uint32_t *)(pk + 0x24) = *(const uint32_t *)(rec + 8) >> 16;
				*(uint32_t *)(pk + 8) = *(const uint32_t *)p0;
				*(uint32_t *)(pk + 0x14) = *(const uint32_t *)p1;
				*(uint32_t *)(pk + 0x20) = *(const uint32_t *)p2;
				*(uint32_t *)(pk + 0x10) = *(const uint32_t *)(rec + 0x14);
				*(uint32_t *)(pk + 0x1C) = *(const uint32_t *)(rec + 0x18);
				GteReadOTZWord(h + 0x24);
			}
			if (side)
			{
				InsertPrimAutoDepth(Bucket(h, ot, shift), pk);
				pk += 0x28;
			}
			rec += 0x1C;
		} while (--left != 0);
		*(uint8_t **)(h + 0x20) = rec;
		return (uint32_t)pk;
	}

	static uint32_t RenderCutModel(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor, const uint8_t *proj)
	{
		uint8_t *m = *(uint8_t **)h;
		*(uint8_t **)(h + 0x20) = m + *(const uint32_t *)m + 8;
		const uint32_t c = CutTrianglesFT(h, ot, shift, cursor, proj);
		*(uint8_t **)(h + 0x20) += 0xC;
		return CutTrianglesGT(h, ot, shift, c, proj);
	}

	static void PartDraw(prim::Layout *l, prim::Record *r, const PrimArg *arg, int32_t frame, uint8_t *proj)
	{
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
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
		if (r->flags & 0x400) RotationFromAnglesC(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		int16_t off[4] = {};
		const int32_t scaled = arg->scaled;
		if (scaled)
		{
			off[0] = (int16_t)(mul32(r->pos[0], arg->scale[0]) >> 12);
			off[1] = (int16_t)(mul32(r->pos[1], arg->scale[1]) >> 12);
			off[2] = (int16_t)(mul32(r->pos[2], arg->scale[2]) >> 12);
		}
		else
		{
			off[0] = r->pos[0];
			off[1] = r->pos[1];
			off[2] = r->pos[2];
		}
		if (scaled) Scale3DMatrix(&m, arg->scale);
		if (r->flags & 0x1000)
		{
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			GteSetRotMatrixCtrl(EffectCamera());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			int32_t v[3];
			GteReadMAC123(v);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)v[1]);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)v[0]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)v[2]);
			MatrixMultiply(EffectCamera(), &m);
		}
		else
		{
			off[0] = (int16_t)(off[0] + arg->pos[0]);
			off[1] = (int16_t)(off[1] + arg->pos[1]);
			off[2] = (int16_t)(off[2] + arg->pos[2]);
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			MatrixMultiply(&Camera(), &m);
		}
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (r->flags & 0x100)
			{
				Mat4x3 d = {};
				d.m[0][0] = r->scale[0];
				d.m[1][1] = r->scale[1];
				d.m[2][2] = r->scale[2];
				MatrixMultiply3(&m, &d);
			}
			else
			{
				int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
				Scale3DMatrix(&m, v);
			}
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)(h + 0x1C) = (r->flags & 0x4000) ? 0x2000 : 0x2030;
		if (r->flags & 0x2000) *(uint32_t *)(h + 0x1C) |= 0xC;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) |= 0xC0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		if ((int16_t)r->index <= 0x1B && frame < 0x32)
		{
			const int32_t cut = -((int32_t)r->pos[1] / 2);
			*(int32_t *)(h + 0x2C) = cut;
			ProjectCut(*(const uint8_t **)(h + 4), proj, *(const int32_t *)(model + 4), cut);
			PacketCursor() = RenderCutModel(h, RenderOT(0x44), 0, PacketCursor(), proj);
		}
		else PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void __cdecl PartCallback(prim::Layout *l, prim::Record *r, int arg)
	{
		PartDraw(l, r, (const PrimArg *)arg, l->frame, (uint8_t *)(TexBase() + BUF_Proj));
	}

	static void DebrisDraw(const DebrisNode *d)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		Mat4x3 m = {};
		BuildRotationMatrixFromAngles(d->rot, &m);
		m.t[0] = d->pos[0];
		m.t[1] = d->pos[1];
		m.t[2] = d->pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)h = game_addr(MODEL_Debris);
		*(uint32_t *)(h + 0x1C) = 0;
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void DebrisMove(DebrisNode *d)
	{
		d->rot[0] = (int16_t)(d->rot[0] + d->spin[0]);
		d->rot[2] = (int16_t)(d->rot[2] + d->spin[2]);
		d->vel[1] = (int16_t)(d->vel[1] + 0xA);
		d->rot[1] = (int16_t)(d->rot[1] + d->spin[1]);
		d->pos[0] = (int16_t)(d->pos[0] + d->vel[0]);
		d->pos[1] = (int16_t)(d->pos[1] + d->vel[1]);
		d->pos[2] = (int16_t)(d->pos[2] + d->vel[2]);
		if (d->pos[1] > 0)
		{
			const int16_t v = d->vel[1];
			d->pos[1] = 0;
			d->vel[1] = (int16_t)((int16_t)(v >> 2) - v);
		}
	}

	static void SparkDraw(const SparkNode *p)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		TransformCameraByShadowRotation(p->pos, 0x1000, (int32_t)0xFFFFFC00);
		*(uint32_t *)h = game_addr(SEQ_Spark);
		*(int16_t *)(h + 4) = p->frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void SparkMove(SparkNode *p)
	{
		p->vel[1] = (int16_t)(p->vel[1] + p->acc[1]);
		p->vel[2] = (int16_t)(p->vel[2] + p->acc[2]);
		p->vel[0] = (int16_t)(p->vel[0] + p->acc[0]);
		p->pos[1] = (int16_t)(p->pos[1] + p->vel[1]);
		p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
		p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
	}
}
}

namespace ff8fx
{
namespace break117
{
	static uint32_t __cdecl DebrisTask(TaskNode *n)
	{
		DebrisNode *d = (DebrisNode *)n;
		DebrisDraw(d);
		DebrisMove(d);
		d->counter++;
		return d->counter < 0x1E ? 0 : TASK_END;
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *p = (SparkNode *)n;
		SparkDraw(p);
		p->frame++;
		if (p->frame >= 0xE) return TASK_END;
		SparkMove(p);
		return 0;
	}

	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		PrimArg arg;
		*(uint32_t *)&arg.pos[0] = *(const uint32_t *)&t->pos[0];
		*(uint32_t *)&arg.pos[2] = *(const uint32_t *)&t->pos[2];
		const int32_t scale = ((int32_t)*(const int16_t *)(t->entity + 0x26) >> 2) + 0x400;
		arg.scale[2] = scale;
		arg.scale[1] = scale;
		arg.scale[0] = scale;
		arg.scaled = 1;
		arg.morph = TexBase() + BUF_Morph;
		prim::play((prim::Layout *)t->layout, PartCallback, (int)&arg, 0);
		uint8_t *e = t->entity;
		if ((uint32_t)(t->counter - 0x14) < 0x40)
		{
			do
			{
				e[0] |= 8;
				e = *(uint8_t **)(e + 0x8C);
			} while (e && e != t->entity);
		}
		else
		{
			*(uint16_t *)e &= 0xFFFB;
			e = t->entity;
			do
			{
				*(uint16_t *)e &= 0xFFF7;
				e = *(uint8_t **)(e + 0x8C);
			} while (e && e != t->entity);
		}
		const uint32_t stone = (uint32_t)(t->counter - 0x18);
		if (stone < 0x3C)
		{
			const uint32_t level = StoneLevel(stone);
			const uint32_t unused[2] = { 0, 0x01000100 };
			t->entity[0] |= 4;
			DrawStoneTarget(t->entity, (int32_t)level, unused, STONE_TPage, STONE_Clut);
		}
		if (t->counter == 0)
		{
			int16_t pos[4];
			GetDefaultEffectPosition(t->entity, pos);
			BdPlaySE3D(SOUND_Break(), 0x101, pos);
		}
		const int32_t e3 = t->counter - 0x32;
		if ((uint32_t)e3 < 0x1E && !(e3 & 1))
		{
			DebrisNode *d = (DebrisNode *)AddTaskToQueue(QDebris(), game_addr(ORIG_DebrisTask));
			if (d)
			{
				Memset32(&d->counter, 0, 9);
				const int32_t dist = (int32_t)((uint32_t)mul32(e3 + 0x28, scale) >> 10);
				const int32_t a = CrtRand();
				d->vel[0] = (int16_t)(mul32(ComputeCos(a), dist) >> 12);
				const int16_t vz = (int16_t)(mul32(ComputeSin(a), dist) >> 12);
				d->vel[1] = 0;
				d->vel[2] = vz;
				d->pos[0] = (int16_t)(d->vel[0] + t->pos[0]);
				d->pos[2] = (int16_t)(t->pos[2] + vz);
				const int32_t h = CrtRand();
				d->pos[1] = (int16_t)(0 - (uint32_t)((h & 0xFF) + shl32(dist, 5)));
				d->rot[0] = (int16_t)CrtRand();
				d->rot[1] = (int16_t)CrtRand();
				d->rot[2] = (int16_t)CrtRand();
				d->spin[0] = (int16_t)((CrtRand() & 0x7F) - 0x40);
				d->spin[1] = (int16_t)((CrtRand() & 0x7F) - 0x40);
				d->spin[2] = (int16_t)((CrtRand() & 0x7F) - 0x40);
			}
		}
		if ((uint32_t)(t->counter - 0x32) < 0x14)
		{
			SparkNode *p = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
			if (p)
			{
				Memset32(&p->frame, 0, 7);
				const int32_t speed = (CrtRand() & 0x7F) + 0x40;
				const int32_t a = CrtRand();
				p->vel[0] = (int16_t)(mul32(ComputeCos(a), speed) >> 12);
				const int16_t vz = (int16_t)(mul32(ComputeSin(a), speed) >> 12);
				p->acc[0] = (int16_t)(-(int32_t)p->vel[0] >> 4);
				p->vel[2] = vz;
				p->acc[2] = (int16_t)(-(int32_t)vz >> 4);
				p->vel[1] = 0;
				p->acc[1] = 0;
				p->pos[0] = (int16_t)(p->vel[0] + t->pos[0]);
				p->pos[2] = (int16_t)(t->pos[2] + vz);
				p->pos[1] = t->pos[1];
			}
		}
		if (t->counter == 0x4E) ApplyActionResultToTarget(Ctx()->actions[t->action].targets);
		if (t->counter >= 0x54) return TASK_END;
		t->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0x5EF0;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x1DEF0;
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QTargets(), (void *)(TexBase() + 0xAA0), 0x1570, 3);
				InitTaskQueuePool(QDebris(), (void *)(TexBase() + 0x500), 0x30, 0x1E);
				InitTaskQueuePool(QSparks(), (void *)TexBase(), 0x28, 0x20);
				for (uint32_t off = BUF_Dither; off < 0x5EF0; off += 2)
					*(uint16_t *)(TexBase() + off) = (uint16_t)CrtRand();
			}
			ActionData *acts = Ctx()->actions;
			if (r->action <= acts[0].last_action)
			{
				uint8_t *entity = Entity(acts[r->action].targets[0]);
				TargetNode *pool = (TargetNode *)(TexBase() + 0xAA0);
				bool busy = false;
				for (int k = 0; k < 3; k++)
					if ((*(const uint8_t *)&pool[k].hdr.flags & 1) && pool[k].entity == entity)
					{
						busy = true;
						break;
					}
				if (busy) r->counter = 0;
				else
				{
					TargetNode *t = (TargetNode *)AddTaskToQueue(QTargets(), game_addr(ORIG_TargetTask));
					if (t)
					{
						Memset32(&t->counter, 0, 0x559);
						t->action = (int16_t)(uint16_t)r->action;
						t->entity = entity;
						GetEffectSpawnPosition(entity, 0xF1, 0, t->pos);
						t->pos[1] = *(const int16_t *)(entity + 0x24);
						DecodeModelPrimLayout(game_addr(MODEL_Shell), t->layout, 0x1554);
						r->action++;
					}
				}
			}
		}
		EffectCameraMatrix(&Camera(), EffectCamera());
		int a, b, c;
		if (r->started)
		{
			a = ExecuteTaskQueue(QTargets());
			b = ExecuteTaskQueue(QDebris());
			c = ExecuteTaskQueue(QSparks());
		}
		else a = b = c = (int)n;
		if (r->started && !a && !b && !c) return TASK_END;
		r->counter++;
		if (r->counter >= 0x14) r->counter = 0;
		return 0;
	}
}

	void register_mag117_break()
	{
		register_port(break117::ORIG_RootTask, (void *)break117::RootTask, "B117 RootTask", 117);
		register_port(break117::ORIG_TargetTask, (void *)break117::TargetTask, "B117 TargetTask", 117);
		register_port(break117::ORIG_DebrisTask, (void *)break117::DebrisTask, "B117 DebrisTask", 117);
		register_port(break117::ORIG_SparkTask, (void *)break117::SparkTask, "B117 SparkTask", 117);
	}
}

