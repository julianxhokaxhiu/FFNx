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

namespace ff8fx
{
namespace xatm276
{
	using namespace eng;
	using namespace magc;

	template<typename T> inline T &AT(uint32_t p, int32_t o) { return *(T *)(p + o); }
	inline int16_t &S16(uint32_t p, int32_t o) { return AT<int16_t>(p, o); }
	inline uint16_t &U16(uint32_t p, int32_t o) { return AT<uint16_t>(p, o); }
	inline int32_t &S32(uint32_t p, int32_t o) { return AT<int32_t>(p, o); }
	inline uint32_t &U32(uint32_t p, int32_t o) { return AT<uint32_t>(p, o); }
	inline uint8_t &U8(uint32_t p, int32_t o) { return AT<uint8_t>(p, o); }

	inline uint32_t &Caster() { return var<uint32_t>(game_addr(game::MAG_276_CasterEntity)); }
	inline uint32_t &Buffer() { return var<uint32_t>(game_addr(game::MAG_276_MagicBuffer)); }
	inline uint32_t &Seed() { return var<uint32_t>(game_addr(game::MAG_276_RandSeed)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_276_PacketCursor)); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t OTBase() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }
	inline TaskQueue *QSmoke() { return (TaskQueue *)game_addr(game::MAG_276_SmokeQueue); }
	inline TaskQueue *QFall() { return (TaskQueue *)game_addr(game::MAG_276_FallingSparkQueue); }
	inline TaskQueue *QSpark() { return (TaskQueue *)game_addr(game::MAG_276_SparkQueue); }
	inline TaskQueue *QPrim() { return (TaskQueue *)game_addr(game::MAG_276_PrimQueue); }
	inline TaskQueue *QArc() { return (TaskQueue *)game_addr(game::MAG_276_ArcQueue); }
	inline TaskQueue *QEvent() { return (TaskQueue *)game_addr(game::MAG_276_EventQueue); }
	inline TaskQueue *QDirector() { return (TaskQueue *)game_addr(game::MAG_276_DirectorQueue); }
	inline Mat4x3 &EffectCamera() { return var<Mat4x3>(game_addr(game::MAG_276_EffectCamera)); }

	static constexpr game::Id ORIG_RootTask = game::MAG_276_RootTask;
	static constexpr game::Id ORIG_Director = game::MAG_276_DirectorTask;
	static constexpr game::Id ORIG_EventTask = game::MAG_276_EventTask;
	static constexpr game::Id ORIG_SmokeTask = game::MAG_276_SmokeTask;
	static constexpr game::Id ORIG_PrimTask = game::MAG_276_PrimModelTask;
	static constexpr game::Id ORIG_SparkTask = game::MAG_276_SparkTask;
	static constexpr game::Id ORIG_FallTask = game::MAG_276_FallingSparkTask;
	static constexpr game::Id ORIG_ArcTask = game::MAG_276_ArcTask;

	static constexpr game::Id SEQ_Smoke = game::MAG_276_Xatm092Death_SeqSmoke, SEQ_Fall = game::unk_10D304C, SEQ_Spark = game::unk_10D31C4;
	static constexpr game::Id PRIM_Model = game::unk_10D3364;
	static constexpr game::Id DEBRIS_Shapes = game::MAG_276_DebrisShapes;
	static constexpr game::Id ARC_Lengths = game::unk_10D9728;
	static constexpr game::Id ARC_Distance = game::MAG_276_ArcDistanceTable;
	static const void *SOUND_Start() { return (const void *)game_addr(game::MAG_276_Xatm092Death_SoundStart); }
	static const void *SOUND_Voice() { return (const void *)game_addr(game::MAG_276_Xatm092Death_SoundVoice); }
	static constexpr game::Id EVENTS[5] = { game::MAG_276_EventTable0, game::MAG_276_EventTable1, game::MAG_276_EventTable2, game::MAG_276_EventTable3, game::MAG_276_EventTable4 };

	static const uint32_t BUF_PROFILE = 0x4304, BUF_PROFILE_END = 0x4314;
	static const uint32_t BUF_ARENA0 = 0x4314, BUF_ARENA1 = 0x14314;
	static const uint32_t BUF_MORPH = 0x4084;
	static const uint32_t BUF_DIRECTOR = 0x1C30;

	inline void Music_47E3C0(int32_t v) { fn<void (__cdecl *)(int32_t)>(game_addr(game::Battle_SetMusicVolumeUnlessEncounterFlag2))(v); }
	inline void ReleaseVoiceSlot(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
	inline uint32_t DrawShadow(uint32_t entity, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, mode, cursor); }
	inline uint32_t Tile(int32_t r, int32_t g, int32_t b, int32_t level, uint32_t cursor) { return fn<uint32_t (__cdecl *)(int32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_FullScreenTile))(r, g, b, level, cursor); }
	inline void GteLoadV1(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV1))(v); }
	inline void GteLoadV2(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV2))(v); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadSXY012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZ32(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteMVMVA_RotV0() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0))(); }
	inline void GteLoadV0u(const void *v) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0_u16))(v); }
	inline void GteStoreIRVertex(void *after) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreIR123_Minus8))(after); }
	inline void GteSetSXY012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSXY012))(a, b, c); }
	inline void GteSetSZ123(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSZ123))(a, b, c); }
	inline void GteSetSZ0123(uint32_t a, uint32_t b, uint32_t c, uint32_t d) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSZ0123))(a, b, c, d); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteSetRotMatrixW(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetRotMatrix_W))(m); }
	inline void GteSetTransVectorW(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetTransVector_W))(m); }
	inline void GteSetRotCtrl(uint32_t m) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_SetRotMatrix))(m); }
	inline void GteSetTransCtrl(uint32_t m) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_SetTransVector))(m); }
	inline void TransformGroupScreen(uint32_t *objp, uint32_t *outp, uint32_t s) { fn<void (__cdecl *)(uint32_t *, uint32_t *, uint32_t)>(game_addr(game::MAG_212_BURNINGRAVE_GhostVertices))(objp, outp, s); }
	inline void TriangleFrame(uint32_t bones, uint32_t tri, void *centre, void *normal) { fn<void (__cdecl *)(uint32_t, uint32_t, void *, void *)>(game_addr(game::MAG_TriangleCentreNormal))(bones, tri, centre, normal); }
	inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void MatrixMultiply3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void RotationFromAngles(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::Effect_RotationFromAnglesZXY))(angles, out); }
	inline void RotationFromAnglesB(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(angles, out); }
	inline void BlendVertexFrames(uint32_t model, int32_t f0, int32_t f1, int32_t t, uint32_t out) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t, int32_t, uint32_t)>(game_addr(game::Effect_BlendVertexFrames))(model, f0, f1, t, out); }
	inline uint32_t RenderPrimModel2(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_070_RenderPrimModel))(h, ot, mode, cursor); }
	inline void DecodeModelPrimLayout(uint32_t data, void *layout, int32_t size) { fn<void (__cdecl *)(uint32_t, void *, int32_t)>(game_addr(game::Effect_DecodeModelPrimLayout))(data, layout, size); }
	inline int32_t SharedRand() { return fn<int32_t (__cdecl *)()>(game_addr(game::Effect_SharedLcg_Rand))(); }

	static int32_t Rnd()
	{
		uint32_t s = Seed();
		s = s * 5;
		s = s * 5;
		s = s * 5 + 0xE;
		s &= 0x7FFF;
		Seed() = s;
		return (int32_t)s;
	}

#pragma pack(push, 1)
	struct PrimArg
	{
		int16_t pos[4];
		int32_t scale[3];
		int32_t scaled;
		int32_t depth;
		uint32_t morph;
	};
#pragma pack(pop)
	static_assert(sizeof(PrimArg) == 0x20, "X-ATM092 prim block");
	enum : uint8_t { SPR_SPARK = 1, SPR_FALL = 2, SPR_SMOKE = 3 };
}
}

namespace ff8fx
{
namespace xatm276
{
	static inline void CopyVertex(uint32_t s, int32_t off, uint32_t verts, uint16_t index)
	{
		const uint32_t v = verts + (uint32_t)(index & 0xFFF) * 8;
		U32(s, off) = U32(v, 0);
		U32(s, off + 4) = U32(v, 4);
	}

	static uint32_t ModelPolygons(uint32_t s, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint32_t pk = cursor;
		const uint32_t verts = U32(s, 4);
		uint32_t p = U32(s, 0);
		uint16_t k = U16(s, 8);
		U16(s, 8) = (uint16_t)(k - 1);
		if (k != 0)
		{
			do
			{
				CopyVertex(s, 0x1C, verts, U16(p, 0));
				CopyVertex(s, 0x24, verts, U16(p, 2));
				CopyVertex(s, 0x2C, verts, U16(p, 4));
				const uint8_t a = U8(s, 0x22), b = U8(s, 0x2A), c = U8(s, 0x32);
				if ((uint8_t)(a | b | c) < 0x10 && !(c & (a & b)))
				{
					GteSetSXY012(U32(s, 0x1C), U32(s, 0x24), U32(s, 0x2C));
					GteNCLIP();
					GteReadMAC0((void *)(s + 0x3C));
					if (S32(s, 0x3C) >= 0)
					{
						GteSetSZ123(U16(s, 0x20), U16(s, 0x28), U16(s, 0x30));
						GteAVSZ3();
						U32(pk, 0) = 0x7000000;
						U32(pk, 4) = (((uint32_t)U16(p, 0xE) & 0x200 | 0x2400) << 16) | U32(s, 0x14);
						U32(pk, 8) = U32(s, 0x1C);
						U32(pk, 0x10) = U32(s, 0x24);
						U32(pk, 0x18) = U32(s, 0x2C);
						U32(pk, 0xC) = U32(p, 8);
						U32(pk, 0x14) = U32(p, 0xC);
						U16(pk, 0x16) &= 0xFDFF;
						U16(pk, 0x1C) = U16(p, 6);
						GteReadOTZ32((void *)(s + 0x44));
						InsertPrimAutoDepth(ot + (uint32_t)(S32(s, 0x44) >> shift) * 4, (void *)pk);
						pk += 0x20;
					}
				}
				k = U16(s, 8);
				p += 0x10;
				U16(s, 8) = (uint16_t)(k - 1);
			} while (k != 0);
		}
		k = U16(s, 0xA);
		U16(s, 0xA) = (uint16_t)(k - 1);
		if (k != 0)
		{
			p += 4;
			do
			{
				CopyVertex(s, 0x1C, verts, U16(p, -4));
				CopyVertex(s, 0x24, verts, U16(p, -2));
				CopyVertex(s, 0x2C, verts, U16(p, 0));
				CopyVertex(s, 0x34, verts, U16(p, 2));
				const uint8_t d = U8(s, 0x3A), a = U8(s, 0x22), b = U8(s, 0x2A), c = U8(s, 0x32);
				if ((uint8_t)(d | a | b | c) < 0x10 && !(c & (d & a & b)))
				{
					GteSetSXY012(U32(s, 0x1C), U32(s, 0x24), U32(s, 0x2C));
					GteNCLIP();
					GteReadMAC0((void *)(s + 0x3C));
					if (S32(s, 0x3C) >= 0)
					{
						GteSetSZ0123(U16(s, 0x20), U16(s, 0x28), U16(s, 0x30), U16(s, 0x38));
						GteAVSZ4();
						U32(pk, 0) = 0x9000000;
						U32(pk, 4) = (((uint32_t)U16(p, 0xA) & 0x200 | 0x2C00) << 16) | U32(s, 0x14);
						U32(pk, 8) = U32(s, 0x1C);
						U32(pk, 0x10) = U32(s, 0x24);
						U32(pk, 0x18) = U32(s, 0x2C);
						U32(pk, 0x20) = U32(s, 0x34);
						U32(pk, 0xC) = U32(p, 4);
						U32(pk, 0x14) = U32(p, 8);
						U16(pk, 0x1C) = U16(p, 0xC);
						U16(pk, 0x16) &= 0xFDFF;
						U16(pk, 0x24) = U16(p, 0xE);
						GteReadOTZ32((void *)(s + 0x44));
						InsertPrimAutoDepth(ot + (uint32_t)(S32(s, 0x44) >> shift) * 4, (void *)pk);
						pk += 0x28;
					}
				}
				k = U16(s, 0xA);
				p += 0x14;
				U16(s, 0xA) = (uint16_t)(k - 1);
			} while (k != 0);
		}
		return pk;
	}

	static uint32_t ModelDraw(uint32_t model, uint32_t s, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t table = U32(model, 4);
		const uint32_t bones = U32(model, 0) + 0x10;
		const int32_t count = S32(table, 0);
		if (count <= 0) return cursor;
		uint32_t offs = table + 4;
		for (int32_t i = 0; i < count; i++)
		{
			uint32_t obj = U32(model, 4) + U32(offs, 0);
			offs += 4;
			if (!(U32(s, 0x18) & (1u << (i & 31)))) continue;
			uint32_t out = U32(s, 4);
			int32_t groups = S16(obj, 0);
			obj += 2;
			if (groups > 0)
			{
				do
				{
					const int32_t bone = S16(obj, 0);
					obj += 2;
					const uint32_t rec = bones + (uint32_t)(bone * 0x30);
					if (S16(rec, 0x22) == 0)
					{
						GteSetRotCtrl(rec + 0x10);
						GteSetTransCtrl(rec + 0x10);
						TransformGroupScreen(&obj, &out, s);
					}
					else
					{
						const int32_t n = S16(obj, 0);
						obj += (uint32_t)(n * 6 + 2);
						for (int32_t v = n; v != 0; v--)
						{
							S16(out, 6) = 0x10;
							out += 8;
						}
					}
				} while (--groups != 0);
			}
			obj = (obj + 3) & ~3u;
			U16(s, 8) = U16(obj, 0);
			obj += 2;
			U16(s, 0xA) = U16(obj, 0);
			obj += 0xA;
			U32(s, 0) = obj;
			cursor = ModelPolygons(s, ot, shift, cursor);
		}
		return cursor;
	}

	static void DrawCaster(uint32_t entity, uint32_t list)
	{
		const uint32_t b = (uint32_t)FieldAlloc(0x6C);
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(entity + 0x40), (Mat4x3 *)b);
		FrameCursor() = DrawShadow(entity, OTBase() + 0x4040, 0x10, FrameCursor());
		ComputeBonesWorldMatrices((void *)(entity + 0x60), (void *)b);
		U32(b, 0x24) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		S16(b, 0x2C) = 0;
		S16(b, 0x2E) = 0;
		S16(b, 0x30) = 0x140;
		S16(b, 0x32) = 0xD8;
		U32(b, 0x34) = U32(entity, 0x28);
		U32(b, 0x38) = U32(entity, 0x7C);
		const uint32_t sk = U32(U32(entity, 0x64), 0);
		for (uint32_t i = 0, n = U8(sk, 0); i < n; i++) S16(sk, 0x32 + 0x30 * i) = 0;
		for (uint32_t i = 0, n = U8(sk, 0); i < n; i++)
		{
			const uint8_t id = U8(list, i);
			if (id != 0xFF) S16(sk, (int32_t)id * 0x30 + 0x32) = 1;
		}
		FrameCursor() = ModelDraw(U32(entity, 0x64), b + 0x20, OTBase() + 0x44, 0, FrameCursor());
		BuildBoneMatricesFromPose((void *)(entity + 0x60));
		FieldFree(0x6C);
	}

	static uint32_t AllocDebris(uint32_t node)
	{
		uint32_t r = U32(node, 0x50);
		if (S16(r, 6) != 0)
		{
			r = node + 0x54;
			if (S16(node, 0x5A) != 0)
			{
				int32_t left = 0x80;
				do
				{
					r += 0x48;
					if (--left == 0) return 0;
				} while (S16(r, 6) != 0);
			}
		}
		U32(node, 0x50) = r >= node + 0x240C ? node + 0x54 : r + 0x48;
		return r;
	}

	static inline uint8_t Shrink(uint32_t v, int32_t avg, int32_t centre_lo) { return (uint8_t)((uint8_t)((int32_t)(v - (uint32_t)avg) >> 1) + (uint8_t)centre_lo); }

	static void SpawnDebris(uint32_t s)
	{
		const uint32_t verts = U32(s, 4);
		uint32_t p = U32(s, 0);
		uint16_t k = U16(s, 8);
		U16(s, 8) = (uint16_t)(k - 1);
		if (k != 0)
		{
			do
			{
				if (Rnd() <= S32(s, 0x14))
				{
					CopyVertex(s, 0x24, verts, U16(p, 0));
					CopyVertex(s, 0x2C, verts, U16(p, 2));
					CopyVertex(s, 0x34, verts, U16(p, 4));
					if ((uint8_t)(U8(s, 0x3A) | U8(s, 0x32) | U8(s, 0x2A)) == 0)
					{
						const uint32_t r = AllocDebris(U32(s, 0x20));
						if (!r) { S32(s, 0x18) = 0; return; }
						S16(r, 0) = (int16_t)(((int32_t)S16(s, 0x24) + S16(s, 0x2C) + S16(s, 0x34)) / 3);
						S16(r, 2) = (int16_t)(((int32_t)S16(s, 0x36) + S16(s, 0x2E) + S16(s, 0x26)) / 3);
						S16(r, 6) = 1;
						S16(r, 4) = (int16_t)(((int32_t)S16(s, 0x38) + S16(s, 0x30) + S16(s, 0x28)) / 3);
						GteSetRotCtrl(U32(s, 0x8C) + (uint32_t)((Rnd() & 0x3F) << 5));
						GteLoadV0((void *)(s + 0x54));
						GteMVMVA_RotV0();
						GteStoreIR123((void *)(r + 0x24));
						GteLoadV0((void *)(s + 0x5C));
						GteMVMVA_RotV0();
						GteStoreIR123((void *)(r + 0x2A));
						GteLoadV0((void *)(s + 0x64));
						GteMVMVA_RotV0();
						GteStoreIR123((void *)(r + 0x30));
						const int32_t c = S32(s, 0x1C);
						S16(r, 8) = (int16_t)((int32_t)((uint32_t)Rnd() & (0xFFFF0000u | (uint16_t)c)) - (c >> 1));
						S16(r, 0xC) = (int16_t)((int32_t)((uint32_t)Rnd() & (0xFFFF0000u | (uint16_t)c)) - (c >> 1));
						const int32_t half = c >> 1;
						S16(r, 0xA) = (int16_t)((Rnd() & half) + (c >> 3) - half);
						S16(r, 0xE) = (int16_t)((Rnd() & 7) + 0xC);
						U32(r, 0x3C) = U32(p, 8);
						U32(r, 0x40) = U32(p, 0xC);
						U16(r, 0x44) = U16(p, 6);
						{
							const int32_t u0 = U8(r, 0x3C), u1 = U8(r, 0x40), u2 = U8(r, 0x44);
							const int32_t avg = (u0 + u1 + u2) / 3;
							U8(r, 0x3C) = Shrink(u0, (int16_t)avg, avg);
							U8(r, 0x44) = Shrink(u2, (int16_t)avg, avg);
							U8(r, 0x40) = Shrink(u1, (int16_t)avg, avg);
						}
						{
							const int32_t v0 = U8(r, 0x3D), v1 = U8(r, 0x41), v2 = U8(r, 0x45);
							const int32_t avg = (v0 + v1 + v2) / 3;
							U8(r, 0x3D) = Shrink(v0, (int16_t)avg, avg);
							U8(r, 0x45) = Shrink(v2, (int16_t)avg, avg);
							U8(r, 0x41) = Shrink(v1, (int16_t)avg, avg);
						}
						const uint32_t m = U32(s, 0x90) + (uint32_t)((Rnd() & 0x3F) << 5);
						ff8fx::copy<(0x12)>((void *)(r + 0x10), (const void *)m);
						if (--S32(s, 0x18) <= 0) return;
					}
				}
				k = U16(s, 8);
				p += 0x10;
				U16(s, 8) = (uint16_t)(k - 1);
			} while (k != 0);
		}
		k = U16(s, 0xA);
		U16(s, 0xA) = (uint16_t)(k - 1);
		if (k == 0) return;
		p += 4;
		do
		{
			if (Rnd() <= S32(s, 0x14))
			{
				CopyVertex(s, 0x24, verts, U16(p, -4));
				CopyVertex(s, 0x2C, verts, U16(p, -2));
				CopyVertex(s, 0x34, verts, U16(p, 0));
				CopyVertex(s, 0x3C, verts, U16(p, 2));
				if ((uint8_t)(U8(s, 0x42) | U8(s, 0x3A) | U8(s, 0x32) | U8(s, 0x2A)) == 0)
				{
					const uint32_t r = AllocDebris(U32(s, 0x20));
					if (!r) { S32(s, 0x18) = 0; return; }
					const int32_t x = ((int32_t)S16(s, 0x3C) + S16(s, 0x24) + S16(s, 0x2C) + S16(s, 0x34)) / 4;
					const int32_t y = ((int32_t)S16(s, 0x3E) + S16(s, 0x36) + S16(s, 0x2E) + S16(s, 0x26)) / 4;
					const int32_t z = ((int32_t)S16(s, 0x40) + S16(s, 0x38) + S16(s, 0x30) + S16(s, 0x28)) / 4;
					GteSetRotCtrl(U32(s, 0x8C) + (uint32_t)((Rnd() & 0x3F) << 5));
					GteLoadV0((void *)(s + 0x6C));
					GteMVMVA_RotV0();
					GteStoreIR123((void *)(r + 0x24));
					GteLoadV0((void *)(s + 0x74));
					GteMVMVA_RotV0();
					GteStoreIR123((void *)(r + 0x2A));
					GteLoadV0((void *)(s + 0x7C));
					GteMVMVA_RotV0();
					GteStoreIR123((void *)(r + 0x30));
					GteLoadV0((void *)(s + 0x84));
					GteMVMVA_RotV0();
					GteStoreIR123((void *)(r + 0x36));
					S16(r, 0) = (int16_t)x;
					S16(r, 2) = (int16_t)y;
					S16(r, 4) = (int16_t)z;
					S16(r, 6) = 2;
					S16(r, 8) = (int16_t)((Rnd() & 0x1FF) - 0x100);
					S16(r, 0xC) = (int16_t)((Rnd() & 0x1FF) - 0x100);
					S16(r, 0xA) = (int16_t)((Rnd() & 0xFF) - 0xC8);
					S16(r, 0xE) = (int16_t)((Rnd() & 7) + 0xC);
					U32(r, 0x3C) = U32(p, 4);
					U32(r, 0x40) = U32(p, 8);
					U16(r, 0x44) = U16(p, 0xC);
					U16(r, 0x46) = U16(p, 0xE);
					{
						const int32_t u0 = U8(r, 0x3C), u1 = U8(r, 0x40), u2 = U8(r, 0x44), u3 = U8(r, 0x46);
						const int32_t avg = (u0 + u1 + u2 + u3) / 4;
						U8(r, 0x3C) = Shrink(u0, (int16_t)avg, avg);
						U8(r, 0x40) = Shrink(u1, (int16_t)avg, avg);
						U8(r, 0x44) = Shrink(u2, (int16_t)avg, avg);
						U8(r, 0x46) = Shrink(u3, (int16_t)avg, avg);
					}
					{
						const int32_t v0 = U8(r, 0x3D), v1 = U8(r, 0x41), v2 = U8(r, 0x45), v3 = U8(r, 0x47);
						const int32_t avg = (v0 + v1 + v2 + v3) / 4;
						U8(r, 0x3D) = Shrink(v0, (int16_t)avg, avg);
						U8(r, 0x41) = Shrink(v1, (int16_t)avg, avg);
						U8(r, 0x45) = Shrink(v2, (int16_t)avg, avg);
						U8(r, 0x47) = Shrink(v3, (int16_t)avg, avg);
					}
					const uint32_t m = U32(s, 0x90) + (uint32_t)((Rnd() & 0x3F) << 5);
					ff8fx::copy<(0x12)>((void *)(r + 0x10), (const void *)m);
					if (--S32(s, 0x18) <= 0) return;
				}
			}
			k = U16(s, 0xA);
			p += 0x14;
			U16(s, 0xA) = (uint16_t)(k - 1);
		} while (k != 0);
	}

	static void TransformGroupWorld(uint32_t *objp, uint32_t *outp)
	{
		uint32_t obj = *objp;
		uint32_t out = *outp;
		const int32_t n = S16(obj, 0);
		obj += 2;
		for (int32_t v = n; v != 0; v--)
		{
			GteLoadV0u((void *)obj);
			GteMVMVA_RotV0Tr();
			S16(out, 6) = 0;
			out += 8;
			obj += 6;
			GteStoreIRVertex((void *)out);
		}
		*objp = obj;
		*outp = out;
	}

	static void BurstModel(uint32_t model, uint32_t s)
	{
		const uint32_t table = U32(model, 4);
		const uint32_t bones = U32(model, 0) + 0x10;
		const int32_t count = S32(table, 0);
		if (count <= 0) return;
		uint32_t offs = table + 4;
		for (int32_t i = 0; i < count; i++)
		{
			uint32_t obj = U32(model, 4) + U32(offs, 0);
			offs += 4;
			if (U32(s, 0x10) & (1u << (i & 31)))
			{
				uint32_t out = U32(s, 4);
				int32_t groups = S16(obj, 0);
				obj += 2;
				if (groups > 0)
				{
					do
					{
						const int32_t bone = S16(obj, 0);
						obj += 2;
						const uint32_t rec = bones + (uint32_t)(bone * 0x30);
						if (S16(rec, 0x22) != 0)
						{
							GteSetRotCtrl(rec + 0x10);
							GteSetTransCtrl(rec + 0x10);
							TransformGroupWorld(&obj, &out);
						}
						else
						{
							const int32_t n = S16(obj, 0);
							obj += (uint32_t)(n * 6 + 2);
							for (int32_t v = n; v != 0; v--)
							{
								S16(out, 6) = 1;
								out += 8;
							}
						}
					} while (--groups != 0);
				}
				obj = (obj + 3) & ~3u;
				U16(s, 8) = U16(obj, 0);
				obj += 2;
				U16(s, 0xA) = U16(obj, 0);
				obj += 0xA;
				U32(s, 0) = obj;
				SpawnDebris(s);
				if (S32(s, 0x18) <= 0) return;
			}
		}
	}

	static void Burst(uint32_t entity, uint32_t list, int32_t a, int32_t b, int32_t c)
	{
		const uint32_t B = (uint32_t)FieldAlloc(0xAC);
		S32(B, 0x2C) = a;
		S32(B, 0x30) = b;
		S32(B, 0x34) = c;
		U32(B, 0x38) = Buffer() + BUF_DIRECTOR;
		for (int i = 0; i < 7; i++)
		{
			U32(B, 0x6C + 8 * i) = U32(game_addr(DEBRIS_Shapes), 8 * i);
			U32(B, 0x70 + 8 * i) = U32(game_addr(DEBRIS_Shapes), 8 * i + 4);
		}
		uint32_t m = PacketCursor();
		U32(B, 0xA4) = m;
		for (int i = 0; i < 0x40; i++, m += 0x20)
		{
			S16(B, 0) = (int16_t)Rnd();
			S16(B, 2) = (int16_t)Rnd();
			S16(B, 4) = (int16_t)Rnd();
			BuildRotationMatrixFromAngles((const int16_t *)B, (Mat4x3 *)m);
			const int32_t k = (Rnd() & 0xFFF) + 0x2000;
			S32(B, 0x10) = k;
			S32(B, 0xC) = k;
			S32(B, 8) = k;
			Scale3DMatrix((Mat4x3 *)m, (const int32_t *)(B + 8));
		}
		U32(B, 0xA8) = m;
		for (int i = 0; i < 0x40; i++, m += 0x20)
		{
			S16(B, 0) = (int16_t)((Rnd() & 0x1FF) - 0x100);
			S16(B, 2) = (int16_t)((Rnd() & 0x1FF) - 0x100);
			S16(B, 4) = (int16_t)((Rnd() & 0x1FF) - 0x100);
			BuildRotationMatrixFromAngles((const int16_t *)B, (Mat4x3 *)m);
			const int32_t k = 0xF81 - (Rnd() & 0xFF);
			S32(B, 0x10) = k;
			S32(B, 0xC) = k;
			S32(B, 8) = k;
			Scale3DMatrix((Mat4x3 *)m, (const int32_t *)(B + 8));
		}
		U32(B, 0x1C) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		U32(B, 0x24) = U32(entity, 0x28);
		U32(B, 0x28) = U32(entity, 0x7C);
		const uint32_t sk = U32(U32(entity, 0x64), 0);
		for (uint32_t i = 0, n = U8(sk, 0); i < n; i++) S16(sk, 0x32 + 0x30 * i) = 0;
		for (uint32_t e = list; U8(e, 0) != 0xFF; e += 6) S16(sk, (int32_t)(U8(e, 0) & 0x7F) * 0x30 + 0x32) = 1;
		BurstModel(U32(entity, 0x64), B + 0x18);
		FieldFree(0xAC);
	}

	static void BonePosition(int32_t bone, int32_t scale, void *out)
	{
		const uint32_t sk = U32(U32(Caster(), 0x64), 0) + 0x10;
		const uint32_t rec = sk + (uint32_t)(bone * 0x30);
		int16_t v[4];
		v[0] = 0;
		v[1] = 0;
		if (S16(rec, 0) >= 0) v[2] = (int16_t)(mul32(S16(sk, (int32_t)S16(rec, 0) * 0x30 + 2), scale) >> 12);
		else v[2] = 0;
		v[3] = 0;
		GteSetRotMatrixW((const void *)(rec + 0x10));
		GteSetTransVectorW((const void *)(rec + 0x10));
		GteLoadV0(v);
		GteMVMVA_RotV0Tr();
		GteStoreIR123(out);
	}

	static int32_t BoneVertexCount(uint32_t entity, int32_t bone)
	{
		const uint32_t base = U32(U32(entity, 0x64), 4);
		int32_t total = 0;
		uint32_t offs = base;
		int32_t count = S32(offs, 0);
		offs += 4;
		if (count <= 0) return 0;
		do
		{
			uint32_t obj = base + U32(offs, 0);
			offs += 4;
			int32_t groups = S16(obj, 0);
			obj += 2;
			if (groups > 0)
			{
				do
				{
					const int32_t b = S16(obj, 0);
					const int32_t n = S16(obj, 2);
					obj += 4;
					if (b == bone) total += n;
					obj += (uint32_t)(n * 6);
				} while (--groups != 0);
			}
		} while (--count != 0);
		return total;
	}

	static void BoneVertex(uint32_t entity, int32_t bone, int32_t index, void *out)
	{
		const uint32_t model = U32(entity, 0x64);
		const uint32_t base = U32(model, 4);
		const uint32_t bones = U32(model, 0) + 0x10;
		uint32_t offs = base;
		const int32_t count = S32(offs, 0);
		offs += 4;
		if (count <= 0) return;
		for (int32_t i = 0; i < count; i++)
		{
			uint32_t obj = base + U32(offs, 0);
			offs += 4;
			const int32_t groups = S16(obj, 0);
			obj += 2;
			for (int32_t g = 0; g < groups; g++)
			{
				const int32_t b = S16(obj, 0);
				const int32_t n = S16(obj, 2);
				obj += 4;
				if (b == bone)
				{
					if (index < n)
					{
						const uint32_t m = bones + (uint32_t)(bone * 0x30) + 0x10;
						GteSetRotCtrl(m);
						GteSetTransCtrl(m);
						GteLoadV0u((void *)(obj + (uint32_t)(index * 6)));
						GteMVMVA_RotV0Tr();
						GteStoreIR123(out);
						return;
					}
					index -= n;
				}
				obj += (uint32_t)(n * 6);
			}
		}
	}

	// The game reads a stack address here (the arc control point it pushed): the value it has in the game
	static inline uint32_t ArcResidue() { return 0x1A4EB0; }

	static void ArcEdge(uint32_t p, uint32_t next, int16_t *prev, int16_t *out)
	{
		if (S16(p, 4) < 0) return;
		int32_t ox, oy;
		if (next == 0)
		{
			oy = 0;
			ox = 0;
		}
		else
		{
			if (S16(next, 4) < 0) return;
			const int32_t dx = ScreenPx(S16(next, 0)) - ScreenPx(S16(p, 0));
			int32_t ax = dx < 0 ? -dx : dx;
			if (ax >= 0x80) ax = 0x7F;
			const int32_t dy = ScreenPx(S16(p, 2)) - ScreenPx(S16(next, 2));
			int32_t ay = dy < 0 ? -dy : dy;
			if (ay >= 0x80) ay = 0x7F;
			const int32_t dist = U8(game_addr(ARC_Distance), ay * 128 + ax);
			int32_t nx, ny;
			if (prev)
			{
				if (dist == 0)
				{
					out[0] = prev[0];
					out[1] = prev[1];
					nx = out[0];
					ny = out[1];
				}
				else
				{
					const int32_t ex = shl32(dy, 12) / dist;
					const int32_t ey = shl32(dx, 12) / dist;
					int32_t sx = ex + prev[0];
					int32_t sy = ey + prev[1];
					out[0] = (int16_t)ex;
					out[1] = (int16_t)ey;
					if (sx == 0 && sy == 0)
					{
						sx = ex + ex;
						sy = ey + ey;
					}
					const int32_t k = 0x4000 - ((mul32(sx, sx) + mul32(sy, sy)) >> 13);
					ny = mul32(k, sy) >> 13;
					nx = mul32(k, sx) >> 13;
				}
			}
			else
			{
				if (dist == 0)
				{
					S16(p, 4) = -1;
					return;
				}
				nx = shl32(dy, 12) / dist;
				ny = shl32(dx, 12) / dist;
				out[0] = (int16_t)nx;
				out[1] = (int16_t)ny;
			}
			const int32_t w = shl32(S16(next, 6), 5) / ((int32_t)S16(p, 4) + 0x400);
			oy = ScreenEighths(mul32(w, ny) >> 12);
			ox = ScreenEighths(mul32(w, nx) >> 12);
		}
		const int16_t x = S16(p, 0), y = S16(p, 2);
		S16(p, 0) = (int16_t)(x - ox);
		S16(p, 8) = (int16_t)(x + ox);
		S16(p, 0xA) = (int16_t)(y + oy);
		S16(p, 2) = (int16_t)(y - oy);
	}

	static void ArcDraw(uint32_t pts, uint32_t residue)
	{
		GteSetRotMatrixCtrl(&Camera());
		GteSetTransVectorCtrl(&Camera());
		uint32_t q = pts;
		for (int i = 0; i < 8; i++, q += 0x10)
		{
			GteLoadV0((void *)q);
			GteRTPS();
			GteReadDataReg(0xE, (void *)q);
			int32_t sz;
			GteReadDataRegTo(&sz, 0x13);
			S16(q, 4) = (int16_t)(sz >> 2);
		}
		int16_t dir[2] = { (int16_t)(residue & 0xFFFF), (int16_t)(residue >> 16) };
		ArcEdge(pts, pts + 0x10, nullptr, dir);
		for (int i = 1; i < 7; i++) ArcEdge(pts + 0x10 * i, pts + 0x10 * (i + 1), dir, dir);
		ArcEdge(pts + 0x70, 0, dir, nullptr);
		q = pts;
		for (int i = 0; i < 7; i++, q += 0x10)
		{
			if (S16(q, 4) <= 0 || S16(q, 0x14) <= 0) continue;
			const uint32_t pk = FrameCursor();
			FrameCursor() = pk + 0x34;
			U32(pk, 8) = U32(q, 0);
			U32(pk, 0x14) = U32(q, 8);
			U32(pk, 0xC) = 0x3CD4C008;
			U32(pk, 0x18) = 0xBAC038;
			U16(pk, 0x24) = 0xC008;
			U16(pk, 0x30) = 0xC038;
			U32(pk, 0x20) = U32(q, 0x10);
			U32(pk, 0x2C) = U32(q, 0x18);
			const int32_t c0 = S16(q, 0xE);
			const uint32_t col0 = (uint32_t)((((c0 << 8) | c0) << 8) | c0);
			U32(pk, 0x10) = col0;
			U32(pk, 4) = col0;
			const int32_t c1 = S16(q, 0x1E);
			const uint32_t col1 = (uint32_t)((((c1 << 8) | c1) << 8) | c1);
			U32(pk, 0) = 0xC000000;
			U8(pk, 7) = 0x3E;
			U32(pk, 0x28) = col1;
			U32(pk, 0x1C) = col1;
			InsertPrimAutoDepth(OTBase() + (uint32_t)(((int32_t)S16(q, 0x14) + S16(q, 4)) >> 3) * 4 + 0x44, (void *)pk);
		}
	}

	static uint32_t __cdecl ArcTask(TaskNode *t)
	{
		const uint32_t n = (uint32_t)t;
		const uint32_t bones = U32(U32(n, 0x18), 0) + 0x10;
		int16_t p1[4], n1[4], p2[4], n2[4];
		TriangleFrame(bones, U32(n, 0x10), p1, n1);
		TriangleFrame(bones, U32(n, 0x14), p2, n2);
		int32_t d[3];
		d[0] = (int32_t)p2[0] - p1[0];
		d[1] = (int32_t)p2[1] - p1[1];
		d[2] = (int32_t)p2[2] - p1[2];
		GteLoadIR123_32b(d);
		GteSQR();
		GteReadMAC123(d);
		int32_t sum = d[1] + d[2] + d[0];
		GteSetLZCS(sum);
		int32_t lz;
		GteReadLZCR(&lz);
		lz &= ~1;
		const int32_t shift = (0x1F - lz) >> 1;
		const int32_t norm = lz - 0x18;
		if (norm > 0) sum = shl32(sum, norm);
		else sum >>= (0x18 - lz);
		const int32_t len = (int32_t)((uint32_t)AT<uint16_t>(game_addr(ARC_Lengths), sum * 2) << shift);
		const int32_t amp = len >> 12;
		GteSetIR0(len >> U8(n, 0xE));
		GteLoadIR123(n1);
		GteGPF();
		GteStoreIR123(n1);
		GteLoadIR123(n2);
		GteGPF();
		GteStoreIR123(n2);
		n1[0] = (int16_t)(n1[0] + p1[0]);
		n1[1] = (int16_t)(n1[1] + p1[1]);
		n1[2] = (int16_t)(n1[2] + p1[2]);
		n2[0] = (int16_t)(n2[0] + p2[0]);
		n2[1] = (int16_t)(n2[1] + p2[1]);
		n2[2] = (int16_t)(n2[2] + p2[2]);
		const uint32_t b = (uint32_t)FieldAlloc(0x80);
		U32(b, 0) = *(const uint32_t *)p1;
		U32(b, 4) = *(const uint32_t *)(p1 + 2);
		S16(b, 6) = S16(n, 0x1C);
		S16(b, 0xE) = S16(n, 0x1E);
		U32(b, 0x70) = *(const uint32_t *)p2;
		U32(b, 0x74) = *(const uint32_t *)(p2 + 2);
		S16(b, 0x76) = S16(n, 0x46);
		S16(b, 0x7E) = S16(n, 0x48);
		uint32_t w = b + 0x10, src = n + 0x22;
		for (int32_t i = 1; i < 7; i++)
		{
			const int32_t t = shl32(i, 12) / 7;
			const int32_t u = 0x1000 - t;
			int16_t a[4], c[4];
			GteSetIR0(u);
			GteLoadIR123(p1);
			GteGPF();
			GteSetIR0(t);
			GteLoadIR123(n1);
			GteGPL();
			GteStoreIR123(a);
			GteSetIR0(t);
			GteLoadIR123(p2);
			GteGPF();
			GteSetIR0(u);
			GteLoadIR123(n2);
			GteGPL();
			GteStoreIR123(c);
			GteSetIR0(u);
			GteLoadIR123(a);
			GteGPF();
			GteSetIR0(t);
			GteLoadIR123(c);
			GteGPL();
			GteStoreIR123((void *)w);
			w += 0x10;
			S16(w, -0xA) = S16(src, 0);
			S16(w, -2) = S16(src, 2);
			src += 6;
		}
		uint32_t q = b + 0x10;
		for (uint32_t k = 0x4306; k < 0x4312; k += 2, q += 0x10)
		{
			const int32_t e = mul32(S16(Buffer(), (int32_t)k), amp) >> 12;
			S16(q, 0) = (int16_t)(S16(q, 0) + (mul32(SharedRand() - 0x4000, e) >> 18));
			S16(q, 2) = (int16_t)(S16(q, 2) + (mul32(SharedRand() - 0x4000, e) >> 18));
			S16(q, 4) = (int16_t)(S16(q, 4) + (mul32(SharedRand() - 0x4000, e) >> 18));
		}
		ArcDraw(b, ArcResidue());
		FieldFree(0x80);
		if (S16(n, 0xC) >= 0xA)
		{
			int32_t lit = 0;
			uint32_t bright = n + 0x1E;
			for (uint32_t k = BUF_PROFILE; k < BUF_PROFILE_END; k += 2, bright += 6)
			{
				const int32_t dec = (0x1400 - S16(Buffer(), (int32_t)k)) >> U8(n, 0xF);
				S16(bright, 0) = (int16_t)(S16(bright, 0) - dec);
				if (S16(bright, 0) < 0) S16(bright, 0) = 0;
				else lit++;
			}
			if (lit <= 1) return TASK_END;
		}
		S16(n, 0xC)++;
		return 0;
	}

	static int32_t SpawnArc(uint32_t model, uint32_t table, int32_t i, int32_t j, uint8_t amp_shift, uint8_t fade_shift)
	{
		const uint32_t a = table + (uint32_t)(i * 0x18), b = table + (uint32_t)(j * 0x18);
		int32_t dz = ((int32_t)S16(a, 0x10) - S16(b, 0x10) - S16(b, 8) + S16(a, 8) - S16(b, 0) + S16(a, 0));
		if (dz < 0) dz = -dz;
		int32_t dx = ((int32_t)S16(a, 0x12) + S16(a, 0xA) + S16(a, 2)) - ((int32_t)S16(b, 0x12) + S16(b, 0xA) + S16(b, 2));
		if (dx < 0) dx = -dx;
		int32_t dy = ((int32_t)S16(a, 0x14) + S16(a, 0xC) + S16(a, 4)) - ((int32_t)S16(b, 0x14) + S16(b, 0xC) + S16(b, 4));
		if (dy < 0) dy = -dy;
		if (dz < 0x100 && dx < 0x100 && dy < 0x100) return 0;
		const uint32_t n = (uint32_t)AddTaskToQueue(QArc(), game_addr(ORIG_ArcTask));
		if (n)
		{
			U8(n, 0xE) = amp_shift;
			U32(n, 0x10) = a;
			U32(n, 0x14) = b;
			S16(n, 0xC) = 0;
			U8(n, 0xF) = fade_shift;
			U32(n, 0x18) = model;
			uint32_t p = n + 0x1C;
			for (int32_t k = 0; k < 8; k++, p += 6)
			{
				const int32_t s = ComputeSin(shl32(k, 11) / 7);
				S16(p, 2) = 0x80;
				S16(p, 0) = (int16_t)((s * 0x104) >> 12);
				S16(p, 4) = 0;
			}
		}
		return 1;
	}

	static void DrawMoving(uint32_t n, uint32_t seq)
	{
		const uint32_t h = (uint32_t)AllocHeader(0xB4);
		const int16_t size = S16(n, 0x1A);
		TransformCameraByShadowRotation((const void *)(n + 0xC), size, -((int32_t)size >> 2));
		U32(h, 0) = seq;
		S16(h, 4) = (int16_t)(S16(n, 0x12) >> 1);
		S16(h, 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData((void *)h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl SmokeTask(TaskNode *t)
	{
		const uint32_t n = (uint32_t)t;
		DrawMoving(n, game_addr(SEQ_Smoke));
		const int16_t vy = S16(n, 0x16);
		S16(n, 0xE) = (int16_t)(S16(n, 0xE) + vy);
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + S16(n, 0x14));
		S16(n, 0x10) = (int16_t)(S16(n, 0x10) + S16(n, 0x18));
		S16(n, 0x12)++;
		S16(n, 0x16) = (int16_t)(vy + 0x10);
		return S16(n, 0x12) < 0x10 ? 0 : TASK_END;
	}
	static uint32_t __cdecl FallTask(TaskNode *t)
	{
		const uint32_t n = (uint32_t)t;
		DrawMoving(n, game_addr(SEQ_Fall));
		const int16_t vy = S16(n, 0x16);
		S16(n, 0xE) = (int16_t)(S16(n, 0xE) + vy);
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + S16(n, 0x14));
		S16(n, 0x10) = (int16_t)(S16(n, 0x10) + S16(n, 0x18));
		S16(n, 0x12)++;
		S16(n, 0x16) = (int16_t)(vy - 5);
		return S16(n, 0x12) < 0x1C ? 0 : TASK_END;
	}

	static void DrawSpark(uint32_t n)
	{
		const uint32_t h = (uint32_t)AllocHeader(0xB4);
		const int16_t size = S16(n, 0x16);
		TransformCameraByShadowRotation((const void *)(n + 0x10), size, -((int32_t)size >> 2));
		U32(h, 0) = game_addr(SEQ_Spark);
		S16(h, 4) = S16(n, 0xC);
		S16(h, 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData((void *)h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl SparkTask(TaskNode *t)
	{
		const uint32_t n = (uint32_t)t;
		DrawSpark(n);
		if (S16(n, 0xC) == 5)
		{
			const uint32_t spec = U8(n, 0xF);
			const int16_t first = (int16_t)(spec >> 4);
			for (uint32_t k = spec & 0xF; k != 0; k--)
			{
				const uint32_t c = (uint32_t)AddTaskToQueue(QFall(), game_addr(ORIG_FallTask));
				if (!c) continue;
				U32(c, 0xC) = U32(n, 0x10);
				U32(c, 0x10) = U32(n, 0x14);
				const int32_t r = SharedRand() & 0xFF;
				S16(c, 0x12) = first;
				S16(c, 0xE) = (int16_t)(S16(c, 0xE) + (int16_t)(-0x80 - r));
				S16(c, 0x14) = (int16_t)((SharedRand() & 0x7F) - 0x40);
				const int32_t r2 = SharedRand() & 0x7F;
				S16(c, 0x16) = -0x32;
				S16(c, 0x18) = (int16_t)(r2 - 0x40);
				S16(c, 0x1A) = (int16_t)((SharedRand() & 0x7FF) + 0x1000);
			}
		}
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + (uint16_t)U8(n, 0xE));
		return S16(n, 0xC) < 0xE ? 0 : TASK_END;
	}

	static void SpawnSparks(int32_t bone, int32_t size, int32_t step, int32_t count, uint8_t falling)
	{
		const int32_t n = BoneVertexCount(Caster(), bone);
		for (int32_t k = count; k != 0; k--)
		{
			const uint32_t s = (uint32_t)AddTaskToQueue(QSpark(), game_addr(ORIG_SparkTask));
			if (!s) return;
			const int32_t idx = mul32(SharedRand(), n) >> 15;
			BoneVertex(Caster(), bone, idx, (void *)(s + 0x10));
			S16(s, 0x16) = (int16_t)shl32(size, 6);
			S16(s, 0xC) = 0;
			U8(s, 0xE) = (uint8_t)step;
			U8(s, 0xF) = falling;
		}
	}

	static inline int32_t PrimScaled() { return 0; }

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
		const uint32_t flags = r->flags;
		if (flags & 0x1000)
		{
			GteSetRotMatrixCtrl(&Camera());
			GteLoadV0(arg->pos);
			GteMVMVA_RotV0();
			GteReadMAC123(m.t);
			GteSetRotMatrixCtrl(&EffectCamera());
			GteLoadV0(off);
			GteMVMVA_RotV0();
			int32_t mac[3];
			GteReadMAC123(mac);
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)mac[0]);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)mac[1]);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)mac[2]);
			MatrixMultiply(&EffectCamera(), &m);
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
			if (flags & 0x100)
			{
				Mat4x3 d = {};
				d.m[0][0] = r->scale[0];
				d.m[1][1] = r->scale[1];
				d.m[2][2] = r->scale[2];
				MatrixMultiply3(&m, &d);
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
		PacketCursor() = RenderPrimModel2(h, OTBase() + 0x44, 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl PrimTask(TaskNode *t)
	{
		const uint32_t n = (uint32_t)t;
		PrimArg a;
		ff8fx::copy<(8)>(a.pos, (const void *)(n + 0xE));
		const int32_t scale = S16(n, 0x14);
		a.scale[2] = scale;
		a.scale[1] = scale;
		a.scale[0] = scale;
		a.scaled = PrimScaled();
		a.depth = -0x80;
		a.morph = Buffer() + BUF_MORPH;
		prim::play((prim::Layout *)(n + 0x18), PrimCallback, (int)&a, 0);
		S16(n, 0xC)++;
		return S16(n, 0xC) < 0x10 ? 0 : TASK_END;
	}

	static uint32_t __cdecl EventTask(TaskNode *t)
	{
		const uint32_t n = (uint32_t)t;
		const uint32_t e = U32(n, 0x10);
		if (S32(n, 0xC) < (int32_t)U8(e, 0))
		{
			for (uint32_t left = U8(e, 1); left != 0;)
			{
				const int32_t i = mul32(SharedRand(), S32(e, 0x78)) >> 15;
				int32_t j = mul32(SharedRand(), S32(e, 0x78)) >> 15;
				while (i == j) j = mul32(SharedRand(), S32(e, 0x78)) >> 15;
				if (SpawnArc(U32(Caster(), 0x64), U32(e, 0x7C), i, j, 0xC, 7)) left--;
			}
		}
		const int32_t tick = S32(n, 0xC);
		const int32_t end = U8(e, 2);
		if (tick >= (end >> 1) && tick < end)
			for (uint32_t l = e + 0x16; U8(l, 0) != 0xFF; l += 6)
				if ((int32_t)U8(l, 4) > (SharedRand() & 0xFF)) SpawnSparks(U8(l, 0) & 0x7F, U8(l, 5), 2, 1, 0);
		if ((uint32_t)(S32(n, 0xC) - end + 4) < 4)
		{
			const uint32_t late = 3 - (uint32_t)(S32(n, 0xC) - end + 4);
			for (uint32_t l = e + 0x16; U8(l, 0) != 0xFF; l += 6)
				if (U8(l, 1) != 0 && (uint32_t)U8(l, 1) < late) SpawnSparks(U8(l, 0) & 0x7F, U8(l, 3), 1, 1, U8(e, 0x15));
			if (U8(e, 0x14) != 0xFF)
			{
				int32_t count = (SharedRand() & 3) + 6;
				int16_t pos[4];
				BonePosition(U8(e, 0x14), 0, pos);
				pos[3] = 0;
				do
				{
					const uint32_t s = (uint32_t)AddTaskToQueue(QSmoke(), game_addr(ORIG_SmokeTask));
					if (s)
					{
						U32(s, 0xC) = *(const uint32_t *)pos;
						U32(s, 0x10) = *(const uint32_t *)(pos + 2);
						S16(s, 0x14) = (int16_t)((SharedRand() & 0x1FF) - 0x100);
						S16(s, 0x18) = (int16_t)((SharedRand() & 0x1FF) - 0x100);
						S16(s, 0x16) = (int16_t)((SharedRand() & 0x1FF) - 0x12C);
						S16(s, 0x1A) = (int16_t)((SharedRand() & 0x7FF) + 0x800);
					}
				} while (--count != 0);
			}
		}
		if (S32(n, 0xC) == (int32_t)U8(e, 2))
		{
			for (uint32_t l = e + 0x16; U8(l, 0) != 0xFF; l += 6)
			{
				const uint8_t id = U8(l, 0);
				if (id < 0x80)
				{
					U8(U32(Buffer(), BUF_DIRECTOR + 0x48), 0) = id;
					U32(Buffer(), BUF_DIRECTOR + 0x48)++;
				}
			}
			if (S16(e, 0xE) != 0) Burst(Caster(), e + 0x16, S16(e, 0xE), S16(e, 0x10), S16(e, 0x12));
			if (U8(e, 0x15) == 0) return TASK_END;
		}
		if (S32(n, 0xC) == (int32_t)U8(e, 8))
		{
			const uint32_t p = (uint32_t)AddTaskToQueue(QPrim(), game_addr(ORIG_PrimTask));
			if (p)
			{
				BonePosition(U8(e, 9), S16(e, 0xA), (void *)(p + 0xE));
				S16(p, 0xC) = 0;
				S16(p, 0x14) = S16(e, 0xC);
				DecodeModelPrimLayout(game_addr(PRIM_Model), (void *)(p + 0x18), 0x124);
			}
		}
		if (U8(e, 3) != 0xFF && S32(n, 0xC) == (int32_t)U8(e, 3)) ScreenFadeTask(U8(e, 4), U8(e, 5), U8(e, 6), 0x80);
		U32(n, 0xC)++;
		return 0;
	}

	static void SpawnEvent(uint32_t table)
	{
		const uint32_t n = (uint32_t)AddTaskToQueue(QEvent(), game_addr(ORIG_EventTask));
		if (!n) return;
		U32(n, 0xC) = 0;
		U32(n, 0x10) = table;
	}

	static inline bool OffX(int16_t v) { return v < 0 || v > 0xA00; }
	static inline bool OffY(int16_t v) { return v < 0 || v > 0x6C0; }

	static void DrawDebris(uint32_t node)
	{
		int16_t *v = (int16_t *)FieldAlloc(8);
		uint32_t r = node + 0x54;
		for (int k = 0x80; k != 0; k--, r += 0x48)
		{
			if (S16(r, 6) == 0) continue;
			const uint32_t pk = PacketCursor();
			v[0] = (int16_t)(S16(r, 0x24) + S16(r, 0));
			v[1] = (int16_t)(S16(r, 0x26) + S16(r, 2));
			v[2] = (int16_t)(S16(r, 0x28) + S16(r, 4));
			GteLoadV0(v);
			v[0] = (int16_t)(S16(r, 0x2A) + S16(r, 0));
			v[1] = (int16_t)(S16(r, 0x2C) + S16(r, 2));
			v[2] = (int16_t)(S16(r, 0x2E) + S16(r, 4));
			GteLoadV1(v);
			v[0] = (int16_t)(S16(r, 0x30) + S16(r, 0));
			v[1] = (int16_t)(S16(r, 0x32) + S16(r, 2));
			v[2] = (int16_t)(S16(r, 0x34) + S16(r, 4));
			GteLoadV2(v);
			GteRTPT();
			U32(pk, 0xC) = U32(r, 0x3C);
			U32(pk, 0x14) = U32(r, 0x40);
			U16(pk, 0x1C) = U16(r, 0x44);
			GteReadSXY012((void *)(pk + 8), (void *)(pk + 0x10), (void *)(pk + 0x18));
			uint32_t f = 0;
			if (OffX(S16(pk, 8))) f = 1;
			if (OffX(S16(pk, 0x10))) f |= 2;
			if (OffX(S16(pk, 0x18))) f |= 4;
			if (OffY(S16(pk, 0xA))) f |= 0x10;
			if (OffY(S16(pk, 0x12))) f |= 0x20;
			if (OffY(S16(pk, 0x1A))) f |= 0x40;
			if (S16(r, 6) == 2)
			{
				v[0] = (int16_t)(S16(r, 0x36) + S16(r, 0));
				v[1] = (int16_t)(S16(r, 0x38) + S16(r, 2));
				v[2] = (int16_t)(S16(r, 0x3A) + S16(r, 4));
				GteLoadV0(v);
				GteRTPS();
				U16(pk, 0x24) = U16(r, 0x46);
				GteReadSXY2((void *)(pk + 0x20));
				GteAVSZ4();
				if (OffX(S16(pk, 0x20))) f |= 8;
				if (OffY(S16(pk, 0x22))) f |= 0x80;
				if ((f & 0xF) == 0xF || (f & 0xF0) == 0xF0) continue;
				GteReadOTZ32(v);
				if (v[0] <= 0) continue;
				U32(pk, 0) = 0x9000000;
				U32(pk, 4) = 0x2CF0F0F0;
				PacketCursor() = PacketCursor() + 0x28;
			}
			else
			{
				GteAVSZ3();
				if ((f & 7) == 7 || (f & 0x70) == 0x70) continue;
				U32(pk, 0) = 0x7000000;
				U32(pk, 4) = 0x24F0F0F0;
				GteReadOTZ32(v);
				if (v[0] <= 0) continue;
				PacketCursor() = PacketCursor() + 0x20;
			}
			InsertPrimAutoDepth(OTBase() + (uint32_t)(v[0] >> 2) * 4, (void *)pk);
		}
		FieldFree(8);
	}

	static void DrawWhite(int32_t c)
	{
		uint32_t cur;
		if ((uint32_t)(c - 0x64) < 0x10) cur = PacketCursor() = Tile(0xFF, 0xFF, 0xFF, (int32_t)(((uint32_t)(c - 0x64) & 0xFFFFF) << 8), PacketCursor());
		else cur = PacketCursor();
		const uint32_t e = (uint32_t)(c - 0x7C);
		if (e < 0x3E8 && e < 0x14) cur = PacketCursor() = Tile(0xFF, 0xFF, 0xFF, (int32_t)(0x1000 - (uint32_t)(((uint64_t)(e << 12) * 0xCCCCCCCDu) >> 36)), cur);
		if (c >= 0x74 && c < 0x7C) PacketCursor() = Tile(0xFF, 0xFF, 0xFF, 0x1000, cur);
	}

	static void MoveDebris(uint32_t node)
	{
		uint32_t r = node + 0x54;
		for (int k = 0x80; k != 0; k--, r += 0x48)
		{
			if (S16(r, 6) == 0) continue;
			S16(r, 0xE)--;
			if (S16(r, 0xE) != 0)
			{
				GteSetRotCtrl(r + 0x10);
				GteLoadV0((void *)(r + 0x24));
				GteMVMVA_RotV0();
				GteStoreIR123((void *)(r + 0x24));
				GteLoadV0u((void *)(r + 0x2A));
				GteMVMVA_RotV0();
				if (U32(r, 0x24) != 0 || S16(r, 0x28) != 0)
				{
					GteStoreIR123((void *)(r + 0x2A));
					GteLoadV0((void *)(r + 0x30));
					GteMVMVA_RotV0();
					GteStoreIR123((void *)(r + 0x30));
					GteLoadV0u((void *)(r + 0x36));
					GteMVMVA_RotV0();
					GteStoreIR123((void *)(r + 0x36));
					const int16_t vy = S16(r, 0xA);
					S16(r, 2) = (int16_t)(S16(r, 2) + vy);
					S16(r, 0) = (int16_t)(S16(r, 0) + S16(r, 8));
					S16(r, 4) = (int16_t)(S16(r, 4) + S16(r, 0xC));
					const int16_t nv = (int16_t)(vy + 0x1E);
					S16(r, 0xA) = nv;
					if (S16(r, 2) >= 0) S16(r, 0xA) = (int16_t)-nv;
					continue;
				}
			}
			S16(r, 6) = 0;
			U32(node, 0x50) = r;
		}
	}

	static uint32_t __cdecl DirectorTask(TaskNode *t)
	{
		const uint32_t n = (uint32_t)t;
		if (S16(n, 0xC) >= 0x90) return TASK_END;
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		DrawDebris(n);
		DrawWhite(S16(n, 0xC));
		if (S16(n, 0xC) <= 0x6C) DrawCaster(Caster(), n + 0xE);
		U8(Caster(), 0) |= 4;
		const int16_t c = S16(n, 0xC);
		if (c == 0) BdPlaySE(SOUND_Start(), 0, 0x80);
		else if (c == 0x8C) ReleaseVoiceSlot(U32(n, 0x4C));
		MoveDebris(n);
		switch (c)
		{
		case 0: SpawnEvent(game_addr(EVENTS[0])); break;
		case 0xC: SpawnEvent(game_addr(EVENTS[1])); break;
		case 0x26: SpawnEvent(game_addr(EVENTS[2])); break;
		case 0x32: SpawnEvent(game_addr(EVENTS[3])); break;
		case 0x41: SpawnEvent(game_addr(EVENTS[4])); break;
		}
		S16(n, 0xC)++;
		return 0;
	}

	static void Start()
	{
		Music_47E3C0(0xF0);
		InitTaskQueuePool(QSmoke(), (void *)Buffer(), 0x1C, 0x40);
		InitTaskQueuePool(QFall(), (void *)(Buffer() + 0x700), 0x1C, 0x2A);
		InitTaskQueuePool(QSpark(), (void *)(Buffer() + 0xB98), 0x18, 0x20);
		InitTaskQueuePool(QPrim(), (void *)(Buffer() + 0xE98), 0x13C, 3);
		InitTaskQueuePool(QDirector(), (void *)(Buffer() + BUF_DIRECTOR), 0x2454, 1);
		InitTaskQueuePool(QEvent(), (void *)(Buffer() + 0x1BCC), 0x14, 5);
		InitTaskQueuePool(QArc(), (void *)(Buffer() + 0x124C), 0x4C, 0x20);
		const uint32_t d = (uint32_t)AddTaskToQueue(QDirector(), game_addr(ORIG_Director));
		Memset32((void *)(d + 0xC), 0, 0x912);
		U32(d, 0x4C) = ClaimVoiceSlot(SOUND_Voice(), 1, 0x80);
		U32(d, 0x50) = d + 0x54;
		U32(d, 0x48) = d + 0xE;
		ff8fx::fill<(0x37)>((void *)(d + 0xE), 0xFF);
		for (int32_t i = 0; i < 8; i++) S16(Buffer(), (int32_t)(BUF_PROFILE + 2 * i)) = (int16_t)ComputeSin(shl32(i, 11) / 7);
	}

	static uint32_t __cdecl RootTask(TaskNode *t)
	{
		const uint32_t n = (uint32_t)t;
		if (U32(n, 0x10))
		{
			PacketCursor() = Buffer() + BUF_ARENA0;
			U32(n, 0x10) = 0;
		}
		else
		{
			PacketCursor() = Buffer() + BUF_ARENA1;
			U32(n, 0x10) = 1;
		}
		if (S16(n, 0xC) == 1 && U8(n, 0xF) == 0)
		{
			U8(n, 0xF) = 1;
			Start();
		}
		if (U8(n, 0xF))
		{
			const uint32_t e = Caster();
			ComputeBonesWorldMatrices((void *)(e + 0x60), (void *)(e + 0x40));
			ExecuteTaskQueue(QEvent());
			const int arcs = ExecuteTaskQueue(QArc());
			BuildBoneMatricesFromPose((void *)(Caster() + 0x60));
			ExecuteTaskQueue(QPrim());
			ExecuteTaskQueue(QSpark());
			ExecuteTaskQueue(QFall());
			ExecuteTaskQueue(QSmoke());
			const int director = ExecuteTaskQueue(QDirector());
			if (director == 0 && arcs == 0) return TASK_END;
		}
		S16(n, 0xC)++;
		return 0;
	}
}

	void register_mag276_xatm092_death()
	{
		register_port(xatm276::ORIG_RootTask, (void *)xatm276::RootTask, "276 RootTask", 276);
		register_port(xatm276::ORIG_Director, (void *)xatm276::DirectorTask, "276 Director", 276);
		register_port(xatm276::ORIG_EventTask, (void *)xatm276::EventTask, "276 EventTask", 276);
		register_port(xatm276::ORIG_SmokeTask, (void *)xatm276::SmokeTask, "276 SmokeTask", 276);
		register_port(xatm276::ORIG_PrimTask, (void *)xatm276::PrimTask, "276 PrimTask", 276);
		register_port(xatm276::ORIG_SparkTask, (void *)xatm276::SparkTask, "276 SparkTask", 276);
		register_port(xatm276::ORIG_FallTask, (void *)xatm276::FallTask, "276 FallTask", 276);
		register_port(xatm276::ORIG_ArcTask, (void *)xatm276::ArcTask, "276 ArcTask", 276);
	}
}

