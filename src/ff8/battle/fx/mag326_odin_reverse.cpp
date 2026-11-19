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

// Effect 326: Odin - Zantetsuken Reverse

#include "fx_port.h"

namespace ff8fx
{
namespace o326
{
	using namespace eng;

	namespace x
	{
		inline int32_t CrtRand() { return fn<int32_t (__cdecl *)()>(game_addr(game::_rand))(); }
		inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
		inline void ComposeZYXRotationMatrix(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::ComposeZYXRotationMatrix))(angles, out); }
		inline void Scale3DMatrix(Mat4x3 *m, const int32_t *v) { fn<void (__cdecl *)(Mat4x3 *, const int32_t *)>(game_addr(game::scale3DMatrix))(m, v); }
		inline void GteMatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
		inline int32_t NormalizeVectorToFixedPoint(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
		inline void NormalizeVector16(const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const int16_t *, int16_t *)>(game_addr(game::VectorNormalS16))(in, out); }
		inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
		inline uint32_t MatrixMultiplyVector(const Mat4x3 *m, const int16_t *in, int16_t *out) { return fn<uint32_t (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
		inline void TransformVectorBy3x3Matrix(const Mat4x3 *m, const int32_t *in, int32_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int32_t *, int32_t *)>(game_addr(game::TransformVectorBy3x3Matrix))(m, in, out); }
		inline int32_t GetEffectSpawnPosition(void *entity, int32_t bone, int32_t frac, int16_t *out) { return fn<int32_t (__cdecl *)(void *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, frac, out); }
		inline int32_t GetDefaultEffectPosition(void *entity, int16_t *out) { return fn<int32_t (__cdecl *)(void *, int16_t *)>(game_addr(game::GetDefaultEffectPosition))(entity, out); }
		inline int32_t RotationBetweenVectors(uint32_t from, const int32_t *to, int32_t *axis) { return fn<int32_t (__cdecl *)(uint32_t, const int32_t *, int32_t *)>(game_addr(game::GetRotationBetweenVectors_AxisAngle))(from, to, axis); }
		inline void AxisAngleMatrix(int32_t angle, Mat4x3 *out, const int32_t *axis) { fn<void (__cdecl *)(int32_t, Mat4x3 *, const int32_t *)>(game_addr(game::BuildAxisAngleRotationMatrix))(angle, out, axis); }
		inline void SplineSetup(int32_t n, uint32_t points, void *work) { fn<void (__cdecl *)(int32_t, uint32_t, void *)>(game_addr(game::Effect_SplineSetup))(n, points, work); }
		inline void SplineEval(int32_t n, void *work, uint32_t out, int32_t t) { fn<void (__cdecl *)(int32_t, void *, uint32_t, int32_t)>(game_addr(game::Effect_SplineEval))(n, work, out, t); }
		inline int32_t PreReadAnimation(void *header, void *cmd, int32_t id) { return fn<int32_t (__cdecl *)(void *, void *, int32_t)>(game_addr(game::pre_Battle_ReadAnimation))(header, cmd, id); }
		inline int32_t ReadAnimation(void *header, void *cmd) { return fn<int32_t (__cdecl *)(void *, void *)>(game_addr(game::Battle_ReadAnimation))(header, cmd); }
		inline uint32_t RenderGeometry(void *geom, void *hdr, uint32_t ot, int32_t shift, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(geom, hdr, ot, shift, cursor); }
		inline uint32_t DrawShadow(void *entity, uint32_t ot, int32_t shift, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(entity, ot, shift, cursor); }
		inline void BdPlaySE(uint32_t se, int32_t a, int32_t b) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t)>(game_addr(game::BdPlaySE))(se, a, b); }
		inline void BdPlaySummonStream(int32_t a, int32_t b, int32_t c) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::BdPlaySummonStream))(a, b, c); }
		inline void BdTransSummonStream(uint32_t src, uint32_t state) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::BdTransSummonStream))(src, state); }
		inline int32_t VoiceSlotBusy(uint32_t slot) { return fn<int32_t (__cdecl *)(uint32_t)>(game_addr(game::BdSound_IsVoiceSlotActive))(slot); }
		inline void ReleaseVoiceSlot(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
		inline void CameraScriptStart(uint32_t script, const void *frame, void *entity, TaskQueue *q, int32_t n) { fn<void (__cdecl *)(uint32_t, const void *, void *, TaskQueue *, int32_t)>(game_addr(game::MAG066_CamScript_Start))(script, frame, entity, q, n); }
		inline void CameraShake(int32_t a, int32_t b, int32_t c, int32_t d) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, int32_t)>(game_addr(game::Effect_SpawnScreenFadeTask))(a, b, c, d); }
		inline void ApplyActionResultToTargets(uint32_t list, uint32_t count) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::ApplyActionResultToTargets))(list, count); }
		inline void QueueChainTransformation(void *entity, int32_t id) { fn<void (__cdecl *)(void *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, id); }
		inline void WaitAnimSeq(uint32_t buffer, void *flag) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::Battle_RestoreDefaultEffectTexture))(buffer, flag); }
		inline int32_t LoadState() { return fn<int32_t (__cdecl *)()>(game_addr(game::BattleFile_GetLoadingState))(); }
		inline void CharacterLoad(int32_t id, uint32_t dst) { fn<void (__cdecl *)(int32_t, uint32_t)>(game_addr(game::BattleFile_CharacterLoad))(id, dst); }
		inline void QueueTIMUpload(uint32_t tim) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::Battle_QueueTIMUpload_GetEOF))(tim); }
		inline void QueueVramUpload(uint32_t rect, const void *data) { fn<void (__cdecl *)(uint32_t, const void *)>(game_addr(game::Battle_QueueVramUpload_Type0_RectData))(rect, data); }
		inline uint32_t SummonData() { return fn<uint32_t (__cdecl *)()>(game_addr(game::sub_571B70))(); }
		inline uint32_t Mag066Draw(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_066_RenderScrollPrimModel))(h, ot, mode, cursor); }
		inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
		inline void GteSetBackColorFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
		inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
		inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
		inline void GteTransFromIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_TransFromIR))(); }
		inline void GteTransFromMAC() { fn<void (__cdecl *)()>(game_addr(game::GTE_TransFromMAC))(); }
		inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
		inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
		inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
		inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
		inline void GteAVSZ4() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ4))(); }
		inline void GteReadOTZ32(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
		inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
		inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
		inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
		inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
		inline void GteSetFarColor2(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColorRGB))(r, g, b); }
		inline void GridRender(void *h, uint32_t ot, int32_t shift) { fn<void (__cdecl *)(void *, uint32_t, int32_t)>(game_addr(game::MAG_326_GridRender))(h, ot, shift); }
		inline uint32_t SliceRender(void *geom, void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::GF_326OdinRev_ClippedModelRender))(geom, h, ot, mode, cursor); }
		inline uint32_t SlashRender(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_326_SlashRender))(h, ot, mode, cursor); }
		inline void ScreenTile(int32_t r, int32_t g, int32_t b, int32_t otz) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, int32_t)>(game_addr(game::GF_326OdinRev_ScreenTile))(r, g, b, otz); }
		inline int32_t BonePosition(void *entity, int32_t bone, int32_t frac, int16_t *out) { return fn<int32_t (__cdecl *)(void *, int32_t, int32_t, int16_t *)>(game_addr(game::GF_326OdinRev_GetBonePosition))(entity, bone, frac, out); }
	}
	using namespace x;

	inline bool Paused() { return (var<uint32_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C)) & 0x201) != 0; }
	inline Mat4x3 &RootMatrix() { return var<Mat4x3>(game_addr(game::MAG_326_RootMatrix)); }
	inline Mat4x3 &Frame() { return var<Mat4x3>(game_addr(game::MAG_326_Frame)); }
	inline TaskQueue &SubQueue() { return var<TaskQueue>(game_addr(game::MAG_326_SubQueue)); }
	inline uint8_t *CastCtx() { return var<uint8_t *>(game_addr(game::unk_24F2174)); }
	inline uint32_t &VoiceSlot() { return var<uint32_t>(game_addr(game::MAG_326_VoiceSlot)); }
	inline uint32_t CasterSlot() { return var<uint32_t>(game_addr(game::unk_24F0C80)); }
	inline uint8_t *Creature() { return (uint8_t *)game_addr(game::g_326Creature); }
	inline Mat4x3 &CreatureMat() { return var<Mat4x3>(game_addr(game::unk_24F12A8)); }
	inline uint8_t *Model2() { return (uint8_t *)game_addr(game::g_326Model2); }
	inline Mat4x3 &Model2Mat() { return var<Mat4x3>(game_addr(game::MAG_326_Model2Mat)); }
	inline uint8_t *Model3() { return (uint8_t *)game_addr(game::g_326Model3); }
	inline Mat4x3 &BoneCopy() { return var<Mat4x3>(game_addr(game::MAG_326_BoneCopy)); }
	inline int16_t *BladeP0() { return (int16_t *)game_addr(game::unk_24F1310); }
	inline int16_t *BladeP1() { return (int16_t *)game_addr(game::MAG_326_Tip); }
	inline uint32_t &DoneFlags() { return var<uint32_t>(game_addr(game::MAG_326_DoneFlags)); }
	inline uint32_t MB() { return var<uint32_t>(game_addr(game::unk_24F22DC)); }
	inline uint32_t &Cursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t OT() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }
	inline uint8_t *PoolA() { return var<uint8_t *>(game_addr(game::unk_E19668)); }
	inline uint8_t *PoolB() { return var<uint8_t *>(game_addr(game::unk_E1966C)); }
	inline uint8_t *PoolC() { return var<uint8_t *>(game_addr(game::unk_E19670)); }
	inline uint8_t *Entity(uint32_t slot) { return (uint8_t *)(game_addr(game::BattleEntitySlotData) + 156 * slot); }
	static constexpr game::Id HIST_A = game::MAG_326_HistA;
	static constexpr game::Id HIST_B = game::MAG_326_HistB;
	static constexpr game::Id SPLINE_A = game::MAG_326_SplineA;
	static constexpr game::Id SPLINE_B = game::MAG_326_SplineB;
	static constexpr game::Id TRAIL_RECS = game::MAG_326_TrailRecs;
	static constexpr game::Id GHOST_RECS = game::g_326GhostRing;
	static constexpr game::Id STREAM_STATE = game::g_326StreamState;

	static constexpr game::Id ORIG_SequenceTick = game::GF_326OdinRev_SequenceTick;
	static constexpr game::Id ORIG_TimelineTask = game::GF_326OdinRev_TimelineTask;
	static constexpr game::Id ORIG_GridATask = game::GF_326OdinRev_GridATask;
	static constexpr game::Id ORIG_GridBTask = game::GF_326OdinRev_GridBTask;
	static constexpr game::Id ORIG_DebrisTask = game::GF_326OdinRev_DebrisTask;
	static constexpr game::Id ORIG_WarpTask = game::GF_326OdinRev_TextureWarpTask;
	static constexpr game::Id ORIG_GlowTask = game::GF_326OdinRev_GlowTask;
	static constexpr game::Id ORIG_StreakTask = game::GF_326OdinRev_StreakTask;
	static constexpr game::Id ORIG_GridCTask = game::GF_326OdinRev_GridCTask;
	static constexpr game::Id ORIG_GridDTask = game::GF_326OdinRev_GridDTask;
	static constexpr game::Id ORIG_WallTask = game::GF_326OdinRev_WallTask;
	static constexpr game::Id ORIG_ShockTask = game::GF_326OdinRev_ShockPlaneTask;
	static constexpr game::Id ORIG_GhostTask = game::GF_326OdinRev_GhostTrailTask;
	static constexpr game::Id ORIG_SwordTask = game::GF_326OdinRev_GhostSwordTask;
	static constexpr game::Id ORIG_FadeATask = game::GF_326OdinRev_FadeModelTaskA;
	static constexpr game::Id ORIG_FadeBTask = game::GF_326OdinRev_FadeModelTaskB;
	static constexpr game::Id ORIG_TintTask = game::GF_326OdinRev_ScreenTintTask;
	static constexpr game::Id ORIG_EndTask = game::MAG_326_EndTask;
	static constexpr game::Id ORIG_CreatureTask = game::GF_326OdinRev_CreatureTask;
	static constexpr game::Id ORIG_SmokeTask = game::GF_326OdinRev_SmokeTask;
	static constexpr game::Id ORIG_RingATask = game::GF_326OdinRev_RingATask;
	static constexpr game::Id ORIG_RingBTask = game::GF_326OdinRev_RingBTask;
	static constexpr game::Id ORIG_DustTask = game::GF_326OdinRev_DustTask;
	static constexpr game::Id ORIG_MistTask = game::GF_326OdinRev_MistTask;
	static constexpr game::Id ORIG_FadeInTask = game::GF_326OdinRev_CreatureFadeInTask;
	static constexpr game::Id ORIG_FlashTask = game::GF_326OdinRev_ScreenFlashTask;
	static constexpr game::Id ORIG_MorphTask = game::GF_326OdinRev_MorphFlashTask;
	static constexpr game::Id ORIG_SparkTask = game::GF_326OdinRev_SwordSparkTask;
	static constexpr game::Id ORIG_Spark2Task = game::GF_326OdinRev_SwordSpark2Task;
	static constexpr game::Id ORIG_BladeTask = game::GF_326OdinRev_BladeTask;
	static constexpr game::Id ORIG_TrailTask = game::GF_326OdinRev_TrailTask;
	static constexpr game::Id ORIG_BurstATask = game::GF_326OdinRev_BurstATask;
	static constexpr game::Id ORIG_BurstBTask = game::GF_326OdinRev_BurstBTask;
	static constexpr game::Id ORIG_BloodTask = game::GF_326OdinRev_BloodTask;
	static constexpr game::Id ORIG_SlashATask = game::GF_326OdinRev_CutLineATask;
	static constexpr game::Id ORIG_SlashBTask = game::GF_326OdinRev_CutLineBTask;
	static constexpr game::Id ORIG_SlashCTask = game::GF_326OdinRev_CutLineCTask;
	static constexpr game::Id ORIG_SlashDTask = game::GF_326OdinRev_CutLineDTask;

#pragma pack(push, 1)
	struct Node24
	{
		TaskNode hdr;
		int16_t c;
		int16_t e;
		int16_t p10, p12, p14, p16;
		int16_t a18;
		int16_t s1A;
		int16_t s1C, s1E;
		int16_t s20, s22;
	};
	struct Rec
	{
		uint32_t alive;
		int16_t age;
		int16_t size;
		int16_t x, y, z;
		int16_t w0E;
		int16_t vx, vy, vz;
		int16_t w16;
	};
#pragma pack(pop)
	static_assert(sizeof(Node24) == 0x24, "sub-queue node is 0x24 bytes");
	static_assert(sizeof(Rec) == 0x18, "sprite record is 0x18 bytes");
}
}

namespace ff8fx
{
namespace o326
{
	inline Rec *RecOf(uint8_t *pool, int i) { return (Rec *)(pool + 0x18 * i); }

	static int FreeRecord(uint8_t *pool)
	{
		for (int i = 0; i < 150; i++)
			if (*(uint32_t *)(pool + 0x18 * i) == 0) return i;
		return -1;
	}

	static Node24 *LinkTask(uint32_t task_fn)
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), task_fn);
		n->c = 0;
		n->e = 0;
		return n;
	}

	static uint32_t Rgb(int32_t v)
	{
		uint32_t c = (uint32_t)v;
		c = (c << 8) | (uint32_t)v;
		return (c << 8) | (uint32_t)v;
	}

	static uint32_t __cdecl SequenceTick(TaskNode *tn)
	{
		int left = ExecuteTaskQueue(&SubQueue());
		((Node24 *)tn)->c++;
		return left ? 0 : TASK_END;
	}

	static void HideParty() { for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) *(uint8_t *)a |= 2; }
	static void ShowParty() { for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) *(uint8_t *)a &= 0xFD; }

	static void SpawnGrids()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_GridATask));
		n->c = 0;
		n->p10 = 0;
		n->p12 = (int16_t)0xE4A8;
		n->p14 = (int16_t)0xEE6C;
		n->a18 = 0x400;
		n->s1C = 0x3000;
		n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_GridBTask));
		n->c = 0;
		n->p10 = 0;
		n->p12 = (int16_t)0xE570;
		n->p14 = (int16_t)0xEE6C;
		n->a18 = 0x400;
		n->s1C = 0x3000;
	}

	static void SpawnGrids2()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_GridCTask));
		n->c = 0;
		n->p10 = 0;
		n->p12 = (int16_t)0xCD38;
		n->p14 = (int16_t)0xEE6C;
		n->a18 = 0;
		n->s1C = 0x3000;
		n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_GridDTask));
		n->c = 0;
		n->p10 = 0;
		n->a18 = 0;
		n->p12 = (int16_t)0xD120;
		n->p14 = (int16_t)0xEE6C;
		n->s1C = 0x3000;
	}

	static void SpawnIntro()
	{
		Node24 *w = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_WarpTask));
		w->c = 0;
		int32_t r = CrtRand() % 0x1000;
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_GlowTask));
		w->a18 = (int16_t)r;
		n->c = 0;
		n->p10 = 0;
		n->p12 = 0;
		n->p14 = 0;
		n->s1C = 0xB00;
		n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_StreakTask));
		n->c = 0;
		n->e = 1;
		n->p10 = -50;
		n->p12 = 0x3C;
		n->a18 = 0;
		n->s1C = 0xC00;
		n->s1E = -4;
		n->s20 = 0x500;
		n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_StreakTask));
		n->e = 1;
		n->c = 0;
		n->p10 = 0x5A;
		n->p12 = 0xA;
		n->a18 = 0x800;
		n->s1C = 0x1200;
		n->s1E = -6;
		n->s20 = 0x700;
	}

	static void SpawnWalls()
	{
		static const int16_t y[4] = { (int16_t)0xD120, (int16_t)0xD120, (int16_t)0xCF5E, (int16_t)0xCF5E };
		static const int16_t z[4] = { (int16_t)0xEE6C, (int16_t)0xFBB4, (int16_t)0xEE6C, (int16_t)0xFBB4 };
		for (int i = 0; i < 4; i++)
		{
			Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_WallTask));
			bool odd = (i & 1) != 0;
			n->c = 0;
			n->e = odd ? 1 : 0;
			n->p10 = 0;
			n->p12 = y[i];
			n->p14 = z[i];
			n->p16 = i < 2 ? 0 : 0x800;
			n->a18 = 0;
			n->s1C = 0x3000;
			n->s22 = odd ? (int16_t)-40 : (int16_t)0x32;
			n->s1E = odd ? (int16_t)0x21 : (int16_t)-33;
			n->s20 = n->s1E;
		}
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_ShockTask));
		n->c = 0;
		n->p10 = 0;
		n->a18 = 0;
		n->p12 = (int16_t)0xCF2C;
		n->p14 = (int16_t)0xEE6C;
		n->s1E = 0x120;
		n->s1C = 0x120;
		n->s20 = 0x4800;
	}

	static void Stream() { BdPlaySummonStream(0x80, 0, 0x60); }

	static uint32_t __cdecl TimelineTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ComposeAffineTransform(&Camera(), &RootMatrix(), &Frame());
		uint32_t flags = var<uint32_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C));
		if (flags & 0x201)
		{
			if (flags & 1) return 0;
			if (LoadState() < 0) return 0;
		}
		if (n->c == 0)
		{
			CameraScriptStart(game_addr(game::unk_E197D0), &RootMatrix(), Creature(), &SubQueue(), 0);
			BdTransSummonStream(game_addr(game::unk_EECC2C), game_addr(STREAM_STATE));
			VoiceSlot() = ClaimVoiceSlot((const void *)game_addr(game::unk_E194E4), 1, 0x80);
		}
		int32_t c = n->c;
		if (c < 12)
		{
			if (c == 1)
			{
				BdPlaySE(game_addr(game::unk_E1964C), 0, 0x80);
				CharacterLoad(0x2E1, MB() + 0x14800);
			}
			else if (c == 6) SpawnGrids();
			else if (c == 10) LinkTask(game_addr(ORIG_DebrisTask));
		}
		else if ((c -= 12) < 0x1C)
		{
			if (c == 1)
			{
				if (LoadState() < 0) return 0;
				QueueTIMUpload(MB() + 0x14800);
				CharacterLoad(0x2E2, MB());
			}
			else if (c == 10)
			{
				if (LoadState() < 0) return 0;
				CharacterLoad(0x2E3, game_addr(game::unk_EECC2C));
			}
		}
		else if ((c -= 0x1C) < 0x17)
		{
			if (c == 0x10) Stream();
			else if (c == 0) SpawnIntro();
			else if (c == 0xC) LinkTask(game_addr(ORIG_CreatureTask));
		}
		else if ((c -= 0x17) >= 0x1C)
		{
			c -= 0x1C;
			if (c < 0x19)
			{
				if (c == 0x14) BdPlaySE(game_addr(game::unk_E19650), 0, 0x80);
			}
			else if ((c -= 0x19) < 0x23)
			{
				if (c == 0x11) BdTransSummonStream(game_addr(game::unk_EECC2C), game_addr(STREAM_STATE));
				else if (c == 0x16) Stream();
				else if (c == 1)
				{
					if (LoadState() < 0) return 0;
					CharacterLoad(0x2E4, MB() + 0x14800);
				}
				else if (c == 0xB)
				{
					if (LoadState() < 0) return 0;
					QueueTIMUpload(MB() + 0x14800);
				}
				else if (c == 0x19) CharacterLoad(0x2E5, SummonData());
			}
			else if ((c -= 0x23) < 0x14)
			{
				if (c == 0x13) QueueChainTransformation(Entity(CasterSlot()), 0x34);
				else if (c == 0xC)
				{
					if (LoadState() < 0) return 0;
					BdTransSummonStream(SummonData(), game_addr(STREAM_STATE));
				}
			}
			else if ((c -= 0x14) < 0xC)
			{
				if (c == 0) BdPlaySE(game_addr(game::unk_E19654), 0, 0x80);
			}
			else if ((c -= 0xC) < 0xC) {}
			else if ((c -= 0xC) < 0x17)
			{
				if (c == 3) Stream();
				else if (c == 0) CharacterLoad(0x2E6, MB() + 0x14800);
			}
			else if ((c -= 0x17) < 0x22)
			{
				if (c == 0)
				{
					QueueChainTransformation(Entity(CasterSlot()), 0x35);
					if (LoadState() < 0) return 0;
					CharacterLoad(0x2E7, SummonData());
				}
			}
			else if ((c -= 0x22) < 0x46)
			{
				if (c == 0)
				{
					BdPlaySE(game_addr(game::unk_E19658), 0, 0x80);
					if (LoadState() < 0) return 0;
					BdTransSummonStream(SummonData(), game_addr(STREAM_STATE));
					CharacterLoad(0x2E8, var<uint32_t>(game_addr(game::unk_24F22E4)));
				}
			}
			else if ((c -= 0x46) < 0x3C)
			{
				if (c == 0)
				{
					BdPlaySE(game_addr(game::unk_E1965C), 0, 0x80);
					QueueTIMUpload(MB() + 0x14800);
					QueueChainTransformation(Entity(CasterSlot()), 1);
					SpawnGrids2();
				}
				else if (c == 0x1E)
				{
					Stream();
					SpawnWalls();
				}
				else if (c == 1)
				{
					LinkTask(game_addr(ORIG_GhostTask));
					LinkTask(game_addr(ORIG_SwordTask));
				}
				else if (c == 0x1B) CameraShake(2, 1, 2, 0xFF);
			}
			else if ((c -= 0x3C) < 0x14)
			{
				if (c == 0)
				{
					BdPlaySE(game_addr(game::unk_E19660), 0, 0x80);
					LinkTask(game_addr(ORIG_FadeATask));
					LinkTask(game_addr(ORIG_FadeBTask));
					LinkTask(game_addr(ORIG_TintTask));
					ShowParty();
				}
			}
			else if ((c -= 0x14) < 8)
			{
				if (c == 5)
				{
					if (VoiceSlotBusy(VoiceSlot())) ReleaseVoiceSlot(VoiceSlot());
				}
			}
		}
		int16_t cc = n->c;
		if (cc < 6) SetScreenFlash((uint32_t)(682 * (int32_t)cc), 0);
		else if (cc == 6) ShowParty();
		else if (cc == 0x28)
		{
			HideParty();
			SetScreenFlash(0xA00, 0);
		}
		else if (cc >= 0x162 && cc < 0x172) SetScreenFlash((uint32_t)((0x172 - (int32_t)cc) * 160), 0);
		if (n->c == 0x198)
		{
			uint8_t *t = *(uint8_t **)(CastCtx() + 4);
			ApplyActionResultToTargets(*(uint32_t *)(t + 8), t[0x10]);
		}
		n->c++;
		if (n->c < 0x19A) return 0;
		LinkTask(game_addr(ORIG_EndTask));
		HideParty();
		SetScreenFlash(0, 0);
		return TASK_END;
	}

	static uint32_t __cdecl EndTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->c == 1) WaitAnimSeq(MB() + 0x14800, &n->e);
		uint16_t done = (uint16_t)n->e;
		n->c++;
		return done ? TASK_END : 0;
	}

	static int32_t GridAFade(int32_t c) { return c < 8 ? c * 8 : (c >= 0x2E ? (0x36 - c) * 8 : 0x40); }
	static int32_t GridBFade(int32_t c) { return c < 8 ? c * 8 : (c >= 0x26 ? (0x36 - c) * 4 : 0x40); }
	static int32_t GridCU(int32_t c) { return c < 0x1E ? c : 0x1E; }
	static int32_t GridCFade(int32_t c) { return c < 0x1E ? 0x40 : 0x20; }

	static void GridDraw(const Node24 *n, int32_t w, int32_t count, int32_t u, int32_t v, int32_t fade)
	{
		int16_t ang[4] = { 0, n->a18, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = n->p10;
		m.t[2] = n->p14;
		m.t[1] = n->p12;
		int32_t sc[3] = { n->s1C, n->s1C, n->s1C };
		Scale3DMatrix(&m, sc);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x78);
		int32_t *hd = (int32_t *)h;
		hd[1] = 0x20;
		hd[2] = count;
		hd[0] = w;
		hd[3] = u;
		hd[4] = v;
		*(uint32_t *)(h + 0x74) = Rgb(fade);
		GridRender(h, OT(), 2);
		FieldFree(0x78);
	}

	static uint32_t __cdecl GridATask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int32_t c = n->c;
		GridDraw(n, 0x100, 0x20, c, c * 2, GridAFade(c));
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x36 ? TASK_END : 0;
	}

	static uint32_t __cdecl GridBTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int32_t c = n->c;
		GridDraw(n, 0x200, 0x10, c * 2, c * 2, GridBFade(c));
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x36 ? TASK_END : 0;
	}

	static uint32_t __cdecl GridCTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int32_t c = n->c;
		int32_t u = GridCU(c);
		GridDraw(n, 0x100, 0x20, u, -u, GridCFade(c));
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x3C ? TASK_END : 0;
	}

	static uint32_t __cdecl GridDTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int32_t c = n->c;
		GridDraw(n, 0x200, 0x10, c * 2, shl32(-c, 1), 0x40);
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x1E ? TASK_END : 0;
	}

	static void LitSprite(uint8_t *h, const int16_t *pos, int16_t size, int16_t frame, bool mac = false)
	{
		GteLoadV0(pos);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(size);
		*(int16_t *)(h + 4) = frame;
		if (mac) GteTransFromMAC();
		else GteTransFromIR();
		Cursor() = InitEffectSequenceFromData(h, OT(), 2, Cursor());
	}

	enum Integrator { INT_NONE, INT_FALL, INT_DRAG4, INT_GROW5_DRAG8, INT_XZ_DRAG8, INT_GROW6_DRAG8, INT_BLOOD };
	static void Integrate(int kind, Rec *r)
	{
		switch (kind)
		{
		case INT_FALL:
			r->y = (int16_t)(r->y + r->vy);
			r->vy = (int16_t)(r->vy + (int16_t)(r->vy >> 5));
			break;
		case INT_DRAG4:
			r->x = (int16_t)(r->x + r->vx);
			r->y = (int16_t)(r->y + r->vy);
			r->z = (int16_t)(r->z + r->vz);
			r->vx = (int16_t)(r->vx - (int16_t)(r->vx >> 2));
			r->vy = (int16_t)(r->vy - (int16_t)(r->vy >> 2));
			r->vz = (int16_t)(r->vz - (int16_t)(r->vz >> 2));
			break;
		case INT_GROW5_DRAG8:
		case INT_GROW6_DRAG8:
			r->size = (int16_t)(r->size + (int16_t)(r->size >> (kind == INT_GROW5_DRAG8 ? 5 : 6)));
			r->x = (int16_t)(r->x + r->vx);
			r->y = (int16_t)(r->y + r->vy);
			r->z = (int16_t)(r->z + r->vz);
			r->vx = (int16_t)(r->vx - (int16_t)(r->vx >> 3));
			r->vy = (int16_t)(r->vy - (int16_t)(r->vy >> 3));
			r->vz = (int16_t)(r->vz - (int16_t)(r->vz >> 3));
			break;
		case INT_XZ_DRAG8:
			r->x = (int16_t)(r->x + r->vx);
			r->z = (int16_t)(r->z + r->vz);
			r->vx = (int16_t)(r->vx - (int16_t)(r->vx >> 3));
			r->vz = (int16_t)(r->vz - (int16_t)(r->vz >> 3));
			break;
		case INT_BLOOD:
			r->size = (int16_t)(r->size + (int16_t)(r->size >> 5));
			r->x = (int16_t)(r->x + r->vx);
			r->y = (int16_t)(r->y + r->vy);
			r->z = (int16_t)(r->z + r->vz);
			r->vx = (int16_t)(r->vx - (int16_t)(r->vx >> 3));
			r->vy = (int16_t)(r->vy + (int16_t)(r->vy >> 6));
			r->vz = (int16_t)(r->vz - (int16_t)(r->vz >> 3));
			break;
		default:
			break;
		}
	}

	static void DebrisDrawOne(uint8_t *h, const int16_t *pos, int16_t size)
	{
		GteLoadV0(pos);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(size);
		*(int32_t *)(h + 0x10) = (int32_t)size * 2 + 0x1200;
		GteTransFromIR();
		Cursor() = InitEffectSequenceFromData(h, OT(), 2, Cursor());
	}

	static void SplashDrawOne(uint8_t *h, uint8_t *s, const Rec *r)
	{
		ff8fx::copy<(0x20)>(s + 0x28, s + 8);
		*(int32_t *)(s + 0x3C) = r->x;
		*(int32_t *)(s + 0x40) = r->y;
		*(int32_t *)(s + 0x44) = r->z;
		*(int32_t *)(s + 0x50) = r->size;
		*(int32_t *)(s + 0x4C) = r->size;
		*(int32_t *)(s + 0x48) = r->size;
		Scale3DMatrix((Mat4x3 *)(s + 0x28), (const int32_t *)(s + 0x48));
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(s + 0x28), (Mat4x3 *)(s + 0x28));
		GteSetRotMatrixCtrl((const Mat4x3 *)(s + 0x28));
		GteSetTransVectorCtrl((const Mat4x3 *)(s + 0x28));
		*(int16_t *)(h + 4) = r->age;
		Cursor() = InitEffectSequenceFromData(h, OT(), 2, Cursor());
	}

	static void DebrisSetupA(uint8_t *h, uint8_t *s)
	{
		*(uint32_t *)(h + 0x14) = 0x1000;
		*(uint32_t *)(h + 0xC) = 0x1000;
		ff8fx::copy<(0x20)>(s + 8, &Camera());
		*(uint32_t *)h = game_addr(game::unk_E10C80);
		*(int16_t *)(h + 4) = 0;
		*(int16_t *)(h + 0x24) = 2;
		GteSetLightMatrix(s + 8);
		GteSetBackColorFromTrans(s + 8);
	}

	static void DebrisSetupB(uint8_t *h, uint8_t *s)
	{
		*(uint32_t *)h = game_addr(game::unk_E10CA8);
		*(int16_t *)(h + 0x24) = 0x200;
		*(int16_t *)(s + 0) = 0x400;
		*(int16_t *)(s + 2) = 0;
		*(int16_t *)(s + 4) = 0;
		ComposeZYXRotationMatrix((const int16_t *)s, (Mat4x3 *)(s + 8));
	}

	static uint32_t __cdecl DebrisTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x68);
		int32_t count = 0;
		DebrisSetupA(h, s);
		for (int i = 0; i < 150; i++)
		{
			Rec *r = RecOf(PoolA(), i);
			if (!(r->alive & 1)) continue;
			DebrisDrawOne(h, &r->x, r->size);
			if (Paused()) continue;
			r->y = (int16_t)(r->y + r->vy);
			r->vy = (int16_t)(r->vy + (int16_t)(r->vy >> 5));
			if (r->y < 0)
			{
				count++;
				continue;
			}
			r->alive = 0;
			int j = FreeRecord(PoolB());
			if (j < 0) continue;
			Rec *b = RecOf(PoolB(), j);
			b->alive = 1;
			b->age = 0;
			int32_t rr = CrtRand() % 0x400;
			b->size = (int16_t)(rr + r->size + 0x400);
			b->x = r->x;
			b->y = 0;
			b->z = r->z;
		}
		DebrisSetupB(h, s);
		for (int i = 0; i < 150; i++)
		{
			Rec *r = RecOf(PoolB(), i);
			if (!(r->alive & 1)) continue;
			SplashDrawOne(h, s, r);
			if (Paused()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				r->alive = 0;
			}
			else count++;
		}
		FieldFree(0x68);
		FieldFree(0xB4);
		if (Paused()) return 0;
		FieldAlloc(0x68);
		int16_t c = n->c;
		if (c >= 0 && c <= 0x96)
		{
			int32_t k = CrtRand() % 6;
			k = k + (int32_t)c / 4 + 1;
			if (k > 0x1E) k = 0x1E;
			for (int m = 0; m < k; m++)
			{
				int i = FreeRecord(PoolA());
				if (i < 0) break;
				Rec *r = RecOf(PoolA(), i);
				r->alive = 1;
				r->age = 0;
				r->size = (int16_t)(CrtRand() % 0x480 + 0x300);
				r->x = (int16_t)(CrtRand() % 0x1770 - 0xBB8);
				r->y = (int16_t)(-4000 - CrtRand() % 1000);
				r->z = (int16_t)(CrtRand() % 10000 - 5000);
				r->vy = (int16_t)(CrtRand() % 0x226 + 0xB4);
			}
		}
		FieldFree(0x68);
		n->c++;
		if (n->c < 4 || count != 0) return 0;
		return TASK_END;
	}

	static uint32_t __cdecl WarpTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *src = var<uint8_t *>(game_addr(game::unk_24F22E4));
		uint8_t *dst = src + 0x4000;
		int32_t a1 = n->a18, a2 = n->a18;
		uint8_t *row = dst;
		int32_t y = 0;
		for (int32_t acc = 0; acc < 0x40000; acc += 0x800, y++, row += 0x80)
		{
			int32_t s1 = ComputeSin(a1);
			int32_t amp = shl32(-mul32(s1, 3), 2) >> 12;
			int32_t s2 = ComputeSin(acc / 128);
			a1 += 0x20;
			int32_t off = mul32(s2, amp) >> 12;
			int32_t s3 = ComputeSin(a2);
			int32_t sy = (y - (shl32(s3, 1) >> 12)) & 0x7F;
			a2 += 0x70;
			int32_t base = (0x7F - sy) << 7;
			for (int32_t x = 0; x < 0x80; x++) row[x] = src[((x - off) & 0x7F) + base];
		}
		QueueVramUpload(game_addr(game::unk_E196B8), dst);
		if (Paused()) return 0;
		n->a18 = (int16_t)(n->a18 + 0x20);
		n->c++;
		return n->c > 0x1A ? TASK_END : 0;
	}

	static void GlowDraw(const Mat4x3 *frame, const Node24 *n, int32_t c)
	{
		int16_t ang[4] = { 0, 0, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[1] = n->p12;
		m.t[0] = n->p10;
		int32_t sc[3] = { n->s1C, n->s1C, n->s1C };
		m.t[2] = n->p14;
		Scale3DMatrix(&m, sc);
		ComposeAffineTransform(frame, &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x90);
		*(uint32_t *)h = game_addr(game::unk_E163F8);
		*(uint16_t *)(h + 0x1C) = 0x80;
		*(uint16_t *)(h + 0x1E) = 0x80;
		*(int32_t *)(h + 0x14) = c;
		*(int32_t *)(h + 0xC) = 0x400;
		*(int32_t *)(h + 0x10) = -c;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x20) = 0xF3;
		*(uint16_t *)(h + 0x18) = 0;
		*(uint16_t *)(h + 0x1A) = 0;
		*(uint32_t *)(h + 0x24) = 0x808080;
		Cursor() = Mag066Draw(h, OT(), 2, Cursor());
		FieldFree(0x90);
	}

	static uint32_t __cdecl GlowTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		GlowDraw(&Frame(), n, n->c);
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x1A ? TASK_END : 0;
	}

	static void StreakDraw(const Node24 *n, int32_t x)
	{
		int16_t ang[4] = { 0, n->a18, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = x;
		m.t[1] = n->p12;
		int32_t sc[3];
		sc[0] = n->s1C;
		m.t[2] = var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		sc[2] = n->s20;
		sc[1] = n->s20;
		Scale3DMatrix(&m, sc);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = game_addr(game::unk_E15D10);
		*(uint32_t *)(h + 0xC) = 0x800;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0xF3;
		Cursor() = RenderPrimModel(h, OT(), 0xE, Cursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl StreakTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->e > 0)
		{
			if (Paused()) return 0;
			n->e--;
			return 0;
		}
		StreakDraw(n, n->p10);
		if (Paused()) return 0;
		n->p10 = (int16_t)(n->p10 + n->s1E);
		n->c++;
		return n->c >= 0x17 ? TASK_END : 0;
	}

	static void WallDraw(const Node24 *n, int32_t s20, int32_t z)
	{
		int16_t ang[4] = { 0, n->a18, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[1] = n->p12;
		m.t[0] = (int32_t)n->p10 + s20;
		m.t[2] = z;
		int32_t sc[3] = { n->s1C, n->s1C, n->s1C };
		Scale3DMatrix(&m, sc);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = n->e != 0 ? game_addr(game::unk_E17450) : game_addr(game::unk_E16CF0);
		*(int32_t *)(h + 0xC) = n->p16;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0xF3;
		Cursor() = RenderPrimModel(h, OT(), 2, Cursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl WallTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		WallDraw(n, n->s20, n->p14);
		if (Paused()) return 0;
		n->p14 = (int16_t)(n->p14 + n->s22);
		n->s20 = (int16_t)(n->s20 + (int16_t)((int32_t)n->s1E / 4));
		n->c++;
		return n->c >= 0x1E ? TASK_END : 0;
	}

	static void ShockDraw(const Node24 *n, int32_t sx)
	{
		int16_t ang[4] = { 0, 0, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[1] = n->p12;
		m.t[0] = n->p10;
		int32_t sc[3];
		sc[2] = n->s20;
		sc[1] = n->s20;
		m.t[2] = n->p14;
		sc[0] = sx;
		Scale3DMatrix(&m, sc);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = game_addr(game::unk_E17BB0);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		Cursor() = RenderPrimModel(h, OT(), 2, Cursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl ShockTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ShockDraw(n, n->s1C);
		if (Paused()) return 0;
		int16_t v = n->s1E;
		n->s1C = (int16_t)(n->s1C + v);
		n->c++;
		n->s1E = (int16_t)(v + (int16_t)(v >> 5));
		return n->c >= 0x1E ? TASK_END : 0;
	}

	static void ResetRotation(void *m)
	{
		uint32_t *d = (uint32_t *)m;
		d[3] = 0;
		d[2] = 0;
		d[1] = 0;
		d[0] = 0;
		*(uint16_t *)((uint8_t *)m + 0x10) = 0x1000;
		*(uint16_t *)((uint8_t *)m + 8) = 0x1000;
		*(uint16_t *)((uint8_t *)m + 0) = 0x1000;
	}

	static void ModelInit(uint8_t *E, uint8_t *data, uint8_t *buf, int32_t id)
	{
		*(uint16_t *)(E + 0) = 2;
		E[7] = 0;
		*(uint32_t *)(E + 0x28) = 0x808080;
		E[0x61] = 0;
		E[0x60] = 0;
		*(uint8_t **)(E + 0x68) = data + 0xC;
		*(uint8_t **)(E + 0x64) = data + 0xC;
		*(uint32_t *)(E + 0x78) = 0;
		*(uint32_t *)(E + 0x7C) = 0xFFFFFFFF;
		*(uint8_t **)(E + 0x84) = data;
		*(uint32_t *)(E + 0x8C) = 0;
		*(uint8_t **)(data + 0xC) = buf + *(uint32_t *)(buf + 4);
		*(uint8_t **)(data + 0x10) = buf + *(uint32_t *)(buf + 8);
		*(uint8_t **)(data + 0x14) = buf + *(uint32_t *)(buf + 0xC);
		*(uint16_t *)(data + 2) = (uint16_t)id;
		*(uint8_t **)(data + 0x30) = buf + *(uint32_t *)(buf + 0x10);
	}

	static void SetAnim(uint8_t *E, int32_t id) { PreReadAnimation(E + 0x60, E + 0x6C, id); }

	static void AdvanceModelAnimLoop(uint8_t *E)
	{
		if (ReadAnimation(E + 0x60, E + 0x6C) == 1 && !(E[0] & 1))
			PreReadAnimation(E + 0x60, E + 0x6C, E[0x6C]);
	}

	static void DrawModel(uint8_t *E, const Mat4x3 *frame, uint32_t scratch)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x4C);
		ComposeAffineTransform(frame, (const Mat4x3 *)(E + 0x40), (Mat4x3 *)h);
		ComputeBonesWorldMatrices(E + 0x60, h);
		*(uint32_t *)(h + 0x30) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		*(uint32_t *)(h + 0x24) = scratch;
		*(uint16_t *)(h + 0x34) = 0;
		*(uint16_t *)(h + 0x36) = 0;
		*(uint32_t *)(h + 0x3C) = *(uint32_t *)(E + 0x28);
		*(uint32_t *)(h + 0x48) = 0;
		*(uint16_t *)(h + 0x44) = 0;
		*(uint16_t *)(h + 0x38) = 0x140;
		*(uint16_t *)(h + 0x3A) = 0xD8;
		*(uint32_t *)(h + 0x40) = *(uint32_t *)(E + 0x7C);
		Cursor() = RenderGeometry(*(void **)(E + 0x64), h + 0x20, OT(), 4, Cursor());
		BuildBoneMatricesFromPose(E + 0x60);
		FieldFree(0x4C);
	}

	static uint32_t Scratch() { return var<uint32_t>(game_addr(game::unk_E196B0)); }

	static void SpawnRings()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_RingATask));
		n->c = 0;
		n->p10 = 0;
		n->p12 = 0;
		n->p14 = 0x28;
		n->a18 = (int16_t)(CrtRand() % 0x1000);
		n->s1C = 0xE00;
		n->s22 = 0x555;
		n->s20 = 0x555;
		n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_RingBTask));
		n->c = 0;
		n->p10 = 0;
		n->p12 = 0;
		n->p14 = 0x28;
		n->a18 = (int16_t)(CrtRand() % 0x1000);
		n->s1E = 0x500;
		n->s1C = 0x500;
		n->s20 = 0x200;
	}

	static void SpawnFadeIn()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_FadeInTask));
		n->c = 0;
		n->e = 0;
		n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_FlashTask));
		n->c = 0;
		n->e = 0xA10;
		n->s1C = 0xFF;
		n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_MorphTask));
		n->c = 0;
		n->s1C = 0x400;
		n->s20 = 0x400;
	}

	static void SpawnSparks()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_SparkTask));
		n->c = 0;
		for (uint32_t a = game_addr(game::unk_24F0C88); a < game_addr(game::unk_24F0CC4); a += 0x14) *(uint32_t *)a = 0;
	}

	static void SpawnSparks2()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_Spark2Task));
		n->c = 0;
		n->e = 0x1E;
		n->a18 = 1;
		n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_Spark2Task));
		n->c = 0;
		n->e = 0x34;
		n->a18 = 2;
		for (uint32_t a = game_addr(TRAIL_RECS); a < game_addr(game::MAG_326_RootMatrix); a += 0x14) *(uint32_t *)a = 0;
		for (uint32_t k = 0; k < 0x3C; k += 0x14)
		{
			*(uint32_t *)(game_addr(game::unk_24F0C88) + k) = 0;
			*(uint32_t *)(game_addr(game::unk_24F0DF0) + k) = 0;
		}
	}

	static void SpawnTrail()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_TrailTask));
		n->c = 0;
		for (uint32_t a = game_addr(TRAIL_RECS); a < game_addr(game::MAG_326_RootMatrix); a += 0x14) *(uint32_t *)a = 0;
	}

	static void SpawnBurstA()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_BurstATask));
		n->c = 0;
		GetDefaultEffectPosition(Entity(CasterSlot()), &n->p10);
		n->a18 = (int16_t)(CrtRand() % 0x1000);
		n->s1C = 0x1000;
		n->s20 = 0x1E00;
	}

	static void SpawnBurstB()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_BurstBTask));
		n->c = 0;
		n->a18 = (int16_t)(CrtRand() % 0x1000);
		n->s1C = 0x600;
		n->s20 = 0xA00;
	}

	static void SpawnBlood()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_BloodTask));
		n->c = 0;
		n->e = 4;
		GetEffectSpawnPosition(Creature(), 0xF1, 0, &n->p10);
		n->p10 = 0x320;
		n->p12 = 0;
		Node24 *m = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_BloodTask));
		int16_t x = n->p10, z = n->p14;
		m->c = 0;
		m->p12 = 0;
		m->e = 8;
		m->p10 = (int16_t)-x;
		m->p14 = z;
	}

	static void SpawnSlashes()
	{
		for (uint32_t e = game_addr(game::unk_E19710); e < game_addr(game::unk_E19728); e += 8)
		{
			const int16_t *t = (const int16_t *)e;
			Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_SlashATask));
			n->e = t[3];
			int16_t z = t[2];
			n->p10 = t[0];
			n->c = 0;
			n->p12 = t[1];
			n->p14 = z;
			n->p16 = 0;
			n->s1C = 0x4000;
			n->s1E = 0x2000;
			n->s20 = (int16_t)(0x28 - z);
			n->s22 = z;
			for (int i = 0; i < 2; i++)
			{
				Node24 *u = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_SlashBTask));
				u->e = n->e;
				*(uint32_t *)&u->p10 = *(const uint32_t *)&n->p10;
				*(uint32_t *)&u->p14 = *(const uint32_t *)&n->p14;
				u->s1C = n->s1C;
				u->s1E = n->s1E;
				int16_t cc = (int16_t)(u->p14 + i);
				u->c = 0;
				u->p14 = cc;
				u->p16 = 0x800;
				u->s20 = (int16_t)(0x28 - cc);
				u->s22 = cc;
			}
		}
		const int16_t *t = (const int16_t *)game_addr(game::unk_E19728);
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_SlashCTask));
		n->e = t[3];
		int16_t z = t[2];
		n->p10 = t[0];
		n->c = 0;
		n->p12 = t[1];
		n->p14 = z;
		n->p16 = 0;
		n->s1C = 0x2DC0;
		n->s1E = 0xF40;
		n->s20 = (int16_t)(0x28 - z);
		n->s22 = z;
		for (int i = 0; i < 4; i++)
		{
			Node24 *u = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_SlashDTask));
			u->e = n->e;
			*(uint32_t *)&u->p10 = *(const uint32_t *)&n->p10;
			*(uint32_t *)&u->p14 = *(const uint32_t *)&n->p14;
			u->s1C = n->s1C;
			u->s1E = n->s1E;
			int16_t cc = (int16_t)(u->p14 + i);
			u->c = 0;
			u->p14 = cc;
			u->p16 = 0x800;
			u->s20 = (int16_t)(0x28 - cc);
			u->s22 = cc;
		}
	}

	static void CasterFlags(bool hide)
	{
		uint16_t *a = (uint16_t *)Entity(CasterSlot());
		uint16_t *b = (uint16_t *)(Entity(CasterSlot()) + 0x9C);
		if (hide)
		{
			*a = (uint16_t)(*a | 8);
			*b = (uint16_t)(*b | 8);
		}
		else
		{
			*a = (uint16_t)(*a & 0xFFF7);
			*b = (uint16_t)(*b & 0xFFF7);
		}
	}

	static void PlaneDistance(const int16_t *p, int16_t *nrm)
	{
		int32_t d = mul32(p[2], nrm[2]) + mul32(p[1], nrm[1]);
		d = d + mul32(p[0], nrm[0]);
		nrm[3] = (int16_t)-(d >> 12);
	}

	static void SliceSetup(uint8_t *h, uint8_t *E)
	{
		*(int16_t *)(h + 0xA2) = 0;
		*(int16_t *)(h + 0xA4) = 0;
		*(uint16_t *)(h + 0xA0) = 0xF000;
		BonePosition(E, 0x21, 0x800, (int16_t *)(h + 0x40));
		*(int16_t *)(h + 0x40) = (int16_t)(*(int16_t *)(h + 0x40) + 0xD);
		PlaneDistance((const int16_t *)(h + 0x40), (int16_t *)(h + 0xA0));
	}

	static void CreatureSliceDraw(uint8_t *h, uint8_t *E, const Mat4x3 *m1, const Mat4x3 *m2, const Mat4x3 *frame, uint32_t scratch)
	{
		if (!(E[0] & 0x20))
			Cursor() = DrawShadow(E, var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4064, 0x10, Cursor());
		ComposeAffineTransform(frame, m1, (Mat4x3 *)(h + 0x190));
		ComposeAffineTransform(frame, m2, (Mat4x3 *)(h + 0x1B0));
		*(uint32_t *)(h + 4) = scratch;
		*(uint32_t *)(h + 0x18) = *(uint32_t *)(E + 0x28);
		*(uint32_t *)(h + 0x10) = *(uint32_t *)(E + 0x7C);
		Cursor() = SliceRender(*(void **)(E + 0x64), h, OT(), 2, Cursor());
		BuildBoneMatricesFromPose(E + 0x60);
	}

	static void SliceADraw(uint8_t *E, int32_t d, const Mat4x3 *frame, uint32_t scratch)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x1D0);
		SliceSetup(h, E);
		int32_t t0 = CreatureMat().t[0];
		Mat4x3 ma = CreatureMat(), mb = CreatureMat();
		ma.t[0] = (int32_t)((uint32_t)t0 - (uint32_t)d);
		mb.t[0] = (int32_t)((uint32_t)t0 + (uint32_t)d);
		CreatureSliceDraw(h, E, &mb, &ma, frame, scratch);
		FieldFree(0x1D0);
	}

	static void DepthCue(int32_t w, int32_t r, int32_t g, int32_t b, void *out)
	{
		GteSetFarColor(r, g, b);
		uint32_t zero = 0;
		GteLoadRGBC(&zero);
		GteSetIR0(w);
		GteDPCS();
		GteStoreRGB2(out);
	}

	static int32_t SliceBFadeW(int32_t c) { return 0x1000 - ComputeSin(shl32(c - 0x2E, 10) / 12); }

	static void SliceBDraw(uint8_t *E, int32_t c, int16_t a, const Mat4x3 *frame, uint32_t scratch, bool recolor, uint32_t color)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x1D0);
		SliceSetup(h, E);
		Mat4x3 ma = CreatureMat(), mb = CreatureMat();
		ma.t[0] = (int32_t)0xFFFFFCE0;
		mb.t[0] = 0x320;
		int16_t ang[4] = { 0, 0, a, 0 };
		Mat4x3 mc;
		ComposeZYXRotationMatrix(ang, &mc);
		GteMatrixMultiply(&mc, &mb);
		ang[2] = (int16_t)-a;
		ComposeZYXRotationMatrix(ang, &mc);
		GteMatrixMultiply(&mc, &ma);
		if (recolor)
		{
			if (c >= 0x2E)
			{
				int32_t w = SliceBFadeW(c);
				DepthCue(w, 0x80, 0x80, 0x80, E + 0x28);
				E[0x2B] = 2;
			}
		}
		else *(uint32_t *)(E + 0x28) = color;
		CreatureSliceDraw(h, E, &mb, &ma, frame, scratch);
		FieldFree(0x1D0);
	}

	static void Draw(uint8_t *E)
	{
		DrawModel(E, &Frame(), Scratch());
	}

	static uint32_t __cdecl CreatureTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *E = Creature();
		if (n->c == 0)
		{
			ResetRotation((void *)game_addr(game::unk_24F12A8));
			var<uint16_t>(game_addr(game::unk_24F12B8)) = 0x2000;
			var<uint16_t>(game_addr(game::unk_24F12B0)) = 0x2000;
			var<uint16_t>(game_addr(game::unk_24F12A8)) = 0x2000;
			var<int32_t>(game_addr(game::unk_24F12C0)) = 0;
			var<int32_t>(game_addr(game::unk_24F12BC)) = 0;
			var<int32_t>(game_addr(game::unk_24F12C4)) = 0x708;
			ModelInit(E, (uint8_t *)game_addr(game::unk_24F22E8), (uint8_t *)MB(), 0xF);
			E[0] |= 1;
			LinkTask(game_addr(ORIG_BladeTask));
			var<uint32_t>(game_addr(game::unk_24F12E4)) = 7;
		}
		int32_t c = n->c;
		if (c < 12)
		{
			if (Paused()) goto draw_advance;
			if (c == 0) SetAnim(E, 0);
			else if (c == 3)
			{
				LinkTask(game_addr(ORIG_SmokeTask));
				SpawnRings();
				LinkTask(game_addr(ORIG_DustTask));
			}
			goto draw_advance;
		}
		if ((c -= 12) < 0x1C)
		{
			var<uint16_t>(game_addr(game::unk_24F12B8)) = 0x1000;
			var<uint16_t>(game_addr(game::unk_24F12B0)) = 0x1000;
			var<uint16_t>(game_addr(game::unk_24F12A8)) = 0x1000;
			if (Paused()) goto draw;
			if (c == 0) SetAnim(E, 1);
			else if (c == 9) LinkTask(game_addr(ORIG_MistTask));
			goto advance_draw;
		}
		if ((c -= 0x1C) < 0xD)
		{
			if (Paused()) goto draw;
			if (c == 0)
			{
				SetAnim(E, 2);
				AdvanceModelAnimLoop(E);
			}
			else if (c == 0xC) SpawnFadeIn();
			goto advance_draw;
		}
		if ((c -= 0xD) < 0xC) goto draw;
		if ((c -= 0xC) < 0x2B)
		{
			if (Paused()) goto draw_advance;
			if (c == 0) SetAnim(E, 3);
			else if (c == 0x11) SpawnSparks();
			else if (c == 0x29) SpawnSparks2();
			goto draw_advance;
		}
		if ((c -= 0x2B) < 0xC)
		{
			if (Paused()) goto draw_advance;
			if (c == 0)
			{
				SetAnim(E, 4);
				SpawnBurstA();
			}
			goto draw_advance;
		}
		if ((c -= 0xC) < 0xC)
		{
			if (Paused()) goto draw;
			if (c == 0) SetAnim(E, 4);
			goto draw;
		}
		if ((c -= 0xC) < 0xB)
		{
			if (!Paused() && c == 0) SpawnBurstB();
			Draw(E);
			if (Paused()) goto end;
			AdvanceModelAnimLoop(E);
			GetEffectSpawnPosition(E, 0, 0, &n->p10);
			goto end;
		}
		if ((c -= 0xB) < 0x1F)
		{
			if (Paused()) goto draw_advance;
			if (c == 0)
			{
				var<int32_t>(game_addr(game::unk_24F12BC)) = 0xD2;
				var<int32_t>(game_addr(game::unk_24F12C4)) = (int32_t)n->p14 + 0x4B0;
				SetAnim(E, 5);
				CameraShake(4, 6, 0xC, 0xFF);
				var<int32_t>(game_addr(game::unk_24F12C4)) = (int32_t)((uint32_t)var<int32_t>(game_addr(game::unk_24F12C4)) - 0x1C2u);
			}
			else if (c == 0xA) var<uint32_t>(game_addr(game::unk_24F12E4)) = 3;
			else if (c < 0xA)
			{
				var<int32_t>(game_addr(game::unk_24F12C4)) = (int32_t)((uint32_t)var<int32_t>(game_addr(game::unk_24F12C4)) - 0x1C2u);
				if (c == 3) SpawnTrail();
			}
			goto draw_advance;
		}
		if ((c -= 0x1F) < 0x1A)
		{
			if (!Paused() && c == 0x18) SpawnSlashes();
			else if (c < 1) goto draw;
			SliceADraw(E, c * 8 - 8, &Frame(), Scratch());
			n->s20 = 0x80;
			goto end;
		}
		if ((c -= 0x1A) < 0x36)
		{
			if (!Paused())
			{
				if (c == 0x16)
				{
					CameraShake(2, 1, 4, 0xFF);
					CasterFlags(true);
				}
				else if (c == 0x2E || c >= 0x1E)
				{
					if (c == 0x2E) SpawnBlood();
					CasterFlags(false);
					n->s20 = (int16_t)(n->s20 + 0x12);
				}
				else CasterFlags(true);
			}
			SliceBDraw(E, c, n->s20, &Frame(), Scratch(), true, 0);
		}
		goto end;

	advance_draw:
		AdvanceModelAnimLoop(E);
	draw:
		Draw(E);
		goto end;

	draw_advance:
		Draw(E);
		if (!Paused()) AdvanceModelAnimLoop(E);

	end:
		if (Paused()) return 0;
		n->c++;
		if (n->c < 0x100) return 0;
		DoneFlags() |= 1;
		return TASK_END;
	}

	static uint32_t __cdecl FadeInTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->e > 0)
		{
			if (Paused()) return 0;
			n->e--;
			return 0;
		}
		int16_t c = n->c;
		int32_t v = c >= 1 ? (int32_t)c * 12 - 12 : 0;
		var<uint32_t>(game_addr(game::unk_24F1290)) = Rgb(v);
		if (Paused()) return 0;
		n->c = (int16_t)(c + 1);
		if (n->c < 0xB) return 0;
		var<uint32_t>(game_addr(game::unk_24F1290)) = 0x808080;
		return TASK_END;
	}

	static int32_t FlashLevel(int32_t c, int32_t e, int32_t level)
	{
		int32_t a = e & 0xF;
		if (c < a) return (level / a) * c;
		c -= a;
		int32_t b = (e >> 4) & 0xF;
		if (c < b) return level;
		int32_t o = (e >> 8) & 0xF;
		c -= b;
		if (c < o) return level - (level / o) * c;
		return -1;
	}

	static int32_t FlashDepth()
	{
		int32_t d[3];
		d[0] = (int32_t)var<int16_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16)) - (int32_t)var<int16_t>(game_addr(game::Battle_Camera_world_XZ_s16));
		d[1] = (int32_t)var<int16_t>(game_addr(game::unk_B8B7FA)) - (int32_t)var<int16_t>(game_addr(game::unk_B8B7F2));
		d[2] = (int32_t)var<int16_t>(game_addr(game::Battle_Camera_LookAt_Y)) - (int32_t)var<int16_t>(game_addr(game::Battle_Camera_world_Y));
		NormalizeVectorToFixedPoint(d, d);
		int16_t p[4];
		ff8fx::copy<(8)>(p, (const void *)game_addr(game::Battle_Camera_LookAt_XZ_s16));
		p[0] = (int16_t)(p[0] + (int16_t)(mul32(d[0], 200) >> 12));
		p[1] = (int16_t)(p[1] + (int16_t)(mul32(d[1], 200) >> 12));
		p[2] = (int16_t)(p[2] + (int16_t)(mul32(d[2], 200) >> 12));
		GteLoadV0(p);
		GteRTPS();
		int32_t otz;
		GteReadOTZ32(&otz);
		return otz;
	}

	static uint32_t __cdecl FlashTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int32_t otz = FlashDepth();
		int32_t level = FlashLevel(n->c, n->e, n->s1C);
		if (level >= 0) ScreenTile(level, level, level, otz);
		if (Paused()) return 0;
		n->c++;
		return level < 0 ? TASK_END : 0;
	}

	static void MorphDraw(const Node24 *n, int32_t sin_arg, bool fade, int32_t fade_value, uint32_t vbuf)
	{
		int16_t ang[4] = { 0, 0, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t sc[3];
		sc[2] = n->s20;
		sc[1] = n->s20;
		sc[0] = n->s1C;
		Scale3DMatrix(&m, sc);
		int32_t d[3];
		d[0] = (int32_t)var<int16_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16)) - (int32_t)var<int16_t>(game_addr(game::Battle_Camera_world_XZ_s16));
		d[1] = (int32_t)var<int16_t>(game_addr(game::unk_B8B7FA)) - (int32_t)var<int16_t>(game_addr(game::unk_B8B7F2));
		d[2] = (int32_t)var<int16_t>(game_addr(game::Battle_Camera_LookAt_Y)) - (int32_t)var<int16_t>(game_addr(game::Battle_Camera_world_Y));
		NormalizeVectorToFixedPoint(d, d);
		int16_t p[4];
		ff8fx::copy<(8)>(p, (const void *)game_addr(game::Battle_Camera_LookAt_XZ_s16));
		p[0] = (int16_t)(p[0] + (int16_t)(mul32(d[0], 200) >> 13));
		p[1] = (int16_t)(p[1] + (int16_t)(mul32(d[1], 200) >> 13));
		p[2] = (int16_t)(p[2] + (int16_t)(mul32(d[2], 200) >> 13));
		MatrixMultiplyVector(&Camera(), p, p);
		m.t[0] = (int32_t)p[0] + Camera().t[0];
		m.t[1] = (int32_t)p[1] + Camera().t[1];
		m.t[2] = (int32_t)p[2] + Camera().t[2];
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = game_addr(game::unk_E148E8);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x2033;
		if (fade)
		{
			*(uint32_t *)(h + 0x1C) = 0x20F3;
			*(int32_t *)(h + 0xC) = fade_value;
		}
		int32_t w = ComputeSin(sin_arg);
		const int16_t *a = (const int16_t *)game_addr(game::unk_E148F0), *b = (const int16_t *)game_addr(game::unk_E15668);
		int16_t *o = (int16_t *)vbuf;
		for (int j = 0; j < 0x7E; j++, a += 4, b += 4, o += 4)
		{
			o[0] = (int16_t)(a[0] + (mul32((int32_t)b[0] - (int32_t)a[0], w) >> 12));
			o[1] = (int16_t)(a[1] + (mul32((int32_t)b[1] - (int32_t)a[1], w) >> 12));
			o[2] = (int16_t)(a[2] + (mul32((int32_t)b[2] - (int32_t)a[2], w) >> 12));
		}
		*(uint32_t *)(h + 4) = vbuf;
		Cursor() = RenderPrimModel(h, OT(), 2, Cursor());
		Cursor() = RenderPrimModel(h, OT(), 2, Cursor());
		FieldFree(0x58);
	}

	static int32_t MorphArg(int32_t c) { return shl32(c, 10) / 12; }

	static uint32_t __cdecl MorphTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int32_t c = n->c;
		MorphDraw(n, MorphArg(c), c >= 6, mul32(c - 6, 682), Scratch());
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0xC ? TASK_END : 0;
	}

	static void LitSetup(const Mat4x3 *frame, uint8_t *s, int32_t tx, int32_t ty, int32_t tz)
	{
		*(int16_t *)(s + 0) = 0;
		*(int16_t *)(s + 2) = 0;
		*(int16_t *)(s + 4) = 0;
		ComposeZYXRotationMatrix((const int16_t *)s, (Mat4x3 *)(s + 8));
		*(int32_t *)(s + 0x1C) = tx;
		*(int32_t *)(s + 0x20) = ty;
		*(int32_t *)(s + 0x24) = tz;
		ComposeAffineTransform(frame, (const Mat4x3 *)(s + 8), (Mat4x3 *)(s + 8));
		GteSetLightMatrix(s + 8);
		GteSetBackColorFromTrans(s + 8);
	}

	static uint32_t __cdecl SmokeTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		*(uint32_t *)h = game_addr(game::unk_E10FB0);
		*(int16_t *)(h + 0x24) = 0;
		int32_t count = 0;
		LitSetup(&Frame(), s, 0, 0, 0x28);
		for (int i = 0; i < 150; i++)
		{
			Rec *r = RecOf(PoolA(), i);
			if (!(r->alive & 2)) continue;
			LitSprite(h, &r->x, r->size, r->age);
			if (Paused()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				r->alive = 0;
				continue;
			}
			Integrate(INT_DRAG4, r);
			count++;
		}
		FieldFree(0x48);
		FieldFree(0xB4);
		if (Paused()) return 0;
		FieldAlloc(0x48);
		int16_t c = n->c;
		if (c >= 0 && c <= 3)
		{
			for (int m = 0; m < 10; m++)
			{
				int i = FreeRecord(PoolA());
				if (i < 0) break;
				Rec *r = RecOf(PoolA(), i);
				r->alive = 2;
				r->age = 0;
				r->size = (int16_t)(CrtRand() % 0x160 + 0x80);
				int32_t a = CrtRand() % 0x1000;
				int32_t rad = CrtRand() % 0x96 + 0xC8;
				r->x = (int16_t)(mul32(ComputeCos(a), rad) >> 12);
				r->y = (int16_t)-(CrtRand() % 100);
				r->z = (int16_t)(mul32(ComputeSin(a), rad) >> 12);
				int32_t sp = CrtRand() % 0x37 + 0x23;
				r->vx = (int16_t)(mul32(ComputeCos(a), sp) >> 12);
				r->vy = (int16_t)(-50 - CrtRand() % 0x7D);
				r->vz = (int16_t)(mul32(ComputeSin(a), sp) >> 12);
			}
		}
		FieldFree(0x48);
		n->c++;
		if (n->c < 4 || count != 0) return 0;
		return TASK_END;
	}

	static void RingDraw(const Mat4x3 *frame, const Node24 *n, int32_t sx, int32_t sy, bool fade, int32_t fade_value)
	{
		int16_t ang[4] = { 0, n->a18, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = n->p10;
		m.t[1] = n->p12;
		int32_t sc[3];
		sc[2] = sx;
		sc[0] = sx;
		m.t[2] = n->p14;
		sc[1] = sy;
		Scale3DMatrix(&m, sc);
		ComposeAffineTransform(frame, &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = game_addr(game::unk_E18150);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		if (fade)
		{
			*(uint32_t *)(h + 0x1C) = 0xF3;
			*(int32_t *)(h + 0xC) = fade_value;
		}
		Cursor() = RenderPrimModel(h, OT(), 2, Cursor());
		FieldFree(0x58);
	}

	static int32_t RingAFade(int32_t c) { return mul32(c - 6, 682); }
	static int32_t RingBFade(int32_t c) { return shl32(c - 4, 9); }

	static uint32_t __cdecl RingATask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int16_t c = n->c;
		RingDraw(&Frame(), n, n->s1C, n->s20, c >= 6, RingAFade(c));
		if (Paused()) return 0;
		int16_t v = n->s22;
		n->s20 = (int16_t)(n->s20 + v);
		n->c++;
		n->s22 = (int16_t)(v - (int16_t)((int32_t)v / 4));
		return n->c >= 0xC ? TASK_END : 0;
	}

	static uint32_t __cdecl RingBTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int16_t c = n->c;
		RingDraw(&Frame(), n, n->s1C, n->s20, c >= 4, RingBFade(c));
		if (Paused()) return 0;
		int16_t v = n->s1E;
		n->s1C = (int16_t)(n->s1C + v);
		n->c++;
		n->s1E = (int16_t)(v - (int16_t)((int32_t)v / 4));
		return n->c >= 0xC ? TASK_END : 0;
	}

	static uint32_t __cdecl DustTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		*(uint32_t *)h = game_addr(game::unk_E108E8);
		*(int16_t *)(h + 0x24) = 0;
		int32_t count = 0;
		LitSetup(&Frame(), s, 0, -40, 0x28);
		for (int i = 0; i < 150; i++)
		{
			Rec *r = RecOf(PoolB(), i);
			if (!(r->alive & 2)) continue;
			LitSprite(h, &r->x, r->size, r->age);
			if (Paused()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				r->alive = 0;
				continue;
			}
			Integrate(INT_XZ_DRAG8, r);
			count++;
		}
		FieldFree(0x48);
		FieldFree(0xB4);
		if (Paused()) return 0;
		FieldAlloc(0x48);
		int16_t c = n->c;
		if (c >= 0 && c <= 1)
		{
			for (int m = 0; m < 12; m++)
			{
				int i = FreeRecord(PoolB());
				if (i < 0) break;
				Rec *r = RecOf(PoolB(), i);
				r->alive = 2;
				r->age = 0;
				r->size = (int16_t)(CrtRand() % 0x280 + 0x600);
				int32_t a = CrtRand() % 0x1000;
				int32_t rad = CrtRand() % 0x3C + 0xAA;
				r->x = (int16_t)(mul32(ComputeCos(a), rad) >> 12);
				r->y = 0;
				r->z = (int16_t)(mul32(ComputeSin(a), rad) >> 12);
				int32_t sp = CrtRand() % 0x14 + 0xF;
				r->vx = (int16_t)(mul32(ComputeCos(a), sp) >> 12);
				r->vz = (int16_t)(mul32(ComputeSin(a), sp) >> 12);
			}
		}
		FieldFree(0x48);
		n->c++;
		if (n->c < 4 || count != 0) return 0;
		return TASK_END;
	}

	static uint32_t __cdecl MistTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		*(uint32_t *)h = game_addr(game::unk_E108E8);
		*(uint32_t *)(h + 0x1C) = 0x404040;
		*(int16_t *)(h + 0x24) = 4;
		int32_t count = 0;
		*(int16_t *)(s + 0) = 0;
		*(int16_t *)(s + 2) = 0;
		*(int16_t *)(s + 4) = 0;
		ComposeZYXRotationMatrix((const int16_t *)s, (Mat4x3 *)(s + 8));
		GetEffectSpawnPosition(Creature(), 0xF0, 0, (int16_t *)s);
		*(int32_t *)(s + 0x1C) = *(int16_t *)(s + 0);
		*(int32_t *)(s + 0x20) = *(int16_t *)(s + 2) + 0x14;
		*(int32_t *)(s + 0x24) = *(int16_t *)(s + 4) - 0x32;
		ComposeAffineTransform(&Frame(), (const Mat4x3 *)(s + 8), (Mat4x3 *)(s + 8));
		GteSetLightMatrix(s + 8);
		GteSetBackColorFromTrans(s + 8);
		for (int i = 0; i < 150; i++)
		{
			Rec *r = RecOf(PoolA(), i);
			if (!(r->alive & 4)) continue;
			LitSprite(h, &r->x, r->size, r->age);
			if (Paused()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				r->alive = 0;
				continue;
			}
			Integrate(INT_GROW5_DRAG8, r);
			count++;
		}
		FieldFree(0x48);
		FieldFree(0xB4);
		if (Paused()) return 0;
		uint8_t *t = (uint8_t *)FieldAlloc(0x48);
		int16_t c = n->c;
		if (c >= 0 && c <= 7)
		{
			for (int m = 0; m < 2; m++)
			{
				int i = FreeRecord(PoolA());
				if (i < 0) break;
				Rec *r = RecOf(PoolA(), i);
				r->alive = 4;
				r->age = 0;
				r->size = (int16_t)(CrtRand() % 0x80 + 0x200);
				r->x = (int16_t)(CrtRand() % 10 - 5);
				r->y = (int16_t)(CrtRand() % 10 - 5);
				r->z = (int16_t)(CrtRand() % 10 - 5);
				int32_t sp = CrtRand() % 8 + 0xF;
				int32_t *v = (int32_t *)(t + 0x28);
				v[0] = CrtRand() % 0x200 - 0x100;
				*(int32_t *)(t + 0x2C) = CrtRand() % 0x400 - 0x180;
				int32_t z = CrtRand() % 0x200;
				*(int32_t *)(t + 0x30) = -0xE00 - z;
				NormalizeVectorToFixedPoint(v, v);
				r->vx = (int16_t)(mul32(v[0], sp) >> 12);
				r->vy = (int16_t)(mul32(*(int32_t *)(t + 0x2C), sp) >> 12);
				r->vz = (int16_t)(mul32(*(int32_t *)(t + 0x30), sp) >> 12);
			}
		}
		FieldFree(0x48);
		n->c++;
		if (n->c < 4 || count != 0) return 0;
		return TASK_END;
	}

	static int32_t BoneHistory(uint32_t table, int32_t bone, int32_t c, uint8_t *s)
	{
		GetEffectSpawnPosition(Creature(), bone, 0, (int16_t *)s);
		GetEffectSpawnPosition(Creature(), bone, 0x1000, (int16_t *)(s + 8));
		int16_t *a = (int16_t *)s, *b = (int16_t *)(s + 8);
		int16_t d0 = (int16_t)(b[0] - a[0]), d1 = (int16_t)(b[1] - a[1]), d2 = (int16_t)(b[2] - a[2]);
		a[0] = d0;
		a[1] = d1;
		a[2] = d2;
		NormalizeVector16(a, a);
		MatrixMultiplyVector(&RootMatrix(), a, a);
		int32_t idx = c % 3;
		uint32_t *t = (uint32_t *)(table + 0x14 * idx);
		t[0] = 1;
		t[1] = *(uint32_t *)(s + 8);
		t[2] = *(uint32_t *)(s + 0xC);
		t[3] = *(uint32_t *)(s + 0);
		t[4] = *(uint32_t *)(s + 4);
		int32_t n = 0;
		for (int32_t j = idx; n < 3; n++)
		{
			const uint32_t *e = (const uint32_t *)(table + 0x14 * j);
			if (!e[0]) break;
			var<uint32_t>(game_addr(HIST_A) + 8 * n) = e[1];
			var<uint32_t>(game_addr(HIST_A) + 8 * n + 4) = e[2];
			var<uint32_t>(game_addr(HIST_B) + 8 * n) = e[3];
			var<uint32_t>(game_addr(HIST_B) + 8 * n + 4) = e[4];
			if (--j < 0) j = 2;
		}
		return n;
	}

	static void SparkSetup(const Mat4x3 *frame, uint8_t *h, uint8_t *s, uint32_t seq, uint32_t color)
	{
		ff8fx::copy<(0x20)>(s + 0x10, frame);
		*(uint32_t *)h = seq;
		*(uint32_t *)(h + 0x1C) = color;
		*(int16_t *)(h + 0x24) = 0xC;
		GteSetLightMatrix(s + 0x10);
		GteSetBackColorFromTrans(s + 0x10);
	}

	static int32_t SparkDrawPool(const void *owner, uint8_t *h, uint32_t mask, int kind)
	{
		int32_t count = 0;
		for (int i = 0; i < 150; i++)
		{
			Rec *r = RecOf(PoolC(), i);
			if (!(r->alive & mask)) continue;
			LitSprite(h, &r->x, r->size, r->age);
			if (Paused()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				r->alive = 0;
				continue;
			}
			Integrate(kind, r);
			count++;
		}
		return count;
	}

	static void SplineRails(int32_t n, int32_t points, int32_t div)
	{
		uint8_t *w = (uint8_t *)FieldAlloc(0x190);
		SplineSetup(n, game_addr(HIST_A), w);
		for (int32_t i = 0, t = 0; i < points; i++, t += 0x1000) SplineEval(n, w, game_addr(SPLINE_A) + 8 * i, t / div);
		SplineSetup(n, game_addr(HIST_B), w);
		for (int32_t i = 0, t = 0; i < points; i++, t += 0x1000) SplineEval(n, w, game_addr(SPLINE_B) + 8 * i, t / div);
		FieldFree(0x190);
	}

	static uint32_t __cdecl SparkTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x50);
		int32_t hist = BoneHistory(game_addr(game::unk_24F0C88), 0x28, n->c, s);
		SparkSetup(&Frame(), h, s, game_addr(game::unk_E108E8), 0x404040);
		int32_t count = SparkDrawPool(n, h, 1, INT_GROW5_DRAG8);
		FieldFree(0x50);
		FieldFree(0xB4);
		if (Paused()) return 0;
		int16_t c = n->c;
		if (c >= 0 && c <= 0xB && hist > 1)
		{
			SplineRails(hist, 6, 5);
			for (int j = 0; j < 5; j++)
			{
				int i = FreeRecord(PoolC());
				if (i < 0) break;
				Rec *r = RecOf(PoolC(), i);
				r->alive = 1;
				r->age = 0;
				int32_t sz = CrtRand() % 0x480;
				uint32_t pa = var<uint32_t>(game_addr(SPLINE_A) + 8 * j), pb = var<uint32_t>(game_addr(SPLINE_A) + 8 * j + 4);
				r->size = (int16_t)(sz + 0x380);
				*(uint32_t *)&r->x = pa;
				*(uint32_t *)&r->z = pb;
				r->x = (int16_t)(r->x + (int16_t)(CrtRand() % 10 - 5));
				r->y = (int16_t)(r->y + (int16_t)(CrtRand() % 10 - 5));
				r->z = (int16_t)(r->z + (int16_t)(CrtRand() % 10 - 5));
				int32_t sp = CrtRand() % 0xF + 0x19;
				const int16_t *d = (const int16_t *)(game_addr(SPLINE_B) + 8 * j);
				int32_t k = CrtRand() % 10;
				r->vx = (int16_t)(k + (mul32(d[0], sp) >> 12) - 5);
				k = CrtRand() % 10;
				r->vy = (int16_t)(k + (mul32(d[1], sp) >> 12) - 5);
				k = CrtRand() % 10;
				r->vz = (int16_t)(k + (mul32(d[2], sp) >> 12) - 5);
			}
		}
		n->c++;
		if (n->c < 4 || count != 0) return 0;
		return TASK_END;
	}

	static uint32_t __cdecl Spark2Task(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x50);
		uint32_t table = var<uint32_t>(game_addr(game::unk_E196A4) + 4 * (int32_t)n->a18);
		int32_t hist = BoneHistory(table, n->e, n->c, s);
		SparkSetup(&Frame(), h, s, game_addr(game::unk_E10E04), 0x202020);
		int32_t count = SparkDrawPool(n, h, 2, INT_GROW6_DRAG8);
		FieldFree(0x50);
		FieldFree(0xB4);
		if (Paused()) return 0;
		int16_t c = n->c;
		if (c >= 0 && c <= 0xB && hist > 1)
		{
			SplineRails(hist, 4, 3);
			for (int j = 0; j < 3; j++)
			{
				int i = FreeRecord(PoolC());
				if (i < 0) break;
				Rec *r = RecOf(PoolC(), i);
				r->alive = 2;
				r->age = 0;
				int32_t sz = CrtRand() % 0x400;
				uint32_t pa = var<uint32_t>(game_addr(SPLINE_A) + 8 * j), pb = var<uint32_t>(game_addr(SPLINE_A) + 8 * j + 4);
				r->size = (int16_t)(sz + 0x100);
				*(uint32_t *)&r->x = pa;
				*(uint32_t *)&r->z = pb;
				r->x = (int16_t)(r->x + (int16_t)(CrtRand() % 20 - 10));
				r->y = (int16_t)(r->y + (int16_t)(CrtRand() % 20 - 10));
				r->z = (int16_t)(r->z + (int16_t)(CrtRand() % 20 - 10));
				int32_t sp = CrtRand() % 0xF + 0x19;
				const int16_t *d = (const int16_t *)(game_addr(SPLINE_B) + 8 * j);
				int32_t k = CrtRand() % 20;
				r->vx = (int16_t)(k + (mul32(d[0], sp) >> 12) - 10);
				k = CrtRand() % 20;
				r->vy = (int16_t)(k + (mul32(d[1], sp) >> 12) - 10);
				k = CrtRand() % 20;
				r->vz = (int16_t)(k + (mul32(d[2], sp) >> 12) - 10);
			}
		}
		n->c++;
		if (n->c < 4 || count != 0) return 0;
		return TASK_END;
	}

	static void BladeDraw(const Mat4x3 *frame, const Mat4x3 *emat, const Mat4x3 *bone, int16_t *p0, int16_t *p1, bool draw)
	{
		int16_t v[4] = { 0x400, 0, 0x800, 0 };
		Mat4x3 m1, m2;
		ComposeZYXRotationMatrix(v, &m1);
		int32_t sc[3] = { 0x1A00, 0x1A00, 0x1A00 };
		m1.t[0] = 0;
		m1.t[1] = 0;
		m1.t[2] = 0;
		Scale3DMatrix(&m1, sc);
		ComposeAffineTransform(emat, bone, &m2);
		GteMatrixMultiply(&m2, &m1);
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		TransformVectorBy3x3Matrix(&m2, m1.t, m1.t);
		int32_t t0 = (int32_t)((uint32_t)m1.t[0] + (uint32_t)m2.t[0]);
		int32_t t1 = (int32_t)((uint32_t)m1.t[1] + (uint32_t)m2.t[1]);
		int32_t t2 = (int32_t)((uint32_t)m1.t[2] + (uint32_t)m2.t[2]);
		m1.t[2] = t2;
		m1.t[0] = t0;
		m1.t[1] = t1;
		v[0] = 0;
		v[1] = 1000;
		v[2] = -1000;
		MatrixMultiplyVector(&m1, v, p0);
		p0[0] = (int16_t)(p0[0] + (int16_t)t0);
		p0[1] = (int16_t)(p0[1] + (int16_t)t1);
		p0[2] = (int16_t)(p0[2] + (int16_t)t2);
		v[0] = 0;
		v[1] = 0x1838;
		v[2] = 0x258;
		MatrixMultiplyVector(&m1, v, p1);
		p1[0] = (int16_t)(p1[0] + (int16_t)t0);
		p1[1] = (int16_t)(p1[1] + (int16_t)t1);
		p1[2] = (int16_t)(p1[2] + (int16_t)t2);
		ComposeAffineTransform(frame, &m1, &m1);
		GteSetRotMatrix(&m1);
		GteSetTransVector(&m1);
		int32_t o0, o1;
		GteLoadV0(p0);
		GteRTPS();
		GteReadSXY2(v);
		GteReadOTZ32(&o0);
		GteLoadV0(p1);
		GteRTPS();
		GteReadSXY2(v);
		GteReadOTZ32(&o1);
		int32_t o = o0 < o1 ? o0 : o1;
		o = o / 4 - 8;
		if (o < 0) o = 0;
		if (!draw) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = game_addr(game::unk_E18B38);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		Cursor() = RenderPrimModel(h, var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 4 * (uint32_t)o, 0xE, Cursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl BladeTask(TaskNode *tn)
	{
		if (!(var<uint8_t>(game_addr(game::unk_24F12E4)) & 4)) return TASK_END;
		Node24 *n = (Node24 *)tn;
		BladeDraw(&Frame(), &CreatureMat(), &BoneCopy(), BladeP0(), BladeP1(), n->c > 0);
		if (Paused()) return 0;
		uint8_t *sk = **(uint8_t ***)(game_addr(game::unk_24F12CC));
		n->c++;
		ff8fx::copy<(0x20)>(&BoneCopy(), sk + 0xB30);
		return 0;
	}

	static void TrailDraw(const Mat4x3 *frame, uint32_t rail_a, uint32_t rail_b, uint8_t *s)
	{
		GteSetRotMatrix(frame);
		GteSetTransVector(frame);
		GteSetFarColor2(0, 0, 0);
		uint32_t pk = Cursor();
		*(uint32_t *)(s + 0x14) = 0x3A903030;
		*(uint32_t *)(s + 0x1C) = 0x3A903030;
		uint32_t pk2 = pk + 0x24;
		int32_t acc = 0;
		for (uint32_t j = 0; j < 0xF0; j += 8, acc += 0x1000)
		{
			*(uint32_t *)(s + 0x10) = *(uint32_t *)(s + 0x14);
			uint32_t *a = (uint32_t *)pk, *b = (uint32_t *)pk2;
			a[0] = 0x8000000;
			GteLoadV012((const void *)(rail_a + j), (const void *)(rail_a + j + 8), (const void *)(rail_b + j));
			GteRTPT();
			GteReadFLAG(s + 0xC);
			if (*(uint32_t *)(s + 0xC) & 0x60000) continue;
			GteReadSXY012Split(a + 2, a + 4, a + 6);
			GteLoadV0((const void *)(rail_b + j + 8));
			GteRTPS();
			GteReadSXY2(a + 8);
			GteAVSZ4();
			GteReadOTZ32(s + 8);
			GteSetIR0(acc / 30);
			GteLoadRGBC(s + 0x1C);
			GteDPCS();
			GteStoreRGB2(s + 0x14);
			uint32_t c0 = *(uint32_t *)(s + 0x10), c1 = *(uint32_t *)(s + 0x14);
			a[5] = c0;
			a[1] = c0;
			a[7] = c1;
			a[3] = c1;
			InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 4 * (uint32_t)(*(int32_t *)(s + 8) >> 2) + 0x44, a);
			b[5] = *(uint32_t *)(s + 0x10);
			b[3] = 0x3A000000;
			b[1] = 0x3A000000;
			b[2] = a[2];
			b[7] = *(uint32_t *)(s + 0x14);
			b[4] = a[4];
			b[6] = a[6];
			b[8] = a[8];
			b[0] = 0x8000000;
			InsertPrimAutoDepth(var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 4 * (uint32_t)(*(int32_t *)(s + 8) >> 2) + 0x44, b);
			pk += 0x48;
			pk2 += 0x48;
		}
		Cursor() = pk2;
	}

	static uint32_t __cdecl TrailTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *s = (uint8_t *)FieldAlloc(0x40);
		const int16_t *p0 = BladeP0(), *p1 = BladeP1();
		int32_t k = var<int32_t>(game_addr(game::unk_E19674) + 4 * (int32_t)n->c);
		int32_t *d = (int32_t *)(s + 0x30);
		d[0] = (int32_t)p1[0] - (int32_t)p0[0];
		d[1] = (int32_t)p1[1] - (int32_t)p0[1];
		d[2] = (int32_t)p1[2] - (int32_t)p0[2];
		int32_t len = Sqrt(NormalizeVectorToFixedPoint(d, d));
		len = mul32(len, k) >> 12;
		uint32_t w0 = var<uint32_t>(game_addr(game::unk_24F1310)), w1 = var<uint32_t>(game_addr(game::unk_24F1314));
		*(int16_t *)(s + 0x28) = (int16_t)((mul32(d[0], len) >> 12) + (int32_t)w0);
		*(int16_t *)(s + 0x2A) = (int16_t)((int16_t)(mul32(len, d[1]) >> 12) + p0[1]);
		*(int16_t *)(s + 0x2C) = (int16_t)((mul32(d[2], len) >> 12) + (int32_t)w1);
		int32_t idx = (int32_t)n->c % 10;
		uint32_t *rec = (uint32_t *)(game_addr(TRAIL_RECS) + 0x14 * idx);
		rec[0] = 1;
		rec[1] = w0;
		rec[2] = w1;
		rec[3] = *(uint32_t *)(s + 0x28);
		rec[4] = *(uint32_t *)(s + 0x2C);
		int32_t cnt = 0;
		for (int32_t j = idx; cnt < 10; cnt++)
		{
			const uint32_t *e = (const uint32_t *)(game_addr(TRAIL_RECS) + 0x14 * j);
			if (!e[0]) break;
			var<uint32_t>(game_addr(HIST_A) + 8 * cnt) = e[1];
			var<uint32_t>(game_addr(HIST_A) + 8 * cnt + 4) = e[2];
			const uint32_t *src = cnt == 0 ? (const uint32_t *)game_addr(game::MAG_326_Tip) : e + 3;
			var<uint32_t>(game_addr(HIST_B) + 8 * cnt) = src[0];
			var<uint32_t>(game_addr(HIST_B) + 8 * cnt + 4) = src[1];
			if (--j < 0) j = 9;
		}
		if (cnt > 1)
		{
			SplineRails(cnt, 32, 31);
			TrailDraw(&Frame(), game_addr(SPLINE_A), game_addr(SPLINE_B), s);
		}
		FieldFree(0x40);
		if (Paused()) return 0;
		n->c++;
		return n->c >= 6 ? TASK_END : 0;
	}

	static void GhostPose(Mat4x3 *m, int16_t a)
	{
		int16_t ang[4] = { a, 0, 0, 0 };
		ComposeZYXRotationMatrix(ang, m);
		int32_t sc[3] = { 0x1000, 0x1000, 0x1000 };
		Scale3DMatrix(m, sc);
	}

	static void GhostEchoes(uint8_t *E, int32_t k, const Mat4x3 *frame, uint32_t scratch)
	{
		uint32_t col = 0x2606060;
		for (int32_t i = 0, j = k; i < 6; i++)
		{
			const uint32_t *r = (const uint32_t *)(game_addr(GHOST_RECS) + 0x24 * j);
			if (!r[0]) break;
			ff8fx::copy<(0x20)>(&Model2Mat(), r + 1);
			var<uint32_t>(game_addr(game::unk_24F0F10)) = col;
			DrawModel(E, frame, scratch);
			col -= 0xC0C0C;
			if (--j < 0) j = 5;
		}
	}

	static uint32_t __cdecl GhostTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *E = Model2();
		if (n->c == 0)
		{
			ResetRotation((void *)game_addr(game::MAG_326_Model2Mat));
			var<uint16_t>(game_addr(game::unk_24F0F38)) = 0x1000;
			var<uint16_t>(game_addr(game::unk_24F0F30)) = 0x1000;
			var<uint16_t>(game_addr(game::MAG_326_Model2Mat)) = 0x1000;
			var<int32_t>(game_addr(game::unk_24F0F3C)) = 0xEA6;
			var<int32_t>(game_addr(game::unk_24F0F40)) = (int32_t)0xFFFFEC78;
			var<int32_t>(game_addr(game::unk_24F0F44)) = 0x1518;
			ModelInit(E, (uint8_t *)game_addr(game::unk_24F2140), (uint8_t *)game_addr(game::unk_E18CF8), 0x10);
			SetAnim(E, 1);
			n->p10 = (int16_t)0xFE89;
			n->p12 = (int16_t)0xFD3C;
			n->p14 = (int16_t)0xFD1B;
			n->a18 = (int16_t)(CrtRand() % 0x1000);
			n->s1A = 0x300;
			for (uint32_t a = game_addr(GHOST_RECS); a < game_addr(game::MAG_326_VoiceSlot); a += 0x24) *(uint32_t *)a = 0;
		}
		if (!Paused())
		{
			Mat4x3 &m = Model2Mat();
			m.t[0] = (int32_t)((uint32_t)m.t[0] + (uint32_t)(int32_t)n->p10);
			m.t[2] = (int32_t)((uint32_t)m.t[2] + (uint32_t)(int32_t)n->p14);
			m.t[1] = (int32_t)((uint32_t)m.t[1] + (uint32_t)(int32_t)n->p12);
			n->a18 = (int16_t)(n->a18 + n->s1A);
			n->c++;
		}
		GhostPose(&Model2Mat(), n->a18);
		DrawModel(E, &Frame(), Scratch());
		int32_t k = (int32_t)n->c % 6;
		uint32_t *rec = (uint32_t *)(game_addr(GHOST_RECS) + 0x24 * k);
		rec[0] = 1;
		ff8fx::copy<(0x20)>(rec + 1, &Model2Mat());
		Mat4x3 save = Model2Mat();
		GhostEchoes(E, k, &Frame(), Scratch());
		Model2Mat() = save;
		var<uint32_t>(game_addr(game::unk_24F0F10)) = 0x808080;
		return n->c >= 0x1E ? TASK_END : 0;
	}

	static void SwordDraw(const Mat4x3 *frame, const Mat4x3 *emat, bool draw)
	{
		int16_t v[4] = { 0, 0, 0, 0 };
		Mat4x3 m1, m2;
		ComposeZYXRotationMatrix(v, &m1);
		int32_t sc[3] = { 0x5400, 0x5400, 0x5400 };
		m1.t[0] = 0;
		m1.t[1] = 0;
		m1.t[2] = 0;
		Scale3DMatrix(&m1, sc);
		uint8_t *sk = **(uint8_t ***)(game_addr(game::unk_24F0F4C));
		ff8fx::copy<(0x20)>(&BoneCopy(), sk + 0x20);
		ComposeAffineTransform(emat, &BoneCopy(), &m2);
		GteMatrixMultiply(&m2, &m1);
		TransformVectorBy3x3Matrix(&m2, m1.t, m1.t);
		int32_t t1 = (int32_t)((uint32_t)m1.t[1] + (uint32_t)m2.t[1]);
		int32_t t2 = (int32_t)((uint32_t)m1.t[2] + (uint32_t)m2.t[2]);
		int32_t t0 = (int32_t)((uint32_t)m1.t[0] + (uint32_t)m2.t[0]);
		m1.t[1] = t1;
		m1.t[2] = t2;
		m1.t[0] = t0;
		ComposeAffineTransform(frame, &m1, &m1);
		GteSetRotMatrix(&m1);
		GteSetTransVector(&m1);
		if (!draw) return;
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = game_addr(game::unk_E18B38);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		Cursor() = RenderPrimModel(h, OT(), 3, Cursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl SwordTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		SwordDraw(&Frame(), &Model2Mat(), n->c > 0);
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x1E ? TASK_END : 0;
	}

	static int32_t FadeW(int32_t c) { return 0x1000 - ComputeSin(shl32(c - 0x13, 10) / 8); }

	static uint32_t FadeTask(Node24 *n, uint8_t *E, uint32_t mat, uint32_t data, uint32_t buf_ptr)
	{
		if (n->c == 0)
		{
			ResetRotation((void *)mat);
			uint8_t *buf = buf_ptr ? var<uint8_t *>(buf_ptr) : (uint8_t *)game_addr(game::unk_E18CF8);
			var<uint16_t>(mat + 0x10) = 0x1000;
			var<uint16_t>(mat + 8) = 0x1000;
			var<uint16_t>(mat + 0) = 0x1000;
			var<int32_t>(mat + 0x14) = (int32_t)0xFFFFFF06;
			var<int32_t>(mat + 0x18) = (int32_t)0xFFFFF8F8;
			var<int32_t>(mat + 0x1C) = 0x258;
			ModelInit(E, (uint8_t *)data, buf, 0x10);
			E[0] |= 1;
			SetAnim(E, 0);
		}
		else if (!Paused()) AdvanceModelAnimLoop(E);
		int16_t c = n->c;
		if (c >= 0x13)
		{
			int32_t w = FadeW(c);
			DepthCue(w, 0x80, 0x80, 0x80, E + 0x28);
		}
		DrawModel(E, &Frame(), Scratch());
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x1B ? TASK_END : 0;
	}

	static uint32_t __cdecl FadeATask(TaskNode *tn) { return FadeTask((Node24 *)tn, Model2(), game_addr(game::MAG_326_Model2Mat), game_addr(game::unk_24F2140), 0); }
	static uint32_t __cdecl FadeBTask(TaskNode *tn) { return FadeTask((Node24 *)tn, Model3(), game_addr(game::unk_24F0D90), game_addr(game::unk_24F22A8), game_addr(game::unk_24F22E4)); }

	static void TintColor(int32_t c, uint8_t *L)
	{
		L[0] = 0x70;
		L[1] = 0x80;
		L[2] = 0xF0;
		if (c >= 0x13)
		{
			int32_t w = FadeW(c);
			uint8_t out[4];
			DepthCue(w, L[0], L[1], L[2], out);
			ff8fx::copy<(4)>(L, out);
		}
	}

	static uint32_t __cdecl TintTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t L[4];
		TintColor(n->c, L);
		ScreenTile(L[0], L[1], L[2], 0x1000);
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x1B ? TASK_END : 0;
	}

	static void BurstDraw(const Node24 *n, int32_t c, bool b, int32_t scroll, int32_t fade)
	{
		int32_t d[3];
		int32_t px, py, pz;
		if (b)
		{
			px = var<int16_t>(game_addr(game::g_CamScript_SavedLookAt));
			py = var<int16_t>(game_addr(game::unk_24FD252));
			pz = var<int16_t>(game_addr(game::unk_24FD254));
		}
		else
		{
			px = n->p10;
			py = n->p12;
			pz = n->p14;
		}
		d[0] = (int32_t)var<int16_t>(game_addr(game::g_CamScript_SavedEye)) - px;
		d[1] = (int32_t)var<int16_t>(game_addr(game::unk_24FD35A)) - py;
		d[2] = (int32_t)var<int16_t>(game_addr(game::unk_24FD35C)) - pz;
		NormalizeVectorToFixedPoint(d, d);
		int32_t axis[4];
		int32_t angle = RotationBetweenVectors(game_addr(game::unk_E19740), d, axis);
		Mat4x3 m1, m2;
		AxisAngleMatrix(angle, &m1, axis);
		int16_t ang[4] = { 0, 0, n->a18, 0 };
		ComposeZYXRotationMatrix(ang, &m2);
		GteMatrixMultiply(&m1, &m2);
		if (b)
		{
			m2.t[1] = var<int16_t>(game_addr(game::unk_24FD252));
			m2.t[0] = var<int16_t>(game_addr(game::g_CamScript_SavedLookAt));
			m2.t[2] = var<int16_t>(game_addr(game::unk_24FD254));
		}
		else
		{
			m2.t[1] = n->p12;
			m2.t[0] = n->p10;
			m2.t[2] = n->p14;
		}
		int32_t sc[3];
		sc[1] = n->s1C;
		sc[0] = n->s1C;
		sc[2] = n->s20;
		Scale3DMatrix(&m2, sc);
		ComposeAffineTransform(&Camera(), &m2, &m2);
		GteSetRotMatrix(&m2);
		GteSetTransVector(&m2);
		uint8_t *h = (uint8_t *)FieldAlloc(0x90);
		*(uint32_t *)h = b ? game_addr(game::unk_E13040) : game_addr(game::unk_E11798);
		*(uint32_t *)(h + 8) = 0;
		*(uint16_t *)(h + 0x18) = 0;
		*(uint16_t *)(h + 0x1A) = 0;
		*(uint32_t *)(h + 0x10) = 0;
		*(uint32_t *)(h + 0x20) = 0x33;
		*(int32_t *)(h + 0x14) = scroll;
		*(uint16_t *)(h + 0x1C) = 0x40;
		*(uint16_t *)(h + 0x1E) = 0x80;
		*(uint32_t *)(h + 0x24) = var<uint32_t>((b ? game_addr(game::unk_E197B0) : game_addr(game::unk_E19790)) + 4 * (c % 8));
		if (c >= (b ? 7 : 6))
		{
			*(uint32_t *)(h + 0x20) = 0xF3;
			*(int32_t *)(h + 0xC) = fade;
		}
		Cursor() = Mag066Draw(h, OT(), 2, Cursor());
		FieldFree(0x90);
	}

	static int32_t BurstFade(bool b, int32_t c) { return b ? shl32(c - 7, 10) : mul32(c - 6, 682); }

	static uint32_t __cdecl BurstATask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int32_t c = n->c;
		BurstDraw(n, c, false, shl32(c, 4), BurstFade(false, c));
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0xC ? TASK_END : 0;
	}

	static uint32_t __cdecl BurstBTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int32_t c = n->c;
		BurstDraw(n, c, true, shl32(c, 4), BurstFade(true, c));
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0xB ? TASK_END : 0;
	}

	static void BloodSetup(const Mat4x3 *frame, uint8_t *h, uint8_t *s)
	{
		*(int16_t *)(h + 0x24) = 8;
		ff8fx::copy<(0x20)>(s + 8, frame);
		*(uint32_t *)h = game_addr(game::unk_E10E04);
		GteSetLightMatrix(s + 8);
		GteSetBackColorFromTrans(s + 8);
	}

	static uint32_t __cdecl BloodTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *pool = PoolC();
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		if (!Paused() && n->c <= 6)
		{
			*(uint32_t *)(s + 0) = *(const uint32_t *)&n->p10;
			*(uint32_t *)(s + 4) = *(const uint32_t *)&n->p14;
			int32_t sign = *(int16_t *)s < 0 ? -1 : 1;
			for (int m = 0; m < 6; m++)
			{
				int i = FreeRecord(pool);
				if (i < 0) break;
				Rec *r = RecOf(pool, i);
				r->alive = (uint32_t)(int32_t)n->e;
				r->age = 0;
				int32_t sz = CrtRand() % 0x600;
				r->size = (int16_t)(sz + 0x200);
				*(uint32_t *)&r->x = *(uint32_t *)(s + 0);
				*(uint32_t *)&r->z = *(uint32_t *)(s + 4);
				int32_t *v = (int32_t *)(s + 0x28);
				v[0] = mul32(CrtRand() % 0x800, sign);
				v[1] = 0;
				v[2] = CrtRand() % 0x2000 - 0x1000;
				NormalizeVectorToFixedPoint(v, v);
				int32_t rad = CrtRand() % 0x190 + 0x12C;
				r->x = (int16_t)(r->x + (int16_t)(mul32(v[0], rad) >> 12));
				r->y = (int16_t)-(CrtRand() % 100);
				r->z = (int16_t)(r->z + (int16_t)(mul32(v[2], rad) >> 12));
				int32_t sp = CrtRand() % 100 + 0x5A;
				r->vx = (int16_t)(mul32(v[0], sp) >> 12);
				int32_t up = CrtRand() % 0x19;
				int32_t vz = mul32(v[2], sp) >> 12;
				r->vy = (int16_t)(-15 - up);
				r->vz = (int16_t)vz;
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		BloodSetup(&Frame(), h, s);
		int32_t count = 0;
		uint32_t mask = (uint32_t)(int32_t)n->e;
		for (int i = 0; i < 150; i++)
		{
			Rec *r = RecOf(pool, i);
			if (!(mask & r->alive)) continue;
			LitSprite(h, &r->x, r->size, r->age, true);
			if (Paused()) continue;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				r->alive = 0;
				continue;
			}
			r->age++;
			Integrate(INT_BLOOD, r);
			count++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (Paused()) return 0;
		n->c++;
		return count != 0 ? 0 : TASK_END;
	}

	static int32_t SlashFade(int32_t c, int32_t s20, int32_t base)
	{
		int32_t span = 0x1000 - base;
		if (c < 8) return 0x1000 - mul32(span / 8, c);
		int32_t out = s20 - 8;
		if (c >= out) return mul32(c - out, span / 8) + base;
		return base;
	}

	static void SlashDraw(const Node24 *n, int32_t scale, int32_t fade_value)
	{
		int16_t ang[4] = { 0, 0, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[1] = n->p12;
		m.t[0] = n->p10;
		int32_t sc[3] = { scale, scale, scale };
		m.t[2] = var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		Scale3DMatrix(&m, sc);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x6C);
		int32_t e = n->e;
		*(uint32_t *)h = var<uint32_t>(game_addr(game::unk_E196C0) + 4 * e);
		*(uint32_t *)(h + 8) = 0;
		*(int32_t *)(h + 0xC) = fade_value;
		*(uint32_t *)(h + 0x1C) = 0xF3;
		*(uint32_t *)(h + 0x20) = var<uint32_t>(game_addr(game::unk_E19730) + 4 * e);
		Cursor() = SlashRender(h, OT(), 3, Cursor());
		FieldFree(0x6C);
	}

	static uint32_t SlashTask(Node24 *n, int32_t shrink, int32_t div, int32_t end)
	{
		if (n->s22 > 0)
		{
			if (Paused()) return 0;
			n->s22--;
			return 0;
		}
		SlashDraw(n, n->s1C, SlashFade(n->c, n->s20, n->p16));
		if (Paused()) return 0;
		int16_t cc = n->c;
		if (cc < shrink)
		{
			int16_t v = n->s1E;
			n->s1C = (int16_t)(n->s1C - v);
			n->s1E = (int16_t)(v - (int16_t)((int32_t)v / div));
		}
		n->c = (int16_t)(cc + 1);
		return n->c >= (end < 0 ? n->s20 : end) ? TASK_END : 0;
	}

	static uint32_t __cdecl SlashATask(TaskNode *tn) { return SlashTask((Node24 *)tn, 4, 2, -1); }
	static uint32_t __cdecl SlashBTask(TaskNode *tn) { return SlashTask((Node24 *)tn, 4, 2, 7); }
	static uint32_t __cdecl SlashCTask(TaskNode *tn) { return SlashTask((Node24 *)tn, 6, 3, -1); }
	static uint32_t __cdecl SlashDTask(TaskNode *tn) { return SlashTask((Node24 *)tn, 6, 3, 7); }
}

	void register_mag326_odin_reverse()
	{
		register_port(o326::ORIG_SequenceTick, (void *)o326::SequenceTick, "O326 SequenceTick", 326);
		register_port(o326::ORIG_TimelineTask, (void *)o326::TimelineTask, "O326 TimelineTask", 326);
		register_port(o326::ORIG_GridATask, (void *)o326::GridATask, "O326 GridATask", 326);
		register_port(o326::ORIG_GridBTask, (void *)o326::GridBTask, "O326 GridBTask", 326);
		register_port(o326::ORIG_DebrisTask, (void *)o326::DebrisTask, "O326 DebrisTask", 326);
		register_port(o326::ORIG_WarpTask, (void *)o326::WarpTask, "O326 WarpTask", 326);
		register_port(o326::ORIG_GlowTask, (void *)o326::GlowTask, "O326 GlowTask", 326);
		register_port(o326::ORIG_StreakTask, (void *)o326::StreakTask, "O326 StreakTask", 326);
		register_port(o326::ORIG_GridCTask, (void *)o326::GridCTask, "O326 GridCTask", 326);
		register_port(o326::ORIG_GridDTask, (void *)o326::GridDTask, "O326 GridDTask", 326);
		register_port(o326::ORIG_WallTask, (void *)o326::WallTask, "O326 WallTask", 326);
		register_port(o326::ORIG_ShockTask, (void *)o326::ShockTask, "O326 ShockTask", 326);
		register_port(o326::ORIG_GhostTask, (void *)o326::GhostTask, "O326 GhostTask", 326);
		register_port(o326::ORIG_SwordTask, (void *)o326::SwordTask, "O326 SwordTask", 326);
		register_port(o326::ORIG_FadeATask, (void *)o326::FadeATask, "O326 FadeATask", 326);
		register_port(o326::ORIG_FadeBTask, (void *)o326::FadeBTask, "O326 FadeBTask", 326);
		register_port(o326::ORIG_TintTask, (void *)o326::TintTask, "O326 TintTask", 326);
		register_port(o326::ORIG_EndTask, (void *)o326::EndTask, "O326 EndTask", 326);
		register_port(o326::ORIG_CreatureTask, (void *)o326::CreatureTask, "O326 CreatureTask", 326);
		register_port(o326::ORIG_SmokeTask, (void *)o326::SmokeTask, "O326 SmokeTask", 326);
		register_port(o326::ORIG_RingATask, (void *)o326::RingATask, "O326 RingATask", 326);
		register_port(o326::ORIG_RingBTask, (void *)o326::RingBTask, "O326 RingBTask", 326);
		register_port(o326::ORIG_DustTask, (void *)o326::DustTask, "O326 DustTask", 326);
		register_port(o326::ORIG_MistTask, (void *)o326::MistTask, "O326 MistTask", 326);
		register_port(o326::ORIG_FadeInTask, (void *)o326::FadeInTask, "O326 FadeInTask", 326);
		register_port(o326::ORIG_FlashTask, (void *)o326::FlashTask, "O326 FlashTask", 326);
		register_port(o326::ORIG_MorphTask, (void *)o326::MorphTask, "O326 MorphTask", 326);
		register_port(o326::ORIG_SparkTask, (void *)o326::SparkTask, "O326 SparkTask", 326);
		register_port(o326::ORIG_Spark2Task, (void *)o326::Spark2Task, "O326 Spark2Task", 326);
		register_port(o326::ORIG_BladeTask, (void *)o326::BladeTask, "O326 BladeTask", 326);
		register_port(o326::ORIG_TrailTask, (void *)o326::TrailTask, "O326 TrailTask", 326);
		register_port(o326::ORIG_BurstATask, (void *)o326::BurstATask, "O326 BurstATask", 326);
		register_port(o326::ORIG_BurstBTask, (void *)o326::BurstBTask, "O326 BurstBTask", 326);
		register_port(o326::ORIG_BloodTask, (void *)o326::BloodTask, "O326 BloodTask", 326);
		register_port(o326::ORIG_SlashATask, (void *)o326::SlashATask, "O326 SlashATask", 326);
		register_port(o326::ORIG_SlashBTask, (void *)o326::SlashBTask, "O326 SlashBTask", 326);
		register_port(o326::ORIG_SlashCTask, (void *)o326::SlashCTask, "O326 SlashCTask", 326);
		register_port(o326::ORIG_SlashDTask, (void *)o326::SlashDTask, "O326 SlashDTask", 326);
	}
}

