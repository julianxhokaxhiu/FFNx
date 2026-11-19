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

// Effect 108: Berserk

#include "mag_common.h"

namespace ff8fx
{
namespace berserk108
{
	using namespace eng;
	using namespace magc;

	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::unk_2521AC4)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::unk_2521AC8)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::unk_2521AE4)); }
	inline TaskQueue *QParticles() { return (TaskQueue *)game_addr(game::unk_2521AE8); }
	inline TaskQueue *QSwirls() { return (TaskQueue *)game_addr(game::unk_2521AF8); }
	inline TaskQueue *QMains() { return (TaskQueue *)game_addr(game::unk_2521B08); }
	inline uint32_t RenderOT(uint32_t off) { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + off; }

	static constexpr game::Id ORIG_RootTask = game::MAG_108_BERSERK_RootTask;
	static constexpr game::Id ORIG_MainTask = game::MAG_108_BERSERK_MainVisual_Tick;
	static constexpr game::Id ORIG_SwirlTask = game::MAG_108_SwirlTask;
	static constexpr game::Id ORIG_ParticleTask = game::MAG_108_ParticleTask;
	static constexpr game::Id MODEL_Burst = game::unk_12E7BC0;
	static constexpr game::Id MODEL_Glow = game::unk_12E7010;
	static constexpr game::Id PART_Flags = game::unk_12EC330;
	static constexpr game::Id SEQ_Flash = game::unk_12E6DDC;
	static constexpr game::Id SEQ_Ember = game::unk_12E6E98;
	static constexpr game::Id SEQ_Spark = game::unk_12E6C48;
	static constexpr game::Id RAY_Colours = game::unk_12EC320;
	static constexpr game::Id RAY_Unused = game::unk_2521B28;
	static constexpr game::Id TEXWIN_Strip = game::unk_12EC310;
	static constexpr game::Id TEXWIN_Reset = game::unk_12EC318;
	static const void *SOUND_Berserk() { return (const void *)game_addr(game::unk_12EC30C); }

	inline int32_t ComputeCos(int32_t angle) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(angle); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline void MatrixMultiply3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void BuildOrthonormalBasis(const int32_t *v, Mat4x3 *m) { fn<void (__cdecl *)(const int32_t *, Mat4x3 *)>(game_addr(game::BuildOrthonormalBasis))(v, m); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteSetSXY012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSXY012))(a, b, c); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(int32_t *dst) { fn<void (__cdecl *)(int32_t *)>(game_addr(game::GTE_ReadMAC0))(dst); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t action;
		uint8_t started;
		uint32_t arena;
	};
	struct Ray
	{
		int16_t pad00;
		int16_t tilt;
		int16_t spin;
		int16_t alive;
		uint8_t col0[4];
		uint8_t dec0[4];
		uint8_t col1[4];
		uint8_t dec1[4];
		uint8_t col2[4];
		uint8_t dec2[4];
		int16_t speed;
		int16_t length;
		int16_t length_vel;
		int16_t length_acc;
		int16_t width;
		int16_t width_vel;
		int16_t width_acc;
		int16_t pad2E;
	};
	struct MainNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t action;
		uint8_t pad10[4];
		uint8_t *entity;
		int16_t spawn[4];
		int16_t pos[4];
		Ray rays[32];
		uint8_t burst[0x29C];
	};
	struct SwirlEntry
	{
		int16_t angle;
		int16_t z;
		int16_t radius;
		int16_t phase;
		uint32_t sxy0;
		int32_t otz0;
		uint32_t sxy1;
		int32_t otz1;
		int16_t bright;
		int16_t bright_vel;
		uint16_t shade;
		uint16_t pad1E;
	};
	struct SwirlNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t angle;
		int16_t pad12;
		int16_t pad14;
		int16_t amp;
		int32_t count;
		struct MainNode *owner;
		SwirlEntry trail[32];
	};
	struct ParticleNode
	{
		TaskNode hdr;
		uint8_t counter;
		int8_t accel;
		int16_t scale;
		int16_t pos[3];
		int16_t vel;
		uint32_t seq;
	};
	struct PrimArg
	{
		int16_t pos[4];
		uint32_t flags;
		uint32_t morph;
	};
	struct SwirlFrame
	{
		int32_t v[3];
		uint32_t pad1C;
		Mat4x3 basis;
		Mat4x3 roll;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x14 && sizeof(Ray) == 0x30 && sizeof(MainNode) == 0x8C4, "Berserk nodes");
	static_assert(sizeof(SwirlEntry) == 0x20 && sizeof(SwirlNode) == 0x420 && sizeof(ParticleNode) == 0x1C, "Berserk nodes");
	static_assert(sizeof(PrimArg) == 0x10 && sizeof(SwirlFrame) == 0x50, "Berserk blocks");

	static inline uint32_t Div15(uint32_t x) { return (uint32_t)(((uint64_t)x * 0x88888889u) >> 35); }

	static void CopyDwords(void *dst, const void *src, int32_t n)
	{
		uint32_t *d = (uint32_t *)dst;
		const uint32_t *s = (const uint32_t *)src;
		for (; n; n--) *d++ = *s++;
	}

	static void ZRotation(int16_t angle, Mat4x3 *m)
	{
		const int32_t s = ComputeSin(angle);
		const int32_t c = ComputeCos(angle);
		m->m[0][1] = (int16_t)-s;
		m->m[1][0] = (int16_t)s;
		m->m[0][0] = (int16_t)c;
		m->m[0][2] = 0;
		m->m[1][1] = (int16_t)c;
		m->m[1][2] = 0;
		m->m[2][0] = 0;
		m->m[2][1] = 0;
		m->m[2][2] = 0x1000;
	}

	static void SwirlPoint(int16_t *out, int32_t angle, int32_t z, int32_t r)
	{
		out[0] = (int16_t)(mul32(ComputeCos(angle), r) >> 14);
		out[1] = (int16_t)(mul32(ComputeSin(angle), r) >> 14);
		out[2] = (int16_t)((z >> 2) - z);
	}

	static void __cdecl BurstPart(prim::Layout *l, prim::Record *r, int arg_)
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
		RotationFromAngles(r->rot, &m);
		m.t[0] = (int32_t)r->pos[0] + arg->pos[0];
		m.t[1] = (int32_t)r->pos[1] + arg->pos[1];
		m.t[2] = (int32_t)r->pos[2] + arg->pos[2];
		const uint32_t *flags = (const uint32_t *)arg->flags;
		if (flags[(int16_t)r->index] & 0x10000000)
		{
			TransformVectorBy3x3Matrix(&Camera(), m.t, m.t);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
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
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void RaySpawn(int32_t max, Ray *rays, int32_t n, const uint32_t *c0, const uint32_t *d0, const uint32_t *c1, const uint32_t *d1,
		const void *p8, const void *p9, int32_t speed, int32_t length, int32_t length_vel, int32_t length_acc, int32_t width, int32_t width_vel, int32_t width_acc)
	{
		(void)p8;
		(void)p9;
		if (n <= 0) return;
		int32_t made = 0;
		for (;;)
		{
			Ray *e = rays;
			int32_t left = max;
			for (;;)
			{
				if (left-- == 0) return;
				if (e->alive == 0) break;
				e++;
			}
			e->alive = 1;
			e->tilt = (int16_t)CrtRand();
			e->spin = (int16_t)((CrtRand() & 0x7FF) - 0x400);
			if (CrtRand() & 1) e->speed = (int16_t)speed;
			else e->speed = (int16_t)-speed;
			made++;
			*(uint32_t *)e->col0 = *c0;
			*(uint32_t *)e->dec0 = *d0;
			*(uint32_t *)e->col1 = *c1;
			e->length = (int16_t)length;
			e->width = (int16_t)width;
			*(uint32_t *)e->dec1 = *d1;
			e->length_vel = (int16_t)length_vel;
			e->length_acc = (int16_t)length_acc;
			e->width_vel = (int16_t)width_vel;
			e->width_acc = (int16_t)width_acc;
			if (made >= n) return;
		}
	}

	static void RayDirection(const Ray *e, int16_t *out)
	{
		const int32_t s = ComputeSin(e->tilt);
		const int32_t c = ComputeCos(e->tilt);
		out[2] = (int16_t)-c;
		out[1] = (int16_t)(mul32(ComputeSin(e->spin), s) >> 12);
		out[0] = (int16_t)((int32_t)(0u - (uint32_t)mul32(ComputeCos(e->spin), s)) >> 12);
	}

	static uint32_t RayPrims(const Ray *e, int32_t width, int32_t base, int32_t length, const uint8_t *c0, const uint8_t *c1, const uint8_t *c2,
		uint32_t ot, int32_t shift, uint32_t cursor)
	{
		int16_t d[3];
		RayDirection(e, d);
		int16_t v[4];
		v[3] = 0;
		v[0] = (int16_t)(mul32(d[0], base) >> 12);
		v[1] = (int16_t)(mul32(d[1], base) >> 12);
		v[2] = (int16_t)(mul32(d[2], base) >> 12);
		GteLoadV0(v);
		GteRTPS();
		uint8_t *p = (uint8_t *)cursor;
		GteReadSXY2(p + 8);
		int32_t otz1;
		GteReadOTZ(&otz1);
		if (otz1 <= 0) return cursor;
		v[0] = (int16_t)(mul32(d[0], length) >> 12);
		v[1] = (int16_t)(mul32(d[1], length) >> 12);
		v[2] = (int16_t)(mul32(d[2], length) >> 12);
		GteLoadV0(v);
		GteRTPS();
		GteReadSXY2(p + 0x10);
		int32_t otz2;
		GteReadOTZ(&otz2);
		if (otz2 <= 0) return cursor;
		const int32_t sx1 = *(const int16_t *)(p + 8);
		const int32_t sy2 = *(const int16_t *)(p + 0x12);
		const int32_t dx = *(const int16_t *)(p + 0x10) - sx1;
		const int32_t dy = *(const int16_t *)(p + 0xA) - sy2;
		const int32_t len = Sqrt(mul32(dy, dy) + mul32(dx, dx));
		if (len == 0) return cursor;
		uint8_t *q = p + 0x1C;
		int32_t px = shl32(dy, 12) / len;
		px = mul32(px, width) / otz2;
		int32_t py = shl32(dx, 12) / len;
		px >>= 4;
		py = mul32(py, width) / otz2;
		const int16_t tx = *(const int16_t *)(p + 0x10);
		*(int16_t *)(q + 0x18) = (int16_t)(tx + (int16_t)px);
		*(int16_t *)(q + 0x20) = (int16_t)(tx - (int16_t)px);
		py >>= 4;
		const int16_t ty = *(const int16_t *)(p + 0x12);
		*(int16_t *)(q + 0x1A) = (int16_t)(ty + (int16_t)py);
		*(int16_t *)(q + 0x22) = (int16_t)(ty - (int16_t)py);
		int16_t h = (int16_t)(((int32_t)*(const int16_t *)(q + 0x18) + *(const int16_t *)(p + 8)) >> 1);
		*(int16_t *)(q + 8) = h;
		*(int16_t *)(p + 0x10) = h;
		h = (int16_t)(((int32_t)*(const int16_t *)(q + 0x20) + *(const int16_t *)(p + 8)) >> 1);
		*(int16_t *)(q + 0x10) = h;
		*(int16_t *)(p + 0x18) = h;
		h = (int16_t)(((int32_t)*(const int16_t *)(q + 0x1A) + *(const int16_t *)(p + 0xA)) >> 1);
		*(int16_t *)(q + 0xA) = h;
		*(int16_t *)(p + 0x12) = h;
		h = (int16_t)(((int32_t)*(const int16_t *)(q + 0x22) + *(const int16_t *)(p + 0xA)) >> 1);
		*(int16_t *)(q + 0x12) = h;
		*(int16_t *)(p + 0x1A) = h;
		*(uint32_t *)(p + 4) = *(const uint32_t *)c0;
		const uint32_t k1 = *(const uint32_t *)c1;
		*(uint32_t *)(q + 0xC) = k1;
		*(uint32_t *)(q + 4) = k1;
		*(uint32_t *)(p + 0x14) = k1;
		*(uint32_t *)(p + 0xC) = k1;
		const uint32_t k2 = *(const uint32_t *)c2;
		*(uint32_t *)(q + 0x1C) = k2;
		*(uint32_t *)(q + 0x14) = k2;
		*(uint32_t *)p = 0x6000000;
		p[7] = 0x32;
		*(uint32_t *)q = 0x8000000;
		q[7] = 0x3A;
		const int32_t z = ((otz1 + otz2) / 2) >> shift;
		const uint32_t bucket = ot + (uint32_t)z * 4;
		InsertPrimAutoDepth(bucket, p);
		InsertPrimAutoDepth(bucket, q);
		return (uint32_t)(q + 0x24);
	}

	static inline uint8_t FadeByte(uint8_t c, uint8_t d) { const int32_t v = (int32_t)c - d; return (uint8_t)(v < 0 ? 0 : v); }

	static void RayUpdate(Ray *e)
	{
		e->spin = (int16_t)(e->spin + e->speed);
		e->col0[0] = FadeByte(e->col0[0], e->dec0[0]);
		e->col0[1] = FadeByte(e->col0[1], e->dec0[1]);
		e->col0[2] = FadeByte(e->col0[2], e->dec0[2]);
		e->col1[0] = FadeByte(e->col1[0], e->dec1[0]);
		e->col1[1] = FadeByte(e->col1[1], e->dec1[1]);
		e->col1[2] = FadeByte(e->col1[2], e->dec1[2]);
		e->col2[0] = FadeByte(e->col2[0], e->dec2[0]);
		e->col2[1] = FadeByte(e->col2[1], e->dec2[1]);
		e->col2[2] = FadeByte(e->col2[2], e->dec2[2]);
		if (!e->col0[0] && !e->col0[1] && !e->col0[2] && !e->col1[0] && !e->col1[1] && !e->col1[2] && !e->col2[0] && !e->col2[1] && !e->col2[2])
		{
			e->alive = 0;
			return;
		}
		e->length_vel = (int16_t)(e->length_vel + e->length_acc);
		e->length = (int16_t)(e->length + e->length_vel);
		if (e->length <= 0)
		{
			e->alive = 0;
			return;
		}
		e->width_vel = (int16_t)(e->width_vel + e->width_acc);
		e->width = (int16_t)(e->width + e->width_vel);
		if (e->width <= 0) e->alive = 0;
	}

	static uint32_t RayDraw(int32_t max, Ray *rays, const int16_t *pos, int32_t *alive, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		int32_t drawn = 0;
		if (max == 0)
		{
			*alive = 0;
			return cursor;
		}
		Mat4x3 m;
		Ray *e = rays;
		for (int32_t k = max; k; k--, e++)
		{
			if (e->alive == 0) continue;
			if (drawn++ == 0)
			{
				CopyDwords(&m, &Camera(), 5);
				GteSetRotMatrixCtrl(&m);
				GteLoadV0(pos);
				GteMVMVA_RotV0();
				GteReadMAC123(m.t);
				m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)Camera().t[0]);
				m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)Camera().t[1]);
				m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)Camera().t[2]);
				GteSetRotMatrix(&m);
				GteSetTransVector(&m);
			}
			cursor = RayPrims(e, e->width >> 3, 0, e->length >> 3, e->col0, e->col1, e->col2, ot, shift, cursor);
			RayUpdate(e);
		}
		*alive = drawn;
		return cursor;
	}

	static int32_t SwirlPush(int32_t max, int32_t n, SwirlEntry *t, int32_t angle, int32_t z, int32_t radius, int32_t phase, int32_t bright,
		int32_t bright_vel, int32_t shade)
	{
		if (n < max)
		{
			uint32_t *d = (uint32_t *)t;
			for (int32_t k = n * 8; k > 0; k--) d[k + 7] = d[k - 1];
			n++;
		}
		t[0].angle = (int16_t)angle;
		t[0].z = (int16_t)z;
		t[0].radius = (int16_t)radius;
		t[0].phase = (int16_t)phase;
		t[0].bright = (int16_t)bright;
		t[0].bright_vel = (int16_t)bright_vel;
		t[0].shade = (uint16_t)shade;
		return n;
	}

	static void SwirlFade(int32_t n, SwirlEntry *t, int32_t d)
	{
		for (int32_t k = 0; k < n; k++)
		{
			SwirlEntry &e = t[k];
			if (e.bright < 0x10) continue;
			e.bright_vel = (int16_t)(e.bright_vel + d);
			e.bright = (int16_t)(e.bright + (int16_t)(e.bright_vel >> 3));
			if (e.bright < 0x10) e.bright = 0x10;
			else if (e.bright > 0xFF) e.bright = 0xFF;
		}
	}

	static uint32_t TexWindow(uint32_t rect)
	{
		uint32_t c = (*(const uint8_t *)(rect + 2) & 0xF8) | 0xFFFE2000u;
		c = (c << 5) | (*(const uint32_t *)rect & 0xF8);
		c = (c << 5) | (~((uint32_t)*(const uint16_t *)(rect + 6) - 1) & 0xF8);
		c = (c << 2) | ((uint32_t)((int32_t)~(*(const uint32_t *)(rect + 4) - 1) >> 3) & 0x1F);
		return c;
	}

	static uint32_t SwirlStrip(int32_t *pn, SwirlEntry *t, int32_t shade, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		int32_t n = *pn;
		if (n < 2)
		{
			*pn = n;
			return cursor;
		}
		if (t[n - 1].bright <= 0x10 && t[n - 2].bright <= 0x10)
		{
			n--;
			if (n < 2)
			{
				*pn = n;
				return cursor;
			}
		}
		for (int32_t k = 0; k < n; k++)
		{
			SwirlEntry &e = t[k];
			int16_t a[4], b[4];
			a[3] = 0;
			b[3] = 0;
			SwirlPoint(a, (int32_t)e.angle - 0x80, e.z, e.radius);
			GteLoadV0(a);
			GteRTPS();
			GteReadSXY2(&e.sxy0);
			GteReadOTZ(&e.otz0);
			SwirlPoint(b, (int32_t)e.angle + 0x80, e.z, e.radius);
			GteLoadV0(b);
			GteRTPS();
			GteReadSXY2(&e.sxy1);
			GteReadOTZ(&e.otz1);
			const int32_t s = ComputeSin(e.phase);
			e.phase = (int16_t)(e.phase + 0x200);
			e.angle = (int16_t)(e.angle + (int16_t)(s >> 6));
		}
		shift += 2;
		uint8_t *p = (uint8_t *)cursor;
		for (int32_t k = 0; k < n - 1; k++)
		{
			const SwirlEntry &a = t[k], &b = t[k + 1];
			if ((int16_t)a.otz0 <= 0 || (int16_t)a.otz1 <= 0 || (int16_t)b.otz0 <= 0 || (int16_t)b.otz1 <= 0) continue;
			uint8_t *q = p;
			p += 0x34;
			uint32_t u = (uint32_t)(shade + a.shade);
			uint32_t u2 = u + 0xF;
			if ((int32_t)(u2 | u) > 0xFF)
			{
				u -= 0x80;
				u2 -= 0x80;
			}
			*(uint32_t *)(q + 0x18) = u | 0xB9FF00;
			*(uint32_t *)(q + 0xC) = u | 0x3D94C000;
			*(uint32_t *)(q + 8) = a.sxy0;
			*(uint32_t *)(q + 0x14) = a.sxy1;
			int32_t mac1, mac2;
			GteSetSXY012(a.sxy0, a.sxy1, b.sxy0);
			GteNCLIP();
			GteReadMAC0(&mac1);
			GteSetSXY012(a.sxy1, b.sxy1, b.sxy0);
			GteNCLIP();
			GteReadMAC0(&mac2);
			const int32_t s1 = mac1 > 0, s2 = mac2 > 0;
			if (s1 == s2)
			{
				*(uint32_t *)(q + 0x20) = b.sxy0;
				*(uint32_t *)(q + 0x2C) = b.sxy1;
				*(uint32_t *)(q + 0x24) = u2 | 0xC000;
				u2 |= 0xFF00;
			}
			else
			{
				*(uint32_t *)(q + 0x20) = b.sxy1;
				*(uint32_t *)(q + 0x2C) = b.sxy0;
				*(uint32_t *)(q + 0x24) = u2 | 0xFF00;
				u2 |= 0xC000;
			}
			*(uint32_t *)(q + 0x30) = u2;
			uint32_t c = (uint32_t)(int32_t)a.bright;
			const uint32_t ca = (((c << 8) | c) << 8) | c;
			*(uint32_t *)(q + 0x10) = ca;
			*(uint32_t *)(q + 4) = ca;
			c = (uint32_t)(int32_t)b.bright;
			const uint32_t cb = (((c << 8) | c) << 8) | c;
			*(uint32_t *)q = 0xC000000;
			q[7] = 0x3E;
			*(uint32_t *)(q + 0x28) = cb;
			*(uint32_t *)(q + 0x1C) = cb;
			const int32_t z = ((int32_t)(int16_t)b.otz1 + (int16_t)b.otz0 + (int16_t)a.otz1 + (int16_t)a.otz0) >> shift;
			uint8_t *w = p;
			p += 0xC;
			*(uint32_t *)w = 0x2000000;
			const uint32_t bucket = ot + (uint32_t)z * 4;
			*(uint32_t *)(w + 4) = TexWindow(game_addr(TEXWIN_Strip));
			*(uint32_t *)(w + 8) = 0;
			InsertPrimAutoDepth(bucket, w);
			InsertPrimAutoDepth(bucket, q);
			w = p;
			p += 0xC;
			*(uint32_t *)w = 0x2000000;
			*(uint32_t *)(w + 4) = TexWindow(game_addr(TEXWIN_Reset));
			*(uint32_t *)(w + 8) = 0;
			InsertPrimAutoDepth(bucket, w);
		}
		*pn = n;
		return (uint32_t)p;
	}

	static uint32_t SwirlDraw(int32_t *pn, SwirlEntry *t, int32_t d, int32_t shade, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		SwirlFade(*pn, t, d);
		return SwirlStrip(pn, t, shade, ot, shift, cursor);
	}

	static void SwirlAnchor(const int16_t *pos, SwirlFrame *f)
	{
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteLoadV0(pos);
		GteMVMVA_RotV0Tr();
		GteReadMAC123(f->basis.t);
	}

	static void SwirlBasis(SwirlFrame *f)
	{
		f->v[0] = (int32_t)(0u - (uint32_t)f->basis.t[0]);
		f->v[1] = (int32_t)(0u - (uint32_t)f->basis.t[1]);
		f->v[2] = f->basis.t[2];
		BuildOrthonormalBasis(f->v, &f->basis);
		ZRotation(0, &f->roll);
		MatrixMultiply3(&f->basis, &f->roll);
		GteSetRotMatrix(&f->basis);
		GteSetTransVector(&f->basis);
	}

	static void FlashDraw(const int16_t *pos, int32_t e)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		TransformCameraByShadowRotation(pos, 0x2000, (int32_t)0xFFFFF800);
		*(uint32_t *)h = game_addr(SEQ_Flash);
		*(int16_t *)(h + 4) = (int16_t)e;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static inline int32_t GlowScale(int32_t e) { return ComputeSin((int32_t)Div15((uint32_t)shl32(e, 10))) + 0x400; }
	static inline int32_t GlowFade(int32_t e) { return (int32_t)(Div15((uint32_t)shl32(e, 11)) + 0x800); }

	static void GlowDraw(const int16_t *pos, int32_t scale, int32_t fade)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		Mat4x3 m = {};
		m.m[0][0] = (int16_t)scale;
		m.m[1][1] = (int16_t)scale;
		m.m[2][2] = (int16_t)scale;
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteLoadV0(pos);
		GteMVMVA_RotV0Tr();
		GteReadMAC123(m.t);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)h = game_addr(MODEL_Glow);
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(int32_t *)(h + 0xC) = fade;
		*(uint32_t *)(h + 0x1C) = 0xF0;
		PacketCursor() = RenderPrimModel(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void ParticleDraw(const ParticleNode *p)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		TransformCameraByShadowRotation(p->pos, (int32_t)(((uint32_t)h & 0xFFFF0000u) | (uint16_t)p->scale), -((int32_t)p->scale >> 2));
		*(uint32_t *)h = p->seq;
		*(int16_t *)(h + 4) = (int16_t)(int8_t)p->counter;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(0x44), 2, PacketCursor());
		FieldFree(0xB4);
	}
}
}

namespace ff8fx
{
namespace berserk108
{
	static uint32_t __cdecl ParticleTask(TaskNode *n)
	{
		ParticleNode *p = (ParticleNode *)n;
		ParticleDraw(p);
		if (p->seq == game_addr(SEQ_Ember))
		{
			p->vel = (int16_t)(p->vel + p->accel);
			p->pos[1] = (int16_t)(p->pos[1] + p->vel);
			p->counter++;
			return (int8_t)p->counter < 0xE ? 0 : TASK_END;
		}
		p->counter++;
		return (int8_t)p->counter < 0xF ? 0 : TASK_END;
	}

	static uint32_t __cdecl SwirlTask(TaskNode *n)
	{
		SwirlNode *s = (SwirlNode *)n;
		SwirlFrame f = {};
		SwirlAnchor(s->owner->pos, &f);
		for (int32_t i = 0; i < 4; i++)
		{
			if (s->counter > 6) continue;
			const int32_t e = i + s->counter * 4;
			const int32_t x = shl32(e, 12);
			int32_t q = (int32_t)(((int64_t)x * 0x2AAAAAAB) >> 32) >> 2;
			q += (int32_t)((uint32_t)q >> 31);
			const int32_t w = 0x1000 - q;
			const int32_t z = mul32(f.basis.t[2], w) >> 12;
			const int32_t radius = (mul32(ComputeSin(w >> 1), s->amp) >> 12) + w;
			const int32_t shade = (7 - (e & 7)) << 4;
			const int32_t count = SwirlPush(0x20, s->count, s->trail, s->angle, z, radius, w, 0x10, 0x100, shade);
			s->angle = (int16_t)(s->angle + 0x20);
			s->count = count;
		}
		SwirlBasis(&f);
		PacketCursor() = SwirlDraw(&s->count, s->trail, -0x28, shl32(s->counter, 5), RenderOT(0x44), 2, PacketCursor());
		s->counter++;
		if (s->counter > 8 && s->count < 2) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl MainTask(TaskNode *n)
	{
		MainNode *m = (MainNode *)n;
		const int32_t c0 = m->counter;
		if ((uint32_t)c0 < 4)
		{
			SwirlNode *s = (SwirlNode *)AddTaskToQueue(QSwirls(), game_addr(ORIG_SwirlTask));
			if (s)
			{
				Memset32(&s->counter, 0, 0x105);
				s->owner = m;
				s->angle = (int16_t)(shl32(c0, 10) + 0x200);
				s->pad12 = 0;
				s->amp = 0x800;
			}
		}
		int32_t e = m->counter - 0x11;
		if ((uint32_t)e < 6)
		{
			FlashDraw(m->pos, e);
		}
		e = m->counter - 0x11;
		if ((uint32_t)e < 0xF)
		{
			GlowDraw(m->pos, GlowScale(e), GlowFade(e));
		}
		e = m->counter - 0x11;
		if ((uint32_t)e < 0xF)
		{
			int32_t angle = CrtRand();
			for (int32_t k = 3; k; k--)
			{
				const int32_t dist = (CrtRand() & 0xFF) + 0x96;
				ParticleNode *p = (ParticleNode *)AddTaskToQueue(QParticles(), game_addr(ORIG_ParticleTask));
				if (!p) continue;
				Memset32(&p->counter, 0, 4);
				p->seq = game_addr(SEQ_Ember);
				p->scale = 0x800;
				p->pos[0] = (int16_t)((mul32(ComputeSin(angle), dist) >> 12) + m->spawn[0]);
				p->pos[2] = (int16_t)((mul32(ComputeCos(angle), dist) >> 12) + m->spawn[2]);
				const int32_t r1 = CrtRand();
				p->pos[1] = (int16_t)(m->spawn[1] - (int16_t)(r1 >> 5));
				const int32_t r2 = CrtRand();
				angle += 0x555;
				p->accel = (int8_t)(uint8_t)(0xFE - (r2 & 0xF));
			}
		}
		e = m->counter - 9;
		if ((uint32_t)e < 8 && !(e & 1))
		{
			ParticleNode *p = (ParticleNode *)AddTaskToQueue(QParticles(), game_addr(ORIG_ParticleTask));
			if (p)
			{
				Memset32(&p->counter, 0, 4);
				p->seq = game_addr(SEQ_Spark);
				p->scale = 0x1000;
				const int32_t r1 = CrtRand();
				p->pos[0] = (int16_t)((r1 & 0x1FF) + m->pos[0] - 0x100);
				const int32_t r2 = CrtRand();
				p->pos[1] = (int16_t)((r2 & 0x1FF) + m->pos[1] - 0x100);
				const int32_t r3 = CrtRand();
				p->pos[2] = (int16_t)((r3 & 0x1FF) + m->pos[2] - 0x100);
			}
		}
		if ((uint32_t)(m->counter - 0xA) < 8)
		{
			const uint32_t *col = (const uint32_t *)game_addr(RAY_Colours);
			RaySpawn(0x20, m->rays, 8, col, col + 1, col + 2, col + 3, (const void *)game_addr(RAY_Unused), (const void *)(game_addr(RAY_Unused) + 4),
				0x64, 0xA0, 0xB40, -0x20, 0xA0, 0x40, -0x14);
		}
		if (m->counter == 0x11) ScreenFadeTask(0, 1, 1, 0x80);
		if ((uint32_t)(m->counter - 0x11) < 0x24)
		{
			PrimArg arg;
			*(uint32_t *)&arg.pos[0] = *(const uint32_t *)&m->spawn[0];
			*(uint32_t *)&arg.pos[2] = *(const uint32_t *)&m->spawn[2];
			arg.flags = game_addr(PART_Flags);
			arg.morph = TexBase() + 0x52CC;
			prim::play((prim::Layout *)m->burst, BurstPart, (int)&arg, 0);
		}
		int32_t alive;
		PacketCursor() = RayDraw(0x20, m->rays, m->pos, &alive, RenderOT(0x44), 2, PacketCursor());
		if (m->counter == 0)
		{
			int16_t pos[4];
			GetDefaultEffectPosition(m->entity, pos);
			BdPlaySE3D(SOUND_Berserk(), 0x101, pos);
		}
		if (m->counter == 0x2B) ApplyActionResultToTarget(Ctx()->actions[m->action].targets);
		if (m->counter >= 0x35 && alive == 0) return TASK_END;
		m->counter++;
		return 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->arena)
		{
			PacketCursor() = TexBase() + 0x554C;
			r->arena = 0;
		}
		else
		{
			PacketCursor() = TexBase() + 0x1D54C;
			r->arena = 1;
		}
		if (r->counter == 1)
		{
			if (!r->started)
			{
				r->started = 1;
				InitTaskQueuePool(QMains(), (void *)TexBase(), 0x8C4, 3);
				InitTaskQueuePool(QSwirls(), (void *)(TexBase() + 0x1A4C), 0x420, 0xC);
				InitTaskQueuePool(QParticles(), (void *)(TexBase() + 0x4BCC), 0x1C, 0x40);
			}
			ActionData *acts = Ctx()->actions;
			if (r->action <= acts[0].last_action)
			{
				uint8_t *entity = Entity(acts[r->action].targets[0]);
				MainNode *pool = (MainNode *)TexBase();
				bool busy = false;
				for (int k = 0; k < 3; k++)
					if ((*(const uint8_t *)&pool[k].hdr.flags & 1) && pool[k].entity == entity && pool[k].counter <= 0x2B)
					{
						busy = true;
						break;
					}
				if (busy) r->counter = 0;
				else
				{
					MainNode *m = (MainNode *)AddTaskToQueue(QMains(), game_addr(ORIG_MainTask));
					Memset32(&m->counter, 0, 0x22E);
					m->action = (int16_t)(uint16_t)r->action;
					m->entity = entity;
					GetEffectSpawnPosition(entity, 0xF1, 0, m->spawn);
					m->spawn[1] = *(const int16_t *)(entity + 0x24);
					GetDefaultEffectPosition(m->entity, m->pos);
					DecodeModelPrimLayout(game_addr(MODEL_Burst), m->burst, 0x29C);
					r->action++;
				}
			}
		}
		int a, b, c;
		if (r->started)
		{
			a = ExecuteTaskQueue(QMains());
			b = ExecuteTaskQueue(QSwirls());
			c = ExecuteTaskQueue(QParticles());
		}
		else
		{
			a = (int)n;
			b = (int)n;
			c = (int)n;
		}
		if (r->started && a == 0 && b == 0 && c == 0) return TASK_END;
		r->counter++;
		if (r->counter >= 10) r->counter = 0;
		return 0;
	}
}

	void register_mag108_berserk()
	{
		register_port(berserk108::ORIG_RootTask, (void *)berserk108::RootTask, "B108 RootTask", 108);
		register_port(berserk108::ORIG_MainTask, (void *)berserk108::MainTask, "B108 MainTask", 108);
		register_port(berserk108::ORIG_SwirlTask, (void *)berserk108::SwirlTask, "B108 SwirlTask", 108);
		register_port(berserk108::ORIG_ParticleTask, (void *)berserk108::ParticleTask, "B108 ParticleTask", 108);
	}
}

