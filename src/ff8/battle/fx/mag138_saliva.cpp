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

// Effect 138: Saliva

#include "mag_common.h"
#include "mag_ice_common.h"

namespace ff8fx
{
namespace saliva138
{
	using namespace eng;
	using namespace magc;
	using namespace icec;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_138_Saliva_CasterEntity)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_138_Saliva_MagicBufferBase)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_138_Saliva_CastContext)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_138_Saliva_PacketCursor)); }
	inline TaskQueue *QSplats() { return (TaskQueue *)game_addr(game::MAG_138_Saliva_SplatsQueue); }
	inline TaskQueue *QSprays() { return (TaskQueue *)game_addr(game::MAG_138_Saliva_SpraysQueue); }
	inline TaskQueue *QStrings() { return (TaskQueue *)game_addr(game::MAG_138_Saliva_StringsQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_138_Saliva_MainQueue); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_138_MasterTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_138_MainTask;
	static constexpr game::Id ORIG_StringTask = game::MAG_138_StringTask;
	static constexpr game::Id ORIG_SplatTask = game::MAG_138_SplatTask;
	static constexpr game::Id ORIG_SprayTask = game::MAG_138_SprayTask;
	static const uint32_t POINTS = 0x5B0;
	static const uint32_t VERTS = 0x11B0;
	static const uint32_t ARENA_A = 0x1510;
	static const uint32_t ARENA_B = 0x11510;
	static uint8_t *MESH() { return (uint8_t *)game_addr(game::unk_11DF764); }
	static constexpr game::Id MODEL_A = game::MAG_138_Saliva_ModelA;
	static constexpr game::Id MODEL_B = game::MAG_138_Saliva_ModelB;
	static constexpr game::Id MODEL_C = game::MAG_138_Saliva_ModelC;
	static constexpr game::Id SEQ_Spray = game::MAG_138_Saliva_SeqSpray;
	static constexpr game::Id SEQ_Splat = game::MAG_138_Saliva_SeqSplat;
	static const int16_t *POINT() { return (const int16_t *)game_addr(game::unk_11E3188); }
	static const void *SCALE_Z() { return (const void *)game_addr(game::unk_11E3138); }
	static int16_t &ScaleZ() { return *(int16_t *)game_addr(game::unk_11E3148); }
	static const int16_t *DRIP() { return (const int16_t *)game_addr(game::unk_11E3158); }
	static const int32_t *WOBBLE() { return (const int32_t *)game_addr(game::unk_11E3190); }
	static const uint8_t *DIST_TABLE() { return (const uint8_t *)game_addr(game::unk_11E5194); }
	static const void *SOUND_Spit() { return (const void *)game_addr(game::MAG_138_Saliva_SoundSpit); }

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAngles2(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MulMatrix3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline uint32_t RenderPrim2(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline int32_t BuildOrthonormalBasis(const int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline void RotY(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::Effect_RotationYMatrix))(angle, out); }
	inline void RotZ(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_065_RotationZMatrix))(angle, out); }
	inline void MatrixMultiplyVector(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void BlendVectors(const int16_t *a, const int16_t *b, int32_t wa, int32_t wb, int16_t *out) { fn<void (__cdecl *)(const int16_t *, const int16_t *, int32_t, int32_t, int16_t *)>(game_addr(game::VecLerp))(a, b, wa, wb, out); }
	inline void GteZeroTrans() { fn<void (__cdecl *)()>(game_addr(game::GTE_ZeroTranslation))(); }
	inline void GteReadSZ3Quarter(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadSZ3Quarter))(dst); }
	inline uint32_t GteSZ3() { return *(const uint32_t *)game_addr(game::GTE_SZ3); }

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t started;
		uint32_t arena;
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint8_t *target;
		uint8_t pad14[8];
		int16_t pos[4];
		uint8_t pad24[8];
		uint8_t layA[0xA8];
		uint8_t layB[0x194];
		uint8_t layC[0x118];
	};
	struct Point
	{
		int16_t pos[3];
		int16_t width;
		int16_t sx, sy;
		int16_t otz;
		int16_t idx;
		int16_t sx2, sy2;
		uint8_t pad14[4];
		int16_t fade;
		int16_t zero;
		Point *next;
	};
	struct StringNode
	{
		TaskNode hdr;
		int16_t start[3];
		int16_t counter;
		int16_t target[3];
		int16_t amp;
		int16_t prev[4];
		int16_t count;
		int16_t pad26;
		Point *head;
		Point *tail;
	};
	struct SplatNode
	{
		TaskNode hdr;
		int16_t pos[3];
		int16_t frame;
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
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(MainNode) == 0x380 && sizeof(StringNode) == 0x30, "Saliva nodes");
	static_assert(sizeof(Point) == 0x20 && sizeof(SplatNode) == 0x14 && sizeof(SprayNode) == 0x20, "Saliva nodes");
	static_assert(sizeof(PrimArg) == 0x28, "Saliva prim block");

	static uint8_t *&Cursor(uint8_t *h, int off) { return *(uint8_t **)(h + off); }
	static bool OffX(int16_t x) { return x < 0 || x > 0xA00; }
	static bool OffY(int16_t y) { return y < 0 || y > 0x6C0; }

	static void ScrollU(uint8_t *p, const int *off, uint16_t add, uint8_t wrap)
	{
		uint32_t v[4], o = 0;
		for (int k = 0; k < 4; k++)
		{
			v[k] = p[off[k]] + (uint32_t)add;
			o |= v[k];
		}
		for (int k = 0; k < 4; k++) p[off[k]] = (uint8_t)(o > 0xFF ? v[k] - wrap : v[k]);
	}

	static uint8_t *Windowed(const uint8_t *h, uint32_t otp, uint8_t *main, uint8_t *p)
	{
		uint8_t *t1 = p;
		*(uint32_t *)t1 = 0x02000000;
		*(uint32_t *)(t1 + 4) = TextureWindow(h + 0x14);
		*(uint32_t *)(t1 + 8) = 0;
		InsertPrimAutoDepth(otp, t1);
		InsertPrimAutoDepth(otp, main);
		uint8_t *t2 = p + 0xC;
		*(uint32_t *)t2 = 0x02000000;
		*(uint32_t *)(t2 + 4) = TextureWindow(h + 0x1C);
		*(uint32_t *)(t2 + 8) = 0;
		InsertPrimAutoDepth(otp, t2);
		return p + 0x18;
	}

	static uint32_t TriClip(const uint8_t *a, const uint8_t *b, const uint8_t *c)
	{
		uint32_t clip = 0;
		if (OffX(*(const int16_t *)a)) clip = 1;
		if (OffX(*(const int16_t *)b)) clip |= 2;
		if (OffX(*(const int16_t *)c)) clip |= 4;
		if (OffY(*(const int16_t *)(a + 2))) clip |= 0x10;
		if (OffY(*(const int16_t *)(b + 2))) clip |= 0x20;
		if (OffY(*(const int16_t *)(c + 2))) clip |= 0x40;
		return clip;
	}
	static bool QuadClipped(uint32_t clip3, const uint8_t *x3)
	{
		uint32_t clip = clip3;
		if (OffX(*(const int16_t *)x3)) clip |= 8;
		if (OffY(*(const int16_t *)(x3 + 2))) clip |= 0x80;
		return (clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0;
	}

	static uint32_t FlatTexQuads(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *rec = Cursor(h, 0x24);
		const int32_t count = *(const int32_t *)rec;
		rec += 4;
		uint8_t *verts = Cursor(h, 4);
		Cursor(h, 0x24) = rec;
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
			GteReadFLAG(h + 0x34);
			if (*(const uint32_t *)(h + 0x34) & 0x60000) continue;
			GteReadSXY012(p + 8, p + 0x10, p + 0x18);
			GteLoadV0(verts + 4 * *(const uint16_t *)(rec + 0xA));
			GteRTPS();
			const uint32_t clip = TriClip(p + 8, p + 0x10, p + 0x18);
			GteReadSXY2(p + 0x20);
			GteAVSZ4();
			if (QuadClipped(clip, p + 0x20)) continue;
			GteReadOTZReg(h + 0x30);
			static const int us[4] = { 0xC, 0x14, 0x1C, 0x24 };
			ScrollU(p, us, *(const uint16_t *)(h + 0x10), h[0x20]);
			const uint32_t otp = ot + 4 * (*(const int32_t *)(h + 0x30) >> shift);
			p = Windowed(h, otp, p, p + 0x28);
		}
		Cursor(h, 0x24) = rec;
		return (uint32_t)p;
	}

	static uint32_t GouraudTexQuads(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
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
			const uint32_t clip = TriClip(p + 8, p + 0x14, p + 0x20);
			GteReadSXY2(p + 0x2C);
			GteAVSZ4();
			if (QuadClipped(clip, p + 0x2C)) continue;
			GteReadOTZReg(h + 0x30);
			*(uint32_t *)(p + 0x10) = *(const uint32_t *)(rec + 0x18);
			*(uint32_t *)(p + 0x1C) = *(const uint32_t *)(rec + 0x1C);
			*(uint32_t *)(p + 0x28) = *(const uint32_t *)(rec + 0x20);
			static const int us[4] = { 0xC, 0x18, 0x24, 0x30 };
			ScrollU(p, us, *(const uint16_t *)(h + 0x10), h[0x20]);
			const uint32_t otp = ot + 4 * (*(const int32_t *)(h + 0x30) >> shift);
			p = Windowed(h, otp, p, p + 0x34);
		}
		Cursor(h, 0x24) = rec;
		return (uint32_t)p;
	}

	static uint32_t MeshDraw(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *m = Cursor(h, 0);
		Cursor(h, 0x24) = m + *(const uint32_t *)m + 0xC;
		const uint32_t r = FlatTexQuads(h, ot, shift, cursor);
		Cursor(h, 0x24) += 0xC;
		return GouraudTexQuads(h, ot, shift, r);
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
		PacketCursor() = RenderPrim2(h, RenderOT(), 2, PacketCursor());
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

	static void AngleVector(int32_t a, int32_t b, int16_t *out)
	{
		out[2] = (int16_t)ComputeCos(a);
		const int32_t s = -ComputeSin(a);
		out[0] = (int16_t)(mul32(ComputeSin(b), s) >> 12);
		out[1] = (int16_t)((int32_t)(0u - (uint32_t)mul32(ComputeCos(b), s)) >> 12);
	}

	static void BonePoint(const int16_t *v, int16_t *out)
	{
		uint8_t *c = Caster();
		Mat4x3 m = {};
		ComposeAffineTransform((const Mat4x3 *)(c + 0x40), (const Mat4x3 *)(**(uint8_t ***)(c + 0x64) + 0x20 + (int32_t)v[3] * 0x30), &m);
		GteSetRotMatrixCtrl(&m);
		GteSetTransVectorCtrl(&m);
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(out);
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
			out[1] = (int16_t)(out[1] - (int16_t)(mul32(WOBBLE()[w], amp) >> 0x10));
			out += 4;
			v += 4;
		} while (--count);
	}

	static int32_t StringT(int32_t x) { return shl32(x, 11) / 10; }

	static void StringPoint(const StringNode *s, int32_t t, int32_t wave, int16_t *P)
	{
		BlendVectors(s->start, s->target, 0x1000 - t, t, P);
		P[1] = (int16_t)(P[1] - (int16_t)(wave >> 14));
	}

	static int32_t StringBasis(const int16_t *P, const int16_t *prev, Mat4x3 *M)
	{
		const int32_t v[3] = { (int32_t)P[0] - prev[0], (int32_t)prev[1] - P[1], (int32_t)prev[2] - P[2] };
		return BuildOrthonormalBasis(v, M);
	}

	static void MeshRender(Mat4x3 *M, uint16_t u, uint32_t verts)
	{
		ComposeAffineTransform(&Camera(), M, M);
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

	static Point *AllocPoint(int16_t w, const int16_t *pos)
	{
		Point *p = (Point *)(TexBase() + POINTS);
		int n = 0x60;
		while (p->width != -1)
		{
			p++;
			if (--n == 0) return nullptr;
		}
		ff8fx::copy<(8)>(p->pos, pos);
		p->width = w;
		p->fade = 0x80;
		p->zero = 0;
		p->next = nullptr;
		return p;
	}

	static void RibbonEdges(Point *p, const Point *n, const int16_t *prev, int16_t *dir)
	{
		if (p->otz < 0) return;
		int32_t ox = 0, oy = 0;
		if (n)
		{
			if (n->otz < 0) return;
			const int32_t dx = ScreenPx(n->sx) - ScreenPx(p->sx);
			const int32_t ndy = ScreenPx(p->sy) - ScreenPx(n->sy);
			int32_t adx = dx < 0 ? -dx : dx, ady = ndy < 0 ? -ndy : ndy;
			if (adx >= 0x80) adx = 0x7F;
			if (ady >= 0x80) ady = 0x7F;
			const int32_t len = DIST_TABLE()[ady * 128 + adx];
			int32_t px, py;
			if (prev)
			{
				if (len == 0)
				{
					dir[0] = prev[0];
					dir[1] = prev[1];
					px = prev[0];
					py = prev[1];
				}
				else
				{
					const int32_t cx = shl32(ndy, 12) / len, cy = shl32(dx, 12) / len;
					int32_t sx = cx + prev[0], sy = cy + prev[1];
					dir[0] = (int16_t)cx;
					dir[1] = (int16_t)cy;
					if (sx == 0 && sy == 0)
					{
						sx = cx * 2;
						sy = cy * 2;
					}
					const int32_t sq = (int32_t)((uint32_t)mul32(sx, sx) + (uint32_t)mul32(sy, sy));
					const int32_t k = 0x4000 - (sq >> 13);
					px = mul32(k, sx) >> 13;
					py = mul32(k, sy) >> 13;
				}
			}
			else
			{
				if (len == 0)
				{
					p->otz = -1;
					return;
				}
				px = shl32(ndy, 12) / len;
				py = shl32(dx, 12) / len;
				dir[0] = (int16_t)px;
				dir[1] = (int16_t)py;
			}
			const int32_t w = shl32(n->width, 5) / (p->otz + 0x1000);
			ox = ScreenEighths(mul32(w, px) >> 12);
			oy = ScreenEighths(mul32(w, py) >> 12);
		}
		const int16_t x = p->sx, y = p->sy;
		p->sx = (int16_t)(x - ox);
		p->sx2 = (int16_t)(x + ox);
		p->sy2 = (int16_t)(y + oy);
		p->sy = (int16_t)(y - oy);
	}

	static void DrawRibbon(Point *head, int32_t count, uint32_t residue)
	{
		int16_t dir[2] = { (int16_t)(residue & 0xFFFF), (int16_t)(residue >> 16) };
		RibbonEdges(head, head->next, nullptr, dir);
		Point *p = head->next;
		for (int32_t i = count - 2; i != 0; i--)
		{
			RibbonEdges(p, p->next, dir, dir);
			p = p->next;
		}
		RibbonEdges(p, nullptr, dir, nullptr);
		Point *a = head;
		for (int32_t i = count - 1; i != 0; i--)
		{
			Point *b = a->next;
			if (a->otz > 0 && b->otz > 0)
			{
				uint32_t *pk = (uint32_t *)PacketCursor();
				PacketCursor() += 0x34;
				pk[2] = *(const uint32_t *)&a->sx;
				pk[5] = *(const uint32_t *)&a->sx2;
				pk[8] = *(const uint32_t *)&b->sx;
				pk[11] = *(const uint32_t *)&b->sx2;
				uint32_t ca = (uint32_t)(int32_t)a->fade;
				ca = (((ca << 8) | ca) << 8) | ca;
				pk[4] = ca;
				pk[1] = ca;
				uint32_t cb = (uint32_t)(int32_t)b->fade;
				pk[3] = 0x3C14C008;
				cb = (((cb << 8) | cb) << 8) | cb;
				pk[6] = 0x00BAC038;
				*(uint16_t *)((uint8_t *)pk + 0x24) = 0xC008;
				pk[10] = cb;
				pk[7] = cb;
				*(uint16_t *)((uint8_t *)pk + 0x30) = 0xC038;
				pk[0] = 0x0C000000;
				((uint8_t *)pk)[7] = 0x3E;
				InsertPrimAutoDepth(RenderOT() + 4 * ((a->otz + b->otz) >> 3), pk);
			}
			a = b;
		}
	}

	static void StringDraw(Point *head, int32_t count)
	{
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		Point *p = head;
		do
		{
			GteLoadV0(p);
			GteRTPS();
			GteReadSXY2(&p->sx);
			GteReadSZ3Quarter(&p->otz);
			p = p->next;
		} while (p);
		DrawRibbon(head, count, GteSZ3());
	}

	static int16_t StringResidue()
	{
		return 0x0252;
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
namespace saliva138
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
		return s->frame < 0xA ? 0 : TASK_END;
	}

	static void SplatDraw(const int16_t *pos, int16_t frame)
	{
		TransformCameraByShadowRotation(pos, 0xC00, -0x300);
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
		return s->frame < 0xD ? 0 : TASK_END;
	}

	static uint32_t __cdecl StringTask(TaskNode *n)
	{
		StringNode *s = (StringNode *)n;
		if (s->counter == 0)
		{
			BonePoint(POINT(), s->start);
			const int16_t c = s->counter;
			s->counter++;
			ff8fx::copy<(6)>(s->prev, s->start);
			s->prev[3] = c;
			return 0;
		}
		if (s->counter < 0xA)
		{
			for (int32_t j = 0; j < 2; j++)
			{
				const int32_t t = StringT(j + s->counter * 2);
				const int32_t sine = ComputeSin(t >> 1);
				Mat4x3 M;
				RotZ((int16_t)((s->counter & 0xFF) << 8), &M);
				GteSetRotMatrix(&M);
				GteZeroTrans();
				const int32_t wave = mul32(s->amp, sine);
				WobbleMesh(MESH(), wave >> 13, (int16_t *)(TexBase() + VERTS));
				int16_t P[4];
				P[3] = StringResidue();
				StringPoint(s, t, wave, P);
				StringBasis(P, s->prev, &M);
				ScaleZ() = (int16_t)(sine + 0x800);
				MulMatrix3(&M, SCALE_Z());
				M.t[0] = P[0];
				M.t[1] = P[1];
				M.t[2] = P[2];
				if (j == 1)
				{
					MeshRender(&M, (uint16_t)((s->counter & 7) << 4), TexBase() + VERTS);
				}
				ff8fx::copy<(8)>(s->prev, P);
				Point *p = AllocPoint(0xC8, P);
				if (s->count == 0)
				{
					s->tail = p;
					s->head = p;
				}
				else
				{
					s->tail->next = p;
					s->tail = p;
				}
				p->idx = s->count;
				s->count++;
			}
		}
		else
		{
			Point *p = s->head;
			do
			{
				p->pos[1] = (int16_t)(p->pos[1] + DRIP()[p->idx]);
				p = p->next;
			} while (p);
		}
		if (s->count >= 2)
		{
			Point *p = s->head;
			do
			{
				p->fade = (int16_t)(p->fade - 4);
				if (p->fade < 0) p->fade = 0;
				p = p->next;
			} while (p);
			p = s->head;
			if (p->fade <= 0x20)
			{
				for (;;)
				{
					Point *q = p->next;
					if (q->fade > 0x20) break;
					p->width = -1;
					s->count--;
					s->head = q;
					if (s->count < 2) return TASK_END;
					p = q;
					if (p->fade > 0x20) break;
				}
			}
		}
		if (s->count >= 3)
		{
			StringDraw(s->head, s->count);
		}
		s->counter++;
		return 0;
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *w = (MainNode *)n;
		int16_t M4[4];
		M4[3] = 0;
		PrimArg arg;
		arg.m.pad = 0;
		const int32_t e = w->counter - 0x17;
		if ((uint32_t)e < 0xE)
		{
			BonePoint(POINT(), M4);
			RotY((int16_t)-*(const int16_t *)(Caster() + 0xE), &arg.m);
			if ((uint32_t)e < 7)
			{
				for (int i = 2; i; i--)
				{
					SprayNode *s = (SprayNode *)AddTaskToQueue(QSprays(), game_addr(ORIG_SprayTask));
					if (!s) continue;
					s->angle = (int16_t)CrtRand();
					s->spin = (int16_t)((CrtRand() & 0x1FF) - 0x100);
					ff8fx::copy<(8)>(s->pos, M4);
					s->scale = (int16_t)((CrtRand() & 0x3FF) + 0x400);
					const int32_t b = CrtRand();
					const int32_t a = (CrtRand() & 0x3FF) - 0x200;
					AngleVector(a, b, s->vel);
					s->vel[2] = (int16_t)-s->vel[2];
					MatrixMultiplyVector(&arg.m, s->vel, s->vel);
					GteSetIR0((CrtRand() & 0x7F) + 0x80);
					GteLoadIR123(s->vel);
					GteGPF();
					GteStoreIR123(s->vel);
					s->frame = 0;
				}
			}
			arg.m.t[0] = M4[0];
			arg.m.t[1] = M4[1];
			arg.m.t[2] = M4[2];
			ComposeAffineTransform(&Camera(), &arg.m, &arg.m);
			arg.morph = TexBase() + VERTS;
			arg.depth = -0x80;
			Play(w->layA, &arg);
			Play(w->layB, &arg);
			if ((uint32_t)e < 3)
			{
				StringNode *s = (StringNode *)AddTaskToQueue(QStrings(), game_addr(ORIG_StringTask));
				s->counter = 0;
				ff8fx::copy<(8)>(s->target, w->pos);
				const int32_t r = CrtRand();
				s->count = 0;
				s->amp = (int16_t)((r & 0x7FF) + 0x800);
			}
		}
		const int32_t h = w->counter - 0x1D;
		if ((uint32_t)h < 0x1A)
		{
			BlockAt(&arg, w->pos);
			arg.morph = TexBase() + VERTS;
			arg.depth = 0;
			Play(w->layC, &arg);
			if ((uint32_t)h >= 4 && (uint32_t)h < 0xC && !(h & 1))
			{
				SplatNode *s = (SplatNode *)AddTaskToQueue(QSplats(), game_addr(ORIG_SplatTask));
				if (s)
				{
					ff8fx::copy<(8)>(s->pos, w->pos);
					s->pos[0] = (int16_t)(s->pos[0] + ((CrtRand() & 0x1FF) - 0x100));
					s->pos[1] = (int16_t)(s->pos[1] + ((CrtRand() & 0x1FF) - 0x12C));
					const int32_t r = CrtRand();
					s->frame = 0;
					s->pos[2] = (int16_t)(s->pos[2] + ((r & 0x1FF) - 0x100));
				}
			}
		}
		if (w->counter == 0x19) BdPlaySE(SOUND_Spit(), 0, 0x80);
		if (w->counter == 0x28) ApplyActionResultToTarget(Ctx()->actions->targets);
		if (w->counter >= 0x37) return TASK_END;
		w->counter++;
		return 0;
	}

	static void StartMain(MasterNode *m)
	{
		uint8_t *target = Entity(Ctx()->actions->targets[0]);
		m->started = 1;
		InitTaskQueuePool(QMain(), (void *)(TexBase() + 0x230), 0x380, 1);
		InitTaskQueuePool(QStrings(), (void *)(TexBase() + 0x1A0), 0x30, 3);
		InitTaskQueuePool(QSprays(), (void *)(TexBase() + 0xA0), 0x20, 8);
		InitTaskQueuePool(QSplats(), (void *)TexBase(), 0x14, 8);
		MainNode *w = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
		Memset32(&w->counter, 0, 0xDD);
		w->target = target;
		GetEffectSpawnPosition(target, 0xF1, 0, w->pos);
		DecodeModelPrimLayout(game_addr(MODEL_A), w->layA, 0xA8);
		DecodeModelPrimLayout(game_addr(MODEL_B), w->layB, 0x194);
		DecodeModelPrimLayout(game_addr(MODEL_C), w->layC, 0x118);
		NormalizeMesh(MESH());
		Point *p = (Point *)(TexBase() + POINTS);
		for (int i = 0x60; i; i--, p++) p->width = -1;
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
		if (m->counter == 1) StartMain(m);
		int r_main, r_splat;
		if (m->started)
		{
			r_main = ExecuteTaskQueue(QMain());
			ExecuteTaskQueue(QStrings());
			ExecuteTaskQueue(QSprays());
			r_splat = ExecuteTaskQueue(QSplats());
		}
		else r_main = r_splat = (int)n;
		if (m->started && r_main == 0 && r_splat == 0) return TASK_END;
		m->counter++;
		return 0;
	}
}

	void register_mag138_saliva()
	{
		register_port(saliva138::ORIG_MasterTask, (void *)saliva138::MasterTask, "138 MasterTask", 138);
		register_port(saliva138::ORIG_MainTask, (void *)saliva138::MainTask, "138 MainTask", 138);
		register_port(saliva138::ORIG_StringTask, (void *)saliva138::StringTask, "138 StringTask", 138);
		register_port(saliva138::ORIG_SprayTask, (void *)saliva138::SprayTask, "138 SprayTask", 138);
		register_port(saliva138::ORIG_SplatTask, (void *)saliva138::SplatTask, "138 SplatTask", 138);
	}
}

