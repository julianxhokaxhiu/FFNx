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

// Effect 193: Dark Shot

#include "mag_common.h"

namespace ff8fx
{
namespace dark193
{
	using namespace eng;
	using namespace magc;

	template<typename T> inline T &AT(uint32_t p, int32_t o) { return *(T *)(p + o); }
	inline uint16_t &U16(uint32_t p, int32_t o) { return AT<uint16_t>(p, o); }
	inline int32_t &S32(uint32_t p, int32_t o) { return AT<int32_t>(p, o); }
	inline uint32_t &U32(uint32_t p, int32_t o) { return AT<uint32_t>(p, o); }
	inline uint8_t &U8(uint32_t p, int32_t o) { return AT<uint8_t>(p, o); }
	inline int16_t S16v(uint32_t p, int32_t o) { return AT<int16_t>(p, o); }

	inline int32_t &EndTick() { return var<int32_t>(game_addr(game::MAG_193_DARKSHOT_AfterimageEndTick)); }
	inline int32_t &ModuleTick() { return var<int32_t>(game_addr(game::MAG_193_DARKSHOT_ModuleTick)); }
	inline uint32_t &Flags() { return var<uint32_t>(game_addr(game::MAG_193_DARKSHOT_Flags)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_193_DARKSHOT_CastContext)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::MAG_193_DARKSHOT_CasterSlot)); }
	inline int32_t &ImpactCount() { return var<int32_t>(game_addr(game::MAG_193_DARKSHOT_ImpactCount)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_193_DARKSHOT_TextureBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_193_DARKSHOT_PacketCursor)); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::MAG_193_DARKSHOT_RootQueue); }
	inline TaskQueue *QShots() { return (TaskQueue *)game_addr(game::MAG_193_DARKSHOT_ShotQueue); }
	inline TaskQueue *QAux() { return (TaskQueue *)game_addr(game::MAG_193_DARKSHOT_AuxQueue); }
	static constexpr game::Id PARTS_A = game::MAG_193_DARKSHOT_ParticlesEven, PARTS_B = game::MAG_193_DARKSHOT_ParticlesOdd;
	static const int32_t PARTS_N = 150;
	static constexpr game::Id SHOTS = game::MAG_193_DARKSHOT_ShotRecords;
	static constexpr game::Id BIND_OUT = game::MAG_193_DARKSHOT_AnimBindBlock;
	static constexpr game::Id BONES_NOW = game::MAG_193_DARKSHOT_WeaponBonesNow;
	static constexpr game::Id GHOSTS = game::MAG_193_DARKSHOT_GhostSlots, GHOSTS_END = game::MAG_193_DARKSHOT_ShotCount;
	static const uint32_t ARENA_ODD = 0xC000;
	static const uint32_t FADE_COLOUR = 0x204820;

	static constexpr game::Id ORIG_RootTask = game::MAG_193_DARKSHOT_RootTask;
	static constexpr game::Id ORIG_RestoreTask = game::MAG_193_DARKSHOT_TextureRestoreTask;
	static constexpr game::Id ORIG_StartTask = game::MAG_193_DARKSHOT_StartTask;
	static constexpr game::Id ORIG_ShotTask = game::MAG_193_DARKSHOT_ShotTask;
	static constexpr game::Id ORIG_EndTask = game::MAG_193_DARKSHOT_EndTask;
	static constexpr game::Id ORIG_FadeOutTask = game::MAG_193_DARKSHOT_FadeOutTask;
	static constexpr game::Id ORIG_FadeInTask = game::MAG_193_DARKSHOT_FadeInTask;
	static constexpr game::Id ORIG_AfterimageTask = game::MAG_193_DARKSHOT_AfterimageTask;
	static constexpr game::Id ORIG_SmokeTask = game::MAG_193_DARKSHOT_SmokeTask;
	static constexpr game::Id ORIG_Flash2Task = game::MAG_193_DARKSHOT_Flash2Task;
	static constexpr game::Id ORIG_Smoke2Task = game::MAG_193_DARKSHOT_Smoke2Task;
	static constexpr game::Id ORIG_BlastTask = game::MAG_193_DARKSHOT_BlastTask;
	static constexpr game::Id ORIG_GroundSmokeTask = game::MAG_193_DARKSHOT_GroundSmokeTask;
	static constexpr game::Id ORIG_ImpactTask = game::MAG_193_DARKSHOT_ImpactTask;
	static constexpr game::Id ORIG_TargetSparkTask = game::MAG_193_DARKSHOT_TargetSparkTask;
	static constexpr game::Id ORIG_ModelTask = game::MAG_193_DARKSHOT_ModelTask;

	static constexpr game::Id ANIM_DATA = game::MAG_193_DARKSHOT_AnimSetComData;
	static constexpr game::Id MODEL_Flash2 = game::MAG_193_DARKSHOT_ModelFlash2, MODEL_Blast = game::MAG_193_DARKSHOT_ModelBlast, MODEL_Model = game::MAG_193_DARKSHOT_ModelTarget;
	static constexpr game::Id SEQ_Smoke = game::MAG_193_DARKSHOT_SeqSmoke, SEQ_Smoke2 = game::MAG_193_DARKSHOT_SeqSmoke2, SEQ_GroundSmoke = game::MAG_193_DARKSHOT_SeqGroundSmoke, SEQ_Spark = game::MAG_193_DARKSHOT_SeqSpark, SEQ_TargetSpark = game::MAG_193_DARKSHOT_SeqTargetSpark;
	static const void *SOUND_Gun() { return (const void *)game_addr(game::MAG_193_DARKSHOT_SoundGun); }
	static const int32_t *MUZZLE_REF() { return (const int32_t *)game_addr(game::MAG_193_DARKSHOT_MuzzleRefDir); }
	static const uint32_t *GHOST_COLOURS() { return (const uint32_t *)game_addr(game::MAG_193_DARKSHOT_GhostColours); }

	inline uint32_t BindExternalAnimSet(uint8_t *entity, uint32_t out, uint32_t data) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline uint32_t QueueChainTransformation(uint8_t *entity, uint32_t id) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t angle, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, angle, out); }
	inline int32_t Normalize(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline void MatVec(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void GteMatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline uint32_t RenderGeometry(uint32_t model, void *ctx, uint32_t ot, int32_t a, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(model, ctx, ot, a, cursor); }
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteSetBackColorFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
	inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
	inline void GteSetTransFromVec32(const int32_t *v) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_SetTransFromVec32))(v); }
	inline void GteLoadV012u(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_LoadV012))(a, b, c); }
	inline void GteLoadV0u(uint32_t v) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadV0))(v); }
	inline void GteRTPTx() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteNCLIPx() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0u(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadFLAGu(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteReadSXY012u(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteReadSXY2u(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadSXY2))(dst); }
	inline void GteAVSZ3x() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4x() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteReadOTZ32(uint32_t dst) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteSetFarColor(uint32_t r, uint32_t g, uint32_t b) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadRGBCu(uint32_t c) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCSx() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteDPCTx() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCT))(); }
	inline void GteStoreRGB2u(uint32_t c) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::GTE_StoreRGB2))(c); }
	inline void GteLoadRGB012u(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_LoadRGB012))(a, b, c); }
	inline void GteStoreRGB012u(uint32_t a, uint32_t b, uint32_t c) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_StoreRGB012))(a, b, c); }
	inline uint32_t &FramePackets() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		int16_t done;
	};
	struct Node
	{
		TaskNode hdr;
		int16_t counter;
		int16_t f0E;
		int16_t pos[4];
		int16_t f18;
		int16_t f1A;
		int16_t f1C;
		int16_t f1E;
		uint8_t pad20[4];
	};
	struct ShotRec
	{
		int16_t pos[4];
		Mat4x3 m;
	};
	struct Particle
	{
		int32_t id;
		int16_t frame;
		int16_t size;
		int16_t pos[4];
		int16_t vel[3];
		int16_t pad;
	};
	struct Ghost
	{
		int32_t used;
		uint8_t bones[0x80];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node) == 0x24, "Dark Shot nodes");
	static_assert(sizeof(ShotRec) == 0x28 && sizeof(Particle) == 0x18 && sizeof(Ghost) == 0x84, "Dark Shot records");

	inline ShotRec *Shot(int32_t i) { return (ShotRec *)(game_addr(SHOTS) + 0x28 * i); }
	inline Ghost *GhostSlot(int32_t i) { return (Ghost *)(game_addr(GHOSTS) + 0x84 * i); }
	inline uint8_t *Caster() { return Entity(CasterSlot()); }
	inline Node *AddTask(TaskQueue *q, uint32_t fn) { return (Node *)AddTaskToQueue(q, fn); }
	inline Particle *PartArray(int16_t number) { return (Particle *)((number & 1) ? game_addr(PARTS_B) : game_addr(PARTS_A)); }
}
}

namespace ff8fx
{
namespace dark193
{
	static void BoneFrame(uint8_t *anim_header, int32_t bone, int32_t *dir, int16_t *pos)
	{
		const uint8_t *sk = *(uint8_t **)*(uint8_t **)(anim_header + 4);
		if (bone >= 0xF0) bone = sk[bone - 0xEC];
		const uint8_t *b = sk + 0x10 + bone * 0x30;
		dir[1] = 0;
		dir[0] = 0;
		dir[2] = -0x1000;
		TransformVectorBy3x3Matrix((const Mat4x3 *)(b + 0x10), dir, dir);
		Normalize(dir, dir);
		pos[0] = *(const int16_t *)(b + 0x24);
		pos[1] = *(const int16_t *)(b + 0x28);
		pos[2] = *(const int16_t *)(b + 0x2C);
	}

	static void WeaponBone(uint8_t *entity, int32_t bone, int32_t *dir, int16_t *pos)
	{
		uint8_t *w = *(uint8_t **)(entity + 0x78);
		if (!w) return;
		BoneFrame(w, bone, dir, pos);
		Mat4x3 m;
		ComposeAffineTransform((const Mat4x3 *)(entity + 0x40), (const Mat4x3 *)(w + 0x24), &m);
		TransformVectorBy3x3Matrix(&m, dir, dir);
		Normalize(dir, dir);
		MatVec(&m, pos, pos);
		pos[0] = (int16_t)(pos[0] + (int16_t)m.t[0]);
		pos[1] = (int16_t)(pos[1] + (int16_t)m.t[1]);
		pos[2] = (int16_t)(pos[2] + (int16_t)m.t[2]);
	}

	static int32_t ClampedSize(const uint8_t *entity)
	{
		const int32_t s = *(const int16_t *)(entity + 0x26);
		if (s > 0x1100) return 0x1100;
		if (s < 0x600) return 0x600;
		return s;
	}

	static void BoneSnapshot(uint8_t *w, uint8_t *dst)
	{
		const uint8_t *sk = *(const uint8_t **)*(uint8_t **)(w + 4);
		const int32_t n = sk[0];
		const uint8_t *src = sk + 0x20;
		for (int32_t k = 0; k < n; k++, src += 0x30, dst += 0x20) ff8fx::copy<(0x20)>(dst, src);
	}
	static void BoneRestore(uint8_t *w, const uint8_t *src)
	{
		uint8_t *sk = *(uint8_t **)*(uint8_t **)(w + 4);
		uint8_t *dst = sk + 0x20;
		for (int32_t k = 0; k < (int32_t)sk[0]; k++, src += 0x20, dst += 0x30) ff8fx::copy<(0x20)>(dst, src);
	}

	static void GhostDraw(uint8_t *entity)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x4C);
		uint8_t *w = *(uint8_t **)(entity + 0x78);
		if (w && !(entity[1] & 2))
		{
			Mat4x3 *m = (Mat4x3 *)h;
			ComposeAffineTransform(&Camera(), (const Mat4x3 *)(entity + 0x40), m);
			*(uint16_t *)(h + 0x34) = 0;
			*(uint16_t *)(h + 0x36) = 0;
			*(uint32_t *)(h + 0x24) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
			*(uint32_t *)(h + 0x3C) = *(const uint32_t *)(entity + 0x28);
			*(uint16_t *)(h + 0x38) = 0x140;
			*(uint16_t *)(h + 0x3A) = 0xD8;
			*(uint32_t *)(h + 0x30) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
			*(uint32_t *)(h + 0x40) = 0xFFFFFFFF;
			const uint8_t c = entity[7];
			h[0x4A] = c;
			h[0x49] = c;
			h[0x48] = c;
			ComposeAffineTransform(m, (const Mat4x3 *)(w + 0x24), m);
			ComputeBonesWorldMatrices(w, m);
			FramePackets() = RenderGeometry(*(const uint32_t *)(w + 4), h + 0x20, RenderOT(), 4, FramePackets());
		}
		FieldFree(0x4C);
	}

	static uint32_t ClipX(uint32_t xy, uint32_t bit) { const int16_t x = S16v(xy, 0); return (x < 0 || x > 0xA00) ? bit : 0; }
	static uint32_t ClipY(uint32_t xy, uint32_t bit) { const int16_t y = S16v(xy, 2); return (y < 0 || y > 0x6C0) ? bit : 0; }

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
			GteLoadV012u(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPTx();
			U32(pk, 0) = 0x8000000;
			U32(pk, 4) = U32(rec, 0);
			SemiTrans(h, pk, 1, 4);
			const uint32_t d = U32(h, 0x18);
			U32(pk, 0xC) = U32(rec, 0xC) + d;
			U32(pk, 0x14) = U32(rec, 0x10) + d;
			U32(pk, 0x1C) = (U32(rec, 8) >> 16) + d;
			GteReadFLAGu(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIPx();
			PageWords(h, pk, 0x16);
			GteReadMAC0u(h + 0x24);
			const int32_t nc = S32(h, 0x24);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x1C) & 0x10)) continue;
			GteReadSXY012u(pk + 8, pk + 0x10, pk + 0x18);
			GteAVSZ3x();
			uint32_t m = ClipX(pk + 8, 1) | ClipX(pk + 0x10, 2) | ClipX(pk + 0x18, 4);
			m |= ClipY(pk + 8, 0x10) | ClipY(pk + 0x10, 0x20) | ClipY(pk + 0x18, 0x40);
			if ((m & 7) == 7 || (m & 0x70) == 0x70) continue;
			GteReadOTZ32(h + 0x2C);
			if (U8(h, 0x1C) & 0x40)
			{
				GteLoadRGBCu(pk + 4);
				GteSetIR0(S32(h, 0xC));
				GteDPCSx();
				GteStoreRGB2u(pk + 4);
			}
			U32(pk, 0x20) = U32(h, 0x58);
			InsertPrimAutoDepth(ot + (uint32_t)shl32(S32(h, 0x2C) >> (shift & 31), 2), (void *)pk);
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
			GteLoadV012u(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPTx();
			U32(pk, 0) = 0xA000000;
			U32(pk, 4) = U32(rec, 0);
			SemiTrans(h, pk, 1, 4);
			const uint32_t d = U32(h, 0x18);
			U32(pk, 0xC) = U32(rec, 0xC) + d;
			const uint32_t uv3 = (d << 16) + d + U32(rec, 0x14);
			U32(pk, 0x1C) = uv3;
			U32(pk, 0x14) = U32(rec, 0x10) + d;
			U32(pk, 0x24) = uv3 >> 16;
			GteReadFLAGu(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIPx();
			PageWords(h, pk, 0x16);
			GteReadMAC0u(h + 0x24);
			const int32_t nc = S32(h, 0x24);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x1C) & 0x10)) continue;
			GteReadSXY012u(pk + 8, pk + 0x10, pk + 0x18);
			GteLoadV0u(verts + U16(rec, 0xA) * 4);
			GteRTPS();
			uint32_t m = ClipX(pk + 8, 1) | ClipX(pk + 0x10, 2) | ClipX(pk + 0x18, 4);
			m |= ClipY(pk + 8, 0x10) | ClipY(pk + 0x10, 0x20) | ClipY(pk + 0x18, 0x40);
			GteReadSXY2u(pk + 0x20);
			GteAVSZ4x();
			m |= ClipX(pk + 0x20, 8) | ClipY(pk + 0x20, 0x80);
			if ((m & 0xF) == 0xF || (m & 0xF0) == 0xF0) continue;
			GteReadOTZ32(h + 0x2C);
			if (U8(h, 0x1C) & 0x40)
			{
				GteLoadRGBCu(pk + 4);
				GteSetIR0(S32(h, 0xC));
				GteDPCSx();
				GteStoreRGB2u(pk + 4);
			}
			U32(pk, 0x28) = U32(h, 0x58);
			InsertPrimAutoDepth(ot + (uint32_t)shl32(S32(h, 0x2C) >> (shift & 31), 2), (void *)pk);
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
			GteLoadV012u(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPTx();
			U32(pk, 0) = 0xA000000;
			U32(pk, 4) = U32(rec, 0);
			SemiTrans(h, pk, 2, 8);
			const uint32_t d = U32(h, 0x18);
			U32(pk, 0xC) = U32(rec, 0xC) + d;
			U32(pk, 0x18) = U32(rec, 0x10) + d;
			U32(pk, 0x24) = (U32(rec, 8) >> 16) + d;
			GteReadFLAGu(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIPx();
			PageWords(h, pk, 0x1A);
			GteReadMAC0u(h + 0x24);
			const int32_t nc = S32(h, 0x24);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x1C) & 0x20)) continue;
			GteReadSXY012u(pk + 8, pk + 0x14, pk + 0x20);
			GteAVSZ3x();
			uint32_t m = ClipX(pk + 8, 1) | ClipX(pk + 0x14, 2) | ClipX(pk + 0x20, 4);
			m |= ClipY(pk + 8, 0x10) | ClipY(pk + 0x14, 0x20) | ClipY(pk + 0x20, 0x40);
			if ((m & 7) == 7 || (m & 0x70) == 0x70) continue;
			GteReadOTZ32(h + 0x2C);
			if (U8(h, 0x1C) & 0x80)
			{
				GteLoadRGB012u(rec + 0x14, rec + 0x18, pk + 4);
				GteSetIR0(S32(h, 0xC));
				GteDPCTx();
				GteStoreRGB012u(pk + 0x10, pk + 0x1C, pk + 4);
			}
			else
			{
				U32(pk, 0x10) = U32(rec, 0x14);
				U32(pk, 0x1C) = U32(rec, 0x18);
			}
			U32(pk, 0x28) = U32(h, 0x58);
			InsertPrimAutoDepth(ot + (uint32_t)shl32(S32(h, 0x2C) >> (shift & 31), 2), (void *)pk);
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
			GteLoadV012u(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPTx();
			U32(pk, 0) = 0xD000000;
			U32(pk, 4) = U32(rec, 0);
			SemiTrans(h, pk, 2, 8);
			const uint32_t d = U32(h, 0x18);
			U32(pk, 0xC) = U32(rec, 0xC) + d;
			const uint32_t uv3 = (d << 16) + d + U32(rec, 0x14);
			U32(pk, 0x24) = uv3;
			U32(pk, 0x18) = U32(rec, 0x10) + d;
			U32(pk, 0x30) = uv3 >> 16;
			GteReadFLAGu(h + 0x30);
			if (U32(h, 0x30) & 0x60000) continue;
			GteNCLIPx();
			PageWords(h, pk, 0x1A);
			GteReadMAC0u(h + 0x24);
			const int32_t nc = S32(h, 0x24);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x1C) & 0x20)) continue;
			GteReadSXY012u(pk + 8, pk + 0x14, pk + 0x20);
			GteLoadV0u(verts + U16(rec, 0xA) * 4);
			GteRTPS();
			uint32_t m = ClipX(pk + 8, 1) | ClipX(pk + 0x14, 2) | ClipX(pk + 0x20, 4);
			m |= ClipY(pk + 8, 0x10) | ClipY(pk + 0x14, 0x20) | ClipY(pk + 0x20, 0x40);
			GteReadSXY2u(pk + 0x2C);
			GteAVSZ4x();
			m |= ClipX(pk + 0x2C, 8) | ClipY(pk + 0x2C, 0x80);
			if ((m & 0xF) == 0xF || (m & 0xF0) == 0xF0) continue;
			GteReadOTZ32(h + 0x2C);
			if (U8(h, 0x1C) & 0x80)
			{
				GteLoadRGB012u(rec + 0x18, rec + 0x1C, rec + 0x20);
				GteSetIR0(S32(h, 0xC));
				GteDPCTx();
				GteStoreRGB012u(pk + 0x10, pk + 0x1C, pk + 0x28);
				GteLoadRGBCu(pk + 4);
				GteDPCSx();
				GteStoreRGB2u(pk + 4);
			}
			else
			{
				U32(pk, 0x10) = U32(rec, 0x18);
				U32(pk, 0x1C) = U32(rec, 0x1C);
				U32(pk, 0x28) = U32(rec, 0x20);
			}
			U32(pk, 0x34) = U32(h, 0x58);
			InsertPrimAutoDepth(ot + (uint32_t)shl32(S32(h, 0x2C) >> (shift & 31), 2), (void *)pk);
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

	static void ModelDraw(uint32_t model, bool faded, int32_t fade)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = model;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (faded)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void MuzzleModel(const Node *n, int16_t roll, uint32_t model, bool faded, int32_t fade)
	{
		const ShotRec *s = Shot(n->f0E);
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = roll;
		ang[3] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		GteMatrixMultiply(&s->m, &m);
		m.t[0] = s->pos[0];
		m.t[1] = s->pos[1];
		const int32_t sc = n->f1C;
		const int32_t v[3] = { sc, sc, sc };
		m.t[2] = s->pos[2];
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		ModelDraw(model, faded, fade);
	}
	static int32_t Flash2Fade(int32_t c) { return mul32(c, 0x555) - 0x555; }
	static int32_t BlastFade(int32_t c) { return mul32(c, 0x2AA) - 0x2AA; }
	static int32_t ModelFade(int32_t c) { return (int32_t)((uint32_t)(c - 4) << 9); }

	static void TargetModelDraw(const Node *n, bool faded, int32_t fade)
	{
		TransformCameraByShadowRotation(n->pos, n->f1C, n->pos[3]);
		uint8_t *h = (uint8_t *)FieldAlloc(0x5C);
		*(uint32_t *)h = game_addr(MODEL_Model);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (faded)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = PrimRender((uint32_t)h, RenderOT(), 2, PacketCursor());
		FieldFree(0x5C);
	}

	static int16_t SmokeDraw(const Node *n, uint32_t seq)
	{
		TransformCameraByShadowRotation(Shot(n->f0E)->pos, n->f1C, -((int32_t)n->f1C >> 3));
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = seq;
		*(int16_t *)(h + 4) = n->counter;
		*(int16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
		return *(const int16_t *)(h + 0x28);
	}

	static int16_t GroundSmokeDraw(const Node *n)
	{
		const ShotRec *s = Shot(n->f0E);
		int16_t ang[4];
		ang[0] = 0x400;
		ang[1] = 0;
		ang[2] = 0;
		ang[3] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = s->pos[0];
		m.t[1] = 0;
		const int32_t sc = n->f1C;
		const int32_t v[3] = { sc, sc, sc };
		m.t[2] = s->pos[2];
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = game_addr(SEQ_GroundSmoke);
		*(int16_t *)(h + 4) = n->counter;
		*(int16_t *)(h + 0x24) = 8;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
		return *(const int16_t *)(h + 0x28);
	}

	static void ImpactFlashDraw(const Node *n, uint8_t *h)
	{
		*(int16_t *)(h + 4) = n->counter;
		*(uint32_t *)h = game_addr(SEQ_Smoke2);
		*(int16_t *)(h + 0x24) = 8;
		TransformCameraByShadowRotation(n->pos, n->f1C, -(int32_t)n->pos[3]);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}
	static void SparkHeader(uint8_t *h)
	{
		*(int16_t *)(h + 0x24) = 8;
		*(uint32_t *)h = game_addr(SEQ_Spark);
	}

	static void SparkDraw(const Particle *e, int16_t depth, uint8_t *h)
	{
		*(int16_t *)(h + 4) = e->frame;
		TransformCameraByShadowRotation(e->pos, e->size, -(int32_t)depth);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}
	static void SparkMove(Particle *e)
	{
		e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
		e->pos[1] = (int16_t)(e->pos[1] + e->vel[1]);
		e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
		e->vel[0] = (int16_t)(e->vel[0] - (int16_t)(e->vel[0] >> 2));
		e->vel[1] = (int16_t)(e->vel[1] - (int16_t)(e->vel[1] >> 2));
		e->vel[2] = (int16_t)(e->vel[2] - (int16_t)(e->vel[2] >> 2));
	}

	static void TargetSparkBegin(uint8_t *s, uint8_t *h)
	{
		*(int16_t *)(h + 0x24) = 0;
		ff8fx::copy<(0x20)>(s + 8, &Camera());
		*(uint32_t *)h = game_addr(SEQ_TargetSpark);
		GteSetLightMatrix(s + 8);
		GteSetBackColorFromTrans(s + 8);
	}
	static void TargetSparkDraw(const Particle *e, int32_t pull, uint8_t *s, uint8_t *h)
	{
		GteLoadV0(e->pos);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(e->size);
		*(int16_t *)(h + 4) = e->frame;
		int32_t *v = (int32_t *)(s + 0x38), *d = (int32_t *)(s + 0x28);
		GteReadMAC123(v);
		Normalize(v, d);
		const int32_t k = -pull;
		v[0] += mul32(d[0], k) >> 12;
		v[1] += mul32(d[1], k) >> 12;
		v[2] += mul32(d[2], k) >> 12;
		GteSetTransFromVec32(v);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}

	static uint32_t SmokeStep(Node *n, uint32_t orig, uint32_t seq)
	{
		const int16_t next = SmokeDraw(n, seq);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return next < 0 ? TASK_END : 0;
	}
	static uint32_t __cdecl SmokeTask(TaskNode *n_) { return SmokeStep((Node *)n_, game_addr(ORIG_SmokeTask), game_addr(SEQ_Smoke)); }
	static uint32_t __cdecl Smoke2Task(TaskNode *n_) { return SmokeStep((Node *)n_, game_addr(ORIG_Smoke2Task), game_addr(SEQ_Smoke2)); }

	static uint32_t __cdecl Flash2Task(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		MuzzleModel(n, n->f18, game_addr(MODEL_Flash2), n->counter >= 1, Flash2Fade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 3));
		n->counter++;
		return n->counter >= 4 ? TASK_END : 0;
	}

	static uint32_t __cdecl BlastTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		MuzzleModel(n, 0, game_addr(MODEL_Blast), n->counter >= 1, BlastFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 4));
		n->counter++;
		return n->counter >= 7 ? TASK_END : 0;
	}

	static uint32_t __cdecl GroundSmokeTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		const int16_t next = GroundSmokeDraw(n);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return next < 0 ? TASK_END : 0;
	}

	static uint32_t __cdecl ImpactTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		Particle *arr = PartArray(n->f0E);
		*(volatile uint32_t *)&n_ = (uint32_t)arr;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		if (n->counter < 7) ImpactFlashDraw(n, h);
		SparkHeader(h);
		int32_t alive = 0;
		const int32_t id = n->f0E + 1;
		for (Particle *e = arr; e < arr + PARTS_N; e++)
		{
			if (e->id != id) continue;
			SparkDraw(e, n->pos[3], h);
			if (UpdateFlags() & 0x201) continue;
			e->frame++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				e->id = 0;
				continue;
			}
			SparkMove(e);
			alive++;
		}
		FieldFree(0xB4);
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter == 0)
		{
			for (int32_t k = 0; k < 0x10; k++)
			{
				int32_t i = 0;
				while (i < PARTS_N && arr[i].id != 0) i++;
				if (i >= PARTS_N) break;
				Particle *e = &arr[i];
				e->id = id;
				e->frame = 0;
				e->size = (int16_t)(CrtRand() % 0x480 + 0x100);
				*(uint32_t *)&e->pos[0] = *(const uint32_t *)&n->pos[0];
				*(uint32_t *)&e->pos[2] = *(const uint32_t *)&n->pos[2];
				e->pos[0] = (int16_t)(e->pos[0] + (CrtRand() % 0x140 - 0xA0));
				e->pos[1] = (int16_t)(e->pos[1] + (CrtRand() % 0x140 - 0xA0));
				e->pos[2] = (int16_t)(e->pos[2] + (CrtRand() % 0x140 - 0xA0));
				int32_t v[3];
				v[0] = CrtRand() % 0x1000 - 0x800;
				v[1] = CrtRand() % 0x1000 - 0x800;
				v[2] = CrtRand() % 0x1000 - 0x800;
				Normalize(v, v);
				const int32_t sp = CrtRand() % 0x190 + 0xC8;
				e->vel[0] = (int16_t)(mul32(sp, v[0]) >> 12);
				e->vel[1] = (int16_t)(mul32(sp, v[1]) >> 12);
				e->vel[2] = (int16_t)(mul32(sp, v[2]) >> 12);
			}
		}
		n->counter++;
		return (n->counter >= 4 && alive == 0) ? TASK_END : 0;
	}

	static uint32_t __cdecl TargetSparkTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		Particle *arr = PartArray(n->f0E);
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		const int32_t id = n->f0E + 1;
		if (!(UpdateFlags() & 0x201) && n->counter <= 2)
		{
			const int32_t anchors = ***(const uint8_t ***)(Entity(n->f1A) + 0x64);
			for (int32_t j = 0; j < 4; j++)
			{
				int32_t i = 0;
				while (i < PARTS_N && arr[i].id != 0) i++;
				if (i >= PARTS_N) break;
				Particle *e = &arr[i];
				e->id = id;
				e->frame = 0;
				e->size = (int16_t)(CrtRand() % 0x900 + 0x400);
				const int32_t bone = anchors ? CrtRand() % anchors : 0;
				GetEffectSpawnPosition(Entity(n->f1A), bone, CrtRand() % 0x1000, e->pos);
				e->pos[0] = (int16_t)(e->pos[0] + (CrtRand() % 1000 - 500));
				e->pos[1] = (int16_t)(e->pos[1] + (CrtRand() % 1000 - 500));
				e->pos[2] = (int16_t)(e->pos[2] + (CrtRand() % 1000 - 500));
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t alive = 0;
		TargetSparkBegin(s, h);
		for (Particle *e = arr; e < arr + PARTS_N; e++)
		{
			if (e->id != id) continue;
			TargetSparkDraw(e, n->pos[3], s, h);
			if (UpdateFlags() & 0x201) continue;
			e->frame++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				e->id = 0;
				continue;
			}
			alive++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return alive ? 0 : TASK_END;
	}

	static uint32_t __cdecl ModelTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		TargetModelDraw(n, n->counter >= 4, ModelFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 5));
		n->counter++;
		return n->counter >= 12 ? TASK_END : 0;
	}

	static uint32_t __cdecl ShotTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		if (UpdateFlags() & 0x201) return 0;
		if (!(Flags() & 1)) return 0;
		if (n->counter == 0)
		{
			if (Flags() & 4) return 0;
			QueueChainTransformation(Caster(), n->f18);
			Flags() |= 4;
		}
		if (n->counter == 8)
		{
			Ctx()->done = 1;
			Flags() &= ~4u;
		}
		if (n->counter == 0)
		{
			ShotRec *s = Shot(n->f0E);
			int16_t pos[4];
			int32_t dir[3];
			WeaponBone(Caster(), 2, dir, pos);
			s->pos[0] = (int16_t)((mul32(dir[0], 350) >> 12) + pos[0]);
			s->pos[1] = (int16_t)((mul32(dir[1], 350) >> 12) + pos[1]);
			s->pos[2] = (int16_t)((mul32(dir[2], 350) >> 12) + pos[2]);
			int32_t axis[3];
			const int32_t angle = RotationBetweenVectors(MUZZLE_REF(), dir, axis);
			BuildAxisAngleRotationMatrix(angle, &s->m, axis);
			uint8_t *t = Entity(n->f1A);
			GetDefaultEffectPosition(t, n->pos);
			n->pos[3] = (int16_t)(mul32(*(const int16_t *)(t + 0x26), 2000) >> 12);
			n->f1C = (int16_t)(mul32(ClampedSize(t), 2000) >> 12);
			Node *k = AddTask(QAux(), game_addr(ORIG_SmokeTask));
			k->counter = 0;
			k->f0E = n->f0E;
			k->f1C = (int16_t)(CrtRand() % 0x700 + 0x400);
			Node *f = AddTask(QAux(), game_addr(ORIG_Flash2Task));
			f->counter = 0;
			f->f0E = n->f0E;
			f->f18 = (int16_t)(CrtRand() % 0x1000);
			const int16_t third = (int16_t)((CrtRand() % 0x600 + 0x800) / 3);
			f->f1E = third;
			f->f1C = third;
			Node *k2 = AddTask(QAux(), game_addr(ORIG_Smoke2Task));
			k2->counter = 0;
			k2->f0E = n->f0E;
			k2->f1C = (int16_t)(CrtRand() % 0x700 + 0x400);
			Node *d = AddTask(QAux(), game_addr(ORIG_GroundSmokeTask));
			d->f0E = n->f0E;
			d->counter = 0;
			d->f1C = (int16_t)(mul32((int32_t)k2->f1C * 7, 0x400) >> 12);
			Node *b = AddTask(QAux(), game_addr(ORIG_BlastTask));
			b->counter = 0;
			b->f0E = n->f0E;
			const int16_t third2 = (int16_t)((CrtRand() % 0x300 + 0x780) / 3);
			b->f1E = third2;
			b->f1C = third2;
		}
		if (n->counter == 1)
		{
			Node *im = AddTask(QAux(), game_addr(ORIG_ImpactTask));
			const int32_t num = ImpactCount();
			im->f0E = (int16_t)num;
			ImpactCount() = num + 1;
			ff8fx::copy<(8)>(im->pos, n->pos);
			im->counter = 0;
			im->f1C = (int16_t)(CrtRand() % 0xA00 + 0x700);
			Node *sp = AddTask(QAux(), game_addr(ORIG_TargetSparkTask));
			const int32_t num2 = ImpactCount();
			sp->f0E = (int16_t)num2;
			ImpactCount() = num2 + 1;
			sp->counter = 0;
			sp->pos[3] = im->pos[3];
			sp->f1A = n->f1A;
			Node *m = AddTask(QAux(), game_addr(ORIG_ModelTask));
			m->counter = 0;
			GetDefaultEffectPosition(Entity(n->f1A), m->pos);
			const int16_t depth = im->pos[3];
			const int32_t k = ((int32_t)depth << 12) / 1000;
			m->pos[3] = depth;
			const int16_t quarter = (int16_t)((mul32(CrtRand() % 0x600 + 0x1600, k) >> 12) / 4);
			m->f1E = quarter;
			m->f1C = quarter;
			ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
			BdPlaySE(SOUND_Gun(), (n->f0E & 1) ? 1 : 2, 0x80);
		}
		n->counter++;
		return n->counter >= 10 ? TASK_END : 0;
	}

	static uint32_t __cdecl FadeOutTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		SetScreenFlash(n->f18 - n->f18 / n->f0E * n->counter, FADE_COLOUR);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		if (n->counter < n->f0E) return 0;
		SetScreenFlash(0, 0);
		return TASK_END;
	}

	static uint32_t __cdecl FadeInTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		SetScreenFlash(n->f18 / n->f0E * n->counter, FADE_COLOUR);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return n->counter >= n->f0E ? TASK_END : 0;
	}

	static uint32_t __cdecl AfterimageTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		const int32_t c = n->counter;
		Ghost *g = GhostSlot(c % 5);
		g->used = 1;
		uint8_t *e = Caster();
		BoneSnapshot(*(uint8_t **)(e + 0x78), g->bones);
		BoneSnapshot(*(uint8_t **)(Caster() + 0x78), (uint8_t *)game_addr(BONES_NOW));
		const uint32_t colour = *(const uint32_t *)(Caster() + 0x28);
		int32_t slot = c - 1 >= 0 ? (c - 1) % 5 : 0;
		const int32_t end = EndTick();
		const int32_t count = c >= end ? end - c + 5 : 5;
		*(volatile int32_t *)&n_ = count;
		for (int32_t k = 0; k < count; k++)
		{
			const Ghost *s = GhostSlot(slot);
			if (!s->used) break;
			BoneRestore(*(uint8_t **)(Caster() + 0x78), s->bones);
			*(uint32_t *)(Caster() + 0x28) = GHOST_COLOURS()[k];
			GhostDraw(Caster());
			if (--slot < 0) slot = 4;
		}
		BoneRestore(*(uint8_t **)(Caster() + 0x78), (const uint8_t *)game_addr(BONES_NOW));
		*(uint32_t *)(Caster() + 0x28) = colour;
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return n->counter >= EndTick() + 5 ? TASK_END : 0;
	}

	static uint32_t __cdecl StartTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		if (UpdateFlags() & 0x201) return 0;
		if (n->counter == 0)
		{
			BindExternalAnimSet(Caster(), game_addr(BIND_OUT), game_addr(ANIM_DATA));
			QueueChainTransformation(Caster(), 1);
			Node *f = AddTask(QAux(), game_addr(ORIG_FadeInTask));
			f->counter = 0;
			f->f0E = 8;
			f->f18 = 0x800;
			AddTask(QAux(), game_addr(ORIG_AfterimageTask))->counter = 0;
			EndTick() = 999999;
			for (uint32_t a = game_addr(GHOSTS); a < game_addr(GHOSTS_END); a += 0x84) *(int32_t *)a = 0;
		}
		n->counter++;
		if (n->counter < 7) return 0;
		Flags() |= 1;
		Ctx()->done = 1;
		return TASK_END;
	}

	static uint32_t __cdecl EndTask(TaskNode *n_)
	{
		if (UpdateFlags() & 0x201) return 0;
		if (Flags() & 4) return 0;
		Node *n = (Node *)n_;
		if (n->counter == 0)
		{
			QueueChainTransformation(Caster(), 2);
			Node *f = AddTask(QAux(), game_addr(ORIG_FadeOutTask));
			f->counter = 0;
			f->f0E = 8;
			f->f18 = 0x800;
			EndTick() = ModuleTick();
		}
		if (n->counter == 4) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		n->counter++;
		if (n->counter < 0xC) return 0;
		Flags() |= 2;
		Ctx()->done = 1;
		return TASK_END;
	}

	static uint32_t __cdecl RestoreTask(TaskNode *n_)
	{
		RootNode *r = (RootNode *)n_;
		if (r->counter == 1) TextureRestoreTask((void *)TexBase(), &r->done);
		const int16_t done = r->done;
		r->counter++;
		return done ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n_)
	{
		RootNode *r = (RootNode *)n_;
		PacketCursor() = (*(const uint8_t *)&r->counter & 1) ? TexBase() + ARENA_ODD : TexBase();
		const int shots = ExecuteTaskQueue(QShots());
		const int aux = ExecuteTaskQueue(QAux());
		if (!(UpdateFlags() & 0x201)) ModuleTick()++;
		if (aux == 0 && (Flags() & 2))
		{
			RootNode *t = (RootNode *)AddTaskToQueue(QRoot(), game_addr(ORIG_RestoreTask));
			t->counter = 0;
			t->done = 0;
		}
		r->counter++;
		return (aux == 0 && shots == 0 && (Flags() & 2)) ? TASK_END : 0;
	}
}

	void register_mag193_dark_shot()
	{
		register_port(dark193::ORIG_RootTask, (void *)dark193::RootTask, "193 RootTask", 193);
		register_port(dark193::ORIG_RestoreTask, (void *)dark193::RestoreTask, "193 RestoreTask", 193);
		register_port(dark193::ORIG_StartTask, (void *)dark193::StartTask, "193 StartTask", 193);
		register_port(dark193::ORIG_ShotTask, (void *)dark193::ShotTask, "193 ShotTask", 193);
		register_port(dark193::ORIG_EndTask, (void *)dark193::EndTask, "193 EndTask", 193);
		register_port(dark193::ORIG_FadeOutTask, (void *)dark193::FadeOutTask, "193 FadeOutTask", 193);
		register_port(dark193::ORIG_FadeInTask, (void *)dark193::FadeInTask, "193 FadeInTask", 193);
		register_port(dark193::ORIG_AfterimageTask, (void *)dark193::AfterimageTask, "193 AfterimageTask", 193);
		register_port(dark193::ORIG_SmokeTask, (void *)dark193::SmokeTask, "193 SmokeTask", 193);
		register_port(dark193::ORIG_Flash2Task, (void *)dark193::Flash2Task, "193 Flash2Task", 193);
		register_port(dark193::ORIG_Smoke2Task, (void *)dark193::Smoke2Task, "193 Smoke2Task", 193);
		register_port(dark193::ORIG_BlastTask, (void *)dark193::BlastTask, "193 BlastTask", 193);
		register_port(dark193::ORIG_GroundSmokeTask, (void *)dark193::GroundSmokeTask, "193 GroundSmokeTask", 193);
		register_port(dark193::ORIG_ImpactTask, (void *)dark193::ImpactTask, "193 ImpactTask", 193);
		register_port(dark193::ORIG_TargetSparkTask, (void *)dark193::TargetSparkTask, "193 TargetSparkTask", 193);
		register_port(dark193::ORIG_ModelTask, (void *)dark193::ModelTask, "193 ModelTask", 193);
	}
}

