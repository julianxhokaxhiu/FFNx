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

// Effect 172: Morph

#include "mag_common.h"

namespace ff8fx
{
namespace morph172
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

	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::MAG_172_Morph_RootQueue); }
	inline TaskQueue *EffectQueue() { return (TaskQueue *)game_addr(game::MAG_172_Morph_EffectQueue); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_172_Morph_CastContext)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::MAG_172_Morph_CasterSlot)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_172_Morph_PacketCursor)); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint8_t *TintEntity() { return Entity(CasterSlot()); }
	static constexpr game::Id SPARKS = game::unk_239A908, SPARKS_END = game::unk_239B268;
	static constexpr game::Id RING_VERTS_A = game::unk_239BEB8, RING_VERTS_B = game::unk_239B268;
	static constexpr game::Id RADII = game::unk_239BA90, RADII_END = game::unk_239BEA4;
	static constexpr game::Id MESH = game::unk_D629A4;
	static constexpr game::Id ARENA_A = game::unk_239D6FC, ARENA_B = game::unk_23A56FC;

	static constexpr game::Id ORIG_Root = game::au_re_BdlinkTask_15, ORIG_Director = game::MAG_172_Morph_Director, ORIG_Ring = game::MAG_172_sub_5D9F20, ORIG_Ring2 = game::MAG_172_sub_5DA0F0, ORIG_Sparks = game::MAG_172_Morph_Sparkles, ORIG_Tint = game::MAG_172_Morph_Tint;
	static constexpr game::Id MODEL_Ring = game::MAG_172_Morph_ModelRing, SEQ_Spark = game::unk_D61F8C, SOUND_Morph = game::unk_D655DC;
	static const uint32_t *TINT_COLOURS() { return (const uint32_t *)game_addr(game::unk_D655E4); }

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
	inline void GteMatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline const Mat4x3 *ShadowCamera(const void *pos, int32_t r, int32_t pull) { return fn<const Mat4x3 *(__cdecl *)(const void *, int32_t, int32_t)>(game_addr(game::TransformCameraByShadowRotation))(pos, r, pull); }
}
}

namespace ff8fx
{
namespace morph172
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
			U32(pk, 4) = U32(hdr, 0x1C) | 0x24000000;
			U32(pk, 0xC) = U32(rec, 8);
			U32(pk, 0x14) = w;
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
			U32(pk, 0x14) = w;
			U32(pk, 4) = U32(hdr, 0x1C) | 0x2C000000;
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

	static uint32_t TintColour(int32_t c) { return TINT_COLOURS()[c % 0x14]; }
	static void TintDraw(uint32_t n, int32_t fade, uint32_t colour)
	{
		uint8_t *entity = TintEntity();
		entity[0] |= 4;
		const uint32_t h = P(FieldAlloc(0xDC));
		S32(h, 0xAC) = S16(n, 0x12);
		S32(h, 0xA8) = S16(n, 0x10);
		S32(h, 0xB0) = S16(n, 0x14);
		S32(h, 0xB4) = 0x3E8;
		U32(h, 0xB8) = colour;
		GteSetFarColorB(0, 0, 0);
		GteLoadRGBC(h + 0xB8);
		GteSetIR0(fade);
		GteDPCS();
		GteStoreRGB2(h + 0xB8);
		ModelRender(h, P(entity));
		FieldFree(0xDC);
	}

	static int32_t TintFade(int32_t c) { return c < 8 ? shl32(8 - c, 9) : (c >= 0x30 ? shl32(c - 0x30, 9) : 0); }

	static uint32_t __cdecl TintTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		if (S16(n, 0xC) < 0x38)
		{
			TintDraw(n, TintFade(S16(n, 0xC)), TintColour(S16(n, 0xC)));
		}
		if (DrawOnly()) return 0;
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		if (S16(n, 0xC) < 0x38) return 0;
		*(uint16_t *)TintEntity() &= 0xFFFB;
		return TASK_END;
	}

	static void MeshRadii()
	{
		uint32_t v = game_addr(MESH);
		for (uint32_t o = game_addr(RADII); o < game_addr(RADII_END); o += 4, v += 8)
		{
			const int32_t x = S16(v, 0), y = S16(v, 2);
			S32(o, 0) = Sqrt(mul32(x, x) + mul32(y, y));
		}
	}

	static void WaveVerts(uint32_t out, int32_t amp)
	{
		uint32_t v = game_addr(MESH) + 2, r = game_addr(RADII), o = out + 2;
		for (; r < game_addr(RADII_END); r += 4, v += 8, o += 8)
		{
			const int32_t a = mul32(ComputeSin(S32(r, 0) * 3), amp) >> 12;
			const int32_t c = ComputeCos(a), s = ComputeSin(a);
			const int32_t x = S16(v, -2), y = S16(v, 0);
			S16(o, -2) = (int16_t)((mul32(x, c) >> 12) + (mul32(y, s) >> 12));
			S16(o, 0) = (int16_t)((-mul32(x, s) >> 12) + (mul32(y, c) >> 12));
			S16(o, 2) = S16(v, 2);
		}
	}

	static int32_t RingFade(int32_t c) { return c < 8 ? shl32(0x20 - c, 7) : (c >= 0x30 ? shl32(c - 0x18, 7) : 0xC00); }

	static void RingDraw(uint32_t n, int32_t mul, uint32_t verts, int32_t fade)
	{
		const int16_t r = S16(n, 0x1C);
		const Mat4x3 *sm = ShadowCamera((const void *)(n + 0x10), (int32_t)(uint16_t)r, r >> 3);
		const int16_t ang[3] = { 0, 0, (int16_t)(S16(n, 0x18) * mul) };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		GteMatrixMultiply(sm, &m);
		GteSetRotMatrix(&m);
		WaveVerts(verts, 0);
		const uint32_t h = P(FieldAlloc(0x58));
		U32(h, 0) = game_addr(MODEL_Ring);
		U32(h, 4) = verts;
		S32(h, 0xC) = 0xC00;
		U32(h, 8) = 0;
		U32(h, 0x1C) = 0x20F3;
		S32(h, 0xC) = fade;
		PacketCursor() = RenderPrimModel((void *)h, RenderOT44(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t RingTaskImpl(uint32_t n, bool second)
	{
		RingDraw(n, second ? -3 : -2, second ? game_addr(RING_VERTS_B) : game_addr(RING_VERTS_A), RingFade(S16(n, 0xC)));
		if (DrawOnly()) return 0;
		S16(n, 0x18) = (int16_t)(S16(n, 0x18) + S16(n, 0x1A));
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= 0x38 ? TASK_END : 0;
	}

	static uint32_t __cdecl RingTask(TaskNode *tn) { return RingTaskImpl(P(tn), false); }
	static uint32_t __cdecl Ring2Task(TaskNode *tn) { return RingTaskImpl(P(tn), true); }

	static void SparkDraw(uint32_t h, uint32_t p)
	{
		const int16_t size = S16(p, 6);
		U16(h, 4) = U16(p, 4);
		S32(h, 0xC) = S16(p, 0x10);
		S32(h, 0x10) = S16(p, 0x12);
		TransformCameraByShadowRotation((const void *)(p + 8), (int32_t)(uint16_t)size, -(size >> 4));
		PacketCursor() = InitEffectSequenceFromData((void *)h, RenderOT44(), 2, PacketCursor());
	}

	static uint32_t __cdecl SparksTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		const uint32_t h = P(FieldAlloc(0xB4));
		int alive = 0;
		U32(h, 0) = game_addr(SEQ_Spark);
		U16(h, 0x24) = 2;
		S32(h, 0x14) = 0x1000;
		for (uint32_t p = game_addr(SPARKS); p < game_addr(SPARKS_END); p += 0x18)
		{
			if (!(U8(p, 0) & 1)) continue;
			SparkDraw(h, p);
			if (DrawOnly()) continue;
			S16(p, 4) = (int16_t)(S16(p, 4) + 1);
			if (S16(h, 0x28) < 0)
			{
				U32(p, 0) = 0;
				continue;
			}
			alive++;
		}
		FieldFree(0xB4);
		if (DrawOnly()) return 0;
		const int16_t c = S16(n, 0xC);
		if (c >= 0 && c <= 0x2A)
		{
			const int32_t bones = *(const uint8_t *)U32(U32(P(TintEntity()), 0x64), 0);
			int32_t i = 0;
			uint32_t p = game_addr(SPARKS);
			for (; U32(p, 0) != 0; p += 0x18, i++)
				if (p + 0x18 >= game_addr(SPARKS_END)) { p = 0; break; }
			if (p && i < 0x64)
			{
				U32(p, 0) = 1;
				S16(p, 4) = 0;
				S16(p, 6) = (int16_t)(CrtRand() % 0x680 + 0x140);
				const int32_t flag = CrtRand() & 0xFFF;
				const int32_t bone = CrtRand() % bones;
				GetEffectSpawnPosition(TintEntity(), bone, flag, (int16_t *)(p + 8));
				S16(p, 8) = (int16_t)(S16(p, 8) + CrtRand() % 0x168 - 0xB4);
				S16(p, 0xA) = (int16_t)(S16(p, 0xA) + CrtRand() % 0x168 - 0xB4);
				S16(p, 0xC) = (int16_t)(S16(p, 0xC) + CrtRand() % 0x168 - 0xB4);
				S16(p, 0x10) = (int16_t)((U8(p, 8) & 1) ? 0x1000 : -0x1000);
				S16(p, 0x12) = (int16_t)((U8(p, 0xA) & 1) ? 0x1000 : -0x1000);
			}
		}
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= 4 && !alive ? TASK_END : 0;
	}

	static uint32_t NewTask(uint32_t f)
	{
		const uint32_t t = P(AddTaskToQueue(EffectQueue(), f));
		S16(t, 0xC) = 0;
		return t;
	}

	static uint32_t __cdecl DirectorTask(TaskNode *tn)
	{
		if (DrawOnly()) return 0;
		const uint32_t n = P(tn);
		if (S16(n, 0xC) == 0) GetDefaultEffectPosition(TintEntity(), (int16_t *)(n + 0x10));
		if (S16(n, 0xC) == 1)
		{
			uint32_t t = NewTask(game_addr(ORIG_Ring));
			U32(t, 0x10) = U32(n, 0x10);
			U32(t, 0x14) = U32(n, 0x14);
			S16(t, 0x18) = 0;
			S16(t, 0x1A) = -10;
			S16(t, 0x1C) = 0xC00;
			t = NewTask(game_addr(ORIG_Ring2));
			U32(t, 0x10) = U32(n, 0x10);
			U32(t, 0x14) = U32(n, 0x14);
			S16(t, 0x18) = 0;
			S16(t, 0x1A) = -10;
			S16(t, 0x1C) = 0x1600;
			MeshRadii();
			NewTask(game_addr(ORIG_Sparks));
		}
		if (S16(n, 0xC) == 8)
		{
			const uint32_t t = NewTask(game_addr(ORIG_Tint));
			U32(t, 0x10) = U32(n, 0x10);
			U32(t, 0x14) = U32(n, 0x14);
		}
		if (S16(n, 0xC) == 1)
		{
			int16_t at[4];
			GetDefaultEffectPosition(TintEntity(), at);
			BdPlaySE3D((const void *)game_addr(SOUND_Morph), 0, at);
		}
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) > 0x3C ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		PacketCursor() = (U8(n, 0xC) & 1) ? game_addr(ARENA_B) : game_addr(ARENA_A);
		const int running = ExecuteTaskQueue(EffectQueue());
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return running ? 0 : TASK_END;
	}

	void register_mag172_morph_ports()
	{
		register_port(ORIG_Root, (void *)RootTask, "172 RootTask", 172);
		register_port(ORIG_Director, (void *)DirectorTask, "172 DirectorTask", 172);
		register_port(ORIG_Ring, (void *)RingTask, "172 RingTask", 172);
		register_port(ORIG_Ring2, (void *)Ring2Task, "172 Ring2Task", 172);
		register_port(ORIG_Sparks, (void *)SparksTask, "172 SparksTask", 172);
		register_port(ORIG_Tint, (void *)TintTask, "172 TintTask", 172);
	}
}

	void register_mag172_morph() { morph172::register_mag172_morph_ports(); }
}

