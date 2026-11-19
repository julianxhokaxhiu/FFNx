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

// Effect 107: Demi

#include "mag_common.h"

namespace ff8fx
{
namespace demi107
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_2521B34)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_2521B38)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_2521B54)); }
	inline TaskQueue *QSparks() { return (TaskQueue *)game_addr(game::unk_2521B68); }
	inline TaskQueue *QMotes() { return (TaskQueue *)game_addr(game::unk_2521B78); }
	inline TaskQueue *QOrbs() { return (TaskQueue *)game_addr(game::unk_2521B88); }
	inline TaskQueue *QTargets() { return (TaskQueue *)game_addr(game::unk_2521B98); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t &BoneWorkspace() { return var<uint32_t>(game_addr(game::g_BattleVertexWorkspace)); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_107_DEMI_Director_Tick;
	static constexpr game::Id ORIG_TargetTask = game::MAG_107_Demi_TargetTask;
	static constexpr game::Id ORIG_MoteTask = game::MAG_107_Demi_MoteTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_107_Demi_SparkTask;
	static constexpr game::Id ORIG_OrbTask = game::MAG_107_Demi_OrbTask;
	static constexpr game::Id MODEL_Vortex = game::unk_12EC9D4;
	static constexpr game::Id PART_Flags = game::unk_12F9998;
	static constexpr game::Id SEQ_Burst = game::unk_12EC358;
	static constexpr game::Id SEQ_Ring = game::unk_12EC8F8;
	static constexpr game::Id SEQ_Mote = game::unk_12EC7CC;
	static constexpr game::Id SEQ_Orb = game::unk_12EC6D4;
	static constexpr game::Id SEQ_SparkA = game::unk_12EC414;
	static constexpr game::Id SEQ_SparkB = game::unk_12EC574;
	static Mat4x3 *MOTE_Matrix() { return (Mat4x3 *)game_addr(game::unk_12F9978); }
	static constexpr game::Id PULL_Distance = game::unk_12F5978;
	static constexpr game::Id PULL_Weights = game::unk_12F3978;
	static const void *SOUND_Demi() { return (const void *)game_addr(game::unk_12F3974); }

	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_MorphVertices))(model, f0, f1, t, out); }
	inline uint32_t RenderPrimModel2(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline uint32_t DrawShadow(uint8_t *entity, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, mode, cursor); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteLoadV0u(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
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
		int16_t radius;
		int16_t size;
		uint8_t *entity;
		int16_t pos[4];
		uint8_t vortex[0x278];
	};
	struct MoteNode
	{
		TaskNode hdr;
		int16_t frame;
		int16_t pad0E;
		TargetNode *owner;
		int16_t angle;
		int16_t radius;
		int16_t vel;
		int16_t acc;
	};
	struct OrbNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		TargetNode *owner;
		int16_t angle;
		int16_t reach;
		int16_t rot[3];
		int16_t pad1E;
	};
	struct SparkNode
	{
		TaskNode hdr;
		int16_t frame;
		int16_t end;
		uint32_t seq;
		int16_t pos[3];
		int16_t scale;
	};
	struct PrimArg
	{
		int16_t pos[4];
		uint32_t flags;
		TargetNode *owner;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(TargetNode) == 0x298 && sizeof(MoteNode) == 0x1C
		&& sizeof(OrbNode) == 0x20 && sizeof(SparkNode) == 0x1C, "Demi nodes");
	static_assert(sizeof(PrimArg) == 0x14, "Demi prim block");

	template<typename T> static inline T &F(uint8_t *s, uint32_t off) { return *(T *)(s + off); }

	static void PullVertices(uint8_t **objp, uint8_t **outp, uint8_t *s)
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
			int16_t sy = (int16_t)(F<int16_t>(s, 0x26) >> 3);
			int16_t sx = (int16_t)(F<int16_t>(s, 0x24) >> 3);
			GteReadOTZ(s + 0x28);
			if (F<uint16_t>(s, 0x28) == 0)
				*(uint16_t *)(out + 6) = 0x10;
			else
			{
				const int16_t cx = F<int16_t>(s, 0x5A);
				const int16_t cy = F<int16_t>(s, 0x5C);
				const int32_t dx = (int16_t)(sx - cx);
				const int32_t dy = (int16_t)(sy - cy);
				int32_t adx = dx;
				if ((int16_t)dx < 0) adx = -adx;
				if ((int16_t)adx >= 0x80) adx = 0x80;
				int32_t ady = dy;
				if ((int16_t)dy < 0) ady = -ady;
				if ((int16_t)ady >= 0x80) ady = 0x80;
				if (F<int16_t>(s, 0x5E) < (int16_t)adx) F<int16_t>(s, 0x5E) = (int16_t)adx;
				if (F<int16_t>(s, 0x5E) < (int16_t)ady) F<int16_t>(s, 0x5E) = (int16_t)ady;
				const uint32_t d = *(const uint8_t *)(game_addr(PULL_Distance) + (int32_t)(int16_t)adx + shl32((int16_t)ady, 7));
				if (d != 0)
				{
					const int32_t radius = F<int16_t>(s, 0x58);
					if ((int32_t)d < radius)
					{
						const int32_t q = (int32_t)(d << 7) / radius;
						const uint32_t w = *(const uint16_t *)(F<uint32_t>(s, 0x54) + q * 2);
						sx = (int16_t)((mul32(dx, (int32_t)w) >> 12) + cx);
						sy = (int16_t)((mul32(dy, (int32_t)w) >> 12) + cy);
						F<int16_t>(s, 0x24) = (int16_t)(sx << 3);
						F<int16_t>(s, 0x26) = (int16_t)(sy << 3);
					}
				}
				if (sx < F<int16_t>(s, 0x14)) s[0x2A] |= 1;
				else if (sx >= F<int16_t>(s, 0x18)) s[0x2A] |= 2;
				if (sy < F<int16_t>(s, 0x16)) s[0x2A] |= 4;
				else if (sy >= F<int16_t>(s, 0x1A)) s[0x2A] |= 8;
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

	static uint32_t FlatPolygons(uint8_t *s, uint32_t ot, int32_t shift, uint32_t cursor)
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
						GteSetSZ123(F<uint16_t>(s, 0x28), F<uint16_t>(s, 0x30), F<uint16_t>(s, 0x38));
						GteAVSZ3();
						*(uint32_t *)pk = 0x7000000;
						*(uint32_t *)(pk + 4) = ((((uint32_t)*(const uint16_t *)(p + 0xE) & 0x200) | 0x2400) << 16) | F<uint32_t>(s, 0x1C);
						*(uint32_t *)(pk + 8) = F<uint32_t>(s, 0x24);
						*(uint32_t *)(pk + 0x10) = F<uint32_t>(s, 0x2C);
						*(uint32_t *)(pk + 0x18) = F<uint32_t>(s, 0x34);
						*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 8);
						*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(p + 0xC);
						*(uint16_t *)(pk + 0x16) &= 0xFDFF;
						*(uint16_t *)(pk + 0x1C) = *(const uint16_t *)(p + 6);
						GteReadOTZWord(s + 0x4C);
						InsertPrimAutoDepth(ot + (uint32_t)(F<int32_t>(s, 0x4C) >> shift) * 4, pk);
						pk += 0x20;
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
						GteSetSZ0123(F<uint16_t>(s, 0x28), F<uint16_t>(s, 0x30), F<uint16_t>(s, 0x38), F<uint16_t>(s, 0x40));
						GteAVSZ4();
						*(uint32_t *)pk = 0x9000000;
						*(uint32_t *)(pk + 4) = ((((uint32_t)*(const uint16_t *)(p + 0xA) & 0x200) | 0x2C00) << 16) | F<uint32_t>(s, 0x1C);
						*(uint32_t *)(pk + 8) = F<uint32_t>(s, 0x24);
						*(uint32_t *)(pk + 0x10) = F<uint32_t>(s, 0x2C);
						*(uint32_t *)(pk + 0x18) = F<uint32_t>(s, 0x34);
						*(uint32_t *)(pk + 0x20) = F<uint32_t>(s, 0x3C);
						*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(p + 4);
						*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(p + 8);
						*(uint16_t *)(pk + 0x1C) = *(const uint16_t *)(p + 0xC);
						*(uint16_t *)(pk + 0x16) &= 0xFDFF;
						*(uint16_t *)(pk + 0x24) = *(const uint16_t *)(p + 0xE);
						GteReadOTZWord(s + 0x4C);
						InsertPrimAutoDepth(ot + (uint32_t)(F<int32_t>(s, 0x4C) >> shift) * 4, pk);
						pk += 0x28;
					}
				}
				k = F<uint16_t>(s, 0xA);
				p += 0x14;
				F<uint16_t>(s, 0xA) = (uint16_t)(k - 1);
			} while (k != 0);
		}
		return (uint32_t)pk;
	}

	static uint32_t PullModel(const uint8_t *model, uint8_t *s, uint32_t ot, int32_t shift, uint32_t cursor)
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
					PullVertices(&obj, &out, s);
				} while (--groups != 0);
			}
			obj = (uint8_t *)(((uintptr_t)obj + 3) & ~(uintptr_t)3);
			for (int w = 0; w < 6; w++)
			{
				F<uint16_t>(s, 8 + 2 * w) = *(const uint16_t *)obj;
				obj += 2;
			}
			F<uint8_t *>(s, 0) = obj;
			cursor = FlatPolygons(s, ot, shift, cursor);
		}
		return cursor;
	}

	static int32_t DrawSilhouette(uint8_t *entity, int16_t sx, int16_t sy, int16_t radius, int32_t step)
	{
		uint8_t *c = (uint8_t *)FieldAlloc(0x80);
		uint8_t *s = c + 0x20;
		F<int16_t>(c, 0x7A) = sx;
		F<int16_t>(c, 0x7C) = sy;
		F<uint32_t>(c, 0x74) = game_addr(PULL_Weights) + (uint32_t)shl32(step, 8);
		F<int16_t>(c, 0x78) = radius;
		F<int16_t>(c, 0x7E) = 0;
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(entity + 0x40), (Mat4x3 *)c);
		FrameCursor() = DrawShadow(entity, RenderOT(0x4040), 0x10, FrameCursor());
		ComputeBonesWorldMatrices(entity + 0x60, c);
		F<int16_t>(c, 0x34) = 0;
		F<uint32_t>(c, 0x24) = BoneWorkspace();
		F<int16_t>(c, 0x36) = 0;
		F<int16_t>(c, 0x38) = 0x140;
		F<int16_t>(c, 0x3A) = 0xD8;
		F<uint32_t>(c, 0x3C) = *(const uint32_t *)(entity + 0x28);
		F<uint32_t>(c, 0x40) = *(const uint32_t *)(entity + 0x7C);
		FrameCursor() = PullModel(*(const uint8_t *const *)(entity + 0x64), s, RenderOT(0x44), 0, FrameCursor());
		BuildBoneMatricesFromPose(entity + 0x60);
		uint8_t *second = *(uint8_t *const *)(entity + 0x78);
		if (second)
		{
			F<uint32_t>(c, 0x40) = 0xFFFFFFFF;
			ComputeBonesWorldMatrices(second, c);
			FrameCursor() = PullModel(*(const uint8_t *const *)(second + 4), s, RenderOT(0x44), 0, FrameCursor());
			BuildBoneMatricesFromPose(second);
		}
		FieldFree(0x80);
		return F<int16_t>(c, 0x7E);
	}

	static void HideTarget(uint8_t *entity)
	{
		entity[0] |= 4;
		uint8_t *e = entity;
		do
		{
			e[0] |= 8;
			e = *(uint8_t **)(e + 0x8C);
		} while (e && e != entity);
	}

	static void ShowTarget(uint8_t *entity)
	{
		*(uint16_t *)entity &= 0xFFFB;
		uint8_t *e = entity;
		do
		{
			*(uint16_t *)e &= 0xFFF7;
			e = *(uint8_t **)(e + 0x8C);
		} while (e && e != entity);
	}

	static int32_t PullStep(uint32_t e)
	{
		int32_t step;
		if (e < 0xF) step = 0x10 - (int32_t)((uint32_t)(((uint64_t)(e << 4) * 0x88888889u) >> 32) >> 3);
		else if (e < 0x1E) step = (int32_t)((uint32_t)(((uint64_t)((e - 0xF) << 4) * 0x88888889u) >> 32) >> 3);
		else
		{
			step = (int32_t)((uint32_t)(((uint64_t)((e - 0x1E) << 4) * 0x88888889u) >> 32) >> 3) + 0x10;
			if (step <= 0x1F) return step;
			step = 0x40 - step;
		}
		if (step > 0x1F) step = 0x1F;
		return step;
	}

	static inline uint8_t *&Cursor20(uint8_t *h) { return *(uint8_t **)(h + 0x20); }

	static uint32_t VortexTriangles(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *rec = Cursor20(h);
		const int32_t count = *(const int32_t *)rec;
		rec += 4;
		const uint8_t *verts = *(uint8_t **)(h + 4);
		Cursor20(h) = rec;
		uint8_t *p = (uint8_t *)cursor;
		if (count <= 0)
		{
			Cursor20(h) = rec;
			return (uint32_t)p;
		}
		for (int32_t left = count; left; left--, rec += 0x14)
		{
			GteLoadV012(verts + 4 * *(const uint16_t *)(rec + 4), verts + 4 * *(const uint16_t *)(rec + 6), verts + 4 * *(const uint16_t *)(rec + 8));
			GteRTPT();
			*(uint32_t *)p = 0x6000000;
			*(uint32_t *)(p + 4) = *(const uint32_t *)rec;
			GteReadFLAG(h + 0x30);
			if (*(const uint32_t *)(h + 0x30) & 0x60000) continue;
			GteNCLIP();
			uint32_t clip = 0;
			GteReadMAC0(h + 0x24);
			if (*(const int32_t *)(h + 0x24) < 0 && !(h[0x1C] & 0x20)) continue;
			GteReadSXY012(p + 8, p + 0x10, p + 0x18);
			GteAVSZ3();
			if (*(const uint16_t *)(p + 8) > 0xA00) clip = 1;
			if (*(const uint16_t *)(p + 0x10) > 0xA00) clip |= 2;
			if (*(const uint16_t *)(p + 0x18) > 0xA00) clip |= 4;
			if (*(const uint16_t *)(p + 0xA) > 0x6C0) clip |= 0x10;
			if (*(const uint16_t *)(p + 0x12) > 0x6C0) clip |= 0x20;
			if (*(const uint16_t *)(p + 0x1A) > 0x6C0) clip |= 0x40;
			if ((clip & 7) == 7 || (clip & 0x70) == 0x70) continue;
			GteReadOTZWord(h + 0x2C);
			*(uint32_t *)(p + 0xC) = *(const uint32_t *)(rec + 0xC);
			*(uint32_t *)(p + 0x14) = *(const uint32_t *)(rec + 0x10);
			const int32_t z = (int32_t)((uint32_t)*(const int32_t *)(h + 0x2C) + *(const uint32_t *)(h + 0xC));
			*(int32_t *)(h + 0x2C) = z;
			if (z < 0) continue;
			const uint32_t otp = ot + 4 * (z >> shift);
			uint8_t *main = p;
			uint8_t *m1 = p + 0x1C;
			*(uint32_t *)m1 = 0x1000000;
			*(uint32_t *)(m1 + 4) = 0xE1000220;
			InsertPrimAutoDepth(otp, m1);
			InsertPrimAutoDepth(otp, main);
			uint8_t *m2 = p + 0x24;
			*(uint32_t *)m2 = 0x1000000;
			*(uint32_t *)(m2 + 4) = 0xE1000240;
			InsertPrimAutoDepth(otp, m2);
			p += 0x2C;
		}
		Cursor20(h) = rec;
		return (uint32_t)p;
	}

	static uint32_t VortexQuads(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *rec = Cursor20(h);
		const int32_t count = *(const int32_t *)rec;
		rec += 4;
		const uint8_t *verts = *(uint8_t **)(h + 4);
		Cursor20(h) = rec;
		uint8_t *p = (uint8_t *)cursor;
		if (count <= 0)
		{
			Cursor20(h) = rec;
			return (uint32_t)p;
		}
		for (int32_t left = count; left; left--, rec += 0x18)
		{
			GteLoadV012(verts + 4 * *(const uint16_t *)(rec + 4), verts + 4 * *(const uint16_t *)(rec + 6), verts + 4 * *(const uint16_t *)(rec + 8));
			GteRTPT();
			*(uint32_t *)p = 0x8000000;
			*(uint32_t *)(p + 4) = *(const uint32_t *)rec;
			GteReadFLAG(h + 0x30);
			if (*(const uint32_t *)(h + 0x30) & 0x60000) continue;
			GteNCLIP();
			uint32_t clip = 0;
			GteReadMAC0(h + 0x24);
			if (*(const int32_t *)(h + 0x24) < 0 && !(h[0x1C] & 0x20)) continue;
			GteReadSXY012(p + 8, p + 0x10, p + 0x18);
			GteLoadV0(verts + 4 * *(const uint16_t *)(rec + 0xA));
			GteRTPS();
			if (*(const uint16_t *)(p + 8) > 0xA00) clip = 1;
			if (*(const uint16_t *)(p + 0x10) > 0xA00) clip |= 2;
			if (*(const uint16_t *)(p + 0x18) > 0xA00) clip |= 4;
			if (*(const uint16_t *)(p + 0xA) > 0x6C0) clip |= 0x10;
			if (*(const uint16_t *)(p + 0x12) > 0x6C0) clip |= 0x20;
			if (*(const uint16_t *)(p + 0x1A) > 0x6C0) clip |= 0x40;
			GteReadSXY2(p + 0x20);
			GteAVSZ4();
			if (*(const uint16_t *)(p + 0x20) > 0xA00) clip |= 8;
			if (*(const uint16_t *)(p + 0x22) > 0x6C0) clip |= 0x80;
			if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
			GteReadOTZWord(h + 0x2C);
			*(uint32_t *)(p + 0xC) = *(const uint32_t *)(rec + 0xC);
			*(uint32_t *)(p + 0x14) = *(const uint32_t *)(rec + 0x10);
			*(uint32_t *)(p + 0x1C) = *(const uint32_t *)(rec + 0x14);
			const int32_t z = (int32_t)((uint32_t)*(const int32_t *)(h + 0x2C) + *(const uint32_t *)(h + 0xC));
			*(int32_t *)(h + 0x2C) = z;
			if (z < 0) continue;
			const uint32_t otp = ot + 4 * (z >> shift);
			uint8_t *main = p;
			uint8_t *m1 = p + 0x24;
			*(uint32_t *)m1 = 0x1000000;
			*(uint32_t *)(m1 + 4) = 0xE1000220;
			InsertPrimAutoDepth(otp, m1);
			InsertPrimAutoDepth(otp, main);
			uint8_t *m2 = p + 0x2C;
			*(uint32_t *)m2 = 0x1000000;
			*(uint32_t *)(m2 + 4) = 0xE1000240;
			InsertPrimAutoDepth(otp, m2);
			p += 0x34;
		}
		Cursor20(h) = rec;
		return (uint32_t)p;
	}

	static uint32_t RenderVortex(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *m = *(uint8_t **)h;
		Cursor20(h) = m + *(const uint32_t *)m + 0x10;
		uint32_t r = cursor;
		if (*(const uint32_t *)Cursor20(h) != 0) r = VortexTriangles(h, ot, shift, r);
		else Cursor20(h) += 4;
		if (*(const uint32_t *)Cursor20(h) != 0) return VortexQuads(h, ot, shift, r);
		Cursor20(h) += 4;
		return r;
	}

	static void AngleVector(const int16_t *ang, int16_t *out)
	{
		const int32_t ca = ComputeCos(ang[0]);
		out[1] = (int16_t)-ComputeSin(ang[0]);
		out[0] = (int16_t)(mul32(ComputeSin(ang[1]), ca) >> 12);
		out[2] = (int16_t)((int32_t)(0u - (uint32_t)mul32(ComputeCos(ang[1]), ca)) >> 12);
	}

	static void SpawnFromVortex(prim::Layout *l, prim::Record *r, PrimArg *arg)
	{
		arg->owner->size = r->scale[0];
		if (l->frame == 0xE)
		{
			for (int k = 0; k < 0x20; k++)
			{
				OrbNode *o = (OrbNode *)AddTaskToQueue(QOrbs(), game_addr(ORIG_OrbTask));
				if (!o) continue;
				Memset32(&o->counter, 0, 4);
				o->owner = arg->owner;
				o->counter = (int16_t)((CrtRand() * 9) >> 15);
				o->angle = (int16_t)CrtRand();
				o->rot[0] = (int16_t)CrtRand();
				o->rot[1] = (int16_t)CrtRand();
				o->rot[2] = (int16_t)CrtRand();
				o->reach = (int16_t)((CrtRand() & 0x3FF) + 0x5DC);
			}
		}
		const uint32_t e = (uint32_t)(l->frame - 0x1E);
		if (e < 0x25)
		{
			const int32_t chance = e > 0x12 ? 0x3000 : 0x6000;
			for (int k = 0; k < 2; k++)
			{
				if (CrtRand() >= chance) continue;
				SparkNode *p = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
				if (!p) continue;
				Memset32(&p->frame, 0, 4);
				const int32_t pick = CrtRand() & 1;
				if (pick == 0 || pick == 1)
				{
					p->seq = pick == 0 ? game_addr(SEQ_SparkA) : game_addr(SEQ_SparkB);
					p->end = 0xD;
					p->scale = 0x1000;
				}
				int16_t ang[2];
				ang[0] = (int16_t)CrtRand();
				ang[1] = (int16_t)CrtRand();
				AngleVector(ang, p->pos);
				const int32_t q = r->scale[0] >> 2;
				const int32_t dist = (mul32(CrtRand(), q) >> 17) + q;
				p->pos[0] = (int16_t)((mul32(p->pos[0], dist) >> 12) + arg->pos[0]);
				p->pos[1] = (int16_t)((mul32(p->pos[1], dist) >> 12) + arg->pos[1]);
				p->pos[2] = (int16_t)((mul32(p->pos[2], dist) >> 12) + arg->pos[2]);
			}
		}
	}

	static void VortexDraw(prim::Layout *l, prim::Record *r, const PrimArg *arg)
	{
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
		ComposeZYXRotationMatrix(r->rot, &m);
		m.t[0] = (int32_t)r->pos[0] + arg->pos[0];
		m.t[1] = (int32_t)r->pos[1] + arg->pos[1];
		m.t[2] = (int32_t)r->pos[2] + arg->pos[2];
		const uint32_t *flags = (const uint32_t *)arg->flags;
		if (flags[(int16_t)r->index] & 0x10000000)
		{
			TransformVectorBy3x3Matrix(&Camera(), m.t, m.t);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2]);
		}
		else ComposeAffineTransform(&Camera(), &m, &m);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
			Scale3DMatrix(&m, v);
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		const uint32_t mode = (flags[(int16_t)r->index] & 0xDFFF) | 0x2000;
		*(uint32_t *)(h + 0x1C) = mode;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) = mode | 0xC0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		const int16_t index = (int16_t)r->index;
		const uint32_t f = flags[index];
		if (f & 0x20000000)
		{
			if (index == 9) *(int32_t *)(h + 0xC) = -0x80;
			else *(int32_t *)(h + 0xC) = index == 5 ? 0x80 : 0;
			PacketCursor() = RenderVortex(h, RenderOT(0x44), 2, PacketCursor());
		}
		else
		{
			*(int32_t *)(h + 0x18) = (f & 0x40000000) ? -0x200 : 0;
			PacketCursor() = RenderPrimModel2(h, RenderOT(0x44), 2, PacketCursor());
		}
		FieldFree(0x58);
	}

	static inline bool VortexSkips(const prim::Record *r)
	{
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return true;
		return r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0;
	}

	static void __cdecl VortexPart(prim::Layout *l, prim::Record *r, int arg_)
	{
		PrimArg *arg = (PrimArg *)arg_;
		if (VortexSkips(r))
		{
			if (r->index == 0) arg->owner->size = 0;
			return;
		}
		if (r->index == 0) SpawnFromVortex(l, r, arg);
		VortexDraw(l, r, arg);
	}

	static void MoteDraw(const MoteNode *p)
	{
		const TargetNode *t = p->owner;
		const int32_t v[3] = { t->pos[0], t->pos[1], t->pos[2] };
		TransformVectorBy3x3Matrix(&Camera(), v, MOTE_Matrix()->t);
		const int32_t c = ComputeCos(p->angle);
		MOTE_Matrix()->t[0] = (int32_t)((uint32_t)MOTE_Matrix()->t[0] + (uint32_t)((mul32(c, p->radius) >> 12) + Camera().t[0]));
		const int32_t s = ComputeSin(p->angle);
		MOTE_Matrix()->t[1] = (int32_t)((uint32_t)MOTE_Matrix()->t[1] + (uint32_t)((mul32(s, p->radius) >> 12) + Camera().t[1]));
		MOTE_Matrix()->t[2] = (int32_t)((uint32_t)MOTE_Matrix()->t[2] + (uint32_t)Camera().t[2]);
		GteSetRotMatrix(MOTE_Matrix());
		GteSetTransVector(MOTE_Matrix());
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Mote);
		*(int16_t *)(h + 4) = p->frame;
		*(int32_t *)(h + 8) = p->angle;
		*(uint16_t *)(h + 0x24) = 1;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void MoteUpdate(MoteNode *p)
	{
		p->vel = (int16_t)(p->vel + p->acc);
		p->radius = (int16_t)(p->radius + p->vel);
	}

	static void OrbDraw(const OrbNode *o, int16_t size)
	{
		Mat4x3 m = {};
		ComposeZYXRotationMatrix(o->rot, &m);
		const int32_t dist = mul32(size, o->reach) >> 12;
		int16_t v[4] = {};
		v[0] = (int16_t)(mul32(ComputeSin(o->angle), dist) >> 12);
		v[2] = (int16_t)(mul32(ComputeCos(o->angle), dist) >> 12);
		v[1] = 0;
		GteSetRotMatrixCtrl(&m);
		GteLoadV0(v);
		GteMVMVA_RotV0();
		GteStoreIR123(v);
		const TargetNode *t = o->owner;
		v[0] = (int16_t)(v[0] + t->pos[0]);
		v[1] = (int16_t)(v[1] + t->pos[1]);
		v[2] = (int16_t)(v[2] + t->pos[2]);
		TransformCameraByShadowRotation(v, 0x1000, (int32_t)0xFFFFFC00);
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Orb);
		*(uint16_t *)(h + 4) = 1;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void SparkDraw(const SparkNode *p)
	{
		TransformCameraByShadowRotation(p->pos, p->scale, -(p->scale >> 2));
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = p->seq;
		*(int16_t *)(h + 4) = p->frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void BurstDraw(const TargetNode *t, uint32_t seq, int32_t e)
	{
		TransformCameraByShadowRotation(t->pos, 0x2000, (int32_t)0xFFFFF800);
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = seq;
		*(int16_t *)(h + 4) = (int16_t)e;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}
}
}

namespace ff8fx
{
namespace demi107
{
	static uint32_t __cdecl MoteTask(TaskNode *n)
	{
		MoteNode *p = (MoteNode *)n;
		MoteDraw(p);
		MoteUpdate(p);
		if (p->radius <= 0) return TASK_END;
		p->frame++;
		if (p->frame >= 0xB) p->frame = 0;
		return 0;
	}

	static uint32_t __cdecl OrbTask(TaskNode *n)
	{
		OrbNode *o = (OrbNode *)n;
		OrbDraw(o, o->owner->size);
		o->angle = (int16_t)(o->angle + 8);
		if (o->owner->size < 0x200) return TASK_END;
		o->counter++;
		if (o->counter >= 9) o->counter = 0;
		return 0;
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *p = (SparkNode *)n;
		SparkDraw(p);
		p->frame++;
		return p->frame < p->end ? 0 : TASK_END;
	}

	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		if ((uint16_t)t->counter < 0xD)
		{
			int32_t angle = CrtRand();
			for (int k = 0; k < 4; k++)
			{
				MoteNode *p = (MoteNode *)AddTaskToQueue(QMotes(), game_addr(ORIG_MoteTask));
				if (!p) continue;
				Memset32(&p->frame, 0, 4);
				p->owner = t;
				p->angle = (int16_t)angle;
				const int16_t r = (int16_t)((CrtRand() & 0x1FF) + 0x800);
				p->radius = r;
				p->vel = (int16_t)(-(int32_t)r >> 4);
				p->acc = (int16_t)(r >> 10);
				angle += 0x400;
			}
		}
		int32_t e = t->counter - 0x14;
		if ((uint32_t)e < 6)
		{
			BurstDraw(t, game_addr(SEQ_Burst), e);
		}
		e = t->counter - 0x17;
		if ((uint32_t)e < 6)
		{
			BurstDraw(t, game_addr(SEQ_Ring), e);
		}
		if (t->counter == 0x17) ScreenFadeTask(0, 1, 1, 0x80);
		const uint32_t ps = (uint32_t)(t->counter - 0x1E);
		if (ps < 0x3C)
		{
			const int32_t step = PullStep(ps);
			GteSetRotMatrix(&Camera());
			GteSetTransVector(&Camera());
			GteLoadV0(t->pos);
			GteRTPS();
			int16_t sxy[2];
			uint32_t otz;
			GteReadSXY2(sxy);
			GteReadOTZ(&otz);
			sxy[0] = (int16_t)(sxy[0] >> 3);
			sxy[1] = (int16_t)(sxy[1] >> 3);
			if ((uint16_t)otz != 0)
			{
				t->radius = (int16_t)DrawSilhouette(t->entity, sxy[0], sxy[1], t->radius, step);
				HideTarget(t->entity);
			}
			else ShowTarget(t->entity);
		}
		else ShowTarget(t->entity);
		if (t->counter >= 0xA)
		{
			PrimArg arg;
			*(uint32_t *)&arg.pos[0] = *(const uint32_t *)&t->pos[0];
			*(uint32_t *)&arg.pos[2] = *(const uint32_t *)&t->pos[2];
			arg.owner = t;
			arg.flags = game_addr(PART_Flags);
			arg.morph = TexBase() + 0x1DC8;
			prim::play((prim::Layout *)t->vortex, VortexPart, (int)&arg, 0);
		}
		if (t->counter == 0)
		{
			int16_t pos[4];
			GetDefaultEffectPosition(t->entity, pos);
			BdPlaySE3D(SOUND_Demi(), 0x101, pos);
		}
		if (t->counter == 0x56) ApplyActionResultToTarget(Ctx()->actions[t->action].targets);
		if (t->counter >= 0x5B) return TASK_END;
		t->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		bool may_end = true;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0x21C8;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x121C8;
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QTargets(), (void *)TexBase(), 0x298, 3);
				InitTaskQueuePool(QOrbs(), (void *)(TexBase() + 0xEC8), 0x20, 0x40);
				InitTaskQueuePool(QMotes(), (void *)(TexBase() + 0x7C8), 0x1C, 0x40);
				InitTaskQueuePool(QSparks(), (void *)(TexBase() + 0x16C8), 0x1C, 0x40);
			}
			ActionData *acts = Ctx()->actions;
			if (r->action <= acts[0].last_action)
			{
				uint8_t *entity = Entity(acts[r->action].targets[0]);
				TargetNode *pool = (TargetNode *)TexBase();
				bool busy = false;
				for (int k = 0; k < 3; k++)
					if ((*(const uint8_t *)&pool[k].hdr.flags & 1) && pool[k].entity == entity)
					{
						busy = true;
						break;
					}
				if (busy)
				{
					may_end = false;
					r->counter = 0;
				}
				else
				{
					TargetNode *t = (TargetNode *)AddTaskToQueue(QTargets(), game_addr(ORIG_TargetTask));
					Memset32(&t->counter, 0, 0xA3);
					t->action = (int16_t)(uint16_t)r->action;
					t->entity = Entity(Ctx()->actions[t->action].targets[0]);
					int16_t a[4], b[4];
					GetEffectSpawnPosition(t->entity, 0xF1, 0, a);
					GetEffectSpawnPosition(t->entity, 0xF0, 0, b);
					t->pos[0] = (int16_t)(((int32_t)b[0] + a[0]) >> 1);
					t->pos[1] = (int16_t)(((int32_t)b[1] + a[1]) >> 1);
					t->pos[2] = (int16_t)(((int32_t)b[2] + a[2]) >> 1);
					DecodeModelPrimLayout(game_addr(MODEL_Vortex), t->vortex, 0x278);
					r->action++;
				}
			}
		}
		int q0, q1, q2, q3;
		if (r->started)
		{
			q0 = ExecuteTaskQueue(QTargets());
			q1 = ExecuteTaskQueue(QMotes());
			q2 = ExecuteTaskQueue(QOrbs());
			q3 = ExecuteTaskQueue(QSparks());
		}
		else q0 = q1 = q2 = q3 = (int)n;
		if (may_end && r->started && !q0 && !q1 && !q3 && !q2) return TASK_END;
		r->counter++;
		if (r->counter >= 0x2D) r->counter = 0;
		return 0;
	}
}

	void register_mag107_demi()
	{
		register_port(demi107::ORIG_RootTask, (void *)demi107::RootTask, "D107 RootTask", 107);
		register_port(demi107::ORIG_TargetTask, (void *)demi107::TargetTask, "D107 TargetTask", 107);
		register_port(demi107::ORIG_MoteTask, (void *)demi107::MoteTask, "D107 MoteTask", 107);
		register_port(demi107::ORIG_OrbTask, (void *)demi107::OrbTask, "D107 OrbTask", 107);
		register_port(demi107::ORIG_SparkTask, (void *)demi107::SparkTask, "D107 SparkTask", 107);
	}
}

