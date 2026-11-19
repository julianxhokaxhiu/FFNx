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

// Effect 171: Poison Gas

#include "mag_common.h"

namespace ff8fx
{
namespace poisongas171
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

	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::MAG_171_PoisonGas_RootQueue); }
	inline TaskQueue *EffectQueue() { return (TaskQueue *)game_addr(game::MAG_171_PoisonGas_EffectQueue); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_171_PoisonGas_CastContext)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::MAG_171_PoisonGas_CasterSlot)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_171_PoisonGas_MagicBufferBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_171_PoisonGas_PacketCursor)); }
	inline uint32_t &FrameCursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	static constexpr game::Id PARTS_A = game::unk_23AD728, PARTS_B = game::unk_23AE088, PARTS_END = game::MAG_171_PoisonGas_EffectQueue;
	static constexpr game::Id ROWS = game::unk_D68CC0;

	static constexpr game::Id ORIG_Root = game::MAG_171_PoisonGas_Root, ORIG_Director = game::MAG_171_PoisonGas_Director, ORIG_Tint = game::MAG_171_sub_5DB620, ORIG_Puff = game::MAG_171_PoisonGas_Puff, ORIG_Wisp = game::MAG_171_PoisonGas_Wisp, ORIG_Cloud = game::MAG_171_PoisonGas_Cloud, ORIG_Emitter = game::MAG_171_PoisonGas_Emitter;
	static constexpr game::Id SEQ_Puff = game::MAG_171_PoisonGas_SeqPuff, SEQ_Emitter = game::unk_D656F4, MODEL_Cloud = game::unk_D65A4C, SOUND_Gas = game::unk_D68CB4;

	inline uint32_t RenderOT44() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }

	inline void GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t mode, int16_t *out) { fn<void (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, mode, out); }
	inline uint32_t DrawShadow(uint32_t entity, uint32_t ot, uint32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, mode, cursor); }
	inline uint32_t RenderGeometry(uint32_t model, uint32_t hdr, uint32_t ot, uint32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::RenderGeometry))(model, hdr, ot, mode, cursor); }
	inline void MatVec(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
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
	inline void GteLoadV0SVec3(uint32_t v) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadV0FromSVec3))(v); }
	inline void GteMVMVA_RotV0Tr2() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0_Tr_2))(); }
	inline void GteStoreIR123SVec(uint32_t v) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreIR123ToSVec))(v); }
	inline void InsertPrim(uint32_t bucket, uint32_t pkt) { InsertPrimAutoDepth(bucket, (void *)pkt); }

	static int16_t SpawnPad() { return 0; }

	enum { P_MASK = 0, P_FRAME = 4, P_SIZE = 6, P_POS = 8, P_VEL = 0x10 };

	static uint32_t FreeParticle(uint32_t lo, uint32_t hi)
	{
		int32_t i = 0;
		uint32_t p = lo;
		for (; U32(p, P_MASK) != 0; p += 0x18, i++)
			if (p + 0x18 >= hi) return 0;
		return i < 0x64 ? p : 0;
	}
}
}

namespace ff8fx
{
namespace poisongas171
{
	static void GasScroll(uint32_t hdr, uint32_t o, const int32_t *off, int n, int32_t h)
	{
		const int32_t s = S32(hdr, 0x84);
		if (!s) return;
		int32_t a[4];
		for (int i = 0; i < n; i++) { a[i] = (int32_t)U8(o, off[i] + 1) + s; S32(hdr, 0x70 + 4 * i) = a[i]; }
		bool hi = false, lo = false;
		for (int i = 0; i < n; i++) if (a[i] >= 0x100) hi = true;
		if (!hi) for (int i = 0; i < n; i++) if (a[i] < 0) lo = true;
		if (hi) for (int i = 0; i < n; i++) S32(hdr, 0x70 + 4 * i) = a[i] - h;
		else if (lo) for (int i = 0; i < n; i++) S32(hdr, 0x70 + 4 * i) = a[i] + h;
		if (hi || lo) for (int i = 0; i < n; i++) U8(o, off[i] + 1) = U8(hdr, 0x70 + 4 * i);
		else for (int i = 0; i < n; i++) U8(o, off[i] + 1) = (uint8_t)a[i];
	}

	static uint32_t GasPrims(uint32_t hdr, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint32_t pk = cursor;
		const uint32_t verts = U32(hdr, 4);
		const int32_t h = S16(hdr, 0x3A);
		uint32_t rec = U32(hdr, 0);
		for (uint32_t i = 0; i < U16(hdr, 8); i++, rec += 0x10)
		{
			GteLoadV012(verts + (U16(rec, 0) & 0xFFF) * 8, verts + (U16(rec, 2) & 0xFFF) * 8, verts + (U16(rec, 4) & 0xFFF) * 8);
			GteRTPT();
			U32(pk, 0x14) = U32(rec, 0xC);
			U32(pk, 0) = 0x7000000;
			U32(pk, 4) = U32(hdr, 0x1C) | 0x24000000;
			U32(pk, 0xC) = U32(rec, 8);
			GteReadFLAG(hdr + 0x48);
			if (U32(hdr, 0x48) & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(hdr + 0x3C);
			if (S32(hdr, 0x3C) <= 0) continue;
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteAVSZ3();
			S32(pk, 0x1C) = S32(rec, 4) >> 16;
			GteReadOTZ32(hdr + 0x44);
			int32_t b = S32(hdr, 0x44) >> (shift & 31);
			InsertPrim(ot + (uint32_t)shl32(b, 2), pk);
			if (b > 0) b--;
			const uint32_t o = pk + 0x20;
			U32(o, 8) = U32(hdr, 0x6C) | 0x26000000;
			U16(o, 0x10) = U16(pk, 0xC);
			U16(o, 0x18) = U16(pk, 0x14);
			U32(o, 0x20) = U32(pk, 0x1C);
			U32(o, 0xC) = U32(pk, 8);
			U32(o, 0x14) = U32(pk, 0x10);
			U32(o, 0x1C) = U32(pk, 0x18);
			U32(o, 0) = 0x9000000;
			U16(o, 0x12) = 0x3F54;
			U16(o, 0x1A) = 0xB9;
			static const int32_t UV3[3] = { 0x10, 0x18, 0x20 };
			GasScroll(hdr, o, UV3, 3, h);
			U32(o, 4) = U32(hdr, 0x88);
			U32(o, 0x24) = U32(hdr, 0x8C);
			InsertPrim(ot + (uint32_t)shl32(b, 2), o);
			pk = o + 0x28;
		}
		const uint32_t quads = rec + 2;
		for (uint32_t i = 0; i < U16(hdr, 0xA); i++)
		{
			const uint32_t q = quads + i * 0x14;
			GteLoadV012(verts + (U16(q, -2) & 0xFFF) * 8, verts + (U16(q, 0) & 0xFFF) * 8, verts + (U16(q, 2) & 0xFFF) * 8);
			GteRTPT();
			U32(pk, 0xC) = U32(q, 6);
			U32(pk, 0) = 0x9000000;
			U32(pk, 4) = U32(hdr, 0x1C) | 0x2C000000;
			U32(pk, 0x14) = U32(q, 0xA);
			GteReadFLAG(hdr + 0x48);
			if (U32(hdr, 0x48) & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(hdr + 0x3C);
			if (S32(hdr, 0x3C) <= 0) continue;
			GteReadSXY012(pk + 8, pk + 0x10, pk + 0x18);
			GteLoadV0u(verts + (U16(q, 4) & 0xFFF) * 8);
			GteRTPS();
			U32(pk, 0x1C) = U32(q, 0xE);
			S32(pk, 0x24) = S32(q, 0xE) >> 16;
			GteReadSXY2u(pk + 0x20);
			GteAVSZ4();
			GteReadOTZ32(hdr + 0x44);
			int32_t b = S32(hdr, 0x44) >> (shift & 31);
			InsertPrim(ot + (uint32_t)shl32(b, 2), pk);
			if (b > 0) b--;
			const uint32_t o = pk + 0x28;
			U32(o, 8) = U32(hdr, 0x6C) | 0x2E000000;
			U16(o, 0x10) = U16(pk, 0xC);
			U32(o, 0x28) = U32(pk, 0x24);
			U16(o, 0x18) = U16(pk, 0x14);
			U32(o, 0x20) = U32(pk, 0x1C);
			U32(o, 0xC) = U32(pk, 8);
			U32(o, 0x14) = U32(pk, 0x10);
			U32(o, 0) = 0xB000000;
			U16(o, 0x12) = 0x3F54;
			U32(o, 0x1C) = U32(pk, 0x18);
			U32(o, 0x24) = U32(pk, 0x20);
			U16(o, 0x1A) = 0xB9;
			static const int32_t UV4[4] = { 0x10, 0x18, 0x20, 0x28 };
			GasScroll(hdr, o, UV4, 4, h);
			U32(o, 4) = U32(hdr, 0x88);
			U32(o, 0x2C) = U32(hdr, 0x8C);
			InsertPrim(ot + (uint32_t)shl32(b, 2), o);
			pk = o + 0x30;
		}
		return pk;
	}

	static uint32_t GasObjects(uint32_t model, uint32_t hdr, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t bones = U32(model, 0) + 0x10;
		uint32_t table = U32(model, 4);
		const int32_t count = S32(table, 0);
		table += 4;
		for (int32_t i = 0; i < count; i++)
		{
			uint32_t obj = U32(model, 4) + U32(table, 0);
			table += 4;
			if (!(U32(hdr, 0x20) & (1u << (i & 31)))) continue;
			int32_t groups = S16(obj, 0);
			uint32_t out = U32(hdr, 4);
			obj += 2;
			for (; groups > 0; groups--)
			{
				const int32_t bone = S16(obj, 0);
				obj += 2;
				const Mat4x3 *m = (const Mat4x3 *)(bones + (uint32_t)(bone * 0x30) + 0x10);
				GteSetRotMatrixCtrl(m);
				GteSetTransVectorCtrl(m);
				int32_t nv = S16(obj, 0);
				obj += 2;
				for (; nv > 0; nv--, out += 8)
				{
					GteLoadV0SVec3(obj);
					obj += 6;
					GteMVMVA_RotV0Tr2();
					GteStoreIR123SVec(out);
				}
			}
			obj = (obj + 3) & ~3u;
			U16(hdr, 8) = U16(obj, 0);
			U16(hdr, 0xA) = U16(obj, 2);
			U16(hdr, 0xC) = U16(obj, 4);
			U16(hdr, 0xE) = U16(obj, 6);
			U32(hdr, 0) = obj + 0xC;
			GteSetRotMatrixCtrl(&Camera());
			GteSetTransVectorCtrl(&Camera());
			cursor = GasPrims(hdr, ot, shift, cursor);
		}
		return cursor;
	}

	static void ModelRender(uint32_t hdr, uint32_t entity)
	{
		uint32_t tw = ((uint32_t)U8(hdr, 0x36) & 0xF8) | 0xFFFE2000u;
		tw = (tw << 5) | ((uint32_t)U8(hdr, 0x34) & 0xF8);
		tw = (tw << 5) | (~((uint32_t)U16(hdr, 0x3A) - 1) & 0xF8);
		tw = (tw << 2) | ((~((uint32_t)U16(hdr, 0x38) - 1) >> 3) & 0x1F);
		U32(hdr, 0x88) = tw;
		U32(hdr, 0x8C) = 0xE2000000;
		S32(hdr, 0x84) = S32(hdr, 0x30) % (int32_t)S16(hdr, 0x3A);
		const uint32_t anim = entity + 0x60;
		if (!(U8(entity, 0) & 0x20))
			FrameCursor() = DrawShadow(entity, var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4064, 0x10, FrameCursor());
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(entity + 0x40), (Mat4x3 *)(hdr + 0x90));
		ComputeBonesWorldMatrices((void *)anim, (void *)(entity + 0x40));
		U32(hdr, 0x20) = U32(entity, 0x7C);
		U32(hdr, 0x1C) = U32(entity, 0x28);
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
		FrameCursor() = GasObjects(U32(anim, 4), hdr, RenderOT44(), 2, FrameCursor());
		BuildBoneMatricesFromPose((void *)anim);
		const uint32_t weapon = U32(entity, 0x78);
		if (weapon && !(U8(entity, 1) & 2))
		{
			U32(hdr, 0x20) = 0xFFFFFFFF;
			ComputeBonesWorldMatrices((void *)weapon, (void *)(hdr + 0x90));
			FrameCursor() = RenderGeometry(U32(weapon, 4), hdr, RenderOT44(), 4, FrameCursor());
			BuildBoneMatricesFromPose((void *)weapon);
		}
	}

	static int32_t TintGrey(int32_t c) { return c < 4 ? shl32(c, 5) : (c >= 0x10 ? shl32(0x18 - c, 4) : 0x80); }

	static void TintDraw(int32_t g, int32_t s)
	{
		const uint32_t h = P(FieldAlloc(0xB0));
		U16(h, 0x38) = 0x80;
		U16(h, 0x3A) = 0x80;
		U32(h, 0x6C) = (uint32_t)((((g << 8) | g) << 8) | g);
		U16(h, 0x34) = 0;
		U16(h, 0x36) = 0;
		U32(h, 0x2C) = 0;
		S32(h, 0x30) = s;
		uint8_t *entity = Entity(CasterSlot());
		entity[0] |= 4;
		ModelRender(h, P(entity));
		FieldFree(0xB0);
	}

	static uint32_t __cdecl TintTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		const int16_t c = S16(n, 0xC);
		if (c < 0x18)
		{
			TintDraw(TintGrey(c), shl32(c, 2));
		}
		if (DrawOnly()) return 0;
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		if (S16(n, 0xC) < 0x18) return 0;
		uint16_t &w = *(uint16_t *)Entity(CasterSlot());
		w = (uint16_t)((w & 0xFFFB) | U16(n, 0x1A));
		return TASK_END;
	}

	static int32_t CloudFade(int32_t c) { return c < 0xC ? 0x1000 - mul32(c, 170) : (c >= 0x2A ? mul32(c, 170) - 0x13E4 : 0x800); }

	static void CloudDraw(uint32_t n, int32_t fade)
	{
		const int16_t ang[3] = { S16(n, 0x18), 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = S16(n, 0x10);
		m.t[2] = var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		const int32_t s = S16(n, 0x1C);
		const int32_t sc[3] = { s + (s >> 1), s, s };
		m.t[1] = 0;
		Scale3DMatrix(&m, sc);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		const uint32_t h = P(FieldAlloc(0x58));
		U32(h, 0) = game_addr(MODEL_Cloud);
		S32(h, 0xC) = 0x800;
		U32(h, 8) = 0;
		U32(h, 0x1C) = 0xC3;
		S32(h, 0xC) = fade;
		PacketCursor() = RenderPrimModel((void *)h, RenderOT44(), 4, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl CloudTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		CloudDraw(n, CloudFade(S16(n, 0xC)));
		if (DrawOnly()) return 0;
		S16(n, 0x10) = (int16_t)(S16(n, 0x10) + S16(n, 0x16));
		S16(n, 0x18) = (int16_t)(S16(n, 0x18) + S16(n, 0x1A));
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= 0x36 ? TASK_END : 0;
	}

	static void PartDraw(uint32_t h, uint32_t p, int16_t frame)
	{
		const int16_t size = S16(p, P_SIZE);
		U16(h, 4) = (uint16_t)frame;
		TransformCameraByShadowRotation((const void *)(p + P_POS), (int32_t)(uint16_t)size, -(size >> 4));
		PacketCursor() = InitEffectSequenceFromData((void *)h, RenderOT44(), 2, PacketCursor());
	}

	static int PartsDraw(uint32_t n, uint32_t lo, uint32_t hi, uint32_t seq, bool half, bool moving)
	{
		const uint32_t h = P(FieldAlloc(0xB4));
		int alive = 0;
		U32(h, 0) = seq;
		U16(h, 0x24) = 0;
		for (uint32_t p = lo; p < hi; p += 0x18)
		{
			if (!(U32(p, P_MASK) & (uint32_t)(int32_t)S16(n, 0xE))) continue;
			PartDraw(h, p, half ? (int16_t)(S16(p, P_FRAME) >> 1) : S16(p, P_FRAME));
			if (DrawOnly()) continue;
			S16(p, P_FRAME) = (int16_t)(S16(p, P_FRAME) + 1);
			if (S16(h, 0x28) < 0)
			{
				U32(p, P_MASK) = 0;
				continue;
			}
			if (moving)
			{
				S16(p, P_SIZE) = (int16_t)(S16(p, P_SIZE) + (int16_t)(S16(p, P_SIZE) >> 5));
				for (int k = 0; k < 3; k++) S16(p, P_POS + 2 * k) = (int16_t)(S16(p, P_POS + 2 * k) + S16(p, P_VEL + 2 * k));
				for (int k = 0; k < 3; k++) S16(p, P_VEL + 2 * k) = (int16_t)(S16(p, P_VEL + 2 * k) - (int16_t)(S16(p, P_VEL + 2 * k) >> 5));
			}
			alive++;
		}
		FieldFree(0xB4);
		return alive;
	}

	static void PartsSpawn(uint32_t n, uint32_t lo, uint32_t hi, bool puff)
	{
		const Mat4x3 *cm = (const Mat4x3 *)(Entity(CasterSlot()) + 0x40);
		const int32_t row = S16(n, 0x18);
		int16_t from[4], to[4];
		from[3] = SpawnPad();
		MatVec(cm, (const int16_t *)(game_addr(ROWS) + (uint32_t)row * 8), from);
		if (puff)
		{
			const int16_t c = S16(n, 0xC);
			ff8fx::copy<(8)>(to, (const void *)(game_addr(ROWS) + (uint32_t)row * 8 + 8));
			to[0] = (int16_t)(to[0] + (int16_t)(to[0] / 32 * c));
			to[2] = (int16_t)(to[2] + (int16_t)(to[2] / 10 * c));
			MatVec(cm, to, to);
		}
		else MatVec(cm, (const int16_t *)(game_addr(ROWS) + (uint32_t)row * 8 + 8), to);
		for (int k = 0; k < 3; k++)
		{
			from[k] = (int16_t)(from[k] + S16(n, 0x10 + 2 * k));
			to[k] = (int16_t)(to[k] + S16(n, 0x10 + 2 * k));
		}
		int32_t d[3] = { (int32_t)to[0] - from[0], (int32_t)to[1] - from[1], (int32_t)to[2] - from[2] };
		NormalizeVector(d, d);
		for (int j = 0; j < 2; j++)
		{
			const uint32_t p = FreeParticle(lo, hi);
			if (!p) break;
			U32(p, P_MASK) = (uint32_t)(int32_t)S16(n, 0xE);
			S16(p, P_FRAME) = 0;
			S16(p, P_SIZE) = (int16_t)((CrtRand() & 0x3FF) + 0x680);
			ff8fx::copy<(8)>((void *)(p + P_POS), from);
			const int32_t v = CrtRand() % 0x5F + 0x28;
			if (puff)
			{
				S16(p, P_VEL) = (int16_t)(CrtRand() % 0x28 + (mul32(v, d[0]) >> 12) - 0x14);
				S16(p, P_VEL + 2) = (int16_t)(CrtRand() % 0x28 + (mul32(v, d[1]) >> 12) - 0x14);
				const int32_t r = CrtRand() % 0x28;
				S16(p, P_VEL + 4) = (int16_t)(r + (mul32(v, d[2]) >> 12) - 0x14);
			}
			else
			{
				S16(p, P_VEL) = (int16_t)(CrtRand() % 0x50 + (mul32(v, d[0]) >> 12) - 0x28);
				S16(p, P_VEL + 2) = (int16_t)(CrtRand() % 0x3C + (mul32(v, d[1]) >> 12) - 0x1E);
				const int32_t r = CrtRand() % 0x50;
				S16(p, P_VEL + 4) = (int16_t)(r + (mul32(v, d[2]) >> 12) - 0x28);
			}
		}
	}

	static uint32_t PuffTaskImpl(uint32_t n, bool puff)
	{
		const int alive = puff ? PartsDraw(n, game_addr(PARTS_B), game_addr(PARTS_END), game_addr(SEQ_Puff), true, true) : PartsDraw(n, game_addr(PARTS_A), game_addr(PARTS_B), game_addr(SEQ_Puff), false, true);
		if (DrawOnly()) return 0;
		const int16_t c = S16(n, 0xC);
		if (c >= 0 && c <= 0x10)
		{
			if (puff) PartsSpawn(n, game_addr(PARTS_B), game_addr(PARTS_END), true);
			else PartsSpawn(n, game_addr(PARTS_A), game_addr(PARTS_B), false);
		}
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= 4 && !alive ? TASK_END : 0;
	}

	static uint32_t __cdecl PuffTask(TaskNode *tn) { return PuffTaskImpl(P(tn), true); }
	static uint32_t __cdecl WispTask(TaskNode *tn) { return PuffTaskImpl(P(tn), false); }

	static uint32_t __cdecl EmitterTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		const int alive = PartsDraw(n, game_addr(PARTS_B), game_addr(PARTS_END), game_addr(SEQ_Emitter), false, false);
		if (DrawOnly()) return 0;
		const int16_t c = S16(n, 0xC);
		if (c >= 0 && c <= 0xD)
		{
			const uint32_t p = FreeParticle(game_addr(PARTS_B), game_addr(PARTS_END));
			if (p)
			{
				U32(p, P_MASK) = (uint32_t)(int32_t)S16(n, 0xE);
				S16(p, P_FRAME) = 0;
				S16(p, P_SIZE) = (int16_t)(CrtRand() % 0x900 + 0x300);
				U32(p, P_POS) = U32(n, 0x10);
				U32(p, P_POS + 4) = U32(n, 0x14);
				S16(p, P_POS) = (int16_t)(S16(p, P_POS) + CrtRand() % 0x3E8 - 0x1F4);
				S16(p, P_POS + 2) = (int16_t)(S16(p, P_POS + 2) + CrtRand() % 0x3E8 - 0x1F4);
				S16(p, P_POS + 4) = (int16_t)(S16(p, P_POS + 4) + CrtRand() % 0x3E8 - 0x1F4);
			}
		}
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return S16(n, 0xC) >= 4 && !alive ? TASK_END : 0;
	}

	static uint32_t NewTask(uint32_t f)
	{
		const uint32_t t = (uint32_t)AddTaskToQueue(EffectQueue(), f);
		S16(t, 0xC) = 0;
		return t;
	}

	static uint32_t __cdecl DirectorTask(TaskNode *tn)
	{
		if (DrawOnly()) return 0;
		const uint32_t n = P(tn);
		if (S16(n, 0xC) == 1)
		{
			const uint32_t t = NewTask(game_addr(ORIG_Tint));
			U16(t, 0x1A) = (uint16_t)(Entity(CasterSlot())[0] & 4);
			GetEffectSpawnPosition(Entity(CasterSlot()), 0xF1, 0, (int16_t *)(n + 0x10));
			static const int16_t MASK[4] = { 1, 2, 1, 2 }, ROW[4] = { 0, 2, 4, 6 };
			for (int k = 0; k < 4; k++)
			{
				const uint32_t p = NewTask(k < 2 ? game_addr(ORIG_Puff) : game_addr(ORIG_Wisp));
				U32(p, 0x10) = U32(n, 0x10);
				S16(p, 0xE) = MASK[k];
				U32(p, 0x14) = U32(n, 0x14);
				S16(p, 0x18) = ROW[k];
			}
		}
		if (S16(n, 0xC) == 8)
		{
			uint32_t t = NewTask(game_addr(ORIG_Cloud));
			S16(t, 0x10) = -400;
			S16(t, 0x16) = 0xA;
			S16(t, 0x1C) = 0xB00;
			S16(t, 0x18) = (int16_t)(CrtRand() & 0xFFF);
			S16(t, 0x1A) = 0x1E;
			t = NewTask(game_addr(ORIG_Cloud));
			S16(t, 0x10) = -400;
			S16(t, 0x16) = 6;
			S16(t, 0x1C) = 0xB00;
			S16(t, 0x18) = (int16_t)(CrtRand() & 0xFFF);
			S16(t, 0x1A) = 0xF;
		}
		if (S16(n, 0xC) == 0x1B)
		{
			for (int j = 0; j < Ctx()->actions->target_count; j++)
			{
				const uint8_t slot = Ctx()->actions->targets[j * TARGET_STRIDE];
				const uint32_t e = NewTask(game_addr(ORIG_Emitter));
				S16(e, 0xE) = (int16_t)(4 << (j & 31));
				GetEffectSpawnPosition(Entity(slot), 0xF1, 0, (int16_t *)(e + 0x10));
			}
		}
		if (S16(n, 0xC) == 0x1E) ApplyActionResultToTargets(Ctx()->actions->targets, Ctx()->actions->target_count);
		const int16_t c = S16(n, 0xC);
		if (c <= 0x10) SetScreenFlash((uint32_t)shl32(c, 7), 0);
		else if (c >= 0x2A) SetScreenFlash((uint32_t)shl32(0x32 - c, 8), 0);
		if (S16(n, 0xC) == 1) BdPlaySE((const void *)game_addr(SOUND_Gas), 0, 0x80);
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		if (S16(n, 0xC) <= 0x32) return 0;
		SetScreenFlash(0, 0);
		return TASK_END;
	}

	static uint32_t __cdecl RootTask(TaskNode *tn)
	{
		const uint32_t n = P(tn);
		PacketCursor() = (U8(n, 0xC) & 1) ? TexBase() + 0xC000 : TexBase();
		const int running = ExecuteTaskQueue(EffectQueue());
		S16(n, 0xC) = (int16_t)(S16(n, 0xC) + 1);
		return running ? 0 : TASK_END;
	}

	void register_mag171_poison_gas_ports()
	{
		register_port(ORIG_Root, (void *)RootTask, "171 RootTask", 171);
		register_port(ORIG_Director, (void *)DirectorTask, "171 DirectorTask", 171);
		register_port(ORIG_Tint, (void *)TintTask, "171 TintTask", 171);
		register_port(ORIG_Puff, (void *)PuffTask, "171 PuffTask", 171);
		register_port(ORIG_Wisp, (void *)WispTask, "171 WispTask", 171);
		register_port(ORIG_Cloud, (void *)CloudTask, "171 CloudTask", 171);
		register_port(ORIG_Emitter, (void *)EmitterTask, "171 EmitterTask", 171);
	}
}

	void register_mag171_poison_gas() { poisongas171::register_mag171_poison_gas_ports(); }
}

