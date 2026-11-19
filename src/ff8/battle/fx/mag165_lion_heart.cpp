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

// Effect 165: Lion Heart

#include "mag_common.h"

namespace ff8fx
{
namespace lion165
{
	using namespace eng;
	using namespace magc;

	inline int32_t &TargetSlot() { return var<int32_t>(game_addr(game::MAG_165_LION_HEART_TargetSlot)); }
	inline int32_t &CasterSlot() { return var<int32_t>(game_addr(game::MAG_165_LION_HEART_CasterSlot)); }
	inline CastContext *&Context() { return var<CastContext *>(game_addr(game::MAG_165_LION_HEART_CastContext)); }
	inline uint32_t &TexBase() { return var<uint32_t>(game_addr(game::MAG_165_LION_HEART_TextureBase)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_165_LION_HEART_PacketCursor)); }
	inline uint32_t &PacketArena() { return var<uint32_t>(game_addr(game::MAG_165_LION_HEART_PacketArena)); }
	inline uint32_t &PoolCBase() { return var<uint32_t>(game_addr(game::MAG_165_LION_HEART_PoolCBase)); }
	inline int32_t &NextId() { return var<int32_t>(game_addr(game::MAG_165_LION_HEART_NextNodeId)); }
	inline int32_t &ResultIndex() { return var<int32_t>(game_addr(game::MAG_165_LION_HEART_ResultIndex)); }
	inline uint32_t &FrameArena() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline TaskQueue *QRoot() { return (TaskQueue *)game_addr(game::MAG_165_LION_HEART_RootQueue); }
	inline TaskQueue *QMain() { return (TaskQueue *)game_addr(game::MAG_165_LION_HEART_MainQueue); }
	static constexpr game::Id POOL_A = game::MAG_165_LION_HEART_PoolA, POOL_A_END = game::MAG_165_LION_HEART_PoolB;
	static constexpr game::Id POOL_B = game::MAG_165_LION_HEART_PoolB, POOL_B_END = game::MAG_165_LION_HEART_PoolCBase;
	static constexpr game::Id FINAL_SHARDS_END = game::unk_23C6080;
	static constexpr game::Id PTS_BASE = game::MAG_165_LION_HEART_TrailPointsBase, PTS_TIP = game::MAG_165_LION_HEART_TrailPointsTip;
	static constexpr game::Id SPL_BASE = game::MAG_165_LION_HEART_TrailSplineBase, SPL_TIP = game::MAG_165_LION_HEART_TrailSplineTip;
	static constexpr game::Id TIP0 = game::MAG_165_LION_HEART_TrailTip0, TRAIL_DIR = game::MAG_165_LION_HEART_TrailDir, BASE0 = game::MAG_165_LION_HEART_TrailBase0, BLADE_DIR = game::MAG_165_LION_HEART_BladeDir;
	static constexpr game::Id RING = game::MAG_165_LION_HEART_TrailRing;
	static constexpr game::Id FRAME = game::MAG_165_LION_HEART_EffectFrame;
	static constexpr game::Id CAM_FRAME = game::MAG_165_LION_HEART_CameraFrame;
	static constexpr game::Id BIND_OUT = game::MAG_165_LION_HEART_BindOut;
	static constexpr game::Id HITS = game::MAG_165_LION_HEART_HitRecords;
	static constexpr game::Id PREV_HIT = game::MAG_165_LION_HEART_PrevHitPoint, LAST_HIT = game::MAG_165_LION_HEART_LastHitRecord;
	static constexpr game::Id KEEP = game::MAG_165_LION_HEART_EntityKeep;
	static constexpr game::Id TIP = game::MAG_165_LION_HEART_BladeTip;
	static constexpr game::Id BONE_F1 = game::MAG_165_LION_HEART_BoneF1Pos;
	static constexpr game::Id TARGET_KEPT = game::MAG_165_LION_HEART_TargetKept, CASTER_KEPT = game::MAG_165_LION_HEART_CasterKept;
	static constexpr game::Id TARGET_ANCHOR = game::MAG_165_LION_HEART_TargetAnchor, TARGET_CENTRE = game::MAG_165_LION_HEART_TargetCentre;
	static constexpr game::Id CASTER_POS = game::MAG_165_LION_HEART_CasterPosF3, TARGET_POS = game::MAG_165_LION_HEART_TargetPosF3;
	static constexpr game::Id CAM_PTR = game::MAG_165_LION_HEART_ClipCameraPtr;

	static constexpr game::Id ORIG_Root = game::MAG_165_LION_HEART_RootTask, ORIG_Director = game::MAG_165_LION_HEART_DirectorTask, ORIG_Flash = game::MAG_165_LION_HEART_FlashTask, ORIG_Trail = game::MAG_165_LION_HEART_TrailTask, ORIG_GroundRing = game::MAG_165_LION_HEART_GroundRingTask, ORIG_RockBurst = game::MAG_165_LION_HEART_RockBurstTask, ORIG_Debris = game::MAG_165_LION_HEART_DebrisTask, ORIG_BoneSparks = game::MAG_165_LION_HEART_BoneSparksTask, ORIG_BladeSparks = game::MAG_165_LION_HEART_BladeSparksTask, ORIG_TipFlash = game::MAG_165_LION_HEART_TipFlashTask, ORIG_TipShards = game::MAG_165_LION_HEART_TipShardsTask, ORIG_TipSmoke = game::MAG_165_LION_HEART_TipSmokeTask, ORIG_TipRing = game::MAG_165_LION_HEART_TipRingTask, ORIG_HitDirector = game::MAG_165_LION_HEART_HitDirectorTask, ORIG_HitSparkA = game::MAG_165_LION_HEART_HitSparkATask, ORIG_HitSparkB = game::MAG_165_LION_HEART_HitSparkBTask, ORIG_HitSlashA = game::MAG_165_LION_HEART_HitSlashATask, ORIG_HitSlashB = game::MAG_165_LION_HEART_HitSlashBTask, ORIG_HitShards = game::MAG_165_LION_HEART_HitShardsTask, ORIG_TargetBounce = game::MAG_165_LION_HEART_TargetBounceTask, ORIG_FlatRing = game::MAG_165_LION_HEART_FlatRingTask, ORIG_WaveSpawner = game::MAG_165_LION_HEART_WaveSpawnerTask, ORIG_ScreenWave = game::MAG_165_LION_HEART_ScreenWaveTask, ORIG_TipFlash2 = game::MAG_165_LION_HEART_TipFlash2Task, ORIG_TipRocks = game::MAG_165_LION_HEART_TipRocksTask, ORIG_Streak = game::MAG_165_LION_HEART_StreakTask, ORIG_FinalRing = game::MAG_165_LION_HEART_FinalRingTask, ORIG_FinalSparkA = game::MAG_165_LION_HEART_FinalSparkATask, ORIG_FinalShards = game::MAG_165_LION_HEART_FinalShardsTask, ORIG_FinalRingSparks = game::MAG_165_LION_HEART_FinalRingSparksTask, ORIG_TargetSink = game::MAG_165_LION_HEART_TargetSinkTask, ORIG_TargetSparks = game::MAG_165_LION_HEART_TargetSparksTask, ORIG_TargetRing = game::MAG_165_LION_HEART_TargetRingTask, ORIG_Shake = game::MAG_165_LION_HEART_ShakeTask, ORIG_Restore = game::MAG_165_LION_HEART_RestoreTextureTask; static constexpr game::Id ORIG_FinalFlash = game::MAG_165_LION_HEART_FinalFlashTask;

	static constexpr game::Id ANIM_DATA = game::MAG_165_LION_HEART_AnimSet, CAMERA_SCRIPT = game::MAG_165_LION_HEART_CameraScript;
	static const void *SOUND_A() { return (const void *)game_addr(game::MAG_165_LION_HEART_Sounds); }
	static const void *SOUND_B() { return (const void *)game_addr(game::unk_D886D8); }
	static const void *SOUND_C() { return (const void *)game_addr(game::unk_D886DC); }
	static const void *SOUND_D() { return (const void *)game_addr(game::unk_D886E0); }
	static constexpr game::Id REF_BACK = game::MAG_165_LION_HEART_RefBack;
	static constexpr game::Id REF_RIGHT = game::MAG_165_LION_HEART_RefRight;
	static constexpr game::Id REF_UP = game::MAG_165_LION_HEART_RefUp;
	static constexpr game::Id HIT_TIMES = game::MAG_165_LION_HEART_HitTicks;
	static constexpr game::Id HIT_OFFSETS = game::MAG_165_LION_HEART_HitOffsets;
	static constexpr game::Id HIT_HEIGHTS = game::MAG_165_LION_HEART_BounceHeights;
	static constexpr game::Id WAVE_COLOURS = game::MAG_165_LION_HEART_WaveColours;
	static constexpr game::Id CLIP_SLOTS = game::MAG_165_LION_HEART_ClipSlotTable;
	static constexpr game::Id SEQ_SPARK = game::MAG_165_LION_HEART_SeqBoneSpark, SEQ_SMOKE = game::MAG_165_LION_HEART_SeqTipSmoke, SEQ_HITA = game::MAG_165_LION_HEART_SeqHitSpark, SEQ_HITB = game::MAG_165_LION_HEART_SeqSparkB;
	static constexpr game::Id MODEL_GROUNDRING = game::MAG_165_LION_HEART_ModelGroundRing, MODEL_ROCK = game::MAG_165_LION_HEART_ModelRock, MODEL_DEBRIS = game::MAG_165_LION_HEART_ModelDebris, MODEL_TIPFLASH = game::MAG_165_LION_HEART_ModelTipFlash, MODEL_TIPSHARD = game::MAG_165_LION_HEART_ModelTipShard, MODEL_TIPRING = game::MAG_165_LION_HEART_ModelTipRing, MODEL_SLASHA = game::MAG_165_LION_HEART_ModelHitSlashA, MODEL_SLASHB = game::MAG_165_LION_HEART_ModelHitSlashB, MODEL_SHARD = game::MAG_165_LION_HEART_ModelShard, MODEL_FLATRING = game::MAG_165_LION_HEART_ModelFlatRing, MODEL_WAVE = game::MAG_165_LION_HEART_ModelScreenWave, MODEL_TIPFLASH2 = game::MAG_165_LION_HEART_ModelTipFlash2, MODEL_TIPROCK = game::MAG_165_LION_HEART_ModelTipRock, MODEL_STREAK = game::MAG_165_LION_HEART_ModelStreak, MODEL_FINALRING = game::MAG_165_LION_HEART_ModelFinalRing, MODEL_FINALFLASH = game::MAG_165_LION_HEART_ModelFinalFlash;
	static const float SPLINE_THREE = 3.0f, SPLINE_SIX = 6.0f;

	inline int32_t PauseQuery() { return fn<int32_t (__cdecl *)()>(game_addr(game::BattleFile_GetLoadingState))(); }
	inline void StopSounds(int32_t a, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::sub_46B450))(a, b); }
	inline void CameraScriptStart(uint32_t script, uint32_t frame, void *entity, TaskQueue *q, int32_t n) { fn<void (__cdecl *)(uint32_t, uint32_t, void *, TaskQueue *, int32_t)>(game_addr(game::MAG066_CamScript_Start))(script, frame, entity, q, n); }
	inline void ArmCameraReturn() { fn<void (__cdecl *)()>(game_addr(game::BS_Camera_ArmReturnAfterEffect))(); }
	inline void BindEntityModelFromComData(uint8_t *entity, uint32_t out, uint32_t data) { fn<void (__cdecl *)(uint8_t *, uint32_t, uint32_t)>(game_addr(game::BattleEntity_BindExternalAnimSet))(entity, out, data); }
	inline void QueueChainTransformation(uint8_t *entity, int32_t seq) { fn<void (__cdecl *)(uint8_t *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, seq); }
	inline void CharacterLoad(int32_t id, uint32_t dst) { fn<void (__cdecl *)(int32_t, uint32_t)>(game_addr(game::BattleFile_CharacterLoad))(id, dst); }
	inline void QueueTIMUpload(uint32_t tim) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::Battle_QueueTIMUpload_GetEOF))(tim); }
	inline void GetEffectSpawnPosition(uint8_t *entity, int32_t bone, int32_t a, void *out) { fn<void (__cdecl *)(uint8_t *, int32_t, int32_t, void *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, a, out); }
	inline int32_t Normalize(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
	inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
	inline void MatVec(const Mat4x3 *m, const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
	inline void GteMatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
	inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColorRGB))(r, g, b); }
	inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
	inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
	inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
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
	inline void GteMVMVA_LightV1Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV1_Bk))(); }
	inline void GteMVMVA_LightV2Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV2_Bk))(); }
	inline void GteMVMVA_RotV0Tr2() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0_Tr_2))(); }
	inline void GteSetBackColorFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
	inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
	inline void GteSetTransFromVec32(const int32_t *v) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_SetTransFromVec32))(v); }
	inline void GteTransFromMAC() { fn<void (__cdecl *)()>(game_addr(game::GTE_TransFromMAC))(); }
	inline void GteWriteControlReg(int32_t value, int32_t reg) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::GTE_WriteControlReg))(value, reg); }
	inline void GteWriteDataReg(uint32_t value, int32_t reg) { fn<void (__cdecl *)(uint32_t, int32_t)>(game_addr(game::GTE_WriteDataReg))(value, reg); }
	inline uint32_t ScrollRender(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_066_RenderScrollPrimModel))(h, ot, mode, cursor); }
	inline uint32_t DrawShadow(uint8_t *entity, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(uint8_t *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, mode, cursor); }
	inline uint32_t OT0() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }
	inline int16_t ProjectionH() { return var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)); }

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
		int32_t id;
		int16_t life;
		int16_t scale;
		int16_t a[8];
	};
	struct PoolE2
	{
		int32_t id;
		int16_t life;
		int16_t scale;
		int16_t a[12];
	};
	struct RingEntry
	{
		uint32_t alive;
		uint32_t base[2];
		uint32_t tip[2];
	};
	struct HitRec
	{
		int16_t pos[4];
		int32_t dir[4];
		uint32_t pad18;
	};
	struct KeepBlock
	{
		uint16_t flag20;
		uint16_t pad02;
		uint32_t rot[2];
		uint32_t pos[2];
		uint8_t pad14[0x14];
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
	struct ModelWork
	{
		int16_t ang[4];
		Mat4x3 m1;
		Mat4x3 m2;
		int32_t v[4];
		int32_t w[4];
		int32_t x[4];
	};
	struct ClipHeader
	{
		uint32_t prims;
		uint32_t verts;
		uint16_t ntri;
		uint16_t nquad;
		uint16_t w0C, w0E;
		uint32_t code;
		uint32_t mask;
		uint8_t pad18[0x14];
		uint32_t clip;
		uint32_t col;
		uint32_t uv0;
		uint32_t uv1;
		uint32_t ot;
		uint32_t shift;
		uint32_t cursor;
		int32_t mac0;
		uint32_t pad4C;
		int32_t otz;
		uint32_t flag;
		uint8_t slot[6][0x10];
		int16_t plane[4];
		uint8_t padC0[8];
		uint32_t ptr[7][8];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(Node24) == 0x24 && sizeof(PoolE) == 0x18 && sizeof(PoolE2) == 0x20, "Lion Heart nodes");
	static_assert(sizeof(RingEntry) == 0x14 && sizeof(HitRec) == 28 && sizeof(KeepBlock) == 0x28, "Lion Heart records");
	static_assert(sizeof(TrailWork) == 0x40 && sizeof(ModelWork) == 0x78 && sizeof(ClipHeader) == 0x1A8, "Lion Heart blocks");

	template<typename T> static inline T &At(uint32_t base, int32_t off) { return *(T *)(base + off); }
	inline uint8_t *Caster() { return Entity(CasterSlot()); }
	inline uint8_t *Target() { return Entity(TargetSlot()); }
	inline PoolE *EntryA(int32_t i) { return (PoolE *)(game_addr(POOL_A) + 0x18 * i); }
	inline PoolE2 *EntryB(int32_t i) { return (PoolE2 *)(game_addr(POOL_B) + 0x20 * i); }
	inline HitRec *Hit(int32_t i) { return (HitRec *)(game_addr(HITS) + 28 * i); }
	inline KeepBlock *Keep(int32_t slot) { return (KeepBlock *)(game_addr(KEEP) + 0x28 * slot); }

	static int32_t FreeSlot(uint32_t pool, uint32_t end, uint32_t stride, int32_t max)
	{
		int32_t i = 0;
		for (uint32_t a = pool; *(const int32_t *)a != 0; i++)
		{
			a += stride;
			if (a >= end) return -1;
		}
		return i >= max ? -1 : i;
	}
	static int32_t FreeA() { return FreeSlot(game_addr(POOL_A), game_addr(POOL_A_END), 0x18, 0xC8); }
	static int32_t FreeB() { return FreeSlot(game_addr(POOL_B), game_addr(POOL_B_END), 0x20, 0x78); }

	static void ClearIdA(int32_t id)
	{
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
			if (*(const int32_t *)a == id) *(int32_t *)a = 0;
	}

	static inline int16_t TakeId()
	{
		const int32_t id = NextId();
		NextId() = id + 1;
		return (int16_t)id;
	}

	static inline uint32_t EndAt(int16_t c, int16_t limit) { return c >= limit ? TASK_END : 0; }
	static inline uint32_t EndIfNone(int32_t count) { return count != 0 ? 0 : TASK_END; }

	static int32_t EntityIndex(const uint8_t *e)
	{
		int32_t i = 0;
		for (uint32_t a = game_addr(game::BattleEntitySlotData); a != (uint32_t)e; i++)
		{
			a += 0x9C;
			if (a >= game_addr(game::CAMERA_FLAG_RELATED)) return 7;
		}
		return i;
	}
}
}

namespace ff8fx
{
namespace lion165
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

	static void SetTipPoint()
	{
		int32_t dir[4];
		WeaponBone(Caster(), 1, dir, (int16_t *)game_addr(TIP));
		var<int16_t>(game_addr(TIP)) = (int16_t)(var<int16_t>(game_addr(TIP)) + (int16_t)(mul32(dir[0], 380) >> 12));
		var<int16_t>(game_addr(TIP) + 2) = (int16_t)(var<int16_t>(game_addr(TIP) + 2) + (int16_t)(mul32(dir[1], 380) >> 12));
		var<int16_t>(game_addr(TIP) + 4) = (int16_t)(var<int16_t>(game_addr(TIP) + 4) + (int16_t)(mul32(dir[2], 380) >> 12));
	}

	static void StageGroupsHide()
	{
		for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) var<uint8_t>(a) |= 2;
	}
	static void StageGroupsShow()
	{
		for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) var<uint8_t>(a) &= 0xFD;
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

	static void ClipEdge(const int16_t *a, const int16_t *b, const int16_t *plane, int16_t *out)
	{
		const int32_t dx = (int32_t)b[0] - a[0];
		const int32_t dy = (int32_t)b[1] - a[1];
		const int32_t dz = (int32_t)b[2] - a[2];
		int32_t den = (mul32(plane[0], dx) + mul32(plane[1], dy) + mul32(plane[2], dz)) >> 12;
		if (den == 0) den = 1;
		const int32_t num = shl32(plane[3], 12) + mul32(plane[2], a[2]) + mul32(plane[1], a[1]) + mul32(plane[0], a[0]);
		const int32_t t = -(num / den);
		out[0] = (int16_t)((mul32(t, dx) >> 12) + a[0]);
		out[1] = (int16_t)((mul32(t, dy) >> 12) + a[1]);
		out[2] = (int16_t)((mul32(t, dz) >> 12) + a[2]);
		const int32_t au = (uint16_t)a[3] & 0xFF, bu = (uint16_t)b[3] & 0xFF;
		const int32_t av = (uint16_t)a[3] & 0xFF00, bv = (uint16_t)b[3] & 0xFF00;
		const int32_t u = (mul32(bu - au, t) >> 12) + au;
		const int32_t v = (mul32(bv - av, t) >> 12) + av;
		out[3] = (int16_t)((uint16_t)u | (uint16_t)(v & 0xFF00));
	}

	static void ClipTriFT3(ClipHeader *h, const int16_t *a, const int16_t *b, const int16_t *c)
	{
		const uint32_t pkt = h->cursor;
		GteLoadV012(a, b, c);
		GteRTPT();
		At<uint32_t>(pkt, 4) = h->col | 0x24000000;
		At<uint32_t>(pkt, 0xC) = h->uv0;
		At<uint32_t>(pkt, 0x14) = h->uv1;
		At<uint32_t>(pkt, 0) = 0x7000000;
		GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x10), (void *)(pkt + 0x18));
		GteAVSZ3();
		At<int16_t>(pkt, 0x14) = b[3];
		At<int16_t>(pkt, 0xC) = a[3];
		At<int16_t>(pkt, 0x1C) = c[3];
		GteStoreOTZ(&h->otz);
		InsertPrimAutoDepth(h->ot + (uint32_t)(h->otz >> h->shift) * 4, (void *)pkt);
		h->cursor = pkt + 0x20;
	}

	static void ClipQuadFT4(ClipHeader *h, const int16_t *a, const int16_t *b, const int16_t *c, const int16_t *d)
	{
		const uint32_t pkt = h->cursor;
		GteLoadV012(a, b, c);
		GteRTPT();
		At<uint32_t>(pkt, 4) = h->col | 0x2C000000;
		At<uint32_t>(pkt, 0xC) = h->uv0;
		At<uint32_t>(pkt, 0x14) = h->uv1;
		At<uint32_t>(pkt, 0) = 0x9000000;
		GteReadSXY012Split((void *)(pkt + 8), (void *)(pkt + 0x10), (void *)(pkt + 0x18));
		GteLoadV0(d);
		GteRTPS();
		At<int16_t>(pkt, 0x14) = b[3];
		At<int16_t>(pkt, 0xC) = a[3];
		At<int16_t>(pkt, 0x1C) = c[3];
		At<int16_t>(pkt, 0x24) = d[3];
		GteReadSXY2((void *)(pkt + 0x20));
		GteAVSZ4();
		GteStoreOTZ(&h->otz);
		InsertPrimAutoDepth(h->ot + (uint32_t)(h->otz >> h->shift) * 4, (void *)pkt);
		h->cursor = pkt + 0x28;
	}

	static inline int16_t *Slot(ClipHeader *h, int32_t k) { return (int16_t *)h->slot[k]; }
	static inline int32_t &SlotWord(ClipHeader *h, int32_t k, int32_t off) { return *(int32_t *)(h->slot[k] + off); }
	static inline const int16_t *SlotPtr(const ClipHeader *h, int32_t f, int32_t i) { return (const int16_t *)h->ptr[f][i]; }

	static void ClipTriangle(ClipHeader *h)
	{
		if (SlotWord(h, 3, 8)) ClipEdge(Slot(h, 0), Slot(h, 1), h->plane, Slot(h, 3));
		if (SlotWord(h, 4, 8)) ClipEdge(Slot(h, 1), Slot(h, 2), h->plane, Slot(h, 4));
		if (SlotWord(h, 5, 8)) ClipEdge(Slot(h, 2), Slot(h, 0), h->plane, Slot(h, 5));
		const uint32_t f = h->clip;
		switch (f)
		{
		case 1: case 2: case 4:
			if (*(const int32_t *)((const uint8_t *)SlotPtr(h, f, 3) + 0xC) > 0) ClipTriFT3(h, SlotPtr(h, f, 0), SlotPtr(h, f, 1), SlotPtr(h, f, 2));
			else ClipTriFT3(h, SlotPtr(h, f, 4), SlotPtr(h, f, 5), SlotPtr(h, f, 6));
			break;
		case 3: case 5: case 6:
			if (*(const int32_t *)((const uint8_t *)SlotPtr(h, f, 3) + 0xC) > 0) ClipTriFT3(h, SlotPtr(h, f, 0), SlotPtr(h, f, 1), SlotPtr(h, f, 2));
			else ClipQuadFT4(h, SlotPtr(h, f, 4), SlotPtr(h, f, 5), SlotPtr(h, f, 6), SlotPtr(h, f, 7));
			break;
		default:
			break;
		}
	}

	static void ClipPlaneTest(ClipHeader *h)
	{
		GteMVMVA_LightV0Bk();
		SlotWord(h, 3, 8) = 0;
		GteReadDataReg(9, &SlotWord(h, 3, 0xC));
		GteMVMVA_LightV1Bk();
		SlotWord(h, 4, 8) = 0;
		GteReadDataReg(9, &SlotWord(h, 4, 0xC));
		GteMVMVA_LightV2Bk();
		SlotWord(h, 5, 8) = 0;
		GteReadDataReg(9, &SlotWord(h, 5, 0xC));
		const int32_t d0 = SlotWord(h, 3, 0xC), d1 = SlotWord(h, 4, 0xC), d2 = SlotWord(h, 5, 0xC);
		if (d0 != 0 && (d0 ^ d1) < 0) SlotWord(h, 3, 8) = 1;
		if (d1 != 0 && (d1 ^ d2) < 0) SlotWord(h, 4, 8) = 2;
		if (d2 != 0 && (d2 ^ d0) < 0) SlotWord(h, 5, 8) = 4;
		h->clip = (uint32_t)(SlotWord(h, 5, 8) | SlotWord(h, 3, 8) | SlotWord(h, 4, 8));
		if (h->clip) ClipTriangle(h);
		else if ((d2 | d1 | d0) > 0) ClipTriFT3(h, Slot(h, 0), Slot(h, 1), Slot(h, 2));
	}

	static inline void LoadSlot(ClipHeader *h, int32_t k, uint16_t index)
	{
		const uint32_t v = h->verts + (uint32_t)(index & 0xFFF) * 8;
		*(uint32_t *)h->slot[k] = At<uint32_t>(v, 0);
		*(uint32_t *)(h->slot[k] + 4) = At<uint32_t>(v, 4);
	}

	static uint32_t ClipRenderPrims(ClipHeader *h, uint32_t ot, uint32_t shift, uint32_t cursor)
	{
		h->col = h->code;
		h->cursor = cursor;
		uint32_t p = h->prims;
		h->ot = ot;
		h->shift = shift;
		for (uint32_t k = 0; k < h->ntri; k++, p += 0x10)
		{
			LoadSlot(h, 0, At<uint16_t>(p, 0));
			LoadSlot(h, 1, At<uint16_t>(p, 2));
			LoadSlot(h, 2, At<uint16_t>(p, 4));
			GteLoadV012(Slot(h, 0), Slot(h, 1), Slot(h, 2));
			GteRTPT();
			h->uv0 = At<uint32_t>(p, 8);
			h->uv1 = At<uint32_t>(p, 0xC);
			Slot(h, 0)[3] = (int16_t)At<uint16_t>(p, 8);
			Slot(h, 1)[3] = (int16_t)At<uint16_t>(p, 0xC);
			Slot(h, 2)[3] = (int16_t)At<uint16_t>(p, 6);
			GteReadFLAG(&h->flag);
			if (h->flag & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(&h->mac0);
			if (h->mac0 <= 0) continue;
			ClipPlaneTest(h);
		}
		for (uint32_t k = 0; k < h->nquad; k++, p += 0x14)
		{
			LoadSlot(h, 0, At<uint16_t>(p, 0));
			LoadSlot(h, 1, At<uint16_t>(p, 2));
			LoadSlot(h, 2, At<uint16_t>(p, 4));
			GteLoadV012(Slot(h, 0), Slot(h, 1), Slot(h, 2));
			GteRTPT();
			const uint32_t uv3 = At<uint32_t>(p, 0x10);
			h->uv0 = At<uint32_t>(p, 8);
			Slot(h, 2)[3] = (int16_t)uv3;
			h->uv1 = At<uint32_t>(p, 0xC);
			Slot(h, 0)[3] = (int16_t)At<uint16_t>(p, 8);
			Slot(h, 1)[3] = (int16_t)At<uint16_t>(p, 0xC);
			GteReadFLAG(&h->flag);
			if (h->flag & 0x60000) continue;
			GteNCLIP();
			GteReadMAC0(&h->mac0);
			if (h->mac0 <= 0) continue;
			ClipPlaneTest(h);
			LoadSlot(h, 0, At<uint16_t>(p, 6));
			LoadSlot(h, 1, At<uint16_t>(p, 2));
			LoadSlot(h, 2, At<uint16_t>(p, 4));
			GteLoadV012(Slot(h, 0), Slot(h, 1), Slot(h, 2));
			GteRTPT();
			Slot(h, 2)[3] = (int16_t)uv3;
			Slot(h, 1)[3] = (int16_t)h->uv1;
			Slot(h, 0)[3] = (int16_t)(uv3 >> 16);
			GteReadFLAG(&h->flag);
			ClipPlaneTest(h);
		}
		return h->cursor;
	}

	static uint32_t ClipRenderModel(uint32_t model, ClipHeader *h, uint32_t ot, uint32_t shift, uint32_t cursor)
	{
		GteWriteControlReg(*(const int32_t *)&h->plane[0], 8);
		const int32_t w = *(const int32_t *)&h->plane[2];
		GteWriteControlReg(w, 9);
		GteWriteControlReg(w >> 16, 0xD);
		for (int32_t i = 0; i < 0x38; i++) (&h->ptr[0][0])[i] = (uint32_t)h + 0x58 + var<uint32_t>(game_addr(CLIP_SLOTS) + 4 * i) * 0x10;
		const uint32_t bones = At<uint32_t>(model, 0) + 0x10;
		uint32_t offs = At<uint32_t>(model, 4);
		const int32_t nparts = At<int32_t>(offs, 0);
		offs += 4;
		for (int32_t i = 0; i < nparts; i++)
		{
			uint32_t s = At<uint32_t>(offs, 0) + At<uint32_t>(model, 4);
			offs += 4;
			if (!(h->mask & (uint32_t)shl32(1, i))) continue;
			int32_t groups = At<int16_t>(s, 0);
			uint32_t out = h->verts;
			s += 2;
			for (; groups > 0; groups--)
			{
				const uint32_t m = (uint32_t)At<int16_t>(s, 0) * 0x30 + bones + 0x10;
				s += 2;
				GteSetRotMatrixCtrl((const Mat4x3 *)m);
				GteSetTransVectorCtrl((const Mat4x3 *)m);
				int32_t n = At<int16_t>(s, 0);
				s += 2;
				for (; n != 0; n--)
				{
					GteWriteDataReg((uint32_t)At<uint16_t>(s, 0) | (uint32_t)At<uint16_t>(s, 2) << 16, 0);
					GteWriteDataReg(At<uint16_t>(s, 4), 1);
					s += 6;
					out += 8;
					GteMVMVA_RotV0Tr2();
					int32_t ir[3];
					GteReadDataRegTo(&ir[0], 9);
					GteReadDataRegTo(&ir[1], 10);
					GteReadDataRegTo(&ir[2], 11);
					At<uint32_t>(out, -8) = (uint32_t)ir[1] << 16 | ((uint32_t)ir[0] & 0xFFFF);
					At<int16_t>(out, -4) = (int16_t)ir[2];
				}
			}
			s = (s + 3) & ~3u;
			h->ntri = At<uint16_t>(s, 0);
			h->nquad = At<uint16_t>(s, 2);
			h->w0C = At<uint16_t>(s, 4);
			h->w0E = At<uint16_t>(s, 6);
			h->prims = s + 0xC;
			GteSetRotMatrixCtrl((const Mat4x3 *)var<uint32_t>(game_addr(CAM_PTR)));
			GteSetTransVectorCtrl((const Mat4x3 *)var<uint32_t>(game_addr(CAM_PTR)));
			cursor = ClipRenderPrims(h, ot, shift, cursor);
		}
		return cursor;
	}

	static void PlaneDistance(const int16_t *a, int16_t *b)
	{
		b[3] = (int16_t)-((mul32(a[2], b[2]) + mul32(a[1], b[1]) + mul32(a[0], b[0])) >> 12);
	}

	static void DrawClipped(ClipHeader *h, uint8_t *ent)
	{
		uint8_t *b = (uint8_t *)h;
		if (!(*ent & 0x20)) FrameArena() = DrawShadow(ent, OT0() + 0x4064, 0x10, FrameArena());
		ff8fx::copy<(0x20)>(b + 0xC8, ent + 0x40);
		var<uint32_t>(game_addr(CAM_PTR)) = game_addr(game::unk_1D97778);
		ComputeBonesWorldMatrices(ent + 0x60, b + 0xC8);
		h->verts = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		h->code = *(const uint32_t *)(ent + 0x28);
		h->mask = *(const uint32_t *)(ent + 0x7C);
		FrameArena() = ClipRenderModel(*(const uint32_t *)(ent + 0x64), h, RenderOT(), 2, FrameArena());
		BuildBoneMatricesFromPose(ent + 0x60);
		uint8_t *w = *(uint8_t **)(ent + 0x78);
		if (w && !(ent[1] & 2))
		{
			h->mask = 0xFFFFFFFF;
			ComposeAffineTransform((const Mat4x3 *)(b + 0xC8), (const Mat4x3 *)(w + 0x24), (Mat4x3 *)(b + 0xC8));
			ComputeBonesWorldMatrices(w, b + 0xC8);
			FrameArena() = ClipRenderModel(*(const uint32_t *)(w + 4), h, RenderOT(), 2, FrameArena());
			BuildBoneMatricesFromPose(w);
		}
	}

	static void TrailPoints(TrailWork *w)
	{
		WeaponBone(Caster(), 1, w->dir, w->base);
		w->tip[0] = (int16_t)((int16_t)(mul32(w->dir[0], 666) >> 12) + w->base[0]);
		w->tip[1] = (int16_t)((int16_t)(mul32(w->dir[1], 666) >> 12) + w->base[1]);
		w->tip[2] = (int16_t)((int16_t)(mul32(w->dir[2], 666) >> 12) + w->base[2]);
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
		GteSetFarColor(0, 0, 0);
		uint32_t *pk = (uint32_t *)PacketCursor();
		uint32_t *pk2 = pk + 9;
		w->base_col = 0x3A903030;
		w->col1 = 0x3A903030;
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

	static int32_t TrailWant(int32_t c) { return c >= 0x128 ? 0x12E - c : 6; }

	static uint32_t g_trail_slot4 = 0;
	static bool g_trail_slot4_known = false;

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
			g_trail_slot4 = 0x1E000;
			g_trail_slot4_known = true;
		}
		else g_trail_slot4_known = false;
		FieldFree(0x40);
		if (DrawOnly()) return 0;
		n->c++;
		return EndAt(n->c, 0x12D);
	}

	static Node24 *AddMain(uint32_t fn) { return (Node24 *)AddTaskToQueue(QMain(), fn); }

	static void AddCleared(uint32_t fn)
	{
		Node24 *t = AddMain(fn);
		t->c = 0;
		t->e = 0;
	}

	static void AddFlash()
	{
		Node24 *t = AddMain(game_addr(ORIG_Flash));
		t->c = 0;
		t->e = 0x10;
		t->w18 = 0x800;
		t->w1A = 1;
	}

	static void AddTrail()
	{
		Node24 *t = AddMain(game_addr(ORIG_Trail));
		t->c = 0;
		for (uint32_t a = game_addr(RING); a < game_addr(RING) + 6 * 0x14; a += 0x14) *(uint32_t *)a = 0;
	}

	static void AddGroundRing()
	{
		GetEffectSpawnPosition(Caster(), 0xF1, 0, (void *)game_addr(BONE_F1));
		Node24 *t = AddMain(game_addr(ORIG_GroundRing));
		t->c = 0;
		t->w10 = var<int16_t>(game_addr(BONE_F1));
		t->w12 = 0;
		t->w14 = var<int16_t>(game_addr(BONE_F1) + 4);
		t->w18 = (int16_t)(CrtRand() % 0x1000);
		t->w1C = 0x1800;
		t->w22 = 0x100;
		t->w20 = 0x100;
	}

	static void AddRockTrio()
	{
		Node24 *t = AddMain(game_addr(ORIG_RockBurst));
		t->e = TakeId();
		t->c = 0;
		t->w10 = var<int16_t>(game_addr(BONE_F1));
		t->w12 = 0;
		t->w14 = var<int16_t>(game_addr(BONE_F1) + 4);
		t = AddMain(game_addr(ORIG_Debris));
		t->e = TakeId();
		*(uint32_t *)&t->w10 = var<uint32_t>(game_addr(BONE_F1));
		t->c = 0;
		*(uint32_t *)&t->w14 = var<uint32_t>(game_addr(BONE_F1) + 4);
		t = AddMain(game_addr(ORIG_BoneSparks));
		t->e = TakeId();
		t->c = 0;
	}

	static void AddBladeSparks()
	{
		Node24 *t = AddMain(game_addr(ORIG_BladeSparks));
		t->e = TakeId();
		t->c = 0;
	}

	static void AtTip(Node24 *t)
	{
		*(uint32_t *)&t->w10 = var<uint32_t>(game_addr(TIP));
		*(uint32_t *)&t->w14 = var<uint32_t>(game_addr(TIP) + 4);
	}

	static void AddTipFlash()
	{
		SetTipPoint();
		Node24 *t = AddMain(game_addr(ORIG_TipFlash));
		AtTip(t);
		t->c = 0;
		t->w1E = 0x95;
		t->w1C = 0x95;
	}

	static void AddAtTipWithId(uint32_t fn)
	{
		Node24 *t = AddMain(fn);
		t->e = TakeId();
		t->c = 0;
		AtTip(t);
	}

	static void AddTipRings()
	{
		for (int16_t k = 0; k < 2; k++)
		{
			Node24 *t = AddMain(game_addr(ORIG_TipRing));
			AtTip(t);
			t->c = 0;
			t->e = k;
			t->w18 = (int16_t)(CrtRand() % 0x1000);
			const int32_t r = CrtRand() % 0x3C;
			t->w1E = 0xC00;
			t->w1C = 0xC00;
			t->w1A = (int16_t)(r + 0x1E);
		}
	}

	static void AddTargetFlatRing()
	{
		Node24 *t = AddMain(game_addr(ORIG_FlatRing));
		t->c = 0;
		GetDefaultEffectPosition(Target(), &t->w10);
		t->w18 = (int16_t)(CrtRand() % 0x1000);
		t->w1C = 0xE00;
		const int16_t s = (int16_t)((CrtRand() % 0x300 + 0x600) / 3);
		t->w22 = s;
		t->w20 = s;
	}

	static void AddCasterFlatRing()
	{
		Node24 *t = AddMain(game_addr(ORIG_FlatRing));
		t->c = 0;
		GetEffectSpawnPosition(Caster(), 0xF0, 0, &t->w10);
		t->w18 = (int16_t)(CrtRand() % 0x1000);
		t->w1C = 0xC00;
		const int16_t s = (int16_t)((CrtRand() % 0x200 + 0x480) / 3);
		t->w22 = s;
		t->w20 = s;
	}

	static void AddTipFlash2()
	{
		SetTipPoint();
		Node24 *t = AddMain(game_addr(ORIG_TipFlash2));
		AtTip(t);
		t->c = 0;
		t->w18 = (int16_t)(CrtRand() % 0x1000);
		t->w1A = 0x40;
		t->w1E = 0x95;
		t->w1C = 0x95;
	}

	static void AddStreak()
	{
		SetTipPoint();
		Node24 *t = AddMain(game_addr(ORIG_Streak));
		AtTip(t);
		t->c = 0;
		t->e = 2;
		t->w1C = 0x1000;
		t->w20 = 0x2000;
	}

	static void AddFinal()
	{
		Node24 *a = AddMain(game_addr(ORIG_FinalRing));
		a->c = 0;
		GetDefaultEffectPosition(Target(), &a->w10);
		a->w18 = (int16_t)(CrtRand() % 0x400 - 0x200);
		const int16_t s = (int16_t)((CrtRand() % 0x1200 + 0x3000) / 3);
		a->w1E = s;
		a->w1C = s;
		const uint32_t p0 = *(const uint32_t *)&a->w10, p1 = *(const uint32_t *)&a->w14;
		Node24 *t = AddMain(game_addr(ORIG_FinalSparkA));
		t->e = TakeId();
		*(uint32_t *)&t->w10 = p0;
		t->c = 0;
		*(uint32_t *)&t->w14 = p1;
		t = AddMain(game_addr(ORIG_FinalShards));
		t->e = TakeId();
		*(uint32_t *)&t->w10 = p0;
		t->c = 0;
		*(uint32_t *)&t->w14 = p1;
		t = AddMain(game_addr(ORIG_FinalFlash));
		*(uint32_t *)&t->w10 = p0;
		t->c = 0;
		*(uint32_t *)&t->w14 = p1;
		const int16_t f = (int16_t)((CrtRand() % 0xC00 + 0x3000) / 4);
		t->w1E = f;
		t->w1C = f;
		t = AddMain(game_addr(ORIG_FinalRingSparks));
		t->e = TakeId();
		*(uint32_t *)&t->w10 = p0;
		t->c = 0;
		*(uint32_t *)&t->w14 = p1;
		for (uint32_t k = 0, a3 = PoolCBase(); k < 500; k++, a3 += 0x18) *(uint32_t *)a3 = 0;
		uint8_t *cs = Caster();
		const uint16_t cf = *(const uint16_t *)cs;
		Keep(CasterSlot())->flag20 = (uint16_t)(cf & 0x20);
		uint8_t *tg = Target();
		const uint8_t tf = *tg;
		*(uint16_t *)cs = (uint16_t)(cf | 0x20);
		*tg |= 0x20;
		Keep(TargetSlot())->flag20 = (uint16_t)(tf & 0x20);
		uint8_t *e = *(uint8_t **)(tg + 0x8C);
		if (e && e != tg)
		{
			do
			{
				if (*e & 2)
				{
					const int32_t i = EntityIndex(e);
					if (i < 7)
					{
						uint8_t *le = Entity(i);
						const uint16_t lf = *(const uint16_t *)le;
						Keep(i)->flag20 = (uint16_t)(lf & 0x20);
						*(uint16_t *)le = (uint16_t)(lf | 0x20);
					}
				}
				e = *(uint8_t **)(e + 0x8C);
			} while (e != tg);
		}
		const int32_t k = ResultIndex();
		ResultIndex() = k + 1;
		ApplyActionResultToTarget(Context()->actions->targets + TARGET_STRIDE * k);
	}

	static void AddTargetSink()
	{
		Node24 *t = AddMain(game_addr(ORIG_TargetSink));
		t->c = 0;
		t->e = (int16_t)TargetSlot();
		uint8_t *tg = Target();
		uint8_t *e = *(uint8_t **)(tg + 0x8C);
		if (e && e != tg)
		{
			do
			{
				if (*e & 2)
				{
					const int32_t i = EntityIndex(e);
					if (i < 7)
					{
						t = AddMain(game_addr(ORIG_TargetSink));
						t->c = 0;
						t->e = (int16_t)i;
					}
				}
				e = *(uint8_t **)(e + 0x8C);
			} while (e != tg);
		}
		uint8_t *cs = Caster();
		*(uint16_t *)cs = (uint16_t)(Keep(CasterSlot())->flag20 | (*(const uint16_t *)cs & 0xFFDF));
	}

	static uint32_t __cdecl DirectorTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)game_addr(FRAME), (Mat4x3 *)game_addr(CAM_FRAME));
		if (UpdateFlags() & 0x201)
		{
			if (UpdateFlags() & 1) return 0;
			if (PauseQuery() < 0) return 0;
		}
		if (n->c == 0)
		{
			StopSounds(-1, 0);
			CameraScriptStart(game_addr(CAMERA_SCRIPT), game_addr(FRAME), Caster(), QMain(), 0);
			ArmCameraReturn();
			BindEntityModelFromComData(Caster(), game_addr(BIND_OUT), game_addr(ANIM_DATA));
			var<uint32_t>(game_addr(TARGET_KEPT)) = *(const uint32_t *)(Target() + 0x1C);
			var<uint32_t>(game_addr(TARGET_KEPT) + 4) = *(const uint32_t *)(Target() + 0x20);
			var<uint32_t>(game_addr(CASTER_KEPT)) = *(const uint32_t *)(Caster() + 0x1C);
			var<uint32_t>(game_addr(CASTER_KEPT) + 4) = *(const uint32_t *)(Caster() + 0x20);
			for (int32_t i = 0; i < 7; i++)
			{
				Keep(i)->pos[0] = *(const uint32_t *)(Entity(i) + 0x1C);
				Keep(i)->pos[1] = *(const uint32_t *)(Entity(i) + 0x20);
			}
			GetDefaultEffectPosition(Target(), (int16_t *)game_addr(TARGET_ANCHOR));
		}
		int32_t c = n->c;
		if (c < 0x4E)
		{
			if (c == 0xA)
			{
				BdPlaySE(SOUND_A(), 0, 0x80);
				AddRockTrio();
			}
			else if (c == 0) QueueChainTransformation(Caster(), 1);
			else if (c == 2) CharacterLoad(0x21C, TexBase());
			else if (c == 0xE)
			{
				if (PauseQuery() < 0) return 0;
				QueueTIMUpload(TexBase());
			}
			else if (c == 0xF) CharacterLoad(0x21D, TexBase());
			else if (c == 1) AddTrail();
			else if (c == 8) AddGroundRing();
			else if (c == 0x14) AddTipFlash();
			else if (c == 0x18) AddAtTipWithId(game_addr(ORIG_TipShards));
			else if (c == 0x1A) AddAtTipWithId(game_addr(ORIG_TipSmoke));
			else if (c == 0x43) ScreenFadeTask(0, 1, 8, 0xFF);
			else if (c == 0x44) AddTipRings();
		}
		else if ((c -= 0x4E) < 0xD)
		{
			if (c == 0)
			{
				QueueChainTransformation(Caster(), 2);
				AddBladeSparks();
			}
			else if (c == 0xC)
			{
				GetDefaultEffectPosition(Caster(), &n->w10);
				Mat4x3 *f = (Mat4x3 *)game_addr(FRAME);
				f->t[0] = n->w10;
				f->t[1] = n->w12;
				f->t[2] = n->w14;
			}
		}
		else if ((c -= 0xD) < 0xC)
		{
			if (c == 0) AddCleared(game_addr(ORIG_HitDirector));
			else if (c == 9) AddTargetFlatRing();
		}
		else if ((c -= 0xC) < 0x26)
		{
			if (c == 0x10) BdPlaySE(SOUND_B(), 0, 0x80);
			else if (c == 0) *(uint16_t *)Caster() &= 0xFFF7;
			else
			{
				if (c >= 9 && c <= 0xD) *(int16_t *)(Caster() + 0x1E) = (int16_t)(*(const int16_t *)(Caster() + 0x1E) - 800);
				if (c == 8) AddCasterFlatRing();
			}
		}
		else if ((c -= 0x26) < 0x3C)
		{
			if (c == 0x15)
			{
				if (PauseQuery() < 0) return 0;
				QueueTIMUpload(TexBase());
			}
			else if (c == 0) AddFlash();
			else if (c == 0x14) AddCleared(game_addr(ORIG_WaveSpawner));
		}
		else if ((c -= 0x3C) < 0x2A)
		{
			if (c == 1) BdPlaySE(SOUND_C(), 0, 0x80);
			else if (c == 0)
			{
				CharacterLoad(0x21E, TexBase());
				AddTipFlash2();
			}
			else if (c == 0xA)
			{
				if (PauseQuery() < 0) return 0;
				QueueTIMUpload(TexBase());
			}
			else if (c == 6) AddAtTipWithId(game_addr(ORIG_TipRocks));
			else if (c == 0x28) ScreenFadeTask(0, 4, 8, 0xFF);
			else if (c == 0x29)
			{
				Mat4x3 *f = (Mat4x3 *)game_addr(FRAME);
				f->t[0] = var<int16_t>(game_addr(TARGET_CENTRE));
				f->t[2] = var<int16_t>(game_addr(TARGET_CENTRE) + 4);
			}
		}
		else if ((c -= 0x2A) < 0x5A)
		{
			bool step = false;
			if (c == 0)
			{
				QueueChainTransformation(Caster(), 3);
				GetDefaultEffectPosition(Caster(), (int16_t *)game_addr(CASTER_POS));
				GetDefaultEffectPosition(Target(), (int16_t *)game_addr(TARGET_POS));
				const int32_t dz = (int32_t)var<int16_t>(game_addr(TARGET_POS) + 4) - var<int16_t>(game_addr(CASTER_POS) + 4);
				n->w22 = dz < -2200 ? (int16_t)((dz * 2 + 0x1130) / 16) : 0;
				step = true;
			}
			else if (c < 0x10) step = true;
			if (step) *(int16_t *)(Caster() + 0x20) = (int16_t)(*(const int16_t *)(Caster() + 0x20) + n->w22);
			if (c == 2) AddStreak();
			else if (c == 0x2D) AddFinal();
			else if (c == 0x59)
			{
				Mat4x3 *f = (Mat4x3 *)game_addr(FRAME);
				f->t[0] = var<int16_t>(game_addr(CASTER_KEPT));
				f->t[1] = var<int16_t>(game_addr(CASTER_KEPT) + 2);
				f->t[2] = var<int16_t>(game_addr(CASTER_KEPT) + 4);
			}
		}
		else if ((c -= 0x5A) < 0x25)
		{
			if (c == 0xA) BdPlaySE(SOUND_D(), 0, 0x80);
			else if (c == 0)
			{
				QueueChainTransformation(Caster(), 4);
				*(uint32_t *)(Caster() + 0x1C) = var<uint32_t>(game_addr(CASTER_KEPT));
				*(uint32_t *)(Caster() + 0x20) = var<uint32_t>(game_addr(CASTER_KEPT) + 4);
				SetScreenFlash(0, 0);
				StageGroupsHide();
			}
			else if (c == 2)
			{
				AddTargetSink();
				AddCleared(game_addr(ORIG_Restore));
			}
		}
		n->c++;
		if (n->c <= 0x172) return 0;
		StageGroupsHide();
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

	static void RenderToCursor(uint8_t *h) { PacketCursor() = RenderPrimModel(h, RenderOT(), 2, PacketCursor()); }
	static void RenderToArena(uint8_t *h) { FrameArena() = RenderPrimModel(h, RenderOT(), 2, FrameArena()); }

	static void WorldMatrix(Mat4x3 *m, int16_t ax, int16_t ay, int16_t az, int32_t tx, int32_t ty, int32_t tz, int32_t sx, int32_t sy, int32_t sz, const Mat4x3 *view)
	{
		int16_t ang[4];
		ang[0] = ax;
		ang[1] = ay;
		ang[2] = az;
		ComposeZYXRotationMatrix(ang, m);
		m->t[0] = tx;
		m->t[1] = ty;
		m->t[2] = tz;
		int32_t sv[3];
		sv[0] = sx;
		sv[1] = sy;
		sv[2] = sz;
		Scale3DMatrix(m, sv);
		ComposeAffineTransform(view, m, m);
		GteSetRotMatrix(m);
		GteSetTransVector(m);
	}

	static void FacingMatrix(Mat4x3 *m, int16_t roll, int32_t s, const int16_t *pos, int32_t scale, int32_t off)
	{
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = roll;
		ComposeZYXRotationMatrix(ang, m);
		int32_t sv[3];
		sv[0] = s;
		sv[1] = s;
		sv[2] = s;
		Scale3DMatrix(m, sv);
		TransformCameraByShadowRotation(pos, scale, off);
		GteSetRotMatrix(m);
	}

	static int32_t GroundRingFade(int32_t c) { return shl32(8 - c, 9); }
	static void GroundRingDraw(const Node24 *n, int16_t sy, int32_t scroll, int32_t c, int32_t fade)
	{
		Mat4x3 m = {};
		WorldMatrix(&m, 0, n->w18, 0, n->w10, n->w12, n->w14, n->w1C, sy, n->w1C, &Camera());
		uint8_t *h = AllocHeader(0x90);
		*(uint32_t *)(h + 8) = 0;
		*(int16_t *)(h + 0x18) = 0x40;
		*(int16_t *)(h + 0x1C) = 0x40;
		*(int16_t *)(h + 0x1A) = 0;
		*(uint32_t *)(h + 0x10) = 0;
		*(uint32_t *)h = game_addr(MODEL_GROUNDRING);
		*(uint32_t *)(h + 0x20) = 0x33;
		*(int16_t *)(h + 0x1E) = 0x80;
		*(int32_t *)(h + 0x14) = scroll;
		if (c < 8)
		{
			*(uint32_t *)(h + 0x20) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		*(uint32_t *)(h + 0x24) = 0x808080;
		PacketCursor() = ScrollRender(h, RenderOT(), 2, PacketCursor());
		FieldFree(0x90);
	}

	static uint32_t __cdecl GroundRingTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		GroundRingDraw(n, n->w20, shl32(n->c, 4), n->c, GroundRingFade(n->c));
		if (DrawOnly()) return 0;
		const int16_t r = n->w22;
		n->w20 = (int16_t)(n->w20 + r);
		n->w22 = (int16_t)(r - r / 32);
		n->c++;
		return EndAt(n->c, 0x3C);
	}

	static void TipFlashDraw(const Node24 *n, int16_t s)
	{
		Mat4x3 m = {};
		FacingMatrix(&m, 0, s, &n->w10, 0, -(s >> 3));
		uint8_t *h = ModelHeader(game_addr(MODEL_TIPFLASH), false, 0);
		RenderToCursor(h);
		FieldFree(0x58);
	}

	static uint32_t __cdecl TipFlashTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		TipFlashDraw(n, n->w1C);
		if (DrawOnly()) return 0;
		const int16_t c = n->c;
		if (c < 0x10)
		{
			const int16_t r = n->w1E;
			n->w1C = (int16_t)(n->w1C + r);
			n->w1E = (int16_t)(r - r / 32);
		}
		n->c = (int16_t)(c + 1);
		return EndAt(n->c, 0x30);
	}

	static void TipRingDraw(const Node24 *n, int16_t roll, int16_t s, int32_t c, int32_t fade)
	{
		Mat4x3 m = {};
		int16_t ang[4];
		ang[0] = 0x200;
		ang[1] = (int16_t)(n->e != 0 ? 0x300 : -0x300);
		ang[2] = roll;
		ComposeZYXRotationMatrix(ang, &m);
		GteMatrixMultiply((const Mat4x3 *)game_addr(FRAME), &m);
		m.t[0] = n->w10;
		m.t[1] = n->w12;
		m.t[2] = n->w14;
		int32_t sv[3];
		sv[0] = s;
		sv[1] = s;
		sv[2] = s;
		Scale3DMatrix(&m, sv);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = ModelHeader(game_addr(MODEL_TIPRING), c >= 4, fade);
		RenderToCursor(h);
		FieldFree(0x58);
	}

	static uint32_t __cdecl TipRingTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		TipRingDraw(n, n->w18, n->w1C, n->c, mul32(n->c - 4, 682));
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w18 = (int16_t)(n->w18 + n->w1A);
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 7);
		n->c++;
		return EndAt(n->c, 0xA);
	}

	static void HitSlashDraw(const Node24 *n, uint32_t model, int16_t s, bool fade_on, int32_t fade)
	{
		const HitRec *r = Hit(n->e);
		Mat4x3 m1 = {}, m2 = {};
		int16_t ang[4];
		ang[0] = 0;
		ang[1] = 0;
		ang[2] = n->w18;
		ComposeZYXRotationMatrix(ang, &m1);
		int32_t axis[4];
		const int32_t angle = RotationBetweenVectors((const int32_t *)game_addr(REF_BACK), r->dir, axis);
		BuildAxisAngleRotationMatrix(angle, &m2, axis);
		GteMatrixMultiply(&m2, &m1);
		m1.t[0] = r->pos[0];
		m1.t[1] = r->pos[1];
		int32_t sv[3];
		sv[2] = s;
		sv[1] = s;
		sv[0] = s;
		m1.t[2] = r->pos[2];
		Scale3DMatrix(&m1, sv);
		ComposeAffineTransform(&Camera(), &m1, &m1);
		GteSetRotMatrix(&m1);
		GteSetTransVector(&m1);
		uint8_t *h = ModelHeader(model, fade_on, fade);
		RenderToCursor(h);
		FieldFree(0x58);
	}

	static uint32_t __cdecl HitSlashATask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		HitSlashDraw(n, game_addr(MODEL_SLASHA), n->w1C, n->c >= 4, mul32(n->c - 4, 682));
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 4);
		n->c++;
		return EndAt(n->c, 0xA);
	}

	static uint32_t __cdecl HitSlashBTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		HitSlashDraw(n, game_addr(MODEL_SLASHB), n->w1C, n->c >= 2, shl32(n->c - 2, 9));
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 24);
		n->c++;
		return EndAt(n->c, 0xA);
	}

	static void FlatRingDraw(const Node24 *n, int16_t sy, int32_t c, int32_t fade)
	{
		Mat4x3 m = {};
		WorldMatrix(&m, 0, n->w18, 0, n->w10, 0, n->w14, n->w1C, sy, n->w1C, &Camera());
		uint8_t *h = ModelHeader(game_addr(MODEL_FLATRING), c >= 2, fade);
		RenderToArena(h);
		FieldFree(0x58);
	}

	static uint32_t __cdecl FlatRingTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		FlatRingDraw(n, n->w20, n->c, shl32(n->c - 2, 9));
		if (DrawOnly()) return 0;
		const int16_t r = n->w22;
		n->w20 = (int16_t)(n->w20 + r);
		n->w22 = (int16_t)(r - r / 4);
		n->c++;
		return EndAt(n->c, 0xA);
	}

	static uint32_t __cdecl WaveSpawnerTask(TaskNode *tn)
	{
		if (DrawOnly()) return 0;
		Node24 *n = (Node24 *)tn;
		if (n->c % 12 == 1)
		{
			Node24 *t = AddMain(game_addr(ORIG_ScreenWave));
			t->c = 0;
			t->w18 = (int16_t)shl32(CrtRand() % 8, 9);
			t->w1A = (int16_t)shl32(CrtRand() % 4, 9);
			t->w1C = 0x2000;
			t->w20 = (int16_t)(CrtRand() % 16 + 6);
		}
		n->c++;
		return EndAt(n->c, 0x63);
	}

	static int32_t ScreenWaveFade(int32_t c) { return c < 8 ? 0x1000 - mul32(c, 0x180) : c >= 0x14 ? mul32(c, 0x180) - 0x1A00 : 0x400; }
	static uint32_t ScreenWaveColour(int32_t c) { return var<uint32_t>(game_addr(WAVE_COLOURS) + 4 * (c % 0x12)); }
	static void ScreenWaveDraw(const Node24 *n, int32_t scroll, int32_t fade, uint32_t colour)
	{
		Mat4x3 m = {};
		int16_t ang[4];
		ang[0] = n->w18;
		ang[1] = 0;
		ang[2] = n->w1A;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t sv[3];
		sv[2] = n->w1C;
		sv[1] = n->w1C;
		sv[0] = n->w1C;
		m.t[0] = 0;
		m.t[1] = 0;
		m.t[2] = ProjectionH();
		Scale3DMatrix(&m, sv);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = AllocHeader(0x90);
		*(uint32_t *)(h + 8) = 0;
		*(int16_t *)(h + 0x18) = 0;
		*(int16_t *)(h + 0x1A) = 0;
		*(uint32_t *)(h + 0x10) = 0;
		*(uint32_t *)h = game_addr(MODEL_WAVE);
		*(uint32_t *)(h + 0x20) = 0xF3;
		*(int16_t *)(h + 0x1C) = 0x40;
		*(int16_t *)(h + 0x1E) = 0x80;
		*(int32_t *)(h + 0x14) = scroll;
		*(int32_t *)(h + 0xC) = fade;
		*(uint32_t *)(h + 0x24) = colour;
		FrameArena() = ScrollRender(h, OT0() + 0x4068, 6, FrameArena());
		FieldFree(0x90);
	}

	static uint32_t __cdecl ScreenWaveTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ScreenWaveDraw(n, mul32(n->w20, n->c), ScreenWaveFade(n->c), ScreenWaveColour(n->c));
		if (DrawOnly()) return 0;
		n->c++;
		return EndAt(n->c, 0x1C);
	}

	static int32_t TipFlash2Fade(int32_t c) { return c < 8 ? shl32(8 - c, 9) : shl32(c - 0x20, 9); }
	static void TipFlash2Draw(const Node24 *n, int16_t roll, int16_t s, int32_t c, int32_t fade)
	{
		Mat4x3 m = {};
		FacingMatrix(&m, roll, s, &n->w10, 0x1000, -(s >> 3));
		uint8_t *h = ModelHeader(game_addr(MODEL_TIPFLASH2), c < 8 || c >= 0x20, fade);
		RenderToCursor(h);
		FieldFree(0x58);
	}

	static uint32_t __cdecl TipFlash2Task(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		TipFlash2Draw(n, n->w18, n->w1C, n->c, TipFlash2Fade(n->c));
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w18 = (int16_t)(n->w18 + n->w1A);
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 32);
		n->c++;
		return EndAt(n->c, 0x28);
	}

	static void StreakDraw(const Node24 *n, const int16_t *tip, int16_t sy, int32_t c, int32_t fade)
	{
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteLoadV0(&n->w10);
		GteRTPS();
		int16_t sxy[2];
		GteReadSXY2(sxy);
		int32_t v[3];
		const int16_t ax = (int16_t)(sxy[0] / 8), ay = (int16_t)(sxy[1] / 8);
		v[2] = 0;
		v[0] = ax;
		v[1] = ay;
		GteLoadV0(tip);
		GteRTPS();
		GteReadSXY2(sxy);
		const int16_t bx = (int16_t)(sxy[0] / 8), by = (int16_t)(sxy[1] / 8);
		v[0] = v[0] - bx;
		v[1] = v[1] - by;
		Normalize(v, v);
		int32_t axis[4];
		const int32_t angle = RotationBetweenVectors((const int32_t *)game_addr(REF_RIGHT), v, axis);
		Mat4x3 m = {};
		BuildAxisAngleRotationMatrix(angle, &m, axis);
		m.t[0] = bx - 0xA0;
		m.t[1] = by - 0x6C;
		m.t[2] = ProjectionH();
		int32_t sv[3];
		sv[2] = sy;
		sv[1] = sy;
		sv[0] = n->w1C;
		Scale3DMatrix(&m, sv);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = ModelHeader(game_addr(MODEL_STREAK), c >= 0x20, fade);
		RenderToCursor(h);
		FieldFree(0x58);
	}

	static int16_t StreakWidth(int32_t c) { return c <= 0xF ? (int16_t)((shl32(ComputeSin(shl32(c, 10) / 15 + 0x400), 13) >> 12) + 0x80) : 0x80; }

	static uint32_t __cdecl StreakTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->e > 0)
		{
			if (DrawOnly()) return 0;
			n->e--;
			return 0;
		}
		if (n->c == 0) SetTipPoint();
		StreakDraw(n, (const int16_t *)game_addr(TIP), n->w20, n->c, shl32(n->c - 0x20, 9));
		if (DrawOnly()) return 0;
		n->w20 = StreakWidth(n->c);
		n->c++;
		return EndAt(n->c, 0x28);
	}

	static void FinalRingDraw(const Node24 *n, int16_t s, int32_t c, int32_t fade)
	{
		Mat4x3 m = {};
		WorldMatrix(&m, 0, n->w18, 0, n->w10, n->w12, n->w14, s, s, s, &Camera());
		uint8_t *h = ModelHeader(game_addr(MODEL_FINALRING), c >= 0xC, fade);
		RenderToCursor(h);
		FieldFree(0x58);
	}

	static uint32_t __cdecl FinalRingTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		FinalRingDraw(n, n->w1C, n->c, mul32(n->c - 0xC, 682));
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 4);
		n->c++;
		return EndAt(n->c, 0x12);
	}

	static void FinalFlashDraw(const Node24 *n, int16_t s, int32_t c, int32_t fade)
	{
		Mat4x3 m = {};
		int16_t ang[4];
		ang[0] = 0x100;
		ang[2] = 0x100;
		ang[1] = 0;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t sv[3];
		sv[2] = s;
		sv[1] = s;
		sv[0] = s;
		Scale3DMatrix(&m, sv);
		TransformCameraByShadowRotation(&n->w10, 0x1000, 0);
		GteSetRotMatrix(&m);
		uint8_t *h = ModelHeader(game_addr(MODEL_FINALFLASH), c >= 0x10, fade);
		FrameArena() = RenderPrimModel(h, RenderOT(), 1, FrameArena());
		FieldFree(0x58);
	}

	static uint32_t __cdecl FinalFlashTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		var<int16_t>(game_addr(TIP) + 6) = (int16_t)(mul32(n->w1C, 850) >> 12);
		FinalFlashDraw(n, n->w1C, n->c, shl32(n->c - 0x10, 9));
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 8);
		n->c++;
		return EndAt(n->c, 0x18);
	}

	static void TargetRingDraw(const Node24 *n, int16_t s, int32_t c, int32_t fade)
	{
		Mat4x3 m = {};
		WorldMatrix(&m, 0, 0, 0, n->w10, 0, n->w14, s, s, s, &Camera());
		uint8_t *h = ModelHeader(game_addr(MODEL_ROCK), c >= 6, fade);
		RenderToCursor(h);
		FieldFree(0x58);
	}

	static uint32_t __cdecl TargetRingTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		TargetRingDraw(n, n->w1C, n->c, mul32(n->c - 6, 409));
		if (DrawOnly()) return 0;
		const int16_t r = n->w1E;
		n->w1C = (int16_t)(n->w1C + r);
		n->w1E = (int16_t)(r - r / 6);
		n->c++;
		return EndAt(n->c, 0x10);
	}

	static int32_t RockFade(int32_t life) { return mul32(life - 4, 409); }
	static void RockDraw(ModelWork *w, uint8_t *h, const PoolE *p, int16_t s, int32_t life, int32_t fade)
	{
		ComposeZYXRotationMatrix(w->ang, &w->m2);
		w->m2.t[0] = p->a[0];
		w->m2.t[1] = p->a[1];
		w->m2.t[2] = p->a[2];
		w->v[2] = s;
		w->v[1] = s;
		w->v[0] = s;
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
		RenderToCursor(h);
	}

	static uint32_t __cdecl RockBurstTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x68);
		if (!DrawOnly() && n->c <= 0x30)
		{
			*(uint32_t *)&w->ang[0] = *(const uint32_t *)&n->w10;
			*(uint32_t *)&w->ang[2] = *(const uint32_t *)&n->w14;
			if (n->c % 8 == 1)
			{
				const int32_t i = FreeA();
				if (i >= 0)
				{
					PoolE *p = EntryA(i);
					p->id = n->e;
					p->life = 0;
					*(uint32_t *)&p->a[0] = *(const uint32_t *)&n->w10;
					*(uint32_t *)&p->a[2] = *(const uint32_t *)&n->w14;
					p->a[5] = 0x600;
					p->a[4] = 0x600;
				}
			}
		}
		uint8_t *h = AllocHeader(0x58);
		w->m1 = Camera();
		const int32_t id = n->e;
		*(uint32_t *)h = game_addr(MODEL_ROCK);
		*(uint32_t *)(h + 8) = 0;
		w->ang[2] = 0;
		w->ang[1] = 0;
		w->ang[0] = 0;
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (p->id != id) continue;
			RockDraw(w, h, p, p->a[4], p->life, RockFade(p->life));
			if (DrawOnly()) continue;
			if (p->life >= 0xE) p->id = 0;
			else
			{
				p->life++;
				p->a[4] = (int16_t)(p->a[4] + p->a[5]);
				const int16_t r = p->a[5];
				p->a[5] = (int16_t)(r - r / 5);
			}
		}
		FieldFree(0x58);
		FieldFree(0x68);
		if (DrawOnly()) return 0;
		n->c++;
		if (n->c < 0x3A) return 0;
		ClearIdA(id);
		return TASK_END;
	}

	static int32_t DebrisFade(int32_t life) { return life < 6 ? 0x1000 - mul32(life, 682) : mul32(life - 8, 409); }
	static void DebrisDraw(ModelWork *w, uint8_t *h, const PoolE *p, int16_t yaw, int16_t s, int32_t life, int32_t fade)
	{
		w->ang[1] = yaw;
		ComposeZYXRotationMatrix(w->ang, &w->m2);
		w->m2.t[0] = p->a[0];
		w->m2.t[1] = p->a[1];
		w->m2.t[2] = p->a[2];
		w->v[2] = s;
		w->v[0] = s;
		w->v[1] = p->a[5];
		Scale3DMatrix(&w->m2, w->v);
		ComposeAffineTransform(&w->m1, &w->m2, &w->m2);
		GteSetRotMatrix(&w->m2);
		GteSetTransVector(&w->m2);
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (life < 6 || life >= 8)
		{
			*(int32_t *)(h + 0xC) = fade;
			*(uint32_t *)(h + 0x1C) = 0xF3;
		}
		RenderToCursor(h);
	}

	static uint32_t __cdecl DebrisTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x68);
		if (!DrawOnly() && n->c <= 0x30)
		{
			*(uint32_t *)&w->ang[0] = *(const uint32_t *)&n->w10;
			*(uint32_t *)&w->ang[2] = *(const uint32_t *)&n->w14;
			if (n->c % 10 == 2)
			{
				const int32_t i = FreeA();
				if (i >= 0)
				{
					PoolE *p = EntryA(i);
					p->id = n->e;
					p->life = 0;
					*(uint32_t *)&p->a[0] = *(const uint32_t *)&n->w10;
					*(uint32_t *)&p->a[2] = *(const uint32_t *)&n->w14;
					p->a[1] = (int16_t)(p->a[1] + (int16_t)(CrtRand() % 0x4B0 - 0x258));
					const int16_t s = (int16_t)((CrtRand() % 0x600 + 0x800) / 8);
					p->a[6] = s;
					p->a[4] = s;
					p->a[5] = (int16_t)(CrtRand() % 0x600 + 0x300);
					p->a[3] = (int16_t)(CrtRand() % 0x800);
					p->a[7] = (int16_t)(CrtRand() % 0x3C + 0x14);
				}
			}
		}
		uint8_t *h = AllocHeader(0x58);
		w->m1 = Camera();
		const int32_t id = n->e;
		*(uint32_t *)h = game_addr(MODEL_DEBRIS);
		*(uint32_t *)(h + 8) = 0;
		w->ang[2] = 0;
		w->ang[0] = 0;
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (p->id != id) continue;
			DebrisDraw(w, h, p, p->a[3], p->a[4], p->life, DebrisFade(p->life));
			if (DrawOnly()) continue;
			if (p->life >= 0x12) p->id = 0;
			else
			{
				p->life++;
				p->a[4] = (int16_t)(p->a[4] + p->a[6]);
				const int16_t r = p->a[6];
				p->a[6] = (int16_t)(r - r / 24);
				p->a[3] = (int16_t)(p->a[3] + p->a[7]);
			}
		}
		FieldFree(0x58);
		FieldFree(0x68);
		if (DrawOnly()) return 0;
		n->c++;
		if (n->c < 0x3A) return 0;
		ClearIdA(id);
		return TASK_END;
	}

	static int32_t TipShardFade(int32_t life) { return life < 4 ? shl32(4 - life, 10) : mul32(life - 10, 682); }
	static void TipShardDraw(ModelWork *w, uint8_t *h, int16_t scale, int16_t phase, int16_t roll, int32_t life, int32_t fade)
	{
		w->ang[2] = roll;
		ComposeZYXRotationMatrix(w->ang, &w->m2);
		w->v[0] = mul32(ComputeSin(phase), scale) >> 12;
		Scale3DMatrix(&w->m2, w->v);
		GteSetRotMatrix(&w->m2);
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (life < 4)
		{
			*(int32_t *)(h + 0xC) = fade;
			*(uint32_t *)(h + 0x1C) = 0xF3;
		}
		else if (life >= 0xA)
		{
			*(int32_t *)(h + 0xC) = fade;
			*(uint32_t *)(h + 0x1C) = 0xF3;
		}
		RenderToArena(h);
	}

	static void TipShardFrame(ModelWork *w, const Node24 *n, const Mat4x3 *view)
	{
		w->ang[0] = 0;
		w->ang[1] = 0;
		w->ang[2] = 0;
		ComposeZYXRotationMatrix(w->ang, &w->m1);
		w->m1.t[0] = n->w10;
		w->m1.t[1] = n->w12;
		w->m1.t[2] = n->w14;
		ComposeAffineTransform(view, &w->m1, &w->m1);
		GteSetTransVector(&w->m1);
	}

	static uint32_t __cdecl TipShardsTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x68);
		if (!DrawOnly() && n->c <= 0x22 && n->c % 3 == 1)
		{
			const int32_t i = FreeA();
			if (i >= 0)
			{
				PoolE *p = EntryA(i);
				p->id = n->e;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x3000 + 0x1000);
				p->a[2] = 0;
				p->a[3] = 0x80;
				p->a[6] = (int16_t)(CrtRand() % 0x1000);
				const int16_t r = (int16_t)(CrtRand() % 0x28 + 0xA);
				p->a[7] = r;
				if (r & 1) p->a[7] = (int16_t)-r;
			}
		}
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_TIPSHARD);
		*(uint32_t *)(h + 8) = 0;
		TipShardFrame(w, n, &Camera());
		const int32_t id = n->e;
		w->ang[1] = 0;
		w->ang[0] = 0;
		w->v[1] = 0x3000;
		w->v[2] = 0;
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (p->id != id) continue;
			TipShardDraw(w, h, p->scale, p->a[2], p->a[6], p->life, TipShardFade(p->life));
			if (DrawOnly()) continue;
			if (p->life >= 0x10) p->id = 0;
			else
			{
				p->life++;
				p->a[2] = (int16_t)(p->a[2] + p->a[3]);
				p->a[6] = (int16_t)(p->a[6] + p->a[7]);
			}
		}
		FieldFree(0x58);
		FieldFree(0x68);
		if (DrawOnly()) return 0;
		n->c++;
		if (n->c < 0x2C) return 0;
		ClearIdA(id);
		return TASK_END;
	}

	static void ShardMove(PoolE2 *p, int k)
	{
		for (int i = 0; i < k; i++) p->a[i] = (int16_t)(p->a[i] + p->a[4 + i]);
		for (int i = 0; i < k; i++)
		{
			const int16_t v = p->a[4 + i];
			p->a[4 + i] = (int16_t)(v - (int16_t)(v >> 3));
		}
	}

	static void HitShardDraw(ModelWork *w, uint8_t *h, const PoolE2 *p, const int16_t *pos, int32_t life, int32_t fade)
	{
		w->ang[2] = p->a[6];
		ComposeZYXRotationMatrix(w->ang, &w->m2);
		w->m2.t[0] = pos[0];
		w->m2.t[2] = pos[2];
		w->m2.t[1] = pos[1];
		w->v[1] = p->scale;
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
		RenderToArena(h);
	}

	static void HitShardFrame(ModelWork *w, const Node24 *n, const HitRec *r, const Mat4x3 *view)
	{
		w->ang[0] = 0;
		w->ang[1] = 0;
		w->ang[2] = n->w18;
		ComposeZYXRotationMatrix(w->ang, &w->m1);
		const int32_t angle = RotationBetweenVectors((const int32_t *)game_addr(REF_BACK), r->dir, w->v);
		BuildAxisAngleRotationMatrix(angle, &w->m2, w->v);
		GteMatrixMultiply(&w->m2, &w->m1);
		w->m1.t[0] = r->pos[0];
		w->m1.t[1] = r->pos[1];
		w->m1.t[2] = r->pos[2];
		ComposeAffineTransform(view, &w->m1, &w->m1);
		w->v[0] = 0xC00;
		w->ang[1] = 0;
		w->ang[0] = 0;
		w->v[2] = 0;
	}

	static uint32_t __cdecl HitShardsTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		const HitRec *r = Hit(n->w1A);
		ModelWork *w = (ModelWork *)FieldAlloc(0x68);
		if (!DrawOnly() && n->c <= 0)
		{
			for (int32_t j = 0; j < 0xC; j++)
			{
				const int32_t i = FreeB();
				if (i < 0) break;
				PoolE2 *p = EntryB(i);
				p->id = n->e;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x1800 + 0xC00);
				const int32_t a = CrtRand() % 0x1000;
				w->v[0] = -ComputeSin(a);
				w->v[1] = ComputeCos(a);
				const int32_t s = CrtRand() % 0x12C + 0x1F4;
				p->a[0] = (int16_t)(mul32(w->v[0], s) >> 12);
				p->a[1] = (int16_t)(mul32(w->v[1], s) >> 12);
				p->a[2] = 0;
				const int32_t s2 = CrtRand() % 0x12C + 0x8C;
				p->a[4] = (int16_t)(mul32(w->v[0], s2) >> 12);
				p->a[5] = (int16_t)(mul32(w->v[1], s2) >> 12);
				p->a[6] = (int16_t)a;
			}
		}
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)(h + 8) = 0;
		int32_t count = 0;
		*(uint32_t *)h = game_addr(MODEL_SHARD);
		HitShardFrame(w, n, r, &Camera());
		const int32_t id = n->e;
		for (uint32_t a = game_addr(POOL_B); a < game_addr(POOL_B_END); a += 0x20)
		{
			PoolE2 *p = (PoolE2 *)a;
			if (p->id != id) continue;
			HitShardDraw(w, h, p, &p->a[0], p->life, shl32(p->life - 4, 9));
			if (DrawOnly()) continue;
			if (p->life >= 0xC) p->id = 0;
			else
			{
				p->life++;
				ShardMove(p, 2);
				count++;
			}
		}
		FieldFree(0x58);
		FieldFree(0x68);
		if (DrawOnly()) return 0;
		n->c++;
		return EndIfNone(count);
	}

	static void TipRockDraw(ModelWork *w, uint8_t *h, const PoolE2 *p, int16_t s, int16_t ax, int16_t az, int32_t life, int32_t fade)
	{
		w->ang[0] = ax;
		w->ang[2] = az;
		ComposeZYXRotationMatrix(w->ang, &w->m2);
		w->m2.t[0] = p->a[0];
		w->m2.t[1] = p->a[1];
		w->m2.t[2] = p->a[2];
		w->v[2] = s;
		w->v[1] = s;
		w->v[0] = s;
		Scale3DMatrix(&w->m2, w->v);
		ComposeAffineTransform(&w->m1, &w->m2, &w->m2);
		GteSetRotMatrix(&w->m2);
		GteSetTransVector(&w->m2);
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (life >= 0x10)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		RenderToArena(h);
	}

	static uint32_t __cdecl TipRocksTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x68);
		if (!DrawOnly() && n->c <= 0xC && n->c % 4 == 0)
		{
			const int32_t i = FreeB();
			if (i >= 0)
			{
				PoolE2 *p = EntryB(i);
				p->id = n->e;
				p->life = 0;
				*(uint32_t *)&p->a[0] = *(const uint32_t *)&n->w10;
				*(uint32_t *)&p->a[2] = *(const uint32_t *)&n->w14;
				const int16_t s = (int16_t)((CrtRand() % 0x800 + 0x1000) / 18);
				p->a[3] = s;
				p->scale = s;
				p->a[4] = (int16_t)(CrtRand() % 0x1000);
				int16_t r = (int16_t)(CrtRand() % 0x20 + 0x10);
				p->a[5] = r;
				if (r & 1) p->a[5] = (int16_t)-r;
				p->a[6] = (int16_t)(CrtRand() % 0x1000);
				r = (int16_t)(CrtRand() % 0x20 + 0x10);
				p->a[7] = r;
				if (r & 1) p->a[7] = (int16_t)-r;
			}
		}
		uint8_t *h = AllocHeader(0x58);
		int32_t count = 0;
		*(uint32_t *)(h + 8) = 0;
		w->m1 = Camera();
		*(uint32_t *)h = game_addr(MODEL_TIPROCK);
		w->ang[1] = 0;
		const int32_t id = n->e;
		for (uint32_t a = game_addr(POOL_B); a < game_addr(POOL_B_END); a += 0x20)
		{
			PoolE2 *p = (PoolE2 *)a;
			if (p->id != id) continue;
			TipRockDraw(w, h, p, p->scale, p->a[4], p->a[6], p->life, shl32(p->life - 0x10, 9));
			if (DrawOnly()) continue;
			if (p->life >= 0x18) p->id = 0;
			else
			{
				p->life++;
				p->scale = (int16_t)(p->scale + p->a[3]);
				const int16_t r = p->a[3];
				p->a[3] = (int16_t)(r - r / 18);
				p->a[4] = (int16_t)(p->a[4] + p->a[5]);
				p->a[6] = (int16_t)(p->a[6] + p->a[7]);
				count++;
			}
		}
		FieldFree(0x58);
		FieldFree(0x68);
		if (DrawOnly()) return 0;
		n->c++;
		return EndIfNone(count);
	}

	static void FinalShardDraw(ModelWork *w, uint8_t *h, const PoolE2 *p, const int16_t *pos, int32_t life, int32_t fade)
	{
		w->v[0] = p->a[8];
		w->v[1] = p->a[9];
		w->v[2] = p->a[10];
		const int32_t angle = RotationBetweenVectors((const int32_t *)game_addr(REF_UP), w->v, w->w);
		BuildAxisAngleRotationMatrix(angle, &w->m2, w->w);
		w->m2.t[0] = pos[0];
		w->m2.t[1] = pos[1];
		w->m2.t[2] = pos[2];
		w->x[1] = p->scale;
		Scale3DMatrix(&w->m2, w->x);
		ComposeAffineTransform(&w->m1, &w->m2, &w->m2);
		GteSetRotMatrix(&w->m2);
		GteSetTransVector(&w->m2);
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (life >= 6)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		RenderToArena(h);
	}

	static void FinalShardFrame(ModelWork *w, const Node24 *n, const Mat4x3 *view)
	{
		w->ang[0] = 0;
		w->ang[1] = 0;
		w->ang[2] = 0;
		ComposeZYXRotationMatrix(w->ang, &w->m1);
		w->m1.t[0] = n->w10;
		w->m1.t[1] = n->w12;
		w->m1.t[2] = n->w14;
		ComposeAffineTransform(view, &w->m1, &w->m1);
		w->x[0] = 0x2000;
		w->x[2] = 0;
	}

	static uint32_t __cdecl FinalShardsTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x78);
		if (!DrawOnly() && n->c <= 4)
		{
			for (int32_t j = 0; j < 8; j++)
			{
				const int32_t i = FreeB();
				if (i < 0) break;
				PoolE2 *p = EntryB(i);
				p->id = n->e;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x1800 + 0x1C00);
				w->v[0] = CrtRand() % 0x2000 - 0x1000;
				w->v[1] = CrtRand() % 0x2000 - 0x1000;
				w->v[2] = CrtRand() % 0x2000 - 0x1000;
				Normalize(w->v, w->v);
				const int32_t s = CrtRand() % 0x320 + 0x384;
				p->a[0] = (int16_t)(mul32(w->v[0], s) >> 12);
				p->a[1] = (int16_t)(mul32(w->v[1], s) >> 12);
				p->a[2] = (int16_t)(mul32(w->v[2], s) >> 12);
				const int32_t s2 = CrtRand() % 0x1C2 + 0xFA;
				p->a[4] = (int16_t)(mul32(w->v[0], s2) >> 12);
				p->a[5] = (int16_t)(mul32(w->v[1], s2) >> 12);
				p->a[6] = (int16_t)(mul32(w->v[2], s2) >> 12);
				p->a[8] = (int16_t)w->v[0];
				p->a[9] = (int16_t)w->v[1];
				p->a[10] = (int16_t)w->v[2];
			}
		}
		uint8_t *h = AllocHeader(0x58);
		*(uint32_t *)h = game_addr(MODEL_SHARD);
		*(uint32_t *)(h + 8) = 0;
		int32_t count = 0;
		FinalShardFrame(w, n, &Camera());
		const int32_t id = n->e;
		for (uint32_t a = game_addr(POOL_B); a < game_addr(FINAL_SHARDS_END); a += 0x20)
		{
			PoolE2 *p = (PoolE2 *)a;
			if (p->id != id) continue;
			FinalShardDraw(w, h, p, &p->a[0], p->life, mul32(p->life - 6, 682));
			if (DrawOnly()) continue;
			if (p->life >= 0xC) p->id = 0;
			else
			{
				p->life++;
				ShardMove(p, 3);
				count++;
			}
		}
		FieldFree(0x58);
		FieldFree(0x78);
		if (DrawOnly()) return 0;
		n->c++;
		return EndIfNone(count);
	}

	static void SpriteLight(const Mat4x3 *m)
	{
		GteSetLightMatrix(m);
		GteSetBackColorFromTrans(m);
	}

	static inline int32_t *Mac(ModelWork *w) { return (int32_t *)((uint8_t *)w + 0x38); }
	static inline int32_t *Dir(ModelWork *w) { return (int32_t *)((uint8_t *)w + 0x28); }

	static void SmokeSprite(uint8_t *h, int32_t *mac, int32_t *dir, const int16_t *pos, int16_t scale, int16_t life)
	{
		GteLoadV0(pos);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(scale);
		*(int16_t *)(h + 4) = life;
		GteReadMAC123(mac);
		Normalize(mac, dir);
		const int32_t k = -(scale >> 3);
		mac[0] += mul32(dir[0], k) >> 12;
		mac[1] += mul32(dir[1], k) >> 12;
		mac[2] += mul32(dir[2], k) >> 12;
		GteSetTransFromVec32(mac);
		PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
	}

	static void PlainSprite(uint8_t *h, const int16_t *pos, int16_t scale, int16_t frame)
	{
		GteLoadV0(pos);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(scale);
		*(int16_t *)(h + 4) = frame;
		GteTransFromMAC();
	}

	static uint8_t *SequenceHeader(uint32_t seq, int16_t mode)
	{
		uint8_t *h = AllocHeader(0xB4);
		*(int16_t *)(h + 0x24) = mode;
		*(uint32_t *)h = seq;
		return h;
	}

	static void BoneSpark(PoolE *p, int16_t id, int32_t bone)
	{
		p->id = id;
		p->life = 0;
		p->scale = (int16_t)(CrtRand() % 0x240 + 0x380);
		GetEffectSpawnPosition(Caster(), bone, 0x1000, &p->a[0]);
		p->a[0] = (int16_t)(p->a[0] + (int16_t)(CrtRand() % 0xA0 - 0x50));
		p->a[1] = (int16_t)(p->a[1] + (int16_t)(CrtRand() % 0xA0 - 0x50));
		p->a[2] = (int16_t)(p->a[2] + (int16_t)(CrtRand() % 0xA0 - 0x50));
	}

	static uint32_t __cdecl BoneSparksTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x48);
		if (!DrawOnly() && n->c <= 0x30 && n->c % 6 == 1)
		{
			int32_t i = FreeA();
			if (i >= 0)
			{
				BoneSpark(EntryA(i), n->e, 0x13);
				for (i++; i < 0xC8; i++)
					if (EntryA(i)->id == 0) break;
				if (i < 0xC8) BoneSpark(EntryA(i), n->e, 0x14);
			}
		}
		uint8_t *h = SequenceHeader(game_addr(SEQ_SPARK), 0);
		w->m1 = Camera();
		SpriteLight(&w->m1);
		const int32_t id = n->e;
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (p->id != id) continue;
			SmokeSprite(h, Mac(w), Dir(w), &p->a[0], p->scale, p->life);
			if (DrawOnly()) continue;
			if (*(const int16_t *)(h + 0x28) < 0) p->id = 0;
			else p->life++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		n->c++;
		if (n->c < 0x3A) return 0;
		ClearIdA(id);
		return TASK_END;
	}

	static uint32_t __cdecl BladeSparksTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x50);
		if (!DrawOnly() && n->c <= 0xD)
		{
			const int32_t i = FreeA();
			if (i >= 0)
			{
				int16_t *q = w->ang;
				GetEffectSpawnPosition(Caster(), 0x13, 0xE00, q);
				GetEffectSpawnPosition(Caster(), 0x13, 0xE00, q + 4);
				if (q[1] > q[5])
				{
					*(uint32_t *)&q[0] = *(const uint32_t *)&q[4];
					*(uint32_t *)&q[2] = *(const uint32_t *)&q[6];
				}
				if (q[1] > -0x32)
				{
					PoolE *p = EntryA(i);
					p->id = n->e;
					p->life = 0;
					p->scale = (int16_t)(CrtRand() % 0x240 + 0x380);
					*(uint32_t *)&p->a[0] = *(const uint32_t *)&q[0];
					*(uint32_t *)&p->a[2] = *(const uint32_t *)&q[2];
					p->a[0] = (int16_t)(p->a[0] + (int16_t)(CrtRand() % 0xA0 - 0x50));
					p->a[1] = (int16_t)(p->a[1] + (int16_t)(CrtRand() % 0xA0 - 0x50));
					p->a[2] = (int16_t)(p->a[2] + (int16_t)(CrtRand() % 0xA0 - 0x50));
				}
			}
		}
		uint8_t *h = SequenceHeader(game_addr(SEQ_SPARK), 0);
		Mat4x3 *cam = (Mat4x3 *)((uint8_t *)w + 0x10);
		*cam = Camera();
		SpriteLight(cam);
		const int32_t id = n->e;
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (p->id != id) continue;
			SmokeSprite(h, (int32_t *)((uint8_t *)w + 0x40), (int32_t *)((uint8_t *)w + 0x30), &p->a[0], p->scale, p->life);
			if (DrawOnly()) continue;
			if (*(const int16_t *)(h + 0x28) < 0) p->id = 0;
			else p->life++;
		}
		FieldFree(0xB4);
		FieldFree(0x50);
		if (DrawOnly()) return 0;
		n->c++;
		return n->c > 0xD ? TASK_END : 0;
	}

	static void TipSmokeSprite(ModelWork *w, uint8_t *h, int16_t r, int16_t y, int16_t ang, int16_t scale, int16_t life)
	{
		int16_t *q = (int16_t *)((uint8_t *)w + 8);
		q[0] = (int16_t)(mul32(ComputeCos(ang), r) >> 12);
		q[1] = y;
		q[2] = (int16_t)(mul32(ComputeSin(ang), r) >> 12);
		PlainSprite(h, q, scale, (int16_t)(life >> 1));
		FrameArena() = InitEffectSequenceFromData(h, RenderOT(), 2, FrameArena());
	}

	static void TipSmokeFrame(ModelWork *w, const Node24 *n, const Mat4x3 *view)
	{
		Mat4x3 *m = (Mat4x3 *)((uint8_t *)w + 0x10);
		w->ang[0] = 0;
		w->ang[1] = -0x80;
		w->ang[2] = 0x200;
		ComposeZYXRotationMatrix(w->ang, m);
		m->t[0] = n->w10;
		m->t[1] = n->w12;
		m->t[2] = n->w14;
		ComposeAffineTransform(view, m, m);
		SpriteLight(m);
	}

	static uint32_t __cdecl TipSmokeTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x50);
		if (!DrawOnly() && n->c <= 0x32)
		{
			for (int32_t j = 0; j < 3; j++)
			{
				const int32_t i = FreeA();
				if (i < 0) break;
				PoolE *p = EntryA(i);
				p->id = n->e;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x180 + 0x200);
				const int32_t a = CrtRand() % 0x1000;
				const int32_t r = CrtRand() % 0x258 + 0x2BC;
				p->a[0] = (int16_t)r;
				p->a[2] = (int16_t)a;
				p->a[1] = (int16_t)(CrtRand() % 0x4B0 - 0x258);
				p->a[4] = (int16_t)((r - 0x12C) / 16);
				p->a[6] = (int16_t)(CrtRand() % 0x5A + 0x28);
				const int32_t t = CrtRand() % 10 + 10;
				p->a[5] = (int16_t)(p->a[1] / t);
			}
		}
		uint8_t *h = SequenceHeader(game_addr(SEQ_SMOKE), 8);
		TipSmokeFrame(w, n, &Camera());
		const int32_t id = n->e;
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (p->id != id) continue;
			TipSmokeSprite(w, h, p->a[0], p->a[1], p->a[2], p->scale, p->life);
			if (DrawOnly()) continue;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				p->id = 0;
				continue;
			}
			p->life++;
			if (p->life <= 0x10) p->a[0] = (int16_t)(p->a[0] - p->a[4]);
			p->a[2] = (int16_t)(p->a[2] + p->a[6]);
			const int16_t v = p->a[6];
			p->a[6] = (int16_t)(v + (int16_t)(v >> 3));
			p->a[1] = (int16_t)(p->a[1] - p->a[5]);
		}
		FieldFree(0xB4);
		FieldFree(0x50);
		if (DrawOnly()) return 0;
		n->c++;
		if (n->c < 0x2C) return 0;
		ClearIdA(id);
		return TASK_END;
	}

	static uint32_t SparkAPass(Node24 *n, ModelWork *w, uint8_t *h, uint32_t fn)
	{
		const int32_t id = n->e;
		int32_t count = 0;
		(void)fn;
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (p->id != id) continue;
			SmokeSprite(h, Mac(w), Dir(w), &p->a[0], p->scale, p->life);
			if (DrawOnly()) continue;
			if (*(const int16_t *)(h + 0x28) < 0) p->id = 0;
			else
			{
				p->life++;
				count++;
			}
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		n->c++;
		return EndIfNone(count);
	}

	static uint32_t __cdecl HitSparkATask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x48);
		if (!DrawOnly() && n->c <= 0)
		{
			*(uint32_t *)&w->ang[0] = *(const uint32_t *)&n->w10;
			*(uint32_t *)&w->ang[2] = *(const uint32_t *)&n->w14;
			for (int32_t j = 0; j < 3; j++)
			{
				const int32_t i = FreeA();
				if (i < 0) break;
				PoolE *p = EntryA(i);
				p->id = n->e;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0xA00 + 0x600);
				*(uint32_t *)&p->a[0] = *(const uint32_t *)&w->ang[0];
				*(uint32_t *)&p->a[2] = *(const uint32_t *)&w->ang[2];
				p->a[0] = (int16_t)(p->a[0] + (int16_t)(CrtRand() % 0x258 - 0x12C));
				p->a[1] = (int16_t)(p->a[1] + (int16_t)(CrtRand() % 0x258 - 0x12C));
				p->a[2] = (int16_t)(p->a[2] + (int16_t)(CrtRand() % 0x258 - 0x12C));
			}
		}
		uint8_t *h = SequenceHeader(game_addr(SEQ_HITA), 8);
		w->m1 = Camera();
		SpriteLight(&w->m1);
		return SparkAPass(n, w, h, game_addr(ORIG_HitSparkA));
	}

	static uint32_t __cdecl FinalSparkATask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x48);
		if (!DrawOnly() && n->c <= 4)
		{
			*(uint32_t *)&w->ang[0] = *(const uint32_t *)&n->w10;
			*(uint32_t *)&w->ang[2] = *(const uint32_t *)&n->w14;
			for (int32_t j = 0; j < 6; j++)
			{
				const int32_t i = FreeA();
				if (i < 0) break;
				PoolE *p = EntryA(i);
				p->id = n->e;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x1000 + 0xE00);
				*(uint32_t *)&p->a[0] = *(const uint32_t *)&w->ang[0];
				*(uint32_t *)&p->a[2] = *(const uint32_t *)&w->ang[2];
				p->a[0] = (int16_t)(p->a[0] + (int16_t)(CrtRand() % 0x7D0 - 0x3E8));
				p->a[1] = (int16_t)(p->a[1] + (int16_t)(CrtRand() % 0x7D0 - 0x3E8));
				p->a[2] = (int16_t)(p->a[2] + (int16_t)(CrtRand() % 0x7D0 - 0x3E8));
			}
		}
		uint8_t *h = SequenceHeader(game_addr(SEQ_HITA), 8);
		w->m1 = Camera();
		SpriteLight(&w->m1);
		return SparkAPass(n, w, h, game_addr(ORIG_FinalSparkA));
	}

	static uint32_t __cdecl HitSparkBTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x48);
		if (!DrawOnly() && n->c <= 0)
		{
			*(uint32_t *)&w->ang[0] = *(const uint32_t *)&n->w10;
			*(uint32_t *)&w->ang[2] = *(const uint32_t *)&n->w14;
			int32_t *v = (int32_t *)&w->m2;
			for (int32_t j = 0; j < 0xC; j++)
			{
				const int32_t i = FreeA();
				if (i < 0) break;
				PoolE *p = EntryA(i);
				p->id = n->e;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x680 + 0x380);
				*(uint32_t *)&p->a[0] = *(const uint32_t *)&w->ang[0];
				*(uint32_t *)&p->a[2] = *(const uint32_t *)&w->ang[2];
				v[0] = CrtRand() % 0x2000 - 0x1000;
				v[1] = CrtRand() % 0x2000 - 0x1000;
				v[2] = CrtRand() % 0x2000 - 0x1000;
				Normalize(v, v);
				const int32_t s = CrtRand() % 0x12C + 0x96;
				p->a[0] = (int16_t)(p->a[0] + (int16_t)(mul32(v[0], s) >> 12));
				p->a[1] = (int16_t)(p->a[1] + (int16_t)(mul32(v[1], s) >> 12));
				p->a[2] = (int16_t)(p->a[2] + (int16_t)(mul32(v[2], s) >> 12));
				const int32_t s2 = CrtRand() % 0x15E + 0x96;
				p->a[4] = (int16_t)(mul32(v[0], s2) >> 12);
				p->a[5] = (int16_t)(mul32(v[1], s2) >> 12);
				p->a[6] = (int16_t)(mul32(v[2], s2) >> 12);
			}
		}
		uint8_t *h = SequenceHeader(game_addr(SEQ_HITB), 8);
		w->m1 = Camera();
		int32_t count = 0;
		SpriteLight(&w->m1);
		const int32_t id = n->e;
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (p->id != id) continue;
			SmokeSprite(h, Mac(w), Dir(w), &p->a[0], p->scale, p->life);
			if (DrawOnly()) continue;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				p->id = 0;
				continue;
			}
			p->life++;
			for (int k = 0; k < 3; k++) p->a[k] = (int16_t)(p->a[k] + p->a[4 + k]);
			for (int k = 0; k < 3; k++)
			{
				const int16_t v = p->a[4 + k];
				p->a[4 + k] = (int16_t)(v - (int16_t)(v >> 3));
			}
			count++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		n->c++;
		return EndIfNone(count);
	}

	static uint32_t __cdecl TargetSparksTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ModelWork *w = (ModelWork *)FieldAlloc(0x48);
		if (!DrawOnly() && n->c <= 3)
		{
			for (int32_t j = 0; j < 8; j++)
			{
				const int32_t i = FreeA();
				if (i < 0) break;
				PoolE *p = EntryA(i);
				p->id = n->e;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0xA00 + 0x800);
				const int32_t a = CrtRand() % 0x1000;
				int32_t *sc = (int32_t *)&w->m2;
				sc[0] = ComputeSin(a);
				sc[2] = ComputeCos(a);
				const int32_t r = CrtRand() % 0xC8 + 0x190;
				p->a[0] = (int16_t)(mul32(sc[0], r) >> 12);
				const int32_t y = CrtRand() % 0x32;
				p->a[1] = (int16_t)-y;
				p->a[2] = (int16_t)(mul32(r, sc[2]) >> 12);
				const int32_t s2 = CrtRand() % 0x118 + 0x78;
				p->a[4] = (int16_t)(mul32(sc[0], s2) >> 12);
				p->a[6] = (int16_t)(mul32(s2, sc[2]) >> 12);
			}
		}
		uint8_t *h = SequenceHeader(game_addr(SEQ_SPARK), 8);
		int32_t count = 0;
		w->ang[0] = 0;
		w->ang[1] = 0;
		w->ang[2] = 0;
		ComposeZYXRotationMatrix(w->ang, &w->m1);
		w->m1.t[0] = n->w10;
		w->m1.t[1] = 0;
		w->m1.t[2] = n->w14;
		ComposeAffineTransform(&Camera(), &w->m1, &w->m1);
		SpriteLight(&w->m1);
		const int32_t id = n->e;
		for (uint32_t a = game_addr(POOL_A); a < game_addr(POOL_A_END); a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (p->id != id) continue;
			SmokeSprite(h, Mac(w), Dir(w), &p->a[0], p->scale, p->life);
			if (DrawOnly()) continue;
			if (*(const int16_t *)(h + 0x28) < 0)
			{
				p->id = 0;
				continue;
			}
			p->life++;
			p->a[0] = (int16_t)(p->a[0] + p->a[4]);
			p->a[2] = (int16_t)(p->a[2] + p->a[6]);
			int16_t v = p->a[4];
			p->a[4] = (int16_t)(v - (int16_t)(v >> 3));
			v = p->a[6];
			p->a[6] = (int16_t)(v - (int16_t)(v >> 3));
			count++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		n->c++;
		return EndIfNone(count);
	}

	static uint32_t __cdecl FinalRingSparksTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		const uint32_t base = PoolCBase();
		ModelWork *w = (ModelWork *)FieldAlloc(0x48);
		if (!DrawOnly() && n->c <= 0x18)
		{
			int32_t *sc = (int32_t *)&w->m2;
			for (int32_t j = 0; j < 0x14; j++)
			{
				int32_t i = 0;
				uint32_t a = base;
				for (; *(const int32_t *)a != 0; a += 0x18)
					if (++i >= 0x1F4) break;
				if (i >= 0x1F4) break;
				PoolE *p = (PoolE *)(base + 0x18 * i);
				p->id = n->e;
				p->life = 0;
				p->scale = (int16_t)(CrtRand() % 0x400 + 0x200);
				const int32_t ang = CrtRand() % 0x1000;
				sc[0] = ComputeSin(ang);
				sc[2] = ComputeCos(ang);
				const int32_t r = var<int16_t>(game_addr(TIP) + 6) - CrtRand() % 0x64;
				p->a[0] = (int16_t)(mul32(sc[0], r) >> 12);
				const int32_t y = CrtRand() % 0xC8;
				p->a[2] = (int16_t)(mul32(r, sc[2]) >> 12);
				p->a[1] = (int16_t)(y - 0x64);
			}
		}
		uint8_t *h = SequenceHeader(game_addr(SEQ_HITB), 8);
		int32_t count = 0;
		w->ang[0] = 0x100;
		w->ang[1] = 0;
		w->ang[2] = 0x100;
		ComposeZYXRotationMatrix(w->ang, &w->m1);
		w->m1.t[0] = n->w10;
		w->m1.t[1] = n->w12;
		w->m1.t[2] = n->w14;
		TransformVectorBy3x3Matrix(&Camera(), w->m1.t, w->m1.t);
		w->m1.t[0] += Camera().t[0];
		w->m1.t[1] += Camera().t[1];
		w->m1.t[2] += Camera().t[2];
		SpriteLight(&w->m1);
		const int32_t id = n->e;
		for (uint32_t a = base, k = 0; k < 0x1F4; k++, a += 0x18)
		{
			PoolE *p = (PoolE *)a;
			if (p->id != id) continue;
			PlainSprite(h, &p->a[0], p->scale, p->life);
			PacketCursor() = InitEffectSequenceFromData(h, RenderOT(), 2, PacketCursor());
			if (DrawOnly()) continue;
			if (*(const int16_t *)(h + 0x28) < 0) p->id = 0;
			else
			{
				p->life++;
				count++;
			}
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (DrawOnly()) return 0;
		n->c++;
		return EndIfNone(count);
	}

	static bool PredecessorIsTrail(const TaskNode *self)
	{
		const TaskNode *prev = nullptr;
		for (const TaskNode *t = QMain()->head; t && t != self; t = t->next) prev = t;
		return prev && (uint32_t)prev->func == game_addr(ORIG_Trail);
	}

	static uint32_t HitDirectorBody(Node24 *n, uint32_t slot4)
	{
		for (int32_t i = 0; i < 0xF; i++)
		{
			const int32_t t = var<int32_t>(game_addr(HIT_TIMES) + 4 * i);
			if (n->c > t) continue;
			if (n->c < t) break;
			int16_t v[4];
			v[3] = (int16_t)(slot4 >> 16);
			MatVec((const Mat4x3 *)(Caster() + 0x40), (const int16_t *)(game_addr(HIT_OFFSETS) + 8 * i), v);
			v[0] = (int16_t)(v[0] + var<int16_t>(game_addr(TIP0)));
			v[1] = (int16_t)(v[1] + var<int16_t>(game_addr(TIP0) + 2));
			v[2] = (int16_t)(v[2] + var<int16_t>(game_addr(TIP0) + 4));
			if (i > 3)
			{
				v[0] = (int16_t)(((int32_t)var<int16_t>(game_addr(PREV_HIT)) + v[0]) >> 1);
				v[1] = (int16_t)(((int32_t)var<int16_t>(game_addr(PREV_HIT) + 2) + v[1]) >> 1);
				v[2] = (int16_t)(((int32_t)var<int16_t>(game_addr(PREV_HIT) + 4) + v[2]) >> 1);
			}
			const uint32_t p0 = *(const uint32_t *)&v[0], p1 = *(const uint32_t *)&v[2];
			var<uint32_t>(game_addr(PREV_HIT)) = p0;
			HitRec *r = Hit(i);
			*(uint32_t *)&r->pos[0] = p0;
			*(uint32_t *)&r->pos[2] = p1;
			var<uint32_t>(game_addr(PREV_HIT) + 4) = p1;
			for (int k = 0; k < 4; k++)
			{
				const uint32_t d = var<uint32_t>(game_addr(TRAIL_DIR) + 4 * k);
				var<uint32_t>(game_addr(LAST_HIT) + 4 * k) = d;
				((uint32_t *)r->dir)[k] = d;
			}
			Node24 *t1 = AddMain(game_addr(ORIG_HitSparkA));
			t1->c = 0;
			t1->e = TakeId();
			*(uint32_t *)&t1->w10 = p0;
			*(uint32_t *)&t1->w14 = p1;
			Node24 *t2 = AddMain(game_addr(ORIG_HitSparkB));
			t2->c = 0;
			t2->e = TakeId();
			*(uint32_t *)&t2->w10 = p0;
			*(uint32_t *)&t2->w14 = p1;
			Node24 *t3 = AddMain(game_addr(ORIG_HitSlashA));
			t3->c = 0;
			t3->e = (int16_t)i;
			t3->w18 = (int16_t)(CrtRand() % 0x1000);
			const int16_t s3 = (int16_t)((CrtRand() % 0xA00 + 0xE00) / 3);
			t3->w1E = s3;
			t3->w1C = s3;
			Node24 *t4 = AddMain(game_addr(ORIG_HitSlashB));
			t4->c = 0;
			t4->e = (int16_t)i;
			t4->w18 = (int16_t)(CrtRand() % 0x1000);
			const int16_t s4 = (int16_t)((CrtRand() % 0xC00 + 0xC00) / 3);
			t4->w1E = s4;
			t4->w1C = (int16_t)(s4 + 0x580);
			Node24 *t5 = AddMain(game_addr(ORIG_HitShards));
			t5->e = TakeId();
			t5->c = 0;
			t5->w1A = (int16_t)i;
			if (i < 0xE)
			{
				const int32_t ts = TargetSlot();
				uint8_t *tg = Entity(ts);
				tg[1] |= 0x10;
				Node24 *b = AddMain(game_addr(ORIG_TargetBounce));
				b->c = 0;
				b->e = (int16_t)ts;
				b->w1A = (int16_t)(i != 0 ? var<int32_t>(game_addr(HIT_TIMES) + 4 * (i + 1)) - var<int32_t>(game_addr(HIT_TIMES) + 4 * i) : 8);
				b->w1C = (int16_t)(var<int16_t>(game_addr(HIT_HEIGHTS) + 4 * i) - 0xFA0);
				uint8_t *e = *(uint8_t **)(tg + 0x8C);
				if (e && e != tg)
				{
					do
					{
						if (*e & 2)
						{
							const int32_t k = EntityIndex(e);
							if (k < 7)
							{
								Entity(k)[1] |= 0x10;
								Node24 *b2 = AddMain(game_addr(ORIG_TargetBounce));
								b2->w1A = b->w1A;
								b2->w1C = b->w1C;
								b2->c = 0;
								b2->e = (int16_t)k;
							}
						}
						e = *(uint8_t **)(e + 0x8C);
					} while (e != tg);
				}
			}
			const int32_t k = ResultIndex();
			ResultIndex() = k + 1;
			ApplyActionResultToTarget(Context()->actions->targets + TARGET_STRIDE * k);
		}
		n->c++;
		return EndAt(n->c, 0x6E);
	}

	static uint32_t __cdecl HitDirectorTask(TaskNode *tn)
	{
		if (DrawOnly()) return 0;
		const bool modelled = g_trail_slot4_known && PredecessorIsTrail(tn);
		return HitDirectorBody((Node24 *)tn, modelled ? g_trail_slot4 : 0x023C5910);
	}

	static int32_t BounceOffset(const Node24 *n, int32_t c)
	{
		if (n->w22 > 0) return -70 * c;
		return mul32(ComputeSin(shl32(c, 10) / n->w1A), n->w22) >> 12;
	}

	static uint32_t __cdecl TargetBounceTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		const int32_t slot = n->e;
		if (DrawOnly()) return 0;
		uint8_t *ent = Entity(slot);
		if (n->c == 0)
		{
			GetDefaultEffectPosition(ent, &n->w10);
			n->w20 = *(const int16_t *)(ent + 0x1E);
			n->w22 = (int16_t)(n->w1C - n->w12);
		}
		const int32_t d = BounceOffset(n, n->c);
		*(int16_t *)(ent + 0x1E) = (int16_t)(n->w20 + d);
		n->c++;
		return n->c >= n->w1A ? TASK_END : 0;
	}

	static void SinkPlane(ClipHeader *h)
	{
		int16_t *p = Slot(h, 0);
		p[0] = 0;
		p[1] = 0;
		p[2] = 0;
		h->plane[0] = 0;
		h->plane[1] = (int16_t)0xF000;
		h->plane[2] = 0;
		PlaneDistance(p, h->plane);
	}

	static uint32_t __cdecl TargetSinkTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		const int32_t slot = n->e;
		uint8_t *ent = Entity(slot);
		if (n->c == 0)
		{
			GetDefaultEffectPosition(ent, &n->w10);
			const uint32_t r1 = *(const uint32_t *)(ent + 0x10);
			n->w20 = n->w12;
			n->w22 = (int16_t)(n->w12 / 8);
			Keep(slot)->rot[0] = *(const uint32_t *)(ent + 0xC);
			Keep(slot)->rot[1] = r1;
		}
		if (n->c < 0x10)
		{
			ClipHeader *h = (ClipHeader *)FieldAlloc(0x1A8);
			SinkPlane(h);
			*ent |= 4;
			DrawClipped(h, ent);
			FieldFree(0x1A8);
		}
		const int16_t c = n->c;
		if (c <= 8)
		{
			const int16_t y = (int16_t)(n->w20 + var<int16_t>(game_addr(TARGET_ANCHOR) + 2));
			*(int16_t *)(ent + 0x10) = 0x800;
			*(int16_t *)(ent + 0x1E) = y;
		}
		if (DrawOnly()) return 0;
		if (c <= 8) n->w20 = (int16_t)(n->w20 - n->w22);
		if (c == 9)
		{
			Node24 *t = AddMain(game_addr(ORIG_Shake));
			t->c = 0;
			t = AddMain(game_addr(ORIG_TargetSparks));
			t->e = TakeId();
			*(uint32_t *)&t->w10 = var<uint32_t>(game_addr(TARGET_KEPT));
			t->c = 0;
			*(uint32_t *)&t->w14 = var<uint32_t>(game_addr(TARGET_KEPT) + 4);
			t = AddMain(game_addr(ORIG_TargetRing));
			*(uint32_t *)&t->w10 = var<uint32_t>(game_addr(TARGET_KEPT));
			t->c = 0;
			*(uint32_t *)&t->w14 = var<uint32_t>(game_addr(TARGET_KEPT) + 4);
			const int16_t s = (int16_t)((CrtRand() % 0x800 + 0x2000) / 4);
			t->w1E = s;
			t->w1C = s;
			*(uint16_t *)ent = (uint16_t)(Keep(slot)->flag20 | (*(const uint16_t *)ent & 0xFFDF));
		}
		n->c = (int16_t)(c + 1);
		if (n->c < 0x10) return 0;
		if (n->c == 0x10)
		{
			*(uint32_t *)(ent + 0x1C) = Keep(slot)->pos[0];
			*(uint32_t *)(ent + 0x20) = Keep(slot)->pos[1];
			*(uint32_t *)(ent + 0xC) = Keep(slot)->rot[0];
			*(uint32_t *)(ent + 0x10) = Keep(slot)->rot[1];
			*(uint16_t *)ent &= 0xEFFF;
			if (slot == TargetSlot())
			{
				const int32_t k = ResultIndex();
				ResultIndex() = k + 1;
				ApplyActionResultToTarget(Context()->actions->targets + TARGET_STRIDE * k);
			}
			return 0;
		}
		*(uint16_t *)ent &= 0xFFFB;
		return TASK_END;
	}

	static uint32_t __cdecl FlashTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		SetScreenFlash((uint32_t)(mul32(n->w18 / n->e, n->c) + 0x800), 0);
		if (DrawOnly()) return 0;
		n->c++;
		if (n->c < n->e) return 0;
		if (n->w1A != 0) StageGroupsShow();
		return TASK_END;
	}

	static uint32_t __cdecl ShakeTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int32_t v = mul32(ComputeSin(shl32(n->c, 10) / 12 + 0x400), 200) >> 12;
		if (!(n->c & 1)) v = -v;
		var<int16_t>(game_addr(game::BATTLE_CAMERA_SHAKE_OFFSET_Y)) = (int16_t)v;
		if (DrawOnly()) return 0;
		n->c++;
		return EndAt(n->c, 0xC);
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
		if (r->c & 1) PacketCursor() = PacketArena() + 0x8000;
		else PacketCursor() = PacketArena();
		const int alive = ExecuteTaskQueue(QMain());
		r->c++;
		return alive ? 0 : TASK_END;
	}
}

	void register_mag165_lion_heart()
	{
		using namespace lion165;
		register_port(ORIG_Root, (void *)RootTask, "165 Root", 165);
		register_port(ORIG_Director, (void *)DirectorTask, "165 Director", 165);
		register_port(ORIG_Flash, (void *)FlashTask, "165 Flash", 165);
		register_port(ORIG_Trail, (void *)TrailTask, "165 Trail", 165);
		register_port(ORIG_GroundRing, (void *)GroundRingTask, "165 GroundRing", 165);
		register_port(ORIG_RockBurst, (void *)RockBurstTask, "165 RockBurst", 165);
		register_port(ORIG_Debris, (void *)DebrisTask, "165 Debris", 165);
		register_port(ORIG_BoneSparks, (void *)BoneSparksTask, "165 BoneSparks", 165);
		register_port(ORIG_BladeSparks, (void *)BladeSparksTask, "165 BladeSparks", 165);
		register_port(ORIG_TipFlash, (void *)TipFlashTask, "165 TipFlash", 165);
		register_port(ORIG_TipShards, (void *)TipShardsTask, "165 TipShards", 165);
		register_port(ORIG_TipSmoke, (void *)TipSmokeTask, "165 TipSmoke", 165);
		register_port(ORIG_TipRing, (void *)TipRingTask, "165 TipRing", 165);
		register_port(ORIG_HitDirector, (void *)HitDirectorTask, "165 HitDirector", 165);
		register_port(ORIG_HitSparkA, (void *)HitSparkATask, "165 HitSparkA", 165);
		register_port(ORIG_HitSparkB, (void *)HitSparkBTask, "165 HitSparkB", 165);
		register_port(ORIG_HitSlashA, (void *)HitSlashATask, "165 HitSlashA", 165);
		register_port(ORIG_HitSlashB, (void *)HitSlashBTask, "165 HitSlashB", 165);
		register_port(ORIG_HitShards, (void *)HitShardsTask, "165 HitShards", 165);
		register_port(ORIG_TargetBounce, (void *)TargetBounceTask, "165 TargetBounce", 165);
		register_port(ORIG_FlatRing, (void *)FlatRingTask, "165 FlatRing", 165);
		register_port(ORIG_WaveSpawner, (void *)WaveSpawnerTask, "165 WaveSpawner", 165);
		register_port(ORIG_ScreenWave, (void *)ScreenWaveTask, "165 ScreenWave", 165);
		register_port(ORIG_TipFlash2, (void *)TipFlash2Task, "165 TipFlash2", 165);
		register_port(ORIG_TipRocks, (void *)TipRocksTask, "165 TipRocks", 165);
		register_port(ORIG_Streak, (void *)StreakTask, "165 Streak", 165);
		register_port(ORIG_FinalRing, (void *)FinalRingTask, "165 FinalRing", 165);
		register_port(ORIG_FinalSparkA, (void *)FinalSparkATask, "165 FinalSparkA", 165);
		register_port(ORIG_FinalShards, (void *)FinalShardsTask, "165 FinalShards", 165);
		register_port(ORIG_FinalFlash, (void *)FinalFlashTask, "165 FinalFlash", 165);
		register_port(ORIG_FinalRingSparks, (void *)FinalRingSparksTask, "165 FinalRingSparks", 165);
		register_port(ORIG_TargetSink, (void *)TargetSinkTask, "165 TargetSink", 165);
		register_port(ORIG_TargetSparks, (void *)TargetSparksTask, "165 TargetSparks", 165);
		register_port(ORIG_TargetRing, (void *)TargetRingTask, "165 TargetRing", 165);
		register_port(ORIG_Shake, (void *)ShakeTask, "165 Shake", 165);
		register_port(ORIG_Restore, (void *)RestoreTask, "165 Restore", 165);
	}
}

