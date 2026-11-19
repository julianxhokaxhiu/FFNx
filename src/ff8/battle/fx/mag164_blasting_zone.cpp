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

// Effect 164: Blasting Zone

#include "mag_common.h"

namespace ff8fx
{
namespace blast164
{
	using namespace eng;
	using namespace magc;

	inline int32_t &TargetSlot() { return var<int32_t>(game_addr(game::MAG_164_BLASTING_ZONE_TargetSlot)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::MAG_164_BLASTING_ZONE_CasterSlot)); }
	inline CastContext *&Context() { return var<CastContext *>(game_addr(game::MAG_164_BLASTING_ZONE_CastContext)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_164_BLASTING_ZONE_TextureBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_164_BLASTING_ZONE_PacketCursor)); }
	inline uint32_t &Colour() { return var<uint32_t>(game_addr(game::MAG_164_BLASTING_ZONE_PulseColour)); }
	inline uint32_t &FrameArena() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::MAG_164_BLASTING_ZONE_RootQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_164_BLASTING_ZONE_MainQueue); }
	static constexpr game::Id POOL_A = game::MAG_164_BLASTING_ZONE_SparkPool, POOL_A_END = game::MAG_164_BLASTING_ZONE_ShardPool;
	static constexpr game::Id POOL_B = game::MAG_164_BLASTING_ZONE_ShardPool, POOL_B_END = game::MAG_164_BLASTING_ZONE_StarPool;
	static constexpr game::Id POOL_C = game::MAG_164_BLASTING_ZONE_StarPool, POOL_C_END = game::MAG_164_BLASTING_ZONE_AnimBindBlock;
	static constexpr game::Id BIND_OUT = game::MAG_164_BLASTING_ZONE_AnimBindBlock;
	static constexpr game::Id FRAME = game::MAG_164_BLASTING_ZONE_EffectFrame;
	static constexpr game::Id CAM_FRAME = game::MAG_164_BLASTING_ZONE_CameraEffectFrame;
	static constexpr game::Id POINT = game::MAG_164_BLASTING_ZONE_BladeTipPoint;
	static constexpr game::Id MORPH_MAT = game::MAG_164_BLASTING_ZONE_MorphMatrix;
	static constexpr game::Id MORPH_VTX = game::MAG_164_BLASTING_ZONE_MorphVertices;

	static constexpr game::Id ORIG_Root = game::MAG_164_BLASTING_ZONE_RootTask, ORIG_Director = game::MAG_164_BLASTING_ZONE_DirectorTask, ORIG_FlareA = game::MAG_164_BLASTING_ZONE_FlareATask, ORIG_FlareB = game::MAG_164_BLASTING_ZONE_FlareBTask, ORIG_OrbitSpark = game::MAG_164_BLASTING_ZONE_OrbitSparkTask, ORIG_Swirl = game::MAG_164_BLASTING_ZONE_SwirlTask, ORIG_Ring = game::MAG_164_BLASTING_ZONE_RingTask, ORIG_Spawner = game::MAG_164_BLASTING_ZONE_PulseSpawnerTask, ORIG_PulseA = game::MAG_164_BLASTING_ZONE_PulseATask, ORIG_PulseB = game::MAG_164_BLASTING_ZONE_PulseBTask, ORIG_Shard = game::MAG_164_BLASTING_ZONE_ShardTask, ORIG_Ball = game::MAG_164_BLASTING_ZONE_BallTask, ORIG_FrameModel = game::MAG_164_BLASTING_ZONE_FrameModelTask, ORIG_Dome = game::MAG_164_BLASTING_ZONE_DomeTask, ORIG_Dome2 = game::MAG_164_BLASTING_ZONE_Dome2Task, ORIG_Beam = game::MAG_164_BLASTING_ZONE_BeamTask, ORIG_Rock = game::MAG_164_BLASTING_ZONE_RockTask, ORIG_Debris = game::MAG_164_BLASTING_ZONE_DebrisTask, ORIG_Starfield = game::MAG_164_BLASTING_ZONE_StarfieldTask, ORIG_ScreenFlash = game::MAG_164_BLASTING_ZONE_ScreenFlashTask, ORIG_Morph = game::MAG_164_BLASTING_ZONE_MorphTask, ORIG_RingFlat = game::MAG_164_BLASTING_ZONE_RingFlatTask, ORIG_RingFlat2 = game::MAG_164_BLASTING_ZONE_RingFlat2Task, ORIG_MorphSpark = game::MAG_164_BLASTING_ZONE_MorphSparkTask, ORIG_BladeGlow = game::MAG_164_BLASTING_ZONE_BladeGlowTask, ORIG_BurstSpark = game::MAG_164_BLASTING_ZONE_BurstSparkTask, ORIG_Wave = game::MAG_164_BLASTING_ZONE_WaveTask, ORIG_Wave2 = game::MAG_164_BLASTING_ZONE_Wave2Task, ORIG_Smoke = game::MAG_164_BLASTING_ZONE_SmokeTask, ORIG_Shake = game::MAG_164_BLASTING_ZONE_CameraShakeTask, ORIG_Restore = game::MAG_164_BLASTING_ZONE_RestoreTextureTask;

	static constexpr game::Id ANIM_DATA = game::MAG_164_BLASTING_ZONE_AnimSet, CAMERA_SCRIPT = game::MAG_164_BLASTING_ZONE_CameraScript;
	static const void *SOUND_A() { return (const void *)game_addr(game::MAG_164_BLASTING_ZONE_Sound1); }
	static const void *SOUND_B() { return (const void *)game_addr(game::MAG_164_BLASTING_ZONE_Sound2); }
	static constexpr game::Id GLOW_REF = game::MAG_164_BLASTING_ZONE_GlowRefDir;
	static constexpr game::Id DOT_COLOURS = game::MAG_164_BLASTING_ZONE_DotColours;
	static constexpr game::Id SEQ_ORBIT = game::MAG_164_BLASTING_ZONE_OrbitSeq, SEQ_BURST = game::MAG_164_BLASTING_ZONE_BurstSeq, SEQ_SMOKE = game::MAG_164_BLASTING_ZONE_SmokeSeq;
	static constexpr game::Id MODEL_FLAREA = game::unk_D8F8B8, MODEL_FLAREB = game::unk_D909F0, MODEL_SWIRL = game::unk_D91AD0, MODEL_RING = game::unk_D94FAC, MODEL_PULSE = game::unk_D948C4, MODEL_SHARD = game::unk_D95904, MODEL_BALL = game::unk_D90F00, MODEL_FRAME = game::unk_D92154, MODEL_DOME = game::unk_D8C130, MODEL_BEAM = game::unk_D9339C, MODEL_ROCK = game::unk_D8B180, MODEL_DEBRIS = game::unk_D8A048, MODEL_FLASH = game::unk_D8B880, MODEL_MORPH = game::unk_D8D918, MODEL_RINGFLAT = game::unk_D8B610, MODEL_RINGFLAT2 = game::unk_D95B7C, MODEL_GLOW = game::unk_D8EAA8, MODEL_WAVE = game::unk_D97D6C, MODEL_WAVE2 = game::unk_D95DEC;
	static constexpr game::Id MORPH_A = game::unk_D8E410, MORPH_B = game::unk_D8E760;
	static constexpr game::Id MORPH_COUNT = game::unk_D8D91C;

	inline int32_t PauseQuery() { return fn<int32_t (__cdecl *)()>(game_addr(game::BattleFile_GetLoadingState))(); }
	inline void CameraScriptStart(uint32_t script, uint32_t frame, void *entity, TaskQueue *q, int32_t n) { fn<void (__cdecl *)(uint32_t, uint32_t, void *, TaskQueue *, int32_t)>(game_addr(game::MAG066_CamScript_Start))(script, frame, entity, q, n); }
	inline void ArmCameraReturn() { fn<void (__cdecl *)()>(game_addr(game::BS_Camera_ArmReturnAfterEffect))(); }
	inline void BindEntityModelFromComData(uint8_t *entity, uint32_t out, uint32_t data) { fn<void (__cdecl *)(uint8_t *, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline void QueueChainTransformation(uint8_t *entity, int32_t seq) { fn<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, seq); }
	inline void CharacterLoad(int32_t id, uint32_t dst) { fn<void (__cdecl *)(int32_t, uint32_t)>(game_addr(game::BattleFile_CharacterLoad))(id, dst); }
	inline void QueueTIMUpload(uint32_t tim) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::Battle_QueueTIMUpload_GetEOF))(tim); }
	inline int32_t Normalize(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline void NormalizeS16(const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const int16_t *, int16_t *)>(game_addr(game::VectorNormalS16))(in, out); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline void MatVec(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void MatrixMultiply3x3(Mat4x3 *a, const Mat4x3 *b) { fn<void (__cdecl *)(Mat4x3 *, const Mat4x3 *)>(game_addr(game::MatrixMultiply3x3))(a, b); }
	inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
	inline void GteLoadRGB012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadRGB012))(a, b, c); }
	inline void GteDPCT() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCT))(); }
	inline void GteStoreRGB012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_StoreRGB012))(a, b, c); }
	inline void GteLoadV012(const void *v0, const void *v1, const void *v2) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(v0, v1, v2); }
	inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
	inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
	inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
	inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
	inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
	inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
	inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
	inline void GteStoreOTZ(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
	inline void GteReadSZ3(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadSZ3))(dst); }
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteSetBackColorFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
	inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
	inline void GteSetTransFromVec32(const int32_t *v) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_SetTransFromVec32))(v); }
	inline void GteTransFromMAC() { fn<void (__cdecl *)()>(game_addr(game::GTE_TransFromMAC))(); }
	inline uint32_t Mag151Render(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_151_CannonBlow_ScrollRender))(h, ot, mode, cursor); }
	inline void InsertPrimAltViewport(uint32_t bucket, void *packet) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(bucket, packet); }
	inline uint32_t OT0() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t c;
		int16_t pad0E;
	};
	struct Node24
	{
		TaskNode hdr;
		int16_t c;
		int16_t e;
		int16_t w10, w12, w14, w16;
		int16_t w18;
		int16_t w1A;
		int16_t w1C;
		int16_t w1E;
		int16_t w20;
		int16_t w22;
	};
	struct PoolE
	{
		uint32_t flags;
		int16_t life;
		int16_t scale;
		int16_t a[8];
	};
	struct RenderHeader
	{
		uint32_t model;
		uint32_t verts;
		uint8_t far_rgb[4];
		int32_t fade;
		uint16_t u_off;
		uint16_t pad12;
		uint16_t v_off;
		uint16_t pad16;
		int32_t depth;
		uint32_t flags;
		uint32_t colour;
		uint32_t list;
		int32_t mac0;
		uint32_t pad2C;
		int32_t otz;
		uint32_t gte_flag;
		uint8_t pad38[0x20];
		uint32_t scale_rgb;
		uint32_t c[4];
	};
	struct OrbitWork
	{
		int16_t pos[4];
		Mat4x3 m1;
		Mat4x3 m2;
		int32_t v[4];
		int32_t axis[4];
	};
	struct ModelWork
	{
		int16_t ang[4];
		Mat4x3 m1;
		Mat4x3 m2;
		int32_t v[4];
		int32_t axis[4];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node24) == 0x24 && sizeof(PoolE) == 0x18, "Blasting Zone nodes");
	static_assert(sizeof(RenderHeader) == 0x6C && sizeof(OrbitWork) == 0x68 && sizeof(ModelWork) == 0x68, "Blasting Zone blocks");

	template<typename T> static inline T &At(uint32_t base, int32_t off) { return *(T *)(base + off); }
	inline uint8_t *Caster() { return Entity(CasterSlot()); }
	inline Mat4x3 *CamFrame() { return (Mat4x3 *)game_addr(CAM_FRAME); }
	inline int16_t PointX() { return var<int16_t>(game_addr(POINT)); }
	inline int16_t PointZ() { return var<int16_t>(game_addr(POINT) + 4); }
	inline PoolE *Entry(uint32_t pool, int32_t i) { return (PoolE *)(pool + 0x18 * i); }

	static int32_t FreeSlot(uint32_t pool, uint32_t end, int32_t max)
	{
		int32_t i = 0;
		for (uint32_t a = pool; *(const uint32_t *)a != 0; i++)
		{
			a += 0x18;
			if (a >= end) return -1;
		}
		return i >= max ? -1 : i;
	}

	static void ClearType(uint32_t pool, uint32_t end, uint32_t type)
	{
		for (uint32_t a = pool; a < end; a += 0x18)
			if (*(const uint8_t *)a & type) *(uint32_t *)a = 0;
	}

	static inline uint32_t EndAt(int16_t c, int16_t limit) { return c >= limit ? TASK_END : 0; }
}
}

namespace ff8fx
{
namespace blast164
{
	static inline bool OutX(uint32_t xy) { int16_t v = *(const int16_t *)xy; return v < 0 || v > 0xA00; }
	static inline bool OutY(uint32_t xy) { int16_t v = *(const int16_t *)(xy + 2); return v < 0 || v > 0x6C0; }

	static inline void UvOffsets(const RenderHeader *h, uint32_t pkt, uint32_t u_word, uint32_t v_word)
	{
		const uint32_t fl = h->flags;
		if (fl & 0x400) At<uint16_t>(pkt, u_word) = (uint16_t)(At<uint16_t>(pkt, u_word) + h->u_off);
		else if (fl & 0x100) At<uint16_t>(pkt, u_word) = h->u_off;
		if (fl & 0x800) At<uint16_t>(pkt, v_word) = (uint16_t)(At<uint16_t>(pkt, v_word) + h->v_off);
		else if (fl & 0x200) At<uint16_t>(pkt, v_word) = h->v_off;
	}

	static inline void ScaleColours(RenderHeader *h, int n)
	{
		h->scale_rgb = h->colour;
		uint8_t *s = (uint8_t *)&h->scale_rgb;
		for (int k = 0; k < n; k++)
		{
			uint8_t *c = (uint8_t *)&h->c[k];
			c[0] = (uint8_t)((uint32_t)c[0] * s[0] >> 7);
			c[1] = (uint8_t)((uint32_t)c[1] * s[1] >> 7);
			c[2] = (uint8_t)((uint32_t)c[2] * s[2] >> 7);
		}
	}

	static inline uint32_t Bucket(uint32_t ot, const RenderHeader *h, int32_t shift) { return ot + (uint32_t)(h->otz >> shift) * 4; }

	static inline void FlatDepthCue(RenderHeader *h, uint32_t pkt)
	{
		if (h->flags & 0x40)
		{
			GteLoadRGBC((const void *)(pkt + 4));
			GteSetIR0(h->fade);
			GteDPCS();
			GteStoreRGB2((void *)(pkt + 4));
		}
	}

	static uint32_t ListF3(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			const uint32_t code = h->colour | 0x20000000;
			const uint32_t fl = h->flags;
			At<uint32_t>(pkt, 0) = 0x4000000;
			At<uint32_t>(pkt, 4) = code;
			if (fl & 1) At<uint32_t>(pkt, 4) = code | 0x2000000;
			if (fl & 4) At<uint32_t>(pkt, 4) &= 0xFDFFFFFF;
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x10)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0xC), (void *)(pkt + 0x10));
					GteAVSZ3();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0xC)) clip |= 2;
					if (OutX(pkt + 0x10)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0xC)) clip |= 0x20;
					if (OutY(pkt + 0x10)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						GteStoreOTZ(&h->otz);
						FlatDepthCue(h, pkt);
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x14;
					}
				}
			}
			rec += 0xC;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListF4(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			const uint32_t code = h->colour | 0x28000000;
			const uint32_t fl = h->flags;
			At<uint32_t>(pkt, 0) = 0x5000000;
			At<uint32_t>(pkt, 4) = code;
			if (fl & 1) At<uint32_t>(pkt, 4) = code | 0x2000000;
			if (fl & 4) At<uint32_t>(pkt, 4) &= 0xFDFFFFFF;
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x10)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0xC), (void *)(pkt + 0x10));
					GteLoadV0((const void *)(vb + (uint32_t)At<uint16_t>(rec, 0xA) * 4));
					GteRTPS();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0xC)) clip |= 2;
					if (OutX(pkt + 0x10)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0xC)) clip |= 0x20;
					if (OutY(pkt + 0x10)) clip |= 0x40;
					GteReadSXY2((void *)(pkt + 0x14));
					GteAVSZ4();
					if (OutX(pkt + 0x14)) clip |= 8;
					if (OutY(pkt + 0x14)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						GteStoreOTZ(&h->otz);
						FlatDepthCue(h, pkt);
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x18;
					}
				}
			}
			rec += 0xC;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListFT3(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			uint32_t code = h->colour | 0x24000000;
			At<uint32_t>(pkt, 0) = 0x7000000;
			At<uint32_t>(pkt, 4) = code;
			if (h->flags & 1) At<uint32_t>(pkt, 4) = code | 0x2000000;
			if (h->flags & 4) At<uint32_t>(pkt, 4) &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			At<uint32_t>(pkt, 0x14) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x1C) = (At<uint32_t>(rec, 8) >> 16) + d;
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x16, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x10)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x10), (void *)(pkt + 0x18));
					GteAVSZ3();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x10)) clip |= 2;
					if (OutX(pkt + 0x18)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x10)) clip |= 0x20;
					if (OutY(pkt + 0x18)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						GteStoreOTZ(&h->otz);
						FlatDepthCue(h, pkt);
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x20;
					}
				}
			}
			rec += 0x14;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListFT4(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			uint32_t code = h->colour | 0x2C000000;
			At<uint32_t>(pkt, 0) = 0x9000000;
			At<uint32_t>(pkt, 4) = code;
			if (h->flags & 1) At<uint32_t>(pkt, 4) = code | 0x2000000;
			if (h->flags & 4) At<uint32_t>(pkt, 4) &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			const uint32_t uv23 = (d << 16) + d + At<uint32_t>(rec, 0x14);
			At<uint32_t>(pkt, 0x1C) = uv23;
			At<uint32_t>(pkt, 0x14) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x24) = uv23 >> 16;
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x16, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x10)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x10), (void *)(pkt + 0x18));
					GteLoadV0((const void *)(vb + (uint32_t)At<uint16_t>(rec, 0xA) * 4));
					GteRTPS();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x10)) clip |= 2;
					if (OutX(pkt + 0x18)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x10)) clip |= 0x20;
					if (OutY(pkt + 0x18)) clip |= 0x40;
					GteReadSXY2((void *)(pkt + 0x20));
					GteAVSZ4();
					if (OutX(pkt + 0x20)) clip |= 8;
					if (OutY(pkt + 0x20)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						GteStoreOTZ(&h->otz);
						FlatDepthCue(h, pkt);
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x28;
					}
				}
			}
			rec += 0x18;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListG3(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			const uint32_t fl = h->flags;
			const uint32_t code = At<uint32_t>(rec, 0);
			At<uint32_t>(pkt, 0) = 0x6000000;
			h->c[0] = code;
			if (fl & 2) h->c[0] = code | 0x2000000;
			if (fl & 8) h->c[0] &= 0xFDFFFFFF;
			h->c[1] = At<uint32_t>(rec, 0xC);
			h->c[2] = At<uint32_t>(rec, 0x10);
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x20)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x10), (void *)(pkt + 0x18));
					GteAVSZ3();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x10)) clip |= 2;
					if (OutX(pkt + 0x18)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x10)) clip |= 0x20;
					if (OutY(pkt + 0x18)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						GteStoreOTZ(&h->otz);
						ScaleColours(h, 3);
						if (h->flags & 0x80)
						{
							GteLoadRGB012(&h->c[1], &h->c[2], &h->c[0]);
							GteSetIR0(h->fade);
							GteDPCT();
							GteStoreRGB012((void *)(pkt + 0xC), (void *)(pkt + 0x14), (void *)(pkt + 4));
						}
						else
						{
							At<uint32_t>(pkt, 4) = h->c[0];
							At<uint32_t>(pkt, 0xC) = h->c[1];
							At<uint32_t>(pkt, 0x14) = h->c[2];
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x1C;
					}
				}
			}
			rec += 0x14;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListG4(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			const uint32_t fl = h->flags;
			const uint32_t code = At<uint32_t>(rec, 0);
			At<uint32_t>(pkt, 0) = 0x8000000;
			h->c[0] = code;
			if (fl & 2) h->c[0] = code | 0x2000000;
			if (fl & 8) h->c[0] &= 0xFDFFFFFF;
			h->c[1] = At<uint32_t>(rec, 0xC);
			h->c[2] = At<uint32_t>(rec, 0x10);
			h->c[3] = At<uint32_t>(rec, 0x14);
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x20)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x10), (void *)(pkt + 0x18));
					GteLoadV0((const void *)(vb + (uint32_t)At<uint16_t>(rec, 0xA) * 4));
					GteRTPS();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x10)) clip |= 2;
					if (OutX(pkt + 0x18)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x10)) clip |= 0x20;
					if (OutY(pkt + 0x18)) clip |= 0x40;
					GteReadSXY2((void *)(pkt + 0x20));
					GteAVSZ4();
					if (OutX(pkt + 0x20)) clip |= 8;
					if (OutY(pkt + 0x20)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						GteStoreOTZ(&h->otz);
						ScaleColours(h, 4);
						if (h->flags & 0x80)
						{
							GteLoadRGB012(&h->c[1], &h->c[2], &h->c[3]);
							GteSetIR0(h->fade);
							GteDPCT();
							GteStoreRGB012((void *)(pkt + 0xC), (void *)(pkt + 0x14), (void *)(pkt + 0x1C));
							GteLoadRGBC(&h->c[0]);
							GteDPCS();
							GteStoreRGB2((void *)(pkt + 4));
						}
						else
						{
							At<uint32_t>(pkt, 4) = h->c[0];
							At<uint32_t>(pkt, 0xC) = h->c[1];
							At<uint32_t>(pkt, 0x14) = h->c[2];
							At<uint32_t>(pkt, 0x1C) = h->c[3];
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x24;
					}
				}
			}
			rec += 0x18;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListGT3(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			const uint32_t code = At<uint32_t>(rec, 0);
			At<uint32_t>(pkt, 0) = 0x9000000;
			h->c[0] = code;
			if (h->flags & 2) h->c[0] = code | 0x2000000;
			if (h->flags & 8) h->c[0] &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			At<uint32_t>(pkt, 0x18) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x24) = (At<uint32_t>(rec, 8) >> 16) + d;
			h->c[1] = At<uint32_t>(rec, 0x14);
			h->c[2] = At<uint32_t>(rec, 0x18);
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x1A, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x20)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x14), (void *)(pkt + 0x20));
					GteAVSZ3();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x14)) clip |= 2;
					if (OutX(pkt + 0x20)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x14)) clip |= 0x20;
					if (OutY(pkt + 0x20)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						GteStoreOTZ(&h->otz);
						ScaleColours(h, 3);
						if (h->flags & 0x80)
						{
							GteLoadRGB012(&h->c[1], &h->c[2], &h->c[0]);
							GteSetIR0(h->fade);
							GteDPCT();
							GteStoreRGB012((void *)(pkt + 0x10), (void *)(pkt + 0x1C), (void *)(pkt + 4));
						}
						else
						{
							At<uint32_t>(pkt, 4) = h->c[0];
							At<uint32_t>(pkt, 0x10) = h->c[1];
							At<uint32_t>(pkt, 0x1C) = h->c[2];
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x28;
					}
				}
			}
			rec += 0x1C;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t ListGT4(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t list = h->list;
		int32_t count = At<int32_t>(list, 0);
		uint32_t rec = list + 4;
		h->list = rec;
		const uint32_t vb = h->verts;
		if (count <= 0)
		{
			h->list = rec;
			return cursor;
		}
		uint32_t pkt = cursor;
		do
		{
			GteLoadV012((const void *)(vb + (uint32_t)At<uint16_t>(rec, 4) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 6) * 4), (const void *)(vb + (uint32_t)At<uint16_t>(rec, 8) * 4));
			GteRTPT();
			const uint32_t code = At<uint32_t>(rec, 0);
			At<uint32_t>(pkt, 0) = 0xC000000;
			h->c[0] = code;
			if (h->flags & 2) h->c[0] = code | 0x2000000;
			if (h->flags & 8) h->c[0] &= 0xFDFFFFFF;
			const uint32_t d = (uint32_t)h->depth;
			At<uint32_t>(pkt, 0xC) = At<uint32_t>(rec, 0xC) + d;
			const uint32_t uv23 = (d << 16) + d + At<uint32_t>(rec, 0x14);
			At<uint32_t>(pkt, 0x18) = At<uint32_t>(rec, 0x10) + d;
			At<uint32_t>(pkt, 0x24) = uv23;
			At<uint32_t>(pkt, 0x30) = uv23 >> 16;
			h->c[1] = At<uint32_t>(rec, 0x18);
			h->c[2] = At<uint32_t>(rec, 0x1C);
			h->c[3] = At<uint32_t>(rec, 0x20);
			GteReadFLAG(&h->gte_flag);
			if (!(h->gte_flag & 0x60000))
			{
				GteNCLIP();
				UvOffsets(h, pkt, 0x1A, 0xE);
				GteReadMAC0(&h->mac0);
				const int32_t mac0 = h->mac0;
				if (mac0 != 0 && (mac0 > 0 || (h->flags & 0x20)))
				{
					GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x14), (void *)(pkt + 0x20));
					GteLoadV0((const void *)(vb + (uint32_t)At<uint16_t>(rec, 0xA) * 4));
					GteRTPS();
					uint32_t clip = OutX(pkt + 8) ? 1 : 0;
					if (OutX(pkt + 0x14)) clip |= 2;
					if (OutX(pkt + 0x20)) clip |= 4;
					if (OutY(pkt + 8)) clip |= 0x10;
					if (OutY(pkt + 0x14)) clip |= 0x20;
					if (OutY(pkt + 0x20)) clip |= 0x40;
					GteReadSXY2((void *)(pkt + 0x2C));
					GteAVSZ4();
					if (OutX(pkt + 0x2C)) clip |= 8;
					if (OutY(pkt + 0x2C)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						GteStoreOTZ(&h->otz);
						ScaleColours(h, 4);
						if (h->flags & 0x80)
						{
							GteLoadRGB012(&h->c[1], &h->c[2], &h->c[3]);
							GteSetIR0(h->fade);
							GteDPCT();
							GteStoreRGB012((void *)(pkt + 0x10), (void *)(pkt + 0x1C), (void *)(pkt + 0x28));
							GteLoadRGBC(&h->c[0]);
							GteDPCS();
							GteStoreRGB2((void *)(pkt + 4));
						}
						else
						{
							At<uint32_t>(pkt, 4) = h->c[0];
							At<uint32_t>(pkt, 0x10) = h->c[1];
							At<uint32_t>(pkt, 0x1C) = h->c[2];
							At<uint32_t>(pkt, 0x28) = h->c[3];
						}
						InsertPrimAutoDepth(Bucket(ot, h, shift), (void *)pkt);
						pkt += 0x34;
					}
				}
			}
			rec += 0x24;
		} while (--count);
		h->list = rec;
		return pkt;
	}

	static uint32_t RenderModel(RenderHeader *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t fl = h->flags;
		if (!(fl & 0x2000)) h->verts = h->model + 8;
		h->list = At<uint32_t>(h->model, 0) + h->model;
		if (!(fl & 0x1000)) h->depth = 0;
		GteSetFarColor(h->far_rgb[0], h->far_rgb[1], h->far_rgb[2]);
		typedef uint32_t (*List)(RenderHeader *, uint32_t, int32_t, uint32_t);
		static const List lists[8] = { ListF3, ListF4, ListFT3, ListFT4, ListG3, ListG4, ListGT3, ListGT4 };
		for (int i = 0; i < 8; i++)
		{
			if (At<uint32_t>(h->list, 0) != 0) cursor = lists[i](h, ot, shift, cursor);
			else h->list += 4;
		}
		return cursor;
	}

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

	static void SetTipPoint()
	{
		int32_t dir[4];
		WeaponBone(Caster(), 1, dir, (int16_t *)game_addr(POINT));
		var<int16_t>(game_addr(POINT)) = (int16_t)(var<int16_t>(game_addr(POINT)) + (int16_t)(mul32(dir[0], 380) >> 12));
		var<int16_t>(game_addr(POINT) + 2) = (int16_t)(var<int16_t>(game_addr(POINT) + 2) + (int16_t)(mul32(dir[1], 380) >> 12));
		var<int16_t>(game_addr(POINT) + 4) = (int16_t)(var<int16_t>(game_addr(POINT) + 4) + (int16_t)(mul32(dir[2], 380) >> 12));
	}

	static void StageGroupsHide()
	{
		for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) var<uint8_t>(a) |= 2;
	}
	static void StageGroupsShow()
	{
		for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) var<uint8_t>(a) &= 0xFD;
	}

	static Node24 *AddMain(uint32_t fn) { return (Node24 *)AddTaskToQueue(QMain(), fn); }

	static void AddCleared(uint32_t fn)
	{
		Node24 *t = AddMain(fn);
		t->c = 0;
		t->e = 0;
	}

	static void AddFlareA()
	{
		Node24 *t = AddMain(game_addr(ORIG_FlareA));
		*(uint32_t *)&t->w10 = var<uint32_t>(game_addr(POINT));
		t->c = 0;
		*(uint32_t *)&t->w14 = var<uint32_t>(game_addr(POINT) + 4);
		t->w18 = (int16_t)(CrtRand() % 0x1000);
		t->w1A = 0x1E;
		t->w1E = 0x3A0;
		t->w1C = 0x3A0;
	}

	static void AddFlareB()
	{
		Node24 *t = AddMain(game_addr(ORIG_FlareB));
		*(uint32_t *)&t->w10 = var<uint32_t>(game_addr(POINT));
		t->c = 0;
		*(uint32_t *)&t->w14 = var<uint32_t>(game_addr(POINT) + 4);
		t->w18 = (int16_t)(CrtRand() % 0x1000);
		t->w1A = 0x14;
		t->w1C = 0x1800;
	}

	static void AddRings()
	{
		for (int32_t i = 0; i < 4; i++)
		{
			Node24 *t = AddMain(game_addr(ORIG_Ring));
			t->c = 0;
			t->e = (int16_t)(CrtRand() % 8 + i);
			t->w10 = PointX();
			t->w12 = 0;
			t->w14 = PointZ();
			t->w18 = (int16_t)(CrtRand() % 0x1000);
			t->w1C = 0x1600;
			t->w1A = (int16_t)(-0x20 - CrtRand() % 0x48);
			const int16_t s = (int16_t)((CrtRand() % 0x1800 + 0xE00) / 16);
			t->w22 = s;
			t->w20 = s;
		}
	}

	static void AddBall()
	{
		Node24 *t = AddMain(game_addr(ORIG_Ball));
		*(uint32_t *)&t->w10 = var<uint32_t>(game_addr(POINT));
		*(uint32_t *)&t->w14 = var<uint32_t>(game_addr(POINT) + 4);
		t->c = 0;
		t->w16 = (int16_t)(mul32(EntitySize(CasterSlot()), 2000) >> 12);
		t->w1E = 0x80;
		t->w1C = 0x80;
	}

	static void AddFrameModel()
	{
		Node24 *t = AddMain(game_addr(ORIG_FrameModel));
		t->c = 0;
		t->w10 = 0;
		t->w14 = 0;
		t->w12 = -900;
		t->w1E = 0x300;
		t->w1C = 0x300;
	}

	static void AddDome()
	{
		Node24 *t = AddMain(game_addr(ORIG_Dome));
		t->c = 0;
		t->w10 = PointX();
		t->w12 = 0x12C;
		t->w14 = PointZ();
		t->w1C = 0x4000;
		t->w1E = 0x200;
		t->w20 = 0x2000;
	}

	static void AddDome2()
	{
		Node24 *t = AddMain(game_addr(ORIG_Dome2));
		t->c = 0;
		t->w12 = 0;
		t->w10 = PointX();
		t->w14 = PointZ();
		t->w1C = 0xE00;
		t->w20 = 0x2800;
		t->w22 = 0x1AA;
	}

	static void AddPillars()
	{
		Node24 *t = AddMain(game_addr(ORIG_Beam));
		t->c = 0;
		t->w10 = PointX();
		t->w12 = (int16_t)0xDCD8;
		t->w14 = PointZ();
		t->w18 = (int16_t)(CrtRand() % 0x1000);
		t->w1A = (int16_t)(CrtRand() % 0x1000);
		t->w1C = 0x3000;
		t->w20 = 0x3000;
		for (int32_t i = 0; i < 8; i++)
		{
			t = AddMain(game_addr(ORIG_Rock));
			t->c = 0;
			t->w10 = (int16_t)(CrtRand() % 0xD48 + var<int32_t>(game_addr(POINT)) - 0x6A4);
			t->w12 = (int16_t)(CrtRand() % 0x2BC - i * 700 - 0x2C56);
			t->w14 = (int16_t)(CrtRand() % 0xD48 + var<int32_t>(game_addr(POINT) + 4) - 0x6A4);
			t->w18 = (int16_t)((*(const uint8_t *)&t->w10 & 1) << 11);
			t->w1C = (int16_t)(CrtRand() % 0x700 + 0x200);
			const int16_t step = (int16_t)(CrtRand() % 0x19 + 5);
			t->w1E = step;
			if (t->w10 < PointX()) t->w1E = (int16_t)-step;
			const int16_t sy = (int16_t)(CrtRand() % 0x500 + 0x200);
			t->w20 = sy;
			if (sy > t->w1C) t->w20 = (int16_t)(sy * 2 / 3);
		}
	}

	static void AddDebris()
	{
		for (int32_t i = 0; i < 6; i++)
		{
			Node24 *t = AddMain(game_addr(ORIG_Debris));
			t->c = 0;
			t->e = (int16_t)(i * 3);
			t->w10 = PointX();
			const int32_t r = CrtRand() % 0x384;
			t->w14 = PointZ();
			t->w12 = (int16_t)(r - i * 900 - 0x2CBA);
			t->w18 = (int16_t)(CrtRand() % 0x1000);
			t->w1A = (int16_t)(CrtRand() % 0x6E + 0xA);
			const int16_t s = (int16_t)((CrtRand() % 0xC00 + 0x600) / 4);
			t->w1E = s;
			t->w1C = s;
			const int16_t sy = (int16_t)((CrtRand() % 0xE00 + 0x400) / 10);
			t->w22 = sy;
			t->w20 = sy;
		}
	}

	static void AddScreenFlash()
	{
		Node24 *t = AddMain(game_addr(ORIG_ScreenFlash));
		t->c = 0;
		t->w10 = PointX();
		t->w12 = -50;
		t->w14 = PointZ();
		t->w1C = 0xF00;
		t->w22 = 0x12;
	}

	static void AddMorphs()
	{
		for (int32_t i = 0; i < 4; i++)
		{
			Node24 *t = AddMain(game_addr(ORIG_Morph));
			t->e = (int16_t)(i * 2);
			t->c = 0;
			t->w10 = PointX();
			t->w12 = (int16_t)0xD058;
			t->w14 = PointZ();
			t->w1C = 0xC00;
			t->w22 = 0x140;
			t->w20 = 0x140;
			if (i == 0) t->w18 = 0;
			else t->w18 = (int16_t)((i + 3) << 9);
		}
	}

	static void AddRingFlat()
	{
		Node24 *t = AddMain(game_addr(ORIG_RingFlat));
		t->c = 0;
		t->w10 = PointX();
		t->w12 = (int16_t)0xD058;
		t->w14 = PointZ();
		t->w18 = (int16_t)(CrtRand() % 0x1000);
		t->w1A = -0x10;
		t->w1E = 0x60;
		t->w1C = 0x60;
	}

	static void AddRingFlat2()
	{
		Node24 *t = AddMain(game_addr(ORIG_RingFlat2));
		t->c = 0;
		t->w10 = PointX();
		t->w12 = (int16_t)0xD058;
		t->w14 = PointZ();
		t->w18 = (int16_t)(CrtRand() % 0x1000);
		t->w1A = -0x30;
		t->w1E = 0x10A;
		t->w1C = 0x10A;
	}

	static void AddBladeGlow()
	{
		Node24 *t = AddMain(game_addr(ORIG_BladeGlow));
		t->c = 0;
		t->w1C = 0x4000;
		t->w20 = 0x1E00;
	}

	static void AddWave()
	{
		Node24 *t = AddMain(game_addr(ORIG_Wave));
		t->c = 0;
		t->w12 = 0;
		t->w10 = PointX();
		t->w14 = PointZ();
		t->w1C = 0x1000;
		t->w20 = 0x2000;
	}

	static void AddWave2()
	{
		Node24 *t = AddMain(game_addr(ORIG_Wave2));
		t->c = 0;
		t->w10 = 0;
		t->w12 = 0;
		t->w1E = 0x20;
		t->w1C = 0x20;
		t->w14 = (int16_t)0xFBB4;
		t->w22 = 0x400;
		t->w20 = 0x400;
	}

	static uint32_t __cdecl DirectorTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)game_addr(FRAME), CamFrame());
		const int32_t ir0 = (ComputeCos(shl32(n->c, 9)) + 0x1000) >> 1;
		Colour() = 0x30587F;
		GteSetFarColor(0x30, 0x30, 0x80);
		GteLoadRGBC(&Colour());
		GteSetIR0(ir0);
		GteDPCS();
		GteStoreRGB2(&Colour());
		if (UpdateFlags() & 0x201)
		{
			if (UpdateFlags() & 1) return 0;
			if (PauseQuery() < 0) return 0;
		}
		if (n->c == 0)
		{
			CameraScriptStart(game_addr(CAMERA_SCRIPT), game_addr(FRAME), Caster(), QMain(), 0);
			ArmCameraReturn();
			BindEntityModelFromComData(Caster(), game_addr(BIND_OUT), game_addr(ANIM_DATA));
		}
		int32_t c = n->c;
		if (c < 0x23)
		{
			if (c == 0) QueueChainTransformation(Caster(), 1);
			else if (c == 5) CharacterLoad(0x21A, TexBase());
			else if (c == 0xF)
			{
				if (PauseQuery() < 0) return 0;
				QueueTIMUpload(TexBase());
			}
		}
		else if ((c -= 0x23) < 0x46)
		{
			if (c == 0)
			{
				QueueChainTransformation(Caster(), 2);
				SetTipPoint();
				BdPlaySE(SOUND_A(), 0, 0x80);
			}
			else if (c == 1) AddFlareA();
			else if (c == 0xC) AddFlareB();
			else if (c == 0x10)
			{
				AddCleared(game_addr(ORIG_OrbitSpark));
				AddCleared(game_addr(ORIG_Swirl));
			}
			else if (c == 0x16)
			{
				AddRings();
				AddCleared(game_addr(ORIG_Spawner));
			}
			else if (c == 0x20) AddCleared(game_addr(ORIG_Shard));
			else if (c == 0x32) AddBall();
			else if (c == 0x3E) AddFrameModel();
			else if (c == 0x45) ScreenFadeTask(2, 1, 4, 0xFF);
		}
		else if ((c -= 0x46) < 0x13)
		{
			if (c == 2) AddDome();
		}
		else if ((c -= 0x13) < 0x28)
		{
			if (c == 0)
			{
				StageGroupsShow();
				CharacterLoad(0x21B, TexBase());
				AddDome2();
				AddPillars();
				AddDebris();
			}
			else if (c == 0xA)
			{
				if (PauseQuery() < 0) return 0;
				QueueTIMUpload(TexBase());
			}
		}
		else if ((c -= 0x28) < 0x3C)
		{
			if (c == 0)
			{
				BdPlaySE(SOUND_B(), 0, 0x80);
				AddCleared(game_addr(ORIG_Starfield));
				AddScreenFlash();
				*Entity(TargetSlot()) |= 4;
			}
			else if (c == 0x18)
			{
				AddMorphs();
				AddRingFlat2();
				AddCleared(game_addr(ORIG_MorphSpark));
			}
			else if (c == 0x15) AddRingFlat();
		}
		else if ((c -= 0x3C) < 0x14)
		{
			if (c == 0)
			{
				StageGroupsHide();
				AddBladeGlow();
				AddCleared(game_addr(ORIG_BurstSpark));
				AddWave();
				*(uint16_t *)Entity(TargetSlot()) &= 0xFFFB;
			}
		}
		else if ((c -= 0x14) < 0x2D)
		{
			if (c == 0) QueueChainTransformation(Caster(), 3);
			else if (c == 0x13) AddWave2();
			else if (c == 0x14) AddCleared(game_addr(ORIG_Smoke));
			else if (c == 0x20) ScreenFadeTask(0xC, 6, 0xF, 0xFF);
			else if (c == 0x12) *Caster() |= 8;
			else if (c == 0x11) AddCleared(game_addr(ORIG_Shake));
		}
		else if ((c -= 0x2D) < 0x19)
		{
			if (c == 0)
			{
				QueueChainTransformation(Caster(), 4);
				*(uint16_t *)Caster() &= 0xFFF7;
			}
			else if (c == 1) AddCleared(game_addr(ORIG_Restore));
		}
		if (n->c == 0x133) ApplyActionResultToTargets(Context()->actions->targets, Context()->actions->target_count);
		if (n->c >= 0x132) SetScreenFlash((uint32_t)shl32(0x13A - n->c, 8), 0);
		n->c++;
		if (n->c <= 0x13A) return 0;
		SetScreenFlash(0, 0);
		return TASK_END;
	}

	static uint8_t *ModelHeader(uint32_t model, bool fade_on, int32_t fade)
	{
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = model;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (fade_on)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		return h;
	}

	static void WorldMatrix(Mat4x3 *m, int16_t ax, int16_t yaw, const int16_t *pos, int32_t sx, int32_t sy, int32_t sz, const Mat4x3 *view)
	{
		int16_t ang[4];
		ang[0] = ax;
		ang[1] = yaw;
		ang[2] = 0;
		ComposeZYXRotationMatrix(ang, m);
		int32_t sv[3];
		m->t[0] = pos[0];
		m->t[1] = pos[1];
		sv[2] = sz;
		sv[0] = sx;
		m->t[2] = pos[2];
		sv[1] = sy;
		Scale3DMatrix(m, sv);
		ComposeAffineTransform(view, m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
	}

	static void FacingMatrix(Mat4x3 *m, int16_t roll, int32_t scale, const int16_t *pos, int16_t s, int32_t off)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = roll;
		ComposeZYXRotationMatrix(ang, m);
		int32_t sv[3];
		sv[2] = scale;
		sv[1] = scale;
		sv[0] = scale;
		Scale3DMatrix(m, sv);
		TransformCameraByShadowRotation(pos, s, off);
		GteSetRotMatrix(m);
	}

	static void FlareADraw(const Node24 *n, int16_t roll, int16_t scale, int32_t c, int32_t fade)
	{
		Mat4x3 m = {};
		FacingMatrix(&m, roll, scale, &n->w10, scale, -(scale >> 4));
		uint8_t *h = ModelHeader(game_addr(MODEL_FLAREA), c >= 8, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl FlareATask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		FlareADraw(n, n->w18, n->w1C, n->c, mul32(n->c - 8, 682));
		if (DrawOnly()) return 0;
		n->w18 = (int16_t)(n->w18 + n->w1A);
		const int16_t r = n->w1E;
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 5);
		n->c++;
		return EndAt(n->c, 0xE);
	}

	static void FlareBDraw(const Node24 *n, int16_t roll, int16_t scale, int32_t c, int32_t fade)
	{
		Mat4x3 m = {};
		int16_t ang[4] = {};
		ang[2] = roll;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t sv[3];
		sv[2] = scale;
		sv[1] = scale;
		sv[0] = scale;
		Scale3DMatrix(&m, sv);
		TransformCameraByShadowRotation(&n->w10, scale, -(scale >> 4));
		GteSetRotMatrix(&m);
		uint8_t *h = ModelHeader(game_addr(MODEL_FLAREB), c < 8, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		ang[2] = (int16_t)-roll;
		ComposeZYXRotationMatrix(ang, &m);
		GteSetRotMatrix(&m);
		sv[2] = scale >> 1;
		sv[0] = scale >> 1;
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl FlareBTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		FlareBDraw(n, n->w18, n->w1C, n->c, shl32(8 - n->c, 9));
		if (DrawOnly()) return 0;
		n->w18 = (int16_t)(n->w18 + n->w1A);
		n->c++;
		return EndAt(n->c, 0x3A);
	}

	static void RingDraw(const Node24 *n, int16_t yaw, int16_t sy, int32_t c, int32_t fade)
	{
		Mat4x3 m = {};
		WorldMatrix(&m, 0, yaw, &n->w10, n->w1C, sy, n->w1C, &Camera());
		uint8_t *h = ModelHeader(game_addr(MODEL_RING), c < 6 || c >= 0x18, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static int32_t RingFade(int32_t c) { return c < 6 ? 0x1000 - mul32(c, 682) : shl32(c - 0x18, 8); }

	static uint32_t __cdecl RingTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->e > 0)
		{
			if (DrawOnly()) return 0;
			n->e--;
			return 0;
		}
		RingDraw(n, n->w18, n->w20, n->c, RingFade(n->c));
		if (DrawOnly()) return 0;
		n->w18 = (int16_t)(n->w18 + n->w1A);
		const int16_t r = n->w22;
		n->w20 = (int16_t)(n->w20 + r);
		n->w22 = (int16_t)(r - r / 32);
		n->c++;
		return EndAt(n->c, 0x28);
	}

	static void PulseDraw(const Node24 *n, int16_t yaw, int16_t sx, int16_t sy, bool fade_on, int32_t fade)
	{
		Mat4x3 m = {};
		WorldMatrix(&m, 0, yaw, &n->w10, sx, sy, sx, &Camera());
		uint8_t *h = ModelHeader(game_addr(MODEL_PULSE), fade_on, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl PulseATask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		PulseDraw(n, n->w18, n->w1C, n->w20, n->c >= 6, shl32(n->c - 6, 9));
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w18 = (int16_t)(n->w18 + n->w1A);
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 8);
		const int16_t q = n->w22;
		n->w20 = (int16_t)(n->w20 + q);
		n->w22 = (int16_t)(q - q / 10);
		n->c++;
		return EndAt(n->c, 0xE);
	}

	static uint32_t __cdecl PulseBTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		PulseDraw(n, n->w18, n->w1C, n->w20, n->c >= 4, shl32(n->c - 4, 9));
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w18 = (int16_t)(n->w18 + n->w1A);
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 12);
		n->c++;
		return EndAt(n->c, 0xC);
	}

	static uint32_t __cdecl SpawnerTask(TaskNode *tn)
	{
		if (DrawOnly()) return 0;
		Node24 *n = (Node24 *)tn;
		if (n->c < 0x22 && n->c % 4 == 1)
		{
			Node24 *t = AddMain(game_addr(ORIG_PulseA));
			t->c = 0;
			t->w10 = PointX();
			t->w12 = 0;
			t->w14 = PointZ();
			t->w18 = (int16_t)(CrtRand() % 0x1000);
			const int32_t r = CrtRand() % 0x32;
			t->w1E = 0x355;
			t->w1C = 0x355;
			t->w22 = 0x555;
			t->w20 = 0x555;
			t->w1A = (int16_t)(r + 0x28);
		}
		if (n->c < 0x24 && n->c % 4 == 1)
		{
			Node24 *t = AddMain(game_addr(ORIG_PulseB));
			t->c = 0;
			t->w10 = PointX();
			t->w12 = 0;
			t->w14 = PointZ();
			t->w18 = (int16_t)(CrtRand() % 0x1000);
			t->w20 = 0x380;
			const int32_t r = CrtRand() % 0x41;
			t->w1E = 0x733;
			t->w1C = 0x733;
			t->w1A = (int16_t)(r + 0x19);
		}
		n->c++;
		return EndAt(n->c, 0x30);
	}

	static void BallDraw(const Node24 *n, int16_t scale, int32_t c, int32_t fade)
	{
		TransformCameraByShadowRotation(&n->w10, scale, -n->w16);
		uint8_t *h = ModelHeader(game_addr(MODEL_BALL), c >= 0x14, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl BallTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		BallDraw(n, n->w1C, n->c, shl32(n->c - 0x14, 9));
		if (DrawOnly()) return 0;
		const int16_t c = n->c;
		if (c == 0x12) n->w1E = 0x400;
		n->w1C = (int16_t)(n->w1C + n->w1E);
		n->c = (int16_t)(c + 1);
		return EndAt(n->c, 0x1C);
	}

	static void FrameModelDraw(const Node24 *n, int16_t s, int32_t c, int32_t fade, const Mat4x3 *view)
	{
		Mat4x3 m = {};
		WorldMatrix(&m, 0, 0, &n->w10, s, s, s, view);
		uint8_t *h = ModelHeader(game_addr(MODEL_FRAME), c >= 4, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl FrameModelTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		FrameModelDraw(n, n->w1C, n->c, mul32(n->c - 4, 682), CamFrame());
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 6);
		n->c++;
		return EndAt(n->c, 0xA);
	}

	static uint32_t DomeDraw(const Node24 *n, int16_t sx, int16_t sy, uint32_t colour, int32_t shift, uint32_t cursor)
	{
		Mat4x3 m = {};
		WorldMatrix(&m, 0, 0, &n->w10, sx, sy, sx, &Camera());
		RenderHeader *h = (RenderHeader *)AllocHeader(0x6C);
		h->colour = colour;
		h->model = game_addr(MODEL_DOME);
		*(uint32_t *)h->far_rgb = 0;
		h->flags = 0x33;
		cursor = RenderModel(h, RenderOT(), shift, cursor);
		FieldFree(0x6C);
		return cursor;
	}

	static uint32_t __cdecl DomeTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		PacketCursor() = DomeDraw(n, n->w1C, n->w20, Colour(), 3, PacketCursor());
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w1C = (int16_t)(n->w1C - r);
		n->w1E = (int16_t)(r - r / 64);
		n->c++;
		return EndAt(n->c, 0x11);
	}

	static uint32_t __cdecl Dome2Task(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		FrameArena() = DomeDraw(n, n->w1C, n->w20, Colour(), 2, FrameArena());
		if (DrawOnly()) return 0;
		const int16_t c = n->c;
		if (c < 0x14)
		{
			const int16_t r = n->w22;
			n->w20 = (int16_t)(n->w20 + r);
			n->w22 = (int16_t)(r + r / 16);
			if (n->w20 >= 0x7000) n->w20 = 0x7000;
		}
		n->c = (int16_t)(c + 1);
		return EndAt(n->c, 0x28);
	}

	static void BeamDraw(const Node24 *n, int16_t yaw1, int16_t yaw2)
	{
		Mat4x3 m = {};
		int16_t ang[4] = {};
		ang[1] = yaw1;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t sv[3];
		m.t[0] = n->w10;
		m.t[1] = n->w12;
		sv[2] = n->w1C;
		sv[0] = n->w1C;
		m.t[2] = n->w14;
		sv[1] = n->w20;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_BEAM);
		*(int32_t *)(h + 0xC) = 0xA00;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0xCC;
		FrameArena() = RenderPrimModel(h, OT0() + 0x4068, 6, FrameArena());
		ang[1] = yaw2;
		ComposeZYXRotationMatrix(ang, &m);
		sv[2] = n->w1C - 0x100;
		sv[0] = n->w1C - 0x100;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		*(int32_t *)(h + 0xC) = 0x600;
		*(uint32_t *)(h + 0x1C) = 0xC3;
		FrameArena() = RenderPrimModel(h, OT0() + 0x4068, 7, FrameArena());
		FieldFree(0x58);
	}

	static uint32_t __cdecl BeamTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		BeamDraw(n, n->w18, n->w1A);
		if (DrawOnly()) return 0;
		n->w18 = (int16_t)(n->w18 + 4);
		n->w1A = (int16_t)(n->w1A - 2);
		n->c++;
		return EndAt(n->c, 0x28);
	}

	static void RockDraw(const Node24 *n, const int16_t *pos)
	{
		Mat4x3 m = {};
		int16_t ang[4] = {};
		ang[1] = n->w18;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t sv[3];
		sv[0] = n->w1C;
		sv[1] = n->w20;
		sv[2] = 0;
		Scale3DMatrix(&m, sv);
		TransformCameraByShadowRotation(pos, n->w1C, 0);
		GteSetRotMatrix(&m);
		uint8_t *h = ModelHeader(game_addr(MODEL_ROCK), false, 0);
		FrameArena() = RenderPrimModel(h, RenderOT(), 2, FrameArena());
		FieldFree(0x58);
	}

	static uint32_t __cdecl RockTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		RockDraw(n, &n->w10);
		if (DrawOnly()) return 0;
		n->w10 = (int16_t)(n->w10 + n->w1E);
		n->c++;
		return EndAt(n->c, 0x28);
	}

	static void DebrisDraw(const Node24 *n, int16_t yaw, int16_t sx, int16_t sy, int32_t c, int32_t fade)
	{
		Mat4x3 m = {};
		WorldMatrix(&m, 0, yaw, &n->w10, sx, sy, sx, &Camera());
		uint8_t *h = ModelHeader(game_addr(MODEL_DEBRIS), c >= 0xC, fade);
		FrameArena() = RenderPrimModel(h, RenderOT(), 2, FrameArena());
		FieldFree(0x58);
	}

	static uint32_t __cdecl DebrisTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->e > 0)
		{
			if (DrawOnly()) return 0;
			n->e--;
			return 0;
		}
		DebrisDraw(n, n->w18, n->w1C, n->w20, n->c, shl32(n->c - 0xC, 9));
		if (DrawOnly()) return 0;
		int16_t r = n->w1A;
		n->w18 = (int16_t)(n->w18 + r);
		n->w1A = (int16_t)(r - r / 5);
		r = n->w1E;
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 6);
		r = n->w22;
		n->w20 = (int16_t)(n->w20 + r);
		n->w22 = (int16_t)(r - r / 12);
		n->c++;
		return EndAt(n->c, 0x14);
	}

	static void ScreenFlashDraw(int16_t y, int16_t s, int32_t fade2)
	{
		Mat4x3 m = {};
		int16_t ang[4] = {};
		ComposeZYXRotationMatrix(ang, &m);
		int32_t sv[3];
		m.t[2] = var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		m.t[1] = y;
		m.t[0] = 0;
		sv[2] = s;
		sv[1] = s;
		sv[0] = s;
		Scale3DMatrix(&m, sv);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_FLASH);
		*(int32_t *)(h + 0xC) = 0x400;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0xCC;
		PacketCursor() = RenderPrimModel(h, OT0() + 0x406C, 0xC, PacketCursor());
		ComposeZYXRotationMatrix(ang, &m);
		m.t[1] = y;
		sv[2] = s + 0x60;
		sv[1] = s + 0x60;
		sv[0] = s + 0x60;
		m.t[0] = 0;
		m.t[2] = var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		Scale3DMatrix(&m, sv);
		GteSetRotMatrix(&m);
		*(int32_t *)(h + 0xC) = fade2;
		*(uint32_t *)(h + 0x1C) = 0xC3;
		PacketCursor() = RenderPrimModel(h, OT0() + 0x4068, 0xC, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl ScreenFlashTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ScreenFlashDraw(n->w12, n->w1C, shl32(0x50 - n->c, 5));
		if (DrawOnly()) return 0;
		const int16_t c = n->c;
		if (c < 0x12)
		{
			const int16_t r = n->w22;
			n->w12 = (int16_t)(n->w12 + r);
			if (r > 0) n->w22 = (int16_t)(r - 1);
		}
		n->c = (int16_t)(c + 1);
		return EndAt(n->c, 0x3C);
	}

	static void MorphMatrix(const Node24 *n, int16_t sy, Mat4x3 *out)
	{
		Mat4x3 m = {};
		int16_t ang[4] = {};
		ComposeZYXRotationMatrix(ang, &m);
		int32_t sv[3];
		sv[2] = n->w1C;
		sv[0] = n->w1C;
		sv[1] = sy;
		Scale3DMatrix(&m, sv);
		TransformCameraByShadowRotation(&n->w10, n->w1C, 0);
		const Mat4x3 *sh = (const Mat4x3 *)game_addr(game::SOME_CAMERA_Z_FACTOR);
		GteSetRotMatrix(&m);
		const int32_t t0 = sh->t[0], t1 = sh->t[1];
		*out = m;
		out->t[0] = t0;
		out->t[1] = t1;
		out->t[2] = sh->t[2];
	}

	static void MorphVertices(int32_t c, uint8_t *verts)
	{
		const int32_t t = shl32(c, 12) / 36;
		const int32_t count = var<int32_t>(game_addr(MORPH_COUNT));
		const uint8_t *a = (const uint8_t *)game_addr(MORPH_A), *b = (const uint8_t *)game_addr(MORPH_B);
		for (int32_t k = 0; k < count; k++, a += 8, b += 8, verts += 8)
		{
			const int16_t ax = *(const int16_t *)a, ay = *(const int16_t *)(a + 2);
			*(int16_t *)verts = (int16_t)((mul32((int32_t)*(const int16_t *)b - ax, t) >> 12) + ax);
			*(int16_t *)(verts + 2) = (int16_t)((mul32((int32_t)*(const int16_t *)(b + 2) - ay, t - 0x1000) >> 12) + ay);
			*(int16_t *)(verts + 4) = *(const int16_t *)(a + 4);
		}
	}

	static uint32_t MorphRender(const Node24 *n, const uint8_t *verts, uint32_t colour, uint32_t cursor)
	{
		RenderHeader *h = (RenderHeader *)AllocHeader(0x6C);
		h->fade = n->w18;
		h->model = game_addr(MODEL_MORPH);
		*(uint32_t *)h->far_rgb = 0;
		h->flags = 0x20F3;
		h->verts = (uint32_t)verts;
		h->colour = colour;
		cursor = RenderModel(h, RenderOT(), 2, cursor);
		FieldFree(0x6C);
		return cursor;
	}

	static uint32_t __cdecl MorphTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->e > 0)
		{
			if (DrawOnly()) return 0;
			n->e--;
			return 0;
		}
		MorphMatrix(n, n->w20, (Mat4x3 *)game_addr(MORPH_MAT));
		MorphVertices(n->c, (uint8_t *)game_addr(MORPH_VTX));
		PacketCursor() = MorphRender(n, (const uint8_t *)game_addr(MORPH_VTX), Colour(), PacketCursor());
		if (DrawOnly()) return 0;
		const int16_t r = n->w22;
		n->w20 = (int16_t)(n->w20 + r);
		n->w22 = (int16_t)(r - r / 12);
		n->c++;
		return n->c >= 0x24 - n->e ? TASK_END : 0;
	}

	static void RingFlatDraw(const Node24 *n, uint32_t model, int16_t yaw, int16_t s, bool fade_on, int32_t fade)
	{
		Mat4x3 m = {};
		WorldMatrix(&m, 0x400, yaw, &n->w10, s, s, 0, &Camera());
		uint8_t *h = ModelHeader(model, fade_on, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl RingFlatTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		RingFlatDraw(n, game_addr(MODEL_RINGFLAT), n->w18, n->w1C, n->c < 4, shl32(4 - n->c, 0xA));
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w18 = (int16_t)(n->w18 + n->w1A);
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 16);
		n->c++;
		return EndAt(n->c, 0x27);
	}

	static uint32_t __cdecl RingFlat2Task(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		RingFlatDraw(n, game_addr(MODEL_RINGFLAT2), n->w18, n->w1C, n->c >= 2, shl32(n->c - 2, 8));
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w18 = (int16_t)(n->w18 + n->w1A);
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 12);
		n->c++;
		return EndAt(n->c, 0x12);
	}

	static void BladeGlowDraw(const Node24 *n, const int32_t *dir, const int16_t *pos, uint32_t colour, int32_t c, int32_t fade)
	{
		int32_t axis[4] = {};
		const int32_t angle = RotationBetweenVectors((const int32_t *)game_addr(GLOW_REF), dir, axis);
		Mat4x3 m = {};
		BuildAxisAngleRotationMatrix(angle, &m, axis);
		MatrixMultiply3x3(&m, (const Mat4x3 *)game_addr(FRAME));
		int32_t sv[3];
		m.t[2] = pos[2];
		m.t[0] = pos[0];
		m.t[1] = pos[1];
		sv[2] = n->w1C;
		sv[0] = n->w1C;
		sv[1] = n->w20;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		RenderHeader *h = (RenderHeader *)AllocHeader(0x6C);
		h->model = game_addr(MODEL_GLOW);
		*(uint32_t *)h->far_rgb = 0;
		h->flags = 0x33;
		if (c >= 0x39)
		{
			h->flags = 0xF3;
			h->fade = fade;
		}
		h->colour = colour;
		PacketCursor() = RenderModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x6C);
	}

	static uint32_t __cdecl BladeGlowTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int16_t pos[4];
		int32_t dir[4];
		WeaponBone(Caster(), 1, dir, pos);
		BladeGlowDraw(n, dir, pos, Colour(), n->c, shl32(n->c - 0x39, 9));
		if (DrawOnly()) return 0;
		n->c++;
		return EndAt(n->c, 0x41);
	}

	static void WaveDraw(const Node24 *n, int32_t c, int32_t scroll, int32_t fade)
	{
		Mat4x3 m = {};
		WorldMatrix(&m, 0, 0, &n->w10, n->w1C, n->w20, n->w1C, &Camera());
		uint8_t *h = AllocHeader(0x7C);
		*(uint32_t *)(h + 8) = 0;
		*(int16_t *)(h + 0x18) = 0;
		*(int16_t *)(h + 0x1A) = 0;
		*(uint32_t *)(h + 0x10) = 0;
		*(uint32_t *)h = game_addr(MODEL_WAVE);
		*(uint32_t *)(h + 0x20) = 0x33;
		*(int16_t *)(h + 0x1C) = 0x40;
		*(int16_t *)(h + 0x1E) = 0x80;
		*(int32_t *)(h + 0x14) = scroll;
		if (c >= 0xE)
		{
			*(uint32_t *)(h + 0x20) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = Mag151Render(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x7C);
	}

	static uint32_t __cdecl WaveTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		WaveDraw(n, n->c, mul32(n->c, 24), mul32(n->c - 0xE, 341));
		if (DrawOnly()) return 0;
		n->c++;
		return EndAt(n->c, 0x1A);
	}

	static void Wave2Draw(const Node24 *n, int16_t scroll, int16_t sy, int32_t c, int32_t fade, const Mat4x3 *view)
	{
		Mat4x3 m = {};
		int16_t ang[4] = {};
		ComposeZYXRotationMatrix(ang, &m);
		int32_t sv[3];
		sv[0] = 0x3800;
		sv[2] = 0x1000;
		m.t[0] = n->w10;
		m.t[1] = n->w12;
		m.t[2] = n->w14;
		sv[1] = sy;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(view, &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = AllocHeader(0x7C);
		*(int32_t *)(h + 0x14) = scroll;
		*(uint32_t *)h = game_addr(MODEL_WAVE2);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x20) = 0x33;
		*(int16_t *)(h + 0x18) = 0;
		*(int16_t *)(h + 0x1A) = 0;
		*(int16_t *)(h + 0x1C) = 0x40;
		*(int16_t *)(h + 0x1E) = 0x80;
		*(uint32_t *)(h + 0x10) = 0;
		if (c >= 0x12)
		{
			*(uint32_t *)(h + 0x20) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		PacketCursor() = Mag151Render(h, RenderOT(), 2, PacketCursor());
		ComposeZYXRotationMatrix(ang, &m);
		sv[0] = 0x3880;
		sv[1] = sy;
		sv[2] = 0x10C0;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(view, &m, &m);
		GteSetRotMatrix(&m);
		PacketCursor() = Mag151Render(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x7C);
	}

	static uint32_t __cdecl Wave2Task(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		Wave2Draw(n, n->w1C, n->w20, n->c, shl32(n->c - 0x12, 9), CamFrame());
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		const int16_t q = n->w22;
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - 1);
		n->w20 = (int16_t)(n->w20 + q);
		n->w22 = (int16_t)(q - q / 4);
		n->c++;
		return EndAt(n->c, 0x1A);
	}

	static void OrbitDraw(OrbitWork *w, uint8_t *h, const PoolE *p, int16_t angle)
	{
		const int32_t x = p->a[0], y = p->a[1], z = p->a[2];
		w->v[1] = y;
		w->axis[1] = y;
		w->v[0] = x;
		w->v[2] = z;
		w->axis[2] = -x;
		w->axis[0] = -z;
		BuildAxisAngleRotationMatrix(angle, &w->m2, w->axis);
		TransformVectorBy3x3Matrix(&w->m2, w->v, w->v);
		const int32_t r = p->a[3];
		w->pos[0] = (int16_t)(mul32(r, w->v[0]) >> 12);
		w->pos[1] = (int16_t)(mul32(w->v[1], r) >> 12);
		w->pos[2] = (int16_t)(mul32(r, w->v[2]) >> 12);
		GteLoadV0(w->pos);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(p->scale);
		*(int16_t *)(h + 4) = p->life;
		GteTransFromMAC();
		FrameArena() = InitEffectSequenceFromData(h, RenderOT(), 2, FrameArena());
	}

	static void PoolLight(Mat4x3 *m, int16_t *ang, int32_t x, int32_t y, int32_t z, const Mat4x3 *view)
	{
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = 0;
		ComposeZYXRotationMatrix(ang, m);
		m->t[0] = x;
		m->t[1] = y;
		m->t[2] = z;
		ComposeAffineTransform(view, m, m);
		GteSetLightMatrix(m);
		GteSetBackColorFromTrans(m);
	}

	static uint32_t __cdecl OrbitSparkTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		OrbitWork *w = (OrbitWork *)FieldAlloc(0x68);
		if (!DrawOnly() && n->c <= 0x30)
		{
			for (int32_t j = 0; j < 1; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_A), game_addr(POOL_A_END), 0x64);
				if (i < 0) break;
				PoolE *p = Entry(game_addr(POOL_A), i);
				p->flags = 1;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x280 + 0x300);
				w->pos[0] = (int16_t)(CrtRand() % 0x1000 - 0x800);
				w->pos[1] = (int16_t)(CrtRand() % 0x1000 - 0x800);
				w->pos[2] = (int16_t)(CrtRand() % 0x1000 - 0x800);
				NormalizeS16(w->pos, &p->a[0]);
				p->a[3] = (int16_t)(CrtRand() % 0x96 + 0x190);
				p->a[4] = (int16_t)(CrtRand() % 0x1000);
				p->a[5] = (int16_t)(CrtRand() % 0x41 + 0x1E);
			}
		}
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_ORBIT);
		*(int16_t *)(h + 0x24) = 0;
		PoolLight(&w->m1, w->pos, PointX(), var<int16_t>(game_addr(POINT) + 2), PointZ(), &Camera());
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (!(p->flags & 1)) continue;
			OrbitDraw(w, h, p, p->a[4]);
			if (DrawOnly()) continue;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				p->flags = 0;
				continue;
			}
			p->life++;
			p->a[4] = (int16_t)(p->a[4] + p->a[5]);
		}
		FieldFree(0xB4);
		FieldFree(0x68);
		if (DrawOnly()) return 0;
		n->c++;
		if (n->c < 0x36) return 0;
		ClearType(game_addr(POOL_A), game_addr(POOL_A_END), 1);
		return TASK_END;
	}

	static void SwirlHeader(uint8_t *h)
	{
		*(uint32_t *)(h + 8) = 0;
		*(int16_t *)(h + 0x18) = 0;
		*(int16_t *)(h + 0x1A) = 0;
		*(uint32_t *)(h + 0x10) = 0;
		*(uint32_t *)h = game_addr(MODEL_SWIRL);
		*(int16_t *)(h + 0x1C) = 0x40;
		*(int16_t *)(h + 0x1E) = 0x80;
	}

	static void SwirlDraw(ModelWork *w, uint8_t *h, int16_t angle, int16_t scale, int32_t life, int32_t scroll, int32_t fade)
	{
		w->ang[2] = angle;
		ComposeZYXRotationMatrix(w->ang, &w->m2);
		w->v[0] = scale;
		Scale3DMatrix(&w->m2, w->v);
		GteSetRotMatrix(&w->m2);
		*(uint32_t *)(h + 0x20) = 0x33;
		if (life < 6)
		{
			*(int32_t *)(h + 0xC) = fade;
			*(uint32_t *)(h + 0x20) = 0xF3;
		}
		else if (life >= 0xA)
		{
			*(int32_t *)(h + 0xC) = fade;
			*(uint32_t *)(h + 0x20) = 0xF3;
		}
		*(int32_t *)(h + 0x14) = scroll;
		PacketCursor() = Mag151Render(h, RenderOT(), 2, PacketCursor());
	}

	static int32_t SwirlFade(int32_t life) { return life < 6 ? 0x1000 - mul32(life, 682) : mul32(life - 0xA, 682); }

	static uint32_t __cdecl SwirlTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x68);
		if (!DrawOnly() && n->c <= 0x30 && (n->c & 1))
		{
			for (int32_t j = 0; j < 1; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_A), game_addr(POOL_A_END), 0x64);
				if (i < 0) break;
				PoolE *p = Entry(game_addr(POOL_A), i);
				p->flags = 2;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x1800 + 0x400);
				p->a[1] = 0;
				p->a[2] = (int16_t)(CrtRand() % 0x1000);
				p->a[5] = (int16_t)(-8 - CrtRand() % 0x18);
				const int16_t r = (int16_t)(CrtRand() % 0x28 + 0xA);
				p->a[6] = r;
				if (*(const uint8_t *)&p->a[2] & 1) p->a[6] = (int16_t)-r;
			}
		}
		uint8_t *h = AllocHeader(0x7C);
		SwirlHeader(h);
		TransformCameraByShadowRotation((const void *)game_addr(POINT), 0x1000, 0);
		w->v[2] = 0x1000;
		w->v[1] = 0x1000;
		w->ang[1] = 0;
		w->ang[0] = 0;
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (!(p->flags & 2)) continue;
			SwirlDraw(w, h, p->a[2], p->scale, p->life, p->a[1], SwirlFade(p->life));
			if (DrawOnly()) continue;
			if (p->life >= 0x10) p->flags = 0;
			else
			{
				p->life++;
				p->a[1] = (int16_t)(p->a[1] + p->a[5]);
				p->a[2] = (int16_t)(p->a[2] + p->a[6]);
			}
		}
		FieldFree(0x7C);
		FieldFree(0x68);
		if (DrawOnly()) return 0;
		n->c++;
		if (n->c < 0x36) return 0;
		ClearType(game_addr(POOL_A), game_addr(POOL_A_END), 2);
		return TASK_END;
	}

	static void MorphSparkSetup(const Mat4x3 *mat)
	{
		GteSetRotMatrix(mat);
		GteSetTransVector(mat);
	}

	static uint32_t MorphSparkDraw(ModelWork *w, PoolE *p, const int16_t *pos, uint32_t pk, bool update)
	{
		GteLoadV0(pos);
		GteRTPS();
		const int16_t life = p->life;
		*(uint32_t *)pk = 0x2000000;
		*(uint32_t *)(pk + 4) = var<uint32_t>(game_addr(DOT_COLOURS) + 4 * life);
		if (update)
		{
			const int16_t l = (int16_t)(life + 1);
			p->life = l;
			if (l >= 0xC) p->flags = 0;
			else
			{
				p->a[0] = (int16_t)(p->a[0] + p->a[4]);
				p->a[1] = (int16_t)(p->a[1] + p->a[5]);
				p->a[2] = (int16_t)(p->a[2] + p->a[6]);
			}
		}
		GteReadFLAG(&w->v[2]);
		if (w->v[2] & 0x60000) return pk;
		GteReadSXY2((void *)(pk + 8));
		GteReadSZ3(&w->v[3]);
		InsertPrimAutoDepth((uint32_t)w->v[0] + (uint32_t)(w->v[3] >> w->v[1]) * 4, (void *)pk);
		return pk + 0xC;
	}

	static uint32_t __cdecl MorphSparkTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x58);
		if (!DrawOnly() && n->c <= 0x1C)
		{
			for (int32_t j = 0; j < 5; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_A), game_addr(POOL_A_END), 0x64);
				if (i < 0) break;
				PoolE *p = Entry(game_addr(POOL_A), i);
				p->flags = 1;
				p->life = 0;
				const int32_t r = CrtRand() % 0x69;
				*(uint32_t *)&p->a[0] = var<uint32_t>(game_addr(MORPH_VTX) + 8 * r);
				*(uint32_t *)&p->a[2] = var<uint32_t>(game_addr(MORPH_VTX) + 8 * r + 4);
				int32_t *v = (int32_t *)&w->m2;
				v[0] = CrtRand() % 0x1000 - 0x800;
				v[1] = CrtRand() % 0x1000 - 0x800;
				v[2] = CrtRand() % 0x1000 - 0x800;
				Normalize(v, v);
				const int32_t s = CrtRand() % 0x23 + 0xC;
				p->a[4] = (int16_t)(mul32(v[0], s) >> 12);
				p->a[5] = (int16_t)(mul32(s, v[1]) >> 12);
				p->a[6] = (int16_t)(mul32(v[2], s) >> 12);
			}
		}
		MorphSparkSetup((const Mat4x3 *)game_addr(MORPH_MAT));
		uint32_t pk = FrameArena();
		w->v[0] = (int32_t)RenderOT();
		w->v[1] = 4;
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (!(p->flags & 1)) continue;
			pk = MorphSparkDraw(w, p, &p->a[0], pk, !DrawOnly());
		}
		FrameArena() = pk;
		FieldFree(0x58);
		if (DrawOnly()) return 0;
		n->c++;
		if (n->c < 0x24) return 0;
		ClearType(game_addr(POOL_A), game_addr(POOL_A_END), 1);
		return TASK_END;
	}

	static void SpriteAt(uint8_t *h, const int16_t *pos, int16_t scale, int16_t life)
	{
		GteLoadV0(pos);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(scale);
		*(int16_t *)(h + 4) = life;
		GteTransFromMAC();
	}

	static uint32_t __cdecl BurstSparkTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x48);
		if (!DrawOnly() && n->c <= 0x14)
		{
			for (int32_t j = 0; j < 8; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_A), game_addr(POOL_A_END), 0x64);
				if (i < 0) break;
				PoolE *p = Entry(game_addr(POOL_A), i);
				p->flags = 1;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x280 + 0x200);
				const int32_t a = CrtRand() % 0x1000;
				const int32_t r = CrtRand() % 0x258 + 0x258;
				p->a[0] = (int16_t)(mul32(ComputeSin(a), r) >> 12);
				const int32_t up = CrtRand() % 0x1964;
				p->a[1] = (int16_t)(-0x384 - up);
				p->a[2] = (int16_t)(mul32(ComputeCos(a), r) >> 12);
				int32_t *v = (int32_t *)&w->m2;
				v[0] = ComputeSin(a);
				v[1] = CrtRand() % 0x1000 - 0x800;
				v[2] = ComputeCos(a);
				Normalize(v, v);
				const int32_t s = CrtRand() % 0x4B + 0xF;
				p->a[4] = (int16_t)(mul32(s, v[0]) >> 12);
				p->a[5] = (int16_t)(mul32(s, v[1]) >> 12);
				p->a[6] = (int16_t)(mul32(v[2], s) >> 12);
			}
		}
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_BURST);
		*(int16_t *)(h + 0x24) = 0;
		int32_t count = 0;
		PoolLight(&w->m1, w->ang, PointX(), 0, PointZ(), &Camera());
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (!(p->flags & 1)) continue;
			SpriteAt(h, &p->a[0], p->scale, p->life);
			PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
			if (DrawOnly()) continue;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				p->flags = 0;
				continue;
			}
			p->life++;
			p->a[0] = (int16_t)(p->a[0] + p->a[4]);
			p->a[1] = (int16_t)(p->a[1] + p->a[5]);
			p->a[2] = (int16_t)(p->a[2] + p->a[6]);
			count++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		n->c++;
		return count != 0 ? 0 : TASK_END;
	}

	static void ShardDraw(ModelWork *w, uint8_t *h, const PoolE *p, int16_t sy, int32_t life, int32_t fade)
	{
		w->ang[1] = p->a[3];
		ComposeZYXRotationMatrix(w->ang, &w->m2);
		w->m2.t[0] = p->a[0];
		w->m2.t[2] = p->a[2];
		w->m2.t[1] = 0;
		w->v[0] = p->scale;
		w->v[1] = sy;
		Scale3DMatrix(&w->m2, w->v);
		ComposeAffineTransform(&w->m1, &w->m2, &w->m2);
		GteSetRotMatrix(&w->m2);
		GteSetTransVector(&w->m2);
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (life >= 4)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		FrameArena() = RenderPrimModel(h, RenderOT(), 2, FrameArena());
	}

	static void ShardFrame(ModelWork *w, const Mat4x3 *view)
	{
		w->ang[0] = 0;
		w->ang[1] = 0;
		w->ang[2] = 0;
		ComposeZYXRotationMatrix(w->ang, &w->m1);
		w->m1.t[0] = PointX();
		w->m1.t[1] = 0;
		w->m1.t[2] = PointZ();
		ComposeAffineTransform(view, &w->m1, &w->m1);
		w->ang[2] = 0;
		w->ang[0] = 0;
		w->v[2] = 0;
	}

	static uint32_t __cdecl ShardTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x68);
		if (!DrawOnly() && n->c <= 0x1C)
		{
			const int32_t c = n->c;
			const int32_t count = c / 22 + 1;
			const int32_t base = c * 96;
			for (int32_t j = 0; j < count; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_B), game_addr(POOL_B_END), 0x64);
				if (i < 0) break;
				PoolE *p = Entry(game_addr(POOL_B), i);
				p->flags = 1;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x600 + 0x400);
				const int32_t a = CrtRand() % 0x1000;
				const int32_t r = CrtRand() % 0x258 + 0x320;
				p->a[0] = (int16_t)(mul32(ComputeSin(a), r) >> 12);
				p->a[1] = 0;
				p->a[2] = (int16_t)(mul32(ComputeCos(a), r) >> 12);
				p->a[3] = (int16_t)a;
				const int16_t s = (int16_t)((CrtRand() % 0x400 + base + 0x300) / 4);
				p->a[7] = s;
				p->a[5] = s;
			}
		}
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_SHARD);
		*(uint32_t *)(h + 8) = 0;
		ShardFrame(w, &Camera());
		for (uint32_t a = game_addr(POOL_B); a < game_addr(POOL_B_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (!(p->flags & 1)) continue;
			const int16_t life = p->life;
			ShardDraw(w, h, p, p->a[5], life, mul32(life - 4, 682));
			if (DrawOnly()) continue;
			if (life >= 0xA) p->flags = 0;
			else
			{
				p->life = (int16_t)(life + 1);
				p->a[5] = (int16_t)(p->a[5] + p->a[7]);
				const int16_t r = p->a[7];
				p->a[7] = (int16_t)(r - r / 8);
			}
		}
		FieldFree(0x58);
		FieldFree(0x68);
		if (DrawOnly()) return 0;
		n->c++;
		if (n->c < 0x26) return 0;
		ClearType(game_addr(POOL_B), game_addr(POOL_B_END), 1);
		return TASK_END;
	}

	static void SmokeDraw(ModelWork *w, uint8_t *h, const PoolE *p)
	{
		GteLoadV0(&p->a[0]);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(p->scale);
		*(int16_t *)(h + 4) = p->life;
		int32_t *mac = (int32_t *)((uint8_t *)w + 0x38), *dir = (int32_t *)((uint8_t *)w + 0x28);
		GteReadMAC123(mac);
		Normalize(mac, dir);
		const int32_t k = -(p->scale >> 3);
		mac[0] += mul32(k, dir[0]) >> 12;
		mac[1] += mul32(dir[1], k) >> 12;
		mac[2] += mul32(dir[2], k) >> 12;
		GteSetTransFromVec32(mac);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}

	static uint32_t __cdecl SmokeTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x48);
		if (!DrawOnly() && n->c <= 0x14)
		{
			for (int32_t j = 0; j < 5; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_B), game_addr(POOL_B_END), 0x64);
				if (i < 0) break;
				PoolE *p = Entry(game_addr(POOL_B), i);
				p->flags = 1;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0xC00 + 0x300);
				p->a[0] = (int16_t)(CrtRand() % 0x1770 - 0xBB8);
				p->a[1] = (int16_t)-(CrtRand() % 0x5DC);
				p->a[2] = (int16_t)(-0x44C - CrtRand() % 0x1F40);
			}
		}
		uint8_t *h = AllocHeader(0xB4);
		*(uint32_t *)h = game_addr(SEQ_SMOKE);
		*(int16_t *)(h + 0x24) = 8;
		PoolLight(&w->m1, w->ang, PointX(), 0, PointZ(), CamFrame());
		for (uint32_t a = game_addr(POOL_B); a < game_addr(POOL_B_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (!(p->flags & 1)) continue;
			SmokeDraw(w, h, p);
			if (DrawOnly()) continue;
			if (*(const int16_t *)(h + 0x28) < 0) p->flags = 0;
			else p->life++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		n->c++;
		if (n->c < 0x19) return 0;
		ClearType(game_addr(POOL_B), game_addr(POOL_B_END), 1);
		return TASK_END;
	}

	static uint32_t StarfieldDraw(uint32_t pool, uint32_t pool_end, uint32_t pk)
	{
		for (uint32_t a = pool; a < pool_end; a += 0x18)
		{
			const PoolE *p = (const PoolE *)a;
			if (!(*(const uint8_t *)a & 1)) continue;
			*(uint32_t *)pk = 0x2000000;
			*(uint32_t *)(pk + 4) = *(const uint32_t *)&p->a[4];
			*(uint32_t *)(pk + 8) = *(const uint32_t *)&p->a[0];
			InsertPrimAltViewport(OT0() + 0x4478, (void *)pk);
			pk += 0xC;
		}
		return pk;
	}

	static uint32_t __cdecl StarfieldTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		FieldAlloc(0x48);
		if (!DrawOnly() && n->c == 0)
		{
			for (int32_t j = 0; j < 0xA0; j++)
			{
				const int32_t i = FreeSlot(game_addr(POOL_C), game_addr(POOL_C_END), 0xC8);
				if (i < 0) break;
				PoolE *p = Entry(game_addr(POOL_C), i);
				p->flags = 1;
				p->life = 0;
				p->a[0] = (int16_t)(CrtRand() % 0x140);
				p->a[1] = (int16_t)(CrtRand() % 0xD8);
				const int32_t g = CrtRand() % 0xFF;
				p->a[4] = (int16_t)(g | g << 8);
				p->a[5] = (int16_t)(g | 0x6800);
			}
		}
		FrameArena() = StarfieldDraw(game_addr(POOL_C), game_addr(POOL_C_END), FrameArena());
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		n->c++;
		if (n->c < 0x3C) return 0;
		ClearType(game_addr(POOL_C), game_addr(POOL_C_END), 1);
		return TASK_END;
	}

	static uint32_t __cdecl ShakeTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int32_t v = (int32_t)(mul32(ComputeCos(shl32(n->c, 10) / 14), 140) >> 12);
		if (!(n->c & 1)) v = -v;
		var<int16_t>(game_addr(game::BATTLE_CAMERA_SHAKE_OFFSET_Y)) = (int16_t)v;
		if (DrawOnly()) return 0;
		n->c++;
		return EndAt(n->c, 0xE);
	}

	static uint32_t __cdecl RestoreTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->c == 1) TextureRestoreTask((void *)TexBase(), &n->e);
		const int16_t done = n->e;
		n->c++;
		return done != 0 ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *tn)
	{
		RootNode *r = (RootNode *)tn;
		if (r->c & 1) PacketCursor() = TexBase() + 0x10000;
		else PacketCursor() = TexBase();
		const int alive = ExecuteTaskQueue(QMain());
		r->c++;
		return alive ? 0 : TASK_END;
	}
}

	void register_mag164_blasting_zone()
	{
		using namespace blast164;
		register_port(ORIG_Root, (void *)RootTask, "164 Root", 164);
		register_port(ORIG_Director, (void *)DirectorTask, "164 Director", 164);
		register_port(ORIG_FlareA, (void *)FlareATask, "164 FlareA", 164);
		register_port(ORIG_FlareB, (void *)FlareBTask, "164 FlareB", 164);
		register_port(ORIG_OrbitSpark, (void *)OrbitSparkTask, "164 OrbitSpark", 164);
		register_port(ORIG_Swirl, (void *)SwirlTask, "164 Swirl", 164);
		register_port(ORIG_Ring, (void *)RingTask, "164 Ring", 164);
		register_port(ORIG_Spawner, (void *)SpawnerTask, "164 Spawner", 164);
		register_port(ORIG_PulseA, (void *)PulseATask, "164 PulseA", 164);
		register_port(ORIG_PulseB, (void *)PulseBTask, "164 PulseB", 164);
		register_port(ORIG_Shard, (void *)ShardTask, "164 Shard", 164);
		register_port(ORIG_Ball, (void *)BallTask, "164 Ball", 164);
		register_port(ORIG_FrameModel, (void *)FrameModelTask, "164 FrameModel", 164);
		register_port(ORIG_Dome, (void *)DomeTask, "164 Dome", 164);
		register_port(ORIG_Dome2, (void *)Dome2Task, "164 Dome2", 164);
		register_port(ORIG_Beam, (void *)BeamTask, "164 Beam", 164);
		register_port(ORIG_Rock, (void *)RockTask, "164 Rock", 164);
		register_port(ORIG_Debris, (void *)DebrisTask, "164 Debris", 164);
		register_port(ORIG_Starfield, (void *)StarfieldTask, "164 Starfield", 164);
		register_port(ORIG_ScreenFlash, (void *)ScreenFlashTask, "164 ScreenFlash", 164);
		register_port(ORIG_Morph, (void *)MorphTask, "164 Morph", 164);
		register_port(ORIG_RingFlat, (void *)RingFlatTask, "164 RingFlat", 164);
		register_port(ORIG_RingFlat2, (void *)RingFlat2Task, "164 RingFlat2", 164);
		register_port(ORIG_MorphSpark, (void *)MorphSparkTask, "164 MorphSpark", 164);
		register_port(ORIG_BladeGlow, (void *)BladeGlowTask, "164 BladeGlow", 164);
		register_port(ORIG_BurstSpark, (void *)BurstSparkTask, "164 BurstSpark", 164);
		register_port(ORIG_Wave, (void *)WaveTask, "164 Wave", 164);
		register_port(ORIG_Wave2, (void *)Wave2Task, "164 Wave2", 164);
		register_port(ORIG_Smoke, (void *)SmokeTask, "164 Smoke", 164);
		register_port(ORIG_Shake, (void *)ShakeTask, "164 Shake", 164);
		register_port(ORIG_Restore, (void *)RestoreTask, "164 Restore", 164);
	}
}

