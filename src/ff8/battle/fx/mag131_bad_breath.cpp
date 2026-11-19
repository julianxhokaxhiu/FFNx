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

// Effect 131: Bad Breath

#include "mag_common.h"
#include "mag_ice_common.h"

namespace ff8fx
{
namespace badbreath131
{
	using namespace eng;
	using namespace magc;
	using namespace icec;

	inline Mat4x3 &MeshFrame() { return var<Mat4x3>(game_addr(game::g131_MeshFrame)); }
	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g131_Caster)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g131_TexBase)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g131_CastContext)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g131_PacketCursor)); }
	static const void *FADE_COLOUR() { return (const void *)game_addr(game::g131_FadeColour); }
	inline TaskQueue *QSprites() { return (TaskQueue *)game_addr(game::g131_QSprites); }
	inline TaskQueue *QTargets() { return (TaskQueue *)game_addr(game::g131_QTargets); }
	inline TaskQueue *QEmitter() { return (TaskQueue *)game_addr(game::g131_QEmitter); }
	inline uint32_t RenderOT() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	static constexpr game::Id ORIG_MasterTask = game::MAG_131_BadBreath_MasterTask;
	static constexpr game::Id ORIG_EmitterTask = game::MAG_131_BadBreath_EmitterTask;
	static constexpr game::Id ORIG_TargetTask = game::MAG_131_BadBreath_TargetTask;
	static constexpr game::Id ORIG_SpriteTask = game::MAG_131_BadBreath_SpriteTask;
	static uint8_t *MESH1() { return (uint8_t *)game_addr(game::BadBreath_Mesh1); }
	static uint8_t *MESH2() { return (uint8_t *)game_addr(game::BadBreath_Mesh2); }
	static constexpr game::Id MODEL_Target = game::BadBreath_TargetLayout;
	static constexpr game::Id SEQ_Bubble = game::BadBreath_BubbleSeq;
	static const uint8_t *COLOURS() { return (const uint8_t *)game_addr(game::BadBreath_MeshColours); }
	static const void *TINT() { return (const void *)game_addr(game::BadBreath_TargetTint); }
	static const int16_t *SWIRL_TABLE() { return (const int16_t *)game_addr(game::BadBreath_SwirlTable); }
	static const void *SOUND_Breath() { return (const void *)game_addr(game::unk_121C978); }
	static const uint32_t VERTS = 0xA00;

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

#pragma pack(push, 1)
	struct MasterNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E;
		uint8_t started;
		uint32_t arena;
	};
	struct EmitterNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t count;
		uint8_t *ents[3];
		uint8_t idx[3];
		uint8_t pad1F;
		int16_t z;
		int16_t angle;
		int16_t phase;
		int16_t next;
		int32_t length;
		int16_t mouth[4];
	};
	struct TargetNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t idx;
		uint8_t slot;
		int16_t pos[4];
		uint32_t colour;
		uint8_t layout[0x128];
	};
	struct SpriteNode
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
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(MasterNode) == 0x14 && sizeof(EmitterNode) == 0x34 && sizeof(TargetNode) == 0x144 && sizeof(SpriteNode) == 0x18, "Bad Breath nodes");
	static_assert(sizeof(PrimArg) == 0xC, "Bad Breath prim block");

	static void NormalizeMesh(uint8_t *model)
	{
		const int32_t count = *(const int32_t *)(model + 4);
		int32_t mx = *(const int16_t *)(model + 0x1C), mn = mx;
		uint8_t *v = model + 0x18;
		for (int32_t i = count - 1; i; i--, v += 8)
		{
			const int32_t z = *(const int16_t *)(v + 4);
			if (mn > z) mn = z;
			else if (mx < z) mx = z;
			const int16_t x = *(const int16_t *)v;
			*(int16_t *)v = (int16_t)(x + (int16_t)(x >> 2));
			const int16_t y = *(const int16_t *)(v + 2);
			*(int16_t *)(v + 2) = (int16_t)(y + (int16_t)(y >> 2));
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

	static bool OffX(int16_t x) { return x < 0 || x > 0xA00; }
	static bool OffY(int16_t y) { return y < 0 || y > 0x6C0; }
	static uint8_t *&Cursor(uint8_t *h, int off) { return *(uint8_t **)(h + off); }

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

	static int16_t WalkX(int32_t t) { return (int16_t)(ComputeCos((int32_t)((uint32_t)shl32(t, 12) / 50)) >> 1); }

	static int32_t Growth(int32_t t) { return (uint32_t)t < 0x18 ? (int32_t)((uint32_t)shl32(t, 12) / 24) : 0x1000; }

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
		else if ((uint32_t)t > 0x2A)
		{
			const int32_t s = ComputeSin(((t << 7) - 0x1481) & 0x1FFFFF80);
			BlendColour(FADE_COLOUR(), h4, s, 0x1000 - s, h4);
		}
	}
}
}

namespace ff8fx
{
namespace badbreath131
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
		m.t[0] = (int32_t)r->pos[0] + arg->pos[0];
		m.t[1] = (int32_t)r->pos[1] + arg->pos[1];
		m.t[2] = (int32_t)r->pos[2] + arg->pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		if (!(*(const uint32_t *)r->scale == 0x10001000 && r->scale[2] == 0x1000))
		{
			const int32_t v[3] = { r->scale[0], r->scale[1], r->scale[2] };
			Scale3DMatrix(&m, v);
		}
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)(h + 0x1C) = 0x2000;
		*(int32_t *)(h + 0xC) = r->a;
		if (r->a != 0)
		{
			*(uint32_t *)(h + 0x1C) = 0x20C0;
			*(uint32_t *)(h + 8) = *(const uint32_t *)r->rgb;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void SpriteDraw(const SpriteNode *p)
	{
		uint8_t *h = AllocHeader(0xB4);
		TransformCameraByShadowRotation(p->pos, p->size, -((int32_t)p->size >> 2));
		*(uint32_t *)h = game_addr(SEQ_Bubble);
		*(uint16_t *)(h + 4) = p->counter;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl SpriteTask(TaskNode *n)
	{
		SpriteNode *p = (SpriteNode *)n;
		SpriteDraw(p);
		p->pos[1] = (int16_t)(p->pos[1] - 0x10);
		p->counter++;
		return (int16_t)p->counter < 0x12 ? 0 : TASK_END;
	}

	static uint32_t __cdecl TargetTask(TaskNode *n)
	{
		TargetNode *t = (TargetNode *)n;
		uint8_t *entity = Entity(t->slot);
		PrimArg arg;
		ff8fx::copy<(sizeof(arg.pos))>(arg.pos, t->pos);
		arg.morph = TexBase() + VERTS;
		prim::play((prim::Layout *)t->layout, PrimCallback, (int)&arg, 0);
		entity[1] |= 8;
		const int32_t c = t->counter;
		if ((uint32_t)c < 8)
		{
			const int32_t w = shl32(c, 9);
			BlendColour(TINT(), &t->colour, w, 0x1000 - w, entity + 0x28);
		}
		else if ((uint32_t)(c - 0x1E) <= 8)
		{
			const int32_t w = shl32(c - 0x1E, 9);
			BlendColour(TINT(), &t->colour, 0x1000 - w, w, entity + 0x28);
		}
		else if (t->counter >= 0 && t->counter <= 0x1E) *(uint32_t *)(entity + 0x28) = *(const uint32_t *)TINT();
		else *(uint32_t *)(entity + 0x28) = t->colour;
		if (t->counter <= 0x10)
		{
			SpriteNode *s = (SpriteNode *)AddTaskToQueue(QSprites(), game_addr(ORIG_SpriteTask));
			if (s)
			{
				const int32_t bone = (int32_t)((uint32_t)CrtRand() * (uint32_t)***(uint8_t ***)(entity + 0x64)) >> 15;
				GetEffectSpawnPosition(entity, bone, 0, s->pos);
				s->counter = 0;
				s->size = (int16_t)((CrtRand() & 0x1FF) + 0x400);
			}
		}
		if (t->counter == 0x28) ApplyActionResultToTarget(Ctx()->actions[0].targets + t->idx * TARGET_STRIDE);
		t->counter++;
		if (t->counter >= 0x2F)
		{
			*(uint16_t *)entity &= 0xF7FF;
			return TASK_END;
		}
		return 0;
	}

	static uint32_t __cdecl EmitterTask(TaskNode *n)
	{
		EmitterNode *e = (EmitterNode *)n;
		GetEffectSpawnPosition(Caster(), 0xB, 0x2000, e->mouth);
		const int32_t t = e->counter - 0xF;
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
			const int16_t x = WalkX(t);
			const int16_t z = e->z;
			if (t >= 0x19 && e->next < e->count)
			{
				uint8_t *target = e->ents[e->next];
				if ((int32_t)x >= (int32_t)*(const int16_t *)(target + 0x1C) - 1000)
				{
					TargetNode *p = (TargetNode *)AddTaskToQueue(QTargets(), game_addr(ORIG_TargetTask));
					if (p)
					{
						p->counter = 0;
						p->idx = e->idx[e->next];
						p->colour = *(const uint32_t *)(target + 0x28);
						p->slot = (uint8_t)((int32_t)((uint32_t)target - game_addr(game::BattleEntitySlotData)) / 0x9C);
						GetDefaultEffectPosition(target, p->pos);
						p->pos[1] = (int16_t)(*(const int16_t *)(target + 0x24) - 0xC8);
						DecodeModelPrimLayout(game_addr(MODEL_Target), p->layout, 0x128);
					}
					e->next++;
				}
			}
			const int32_t dx = (int32_t)x - e->mouth[0];
			const int32_t dy = -500 - (int32_t)e->mouth[1];
			const int32_t dz = (int32_t)z - e->mouth[2];
			e->length = Sqrt(mul32(dz, dz) + mul32(dy, dy) + mul32(dx, dx)) + 0x200;
			int16_t scale[9] = { 0x1000, 0, 0, 0, 0x1000, 0, 0, 0, (int16_t)e->length };
			MeshColour(t, h + 4);
			if ((uint32_t)t < 0x18) scale[8] = (int16_t)(mul32(scale[8], Growth(t)) >> 12);
			const int32_t v[3] = { dx, -dy, -dz };
			Mat4x3 basis;
			BuildOrthonormalBasis(v, &basis);
			MulMatrix3(&basis, scale);
			basis.t[0] = e->mouth[0];
			basis.t[1] = e->mouth[1];
			basis.t[2] = e->mouth[2];
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
			PacketCursor() = MeshDraw(h, RenderOT(), 2, PacketCursor());
			RotZ((int16_t)-e->angle, &rot);
			GteSetRotMatrix(&rot);
			GteZeroTrans();
			SwirlVertices(MESH2(), (int32_t)e->phase + 0x400, (int16_t *)verts);
			GteSetRotMatrix(&MeshFrame());
			GteSetTransVector(&MeshFrame());
			*(uint8_t **)h = MESH2();
			PacketCursor() = MeshDraw(h, RenderOT(), 2, PacketCursor());
			FieldFree(0x5C);
		}
		if (e->counter == 0x14) BdPlaySE(SOUND_Breath(), 0, 0x80);
		e->angle = (int16_t)(e->angle - 0x32);
		e->phase = (int16_t)(e->phase + 0xC8);
		if (e->counter >= 0x41) return TASK_END;
		e->counter++;
		return 0;
	}

	static uint32_t __cdecl MasterTask(TaskNode *n)
	{
		MasterNode *m = (MasterNode *)n;
		if (m->arena)
		{
			PacketCursor() = TexBase() + 0xF28;
			m->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x18F28;
			m->arena = 1;
		}
		if (m->counter == 1 && !m->started)
		{
			m->started = 1;
			InitTaskQueuePool(QEmitter(), (void *)(TexBase() + 0x9CC), 0x34, 1);
			InitTaskQueuePool(QTargets(), (void *)(TexBase() + 0x600), 0x144, 3);
			InitTaskQueuePool(QSprites(), (void *)TexBase(), 0x18, 0x40);
			EmitterNode *e = (EmitterNode *)AddTaskToQueue(QEmitter(), game_addr(ORIG_EmitterTask));
			Memset32(&e->counter, 0, 0xA);
			uint8_t *const b = (uint8_t *)e;
			e->count = (int16_t)Ctx()->actions[0].target_count;
			e->z = 0;
			for (int32_t i = 0; i < e->count; i++)
			{
				b[0x1C + i] = (uint8_t)i;
				uint8_t *ent = Entity(Ctx()->actions[0].targets[i * TARGET_STRIDE]);
				*(uint8_t **)(b + 0x10 + 4 * i) = ent;
				e->z = (int16_t)(e->z + *(const int16_t *)(ent + 0x20));
			}
			e->z = (int16_t)((int32_t)e->z / (int32_t)e->count);
			for (int32_t i = 0; i < e->count - 1; i++)
			{
				for (int32_t j = i + 1; j < e->count; j++)
				{
					uint8_t *a = *(uint8_t **)(b + 0x10 + 4 * i);
					uint8_t *c = *(uint8_t **)(b + 0x10 + 4 * j);
					if (*(const int16_t *)(a + 0x1C) > *(const int16_t *)(c + 0x1C))
					{
						*(uint8_t **)(b + 0x10 + 4 * i) = c;
						*(uint8_t **)(b + 0x10 + 4 * j) = a;
						const uint8_t x = b[0x1C + j];
						const uint8_t y = b[0x1C + i];
						b[0x1C + i] = x;
						b[0x1C + j] = y;
					}
				}
			}
			NormalizeMesh(MESH1());
			NormalizeMesh(MESH2());
		}
		int a0, a1, a2;
		if (m->started)
		{
			a0 = ExecuteTaskQueue(QEmitter());
			a1 = ExecuteTaskQueue(QTargets());
			a2 = ExecuteTaskQueue(QSprites());
		}
		else a0 = a1 = a2 = (int)n;
		if (m->started && a0 == 0 && a1 == 0 && a2 == 0) return TASK_END;
		m->counter++;
		return 0;
	}
}

	void register_mag131_bad_breath()
	{
		register_port(badbreath131::ORIG_MasterTask, (void *)badbreath131::MasterTask, "131 MasterTask", 131);
		register_port(badbreath131::ORIG_EmitterTask, (void *)badbreath131::EmitterTask, "131 EmitterTask", 131);
		register_port(badbreath131::ORIG_TargetTask, (void *)badbreath131::TargetTask, "131 TargetTask", 131);
		register_port(badbreath131::ORIG_SpriteTask, (void *)badbreath131::SpriteTask, "131 SpriteTask", 131);
	}
}

