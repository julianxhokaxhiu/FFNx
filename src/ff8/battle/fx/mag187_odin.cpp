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

// Effect 187: Odin - Zantetsuken

#include "fx_port.h"

namespace ff8fx
{
namespace o187
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
		inline void WaitAnimSeq(uint32_t buffer, void *flag) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::Battle_RestoreDefaultEffectTexture))(buffer, flag); }
		inline int32_t LoadState() { return fn<int32_t (__cdecl *)()>(game_addr(game::BattleFile_GetLoadingState))(); }
		inline void CharacterLoad(int32_t id, uint32_t dst) { fn<void (__cdecl *)(int32_t, uint32_t)>(game_addr(game::BattleFile_CharacterLoad))(id, dst); }
		inline void QueueTIMUpload(uint32_t tim) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::Battle_QueueTIMUpload_GetEOF))(tim); }
		inline void QueueVramUpload(uint32_t rect, const void *data) { fn<void (__cdecl *)(uint32_t, const void *)>(game_addr(game::Battle_QueueVramUpload_Type0_RectData))(rect, data); }
		inline uint32_t SummonData() { return fn<uint32_t (__cdecl *)()>(game_addr(game::sub_571B70))(); }
		inline uint32_t Mag151Draw(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_151_CannonBlow_ScrollRender))(h, ot, mode, cursor); }
		inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
		inline void GteSetBackColorFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
		inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
		inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
		inline void GteTransFromIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_TransFromIR))(); }
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
		inline void GridRender(void *h, uint32_t ot, int32_t shift) { fn<void (__cdecl *)(void *, uint32_t, int32_t)>(game_addr(game::GF187_RenderGrid))(h, ot, shift); }
		inline uint32_t SliceRender(void *geom, void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::GF187_RenderSlicedModel))(geom, h, ot, mode, cursor); }
		inline uint32_t SlashRender(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::GF187_RenderSlashModel))(h, ot, mode, cursor); }
		inline void ScreenTile(int32_t r, int32_t g, int32_t b, int32_t otz) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, int32_t)>(game_addr(game::GF187_ScreenTile))(r, g, b, otz); }
		inline void SpawnPosMid(void *entity, int16_t *out) { fn<void (__cdecl *)(void *, int16_t *)>(game_addr(game::GF187_TargetMidpoint))(entity, out); }
	}
	using namespace x;

	inline bool Paused() { return (var<uint32_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C)) & 0x201) != 0; }
	inline Mat4x3 &RootMatrix() { return var<Mat4x3>(game_addr(game::unk_24FE880)); }
	inline Mat4x3 &Frame() { return var<Mat4x3>(game_addr(game::unk_24FE8A0)); }
	inline TaskQueue &SubQueue() { return var<TaskQueue>(game_addr(game::unk_24FD990)); }
	inline uint8_t *CastCtx() { return var<uint8_t *>(game_addr(game::unk_24FE7B0)); }
	inline uint32_t &VoiceSlot() { return var<uint32_t>(game_addr(game::unk_24FD6D4)); }
	inline uint8_t *Creature() { return (uint8_t *)game_addr(game::unk_24FD8D8); }
	inline Mat4x3 &CreatureMat() { return var<Mat4x3>(game_addr(game::unk_24FD918)); }
	inline Mat4x3 &BoneCopy() { return var<Mat4x3>(game_addr(game::g187_BoneCopy)); }
	inline int16_t *BladeP0() { return (int16_t *)game_addr(game::g187_BladeP0); }
	inline int16_t *BladeP1() { return (int16_t *)game_addr(game::g187_BladeP1); }
	inline uint32_t &DoneFlags() { return var<uint32_t>(game_addr(game::g187_DoneFlags)); }
	inline uint32_t MB() { return var<uint32_t>(game_addr(game::unk_24FE8E0)); }
	inline uint32_t &Cursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t OT() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }
	inline uint8_t *PoolA() { return var<uint8_t *>(game_addr(game::unk_E41E50)); }
	inline uint8_t *PoolB() { return var<uint8_t *>(game_addr(game::unk_E41E54)); }
	inline uint8_t *PoolC() { return var<uint8_t *>(game_addr(game::unk_E41E58)); }
	inline uint8_t *Targets() { return *(uint8_t **)(CastCtx() + 4); }
	inline uint8_t *Entity(uint32_t slot) { return (uint8_t *)(game_addr(game::BattleEntitySlotData) + 156 * slot); }
	static constexpr game::Id HIST_A = game::unk_24FD458;
	static constexpr game::Id HIST_B = game::unk_24FD4B0;
	static constexpr game::Id SPLINE_A = game::unk_24FD6D8;
	static constexpr game::Id SPLINE_B = game::unk_24FD7D8;
	static constexpr game::Id TRAIL_RECS = game::g187_TrailHistory;

	static constexpr game::Id ORIG_SequenceTick = game::au_re_BdlinkTask_36;
	static constexpr game::Id ORIG_TimelineTask = game::GF_187Odin_TimelineTask;
	static constexpr game::Id ORIG_GridATask = game::GF187_GridATask;
	static constexpr game::Id ORIG_GridBTask = game::GF187_GridBTask;
	static constexpr game::Id ORIG_DebrisTask = game::GF187_DebrisTask;
	static constexpr game::Id ORIG_WarpTask = game::GF187_TextureWarpTask;
	static constexpr game::Id ORIG_GlowTask = game::GF187_GlowTask;
	static constexpr game::Id ORIG_StreakTask = game::GF187_StreakTask;
	static constexpr game::Id ORIG_SliceTask = game::GF187_TargetSliceTask;
	static constexpr game::Id ORIG_EndTask = game::GF187_EndTask;
	static constexpr game::Id ORIG_CreatureTask = game::GF_187Odin_CreatureTask;
	static constexpr game::Id ORIG_SmokeTask = game::GF187_SmokeTask;
	static constexpr game::Id ORIG_RingATask = game::GF187_RingATask;
	static constexpr game::Id ORIG_RingBTask = game::GF187_RingBTask;
	static constexpr game::Id ORIG_DustTask = game::GF187_DustTask;
	static constexpr game::Id ORIG_MistTask = game::GF187_MistTask;
	static constexpr game::Id ORIG_FadeInTask = game::GF187_CreatureFadeInTask;
	static constexpr game::Id ORIG_FlashTask = game::GF187_ScreenFlashTask;
	static constexpr game::Id ORIG_MorphTask = game::GF187_MorphFlashTask;
	static constexpr game::Id ORIG_SparkTask = game::GF187_SwordSparkTask;
	static constexpr game::Id ORIG_Spark2Task = game::GF187_SwordSpark2Task;
	static constexpr game::Id ORIG_BladeTask = game::GF187_BladeGlowTask;
	static constexpr game::Id ORIG_TrailTask = game::GF187_BladeTrailTask;
	static constexpr game::Id ORIG_SlashATask = game::GF187_SlashLineTask;
	static constexpr game::Id ORIG_SlashBTask = game::GF187_SlashLineEchoTask;
	static constexpr game::Id ORIG_SlashCTask = game::GF187_StrokeTask;
	static constexpr game::Id ORIG_SlashDTask = game::GF187_StrokeEchoTask;
	static constexpr game::Id ORIG_HideTask = game::GF187_TargetUnhideTask;

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
namespace o187
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

	static void SpawnSlices()
	{
		uint8_t *t = Targets();
		for (uint32_t i = 0; i < t[0x10]; i++, t = Targets())
		{
			uint8_t *rec = *(uint8_t **)(t + 8) + 0x18 * i;
			if (rec[3] & 4) continue;
			uint32_t slot = rec[0];
			Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_SliceTask));
			n->c = 0;
			n->e = (int16_t)slot;
			SpawnPosMid(Entity(slot), &n->p10);
			int32_t range = *(int16_t *)(Entity(slot) + 0x26);
			int32_t lim = range;
			int32_t half = mul32(CrtRand() % 0x100 + 0x80, range) >> 12;
			int32_t span = half * 2;
			n->p10 = (int16_t)(n->p10 + (int16_t)(CrtRand() % span - half));
			n->p12 = (int16_t)(n->p12 + (int16_t)(CrtRand() % span - half));
			n->p14 = (int16_t)(n->p14 + (int16_t)(CrtRand() % span - half));
			int32_t a = CrtRand() % 0x500;
			n->s1C = 0;
			n->a18 = (int16_t)(a - 0x280);
			if (lim > 0xA00) lim = 0xA00;
			int32_t v = CrtRand() % 0x30 + 0x30;
			n->s1E = (int16_t)(mul32(v, lim) >> 12);
		}
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
			CameraScriptStart(game_addr(game::unk_E41F28), &RootMatrix(), Creature(), &SubQueue(), 0);
			BdTransSummonStream(game_addr(game::unk_FA54A4), game_addr(game::unk_24FD6D0));
			VoiceSlot() = ClaimVoiceSlot((const void *)game_addr(game::unk_E41CE8), 1, 0x80);
		}
		int32_t c = n->c;
		if (c < 12)
		{
			if (c == 1)
			{
				BdPlaySE(game_addr(game::unk_E41E40), 0, 0x80);
				CharacterLoad(0x225, MB() + 0x14000);
			}
			else if (c == 6) SpawnGrids();
			else if (c == 10) LinkTask(game_addr(ORIG_DebrisTask));
		}
		else if ((c -= 12) < 28)
		{
			if (c == 1)
			{
				if (LoadState() < 0) return 0;
				QueueTIMUpload(MB() + 0x14000);
				CharacterLoad(0x226, MB());
			}
			else if (c == 10)
			{
				if (LoadState() < 0) return 0;
				CharacterLoad(0x227, SummonData());
			}
		}
		else if ((c -= 28) < 23)
		{
			if (c == 16) Stream();
			else if (c == 0) SpawnIntro();
			else if (c == 12) LinkTask(game_addr(ORIG_CreatureTask));
		}
		else if ((c -= 23) >= 28)
		{
			c -= 28;
			if (c < 25)
			{
				if (c == 20) BdPlaySE(game_addr(game::unk_E41E44), 0, 0x80);
			}
			else if ((c -= 25) < 35)
			{
				if (c == 17) BdTransSummonStream(SummonData(), game_addr(game::unk_24FD6D0));
				else if (c == 22) Stream();
				else if (c == 1)
				{
					if (LoadState() < 0) return 0;
					CharacterLoad(0x228, MB() + 0x14000);
				}
				else if (c == 11)
				{
					if (LoadState() < 0) return 0;
					QueueTIMUpload(MB() + 0x14000);
				}
				else if (c == 25) CharacterLoad(0x229, SummonData());
			}
			else if ((c -= 35) < 20)
			{
				if (c == 12)
				{
					if (LoadState() < 0) return 0;
					BdTransSummonStream(SummonData(), game_addr(game::unk_24FD6D0));
				}
				else if (c == 19)
				{
					if (var<uint8_t>(game_addr(game::unk_24FD6D0))) CharacterLoad(0x22A, SummonData());
				}
				else if (c == 17) BdPlaySE(game_addr(game::unk_E41E48), 0, 0x80);
			}
			else if ((c -= 20) < 12) { if (c == 2) Stream(); }
			else if ((c -= 12) < 12) { if (c == 2) Stream(); }
			else if ((c -= 12) < 13) { if (c == 2) Stream(); }
			else if ((c -= 13) < 43)
			{
				if (c == 0)
				{
					if (LoadState() < 0) return 0;
					BdTransSummonStream(SummonData(), game_addr(game::unk_24FD6D0));
					SpawnSlices();
				}
				else if (c == 4) Stream();
			}
			else if ((c -= 43) < 30) { if (c == 1) LinkTask(game_addr(ORIG_EndTask)); }
			else if ((c -= 30) < 8)
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
		else if (cc >= 0x111) SetScreenFlash((uint32_t)((0x121 - (int32_t)cc) * 160), 0);
		if (n->c == 0x11F)
		{
			uint8_t *t = Targets();
			ApplyActionResultToTargets(*(uint32_t *)(t + 8), t[0x10]);
		}
		n->c++;
		if (n->c < 0x121) return 0;
		SetScreenFlash(0, 0);
		return TASK_END;
	}

	static uint32_t __cdecl EndTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->c == 1) WaitAnimSeq(MB() + 0x14000, &n->e);
		uint16_t done = (uint16_t)n->e;
		n->c++;
		return done ? TASK_END : 0;
	}

	static int32_t GridAFade(int32_t c) { return c < 8 ? c * 8 : (c >= 0x2E ? (0x36 - c) * 8 : 0x40); }
	static int32_t GridBFade(int32_t c) { return c < 8 ? c * 8 : (c >= 0x26 ? (0x36 - c) * 4 : 0x40); }

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

	static void LitSprite(uint8_t *h, const int16_t *pos, int16_t size, int16_t frame)
	{
		GteLoadV0(pos);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(size);
		*(int16_t *)(h + 4) = frame;
		GteTransFromIR();
		Cursor() = InitEffectSequenceFromData(h, OT(), 2, Cursor());
	}

	enum Integrator { INT_NONE, INT_FALL, INT_DRAG4, INT_GROW5_DRAG8, INT_XZ_DRAG8, INT_GROW6_DRAG8 };
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
		*(uint32_t *)h = game_addr(game::unk_E3CFD0);
		*(int16_t *)(h + 4) = 0;
		*(int16_t *)(h + 0x24) = 2;
		GteSetLightMatrix(s + 8);
		GteSetBackColorFromTrans(s + 8);
	}

	static void DebrisSetupB(uint8_t *h, uint8_t *s)
	{
		*(uint32_t *)h = game_addr(game::unk_E3CFF8);
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
		uint8_t *src = var<uint8_t *>(game_addr(game::unk_24FE8E8));
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
		QueueVramUpload(game_addr(game::unk_E41EA0), dst);
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
		uint8_t *h = (uint8_t *)FieldAlloc(0x7C);
		*(uint32_t *)h = game_addr(game::unk_E40848);
		*(uint16_t *)(h + 0x1C) = 0x80;
		*(uint16_t *)(h + 0x1E) = 0x80;
		*(int32_t *)(h + 0x14) = c;
		*(int32_t *)(h + 0xC) = 0x400;
		*(int32_t *)(h + 0x10) = -c;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x20) = 0xF3;
		*(uint16_t *)(h + 0x18) = 0;
		*(uint16_t *)(h + 0x1A) = 0;
		Cursor() = Mag151Draw(h, OT(), 2, Cursor());
		FieldFree(0x7C);
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
		*(uint32_t *)h = game_addr(game::unk_E40160);
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

	static void PlaneDistance(const int16_t *p, int16_t *nrm)
	{
		int32_t d = mul32(p[2], nrm[2]) + mul32(p[1], nrm[1]);
		d = d + mul32(p[0], nrm[0]);
		nrm[3] = (int16_t)-(d >> 12);
	}

	static void TargetDraw(uint8_t *h, uint8_t *ent, const Mat4x3 *moved)
	{
		if (!(ent[0] & 0x20))
			Cursor() = DrawShadow(ent, var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4064, 0x10, Cursor());
		ComposeAffineTransform(&Camera(), (const Mat4x3 *)(ent + 0x40), (Mat4x3 *)(h + 0x190));
		ComposeAffineTransform(&Camera(), moved, (Mat4x3 *)(h + 0x1B0));
		*(uint32_t *)(h + 4) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		*(uint32_t *)(h + 0x18) = *(uint32_t *)(ent + 0x28);
		*(uint32_t *)(h + 0x10) = *(uint32_t *)(ent + 0x7C);
		Cursor() = SliceRender(*(void **)(ent + 0x64), h, OT(), 2, Cursor());
		BuildBoneMatricesFromPose(ent + 0x60);
	}

	static void SliceDraw(uint8_t *h, const Node24 *n, uint8_t *ent, int32_t offset, int32_t c, bool recolor)
	{
		int16_t v[4] = { 0, 0, n->a18, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(v, &m);
		*(uint32_t *)(h + 0x40) = *(const uint32_t *)&n->p10;
		*(uint32_t *)(h + 0x44) = *(const uint32_t *)&n->p14;
		int16_t *nrm = (int16_t *)(h + 0xA0);
		nrm[0] = 0;
		nrm[1] = (int16_t)0xF000;
		nrm[2] = 0;
		MatrixMultiplyVector(&m, nrm, nrm);
		PlaneDistance((const int16_t *)(h + 0x40), nrm);
		v[0] = 0x1000;
		v[1] = 0;
		v[2] = 0;
		GteMatrixMultiply((const Mat4x3 *)(ent + 0x40), &m);
		MatrixMultiplyVector(&m, v, v);
		int32_t d = offset;
		if (v[1] < 0) d = -d;
		Mat4x3 moved = *(const Mat4x3 *)(ent + 0x40);
		moved.t[0] = (int32_t)((uint32_t)moved.t[0] + (uint32_t)(mul32(v[0], d) >> 12));
		moved.t[1] = (int32_t)((uint32_t)moved.t[1] + (uint32_t)(mul32(v[1], d) >> 12));
		moved.t[2] = (int32_t)((uint32_t)moved.t[2] + (uint32_t)(mul32(v[2], d) >> 12));
		if (recolor && c >= 0x41)
		{
			int32_t t = ComputeSin(shl32(c - 0x41, 10) / 16);
			int32_t w = 0x1000 - t;
			uint32_t k = (uint32_t)(shl32(w, 7) >> 12);
			uint32_t col = ((((k | 0x3200u) << 8) | k) << 8) | k;
			*(uint32_t *)(ent + 0x2C) = col;
			GteSetFarColor(var<uint8_t>(game_addr(game::CURRENT_BS_MODEL_PALETTE)), var<uint8_t>(game_addr(game::unk_B8B7D9)), var<uint8_t>(game_addr(game::unk_B8B7DA)));
			uint32_t zero = 0;
			GteLoadRGBC(&zero);
			GteSetIR0(w);
			GteDPCS();
			GteStoreRGB2(ent + 0x28);
			ent[0x2B] = 2;
			ent[7] = 0;
		}
		TargetDraw(h, ent, &moved);
	}

	static uint32_t __cdecl SliceTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int32_t c = n->c;
		if (c < 30) Entity((uint32_t)(int32_t)n->e)[0] |= 8;
		else if (c - 30 < 50)
		{
			uint8_t *ent = Entity((uint32_t)(int32_t)n->e);
			ent[0] |= 4;
			uint8_t *h = (uint8_t *)FieldAlloc(0x1D0);
			SliceDraw(h, n, ent, n->s1C, c, true);
			FieldFree(0x1D0);
		}
		if (Paused()) return 0;
		int16_t cc = n->c;
		if (cc >= 30)
		{
			if (cc < 34) n->s1C = (int16_t)(n->s1C + (int16_t)(n->s1E * 3));
			else
			{
				int16_t v = n->s1E;
				n->s1C = (int16_t)(n->s1C + v);
				int16_t nv = (int16_t)(v - (int16_t)((int32_t)v / 10));
				n->s1E = nv < 2 ? (int16_t)2 : nv;
			}
		}
		n->c = (int16_t)(cc + 1);
		return n->c > 0x51 ? TASK_END : 0;
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

	static void SetAnim(int32_t id) { PreReadAnimation(Creature() + 0x60, Creature() + 0x6C, id); }

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
		for (uint32_t a = game_addr(game::unk_24FD510); a < game_addr(game::unk_24FD54C); a += 0x14) *(uint32_t *)a = 0;
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
		for (uint32_t a = game_addr(TRAIL_RECS); a < game_addr(game::unk_24FE880); a += 0x14) *(uint32_t *)a = 0;
		for (uint32_t k = 0; k < 0x3C; k += 0x14)
		{
			*(uint32_t *)(game_addr(game::unk_24FD510) + k) = 0;
			*(uint32_t *)(game_addr(game::unk_24FD5D8) + k) = 0;
		}
	}

	static void SpawnTrail()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_TrailTask));
		n->c = 0;
		for (uint32_t a = game_addr(TRAIL_RECS); a < game_addr(game::unk_24FE880); a += 0x14) *(uint32_t *)a = 0;
	}

	static void SpawnStrokes(int32_t k)
	{
		const int16_t *t = (const int16_t *)(game_addr(game::unk_E41EF8) + 8 * k);
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_SlashCTask));
		n->e = t[3];
		int16_t z = t[2];
		n->p10 = t[0];
		n->c = 0;
		n->p12 = t[1];
		n->p14 = z;
		n->p16 = 0;
		n->s1C = 0x2E80;
		n->s1E = 0x1740;
		n->s20 = (int16_t)(0xE - z);
		n->s22 = z;
		for (int i = 0; i < 4; i++)
		{
			Node24 *u = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_SlashDTask));
			u->e = n->e;
			*(uint32_t *)&u->p10 = *(const uint32_t *)&n->p10;
			*(uint32_t *)&u->p14 = *(const uint32_t *)&n->p14;
			u->s1C = n->s1C;
			u->s1E = n->s1E;
			int16_t cc = (int16_t)(i + u->p14);
			u->c = 0;
			u->p14 = cc;
			u->p16 = 0x800;
			u->s20 = (int16_t)(0xE - cc);
			u->s22 = cc;
		}
	}

	static void SpawnSlashLines()
	{
		for (uint32_t e = game_addr(game::unk_E41EF8); e < game_addr(game::unk_E41F10); e += 8)
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
			n->s1C = 0x2DC0;
			n->s1E = 0xF40;
			n->s20 = (int16_t)(0x1E - z);
			n->s22 = z;
			for (int i = 0; i < 2; i++)
			{
				Node24 *u = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_SlashBTask));
				u->e = n->e;
				*(uint32_t *)&u->p10 = *(const uint32_t *)&n->p10;
				*(uint32_t *)&u->p14 = *(const uint32_t *)&n->p14;
				u->s1C = n->s1C;
				u->s1E = n->s1E;
				int16_t cc = (int16_t)(i + u->p14);
				u->c = 0;
				u->p14 = cc;
				u->p16 = 0x800;
				u->s20 = (int16_t)(0x1E - cc);
				u->s22 = cc;
			}
		}
	}

	static void HideTargets(int32_t k)
	{
		for (uint32_t i = 0; i < Targets()[0x10]; i++)
		{
			uint32_t slot = (*(uint8_t **)(Targets() + 8))[0x18 * i];
			Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), game_addr(ORIG_HideTask));
			n->a18 = (int16_t)slot;
			n->c = 0;
			n->e = (int16_t)k;
			uint16_t *flags = (uint16_t *)Entity(slot);
			uint16_t f = *flags;
			*flags = (uint16_t)(f | 8);
			n->s1A = (int16_t)(f & 8);
		}
	}

	static void Draw(uint8_t *E)
	{
		DrawModel(E, &Frame(), var<uint32_t>(game_addr(game::unk_E41E98)));
	}

	static uint32_t __cdecl CreatureTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *E = Creature();
		if (n->c == 0)
		{
			ResetRotation((void *)game_addr(game::unk_24FD918));
			var<uint16_t>(game_addr(game::unk_24FD928)) = 0x2000;
			var<uint16_t>(game_addr(game::unk_24FD920)) = 0x2000;
			var<uint16_t>(game_addr(game::unk_24FD918)) = 0x2000;
			var<int32_t>(game_addr(game::unk_24FD930)) = 0;
			var<int32_t>(game_addr(game::unk_24FD92C)) = 0;
			var<int32_t>(game_addr(game::unk_24FD934)) = 0x708;
			ModelInit(E, (uint8_t *)game_addr(game::unk_24FE8F0), (uint8_t *)MB(), 0xF);
			E[0] |= 1;
			LinkTask(game_addr(ORIG_BladeTask));
		}
		int32_t c = n->c;
		if (c < 12)
		{
			if (Paused()) goto draw_advance;
			if (c == 0) SetAnim(0);
			else if (c == 3)
			{
				LinkTask(game_addr(ORIG_SmokeTask));
				SpawnRings();
				LinkTask(game_addr(ORIG_DustTask));
			}
			goto draw_advance;
		}
		if ((c -= 12) < 28)
		{
			var<uint16_t>(game_addr(game::unk_24FD928)) = 0x1000;
			var<uint16_t>(game_addr(game::unk_24FD920)) = 0x1000;
			var<uint16_t>(game_addr(game::unk_24FD918)) = 0x1000;
			if (Paused()) goto draw;
			if (c == 0) SetAnim(1);
			else if (c == 9) LinkTask(game_addr(ORIG_MistTask));
			goto advance_draw;
		}
		if ((c -= 28) < 13)
		{
			if (Paused()) goto draw;
			if (c == 0)
			{
				SetAnim(2);
				AdvanceModelAnimLoop(E);
			}
			else if (c == 12) SpawnFadeIn();
			goto advance_draw;
		}
		if ((c -= 13) < 12) goto draw;
		if ((c -= 12) < 43)
		{
			if (Paused()) goto draw_advance;
			if (c == 0) SetAnim(3);
			else if (c == 17) SpawnSparks();
			else if (c == 41) SpawnSparks2();
			goto draw_advance;
		}
		if ((c -= 43) < 12)
		{
			if (!Paused() && c == 0) SetAnim(4);
			Draw(E);
			if (Paused()) goto end;
			if (c < 4) var<int32_t>(game_addr(game::unk_24FD934)) += 0x5A;
			AdvanceModelAnimLoop(E);
			GetEffectSpawnPosition(E, 0, 0, &n->p10);
			goto end;
		}
		if ((c -= 12) < 11)
		{
			if (Paused()) goto draw_advance;
			if (c == 0)
			{
				var<int32_t>(game_addr(game::unk_24FD92C)) = 0;
				var<int32_t>(game_addr(game::unk_24FD930)) = 0;
				var<int32_t>(game_addr(game::unk_24FD934)) = n->p14 - 100;
				SetAnim(5);
			}
			else if (c == 3)
			{
				SpawnTrail();
				CameraShake(0, 1, 2, 0xFF);
			}
			else if (c == 1) SpawnStrokes(3);
			goto draw_advance;
		}
		c -= 11;
		if (c < 1) goto hide_one;
		if ((c -= 1) < 11)
		{
			if (Paused()) goto draw_advance;
			if (c == 0) SetAnim(5);
			else if (c == 3)
			{
				SpawnTrail();
				CameraShake(0, 1, 2, 0xFF);
			}
			else if (c == 1) SpawnStrokes(4);
			goto draw_advance;
		}
		if ((c -= 11) < 1) goto hide_one;
		if ((c -= 1) < 11)
		{
			if (Paused()) goto draw_advance;
			if (c == 0) SetAnim(5);
			else if (c == 3)
			{
				SpawnTrail();
				CameraShake(0, 1, 2, 0xFF);
			}
			else if (c == 1) SpawnStrokes(5);
			goto draw_advance;
		}
		if ((c -= 11) < 2)
		{
			if (Paused() || c != 0) goto draw;
			HideTargets(2);
			SpawnSlashLines();
			goto draw;
		}
		if ((c -= 2) < 11)
		{
			if (Paused() || c != 0) goto draw;
			var<int32_t>(game_addr(game::unk_24FD930)) = 0x2BC;
			var<int32_t>(game_addr(game::unk_24FD934)) = (int32_t)0xFFFFDD3C;
			var<int32_t>(game_addr(game::unk_24FD92C)) = mul32(var<int32_t>(game_addr(game::g_CamScript_MirrorSign)), -1900);
			SetAnim(6);
			goto draw;
		}
		if ((c -= 11) < 20) goto draw_advance;
		if ((c -= 20) < 10) goto draw_advance;
		goto end;

	hide_one:
		if (Paused() || c != 0) goto draw;
		HideTargets(1);
		goto draw;

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
		if (n->c < 0xC6) return 0;
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
		var<uint32_t>(game_addr(game::unk_24FD900)) = Rgb(v);
		if (Paused()) return 0;
		n->c = (int16_t)(c + 1);
		if (n->c < 0xB) return 0;
		var<uint32_t>(game_addr(game::unk_24FD900)) = 0x808080;
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
		*(uint32_t *)h = game_addr(game::unk_E3EFF0);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x2033;
		if (fade)
		{
			*(uint32_t *)(h + 0x1C) = 0x20F3;
			*(int32_t *)(h + 0xC) = fade_value;
		}
		int32_t w = ComputeSin(sin_arg);
		const int16_t *a = (const int16_t *)game_addr(game::unk_E3EFF8), *b = (const int16_t *)game_addr(game::unk_E3FD70);
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
		MorphDraw(n, MorphArg(c), c >= 6, mul32(c - 6, 682), var<uint32_t>(game_addr(game::unk_E41E98)));
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
		*(uint32_t *)h = game_addr(game::unk_E3D300);
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
		*(uint32_t *)h = game_addr(game::unk_E41140);
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
		*(uint32_t *)h = game_addr(game::unk_E3CC38);
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
		*(uint32_t *)h = game_addr(game::unk_E3CC38);
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
		int32_t hist = BoneHistory(game_addr(game::unk_24FD510), 0x28, n->c, s);
		SparkSetup(&Frame(), h, s, game_addr(game::unk_E3CC38), 0x404040);
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
		uint32_t table = var<uint32_t>(game_addr(game::unk_E41E8C) + 4 * (int32_t)n->a18);
		int32_t hist = BoneHistory(table, n->e, n->c, s);
		SparkSetup(&Frame(), h, s, game_addr(game::unk_E3D154), 0x202020);
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
		*(uint32_t *)h = game_addr(game::unk_E41B28);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x33;
		Cursor() = RenderPrimModel(h, var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 4 * (uint32_t)o, 0xE, Cursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl BladeTask(TaskNode *tn)
	{
		if (var<uint8_t>(game_addr(game::g187_DoneFlags)) & 1) return TASK_END;
		Node24 *n = (Node24 *)tn;
		BladeDraw(&Frame(), &CreatureMat(), &BoneCopy(), BladeP0(), BladeP1(), n->c > 0);
		if (Paused()) return 0;
		uint8_t *sk = **(uint8_t ***)(game_addr(game::unk_24FD93C));
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
		int32_t k = var<int32_t>(game_addr(game::unk_E41E5C) + 4 * (int32_t)n->c);
		int32_t *d = (int32_t *)(s + 0x30);
		d[0] = (int32_t)p1[0] - (int32_t)p0[0];
		d[1] = (int32_t)p1[1] - (int32_t)p0[1];
		d[2] = (int32_t)p1[2] - (int32_t)p0[2];
		int32_t len = Sqrt(NormalizeVectorToFixedPoint(d, d));
		len = mul32(len, k) >> 12;
		uint32_t w0 = var<uint32_t>(game_addr(game::g187_BladeP0)), w1 = var<uint32_t>(game_addr(game::unk_24FD984));
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
			const uint32_t *src = cnt == 0 ? (const uint32_t *)game_addr(game::g187_BladeP1) : e + 3;
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

	static int32_t SlashFade(int32_t c, int32_t s20, int32_t base, int32_t in_len)
	{
		int32_t span = 0x1000 - base;
		if (c < in_len) return 0x1000 - mul32(span / in_len, c);
		int32_t out = s20 - 8;
		if (c >= out) return mul32(c - out, span / 8) + base;
		return base;
	}

	static void SlashDraw(const Node24 *n, int32_t scale, int32_t fade_value, bool model)
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
		uint8_t *h = (uint8_t *)FieldAlloc(model ? 0x6C : 0x58);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)h = var<uint32_t>(game_addr(game::unk_E41EA8) + 4 * (int32_t)n->e);
		*(int32_t *)(h + 0xC) = fade_value;
		*(uint32_t *)(h + 0x1C) = 0xF3;
		if (model)
		{
			*(uint32_t *)(h + 0x20) = 0x2020A0;
			Cursor() = SlashRender(h, OT(), 3, Cursor());
			FieldFree(0x6C);
		}
		else
		{
			Cursor() = RenderPrimModel(h, OT(), 3, Cursor());
			FieldFree(0x58);
		}
	}

	static uint32_t SlashTask(Node24 *n, bool model, int32_t end, bool third)
	{
		if (n->s22 > 0)
		{
			if (Paused()) return 0;
			n->s22--;
			return 0;
		}
		int32_t c = n->c;
		SlashDraw(n, n->s1C, SlashFade(c, n->s20, n->p16, model ? 8 : 6), model);
		if (Paused()) return 0;
		int16_t cc = n->c;
		if (third ? cc < 6 : cc < 3)
		{
			int16_t v = n->s1E;
			n->s1C = (int16_t)(n->s1C - v);
			n->s1E = (int16_t)(v - (int16_t)((int32_t)v / (third ? 3 : 2)));
		}
		n->c = (int16_t)(cc + 1);
		return n->c >= (end < 0 ? n->s20 : end) ? TASK_END : 0;
	}

	static uint32_t __cdecl SlashATask(TaskNode *tn) { return SlashTask((Node24 *)tn, true, -1, true); }
	static uint32_t __cdecl SlashBTask(TaskNode *tn) { return SlashTask((Node24 *)tn, true, 7, true); }
	static uint32_t __cdecl SlashCTask(TaskNode *tn) { return SlashTask((Node24 *)tn, false, -1, false); }
	static uint32_t __cdecl SlashDTask(TaskNode *tn) { return SlashTask((Node24 *)tn, false, 5, false); }

	static uint32_t __cdecl HideTask(TaskNode *tn)
	{
		if (Paused()) return 0;
		Node24 *n = (Node24 *)tn;
		n->c++;
		if (n->c < n->e) return 0;
		uint16_t *flags = (uint16_t *)Entity((uint32_t)(int32_t)n->a18);
		*flags = (uint16_t)((*flags & 0xFFF7) | (uint16_t)n->s1A);
		return TASK_END;
	}
}

	void register_mag187_odin()
	{
		register_port(o187::ORIG_SequenceTick, (void *)o187::SequenceTick, "O187 SequenceTick", 187);
		register_port(o187::ORIG_TimelineTask, (void *)o187::TimelineTask, "O187 TimelineTask", 187);
		register_port(o187::ORIG_GridATask, (void *)o187::GridATask, "O187 GridATask", 187);
		register_port(o187::ORIG_GridBTask, (void *)o187::GridBTask, "O187 GridBTask", 187);
		register_port(o187::ORIG_DebrisTask, (void *)o187::DebrisTask, "O187 DebrisTask", 187);
		register_port(o187::ORIG_WarpTask, (void *)o187::WarpTask, "O187 WarpTask", 187);
		register_port(o187::ORIG_GlowTask, (void *)o187::GlowTask, "O187 GlowTask", 187);
		register_port(o187::ORIG_StreakTask, (void *)o187::StreakTask, "O187 StreakTask", 187);
		register_port(o187::ORIG_SliceTask, (void *)o187::SliceTask, "O187 SliceTask", 187);
		register_port(o187::ORIG_EndTask, (void *)o187::EndTask, "O187 EndTask", 187);
		register_port(o187::ORIG_CreatureTask, (void *)o187::CreatureTask, "O187 CreatureTask", 187);
		register_port(o187::ORIG_SmokeTask, (void *)o187::SmokeTask, "O187 SmokeTask", 187);
		register_port(o187::ORIG_RingATask, (void *)o187::RingATask, "O187 RingATask", 187);
		register_port(o187::ORIG_RingBTask, (void *)o187::RingBTask, "O187 RingBTask", 187);
		register_port(o187::ORIG_DustTask, (void *)o187::DustTask, "O187 DustTask", 187);
		register_port(o187::ORIG_MistTask, (void *)o187::MistTask, "O187 MistTask", 187);
		register_port(o187::ORIG_FadeInTask, (void *)o187::FadeInTask, "O187 FadeInTask", 187);
		register_port(o187::ORIG_FlashTask, (void *)o187::FlashTask, "O187 FlashTask", 187);
		register_port(o187::ORIG_MorphTask, (void *)o187::MorphTask, "O187 MorphTask", 187);
		register_port(o187::ORIG_SparkTask, (void *)o187::SparkTask, "O187 SparkTask", 187);
		register_port(o187::ORIG_Spark2Task, (void *)o187::Spark2Task, "O187 Spark2Task", 187);
		register_port(o187::ORIG_BladeTask, (void *)o187::BladeTask, "O187 BladeTask", 187);
		register_port(o187::ORIG_TrailTask, (void *)o187::TrailTask, "O187 TrailTask", 187);
		register_port(o187::ORIG_SlashATask, (void *)o187::SlashATask, "O187 SlashATask", 187);
		register_port(o187::ORIG_SlashBTask, (void *)o187::SlashBTask, "O187 SlashBTask", 187);
		register_port(o187::ORIG_SlashCTask, (void *)o187::SlashCTask, "O187 SlashCTask", 187);
		register_port(o187::ORIG_SlashDTask, (void *)o187::SlashDTask, "O187 SlashDTask", 187);
		register_port(o187::ORIG_HideTask, (void *)o187::HideTask, "O187 HideTask", 187);
	}
}

