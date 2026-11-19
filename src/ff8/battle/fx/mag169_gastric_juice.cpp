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

// Effect 169: Gastric Juice

#include "mag_common.h"

namespace ff8fx
{
namespace gastric169
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

	inline int32_t &TargetSlot() { return var<int32_t>(game_addr(game::g_Mag169_TargetSlot)); }
	inline TaskQueue *EffectQueue() { return (TaskQueue *)game_addr(game::g_Mag169_EffectQueue); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::g_Mag169_CastContext)); }
	inline int32_t &AttackerSlot() { return var<int32_t>(game_addr(game::g_Mag169_AttackerSlot)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::g_Mag169_TexBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::g_Mag169_PacketCursor)); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	static constexpr game::Id DROPS = game::g_Mag169_Drops, DROPS_END = game::g_Mag169_EffectQueue;
	static constexpr game::Id STREAMS = game::g_Mag169_Streams;
	static constexpr game::Id MESH_VERTS = game::g_Mag169_MeshVerts;
	static constexpr game::Id SPLASH_MATRIX = game::g_Mag169_SplashMatrix;
	static constexpr game::Id MDATA_LO = game::g_Mag169_TargetSlot, MDATA_HI = game::unk_23B3C10;

	static constexpr game::Id ORIG_Root = game::MAG_169_GastricJuice_RootTask;
	static constexpr game::Id ORIG_Director = game::MAG_169_GastricJuice_DirectorTask;
	static constexpr game::Id ORIG_Tint = game::MAG_169_GastricJuice_TintedTargetTask;
	static constexpr game::Id ORIG_Stream = game::MAG_169_GastricJuice_StreamTask;
	static constexpr game::Id ORIG_Blob = game::MAG_169_GastricJuice_BlobTask;
	static constexpr game::Id ORIG_Mesh = game::MAG_169_GastricJuice_LandingMeshTask;
	static constexpr game::Id ORIG_Drips = game::MAG_169_GastricJuice_DripEmitterTask;
	static constexpr game::Id ORIG_Puddle = game::MAG_169_GastricJuice_PuddleTask;
	static constexpr game::Id ORIG_Splash = game::MAG_169_GastricJuice_SplashTask;
	static constexpr game::Id SEQ_Blob = game::unk_D6A544, SEQ_Splash = game::unk_D6A620, SEQ_Drip = game::unk_D6A718;
	static constexpr game::Id MODEL_Mesh = game::unk_D6A8C4, MODEL_Puddle = game::unk_D6B534;
	static constexpr game::Id MESH_A = game::unk_D6A8CC, MESH_B = game::unk_D6B2EC;
	static const int32_t *DRIP_COUNTS() { return (const int32_t *)game_addr(game::unk_D6BC24); }
	static const void *SOUND_Juice() { return (const void *)game_addr(game::unk_D6BC1C); }

	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	inline void GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t mode, int16_t *out) { fn<void (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, mode, out); }
	inline uint32_t DrawShadow(uint32_t entity, uint32_t ot, uint32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, mode, cursor); }
	inline uint32_t RenderGeometry(uint32_t model, uint32_t hdr, uint32_t ot, uint32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::RenderGeometry))(model, hdr, ot, mode, cursor); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline void BezierInit(int32_t n, const void *points, void *work) { fn<void (__cdecl *)(int32_t, const void *, void *)>(game_addr(game::Effect_SplineSetup))(n, points, work); }
	inline void BezierPoint(int32_t n, void *work, void *out, int32_t t) { fn<void (__cdecl *)(int32_t, void *, void *, int32_t)>(game_addr(game::Effect_SplineEval))(n, work, out, t); }

	inline void GteWriteData(uint32_t value, uint32_t reg) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::GTE_WriteDataReg))(value, reg); }
	inline void GteReadData(void *dst, uint32_t reg) { fn<void (__cdecl *)(void *, uint32_t)>(game_addr(game::GTE_ReadDataRegTo))(dst, reg); }
	inline void GteLoadV012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteLoadV0u(uint32_t v) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadV0))(v); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadFLAG(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteReadSXY2u(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadSXY2))(dst); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZ32(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteSetFarColor(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteSetFarColorB(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColorRGB))(r, g, b); }
	inline void GteLoadRGBC(uint32_t c) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteDPCT() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCT))(); }
	inline void GteStoreRGB2(uint32_t c) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreRGB2))(c); }
	inline void GteLoadRGB012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_LoadRGB012))(a, b, c); }
	inline void GteStoreRGB012(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_StoreRGB012))(a, b, c); }
	inline void GteMVMVA_RotV0Tr2() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0_Tr_2))(); }
	inline void GteSetIR0FromWord(uint32_t p) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_SetIR0FromS16))(p); }
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteSetBKFromMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
	inline void GteSetRotDiagonal16(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotDiag))(s); }
	inline void InsertPrim(uint32_t bucket, uint32_t pkt) { InsertPrimAutoDepth(bucket, (void *)pkt); }

#pragma pack(push, 1)
	struct RootNode { TaskNode hdr; uint16_t counter; uint16_t pad; };
	struct Node
	{
		TaskNode hdr;
		int16_t counter;
		int16_t idx;
		int16_t pos[4];
		int16_t a18;
		int16_t a1a;
		int16_t a1c;
		int16_t a1e;
		uint32_t pad20;
	};
	struct Stream
	{
		int16_t src[4];
		int16_t dst[4];
		int16_t cp[4][4];
		int16_t curve[28][4];
	};
	struct Drop
	{
		uint32_t flags;
		int16_t frame;
		int16_t scale;
		int16_t pos[3];
		int16_t pad0;
		int16_t vel[3];
		int16_t pad1;
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10, "root node is 0x10 bytes");
	static_assert(sizeof(Node) == 0x24, "Gastric Juice nodes are 0x24 bytes");
	static_assert(sizeof(Stream) == 0x110 && sizeof(Drop) == 0x18, "stream / droplet entries");

	inline Stream *StreamOf(int32_t i) { return (Stream *)(game_addr(STREAMS) + (uint32_t)(i * 0x110)); }
	inline Drop *Drops() { return (Drop *)game_addr(DROPS); }
}
}

namespace ff8fx
{
namespace gastric169
{
	static bool CornerNear(uint32_t hdr, int32_t off)
	{
		S16(hdr, off + 2) = (int16_t)(S16(hdr, off + 2) - U16(hdr, 0xAC));
		S16(hdr, off) = (int16_t)(S16(hdr, off) - U16(hdr, 0xA8));
		S16(hdr, off + 4) = (int16_t)(S16(hdr, off + 4) - U16(hdr, 0xB0));
		int32_t y = S16(hdr, off + 2), x = S16(hdr, off), z = S16(hdr, off + 4);
		int32_t d2 = mul32(x, x);
		d2 = (int32_t)((uint32_t)d2 + (uint32_t)mul32(y, y));
		d2 = (int32_t)((uint32_t)d2 + (uint32_t)mul32(z, z));
		const int32_t d = Sqrt(d2);
		const int32_t r = S32(hdr, 0xB4);
		if (d >= r) return false;
		S16(hdr, off + 6) = (int16_t)(0x1000 - shl32(d, 12) / r);
		return true;
	}

	static uint32_t GlowPrims(uint32_t hdr, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint32_t pk = cursor;
		const uint32_t verts = U32(hdr, 4);
		uint32_t rec = U32(hdr, 0);
		uint32_t bucket = 0;
		for (uint32_t i = 0; i < U16(hdr, 8); i++, rec += 0x10)
		{
			U32(hdr, 0x58) = U32(verts + (U16(rec, 0) & 0xFFF) * 8, 0);
			U32(hdr, 0x5C) = U32(verts + (U16(rec, 0) & 0xFFF) * 8, 4);
			U32(hdr, 0x60) = U32(verts + (U16(rec, 2) & 0xFFF) * 8, 0);
			U32(hdr, 0x64) = U32(verts + (U16(rec, 2) & 0xFFF) * 8, 4);
			U32(hdr, 0x68) = U32(verts + (U16(rec, 4) & 0xFFF) * 8, 0);
			U32(hdr, 0x6C) = U32(verts + (U16(rec, 4) & 0xFFF) * 8, 4);
			GteLoadV012(hdr + 0x58, hdr + 0x60, hdr + 0x68);
			GteRTPT();
			U32(pk, 0) = 0x7000000;
			const uint32_t w = U32(rec, 0xC);
			U32(pk, 4) = (w & 0x2000000) | U32(hdr, 0x1C) | 0x24000000;
			U32(pk, 0xC) = U32(rec, 8);
			U32(pk, 0x14) = w & 0x1FFFFFF;
			GteNCLIP();
			GteReadMAC0(hdr + 0x48);
			if (S32(hdr, 0x48) <= 0) continue;
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteAVSZ3();
			S32(pk, 0x1C) = S32(rec, 4) >> 16;
			GteReadOTZ32(hdr + 0x50);
			bucket = ot + (uint32_t)shl32(S32(hdr, 0x50) >> (shift & 31), 2);
			InsertPrim(bucket, pk);
			pk += 0x20;
			uint32_t mask = 0;
			if (CornerNear(hdr, 0x58)) mask = 1;
			if (CornerNear(hdr, 0x60)) mask |= 2;
			if (CornerNear(hdr, 0x68)) mask |= 4;
			if (!mask) continue;
			U32(pk, 4) = 0x32000000;
			U32(pk, 0) = 0x6000000;
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteLoadRGBC(pk + 4);
			GteSetIR0FromWord(hdr + 0x5E);
			GteDPCS();
			GteStoreRGB2(pk + 4);
			GteSetIR0FromWord(hdr + 0x66);
			GteDPCS();
			GteStoreRGB2(pk + 0xC);
			GteSetIR0FromWord(hdr + 0x6E);
			GteDPCS();
			GteStoreRGB2(pk + 0x14);
			InsertPrim(bucket - 4, pk);
			pk += 0x1C;
		}
		const uint32_t quads = rec + 4;
		for (uint32_t i = 0; i < U16(hdr, 0xA); i++)
		{
			const uint32_t q = quads + i * 0x14;
			U32(hdr, 0x58) = U32(verts + (U16(q, -4) & 0xFFF) * 8, 0);
			U32(hdr, 0x5C) = U32(verts + (U16(q, -4) & 0xFFF) * 8, 4);
			U32(hdr, 0x60) = U32(verts + (U16(q, -2) & 0xFFF) * 8, 0);
			U32(hdr, 0x64) = U32(verts + (U16(q, -2) & 0xFFF) * 8, 4);
			U32(hdr, 0x68) = U32(verts + (U16(q, 0) & 0xFFF) * 8, 0);
			U32(hdr, 0x6C) = U32(verts + (U16(q, 0) & 0xFFF) * 8, 4);
			GteLoadV012(hdr + 0x58, hdr + 0x60, hdr + 0x68);
			GteRTPT();
			U32(pk, 0) = 0x9000000;
			U32(hdr, 0x70) = U32(verts + (U16(q, 2) & 0xFFF) * 8, 0);
			U32(hdr, 0x74) = U32(verts + (U16(q, 2) & 0xFFF) * 8, 4);
			const uint32_t w = U32(q, 8);
			U32(pk, 0xC) = U32(q, 4);
			U32(pk, 0x14) = w & 0x1FFFFFF;
			U32(pk, 4) = (w & 0x2000000) | U32(hdr, 0x1C) | 0x2C000000;
			GteNCLIP();
			GteReadMAC0(hdr + 0x48);
			if (S32(hdr, 0x48) <= 0) continue;
			const uint32_t xy0 = pk + 8;
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteLoadV0u(hdr + 0x70);
			GteRTPS();
			U32(pk, 0x1C) = U32(q, 0xC);
			S32(pk, 0x24) = S32(q, 0xC) >> 16;
			GteReadSXY2u(pk + 0x20);
			GteAVSZ4();
			GteReadOTZ32(hdr + 0x50);
			bucket = ot + (uint32_t)shl32(S32(hdr, 0x50) >> (shift & 31), 2);
			InsertPrim(bucket, pk);
			uint32_t mask = 0;
			if (CornerNear(hdr, 0x58)) mask = 1;
			if (CornerNear(hdr, 0x60)) mask |= 2;
			if (CornerNear(hdr, 0x68)) mask |= 4;
			if (CornerNear(hdr, 0x70)) mask |= 8;
			if (!mask)
			{
				pk += 0x28;
				continue;
			}
			pk += 0x28;
			U32(pk, 8) = U32(xy0, 0);
			U32(pk, 0) = 0x8000000;
			U32(pk, 4) = 0x3A000000;
			GteReadSXY012(pk + 0x10, pk + 0x18, pk + 0x20);
			GteLoadRGBC(pk + 4);
			GteSetIR0FromWord(hdr + 0x5E);
			GteDPCS();
			GteStoreRGB2(pk + 4);
			GteSetIR0FromWord(hdr + 0x66);
			GteDPCS();
			GteStoreRGB2(pk + 0xC);
			GteSetIR0FromWord(hdr + 0x6E);
			GteDPCS();
			GteStoreRGB2(pk + 0x14);
			GteSetIR0FromWord(hdr + 0x76);
			GteDPCS();
			GteStoreRGB2(pk + 0x1C);
			InsertPrim(bucket - 4, pk);
			pk += 0x24;
		}
		return pk;
	}

	static uint32_t ModelObjects(uint32_t model, uint32_t hdr, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t bones = U32(model, 0) + 0x10;
		uint32_t table = U32(model, 4);
		const int32_t count = S32(table, 0);
		GteSetFarColor(U8(hdr, 0xB8), U8(hdr, 0xB9), U8(hdr, 0xBA));
		table += 4;
		for (int32_t i = 0; i < count; i++)
		{
			uint32_t obj = U32(model, 4) + U32(table, 0);
			table += 4;
			if (!(U32(hdr, 0x20) & (1u << (i & 31)))) continue;
			int32_t groups = S16(obj, 0);
			uint32_t out = U32(hdr, 4);
			obj += 2;
			if (groups > 0)
			{
				do
				{
					const int32_t bone = S16(obj, 0);
					obj += 2;
					const Mat4x3 *m = (const Mat4x3 *)(bones + (uint32_t)(bone * 0x30) + 0x10);
					GteSetRotMatrixCtrl(m);
					GteSetTransVectorCtrl(m);
					int32_t nv = S16(obj, 0);
					obj += 2;
					for (; nv != 0; nv--)
					{
						const uint32_t xy = (uint32_t)U16(obj, 0) | ((uint32_t)U16(obj, 2) << 16);
						const uint32_t z = U16(obj, 4);
						GteWriteData(xy, 0);
						GteWriteData(z, 1);
						obj += 6;
						out += 8;
						GteMVMVA_RotV0Tr2();
						int32_t a, b, c;
						GteReadData(&a, 9);
						GteReadData(&b, 10);
						GteReadData(&c, 11);
						U32(out, -8) = (uint32_t)shl32(b, 16) | ((uint32_t)a & 0xFFFF);
						U16(out, -4) = (uint16_t)c;
					}
				} while (--groups != 0);
			}
			obj = (obj + 3) & ~3u;
			U16(hdr, 8) = U16(obj, 0);
			U16(hdr, 0xA) = U16(obj, 2);
			U16(hdr, 0xC) = U16(obj, 4);
			U16(hdr, 0xE) = U16(obj, 6);
			U32(hdr, 0) = obj + 0xC;
			GteSetRotMatrixCtrl(&Camera());
			GteSetTransVectorCtrl(&Camera());
			cursor = GlowPrims(hdr, ot, shift, cursor);
		}
		return cursor;
	}

	static void ModelRender(uint32_t hdr, uint32_t entity)
	{
		const uint32_t anim = entity + 0x60;
		if (!(U8(entity, 0) & 0x20))
			FrameCursor() = DrawShadow(entity, var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4064, 0x10, FrameCursor());
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(entity + 0x40), (Mat4x3 *)(hdr + 0xBC));
		ComputeBonesWorldMatrices((void *)anim, (void *)(entity + 0x40));
		U32(hdr, 0x1C) = U32(entity, 0x28);
		U32(hdr, 0x20) = U32(entity, 0x7C);
		const uint8_t cue = U8(entity, 7);
		U8(hdr, 0x2A) = cue;
		U8(hdr, 0x29) = cue;
		U8(hdr, 0x28) = cue;
		U32(hdr, 4) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		U32(hdr, 0x10) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		U16(hdr, 0x14) = 0;
		U16(hdr, 0x16) = 0;
		U16(hdr, 0x18) = 0x140;
		U16(hdr, 0x1A) = 0xD8;
		FrameCursor() = ModelObjects(U32(anim, 4), hdr, RenderOT44(), 2, FrameCursor());
		BuildBoneMatricesFromPose((void *)anim);
		const uint32_t weapon = U32(entity, 0x78);
		if (weapon && !(U8(entity, 1) & 2))
		{
			U32(hdr, 0x20) = 0xFFFFFFFF;
			ComputeBonesWorldMatrices((void *)weapon, (void *)(hdr + 0xBC));
			FrameCursor() = RenderGeometry(U32(weapon, 4), hdr, RenderOT44(), 4, FrameCursor());
			BuildBoneMatricesFromPose((void *)weapon);
		}
	}

	static int32_t TintFade(int32_t c)
	{
		if (c < 8) return shl32(8 - c, 9);
		if (c >= 0xD) return shl32(c - 0xD, 9);
		return 0;
	}

	static void TintDraw(const Node *n, int32_t fade)
	{
		uint8_t *entity = Entity(TargetSlot());
		entity[0] |= 4;
		const uint32_t h = P(FieldAlloc(0xDC));
		S32(h, 0xA8) = n->pos[0];
		S32(h, 0xAC) = 0;
		S32(h, 0xB0) = n->pos[2];
		S32(h, 0xB4) = 0x4B0;
		U8(h, 0xB8) = 0x98;
		U8(h, 0xB9) = 0xC0;
		U8(h, 0xBA) = 0x28;
		GteSetFarColorB(0, 0, 0);
		GteLoadRGBC(h + 0xB8);
		GteSetIR0(fade);
		GteDPCS();
		GteStoreRGB2(h + 0xB8);
		ModelRender(h, P(entity));
		FieldFree(0xDC);
	}

	static uint32_t __cdecl TintTask(TaskNode *tn)
	{
		Node *n = (Node *)tn;
		if (n->counter < 0x16)
		{
			TintDraw(n, TintFade(n->counter));
		}
		if (DrawOnly()) return 0;
		n->counter++;
		if (n->counter <= 0x16) return 0;
		*(uint16_t *)Entity(TargetSlot()) &= 0xFFFB;
		return TASK_END;
	}

	static uint32_t ClipX(uint32_t xy, uint32_t bit) { const int16_t x = S16(xy, 0); return (x < 0 || x > 0xA00) ? bit : 0; }
	static uint32_t ClipY(uint32_t xy, uint32_t bit) { const int16_t y = S16(xy, 2); return (y < 0 || y > 0x6C0) ? bit : 0; }

	static void PageWords(uint32_t h, uint32_t pk, int32_t page_off)
	{
		const uint32_t f = U32(h, 0x1C);
		if (f & 0x400) U16(pk, page_off) = (uint16_t)(U16(pk, page_off) + U16(h, 0x10));
		else if (f & 0x100) U16(pk, page_off) = U16(h, 0x10);
		if (f & 0x800) U16(pk, 0xE) = (uint16_t)(U16(pk, 0xE) + U16(h, 0x14));
		else if (f & 0x200) U16(pk, 0xE) = U16(h, 0x14);
	}

	static void SemiTrans(uint32_t h, uint32_t pk, uint32_t set, uint32_t clear)
	{
		const uint32_t f = U32(h, 0x1C);
		if (f & set) U32(pk, 4) = U32(pk, 4) | 0x2000000;
		if (f & clear) U32(pk, 4) &= 0xFDFFFFFF;
	}

	static uint32_t PrimFT3(uint32_t h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		const uint32_t verts = U32(h, 4);
		int32_t count = S32(U32(h, 0x20), 0);
		uint32_t rec = U32(h, 0x20) + 4;
		U32(h, 0x20) = rec;
		for (; count > 0; count--, rec += 0x14)
		{
			GteLoadV012(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPT();
			U32(pk, 0) = 0x8000000;
			U32(pk, 4) = U32(rec, 0);
			SemiTrans(h, pk, 1, 4);
			const uint32_t d = U32(h, 0x18);
			U32(pk, 0xC) = U32(rec, 0xC) + d;
			U32(pk, 0x14) = U32(rec, 0x10) + d;
			U32(pk, 0x1C) = (U32(rec, 8) >> 16) + d;
			GteReadFLAG(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			PageWords(h, pk, 0x16);
			GteReadMAC0(h + 0x24);
			const int32_t nc = S32(h, 0x24);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x1C) & 0x10)) continue;
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteAVSZ3();
			uint32_t m = ClipX(pk + 8, 1) | ClipX(pk + 0x10, 2) | ClipX(pk + 0x18, 4);
			m |= ClipY(pk + 8, 0x10) | ClipY(pk + 0x10, 0x20) | ClipY(pk + 0x18, 0x40);
			if ((m & 7) == 7 || (m & 0x70) == 0x70) continue;
			GteReadOTZ32(h + 0x2C);
			if (U8(h, 0x1C) & 0x40)
			{
				GteLoadRGBC(pk + 4);
				GteSetIR0(S32(h, 0xC));
				GteDPCS();
				GteStoreRGB2(pk + 4);
			}
			U32(pk, 0x20) = U32(h, 0x58);
			InsertPrim(ot + (uint32_t)shl32(S32(h, 0x2C) >> (shift & 31), 2), pk);
			pk += 0x24;
		}
		U32(h, 0x20) = rec;
		return pk;
	}

	static uint32_t PrimFT4(uint32_t h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		const uint32_t verts = U32(h, 4);
		int32_t count = S32(U32(h, 0x20), 0);
		uint32_t rec = U32(h, 0x20) + 4;
		U32(h, 0x20) = rec;
		for (; count > 0; count--, rec += 0x18)
		{
			GteLoadV012(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPT();
			U32(pk, 0) = 0xA000000;
			U32(pk, 4) = U32(rec, 0);
			SemiTrans(h, pk, 1, 4);
			const uint32_t d = U32(h, 0x18);
			U32(pk, 0xC) = U32(rec, 0xC) + d;
			const uint32_t uv3 = (d << 16) + d + U32(rec, 0x14);
			U32(pk, 0x1C) = uv3;
			U32(pk, 0x14) = U32(rec, 0x10) + d;
			U32(pk, 0x24) = uv3 >> 16;
			GteReadFLAG(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			PageWords(h, pk, 0x16);
			GteReadMAC0(h + 0x24);
			const int32_t nc = S32(h, 0x24);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x1C) & 0x10)) continue;
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteLoadV0u(verts + U16(rec, 0xA) * 4);
			GteRTPS();
			uint32_t m = ClipX(pk + 8, 1) | ClipX(pk + 0x10, 2) | ClipX(pk + 0x18, 4);
			m |= ClipY(pk + 8, 0x10) | ClipY(pk + 0x10, 0x20) | ClipY(pk + 0x18, 0x40);
			GteReadSXY2u(pk + 0x20);
			GteAVSZ4();
			m |= ClipX(pk + 0x20, 8) | ClipY(pk + 0x20, 0x80);
			if ((m & 0xF) == 0xF || (m & 0xF0) == 0xF0) continue;
			GteReadOTZ32(h + 0x2C);
			if (U8(h, 0x1C) & 0x40)
			{
				GteLoadRGBC(pk + 4);
				GteSetIR0(S32(h, 0xC));
				GteDPCS();
				GteStoreRGB2(pk + 4);
			}
			U32(pk, 0x28) = U32(h, 0x58);
			InsertPrim(ot + (uint32_t)shl32(S32(h, 0x2C) >> (shift & 31), 2), pk);
			pk += 0x2C;
		}
		U32(h, 0x20) = rec;
		return pk;
	}

	static uint32_t PrimGT3(uint32_t h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		const uint32_t verts = U32(h, 4);
		int32_t count = S32(U32(h, 0x20), 0);
		uint32_t rec = U32(h, 0x20) + 4;
		U32(h, 0x20) = rec;
		for (; count > 0; count--, rec += 0x1C)
		{
			GteLoadV012(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPT();
			U32(pk, 0) = 0xA000000;
			U32(pk, 4) = U32(rec, 0);
			SemiTrans(h, pk, 2, 8);
			const uint32_t d = U32(h, 0x18);
			U32(pk, 0xC) = U32(rec, 0xC) + d;
			U32(pk, 0x18) = U32(rec, 0x10) + d;
			U32(pk, 0x24) = (U32(rec, 8) >> 16) + d;
			GteReadFLAG(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			PageWords(h, pk, 0x1A);
			GteReadMAC0(h + 0x24);
			const int32_t nc = S32(h, 0x24);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x1C) & 0x20)) continue;
			GteReadSXY012(pk + 8, pk + 0x14, pk + 0x20);
			GteAVSZ3();
			uint32_t m = ClipX(pk + 8, 1) | ClipX(pk + 0x14, 2) | ClipX(pk + 0x20, 4);
			m |= ClipY(pk + 8, 0x10) | ClipY(pk + 0x14, 0x20) | ClipY(pk + 0x20, 0x40);
			if ((m & 7) == 7 || (m & 0x70) == 0x70) continue;
			GteReadOTZ32(h + 0x2C);
			if (U8(h, 0x1C) & 0x80)
			{
				GteLoadRGB012(rec + 0x14, rec + 0x18, pk + 4);
				GteSetIR0(S32(h, 0xC));
				GteDPCT();
				GteStoreRGB012(pk + 0x10, pk + 0x1C, pk + 4);
			}
			else
			{
				U32(pk, 0x10) = U32(rec, 0x14);
				U32(pk, 0x1C) = U32(rec, 0x18);
			}
			U32(pk, 0x28) = U32(h, 0x58);
			InsertPrim(ot + (uint32_t)shl32(S32(h, 0x2C) >> (shift & 31), 2), pk);
			pk += 0x2C;
		}
		U32(h, 0x20) = rec;
		return pk;
	}

	static uint32_t PrimGT4(uint32_t h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		const uint32_t verts = U32(h, 4);
		int32_t count = S32(U32(h, 0x20), 0);
		uint32_t rec = U32(h, 0x20) + 4;
		U32(h, 0x20) = rec;
		for (; count > 0; count--, rec += 0x24)
		{
			GteLoadV012(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPT();
			U32(pk, 0) = 0xD000000;
			U32(pk, 4) = U32(rec, 0);
			SemiTrans(h, pk, 2, 8);
			const uint32_t d = U32(h, 0x18);
			U32(pk, 0xC) = U32(rec, 0xC) + d;
			const uint32_t uv3 = (d << 16) + d + U32(rec, 0x14);
			U32(pk, 0x24) = uv3;
			U32(pk, 0x18) = U32(rec, 0x10) + d;
			U32(pk, 0x30) = uv3 >> 16;
			GteReadFLAG(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIP();
			PageWords(h, pk, 0x1A);
			GteReadMAC0(h + 0x24);
			const int32_t nc = S32(h, 0x24);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x1C) & 0x20)) continue;
			GteReadSXY012(pk + 8, pk + 0x14, pk + 0x20);
			GteLoadV0u(verts + U16(rec, 0xA) * 4);
			GteRTPS();
			uint32_t m = ClipX(pk + 8, 1) | ClipX(pk + 0x14, 2) | ClipX(pk + 0x20, 4);
			m |= ClipY(pk + 8, 0x10) | ClipY(pk + 0x14, 0x20) | ClipY(pk + 0x20, 0x40);
			GteReadSXY2u(pk + 0x2C);
			GteAVSZ4();
			m |= ClipX(pk + 0x2C, 8) | ClipY(pk + 0x2C, 0x80);
			if ((m & 0xF) == 0xF || (m & 0xF0) == 0xF0) continue;
			GteReadOTZ32(h + 0x2C);
			if (U8(h, 0x1C) & 0x80)
			{
				GteLoadRGB012(rec + 0x18, rec + 0x1C, rec + 0x20);
				GteSetIR0(S32(h, 0xC));
				GteDPCT();
				GteStoreRGB012(pk + 0x10, pk + 0x1C, pk + 0x28);
				GteLoadRGBC(pk + 4);
				GteDPCS();
				GteStoreRGB2(pk + 4);
			}
			else
			{
				U32(pk, 0x10) = U32(rec, 0x18);
				U32(pk, 0x1C) = U32(rec, 0x1C);
				U32(pk, 0x28) = U32(rec, 0x20);
			}
			U32(pk, 0x34) = U32(h, 0x58);
			InsertPrim(ot + (uint32_t)shl32(S32(h, 0x2C) >> (shift & 31), 2), pk);
			pk += 0x38;
		}
		U32(h, 0x20) = rec;
		return pk;
	}

	static uint32_t PrimRender(uint32_t h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t f = U32(h, 0x1C);
		if (!(f & 0x2000)) U32(h, 4) = U32(h, 0) + 8;
		U32(h, 0x20) = U32(U32(h, 0), 0) + U32(h, 0);
		if (!(f & 0x1000)) U32(h, 0x18) = 0;
		U32(h, 0x58) = 0xE1000220;
		GteSetFarColor(U8(h, 8), U8(h, 9), U8(h, 0xA));
		U32(h, 0x20) += 8;
		if (U32(U32(h, 0x20), 0)) cursor = PrimFT3(h, ot, shift, cursor);
		else U32(h, 0x20) += 4;
		if (U32(U32(h, 0x20), 0)) cursor = PrimFT4(h, ot, shift, cursor);
		else U32(h, 0x20) += 4;
		U32(h, 0x20) += 8;
		if (U32(U32(h, 0x20), 0)) cursor = PrimGT3(h, ot, shift, cursor);
		else U32(h, 0x20) += 4;
		if (U32(U32(h, 0x20), 0)) cursor = PrimGT4(h, ot, shift, cursor);
		else U32(h, 0x20) += 4;
		return cursor;
	}

	static int32_t MeshBlend(int32_t c) { return ComputeSin(shl32(c, 10) / 32); }

	static int32_t MeshFade(int32_t c)
	{
		if (c < 8) return 0;
		if (c < 0x10) return shl32(c - 8, 9);
		if (c < 0x18) return shl32(8 - (c - 0x10), 9);
		return shl32(c - 0x18, 9);
	}

	static void MeshDraw(const Node *n, uint32_t verts, int32_t s, int32_t fade)
	{
		int16_t ang[3] = { 0, n->a18, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = n->pos[0];
		m.t[1] = n->pos[1];
		m.t[2] = n->pos[2];
		const int32_t sc[3] = { n->a1c, n->a1c, n->a1c };
		Scale3DMatrix(&m, sc);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		const uint32_t h = P(FieldAlloc(0x5C));
		U32(h, 0) = game_addr(MODEL_Mesh);
		U32(h, 8) = 0;
		U32(h, 0x1C) = 0x2033;
		for (int32_t k = 0; k < 0x49; k++)
		{
			const uint32_t a = game_addr(MESH_A) + (uint32_t)k * 8, b = game_addr(MESH_B) + (uint32_t)k * 8, o = verts + (uint32_t)k * 8;
			for (int32_t w = 0; w < 3; w++)
				S16(o, w * 2) = (int16_t)((mul32(S16(b, w * 2) - S16(a, w * 2), s) >> 12) + S16(a, w * 2));
		}
		U32(h, 4) = verts;
		const int32_t c = n->counter;
		if (c < 8)
			PacketCursor() = RenderPrimModel((void *)h, RenderOT44(), 2, PacketCursor());
		else if (c < 0x10)
		{
			S32(h, 0xC) = fade;
			U32(h, 0x1C) |= 0xC0;
			PacketCursor() = RenderPrimModel((void *)h, RenderOT44(), 2, PacketCursor());
		}
		else if (c < 0x20)
		{
			U32(h, 0x1C) |= 0x4C0;
			S32(h, 0x10) = 0x20;
			S32(h, 0xC) = fade;
			PacketCursor() = PrimRender(h, RenderOT44(), 2, PacketCursor());
		}
		FieldFree(0x5C);
	}

	static uint32_t __cdecl MeshTask(TaskNode *tn)
	{
		Node *n = (Node *)tn;
		MeshDraw(n, game_addr(MESH_VERTS) + (uint32_t)(n->idx * 0x248), MeshBlend(n->counter), MeshFade(n->counter));
		if (DrawOnly()) return 0;
		n->counter++;
		return n->counter >= 0x20 ? TASK_END : 0;
	}

	static int32_t PuddleFade(int32_t c) { return c < 8 ? 0x800 : shl32(c, 8); }

	static void PuddleDraw(const Node *n, int32_t fade)
	{
		int16_t ang[3] = { 0, n->a18, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = n->pos[0];
		m.t[1] = n->pos[1];
		m.t[2] = n->pos[2];
		const int32_t sc[3] = { n->a1c, n->a1c, n->a1c };
		Scale3DMatrix(&m, sc);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		const uint32_t h = P(FieldAlloc(0x58));
		U32(h, 0) = game_addr(MODEL_Puddle);
		U32(h, 8) = 0;
		U32(h, 0x1C) = 0xF3;
		S32(h, 0xC) = fade;
		PacketCursor() = RenderPrimModel((void *)h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl PuddleTask(TaskNode *tn)
	{
		Node *n = (Node *)tn;
		PuddleDraw(n, PuddleFade(n->counter));
		if (DrawOnly()) return 0;
		n->a1c = (int16_t)(n->a1c + n->a1e);
		n->counter++;
		return n->counter >= 0x10 ? TASK_END : 0;
	}

	static void BlobDraw(const Node *n, const int16_t *pos, uint32_t seq)
	{
		TransformCameraByShadowRotation(pos, n->a1c, -(n->a1c >> 3));
		const uint32_t h = P(FieldAlloc(0xB4));
		const int16_t c = n->counter;
		U32(h, 0) = seq;
		U16(h, 4) = (uint16_t)(c & 7);
		U16(h, 0x24) = 0;
		if (c < 1)
		{
			const uint32_t v = (uint32_t)shl32(c, 7);
			U16(h, 0x24) = 4;
			U32(h, 0x1C) = (((v << 8) | v) << 8) | v;
		}
		PacketCursor() = InitEffectSequenceFromData((void *)h, RenderOT44(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl BlobTask(TaskNode *tn)
	{
		Node *n = (Node *)tn;
		BlobDraw(n, StreamOf(n->idx)->curve[0] + n->a18 * 4, game_addr(SEQ_Blob));
		if (DrawOnly()) return 0;
		const int16_t v = n->a1a;
		n->a18 = (int16_t)(n->a18 + v);
		if (v > 1 && n->a18 < 0xE) n->a1a = (int16_t)(v - 1);
		n->counter++;
		return n->a18 >= 0x1B ? TASK_END : 0;
	}

	static void DripDraw(uint32_t h, const Drop *e)
	{
		U16(h, 4) = (uint16_t)e->frame;
		TransformCameraByShadowRotation(e->pos, e->scale, -(e->scale >> 4));
		PacketCursor() = InitEffectSequenceFromData((void *)h, RenderOT44(), 2, PacketCursor());
	}

	static uint32_t __cdecl DripTask(TaskNode *tn)
	{
		Node *n = (Node *)tn;
		const uint32_t h = P(FieldAlloc(0xB4));
		int32_t alive = 0;
		U32(h, 0) = game_addr(SEQ_Drip);
		U16(h, 0x24) = 0;
		for (Drop *e = Drops(); P(e) < game_addr(DROPS_END); e++)
		{
			if (!(e->flags & (uint32_t)(int32_t)n->idx)) continue;
			DripDraw(h, e);
			if (DrawOnly()) continue;
			e->frame++;
			if (S16(h, 0x28) < 0)
			{
				e->flags = 0;
				continue;
			}
			e->pos[1] = (int16_t)(e->pos[1] + e->vel[1]);
			e->vel[1] = (int16_t)(e->vel[1] - (int16_t)(e->vel[1] >> 4));
			alive++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		const int16_t c = n->counter;
		if (c >= 0 && c <= 0xB && DRIP_COUNTS()[c] > 0)
		{
			for (int32_t k = 0; k < DRIP_COUNTS()[n->counter]; k++)
			{
				Drop *e = Drops();
				while (P(e) < game_addr(DROPS_END) && e->flags) e++;
				if (P(e) >= game_addr(DROPS_END)) break;
				e->flags = (uint32_t)(int32_t)n->idx;
				e->frame = 0;
				e->scale = (int16_t)(CrtRand() % 0x900 + 0x600);
				ff8fx::copy<(8)>(e->pos, n->pos);
				e->pos[0] = (int16_t)(e->pos[0] + (int16_t)(CrtRand() % 700 - 350));
				e->pos[1] = (int16_t)(e->pos[1] + (int16_t)(-100 - CrtRand() % 300));
				e->pos[2] = (int16_t)(e->pos[2] + (int16_t)(CrtRand() % 700 - 350));
				e->vel[1] = (int16_t)(-40 - CrtRand() % 70);
			}
		}
		n->counter++;
		if (n->counter >= 4 && alive == 0) return TASK_END;
		return 0;
	}

	static void SplashSetup(const Stream *s)
	{
		int32_t down[3] = { 0, -0x1000, 0 };
		int32_t dir[3];
		dir[0] = s->curve[2][0] - s->curve[0][0];
		dir[1] = s->curve[2][1] - s->curve[0][1];
		dir[2] = s->curve[2][2] - s->curve[0][2];
		NormalizeVector(dir, dir);
		int32_t axis[3];
		const int32_t angle = RotationBetweenVectors(down, dir, axis);
		Mat4x3 m;
		BuildAxisAngleRotationMatrix(angle, &m, axis);
		m.t[0] = s->curve[1][0];
		m.t[1] = s->curve[1][1];
		m.t[2] = s->curve[1][2];
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrixCtrl((const Mat4x3 *)game_addr(SPLASH_MATRIX));
		GteSetLightMatrix(&m);
		GteSetBKFromMatrix(&m);
	}

	static void SplashDraw(uint32_t h, const Drop *e)
	{
		Mat4x3 &g = *(Mat4x3 *)game_addr(SPLASH_MATRIX);
		GteLoadV0(e->pos);
		GteMVMVA_LightV0Bk();
		GteSetRotDiagonal16(e->scale);
		U16(h, 4) = (uint16_t)e->frame;
		GteReadMAC123(g.t);
		int32_t nrm[3];
		NormalizeVector(g.t, nrm);
		const int32_t f = -(e->scale >> 4);
		g.t[0] = (int32_t)((uint32_t)g.t[0] + (uint32_t)(mul32(f, nrm[0]) >> 12));
		g.t[1] = (int32_t)((uint32_t)g.t[1] + (uint32_t)(mul32(f, nrm[1]) >> 12));
		g.t[2] = (int32_t)((uint32_t)g.t[2] + (uint32_t)(mul32(f, nrm[2]) >> 12));
		GteSetTransVectorCtrl(&g);
		PacketCursor() = InitEffectSequenceFromData((void *)h, RenderOT44(), 2, PacketCursor());
	}

	static uint32_t __cdecl SplashTask(TaskNode *tn)
	{
		Node *n = (Node *)tn;
		const uint32_t h = P(FieldAlloc(0xB4));
		int32_t alive = 0;
		U32(h, 0) = game_addr(SEQ_Splash);
		U16(h, 0x24) = 0;
		SplashSetup(StreamOf(n->idx));
		for (Drop *e = Drops(); P(e) < game_addr(DROPS_END); e++)
		{
			if (!(e->flags & 0x100)) continue;
			SplashDraw(h, e);
			if (DrawOnly()) continue;
			e->frame++;
			if (S16(h, 0x28) < 0)
			{
				e->flags = 0;
				continue;
			}
			for (int k = 0; k < 3; k++) e->pos[k] = (int16_t)(e->pos[k] + e->vel[k]);
			for (int k = 0; k < 3; k++) e->vel[k] = (int16_t)(e->vel[k] - (int16_t)(e->vel[k] >> 2));
			alive++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		if (n->counter >= 1 && n->counter <= 2)
		{
			for (int32_t k = 0; k < 0xC; k++)
			{
				Drop *e = Drops();
				while (P(e) < game_addr(DROPS_END) && e->flags) e++;
				if (P(e) >= game_addr(DROPS_END)) break;
				e->flags = 0x100;
				e->frame = 0;
				e->scale = (int16_t)(CrtRand() % 0x180 + 0x200);
				e->pos[0] = (int16_t)(CrtRand() % 300 - 150);
				e->pos[1] = 0;
				e->pos[2] = (int16_t)(CrtRand() % 300 - 150);
				const int32_t a = CrtRand() % 4096;
				const int32_t r = CrtRand() % 100 + 0x50;
				e->vel[0] = (int16_t)(mul32(ComputeCos(a), r) >> 12);
				e->vel[1] = (int16_t)(-100 - CrtRand() % 120);
				e->vel[2] = (int16_t)(mul32(ComputeSin(a), r) >> 12);
			}
		}
		n->counter++;
		if (n->counter >= 4 && alive == 0) return TASK_END;
		return 0;
	}

	static void StreamSetup(Stream *s)
	{
		GetEffectSpawnPosition(Entity(AttackerSlot()), 0xF0, 0, s->src);
		GetDefaultEffectPosition(Entity(TargetSlot()), s->dst);
		s->dst[0] = (int16_t)(s->dst[0] + (int16_t)(CrtRand() % 0x578 - 0x2BC));
		s->dst[1] = 0;
		s->dst[2] = (int16_t)(s->dst[2] + (int16_t)(CrtRand() % 0x578 - 0x2BC));
		ff8fx::copy<(8)>(s->cp[0], s->src);
		ff8fx::copy<(8)>(s->cp[3], s->dst);
		const int16_t x0 = s->src[0];
		const int32_t dx = (s->dst[0] - x0) / 3;
		const int32_t dz = (s->dst[2] - s->src[2]) / 3;
		s->cp[1][0] = (int16_t)(x0 + dx);
		s->cp[1][1] = (int16_t)(s->src[1] - (int16_t)(CrtRand() % 0x1F4) - 0x898);
		s->cp[1][2] = (int16_t)(s->src[2] + dz);
		s->cp[2][0] = (int16_t)(dx * 2 + s->src[0]);
		s->cp[2][1] = (int16_t)(s->cp[1][1] - (int16_t)(CrtRand() % 0x258) - 0xC8);
		s->cp[2][2] = (int16_t)(dz * 2 + s->src[2]);
		void *work = FieldAlloc(0x190);
		BezierInit(4, s->cp, work);
		for (int32_t t = 0, k = 0; t < 0x1C000; t += 0x1000, k++)
			BezierPoint(4, work, s->curve[k], t / 27);
		FieldFree(0x190);
	}

	static uint32_t __cdecl StreamTask(TaskNode *tn)
	{
		Node *n = (Node *)tn;
		Stream *s = StreamOf(n->idx);
		if (DrawOnly()) return 0;
		if (n->a18 == 0)
		{
			StreamSetup(s);
			n->a18 = 1;
		}
		if (n->a1a > 0)
		{
			n->a1a--;
			return 0;
		}
		if (n->counter == 1)
		{
			Node *b = (Node *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Blob));
			b->counter = 0;
			b->idx = n->idx;
			b->a18 = 0;
			b->a1a = 4;
			b->a1c = (int16_t)(CrtRand() % 0x180 + 0x140);
		}
		if (n->counter == 0x16)
		{
			Node *m = (Node *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Mesh));
			ff8fx::copy<(8)>(m->pos, s->dst);
			m->counter = 0;
			m->idx = n->idx;
			m->a18 = (int16_t)(CrtRand() % 4096);
			m->a1a = (int16_t)(CrtRand() % 6 + 0x10);
			m->a1c = (int16_t)(CrtRand() % 0x600 + 0x400);
			if (n->counter == 0x16)
			{
				Node *d = (Node *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Drips));
				d->counter = 0;
				d->idx = (int16_t)(1u << ((uint8_t)n->idx & 31));
				ff8fx::copy<(8)>(d->pos, s->dst);
			}
		}
		if (n->counter == 0x15)
		{
			Node *p = (Node *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Puddle));
			ff8fx::copy<(8)>(p->pos, s->dst);
			p->counter = 0;
			p->a18 = (int16_t)(CrtRand() % 4096);
			const int16_t g = (int16_t)((CrtRand() % 2048 + 0xD00) / 10);
			p->a1e = g;
			p->a1c = g;
		}
		n->counter++;
		return n->counter >= 0x28 ? TASK_END : 0;
	}

	static uint32_t __cdecl DirectorTask(TaskNode *tn)
	{
		Node *n = (Node *)tn;
		if (DrawOnly()) return 0;
		if (n->counter == 6)
		{
			for (int k = 0; k < 5; k++)
			{
				Node *s = (Node *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Stream));
				const int16_t i = n->idx;
				s->counter = 0;
				s->idx = i;
				s->a18 = 0;
				const int32_t d = CrtRand() % 4;
				n->idx++;
				s->a1a = (int16_t)d;
			}
			Node *sp = (Node *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Splash));
			sp->counter = 0;
			sp->idx = 0;
		}
		if (n->counter == 0x1E)
		{
			Node *t = (Node *)AddTaskToQueue(EffectQueue(), game_addr(ORIG_Tint));
			t->counter = 0;
			GetDefaultEffectPosition(Entity(TargetSlot()), t->pos);
		}
		if (n->counter == 0x23) ApplyActionResultToTarget(Ctx()->actions[0].targets);
		if (n->counter == 1) BdPlaySE(SOUND_Juice(), 0, 0x80);
		n->counter++;
		return n->counter > 0x32 ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *tn)
	{
		RootNode *r = (RootNode *)tn;
		if (r->counter & 1) PacketCursor() = TexBase() + 0x8000;
		else PacketCursor() = TexBase();
		const int a = ExecuteTaskQueue(EffectQueue());
		r->counter++;
		return a ? 0 : TASK_END;
	}
}

	void register_mag169_gastric_juice()
	{
		register_port(gastric169::ORIG_Root, (void *)gastric169::RootTask, "G169 RootTask", 169);
		register_port(gastric169::ORIG_Director, (void *)gastric169::DirectorTask, "G169 DirectorTask", 169);
		register_port(gastric169::ORIG_Tint, (void *)gastric169::TintTask, "G169 TintTask", 169);
		register_port(gastric169::ORIG_Stream, (void *)gastric169::StreamTask, "G169 StreamTask", 169);
		register_port(gastric169::ORIG_Blob, (void *)gastric169::BlobTask, "G169 BlobTask", 169);
		register_port(gastric169::ORIG_Mesh, (void *)gastric169::MeshTask, "G169 MeshTask", 169);
		register_port(gastric169::ORIG_Drips, (void *)gastric169::DripTask, "G169 DripTask", 169);
		register_port(gastric169::ORIG_Puddle, (void *)gastric169::PuddleTask, "G169 PuddleTask", 169);
		register_port(gastric169::ORIG_Splash, (void *)gastric169::SplashTask, "G169 SplashTask", 169);
	}
}

