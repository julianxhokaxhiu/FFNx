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

// Engine of the cinematic GFs

#pragma once

#include "fx_port.h"

namespace ff8fx
{
namespace gfc
{
	using namespace ff8fx::eng;

	template<typename T> inline T &MEM(uint32_t addr) { return *(T *)addr; }
	template<typename T> inline T &AT(const void *p, int32_t off) { return *(T *)((uint8_t *)p + off); }
	inline int8_t &S8(const void *p, int32_t o) { return AT<int8_t>(p, o); }
	inline uint8_t &U8(const void *p, int32_t o) { return AT<uint8_t>(p, o); }
	inline int16_t &S16(const void *p, int32_t o) { return AT<int16_t>(p, o); }
	inline uint16_t &U16(const void *p, int32_t o) { return AT<uint16_t>(p, o); }
	inline int32_t &S32(const void *p, int32_t o) { return AT<int32_t>(p, o); }
	inline uint32_t &U32(const void *p, int32_t o) { return AT<uint32_t>(p, o); }
	inline uint8_t *&PTR(const void *p, int32_t o) { return AT<uint8_t *>(p, o); }
	inline int32_t add32(int32_t a, int32_t b) { return (int32_t)((uint32_t)a + (uint32_t)b); }
	inline int32_t sub32(int32_t a, int32_t b) { return (int32_t)((uint32_t)a - (uint32_t)b); }

	typedef void (__cdecl *VoidFn)();

	inline uint8_t *&RT() { return MEM<uint8_t *>(game_addr(game::g_GfCinematic_RuntimeSlotPtr)); }
	inline uint8_t *&RCTX() { return MEM<uint8_t *>(game_addr(game::g_GfCinematic_RenderCtxPtr)); }
	inline uint8_t *&SEQ() { return MEM<uint8_t *>(game_addr(game::g_GfCinematic_SequenceStatePtr)); }
	inline uint8_t *&PTR3() { return MEM<uint8_t *>(game_addr(game::unk_27973C4)); }
	inline uint8_t *&CUR() { return MEM<uint8_t *>(game_addr(game::g_GfCinematic_CurBonePtr)); }
	inline uint8_t *&CTX() { return MEM<uint8_t *>(game_addr(game::g_GfCinematic_SequenceCtxPtr)); }
	inline uint8_t *&SCENE() { return MEM<uint8_t *>(game_addr(game::g_GfCinematic_SceneHeaderPtr)); }
	inline uint8_t *&STREAM() { return MEM<uint8_t *>(game_addr(game::g_GfCinematic_StreamCursor)); }
	inline uint8_t *&WS() { return MEM<uint8_t *>(game_addr(game::g_GfCinematic_WorkspacePtr)); }
	inline uint8_t *ORDER() { return (uint8_t *)game_addr(game::g_GfCinematic_BoneOrderList); }
	inline uint8_t *DRAWORDER() { return (uint8_t *)game_addr(game::g_GfCinematic_DrawOrderList); }
	inline uint16_t *NODEKEYS() { return (uint16_t *)game_addr(game::unk_2797204); }
	inline Mat4x3 *NODEMAT() { return (Mat4x3 *)game_addr(game::g_GfCinematic_CamMatrixMain); }
	inline Mat4x3 *CAMALT() { return (Mat4x3 *)game_addr(game::g_GfCinematic_CamMatrixAlt); }
	inline uint8_t *LIGHTS() { return (uint8_t *)game_addr(game::g_GfCinematic_BoneMatrixTable); }
	inline Mat4x3 *BBMAT() { return (Mat4x3 *)game_addr(game::unk_2797968); }
	inline uint8_t **RESFILE() { return (uint8_t **)game_addr(game::Magic_b_00); }
	inline uint32_t *DEPTHARR() { return (uint32_t *)game_addr(game::unk_2798C18); }
	inline float &FLT_1877DA8() { return MEM<float>(game_addr(game::g_GfCinematic_MeshDepthScale)); }
	inline uint32_t &PKTCUR() { return MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)); }
	inline int16_t &PROJH() { return MEM<int16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)); }
	inline uint32_t &FRAMERL() { return MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)); }
	inline uint32_t &UPDFLAGS() { return MEM<uint32_t>(game_addr(game::battle_to_update_flags_dword_1D96A9C)); }
	inline Mat4x3 &BATTLEVIEW() { return MEM<Mat4x3>(game_addr(game::unk_1D97778)); }
	inline int16_t *CAMEYE() { return (int16_t *)game_addr(game::Battle_Camera_world_XZ_s16); }
	inline int16_t *CAMAT() { return (int16_t *)game_addr(game::Battle_Camera_LookAt_XZ_s16); }

	static constexpr game::Id STATE_LO = game::unk_2796E00, STATE_HI = game::unk_2798C18;

	struct Clone
	{
		const char *name;
		int effect_id;
		uint32_t vm_table;
		uint32_t bone_table;
		uint32_t prim_table;
		uint32_t prim_size;
		uint32_t spr_table;
		uint32_t ptype_table;
		uint32_t pop_table;
		uint32_t draw_table;
		uint32_t attr_16A;
		uint32_t attr_184;
		uint32_t attr_186;
		uint32_t attr_194;
		uint32_t attr_19C;
		uint32_t desc;
		uint32_t ptr_block;
		uint32_t queue;
		uint32_t file00, file01;
		uint32_t tick;
		uint32_t load_cb;
		bool lit_dispatcher;
		int bone_count;
		VoidFn vm[0x200];
		VoidFn bigtab[256];
		VoidFn *bone, *prim, *spr, *ptype, *pop;
		VoidFn draw[256];
	};
	extern Clone *g_clone;
	inline Clone &C() { return *g_clone; }

	struct Generic
	{
		VoidFn vm[0x200];
		VoidFn bone[32];
		VoidFn prim[60];
		VoidFn spr[11];
		VoidFn ptype[5];
		VoidFn pop[24];
		VoidFn draw[73];
	};
	extern Generic g_generic;
	void fill_generic();

	struct Exception { int table; int index; };
	void init_clone(Clone &c, const Exception *ex, int nex);
	Clone *find_clone(int effect_id);
}
}

namespace ff8fx
{
namespace gfc
{
#define GFC_TAINT(d, s, n) do { } while (0)
	inline void call_vm(uint32_t op) { C().vm[op & 0x1FF](); }

	uint32_t SequenceTick(Clone &c);
	void BindContext();
	void ReleaseContext();
	void AnimChannelsNeg();
	void AnimChannelsPos();
	void AnimIntegrator();
	void BuildMatricesAndDraw();

	namespace x
	{
		template<typename F> inline F f(uint32_t a) { return (F)a; }
		inline void GteWriteData(int32_t value, int reg) { f<void (__cdecl *)(int32_t, int)>(game_addr(game::GTE_WriteDataReg))(value, reg); }
		inline void GteReadData(void *dst, int reg) { f<void (__cdecl *)(void *, int)>(game_addr(game::GTE_ReadDataRegTo))(dst, reg); }
		inline void GteReadCtrl(void *dst, int reg) { f<void (__cdecl *)(void *, int)>(game_addr(game::sub_45D7D0))(dst, reg); }
		inline void GteWriteCtrl(int32_t value, int reg) { f<void (__cdecl *)(int32_t, int)>(game_addr(game::GTE_WriteControlReg))(value, reg); }
		inline void GteWriteData2(int reg, int32_t value) { f<void (__cdecl *)(int, int32_t)>(game_addr(game::GTE_WriteDataRegAt))(reg, value); }
		inline void GteReadData2(int reg, void *dst) { f<void (__cdecl *)(int, void *)>(game_addr(game::GTE_ReadDataReg))(reg, dst); }
		inline void GteSetBackColor3(int32_t r, int32_t g, int32_t b) { f<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GTE_SetFarColorRGB))(r, g, b); }
		inline void GteSetRotMatrix(const void *m) { f<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetRotMatrix))(m); }
		inline void GteSetTransVector(const void *m) { f<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetTransVector))(m); }
		inline void GteLoadV0(const void *v) { f<void (__cdecl *)(const void *)>(game_addr(game::GTE_LoadV0))(v); }
		inline void GteLoadV012(const void *v0, const void *v1, const void *v2) { f<void (__cdecl *)(const void *, const void *, const void *)>(game_addr(game::GTE_LoadV012))(v0, v1, v2); }
		inline void GteReadOTZdiv4(void *dst) { f<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ_SZ3div4))(dst); }
		inline void GteReadSXY2(void *dst) { f<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadSXY2))(dst); }
		inline void GteReadSXY012(void *a, void *b, void *c) { f<void (__cdecl *)(void *, void *, void *)>(game_addr(game::GTE_ReadSXY012_Split))(a, b, c); }
		inline void GteReadOTZ(void *dst) { f<void (__cdecl *)(void *)>(game_addr(game::GTE_ReadOTZ))(dst); }
		inline void Gte_45E580() { f<void (__cdecl *)()>(game_addr(game::sub_45E580))(); }
		inline void GteAVSZ3() { f<void (__cdecl *)()>(game_addr(game::GTE_AVSZ3))(); }
		inline void GteNCLIP() { f<void (__cdecl *)()>(game_addr(game::GTE_NCLIP))(); }
		inline void GteRTPS() { f<void (__cdecl *)()>(game_addr(game::GTE_RTPS))(); }
		inline void GteRTPT() { f<void (__cdecl *)()>(game_addr(game::GTE_RTPT))(); }
		inline void Gte_4601B0() { f<void (__cdecl *)()>(game_addr(game::GTE_NCCS))(); }
		inline void GteMVMVA_RotV0Tr() { f<void (__cdecl *)()>(game_addr(game::GTE_MVMVA_RotV0_Tr))(); }
		inline void SetRotMatrix(const void *m) { f<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetRotMatrix_W))(m); }
		inline void SetLightMatrix(const void *m) { f<void (__cdecl *)(const void *)>(game_addr(game::sub_56BB00))(m); }
		inline void SetColorMatrix(const void *m) { f<void (__cdecl *)(const void *)>(game_addr(game::sub_56BB20))(m); }
		inline void SetTransVector(const void *m) { f<void (__cdecl *)(const void *)>(game_addr(game::GTE_SetTransVector_W))(m); }
		inline void ScaleMatrix(void *m, const void *v3) { f<void (__cdecl *)(void *, const void *)>(game_addr(game::scale3DMatrix))(m, v3); }
		inline void *MulMatrix3(const void *a, const void *b, void *out) { return f<void *(__cdecl *)(const void *, const void *, void *)>(game_addr(game::Matrix3x3_MultiplyPSX))(a, b, out); }
		inline void *MulMatrixInPlace(void *m, const void *n) { return f<void *(__cdecl *)(void *, const void *)>(game_addr(game::MatrixMultiply3x3))(m, n); }
		inline void *MulMatrixGte(const void *a, void *b) { return f<void *(__cdecl *)(const void *, void *)>(game_addr(game::GTE_MatrixMultiply))(a, b); }
		inline void Matrix_56C2C0(const void *m) { f<void (__cdecl *)(const void *)>(game_addr(game::sub_56C2C0))(m); }
		inline void *ComposeAffine(const void *a, const void *b, void *out) { return f<void *(__cdecl *)(const void *, const void *, void *)>(game_addr(game::ComposeAffineTransform))(a, b, out); }
		inline void TransformToCamera(const void *v, void *mac, void *flag) { f<void (__cdecl *)(const void *, void *, void *)>(game_addr(game::TransformCoordinateToCameraSpace))(v, mac, flag); }
		inline void SetBackColor(int32_t r, int32_t g, int32_t b) { f<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::sub_56CC30))(r, g, b); }
		inline void Fog_56CCA0(int32_t a, int32_t b) { f<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::sub_56CCA0))(a, b); }
		inline void Fog_56CCC0(int32_t a, int32_t b) { f<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::sub_56CCC0))(a, b); }
		inline void ParseCamera2(int32_t h) { f<void (__cdecl *)(int32_t)>(game_addr(game::Battle_SetProjectionH))(h); }
		inline void *RotMatrixFromAngles(const void *angles, void *out) { return f<void *(__cdecl *)(const void *, void *)>(game_addr(game::BuildRotationMatrixFromAngles))(angles, out); }
		inline int32_t Cos(int32_t a) { return f<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeCosine))(a); }
		inline int32_t Sin(int32_t a) { return f<int32_t (__cdecl *)(int32_t)>(game_addr(game::computeSin))(a); }
		inline void SetDrawMode(void *p, int32_t dfe, int32_t dtd, int32_t tpage, const void *rect) { f<void (__cdecl *)(void *, int32_t, int32_t, int32_t, const void *)>(game_addr(game::GPU_SetDrawMode))(p, dfe, dtd, tpage, rect); }
		inline void InsertPrimDepthKeys(uint32_t ot, void *prim, int32_t a, int32_t b, int32_t c, int32_t d) { f<void (__cdecl *)(uint32_t, void *, int32_t, int32_t, int32_t, int32_t)>(game_addr(game::SSIGPU_InsertPrimDepthKeys))(ot, prim, a, b, c, d); }
		inline void InsertPrimAltViewport(uint32_t ot, void *prim) { f<void (__cdecl *)(uint32_t, void *)>(game_addr(game::SSIGPU_InsertPrimAltViewport))(ot, prim); }
		inline void ClearOT(uint32_t ot, int32_t n) { f<void (__cdecl *)(uint32_t, int32_t)>(game_addr(game::SSIGPU_ClearOrderingTable))(ot, n); }
		inline void BuildBoneMatricesFromPose(void *hdr) { f<void (__cdecl *)(void *)>(game_addr(game::BattleModel_BuildBoneMatricesFromPose))(hdr); }
		inline int32_t ReadAnimation(void *hdr, void *cmd) { return f<int32_t (__cdecl *)(void *, void *)>(game_addr(game::Battle_ReadAnimation))(hdr, cmd); }
		inline int32_t PreReadAnimation(void *hdr, void *cmd, int32_t anim) { return f<int32_t (__cdecl *)(void *, void *, int32_t)>(game_addr(game::pre_Battle_ReadAnimation))(hdr, cmd, anim); }
		inline void ComputeBonesWorldMatrices(void *hdr, void *root) { f<void (__cdecl *)(void *, void *)>(game_addr(game::BS_ComputeBonesWorldMatrices))(hdr, root); }
		inline uint32_t RenderGeometry(void *a, void *b, uint32_t ot, int32_t mode, uint32_t cursor) { return f<uint32_t (__cdecl *)(void *, void *, uint32_t, int32_t, uint32_t)>(game_addr(game::RenderGeometry))(a, b, ot, mode, cursor); }
		inline uint32_t Render_5088A0(void *a, uint32_t ot, int32_t mode, uint32_t cursor) { return f<uint32_t (__cdecl *)(void *, uint32_t, int32_t, uint32_t)>(game_addr(game::BattleEntity_DrawShadow))(a, ot, mode, cursor); }
		inline int32_t Rand() { return f<int32_t (__cdecl *)()>(game_addr(game::_rand))(); }
		inline void *FieldAlloc(int32_t n) { return f<void *(__cdecl *)(int32_t)>(game_addr(game::Field_Alloc))(n); }
		inline void FieldFree(int32_t n) { f<void (__cdecl *)(int32_t)>(game_addr(game::Field_Free))(n); }

		inline void PlaySE(void *snd, int32_t a, int32_t b) { f<void (__cdecl *)(void *, int32_t, int32_t)>(game_addr(game::BdPlaySE))(snd, a, b); }
		inline void TransSummonStream(void *a, void *b) { f<void (__cdecl *)(void *, void *)>(game_addr(game::BdTransSummonStream))(a, b); }
		inline void PlaySummonStream(int32_t a, int32_t b, int32_t c) { f<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::BdPlaySummonStream))(a, b, c); }
		inline void Snd_46B3A0(int32_t a) { f<void (__cdecl *)(int32_t)>(game_addr(game::sub_46B3A0))(a); }
		inline void Snd_46B3E0(int32_t a) { f<void (__cdecl *)(int32_t)>(game_addr(game::sub_46B3E0))(a); }
		inline void Snd_46B450(int32_t a, int32_t b) { f<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::sub_46B450))(a, b); }
		inline void MusicSetVolumeTrans(int32_t a, int32_t b, int32_t c) { f<void (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::Music_SetVolumeTrans))(a, b, c); }
		inline void Music_47E3C0(int32_t a) { f<void (__cdecl *)(int32_t)>(game_addr(game::Battle_SetMusicVolumeUnlessEncounterFlag2))(a); }
		inline void PreLoadBattleFile(int32_t id, void *dst, int32_t a, uint32_t cb) { f<void (__cdecl *)(int32_t, void *, int32_t, uint32_t)>(game_addr(game::pre_LoadBattleFile))(id, dst, a, cb); }
		inline void Snd_4A2940(int32_t a) { f<void (__cdecl *)(int32_t)>(game_addr(game::BdSound_ReleaseVoiceSlot))(a); }
		inline uint32_t ClaimVoiceSlot(const void *snd, int32_t a, int32_t b) { return f<uint32_t (__cdecl *)(const void *, int32_t, int32_t)>(game_addr(game::BdSound_ClaimVoiceSlot))(snd, a, b); }
		inline void Battle_4A8480(int32_t a) { f<void (__cdecl *)(int32_t)>(game_addr(game::BattleMenu_SetVisible))(a); }
		inline void Battle_501E40(int32_t a) { f<void (__cdecl *)(int32_t)>(game_addr(game::BattleFade_QueueWhiteOverlay))(a); }
		inline void Battle_501F30(int32_t a) { f<void (__cdecl *)(int32_t)>(game_addr(game::BattleFade_ReleaseWhiteOverlay))(a); }
		inline void Camera_504270(int32_t a) { f<void (__cdecl *)(int32_t)>(game_addr(game::BattleAnim_SetSequencePause))(a); }
		inline void QueueVramUpload(const void *rect, const void *data) { f<void (__cdecl *)(const void *, const void *)>(game_addr(game::Battle_QueueVramUpload_Type0_RectData))(rect, data); }
		inline void ApplyActionResultToTarget(int32_t a) { f<void (__cdecl *)(int32_t)>(game_addr(game::ApplyActionResultToTarget))(a); }
		inline void ApplyActionResultToTargets(int32_t a, int32_t b) { f<void (__cdecl *)(int32_t, int32_t)>(game_addr(game::ApplyActionResultToTargets))(a, b); }
		inline void Battle_508630(void *a, void *b) { f<void (__cdecl *)(void *, void *)>(game_addr(game::Battle_RestoreDefaultEffectTexture))(a, b); }
		inline void PlayCameraAnimation(void *a) { f<void (__cdecl *)(void *)>(game_addr(game::Battle_PlayCameraAnimation))(a); }
		inline void CameraArmReturn() { f<void (__cdecl *)()>(game_addr(game::BS_Camera_ArmReturnAfterEffect))(); }
	}

	inline void pad_after_rot_mul(void *m, const void *angles) { U16(m, 0x12) = S16(angles, 2) < 0 ? 0xFFFF : 0; }

	namespace blob
	{
		template<typename F> inline F f(uint32_t a) { return (F)a; }
		inline void InitBone(int32_t id) { f<int32_t (__cdecl *)(int32_t)>(game_addr(game::MAG_006_sub_B65160))(id); }
		inline void OrderListRemove(int32_t id) { f<int32_t (__cdecl *)(int32_t)>(game_addr(game::MAG_006_sub_B651E0))(id); }
		inline void DrawListRemove(int32_t id) { f<int32_t (__cdecl *)(int32_t)>(game_addr(game::MAG_006_sub_B65270))(id); }
		inline void DrawListAddId(int32_t id) { f<int32_t (__cdecl *)(int32_t)>(game_addr(game::sub_B652E0))(id); }
		inline void SetDrawHandler(int32_t h) { f<int32_t (__cdecl *)(int32_t)>(game_addr(game::sub_B65320))(h); }
		inline uint8_t *GetBone(int32_t ref) { return f<uint8_t *(__cdecl *)(int32_t)>(game_addr(game::GfCinematic_GetRotationVector))(ref); }
		inline Mat4x3 *NodeForCurBone() { return f<Mat4x3 *(__cdecl *)()>(game_addr(game::sub_B65480))(); }
		inline Mat4x3 *GetParentMatrix(int32_t ref) { return f<Mat4x3 *(__cdecl *)(int32_t)>(game_addr(game::GfCinematic_GetParentMatrix))(ref); }
		inline Mat4x3 *RotMatrixOrder(const void *angles, int32_t order) { return f<Mat4x3 *(__cdecl *)(const void *, int32_t)>(game_addr(game::GfCinematic_RotMatrixOrder))(angles, order); }
		inline void CopyMatrix(void *dst, const void *src) { f<void (__cdecl *)(void *, const void *)>(game_addr(game::sub_B656A0))(dst, src); }
		inline uint8_t *ArenaAlloc(int32_t n)
		{
			return f<uint8_t *(__cdecl *)(int32_t)>(game_addr(game::GfCinematic_ArenaAlloc))(n);
		}
		inline void ArenaFree_B65710() { f<int32_t (__cdecl *)()>(game_addr(game::sub_B65710))(); }
		inline int32_t Rand73(int32_t range) { return f<int32_t (__cdecl *)(int32_t)>(game_addr(game::au_re__rand_73))(range); }
		inline int32_t Rand74(int32_t range) { return f<int32_t (__cdecl *)(int32_t)>(game_addr(game::au_re__rand_74))(range); }
		inline uint8_t *ObjectPtr(int32_t id) { return f<uint8_t *(__cdecl *)(int32_t)>(game_addr(game::sub_B657E0))(id); }
		inline uint8_t *ObjectPtrWs(int32_t id) { return f<uint8_t *(__cdecl *)(int32_t)>(game_addr(game::sub_B66230))(id); }
		inline void SetSpriteAnim(int32_t id) { f<int32_t (__cdecl *)(int32_t)>(game_addr(game::sub_B66270))(id); }
		inline uint8_t *VramRectRing() { return f<uint8_t *(__cdecl *)()>(game_addr(game::sub_B663A0))(); }
		inline int32_t AltTextureUpload(const void *rect, const void *data)
		{
			return f<int32_t (__cdecl *)(const void *, const void *)>(game_addr(game::GF_AlternativeTexture_UNK))(rect, data);
		}
		inline void ReadAltTexture(int32_t id) { f<int32_t (__cdecl *)(int32_t)>(game_addr(game::Magic_ReadAlternativeTexture))(id); }
		inline void ReadClut_B66560(int32_t id) { f<int32_t (__cdecl *)(int32_t)>(game_addr(game::sub_B66560))(id); }
		inline void TexInfo_B665C0(int32_t id) { f<int32_t (__cdecl *)(int32_t)>(game_addr(game::sub_B665C0))(id); }
		inline int32_t ClutWord_B66640(int32_t id) { return f<int32_t (__cdecl *)(int32_t)>(game_addr(game::sub_B66640))(id); }
		inline void ClutTint_B666F0(int32_t r, int32_t g, int32_t b) { f<int32_t (__cdecl *)(int32_t, int32_t, int32_t)>(game_addr(game::GfCinematic_ClutTintUpload))(r, g, b); }
		inline void Blob_B66A90() { f<int32_t (__cdecl *)()>(game_addr(game::sub_B66A90))(); }
		inline int32_t Blob_B66B30(int32_t a, int32_t b, int32_t c, int32_t d) { return f<int32_t (__cdecl *)(int32_t, int32_t, int32_t, int32_t)>(game_addr(game::sub_B66B30))(a, b, c, d); }
		inline void Blob_B66B80(void *a, int32_t b, int32_t c, void *d) { f<int32_t (__cdecl *)(void *, int32_t, int32_t, void *)>(game_addr(game::GfCinematic_ModelJointMatrix))(a, b, c, d); }
		inline void LookAt_B66E50(const void *a, const void *b, void *out) { f<int32_t (__cdecl *)(const void *, const void *, void *)>(game_addr(game::sub_B66E50))(a, b, out); }
	}

	void h_B269E0();
	int32_t h_B26AD0(uint8_t *blk);
	uint8_t *h_B26860(int32_t id);
	void h_B2C440();
	int32_t h_B2D460();
	int32_t h_B2F8F0();
	int32_t h_B2FC20();
	void h_B27000();
	void h_B27130();
	void h_B27360(const void *pos, const void *angles, int32_t scale, int32_t order);
	void h_B27440(const int16_t *clipArg);
	void h_B29450(int32_t c);

	void fill_core(Generic &g);
	void fill_vm_a(Generic &g);
	void fill_vm_b(Generic &g);
	void fill_vm_c(Generic &g);
	void fill_vm_d(Generic &g);
	void fill_draw_mesh(Generic &g);
	void fill_draw_prim(Generic &g);
	void fill_draw_sprite(Generic &g);

	void apply_shared_ribbon(Clone &c);
	void apply_shared_misc(Clone &c);
}
}
