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

// Effect 200: No Mercy

#include "mag_common.h"

namespace ff8fx
{
namespace mercy200
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Context() { return var<CastContext *>(game_addr(game::MAG_200_NOMERCY_g_CastContext)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::MAG_200_NOMERCY_g_CasterSlot)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_200_NOMERCY_g_TextureBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_200_NOMERCY_g_PacketCursor)); }
	inline uint32_t &FrameArena() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::MAG_200_NOMERCY_g_RootQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_200_NOMERCY_g_MainQueue); }
	static constexpr game::Id POOL = game::MAG_200_NOMERCY_g_ParticlePool, POOL_END = game::MAG_200_NOMERCY_g_BindOut;
	static constexpr game::Id PTS_BASE = game::MAG_200_NOMERCY_g_TrailBasePoints, PTS_TIP = game::MAG_200_NOMERCY_g_TrailTipPoints;
	static constexpr game::Id SPL_BASE = game::MAG_200_NOMERCY_g_TrailBaseSpline, SPL_TIP = game::MAG_200_NOMERCY_g_TrailTipSpline;
	static constexpr game::Id TIP0 = game::MAG_200_NOMERCY_g_TrailFirstTip, TRAIL_DIR = game::MAG_200_NOMERCY_g_TrailTipDirection, BASE0 = game::MAG_200_NOMERCY_g_TrailFirstBase, BLADE_DIR = game::MAG_200_NOMERCY_g_BladeDirection;
	static constexpr game::Id RING = game::MAG_200_NOMERCY_g_TrailRing, RING_END = game::MAG_200_NOMERCY_g_TextureBase;
	static constexpr game::Id FRAME = game::MAG_200_NOMERCY_g_EffectFrame;
	static constexpr game::Id CAM_FRAME = game::MAG_200_NOMERCY_g_CameraEffectFrame;
	static constexpr game::Id BIND_OUT = game::MAG_200_NOMERCY_g_BindOut;
	static constexpr game::Id POINT = game::MAG_200_NOMERCY_g_TipPoint;
	static constexpr game::Id WAVES = game::MAG_200_NOMERCY_g_ShockWaveRecords;
	static constexpr game::Id TARGET_FREE = game::MAG_200_NOMERCY_FlameTargetFree;

	static constexpr game::Id ORIG_Root = game::MAG_200_NOMERCY_RootTask, ORIG_Director = game::MAG_200_NOMERCY_DirectorTask, ORIG_FlameMulti = game::MAG_200_NOMERCY_FlameDirectorMultiTask, ORIG_FlameSingle = game::MAG_200_NOMERCY_FlameDirectorSingleTask, ORIG_Flame = game::MAG_200_NOMERCY_FlameTask, ORIG_Spark = game::MAG_200_NOMERCY_SparkTask, ORIG_Ember = game::MAG_200_NOMERCY_EmberTask, ORIG_Trail = game::MAG_200_NOMERCY_TrailTask, ORIG_CasterRing = game::MAG_200_NOMERCY_CasterRingTask, ORIG_TipFlash = game::MAG_200_NOMERCY_TipFlashTask, ORIG_TipShards = game::MAG_200_NOMERCY_TipShardsTask, ORIG_Slash = game::MAG_200_NOMERCY_SlashTask, ORIG_SparkBurst = game::MAG_200_NOMERCY_SparkBurstTask, ORIG_FrameModel = game::MAG_200_NOMERCY_FrameModelTask, ORIG_WaveRing = game::MAG_200_NOMERCY_WaveRingTask, ORIG_WaveTrail = game::MAG_200_NOMERCY_WaveTrailTask, ORIG_ImpactFlash = game::MAG_200_NOMERCY_ImpactFlashTask, ORIG_TargetSparks = game::MAG_200_NOMERCY_TargetSparksTask, ORIG_TargetBurst = game::MAG_200_NOMERCY_TargetBurstTask, ORIG_TargetRing = game::MAG_200_NOMERCY_TargetRingTask, ORIG_Restore = game::MAG_200_NOMERCY_RestoreTextureTask;

	static constexpr game::Id ANIM_DATA = game::MAG_200_NOMERCY_SeiferLimitAnimSet, CAMERA_SCRIPT = game::MAG_200_NOMERCY_CameraScript;
	static const void *SOUND_A() { return (const void *)game_addr(game::MAG_200_NOMERCY_SoundA); }
	static const void *SOUND_B() { return (const void *)game_addr(game::MAG_200_NOMERCY_SoundB); }
	static constexpr game::Id REF_DIR = game::MAG_200_NOMERCY_RefDirection;
	static constexpr game::Id SHARD_COLOURS = game::MAG_200_NOMERCY_ShardColours;
	static const void *SEQ_Flame() { return (const void *)game_addr(game::MAG_200_NOMERCY_FlameSequence); }
	static const void *SEQ_FlameGlow() { return (const void *)game_addr(game::MAG_200_NOMERCY_FlameGlowSequence); }
	static const void *SEQ_Ember() { return (const void *)game_addr(game::MAG_200_NOMERCY_EmberSequence); }
	static constexpr game::Id SEQ_SPARKBURST = game::MAG_200_NOMERCY_SparkBurstSequence, SEQ_TSPARK = game::MAG_200_NOMERCY_TargetSparksSequence, SEQ_TBURST = game::MAG_200_NOMERCY_TargetBurstSequence;
	static constexpr game::Id MODEL_RING = game::MAG_200_NOMERCY_CasterRingModel, MODEL_FLASH = game::MAG_200_NOMERCY_FlashModel, MODEL_SHARD = game::MAG_200_NOMERCY_ShardModel, MODEL_SLASH_A = game::MAG_200_NOMERCY_SlashModelA, MODEL_SLASH_B = game::MAG_200_NOMERCY_SlashModelB, MODEL_FRAME = game::MAG_200_NOMERCY_FrameModel, MODEL_WAVERING = game::MAG_200_NOMERCY_WaveRingModel, MODEL_WAVETRAIL = game::MAG_200_NOMERCY_WaveTrailModel, MODEL_TRING = game::MAG_200_NOMERCY_TargetRingModel;
	static const float SPLINE_THREE = 3.0f, SPLINE_SIX = 6.0f;

	inline int32_t PauseQuery() { return fn<int32_t (__cdecl *)()>(game_addr(game::BattleFile_GetLoadingState))(); }
	inline void CameraScriptStart(uint32_t script, uint32_t frame, void *entity, TaskQueue *q, int32_t n) { fn<void (__cdecl *)(uint32_t, uint32_t, void *, TaskQueue *, int32_t)>(game_addr(game::MAG066_CamScript_Start))(script, frame, entity, q, n); }
	inline void ArmCameraReturn() { fn<void (__cdecl *)()>(game_addr(game::BS_Camera_ArmReturnAfterEffect))(); }
	inline void BindEntityModelFromComData(uint8_t *entity, uint32_t out, uint32_t data) { fn<void (__cdecl *)(uint8_t *, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline void QueueChainTransformation(uint8_t *entity, int32_t seq) { fn<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, seq); }
	inline void CharacterLoad(int32_t id, uint32_t dst) { fn<void (__cdecl *)(int32_t, uint32_t)>(game_addr(game::BattleFile_CharacterLoad))(id, dst); }
	inline void QueueTIMUpload(uint32_t tim) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::Battle_QueueTIMUpload_GetEOF))(tim); }
	inline void GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t a, void *out) { fn<void (__cdecl *)(uint8_t *, int32_t, int32_t, void *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, a, out); }
	inline int32_t Normalize(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline void MatVec(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void GteMatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
	inline void GteSetFarColorB(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColorRGB))(r, g, b); }
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
	inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
	inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
	inline void GteSetBackColorFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
	inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
	inline void GteSetTransFromVec32(const int32_t *v) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_SetTransFromVec32))(v); }
	inline uint32_t Mag151Render(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_151_CannonBlow_ScrollRender))(h, ot, mode, cursor); }
	inline void EffSplineSetup(int32_t n, const void *points, void *buf) { fn<void (__cdecl *)(int32_t, const void *, void *)>(game_addr(game::Effect_SplineSetup))(n, points, buf); }
	inline void EffSplineEval(int32_t n, const void *buf, void *out, int32_t t) { fn<void (__cdecl *)(int32_t, const void *, void *, int32_t)>(game_addr(game::Effect_SplineEval))(n, buf, out, t); }
	inline uint32_t OT0() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t c;
		int16_t e;
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
		uint32_t type;
		int16_t life;
		int16_t scale;
		int16_t a[8];
	};
	struct RingEntry
	{
		uint32_t alive;
		uint32_t base[2];
		uint32_t tip[2];
	};
	struct WaveRec
	{
		int16_t p0[4];
		int16_t p1[4];
		int16_t cp[4][4];
		int16_t pt[64][4];
		Mat4x3 mat;
	};
	struct TrailWork
	{
		uint8_t pad00[8];
		int32_t otz;
		uint32_t flag;
		uint32_t col0;
		uint32_t col1;
		uint8_t pad18[4];
		uint32_t base_col;
		int16_t base[4];
		int16_t tip[4];
		int32_t dir[4];
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
	struct ModelWork
	{
		int16_t ang[4];
		Mat4x3 m1;
		Mat4x3 m2;
		int32_t v[4];
		int32_t w[4];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node24) == 0x24 && sizeof(PoolE) == 0x18, "No Mercy nodes");
	static_assert(sizeof(RingEntry) == 0x14 && sizeof(WaveRec) == 0x250 && sizeof(TrailWork) == 0x40, "No Mercy records");
	static_assert(sizeof(RenderHeader) == 0x6C && sizeof(ModelWork) == 0x68, "No Mercy blocks");

	template<typename T> static inline T &At(uint32_t base, int32_t off) { return *(T *)(base + off); }
	inline uint8_t *Caster() { return Entity(CasterSlot()); }
	inline PoolE *Entry(int32_t i) { return (PoolE *)(game_addr(POOL) + 0x18 * i); }
	inline WaveRec *Wave(int32_t i) { return (WaveRec *)(game_addr(WAVES) + 0x250 * i); }
	inline ActionData *Action0() { return Context()->actions; }

	static int32_t FreeSlot()
	{
		int32_t i = 0;
		for (uint32_t a = game_addr(POOL); *(const uint32_t *)a != 0; i++)
		{
			a += 0x18;
			if (a >= game_addr(POOL_END)) return -1;
		}
		return i >= 0x64 ? -1 : i;
	}

	static inline uint32_t EndAt(int16_t c, int16_t limit) { return c >= limit ? TASK_END : 0; }
	static inline uint32_t EndIfNone(int32_t count) { return count != 0 ? 0 : TASK_END; }
	static inline int16_t RandMask(int32_t mask) { return (int16_t)(CrtRand() % (mask + 1)); }
}
}

namespace ff8fx
{
namespace mercy200
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

	static inline int16_t Ftol16(double v)
	{
		if (!(v >= -9223372036854775808.0 && v < 9223372036854775808.0)) return 0;
		return (int16_t)(uint16_t)(uint64_t)(int64_t)v;
	}

	static void SplineSolve(int32_t n, const float *X, const float *Y, float *M)
	{
		float *H = (float *)FieldAlloc(0x44);
		float *D = H + n;
		*(uint32_t *)&M[n - 1] = 0;
		*(uint32_t *)&M[0] = 0;
		for (int32_t i = 0; i < n - 1; i++)
		{
			const double h = (double)X[i + 1] - (double)X[i];
			H[i] = (float)h;
			D[i + 1] = (float)(((double)Y[i + 1] - (double)Y[i]) / h);
		}
		M[1] = (float)((double)D[2] - (double)D[1]);
		D[1] = (float)(((double)X[2] - (double)X[0]) * 2.0);
		for (int32_t k = 2; k <= n - 2; k++)
		{
			const double t = (double)H[k - 1] / (double)D[k - 1];
			M[k] = (float)(((double)D[k + 1] - (double)D[k]) - t * (double)M[k - 1]);
			D[k] = (float)(((double)X[k + 1] - (double)X[k - 1]) * 2.0 - t * (double)H[k - 1]);
		}
		M[n - 2] = (float)((double)M[n - 2] - (double)H[n - 2] * (double)M[n - 1]);
		for (int32_t j = n - 2; j > 0; j--)
			M[j] = (float)(((double)M[j] - (double)H[j] * (double)M[j + 1]) / (double)D[j]);
		FieldFree(0x44);
	}

	static void SplineEval(const float *X, const float *Y, const float *M, int32_t seg, int32_t steps, float *out)
	{
		const int32_t s = (int16_t)seg;
		const int32_t last = (int16_t)(steps - 1);
		const float yb = Y[s + 1];
		out[last] = yb;
		double y = Y[s];
		out[0] = (float)y;
		if ((int16_t)(steps - 1) <= 1) return;
		const double h = (double)X[s + 1] - (double)X[s];
		const double h2 = h * h;
		const float p = (float)(((double)Y[s] * 2.0 + (double)Y[s + 1]) - ((double)M[s] * 2.0 + (double)M[s + 1]) * h2);
		const double q = ((double)Y[s + 1] * 2.0 + (double)Y[s]) - ((double)M[s + 1] * 2.0 + (double)M[s]) * h2;
		const float nf = (float)last;
		const float a3 = (float)((((((double)p + (double)yb) - q) - y) / (double)nf) / (double)nf / (double)nf);
		const float y3 = (float)(y * (double)SPLINE_THREE);
		const float b2 = (float)(((q - (double)p * 2.0) + (double)y3) / (double)nf / (double)nf);
		double c = ((double)p - (double)y3) / (double)nf - ((double)b2 - (double)a3);
		const double a6 = (double)a3 * (double)SPLINE_SIX;
		const float a6f = (float)a6;
		double e = ((double)b2 - a6) + (double)b2;
		for (int32_t i = 1; i <= last; i++)
		{
			e += (double)a6f;
			c += e;
			y += c;
			out[i] = (float)y;
		}
	}

	static void SplineCurve(int32_t n, const int16_t *src, int32_t total, int16_t *dst)
	{
		uint8_t *a = (uint8_t *)FieldAlloc(0x190);
		float *X = (float *)a, *Y = (float *)(a + 0x44), *M = (float *)(a + 0x88), *S = (float *)(a + 0xCC);
		for (int32_t i = 0; i < n; i++)
		{
			X[i] = (float)i;
			Y[i] = (float)src[4 * i];
		}
		for (int32_t k = 0; k < 3; k++)
		{
			if (k)
				for (int32_t i = 0; i < n; i++) Y[i] = (float)src[4 * i + k];
			SplineSolve(n, X, Y, M);
			if (n - 1 > 0)
			{
				const int32_t q = total / (n - 1);
				for (int32_t seg = 0; seg < n - 1; seg++) SplineEval(X, Y, M, seg, q + 1, S + q * seg);
			}
			for (int32_t i = 0; i < total; i++) dst[4 * i + k] = Ftol16((double)S[i]);
		}
		FieldFree(0x190);
	}

	static void TrailPoints(TrailWork *w)
	{
		WeaponBone(Caster(), 1, w->dir, w->base);
		w->tip[0] = (int16_t)((int16_t)(mul32(w->dir[0], 737) >> 12) + w->base[0]);
		w->tip[1] = (int16_t)((int16_t)(mul32(w->dir[1], 737) >> 12) + w->base[1]);
		w->tip[2] = (int16_t)((int16_t)(mul32(w->dir[2], 737) >> 12) + w->base[2]);
	}

	static int32_t TrailGather(const uint8_t *ring, int32_t idx, int32_t want, uint8_t *pa, uint8_t *pb)
	{
		int32_t cnt = 0;
		if (want <= 0) return 0;
		do
		{
			const RingEntry *e = (const RingEntry *)(ring + 0x14 * idx);
			if (!e->alive) break;
			idx--;
			*(uint32_t *)(pa + 8 * cnt) = e->base[0];
			*(uint32_t *)(pa + 8 * cnt + 4) = e->base[1];
			*(uint32_t *)(pb + 8 * cnt) = e->tip[0];
			*(uint32_t *)(pb + 8 * cnt + 4) = e->tip[1];
			if (idx < 0) idx = 5;
			cnt++;
		} while (cnt < want);
		return cnt;
	}

	static void TrailDirections(const uint8_t *sa, const uint8_t *sb)
	{
		var<uint32_t>(game_addr(TIP0)) = *(const uint32_t *)sb;
		var<uint32_t>(game_addr(TIP0) + 4) = *(const uint32_t *)(sb + 4);
		int32_t *d = (int32_t *)game_addr(TRAIL_DIR);
		d[0] = (int32_t)*(const int16_t *)sb - *(const int16_t *)(sb + 8);
		d[1] = (int32_t)*(const int16_t *)(sb + 2) - *(const int16_t *)(sb + 0xA);
		d[2] = (int32_t)*(const int16_t *)(sb + 4) - *(const int16_t *)(sb + 0xC);
		Normalize(d, d);
		var<uint32_t>(game_addr(BASE0)) = *(const uint32_t *)sa;
		var<uint32_t>(game_addr(BASE0) + 4) = *(const uint32_t *)(sa + 4);
		int32_t *b = (int32_t *)game_addr(BLADE_DIR);
		b[0] = (int32_t)var<int16_t>(game_addr(TIP0)) - *(const int16_t *)sa;
		b[1] = (int32_t)var<int16_t>(game_addr(TIP0) + 2) - *(const int16_t *)(sa + 2);
		b[2] = (int32_t)var<int16_t>(game_addr(TIP0) + 4) - *(const int16_t *)(sa + 4);
		Normalize(b, b);
	}

	static void TrailDraw(const uint8_t *sa, const uint8_t *sb, TrailWork *w)
	{
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteSetFarColorB(0, 0, 0);
		uint32_t *pk = (uint32_t *)PacketCursor();
		uint32_t *pk2 = pk + 9;
		w->base_col = 0x3A185820;
		w->col1 = 0x3A185820;
		int32_t tacc = 0;
		for (int32_t off = 0; off < 0xF0; off += 8, tacc += 0x1000)
		{
			w->col0 = w->col1;
			pk[0] = 0x8000000;
			GteLoadV012(sa + off, sa + off + 8, sb + off);
			GteRTPT();
			GteReadFLAG(&w->flag);
			if (w->flag & 0x60000) continue;
			GteReadSXY012Split(pk + 2, pk + 4, pk + 6);
			GteLoadV0(sb + off + 8);
			GteRTPS();
			GteReadSXY2(pk + 8);
			GteAVSZ4();
			GteStoreOTZ(&w->otz);
			GteSetIR0(tacc / 30);
			GteLoadRGBC(&w->base_col);
			GteDPCS();
			GteStoreRGB2(&w->col1);
			pk[5] = pk[1] = w->col0;
			pk[7] = pk[3] = w->col1;
			InsertPrimAutoDepth(OT0() + 4 * (uint32_t)(w->otz >> 2) + 0x44, pk);
			pk2[5] = w->col0;
			pk2[3] = pk2[1] = 0x3A000000;
			pk2[7] = w->col1;
			pk2[4] = pk[4];
			pk2[2] = pk[2];
			pk2[6] = pk[6];
			pk2[8] = pk[8];
			pk2[0] = 0x8000000;
			InsertPrimAutoDepth(OT0() + 4 * (uint32_t)(w->otz >> 2) + 0x44, pk2);
			pk += 0x12;
			pk2 += 0x12;
		}
		PacketCursor() = (uint32_t)pk2;
	}

	static int32_t TrailWant(int32_t c) { return c >= 0x93 ? 0x99 - c : 6; }

	static uint32_t __cdecl TrailTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		TrailWork *w = (TrailWork *)FieldAlloc(0x40);
		TrailPoints(w);
		const int32_t c = n->c;
		const int32_t idx = c % 6;
		RingEntry *e = (RingEntry *)(game_addr(RING) + 0x14 * idx);
		e->alive = 1;
		e->base[0] = *(const uint32_t *)&w->base[0];
		e->base[1] = *(const uint32_t *)&w->base[2];
		e->tip[0] = *(const uint32_t *)&w->tip[0];
		e->tip[1] = *(const uint32_t *)&w->tip[2];
		const int32_t cnt = TrailGather((const uint8_t *)game_addr(RING), idx, TrailWant(c), (uint8_t *)game_addr(PTS_BASE), (uint8_t *)game_addr(PTS_TIP));
		if (cnt > 1)
		{
			SplineCurve(cnt, (const int16_t *)game_addr(PTS_BASE), 0x20, (int16_t *)game_addr(SPL_BASE));
			SplineCurve(cnt, (const int16_t *)game_addr(PTS_TIP), 0x20, (int16_t *)game_addr(SPL_TIP));
			TrailDirections((const uint8_t *)game_addr(SPL_BASE), (const uint8_t *)game_addr(SPL_TIP));
			TrailDraw((const uint8_t *)game_addr(SPL_BASE), (const uint8_t *)game_addr(SPL_TIP), w);
		}
		FieldFree(0x40);
		if (DrawOnly()) return 0;
		n->c++;
		return EndAt(n->c, 0x98);
	}

	static void FlameDraw(const Node24 *p)
	{
		const int16_t *pos = &p->w10;
		TransformCameraByShadowRotation(pos, p->w1C, -(p->w1C >> 4));
		uint8_t *h = AllocHeader(0xB4);
		*(const void **)h = SEQ_Flame();
		*(uint16_t *)(h + 4) = (uint16_t)p->c;
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		int16_t angles[4] = { 0x400, 0, 0, 0 };
		Mat4x3 m = {};
		BuildRotationMatrixFromAngles(angles, &m);
		m.t[0] = pos[0];
		m.t[1] = p->w16;
		m.t[2] = pos[2];
		int32_t sv[4] = { p->w1C, p->w1C, 0x1000, 0 };
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		int32_t y = pos[1];
		if (y < -2000) y = -2000;
		int32_t q = (int32_t)(((int64_t)shl32(y, 12) * 0x10624DD3) >> 32) >> 7;
		q += (int32_t)((uint32_t)q >> 31);
		uint8_t level = (uint8_t)((q * 160) / 4096 + 0xA0);
		*(const void **)h = SEQ_FlameGlow();
		*(uint16_t *)(h + 0x24) = 0xC;
		*(uint16_t *)(h + 4) = (uint16_t)p->c;
		h[0x1E] = level;
		h[0x1D] = level;
		h[0x1C] = level;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static uint32_t __cdecl FlameTask(TaskNode *tn)
	{
		Node24 *p = (Node24 *)tn;
		if (p->e > 0)
		{
			if (DrawOnly()) return 0;
			p->e--;
			return 0;
		}
		FlameDraw(p);
		if (DrawOnly()) return 0;
		if (p->c == 0)
		{
			for (int k = 0; k < 4; k++)
			{
				Node24 *e = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_Ember));
				e->c = 0;
				e->e = (int16_t)(CrtRand() % 6 + k);
				ff8fx::copy<(8)>(&e->w10, &p->w10);
				e->w10 += (int16_t)(CrtRand() % 300 - 150);
				e->w12 -= (int16_t)(CrtRand() % 100);
				e->w14 += (int16_t)(CrtRand() % 300 - 150);
				e->w16 = (int16_t)(-20 - CrtRand() % 30);
				e->w1C = (int16_t)(CrtRand() % 0x500 + 0xA00);
			}
			for (int k = 0; k < 10; k++)
			{
				Node24 *s = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_Spark));
				s->c = (int16_t)(CrtRand() % 4 + 5);
				s->e = (int16_t)(CrtRand() % 4 + k);
				ff8fx::copy<(8)>(&s->w10, &p->w10);
				s->w10 += (int16_t)(CrtRand() % 400 - 200);
				s->w12 -= (int16_t)(CrtRand() % 200);
				s->w14 += (int16_t)(CrtRand() % 400 - 200);
				s->w18 = (int16_t)(CrtRand() % 80 - 40);
				s->w1A = (int16_t)(-20 - CrtRand() % 50);
				s->w1C = (int16_t)(CrtRand() % 80 - 40);
				s->w20 = (int16_t)(CrtRand() % 0x600 + 0x900);
			}
		}
		p->c++;
		return p->c >= 0x10 ? TASK_END : 0;
	}

	static void SparkDraw(const Node24 *p)
	{
		TransformCameraByShadowRotation(&p->w10, p->w20, -(p->w20 >> 3));
		uint8_t *h = AllocHeader(0xB4);
		*(void **)h = EffectSpriteSharedSequence(5);
		*(uint16_t *)(h + 4) = (uint16_t)(9 - p->c);
		h[0x20] = 4;
		*(uint16_t *)(h + 0x24) = 0x10;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void SparkUpdate(Node24 *p)
	{
		int16_t v = p->w18;
		p->w20 = (int16_t)(p->w20 - 0x40);
		p->w10 = (int16_t)(p->w10 + v);
		p->w18 = (int16_t)(v - (int16_t)(v >> 4));
		v = p->w1A;
		p->w12 = (int16_t)(p->w12 + v);
		p->w1A = (int16_t)(v + (int16_t)(v >> 4));
		v = p->w1C;
		p->w14 = (int16_t)(p->w14 + v);
		p->w1C = (int16_t)(v - (int16_t)(v >> 4));
		p->c--;
	}

	static uint32_t __cdecl SparkTask(TaskNode *tn)
	{
		Node24 *p = (Node24 *)tn;
		if (p->e > 0)
		{
			if (DrawOnly()) return 0;
			p->e--;
			return 0;
		}
		SparkDraw(p);
		if (DrawOnly()) return 0;
		SparkUpdate(p);
		return p->c < 0 ? TASK_END : 0;
	}

	static void EmberDraw(const Node24 *p)
	{
		TransformCameraByShadowRotation(&p->w10, p->w1C, -(p->w1C >> 3));
		uint8_t *h = AllocHeader(0xB4);
		*(uint16_t *)(h + 4) = (uint16_t)p->c;
		*(const void **)h = SEQ_Ember();
		*(uint16_t *)(h + 0x24) = 0;
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
		FieldFree(0xB4);
	}

	static void EmberUpdate(Node24 *p)
	{
		int16_t v = p->w16;
		p->w12 = (int16_t)(p->w12 + v);
		p->w16 = (int16_t)(v + (int16_t)(v >> 5));
		int16_t s = p->w1C;
		p->w1C = (int16_t)(s - (int16_t)(s >> 6));
		p->c++;
	}

	static uint32_t __cdecl EmberTask(TaskNode *tn)
	{
		Node24 *p = (Node24 *)tn;
		if (p->e > 0)
		{
			if (DrawOnly()) return 0;
			p->e--;
			return 0;
		}
		EmberDraw(p);
		if (DrawOnly()) return 0;
		EmberUpdate(p);
		return p->c >= 0x10 ? TASK_END : 0;
	}

	static void SpawnNextDirector(const Node24 *d)
	{
		ActionData *acts = Context()->actions;
		const int32_t next = d->e + 1;
		if (next > acts[0].last_action) return;
		Node24 *t;
		if (acts[next].target_count > 1)
		{
			t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_FlameMulti));
			t->c = 0;
			t->e = (int16_t)next;
		}
		else
		{
			t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_FlameSingle));
			t->e = (int16_t)next;
			t->c = 0;
			t->w20 = (int16_t)(uint16_t)Context()->actions[next].targets[0];
		}
		t->w22 = 0;
	}

	static int32_t SpreadOf(int32_t slot)
	{
		const int32_t s = mul32(EntitySize(slot), 2800) >> 12;
		return s > 1000 ? 1000 : s;
	}

	static void AddFlame(int32_t slot, int32_t spread, const int16_t *pos, int32_t k)
	{
		const int32_t half = spread >> 1;
		Node24 *f = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_Flame));
		f->c = 0;
		f->e = (int16_t)(CrtRand() % 6 + 2 * k);
		ff8fx::copy<(8)>(&f->w10, pos);
		f->w10 += (int16_t)(CrtRand() % spread - half);
		f->w12 += (int16_t)(CrtRand() % spread - half);
		const int32_t rz = CrtRand() % spread;
		const int16_t h = EntityHeight(slot);
		f->w16 = h;
		f->w14 += (int16_t)(rz - half);
		if (f->w12 > h - 0x226)
			f->w12 += (int16_t)(-500 - CrtRand() % 300);
		f->w1C = (int16_t)(CrtRand() % 0x600 + 0xD00);
	}

	static uint32_t __cdecl FlameMultiTask(TaskNode *tn)
	{
		Node24 *d = (Node24 *)tn;
		if (DrawOnly()) return 0;
		if (d->w22 == 12) SpawnNextDirector(d);
		d->w22++;
		if (d->c == 1)
		{
			ActionData *ad = &Context()->actions[d->e];
			if (ad->target_count > 0)
			{
				int32_t off = 0;
				for (int32_t t = 0;;)
				{
					const int32_t slot = ad->targets[off];
					const int32_t spread = SpreadOf(slot);
					int16_t pos[4];
					GetDefaultEffectPosition(Entity(slot), pos);
					for (int32_t k = 0; k < 3; k++) AddFlame(slot, spread, pos, k);
					off += TARGET_STRIDE;
					t++;
					ad = &Context()->actions[d->e];
					if (t >= ad->target_count) break;
				}
			}
		}
		d->c++;
		return d->c >= 0x16 ? TASK_END : 0;
	}

	static uint32_t __cdecl FlameSingleTask(TaskNode *tn)
	{
		Node24 *d = (Node24 *)tn;
		if (DrawOnly()) return 0;
		if (d->w22 == 12) SpawnNextDirector(d);
		const int32_t slot = d->w20;
		d->w22++;
		uint32_t *free_flag = (uint32_t *)game_addr(TARGET_FREE);
		if (free_flag[slot] == 0 && d->c == 0) return 0;
		if (d->c == 0) free_flag[slot] = 0;
		if (d->c == 1)
		{
			const int32_t spread = SpreadOf(slot);
			int16_t pos[4];
			GetDefaultEffectPosition(Entity(slot), pos);
			const int32_t half = spread >> 1;
			for (int32_t k = 0; k < 4; k++)
			{
				Node24 *f = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_Flame));
				f->c = 0;
				f->e = (int16_t)(CrtRand() % 6 + 2 * k);
				ff8fx::copy<(8)>(&f->w10, pos);
				f->w10 += (int16_t)(CrtRand() % spread - half);
				f->w12 += (int16_t)(CrtRand() % spread - half);
				f->w14 += (int16_t)(CrtRand() % spread - half);
				const int16_t h = EntityHeight(d->w20);
				f->w16 = h;
				if (f->w12 > h - 0x226)
					f->w12 += (int16_t)(-500 - CrtRand() % 300);
				f->w1C = (int16_t)(CrtRand() % 0x600 + 0xD00);
			}
		}
		d->c++;
		if (d->c < 0x16) return 0;
		free_flag[d->w20] = 1;
		return TASK_END;
	}

	static void AddFlameDirector()
	{
		uint32_t *free_flag = (uint32_t *)game_addr(TARGET_FREE);
		for (int32_t k = 0; k < 7; k++) free_flag[k] = 1;
		ActionData *ad = Context()->actions;
		if (ad->target_count > 1)
		{
			Node24 *t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_FlameMulti));
			t->c = 0;
			t->e = 0;
			t->w22 = 0;
			return;
		}
		Node24 *t = (Node24 *)AddTaskToQueue(QMain(), game_addr(ORIG_FlameSingle));
		t->c = 0;
		t->e = 0;
		t->w22 = 0;
		t->w20 = (int16_t)(uint16_t)Context()->actions[0].targets[0];
	}

	static uint8_t *ModelHeader(uint32_t model, bool faded, int32_t fade)
	{
		uint8_t *h = AllocHeader(0x58);
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

	static void ModelMatrix(const Mat4x3 *view, const int16_t *ang, const int16_t *pos, int32_t sx, int32_t sy, int32_t sz)
	{
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = pos[0];
		m.t[1] = pos[1];
		m.t[2] = pos[2];
		int32_t sv[3];
		sv[0] = sx;
		sv[1] = sy;
		sv[2] = sz;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(view, &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}

	static void CasterRingDraw(const Node24 *n, int32_t scroll, bool faded, int32_t fade)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = n->w18;
		ang[2] = 0;
		ModelMatrix(&Camera(), ang, &n->w10, n->w1C, n->w20, n->w1C);
		uint8_t *h = AllocHeader(0x7C);
		*(uint32_t *)(h + 8) = 0;
		*(uint16_t *)(h + 0x1A) = 0x80;
		*(uint16_t *)(h + 0x1E) = 0x80;
		*(uint16_t *)(h + 0x18) = 0;
		*(uint32_t *)(h + 0x10) = 0;
		*(uint32_t *)h = game_addr(MODEL_RING);
		*(uint32_t *)(h + 0x20) = 0x33;
		*(uint16_t *)(h + 0x1C) = 0x40;
		*(int32_t *)(h + 0x14) = scroll;
		if (faded)
		{
			*(int32_t *)(h + 0xC) = fade;
			*(uint32_t *)(h + 0x20) = 0xF3;
		}
		PacketCursor() = Mag151Render(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x7C);
	}
	static bool CasterRingFaded(int32_t c) { return c < 8 || c >= 0x20; }
	static int32_t CasterRingFade(int32_t c) { return c < 8 ? shl32(8 - c, 9) : shl32(c - 0x20, 9); }

	static uint32_t __cdecl CasterRingTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		CasterRingDraw(n, shl32(n->c, 3), CasterRingFaded(n->c), CasterRingFade(n->c));
		if (DrawOnly()) return 0;
		n->c++;
		return EndAt(n->c, 0x28);
	}

	static void TipFlashDraw(const Node24 *n, int16_t roll, int16_t scale, bool faded, int32_t fade)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = 0x40;
		ang[2] = roll;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t sv[3];
		sv[2] = scale;
		sv[1] = scale;
		sv[0] = scale;
		Scale3DMatrix(&m, sv);
		TransformCameraByShadowRotation(&n->w10, scale, -(scale >> 3));
		GteSetRotMatrix(&m);
		uint8_t *h = ModelHeader(game_addr(MODEL_FLASH), faded, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl TipFlashTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		TipFlashDraw(n, n->w18, n->w1C, n->c >= 0xA, mul32(n->c - 0xA, 682));
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w18 = (int16_t)(n->w18 + n->w1A);
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 12);
		n->c++;
		return EndAt(n->c, 0x10);
	}

	static void SlashPoint()
	{
		var<int16_t>(game_addr(POINT)) = (int16_t)((mul32(var<int32_t>(game_addr(BLADE_DIR)), 400) >> 12) + var<int32_t>(game_addr(BASE0)));
		var<int16_t>(game_addr(POINT) + 2) = (int16_t)((int16_t)(mul32(var<int32_t>(game_addr(BLADE_DIR) + 4), 400) >> 12) + var<int16_t>(game_addr(BASE0) + 2));
		var<int16_t>(game_addr(POINT) + 4) = (int16_t)((mul32(var<int32_t>(game_addr(BLADE_DIR) + 8), 400) >> 12) + var<int32_t>(game_addr(BASE0) + 4));
	}

	static void SlashDraw(const Node24 *n, const int16_t *point, int16_t ax, int16_t az, int16_t scale, bool faded, int32_t fade)
	{
		int16_t ang[4];
		ang[0] = ax;
		ang[1] = 0;
		ang[2] = az;
		ModelMatrix(&Camera(), ang, point, scale, scale, scale);
		uint8_t *h = ModelHeader((n->e & 1) ? game_addr(MODEL_SLASH_B) : game_addr(MODEL_SLASH_A), faded, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl SlashTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->e == 0 && n->c >= 0x1E && n->c <= 0x23) SlashPoint();
		SlashDraw(n, (const int16_t *)game_addr(POINT), n->w18, n->w20, n->w1C, n->c < 0xC, 0x1000 - mul32(n->c, 341));
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w18 = (int16_t)(n->w18 + n->w1A);
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 18);
		n->w20 = (int16_t)(n->w20 + n->w22);
		n->c++;
		return EndAt(n->c, 0x25);
	}

	static void FrameModelDraw(const Mat4x3 *cam_frame, const Node24 *n, int16_t scale, bool faded, int32_t fade)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = 0;
		ModelMatrix(cam_frame, ang, &n->w10, scale, scale, scale);
		uint8_t *h = ModelHeader(game_addr(MODEL_FRAME), faded, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl FrameModelTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		FrameModelDraw((const Mat4x3 *)game_addr(CAM_FRAME), n, n->w1C, n->c >= 8, shl32(n->c - 8, 9));
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 6);
		n->c++;
		return EndAt(n->c, 0x10);
	}

	static void WaveRingDraw(const Node24 *n, int16_t ax, const int16_t *pos, int32_t fade)
	{
		const WaveRec *rec = Wave(n->e);
		int16_t ang[4];
		ang[0] = ax;
		ang[1] = 0;
		ang[2] = 0;
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		GteMatrixMultiply(&rec->mat, &m);
		m.t[0] = pos[0];
		m.t[1] = pos[1];
		m.t[2] = pos[2];
		int32_t sv[3];
		sv[2] = n->w1C;
		sv[1] = n->w1C;
		sv[0] = n->w1C;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_WAVERING);
		*(int32_t *)(h + 0xC) = fade;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0xF3;
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl WaveRingTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->w16 > 0)
		{
			if (DrawOnly()) return 0;
			n->w16--;
			return 0;
		}
		WaveRingDraw(n, n->w18, Wave(n->e)->pt[n->w20], mul32(n->w14 * 3, 0x100));
		if (DrawOnly()) return 0;
		int16_t step = n->w22;
		const int16_t c = n->c;
		n->w18 = (int16_t)(n->w18 + n->w1A);
		n->w20 = (int16_t)(n->w20 + step);
		if (c > 4 && step < 6)
		{
			step = (int16_t)(step + 2);
			n->w22 = step;
		}
		n->c = (int16_t)(c + 1);
		return n->w20 >= 0x40 ? TASK_END : 0;
	}

	static void ImpactFlashDraw(const Node24 *n, int16_t scale, bool faded, int32_t fade)
	{
		int16_t ang[4];
		ang[0] = n->w18;
		ang[2] = 0;
		ang[1] = n->w1A;
		ModelMatrix(&Camera(), ang, &n->w10, scale, scale, scale);
		uint8_t *h = ModelHeader(game_addr(MODEL_FLASH), faded, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl ImpactFlashTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ImpactFlashDraw(n, n->w1C, n->c >= 6, shl32(n->c - 6, 9));
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 4);
		n->c++;
		return EndAt(n->c, 0xE);
	}

	static void TargetRingDraw(const Node24 *n, int16_t ay, int16_t scale, bool faded, int32_t fade)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = ay;
		ang[2] = 0;
		ModelMatrix(&Camera(), ang, &n->w10, scale, scale, scale);
		uint8_t *h = ModelHeader(game_addr(MODEL_TRING), faded, fade);
		PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl TargetRingTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		TargetRingDraw(n, n->w18, n->w1C, n->c >= 8, shl32(n->c - 8, 9));
		if (DrawOnly()) return 0;
		const int16_t rate = n->w1A;
		const int16_t r = n->w1E;
		n->w18 = (int16_t)(n->w18 + rate);
		n->w1C = (int16_t)(n->w1C + r);
		n->w1A = (int16_t)(rate - (int16_t)(rate >> 4));
		n->w1E = (int16_t)(r - r / 8);
		n->c++;
		return EndAt(n->c, 0x10);
	}

	static void ShardSpawn()
	{
		const int32_t i = FreeSlot();
		if (i < 0) return;
		PoolE *p = Entry(i);
		p->type = 1;
		p->life = 0;
		p->scale = (int16_t)(CrtRand() % 0x1800 + 0x400);
		p->a[0] = RandMask(7);
		p->a[1] = 0;
		p->a[2] = RandMask(0xFFF);
		p->a[5] = (int16_t)(-8 - CrtRand() % 0x18);
		int16_t rate = (int16_t)(CrtRand() % 0x28 + 0xA);
		p->a[6] = rate;
		if (p->a[2] & 1) p->a[6] = (int16_t)-rate;
	}

	static bool ShardFaded(int32_t life) { return life < 6 || life >= 0xA; }
	static int32_t ShardFade(int32_t life) { return life < 6 ? 0x1000 - mul32(life, 682) : mul32(life - 0xA, 682); }

	static void ShardDraw(ModelWork *w, RenderHeader *h, const PoolE *p, int16_t roll, bool faded, int32_t fade)
	{
		w->ang[2] = roll;
		ComposeZYXRotationMatrix(w->ang, &w->m2);
		w->v[0] = p->scale;
		Scale3DMatrix(&w->m2, w->v);
		GteSetRotMatrix(&w->m2);
		h->flags = 0x33;
		if (faded)
		{
			h->fade = fade;
			h->flags = 0xF3;
		}
		h->colour = At<uint32_t>(game_addr(SHARD_COLOURS), 4 * p->a[0]);
		PacketCursor() = RenderModel(h, RenderOT(), 2, PacketCursor());
	}

	static uint32_t __cdecl TipShardsTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x68);
		if (!DrawOnly() && n->c <= 0x10) ShardSpawn();
		RenderHeader *h = (RenderHeader *)FieldAlloc(0x6C);
		h->model = game_addr(MODEL_SHARD);
		*(uint32_t *)h->far_rgb = 0;
		int32_t count = 0;
		TransformCameraByShadowRotation((const void *)game_addr(POINT), 0x1000, 0);
		w->v[2] = 0x1000;
		w->v[1] = 0x1000;
		w->ang[1] = 0;
		w->ang[0] = 0;
		for (uint32_t a = game_addr(POOL); a < game_addr(POOL_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (!(p->type & 1)) continue;
			ShardDraw(w, h, p, p->a[2], ShardFaded(p->life), ShardFade(p->life));
			if (DrawOnly()) continue;
			if (p->life >= 0x10)
			{
				p->type = 0;
				continue;
			}
			p->life++;
			int16_t k = (int16_t)(p->a[0] + 1);
			p->a[0] = k;
			if (k >= 8) p->a[0] = 0;
			p->a[1] = (int16_t)(p->a[1] + p->a[5]);
			p->a[2] = (int16_t)(p->a[2] + p->a[6]);
			count++;
		}
		FieldFree(0x6C);
		FieldFree(0x68);
		if (DrawOnly()) return 0;
		n->c++;
		return EndIfNone(count);
	}

	static void SpriteDraw(ModelWork *w, uint8_t *h, const PoolE *p, int32_t life, const int16_t *pos, int32_t pull)
	{
		GteLoadV0(pos);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(p->scale);
		*(int16_t *)(h + 4) = (int16_t)life;
		int32_t *mac = (int32_t *)((uint8_t *)w + 0x38);
		int32_t *dir = (int32_t *)((uint8_t *)w + 0x28);
		GteReadMAC123(mac);
		Normalize(mac, dir);
		mac[0] = mac[0] + (mul32(dir[0], pull) >> 12);
		mac[1] = mac[1] + (mul32(dir[1], pull) >> 12);
		mac[2] = mac[2] + (mul32(dir[2], pull) >> 12);
		GteSetTransFromVec32(mac);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}

	static void SparkBurstSpawn(ModelWork *w, int32_t c)
	{
		const int32_t n6 = 6 * c, n10 = 10 * c;
		const int32_t i = FreeSlot();
		if (i < 0) return;
		PoolE *p = Entry(i);
		p->type = 2;
		p->life = 0;
		p->scale = (int16_t)(CrtRand() % 0x380 + n6 + 0x100);
		int32_t *d = (int32_t *)&w->m2;
		d[0] = CrtRand() % 0x2000 - 0x1000;
		d[1] = CrtRand() % 0x2000 - 0x1000;
		d[2] = CrtRand() % 0x2000 - 0x1000;
		Normalize(d, d);
		const int32_t dist = CrtRand() % 0x28 + n10 + 0x14;
		*(uint32_t *)&p->a[0] = var<uint32_t>(game_addr(POINT));
		*(uint32_t *)&p->a[2] = var<uint32_t>(game_addr(POINT) + 4);
		p->a[0] = (int16_t)(p->a[0] + (int16_t)(mul32(d[0], dist) >> 12));
		p->a[1] = (int16_t)(p->a[1] + (int16_t)(mul32(d[1], dist) >> 12));
		p->a[2] = (int16_t)(p->a[2] + (int16_t)(mul32(d[2], dist) >> 12));
	}

	static void SpriteFrame(ModelWork *w)
	{
		w->m1 = Camera();
		GteSetLightMatrix(&w->m1);
		GteSetBackColorFromTrans(&w->m1);
	}

	static uint32_t __cdecl SparkBurstTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x48);
		if (!DrawOnly() && n->c <= 0x13)
		{
			const int32_t c = n->c;
			const int32_t many = c / 12 + 1;
			for (int32_t k = 0; k < many; k++) SparkBurstSpawn(w, c);
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint16_t *)(h + 0x24) = 8;
		SpriteFrame(w);
		*(uint32_t *)h = game_addr(SEQ_SPARKBURST);
		int32_t count = 0;
		for (uint32_t a = game_addr(POOL); a < game_addr(POOL_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (!(p->type & 2)) continue;
			SpriteDraw(w, h, p, p->life, &p->a[0], -(p->scale >> 3));
			if (DrawOnly()) continue;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				p->type = 0;
				continue;
			}
			p->life++;
			count++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		n->c++;
		return EndIfNone(count);
	}

	static void IdentityRotation(Mat4x3 *m)
	{
		uint32_t *d = (uint32_t *)m;
		d[3] = 0;
		d[2] = 0;
		d[1] = 0;
		d[0] = 0;
		m->m[2][2] = 0x1000;
		m->m[1][1] = 0x1000;
		m->m[0][0] = 0x1000;
	}

	static void WaveTrailDraw(ModelWork *w, uint8_t *h, const WaveRec *rec, const PoolE *p, bool faded, int32_t fade, int16_t len)
	{
		IdentityRotation(&w->m2);
		const int16_t *pt = rec->pt[p->a[3]];
		w->m2.t[1] = 0;
		w->m2.t[2] = pt[2];
		w->v[0] = p->a[1];
		w->m2.t[0] = pt[0];
		w->v[2] = len;
		w->v[1] = len;
		Scale3DMatrix(&w->m2, w->v);
		ComposeAffineTransform(&w->m1, &w->m2, &w->m2);
		GteSetRotMatrix(&w->m2);
		GteSetTransVector(&w->m2);
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (faded)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		FrameArena() = RenderPrimModel(h, RenderOT(), 2, FrameArena());
	}

	static uint32_t __cdecl WaveTrailTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		const WaveRec *rec = Wave(n->e);
		const uint32_t type = (uint32_t)(n->e + 0x20);
		ModelWork *w = (ModelWork *)FieldAlloc(0x68);
		if (!DrawOnly() && n->c <= 8 && !(n->c & 1))
		{
			for (int32_t k = 0; k < 4; k++)
			{
				const int32_t i = FreeSlot();
				if (i < 0) break;
				PoolE *p = Entry(i);
				p->type = type;
				p->life = 0;
				const int32_t len = (CrtRand() % 0x300 + shl32(n->c, 8) + 0x600) / 3;
				p->a[4] = (int16_t)len;
				p->a[0] = (int16_t)len;
				p->a[1] = 0x600;
				p->a[3] = n->w20;
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		ff8fx::copy<(sizeof(Mat4x3))>(&w->m1, &rec->mat);
		*(uint32_t *)h = game_addr(MODEL_WAVETRAIL);
		*(uint32_t *)(h + 8) = 0;
		int32_t count = 0;
		w->m1.t[0] = 0;
		w->m1.t[1] = 0;
		w->m1.t[2] = 0;
		ComposeAffineTransform(&Camera(), &w->m1, &w->m1);
		for (uint32_t a = game_addr(POOL); a < game_addr(POOL_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (p->type != type) continue;
			WaveTrailDraw(w, h, rec, p, p->life >= 0, mul32(p->life, 682), p->a[0]);
			if (DrawOnly()) continue;
			if (p->life >= 6)
			{
				p->type = 0;
				continue;
			}
			p->life++;
			const int16_t r = p->a[4];
			p->a[0] = (int16_t)(p->a[0] + r);
			p->a[4] = (int16_t)(r - r / 4);
			count++;
		}
		FieldFree(0x58);
		FieldFree(0x68);
		if (DrawOnly()) return 0;
		const int16_t step = n->w22;
		n->w20 = (int16_t)(n->w20 + step);
		if (n->c > 4 && step < 6) n->w22 = (int16_t)(step + 1);
		n->c++;
		return EndIfNone(count);
	}

	static void TargetSparkSpawn(const Node24 *n, uint32_t type, int32_t bones)
	{
		const int32_t i = FreeSlot();
		if (i < 0) return;
		PoolE *p = Entry(i);
		p->type = type;
		p->life = 0;
		p->scale = (int16_t)(CrtRand() % 0xF00 + 0x300);
		const int32_t bone = bones ? CrtRand() % bones : 0;
		const int32_t a = RandMask(0xFFF);
		GetEffectSpawnPosition(Entity(n->w1A), bone, a, &p->a[0]);
		p->a[0] = (int16_t)(p->a[0] + (int16_t)(CrtRand() % 800 - 400));
		p->a[1] = (int16_t)(p->a[1] + (int16_t)(CrtRand() % 800 - 400));
		p->a[2] = (int16_t)(p->a[2] + (int16_t)(CrtRand() % 800 - 400));
	}

	static int32_t BoneCount(int32_t slot) { return **(const uint8_t **)*(uint8_t **)(Entity(slot) + 0x64); }

	static uint32_t __cdecl TargetSparksTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		const uint32_t type = (uint32_t)(n->e + 0x40);
		ModelWork *w = (ModelWork *)FieldAlloc(0x48);
		if (!DrawOnly() && n->c <= 7)
		{
			const int32_t bones = BoneCount(n->w1A);
			for (int32_t k = 0; k < 2; k++) TargetSparkSpawn(n, type, bones);
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint16_t *)(h + 0x24) = 8;
		SpriteFrame(w);
		*(uint32_t *)h = game_addr(SEQ_TSPARK);
		int32_t count = 0;
		const int32_t pull = -n->w18;
		for (uint32_t a = game_addr(POOL); a < game_addr(POOL_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (p->type != type) continue;
			SpriteDraw(w, h, p, p->life, &p->a[0], pull);
			if (DrawOnly()) continue;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				p->type = 0;
				continue;
			}
			p->life++;
			count++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		n->c++;
		return EndIfNone(count);
	}

	static void TargetBurstSpawn(const Node24 *n, ModelWork *w, uint32_t type, int32_t bones)
	{
		const int32_t i = FreeSlot();
		if (i < 0) return;
		PoolE *p = Entry(i);
		p->type = type;
		p->life = 0;
		p->scale = (int16_t)(RandMask(0x3FF) + 0x300);
		const int32_t bone = bones ? CrtRand() % bones : 0;
		const int32_t a = RandMask(0xFFF);
		GetEffectSpawnPosition(Entity(n->w1A), bone, a, &p->a[0]);
		int32_t *d = (int32_t *)&w->m2;
		d[0] = (int32_t)p->a[0] - w->ang[0];
		d[1] = (int32_t)p->a[1] - w->ang[1];
		d[2] = (int32_t)p->a[2] - w->ang[2];
		Normalize(d, d);
		const int32_t speed = CrtRand() % 0x118 + 0xDC;
		p->a[4] = (int16_t)(mul32(d[0], speed) >> 12);
		p->a[5] = (int16_t)(mul32(d[1], speed) >> 12);
		p->a[6] = (int16_t)(mul32(d[2], speed) >> 12);
	}

	static void TargetBurstUpdate(PoolE *p)
	{
		p->life++;
		p->a[0] = (int16_t)(p->a[0] + p->a[4]);
		p->a[1] = (int16_t)(p->a[1] + p->a[5]);
		p->a[2] = (int16_t)(p->a[2] + p->a[6]);
		for (int k = 4; k < 7; k++)
		{
			const int16_t v = p->a[k];
			p->a[k] = (int16_t)(v - (int16_t)(v >> 2));
		}
	}

	static uint32_t __cdecl TargetBurstTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		const uint32_t type = (uint32_t)(n->e + 0x80);
		ModelWork *w = (ModelWork *)FieldAlloc(0x48);
		if (!DrawOnly() && n->c <= 6)
		{
			const int32_t bones = BoneCount(n->w1A);
			ff8fx::copy<(8)>(w->ang, &n->w10);
			for (int32_t k = 0; k < 3; k++) TargetBurstSpawn(n, w, type, bones);
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint16_t *)(h + 0x24) = 0;
		int32_t count = 0;
		SpriteFrame(w);
		*(uint32_t *)h = game_addr(SEQ_TBURST);
		const int32_t pull = -n->w18;
		for (uint32_t a = game_addr(POOL); a < game_addr(POOL_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (p->type != type) continue;
			SpriteDraw(w, h, p, p->life, &p->a[0], pull);
			if (DrawOnly()) continue;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				p->type = 0;
				continue;
			}
			TargetBurstUpdate(p);
			count++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		n->c++;
		return EndIfNone(count);
	}

	static Node24 *AddMain(uint32_t fn) { return (Node24 *)AddTaskToQueue(QMain(), fn); }

	static void AddCleared(uint32_t fn)
	{
		Node24 *t = AddMain(fn);
		t->c = 0;
		t->e = 0;
	}

	static void AddTrail()
	{
		Node24 *t = AddMain(game_addr(ORIG_Trail));
		t->c = 0;
		for (uint32_t a = game_addr(RING); a < game_addr(RING_END); a += 0x14) *(uint32_t *)a = 0;
	}

	static void AddCasterRing()
	{
		Node24 *t = AddMain(game_addr(ORIG_CasterRing));
		t->c = 0;
		GetDefaultEffectPosition(Caster(), &t->w10);
		t->w12 = 0;
		t->w18 = RandMask(0x7FF);
		t->w1C = 0xE00;
		t->w20 = 0x1B00;
	}

	static void AddTipFlash()
	{
		SlashPoint();
		Node24 *t = AddMain(game_addr(ORIG_TipFlash));
		*(uint32_t *)&t->w10 = var<uint32_t>(game_addr(POINT));
		t->c = 0;
		*(uint32_t *)&t->w14 = var<uint32_t>(game_addr(POINT) + 4);
		t->w18 = RandMask(0x3FF);
		t->w1A = 0x28;
		t->w1E = 0x180;
		t->w1C = 0x180;
	}

	static void AddSlashes()
	{
		for (int16_t k = 0; k < 2; k++)
		{
			Node24 *t = AddMain(game_addr(ORIG_Slash));
			t->c = 0;
			t->e = k;
			t->w18 = RandMask(0x7FF);
			int16_t r = (int16_t)(CrtRand() % 0x3C + 0x28);
			t->w1A = r;
			if (r & 1) t->w1A = (int16_t)-r;
			t->w1E = 0x38;
			t->w1C = 0x38;
			t->w20 = RandMask(0xFFF);
			r = (int16_t)(CrtRand() % 0x46 + 0x1E);
			t->w22 = r;
			if (r & 1) t->w22 = (int16_t)-r;
		}
	}

	static void AddFrameModel()
	{
		Node24 *t = AddMain(game_addr(ORIG_FrameModel));
		t->c = 0;
		t->w10 = -310;
		t->w12 = -200;
		t->w14 = -350;
		t->w1E = 0x500;
		t->w1C = 0x500;
	}

	static void AddShockWaves()
	{
		var<int16_t>(game_addr(POINT)) = -400;
		var<int16_t>(game_addr(POINT) + 2) = -500;
		var<int16_t>(game_addr(POINT) + 4) = -500;
		MatVec((const Mat4x3 *)game_addr(FRAME), (const int16_t *)game_addr(POINT), (int16_t *)game_addr(POINT));
		const Mat4x3 *fr = (const Mat4x3 *)game_addr(FRAME);
		var<int16_t>(game_addr(POINT)) = (int16_t)(var<int16_t>(game_addr(POINT)) + (int16_t)fr->t[0]);
		var<int16_t>(game_addr(POINT) + 2) = (int16_t)(var<int16_t>(game_addr(POINT) + 2) + (int16_t)fr->t[1]);
		var<int16_t>(game_addr(POINT) + 4) = (int16_t)(var<int16_t>(game_addr(POINT) + 4) + (int16_t)fr->t[2]);
		ActionData *ad = Context()->actions;
		if (ad->target_count == 0) return;
		int32_t off = 0;
		for (int32_t i = 0;;)
		{
			WaveRec *rec = Wave(i);
			int16_t pos[4];
			GetDefaultEffectPosition(Entity(ad->targets[off]), pos);
			int32_t v[3];
			v[0] = (int32_t)pos[0] - var<int16_t>(game_addr(POINT));
			v[1] = 0;
			v[2] = (int32_t)pos[2] - var<int16_t>(game_addr(POINT) + 4);
			Normalize(v, v);
			const uint32_t p0 = var<uint32_t>(game_addr(POINT)), p1 = var<uint32_t>(game_addr(POINT) + 4);
			*(uint32_t *)&rec->p0[0] = p0;
			*(uint32_t *)&rec->p0[2] = p1;
			*(uint32_t *)&rec->p1[0] = *(const uint32_t *)&pos[0];
			*(uint32_t *)&rec->p1[2] = *(const uint32_t *)&pos[2];
			*(uint32_t *)&rec->cp[0][0] = p0;
			*(uint32_t *)&rec->cp[0][2] = p1;
			*(uint32_t *)&rec->cp[1][0] = *(const uint32_t *)&pos[0];
			*(uint32_t *)&rec->cp[1][2] = *(const uint32_t *)&pos[2];
			rec->cp[1][1] = 0;
			int32_t d = CrtRand() % 1000 + 0x5DC;
			rec->cp[2][0] = (int16_t)((mul32(d, v[0]) >> 12) + *(const int32_t *)&pos[0]);
			rec->cp[2][2] = (int16_t)((mul32(d, v[2]) >> 12) + *(const int32_t *)&pos[2]);
			const int32_t r2 = CrtRand() % 1000;
			rec->cp[2][1] = (int16_t)(pos[1] - r2 - 0x5DC);
			rec->cp[3][0] = (int16_t)((int16_t)(mul32(d, v[0]) >> 12) + rec->cp[2][0]);
			rec->cp[3][2] = (int16_t)((int16_t)(mul32(d, v[2]) >> 12) + rec->cp[2][2]);
			const int32_t r3 = CrtRand() % 1000;
			rec->cp[3][1] = (int16_t)(rec->cp[2][1] - r3 - 0x5DC);
			void *buf = FieldAlloc(0x190);
			EffSplineSetup(4, rec->cp, buf);
			for (int32_t t = 0, k = 0; t < 0x40000; t += 0x1000, k++) EffSplineEval(4, buf, rec->pt[k], t / 63);
			FieldFree(0x190);
			int32_t axis[4];
			const int32_t angle = RotationBetweenVectors((const int32_t *)game_addr(REF_DIR), v, axis);
			BuildAxisAngleRotationMatrix(angle, &rec->mat, axis);
			for (int16_t k = 0; k < 4; k++)
			{
				Node24 *t = AddMain(game_addr(ORIG_WaveRing));
				t->c = 0;
				t->e = (int16_t)i;
				t->w16 = k;
				t->w14 = k;
				t->w18 = RandMask(0x7FF);
				const int32_t rr = CrtRand() % 0x3C;
				t->w1C = 0x800;
				t->w20 = 1;
				t->w22 = 2;
				t->w1A = (int16_t)(rr + 0xB4);
			}
			Node24 *t = AddMain(game_addr(ORIG_WaveTrail));
			t->c = 0;
			t->e = (int16_t)i;
			t->w20 = 1;
			t->w22 = 2;
			i++;
			off += TARGET_STRIDE;
			ad = Context()->actions;
			if (i >= ad->target_count) break;
		}
	}

	static void AddImpactFlashes()
	{
		ActionData *ad = Context()->actions;
		if (ad->target_count == 0) return;
		int32_t off = 0;
		for (int32_t i = 0;;)
		{
			const int32_t slot = ad->targets[off];
			Node24 *t = AddMain(game_addr(ORIG_ImpactFlash));
			t->c = 0;
			GetDefaultEffectPosition(Entity(slot), &t->w10);
			t->w18 = RandMask(0x7FF);
			t->w1A = (int16_t)(RandMask(0x3FF) - 0x200);
			off += TARGET_STRIDE;
			const int16_t s = (int16_t)((CrtRand() % 0xC00 + 0x1200) / 3);
			t->w1E = s;
			t->w1C = s;
			i++;
			ad = Context()->actions;
			if (i >= ad->target_count) break;
		}
	}

	static void AddTargetBursts()
	{
		ActionData *ad = Context()->actions;
		if (ad->target_count == 0) return;
		int32_t off = 0;
		for (int32_t i = 0;;)
		{
			const int32_t slot = ad->targets[off];
			const int16_t spread = (int16_t)(mul32(EntitySize(slot), 2000) >> 12);
			Node24 *s = AddMain(game_addr(ORIG_TargetSparks));
			s->c = 0;
			s->e = (int16_t)i;
			s->w18 = spread;
			s->w1A = (int16_t)slot;
			Node24 *b = AddMain(game_addr(ORIG_TargetBurst));
			b->c = 0;
			GetDefaultEffectPosition(Entity(slot), &b->w10);
			b->e = (int16_t)i;
			b->w18 = spread;
			b->w1A = (int16_t)slot;
			Node24 *r = AddMain(game_addr(ORIG_TargetRing));
			*(uint32_t *)&r->w10 = *(const uint32_t *)&b->w10;
			r->c = 0;
			*(uint32_t *)&r->w14 = *(const uint32_t *)&b->w14;
			r->w18 = RandMask(0x7FF);
			r->w1A = (int16_t)(CrtRand() % 0x78 + 0x32);
			const int16_t sc = (int16_t)((CrtRand() % 0xA00 + 0x600) / 4);
			r->w1E = sc;
			r->w1C = sc;
			r->w20 = (int16_t)(RandMask(0x3FF) + 0x500);
			i++;
			off += TARGET_STRIDE;
			ad = Context()->actions;
			if (i >= ad->target_count) break;
		}
	}

	static void TargetsSequence5()
	{
		ActionData *ad = Context()->actions;
		if (ad->target_count == 0) return;
		int32_t off = 0;
		for (int32_t i = 0;;)
		{
			QueueChainTransformation(Entity(ad->targets[off]), 5);
			i++;
			off += TARGET_STRIDE;
			ad = Context()->actions;
			if (i >= ad->target_count) break;
		}
	}

	static void Sequence(int32_t seq) { QueueChainTransformation(Caster(), seq); }

	static uint32_t __cdecl DirectorTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)game_addr(FRAME), (Mat4x3 *)game_addr(CAM_FRAME));
		const uint32_t fl = UpdateFlags();
		if (fl & 0x201)
		{
			if (fl & 1) return 0;
			if (PauseQuery() < 0) return 0;
		}
		if (n->c == 0)
		{
			CameraScriptStart(game_addr(CAMERA_SCRIPT), game_addr(FRAME), Caster(), QMain(), 0);
			ArmCameraReturn();
		}
		const int32_t c = n->c;
		if (c < 0xA)
		{
			if (c == 6)
			{
				BdPlaySE(SOUND_A(), 0, 0x80);
				TargetsSequence5();
			}
			else if (c == 3)
			{
				BindEntityModelFromComData(Caster(), game_addr(BIND_OUT), game_addr(ANIM_DATA));
				Sequence(4);
			}
			else if (c == 4) AddFlameDirector();
		}
		else if (c < 0x15)
		{
			if (c == 0xA) Sequence(5);
		}
		else if (c < 0x40)
		{
			switch (c)
			{
			case 0x23: BdPlaySE(SOUND_B(), 0, 0x80); break;
			case 0x15: Sequence(1); AddTrail(); break;
			case 0x1F: CharacterLoad(0x232, TexBase() + 0x1C000); break;
			case 0x2D:
				if (PauseQuery() < 0) return 0;
				QueueTIMUpload(TexBase() + 0x1C000);
				break;
			case 0x2E: CharacterLoad(0x233, TexBase() + 0x1C000); break;
			case 0x3D:
				if (PauseQuery() < 0) return 0;
				QueueTIMUpload(TexBase() + 0x1C000);
				break;
			}
		}
		else if (c < 0x72)
		{
			switch (c)
			{
			case 0x40: Sequence(2); break;
			case 0x41: AddCasterRing(); break;
			case 0x48: AddTipFlash(); break;
			case 0x50: AddCleared(game_addr(ORIG_TipShards)); break;
			case 0x54: AddSlashes(); break;
			case 0x5A: AddCleared(game_addr(ORIG_SparkBurst)); break;
			}
		}
		else if (c < 0x8B)
		{
			switch (c)
			{
			case 0x72: Sequence(3); CharacterLoad(0x234, TexBase() + 0x1C000); break;
			case 0x7A:
				if (PauseQuery() < 0) return 0;
				QueueTIMUpload(TexBase() + 0x1C000);
				break;
			case 0x77: ScreenFadeTask(0, 1, 6, 0xFF); AddFrameModel(); break;
			case 0x78: AddShockWaves(); break;
			case 0x80: AddImpactFlashes(); break;
			}
		}
		else if (c < 0xAE)
		{
			if (c == 0x8B)
			{
				Sequence(6);
				AddTargetBursts();
			}
			else if (c == 0x8D)
			{
				ActionData *ad = Context()->actions;
				ApplyActionResultToTargets(ad->targets, ad->target_count);
			}
		}
		if (n->c <= 8) SetScreenFlash(shl32(n->c, 8), 0);
		else if (n->c >= 0xA6) SetScreenFlash(shl32(0xAE - n->c, 8), 0);
		n->c++;
		if (n->c <= 0xAE) return 0;
		SetScreenFlash(0, 0);
		return TASK_END;
	}

	static uint32_t __cdecl RootTask(TaskNode *tn)
	{
		RootNode *r = (RootNode *)tn;
		if (r->c & 1) PacketCursor() = TexBase() + 0xE000;
		else PacketCursor() = TexBase();
		const int alive = ExecuteTaskQueue(QMain());
		if (!alive)
		{
			RootNode *t = (RootNode *)AddTaskToQueue(QRoot(), game_addr(ORIG_Restore));
			t->c = 0;
			t->e = 0;
		}
		r->c++;
		return alive ? 0 : TASK_END;
	}

	static uint32_t __cdecl RestoreTask(TaskNode *tn)
	{
		RootNode *r = (RootNode *)tn;
		if (r->c == 1) TextureRestoreTask((void *)(TexBase() + 0x1C000), &r->e);
		const int16_t done = r->e;
		r->c++;
		return done ? TASK_END : 0;
	}
}

	void register_mag200_no_mercy()
	{
		using namespace mercy200;
		register_port(ORIG_Root, (void *)RootTask, "200 Root", 200);
		register_port(ORIG_Director, (void *)DirectorTask, "200 Director", 200);
		register_port(ORIG_FlameMulti, (void *)FlameMultiTask, "200 FlameDirectorMulti", 200);
		register_port(ORIG_FlameSingle, (void *)FlameSingleTask, "200 FlameDirectorSingle", 200);
		register_port(ORIG_Flame, (void *)FlameTask, "200 Flame", 200);
		register_port(ORIG_Spark, (void *)SparkTask, "200 Spark", 200);
		register_port(ORIG_Ember, (void *)EmberTask, "200 Ember", 200);
		register_port(ORIG_Trail, (void *)TrailTask, "200 Trail", 200);
		register_port(ORIG_CasterRing, (void *)CasterRingTask, "200 CasterRing", 200);
		register_port(ORIG_TipFlash, (void *)TipFlashTask, "200 TipFlash", 200);
		register_port(ORIG_TipShards, (void *)TipShardsTask, "200 TipShards", 200);
		register_port(ORIG_Slash, (void *)SlashTask, "200 Slash", 200);
		register_port(ORIG_SparkBurst, (void *)SparkBurstTask, "200 SparkBurst", 200);
		register_port(ORIG_FrameModel, (void *)FrameModelTask, "200 FrameModel", 200);
		register_port(ORIG_WaveRing, (void *)WaveRingTask, "200 WaveRing", 200);
		register_port(ORIG_WaveTrail, (void *)WaveTrailTask, "200 WaveTrail", 200);
		register_port(ORIG_ImpactFlash, (void *)ImpactFlashTask, "200 ImpactFlash", 200);
		register_port(ORIG_TargetSparks, (void *)TargetSparksTask, "200 TargetSparks", 200);
		register_port(ORIG_TargetBurst, (void *)TargetBurstTask, "200 TargetBurst", 200);
		register_port(ORIG_TargetRing, (void *)TargetRingTask, "200 TargetRing", 200);
		register_port(ORIG_Restore, (void *)RestoreTask, "200 Restore", 200);
	}
}

