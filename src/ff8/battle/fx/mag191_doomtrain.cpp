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

// Effect 191: Doomtrain - Runaway Train

#include "fx_port.h"

namespace ff8fx
{
namespace d191
{
	using namespace eng;

	namespace x
	{
		inline int32_t CrtRand() { return fn<int32_t (__cdecl *)()>(game_addr(game::_rand))(); }
		inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
		inline void ComposeZYXRotationMatrix(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::ComposeZYXRotationMatrix))(angles, out); }
		inline void RotMatrixXYZ(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::BuildRotationMatrixXYZ))(angles, out); }
		inline void Scale3DMatrix(Mat4x3 *m, const int32_t *v) { fn<void (__cdecl *)(Mat4x3 *, const int32_t *)>(game_addr(game::scale3DMatrix))(m, v); }
		inline void UnpackRotationMatrix(const Mat4x3 *src, Mat4x3 *dst) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::UnpackRotationMatrix))(src, dst); }
		inline uint32_t MatVec(const Mat4x3 *m, const int16_t *in, int16_t *out) { return fn<uint32_t (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
		inline int32_t Sqrt(int32_t v) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::Math_SqrtInt))(v); }
		inline int32_t NormalizeQ12(const int32_t *in, int16_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int16_t *)>(game_addr(game::Math_NormalizeVec3_Q12))(in, out); }
		inline int32_t Normalize(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
		inline int32_t GetRotationBetweenVectors(const void *ref, const int32_t *v, int32_t *axis) { return fn<int32_t (__cdecl *)(const void *, const int32_t *, int32_t *)>(game_addr(game::GetRotationBetweenVectors_AxisAngle))(ref, v, axis); }
		inline void BuildAxisAngleRotationMatrix(int32_t angle, Mat4x3 *out, const int32_t *axis) { fn<void (__cdecl *)(int32_t, Mat4x3 *, const int32_t *)>(game_addr(game::BuildAxisAngleRotationMatrix))(angle, out, axis); }
		inline void GteMatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
		inline void CurveCoeffs(int32_t n, const void *pts, void *buf) { fn<void (__cdecl *)(int32_t, const void *, void *)>(game_addr(game::Effect_SplineSetup))(n, pts, buf); }
		inline void CurveEval(int32_t n, void *buf, void *dst, int32_t t) { fn<void (__cdecl *)(int32_t, void *, void *, int32_t)>(game_addr(game::Effect_SplineEval))(n, buf, dst, t); }
		inline void CalculateCenterPosition(uint32_t mask, int16_t *out) { fn<void (__cdecl *)(uint32_t, int16_t *)>(game_addr(game::CalculateCenterPosition))(mask, out); }
		inline int32_t GetEffectSpawnPosition(void *entity, int32_t bone, int32_t frac, int16_t *out) { return fn<int32_t (__cdecl *)(void *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, frac, out); }
		inline uint32_t RenderGeometry(void *geom, void *hdr, uint32_t ot, int32_t shift, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(geom, hdr, ot, shift, cursor); }
		inline void ProjectionH(int32_t h) { fn<void (__cdecl *)(int32_t)>(game_addr(game::Battle_SetProjectionH))(h); }
		inline void BdPlaySE(uint32_t se, int32_t a, int32_t b) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t)>(game_addr(game::BdPlaySE))(se, a, b); }
		inline void BdPlaySummonStream(int32_t a, int32_t b, int32_t c) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::BdPlaySummonStream))(a, b, c); }
		inline void BdTransSummonStream(uint32_t src, uint32_t state) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::BdTransSummonStream))(src, state); }
		inline int32_t VoiceSlotBusy(uint32_t slot) { return fn<int32_t (__cdecl *)(uint32_t)>(game_addr(game::BdSound_IsVoiceSlotActive))(slot); }
		inline void ReleaseVoiceSlot(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
		inline void CameraShake(int32_t a, int32_t b, int32_t c, int32_t d) { fn<void (__cdecl *)(int32_t, int32_t, int32_t, int32_t)>(game_addr(game::Effect_SpawnScreenFadeTask))(a, b, c, d); }
		inline void ApplyActionResultToTargets(uint32_t list, uint32_t count) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::ApplyActionResultToTargets))(list, count); }
		inline void WaitAnimSeq(void *buffer, void *flag) { fn<void (__cdecl *)(void *, void *)>(game_addr(game::Battle_RestoreDefaultEffectTexture))(buffer, flag); }
		inline int32_t LoadState() { return fn<int32_t (__cdecl *)()>(game_addr(game::BattleFile_GetLoadingState))(); }
		inline void CharacterLoad(int32_t id, uint32_t dst) { fn<void (__cdecl *)(int32_t, uint32_t)>(game_addr(game::BattleFile_CharacterLoad))(id, dst); }
		inline void QueueTIMUpload(uint32_t tim) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::Battle_QueueTIMUpload_GetEOF))(tim); }
		inline uint32_t SummonData() { return fn<uint32_t (__cdecl *)()>(game_addr(game::sub_571B70))(); }
		inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
		inline void GteSetBackColorFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
		inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
		inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
		inline void GteTransFromMAC() { fn<void (__cdecl *)()>(game_addr(game::GTE_TransFromMAC))(); }
		inline void GteTransFromIR() { fn<void (__cdecl *)()>(game_addr(game::GTE_TransFromIR))(); }
		inline void GteSetTransFromVec32(const int32_t *v) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_SetTransFromVec32))(v); }
		inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
		inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
		inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
		inline void GteStoreRGB2(void *c) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(c); }
		inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
		inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
		inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
		inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
		inline void GteReadSZ3(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadSZ3))(dst); }
		inline void ReaderSetAnim(void *hdr, void *cmd, int32_t id) { fn<void (__cdecl *)(void *, void *, int32_t)>(game_addr(game::GF_191Doomtrain_SetAnim_PrivateReader))(hdr, cmd, id); }
		inline int32_t ReaderRead(void *hdr, void *cmd) { return fn<int32_t (__cdecl *)(void *, void *)>(game_addr(game::GF191_ReadAnimation_PrivateReader))(hdr, cmd); }
		inline void ReaderBuildBones(void *hdr) { fn<void (__cdecl *)(void *)>(game_addr(game::GF191_BuildBoneMatricesFromPose_PrivateReader))(hdr); }
		inline uint32_t ComposeBones(void *hdr, const Mat4x3 *frame, const Mat4x3 *model) { return fn<uint32_t (__cdecl *)(void *, const Mat4x3 *, const Mat4x3 *)>(game_addr(game::GF191_ComposeBones))(hdr, frame, model); }
		inline uint32_t SkyRender(void *h, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::GF191_SkyRender))(h, ot, mode, cursor); }
	}
	using namespace x;

	inline uint32_t Flags() { return var<uint32_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C)); }
	inline bool P1() { return (var<uint8_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C)) & 1) != 0; }
	inline Mat4x3 &Root() { return var<Mat4x3>(game_addr(game::g191_Root)); }
	inline Mat4x3 &Frame() { return var<Mat4x3>(game_addr(game::g191_Frame)); }
	inline Mat4x3 &BoneWorld() { return var<Mat4x3>(game_addr(game::g191_ChimneyWorld)); }
	inline TaskQueue &SubQueue() { return var<TaskQueue>(game_addr(game::g191_SubQueue)); }
	inline uint8_t *CastCtx() { return var<uint8_t *>(game_addr(game::unk_24FD258)); }
	inline uint32_t MB() { return var<uint32_t>(game_addr(game::unk_24FD3A0)); }
	inline uint8_t *Creature() { return (uint8_t *)game_addr(game::g191_Creature); }
	inline Mat4x3 &CreatureMat() { return var<Mat4x3>(game_addr(game::MAG_191_CreatureMat)); }
	inline int32_t &CreatureZ() { return var<int32_t>(game_addr(game::MAG_191_CreatureZ)); }
	inline uint32_t &StateFlags() { return var<uint32_t>(game_addr(game::g191_StateFlags)); }
	inline uint32_t &VoiceSlot() { return var<uint32_t>(game_addr(game::MAG_191_VoiceSlot)); }
	inline uint32_t &Cursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t OT() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44; }
	inline uint8_t *Pool(uint32_t cell) { return var<uint8_t *>(cell); }
	static constexpr game::Id POOL_A = game::g191_PoolA, POOL_B = game::g191_PoolB, POOL_C = game::g191_PoolC, POOL_D = game::g191_PoolD, POOL_E = game::g191_PoolE, POOL_F = game::g191_PoolF;
	inline uint8_t *Entity(uint32_t slot) { return (uint8_t *)(game_addr(game::BattleEntitySlotData) + 156 * slot); }
	inline int32_t *TargetList() { return (int32_t *)game_addr(game::g191_TargetList); }
	inline int32_t &TargetCount() { return var<int32_t>(game_addr(game::g191_TargetCount)); }
	inline Mat4x3 *RigW(int16_t e) { return (Mat4x3 *)(e ? game_addr(game::unk_24FD418) : game_addr(game::unk_24FD380)); }
	inline Mat4x3 *RigM(int16_t e) { return (Mat4x3 *)(e ? game_addr(game::unk_24FD360) : game_addr(game::unk_24FD2B8)); }

	static constexpr game::Id ORIG_SequenceTick = game::GF_191Doomtrain_SequenceTick;
	static constexpr game::Id ORIG_TimelineTask = game::GF_191Doomtrain_TimelineTask;
	static constexpr game::Id ORIG_CamScriptTask = game::MAG066_CamScript_Task;
	static constexpr game::Id ORIG_FlashInTask = game::GF191_FlashInTask;
	static constexpr game::Id ORIG_FlashOutTask = game::GF191_FlashOutTask;
	static constexpr game::Id ORIG_RigTask = game::GF191_RigTask;
	static constexpr game::Id ORIG_RigPrimATask = game::GF191_RigPrimATask;
	static constexpr game::Id ORIG_RigPrimBTask = game::GF191_RigPrimBTask;
	static constexpr game::Id ORIG_RigRockTask = game::GF191_RigRockTask;
	static constexpr game::Id ORIG_RigFlashTask = game::GF191_RigFlashTask;
	static constexpr game::Id ORIG_RigSpriteTask = game::GF191_RigSpriteTask;
	static constexpr game::Id ORIG_SkyTask = game::GF191_SkyTask;
	static constexpr game::Id ORIG_SteamPuffTask = game::GF191_SteamPuffTask;
	static constexpr game::Id ORIG_SteamPairTask = game::GF191_SteamPairTask;
	static constexpr game::Id ORIG_RainTask = game::GF191_RainTask;
	static constexpr game::Id ORIG_EndTask = game::GF191_EndTask;
	static constexpr game::Id ORIG_CreatureTask = game::GF_191Doomtrain_CreatureTask;
	static constexpr game::Id ORIG_WheelTrailTask = game::GF191_WheelTrailTask;
	static constexpr game::Id ORIG_PuffLineTask = game::GF191_PuffLineTask;
	static constexpr game::Id ORIG_PuffWallTask = game::GF191_PuffWallTask;
	static constexpr game::Id ORIG_FlareTask = game::GF191_FlareTask;
	static constexpr game::Id ORIG_GlowTask = game::GF191_GlowTask;
	static constexpr game::Id ORIG_ChimneyTask = game::GF191_ChimneyTask;
	static constexpr game::Id ORIG_SteamTrailTask = game::GF191_SteamTrailTask;
	static constexpr game::Id ORIG_SteamStillTask = game::GF191_SteamStillTask;
	static constexpr game::Id ORIG_SteamBurstTask = game::GF191_SteamBurstTask;
	static constexpr game::Id ORIG_WheelSteamTask = game::GF191_WheelSteamTask;
	static constexpr game::Id ORIG_WheelStillTask = game::GF191_WheelStillTask;
	static constexpr game::Id ORIG_WheelBurstTask = game::GF191_WheelBurstTask;
	static constexpr game::Id ORIG_DebrisSpawnTask = game::GF191_DebrisSpawnTask;
	static constexpr game::Id ORIG_DebrisTask = game::GF191_DebrisTask;
	static constexpr game::Id ORIG_ExplosionTask = game::GF191_ExplosionTask;
	static constexpr game::Id ORIG_KnockTask = game::GF191_KnockTask;
	static constexpr game::Id ORIG_LandDustTask = game::GF191_LandDustTask;

#pragma pack(push, 1)
	struct Node24
	{
		TaskNode hdr;
		int16_t c;
		int16_t e;
		int16_t f10, f12, f14, f16;
		int16_t f18, f1A;
		int16_t f1C, f1E;
		int16_t f20, f22;
	};
	struct Rec
	{
		uint32_t mask;
		int16_t age;
		int16_t size;
		int16_t pos[4];
		int16_t vel[3];
		int16_t w16;
	};
	struct Hist { uint32_t valid; uint32_t a0, a1, b0, b1; };
#pragma pack(pop)
	static_assert(sizeof(Node24) == 0x24, "sub-queue node is 0x24 bytes");
	static_assert(sizeof(Rec) == 0x18, "pool record is 0x18 bytes");
	static_assert(sizeof(Hist) == 0x14, "history entry is 0x14 bytes");

	inline Rec *R(uint32_t cell, int i) { return (Rec *)(Pool(cell) + 0x18 * i); }
	inline Hist *HistA(int k) { return (Hist *)(game_addr(game::g191_SteamHistA) + 0x14 * k); }
	inline Hist *HistB(int k) { return (Hist *)(game_addr(game::g191_SteamHistB) + 0x14 * k); }
	inline int16_t W16(uint32_t a) { return var<int16_t>(a); }

	static int FreeRec(uint32_t cell, int count)
	{
		uint8_t *p = Pool(cell);
		for (int i = 0; i < count; i++, p += 0x18)
			if (*(uint32_t *)p == 0) return i;
		return -1;
	}

#pragma pack(push, 1)
	struct CamTrack
	{
		int16_t op;
		int16_t dur;
		int32_t acc;
		int32_t step;
		int32_t pad0C;
		int16_t pos[4];
		int16_t start[4];
		int16_t delta[4];
		int16_t ease;
		int16_t npts;
		int16_t pts[16][4];
	};
	struct CamNode
	{
		TaskNode hdr;
		int16_t c, e;
		uint32_t f10;
		const int16_t *script;
		const Mat4x3 *frame;
		void *entity;
	};
#pragma pack(pop)
	static_assert(sizeof(CamTrack) == 0xAC, "camera track is 0xAC bytes");
	static_assert(sizeof(CamNode) == 0x20, "camera-script node fields end at +0x20");
}
}

namespace ff8fx
{
namespace d191
{
	inline CamTrack *TrackLook() { return (CamTrack *)game_addr(game::g_CamScript_TrackLookAt); }
	inline CamTrack *TrackEye() { return (CamTrack *)game_addr(game::g_CamScript_TrackEye); }
	inline CamTrack *&TrackCur() { return var<CamTrack *>(game_addr(game::g_CamScript_CurTrack)); }
	inline CamTrack *&TrackOther() { return var<CamTrack *>(game_addr(game::g_CamScript_OtherTrack)); }
	inline int16_t &CamH() { return var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)); }
	inline int16_t &CamRoll() { return var<int16_t>(game_addr(game::g_BattleCam_Roll)); }
	static constexpr game::Id CAM_EYE = game::Battle_Camera_world_XZ_s16, CAM_AT = game::Battle_Camera_LookAt_XZ_s16;

	static int32_t CsArg(const int16_t **pp)
	{
		const int16_t *p = *pp;
		int32_t v = p[0];
		*pp = p + 1;
		if (v < 0) v = *(const int16_t *)(uint32_t)(game_addr(game::g_CamScript_VarTable) - (uint32_t)(2 * v + 2));
		return v;
	}

	static void CsVec(const int16_t **pp, int16_t *out)
	{
		const int16_t *p = *pp;
		out[0] = (int16_t)(p[0] * var<int16_t>(game_addr(game::g_CamScript_MirrorSign)));
		out[1] = p[1];
		out[2] = p[2];
		*pp = p + 3;
	}

	static int32_t CsEase(int32_t t, int32_t p)
	{
		int32_t ecx = t, w = p;
		if (p > 0)
		{
			for (int32_t n = p >> 8; n > 0; n--) ecx = ComputeSin(ecx / 4);
		}
		else if (p < 0)
		{
			w = -p;
			int32_t eax = 0x1000 - ecx;
			for (int32_t n = w >> 8; n > 0; n--) eax = ComputeSin(eax / 4);
			ecx = 0x1000 - eax;
		}
		else return t;
		w = (w & 0xFF) + 1;
		return (mul32(0x100 - w, t) + mul32(w, ecx)) >> 8;
	}

	static void CameraScriptStart(uint32_t script, const Mat4x3 *frame, void *entity, TaskQueue *q, int32_t n)
	{
		CamNode *m = (CamNode *)AddTaskToQueue(q, game_addr(ORIG_CamScriptTask));
		m->script = (const int16_t *)script;
		m->frame = frame;
		m->c = 0;
		m->e = 0;
		m->entity = entity;
		if (n != 0)
		{
			int32_t r = CrtRand() % n;
			var<int32_t>(game_addr(game::g_CamScript_MirrorSign)) = r != 1 ? 1 : -1;
		}
		else var<int32_t>(game_addr(game::g_CamScript_MirrorSign)) = 1;
	}

	static uint32_t CsRun(CamNode *n, bool *cut)
	{
		uint8_t *S = (uint8_t *)FieldAlloc(0x1B0);
		int16_t *s16 = (int16_t *)S;
		CamTrack *T0 = TrackLook(), *T1 = TrackEye();
		if (!(var<uint8_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C)) & 1))
		{
			if (n->c == 0)
			{
				T0->dur = 0;
				T1->dur = 0;
			}
			while (T1->dur == 0 || T0->dur == 0)
			{
				const int16_t *p = n->script;
				uint16_t word = (uint16_t)p[0];
				n->script = p + 1;
				bool eye = (word & 0x40) != 0;
				CamTrack *cur = eye ? T1 : T0, *oth = eye ? T0 : T1;
				TrackCur() = cur;
				TrackOther() = oth;
				word = (uint16_t)(word & 0xFF3F);
				cur->op = (int16_t)word;
				int32_t op = (int16_t)word;
				if ((uint32_t)op > 10)
				{
					FieldFree(0x1B0);
					return TASK_END;
				}
				switch (op)
				{
				case 0:
					cur->dur = (int16_t)CsArg(&n->script);
					break;
				case 1:
					CsVec(&n->script, cur->pos);
					if (cut) *cut = true;
					break;
				case 2:
					cur->dur = (int16_t)CsArg(&n->script);
					CsVec(&n->script, s16);
					for (int i = 0; i < 3; i++) cur->delta[i] = (int16_t)(((int32_t)s16[i] - cur->pos[i]) / (int32_t)cur->dur);
					break;
				case 3:
				{
					cur->dur = (int16_t)CsArg(&n->script);
					const int16_t *q = n->script;
					cur->ease = q[0];
					n->script = q + 1;
					cur->acc = 0;
					cur->step = 0x1000 / (int32_t)cur->dur;
					for (int i = 0; i < 3; i++) cur->start[i] = cur->pos[i];
					CsVec(&n->script, s16);
					for (int i = 0; i < 3; i++) cur->delta[i] = (int16_t)(s16[i] - cur->pos[i]);
					break;
				}
				case 4:
				{
					cur->dur = (int16_t)CsArg(&n->script);
					const int16_t *q = n->script;
					int32_t w = q[0];
					n->script = q + 1;
					cur->ease = (int16_t)(w & 0xFFFFFFF0);
					cur->npts = (int16_t)(w & 0xF);
					cur->acc = 0;
					cur->step = 0x1000 / (int32_t)cur->dur;
					ff8fx::copy<(8)>(cur->pts[0], cur->pos);
					for (int i = 1; i <= cur->npts; i++) CsVec(&n->script, cur->pts[i]);
					break;
				}
				case 5:
				{
					for (int i = 0; i < 3; i++) s16[i] = (int16_t)(oth->pos[i] - cur->pos[i]);
					uint32_t d2 = (uint32_t)mul32(s16[0], s16[0]) + (uint32_t)mul32(s16[1], s16[1]) + (uint32_t)mul32(s16[2], s16[2]);
					cur->acc = Sqrt((int32_t)d2);
					ff8fx::copy<(8)>(cur->start, cur->pos);
					cur->dur = (int16_t)CsArg(&n->script);
					CsVec(&n->script, s16);
					for (int i = 0; i < 3; i++) cur->delta[i] = (int16_t)(((int32_t)s16[i] - cur->pos[i]) / (int32_t)cur->dur);
					CamTrack *o = TrackOther();
					switch (o->op - 2)
					{
					case 0:
					{
						int16_t k = cur->dur;
						for (int i = 0; i < 3; i++) s16[4 + i] = (int16_t)((int16_t)(o->delta[i] * k) + o->pos[i]);
						break;
					}
					case 1:
						for (int i = 0; i < 3; i++) s16[4 + i] = (int16_t)(o->delta[i] + o->start[i]);
						break;
					case 2:
						ff8fx::copy<(8)>(S + 8, cur->pts[cur->npts]);
						break;
					default:
						ff8fx::copy<(8)>(S + 8, o->pos);
						break;
					}
					for (int i = 0; i < 3; i++) s16[i] = (int16_t)(s16[i] - s16[4 + i]);
					d2 = (uint32_t)mul32(s16[0], s16[0]) + (uint32_t)mul32(s16[1], s16[1]) + (uint32_t)mul32(s16[2], s16[2]);
					int32_t len = Sqrt((int32_t)d2);
					cur->step = (len - cur->acc) / (int32_t)cur->dur;
					break;
				}
				case 6:
				{
					const int16_t *q = n->script;
					CamH() = q[0];
					n->script = q + 1;
					break;
				}
				case 7:
				{
					cur->dur = (int16_t)CsArg(&n->script);
					int16_t h = CamH();
					const int16_t *q = n->script;
					cur->acc = (int32_t)q[0] - (int32_t)h;
					n->script = q + 1;
					cur->step = (int32_t)cur->dur;
					cur->ease = h;
					break;
				}
				case 8:
				{
					const int16_t *q = n->script;
					CamRoll() = (int16_t)(var<int16_t>(game_addr(game::g_CamScript_MirrorSign)) * q[0]);
					n->script = q + 1;
					break;
				}
				case 9:
				{
					cur->dur = (int16_t)CsArg(&n->script);
					const int16_t *q = n->script;
					int16_t r = CamRoll();
					cur->acc = mul32(q[0], var<int32_t>(game_addr(game::g_CamScript_MirrorSign))) - (int32_t)r;
					n->script = q + 1;
					cur->step = (int32_t)cur->dur;
					cur->ease = r;
					break;
				}
				case 10:
				{
					ff8fx::copy<(8)>(cur->pos, (const void *)(eye ? game_addr(CAM_EYE) : game_addr(CAM_AT)));
					Mat4x3 inv;
					UnpackRotationMatrix(n->frame, &inv);
					CamTrack *c2 = TrackCur();
					const Mat4x3 *f = n->frame;
					c2->pos[0] = (int16_t)(c2->pos[0] - (int16_t)f->t[0]);
					c2->pos[1] = (int16_t)(c2->pos[1] - (int16_t)f->t[1]);
					c2->pos[2] = (int16_t)(c2->pos[2] - (int16_t)f->t[2]);
					MatVec(&inv, c2->pos, c2->pos);
					if (cut) *cut = true;
					break;
				}
				}
			}
			TrackCur() = T1;
			TrackOther() = T0;
			for (int it = 2; it; it--)
			{
				CamTrack *cur = TrackCur();
				switch (cur->op - 2)
				{
				case 0:
					for (int i = 0; i < 3; i++) cur->pos[i] = (int16_t)(cur->pos[i] + cur->delta[i]);
					break;
				case 1:
				{
					cur->acc += cur->step;
					int32_t e = CsEase(cur->acc, cur->ease);
					for (int i = 0; i < 3; i++) cur->pos[i] = (int16_t)((mul32(cur->delta[i], e) >> 12) + cur->start[i]);
					break;
				}
				case 2:
				{
					cur->acc += cur->step;
					int32_t e = CsEase(cur->acc, cur->ease);
					CurveCoeffs(cur->npts + 1, cur->pts, S + 0x20);
					CurveEval(cur->npts + 1, S + 0x20, cur->pos, e);
					break;
				}
				case 3:
				{
					CamTrack *o = TrackOther();
					for (int i = 0; i < 3; i++) cur->start[i] = (int16_t)(cur->start[i] + cur->delta[i]);
					cur->acc += cur->step;
					int32_t *v = (int32_t *)(S + 0x10);
					for (int i = 0; i < 3; i++) v[i] = (int32_t)o->pos[i] - (int32_t)cur->start[i];
					NormalizeQ12(v, s16);
					for (int i = 0; i < 3; i++) cur->pos[i] = (int16_t)(o->pos[i] - (int16_t)(mul32(s16[i], cur->acc) >> 12));
					break;
				}
				case 5:
					CamH() = (int16_t)(mul32(cur->step - cur->dur + 1, cur->acc) / cur->step + cur->ease);
					break;
				case 7:
					CamRoll() = (int16_t)(mul32(cur->step - cur->dur + 1, cur->acc) / cur->step + cur->ease);
					break;
				default:
					break;
				}
				cur->dur--;
				TrackCur() = T0;
				TrackOther() = T1;
			}
		}
		var<uint32_t>(game_addr(game::unk_24FD35C)) = var<uint32_t>(game_addr(game::Battle_Camera_world_Y));
		var<uint32_t>(game_addr(game::g_CamScript_SavedLookAt)) = var<uint32_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16));
		var<uint32_t>(game_addr(game::g_CamScript_SavedEye)) = var<uint32_t>(game_addr(game::Battle_Camera_world_XZ_s16));
		var<uint32_t>(game_addr(game::unk_24FD254)) = var<uint32_t>(game_addr(game::Battle_Camera_LookAt_Y));
		MatVec(n->frame, TrackEye()->pos, s16);
		for (int i = 0; i < 3; i++) s16[i] = (int16_t)(s16[i] + (int16_t)n->frame->t[i]);
		MatVec(n->frame, TrackLook()->pos, s16 + 4);
		for (int i = 0; i < 3; i++) s16[4 + i] = (int16_t)(s16[4 + i] + (int16_t)n->frame->t[i]);
		var<uint32_t>(game_addr(game::Battle_Camera_world_XZ_s16)) = *(uint32_t *)(S + 0);
		var<uint32_t>(game_addr(game::Battle_Camera_world_Y)) = *(uint32_t *)(S + 4);
		var<uint32_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16)) = *(uint32_t *)(S + 8);
		var<uint32_t>(game_addr(game::Battle_Camera_LookAt_Y)) = *(uint32_t *)(S + 0xC);
		FieldFree(0x1B0);
		if (!(var<uint8_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C)) & 1)) n->c++;
		return 0;
	}

	static uint32_t __cdecl CamScriptTask(TaskNode *tn)
	{
		CamNode *n = (CamNode *)tn;
		uint32_t r = CsRun(n, nullptr);
		return r;
	}

	static uint32_t __cdecl SequenceTick(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int left = ExecuteTaskQueue(&SubQueue());
		n->c++;
		return left ? 0 : TASK_END;
	}

	static Node24 *LinkTask(uint32_t task_fn)
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&SubQueue(), task_fn);
		n->c = 0;
		n->e = 0;
		return n;
	}
	static Node24 *AddTask(uint32_t task_fn) { return (Node24 *)AddTaskToQueue(&SubQueue(), task_fn); }

	static void BsFlagSet() { for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) var<uint8_t>(a) |= 2; }
	static void BsFlagClear() { for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) var<uint8_t>(a) &= 0xFD; }

	static void BuildTargetList()
	{
		int32_t *list = TargetList();
		int32_t cnt = 0;
		uint8_t *t = *(uint8_t **)(CastCtx() + 4);
		int32_t n = t[0x10];
		if (n > 0)
		{
			uint8_t *rec = *(uint8_t **)(t + 8);
			for (int32_t k = n; k; k--, rec += 0x18)
			{
				uint32_t slot = rec[0];
				if (cnt > 0)
				{
					int32_t i = 0;
					while (i < cnt && (uint32_t)list[i] != slot) i++;
					if (i < cnt) continue;
				}
				list[cnt] = (int32_t)slot;
				cnt++;
				uint8_t *link = *(uint8_t **)(game_addr(game::unk_1D9734C) + 156 * slot);
				uint8_t *self = Entity(slot);
				if (!link || link == self) continue;
				int32_t *out = &list[cnt];
				do
				{
					if (link[0] & 2)
					{
						int32_t j = 0;
						uint8_t *e = (uint8_t *)game_addr(game::BattleEntitySlotData);
						while (e != link)
						{
							e += 0x9C;
							j++;
							if (e >= (uint8_t *)game_addr(game::CAMERA_FLAG_RELATED)) break;
						}
						if (e == link && e < (uint8_t *)game_addr(game::CAMERA_FLAG_RELATED) && j < 7)
						{
							*out++ = j;
							cnt++;
						}
					}
					link = *(uint8_t **)(link + 0x8C);
				} while (link != self);
			}
		}
		TargetCount() = cnt;
	}

	static void SaveTargets()
	{
		for (int32_t i = 0; i < TargetCount(); i++)
		{
			int32_t slot = TargetList()[i];
			ff8fx::copy<(8)>((void *)(game_addr(game::unk_24FD260) + 8 * slot), Entity(slot) + 0x1C);
			ff8fx::copy<(8)>((void *)(game_addr(game::unk_24FC0C8) + 8 * slot), Entity(slot) + 0x94);
		}
	}
	static void RestoreTargets()
	{
		for (int32_t i = 0; i < TargetCount(); i++)
		{
			int32_t slot = TargetList()[i];
			ff8fx::copy<(8)>(Entity(slot) + 0x1C, (const void *)(game_addr(game::unk_24FD260) + 8 * slot));
			ff8fx::copy<(8)>(Entity(slot) + 0x94, (const void *)(game_addr(game::unk_24FC0C8) + 8 * slot));
		}
	}
	static void SaveTargetsAndMove()
	{
		for (int32_t i = 0; i < TargetCount(); i++)
		{
			int32_t slot = TargetList()[i];
			uint8_t *E = Entity(slot);
			*(uint32_t *)(game_addr(game::unk_24FD260) + 8 * slot) = *(uint32_t *)(E + 0x1C);
			uint32_t z = *(uint32_t *)(E + 0x20);
			*(int16_t *)(E + 0x20) = (int16_t)(*(int16_t *)(E + 0x20) + (int16_t)0xA240);
			*(uint32_t *)(game_addr(game::unk_24FD264) + 8 * slot) = z;
		}
	}
	static void HideTargets() { for (int32_t i = 0; i < TargetCount(); i++) *(uint8_t *)Entity(TargetList()[i]) |= 4; }
	static void ShowTargets() { for (int32_t i = 0; i < TargetCount(); i++) *(uint16_t *)Entity(TargetList()[i]) &= 0xFFFB; }

	static void SpawnRigs();
	static void SpawnSky();
	static void LinkFlash(uint32_t task_fn);
	static void LinkSteamPuff();
	static void LinkSteamPair();

	static uint32_t __cdecl TimelineTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		ff8fx::copy<(sizeof(Mat4x3))>((void *)game_addr(game::unk_24FBDA8), &Camera());
		ComposeAffineTransform(&Camera(), &Root(), &Frame());
		uint32_t fl = Flags();
		if (fl & 0x201)
		{
			if (fl & 1) return 0;
			if (LoadState() < 0) return 0;
		}
		if (n->c == 0)
		{
			CameraScriptStart(game_addr(game::unk_E3CA88), &Root(), Creature(), &SubQueue(), 0);
			BuildTargetList();
			SaveTargets();
			BdTransSummonStream(game_addr(game::unk_F78380), game_addr(game::unk_24FC280));
			VoiceSlot() = ClaimVoiceSlot((const void *)game_addr(game::unk_E3C774), 1, 0x80);
		}
		switch (n->c)
		{
		case 1: SpawnRigs(); break;
		case 4: BdPlaySE(game_addr(game::unk_E3C8A8), 0, 0x80); break;
		case 5: CharacterLoad(0x22B, MB() + 0x10000); break;
		case 15:
			if (LoadState() < 0) return 0;
			QueueTIMUpload(MB() + 0x10000);
			CharacterLoad(0x22C, MB());
			LinkFlash(game_addr(ORIG_FlashInTask));
			break;
		case 20:
			SpawnSky();
			LinkTask(game_addr(ORIG_RainTask));
			break;
		case 30:
			if (LoadState() < 0) return 0;
			CharacterLoad(0x22D, MB() + 0x10000);
			break;
		case 85:
			if (LoadState() < 0) return 0;
			CharacterLoad(0x22E, SummonData());
			LinkSteamPuff();
			break;
		case 95: case 178: case 313: case 355:
			if (LoadState() < 0) return 0;
			BdTransSummonStream(SummonData(), game_addr(game::unk_24FC280));
			break;
		case 103:
			HideTargets();
			LinkSteamPair();
			break;
		case 124:
			BdPlaySummonStream(0x80, 0, 0x60);
			LinkTask(game_addr(ORIG_CreatureTask));
			break;
		case 160: BdPlaySE(game_addr(game::unk_E3C8AC), 0, 0x80); break;
		case 168: CharacterLoad(0x22F, SummonData()); break;
		case 293: CharacterLoad(0x230, SummonData()); break;
		case 294:
			BdPlaySE(game_addr(game::unk_E3C8B0), 0, 0x80);
			BdPlaySummonStream(0x80, 0, 0x60);
			break;
		case 335:
			BdPlaySummonStream(0x80, 0, 0x60);
			CharacterLoad(0x231, SummonData());
			break;
		case 336: BdPlaySE(game_addr(game::unk_E3C8B4), 0, 0x80); break;
		case 366: BdPlaySummonStream(0x80, 0, 0x60); break;
		case 405:
			LinkFlash(game_addr(ORIG_FlashOutTask));
			QueueTIMUpload(MB() + 0x10000);
			break;
		case 406: LinkTask(game_addr(ORIG_EndTask)); break;
		case 417: BdPlaySE(game_addr(game::unk_E3C8B8), 0, 0x80); break;
		case 420:
			if (VoiceSlotBusy(VoiceSlot())) ReleaseVoiceSlot(VoiceSlot());
			break;
		}
		if (n->c == 0x1A4)
		{
			uint8_t *t = *(uint8_t **)(CastCtx() + 4);
			ApplyActionResultToTargets(*(uint32_t *)(t + 8), t[0x10]);
		}
		n->c++;
		if (n->c <= 0x1A7) return 0;
		BsFlagSet();
		ShowTargets();
		SetScreenFlash(0, 0);
		return TASK_END;
	}

	static void LinkFlash(uint32_t task_fn)
	{
		Node24 *n = AddTask(task_fn);
		n->c = 0;
		n->e = 0x10;
		n->f18 = 0x1000;
		n->f1A = 1;
	}

	static uint32_t __cdecl FlashInTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		SetScreenFlash((uint32_t)mul32((int32_t)n->f18 / (int32_t)n->e, n->c), 0);
		if (P1()) return 0;
		n->c++;
		if (n->c < n->e) return 0;
		if (n->f1A) BsFlagClear();
		return TASK_END;
	}

	static uint32_t __cdecl FlashOutTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int32_t level = n->f18;
		SetScreenFlash((uint32_t)(level - mul32(level / (int32_t)n->e, n->c)), 0);
		if (n->c == 0 && n->f1A) BsFlagSet();
		if (P1()) return 0;
		n->c++;
		if (n->c < n->e) return 0;
		SetScreenFlash(0, 0);
		return TASK_END;
	}

	static uint32_t __cdecl EndTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->c == 1) WaitAnimSeq((void *)(MB() + 0x10000), &n->e);
		uint16_t done = (uint16_t)n->e;
		n->c++;
		return done ? TASK_END : 0;
	}

	static void SpawnRigs()
	{
		Node24 *a = AddTask(game_addr(ORIG_RigTask));
		a->c = 0;
		a->e = 0;
		a->f10 = 0xC80;
		a->f12 = 0;
		a->f14 = (int16_t)0xF380;
		a->f16 = 0x67;
		a->f18 = (int16_t)0xFC00;
		a->f1A = 1;
		a->f1C = 0x1000;
		a->f20 = 1;
		Node24 *b = AddTask(game_addr(ORIG_RigTask));
		b->e = 1;
		b->f1A = 1;
		b->c = 0;
		b->f12 = 0;
		b->f20 = 0;
		b->f10 = (int16_t)0xF380;
		b->f14 = 0xC80;
		b->f16 = 0x67;
		b->f18 = 0x400;
		b->f1C = 0x1000;
	}

	static uint32_t __cdecl RigTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		Mat4x3 *M = RigM(n->e), *W = RigW(n->e);
		if (!P1())
		{
			if (n->c == 0)
			{
				int16_t ang[3] = { 0, n->f18, 0 };
				ComposeZYXRotationMatrix(ang, M);
				M->t[0] = n->f10;
				M->t[1] = n->f12;
				M->t[2] = n->f14;
				int32_t s = n->f1C;
				int32_t v[3] = { s, s, s };
				Scale3DMatrix(M, v);
				Node24 *a = AddTask(game_addr(ORIG_RigPrimATask));
				a->c = 0; a->e = n->e; a->f10 = 0; a->f12 = 0; a->f14 = 0; a->f16 = n->f16; a->f1C = 0x800;
				if (n->f20)
				{
					Node24 *b = AddTask(game_addr(ORIG_RigPrimBTask));
					b->c = 0; b->e = n->e; b->f10 = 0; b->f12 = 0; b->f14 = 0; b->f16 = n->f16; b->f1C = 0x800;
				}
				Node24 *r = AddTask(game_addr(ORIG_RigRockTask));
				r->c = 0; r->e = n->e; r->f10 = 0x208; r->f12 = (int16_t)0xFDDA; r->f14 = 0x1CC; r->f16 = n->f16; r->f1C = 0x600; r->f20 = 0x600;
				if (n->f1A) { r->f18 = (int16_t)0xFC00; r->f1A = (int16_t)0xFFCD; }
				else { r->f18 = 0; r->f1A = 0; }
				if (n->f20)
				{
					Node24 *s2 = AddTask(game_addr(ORIG_RigSpriteTask));
					s2->c = 0; s2->e = n->e; s2->f10 = 0; s2->f12 = (int16_t)0xFD30; s2->f14 = (int16_t)0xFF4C; s2->f16 = n->f16; s2->f1C = 0x600;
				}
			}
			int32_t c = n->c;
			if (c % 7 == 0 && n->f20 && n->c < 0x50)
			{
				int16_t x = ((c / 7) & 1) ? 0xB4 : -0xB4;
				Node24 *f = AddTask(game_addr(ORIG_RigFlashTask));
				f->c = 0; f->e = n->e; f->f10 = x; f->f12 = (int16_t)0xFB64; f->f14 = (int16_t)0xFF42; f->f1C = 0x780;
				BdPlaySummonStream(0x80, 0, 0x60);
			}
			n->c++;
		}
		ComposeAffineTransform(&Frame(), M, W);
		return n->c > n->f16 ? TASK_END : 0;
	}

	static void RigMatrix(const Mat4x3 *W, const int16_t *ang, const Node24 *n, int32_t sx, int32_t sy, int32_t sz)
	{
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = n->f10;
		m.t[1] = n->f12;
		m.t[2] = n->f14;
		int32_t v[3] = { sx, sy, sz };
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(W, &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
	}

	static void PrimModel(uint32_t model, int32_t mode, uint32_t h1C)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = model;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = h1C;
		Cursor() = RenderPrimModel(h, OT(), mode, Cursor());
		FieldFree(0x58);
	}

	static void RigPrimADraw(const Mat4x3 *W, const Node24 *n)
	{
		int16_t ang[3] = { 0, 0, 0 };
		RigMatrix(W, ang, n, n->f1C, n->f1C, n->f1C);
		PrimModel(game_addr(game::unk_E375BC), 2, 0x30);
	}
	static uint32_t __cdecl RigPrimATask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		RigPrimADraw(RigW(n->e), n);
		if (P1()) return 0;
		n->c++;
		return n->c >= n->f16 ? TASK_END : 0;
	}

	static void RigPrimBDraw(const Mat4x3 *W, const Node24 *n)
	{
		int16_t ang[3] = { 0, 0, 0 };
		RigMatrix(W, ang, n, n->f1C, n->f1C, n->f1C);
		PrimModel(game_addr(game::unk_E38E78), 3, 0);
	}
	static uint32_t __cdecl RigPrimBTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		RigPrimBDraw(RigW(n->e), n);
		if (P1()) return 0;
		n->c++;
		return n->c >= n->f16 ? TASK_END : 0;
	}

	static void RigRockDraw(const Mat4x3 *W, const Node24 *n, int16_t roll)
	{
		int16_t ang[3] = { 0, 0, roll };
		RigMatrix(W, ang, n, n->f1C, n->f20, n->f20);
		PrimModel(game_addr(game::unk_E396B0), 2, 0);
	}
	static uint32_t __cdecl RigRockTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		RigRockDraw(RigW(n->e), n, n->f18);
		if (P1()) return 0;
		int16_t c = n->c;
		int32_t k = c;
		if (k >= 0x19)
		{
			k -= 0x19;
			if (k < 0x14) n->f18 = (int16_t)(n->f18 - n->f1A);
			else if ((k -= 0x14) < 6)
			{
				int32_t s = ComputeSin(shl32(k, 10) / 6 + 0x400);
				int32_t v = mul32(s, 20) >> 12;
				if (!(c & 1)) v = -v;
				n->f18 = (int16_t)(n->f18 + v);
			}
			else n->f18 = 0;
		}
		n->c = (int16_t)(c + 1);
		return n->c >= n->f16 ? TASK_END : 0;
	}

	static int32_t RigFlashFade(int32_t c) { return mul32(c - 2, 682); }
	static void RigFlashDraw(const Mat4x3 *W, const Node24 *n, int16_t c, bool fade_override, int32_t fade)
	{
		int16_t ang[3] = { 0, 0, 0 };
		RigMatrix(W, ang, n, n->f1C, n->f1C, n->f1C);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = game_addr(game::unk_E3B83C);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 3;
		if (c == 0)
		{
			*(uint32_t *)(h + 0xC) = 0x800;
			*(uint32_t *)(h + 0x1C) = 0xC3;
		}
		else if (c >= 2)
		{
			*(int32_t *)(h + 0xC) = fade_override ? fade : RigFlashFade(c);
			*(uint32_t *)(h + 0x1C) = 0xC3;
		}
		Cursor() = RenderPrimModel(h, OT(), 4, Cursor());
		FieldFree(0x58);
	}
	static uint32_t __cdecl RigFlashTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		RigFlashDraw(RigW(n->e), n, n->c, false, 0);
		if (P1()) return 0;
		n->c++;
		return n->c >= 8 ? TASK_END : 0;
	}

	static void RigSpriteDraw(const Mat4x3 *W, const Node24 *n, int16_t c)
	{
		int16_t ang[3] = { 0, 0, 0 };
		RigMatrix(W, ang, n, n->f1C, n->f1C, n->f1C);
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint16_t *)(h + 4) = (uint16_t)(c & 7);
		*(uint32_t *)h = game_addr(game::unk_E346B0);
		*(uint16_t *)(h + 0x24) = 8;
		Cursor() = InitEffectSequenceFromData(h, OT(), 3, Cursor());
		FieldFree(0xB4);
	}
	static uint32_t __cdecl RigSpriteTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		RigSpriteDraw(RigW(n->e), n, n->c);
		if (P1()) return 0;
		n->c++;
		return n->c >= n->f16 ? TASK_END : 0;
	}

	static int16_t RandMod(int32_t m) { return (int16_t)(CrtRand() % m); }

	static void SpawnSky()
	{
		Node24 *n = AddTask(game_addr(ORIG_SkyTask));
		n->c = 0;
		n->f10 = RandMod(0x1000);
		n->f12 = (int16_t)(CrtRand() % 8 + 4);
		if (n->f10 & 1) n->f12 = (int16_t)-n->f12;
		n->f14 = RandMod(0x1000);
		n->f16 = (int16_t)(CrtRand() % 4 + 2);
		if (n->f14 & 1) n->f16 = (int16_t)-n->f16;
		n->f18 = RandMod(0x1000);
		n->f1A = 1;
		n->f1C = 0xB80;
		n->f1E = RandMod(0x20);
		n->f20 = RandMod(0x1000);
		n->f22 = (int16_t)(CrtRand() % 0x18 + 0x24);
		n = AddTask(game_addr(ORIG_SkyTask));
		n->c = 0;
		n->f10 = RandMod(0x1000);
		n->f12 = (int16_t)(CrtRand() % 8 + 4);
		if (n->f10 & 1) n->f12 = (int16_t)-n->f12;
		n->f14 = RandMod(0x1000);
		n->f16 = (int16_t)(CrtRand() % 4 + 2);
		if (n->f14 & 1) n->f16 = (int16_t)-n->f16;
		n->f18 = RandMod(0x1000);
		n->f1A = 1;
		n->f1C = 0xC00;
		n->f1E = RandMod(0x20);
		n->f20 = RandMod(0x800);
		n->f22 = (int16_t)(CrtRand() % 0x28 + 0x3C);
	}

	static int32_t SkyFade(int32_t c)
	{
		if (c < 0x20) return shl32(0x20 - c, 7);
		if (c >= 0x174) return shl32(c - 0x174, 8);
		return 0;
	}

	static void SkyDraw(int16_t ay, int16_t ax, int16_t az, int16_t f1A, int16_t scale, int16_t idx, int16_t phase, int32_t fade)
	{
		int16_t ang[3] = { ay, ax, az };
		Mat4x3 m;
		RotMatrixXYZ(ang, &m);
		int32_t v[3] = { scale, scale, scale };
		m.t[0] = 0;
		m.t[1] = 0;
		m.t[2] = 0x120;
		Scale3DMatrix(&m, v);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x6C);
		uint32_t mode = f1A ? 3 : 0xC;
		*(uint32_t *)h = game_addr(game::unk_E3496C);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = mode | 0x20C0;
		*(uint32_t *)(h + 0x20) = ((const uint32_t *)game_addr(game::unk_E3C8DC))[idx & 0x1F];
		*(int32_t *)(h + 0xC) = (ComputeSin(phase) + 0x1000) / 2;
		GteSetFarColor(0, 0, 0);
		GteLoadRGBC(h + 0x20);
		GteSetIR0(*(int32_t *)(h + 0xC));
		GteDPCS();
		GteStoreRGB2(h + 0x20);
		*(int32_t *)(h + 0xC) = fade;
		int32_t t = (ComputeSin((int32_t)phase >> 2) + 0x1000) / 2;
		int32_t count = var<int32_t>(game_addr(game::unk_E34970));
		int16_t *out = (int16_t *)(MB() + 0x37000);
		const int16_t *a = (const int16_t *)game_addr(game::unk_E34974), *b = (const int16_t *)game_addr(game::unk_E3702C);
		for (int32_t i = 0; i < count; i++, a += 4, b += 4, out += 4)
		{
			out[0] = (int16_t)((mul32(b[0] - a[0], t) >> 12) + a[0]);
			out[1] = (int16_t)((mul32(b[1] - a[1], t) >> 12) + a[1]);
			out[2] = (int16_t)((mul32(b[2] - a[2], t) >> 12) + a[2]);
		}
		*(uint32_t *)(h + 4) = MB() + 0x37000;
		ProjectionH(0x120);
		Cursor() = SkyRender(h, var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4078, 6, Cursor());
		ProjectionH(CamH());
		FieldFree(0x6C);
	}

	static uint32_t __cdecl SkyTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		SkyDraw(n->f18, n->f10, n->f14, n->f1A, n->f1C, n->f1E, n->f20, SkyFade(n->c));
		if (P1()) return 0;
		n->f10 = (int16_t)(n->f10 + n->f12);
		n->f14 = (int16_t)(n->f14 + n->f16);
		n->f1E++;
		n->f20 = (int16_t)(n->f20 + n->f22);
		n->c++;
		return n->c >= 0x184 ? TASK_END : 0;
	}

	static void LitFrame(uint8_t *s, const Mat4x3 *pre, const Mat4x3 *frame, int32_t tx, int32_t ty, int32_t tz)
	{
		*(int16_t *)s = 0;
		*(int16_t *)(s + 2) = 0;
		*(int16_t *)(s + 4) = 0;
		ComposeZYXRotationMatrix((const int16_t *)s, (Mat4x3 *)(s + 8));
		*(int32_t *)(s + 0x1C) = tx;
		*(int32_t *)(s + 0x20) = ty;
		*(int32_t *)(s + 0x24) = tz;
		if (pre) ComposeAffineTransform(pre, (Mat4x3 *)(s + 8), (Mat4x3 *)(s + 8));
		ComposeAffineTransform(frame, (Mat4x3 *)(s + 8), (Mat4x3 *)(s + 8));
		GteSetLightMatrix(s + 8);
		GteSetBackColorFromTrans(s + 8);
	}

	static void LitSprite(uint8_t *h, const int16_t *pos, int16_t size, int16_t frame, bool ir)
	{
		GteLoadV0(pos);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(size);
		*(int16_t *)(h + 4) = frame;
		if (ir) GteTransFromIR();
		else GteTransFromMAC();
		Cursor() = InitEffectSequenceFromData(h, OT(), 2, Cursor());
	}

	static void OffsetSprite(uint8_t *h, int32_t *mac, int32_t *nv, const int16_t *pos, int16_t size, int16_t frame, int shift)
	{
		GteLoadV0(pos);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(size);
		*(int16_t *)(h + 4) = frame;
		GteReadMAC123(mac);
		Normalize(mac, nv);
		int32_t k = -((int32_t)size >> shift);
		mac[0] += mul32(nv[0], k) >> 12;
		mac[1] += mul32(nv[1], k) >> 12;
		mac[2] += mul32(nv[2], k) >> 12;
		GteSetTransFromVec32(mac);
		Cursor() = InitEffectSequenceFromData(h, OT(), 2, Cursor());
	}

	static void LinkSteamPuff()
	{
		Node24 *n = AddTask(game_addr(ORIG_SteamPuffTask));
		n->c = 0;
		n->e = 1;
		n->f10 = 0;
		n->f12 = 0;
		n->f14 = (int16_t)0xF830;
		n->f18 = 0;
		n->f1C = 0x7D0;
		n->f1E = 0x1000;
		n->f20 = (int16_t)0xFFA6;
	}
	static void LinkSteamPair()
	{
		Node24 *n = AddTask(game_addr(ORIG_SteamPairTask));
		n->c = 0;
		n->e = 2;
		n->f10 = 0;
		n->f12 = (int16_t)0xFF38;
		n->f14 = (int16_t)0x8AD0;
		n->f1E = 0x800;
	}

	static void PuffLoop(const Node24 *n, bool ir)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		*(uint32_t *)h = game_addr(game::unk_E33E40);
		*(uint16_t *)(h + 0x24) = 8;
		LitFrame(s, nullptr, &Frame(), 0, 0, 0);
		for (int i = 0; i < 170; i++)
		{
			Rec *r = R(game_addr(POOL_A), i);
			if (!(r->mask & (uint32_t)(int32_t)n->e)) continue;
			LitSprite(h, r->pos, r->size, r->age, ir);
			if (P1()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0) r->age = 0;
		}
		FieldFree(0x48);
		FieldFree(0xB4);
	}

	static void ClearMask(uint32_t cell, int count, uint32_t mask, bool dword_test)
	{
		uint8_t *p = Pool(cell);
		for (int i = 0; i < count; i++, p += 0x18)
			if (dword_test ? (*(uint32_t *)p & mask) != 0 : (p[0] & mask) != 0) *(uint32_t *)p = 0;
	}

	static uint32_t __cdecl SteamPuffTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		PuffLoop(n, true);
		if (P1()) return 0;
		FieldAlloc(0x48);
		if (n->c <= 0x12)
		{
			for (int k = 0; k < 1; k++)
			{
				int i = FreeRec(game_addr(POOL_A), 170);
				if (i < 0) break;
				int32_t s0 = ComputeSin(n->f18);
				int32_t ang = mul32(s0, 180) >> 12;
				uint32_t e = (uint32_t)(int32_t)n->e;
				Rec *a = R(game_addr(POOL_A), i);
				a->mask = e;
				a->age = 0;
				a->size = n->f1E;
				ff8fx::copy<(8)>(a->pos, &n->f10);
				int32_t t1 = ang - 0x400;
				int32_t sa = ComputeSin(t1);
				a->pos[0] = (int16_t)(a->pos[0] + (mul32(sa, n->f1C) >> 12));
				int32_t ca = ComputeCos(t1);
				a->pos[2] = (int16_t)(a->pos[2] + (mul32(ca, n->f1C) >> 12));
				Rec *b = R(game_addr(POOL_A), i + 1);
				b->mask = e;
				b->age = 0;
				b->size = n->f1E;
				ff8fx::copy<(8)>(b->pos, &n->f10);
				int32_t t2 = ang + 0x400;
				int32_t sb = ComputeSin(t2);
				b->pos[0] = (int16_t)(b->pos[0] + (mul32(sb, n->f1C) >> 12));
				int32_t cb = ComputeCos(t2);
				b->pos[2] = (int16_t)(b->pos[2] + (mul32(cb, n->f1C) >> 12));
				int32_t sd = ComputeSin(ang);
				n->f10 = (int16_t)(n->f10 + (mul32(sd, 2000) >> 12));
				n->f12 = (int16_t)(n->f12 - 0x14);
				int32_t cd = ComputeCos(ang);
				n->f18 = (int16_t)(n->f18 + 0xA0);
				n->f1E = (int16_t)(n->f1E - 0x58);
				n->f14 = (int16_t)(n->f14 + (mul32(cd, 2000) >> 12));
				int16_t g = n->f20;
				n->f1C = (int16_t)(n->f1C + g);
				n->f20 = (int16_t)(g - 1);
			}
		}
		FieldFree(0x48);
		n->c++;
		if (n->c < 0x12) return 0;
		ClearMask(game_addr(POOL_A), 170, (uint32_t)(int32_t)n->e, true);
		return TASK_END;
	}

	static uint32_t __cdecl SteamPairTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		PuffLoop(n, false);
		if (P1()) return 0;
		FieldAlloc(0x48);
		if (n->c <= 0x14)
		{
			for (int k = 0; k < 2; k++)
			{
				int i = FreeRec(game_addr(POOL_A), 170);
				if (i < 0) break;
				uint32_t e = (uint32_t)(int32_t)n->e;
				int16_t sz = n->f1E;
				Rec *a = R(game_addr(POOL_A), i);
				a->mask = e;
				a->age = 0;
				a->size = sz;
				ff8fx::copy<(8)>(a->pos, &n->f10);
				a->pos[0] = (int16_t)(a->pos[0] - 900);
				Rec *b = R(game_addr(POOL_A), i + 1);
				b->mask = e;
				b->age = 0;
				b->size = sz;
				ff8fx::copy<(4)>(b->pos, &n->f10);
				ff8fx::copy<(4)>(&b->pos[2], &n->f14);
				n->f14 = (int16_t)(n->f14 + 0x5DC);
				b->pos[0] = (int16_t)(b->pos[0] + 900);
				n->f1E = (int16_t)(sz - 0x10);
			}
		}
		FieldFree(0x48);
		n->c++;
		if (n->c < 0x5A) return 0;
		ClearMask(game_addr(POOL_A), 170, (uint32_t)(int32_t)n->e, true);
		return TASK_END;
	}

	static void RainTemplate(uint8_t *T)
	{
		T[0x3C] = 0xA0;
		T[0x34] = 0xA0;
		T[0x40] = 0xC0;
		T[0x38] = 0xC0;
		T[0x39] = 0xC0;
		T[0x35] = 0xC0;
		T[0x41] = 0xE0;
		T[0x3D] = 0xE0;
		*(int16_t *)(T + 0x10) = (int16_t)0xFF00;
		*(int16_t *)(T + 0x12) = (int16_t)0xFF00;
		*(int16_t *)(T + 0x1A) = (int16_t)0xFF00;
		*(int16_t *)(T + 0x20) = (int16_t)0xFF00;
		*(uint32_t *)(T + 0x30) = 0x2E808080;
		*(uint16_t *)(T + 0x36) = 0x3EE4;
		*(uint16_t *)(T + 0x3A) = 0x36;
		*(int16_t *)(T + 0x18) = 0x100;
		*(int16_t *)(T + 0x22) = 0x100;
		*(int16_t *)(T + 0x24) = 0;
		*(int16_t *)(T + 0x1C) = 0;
		*(int16_t *)(T + 0x14) = 0;
	}

	static bool RainQuad(uint8_t *T, uint32_t &cursor, uint32_t color, int16_t size)
	{
		uint32_t *pk = (uint32_t *)cursor;
		pk[1] = color;
		GteSetRotScale(size);
		GteTransFromMAC();
		GteLoadV012(T + 0x10, T + 0x18, T + 0x20);
		GteRTPT();
		pk[3] = *(uint32_t *)(T + 0x34);
		pk[0] = 0x09000000;
		pk[5] = *(uint32_t *)(T + 0x38);
		pk[7] = *(uint32_t *)(T + 0x3C);
		pk[9] = *(uint32_t *)(T + 0x40);
		GteReadFLAG(T + 8);
		if (*(uint32_t *)(T + 8) & 0x60000) return false;
		GteReadSXY012Split(pk + 2, pk + 4, pk + 6);
		*(int16_t *)((uint8_t *)pk + 0x20) = *(int16_t *)((uint8_t *)pk + 0x10);
		*(int16_t *)((uint8_t *)pk + 0x22) = *(int16_t *)((uint8_t *)pk + 0x1A);
		GteReadSZ3(T + 0xC);
		int32_t z = *(int32_t *)(T + 0xC);
		InsertPrimAutoDepth(*(uint32_t *)T + 4 * (uint32_t)(z >> (*(uint32_t *)(T + 4) & 31)), pk);
		cursor += 0x28;
		return true;
	}

	static const uint32_t *RAIN_COLOR() { return (const uint32_t *)game_addr(game::unk_E3C988); }

	static uint32_t __cdecl RainTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *s = (uint8_t *)FieldAlloc(0x50) + 0x10;
		uint8_t *T = (uint8_t *)FieldAlloc(0x44);
		RainTemplate(T);
		ff8fx::copy<(sizeof(Mat4x3))>(s, &Camera());
		int32_t count = 0;
		GteSetLightMatrix(s);
		GteSetBackColorFromTrans(s);
		*(uint32_t *)T = OT();
		*(uint32_t *)(T + 4) = (var<uint8_t>(game_addr(game::g191_StateFlags)) & 1) ? 0xE : 4;
		uint32_t cursor = Cursor();
		for (int i = 0; i < 590; i++)
		{
			Rec *r = R(game_addr(POOL_F), i);
			if (!(*(uint8_t *)r & 1)) continue;
			int32_t fr = r->age;
			GteLoadV0(r->pos);
			GteMVMVA_LightV0Bk();
			uint32_t color = RAIN_COLOR()[fr];
			if (!P1())
			{
				fr++;
				r->age = (int16_t)fr;
				if (fr >= 0x40) r->mask = 0;
				else
				{
					r->pos[0] = (int16_t)(r->pos[0] + r->vel[0]);
					r->pos[1] = (int16_t)(r->pos[1] + r->vel[1]);
					r->pos[2] = (int16_t)(r->pos[2] + r->vel[2]);
				}
			}
			if (RainQuad(T, cursor, color, r->size)) count++;
		}
		Cursor() = cursor;
		FieldFree(0x44);
		FieldFree(0x50);
		if (P1()) return 0;
		FieldAlloc(0x50);
		if (n->c < 0x134)
		{
			for (int k = 0; k < 0x1C; k++)
			{
				int i = FreeRec(game_addr(POOL_F), 590);
				if (i < 0) break;
				Rec *r = R(game_addr(POOL_F), i);
				r->mask = 1;
				r->age = 0;
				r->size = (int16_t)(CrtRand() % 0x600 + 0x600);
				r->pos[0] = (int16_t)(2 * (CrtRand() % 20000) - 20000);
				r->pos[1] = (int16_t)(CrtRand() % 20000 - 10000);
				r->pos[2] = (int16_t)(2 * (CrtRand() % 20000) - 20000);
				r->vel[0] = (int16_t)(CrtRand() % 0x30 - 0x18);
				r->vel[1] = (int16_t)(CrtRand() % 0x30 - 0x18);
				r->vel[2] = (int16_t)(CrtRand() % 0x30 - 0x18);
			}
		}
		FieldFree(0x50);
		n->c++;
		if (n->c >= 0x134 && count == 0) return TASK_END;
		return 0;
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
		*(uint8_t **)(data + 0xC) = buf + *(uint32_t *)(buf + 4);
		*(uint8_t **)(data + 0x10) = buf + *(uint32_t *)(buf + 8);
		*(uint8_t **)(data + 0x14) = buf + *(uint32_t *)(buf + 0xC);
		*(uint16_t *)(data + 2) = (uint16_t)id;
		*(uint8_t **)(data + 0x30) = buf + *(uint32_t *)(buf + 0x10);
	}

	static void SetAnim(uint8_t *E, int32_t id) { ReaderSetAnim(E + 0x60, E + 0x6C, id); }

	static void AdvanceLoop(uint8_t *E)
	{
		if (ReaderRead(E + 0x60, E + 0x6C) == 1 && !(E[0] & 1))
			ReaderSetAnim(E + 0x60, E + 0x6C, (int32_t)E[0x6C]);
	}

	static void DrawModel(uint8_t *E, const Mat4x3 *frame)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x4C);
		uint32_t mask = ComposeBones(E + 0x60, frame, (const Mat4x3 *)(E + 0x40));
		*(uint32_t *)(h + 0x30) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		*(uint32_t *)(h + 0x24) = var<uint32_t>(game_addr(game::g191_VertexScratch));
		*(uint16_t *)(h + 0x34) = 0;
		*(uint16_t *)(h + 0x36) = 0;
		*(uint32_t *)(h + 0x48) = 0;
		*(uint16_t *)(h + 0x44) = 0;
		*(uint32_t *)(h + 0x3C) = *(uint32_t *)(E + 0x28);
		*(uint16_t *)(h + 0x38) = 0x140;
		*(uint16_t *)(h + 0x3A) = 0xD8;
		*(uint32_t *)(h + 0x40) = *(uint32_t *)(E + 0x7C) ^ mask;
		Cursor() = RenderGeometry(*(void **)(E + 0x64), h + 0x20, OT(), 4, Cursor());
		ReaderBuildBones(E + 0x60);
		FieldFree(0x4C);
	}

	static uint8_t *Skeleton()
	{
		uint8_t *com = *(uint8_t **)(Creature() + 0x64);
		return com ? *(uint8_t **)com : nullptr;
	}

	static void SetDiag(uint16_t d)
	{
		var<uint16_t>(game_addr(game::unk_24FC2E0)) = d;
		var<uint16_t>(game_addr(game::unk_24FC2D8)) = d;
		var<uint16_t>(game_addr(game::MAG_191_CreatureMat)) = d;
	}

	static void SpawnFlare();
	static void LinkGlow(int16_t e, int16_t y, int16_t s1c, int16_t s1e);
	static void LinkSteam(uint32_t task_fn, int16_t e, int16_t f12, int16_t f1C, int16_t f1E, int16_t f20, int16_t f22, bool set_e, bool set_f12, bool hist_b);
	static void SpawnPuffLine(bool second);
	static void KnockTargets();
	static void SpawnExplosions();

	static uint32_t __cdecl CreatureTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *E = Creature();
		int16_t *sp;
		int32_t z;
		if (n->c == 0)
		{
			ResetRotation(&CreatureMat());
			var<int32_t>(game_addr(game::unk_24FC2E8)) = 0;
			var<int32_t>(game_addr(game::unk_24FC2E4)) = 0;
			ModelInit(E, (uint8_t *)game_addr(game::unk_24FBD70), (uint8_t *)MB(), 0xF);
			sp = (int16_t *)(var<uint32_t>(game_addr(game::unk_24FBD7C)) + 2);
			var<int16_t *>(game_addr(game::unk_24FC0C0)) = sp;
			n->f1C = *sp;
		}
		else sp = var<int16_t *>(game_addr(game::unk_24FC0C0));
		int32_t k = n->c;
		if (k < 0x23)
		{
			*sp = (int16_t)(n->f1C / 4);
			CreatureZ() = 0x61A8;
			SetDiag(0x2000);
			if (P1()) goto draw;
			if (k == 0) { SetAnim(E, 0); goto draw; }
			if (k == 1) { LinkTask(game_addr(ORIG_ChimneyTask)); goto draw; }
			if (k == 0xE) SpawnFlare();
			goto draw;
		}
		k -= 0x23;
		if (k < 9)
		{
			if (P1()) goto after_advance;
			if (k == 1) LinkGlow(0x21, (int16_t)0xFB82, 0xF00, 0xF00);
			else if (k == 0)
			{
				LinkSteam(game_addr(ORIG_SteamTrailTask), 0x22, 0xF, 0x580, 0x380, 0x32, 0x78, true, true, false);
				LinkSteam(game_addr(ORIG_WheelSteamTask), 0x22, 0xA, 0x480, 0x440, 0x32, 0x32, true, true, true);
			}
			z = CreatureZ() - 0x190;
			goto store;
		}
		k -= 9;
		if (k < 0x19)
		{
			if (P1()) goto draw;
			CreatureZ() -= 0x258;
			AdvanceLoop(E);
			goto draw;
		}
		k -= 0x19;
		if (k < 0x64)
		{
			CreatureZ() = 0;
			SetDiag(0x1000);
			*sp = (int16_t)(n->f1C / 10);
			if (P1()) goto after_advance;
			if (k == 0)
			{
				SetAnim(E, 1);
				LinkTask(game_addr(ORIG_WheelTrailTask));
				goto compose;
			}
			AdvanceLoop(E);
			if (k == 1)
			{
				LinkGlow(0x63, (int16_t)0xFB82, 0x1C00, 0x1C00);
				LinkSteam(game_addr(ORIG_SteamStillTask), 0x63, 0, 0x198, 0x168, 0xA, 0xF, true, false, false);
				LinkSteam(game_addr(ORIG_WheelStillTask), 0x63, 0, 0x168, 0x148, 8, 8, true, false, true);
				goto draw;
			}
			goto after_advance;
		}
		k -= 0x64;
		if (k < 0x14)
		{
			*sp = (int16_t)(n->f1C / 4);
			SetDiag(0x2000);
			if (!P1())
			{
				if (k == 0)
				{
					CreatureZ() = 0x7148;
					SetAnim(E, 0);
					SaveTargetsAndMove();
					LinkSteam(game_addr(ORIG_SteamTrailTask), 0x14, 0, 0x680, 0x480, 0x32, 0x78, true, true, false);
					LinkSteam(game_addr(ORIG_WheelSteamTask), 0x14, 0, 0x480, 0x440, 0x32, 0x32, true, true, true);
				}
				else if (k == 1)
				{
					ShowTargets();
					SpawnPuffLine(false);
					LinkGlow(0x12, (int16_t)0xFB82, 0xF00, 0xF00);
				}
				else if (k == 0x13) HideTargets();
				CreatureZ() -= 0x3E8;
				AdvanceLoop(E);
			}
			if (k == 0) goto compose;
			DrawModel(E, &Frame());
			{
				int32_t v[3] = { 0x800, 0x800, 0x800 };
				Scale3DMatrix(&Camera(), v);
			}
			goto compose;
		}
		k -= 0x14;
		if (k < 0x16)
		{
			*sp = (int16_t)(n->f1C / 4);
			SetDiag(0x4000);
			if (P1()) goto after_advance;
			if (k == 0)
			{
				CreatureZ() = 0x6D60;
				AdvanceLoop(E);
				SpawnPuffLine(true);
				LinkSteam(game_addr(ORIG_SteamBurstTask), 0, 0, 0x1200, 0xB80, 0x96, 0xFA, false, false, false);
				LinkSteam(game_addr(ORIG_WheelBurstTask), 0, 0, 0xC00, 0x580, 0x96, 0x96, false, false, true);
			}
			else if (k == 1) LinkGlow(0x34, (int16_t)0xFB6E, 0x1300, 0x1000);
			else if (k == 4) LinkTask(game_addr(ORIG_DebrisSpawnTask));
			z = CreatureZ() - 0x320;
			goto store;
		}
		k -= 0x16;
		if (k < 0x1F)
		{
			*sp = (int16_t)(n->f1C / 4);
			SetDiag(0x4000);
			if (P1()) goto after_advance;
			if (k == 0)
			{
				CreatureZ() = 0x7D00;
				ShowTargets();
				RestoreTargets();
				StateFlags() |= 2;
			}
			else if (k == 0x14) LinkTask(game_addr(ORIG_PuffWallTask));
			else if (k > 0x14) CreatureZ() -= 0x708;
			goto advance;
		}
		k -= 0x1F;
		if (k < 0x28)
		{
			*sp = (int16_t)(n->f1C / 4);
			SetDiag(0x4000);
			if (P1()) goto after_advance;
			if (k == 0)
			{
				CreatureZ() = 0x6D60;
				LinkGlow(0xA, (int16_t)0xFB6E, 0x1300, 0x1000);
			}
			else if (k == 3)
			{
				KnockTargets();
				CameraShake(0, 1, 4, 0x80);
			}
			else if (k == 2) SpawnExplosions();
			z = CreatureZ() - 0x12C0;
			goto store;
		}
		k -= 0x28;
		if (k < 4) goto draw;
		goto compose;
	store:
		CreatureZ() = z;
	advance:
		AdvanceLoop(E);
	after_advance:
		if (k == 0) goto compose;
	draw:
		DrawModel(E, &Frame());
	compose:
		ComposeAffineTransform(&CreatureMat(), (const Mat4x3 *)(*(uint8_t **)*(uint8_t **)(E + 0x64) + 0x50), &BoneWorld());
		if (P1()) return 0;
		n->c++;
		if (n->c < 0x117) return 0;
		StateFlags() |= 1;
		return TASK_END;
	}

	static void WheelTrailPair(uint8_t *h, int32_t *mac, int32_t *nv, const Rec *a, const Rec *b, int16_t fa, int16_t fb)
	{
		OffsetSprite(h, mac, nv, a->pos, a->size, fa, 4);
		OffsetSprite(h, mac, nv, b->pos, b->size, fb, 4);
	}

	static uint32_t __cdecl WheelTrailTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x50);
		*(uint32_t *)h = game_addr(game::unk_E33E40);
		*(uint16_t *)(h + 0x24) = 8;
		*(int16_t *)s = 0;
		*(int16_t *)(s + 2) = 0;
		*(int16_t *)(s + 4) = 0;
		ComposeZYXRotationMatrix((const int16_t *)s, (Mat4x3 *)(s + 0x10));
		*(int32_t *)(s + 0x24) = 0;
		*(int32_t *)(s + 0x28) = 0;
		*(int32_t *)(s + 0x2C) = 0;
		ComposeAffineTransform(&Frame(), (Mat4x3 *)(s + 0x10), (Mat4x3 *)(s + 0x10));
		GteSetLightMatrix(s + 0x10);
		GteSetBackColorFromTrans(s + 0x10);
		for (int i = 0; i < 170; i++)
		{
			Rec *a = R(game_addr(POOL_A), i), *b = R(game_addr(POOL_B), i);
			if (!(*(uint8_t *)a & 1)) continue;
			WheelTrailPair(h, (int32_t *)(s + 0x40), (int32_t *)(s + 0x30), a, b, a->age, b->age);
			if (P1()) continue;
			a->age++;
			b->age++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				a->age = 0;
				b->age = 0;
			}
		}
		FieldFree(0x50);
		FieldFree(0xB4);
		if (P1()) return 0;
		s = (uint8_t *)FieldAlloc(0x50);
		if (n->c == 0)
		{
			uint8_t *E = Creature();
			uint8_t *bone = *(uint8_t **)*(uint8_t **)(E + 0x64) + 0x40;
			SetAnim(E, 1);
			for (int k = 0; k < 0x77; k++)
			{
				int i = FreeRec(game_addr(POOL_A), 170);
				if (i < 0) break;
				Mat4x3 *m = (Mat4x3 *)(s + 0x10);
				int16_t *v = (int16_t *)(s + 8);
				ComposeAffineTransform(&CreatureMat(), (const Mat4x3 *)(bone + 0x10), m);
				v[1] = 0x2BC;
				v[0] = (int16_t)0xFB50;
				v[2] = 0;
				MatVec(m, v, v);
				v[1] = (int16_t)(v[1] + *(int16_t *)(s + 0x28));
				v[2] = (int16_t)(v[2] + *(int16_t *)(s + 0x2C));
				v[0] = (int16_t)(v[0] + *(int16_t *)(s + 0x24));
				Rec *a = R(game_addr(POOL_A), i);
				a->mask = 1;
				a->age = 0;
				a->size = 0x180;
				ff8fx::copy<(8)>(a->pos, v);
				v[0] = 0x4B0;
				v[1] = 0x2BC;
				v[2] = 0;
				MatVec(m, v, v);
				v[0] = (int16_t)(v[0] + *(int16_t *)(s + 0x24));
				v[1] = (int16_t)(v[1] + *(int16_t *)(s + 0x28));
				v[2] = (int16_t)(v[2] + *(int16_t *)(s + 0x2C));
				Rec *b = R(game_addr(POOL_B), i);
				b->mask = 1;
				b->age = 0;
				b->size = 0x180;
				ff8fx::copy<(8)>(b->pos, v);
				AdvanceLoop(E);
			}
			SetAnim(E, 1);
		}
		FieldFree(0x50);
		n->c++;
		if (n->c < 0x64) return 0;
		for (int i = 0; i < 170; i++)
		{
			Rec *a = R(game_addr(POOL_A), i), *b = R(game_addr(POOL_B), i);
			if (*(uint8_t *)a & 1) a->mask = 0;
			if (*(uint8_t *)b & 1) b->mask = 0;
		}
		return TASK_END;
	}

	static void SpawnPuffLine(bool second)
	{
		Node24 *n = AddTask(game_addr(ORIG_PuffLineTask));
		n->c = 0;
		if (second)
		{
			n->e = 0x2A;
			n->f10 = 0;
			n->f12 = (int16_t)0xFE70;
			n->f14 = 0x7530;
			n->f16 = 0x14;
			n->f1C = 0x7D0;
			n->f1E = 0x320;
			n->f20 = 0x1400;
			n->f22 = (int16_t)0xF448;
		}
		else
		{
			n->e = 0x13;
			n->f10 = 0;
			n->f12 = (int16_t)0xFF38;
			n->f14 = 0x7530;
			n->f16 = 0x1E;
			n->f1C = 0x384;
			n->f1E = 0;
			n->f20 = 0x800;
			n->f22 = (int16_t)0xF830;
		}
	}

	static uint32_t __cdecl PuffLineTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		*(uint32_t *)h = game_addr(game::unk_E33E40);
		*(uint16_t *)(h + 0x24) = 8;
		LitFrame(s, nullptr, &Frame(), 0, 0, 0);
		for (int i = 0; i < 170; i++)
		{
			Rec *r = R(game_addr(POOL_A), i);
			if (!(*(uint8_t *)r & 1)) continue;
			LitSprite(h, r->pos, r->size, r->age, false);
			if (P1()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0) r->age = 0;
			else r->pos[2] = (int16_t)(r->pos[2] + n->f1E);
		}
		FieldFree(0x48);
		FieldFree(0xB4);
		if (P1()) return 0;
		FieldAlloc(0x48);
		if (n->c == 0)
		{
			for (int k = 0; k < (int32_t)n->f16; k++)
			{
				int i = FreeRec(game_addr(POOL_A), 170);
				if (i < 0) break;
				int16_t sz = n->f20, dx = n->f1C;
				Rec *a = R(game_addr(POOL_A), i);
				a->mask = 1;
				a->age = 0;
				a->size = sz;
				ff8fx::copy<(8)>(a->pos, &n->f10);
				a->pos[0] = (int16_t)(a->pos[0] - dx);
				Rec *b = R(game_addr(POOL_A), i + 1);
				b->mask = 1;
				b->age = 0;
				b->size = sz;
				ff8fx::copy<(8)>(b->pos, &n->f10);
				b->pos[0] = (int16_t)(b->pos[0] + dx);
				n->f14 = (int16_t)(n->f14 + n->f22);
			}
		}
		FieldFree(0x48);
		n->c++;
		if (n->c < n->e) return 0;
		ClearMask(game_addr(POOL_A), 170, 1, false);
		return TASK_END;
	}

	static uint32_t WallGrey(int32_t c)
	{
		uint32_t v = (uint32_t)shl32(0x31 - c, 4);
		return (((v << 8) | v) << 8) | v;
	}

	static void PuffWallSetup(uint8_t *h, uint8_t *s, const Mat4x3 *frame, int16_t c, bool grey_override, uint32_t grey)
	{
		*(uint32_t *)h = game_addr(game::unk_E33E40);
		*(uint16_t *)(h + 0x24) = 8;
		if (c < 0xB) *(uint16_t *)(h + 0x24) = 0x208;
		*(int16_t *)s = 0;
		*(int16_t *)(s + 2) = 0;
		*(int16_t *)(s + 4) = 0;
		ComposeZYXRotationMatrix((const int16_t *)s, (Mat4x3 *)(s + 8));
		*(int32_t *)(s + 0x1C) = 0;
		*(int32_t *)(s + 0x20) = 0;
		*(int32_t *)(s + 0x24) = 0;
		ComposeAffineTransform(frame, (Mat4x3 *)(s + 8), (Mat4x3 *)(s + 8));
		if (c >= 0x29)
		{
			h[0x24] |= 4;
			*(uint32_t *)(h + 0x1C) = grey_override ? grey : WallGrey(c);
		}
		GteSetLightMatrix(s + 8);
		GteSetBackColorFromTrans(s + 8);
	}

	static uint32_t __cdecl PuffWallTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		PuffWallSetup(h, s, &Frame(), n->c, false, 0);
		for (int i = 0; i < 170; i++)
		{
			Rec *r = R(game_addr(POOL_A), i);
			if (!(*(uint8_t *)r & 1)) continue;
			LitSprite(h, r->pos, r->size, r->age, false);
			if (P1()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0) r->age = 0;
		}
		FieldFree(0x48);
		FieldFree(0xB4);
		if (P1()) return 0;
		FieldAlloc(0x48);
		if (n->c == 0)
		{
			n->f10 = 0;
			n->f12 = (int16_t)0xFE70;
			n->f14 = 0x7530;
			for (int k = 0; k < 0x14; k++)
			{
				int i = FreeRec(game_addr(POOL_A), 170);
				if (i < 0) break;
				Rec *a = R(game_addr(POOL_A), i);
				a->mask = 1;
				a->age = 0;
				a->size = 0x1400;
				ff8fx::copy<(8)>(a->pos, &n->f10);
				a->pos[0] = (int16_t)(a->pos[0] - 2000);
				Rec *b = R(game_addr(POOL_A), i + 1);
				b->mask = 1;
				b->age = 0;
				b->size = 0x1400;
				ff8fx::copy<(4)>(b->pos, &n->f10);
				ff8fx::copy<(4)>(&b->pos[2], &n->f14);
				n->f14 = (int16_t)(n->f14 - 3000);
				b->pos[0] = (int16_t)(b->pos[0] + 2000);
			}
		}
		FieldFree(0x48);
		n->c++;
		if (n->c < 0x31) return 0;
		ClearMask(game_addr(POOL_A), 170, 1, false);
		return TASK_END;
	}

	static void SpawnFlare()
	{
		Node24 *n = AddTask(game_addr(ORIG_FlareTask));
		int16_t *v = &n->f10;
		n->c = 0;
		v[0] = 0;
		n->f12 = (int16_t)0xFB82;
		n->f14 = (int16_t)0xD9EA;
		MatVec(&BoneWorld(), v, v);
		v[0] = (int16_t)(v[0] + W16(game_addr(game::unk_24FBE6C)));
		n->f12 = (int16_t)(n->f12 + W16(game_addr(game::unk_24FBE70)));
		n->f14 = (int16_t)(n->f14 + W16(game_addr(game::unk_24FBE74)));
		n->f1C = 0xB00;
		var<int32_t>(game_addr(game::g191_GlowPhase)) = CrtRand() % 0x1000;
		var<int32_t>(game_addr(game::g191_GlowSpeed)) = 0x110;
	}

	static int32_t FlareScale(int32_t c, int32_t s)
	{
		if (c >= 0x13) c = 0x13;
		return mul32(ComputeSin(1760 * c / 19), s) >> 12;
	}

	static uint32_t __cdecl FlareTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int32_t c = n->c;
		int32_t v;
		{
			int16_t ang[3] = { 0, 0, 0 };
			Mat4x3 m;
			ComposeZYXRotationMatrix(ang, &m);
			m.t[1] = n->f12;
			m.t[0] = n->f10;
			m.t[2] = n->f14;
			v = FlareScale(c, n->f1C);
			int32_t sc[3] = { v, v, n->f1C };
			Scale3DMatrix(&m, sc);
			ComposeAffineTransform(&Frame(), &m, &m);
			GteSetRotMatrix(&m);
			GteSetTransVector(&m);
			PrimModel(game_addr(game::unk_E3A744), 2, 0x33);
		}
		if (P1()) return 0;
		n->c++;
		return n->c >= 0x16 ? TASK_END : 0;
	}

	static void LinkGlow(int16_t e, int16_t y, int16_t s1c, int16_t s1e)
	{
		Node24 *n = AddTask(game_addr(ORIG_GlowTask));
		n->c = 0;
		n->f10 = 0;
		n->e = e;
		n->f12 = y;
		n->f14 = (int16_t)0xDA1C;
		n->f1C = s1c;
		n->f1E = s1e;
	}

	static int32_t GlowFade()
	{
		int32_t f;
		if (var<int32_t>(game_addr(game::g191_GlowSpeed)) % 13 == 1) f = 0x100;
		else
		{
			int32_t x = (ComputeSin(var<int32_t>(game_addr(game::g191_GlowPhase))) + 0x1000) / 2;
			f = shl32(x * 9, 8) >> 12;
		}
		return f;
	}

	static int32_t GlowDraw(const Mat4x3 *frame, const Mat4x3 *bone, const Node24 *n, int32_t fade_override)
	{
		int16_t ang[3] = { 0, 0, 0 };
		Mat4x3 m1, m2;
		ComposeZYXRotationMatrix(ang, &m1);
		m1.t[0] = n->f10;
		m1.t[1] = n->f12;
		m1.t[2] = n->f14;
		ComposeAffineTransform(bone, &m1, &m1);
		m2 = m1;
		int32_t v[3] = { n->f1C, n->f1C, n->f1C };
		Scale3DMatrix(&m2, v);
		ComposeAffineTransform(frame, &m2, &m2);
		GteSetRotMatrix(&m2);
		GteSetTransVector(&m2);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = game_addr(game::unk_E3A744);
		*(int32_t *)(h + 0xC) = fade_override >= 0 ? fade_override : GlowFade();
		if (*(int32_t *)(h + 0xC) < 0) *(int32_t *)(h + 0xC) = 0;
		int32_t fade = *(int32_t *)(h + 0xC);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0xF3;
		Cursor() = RenderPrimModel(h, OT(), 3, Cursor());
		int32_t v2[3] = { n->f1E, n->f1E, n->f1E };
		Scale3DMatrix(&m1, v2);
		ComposeAffineTransform(frame, &m1, &m1);
		GteSetRotMatrix(&m1);
		GteSetTransVector(&m1);
		*(uint32_t *)h = game_addr(game::unk_E3AD34);
		*(uint32_t *)(h + 0xC) = 0;
		Cursor() = RenderPrimModel(h, OT(), 3, Cursor());
		FieldFree(0x58);
		return fade;
	}

	static uint32_t __cdecl GlowTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		GlowDraw(&Frame(), &BoneWorld(), n, -1);
		if (P1()) return 0;
		var<int32_t>(game_addr(game::g191_GlowPhase)) += var<int32_t>(game_addr(game::g191_GlowSpeed));
		int32_t r = CrtRand() % 0x40;
		var<int32_t>(game_addr(game::g191_GlowSpeed)) += r;
		n->c++;
		return n->c >= n->e ? TASK_END : 0;
	}

	static void ChimneyFrame(uint8_t *s, const Mat4x3 *bone, const Mat4x3 *frame)
	{
		LitFrame(s, bone, frame, -0xB4, -0x456, -0x2454);
	}

	static uint32_t __cdecl ChimneyTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		*(uint32_t *)h = game_addr(game::unk_E342A4);
		*(uint16_t *)(h + 0x24) = 8;
		int32_t count = 0;
		ChimneyFrame(s, &BoneWorld(), &Frame());
		for (int i = 0; i < 170; i++)
		{
			Rec *r = R(game_addr(POOL_A), i);
			if (!(*(uint8_t *)r & 4)) continue;
			LitSprite(h, r->pos, r->size, r->age, false);
			if (P1()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				r->mask = 0;
				continue;
			}
			int16_t sz = r->size;
			r->size = (int16_t)(sz + (int16_t)(sz >> 3));
			r->pos[1] = (int16_t)(r->pos[1] + r->vel[1]);
			int16_t vy = r->vel[1];
			r->vel[1] = (int16_t)(vy - vy / 11);
			count++;
		}
		FieldFree(0x48);
		FieldFree(0xB4);
		if (P1()) return 0;
		FieldAlloc(0x48);
		if (n->c < 0xC)
		{
			for (int k = 0; k < 7; k++)
			{
				int i = FreeRec(game_addr(POOL_A), 170);
				if (i < 0) break;
				Rec *r = R(game_addr(POOL_A), i);
				r->mask = 4;
				r->age = 0;
				r->size = (int16_t)(CrtRand() % 0x90 + 0x90);
				r->pos[0] = (int16_t)(CrtRand() % 0x1E - 0xF);
				r->pos[1] = (int16_t)-(CrtRand() % 0x28);
				r->pos[2] = (int16_t)(CrtRand() % 0x1E - 0xF);
				r->vel[1] = (int16_t)(-0x73 - CrtRand() % 0xF0);
			}
		}
		FieldFree(0x48);
		n->c++;
		if (n->c < 0xC) return 0;
		if (count) return 0;
		ClearMask(game_addr(POOL_A), 170, 4, false);
		return TASK_END;
	}

	static void LinkSteam(uint32_t task_fn, int16_t e, int16_t f12, int16_t f1C, int16_t f1E, int16_t f20, int16_t f22, bool set_e, bool set_f12, bool hist_b)
	{
		Node24 *n = AddTask(task_fn);
		n->c = 0;
		if (set_e) n->e = e;
		if (set_f12) n->f12 = f12;
		n->f1C = f1C;
		n->f1E = f1E;
		n->f20 = f20;
		n->f22 = f22;
		for (int k = 0; k < 2; k++) (hist_b ? HistB(k) : HistA(k))->valid = 0;
	}

	static void SteamHistory(uint8_t *s, int32_t c, bool clamp)
	{
		int16_t *v = (int16_t *)(s + 8);
		v[1] = (int16_t)0xF90C;
		v[0] = 0;
		v[2] = (int16_t)0xDBAC;
		MatVec(&BoneWorld(), v, v);
		v[0] = (int16_t)(v[0] + W16(game_addr(game::unk_24FBE6C)));
		v[1] = (int16_t)(v[1] + W16(game_addr(game::unk_24FBE70)));
		v[2] = (int16_t)(v[2] + W16(game_addr(game::unk_24FBE74)));
		if (clamp && v[2] < (int16_t)0x9E58) v[2] = (int16_t)0x9E58;
		Hist *h = HistA(c % 2);
		h->valid = 1;
		h->a0 = *(uint32_t *)(s + 8);
		h->a1 = *(uint32_t *)(s + 0xC);
	}

	static int32_t SteamCollect(int32_t c, bool two)
	{
		int32_t k = c % 2, n = 0;
		for (; n < 2; n++)
		{
			Hist *h = two ? HistB(k) : HistA(k);
			if (!h->valid) break;
			var<uint32_t>(game_addr(game::unk_24FBDC8) + 8 * n) = h->a0;
			var<uint32_t>(game_addr(game::unk_24FBDCC) + 8 * n) = h->a1;
			if (two)
			{
				var<uint32_t>(game_addr(game::unk_24FBE10) + 8 * n) = h->b0;
				var<uint32_t>(game_addr(game::unk_24FBE14) + 8 * n) = h->b1;
			}
			k--;
			if (k < 0) k = 1;
		}
		return n;
	}

	static void SteamCurve(int32_t n, uint32_t pts, uint32_t dst, int32_t count, int32_t div, uint8_t *buf, bool coeffs)
	{
		if (coeffs) CurveCoeffs(n, (const void *)pts, buf);
		for (int32_t i = 0, t = 0; i < count; i++, t += 0x1000) CurveEval(n, buf, (void *)(dst + 8 * i), t / div);
	}

	static void SteamSetup(uint8_t *h, uint8_t *s, uint32_t seq, bool tint, int16_t w24)
	{
		*(uint32_t *)h = seq;
		if (tint) *(uint32_t *)(h + 0x1C) = 0x586880;
		*(uint16_t *)(h + 0x24) = (uint16_t)w24;
		ff8fx::copy<(sizeof(Mat4x3))>(s + 0x10, &Frame());
		GteSetLightMatrix(s + 0x10);
		GteSetBackColorFromTrans(s + 0x10);
	}

	static Rec *SteamPuff(uint32_t cell, int i, const Node24 *n, uint32_t pt, bool y_signed)
	{
		Rec *r = R(cell, i);
		r->mask = 1;
		r->age = 0;
		r->size = (int16_t)(CrtRand() % n->f1E + n->f1C);
		ff8fx::copy<(8)>(r->pos, (const void *)pt);
		int32_t f20 = n->f20;
		r->pos[0] = (int16_t)(r->pos[0] + (int16_t)(CrtRand() % (2 * f20) - f20));
		if (y_signed)
		{
			int32_t f22 = n->f22;
			r->pos[1] = (int16_t)(r->pos[1] + (int16_t)(CrtRand() % (2 * f22) - f22));
		}
		else r->pos[1] = (int16_t)(r->pos[1] - (int16_t)(CrtRand() % n->f22));
		r->pos[2] = (int16_t)(r->pos[2] + (int16_t)(CrtRand() % (2 * f20) - f20));
		return r;
	}

	static uint32_t __cdecl SteamTrailTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x50);
		SteamHistory(s, n->c, false);
		int32_t np = SteamCollect(n->c, false);
		SteamSetup(h, s, game_addr(game::unk_E342A4), true, 0xC);
		for (int i = 0; i < 150; i++)
		{
			Rec *r = R(game_addr(POOL_C), i);
			if (!(*(uint8_t *)r & 1)) continue;
			OffsetSprite(h, (int32_t *)(s + 0x40), (int32_t *)(s + 0x30), r->pos, r->size, r->age, 4);
			if (P1()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				r->mask = 0;
				continue;
			}
			int16_t sz = r->size;
			r->size = (int16_t)(sz + (int16_t)(sz >> 5));
			r->pos[1] = (int16_t)(r->pos[1] + r->vel[1]);
			int16_t vy = r->vel[1];
			r->vel[1] = (int16_t)(vy - (int16_t)(vy >> 1));
		}
		FieldFree(0x50);
		FieldFree(0xB4);
		if (P1()) return 0;
		if (n->c < n->e && np > 1)
		{
			uint8_t *buf = (uint8_t *)FieldAlloc(0x190);
			SteamCurve(np, game_addr(game::unk_24FBDC8), game_addr(game::MAG_191_Pt), 8, 7, buf, true);
			FieldFree(0x190);
			int32_t num = n->c >= n->f12 ? 7 : 1;
			uint32_t pt = game_addr(game::MAG_191_Pt);
			for (int32_t j = 0; j < num; j++, pt += 8)
			{
				int i = FreeRec(game_addr(POOL_C), 150);
				if (i < 0) break;
				Rec *r = SteamPuff(game_addr(POOL_C), i, n, pt, false);
				r->vel[1] = (int16_t)(-0x41 - CrtRand() % 0x82);
			}
		}
		n->c++;
		if (n->c < n->e) return 0;
		ClearMask(game_addr(POOL_C), 150, 1, false);
		return TASK_END;
	}

	static uint32_t __cdecl SteamStillTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *s = (uint8_t *)FieldAlloc(0x50);
		SteamHistory(s, n->c, false);
		FieldFree(0x50);
		int32_t np = SteamCollect(n->c, false);
		if (!P1() && n->c < n->e && np > 1)
		{
			uint8_t *buf = (uint8_t *)FieldAlloc(0x190);
			SteamCurve(np, game_addr(game::unk_24FBDC8), game_addr(game::MAG_191_Pt), 5, 4, buf, true);
			FieldFree(0x190);
			for (uint32_t pt = game_addr(game::MAG_191_Pt); pt < game_addr(game::unk_24FC120); pt += 8)
			{
				int i = FreeRec(game_addr(POOL_C), 150);
				if (i < 0) break;
				SteamPuff(game_addr(POOL_C), i, n, pt, false);
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		s = (uint8_t *)FieldAlloc(0x50);
		SteamSetup(h, s, game_addr(game::unk_E342A4), true, 0xC);
		for (int i = 0; i < 150; i++)
		{
			Rec *r = R(game_addr(POOL_C), i);
			if (!(*(uint8_t *)r & 1)) continue;
			OffsetSprite(h, (int32_t *)(s + 0x40), (int32_t *)(s + 0x30), r->pos, r->size, r->age, 2);
			if (P1()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0) r->mask = 0;
		}
		FieldFree(0x50);
		FieldFree(0xB4);
		if (P1()) return 0;
		n->c++;
		if (n->c < n->e) return 0;
		ClearMask(game_addr(POOL_C), 150, 1, false);
		return TASK_END;
	}

	static uint32_t __cdecl SteamBurstTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *s = (uint8_t *)FieldAlloc(0x50);
		SteamHistory(s, n->c, true);
		FieldFree(0x50);
		int32_t np = SteamCollect(n->c, false);
		if (!P1() && n->c < 0x3B && np > 1)
		{
			uint8_t *buf = (uint8_t *)FieldAlloc(0x190);
			SteamCurve(np, game_addr(game::unk_24FBDC8), game_addr(game::MAG_191_Pt), 6, 5, buf, true);
			FieldFree(0x190);
			int16_t c = n->c;
			int32_t num = c < 0x16 ? 5 : (c < 0x2A ? 2 : 5);
			uint32_t pt = game_addr(game::MAG_191_Pt);
			for (int32_t j = 0; j < num; j++, pt += 8)
			{
				int i = FreeRec(game_addr(POOL_C), 150);
				if (i < 0) break;
				Rec *r = SteamPuff(game_addr(POOL_C), i, n, pt, false);
				r->vel[1] = (int16_t)(-0xA0 - CrtRand() % 0x12C);
				r->vel[2] = n->c < 0x2A ? 0x1F4 : 0;
				if (num == 2) r->vel[1] = (int16_t)(r->vel[1] << 1);
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		s = (uint8_t *)FieldAlloc(0x50);
		SteamSetup(h, s, game_addr(game::unk_E342A4), true, 0xC);
		int32_t count = 0;
		for (int i = 0; i < 150; i++)
		{
			Rec *r = R(game_addr(POOL_C), i);
			if (!(*(uint8_t *)r & 1)) continue;
			OffsetSprite(h, (int32_t *)(s + 0x40), (int32_t *)(s + 0x30), r->pos, r->size, r->age, 4);
			if (P1()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				r->mask = 0;
				continue;
			}
			int16_t sz = r->size;
			r->size = (int16_t)(sz + (int16_t)(sz >> 5));
			r->pos[1] = (int16_t)(r->pos[1] + r->vel[1]);
			r->pos[2] = (int16_t)(r->pos[2] + r->vel[2]);
			int16_t vy = r->vel[1], vz = r->vel[2];
			r->vel[1] = (int16_t)(vy - (int16_t)(vy >> 1));
			r->vel[2] = (int16_t)(vz + (int16_t)(vz >> 2));
			count++;
		}
		FieldFree(0x50);
		FieldFree(0xB4);
		if (P1()) return 0;
		n->c++;
		if (n->c < 0x3B) return 0;
		if (count) return 0;
		ClearMask(game_addr(POOL_C), 150, 1, false);
		return TASK_END;
	}

	static void WheelHistory(uint8_t *s, int32_t c, bool clamp)
	{
		int16_t *w = (int16_t *)s, *v = (int16_t *)(s + 8);
		Hist *h = HistB(c % 2);
		h->valid = 1;
		v[0] = 0x4B0;
		v[1] = 0x212;
		v[2] = (int16_t)0xDECC;
		MatVec(&BoneWorld(), v, w);
		w[0] = (int16_t)(w[0] + W16(game_addr(game::unk_24FBE6C)));
		w[1] = (int16_t)(w[1] + W16(game_addr(game::unk_24FBE70)));
		w[2] = (int16_t)(w[2] + W16(game_addr(game::unk_24FBE74)));
		if (clamp && w[2] < (int16_t)0x9E58) w[2] = (int16_t)0x9E58;
		h->a0 = *(uint32_t *)s;
		h->a1 = *(uint32_t *)(s + 4);
		v[0] = (int16_t)-v[0];
		MatVec(&BoneWorld(), v, w);
		w[0] = (int16_t)(w[0] + W16(game_addr(game::unk_24FBE6C)));
		w[1] = (int16_t)(w[1] + W16(game_addr(game::unk_24FBE70)));
		w[2] = (int16_t)(w[2] + W16(game_addr(game::unk_24FBE74)));
		if (clamp && w[2] < (int16_t)0x9E58) w[2] = (int16_t)0x9E58;
		h->b0 = *(uint32_t *)s;
		h->b1 = *(uint32_t *)(s + 4);
	}

	static void WheelDrawLoop(const Node24 *n, uint8_t *h, uint8_t *s, int shift, bool move, bool burst, int32_t *count)
	{
		for (int i = 0; i < 120; i++)
		{
			Rec *r = R(game_addr(POOL_D), i);
			if (!(*(uint8_t *)r & 1)) continue;
			OffsetSprite(h, (int32_t *)(s + 0x40), (int32_t *)(s + 0x30), r->pos, r->size, (int16_t)(r->age << 1), shift);
			if (P1()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				r->mask = 0;
				continue;
			}
			if (!move) continue;
			r->pos[0] = (int16_t)(r->pos[0] + r->vel[0]);
			r->pos[1] = (int16_t)(r->pos[1] + r->vel[1]);
			if (burst) r->pos[2] = (int16_t)(r->pos[2] + r->vel[2]);
			int16_t vx = r->vel[0], vy = r->vel[1];
			r->vel[0] = (int16_t)(vx - (int16_t)(vx >> 1));
			r->vel[1] = (int16_t)(vy - (int16_t)(vy >> 1));
			if (burst)
			{
				int16_t vz = r->vel[2];
				r->vel[2] = (int16_t)(vz + (int16_t)(vz >> 2));
				(*count)++;
			}
		}
	}

	static uint32_t __cdecl WheelSteamTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x50);
		WheelHistory(s, n->c, false);
		int32_t np = SteamCollect(n->c, true);
		SteamSetup(h, s, game_addr(game::unk_E342A4), false, 8);
		WheelDrawLoop(n, h, s, 3, true, false, nullptr);
		FieldFree(0x50);
		FieldFree(0xB4);
		if (P1()) return 0;
		if (n->c < n->e && np > 1)
		{
			uint8_t *buf = (uint8_t *)FieldAlloc(0x190);
			SteamCurve(np, game_addr(game::unk_24FBDC8), game_addr(game::MAG_191_Pt), 8, 7, buf, true);
			SteamCurve(np, game_addr(game::unk_24FBE10), game_addr(game::unk_24FC1C0), 8, 7, buf, true);
			FieldFree(0x190);
			int32_t num = n->c >= n->f12 ? 7 : 2;
			for (int32_t j = 0; j < num; j++)
			{
				int i = FreeRec(game_addr(POOL_D), 120);
				if (i < 0) break;
				Rec *a = SteamPuff(game_addr(POOL_D), i, n, game_addr(game::MAG_191_Pt) + 8 * j, true);
				a->vel[0] = (int16_t)(CrtRand() % 0x37 + 0x46);
				a->vel[1] = (int16_t)(CrtRand() % 0x37 + 0x46);
				Rec *b = SteamPuff(game_addr(POOL_D), i + 1, n, game_addr(game::unk_24FC1C0) + 8 * j, true);
				b->vel[0] = (int16_t)(-0x46 - CrtRand() % 0x37);
				b->vel[1] = (int16_t)(CrtRand() % 0x37 + 0x46);
			}
		}
		n->c++;
		if (n->c < n->e) return 0;
		ClearMask(game_addr(POOL_D), 120, 1, false);
		return TASK_END;
	}

	static uint32_t __cdecl WheelStillTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *s = (uint8_t *)FieldAlloc(0x50);
		WheelHistory(s, n->c, false);
		FieldFree(0x50);
		int32_t np = SteamCollect(n->c, true);
		if (n->c < n->e && np > 1)
		{
			uint8_t *buf = (uint8_t *)FieldAlloc(0x190);
			SteamCurve(np, game_addr(game::unk_24FBDC8), game_addr(game::MAG_191_Pt), 5, 4, buf, true);
			SteamCurve(np, game_addr(game::unk_24FBE10), game_addr(game::unk_24FC1C0), 5, 4, buf, true);
			FieldFree(0x190);
			for (uint32_t o = 0; o < 0x20; o += 8)
			{
				int i = FreeRec(game_addr(POOL_D), 120);
				if (i < 0) break;
				SteamPuff(game_addr(POOL_D), i, n, game_addr(game::MAG_191_Pt) + o, true);
				SteamPuff(game_addr(POOL_D), i + 1, n, game_addr(game::unk_24FC1C0) + o, true);
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		s = (uint8_t *)FieldAlloc(0x50);
		SteamSetup(h, s, game_addr(game::unk_E342A4), false, 8);
		WheelDrawLoop(n, h, s, 2, false, false, nullptr);
		FieldFree(0x50);
		FieldFree(0xB4);
		if (P1()) return 0;
		n->c++;
		if (n->c < n->e) return 0;
		ClearMask(game_addr(POOL_D), 120, 1, false);
		return TASK_END;
	}

	static uint32_t __cdecl WheelBurstTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *s = (uint8_t *)FieldAlloc(0x50);
		WheelHistory(s, n->c, true);
		FieldFree(0x50);
		int32_t np = SteamCollect(n->c, true);
		if (!P1() && n->c < 0x3B && np > 1)
		{
			uint8_t *buf = (uint8_t *)FieldAlloc(0x190);
			SteamCurve(np, game_addr(game::unk_24FBDC8), game_addr(game::MAG_191_Pt), 6, 5, buf, true);
			SteamCurve(np, game_addr(game::unk_24FBE10), game_addr(game::unk_24FC1C0), 6, 5, buf, true);
			FieldFree(0x190);
			int16_t c = n->c;
			int32_t num = c < 0x16 ? 5 : (c < 0x2A ? 2 : 5);
			for (int32_t j = 0; j < num; j++)
			{
				int i = FreeRec(game_addr(POOL_D), 120);
				if (i < 0) break;
				Rec *a = SteamPuff(game_addr(POOL_D), i, n, game_addr(game::MAG_191_Pt) + 8 * j, true);
				a->vel[0] = (int16_t)(CrtRand() % 0xEB + 0xA0);
				a->vel[1] = (int16_t)(CrtRand() % 0xEB + 0xA0);
				a->vel[2] = n->c < 0x2A ? 0x1F4 : 0;
				Rec *b = SteamPuff(game_addr(POOL_D), i + 1, n, game_addr(game::unk_24FC1C0) + 8 * j, true);
				b->vel[0] = (int16_t)(-0xA0 - CrtRand() % 0xEB);
				b->vel[1] = (int16_t)(CrtRand() % 0xEB + 0xA0);
				b->vel[2] = n->c < 0x2A ? 0x1F4 : 0;
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		s = (uint8_t *)FieldAlloc(0x50);
		SteamSetup(h, s, game_addr(game::unk_E342A4), false, 8);
		int32_t count = 0;
		WheelDrawLoop(n, h, s, 2, true, true, &count);
		FieldFree(0x50);
		FieldFree(0xB4);
		if (P1()) return 0;
		n->c++;
		if (n->c < 0x3B) return 0;
		if (count) return 0;
		ClearMask(game_addr(POOL_D), 120, 1, false);
		return TASK_END;
	}

	static uint32_t __cdecl DebrisSpawnTask(TaskNode *tn)
	{
		if (P1()) return 0;
		Node24 *n = (Node24 *)tn;
		int16_t c = n->c;
		if (c >= 0 && c < 0xC && c % 3 == 1)
		{
			int32_t k = CrtRand() % 2 + 1;
			for (int32_t i = 0; i < k; i++)
			{
				Node24 *m = AddTask(game_addr(ORIG_DebrisTask));
				m->c = 0;
				m->e = (int16_t)(CrtRand() % 4 + i);
				m->f10 = (int16_t)(CrtRand() % 0x190 + 0x3E8);
				m->f12 = (int16_t)(CrtRand() % 0x7D0 - 0x514);
				int32_t r = CrtRand() % 0xBB8;
				m->f16 = 0xC;
				m->f14 = (int16_t)(-0x1770 - r);
				int16_t s = (int16_t)(CrtRand() % 0xC00 + 0x480);
				m->f1C = s;
				m->f1E = (int16_t)(s / 6);
				int32_t v = (CrtRand() % 0x1800 + 0x680) / 2;
				m->f22 = (int16_t)v;
				m->f20 = (int16_t)v;
			}
		}
		n->c++;
		return n->c > 0xC ? TASK_END : 0;
	}

	static void DebrisDraw(const Mat4x3 *frame, const Mat4x3 *bone, const Node24 *n, int16_t sxz, int16_t sy, bool fade, int32_t fadev, int32_t t)
	{
		int16_t ang[3] = { 0, (int16_t)(n->f10 < 0 ? -100 : 100), 0x400 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[1] = n->f12;
		m.t[0] = n->f10;
		m.t[2] = n->f14;
		int32_t v[3] = { sxz, sxz, sy };
		Scale3DMatrix(&m, v);
		ComposeAffineTransform(bone, &m, &m);
		ComposeAffineTransform(frame, &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = game_addr(game::unk_E3BD64);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x2033;
		if (fade)
		{
			*(uint32_t *)(h + 0x1C) = 0x20F3;
			*(int32_t *)(h + 0xC) = fadev;
		}
		int32_t count = var<int32_t>(game_addr(game::unk_E3BD68));
		int16_t *out = (int16_t *)(MB() + 0x37000);
		const int16_t *a = (const int16_t *)game_addr(game::unk_E3C37C), *b = (const int16_t *)game_addr(game::unk_E3C57C);
		for (int32_t i = 0; i < count; i++, a += 4, b += 4, out += 4)
		{
			out[0] = (int16_t)((mul32(b[0] - a[0], t) >> 12) + a[0]);
			out[1] = (int16_t)((mul32(b[1] - a[1], t) >> 12) + a[1]);
			out[2] = (int16_t)((mul32(b[2] - a[2], t) >> 12) + a[2]);
		}
		*(uint32_t *)(h + 4) = MB() + 0x37000;
		Cursor() = RenderPrimModel(h, OT(), 3, Cursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl DebrisTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int16_t e = n->e;
		if (e > 0)
		{
			if (P1()) return 0;
			n->e = (int16_t)(e - 1);
			return 0;
		}
		int32_t c = n->c, d = n->f16 - 8;
		DebrisDraw(&Frame(), &BoneWorld(), n, n->f1C, n->f20, c >= d, shl32(c - d, 9), shl32(c, 12) / 12);
		if (P1()) return 0;
		n->f1C = (int16_t)(n->f1C - n->f1E);
		int16_t v = n->f22;
		n->f20 = (int16_t)(n->f20 + v);
		n->f22 = (int16_t)(v - v / 8);
		if (n->f20 > 0x7000) n->f20 = 0x7000;
		n->c++;
		if (n->c >= n->f16 || (var<uint8_t>(game_addr(game::g191_StateFlags)) & 2)) return TASK_END;
		return 0;
	}

	static void SpawnExplosions()
	{
		int32_t cnt = CrtRand() % 4 + 5;
		for (int32_t i = 0; i < cnt; i++)
		{
			Node24 *m = AddTask(game_addr(ORIG_ExplosionTask));
			m->c = 0;
			m->e = (int16_t)(1 << (i + 1));
			m->f10 = (int16_t)(CrtRand() % 0x3E8 + 0x3E8);
			m->f12 = (int16_t)(CrtRand() % 0x4B0 - 0x320);
			m->f14 = (int16_t)(CrtRand() % 0x2328 - 0x10CC);
			m->f16 = (int16_t)(CrtRand() % 0xC + 2 * i);
			m->f1C = (int16_t)(CrtRand() % 0x1000 + 0x1600);
		}
	}

	static void ShardDraw(uint8_t *h, uint8_t *s, const int16_t *pos, int16_t size, const int16_t *vel, int16_t age)
	{
		Mat4x3 *mm = (Mat4x3 *)(s + 0x28);
		ResetRotation(mm);
		*(int32_t *)(s + 0x3C) = pos[0];
		*(int32_t *)(s + 0x40) = pos[1];
		*(int32_t *)(s + 0x44) = pos[2];
		*(int32_t *)(s + 0x6C) = size;
		Scale3DMatrix(mm, (const int32_t *)(s + 0x68));
		int32_t *dir = (int32_t *)(s + 0x78);
		dir[0] = vel[0];
		dir[1] = vel[1];
		dir[2] = vel[2];
		Normalize(dir, dir);
		int32_t angle = GetRotationBetweenVectors((const void *)game_addr(game::unk_E3C978), dir, (int32_t *)(s + 0x88));
		BuildAxisAngleRotationMatrix(angle, (Mat4x3 *)(s + 0x48), (const int32_t *)(s + 0x88));
		GteMatrixMultiply((const Mat4x3 *)(s + 0x48), mm);
		ComposeAffineTransform((const Mat4x3 *)(s + 8), mm, mm);
		GteSetRotMatrix(mm);
		GteSetTransVector(mm);
		*(int16_t *)(h + 4) = age;
		Cursor() = InitEffectSequenceFromData(h, OT(), 2, Cursor());
	}

	static void ExplosionFrame(uint8_t *s, const Node24 *n, const Mat4x3 *frame)
	{
		*(int16_t *)s = 0;
		*(int16_t *)(s + 2) = 0;
		*(int16_t *)(s + 4) = 0;
		ComposeZYXRotationMatrix((const int16_t *)s, (Mat4x3 *)(s + 8));
		*(int32_t *)(s + 0x1C) = n->f10;
		*(int32_t *)(s + 0x20) = n->f12;
		*(int32_t *)(s + 0x24) = n->f14;
		ComposeAffineTransform(frame, (Mat4x3 *)(s + 8), (Mat4x3 *)(s + 8));
		GteSetRotMatrix((Mat4x3 *)(s + 8));
		GteSetTransVector((Mat4x3 *)(s + 8));
	}

	static uint32_t __cdecl ExplosionTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int16_t d = n->f16;
		if (d > 0)
		{
			if (!P1()) n->f16 = (int16_t)(d - 1);
			return 0;
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		uint8_t *s = (uint8_t *)FieldAlloc(0x98);
		*(uint32_t *)h = game_addr(game::unk_E3460C);
		*(uint16_t *)(h + 0x24) = 0x208;
		int32_t count = 0;
		if (n->c < 5)
		{
			int16_t sz = n->f1C;
			TransformCameraByShadowRotation(&n->f10, (int32_t)(((uint32_t)s & 0xFFFF0000u) | (uint16_t)sz), -((int32_t)sz >> 3));
			*(int16_t *)(h + 4) = n->c;
			Cursor() = InitEffectSequenceFromData(h, OT(), 2, Cursor());
		}
		*(uint32_t *)h = game_addr(game::unk_E34548);
		ExplosionFrame(s, n, &Frame());
		*(int32_t *)(s + 0x70) = 0x1000;
		*(int32_t *)(s + 0x68) = 0x1000;
		for (int i = 0; i < 130; i++)
		{
			Rec *r = R(game_addr(POOL_E), i);
			if (r->mask != (uint32_t)(int32_t)n->e) continue;
			ShardDraw(h, s, r->pos, r->size, r->vel, r->age);
			if (P1()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				r->mask = 0;
				continue;
			}
			for (int k = 0; k < 3; k++) r->pos[k] = (int16_t)(r->pos[k] + r->vel[k]);
			for (int k = 0; k < 3; k++)
			{
				int16_t v = r->vel[k];
				r->vel[k] = (int16_t)(v - (int16_t)(v >> 1));
			}
			count++;
		}
		FieldFree(0x98);
		FieldFree(0xB4);
		if (P1()) return 0;
		s = (uint8_t *)FieldAlloc(0x98);
		int16_t c = n->c;
		if (c >= 0 && c <= 4)
		{
			for (int k = 0; k < 6; k++)
			{
				int i = FreeRec(game_addr(POOL_E), 130);
				if (i < 0) break;
				Rec *r = R(game_addr(POOL_E), i);
				r->mask = (uint32_t)(int32_t)n->e;
				r->age = 0;
				r->size = (int16_t)(CrtRand() % 0x1000 + 0x1400);
				int32_t *dv = (int32_t *)(s + 0x68);
				dv[0] = CrtRand() % 0x1000 - 0x800;
				dv[1] = CrtRand() % 0x1000 - 0x800;
				dv[2] = CrtRand() % 0x1000 - 0x800;
				Normalize(dv, dv);
				int32_t d1 = CrtRand() % 0x190 + 0x12C;
				r->pos[0] = (int16_t)(mul32(dv[0], d1) >> 12);
				r->pos[1] = (int16_t)(mul32(d1, dv[1]) >> 12);
				r->pos[2] = (int16_t)(mul32(dv[2], d1) >> 12);
				int32_t d2 = CrtRand() % 0x104 + 0xDC;
				r->vel[0] = (int16_t)(mul32(dv[0], d2) >> 12);
				r->vel[1] = (int16_t)(mul32(d2, dv[1]) >> 12);
				r->vel[2] = (int16_t)(mul32(dv[2], d2) >> 12);
			}
		}
		FieldFree(0x98);
		n->c++;
		if (n->c < 4) return 0;
		if (count) return 0;
		return TASK_END;
	}

	inline uint8_t *KnockSlot(int32_t slot) { return (uint8_t *)(game_addr(game::g191_KnockTable) + 36 * slot); }

	static void KnockTargets()
	{
		int16_t center[4];
		CalculateCenterPosition((uint32_t)*(uint16_t *)(CastCtx() + 2), center);
		for (uint32_t i = 0; i < (*(uint8_t **)(CastCtx() + 4))[0x10]; i++)
		{
			uint8_t *t = *(uint8_t **)(CastCtx() + 4);
			uint32_t slot = (*(uint8_t **)(t + 8))[0x18 * i];
			Node24 *m = AddTask(game_addr(ORIG_KnockTask));
			m->c = 0;
			m->e = (int16_t)slot;
			int32_t r = CrtRand() % 2 + 1;
			m->f18 = (int16_t)shl32(r, 12);
			m->f1A = (int16_t)(1 << (i + 1));
			uint8_t *E = Entity(slot);
			GetEffectSpawnPosition(E, 0xF1, 0, &m->f10);
			uint8_t *T = KnockSlot((int32_t)slot);
			*(uint16_t *)T = (uint16_t)(*(uint16_t *)E & 0x1020);
			*(uint32_t *)(T + 0xC) = *(uint32_t *)(E + 0x1C);
			*(uint32_t *)(T + 0x10) = *(uint32_t *)(E + 0x20);
			*(uint32_t *)(T + 4) = *(uint32_t *)(E + 0xC);
			*(uint32_t *)(T + 8) = *(uint32_t *)(E + 0x10);
			int32_t d[3];
			d[0] = (int32_t)m->f10 - center[0];
			d[1] = (int32_t)m->f12 - center[1];
			d[2] = (int32_t)m->f14 - center[2];
			if (d[2] < 0) d[2] = -d[2];
			Normalize(d, d);
			*(int16_t *)(T + 0x18) = 0;
			*(int16_t *)(T + 0x16) = 0;
			*(int16_t *)(T + 0x14) = 0;
			for (int k = 0; k < 3; k++)
			{
				int32_t rr = CrtRand() % 0x1770;
				*(int16_t *)(T + 0x1C + 2 * k) = (int16_t)((mul32(d[k], 15000) >> 12) + center[k] + rr - 0xBB8);
			}
			*(int16_t *)(T + 0x1C) = (int16_t)(((int32_t)*(int16_t *)(T + 0x1C) - m->f10) / 16);
			*(int16_t *)(T + 0x1E) = (int16_t)(((int32_t)*(int16_t *)(T + 0x1E) - m->f12) / 16);
			*(int16_t *)(T + 0x20) = (int16_t)(((int32_t)*(int16_t *)(T + 0x20) - m->f14) / 16);
		}
	}

	static uint32_t __cdecl KnockTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int32_t c = n->c;
		int32_t w = mul32(n->f18, c) / 16;
		int32_t slot = n->e;
		uint8_t *E = Entity((uint32_t)slot);
		uint8_t *T = KnockSlot(slot);
		*(uint16_t *)E |= 0x1020;
		int16_t *rot = (int16_t *)(E + 0xC);
		rot[0] = (int16_t)w;
		rot[1] = (int16_t)(shl32(c, 12) / 16);
		int16_t d[4];
		d[0] = (int16_t)(n->f10 - *(int16_t *)(T + 0xC));
		d[1] = (int16_t)(n->f12 - *(int16_t *)(T + 0xE));
		d[2] = (int16_t)(n->f14 - *(int16_t *)(T + 0x10));
		Mat4x3 m;
		ComposeZYXRotationMatrix(rot, &m);
		MatVec(&m, d, d);
		int16_t *p = (int16_t *)(E + 0x1C);
		p[0] = (int16_t)(n->f10 - d[0]);
		p[1] = (int16_t)(n->f12 - d[1]);
		p[2] = (int16_t)(n->f14 - d[2]);
		int16_t cc = n->c;
		if (cc < 0x10)
		{
			p[0] = (int16_t)(p[0] + *(int16_t *)(T + 0x14));
			p[1] = (int16_t)(p[1] + *(int16_t *)(T + 0x16));
			p[2] = (int16_t)(p[2] + *(int16_t *)(T + 0x18));
		}
		else if (cc < 0x32)
		{
			if (cc == 0x10)
			{
				n->f20 = (int16_t)0xDCD8;
				n->f22 = 0x465;
			}
			p[1] = (int16_t)(p[1] + n->f20);
		}
		if (P1()) return 0;
		if (cc < 0x10)
		{
			*(int16_t *)(T + 0x14) = (int16_t)(*(int16_t *)(T + 0x14) + *(int16_t *)(T + 0x1C));
			*(int16_t *)(T + 0x16) = (int16_t)(*(int16_t *)(T + 0x16) + *(int16_t *)(T + 0x1E));
			*(int16_t *)(T + 0x18) = (int16_t)(*(int16_t *)(T + 0x18) + *(int16_t *)(T + 0x20));
		}
		else if (cc >= 0x2A) n->f20 = (int16_t)(n->f20 + n->f22);
		cc = (int16_t)(cc + 1);
		n->c = cc;
		if (cc < 0x32) return 0;
		*(uint16_t *)E = (uint16_t)((*(uint16_t *)E & 0xEFDF) | *(uint16_t *)T);
		*(uint32_t *)(E + 0x1C) = *(uint32_t *)(T + 0xC);
		*(uint32_t *)(E + 0x20) = *(uint32_t *)(T + 0x10);
		Node24 *m2 = AddTask(game_addr(ORIG_LandDustTask));
		m2->e = n->f1A;
		*(uint32_t *)&m2->f10 = *(uint32_t *)(T + 0xC);
		*(uint32_t *)&m2->f14 = *(uint32_t *)(T + 0x10);
		*(uint32_t *)(E + 0xC) = *(uint32_t *)(T + 4);
		*(uint32_t *)(E + 0x10) = *(uint32_t *)(T + 8);
		m2->c = 0;
		m2->f1C = (int16_t)(mul32(*(int16_t *)(E + 0x26), 2000) >> 12);
		return TASK_END;
	}

	static uint32_t __cdecl LandDustTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		if (!P1() && n->c <= 1)
		{
			for (int k = 0; k < 8; k++)
			{
				int i = FreeRec(game_addr(POOL_A), 170);
				if (i < 0) break;
				Rec *r = R(game_addr(POOL_A), i);
				r->mask = (uint32_t)(int32_t)n->e;
				r->age = 0;
				r->size = (int16_t)(CrtRand() % 0xC00 + 0x600);
				int32_t a = CrtRand() % 0x1000;
				int32_t sc = n->f1C;
				int32_t rr = CrtRand() % ((sc >> 1) & ~1);
				int32_t rad = rr - (sc >> 2) + sc;
				ff8fx::copy<(8)>(r->pos, &n->f10);
				r->pos[0] = (int16_t)(r->pos[0] + (mul32(ComputeSin(a), rad) >> 12));
				r->pos[1] = (int16_t)-(CrtRand() % 0xC8);
				r->pos[2] = (int16_t)(r->pos[2] + (mul32(ComputeCos(a), rad) >> 12));
				int32_t sp = CrtRand() % 0xBE + 0x78;
				r->vel[0] = (int16_t)(mul32(ComputeSin(a), sp) >> 12);
				r->vel[2] = (int16_t)(mul32(ComputeCos(a), sp) >> 12);
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		ff8fx::copy<(sizeof(Mat4x3))>(s + 8, &Camera());
		*(uint32_t *)h = game_addr(game::unk_E34404);
		*(uint32_t *)(h + 0x1C) = 0x586880;
		*(uint16_t *)(h + 0x24) = 0xC;
		int32_t count = 0;
		GteSetLightMatrix(s + 8);
		GteSetBackColorFromTrans(s + 8);
		for (int i = 0; i < 170; i++)
		{
			Rec *r = R(game_addr(POOL_A), i);
			if (!(r->mask & (uint32_t)(int32_t)n->e)) continue;
			OffsetSprite(h, (int32_t *)(s + 0x38), (int32_t *)(s + 0x28), r->pos, r->size, r->age, 3);
			if (P1()) continue;
			r->age++;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				r->mask = 0;
				continue;
			}
			r->pos[0] = (int16_t)(r->pos[0] + r->vel[0]);
			r->pos[2] = (int16_t)(r->pos[2] + r->vel[2]);
			int16_t vx = r->vel[0], vz = r->vel[2];
			r->vel[0] = (int16_t)(vx - (int16_t)(vx >> 2));
			r->vel[2] = (int16_t)(vz - (int16_t)(vz >> 2));
			count++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (P1()) return 0;
		n->c++;
		return count ? 0 : TASK_END;
	}
}

	void register_mag191_doomtrain()
	{
		using namespace d191;
		register_port(ORIG_CamScriptTask, (void *)CamScriptTask, "D191 CameraScriptTask (MAG_066, shared)", 191);
		register_port(ORIG_SequenceTick, (void *)SequenceTick, "D191 SequenceTick", 191);
		register_port(ORIG_TimelineTask, (void *)TimelineTask, "D191 TimelineTask", 191);
		register_port(ORIG_FlashInTask, (void *)FlashInTask, "D191 FlashInTask", 191);
		register_port(ORIG_FlashOutTask, (void *)FlashOutTask, "D191 FlashOutTask", 191);
		register_port(ORIG_RigTask, (void *)RigTask, "D191 RigTask", 191);
		register_port(ORIG_RigPrimATask, (void *)RigPrimATask, "D191 RigPrimATask", 191);
		register_port(ORIG_RigPrimBTask, (void *)RigPrimBTask, "D191 RigPrimBTask", 191);
		register_port(ORIG_RigRockTask, (void *)RigRockTask, "D191 RigRockTask", 191);
		register_port(ORIG_RigFlashTask, (void *)RigFlashTask, "D191 RigFlashTask", 191);
		register_port(ORIG_RigSpriteTask, (void *)RigSpriteTask, "D191 RigSpriteTask", 191);
		register_port(ORIG_SkyTask, (void *)SkyTask, "D191 SkyTask", 191);
		register_port(ORIG_SteamPuffTask, (void *)SteamPuffTask, "D191 SteamPuffTask", 191);
		register_port(ORIG_SteamPairTask, (void *)SteamPairTask, "D191 SteamPairTask", 191);
		register_port(ORIG_RainTask, (void *)RainTask, "D191 RainTask", 191);
		register_port(ORIG_EndTask, (void *)EndTask, "D191 EndTask", 191);
		register_port(ORIG_CreatureTask, (void *)CreatureTask, "D191 CreatureTask", 191);
		register_port(ORIG_WheelTrailTask, (void *)WheelTrailTask, "D191 WheelTrailTask", 191);
		register_port(ORIG_PuffLineTask, (void *)PuffLineTask, "D191 PuffLineTask", 191);
		register_port(ORIG_PuffWallTask, (void *)PuffWallTask, "D191 PuffWallTask", 191);
		register_port(ORIG_FlareTask, (void *)FlareTask, "D191 FlareTask", 191);
		register_port(ORIG_GlowTask, (void *)GlowTask, "D191 GlowTask", 191);
		register_port(ORIG_ChimneyTask, (void *)ChimneyTask, "D191 ChimneyTask", 191);
		register_port(ORIG_SteamTrailTask, (void *)SteamTrailTask, "D191 SteamTrailTask", 191);
		register_port(ORIG_SteamStillTask, (void *)SteamStillTask, "D191 SteamStillTask", 191);
		register_port(ORIG_SteamBurstTask, (void *)SteamBurstTask, "D191 SteamBurstTask", 191);
		register_port(ORIG_WheelSteamTask, (void *)WheelSteamTask, "D191 WheelSteamTask", 191);
		register_port(ORIG_WheelStillTask, (void *)WheelStillTask, "D191 WheelStillTask", 191);
		register_port(ORIG_WheelBurstTask, (void *)WheelBurstTask, "D191 WheelBurstTask", 191);
		register_port(ORIG_DebrisSpawnTask, (void *)DebrisSpawnTask, "D191 DebrisSpawnTask", 191);
		register_port(ORIG_DebrisTask, (void *)DebrisTask, "D191 DebrisTask", 191);
		register_port(ORIG_ExplosionTask, (void *)ExplosionTask, "D191 ExplosionTask", 191);
		register_port(ORIG_KnockTask, (void *)KnockTask, "D191 KnockTask", 191);
		register_port(ORIG_LandDustTask, (void *)LandDustTask, "D191 LandDustTask", 191);
	}
}

