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

#include "fx_port.h"
#include "mag_common.h"
#include "mag_ice_common.h"

namespace ff8fx
{
namespace brr289
{
	using namespace eng;
	using namespace magc;
	using namespace icec;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_mag289_CastContext)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g_mag289_CasterEntity)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_mag289_PacketCursor)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g_mag289_MagicBuffer)); }
	inline TaskQueue *QSprites() { return (TaskQueue *)game_addr(game::g_mag289_SpriteQueue); }
	inline TaskQueue *QSplashes() { return (TaskQueue *)game_addr(game::g_mag289_SplashQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::g_mag289_MainQueue); }

	static constexpr game::Id ORIG_MasterTask = game::MAG_289_Brrawghh_MasterTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_289_Brrawghh_MainTask;
	static constexpr game::Id ORIG_SpriteTask = game::MAG_289_Brrawghh_SpriteTask;
	static constexpr game::Id ORIG_SplashTask = game::MAG_289_Brrawghh_SplashTask;
	static const uint32_t VERTS = 0x1520;
	static const uint32_t ARENA_A = 0x1A40;
	static const uint32_t ARENA_B = 0x11A40;
	static uint8_t *TUBE_A() { return (uint8_t *)game_addr(game::mag289_TubeMeshA); }
	static uint8_t *TUBE_B() { return (uint8_t *)game_addr(game::mag289_TubeMeshB); }
	static constexpr game::Id LAYOUT_Target = game::mag289_LayoutTarget;
	static constexpr game::Id LAYOUT_Splash = game::mag289_LayoutSplash;
	static constexpr game::Id SEQ_Drip = game::mag289_SeqDrip;
	static constexpr game::Id SEQ_Splat = game::mag289_SeqSplat;
	static constexpr game::Id SEQ_Spit = game::mag289_SeqSpit;
	static const int16_t *POINT() { return (const int16_t *)game_addr(game::mag289_BonePoint); }
	static const int16_t *WAVE() { return (const int16_t *)game_addr(game::mag289_WaveTable); }
	static const void *SOUND_Roar() { return (const void *)game_addr(game::mag289_SoundRoar); }

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
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }

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
		int16_t pad0E;
		uint8_t *target;
		int16_t pos[4];
		uint8_t layout[0x184];
	};
	struct SpriteNode
	{
		TaskNode hdr;
		uint32_t seq;
		int32_t scale;
		int16_t pos[3];
		int16_t frame;
		int16_t vel[3];
		int16_t frames;
	};
	struct SplashNode
	{
		TaskNode hdr;
		int16_t pos[4];
		uint8_t layout[0x40];
	};
	struct PrimArg
	{
		Mat4x3 m;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(MainNode) == 0x1A0, "Brrawghh nodes");
	static_assert(sizeof(SpriteNode) == 0x24 && sizeof(SplashNode) == 0x54, "Brrawghh nodes");
	static_assert(sizeof(PrimArg) == 0x28, "Brrawghh prim block");

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
		*(uint32_t *)(t1 + 4) = TextureWindow(h + 0x24);
		*(uint32_t *)(t1 + 8) = 0;
		InsertPrimAutoDepth(otp, t1);
		InsertPrimAutoDepth(otp, main);
		uint8_t *t2 = p + 0xC;
		*(uint32_t *)t2 = 0x02000000;
		*(uint32_t *)(t2 + 4) = TextureWindow(h + 0x2C);
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

	static bool FirstVertex(uint8_t *h, const uint8_t *verts, const uint8_t *rec)
	{
		const uint32_t i = *(const uint16_t *)(rec + 4);
		*(uint32_t *)(h + 0x48) = *(const uint32_t *)(verts + 4 * i);
		*(uint32_t *)(h + 0x4C) = *(const uint32_t *)(verts + 4 * i + 4);
		return *(const int16_t *)(h + 0x4E) < 0x1000;
	}

	static uint32_t FlatTexQuads(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *rec = Cursor(h, 0x34);
		const int32_t count = *(const int32_t *)rec;
		rec += 4;
		const uint8_t *verts = Cursor(h, 4);
		Cursor(h, 0x34) = rec;
		uint8_t *p = (uint8_t *)cursor;
		if (count <= 0) return cursor;
		for (int32_t left = count; left; left--, rec += 0x18)
		{
			if (!FirstVertex(h, verts, rec)) continue;
			GteLoadV012(h + 0x48, verts + 4 * *(const uint16_t *)(rec + 6), verts + 4 * *(const uint16_t *)(rec + 8));
			GteRTPT();
			*(uint32_t *)p = 0x09000000;
			*(uint32_t *)(p + 4) = *(const uint32_t *)rec;
			*(uint32_t *)(p + 0xC) = *(const uint32_t *)(rec + 0xC);
			*(uint32_t *)(p + 0x14) = *(const uint32_t *)(rec + 0x10);
			const uint32_t uv = *(const uint32_t *)(rec + 0x14);
			*(uint32_t *)(p + 0x1C) = uv;
			*(uint32_t *)(p + 0x24) = uv >> 16;
			GteReadFLAG(h + 0x44);
			if (*(const uint32_t *)(h + 0x44) & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(h + 0x38);
			if (*(const int32_t *)(h + 0x38) < 0 && !(h[0x1C] & 0x10)) continue;
			GteReadSXY012(p + 8, p + 0x10, p + 0x18);
			GteLoadV0(verts + 4 * *(const uint16_t *)(rec + 0xA));
			GteRTPS();
			const uint32_t clip = TriClip(p + 8, p + 0x10, p + 0x18);
			GteReadSXY2(p + 0x20);
			GteAVSZ4();
			if (QuadClipped(clip, p + 0x20)) continue;
			GteReadOTZReg(h + 0x40);
			const int16_t w = *(const int16_t *)(h + 0x4E);
			if (w != 0)
			{
				GteLoadRGBC(p + 4);
				GteSetIR0(w);
				GteDPCS();
				GteStoreRGB2(p + 4);
			}
			const uint32_t otp = ot + 4 * (*(const int32_t *)(h + 0x40) >> shift);
			if (*(const uint16_t *)(h + 0x20) != 0)
			{
				static const int us[4] = { 0xC, 0x14, 0x1C, 0x24 };
				ScrollU(p, us, *(const uint16_t *)(h + 0x20), h[0x30]);
				p = Windowed(h, otp, p, p + 0x28);
			}
			else
			{
				InsertPrimAutoDepth(otp, p);
				p += 0x28;
			}
		}
		Cursor(h, 0x34) = rec;
		return (uint32_t)p;
	}

	static uint32_t GouraudTexQuads(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *rec = Cursor(h, 0x34);
		const int32_t count = *(const int32_t *)rec;
		rec += 4;
		const uint8_t *verts = Cursor(h, 4);
		Cursor(h, 0x34) = rec;
		uint8_t *p = (uint8_t *)cursor;
		if (count <= 0) return cursor;
		for (int32_t left = count; left; left--, rec += 0x24)
		{
			if (!FirstVertex(h, verts, rec)) continue;
			GteLoadV012(h + 0x48, verts + 4 * *(const uint16_t *)(rec + 6), verts + 4 * *(const uint16_t *)(rec + 8));
			GteRTPT();
			*(uint32_t *)p = 0x0C000000;
			*(uint32_t *)(p + 4) = *(const uint32_t *)rec;
			if (h[0x1C] & 2) *(uint32_t *)(p + 4) |= 0x02000000;
			if (h[0x1C] & 8) *(uint32_t *)(p + 4) &= 0xFDFFFFFF;
			*(uint32_t *)(p + 0xC) = *(const uint32_t *)(rec + 0xC);
			*(uint32_t *)(p + 0x18) = *(const uint32_t *)(rec + 0x10);
			const uint32_t uv = *(const uint32_t *)(rec + 0x14);
			*(uint32_t *)(p + 0x24) = uv;
			*(uint32_t *)(p + 0x30) = uv >> 16;
			GteReadFLAG(h + 0x44);
			if (*(const uint32_t *)(h + 0x44) & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(h + 0x38);
			if (*(const int32_t *)(h + 0x38) < 0 && !(h[0x1C] & 0x20)) continue;
			GteReadSXY012(p + 8, p + 0x14, p + 0x20);
			GteLoadV0(verts + 4 * *(const uint16_t *)(rec + 0xA));
			GteRTPS();
			const uint32_t clip = TriClip(p + 8, p + 0x14, p + 0x20);
			GteReadSXY2(p + 0x2C);
			GteAVSZ4();
			if (QuadClipped(clip, p + 0x2C)) continue;
			GteReadOTZReg(h + 0x40);
			const int16_t w = *(const int16_t *)(h + 0x4E);
			if (w != 0)
			{
				GteLoadRGB012(rec + 0x18, rec + 0x1C, rec + 0x20);
				GteSetIR0(w);
				GteDPCT();
				GteStoreRGB012(p + 0x10, p + 0x1C, p + 0x28);
				GteLoadRGBC(p + 4);
				GteDPCS();
				GteStoreRGB2(p + 4);
			}
			else
			{
				*(uint32_t *)(p + 0x10) = *(const uint32_t *)(rec + 0x18);
				*(uint32_t *)(p + 0x1C) = *(const uint32_t *)(rec + 0x1C);
				*(uint32_t *)(p + 0x28) = *(const uint32_t *)(rec + 0x20);
			}
			const uint32_t otp = ot + 4 * (*(const int32_t *)(h + 0x40) >> shift);
			if (*(const uint16_t *)(h + 0x20) != 0)
			{
				static const int us[4] = { 0xC, 0x18, 0x24, 0x30 };
				ScrollU(p, us, *(const uint16_t *)(h + 0x20), h[0x30]);
				p = Windowed(h, otp, p, p + 0x34);
			}
			else
			{
				InsertPrimAutoDepth(otp, p);
				p += 0x34;
			}
		}
		Cursor(h, 0x34) = rec;
		return (uint32_t)p;
	}

	static uint32_t TubeRender(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *m = Cursor(h, 0);
		Cursor(h, 0x34) = m + *(const uint32_t *)m;
		GteSetFarColor(h[8], h[9], h[0xA]);
		Cursor(h, 0x34) += 0xC;
		const uint32_t r = FlatTexQuads(h, ot, shift, cursor);
		Cursor(h, 0x34) += 0xC;
		return GouraudTexQuads(h, ot, shift, r);
	}

	static void TubeBend(const uint8_t *model, int32_t angle, int32_t amp_a, int32_t amp_b, int32_t wofs, int32_t scale, int16_t *out)
	{
		uint32_t count = *(const uint32_t *)(model + 4);
		const int16_t *v = (const int16_t *)(model + 8);
		Mat4x3 R = {};
		RotZ(angle, &R);
		if (scale != 0x1000)
		{
			R.m[0][0] = (int16_t)(mul32(R.m[0][0], scale) >> 12);
			R.m[1][0] = (int16_t)(mul32(R.m[1][0], scale) >> 12);
			R.m[0][1] = (int16_t)(mul32(R.m[0][1], scale) >> 12);
			R.m[1][1] = (int16_t)(mul32(R.m[1][1], scale) >> 12);
		}
		GteSetRotMatrix(&R);
		do
		{
			GteLoadV0(v);
			GteMVMVA_RotV0();
			int32_t d = (int32_t)((uint32_t)(int32_t)v[3] - (uint32_t)wofs);
			if (d < 0) d = -d;
			if (d > 0x1000) d = 0x1000;
			out[3] = (int16_t)d;
			const int32_t w = v[3];
			const int32_t b = WAVE()[w];
			const int32_t a = WAVE()[w >> 1];
			v += 4;
			GteStoreIR123(out);
			const int32_t y = (int32_t)((uint32_t)mul32(a, amp_a) + (uint32_t)mul32(b, amp_b)) >> 12;
			out[1] = (int16_t)(out[1] - (int16_t)y);
			out += 4;
		} while (--count);
	}

	static void TubeDraw(const Mat4x3 *m, const uint8_t *model, int32_t angle, int32_t amp_a, int32_t amp_b, int32_t wofs, int32_t uadd, int32_t scale, uint32_t verts)
	{
		TubeBend(model, angle, amp_a, amp_b, wofs, scale, (int16_t *)verts);
		uint8_t *h = AllocHeader(0x6C);
		*(uint16_t *)(h + 0x26) = 0;
		*(uint16_t *)(h + 0x24) = 0;
		*(uint16_t *)(h + 0x2A) = 0x100;
		*(uint16_t *)(h + 0x28) = 0x100;
		*(uint16_t *)(h + 0x2E) = 0;
		*(uint16_t *)(h + 0x2C) = 0;
		*(uint16_t *)(h + 0x30) = 0x80;
		*(uint16_t *)(h + 0x32) = 0x100;
		*(uint16_t *)(h + 0x20) = (uint16_t)(uadd & 0x7F);
		*(const uint8_t **)h = model;
		*(uint32_t *)(h + 4) = verts;
		*(uint32_t *)(h + 8) = 0;
		GteSetRotMatrix(m);
		GteSetTransVector(m);
		PacketCursor() = TubeRender(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x6C);
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

	static void NegateZ(uint8_t *model)
	{
		uint32_t count = *(const uint32_t *)(model + 4);
		uint8_t *p = model + 0xE;
		do
		{
			*(int16_t *)p = (int16_t)-*(const int16_t *)(p - 2);
			p += 8;
		} while (--count);
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

	static int32_t TubeT(int32_t c) { return shl32(c, 12) / 22; }
	static void TubeArgs(int32_t c, int k, int32_t *x)
	{
		const int32_t t = TubeT(c);
		const int32_t uadd = shl32(-c, 3);
		if (k == 0)
		{
			x[0] = shl32(c, 4);
			x[1] = ComputeSin(t >> 1) / 4 + 0xFA;
			x[2] = -(t / 8);
			x[3] = c < 0xC ? c * 558 - 4648 : c > 0xE ? c * 877 - 10230 : 0x800;
			x[4] = uadd;
			x[5] = c > 0xA ? (0x16 - c) * 0x174 : 0x1000;
			return;
		}
		int32_t wofs = c * 558 - 4648;
		if (wofs > 0) wofs = shl32(wofs, 1);
		const int32_t scale = c > 2 ? (0x16 - c) * 0x174 : 0x1000;
		if (k == 1)
		{
			x[0] = shl32(c, 4);
			x[2] = ComputeSin(t >> 1) / 8;
			x[1] = ComputeSin(t >> 1) / 4 + 0x15E;
		}
		else
		{
			x[0] = shl32(-c, 4);
			x[2] = -(ComputeSin(t) / 8);
			x[1] = ComputeSin(t >> 1) / 4 + 0x96;
		}
		x[3] = wofs;
		x[4] = uadd;
		x[5] = scale;
	}
	static void TubeTarget(const MainNode *w, int32_t c, int16_t *P)
	{
		const int32_t d = (TubeT(c) - 0x800) >> 1;
		P[0] = (int16_t)(w->pos[0] + d);
		P[1] = *(const int16_t *)(w->target + 0x24);
		P[2] = (int16_t)(w->pos[2] - d);
	}
	static void TubeBasis(const int16_t *P, const int16_t *B, Mat4x3 *M)
	{
		const int32_t D[3] = { (int32_t)P[0] - B[0], (int32_t)B[1] - P[1], (int32_t)B[2] - P[2] };
		const int32_t r = BuildOrthonormalBasis(D, M);
		const int32_t s[3] = { r, r, r };
		Scale3DMatrix(M, s);
		M->t[0] = B[0];
		M->t[1] = B[1];
		M->t[2] = B[2];
	}
	static const uint8_t *TubeMesh(int i) { return i == 0 ? TUBE_A() : TUBE_B(); }

	static void Play(void *layout, const PrimArg *arg)
	{
		prim::play((prim::Layout *)layout, PrimCallback, (int)arg, 0);
	}

	static void BlockAt(PrimArg *arg, const int16_t *p)
	{
		RotY(0, &arg->m);
		arg->m.t[0] = p[0];
		arg->m.t[1] = p[1];
		arg->m.t[2] = p[2];
		ComposeAffineTransform(&Camera(), &arg->m, &arg->m);
	}

	static int16_t SplashResidue()
	{
		return 0;
	}
}
}

namespace ff8fx
{
namespace brr289
{
	static void SpriteDraw(uint32_t seq, const int16_t *pos, int32_t scale, int16_t frame)
	{
		TransformCameraByShadowRotation(pos, (int16_t)scale, -(scale >> 2));
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = seq;
		*(int16_t *)(h + 4) = frame;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl SpriteTask(TaskNode *n)
	{
		SpriteNode *s = (SpriteNode *)n;
		SpriteDraw(s->seq, s->pos, s->scale, s->frame);
		s->pos[0] = (int16_t)(s->pos[0] + s->vel[0]);
		s->pos[1] = (int16_t)(s->pos[1] + s->vel[1]);
		s->pos[2] = (int16_t)(s->pos[2] + s->vel[2]);
		s->frame++;
		return s->frame < s->frames ? 0 : TASK_END;
	}

	static uint32_t __cdecl SplashTask(TaskNode *n)
	{
		SplashNode *s = (SplashNode *)n;
		PrimArg arg;
		arg.m.pad = 0;
		BlockAt(&arg, s->pos);
		arg.morph = TexBase() + VERTS;
		arg.depth = -0x80;
		return prim::play((prim::Layout *)s->layout, PrimCallback, (int)&arg, 0) ? 0 : TASK_END;
	}

	static void SpawnAtTarget(SpriteNode *s, const MainNode *w, uint32_t seq, int32_t mask, int32_t half)
	{
		s->seq = seq;
		s->pos[0] = (int16_t)((int16_t)((CrtRand() & mask) + w->pos[0]) - half);
		s->pos[2] = (int16_t)((int16_t)((CrtRand() & mask) + w->pos[2]) - half);
		const uint8_t *e = w->target;
		s->pos[1] = (int16_t)((int16_t)(mul32(CrtRand(), *(const int16_t *)(e + 0x92)) >> 15) + *(const int16_t *)(e + 0x24));
	}
	static void RotateVelocity(SpriteNode *s, int16_t frames, int32_t mask, int32_t add)
	{
		GteLoadV0(s->vel);
		GteMVMVA_RotV0();
		s->frames = frames;
		s->frame = 0;
		s->scale = (CrtRand() & mask) + add;
		GteStoreIR123(s->vel);
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *w = (MainNode *)n;
		int16_t B[4];
		B[3] = 0;
		int16_t P[4];
		P[3] = SplashResidue();
		Mat4x3 M = {};
		const int16_t c = w->counter;
		if (c < 0x16)
		{
			TubeTarget(w, c, P);
			BonePoint(POINT(), B);
			TubeBasis(P, B, &M);
			Mat4x3 CM = {};
			ComposeAffineTransform(&Camera(), &M, &CM);
			for (int k = 0; k < 3; k++)
			{
				int32_t x[6];
				TubeArgs(c, k, x);
				TubeDraw(&CM, TubeMesh(k), x[0], x[1], x[2], x[3], x[4], x[5], TexBase() + VERTS);
			}
		}
		const int32_t e = c - 0xA;
		if ((uint32_t)e < 0x14)
		{
			if ((uint32_t)e >= 4 && (uint32_t)e < 0xC)
			{
				GteSetRotMatrix(&M);
				SpriteNode *s = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_SpriteTask));
				if (s)
				{
					SpawnAtTarget(s, w, game_addr(SEQ_Drip), 0x1FF, 0x100);
					s->vel[1] = 0;
					s->vel[2] = 0;
					s->vel[0] = 0;
					s->frames = 0x10;
					s->frame = 0;
					s->scale = (CrtRand() & 0x1FF) + 0x5DC;
				}
				s = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_SpriteTask));
				if (s)
				{
					SpawnAtTarget(s, w, game_addr(SEQ_Splat), 0xFF, 0x80);
					s->vel[0] = 0;
					s->vel[2] = (int16_t)(-0x20 - (CrtRand() & 0xF));
					s->vel[1] = (int16_t)(-0x20 - (CrtRand() & 0xF));
					RotateVelocity(s, 0xC, 0x3FF, 0x7D0);
				}
			}
			PrimArg arg;
			arg.m.pad = 0;
			BlockAt(&arg, w->pos);
			arg.morph = TexBase() + VERTS;
			arg.depth = -0x80;
			Play(w->layout, &arg);
		}
		if (c == 0) BdPlaySE(SOUND_Roar(), 0, 0x80);
		if ((uint32_t)(c - 2) < 0xA)
		{
			GteSetRotMatrix(&M);
			for (int i = 4; i; i--)
			{
				SpriteNode *s = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_SpriteTask));
				if (!s) continue;
				s->seq = game_addr(SEQ_Spit);
				ff8fx::copy<(8)>(s->pos, B);
				const int32_t a = CrtRand();
				const int32_t r = (CrtRand() & 0xF) + 0x20;
				s->vel[0] = (int16_t)(mul32(ComputeSin(a), r) >> 12);
				s->vel[1] = (int16_t)(mul32(ComputeCos(a), r) >> 12);
				s->vel[2] = (int16_t)(-0x20 - (CrtRand() & 0x10));
				RotateVelocity(s, 0xC, 0x1FF, 0x3E8);
			}
		}
		if ((uint32_t)(c - 8) < 0xE)
		{
			SplashNode *l = (SplashNode *)AddTaskToQueue(QSplashes(), game_addr(ORIG_SplashTask));
			if (l)
			{
				ff8fx::copy<(8)>(l->pos, P);
				DecodeModelPrimLayout(game_addr(LAYOUT_Splash), l->layout, 0x40);
			}
			GteSetRotMatrix(&M);
			SpriteNode *s = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_SpriteTask));
			if (s)
			{
				ff8fx::copy<(8)>(s->pos, P);
				s->seq = game_addr(SEQ_Splat);
				s->vel[0] = 0;
				s->vel[2] = (int16_t)(-0x10 - (CrtRand() & 0xF));
				s->vel[1] = (int16_t)(-0x20 - (CrtRand() & 0xF));
				RotateVelocity(s, 0xC, 0x3FF, 0x7D0);
			}
		}
		if (w->counter == 0xF) ApplyActionResultToTarget(Ctx()->actions->targets);
		w->counter++;
		return w->counter < 0x1E ? 0 : TASK_END;
	}

	static void StartMain(MasterNode *m)
	{
		m->started = 1;
		InitTaskQueuePool(QMain(), (void *)(TexBase() + 0x1380), 0x1A0, 1);
		InitTaskQueuePool(QSprites(), (void *)TexBase(), 0x24, 0x40);
		InitTaskQueuePool(QSplashes(), (void *)(TexBase() + 0x900), 0x54, 0x20);
		MainNode *w = (MainNode *)AddTaskToQueue(QMain(), game_addr(ORIG_MainTask));
		Memset32(&w->counter, 0, 0x65);
		w->target = Entity(Ctx()->actions->targets[0]);
		DecodeModelPrimLayout(game_addr(LAYOUT_Target), w->layout, 0x184);
		GetDefaultEffectPosition(w->target, w->pos);
		NegateZ(TUBE_A());
		NegateZ(TUBE_B());
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
		if (m->counter == 1 && !m->started) StartMain(m);
		int r_main, r_splash;
		if (m->started)
		{
			r_main = ExecuteTaskQueue(QMain());
			ExecuteTaskQueue(QSprites());
			r_splash = ExecuteTaskQueue(QSplashes());
		}
		else r_main = r_splash = (int)n;
		if (m->started && r_main == 0 && r_splash == 0) return TASK_END;
		m->counter++;
		return 0;
	}
}

	void register_mag289_brrawghh()
	{
		register_port(brr289::ORIG_MasterTask, (void *)brr289::MasterTask, "289 MasterTask", 289);
		register_port(brr289::ORIG_MainTask, (void *)brr289::MainTask, "289 MainTask", 289);
		register_port(brr289::ORIG_SpriteTask, (void *)brr289::SpriteTask, "289 SpriteTask", 289);
		register_port(brr289::ORIG_SplashTask, (void *)brr289::SplashTask, "289 SplashTask", 289);
	}
}

