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

// Effect 279: Mega Spark

#include "mag_common.h"

namespace ff8fx
{
namespace megaspark279
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_279_MegaSpark_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_279_MegaSpark_CasterEntity)); }
	inline TaskQueue *QSparks() { return (TaskQueue *)game_addr(game::MAG_279_MegaSpark_SparksQueue); }
	inline TaskQueue *QPrimA() { return (TaskQueue *)game_addr(game::MAG_279_MegaSpark_PrimAQueue); }
	inline TaskQueue *QPrimB() { return (TaskQueue *)game_addr(game::MAG_279_MegaSpark_PrimBQueue); }
	inline TaskQueue *QBolts() { return (TaskQueue *)game_addr(game::MAG_279_MegaSpark_BoltsQueue); }
	inline TaskQueue *QArcs() { return (TaskQueue *)game_addr(game::MAG_279_MegaSpark_ArcsQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_279_MegaSpark_MainQueue); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_279_MegaSpark_PacketCursor)); }
	inline uint32_t &Seed() { return var<uint32_t>(game_addr(game::MAG_279_MegaSpark_Seed)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_279_MegaSpark_MagicBufferBase)); }
	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_279_MegaSpark_MasterTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_279_MegaSpark_MainTask;
	static constexpr game::Id ORIG_BoltTask = game::MAG_279_MegaSpark_BoltTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_279_MegaSpark_SparkTask;
	static constexpr game::Id ORIG_PrimTask = game::MAG_279_MegaSpark_PrimTask;
	static constexpr game::Id ORIG_ArcTask = game::MAG_279_MegaSpark_ArcTask;
	static constexpr game::Id ORIG_PrimCallback = game::MAG_279_MegaSpark_PrimCallback;
	static const uint32_t POOL_PrimA = 0x140, POOL_PrimB = 0x4F4, POOL_Bolts = 0x18B4, POOL_Arcs = 0x2334, POOL_Main = 0x3334;
	static const uint32_t BUF_Points = 0x3E68;
	static const uint32_t BUF_PointCache = 0x7A68;
	static const uint32_t BUF_Morph = 0x7A6C;
	static const uint32_t BUF_Profile = 0x7C6C, BUF_ProfileEnd = 0x7C7C;
	static const uint32_t ARENA_A = 0x7C7C, ARENA_B = 0x17C7C;
	static constexpr game::Id MODEL_Ring = game::MAG_279_MegaSpark_ModelRing;
	static constexpr game::Id MODEL_Orb = game::MAG_279_MegaSpark_ModelOrb;
	static constexpr game::Id MODEL_Strike = game::MAG_279_MegaSpark_ModelStrike;
	static constexpr game::Id MODEL_Hit = game::MAG_279_MegaSpark_ModelHit;
	static constexpr game::Id SEQ_Spark = game::MAG_279_MegaSpark_SeqSpark;
	static constexpr game::Id ANCHOR_0 = game::unk_10B6D28;
	static constexpr game::Id POINT_Orb = game::unk_10B6F80;
	static constexpr game::Id TRI_Rows = game::unk_10B6D40;
	static constexpr game::Id TBL_Distance = game::MAG_279_MegaSpark_TableDistance;
	static constexpr game::Id TBL_Lengths = game::MAG_279_MegaSpark_TableLengths;
	static constexpr game::Id TBL_Sine = game::MAG_279_MegaSpark_TableSine;
	static const void *SOUND_Spark() { return (const void *)game_addr(game::MAG_279_MegaSpark_SoundSpark); }
	static const uint32_t QUAD_UV0 = 0x3D948048, QUAD_UV1 = 0xB68078;
	static const uint16_t QUAD_UV2 = 0x8048, QUAD_UV3 = 0x8078;

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAngles2(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MulMatrix3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline int32_t BuildOrthonormalBasis(const int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline void RotX(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_063_RotationXMatrix))(angle, out); }
	inline void RotY(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline void GetEffectSpawnPosition(uint8_t *ent, int32_t bone, int32_t mode, int16_t *out) { fn<void (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(ent, bone, mode, out); }
	inline void TriangleCentreNormal(uint32_t bones, uint32_t tri, int16_t *centre, int16_t *normal) { fn<void (__cdecl *)(uint32_t, uint32_t, int16_t *, int16_t *)>(game_addr(game::MAG_TriangleCentreNormal))(bones, tri, centre, normal); }
	inline int32_t SharedRand() { return fn<int32_t (__cdecl *)()>(game_addr(game::Effect_SharedLcg_Rand))(); }

	template<typename T> inline T &AT(uint32_t a, int32_t off = 0) { return *(T *)(a + (uint32_t)off); }
	inline int16_t &S16(uint32_t a, int32_t off) { return AT<int16_t>(a, off); }
	inline uint16_t &U16(uint32_t a, int32_t off) { return AT<uint16_t>(a, off); }
	inline uint32_t &U32(uint32_t a, int32_t off) { return AT<uint32_t>(a, off); }
	inline uint8_t &U8(uint32_t a, int32_t off) { return AT<uint8_t>(a, off); }

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
		int16_t ntargets;
		int16_t mean;
		int16_t pad12;
		uint8_t *targets[3];
		uint32_t saved1C;
		uint32_t saved20;
		uint8_t orb[0x734];
		uint8_t ring[3][0x148];
	};
	struct BoltNode
	{
		TaskNode hdr;
		int16_t count;
		int16_t strike;
		uint32_t head;
		uint32_t tail;
		int16_t cur[3];
		int16_t t;
		int16_t dst[3];
		int16_t dt;
		int16_t dir[3];
		int16_t branches;
		int16_t bend[3];
		int16_t bend3;
	};
	struct PrimNode
	{
		TaskNode hdr;
		Mat4x3 m;
		uint8_t layout[0x110];
	};
	struct SparkNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t frame;
	};
	struct ArcNode
	{
		TaskNode hdr;
		uint8_t counter;
		uint8_t anchor;
		uint8_t amp_shift;
		uint8_t fade_shift;
		int16_t off[4];
		uint32_t tri;
		uint32_t model;
		int16_t bright[8][2];
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(MainNode) == 0xB34 && sizeof(BoltNode) == 0x38, "Mega Spark nodes");
	static_assert(sizeof(PrimNode) == 0x13C && sizeof(SparkNode) == 0x14 && sizeof(ArcNode) == 0x40, "Mega Spark nodes");
	static_assert(sizeof(PrimArg) == 0x28, "Mega Spark prim block");

	// The game reads a stack address here (the arc control point it pushed): the value it has in the game
	static inline uint32_t ArcResidue() { return 0x1A4EB4; }
	static const uint32_t RibbonResidue = 0x43300000;
	static uint16_t MainSrcPad() { return 0; }
	static uint16_t g_bolt_pt_pad = 0;
	static uint16_t BoltPtPad() { return g_bolt_pt_pad; }

	static void BonePoint(const int16_t *v, int32_t *out)
	{
		const uint8_t *bones = *(uint8_t *const *)*(uint8_t *const *)(Caster() + 0x64);
		const Mat4x3 *bm = (const Mat4x3 *)(bones + v[3] * 0x30 + 0x20);
		GteSetRotMatrixCtrl(bm);
		GteSetTransVectorCtrl(bm);
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		GteReadMAC123(out);
	}

	static int32_t Lcg()
	{
		Seed() = (Seed() * 125 + 0xE) & 0x7FFF;
		return (int32_t)Seed();
	}

	static uint32_t AllocPoint(const int16_t *src)
	{
		const uint32_t buf = TexBase();
		uint32_t p = U32(buf, BUF_PointCache);
		if (p)
			U32(buf, BUF_PointCache) = 0;
		else
		{
			p = buf + BUF_Points;
			int left = 0x180;
			while (S16(p, 6) != -1)
			{
				p += 0x28;
				if (--left == 0) return 0;
			}
		}
		U32(p, 0) = *(const uint32_t *)src;
		U32(p, 4) = *(const uint32_t *)(src + 2);
		S16(p, 0xE) = 2;
		S16(p, 0x20) = 0x200;
		S16(p, 0x22) = 0x300;
		U32(p, 0x24) = 0;
		return p;
	}

	static uint32_t SpawnBolt(const int16_t *dir, const int16_t *src, const int16_t *dst, const int16_t *bend, int16_t bend3,
		int16_t t, int16_t dt, int16_t strike, int16_t branches)
	{
		const uint32_t p = AllocPoint(src);
		if (!p) return 0;
		BoltNode *b = (BoltNode *)AddTaskToQueue(QBolts(), game_addr(ORIG_BoltTask));
		if (!b)
		{
			S16(p, 6) = -1;
			return 0;
		}
		*(uint32_t *)&b->cur[0] = *(const uint32_t *)src;
		*(uint32_t *)&b->cur[2] = *(const uint32_t *)(src + 2);
		*(uint32_t *)&b->dst[0] = *(const uint32_t *)dst;
		*(uint32_t *)&b->dst[2] = *(const uint32_t *)(dst + 2);
		*(uint32_t *)&b->dir[0] = *(const uint32_t *)dir;
		*(uint32_t *)&b->dir[2] = *(const uint32_t *)(dir + 2);
		b->t = t;
		b->dt = dt;
		b->strike = strike;
		b->branches = branches;
		b->tail = p;
		b->head = p;
		b->count = 1;
		if (!bend)
		{
			S16(p, 0xC) = 0;
			S16(p, 0xA) = 0;
			S16(p, 8) = 0;
			b->bend[0] = (int16_t)((Lcg() & 0xFF) - 0x80);
			b->bend[1] = (int16_t)((Lcg() & 0xFF) - 0x80);
			b->bend[2] = (int16_t)((Lcg() & 0xFF) - 0x80);
			b->bend3 = bend3;
			return p;
		}
		*(uint32_t *)&b->bend[0] = *(const uint32_t *)bend;
		*(uint32_t *)&b->bend[2] = *(const uint32_t *)(bend + 2);
		b->bend3 = bend3;
		return p;
	}
}
}

namespace ff8fx
{
namespace megaspark279
{
	static void __cdecl PrimCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PrimArg *arg = (const PrimArg *)arg_;
		if (((int32_t)r->scale[2] | *(const int32_t *)r->scale) == 0) return;
		if (r->a >= 0x1000 && *(const uint32_t *)r->rgb == 0) return;
		uint8_t *h = AllocHeader(0x58);
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
		if (r->flags & 0x40000) RotationFromAngles2(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		int32_t t[3];
		if (r->flags & 0x200)
		{
			t[0] = off[0];
			t[1] = off[1];
			t[2] = off[2];
		}
		else
		{
			GteSetRotMatrixCtrl(&arg->m);
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			MatrixMultiply(&arg->m, &m);
			t[0] = m.t[0];
			t[1] = m.t[1];
			t[2] = m.t[2];
		}
		m.t[0] = (int32_t)((uint32_t)t[0] + (uint32_t)arg->m.t[0]);
		m.t[1] = (int32_t)((uint32_t)t[1] + (uint32_t)arg->m.t[1]);
		m.t[2] = (int32_t)((uint32_t)t[2] + (uint32_t)arg->m.t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (r->flags & 0x100)
			{
				int16_t s[9] = { r->scale[0], 0, 0, 0, r->scale[1], 0, 0, 0, r->scale[2] };
				MulMatrix3(&m, s);
			}
			else
			{
				const int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
				Scale3DMatrix(&m, v);
			}
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
		*(int32_t *)(h + 0x18) = arg->depth;
		PacketCursor() = RenderPrim2(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void RibbonEdge(uint32_t p, uint32_t next, const int16_t *prev, int16_t *out, int32_t sxy, int32_t num, int32_t den)
	{
		if (S16(p, sxy + 4) < 0) return;
		int32_t ox, oy;
		if (next == 0)
		{
			oy = 0;
			ox = 0;
		}
		else
		{
			if (S16(next, sxy + 4) < 0) return;
			const int32_t dx = ScreenPx(S16(next, sxy)) - ScreenPx(S16(p, sxy));
			int32_t ax = dx < 0 ? -dx : dx;
			if (ax >= 0x80) ax = 0x7F;
			const int32_t dy = ScreenPx(S16(p, sxy + 2)) - ScreenPx(S16(next, sxy + 2));
			int32_t ay = dy < 0 ? -dy : dy;
			if (ay >= 0x80) ay = 0x7F;
			const int32_t dist = U8(game_addr(TBL_Distance), ay * 128 + ax);
			int32_t nx, ny;
			if (prev)
			{
				if (dist == 0)
				{
					out[0] = prev[0];
					out[1] = prev[1];
					nx = out[0];
					ny = out[1];
				}
				else
				{
					const int32_t ex = shl32(dy, 12) / dist;
					const int32_t ey = shl32(dx, 12) / dist;
					int32_t sx = ex + prev[0];
					int32_t sy = ey + prev[1];
					out[0] = (int16_t)ex;
					out[1] = (int16_t)ey;
					if (sx == 0 && sy == 0)
					{
						sx = ex + ex;
						sy = ey + ey;
					}
					const int32_t k = 0x4000 - ((mul32(sx, sx) + mul32(sy, sy)) >> 13);
					ny = mul32(k, sy) >> 13;
					nx = mul32(k, sx) >> 13;
				}
			}
			else
			{
				if (dist == 0)
				{
					S16(p, sxy + 4) = -1;
					return;
				}
				nx = shl32(dy, 12) / dist;
				ny = shl32(dx, 12) / dist;
				out[0] = (int16_t)nx;
				out[1] = (int16_t)ny;
			}
			const int32_t w = num / ((int32_t)S16(p, sxy + 4) + den);
			oy = ScreenEighths(mul32(w, ny) >> 12);
			ox = ScreenEighths(mul32(w, nx) >> 12);
		}
		const int16_t x = S16(p, sxy), y = S16(p, sxy + 2);
		S16(p, sxy) = (int16_t)(x - ox);
		S16(p, sxy + 8) = (int16_t)(x + ox);
		S16(p, sxy + 0xA) = (int16_t)(y + oy);
		S16(p, sxy + 2) = (int16_t)(y - oy);
	}
	static void BoltEdge(uint32_t p, uint32_t next, const int16_t *prev, int16_t *out) { RibbonEdge(p, next, prev, out, 0x10, 0x1F40, 0x200); }
	static void ArcEdge(uint32_t p, uint32_t next, const int16_t *prev, int16_t *out) { RibbonEdge(p, next, prev, out, 0, 0x2080, 0x400); }

	static void RibbonQuad(uint32_t pk, uint32_t a0, uint32_t a1, uint32_t b0, uint32_t b1, int32_t c0, int32_t c1, uint32_t bucket)
	{
		U32(pk, 8) = a0;
		U32(pk, 0x14) = a1;
		U32(pk, 0xC) = QUAD_UV0;
		U32(pk, 0x18) = QUAD_UV1;
		U16(pk, 0x24) = QUAD_UV2;
		U16(pk, 0x30) = QUAD_UV3;
		U32(pk, 0x20) = b0;
		U32(pk, 0x2C) = b1;
		const uint32_t col0 = (uint32_t)((((c0 << 8) | c0) << 8) | c0);
		U32(pk, 0x10) = col0;
		U32(pk, 4) = col0;
		const uint32_t col1 = (uint32_t)((((c1 << 8) | c1) << 8) | c1);
		U32(pk, 0) = 0xC000000;
		U8(pk, 7) = 0x3E;
		U32(pk, 0x28) = col1;
		U32(pk, 0x1C) = col1;
		InsertPrimAutoDepth(bucket, (void *)pk);
	}

	static void BoltRibbon(uint32_t head, int32_t count)
	{
		int16_t dir[2] = { (int16_t)(RibbonResidue & 0xFFFF), (int16_t)(RibbonResidue >> 16) };
		BoltEdge(head, U32(head, 0x24), nullptr, dir);
		uint32_t p = U32(head, 0x24);
		for (int32_t k = count - 2; k; k--)
		{
			BoltEdge(p, U32(p, 0x24), dir, dir);
			p = U32(p, 0x24);
		}
		BoltEdge(p, 0, dir, nullptr);
		uint32_t q = head;
		for (int32_t k = count - 1; k; k--)
		{
			const uint32_t nx = U32(q, 0x24);
			if (S16(q, 0x14) > 0 && S16(nx, 0x14) > 0)
			{
				const uint32_t pk = PacketCursor();
				PacketCursor() = pk + 0x34;
				const int32_t c0 = (int16_t)(S16(q, 0x20) >> 4), c1 = (int16_t)(S16(nx, 0x20) >> 4);
				RibbonQuad(pk, U32(q, 0x10), U32(q, 0x18), U32(nx, 0x10), U32(nx, 0x18), c0, c1,
					RenderOT44() + (uint32_t)(((int32_t)S16(nx, 0x14) + S16(q, 0x14)) >> 3) * 4);
			}
			q = nx;
		}
	}

	static void ArcDraw(uint32_t pts, uint32_t residue)
	{
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		uint32_t q = pts;
		for (int i = 0; i < 8; i++, q += 0x10)
		{
			GteLoadV0((void *)q);
			GteRTPS();
			GteReadDataReg(0xE, (void *)q);
			int32_t sz;
			GteReadDataRegTo(&sz, 0x13);
			S16(q, 4) = (int16_t)(sz >> 2);
		}
		int16_t dir[2] = { (int16_t)(residue & 0xFFFF), (int16_t)(residue >> 16) };
		ArcEdge(pts, pts + 0x10, nullptr, dir);
		for (int i = 1; i < 7; i++) ArcEdge(pts + 0x10 * i, pts + 0x10 * (i + 1), dir, dir);
		ArcEdge(pts + 0x70, 0, dir, nullptr);
		q = pts;
		for (int i = 0; i < 7; i++, q += 0x10)
		{
			if (S16(q, 4) <= 0 || S16(q, 0x14) <= 0) continue;
			const uint32_t pk = FrameCursor();
			FrameCursor() = pk + 0x34;
			RibbonQuad(pk, U32(q, 0), U32(q, 8), U32(q, 0x10), U32(q, 0x18), S16(q, 0xE), S16(q, 0x1E),
				RenderOT44() + (uint32_t)(((int32_t)S16(q, 0x14) + S16(q, 4)) >> 3) * 4);
		}
	}

	static void SparkDraw(const SparkNode *s)
	{
		TransformCameraByShadowRotation(s->pos, 0x800, -0x200);
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Spark);
		*(int16_t *)(h + 4) = s->frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *s = (SparkNode *)n;
		SparkDraw(s);
		s->frame++;
		return s->frame < 8 ? 0 : TASK_END;
	}

	static uint32_t __cdecl PrimTask(TaskNode *n)
	{
		PrimNode *p = (PrimNode *)n;
		PrimArg arg;
		ComposeAffineTransform(&Camera(), &p->m, &arg.m);
		arg.morph = TexBase() + BUF_Morph;
		arg.depth = -0x80;
		return prim::play((prim::Layout *)p->layout, PrimCallback, (int)&arg, 0) ? 0 : TASK_END;
	}

	static void BoltEnd(BoltNode *b)
	{
		PrimNode *p;
		int32_t scale = 0x1000;
		if (b->branches < 0)
		{
			if (b->dst[1] < 0)
			{
				SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
				if (!s) return;
				*(uint32_t *)&s->pos[0] = *(const uint32_t *)&b->dst[0];
				*(uint32_t *)&s->pos[2] = *(const uint32_t *)&b->dst[2];
				s->frame = 0;
				return;
			}
			p = (PrimNode *)AddTaskToQueue(QPrimB(), game_addr(ORIG_PrimTask));
			if (!p) return;
			DecodeModelPrimLayout(game_addr(MODEL_Hit), p->layout, 0x88);
			scale = (CrtRand() & 0xFFF) + 0x800;
		}
		else
		{
			if (b->strike == 0) return;
			p = (PrimNode *)AddTaskToQueue(QPrimA(), game_addr(ORIG_PrimTask));
			if (!p) return;
			DecodeModelPrimLayout(game_addr(MODEL_Strike), p->layout, 0x110);
		}
		const uint8_t *caster = Caster();
		int32_t v[3] = { (int32_t)b->dst[0] - *(const int16_t *)(caster + 0x1C), 0, (int32_t)b->dst[2] - *(const int16_t *)(caster + 0x20) };
		BuildOrthonormalBasis(v, &p->m);
		p->m.t[0] = b->dst[0];
		p->m.t[1] = b->dst[1];
		p->m.t[2] = b->dst[2];
		if ((int16_t)scale != 0x1000)
		{
			const int32_t s[3] = { (int16_t)scale, (int16_t)scale, (int16_t)scale };
			Scale3DMatrix(&p->m, s);
		}
	}

	static uint32_t __cdecl BoltTask(TaskNode *n)
	{
		BoltNode *b = (BoltNode *)n;
		for (int step = 8; step; step--)
		{
			if (b->t >= 0x1000) continue;
			b->cur[0] = (int16_t)(b->cur[0] + b->dir[0]);
			b->t = (int16_t)(b->dt + b->t);
			b->cur[1] = (int16_t)(b->cur[1] + b->dir[1]);
			b->cur[2] = (int16_t)(b->cur[2] + b->dir[2]);
			b->dir[0] = (int16_t)(b->dir[0] + ((Lcg() & 0x7F) - 0x40));
			b->dir[1] = (int16_t)(b->dir[1] + ((Lcg() & 0x7F) - 0x40));
			b->dir[2] = (int16_t)(b->dir[2] + ((Lcg() & 0x7F) - 0x40));
			if (Lcg() < 0x1000)
			{
				b->cur[0] = (int16_t)(b->cur[0] + ((Lcg() & 0x1FF) - 0x100));
				b->cur[1] = (int16_t)(b->cur[1] + ((Lcg() & 0x1FF) - 0x100));
				b->cur[2] = (int16_t)(b->cur[2] + ((Lcg() & 0x1FF) - 0x100));
			}
			int16_t pt[4];
			if (b->t >= 0x1000)
			{
				BoltEnd(b);
				*(uint32_t *)&pt[0] = *(const uint32_t *)&b->dst[0];
				*(uint32_t *)&pt[2] = *(const uint32_t *)&b->dst[2];
				b->t = 0x1000;
				g_bolt_pt_pad = (uint16_t)pt[3];
			}
			else
			{
				GteSetIR0(0x1000 - b->t);
				GteLoadIR123(b->cur);
				GteGPF();
				GteSetIR0(b->t);
				GteLoadIR123(b->dst);
				GteGPL();
				GteStoreIR123(pt);
				pt[3] = (int16_t)BoltPtPad();
			}
			const uint32_t p = AllocPoint(pt);
			if (!p)
			{
				b->t = 0x1000;
				continue;
			}
			U32(b->tail, 0x24) = p;
			b->tail = p;
			GteSetIR0(AT<int16_t>(game_addr(TBL_Sine), ((b->t >> 1) & 0xFFF) * 2));
			GteLoadIR123(b->bend);
			GteGPF();
			b->count++;
			GteStoreIR123((void *)(p + 8));
			if (b->t >= 0xC00) continue;
			if (Lcg() >= shl32(b->branches, 13)) continue;
			int16_t nd[4], nv[4];
			nd[0] = (int16_t)((Lcg() & 0xFFF) + b->dst[0] - 0x800);
			nd[1] = (int16_t)((Lcg() & 0x7FF) + b->dst[1] - 0x400);
			nd[2] = (int16_t)((Lcg() & 0x1FF) + b->dst[2] + 0x100);
			nv[0] = (int16_t)((Lcg() & 0x1FF) + b->dir[0] - 0x100);
			nv[1] = (int16_t)((Lcg() & 0x1FF) + b->dir[1] - 0x100);
			nv[2] = (int16_t)((Lcg() & 0x1FF) + b->dir[2] - 0x100);
			nd[3] = 0;
			nv[3] = 0;
			const uint32_t q = SpawnBolt(nv, pt, nd, b->bend, -1, b->t, 0x200, 0, -1);
			if (q)
			{
				U32(q, 8) = U32(p, 8);
				U32(q, 0xC) = U32(p, 0xC);
			}
			b->branches--;
		}
		uint32_t p = b->head;
		do
		{
			if (S16(p, 0x20) > 0)
			{
				S16(p, 0x20) = (int16_t)(S16(p, 0x20) + S16(p, 0x22));
				if (S16(p, 0x20) <= 0) S16(p, 0x20) = 0;
				else if (S16(p, 0xE) != 0 && S16(p, 0x22) <= 0) S16(p, 0xE) = (int16_t)(S16(p, 0xE) - 1);
				else if (S16(p, 0x22) > -0x180) S16(p, 0x22) = (int16_t)(S16(p, 0x22) - 0x140);
			}
			p = U32(p, 0x24);
		} while (p);
		p = b->head;
		do
		{
			GteLoadV0((void *)p);
			GteRTPS();
			GteReadSXY2((void *)(p + 0x10));
			GteReadOTZ((void *)(p + 0x14));
			p = U32(p, 0x24);
		} while (p);
		if (b->count >= 2) BoltRibbon(b->head, b->count);
		p = b->head;
		do
		{
			S16(p, 0) = (int16_t)(S16(p, 0) + S16(p, 8));
			S16(p, 2) = (int16_t)(S16(p, 2) + S16(p, 0xA));
			S16(p, 4) = (int16_t)(S16(p, 4) + S16(p, 0xC));
			p = U32(p, 0x24);
		} while (p);
		p = b->head;
		if (S16(p, 0x20) != 0) return 0;
		for (;;)
		{
			const uint32_t nx = U32(p, 0x24);
			if (S16(nx, 0x20) != 0) return 0;
			U32(TexBase(), BUF_PointCache) = p;
			S16(p, 6) = -1;
			b->count--;
			b->head = nx;
			if (b->count < 2) return TASK_END;
			p = nx;
			if (S16(p, 0x20) != 0) return 0;
		}
	}

	static void SpawnArc(uint32_t model, uint32_t from, uint32_t to, uint8_t anchor, uint8_t amp_shift, uint8_t fade_shift)
	{
		ArcNode *a = (ArcNode *)AddTaskToQueue(QArcs(), game_addr(ORIG_ArcTask));
		if (!a) return;
		a->amp_shift = amp_shift;
		a->fade_shift = fade_shift;
		a->anchor = anchor;
		a->counter = 0;
		*(uint32_t *)&a->off[0] = U32(to, 0);
		*(uint32_t *)&a->off[2] = U32(to, 4);
		a->tri = from;
		a->model = model;
		for (int k = 0; k < 8; k++)
		{
			a->bright[k][0] = 0x80;
			a->bright[k][1] = 0;
		}
	}

	static uint32_t __cdecl ArcTask(TaskNode *n)
	{
		ArcNode *a = (ArcNode *)n;
		const uint32_t bones = U32(a->model, 0) + 0x10;
		int16_t c[4], ctrl[4], p2[4];
		c[3] = 0;
		p2[3] = 0;
		int32_t amp;
		if (a->anchor == 0)
		{
			TriangleCentreNormal(bones, a->tri, c, ctrl);
			const Mat4x3 *bm = (const Mat4x3 *)(bones + a->off[3] * 0x30 + 0x10);
			GteSetRotMatrixCtrl(bm);
			GteSetTransVectorCtrl(bm);
			GteLoadV0(a->off);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(p2);
			int32_t d[3];
			d[0] = (int32_t)p2[0] - c[0];
			d[1] = (int32_t)p2[1] - c[1];
			d[2] = (int32_t)p2[2] - c[2];
			GteLoadIR123_32b(d);
			GteSQR();
			GteReadMAC123(d);
			int32_t sum = d[2] + d[1] + d[0];
			GteSetLZCS(sum);
			int32_t lz;
			GteReadLZCR(&lz);
			lz &= ~1;
			const int32_t shift = (0x1F - lz) >> 1;
			const int32_t norm = lz - 0x18;
			if (norm > 0) sum = shl32(sum, norm);
			else sum >>= (0x18 - lz);
			const int32_t len = (int32_t)((uint32_t)AT<uint16_t>(game_addr(TBL_Lengths), sum * 2) << shift);
			amp = len >> 12;
			GteSetIR0(len >> a->amp_shift);
			GteLoadIR123(ctrl);
			GteGPF();
			GteStoreIR123(ctrl);
			ctrl[0] = (int16_t)(ctrl[0] + c[0]);
			ctrl[1] = (int16_t)(ctrl[1] + c[1]);
			ctrl[2] = (int16_t)(ctrl[2] + c[2]);
		}
		else
		{
			const Mat4x3 *am = (const Mat4x3 *)(bones + S16(a->tri, 6) * 0x30 + 0x10);
			GteSetRotMatrixCtrl(am);
			GteSetTransVectorCtrl(am);
			GteLoadV0((const void *)a->tri);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(c);
			*(uint32_t *)&ctrl[0] = *(const uint32_t *)&c[0];
			*(uint32_t *)&ctrl[2] = *(const uint32_t *)&c[2];
			const Mat4x3 *bm = (const Mat4x3 *)(bones + a->off[3] * 0x30 + 0x10);
			GteSetRotMatrixCtrl(bm);
			GteSetTransVectorCtrl(bm);
			GteLoadV0(a->off);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(p2);
			amp = 0x800;
		}
		const uint32_t b = (uint32_t)FieldAlloc(0x80);
		U32(b, 0) = *(const uint32_t *)&c[0];
		U32(b, 4) = *(const uint32_t *)&c[2];
		S16(b, 0xE) = a->bright[0][0];
		U32(b, 0x70) = *(const uint32_t *)&p2[0];
		U32(b, 0x74) = *(const uint32_t *)&p2[2];
		S16(b, 0x7E) = a->bright[7][0];
		for (int32_t k = 1; k < 7; k++)
		{
			const int32_t t = shl32(k, 12) / 7;
			const int32_t u = 0x1000 - t;
			int16_t q[4];
			GteSetIR0(u);
			GteLoadIR123(c);
			GteGPF();
			GteSetIR0(t);
			GteLoadIR123(ctrl);
			GteGPL();
			GteStoreIR123(q);
			GteSetIR0(u);
			GteLoadIR123(q);
			GteGPF();
			GteSetIR0(t);
			GteLoadIR123(p2);
			GteGPL();
			GteStoreIR123((void *)(b + 0x10 * k));
			S16(b, 0x10 * k + 0xE) = a->bright[k][0];
		}
		for (int32_t k = 1; k < 7; k++)
		{
			const uint32_t q = b + 0x10 * k;
			const int32_t e = mul32(S16(TexBase(), BUF_Profile + 2 * k), amp) >> 12;
			S16(q, 0) = (int16_t)(S16(q, 0) + (mul32(SharedRand() - 0x4000, e) >> 18));
			S16(q, 2) = (int16_t)(S16(q, 2) + (mul32(SharedRand() - 0x4000, e) >> 18));
			S16(q, 4) = (int16_t)(S16(q, 4) + (mul32(SharedRand() - 0x4000, e) >> 18));
		}
		ArcDraw(b, ArcResidue());
		FieldFree(0x80);
		if (a->counter >= 0xA)
		{
			int32_t lit = 0;
			for (int k = 0; k < 8; k++)
			{
				const int32_t dec = (0x1400 - S16(TexBase(), BUF_Profile + 2 * k)) >> a->fade_shift;
				a->bright[k][0] = (int16_t)(a->bright[k][0] - dec);
				if (a->bright[k][0] < 0) a->bright[k][0] = 0;
				else lit++;
			}
			if (lit <= 1) return TASK_END;
		}
		a->counter++;
		return 0;
	}

	static void ArcSet(uint32_t from_or_table, uint32_t to, uint8_t anchor, uint8_t amp_shift)
	{
		SpawnArc(U32((uint32_t)Caster(), 0x64), from_or_table, to, anchor, amp_shift, 7);
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		*(Caster() + 1) |= 0x10;
		*(int16_t *)(Caster() + 0xC) = 0x80;
		PrimArg arg;
		arg.depth = 0;
		arg.morph = 0;
		int32_t c = m->counter;
		if ((uint32_t)c < 0x20)
		{
			const int16_t saved = (int16_t)m->saved20;
			const uint32_t d = ((uint32_t)((int32_t)saved - (int32_t)m->mean + 0x1388) * (uint32_t)c) >> 5;
			*(int16_t *)(Caster() + 0x20) = (int16_t)(saved - (int16_t)d);
			*(int16_t *)(Caster() + 0xC) = (int16_t)(((uint32_t)c << 7) >> 5);
			if ((uint32_t)c >= 0xF && (uint32_t)c < 0x1F)
			{
				if (c == 0xF)
					for (int i = 2; i >= 0; i--) DecodeModelPrimLayout(game_addr(MODEL_Ring), m->ring[i], 0x148);
				arg.depth = -0x80;
				arg.morph = TexBase() + BUF_Morph;
				for (int i = 2; i >= 0; i--)
				{
					BonePoint((const int16_t *)(game_addr(ANCHOR_0) + 8 * i), arg.m.t);
					RotX(*(const int16_t *)(Caster() + 0xC), &arg.m);
					ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
					prim::play((prim::Layout *)m->ring[i], PrimCallback, (int)&arg, 0);
				}
			}
			if ((c & 3) == 0)
			{
				const uint32_t row = (((uint32_t)c >> 2) & 7) * 0x18;
				ArcSet(game_addr(TRI_Rows) + row, game_addr(ANCHOR_0), 0, 0xC);
				ArcSet(game_addr(TRI_Rows) + 0xC0 + row, game_addr(ANCHOR_0) + 8, 0, 0xC);
				ArcSet(game_addr(TRI_Rows) + 0x180 + row, game_addr(ANCHOR_0) + 0x10, 0, 0xC);
			}
		}
		c = m->counter - 0x14;
		if ((uint32_t)c < 0x28)
		{
			if ((uint32_t)c >= 4)
			{
				if (c == 4) DecodeModelPrimLayout(game_addr(MODEL_Orb), m->orb, 0x734);
				BonePoint((const int16_t *)game_addr(POINT_Orb), arg.m.t);
				RotY(0, &arg.m);
				ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
				arg.morph = TexBase() + BUF_Morph;
				arg.depth = -0x80;
				prim::play((prim::Layout *)m->orb, PrimCallback, (int)&arg, 0);
			}
			if ((c & 7) == 0)
			{
				ArcSet(game_addr(ANCHOR_0), game_addr(POINT_Orb), 1, 0xE);
				ArcSet(game_addr(ANCHOR_0) + 8, game_addr(POINT_Orb), 1, 0xE);
				ArcSet(game_addr(ANCHOR_0) + 0x10, game_addr(POINT_Orb), 1, 0xE);
			}
		}
		c = m->counter - 0x28;
		if ((uint32_t)c < 0xF)
		{
			uint8_t *tgt = m->targets[(uint32_t)c % (uint32_t)(int32_t)m->ntargets];
			if (c == 0) ScreenFadeTask(0, 1, 2, 0x80);
			int32_t tmp[3];
			BonePoint((const int16_t *)game_addr(POINT_Orb), tmp);
			int16_t src[4] = { (int16_t)tmp[0], (int16_t)tmp[1], (int16_t)tmp[2], (int16_t)MainSrcPad() };
			int16_t dst[4] = { 0, 0, 0, 0 };
			int16_t strike;
			if ((uint32_t)c >= 4 && (uint32_t)c <= (uint32_t)((int32_t)m->ntargets + 4))
			{
				strike = 1;
				GetDefaultEffectPosition(tgt, dst);
			}
			else
			{
				strike = 0;
				const int32_t r = CrtRand();
				GetEffectSpawnPosition(tgt, mul32(r, ***(const uint8_t *const *const *)(tgt + 0x64)) >> 15, 0, dst);
			}
			int16_t vel[4];
			vel[0] = (int16_t)((CrtRand() & 0xFF) - 0x80);
			vel[1] = (int16_t)(-0x40 - (CrtRand() & 0x7F));
			vel[2] = (int16_t)((CrtRand() & 0x3F) + 0x40);
			vel[3] = 0;
			SpawnBolt(vel, src, dst, nullptr, 0, 0, 0x200, strike, 2);
		}
		if (m->counter == 0) BdPlaySE(SOUND_Spark(), 0, 0x80);
		if (m->counter == 0x30) ScreenFadeTask(0, 1, 0, 0x80);
		else if (m->counter == 0x32) ScreenFadeTask(0, 1, 0, 0x40);
		else if (m->counter == 0x3A) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		const int32_t e = 0x55 - m->counter;
		if ((uint32_t)e < 0x10)
		{
			const int16_t saved = (int16_t)m->saved20;
			const uint32_t d = ((uint32_t)((int32_t)saved - (int32_t)m->mean + 0x1388) * (uint32_t)e) >> 4;
			*(int16_t *)(Caster() + 0x20) = (int16_t)(saved - (int16_t)d);
			*(int16_t *)(Caster() + 0xC) = (int16_t)(((uint32_t)e << 7) >> 4);
		}
		m->counter++;
		if (m->counter < 0x55) return 0;
		*(uint16_t *)Caster() &= 0xEFFF;
		*(int16_t *)(Caster() + 0xC) = 0;
		*(int16_t *)(Caster() + 0x20) = (int16_t)m->saved20;
		return TASK_END;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		g_bolt_pt_pad = 0;
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
			const uint32_t buf = TexBase();
			InitTaskQueuePool(QMain(), (void *)(buf + POOL_Main), 0xB34, 1);
			InitTaskQueuePool(QArcs(), (void *)(TexBase() + POOL_Arcs), 0x40, 0x40);
			InitTaskQueuePool(QBolts(), (void *)(TexBase() + POOL_Bolts), 0x38, 0x30);
			InitTaskQueuePool(QPrimA(), (void *)(TexBase() + POOL_PrimA), 0x13C, 3);
			InitTaskQueuePool(QPrimB(), (void *)(TexBase() + POOL_PrimB), 0x13C, 0x10);
			InitTaskQueuePool(QSparks(), (void *)TexBase(), 0x14, 0x10);
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&m->counter, 0, 0x2CA);
			for (int32_t k = 0; k < 8; k++)
				S16(TexBase(), BUF_Profile + 2 * k) = (int16_t)ComputeSin(shl32(k, 11) / 7);
			for (uint32_t p = TexBase() + BUF_Points + 6, i = 0; i < 0x180; i++, p += 0x28) S16(p, 0) = -1;
			U32(TexBase(), BUF_PointCache) = 0;
			const ActionData *act = Ctx()->actions;
			m->ntargets = act->target_count;
			m->mean = 0;
			for (int32_t i = 0; i < m->ntargets; i++)
			{
				uint8_t *t = Entity(Ctx()->actions->targets[i * TARGET_STRIDE]);
				m->targets[i] = t;
				m->mean = (int16_t)(m->mean + *(const int16_t *)(t + 0x20));
			}
			m->mean = (int16_t)((int32_t)m->mean / (int32_t)m->ntargets);
			m->saved1C = *(const uint32_t *)(Caster() + 0x1C);
			m->saved20 = *(const uint32_t *)(Caster() + 0x20);
		}
		int a0, a1;
		if (mn->started)
		{
			ExecuteTaskQueue(QBolts());
			uint8_t *caster = Caster();
			ComputeBonesWorldMatrices(caster + 0x60, caster + 0x40);
			a0 = ExecuteTaskQueue(QMain());
			a1 = ExecuteTaskQueue(QArcs());
			BuildBoneMatricesFromPose(Caster() + 0x60);
			ExecuteTaskQueue(QPrimA());
			ExecuteTaskQueue(QPrimB());
			ExecuteTaskQueue(QSparks());
		}
		else a0 = a1 = (int)n;
		if (mn->started && !a0 && !a1) return TASK_END;
		mn->counter++;
		return 0;
	}
}

	void register_mag279_mega_spark()
	{
		register_port(megaspark279::ORIG_MasterTask, (void *)megaspark279::MasterTask, "279 MasterTask", 279);
		register_port(megaspark279::ORIG_MainTask, (void *)megaspark279::MainTask, "279 MainTask", 279);
		register_port(megaspark279::ORIG_BoltTask, (void *)megaspark279::BoltTask, "279 BoltTask", 279);
		register_port(megaspark279::ORIG_ArcTask, (void *)megaspark279::ArcTask, "279 ArcTask", 279);
		register_port(megaspark279::ORIG_PrimTask, (void *)megaspark279::PrimTask, "279 PrimTask", 279);
		register_port(megaspark279::ORIG_SparkTask, (void *)megaspark279::SparkTask, "279 SparkTask", 279);
	}
}

