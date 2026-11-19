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

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
	namespace xm
	{
		static inline uint32_t GTE_SetBackgroundVector(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetBackgroundVector))(a1, a2, a3); }
		static inline uint32_t GTE_SetLightMatrix(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_SetLightMatrix))(a1); }
		static inline uint32_t sub_45E0B0(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_LoadIR123_u16))(a1); }
		static inline uint32_t sub_45E160(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::GTE_SetSXY012))(a1, a2, a3); }
		static inline uint32_t sub_45E220(uint32_t a1) { return fn<uint32_t (__cdecl *)(uint32_t)>(game_addr(game::GTE_ReadSZ3))(a1); }
		static inline uint32_t sub_45E9D0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPF))(); }
		static inline uint32_t sub_45EBF0() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_GPL))(); }
		static inline uint32_t GTE_MVMVA_LightV0_Bk() { return fn<uint32_t (__cdecl *)()>(game_addr(game::GTE_MVMVA_LightV0_Bk))(); }
		static inline uint32_t TransformCameraByShadowRotation(uint32_t a1, uint32_t a2, uint32_t a3) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::TransformCameraByShadowRotation))(a1, a2, a3); }
		static inline uint32_t InitEffectSequenceFromData(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::InitEffectSequenceFromData))(a1, a2, a3, a4); }
		static inline uint32_t MAG_063_sub_7015B0(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::MAG_063_RotationFromAnglesYXZ))(a1, a2); }
	}
	enum BGIdx : int
	{
		BG_257B5BC,
		BG_2575378,
		BG_2579EBC,
		BG_2579544,
		BG_257526C,
		BG_2575374,
		BG_2579EB8,
		BG_2579540,
		BG_2579D48,
		BG_257B5A0,
		BG_257B168,
		BG_2575350,
		BG_257B5A8,
		BG_257B158,
		BG_2579D38,
		BG_2579530,
		BG_2575458,
		BG_152B5B8,
		BG_1683420,
		BG_168C654,
		BG_168F8BC,
		BG_1688434,
		BG_152B5BC,
		BG_257802C,
		BG_257B1E0,
		BG_257B5CC,
		BG_152BA38,
		BG_152BA80,
		BG_257AFB8,
		BG_168354C,
		BG_2579EC8,
		BG_152BB58,
		BG_152B5C8,
		BG_152B5C0,
		NBG
	};
	enum BFIdx : int
	{
		BF_729D50,
		BF_729D60,
		BF_729D70,
		BF_729DF0,
		BF_731CF0,
		BF_731D30,
		BF_731D40,
		BF_731D50,
		BF_731D80,
		BF_731D90,
		BF_731DB0,
		BF_72A380,
		BF_72AB20,
		BF_72A5C0,
		BF_72A660,
		BF_72A610,
		BF_72FBC0,
		BF_72A720,
		BF_72A840,
		BF_72FC70,
		BF_72FC90,
		BF_730490,
		BF_72FF80,
		BF_730610,
		BF_7306A0,
		BF_730B00,
		BF_730BA0,
		BF_730BD0,
		BF_730E30,
		BF_730EA0,
		BF_730EF0,
		BF_730F10,
		BF_730F50,
		BF_730F80,
		BF_730FB0,
		BF_730760,
		BF_730940,
		BF_730B60,
		BF_730C10,
		BF_730D00,
		BF_730D50,
		BF_730D70,
		BF_730DD0,
		BF_730E00,
		BF_730E20,
		BF_730C90,
		BF_730DA0,
		BF_730E80,
		BF_730EC0,
		BF_7318D0,
		BF_731920,
		BF_731940,
		BF_7311C0,
		BF_7319D0,
		BF_731A20,
		BF_731A40,
		BF_7186C0,
		NBF
	};
	struct BokoMod { int effect_id; game::Id g[NBG]; game::Id f[NBF]; };
	extern const BokoMod BOKO_097, BOKO_098, BOKO_099, BOKO_100;
	inline const BokoMod &boko_mod()
	{
		switch (g_mod->effect_id)
		{
		case 98: return BOKO_098;
		case 99: return BOKO_099;
		case 100: return BOKO_100;
		default: return BOKO_097;
		}
	}
	inline uint32_t BG(int k) { return game_addr(boko_mod().g[k]); }
	inline uint32_t BF(int k) { return game_addr(boko_mod().f[k]); }
	const BokoMod BOKO_097 = { 97,
		{ game::unk_257B5BC, game::MAG_097_CameraMatrixPtr, game::unk_2579EBC, game::unk_2579544, game::unk_257526C, game::unk_2575374, game::unk_2579EB8, game::unk_2579540, game::unk_2579D48, game::unk_257B5A0, game::unk_257B168, game::MAG_097_DirectorQueue, game::MAG_097_LoadScriptQueue, game::unk_257B158, game::unk_2579D38, game::unk_2579530, game::MAG_097_TaskQueue, game::unk_152B5B8, game::unk_1683420, game::unk_168C654, game::unk_168F8BC, game::MAG097_Boko_BodyTIM, game::unk_152B5BC, game::unk_257802C, game::unk_257B1E0, game::unk_257B5CC, game::unk_152BA38, game::unk_152BA80, game::MAG_097_AssetTablePtr, game::MAG097_Boko_ModelContainer, game::unk_2579EC8, game::unk_152BB58, game::unk_152B5C8, game::unk_152B5C0 },
		{ game::MAG_097_Boko_Master_State0_Advance, game::MAG_097_Boko_Master_State1_Advance, game::MAG_097_Boko_Master_State2_CarveWorkBuffers, game::MAG_097_Boko_Master_State3_SpawnEmitter, game::MAG_097_Boko_Master_State4_NextTargetLoop, game::MAG_097_Boko_Master_State5_Advance, game::MAG_097_Boko_Master_State6_WaitQueuesEmpty, game::MAG_097_Boko_Master_State7_RequestBattleSync, game::MAG_097_Boko_Master_State8_WaitBattleSyncDone, game::MAG_097_Boko_Master_State9_Finish, game::MAG_097_Boko_Master_State10_Empty, game::MAG_097_GetMonstersCentre, game::MAG_097_EmitterStageSpawnTask, game::MAG_097_SpawnChildTask, game::MAG_097_MainModelTask, game::Boko_SpawnPrimModelTask, game::MAG_097_EmitterStageTask, game::Boko_PrimTaskPlay, game::Boko_PrimDrawRecord, game::MAG_097_Camera_ResetBlock, game::MAG_097_Camera_StartKeyMove, game::MAG_097_DirStepGE, game::MAG_097_Camera_KeyStep, game::MAG_097_SetPauseSwitch, game::Boko_CreatureActorTask, game::MAG_097_Boko_CreatureActor_State0_Init, game::MAG_097_Boko_CreatureActor_State1_WaitModelAnimDone, game::MAG_097_Boko_CreatureActor_State2_SpawnShadow, game::MAG_097_Boko_CreatureActor_State3_Anim3AtCue1, game::MAG_097_Boko_CreatureActor_State4_PoseUntilCue2, game::MAG_097_Boko_CreatureActor_State5_PoseUntilFrame32, game::MAG_097_Boko_CreatureActor_State6_Anim4AtCue4, game::MAG_097_Boko_CreatureActor_State7_Anim5AtEnd, game::MAG_097_Boko_CreatureActor_State8_AnimUntilDirPhase6, game::MAG_097_Boko_CreatureActor_State9_Empty, game::MAG_097_DrawBattleModel, game::MAG_097_TexAnim_Step, game::MAG_097_TexAnim_Start, game::Boko_ShadowTask, game::MAG_097_Boko_Shadow_State0_Init, game::MAG_097_Boko_Shadow_State1_ShowAfterDelay, game::MAG_097_Boko_Shadow_State2_AnimateToFrame8, game::MAG_097_Boko_Shadow_State3_HoldFrames, game::MAG_097_Boko_Shadow_State4_CounterFinish, game::MAG_097_Boko_Shadow_State5_Empty, game::Boko_Shadow_Draw, game::MAG_097_StepCounterLimit, game::MAG_097_DirPhaseGE, game::MAG_097_DirSyncGate, game::MAG_097_Boko_ParticleTaskA_State0_Init, game::MAG_097_Boko_ParticleTaskA_State1_Play, game::MAG_097_Boko_ParticleTaskA_State2_Empty, game::MAG_097_UvModel_ScrollTextureV, game::MAG_097_Boko_ParticleTaskB_State0_Init, game::MAG_097_Boko_ParticleTaskB_State1_Play, game::MAG_097_Boko_ParticleTaskB_State2_Empty, {} } };
	const BokoMod BOKO_098 = { 98,
		{ game::unk_25750E4, game::unk_256E468, game::unk_2573C3C, game::unk_25732C4, game::unk_256E35C, game::unk_256E464, game::unk_2573C38, game::unk_25732C0, game::unk_2573AC8, game::unk_25750C8, game::unk_2574EE8, game::unk_256E440, game::unk_25750D0, game::unk_2574ED8, game::unk_2573AB8, game::unk_25732B0, game::unk_256E548, game::unk_152AF30, {}, {}, {}, {}, game::unk_152AF34, game::unk_257111C, game::unk_2574F60, game::unk_25750F4, game::unk_152B3D0, game::unk_152B418, game::unk_2574D38, game::MAG098_Boko_ModelContainer, game::unk_2573C48, game::unk_152B580, game::unk_152AF40, game::unk_152AF38 },
		{ game::MAG_098_sub_721B50, game::MAG_098_sub_721B60, game::MAG_098_sub_721B70, game::MAG_098_sub_721BF0, game::MAG_098_sub_729970, game::MAG_098_sub_7299B0, game::MAG_098_sub_7299C0, game::MAG_098_sub_7299D0, game::MAG_098_sub_729A00, game::MAG_098_sub_729A10, game::nullsub_1120, game::sub_722180, {}, {}, {}, {}, {}, game::sub_722520, game::sub_722640, game::sub_7277B0, game::sub_7277D0, game::sub_727FD0, game::sub_727AC0, game::sub_728180, game::sub_728210, game::sub_728670, game::sub_728710, game::sub_728740, game::sub_7289A0, game::sub_728A10, game::sub_728A60, game::sub_728A80, game::sub_728AB0, game::sub_728AE0, game::nullsub_1127, game::sub_7282D0, game::sub_7284B0, game::sub_7286D0, game::sub_728780, game::sub_728870, game::sub_7288C0, game::sub_7288E0, game::sub_728940, game::sub_728970, game::nullsub_1126, game::sub_728800, game::sub_728910, game::sub_7289F0, game::sub_728A30, game::sub_729550, game::sub_7295A0, game::nullsub_1133, game::sub_728E60, game::sub_729650, game::sub_7296A0, game::nullsub_1134, {} } };
	const BokoMod BOKO_099 = { 99,
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::unk_256D9D0, {}, {}, {}, game::unk_1529FA0, {}, {}, {}, {}, game::unk_1529FA4, game::unk_2569334, game::unk_256DA58, game::unk_256E1E4, game::unk_152AC48, game::unk_152AC90, game::unk_256D6F0, game::MAG099_Boko_ModelContainer, {}, game::unk_152AED0, {}, game::unk_1529FA8 },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::sub_718A70, game::sub_718B90, game::sub_71DD00, game::sub_71DD20, game::sub_71E520, {}, game::sub_71E840, game::Choco099_CreatureTask, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::sub_71ED80, {}, game::sub_71EF30, game::sub_71EF80, game::sub_71EFB0, game::sub_71F010, game::sub_71F050, game::nullsub_1092, game::sub_71EEC0, {}, game::sub_71F0D0, game::sub_71F110, game::sub_71FD40, game::sub_71FD90, game::nullsub_1098, game::sub_71F9E0, game::sub_71FE40, game::sub_71FE90, game::nullsub_1099, game::sub_7186C0 } };
	const BokoMod BOKO_100 = { 100,
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::unk_25656B8, {}, game::unk_2563420, game::unk_255ECB8, game::unk_15297F0, game::unk_1656CB8, game::unk_165FEEC, game::unk_1663154, game::MAG100_Boko_BodyTIM, game::unk_15297F4, game::unk_256188C, game::unk_2565740, game::unk_256588C, game::unk_1529CD8, game::unk_1529D20, game::unk_25653D8, game::MAG100_Boko_ModelContainer, game::unk_25642E8, game::unk_1529F18, game::unk_1529804, game::unk_15297F8 },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::sub_70FAF0, game::sub_70F550, game::sub_70F5F0, game::sub_70F5A0, game::MAG_100_BokoCreatureTask, game::sub_70F6B0, game::MAG_100_PrimModelDrawCallback, game::sub_7149A0, game::sub_7149C0, game::sub_7151C0, {}, game::sub_7154B0, game::MAG_100_CreatureActorTask, game::sub_7176B0, game::sub_717710, game::sub_7177E0, game::cintrindisp1_717860, game::sub_7178A0, game::sub_717930, game::sub_717980, game::sub_7179C0, game::sub_7179F0, game::nullsub_1085, game::sub_715600, game::sub_7157E0, game::sub_715A00, game::sub_715AB0, game::sub_715BA0, game::sub_715BF0, game::sub_715C10, game::sub_715C70, game::sub_715CA0, game::nullsub_1074, game::sub_715B30, game::sub_715C40, game::sub_715D20, game::sub_715300, game::sub_7167C0, game::sub_716810, game::nullsub_1079, game::sub_716460, game::sub_7168C0, game::sub_716910, game::nullsub_1080, game::sub_70F2E0 } };

	uint32_t __cdecl b8_728050(uint32_t a1);
	uint32_t __cdecl b_729BD0(uint32_t a1);
	uint32_t __cdecl b_72A350(void);
	uint32_t __cdecl b_72A540(uint32_t a1);
	uint32_t __cdecl b_72A610(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl b_72A6C0(uint32_t a1);
	uint32_t __cdecl b_72A720(uint32_t a1);
	uint32_t __cdecl b_72A840(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl b_72AAF0(uint32_t a1);
	uint32_t __cdecl b_72AD60(uint32_t a1);
	uint32_t __cdecl b_72AFE0(void);
	uint32_t __cdecl b_72BA00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl b_72BC10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl b_72C370(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl b_72C5A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl b_72C860(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl b_72CAC0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl b_72E220(uint32_t a1, uint32_t a2);
	uint32_t __cdecl b_72EF80(uint32_t a1);
	uint32_t __cdecl b_72FC40(uint32_t a1);
	uint32_t __cdecl b_730460(uint32_t a1);
	uint32_t __cdecl b_730510(uint32_t a1);
	uint32_t __cdecl b_7305D0(uint32_t a1);
	uint32_t __cdecl b_730660(uint32_t a1);
	uint32_t __cdecl b_7306A0(uint32_t a1);
	uint32_t __cdecl b_730B00(uint32_t a1);
	uint32_t __cdecl b_730BD0(uint32_t a1);
	uint32_t __cdecl b_730C10(uint32_t a1);
	uint32_t __cdecl b_730C90(uint32_t a1, uint32_t a2);
	uint32_t __cdecl b_730D00(uint32_t a1);
	uint32_t __cdecl b_730D50(uint32_t a1);
	uint32_t __cdecl b_730D70(uint32_t a1);
	uint32_t __cdecl b_730DD0(uint32_t a1);
	uint32_t __cdecl b_730E30(uint32_t a1);
	uint32_t __cdecl b_730EA0(uint32_t a1);
	uint32_t __cdecl b_730EF0(uint32_t a1);
	uint32_t __cdecl b_730F50(uint32_t a1);
	uint32_t __cdecl b_730FE0(uint32_t a1);
	uint32_t __cdecl b_731090(uint32_t a1);
	uint32_t __cdecl b_731520(uint32_t a1);
	uint32_t __cdecl b_7315D0(uint32_t a1);
	uint32_t __cdecl b_731690(uint32_t a1);
	uint32_t __cdecl b_7316B0(uint32_t a1);
	uint32_t __cdecl b_731720(uint32_t a1);
	uint32_t __cdecl b_731840(uint32_t a1);
	uint32_t __cdecl b_7318D0(uint32_t a1);
	uint32_t __cdecl b_731950(uint32_t a1);
	uint32_t __cdecl b_7319D0(uint32_t a1);
	uint32_t __cdecl b_731AF0(uint32_t a1);
	uint32_t __cdecl b_731BE0(uint32_t a1);
	uint32_t __cdecl b_728A80(uint32_t a1);
	uint32_t __cdecl b_718670(void);
	uint32_t __cdecl b_71F500(uint32_t a1);
	uint32_t __cdecl b_71F8E0(uint32_t a1);
	uint32_t __cdecl b_71F900(uint32_t a1);
	uint32_t __cdecl b7_729EA0(uint32_t a1);
	uint32_t __cdecl b7_729EF0(void);
	uint32_t __cdecl b7_72A410(uint32_t a1);
	uint32_t __cdecl b7_72A4F0(uint32_t a1);
	uint32_t __cdecl b7_72AB80(uint32_t a1);
	uint32_t __cdecl b7_72B690(uint32_t a1);
	uint32_t __cdecl b7_72CEE0(void);
	uint32_t __cdecl b7_72CF20(uint32_t a1);
	uint32_t __cdecl b7_72D7C0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl b7_72DF60(uint32_t a1, uint32_t a2);
	uint32_t __cdecl b7_72FB90(uint32_t a1);
	uint32_t __cdecl b7_72FBC0(uint32_t a1);
	uint32_t __cdecl b7_730F10(uint32_t a1);
	uint32_t __cdecl b7_7310C0(uint32_t a1);
	uint32_t __cdecl b7_731100(uint32_t a1);
	uint32_t __cdecl b7_7315A0(uint32_t a1);
	uint32_t __cdecl b7_731600(uint32_t a1);
	uint32_t __cdecl b7_731780(uint32_t a1);
	uint32_t __cdecl b8_721CA0(uint32_t a1);
	uint32_t __cdecl b8_721CF0(void);
	uint32_t __cdecl b8_722210(uint32_t a1);
	uint32_t __cdecl b8_7222F0(uint32_t a1);
	uint32_t __cdecl b8_722340(uint32_t a1);
	uint32_t __cdecl b8_722410(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl b8_7276F0(uint32_t a1);
	uint32_t __cdecl b8_728AE0(uint32_t a1);
	uint32_t __cdecl b8_728B20(uint32_t a1);
	uint32_t __cdecl b8_728C10(uint32_t a1);
	uint32_t __cdecl b8_728CC0(uint32_t a1);
	uint32_t __cdecl b8_728D60(uint32_t a1);
	uint32_t __cdecl b8_728DA0(uint32_t a1);
	uint32_t __cdecl b8_729240(uint32_t a1);
	uint32_t __cdecl b8_729270(uint32_t a1);
	uint32_t __cdecl b8_7292A0(uint32_t a1);
	uint32_t __cdecl b8_729350(uint32_t a1);
	uint32_t __cdecl b8_729420(uint32_t a1);
	uint32_t __cdecl b9_7181B0(uint32_t a1);
	uint32_t __cdecl b9_718200(void);
	uint32_t __cdecl b9_718750(uint32_t a1);
	uint32_t __cdecl b9_718840(uint32_t a1);
	uint32_t __cdecl b9_718890(uint32_t a1);
	uint32_t __cdecl b9_718960(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl b9_71DC40(uint32_t a1);
	uint32_t __cdecl b9_71E570(uint32_t a1);
	uint32_t __cdecl b9_71E5C0(uint32_t a1);
	uint32_t __cdecl b9_71E610(uint32_t a1);
	uint32_t __cdecl b9_71E6C0(uint32_t a1);
	uint32_t __cdecl b9_71E700(uint32_t a1);
	uint32_t __cdecl b9_71E8D0(uint32_t a1);
	uint32_t __cdecl b9_71EDF0(uint32_t a1);
	uint32_t __cdecl b9_71EF30(uint32_t a1);
	uint32_t __cdecl b9_71EF80(uint32_t a1);
	uint32_t __cdecl b9_71EFB0(uint32_t a1);
	uint32_t __cdecl b9_71F010(uint32_t a1);
	uint32_t __cdecl b9_71F190(uint32_t a1);
	uint32_t __cdecl b9_71F2D0(uint32_t a1);
	uint32_t __cdecl b9_71F3A0(uint32_t a1);
	uint32_t __cdecl b9_71F450(uint32_t a1);
	uint32_t __cdecl b9_71F470(uint32_t a1);
	uint32_t __cdecl b9_71F4C0(uint32_t a1);
	uint32_t __cdecl b9_71F520(uint32_t a1);
	uint32_t __cdecl b9_71F550(uint32_t a1);
	uint32_t __cdecl b9_71F5A0(uint32_t a1);
	uint32_t __cdecl b9_71F5C0(uint32_t a1);
	uint32_t __cdecl b9_71F620(uint32_t a1);
	uint32_t __cdecl b9_71FF20(uint32_t a1);
	uint32_t __cdecl b9_71FF70(uint32_t a1);
	uint32_t __cdecl b9_720020(uint32_t a1);
	uint32_t __cdecl b9_720080(uint32_t a1);
	uint32_t __cdecl b9_7200F0(uint32_t a1);
	uint32_t __cdecl b9_720140(uint32_t a1);
	uint32_t __cdecl b9_720250(uint32_t a1);
	uint32_t __cdecl b9_720290(uint32_t a1);
	uint32_t __cdecl b9_7202F0(uint32_t a1);
	uint32_t __cdecl b9_720380(uint32_t a1);
	uint32_t __cdecl b9_7203D0(uint32_t a1);
	uint32_t __cdecl b9_720470(uint32_t a1);
	uint32_t __cdecl b9_7204C0(uint32_t a1);
	uint32_t __cdecl b9_720500(uint32_t a1);
	uint32_t __cdecl b9_720570(uint32_t a1);
	uint32_t __cdecl b9_7205A0(uint32_t a1);
	uint32_t __cdecl b9_7205D0(uint32_t a1);
	uint32_t __cdecl b9_720670(uint32_t a1);
	uint32_t __cdecl b9_7206D0(uint32_t a1);
	uint32_t __cdecl b9_720790(uint32_t a1);
	uint32_t __cdecl b9_7207E0(uint32_t a1);
	uint32_t __cdecl b9_7208D0(uint32_t a1);
	uint32_t __cdecl b9_720940(uint32_t a1);
	uint32_t __cdecl b9_720960(uint32_t a1);
	uint32_t __cdecl b9_7209B0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl b9_720AE0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl b9_720B90(uint32_t a1);
	uint32_t __cdecl b9_720C90(uint32_t a1);
	uint32_t __cdecl b9_720D00(uint32_t a1);
	uint32_t __cdecl b9_720D20(uint32_t a1);
	uint32_t __cdecl b9_720D70(uint32_t a1, uint32_t a2);
	uint32_t __cdecl b9_720EA0(uint32_t a1);
	uint32_t __cdecl b9_720FA0(uint32_t a1);
	uint32_t __cdecl b9_720FC0(uint32_t a1);
	uint32_t __cdecl b9_7211D0(uint32_t a1);
	uint32_t __cdecl b9_721320(uint32_t a1);
	uint32_t __cdecl b9_721360(uint32_t a1);
	uint32_t __cdecl b9_721390(uint32_t a1);
	uint32_t __cdecl b9_7213F0(uint32_t a1);
	uint32_t __cdecl b9_721430(uint32_t a1);
	uint32_t __cdecl b9_7214C0(uint32_t a1);
	uint32_t __cdecl b9_7214E0(uint32_t a1);
	uint32_t __cdecl b9_721610(uint32_t a1);
	uint32_t __cdecl b9_721630(uint32_t a1);
	uint32_t __cdecl b9_721660(uint32_t a1);
	uint32_t __cdecl b10_70D530(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
	uint32_t __cdecl b10_70D680(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
	uint32_t __cdecl b10_70D8A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
	uint32_t __cdecl b10_70DB40(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
	uint32_t __cdecl b10_70DDA0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
	uint32_t __cdecl b10_70E060(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
	uint32_t __cdecl b10_70E2A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
	uint32_t __cdecl b10_70E580(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
	uint32_t __cdecl b10_70E800(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
	uint32_t __cdecl b10_70EDE0(uint32_t a1);
	uint32_t __cdecl b10_70EE30(void);
	uint32_t __cdecl b10_70F370(uint32_t a1);
	uint32_t __cdecl b10_70F480(uint32_t a1);
	uint32_t __cdecl b10_70F7D0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl b10_7148C0(uint32_t a1);
	uint32_t __cdecl b10_715210(uint32_t a1);
	uint32_t __cdecl b10_715250(uint32_t a1);
	uint32_t __cdecl b10_715280(uint32_t a1);
	uint32_t __cdecl b10_7152C0(uint32_t a1);
	uint32_t __cdecl b10_7152E0(uint32_t a1);
	uint32_t __cdecl b10_715330(uint32_t a1);
	uint32_t __cdecl b10_7153B0(uint32_t a1);
	uint32_t __cdecl b10_715540(uint32_t a1);
	uint32_t __cdecl b10_715DB0(uint32_t a1);
	uint32_t __cdecl b10_715DD0(uint32_t a1);
	uint32_t __cdecl b10_715DF0(uint32_t a1);
	uint32_t __cdecl b10_715F80(uint32_t a1);
	uint32_t __cdecl b10_716040(uint32_t a1);
	uint32_t __cdecl b10_7160C0(uint32_t a1);
	uint32_t __cdecl b10_716120(uint32_t a1);
	uint32_t __cdecl b10_716910(uint32_t a1);
	uint32_t __cdecl b10_7169B0(uint32_t a1);
	uint32_t __cdecl b10_716A20(uint32_t a1);
	uint32_t __cdecl b10_716BB0(uint32_t a1);
	uint32_t __cdecl b10_716C20(uint32_t a1);
	uint32_t __cdecl b10_716CD0(uint32_t a1);
	uint32_t __cdecl b10_716DD0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl b10_717330(uint32_t a1);
	uint32_t __cdecl b10_717350(uint32_t a1);
	uint32_t __cdecl b10_717450(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl b10_7174B0(uint32_t a1);
	uint32_t __cdecl b10_7174D0(uint32_t a1);
	uint32_t __cdecl b10_717540(uint32_t a1);
	uint32_t __cdecl b10_717590(uint32_t a1);
	uint32_t __cdecl b10_7176B0(uint32_t a1);
	uint32_t __cdecl b10_717710(uint32_t a1);
	uint32_t __cdecl b10_717750(uint32_t a1);
	uint32_t __cdecl b10_717770(uint32_t a1);
	uint32_t __cdecl b10_7177E0(uint32_t a1);
	uint32_t __cdecl b10_717800(uint32_t a1);
	uint32_t __cdecl b10_717860(uint32_t a1);
	uint32_t __cdecl b10_7178A0(uint32_t a1);
	uint32_t __cdecl b10_717910(uint32_t a1);
	uint32_t __cdecl b10_717930(uint32_t a1);
	uint32_t __cdecl b10_717980(uint32_t a1);
	uint32_t __cdecl b10_7179C0(uint32_t a1);
	uint32_t __cdecl b10_7179F0(uint32_t a1);
	uint32_t __cdecl b10_717A50(uint32_t a1, uint32_t a2);

	namespace
	{
		inline uint32_t b1_frame_ptr(uint32_t base, int16_t f)
		{
			if (f == 0)
				return base + 0xC;
			const int32_t n = mul32(S32(base, 4), (int32_t)f);
			return base + (uint32_t)shl32(n, 3) + 0xC;
		}
		inline void b1_copy16(uint32_t dst, uint32_t src)
		{
			for (int i = 0; i < 16; i += 4)
				U32(dst, i) = U32(src, i);
		}
		inline bool b1_out(int16_t v, int16_t lim)
		{
			return v < 0 || v > lim;
		}
	}

	uint32_t __cdecl b_729BD0(uint32_t a1)
	{
		set_mod_by_code(U32(a1, 8));

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(BG(BG_257B5BC)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(BG(BG_2575378)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = BF(BF_729D50);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = BF(BF_729D60);
		states[2] = BF(BF_729D70);
		states[3] = BF(BF_729DF0);
		states[4] = BF(BF_731CF0);
		states[5] = BF(BF_731D30);
		states[6] = BF(BF_731D40);
		states[7] = BF(BF_731D50);
		states[8] = BF(BF_731D80);
		states[9] = BF(BF_731D90);
		states[10] = BF(BF_731DB0);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(BG(BG_2579EBC));
			const uint32_t v2 = MEM<uint32_t>(BG(BG_2579544));
			MEM<uint32_t>(BG(BG_257526C)) = v1;
			MEM<uint32_t>(BG(BG_2575374)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(BG(BG_2579EB8));
			const uint32_t v2 = MEM<uint32_t>(BG(BG_2579540));
			MEM<uint32_t>(BG(BG_257526C)) = v1;
			MEM<uint32_t>(BG(BG_2575374)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(BG(BG_257B5A0)) = 0;
		MEM<uint16_t>(BG(BG_257B168)) = 0;
		const uint32_t queues[7] = { BG(BG_2579D48), BG(BG_2575350), BG(BG_257B5A8), BG(BG_257B158),
			BG(BG_2579D38), BG(BG_2579530), BG(BG_2575458) };
		for (int i = 0; i < 7; i++)
		{
			const uint32_t n = x::ExecuteTaskQueue(queues[i]);
			U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)n);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl b_72A350(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(BG(BG_152B5B8)), 0x54);
		const uint32_t p = MEM<uint32_t>(BG(BG_152B5B8));
		U16(p, 0) = 0;
		U16(p, 2) = 0;
		U16(p, 4) = 0x800;
		return a_73A720(p + 8);
	}

	uint32_t __cdecl b_718670(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(BG(BG_152B5B8)), 0x54);
		uint32_t p = MEM<uint32_t>(BG(BG_152B5B8));
		U16(p, 0) = 0;
		U16(p, 2) = 0;
		U16(p, 4) = 0x800;
		a_73A720(p + 8);
		p = MEM<uint32_t>(BG(BG_152B5B8));
		if (S16(p, 8) <= 0)
			U16(p, 8) = 0;
		return p;
	}

	uint32_t __cdecl b_71F500(uint32_t a1)
	{
		const uint32_t r = a_73B640(6);
		if (r == 0)
			return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl b_71F8E0(uint32_t a1)
	{
		U16(a1, 0x38) = (uint16_t)(U16(a1, 0x38) - 1);
		const int16_t t = S16(a1, 0x38);
		if (t > 0)
			return a1;
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x38) = 0;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return a1;
	}

	uint32_t __cdecl b_71F900(uint32_t a1)
	{
		const int16_t idx = S16(a1, 0x38);
		const uint32_t list = U32(a1, 0x34);
		const uint16_t w = U16(list, (int32_t)idx * 2);
		if (w == 0x7F)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
			U16(a1, 0x38) = (uint16_t)(idx + 1);
			return a1;
		}
		MEM<uint16_t>(game_addr(game::BATTLE_CAMERA_SHAKE_OFFSET_Y)) = w;
		U16(a1, 0x38) = (uint16_t)(idx + 1);
		return a1;
	}

	uint32_t __cdecl b_728A80(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		if (a_73B640(4) == 0)
			return 0;
		x::au_re_Battle_ReadAnimation_7(a1, 4);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b_72A540(uint32_t a1)
	{
		const uint32_t slot = x::BdSound_ClaimVoiceSlot(BG(BG_1683420), 1, 0x80);
		const uint32_t origin = MEM<uint32_t>(BG(BG_152B5B8));
		const uint32_t node = a1;
		U16(origin, 0x52) = (uint16_t)slot;
		a_73C100(node, BF(BF_72AB20), BG(BG_168C654), 0x12, 0x2D, 2);
		b_72A610(node, BF(BF_72A660), BG(BG_168F8BC), 0x4AC, 0, 0);
		x::Effect_AddTaskAndInitFromCtx(BG(BG_2575458), BF(BF_72FBC0), 0x70, node);
		x::Battle_QueueTIMUpload_GetEOF(BG(BG_1688434));
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b_72A610(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t node = x::Effect_AddTaskAndInitFromCtx(BG(BG_2579530), a2, 0x540, a1);
		U16(node, 0x80) = (uint16_t)a5;
		U32(node, 0x74) = a3;
		U32(node, 0x78) = (uint32_t)(int32_t)(int16_t)a4;
		U16(node, 0x82) = (uint16_t)a6;
		return node;
	}

	uint32_t __cdecl b_72A6C0(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		const uint32_t origin = MEM<uint32_t>(BG(BG_152B5B8));
		const uint32_t o0 = U32(origin, 0);
		const uint32_t o4 = U32(origin, 4);
		U32(node, 0x1C) = o0;
		U32(node, 0x20) = o4;
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		b_72A720(node);
		x::BdPlaySE(BG(BG_152B5BC), 0, 0x80);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b_72A720(uint32_t a1)
	{
		alignas(4) uint8_t blk[0x5C] = {};
		const uint32_t L = P(blk);
		const uint32_t node = a1;
		x::MAG_022_sub_8DD770(L);
		int16_t ang = S16(node, 0x62);
		if (ang != 0)
			x::MAG_022_sub_8DD8A0(L, (uint32_t)(int32_t)ang);
		ang = S16(node, 0x60);
		if (ang != 0)
			x::sub_8DD7E0(L, (uint32_t)(int32_t)ang);
		ang = S16(node, 0x64);
		if (ang != 0)
			x::sub_8DD960(L, (uint32_t)(int32_t)ang);
		x::scale3DMatrix(L, node + 0x50);
		const int32_t px = S16(node, 0x1C);
		const int32_t py = S16(node, 0x1E);
		const int32_t pz = S16(node, 0x20);
		S32(L, 0x14) = px;
		S32(L, 0x18) = py;
		S32(L, 0x1C) = pz;
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), L, L);
		U16(L, 0x54) = U16(node, 0x86);
		const uint32_t v70 = U32(node, 0x70);
		const uint32_t v7c = U32(node, 0x7C);
		U16(L, 0x4C) = U16(node, 0x8C);
		U32(L, 0x38) = v70;
		const uint16_t w90 = U16(node, 0x90);
		const uint16_t w92 = U16(node, 0x92);
		U32(L, 0x44) = v7c;
		const uint16_t w8e = U16(node, 0x8E);
		U16(L, 0x50) = w90;
		const uint32_t paused = MEM<uint32_t>(BG(BG_257802C));
		U16(L, 0x52) = w92;
		U16(L, 0x4E) = w8e;
		const uint16_t w80 = U16(node, 0x80);
		U32(L, 0x48) = BG(BG_257B1E0);
		U16(L, 0x56) = w80;
		return prim_play(node + 0x94, BF(BF_72A840), L, paused);
	}

	uint32_t __cdecl b_72A840(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		alignas(4) uint8_t loc[0x48] = {};
		const uint32_t L = P(loc);
		const uint32_t M = L + 0x28;
		const uint32_t rec = a2;
		if (((uint32_t)(int32_t)S16(rec, 0x1C) | U32(rec, 0x18)) == 0)
			return 0;
		if (S16(rec, 0x24) >= 0x1000 && U32(rec, 0x20) == 0)
			return 0;

		const uint32_t hdr = x::Field_Alloc(0x58);
		const int32_t model = S16(rec, 2);
		const uint32_t data = U32(a1, 0);
		uint32_t blk = a3;
		const uint32_t base = data + U32(data + (uint32_t)shl32(model, 2), 8);
		const int16_t f1 = S16(rec, 0x2A);
		const int16_t f0 = S16(rec, 0x28);
		U32(hdr, 0) = base;
		if (f0 == f1)
			U32(hdr, 4) = b1_frame_ptr(base, f0);
		else
		{
			const int16_t t = S16(rec, 0x26);
			if (t == 0)
				U32(hdr, 4) = b1_frame_ptr(base, f0);
			else if (t == 0x1000)
				U32(hdr, 4) = b1_frame_ptr(base, f1);
			else
			{
				const uint32_t out = U32(blk, 0x48);
				x::MAG_017_sub_701390(base, (uint32_t)(int32_t)f0, (uint32_t)(int32_t)f1, (uint32_t)(int32_t)t, out);
				blk = a3;
				U32(hdr, 4) = U32(blk, 0x48);
			}
		}

		if ((U32(rec, 4) & 0x40000) != 0)
			xm::MAG_063_sub_7015B0(rec + 0x10, M);
		else
			x::MAG_017_sub_701310(rec + 0x10, M);
		const uint16_t vx = U16(rec, 8);
		const uint16_t vy = U16(rec, 0xA);
		const uint16_t vz = U16(rec, 0xC);
		const bool raw_pos = (U8(rec, 5) & 2) != 0;
		U16(L, 0) = vx;
		U16(L, 2) = vy;
		U16(L, 4) = vz;
		int32_t tx, ty, tz;
		if (raw_pos)
		{
			tx = (int16_t)vx;
			ty = (int16_t)vy;
			tz = (int16_t)vz;
		}
		else
		{
			x::GTE_SetRotMatrix(blk);
			x::GTE_LoadV0(L);
			x::GTE_MVMVA_RotV0();
			x::GTE_ReadMAC123(M + 0x14);
			x::GTE_MatrixMultiply(blk, M);
			tz = S32(M, 0x1C);
			ty = S32(M, 0x18);
			tx = S32(M, 0x14);
		}
		S32(M, 0x14) = add32(tx, S32(blk, 0x14));
		S32(M, 0x18) = add32(ty, S32(blk, 0x18));
		const uint32_t sxy = U32(rec, 0x18);
		S32(M, 0x1C) = add32(tz, S32(blk, 0x1C));
		if (sxy != 0x10001000 || S16(rec, 0x1C) != 0x1000)
		{
			if ((U32(rec, 4) & 0x100) != 0)
			{
				U16(L, 0x08) = U16(rec, 0x18);
				U16(L, 0x10) = U16(rec, 0x1A);
				U16(L, 0x18) = U16(rec, 0x1C);
				U16(L, 0x12) = 0;
				U16(L, 0x0C) = 0;
				U16(L, 0x16) = 0;
				U16(L, 0x0A) = 0;
				U16(L, 0x14) = 0;
				U16(L, 0x0E) = 0;
				x::sub_56C220(M, L + 8);
			}
			else
			{
				S32(L, 0x08) = S16(rec, 0x18);
				S32(L, 0x0C) = S16(rec, 0x1A);
				S32(L, 0x10) = S16(rec, 0x1C);
				x::scale3DMatrix(M, L + 8);
			}
		}
		x::GTE_SetRotMatrix_W(M);
		x::GTE_SetTransVector_W(M);
		const uint32_t flags = U32(rec, 4);
		U32(hdr, 0x1C) = (flags & 0x4000) != 0 ? 0x2000u : 0x2030u;
		const int32_t alpha = S16(rec, 0x24);
		U32(hdr, 0xC) = (uint32_t)alpha;
		if (alpha != 0)
		{
			const uint32_t mode = U32(hdr, 0x1C);
			U32(hdr, 8) = U32(rec, 0x20);
			U32(hdr, 0x1C) = mode | 0xC0;
		}
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = x::Effect_RenderPrimModel(hdr, ot, 2, cursor);
		return x::Field_Free(0x58);
	}

	uint32_t __cdecl b_72AAF0(uint32_t a1)
	{
		const uint32_t r = b_72A720(a1);
		if (r != 0)
			return r;
		U8(a1, 0x26) |= 1;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b_72AD60(uint32_t a1)
	{
		const uint32_t arena = MEM<uint32_t>(BG(BG_257B5CC));
		const uint32_t v = arena + 0x1B4;
		U32(v, 0) = (uint32_t)shl32((int32_t)S16(a1, 0x290), 16);
		const int32_t y = S16(a1, 0x292);
		const int32_t z = S16(a1, 0x294);
		U32(arena, 0x1B8) = (uint32_t)shl32(y, 16);
		U32(arena, 0x1BC) = (uint32_t)shl32(z, 16);
		b1_copy16(arena + 0x204, v);
		b1_copy16(arena + 0x1F4, v);
		b1_copy16(arena + 0x1E4, v);
		b1_copy16(arena + 0x1D4, v);
		b1_copy16(arena + 0x1C4, v);
		return U32(v, 0xC);
	}

	uint32_t __cdecl b_72AFE0(void)
	{
		const uint32_t arena = MEM<uint32_t>(BG(BG_257B5CC));
		int32_t i = 0;
		if (S16(arena, 0x1C) <= 0)
			return arena;
		const uint32_t src = arena + 0x194;
		uint32_t dst = arena + 0x94;
		do
		{
			i++;
			b1_copy16(dst - 0x40, src);
			b1_copy16(dst, src);
			b1_copy16(dst + 0x40, src);
			b1_copy16(dst + 0x80, src);
			b1_copy16(dst + 0xC0, src);
			dst += 0x10;
		} while (i < (int32_t)S16(arena, 0x1C));
		return dst;
	}

	uint32_t __cdecl b_72BA00(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		const uint32_t cnt_ptr = U32(ctx, 0x20);
		const uint32_t count = U32(cnt_ptr, 0);
		uint32_t face = cnt_ptr + 4;
		U32(ctx, 0x20) = face;
		const uint32_t vbase = U32(ctx, 4);
		uint32_t cur = a4;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x20) = face;
			return cur;
		}
		uint32_t pk = cur + 0xC;
		uint32_t left = count;
		do
		{
			const uint32_t pcur = cur;
			const uint32_t v2 = vbase + (uint32_t)U16(face, 8) * 4;
			const uint32_t v1 = vbase + (uint32_t)U16(face, 6) * 4;
			const uint32_t v0 = vbase + (uint32_t)U16(face, 4) * 4;
			x::GTE_LoadV012(v0, v1, v2);
			x::GTE_RTPT();
			const uint32_t fl = U32(ctx, 0x1C);
			const uint32_t code = U32(face, 0);
			U32(pcur, 0) = 0x4000000;
			U32(pk, -8) = code;
			if ((fl & 1) != 0)
				U32(pk, -8) = code | 0x2000000;
			if ((fl & 4) != 0)
				U32(pk, -8) &= 0xFDFFFFFFu;
			x::GTE_ReadFLAG(ctx + 0x30);
			if ((U32(ctx, 0x30) & 0x60000) == 0)
			{
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x10) != 0)
				{
					x::GTE_ReadSXY012_Split(pk - 4, pk, pk + 4);
					x::GTE_AVSZ3();
					uint32_t clip = 0;
					if (b1_out(S16(pk, -4), 0xA00)) clip = 1;
					if (b1_out(S16(pk, 0), 0xA00)) clip |= 2;
					if (b1_out(S16(pk, 4), 0xA00)) clip |= 4;
					if (b1_out(S16(pk, -2), 0x6C0)) clip |= 0x10;
					if (b1_out(S16(pk, 2), 0x6C0)) clip |= 0x20;
					if (b1_out(S16(pk, 6), 0x6C0)) clip |= 0x40;
					if ((uint8_t)(clip & 7) != 7 && (uint8_t)(clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if ((U8(ctx, 0x1C) & 0x40) != 0)
						{
							x::set_unk_1CA8A28(pk - 8);
							x::set_dword_1CA8A30(U32(ctx, 0xC));
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pk - 8);
						}
						const int32_t otz = S32(ctx, 0x2C) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)shl32(otz, 2), pcur);
						cur = pcur + 0x14;
						pk += 0x14;
					}
				}
			}
			face += 0xC;
			left--;
		} while (left != 0);
		U32(ctx, 0x20) = face;
		return cur;
	}

	uint32_t __cdecl b_72BC10(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		const uint32_t cnt_ptr = U32(ctx, 0x20);
		const uint32_t count = U32(cnt_ptr, 0);
		uint32_t face = cnt_ptr + 4;
		U32(ctx, 0x20) = face;
		const uint32_t vbase = U32(ctx, 4);
		uint32_t cur = a4;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x20) = face;
			return cur;
		}
		uint32_t pk = cur + 0xC;
		uint32_t left = count;
		do
		{
			const uint32_t pcur = cur;
			const uint32_t v2 = vbase + (uint32_t)U16(face, 8) * 4;
			const uint32_t v1 = vbase + (uint32_t)U16(face, 6) * 4;
			const uint32_t v0 = vbase + (uint32_t)U16(face, 4) * 4;
			x::GTE_LoadV012(v0, v1, v2);
			x::GTE_RTPT();
			const uint32_t fl = U32(ctx, 0x1C);
			const uint32_t code = U32(face, 0);
			U32(pcur, 0) = 0x5000000;
			U32(pk, -8) = code;
			if ((fl & 1) != 0)
				U32(pk, -8) = code | 0x2000000;
			if ((fl & 4) != 0)
				U32(pk, -8) &= 0xFDFFFFFFu;
			x::GTE_ReadFLAG(ctx + 0x30);
			if ((U32(ctx, 0x30) & 0x60000) == 0)
			{
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x10) != 0)
				{
					x::GTE_ReadSXY012_Split(pk - 4, pk, pk + 4);
					const uint32_t v3 = vbase + (uint32_t)U16(face, 0xA) * 4;
					x::GTE_LoadV0(v3);
					x::GTE_RTPS();
					uint32_t clip = 0;
					if (b1_out(S16(pk, -4), 0xA00)) clip = 1;
					if (b1_out(S16(pk, 0), 0xA00)) clip |= 2;
					if (b1_out(S16(pk, 4), 0xA00)) clip |= 4;
					if (b1_out(S16(pk, -2), 0x6C0)) clip |= 0x10;
					if (b1_out(S16(pk, 2), 0x6C0)) clip |= 0x20;
					if (b1_out(S16(pk, 6), 0x6C0)) clip |= 0x40;
					x::GTE_ReadSXY2(pk + 8);
					x::GTE_AVSZ4();
					if (b1_out(S16(pk, 8), 0xA00)) clip |= 8;
					if (b1_out(S16(pk, 0xA), 0x6C0)) clip |= 0x80;
					if ((uint8_t)(clip & 0xF) != 0xF && (uint8_t)(clip & 0xF0) != 0xF0)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if ((U8(ctx, 0x1C) & 0x40) != 0)
						{
							x::set_unk_1CA8A28(pk - 8);
							x::set_dword_1CA8A30(U32(ctx, 0xC));
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pk - 8);
						}
						const int32_t otz = S32(ctx, 0x2C) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)shl32(otz, 2), pcur);
						cur = pcur + 0x18;
						pk += 0x18;
					}
				}
			}
			face += 0xC;
			left--;
		} while (left != 0);
		U32(ctx, 0x20) = face;
		return cur;
	}

	namespace
	{
		inline bool b2_out_x(int16_t v) { return v < 0 || v > 0xA00; }
		inline bool b2_out_y(int16_t v) { return v < 0 || v > 0x6C0; }
		inline int32_t b2_fx(uint32_t v)
		{
			int32_t s = (int32_t)v;
			return add32(s, (s >> 31) & 0xFFFF) >> 16;
		}
	}

	uint32_t __cdecl b_72C370(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t cursor = a4;
		uint32_t list = U32(ctx, 0x20);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x20) = rec;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x20) = rec;
			return a4;
		}
		uint32_t pkt = cursor;
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t flags = U32(ctx, 0x1C);
			uint32_t code = U32(rec, 0);
			U32(cursor, 0) = 0x6000000;
			U32(pkt, 4) = code;
			if (flags & 2)
			{
				code |= 0x2000000;
				U32(pkt, 4) = code;
			}
			if (flags & 8)
				U32(pkt, 4) &= 0xFDFFFFFF;
			x::GTE_ReadFLAG(ctx + 0x30);
			if (!(U32(ctx, 0x30) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					x::GTE_AVSZ3();
					if (b2_out_x(S16(pkt, 8))) clip = 1;
					if (b2_out_x(S16(pkt, 0x10))) clip |= 2;
					if (b2_out_x(S16(pkt, 0x18))) clip |= 4;
					if (b2_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (b2_out_y(S16(pkt, 0x12))) clip |= 0x20;
					if (b2_out_y(S16(pkt, 0x1A))) clip |= 0x40;
					if ((uint8_t)(clip & 7) != 7 && (uint8_t)(clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if (U8(ctx, 0x1C) & 0x80)
						{
							x::sub_45E120(rec + 0x0C, rec + 0x10, pkt + 4);
							x::set_dword_1CA8A30(U32(ctx, 0x0C));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0x0C, pkt + 0x14, pkt + 4);
						}
						else
						{
							uint32_t c1 = U32(rec, 0x0C), c2 = U32(rec, 0x10);
							U32(pkt, 0x0C) = c1;
							U32(pkt, 0x14) = c2;
						}
						int32_t z = S32(ctx, 0x2C) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)z * 4, cursor);
						cursor += 0x1C;
						pkt += 0x1C;
					}
				}
			}
			rec += 0x14;
		} while (--count);
		U32(ctx, 0x20) = rec;
		return cursor;
	}

	uint32_t __cdecl b_72C5A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t cursor = a4;
		uint32_t list = U32(ctx, 0x20);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x20) = rec;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x20) = rec;
			return a4;
		}
		uint32_t pkt = cursor;
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t flags = U32(ctx, 0x1C);
			uint32_t code = U32(rec, 0);
			U32(cursor, 0) = 0x8000000;
			U32(pkt, 4) = code;
			if (flags & 2)
			{
				code |= 0x2000000;
				U32(pkt, 4) = code;
			}
			if (flags & 8)
				U32(pkt, 4) &= 0xFDFFFFFF;
			x::GTE_ReadFLAG(ctx + 0x30);
			if (!(U32(ctx, 0x30) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0x0A) * 4);
					x::GTE_RTPS();
					if (b2_out_x(S16(pkt, 8))) clip = 1;
					if (b2_out_x(S16(pkt, 0x10))) clip |= 2;
					if (b2_out_x(S16(pkt, 0x18))) clip |= 4;
					if (b2_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (b2_out_y(S16(pkt, 0x12))) clip |= 0x20;
					if (b2_out_y(S16(pkt, 0x1A))) clip |= 0x40;
					x::GTE_ReadSXY2(pkt + 0x20);
					x::GTE_AVSZ4();
					if (b2_out_x(S16(pkt, 0x20))) clip |= 8;
					if (b2_out_y(S16(pkt, 0x22))) clip |= 0x80;
					if ((uint8_t)(clip & 0xF) != 0xF && (uint8_t)(clip & 0xF0) != 0xF0)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if (U8(ctx, 0x1C) & 0x80)
						{
							x::sub_45E120(rec + 0x0C, rec + 0x10, rec + 0x14);
							x::set_dword_1CA8A30(U32(ctx, 0x0C));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0x0C, pkt + 0x14, pkt + 0x1C);
							x::set_unk_1CA8A28(pkt + 4);
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pkt + 4);
						}
						else
						{
							uint32_t c1 = U32(rec, 0x0C), c2 = U32(rec, 0x10), c3 = U32(rec, 0x14);
							U32(pkt, 0x0C) = c1;
							U32(pkt, 0x14) = c2;
							U32(pkt, 0x1C) = c3;
						}
						int32_t z = S32(ctx, 0x2C) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)z * 4, cursor);
						cursor += 0x24;
						pkt += 0x24;
					}
				}
			}
			rec += 0x18;
		} while (--count);
		U32(ctx, 0x20) = rec;
		return cursor;
	}

	uint32_t __cdecl b_72C860(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t list = U32(ctx, 0x20);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x20) = rec;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x20) = rec;
			return pkt;
		}
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t flags = U32(ctx, 0x1C);
			uint32_t code = U32(rec, 0);
			U32(pkt, 0) = 0x9000000;
			U32(pkt, 4) = code;
			if (flags & 2)
			{
				code |= 0x2000000;
				U32(pkt, 4) = code;
			}
			if (flags & 8)
				U32(pkt, 4) &= 0xFDFFFFFF;
			uint32_t toff = U32(ctx, 0x18);
			uint32_t uv0 = U32(rec, 0x0C);
			uint32_t uv1 = U32(rec, 0x10);
			U32(pkt, 0x0C) = uv0 + toff;
			uint32_t uv2 = U32(rec, 8) >> 16;
			U32(pkt, 0x18) = uv1 + toff;
			U32(pkt, 0x24) = uv2 + toff;
			x::GTE_ReadFLAG(ctx + 0x30);
			if (!(U32(ctx, 0x30) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t f2 = U32(ctx, 0x1C);
				if (f2 & 0x400)
					U16(pkt, 0x1A) = (uint16_t)(U16(pkt, 0x1A) + U16(ctx, 0x10));
				else if (f2 & 0x100)
					U16(pkt, 0x1A) = U16(ctx, 0x10);
				if (f2 & 0x800)
					U16(pkt, 0x0E) = (uint16_t)(U16(pkt, 0x0E) + U16(ctx, 0x14));
				else if (f2 & 0x200)
					U16(pkt, 0x0E) = U16(ctx, 0x14);
				uint32_t zero = 0;
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x14, pkt + 0x20);
					x::GTE_AVSZ3();
					uint32_t clip = b2_out_x(S16(pkt, 8)) ? 1 : zero;
					if (b2_out_x(S16(pkt, 0x14))) clip |= 2;
					if (b2_out_x(S16(pkt, 0x20))) clip |= 4;
					if (b2_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (b2_out_y(S16(pkt, 0x16))) clip |= 0x20;
					if (b2_out_y(S16(pkt, 0x22))) clip |= 0x40;
					if ((uint8_t)(clip & 7) != 7 && (uint8_t)(clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if (U8(ctx, 0x1C) & 0x80)
						{
							x::sub_45E120(rec + 0x14, rec + 0x18, pkt + 4);
							x::set_dword_1CA8A30(U32(ctx, 0x0C));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0x10, pkt + 0x1C, pkt + 4);
						}
						else
						{
							uint32_t c1 = U32(rec, 0x14), c2 = U32(rec, 0x18);
							U32(pkt, 0x10) = c1;
							U32(pkt, 0x1C) = c2;
						}
						int32_t z = S32(ctx, 0x2C) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)z * 4, pkt);
						pkt += 0x28;
					}
				}
			}
			rec += 0x1C;
		} while (--count);
		U32(ctx, 0x20) = rec;
		return pkt;
	}

	uint32_t __cdecl b_72CAC0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t list = U32(ctx, 0x20);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x20) = rec;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x20) = rec;
			return pkt;
		}
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t flags = U32(ctx, 0x1C);
			uint32_t code = U32(rec, 0);
			U32(pkt, 0) = 0xC000000;
			U32(pkt, 4) = code;
			if (flags & 2)
			{
				code |= 0x2000000;
				U32(pkt, 4) = code;
			}
			if (flags & 8)
				U32(pkt, 4) &= 0xFDFFFFFF;
			uint32_t toff = U32(ctx, 0x18);
			uint32_t uv0 = U32(rec, 0x0C);
			uint32_t uv1 = U32(rec, 0x10);
			U32(pkt, 0x0C) = uv0 + toff;
			uint32_t w23 = (toff << 16) + toff;
			uv1 += toff;
			w23 += U32(rec, 0x14);
			U32(pkt, 0x24) = w23;
			U32(pkt, 0x18) = uv1;
			U32(pkt, 0x30) = w23 >> 16;
			x::GTE_ReadFLAG(ctx + 0x30);
			if (!(U32(ctx, 0x30) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t f2 = U32(ctx, 0x1C);
				if (f2 & 0x400)
					U16(pkt, 0x1A) = (uint16_t)(U16(pkt, 0x1A) + U16(ctx, 0x10));
				else if (f2 & 0x100)
					U16(pkt, 0x1A) = U16(ctx, 0x10);
				if (f2 & 0x800)
					U16(pkt, 0x0E) = (uint16_t)(U16(pkt, 0x0E) + U16(ctx, 0x14));
				else if (f2 & 0x200)
					U16(pkt, 0x0E) = U16(ctx, 0x14);
				uint32_t zero = 0;
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x14, pkt + 0x20);
					x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0x0A) * 4);
					x::GTE_RTPS();
					uint32_t clip = b2_out_x(S16(pkt, 8)) ? 1 : zero;
					if (b2_out_x(S16(pkt, 0x14))) clip |= 2;
					if (b2_out_x(S16(pkt, 0x20))) clip |= 4;
					if (b2_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (b2_out_y(S16(pkt, 0x16))) clip |= 0x20;
					if (b2_out_y(S16(pkt, 0x22))) clip |= 0x40;
					x::GTE_ReadSXY2(pkt + 0x2C);
					x::GTE_AVSZ4();
					if (b2_out_x(S16(pkt, 0x2C))) clip |= 8;
					if (b2_out_y(S16(pkt, 0x2E))) clip |= 0x80;
					if ((uint8_t)(clip & 0xF) != 0xF && (uint8_t)(clip & 0xF0) != 0xF0)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if (U8(ctx, 0x1C) & 0x80)
						{
							x::sub_45E120(rec + 0x18, rec + 0x1C, rec + 0x20);
							x::set_dword_1CA8A30(U32(ctx, 0x0C));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0x10, pkt + 0x1C, pkt + 0x28);
							x::set_unk_1CA8A28(pkt + 4);
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pkt + 4);
						}
						else
						{
							uint32_t c1 = U32(rec, 0x18), c2 = U32(rec, 0x1C), c3 = U32(rec, 0x20);
							U32(pkt, 0x10) = c1;
							U32(pkt, 0x1C) = c2;
							U32(pkt, 0x28) = c3;
						}
						int32_t z = S32(ctx, 0x2C) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)z * 4, pkt);
						pkt += 0x34;
					}
				}
			}
			rec += 0x24;
		} while (--count);
		U32(ctx, 0x20) = rec;
		return pkt;
	}

	uint32_t __cdecl b_72E220(uint32_t a1, uint32_t a2)
	{
		const uint32_t data = a2;
		int32_t frame = S16(a1, 0x1CA);
		uint32_t i_dst = U8(U32(data, 0x12C), frame);
		uint32_t st = U32(BG(BG_257B5CC), 0);
		uint32_t dst_tab = U32(st, 0x22C);
		uint32_t src_tab = U32(st, 0x230);
		uint32_t dst = U32(dst_tab, i_dst * 4);
		uint32_t i_src1 = U8(U32(data, 0x130), frame);
		uint32_t src1 = U32(src_tab, i_src1 * 4);
		uint32_t i_src2 = U8(U32(data, 0x134), frame);
		uint32_t src2 = U32(src_tab, i_src2 * 4);
		if (src1 == 0 || src2 == 0)
			return 0;
		uint32_t w_blk = x::Field_Alloc(8);
		int32_t frame2 = S16(a1, 0x1CA);
		uint32_t n = U32(dst, 4);
		int32_t w = S16(U32(data, 0x138), frame2 * 2);
		U32(w_blk, 4) = (uint32_t)w;
		U32(w_blk, 0) = (uint32_t)sub32(0x1000, w);
		if ((int32_t)n > 0)
		{
			uint32_t out = dst + 8;
			uint32_t in2 = src2 + 8;
			uint32_t delta = src1 - src2;
			do
			{
				x::set_dword_1CA8A30(U32(w_blk, 0));
				xm::sub_45E0B0(in2 + delta);
				xm::sub_45E9D0();
				x::set_dword_1CA8A30(U32(w_blk, 4));
				xm::sub_45E0B0(in2);
				xm::sub_45EBF0();
				x::GTE_StoreIR123(out);
				out += 8;
				in2 += 8;
			} while (--n != 0);
		}
		x::Field_Free(8);
		return 0;
	}

	uint32_t __cdecl b_72EF80(uint32_t a1)
	{
		const uint32_t act = a1;
		const uint32_t st = U32(BG(BG_257B5CC), 0);
		int32_t aidx = S8(act, 0x6A);
		int32_t key = S16(act, 0x60);
		const uint32_t data = U32(U32(st, 0x224), aidx * 4);
		uint8_t sel = U8(U32(data, 0x108), key);
		uint8_t kind = U8(U32(data, 0x100), key);
		int16_t ax = 0, ay = 0, az = 0;
		uint32_t base = 0;
		bool have = false;
		if (kind == 0)
		{
			if (sel <= 3)
			{
				static const uint32_t offs0[4] = { 0x94, 0xD4, 0x114, 0x154 };
				base = st + (uint32_t)((int32_t)S8(act, 0x6B) << 4) + offs0[sel];
				have = true;
			}
		}
		else if (kind == 1)
		{
			if (sel <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				base = st + offs1[sel];
				have = true;
			}
		}
		if (have)
		{
			int32_t vx = b2_fx(U32(base, 0));
			int32_t vy = b2_fx(U32(base, 4));
			ay = (int16_t)vy;
			int32_t vz = b2_fx(U32(base, 8));
			az = (int16_t)vz;
			ax = (int16_t)vx;
		}
		int16_t v[4];
		int16_t ir[8] = {};
		v[0] = (int16_t)U16(U32(data, 0x110), key * 2);
		v[1] = (int16_t)U16(U32(data, 0x114), key * 2);
		v[2] = (int16_t)U16(U32(data, 0x118), key * 2);
		v[3] = 0;
		x::GTE_SetRotMatrix(act + 0x2C);
		x::GTE_LoadV0(P(v));
		x::GTE_MVMVA_RotV0();
		x::GTE_StoreIR123(P(ir));
		az = (int16_t)(az + ir[2]);
		ay = (int16_t)(ay + ir[1]);
		const int32_t key2 = S16(act, 0x60);
		uint8_t kind2 = U8(U32(data, 0x104), key2);
		uint8_t sel2 = U8(U32(data, 0x10C), key2);
		ax = (int16_t)(ax + ir[0]);
		int16_t bx, by, bz;
		uint32_t base2 = 0;
		bool have2 = false;
		if (kind2 == 0)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs0[4] = { 0x94, 0xD4, 0x114, 0x154 };
				uint32_t g = U32(BG(BG_257B5CC), 0);
				base2 = (uint32_t)((int32_t)S8(act, 0x6B) << 4) + g + offs0[sel2];
				have2 = true;
			}
		}
		else if (kind2 == 1)
		{
			if (sel2 <= 3)
			{
				static const uint32_t offs1[4] = { 0x1D4, 0x1E4, 0x1F4, 0x204 };
				uint32_t g = U32(BG(BG_257B5CC), 0);
				base2 = g + offs1[sel2];
				have2 = true;
			}
		}
		if (have2)
		{
			int32_t vx = b2_fx(U32(base2, 0));
			int32_t vy = b2_fx(U32(base2, 4));
			int32_t vz = b2_fx(U32(base2, 8));
			bx = (int16_t)vx;
			by = (int16_t)vy;
			bz = (int16_t)vz;
		}
		else
		{
			bz = ir[2];
			by = ir[1];
			bx = ir[0];
		}
		v[0] = (int16_t)U16(U32(data, 0x11C), key2 * 2);
		v[1] = (int16_t)U16(U32(data, 0x120), key2 * 2);
		v[2] = (int16_t)U16(U32(data, 0x124), key2 * 2);
		x::GTE_SetRotMatrix(act + 0x2C);
		x::GTE_LoadV0(P(v));
		x::GTE_MVMVA_RotV0();
		x::GTE_StoreIR123(P(ir));
		bx = (int16_t)(bx + ir[0]);
		by = (int16_t)(by + ir[1]);
		const int32_t key3 = S16(act, 0x60);
		const uint32_t ttab = U32(data, 0x128);
		bz = (int16_t)(bz + ir[2]);
		const int32_t t = S16(ttab, key3 * 2);
		int32_t a = ax;
		S32(act, 0x4C) = shl32(add32(mul32(sub32(bx, a), t), shl32(a, 12)), 4);
		a = ay;
		S32(act, 0x50) = shl32(add32(mul32(sub32(by, a), t), shl32(a, 12)), 4);
		a = az;
		S32(act, 0x54) = shl32(add32(mul32(sub32(bz, a), t), shl32(a, 12)), 4);
		return 0;
	}

	namespace
	{
		inline uint32_t b3_task_tail(uint32_t node)
		{
			const uint8_t status = U8(node, 0x26);
			U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
			if ((status & 1) != 0 && U8(node, 0x28) == 0)
			{
				x::Effect_ReleaseLinkedTask(node);
				return 2;
			}
			return 0;
		}

		inline void b3_next_state(uint32_t node)
		{
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}

		inline void b3_write_fade_words(uint16_t v)
		{
			uint32_t p = game_addr(game::unk_1D98992);
			for (int k = 4; k != 0; --k)
			{
				MEM<uint16_t>(p) = v;
				p += 0x2C;
			}
		}

		inline uint32_t b3_section(uint32_t base, int32_t off)
		{
			return (uint32_t)(base + ((uint32_t)(off / 4) << 2));
		}
	}

	uint32_t __cdecl b_72FC40(uint32_t a1)
	{
		a_73AB00();
		const uint32_t origin = MEM<uint32_t>(BG(BG_152B5B8));
		a_73AB20(BG(BG_152BA38), origin, 0);
		b3_next_state(a1);
		return a1;
	}

	uint32_t __cdecl b_730460(uint32_t a1)
	{
		const uint32_t r = a_73AAE0(2);
		if (r == 0)
			return 0;
		const uint32_t origin = MEM<uint32_t>(BG(BG_152B5B8));
		a_73AB20(BG(BG_152BA80), origin, 0);
		b3_next_state(a1);
		return a1;
	}

	uint32_t __cdecl b_730510(uint32_t a1)
	{
		a_73AE10();
		const uint32_t r = a_73AAE0(6);
		if (r == 0)
			return 0;
		b3_next_state(a1);
		return a1;
	}

	uint32_t __cdecl b_7305D0(uint32_t a1)
	{
		const uint32_t ctx = MEM<uint32_t>(BG(BG_257AFB8));
		if (U16(ctx, 0x166) == 0)
		{
			a_73BB60(0);
			b3_next_state(a1);
			return a1;
		}
		if (S16(a1, 0x24) >= 0x1B)
			return a_73BB60(1);
		return ctx;
	}

	uint32_t __cdecl b_730660(uint32_t a1)
	{
		if (S16(a1, 0x24) < 0x1C)
			return 0;
		const uint32_t task = x::Effect_AddTaskAndInitFromCtx(BG(BG_257B158), BF(BF_7306A0), 0x140, a1);
		x::Effect_BindModelContainerSetAnim(task, BG(BG_168354C), 1);
		b3_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b_7306A0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[10];
		states[0] = BF(BF_730B00);
		states[1] = BF(BF_730BA0);
		states[2] = BF(BF_730BD0);
		states[3] = BF(BF_730E30);
		states[4] = BF(BF_730EA0);
		states[5] = BF(BF_730EF0);
		states[6] = BF(BF_730F10);
		states[7] = BF(BF_730F50);
		states[8] = BF(BF_730F80);
		states[9] = BF(BF_730FB0);
		callp(states[S8(node, 0x29)], node);
		if ((U8(node, 0x26) & 4) == 0)
		{
			const uint32_t cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = a_746C10(node, BG(BG_2579EC8), cursor);
			a_739890(node);
		}
		return b3_task_tail(node);
	}

	uint32_t __cdecl b_730B00(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t origin = MEM<uint32_t>(BG(BG_152B5B8));
		const uint32_t xy = U32(origin, 0);
		const uint32_t zw = U32(origin, 4);
		U32(node, 0x4C) = xy;
		U32(node, 0x50) = zw;
		U32(node, 0x60) = node + 0x114;
		U32(node, 0x11C) = 0x1800;
		U32(node, 0x118) = 0x1800;
		U32(node, 0x114) = 0x1800;
		x::au_re_Battle_ReadAnimation_7(node, 0);
		a_739AC0(node, BG(BG_152BB58), 0);
		b3_next_state(node);
		return 0;
	}

	uint32_t __cdecl b_730BD0(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		if (S16(node, 0x136) < 0)
			return 0;
		x::au_re_Battle_ReadAnimation_7(node, 2);
		x::Effect_AddTaskAndInitFromCtx(BG(BG_2575458), BF(BF_730C10), 0x70, node);
		b3_next_state(node);
		return 0;
	}

	uint32_t __cdecl b_730C10(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[6];
		states[0] = BF(BF_730D00);
		states[1] = BF(BF_730D50);
		states[2] = BF(BF_730D70);
		states[3] = BF(BF_730DD0);
		states[4] = BF(BF_730E00);
		states[5] = BF(BF_730E20);
		callp(states[S8(node, 0x29)], node);
		b_730C90(node, 0x800);
		return b3_task_tail(node);
	}

	uint32_t __cdecl b_730C90(uint32_t a1, uint32_t a2)
	{
		const uint32_t node = a1;
		if ((U8(node, 0x26) & 4) != 0)
			return 0;
		xm::TransformCameraByShadowRotation(node + 0x1C, a2, (uint32_t)(int32_t)S16(node, 0x54));
		const uint32_t blk = x::Field_Alloc(0xB4);
		const uint32_t seq = U32(node, 0x4C);
		const uint16_t frame = U16(node, 0x50);
		U32(blk, 0) = seq;
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		U16(blk, 4) = frame;
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(blk, 0x24) = 0;
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = xm::InitEffectSequenceFromData(blk, ot, 2, cursor);
		return x::Field_Free(0xB4);
	}

	uint32_t __cdecl b_730D00(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t origin = MEM<uint32_t>(BG(BG_152B5B8));
		const uint32_t xy = U32(origin, 0);
		const uint32_t zw = U32(origin, 4);
		U16(node, 0x26) = (uint16_t)(U16(node, 0x26) | 4);
		U32(node, 0x1C) = xy;
		U32(node, 0x20) = zw;
		const uint8_t st = U8(node, 0x29);
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + 0xFD00);
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + 0xFD80);
		U32(node, 0x4C) = BG(BG_152B5C8);
		U16(node, 0x52) = 0x13;
		U16(node, 0x54) = 0xFF00;
		U16(node, 0x6C) = 4;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl b_730D50(uint32_t a1)
	{
		U16(a1, 0x6C) = (uint16_t)(U16(a1, 0x6C) - 1);
		if (S16(a1, 0x6C) <= 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) & 0xFB);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return a1;
	}

	uint32_t __cdecl b_730D70(uint32_t a1)
	{
		const uint32_t node = a1;
		a_743C20(node);
		if (S16(node, 0x50) > 8)
		{
			U16(node, 0x6C) = 2;
			b3_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl b_730DD0(uint32_t a1)
	{
		const uint32_t node = a1;
		a_743C20(node);
		if (S16(node, 0x50) > 9)
			U16(node, 0x50) = 8;
		U16(node, 0x6C) = (uint16_t)(U16(node, 0x6C) - 1);
		if (S16(node, 0x6C) <= 0)
			b3_next_state(node);
		return 0;
	}

	uint32_t __cdecl b_730E30(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t r = x::au_re_Battle_ReadAnimation_8(node);
		if (r != 1)
			return r;
		r = a_73B7E0(r);
		if (r == 0)
			return 0;
		x::au_re_Battle_ReadAnimation_7(node, 3);
		x::BdPlaySE(BG(BG_152B5C0), 0, 0x80);
		b3_next_state(node);
		return 0;
	}

	uint32_t __cdecl b_730EA0(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		const uint32_t r = a_73B640(2);
		if (r != 0)
			b3_next_state(node);
		return r;
	}

	uint32_t __cdecl b_730EF0(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		if (S16(node, 0x136) >= 0x32)
			b3_next_state(node);
		return 0;
	}

	uint32_t __cdecl b_730F50(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t r = x::au_re_Battle_ReadAnimation_8(node);
		if (r != 1)
			return r;
		x::au_re_Battle_ReadAnimation_7(node, 5);
		b3_next_state(node);
		return 0;
	}

	uint32_t __cdecl b_730FE0(uint32_t a1)
	{
		const uint32_t ctx = MEM<uint32_t>(BG(BG_257AFB8));
		if (U16(ctx, 0x166) != 0)
			return ctx;
		const uint32_t r = a_73B640(1);
		if (r == 0)
			return 0;
		b3_next_state(a1);
		return a1;
	}

	uint32_t __cdecl b_731090(uint32_t a1)
	{
		const uint32_t r = a_73B7E0(4);
		if (r == 0)
			return 0;
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x44) = 0x10;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return a1;
	}

	uint32_t __cdecl b_731520(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		const uint32_t origin = MEM<uint32_t>(BG(BG_152B5B8));
		const uint32_t xy = U32(origin, 8);
		const uint32_t zw = U32(origin, 0xC);
		U32(node, 0x1C) = xy;
		U32(node, 0x20) = zw;
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		b_72A720(node);
		b3_next_state(node);
		return 0;
	}

	uint32_t __cdecl b_7315D0(uint32_t a1)
	{
		U16(a1, 0x44) = (uint16_t)(U16(a1, 0x44) - 1);
		if (S16(a1, 0x44) <= 0x12)
		{
			const uint32_t origin = MEM<uint32_t>(BG(BG_152B5B8));
			U16(origin, 0x48) = 1;
			b3_next_state(a1);
		}
		return a1;
	}

	uint32_t __cdecl b_731690(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x1C) = 0xC00;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return a1;
	}

	uint32_t __cdecl b_7316B0(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) - 0x80);
		if (S16(a1, 0x1C) <= 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U16(a1, 0x1C) = 0;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		b3_write_fade_words(level);
		return 0;
	}

	uint32_t __cdecl b_731720(uint32_t a1)
	{
		const uint32_t r = a_73B640(6);
		if (r == 0)
			return 0;
		const uint32_t origin = MEM<uint32_t>(BG(BG_152B5B8));
		x::sub_4A2940((uint32_t)(int32_t)S16(origin, 0x52));
		const uint8_t st = U8(a1, 0x29);
		U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return a1;
	}

	uint32_t __cdecl b_731840(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = BF(BF_7318D0);
		states[1] = BF(BF_731920);
		states[2] = BF(BF_731940);
		callp(states[S8(node, 0x29)], node);
		uint32_t data = U32(node, 0x74);
		a_7340A0(b3_section(data, S32(data, 8)), 0xC);
		data = U32(node, 0x74);
		a_7340A0(b3_section(data, S32(data, 0x14)), 0xC);
		return b3_task_tail(node);
	}

	uint32_t __cdecl b_7318D0(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		const uint32_t origin = MEM<uint32_t>(BG(BG_152B5B8));
		const uint32_t xy = U32(origin, 0);
		const uint32_t zw = U32(origin, 4);
		U32(node, 0x1C) = xy;
		U32(node, 0x20) = zw;
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + 0xFE80);
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		b_72A720(node);
		b3_next_state(node);
		return 0;
	}

	uint32_t __cdecl b_731950(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = BF(BF_7319D0);
		states[1] = BF(BF_731A20);
		states[2] = BF(BF_731A40);
		callp(states[S8(node, 0x29)], node);
		const uint32_t data = U32(node, 0x74);
		a_7340A0(b3_section(data, S32(data, 0x10)), 0x12);
		return b3_task_tail(node);
	}

	uint32_t __cdecl b_7319D0(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		const uint32_t origin = MEM<uint32_t>(BG(BG_152B5B8));
		const uint32_t xy = U32(origin, 0);
		const uint32_t zw = U32(origin, 4);
		U32(node, 0x1C) = xy;
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		const uint8_t st = U8(node, 0x29);
		U32(node, 0x20) = zw;
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + 0xFC00);
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + 0xFE80);
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl b_731AF0(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0x80);
		if (S16(a1, 0x1C) >= 0x600)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U16(a1, 0x1C) = 0x600;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		b3_write_fade_words(level);
		return 0;
	}

	uint32_t __cdecl b_731BE0(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0x100);
		if (S16(a1, 0x1C) >= 0xC00)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U16(a1, 0x1C) = 0xC00;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		b3_write_fade_words(level);
		return 0;
	}

	namespace
	{
		inline uint32_t b71_task_end(uint32_t node, uint8_t status)
		{
			if ((status & 1) != 0 && U8(node, 0x28) == 0)
			{
				x::Effect_ReleaseLinkedTask(node);
				return 2;
			}
			return 0;
		}
		inline void b71_next_state(uint32_t node)
		{
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		inline uint32_t b71_section(uint32_t data, int32_t off)
		{
			const int32_t v = S32(data, off);
			return data + (uint32_t)shl32(v / 4, 2);
		}
		void b71_build_rot(uint32_t node, int32_t sub, uint32_t parent)
		{
			const uint32_t m = node + 0x8C;
			if ((uint32_t)sub > 3)
				return;
			if (sub == 0 || sub == 1)
			{
				if (parent != 0)
					ff8fx::copy<(8 * 4)>((void *)m, (const void *)(parent + 0x2C));
				else
					x::MAG_022_sub_8DD770(m);
			}
			else
			{
				x::UnpackRotationMatrix(MEM<uint32_t>(game_addr(game::MAG_097_CameraMatrixPtr)), m);
			}
			if (sub == 0 || sub == 2)
			{
				if (U16(node, 0xD0) != 0)
					x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0xD0));
				if (U16(node, 0xCC) != 0)
					x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(node, 0xCC));
				if (U16(node, 0xCE) != 0)
					x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(node, 0xCE));
			}
			else
			{
				if (U16(node, 0xCE) != 0)
					x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(node, 0xCE));
				if (U16(node, 0xCC) != 0)
					x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(node, 0xCC));
				if (U16(node, 0xD0) != 0)
					x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0xD0));
			}
		}
		void b71_world_to_view(uint32_t node, uint32_t src, uint32_t cam)
		{
			U32(node, 0xA0) = (uint32_t)(S32(src, 0) / 65536);
			U32(node, 0xA4) = (uint32_t)(S32(src, 4) / 65536);
			U32(node, 0xA8) = (uint32_t)(S32(src, 8) / 65536);
			x::GTE_SetRotMatrix(cam);
			x::GTE_LoadIRFromMatrixColumn(node + 0x8C);
			x::GTE_MVMVA_RotIR();
			x::GTE_StoreIRToMatrixColumn(node + 0xAC);
			x::GTE_LoadIRFromMatrixColumn(node + 0x8E);
			x::GTE_MVMVA_RotIR();
			x::GTE_StoreIRToMatrixColumn(node + 0xAE);
			x::GTE_LoadIRFromMatrixColumn(node + 0x90);
			x::GTE_MVMVA_RotIR();
			x::GTE_StoreIRToMatrixColumn(node + 0xB0);
			x::GTE_SetTransVector(MEM<uint32_t>(game_addr(game::MAG_097_CameraMatrixPtr)));
			x::GTE_LoadV0FromDwords(node + 0xA0);
			x::GTE_MVMVA_RotV0_Tr();
			x::GTE_ReadMAC123(node + 0xC0);
		}
		inline void b71_scroll3(uint32_t r, uint32_t d)
		{
			const uint32_t w10 = U32(r, 0x10);
			const uint32_t w0C = U32(r, 0x0C);
			const uint32_t w08 = U32(r, 0x08);
			uint32_t v0 = (w0C & 0x7F) + d;
			uint32_t v1 = (w10 & 0x7F) + d;
			uint32_t v2 = ((w08 >> 16) & 0x7F) + d;
			if (v0 > 0x7F || v1 > 0x7F || v2 > 0x7F)
			{
				v0 -= 0x40;
				v1 -= 0x40;
				v2 -= 0x40;
			}
			U32(r, 0x0C) = (w0C & 0xFFFFFF00u) | (v0 & 0x7F);
			U32(r, 0x10) = (w10 & 0xFFFFFF00u) | (v1 & 0x7F);
			U32(r, 0x08) = (w08 & 0xFF00FFFFu) | ((v2 & 0x7F) << 16);
		}
		inline void b71_scroll4(uint32_t r, uint32_t d)
		{
			const uint32_t w0C = U32(r, 0x0C);
			const uint32_t w10 = U32(r, 0x10);
			const uint32_t w14 = U32(r, 0x14);
			uint32_t v0 = (w0C & 0x7F) + d;
			uint32_t v1 = (w10 & 0x7F) + d;
			uint32_t v2 = (w14 & 0x7F) + d;
			uint32_t v3 = ((w14 >> 16) & 0x7F) + d;
			if (v0 > 0x7F || v1 > 0x7F || v2 > 0x7F || v3 > 0x7F)
			{
				v0 -= 0x40;
				v1 -= 0x40;
				v2 -= 0x40;
				v3 -= 0x40;
			}
			U32(r, 0x10) = (w10 & 0xFFFFFF00u) | (v1 & 0x7F);
			U32(r, 0x0C) = (w0C & 0xFFFFFF00u) | (v0 & 0x7F);
			U32(r, 0x14) = (w14 & 0xFF00FF00u) | (v2 & 0x7F) | ((v3 & 0x7F) << 16);
		}
	}

	uint32_t __cdecl b7_729EA0(uint32_t a1)
	{
		b_72A350();
		b7_729EF0();
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_097_LoadScriptQueue), game_addr(game::MAG_097_FileLoadScriptTask), 0x30, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_097_DirectorQueue), game_addr(game::MAG_097_DirectorTask), 0x48, a1);
		b71_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b7_729EF0(void)
	{
		MEM<uint32_t>(game_addr(game::unk_2579EC0)) = game_addr(game::unk_152B930);
		MEM<uint32_t>(game_addr(game::MAG_097_AssetTablePtr)) = game_addr(game::MAG_097_AssetTable);
		x::MAG_007_sub_8DCC00(game_addr(game::MAG_097_AssetTable), 0x16C);
		const uint32_t t = MEM<uint32_t>(game_addr(game::MAG_097_AssetTablePtr));
		uint32_t cur = MEM<uint32_t>(game_addr(game::unk_257B5C8));
		U32(t, 0x100) = cur;
		cur += 0xA000;
		MEM<uint32_t>(game_addr(game::unk_257B5C8)) = cur;
		const uint32_t def = game_addr(game::unk_19DB018);
		U32(t, 0x114) = def;
		U32(t, 0x118) = def;
		U32(t, 0x11C) = def;
		U32(t, 0x120) = def;
		U32(t, 0x104) = def;
		U32(t, 0x108) = game_addr(game::unk_19DFEF8);
		U32(t, 0x10C) = def;
		U32(t, 0x110) = def;
		U32(t, 0x124) = def;
		U32(t, 0x128) = def;
		U32(t, 0x12C) = game_addr(game::unk_19E4958);
		U32(t, 0x130) = game_addr(game::unk_19E8660);
		for (int32_t o = 0x134; o <= 0x160; o += 4)
			U32(t, o) = def;
		U16(t, 0x164) = 0;
		U16(t, 0x166) = 0;
		return 0;
	}

	uint32_t __cdecl b7_72A410(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[18];
		states[0] = game_addr(game::Boko_DirectorSpawnMain);
		states[1] = game_addr(game::MAG_097_Director_State1_ArmLoadScriptStep);
		states[2] = game_addr(game::MAG_097_Director_State2_PauseUntilLoaded);
		states[3] = game_addr(game::MAG_097_SpawnBokoCreature);
		states[4] = game_addr(game::MAG_097_Director_State4_ArmLoadScriptStep);
		states[5] = game_addr(game::MAG_097_Director_State5_WaitLoadedCue1);
		states[6] = game_addr(game::MAG_097_Director_State6_WaitDirPhase2);
		states[7] = game_addr(game::MAG_097_Director_State7_ArmLoadScriptStep);
		states[8] = game_addr(game::MAG_097_Director_State8_WaitLoadScriptIdle);
		states[9] = game_addr(game::MAG_097_Director_State9_WaitDirGate3);
		states[10] = game_addr(game::MAG_097_Director_State10_DelayAtCue4);
		states[11] = game_addr(game::MAG_097_Director_State11_SpawnModelAfterDelay);
		states[12] = game_addr(game::MAG_097_Director_State12_CountdownAtCue5);
		states[13] = game_addr(game::MAG_097_Director_State13_SetEndFlag);
		states[14] = game_addr(game::MAG_097_Director_State14_SpawnFadeOut);
		states[15] = game_addr(game::MAG_097_Director_State15_CountdownTimer);
		states[16] = game_addr(game::MAG_097_Director_State16_ReleaseVoiceAtCue6);
		states[17] = game_addr(game::MAG_097_Director_State17_Empty);
		b7_72A4F0(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return b71_task_end(node, status);
	}

	uint32_t __cdecl b7_72A4F0(uint32_t a1)
	{
		uint32_t d = MEM<uint32_t>(game_addr(game::unk_152B5B8));
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			b7_731780(a1);
			d = MEM<uint32_t>(game_addr(game::unk_152B5B8));
		}
		const uint16_t next = U16(d, 0x44);
		if (U16(d, 0x42) != next)
		{
			U16(d, 0x42) = next;
			a_73A6D0();
		}
		return 0;
	}

	uint32_t __cdecl b7_72AB80(uint32_t a1)
	{
		if (S16(a1, 0x24) < S16(a1, 0x298))
			return 0;
		a_73F990(a1);
		b7_72B690(a1);
		b71_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b7_72B690(uint32_t a1)
	{
		uint32_t dir = a1 + 0x34;
		uint32_t done = 0;
		MEM<uint32_t>(game_addr(game::unk_257B5CC)) = dir;
		const uint16_t stage = U16(dir, 0x20);
		switch ((int32_t)(int16_t)stage)
		{
		case 0:
			U16(dir, 0x20) = (uint16_t)(stage + 1);
			break;
		case 1:
			a_743190();
			b7_72CEE0();
			a_740700();
			a_735440();
			dir = MEM<uint32_t>(game_addr(game::unk_257B5CC));
			U16(dir, 0x18) = (uint16_t)(U16(dir, 0x18) + 1);
			if (S16(dir, 0x18) >= S16(dir, 0x1A) && U16(dir, 0x14) == 0 && U16(dir, 0x16) == 0)
				U16(dir, 0x20) = (uint16_t)(U16(dir, 0x20) + 1);
			break;
		case 2:
			done = 1;
			break;
		default:
			break;
		}
		const uint16_t n1 = U16(dir, 0x14);
		const uint16_t n2 = U16(dir, 0x16);
		MEM<uint16_t>(game_addr(game::unk_257B5A0)) = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_257B5A0)) + n1);
		MEM<uint16_t>(game_addr(game::unk_257B168)) = (uint16_t)(MEM<uint16_t>(game_addr(game::unk_257B168)) + n2);
		return done;
	}

	uint32_t __cdecl b7_72CEE0(void)
	{
		uint32_t e = U32(MEM<uint32_t>(game_addr(game::unk_257B5CC)), 0x2C);
		while (e != 0)
		{
			const int32_t kind = (int32_t)S16(e, 8);
			if (kind == 1)
				b7_72CF20(e);
			else if (kind == 0)
				a_737ED0(e);
			e = U32(e, 4);
		}
		return 0;
	}

	uint32_t __cdecl b7_72CF20(uint32_t a1)
	{
		const uint32_t e = a1;
		const uint32_t table = U32(MEM<uint32_t>(game_addr(game::unk_257B5CC)), 0x224);
		const int32_t di = (int32_t)S8(e, 0x1D6);
		const uint32_t desc = U32(table + (uint32_t)(di * 4), 0);
		const int32_t state = (int32_t)S16(e, 0x1CC);
		if (state == 1)
		{
			a_742CD0(e);
			const uint32_t dir = MEM<uint32_t>(game_addr(game::unk_257B5CC));
			U8(e, 0x1D7) = 0;
			U16(dir, 0x16) = (uint16_t)(U16(dir, 0x16) - 1);
			return 0;
		}
		if (state != 0)
			return 0;
		a_741050(e, desc);
		if (a_7419C0(e, desc) == 0)
		{
			a_740910(e, desc);
			b7_72D7C0(e, desc);
			U16(e, 0x1CA) = (uint16_t)(U16(e, 0x1CA) + 1);
			return 0;
		}
		U16(e, 0x1CC) = (uint16_t)(U16(e, 0x1CC) + 1);
		U32(e, 0x170) = 0;
		return 0;
	}

	uint32_t __cdecl b7_72D7C0(uint32_t a1, uint32_t a2)
	{
		const uint32_t node = a1;
		const uint32_t desc = a2;
		const uint32_t m = node + 0x8C;
		uint8_t mode = U8(desc, 0x16);
		if (mode == 4 || mode == 5)
		{
			const int32_t frame = (int32_t)S16(node, 0x1CA);
			const uint32_t tex = U8(U32(desc, 0x12C) + (uint32_t)frame, 0);
			const uint32_t dir = MEM<uint32_t>(game_addr(game::unk_257B5CC));
			U32(node, 0x170) = U32(U32(dir, 0x22C) + tex * 4, 0);
			if (mode == 5)
				b_72E220(node, desc);
			if (U8(node, 0x1D6) == 0)
				b7_72DF60(U32(node, 0x170), (uint32_t)-8);
		}

		const int32_t rot_mode = (int32_t)S8(desc, 0x1B);
		switch ((uint32_t)rot_mode)
		{
		case 0:
			x::UnpackRotationMatrix(MEM<uint32_t>(game_addr(game::MAG_097_CameraMatrixPtr)), m);
			if (U16(node, 0xD0) != 0)
				x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0xD0));
			break;
		case 1:
			x::MAG_022_sub_8DD770(m);
			x::sub_8DD7E0(m, 0x400);
			break;
		case 2:
		{
			const uint32_t yaw = x::sub_8DD7B0(MEM<uint32_t>(game_addr(game::MAG_097_CameraMatrixPtr)));
			x::MAG_022_sub_8DD770(m);
			if ((uint16_t)yaw != 0)
				x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)(int16_t)yaw);
			break;
		}
		case 3:
		case 4:
		{
			const int32_t sub = (int32_t)S8(desc, 0x1D);
			const uint32_t parent = U32(node, 0x1BC);
			b71_build_rot(node, sub, parent);
			break;
		}
		default:
			break;
		}

		mode = U8(desc, 0x16);
		if (mode == 4 || mode == 5)
		{
			const int32_t frame = (int32_t)S16(node, 0x1CA);
			U8(node, 0x16C) = U8(U32(desc, 0xF0) + (uint32_t)frame, 0);
			U8(node, 0x16D) = U8(U32(desc, 0xF4) + (uint32_t)frame, 0);
			U8(node, 0x16E) = U8(U32(desc, 0xF8) + (uint32_t)frame, 0);
			U16(node, 0x1CE) = U16(U32(desc, 0xFC) + (uint32_t)(frame * 2), 0);
			U16(node, 0xD4) = U16(U32(desc, 0xE4) + (uint32_t)(frame * 2), 0);
			U16(node, 0xD6) = U16(U32(desc, 0xE8) + (uint32_t)(frame * 2), 0);
			const uint16_t sz = U16(U32(desc, 0xEC) + (uint32_t)(frame * 2), 0);
			int32_t scale[4];
			scale[0] = (int32_t)S16(node, 0xD4);
			scale[1] = (int32_t)S16(node, 0xD6);
			U16(node, 0xD8) = sz;
			scale[2] = (int32_t)(int16_t)sz;
			scale[3] = 0;
			x::scale3DMatrix(m, P(scale));
		}

		const uint8_t count = U8(node, 0x1D8);
		if (count == 1)
		{
			b71_world_to_view(node, node + 0xDC, MEM<uint32_t>(game_addr(game::MAG_097_CameraMatrixPtr)));
			U32(node, 0xC8) = (uint32_t)add32(S32(node, 0xC8), (int32_t)S16(desc, 0x38));
			return 0;
		}
		if ((int8_t)count > 0)
		{
			int32_t i = 0;
			uint32_t src = node + 0xDC;
			uint32_t dst = node + 0x19C;
			do
			{
				const uint32_t cam = MEM<uint32_t>(game_addr(game::MAG_097_CameraMatrixPtr));
				b71_world_to_view(node, src, cam);
				U16(dst, 0) = U16(node, 0xC0);
				U16(dst, 2) = U16(node, 0xC4);
				const uint16_t z = (uint16_t)(U16(node, 0xC8) + U16(desc, 0x38));
				src += 0x10;
				i++;
				U16(dst, 4) = z;
				dst += 8;
			} while (i < (int32_t)S8(node, 0x1D8));
		}
		return 0;
	}

	uint32_t __cdecl b7_72DF60(uint32_t a1, uint32_t a2)
	{
		const uint32_t d = a2;
		uint32_t p = b71_section(a1, 0);
		p = p + (uint32_t)mul32(S32(p, 0), 12) + 4;
		p = p + (uint32_t)mul32(S32(p, 0), 12) + 4;
		int32_t n = S32(p, 0);
		p += 4;
		for (; n > 0; n--)
		{
			b71_scroll3(p, d);
			p += 0x14;
		}
		n = S32(p, 0);
		p += 4;
		for (; n > 0; n--)
		{
			b71_scroll4(p, d);
			p += 0x18;
		}
		p = p + (uint32_t)mul32(S32(p, 0), 20) + 4;
		p = p + (uint32_t)mul32(S32(p, 0), 24) + 4;
		n = S32(p, 0);
		p += 4;
		for (; n > 0; n--)
		{
			b71_scroll3(p, d);
			p += 0x1C;
		}
		n = S32(p, 0);
		p += 4;
		for (; n > 0; n--)
		{
			b71_scroll4(p, d);
			p += 0x24;
		}
		return 0;
	}

	uint32_t __cdecl b7_72FB90(uint32_t a1)
	{
		if (b7_72B690(a1) != 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl b7_72FBC0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[8];
		states[0] = game_addr(game::MAG_097_EmitterStage_State0_StartCamScript);
		states[1] = game_addr(game::MAG_097_EmitterStage_State1_CamKeyStep);
		states[2] = game_addr(game::MAG_097_EmitterStage_State2_StartScriptAtCue2);
		states[3] = game_addr(game::MAG_097_EmitterStage_State3_CamKeyStepThenKeyAtStep4);
		states[4] = game_addr(game::MAG_097_EmitterStage_State4_CamKeyStepThenStartKey);
		states[5] = game_addr(game::MAG_097_EmitterStage_State5_WaitCamStep6);
		states[6] = game_addr(game::MAG_097_EmitterStage_State6_FinishNode);
		states[7] = game_addr(game::MAG_097_EmitterStage_State7_Empty);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return b71_task_end(node, status);
	}

	uint32_t __cdecl b7_730F10(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		if (a_73B640(4) != 0)
		{
			x::au_re_Battle_ReadAnimation_7(a1, 4);
			x::BdPlaySE(game_addr(game::unk_152B5C4), 0, 0x80);
			b71_next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl b7_7310C0(uint32_t a1)
	{
		U16(a1, 0x44) = (uint16_t)(U16(a1, 0x44) - 1);
		if (S16(a1, 0x44) > 0)
			return 0;
		const uint32_t model = U32(MEM<uint32_t>(game_addr(game::MAG_097_AssetTablePtr)), 0x130);
		b_72A610(a1, game_addr(game::MAG_097_UvModelTask), model, 0x290, 3, 0);
		b71_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b7_731100(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_097_UvModel_State0_Init);
		states[1] = game_addr(game::MAG_097_UvModel_State1_Play);
		states[2] = game_addr(game::MAG_097_UvModel_State2_Nop);
		callp(states[S8(node, 0x29)], node);
		a_7340A0(b71_section(U32(node, 0x74), 0x0C), 6);
		a_7340A0(b71_section(U32(node, 0x74), 0x10), 6);
		a_7340A0(b71_section(U32(node, 0x74), 0x1C), 6);
		a_7340A0(b71_section(U32(node, 0x74), 0x20), 6);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return b71_task_end(node, status);
	}

	uint32_t __cdecl b7_7315A0(uint32_t a1)
	{
		if (a_73B640(5) != 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0x44) = 0x20;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl b7_731600(uint32_t a1)
	{
		U16(a1, 0x44) = (uint16_t)(U16(a1, 0x44) - 1);
		if (S16(a1, 0x44) > 0x10)
			return 0;
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_097_TaskQueue), game_addr(game::MAG_097_EntityFadeOutTask), 0x70, a1);
		b71_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b7_731780(uint32_t a1)
	{
		const int32_t phase = (int32_t)S16(MEM<uint32_t>(game_addr(game::unk_152B5B8)), 0x40);
		if (phase == 2)
		{
			const uint32_t model = U32(MEM<uint32_t>(game_addr(game::MAG_097_AssetTablePtr)), 0x128);
			b_72A610(a1, game_addr(game::Boko_ParticleTaskA), model, 0x44C, 1, 0);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_097_TaskQueue), game_addr(game::MAG_097_EntityFadeInTask), 0x70, a1);
		}
		else if (phase == 4)
		{
			const uint32_t model = U32(MEM<uint32_t>(game_addr(game::MAG_097_AssetTablePtr)), 0x12C);
			b_72A610(a1, game_addr(game::Boko_ParticleTaskB), model, 0x170, 2, 0);
			const uint32_t model2 = U32(MEM<uint32_t>(game_addr(game::MAG_097_AssetTablePtr)), 0x108);
			a_73C100(a1, game_addr(game::MAG_097_EmitterStageSpawnTask), model2, 0, 0x2D, 2);
		}
		else if (phase == 5)
		{
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_097_TaskQueue), game_addr(game::MAG_097_EntityFadeInBrightTask), 0x70, a1);
		}
		return 0;
	}

	uint32_t __cdecl b8_721CA0(uint32_t a1)
	{
		b_72A350();
		b8_721CF0();
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25750D0), game_addr(game::MAG_098_sub_721DF0), 0x30, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_256E440), game_addr(game::Boko098_PhaseDirectorTask), 0x48, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b8_721CF0(void)
	{
		MEM<uint32_t>(game_addr(game::unk_2573C40)) = game_addr(game::unk_152B2A8);
		MEM<uint32_t>(game_addr(game::unk_2574D38)) = game_addr(game::unk_25750F8);
		x::MAG_007_sub_8DCC00(game_addr(game::unk_25750F8), 0x16C);
		uint32_t t = MEM<uint32_t>(game_addr(game::unk_2574D38));
		uint32_t arena = MEM<uint32_t>(game_addr(game::unk_25750F0));
		U32(t, 0x100) = arena;
		MEM<uint32_t>(game_addr(game::unk_25750F0)) = arena + 0xA000;
		const uint32_t base = game_addr(game::unk_19DB018);
		U32(t, 0x114) = base;
		U32(t, 0x118) = base;
		U32(t, 0x11C) = base;
		U32(t, 0x120) = base;
		U32(t, 0x104) = base;
		U32(t, 0x108) = game_addr(game::unk_19DFEF8);
		U32(t, 0x10C) = base;
		U32(t, 0x110) = base;
		U32(t, 0x124) = base;
		U32(t, 0x128) = base;
		U32(t, 0x12C) = game_addr(game::unk_19E187C);
		U32(t, 0x130) = game_addr(game::unk_19E5584);
		U32(t, 0x134) = game_addr(game::unk_19E8E70);
		for (int32_t o = 0x138; o <= 0x160; o += 4)
			U32(t, o) = base;
		U16(t, 0x164) = 0;
		U16(t, 0x166) = 0;
		return 0;
	}

	uint32_t __cdecl b8_722210(uint32_t a1)
	{
		const uint32_t tab[19] = {
			game_addr(game::sub_722340), game_addr(game::sub_7280E0), game_addr(game::sub_728140), game_addr(game::MAG_098_SpawnBokoCreature), game_addr(game::sub_728B20), game_addr(game::sub_728B40), game_addr(game::sub_728B60), game_addr(game::sub_728B80),
			game_addr(game::sub_728BA0), game_addr(game::sub_728BC0), game_addr(game::sub_728BE0), game_addr(game::sub_728C10), game_addr(game::sub_728D60), game_addr(game::sub_729240), game_addr(game::sub_729270), game_addr(game::sub_7292A0),
			game_addr(game::sub_7293A0), game_addr(game::sub_7293C0), game_addr(game::nullsub_1131),
		};
		b8_7222F0(a1);
		callp(tab[S8(a1, 0x29)], a1);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		if ((U8(a1, 0x26) & 1) != 0 && U8(a1, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(a1);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl b8_7222F0(uint32_t a1)
	{
		uint32_t cur = MEM<uint32_t>(game_addr(game::unk_152AF30));
		U16(cur, 0x46) = (uint16_t)(U16(cur, 0x46) + 1);
		uint16_t req = U16(cur, 0x42);
		if (U16(cur, 0x40) != req)
		{
			U16(cur, 0x40) = req;
			U16(cur, 0x46) = 0;
			b8_729420(a1);
			cur = MEM<uint32_t>(game_addr(game::unk_152AF30));
		}
		uint16_t nxt = U16(cur, 0x44);
		if (U16(cur, 0x42) != nxt)
		{
			U16(cur, 0x42) = nxt;
			a_73A6D0();
		}
		return 0;
	}

	uint32_t __cdecl b8_722340(uint32_t a1)
	{
		uint32_t slot = x::BdSound_ClaimVoiceSlot(game_addr(game::unk_16746FC), 1, 0x80);
		uint32_t cur = MEM<uint32_t>(game_addr(game::unk_152AF30));
		U16(cur, 0x52) = (uint16_t)slot;
		a_73C100(a1, game_addr(game::sub_722920), game_addr(game::unk_167D934), 0x12, 0x2D, 2);
		b8_722410(a1, game_addr(game::sub_722460), game_addr(game::unk_1680B9C), 0x4AC, 0, 0);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_256E548), game_addr(game::Boko098_CameraDirectorTask), 0x70, a1);
		x::Battle_QueueTIMUpload_GetEOF(game_addr(game::MAG098_Boko_BodyTIM));
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b8_722410(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25732B0), a2, 0x864, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		U32(t, 0x78) = (uint32_t)(int32_t)(int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl b8_7276F0(uint32_t a1)
	{
		const uint32_t tab[9] = {
			game_addr(game::sub_727780), game_addr(game::sub_727F80), game_addr(game::sub_727FA0), game_addr(game::sub_727FF0), game_addr(game::sub_728020), game_addr(game::Boko098_StartCameraScript2), game_addr(game::sub_728080), game_addr(game::sub_7280A0), game_addr(game::nullsub_1125),
		};
		callp(tab[S8(a1, 0x29)], a1);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		if ((U8(a1, 0x26) & 1) != 0 && U8(a1, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(a1);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl b8_728050(uint32_t a1)
	{
		if (a_73AE10() == 1)
		{
			uint32_t cur = MEM<uint32_t>(game_addr(game::unk_152AF30));
			a_73AB20(game_addr(game::unk_152B538), cur + 8, 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b8_728AE0(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		if (S16(a1, 0x136) >= 0x14)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 5;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl b8_728B20(uint32_t a1)
	{
		if (a_73A950() == 1)
		{
			if (a_73B640(1) != 0)
				U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b8_728C10(uint32_t a1)
	{
		U16(a1, 0x44) = (uint16_t)(U16(a1, 0x44) - 1);
		if (S16(a1, 0x44) <= 0)
		{
			uint32_t layout = U32(MEM<uint32_t>(game_addr(game::unk_2574D38)), 0x130);
			b8_722410(a1, game_addr(game::sub_728C60), layout, 0x494, 3, 0);
			uint8_t st = U8(a1, 0x29);
			U16(a1, 0x44) = 0x18;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl b8_728CC0(uint32_t a1)
	{
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		uint32_t cur = MEM<uint32_t>(game_addr(game::unk_152AF30));
		uint32_t xy = U32(cur, 8);
		uint32_t zw = U32(cur, 0xC);
		U32(a1, 0x1C) = xy;
		U32(a1, 0x20) = zw;
		U16(a1, 0x1E) = (uint16_t)(U16(a1, 0x1E) + 0xFE0C);
		U32(a1, 0x58) = 0x1000;
		U32(a1, 0x54) = 0x1000;
		U32(a1, 0x50) = 0x1000;
		b_72A720(a1);
		x::BdPlaySE(game_addr(game::unk_152AF3C), 0, 0x80);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b8_728D60(uint32_t a1)
	{
		U16(a1, 0x44) = (uint16_t)(U16(a1, 0x44) - 1);
		if (S16(a1, 0x44) <= 0)
		{
			uint32_t layout = U32(MEM<uint32_t>(game_addr(game::unk_2574D38)), 0x134);
			b8_722410(a1, game_addr(game::Boko098_FlareCoreTask), layout, 0x7D0, 4, 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	static uint32_t b81_section(uint32_t layout, int32_t off)
	{
		int32_t q = S32(layout, off) / 4;
		return layout + (uint32_t)q * 4u;
	}

	uint32_t __cdecl b8_728DA0(uint32_t a1)
	{
		const uint32_t tab[3] = { game_addr(game::sub_7291C0), game_addr(game::sub_729210), game_addr(game::nullsub_1129) };
		callp(tab[S8(a1, 0x29)], a1);
		a_7340A0(b81_section(U32(a1, 0x74), 0x8), 6);
		a_7340A0(b81_section(U32(a1, 0x74), 0xC), 4);
		a_7340A0(b81_section(U32(a1, 0x74), 0x10), 6);
		a_7340A0(b81_section(U32(a1, 0x74), 0x1C), 8);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		if ((U8(a1, 0x26) & 1) != 0 && U8(a1, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(a1);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl b8_729240(uint32_t a1)
	{
		if (a_73B640(5) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U16(a1, 0x44) = 0x2C;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl b8_729270(uint32_t a1)
	{
		U16(a1, 0x44) = (uint16_t)(U16(a1, 0x44) - 1);
		if (S16(a1, 0x44) <= 0x18)
		{
			U16(MEM<uint32_t>(game_addr(game::unk_152AF30)), 0x48) = 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b8_7292A0(uint32_t a1)
	{
		U16(a1, 0x44) = (uint16_t)(U16(a1, 0x44) - 1);
		if (S16(a1, 0x44) <= 8)
		{
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_256E548), game_addr(game::sub_7292D0), 0x70, a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b8_729350(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0xFF40);
		if (S16(a1, 0x1C) <= 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		uint16_t v = U16(a1, 0x1C);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int n = 4; n != 0; n--, p += 0x2C)
			MEM<uint16_t>(p) = v;
		return 0;
	}

	uint32_t __cdecl b8_729420(uint32_t a1)
	{
		int32_t phase = S16(MEM<uint32_t>(game_addr(game::unk_152AF30)), 0x40);
		if (phase == 2)
		{
			uint32_t layout = U32(MEM<uint32_t>(game_addr(game::unk_2574D38)), 0x128);
			b8_722410(a1, game_addr(game::sub_7294C0), layout, 0x44C, 1, 0);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_256E548), game_addr(game::sub_7296D0), 0x70, a1);
		}
		else if (phase == 4)
		{
			uint32_t layout = U32(MEM<uint32_t>(game_addr(game::unk_2574D38)), 0x12C);
			b8_722410(a1, game_addr(game::sub_7295D0), layout, 0x170, 2, 0);
		}
		else if (phase == 5)
		{
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_256E548), game_addr(game::sub_7297C0), 0x70, a1);
		}
		return 0;
	}

namespace
{
	uint32_t b91_task_end(uint32_t node, uint8_t status)
	{
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}
	inline void b91_next(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }
	inline void b91_finish(uint32_t node)
	{
		const uint8_t st = U8(node, 0x29);
		U8(node, 0x26) = (uint8_t)(U8(node, 0x26) | 1);
		U8(node, 0x29) = (uint8_t)(st + 1);
	}
}

	uint32_t __cdecl b9_7181B0(uint32_t a1)
	{
		b_718670();
		b9_718200();
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_256E1C0), game_addr(game::MAG_099_sub_718310), 0x30, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2565B18), game_addr(game::Choco099_DirectorTask), 0x48, a1);
		b91_next(a1);
		return 0;
	}

	uint32_t __cdecl b9_718200(void)
	{
		MEM<uint32_t>(game_addr(game::unk_256C5F8)) = game_addr(game::unk_152AAA0);
		MEM<uint32_t>(game_addr(game::unk_256D6F0)) = game_addr(game::unk_256E1E8);
		x::MAG_007_sub_8DCC00(game_addr(game::unk_256E1E8), 0x16C);
		const uint32_t t = MEM<uint32_t>(game_addr(game::unk_256D6F0));
		const uint32_t arena = MEM<uint32_t>(game_addr(game::unk_256E1E0));
		U32(t, 0x100) = arena;
		MEM<uint32_t>(game_addr(game::unk_256E1E0)) = arena + 0xA000;
		const uint32_t base = game_addr(game::unk_19D3018);
		U32(t, 0x114) = base;
		U32(t, 0x118) = base;
		U32(t, 0x11C) = base;
		U32(t, 0x120) = base;
		U32(t, 0x104) = base;
		U32(t, 0x108) = base;
		U32(t, 0x10C) = base;
		U32(t, 0x110) = base;
		U32(t, 0x124) = base;
		U32(t, 0x128) = game_addr(game::unk_19E3018);
		U32(t, 0x12C) = game_addr(game::unk_19EB018);
		U32(t, 0x130) = game_addr(game::unk_19EED20);
		U32(t, 0x134) = game_addr(game::unk_19F0F40);
		U32(t, 0x138) = game_addr(game::unk_19DF380);
		U32(t, 0x13C) = game_addr(game::unk_19E2054);
		U32(t, 0x140) = game_addr(game::unk_19E4778);
		U32(t, 0x144) = game_addr(game::unk_19DD2D0);
		U32(t, 0x148) = game_addr(game::unk_19DE510);
		U32(t, 0x14C) = base;
		U32(t, 0x150) = base;
		U32(t, 0x154) = base;
		U32(t, 0x158) = base;
		U32(t, 0x15C) = base;
		U32(t, 0x160) = base;
		U16(t, 0x164) = 0;
		U16(t, 0x166) = 0;
		return t;
	}

	uint32_t __cdecl b9_718750(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[21];
		states[0] = game_addr(game::sub_718890);
		states[1] = game_addr(game::sub_71E7A0);
		states[2] = game_addr(game::sub_71E800);
		states[3] = game_addr(game::MAG_099_SpawnBokoCreature);
		states[4] = game_addr(game::sub_71F1D0);
		states[5] = game_addr(game::sub_71F1F0);
		states[6] = game_addr(game::sub_71F220);
		states[7] = game_addr(game::sub_71F240);
		states[8] = game_addr(game::sub_71F260);
		states[9] = game_addr(game::sub_71F280);
		states[10] = game_addr(game::sub_71F2A0);
		states[11] = game_addr(game::sub_71F2D0);
		states[12] = game_addr(game::sub_71F4C0);
		states[13] = game_addr(game::sub_71F4E0);
		states[14] = game_addr(game::sub_71F500);
		states[15] = game_addr(game::sub_71F520);
		states[16] = game_addr(game::sub_71F550);
		states[17] = game_addr(game::sub_71F580);
		states[18] = game_addr(game::sub_71F5A0);
		states[19] = game_addr(game::sub_71F5C0);
		states[20] = game_addr(game::nullsub_1095);
		b9_718840(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return b91_task_end(node, status);
	}

	uint32_t __cdecl b9_718840(uint32_t a1)
	{
		uint32_t d = MEM<uint32_t>(game_addr(game::unk_1529FA0));
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			b9_71F620(a1);
			d = MEM<uint32_t>(game_addr(game::unk_1529FA0));
		}
		const uint16_t nxt = U16(d, 0x44);
		if (U16(d, 0x42) != nxt)
		{
			U16(d, 0x42) = nxt;
			return a_73A6D0();
		}
		return d;
	}

	uint32_t __cdecl b9_718890(uint32_t a1)
	{
		const uint32_t slot = x::BdSound_ClaimVoiceSlot(game_addr(game::unk_16659D8), 1, 0x80);
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_1529FA0));
		const uint32_t node = a1;
		U16(d, 0x52) = (uint16_t)slot;
		a_73C100(node, game_addr(game::sub_718E70), game_addr(game::unk_166A9F0), 0x12, 0x2D, 2);
		b9_718960(node, game_addr(game::sub_7189B0), game_addr(game::unk_166DC58), 0x4AC, 0, 0);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2565C20), game_addr(game::Choco099_CameraTask), 0xB0, node);
		x::Battle_QueueTIMUpload_GetEOF(game_addr(game::MAG099_Boko_BodyTIM));
		b91_next(node);
		return 0;
	}

	uint32_t __cdecl b9_718960(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_256B9D8), a2, 0x548, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		S32(t, 0x78) = (int32_t)(int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl b9_71DC40(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[10];
		states[0] = game_addr(game::sub_71DCD0);
		states[1] = game_addr(game::sub_71E4D0);
		states[2] = game_addr(game::sub_71E4F0);
		states[3] = game_addr(game::sub_71E540);
		states[4] = game_addr(game::sub_71E570);
		states[5] = game_addr(game::sub_71E5C0);
		states[6] = game_addr(game::sub_71E610);
		states[7] = game_addr(game::sub_71E6C0);
		states[8] = game_addr(game::sub_71E700);
		states[9] = game_addr(game::nullsub_1091);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return b91_task_end(node, status);
	}

	static void b91_cam_from_origin(uint32_t key)
	{
		alignas(4) uint16_t v[4];
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		v[3] = 0;
		a_73AB20(key, P(v), 0);
	}

	uint32_t __cdecl b9_71E570(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(5) != 0)
		{
			b91_cam_from_origin(game_addr(game::unk_152AD20));
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_71E5C0(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(8) != 0)
		{
			b91_cam_from_origin(game_addr(game::unk_152AD68));
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_71E610(uint32_t a1)
	{
		const uint32_t c = MEM<uint32_t>(game_addr(game::unk_152AC40));
		U16(c, 0x86) = (uint16_t)(U16(c, 0x86) + 0x10);
		if (S16(c, 0x86) >= 0x80)
			U16(c, 0x86) = 0x80;
		U16(c, 0x5E) = (uint16_t)(U16(c, 0x5E) + U16(c, 0x86));
		U16(c, 0x8C) = (uint16_t)(U16(c, 0x8C) + 1);
		if (S16(c, 0x8C) >= 0x20)
			U16(c, 0x8C) = 0x20;
		const int16_t ang = (int16_t)((uint16_t)(U16(c, 0xD8) + U16(c, 0x8C)) & 0xFFF);
		U16(c, 0xD8) = (uint16_t)ang;
		if (ang < 0x800 && ang >= 0)
			U16(c, 0xD8) = 0;
		a_73B160();
		if (a_73AAE0(0xA) != 0)
		{
			const uint32_t d = MEM<uint32_t>(game_addr(game::unk_1529FA0));
			a_73AB20(game_addr(game::unk_152AE40), d + 8, 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_71E6C0(uint32_t a1)
	{
		if (a_73AE10() == 1)
		{
			const uint32_t d = MEM<uint32_t>(game_addr(game::unk_1529FA0));
			a_73AB20(game_addr(game::unk_152AE88), d + 8, 0);
			const uint32_t c = MEM<uint32_t>(game_addr(game::unk_152AC40));
			U16(c, 0x8E) = 6;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_71E700(uint32_t a1)
	{
		a_73AE10();
		const uint32_t c = MEM<uint32_t>(game_addr(game::unk_152AC40));
		if (S16(c, 0xE2) >= 0x42)
		{
			U16(c, 0x8E) = (uint16_t)(U16(c, 0x8E) - 1);
			if (S16(c, 0x8E) < 0)
				U16(c, 0x8E) = 0;
		}
		U16(c, 0xDA) = (uint16_t)((uint16_t)(U16(c, 0x8E) + U16(c, 0xDA)) & 0xFFF);
		a_73B160();
		if (a_73AAE0(0xB) != 0)
		{
			a_73A6D0();
			b91_finish(a1);
		}
		return 0;
	}

	uint32_t __cdecl b9_71E8D0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[9];
		states[0] = game_addr(game::sub_71ED20);
		states[1] = game_addr(game::sub_71EDC0);
		states[2] = game_addr(game::sub_71EDF0);
		states[3] = game_addr(game::sub_71F080);
		states[4] = game_addr(game::sub_71F0F0);
		states[5] = game_addr(game::sub_71F140);
		states[6] = game_addr(game::sub_71F160);
		states[7] = game_addr(game::sub_71F190);
		states[8] = game_addr(game::nullsub_1093);
		callp(states[S8(node, 0x29)], node);
		if ((U8(node, 0x26) & 4) == 0)
		{
			const uint32_t cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = a_746C10(node, game_addr(game::unk_256C600), cursor);
			a_739890(node);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return b91_task_end(node, status);
	}

	uint32_t __cdecl b9_71EDF0(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		if (S16(node, 0x136) >= 0)
		{
			x::au_re_Battle_ReadAnimation_7(node, 2);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2565C20), game_addr(game::sub_71EE40), 0xB0, node);
			b91_next(node);
		}
		return 0;
	}

	uint32_t __cdecl b9_71EF30(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_1529FA0));
		const uint32_t node = a1;
		const uint32_t xy = U32(d, 0);
		const uint32_t zp = U32(d, 4);
		U16(node, 0x26) = (uint16_t)(U16(node, 0x26) | 4);
		U32(node, 0x1C) = xy;
		U32(node, 0x20) = zp;
		const uint8_t st = U8(node, 0x29);
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + 0xFD00);
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + 0xFD80);
		U32(node, 0x4C) = game_addr(game::unk_1529FB4);
		U16(node, 0x52) = 0x13;
		U16(node, 0x54) = 0xFF00;
		U16(node, 0xAC) = 4;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl b9_71EF80(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0xAC) = (uint16_t)(U16(node, 0xAC) - 1);
		if (S16(node, 0xAC) <= 0)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) = (uint8_t)(U8(node, 0x26) & 0xFB);
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_71EFB0(uint32_t a1)
	{
		const uint32_t node = a1;
		a_743C20(node);
		if (S16(node, 0x50) > 8)
		{
			const uint8_t st = U8(node, 0x29);
			U16(node, 0xAC) = 2;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_71F010(uint32_t a1)
	{
		const uint32_t node = a1;
		a_743C20(node);
		if (S16(node, 0x50) > 9)
			U16(node, 0x50) = 8;
		U16(node, 0xAC) = (uint16_t)(U16(node, 0xAC) - 1);
		if (S16(node, 0xAC) <= 0)
			b91_next(node);
		return 0;
	}

	uint32_t __cdecl b9_71F190(uint32_t a1)
	{
		if (a_73B7E0(5) != 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 5);
			U8(a1, 0x29) = (uint8_t)(st + 1);
			return a1;
		}
		return x::sub_8DD1C0(a1);
	}

	uint32_t __cdecl b9_71F2D0(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x44) = (uint16_t)(U16(node, 0x44) - 1);
		if (S16(node, 0x44) <= 0)
		{
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2565C20), game_addr(game::sub_71F310), 0xB0, node);
			b91_next(node);
		}
		return 0;
	}

	uint32_t __cdecl b9_71F3A0(uint32_t a1)
	{
		if (a_73B7E0(5) != 0)
		{
			a_746A10();
			a_73B9F0();
			x::BdPlaySE(game_addr(game::unk_1529FAC), 0, 0x80);
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0xAC) = 0x24;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_71F450(uint32_t a1)
	{
		U16(a1, 0xAC) = (uint16_t)(U16(a1, 0xAC) - 1);
		if (S16(a1, 0xAC) <= 0)
			b91_next(a1);
		return 0;
	}

	uint32_t __cdecl b9_71F470(uint32_t a1)
	{
		if (a_73B640(7) != 0)
		{
			a_73BB60(0);
			b91_finish(a1);
			return 0;
		}
		return a_73BB60(1);
	}

	uint32_t __cdecl b9_71F4C0(uint32_t a1)
	{
		if (a_73B7E0(5) != 0)
		{
			if (a_73A950() == 1)
				b91_next(a1);
		}
		return 0;
	}

	uint32_t __cdecl b9_71F520(uint32_t a1)
	{
		if (a_73B7E0(9) != 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0x44) = 0x20;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_71F550(uint32_t a1)
	{
		U16(a1, 0x44) = (uint16_t)(U16(a1, 0x44) - 1);
		if (S16(a1, 0x44) <= 0)
		{
			const uint32_t d = MEM<uint32_t>(game_addr(game::unk_1529FA0));
			U16(a1, 0x44) = 0x28;
			U16(d, 0x48) = 1;
			b91_next(a1);
		}
		return 0;
	}

	uint32_t __cdecl b9_71F5A0(uint32_t a1)
	{
		if (a_73B640(0xB) != 0)
			b91_finish(a1);
		return 0;
	}

	uint32_t __cdecl b9_71F5C0(uint32_t a1)
	{
		if (a_73B7E0(0xB) != 0)
		{
			const uint32_t d = MEM<uint32_t>(game_addr(game::unk_1529FA0));
			x::sub_4A2940((uint32_t)(int32_t)S16(d, 0x52));
			b91_finish(a1);
		}
		return 0;
	}

	uint32_t __cdecl b9_71F620(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_1529FA0));
		const int32_t sel = (int32_t)S16(d, 0x40) - 2;
		switch (sel)
		{
		case 0:
		{
			const uint32_t tab = MEM<uint32_t>(game_addr(game::unk_256D6F0));
			b9_718960(node, game_addr(game::sub_71F950), U32(tab, 0x128), 0x44C, 1, 0);
			return x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2565C20), game_addr(game::sub_721010), 0xB0, node);
		}
		case 2:
		{
			const uint32_t tab = MEM<uint32_t>(game_addr(game::unk_256D6F0));
			return b9_718960(node, game_addr(game::sub_71FDC0), U32(tab, 0x12C), 0x170, 2, 0);
		}
		case 3:
		{
			const uint32_t tab = MEM<uint32_t>(game_addr(game::unk_256D6F0));
			b9_718960(node, game_addr(game::sub_71FEC0), U32(tab, 0x130), 0xB0, 3, 0);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2565C20), game_addr(game::sub_721100), 0xB0, node);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2565C20), game_addr(game::sub_720500), 0xB0, node);
			return x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2565C20), game_addr(game::sub_720790), 0xB0, node);
		}
		case 5:
			return x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2565C20), game_addr(game::sub_7215A0), 0xB0, node);
		case 6:
		{
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2569338), game_addr(game::sub_7211D0), 0x58, node);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2569338), game_addr(game::sub_721390), 0x58, node);
			uint32_t tab = MEM<uint32_t>(game_addr(game::unk_256D6F0));
			b9_718960(node, game_addr(game::sub_71FFB0), U32(tab, 0x134), 0x2C, 4, 0);
			tab = MEM<uint32_t>(game_addr(game::unk_256D6F0));
			b9_718960(node, game_addr(game::sub_7201F0), U32(tab, 0x138), 0x194, 5, 0);
			tab = MEM<uint32_t>(game_addr(game::unk_256D6F0));
			b9_718960(node, game_addr(game::sub_720320), U32(tab, 0x13C), 0x30, 6, 0);
			tab = MEM<uint32_t>(game_addr(game::unk_256D6F0));
			return b9_718960(node, game_addr(game::sub_720410), U32(tab, 0x140), 0x1E8, 7, 0);
		}
		case 7:
			return x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2565C20), game_addr(game::sub_721460), 0xB0, node);
		case 8:
		{
			const uint32_t tab = MEM<uint32_t>(game_addr(game::unk_256D6F0));
			a_73C100(node, game_addr(game::sub_718E70), U32(tab, 0x108), 4, 0x2D, 2);
			const uint32_t t1 = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_256C470), game_addr(game::sub_71F880), 0x40, node);
			U32(t1, 0x34) = game_addr(game::unk_152AEF0);
			U16(t1, 0x38) = 8;
			const uint32_t t2 = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_256C470), game_addr(game::Effect_ScreenTintTask), 0x40, node);
			U32(t2, 0x34) = game_addr(game::unk_152AF04);
			U16(t2, 0x38) = 9;
			return t2;
		}
		default:
			return (uint32_t)sel;
		}
	}

	uint32_t __cdecl b9_71FF20(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		U16(node, 0x1C) = 0;
		U16(node, 0x1E) = 0;
		U16(node, 0x20) = 0;
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		b_72A720(node);
		b91_next(node);
		return 0;
	}

	uint32_t __cdecl b9_71FF70(uint32_t a1)
	{
		if (a_73B7E0(8) != 0)
		{
			b91_finish(a1);
			return 0;
		}
		return b_72A720(a1);
	}

	uint32_t __cdecl b9_720020(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		U16(node, 0x1C) = 0x1000;
		U16(node, 0x1E) = 0xF000;
		U16(node, 0x20) = 0;
		U16(node, 0x68) = 0xFE80;
		U16(node, 0x6A) = 0x200;
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		b_72A720(node);
		b91_next(node);
		return 0;
	}

	uint32_t __cdecl b9_720080(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t old_xy = U32(node, 0x1C);
		const uint16_t vx = U16(node, 0x68);
		const uint32_t old_zp = U32(node, 0x20);
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + vx);
		const uint16_t spin = U16(node, 0x64);
		U32(node, 0x540) = old_xy;
		const uint16_t vy = U16(node, 0x6A);
		U32(node, 0x544) = old_zp;
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + vy);
		const uint16_t vz = U16(node, 0x6C);
		const int16_t y = S16(node, 0x1E);
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + vz);
		U16(node, 0x64) = (uint16_t)((uint16_t)(spin - 0x40) & 0xFFF);
		if (y >= 0)
		{
			const uint8_t st = U8(node, 0x29);
			U16(node, 0x84) = 0xA;
			U8(node, 0x29) = (uint8_t)(st + 1);
			return 0;
		}
		b_72A720(node);
		return b9_7209B0(node, 0xA);
	}

	uint32_t __cdecl b9_7200F0(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x84) = (uint16_t)(U16(node, 0x84) - 1);
		if (S16(node, 0x84) <= 0)
		{
			const uint16_t nx = (uint16_t)(U16(node, 0x68) * 0xFFFBu);
			const uint16_t vy5 = (uint16_t)(U16(node, 0x6A) * 5u);
			U16(node, 0x1C) = nx;
			U16(node, 0x84) = 0;
			U16(node, 0x1E) = (uint16_t)(0x600u - vy5);
			b91_next(node);
		}
		return 0;
	}

	uint32_t __cdecl b9_720140(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x84) = (uint16_t)(U16(node, 0x84) + 1);
		const int16_t t = S16(node, 0x84);
		if (t >= 4 && t <= 8)
			b9_720D70(node, 0xA);
		const uint16_t vx = U16(node, 0x68);
		const uint32_t old_xy = U32(node, 0x1C);
		const uint32_t old_zp = U32(node, 0x20);
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + vx);
		const uint16_t spin = U16(node, 0x64);
		U32(node, 0x540) = old_xy;
		const uint16_t vy = U16(node, 0x6A);
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + vy);
		U32(node, 0x544) = old_zp;
		const uint16_t vz = U16(node, 0x6C);
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + vz);
		U16(node, 0x64) = (uint16_t)((uint16_t)(spin - 0x40) & 0xFFF);
		if (a_73B7E0(0xA) != 0)
		{
			b91_finish(node);
			return 0;
		}
		b_72A720(node);
		return b9_7209B0(node, 0xA);
	}

	uint32_t __cdecl b9_720250(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		U16(node, 0x1C) = 0;
		U16(node, 0x1E) = 0;
		U16(node, 0x20) = 0x360;
		b9_720290(node);
		b91_next(node);
		return 0;
	}

	uint32_t __cdecl b9_720290(uint32_t a1)
	{
		alignas(4) uint8_t blk[0x5C] = {};
		const uint32_t L = P(blk);
		x::MAG_022_sub_8DD770(L);
		const uint32_t node = a1;
		U32(L, 0x48) = game_addr(game::unk_256DA58);
		S32(L, 0x14) = (int32_t)S16(node, 0x1C);
		S32(L, 0x18) = (int32_t)S16(node, 0x1E);
		const int32_t pz = S16(node, 0x20);
		const uint32_t paused = MEM<uint32_t>(game_addr(game::unk_2569334));
		S32(L, 0x1C) = pz;
		return prim_play(node + 0x94, game_addr(game::sub_718B90), L, paused);
	}

	uint32_t __cdecl b9_7202F0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t left = b9_720290(node);
		if (left == 0)
			b91_finish(node);
		return 0;
	}

	uint32_t __cdecl b9_720380(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		U16(node, 0x1C) = 0;
		U16(node, 0x20) = 0;
		U16(node, 0x1E) = 0x600;
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		b_72A720(node);
		b91_next(node);
		return 0;
	}

	uint32_t __cdecl b9_7203D0(uint32_t a1)
	{
		if (a_73B7E0(0xA) != 0)
		{
			b91_finish(a1);
			return 0;
		}
		return b_72A720(a1);
	}

	namespace
	{
		inline uint32_t b92_task_end(uint32_t node)
		{
			const uint8_t status = U8(node, 0x26);
			U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
			if ((status & 1) != 0 && U8(node, 0x28) == 0)
			{
				x::Effect_ReleaseLinkedTask(node);
				return 2;
			}
			return 0;
		}

		inline int32_t b92_div(int32_t v, int sh) { return v / (1 << sh); }

		inline void b92_particle_step(uint32_t node, int i)
		{
			const uint32_t v = node + 0x8C + 8 * i;
			const uint32_t p = node + 0x6C + 8 * i;
			for (int k = 0; k < 3; k++)
			{
				const uint16_t c = U16(v, 2 * k);
				U16(v, 2 * k) = (uint16_t)(c - (uint16_t)b92_div((int16_t)c, 3));
			}
			for (int k = 0; k < 3; k++)
			{
				const int32_t d = b92_div(S16(v, 2 * k), 4);
				U16(p, 2 * k) = (uint16_t)(U16(p, 2 * k) + (uint16_t)d);
			}
			U32(node, 0x1C) = U32(p, 0);
			U32(node, 0x20) = U32(p, 4);
		}
	}

	uint32_t __cdecl b9_720470(uint32_t a1)
	{
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		U16(a1, 0x1C) = 0;
		U16(a1, 0x20) = 0;
		U16(a1, 0x1E) = 0x600;
		U32(a1, 0x58) = 0x1000;
		U32(a1, 0x54) = 0x1000;
		U32(a1, 0x50) = 0x1000;
		b_72A720(a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b9_7204C0(uint32_t a1)
	{
		if (a_73B7E0(0xA) != 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
			return 0;
		}
		b_72A720(a1);
		return 0;
	}

	uint32_t __cdecl b9_720500(uint32_t a1)
	{
		const uint32_t states[4] = { game_addr(game::sub_720570), game_addr(game::sub_7205A0), game_addr(game::sub_7205D0), game_addr(game::nullsub_1106) };
		callp(states[S8(a1, 0x29)], a1);
		b_730C90(a1, 0x1800);
		return b92_task_end(a1);
	}

	uint32_t __cdecl b9_720570(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U8(a1, 0x26) |= 4;
		U32(a1, 0x4C) = game_addr(game::unk_152A460);
		U16(a1, 0x52) = 0xE;
		U16(a1, 0x1E) = 0xE000;
		U16(a1, 0xAC) = 0x10;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl b9_7205A0(uint32_t a1)
	{
		U16(a1, 0xAC) = (uint16_t)(U16(a1, 0xAC) - 1);
		if (S16(a1, 0xAC) <= 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) &= 0xFB;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_7205D0(uint32_t a1)
	{
		if (a_743C20(a1) != 0)
		{
			const uint32_t asset = U32(MEM<uint32_t>(game_addr(game::unk_256D6F0)), 0x134);
			b9_718960(a1, game_addr(game::sub_720610), asset, 0x2C, 4, 0);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_720670(uint32_t a1)
	{
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		U16(a1, 0x1C) = 0;
		U16(a1, 0x20) = 0;
		U16(a1, 0x1E) = 0xE000;
		U16(a1, 0x68) = 0xFE80;
		U16(a1, 0x6A) = 0x200;
		U32(a1, 0x58) = 0x100;
		U32(a1, 0x54) = 0x100;
		U32(a1, 0x50) = 0x100;
		b_72A720(a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b9_7206D0(uint32_t a1)
	{
		const int32_t sc = S32(a1, 0x50);
		if (sc < 0x1000)
		{
			const int32_t n = add32(sc, 0x300);
			S32(a1, 0x50) = n;
			if (n >= 0x1000)
				S32(a1, 0x50) = 0x1000;
			const uint32_t v = U32(a1, 0x50);
			U32(a1, 0x58) = v;
			U32(a1, 0x54) = v;
		}
		const uint16_t vx = U16(a1, 0x68);
		const uint32_t oldxy = U32(a1, 0x1C);
		const uint32_t oldz = U32(a1, 0x20);
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + vx);
		const uint16_t rot = U16(a1, 0x64);
		U32(a1, 0x540) = oldxy;
		const uint16_t vy = U16(a1, 0x6A);
		U16(a1, 0x1E) = (uint16_t)(U16(a1, 0x1E) + vy);
		U32(a1, 0x544) = oldz;
		const uint16_t vz = U16(a1, 0x6C);
		U16(a1, 0x20) = (uint16_t)(U16(a1, 0x20) + vz);
		U16(a1, 0x64) = (uint16_t)((uint16_t)(rot - 0x40) & 0xFFF);
		if (a_73B7E0(8) != 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		}
		b_72A720(a1);
		b9_7209B0(a1, 8);
		return 0;
	}

	uint32_t __cdecl b9_720790(uint32_t a1)
	{
		const uint32_t states[2] = { game_addr(game::sub_7207E0), game_addr(game::nullsub_1108) };
		callp(states[S8(a1, 0x29)], a1);
		return b92_task_end(a1);
	}

	uint32_t __cdecl b9_7207E0(uint32_t a1)
	{
		alignas(4) int16_t vec[4];
		alignas(4) uint8_t mat[0x20];
		for (int n = 2; n != 0; n--)
		{
			uint32_t node = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2565C20), game_addr(game::sub_7208D0), 0xB0, a1);
			const int32_t r = (int32_t)x::CrtRand();
			switch (r % 3)
			{
			case 0: U32(node, 0x4C) = game_addr(game::unk_152A70C); break;
			case 1: U32(node, 0x4C) = game_addr(game::unk_152A7D0); break;
			case 2: U32(node, 0x4C) = game_addr(game::unk_152A894); break;
			default: break;
			}
			U16(node, 0x52) = 6;
			x::MAG_022_sub_8DD770(P(mat));
			const uint32_t ry = x::CrtRand() & 0xFFF;
			x::MAG_022_sub_8DD8A0(P(mat), ry);
			const int32_t r2 = (int32_t)x::CrtRand();
			x::sub_8DD7E0(P(mat), (uint32_t)(r2 % 0x280 + 0x80));
			node += 0x1C;
			vec[0] = 0;
			vec[1] = (int16_t)0xDC00;
			vec[2] = 0;
			x::matrixMultiplyVector(P(mat), P(vec), node);
		}
		if (a_73B7E0(8) != 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_7208D0(uint32_t a1)
	{
		const uint32_t states[3] = { game_addr(game::sub_720940), game_addr(game::sub_720960), game_addr(game::nullsub_1107) };
		callp(states[S8(a1, 0x29)], a1);
		b_730C90(a1, 0x1600);
		return b92_task_end(a1);
	}

	uint32_t __cdecl b9_720940(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U32(a1, 0x4C) = game_addr(game::unk_152A70C);
		U16(a1, 0x52) = 6;
		U16(a1, 0x1E) = 0xE000;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl b9_720960(uint32_t a1)
	{
		if (a_743C20(a1) != 0 || a_73B7E0(8) != 0)
		{
			U8(a1, 0x26) |= 5;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_7209B0(uint32_t a1, uint32_t a2)
	{
		const uint32_t node = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2565C20), game_addr(game::Choco099_TrailTask), 0xB0, a1);
		alignas(4) uint32_t pos[2];
		pos[0] = U32(a1, 0x1C);
		pos[1] = U32(a1, 0x20);
		U16(node, 0xAE) = (uint16_t)a2;
		const int32_t dx = (S16(a1, 0x540) - S16(a1, 0x1C)) / 4;
		const int32_t dy = (S16(a1, 0x542) - S16(a1, 0x1E)) / 4;
		const int32_t dz = (S16(a1, 0x544) - S16(a1, 0x20)) / 4;
		const uint16_t dzw = (uint16_t)dz;
		uint32_t e = node + 0x8C;
		for (int n = 4; n != 0; n--)
		{
			U32(node, 0x1C) = pos[0];
			U32(node, 0x20) = pos[1];
			b9_720AE0(node, 0x100);
			U32(e, -0x20) = U32(node, 0x1C);
			U32(e, -0x1C) = U32(node, 0x20);
			const uint32_t r0 = x::CrtRand();
			U16(e, 0) = (uint16_t)((r0 & 0x3FF) - 0x200);
			const uint32_t r1 = x::CrtRand();
			U16(e, 2) = (uint16_t)((r1 & 0x3FF) - 0x200);
			const uint32_t r2 = x::CrtRand();
			U16(P(pos), 0) = (uint16_t)(U16(P(pos), 0) + (uint16_t)dx);
			U16(P(pos), 2) = (uint16_t)(U16(P(pos), 2) + (uint16_t)dy);
			U16(e, 4) = (uint16_t)((r2 & 0x3FF) - 0x200);
			U16(P(pos), 4) = (uint16_t)(U16(P(pos), 4) + dzw);
			e += 8;
		}
		return 0;
	}

	uint32_t __cdecl b9_720AE0(uint32_t a1, uint32_t a2)
	{
		uint32_t range = a2;
		if ((uint16_t)range == 0)
			range = 1;
		alignas(4) int16_t vec[4];
		alignas(4) int16_t out[4];
		alignas(4) uint8_t mat[0x20];
		const uint32_t ra = x::CrtRand() & 0xFFF;
		const uint32_t rb = x::CrtRand() & 0xFFF;
		x::MAG_022_sub_8DD770(P(mat));
		x::MAG_022_sub_8DD8A0(P(mat), (uint32_t)(int32_t)(int16_t)ra);
		x::sub_8DD7E0(P(mat), (uint32_t)(int32_t)(int16_t)rb);
		vec[0] = 0;
		vec[1] = 0;
		const int32_t r = (int32_t)x::CrtRand();
		vec[2] = (int16_t)(r % (int32_t)(int16_t)range);
		x::matrixMultiplyVector(P(mat), P(vec), P(out));
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + (uint16_t)out[0]);
		U16(a1, 0x1E) = (uint16_t)(U16(a1, 0x1E) + (uint16_t)out[1]);
		U16(a1, 0x20) = (uint16_t)(U16(a1, 0x20) + (uint16_t)out[2]);
		return 0;
	}

	uint32_t __cdecl b9_720B90(uint32_t a1)
	{
		const uint32_t states[3] = { game_addr(game::sub_720D00), game_addr(game::sub_720D20), game_addr(game::nullsub_1109) };
		callp(states[S8(a1, 0x29)], a1);
		for (int i = 0; i < 4; i++)
		{
			b92_particle_step(a1, i);
			b9_720C90(a1);
		}
		return b92_task_end(a1);
	}

	uint32_t __cdecl b9_720C90(uint32_t a1)
	{
		if ((U8(a1, 0x26) & 4) != 0)
			return 0;
		const uint32_t blk = x::Field_Alloc(0xB4);
		xm::TransformCameraByShadowRotation(a1 + 0x1C, 0x1000, (uint32_t)(int32_t)S16(a1, 0x54));
		const uint32_t seq = U32(a1, 0x4C);
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		const uint16_t frame = U16(a1, 0x50);
		U32(blk, 0) = seq;
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(blk, 4) = frame;
		U16(blk, 0x24) = 0;
		const uint32_t nc = xm::InitEffectSequenceFromData(blk, ot, 2, cursor);
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = nc;
		x::Field_Free(0xB4);
		return 0;
	}

	uint32_t __cdecl b9_720D00(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U32(a1, 0x4C) = game_addr(game::unk_152A958);
		U16(a1, 0x52) = 0xB;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl b9_720D20(uint32_t a1)
	{
		if (a_743C20(a1) != 0 || a_73B7E0(U16(a1, 0xAE)) != 0)
		{
			U8(a1, 0x26) |= 5;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_720D70(uint32_t a1, uint32_t a2)
	{
		uint32_t e = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2565C20), game_addr(game::Choco099_BurstTask), 0xB0, a1);
		U16(e, 0xAE) = (uint16_t)a2;
		uint32_t angle = x::CrtRand() & 0xFFF;
		e += 0x6C;
		for (int n = 4; n != 0; n--)
		{
			U32(e, 0) = U32(a1, 0x1C);
			U32(e, 4) = U32(a1, 0x20);
			const uint32_t r0 = x::CrtRand();
			const uint32_t a = (uint32_t)(int32_t)(int16_t)angle;
			const int32_t radius = (int16_t)(uint16_t)((r0 & 0x7F) + 0x80);
			const int32_t s0 = (int32_t)x::computeSin(a);
			U16(e, 0) = (uint16_t)(U16(e, 0) + (uint16_t)(mul32(s0, radius) / 0x1000));
			const int32_t c0 = (int32_t)x::computeCosine(a);
			U16(e, 4) = (uint16_t)(U16(e, 4) + (uint16_t)(mul32(c0, radius) / 0x1000));
			const uint32_t r1 = x::CrtRand();
			const uint32_t spd = (r1 & 0x7FF) + 0x400;
			const uint32_t r2 = x::CrtRand();
			const uint32_t up = (r2 & 0x3FF) + 0x200;
			const int32_t speed = (int16_t)(uint16_t)spd;
			const int32_t s1 = (int32_t)x::computeSin(a);
			U16(e, 0x20) = (uint16_t)(mul32(s1, speed) / 0x1000);
			U16(e, 0x22) = (uint16_t)up;
			const int32_t c1 = (int32_t)x::computeCosine(a);
			e += 8;
			U16(e, 0x1C) = (uint16_t)(mul32(c1, speed) / 0x1000);
			angle = (angle + 0x400) & 0xFFF;
		}
		return 0;
	}

	uint32_t __cdecl b9_720EA0(uint32_t a1)
	{
		const uint32_t states[3] = { game_addr(game::sub_720FA0), game_addr(game::sub_720FC0), game_addr(game::nullsub_1110) };
		callp(states[S8(a1, 0x29)], a1);
		for (int i = 0; i < 4; i++)
		{
			b92_particle_step(a1, i);
			b_730C90(a1, 0x2000);
		}
		return b92_task_end(a1);
	}

	uint32_t __cdecl b9_720FA0(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U32(a1, 0x4C) = game_addr(game::unk_152A31C);
		U16(a1, 0x52) = 0xB;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl b9_720FC0(uint32_t a1)
	{
		if (a_743C20(a1) != 0 || a_73B7E0(U16(a1, 0xAE)) != 0)
		{
			U8(a1, 0x26) |= 5;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_7211D0(uint32_t a1)
	{
		const uint32_t states[3] = { game_addr(game::sub_721320), game_addr(game::sub_721360), game_addr(game::nullsub_1113) };
		callp(states[S8(a1, 0x29)], a1);
		a_7458E0(a1);
		return b92_task_end(a1);
	}

	uint32_t __cdecl b9_721320(uint32_t a1)
	{
		const uint32_t model = U32(MEM<uint32_t>(game_addr(game::unk_256D6F0)), 0x144);
		U32(a1, 0x4C) = model;
		U32(a1, 0x30) = 0x1000;
		U32(a1, 0x38) = 0x1000;
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x1E) = 0x1400;
		U16(a1, 0x20) = 0x2000;
		U32(a1, 0x34) = 0x2000;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl b9_721360(uint32_t a1)
	{
		if (a_73B7E0(0xA) != 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_721390(uint32_t a1)
	{
		const uint32_t states[3] = { game_addr(game::sub_7213F0), game_addr(game::sub_721430), game_addr(game::nullsub_1114) };
		callp(states[S8(a1, 0x29)], a1);
		a_7458E0(a1);
		return b92_task_end(a1);
	}

	uint32_t __cdecl b9_7213F0(uint32_t a1)
	{
		const uint32_t model = U32(MEM<uint32_t>(game_addr(game::unk_256D6F0)), 0x148);
		U32(a1, 0x4C) = model;
		U32(a1, 0x38) = 0x1000;
		U32(a1, 0x34) = 0x1000;
		U32(a1, 0x30) = 0x1000;
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x1E) = 0;
		U16(a1, 0x54) = 0x10;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl b9_721430(uint32_t a1)
	{
		U16(a1, 0x54) = (uint16_t)(U16(a1, 0x54) - 1);
		if (S16(a1, 0x54) <= 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_7214C0(uint32_t a1)
	{
		if (a_73B640(0xA) != 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b9_7214E0(uint32_t a1)
	{
		if (a_73B7E0(0xA) != 0)
		{
			a_746AE0();
			a_73BA90();
			x::BdPlaySE(game_addr(game::unk_1529FB0), 0, 0x80);
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_721610(uint32_t a1)
	{
		if (a_73B640(8) != 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b9_721630(uint32_t a1)
	{
		if (a_73B7E0(8) != 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0xAC) = 0x22;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl b9_721660(uint32_t a1)
	{
		U16(a1, 0xAC) = (uint16_t)(U16(a1, 0xAC) - 1);
		if (S16(a1, 0xAC) <= 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	namespace
	{
		inline bool b101_out(int16_t v, int16_t hi) { return v < 0 || v > hi; }
		constexpr int16_t B101_X_LIMIT = 0x140 * 8;
		constexpr int16_t B101_Y_LIMIT = 0xD8 * 8;

		inline void b101_insert(uint32_t ctx, uint32_t a2, uint32_t a3, uint32_t a5, uint32_t pkt)
		{
			int32_t z = add32(S32(ctx, 0x2C), (int32_t)(int16_t)a5);
			S32(ctx, 0x2C) = z;
			if (z < 0x10)
				S32(ctx, 0x2C) = 0x10;
			int32_t sz = S32(ctx, 0x2C) >> (a3 & 31);
			x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)sz * 4, pkt);
		}
	}

	uint32_t __cdecl b10_70D530(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
	{
		uint32_t flags = U32(a1, 0x1C);
		if (!(flags & 0x2000))
			U32(a1, 4) = U32(a1, 0) + 8;
		uint32_t model = U32(a1, 0);
		U32(a1, 0x20) = U32(model, 0) + model;
		if (!(flags & 0x1000))
			U32(a1, 0x18) = 0;
		{
			uint32_t b = U8(a1, 0xA);
			uint32_t g = U8(a1, 9);
			uint32_t r = U8(a1, 8);
			x::someCameraWork_45DD60(r, g, b);
		}
		uint32_t cur = a4;
		if (U32(U32(a1, 0x20), 0) != 0)
			cur = b10_70D680(a1, a2, a3, cur, a5);
		else
			U32(a1, 0x20) = U32(a1, 0x20) + 4;
		if (U32(U32(a1, 0x20), 0) != 0)
			cur = b10_70D8A0(a1, a2, a3, cur, a5);
		else
			U32(a1, 0x20) = U32(a1, 0x20) + 4;
		if (U32(U32(a1, 0x20), 0) != 0)
			cur = b10_70DB40(a1, a2, a3, cur, a5);
		else
			U32(a1, 0x20) = U32(a1, 0x20) + 4;
		if (U32(U32(a1, 0x20), 0) != 0)
			cur = b10_70DDA0(a1, a2, a3, cur, a5);
		else
			U32(a1, 0x20) = U32(a1, 0x20) + 4;
		if (U32(U32(a1, 0x20), 0) != 0)
			cur = b10_70E060(a1, a2, a3, cur, a5);
		else
			U32(a1, 0x20) = U32(a1, 0x20) + 4;
		if (U32(U32(a1, 0x20), 0) != 0)
			cur = b10_70E2A0(a1, a2, a3, cur, a5);
		else
			U32(a1, 0x20) = U32(a1, 0x20) + 4;
		if (U32(U32(a1, 0x20), 0) != 0)
			cur = b10_70E580(a1, a2, a3, cur, a5);
		else
			U32(a1, 0x20) = U32(a1, 0x20) + 4;
		if (U32(U32(a1, 0x20), 0) != 0)
			return b10_70E800(a1, a2, a3, cur, a5);
		U32(a1, 0x20) = U32(a1, 0x20) + 4;
		return cur;
	}

	uint32_t __cdecl b10_70D680(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
	{
		uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t list = U32(ctx, 0x20);
		int32_t count = S32(list, 0);
		uint32_t rec = list + 4;
		U32(ctx, 0x20) = rec;
		uint32_t vtx = U32(ctx, 4);
		if (count <= 0)
		{
			U32(ctx, 0x20) = rec;
			return pkt;
		}
		int32_t n = count;
		do
		{
			x::GTE_LoadV012(vtx + (uint32_t)U16(rec, 4) * 4, vtx + (uint32_t)U16(rec, 6) * 4, vtx + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t fl = U32(ctx, 0x1C);
			uint32_t w0 = U32(rec, 0);
			U32(pkt, 0) = 0x4000000;
			U32(pkt, 4) = w0;
			if (fl & 1)
				U32(pkt, 4) = w0 | 0x2000000;
			if (fl & 4)
				U32(pkt, 4) = U32(pkt, 4) & 0xFDFFFFFF;
			x::GTE_ReadFLAG(ctx + 0x30);
			if (!(U32(ctx, 0x30) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t zero = 0;
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x10))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0xC, pkt + 0x10);
					x::GTE_AVSZ3();
					uint32_t clip = b101_out(S16(pkt, 8), B101_X_LIMIT) ? 1 : zero;
					if (b101_out(S16(pkt, 0xC), B101_X_LIMIT)) clip |= 2;
					if (b101_out(S16(pkt, 0x10), B101_X_LIMIT)) clip |= 4;
					if (b101_out(S16(pkt, 0xA), B101_Y_LIMIT)) clip |= 0x10;
					if (b101_out(S16(pkt, 0xE), B101_Y_LIMIT)) clip |= 0x20;
					if (b101_out(S16(pkt, 0x12), B101_Y_LIMIT)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if (U8(ctx, 0x1C) & 0x40)
						{
							x::set_unk_1CA8A28(pkt + 4);
							x::set_dword_1CA8A30(U32(ctx, 0xC));
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pkt + 4);
						}
						b101_insert(ctx, a2, a3, a5, pkt);
						pkt += 0x14;
					}
				}
			}
			rec += 0xC;
		} while (--n != 0);
		U32(ctx, 0x20) = rec;
		return pkt;
	}

	uint32_t __cdecl b10_70D8A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
	{
		uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t list = U32(ctx, 0x20);
		int32_t count = S32(list, 0);
		uint32_t rec = list + 4;
		U32(ctx, 0x20) = rec;
		uint32_t vtx = U32(ctx, 4);
		if (count <= 0)
		{
			U32(ctx, 0x20) = rec;
			return pkt;
		}
		int32_t n = count;
		do
		{
			x::GTE_LoadV012(vtx + (uint32_t)U16(rec, 4) * 4, vtx + (uint32_t)U16(rec, 6) * 4, vtx + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t fl = U32(ctx, 0x1C);
			uint32_t w0 = U32(rec, 0);
			U32(pkt, 0) = 0x5000000;
			U32(pkt, 4) = w0;
			if (fl & 1)
				U32(pkt, 4) = w0 | 0x2000000;
			if (fl & 4)
				U32(pkt, 4) = U32(pkt, 4) & 0xFDFFFFFF;
			x::GTE_ReadFLAG(ctx + 0x30);
			if (!(U32(ctx, 0x30) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x10))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0xC, pkt + 0x10);
					x::GTE_LoadV0(vtx + (uint32_t)U16(rec, 0xA) * 4);
					x::GTE_RTPS();
					if (b101_out(S16(pkt, 8), B101_X_LIMIT)) clip = 1;
					if (b101_out(S16(pkt, 0xC), B101_X_LIMIT)) clip |= 2;
					if (b101_out(S16(pkt, 0x10), B101_X_LIMIT)) clip |= 4;
					if (b101_out(S16(pkt, 0xA), B101_Y_LIMIT)) clip |= 0x10;
					if (b101_out(S16(pkt, 0xE), B101_Y_LIMIT)) clip |= 0x20;
					if (b101_out(S16(pkt, 0x12), B101_Y_LIMIT)) clip |= 0x40;
					x::GTE_ReadSXY2(pkt + 0x14);
					x::GTE_AVSZ4();
					if (b101_out(S16(pkt, 0x14), B101_X_LIMIT)) clip |= 8;
					if (b101_out(S16(pkt, 0x16), B101_Y_LIMIT)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if (U8(ctx, 0x1C) & 0x40)
						{
							x::set_unk_1CA8A28(pkt + 4);
							x::set_dword_1CA8A30(U32(ctx, 0xC));
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pkt + 4);
						}
						b101_insert(ctx, a2, a3, a5, pkt);
						pkt += 0x18;
					}
				}
			}
			rec += 0xC;
		} while (--n != 0);
		U32(ctx, 0x20) = rec;
		return pkt;
	}

	uint32_t __cdecl b10_70DB40(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
	{
		uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t list = U32(ctx, 0x20);
		int32_t count = S32(list, 0);
		uint32_t rec = list + 4;
		uint32_t vtx = U32(ctx, 4);
		U32(ctx, 0x20) = rec;
		if (count <= 0)
		{
			U32(ctx, 0x20) = rec;
			return pkt;
		}
		int32_t n = count;
		do
		{
			x::GTE_LoadV012(vtx + (uint32_t)U16(rec, 4) * 4, vtx + (uint32_t)U16(rec, 6) * 4, vtx + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t fl = U32(ctx, 0x1C);
			uint32_t w0 = U32(rec, 0);
			U32(pkt, 0) = 0x7000000;
			U32(pkt, 4) = w0;
			if (fl & 1)
				U32(pkt, 4) = w0 | 0x2000000;
			if (fl & 4)
				U32(pkt, 4) = U32(pkt, 4) & 0xFDFFFFFF;
			uint32_t toff = U32(ctx, 0x18);
			uint32_t uv0 = U32(rec, 0xC);
			uint32_t uv1 = U32(rec, 0x10);
			U32(pkt, 0xC) = uv0 + toff;
			uint32_t uv2 = U32(rec, 8) >> 16;
			U32(pkt, 0x14) = uv1 + toff;
			U32(pkt, 0x1C) = uv2 + toff;
			x::GTE_ReadFLAG(ctx + 0x30);
			if (!(U32(ctx, 0x30) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t f2 = U32(ctx, 0x1C);
				if (f2 & 0x400)
					U16(pkt, 0x16) = (uint16_t)(U16(pkt, 0x16) + U16(ctx, 0x10));
				else if (f2 & 0x100)
					U16(pkt, 0x16) = U16(ctx, 0x10);
				if (f2 & 0x800)
					U16(pkt, 0xE) = (uint16_t)(U16(pkt, 0xE) + U16(ctx, 0x14));
				else if (f2 & 0x200)
					U16(pkt, 0xE) = U16(ctx, 0x14);
				uint32_t zero = 0;
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x10))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					x::GTE_AVSZ3();
					uint32_t clip = b101_out(S16(pkt, 8), B101_X_LIMIT) ? 1 : zero;
					if (b101_out(S16(pkt, 0x10), B101_X_LIMIT)) clip |= 2;
					if (b101_out(S16(pkt, 0x18), B101_X_LIMIT)) clip |= 4;
					if (b101_out(S16(pkt, 0xA), B101_Y_LIMIT)) clip |= 0x10;
					if (b101_out(S16(pkt, 0x12), B101_Y_LIMIT)) clip |= 0x20;
					if (b101_out(S16(pkt, 0x1A), B101_Y_LIMIT)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if (U8(ctx, 0x1C) & 0x40)
						{
							x::set_unk_1CA8A28(pkt + 4);
							x::set_dword_1CA8A30(U32(ctx, 0xC));
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pkt + 4);
						}
						b101_insert(ctx, a2, a3, a5, pkt);
						pkt += 0x20;
					}
				}
			}
			rec += 0x14;
		} while (--n != 0);
		U32(ctx, 0x20) = rec;
		return pkt;
	}

	uint32_t __cdecl b10_70DDA0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
	{
		uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t list = U32(ctx, 0x20);
		int32_t count = S32(list, 0);
		uint32_t rec = list + 4;
		uint32_t vtx = U32(ctx, 4);
		U32(ctx, 0x20) = rec;
		if (count <= 0)
		{
			U32(ctx, 0x20) = rec;
			return pkt;
		}
		int32_t n = count;
		do
		{
			x::GTE_LoadV012(vtx + (uint32_t)U16(rec, 4) * 4, vtx + (uint32_t)U16(rec, 6) * 4, vtx + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t fl = U32(ctx, 0x1C);
			uint32_t w0 = U32(rec, 0);
			U32(pkt, 0) = 0x9000000;
			U32(pkt, 4) = w0;
			if (fl & 1)
				U32(pkt, 4) = w0 | 0x2000000;
			if (fl & 4)
				U32(pkt, 4) = U32(pkt, 4) & 0xFDFFFFFF;
			uint32_t toff = U32(ctx, 0x18);
			uint32_t uv0 = U32(rec, 0xC);
			uint32_t uv1 = U32(rec, 0x10);
			U32(pkt, 0xC) = uv0 + toff;
			uint32_t w1c = (toff << 16) + toff;
			uv1 += toff;
			w1c += U32(rec, 0x14);
			U32(pkt, 0x1C) = w1c;
			U32(pkt, 0x14) = uv1;
			U32(pkt, 0x24) = w1c >> 16;
			x::GTE_ReadFLAG(ctx + 0x30);
			if (!(U32(ctx, 0x30) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t f2 = U32(ctx, 0x1C);
				if (f2 & 0x400)
					U16(pkt, 0x16) = (uint16_t)(U16(pkt, 0x16) + U16(ctx, 0x10));
				else if (f2 & 0x100)
					U16(pkt, 0x16) = U16(ctx, 0x10);
				if (f2 & 0x800)
					U16(pkt, 0xE) = (uint16_t)(U16(pkt, 0xE) + U16(ctx, 0x14));
				else if (f2 & 0x200)
					U16(pkt, 0xE) = U16(ctx, 0x14);
				uint32_t zero = 0;
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x10))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					x::GTE_LoadV0(vtx + (uint32_t)U16(rec, 0xA) * 4);
					x::GTE_RTPS();
					uint32_t clip = b101_out(S16(pkt, 8), B101_X_LIMIT) ? 1 : zero;
					if (b101_out(S16(pkt, 0x10), B101_X_LIMIT)) clip |= 2;
					if (b101_out(S16(pkt, 0x18), B101_X_LIMIT)) clip |= 4;
					if (b101_out(S16(pkt, 0xA), B101_Y_LIMIT)) clip |= 0x10;
					if (b101_out(S16(pkt, 0x12), B101_Y_LIMIT)) clip |= 0x20;
					if (b101_out(S16(pkt, 0x1A), B101_Y_LIMIT)) clip |= 0x40;
					x::GTE_ReadSXY2(pkt + 0x20);
					x::GTE_AVSZ4();
					if (b101_out(S16(pkt, 0x20), B101_X_LIMIT)) clip |= 8;
					if (b101_out(S16(pkt, 0x22), B101_Y_LIMIT)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if (U8(ctx, 0x1C) & 0x40)
						{
							x::set_unk_1CA8A28(pkt + 4);
							x::set_dword_1CA8A30(U32(ctx, 0xC));
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pkt + 4);
						}
						b101_insert(ctx, a2, a3, a5, pkt);
						pkt += 0x28;
					}
				}
			}
			rec += 0x18;
		} while (--n != 0);
		U32(ctx, 0x20) = rec;
		return pkt;
	}

	uint32_t __cdecl b10_70E060(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
	{
		uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t list = U32(ctx, 0x20);
		int32_t count = S32(list, 0);
		uint32_t rec = list + 4;
		uint32_t vtx = U32(ctx, 4);
		U32(ctx, 0x20) = rec;
		if (count <= 0)
		{
			U32(ctx, 0x20) = rec;
			return pkt;
		}
		int32_t n = count;
		do
		{
			x::GTE_LoadV012(vtx + (uint32_t)U16(rec, 4) * 4, vtx + (uint32_t)U16(rec, 6) * 4, vtx + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t fl = U32(ctx, 0x1C);
			uint32_t w0 = U32(rec, 0);
			U32(pkt, 0) = 0x6000000;
			U32(pkt, 4) = w0;
			if (fl & 2)
				U32(pkt, 4) = w0 | 0x2000000;
			if (fl & 8)
				U32(pkt, 4) = U32(pkt, 4) & 0xFDFFFFFF;
			x::GTE_ReadFLAG(ctx + 0x30);
			if (!(U32(ctx, 0x30) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t zero = 0;
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					x::GTE_AVSZ3();
					uint32_t clip = b101_out(S16(pkt, 8), B101_X_LIMIT) ? 1 : zero;
					if (b101_out(S16(pkt, 0x10), B101_X_LIMIT)) clip |= 2;
					if (b101_out(S16(pkt, 0x18), B101_X_LIMIT)) clip |= 4;
					if (b101_out(S16(pkt, 0xA), B101_Y_LIMIT)) clip |= 0x10;
					if (b101_out(S16(pkt, 0x12), B101_Y_LIMIT)) clip |= 0x20;
					if (b101_out(S16(pkt, 0x1A), B101_Y_LIMIT)) clip |= 0x40;
					if ((clip & 7) != 7 && (clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if (U8(ctx, 0x1C) & 0x80)
						{
							x::sub_45E120(rec + 0xC, rec + 0x10, pkt + 4);
							x::set_dword_1CA8A30(U32(ctx, 0xC));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0xC, pkt + 0x14, pkt + 4);
						}
						else
						{
							U32(pkt, 0xC) = U32(rec, 0xC);
							U32(pkt, 0x14) = U32(rec, 0x10);
						}
						b101_insert(ctx, a2, a3, a5, pkt);
						pkt += 0x1C;
					}
				}
			}
			rec += 0x14;
		} while (--n != 0);
		U32(ctx, 0x20) = rec;
		return pkt;
	}

	uint32_t __cdecl b10_70E2A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
	{
		uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t list = U32(ctx, 0x20);
		int32_t count = S32(list, 0);
		uint32_t rec = list + 4;
		uint32_t vtx = U32(ctx, 4);
		U32(ctx, 0x20) = rec;
		if (count <= 0)
		{
			U32(ctx, 0x20) = rec;
			return pkt;
		}
		int32_t n = count;
		do
		{
			x::GTE_LoadV012(vtx + (uint32_t)U16(rec, 4) * 4, vtx + (uint32_t)U16(rec, 6) * 4, vtx + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t fl = U32(ctx, 0x1C);
			uint32_t w0 = U32(rec, 0);
			U32(pkt, 0) = 0x8000000;
			U32(pkt, 4) = w0;
			if (fl & 2)
				U32(pkt, 4) = w0 | 0x2000000;
			if (fl & 8)
				U32(pkt, 4) = U32(pkt, 4) & 0xFDFFFFFF;
			x::GTE_ReadFLAG(ctx + 0x30);
			if (!(U32(ctx, 0x30) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x24);
				if (S32(ctx, 0x24) >= 0 || (U8(ctx, 0x1C) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					x::GTE_LoadV0(vtx + (uint32_t)U16(rec, 0xA) * 4);
					x::GTE_RTPS();
					if (b101_out(S16(pkt, 8), B101_X_LIMIT)) clip = 1;
					if (b101_out(S16(pkt, 0x10), B101_X_LIMIT)) clip |= 2;
					if (b101_out(S16(pkt, 0x18), B101_X_LIMIT)) clip |= 4;
					if (b101_out(S16(pkt, 0xA), B101_Y_LIMIT)) clip |= 0x10;
					if (b101_out(S16(pkt, 0x12), B101_Y_LIMIT)) clip |= 0x20;
					if (b101_out(S16(pkt, 0x1A), B101_Y_LIMIT)) clip |= 0x40;
					x::GTE_ReadSXY2(pkt + 0x20);
					x::GTE_AVSZ4();
					if (b101_out(S16(pkt, 0x20), B101_X_LIMIT)) clip |= 8;
					if (b101_out(S16(pkt, 0x22), B101_Y_LIMIT)) clip |= 0x80;
					if ((clip & 0xF) != 0xF && (clip & 0xF0) != 0xF0)
					{
						x::GTE_ReadOTZ(ctx + 0x2C);
						if (U8(ctx, 0x1C) & 0x80)
						{
							x::sub_45E120(rec + 0xC, rec + 0x10, rec + 0x14);
							x::set_dword_1CA8A30(U32(ctx, 0xC));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0xC, pkt + 0x14, pkt + 0x1C);
							x::set_unk_1CA8A28(pkt + 4);
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pkt + 4);
						}
						else
						{
							U32(pkt, 0xC) = U32(rec, 0xC);
							U32(pkt, 0x14) = U32(rec, 0x10);
							U32(pkt, 0x1C) = U32(rec, 0x14);
						}
						b101_insert(ctx, a2, a3, a5, pkt);
						pkt += 0x24;
					}
				}
			}
			rec += 0x18;
		} while (--n != 0);
		U32(ctx, 0x20) = rec;
		return pkt;
	}

	namespace
	{
		inline uint32_t b102_task_end(uint32_t node, uint8_t status)
		{
			if ((status & 1) != 0 && U8(node, 0x28) == 0)
			{
				x::Effect_ReleaseLinkedTask(node);
				return 2;
			}
			return 0;
		}

		inline bool b102_out(int16_t v, int16_t lim)
		{
			return v < 0 || v > lim;
		}
	}

	uint32_t __cdecl b10_70E580(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
	{
		const uint32_t hdr = a1;
		uint32_t pkt = a4;
		const uint32_t list = U32(hdr, 0x20);
		int32_t count = S32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t verts = U32(hdr, 4);
		U32(hdr, 0x20) = rec;
		if (count <= 0)
		{
			U32(hdr, 0x20) = rec;
			return pkt;
		}
		do
		{
			{
				const uint32_t v2 = verts + U16(rec, 8) * 4u;
				const uint32_t v1 = verts + U16(rec, 6) * 4u;
				const uint32_t v0 = verts + U16(rec, 4) * 4u;
				x::GTE_LoadV012(v0, v1, v2);
			}
			x::GTE_RTPT();
			{
				const uint32_t fl = U32(hdr, 0x1C);
				const uint32_t c0 = U32(rec, 0);
				U32(pkt, 0) = 0x9000000;
				U32(pkt, 4) = c0;
				if ((fl & 2) != 0)
					U32(pkt, 4) = c0 | 0x2000000;
				if ((fl & 8) != 0)
					U32(pkt, 4) &= 0xFDFFFFFF;
			}
			{
				const uint32_t uvo = U32(hdr, 0x18);
				U32(pkt, 0xC) = U32(rec, 0xC) + uvo;
				U32(pkt, 0x18) = U32(rec, 0x10) + uvo;
				U32(pkt, 0x24) = (U32(rec, 8) >> 16) + uvo;
			}
			x::GTE_ReadFLAG(hdr + 0x30);
			if ((U32(hdr, 0x30) & 0x60000) == 0)
			{
				x::GTE_NCLIP();
				const uint32_t fl = U32(hdr, 0x1C);
				if ((fl & 0x400) != 0)
					U16(pkt, 0x1A) = (uint16_t)(U16(pkt, 0x1A) + U16(hdr, 0x10));
				else if ((fl & 0x100) != 0)
					U16(pkt, 0x1A) = U16(hdr, 0x10);
				if ((fl & 0x800) != 0)
					U16(pkt, 0xE) = (uint16_t)(U16(pkt, 0xE) + U16(hdr, 0x14));
				else if ((fl & 0x200) != 0)
					U16(pkt, 0xE) = U16(hdr, 0x14);
				const uint32_t clip_init = 0;
				x::GTE_ReadMAC0(hdr + 0x24);
				if (S32(hdr, 0x24) >= 0 || (U8(hdr, 0x1C) & 0x20) != 0)
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x14, pkt + 0x20);
					x::GTE_AVSZ3();
					uint32_t clip = b102_out(S16(pkt, 8), 0x140) ? 1u : clip_init;
					if (b102_out(S16(pkt, 0x14), 0x140))
						clip |= 2;
					if (b102_out(S16(pkt, 0x20), 0x140))
						clip |= 4;
					if (b102_out(S16(pkt, 0xA), 0xD8))
						clip |= 0x10;
					if (b102_out(S16(pkt, 0x16), 0xD8))
						clip |= 0x20;
					if (b102_out(S16(pkt, 0x22), 0xD8))
						clip |= 0x40;
					if ((uint8_t)(clip & 7) != 7 && (uint8_t)(clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(hdr + 0x2C);
						if ((U8(hdr, 0x1C) & 0x80) != 0)
						{
							x::sub_45E120(rec + 0x14, rec + 0x18, pkt + 4);
							x::set_dword_1CA8A30(U32(hdr, 0xC));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0x10, pkt + 0x1C, pkt + 4);
						}
						else
						{
							U32(pkt, 0x10) = U32(rec, 0x14);
							U32(pkt, 0x1C) = U32(rec, 0x18);
						}
						const int32_t z = add32(S32(hdr, 0x2C), (int32_t)(int16_t)a5);
						S32(hdr, 0x2C) = z;
						if (z < 0x10)
							S32(hdr, 0x2C) = 0x10;
						const int32_t zz = S32(hdr, 0x2C) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)zz * 4u, pkt);
						pkt += 0x28;
					}
				}
			}
			rec += 0x1C;
		} while (--count != 0);
		U32(hdr, 0x20) = rec;
		return pkt;
	}

	uint32_t __cdecl b10_70E800(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
	{
		const uint32_t hdr = a1;
		uint32_t pkt = a4;
		const uint32_t list = U32(hdr, 0x20);
		int32_t count = S32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t verts = U32(hdr, 4);
		U32(hdr, 0x20) = rec;
		if (count <= 0)
		{
			U32(hdr, 0x20) = rec;
			return pkt;
		}
		do
		{
			{
				const uint32_t v2 = verts + U16(rec, 8) * 4u;
				const uint32_t v1 = verts + U16(rec, 6) * 4u;
				const uint32_t v0 = verts + U16(rec, 4) * 4u;
				x::GTE_LoadV012(v0, v1, v2);
			}
			x::GTE_RTPT();
			{
				const uint32_t fl = U32(hdr, 0x1C);
				const uint32_t c0 = U32(rec, 0);
				U32(pkt, 0) = 0xC000000;
				U32(pkt, 4) = c0;
				if ((fl & 2) != 0)
					U32(pkt, 4) = c0 | 0x2000000;
				if ((fl & 8) != 0)
					U32(pkt, 4) &= 0xFDFFFFFF;
			}
			{
				const uint32_t uvo = U32(hdr, 0x18);
				U32(pkt, 0xC) = U32(rec, 0xC) + uvo;
				const uint32_t uv1 = U32(rec, 0x10) + uvo;
				const uint32_t uv23 = (uvo << 16) + uvo + U32(rec, 0x14);
				U32(pkt, 0x24) = uv23;
				U32(pkt, 0x18) = uv1;
				U32(pkt, 0x30) = uv23 >> 16;
			}
			x::GTE_ReadFLAG(hdr + 0x30);
			if ((U32(hdr, 0x30) & 0x60000) == 0)
			{
				x::GTE_NCLIP();
				const uint32_t fl = U32(hdr, 0x1C);
				if ((fl & 0x400) != 0)
					U16(pkt, 0x1A) = (uint16_t)(U16(pkt, 0x1A) + U16(hdr, 0x10));
				else if ((fl & 0x100) != 0)
					U16(pkt, 0x1A) = U16(hdr, 0x10);
				if ((fl & 0x800) != 0)
					U16(pkt, 0xE) = (uint16_t)(U16(pkt, 0xE) + U16(hdr, 0x14));
				else if ((fl & 0x200) != 0)
					U16(pkt, 0xE) = U16(hdr, 0x14);
				const uint32_t clip_init = 0;
				x::GTE_ReadMAC0(hdr + 0x24);
				if (S32(hdr, 0x24) >= 0 || (U8(hdr, 0x1C) & 0x20) != 0)
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x14, pkt + 0x20);
					x::GTE_LoadV0(verts + U16(rec, 0xA) * 4u);
					x::GTE_RTPS();
					uint32_t clip = b102_out(S16(pkt, 8), 0x140) ? 1u : clip_init;
					if (b102_out(S16(pkt, 0x14), 0x140))
						clip |= 2;
					if (b102_out(S16(pkt, 0x20), 0x140))
						clip |= 4;
					if (b102_out(S16(pkt, 0xA), 0xD8))
						clip |= 0x10;
					if (b102_out(S16(pkt, 0x16), 0xD8))
						clip |= 0x20;
					if (b102_out(S16(pkt, 0x22), 0xD8))
						clip |= 0x40;
					x::GTE_ReadSXY2(pkt + 0x2C);
					x::GTE_AVSZ4();
					if (b102_out(S16(pkt, 0x2C), 0x140))
						clip |= 8;
					if (b102_out(S16(pkt, 0x2E), 0xD8))
						clip |= 0x80;
					if ((uint8_t)(clip & 0xF) != 0xF && (uint8_t)(clip & 0xF0) != 0xF0)
					{
						x::GTE_ReadOTZ(hdr + 0x2C);
						if ((U8(hdr, 0x1C) & 0x80) != 0)
						{
							x::sub_45E120(rec + 0x18, rec + 0x1C, rec + 0x20);
							x::set_dword_1CA8A30(U32(hdr, 0xC));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0x10, pkt + 0x1C, pkt + 0x28);
							x::set_unk_1CA8A28(pkt + 4);
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pkt + 4);
						}
						else
						{
							U32(pkt, 0x10) = U32(rec, 0x18);
							U32(pkt, 0x1C) = U32(rec, 0x1C);
							U32(pkt, 0x28) = U32(rec, 0x20);
						}
						const int32_t z = add32(S32(hdr, 0x2C), (int32_t)(int16_t)a5);
						S32(hdr, 0x2C) = z;
						if (z < 0x10)
							S32(hdr, 0x2C) = 0x10;
						const int32_t zz = S32(hdr, 0x2C) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)zz * 4u, pkt);
						pkt += 0x34;
					}
				}
			}
			rec += 0x24;
		} while (--count != 0);
		U32(hdr, 0x20) = rec;
		return pkt;
	}

	uint32_t __cdecl b10_70EDE0(uint32_t a1)
	{
		const uint32_t node = a1;
		b_718670();
		b10_70EE30();
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2565868), game_addr(game::MAG_100_sub_70EF30), 0x30, node);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_255EBB0), game_addr(game::MAG_100_ParticleDirectorTask), 0x48, node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b10_70EE30(void)
	{
		MEM<uint32_t>(game_addr(game::unk_25642E0)) = game_addr(game::unk_1529B70);
		MEM<uint32_t>(game_addr(game::unk_25653D8)) = game_addr(game::unk_25658C0);
		x::MAG_007_sub_8DCC00(game_addr(game::unk_25658C0), 0x16C);
		const uint32_t c = MEM<uint32_t>(game_addr(game::unk_25653D8));
		const uint32_t arena = MEM<uint32_t>(game_addr(game::unk_2565888));
		U32(c, 0x100) = arena;
		MEM<uint32_t>(game_addr(game::unk_2565888)) = arena + 0xA000;
		const uint32_t dflt = game_addr(game::unk_19D5018);
		U32(c, 0x114) = dflt;
		U32(c, 0x118) = game_addr(game::unk_19E8678);
		U32(c, 0x11C) = dflt;
		U32(c, 0x120) = dflt;
		U32(c, 0x104) = dflt;
		U32(c, 0x108) = game_addr(game::unk_19E3018);
		U32(c, 0x10C) = dflt;
		U32(c, 0x110) = dflt;
		U32(c, 0x124) = dflt;
		U32(c, 0x128) = game_addr(game::unk_19EF018);
		U32(c, 0x12C) = game_addr(game::unk_19E4970);
		U32(c, 0x130) = game_addr(game::unk_19DC678);
		for (int32_t o = 0x134; o <= 0x160; o += 4)
			U32(c, o) = dflt;
		U16(c, 0x164) = 0;
		U16(c, 0x166) = 0;
		return c;
	}

	uint32_t __cdecl b10_70F370(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[24];
		states[0] = game_addr(game::MAG_100_SetupBokoCreature);
		states[1] = game_addr(game::sub_715410);
		states[2] = game_addr(game::sub_715470);
		states[3] = game_addr(game::MAG_100_SpawnBokoCreature);
		states[4] = game_addr(game::sub_715E30);
		states[5] = game_addr(game::sub_715E50);
		states[6] = game_addr(game::sub_715E80);
		states[7] = game_addr(game::sub_715EA0);
		states[8] = game_addr(game::sub_715EC0);
		states[9] = game_addr(game::sub_715EE0);
		states[10] = game_addr(game::sub_715F00);
		states[11] = game_addr(game::sub_715F20);
		states[12] = game_addr(game::sub_715F40);
		states[13] = game_addr(game::sub_715F60);
		states[14] = game_addr(game::sub_715F80);
		states[15] = game_addr(game::sub_715FC0);
		states[16] = game_addr(game::sub_715FE0);
		states[17] = game_addr(game::sub_716000);
		states[18] = game_addr(game::sub_716020);
		states[19] = game_addr(game::sub_716040);
		states[20] = game_addr(game::sub_716070);
		states[21] = game_addr(game::sub_7160A0);
		states[22] = game_addr(game::sub_7160C0);
		states[23] = game_addr(game::nullsub_1076);
		b10_70F480(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return b102_task_end(node, status);
	}

	uint32_t __cdecl b10_70F480(uint32_t a1)
	{
		uint32_t d = MEM<uint32_t>(game_addr(game::unk_15297F0));
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		uint16_t v = U16(d, 0x42);
		if (U16(d, 0x40) != v)
		{
			U16(d, 0x40) = v;
			U16(d, 0x46) = 0;
			b10_716120(a1);
			d = MEM<uint32_t>(game_addr(game::unk_15297F0));
		}
		v = U16(d, 0x44);
		if (U16(d, 0x42) != v)
		{
			U16(d, 0x42) = v;
			a_73A6D0();
		}
		return 0;
	}

	uint32_t __cdecl b10_70F7D0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		alignas(4) uint8_t loc[0x48] = {};
		const uint32_t L = P(loc);
		const uint32_t M = L + 0x28;
		const uint32_t rec = a2;
		const uint32_t blk = a3;
		if (((uint32_t)(int32_t)S16(rec, 0x1C) | U32(rec, 0x18)) == 0)
			return 0;
		if (S16(rec, 0x24) >= 0x1000 && U32(rec, 0x20) == 0)
			return 0;

		const uint32_t hdr = x::Field_Alloc(0x58);
		const int32_t model = S16(rec, 2);
		const uint32_t data = U32(a1, 0);
		const uint32_t base = data + U32(data, model * 4 + 8);
		const int16_t f1 = S16(rec, 0x2A);
		const int16_t f0 = S16(rec, 0x28);
		U32(hdr, 0) = base;
		auto frame_ptr = [base](int16_t f) -> uint32_t {
			if (f == 0)
				return base + 0xC;
			const int32_t n = mul32(S32(base, 4), (int32_t)f);
			return base + (uint32_t)shl32(n, 3) + 0xC;
		};
		if (f0 == f1)
			U32(hdr, 4) = frame_ptr(f0);
		else
		{
			const int16_t t = S16(rec, 0x26);
			if (t == 0)
				U32(hdr, 4) = frame_ptr(f0);
			else if (t == 0x1000)
				U32(hdr, 4) = frame_ptr(f1);
			else
			{
				const uint32_t out = U32(blk, 0x48);
				x::MAG_017_sub_701390(base, (uint32_t)(int32_t)f0, (uint32_t)(int32_t)f1, (uint32_t)(int32_t)t, out);
				U32(hdr, 4) = U32(blk, 0x48);
			}
		}

		if ((U32(rec, 4) & 0x40000) != 0)
			xm::MAG_063_sub_7015B0(rec + 0x10, M);
		else
			x::MAG_017_sub_701310(rec + 0x10, M);
		const uint16_t vx = U16(rec, 8);
		const uint16_t vy = U16(rec, 0xA);
		const uint16_t vz = U16(rec, 0xC);
		U16(L, 0) = vx;
		U16(L, 2) = vy;
		U16(L, 4) = vz;
		if ((U8(rec, 5) & 2) != 0)
		{
			S32(M, 0x14) = (int16_t)vx;
			S32(M, 0x18) = (int16_t)vy;
			S32(M, 0x1C) = (int16_t)vz;
		}
		else
		{
			x::GTE_SetRotMatrix(blk);
			x::GTE_LoadV0(L);
			x::GTE_MVMVA_RotV0();
			x::GTE_ReadMAC123(M + 0x14);
			if ((U32(rec, 4) & 0x8000) == 0)
				x::GTE_MatrixMultiply(blk, M);
		}
		{
			const int32_t bx = S32(blk, 0x14);
			const int32_t t0 = S32(M, 0x14);
			const int32_t t1 = S32(M, 0x18);
			S32(M, 0x14) = add32(t0, bx);
			const int32_t by = S32(blk, 0x18);
			S32(M, 0x18) = add32(t1, by);
			const int32_t t2 = S32(M, 0x1C);
			const int32_t bz = S32(blk, 0x1C);
			S32(M, 0x1C) = add32(t2, bz);
		}
		if (U32(rec, 0x18) != 0x10001000 || U16(rec, 0x1C) != 0x1000)
		{
			if ((U32(rec, 4) & 0x100) != 0)
			{
				U16(L, 0x08) = U16(rec, 0x18);
				U16(L, 0x10) = U16(rec, 0x1A);
				U16(L, 0x18) = U16(rec, 0x1C);
				U16(L, 0x12) = 0;
				U16(L, 0x0C) = 0;
				U16(L, 0x16) = 0;
				U16(L, 0x0A) = 0;
				U16(L, 0x14) = 0;
				U16(L, 0x0E) = 0;
				x::sub_56C220(M, L + 8);
			}
			else
			{
				S32(L, 0x08) = S16(rec, 0x18);
				S32(L, 0x0C) = S16(rec, 0x1A);
				S32(L, 0x10) = S16(rec, 0x1C);
				x::scale3DMatrix(M, L + 8);
			}
		}
		x::GTE_SetRotMatrix_W(M);
		x::GTE_SetTransVector_W(M);
		const uint32_t flags = U32(rec, 4);
		U32(hdr, 0x1C) = (flags & 0x4000) != 0 ? 0x2000u : 0x2030u;
		const int32_t alpha = S16(rec, 0x24);
		U32(hdr, 0xC) = (uint32_t)alpha;
		if (alpha != 0)
		{
			U32(hdr, 0x1C) |= 0xC0;
			U32(hdr, 8) = U32(rec, 0x20);
		}
		const uint16_t groups = U16(blk, 0x54);
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		uint32_t next;
		if (groups == 0)
			next = x::Effect_RenderPrimModel(hdr, ot, 2, cursor);
		else
			next = b10_70D530(hdr, ot, 2, cursor, (blk & 0xFFFF0000u) | groups);
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = next;
		x::Field_Free(0x58);
		return 0;
	}

	uint32_t __cdecl b10_7148C0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[14];
		states[0] = game_addr(game::sub_714970);
		states[1] = game_addr(game::sub_715170);
		states[2] = game_addr(game::sub_715190);
		states[3] = game_addr(game::sub_7151E0);
		states[4] = game_addr(game::sub_715210);
		states[5] = game_addr(game::sub_715250);
		states[6] = game_addr(game::sub_715280);
		states[7] = game_addr(game::sub_7152C0);
		states[8] = game_addr(game::sub_7152E0);
		states[9] = game_addr(game::sub_715330);
		states[10] = game_addr(game::sub_715380);
		states[11] = game_addr(game::sub_7153B0);
		states[12] = game_addr(game::sub_7153D0);
		states[13] = game_addr(game::nullsub_1073);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return b102_task_end(node, status);
	}

	uint32_t __cdecl b10_715210(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(5) != 0)
		{
			a_73AB20(game_addr(game::unk_1529DB0), MEM<uint32_t>(game_addr(game::unk_15297F0)) + 8, 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b10_715250(uint32_t a1)
	{
		if (a_73AAE0(6) != 0)
		{
			a_73AB20(game_addr(game::unk_1529DF8), MEM<uint32_t>(game_addr(game::unk_15297F0)) + 8, 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b10_715280(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(8) != 0)
		{
			a_73AB20(game_addr(game::unk_1529E40), MEM<uint32_t>(game_addr(game::unk_15297F0)) + 8, 0);
			U16(a1, 0x6C) = 9;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b10_7152C0(uint32_t a1)
	{
		a_73AE10();
		U16(a1, 0x6C) = (uint16_t)(U16(a1, 0x6C) - 1);
		if (S16(a1, 0x6C) <= 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b10_7152E0(uint32_t a1)
	{
		a_73AE10();
		if (a_73B640(9) != 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b10_715330(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(9) != 0)
		{
			a_73AB20(game_addr(game::unk_1529E88), MEM<uint32_t>(game_addr(game::unk_15297F0)) + 8, 0);
			x::BdPlaySE(game_addr(game::unk_1529800), 0, 0x80);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl b10_7153B0(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(0xA) != 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b10_715540(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[11];
		states[0] = game_addr(game::sub_7159A0);
		states[1] = game_addr(game::sub_715A40);
		states[2] = game_addr(game::sub_715A70);
		states[3] = game_addr(game::sub_715CD0);
		states[4] = game_addr(game::sub_715D40);
		states[5] = game_addr(game::sub_715D60);
		states[6] = game_addr(game::sub_715D80);
		states[7] = game_addr(game::sub_715DB0);
		states[8] = game_addr(game::sub_715DD0);
		states[9] = game_addr(game::sub_715DF0);
		states[10] = game_addr(game::nullsub_1075);
		callp(states[S8(node, 0x29)], node);
		if ((U8(node, 0x26) & 4) == 0)
		{
			const uint32_t cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = a_746C10(node, game_addr(game::unk_25642E8), cursor);
			a_739890(node);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return b102_task_end(node, status);
	}

	uint32_t __cdecl b10_715DB0(uint32_t a1)
	{
		x::au_re_Battle_ReadAnimation_8(a1);
		if (S16(a1, 0x136) >= 0x10)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b10_715DD0(uint32_t a1)
	{
		x::au_re_Battle_ReadAnimation_8(a1);
		if (a_73B640(5) != 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl b10_715DF0(uint32_t a1)
	{
		x::au_re_Battle_ReadAnimation_8(a1);
		if (a_73B7E0(5) != 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 5;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	namespace
	{
		inline uint32_t b103_entity_of(uint32_t node) { return game_addr(game::BattleEntitySlotData) + (uint32_t)U8(node, 0x2D) * 0x9Cu; }
		inline void b103_next_state(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }
	}

	uint32_t __cdecl b10_715F80(uint32_t a1)
	{
		if (a_73B7E0(6) == 0)
			return 0;
		U16(a1, 0x44) = U16(a1, 0x24);
		x::BdPlaySE(game_addr(game::unk_15297FC), 0, 0x80);
		b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b10_716040(uint32_t a1)
	{
		if (a_73B7E0(8) == 0)
			return 0;
		U16(a1, 0x44) = 0x64;
		b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b10_7160C0(uint32_t a1)
	{
		if (a_73B640(0xA) == 0)
			return 0;
		x::sub_4A2940((uint32_t)(int32_t)S16(U32(game_addr(game::unk_15297F0), 0), 0x52));
		U8(a1, 0x26) |= 1;
		b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b10_716120(uint32_t a1)
	{
		uint32_t dir = U32(game_addr(game::unk_15297F0), 0);
		int32_t step = (int32_t)S16(dir, 0x40);
		uint32_t d;
		uint32_t t;
		switch (step)
		{
		case 2:
			d = U32(game_addr(game::unk_25653D8), 0);
			b_72A610(a1, game_addr(game::sub_7163D0), U32(d, 0x128), 0x44C, 1, 0);
			return x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_255ECB8), game_addr(game::sub_716A60), 0x70, a1);
		case 4:
			d = U32(game_addr(game::unk_25653D8), 0);
			b_72A610(a1, game_addr(game::sub_716840), U32(d, 0x12C), 0x170, 2, 0);
			return x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_255ECB8), game_addr(game::sub_716B50), 0x70, a1);
		case 6:
			d = U32(game_addr(game::unk_25653D8), 0);
			a_73C100(a1, game_addr(game::sub_70FAF0), U32(d, 0x108), 0, 0x2D, 2);
			t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_25656B8), game_addr(game::sub_7175F0), 0x140, a1);
			d = U32(game_addr(game::unk_25653D8), 0);
			return x::Effect_BindModelContainerSetAnim(t, U32(d, 0x118), 0x3C);
		case 8:
			d = U32(game_addr(game::unk_25653D8), 0);
			b_72A610(a1, game_addr(game::sub_716950), U32(d, 0x130), 0x32C, 3, 0);
			t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2564158), game_addr(game::sub_716300), 0x40, a1);
			d = U32(game_addr(game::unk_25653D8), 0);
			U32(t, 0x34) = game_addr(game::unk_1529F70);
			U16(t, 0x38) = 0xF;
			return a_73C100(a1, game_addr(game::sub_70FAF0), U32(d, 0x10C), 0xF, 0x2D, 2);
		case 9:
		{
			int32_t act = (int32_t)S8(a1, 0x2A);
			uint32_t acts = U32(U32(a1, 0xC), 4);
			if (U8(acts + (uint32_t)(act * 20), 0x10) == 0)
				return acts;
			int32_t i = 0;
			uint32_t off = 0;
			for (;;)
			{
				t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::unk_2561890), game_addr(game::Boko100_TargetWobbleTask), 0xD0, a1);
				int32_t a = (int32_t)S8(a1, 0x2A);
				uint32_t rec = U32(U32(a1, 0xC), 4) + (uint32_t)(a * 20);
				i++;
				off += 0x18;
				U8(t, 0x2D) = U8(U32(rec, 8) + off, -0x18);
				uint32_t n = U8(rec, 0x10);
				if (i >= (int32_t)n)
					return n;
			}
		}
		default:
			return (uint32_t)(step - 2);
		}
	}

	uint32_t __cdecl b10_716910(uint32_t a1)
	{
		if (a_73B7E0(5) != 0)
		{
			U8(a1, 0x26) |= 1;
			b103_next_state(a1);
			return 0;
		}
		return b_72A720(a1);
	}

	uint32_t __cdecl b10_7169B0(uint32_t a1)
	{
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		uint32_t dir = U32(game_addr(game::unk_15297F0), 0);
		U16(a1, 0x60) = 0x480;
		U32(a1, 0x58) = 0x2800;
		uint32_t p0 = U32(dir, 8);
		uint32_t p1 = U32(dir, 0xC);
		U32(a1, 0x1C) = p0;
		U32(a1, 0x20) = p1;
		U16(a1, 0x1E) = (uint16_t)(U16(a1, 0x1E) + 0xA800);
		U16(a1, 0x20) = (uint16_t)(U16(a1, 0x20) + 0x400);
		U32(a1, 0x50) = 0x1000;
		U32(a1, 0x54) = 0x1000;
		U16(a1, 0x86) = 0xF000;
		b_72A720(a1);
		b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b10_716A20(uint32_t a1)
	{
		if (a_73B7E0(9) != 0)
		{
			U8(a1, 0x26) |= 1;
			b103_next_state(a1);
			return 0;
		}
		return b_72A720(a1);
	}

	uint32_t __cdecl b10_716BB0(uint32_t a1)
	{
		U16(a1, 0x1C) = 0x600;
		b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b10_716C20(uint32_t a1)
	{
		uint32_t tab[7] = { game_addr(game::sub_717330), game_addr(game::sub_717350), game_addr(game::sub_7174B0), game_addr(game::sub_7174D0), game_addr(game::sub_717540), game_addr(game::sub_717590), game_addr(game::nullsub_1084) };
		callp(tab[S8(a1, 0x29)], a1);
		if (U8(a1, 0x26) & 8)
		{
			uint32_t r = a1 + 0x6C;
			U16(r, 0x54) = (uint16_t)((U16(a1, 0xC0) + 0x180) & 0xFFF);
			b10_716CD0(r);
		}
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		if ((U8(a1, 0x26) & 1) && U8(a1, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(a1);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl b10_716CD0(uint32_t a1)
	{
		uint32_t ent = U32(a1, 0x48);
		ff8fx::copy<(0x20)>((void *)a1, (const void *)(ent + 0x40));
		uint32_t pose = ent + 0x60;
		x::BattleModel_BuildBoneMatricesFromPose(pose);
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(a1);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(a1 + 0x20);
		x::GTE_LoadIRFromMatrixColumn(a1 + 2);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(a1 + 0x22);
		x::GTE_LoadIRFromMatrixColumn(a1 + 4);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(a1 + 0x24);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(a1 + 0x14);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(a1 + 0x34);
		uint32_t cp = U32(a1, 0x4C);
		uint32_t ot = U32(game_addr(game::g_Battle_FrameRenderListBase), 0) + 0x4040;
		uint32_t cur = U32(cp, 0);
		uint32_t r = x::sub_5088A0(ent, ot, 0x10, cur);
		cp = U32(a1, 0x4C);
		uint32_t ot44 = U32(game_addr(game::g_Battle_FrameRenderListBase), 0) + 0x44;
		U32(cp, 0) = r;
		b10_716DD0(U32(ent, 0x64), ot44, 4, a1);
		uint32_t wpn = U32(ent, 0x78);
		if (wpn != 0)
		{
			uint32_t ot2 = U32(game_addr(game::g_Battle_FrameRenderListBase), 0) + 0x44;
			b10_716DD0(U32(wpn, 4), ot2, 4, a1);
		}
		x::BattleModel_BuildBoneMatricesFromPose(pose);
		return 0;
	}

	uint32_t __cdecl b10_716DD0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		uint32_t node = a4;
		uint32_t verts = U32(U32(node, 0x44), 4);
		uint32_t cursor = U32(U32(node, 0x4C), 0);
		uint32_t tab = U32(a1, 4);
		uint32_t bones = U32(a1, 0) + 0x10;
		int32_t count = S32(tab, 0);
		uint32_t ent = U32(node, 0x48);
		uint32_t w = x::Field_Alloc(0x64);
		tab += 4;
		uint32_t mask = U32(ent, 0x7C);
		uint32_t rgb = U32(ent, 0x28) & 0xFFFFFF;
		U32(w, 0x50) = mask;
		U32(w, 0x48) = rgb | 0x24000000;
		int32_t amp_a = (int32_t)S16(node, 0x56);
		U32(w, 0x4C) = rgb | 0x2C000000;
		uint16_t s58 = U16(node, 0x58);
		U16(w, 0x5C) = s58;
		U16(w, 0x5E) = U16(node, 0x5A);
		U16(w, 0x5A) = (uint16_t)(mul32(amp_a, (int32_t)(int16_t)s58) / 4096);
		U16(w, 0x60) = U16(node, 0x5C);
		U16(w, 0x56) = U16(node, 0x54);
		x::GTE_SetRotMatrix(node + 0x20);
		x::GTE_SetTransVector(node + 0x20);
		for (int32_t i = 0; i < count; i++)
		{
			tab += 4;
			uint32_t wv = verts;
			uint32_t obj = U32(a1, 4) + U32(tab, -4);
			if (((U32(w, 0x50) >> (i & 31)) & 1) == 0)
				continue;
			int32_t ngroups = (int32_t)S16(obj, 0);
			uint32_t groups = obj + 2;
			uint32_t p = groups;
			for (int32_t g = ngroups; g > 0; g--)
			{
				int32_t bone = (int32_t)S16(p, 0);
				p += 2;
				uint32_t m = (uint32_t)(bone * 3 * 16) + bones + 0x10;
				xm::GTE_SetLightMatrix(m);
				uint32_t bk0 = U32(m, 0x14);
				uint32_t bk1 = U32(m, 0x18);
				uint32_t bk2 = U32(m, 0x1C);
				xm::GTE_SetBackgroundVector(bk0, bk1, bk2);
				int32_t nv = (int32_t)S16(p, 0);
				p += 2;
				uint32_t v = w + 0x28;
				for (int32_t k = nv; k > 0; k--)
				{
					uint16_t vx = U16(p, 0);
					uint16_t vy = U16(p, 2);
					p += 4;
					U16(v, 0) = vx;
					U16(w, 0x2A) = vy;
					U16(w, 0x2C) = U16(p, 0);
					p += 2;
					x::GTE_LoadV0(v);
					xm::GTE_MVMVA_LightV0_Bk();
					x::GTE_StoreIR123(v);
					uint16_t z = U16(w, 0x2C);
					int32_t sq = ((int32_t)(int16_t)z * (int32_t)S16(w, 0x5E)) / 4096;
					uint16_t ph = (uint16_t)((uint16_t)(U16(w, 0x60) * U16(w, 0x2A)) + U16(w, 0x56));
					U16(w, 0x2C) = (uint16_t)(z - (uint16_t)sq);
					ph &= 0xFFF;
					U16(w, 0x54) = ph;
					int32_t c = (int32_t)x::computeCosine((uint32_t)(int32_t)(int16_t)ph);
					int32_t d = mul32(c, (int32_t)S16(w, 0x5A)) / 32768;
					U16(w, 0x2C) = (uint16_t)(U16(w, 0x2C) + (uint16_t)d);
					uint32_t lo = U32(v, 0);
					uint32_t hi = U32(v, 4);
					U16(w, 0x58) = (uint16_t)d;
					U32(wv, 0) = lo;
					U32(wv, 4) = hi;
					wv += 0x10;
				}
			}
			uint32_t q = groups;
			uint32_t vp = verts;
			int32_t ng2 = (int32_t)S16(obj, 0);
			for (int32_t g = ng2; g > 0; g--)
			{
				int32_t nv = (int32_t)S16(q, 2);
				q += 4;
				if (nv <= 0)
					continue;
				uint32_t zp = vp + 0xC;
				q += (uint32_t)(nv * 3 * 2);
				for (int32_t k = nv; k > 0; k--)
				{
					uint32_t lo = U32(vp, 0);
					uint32_t hi = U32(vp, 4);
					U32(w, 0x10) = lo;
					U32(w, 0x14) = hi;
					x::GTE_LoadV0(w + 0x10);
					x::GTE_RTPS();
					x::GTE_ReadSXY2(zp - 4);
					xm::sub_45E220(zp);
					vp += 0x10;
					zp += 0x10;
				}
			}
			uint32_t f = (q + 3) & 0xFFFFFFFCu;
			int32_t ntri = (int32_t)S16(f, 0);
			int32_t nquad = (int32_t)S16(f, 2);
			f += 0xC;
			uint32_t pk = cursor;
			for (int32_t k = ntri; k > 0; k--, f += 0x10)
			{
				uint32_t i2 = U16(f, 4) & 0xFFF;
				uint32_t i1 = U16(f, 2) & 0xFFF;
				uint32_t i0 = U16(f, 0) & 0xFFF;
				U32(w, 0x40) = i2;
				U32(w, 0x3C) = i1;
				U32(w, 0x38) = i0;
				uint32_t s2 = U32(verts + (i2 << 4), 8);
				uint32_t s1 = U32(verts + (i1 << 4), 8);
				uint32_t s0 = U32(verts + (i0 << 4), 8);
				U32(w, 0) = s0;
				U32(w, 4) = s1;
				U32(w, 8) = s2;
				xm::sub_45E160(s0, s1, s2);
				x::GTE_NCLIP();
				x::GTE_ReadMAC0(w + 0x34);
				if (S32(w, 0x34) <= 0)
					continue;
				U32(pk, 8) = U32(w, 0);
				uint32_t uv0 = U32(f, 8);
				U32(pk, 0x10) = U32(w, 4);
				uint32_t uv1 = U32(f, 0xC);
				U32(pk, 0x18) = U32(w, 8);
				uint16_t uv2 = U16(f, 6);
				U32(pk, 0xC) = uv0;
				uint32_t code = U32(w, 0x48);
				U32(pk, 0) = 0x7000000;
				bool semi = (U8(f, 0xF) & 2) != 0;
				U32(pk, 0x14) = uv1;
				U16(pk, 0x1C) = uv2;
				U32(pk, 4) = code;
				if (semi)
					U8(pk, 7) |= 2;
				int32_t z2 = S32(verts + (U32(w, 0x40) << 4), 0xC);
				int32_t z1 = S32(verts + (U32(w, 0x3C) << 4), 0xC);
				int32_t z0 = S32(verts + (U32(w, 0x38) << 4), 0xC);
				int32_t sum = add32(add32(z1, z0), z2);
				int32_t hq = (int32_t)(((int64_t)sum * 0x55555556LL) >> 32);
				hq = add32(hq, (int32_t)((uint32_t)hq >> 31));
				int32_t key = hq >> (a3 & 31);
				U32(w, 0x30) = (uint32_t)key;
				x::SSIGPU_InsertPrimDepthKeys(a2 + (uint32_t)key * 4, pk, (uint32_t)z0, (uint32_t)z1, (uint32_t)z2, 0);
				pk += 0x20;
			}
			if (nquad > 0)
			{
				f += 4;
				for (int32_t k = nquad; k > 0; k--, f += 0x14)
				{
					uint32_t i2 = U16(f, 0) & 0xFFF;
					uint32_t i1 = U16(f, -2) & 0xFFF;
					uint32_t i0 = U16(f, -4) & 0xFFF;
					U32(w, 0x40) = i2;
					U32(w, 0x3C) = i1;
					U32(w, 0x38) = i0;
					uint32_t s2 = U32(verts + (i2 << 4), 8);
					uint32_t s1 = U32(verts + (i1 << 4), 8);
					uint32_t s0 = U32(verts + (i0 << 4), 8);
					U32(w, 0) = s0;
					U32(w, 4) = s1;
					U32(w, 8) = s2;
					xm::sub_45E160(s0, s1, s2);
					x::GTE_NCLIP();
					x::GTE_ReadMAC0(w + 0x34);
					if (S32(w, 0x34) <= 0)
						continue;
					uint32_t i3 = U16(f, 2) & 0xFFF;
					U32(pk, 8) = U32(w, 0);
					U32(w, 0x44) = i3;
					uint32_t v3 = verts + (i3 << 4);
					U32(pk, 0x10) = U32(w, 4);
					U32(pk, 0) = 0x9000000;
					uint32_t s3 = U32(v3, 8);
					U32(pk, 0x18) = U32(w, 8);
					uint32_t uv1 = U32(f, 8);
					U32(w, 0xC) = s3;
					U32(pk, 0x20) = s3;
					uint32_t uv0 = U32(f, 4);
					bool semi = (U8(f, 0xB) & 2) != 0;
					U32(pk, 0xC) = uv0;
					uint16_t uv2 = U16(f, 0xC);
					U32(pk, 0x14) = uv1;
					uint16_t uv3 = U16(f, 0xE);
					U16(pk, 0x1C) = uv2;
					uint32_t code = U32(w, 0x4C);
					U16(pk, 0x24) = uv3;
					U32(pk, 4) = code;
					if (semi)
						U8(pk, 7) |= 2;
					int32_t z3 = S32(v3, 0xC);
					int32_t z2 = S32(verts + (U32(w, 0x40) << 4), 0xC);
					int32_t z1 = S32(verts + (U32(w, 0x3C) << 4), 0xC);
					int32_t z0 = S32(verts + (U32(w, 0x38) << 4), 0xC);
					int32_t sum = add32(add32(add32(z0, z3), z1), z2);
					int32_t key = sum >> ((a3 + 2) & 31);
					U32(w, 0x30) = (uint32_t)key;
					x::SSIGPU_InsertPrimDepthKeys(a2 + (uint32_t)key * 4, pk, (uint32_t)z0, (uint32_t)z1, (uint32_t)z2, (uint32_t)z3);
					pk += 0x28;
				}
			}
			cursor = pk;
		}
		U32(U32(node, 0x4C), 0) = cursor;
		x::Field_Free(0x64);
		return 0;
	}

	uint32_t __cdecl b10_717330(uint32_t a1)
	{
		U16(a1, 0xCC) = 4;
		b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b10_717350(uint32_t a1)
	{
		uint32_t r = a1 + 0x6C;
		uint32_t ent = b103_entity_of(a1);
		U16(a1, 0xCC) = (uint16_t)(U16(a1, 0xCC) - 1);
		if (S16(a1, 0xCC) > 0)
			return 0;
		b10_717450(r, U32(U32(game_addr(game::unk_25653D8), 0), 0x100), game_addr(game::unk_2565890), ent);
		U8(ent, 0) |= 4;
		int16_t h = (int16_t)(U16(ent, 0x3C) - U16(ent, 0x36));
		U16(a1, 0x26) |= 8;
		U16(r, 0x56) = (uint16_t)h;
		if (h <= 0x400)
			U16(r, 0x56) = 0x400;
		else if (h >= 0x1000)
			U16(r, 0x56) = 0x1000;
		U16(r, 0x5C) = (uint16_t)h;
		uint16_t step;
		if (h <= 0x500)
			step = 8;
		else if (h <= 0x700)
			step = 7;
		else if (h <= 0x900)
			step = 6;
		else if (h <= 0xB00)
			step = 5;
		else if (h <= 0xD00)
			step = 4;
		else
			step = (uint16_t)((h <= 0xF00 ? 1 : 0) + 2);
		U16(r, 0x5C) = step;
		U16(r, 0x5A) = 0x1000;
		U16(r, 0x58) = 0;
		U16(ent, 0xC) = 0xC00;
		U16(a1, 0xCC) = 0x2D;
		b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b10_717450(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		x::MAG_007_sub_8DCC00(a1, 0x60);
		U32(a1, 0x48) = a4;
		U32(a1, 0x40) = a2;
		U32(a1, 0x4C) = game_addr(game::BATTLE_PACKET_WRITE_CURSOR);
		U32(a1, 0x50) = game_addr(game::BATTLE_PACKET_WRITE_CURSOR);
		U32(a3, 4) = a2;
		U32(a1, 0x44) = a3;
		U16(a3, 0x14) = 0;
		U16(a3, 0x16) = 0;
		U16(a3, 0x18) = 0x140;
		U16(a3, 0x1A) = 0;
		U8(a3, 0x1C) = 0x80;
		U8(a3, 0x1D) = 0x80;
		U8(a3, 0x1E) = 0x80;
		U32(a3, 0x20) = 0xFFFFFFFF;
		U16(a3, 0x24) = 0;
		return a3;
	}

	uint32_t __cdecl b10_7174B0(uint32_t a1)
	{
		U16(a1, 0xCC) = (uint16_t)(U16(a1, 0xCC) - 1);
		if (S16(a1, 0xCC) <= 0)
			b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b10_7174D0(uint32_t a1)
	{
		uint32_t ent = b103_entity_of(a1);
		uint16_t ang = (uint16_t)((U16(ent, 0xC) + 0x40) & 0xFFF);
		U16(ent, 0xC) = ang;
		if ((int16_t)ang <= 0x800)
		{
			U16(ent, 0xC) = 0;
			b103_next_state(a1);
		}
		U16(a1, 0xC4) = (uint16_t)(U16(a1, 0xC4) + 0x200);
		if (S16(a1, 0xC4) >= 0x1000)
			U16(a1, 0xC4) = 0x1000;
		return 0;
	}

	uint32_t __cdecl b10_717540(uint32_t a1)
	{
		U16(a1, 0xC4) = (uint16_t)(U16(a1, 0xC4) + 0xFE00);
		if (S16(a1, 0xC4) <= 0)
		{
			U16(a1, 0xC4) = 0;
			b103_next_state(a1);
		}
		U16(a1, 0xC6) = U16(a1, 0xC4);
		return 0;
	}

	uint32_t __cdecl b10_717590(uint32_t a1)
	{
		uint32_t ent = b103_entity_of(a1);
		U16(a1, 0xCC) = (uint16_t)(U16(a1, 0xCC) - 1);
		if (S16(a1, 0xCC) > 0)
			return 0;
		U16(ent, 0) &= 0xFFFB;
		U16(a1, 0x26) = (uint16_t)((U16(a1, 0x26) & 0xFFF7) | 1);
		b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b10_7176B0(uint32_t a1)
	{
		uint32_t dir = U32(game_addr(game::unk_15297F0), 0);
		uint32_t p0 = U32(dir, 8);
		uint32_t p1 = U32(dir, 0xC);
		U32(a1, 0x4C) = p0;
		U32(a1, 0x50) = p1;
		U16(a1, 0x4E) = 0xB000;
		U32(a1, 0x60) = a1 + 0x114;
		U32(a1, 0x11C) = 0x2800;
		U32(a1, 0x118) = 0x2800;
		U32(a1, 0x114) = 0x2800;
		U16(a1, 0x3E) = 0x800;
		x::au_re_Battle_ReadAnimation_7(a1, 0);
		U8(a1, 0x26) |= 4;
		U16(a1, 0x134) = 8;
		b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b10_717710(uint32_t a1)
	{
		U16(a1, 0x134) = (uint16_t)(U16(a1, 0x134) - 1);
		if (S16(a1, 0x134) > 0)
			return 0;
		U8(a1, 0x26) &= 0xFB;
		b10_717750(a1);
		U16(a1, 0x134) = 0x1E;
		b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b10_717750(uint32_t a1)
	{
		U16(a1, 0x13E) = 0x2400;
		U16(a1, 0x13C) = 0x1000;
		return b10_717770(a1);
	}

	uint32_t __cdecl b10_717770(uint32_t a1)
	{
		uint32_t col = U32(game_addr(game::CURRENT_BS_SHADOW_PALETTE), 0);
		int32_t k = (int32_t)S16(a1, 0x13C);
		uint8_t c0 = (uint8_t)(col & 0xFF);
		int32_t d0 = mul32((int32_t)c0, k) / 4096;
		U8(a1, 0x5C) = (uint8_t)(c0 - (uint8_t)d0);
		uint8_t c1 = (uint8_t)((col >> 8) & 0xFF);
		int32_t d1 = mul32((int32_t)c1, k) / 4096;
		U8(a1, 0x5D) = (uint8_t)(c1 - (uint8_t)d1);
		uint8_t c2 = U8(game_addr(game::unk_B8B9AA), 0);
		int32_t d2 = mul32((int32_t)c2, k) / 4096;
		U8(a1, 0x5E) = (uint8_t)(c2 - (uint8_t)d2);
		return 0;
	}

	uint32_t __cdecl b10_7177E0(uint32_t a1)
	{
		U16(a1, 0x134) = (uint16_t)(U16(a1, 0x134) - 1);
		if (S16(a1, 0x134) <= 0)
			b103_next_state(a1);
		return b10_717800(a1);
	}

	uint32_t __cdecl b10_717800(uint32_t a1)
	{
		uint16_t v = U16(a1, 0x13E);
		if (v != 0x1000)
		{
			v = (uint16_t)(v - 0x78);
			U16(a1, 0x13E) = v;
			if ((int16_t)v <= 0x1400)
				U16(a1, 0x13E) = 0x1400;
		}
		uint16_t d = U16(a1, 0x13C);
		if (d != 0)
		{
			d = (uint16_t)(d - 0x69);
			U16(a1, 0x13C) = d;
			if ((int16_t)d <= 0x200)
				U16(a1, 0x13C) = 0x200;
		}
		return b10_717770(a1);
	}

	uint32_t __cdecl b10_717860(uint32_t a1)
	{
		if (a_73B640(8) != 0)
		{
			U16(a1, 0x134) = 0;
			U16(a1, 0x126) = 0x200;
			b103_next_state(a1);
		}
		return b10_717800(a1);
	}

	uint32_t __cdecl b10_7178A0(uint32_t a1)
	{
		U16(a1, 0x134) = (uint16_t)(U16(a1, 0x134) + 1);
		if (U16(a1, 0x134) == 4)
			b10_717910(a1);
		U16(a1, 0x126) = (uint16_t)(U16(a1, 0x126) + 0x60);
		U16(a1, 0x4E) = (uint16_t)(U16(a1, 0x4E) + U16(a1, 0x126));
		if (a_73B7E0(9) != 0)
		{
			uint16_t y = U16(game_addr(game::unk_1529F64), 0);
			U16(a1, 0x134) = 0;
			U16(a1, 0x4E) = y;
			b103_next_state(a1);
		}
		return b10_717800(a1);
	}

	uint32_t __cdecl b10_717910(uint32_t a1)
	{
		U16(a1, 0x13E) = 0x1000;
		U16(a1, 0x13C) = 0;
		return b10_717770(a1);
	}

	uint32_t __cdecl b10_717930(uint32_t a1)
	{
		U16(a1, 0x134) = (uint16_t)(U16(a1, 0x134) + 1);
		int32_t idx = (int32_t)S16(a1, 0x134);
		uint16_t y = U16(game_addr(game::unk_1529F64) + (uint32_t)(idx * 2), 0);
		U16(a1, 0x4E) = y;
		if (y != 0)
			return 0;
		x::au_re_Battle_ReadAnimation_7(a1, 1);
		a_739AC0(a1, game_addr(game::unk_1529F38), 0);
		b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b10_717980(uint32_t a1)
	{
		if (x::au_re_Battle_ReadAnimation_8(a1) != 1)
			return 0;
		x::au_re_Battle_ReadAnimation_7(a1, 2);
		U16(a1, 0x134) = 0xA;
		b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b10_7179C0(uint32_t a1)
	{
		x::sub_8DD1C0(a1);
		U16(a1, 0x134) = (uint16_t)(U16(a1, 0x134) - 1);
		if (S16(a1, 0x134) <= 0)
			b103_next_state(a1);
		return 0;
	}

	uint32_t __cdecl b10_7179F0(uint32_t a1)
	{
		U16(a1, 0x13A) = (uint16_t)(U16(a1, 0x13A) + 0x100);
		if (S16(a1, 0x13A) >= 0x1000)
		{
			U8(a1, 0x26) |= 5;
			U16(a1, 0x13A) = 0x1000;
			b103_next_state(a1);
		}
		U16(a1, 0x13C) = U16(a1, 0x13A);
		b10_717A50(a1, 1);
		return b10_717770(a1);
	}

	uint32_t __cdecl b10_717A50(uint32_t a1, uint32_t a2)
	{
		uint32_t amount = U16(a1, 0x13A);
		return a_733950(a1 + 0x30, amount, a2);
	}

namespace boko
{
	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort PORTS_097[] = {
		{ game::Boko_MasterTask, (void *)b_729BD0, "097 MAG_097_sub_729BD0" },
		{ game::MAG_097_Emitter_State0_InitDirectorAndTasks, (void *)b7_729EA0, "097 MAG_097_sub_729EA0" },
		{ game::MAG_097_InitAssetTable, (void *)b7_729EF0, "097 MAG_097_sub_729EF0" },
		{ game::Boko_InitEffectOrigin, (void *)b_72A350, "097 MAG_097_sub_72A350" },
		{ game::MAG_097_DirectorTask, (void *)b7_72A410, "097 MAG_097_sub_72A410" },
		{ game::MAG_097_Director_PhaseBookkeeping, (void *)b7_72A4F0, "097 sub_72A4F0" },
		{ game::Boko_DirectorSpawnMain, (void *)b_72A540, "097 sub_72A540" },
		{ game::Boko_SpawnPrimModelTask, (void *)b_72A610, "097 sub_72A610" },
		{ game::MAG_097_MainModel_State0_Init, (void *)b_72A6C0, "097 sub_72A6C0" },
		{ game::Boko_PrimTaskPlay, (void *)b_72A720, "097 sub_72A720" },
		{ game::Boko_PrimDrawRecord, (void *)b_72A840, "097 sub_72A840" },
		{ game::MAG_097_MainModel_State1_Play, (void *)b_72AAF0, "097 sub_72AAF0" },
		{ game::MAG_097_EmitterStageSpawn_State0_StartAtFrame, (void *)b7_72AB80, "097 sub_72AB80" },
		{ game::Boko_ResetTrailHistory, (void *)b_72AD60, "097 sub_72AD60" },
		{ game::Boko_FillTrailRows, (void *)b_72AFE0, "097 sub_72AFE0" },
		{ game::MAG_097_EmitterStage_Step, (void *)b7_72B690, "097 sub_72B690" },
		{ game::Boko_EmitMeshF3, (void *)b_72BA00, "097 sub_72BA00" },
		{ game::Boko_EmitMeshF4, (void *)b_72BC10, "097 sub_72BC10" },
		{ game::BokoPrim_DrawG3List, (void *)b_72C370, "097 sub_72C370" },
		{ game::BokoPrim_DrawG4List, (void *)b_72C5A0, "097 sub_72C5A0" },
		{ game::BokoPrim_DrawGT3List, (void *)b_72C860, "097 sub_72C860" },
		{ game::BokoPrim_DrawGT4List, (void *)b_72CAC0, "097 sub_72CAC0" },
		{ game::MAG_097_ActSys_UpdatePass, (void *)b7_72CEE0, "097 sub_72CEE0" },
		{ game::MAG_097_UpdateEmitter, (void *)b7_72CF20, "097 sub_72CF20" },
		{ game::MAG_097_Emitter_BuildTransform, (void *)b7_72D7C0, "097 sub_72D7C0" },
		{ game::MAG_097_ScrollModelTextureV, (void *)b7_72DF60, "097 sub_72DF60" },
		{ game::BokoModel_MorphVertices, (void *)b_72E220, "097 sub_72E220" },
		{ game::BokoActor_InterpolatePosition, (void *)b_72EF80, "097 sub_72EF80" },
		{ game::MAG_097_EmitterStageSpawn_State1_StepUntilDone, (void *)b7_72FB90, "097 sub_72FB90" },
		{ game::MAG_097_EmitterStageTask, (void *)b7_72FBC0, "097 sub_72FBC0" },
		{ game::MAG_097_EmitterStage_State0_StartCamScript, (void *)b_72FC40, "097 sub_72FC40" },
		{ game::MAG_097_EmitterStage_State2_StartScriptAtCue2, (void *)b_730460, "097 sub_730460" },
		{ game::MAG_097_EmitterStage_State5_WaitCamStep6, (void *)b_730510, "097 sub_730510" },
		{ game::MAG_097_Director_State2_PauseUntilLoaded, (void *)b_7305D0, "097 sub_7305D0" },
		{ game::MAG_097_SpawnBokoCreature, (void *)b_730660, "097 MAG_097_SpawnBokoCreature" },
		{ game::Boko_CreatureActorTask, (void *)b_7306A0, "097 sub_7306A0" },
		{ game::MAG_097_Boko_CreatureActor_State0_Init, (void *)b_730B00, "097 sub_730B00" },
		{ game::MAG_097_Boko_CreatureActor_State2_SpawnShadow, (void *)b_730BD0, "097 sub_730BD0" },
		{ game::Boko_ShadowTask, (void *)b_730C10, "097 sub_730C10" },
		{ game::Boko_Shadow_Draw, (void *)b_730C90, "097 sub_730C90" },
		{ game::MAG_097_Boko_Shadow_State0_Init, (void *)b_730D00, "097 sub_730D00" },
		{ game::MAG_097_Boko_Shadow_State1_ShowAfterDelay, (void *)b_730D50, "097 sub_730D50" },
		{ game::MAG_097_Boko_Shadow_State2_AnimateToFrame8, (void *)b_730D70, "097 sub_730D70" },
		{ game::MAG_097_Boko_Shadow_State3_HoldFrames, (void *)b_730DD0, "097 sub_730DD0" },
		{ game::MAG_097_Boko_CreatureActor_State3_Anim3AtCue1, (void *)b_730E30, "097 sub_730E30" },
		{ game::MAG_097_Boko_CreatureActor_State4_PoseUntilCue2, (void *)b_730EA0, "097 sub_730EA0" },
		{ game::MAG_097_Boko_CreatureActor_State5_PoseUntilFrame32, (void *)b_730EF0, "097 sub_730EF0" },
		{ game::MAG_097_Boko_CreatureActor_State6_Anim4AtCue4, (void *)b7_730F10, "097 sub_730F10" },
		{ game::MAG_097_Boko_CreatureActor_State7_Anim5AtEnd, (void *)b_730F50, "097 sub_730F50" },
		{ game::MAG_097_Director_State5_WaitLoadedCue1, (void *)b_730FE0, "097 sub_730FE0" },
		{ game::MAG_097_Director_State10_DelayAtCue4, (void *)b_731090, "097 sub_731090" },
		{ game::MAG_097_Director_State11_SpawnModelAfterDelay, (void *)b7_7310C0, "097 sub_7310C0" },
		{ game::MAG_097_UvModelTask, (void *)b7_731100, "097 sub_731100" },
		{ game::MAG_097_UvModel_State0_Init, (void *)b_731520, "097 sub_731520" },
		{ game::MAG_097_UvModel_State1_Play, (void *)b_72AAF0, "097 sub_731570" },
		{ game::MAG_097_Director_State12_CountdownAtCue5, (void *)b7_7315A0, "097 sub_7315A0" },
		{ game::MAG_097_Director_State13_SetEndFlag, (void *)b_7315D0, "097 sub_7315D0" },
		{ game::MAG_097_Director_State14_SpawnFadeOut, (void *)b7_731600, "097 sub_731600" },
		{ game::MAG_097_EntityFadeOut_State0_SetC00, (void *)b_731690, "097 sub_731690" },
		{ game::MAG_097_EntityFadeOut_State1_FadeOut, (void *)b_7316B0, "097 sub_7316B0" },
		{ game::MAG_097_Director_State16_ReleaseVoiceAtCue6, (void *)b_731720, "097 sub_731720" },
		{ game::MAG_097_Director_PhaseSpawns, (void *)b7_731780, "097 sub_731780" },
		{ game::Boko_ParticleTaskA, (void *)b_731840, "097 sub_731840" },
		{ game::MAG_097_Boko_ParticleTaskA_State0_Init, (void *)b_7318D0, "097 sub_7318D0" },
		{ game::MAG_097_Boko_ParticleTaskA_State1_Play, (void *)b_72AAF0, "097 sub_731920" },
		{ game::Boko_ParticleTaskB, (void *)b_731950, "097 sub_731950" },
		{ game::MAG_097_Boko_ParticleTaskB_State0_Init, (void *)b_7319D0, "097 sub_7319D0" },
		{ game::MAG_097_Boko_ParticleTaskB_State1_Play, (void *)b_72AAF0, "097 sub_731A20" },
		{ game::MAG_097_EntityFadeIn_State1_FadeIn600, (void *)b_731AF0, "097 sub_731AF0" },
		{ game::MAG_097_EntityFadeInBright_State1_FadeInC00, (void *)b_731BE0, "097 sub_731BE0" },
		{ {}, nullptr, nullptr } };
	static const ModPort PORTS_098[] = {
		{ game::MAG_098_sub_7219D0, (void *)b_729BD0, "098 MAG_098_sub_7219D0" },
		{ game::MAG_098_sub_721CA0, (void *)b8_721CA0, "098 MAG_098_sub_721CA0" },
		{ game::Boko098_InitAssetTable, (void *)b8_721CF0, "098 MAG_098_sub_721CF0" },
		{ game::MAG_098_sub_722150, (void *)b_72A350, "098 MAG_098_sub_722150" },
		{ game::Boko098_PhaseDirectorTask, (void *)b8_722210, "098 MAG_098_sub_722210" },
		{ game::sub_7222F0, (void *)b8_7222F0, "098 sub_7222F0" },
		{ game::sub_722340, (void *)b8_722340, "098 sub_722340" },
		{ game::sub_722410, (void *)b8_722410, "098 sub_722410" },
		{ game::sub_7224C0, (void *)b_72A6C0, "098 sub_7224C0" },
		{ game::sub_722520, (void *)b_72A720, "098 sub_722520" },
		{ game::sub_722640, (void *)b_72A840, "098 sub_722640" },
		{ game::sub_7228F0, (void *)b_72AAF0, "098 sub_7228F0" },
		{ game::sub_722B60, (void *)b_72AD60, "098 sub_722B60" },
		{ game::sub_722DE0, (void *)b_72AFE0, "098 sub_722DE0" },
		{ game::sub_723800, (void *)b_72BA00, "098 sub_723800" },
		{ game::sub_723A10, (void *)b_72BC10, "098 sub_723A10" },
		{ game::sub_724170, (void *)b_72C370, "098 sub_724170" },
		{ game::sub_7243A0, (void *)b_72C5A0, "098 sub_7243A0" },
		{ game::sub_724660, (void *)b_72C860, "098 sub_724660" },
		{ game::sub_7248C0, (void *)b_72CAC0, "098 sub_7248C0" },
		{ game::sub_725D50, (void *)b_72E220, "098 sub_725D50" },
		{ game::sub_726AB0, (void *)b_72EF80, "098 sub_726AB0" },
		{ game::Boko098_CameraDirectorTask, (void *)b8_7276F0, "098 sub_7276F0" },
		{ game::sub_727780, (void *)b_72FC40, "098 sub_727780" },
		{ game::sub_727FA0, (void *)b_730460, "098 sub_727FA0" },
		{ game::Boko098_StartCameraScript2, (void *)b8_728050, "098 sub_728050" },
		{ game::sub_728080, (void *)b_730510, "098 sub_728080" },
		{ game::sub_728140, (void *)b_7305D0, "098 sub_728140" },
		{ game::MAG_098_SpawnBokoCreature, (void *)b_730660, "098 MAG_098_SpawnBokoCreature" },
		{ game::sub_728210, (void *)b_7306A0, "098 sub_728210" },
		{ game::sub_728670, (void *)b_730B00, "098 sub_728670" },
		{ game::sub_728740, (void *)b_730BD0, "098 sub_728740" },
		{ game::sub_728780, (void *)b_730C10, "098 sub_728780" },
		{ game::sub_728800, (void *)b_730C90, "098 sub_728800" },
		{ game::sub_728870, (void *)b_730D00, "098 sub_728870" },
		{ game::sub_7288C0, (void *)b_730D50, "098 sub_7288C0" },
		{ game::sub_7288E0, (void *)b_730D70, "098 sub_7288E0" },
		{ game::sub_728940, (void *)b_730DD0, "098 sub_728940" },
		{ game::sub_7289A0, (void *)b_730E30, "098 sub_7289A0" },
		{ game::sub_728A10, (void *)b_730EA0, "098 sub_728A10" },
		{ game::sub_728A60, (void *)b_730EF0, "098 sub_728A60" },
		{ game::sub_728A80, (void *)b_728A80, "098 sub_728A80" },
		{ game::sub_728AB0, (void *)b_730F50, "098 sub_728AB0" },
		{ game::sub_728AE0, (void *)b8_728AE0, "098 sub_728AE0" },
		{ game::sub_728B20, (void *)b8_728B20, "098 sub_728B20" },
		{ game::sub_728BE0, (void *)b_731090, "098 sub_728BE0" },
		{ game::sub_728C10, (void *)b8_728C10, "098 sub_728C10" },
		{ game::Boko098_PrimTask_Init, (void *)b8_728CC0, "098 MAG_310_HolyWar_Code" },
		{ game::sub_728D30, (void *)b_72AAF0, "098 sub_728D30" },
		{ game::sub_728D60, (void *)b8_728D60, "098 sub_728D60" },
		{ game::Boko098_FlareCoreTask, (void *)b8_728DA0, "098 sub_728DA0" },
		{ game::sub_7291C0, (void *)b_731520, "098 sub_7291C0" },
		{ game::sub_729210, (void *)b_72AAF0, "098 sub_729210" },
		{ game::sub_729240, (void *)b8_729240, "098 sub_729240" },
		{ game::sub_729270, (void *)b8_729270, "098 sub_729270" },
		{ game::sub_7292A0, (void *)b8_7292A0, "098 sub_7292A0" },
		{ game::sub_729330, (void *)b_731690, "098 sub_729330" },
		{ game::sub_729350, (void *)b8_729350, "098 sub_729350" },
		{ game::sub_7293C0, (void *)b_731720, "098 sub_7293C0" },
		{ game::Boko098_SpawnPhaseTasks, (void *)b8_729420, "098 sub_729420" },
		{ game::sub_7294C0, (void *)b_731840, "098 sub_7294C0" },
		{ game::sub_729550, (void *)b_7318D0, "098 sub_729550" },
		{ game::sub_7295A0, (void *)b_72AAF0, "098 sub_7295A0" },
		{ game::sub_7295D0, (void *)b_731950, "098 sub_7295D0" },
		{ game::sub_729650, (void *)b_7319D0, "098 sub_729650" },
		{ game::sub_7296A0, (void *)b_72AAF0, "098 sub_7296A0" },
		{ game::sub_729770, (void *)b_731AF0, "098 sub_729770" },
		{ game::sub_729860, (void *)b_731BE0, "098 sub_729860" },
		{ {}, nullptr, nullptr } };
	static const ModPort PORTS_099[] = {
		{ game::MAG_099_sub_7181B0, (void *)b9_7181B0, "099 MAG_099_sub_7181B0" },
		{ game::Choco099_InitAssetTable, (void *)b9_718200, "099 MAG_099_sub_718200" },
		{ game::MAG_099_sub_718670, (void *)b_718670, "099 MAG_099_sub_718670" },
		{ game::Choco099_DirectorTask, (void *)b9_718750, "099 MAG_099_sub_718750" },
		{ game::sub_718840, (void *)b9_718840, "099 sub_718840" },
		{ game::sub_718890, (void *)b9_718890, "099 sub_718890" },
		{ game::sub_718960, (void *)b9_718960, "099 sub_718960" },
		{ game::sub_718A10, (void *)b_72A6C0, "099 sub_718A10" },
		{ game::sub_718A70, (void *)b_72A720, "099 sub_718A70" },
		{ game::sub_718B90, (void *)b_72A840, "099 sub_718B90" },
		{ game::sub_718E40, (void *)b_72AAF0, "099 sub_718E40" },
		{ game::sub_7190B0, (void *)b_72AD60, "099 sub_7190B0" },
		{ game::sub_719330, (void *)b_72AFE0, "099 sub_719330" },
		{ game::sub_719D50, (void *)b_72BA00, "099 sub_719D50" },
		{ game::sub_719F60, (void *)b_72BC10, "099 sub_719F60" },
		{ game::sub_71A6C0, (void *)b_72C370, "099 sub_71A6C0" },
		{ game::sub_71A8F0, (void *)b_72C5A0, "099 sub_71A8F0" },
		{ game::sub_71ABB0, (void *)b_72C860, "099 sub_71ABB0" },
		{ game::sub_71AE10, (void *)b_72CAC0, "099 sub_71AE10" },
		{ game::sub_71C2A0, (void *)b_72E220, "099 sub_71C2A0" },
		{ game::sub_71D000, (void *)b_72EF80, "099 sub_71D000" },
		{ game::Choco099_CameraTask, (void *)b9_71DC40, "099 sub_71DC40" },
		{ game::sub_71DCD0, (void *)b_72FC40, "099 sub_71DCD0" },
		{ game::sub_71E4F0, (void *)b_730460, "099 sub_71E4F0" },
		{ game::sub_71E570, (void *)b9_71E570, "099 sub_71E570" },
		{ game::sub_71E5C0, (void *)b9_71E5C0, "099 sub_71E5C0" },
		{ game::sub_71E610, (void *)b9_71E610, "099 sub_71E610" },
		{ game::sub_71E6C0, (void *)b9_71E6C0, "099 sub_71E6C0" },
		{ game::sub_71E700, (void *)b9_71E700, "099 sub_71E700" },
		{ game::sub_71E800, (void *)b_7305D0, "099 sub_71E800" },
		{ game::MAG_099_SpawnBokoCreature, (void *)b_730660, "099 MAG_099_SpawnBokoCreature" },
		{ game::Choco099_CreatureTask, (void *)b9_71E8D0, "099 sub_71E8D0" },
		{ game::sub_71ED20, (void *)b_730B00, "099 sub_71ED20" },
		{ game::sub_71EDF0, (void *)b9_71EDF0, "099 sub_71EDF0" },
		{ game::sub_71EE40, (void *)b_730C10, "099 sub_71EE40" },
		{ game::sub_71EEC0, (void *)b_730C90, "099 sub_71EEC0" },
		{ game::sub_71EF30, (void *)b9_71EF30, "099 sub_71EF30" },
		{ game::sub_71EF80, (void *)b9_71EF80, "099 sub_71EF80" },
		{ game::sub_71EFB0, (void *)b9_71EFB0, "099 sub_71EFB0" },
		{ game::sub_71F010, (void *)b9_71F010, "099 sub_71F010" },
		{ game::sub_71F080, (void *)b_730E30, "099 sub_71F080" },
		{ game::sub_71F0F0, (void *)b_730EA0, "099 sub_71F0F0" },
		{ game::sub_71F140, (void *)b_730EF0, "099 sub_71F140" },
		{ game::sub_71F160, (void *)b_728A80, "099 sub_71F160" },
		{ game::sub_71F190, (void *)b9_71F190, "099 sub_71F190" },
		{ game::sub_71F1F0, (void *)b_730FE0, "099 sub_71F1F0" },
		{ game::sub_71F2A0, (void *)b_731090, "099 sub_71F2A0" },
		{ game::sub_71F2D0, (void *)b9_71F2D0, "099 sub_71F2D0" },
		{ game::sub_71F3A0, (void *)b9_71F3A0, "099 sub_71F3A0" },
		{ game::sub_71F450, (void *)b9_71F450, "099 sub_71F450" },
		{ game::sub_71F470, (void *)b9_71F470, "099 sub_71F470" },
		{ game::sub_71F4C0, (void *)b9_71F4C0, "099 sub_71F4C0" },
		{ game::sub_71F500, (void *)b_71F500, "099 sub_71F500" },
		{ game::sub_71F520, (void *)b9_71F520, "099 sub_71F520" },
		{ game::sub_71F550, (void *)b9_71F550, "099 sub_71F550" },
		{ game::sub_71F5A0, (void *)b9_71F5A0, "099 sub_71F5A0" },
		{ game::sub_71F5C0, (void *)b9_71F5C0, "099 sub_71F5C0" },
		{ game::Choco099_SpawnPhase, (void *)b9_71F620, "099 sub_71F620" },
		{ game::sub_71F8E0, (void *)b_71F8E0, "099 sub_71F8E0" },
		{ game::Boko_StateStepWordList, (void *)b_71F900, "099 sub_71F900" },
		{ game::sub_71F950, (void *)b_731840, "099 sub_71F950" },
		{ game::sub_71FD40, (void *)b_7318D0, "099 sub_71FD40" },
		{ game::sub_71FD90, (void *)b_72AAF0, "099 sub_71FD90" },
		{ game::sub_71FDC0, (void *)b_731950, "099 sub_71FDC0" },
		{ game::sub_71FE40, (void *)b_7319D0, "099 sub_71FE40" },
		{ game::sub_71FE90, (void *)b_72AAF0, "099 sub_71FE90" },
		{ game::sub_71FF20, (void *)b9_71FF20, "099 sub_71FF20" },
		{ game::sub_71FF70, (void *)b9_71FF70, "099 sub_71FF70" },
		{ game::sub_720020, (void *)b9_720020, "099 sub_720020" },
		{ game::Choco099_Meteor_Fall, (void *)b9_720080, "099 sub_720080" },
		{ game::sub_7200F0, (void *)b9_7200F0, "099 sub_7200F0" },
		{ game::sub_720140, (void *)b9_720140, "099 sub_720140" },
		{ game::sub_720250, (void *)b9_720250, "099 sub_720250" },
		{ game::Choco099_PrimPlayNoCam, (void *)b9_720290, "099 sub_720290" },
		{ game::sub_7202F0, (void *)b9_7202F0, "099 sub_7202F0" },
		{ game::sub_720380, (void *)b9_720380, "099 sub_720380" },
		{ game::sub_7203D0, (void *)b9_7203D0, "099 sub_7203D0" },
		{ game::sub_720470, (void *)b9_720470, "099 sub_720470" },
		{ game::sub_7204C0, (void *)b9_7204C0, "099 sub_7204C0" },
		{ game::sub_720500, (void *)b9_720500, "099 sub_720500" },
		{ game::sub_720570, (void *)b9_720570, "099 sub_720570" },
		{ game::sub_7205A0, (void *)b9_7205A0, "099 sub_7205A0" },
		{ game::sub_7205D0, (void *)b9_7205D0, "099 sub_7205D0" },
		{ game::sub_720670, (void *)b9_720670, "099 sub_720670" },
		{ game::sub_7206D0, (void *)b9_7206D0, "099 sub_7206D0" },
		{ game::sub_720790, (void *)b9_720790, "099 sub_720790" },
		{ game::sub_7207E0, (void *)b9_7207E0, "099 sub_7207E0" },
		{ game::sub_7208D0, (void *)b9_7208D0, "099 sub_7208D0" },
		{ game::sub_720940, (void *)b9_720940, "099 sub_720940" },
		{ game::sub_720960, (void *)b9_720960, "099 sub_720960" },
		{ game::Choco099_SpawnTrail, (void *)b9_7209B0, "099 sub_7209B0" },
		{ game::sub_720AE0, (void *)b9_720AE0, "099 sub_720AE0" },
		{ game::Choco099_TrailTask, (void *)b9_720B90, "099 sub_720B90" },
		{ game::Choco099_DrawSeqAtNode, (void *)b9_720C90, "099 sub_720C90" },
		{ game::sub_720D00, (void *)b9_720D00, "099 sub_720D00" },
		{ game::sub_720D20, (void *)b9_720D20, "099 sub_720D20" },
		{ game::Choco099_SpawnBurst, (void *)b9_720D70, "099 sub_720D70" },
		{ game::Choco099_BurstTask, (void *)b9_720EA0, "099 sub_720EA0" },
		{ game::sub_720FA0, (void *)b9_720FA0, "099 sub_720FA0" },
		{ game::sub_720FC0, (void *)b9_720FC0, "099 sub_720FC0" },
		{ game::sub_7210B0, (void *)b_731AF0, "099 sub_7210B0" },
		{ game::sub_721160, (void *)b_731690, "099 sub_721160" },
		{ game::sub_721180, (void *)b_7316B0, "099 sub_721180" },
		{ game::sub_7211D0, (void *)b9_7211D0, "099 sub_7211D0" },
		{ game::sub_721320, (void *)b9_721320, "099 sub_721320" },
		{ game::sub_721360, (void *)b9_721360, "099 sub_721360" },
		{ game::sub_721390, (void *)b9_721390, "099 sub_721390" },
		{ game::sub_7213F0, (void *)b9_7213F0, "099 sub_7213F0" },
		{ game::sub_721430, (void *)b9_721430, "099 sub_721430" },
		{ game::sub_7214C0, (void *)b9_7214C0, "099 sub_7214C0" },
		{ game::sub_7214E0, (void *)b9_7214E0, "099 sub_7214E0" },
		{ game::sub_721610, (void *)b9_721610, "099 sub_721610" },
		{ game::sub_721630, (void *)b9_721630, "099 sub_721630" },
		{ game::sub_721660, (void *)b9_721660, "099 sub_721660" },
		{ {}, nullptr, nullptr } };
	static const ModPort PORTS_100[] = {
		{ game::BokoPrim_RenderModelBiased, (void *)b10_70D530, "100 sub_70D530" },
		{ game::BokoPrim_RenderFlatTriangles, (void *)b10_70D680, "100 sub_70D680" },
		{ game::BokoPrim_RenderFlatQuads, (void *)b10_70D8A0, "100 sub_70D8A0" },
		{ game::BokoPrim_RenderTexturedTriangles, (void *)b10_70DB40, "100 sub_70DB40" },
		{ game::BokoPrim_RenderTexturedQuads, (void *)b10_70DDA0, "100 sub_70DDA0" },
		{ game::BokoPrim_RenderGouraudTriangles, (void *)b10_70E060, "100 sub_70E060" },
		{ game::BokoPrim_RenderGouraudQuads, (void *)b10_70E2A0, "100 sub_70E2A0" },
		{ game::BokoPrim_RenderGouraudTexturedTriangles, (void *)b10_70E580, "100 sub_70E580" },
		{ game::BokoPrim_RenderGouraudTexturedQuads, (void *)b10_70E800, "100 sub_70E800" },
		{ game::MAG_100_sub_70EDE0, (void *)b10_70EDE0, "100 MAG_100_sub_70EDE0" },
		{ game::MAG_100_InitModelContainer, (void *)b10_70EE30, "100 MAG_100_sub_70EE30" },
		{ game::MAG_100_sub_70F290, (void *)b_718670, "100 MAG_100_sub_70F290" },
		{ game::MAG_100_ParticleDirectorTask, (void *)b10_70F370, "100 MAG_100_sub_70F370" },
		{ game::sub_70F480, (void *)b10_70F480, "100 sub_70F480" },
		{ game::MAG_100_SetupBokoCreature, (void *)b_72A540, "100 MAG_100_SetupBokoCreature" },
		{ game::sub_70F5A0, (void *)b_72A610, "100 sub_70F5A0" },
		{ game::sub_70F650, (void *)b_72A6C0, "100 sub_70F650" },
		{ game::sub_70F6B0, (void *)b_72A720, "100 sub_70F6B0" },
		{ game::MAG_100_PrimModelDrawCallback, (void *)b10_70F7D0, "100 sub_70F7D0" },
		{ game::sub_70FAC0, (void *)b_72AAF0, "100 sub_70FAC0" },
		{ game::sub_70FD30, (void *)b_72AD60, "100 sub_70FD30" },
		{ game::sub_70FFB0, (void *)b_72AFE0, "100 sub_70FFB0" },
		{ game::sub_7109D0, (void *)b_72BA00, "100 sub_7109D0" },
		{ game::sub_710BE0, (void *)b_72BC10, "100 sub_710BE0" },
		{ game::sub_711340, (void *)b_72C370, "100 sub_711340" },
		{ game::sub_711570, (void *)b_72C5A0, "100 sub_711570" },
		{ game::sub_711830, (void *)b_72C860, "100 sub_711830" },
		{ game::sub_711A90, (void *)b_72CAC0, "100 sub_711A90" },
		{ game::sub_712F20, (void *)b_72E220, "100 sub_712F20" },
		{ game::sub_713C80, (void *)b_72EF80, "100 sub_713C80" },
		{ game::MAG_100_BokoCreatureTask, (void *)b10_7148C0, "100 MAG_100_BokoCreatureTask" },
		{ game::sub_714970, (void *)b_72FC40, "100 sub_714970" },
		{ game::sub_715190, (void *)b_730460, "100 sub_715190" },
		{ game::sub_715210, (void *)b10_715210, "100 sub_715210" },
		{ game::sub_715250, (void *)b10_715250, "100 sub_715250" },
		{ game::sub_715280, (void *)b10_715280, "100 sub_715280" },
		{ game::sub_7152C0, (void *)b10_7152C0, "100 sub_7152C0" },
		{ game::sub_7152E0, (void *)b10_7152E0, "100 sub_7152E0" },
		{ game::sub_715330, (void *)b10_715330, "100 sub_715330" },
		{ game::sub_7153B0, (void *)b10_7153B0, "100 sub_7153B0" },
		{ game::sub_715470, (void *)b_7305D0, "100 sub_715470" },
		{ game::MAG_100_SpawnBokoCreature, (void *)b_730660, "100 MAG_100_SpawnBokoCreature" },
		{ game::MAG_100_CreatureActorTask, (void *)b10_715540, "100 sub_715540" },
		{ game::sub_7159A0, (void *)b_730B00, "100 sub_7159A0" },
		{ game::sub_715A70, (void *)b_730BD0, "100 sub_715A70" },
		{ game::sub_715AB0, (void *)b_730C10, "100 sub_715AB0" },
		{ game::sub_715B30, (void *)b_730C90, "100 sub_715B30" },
		{ game::sub_715BA0, (void *)b_730D00, "100 sub_715BA0" },
		{ game::sub_715BF0, (void *)b_730D50, "100 sub_715BF0" },
		{ game::sub_715C10, (void *)b_730D70, "100 sub_715C10" },
		{ game::sub_715C70, (void *)b_730DD0, "100 sub_715C70" },
		{ game::sub_715CD0, (void *)b_730E30, "100 sub_715CD0" },
		{ game::sub_715D40, (void *)b_730EA0, "100 sub_715D40" },
		{ game::sub_715D60, (void *)b_730EF0, "100 sub_715D60" },
		{ game::sub_715D80, (void *)b_728A80, "100 sub_715D80" },
		{ game::sub_715DB0, (void *)b10_715DB0, "100 sub_715DB0" },
		{ game::sub_715DD0, (void *)b10_715DD0, "100 sub_715DD0" },
		{ game::sub_715DF0, (void *)b10_715DF0, "100 sub_715DF0" },
		{ game::sub_715E50, (void *)b_730FE0, "100 sub_715E50" },
		{ game::sub_715F60, (void *)b_71F500, "100 sub_715F60" },
		{ game::sub_715F80, (void *)b10_715F80, "100 sub_715F80" },
		{ game::sub_716040, (void *)b10_716040, "100 sub_716040" },
		{ game::sub_716070, (void *)b_7315D0, "100 sub_716070" },
		{ game::sub_7160C0, (void *)b10_7160C0, "100 sub_7160C0" },
		{ game::Boko100_DirectorSpawnByStep, (void *)b10_716120, "100 sub_716120" },
		{ game::sub_716360, (void *)b_71F8E0, "100 sub_716360" },
		{ game::sub_716380, (void *)b_71F900, "100 sub_716380" },
		{ game::sub_7163D0, (void *)b_731840, "100 sub_7163D0" },
		{ game::sub_7167C0, (void *)b_7318D0, "100 sub_7167C0" },
		{ game::sub_716810, (void *)b_72AAF0, "100 sub_716810" },
		{ game::sub_716840, (void *)b_731950, "100 sub_716840" },
		{ game::sub_7168C0, (void *)b_7319D0, "100 sub_7168C0" },
		{ game::sub_716910, (void *)b10_716910, "100 sub_716910" },
		{ game::sub_7169B0, (void *)b10_7169B0, "100 sub_7169B0" },
		{ game::sub_716A20, (void *)b10_716A20, "100 sub_716A20" },
		{ game::sub_716B00, (void *)b_731AF0, "100 sub_716B00" },
		{ game::sub_716BB0, (void *)b10_716BB0, "100 sub_716BB0" },
		{ game::sub_716BD0, (void *)b_7316B0, "100 sub_716BD0" },
		{ game::Boko100_TargetWobbleTask, (void *)b10_716C20, "100 sub_716C20" },
		{ game::Boko100_DrawTargetWobbled, (void *)b10_716CD0, "100 sub_716CD0" },
		{ game::Boko100_RenderModelWobble, (void *)b10_716DD0, "100 sub_716DD0" },
		{ game::sub_717330, (void *)b10_717330, "100 sub_717330" },
		{ game::sub_717350, (void *)b10_717350, "100 sub_717350" },
		{ game::Boko100_InitRenderBlock, (void *)b10_717450, "100 sub_717450" },
		{ game::sub_7174B0, (void *)b10_7174B0, "100 sub_7174B0" },
		{ game::sub_7174D0, (void *)b10_7174D0, "100 sub_7174D0" },
		{ game::sub_717540, (void *)b10_717540, "100 sub_717540" },
		{ game::sub_717590, (void *)b10_717590, "100 sub_717590" },
		{ game::sub_7175F0, (void *)b_7306A0, "100 sub_7175F0" },
		{ game::sub_7176B0, (void *)b10_7176B0, "100 sub_7176B0" },
		{ game::sub_717710, (void *)b10_717710, "100 sub_717710" },
		{ game::sub_717750, (void *)b10_717750, "100 sub_717750" },
		{ game::sub_717770, (void *)b10_717770, "100 sub_717770" },
		{ game::sub_7177E0, (void *)b10_7177E0, "100 sub_7177E0" },
		{ game::sub_717800, (void *)b10_717800, "100 sub_717800" },
		{ game::cintrindisp1_717860, (void *)b10_717860, "100 __cintrindisp1" },
		{ game::sub_7178A0, (void *)b10_7178A0, "100 sub_7178A0" },
		{ game::sub_717910, (void *)b10_717910, "100 sub_717910" },
		{ game::sub_717930, (void *)b10_717930, "100 sub_717930" },
		{ game::sub_717980, (void *)b10_717980, "100 sub_717980" },
		{ game::sub_7179C0, (void *)b10_7179C0, "100 sub_7179C0" },
		{ game::sub_7179F0, (void *)b10_7179F0, "100 sub_7179F0" },
		{ game::sub_717A50, (void *)b10_717A50, "100 sub_717A50" },
		{ {}, nullptr, nullptr } };
	static void reg(int eff, const ModPort *ports)
	{
		register_module(eff);
		for (const ModPort *p = ports; p->addr; p++)
			register_module_port(eff, p->addr, p->port, p->name);
	}
}
}
	void register_mag097_boko()
	{
		using namespace act::boko;
		reg(97, PORTS_097);
		reg(98, PORTS_098);
		reg(99, PORTS_099);
		reg(100, PORTS_100);
	}
}

