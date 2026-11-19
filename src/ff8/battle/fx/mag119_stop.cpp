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

// Effect 119: Stop

#include "mag_common.h"

namespace ff8fx
{
namespace stop119
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_2521594)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_2521598)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_Stop_PacketCursor)); }
	inline TaskQueue *QTargets() { return (TaskQueue *)game_addr(game::unk_25215B8); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_119_STOP_RootTask;
	static constexpr game::Id ORIG_TargetTask = game::MAG_119_STOP_TargetTask;
	static constexpr game::Id MODEL_Clock = game::unk_127DB5C;
	static const void *SOUND_Stop() { return (const void *)game_addr(game::unk_1287C34); }

	static const uint32_t SHARDS = 0x15FC;
	static const int SHARD_COUNT = 0x202;
	struct ShardGroup { int16_t record; uint32_t offset; int32_t count; game::Id object_cell; };
	static const ShardGroup GROUPS[7] = {
		{ 3, 0x15FC, 0x1D, game::unk_127DB68 }, { 5, 0x1C54, 0x4C, game::unk_127DB6C }, { 7, 0x2CF4, 0xE8, game::unk_127DB74 },
		{ 9, 0x5FB4, 0x34, game::unk_127DB78 }, { 11, 0x6B14, 0x1E, game::unk_127DB7C }, { 13, 0x71A4, 0x25, game::unk_127DB80 },
		{ 15, 0x79BC, 0x3A, game::unk_127DB84 },
	};

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesA(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_117_RotationFromAnglesZYX))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_065_RotationFromAnglesXZY))(angles, out); }
	inline void RotationY(int16_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))((int32_t)(uint16_t)angle, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3x3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZ32(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteSetFarColor(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteLoadRGB012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadRGB012))(a, b, c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteDPCT() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCT))(); }
	inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
	inline void GteStoreRGB012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_StoreRGB012))(a, b, c); }

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
		uint8_t *entity;
		int16_t pos[4];
		uint8_t clock[0x4E8];
	};
	struct PrimArg
	{
		Mat4x3 m;
		uint32_t morph;
		int16_t pos[4];
		Mat4x3 local;
	};
	struct Shard
	{
		int16_t pos[3];
		int16_t quad;
		int16_t vel[3];
		int16_t fade;
		int16_t spin[3];
		int16_t pad16;
		int16_t v[4][4];
	};
	struct DrawBlock
	{
		uint8_t far[4];
		uint8_t *group;
		Shard *shard;
		uint32_t cursor;
		uint32_t ot;
		int32_t shift;
		uint8_t pad18[8];
		int32_t otz;
		int32_t flag;
		int16_t pos[4];
		int16_t v[4][4];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(TargetNode) == 0x504, "Stop nodes");
	static_assert(sizeof(PrimArg) == 0x4C && sizeof(Shard) == 0x38 && sizeof(DrawBlock) == 0x50, "Stop blocks");

	static inline int16_t Div3(int32_t x)
	{
		const int32_t hi = (int32_t)(((int64_t)x * 0x55555556) >> 32);
		return (int16_t)(hi + (int32_t)((uint32_t)hi >> 31));
	}
	static inline int16_t Div4(int32_t x) { return (int16_t)(x / 4); }

	static Shard *InitTris(uint8_t *h, Shard *out, int32_t stride)
	{
		uint8_t *g = *(uint8_t **)(h + 0x20);
		int32_t n = *(const int32_t *)g;
		uint8_t *rec = g + 4;
		const uint8_t *verts = *(uint8_t **)(h + 4);
		*(uint8_t **)(h + 0x20) = rec;
		if (n <= 0) return out;
		int16_t *a = (int16_t *)(h + 0x34), *b = (int16_t *)(h + 0x3C), *c = (int16_t *)(h + 0x44);
		do
		{
			GteLoadV0(verts + *(const uint16_t *)(rec + 4) * 4);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(a);
			GteLoadV0(verts + *(const uint16_t *)(rec + 6) * 4);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(b);
			GteLoadV0(verts + *(const uint16_t *)(rec + 8) * 4);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(c);
			out->pos[0] = Div3(b[0] + c[0] + a[0]);
			out->pos[1] = Div3(a[1] + c[1] + b[1]);
			out->quad = 0;
			out->pos[2] = Div3(a[2] + c[2] + b[2]);
			for (int k = 0; k < 3; k++)
			{
				const int16_t *p = k == 0 ? a : k == 1 ? b : c;
				out->v[k][0] = (int16_t)(p[0] - out->pos[0]);
				out->v[k][1] = (int16_t)(p[1] - out->pos[1]);
				out->v[k][2] = (int16_t)(p[2] - out->pos[2]);
			}
			rec += stride;
			out++;
		} while (--n);
		*(uint8_t **)(h + 0x20) = rec;
		return out;
	}

	static Shard *InitQuads(uint8_t *h, Shard *out, int32_t stride)
	{
		uint8_t *g = *(uint8_t **)(h + 0x20);
		const uint8_t *verts = *(uint8_t **)(h + 4);
		int32_t n = *(const int32_t *)g;
		uint8_t *rec = g + 4;
		*(uint8_t **)(h + 0x20) = rec;
		if (n <= 0) return out;
		int16_t *p[4] = { (int16_t *)(h + 0x34), (int16_t *)(h + 0x3C), (int16_t *)(h + 0x44), (int16_t *)(h + 0x4C) };
		do
		{
			for (int k = 0; k < 4; k++)
			{
				GteLoadV0(verts + *(const uint16_t *)(rec + 4 + 2 * k) * 4);
				GteMVMVA_RotV0Tr();
				GteStoreIR123(p[k]);
			}
			out->pos[0] = Div4(p[1][0] + p[2][0] + p[3][0] + p[0][0]);
			out->pos[1] = Div4(p[3][1] + p[2][1] + p[0][1] + p[1][1]);
			out->quad = 1;
			out->pos[2] = Div4(p[0][2] + p[1][2] + p[3][2] + p[2][2]);
			for (int k = 0; k < 4; k++)
			{
				out->v[k][0] = (int16_t)(p[k][0] - out->pos[0]);
				out->v[k][1] = (int16_t)(p[k][1] - out->pos[1]);
				out->v[k][2] = (int16_t)(p[k][2] - out->pos[2]);
			}
			rec += stride;
			out++;
		} while (--n);
		*(uint8_t **)(h + 0x20) = rec;
		return out;
	}

	static void ShardsInit(uint8_t *h, Shard *buf, int32_t count, const int16_t *s)
	{
		uint8_t *model = *(uint8_t **)h;
		uint8_t *g = model + *(const int32_t *)model + 8;
		*(uint8_t **)(h + 0x20) = g;
		Shard *p = buf;
		if (*(const int32_t *)g != 0) p = InitTris(h, buf, 0x14);
		else *(uint8_t **)(h + 0x20) = g + 4;
		if (**(const int32_t **)(h + 0x20) != 0) p = InitQuads(h, p, 0x18);
		else *(uint8_t **)(h + 0x20) += 4;
		*(uint8_t **)(h + 0x20) += 8;
		if (**(const int32_t **)(h + 0x20) != 0) p = InitTris(h, p, 0x1C);
		else *(uint8_t **)(h + 0x20) += 4;
		if (**(const int32_t **)(h + 0x20) != 0) p = InitQuads(h, p, 0x24);
		else *(uint8_t **)(h + 0x20) += 4;
		Shard *q = buf;
		do
		{
			q->vel[0] = (int16_t)((q->pos[0] - s[0]) >> 4);
			q->vel[1] = (int16_t)((q->pos[1] - s[1]) >> 4);
			q->fade = 0;
			q->vel[2] = (int16_t)((q->pos[2] - s[2]) >> 4);
			const int32_t r0 = CrtRand();
			q->spin[0] = (int16_t)((r0 - 0x4000) >> 8);
			const int32_t r1 = CrtRand();
			q->spin[1] = (int16_t)((r1 - 0x4000) >> 8);
			const int32_t r2 = CrtRand();
			q->pad16 = 0;
			q->spin[2] = (int16_t)((r2 - 0x4000) >> 8);
			q++;
		} while (--count);
	}

	static void ShardsUpdate(Shard *q, int32_t n)
	{
		do
		{
			q->pos[0] = (int16_t)(q->pos[0] + q->vel[0]);
			q->pos[1] = (int16_t)(q->pos[1] + q->vel[1]);
			q->pos[2] = (int16_t)(q->pos[2] + q->vel[2]);
			Mat4x3 r = {};
			BuildRotationMatrixFromAngles(q->spin, &r);
			r.t[2] = 0;
			r.t[1] = 0;
			r.t[0] = 0;
			GteSetRotMatrixCtrl(&r);
			GteSetTransVectorCtrl(&r);
			for (int k = 0; k < 3; k++)
			{
				GteLoadV0(q->v[k]);
				GteMVMVA_RotV0Tr();
				GteStoreIR123(q->v[k]);
			}
			if (q->quad != 0)
			{
				GteLoadV0(q->v[3]);
				GteMVMVA_RotV0Tr();
				GteStoreIR123(q->v[3]);
			}
			q->fade = (int16_t)(q->fade + 0x100);
			if (q->fade > 0x1000) q->fade = 0x1000;
			q++;
		} while (--n);
	}

	static void ShardVertices(DrawBlock *P, int nv)
	{
		const Shard *s = P->shard;
		ff8fx::copy<(8)>(P->pos, s->pos);
		copy_bytes(P->v, s->v, 8 * nv);
		for (int k = 0; k < nv; k++)
		{
			P->v[k][0] = (int16_t)(P->v[k][0] + P->pos[0]);
			P->v[k][1] = (int16_t)(P->v[k][1] + P->pos[1]);
			P->v[k][2] = (int16_t)(P->v[k][2] + P->pos[2]);
		}
	}

	static inline uint32_t Clip(const uint8_t *xy, uint32_t bit)
	{
		const int16_t x = *(const int16_t *)xy, y = *(const int16_t *)(xy + 2);
		uint32_t c = 0;
		if (x < 0 || x > 0xA00) c |= bit;
		if (y < 0 || y > 0x6C0) c |= bit << 4;
		return c;
	}

	static inline void Insert(DrawBlock *P, uint8_t *packet)
	{
		InsertPrimAutoDepth(P->ot + (uint32_t)(P->otz >> P->shift) * 4, packet);
	}

	static void DrawFT3(DrawBlock *P)
	{
		uint8_t *rec = P->group;
		int32_t n = *(const int32_t *)rec;
		rec += 4;
		P->group = rec;
		uint8_t *base = (uint8_t *)P->cursor;
		if (n <= 0) return;
		do
		{
			ShardVertices(P, 3);
			GteLoadV012(P->v[0], P->v[1], P->v[2]);
			GteRTPT();
			uint8_t *e = base + 8;
			*(uint32_t *)base = 0x7000000;
			*(uint32_t *)(e - 4) = *(const uint32_t *)rec;
			*(uint32_t *)(e + 4) = *(const uint32_t *)(rec + 0xC);
			*(uint32_t *)(e + 0xC) = *(const uint32_t *)(rec + 0x10);
			*(uint32_t *)(e + 0x14) = *(const uint32_t *)(rec + 8) >> 16;
			GteReadFLAG(&P->flag);
			if (!(P->flag & 0x60000))
			{
				GteReadSXY012(e, e + 8, e + 0x10);
				GteAVSZ3();
				const uint32_t c = Clip(e, 1) | Clip(e + 8, 2) | Clip(e + 0x10, 4);
				if ((c & 7) != 7 && (c & 0x70) != 0x70)
				{
					GteReadOTZ32(&P->otz);
					if (P->shard->fade != 0)
					{
						GteSetFarColor(P->far[0], P->far[1], P->far[2]);
						GteLoadRGBC(e - 4);
						GteSetIR0(P->shard->fade);
						GteDPCS();
						GteStoreRGB2(e - 4);
					}
					Insert(P, base);
					base += 0x20;
				}
			}
			rec += 0x14;
			P->shard++;
		} while (--n);
		P->group = rec;
		P->cursor = (uint32_t)base;
	}

	static void DrawFT4(DrawBlock *P)
	{
		uint8_t *rec = P->group;
		int32_t n = *(const int32_t *)rec;
		rec += 4;
		P->group = rec;
		uint8_t *e = (uint8_t *)P->cursor;
		if (n <= 0) return;
		do
		{
			ShardVertices(P, 4);
			GteLoadV012(P->v[0], P->v[1], P->v[2]);
			GteRTPT();
			*(uint32_t *)e = 0x9000000;
			*(uint32_t *)(e + 4) = *(const uint32_t *)rec;
			*(uint32_t *)(e + 0xC) = *(const uint32_t *)(rec + 0xC);
			*(uint32_t *)(e + 0x14) = *(const uint32_t *)(rec + 0x10);
			*(uint32_t *)(e + 0x1C) = *(const uint32_t *)(rec + 0x14);
			*(uint32_t *)(e + 0x24) = *(const uint32_t *)(rec + 0x14) >> 16;
			GteReadFLAG(&P->flag);
			if (!(P->flag & 0x60000))
			{
				GteReadSXY012(e + 8, e + 0x10, e + 0x18);
				GteLoadV0(P->v[3]);
				GteRTPS();
				uint32_t c = Clip(e + 8, 1) | Clip(e + 0x10, 2) | Clip(e + 0x18, 4);
				GteReadSXY2(e + 0x20);
				GteAVSZ4();
				c |= Clip(e + 0x20, 8);
				if ((c & 0xF) != 0xF && (c & 0xF0) != 0xF0)
				{
					GteReadOTZ32(&P->otz);
					if (P->shard->fade != 0)
					{
						GteSetFarColor(P->far[0], P->far[1], P->far[2]);
						GteLoadRGBC(e + 4);
						GteSetIR0(P->shard->fade);
						GteDPCS();
						GteStoreRGB2(e + 4);
					}
					Insert(P, e);
					e += 0x28;
				}
			}
			rec += 0x18;
			P->shard++;
		} while (--n);
		P->cursor = (uint32_t)e;
		P->group = rec;
	}

	static void DrawGT3(DrawBlock *P)
	{
		uint8_t *rec = P->group;
		int32_t n = *(const int32_t *)rec;
		rec += 4;
		P->group = rec;
		uint8_t *base = (uint8_t *)P->cursor;
		if (n <= 0) return;
		do
		{
			ShardVertices(P, 3);
			GteLoadV012(P->v[0], P->v[1], P->v[2]);
			GteRTPT();
			uint8_t *e = base + 8;
			*(uint32_t *)base = 0x9000000;
			*(uint32_t *)(e - 4) = *(const uint32_t *)rec;
			*(uint32_t *)(e + 4) = *(const uint32_t *)(rec + 0xC);
			*(uint32_t *)(e + 0x10) = *(const uint32_t *)(rec + 0x10);
			*(uint32_t *)(e + 0x1C) = *(const uint32_t *)(rec + 8) >> 16;
			GteReadFLAG(&P->flag);
			if (!(P->flag & 0x60000))
			{
				GteReadSXY012(e, e + 0xC, e + 0x18);
				GteAVSZ3();
				const uint32_t c = Clip(e, 1) | Clip(e + 0xC, 2) | Clip(e + 0x18, 4);
				if ((c & 7) != 7 && (c & 0x70) != 0x70)
				{
					GteReadOTZ32(&P->otz);
					if (P->shard->fade != 0)
					{
						GteSetFarColor(P->far[0], P->far[1], P->far[2]);
						GteLoadRGB012(rec + 0x14, rec + 0x18, e - 4);
						GteSetIR0(P->shard->fade);
						GteDPCT();
						GteStoreRGB012(e + 8, e + 0x14, e - 4);
					}
					else
					{
						*(uint32_t *)(e + 8) = *(const uint32_t *)(rec + 0x14);
						*(uint32_t *)(e + 0x14) = *(const uint32_t *)(rec + 0x18);
					}
					Insert(P, base);
					base += 0x28;
				}
			}
			rec += 0x1C;
			P->shard++;
		} while (--n);
		P->group = rec;
		P->cursor = (uint32_t)base;
	}

	static void DrawGT4(DrawBlock *P)
	{
		uint8_t *rec = P->group;
		int32_t n = *(const int32_t *)rec;
		rec += 4;
		P->group = rec;
		uint8_t *e = (uint8_t *)P->cursor;
		if (n <= 0) return;
		do
		{
			ShardVertices(P, 4);
			GteLoadV012(P->v[0], P->v[1], P->v[2]);
			GteRTPT();
			*(uint32_t *)e = 0xC000000;
			*(uint32_t *)(e + 4) = *(const uint32_t *)rec;
			*(uint32_t *)(e + 0xC) = *(const uint32_t *)(rec + 0xC);
			*(uint32_t *)(e + 0x18) = *(const uint32_t *)(rec + 0x10);
			*(uint32_t *)(e + 0x24) = *(const uint32_t *)(rec + 0x14);
			*(uint32_t *)(e + 0x30) = *(const uint32_t *)(rec + 0x14) >> 16;
			GteReadFLAG(&P->flag);
			if (!(P->flag & 0x60000))
			{
				GteReadSXY012(e + 8, e + 0x14, e + 0x20);
				GteLoadV0(P->v[3]);
				GteRTPS();
				uint32_t c = Clip(e + 8, 1) | Clip(e + 0x14, 2) | Clip(e + 0x20, 4);
				GteReadSXY2(e + 0x2C);
				GteAVSZ4();
				c |= Clip(e + 0x2C, 8);
				if ((c & 0xF) != 0xF && (c & 0xF0) != 0xF0)
				{
					GteReadOTZ32(&P->otz);
					if (P->shard->fade != 0)
					{
						GteSetFarColor(P->far[0], P->far[1], P->far[2]);
						GteLoadRGB012(rec + 0x18, rec + 0x1C, rec + 0x20);
						GteSetIR0(P->shard->fade);
						GteDPCT();
						GteStoreRGB012(e + 0x10, e + 0x1C, e + 0x28);
						GteLoadRGBC(e + 4);
						GteDPCS();
						GteStoreRGB2(e + 4);
					}
					else
					{
						*(uint32_t *)(e + 0x10) = *(const uint32_t *)(rec + 0x18);
						*(uint32_t *)(e + 0x1C) = *(const uint32_t *)(rec + 0x1C);
						*(uint32_t *)(e + 0x28) = *(const uint32_t *)(rec + 0x20);
					}
					Insert(P, e);
					e += 0x34;
				}
			}
			rec += 0x24;
			P->shard++;
		} while (--n);
		P->cursor = (uint32_t)e;
		P->group = rec;
	}

	static void ShardsDraw(Shard *buf, uint8_t *model)
	{
		DrawBlock *P = (DrawBlock *)FieldAlloc(0x50);
		*(uint32_t *)P->far = 0;
		P->cursor = PacketCursor();
		P->ot = RenderOT(0x44);
		P->shift = 2;
		P->shard = buf;
		P->group = model + *(const int32_t *)model + 8;
		if (*(const int32_t *)P->group != 0) DrawFT3(P);
		else P->group += 4;
		if (*(const int32_t *)P->group != 0) DrawFT4(P);
		else P->group += 4;
		P->group += 8;
		if (*(const int32_t *)P->group != 0) DrawGT3(P);
		else P->group += 4;
		if (*(const int32_t *)P->group != 0) DrawGT4(P);
		else P->group += 4;
		PacketCursor() = P->cursor;
		FieldFree(0x50);
	}

	static void ShardsDrawAll(uint8_t *shards)
	{
		for (int k = 0; k < 7; k++)
			ShardsDraw((Shard *)(shards + GROUPS[k].offset - SHARDS), (uint8_t *)(game_addr(MODEL_Clock) + *(const uint32_t *)game_addr(GROUPS[k].object_cell)));
	}

	static void ClockPartBody(prim::Layout *l, prim::Record *r, const PrimArg *arg, int32_t frame)
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
		if (r->flags & 0x400) RotationFromAnglesA(r->rot, &m);
		else if (r->flags & 0x20000) RotationFromAnglesB(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		int16_t v[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		int32_t t[3];
		if (r->flags & 0x200)
		{
			t[0] = v[0];
			t[1] = v[1];
			t[2] = v[2];
		}
		else if (frame >= 0x35 && (int16_t)r->index >= 3 && (int16_t)r->index <= 0xF && (r->index & 1))
		{
			if (frame == 0x35)
			{
				Mat4x3 w = {};
				int16_t s[4] = {};
				w.t[0] = arg->pos[0] + v[0];
				s[0] = (int16_t)w.t[0];
				w.t[1] = arg->pos[1] + v[1];
				s[1] = (int16_t)w.t[1];
				w.t[2] = arg->pos[2] + v[2];
				s[2] = (int16_t)w.t[2];
				MatrixMultiply(&arg->local, &m);
				GteSetRotMatrix(&m);
				GteSetTransVector(&w);
				for (int k = 0; k < 7; k++)
					if (GROUPS[k].record == (int16_t)r->index)
						ShardsInit(h, (Shard *)(TexBase() + GROUPS[k].offset), GROUPS[k].count, s);
			}
			FieldFree(0x58);
			return;
		}
		else
		{
			GteSetRotMatrixCtrl(&arg->m);
			GteLoadV0(v);
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
				Mat4x3 s = {};
				s.m[0][0] = r->scale[0];
				s.m[1][1] = r->scale[1];
				s.m[2][2] = r->scale[2];
				MatrixMultiply3x3(&m, &s);
			}
			else
			{
				int32_t sv[3] = { r->scale[0], r->scale[1], r->scale[2] };
				Scale3DMatrix(&m, sv);
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
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void __cdecl ClockPart(prim::Layout *l, prim::Record *r, int arg)
	{
		ClockPartBody(l, r, (const PrimArg *)arg, l->frame);
	}
}
}

namespace ff8fx
{
namespace stop119
{
	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		if (t->counter == 0) BdPlaySE3D(SOUND_Stop(), 1, t->pos);
		if (t->counter >= 0)
		{
			PrimArg arg;
			RotationY((int16_t)-*(const int16_t *)(t->entity + 0xE), &arg.local);
			const int32_t size = *(const int16_t *)(t->entity + 0x26) + 0x1000;
			const int32_t sn = ComputeSin(*(const int16_t *)(t->entity + 0xE));
			const int32_t x = t->pos[0] - (mul32(sn, size) >> 15);
			arg.local.t[0] = x;
			arg.pos[0] = (int16_t)x;
			const int32_t cs = ComputeCos(*(const int16_t *)(t->entity + 0xE));
			const int32_t z = t->pos[2] - (mul32(cs, size) >> 15);
			arg.local.t[2] = z;
			arg.pos[2] = (int16_t)z;
			arg.local.t[1] = t->pos[1];
			arg.pos[1] = t->pos[1];
			arg.pos[3] = 0;
			ComposeAffineTransform(&Camera(), &arg.local, &arg.m);
			arg.morph = TexBase() + 0xF0C;
			prim::play((prim::Layout *)t->clock, ClockPart, (int)&arg, 0);
			if (t->counter == 0x32) ScreenFadeTask(0, 1, 0, 0x80);
			if (t->counter < 0x35) goto end;
			ShardsUpdate((Shard *)(TexBase() + SHARDS), SHARD_COUNT);
			GteSetRotMatrix(&Camera());
			GteSetTransVector(&Camera());
			ShardsDrawAll((uint8_t *)(TexBase() + SHARDS));
		}
		if (t->counter == 0x35) ApplyActionResultToTarget(Ctx()->actions[t->action].targets);
	end:
		if (t->counter >= 0x46) return TASK_END;
		t->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0x866C;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x2066C;
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QTargets(), (void *)TexBase(), 0x504, 3);
			}
			if (r->action <= Ctx()->actions[0].last_action)
			{
				TargetNode *t = (TargetNode *)AddTaskToQueue(QTargets(), game_addr(ORIG_TargetTask));
				Memset32(&t->counter, 0, 0x13E);
				t->action = (int16_t)(uint16_t)r->action;
				t->entity = Entity(Ctx()->actions[t->action].targets[0]);
				GetDefaultEffectPosition(t->entity, t->pos);
				DecodeModelPrimLayout(game_addr(MODEL_Clock), t->clock, 0x4E8);
				r->action++;
			}
		}
		const int a = r->started ? ExecuteTaskQueue(QTargets()) : (int)n;
		if (r->started && a == 0) return TASK_END;
		r->counter++;
		if (r->counter >= 0x14) r->counter = 0;
		return 0;
	}
}

	void register_mag119_stop()
	{
		register_port(stop119::ORIG_RootTask, (void *)stop119::RootTask, "S119 RootTask", 119);
		register_port(stop119::ORIG_TargetTask, (void *)stop119::TargetTask, "S119 TargetTask", 119);
	}
}

