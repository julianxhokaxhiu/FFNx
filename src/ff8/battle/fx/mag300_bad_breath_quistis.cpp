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

#include "mag_common.h"
#include "mag_ice_common.h"

namespace ff8fx
{
namespace badbreath300
{
	using namespace eng;
	using namespace magc;
	using namespace icec;

	inline Mat4x3 &MeshFrame() { return var<Mat4x3>(game_addr(game::MAG_300_BAD_BREATH_QUISTIS_MeshFrame)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::MAG_300_BAD_BREATH_QUISTIS_Caster)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_300_BAD_BREATH_QUISTIS_MagicBuffer)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_300_BAD_BREATH_QUISTIS_Context)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_300_BAD_BREATH_QUISTIS_PacketCursor)); }
	static const void *FADE_COLOUR() { return (const void *)game_addr(game::MAG_300_BAD_BREATH_QUISTIS_FadeColour); }
	inline TaskQueue *QSmoke() { return (TaskQueue *)game_addr(game::MAG_300_BAD_BREATH_QUISTIS_SmokeQueue); }
	inline TaskQueue *QHits() { return (TaskQueue *)game_addr(game::MAG_300_BAD_BREATH_QUISTIS_HitQueue); }
	inline TaskQueue *QBreath() { return (TaskQueue *)game_addr(game::MAG_300_BAD_BREATH_QUISTIS_BreathQueue); }
	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	static constexpr game::Id ORIG_MasterTask = game::MAG_300_BAD_BREATH_QUISTIS_MasterTask;
	static constexpr game::Id ORIG_BreathTask = game::MAG_300_BAD_BREATH_QUISTIS_BreathTask;
	static constexpr game::Id ORIG_HitTask = game::MAG_300_BAD_BREATH_QUISTIS_HitTask;
	static constexpr game::Id ORIG_SmokeTask = game::MAG_300_BAD_BREATH_QUISTIS_SmokeTask;
	static constexpr game::Id ORIG_PrimCallback = game::MAG_300_BAD_BREATH_QUISTIS_PrimCallback;
	static uint8_t *MESH1() { return (uint8_t *)game_addr(game::MAG_300_BAD_BREATH_QUISTIS_Mesh1); }
	static uint8_t *MESH2() { return (uint8_t *)game_addr(game::MAG_300_BAD_BREATH_QUISTIS_Mesh2); }
	static constexpr game::Id MODEL_Hit = game::MAG_300_BAD_BREATH_QUISTIS_HitLayout;
	static constexpr game::Id SEQ_Smoke = game::MAG_300_BAD_BREATH_QUISTIS_SmokeSequence;
	static constexpr game::Id CAMERA_Anim = game::MAG_300_BAD_BREATH_QUISTIS_CameraAnim;
	static constexpr game::Id COM_Anim = game::MAG_300_BAD_BREATH_QUISTIS_CasterAnimSet;
	static const uint32_t BIND_Block = 0x83C;
	static const uint8_t *COLOURS() { return (const uint8_t *)game_addr(game::MAG_300_BAD_BREATH_QUISTIS_Colours); }
	static const int16_t *SWIRL_TABLE() { return (const int16_t *)game_addr(game::MAG_300_BAD_BREATH_QUISTIS_SwirlTable); }
	static const int16_t *MOUTH() { return (const int16_t *)game_addr(game::MAG_300_BAD_BREATH_QUISTIS_MouthPoint); }
	static const void *SOUND_Breath() { return (const void *)game_addr(game::MAG_300_BAD_BREATH_QUISTIS_Sound); }
	static const uint32_t VERTS = 0x85C;
	static const uint32_t ARENA_A = 0xD84, ARENA_B = 0x18D84;
	static const uint32_t POOL_Hits = 0x300, POOL_Breath = 0x800;

	inline uint32_t FadeTile(int32_t t, int32_t r, int32_t g, int32_t b, int32_t in, int32_t hold, int32_t out, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_FadeTileInHoldOut))(t, r, g, b, in, hold, out, cursor); }
	inline uint32_t Tile(int32_t r, int32_t g, int32_t b, int32_t level, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_FullScreenTile))(r, g, b, level, cursor); }
	inline void RotZ(int32_t angle, Mat4x3 *out) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::MAG_065_RotationZMatrix))(angle, out); }
	inline void GteZeroTrans() { fn<void (__cdecl *)()>(game_addr(game::GTE_ZeroTranslation))(); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline int32_t BuildOrthonormalBasis(const int32_t *v, Mat4x3 *m) { return fn<int32_t (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline void MulMatrix3(Mat4x3 *a, const void *b) { fn<void (__cdecl *)(Mat4x3 *, const void *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void BlendColour(const void *a, const void *b, int32_t wa, int32_t wb, void *out) { fn<void (__cdecl *)(const void *, const void *, int32_t, int32_t, void *)>(game_addr(game::ColourLerp))(a, b, wa, wb, out); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void GteLoadRGBBytes(const void *rgb) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadIR123FromBytes))(rgb); }
	inline void GteStoreRGBBytes(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123LowBytes))(dst); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void BindEntityModelFromComData(uint8_t *entity, uint32_t out, uint32_t data) { fn<void (__cdecl *)(uint8_t *, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline void QueueChainTransformation(uint8_t *entity, int32_t id) { fn<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct BreathNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t count;
		uint8_t *ents[4];
		uint8_t idx[4];
		int16_t width;
		int16_t z;
		int16_t y;
		int16_t angle;
		int16_t phase;
		int16_t next;
		int32_t length;
		int16_t mouth[3];
		int16_t pad3A;
	};
	struct HitNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t idx;
		uint8_t slot;
		int16_t pos[4];
		uint8_t layout[0x128];
	};
	struct SmokeNode
	{
		TaskNode hdr;
		uint16_t counter;
		uint16_t pad0E;
		int16_t pos[3];
		int16_t size;
	};
	struct PrimArg
	{
		int16_t pos[4];
		int32_t scale[3];
		uint32_t flag;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(BreathNode) == 0x3C && sizeof(HitNode) == 0x140 && sizeof(SmokeNode) == 0x18, "Bad Breath (Quistis) nodes");
	static_assert(sizeof(PrimArg) == 0x1C, "Bad Breath (Quistis) prim block");

	static uint32_t g_hit_flag_word = 0;
	inline uint32_t &HitFlagWord() { return g_hit_flag_word; }

	static void NormalizeDepth(uint8_t *model)
	{
		const int32_t count = *(const int32_t *)(model + 4);
		int32_t mn = *(const int16_t *)(model + 0x1C), mx = mn;
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
			*(int16_t *)p = (int16_t)(shl32(*(const int16_t *)(p - 2), 12) / range);
	}

	static void SwirlVertices(const uint8_t *model, int32_t phase, int16_t *out)
	{
		int32_t count = *(const int32_t *)(model + 4);
		const int16_t *v = (const int16_t *)(model + 8);
		do
		{
			GteLoadV0(v);
			GteMVMVA_RotV0Tr();
			GteStoreIR123(out);
			const int32_t w = v[3];
			const int32_t a = (phase + w * 2) & 0xFFF;
			const int32_t amp = (int32_t)SWIRL_TABLE()[w & 0xFFE] >> 4;
			out[1] = (int16_t)(out[1] - (int16_t)(mul32(SWIRL_TABLE()[a * 2], amp) >> 12));
			out[0] = (int16_t)(out[0] + (int16_t)(mul32(SWIRL_TABLE()[a * 2 + 1], amp) >> 12));
			out += 4;
			v += 4;
		} while (--count);
	}

	static void BonePoint(const int16_t *v, int16_t *out)
	{
		uint8_t *caster = Caster();
		uint8_t *bones = *(uint8_t **)*(uint8_t **)(caster + 0x64) + 0x10;
		BuildBoneMatricesFromPose(caster + 0x60);
		Mat4x3 m;
		ComposeAffineTransform((const Mat4x3 *)(caster + 0x40), (const Mat4x3 *)(bones + v[3] * 0x30 + 0x10), &m);
		GteSetRotMatrixCtrl(&m);
		GteSetTransVectorCtrl(&m);
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(out);
	}

	static bool OffX(int16_t x) { return x < 0 || x > 0xA00; }
	static bool OffY(int16_t y) { return y < 0 || y > 0x6C0; }

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

	static void Shade(const uint8_t *h, uint8_t red, void *dst)
	{
		GteSetIR0((int32_t)red << 5);
		GteLoadRGBBytes(h + 4);
		GteGPF();
		GteStoreRGBBytes(dst);
	}

	static uint8_t *Insert3(const uint8_t *h, uint32_t otp, uint8_t *main, uint8_t *p)
	{
		uint8_t *t1 = p;
		*(uint32_t *)t1 = 0x02000000;
		*(uint32_t *)(t1 + 4) = TextureWindow(h + 0x10);
		*(uint32_t *)(t1 + 8) = 0;
		InsertPrimAutoDepth(otp, t1);
		uint8_t *md = p + 0xC;
		*(uint32_t *)md = 0x01000000;
		*(uint32_t *)(md + 4) = 0xE1000220;
		InsertPrimAutoDepth(otp, md);
		InsertPrimAutoDepth(otp, main);
		uint8_t *t2 = p + 0x14;
		*(uint32_t *)t2 = 0x02000000;
		*(uint32_t *)(t2 + 4) = TextureWindow(h + 0x18);
		*(uint32_t *)(t2 + 8) = 0;
		InsertPrimAutoDepth(otp, t2);
		return p + 0x20;
	}

	static uint32_t MeshQuads(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *verts = Cursor(h, 0x20);
		uint8_t *prim = Cursor(h, 0x24);
		int32_t count = *(int32_t *)prim;
		prim += 4;
		Cursor(h, 0x24) = prim;
		uint8_t *p = (uint8_t *)cursor;
		if (count <= 0) return cursor;
		for (int32_t left = count; left; left--, prim += 0x18)
		{
			GteLoadV012(verts + 4 * *(uint16_t *)(prim + 4), verts + 4 * *(uint16_t *)(prim + 6), verts + 4 * *(uint16_t *)(prim + 8));
			GteRTPT();
			*(uint32_t *)p = 0x09000000;
			*(uint32_t *)(p + 4) = *(uint32_t *)prim;
			*(uint32_t *)(p + 0xC) = *(uint32_t *)(prim + 0xC);
			*(uint32_t *)(p + 0x14) = *(uint32_t *)(prim + 0x10);
			const uint32_t uv = *(uint32_t *)(prim + 0x14);
			*(uint32_t *)(p + 0x1C) = uv;
			*(uint32_t *)(p + 0x24) = uv >> 16;
			GteReadFLAG(h + 0x34);
			if (*(uint32_t *)(h + 0x34) & 0x60000) continue;
			GteReadSXY012(p + 8, p + 0x10, p + 0x18);
			GteLoadV0(verts + 4 * *(uint16_t *)(prim + 0xA));
			GteRTPS();
			uint32_t clip = 0;
			if (OffX(*(int16_t *)(p + 8))) clip = 1;
			if (OffX(*(int16_t *)(p + 0x10))) clip |= 2;
			if (OffX(*(int16_t *)(p + 0x18))) clip |= 4;
			if (OffY(*(int16_t *)(p + 0xA))) clip |= 0x10;
			if (OffY(*(int16_t *)(p + 0x12))) clip |= 0x20;
			if (OffY(*(int16_t *)(p + 0x1A))) clip |= 0x40;
			GteReadSXY2(p + 0x20);
			GteAVSZ4();
			if (OffX(*(int16_t *)(p + 0x20))) clip |= 8;
			if (OffY(*(int16_t *)(p + 0x22))) clip |= 0x80;
			if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
			GteReadOTZReg(h + 0x30);
			Shade(h, prim[0], p + 4);
			static const int us[4] = { 0xC, 0x14, 0x1C, 0x24 };
			ScrollU(p, us, 4, *(uint16_t *)(h + 0xC), h[0x1C]);
			const uint32_t otp = ot + 4 * (*(int32_t *)(h + 0x30) >> shift);
			p = Insert3(h, otp, p, p + 0x28);
		}
		Cursor(h, 0x24) = prim;
		return (uint32_t)p;
	}

	static uint32_t MeshGouraudQuads(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *verts = Cursor(h, 0x20);
		uint8_t *prim = Cursor(h, 0x24);
		int32_t count = *(int32_t *)prim;
		prim += 4;
		Cursor(h, 0x24) = prim;
		uint8_t *p = (uint8_t *)cursor;
		if (count <= 0) return cursor;
		for (int32_t left = count; left; left--, prim += 0x24)
		{
			GteLoadV012(verts + 4 * *(uint16_t *)(prim + 4), verts + 4 * *(uint16_t *)(prim + 6), verts + 4 * *(uint16_t *)(prim + 8));
			GteRTPT();
			*(uint32_t *)p = 0x0C000000;
			*(uint32_t *)(p + 4) = *(uint32_t *)prim;
			*(uint32_t *)(p + 0xC) = *(uint32_t *)(prim + 0xC);
			*(uint32_t *)(p + 0x18) = *(uint32_t *)(prim + 0x10);
			const uint32_t uv = *(uint32_t *)(prim + 0x14);
			*(uint32_t *)(p + 0x24) = uv;
			*(uint32_t *)(p + 0x30) = uv >> 16;
			GteReadFLAG(h + 0x34);
			if (*(uint32_t *)(h + 0x34) & 0x60000) continue;
			GteReadSXY012(p + 8, p + 0x14, p + 0x20);
			GteLoadV0(verts + 4 * *(uint16_t *)(prim + 0xA));
			GteRTPS();
			uint32_t clip = 0;
			if (OffX(*(int16_t *)(p + 8))) clip = 1;
			if (OffX(*(int16_t *)(p + 0x14))) clip |= 2;
			if (OffX(*(int16_t *)(p + 0x20))) clip |= 4;
			if (OffY(*(int16_t *)(p + 0xA))) clip |= 0x10;
			if (OffY(*(int16_t *)(p + 0x16))) clip |= 0x20;
			if (OffY(*(int16_t *)(p + 0x22))) clip |= 0x40;
			GteReadSXY2(p + 0x2C);
			GteAVSZ4();
			if (OffX(*(int16_t *)(p + 0x2C))) clip |= 8;
			if (OffY(*(int16_t *)(p + 0x2E))) clip |= 0x80;
			if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
			GteReadOTZReg(h + 0x30);
			Shade(h, prim[0], p + 4);
			Shade(h, prim[0x18], p + 0x10);
			Shade(h, prim[0x1C], p + 0x1C);
			Shade(h, prim[0x20], p + 0x28);
			static const int us[4] = { 0xC, 0x18, 0x24, 0x30 };
			ScrollU(p, us, 4, *(uint16_t *)(h + 0xC), h[0x1C]);
			const uint32_t otp = ot + 4 * (*(int32_t *)(h + 0x30) >> shift);
			p = Insert3(h, otp, p, p + 0x34);
		}
		Cursor(h, 0x24) = prim;
		return (uint32_t)p;
	}

	static uint32_t MeshDraw(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *m = Cursor(h, 0);
		Cursor(h, 0x24) = m + *(uint32_t *)m + 0xC;
		const uint32_t r = MeshQuads(h, ot, shift, cursor);
		Cursor(h, 0x24) = Cursor(h, 0x24) + 0xC;
		return MeshGouraudQuads(h, ot, shift, r);
	}

	static int16_t SweepX(int32_t t, int16_t width) { return (int16_t)(mul32(ComputeCos((int32_t)((uint32_t)shl32(t, 11) / 50)), width) >> 12); }

	static int32_t Growth(int32_t t) { return (uint32_t)t < 0x10 ? ComputeSin((t & 0x3FFFFF) << 6) : 0x1000; }

	static void MeshColour(int32_t t, uint8_t *h4)
	{
		const int32_t v = shl32(t, 10);
		const int32_t i = (v >> 12) % 3, f = v & 0xFFF;
		BlendColour(COLOURS() + i * 4, COLOURS() + i * 4 + 4, 0x1000 - f, f, h4);
		if ((uint32_t)t < 8)
		{
			const int32_t s = ComputeSin((t & 0x3FFFFF) << 7);
			BlendColour(FADE_COLOUR(), h4, 0x1000 - s, s, h4);
		}
		else if ((uint32_t)t > 0x22)
		{
			const int32_t s = ComputeSin((shl32(t, 6) - 0x841) & 0xFFFFFC0);
			BlendColour(FADE_COLOUR(), h4, s, 0x1000 - s, h4);
		}
	}
}
}

namespace ff8fx
{
namespace badbreath300
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
		RotationFromAngles(r->rot, &m);
		int16_t off[4] = { r->pos[0], r->pos[1], r->pos[2], 0 };
		if (arg->flag)
		{
			off[0] = (int16_t)(mul32(r->pos[0], arg->scale[0]) >> 12);
			off[1] = (int16_t)(mul32(r->pos[1], arg->scale[1]) >> 12);
			off[2] = (int16_t)(mul32(r->pos[2], arg->scale[2]) >> 12);
			Scale3DMatrix(&m, arg->scale);
		}
		off[0] = (int16_t)(off[0] + arg->pos[0]);
		off[1] = (int16_t)(off[1] + arg->pos[1]);
		off[2] = (int16_t)(off[2] + arg->pos[2]);
		GteSetRotMatrixCtrl(&Camera());
		GteLoadV0(off);
		GteMVMVA_RotV0();
		GteReadMAC123(m.t);
		MatrixMultiply(&Camera(), &m);
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
		PacketCursor() = RenderPrimModel(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void SmokeDraw(const SmokeNode *p)
	{
		uint8_t *h = AllocHeader(0xB4);
		TransformCameraByShadowRotation(p->pos, p->size, -((int32_t)p->size >> 2));
		*(uint32_t *)h = game_addr(SEQ_Smoke);
		*(uint16_t *)(h + 4) = p->counter;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl SmokeTask(TaskNode *n)
	{
		SmokeNode *p = (SmokeNode *)n;
		HitFlagWord() = (uint32_t)n;
		SmokeDraw(p);
		p->pos[1] = (int16_t)(p->pos[1] - 0x10);
		p->counter++;
		return (int16_t)p->counter < 0x12 ? 0 : TASK_END;
	}

	static void HitArg(const HitNode *t, PrimArg *arg)
	{
		ff8fx::copy<(sizeof(arg->pos))>(arg->pos, t->pos);
		const int32_t s = ((int32_t)*(const int16_t *)(Entity(t->slot) + 0x26) >> 2) + 0x200;
		arg->scale[0] = s;
		arg->scale[1] = s;
		arg->scale[2] = s;
		arg->flag = HitFlagWord();
		arg->morph = TexBase() + VERTS;
	}

	static uint32_t __cdecl HitTask(TaskNode *n)
	{
		HitNode *t = (HitNode *)n;
		uint8_t *entity = Entity(t->slot);
		PrimArg arg;
		HitArg(t, &arg);
		prim::play((prim::Layout *)t->layout, PrimCallback, (int)&arg, 0);
		if (t->counter <= 8)
		{
			SmokeNode *s = (SmokeNode *)AddTaskToQueue(QSmoke(), game_addr(ORIG_SmokeTask));
			if (s)
			{
				const int32_t bone = mul32(CrtRand(), ***(const uint8_t *const *const *)(entity + 0x64)) >> 15;
				GetEffectSpawnPosition(entity, bone, 0, s->pos);
				s->counter = 0;
				s->size = (int16_t)((CrtRand() & 0x1FF) + 0x400);
			}
		}
		if (t->counter == 0x1E) ApplyActionResultToTarget(Ctx()->actions[0].targets + t->idx * TARGET_STRIDE);
		t->counter++;
		return t->counter < 0x25 ? 0 : TASK_END;
	}

	static void PartyReact(int32_t id)
	{
		for (uint8_t *e = (uint8_t *)game_addr(game::BattleEntitySlotData); e < (uint8_t *)game_addr(game::unk_1D97494); e += 0x9C)
			if ((e[0] & 2) && !(e[8] & 3) && e != Caster()) QueueChainTransformation(e, id);
	}

	static uint32_t __cdecl BreathTask(TaskNode *n)
	{
		BreathNode *e = (BreathNode *)n;
		BonePoint(MOUTH(), e->mouth);
		const int32_t t = e->counter - 8;
		if ((uint32_t)t < 0x32)
		{
			uint8_t *h = AllocHeader(0x5C);
			PacketCursor() = FadeTile(t, 0x40, 0x40, 0, 0xE, 0x16, 0xE, PacketCursor());
			Mat4x3 rot;
			RotZ(e->angle, &rot);
			GteSetRotMatrix(&rot);
			GteZeroTrans();
			const uint32_t verts = TexBase() + VERTS;
			SwirlVertices(MESH1(), e->phase, (int16_t *)verts);
			const int16_t x = SweepX(t, e->width);
			const int16_t y = e->y, z = e->z;
			if (e->next < e->count)
			{
				uint8_t *target = e->ents[e->next];
				if ((int32_t)x - 0x12C <= (int32_t)*(const int16_t *)(target + 0x1C))
				{
					HitNode *p = (HitNode *)AddTaskToQueue(QHits(), game_addr(ORIG_HitTask));
					if (p)
					{
						p->counter = 0;
						p->idx = e->idx[e->next];
						p->slot = (uint8_t)((int32_t)((uint32_t)target - game_addr(game::BattleEntitySlotData)) / 0x9C);
						GetDefaultEffectPosition(target, p->pos);
						p->pos[1] = (int16_t)((int32_t)*(const int16_t *)(target + 0x92) / 2 + *(const int16_t *)(target + 0x1E));
						DecodeModelPrimLayout(game_addr(MODEL_Hit), p->layout, 0x128);
					}
					e->next++;
				}
			}
			const int32_t dx = (int32_t)x - e->mouth[0];
			const int32_t dy = (int32_t)y - e->mouth[1];
			const int32_t dz = (int32_t)z - e->mouth[2];
			e->length = Sqrt(mul32(dz, dz) + mul32(dy, dy) + mul32(dx, dx)) + 0x200;
			int16_t scale[9] = { 0x1000, 0, 0, 0, 0x1000, 0, 0, 0, (int16_t)e->length };
			MeshColour(t, h + 4);
			if ((uint32_t)t < 0x10) scale[8] = (int16_t)(mul32(scale[8], Growth(t)) >> 12);
			const int32_t v[3] = { dx, dy, -dz };
			Mat4x3 basis;
			BuildOrthonormalBasis(v, &basis);
			MulMatrix3(&basis, scale);
			basis.t[0] = e->mouth[0];
			basis.t[1] = e->mouth[1];
			basis.t[2] = e->mouth[2];
			HitFlagWord() = (uint32_t)(int32_t)e->mouth[0];
			ComposeAffineTransform(&Camera(), &basis, &MeshFrame());
			GteSetRotMatrix(&MeshFrame());
			GteSetTransVector(&MeshFrame());
			*(uint16_t *)(h + 0x16) = 0x100;
			*(uint16_t *)(h + 0x14) = 0x100;
			*(uint16_t *)(h + 0x1E) = 0x100;
			*(uint16_t *)(h + 0x12) = 0;
			*(uint16_t *)(h + 0x10) = 0;
			*(uint16_t *)(h + 0x18) = 0;
			*(uint16_t *)(h + 0x1A) = 0;
			*(uint16_t *)(h + 0x1C) = 0x80;
			*(uint16_t *)(h + 0xC) = (uint16_t)((uint8_t)t * 10 & 0x7F);
			*(uint8_t **)h = MESH1();
			*(uint32_t *)(h + 0x20) = verts;
			PacketCursor() = MeshDraw(h, RenderOT44(), 2, PacketCursor());
			RotZ((int16_t)-e->angle, &rot);
			GteSetRotMatrix(&rot);
			GteZeroTrans();
			SwirlVertices(MESH2(), (int32_t)e->phase + 0x400, (int16_t *)verts);
			GteSetRotMatrix(&MeshFrame());
			GteSetTransVector(&MeshFrame());
			*(uint8_t **)h = MESH2();
			PacketCursor() = MeshDraw(h, RenderOT44(), 2, PacketCursor());
			FieldFree(0x5C);
			if (t == 0) PartyReact(2);
			else if (t == 0x31) PartyReact(1);
		}
		if (e->counter == 0xA) BdPlaySE(SOUND_Breath(), 0, 0x80);
		e->angle = (int16_t)(e->angle - 0x32);
		e->phase = (int16_t)(e->phase + 0xC8);
		if (e->counter >= 0x3A) return TASK_END;
		e->counter++;
		return 0;
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
		if (m->counter == 1 && !m->started)
		{
			m->started = 1;
			InitTaskQueuePool(QBreath(), (void *)(TexBase() + POOL_Breath), 0x3C, 1);
			InitTaskQueuePool(QHits(), (void *)(TexBase() + POOL_Hits), 0x140, 4);
			InitTaskQueuePool(QSmoke(), (void *)TexBase(), 0x18, 0x20);
			BreathNode *e = (BreathNode *)AddTaskToQueue(QBreath(), game_addr(ORIG_BreathTask));
			Memset32(&e->counter, 0, 0xC);
			uint8_t *const b = (uint8_t *)e;
			e->count = (int16_t)Ctx()->actions[0].target_count;
			e->width = 0x700;
			e->z = 0;
			e->y = 0;
			for (int32_t i = 0; i < e->count; i++)
			{
				b[0x20 + i] = (uint8_t)i;
				uint8_t *ent = Entity(Ctx()->actions[0].targets[i * TARGET_STRIDE]);
				*(uint8_t **)(b + 0x10 + 4 * i) = ent;
				int16_t pos[4];
				GetDefaultEffectPosition(ent, pos);
				int16_t ax = pos[0];
				if (ax < 0) ax = (int16_t)(0u - (uint32_t)*(const int32_t *)pos);
				if (e->width < ax) e->width = ax;
				e->z = (int16_t)(e->z + pos[2]);
				e->y = (int16_t)(e->y + pos[1]);
			}
			e->z = (int16_t)((int32_t)e->z / (int32_t)e->count);
			e->width = (int16_t)(e->width + 0x100);
			e->y = (int16_t)((int32_t)e->y / (int32_t)e->count);
			for (int32_t i = 0; i < e->count - 1; i++)
			{
				for (int32_t j = i + 1; j < e->count; j++)
				{
					uint8_t *a = *(uint8_t **)(b + 0x10 + 4 * i);
					uint8_t *c = *(uint8_t **)(b + 0x10 + 4 * j);
					if (*(const int16_t *)(a + 0x1C) < *(const int16_t *)(c + 0x1C))
					{
						*(uint8_t **)(b + 0x10 + 4 * i) = c;
						*(uint8_t **)(b + 0x10 + 4 * j) = a;
						const uint8_t x = b[0x20 + j];
						const uint8_t y = b[0x20 + i];
						b[0x20 + i] = x;
						b[0x20 + j] = y;
					}
				}
			}
			NormalizeDepth(MESH1());
			NormalizeDepth(MESH2());
			BindEntityModelFromComData(Caster(), TexBase() + BIND_Block, game_addr(COM_Anim));
			QueueChainTransformation(Caster(), 1);
		}
		int a0, a1, a2;
		if (m->started)
		{
			a0 = ExecuteTaskQueue(QBreath());
			a1 = ExecuteTaskQueue(QHits());
			a2 = ExecuteTaskQueue(QSmoke());
		}
		else a0 = a1 = a2 = (int)n;
		if (m->started && a0 == 0 && a1 == 0 && a2 == 0) return TASK_END;
		m->counter++;
		return 0;
	}
}

	void register_mag300_bad_breath_quistis()
	{
		register_port(badbreath300::ORIG_MasterTask, (void *)badbreath300::MasterTask, "300 MasterTask", 300);
		register_port(badbreath300::ORIG_BreathTask, (void *)badbreath300::BreathTask, "300 BreathTask", 300);
		register_port(badbreath300::ORIG_HitTask, (void *)badbreath300::HitTask, "300 HitTask", 300);
		register_port(badbreath300::ORIG_SmokeTask, (void *)badbreath300::SmokeTask, "300 SmokeTask", 300);
	}
}

