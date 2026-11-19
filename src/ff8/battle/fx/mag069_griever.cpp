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

// Effect 69: Griever

#include "fx_port.h"

namespace ff8fx
{
namespace g069
{
	using namespace eng;

	inline uint8_t *&SparkHint() { return var<uint8_t *>(game_addr(game::g069_SparkHint)); }
	inline uint8_t *&ShardHint() { return var<uint8_t *>(game_addr(game::g069_ShardHint)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g069_PacketCursor)); }
	inline uint8_t *&Attacker() { return var<uint8_t *>(game_addr(game::g069_Attacker)); }
	inline TaskQueue &QueueTimeline() { return var<TaskQueue>(game_addr(game::g069_QueueTimeline)); }
	inline uint32_t &LcgSeed() { return var<uint32_t>(game_addr(game::g069_RandSeed)); }
	inline uint8_t *&ModelBuffer() { return var<uint8_t *>(game_addr(game::g069_ModelBuffer)); }
	inline Mat4x3 &EffectCamera() { return var<Mat4x3>(game_addr(game::g069_EffectCamera)); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t RenderList() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }

	static constexpr game::Id ORIG_SequenceTask = game::GF_069Griever_SequenceTask;
	static constexpr game::Id ORIG_TimelineTask = game::GF_069Griever_TimelineTask;
	static constexpr game::Id CB_PrimObject = game::GF069_PrimObjectCallback;

	static uint8_t *MODEL_1() { return (uint8_t *)game_addr(game::unk_1D989BC); }
	static uint8_t *MODEL_2() { return (uint8_t *)game_addr(game::unk_1D989E8); }
	static uint8_t *MODEL_1_FLAGS() { return (uint8_t *)game_addr(game::unk_1D989BD); }
	static uint8_t *MODEL_2_FLAGS() { return (uint8_t *)game_addr(game::unk_1D989E9); }

	static const uint32_t MB_SHARDS = 0x0;
	static const uint32_t MB_SPIN = 0x5550;
	static const uint32_t MB_FACE_DIST = 0x5A50;
	static const uint32_t MB_SCRATCH = 0x6000;
	static const uint32_t MB_SPARKS = 0x8020;
	static const uint32_t MB_ARENA_A = 0x3F020, MB_ARENA_B = 0x47020;

	namespace w
	{
		inline int32_t Rand() { return fn<int32_t (__cdecl *)()>(game_addr(game::_rand))(); }
		inline int32_t Cos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
		inline void Decode(uint32_t src, void *layout, int n) { fn<void (__cdecl *)(uint32_t, void *, int)>(game_addr(game::Effect_DecodeModelPrimLayout))(src, layout, n); }
		inline void PlaySE(uint32_t se, int a, int b) { fn<void (__cdecl *)(uint32_t, int, int)>(game_addr(game::BdPlaySE))(se, a, b); }
		inline void ReleaseVoice(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
		inline int32_t Sub508500() { return fn<int32_t (__cdecl *)()>(game_addr(game::BattleFile_GetLoadingState))(); }
		inline void LoadBS(int a, void *dst) { fn<void (__cdecl *)(int, void *)>(game_addr(game::loadBS_36Or37))(a, dst); }
		inline void SwapBS(int a, uint32_t src) { fn<void (__cdecl *)(int, uint32_t)>(game_addr(game::BS_Stage142_ReplaceGeometry))(a, src); }
		inline uint32_t Tile(int r, int g, int b, int level, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int, int, int, int, uint32_t)>(game_addr(game::Effect_FullScreenTile))(r, g, b, level, cursor); }
		inline uint32_t Flipbook(void *h, uint32_t ot, int mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int, uint32_t)>(game_addr(game::MAG_065_RenderFlipbook))(h, ot, mode, cursor); }
		inline void InitSparks() { fn<void (__cdecl *)()>(game_addr(game::GF069_InitSparks))(); }
		inline void InitShards() { fn<void (__cdecl *)()>(game_addr(game::GF069_InitShards))(); }
		inline uint8_t *FaceDistances(void *model, const Mat4x3 *m, void *out) { return fn<uint8_t *(__cdecl *)(void *, const Mat4x3 *, void *)>(game_addr(game::GF069_ModelFaceDistances))(model, m, out); }
		inline uint8_t *SparkAlloc() { return fn<uint8_t *(__cdecl *)()>(game_addr(game::GF069_AllocSpark))(); }
		inline void SparkUpdate() { fn<void (__cdecl *)()>(game_addr(game::GF069_UpdateSparks))(); }
		inline void ShardUpdate() { fn<void (__cdecl *)()>(game_addr(game::GF069_UpdateShards))(); }
		inline void AttackerDraw(void *ent, int32_t y, uint32_t verts) { fn<void (__cdecl *)(void *, int32_t, uint32_t)>(game_addr(game::GF069_DrawEntityStylised))(ent, y, verts); }
		inline void LoadV0u(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
		inline void StoreIRm8(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123_Minus8))(p); }
		inline void SetSXY012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSXY012))(a, b, c); }
		inline void NCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
		inline void ReadMAC0(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(p); }
		inline void SetSZ123(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSZ123))(a, b, c); }
		inline void SetSZ0123(uint32_t a, uint32_t b, uint32_t c, uint32_t d) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSZ0123))(a, b, c, d); }
		inline void AVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
		inline void AVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
		inline void ReadOTZ(void *p) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(p); }
		inline void LoadV1(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV1))(v); }
		inline void LoadV2(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV2))(v); }
		inline void RTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
		inline void ReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	}

	inline int16_t &S16(uint8_t *p, int o) { return *(int16_t *)(p + o); }
	inline uint16_t &U16(uint8_t *p, int o) { return *(uint16_t *)(p + o); }
	inline int32_t &S32(uint8_t *p, int o) { return *(int32_t *)(p + o); }
	inline uint32_t &U32(uint8_t *p, int o) { return *(uint32_t *)(p + o); }

#pragma pack(push, 1)
	struct Shard
	{
		int16_t pos[3];
		int16_t type;
		int16_t vel[3];
		int16_t life;
		const void *spin;
		int16_t v[4][3];
		uint32_t uv0;
		uint32_t uv1;
		uint16_t uv2, uv3;
		uint32_t color;
	};
	struct Spark
	{
		const uint8_t *seq;
		int16_t pos[3];
		int16_t scale;
		int8_t vel[3];
		uint8_t life;
	};
	struct PrimCtx
	{
		int16_t pos[3];
		int16_t pad06;
		uint32_t modes;
		int16_t bias;
		int16_t scroll;
		uint32_t scratch;
	};
	struct MasterNode
	{
		TaskNode hdr;
		uint16_t counter;
		uint8_t parity;
		uint8_t spawned;
	};
	struct TimelineNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t pad0E;
		uint32_t voice;
		uint8_t pad14[8];
		uint8_t prim[0x8D4];
	};
#pragma pack(pop)
	static_assert(sizeof(Shard) == 0x3C, "shard is 0x3C bytes");
	static_assert(sizeof(Spark) == 0x10, "spark is 0x10 bytes");
	static_assert(sizeof(PrimCtx) == 0x14, "prim ctx is 0x14 bytes");
	static_assert(sizeof(MasterNode) == 0x10, "master node is 0x10 bytes");
	static_assert(sizeof(TimelineNode) == 0x8F0, "timeline node is 0x8F0 bytes");

	static const int SHARD_COUNT = 0x16C, SPARK_COUNT = 0x80;
}
}

namespace ff8fx
{
namespace g069
{
	static int32_t Lcg()
	{
		uint32_t s = (LcgSeed() * 125 + 14) & 0x7FFF;
		LcgSeed() = s;
		return (int32_t)s;
	}

	static uint8_t *ShardAlloc()
	{
		uint8_t *mb = ModelBuffer();
		uint8_t *e = ShardHint();
		if (!e || S16(e, 6) != 0)
		{
			e = mb;
			if (S16(mb, 6) != 0)
			{
				int n = SHARD_COUNT;
				for (;;)
				{
					e += 0x3C;
					if (--n == 0) return nullptr;
					if (S16(e, 6) == 0) break;
				}
			}
		}
		ShardHint() = e < mb + 0x5514 ? e + 0x3C : mb;
		return e;
	}

	static void VertexGroup(uint8_t **pobj, uint8_t **pvcur, uint8_t *sc)
	{
		uint8_t *src = *pobj, *dst = *pvcur;
		int32_t n = *(int16_t *)src;
		src += 2;
		if (n)
		{
			int32_t k = n;
			do
			{
				w::LoadV0u(src);
				GteMVMVA_RotV0Tr();
				dst += 0x10;
				src += 6;
				w::StoreIRm8(dst);
			} while (--k);
		}
		*pobj = src;
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		dst = *pvcur;
		if (n)
		{
			int32_t k = n;
			do
			{
				GteLoadV0(dst + 8);
				GteRTPS();
				GteReadSXY2(sc + 0x20);
				GteReadOTZ(sc + 0x24);
				if (U16(sc, 0x24) == 0) U16(dst, 6) = 0x10;
				else
				{
					int32_t sx = S16(sc, 0x20);
					U16(sc, 0x26) = 0;
					if (sx < shl32(S16(sc, 0x0C), 3)) U16(sc, 0x26) = 1;
					else if (sx >= shl32(S16(sc, 0x10), 3)) U16(sc, 0x26) = 2;
					int32_t sy = S16(sc, 0x22);
					if (sy < shl32(S16(sc, 0x0E), 3)) sc[0x26] |= 4;
					else if (sy >= shl32(S16(sc, 0x12), 3)) sc[0x26] |= 8;
					U32(dst, 0) = U32(sc, 0x20);
					U32(dst, 4) = U32(sc, 0x24);
				}
				dst += 0x10;
			} while (--k);
		}
		*pvcur = dst;
	}

	static inline void CopyVertex(uint8_t *sc, int o, const uint8_t *verts, uint16_t idx)
	{
		ff8fx::copy<(16)>(sc + o, verts + 16 * (uint32_t)(idx & 0xFFF));
	}

	static void ShardVelocity(uint8_t *s, uint16_t m)
	{
		int32_t r = Lcg();
		S16(s, 0x08) = (int16_t)((r & m) - ((int16_t)m >> 1));
		r = Lcg();
		S16(s, 0x0C) = (int16_t)((r & m) - ((int16_t)m >> 1));
		r = Lcg();
		S16(s, 0x0A) = (int16_t)-(4 * (int32_t)m + 4 * (r & m));
		S16(s, 0x0E) = (int16_t)((Lcg() & 7) + 0xC);
	}

	static uint32_t Faces(uint8_t *sc, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *f = *(uint8_t **)sc;
		const uint8_t *verts = *(uint8_t **)(sc + 4);
		uint8_t *pkt = (uint8_t *)cursor;
		uint32_t thr = U16(sc, 0x14);

		for (;;)
		{
			int16_t c = S16(sc, 8);
			S16(sc, 8) = (int16_t)(c - 1);
			if (!c) break;
			int32_t *dp = *(int32_t **)(sc + 0x18);
			int32_t d = *dp;
			*(int32_t **)(sc + 0x18) = dp + 1;
			if (d != -1)
			{
				CopyVertex(sc, 0x20, verts, U16(f, 0));
				CopyVertex(sc, 0x30, verts, U16(f, 2));
				CopyVertex(sc, 0x40, verts, U16(f, 4));
				if (d < (int32_t)thr)
				{
					uint8_t *s = ShardAlloc();
					if (s)
					{
						int16_t x = (int16_t)(((int32_t)S16(sc, 0x48) + S16(sc, 0x38) + S16(sc, 0x28)) / 3);
						S16(s, 0) = x;
						int32_t y = ((int32_t)S16(sc, 0x4A) + S16(sc, 0x3A) + S16(sc, 0x2A)) / 3;
						S16(s, 2) = (int16_t)y;
						int32_t z = ((int32_t)S16(sc, 0x2C) + S16(sc, 0x4C) + S16(sc, 0x3C)) / 3;
						S16(s, 6) = 1;
						S16(s, 4) = (int16_t)z;
						for (int k = 0; k < 3; k++)
						{
							S16(s, 0x14 + 6 * k) = (int16_t)(S16(sc, 0x28 + 0x10 * k) - x);
							S16(s, 0x16 + 6 * k) = (int16_t)(S16(sc, 0x2A + 0x10 * k) - (int16_t)y);
							S16(s, 0x18 + 6 * k) = (int16_t)(S16(sc, 0x2C + 0x10 * k) - (int16_t)z);
						}
						ShardVelocity(s, U16(sc, 0x16));
						U32(s, 0x2C) = U32(f, 8);
						U32(s, 0x30) = U32(f, 0xC);
						U16(s, 0x34) = U16(f, 6);
						U32(s, 0x38) = U32(f, 0x10);
						U32(s, 0x10) = U32(sc, 0x1C) + 20 * (uint32_t)(Lcg() & 0x3F);
						(*(int32_t **)(sc + 0x18))[-1] = -1;
					}
				}
				else
				{
					uint8_t o0 = sc[0x26], o1 = sc[0x36], o2 = sc[0x46];
					if ((uint8_t)(o0 | o1 | o2) < 0x10 && !(o2 & (uint8_t)(o0 & o1)))
					{
						w::SetSXY012(U32(sc, 0x20), U32(sc, 0x30), U32(sc, 0x40));
						w::NCLIP();
						w::ReadMAC0(sc + 0x60);
						if (S32(sc, 0x60) >= 0)
						{
							w::SetSZ123(U16(sc, 0x24), U16(sc, 0x34), U16(sc, 0x44));
							w::AVSZ3();
							U32(pkt, 0) = 0x07000000;
							U32(pkt, 8) = U32(sc, 0x20);
							U32(pkt, 0x10) = U32(sc, 0x30);
							U32(pkt, 0x18) = U32(sc, 0x40);
							U32(pkt, 4) = ((uint32_t)(U16(f, 0xE) & 0x200) << 16) | U32(f, 0x10);
							U32(pkt, 0xC) = U32(f, 8);
							U32(pkt, 0x14) = U32(f, 0xC);
							U16(pkt, 0x16) &= 0xFDFF;
							U16(pkt, 0x1C) = U16(f, 6);
							w::ReadOTZ(sc + 0x68);
							InsertPrimAutoDepth((uint32_t)(ot + 4 * (uint32_t)(S32(sc, 0x68) >> (shift & 31))), pkt);
							pkt += 0x20;
						}
					}
				}
			}
			f += 0x14;
		}

		for (;;)
		{
			int16_t c = S16(sc, 0xA);
			S16(sc, 0xA) = (int16_t)(c - 1);
			if (!c) break;
			int32_t *dp = *(int32_t **)(sc + 0x18);
			int32_t d = *dp;
			*(int32_t **)(sc + 0x18) = dp + 1;
			if (d != -1)
			{
				CopyVertex(sc, 0x20, verts, U16(f, 0));
				CopyVertex(sc, 0x30, verts, U16(f, 2));
				CopyVertex(sc, 0x40, verts, U16(f, 4));
				CopyVertex(sc, 0x50, verts, U16(f, 6));
				if (d < (int32_t)thr)
				{
					uint8_t *s = ShardAlloc();
					if (s)
					{
						int16_t x = (int16_t)(((int32_t)S16(sc, 0x48) + S16(sc, 0x38) + S16(sc, 0x58) + S16(sc, 0x28)) / 4);
						S16(s, 0) = x;
						int32_t y = ((int32_t)S16(sc, 0x4A) + S16(sc, 0x3A) + S16(sc, 0x5A) + S16(sc, 0x2A)) / 4;
						S16(s, 2) = (int16_t)y;
						int32_t z = ((int32_t)S16(sc, 0x5C) + S16(sc, 0x2C) + S16(sc, 0x4C) + S16(sc, 0x3C)) / 4;
						S16(s, 6) = 2;
						S16(s, 4) = (int16_t)z;
						for (int k = 0; k < 4; k++)
						{
							S16(s, 0x14 + 6 * k) = (int16_t)(S16(sc, 0x28 + 0x10 * k) - x);
							S16(s, 0x16 + 6 * k) = (int16_t)(S16(sc, 0x2A + 0x10 * k) - (int16_t)y);
							S16(s, 0x18 + 6 * k) = (int16_t)(S16(sc, 0x2C + 0x10 * k) - (int16_t)z);
						}
						ShardVelocity(s, U16(sc, 0x16));
						U32(s, 0x2C) = U32(f, 8);
						U32(s, 0x30) = U32(f, 0xC);
						U16(s, 0x34) = U16(f, 0x10);
						U16(s, 0x36) = U16(f, 0x12);
						U32(s, 0x38) = U32(f, 0x14);
						U32(s, 0x10) = U32(sc, 0x1C) + 20 * (uint32_t)(Lcg() & 0x3F);
						(*(int32_t **)(sc + 0x18))[-1] = -1;
					}
				}
				else
				{
					uint8_t o3 = sc[0x56], o0 = sc[0x26], o1 = sc[0x36], o2 = sc[0x46];
					if ((uint8_t)(o3 | o0 | o1 | o2) < 0x10 && !(o2 & (uint8_t)(o3 & o0 & o1)))
					{
						w::SetSXY012(U32(sc, 0x20), U32(sc, 0x30), U32(sc, 0x40));
						w::NCLIP();
						w::ReadMAC0(sc + 0x60);
						if (S32(sc, 0x60) >= 0)
						{
							w::SetSZ0123(U16(sc, 0x24), U16(sc, 0x34), U16(sc, 0x44), U16(sc, 0x54));
							w::AVSZ4();
							U32(pkt, 0) = 0x09000000;
							U32(pkt, 8) = U32(sc, 0x20);
							U32(pkt, 0x10) = U32(sc, 0x30);
							U32(pkt, 0x18) = U32(sc, 0x40);
							U32(pkt, 0x20) = U32(sc, 0x50);
							U32(pkt, 4) = ((uint32_t)(U16(f, 0xE) & 0x200) << 16) | U32(f, 0x14);
							U32(pkt, 0xC) = U32(f, 8);
							U32(pkt, 0x14) = U32(f, 0xC);
							U16(pkt, 0x1C) = U16(f, 0x10);
							U16(pkt, 0x16) &= 0xFDFF;
							U16(pkt, 0x24) = U16(f, 0x12);
							w::ReadOTZ(sc + 0x68);
							InsertPrimAutoDepth((uint32_t)(ot + 4 * (uint32_t)(S32(sc, 0x68) >> (shift & 31))), pkt);
							pkt += 0x28;
						}
					}
				}
			}
			f += 0x18;
		}
		return (uint32_t)pkt;
	}

	static uint32_t Objects(uint8_t *com, uint8_t *sc, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *bones = *(uint8_t **)com + 0x10;
		const uint32_t *offs = (const uint32_t *)(*(uint8_t **)(com + 4) + 4);
		int32_t count = (int32_t)offs[-1];
		if (count <= 0) return cursor;
		do
		{
			uint8_t *obj = *(uint8_t **)(com + 4) + *offs++;
			uint8_t *vcur = *(uint8_t **)(sc + 4);
			int32_t groups = *(int16_t *)obj;
			obj += 2;
			if (groups > 0)
			{
				int32_t g = groups;
				do
				{
					int32_t bone = *(int16_t *)obj;
					obj += 2;
					const Mat4x3 *m = (const Mat4x3 *)(bones + 48 * bone + 0x10);
					GteSetRotMatrixCtrl(m);
					GteSetTransVectorCtrl(m);
					VertexGroup(&obj, &vcur, sc);
				} while (--g);
			}
			obj = (uint8_t *)(((uint32_t)obj + 3) & ~3u) + 4;
			S16(sc, 8) = *(int16_t *)obj;
			obj += 2;
			S16(sc, 0xA) = *(int16_t *)obj;
			obj += 6;
			*(uint8_t **)sc = obj;
			cursor = Faces(sc, ot, shift, cursor);
		} while (--count);
		return cursor;
	}

	static uint8_t *ShatterDraw(uint8_t *model, uint8_t *dist, int32_t thr, int32_t mask, uint8_t *scratch,
		uint32_t ot, int32_t shift)
	{
		uint8_t *sc = (uint8_t *)FieldAlloc(0x70);
		U16(sc, 0x14) = (uint16_t)thr;
		*(uint8_t **)(sc + 0x18) = dist;
		U16(sc, 0x16) = (uint16_t)mask;
		uint8_t *hdr = model + 0x14;
		*(uint8_t **)(sc + 0x1C) = ModelBuffer() + MB_SPIN;
		BuildBoneMatricesFromPose(hdr);
		*(uint8_t **)(sc + 4) = scratch;
		S16(sc, 0xC) = 0;
		S16(sc, 0xE) = 0;
		S16(sc, 0x10) = 0x140;
		S16(sc, 0x12) = 0xD8;
		FrameCursor() = Objects(*(uint8_t **)(hdr + 4), sc, ot, shift, FrameCursor());
		uint8_t *r = *(uint8_t **)(sc + 0x18);
		FieldFree(0x70);
		return r;
	}

	static void ShardsDraw(const uint8_t *pool)
	{
		uint8_t *pkt = (uint8_t *)FrameCursor();
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		int16_t *tmp = (int16_t *)FieldAlloc(8);
		const uint8_t *e = pool;
		for (int n = SHARD_COUNT; n; n--, e += 0x3C)
		{
			const Shard *s = (const Shard *)e;
			if (s->type == 0) continue;
			uint8_t *base = pkt;
			tmp[0] = (int16_t)(s->v[0][0] + s->pos[0]);
			tmp[1] = (int16_t)(s->v[0][1] + s->pos[1]);
			tmp[2] = (int16_t)(s->v[0][2] + s->pos[2]);
			GteLoadV0(tmp);
			tmp[0] = (int16_t)(s->v[1][0] + s->pos[0]);
			tmp[1] = (int16_t)(s->v[1][1] + s->pos[1]);
			tmp[2] = (int16_t)(s->v[1][2] + s->pos[2]);
			w::LoadV1(tmp);
			tmp[0] = (int16_t)(s->v[2][0] + s->pos[0]);
			tmp[1] = (int16_t)(s->v[2][1] + s->pos[1]);
			tmp[2] = (int16_t)(s->v[2][2] + s->pos[2]);
			w::LoadV2(tmp);
			w::RTPT();
			U32(pkt, 0xC) = s->uv0;
			U32(pkt, 0x14) = s->uv1;
			U16(pkt, 0x1C) = s->uv2;
			w::ReadSXY012(pkt + 8, pkt + 0x10, pkt + 0x18);
			uint32_t out = 0;
			int16_t v = S16(pkt, 8);
			if (v < 0 || v > 0xA00) out = 1;
			v = S16(pkt, 0x10);
			if (v < 0 || v > 0xA00) out |= 2;
			v = S16(pkt, 0x18);
			if (v < 0 || v > 0xA00) out |= 4;
			v = S16(pkt, 0xA);
			if (v < 0 || v > 0x6C0) out |= 0x10;
			v = S16(pkt, 0x12);
			if (v < 0 || v > 0x6C0) out |= 0x20;
			v = S16(pkt, 0x1A);
			if (v < 0 || v > 0x6C0) out |= 0x40;
			uint32_t size;
			if (s->type == 2)
			{
				tmp[0] = (int16_t)(s->v[3][0] + s->pos[0]);
				tmp[1] = (int16_t)(s->v[3][1] + s->pos[1]);
				tmp[2] = (int16_t)(s->v[3][2] + s->pos[2]);
				GteLoadV0(tmp);
				GteRTPS();
				U16(pkt, 0x24) = s->uv3;
				GteReadSXY2(pkt + 0x20);
				w::AVSZ4();
				v = S16(pkt, 0x20);
				if (v < 0 || v > 0xA00) out |= 8;
				v = S16(pkt, 0x22);
				if (v < 0 || v > 0x6C0) out |= 0x80;
				if ((out & 0xF) == 0xF || (out & 0xF0) == 0xF0) continue;
				w::ReadOTZ(tmp);
				if (tmp[0] <= 0) continue;
				U32(pkt, 0) = 0x09000000;
				U32(pkt, 4) = 0x2CF0F0F0;
				size = 0x28;
			}
			else
			{
				w::AVSZ3();
				if ((out & 7) == 7 || (out & 0x70) == 0x70) continue;
				U32(pkt, 0) = 0x07000000;
				U32(pkt, 4) = 0x24F0F0F0;
				w::ReadOTZ(tmp);
				if (tmp[0] <= 0) continue;
				size = 0x20;
			}
			pkt += size;
			InsertPrimAutoDepth(RenderList() + 4 * (uint32_t)((int32_t)tmp[0] >> 2), base);
		}
		FieldFree(8);
		FrameCursor() = (uint32_t)pkt;
	}

	static void SparksDraw(const uint8_t *pool)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		U16(h, 0x60) = 0;
		U16(h, 0x58) = 0;
		U16(h, 0x50) = 0;
		U16(h, 0x48) = 0;
		const uint8_t *e = pool;
		for (int n = SPARK_COUNT; n; n--, e += 0x10)
		{
			const Spark *s = (const Spark *)e;
			if (s->life == 0) continue;
			GteSetRotMatrixCtrl(&Camera());
			GteSetTransVectorCtrl(&Camera());
			GteLoadV0(s->pos);
			GteMVMVA_RotV0Tr();
			GteReadMAC123((int32_t *)(h + 0x98));
			ff8fx::fill<(0x14)>(h + 0x84, 0);
			U16(h, 0x94) = (uint16_t)s->scale;
			U16(h, 0x8C) = (uint16_t)s->scale;
			U16(h, 0x84) = (uint16_t)s->scale;
			const uint8_t *seq = s->seq;
			*(const uint8_t **)h = seq;
			int32_t frames = *(const int16_t *)(seq + 8);
			int32_t k = frames - ((int32_t)(int8_t)s->life - 1) % frames;
			const uint8_t *frame = seq + *(const int16_t *)(seq + 8 + 2 * k);
			*(const uint8_t **)(h + 0x2C) = frame;
			int32_t word = *(const int32_t *)frame;
			S32(h, 0x30) = word;
			if (word < 0)
			{
				h[0x25] |= 1;
				U32(h, 0x30) = (uint32_t)word & 0x7FFFFFFF;
			}
			*(const uint8_t **)(h + 0x2C) = frame + 4;
			FrameCursor() = w::Flipbook(h, RenderList() + 0x44, 2, FrameCursor());
		}
		FieldFree(0xB4);
	}

	static int32_t FadeLevel(int32_t t, int32_t in = 0xE, int32_t hold = 4, int32_t out = 0xE)
	{
		if (t < in) return shl32(t + 1, 12) / in;
		if (t < hold + in) return 0x1000;
		return (shl32(in - t + hold, 12) - 0x1000) / out + 0x1000;
	}

	static uint32_t __cdecl SequenceTask(TaskNode *n)
	{
		MasterNode *node = (MasterNode *)n;
		if (node->parity)
		{
			PacketCursor() = (uint32_t)(ModelBuffer() + MB_ARENA_A);
			node->parity = 0;
		}
		else
		{
			PacketCursor() = (uint32_t)(ModelBuffer() + MB_ARENA_B);
			node->parity = 1;
		}

		if (node->counter == 1 && !node->spawned)
		{
			node->spawned = 1;
			w::InitSparks();
			w::InitShards();
			InitTaskQueuePool(&QueueTimeline(), (void *)game_addr(game::unk_2556680), 0x8F0, 1);
			TaskNode *t = AddTaskToQueue(&QueueTimeline(), game_addr(ORIG_TimelineTask));
			Memset32((uint8_t *)t + 0xC, 0, 0x239);
			*(uint32_t *)((uint8_t *)t + 0x10) = ClaimVoiceSlot((const void *)game_addr(game::unk_14D0740), 1, 0x80);
			uint8_t *a = Attacker();
			Mat4x3 m;
			m.m[0][0] = 0x400; m.m[0][1] = 0; m.m[0][2] = 0;
			m.m[1][0] = 0; m.m[1][1] = 0x800; m.m[1][2] = 0;
			m.m[2][0] = 0; m.m[2][1] = 0; m.m[2][2] = 0x400;
			m.pad = 0;
			m.t[0] = -((int32_t)*(int16_t *)(a + 0x1C) / 4);
			m.t[1] = 0;
			m.t[2] = -((int32_t)*(int16_t *)(a + 0x20) / 4);
			uint8_t *r = w::FaceDistances(MODEL_1(), &m, ModelBuffer() + MB_FACE_DIST);
			w::FaceDistances(MODEL_2(), &m, r);
		}

		int left = 1;
		if (node->spawned)
		{
			EffectCameraMatrix(&Camera(), &EffectCamera());
			left = ExecuteTaskQueue(&QueueTimeline());
		}
		if (node->spawned && left == 0) return TASK_END;
		node->counter++;
		return 0;
	}

	static uint32_t __cdecl TimelineTask(TaskNode *n)
	{
		TimelineNode *node = (TimelineNode *)n;
		uint8_t *MB = ModelBuffer();
		int32_t c = node->counter;

		if ((uint32_t)c < 0x5C)
		{
			if (c == 0) w::Decode(game_addr(game::unk_14D0C2C), node->prim, 0x8D4);
			PrimCtx ctx;
			uint8_t *a = Attacker();
			ctx.pos[0] = *(int16_t *)(a + 0x1C);
			ctx.pos[1] = 0;
			ctx.pos[2] = *(int16_t *)(a + 0x20);
			ctx.pad06 = 0;
			ctx.modes = game_addr(game::a000100200);
			ctx.bias = -128;
			ctx.scratch = (uint32_t)(MB + MB_SCRATCH);
			ctx.scroll = (int16_t)(c << 4);
			prim::play((prim::Layout *)node->prim, (prim::Callback)game_addr(CB_PrimObject), (int)&ctx, 0);
		}

		{
			int32_t d = (int32_t)node->counter - 0x37;
			bool draw = false;
			if (d == 0)
			{
				*(uint16_t *)Attacker() &= 0xFFF7;
				draw = true;
			}
			else if ((uint32_t)d < 0x1E) draw = true;
			else if (d == 0x1E) *(uint16_t *)Attacker() &= 0xFFFB;
			if (draw)
			{
				w::AttackerDraw(Attacker(), 0, var<uint32_t>(game_addr(game::g_BattleVertexWorkspace)));
				*Attacker() |= 4;
			}
		}

		{
			uint32_t v = (uint32_t)((int32_t)node->counter - 0x49);
			if (v < 0x23)
			{
				int32_t q = (int32_t)((v * 28000u) / 50u);
				if (q > 6000) q = 6000;
				int32_t thr = (q * q) >> 12;
				uint8_t *r = ShatterDraw(MODEL_1(), MB + MB_FACE_DIST, thr, 0x3F, MB + MB_SCRATCH, RenderList() + 0x4068, 0x10);
				ShatterDraw(MODEL_2(), r, thr, 0x3F, MB + MB_SCRATCH, RenderList() + 0x44, 0);
				*MODEL_1_FLAGS() &= 0xFD;
				*MODEL_2_FLAGS() &= 0xFD;
			}
			else
			{
				*MODEL_1_FLAGS() |= 2;
				*MODEL_2_FLAGS() |= 2;
			}
		}

		if (node->counter < 0x30)
		{
			SparksDraw(MB + MB_SPARKS);
		}
		ShardsDraw(MB + MB_SHARDS);

		{
			uint32_t t = (uint32_t)((int32_t)node->counter - 0x5E);
			if (t < 0x20)
			{
				PacketCursor() = w::Tile(0xFF, 0xFF, 0xFF, FadeLevel((int32_t)t), PacketCursor());
			}
		}

		if (node->counter == 0) w::PlaySE(game_addr(game::unk_14D0A48), 0, 0x80);
		if (node->counter == 0x7A) w::ReleaseVoice(node->voice);
		if (node->counter < 0x2F) w::SparkUpdate();
		w::ShardUpdate();

		uint32_t flags = var<uint32_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C));
		if (flags & 0x201)
		{
			if (flags & 1) return 0;
			if (w::Sub508500() < 0) return 0;
		}

		if (node->counter == 0x32) w::LoadBS(0, MB + MB_SPARKS);
		else if (node->counter == 0x6E)
		{
			if (w::Sub508500() < 0) return 0;
			w::SwapBS(0, (uint32_t)(ModelBuffer() + MB_SPARKS));
			*Attacker() |= 0x20;
			uint32_t pal = var<uint32_t>(game_addr(game::CURRENT_BS_SHADOW_PALETTE));
			for (uint32_t a = game_addr(game::unk_1D972EC); a < game_addr(game::unk_1D97730); a += 0x9C) var<uint32_t>(a) = pal;
		}

		{
			uint32_t t = (uint32_t)((int32_t)node->counter - 0x5E);
			if (t < 0x20)
			{
				*(int16_t *)(Attacker() + 0x20) = (int16_t)(-2000 - (int32_t)(t * 1400) / 31);
				*(int32_t *)(Attacker() + 0x5C) = *(int16_t *)(Attacker() + 0x20);
			}
		}

		if ((uint16_t)node->counter < 0x1E)
		{
			int k = 4;
			int32_t angle = w::Rand();
			uint8_t *s = w::SparkAlloc();
			while (s)
			{
				int32_t r = w::Rand() & 0x7FF;
				int32_t sn = ComputeSin(angle);
				*(int16_t *)(s + 4) = (int16_t)(mul32(sn, r) / 4096 + *(int16_t *)(Attacker() + 0x1C));
				int32_t cs = w::Cos(angle);
				int16_t z = (int16_t)(mul32(cs, r) / 4096 + *(int16_t *)(Attacker() + 0x20));
				*(int16_t *)(s + 6) = 0;
				*(uint32_t *)s = game_addr(game::unk_14D0A4C);
				*(int16_t *)(s + 8) = z;
				s[0xC] = (uint8_t)((w::Rand() & 3) - 2);
				s[0xE] = (uint8_t)((w::Rand() & 3) - 2);
				s[0xD] = (uint8_t)(0xF0 - (w::Rand() & 0x1F));
				angle += 0x400;
				*(int16_t *)(s + 0xA) = 0x800;
				s[0xF] = 0x12;
				if (--k == 0) break;
				s = w::SparkAlloc();
			}
		}

		if (node->counter >= 0x7E) return TASK_END;
		node->counter = (int16_t)(node->counter + 1);
		return 0;
	}
}

	void register_mag069_griever()
	{
		register_port(g069::ORIG_SequenceTask, (void *)g069::SequenceTask, "G069 SequenceTask", 69);
		register_port(g069::ORIG_TimelineTask, (void *)g069::TimelineTask, "G069 TimelineTask", 69);
	}
}

