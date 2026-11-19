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

// Effect 271: LV Up

#include "mag_common.h"

namespace ff8fx
{
namespace lv27x
{
	void ModelBounds(uint8_t *block, uint8_t *anim_header);
}
namespace lv271
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_271_LV_UP_CastContext)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_271_LV_UP_MagicBuffer)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_271_LV_UP_PacketCursor)); }
	inline Mat4x3 &CameraN() { return var<Mat4x3>(game_addr(game::MAG_271_LV_UP_CameraOrthonormal)); }
	inline TaskQueue *QPlay() { return (TaskQueue *)game_addr(game::MAG_271_LV_UP_PlayQueue); }

	static constexpr game::Id ORIG_RootTask = game::MAG_271_LV_UP_RootTask;
	static constexpr game::Id ORIG_PlayTask = game::MAG_271_LV_UP_PlayTask;
	static constexpr game::Id MODEL_LAYOUT = game::MAG_271_LV_UP_Layout;
	static const uint32_t LAYOUT_BYTES = 0x4A0;
	static const uint32_t NODE_BYTES = 0x4CC;
	static const uint32_t MORPH = 0x4CC;
	static const uint32_t ARENA_A = 0x4D4;
	static const uint32_t ARENA_B = 0x104D4;
	static const void *SOUND() { return (const void *)game_addr(game::MAG_271_LV_UP_Sound); }

	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesZYX(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_117_RotationFromAnglesZYX))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline uint32_t RenderPrimModel2(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteStoreOTZ(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteSetFarColour(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteLoadRGB012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadRGB012))(a, b, c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteDPCT() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCT))(); }
	inline void GteStoreRGB2(void *c) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(c); }
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
	struct PlayNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		uint8_t *entity;
		int16_t anchor[4];
		int32_t scale[3];
		int32_t pad28;
		uint8_t layout[LAYOUT_BYTES];
	};
	struct PrimArg
	{
		int16_t pos[4];
		int32_t scale[3];
		int32_t scaled;
		int32_t depth;
		int16_t scroll;
		int16_t pad1E;
		uint32_t morph;
	};
	struct BoundsBlock
	{
		uint32_t pad00;
		uint8_t *cursor;
		uint8_t *matrix;
		int16_t v[3];
		int16_t pad12;
		int16_t box[6];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(PlayNode) == NODE_BYTES, "LV Up nodes");
	static_assert(sizeof(PrimArg) == 0x24 && sizeof(BoundsBlock) == 0x20, "LV Up blocks");

	static inline bool OutX(const uint8_t *xy) { const int16_t v = *(const int16_t *)xy; return v < 0 || v > 0xA00; }
	static inline bool OutY(const uint8_t *xy) { const int16_t v = *(const int16_t *)xy; return v < 0 || v > 0x6C0; }
	static uint8_t *&ListCursor(uint8_t *h) { return *(uint8_t **)(h + 0x34); }

	static uint32_t TexWindow(const uint8_t *tw)
	{
		uint32_t e = (uint32_t)(tw[2] & 0xF8) | 0xFFFE2000u;
		e <<= 5;
		e |= (uint32_t)(tw[0] & 0xF8);
		e <<= 5;
		e |= ~((uint32_t)*(const uint16_t *)(tw + 6) - 1) & 0xF8;
		e <<= 2;
		e |= (uint32_t)((int32_t)~((uint32_t)*(const uint16_t *)(tw + 4) - 1) >> 3) & 0x1F;
		return e;
	}

	static uint8_t *QuadWindows(const uint8_t *h, uint32_t bucket, uint8_t *pk, uint8_t *p2)
	{
		*(uint32_t *)p2 = 0x2000000;
		*(uint32_t *)(p2 + 4) = TexWindow(h + 0x24);
		*(uint32_t *)(p2 + 8) = 0;
		InsertPrimAutoDepth(bucket, p2);
		InsertPrimAutoDepth(bucket, pk);
		uint8_t *p3 = p2 + 0xC;
		*(uint32_t *)p3 = 0x2000000;
		*(uint32_t *)(p3 + 4) = TexWindow(h + 0x2C);
		*(uint32_t *)(p3 + 8) = 0;
		InsertPrimAutoDepth(bucket, p3);
		return p3 + 0xC;
	}

	static void ScrollV(const uint8_t *h, uint8_t *pk, const int *o)
	{
		const uint32_t scroll = *(const uint16_t *)(h + 0x22);
		const uint32_t v3 = pk[o[3]] + scroll, v0 = pk[o[0]] + scroll, v1 = pk[o[1]] + scroll, v2 = pk[o[2]] + scroll;
		uint32_t w3 = v3;
		if ((int32_t)(v3 | v2 | v1 | v0) > 0xFF)
		{
			const uint8_t wrap = h[0x32];
			pk[o[0]] = (uint8_t)(v0 - wrap);
			pk[o[1]] = (uint8_t)(v1 - wrap);
			pk[o[2]] = (uint8_t)(v2 - wrap);
			w3 = (uint8_t)(v3 - wrap);
		}
		else
		{
			pk[o[0]] = (uint8_t)v0;
			pk[o[1]] = (uint8_t)v1;
			pk[o[2]] = (uint8_t)v2;
		}
		pk[o[3]] = (uint8_t)w3;
	}

	static bool QuadOnScreen(uint8_t *x0, uint8_t *x1, uint8_t *x2, uint8_t *x3)
	{
		uint32_t clip = 0;
		if (OutX(x0)) clip = 1;
		if (OutX(x1)) clip |= 2;
		if (OutX(x2)) clip |= 4;
		if (OutY(x0 + 2)) clip |= 0x10;
		if (OutY(x1 + 2)) clip |= 0x20;
		if (OutY(x2 + 2)) clip |= 0x40;
		GteReadSXY2(x3);
		GteAVSZ4();
		if (OutX(x3)) clip |= 8;
		if (OutY(x3 + 2)) clip |= 0x80;
		return (clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0;
	}

	static uint32_t ScrollFT4(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *rec = ListCursor(h);
		const int32_t count = *(const int32_t *)rec;
		rec += 4;
		const uint8_t *verts = *(uint8_t **)(h + 4);
		ListCursor(h) = rec;
		uint8_t *pk = (uint8_t *)cursor;
		if (count <= 0) return cursor;
		static const int V[4] = { 0xD, 0x15, 0x1D, 0x25 };
		for (int32_t left = count; left; left--, rec += 0x18)
		{
			GteLoadV012(verts + 4 * *(const uint16_t *)(rec + 4), verts + 4 * *(const uint16_t *)(rec + 6), verts + 4 * *(const uint16_t *)(rec + 8));
			GteRTPT();
			*(uint32_t *)pk = 0x9000000;
			*(uint32_t *)(pk + 4) = *(const uint32_t *)rec;
			*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(rec + 0xC);
			*(uint32_t *)(pk + 0x14) = *(const uint32_t *)(rec + 0x10);
			const uint32_t uv = *(const uint32_t *)(rec + 0x14);
			*(uint32_t *)(pk + 0x1C) = uv;
			*(uint32_t *)(pk + 0x24) = uv >> 16;
			GteReadFLAG(h + 0x44);
			if (*(const uint32_t *)(h + 0x44) & 0x60000) continue;
			GteReadSXY012Split(pk + 8, pk + 0x10, pk + 0x18);
			GteLoadV0(verts + 4 * *(const uint16_t *)(rec + 0xA));
			GteRTPS();
			if (!QuadOnScreen(pk + 8, pk + 0x10, pk + 0x18, pk + 0x20)) continue;
			GteStoreOTZ(h + 0x40);
			if (*(const int32_t *)(h + 0xC))
			{
				GteSetFarColour(h[8], h[9], h[0xA]);
				GteLoadRGBC(pk + 4);
				GteSetIR0(*(const int32_t *)(h + 0xC));
				GteDPCS();
				GteStoreRGB2(pk + 4);
			}
			ScrollV(h, pk, V);
			const uint32_t bucket = ot + 4 * (uint32_t)(*(const int32_t *)(h + 0x40) >> shift);
			pk = QuadWindows(h, bucket, pk, pk + 0x28);
		}
		ListCursor(h) = rec;
		return (uint32_t)pk;
	}

	static uint32_t ScrollGT4(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *rec = ListCursor(h);
		const int32_t count = *(const int32_t *)rec;
		rec += 4;
		const uint8_t *verts = *(uint8_t **)(h + 4);
		ListCursor(h) = rec;
		uint8_t *pk = (uint8_t *)cursor;
		if (count <= 0) return cursor;
		static const int V[4] = { 0xD, 0x19, 0x25, 0x31 };
		for (int32_t left = count; left; left--, rec += 0x24)
		{
			GteLoadV012(verts + 4 * *(const uint16_t *)(rec + 4), verts + 4 * *(const uint16_t *)(rec + 6), verts + 4 * *(const uint16_t *)(rec + 8));
			GteRTPT();
			*(uint32_t *)pk = 0xC000000;
			*(uint32_t *)(pk + 4) = *(const uint32_t *)rec;
			*(uint32_t *)(pk + 0xC) = *(const uint32_t *)(rec + 0xC);
			*(uint32_t *)(pk + 0x18) = *(const uint32_t *)(rec + 0x10);
			const uint32_t uv = *(const uint32_t *)(rec + 0x14);
			*(uint32_t *)(pk + 0x24) = uv;
			*(uint32_t *)(pk + 0x30) = uv >> 16;
			GteReadFLAG(h + 0x44);
			if (*(const uint32_t *)(h + 0x44) & 0x60000) continue;
			GteReadSXY012Split(pk + 8, pk + 0x14, pk + 0x20);
			GteLoadV0(verts + 4 * *(const uint16_t *)(rec + 0xA));
			GteRTPS();
			if (!QuadOnScreen(pk + 8, pk + 0x14, pk + 0x20, pk + 0x2C)) continue;
			GteStoreOTZ(h + 0x40);
			if (*(const int32_t *)(h + 0xC))
			{
				GteSetFarColour(h[8], h[9], h[0xA]);
				GteLoadRGB012(rec + 0x18, rec + 0x1C, rec + 0x20);
				GteSetIR0(*(const int32_t *)(h + 0xC));
				GteDPCT();
				GteStoreRGB012(pk + 0x10, pk + 0x1C, pk + 0x28);
				GteLoadRGBC(pk + 4);
				GteDPCS();
				GteStoreRGB2(pk + 4);
			}
			else
			{
				*(uint32_t *)(pk + 0x10) = *(const uint32_t *)(rec + 0x18);
				*(uint32_t *)(pk + 0x1C) = *(const uint32_t *)(rec + 0x1C);
				*(uint32_t *)(pk + 0x28) = *(const uint32_t *)(rec + 0x20);
			}
			ScrollV(h, pk, V);
			const uint32_t bucket = ot + 4 * (uint32_t)(*(const int32_t *)(h + 0x40) >> shift);
			pk = QuadWindows(h, bucket, pk, pk + 0x34);
		}
		ListCursor(h) = rec;
		return (uint32_t)pk;
	}

	static uint32_t ScrolledMesh(uint8_t *h, uint32_t ot, int shift, uint32_t cursor)
	{
		uint8_t *model = *(uint8_t **)h;
		ListCursor(h) = model + *(const uint32_t *)model + 0xC;
		cursor = ScrollFT4(h, ot, shift, cursor);
		ListCursor(h) += 0xC;
		return ScrollGT4(h, ot, shift, cursor);
	}

	static void __cdecl PrimCallback(prim::Layout *l, prim::Record *r, int arg_)
	{
		const PrimArg *arg = (const PrimArg *)arg_;
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
		const uint32_t flags = r->flags;
		if (flags & 0x400) RotationFromAnglesZYX(r->rot, &m);
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
		if (flags & 0x1000)
		{
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			int32_t o[3];
			GteSetRotMatrixCtrl(&CameraN());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			GteReadMAC123(o);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)o[0]);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)o[1]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)o[2]);
			MatrixMultiply(&CameraN(), &m);
		}
		else if (flags & 0x200)
		{
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)(int32_t)off[1]);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)(int32_t)off[0]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)(int32_t)off[2]);
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
		const int16_t obj = (int16_t)r->flags_lo;
		uint32_t cur;
		if (obj <= 1)
		{
			*(int16_t *)(h + 0x26) = 0;
			*(int16_t *)(h + 0x24) = 0;
			*(int16_t *)(h + 0x2A) = 0x100;
			*(int16_t *)(h + 0x28) = 0x100;
			*(int16_t *)(h + 0x2E) = 0;
			*(int16_t *)(h + 0x2C) = 0;
			*(int16_t *)(h + 0x30) = 0x100;
			*(int16_t *)(h + 0x32) = 0x80;
			*(uint16_t *)(h + 0x22) = (uint16_t)((uint8_t)((uint32_t)(uint8_t)arg->scroll << ((obj + 3) & 0x1F)) & 0x7F);
			cur = ScrolledMesh(h, RenderOT(), 2, PacketCursor());
		}
		else
		{
			const int32_t depth = arg->depth;
			*(int32_t *)(h + 0x18) = depth;
			if (obj == 2 || obj == 3) *(int32_t *)(h + 0x18) = depth + depth;
			cur = RenderPrimModel2(h, RenderOT(), 2, PacketCursor());
		}
		PacketCursor() = cur;
		FieldFree(0x58);
	}
}
}

namespace ff8fx
{
namespace lv271
{
	static void TargetBounds(uint8_t *entity, int16_t out[6])
	{
		BoundsBlock *b = (BoundsBlock *)FieldAlloc(0x20);
		uint8_t *anim = entity + 0x60;
		ComputeBonesWorldMatrices(anim, entity + 0x40);
		b->box[2] = 0x7FFF;
		b->box[1] = 0x7FFF;
		b->box[0] = 0x7FFF;
		b->box[5] = -0x7FFF;
		b->box[4] = -0x7FFF;
		b->box[3] = -0x7FFF;
		lv27x::ModelBounds((uint8_t *)b, anim);
		ff8fx::copy<(12)>(out, b->box);
		FieldFree(0x20);
		BuildBoneMatricesFromPose(anim);
	}

	static uint32_t __cdecl PlayTask(TaskNode *n)
	{
		PlayNode *p = (PlayNode *)n;
		PrimArg arg;
		arg.morph = TexBase() + MORPH;
		arg.depth = -(int32_t)*(const int16_t *)(p->entity + 0x26) >> 4;
		arg.scale[0] = p->scale[0];
		arg.scale[1] = p->scale[1];
		arg.scale[2] = p->scale[2];
		ff8fx::copy<(sizeof(arg.pos))>(arg.pos, p->anchor);
		arg.scaled = 1;
		arg.scroll = p->counter;
		arg.pad1E = 0;
		if (prim::play((prim::Layout *)p->layout, PrimCallback, (int)&arg, 0) == 0)
		{
			ApplyActionResultToTarget(Ctx()->actions[0].targets);
			return TASK_END;
		}
		p->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + ARENA_A;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + ARENA_B;
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QPlay(), (void *)TexBase(), NODE_BYTES, 1);
			}
			PlayNode *p = (PlayNode *)AddTaskToQueue(QPlay(), game_addr(ORIG_PlayTask));
			Memset32(&p->counter, 0, 0x130);
			p->action = (int16_t)r->action;
			p->entity = Entity(Ctx()->actions[p->action].targets[0]);
			int16_t box[6];
			TargetBounds(p->entity, box);
			GetDefaultEffectPosition(p->entity, p->anchor);
			p->anchor[1] = (int16_t)(((int32_t)box[4] + box[1]) >> 1);
			p->scale[1] = (int32_t)box[4] - box[1];
			const int32_t dx = (int32_t)box[3] - box[0];
			p->scale[0] = dx;
			const int32_t dz = (int32_t)box[5] - box[2];
			p->scale[2] = dz;
			if (dx < dz) p->scale[0] = dz;
			else p->scale[2] = dx;
			if (p->scale[1] > p->scale[0])
			{
				p->scale[2] = p->scale[1];
				p->scale[0] = p->scale[1];
			}
			DecodeModelPrimLayout(game_addr(MODEL_LAYOUT), p->layout, LAYOUT_BYTES);
			r->action++;
			BdPlaySE(SOUND(), 0, 0x80);
		}
		EffectCameraMatrix(&Camera(), &CameraN());
		int a;
		if (r->started) a = ExecuteTaskQueue(QPlay());
		else a = (int)n;
		if (r->started && a == 0) return TASK_END;
		r->counter++;
		return 0;
	}
}

	void register_mag271_lv_up()
	{
		register_port(lv271::ORIG_RootTask, (void *)lv271::RootTask, "271 RootTask", 271);
		register_port(lv271::ORIG_PlayTask, (void *)lv271::PlayTask, "271 PlayTask", 271);
	}
}

