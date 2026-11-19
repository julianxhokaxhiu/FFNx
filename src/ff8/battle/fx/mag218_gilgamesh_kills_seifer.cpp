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

#include "fx_port.h"
#include <stddef.h>

namespace ff8fx
{
namespace g218
{
	using namespace eng;

	namespace x
	{
		inline int32_t CrtRand() { return fn<int32_t (__cdecl *)()>(game_addr(game::_rand))(); }
		inline int32_t ComputeCos(int32_t a) { return fn<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
		inline void ComposeZYXRotationMatrix(const int16_t *angles, Mat4x3 *out) { fn<void (__cdecl *)(const int16_t *, Mat4x3 *)>(game_addr(game::ComposeZYXRotationMatrix))(angles, out); }
		inline void Scale3DMatrix(Mat4x3 *m, const int32_t *v) { fn<void (__cdecl *)(Mat4x3 *, const int32_t *)>(game_addr(game::scale3DMatrix))(m, v); }
		inline int32_t NormalizeVectorToFixedPoint(const int32_t *in, int32_t *out) { return fn<int32_t (__cdecl *)(const int32_t *, int32_t *)>(game_addr(game::NormalizeVectorToFixedPoint))(in, out); }
		inline void VectorNormalS16(const int16_t *in, int16_t *out) { fn<void (__cdecl *)(const int16_t *, int16_t *)>(game_addr(game::VectorNormalS16))(in, out); }
		inline uint32_t MatrixMultiplyVector(const Mat4x3 *m, const int16_t *in, int16_t *out) { return fn<uint32_t (__cdecl *)(const Mat4x3 *, const int16_t *, int16_t *)>(game_addr(game::matrixMultiplyVector))(m, in, out); }
		inline void MatrixMultiply(const Mat4x3 *a, Mat4x3 *b) { fn<void (__cdecl *)(const Mat4x3 *, Mat4x3 *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
		inline void TransformVectorBy3x3Matrix(const Mat4x3 *m, const int32_t *in, int32_t *out) { fn<void (__cdecl *)(const Mat4x3 *, const int32_t *, int32_t *)>(game_addr(game::TransformVectorBy3x3Matrix))(m, in, out); }
		inline int32_t GetRotationBetweenVectors(uint32_t ref, const int32_t *v, int32_t *axis) { return fn<int32_t (__cdecl *)(uint32_t, const int32_t *, int32_t *)>(game_addr(game::GetRotationBetweenVectors_AxisAngle))(ref, v, axis); }
		inline void BuildAxisAngleRotationMatrix(int32_t angle, Mat4x3 *out, const int32_t *axis) { fn<void (__cdecl *)(int32_t, Mat4x3 *, const int32_t *)>(game_addr(game::BuildAxisAngleRotationMatrix))(angle, out, axis); }
		inline int32_t GetEffectSpawnPosition(void *entity, int32_t bone, int32_t frac, int16_t *out) { return fn<int32_t (__cdecl *)(void *, int32_t, int32_t, int16_t *)>(game_addr(game::GetEffectSpawnPosition))(entity, bone, frac, out); }
		inline void GetDefaultEffectPosition(void *entity, int16_t *out) { fn<void (__cdecl *)(void *, int16_t *)>(game_addr(game::GetDefaultEffectPosition))(entity, out); }
		inline void GteReadOffset(int32_t *ofx, int32_t *ofy) { fn<void (__cdecl *)(int32_t *, int32_t *)>(game_addr(game::GTE_ReadScreenOffset))(ofx, ofy); }
		inline void GteSetOffset(int32_t ofx, int32_t ofy) { fn<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::Call_Bs_ParseCamera))(ofx, ofy); }
		inline int32_t PreReadAnimation(void *header, void *cmd, int32_t id) { return fn<int32_t (__cdecl *)(void *, void *, int32_t)>(game_addr(game::pre_Battle_ReadAnimation))(header, cmd, id); }
		inline int32_t ReadAnimation(void *header, void *cmd) { return fn<int32_t (__cdecl *)(void *, void *)>(game_addr(game::Battle_ReadAnimation))(header, cmd); }
		inline uint32_t RenderGeometry(void *geom, void *hdr, uint32_t ot, int32_t shift, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(geom, hdr, ot, shift, cursor); }
		inline void BdPlaySE(uint32_t se, int32_t a, int32_t b) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t)>(game_addr(game::BdPlaySE))(se, a, b); }
		inline void BdPlaySummonStream(int32_t a, int32_t b, int32_t c) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::BdPlaySummonStream))(a, b, c); }
		inline void BdTransSummonStream(uint32_t src, uint32_t state) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::BdTransSummonStream))(src, state); }
		inline int32_t VoiceSlotBusy(uint32_t slot) { return fn<int32_t (__cdecl *)(uint32_t)>(game_addr(game::BdSound_IsVoiceSlotActive))(slot); }
		inline void ReleaseVoiceSlot(uint32_t slot) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(slot); }
		inline void CameraScriptStart(uint32_t script, const void *frame, void *entity, TaskQueue *q, int32_t n) { fn<void (__cdecl *)(uint32_t, const void *, void *, TaskQueue *, int32_t)>(game_addr(game::MAG066_CamScript_Start))(script, frame, entity, q, n); }
		inline void ApplyActionResultToTarget(uint32_t rec) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::ApplyActionResultToTarget))(rec); }
		inline void QueueChainTransformation(void *entity, int32_t chain) { fn<void (__cdecl *)(void *, int32_t)>(game_addr(game::QueueChainTransformation))(entity, chain); }
		inline void PrintMonsterText(uint32_t slot, int32_t line, int32_t len) { fn<void (__cdecl *)(uint32_t, int32_t, int32_t)>(game_addr(game::printMonsterRelatedText))(slot, line, len); }
		inline void WaitAnimSeq(uint32_t buffer, void *flag) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::Battle_RestoreDefaultEffectTexture))(buffer, flag); }
		inline int32_t LoadState() { return fn<int32_t (__cdecl *)()>(game_addr(game::BattleFile_GetLoadingState))(); }
		inline void CharacterLoad(int32_t id, uint32_t dst) { fn<void (__cdecl *)(int32_t, uint32_t)>(game_addr(game::BattleFile_CharacterLoad))(id, dst); }
		inline void QueueTIMUpload(uint32_t tim) { fn<void (__cdecl *)(uint32_t)>(game_addr(game::Battle_QueueTIMUpload_GetEOF))(tim); }
		inline uint32_t SummonData() { return fn<uint32_t (__cdecl *)()>(game_addr(game::sub_571B70))(); }
		inline void InsertPrimAltViewport(uint32_t bucket, void *packet) { fn<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(bucket, packet); }
		inline void SetDrawEnv(uint32_t packet, uint32_t env) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_SetDrawEnv))(packet, env); }
		inline void SetDrawArea(uint32_t packet, const int16_t *rect) { fn<void (__cdecl *)(uint32_t, const int16_t *)>(game_addr(game::GPU_SetDrawAreaPrim))(packet, rect); }
		inline void SetDrawOffset(uint32_t packet, uint32_t xy) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::SSIGPU_SetDrawOffsetPrim))(packet, xy); }
		inline void SetDrawStp(uint32_t packet, int32_t on) { fn<void (__cdecl *)(uint32_t, int32_t)>(game_addr(game::SetDrawStp))(packet, on); }
		inline uint32_t RenderScrollPrimModel(void *header, uint32_t ot, int32_t mode, uint32_t cursor) { return fn<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::MAG_066_RenderScrollPrimModel))(header, ot, mode, cursor); }
		inline void GteSetLightMatrix(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetLightMatrix))(m); }
		inline void GteSetBackColorFromTrans(const void *m) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetBackColorFromTrans))(m); }
		inline void GteMVMVA_LightV0Bk() { fn<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
		inline void GteSetRotScale(int32_t s) { fn<void (__cdecl *)(int32_t)>(game_addr(game::GTE_SetRotScale))(s); }
		inline void GteSetTransFromVec32(const int32_t *v) { fn<void (__cdecl *)(const int32_t *)>(game_addr(game::GTE_SetTransFromVec32))(v); }
		inline void GteTransFromMAC() { fn<void (__cdecl *)()>(game_addr(game::GTE_TransFromMAC))(); }
		inline void GteLoadV012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(a, b, c); }
		inline void GteRTPT() { fn<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
		inline void GteNCLIP() { fn<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
		inline void GteReadFLAG(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadFLAG))(dst); }
		inline void GteReadMAC0(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadMAC0))(dst); }
		inline void GteReadSXY012Split(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
		inline void GteAVSZ3() { fn<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
		inline void GteReadOTZ32(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
		inline void GteLoadRGBC(const void *c) { fn<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadRGBC))(c); }
		inline void GteLoadRGB012(const void *a, const void *b, const void *c) { fn<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadRGB012))(a, b, c); }
		inline void GteDPCS() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCS))(); }
		inline void GteDPCT() { fn<void (__cdecl *)()>(game_addr(game::GTE_DPCT))(); }
		inline void GteStoreRGB2(void *dst) { fn<void (__cdecl *)(void *)>(game_addr(game::GTE_StoreRGB2))(dst); }
		inline void GteStoreRGB012(void *a, void *b, void *c) { fn<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_StoreRGB012))(a, b, c); }
		inline void GteSetFarColor(int32_t r, int32_t g, int32_t b) { fn<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColor))(r, g, b); }
		typedef uint32_t (__cdecl *ListFn)(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor);
		static constexpr game::Id LIST_F3 = game::sub_6353C0, LIST_F4 = game::sub_635650, LIST_FT4 = game::sub_635C40, LIST_G3 = game::sub_635FB0, LIST_G4 = game::sub_636270, LIST_GT4 = game::sub_6368E0;
	}
	using namespace x;

	inline bool Paused() { return (var<uint32_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C)) & 0x201) != 0; }
	inline uint32_t &VoiceSlot() { return var<uint32_t>(game_addr(game::unk_24F2340)); }
	static constexpr game::Id STREAM_STATE = game::unk_24F2344;
	inline Mat4x3 &RootMatrix() { return var<Mat4x3>(game_addr(game::unk_24F2348)); }
	inline uint32_t TargetSlot() { return var<uint32_t>(game_addr(game::unk_24F2368)); }
	inline TaskQueue &Q2() { return var<TaskQueue>(game_addr(game::unk_24F23B8)); }
	inline Mat4x3 &Frame() { return var<Mat4x3>(game_addr(game::unk_24F23F8)); }
	inline TaskQueue &Q1() { return var<TaskQueue>(game_addr(game::unk_24F2518)); }
	inline uint8_t *CastCtx() { return var<uint8_t *>(game_addr(game::unk_24F336C)); }
	inline uint32_t &Flags() { return var<uint32_t>(game_addr(game::unk_24F3544)); }
	inline uint32_t Tex() { return var<uint32_t>(game_addr(game::unk_24F3550)); }
	inline uint32_t &Cursor() { return var<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline uint32_t RenderList() { return var<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }
	inline uint32_t OT() { return RenderList() + 0x44; }
	inline uint8_t *PoolB() { return var<uint8_t *>(game_addr(game::unk_E257E0)); }
	inline uint8_t *PoolA() { return var<uint8_t *>(game_addr(game::unk_E257E4)); }
	inline uint32_t VertexScratch() { return var<uint32_t>(game_addr(game::unk_E257E8)); }
	inline uint8_t *Entity(uint32_t slot) { return (uint8_t *)(game_addr(game::BattleEntitySlotData) + 156 * slot); }
	static uint8_t *E1() { return (uint8_t *)game_addr(game::unk_24F2428); }
	static uint8_t *E2() { return (uint8_t *)game_addr(game::unk_24F3558); }
	static uint8_t *SHARDS() { return (uint8_t *)game_addr(game::unk_24F3378); }
	inline int16_t *Centroid16(int k) { return (int16_t *)(game_addr(game::unk_24F235C) + 4 * k); }

	static constexpr game::Id ORIG_SequenceTick = game::MAG_218_sub_6374D0;
	static constexpr game::Id ORIG_TimelineTask = game::MAG_218_sub_6328A0;
	static constexpr game::Id ORIG_FadeTileTask = game::MAG_218_sub_632EB0;
	static constexpr game::Id ORIG_SpinnerTask = game::MAG_218_sub_633090;
	static constexpr game::Id ORIG_FlareTask = game::MAG_218_sub_633200;
	static constexpr game::Id ORIG_RingTask = game::MAG_218_sub_633560;
	static constexpr game::Id ORIG_TileTask = game::MAG_218_sub_633680;
	static constexpr game::Id ORIG_GroundWaveTask = game::MAG_218_sub_633720;
	static constexpr game::Id ORIG_SlashTask = game::MAG_218_sub_6338B0;
	static constexpr game::Id ORIG_SwordTask = game::MAG_218_sub_633AC0;
	static constexpr game::Id ORIG_EndTask = game::MAG_218_sub_633BF0;
	static constexpr game::Id ORIG_GilTask = game::MAG_218_sub_633C30;
	static constexpr game::Id ORIG_GlowTask = game::MAG_218_sub_6340A0;
	static constexpr game::Id ORIG_DustTask = game::MAG_218_sub_634300;
	static constexpr game::Id ORIG_ShockRingTask = game::MAG_218_sub_634750;
	static constexpr game::Id ORIG_RisingRingTask = game::MAG_218_sub_634890;
	static constexpr game::Id ORIG_WideRingTask = game::MAG_218_sub_634A10;
	static constexpr game::Id ORIG_ShakeTask = game::MAG_218_sub_634B90;
	static constexpr game::Id ORIG_ShardsTask = game::MAG_218_sub_634C00;
	static constexpr game::Id ORIG_ShardTask = game::MAG_218_sub_635110;
	static constexpr game::Id ORIG_ShardRenderer = game::MAG_218_sub_635280;
	static constexpr game::Id ORIG_ListFT3 = game::sub_635970;
	static constexpr game::Id ORIG_ListGT3 = game::sub_6365E0;
	static constexpr game::Id ORIG_ShardDustTask = game::MAG_218_sub_636CA0;
	static constexpr game::Id ORIG_ShardChipTask = game::MAG_218_sub_636EB0;
	static constexpr game::Id ORIG_FlashTask = game::MAG_218_sub_637200;
	static constexpr game::Id ORIG_ModelDrawTask = game::MAG_218_sub_637350;

#pragma pack(push, 1)
	struct Node24
	{
		TaskNode hdr;
		int16_t c;
		int16_t e;
		int16_t p10, p12, p14, p16;
		int16_t a18, a1A;
		int16_t s1C, s1E;
		int16_t s20, s22;
	};
	struct RecB
	{
		uint32_t mask;
		int16_t age, size;
		int16_t x, y, z, w0E;
		int16_t vx, vy, vz, w16;
	};
	struct RecA
	{
		uint32_t mask;
		int16_t age, size;
		int16_t x, y, z, w0E;
		int16_t vx, vy, vz, w16;
		int16_t ax, sx, az, sz;
	};
	struct Shard
	{
		int16_t x, y, z, w6;
		int16_t vx, vy, vz, wE;
	};
#pragma pack(pop)
	static_assert(sizeof(Node24) == 0x24, "queue node is 0x24 bytes");
	static_assert(sizeof(RecB) == 0x18, "pool B record is 0x18 bytes");
	static_assert(sizeof(RecA) == 0x20, "pool A record is 0x20 bytes");
	static_assert(sizeof(Shard) == 0x10, "shard is 0x10 bytes");
}
}

namespace ff8fx
{
namespace g218
{
	static const int POOLB_N = 200, POOLA_N = 150;
	inline Shard *SH(int k) { return (Shard *)(SHARDS() + 0x10 * k); }
	static void SpawnRings();

	static Node24 *LinkQ1(uint32_t task_fn)
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&Q1(), task_fn);
		n->c = 0;
		n->e = 0;
		return n;
	}
	static Node24 *LinkQ2(uint32_t task_fn)
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&Q2(), task_fn);
		n->c = 0;
		n->e = 0;
		return n;
	}

	static void HideStage()
	{
		for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) var<uint8_t>(a) |= 2;
	}
	static void ShowStage()
	{
		for (uint32_t a = game_addr(game::unk_1D98991); a < game_addr(game::unk_1D98A41); a += 0x2C) var<uint8_t>(a) &= 0xFD;
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

	static void PrimModel(uint32_t model, uint32_t flags, bool fade, int32_t level, uint32_t ot, int32_t shift)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = model;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = flags;
		if (fade) *(int32_t *)(h + 0xC) = level;
		Cursor() = RenderPrimModel(h, ot, shift, Cursor());
		FieldFree(0x58);
	}

	static uint8_t *ScrollHeader(uint32_t model)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x90);
		*(uint32_t *)h = model;
		return h;
	}

	static uint32_t __cdecl SequenceTick(TaskNode *tn)
	{
		int a = ExecuteTaskQueue(&Q1());
		int b = ExecuteTaskQueue(&Q2());
		((Node24 *)tn)->c++;
		return (a | b) ? 0 : TASK_END;
	}

	static void Tile(int32_t r, int32_t g, int32_t b, int32_t otz)
	{
		uint32_t *p = (uint32_t *)Cursor();
		uint32_t col = ((((((uint32_t)b & 0xFF) | 0x6200u) << 8) | ((uint32_t)g & 0xFF)) << 8) | ((uint32_t)r & 0xFF);
		p[1] = col;
		((uint16_t *)p)[4] = 0;
		((uint16_t *)p)[5] = 0;
		p[0] = 0x3000000;
		((uint16_t *)p)[6] = 0x140;
		((uint16_t *)p)[7] = 0xD8;
		InsertPrimAltViewport(RenderList() + 4 * (uint32_t)otz + 0x44, p);
		p += 4;
		p[0] = 0x1000000;
		p[1] = 0xE1000220;
		InsertPrimAltViewport(RenderList() + 4 * (uint32_t)otz + 0x44, p);
		Cursor() = (uint32_t)(p + 2);
	}

	static int32_t FadeTileLevel(int32_t c)
	{
		if (c < 4) return (mul32(c, 255) / 4) & 0xFF;
		if (c >= 16) return 0xFF - ((mul32(c - 16, 255) / 8) & 0xFF);
		return 0xFF;
	}

	static uint32_t __cdecl FadeTileTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int32_t v = FadeTileLevel(n->c);
		Tile(v, v, v, 0);
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x18 ? TASK_END : 0;
	}

	static uint32_t __cdecl TileTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		Tile(0x70, 0x80, 0xF0, 0x1000);
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x14 ? TASK_END : 0;
	}

	static void SpawnSpinners()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&Q1(), game_addr(ORIG_SpinnerTask));
		n->c = 0;
		n->e = 0;
		n->a18 = (int16_t)(CrtRand() % 0x800 - 0x400);
		n->a1A = -16;
		n->s1C = 0xC80;
		n->s20 = 0x600;
		n = (Node24 *)AddTaskToQueue(&Q1(), game_addr(ORIG_SpinnerTask));
		n->c = 0;
		n->e = 1;
		n->a18 = (int16_t)(CrtRand() % 0x200);
		n->a1A = -8;
		n->s1C = 0xC00;
		n->s20 = 0xA00;
	}

	static void SpinnerDraw(const Node24 *n, int16_t a18, int16_t s1C)
	{
		int16_t ang[4] = { 0x400, a18, 0, 0 };
		Mat4x3 m;
		BuildRotationMatrixFromAngles(ang, &m);
		m.t[2] = var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		m.t[0] = 0;
		m.t[1] = 0;
		int32_t s[3] = { s1C, s1C, s1C };
		Scale3DMatrix(&m, s);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		*(uint32_t *)h = game_addr(game::unk_E21CC0);
		*(int32_t *)(h + 0xC) = n->s20;
		*(uint32_t *)(h + 0x1C) = (n->e ? 0xCu : 3u) | 0xF0u;
		*(uint32_t *)(h + 8) = 0;
		Cursor() = RenderPrimModel(h, RenderList() + 0x4068, 0xE, Cursor());
		FieldFree(0x58);
	}

	static uint32_t __cdecl SpinnerTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		SpinnerDraw(n, n->a18, n->s1C);
		if (Paused()) return 0;
		n->a18 = (int16_t)(n->a18 + n->a1A);
		n->c++;
		return n->c >= 0x3D ? TASK_END : 0;
	}

	static void SpawnFlare()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&Q1(), game_addr(ORIG_FlareTask));
		n->c = 0;
		n->p10 = 0;
		n->p12 = (int16_t)0xD8F0;
		n->p14 = 0;
		MatrixMultiplyVector(&RootMatrix(), &n->p10, &n->p10);
		n->p10 = (int16_t)(n->p10 + *Centroid16(0));
		n->p12 = (int16_t)(n->p12 + *Centroid16(1));
		n->p14 = (int16_t)(n->p14 + *Centroid16(2));
	}

	static void ScreenSpriteMatrix(const int16_t *v, int16_t size)
	{
		var<int16_t>(game_addr(game::unk_24F3608)) = size;
		var<int16_t>(game_addr(game::unk_24F3600)) = size;
		var<int16_t>(game_addr(game::unk_24F35F8)) = size;
		GteSetRotMatrix(&Camera());
		GteSetTransVector(&Camera());
		GteLoadV0(v);
		GteRTPS();
		int16_t sxy[2];
		GteReadSXY2(sxy);
		int16_t sx = (int16_t)((int32_t)sxy[0] / 8);
		int16_t sy = (int16_t)((int32_t)sxy[1] / 8);
		var<int32_t>(game_addr(game::unk_24F360C)) = (int32_t)sx - 0xA0;
		var<int32_t>(game_addr(game::unk_24F3610)) = (int32_t)sy - 0x6C;
		var<int32_t>(game_addr(game::unk_24F3614)) = (int32_t)var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		GteSetRotMatrix((const Mat4x3 *)game_addr(game::unk_24F35F8));
		GteSetTransVector((const Mat4x3 *)game_addr(game::unk_24F35F8));
	}

	static bool FlareColour(int32_t c, uint32_t *col)
	{
		int32_t v;
		if (c < 2) v = shl32(c, 6);
		else if (c >= 16) v = shl32(0x18 - c, 4);
		else return false;
		*col = (uint32_t)shl32(shl32(v, 8) | v, 8);
		return true;
	}

	static int32_t FlareA(int32_t c) { return -0x200 - mul32(c, 800); }
	static int32_t FlareB(int32_t c) { return mul32(c, 400) - 0x100; }
	static int32_t FlareDZ(int32_t c) { return -0xE00 - mul32(c, 25); }

	static void FlareDraw(const Node24 *n, int32_t c, int32_t a, int32_t b, int32_t dz)
	{
		int32_t d[3];
		d[0] = (int32_t)var<int16_t>(game_addr(game::Battle_Camera_world_XZ_s16)) - n->p10;
		d[1] = (int32_t)var<int16_t>(game_addr(game::unk_B8B7F2)) - n->p12;
		d[2] = (int32_t)var<int16_t>(game_addr(game::Battle_Camera_world_Y)) - n->p14;
		int32_t ref[3] = { 0, 0, -0x1000 };
		NormalizeVectorToFixedPoint(d, d);
		int32_t axis[4];
		int32_t angle = GetRotationBetweenVectors((uint32_t)ref, d, axis);
		Mat4x3 m;
		BuildAxisAngleRotationMatrix(angle, &m, axis);
		m.t[0] = n->p10;
		m.t[1] = n->p12;
		m.t[2] = n->p14;
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		*(uint32_t *)h = game_addr(game::unk_E19C14);
		*(uint16_t *)(h + 0x24) = 0;
		uint32_t col;
		if (FlareColour(c, &col))
		{
			*(uint16_t *)(h + 0x24) = 4;
			*(uint32_t *)(h + 0x1C) = col;
		}
		for (uint32_t t = game_addr(game::unk_E257F2); t < game_addr(game::unk_E25832); t += 8)
		{
			int32_t k = mul32(*(const int16_t *)(t + 2), 2200) >> 12;
			int16_t v[4];
			v[0] = (int16_t)(mul32(k, a) >> 12);
			v[2] = (int16_t)(mul32(k, dz) >> 12);
			v[1] = (int16_t)(mul32(k, b) >> 12);
			MatrixMultiplyVector(&m, v, v);
			v[0] = (int16_t)(v[0] + (int16_t)m.t[0]);
			v[1] = (int16_t)(v[1] + (int16_t)m.t[1]);
			v[2] = (int16_t)(v[2] + (int16_t)m.t[2]);
			ScreenSpriteMatrix(v, *(const int16_t *)t);
			*(int16_t *)(h + 4) = *(const int16_t *)(t - 2);
			Cursor() = InitEffectSequenceFromData(h, OT(), 2, Cursor());
		}
		FieldFree(0xB4);
	}

	static uint32_t __cdecl FlareTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		FlareDraw(n, n->c, FlareA(n->c), FlareB(n->c), FlareDZ(n->c));
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x18 ? TASK_END : 0;
	}

	static void SpawnRing()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&Q1(), game_addr(ORIG_RingTask));
		n->c = 0;
		n->p10 = 0;
		n->p12 = (int16_t)0xD8F0;
		n->p14 = 0;
		n->a18 = (int16_t)(CrtRand() % 0x200);
		n->a1A = (int16_t)(CrtRand() % 30 + 0x19);
	}

	static int32_t RingScale(int32_t c) { return shl32(ComputeSin(shl32(c, 11) / 16), 13) >> 12; }

	static void RingDraw(const Node24 *n, int16_t a18, int32_t v)
	{
		int16_t ang[4] = { 0x400, a18, 0, 0 };
		Mat4x3 m;
		BuildRotationMatrixFromAngles(ang, &m);
		m.t[0] = n->p10;
		m.t[2] = n->p14;
		m.t[1] = n->p12;
		int32_t s[3] = { v, v, v };
		Scale3DMatrix(&m, s);
		Mat4x3 w;
		ComposeAffineTransform(&Frame(), &m, &w);
		GteSetRotMatrix(&m);
		GteSetTransVector(&w);
		PrimModel(game_addr(game::unk_E1B7E0), 0x33, false, 0, OT(), 2);
	}

	static uint32_t __cdecl RingTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		RingDraw(n, n->a18, RingScale(n->c));
		if (Paused()) return 0;
		n->a18 = (int16_t)(n->a18 + n->a1A);
		n->c++;
		return n->c >= 0x10 ? TASK_END : 0;
	}

	static void SpawnGroundWave()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&Q1(), game_addr(ORIG_GroundWaveTask));
		n->c = 0;
		GetDefaultEffectPosition(Entity(TargetSlot()), &n->p10);
		n->p14 = (int16_t)(n->p14 + 0x190);
		n->p12 = 0;
	}

	static int32_t GroundWaveIR0(int32_t c) { return shl32(c, 12) / 18; }

	static void GroundWaveDraw(const Node24 *n, int32_t uv, int32_t ir0)
	{
		int16_t ang[4] = { 0, 0x800, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t s[3] = { 0xC00, 0x1E00, 0x1800 };
		m.t[0] = n->p10;
		m.t[1] = n->p12;
		m.t[2] = n->p14;
		Scale3DMatrix(&m, s);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = ScrollHeader(game_addr(game::unk_E22570));
		*(uint16_t *)(h + 0x1A) = 0x80;
		*(uint16_t *)(h + 0x1E) = 0x80;
		*(uint16_t *)(h + 0x18) = 0x40;
		*(uint16_t *)(h + 0x1C) = 0x40;
		*(int32_t *)(h + 0x14) = uv;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x20) = 0x33;
		*(uint32_t *)(h + 0x10) = 0;
		GteSetFarColor(0x10, 0x10, 0x90);
		uint32_t rgbc = 0x808080;
		GteLoadRGBC(&rgbc);
		GteSetIR0(ir0);
		GteDPCS();
		GteStoreRGB2(h + 0x24);
		Cursor() = RenderScrollPrimModel(h, OT(), 2, Cursor());
		FieldFree(0x90);
	}

	static uint32_t __cdecl GroundWaveTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		GroundWaveDraw(n, mul32(n->c, 24), GroundWaveIR0(n->c));
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x12 ? TASK_END : 0;
	}

	static void SlashDraw(const int16_t *p, const int16_t *eye, int32_t uv)
	{
		const int32_t ex = eye[0], ey = eye[1], ez = eye[2];
		int32_t d[3];
		d[0] = ex - p[0];
		d[1] = ey - p[1];
		d[2] = ez - p[2];
		NormalizeVectorToFixedPoint(d, d);
		int32_t axis[4];
		int32_t angle = GetRotationBetweenVectors(game_addr(game::unk_E25840), d, axis);
		Mat4x3 m;
		BuildAxisAngleRotationMatrix(angle, &m, axis);
		m.t[0] = (mul32(-2000, d[0]) >> 12) + ex;
		m.t[1] = (mul32(-2000, d[1]) >> 12) + ey;
		m.t[2] = (mul32(-2000, d[2]) >> 12) + ez;
		int32_t s[3] = { 0x1800, 0x1800, 0x3400 };
		Scale3DMatrix(&m, s);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = ScrollHeader(game_addr(game::unk_E23660));
		*(uint16_t *)(h + 0x1A) = 0x80;
		*(uint16_t *)(h + 0x1E) = 0x80;
		*(uint16_t *)(h + 0x18) = 0x40;
		*(uint16_t *)(h + 0x1C) = 0x40;
		*(int32_t *)(h + 0x14) = uv;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x20) = 0x33;
		*(uint32_t *)(h + 0x10) = 0;
		*(uint32_t *)(h + 0x24) = 0x808080;
		Cursor() = RenderScrollPrimModel(h, OT(), 2, Cursor());
		FieldFree(0x90);
	}

	static uint32_t __cdecl SlashTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		GetEffectSpawnPosition(Entity(TargetSlot()), 0xF0, 0x1000, &n->p10);
		SlashDraw(&n->p10, (const int16_t *)game_addr(game::g_CamScript_SavedEye), -mul32(n->c, 24));
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x16 ? TASK_END : 0;
	}

	static void SwordLocal(Mat4x3 *m)
	{
		int16_t ang[4] = { (int16_t)0xFC00, 0, 0, 0 };
		ComposeZYXRotationMatrix(ang, m);
		int32_t s[3] = { 0x5400, 0x5400, 0x5400 };
		m->t[0] = 0;
		m->t[1] = 0;
		m->t[2] = 0;
		Scale3DMatrix(m, s);
		Mat4x3 b;
		ComposeAffineTransform((const Mat4x3 *)(E1() + 0x40), (const Mat4x3 *)(**(uint32_t **)(E1() + 0x64) + 0x50), &b);
		ComposeAffineTransform(&b, m, m);
	}

	static uint32_t __cdecl SwordTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (Flags() & 2) return TASK_END;
		Mat4x3 m;
		SwordLocal(&m);
		ComposeAffineTransform(&Frame(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		if (n->c > 0 && !(Flags() & 4)) PrimModel(game_addr(game::unk_E254D8), 0x33, false, 0, OT(), 3);
		if (!Paused()) n->c++;
		return 0;
	}

	static uint32_t __cdecl EndTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->c == 1) WaitAnimSeq(Tex() + 0x11000, &n->e);
		uint16_t done = (uint16_t)n->e;
		n->c++;
		return done ? TASK_END : 0;
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

	static void AdvanceModel(uint8_t *E)
	{
		if (ReadAnimation(E + 0x60, E + 0x6C) == 1 && !(E[0] & 1))
			PreReadAnimation(E + 0x60, E + 0x6C, E[0x6C]);
	}

	static uint32_t g_vertex_scratch_override = 0;
	static uint32_t ModelScratch() { return g_vertex_scratch_override ? g_vertex_scratch_override : VertexScratch(); }

	static void DrawModel(uint8_t *E, const Mat4x3 *frame)
	{
		uint8_t *h = (uint8_t *)FieldAlloc(0x4C);
		ComposeAffineTransform(frame, (const Mat4x3 *)(E + 0x40), (Mat4x3 *)h);
		ComputeBonesWorldMatrices(E + 0x60, h);
		*(uint32_t *)(h + 0x30) = var<uint32_t>(game_addr(game::BATTLE_PACKET_BUFFER_END));
		*(uint32_t *)(h + 0x24) = ModelScratch();
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

	static uint32_t __cdecl ModelDrawTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint16_t fl = *(uint16_t *)E2();
		*(uint16_t *)E1() = fl;
		if (!(fl & 4))
		{
			ff8fx::copy<(0x20)>(E1() + 0x40, E2() + 0x40);
			*(uint32_t *)(E1() + 0x28) = var<uint32_t>(game_addr(game::unk_24F3580));
			DrawModel(E1(), &Frame());
			DrawModel(E2(), &Frame());
			fl = *(uint16_t *)E2();
		}
		if (Paused()) return 0;
		if (!(fl & 8))
		{
			AdvanceModel(E1());
			AdvanceModel(E2());
		}
		n->c++;
		return n->c >= 0x20E ? TASK_END : 0;
	}

	static void LinkGlow()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&Q1(), game_addr(ORIG_GlowTask));
		n->c = 0;
		n->s1C = 0x2400;
		n->s20 = 0x1E00;
	}

	static uint32_t __cdecl GilTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		Mat4x3 *M2 = (Mat4x3 *)(E2() + 0x40);
		if (n->c == 0)
		{
			ResetRotation(M2);
			var<uint16_t>(game_addr(game::unk_24F35A8)) = 0x1000;
			var<uint16_t>(game_addr(game::unk_24F35A0)) = 0x1000;
			var<uint16_t>(game_addr(game::unk_24F3598)) = 0x1000;
			M2->t[1] = 0;
			M2->t[0] = 0;
			M2->t[2] = 0;
			ModelInit(E2(), (uint8_t *)game_addr(game::unk_24F3500), (uint8_t *)Tex(), 0xF);
			ff8fx::copy<(0x20)>(E1() + 0x40, M2);
			ModelInit(E1(), (uint8_t *)game_addr(game::unk_24F3338), (uint8_t *)game_addr(game::unk_F1DD54), 0x10);
		}
		if (Paused()) return 0;
		int32_t d = n->c;
		if (d < 0x14)
		{
			if (d == 0)
			{
				SetAnim(E1(), 0);
				SetAnim(E2(), 0);
				M2->t[1] = -10000;
				n->s1C = 0x140;
				n->s20 = 0x50;
				n->s22 = -8;
			}
			int16_t v = n->s20;
			M2->t[1] += v;
			n->s20 = (int16_t)((int16_t)(v >> 3) + v);
			int16_t w = n->s22;
			M2->t[2] += w;
			n->s22 = (int16_t)((int16_t)(w >> 3) + w);
			int16_t s = n->s1C;
			var<int16_t>(game_addr(game::unk_24F35A8)) = s;
			var<int16_t>(game_addr(game::unk_24F35A0)) = s;
			var<int16_t>(game_addr(game::unk_24F3598)) = s;
			n->s1C = (int16_t)((int16_t)(s >> 3) + s);
			if (d == 0) LinkGlow();
		}
		else if ((d -= 0x14) < 0x14)
		{
			if (d == 0)
			{
				M2->t[1] = -5000;
				M2->t[2] = 0;
			}
			var<int16_t>(game_addr(game::unk_24F35A8)) = 0x1000;
			var<int16_t>(game_addr(game::unk_24F35A0)) = 0x1000;
			var<int16_t>(game_addr(game::unk_24F3598)) = 0x1000;
		}
		else if ((d -= 0x14) < 0x1A)
		{
			if (d == 0)
			{
				SetAnim(E1(), 1);
				SetAnim(E2(), 1);
				M2->t[1] = 0;
			}
			else if (d == 5)
			{
				LinkQ1(game_addr(ORIG_DustTask));
				SpawnRings();
			}
		}
		else if ((d -= 0x1A) < 0xF4)
		{
			if (d == 0)
			{
				SetAnim(E1(), 2);
				SetAnim(E2(), 2);
			}
		}
		else if ((d -= 0xF4) < 0x15)
		{
			if (d == 0)
			{
				SetAnim(E1(), 3);
				SetAnim(E2(), 3);
				LinkQ1(game_addr(ORIG_ShardsTask));
			}
			else if (d == 0xF) LinkQ1(game_addr(ORIG_FlashTask));
		}
		else if ((d -= 0x15) < 0x28)
		{
		}
		else if ((d -= 0x28) < 0x1E)
		{
			if (d == 0)
			{
				SetAnim(E1(), 4);
				SetAnim(E2(), 4);
			}
		}
		else if ((d -= 0x1E) < 0x7D)
		{
			if (d == 0)
			{
				SetAnim(E1(), 5);
				SetAnim(E2(), 5);
			}
			else if (d == 0xF) E2()[0] |= 8;
			else if (d == 0x69) *(uint16_t *)E2() &= 0xFFF7;
			else if (d == 0x6A) Flags() |= 2;
			else if (d >= 0x6D)
			{
				GteSetFarColor(0, 0, 0);
				uint32_t rgbc = 0x2606060;
				GteLoadRGBC(&rgbc);
				GteSetIR0(shl32(d - 0x6D, 12) / 16);
				GteDPCS();
				GteStoreRGB2((void *)game_addr(game::unk_24F3580));
			}
		}
		n->c++;
		if (n->c < 0x20E) return 0;
		Flags() |= 1;
		return TASK_END;
	}

	static void GlowPoints(int16_t *A, int16_t *C)
	{
		int16_t B[4];
		GetEffectSpawnPosition(E2(), 0x2E, 0xC00, A);
		GetEffectSpawnPosition(E2(), 0x34, 0xC00, B);
		GetEffectSpawnPosition(E2(), 0xF0, 0, C);
		A[0] = (int16_t)(((int32_t)B[0] + A[0]) / 2);
		A[1] = (int16_t)(((int32_t)B[1] + A[1]) / 2);
		A[2] = (int16_t)(((int32_t)B[2] + A[2]) / 2);
	}

	static void GlowDraw(const Node24 *n, const int16_t *A, const int16_t *C, int32_t k, int32_t uv)
	{
		int32_t d[3] = { (int32_t)C[0] - A[0], (int32_t)C[1] - A[1], (int32_t)C[2] - A[2] };
		NormalizeVectorToFixedPoint(d, d);
		int32_t axis[4];
		int32_t angle = GetRotationBetweenVectors(game_addr(game::unk_E25830), d, axis);
		Mat4x3 m;
		BuildAxisAngleRotationMatrix(angle, &m, axis);
		m.t[0] = (mul32(d[0], 600) >> 12) + C[0];
		m.t[1] = (mul32(d[1], 600) >> 12) + C[1];
		m.t[2] = (mul32(d[2], 600) >> 12) + C[2];
		int32_t s[3];
		s[0] = mul32(n->s1C, k) >> 12;
		s[2] = s[0];
		s[1] = mul32(n->s20, k) >> 12;
		Scale3DMatrix(&m, s);
		ComposeAffineTransform(&Frame(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = ScrollHeader(game_addr(game::unk_E1A0F0));
		*(int32_t *)(h + 0x14) = uv;
		*(int32_t *)(h + 0xC) = 0x400;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x20) = 0xF3;
		*(uint16_t *)(h + 0x18) = 0;
		*(uint16_t *)(h + 0x1A) = 0;
		*(uint16_t *)(h + 0x1C) = 0x40;
		*(uint16_t *)(h + 0x1E) = 0x80;
		*(uint32_t *)(h + 0x10) = 0;
		*(uint32_t *)(h + 0x24) = 0x808080;
		Cursor() = RenderScrollPrimModel(h, OT(), 2, Cursor());
		FieldFree(0x90);
	}

	static uint32_t __cdecl GlowTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		int16_t A[4], C[4];
		GlowPoints(A, C);
		GlowDraw(n, A, C, var<int16_t>(game_addr(game::unk_24F3598)), shl32(-n->c, 5));
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x28 ? TASK_END : 0;
	}

	static int FreeRecord(uint8_t *pool, int stride, int count)
	{
		for (int i = 0; i < count; i++)
			if (*(uint32_t *)(pool + stride * i) == 0) return i;
		return -1;
	}

	static void SpritePush(uint8_t *s, int16_t size)
	{
		int32_t *v = (int32_t *)(s + 0x38), *u = (int32_t *)(s + 0x28);
		GteReadMAC123(v);
		NormalizeVectorToFixedPoint(v, u);
		int32_t k = -((int32_t)size >> 3);
		v[0] += mul32(u[0], k) >> 12;
		v[1] += mul32(u[1], k) >> 12;
		v[2] += mul32(u[2], k) >> 12;
		GteSetTransFromVec32(v);
	}

	static void DustSetup(uint8_t *h, uint8_t *s)
	{
		*(uint16_t *)(h + 0x24) = 8;
		ff8fx::copy<(0x20)>(s + 8, &Frame());
		*(uint32_t *)h = game_addr(game::unk_E19CBC);
	}

	static void DustOne(uint8_t *h, uint8_t *s, const RecB *p)
	{
		GteLoadV0(&p->x);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(p->size);
		*(int16_t *)(h + 4) = p->age;
		SpritePush(s, p->size);
		Cursor() = InitEffectSequenceFromData(h, OT(), 2, Cursor());
	}

	static uint32_t __cdecl DustTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *pool = PoolB();
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		if (!Paused() && n->c <= 4)
		{
			for (int k = 0; k < 8; k++)
			{
				int i = FreeRecord(pool, 0x18, POOLB_N);
				if (i < 0) break;
				RecB *p = (RecB *)(pool + 0x18 * i);
				p->mask = 1;
				p->age = 0;
				p->size = (int16_t)(CrtRand() % 0xC00 + 0xA00);
				int32_t a = CrtRand() % 0x1000;
				int32_t r = CrtRand() % 300 + 400;
				p->x = (int16_t)(mul32(ComputeSin(a), r) >> 12);
				p->y = (int16_t)-(CrtRand() % 60);
				p->z = (int16_t)(mul32(ComputeCos(a), r) >> 12);
				int32_t r2 = CrtRand() % 120 + 40;
				p->vx = (int16_t)(mul32(ComputeSin(a), r2) >> 12);
				p->vz = (int16_t)(mul32(ComputeCos(a), r2) >> 12);
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		DustSetup(h, s);
		int live = 0;
		GteSetLightMatrix(s + 8);
		GteSetBackColorFromTrans(s + 8);
		for (int i = 0; i < POOLB_N; i++)
		{
			RecB *p = (RecB *)(pool + 0x18 * i);
			if (!(p->mask & 1)) continue;
			DustOne(h, s, p);
			if (Paused()) continue;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				p->mask = 0;
				continue;
			}
			p->age++;
			p->x = (int16_t)(p->x + p->vx);
			p->z = (int16_t)(p->z + p->vz);
			p->vx = (int16_t)(p->vx - (int16_t)(p->vx >> 3));
			p->vz = (int16_t)(p->vz - (int16_t)(p->vz >> 3));
			live++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (Paused()) return 0;
		n->c++;
		return live ? 0 : TASK_END;
	}

	static void SpawnRings()
	{
		Node24 *n = (Node24 *)AddTaskToQueue(&Q1(), game_addr(ORIG_ShockRingTask));
		n->c = 0;
		n->p14 = 0;
		n->p12 = 0;
		n->p10 = 0;
		n->s1E = 0x900;
		n->s1C = 0x900;
		for (int k = 0; k < 2; k++)
		{
			n = (Node24 *)AddTaskToQueue(&Q1(), game_addr(ORIG_RisingRingTask));
			n->c = 0;
			n->p14 = 0;
			n->p12 = 0;
			n->p10 = 0;
			n->a18 = (int16_t)(CrtRand() % 0x800 - 0x400);
			int32_t r = CrtRand() % 90;
			n->s1E = 0xB80;
			n->s1C = 0xB80;
			n->a1A = (int16_t)(-30 - r);
			int32_t v = (CrtRand() % 0x1000 + 0x1600) / 10;
			n->s22 = (int16_t)v;
			n->s20 = (int16_t)v;
		}
		n = (Node24 *)AddTaskToQueue(&Q1(), game_addr(ORIG_WideRingTask));
		n->c = 0;
		n->p14 = 0;
		n->p12 = 0;
		n->p10 = 0;
		n->a18 = (int16_t)(CrtRand() % 0x400 - 0x200);
		int32_t r = CrtRand() % 120;
		n->s1E = 0x740;
		n->s1C = 0x740;
		n->s22 = 0x3AA;
		n->s20 = 0x3AA;
		n->a1A = (int16_t)(r + 0xF);
		n = (Node24 *)AddTaskToQueue(&Q1(), game_addr(ORIG_ShakeTask));
		n->c = 0;
		n->e = 0x28;
	}

	static void RingPrim(const Node24 *n, uint32_t model, int16_t ay, int32_t sx, int32_t sy, bool fade, int32_t level)
	{
		int16_t ang[4] = { 0, ay, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = n->p10;
		m.t[1] = n->p12;
		m.t[2] = n->p14;
		int32_t s[3] = { sx, sy, sx };
		Scale3DMatrix(&m, s);
		ComposeAffineTransform(&Frame(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		PrimModel(model, fade ? 0xF3 : 0x33, fade, level, OT(), 2);
	}

	static uint32_t __cdecl ShockRingTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		RingPrim(n, game_addr(game::unk_E1EF08), 0, n->s1C, n->s1C, n->c >= 6, shl32(n->c - 6, 9));
		if (Paused()) return 0;
		int16_t v = n->s1E;
		n->s1C = (int16_t)(n->s1C + v);
		n->s1E = (int16_t)(v - (int16_t)(v / 5));
		n->c++;
		return n->c >= 0xE ? TASK_END : 0;
	}

	static uint32_t __cdecl RisingRingTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		RingPrim(n, game_addr(game::unk_E1C918), n->a18, n->s1C, n->s20, n->c >= 10, mul32(n->c - 10, 341));
		if (Paused()) return 0;
		n->a18 = (int16_t)(n->a18 + n->a1A);
		int16_t v = n->s1E;
		n->s1C = (int16_t)(n->s1C + v);
		n->s1E = (int16_t)(v - (int16_t)(v / 6));
		int16_t w = n->s22;
		n->s20 = (int16_t)(n->s20 + w);
		n->s22 = (int16_t)(w - (int16_t)(w / 18));
		n->c++;
		return n->c >= 0x16 ? TASK_END : 0;
	}

	static uint32_t __cdecl WideRingTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		RingPrim(n, game_addr(game::unk_E1E260), n->a18, n->s1C, n->s20, n->c >= 10, mul32(n->c - 10, 682));
		if (Paused()) return 0;
		n->a18 = (int16_t)(n->a18 + n->a1A);
		int16_t v = n->s1E;
		n->s1C = (int16_t)(n->s1C + v);
		n->s1E = (int16_t)(v - (int16_t)(v / 5));
		int16_t w = n->s22;
		n->s20 = (int16_t)(n->s20 + w);
		n->s22 = (int16_t)(w - (int16_t)(w / 8));
		n->c++;
		return n->c >= 0x10 ? TASK_END : 0;
	}

	static int16_t ShakeValue(int32_t c, int32_t amp)
	{
		int32_t v = mul32(ComputeSin(shl32(c, 10) / 8 + 0x400), amp) >> 12;
		if (!(c & 1)) v = -v;
		return (int16_t)v;
	}

	static uint32_t __cdecl ShakeTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		var<int16_t>(game_addr(game::BATTLE_CAMERA_SHAKE_OFFSET_Y)) = ShakeValue(n->c, n->e);
		if (Paused()) return 0;
		n->c++;
		return n->c >= 8 ? TASK_END : 0;
	}

	struct GrabPackets { game::Id env, exe, stp, area, ofs, xy; };
	static const GrabPackets GRAB_MODULE = { game::unk_24F24D8, game::unk_E25AD8, game::unk_24F3538, game::unk_24F23C8, game::unk_24F24C8, game::unk_24F34F8 };

	static void ScreenGrab(const int16_t *pos, const GrabPackets &us_pk)
	{
		const struct { uint32_t env, exe, stp, area, ofs, xy; } pk = { game_addr(us_pk.env), game_addr(us_pk.exe), game_addr(us_pk.stp), game_addr(us_pk.area), game_addr(us_pk.ofs), game_addr(us_pk.xy) };
		int16_t rect[4] = { 0x240, 0x100, 0x80, 0x100 };
		Mat4x3 m1, m2;
		m1.t[0] = 4 - (int32_t)pos[0] / 17;
		m1.t[1] = (int32_t)pos[2] / 17 - 3;
		m1.t[2] = var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		int16_t ang[4] = { 0x400, 0, 0, 0 };
		BuildRotationMatrixFromAngles(ang, &m1);
		int32_t s[3];
		s[2] = 0x1000000 / (0x10A4000 / rect[3]);
		s[1] = 0;
		s[0] = 0x1000000 / (0x820000 / rect[2]);
		Scale3DMatrix(&m1, s);
		int16_t ang2[4] = { 0, 0, (int16_t)0xFC00, 0 };
		BuildRotationMatrixFromAngles(ang2, &m2);
		MatrixMultiply(&m2, &m1);
		TransformVectorBy3x3Matrix(&m2, m1.t, m1.t);
		SetDrawEnv(pk.env, game_addr(game::g_Battle_DrawEnv) + 0x5C * (var<uint8_t>(game_addr(game::g_Battle_DisplayParity)) ^ 1u));
		InsertPrimAutoDepth(OT(), (void *)pk.env);
		int32_t ofx, ofy;
		GteReadOffset(&ofx, &ofy);
		GteSetOffset(rect[2] / 2, rect[3] / 2);
		GteSetRotMatrix(&m1);
		GteSetTransVector(&m1);
		uint8_t *h = (uint8_t *)FieldAlloc(0x2C);
		*(int16_t *)(h + 0x24) = var<int16_t>(game_addr(game::unk_1D989BE));
		*(int32_t *)(h + 0x10) = -1;
		*(int32_t *)(h + 0x20) = -1;
		*(uint16_t *)(h + 0x14) = 0;
		*(uint16_t *)(h + 0x16) = 0;
		*(uint16_t *)(h + 0x18) = 0x140;
		*(uint16_t *)(h + 0x1A) = 0xD8;
		*(uint32_t *)(h + 0x1C) = var<uint32_t>(game_addr(game::unk_1D989E4));
		*(uint32_t *)(h + 4) = var<uint32_t>(game_addr(game::g_BattleVertexWorkspace));
		if (var<uint8_t>(game_addr(game::unk_1D989BD)) & 1)
		{
			BuildBoneMatricesFromPose((void *)game_addr(game::unk_1D989D0));
			ComputeBonesWorldMatrices((void *)game_addr(game::unk_1D989D0), &m1);
			Cursor() = RenderGeometry((void *)game_addr(game::unk_1D989C0), h, OT(), 0x10, Cursor());
		}
		FieldFree(0x2C);
		GteSetOffset(ofx, ofy);
		InsertPrimAltViewport(OT(), (void *)pk.exe);
		SetDrawStp(pk.stp, 0);
		InsertPrimAutoDepth(OT(), (void *)pk.stp);
		SetDrawArea(pk.area, rect);
		InsertPrimAutoDepth(OT(), (void *)pk.area);
		var<int16_t>(pk.xy) = rect[0];
		var<int16_t>(pk.xy + 2) = rect[1];
		SetDrawOffset(pk.ofs, pk.xy);
		InsertPrimAutoDepth(OT(), (void *)pk.ofs);
	}

	static uint32_t __cdecl ShardsTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->c == 0)
		{
			n->p12 = 0;
			n->p10 = 0;
			n->p14 = (int16_t)0xFD44;
			MatrixMultiplyVector(&RootMatrix(), &n->p10, &n->p10);
			n->p10 = (int16_t)(n->p10 + *Centroid16(0));
			n->p12 = (int16_t)(n->p12 + *Centroid16(1));
			n->p14 = (int16_t)(n->p14 + *Centroid16(2));
			ScreenGrab(&n->p10, GRAB_MODULE);
		}
		if (n->c == 0xF)
		{
			int16_t P[4];
			GetEffectSpawnPosition(Entity(TargetSlot()), 0xF1, 0, P);
			P[1] = (int16_t)(P[1] + 0x64);
			int16_t V[4];
			V[0] = 0;
			V[1] = 0;
			V[2] = 0;
			MatrixMultiplyVector(&RootMatrix(), V, V);
			V[0] = (int16_t)(V[0] + *Centroid16(0));
			V[1] = (int16_t)(V[1] + *Centroid16(1));
			V[2] = (int16_t)(V[2] + *Centroid16(2));
			for (int k = 0; k < 24; k++)
			{
				Node24 *t = (Node24 *)AddTaskToQueue(&Q1(), game_addr(ORIG_ShardTask));
				t->c = 0;
				t->p16 = (int16_t)(k / 4);
				Shard *sh = SH(k);
				*(uint32_t *)&sh->x = *(uint32_t *)&n->p10;
				*(uint32_t *)&sh->z = *(uint32_t *)&n->p14;
				const int16_t *tb = (const int16_t *)(game_addr(game::unk_E258B2) + 8 * k);
				sh->x = (int16_t)(sh->x + tb[-1]);
				sh->y = (int16_t)(sh->y + tb[0]);
				sh->z = (int16_t)(sh->z + tb[1]);
				t->e = (int16_t)k;
				t->s1C = 0x1000;
				int32_t D[3];
				D[0] = (sh->x / 3) * 2;
				int32_t r = CrtRand() % 800;
				D[1] = (r + P[1]) * 2 - 0x320;
				D[2] = (int32_t)P[2] - V[2];
				NormalizeVectorToFixedPoint(D, D);
				int32_t m = CrtRand() % 350 + 250;
				sh->vx = (int16_t)(mul32(m, D[0]) >> 12);
				sh->vy = (int16_t)(mul32(m, D[1]) >> 12);
				sh->vz = (int16_t)(mul32(m, D[2]) >> 12);
			}
		}
		if (n->c == 0x10)
		{
			((Node24 *)AddTaskToQueue(&Q1(), game_addr(ORIG_ShardDustTask)))->c = 0;
			((Node24 *)AddTaskToQueue(&Q1(), game_addr(ORIG_ShardChipTask)))->c = 0;
		}
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0x15 ? TASK_END : 0;
	}

	static void ListVertices(uint8_t *h, const uint8_t *rec)
	{
		const uint8_t *vb = *(uint8_t **)(h + 4);
		static const int OFS[3] = { 0x34, 0x3C, 0x44 };
		const uint16_t idx[3] = { *(const uint16_t *)(rec + 4), *(const uint16_t *)(rec + 6), *(const uint16_t *)(rec + 8) };
		for (int k = 0; k < 3; k++)
		{
			int16_t *v = (int16_t *)(h + OFS[k]);
			*(uint32_t *)v = *(const uint32_t *)(vb + 4 * idx[k]);
			*(uint32_t *)(v + 2) = *(const uint32_t *)(vb + 4 * idx[k] + 4);
			v[0] = (int16_t)(v[0] - *(int16_t *)(h + 0x58));
			v[1] = (int16_t)(v[1] - *(int16_t *)(h + 0x5A));
			v[2] = (int16_t)(v[2] - *(int16_t *)(h + 0x5C));
		}
		GteLoadV012(h + 0x34, h + 0x3C, h + 0x44);
		GteRTPT();
	}

	static bool ListOffScreen(const uint8_t *p0, const uint8_t *p1, const uint8_t *p2)
	{
		uint32_t f = 0;
		const uint8_t *p[3] = { p0, p1, p2 };
		for (int k = 0; k < 3; k++)
		{
			int16_t x = *(const int16_t *)p[k], y = *(const int16_t *)(p[k] + 2);
			if (x < 0 || x > 0xA00) f |= 1u << k;
			if (y < 0 || y > 0x6C0) f |= 0x10u << k;
		}
		return (f & 7) == 7 || (f & 0x70) == 0x70;
	}

	static void ListTpageClut(const uint8_t *h, uint8_t *pkt, int tpage_ofs)
	{
		uint32_t fl = *(const uint32_t *)(h + 0x1C);
		if (fl & 0x400) *(int16_t *)(pkt + tpage_ofs) = (int16_t)(*(int16_t *)(pkt + tpage_ofs) + *(const int16_t *)(h + 0x10));
		else if (fl & 0x100) *(int16_t *)(pkt + tpage_ofs) = *(const int16_t *)(h + 0x10);
		if (fl & 0x800) *(int16_t *)(pkt + 0xE) = (int16_t)(*(int16_t *)(pkt + 0xE) + *(const int16_t *)(h + 0x14));
		else if (fl & 0x200) *(int16_t *)(pkt + 0xE) = *(const int16_t *)(h + 0x14);
	}

	static uint32_t __cdecl ListFT3(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *pkt = (uint8_t *)cursor;
		uint8_t *lp = *(uint8_t **)(h + 0x20);
		int32_t count = *(int32_t *)lp;
		uint8_t *rec = lp + 4;
		*(uint8_t **)(h + 0x20) = rec;
		for (; count > 0; count--, rec += 0x14)
		{
			ListVertices(h, rec);
			uint32_t fl = *(uint32_t *)(h + 0x1C);
			*(uint32_t *)pkt = 0x7000000;
			*(uint32_t *)(pkt + 4) = *(uint32_t *)rec;
			if (fl & 1) *(uint32_t *)(pkt + 4) |= 0x2000000;
			if (fl & 4) *(uint32_t *)(pkt + 4) &= 0xFDFFFFFF;
			const uint32_t uv = *(uint32_t *)(h + 0x18);
			*(uint32_t *)(pkt + 0xC) = *(uint32_t *)(rec + 0xC) + uv;
			*(uint32_t *)(pkt + 0x14) = *(uint32_t *)(rec + 0x10) + uv;
			*(uint32_t *)(pkt + 0x1C) = (*(uint32_t *)(rec + 8) >> 16) + uv;
			GteReadFLAG(h + 0x30);
			if (*(uint32_t *)(h + 0x30) & 0x60000) continue;
			GteNCLIP();
			ListTpageClut(h, pkt, 0x16);
			GteReadMAC0(h + 0x24);
			const int32_t mac0 = *(int32_t *)(h + 0x24);
			if (mac0 == 0) continue;
			if (mac0 < 0 && !(*(uint8_t *)(h + 0x1C) & 0x10)) continue;
			GteReadSXY012Split(pkt + 8, pkt + 0x10, pkt + 0x18);
			GteAVSZ3();
			if (ListOffScreen(pkt + 8, pkt + 0x10, pkt + 0x18)) continue;
			GteReadOTZ32(h + 0x2C);
			if (*(uint8_t *)(h + 0x1C) & 0x40)
			{
				GteLoadRGBC(pkt + 4);
				GteSetIR0(*(int32_t *)(h + 0xC));
				GteDPCS();
				GteStoreRGB2(pkt + 4);
			}
			InsertPrimAutoDepth(ot + 4 * (uint32_t)(*(int32_t *)(h + 0x2C) >> shift), pkt);
			pkt += 0x20;
		}
		*(uint8_t **)(h + 0x20) = rec;
		return (uint32_t)pkt;
	}

	static uint32_t __cdecl ListGT3(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		uint8_t *pkt = (uint8_t *)cursor;
		uint8_t *lp = *(uint8_t **)(h + 0x20);
		int32_t count = *(int32_t *)lp;
		uint8_t *rec = lp + 4;
		*(uint8_t **)(h + 0x20) = rec;
		for (; count > 0; count--, rec += 0x1C)
		{
			ListVertices(h, rec);
			uint32_t fl = *(uint32_t *)(h + 0x1C);
			*(uint32_t *)pkt = 0x9000000;
			*(uint32_t *)(pkt + 4) = *(uint32_t *)rec;
			if (fl & 2) *(uint32_t *)(pkt + 4) |= 0x2000000;
			if (fl & 8) *(uint32_t *)(pkt + 4) &= 0xFDFFFFFF;
			const uint32_t uv = *(uint32_t *)(h + 0x18);
			*(uint32_t *)(pkt + 0xC) = *(uint32_t *)(rec + 0xC) + uv;
			*(uint32_t *)(pkt + 0x18) = *(uint32_t *)(rec + 0x10) + uv;
			*(uint32_t *)(pkt + 0x24) = (*(uint32_t *)(rec + 8) >> 16) + uv;
			GteReadFLAG(h + 0x30);
			if (*(uint32_t *)(h + 0x30) & 0x60000) continue;
			GteNCLIP();
			ListTpageClut(h, pkt, 0x1A);
			GteReadMAC0(h + 0x24);
			if (*(int32_t *)(h + 0x24) < 0 && !(*(uint8_t *)(h + 0x1C) & 0x20)) continue;
			GteReadSXY012Split(pkt + 8, pkt + 0x14, pkt + 0x20);
			GteAVSZ3();
			if (ListOffScreen(pkt + 8, pkt + 0x14, pkt + 0x20)) continue;
			GteReadOTZ32(h + 0x2C);
			if (*(uint8_t *)(h + 0x1C) & 0x80)
			{
				GteLoadRGB012(rec + 0x14, rec + 0x18, pkt + 4);
				GteSetIR0(*(int32_t *)(h + 0xC));
				GteDPCT();
				GteStoreRGB012(pkt + 0x10, pkt + 0x1C, pkt + 4);
			}
			else
			{
				*(uint32_t *)(pkt + 0x10) = *(uint32_t *)(rec + 0x14);
				*(uint32_t *)(pkt + 0x1C) = *(uint32_t *)(rec + 0x18);
			}
			InsertPrimAutoDepth(ot + 4 * (uint32_t)(*(int32_t *)(h + 0x2C) >> shift), pkt);
			pkt += 0x28;
		}
		*(uint8_t **)(h + 0x20) = rec;
		return (uint32_t)pkt;
	}

	static uint32_t ShardRender(uint8_t *h, uint32_t ot, int32_t shift, uint32_t cursor)
	{
		const uint32_t fl = *(uint32_t *)(h + 0x1C);
		if (!(fl & 0x2000)) *(uint32_t *)(h + 4) = *(uint32_t *)h + 8;
		const uint32_t model = *(uint32_t *)h;
		*(uint32_t *)(h + 0x20) = model + *(uint32_t *)model;
		if (!(fl & 0x1000)) *(uint32_t *)(h + 0x18) = 0;
		GteSetFarColor(h[8], h[9], h[0xA]);
		const ListFn lists[8] = { (ListFn)game_addr(LIST_F3), (ListFn)game_addr(LIST_F4), ListFT3, (ListFn)game_addr(LIST_FT4), (ListFn)game_addr(LIST_G3), (ListFn)game_addr(LIST_G4), ListGT3, (ListFn)game_addr(LIST_GT4) };
		for (int k = 0; k < 8; k++)
		{
			if (**(uint32_t **)(h + 0x20) != 0) cursor = lists[k](h, ot, shift, cursor);
			else *(uint32_t *)(h + 0x20) += 4;
		}
		return cursor;
	}

	static void ShardDraw(const Node24 *n, const Shard *sh)
	{
		int16_t ang[4] = { 0, 0, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		m.t[0] = sh->x;
		m.t[1] = sh->y;
		int32_t s[3] = { n->s1C, n->s1C, n->s1C };
		m.t[2] = sh->z;
		Scale3DMatrix(&m, s);
		ComposeAffineTransform(&Camera(), &m, &m);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = (uint8_t *)FieldAlloc(0x60);
		const int32_t e = n->e;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0x3C;
		*(uint32_t *)h = *(const uint32_t *)(game_addr(game::unk_E25850) + 4 * e);
		*(uint32_t *)(h + 0x58) = *(const uint32_t *)(game_addr(game::unk_E258B0) + 8 * e);
		*(uint32_t *)(h + 0x5C) = *(const uint32_t *)(game_addr(game::unk_E258B4) + 8 * e);
		Cursor() = ShardRender(h, OT(), 2, Cursor());
		FieldFree(0x60);
	}

	static uint32_t __cdecl ShardTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		if (n->p16 > 0)
		{
			if (!Paused()) n->p16--;
			return 0;
		}
		Shard *sh = SH(n->e);
		ShardDraw(n, sh);
		if (Paused()) return 0;
		sh->x = (int16_t)(sh->x + sh->vx);
		sh->y = (int16_t)(sh->y + sh->vy);
		sh->z = (int16_t)(sh->z + sh->vz);
		n->c++;
		return n->c >= 0x2E ? TASK_END : 0;
	}

	static void ShardDustSetup(uint8_t *h, uint8_t *s)
	{
		ff8fx::copy<(0x20)>(s + 8, &Camera());
		*(uint32_t *)h = game_addr(game::unk_E19E68);
		*(uint32_t *)(h + 0x1C) = 0x404040;
		*(uint16_t *)(h + 0x24) = 0xC;
	}

	static void ShardDustOne(uint8_t *h, const RecB *p)
	{
		GteLoadV0(&p->x);
		GteMVMVA_LightV0Bk();
		GteSetRotScale(p->size);
		*(int16_t *)(h + 4) = (int16_t)(shl32(p->age, 1) + 1);
		GteTransFromMAC();
		Cursor() = InitEffectSequenceFromData(h, OT(), 2, Cursor());
	}

	static uint32_t __cdecl ShardDustTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *pool = PoolB();
		uint8_t *s = (uint8_t *)FieldAlloc(0x48);
		if (!Paused() && n->c <= 0x12)
		{
			for (int k = 0; k < 24; k++)
			{
				int i = FreeRecord(pool, 0x18, POOLB_N);
				if (i < 0) break;
				RecB *p = (RecB *)(pool + 0x18 * i);
				p->mask = 1;
				p->age = 0;
				p->size = (int16_t)(CrtRand() % 0xA00 + 0x600);
				*(uint32_t *)&p->x = *(uint32_t *)&SH(k)->x;
				*(uint32_t *)&p->z = *(uint32_t *)&SH(k)->z;
				p->x = (int16_t)(p->x + (CrtRand() % 1000 - 500));
				p->y = (int16_t)(p->y + (CrtRand() % 1000 - 500));
				p->z = (int16_t)(p->z + (CrtRand() % 1000 - 500));
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0xB4);
		ShardDustSetup(h, s);
		int live = 0;
		GteSetLightMatrix(s + 8);
		GteSetBackColorFromTrans(s + 8);
		for (int i = 0; i < POOLB_N; i++)
		{
			RecB *p = (RecB *)(pool + 0x18 * i);
			if (!(p->mask & 1)) continue;
			ShardDustOne(h, p);
			if (Paused()) continue;
			if (*(int16_t *)(h + 0x28) < 0)
			{
				p->mask = 0;
				continue;
			}
			p->age++;
			live++;
		}
		FieldFree(0xB4);
		FieldFree(0x48);
		if (Paused()) return 0;
		n->c++;
		return live ? 0 : TASK_END;
	}

	static void ChipSetup(uint8_t *h, uint8_t *s)
	{
		ff8fx::copy<(0x20)>(s + 8, &Camera());
		*(uint32_t *)h = game_addr(game::unk_E1F5F0);
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x1C) = 0;
		*(int16_t *)(s + 2) = 0;
	}

	static void ChipOne(uint8_t *h, uint8_t *s, const RecA *p)
	{
		Mat4x3 *M = (Mat4x3 *)(s + 0x28);
		*(int16_t *)(s + 0) = p->ax;
		*(int16_t *)(s + 4) = p->az;
		ComposeZYXRotationMatrix((const int16_t *)s, M);
		M->t[1] = p->y;
		M->t[2] = p->z;
		M->t[0] = p->x;
		int32_t *sc = (int32_t *)(s + 0x48);
		sc[2] = p->size;
		sc[1] = p->size;
		sc[0] = p->size;
		Scale3DMatrix(M, sc);
		ComposeAffineTransform((const Mat4x3 *)(s + 8), M, M);
		GteSetRotMatrix(M);
		GteSetTransVector(M);
		Cursor() = RenderPrimModel(h, OT(), 2, Cursor());
	}

	static uint32_t __cdecl ShardChipTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		uint8_t *pool = PoolA();
		uint8_t *s = (uint8_t *)FieldAlloc(0x68);
		if (!Paused() && n->c <= 6)
		{
			for (int k = 0; k < 24; k++)
			{
				int i = FreeRecord(pool, 0x20, POOLA_N);
				if (i < 0) break;
				RecA *p = (RecA *)(pool + 0x20 * i);
				p->mask = 1;
				p->age = 0;
				p->size = (int16_t)(CrtRand() % 0x400 + 0x80);
				*(uint32_t *)&p->x = *(uint32_t *)&SH(k)->x;
				*(uint32_t *)&p->z = *(uint32_t *)&SH(k)->z;
				p->x = (int16_t)(p->x + (CrtRand() % 1400 - 700));
				p->y = (int16_t)(p->y + (CrtRand() % 1400 - 700));
				p->z = (int16_t)(p->z + (CrtRand() % 1400 - 700));
				*(uint32_t *)s = *(uint32_t *)&SH(k)->vx;
				*(uint32_t *)(s + 4) = *(uint32_t *)&SH(k)->vz;
				VectorNormalS16((int16_t *)s, (int16_t *)s);
				int32_t m = CrtRand() % 950 + 100;
				p->vx = (int16_t)(mul32(((int16_t *)s)[0], m) >> 12);
				p->vy = (int16_t)(mul32(((int16_t *)s)[1], m) >> 12);
				p->vz = (int16_t)(mul32(((int16_t *)s)[2], m) >> 12);
				p->ax = (int16_t)(CrtRand() % 0x400);
				int32_t sp = CrtRand() % 67 + 0x1E;
				p->sx = (int16_t)((sp & 1) ? -sp : sp);
				p->az = (int16_t)(CrtRand() % 0x400 - 0x200);
				p->sz = (int16_t)(CrtRand() % 40 + 0xF);
			}
		}
		uint8_t *h = (uint8_t *)FieldAlloc(0x58);
		ChipSetup(h, s);
		for (int i = 0; i < POOLA_N; i++)
		{
			RecA *p = (RecA *)(pool + 0x20 * i);
			if (!(p->mask & 1)) continue;
			ChipOne(h, s, p);
			if (Paused()) continue;
			p->age++;
			p->x = (int16_t)(p->x + p->vx);
			p->y = (int16_t)(p->y + p->vy);
			p->z = (int16_t)(p->z + p->vz);
			p->vx = (int16_t)(p->vx - (int16_t)(p->vx >> 7));
			p->vy = (int16_t)(p->vy - (int16_t)(p->vy >> 7));
			p->vz = (int16_t)(p->vz - (int16_t)(p->vz >> 7));
			p->ax = (int16_t)(p->ax + p->sx);
			p->az = (int16_t)(p->az + p->sz);
		}
		FieldFree(0x58);
		FieldFree(0x68);
		if (Paused()) return 0;
		n->c++;
		if (n->c < 0x2D) return 0;
		for (int i = 0; i < POOLA_N; i++)
			if (*(uint8_t *)(pool + 0x20 * i) & 1) *(uint32_t *)(pool + 0x20 * i) = 0;
		return TASK_END;
	}

	static void FlashDraw(int32_t uv, bool fade, int32_t level)
	{
		int16_t ang[4] = { 0, 0, 0, 0 };
		Mat4x3 m;
		ComposeZYXRotationMatrix(ang, &m);
		int32_t s[3] = { 0xE00, 0xE00, 0xE00 };
		m.t[0] = 0;
		m.t[1] = 0;
		m.t[2] = var<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE));
		Scale3DMatrix(&m, s);
		GteSetRotMatrix(&m);
		GteSetTransVector(&m);
		uint8_t *h = ScrollHeader(game_addr(game::unk_E24FC8));
		*(uint16_t *)(h + 0x18) = 0x40;
		*(uint16_t *)(h + 0x1C) = 0x40;
		*(uint32_t *)(h + 8) = 0;
		*(uint32_t *)(h + 0x10) = 0;
		*(uint16_t *)(h + 0x1A) = 0x80;
		*(uint16_t *)(h + 0x1E) = 0x80;
		*(uint32_t *)(h + 0x20) = 0x33;
		*(uint32_t *)(h + 0x24) = 0x808080;
		*(int32_t *)(h + 0x14) = uv;
		if (fade)
		{
			*(uint32_t *)(h + 0x20) = 0xF3;
			*(int32_t *)(h + 0xC) = level;
		}
		Cursor() = RenderScrollPrimModel(h, OT(), 0xE, Cursor());
		FieldFree(0x90);
	}

	static uint32_t __cdecl FlashTask(TaskNode *tn)
	{
		Node24 *n = (Node24 *)tn;
		FlashDraw(mul32(-3 - n->c, 28), n->c >= 6, shl32(n->c - 6, 10));
		if (Paused()) return 0;
		n->c++;
		return n->c >= 0xA ? TASK_END : 0;
	}

	static void Text(int32_t line, int32_t len) { PrintMonsterText(TargetSlot(), line, len); }

	static uint32_t __cdecl TimelineTask(TaskNode *tn)
	{
		ComposeAffineTransform(&Camera(), &RootMatrix(), &Frame());
		const uint32_t f = var<uint32_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C));
		if (f & 0x201)
		{
			if (f & 1) return 0;
			if (LoadState() < 0) return 0;
		}
		Node24 *n = (Node24 *)tn;
		if (n->c == 0)
		{
			CameraScriptStart(game_addr(game::unk_E25970), &RootMatrix(), E2(), &Q1(), 0);
			BdTransSummonStream(game_addr(game::unk_F1DD50), game_addr(STREAM_STATE));
			VoiceSlot() = ClaimVoiceSlot((const void *)game_addr(game::unk_E25698), 1, 0x80);
		}
		int32_t d = n->c;
		if (d < 0x34)
		{
			if (d == 2)
			{
				BdPlaySE(game_addr(game::unk_E257D0), 0, 0x80);
				CharacterLoad(0x2C1, Tex() + 0x11000);
			}
			else if (d == 0x10)
			{
				if (LoadState() < 0) return 0;
				QueueTIMUpload(Tex() + 0x11000);
				CharacterLoad(0x2C2, Tex());
			}
			else if (d == 0) LinkQ1(game_addr(ORIG_FadeTileTask));
			else if (d == 0xB)
			{
				SpawnSpinners();
				ShowStage();
			}
			else if (d == 0x18) SpawnFlare();
			else if (d == 0x20) SpawnRing();
		}
		else if ((d -= 0x34) < 0x14)
		{
			if (d == 0)
			{
				if (LoadState() < 0) return 0;
				LinkQ2(game_addr(ORIG_ModelDrawTask));
				LinkQ1(game_addr(ORIG_GilTask));
				LinkQ1(game_addr(ORIG_SwordTask));
			}
		}
		else if ((d -= 0x14) < 0x14)
		{
			if (d == 0) LinkQ1(game_addr(ORIG_TileTask));
		}
		else if ((d -= 0x14) < 9)
		{
			if (d == 4) BdPlaySummonStream(0x80, 0, 0x60);
			else if (d == 0) HideStage();
		}
		else if ((d -= 9) < 0x33)
		{
			if (d == 0) CharacterLoad(0x2C3, Tex() + 0x11000);
			else if (d == 0xE)
			{
				if (LoadState() < 0) return 0;
				QueueTIMUpload(Tex() + 0x11000);
			}
			else if (d == 0xF) CharacterLoad(0x2C4, Tex() + 0x11000);
			else if (d == 0x19)
			{
				if (LoadState() < 0) return 0;
				QueueTIMUpload(Tex() + 0x11000);
			}
		}
		else if ((d -= 0x33) < 0x2D)
		{
			if (d == 0)
			{
				CharacterLoad(0x2C5, SummonData());
				Text(0, 0x2D);
			}
			else if (d == 0xA)
			{
				if (LoadState() < 0) return 0;
				BdTransSummonStream(SummonData(), game_addr(STREAM_STATE));
			}
		}
		else if ((d -= 0x2D) < 0xA5)
		{
			if (d == 0x14)
			{
				Text(1, 0x2D);
				E2()[0] |= 8;
			}
			else if (d == 0x41) *(uint16_t *)E2() &= 0xFFF7;
			else if (d == 0x55)
			{
				Text(2, 0x2D);
				E2()[0] |= 8;
			}
			else if (d == 0x82) *(uint16_t *)E2() &= 0xFFF7;
			else if (d == 0x96) Text(3, 0xF);
		}
		else if ((d -= 0xA5) < 0x15)
		{
			if (d == 4)
			{
				BdPlaySE(game_addr(game::unk_E257D4), 0, 0x80);
				Flags() &= ~4u;
			}
			else if (d == 0xE)
			{
				BdPlaySummonStream(0x80, 0, 0x60);
				Flags() &= ~4u;
			}
			else if (d == 0)
			{
				*(uint16_t *)E2() &= 0xFFF7;
				Flags() &= ~4u;
			}
			else if (d >= 8 && d <= 0xB) Flags() |= 4;
			else Flags() &= ~4u;
		}
		else if ((d -= 0x15) < 0x12)
		{
			if (d == 0)
			{
				uint8_t *t = Entity(TargetSlot());
				QueueChainTransformation(t, 0x37);
				t[0] |= 0x20;
				SpawnGroundWave();
			}
		}
		else if ((d -= 0x12) < 0x16)
		{
			if (d == 0)
			{
				LinkQ1(game_addr(ORIG_SlashTask));
				Text(5, 0x14);
			}
		}
		else if ((d -= 0x16) < 0x9C)
		{
			if (d == 0x88) BdPlaySE(game_addr(game::unk_E257D8), 0, 0x80);
			else if (d == 0x51) Text(6, 0x2D);
			else if (d == 0x99)
			{
				if (VoiceSlotBusy(VoiceSlot())) ReleaseVoiceSlot(VoiceSlot());
			}
		}
		if (n->c == 0x17F) ApplyActionResultToTarget(*(uint32_t *)(*(uint8_t **)(CastCtx() + 4) + 8));
		n->c++;
		if (n->c <= 0x243) return 0;
		LinkQ1(game_addr(ORIG_EndTask));
		uint8_t *t = Entity(TargetSlot());
		QueueChainTransformation(t, 1);
		t[0] |= 4;
		return TASK_END;
	}

	static void RegisterPorts()
	{
		register_port(ORIG_SequenceTick, (void *)SequenceTick, "G218 SequenceTick", 218);
		register_port(ORIG_TimelineTask, (void *)TimelineTask, "G218 TimelineTask", 218);
		register_port(ORIG_FadeTileTask, (void *)FadeTileTask, "G218 FadeTileTask", 218);
		register_port(ORIG_SpinnerTask, (void *)SpinnerTask, "G218 SpinnerTask", 218);
		register_port(ORIG_FlareTask, (void *)FlareTask, "G218 FlareTask", 218);
		register_port(ORIG_RingTask, (void *)RingTask, "G218 RingTask", 218);
		register_port(ORIG_TileTask, (void *)TileTask, "G218 TileTask", 218);
		register_port(ORIG_GroundWaveTask, (void *)GroundWaveTask, "G218 GroundWaveTask", 218);
		register_port(ORIG_SlashTask, (void *)SlashTask, "G218 SlashTask", 218);
		register_port(ORIG_SwordTask, (void *)SwordTask, "G218 SwordTask", 218);
		register_port(ORIG_EndTask, (void *)EndTask, "G218 EndTask", 218);
		register_port(ORIG_GilTask, (void *)GilTask, "G218 GilTask", 218);
		register_port(ORIG_GlowTask, (void *)GlowTask, "G218 GlowTask", 218);
		register_port(ORIG_DustTask, (void *)DustTask, "G218 DustTask", 218);
		register_port(ORIG_ShockRingTask, (void *)ShockRingTask, "G218 ShockRingTask", 218);
		register_port(ORIG_RisingRingTask, (void *)RisingRingTask, "G218 RisingRingTask", 218);
		register_port(ORIG_WideRingTask, (void *)WideRingTask, "G218 WideRingTask", 218);
		register_port(ORIG_ShakeTask, (void *)ShakeTask, "G218 ShakeTask", 218);
		register_port(ORIG_ShardsTask, (void *)ShardsTask, "G218 ShardsTask", 218);
		register_port(ORIG_ShardTask, (void *)ShardTask, "G218 ShardTask", 218);
		register_port(ORIG_ShardDustTask, (void *)ShardDustTask, "G218 ShardDustTask", 218);
		register_port(ORIG_ShardChipTask, (void *)ShardChipTask, "G218 ShardChipTask", 218);
		register_port(ORIG_FlashTask, (void *)FlashTask, "G218 FlashTask", 218);
		register_port(ORIG_ModelDrawTask, (void *)ModelDrawTask, "G218 ModelDrawTask", 218);
	}
}

	void register_mag218_gilgamesh_kills_seifer() { g218::RegisterPorts(); }
}

