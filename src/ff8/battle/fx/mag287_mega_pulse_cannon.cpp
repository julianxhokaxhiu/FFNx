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

// Effect 287: Mega Pulse Cannon

#include "mag_common.h"

namespace ff8fx
{
namespace megapulse287
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_287_MegaPulseCannon_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_287_MegaPulseCannon_CasterEntity)); }
	inline TaskQueue *QSparks() { return (TaskQueue *)game_addr(game::MAG_287_MegaPulseCannon_SparksQueue); }
	inline TaskQueue *QRocks() { return (TaskQueue *)game_addr(game::MAG_287_MegaPulseCannon_RocksQueue); }
	inline TaskQueue *QSpots() { return (TaskQueue *)game_addr(game::MAG_287_MegaPulseCannon_SpotsQueue); }
	inline TaskQueue *QArcs() { return (TaskQueue *)game_addr(game::MAG_287_MegaPulseCannon_ArcsQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_287_MegaPulseCannon_MainQueue); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_287_MegaPulseCannon_PacketCursor)); }
	inline uint32_t &Flash() { return var<uint32_t>(game_addr(game::MAG_287_MegaPulseCannon_Flash)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_287_MegaPulseCannon_MagicBufferBase)); }
	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_287_MegaPulseCannon_MasterTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_287_MegaPulseCannon_MainTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_287_MegaPulseCannon_SparkTask;
	static constexpr game::Id ORIG_RockTask = game::MAG_287_MegaPulseCannon_RockTask;
	static constexpr game::Id ORIG_SpotTask = game::MAG_287_MegaPulseCannon_SpotTask;
	static constexpr game::Id ORIG_ArcTask = game::MAG_287_MegaPulseCannon_ArcTask;
	static constexpr game::Id ORIG_PrimCallback = game::MAG_287_MegaPulseCannon_PrimCallback;
	static const uint32_t POOL_Rocks = 0x900, POOL_Spots = 0xB00, POOL_Main = 0x1070, POOL_Arcs = 0x21FF4;
	static const uint32_t BUF_Morph = 0x1A18;
	static const uint32_t BUF_Profile = 0x1F98, BUF_ProfileEnd = 0x1FA8;
	static const uint32_t BUF_Stream = 0x1FA8;
	static const uint32_t BUF_StreamDst = 0x21FF0;
	static const uint32_t ARENA_A = 0x1FF0, ARENA_B = 0x11FF0;
	static constexpr game::Id MODEL_Cannon = game::MAG_287_MegaPulseCannon_ModelCannon;
	static constexpr game::Id MODEL_Beam = game::MAG_287_MegaPulseCannon_ModelBeam;
	static constexpr game::Id MODEL_Spot = game::MAG_287_MegaPulseCannon_ModelSpot;
	static constexpr game::Id MODEL_Rock = game::MAG_287_MegaPulseCannon_ModelRock;
	static constexpr game::Id SEQ_Charge = game::MAG_287_MegaPulseCannon_SeqCharge;
	static constexpr game::Id SEQ_Spark = game::MAG_287_MegaPulseCannon_SeqSpark;
	static constexpr game::Id POINT_Muzzle = game::unk_13FAEA0;
	static constexpr game::Id TBL_SpotTargets = game::MAG_287_MegaPulseCannon_TableSpotTargets;
	static constexpr game::Id TRI_Rows = game::unk_13FAEB0;
	static constexpr game::Id TBL_Distance = game::MAG_287_MegaPulseCannon_TableDistance;
	static constexpr game::Id TBL_Lengths = game::MAG_287_MegaPulseCannon_TableLengths;
	static const int32_t STREAM_File = 0x168;
	static const void *SOUND_Cannon() { return (const void *)game_addr(game::MAG_287_MegaPulseCannon_SoundCannon); }
	static const uint32_t QUAD_UV0 = 0x3E14C048, QUAD_UV1 = 0xB9C078;
	static const uint16_t QUAD_UV2 = 0xC048, QUAD_UV3 = 0xC078;

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MulMatrix3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void MatrixMultiplyVector(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrimSet(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_063_RenderPrimSet))(h, ot, mode, cursor); }
	inline void RotY(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline void TriangleCentreNormal(uint32_t bones, uint32_t tri, int16_t *centre, int16_t *normal) { fn<void (__cdecl *)(uint32_t, uint32_t, int16_t *, int16_t *)>(game_addr(game::MAG_TriangleCentreNormal))(bones, tri, centre, normal); }
	inline void TransformCoordinateToCameraSpace(const void *v, int32_t *out, void *flag) { fn<void (__cdecl *)(const void *, int32_t *, void *)>(game_addr(game::TransformCoordinateToCameraSpace))(v, out, flag); }
	inline int32_t SharedRand() { return fn<int32_t (__cdecl *)()>(game_addr(game::Effect_SharedLcg_Rand))(); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline void StreamQueueLoad(int32_t id, uint32_t dst, int32_t kind) { fn<void (__cdecl *)(int32_t, uint32_t, int32_t)>(game_addr(game::Stream_QueueLoad))(id, dst, kind); }
	inline void StreamPump() { fn<void (__cdecl *)()>(game_addr(game::Stream_Pump))(); }
	inline int32_t StreamPreload(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::BattleFile_PreLoad_IsLoaded))(a); }

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
		int32_t point[4];
		uint8_t *targets[3];
		uint8_t cannon[0x8C4];
		uint8_t beam[0xB8];
	};
	struct SparkNode
	{
		TaskNode hdr;
		uint32_t seq;
		int32_t shadow;
		int16_t pos[3];
		int16_t frame;
		int16_t vel[3];
		int16_t life;
	};
	struct RockNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[3];
		int16_t vy;
		int16_t angles[3];
		int16_t spin;
	};
	struct SpotNode
	{
		TaskNode hdr;
		int16_t pos[4];
		uint8_t layout[0x1BC];
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
		int16_t depth;
		uint8_t second;
		uint8_t scroll;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(MainNode) == 0x9A8 && sizeof(SparkNode) == 0x24, "Mega Pulse Cannon nodes");
	static_assert(sizeof(RockNode) == 0x20 && sizeof(SpotNode) == 0x1D0 && sizeof(ArcNode) == 0x40, "Mega Pulse Cannon nodes");
	static_assert(sizeof(PrimArg) == 0x28, "Mega Pulse Cannon prim block");

	// The game reads a stack address here (the arc control point it pushed): the value it has in the game
	static inline uint32_t ArcResidue() { return 0x1A4EBC; }
	static int32_t MainPointPad() { return 0; }

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

	static void BoneTipPoint(const uint8_t *ent, int32_t idx, int16_t *out)
	{
		const uint32_t blk = (uint32_t)FieldAlloc(0x3C);
		U32(blk, 0x34) = 0;
		const uint32_t skel = **(const uint32_t *const *)(ent + 0x64);
		const uint32_t bones = skel + 0x10;
		if (idx >= 0xF0) idx = U8(skel, idx - 0xEC);
		const uint32_t bone = bones + (uint32_t)idx * 0x30;
		const int16_t parent = S16(bone, 0);
		if (parent >= 0) S16(blk, 0x38) = S16(bones + (uint32_t)(int32_t)parent * 0x30, 2);
		else S16(blk, 0x38) = 0;
		GteSetRotMatrix((const Mat4x3 *)(bone + 0x10));
		GteSetTransVector((const Mat4x3 *)(bone + 0x10));
		TransformCoordinateToCameraSpace((const void *)(blk + 0x34), (int32_t *)(blk + 0x20), (void *)(blk + 0x30));
		out[0] = S16(blk, 0x20);
		out[1] = S16(blk, 0x24);
		out[2] = S16(blk, 0x28);
		FieldFree(0x3C);
	}
}
}

namespace ff8fx
{
namespace megapulse287
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
		if (r->flags & 0x800) MatrixMultiplyVector(&m, off, off);
		if (r->flags & 0x200)
		{
			m.t[0] = off[0];
			m.t[1] = off[1];
			m.t[2] = off[2];
		}
		else
		{
			GteSetRotMatrixCtrl(&arg->m);
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			if (!(r->flags & 0x8000)) MatrixMultiply(&arg->m, &m);
		}
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)arg->m.t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)arg->m.t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)arg->m.t[2]);
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
		*(uint32_t *)(h + 0x1C) = (r->flags & 0x4000) ? 0x2000 : 0x2030;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint8_t *)(h + 0x1C) |= 0xC0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		if (arg->scroll != 0 && (int16_t)r->flags_lo <= 1)
		{
			*(uint16_t *)(h + 0x26) = 0;
			*(uint16_t *)(h + 0x24) = 0;
			*(uint16_t *)(h + 0x2A) = 0x100;
			*(uint16_t *)(h + 0x28) = 0x100;
			*(uint16_t *)(h + 0x30) = 0x100;
			*(uint16_t *)(h + 0x2C) = 0x100;
			if ((int16_t)r->flags_lo == 0 && arg->second == 0)
			{
				*(uint16_t *)(h + 0x2E) = 0;
				*(uint16_t *)(h + 0x32) = 0x80;
			}
			else
			{
				*(uint16_t *)(h + 0x2E) = 0x80;
				*(uint16_t *)(h + 0x32) = 0x80;
			}
			*(uint16_t *)(h + 0x22) = arg->scroll;
		}
		else *(uint16_t *)(h + 0x22) = 0;
		*(int32_t *)(h + 0x18) = arg->depth;
		*(uint16_t *)(h + 0x20) = 0;
		PacketCursor() = RenderPrimSet(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x6C);
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
			const int32_t dx = (int32_t)S16(next, sxy) - S16(p, sxy);
			int32_t ax = dx < 0 ? -dx : dx;
			if (ax >= 0x80) ax = 0x7F;
			const int32_t dy = (int32_t)S16(p, sxy + 2) - S16(next, sxy + 2);
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
			oy = mul32(w, ny) >> 12;
			ox = mul32(w, nx) >> 12;
		}
		const int16_t x = S16(p, sxy), y = S16(p, sxy + 2);
		S16(p, sxy) = (int16_t)(x - ox);
		S16(p, sxy + 8) = (int16_t)(x + ox);
		S16(p, sxy + 0xA) = (int16_t)(y + oy);
		S16(p, sxy + 2) = (int16_t)(y - oy);
	}
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
		TransformCameraByShadowRotation(s->pos, (int16_t)s->shadow, -(s->shadow >> 2));
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = s->seq;
		*(int16_t *)(h + 4) = s->frame;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl SparkTask(TaskNode *n)
	{
		SparkNode *s = (SparkNode *)n;
		SparkDraw(s);
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
		s->frame++;
		return s->frame < s->life ? 0 : TASK_END;
	}

	static void RockDraw(const RockNode *k)
	{
		uint8_t *h = AllocHeader(0x58);
		Mat4x3 m;
		BuildRotationMatrixFromAngles(k->angles, &m);
		m.t[0] = k->pos[0];
		m.t[1] = k->pos[1];
		m.t[2] = k->pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)h = game_addr(MODEL_Rock);
		*(uint32_t *)(h + 0x1C) = 0;
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl RockTask(TaskNode *n)
	{
		RockNode *k = (RockNode *)n;
		RockDraw(k);
		k->vy = (int16_t)(k->vy + 0xA);
		k->angles[0] = (int16_t)(k->angles[0] + k->spin);
		k->pos[2] = (int16_t)(k->pos[2] + 0x190);
		k->pos[1] = (int16_t)(k->pos[1] + k->vy);
		if (k->pos[1] > 0) k->vy = (int16_t)-k->vy;
		k->counter++;
		return k->counter < 0xA ? 0 : TASK_END;
	}

	static uint32_t __cdecl SpotTask(TaskNode *n)
	{
		SpotNode *p = (SpotNode *)n;
		PrimArg arg;
		arg.second = 0;
		RotY(0, &arg.m);
		arg.m.t[0] = p->pos[0];
		arg.m.t[1] = p->pos[1];
		arg.m.t[2] = p->pos[2];
		ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
		arg.morph = TexBase() + BUF_Morph;
		arg.depth = -0x80;
		arg.scroll = 0;
		return prim::play((prim::Layout *)p->layout, PrimCallback, (int)&arg, 0) ? 0 : TASK_END;
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

	template<typename R> static uint32_t ArcPoints(const ArcNode *a, R rnd)
	{
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
			S16(q, 0) = (int16_t)(S16(q, 0) + (mul32(rnd() - 0x4000, e) >> 18));
			S16(q, 2) = (int16_t)(S16(q, 2) + (mul32(rnd() - 0x4000, e) >> 18));
			S16(q, 4) = (int16_t)(S16(q, 4) + (mul32(rnd() - 0x4000, e) >> 18));
		}
		return b;
	}

	static uint32_t __cdecl ArcTask(TaskNode *n)
	{
		ArcNode *a = (ArcNode *)n;
		const uint32_t b = ArcPoints(a, SharedRand);
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

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		*(uint16_t *)Caster() &= 0xFFF7;
		const int32_t c = m->counter;
		if ((uint32_t)c < 0x4B)
		{
			int32_t pt[4];
			if ((uint32_t)c < 0x24)
			{
				BonePoint((const int16_t *)game_addr(POINT_Muzzle), pt);
				pt[3] = MainPointPad();
				for (int k = 0; k < 4; k++) m->point[k] = pt[k];
			}
			else
				for (int k = 0; k < 4; k++) pt[k] = m->point[k];
			if ((uint32_t)c < 0x1E)
			{
				SpawnArc(U32((uint32_t)Caster(), 0x64), game_addr(TRI_Rows) + ((uint32_t)c % 0xD) * 0x18, game_addr(POINT_Muzzle), 0, 0xC, 7);
				*Caster() |= 8;
			}
			else if ((uint32_t)c < 0x3C)
			{
				if ((uint32_t)c >= 0x20 && (uint32_t)c < 0x26)
					for (int32_t i = 5; i >= 0; i--)
					{
						SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
						if (!s) continue;
						s->seq = game_addr(SEQ_Spark);
						BoneTipPoint(Caster(), i + 0x16, s->pos);
						s->vel[2] = 0;
						s->vel[0] = 0;
						s->vel[1] = (int16_t)(-0x14 - (CrtRand() & 0xF));
						s->life = 0xC;
						s->frame = 0;
						s->shadow = (CrtRand() & 0x3FF) + 0x1000;
					}
				if (c == 0x1E)
				{
					U32((uint32_t)QArcs(), 4) = 0;
					U32((uint32_t)QArcs(), 0) = 0;
				}
				if ((c & 1) == 0)
				{
					RockNode *k = (RockNode *)AddTaskToQueue(QRocks(), game_addr(ORIG_RockTask));
					if (k)
					{
						const int32_t a = CrtRand() & 0x7FF;
						k->pos[0] = (int16_t)((ComputeCos(a) >> 1) + *(const int16_t *)(Caster() + 0x1C));
						const int32_t s = ComputeSin(a);
						k->pos[1] = 0;
						k->pos[2] = (int16_t)(*(const int16_t *)(Caster() + 0x20) - (int16_t)(s >> 1) + 0x1388);
						k->vy = (int16_t)(-0x20 - (CrtRand() & 0x1F));
						k->angles[0] = (int16_t)CrtRand();
						k->angles[1] = (int16_t)CrtRand();
						k->angles[2] = (int16_t)CrtRand();
						k->spin = (int16_t)((CrtRand() & 0x1FF) + 0x200);
					}
				}
			}
			if ((uint32_t)c >= 0x21)
			{
				if ((uint32_t)(c - 0x21) < 8) Flash() = (uint32_t)(c - 0x21) << 8;
				else if ((uint32_t)(c - 0x3B) < 0x10) Flash() = (uint32_t)(0x10 - (c - 0x3B)) << 7;
				else Flash() = 0x800;
			}
			Mat4x3 M;
			M.t[0] = pt[0];
			M.t[1] = pt[1];
			M.t[2] = pt[2];
			RotY(*(const int16_t *)(Caster() + 0xE), &M);
			if ((uint32_t)c < 8)
			{
				GteSetRotMatrix(&M);
				SparkNode *s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
				int32_t base = shl32(c + 0x20, 7);
				for (int32_t left = 4; s; )
				{
					s->seq = game_addr(SEQ_Charge);
					const int32_t a = (CrtRand() & 0x3F) + base - 0x20;
					const int32_t r = (CrtRand() & 0x7FF) + 0x400;
					s->pos[0] = (int16_t)(mul32(ComputeSin(a), r) >> 12);
					s->pos[1] = (int16_t)(mul32(ComputeCos(a), r) >> 12);
					s->pos[2] = (int16_t)((CrtRand() & 0xFF) - 0x80);
					GteLoadV0(s->pos);
					GteMVMVA_RotV0();
					GteStoreIR123(s->pos);
					const int16_t x = s->pos[0], y = s->pos[1], z = s->pos[2];
					s->life = 0xE;
					s->vel[0] = (int16_t)((int32_t)x / -14);
					s->vel[1] = (int16_t)((int32_t)y / -14);
					s->vel[2] = (int16_t)((int32_t)z / -14);
					s->pos[0] = (int16_t)(x + M.t[0]);
					s->pos[1] = (int16_t)(y + M.t[1]);
					s->pos[2] = (int16_t)(z + M.t[2]);
					s->frame = 0;
					s->shadow = 0x7D0;
					base -= 0x400;
					if (--left == 0) break;
					s = (SparkNode *)AddTaskToQueue(QSparks(), game_addr(ORIG_SparkTask));
				}
			}
			PrimArg arg;
			ComposeAffineTransform(&Camera(), &M, &arg.m);
			arg.morph = TexBase() + BUF_Morph;
			arg.depth = -0x80;
			arg.second = 0;
			arg.scroll = (uint8_t)((c & 3) << 5);
			prim::play((prim::Layout *)m->cannon, PrimCallback, (int)&arg, 0);
			M.t[1] = *(const int16_t *)(Caster() + 0x24);
			ComposeAffineTransform(&Camera(), &M, &arg.m);
			arg.second = 1;
			arg.scroll = (uint8_t)((-c & 3) << 5);
			prim::play((prim::Layout *)m->beam, PrimCallback, (int)&arg, 0);
		}
		const int32_t e = m->counter - 0x28;
		if ((uint32_t)e < 0xC && (e & 3) == 0)
		{
			const int32_t idx = U8(game_addr(TBL_SpotTargets), (int32_t)((uint32_t)e >> 2));
			if (idx < m->ntargets)
			{
				SpotNode *p = (SpotNode *)AddTaskToQueue(QSpots(), game_addr(ORIG_SpotTask));
				if (p)
				{
					GetDefaultEffectPosition(m->targets[idx], p->pos);
					p->pos[2] = (int16_t)(p->pos[2] - 0x1F4);
					DecodeModelPrimLayout(game_addr(MODEL_Spot), p->layout, 0x1BC);
				}
			}
		}
		if (m->counter == 0) BdPlaySE(SOUND_Cannon(), 0, 0x80);
		if (m->counter == 0x37) StreamQueueLoad(STREAM_File, TexBase() + BUF_StreamDst, 0);
		StreamPump();
		if (m->counter == 0x41) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		m->counter++;
		if (m->counter < 0x4B) return 0;
		Flash() = 0;
		return TASK_END;
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
		Flash() = 0;
		if (mn->counter == 1 && !mn->started)
		{
			mn->started = 1;
			InitTaskQueuePool(QMain(), (void *)(TexBase() + POOL_Main), 0x9A8, 1);
			InitTaskQueuePool(QArcs(), (void *)(TexBase() + POOL_Arcs), 0x40, 0x40);
			InitTaskQueuePool(QSpots(), (void *)(TexBase() + POOL_Spots), 0x1D0, 3);
			InitTaskQueuePool(QRocks(), (void *)(TexBase() + POOL_Rocks), 0x20, 0x10);
			InitTaskQueuePool(QSparks(), (void *)TexBase(), 0x24, 0x40);
			MainNode *m = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
			Memset32(&m->counter, 0, 0x267);
			DecodeModelPrimLayout(game_addr(MODEL_Cannon), m->cannon, 0x8C4);
			DecodeModelPrimLayout(game_addr(MODEL_Beam), m->beam, 0xB8);
			for (int32_t k = 0; k < 8; k++)
				S16(TexBase(), BUF_Profile + 2 * k) = (int16_t)ComputeSin(shl32(k, 11) / 7);
			m->ntargets = Ctx()->actions->target_count;
			for (int32_t i = 0; i < m->ntargets; i++)
				m->targets[i] = Entity(Ctx()->actions->targets[i * TARGET_STRIDE]);
			StreamStateInit((void *)(TexBase() + BUF_Stream));
		}
		int a0;
		if (mn->started)
		{
			uint8_t *caster = Caster();
			ComputeBonesWorldMatrices(caster + 0x60, caster + 0x40);
			a0 = ExecuteTaskQueue(QMain());
			ExecuteTaskQueue(QArcs());
			BuildBoneMatricesFromPose(Caster() + 0x60);
			ExecuteTaskQueue(QSpots());
			ExecuteTaskQueue(QRocks());
			ExecuteTaskQueue(QSparks());
		}
		else a0 = (int)n;
		SetScreenFlash(Flash(), 0);
		if (mn->started && !a0)
		{
			if (StreamPreload(0)) return TASK_END;
			mn->counter = 2;
			return 0;
		}
		mn->counter++;
		return 0;
	}
}

	void register_mag287_mega_pulse_cannon()
	{
		register_port(megapulse287::ORIG_MasterTask, (void *)megapulse287::MasterTask, "287 MasterTask", 287);
		register_port(megapulse287::ORIG_MainTask, (void *)megapulse287::MainTask, "287 MainTask", 287);
		register_port(megapulse287::ORIG_ArcTask, (void *)megapulse287::ArcTask, "287 ArcTask", 287);
		register_port(megapulse287::ORIG_SpotTask, (void *)megapulse287::SpotTask, "287 SpotTask", 287);
		register_port(megapulse287::ORIG_RockTask, (void *)megapulse287::RockTask, "287 RockTask", 287);
		register_port(megapulse287::ORIG_SparkTask, (void *)megapulse287::SparkTask, "287 SparkTask", 287);
	}
}

