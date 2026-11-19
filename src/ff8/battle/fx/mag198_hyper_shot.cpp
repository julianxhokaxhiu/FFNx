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

// Effect 198: Hyper Shot

#include "mag_common.h"

namespace ff8fx
{
namespace hyper198
{
	using namespace eng;
	using namespace magc;

	template<typename T> inline T &AT(uint32_t p, int32_t o) { return *(T *)(p + o); }
	inline uint16_t &U16(uint32_t p, int32_t o) { return AT<uint16_t>(p, o); }
	inline int32_t &S32(uint32_t p, int32_t o) { return AT<int32_t>(p, o); }
	inline uint32_t &U32(uint32_t p, int32_t o) { return AT<uint32_t>(p, o); }
	inline uint8_t &U8(uint32_t p, int32_t o) { return AT<uint8_t>(p, o); }
	inline int16_t S16v(uint32_t p, int32_t o) { return AT<int16_t>(p, o); }

	inline int32_t &EndTick() { return var<int32_t>(game_addr(game::MAG_198_HYPERSHOT_AfterimageEndTick)); }
	inline int32_t &ModuleTick() { return var<int32_t>(game_addr(game::MAG_198_HYPERSHOT_ModuleTick)); }
	inline uint32_t &Flags() { return var<uint32_t>(game_addr(game::MAG_198_HYPERSHOT_Flags)); }
	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_198_HYPERSHOT_CastContext)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::MAG_198_HYPERSHOT_CasterSlot)); }
	inline int32_t &IdCount() { return var<int32_t>(game_addr(game::MAG_198_HYPERSHOT_ParticleIdCount)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_198_HYPERSHOT_TextureBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_198_HYPERSHOT_PacketCursor)); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::MAG_198_HYPERSHOT_RootQueue); }
	inline TaskQueue *QShots() { return (TaskQueue *)game_addr(game::MAG_198_HYPERSHOT_ShotQueue); }
	inline TaskQueue *QAux() { return (TaskQueue *)game_addr(game::MAG_198_HYPERSHOT_AuxQueue); }
	static constexpr game::Id PARTS_A = game::MAG_198_HYPERSHOT_ParticlesEven, PARTS_B = game::MAG_198_HYPERSHOT_ParticlesOdd;
	static const int32_t PARTS_N = 200;
	static constexpr game::Id SHOTS = game::MAG_198_HYPERSHOT_ShotRecords;
	static constexpr game::Id BIND_OUT = game::MAG_198_HYPERSHOT_AnimBindBlock;
	static constexpr game::Id BONES_NOW = game::MAG_198_HYPERSHOT_WeaponBonesNow;
	static constexpr game::Id GHOSTS = game::MAG_198_HYPERSHOT_GhostSlots, GHOSTS_END = game::MAG_198_HYPERSHOT_ShotCount;
	static const uint32_t ARENA_ODD = 0xC000;

	static constexpr game::Id ORIG_RootTask = game::MAG_198_HYPERSHOT_RootTask;
	static constexpr game::Id ORIG_StartTask = game::MAG_198_HYPERSHOT_StartTask;
	static constexpr game::Id ORIG_ShotTask = game::MAG_198_HYPERSHOT_ShotTask;
	static constexpr game::Id ORIG_EndTask = game::MAG_198_HYPERSHOT_EndTask;
	static constexpr game::Id ORIG_FadeOutTask = game::MAG_198_HYPERSHOT_FadeOutTask;
	static constexpr game::Id ORIG_FadeInTask = game::MAG_198_HYPERSHOT_FadeInTask;
	static constexpr game::Id ORIG_AfterimageTask = game::MAG_198_HYPERSHOT_AfterimageTask;
	static constexpr game::Id ORIG_MuzzleFlashTask = game::MAG_198_HYPERSHOT_MuzzleFlashTask;
	static constexpr game::Id ORIG_BoneSmokeTask = game::MAG_198_HYPERSHOT_BoneSmokeTask;
	static constexpr game::Id ORIG_BeamTask = game::MAG_198_HYPERSHOT_BeamTask;
	static constexpr game::Id ORIG_SprayTask = game::MAG_198_HYPERSHOT_SprayTask;
	static constexpr game::Id ORIG_MuzzleConeTask = game::MAG_198_HYPERSHOT_MuzzleConeTask;
	static constexpr game::Id ORIG_ImpactSparkTask = game::MAG_198_HYPERSHOT_ImpactSparkTask;
	static constexpr game::Id ORIG_ImpactSmokeTask = game::MAG_198_HYPERSHOT_ImpactSmokeTask;
	static constexpr game::Id ORIG_GroundRingTask = game::MAG_198_HYPERSHOT_GroundRingTask;

	static constexpr game::Id ANIM_DATA = game::MAG_198_HYPERSHOT_AnimSetComData;
	static constexpr game::Id MODEL_MuzzleFlash = game::MAG_198_HYPERSHOT_ModelMuzzleFlash, MODEL_Beam = game::MAG_198_HYPERSHOT_ModelBeam, MODEL_BeamShadow = game::MAG_198_HYPERSHOT_ModelBeamShadow, MODEL_Cone = game::MAG_198_HYPERSHOT_ModelMuzzleCone, MODEL_GroundRing = game::MAG_198_HYPERSHOT_ModelGroundRing;
	static constexpr game::Id SEQ_BoneSmoke = game::MAG_198_HYPERSHOT_SeqBoneSmoke, SEQ_Spark = game::MAG_198_HYPERSHOT_SeqSpark, SEQ_Smoke = game::MAG_198_HYPERSHOT_SeqImpactSmoke;
	static const void *SOUND_Gun() { return (const void *)game_addr(game::MAG_198_HYPERSHOT_SoundGun); }
	static const int32_t *DIR_BACK() { return (const int32_t *)game_addr(game::MAG_198_HYPERSHOT_DirBack); }
	static const int32_t *DIR_FRONT() { return (const int32_t *)game_addr(game::MAG_198_HYPERSHOT_DirFront); }
	static const uint32_t *GHOST_COLOURS() { return (const uint32_t *)game_addr(game::MAG_198_HYPERSHOT_GhostColours); }

	inline uint32_t BindExternalAnimSet(uint8_t *entity, uint32_t out, uint32_t data) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline uint32_t QueueChainTransformation(uint8_t *entity, uint32_t id) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
	inline int32_t GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t angle, int16_t *out) { return fn<int32_t (__cdecl *)(uint8_t *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, angle, out); }
	inline int32_t Normalize(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
	inline void MatVec(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void GteMatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline uint32_t RenderGeometry(uint32_t model, void *ctx, uint32_t ot, int32_t a, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint32_t, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(model, ctx, ot, a, cursor); }
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteSetBackColorFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
	inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
	inline void GteSetTransFromVec32(const int32_t *v) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_SetTransFromVec32))(v); }
	inline void GteTransFromMAC() { fn<void (__cdecl *)()>(game_addr(game::GTE_TransFromMAC))(); }
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
		int16_t f0E;
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
		int16_t f20;
		int16_t f22;
	};
	struct ShotRec
	{
		int16_t pos[4];
		int32_t dir[3];
		uint8_t pad14[4];
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
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node) == 0x24, "Hyper Shot nodes");
	static_assert(sizeof(ShotRec) == 0x38 && sizeof(Particle) == 0x18 && sizeof(Ghost) == 0x84, "Hyper Shot records");

	inline ShotRec *Shot(int32_t i) { return (ShotRec *)(game_addr(SHOTS) + 0x38 * i); }
	inline Ghost *GhostSlot(int32_t i) { return (Ghost *)(game_addr(GHOSTS) + 0x84 * i); }
	inline uint8_t *Caster() { return Entity(CasterSlot()); }
	inline Node *AddTask(TaskQueue *q, uint32_t fn) { return (Node *)AddTaskToQueue(q, fn); }
	inline Particle *PartArray(int16_t k) { return (Particle *)((k & 1) ? game_addr(PARTS_B) : game_addr(PARTS_A)); }
	inline int32_t FindFree(const Particle *arr) { for (int32_t i = 0; i < PARTS_N; i++) if (arr[i].id == 0) return i; return -1; }
}
}

namespace ff8fx
{
namespace hyper198
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

	static void ScrSemi(uint32_t h, uint32_t pk, int32_t off, uint32_t set, uint32_t clear)
	{
		const uint32_t f = U32(h, 0x20);
		if (f & set) U32(pk, off) = U32(pk, off) | 0x2000000;
		if (f & clear) U32(pk, off) &= 0xFDFFFFFF;
	}
	static void ScrScroll(uint32_t h, uint32_t pk, const int32_t *off, int n, int32_t o, int32_t span)
	{
		const int32_t s = S32(h, o ? 0x70 : 0x6C);
		if (!s) return;
		int32_t a[4];
		for (int i = 0; i < n; i++) { a[i] = (int32_t)U8(pk, off[i] + o) + s; S32(h, 0x5C + 4 * i) = a[i]; }
		bool hi = false, lo = false;
		for (int i = 0; i < n; i++) if (a[i] >= 0x100) hi = true;
		if (!hi) for (int i = 0; i < n; i++) if (a[i] < 0) lo = true;
		if (hi) for (int i = 0; i < n; i++) S32(h, 0x5C + 4 * i) = a[i] - span;
		else if (lo) for (int i = 0; i < n; i++) S32(h, 0x5C + 4 * i) = a[i] + span;
		for (int i = 0; i < n; i++) U8(pk, off[i] + o) = U8(h, 0x5C + 4 * i);
	}
	static void ScrCueFlat(uint32_t h, uint32_t c)
	{
		GteLoadRGBCu(c);
		GteSetIR0(S32(h, 0xC));
		GteDPCSx();
		GteStoreRGB2u(c);
	}
	static void ScrInsert(uint32_t h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		InsertPrimAutoDepth(ot + (uint32_t)shl32(S32(h, 0x30) >> (shift & 31), 2), (void *)pk);
	}

	static uint32_t ScrF3(uint32_t h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		int32_t count = S32(U32(h, 0x24), 0);
		uint32_t rec = U32(h, 0x24) + 4;
		U32(h, 0x24) = rec;
		const uint32_t verts = U32(h, 4);
		for (; count > 0; count--, rec += 0xC)
		{
			GteLoadV012u(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPTx();
			U32(pk, 0) = 0x4000000;
			U32(pk, 4) = U32(rec, 0);
			ScrSemi(h, pk, 4, 1, 4);
			GteReadFLAGu(h + 0x34);
			if (U32(h, 0x34) & 0x60000) continue;
			GteNCLIPx();
			GteReadMAC0u(h + 0x28);
			const int32_t nc = S32(h, 0x28);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x20) & 0x10)) continue;
			GteReadSXY012u(pk + 8, pk + 0xC, pk + 0x10);
			GteAVSZ3x();
			uint32_t m = ClipX(pk + 8, 1) | ClipX(pk + 0xC, 2) | ClipX(pk + 0x10, 4);
			m |= ClipY(pk + 8, 0x10) | ClipY(pk + 0xC, 0x20) | ClipY(pk + 0x10, 0x40);
			if ((m & 7) == 7 || (m & 0x70) == 0x70) continue;
			GteReadOTZ32(h + 0x30);
			if (U8(h, 0x20) & 0x40) ScrCueFlat(h, pk + 4);
			ScrInsert(h, ot, shift, pk);
			pk += 0x14;
		}
		U32(h, 0x24) = rec;
		return pk;
	}

	static uint32_t ScrF4(uint32_t h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		int32_t count = S32(U32(h, 0x24), 0);
		uint32_t rec = U32(h, 0x24) + 4;
		U32(h, 0x24) = rec;
		const uint32_t verts = U32(h, 4);
		for (; count > 0; count--, rec += 0xC)
		{
			GteLoadV012u(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPTx();
			U32(pk, 0) = 0x5000000;
			U32(pk, 4) = U32(rec, 0);
			ScrSemi(h, pk, 4, 1, 4);
			GteReadFLAGu(h + 0x34);
			if (U32(h, 0x34) & 0x60000) continue;
			GteNCLIPx();
			GteReadMAC0u(h + 0x28);
			const int32_t nc = S32(h, 0x28);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x20) & 0x10)) continue;
			GteReadSXY012u(pk + 8, pk + 0xC, pk + 0x10);
			GteLoadV0u(verts + U16(rec, 0xA) * 4);
			GteRTPS();
			uint32_t m = ClipX(pk + 8, 1) | ClipX(pk + 0xC, 2) | ClipX(pk + 0x10, 4);
			m |= ClipY(pk + 8, 0x10) | ClipY(pk + 0xC, 0x20) | ClipY(pk + 0x10, 0x40);
			GteReadSXY2u(pk + 0x14);
			GteAVSZ4x();
			m |= ClipX(pk + 0x14, 8) | ClipY(pk + 0x14, 0x80);
			if ((m & 0xF) == 0xF || (m & 0xF0) == 0xF0) continue;
			GteReadOTZ32(h + 0x30);
			if (U8(h, 0x20) & 0x40) ScrCueFlat(h, pk + 4);
			ScrInsert(h, ot, shift, pk);
			pk += 0x18;
		}
		U32(h, 0x24) = rec;
		return pk;
	}

	static uint32_t ScrFT3(uint32_t h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		const int32_t w = S16v(h, 0x1C), hh = S16v(h, 0x1E);
		int32_t count = S32(U32(h, 0x24), 0);
		uint32_t rec = U32(h, 0x24) + 4;
		U32(h, 0x24) = rec;
		const uint32_t verts = U32(h, 4);
		static const int32_t UV[3] = { 0x10, 0x18, 0x20 };
		for (; count > 0; count--, rec += 0x14)
		{
			GteLoadV012u(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPTx();
			U32(pk, 0) = 0x9000000;
			U32(pk, 8) = U32(rec, 0);
			ScrSemi(h, pk, 8, 1, 4);
			U32(pk, 0x10) = U32(rec, 0xC);
			U32(pk, 0x18) = U32(rec, 0x10);
			U32(pk, 0x20) = U32(rec, 8) >> 16;
			GteReadFLAGu(h + 0x34);
			if (U32(h, 0x34) & 0x60000) continue;
			GteNCLIPx();
			ScrScroll(h, pk, UV, 3, 0, w);
			ScrScroll(h, pk, UV, 3, 1, hh);
			GteReadMAC0u(h + 0x28);
			const int32_t nc = S32(h, 0x28);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x20) & 0x10)) continue;
			GteReadSXY012u(pk + 0xC, pk + 0x14, pk + 0x1C);
			GteAVSZ3x();
			uint32_t m = ClipX(pk + 0xC, 1) | ClipX(pk + 0x14, 2) | ClipX(pk + 0x1C, 4);
			m |= ClipY(pk + 0xC, 0x10) | ClipY(pk + 0x14, 0x20) | ClipY(pk + 0x1C, 0x40);
			if ((m & 7) == 7 || (m & 0x70) == 0x70) continue;
			GteReadOTZ32(h + 0x30);
			if (U8(h, 0x20) & 0x40) ScrCueFlat(h, pk + 8);
			U32(pk, 4) = U32(h, 0x74);
			U32(pk, 0x24) = U32(h, 0x78);
			ScrInsert(h, ot, shift, pk);
			pk += 0x28;
		}
		U32(h, 0x24) = rec;
		return pk;
	}

	static uint32_t ScrFT4(uint32_t h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		const int32_t w = S16v(h, 0x1C), hh = S16v(h, 0x1E);
		int32_t count = S32(U32(h, 0x24), 0);
		uint32_t rec = U32(h, 0x24) + 4;
		U32(h, 0x24) = rec;
		const uint32_t verts = U32(h, 4);
		static const int32_t UV[4] = { 0x10, 0x18, 0x20, 0x28 };
		for (; count > 0; count--, rec += 0x18)
		{
			GteLoadV012u(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPTx();
			U32(pk, 0) = 0xB000000;
			U32(pk, 8) = U32(rec, 0);
			ScrSemi(h, pk, 8, 1, 4);
			U32(pk, 0x10) = U32(rec, 0xC);
			U32(pk, 0x20) = U32(rec, 0x14);
			U32(pk, 0x18) = U32(rec, 0x10);
			U32(pk, 0x28) = U32(rec, 0x14) >> 16;
			GteReadFLAGu(h + 0x34);
			if (U32(h, 0x34) & 0x60000) continue;
			GteNCLIPx();
			ScrScroll(h, pk, UV, 4, 0, w);
			ScrScroll(h, pk, UV, 4, 1, hh);
			GteReadMAC0u(h + 0x28);
			const int32_t nc = S32(h, 0x28);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x20) & 0x10)) continue;
			GteReadSXY012u(pk + 0xC, pk + 0x14, pk + 0x1C);
			GteLoadV0u(verts + U16(rec, 0xA) * 4);
			GteRTPS();
			uint32_t m = ClipX(pk + 0xC, 1) | ClipX(pk + 0x14, 2) | ClipX(pk + 0x1C, 4);
			m |= ClipY(pk + 0xC, 0x10) | ClipY(pk + 0x14, 0x20) | ClipY(pk + 0x1C, 0x40);
			GteReadSXY2u(pk + 0x24);
			GteAVSZ4x();
			m |= ClipX(pk + 0x24, 8) | ClipY(pk + 0x24, 0x80);
			if ((m & 0xF) == 0xF || (m & 0xF0) == 0xF0) continue;
			GteReadOTZ32(h + 0x30);
			if (U8(h, 0x20) & 0x40) ScrCueFlat(h, pk + 8);
			U32(pk, 4) = U32(h, 0x74);
			U32(pk, 0x2C) = U32(h, 0x78);
			ScrInsert(h, ot, shift, pk);
			pk += 0x30;
		}
		U32(h, 0x24) = rec;
		return pk;
	}

	static uint32_t ScrG3(uint32_t h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		int32_t count = S32(U32(h, 0x24), 0);
		uint32_t rec = U32(h, 0x24) + 4;
		U32(h, 0x24) = rec;
		const uint32_t verts = U32(h, 4);
		for (; count > 0; count--, rec += 0x14)
		{
			GteLoadV012u(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPTx();
			U32(pk, 0) = 0x6000000;
			U32(pk, 4) = U32(rec, 0);
			ScrSemi(h, pk, 4, 2, 8);
			GteReadFLAGu(h + 0x34);
			if (U32(h, 0x34) & 0x60000) continue;
			GteNCLIPx();
			GteReadMAC0u(h + 0x28);
			const int32_t nc = S32(h, 0x28);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x20) & 0x20)) continue;
			GteReadSXY012u(pk + 8, pk + 0x10, pk + 0x18);
			GteAVSZ3x();
			uint32_t m = ClipX(pk + 8, 1) | ClipX(pk + 0x10, 2) | ClipX(pk + 0x18, 4);
			m |= ClipY(pk + 8, 0x10) | ClipY(pk + 0x10, 0x20) | ClipY(pk + 0x18, 0x40);
			if ((m & 7) == 7 || (m & 0x70) == 0x70) continue;
			GteReadOTZ32(h + 0x30);
			if (U8(h, 0x20) & 0x80)
			{
				GteLoadRGB012u(rec + 0xC, rec + 0x10, pk + 4);
				GteSetIR0(S32(h, 0xC));
				GteDPCTx();
				GteStoreRGB012u(pk + 0xC, pk + 0x14, pk + 4);
			}
			else
			{
				U32(pk, 0xC) = U32(rec, 0xC);
				U32(pk, 0x14) = U32(rec, 0x10);
			}
			ScrInsert(h, ot, shift, pk);
			pk += 0x1C;
		}
		U32(h, 0x24) = rec;
		return pk;
	}

	static uint32_t ScrG4(uint32_t h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		int32_t count = S32(U32(h, 0x24), 0);
		uint32_t rec = U32(h, 0x24) + 4;
		U32(h, 0x24) = rec;
		const uint32_t verts = U32(h, 4);
		for (; count > 0; count--, rec += 0x18)
		{
			GteLoadV012u(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPTx();
			U32(pk, 0) = 0x8000000;
			U32(pk, 4) = U32(rec, 0);
			ScrSemi(h, pk, 4, 2, 8);
			GteReadFLAGu(h + 0x34);
			if (U32(h, 0x34) & 0x60000) continue;
			GteNCLIPx();
			GteReadMAC0u(h + 0x28);
			const int32_t nc = S32(h, 0x28);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x20) & 0x20)) continue;
			GteReadSXY012u(pk + 8, pk + 0x10, pk + 0x18);
			GteLoadV0u(verts + U16(rec, 0xA) * 4);
			GteRTPS();
			uint32_t m = ClipX(pk + 8, 1) | ClipX(pk + 0x10, 2) | ClipX(pk + 0x18, 4);
			m |= ClipY(pk + 8, 0x10) | ClipY(pk + 0x10, 0x20) | ClipY(pk + 0x18, 0x40);
			GteReadSXY2u(pk + 0x20);
			GteAVSZ4x();
			m |= ClipX(pk + 0x20, 8) | ClipY(pk + 0x20, 0x80);
			if ((m & 0xF) == 0xF || (m & 0xF0) == 0xF0) continue;
			GteReadOTZ32(h + 0x30);
			if (U8(h, 0x20) & 0x80)
			{
				GteLoadRGB012u(rec + 0xC, rec + 0x10, rec + 0x14);
				GteSetIR0(S32(h, 0xC));
				GteDPCTx();
				GteStoreRGB012u(pk + 0xC, pk + 0x14, pk + 0x1C);
				GteLoadRGBCu(pk + 4);
				GteDPCSx();
				GteStoreRGB2u(pk + 4);
			}
			else
			{
				U32(pk, 0xC) = U32(rec, 0xC);
				U32(pk, 0x14) = U32(rec, 0x10);
				U32(pk, 0x1C) = U32(rec, 0x14);
			}
			ScrInsert(h, ot, shift, pk);
			pk += 0x24;
		}
		U32(h, 0x24) = rec;
		return pk;
	}

	static uint32_t ScrGT3(uint32_t h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		const int32_t w = S16v(h, 0x1C), hh = S16v(h, 0x1E);
		int32_t count = S32(U32(h, 0x24), 0);
		uint32_t rec = U32(h, 0x24) + 4;
		U32(h, 0x24) = rec;
		const uint32_t verts = U32(h, 4);
		static const int32_t UV[3] = { 0x10, 0x1C, 0x28 };
		for (; count > 0; count--, rec += 0x1C)
		{
			GteLoadV012u(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPTx();
			U32(pk, 0) = 0xB000000;
			U32(pk, 8) = U32(rec, 0);
			ScrSemi(h, pk, 8, 2, 8);
			U32(pk, 0x10) = U32(rec, 0xC);
			U32(pk, 0x1C) = U32(rec, 0x10);
			U32(pk, 0x28) = U32(rec, 8) >> 16;
			GteReadFLAGu(h + 0x34);
			if (U32(h, 0x34) & 0x60000) continue;
			GteNCLIPx();
			ScrScroll(h, pk, UV, 3, 0, w);
			ScrScroll(h, pk, UV, 3, 1, hh);
			GteReadMAC0u(h + 0x28);
			const int32_t nc = S32(h, 0x28);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x20) & 0x20)) continue;
			GteReadSXY012u(pk + 0xC, pk + 0x18, pk + 0x24);
			GteAVSZ3x();
			uint32_t m = ClipX(pk + 0xC, 1) | ClipX(pk + 0x18, 2) | ClipX(pk + 0x24, 4);
			m |= ClipY(pk + 0xC, 0x10) | ClipY(pk + 0x18, 0x20) | ClipY(pk + 0x24, 0x40);
			if ((m & 7) == 7 || (m & 0x70) == 0x70) continue;
			GteReadOTZ32(h + 0x30);
			if (U8(h, 0x20) & 0x80)
			{
				GteLoadRGB012u(rec + 0x14, rec + 0x18, pk + 8);
				GteSetIR0(S32(h, 0xC));
				GteDPCTx();
				GteStoreRGB012u(pk + 0x14, pk + 0x20, pk + 8);
			}
			else
			{
				U32(pk, 0x14) = U32(rec, 0x14);
				U32(pk, 0x20) = U32(rec, 0x18);
			}
			U32(pk, 4) = U32(h, 0x74);
			U32(pk, 0x2C) = U32(h, 0x78);
			ScrInsert(h, ot, shift, pk);
			pk += 0x30;
		}
		U32(h, 0x24) = rec;
		return pk;
	}

	static uint32_t ScrGT4(uint32_t h, uint32_t ot, int32_t shift, uint32_t pk)
	{
		const int32_t w = S16v(h, 0x1C), hh = S16v(h, 0x1E);
		int32_t count = S32(U32(h, 0x24), 0);
		uint32_t rec = U32(h, 0x24) + 4;
		U32(h, 0x24) = rec;
		const uint32_t verts = U32(h, 4);
		static const int32_t UV[4] = { 0x10, 0x1C, 0x28, 0x34 };
		for (; count > 0; count--, rec += 0x24)
		{
			GteLoadV012u(verts + U16(rec, 4) * 4, verts + U16(rec, 6) * 4, verts + U16(rec, 8) * 4);
			GteRTPTx();
			U32(pk, 0) = 0xE000000;
			U32(pk, 8) = U32(rec, 0);
			ScrSemi(h, pk, 8, 2, 8);
			U32(pk, 0x10) = U32(rec, 0xC);
			U32(pk, 0x28) = U32(rec, 0x14);
			U32(pk, 0x1C) = U32(rec, 0x10);
			U32(pk, 0x34) = U32(rec, 0x14) >> 16;
			GteReadFLAGu(h + 0x34);
			if (U32(h, 0x34) & 0x60000) continue;
			GteNCLIPx();
			ScrScroll(h, pk, UV, 4, 0, w);
			ScrScroll(h, pk, UV, 4, 1, hh);
			GteReadMAC0u(h + 0x28);
			const int32_t nc = S32(h, 0x28);
			if (nc == 0) continue;
			if (nc < 0 && !(U8(h, 0x20) & 0x20)) continue;
			GteReadSXY012u(pk + 0xC, pk + 0x18, pk + 0x24);
			GteLoadV0u(verts + U16(rec, 0xA) * 4);
			GteRTPS();
			uint32_t m = ClipX(pk + 0xC, 1) | ClipX(pk + 0x18, 2) | ClipX(pk + 0x24, 4);
			m |= ClipY(pk + 0xC, 0x10) | ClipY(pk + 0x18, 0x20) | ClipY(pk + 0x24, 0x40);
			GteReadSXY2u(pk + 0x30);
			GteAVSZ4x();
			m |= ClipX(pk + 0x30, 8) | ClipY(pk + 0x30, 0x80);
			if ((m & 0xF) == 0xF || (m & 0xF0) == 0xF0) continue;
			GteReadOTZ32(h + 0x30);
			if (U8(h, 0x20) & 0x80)
			{
				GteLoadRGB012u(rec + 0x18, rec + 0x1C, rec + 0x20);
				GteSetIR0(S32(h, 0xC));
				GteDPCTx();
				GteStoreRGB012u(pk + 0x14, pk + 0x20, pk + 0x2C);
				GteLoadRGBCu(pk + 8);
				GteDPCSx();
				GteStoreRGB2u(pk + 8);
			}
			else
			{
				U32(pk, 0x14) = U32(rec, 0x18);
				U32(pk, 0x20) = U32(rec, 0x1C);
				U32(pk, 0x2C) = U32(rec, 0x20);
			}
			U32(pk, 4) = U32(h, 0x74);
			U32(pk, 0x38) = U32(h, 0x78);
			ScrInsert(h, ot, shift, pk);
			pk += 0x3C;
		}
		U32(h, 0x24) = rec;
		return pk;
	}

	static uint32_t ScrollRender(uint32_t h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t model = U32(h, 0);
		U32(h, 4) = model + 8;
		U32(h, 0x24) = model + U32(model, 0);
		uint32_t tw = ((uint32_t)U8(h, 0x1A) & 0xF8) | 0xFFFE2000u;
		tw = (tw << 5) | ((uint32_t)U8(h, 0x18) & 0xF8);
		tw = (tw << 5) | (~((uint32_t)U16(h, 0x1E) - 1) & 0xF8);
		tw = (tw << 2) | ((~((uint32_t)U16(h, 0x1C) - 1) >> 3) & 0x1F);
		U32(h, 0x74) = tw;
		U32(h, 0x78) = 0xE2000000;
		S32(h, 0x6C) = S32(h, 0x10) % (int32_t)S16v(h, 0x1C);
		S32(h, 0x70) = S32(h, 0x14) % (int32_t)S16v(h, 0x1E);
		GteSetFarColor(U8(h, 8), U8(h, 9), U8(h, 0xA));
		if (U32(U32(h, 0x24), 0)) cursor = ScrF3(h, ot, shift, cursor);
		else U32(h, 0x24) += 4;
		if (U32(U32(h, 0x24), 0)) cursor = ScrF4(h, ot, shift, cursor);
		else U32(h, 0x24) += 4;
		if (U32(U32(h, 0x24), 0)) cursor = ScrFT3(h, ot, shift, cursor);
		else U32(h, 0x24) += 4;
		if (U32(U32(h, 0x24), 0)) cursor = ScrFT4(h, ot, shift, cursor);
		else U32(h, 0x24) += 4;
		if (U32(U32(h, 0x24), 0)) cursor = ScrG3(h, ot, shift, cursor);
		else U32(h, 0x24) += 4;
		if (U32(U32(h, 0x24), 0)) cursor = ScrG4(h, ot, shift, cursor);
		else U32(h, 0x24) += 4;
		if (U32(U32(h, 0x24), 0)) cursor = ScrGT3(h, ot, shift, cursor);
		else U32(h, 0x24) += 4;
		if (U32(U32(h, 0x24), 0)) cursor = ScrGT4(h, ot, shift, cursor);
		else U32(h, 0x24) += 4;
		return cursor;
	}

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

	static uint8_t *ModelHeader(uint32_t bytes, uint32_t model, bool faded, int32_t fade)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(bytes);
		*(uint32_t *)h = model;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (faded)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		return h;
	}
	static void ModelMatrix(Mat4x3 *m)
	{
		ComposeAffineTransform(&Camera(), m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
	}
	static void MuzzleMatrix(const Node *n, Mat4x3 *m)
	{
		const ShotRec *s = Shot(n->f0E);
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = n->f18;
		ang[3] = 0;
		ComposeZYXRotationMatrix(ang, m);
		GteMatrixMultiply(&s->m, m);
		m->t[0] = s->pos[0];
		m->t[1] = s->pos[1];
		m->t[2] = s->pos[2];
		const int32_t sc = n->f1C;
		const int32_t v[3] = { sc, sc, sc };
		Scale3DMatrix(m, v);
		ModelMatrix(m);
	}

	static int32_t FadeFrom2(int32_t c) { return (int32_t)((uint32_t)(c - 2) << 9); }
	static int32_t FadeFrom8(int32_t c) { return (int32_t)((uint32_t)(c - 8) << 9); }
	static int32_t RingFade(int32_t c) { return mul32(c - 4, 0x2AA); }
	static int32_t FlashScroll(int32_t c) { return (int32_t)((uint32_t)(-c) << 4); }

	static void MuzzleFlashDraw(const Node *n, bool faded, int32_t fade, int32_t scroll)
	{
		Mat4x3 m;
		MuzzleMatrix(n, &m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x7C);
		*(uint16_t *)(h + 0x1A) = 0x80;
		*(uint16_t *)(h + 0x1E) = 0x80;
		*(uint32_t *)h = game_addr(MODEL_MuzzleFlash);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x20) = 0x33;
		*(uint16_t *)(h + 0x18) = 0;
		*(uint16_t *)(h + 0x1C) = 0x40;
		*(int32_t *)(h + 0x10) = 0;
		*(int32_t *)(h + 0x14) = scroll;
		if (faded)
		{
			*(uint32_t *)(h + 0x20) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = ScrollRender((uint32_t)h, RenderOT(), 2, PacketCursor());
		FieldFree(0x7C);
	}

	static void MuzzleConeDraw(const Node *n, bool faded, int32_t fade)
	{
		Mat4x3 m;
		MuzzleMatrix(n, &m);
		uint8_t *h = ModelHeader(0x5C, game_addr(MODEL_Cone), faded, fade);
		PacketCursor() = PrimRender((uint32_t)h, RenderOT(), 2, PacketCursor());
		FieldFree(0x5C);
	}

	static void BeamDraw(const Node *n, bool faded, int32_t fade)
	{
		ShotRec *s = Shot(n->f0E);
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = 0;
		ang[3] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t axis[3];
		const int32_t angle = RotationBetweenVectors(DIR_BACK(), s->dir, axis);
		Mat4x3 r;
		BuildAxisAngleRotationMatrix(angle, &r, axis);
		GteMatrixMultiply(&r, &m);
		m.t[0] = s->pos[0];
		m.t[1] = s->pos[1];
		m.t[2] = s->pos[2];
		int32_t v[3] = { n->f1C, n->f1C, ((int32_t)n->f20 << 12) / 6000 };
		Scale3DMatrix(&m, v);
		s->m.t[0] = mul32(n->f1C, 600) >> 12;
		ComposeAffineTransform(&Camera(), &m, &m);
		s->m.t[2] = n->f20;
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = ModelHeader(0x58, game_addr(MODEL_Beam), faded, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		int32_t d[4];
		d[0] = s->dir[0];
		d[1] = s->dir[1];
		d[2] = s->dir[2];
		d[3] = *(const int32_t *)s->pad14;
		d[1] = 0;
		Normalize(d, d);
		const int32_t angle2 = RotationBetweenVectors(DIR_BACK(), d, axis);
		BuildAxisAngleRotationMatrix(angle2, &m, axis);
		m.t[0] = s->pos[0];
		m.t[1] = 0;
		m.t[2] = s->pos[2];
		v[1] = n->f1C;
		v[0] = n->f1C;
		const int32_t y = (mul32(s->dir[1], n->f20) >> 12) + s->pos[1];
		v[2] = (Sqrt(mul32(n->f20, n->f20) - mul32(y, y)) << 12) / 6000;
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		*(uint32_t *)h = game_addr(MODEL_BeamShadow);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void GroundRingDraw(const Node *n, bool faded, int32_t fade)
	{
		const ShotRec *s = Shot(n->f0E);
		int32_t d[4];
		d[0] = s->dir[0];
		d[1] = s->dir[1];
		d[2] = s->dir[2];
		d[3] = *(const int32_t *)s->pad14;
		d[1] = 0;
		Normalize(d, d);
		int32_t axis[3];
		const int32_t angle = RotationBetweenVectors(DIR_FRONT(), d, axis);
		Mat4x3 m;
		BuildAxisAngleRotationMatrix(angle, &m, axis);
		const int32_t k = -(int32_t)n->pos[3];
		m.t[1] = 0;
		m.t[0] = (mul32(d[0], k) >> 12) + n->pos[0];
		m.t[2] = (mul32(d[2], k) >> 12) + n->pos[2];
		const int32_t sc = n->f1C;
		const int32_t v[3] = { sc, sc, sc };
		Scale3DMatrix(&m, v);
		ModelMatrix(&m);
		uint8_t *h = ModelHeader(0x58, game_addr(MODEL_GroundRing), faded, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static void SparkBegin(uint8_t *s, uint8_t *h, uint32_t seq)
	{
		*(int16_t *)(h + 0x24) = 0;
		ff8fx::copy<(0x20)>(s + 8, &Camera());
		*(uint32_t *)h = seq;
		GteSetLightMatrix(s + 8);
		GteSetBackColorFromTrans(s + 8);
	}
	static void SparkDraw(const Particle *e, int32_t pull, uint8_t *s, uint8_t *h)
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
	static int32_t BoneSmokePull(const Particle *e) { return (int32_t)e->size >> 4; }
	static void SmokeDraw(const Particle *e, uint8_t *h)
	{
		GteLoadV0(e->pos);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(e->size);
		*(int16_t *)(h + 4) = e->frame;
		GteTransFromMAC();
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}
	static void MoveDecay(Particle *e)
	{
		e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
		e->pos[1] = (int16_t)(e->pos[1] + e->vel[1]);
		e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
		e->vel[0] = (int16_t)(e->vel[0] - (int16_t)(e->vel[0] >> 2));
		e->vel[1] = (int16_t)(e->vel[1] - (int16_t)(e->vel[1] >> 2));
		e->vel[2] = (int16_t)(e->vel[2] - (int16_t)(e->vel[2] >> 2));
	}

	static uint32_t __cdecl MuzzleFlashTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		MuzzleFlashDraw(n, n->counter >= 8, FadeFrom8(n->counter), FlashScroll(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 4));
		n->counter++;
		return n->counter >= 16 ? TASK_END : 0;
	}

	static uint32_t __cdecl MuzzleConeTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		MuzzleConeDraw(n, n->counter >= 2, FadeFrom2(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t spin = n->f1A;
		n->f18 = (int16_t)(n->f18 + spin);
		n->f1A = (int16_t)(spin - (int16_t)(spin / 10));
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 5));
		n->counter++;
		return n->counter >= 10 ? TASK_END : 0;
	}

	static uint32_t __cdecl BeamTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		BeamDraw(n, n->counter >= 8, FadeFrom8(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t shrink = n->f1E;
		n->f1C = (int16_t)(n->f1C - shrink);
		n->f1E = (int16_t)(shrink + (int16_t)(shrink / 8));
		if (n->counter < 4) n->f20 = (int16_t)(n->f20 + n->f22);
		n->counter++;
		return n->counter >= 16 ? TASK_END : 0;
	}

	static uint32_t __cdecl GroundRingTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		GroundRingDraw(n, n->counter >= 4, RingFade(n->counter));
		if (UpdateFlags() & 0x201) return 0;
		const int16_t step = n->f1E;
		n->f1C = (int16_t)(n->f1C + step);
		n->f1E = (int16_t)(step - (int16_t)(step / 4));
		n->counter++;
		return n->counter >= 10 ? TASK_END : 0;
	}

	static uint32_t __cdecl BoneSmokeTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		Particle *arr = PartArray(n->f0E);
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		const int32_t id = n->f0E + 1;
		if (!(UpdateFlags() & 0x201) && n->counter <= 3)
		{
			for (int32_t j = 0; j < 2; j++)
			{
				const int32_t i = FindFree(arr);
				if (i < 0) break;
				Particle *e = &arr[i];
				e->id = id;
				e->frame = 0;
				e->size = (int16_t)(CrtRand() % 0x280 + 0x320);
				GetEffectSpawnPosition(Caster(), 0x1D, 0xC00, e->pos);
				e->pos[0] = (int16_t)(e->pos[0] + (CrtRand() % 0x32 - 0x19));
				e->pos[1] = (int16_t)(e->pos[1] + (CrtRand() % 0x32 - 0x19));
				e->pos[2] = (int16_t)(e->pos[2] + (CrtRand() % 0x32 - 0x19));
				Particle *e2 = (Particle *)((uint32_t)arr + 0x18 * (uint32_t)(i + 1));
				e2->id = id;
				e2->frame = 0;
				e2->size = (int16_t)(CrtRand() % 0x280 + 0x320);
				GetEffectSpawnPosition(Caster(), 0x1E, 0xC00, e2->pos);
				e2->pos[0] = (int16_t)(e2->pos[0] + (CrtRand() % 0x32 - 0x19));
				e2->pos[1] = (int16_t)(e2->pos[1] + (CrtRand() % 0x32 - 0x19));
				e2->pos[2] = (int16_t)(e2->pos[2] + (CrtRand() % 0x32 - 0x19));
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t alive = 0;
		SparkBegin(s, h, game_addr(SEQ_BoneSmoke));
		for (Particle *e = arr; e < arr + PARTS_N; e++)
		{
			if (e->id != id) continue;
			SparkDraw(e, BoneSmokePull(e), s, h);
			if (UpdateFlags() & 0x201) continue;
			e->frame++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				e->id = 0;
				continue;
			}
			e->size = (int16_t)(e->size + (int16_t)(e->size >> 4));
			alive++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return alive ? 0 : TASK_END;
	}

	static uint32_t __cdecl SprayTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		const ShotRec *rec = Shot(n->f0E);
		Particle *arr = PartArray(n->pos[3]);
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		const int32_t id = n->pos[3] + 1;
		if (!(UpdateFlags() & 0x201) && n->counter <= 5)
		{
			ff8fx::copy<(8)>(s, rec->pos);
			ff8fx::copy<(0x10)>(s + 0x28, rec->dir);
			ff8fx::copy<(0x20)>(s + 8, &rec->m);
			for (int32_t j = 0; j < 8; j++)
			{
				const int32_t i = FindFree(arr);
				if (i < 0) break;
				Particle *e = &arr[i];
				e->id = id;
				e->frame = 0;
				e->size = (int16_t)(CrtRand() % 0x180 + 0x300);
				const int32_t along = CrtRand() % *(const int32_t *)(s + 0x24);
				e->pos[0] = (int16_t)((mul32(*(const int32_t *)(s + 0x28), along) >> 12) + *(const int16_t *)s);
				e->pos[1] = (int16_t)((mul32(*(const int32_t *)(s + 0x2C), along) >> 12) + *(const int16_t *)(s + 2));
				e->pos[2] = (int16_t)((mul32(*(const int32_t *)(s + 0x30), along) >> 12) + *(const int16_t *)(s + 4));
				const int32_t out = CrtRand() % *(const int32_t *)(s + 0x1C);
				int32_t *v = (int32_t *)(s + 0x38);
				v[0] = CrtRand() % 0x1000 - 0x800;
				v[1] = CrtRand() % 0x1000 - 0x800;
				v[2] = CrtRand() % 0x680 - 0x340;
				Normalize(v, v);
				TransformVectorBy3x3Matrix((const Mat4x3 *)(s + 8), v, v);
				e->pos[0] = (int16_t)(e->pos[0] + (mul32(out, v[0]) >> 12));
				e->pos[1] = (int16_t)(e->pos[1] + (mul32(out, v[1]) >> 12));
				e->pos[2] = (int16_t)(e->pos[2] + (mul32(out, v[2]) >> 12));
				const int32_t sp = CrtRand() % 0x55 + 0xF;
				e->vel[0] = (int16_t)(mul32(sp, v[0]) >> 12);
				e->vel[1] = (int16_t)(mul32(sp, v[1]) >> 12);
				e->vel[2] = (int16_t)(mul32(sp, v[2]) >> 12);
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t alive = 0;
		SparkBegin(s, h, game_addr(SEQ_Spark));
		for (Particle *e = arr; e < arr + PARTS_N; e++)
		{
			if (e->id != id) continue;
			SparkDraw(e, n->pos[3], s, h);
			if (UpdateFlags() & 0x201) continue;
			e->frame++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				e->id = 0;
				continue;
			}
			e->pos[0] = (int16_t)(e->pos[0] + e->vel[0]);
			e->pos[1] = (int16_t)(e->pos[1] + e->vel[1]);
			e->pos[2] = (int16_t)(e->pos[2] + e->vel[2]);
			alive++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return alive ? 0 : TASK_END;
	}

	static void ImpactFrame(const Node *n, uint8_t *s)
	{
		const ShotRec *rec = Shot(n->f0E);
		ff8fx::copy<(8)>(s, n->pos);
		const int32_t k = -(int32_t)n->pos[3];
		int16_t *p = (int16_t *)s;
		p[0] = (int16_t)(p[0] + (mul32(k, rec->dir[0]) >> 12));
		p[1] = (int16_t)(p[1] + (mul32(rec->dir[1], k) >> 12));
		p[2] = (int16_t)(p[2] + (mul32(rec->dir[2], k) >> 12));
		const int32_t angle = RotationBetweenVectors(DIR_BACK(), rec->dir, (int32_t *)(s + 0x28));
		BuildAxisAngleRotationMatrix(angle, (Mat4x3 *)(s + 8), (const int32_t *)(s + 0x28));
	}

	static uint32_t __cdecl ImpactSparkTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		Particle *arr = PartArray(n->f20);
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		const int32_t id = n->f20 + 1;
		if (!(UpdateFlags() & 0x201) && n->counter <= 0xC)
		{
			ImpactFrame(n, s);
			for (int32_t j = 0; j < 8; j++)
			{
				const int32_t i = FindFree(arr);
				if (i < 0) break;
				Particle *e = &arr[i];
				e->id = id;
				e->frame = 0;
				e->size = (int16_t)(CrtRand() % 0x280 + 0x400);
				ff8fx::copy<(8)>(e->pos, s);
				int32_t *v = (int32_t *)(s + 0x38);
				v[0] = CrtRand() % 0x1000 - 0x800;
				v[1] = CrtRand() % 0x1000 - 0x800;
				v[2] = -0x80 - CrtRand() % 0x400;
				Normalize(v, v);
				TransformVectorBy3x3Matrix((const Mat4x3 *)(s + 8), v, v);
				const int32_t r = CrtRand() % 0x258 + 0x12C;
				e->pos[0] = (int16_t)(e->pos[0] + (mul32(r, v[0]) >> 12));
				e->pos[1] = (int16_t)(e->pos[1] + (mul32(r, v[1]) >> 12));
				e->pos[2] = (int16_t)(e->pos[2] + (mul32(r, v[2]) >> 12));
				const int32_t sp = CrtRand() % 0x12C + 0xC8;
				e->vel[0] = (int16_t)(mul32(sp, v[0]) >> 12);
				e->vel[1] = (int16_t)(mul32(sp, v[1]) >> 12);
				e->vel[2] = (int16_t)(mul32(sp, v[2]) >> 12);
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t alive = 0;
		SparkBegin(s, h, game_addr(SEQ_Spark));
		for (Particle *e = arr; e < arr + PARTS_N; e++)
		{
			if (e->id != id) continue;
			SparkDraw(e, n->pos[3], s, h);
			if (UpdateFlags() & 0x201) continue;
			e->frame++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				e->id = 0;
				continue;
			}
			MoveDecay(e);
			alive++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return alive ? 0 : TASK_END;
	}

	static uint32_t __cdecl ImpactSmokeTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		Particle *arr = PartArray(n->f20);
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		const int32_t id = n->f20 + 1;
		if (!(UpdateFlags() & 0x201) && n->counter <= 0xC)
		{
			ImpactFrame(n, s);
			for (int32_t j = 0; j < 2; j++)
			{
				const int32_t i = FindFree(arr);
				if (i < 0) break;
				Particle *e = &arr[i];
				e->id = id;
				e->frame = 0;
				e->size = (int16_t)(CrtRand() % 0x1100 + 0x680);
				ff8fx::copy<(8)>(e->pos, s);
				int32_t *v = (int32_t *)(s + 0x38);
				v[0] = CrtRand() % 0x700 - 0x380;
				v[1] = CrtRand() % 0x700 - 0x380;
				v[2] = -0x400 - CrtRand() % 0x1200;
				Normalize(v, v);
				TransformVectorBy3x3Matrix((const Mat4x3 *)(s + 8), v, v);
				const int32_t r = CrtRand() % 0x12C + 0x64;
				e->pos[0] = (int16_t)(e->pos[0] + (mul32(r, v[0]) >> 12));
				e->pos[1] = (int16_t)(e->pos[1] + (mul32(r, v[1]) >> 12));
				e->pos[2] = (int16_t)(e->pos[2] + (mul32(r, v[2]) >> 12));
				const int32_t sp = CrtRand() % 0x15E + 0x64;
				e->vel[0] = (int16_t)(mul32(sp, v[0]) >> 12);
				e->vel[1] = (int16_t)(mul32(sp, v[1]) >> 12);
				e->vel[2] = (int16_t)(mul32(sp, v[2]) >> 12);
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		int32_t alive = 0;
		SparkBegin(s, h, game_addr(SEQ_Smoke));
		for (Particle *e = arr; e < arr + PARTS_N; e++)
		{
			if (e->id != id) continue;
			SmokeDraw(e, h);
			if (UpdateFlags() & 0x201) continue;
			e->frame++;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				e->id = 0;
				continue;
			}
			MoveDecay(e);
			alive++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		return alive ? 0 : TASK_END;
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
		if (n->counter == 0x16)
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
			const int32_t angle = RotationBetweenVectors(DIR_BACK(), dir, axis);
			BuildAxisAngleRotationMatrix(angle, &s->m, axis);
			GetDefaultEffectPosition(Entity(n->f1A), n->pos);
			if (n->pos[1] > s->pos[1]) n->pos[1] = s->pos[1];
			s->dir[0] = n->pos[0] - s->pos[0];
			s->dir[1] = n->pos[1] - s->pos[1];
			s->dir[2] = n->pos[2] - s->pos[2];
			const int32_t dist = Sqrt(Normalize(s->dir, s->dir));
			uint8_t *t = Entity(n->f1A);
			n->pos[3] = (int16_t)(mul32(*(const int16_t *)(t + 0x26), 2000) >> 12);
			n->f1C = (int16_t)(mul32(ClampedSize(t), 2000) >> 12);
			n->f1E = *(const int16_t *)(Entity(n->f1A) + 0x3C) > -500 ? 1 : 0;
			Node *f = AddTask(QAux(), game_addr(ORIG_MuzzleFlashTask));
			f->counter = 0;
			f->f0E = n->f0E;
			f->f18 = (int16_t)(CrtRand() % 0x1000);
			const int16_t third = (int16_t)((CrtRand() % 0x380 + 0xC80) / 3);
			f->f1E = third;
			f->f1C = third;
			Node *b = AddTask(QAux(), game_addr(ORIG_BoneSmokeTask));
			const int32_t num = IdCount();
			b->f0E = (int16_t)num;
			IdCount() = num + 1;
			b->counter = 0;
			Node *m = AddTask(QAux(), game_addr(ORIG_BeamTask));
			m->f0E = n->f0E;
			m->counter = 0;
			m->f1C = 0x3400;
			m->f1E = 0x14C;
			const int16_t half = (int16_t)(dist / 2);
			m->f22 = half;
			m->f20 = half;
			Node *p = AddTask(QAux(), game_addr(ORIG_SprayTask));
			p->f0E = n->f0E;
			const int32_t num2 = IdCount();
			p->pos[3] = (int16_t)num2;
			IdCount() = num2 + 1;
			p->counter = 0;
		}
		if (n->counter >= 0 && n->counter <= 8 && n->counter % 3 == 0)
		{
			Node *c = AddTask(QAux(), game_addr(ORIG_MuzzleConeTask));
			c->counter = 0;
			c->f0E = n->f0E;
			c->f18 = (int16_t)(CrtRand() % 0x1000);
			c->f1A = (int16_t)(CrtRand() % 0x96 + 0x6E);
			const int16_t third = (int16_t)((CrtRand() % 0x380 + 0xC80) / 3);
			c->f1E = third;
			c->f1C = third;
		}
		if (n->counter == 2)
		{
			Node *k = AddTask(QAux(), game_addr(ORIG_ImpactSparkTask));
			k->f0E = n->f0E;
			ff8fx::copy<(8)>(k->pos, n->pos);
			const int32_t num = IdCount();
			k->f20 = (int16_t)num;
			IdCount() = num + 1;
			k->counter = 0;
			Node *g = AddTask(QAux(), game_addr(ORIG_ImpactSmokeTask));
			ff8fx::copy<(4)>(g->pos, n->pos);
			g->f0E = n->f0E;
			const int32_t num2 = IdCount();
			g->f20 = (int16_t)num2;
			IdCount() = num2 + 1;
			g->counter = 0;
			ff8fx::copy<(4)>(g->pos + 2, n->pos + 2);
		}
		if (n->counter >= 2 && n->counter <= 0xA && n->counter % 3 == 0 && n->f1E != 0)
		{
			Node *r = AddTask(QAux(), game_addr(ORIG_GroundRingTask));
			ff8fx::copy<(8)>(r->pos, n->pos);
			const int32_t k = ((int32_t)n->f1C << 12) / 2200;
			r->f0E = n->f0E;
			r->counter = 0;
			const int16_t third = (int16_t)((mul32(CrtRand() % 0x380 + 0xA00, k) >> 12) / 3);
			r->f1E = third;
			r->f1C = third;
		}
		if (n->counter == 0xA) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		if (n->counter == 1) BdPlaySE(SOUND_Gun(), (n->f0E & 1) ? 1 : 2, 0x80);
		n->counter++;
		return n->counter >= 25 ? TASK_END : 0;
	}

	static uint32_t __cdecl FadeOutTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		SetScreenFlash(n->f18 - n->f18 / n->f0E * n->counter, 0);
		if (UpdateFlags() & 0x201) return 0;
		n->counter++;
		if (n->counter < n->f0E) return 0;
		SetScreenFlash(0, 0);
		return TASK_END;
	}

	static uint32_t __cdecl FadeInTask(TaskNode *n_)
	{
		Node *n = (Node *)n_;
		SetScreenFlash(n->f18 / n->f0E * n->counter, 0);
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
		if (n->counter == 0xA) ApplyActionResultToTargets(Ctx()->actions[0].targets, Ctx()->actions[0].target_count);
		n->counter++;
		if (n->counter < 0xE) return 0;
		Flags() |= 2;
		Ctx()->done = 1;
		return TASK_END;
	}

	static uint32_t __cdecl RootTask(TaskNode *n_)
	{
		RootNode *r = (RootNode *)n_;
		PacketCursor() = (*(const uint8_t *)&r->counter & 1) ? TexBase() + ARENA_ODD : TexBase();
		const int shots = ExecuteTaskQueue(QShots());
		const int aux = ExecuteTaskQueue(QAux());
		if (!(UpdateFlags() & 0x201)) ModuleTick()++;
		r->counter++;
		return (aux == 0 && shots == 0 && (Flags() & 2)) ? TASK_END : 0;
	}
}

	void register_mag198_hyper_shot()
	{
		register_port(hyper198::ORIG_RootTask, (void *)hyper198::RootTask, "198 RootTask", 198);
		register_port(hyper198::ORIG_StartTask, (void *)hyper198::StartTask, "198 StartTask", 198);
		register_port(hyper198::ORIG_ShotTask, (void *)hyper198::ShotTask, "198 ShotTask", 198);
		register_port(hyper198::ORIG_EndTask, (void *)hyper198::EndTask, "198 EndTask", 198);
		register_port(hyper198::ORIG_FadeOutTask, (void *)hyper198::FadeOutTask, "198 FadeOutTask", 198);
		register_port(hyper198::ORIG_FadeInTask, (void *)hyper198::FadeInTask, "198 FadeInTask", 198);
		register_port(hyper198::ORIG_AfterimageTask, (void *)hyper198::AfterimageTask, "198 AfterimageTask", 198);
		register_port(hyper198::ORIG_MuzzleFlashTask, (void *)hyper198::MuzzleFlashTask, "198 MuzzleFlashTask", 198);
		register_port(hyper198::ORIG_BoneSmokeTask, (void *)hyper198::BoneSmokeTask, "198 BoneSmokeTask", 198);
		register_port(hyper198::ORIG_BeamTask, (void *)hyper198::BeamTask, "198 BeamTask", 198);
		register_port(hyper198::ORIG_SprayTask, (void *)hyper198::SprayTask, "198 SprayTask", 198);
		register_port(hyper198::ORIG_MuzzleConeTask, (void *)hyper198::MuzzleConeTask, "198 MuzzleConeTask", 198);
		register_port(hyper198::ORIG_ImpactSparkTask, (void *)hyper198::ImpactSparkTask, "198 ImpactSparkTask", 198);
		register_port(hyper198::ORIG_ImpactSmokeTask, (void *)hyper198::ImpactSmokeTask, "198 ImpactSmokeTask", 198);
		register_port(hyper198::ORIG_GroundRingTask, (void *)hyper198::GroundRingTask, "198 GroundRingTask", 198);
	}
}

