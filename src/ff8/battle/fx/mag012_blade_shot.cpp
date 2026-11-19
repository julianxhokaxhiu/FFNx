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

// Effect 12: Blade Shot

#include "mag_common.h"

namespace ff8fx
{
namespace blade012
{
	using namespace eng;
	using namespace magc;

	inline uint8_t *&Caster() { return var<uint8_t *>(game_addr(game::g_Mag012_Caster)); }
	inline uint8_t *&Target() { return var<uint8_t *>(game_addr(game::g_Mag012_Target)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_Mag012_Ctx)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_Mag012_PacketCursor)); }
	inline TaskQueue *QTrails() { return (TaskQueue *)game_addr(game::g_Mag012_TrailQueue); }
	inline TaskQueue *QParts() { return (TaskQueue *)game_addr(game::g_Mag012_PartQueue); }
	static constexpr game::Id ARENA_A = game::unk_2546100, ARENA_B = game::unk_254E100;
	static constexpr game::Id STATIC_V0 = game::unk_2556108;

	static constexpr game::Id ORIG_RootTask = game::MAG_012_RootTask;
	static constexpr game::Id ORIG_PartTask = game::MAG_012_PartTask;
	static constexpr game::Id ORIG_StreakTask = game::MAG_012_StreakTask;
	static constexpr game::Id ORIG_TrailTask = game::MAG_012_TrailTask;
	static constexpr game::Id MODEL_Blade = game::unk_13646EC;
	static constexpr game::Id MODEL_Ring = game::unk_136401C;
	static constexpr game::Id MODEL_Trail = game::unk_1364F14;
	static constexpr game::Id MODEL_WingA = game::unk_1362E3C;
	static constexpr game::Id KEYS_WingA0 = game::unk_136340C, KEYS_WingA1 = game::unk_136359C;
	static constexpr game::Id MODEL_WingB = game::unk_136372C;
	static constexpr game::Id KEYS_WingB0 = game::unk_1363CFC, KEYS_WingB1 = game::unk_1363E8C;
	static constexpr game::Id SEQ_Hit = game::unk_1362C80;
	static constexpr game::Id SEQ_Flash = game::unk_13628FC;
	static constexpr game::Id SEQ_Spark = game::unk_1362AEC;
	static constexpr game::Id SEQ_Glint = game::unk_136298C;
	static constexpr game::Id SEQ_Streak = game::unk_13627E4;
	static const void *SOUND_Throw() { return (const void *)game_addr(game::unk_136562C); }
	static const void *SOUND_Catch() { return (const void *)game_addr(game::unk_1365630); }

	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t flag, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, flag, out); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline int32_t CartesianToGameAngle(int32_t x, int32_t y) { return fn<int32_t (__cdecl *)(int32_t, int32_t)>(game_addr(game::CartesianToGameAngle))(x, y); }
	inline void ApplyZRotation(int32_t angle, Mat4x3 *m) { fn<void (__cdecl *)(int32_t, Mat4x3 *)>(game_addr(game::ApplyZRotation))(angle, m); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	namespace gte
	{
		inline void LoadV012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_LoadV012))(a, b, c); }
		inline void RTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
		inline void RTPS() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPS))(); }
		inline void ReadFLAG(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadFLAG))(dst); }
		inline void NCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
		inline void ReadMAC0(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadMAC0))(dst); }
		inline void ReadSXY012_Split(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
		inline void ReadSXY2(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadSXY2))(dst); }
		inline void AVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
		inline void AVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
		inline void ReadOTZ(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadOTZ))(dst); }
		inline void SetFarColor(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
		inline void LoadRGB012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_LoadRGB012))(a, b, c); }
		inline void DPCT() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCT))(); }
		inline void StoreRGB012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_StoreRGB012))(a, b, c); }
		inline void LoadRGBC(uint32_t src) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadRGBC))(src); }
		inline void DPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
		inline void StoreRGB2(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreRGB2))(dst); }
	}

#pragma pack(push, 1)
	struct Motion
	{
		int16_t pos[4];
		int16_t start[3];
		int16_t t;
		int16_t end[3];
		int16_t dt;
	};
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint32_t arena;
		Motion mot;
		int16_t blade[3];
		int16_t ang;
		int16_t prev[4];
		int16_t accx;
		int16_t pad3E;
		int16_t accz;
		int16_t pad42;
		int16_t velx;
		int16_t pad46;
		int16_t velz;
		int16_t pad4A;
		int16_t hit[4];
		int16_t rot[4];
		int16_t dirx;
		int16_t dirz;
		uint8_t turned;
		uint8_t pad61[3];
	};
	struct PartNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint32_t seq;
		int16_t pos[3];
		int16_t life;
		int16_t vel[3];
		int16_t scale;
		int16_t acc[3];
		int16_t pad2A;
		int16_t dir[4];
	};
	struct TrailNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		int16_t pos[3];
		int16_t fade;
		int16_t rot[4];
	};
#pragma pack(pop)
	static_assert(sizeof(Motion) == 0x18 && sizeof(RootNode) == 0x64, "Blade Shot nodes");
	static_assert(sizeof(PartNode) == 0x34 && sizeof(TrailNode) == 0x20, "Blade Shot nodes");

	static void FillDwords(void *dst, uint32_t value, uint32_t count)
	{
		uint32_t *d = (uint32_t *)dst;
		for (uint32_t k = 0; k < count; k++) d[k] = value;
	}

	static void RotX(int16_t angle, Mat4x3 *m)
	{
		const int32_t s = ComputeSin(angle);
		const int32_t c = ComputeCos(angle);
		m->m[1][2] = (int16_t)-s;
		m->m[2][1] = (int16_t)s;
		m->m[0][0] = 0x1000;
		m->m[0][1] = 0;
		m->m[0][2] = 0;
		m->m[1][0] = 0;
		m->m[1][1] = (int16_t)c;
		m->m[2][0] = 0;
		m->m[2][2] = (int16_t)c;
	}

	static void YawMatrix(int16_t yaw, Mat4x3 *m)
	{
		const int32_t sn = ComputeSin(yaw);
		const int32_t cs = ComputeCos(yaw);
		m->m[0][2] = (int16_t)sn;
		m->m[2][0] = (int16_t)-sn;
		m->m[0][0] = (int16_t)cs;
		m->m[0][1] = 0;
		m->m[1][0] = 0;
		m->m[1][1] = 0x1000;
		m->m[1][2] = 0;
		m->m[2][1] = 0;
		m->m[2][2] = (int16_t)cs;
	}

	static void AngleMatrix(const int16_t *angles, Mat4x3 *m)
	{
		RotX(angles[0], m);
		ApplyZRotation(angles[2], m);
		Mat4x3 y;
		YawMatrix(angles[1], &y);
		MatrixMultiply(&y, m);
	}

	static void MotionInit(Motion *m, int32_t ticks)
	{
		ff8fx::copy<(8)>(m->pos, m->start);
		m->t = 0;
		m->dt = (int16_t)(0x1000 / ticks);
	}

	static void MotionStep(Motion *m)
	{
		m->t = (int16_t)(m->t + m->dt);
		GteSetIR0(0x1000 - (int32_t)m->t);
		GteLoadIR123(m->start);
		GteGPF();
		GteSetIR0((int32_t)m->t);
		GteLoadIR123(m->end);
		GteGPL();
		GteStoreIR123(m->pos);
	}

	static void MorphVertices(const uint8_t *h, uint32_t k0, uint32_t k1, int32_t t)
	{
		const uint32_t model = *(const uint32_t *)h;
		uint32_t dst = model + 8;
		uint32_t count = *(const uint32_t *)(model + 4);
		uint32_t a = k0 + 8, b = k1 + 8;
		for (; count != 0; count--, dst += 8, a += 8, b += 8)
		{
			GteSetIR0(0x1000 - t);
			GteLoadIR123((const void *)a);
			GteGPF();
			GteSetIR0(t);
			GteLoadIR123((const void *)b);
			GteGPL();
			GteStoreIR123((void *)dst);
		}
	}

	static void SphereDir(const int16_t *ang, int16_t *out)
	{
		const int32_t s = ComputeSin(ang[1]);
		out[2] = (int16_t)ComputeCos(ang[1]);
		out[1] = (int16_t)(mul32(ComputeSin(ang[2]), s) >> 12);
		out[0] = (int16_t)((int32_t)(0u - (uint32_t)mul32(ComputeCos(ang[2]), s)) >> 12);
	}

	static void YawVector(const int16_t *in, int16_t *out, int32_t angle)
	{
		const int32_t s = (int32_t)(0u - (uint32_t)ComputeSin(angle));
		const int32_t c = ComputeCos(angle);
		out[0] = (int16_t)((int32_t)((uint32_t)mul32(in[0], c) - (uint32_t)mul32(in[2], s)) >> 12);
		out[2] = (int16_t)((int32_t)((uint32_t)mul32(in[0], s) + (uint32_t)mul32(in[2], c)) >> 12);
		out[1] = in[1];
	}

	static inline bool out_x(int16_t v) { return v < 0 || v > 0xA00; }
	static inline bool out_y(int16_t v) { return v < 0 || v > 0x6C0; }

	static uint32_t DrawTriangles(uint32_t h, uint32_t ot, uint32_t mode, uint32_t cursor)
	{
		uint32_t list = *(const uint32_t *)(h + 0x20);
		const uint32_t vbase = *(const uint32_t *)(h + 4);
		uint32_t count = *(const uint32_t *)list;
		uint32_t rec = list + 4;
		*(uint32_t *)(h + 0x20) = rec;
		if ((int32_t)count <= 0)
		{
			*(uint32_t *)(h + 0x20) = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			const uint32_t v0 = vbase + (uint32_t)*(const uint16_t *)(rec + 4) * 4;
			*(uint32_t *)(h + 0x34) = *(const uint32_t *)v0;
			*(uint32_t *)(h + 0x38) = *(const uint32_t *)(v0 + 4);
			const uint32_t v1 = vbase + (uint32_t)*(const uint16_t *)(rec + 6) * 4;
			*(uint32_t *)(h + 0x3C) = *(const uint32_t *)v1;
			*(uint32_t *)(h + 0x40) = *(const uint32_t *)(v1 + 4);
			const uint32_t v2 = vbase + (uint32_t)*(const uint16_t *)(rec + 8) * 4;
			*(uint32_t *)(h + 0x44) = *(const uint32_t *)v2;
			*(uint32_t *)(h + 0x48) = *(const uint32_t *)(v2 + 4);
			gte::LoadV012(h + 0x34, h + 0x3C, h + 0x44);
			gte::RTPT();
			*(uint32_t *)cursor = 0x6000000;
			*(uint32_t *)(pkt + 4) = *(const uint32_t *)rec;
			gte::ReadFLAG(h + 0x30);
			if (!(*(const uint32_t *)(h + 0x30) & 0x60000))
			{
				gte::NCLIP();
				uint32_t clip = 0;
				gte::ReadMAC0(h + 0x24);
				if (*(const int32_t *)(h + 0x24) >= 0 || (*(const uint8_t *)(h + 0x1C) & 0x20))
				{
					gte::ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					gte::AVSZ3();
					if (out_x(*(const int16_t *)(pkt + 8))) clip = 1;
					if (out_x(*(const int16_t *)(pkt + 0x10))) clip |= 2;
					if (out_x(*(const int16_t *)(pkt + 0x18))) clip |= 4;
					if (out_y(*(const int16_t *)(pkt + 0x0A))) clip |= 0x10;
					if (out_y(*(const int16_t *)(pkt + 0x12))) clip |= 0x20;
					if (out_y(*(const int16_t *)(pkt + 0x1A))) clip |= 0x40;
					gte::ReadOTZ(h + 0x2C);
					if ((uint8_t)(clip & 7) != 7 && (uint8_t)(clip & 0x70) != 0x70)
					{
						const int32_t z = *(const int32_t *)(h + 0x2C) - 8;
						*(int32_t *)(h + 0x2C) = z;
						if (z > 0)
						{
							gte::SetFarColor(*(const uint8_t *)(h + 8), *(const uint8_t *)(h + 9), *(const uint8_t *)(h + 0xA));
							gte::LoadRGB012(rec + 0x0C, rec + 0x10, pkt + 4);
							GteSetIR0(*(const int32_t *)(h + 0xC));
							gte::DPCT();
							gte::StoreRGB012(pkt + 0x0C, pkt + 0x14, pkt + 4);
							InsertPrimAutoDepth(ot + (uint32_t)(*(const int32_t *)(h + 0x2C) >> (mode & 31)) * 4, (void *)cursor);
							cursor += 0x1C;
							pkt += 0x1C;
						}
					}
				}
			}
			rec += 0x14;
		} while (--count);
		*(uint32_t *)(h + 0x20) = rec;
		return cursor;
	}

	static uint32_t DrawQuads(uint32_t h, uint32_t ot, uint32_t mode, uint32_t cursor)
	{
		uint32_t list = *(const uint32_t *)(h + 0x20);
		const uint32_t vbase = *(const uint32_t *)(h + 4);
		uint32_t count = *(const uint32_t *)list;
		uint32_t rec = list + 4;
		*(uint32_t *)(h + 0x20) = rec;
		if ((int32_t)count <= 0)
		{
			*(uint32_t *)(h + 0x20) = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			gte::LoadV012(vbase + (uint32_t)*(const uint16_t *)(rec + 4) * 4, vbase + (uint32_t)*(const uint16_t *)(rec + 6) * 4, vbase + (uint32_t)*(const uint16_t *)(rec + 8) * 4);
			gte::RTPT();
			*(uint32_t *)cursor = 0x8000000;
			*(uint32_t *)(pkt + 4) = *(const uint32_t *)rec;
			gte::ReadFLAG(h + 0x30);
			if (!(*(const uint32_t *)(h + 0x30) & 0x60000))
			{
				gte::NCLIP();
				uint32_t clip = 0;
				gte::ReadMAC0(h + 0x24);
				if (*(const int32_t *)(h + 0x24) >= 0 || (*(const uint8_t *)(h + 0x1C) & 0x20))
				{
					gte::ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					GteLoadV0((const void *)(vbase + (uint32_t)*(const uint16_t *)(rec + 0x0A) * 4));
					GteRTPS();
					if (out_x(*(const int16_t *)(pkt + 8))) clip = 1;
					if (out_x(*(const int16_t *)(pkt + 0x10))) clip |= 2;
					if (out_x(*(const int16_t *)(pkt + 0x18))) clip |= 4;
					if (out_y(*(const int16_t *)(pkt + 0x0A))) clip |= 0x10;
					if (out_y(*(const int16_t *)(pkt + 0x12))) clip |= 0x20;
					if (out_y(*(const int16_t *)(pkt + 0x1A))) clip |= 0x40;
					gte::ReadSXY2(pkt + 0x20);
					gte::AVSZ4();
					if (out_x(*(const int16_t *)(pkt + 0x20))) clip |= 8;
					if (out_y(*(const int16_t *)(pkt + 0x22))) clip |= 0x80;
					if ((uint8_t)(clip & 0xF) != 0xF && (uint8_t)(clip & 0xF0) != 0xF0)
					{
						gte::ReadOTZ(h + 0x2C);
						const int32_t z = *(const int32_t *)(h + 0x2C) - 8;
						*(int32_t *)(h + 0x2C) = z;
						if (z > 0)
						{
							gte::LoadRGB012(rec + 0x0C, rec + 0x10, rec + 0x14);
							GteSetIR0(*(const int32_t *)(h + 0xC));
							gte::DPCT();
							gte::StoreRGB012(pkt + 0x0C, pkt + 0x14, pkt + 0x1C);
							gte::LoadRGBC(pkt + 4);
							gte::DPCS();
							gte::StoreRGB2(pkt + 4);
							InsertPrimAutoDepth(ot + (uint32_t)(*(const int32_t *)(h + 0x2C) >> (mode & 31)) * 4, (void *)cursor);
						}
						cursor += 0x24;
						pkt += 0x24;
					}
				}
			}
			rec += 0x18;
		} while (--count);
		*(uint32_t *)(h + 0x20) = rec;
		return cursor;
	}

	static uint32_t DrawModel(uint8_t *h, uint32_t ot, uint32_t mode, uint32_t cursor)
	{
		const uint32_t model = *(const uint32_t *)h;
		*(uint32_t *)(h + 4) = model + 8;
		uint32_t list = model + *(const uint32_t *)model + 0x10;
		*(uint32_t *)(h + 0x20) = list;
		if (*(const uint32_t *)list != 0)
			cursor = DrawTriangles((uint32_t)h, ot, mode, cursor);
		else
			*(uint32_t *)(h + 0x20) = list + 4;
		list = *(const uint32_t *)(h + 0x20);
		if (*(const uint32_t *)list != 0)
			return DrawQuads((uint32_t)h, ot, mode, cursor);
		*(uint32_t *)(h + 0x20) = list + 4;
		return cursor;
	}

	static void BladeDraw(const RootNode *n, uint8_t *h)
	{
		Mat4x3 m;
		AngleMatrix(n->rot, &m);
		m.t[0] = n->blade[0];
		m.t[1] = n->blade[1];
		m.t[2] = n->blade[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)h = game_addr(MODEL_Blade);
		*(uint32_t *)(h + 0x1C) = 0;
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
	}

	static void SeqDraw(const int16_t *pos, uint8_t *h, uint32_t seq, int16_t frame)
	{
		TransformCameraByShadowRotation(pos, 0x1000, -0x400);
		*(uint32_t *)h = seq;
		*(int16_t *)(h + 4) = frame;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}

	static uint32_t RingScale(int32_t k) { return (uint32_t)shl32(k, 12) / 20; }

	static void RingDraw(const RootNode *n, uint32_t s)
	{
		uint8_t *h = AllocHeader(0x58);
		Mat4x3 m;
		YawMatrix(n->rot[1], &m);
		const int32_t g = (int32_t)s + 0x1000;
		m.m[2][0] = (int16_t)(mul32(m.m[2][0], g) >> 12);
		const int32_t tx = (mul32(m.m[0][2], (int32_t)s) >> 14) + n->hit[0];
		m.m[2][2] = (int16_t)(mul32(m.m[2][2], g) >> 12);
		const int32_t tz = (mul32(m.m[0][0], (int32_t)s) >> 14) + n->hit[2];
		m.t[0] = tx;
		m.t[1] = *(const int16_t *)(Target() + 0x24);
		m.t[2] = tz;
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(uint32_t *)h = game_addr(MODEL_Ring);
		*(int32_t *)(h + 0xC) = ComputeSin((int32_t)s >> 2);
		*(uint32_t *)(h + 0x1C) = 0xF0;
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void Ring(const RootNode *n, int32_t k)
	{
		if ((uint32_t)k >= 0x14) return;
		RingDraw(n, RingScale(k));
	}

	static void WingPair(const RootNode *n, uint32_t s, uint32_t model, uint32_t k0, uint32_t k1, int16_t yaw)
	{
		uint8_t *h = AllocHeader(0x58);
		const int32_t q = (int32_t)s >> 2;
		*(uint32_t *)h = model;
		h[0xA] = 0;
		h[9] = 0;
		h[8] = 0;
		*(int32_t *)(h + 0xC) = ComputeSin(q);
		*(uint32_t *)(h + 0x1C) = 0xF0;
		MorphVertices(h, k0, k1, ComputeSin(q));
		Mat4x3 m;
		YawMatrix((int16_t)(n->rot[1] + yaw), &m);
		m.t[0] = n->hit[0];
		m.t[1] = n->hit[1];
		m.t[2] = n->hit[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		YawMatrix((int16_t)(n->rot[1] - yaw), &m);
		m.t[0] = n->hit[0];
		m.t[1] = n->hit[1];
		m.t[2] = n->hit[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void PartDraw(const PartNode *p, uint8_t *h)
	{
		TransformCameraByShadowRotation(p->pos, p->scale, -((int32_t)p->scale >> 2));
		*(uint32_t *)h = p->seq;
		*(int16_t *)(h + 4) = p->counter;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}

	static void StreakDraw(const PartNode *p, uint8_t *h)
	{
		int16_t d[4];
		d[0] = (int16_t)(p->pos[0] - p->dir[0]);
		d[1] = (int16_t)(p->pos[1] - p->dir[1]);
		d[2] = (int16_t)(p->pos[2] - p->dir[2]);
		TransformCameraByShadowRotation(d, 0x800, -0x200);
		GteLoadV0((const void *)game_addr(STATIC_V0));
		GteMVMVA_RotV0Tr();
		int16_t a[3];
		GteStoreIR123(a);
		TransformCameraByShadowRotation(p->pos, 0x800, -0x200);
		GteLoadV0(p->dir);
		GteMVMVA_RotV0Tr();
		int16_t b[3];
		GteStoreIR123(b);
		const int32_t ang = CartesianToGameAngle((int32_t)a[1] - b[1], (int32_t)b[0] - a[0]);
		*(int32_t *)(h + 8) = (int32_t)(0u - (uint32_t)ang);
		*(uint32_t *)h = p->seq;
		*(int16_t *)(h + 4) = p->counter;
		*(int16_t *)(h + 0x24) = 1;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}

	static int32_t TrailAngle(int16_t counter) { return shl32(counter, 10) / 8; }

	static void TrailDraw(const TrailNode *t, uint8_t *h, int32_t a)
	{
		Mat4x3 m;
		AngleMatrix(t->rot, &m);
		m.t[0] = t->pos[0];
		m.t[1] = t->pos[1];
		m.t[2] = t->pos[2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		int32_t v = ComputeSin(a);
		*(int32_t *)(h + 0xC) = v;
		if (t->fade < 0x1000)
		{
			v = 0x1000 - (mul32(0x1000 - v, t->fade) >> 12);
			*(int32_t *)(h + 0xC) = v;
		}
		h[9] = 0;
		h[8] = 0;
		v = *(const int32_t *)(h + 0xC);
		*(uint32_t *)(h + 0x1C) = 0xC0;
		*(uint32_t *)h = game_addr(MODEL_Trail);
		h[0xA] = (uint8_t)((0x1000 - v) >> 5);
		*(int32_t *)(h + 0xC) = (v >> 1) + 0x800;
		PacketCursor() = DrawModel(h, RenderOT(), 2, PacketCursor());
	}

	static void BladeOut(RootNode *n)
	{
		const int32_t x = ((int32_t)n->mot.t >> 1) - 0x400;
		int32_t r;
		if (x < 0)
			r = (int32_t)(0u - (uint32_t)Sqrt((int32_t)(0u - (uint32_t)mul32(mul32(x, x), x))));
		else
			r = Sqrt(mul32(mul32(x, x), x));
		const int32_t a = (r >> 4) + 0x800;
		MotionStep(&n->mot);
		n->rot[0] = (int16_t)(n->rot[0] + (int16_t)(((uint32_t)(uint16_t)((int16_t)a - n->ang)) * 4 + 0x294));
		n->ang = (int16_t)a;
		const int32_t s1 = ComputeSin(a >> 1);
		const int32_t s2 = ComputeSin(a);
		ff8fx::copy<(8)>(n->prev, n->blade);
		const int32_t e = s2 >> 2;
		n->blade[0] = (int16_t)((mul32(n->dirx, e) >> 12) + n->mot.pos[0]);
		n->blade[1] = (int16_t)(((int32_t)(0u - (uint32_t)mul32(s1, s1)) >> 13) + n->mot.pos[1]);
		n->blade[2] = (int16_t)((mul32(n->dirz, e) >> 12) + n->mot.pos[2]);
	}

	static void BladeTurn(RootNode *n)
	{
		n->turned = 1;
		int32_t d = (int32_t)n->mot.end[0] - n->mot.start[0];
		n->velx = (int16_t)((int16_t)(d / 32) - n->prev[0] + n->blade[0]);
		d = (int32_t)n->mot.end[2] - n->mot.start[2];
		n->velz = (int16_t)((int16_t)(d / 32) - n->prev[2] + n->blade[2]);
		uint8_t s[8];
		ff8fx::copy<(8)>(s, n->mot.start);
		ff8fx::copy<(8)>(n->mot.start, n->mot.end);
		ff8fx::copy<(8)>(n->mot.end, s);
		MotionInit(&n->mot, 0x1E);
		d = (int32_t)n->mot.start[0] - n->mot.end[0];
		n->velx = (int16_t)((int16_t)(d / 30 + n->velx) >> 1);
		d = (int32_t)n->mot.start[2] - n->mot.end[2];
		n->velz = (int16_t)((int16_t)(d / 30 + n->velz) >> 1);
		n->dirx = (int16_t)-n->dirx;
		n->dirz = (int16_t)-n->dirz;
	}

	static void BladeBack(RootNode *n)
	{
		n->mot.end[1] = (int16_t)(n->mot.end[1] - 600);
		MotionStep(&n->mot);
		n->rot[0] = (int16_t)(n->rot[0] + 0x210);
		n->accx = (int16_t)(n->accx + n->velx);
		n->accz = (int16_t)(n->accz + n->velz);
		const int32_t t = n->mot.t;
		const int32_t w = 0x1000 - t;
		const int32_t ox = mul32(n->accx, w) >> 12;
		const int32_t oz = mul32(n->accz, w) >> 12;
		const int32_t s = ComputeSin(t >> 1);
		ff8fx::copy<(8)>(n->prev, n->blade);
		n->blade[0] = (int16_t)(n->mot.pos[0] + ox);
		n->blade[1] = (int16_t)(n->mot.pos[1] + ((int32_t)(0u - (uint32_t)s) >> 2));
		n->blade[2] = (int16_t)(n->mot.pos[2] + oz);
	}
}
}

namespace ff8fx
{
namespace blade012
{
	static uint32_t __cdecl PartTask(TaskNode *tn)
	{
		PartNode *p = (PartNode *)tn;
		uint8_t *h = AllocHeader(0xB4);
		PartDraw(p, h);
		FieldFree(0xB4);
		p->counter++;
		if (p->counter >= p->life) return TASK_END;
		p->vel[1] = (int16_t)(p->vel[1] + p->acc[1]);
		p->vel[2] = (int16_t)(p->vel[2] + p->acc[2]);
		p->vel[0] = (int16_t)(p->vel[0] + p->acc[0]);
		p->pos[1] = (int16_t)(p->pos[1] + p->vel[1]);
		p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
		p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
		return 0;
	}

	static uint32_t __cdecl StreakTask(TaskNode *tn)
	{
		PartNode *p = (PartNode *)tn;
		uint8_t *h = AllocHeader(0xB4);
		StreakDraw(p, h);
		FieldFree(0xB4);
		p->counter++;
		if (p->counter >= p->life) return TASK_END;
		p->vel[0] = (int16_t)(p->vel[0] + p->acc[0]);
		p->vel[1] = (int16_t)(p->vel[1] + p->acc[1]);
		p->vel[2] = (int16_t)(p->vel[2] + p->acc[2]);
		p->pos[1] = (int16_t)(p->pos[1] + p->vel[1]);
		p->pos[2] = (int16_t)(p->pos[2] + p->vel[2]);
		p->pos[0] = (int16_t)(p->pos[0] + p->vel[0]);
		return 0;
	}

	static uint32_t __cdecl TrailTask(TaskNode *tn)
	{
		TrailNode *t = (TrailNode *)tn;
		uint8_t *h = AllocHeader(0x58);
		TrailDraw(t, h, TrailAngle(t->counter));
		FieldFree(0x58);
		t->counter++;
		return t->counter < 8 ? 0 : TASK_END;
	}

	static void SpawnSparks(const RootNode *n)
	{
		for (int32_t i = 2; i != 0; i--)
		{
			PartNode *p = (PartNode *)AddTaskToQueue(QParts(), game_addr(ORIG_PartTask));
			if (!p) continue;
			FillDwords(&p->counter, 0, 10);
			const int32_t side = i & 1;
			int32_t a = ((CrtRand() - 0x4000) >> 6) + (side ? 0x400 : -0x400) + n->rot[1];
			p->pos[0] = (int16_t)((ComputeSin(a) >> 3) + n->hit[0]);
			p->pos[2] = (int16_t)((ComputeCos(a) >> 3) + n->hit[2]);
			p->pos[1] = (int16_t)(*(const int16_t *)(Target() + 0x24) - (CrtRand() >> 8));
			a = ((CrtRand() - 0x4000) >> 6) + (side ? 0x190 : -0x190) + n->rot[1];
			int16_t v = (int16_t)(ComputeSin(a) >> 4);
			p->vel[0] = v;
			p->acc[0] = (int16_t)(-(int32_t)v >> 5);
			v = (int16_t)(ComputeCos(a) >> 4);
			p->vel[2] = v;
			p->acc[2] = (int16_t)(-(int32_t)v >> 5);
			v = (int16_t)((int32_t)(0u - (uint32_t)CrtRand()) >> 10);
			p->vel[1] = v;
			p->seq = game_addr(SEQ_Spark);
			p->acc[1] = (int16_t)(-(int32_t)v >> 5);
			p->life = 0xF;
			p->scale = (int16_t)((CrtRand() >> 4) + 0xC00);
		}
	}

	static void SpawnFan(const RootNode *n, int32_t k, bool streaks)
	{
		int16_t ang[3];
		ang[1] = (int16_t)((CrtRand() >> 9) + (streaks ? 0x300 : 0x1E0));
		ang[2] = (int16_t)(k * 17 * 8 + (streaks ? 0 : 0x44));
		for (int32_t i = 5; i != 0; i--)
		{
			PartNode *p = (PartNode *)AddTaskToQueue(QParts(), streaks ? game_addr(ORIG_StreakTask) : game_addr(ORIG_PartTask));
			if (p)
			{
				FillDwords(&p->counter, 0, 10);
				int16_t dir[3];
				SphereDir(ang, dir);
				YawVector(dir, p->pos, n->rot[1]);
				ff8fx::copy<(8)>(p->vel, p->pos);
				p->pos[0] = (int16_t)((p->pos[0] >> 3) + n->hit[0]);
				p->pos[1] = (int16_t)(n->hit[1] + (p->pos[1] >> 3));
				p->pos[2] = (int16_t)((p->pos[2] >> 3) + n->hit[2]);
				const int sh = streaks ? 4 : 5;
				p->vel[0] = (int16_t)(p->vel[0] >> sh);
				p->vel[1] = (int16_t)(p->vel[1] >> sh);
				p->vel[2] = (int16_t)(p->vel[2] >> sh);
				p->seq = streaks ? game_addr(SEQ_Streak) : game_addr(SEQ_Glint);
				p->acc[0] = (int16_t)(-(int32_t)p->vel[0] >> 4);
				p->acc[1] = (int16_t)(-(int32_t)p->vel[1] >> 4);
				p->acc[2] = (int16_t)(-(int32_t)p->vel[2] >> 4);
				if (streaks)
				{
					ff8fx::copy<(8)>(p->dir, p->vel);
					p->life = 8;
				}
				else
				{
					p->life = 0xD;
					p->scale = 0x800;
				}
			}
			ang[2] = (int16_t)(ang[2] + (CrtRand() >> 9) + 0x313);
		}
	}

	static void SpawnTrails(const RootNode *n)
	{
		for (int32_t i = 2; i != 0; i--)
		{
			TrailNode *t = (TrailNode *)AddTaskToQueue(QTrails(), game_addr(ORIG_TrailTask));
			if (!t) continue;
			FillDwords(&t->counter, 0, 5);
			ff8fx::copy<(8)>(t->pos, n->blade);
			ff8fx::copy<(8)>(t->rot, n->rot);
			if (!(i & 1)) t->rot[0] = (int16_t)(t->rot[0] - 0x14A);
			if (n->counter < 8) t->fade = (int16_t)shl32(n->counter, 9);
			else t->fade = 0x1000;
		}
	}

	static uint32_t __cdecl RootTask(TaskNode *tn)
	{
		RootNode *n = (RootNode *)tn;
		if (n->arena != 0)
		{
			PacketCursor() = game_addr(ARENA_A);
			n->arena = 0;
		}
		else
		{
			PacketCursor() = game_addr(ARENA_B);
			n->arena = 1;
		}
		if (n->counter == 5) BdPlaySE3D(SOUND_Throw(), 0, n->mot.start);
		if ((uint16_t)n->counter < 0x20)
		{
			uint8_t *h = AllocHeader(0x58);
			BladeOut(n);
			BladeDraw(n, h);
			FieldFree(0x58);
			n->rot[2] = (int16_t)(n->rot[2] - 0x20);
		}
		else
			n->rot[2] = 0;
		int32_t k = (int32_t)n->counter - 0x20;
		if ((uint32_t)k < 0x1E)
		{
			uint8_t *h = AllocHeader(0x58);
			if (k == 0 && n->turned == 0) BladeTurn(n);
			GetEffectSpawnPosition(Caster(), 7, 0, n->mot.end);
			BladeBack(n);
			BladeDraw(n, h);
			FieldFree(0x58);
		}
		uint32_t &shown = *(uint32_t *)(Caster() + 0x7C);
		if (n->counter < 0x3E) shown &= 0xFFFFFF7Fu;
		else shown |= 0x80;
		k = (int32_t)n->counter - 0x20;
		if ((uint32_t)k < 4)
		{
			uint8_t *h = AllocHeader(0xB4);
			if (k == 0)
			{
				GetEffectSpawnPosition(Target(), 0xF1, 0, n->hit);
				ApplyActionResultToTarget(Ctx()->actions[0].targets);
			}
			SeqDraw(n->hit, h, game_addr(SEQ_Hit), (int16_t)k);
			FieldFree(0xB4);
		}
		k = (int32_t)n->counter - 0x20;
		if ((uint32_t)k < 5)
		{
			uint8_t *h = AllocHeader(0xB4);
			if (k == 0) ff8fx::copy<(8)>(n->hit, n->blade);
			SeqDraw(n->hit, h, game_addr(SEQ_Flash), (int16_t)k);
			FieldFree(0xB4);
		}
		Ring(n, (int32_t)n->counter - 0x20);
		Ring(n, (int32_t)n->counter - 0x22);
		k = (int32_t)n->counter - 0x20;
		if ((uint32_t)k < 0x14)
		{
			WingPair(n, RingScale(k), game_addr(MODEL_WingA), game_addr(KEYS_WingA0), game_addr(KEYS_WingA1), 0x71);
		}
		k = (int32_t)n->counter - 0x20;
		if ((uint32_t)k < 0x14)
		{
			WingPair(n, RingScale(k), game_addr(MODEL_WingB), game_addr(KEYS_WingB0), game_addr(KEYS_WingB1), 0x11C);
		}
		k = (int32_t)n->counter - 0x20;
		if ((uint32_t)k < 0xA) SpawnSparks(n);
		k = (int32_t)n->counter - 0x20;
		if ((uint32_t)k < 6 && !(k & 1)) SpawnFan(n, k, false);
		k = (int32_t)n->counter - 0x20;
		if ((uint32_t)k < 6 && !(k & 1)) SpawnFan(n, k, true);
		if (n->counter < 0x3E) SpawnTrails(n);
		k = (int32_t)n->counter - 0x3C;
		if ((uint32_t)k < 5)
		{
			uint8_t *h = AllocHeader(0xB4);
			int16_t pos[4];
			pos[0] = n->blade[0];
			pos[1] = (int16_t)(n->blade[1] + 0x64);
			pos[2] = n->blade[2];
			SeqDraw(pos, h, game_addr(SEQ_Hit), (int16_t)k);
			FieldFree(0xB4);
			if (k == 0) BdPlaySE3D(SOUND_Catch(), 0, pos);
		}
		const int trails = ExecuteTaskQueue(QTrails());
		const int parts = ExecuteTaskQueue(QParts());
		if (n->counter >= 0x41 && trails == 0 && parts == 0) return TASK_END;
		n->counter++;
		return 0;
	}
}

	void register_mag012_blade_shot()
	{
		register_port(blade012::ORIG_RootTask, (void *)blade012::RootTask, "B012 RootTask", 12);
		register_port(blade012::ORIG_PartTask, (void *)blade012::PartTask, "B012 PartTask", 12);
		register_port(blade012::ORIG_StreakTask, (void *)blade012::StreakTask, "B012 StreakTask", 12);
		register_port(blade012::ORIG_TrailTask, (void *)blade012::TrailTask, "B012 TrailTask", 12);
	}
}

