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

// Effect 136: Oil Shot

#include "mag_common.h"
#include "mag_ice_common.h"

namespace ff8fx
{
namespace oil136
{
	using namespace eng;
	using namespace magc;
	using namespace icec;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g_Mag136_CasterEntity)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g_Mag136_MagicBufferBase)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_Mag136_CastContext)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_Mag136_PacketCursor)); }
	inline Mat4x3 &ZeroMatrix() { return var<Mat4x3>(game_addr(game::g_Mag136_ZeroMatrix)); }
	inline TaskQueue *QFalls() { return (TaskQueue *)game_addr(game::g_Mag136_QueueFalls); }
	inline TaskQueue *QSplats() { return (TaskQueue *)game_addr(game::g_Mag136_QueueSplats); }
	inline TaskQueue *QStreams() { return (TaskQueue *)game_addr(game::g_Mag136_QueueStreams); }
	inline TaskQueue *QSprays() { return (TaskQueue *)game_addr(game::g_Mag136_QueueSprays); }
	inline TaskQueue *QShot() { return (TaskQueue *)game_addr(game::g_Mag136_QueueShot); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_136_OilShot_MasterTask;
	static constexpr game::Id ORIG_ShotTask = game::MAG_136_OilShot_ShotTask;
	static constexpr game::Id ORIG_FallTask = game::MAG_136_OilShot_FallTask;
	static constexpr game::Id ORIG_SplatTask = game::MAG_136_OilShot_SplatTask;
	static constexpr game::Id ORIG_StreamTask = game::MAG_136_OilShot_StreamTask;
	static constexpr game::Id ORIG_SprayTask = game::MAG_136_OilShot_SprayTask;
	static const uint32_t VERTS = 0x3E0C;
	static const uint32_t ARENA_A = 0x416C;
	static const uint32_t ARENA_B = 0x1C16C;
	static uint8_t *MESH() { return (uint8_t *)game_addr(game::Mag136_OilMesh); }
	static constexpr game::Id MODEL_Mouth = game::unk_11F40F4;
	static constexpr game::Id MODEL_Hit = game::unk_11F5A90;
	static constexpr game::Id MODEL_Pool = game::unk_11F4E1C;
	static constexpr game::Id MODEL_Stream = game::unk_11F342C;
	static constexpr game::Id MODEL_Fall = game::unk_11F7D84;
	static constexpr game::Id MODEL_Splash = game::unk_11F74FC;
	static constexpr game::Id SEQ_Spray = game::unk_11F2390;
	static constexpr game::Id SEQ_Splat = game::unk_11F24D8;
	static const int16_t *MOUTH() { return (const int16_t *)game_addr(game::Mag136_MouthBonePoint); }
	static const void *SCALE_Z() { return (const void *)game_addr(game::Mag136_MeshScaleMatrix); }
	static int16_t &ScaleZ() { return *(int16_t *)game_addr(game::unk_11F81E0); }
	static const int32_t *WOBBLE() { return (const int32_t *)game_addr(game::Mag136_WobbleTable); }
	static const void *SOUND_Shot() { return (const void *)game_addr(game::unk_11F81CC); }

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MulMatrix3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline int32_t BuildOrthonormalBasis(const int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline void RotX(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_063_RotationXMatrix))(angle, out); }
	inline void RotY(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline void RotZ(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_065_RotationZMatrix))(angle, out); }
	inline void Matrix3x3MultiplyPSX(const Mat4x3 *a, const Mat4x3 *b, Mat4x3 *out) { fn<void (__cdecl *)(const Mat4x3 *, const Mat4x3 *, Mat4x3 *)>(game_addr(game::Matrix3x3_MultiplyPSX))(a, b, out); }
	inline void MatrixMultiplyVector(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void BlendVectors(const int16_t *a, const int16_t *b, int32_t wa, int32_t wb, int16_t *out) { fn<void (__cdecl *)(const int16_t *, const int16_t *, int32_t, int32_t, int16_t *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }
	inline void GteZeroTrans() { fn<void (__cdecl *)()>(game_addr(game::GTE_ZeroTranslation))(); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
	inline void ScaleMatrix(Mat4x3 *m, const int32_t *v) { fn<void (__cdecl *)(Mat4x3 *, const int32_t *)>(game_addr(game::scale3DMatrix))(m, v); }

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t started;
		uint32_t arena;
	};
	struct ShotNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint8_t *target;
		int16_t start[4];
		int16_t pos[4];
		int16_t prev[4];
		uint8_t mouth[0xC8];
		uint8_t hit[0x1F4];
		uint8_t pool[0x184];
	};
	struct FallNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t landed;
		int16_t vel[3];
		int16_t pad1A;
		uint8_t layout[0x2A0];
	};
	struct SplatNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t frame;
	};
	struct StreamNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t zero;
		int16_t vel[3];
		int16_t pad1A;
		uint8_t layout[0xA8];
	};
	struct SprayNode
	{
		TaskNode hdr;
		int16_t angle;
		int16_t spin;
		int16_t pos[3];
		int16_t scale;
		int16_t vel[3];
		int16_t frame;
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t depth;
		int32_t mode;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(ShotNode) == 0x46C && sizeof(FallNode) == 0x2BC, "Oil Shot nodes");
	static_assert(sizeof(SplatNode) == 0x14 && sizeof(StreamNode) == 0xC4 && sizeof(SprayNode) == 0x20, "Oil Shot nodes");
	static_assert(sizeof(PrimArg) == 0x2C, "Oil Shot prim block");

	static uint8_t *&Cursor(uint8_t *h, int off) { return *(uint8_t **)(h + off); }
	static bool OffX(int16_t x) { return x < 0 || x > 0xA00; }
	static bool OffY(int16_t y) { return y < 0 || y > 0x6C0; }

	static uint32_t FlatQuads(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *rec = Cursor(h, 0x20);
		const int32_t count = *(const int32_t *)rec;
		rec += 4;
		uint8_t *verts = Cursor(h, 4);
		Cursor(h, 0x20) = rec;
		uint8_t *p = (uint8_t *)cursor;
		if (count <= 0) return cursor;
		for (int32_t left = count; left; left--, rec += 0x18)
		{
			GteLoadV012(verts + 4 * *(const uint16_t *)(rec + 4), verts + 4 * *(const uint16_t *)(rec + 6), verts + 4 * *(const uint16_t *)(rec + 8));
			GteRTPT();
			*(uint32_t *)p = 0x09000000;
			*(uint32_t *)(p + 4) = *(const uint32_t *)rec;
			*(uint32_t *)(p + 0xC) = *(const uint32_t *)(rec + 0xC);
			*(uint32_t *)(p + 0x14) = *(const uint32_t *)(rec + 0x10);
			const uint32_t uv = *(const uint32_t *)(rec + 0x14);
			*(uint32_t *)(p + 0x1C) = uv;
			*(uint32_t *)(p + 0x24) = uv >> 16;
			GteReadFLAG(h + 0x30);
			if (*(const uint32_t *)(h + 0x30) & 0x60000) continue;
			GteReadSXY012(p + 8, p + 0x10, p + 0x18);
			GteLoadV0(verts + 4 * *(const uint16_t *)(rec + 0xA));
			GteRTPS();
			uint32_t clip = 0;
			if (OffX(*(const int16_t *)(p + 8))) clip = 1;
			if (OffX(*(const int16_t *)(p + 0x10))) clip |= 2;
			if (OffX(*(const int16_t *)(p + 0x18))) clip |= 4;
			if (OffY(*(const int16_t *)(p + 0xA))) clip |= 0x10;
			if (OffY(*(const int16_t *)(p + 0x12))) clip |= 0x20;
			if (OffY(*(const int16_t *)(p + 0x1A))) clip |= 0x40;
			GteReadSXY2(p + 0x20);
			GteAVSZ4();
			if (OffX(*(const int16_t *)(p + 0x20))) clip |= 8;
			if (OffY(*(const int16_t *)(p + 0x22))) clip |= 0x80;
			if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
			GteReadOTZReg(h + 0x2C);
			if (*(const int32_t *)(h + 0xC) != 0)
			{
				GteSetFarColor(h[8], h[9], h[0xA]);
				GteLoadRGBC(p + 4);
				GteSetIR0(*(const int32_t *)(h + 0xC));
				GteDPCS();
				GteStoreRGB2(p + 4);
			}
			const uint32_t otp = ot + 4 * (*(const int32_t *)(h + 0x2C) >> shift);
			uint8_t *md = p + 0x28;
			*(uint32_t *)md = 0x01000000;
			*(uint32_t *)(md + 4) = 0xE1000220;
			InsertPrimAutoDepth(otp, md);
			InsertPrimAutoDepth(otp, p);
			p += 0x30;
		}
		Cursor(h, 0x20) = rec;
		return (uint32_t)p;
	}

	static uint32_t GouraudQuads(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *rec = Cursor(h, 0x20);
		const int32_t count = *(const int32_t *)rec;
		rec += 4;
		uint8_t *verts = Cursor(h, 4);
		Cursor(h, 0x20) = rec;
		uint8_t *p = (uint8_t *)cursor;
		if (count <= 0) return cursor;
		for (int32_t left = count; left; left--, rec += 0x18)
		{
			GteLoadV012(verts + 4 * *(const uint16_t *)(rec + 4), verts + 4 * *(const uint16_t *)(rec + 6), verts + 4 * *(const uint16_t *)(rec + 8));
			GteRTPT();
			*(uint32_t *)p = 0x08000000;
			*(uint32_t *)(p + 4) = *(const uint32_t *)rec;
			GteReadFLAG(h + 0x30);
			if (*(const uint32_t *)(h + 0x30) & 0x60000) continue;
			GteReadSXY012(p + 8, p + 0x10, p + 0x18);
			GteLoadV0(verts + 4 * *(const uint16_t *)(rec + 0xA));
			GteRTPS();
			uint32_t clip = 0;
			if (OffX(*(const int16_t *)(p + 8))) clip = 1;
			if (OffX(*(const int16_t *)(p + 0x10))) clip |= 2;
			if (OffX(*(const int16_t *)(p + 0x18))) clip |= 4;
			if (OffY(*(const int16_t *)(p + 0xA))) clip |= 0x10;
			if (OffY(*(const int16_t *)(p + 0x12))) clip |= 0x20;
			if (OffY(*(const int16_t *)(p + 0x1A))) clip |= 0x40;
			GteReadSXY2(p + 0x20);
			GteAVSZ4();
			if (OffX(*(const int16_t *)(p + 0x20))) clip |= 8;
			if (OffY(*(const int16_t *)(p + 0x22))) clip |= 0x80;
			if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
			GteReadOTZReg(h + 0x2C);
			*(uint32_t *)(p + 0xC) = *(const uint32_t *)(rec + 0xC);
			*(uint32_t *)(p + 0x14) = *(const uint32_t *)(rec + 0x10);
			*(uint32_t *)(p + 0x1C) = *(const uint32_t *)(rec + 0x14);
			const int32_t z = (int32_t)((uint32_t)*(const int32_t *)(h + 0x2C) + (uint32_t)*(const int32_t *)(h + 0xC));
			*(int32_t *)(h + 0x2C) = z;
			if (z < 0) continue;
			const uint32_t otp = ot + 4 * (z >> shift);
			uint8_t *md = p + 0x24;
			*(uint32_t *)md = 0x01000000;
			*(uint32_t *)(md + 4) = 0xE1000220;
			InsertPrimAutoDepth(otp, md);
			InsertPrimAutoDepth(otp, p);
			uint8_t *md2 = p + 0x2C;
			*(uint32_t *)md2 = 0x01000000;
			*(uint32_t *)(md2 + 4) = 0xE1000240;
			InsertPrimAutoDepth(otp, md2);
			p += 0x34;
		}
		Cursor(h, 0x20) = rec;
		return (uint32_t)p;
	}

	static uint32_t ObjectDraw(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *m = Cursor(h, 0);
		Cursor(h, 0x20) = m + *(const uint32_t *)m + 0xC;
		uint32_t r = cursor;
		if (*(const uint32_t *)Cursor(h, 0x20) != 0) r = FlatQuads(h, ot, shift, cursor);
		else Cursor(h, 0x20) += 4;
		Cursor(h, 0x20) += 4;
		if (*(const uint32_t *)Cursor(h, 0x20) != 0) return GouraudQuads(h, ot, shift, r);
		Cursor(h, 0x20) += 4;
		return r;
	}

	static void ScrollU(uint8_t *p, const int *off, int n, uint16_t add, uint8_t wrap)
	{
		uint32_t v[4], o = 0;
		for (int k = 0; k < n; k++)
		{
			v[k] = p[off[k]] + (uint32_t)add;
			o |= v[k];
		}
		for (int k = 0; k < n; k++) p[off[k]] = (uint8_t)(o > 0xFF ? v[k] - wrap : v[k]);
	}

	static uint8_t *Windowed(const uint8_t *h, uint32_t otp, uint8_t *main, uint8_t *p)
	{
		uint8_t *t1 = p;
		*(uint32_t *)t1 = 0x02000000;
		*(uint32_t *)(t1 + 4) = TextureWindow(h + 0x14);
		*(uint32_t *)(t1 + 8) = 0;
		InsertPrimAutoDepth(otp, t1);
		uint8_t *md = p + 0xC;
		*(uint32_t *)md = 0x01000000;
		*(uint32_t *)(md + 4) = 0xE1000220;
		InsertPrimAutoDepth(otp, md);
		InsertPrimAutoDepth(otp, main);
		uint8_t *t2 = p + 0x14;
		*(uint32_t *)t2 = 0x02000000;
		*(uint32_t *)(t2 + 4) = TextureWindow(h + 0x1C);
		*(uint32_t *)(t2 + 8) = 0;
		InsertPrimAutoDepth(otp, t2);
		return p + 0x20;
	}

	static uint32_t MeshQuads(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *rec = Cursor(h, 0x24);
		const int32_t count = *(const int32_t *)rec;
		rec += 4;
		uint8_t *verts = Cursor(h, 4);
		Cursor(h, 0x24) = rec;
		uint8_t *p = (uint8_t *)cursor;
		if (count <= 0) return cursor;
		for (int32_t left = count; left; left--, rec += 0x24)
		{
			GteLoadV012(verts + 4 * *(const uint16_t *)(rec + 4), verts + 4 * *(const uint16_t *)(rec + 6), verts + 4 * *(const uint16_t *)(rec + 8));
			GteRTPT();
			*(uint32_t *)p = 0x0C000000;
			*(uint32_t *)(p + 4) = *(const uint32_t *)rec;
			*(uint32_t *)(p + 0xC) = *(const uint32_t *)(rec + 0xC);
			*(uint32_t *)(p + 0x18) = *(const uint32_t *)(rec + 0x10);
			const uint32_t uv = *(const uint32_t *)(rec + 0x14);
			*(uint32_t *)(p + 0x24) = uv;
			*(uint32_t *)(p + 0x30) = uv >> 16;
			GteReadFLAG(h + 0x34);
			if (*(const uint32_t *)(h + 0x34) & 0x60000) continue;
			GteReadSXY012(p + 8, p + 0x14, p + 0x20);
			GteLoadV0(verts + 4 * *(const uint16_t *)(rec + 0xA));
			GteRTPS();
			uint32_t clip = 0;
			if (OffX(*(const int16_t *)(p + 8))) clip = 1;
			if (OffX(*(const int16_t *)(p + 0x14))) clip |= 2;
			if (OffX(*(const int16_t *)(p + 0x20))) clip |= 4;
			if (OffY(*(const int16_t *)(p + 0xA))) clip |= 0x10;
			if (OffY(*(const int16_t *)(p + 0x16))) clip |= 0x20;
			if (OffY(*(const int16_t *)(p + 0x22))) clip |= 0x40;
			GteReadSXY2(p + 0x2C);
			GteAVSZ4();
			if (OffX(*(const int16_t *)(p + 0x2C))) clip |= 8;
			if (OffY(*(const int16_t *)(p + 0x2E))) clip |= 0x80;
			if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
			GteReadOTZReg(h + 0x30);
			*(uint32_t *)(p + 0x10) = *(const uint32_t *)(rec + 0x18);
			*(uint32_t *)(p + 0x1C) = *(const uint32_t *)(rec + 0x1C);
			*(uint32_t *)(p + 0x28) = *(const uint32_t *)(rec + 0x20);
			static const int us[4] = { 0xC, 0x18, 0x24, 0x30 };
			ScrollU(p, us, 4, *(const uint16_t *)(h + 0x10), h[0x20]);
			const uint32_t otp = ot + 4 * (*(const int32_t *)(h + 0x30) >> shift);
			p = Windowed(h, otp, p, p + 0x34);
		}
		Cursor(h, 0x24) = rec;
		return (uint32_t)p;
	}

	static uint32_t MeshDraw(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *m = Cursor(h, 0);
		Cursor(h, 0x24) = m + *(const uint32_t *)m + 0x1C;
		return MeshQuads(h, ot, shift, cursor);
	}

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
		if (r->flags & 0x40000) RotationFromAnglesB(r->rot, &m);
		else RotationFromAngles(r->rot, &m);
		const int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		if (r->flags & 0x1000)
		{
			GteSetRotMatrixCtrl(&ZeroMatrix());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			MatrixMultiply(&ZeroMatrix(), &m);
		}
		else if (r->flags & 0x200)
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
			MatrixMultiply(&arg->m, &m);
		}
		m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)arg->m.t[0]);
		m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)arg->m.t[1]);
		m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)arg->m.t[2]);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			if (r->flags & 0x100)
			{
				Mat4x3 d = {};
				d.m[0][0] = r->scale[0];
				d.m[1][1] = r->scale[1];
				d.m[2][2] = r->scale[2];
				MulMatrix3(&m, &d);
			}
			else
			{
				const int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
				ScaleMatrix(&m, v);
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
		if (arg->mode == 1 || (arg->mode == 2 && r->flags_lo == 0))
			PacketCursor() = ObjectDraw(h, RenderOT(), 2, PacketCursor());
		else
		{
			*(int32_t *)(h + 0x18) = arg->depth;
			PacketCursor() = RenderPrim2(h, RenderOT(), 2, PacketCursor());
		}
		FieldFree(0x58);
	}

	static void NormalizeMesh(uint8_t *model)
	{
		const int32_t count = *(const int32_t *)(model + 4);
		int32_t mx = *(const int16_t *)(model + 0x1C), mn = mx;
		const uint8_t *z = model + 0x1C;
		for (int32_t i = count - 1; i; i--, z += 8)
		{
			const int32_t v = *(const int16_t *)z;
			if (mn > v) mn = v;
			else if (mx < v) mx = v;
		}
		const int32_t range = mx - mn;
		uint8_t *p = model + 0xE;
		for (int32_t i = count; i; i--, p += 8)
			*(int16_t *)p = (int16_t)(shl32(*(const int16_t *)(p - 2) - mn, 12) / range);
	}

	static void BonePoint(const int16_t *v, int16_t *out)
	{
		uint8_t *c = Caster();
		const uint32_t bones = **(const uint32_t *const *)(c + 0x64) + 0x10;
		BuildBoneMatricesFromPose(c + 0x60);
		Mat4x3 m;
		const int32_t bone = v[3];
		ComposeAffineTransform((const Mat4x3 *)(Caster() + 0x40), (const Mat4x3 *)(bones + (uint32_t)bone * 0x30 + 0x10), &m);
		GteSetRotMatrixCtrl(&m);
		GteSetTransVectorCtrl(&m);
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(out);
	}

	static void RotXY(int32_t a, int32_t b, Mat4x3 *out)
	{
		Mat4x3 x, y;
		RotX(b, &x);
		RotY(a, &y);
		Matrix3x3MultiplyPSX(&x, &y, out);
	}

	static void AngleVector(int32_t a, int32_t b, int16_t *out)
	{
		out[2] = (int16_t)ComputeCos(a);
		const int32_t s = -ComputeSin(a);
		out[0] = (int16_t)(mul32(ComputeSin(b), s) >> 12);
		out[1] = (int16_t)((int32_t)(0u - (uint32_t)mul32(ComputeCos(b), s)) >> 12);
	}

	static void WobbleMesh(const uint8_t *model, int32_t amp, int16_t *out)
	{
		int32_t count = *(const int32_t *)(model + 4);
		const int16_t *v = (const int16_t *)(model + 8);
		do
		{
			GteLoadV0(v);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(out);
			const int32_t w = v[3] >> 1;
			out[1] = (int16_t)(out[1] - (int16_t)(mul32(WOBBLE()[w], amp) >> 0x11));
			out += 4;
			v += 4;
		} while (--count);
	}

	static int16_t ShotResidue()
	{
		return 0;
	}

	static void Play(void *layout, const PrimArg *arg)
	{
		prim::play((prim::Layout *)layout, PrimCallback, (int)arg, 0);
	}

	static void BlockAt(PrimArg *arg, const int16_t *p4)
	{
		ff8fx::copy<(sizeof(arg->m.m))>(arg->m.m, Camera().m);
		GteSetRotMatrixCtrl(&Camera());
		GteLoadV0(p4);
		GteMVMVA_RotV0();
		GteReadMAC123(arg->m.t);
		arg->m.t[0] = (int32_t)((uint32_t)arg->m.t[0] + (uint32_t)Camera().t[0]);
		arg->m.t[1] = (int32_t)((uint32_t)arg->m.t[1] + (uint32_t)Camera().t[1]);
		arg->m.t[2] = (int32_t)((uint32_t)arg->m.t[2] + (uint32_t)Camera().t[2]);
	}
}
}

namespace ff8fx
{
namespace oil136
{
	static void SprayDraw(const int16_t *pos, int16_t scale, int16_t frame, int16_t angle)
	{
		TransformCameraByShadowRotation(pos, scale, -(scale >> 2));
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Spray);
		*(int16_t *)(h + 4) = frame;
		*(int32_t *)(h + 8) = angle;
		*(int16_t *)(h + 0x24) = 1;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl SprayTask(TaskNode *n)
	{
		SprayNode *s = (SprayNode *)n;
		SprayDraw(s->pos, s->scale, s->frame, s->angle);
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
		s->angle = (int16_t)(s->angle + s->spin);
		s->frame++;
		return s->frame < 9 ? 0 : TASK_END;
	}

	static void SplatDraw(const int16_t *pos, int16_t frame)
	{
		TransformCameraByShadowRotation(pos, 0x1000, -0x400);
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_Splat);
		*(int16_t *)(h + 4) = frame;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl SplatTask(TaskNode *n)
	{
		SplatNode *s = (SplatNode *)n;
		SplatDraw(s->pos, s->frame);
		s->frame++;
		return s->frame < 0x10 ? 0 : TASK_END;
	}

	static uint32_t __cdecl StreamTask(TaskNode *n)
	{
		StreamNode *d = (StreamNode *)n;
		PrimArg arg;
		BlockAt(&arg, d->pos);
		arg.depth = 0;
		arg.mode = 2;
		arg.morph = TexBase() + VERTS;
		if (prim::play((prim::Layout *)d->layout, PrimCallback, (int)&arg, 0) == 0) return TASK_END;
		d->pos[0] = (int16_t)(d->pos[0] + d->vel[0]);
		d->pos[1] = (int16_t)(d->pos[1] + d->vel[1]);
		d->pos[2] = (int16_t)(d->pos[2] + d->vel[2]);
		return 0;
	}

	static uint32_t __cdecl FallTask(TaskNode *n)
	{
		FallNode *d = (FallNode *)n;
		PrimArg arg;
		BlockAt(&arg, d->pos);
		arg.depth = 0;
		arg.mode = d->landed != 0 ? 1 : 2;
		arg.morph = TexBase() + VERTS;
		if (prim::play((prim::Layout *)d->layout, PrimCallback, (int)&arg, 0) == 0 && d->landed == 1) return TASK_END;
		if (d->landed == 0)
		{
			d->vel[1] = (int16_t)(d->vel[1] + 0x10);
			d->pos[0] = (int16_t)(d->pos[0] + d->vel[0]);
			d->pos[1] = (int16_t)(d->pos[1] + d->vel[1]);
			d->pos[2] = (int16_t)(d->pos[2] + d->vel[2]);
			if (d->pos[1] >= 0)
			{
				d->pos[1] = 0;
				d->landed = 1;
				DecodeModelPrimLayout(game_addr(MODEL_Splash), d->layout, 0xA8);
			}
		}
		return 0;
	}

	static int32_t FlashLevel(int32_t c)
	{
		if (c < 0x22) return (int32_t)mul32(c - 0x1E, 1800) / 4;
		if (c < 0x46) return 0x708;
		return (int32_t)mul32(0x52 - c, 1800) / 12;
	}

	static int32_t StreamS(int32_t k) { return (int32_t)(((uint64_t)(uint32_t)shl32(k, 12) * 0xAAAAAAABu) >> 35); }

	static void StreamPoint(const ShotNode *w, int32_t s, int32_t sine, int16_t *P)
	{
		BlendVectors(w->start, w->pos, 0x1000 - s, s, P);
		P[1] = (int16_t)(P[1] - (int16_t)(sine >> 3));
	}

	static int32_t StreamBasis(const int16_t *P, const int16_t *prev, Mat4x3 *M)
	{
		const int32_t v[3] = { (int32_t)P[0] - prev[0], (int32_t)prev[1] - P[1], (int32_t)prev[2] - P[2] - 100 };
		return BuildOrthonormalBasis(v, M);
	}

	static int16_t MeshScaleZ(int32_t s, int32_t sine)
	{
		const int32_t q = ComputeSin(0x400 - s / 4);
		return (int16_t)(sine - q / 2 + 0x800);
	}

	static void MeshRender(Mat4x3 *M, uint16_t u, uint32_t verts)
	{
		ComposeAffineTransform(&Camera(), M, M);
		GteSetTransVector(M);
		GteSetRotMatrix(M);
		GteSetTransVector(M);
		uint8_t *h = (uint8_t *)FieldAlloc(0x5C);
		*(uint8_t **)h = MESH();
		*(uint32_t *)(h + 4) = verts;
		*(uint16_t *)(h + 0x16) = 0;
		*(uint16_t *)(h + 0x14) = 0;
		*(uint16_t *)(h + 0x1A) = 0x100;
		*(uint16_t *)(h + 0x18) = 0x100;
		*(uint16_t *)(h + 0x1E) = 0;
		*(uint16_t *)(h + 0x1C) = 0;
		*(uint16_t *)(h + 0x20) = 0x80;
		*(uint16_t *)(h + 0x22) = 0x100;
		*(uint16_t *)(h + 0x10) = u;
		PacketCursor() = MeshDraw(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x5C);
	}

	static uint32_t __cdecl ShotTask(TaskNode *n)
	{
		ShotNode *w = (ShotNode *)n;
		const int16_t c0 = w->counter;
		if (c0 >= 0x1E) SetScreenFlash((uint32_t)FlashLevel(c0), 0xF0F0F0);
		int16_t P[4];
		P[3] = ShotResidue();
		Mat4x3 M;
		if ((uint32_t)(w->counter - 0x1E) < 8)
		{
			BonePoint(MOUTH(), P);
			RotXY(*(const int16_t *)(Caster() + 0xE), -0x200, &M);
			SprayNode *s = (SprayNode *)AddTaskToQueue(QSprays(), game_addr(ORIG_SprayTask));
			if (s)
			{
				s->angle = (int16_t)CrtRand();
				s->spin = (int16_t)((CrtRand() & 0x1FF) - 0x100);
				ff8fx::copy<(8)>(s->pos, P);
				s->scale = (int16_t)((CrtRand() & 0x7FF) + 0x800);
				const int32_t b = CrtRand();
				const int32_t a = (CrtRand() & 0x3FF) - 0x200;
				AngleVector(a, b, s->vel);
				s->vel[2] = (int16_t)-s->vel[2];
				MatrixMultiplyVector(&M, s->vel, s->vel);
				GteSetIR0((CrtRand() & 0x7F) + 0x80);
				GteLoadIR123(s->vel);
				GteGPF();
				GteStoreIR123(s->vel);
				s->frame = 0;
			}
			M.t[0] = P[0];
			M.t[1] = P[1];
			M.t[2] = P[2];
			ComposeAffineTransform(&Camera(), &M, &M);
			PrimArg arg;
			arg.m = M;
			arg.depth = -0x80;
			arg.mode = 0;
			arg.morph = TexBase() + VERTS;
			Play(w->mouth, &arg);
		}
		const int32_t k = w->counter - 0x1E;
		if ((uint32_t)k <= 0xC)
		{
			const int32_t s = StreamS(k);
			const int32_t sine = ComputeSin(s >> 1);
			if (k == 0)
			{
				BonePoint(MOUTH(), w->start);
				ff8fx::copy<(8)>(w->prev, w->start);
			}
			else
			{
				RotZ((k & 0xFF) << 8, &M);
				GteSetRotMatrix(&M);
				GteZeroTrans();
				WobbleMesh(MESH(), sine, (int16_t *)(TexBase() + VERTS));
				StreamPoint(w, s, sine, P);
				StreamBasis(P, w->prev, &M);
				StreamNode *d = (StreamNode *)AddTaskToQueue(QStreams(), game_addr(ORIG_StreamTask));
				ff8fx::copy<(8)>(w->prev, P);
				if (d)
				{
					const int32_t r = CrtRand();
					d->pos[0] = (int16_t)(ComputeCos(r) >> 6);
					d->pos[1] = (int16_t)(ComputeSin(r) >> 6);
					d->pos[2] = 0xFF;
					MatrixMultiplyVector(&M, d->pos, d->pos);
					d->zero = 0;
					d->pos[0] = (int16_t)(d->pos[0] + P[0]);
					d->pos[1] = (int16_t)(d->pos[1] + P[1]);
					d->pos[2] = (int16_t)(d->pos[2] + P[2]);
					d->vel[0] = (int16_t)(ComputeCos(r) >> 8);
					d->vel[1] = (int16_t)(ComputeSin(r) >> 8);
					d->vel[2] = 0x32;
					MatrixMultiplyVector(&M, d->vel, d->vel);
					DecodeModelPrimLayout(game_addr(MODEL_Stream), d->layout, 0xA8);
				}
				ScaleZ() = MeshScaleZ(s, sine);
				MulMatrix3(&M, SCALE_Z());
				M.t[0] = P[0];
				M.t[1] = P[1];
				M.t[2] = P[2];
				MeshRender(&M, (uint16_t)((k & 7) << 4), TexBase() + VERTS);
			}
		}
		const int32_t h = w->counter - 0x26;
		if ((uint32_t)h < 0x1A)
		{
			PrimArg arg;
			BlockAt(&arg, w->pos);
			arg.depth = -0x80;
			arg.mode = 1;
			arg.morph = TexBase() + VERTS;
			Play(w->hit, &arg);
			if ((uint32_t)h < 0x10 && !(h & 1))
			{
				FallNode *f = (FallNode *)AddTaskToQueue(QFalls(), game_addr(ORIG_FallTask));
				if (f)
				{
					ff8fx::copy<(8)>(f->pos, w->pos);
					f->landed = 0;
					const int32_t a = CrtRand();
					const int32_t sp = (CrtRand() & 0x1F) + 0x20;
					f->vel[0] = (int16_t)(mul32(ComputeSin(a), sp) >> 12);
					f->vel[2] = (int16_t)(mul32(ComputeCos(a), sp) >> 12);
					f->vel[1] = (int16_t)(-0x40 - (CrtRand() & 0x7F));
					DecodeModelPrimLayout(game_addr(MODEL_Fall), f->layout, 0x50);
				}
			}
			if ((uint32_t)h >= 4 && (uint32_t)h < 0x14)
			{
				SplatNode *s = (SplatNode *)AddTaskToQueue(QSplats(), game_addr(ORIG_SplatTask));
				if (s)
				{
					ff8fx::copy<(8)>(s->pos, w->pos);
					s->pos[0] = (int16_t)(s->pos[0] + ((CrtRand() & 0x1FF) - 0x100));
					s->pos[1] = (int16_t)(s->pos[1] + ((CrtRand() & 0x3FF) - 0x200));
					const int32_t r = CrtRand();
					s->frame = 0;
					s->pos[2] = (int16_t)(s->pos[2] + ((r & 0x1FF) - 0x100));
				}
			}
		}
		if ((uint32_t)(w->counter - 0x2C) < 0x26)
		{
			const int16_t p[4] = { w->pos[0], (int16_t)(w->pos[1] - 0xFA), w->pos[2], 0 };
			PrimArg arg;
			BlockAt(&arg, p);
			arg.depth = 0;
			arg.mode = 1;
			arg.morph = TexBase() + VERTS;
			Play(w->pool, &arg);
		}
		if (w->counter == 0x1E) BdPlaySE(SOUND_Shot(), 0, 0x80);
		if (w->counter == 0x3E) ApplyActionResultToTarget(Ctx()->actions->targets);
		if (w->counter >= 0x52)
		{
			SetScreenFlash(0, 0);
			return TASK_END;
		}
		w->counter++;
		return 0;
	}

	static void StartShot()
	{
		uint8_t *target = Entity(Ctx()->actions->targets[0]);
		InitTaskQueuePool(QShot(), (void *)(TexBase() + 0x39A0), 0x46C, 1);
		InitTaskQueuePool(QSprays(), (void *)(TexBase() + 0x38A0), 0x20, 8);
		InitTaskQueuePool(QStreams(), (void *)(TexBase() + 0x2C60), 0xC4, 0x10);
		InitTaskQueuePool(QSplats(), (void *)(TexBase() + 0x2BC0), 0x14, 8);
		InitTaskQueuePool(QFalls(), (void *)TexBase(), 0x2BC, 0x10);
		ShotNode *w = (ShotNode *)AddTaskToQueue(QShot(), game_addr(ORIG_ShotTask));
		Memset32(&w->counter, 0, 0x118);
		w->target = target;
		GetDefaultEffectPosition(target, w->pos);
		DecodeModelPrimLayout(game_addr(MODEL_Mouth), w->mouth, 0xC8);
		DecodeModelPrimLayout(game_addr(MODEL_Hit), w->hit, 0x1F4);
		DecodeModelPrimLayout(game_addr(MODEL_Pool), w->pool, 0x184);
		NormalizeMesh(MESH());
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *m = (MasterNode *)n;
		if (m->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			m->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			m->arena = 1;
		}
		if (m->counter == 1)
		{
			m->started = 1;
			StartShot();
		}
		int r;
		if (m->started)
		{
			r = ExecuteTaskQueue(QShot());
			ExecuteTaskQueue(QSprays());
			ExecuteTaskQueue(QStreams());
			ExecuteTaskQueue(QSplats());
			ExecuteTaskQueue(QFalls());
		}
		else r = (int)n;
		if (m->started && r == 0) return TASK_END;
		m->counter++;
		return 0;
	}
}

	void register_mag136_oil_shot()
	{
		register_port(oil136::ORIG_MasterTask, (void *)oil136::MasterTask, "136 MasterTask", 136);
		register_port(oil136::ORIG_ShotTask, (void *)oil136::ShotTask, "136 ShotTask", 136);
		register_port(oil136::ORIG_SprayTask, (void *)oil136::SprayTask, "136 SprayTask", 136);
		register_port(oil136::ORIG_StreamTask, (void *)oil136::StreamTask, "136 StreamTask", 136);
		register_port(oil136::ORIG_SplatTask, (void *)oil136::SplatTask, "136 SplatTask", 136);
		register_port(oil136::ORIG_FallTask, (void *)oil136::FallTask, "136 FallTask", 136);
	}
}

