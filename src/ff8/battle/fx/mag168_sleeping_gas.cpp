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

// Effect 168: Sleeping Gas

#include "mag_common.h"

namespace ff8fx
{
namespace gas168
{
	using namespace eng;
	using namespace magc;

	template<typename T> inline T &AT(uint32_t p, int32_t o) { return *(T *)(p + o); }
	inline int16_t &S16(uint32_t p, int32_t o) { return AT<int16_t>(p, o); }
	inline uint16_t &U16(uint32_t p, int32_t o) { return AT<uint16_t>(p, o); }
	inline int32_t &S32(uint32_t p, int32_t o) { return AT<int32_t>(p, o); }
	inline uint32_t &U32(uint32_t p, int32_t o) { return AT<uint32_t>(p, o); }
	inline uint8_t &U8(uint32_t p, int32_t o) { return AT<uint8_t>(p, o); }
	inline uint32_t P(const void *p) { return (uint32_t)p; }

	inline TaskQueue *EffectQueue() { return (TaskQueue *)game_addr(game::g_Mag168_EffectQueue); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_Mag168_Ctx)); }
	inline uint32_t &CasterSlot() { return var<uint32_t>(game_addr(game::g_Mag168_CasterSlot)); }
	inline int16_t *Centre() { return (int16_t *)game_addr(game::g_Mag168_CloudCentre); }
	inline int16_t *CasterAnchor() { return (int16_t *)game_addr(game::g_Mag168_CasterAnchor); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g_Mag168_MagicBuffer)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_Mag168_PacketCursor)); }
	static constexpr game::Id CLOUDS = game::g_Mag168_Clouds;
	static constexpr game::Id OFFSETS = game::g_Mag168_WaveOffsets;
	static constexpr game::Id VERTS = game::g_Mag168_CloudVerts;
	static constexpr game::Id PAIRS = game::g_Mag168_VertexPairs;
	static constexpr game::Id PAIRS_END = game::g_Mag168_MagicBuffer;
	static constexpr game::Id POOL_A = game::g_Mag168_TargetSparkles, POOL_A_END = game::g_Mag168_EffectQueue;
	static constexpr game::Id POOL_B = game::g_Mag168_CasterSparkles, POOL_B_END = game::g_Mag168_TargetSparkles;
	static const int32_t WAVE = 0x3D;
	static const uint32_t CLOUD_VERTS = 0x3D0;

	static constexpr game::Id ORIG_Root = game::MAG_168_SLEEPING_GAS_RootTask;
	static constexpr game::Id ORIG_Director = game::MAG_168_SLEEPING_GAS_DirectorTask;
	static constexpr game::Id ORIG_Gas = game::MAG_168_SLEEPING_GAS_GasCloudTask;
	static constexpr game::Id ORIG_SparkTarget = game::MAG_168_SLEEPING_GAS_TargetSparkleTask;
	static constexpr game::Id ORIG_SparkCaster = game::MAG_168_SLEEPING_GAS_CasterSparkleTask;
	static constexpr game::Id MODEL_Gas = game::unk_D6EDE4;
	static constexpr game::Id MODEL_GasVerts = game::unk_D6EDEC;
	static constexpr game::Id SEQ_Sparkle = game::unk_D6BDE8;
	static const int32_t *AXIS_Model() { return (const int32_t *)game_addr(game::unk_D6F788); }
	static const void *SOUND_Gas() { return (const void *)game_addr(game::unk_D6F77C); }

	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	inline void GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t mode, void *out) { fn<void (__cdecl *)(uint8_t *, int32_t, int32_t, void *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, mode, out); }
	inline void CalculateCenterPosition(uint32_t mask, int16_t *out) { fn<void (__cdecl *)(uint32_t, int16_t *)>(game_addr(game::CalculateCenterPosition))(mask, out); }
	inline int32_t Normalize(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void GteSetFarColor(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadV012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadSXY012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteLoadV0u(uint32_t v) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadV0))(v); }
	inline void GteReadSXY2u(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadSXY2))(dst); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZ32(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteLoadRGBC(uint32_t c) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(uint32_t c) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreRGB2))(c); }

#pragma pack(push, 1)
	struct RootNode { TaskNode hdr; uint16_t counter; uint16_t pad; };
	struct DirectorNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t clouds;
		uint8_t pad10[0x14];
	};
	struct GasNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t cloud;
		int16_t pos[3];
		int16_t pad16;
		uint8_t pad18[4];
		int16_t width;
		uint8_t pad1E[6];
	};
	struct SparkNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t mask;
		int16_t pos[4];
		uint8_t pad18[0xC];
	};
	struct Cloud
	{
		int32_t count;
		int32_t step;
		int16_t angle_a;
		int16_t speed_a;
		int16_t angle_a0;
		int16_t amp_a;
		int16_t amp_a_step;
		int16_t angle_b;
		int16_t amp_b_step;
		int16_t angle_b0;
		int16_t amp_b;
		int16_t pad1A;
	};
	struct Spark
	{
		int32_t owner;
		int16_t frame;
		int16_t scale;
		int16_t pos[3];
		int16_t fade;
		int16_t vel[3];
		int16_t fade_len;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10, "root node is 0x10 bytes");
	static_assert(sizeof(DirectorNode) == 0x24 && sizeof(GasNode) == 0x24 && sizeof(SparkNode) == 0x24, "Sleeping Gas nodes are 0x24 bytes");
	static_assert(sizeof(Cloud) == 0x1C && sizeof(Spark) == 0x18, "cloud record / sparkle entry");

	inline Cloud *CloudRec(int32_t i) { return (Cloud *)(game_addr(CLOUDS) + (uint32_t)(i * 0x1C)); }
	inline int32_t *CloudOffsets(int32_t i) { return (int32_t *)(game_addr(OFFSETS) + (uint32_t)(i * WAVE * 8)); }
	inline uint32_t CloudVerts(int32_t i) { return game_addr(VERTS) + (uint32_t)(i * WAVE * 2 * 8); }

	static int32_t Div61(int32_t x)
	{
		int32_t q = (int32_t)(((int64_t)x * 0x4325C53F) >> 32) >> 4;
		return q + (int32_t)((uint32_t)q >> 31);
	}
	static int32_t Div1600(int32_t x)
	{
		int32_t q = (int32_t)(((int64_t)x * 0x51EB851F) >> 32) >> 9;
		return q + (int32_t)((uint32_t)q >> 31);
	}

	static uint32_t QuadList(uint32_t hdr, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint32_t pk = cursor;
		const uint32_t list = U32(hdr, 0x20);
		const int32_t count = S32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vtx = U32(hdr, 4);
		U32(hdr, 0x20) = rec;
		for (int32_t n = count; n > 0; n--, rec += 0x18)
		{
			GteLoadV012(vtx + (uint32_t)U16(rec, 4) * 4, vtx + (uint32_t)U16(rec, 6) * 4, vtx + (uint32_t)U16(rec, 8) * 4);
			GteRTPT();
			const uint32_t fl = U32(hdr, 0x1C);
			const uint32_t w0 = U32(rec, 0);
			U32(pk, 0) = 0x9000000;
			U32(pk, 4) = w0;
			if (fl & 1) U32(pk, 4) = w0 | 0x2000000;
			if (fl & 4) U32(pk, 4) = U32(pk, 4) & 0xFDFFFFFF;
			const uint32_t toff = U32(hdr, 0x18);
			U32(pk, 0xC) = U32(rec, 0xC) + toff;
			const uint32_t w1c = (toff << 16) + toff + U32(rec, 0x14);
			U32(pk, 0x1C) = w1c;
			U32(pk, 0x14) = U32(rec, 0x10) + toff;
			U32(pk, 0x24) = w1c >> 16;
			GteReadFLAG(hdr + 0x30);
			if (U32(hdr, 0x30) & 0x60000) continue;
			GteNCLIP();
			const uint32_t f2 = U32(hdr, 0x1C);
			if (f2 & 0x400) U16(pk, 0x16) = (uint16_t)(U16(pk, 0x16) + U16(hdr, 0x10));
			else if (f2 & 0x100) U16(pk, 0x16) = U16(hdr, 0x10);
			if (f2 & 0x800) U16(pk, 0xE) = (uint16_t)(U16(pk, 0xE) + U16(hdr, 0x14));
			else if (f2 & 0x200) U16(pk, 0xE) = U16(hdr, 0x14);
			GteReadMAC0(hdr + 0x24);
			const int32_t area = S32(hdr, 0x24);
			if (area == 0) continue;
			if (area < 0 && !(U8(hdr, 0x1C) & 0x10)) continue;
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteLoadV0u(vtx + (uint32_t)U16(rec, 0xA) * 4);
			GteRTPS();
			uint32_t clip = 0;
			if (S16(pk, 8) < 0 || S16(pk, 8) > 0xA00) clip = 1;
			if (S16(pk, 0x10) < 0 || S16(pk, 0x10) > 0xA00) clip |= 2;
			if (S16(pk, 0x18) < 0 || S16(pk, 0x18) > 0xA00) clip |= 4;
			if (S16(pk, 0xA) < 0 || S16(pk, 0xA) > 0x6C0) clip |= 0x10;
			if (S16(pk, 0x12) < 0 || S16(pk, 0x12) > 0x6C0) clip |= 0x20;
			if (S16(pk, 0x1A) < 0 || S16(pk, 0x1A) > 0x6C0) clip |= 0x40;
			GteReadSXY2u(pk + 0x20);
			GteAVSZ4();
			if (S16(pk, 0x20) < 0 || S16(pk, 0x20) > 0xA00) clip |= 8;
			if (S16(pk, 0x22) < 0 || S16(pk, 0x22) > 0x6C0) clip |= 0x80;
			if ((clip & 0xF) == 0xF || (clip & 0xF0) == 0xF0) continue;
			GteReadOTZ32(hdr + 0x2C);
			if (U8(hdr, 0x1C) & 0x40)
			{
				GteLoadRGBC(pk + 4);
				GteSetIR0((int32_t)U32(hdr, 0xC));
				GteDPCS();
				GteStoreRGB2(pk + 4);
			}
			const int32_t z = S32(hdr, 0x2C) >> (shift & 31);
			InsertPrimAutoDepth(ot + (uint32_t)z * 4, (void *)pk);
			pk += 0x28;
		}
		U32(hdr, 0x20) = rec;
		return pk;
	}

	static uint32_t QuadModel(uint32_t hdr, uint32_t ot, int32_t mode, uint32_t cursor)
	{
		const uint32_t fl = U32(hdr, 0x1C);
		if (!(fl & 0x2000)) U32(hdr, 4) = U32(hdr, 0) + 8;
		const uint32_t model = U32(hdr, 0);
		U32(hdr, 0x20) = U32(model, 0) + model;
		if (!(fl & 0x1000)) U32(hdr, 0x18) = 0;
		GteSetFarColor(U8(hdr, 8), U8(hdr, 9), U8(hdr, 0xA));
		U32(hdr, 0x20) = U32(hdr, 0x20) + 0xC;
		return QuadList(hdr, ot, mode, cursor);
	}

	static void InitPairs()
	{
		uint32_t a = game_addr(MODEL_GasVerts), b = game_addr(VERTS), e = game_addr(PAIRS);
		for (int32_t i = WAVE; i != 0; i--)
			for (int32_t k = 2; k != 0; k--)
			{
				U32(e, 0) = a;
				U32(e, 4) = b;
				a += 8;
				b += 8;
				e += 8;
			}
		for (uint32_t i = game_addr(PAIRS), n = 0; i < game_addr(PAIRS_END) - 8; i += 8, n++)
		{
			if (n + 1 >= 0x7A) continue;
			for (uint32_t j = i + 8; j < game_addr(PAIRS_END); j += 8)
			{
				const uint32_t vi = U32(i, 0), vj = U32(j, 0);
				if (S16(vi, 4) >= S16(vj, 4)) continue;
				U32(i, 0) = vj;
				U32(j, 0) = vi;
				const uint32_t si = U32(j, 4);
				U32(j, 4) = U32(i, 4);
				U32(i, 4) = si;
			}
		}
	}

	static void WaveStep(Cloud *c, int32_t *out)
	{
		c->angle_a0 = c->angle_a;
		c->angle_b0 = c->angle_b;
		int32_t amp_a = c->amp_a, amp_b = c->amp_b;
		for (int32_t i = WAVE; i != 0; i--, out += 2)
		{
			out[0] = mul32(ComputeSin(c->angle_a), amp_a) >> 12;
			c->angle_a = (int16_t)(c->angle_a + c->speed_a);
			c->speed_a = (int16_t)(c->speed_a - (int16_t)(c->speed_a >> 5));
			amp_a += c->amp_a_step;
			out[1] = mul32(ComputeSin(c->angle_b), amp_b) >> 12;
			amp_b += c->amp_b_step;
		}
		c->angle_a = (int16_t)(c->angle_a0 + 0x64);
		c->angle_b = c->angle_b0;
	}

	static void WaveBuild(Cloud *c, const int32_t *offs, uint32_t verts, uint32_t node)
	{
		const int32_t count = c->count;
		const uint32_t delta = verts - game_addr(VERTS);
		uint32_t e = game_addr(PAIRS);
		uint32_t last = node;
		if (count > 0)
		{
			for (int32_t k = 0; k < count; k++)
			{
				const int16_t ox = (int16_t)offs[k * 2];
				const int16_t oz = (int16_t)offs[k * 2 + 1];
				const int16_t oy = (int16_t)(ox >> 1);
				for (int32_t j = 2; j != 0; j--)
				{
					last = U32(e, 4) + delta;
					const uint32_t m = U32(e, 0);
					e += 8;
					S16(last, 0) = (int16_t)(S16(m, 0) + ox);
					S16(last, 2) = (int16_t)(S16(m, 2) + oy);
					S16(last, 4) = (int16_t)(S16(m, 4) + oz);
				}
			}
		}
		const uint32_t w0 = U32(last, 0), w1 = U32(last, 4);
		if (count < WAVE)
		{
			for (int32_t k = WAVE - count; k != 0; k--)
				for (int32_t j = 2; j != 0; j--)
				{
					const uint32_t v = U32(e, 4);
					e += 8;
					U32(v + delta, 0) = w0;
					U32(v + delta, 4) = w1;
				}
		}
		if (DrawOnly()) return;
		c->count += c->step;
		if (c->count > WAVE) c->count = WAVE;
	}

	static void GasMatrix(const GasNode *g)
	{
		int32_t v[3];
		v[0] = g->pos[0] - CasterAnchor()[0];
		v[1] = g->pos[1] - CasterAnchor()[1];
		v[2] = g->pos[2] - CasterAnchor()[2];
		const int32_t dist = Sqrt(Normalize(v, v));
		int32_t axis[3];
		Mat4x3 m = {};
		const int32_t angle = RotationBetweenVectors(AXIS_Model(), v, axis);
		BuildAxisAngleRotationMatrix(angle, &m, axis);
		m.t[1] = CasterAnchor()[1];
		m.t[0] = CasterAnchor()[0];
		int32_t s[3];
		s[0] = g->width;
		s[1] = g->width + (g->width >> 1);
		s[2] = Div1600(shl32(dist, 12));
		m.t[2] = CasterAnchor()[2];
		Scale3DMatrix(&m, s);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}

	static void GasHeader(uint8_t *h, const GasNode *g, uint32_t verts)
	{
		*(uint32_t *)(h + 0) = game_addr(MODEL_Gas);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x2033;
		*(uint32_t *)(h + 4) = verts;
		const int16_t c = g->counter;
		if (c < 0x18)
		{
			*(int32_t *)(h + 0xC) = 0x1000 - c * 170;
			*(uint32_t *)(h + 0x1C) = 0x20F3;
		}
		else if (c >= 0x34)
		{
			*(int32_t *)(h + 0xC) = shl32(c - 0x34, 9);
			*(uint32_t *)(h + 0x1C) = 0x20F3;
		}
	}

	static void SparkTargetFade(uint8_t *h, Spark *e, int16_t counter)
	{
		*(int16_t *)(h + 4) = e->frame;
		if (e->fade != 0)
		{
			if (counter < e->fade_len)
			{
				const int32_t v = 0x80 / e->fade_len * counter;
				*(uint32_t *)(h + 0x1C) = (uint32_t)(shl32(shl32(v, 8) | v, 8) | v);
				h[0x24] |= 4;
				return;
			}
			e->fade = 0;
		}
		*(uint16_t *)(h + 0x24) = 0;
	}

	static void SparkDraw(uint8_t *h, const Spark *e, int32_t shift)
	{
		TransformCameraByShadowRotation(e->pos, e->scale, -(e->scale >> shift));
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT44(), 2, PacketCursor());
	}
}
}

namespace ff8fx
{
namespace gas168
{
	static uint32_t __cdecl GasTask(TaskNode *n)
	{
		GasNode *g = (GasNode *)n;
		const int32_t i = g->cloud;
		WaveStep(CloudRec(i), CloudOffsets(i));
		WaveBuild(CloudRec(g->cloud), CloudOffsets(g->cloud), CloudVerts(g->cloud), P(g));
		GasMatrix(g);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		GasHeader(h, g, CloudVerts(g->cloud));
		PacketCursor() = QuadModel(P(h), RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
		if (DrawOnly()) return 0;
		g->counter++;
		return g->counter >= 0x3C ? TASK_END : 0;
	}

	static uint32_t __cdecl SparkTargetTask(TaskNode *n)
	{
		SparkNode *s = (SparkNode *)n;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t active = 0;
		*(uint32_t *)h = game_addr(SEQ_Sparkle);
		for (uint32_t p = game_addr(POOL_A); p < game_addr(POOL_A_END); p += 0x18)
		{
			Spark *e = (Spark *)p;
			if (!(e->owner & (int32_t)s->mask)) continue;
			SparkTargetFade(h, e, s->counter);
			SparkDraw(h, e, 4);
			if (DrawOnly()) continue;
			e->frame++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				e->owner = 0;
				continue;
			}
			e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
			e->pos[1] = (int16_t)(e->pos[1] + e->vel[1]);
			e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
			e->vel[1] = (int16_t)(e->vel[1] - (int16_t)(e->vel[1] >> 5));
			active++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (s->counter >= 2 && s->counter <= 0x22)
		{
			int32_t v[3];
			v[0] = s->pos[0] - CasterAnchor()[0];
			v[1] = s->pos[1] - CasterAnchor()[1];
			v[2] = s->pos[2] - CasterAnchor()[2];
			Normalize(v, v);
			for (int32_t k = 0; k < 1; k++)
			{
				int32_t idx = 0;
				uint32_t p = game_addr(POOL_A);
				while (p < game_addr(POOL_A_END) && ((Spark *)p)->owner != 0)
				{
					p += 0x18;
					idx++;
				}
				if (p >= game_addr(POOL_A_END) || idx >= 0x64) break;
				Spark *e = (Spark *)p;
				e->owner = s->mask;
				e->frame = 0;
				e->scale = (int16_t)(CrtRand() % 0xA00 + 0x600);
				e->pos[0] = (int16_t)(CrtRand() % 1000 + s->pos[0] - 0x1F4);
				e->pos[1] = (int16_t)(s->pos[1] - CrtRand() % 300);
				e->pos[2] = (int16_t)(CrtRand() % 1000 + s->pos[2] - 0x1F4);
				e->vel[1] = (int16_t)(-0x32 - CrtRand() % 0x46);
				if (s->counter < 0x10)
				{
					e->fade = 1;
					e->fade_len = (int16_t)(0x10 - s->counter);
				}
				else e->fade = 0;
				const int32_t speed = CrtRand() % 20 + 20;
				e->vel[0] = (int16_t)(mul32(speed, v[0]) >> 12);
				e->vel[2] = (int16_t)(mul32(speed, v[2]) >> 12);
			}
		}
		s->counter++;
		if (s->counter >= 4 && active == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl SparkCasterTask(TaskNode *n)
	{
		SparkNode *s = (SparkNode *)n;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t active = 0;
		*(uint32_t *)h = game_addr(SEQ_Sparkle);
		*(uint16_t *)(h + 0x24) = 8;
		for (uint32_t p = game_addr(POOL_B); p < game_addr(POOL_B_END); p += 0x18)
		{
			Spark *e = (Spark *)p;
			if (!(e->owner & 1)) continue;
			*(int16_t *)(h + 4) = e->frame;
			SparkDraw(h, e, 3);
			if (DrawOnly()) continue;
			e->frame++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				e->owner = 0;
				continue;
			}
			e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
			e->pos[1]--;
			e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
			active++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (s->counter >= 0 && s->counter <= 0x32)
		{
			GetEffectSpawnPosition(Entity((int)CasterSlot()), 0xF0, 0x1000, s->pos);
			s->pos[1] = (int16_t)(s->pos[1] - 0xA0);
			int32_t v[3];
			v[0] = Centre()[0] - s->pos[0];
			v[1] = Centre()[1] - s->pos[1];
			v[2] = Centre()[2] - s->pos[2];
			Normalize(v, v);
			for (int32_t k = 0; k < 1; k++)
			{
				int32_t idx = 0;
				uint32_t p = game_addr(POOL_B);
				while (p < game_addr(POOL_B_END) && ((Spark *)p)->owner != 0)
				{
					p += 0x18;
					idx++;
				}
				if (p >= game_addr(POOL_B_END) || idx >= 0x32) break;
				Spark *e = (Spark *)p;
				e->owner = 1;
				e->frame = 0;
				e->scale = (int16_t)(CrtRand() % 0x800 + 0x600);
				U32(p, 8) = U32(P(s), 0x10);
				U32(p, 0xC) = U32(P(s), 0x14);
				e->pos[0] = (int16_t)(e->pos[0] + (CrtRand() % 100 - 0x32));
				e->pos[1] = (int16_t)(e->pos[1] + (CrtRand() % 100 - 0x32));
				e->pos[2] = (int16_t)(e->pos[2] + (CrtRand() % 100 - 0x32));
				const int32_t speed = CrtRand() % 25 + 15;
				e->vel[0] = (int16_t)(mul32(speed, v[0]) >> 12);
				e->vel[2] = (int16_t)(mul32(speed, v[2]) >> 12);
			}
		}
		s->counter++;
		if (s->counter >= 4 && active == 0) return TASK_END;
		return 0;
	}

	static uint32_t __cdecl DirectorTask(TaskNode *n)
	{
		if (DrawOnly()) return 0;
		DirectorNode *d = (DirectorNode *)n;
		if (d->counter == 0)
		{
			GetEffectSpawnPosition(Entity((int)CasterSlot()), 0xF0, 0x1000, CasterAnchor());
			CasterAnchor()[1] = (int16_t)(CasterAnchor()[1] - 0xA0);
			CalculateCenterPosition(Ctx()->target_mask, Centre());
			int32_t v[3];
			v[0] = Centre()[0] - CasterAnchor()[0];
			v[1] = Centre()[1] - CasterAnchor()[1];
			v[2] = Centre()[2] - CasterAnchor()[2];
			const int32_t len = shl32(mul32(Sqrt(Normalize(v, v)), 13), 8) >> 12;
			Centre()[1] = (int16_t)((mul32(len, v[1]) >> 12) + CasterAnchor()[1]);
			Centre()[0] = (int16_t)((mul32(len, v[0]) >> 12) + CasterAnchor()[0]);
			Centre()[2] = (int16_t)((mul32(len, v[2]) >> 12) + CasterAnchor()[2]);
			InitPairs();
		}
		if (d->counter >= 6 && d->counter <= 0xB)
		{
			for (int32_t k = 2; k != 0; k--)
			{
				GasNode *g = (GasNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Gas));
				const int16_t cloud = d->clouds;
				const uint32_t z = U32(game_addr(game::unk_23B78AC), 0);
				U32(P(g), 0x10) = U32(game_addr(game::g_Mag168_CloudCentre), 0);
				g->counter = 0;
				g->cloud = cloud;
				U32(P(g), 0x14) = z;
				g->pos[0] = (int16_t)(g->pos[0] + (int16_t)(CrtRand() % 5000 - 0x9C4));
				g->pos[1] = (int16_t)(-0xC8 - CrtRand() % 0x6A4);
				g->pos[2] = (int16_t)(g->pos[2] + (int16_t)(CrtRand() % 0x9C4 - 0x4E2));
				const int32_t w = CrtRand() % 0xD00;
				Cloud *c = CloudRec(g->cloud);
				c->step = 2;
				c->count = 2;
				g->width = (int16_t)(w + 0x500);
				c->angle_a = (int16_t)(CrtRand() % 0x1000);
				c->speed_a = (int16_t)(CrtRand() % 0xA0 + 0x96);
				c->amp_a = 0;
				c->amp_a_step = (int16_t)(CrtRand() % 0x14 + 5);
				c->angle_b = 0x800;
				c->amp_b_step = (int16_t)Div61(CrtRand() % 0x400 + 0x640);
				c->amp_b = (int16_t)(CrtRand() % 0x258 + 0x384);
				d->clouds++;
			}
		}
		if (d->counter == 0x17)
		{
			for (uint32_t i = 0; i < Ctx()->actions[0].target_count; i++)
			{
				const uint8_t slot = Ctx()->actions[0].targets[i * TARGET_STRIDE];
				SparkNode *s = (SparkNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_SparkTarget));
				s->counter = 0;
				s->mask = (int16_t)(1 << (i & 31));
				GetEffectSpawnPosition(Entity(slot), 0xF1, 0, s->pos);
				s->pos[1] = *(int16_t *)(Entity(slot) + 0x3C);
			}
		}
		if (d->counter == 4) ((SparkNode *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_SparkCaster)))->counter = 0;
		if (d->counter == 0x3C) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		if (d->counter >= 4 && d->counter <= 0xC) SetScreenFlash((uint32_t)((d->counter - 4) * 0xC0), 0x302060);
		else if (d->counter >= 0x3E) SetScreenFlash((uint32_t)((0x46 - d->counter) * 0xC0), 0x302060);
		if (d->counter == 1) BdPlaySE(SOUND_Gas(), 0, 0x80);
		d->counter++;
		if (d->counter <= 0x46) return 0;
		SetScreenFlash(0, 0);
		return TASK_END;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		if (r->counter & 1) PacketCursor() = TexBase() + 0x14000;
		else PacketCursor() = TexBase();
		const int a = ExecuteTaskQueue(EffectQueue());
		r->counter++;
		return a ? 0 : TASK_END;
	}
}

	void register_mag168_sleeping_gas()
	{
		register_port(gas168::ORIG_Root, (void *)gas168::RootTask, "G168 RootTask", 168);
		register_port(gas168::ORIG_Director, (void *)gas168::DirectorTask, "G168 DirectorTask", 168);
		register_port(gas168::ORIG_Gas, (void *)gas168::GasTask, "G168 GasTask", 168);
		register_port(gas168::ORIG_SparkTarget, (void *)gas168::SparkTargetTask, "G168 SparkTargetTask", 168);
		register_port(gas168::ORIG_SparkCaster, (void *)gas168::SparkCasterTask, "G168 SparkCasterTask", 168);
	}
}

