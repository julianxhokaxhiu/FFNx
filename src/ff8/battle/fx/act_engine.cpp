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

// Engine of the actor-based effects (GFs, limit breaks, enemy attacks)

#include "act_engine.h"
#include "../../../log.h"

namespace ff8fx
{
namespace act
{
	const Mod MOD_090 = { "tonberry", 90, game::MAG_090_TONBERRY_SUMMON_CHEFS_KNIFE_FL, game::MAG_088_INVINCIBLE_MOON_FL,
		{ game::unk_15474D0, {}, game::unk_1547168, game::unk_1547510, game::unk_154EB08, game::unk_259EFB4, {}, {}, {}, {}, game::unk_25A4C1C, game::unk_25A4C28, game::unk_25A4C20, {}, {}, {}, {}, {}, {}, game::unk_25A2118, {}, {}, {}, {}, {}, {}, game::MAG_090_EmitterQueue, {}, {}, game::unk_25A2CF8, game::unk_25A3DEC, {}, {}, {}, game::unk_25A4008, {}, {}, {}, game::unk_25A4C18, {} },
		{ {}, game::MAG_090_CameraScript_State0_ResetCamera, game::MAG_090_CameraScript_State1_StartScriptAtCue2, game::MAG_090_CameraScript_State2_StartCamScriptAtEnd, game::MAG_090_CameraScript_State3_StepCamScript, game::MAG_090_CameraScript_State4_WaitDirGate3, game::MAG_090_CameraScript_State5_PlaySEAtCue3, game::MAG_090_CameraScript_State6_CamKeyStep, game::MAG_090_CameraScript_State7_WaitCue4, game::MAG_090_CameraScript_State8_CamKeyStepThenKeyAtStep4, game::MAG_090_CameraScript_State9_StartScriptAtCue6, game::MAG_090_CameraScript_State10_CamKeyStep, game::MAG_090_CameraScript_State11_WaitCue7, game::MAG_090_CameraScript_State12_StartScriptAtCue7, game::MAG_090_CameraScript_State13_WaitCue9, game::MAG_090_CameraScript_State14_FinishNode, game::MAG_090_CameraScript_State15_Empty, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_090_EmitterTask, game::Tonberry_Master_Init, game::MAG_090_LoadScript_Issue, game::MAG_090_LoadCb_UploadTim, game::MAG_090_LoadCb_SummonStream, game::MAG_090_LoadCb_ClearBusy, game::MAG_090_LoadCb_ClearBusy_2, game::MAG_090_LoadCb_ClearBusy_3, game::MAG_090_LoadCb_ClearBusy_4, game::MAG_090_LoadScript_RecordFile, game::MAG_090_FileLoadScript_State1_LoadScriptWait, game::MAG_090_FileLoadScript_State2_Empty, game::MAG_090_LoadScript_TryArmStep, game::MAG_090_DirStepGE, game::MAG_090_Camera_StartKeyMove, game::MAG_090_Camera_KeyStep, game::MAG_090_Camera_WriteBattleCamera, game::MAG_090_CamScript_ReadKey, game::MAG_090_CamScript_ChannelStep, game::MAG_090_CamScript_RunKey, game::MAG_090_DirSyncGate, game::MAG_090_St_Empty, {}, game::Tonberry_PrimDrawCallback, game::MAG_090_RingSpawner_State0_SetupAtFrame29, game::MAG_090_RingSpawner_State1_SpawnRing, game::MAG_090_RingSpawner_State2_WidenAtFrame52, game::MAG_090_RingSpawner_State3_LowerAndSpawn, game::MAG_090_RingSpawner_State4_Nop, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_090_SpriteStrip_DrawQuads, game::MAG_090_StepCounterLimit, game::TonDamage_ApplyResult, game::MAG_090_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_090_Emitter_State3_FinishReleaseRoot, game::MAG_090_Emitter_State4_Empty, {}, {}, {}, {}, {}, {}, {} } };
	const Mod MOD_095 = { "siren", 95, game::MAG_095_SIREN_SUMMON_SILENT_VOICE_FL, game::MAG_094_ANGELO_REVERSE_FL,
		{ {}, {}, game::unk_1533010, game::unk_15339D0, game::unk_1533D18, game::BATTLE_PACKET_BUFFER_END, {}, {}, {}, {}, {}, {}, game::unk_257F8A0, game::unk_257F8A4, game::MAG_095_DirectorQueue, game::unk_257F9AC, game::unk_257F9B0, game::MAG_095_EffectQueueA, game::MAG_095_ParticleCursor, game::MAG_095_PrimPlayerPause, game::MAG_095_EffectQueueB, game::MAG_095_ModelEmitterQueue, game::unk_258A020, game::unk_258A024, game::unk_258BA90, game::MAG_095_ScreenTintQueue, game::unk_258BE30, game::unk_258BFA0, game::unk_258BFA4, game::MAG_095_LoadScriptCursorPtr, game::MAG_095_AssetTablePtr, game::MAG_095_CreatureQueue, game::MAG_095_LiveCountB, game::MAG_095_ParticlePoolPtr, game::unk_258EF98, game::MAG_095_LiveCountA, game::MAG_095_LoadScriptQueue, game::MAG_095_ScratchSP, game::MAG_095_PauseToggle, game::MAG_095_ActorStatePtr },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_095_Sequence_State0_Advance, game::MAG_095_Sequence_State1_Advance, game::MAG_095_Sequence_State2_CarveScratchArenas, game::MAG_095_Sequence_State3_SpawnEmitter, game::ActEng_EmitterTask, game::MAG_095_ActEng_Emitter_State0_SpawnFileLoadScript, game::ActEng_LoadScript_Issue, game::MAG_095_LoadCb_UploadTim, game::MAG_095_LoadCb_SummonStream, game::MAG_095_LoadCb_ClearBusy, game::MAG_095_LoadCb_ClearBusy_2, game::MAG_095_LoadCb_ClearBusy_3, game::MAG_095_LoadCb_ClearBusy_4, game::MAG_095_LoadScript_RecordFile, game::MAG_095_ActEng_FileLoadScript_State1_LoadScriptWait, game::MAG_095_ActEng_FileLoadScript_State2_Empty, game::MAG_095_LoadScript_TryArmStep, game::ActEng_DirStepGE, game::ActEng_Camera_StartKeyMove, game::ActEng_Camera_KeyStep, game::ActEng_Camera_WriteBattleCamera, game::MAG_095_CamScript_ReadKey, game::MAG_095_CamScript_ChannelStep, game::MAG_095_CamScript_RunKey, game::ActEng_DirSyncGate, game::MAG_095_St_Empty, game::ActEng_DirPhaseGE, game::Siren_RenderModelInstance, game::MAG_095_ModelTaskC_State0_Init, game::MAG_095_ModelTaskC_State1_WaitFrame18, game::MAG_095_ModelTaskC_State2_ResizeAtPhase4, game::MAG_095_ModelTaskC_State3_DrawUntilEnd, game::MAG_095_ModelTaskC_State4_Nop, game::MAG_095_ResetTrailHistory, game::MAG_095_Director_StartPositions, game::MAG_095_FillTrailRows, game::MAG_095_Director_Mode3Setup, game::MAG_095_Director_TargetPositions, game::MAG_095_RelocateActorData, game::MAG_095_ActSys_DrawActorSprite, game::ActEng_ActorMotion, game::MAG_095_DampValue, game::MAG_095_DampVector, game::MAG_095_Instance_AdvanceTexFrame, game::ActEng_InstanceBuildTransform, game::MAG_095_MorphVertexBlock, game::MAG_095_Instance_EndOfLifeTest, game::MAG_095_Instance_SpawnFollowUp, game::ActEng_AllocParticle, game::MAG_095_RandRange, game::MAG_095_RandRangeWide, game::MAG_095_RandAngleJitter, game::MAG_095_Particle_AttachToOwner, game::MAG_095_Actor_InterpolatePosition, game::MAG_095_ParticleList_Append, game::MAG_095_ParticleList_Unlink, game::MAG_095_Particle_PlaceAtAnchorSet, game::MAG_095_Particle_PlaceAtPoint, game::SpriteStrip_DrawQuads, game::MAG_095_StepCounterLimit, game::MAG_095_ActEng_Emitter_State1_ApplyActionResults, game::MAG_095_ActEng_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_095_ActEng_Emitter_State3_FinishReleaseRoot, game::MAG_095_ActEng_Emitter_State4_Empty, game::MAG_095_Sequence_State4_NextTargetLoop, game::MAG_095_Sequence_State5_Advance, game::MAG_095_Sequence_State6_WaitQueuesEmpty, game::MAG_095_Sequence_State7_RequestBattleSync, game::MAG_095_Sequence_State8_WaitBattleSyncDone, game::MAG_095_Sequence_State9_Finish, game::MAG_095_Sequence_State10_Empty } };
	const Mod MOD_096 = { "minimog", 96, game::MAG_096_MOOGLE_DANCE_FL, game::MAG_095_SIREN_SUMMON_SILENT_VOICE_FL,
		{ game::unk_152BBA0, game::unk_1532F80, game::unk_152BB90, game::unk_152BC00, {}, game::BATTLE_PACKET_BUFFER_END, game::unk_257B748, game::unk_257F4D4, game::unk_257F698, game::unk_257F720, game::unk_257F728, game::unk_257F730, game::unk_257B740, {}, {}, {}, game::unk_257B850, {}, game::unk_257CF20, {}, {}, {}, {}, {}, game::unk_257E258, {}, game::unk_257E268, {}, {}, game::unk_257E3E0, game::unk_257F4D8, {}, game::unk_257F688, game::unk_257F68C, {}, game::unk_257F704, {}, game::unk_257F71C, {}, game::unk_257F72C },
		{ game::MAG_096_ClearWorkBuffers, game::MAG_096_CameraScript_State0_StartScript, game::MAG_096_CameraScript_State1_StepNextAtCue2, game::MAG_096_CameraScript_State2_NextScriptAtEnd, game::MAG_096_CameraScript_State3_NextScriptAtEnd, game::MAG_096_CameraScript_State4_NextScriptAtEnd, game::MAG_096_CameraScript_State5_NextScriptAtEnd, game::MAG_096_CameraScript_State6_NextScriptAtEnd, game::MAG_096_CameraScript_State7_NextScriptAtEnd, game::MAG_096_CameraScript_State8_NextScriptAtEnd, game::MAG_096_CameraScript_State9_NextScriptAtEnd, game::MAG_096_CameraScript_State10_NextScriptAtEnd, game::MAG_096_CameraScript_State11_CamKeyStepThenStartKey, game::MAG_096_CameraScript_State12_CamKeyStep, game::MAG_096_CameraScript_State13_WaitCue4, game::MAG_096_CameraScript_State14_FinishNode, game::MAG_096_CameraScript_State15_Empty, game::MAG_096_DirectorSetup, game::ActSys_Step, game::MAG_096_ActSys_DrawPass, game::MAG_096_ActSys_DrawActorModel, game::MAG_096_RenderPrimModel, game::MAG_096_MeshEmitF3, game::MAG_096_MeshEmitF4, game::MAG_096_RenderPolyFT3List, game::MAG_096_RenderPolyFT4List, game::MAG_096_PrimList_G3, game::MAG_096_PrimList_G4, game::MAG_096_PrimList_GT3, game::MAG_096_PrimList_GT4, game::MAG_096_ActSys_DrawSpritePass, game::MAG_096_ActSys_UpdatePass, game::MAG_096_ActorStep, game::MAG_096_ActSys_UpdateEmitter, game::MAG_096_Particle_LifeCountdown, game::MAG_096_ActSys_EmitterSpawnStep, game::sub_737FD0, game::MAG_096_RandVec3, game::MAG_096_JitterAngles3, game::MAG_096_ActSys_AllocParticleSlot, game::MAG_096_GetWord8Minus1, game::MAG_096_Particle_Init, game::MAG_096_SpawnSubEffects, game::MAG_096_Director_SpawnGroupEffects, game::TexAnim_UploadFrame, {}, {}, {}, {}, game::MAG_096_EmitterTask, game::MAG_096_Emitter_State0_InitDirectorAndTasks, game::MAG_096_LoadScript_Issue, game::MAG_096_LoadCb_UploadTim, game::MAG_096_LoadCb_SummonStream, game::MAG_096_LoadCb_ClearBusy, game::MAG_096_LoadCb_ClearBusy_2, game::MAG_096_LoadCb_ClearBusy_3, game::MAG_096_LoadCb_ClearBusy_4, game::MAG_096_LoadScript_RecordFile, game::MAG_096_FileLoadScript_State1_LoadScriptWait, game::MAG_096_FileLoadScript_State2_Empty, game::MAG_096_LoadScript_TryArmStep, {}, game::MAG_096_Camera_StartKeyMove, game::MAG_096_Camera_KeyStep, game::MAG_096_Camera_WriteBattleCamera, {}, {}, {}, game::MAG_096_DirSyncGate, game::MAG_096_St_Empty, game::MAG_096_DirPhaseGE, {}, game::MAG_096_MusicFade_State0_SetVolumeAtCue2, game::MAG_096_MusicFade_State1_FadeOut, game::MAG_096_MusicFade_State2_WaitDirPhase4, game::MAG_096_MusicFade_State3_FadeIn, game::MAG_096_MusicFade_State4_Empty, game::MAG_096_ResetTrailHistory, game::MAG_096_Director_StartPositions, game::MAG_096_FillTrailRows, game::MAG_096_Director_Mode3Setup, game::MAG_096_Director_TargetPositions, game::MAG_096_RelocateActorData, game::MAG_096_ActSys_DrawActorSprite, game::MAG_096_ActorMotion, game::MAG_096_DampValue, game::MAG_096_DampVector, game::MAG_096_Instance_AdvanceTexFrame, game::MAG_096_Instance_BuildTransform, game::MAG_096_MorphVertexBlock, game::MAG_096_Instance_EndOfLifeTest, game::MAG_096_Instance_SpawnFollowUp, game::MAG_096_AllocParticle, game::MAG_096_RandRange, game::MAG_096_RandRangeWide, game::MAG_096_RandAngleJitter, game::MAG_096_Particle_AttachToOwner, game::MAG_096_Actor_InterpolatePosition, game::MAG_096_ParticleList_Append, game::MAG_096_ParticleList_Unlink, game::MAG_096_Particle_PlaceAtAnchorSet, game::MAG_096_Particle_PlaceAtPoint, {}, {}, game::MAG_096_Emitter_State1_ApplyActionResults, game::MAG_096_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_096_Emitter_State3_FinishReleaseRoot, game::MAG_096_Emitter_State4_Empty, {}, {}, {}, {}, {}, {}, {} } };
	const Mod MOD_097 = { "chocofire", 97, game::MAG_097_CHOCOFIRE_FL, game::MAG_096_MOOGLE_DANCE_FL,
		{ {}, game::unk_152BB10, game::unk_152B5B8, game::unk_152BA30, game::unk_152BAC8, game::BATTLE_PACKET_BUFFER_END, game::unk_2575270, game::unk_257AFB4, game::unk_257B178, game::unk_257B5C0, game::unk_257B5C8, {}, game::unk_2575268, {}, {}, {}, game::MAG_097_CameraMatrixPtr, {}, game::unk_2578028, game::unk_257802C, {}, {}, {}, {}, game::unk_2579D38, {}, game::unk_2579D48, {}, {}, game::unk_2579EC0, game::MAG_097_AssetTablePtr, {}, {}, game::unk_257B16C, {}, {}, {}, game::unk_257B5BC, game::unk_257B5C4, game::unk_257B5CC },
		{ game::MAG_097_ClearWorkBuffers, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_097_ActSys_DrawActorModel, game::MAG_097_RenderPrimModel, game::Boko_EmitMeshF3, game::Boko_EmitMeshF4, game::MAG_097_RenderPolyFT3List, game::MAG_097_RenderPolyFT4List, game::BokoPrim_DrawG3List, game::BokoPrim_DrawG4List, game::BokoPrim_DrawGT3List, game::BokoPrim_DrawGT4List, {}, {}, {}, {}, game::MAG_097_Particle_LifeCountdown, game::MAG_097_ActSys_EmitterSpawnStep, game::MAG_097_ActSys_SpawnParticles, game::MAG_097_RandVec3, game::MAG_097_JitterAngles3, game::MAG_097_ActSys_AllocParticleSlot, game::MAG_097_GetWord8Minus1, game::MAG_097_Particle_Init, game::MAG_097_SpawnSubEffects, {}, game::MAG_097_TexAnim_UploadFrame, {}, {}, {}, {}, game::MAG_097_EmitterTask, game::MAG_097_Emitter_State0_InitDirectorAndTasks, game::MAG_097_LoadScript_Issue, game::MAG_097_LoadCb_UploadTim, game::MAG_097_LoadCb_SummonStream, game::MAG_097_LoadCb_ClearBusy, game::MAG_097_LoadCb_ClearBusy_2, game::MAG_097_LoadCb_ClearBusy_3, game::MAG_097_LoadCb_ClearBusy_4, game::MAG_097_LoadScript_RecordFile, game::MAG_097_FileLoadScript_State1_LoadScriptWait, game::MAG_097_FileLoadScript_State2_Empty, game::MAG_097_LoadScript_TryArmStep, game::MAG_097_DirStepGE, game::MAG_097_Camera_StartKeyMove, game::MAG_097_Camera_KeyStep, game::MAG_097_Camera_WriteBattleCamera, {}, {}, {}, game::MAG_097_DirSyncGate, game::MAG_097_St_Empty, game::MAG_097_DirPhaseGE, {}, {}, {}, {}, {}, {}, game::Boko_ResetTrailHistory, game::MAG_097_Director_StartPositions, game::Boko_FillTrailRows, game::MAG_097_Director_Mode3Setup, game::MAG_097_Director_TargetPositions, game::MAG_097_RelocateActorData, game::MAG_097_ActSys_DrawActorSprite, {}, game::MAG_097_DampValue, game::MAG_097_DampVector, {}, {}, {}, {}, game::MAG_097_Instance_SpawnFollowUp, game::MAG_097_AllocParticle, game::MAG_097_RandRange, game::MAG_097_RandRangeWide, game::MAG_097_RandAngleJitter, game::MAG_097_Particle_AttachToOwner, game::BokoActor_InterpolatePosition, game::MAG_097_ParticleList_Append, game::MAG_097_ParticleList_Unlink, game::MAG_097_Particle_PlaceAtAnchorSet, game::MAG_097_Particle_PlaceAtPoint, {}, game::MAG_097_StepCounterLimit, game::MAG_097_Emitter_State1_ApplyActionResults, game::MAG_097_Emitter_State2_WaitChildrenOrDirFlag, game::MAG_097_Emitter_State3_FinishReleaseRoot, game::MAG_097_Emitter_State4_Empty, {}, {}, {}, {}, {}, {}, {} } };
	const Mod MOD_098 = { "chocoflare", 98, game::MAG_098_CHOCOFLARE_FL, game::MAG_097_CHOCOFIRE_FL,
		{ {}, game::unk_152B4F0, game::unk_152AF30, game::unk_152B3C8, game::unk_152B460, game::BATTLE_PACKET_BUFFER_END, game::unk_256E360, game::unk_2574D34, game::unk_2574EF8, game::unk_25750E8, game::unk_25750F0, {}, game::unk_256E358, {}, {}, {}, game::unk_256E468, {}, game::unk_2571118, game::unk_257111C, {}, {}, {}, {}, game::unk_2573AB8, {}, game::unk_2573AC8, {}, {}, game::unk_2573C40, game::unk_2574D38, {}, game::unk_2574EE8, game::unk_2574EEC, {}, game::unk_25750C8, {}, game::unk_25750E4, game::unk_25750EC, game::unk_25750F4 },
		{ game::MAG_098_sub_721BB0, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::sub_7229B0, game::sub_723490, game::sub_723520, game::sub_723580, game::sub_7236C0, game::sub_723800, game::sub_723A10, game::sub_723C90, game::sub_723ED0, game::sub_724170, game::sub_7243A0, game::sub_724660, game::sub_7248C0, game::sub_724BA0, game::sub_724CE0, game::sub_724D20, game::sub_725FB0, game::sub_726000, game::sub_726030, game::sub_7260B0, game::sub_7267A0, game::sub_7267F0, game::sub_726890, game::sub_726950, game::sub_7271A0, game::sub_7275A0, game::sub_727630, game::sub_728580, {}, {}, {}, {}, game::MAG_098_sub_721C20, game::MAG_098_sub_721CA0, game::sub_721E50, game::sub_721FF0, game::sub_722020, game::sub_722050, game::sub_722060, game::sub_722070, game::sub_722080, game::sub_722090, game::sub_7220D0, game::nullsub_1121, game::sub_728100, game::sub_727FD0, game::sub_7277D0, game::sub_727AC0, game::sub_727E10, {}, {}, {}, game::sub_728A30, game::nullsub_1124, game::sub_7289F0, {}, {}, {}, {}, {}, {}, game::sub_722B60, game::sub_722C40, game::sub_722DE0, game::sub_722EB0, game::sub_722FD0, game::sub_723230, game::sub_724C60, game::sub_724DB0, game::sub_725410, game::sub_725460, game::sub_7254F0, game::sub_7255C0, game::sub_725D50, game::sub_725E60, game::sub_725EE0, game::sub_725F10, game::MAG_098_RandRange, game::sub_726760, game::MAG_098_RandAngleJitter, game::sub_726960, game::sub_726AB0, game::sub_727140, game::sub_727170, game::sub_727350, game::sub_727490, {}, game::sub_728910, game::MAG_098_sub_7298B0, game::MAG_098_sub_729920, game::MAG_098_sub_729940, game::nullsub_1137, {}, {}, {}, {}, {}, {}, {} } };
	const Mod MOD_099 = { "chocometeor", 99, game::MAG_099_CHOCOMETEOR_FL, game::MAG_098_CHOCOFLARE_FL,
		{ {}, {}, game::unk_1529FA0, game::unk_152AC40, game::unk_152ACD8, game::BATTLE_PACKET_BUFFER_END, game::unk_2565A38, game::unk_256D6EC, game::unk_256D9F0, game::unk_256E1D8, game::unk_256E1E0, {}, game::unk_2565A30, game::unk_2565A34, game::unk_2565B18, game::unk_2565B3C, game::unk_2565B40, game::unk_2565C20, game::unk_2569330, game::unk_2569334, game::unk_2569338, game::unk_256B9D8, game::unk_256B9E8, game::unk_256B9EC, game::unk_256C1E0, game::unk_256C470, game::unk_256C480, game::unk_256C5F0, game::unk_256C5F4, game::unk_256C5F8, game::unk_256D6F0, game::unk_256D9D0, game::unk_256D9E0, game::unk_256D9E4, {}, game::unk_256E1B8, game::unk_256E1C0, game::unk_256E1D4, game::unk_256E1DC, game::unk_256E1E4 },
		{ game::MAG_099_sub_7180C0, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::sub_718F00, game::sub_7199E0, game::sub_719A70, game::sub_719AD0, game::sub_719C10, game::sub_719D50, game::sub_719F60, game::sub_71A1E0, game::sub_71A420, game::sub_71A6C0, game::sub_71A8F0, game::sub_71ABB0, game::sub_71AE10, game::sub_71B0F0, game::sub_71B230, game::sub_71B270, game::sub_71C500, game::sub_71C550, game::sub_71C580, game::sub_71C600, game::sub_71CCF0, game::sub_71CD40, game::sub_71CDE0, game::sub_71CEA0, game::sub_71D6F0, game::sub_71DAF0, game::sub_71DB80, game::sub_71EC30, game::MAG_099_sub_718060, game::MAG_099_sub_718070, game::MAG_099_sub_718080, game::MAG_099_sub_718100, game::MAG_099_sub_718130, game::MAG_099_sub_7181B0, game::sub_718370, game::sub_718510, game::sub_718540, game::sub_718570, game::sub_718580, game::sub_718590, game::sub_7185A0, game::sub_7185B0, game::sub_7185F0, game::nullsub_1088, game::sub_71E7C0, game::sub_71E520, game::sub_71DD20, game::sub_71E010, game::sub_71E360, {}, {}, {}, game::sub_71F110, {}, game::sub_71F0D0, {}, game::sub_71F380, game::sub_71F3A0, game::sub_71F450, game::sub_71F470, game::nullsub_1094, game::sub_7190B0, game::sub_719190, game::sub_719330, game::sub_719400, game::sub_719520, game::sub_719780, game::sub_71B1B0, game::sub_71B300, game::sub_71B960, game::sub_71B9B0, game::sub_71BA40, game::sub_71BB10, game::sub_71C2A0, game::sub_71C3B0, game::sub_71C430, game::sub_71C460, game::MAG_099_RandRange, game::sub_71CCB0, game::MAG_099_RandAngleJitter, game::sub_71CEB0, game::sub_71D000, game::sub_71D690, game::sub_71D6C0, game::sub_71D8A0, game::sub_71D9E0, {}, game::sub_71EFE0, game::MAG_099_sub_7216B0, game::MAG_099_sub_721720, game::MAG_099_sub_721740, game::nullsub_1117, game::MAG_099_sub_721770, game::MAG_099_sub_7217B0, game::MAG_099_sub_7217C0, game::MAG_099_sub_7217D0, game::MAG_099_sub_721800, game::MAG_099_sub_721810, game::nullsub_1087 } };
	const Mod MOD_100 = { "chocobocle", 100, game::MAG_100_CHOCOBOCLE_FL, game::MAG_099_CHOCOMETEOR_FL,
		{ {}, game::unk_1529ED0, game::unk_15297F0, game::unk_1529CD0, game::unk_1529D68, game::BATTLE_PACKET_BUFFER_END, game::unk_255EAD0, game::unk_25653D4, game::unk_25656D8, game::unk_2565880, game::unk_2565888, {}, game::unk_255EAC8, game::unk_255EACC, game::unk_255EBB0, game::unk_255EBD4, game::unk_255EBD8, game::unk_255ECB8, game::unk_2561888, game::unk_256188C, game::unk_2561890, game::unk_2563420, game::unk_2563430, game::unk_2563434, game::unk_2563EC8, game::unk_2564158, game::unk_2564168, game::unk_25642D8, game::unk_25642DC, game::unk_25642E0, game::unk_25653D8, game::unk_25656B8, game::unk_25656C8, game::unk_25656CC, {}, game::unk_2565860, game::unk_2565868, game::unk_256587C, game::unk_2565884, game::unk_256588C },
		{ game::MAG_100_sub_70ECF0, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::sub_70FB80, game::sub_710660, game::sub_7106F0, game::sub_710750, game::sub_710890, game::sub_7109D0, game::sub_710BE0, game::sub_710E60, game::sub_7110A0, game::sub_711340, game::sub_711570, game::sub_711830, game::sub_711A90, game::sub_711D70, game::sub_711EB0, game::sub_711EF0, game::sub_713180, game::sub_7131D0, game::sub_713200, game::sub_713280, game::sub_713970, game::sub_7139C0, game::sub_713A60, game::sub_713B20, game::sub_714370, game::sub_714770, game::sub_714800, game::sub_7158B0, game::MAG_100_sub_70EC90, game::MAG_100_sub_70ECA0, game::MAG_100_sub_70ECB0, game::MAG_100_sub_70ED30, game::MAG_100_sub_70ED60, game::MAG_100_sub_70EDE0, game::sub_70EF90, game::sub_70F130, game::sub_70F160, game::sub_70F190, game::sub_70F1A0, game::sub_70F1B0, game::sub_70F1C0, game::sub_70F1D0, game::sub_70F210, game::nullsub_1069, game::sub_715430, game::sub_7151C0, game::sub_7149C0, game::sub_714CB0, game::sub_715000, {}, {}, {}, game::sub_715300, game::nullsub_1072, game::sub_715D20, {}, {}, {}, {}, {}, {}, game::sub_70FD30, game::sub_70FE10, game::sub_70FFB0, game::sub_710080, game::sub_7101A0, game::sub_710400, game::sub_711E30, game::sub_711F80, game::sub_7125E0, game::sub_712630, game::sub_7126C0, game::sub_712790, game::sub_712F20, game::sub_713030, game::sub_7130B0, game::sub_7130E0, game::MAG_100_RandRange, game::sub_713930, game::MAG_100_RandAngleJitter, game::sub_713B30, game::sub_713C80, game::sub_714310, game::sub_714340, game::sub_714520, game::sub_714660, {}, game::sub_715C40, game::MAG_100_sub_717B80, game::MAG_100_sub_717BF0, game::MAG_100_sub_717C10, game::nullsub_1086, game::MAG_100_sub_717C40, game::MAG_100_sub_717C80, game::MAG_100_sub_717C90, game::MAG_100_sub_717CA0, game::MAG_100_sub_717CD0, game::MAG_100_sub_717CE0, game::nullsub_1068 } };
	struct AddrPort { game::Id addr; void *port; const char *name; };
	static const AddrPort ENGINE_PORTS_090[] = {
		{ game::MAG_090_InitModuleState, (void *)a_7322A0, "090 MAG_090_sub_762760" },
		{ game::MAG_090_CameraScriptTask, (void *)a_732A00, "090 sub_762DA0" },
		{ game::MAG_090_SetEntityColourTint, (void *)a_733950, "090 sub_7678D0" },
		{ game::MAG_090_Tonberry_Director_State5_WaitLoadScriptIdle, (void *)a_733AC0, "090 sub_763C50" },
		{ game::MAG_090_CameraScript_State4_WaitDirGate3, (void *)a_733B70, "090 sub_763920" },
		{ game::MAG_090_Tonberry_Master_State0_Advance, (void *)a_73A0D0, "090 MAG_090_sub_762630" },
		{ game::MAG_090_Tonberry_Master_State1_Advance, (void *)a_73A0D0, "090 MAG_090_sub_762640" },
		{ game::MAG_090_Tonberry_Master_State5_Advance, (void *)a_73A0D0, "090 MAG_090_sub_768440" },
		{ game::MAG_090_Tonberry_Master_State3_SpawnEmitter, (void *)a_73A170, "090 MAG_090_sub_762660" },
		{ game::MAG_090_EmitterTask, (void *)a_73A1A0, "090 MAG_090_sub_762690" },
		{ game::MAG_090_FileLoadScriptTask, (void *)a_73A380, "090 MAG_090_sub_762850" },
		{ game::MAG_090_SparkFollowTask, (void *)a_73A380, "090 sub_767B40" },
		{ game::MAG_090_EntityFadeInTask, (void *)a_73A380, "090 sub_7681B0" },
		{ game::MAG_090_EntityFadeOutTask, (void *)a_73A380, "090 sub_7682A0" },
		{ game::MAG_090_LoadScript_Issue, (void *)a_73A3E0, "090 sub_7628B0" },
		{ game::MAG_090_LoadCb_UploadTim, (void *)a_73A580, "090 sub_762A50" },
		{ game::MAG_090_LoadCb_SummonStream, (void *)a_73A5B0, "090 sub_762A80" },
		{ game::MAG_090_LoadCb_ClearBusy, (void *)a_73A5E0, "090 sub_762AB0" },
		{ game::MAG_090_LoadCb_ClearBusy_2, (void *)a_73A5E0, "090 sub_762AC0" },
		{ game::MAG_090_LoadCb_ClearBusy_3, (void *)a_73A5E0, "090 sub_762AD0" },
		{ game::MAG_090_LoadCb_ClearBusy_4, (void *)a_73A5E0, "090 sub_762AE0" },
		{ game::MAG_090_LoadScript_RecordFile, (void *)a_73A620, "090 sub_762AF0" },
		{ game::MAG_090_FileLoadScript_State1_LoadScriptWait, (void *)a_73A660, "090 sub_762B30" },
		{ game::MAG_090_FileLoadScript_State2_Empty, (void *)a_73A6D0, "090 nullsub_1247" },
		{ game::MAG_090_St_Empty, (void *)a_73A6D0, "090 nullsub_1248" },
		{ game::MAG_090_CameraScript_State15_Empty, (void *)a_73A6D0, "090 nullsub_1249" },
		{ game::MAG_090_Tonberry_Director_State11_Empty, (void *)a_73A6D0, "090 nullsub_1250" },
		{ game::MAG_090_St_Empty_2, (void *)a_73A6D0, "090 nullsub_1251" },
		{ game::MAG_090_Tonberry_Particle_State3_Empty, (void *)a_73A6D0, "090 nullsub_1252" },
		{ game::MAG_090_Tonberry_PrimModelActor_State4_Empty, (void *)a_73A6D0, "090 nullsub_1253" },
		{ game::MAG_090_PrimModelRise_State3_Nop, (void *)a_73A6D0, "090 nullsub_1254" },
		{ game::MAG_090_TonRingParticle_State2_Empty, (void *)a_73A6D0, "090 nullsub_1255" },
		{ game::MAG_090_RingSpawner_State4_Nop, (void *)a_73A6D0, "090 nullsub_1256" },
		{ game::MAG_090_SparkFollow_State2_Nop, (void *)a_73A6D0, "090 nullsub_1257" },
		{ game::MAG_090_TonSpark_State2_Empty, (void *)a_73A6D0, "090 nullsub_1258" },
		{ game::MAG_090_CreatureActor_State13_Empty, (void *)a_73A6D0, "090 nullsub_1259" },
		{ game::MAG_090_EntityFadeIn_State2_Nop, (void *)a_73A6D0, "090 nullsub_1260" },
		{ game::MAG_090_EntityFadeOut_State2_Nop, (void *)a_73A6D0, "090 nullsub_1261" },
		{ game::MAG_090_Emitter_State4_Empty, (void *)a_73A6D0, "090 nullsub_1262" },
		{ game::MAG_090_Tonberry_Master_State10_Empty, (void *)a_73A6D0, "090 nullsub_1246" },
		{ game::MAG_090_Tonberry_Director_State4_ArmLoadScriptStep, (void *)a_73A930, "090 sub_763BF0" },
		{ game::MAG_090_LoadScript_TryArmStep, (void *)a_73A950, "090 sub_763C10" },
		{ game::MAG_090_DirStepGE, (void *)a_73AAE0, "090 sub_762EC0" },
		{ game::MAG_090_Camera_ResetBlock, (void *)a_73AB00, "090 sub_762E70" },
		{ game::MAG_090_Camera_StartKeyMove, (void *)a_73AB20, "090 sub_762EE0" },
		{ game::MAG_090_Camera_KeyStep, (void *)a_73AE10, "090 sub_7631D0" },
		{ game::MAG_090_Camera_WriteBattleCamera, (void *)a_73B160, "090 sub_763520" },
		{ game::MAG_090_CameraScript_State6_CamKeyStep, (void *)a_73B2D0, "090 sub_7639C0" },
		{ game::MAG_090_CameraScript_State10_CamKeyStep, (void *)a_73B2D0, "090 sub_763A80" },
		{ game::MAG_090_CameraScript_State8_CamKeyStepThenKeyAtStep4, (void *)a_73B350, "090 sub_763A00" },
		{ game::MAG_090_CamScript_Start, (void *)a_73B3B0, "090 sub_7636C0" },
		{ game::MAG_090_CamScript_ReadKey, (void *)a_73B430, "090 sub_763740" },
		{ game::MAG_090_CamScript_Tick, (void *)a_73B4B0, "090 sub_7637B0" },
		{ game::MAG_090_CamScript_ChannelStep, (void *)a_73B520, "090 sub_763820" },
		{ game::MAG_090_CamScript_RunKey, (void *)a_73B570, "090 sub_763870" },
		{ game::MAG_090_Tonberry_Director_State6_WaitDirGate5, (void *)a_73B620, "090 sub_763C70" },
		{ game::MAG_090_DirSyncGate, (void *)a_73B640, "090 sub_763940" },
		{ game::MAG_090_CameraScript_State14_FinishNode, (void *)a_73B750, "090 sub_763B30" },
		{ game::MAG_090_Tonberry_Director_State1_WaitDirGate1, (void *)a_73B790, "090 sub_763B70" },
		{ game::MAG_090_DirPhaseGE, (void *)a_73B7E0, "090 sub_763BB0" },
		{ game::MAG_090_Tonberry_Particle_State0_WaitDirGate2, (void *)a_73B8A0, "090 sub_7640E0" },
		{ game::MAG_090_SetPauseSwitch, (void *)a_73BB60, "090 sub_767ED0" },
		{ game::MAG_090_DrawPrimModel, (void *)a_73C280, "090 sub_764130" },
		{ game::MAG_090_EmitG3, (void *)a_73D770, "090 sub_765600" },
		{ game::MAG_090_EmitG4, (void *)a_73D9C0, "090 sub_765850" },
		{ game::MAG_090_RingSpawnerTask, (void *)a_73F110, "090 sub_7670E0" },
		{ game::InitEffectSequenceFromData_c5, (void *)a_7435E0, "090 InitEffectSequenceFromData_c5" },
		{ game::MAG_090_SpriteStrip_DrawQuads, (void *)a_743720, "090 sub_766A40" },
		{ game::MAG_090_Tonberry_PrimModelActor_State3_CounterFinish, (void *)a_743C00, "090 sub_766F80" },
		{ game::MAG_090_StepCounterLimit, (void *)a_743C20, "090 sub_766F50" },
		{ game::MAG_090_DrawBattleModel, (void *)a_746C10, "090 sub_767610" },
		{ game::MAG_090_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "090 MAG_090_sub_7683B0" },
		{ game::MAG_090_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "090 MAG_090_sub_7683D0" },
		{ game::MAG_090_Tonberry_Master_State4_NextTargetLoop, (void *)a_747500, "090 MAG_090_sub_768400" },
		{ game::MAG_090_Tonberry_Master_State6_WaitQueuesEmpty, (void *)a_747550, "090 MAG_090_sub_768450" },
		{ game::MAG_090_Tonberry_Master_State7_RequestBattleSync, (void *)a_747560, "090 MAG_090_sub_768460" },
		{ game::MAG_090_Tonberry_Master_State8_WaitBattleSyncDone, (void *)a_747590, "090 MAG_090_sub_768490" },
		{ game::MAG_090_Tonberry_Director_State10_Finish, (void *)a_7475A0, "090 sub_763D20" },
		{ game::MAG_090_Tonberry_Master_State9_Finish, (void *)a_7475A0, "090 MAG_090_sub_7684A0" },
		{ {}, nullptr, nullptr }
	};
	static const AddrPort ENGINE_PORTS_095[] = {
		{ game::GF_095Siren_SequenceTick, (void *)a_739F40, "095 GF_095Siren_SequenceTick" },
		{ game::MAG_095_Sequence_State0_Advance, (void *)a_73A0D0, "095 MAG_095_sub_73A0D0" },
		{ game::MAG_095_Sequence_State1_Advance, (void *)a_73A0D0, "095 MAG_095_sub_73A0E0" },
		{ game::MAG_095_BoxSparkleSpawner_State0_Advance, (void *)a_73A0D0, "095 sub_7432B0" },
		{ game::MAG_095_OrbitSparkleSpawner_State1_Advance, (void *)a_73A0D0, "095 sub_743D00" },
		{ game::MAG_095_BurstSparkleSpawner_State1_Advance, (void *)a_73A0D0, "095 sub_7440F0" },
		{ game::MAG_095_KeyPathSparkleSpawner_State1_Advance, (void *)a_73A0D0, "095 sub_7444E0" },
		{ game::MAG_095_Sequence_State5_Advance, (void *)a_73A0D0, "095 MAG_095_sub_747540" },
		{ game::MAG_095_Sequence_State3_SpawnEmitter, (void *)a_73A170, "095 MAG_095_sub_73A170" },
		{ game::ActEng_EmitterTask, (void *)a_73A1A0, "095 MAG_095_sub_73A1A0" },
		{ game::ActEng_FileLoadScriptTask, (void *)a_73A380, "095 MAG_095_sub_73A380" },
		{ game::MAG_095_TargetModelTask, (void *)a_73A380, "095 sub_73F800" },
		{ game::MAG_095_ParticleSystemTask, (void *)a_73A380, "095 sub_73F900" },
		{ game::MAG_095_BoxSparkleSpawnerTask, (void *)a_73A380, "095 sub_743250" },
		{ game::MAG_095_TargetRiseSpawnerTask, (void *)a_73A380, "095 sub_7447C0" },
		{ game::MAG_095_ScriptParticleSystemTask, (void *)a_73A380, "095 sub_745180" },
		{ game::MAG_095_OrbitSpriteSpawnerTask, (void *)a_73A380, "095 sub_745250" },
		{ game::MAG_095_StageWobbleInTask, (void *)a_73A380, "095 sub_746920" },
		{ game::MAG_095_StageWobbleOutTask, (void *)a_73A380, "095 sub_746A40" },
		{ game::ActEng_LoadScript_Issue, (void *)a_73A3E0, "095 sub_73A3E0" },
		{ game::MAG_095_LoadCb_UploadTim, (void *)a_73A580, "095 sub_73A580" },
		{ game::MAG_095_LoadCb_SummonStream, (void *)a_73A5B0, "095 sub_73A5B0" },
		{ game::MAG_095_LoadCb_ClearBusy, (void *)a_73A5E0, "095 sub_73A5E0" },
		{ game::MAG_095_LoadCb_ClearBusy_2, (void *)a_73A5E0, "095 sub_73A5F0" },
		{ game::MAG_095_LoadCb_ClearBusy_3, (void *)a_73A5E0, "095 sub_73A600" },
		{ game::MAG_095_LoadCb_ClearBusy_4, (void *)a_73A5E0, "095 sub_73A610" },
		{ game::MAG_095_LoadScript_RecordFile, (void *)a_73A620, "095 sub_73A620" },
		{ game::MAG_095_ActEng_FileLoadScript_State1_LoadScriptWait, (void *)a_73A660, "095 sub_73A660" },
		{ game::MAG_095_ActEng_FileLoadScript_State2_Empty, (void *)a_73A6D0, "095 nullsub_1171" },
		{ game::MAG_095_St_Empty, (void *)a_73A6D0, "095 nullsub_1172" },
		{ game::MAG_095_Siren_CameraScript_State14_Empty, (void *)a_73A6D0, "095 nullsub_1173" },
		{ game::MAG_095_Siren_Director_State27_Empty, (void *)a_73A6D0, "095 nullsub_1174" },
		{ game::MAG_095_St_Empty_2, (void *)a_73A6D0, "095 nullsub_1175" },
		{ game::MAG_095_Cut1ModelEmitter_State2_Nop, (void *)a_73A6D0, "095 nullsub_1176" },
		{ game::MAG_095_ModelTaskA_State2_Nop, (void *)a_73A6D0, "095 nullsub_1177" },
		{ game::MAG_095_ModelTaskB_State4_Nop, (void *)a_73A6D0, "095 nullsub_1178" },
		{ game::MAG_095_ModelTaskC_State4_Nop, (void *)a_73A6D0, "095 nullsub_1179" },
		{ game::MAG_095_ModelTaskD_State3_Nop, (void *)a_73A6D0, "095 nullsub_1180" },
		{ game::MAG_095_ModelTaskE_State4_Nop, (void *)a_73A6D0, "095 nullsub_1181" },
		{ game::MAG_095_ModelTaskF_State4_Nop, (void *)a_73A6D0, "095 nullsub_1182" },
		{ game::MAG_095_ModelTaskG_State5_Nop, (void *)a_73A6D0, "095 nullsub_1183" },
		{ game::MAG_095_TargetModel_State2_Nop, (void *)a_73A6D0, "095 nullsub_1184" },
		{ game::MAG_095_ParticleSystem_State2_Nop, (void *)a_73A6D0, "095 nullsub_1185" },
		{ game::MAG_095_BoxSparkle_State2_Nop, (void *)a_73A6D0, "095 nullsub_1186" },
		{ game::MAG_095_BoxSparkleSpawner_State2_Nop, (void *)a_73A6D0, "095 nullsub_1187" },
		{ game::MAG_095_OrbitSparkle_State2_Nop, (void *)a_73A6D0, "095 nullsub_1188" },
		{ game::MAG_095_OrbitSparkleSpawner_State3_Nop, (void *)a_73A6D0, "095 nullsub_1189" },
		{ game::MAG_095_BurstSparkle_State2_Nop, (void *)a_73A6D0, "095 nullsub_1190" },
		{ game::MAG_095_BurstSparkleSpawner_State3_Nop, (void *)a_73A6D0, "095 nullsub_1191" },
		{ game::MAG_095_KeyPathSparkle_State2_Nop, (void *)a_73A6D0, "095 nullsub_1192" },
		{ game::MAG_095_KeyPathSparkleSpawner_State3_Nop, (void *)a_73A6D0, "095 nullsub_1193" },
		{ game::MAG_095_RisingSparkle_State2_Nop, (void *)a_73A6D0, "095 nullsub_1194" },
		{ game::MAG_095_SplashSparkle_State3_Nop, (void *)a_73A6D0, "095 nullsub_1195" },
		{ game::MAG_095_TargetRiseSpawner_State2_Nop, (void *)a_73A6D0, "095 nullsub_1196" },
		{ game::MAG_095_ScriptParticleSystem_State2_Nop, (void *)a_73A6D0, "095 nullsub_1197" },
		{ game::MAG_095_SpawnScript_State3_Nop, (void *)a_73A6D0, "095 nullsub_1198" },
		{ game::MAG_095_OrbitSprite_State4_Nop, (void *)a_73A6D0, "095 nullsub_1199" },
		{ game::MAG_095_OrbitSpriteSpawner_State2_Nop, (void *)a_73A6D0, "095 nullsub_1200" },
		{ game::MAG_095_TargetSpark_State4_Nop, (void *)a_73A6D0, "095 nullsub_1201" },
		{ game::MAG_095_TargetSparkSpawner_State3_Nop, (void *)a_73A6D0, "095 nullsub_1202" },
		{ game::MAG_095_SurfaceSplash_State2_Nop, (void *)a_73A6D0, "095 nullsub_1203" },
		{ game::MAG_095_Siren_WaterSurface_State5_Empty, (void *)a_73A6D0, "095 nullsub_1204" },
		{ game::MAG_095_StageWobbleIn_State2_Nop, (void *)a_73A6D0, "095 nullsub_1205" },
		{ game::MAG_095_StageWobbleOut_State2_Nop, (void *)a_73A6D0, "095 nullsub_1206" },
		{ game::MAG_095_Droplet_State2_Nop, (void *)a_73A6D0, "095 nullsub_1207" },
		{ game::MAG_095_Bubble_State2_Nop, (void *)a_73A6D0, "095 nullsub_1208" },
		{ game::MAG_095_CreatureActor_State7_Empty, (void *)a_73A6D0, "095 nullsub_1209" },
		{ game::MAG_095_ActEng_Emitter_State4_Empty, (void *)a_73A6D0, "095 nullsub_1210" },
		{ game::MAG_095_Sequence_State10_Empty, (void *)a_73A6D0, "095 nullsub_1170" },
		{ game::MAG_095_GetMonstersCentre, (void *)a_73A720, "095 sub_73A720" },
		{ game::MAG_095_Siren_Director_State0_ArmLoadScriptStep, (void *)a_73A930, "095 sub_73A930" },
		{ game::MAG_095_Siren_Director_State8_ArmLoadScriptStep, (void *)a_73A930, "095 sub_73B8E0" },
		{ game::MAG_095_Siren_Director_State17_ArmLoadScriptStep, (void *)a_73A930, "095 sub_73BB00" },
		{ game::MAG_095_LoadScript_TryArmStep, (void *)a_73A950, "095 sub_73A950" },
		{ game::ActEng_DirStepGE, (void *)a_73AAE0, "095 sub_73AAE0" },
		{ game::MAG_095_Camera_ResetBlock, (void *)a_73AB00, "095 sub_73AB00" },
		{ game::ActEng_Camera_StartKeyMove, (void *)a_73AB20, "095 sub_73AB20" },
		{ game::ActEng_Camera_KeyStep, (void *)a_73AE10, "095 sub_73AE10" },
		{ game::ActEng_Camera_WriteBattleCamera, (void *)a_73B160, "095 sub_73B160" },
		{ game::MAG_095_Siren_CameraScript_State1_CamKeyStep, (void *)a_73B2D0, "095 sub_73B2D0" },
		{ game::MAG_095_Siren_CameraScript_State4_CamKeyStepThenKeyAtStep4, (void *)a_73B350, "095 sub_73B350" },
		{ game::ActEng_CamScript_Start, (void *)a_73B3B0, "095 sub_73B3B0" },
		{ game::MAG_095_CamScript_ReadKey, (void *)a_73B430, "095 sub_73B430" },
		{ game::ActEng_CamScript_Tick, (void *)a_73B4B0, "095 sub_73B4B0" },
		{ game::MAG_095_CamScript_ChannelStep, (void *)a_73B520, "095 sub_73B520" },
		{ game::MAG_095_CamScript_RunKey, (void *)a_73B570, "095 sub_73B570" },
		{ game::MAG_095_Siren_CameraScript_State7_WaitDirGate5, (void *)a_73B620, "095 sub_73B620" },
		{ game::ActEng_DirSyncGate, (void *)a_73B640, "095 sub_73B640" },
		{ game::MAG_095_Siren_CameraScript_State13_FinishNode, (void *)a_73B750, "095 sub_73B750" },
		{ game::MAG_095_Siren_Director_State2_WaitDirGate1, (void *)a_73B790, "095 sub_73B790" },
		{ game::ActEng_DirPhaseGE, (void *)a_73B7E0, "095 sub_73B7E0" },
		{ game::MAG_095_Siren_Director_State6_WaitDirGate2, (void *)a_73B8A0, "095 sub_73B8A0" },
		{ game::MAG_095_Siren_Director_State7_WaitDirPhase2, (void *)a_73B8C0, "095 sub_73B8C0" },
		{ game::MAG_095_HideMonsters, (void *)a_73B9F0, "095 sub_73B9F0" },
		{ game::MAG_095_RestoreMonsters, (void *)a_73BA90, "095 sub_73BA90" },
		{ game::MAG_095_SetPauseSwitch, (void *)a_73BB60, "095 sub_73BB60" },
		{ game::MAG_095_Siren_Director_State20_WaitDirGate7, (void *)a_73BBD0, "095 sub_73BBD0" },
		{ game::MAG_095_Siren_Director_State23_CountdownTimer, (void *)a_73BC40, "095 sub_73BC40" },
		{ game::MAG_095_Siren_Director_State26_WaitDirGate9Finish, (void *)a_73BCB0, "095 sub_73BCB0" },
		{ game::MAG_095_SpawnChildTask, (void *)a_73C100, "095 sub_73C100" },
		{ game::ActEng_DrawPrimModel, (void *)a_73C280, "095 sub_73C280" },
		{ game::ActEng_EmitG3, (void *)a_73D770, "095 sub_73D770" },
		{ game::ActEng_EmitG4, (void *)a_73D9C0, "095 sub_73D9C0" },
		{ game::MAG_095_ModelTaskC, (void *)a_73F110, "095 sub_73F110" },
		{ game::MAG_095_ModelTaskE, (void *)a_73F110, "095 sub_73F370" },
		{ game::MAG_095_ModelTaskF, (void *)a_73F110, "095 sub_73F500" },
		{ game::ActEng_DirectorSetup, (void *)a_73F990, "095 sub_73F990" },
		{ game::MAG_095_Director_StartPositions, (void *)a_73FC20, "095 sub_73FC20" },
		{ game::MAG_095_Director_Mode3Setup, (void *)a_73FE90, "095 sub_73FE90" },
		{ game::MAG_095_Director_TargetPositions, (void *)a_73FFB0, "095 sub_73FFB0" },
		{ game::MAG_095_RelocateActorData, (void *)a_740210, "095 sub_740210" },
		{ game::MAG_095_ActSys_DrawSpritePass, (void *)a_740700, "095 sub_740700" },
		{ game::MAG_095_ActSys_DrawActorSprite, (void *)a_7407C0, "095 sub_7407C0" },
		{ game::ActEng_ActorStep, (void *)a_740880, "095 sub_740880" },
		{ game::ActEng_ActorMotion, (void *)a_740910, "095 sub_740910" },
		{ game::MAG_095_DampValue, (void *)a_740F70, "095 sub_740F70" },
		{ game::MAG_095_DampVector, (void *)a_740FC0, "095 sub_740FC0" },
		{ game::MAG_095_Instance_AdvanceTexFrame, (void *)a_741050, "095 sub_741050" },
		{ game::ActEng_InstanceBuildTransform, (void *)a_741120, "095 sub_741120" },
		{ game::MAG_095_Instance_EndOfLifeTest, (void *)a_7419C0, "095 sub_7419C0" },
		{ game::MAG_095_Instance_SpawnFollowUp, (void *)a_741A40, "095 sub_741A40" },
		{ game::ActEng_AllocParticle, (void *)a_741A70, "095 sub_741A70" },
		{ game::MAG_095_Particle_LifeCountdown, (void *)a_741B60, "095 sub_741B60" },
		{ game::MAG_095_RandRange, (void *)a_742290, "095 au_re__rand_8" },
		{ game::MAG_095_RandRangeWide, (void *)a_7422C0, "095 sub_7422C0" },
		{ game::MAG_095_RandVec3, (void *)a_742300, "095 sub_742300" },
		{ game::MAG_095_JitterAngles3, (void *)a_742350, "095 sub_742350" },
		{ game::MAG_095_RandAngleJitter, (void *)a_7423A0, "095 au_re__rand_8_0" },
		{ game::MAG_095_GetWord8Minus1, (void *)a_7424B0, "095 sub_7424B0" },
		{ game::MAG_095_Particle_AttachToOwner, (void *)a_7424C0, "095 sub_7424C0" },
		{ game::MAG_095_ParticleList_Append, (void *)a_742CA0, "095 sub_742CA0" },
		{ game::MAG_095_ParticleList_Unlink, (void *)a_742CD0, "095 sub_742CD0" },
		{ game::MAG_095_Particle_Init, (void *)a_742D00, "095 sub_742D00" },
		{ game::MAG_095_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "095 sub_742EB0" },
		{ game::MAG_095_Particle_PlaceAtPoint, (void *)a_742FF0, "095 sub_742FF0" },
		{ game::MAG_095_SpawnSubEffects, (void *)a_743100, "095 sub_743100" },
		{ game::MAG_095_Director_SpawnGroupEffects, (void *)a_743190, "095 sub_743190" },
		{ game::InitEffectSequenceFromData_c2, (void *)a_7435E0, "095 InitEffectSequenceFromData_c2" },
		{ game::SpriteStrip_DrawQuads, (void *)a_743720, "095 sub_743720" },
		{ game::MAG_095_BoxSparkle_State1_CounterFinish, (void *)a_743C00, "095 sub_743C00" },
		{ game::MAG_095_SurfaceSplash_State1_PlayUntilEnd, (void *)a_743C00, "095 sub_746780" },
		{ game::MAG_095_Droplet_State1_PlayUntilEnd, (void *)a_743C00, "095 sub_747130" },
		{ game::MAG_095_Bubble_State1_PlayUntilEnd, (void *)a_743C00, "095 sub_747340" },
		{ game::MAG_095_StepCounterLimit, (void *)a_743C20, "095 sub_743C20" },
		{ game::MAG_095_DrawNodePrimModel, (void *)a_7458E0, "095 sub_7458E0" },
		{ game::MAG_095_StageWobbleIn_State0_InitEntityRecords, (void *)a_746980, "095 sub_746980" },
		{ game::MAG_095_ClearEntityRecordBit1, (void *)a_746A10, "095 sub_746A10" },
		{ game::MAG_095_RestoreEntityRecordBit1, (void *)a_746AE0, "095 sub_746AE0" },
		{ game::GF_095Siren_DrawModel, (void *)a_746C10, "095 GF_095Siren_DrawModel" },
		{ game::MAG_095_CreatureActor_State6_AnimUntilDirPhase6, (void *)a_747400, "095 sub_747400" },
		{ game::MAG_095_ActEng_Emitter_State1_ApplyActionResults, (void *)a_747440, "095 MAG_095_sub_747440" },
		{ game::MAG_095_ActEng_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "095 MAG_095_sub_7474B0" },
		{ game::MAG_095_ActEng_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "095 MAG_095_sub_7474D0" },
		{ game::MAG_095_Sequence_State4_NextTargetLoop, (void *)a_747500, "095 MAG_095_sub_747500" },
		{ game::MAG_095_Sequence_State6_WaitQueuesEmpty, (void *)a_747550, "095 MAG_095_sub_747550" },
		{ game::MAG_095_Sequence_State7_RequestBattleSync, (void *)a_747560, "095 MAG_095_sub_747560" },
		{ game::MAG_095_Sequence_State8_WaitBattleSyncDone, (void *)a_747590, "095 MAG_095_sub_747590" },
		{ game::MAG_095_Sequence_State9_Finish, (void *)a_7475A0, "095 MAG_095_sub_7475A0" },
		{ {}, nullptr, nullptr }
	};
	static const AddrPort ENGINE_PORTS_096[] = {
		{ game::MAG_096_MiniMog_Master_State2_CarveWorkBuffers, (void *)a_732120, "096 MAG_096_sub_732120" },
		{ game::MAG_096_ClearWorkBuffers, (void *)a_732160, "096 MAG_096_sub_732160" },
		{ game::MAG_096_InitModuleState, (void *)a_7322A0, "096 MAG_096_sub_7322A0" },
		{ game::MAG_096_CameraScriptTask, (void *)a_732A00, "096 sub_732A00" },
		{ game::MAG_096_CameraScript_State11_CamKeyStepThenStartKey, (void *)a_7334C0, "096 sub_7334C0" },
		{ game::MAG_096_StageFadeIn_State0_ResetEntityRecords, (void *)a_7335D0, "096 sub_7335D0" },
		{ game::MAG_096_MusicFade_State2_WaitDirPhase4, (void *)a_733760, "096 sub_733760" },
		{ game::MAG_096_SetEntityColourTint, (void *)a_733950, "096 sub_733950" },
		{ game::MAG_096_MiniMog_Director_State4_WaitLoadScriptIdle, (void *)a_733AC0, "096 sub_733AC0" },
		{ game::MAG_096_MiniMog_Director_State5_Wait20Frames, (void *)a_733AE0, "096 sub_733AE0" },
		{ game::MAG_096_MiniMog_Director_State9_WaitDirGate3, (void *)a_733B70, "096 sub_733B70" },
		{ game::MAG_096_ScrollModelTextureV, (void *)a_7340A0, "096 sub_7340A0" },
		{ game::MAG_096_ActorSystem_State0_StartActorSystemAtFrame, (void *)a_7348A0, "096 sub_7348A0" },
		{ game::ActSys_Step, (void *)a_7353B0, "096 sub_7353B0" },
		{ game::MAG_096_ActSys_DrawPass, (void *)a_735440, "096 sub_735440" },
		{ game::MAG_096_ActSys_DrawActorModel, (void *)a_7354A0, "096 sub_7354A0" },
		{ game::MAG_096_RenderPrimModel, (void *)a_7355E0, "096 sub_7355E0" },
		{ game::MAG_096_RenderPolyFT3List, (void *)a_735BB0, "096 sub_735BB0" },
		{ game::MAG_096_RenderPolyFT4List, (void *)a_735DF0, "096 sub_735DF0" },
		{ game::MAG_096_ActSys_UpdatePass, (void *)a_736C00, "096 sub_736C00" },
		{ game::MAG_096_ActSys_UpdateEmitter, (void *)a_737ED0, "096 sub_737ED0" },
		{ game::MAG_096_ActSys_EmitterSpawnStep, (void *)a_737F50, "096 sub_737F50" },
		{ game::sub_737FD0, (void *)a_737FD0, "096 sub_737FD0" },
		{ game::MAG_096_ActSys_AllocParticleSlot, (void *)a_7387B0, "096 sub_7387B0" },
		{ game::MAG_096_ActorSystem_State1_StepActorSystem, (void *)a_7395E0, "096 sub_7395E0" },
		{ game::TexAnim_Step, (void *)a_739890, "096 sub_739890" },
		{ game::TexAnim_UploadFrame, (void *)a_739960, "096 sub_739960" },
		{ game::MAG_096_TexAnim_Start, (void *)a_739AC0, "096 sub_739AC0" },
		{ game::MAG_096_MiniMog_CreatureActor_State2_WaitModelAnimDone, (void *)a_739B40, "096 sub_739B40" },
		{ game::MAG_096_MiniMog_Master_State0_Advance, (void *)a_73A0D0, "096 MAG_096_sub_732100" },
		{ game::MAG_096_MiniMog_Master_State1_Advance, (void *)a_73A0D0, "096 MAG_096_sub_732110" },
		{ game::MAG_096_MiniMog_Master_State5_Advance, (void *)a_73A0D0, "096 MAG_096_sub_739CF0" },
		{ game::MAG_096_MiniMog_Master_State3_SpawnEmitter, (void *)a_73A170, "096 MAG_096_sub_7321A0" },
		{ game::MAG_096_EmitterTask, (void *)a_73A1A0, "096 MAG_096_sub_7321D0" },
		{ game::MAG_096_FileLoadScriptTask, (void *)a_73A380, "096 MAG_096_sub_732390" },
		{ game::MAG_096_StageFadeInTask, (void *)a_73A380, "096 sub_733570" },
		{ game::MAG_096_EntityFadeInTask, (void *)a_73A380, "096 sub_733810" },
		{ game::MAG_096_StageFadeOutTask, (void *)a_73A380, "096 sub_733BE0" },
		{ game::MAG_096_EntityFadeOutTask, (void *)a_73A380, "096 sub_733CE0" },
		{ game::MAG_096_ActorSystemTask, (void *)a_73A380, "096 sub_734840" },
		{ game::MAG_096_LoadScript_Issue, (void *)a_73A3E0, "096 sub_7323F0" },
		{ game::MAG_096_LoadCb_UploadTim, (void *)a_73A580, "096 sub_732590" },
		{ game::MAG_096_LoadCb_SummonStream, (void *)a_73A5B0, "096 sub_7325C0" },
		{ game::MAG_096_LoadCb_ClearBusy, (void *)a_73A5E0, "096 sub_7325F0" },
		{ game::MAG_096_LoadCb_ClearBusy_2, (void *)a_73A5E0, "096 sub_732600" },
		{ game::MAG_096_LoadCb_ClearBusy_3, (void *)a_73A5E0, "096 sub_732610" },
		{ game::MAG_096_LoadCb_ClearBusy_4, (void *)a_73A5E0, "096 sub_732620" },
		{ game::MAG_096_LoadScript_RecordFile, (void *)a_73A620, "096 sub_732630" },
		{ game::MAG_096_FileLoadScript_State1_LoadScriptWait, (void *)a_73A660, "096 sub_732670" },
		{ game::MAG_096_FileLoadScript_State2_Empty, (void *)a_73A6D0, "096 nullsub_1156" },
		{ game::MAG_096_St_Empty, (void *)a_73A6D0, "096 nullsub_1166" },
		{ game::MAG_096_CameraScript_State15_Empty, (void *)a_73A6D0, "096 nullsub_1167" },
		{ game::MAG_096_StageFadeIn_State2_Empty, (void *)a_73A6D0, "096 nullsub_1168" },
		{ game::MAG_096_MusicFade_State4_Empty, (void *)a_73A6D0, "096 nullsub_1169" },
		{ game::MAG_096_EntityFadeIn_State2_Empty, (void *)a_73A6D0, "096 nullsub_1157" },
		{ game::MAG_096_StageFadeOut_State2_Empty, (void *)a_73A6D0, "096 nullsub_1158" },
		{ game::MAG_096_EntityFadeOut_State2_Empty, (void *)a_73A6D0, "096 nullsub_1159" },
		{ game::MAG_096_MiniMog_Director_State14_Empty, (void *)a_73A6D0, "096 nullsub_1160" },
		{ game::MAG_096_St_Empty_2, (void *)a_73A6D0, "096 nullsub_1161" },
		{ game::MAG_096_ParticleModel_State2_Empty, (void *)a_73A6D0, "096 nullsub_1162" },
		{ game::MAG_096_ActorSystem_State2_Empty, (void *)a_73A6D0, "096 nullsub_1163" },
		{ game::MAG_096_MiniMog_CreatureActor_State5_Empty, (void *)a_73A6D0, "096 nullsub_1164" },
		{ game::MAG_096_Emitter_State4_Empty, (void *)a_73A6D0, "096 nullsub_1165" },
		{ game::MAG_096_MiniMog_Master_State10_Empty, (void *)a_73A6D0, "096 nullsub_1155" },
		{ game::MAG_096_GetMonstersCentre, (void *)a_73A720, "096 sub_732720" },
		{ game::MAG_096_MiniMog_Director_State3_ArmLoadScriptStep, (void *)a_73A930, "096 sub_733A60" },
		{ game::MAG_096_LoadScript_TryArmStep, (void *)a_73A950, "096 sub_733A80" },
		{ game::MAG_096_DirStepGE, (void *)a_73AAE0, "096 sub_7332F0" },
		{ game::MAG_096_Camera_ResetBlock, (void *)a_73AB00, "096 sub_732AF0" },
		{ game::MAG_096_Camera_StartKeyMove, (void *)a_73AB20, "096 sub_732B10" },
		{ game::MAG_096_Camera_KeyStep, (void *)a_73AE10, "096 sub_732E00" },
		{ game::MAG_096_Camera_WriteBattleCamera, (void *)a_73B160, "096 sub_733150" },
		{ game::MAG_096_CameraScript_State12_CamKeyStep, (void *)a_73B2D0, "096 sub_7334F0" },
		{ game::MAG_096_DirSyncGate, (void *)a_73B640, "096 sub_732920" },
		{ game::MAG_096_CameraScript_State14_FinishNode, (void *)a_73B750, "096 sub_733530" },
		{ game::MAG_096_MiniMog_Director_State0_WaitDirGate1, (void *)a_73B790, "096 sub_732900" },
		{ game::MAG_096_DirPhaseGE, (void *)a_73B7E0, "096 sub_7329E0" },
		{ game::MAG_096_MiniMog_Director_State6_WaitDirGate2, (void *)a_73B8A0, "096 sub_733B00" },
		{ game::MAG_096_HideMonsters, (void *)a_73B9F0, "096 sub_7338D0" },
		{ game::MAG_096_RestoreMonsters, (void *)a_73BA90, "096 sub_733D60" },
		{ game::MAG_096_MiniMog_Director_State8_CountdownTimer, (void *)a_73BC40, "096 sub_733B50" },
		{ game::MAG_096_MiniMog_Director_State11_CountdownTimer, (void *)a_73BC40, "096 sub_733DF0" },
		{ game::MAG_096_SpawnChildTask, (void *)a_73C100, "096 sub_733F20" },
		{ game::MAG_096_MusicFadeTask, (void *)a_73F110, "096 sub_733690" },
		{ game::MAG_096_DirectorSetup, (void *)a_73F990, "096 sub_7348D0" },
		{ game::MAG_096_Director_StartPositions, (void *)a_73FC20, "096 sub_734B60" },
		{ game::MAG_096_Director_Mode3Setup, (void *)a_73FE90, "096 sub_734DD0" },
		{ game::MAG_096_Director_TargetPositions, (void *)a_73FFB0, "096 sub_734EF0" },
		{ game::MAG_096_RelocateActorData, (void *)a_740210, "096 sub_735150" },
		{ game::MAG_096_ActSys_DrawSpritePass, (void *)a_740700, "096 sub_736AC0" },
		{ game::MAG_096_ActSys_DrawActorSprite, (void *)a_7407C0, "096 sub_736B80" },
		{ game::MAG_096_ActorStep, (void *)a_740880, "096 sub_736C40" },
		{ game::MAG_096_ActorMotion, (void *)a_740910, "096 sub_736CD0" },
		{ game::MAG_096_DampValue, (void *)a_740F70, "096 sub_737330" },
		{ game::MAG_096_DampVector, (void *)a_740FC0, "096 sub_737380" },
		{ game::MAG_096_Instance_AdvanceTexFrame, (void *)a_741050, "096 sub_737410" },
		{ game::MAG_096_Instance_BuildTransform, (void *)a_741120, "096 sub_7374E0" },
		{ game::MAG_096_Instance_EndOfLifeTest, (void *)a_7419C0, "096 sub_737D80" },
		{ game::MAG_096_Instance_SpawnFollowUp, (void *)a_741A40, "096 sub_737E00" },
		{ game::MAG_096_AllocParticle, (void *)a_741A70, "096 sub_737E30" },
		{ game::MAG_096_Particle_LifeCountdown, (void *)a_741B60, "096 sub_737F20" },
		{ game::MAG_096_RandRange, (void *)a_742290, "096 au_re__rand_7" },
		{ game::MAG_096_RandRangeWide, (void *)a_7422C0, "096 sub_738680" },
		{ game::MAG_096_RandVec3, (void *)a_742300, "096 sub_7386C0" },
		{ game::MAG_096_JitterAngles3, (void *)a_742350, "096 sub_738710" },
		{ game::MAG_096_RandAngleJitter, (void *)a_7423A0, "096 au_re__rand_7_0" },
		{ game::MAG_096_GetWord8Minus1, (void *)a_7424B0, "096 sub_738870" },
		{ game::MAG_096_Particle_AttachToOwner, (void *)a_7424C0, "096 sub_738880" },
		{ game::MAG_096_ParticleList_Append, (void *)a_742CA0, "096 sub_739060" },
		{ game::MAG_096_ParticleList_Unlink, (void *)a_742CD0, "096 sub_739090" },
		{ game::MAG_096_Particle_Init, (void *)a_742D00, "096 sub_7390C0" },
		{ game::MAG_096_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "096 sub_739270" },
		{ game::MAG_096_Particle_PlaceAtPoint, (void *)a_742FF0, "096 sub_7393B0" },
		{ game::MAG_096_SpawnSubEffects, (void *)a_743100, "096 sub_7394C0" },
		{ game::MAG_096_Director_SpawnGroupEffects, (void *)a_743190, "096 sub_739550" },
		{ game::MAG_096_ClearEntityRecordBit1, (void *)a_746A10, "096 sub_733660" },
		{ game::MAG_096_RestoreEntityRecordBit1, (void *)a_746AE0, "096 sub_733C60" },
		{ game::MAG_096_DrawBattleModel, (void *)a_746C10, "096 sub_7396B0" },
		{ game::MAG_096_Emitter_State1_ApplyActionResults, (void *)a_747440, "096 MAG_096_sub_739BF0" },
		{ game::MAG_096_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "096 MAG_096_sub_739C60" },
		{ game::MAG_096_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "096 MAG_096_sub_739C80" },
		{ game::MAG_096_MiniMog_Master_State4_NextTargetLoop, (void *)a_747500, "096 MAG_096_sub_739CB0" },
		{ game::MAG_096_MiniMog_Master_State6_WaitQueuesEmpty, (void *)a_747550, "096 MAG_096_sub_739D00" },
		{ game::MAG_096_MiniMog_Master_State7_RequestBattleSync, (void *)a_747560, "096 MAG_096_sub_739D10" },
		{ game::MAG_096_MiniMog_Master_State8_WaitBattleSyncDone, (void *)a_747590, "096 MAG_096_sub_739D40" },
		{ game::MAG_096_MiniMog_Director_State13_Finish, (void *)a_7475A0, "096 sub_733E50" },
		{ game::MAG_096_MiniMog_Master_State9_Finish, (void *)a_7475A0, "096 MAG_096_sub_739D50" },
		{ {}, nullptr, nullptr }
	};
	static const AddrPort ENGINE_PORTS_097[] = {
		{ game::MAG_097_Boko_Master_State2_CarveWorkBuffers, (void *)a_732120, "097 MAG_097_sub_729D70" },
		{ game::MAG_097_ClearWorkBuffers, (void *)a_732160, "097 MAG_097_sub_729DB0" },
		{ game::MAG_097_EmitterStage_State4_CamKeyStepThenStartKey, (void *)a_7334C0, "097 sub_7304E0" },
		{ game::MAG_097_EntityFadeIn_State0_Clear, (void *)a_7335D0, "097 sub_731AB0" },
		{ game::MAG_097_Director_State8_WaitLoadScriptIdle, (void *)a_733AC0, "097 sub_731050" },
		{ game::MAG_097_Director_State9_WaitDirGate3, (void *)a_733B70, "097 sub_731070" },
		{ game::MAG_097_UvModel_ScrollTextureV, (void *)a_7340A0, "097 sub_7311C0" },
		{ game::MAG_097_ActSys_DrawPass, (void *)a_735440, "097 sub_72B720" },
		{ game::MAG_097_ActSys_DrawActorModel, (void *)a_7354A0, "097 sub_72B780" },
		{ game::MAG_097_RenderPrimModel, (void *)a_7355E0, "097 sub_72B8C0" },
		{ game::MAG_097_RenderPolyFT3List, (void *)a_735BB0, "097 sub_72BE90" },
		{ game::MAG_097_RenderPolyFT4List, (void *)a_735DF0, "097 sub_72C0D0" },
		{ game::MAG_097_ActSys_UpdateEmitter, (void *)a_737ED0, "097 sub_72E480" },
		{ game::MAG_097_ActSys_EmitterSpawnStep, (void *)a_737F50, "097 sub_72E500" },
		{ game::MAG_097_ActSys_SpawnParticles, (void *)a_737FD0, "097 sub_72E580" },
		{ game::MAG_097_ActSys_AllocParticleSlot, (void *)a_7387B0, "097 sub_72ED60" },
		{ game::MAG_097_TexAnim_Step, (void *)a_739890, "097 sub_730940" },
		{ game::MAG_097_TexAnim_UploadFrame, (void *)a_739960, "097 sub_730A10" },
		{ game::MAG_097_TexAnim_Start, (void *)a_739AC0, "097 sub_730B60" },
		{ game::MAG_097_Boko_CreatureActor_State1_WaitModelAnimDone, (void *)a_739B40, "097 sub_730BA0" },
		{ game::MAG_097_Boko_Master_State0_Advance, (void *)a_73A0D0, "097 MAG_097_sub_729D50" },
		{ game::MAG_097_Boko_Master_State1_Advance, (void *)a_73A0D0, "097 MAG_097_sub_729D60" },
		{ game::MAG_097_Boko_Master_State5_Advance, (void *)a_73A0D0, "097 MAG_097_sub_731D30" },
		{ game::MAG_097_Boko_Master_State3_SpawnEmitter, (void *)a_73A170, "097 MAG_097_sub_729DF0" },
		{ game::MAG_097_EmitterTask, (void *)a_73A1A0, "097 MAG_097_sub_729E20" },
		{ game::MAG_097_FileLoadScriptTask, (void *)a_73A380, "097 MAG_097_sub_729FF0" },
		{ game::MAG_097_MainModelTask, (void *)a_73A380, "097 sub_72A660" },
		{ game::MAG_097_EmitterStageSpawnTask, (void *)a_73A380, "097 sub_72AB20" },
		{ game::MAG_097_EntityFadeOutTask, (void *)a_73A380, "097 sub_731630" },
		{ game::MAG_097_EntityFadeInTask, (void *)a_73A380, "097 sub_731A50" },
		{ game::MAG_097_EntityFadeInBrightTask, (void *)a_73A380, "097 sub_731B40" },
		{ game::MAG_097_LoadScript_Issue, (void *)a_73A3E0, "097 sub_72A050" },
		{ game::MAG_097_LoadCb_UploadTim, (void *)a_73A580, "097 sub_72A1F0" },
		{ game::MAG_097_LoadCb_SummonStream, (void *)a_73A5B0, "097 sub_72A220" },
		{ game::MAG_097_LoadCb_ClearBusy, (void *)a_73A5E0, "097 sub_72A250" },
		{ game::MAG_097_LoadCb_ClearBusy_2, (void *)a_73A5E0, "097 sub_72A260" },
		{ game::MAG_097_LoadCb_ClearBusy_3, (void *)a_73A5E0, "097 sub_72A270" },
		{ game::MAG_097_LoadCb_ClearBusy_4, (void *)a_73A5E0, "097 sub_72A280" },
		{ game::MAG_097_LoadScript_RecordFile, (void *)a_73A620, "097 sub_72A290" },
		{ game::MAG_097_FileLoadScript_State1_LoadScriptWait, (void *)a_73A660, "097 sub_72A2D0" },
		{ game::MAG_097_FileLoadScript_State2_Empty, (void *)a_73A6D0, "097 nullsub_1139" },
		{ game::MAG_097_MainModel_State2_Nop, (void *)a_73A6D0, "097 nullsub_1140" },
		{ game::MAG_097_EmitterStageSpawn_State2_Nop, (void *)a_73A6D0, "097 nullsub_1141" },
		{ game::MAG_097_St_Empty, (void *)a_73A6D0, "097 nullsub_1142" },
		{ game::MAG_097_EmitterStage_State7_Empty, (void *)a_73A6D0, "097 nullsub_1143" },
		{ game::MAG_097_Boko_Shadow_State5_Empty, (void *)a_73A6D0, "097 nullsub_1144" },
		{ game::MAG_097_Boko_CreatureActor_State9_Empty, (void *)a_73A6D0, "097 nullsub_1145" },
		{ game::MAG_097_UvModel_State2_Nop, (void *)a_73A6D0, "097 nullsub_1146" },
		{ game::MAG_097_EntityFadeOut_State2_Nop, (void *)a_73A6D0, "097 nullsub_1147" },
		{ game::MAG_097_Director_State17_Empty, (void *)a_73A6D0, "097 nullsub_1148" },
		{ game::MAG_097_St_Empty_2, (void *)a_73A6D0, "097 nullsub_1149" },
		{ game::MAG_097_Boko_ParticleTaskA_State2_Empty, (void *)a_73A6D0, "097 nullsub_1150" },
		{ game::MAG_097_Boko_ParticleTaskB_State2_Empty, (void *)a_73A6D0, "097 nullsub_1151" },
		{ game::MAG_097_EntityFadeIn_State2_Nop, (void *)a_73A6D0, "097 nullsub_1152" },
		{ game::MAG_097_EntityFadeInBright_State2_Nop, (void *)a_73A6D0, "097 nullsub_1153" },
		{ game::MAG_097_Emitter_State4_Empty, (void *)a_73A6D0, "097 nullsub_1154" },
		{ game::MAG_097_Boko_Master_State10_Empty, (void *)a_73A6D0, "097 nullsub_1138" },
		{ game::MAG_097_GetMonstersCentre, (void *)a_73A720, "097 sub_72A380" },
		{ game::MAG_097_Director_State1_ArmLoadScriptStep, (void *)a_73A930, "097 sub_730570" },
		{ game::MAG_097_Director_State4_ArmLoadScriptStep, (void *)a_73A930, "097 sub_730FC0" },
		{ game::MAG_097_Director_State7_ArmLoadScriptStep, (void *)a_73A930, "097 sub_731030" },
		{ game::MAG_097_LoadScript_TryArmStep, (void *)a_73A950, "097 sub_730590" },
		{ game::MAG_097_DirStepGE, (void *)a_73AAE0, "097 sub_730490" },
		{ game::MAG_097_Camera_ResetBlock, (void *)a_73AB00, "097 sub_72FC70" },
		{ game::MAG_097_Camera_StartKeyMove, (void *)a_73AB20, "097 sub_72FC90" },
		{ game::MAG_097_Camera_KeyStep, (void *)a_73AE10, "097 sub_72FF80" },
		{ game::MAG_097_Camera_WriteBattleCamera, (void *)a_73B160, "097 sub_7302D0" },
		{ game::MAG_097_EmitterStage_State1_CamKeyStep, (void *)a_73B2D0, "097 sub_730440" },
		{ game::MAG_097_EmitterStage_State3_CamKeyStepThenKeyAtStep4, (void *)a_73B350, "097 sub_7304B0" },
		{ game::MAG_097_DirSyncGate, (void *)a_73B640, "097 sub_730EC0" },
		{ game::MAG_097_EmitterStage_State6_FinishNode, (void *)a_73B750, "097 sub_730530" },
		{ game::MAG_097_DirPhaseGE, (void *)a_73B7E0, "097 sub_730E80" },
		{ game::MAG_097_Director_State6_WaitDirPhase2, (void *)a_73B8C0, "097 sub_731010" },
		{ game::MAG_097_SetPauseSwitch, (void *)a_73BB60, "097 sub_730610" },
		{ game::MAG_097_Director_State15_CountdownTimer, (void *)a_73BC40, "097 sub_731700" },
		{ game::MAG_097_SpawnChildTask, (void *)a_73C100, "097 sub_72A5C0" },
		{ game::MAG_097_DirectorSetup, (void *)a_73F990, "097 sub_72ABB0" },
		{ game::MAG_097_Director_StartPositions, (void *)a_73FC20, "097 sub_72AE40" },
		{ game::MAG_097_Director_Mode3Setup, (void *)a_73FE90, "097 sub_72B0B0" },
		{ game::MAG_097_Director_TargetPositions, (void *)a_73FFB0, "097 sub_72B1D0" },
		{ game::MAG_097_RelocateActorData, (void *)a_740210, "097 sub_72B430" },
		{ game::MAG_097_ActSys_DrawSpritePass, (void *)a_740700, "097 sub_72CDA0" },
		{ game::MAG_097_ActSys_DrawActorSprite, (void *)a_7407C0, "097 sub_72CE60" },
		{ game::MAG_097_ActorMotion, (void *)a_740910, "097 sub_72CFB0" },
		{ game::MAG_097_DampValue, (void *)a_740F70, "097 sub_72D610" },
		{ game::MAG_097_DampVector, (void *)a_740FC0, "097 sub_72D660" },
		{ game::MAG_097_Instance_AdvanceTexFrame, (void *)a_741050, "097 sub_72D6F0" },
		{ game::MAG_097_Instance_EndOfLifeTest, (void *)a_7419C0, "097 sub_72E330" },
		{ game::MAG_097_Instance_SpawnFollowUp, (void *)a_741A40, "097 sub_72E3B0" },
		{ game::MAG_097_AllocParticle, (void *)a_741A70, "097 sub_72E3E0" },
		{ game::MAG_097_Particle_LifeCountdown, (void *)a_741B60, "097 sub_72E4D0" },
		{ game::MAG_097_RandRange, (void *)a_742290, "097 au_re__rand_6" },
		{ game::MAG_097_RandRangeWide, (void *)a_7422C0, "097 sub_72EC30" },
		{ game::MAG_097_RandVec3, (void *)a_742300, "097 sub_72EC70" },
		{ game::MAG_097_JitterAngles3, (void *)a_742350, "097 sub_72ECC0" },
		{ game::MAG_097_RandAngleJitter, (void *)a_7423A0, "097 au_re__rand_6_0" },
		{ game::MAG_097_GetWord8Minus1, (void *)a_7424B0, "097 sub_72EE20" },
		{ game::MAG_097_Particle_AttachToOwner, (void *)a_7424C0, "097 sub_72EE30" },
		{ game::MAG_097_ParticleList_Append, (void *)a_742CA0, "097 sub_72F610" },
		{ game::MAG_097_ParticleList_Unlink, (void *)a_742CD0, "097 sub_72F640" },
		{ game::MAG_097_Particle_Init, (void *)a_742D00, "097 sub_72F670" },
		{ game::MAG_097_Particle_PlaceAtAnchorSet, (void *)a_742EB0, "097 sub_72F820" },
		{ game::MAG_097_Particle_PlaceAtPoint, (void *)a_742FF0, "097 sub_72F960" },
		{ game::MAG_097_SpawnSubEffects, (void *)a_743100, "097 sub_72FA70" },
		{ game::MAG_097_Director_SpawnGroupEffects, (void *)a_743190, "097 sub_72FB00" },
		{ game::MAG_097_Boko_Shadow_State4_CounterFinish, (void *)a_743C00, "097 sub_730E00" },
		{ game::MAG_097_StepCounterLimit, (void *)a_743C20, "097 sub_730DA0" },
		{ game::MAG_097_EntityFadeInBright_State0_Clear, (void *)a_746980, "097 sub_731BA0" },
		{ game::MAG_097_DrawBattleModel, (void *)a_746C10, "097 sub_730760" },
		{ game::MAG_097_Boko_CreatureActor_State8_AnimUntilDirPhase6, (void *)a_747400, "097 sub_730F80" },
		{ game::MAG_097_Emitter_State1_ApplyActionResults, (void *)a_747440, "097 MAG_097_sub_731C30" },
		{ game::MAG_097_Emitter_State2_WaitChildrenOrDirFlag, (void *)a_7474B0, "097 MAG_097_sub_731CA0" },
		{ game::MAG_097_Emitter_State3_FinishReleaseRoot, (void *)a_7474D0, "097 MAG_097_sub_731CC0" },
		{ game::MAG_097_Boko_Master_State4_NextTargetLoop, (void *)a_747500, "097 MAG_097_sub_731CF0" },
		{ game::MAG_097_Boko_Master_State6_WaitQueuesEmpty, (void *)a_747550, "097 MAG_097_sub_731D40" },
		{ game::MAG_097_Boko_Master_State7_RequestBattleSync, (void *)a_747560, "097 MAG_097_sub_731D50" },
		{ game::MAG_097_Boko_Master_State8_WaitBattleSyncDone, (void *)a_747590, "097 MAG_097_sub_731D80" },
		{ game::MAG_097_Boko_Master_State9_Finish, (void *)a_7475A0, "097 MAG_097_sub_731D90" },
		{ {}, nullptr, nullptr }
	};
	static const AddrPort ENGINE_PORTS_098[] = {
		{ game::MAG_098_sub_721B70, (void *)a_732120, "098 MAG_098_sub_721B70" },
		{ game::MAG_098_sub_721BB0, (void *)a_732160, "098 MAG_098_sub_721BB0" },
		{ game::sub_728020, (void *)a_7334C0, "098 sub_728020" },
		{ game::sub_729730, (void *)a_7335D0, "098 sub_729730" },
		{ game::sub_728B40, (void *)a_733AC0, "098 sub_728B40" },
		{ game::sub_728BA0, (void *)a_733AC0, "098 sub_728BA0" },
		{ game::sub_728BC0, (void *)a_733B70, "098 sub_728BC0" },
		{ game::sub_728E60, (void *)a_7340A0, "098 sub_728E60" },
		{ game::sub_722980, (void *)a_7348A0, "098 sub_722980" },
		{ game::sub_723490, (void *)a_7353B0, "098 sub_723490" },
		{ game::sub_723520, (void *)a_735440, "098 sub_723520" },
		{ game::sub_723580, (void *)a_7354A0, "098 sub_723580" },
		{ game::sub_7236C0, (void *)a_7355E0, "098 sub_7236C0" },
		{ game::sub_723C90, (void *)a_735BB0, "098 sub_723C90" },
		{ game::sub_723ED0, (void *)a_735DF0, "098 sub_723ED0" },
		{ game::sub_724CE0, (void *)a_736C00, "098 sub_724CE0" },
		{ game::sub_725FB0, (void *)a_737ED0, "098 sub_725FB0" },
		{ game::sub_726030, (void *)a_737F50, "098 sub_726030" },
		{ game::sub_7260B0, (void *)a_737FD0, "098 sub_7260B0" },
		{ game::sub_726890, (void *)a_7387B0, "098 sub_726890" },
		{ game::sub_7276C0, (void *)a_7395E0, "098 sub_7276C0" },
		{ game::sub_7284B0, (void *)a_739890, "098 sub_7284B0" },
		{ game::sub_728580, (void *)a_739960, "098 sub_728580" },
		{ game::sub_7286D0, (void *)a_739AC0, "098 sub_7286D0" },
		{ game::sub_728710, (void *)a_739B40, "098 sub_728710" },
		{ game::MAG_098_sub_721B50, (void *)a_73A0D0, "098 MAG_098_sub_721B50" },
		{ game::MAG_098_sub_721B60, (void *)a_73A0D0, "098 MAG_098_sub_721B60" },
		{ game::MAG_098_sub_7299B0, (void *)a_73A0D0, "098 MAG_098_sub_7299B0" },
		{ game::MAG_098_sub_721BF0, (void *)a_73A170, "098 MAG_098_sub_721BF0" },
		{ game::MAG_098_sub_721C20, (void *)a_73A1A0, "098 MAG_098_sub_721C20" },
		{ game::MAG_098_sub_721DF0, (void *)a_73A380, "098 MAG_098_sub_721DF0" },
		{ game::sub_722460, (void *)a_73A380, "098 sub_722460" },
		{ game::sub_722920, (void *)a_73A380, "098 sub_722920" },
		{ game::sub_728C60, (void *)a_73A380, "098 sub_728C60" },
		{ game::sub_7292D0, (void *)a_73A380, "098 sub_7292D0" },
		{ game::sub_7296D0, (void *)a_73A380, "098 sub_7296D0" },
		{ game::sub_7297C0, (void *)a_73A380, "098 sub_7297C0" },
		{ game::sub_721E50, (void *)a_73A3E0, "098 sub_721E50" },
		{ game::sub_721FF0, (void *)a_73A580, "098 sub_721FF0" },
		{ game::sub_722020, (void *)a_73A5B0, "098 sub_722020" },
		{ game::sub_722050, (void *)a_73A5E0, "098 sub_722050" },
		{ game::sub_722060, (void *)a_73A5E0, "098 sub_722060" },
		{ game::sub_722070, (void *)a_73A5E0, "098 sub_722070" },
		{ game::sub_722080, (void *)a_73A5E0, "098 sub_722080" },
		{ game::sub_722090, (void *)a_73A620, "098 sub_722090" },
		{ game::sub_7220D0, (void *)a_73A660, "098 sub_7220D0" },
		{ game::nullsub_1121, (void *)a_73A6D0, "098 nullsub_1121" },
		{ game::nullsub_1122, (void *)a_73A6D0, "098 nullsub_1122" },
		{ game::nullsub_1123, (void *)a_73A6D0, "098 nullsub_1123" },
		{ game::nullsub_1124, (void *)a_73A6D0, "098 nullsub_1124" },
		{ game::nullsub_1125, (void *)a_73A6D0, "098 nullsub_1125" },
		{ game::nullsub_1126, (void *)a_73A6D0, "098 nullsub_1126" },
		{ game::nullsub_1127, (void *)a_73A6D0, "098 nullsub_1127" },
		{ game::nullsub_1128, (void *)a_73A6D0, "098 nullsub_1128" },
		{ game::nullsub_1129, (void *)a_73A6D0, "098 nullsub_1129" },
		{ game::nullsub_1130, (void *)a_73A6D0, "098 nullsub_1130" },
		{ game::nullsub_1131, (void *)a_73A6D0, "098 nullsub_1131" },
		{ game::nullsub_1132, (void *)a_73A6D0, "098 nullsub_1132" },
		{ game::nullsub_1133, (void *)a_73A6D0, "098 nullsub_1133" },
		{ game::nullsub_1134, (void *)a_73A6D0, "098 nullsub_1134" },
		{ game::nullsub_1135, (void *)a_73A6D0, "098 nullsub_1135" },
		{ game::nullsub_1136, (void *)a_73A6D0, "098 nullsub_1136" },
		{ game::nullsub_1137, (void *)a_73A6D0, "098 nullsub_1137" },
		{ game::nullsub_1120, (void *)a_73A6D0, "098 nullsub_1120" },
		{ game::sub_722180, (void *)a_73A720, "098 sub_722180" },
		{ game::sub_7280E0, (void *)a_73A930, "098 sub_7280E0" },
		{ game::sub_728B80, (void *)a_73A930, "098 sub_728B80" },
		{ game::sub_728100, (void *)a_73A950, "098 sub_728100" },
		{ game::sub_727FD0, (void *)a_73AAE0, "098 sub_727FD0" },
		{ game::sub_7277B0, (void *)a_73AB00, "098 sub_7277B0" },
		{ game::sub_7277D0, (void *)a_73AB20, "098 sub_7277D0" },
		{ game::sub_727AC0, (void *)a_73AE10, "098 sub_727AC0" },
		{ game::sub_727E10, (void *)a_73B160, "098 sub_727E10" },
		{ game::sub_727F80, (void *)a_73B2D0, "098 sub_727F80" },
		{ game::sub_727FF0, (void *)a_73B350, "098 sub_727FF0" },
		{ game::sub_728A30, (void *)a_73B640, "098 sub_728A30" },
		{ game::sub_7280A0, (void *)a_73B750, "098 sub_7280A0" },
		{ game::sub_7289F0, (void *)a_73B7E0, "098 sub_7289F0" },
		{ game::sub_728B60, (void *)a_73B8C0, "098 sub_728B60" },
		{ game::sub_728180, (void *)a_73BB60, "098 sub_728180" },
		{ game::sub_7293A0, (void *)a_73BC40, "098 sub_7293A0" },
		{ game::sub_7223C0, (void *)a_73C100, "098 sub_7223C0" },
		{ game::sub_7229B0, (void *)a_73F990, "098 sub_7229B0" },
		{ game::sub_722C40, (void *)a_73FC20, "098 sub_722C40" },
		{ game::sub_722EB0, (void *)a_73FE90, "098 sub_722EB0" },
		{ game::sub_722FD0, (void *)a_73FFB0, "098 sub_722FD0" },
		{ game::sub_723230, (void *)a_740210, "098 sub_723230" },
		{ game::sub_724BA0, (void *)a_740700, "098 sub_724BA0" },
		{ game::sub_724C60, (void *)a_7407C0, "098 sub_724C60" },
		{ game::sub_724D20, (void *)a_740880, "098 sub_724D20" },
		{ game::sub_724DB0, (void *)a_740910, "098 sub_724DB0" },
		{ game::sub_725410, (void *)a_740F70, "098 sub_725410" },
		{ game::sub_725460, (void *)a_740FC0, "098 sub_725460" },
		{ game::sub_7254F0, (void *)a_741050, "098 sub_7254F0" },
		{ game::sub_7255C0, (void *)a_741120, "098 sub_7255C0" },
		{ game::sub_725E60, (void *)a_7419C0, "098 sub_725E60" },
		{ game::sub_725EE0, (void *)a_741A40, "098 sub_725EE0" },
		{ game::sub_725F10, (void *)a_741A70, "098 sub_725F10" },
		{ game::sub_726000, (void *)a_741B60, "098 sub_726000" },
		{ game::MAG_098_RandRange, (void *)a_742290, "098 au_re__rand_5" },
		{ game::sub_726760, (void *)a_7422C0, "098 sub_726760" },
		{ game::sub_7267A0, (void *)a_742300, "098 sub_7267A0" },
		{ game::sub_7267F0, (void *)a_742350, "098 sub_7267F0" },
		{ game::MAG_098_RandAngleJitter, (void *)a_7423A0, "098 au_re__rand_5_0" },
		{ game::sub_726950, (void *)a_7424B0, "098 sub_726950" },
		{ game::sub_726960, (void *)a_7424C0, "098 sub_726960" },
		{ game::sub_727140, (void *)a_742CA0, "098 sub_727140" },
		{ game::sub_727170, (void *)a_742CD0, "098 sub_727170" },
		{ game::sub_7271A0, (void *)a_742D00, "098 sub_7271A0" },
		{ game::sub_727350, (void *)a_742EB0, "098 sub_727350" },
		{ game::sub_727490, (void *)a_742FF0, "098 sub_727490" },
		{ game::sub_7275A0, (void *)a_743100, "098 sub_7275A0" },
		{ game::sub_727630, (void *)a_743190, "098 sub_727630" },
		{ game::sub_728970, (void *)a_743C00, "098 sub_728970" },
		{ game::sub_728910, (void *)a_743C20, "098 sub_728910" },
		{ game::sub_729820, (void *)a_746980, "098 sub_729820" },
		{ game::sub_7282D0, (void *)a_746C10, "098 sub_7282D0" },
		{ game::MAG_098_sub_7298B0, (void *)a_747440, "098 MAG_098_sub_7298B0" },
		{ game::MAG_098_sub_729920, (void *)a_7474B0, "098 MAG_098_sub_729920" },
		{ game::MAG_098_sub_729940, (void *)a_7474D0, "098 MAG_098_sub_729940" },
		{ game::MAG_098_sub_729970, (void *)a_747500, "098 MAG_098_sub_729970" },
		{ game::MAG_098_sub_7299C0, (void *)a_747550, "098 MAG_098_sub_7299C0" },
		{ game::MAG_098_sub_7299D0, (void *)a_747560, "098 MAG_098_sub_7299D0" },
		{ game::MAG_098_sub_729A00, (void *)a_747590, "098 MAG_098_sub_729A00" },
		{ game::MAG_098_sub_729A10, (void *)a_7475A0, "098 MAG_098_sub_729A10" },
		{ {}, nullptr, nullptr }
	};
	static const AddrPort ENGINE_PORTS_099[] = {
		{ game::MAG_099_sub_718080, (void *)a_732120, "099 MAG_099_sub_718080" },
		{ game::MAG_099_sub_7180C0, (void *)a_732160, "099 MAG_099_sub_7180C0" },
		{ game::sub_721070, (void *)a_7335D0, "099 sub_721070" },
		{ game::sub_71F260, (void *)a_733AC0, "099 sub_71F260" },
		{ game::sub_71F4E0, (void *)a_733AC0, "099 sub_71F4E0" },
		{ game::sub_71F280, (void *)a_733B70, "099 sub_71F280" },
		{ game::sub_71F9E0, (void *)a_7340A0, "099 sub_71F9E0" },
		{ game::sub_718ED0, (void *)a_7348A0, "099 sub_718ED0" },
		{ game::sub_7199E0, (void *)a_7353B0, "099 sub_7199E0" },
		{ game::sub_719A70, (void *)a_735440, "099 sub_719A70" },
		{ game::sub_719AD0, (void *)a_7354A0, "099 sub_719AD0" },
		{ game::sub_719C10, (void *)a_7355E0, "099 sub_719C10" },
		{ game::sub_71A1E0, (void *)a_735BB0, "099 sub_71A1E0" },
		{ game::sub_71A420, (void *)a_735DF0, "099 sub_71A420" },
		{ game::sub_71B230, (void *)a_736C00, "099 sub_71B230" },
		{ game::sub_71C500, (void *)a_737ED0, "099 sub_71C500" },
		{ game::sub_71C580, (void *)a_737F50, "099 sub_71C580" },
		{ game::sub_71C600, (void *)a_737FD0, "099 sub_71C600" },
		{ game::sub_71CDE0, (void *)a_7387B0, "099 sub_71CDE0" },
		{ game::sub_71DC10, (void *)a_7395E0, "099 sub_71DC10" },
		{ game::sub_71EB60, (void *)a_739890, "099 sub_71EB60" },
		{ game::sub_71EC30, (void *)a_739960, "099 sub_71EC30" },
		{ game::sub_71ED80, (void *)a_739AC0, "099 sub_71ED80" },
		{ game::sub_71EDC0, (void *)a_739B40, "099 sub_71EDC0" },
		{ game::MAG_099_sub_717ED0, (void *)a_739F40, "099 MAG_099_sub_717ED0" },
		{ game::MAG_099_sub_718060, (void *)a_73A0D0, "099 MAG_099_sub_718060" },
		{ game::MAG_099_sub_718070, (void *)a_73A0D0, "099 MAG_099_sub_718070" },
		{ game::MAG_099_sub_7217B0, (void *)a_73A0D0, "099 MAG_099_sub_7217B0" },
		{ game::MAG_099_sub_718100, (void *)a_73A170, "099 MAG_099_sub_718100" },
		{ game::MAG_099_sub_718130, (void *)a_73A1A0, "099 MAG_099_sub_718130" },
		{ game::MAG_099_sub_718310, (void *)a_73A380, "099 MAG_099_sub_718310" },
		{ game::sub_7189B0, (void *)a_73A380, "099 sub_7189B0" },
		{ game::sub_718E70, (void *)a_73A380, "099 sub_718E70" },
		{ game::sub_71F880, (void *)a_73A380, "099 sub_71F880" },
		{ game::sub_71FEC0, (void *)a_73A380, "099 sub_71FEC0" },
		{ game::sub_7201F0, (void *)a_73A380, "099 sub_7201F0" },
		{ game::sub_720320, (void *)a_73A380, "099 sub_720320" },
		{ game::sub_720410, (void *)a_73A380, "099 sub_720410" },
		{ game::sub_720610, (void *)a_73A380, "099 sub_720610" },
		{ game::sub_721010, (void *)a_73A380, "099 sub_721010" },
		{ game::sub_721100, (void *)a_73A380, "099 sub_721100" },
		{ game::sub_721460, (void *)a_73A380, "099 sub_721460" },
		{ game::sub_718370, (void *)a_73A3E0, "099 sub_718370" },
		{ game::sub_718510, (void *)a_73A580, "099 sub_718510" },
		{ game::sub_718540, (void *)a_73A5B0, "099 sub_718540" },
		{ game::sub_718570, (void *)a_73A5E0, "099 sub_718570" },
		{ game::sub_718580, (void *)a_73A5E0, "099 sub_718580" },
		{ game::sub_718590, (void *)a_73A5E0, "099 sub_718590" },
		{ game::sub_7185A0, (void *)a_73A5E0, "099 sub_7185A0" },
		{ game::sub_7185B0, (void *)a_73A620, "099 sub_7185B0" },
		{ game::sub_7185F0, (void *)a_73A660, "099 sub_7185F0" },
		{ game::nullsub_1088, (void *)a_73A6D0, "099 nullsub_1088" },
		{ game::nullsub_1089, (void *)a_73A6D0, "099 nullsub_1089" },
		{ game::nullsub_1090, (void *)a_73A6D0, "099 nullsub_1090" },
		{ game::nullsub_1119, (void *)a_73A6D0, "099 nullsub_1119" },
		{ game::nullsub_1091, (void *)a_73A6D0, "099 nullsub_1091" },
		{ game::nullsub_1092, (void *)a_73A6D0, "099 nullsub_1092" },
		{ game::nullsub_1093, (void *)a_73A6D0, "099 nullsub_1093" },
		{ game::nullsub_1094, (void *)a_73A6D0, "099 nullsub_1094" },
		{ game::nullsub_1095, (void *)a_73A6D0, "099 nullsub_1095" },
		{ game::nullsub_1096, (void *)a_73A6D0, "099 nullsub_1096" },
		{ game::nullsub_1097, (void *)a_73A6D0, "099 nullsub_1097" },
		{ game::nullsub_1098, (void *)a_73A6D0, "099 nullsub_1098" },
		{ game::nullsub_1099, (void *)a_73A6D0, "099 nullsub_1099" },
		{ game::nullsub_1100, (void *)a_73A6D0, "099 nullsub_1100" },
		{ game::nullsub_1101, (void *)a_73A6D0, "099 nullsub_1101" },
		{ game::nullsub_1102, (void *)a_73A6D0, "099 nullsub_1102" },
		{ game::nullsub_1103, (void *)a_73A6D0, "099 nullsub_1103" },
		{ game::nullsub_1104, (void *)a_73A6D0, "099 nullsub_1104" },
		{ game::nullsub_1105, (void *)a_73A6D0, "099 nullsub_1105" },
		{ game::nullsub_1106, (void *)a_73A6D0, "099 nullsub_1106" },
		{ game::nullsub_1107, (void *)a_73A6D0, "099 nullsub_1107" },
		{ game::nullsub_1108, (void *)a_73A6D0, "099 nullsub_1108" },
		{ game::nullsub_1109, (void *)a_73A6D0, "099 nullsub_1109" },
		{ game::nullsub_1110, (void *)a_73A6D0, "099 nullsub_1110" },
		{ game::nullsub_1111, (void *)a_73A6D0, "099 nullsub_1111" },
		{ game::nullsub_1112, (void *)a_73A6D0, "099 nullsub_1112" },
		{ game::nullsub_1113, (void *)a_73A6D0, "099 nullsub_1113" },
		{ game::nullsub_1114, (void *)a_73A6D0, "099 nullsub_1114" },
		{ game::nullsub_1115, (void *)a_73A6D0, "099 nullsub_1115" },
		{ game::nullsub_1116, (void *)a_73A6D0, "099 nullsub_1116" },
		{ game::nullsub_1117, (void *)a_73A6D0, "099 nullsub_1117" },
		{ game::nullsub_1087, (void *)a_73A6D0, "099 nullsub_1087" },
		{ game::sub_7186C0, (void *)a_73A720, "099 sub_7186C0" },
		{ game::sub_71E7A0, (void *)a_73A930, "099 sub_71E7A0" },
		{ game::sub_71F1D0, (void *)a_73A930, "099 sub_71F1D0" },
		{ game::sub_71F240, (void *)a_73A930, "099 sub_71F240" },
		{ game::sub_71E7C0, (void *)a_73A950, "099 sub_71E7C0" },
		{ game::sub_71E520, (void *)a_73AAE0, "099 sub_71E520" },
		{ game::sub_71DD00, (void *)a_73AB00, "099 sub_71DD00" },
		{ game::sub_71DD20, (void *)a_73AB20, "099 sub_71DD20" },
		{ game::sub_71E010, (void *)a_73AE10, "099 sub_71E010" },
		{ game::sub_71E360, (void *)a_73B160, "099 sub_71E360" },
		{ game::sub_71E4D0, (void *)a_73B2D0, "099 sub_71E4D0" },
		{ game::sub_71E540, (void *)a_73B350, "099 sub_71E540" },
		{ game::sub_71F380, (void *)a_73B620, "099 sub_71F380" },
		{ game::sub_71F110, (void *)a_73B640, "099 sub_71F110" },
		{ game::sub_71F0D0, (void *)a_73B7E0, "099 sub_71F0D0" },
		{ game::sub_71F220, (void *)a_73B8C0, "099 sub_71F220" },
		{ game::sub_71F410, (void *)a_73B9F0, "099 sub_71F410" },
		{ game::sub_721550, (void *)a_73BA90, "099 sub_721550" },
		{ game::sub_71E840, (void *)a_73BB60, "099 sub_71E840" },
		{ game::sub_71F580, (void *)a_73BC40, "099 sub_71F580" },
		{ game::sub_721680, (void *)a_73BCB0, "099 sub_721680" },
		{ game::sub_718910, (void *)a_73C100, "099 sub_718910" },
		{ game::sub_71F310, (void *)a_73F110, "099 sub_71F310" },
		{ game::sub_71FFB0, (void *)a_73F110, "099 sub_71FFB0" },
		{ game::sub_7215A0, (void *)a_73F110, "099 sub_7215A0" },
		{ game::sub_718F00, (void *)a_73F990, "099 sub_718F00" },
		{ game::sub_719190, (void *)a_73FC20, "099 sub_719190" },
		{ game::sub_719400, (void *)a_73FE90, "099 sub_719400" },
		{ game::sub_719520, (void *)a_73FFB0, "099 sub_719520" },
		{ game::sub_719780, (void *)a_740210, "099 sub_719780" },
		{ game::sub_71B0F0, (void *)a_740700, "099 sub_71B0F0" },
		{ game::sub_71B1B0, (void *)a_7407C0, "099 sub_71B1B0" },
		{ game::sub_71B270, (void *)a_740880, "099 sub_71B270" },
		{ game::sub_71B300, (void *)a_740910, "099 sub_71B300" },
		{ game::sub_71B960, (void *)a_740F70, "099 sub_71B960" },
		{ game::sub_71B9B0, (void *)a_740FC0, "099 sub_71B9B0" },
		{ game::sub_71BA40, (void *)a_741050, "099 sub_71BA40" },
		{ game::sub_71BB10, (void *)a_741120, "099 sub_71BB10" },
		{ game::sub_71C3B0, (void *)a_7419C0, "099 sub_71C3B0" },
		{ game::sub_71C430, (void *)a_741A40, "099 sub_71C430" },
		{ game::sub_71C460, (void *)a_741A70, "099 sub_71C460" },
		{ game::sub_71C550, (void *)a_741B60, "099 sub_71C550" },
		{ game::MAG_099_RandRange, (void *)a_742290, "099 au_re__rand_4" },
		{ game::sub_71CCB0, (void *)a_7422C0, "099 sub_71CCB0" },
		{ game::sub_71CCF0, (void *)a_742300, "099 sub_71CCF0" },
		{ game::sub_71CD40, (void *)a_742350, "099 sub_71CD40" },
		{ game::MAG_099_RandAngleJitter, (void *)a_7423A0, "099 au_re__rand_4_0" },
		{ game::sub_71CEA0, (void *)a_7424B0, "099 sub_71CEA0" },
		{ game::sub_71CEB0, (void *)a_7424C0, "099 sub_71CEB0" },
		{ game::sub_71D690, (void *)a_742CA0, "099 sub_71D690" },
		{ game::sub_71D6C0, (void *)a_742CD0, "099 sub_71D6C0" },
		{ game::sub_71D6F0, (void *)a_742D00, "099 sub_71D6F0" },
		{ game::sub_71D8A0, (void *)a_742EB0, "099 sub_71D8A0" },
		{ game::sub_71D9E0, (void *)a_742FF0, "099 sub_71D9E0" },
		{ game::sub_71DAF0, (void *)a_743100, "099 sub_71DAF0" },
		{ game::sub_71DB80, (void *)a_743190, "099 sub_71DB80" },
		{ game::sub_71F050, (void *)a_743C00, "099 sub_71F050" },
		{ game::sub_71EFE0, (void *)a_743C20, "099 sub_71EFE0" },
		{ game::sub_721230, (void *)a_7458E0, "099 sub_721230" },
		{ game::sub_71F3F0, (void *)a_746A10, "099 sub_71F3F0" },
		{ game::sub_721520, (void *)a_746AE0, "099 sub_721520" },
		{ game::sub_71E980, (void *)a_746C10, "099 sub_71E980" },
		{ game::MAG_099_sub_7216B0, (void *)a_747440, "099 MAG_099_sub_7216B0" },
		{ game::MAG_099_sub_721720, (void *)a_7474B0, "099 MAG_099_sub_721720" },
		{ game::MAG_099_sub_721740, (void *)a_7474D0, "099 MAG_099_sub_721740" },
		{ game::MAG_099_sub_721770, (void *)a_747500, "099 MAG_099_sub_721770" },
		{ game::MAG_099_sub_7217C0, (void *)a_747550, "099 MAG_099_sub_7217C0" },
		{ game::MAG_099_sub_7217D0, (void *)a_747560, "099 MAG_099_sub_7217D0" },
		{ game::MAG_099_sub_721800, (void *)a_747590, "099 MAG_099_sub_721800" },
		{ game::MAG_099_sub_721810, (void *)a_7475A0, "099 MAG_099_sub_721810" },
		{ {}, nullptr, nullptr }
	};
	static const AddrPort ENGINE_PORTS_100[] = {
		{ game::MAG_100_sub_70ECB0, (void *)a_732120, "100 MAG_100_sub_70ECB0" },
		{ game::MAG_100_sub_70ECF0, (void *)a_732160, "100 MAG_100_sub_70ECF0" },
		{ game::sub_715380, (void *)a_7334C0, "100 sub_715380" },
		{ game::sub_716AC0, (void *)a_7335D0, "100 sub_716AC0" },
		{ game::sub_715F00, (void *)a_733760, "100 sub_715F00" },
		{ game::sub_717A70, (void *)a_733950, "100 sub_717A70" },
		{ game::sub_715EC0, (void *)a_733AC0, "100 sub_715EC0" },
		{ game::sub_715F40, (void *)a_733AC0, "100 sub_715F40" },
		{ game::sub_715FE0, (void *)a_733AC0, "100 sub_715FE0" },
		{ game::sub_716020, (void *)a_733AE0, "100 sub_716020" },
		{ game::sub_715EE0, (void *)a_733B70, "100 sub_715EE0" },
		{ game::sub_716460, (void *)a_7340A0, "100 sub_716460" },
		{ game::sub_70FB50, (void *)a_7348A0, "100 sub_70FB50" },
		{ game::sub_710660, (void *)a_7353B0, "100 sub_710660" },
		{ game::sub_7106F0, (void *)a_735440, "100 sub_7106F0" },
		{ game::sub_710750, (void *)a_7354A0, "100 sub_710750" },
		{ game::sub_710890, (void *)a_7355E0, "100 sub_710890" },
		{ game::sub_710E60, (void *)a_735BB0, "100 sub_710E60" },
		{ game::sub_7110A0, (void *)a_735DF0, "100 sub_7110A0" },
		{ game::sub_711EB0, (void *)a_736C00, "100 sub_711EB0" },
		{ game::sub_713180, (void *)a_737ED0, "100 sub_713180" },
		{ game::sub_713200, (void *)a_737F50, "100 sub_713200" },
		{ game::sub_713280, (void *)a_737FD0, "100 sub_713280" },
		{ game::sub_713A60, (void *)a_7387B0, "100 sub_713A60" },
		{ game::sub_714890, (void *)a_7395E0, "100 sub_714890" },
		{ game::sub_7157E0, (void *)a_739890, "100 sub_7157E0" },
		{ game::sub_7158B0, (void *)a_739960, "100 sub_7158B0" },
		{ game::sub_715A00, (void *)a_739AC0, "100 sub_715A00" },
		{ game::sub_715A40, (void *)a_739B40, "100 sub_715A40" },
		{ game::MAG_100_sub_70EB00, (void *)a_739F40, "100 MAG_100_sub_70EB00" },
		{ game::MAG_100_sub_70EC90, (void *)a_73A0D0, "100 MAG_100_sub_70EC90" },
		{ game::MAG_100_sub_70ECA0, (void *)a_73A0D0, "100 MAG_100_sub_70ECA0" },
		{ game::MAG_100_sub_717C80, (void *)a_73A0D0, "100 MAG_100_sub_717C80" },
		{ game::MAG_100_sub_70ED30, (void *)a_73A170, "100 MAG_100_sub_70ED30" },
		{ game::MAG_100_sub_70ED60, (void *)a_73A1A0, "100 MAG_100_sub_70ED60" },
		{ game::MAG_100_sub_70EF30, (void *)a_73A380, "100 MAG_100_sub_70EF30" },
		{ game::sub_70F5F0, (void *)a_73A380, "100 sub_70F5F0" },
		{ game::sub_70FAF0, (void *)a_73A380, "100 sub_70FAF0" },
		{ game::sub_716300, (void *)a_73A380, "100 sub_716300" },
		{ game::sub_716950, (void *)a_73A380, "100 sub_716950" },
		{ game::sub_716A60, (void *)a_73A380, "100 sub_716A60" },
		{ game::sub_716B50, (void *)a_73A380, "100 sub_716B50" },
		{ game::sub_70EF90, (void *)a_73A3E0, "100 sub_70EF90" },
		{ game::sub_70F130, (void *)a_73A580, "100 sub_70F130" },
		{ game::sub_70F160, (void *)a_73A5B0, "100 sub_70F160" },
		{ game::sub_70F190, (void *)a_73A5E0, "100 sub_70F190" },
		{ game::sub_70F1A0, (void *)a_73A5E0, "100 sub_70F1A0" },
		{ game::sub_70F1B0, (void *)a_73A5E0, "100 sub_70F1B0" },
		{ game::sub_70F1C0, (void *)a_73A5E0, "100 sub_70F1C0" },
		{ game::sub_70F1D0, (void *)a_73A620, "100 sub_70F1D0" },
		{ game::sub_70F210, (void *)a_73A660, "100 sub_70F210" },
		{ game::nullsub_1069, (void *)a_73A6D0, "100 nullsub_1069" },
		{ game::nullsub_1070, (void *)a_73A6D0, "100 nullsub_1070" },
		{ game::nullsub_1071, (void *)a_73A6D0, "100 nullsub_1071" },
		{ game::nullsub_1072, (void *)a_73A6D0, "100 nullsub_1072" },
		{ game::nullsub_1073, (void *)a_73A6D0, "100 nullsub_1073" },
		{ game::nullsub_1074, (void *)a_73A6D0, "100 nullsub_1074" },
		{ game::nullsub_1075, (void *)a_73A6D0, "100 nullsub_1075" },
		{ game::nullsub_1076, (void *)a_73A6D0, "100 nullsub_1076" },
		{ game::nullsub_1077, (void *)a_73A6D0, "100 nullsub_1077" },
		{ game::nullsub_1078, (void *)a_73A6D0, "100 nullsub_1078" },
		{ game::nullsub_1079, (void *)a_73A6D0, "100 nullsub_1079" },
		{ game::nullsub_1080, (void *)a_73A6D0, "100 nullsub_1080" },
		{ game::nullsub_1081, (void *)a_73A6D0, "100 nullsub_1081" },
		{ game::nullsub_1082, (void *)a_73A6D0, "100 nullsub_1082" },
		{ game::nullsub_1083, (void *)a_73A6D0, "100 nullsub_1083" },
		{ game::nullsub_1084, (void *)a_73A6D0, "100 nullsub_1084" },
		{ game::nullsub_1085, (void *)a_73A6D0, "100 nullsub_1085" },
		{ game::nullsub_1086, (void *)a_73A6D0, "100 nullsub_1086" },
		{ game::nullsub_1068, (void *)a_73A6D0, "100 nullsub_1068" },
		{ game::sub_70F2E0, (void *)a_73A720, "100 sub_70F2E0" },
		{ game::sub_715410, (void *)a_73A930, "100 sub_715410" },
		{ game::sub_715E30, (void *)a_73A930, "100 sub_715E30" },
		{ game::sub_715EA0, (void *)a_73A930, "100 sub_715EA0" },
		{ game::sub_715F20, (void *)a_73A930, "100 sub_715F20" },
		{ game::sub_715FC0, (void *)a_73A930, "100 sub_715FC0" },
		{ game::sub_715430, (void *)a_73A950, "100 sub_715430" },
		{ game::sub_7151C0, (void *)a_73AAE0, "100 sub_7151C0" },
		{ game::sub_7149A0, (void *)a_73AB00, "100 sub_7149A0" },
		{ game::sub_7149C0, (void *)a_73AB20, "100 sub_7149C0" },
		{ game::sub_714CB0, (void *)a_73AE10, "100 sub_714CB0" },
		{ game::sub_715000, (void *)a_73B160, "100 sub_715000" },
		{ game::sub_715170, (void *)a_73B2D0, "100 sub_715170" },
		{ game::sub_7151E0, (void *)a_73B350, "100 sub_7151E0" },
		{ game::sub_715300, (void *)a_73B640, "100 sub_715300" },
		{ game::sub_7153D0, (void *)a_73B750, "100 sub_7153D0" },
		{ game::sub_715D20, (void *)a_73B7E0, "100 sub_715D20" },
		{ game::sub_715E80, (void *)a_73B8C0, "100 sub_715E80" },
		{ game::sub_7154B0, (void *)a_73BB60, "100 sub_7154B0" },
		{ game::sub_716000, (void *)a_73BBD0, "100 sub_716000" },
		{ game::sub_7160A0, (void *)a_73BC40, "100 sub_7160A0" },
		{ game::sub_70F550, (void *)a_73C100, "100 sub_70F550" },
		{ game::sub_70FB80, (void *)a_73F990, "100 sub_70FB80" },
		{ game::sub_70FE10, (void *)a_73FC20, "100 sub_70FE10" },
		{ game::sub_710080, (void *)a_73FE90, "100 sub_710080" },
		{ game::sub_7101A0, (void *)a_73FFB0, "100 sub_7101A0" },
		{ game::sub_710400, (void *)a_740210, "100 sub_710400" },
		{ game::sub_711D70, (void *)a_740700, "100 sub_711D70" },
		{ game::sub_711E30, (void *)a_7407C0, "100 sub_711E30" },
		{ game::sub_711EF0, (void *)a_740880, "100 sub_711EF0" },
		{ game::sub_711F80, (void *)a_740910, "100 sub_711F80" },
		{ game::sub_7125E0, (void *)a_740F70, "100 sub_7125E0" },
		{ game::sub_712630, (void *)a_740FC0, "100 sub_712630" },
		{ game::sub_7126C0, (void *)a_741050, "100 sub_7126C0" },
		{ game::sub_712790, (void *)a_741120, "100 sub_712790" },
		{ game::sub_713030, (void *)a_7419C0, "100 sub_713030" },
		{ game::sub_7130B0, (void *)a_741A40, "100 sub_7130B0" },
		{ game::sub_7130E0, (void *)a_741A70, "100 sub_7130E0" },
		{ game::sub_7131D0, (void *)a_741B60, "100 sub_7131D0" },
		{ game::MAG_100_RandRange, (void *)a_742290, "100 au_re__rand_3" },
		{ game::sub_713930, (void *)a_7422C0, "100 sub_713930" },
		{ game::sub_713970, (void *)a_742300, "100 sub_713970" },
		{ game::sub_7139C0, (void *)a_742350, "100 sub_7139C0" },
		{ game::MAG_100_RandAngleJitter, (void *)a_7423A0, "100 au_re__rand_3_0" },
		{ game::sub_713B20, (void *)a_7424B0, "100 sub_713B20" },
		{ game::sub_713B30, (void *)a_7424C0, "100 sub_713B30" },
		{ game::sub_714310, (void *)a_742CA0, "100 sub_714310" },
		{ game::sub_714340, (void *)a_742CD0, "100 sub_714340" },
		{ game::sub_714370, (void *)a_742D00, "100 sub_714370" },
		{ game::sub_714520, (void *)a_742EB0, "100 sub_714520" },
		{ game::sub_714660, (void *)a_742FF0, "100 sub_714660" },
		{ game::sub_714770, (void *)a_743100, "100 sub_714770" },
		{ game::sub_714800, (void *)a_743190, "100 sub_714800" },
		{ game::sub_715CA0, (void *)a_743C00, "100 sub_715CA0" },
		{ game::sub_715C40, (void *)a_743C20, "100 sub_715C40" },
		{ game::sub_715600, (void *)a_746C10, "100 sub_715600" },
		{ game::MAG_100_sub_717B80, (void *)a_747440, "100 MAG_100_sub_717B80" },
		{ game::MAG_100_sub_717BF0, (void *)a_7474B0, "100 MAG_100_sub_717BF0" },
		{ game::MAG_100_sub_717C10, (void *)a_7474D0, "100 MAG_100_sub_717C10" },
		{ game::MAG_100_sub_717C40, (void *)a_747500, "100 MAG_100_sub_717C40" },
		{ game::MAG_100_sub_717C90, (void *)a_747550, "100 MAG_100_sub_717C90" },
		{ game::MAG_100_sub_717CA0, (void *)a_747560, "100 MAG_100_sub_717CA0" },
		{ game::MAG_100_sub_717CD0, (void *)a_747590, "100 MAG_100_sub_717CD0" },
		{ game::MAG_100_sub_717CE0, (void *)a_7475A0, "100 MAG_100_sub_717CE0" },
		{ {}, nullptr, nullptr }
	};

	const Mod *g_mod = &MOD_095;

	static const Mod *const MODS[] = { &MOD_090, &MOD_095, &MOD_096, &MOD_097, &MOD_098, &MOD_099, &MOD_100 };

	const Mod *find_mod(int effect_id)
	{
		for (const Mod *m : MODS)
			if (m->effect_id == effect_id) return m;
		return nullptr;
	}

	void set_mod_by_code(uint32_t code_addr)
	{
		for (const Mod *m : MODS)
			if (code_addr >= game_addr(m->lo) && code_addr < game_addr(m->hi)) { g_mod = m; return; }
	}

	static const int DISPATCH_BITS = 14;
	static const int DISPATCH_SIZE = 1 << DISPATCH_BITS;
	static uint32_t g_disp_key[DISPATCH_SIZE];
	static void *g_disp_val[DISPATCH_SIZE];
	static int g_disp_count;
	static inline uint32_t disp_slot(uint32_t a) { return (a * 2654435761u) >> (32 - DISPATCH_BITS); }

	void *port_of(uint32_t orig)
	{
		for (uint32_t h = disp_slot(orig);; h++)
		{
			uint32_t k = g_disp_key[h & (DISPATCH_SIZE - 1)];
			if (k == orig) return g_disp_val[h & (DISPATCH_SIZE - 1)];
			if (k == 0) return nullptr;
		}
	}

	void add_port(uint32_t orig, void *port)
	{
		for (uint32_t h = disp_slot(orig);; h++)
		{
			uint32_t &k = g_disp_key[h & (DISPATCH_SIZE - 1)];
			if (k == 0 || k == orig)
			{
				if (k == 0 && ++g_disp_count >= DISPATCH_SIZE - 1)
				{
					ffnx_error("%s: act dispatch table full (%d), raise DISPATCH_BITS\n", __func__, g_disp_count);
					return;
				}
				k = orig;
				g_disp_val[h & (DISPATCH_SIZE - 1)] = port;
				return;
			}
		}
	}

	void remove_port(uint32_t orig)
	{
		for (uint32_t h = disp_slot(orig);; h++)
		{
			uint32_t k = g_disp_key[h & (DISPATCH_SIZE - 1)];
			if (k == orig) { g_disp_val[h & (DISPATCH_SIZE - 1)] = nullptr; return; }
			if (k == 0) return;
		}
	}

	static const AddrPort *engine_ports(int effect_id)
	{
		switch (effect_id)
		{
		case 90: return ENGINE_PORTS_090;
		case 95: return ENGINE_PORTS_095;
		case 96: return ENGINE_PORTS_096;
		case 97: return ENGINE_PORTS_097;
		case 98: return ENGINE_PORTS_098;
		case 99: return ENGINE_PORTS_099;
		case 100: return ENGINE_PORTS_100;
		}
		return nullptr;
	}

	void register_module_port(int effect_id, game::Id orig, void *port, const char *name)
	{
		add_port(game_addr(orig), port);
		ff8fx::register_port(orig, port, name, effect_id);
	}

	void register_module(int effect_id)
	{
		static bool global_done = false;
		if (!global_done)
		{
			global_done = true;
			add_port(game_addr(game::Effect_ScreenTintTask), (void *)a_8DDC30);
			add_port(game_addr(game::sub_8DDCA0), (void *)a_8DDCA0);
			add_port(game_addr(game::sub_8DDCD0), (void *)a_8DDCD0);
		}
		ff8fx::register_port(game::Effect_ScreenTintTask, (void *)a_8DDC30, "act 8DDC30 shared engine task", effect_id);
		for (const AddrPort *p = engine_ports(effect_id); p && p->addr; p++)
			register_module_port(effect_id, p->addr, p->port, p->name);
	}

	uint32_t orig_of_port(void *port)
	{
		for (const AddrPort *p = engine_ports(g_mod->effect_id); p && p->addr; p++)
			if (p->port == port) return game_addr(p->addr);
		return 0;
	}

	uint32_t prim_play(uint32_t layout, uint32_t cb, uint32_t arg, uint32_t paused)
	{
		void *p = port_of(cb);
		uint32_t f = p ? P(p) : cb;
		int r = ff8fx::prim::play((ff8fx::prim::Layout *)layout, (ff8fx::prim::Callback)f, (int)arg, (int)paused);
		return (uint32_t)r;
	}

	namespace
	{
		inline bool e1_out(int16_t v, int16_t hi) { return v < 0 || v > hi; }

		void e1_shift_v_a(uint32_t it, uint32_t sh)
		{
			uint32_t w8 = U32(it, 8);
			uint32_t wc = U32(it, 0xC);
			uint32_t w10 = U32(it, 0x10);
			uint32_t keep8 = w8 & 0xFFFFFF;
			uint32_t d = ((w8 >> 24) & 0xFF) << 8;
			uint32_t s = wc & 0xFF00;
			uint32_t e = w10 & 0xFF00;
			uint32_t bp = wc;
			uint32_t bx = w10;
			s += sh;
			bp &= 0xFFFF00FF;
			bx &= 0xFFFF00FF;
			e += sh;
			d += sh;
			if (s > 0xFF00 || e > 0xFF00 || d > 0xFF00)
			{
				s -= 0x8000;
				e -= 0x8000;
				d -= 0x8000;
			}
			s &= 0xFF00;
			d &= 0xFFFFFF00;
			bp |= s;
			e &= 0xFF00;
			U32(it, 0xC) = bp;
			d <<= 16;
			bx |= e;
			keep8 |= d;
			U32(it, 0x10) = bx;
			U32(it, 8) = keep8;
		}

		void e1_shift_v_b(uint32_t it, uint32_t sh, bool list4)
		{
			uint32_t wc = U32(it, 0xC);
			uint32_t w10 = U32(it, 0x10);
			uint32_t s = wc & 0xFF00;
			uint32_t keepc = wc & 0xFFFF00FF;
			s += sh;
			uint32_t keep10 = w10 & 0xFFFF00FF;
			uint32_t bp = w10 & 0xFF00;
			uint32_t w14 = U32(it, 0x14);
			uint32_t di = w14 & 0xFF00FF;
			uint32_t bx = w14 & 0xFF00;
			uint32_t d = ((w14 >> 24) & 0xFF) << 8;
			bp += sh;
			bx += sh;
			d += sh;
			if (s > 0xFF00 || bp > 0xFF00 || bx > 0xFF00 || d > 0xFF00)
			{
				s -= 0x8000;
				bp -= 0x8000;
				bx -= 0x8000;
				d -= 0x8000;
			}
			d &= 0xFFFFFF00;
			s &= 0xFF00;
			d <<= 16;
			bx &= 0xFF00;
			bp &= 0xFF00;
			uint32_t outc = keepc | s;
			uint32_t out10 = keep10 | bp;
			uint32_t out14 = di | bx | d;
			if (list4)
			{
				U32(it, 0x10) = out10;
				U32(it, 0xC) = outc;
				U32(it, 0x14) = out14;
			}
			else
			{
				U32(it, 0xC) = outc;
				U32(it, 0x10) = out10;
				U32(it, 0x14) = out14;
			}
		}
	}

	uint32_t __cdecl a_732120(uint32_t a1)
	{
		if (U8(a1, 0x28) == 0)
		{
			uint32_t cur = U32(G(G_257F728), 0);
			U32(G(G_258EC54), 0) = cur;
			cur += 0x10E0;
			U32(G(G_257F720), 0) = cur;
			cur += 0x10680;
			U32(G(G_257F728), 0) = cur;
			a_732160();
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl a_732160(void)
	{
		x::MAG_007_sub_8DCC00(U32(G(G_258EC54), 0), 0x10E0);
		x::MAG_007_sub_8DCC00(U32(G(G_257F720), 0), 0x10680);
		U16(G(G_2585AE0), 0) = 0;
		U16(G(G_257F698), 0) = 0;
		U16(G(G_257F4D4), 0) = 0;
		U16(G(G_257B748), 0) = 0;
		return 0;
	}

	uint32_t __cdecl a_7322A0(void)
	{
		U32(G(G_258BFA8), 0) = G(G_152BBA0);
		U32(G(G_258EAA0), 0) = G(G_257F730);
		x::MAG_007_sub_8DCC00(G(G_257F730), 0x16C);
		uint32_t st = U32(G(G_258EAA0), 0);
		uint32_t cur = U32(G(G_257F728), 0);
		U32(st, 0x100) = cur;
		cur += 0xA000;
		U32(G(G_257F728), 0) = cur;
		const uint32_t def = game_addr(game::unk_19DB018);
		U32(st, 0x114) = def;
		U32(st, 0x118) = def;
		U32(st, 0x11C) = def;
		U32(st, 0x120) = def;
		U32(st, 0x104) = def;
		U32(st, 0x108) = def;
		U32(st, 0x10C) = def;
		U32(st, 0x110) = def;
		U32(st, 0x124) = def;
		U32(st, 0x128) = def;
		U32(st, 0x12C) = def;
		U32(st, 0x130) = def;
		U32(st, 0x134) = def;
		U32(st, 0x138) = def;
		U32(st, 0x13C) = def;
		U32(st, 0x140) = def;
		U32(st, 0x144) = def;
		U32(st, 0x148) = def;
		U32(st, 0x14C) = def;
		U32(st, 0x150) = def;
		U32(st, 0x154) = def;
		U32(st, 0x158) = def;
		U32(st, 0x15C) = def;
		U32(st, 0x160) = def;
		U16(st, 0x164) = 0;
		U16(st, 0x166) = 0;
		return 0;
	}

	uint32_t __cdecl a_732A00(uint32_t a1)
	{
		uint32_t tab[16];
		tab[0] = F(F_732AC0);
		tab[1] = F(F_7332C0);
		tab[2] = F(F_733310);
		tab[3] = F(F_733340);
		tab[4] = F(F_733370);
		tab[5] = F(F_7333A0);
		tab[6] = F(F_7333D0);
		tab[7] = F(F_733400);
		tab[8] = F(F_733430);
		tab[9] = F(F_733460);
		tab[10] = F(F_733490);
		tab[11] = F(F_7334C0);
		tab[12] = F(F_7334F0);
		tab[13] = F(F_733510);
		tab[14] = F(F_733530);
		tab[15] = F(F_733560);
		callp(tab[(int32_t)S8(a1, 0x29)], a1);
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		if ((status & 1) && U8(a1, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(a1);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl a_7334C0(uint32_t a1)
	{
		if (a_73AE10() == 1)
		{
			uint32_t data = U32(G(G_1533010), 0);
			a_73AB20(G(G_1532F80), data + 8, 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl a_7335D0(uint32_t a1)
	{
		U16(a1, 0x1C) = 0;
		uint32_t p = game_addr(game::unk_1D989BA);
		for (int n = 4; n != 0; n--)
		{
			U16(p, -0x28) = 0;
			U8(p, 0) = 0;
			U8(p, -1) = 0;
			U8(p, -2) = 0;
			p += 0x2C;
		}
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl a_733760(uint32_t a1)
	{
		if (a_73B7E0(4) != 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl a_733950(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint8_t c[4];
		uint32_t base = U32(game_addr(game::CURRENT_BS_MODEL_PALETTE), 0);
		ff8fx::copy<(4)>(c, &base);
		int16_t amount = (int16_t)a2;
		if (amount == 0)
		{
			U32(a1, 0x28) = base;
			U8(a1, 0x2B) = 0;
			U8(a1, 7) = 0;
			return 0;
		}
		int32_t mode = (int16_t)a3;
		if (mode == 1)
		{
			c[0] = (uint8_t)(c[0] >> 1);
			c[1] = (uint8_t)(c[1] >> 1);
			c[2] = (uint8_t)(c[2] >> 1);
			U8(a1, 0x2B) = 2;
		}
		else if (mode == 2)
		{
			U8(a1, 0x2B) = 2;
		}
		else if (mode == 3)
		{
			int32_t v = (int32_t)amount;
			U8(a1, 0x2B) = 2;
			int32_t q = sub32(shl32(v, 8), v) / 4096;
			U8(a1, 7) = (uint8_t)((uint8_t)(0xFF - (uint8_t)q) >> 2);
		}
		int32_t k = (int32_t)amount;
		for (int i = 0; i < 3; i++)
		{
			int32_t d = mul32((int32_t)c[i], k) / 4096;
			U8(a1, 0x28 + i) = (uint8_t)(c[i] - (uint8_t)d);
		}
		return 0;
	}

	uint32_t __cdecl a_733AC0(uint32_t a1)
	{
		uint32_t st = U32(G(G_258EAA0), 0);
		if (U16(st, 0x166) == 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl a_733AE0(uint32_t a1)
	{
		int32_t start = S16(a1, 0x44);
		int32_t now = S16(a1, 0x24);
		if (now - start >= 0x14)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl a_733B70(uint32_t a1)
	{
		if (a_73B640(3) != 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl a_7340A0(uint32_t a1, uint32_t a2)
	{
		int32_t idx = S32(a1, 0) / 4;
		uint32_t p = a1 + (uint32_t)idx * 4;
		uint32_t n = U32(p, 0);
		uint32_t sh = a2 << 8;
		p = p + n * 12 + 4;
		n = U32(p, 0);
		p = p + n * 12 + 4;
		int32_t cnt = S32(p, 0);
		p += 4;
		if (cnt > 0)
		{
			do
			{
				e1_shift_v_a(p, sh);
				p += 0x14;
			} while (--cnt != 0);
		}
		cnt = S32(p, 0);
		p += 4;
		if (cnt > 0)
		{
			do
			{
				e1_shift_v_b(p, sh, false);
				p += 0x18;
			} while (--cnt != 0);
		}
		n = U32(p, 0);
		p = p + n * 20 + 4;
		n = U32(p, 0);
		p = p + n * 24 + 4;
		cnt = S32(p, 0);
		p += 4;
		if (cnt > 0)
		{
			do
			{
				e1_shift_v_a(p, sh);
				p += 0x1C;
			} while (--cnt != 0);
		}
		cnt = S32(p, 0);
		p += 4;
		if (cnt > 0)
		{
			do
			{
				e1_shift_v_b(p, sh, true);
				p += 0x24;
			} while (--cnt != 0);
		}
		return 0;
	}

	uint32_t __cdecl a_7348A0(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			a_73F990(a1);
			a_7353B0(a1);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl a_7353B0(uint32_t a1)
	{
		uint32_t done = 0;
		uint32_t sys = a1 + 0x34;
		U32(G(G_258FB78), 0) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			a_743190();
			a_736C00();
			a_740700();
			a_735440();
			sys = U32(G(G_258FB78), 0);
			U16(sys, 0x18) = (uint16_t)(U16(sys, 0x18) + 1);
			if (S16(sys, 0x18) >= S16(sys, 0x1A) && U16(sys, 0x14) == 0 && U16(sys, 0x16) == 0)
				U16(sys, 0x20) = (uint16_t)(U16(sys, 0x20) + 1);
			break;
		case 2:
			done = 1;
			break;
		default:
			break;
		}
		uint16_t c14 = U16(sys, 0x14);
		uint16_t c16 = U16(sys, 0x16);
		U16(G(G_258FB40), 0) = (uint16_t)(U16(G(G_258FB40), 0) + c14);
		U16(G(G_258EC50), 0) = (uint16_t)(U16(G(G_258EC50), 0) + c16);
		return done;
	}

	uint32_t __cdecl a_735440(void)
	{
		uint32_t act = U32(U32(G(G_258FB78), 0), 0x2C);
		while (act != 0)
		{
			if (U16(act, 8) == 1)
			{
				uint32_t sys = U32(G(G_258FB78), 0);
				int32_t k = S8(act, 0x1D6);
				uint32_t def = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(def, 0x16);
				if ((kind == 4 || kind == 5) && U32(act, 0x170) != 0)
					a_7354A0(act, def);
			}
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl a_7354A0(uint32_t a1, uint32_t a2)
	{
		uint32_t ctx = U32(G(G_258FB68), 0) - 0x58;
		bool def3a = U16(a2, 0x3A) != 0;
		uint32_t model = U32(a1, 0x170);
		U32(G(G_258FB68), 0) = ctx;
		uint32_t saved_ctx = ctx;
		U32(ctx, 0) = model;
		U32(ctx, 0x1C) = 0;
		if (!def3a)
			U32(ctx, 0x1C) = 0x30;
		uint16_t col = U16(a1, 0x1CE);
		if (col != 0)
		{
			U32(ctx, 8) = U32(a1, 0x16C);
			uint32_t fl = U32(ctx, 0x1C);
			U32(ctx, 0xC) = (uint32_t)(int32_t)(int16_t)col;
			U32(ctx, 0x1C) = fl | 0xC0;
		}
		int8_t copies = S8(a1, 0x1D8);
		if (copies == 1)
		{
			uint32_t m = a1 + 0xAC;
			x::GTE_SetRotMatrix(m);
			x::GTE_SetTransVector(m);
			uint32_t cursor = U32(game_addr(game::BATTLE_PACKET_WRITE_CURSOR), 0);
			uint32_t ot = U32(game_addr(game::g_Battle_FrameRenderListBase), 0) + 0x44;
			uint32_t r = a_7355E0(ctx, ot, 2, cursor);
			U32(game_addr(game::BATTLE_PACKET_WRITE_CURSOR), 0) = r;
		}
		else if (copies > 0)
		{
			uint32_t m = a1 + 0xAC;
			uint32_t pos = a1 + 0x19E;
			int32_t i = 0;
			int32_t n;
			do
			{
				S32(a1, 0xC0) = S16(pos, -2);
				S32(a1, 0xC4) = S16(pos, 0);
				S32(a1, 0xC8) = S16(pos, 2);
				x::GTE_SetRotMatrix(m);
				x::GTE_SetTransVector(m);
				uint32_t cursor = U32(game_addr(game::BATTLE_PACKET_WRITE_CURSOR), 0);
				uint32_t ot = U32(game_addr(game::g_Battle_FrameRenderListBase), 0) + 0x44;
				uint32_t r = a_7355E0(saved_ctx, ot, 2, cursor);
				n = S8(a1, 0x1D8);
				i++;
				pos += 8;
				U32(game_addr(game::BATTLE_PACKET_WRITE_CURSOR), 0) = r;
			} while (i < n);
		}
		uint32_t top = U32(G(G_258FB68), 0) + 0x58;
		U32(G(G_258FB68), 0) = top;
		return top;
	}

	uint32_t __cdecl a_7355E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
			cur = callp(F(F_735720), a1, a2, a3, cur);
		else
			U32(a1, 0x20) = U32(a1, 0x20) + 4;
		if (U32(U32(a1, 0x20), 0) != 0)
			cur = callp(F(F_735930), a1, a2, a3, cur);
		else
			U32(a1, 0x20) = U32(a1, 0x20) + 4;
		if (U32(U32(a1, 0x20), 0) != 0)
			cur = a_735BB0(a1, a2, a3, cur);
		else
			U32(a1, 0x20) = U32(a1, 0x20) + 4;
		if (U32(U32(a1, 0x20), 0) != 0)
			cur = a_735DF0(a1, a2, a3, cur);
		else
			U32(a1, 0x20) = U32(a1, 0x20) + 4;
		if (U32(U32(a1, 0x20), 0) != 0)
			cur = callp(F(F_736090), a1, a2, a3, cur);
		else
			U32(a1, 0x20) = U32(a1, 0x20) + 4;
		if (U32(U32(a1, 0x20), 0) != 0)
			cur = callp(F(F_7362C0), a1, a2, a3, cur);
		else
			U32(a1, 0x20) = U32(a1, 0x20) + 4;
		if (U32(U32(a1, 0x20), 0) != 0)
			cur = callp(F(F_736580), a1, a2, a3, cur);
		else
			U32(a1, 0x20) = U32(a1, 0x20) + 4;
		if (U32(U32(a1, 0x20), 0) != 0)
			return callp(F(F_7367E0), a1, a2, a3, cur);
		U32(a1, 0x20) = U32(a1, 0x20) + 4;
		return cur;
	}

	uint32_t __cdecl a_735BB0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t list = U32(ctx, 0x20);
		int32_t count = S32(list, 0);
		uint32_t rec = list + 4;
		uint32_t vtx = U32(ctx, 4);
		U32(ctx, 0x20) = rec;
		if (count > 0)
		{
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
						uint32_t clip = e1_out(S16(pkt, 8), 0xA00) ? 1 : zero;
						if (e1_out(S16(pkt, 0x10), 0xA00)) clip |= 2;
						if (e1_out(S16(pkt, 0x18), 0xA00)) clip |= 4;
						if (e1_out(S16(pkt, 0xA), 0x6C0)) clip |= 0x10;
						if (e1_out(S16(pkt, 0x12), 0x6C0)) clip |= 0x20;
						if (e1_out(S16(pkt, 0x1A), 0x6C0)) clip |= 0x40;
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
							int32_t z = S32(ctx, 0x2C) >> (a3 & 31);
							x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)z * 4, pkt);
							pkt += 0x20;
						}
					}
				}
				rec += 0x14;
			} while (--n != 0);
		}
		U32(ctx, 0x20) = rec;
		return pkt;
	}

	uint32_t __cdecl a_735DF0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t list = U32(ctx, 0x20);
		int32_t count = S32(list, 0);
		uint32_t rec = list + 4;
		uint32_t vtx = U32(ctx, 4);
		U32(ctx, 0x20) = rec;
		if (count > 0)
		{
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
						uint32_t clip = e1_out(S16(pkt, 8), 0xA00) ? 1 : zero;
						if (e1_out(S16(pkt, 0x10), 0xA00)) clip |= 2;
						if (e1_out(S16(pkt, 0x18), 0xA00)) clip |= 4;
						if (e1_out(S16(pkt, 0xA), 0x6C0)) clip |= 0x10;
						if (e1_out(S16(pkt, 0x12), 0x6C0)) clip |= 0x20;
						if (e1_out(S16(pkt, 0x1A), 0x6C0)) clip |= 0x40;
						x::GTE_ReadSXY2(pkt + 0x20);
						x::GTE_AVSZ4();
						if (e1_out(S16(pkt, 0x20), 0xA00)) clip |= 8;
						if (e1_out(S16(pkt, 0x22), 0x6C0)) clip |= 0x80;
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
							int32_t z = S32(ctx, 0x2C) >> (a3 & 31);
							x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)z * 4, pkt);
							pkt += 0x28;
						}
					}
				}
				rec += 0x18;
			} while (--n != 0);
		}
		U32(ctx, 0x20) = rec;
		return pkt;
	}

	uint32_t __cdecl a_736C00(void)
	{
		uint32_t act = U32(U32(G(G_258FB78), 0), 0x2C);
		while (act != 0)
		{
			int32_t type = S16(act, 8);
			if (type == 0)
				a_737ED0(act);
			else if (type == 1)
				a_740880(act);
			act = U32(act, 4);
		}
		return 0;
	}

	uint32_t __cdecl a_737ED0(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62) = (uint16_t)(U16(a1, 0x62) + 1);
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			callp(F(F_7424C0), a1);
		else if (mode == 4)
			callp(F(F_742610), a1);
		a_737F50(a1);
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) + 1);
		a_741B60(a1);
		return 0;
	}

	uint32_t __cdecl a_737F50(uint32_t a1)
	{
		uint32_t sys = U32(G(G_258FB78), 0);
		int32_t k = S8(a1, 0x6A);
		uint32_t def = U32(U32(sys, 0x224), k * 4);
		int16_t frame = S16(a1, 0x60);
		if (frame > 0x27)
			return 0;
		uint32_t total = U8(U32(def, 0xC8), (int32_t)frame);
		if (total == 0)
			return 0;
		int32_t batch = S8(def, 0x36);
		int32_t full = (int32_t)total / batch;
		int32_t rest = (int32_t)total % batch;
		if (full > 0)
		{
			do
			{
				int32_t b = S8(def, 0x36);
				a_737FD0(a1, def, (uint32_t)b);
			} while (--full != 0);
		}
		if (rest > 0)
			a_737FD0(a1, def, (uint32_t)rest);
		return 0;
	}

	uint32_t __cdecl a_7387B0(uint32_t a1, uint32_t a2)
	{
		uint32_t pool = U32(G(G_257F720), 0);
		int32_t i = S16(G(G_257F698), 0);
		uint32_t slot = 0;
		int32_t tries = 0;
		for (;;)
		{
			uint32_t off = (uint32_t)mul32(i, 0x2A0);
			if (U8(off + pool, 0x1D7) == 0)
			{
				slot = (uint32_t)mul32(i, 0x2A0) + pool;
				x::MAG_007_sub_8DCC00(slot, 0x2A0);
				uint8_t def_idx = (uint8_t)a2;
				U32(slot, 0x1BC) = a1;
				uint8_t b6b = U8(a1, 0x6B);
				uint32_t sys = U32(G(G_258FB78), 0);
				U8(slot, 0x1D7) = 1;
				U16(sys, 0x16) = (uint16_t)(U16(sys, 0x16) + 1);
				U8(slot, 0x1D6) = def_idx;
				U8(slot, 0x1D9) = b6b;
				a_742CA0(slot, 1);
				break;
			}
			i++;
			if (i >= 0x63)
				i = 0;
			tries++;
			if (tries >= 0x64)
				break;
		}
		i++;
		if (i >= 0x63)
			U16(G(G_257F698), 0) = 0;
		else
			U16(G(G_257F698), 0) = (uint16_t)i;
		return slot;
	}

	uint32_t __cdecl a_7395E0(uint32_t a1)
	{
		if (a_7353B0(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl a_739890(uint32_t a1)
	{
		uint32_t scr = U32(a1, 0x110);
		if (scr == 0)
			return 0;
		uint32_t idx;
		uint16_t wait = U16(a1, 0x132);
		if (wait == 0)
		{
			idx = U16(a1, 0x12C);
		}
		else
		{
			uint16_t w = (uint16_t)(wait - 1);
			U16(a1, 0x132) = w;
			if ((int16_t)w > 0)
				return 0;
			U16(a1, 0x12C) = (uint16_t)(U16(a1, 0x12C) + 1);
			idx = U16(a1, 0x12C);
		}
		uint32_t ent = scr + idx * 4;
		uint16_t frame = U16(ent, 0);
		U16(a1, 0x130) = frame;
		U16(a1, 0x132) = U16(ent, 2);
		if (frame == 0xFF)
		{
			U32(a1, 0x110) = 0;
			return 0;
		}
		if (frame == 0xFE)
		{
			uint16_t f0 = U16(scr, 0);
			uint16_t d0 = U16(scr, 2);
			U16(a1, 0x12C) = 0;
			U16(a1, 0x130) = f0;
			U16(a1, 0x132) = d0;
		}
		uint32_t arg_frame = (ent & 0xFFFF0000) | U16(a1, 0x130);
		uint32_t arg_page = (scr & 0xFFFF0000) | U16(a1, 0x12E);
		a_739960(a1 + 0x30, arg_page, arg_frame);
		return 0;
	}

	uint32_t __cdecl a_739960(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t hdr = U32(a1, 0x84);
		uint32_t tab = U32(hdr, 0x30);
		if (tab == 0)
			return 0;
		uint8_t off = U8(tab + (a2 & 0xFFFF), 0);
		if (off == 0)
			return 0;
		uint32_t rec = (uint32_t)off + tab;
		uint32_t cell = rec + (a3 & 0xFFFF) * 2 + 5;
		int32_t page = U8(rec, 0);
		uint32_t mask = U16(hdr, 2);
		for (int32_t bit = 0; bit < 12; bit++)
		{
			if (mask & (1u << bit))
			{
				int32_t t = page;
				page--;
				if (t == 0)
				{
					page = bit;
					break;
				}
			}
		}
		int32_t parity = page & 1;
		int32_t half = page / 2;
		int32_t xbase = shl32(13 - half, 6);
		int32_t ybit = shl32(parity, 7);
		uint16_t rect[4];
		rect[0] = (uint16_t)((uint32_t)U8(cell, 0) + (uint32_t)xbase);
		rect[1] = (uint16_t)((uint32_t)U8(cell, 1) + (uint32_t)ybit + 0x100);
		rect[2] = U8(rec, 3);
		rect[3] = U8(rec, 4);
		uint32_t hi = (uint32_t)half & 0xFFFF0000;
		uint32_t arg_y = (hi | U8(rec, 2)) + (uint32_t)ybit + 0x100;
		uint32_t arg_x = (hi | U8(rec, 1)) + (uint32_t)xbase;
		x::QueueBlitCommand(P(rect), arg_x, arg_y);
		return 0;
	}

	uint32_t __cdecl a_739AC0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		U32(a1, 0x110) = a2;
		U16(a1, 0x12E) = (uint16_t)a3;
		U16(a1, 0x12C) = 0;
		U16(a1, 0x130) = 0;
		U16(a1, 0x132) = 0;
		return 0;
	}

	uint32_t __cdecl a_739B40(uint32_t a1)
	{
		uint32_t r = x::au_re_Battle_ReadAnimation_8(a1);
		if (r == 1)
		{
			x::au_re_Battle_ReadAnimation_7(a1, r);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl a_8DDC30(uint32_t a1)
	{
		uint32_t tab[3];
		tab[0] = game_addr(game::sub_8DDCA0);
		tab[1] = game_addr(game::sub_8DDCD0);
		tab[2] = game_addr(game::nullsub_1118);
		callp(tab[(int32_t)S8(a1, 0x29)], a1);
		if (U32(game_addr(game::unk_2795914), 0) == 0)
			U16(a1, 0x3C) = (uint16_t)(U16(a1, 0x3C) + 1);
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		if ((status & 1) && U8(a1, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(a1);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl a_8DDCA0(uint32_t a1)
	{
		if (U32(game_addr(game::unk_2795914), 0) == 0)
		{
			U16(a1, 0x38) = (uint16_t)(U16(a1, 0x38) - 1);
			if (S16(a1, 0x38) <= 0)
			{
				uint8_t st = U8(a1, 0x29);
				U16(a1, 0x38) = 0;
				U8(a1, 0x29) = (uint8_t)(st + 1);
			}
		}
		return 0;
	}

	uint32_t __cdecl a_8DDCD0(uint32_t a1)
	{
		uint16_t idx = U16(a1, 0x3A);
		uint32_t script = U32(a1, 0x34);
		uint32_t ent = U32(script, (int32_t)(int16_t)idx * 4);
		uint8_t op = (uint8_t)(ent >> 24);
		uint8_t b2 = (uint8_t)(ent >> 16);
		if (op == 0xFF)
		{
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			return 0;
		}
		if (op == 0xFE)
		{
			if (S16(a1, 0x24) >= (int16_t)(uint16_t)b2 && U32(game_addr(game::unk_2795914), 0) == 0)
			{
				uint16_t ni = (uint16_t)(idx + 1);
				U16(a1, 0x3A) = ni;
				U32(a1, 0x30) = U32(script, (int32_t)(int16_t)ni * 4);
			}
		}
		else
		{
			U32(a1, 0x30) = ent;
			if (U32(game_addr(game::unk_2795914), 0) == 0)
				U16(a1, 0x3A) = (uint16_t)(idx + 1);
		}
		uint32_t ot = U32(game_addr(game::g_Battle_FrameRenderListBase), 0) + 0x1C;
		const uint8_t code = 0x2A;
		const uint32_t v_mid = 0x7800A0;

		U32(game_addr(game::unk_2795114), 0) = U32(a1, 0x30);
		U32(game_addr(game::unk_2795110), 0) = 0x5000000;
		U8(game_addr(game::unk_2795117), 0) = code;
		U32(game_addr(game::unk_2795118), 0) = 0;
		U32(game_addr(game::unk_279511C), 0) = 0xA0;
		U32(game_addr(game::unk_2795120), 0) = 0x780000;
		U32(game_addr(game::unk_2795124), 0) = v_mid;
		x::SSIGPU_InsertPrimAltViewport(ot, game_addr(game::unk_2795110));

		U32(game_addr(game::unk_279512C), 0) = U32(a1, 0x30);
		U32(game_addr(game::unk_2795128), 0) = 0x5000000;
		U8(game_addr(game::unk_279512F), 0) = code;
		U32(game_addr(game::unk_2795130), 0) = 0xA0;
		U32(game_addr(game::unk_2795134), 0) = 0x140;
		U32(game_addr(game::unk_2795138), 0) = v_mid;
		U32(game_addr(game::unk_279513C), 0) = 0x780140;
		x::SSIGPU_InsertPrimAltViewport(ot, game_addr(game::unk_2795128));

		U32(game_addr(game::unk_2795144), 0) = U32(a1, 0x30);
		U32(game_addr(game::unk_2795140), 0) = 0x5000000;
		U8(game_addr(game::unk_2795147), 0) = code;
		U32(game_addr(game::unk_2795148), 0) = 0x780000;
		U32(game_addr(game::unk_279514C), 0) = v_mid;
		U32(game_addr(game::unk_2795150), 0) = 0xF00000;
		U32(game_addr(game::unk_2795154), 0) = 0xF000A0;
		x::SSIGPU_InsertPrimAltViewport(ot, game_addr(game::unk_2795140));

		U32(game_addr(game::unk_279515C), 0) = U32(a1, 0x30);
		U32(game_addr(game::unk_2795158), 0) = 0x5000000;
		U8(game_addr(game::unk_279515F), 0) = code;
		U32(game_addr(game::unk_2795160), 0) = v_mid;
		U32(game_addr(game::unk_2795164), 0) = 0x780140;
		U32(game_addr(game::unk_2795168), 0) = 0xF000A0;
		U32(game_addr(game::unk_279516C), 0) = 0xF00140;
		x::SSIGPU_InsertPrimAltViewport(ot, game_addr(game::unk_2795158));

		uint32_t tpage = x::sub_45C690(0, 1, 0x280, 0) & 0xFFFF;
		x::sub_45BFC0(game_addr(game::unk_2795170), 0, 0, tpage, 0);
		x::SSIGPU_InsertPrimAltViewport(ot, game_addr(game::unk_2795170));
		return 0;
	}

	namespace
	{
		inline uint32_t e2_cam() { return MEM<uint32_t>(G(G_15339D0)); }
		inline uint32_t e2_dir() { return MEM<uint32_t>(G(G_1533010)); }
		inline uint32_t e2_ctx() { return MEM<uint32_t>(G(G_258EAA0)); }
		inline uint32_t e2_rec() { return MEM<uint32_t>(G(G_258BFA8)); }

		inline int32_t e2_mulq12(int32_t to, int32_t from, int32_t t)
		{
			return mul32(sub32(to, from), t) / 4096;
		}

		inline uint16_t e2_angle_lerp(uint32_t c, int32_t from_off, int32_t to_off, bool wrap_from)
		{
			int16_t si = S16(c, from_off);
			int16_t ax = S16(c, to_off);
			if (!wrap_from)
			{
				if (si > ax) ax = (int16_t)(uint16_t)((uint16_t)ax + 0x1000);
			}
			else
			{
				if (si < ax) si = (int16_t)(uint16_t)((uint16_t)si + 0x1000);
			}
			int32_t r = e2_mulq12((int32_t)ax, (int32_t)si, (int32_t)S16(c, 0xD0));
			return (uint16_t)(add32(r, (int32_t)si) & 0xFFF);
		}

		inline uint16_t e2_lerp(uint32_t c, int32_t from_off, int32_t to_off)
		{
			int16_t si = S16(c, from_off);
			int32_t r = e2_mulq12((int32_t)S16(c, to_off), (int32_t)si, (int32_t)S16(c, 0xD0));
			return (uint16_t)add32(r, (int32_t)si);
		}
	}

	uint32_t __cdecl a_739F40(uint32_t a1)
	{
		set_mod_by_code(U32(a1, 8));

		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		MEM<uint32_t>(G(G_258FB68)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(G(G_257F9B0)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		uint32_t states[11] = {
			F(F_73A0D0), F(F_73A0E0), F(F_73A0F0), F(F_73A170), F(F_747500), F(F_747540),
			F(F_747550), F(F_747560), F(F_747590), F(F_7475A0), F(F_7475C0),
		};
		if (U8(a1, 0x5C) & 1)
		{
			uint32_t v1 = MEM<uint32_t>(G(G_258BFA4));
			uint32_t v2 = MEM<uint32_t>(G(G_258A024));
			MEM<uint32_t>(G(G_257F8A4)) = v1;
			MEM<uint32_t>(G(G_257F9AC)) = v2;
		}
		else
		{
			uint32_t v1 = MEM<uint32_t>(G(G_258BFA0));
			uint32_t v2 = MEM<uint32_t>(G(G_258A020));
			MEM<uint32_t>(G(G_257F8A4)) = v1;
			MEM<uint32_t>(G(G_257F9AC)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(a1);
		callp(states[S8(a1, 0x29)], a1);

		U16(a1, 0x5E) = 0;
		U16(G(G_258FB40), 0) = 0;
		U16(G(G_258EC50), 0) = 0;
		static const int queues[9] = { G_258BE30, G_257F988, G_258FB48, G_258EC40, G_258BA90,
			G_258A010, G_257FA90, G_2585EF0, G_258BE20 };
		for (int i = 0; i < 9; i++)
		{
			uint32_t n = x::ExecuteTaskQueue(G(queues[i]));
			U16(a1, 0x5E) = (uint16_t)(U16(a1, 0x5E) + (uint16_t)n);
		}
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x5C) = (uint16_t)(U16(a1, 0x5C) + 1);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		if ((status & 1) && U8(a1, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(a1);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl a_73A0D0(uint32_t a1)
	{
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl a_73A170(uint32_t a1)
	{
		U8(a1, 0x63) = 1;
		x::Effect_AddTaskAndInitFromCtx(G(G_258BE30), F(F_73A1A0), 0x58, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl a_73A1A0(uint32_t a1)
	{
		uint32_t states[5] = { F(F_73A220), F(F_747440), F(F_7474B0), F(F_7474D0), F(F_7474F0) };
		x::MAG_001_CURE_Emitter_UpdatePos(a1);
		x::MAG_001_CURE_Emitter_ComputeModelBounds(a1);
		callp(states[S8(a1, 0x29)], a1);
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		if ((status & 1) && U8(a1, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(a1);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl a_73A380(uint32_t a1)
	{
		const uint32_t fn = U32(a1, 8);
		uint32_t states[3] = { U32(fn, 0x0D), U32(fn, 0x19), U32(fn, 0x21) };
		callp(states[S8(a1, 0x29)], a1);
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		if ((status & 1) && U8(a1, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(a1);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl a_73A3E0(uint32_t a1)
	{
		uint32_t rec = e2_rec();
		uint32_t op = U16(rec, 0);
		if ((int32_t)op > 0xFF)
			return 0;
		if (op == 0xFF)
		{
			U16(e2_ctx(), 0x166) = 0;
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		}
		if (op > 6)
			return 0;
		uint32_t ctx, cb, dst;
		switch (op)
		{
		case 0:
			U16(e2_ctx(), 0x166) = 0;
			return 0;
		case 1:
			ctx = e2_ctx();
			U16(ctx, 0x164) = 1;
			dst = U32(ctx, 0x100);
			U32(rec, 0xC) = dst;
			x::pre_LoadBattleFile(U16(rec, 2), dst, 0, F(F_73A580));
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		case 6:
			ctx = e2_ctx();
			U16(ctx, 0x164) = 1;
			dst = U32(ctx, 0x100);
			U32(rec, 0xC) = dst;
			x::pre_LoadBattleFile(U16(rec, 2), dst, 0, F(F_73A5B0));
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		default:
		{
			int32_t base;
			if (op == 2) { cb = F(F_73A5E0); base = 0x104; }
			else if (op == 3) { cb = F(F_73A5F0); base = 0x124; }
			else if (op == 4) { cb = F(F_73A600); base = 0x144; }
			else { cb = F(F_73A610); base = 0x114; }
			ctx = e2_ctx();
			uint32_t slot = U16(rec, 4);
			U16(ctx, 0x164) = 1;
			dst = U32(ctx, (int32_t)(slot * 4) + base);
			U32(rec, 0xC) = dst;
			x::pre_LoadBattleFile(U16(rec, 2), dst, 0, cb);
			a_73A620();
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return 0;
		}
		}
	}

	uint32_t __cdecl a_73A580(void)
	{
		x::Battle_QueueTIMUpload_GetEOF(U32(e2_rec(), 0xC));
		U16(e2_ctx(), 0x164) = 0;
		return 0;
	}

	uint32_t __cdecl a_73A5B0(void)
	{
		uint32_t flag = e2_ctx() + 0x16A;
		uint32_t data = U32(e2_rec(), 0xC);
		x::BdTransSummonStream(data, flag);
		U16(e2_ctx(), 0x164) = 0;
		return 0;
	}

	uint32_t __cdecl a_73A5E0(void)
	{
		U16(e2_ctx(), 0x164) = 0;
		return 0;
	}

	uint32_t __cdecl a_73A620(void)
	{
		uint32_t ctx = e2_ctx();
		uint32_t rec = e2_rec();
		int32_t n = S16(ctx, 0x168);
		uint32_t start = U32(rec, 0xC);
		uint32_t size = U32(rec, 8);
		U32(ctx, n * 4) = start;
		uint32_t end = start + size;
		int32_t n2 = S16(ctx, 0x168);
		U32(ctx, n2 * 4 + 0x80) = end;
		U16(ctx, 0x168) = (uint16_t)(U16(ctx, 0x168) + 1);
		return 0;
	}

	uint32_t __cdecl a_73A660(uint32_t a1)
	{
		if (U8(a1, 0x26) & 1)
		{
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return a1;
		}
		if (U16(e2_rec(), 0) == 6)
		{
			uint32_t ctx = e2_ctx();
			if (U16(ctx, 0x164) != 0) return a1;
			if (U8(ctx, 0x16A) == 0) return a1;
			U8(ctx, 0x16A) = 0;
		}
		else
		{
			if (U16(e2_ctx(), 0x164) != 0) return a1;
		}
		uint32_t next = e2_rec() + 0x10;
		uint8_t st = (uint8_t)(U8(a1, 0x29) - 1);
		MEM<uint32_t>(G(G_258BFA8)) = next;
		U8(a1, 0x29) = st;
		return a1;
	}

	uint32_t __cdecl a_73A6D0(void)
	{
		return 0;
	}

	uint32_t __cdecl a_73A720(uint32_t a1)
	{
		uint32_t count = 0;
		int16_t max_z = 0, min_z = 0, max_x = 0, min_x = 0;
		for (uint32_t e = game_addr(game::unk_1D974B4); e < game_addr(game::battle_camera_world_y_edx); e += 0x9C)
		{
			if (!(U8(e, -0x20) & 2))
				continue;
			if ((uint16_t)count == 0)
			{
				max_x = S16(e, -4);
				max_z = S16(e, 0);
				min_x = max_x;
				min_z = max_z;
			}
			else
			{
				int16_t x = S16(e, -4);
				if (x < min_x) min_x = x;
				else if (x > max_x) max_x = x;
				int16_t z = S16(e, 0);
				if (z < min_z) min_z = z;
				else if (z > max_z) max_z = z;
			}
			count++;
		}
		int32_t cx = ((int32_t)max_x + (int32_t)min_x) / 2;
		U16(a1, 2) = 0;
		U16(a1, 0) = (uint16_t)cx;
		int32_t cz = ((int32_t)max_z + (int32_t)min_z) / 2;
		U16(a1, 4) = (uint16_t)cz;
		return (uint32_t)cz;
	}

	uint32_t __cdecl a_73A930(uint32_t a1)
	{
		uint32_t r = a_73A950();
		if (r != 1)
			return r;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl a_73A950(void)
	{
		uint32_t ctx = e2_ctx();
		if (U16(ctx, 0x166) != 0)
			return 0;
		uint32_t rec = e2_rec();
		if (U16(rec, 0) != 0)
			return 0;
		U16(ctx, 0x166) = 1;
		MEM<uint32_t>(G(G_258BFA8)) = rec + 0x10;
		return 1;
	}

	uint32_t __cdecl a_73AAE0(uint32_t a1)
	{
		uint32_t dir = e2_dir();
		return S16(dir, 0x42) >= (int16_t)a1 ? 1u : 0u;
	}

	uint32_t __cdecl a_73AB00(void)
	{
		uint32_t cam = e2_cam();
		return x::MAG_007_sub_8DCC00(cam, 0xEC);
	}

	uint32_t __cdecl a_73AB20(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		alignas(4) uint8_t fr[0x28] = {};
		uint32_t vB = P(fr), mA = P(fr) + 8;
		x::MAG_022_sub_8DD770(mA);
		uint32_t c = e2_cam();
		x::MAG_022_sub_8DD8A0(mA, (uint32_t)(int32_t)S16(c, 0xE8));
		c = e2_cam();
		U32(c, 0xAC) = a1;
		U16(c, 0xE2) = 0;
		U16(c, 0xD0) = 0;
		U32(c, 0x50) = U32(a1, 0);
		U32(c, 0xA4) = U32(a2, 0);
		U32(c, 0xA8) = U32(a2, 4);
		int16_t mode = (int16_t)a3;
		if (mode == 0)
		{
			U16(c, 0xC2) = U16(a1, 0x2A);
			U16(c, 0xC6) = U16(a1, 0x2E);
			U16(c, 0xCA) = U16(a1, 0x32);
			U16(c, 0xBA) = U16(a1, 0x36);
			U16(c, 0xBE) = U16(a1, 0x26);
		}
		else if (mode == 1)
		{
			U16(c, 0xC2) = U16(c, 0xD8);
			U16(c, 0xC6) = U16(c, 0xDA);
			U16(c, 0xCA) = U16(c, 0xDC);
			U16(c, 0xBA) = U16(c, 0xE0);
			U16(c, 0xBE) = U16(c, 0xDE);
		}
		U16(c, 0xD2) = U16(a1, 0x3A);
		U16(c, 0xD4) = U16(a1, 0x3C);
		U16(c, 0xD6) = U16(a1, 0x3E);
		U16(c, 0xCE) = U16(a1, 0x24);
		U16(c, 0xC4) = U16(a1, 0x2C);
		U16(c, 0xC8) = U16(a1, 0x30);
		U16(c, 0xCC) = U16(a1, 0x34);
		U16(c, 0xB4) = U16(a1, 0x40);
		U16(c, 0xB6) = U16(a1, 0x42);
		U16(c, 0xB8) = U16(a1, 0x44);
		U16(c, 0xBC) = U16(a1, 0x38);
		U16(c, 0xC0) = U16(a1, 0x28);
		U16(c, 0xC6) = (uint16_t)((uint16_t)(U16(c, 0xC6) + U16(c, 0xE8)) & 0xFFF);
		U16(c, 0xC8) = (uint16_t)((uint16_t)(U16(c, 0xC8) + U16(c, 0xE8)) & 0xFFF);
		int16_t cam_mode = S16(c, 0xCE);
		if (cam_mode == 1)
		{
			U32(c, 0x74) = U32(c, 0xA4);
			U32(c, 0x78) = U32(c, 0xA8);
			x::matrixMultiplyVector(mA, a1 + 0x14, vB);
			c = e2_cam();
			U16(c, 0x74) = (uint16_t)(U16(c, 0x74) + U16(vB, 0));
			U16(c, 0x76) = (uint16_t)(U16(c, 0x76) + U16(vB, 2));
			U16(c, 0x78) = (uint16_t)(U16(c, 0x78) + U16(vB, 4));
			U32(c, 0x7C) = U32(c, 0xA4);
			U32(c, 0x80) = U32(c, 0xA8);
			x::matrixMultiplyVector(mA, a1 + 0x1C, vB);
			c = e2_cam();
			U16(c, 0x7C) = (uint16_t)(U16(c, 0x7C) + U16(vB, 0));
			U16(c, 0x7E) = (uint16_t)(U16(c, 0x7E) + U16(vB, 2));
			U16(c, 0x80) = (uint16_t)(U16(c, 0x80) + U16(vB, 4));
		}
		else if (cam_mode == 0)
		{
			U32(c, 0x64) = U32(c, 0xA4);
			U32(c, 0x68) = U32(c, 0xA8);
			x::matrixMultiplyVector(mA, a1 + 4, vB);
			c = e2_cam();
			U16(c, 0x64) = (uint16_t)(U16(c, 0x64) + U16(vB, 0));
			U16(c, 0x66) = (uint16_t)(U16(c, 0x66) + U16(vB, 2));
			U16(c, 0x68) = (uint16_t)(U16(c, 0x68) + U16(vB, 4));
			U32(c, 0x6C) = U32(c, 0xA4);
			U32(c, 0x70) = U32(c, 0xA8);
			x::matrixMultiplyVector(mA, a1 + 0xC, vB);
			c = e2_cam();
			U16(c, 0x6C) = (uint16_t)(U16(c, 0x6C) + U16(vB, 0));
			U16(c, 0x6E) = (uint16_t)(U16(c, 0x6E) + U16(vB, 2));
			U16(c, 0x70) = (uint16_t)(U16(c, 0x70) + U16(vB, 4));
		}
		return a_73AE10();
	}

	uint32_t __cdecl a_73AE10(void)
	{
		uint32_t c = e2_cam();
		uint32_t done = 0;
		U16(c, 0xE2) = (uint16_t)(U16(c, 0xE2) + 1);
		uint32_t curve = U32(c, 0x50);
		if (curve == 0)
		{
			U16(c, 0xD2) = (uint16_t)(U16(c, 0xD2) + U16(c, 0xD4));
			int16_t accel = S16(c, 0xD4);
			int16_t speed = S16(c, 0xD2);
			if (accel > 0)
			{
				int16_t lim = S16(c, 0xD6);
				if (speed > lim) U16(c, 0xD2) = (uint16_t)lim;
			}
			else if (accel < 0)
			{
				int16_t lim = S16(c, 0xD6);
				if (speed < lim) U16(c, 0xD2) = (uint16_t)lim;
			}
			U16(c, 0xD0) = (uint16_t)(U16(c, 0xD0) + U16(c, 0xD2));
		}
		else
		{
			uint16_t t = U16(curve, 0);
			U16(c, 0xD0) = t;
			if (t != 0x1000)
				U32(c, 0x50) = curve + 2;
		}
		if (S16(c, 0xD0) >= 0x1000)
		{
			U16(c, 0xD0) = 0x1000;
			done = 1;
		}

		{
			bool wrap_from = U16(c, 0xB4) != 0;
			U16(c, 0xD8) = e2_angle_lerp(c, 0xC2, 0xC4, wrap_from);
		}
		{
			int16_t sel = S16(c, 0xB6);
			if (sel == 0)
				U16(c, 0xDA) = e2_angle_lerp(c, 0xC6, 0xC8, false);
			else if (sel == 1)
				U16(c, 0xDA) = e2_angle_lerp(c, 0xC6, 0xC8, true);
		}
		{
			bool wrap_from = U16(c, 0xB8) != 0;
			U16(c, 0xDC) = e2_angle_lerp(c, 0xCA, 0xCC, wrap_from);
		}
		U16(c, 0xE0) = e2_lerp(c, 0xBA, 0xBC);
		U16(c, 0xDE) = e2_lerp(c, 0xBE, 0xC0);
		int16_t cam_mode = S16(c, 0xCE);
		if (cam_mode == 1)
		{
			U16(c, 0x54) = e2_lerp(c, 0x74, 0x7C);
			U16(c, 0x56) = e2_lerp(c, 0x76, 0x7E);
			U16(c, 0x58) = e2_lerp(c, 0x78, 0x80);
		}
		else if (cam_mode == 0)
		{
			U16(c, 0x5C) = e2_lerp(c, 0x64, 0x6C);
			U16(c, 0x5E) = e2_lerp(c, 0x66, 0x6E);
			U16(c, 0x60) = e2_lerp(c, 0x68, 0x70);
		}
		a_73B160();
		return done;
	}

	uint32_t __cdecl a_73B160(void)
	{
		alignas(4) uint8_t fr[0x38] = {};
		uint32_t L = P(fr);
		U16(L, 0) = MEM<uint16_t>(game_addr(game::Battle_Camera_world_XZ_s16));
		U16(L, 2) = MEM<uint16_t>(game_addr(game::unk_B8B7F2));
		U16(L, 4) = MEM<uint16_t>(game_addr(game::Battle_Camera_world_Y));
		U16(L, 6) = MEM<uint16_t>(game_addr(game::unk_B8B7F6));
		uint32_t mat = L + 0x18;
		x::MAG_022_sub_8DD770(mat);
		uint32_t c = e2_cam();
		x::MAG_022_sub_8DD8A0(mat, (uint32_t)(int32_t)S16(c, 0xDA));
		c = e2_cam();
		x::sub_8DD7E0(mat, (uint32_t)(int32_t)S16(c, 0xD8));
		c = e2_cam();
		U16(L, 0x10) = 0;
		U16(L, 0x12) = 0;
		U16(L, 0x14) = U16(c, 0xDE);
		x::GTE_SetRotMatrix(mat);
		x::GTE_LoadV0(L + 0x10);
		x::GTE_MVMVA_RotV0();
		x::GTE_StoreIR123(L + 8);
		c = e2_cam();
		int16_t cam_mode = S16(c, 0xCE);
		if (cam_mode == 1)
		{
			U16(L, 0) = (uint16_t)(U16(c, 0x54) + U16(L, 8));
			U16(L, 2) = (uint16_t)(U16(c, 0x56) + U16(L, 0xA));
			U16(L, 4) = (uint16_t)(U16(c, 0x58) + U16(L, 0xC));
			U16(c, 0x5C) = (uint16_t)(U16(L, 0) + U16(L, 8));
			U16(c, 0x5E) = (uint16_t)(U16(L, 2) + U16(L, 0xA));
			U16(c, 0x60) = (uint16_t)(U16(L, 4) + U16(L, 0xC));
		}
		else if (cam_mode == 0)
		{
			U16(L, 0) = (uint16_t)(U16(c, 0x5C) + U16(L, 8));
			U16(L, 2) = (uint16_t)(U16(c, 0x5E) + U16(L, 0xA));
			U16(L, 4) = (uint16_t)(U16(c, 0x60) + U16(L, 0xC));
			U32(c, 0x54) = U32(L, 0);
			U32(c, 0x58) = U32(L, 4);
		}
		uint32_t look_xy = U32(c, 0x5C);
		uint32_t look_z = U32(c, 0x60);
		MEM<uint32_t>(game_addr(game::Battle_Camera_LookAt_XZ_s16)) = look_xy;
		MEM<uint32_t>(game_addr(game::Battle_Camera_world_XZ_s16)) = U32(L, 0);
		uint16_t e0 = U16(c, 0xE0);
		uint16_t dc = U16(c, 0xDC);
		MEM<uint32_t>(game_addr(game::Battle_Camera_LookAt_Y)) = look_z;
		MEM<uint16_t>(game_addr(game::BATTLE_PROJECTION_DISTANCE)) = e0;
		MEM<uint32_t>(game_addr(game::Battle_Camera_world_Y)) = U32(L, 4);
		MEM<uint16_t>(game_addr(game::g_BattleCam_Roll)) = dc;
		return 0;
	}

	uint32_t __cdecl a_73B2D0(uint32_t a1)
	{
		uint32_t r = a_73AE10();
		if (r != 1)
			return r;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl a_73B350(uint32_t a1)
	{
		a_73AE10();
		uint32_t r = a_73AAE0(4);
		if (r == 0)
			return 0;
		uint32_t dir = e2_dir();
		a_73AB20(G(G_1533D18), dir, 0);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl a_73B3B0(uint32_t a1)
	{
		uint32_t c = e2_cam();
		U16(c, 0xE6) = 0;
		U32(c, 0) = U32(a1, 0);
		U32(c, 0x14) = U32(a1, 4);
		uint32_t k2 = U32(a1, 8);
		uint32_t k3 = U32(a1, 0xC);
		U32(c, 0x28) = k2;
		U32(c, 0x3C) = k3;
		a_73B430(c, c + 0x5E);
		c = e2_cam();
		a_73B430(c + 0x14, c + 0xDE);
		c = e2_cam();
		a_73B430(c + 0x28, c + 0xD8);
		c = e2_cam();
		return a_73B430(c + 0x3C, c + 0xDA);
	}

	uint32_t __cdecl a_73B430(uint32_t a1, uint32_t a2)
	{
		uint32_t p = U32(U32(a1, 0), 0);
		uint16_t op = U16(p, 0);
		p += 2;
		U16(a1, 8) = op;
		int32_t sop = (int16_t)op;
		if (sop == 1)
		{
			uint16_t from = U16(p, 0);
			if (from == 0x7FFE)
				from = U16(a2, 0);
			p += 2;
			U16(a1, 0xA) = from;
			U16(a1, 0xC) = U16(p, 0);
			p += 2;
		}
		else if (sop == 2)
		{
			U16(a1, 0x10) = U16(p, 0);
			p += 2;
			U32(a1, 4) = p;
			return p;
		}
		U32(a1, 4) = p;
		return p;
	}

	uint32_t __cdecl a_73B4B0(void)
	{
		uint32_t c = e2_cam();
		a_73B520(c, c + 0x5E);
		c = e2_cam();
		a_73B520(c + 0x14, c + 0xDE);
		c = e2_cam();
		a_73B520(c + 0x28, c + 0xD8);
		c = e2_cam();
		a_73B520(c + 0x3C, c + 0xDA);
		c = e2_cam();
		U16(c, 0xE6) = (uint16_t)(U16(c, 0xE6) + 1);
		return a_73B160();
	}

	uint32_t __cdecl a_73B520(uint32_t a1, uint32_t a2)
	{
		if (U32(U32(a1, 0), 0) == 0)
			return 1;
		uint32_t r = a_73B570(a1, a2);
		if (r == 1)
		{
			uint32_t np = U32(a1, 0) + 4;
			U32(a1, 0) = np;
			if (U32(np, 0) == 0)
				return 1;
			a_73B430(a1, a2);
		}
		return 0;
	}

	uint32_t __cdecl a_73B570(uint32_t a1, uint32_t a2)
	{
		int32_t op = S16(a1, 8);
		if (op == 0)
		{
			uint32_t p = U32(a1, 4);
			uint16_t v = U16(p, 0);
			if (v == 0x7FFF)
				return 1;
			U16(a1, 0xE) = v;
			U32(a1, 4) = p + 2;
			U16(a2, 0) = v;
			return 0;
		}
		if (op == 1)
		{
			uint32_t p = U32(a1, 4);
			uint16_t t = U16(p, 0);
			if (t == 0x7FFF)
				return 1;
			int16_t from = S16(a1, 0xA);
			U32(a1, 4) = p + 2;
			int32_t r = e2_mulq12((int32_t)S16(a1, 0xC), (int32_t)from, (int32_t)(int16_t)t);
			uint16_t v = (uint16_t)add32(r, (int32_t)from);
			U16(a1, 0xE) = v;
			U16(a2, 0) = v;
			return 0;
		}
		if (op == 2)
		{
			uint32_t c = e2_cam();
			if (S16(c, 0xE6) < S16(a1, 0x10))
				return 0;
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl a_73B620(uint32_t a1)
	{
		uint32_t r = a_73B640(5);
		if (r == 0)
			return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl a_73B640(uint32_t a1)
	{
		uint32_t dir = e2_dir();
		uint16_t cur = U16(dir, 0x44);
		if (cur != U16(dir, 0x40))
			return 0;
		int16_t want = (int16_t)a1;
		if ((int32_t)(int16_t)cur != (int32_t)want - 1)
			return 0;
		U16(dir, 0x44) = (uint16_t)want;
		return 1;
	}

	uint32_t __cdecl a_73B750(uint32_t a1)
	{
		a_73A6D0();
		U8(a1, 0x26) |= 1;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl a_73B790(uint32_t a1)
	{
		uint32_t r = a_73B640(1);
		if (r == 0)
			return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl a_73B7E0(uint32_t a1)
	{
		uint32_t dir = e2_dir();
		return S16(dir, 0x40) >= (int16_t)a1 ? 1u : 0u;
	}

	uint32_t __cdecl a_73B8A0(uint32_t a1)
	{
		uint32_t r = a_73B640(2);
		if (r == 0)
			return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl a_73B8C0(uint32_t a1)
	{
		uint32_t r = a_73B7E0(2);
		if (r == 0)
			return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl a_73B9F0(void)
	{
		uint32_t save = e2_dir() + 0x2C;
		for (uint32_t e = game_addr(game::unk_1D97494); e < game_addr(game::CAMERA_FLAG_RELATED); e += 0x9C, save += 4)
		{
			uint16_t f = U16(e, 0);
			if (f & 2)
			{
				U32(save, 0) = (uint32_t)f;
				U16(e, 0) = (uint16_t)(f | 4);
			}
		}
		return 0;
	}

	uint32_t __cdecl a_73BA90(void)
	{
		uint32_t save = e2_dir() + 0x2C;
		for (uint32_t e = game_addr(game::unk_1D97494); e < game_addr(game::CAMERA_FLAG_RELATED); e += 0x9C, save += 4)
		{
			uint16_t f = U16(e, 0);
			if (f & 2)
			{
				uint32_t v = ((uint32_t)(uint8_t)(U8(save, 0) ^ (uint8_t)f) & 4) ^ (uint32_t)f;
				U16(e, 0) = (uint16_t)v;
			}
		}
		return 0;
	}

	uint32_t __cdecl a_73BB60(uint32_t a1)
	{
		uint32_t cur = MEM<uint32_t>(G(G_258FB70));
		if (a1 == 0)
		{
			if (cur != 1)
				return 0;
			MEM<uint32_t>(G(G_258FB70)) = 0;
			MEM<uint32_t>(G(G_2585E38)) = 0;
			x::nullsub_1029();
			return 0;
		}
		if (cur != 0)
			return 0;
		MEM<uint32_t>(G(G_258FB70)) = 1;
		MEM<uint32_t>(G(G_2585E38)) = 1;
		x::nullsub_1029();
		return 0;
	}

	uint32_t __cdecl a_73BBD0(uint32_t a1)
	{
		uint32_t r = a_73B640(7);
		if (r == 0)
			return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl a_73BC40(uint32_t a1)
	{
		U16(a1, 0x44) = (uint16_t)(U16(a1, 0x44) - 1);
		if (S16(a1, 0x44) <= 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl a_73BCB0(uint32_t a1)
	{
		uint32_t r = a_73B640(9);
		if (r == 0)
			return 0;
		U8(a1, 0x26) |= 1;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl a_73C100(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		uint32_t n = x::Effect_AddTaskAndInitFromCtx(G(G_258BA90), a2, 0x2A4, a1);
		U32(n, 0x30) = a3;
		U16(n, 0x298) = (uint16_t)a4;
		U16(n, 0x29C) = (uint16_t)a6;
		U16(n, 0x29E) = (uint16_t)a5;
		return n;
	}

	uint32_t __cdecl a_73C280(uint32_t a1)
	{
		alignas(4) uint8_t fr[0x5C] = {};
		uint32_t L = P(fr);
		x::MAG_022_sub_8DD770(L);
		uint16_t ax = U16(a1, 0x62);
		if (ax != 0)
			x::MAG_022_sub_8DD8A0(L, (uint32_t)(int32_t)(int16_t)ax);
		ax = U16(a1, 0x60);
		if (ax != 0)
			x::sub_8DD7E0(L, (uint32_t)(int32_t)(int16_t)ax);
		ax = U16(a1, 0x64);
		if (ax != 0)
			x::sub_8DD960(L, (uint32_t)(int32_t)(int16_t)ax);
		x::scale3DMatrix(L, a1 + 0x50);
		S32(L, 0x14) = S16(a1, 0x1C);
		S32(L, 0x18) = S16(a1, 0x1E);
		S32(L, 0x1C) = S16(a1, 0x20);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), L, L);
		U16(L, 0x54) = U16(a1, 0x86);
		U32(L, 0x44) = U32(a1, 0x7C);
		U16(L, 0x4C) = U16(a1, 0x8C);
		U16(L, 0x4E) = U16(a1, 0x8E);
		U32(L, 0x38) = U32(a1, 0x70);
		U16(L, 0x52) = U16(a1, 0x92);
		uint32_t paused = MEM<uint32_t>(G(G_2585E38));
		U16(L, 0x56) = U16(a1, 0x80);
		U16(L, 0x50) = U16(a1, 0x90);
		U32(L, 0x48) = G(G_258EF98);
		U16(L, 0x58) = U16(a1, 0x88);
		return prim_play(a1 + 0x94, F(F_73C3B0), L, paused);
	}

	static inline uint32_t e3_sx16(uint16_t v) { return (uint32_t)(int32_t)(int16_t)v; }
	static inline bool e3_out_x(int16_t v) { return v < 0 || v > 0xA00; }
	static inline bool e3_out_y(int16_t v) { return v < 0 || v > 0x6C0; }

	uint32_t __cdecl a_73D770(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t cursor = a4;
		uint32_t list = U32(ctx, 0x2C);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = rec;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return a4;
		}
		uint32_t pkt = cursor;
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t flags = U32(ctx, 0x14);
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
			x::GTE_ReadFLAG(ctx + 0x3C);
			if (!(U32(ctx, 0x3C) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) >= 0 || (U8(ctx, 0x14) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					x::GTE_AVSZ3();
					if (e3_out_x(S16(pkt, 8))) clip = 1;
					if (e3_out_x(S16(pkt, 0x10))) clip |= 2;
					if (e3_out_x(S16(pkt, 0x18))) clip |= 4;
					if (e3_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (e3_out_y(S16(pkt, 0x12))) clip |= 0x20;
					if (e3_out_y(S16(pkt, 0x1A))) clip |= 0x40;
					if ((uint8_t)(clip & 7) != 7 && (uint8_t)(clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x38);
						if (U8(ctx, 0x14) & 0x80)
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
						int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
						S32(ctx, 0x38) = z;
						if (z < 0)
							S32(ctx, 0x38) = 0;
						z = S32(ctx, 0x38) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)z * 4, cursor);
						cursor += 0x1C;
						pkt += 0x1C;
					}
				}
			}
			rec += 0x14;
		} while (--count);
		U32(ctx, 0x2C) = rec;
		return cursor;
	}

	uint32_t __cdecl a_73D9C0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t cursor = a4;
		uint32_t list = U32(ctx, 0x2C);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = rec;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return a4;
		}
		uint32_t pkt = cursor;
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			uint32_t flags = U32(ctx, 0x14);
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
			x::GTE_ReadFLAG(ctx + 0x3C);
			if (!(U32(ctx, 0x3C) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) >= 0 || (U8(ctx, 0x14) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
					x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0x0A) * 4);
					x::GTE_RTPS();
					if (e3_out_x(S16(pkt, 8))) clip = 1;
					if (e3_out_x(S16(pkt, 0x10))) clip |= 2;
					if (e3_out_x(S16(pkt, 0x18))) clip |= 4;
					if (e3_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (e3_out_y(S16(pkt, 0x12))) clip |= 0x20;
					if (e3_out_y(S16(pkt, 0x1A))) clip |= 0x40;
					x::GTE_ReadSXY2(pkt + 0x20);
					x::GTE_AVSZ4();
					if (e3_out_x(S16(pkt, 0x20))) clip |= 8;
					if (e3_out_y(S16(pkt, 0x22))) clip |= 0x80;
					if ((uint8_t)(clip & 0xF) != 0xF && (uint8_t)(clip & 0xF0) != 0xF0)
					{
						x::GTE_ReadOTZ(ctx + 0x38);
						if (U8(ctx, 0x14) & 0x80)
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
						int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
						S32(ctx, 0x38) = z;
						if (z < 0)
							S32(ctx, 0x38) = 0;
						z = S32(ctx, 0x38) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)z * 4, cursor);
						cursor += 0x24;
						pkt += 0x24;
					}
				}
			}
			rec += 0x18;
		} while (--count);
		U32(ctx, 0x2C) = rec;
		return cursor;
	}

	uint32_t __cdecl a_73F110(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t fn = U32(node, 8);
		uint32_t states[5] = { U32(fn, 0x0D), U32(fn, 0x19), U32(fn, 0x21), U32(fn, 0x29), U32(fn, 0x31) };
		int32_t state = S8(node, 0x29);
		callp(states[state], node);
		uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((status & 1) && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl a_73F990(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t data = U32(node, 0x30);
		uint16_t w29a = U16(node, 0x29A);
		uint16_t w29c = U16(node, 0x29C);
		uint32_t st = node + 0x34;
		const uint32_t root = U32(node, 0x10);
		MEM<uint32_t>(G(G_258FB78)) = st;
		U16(st, 0x22) = w29a;
		uint16_t caster = (uint16_t)U8(node, 0x2C);
		U16(st, 0x24) = w29c;
		U32(st, 0x224) = data + 4;
		U32(st, 0x228) = data + 0x44;
		U32(st, 0x22C) = data + 0x84;
		uint32_t relocated = U32(data, 0);
		U32(st, 0x230) = data + 0xC4;
		if (relocated == 0)
		{
			a_740210(data);
			st = MEM<uint32_t>(G(G_258FB78));
			U32(data, 0) = 1;
		}
		uint16_t ntargets = U16(root, 0x5A);
		U16(st, 0x1E) = caster;
		U16(st, 0x1C) = ntargets;
		if ((int16_t)ntargets > 0)
		{
			int32_t i = 0;
			do
			{
				int32_t action = S8(node, 0x2A);
				uint32_t tab = U32(U32(node, 0x0C), 4);
				uint32_t rec = U32(tab + (uint32_t)mul32(action, 5) * 4, 8);
				U32(st, 4 + 4 * i) = U8(rec, 0x18 * i);
				i++;
			} while (i < S16(st, 0x1C));
		}
		U16(st, 0x1A) = U16(node, 0x29E);
		a_73FC20();
		st = MEM<uint32_t>(G(G_258FB78));
		if (U16(st, 0x24) == 4)
			callp(F(F_73FB40), node);
		a_73FFB0();
		st = MEM<uint32_t>(G(G_258FB78));
		if (U16(st, 0x24) == 1)
		{
			callp(F(F_73FDC0), node);
			st = MEM<uint32_t>(G(G_258FB78));
		}
		if (U16(st, 0x24) == 3)
		{
			a_73FE90(node);
			st = MEM<uint32_t>(G(G_258FB78));
		}
		int32_t s1 = S16(st, 0x1E);
		uint32_t e1 = game_addr(game::BattleEntitySlotData) + (uint32_t)mul32(s1, 0x9C);
		uint32_t d10 = U32(e1, 0x1C);
		uint32_t d14 = U32(e1, 0x20);
		uint32_t s2 = U32(st, 4);
		uint32_t e2 = game_addr(game::BattleEntitySlotData) + s2 * 0x9C;
		uint32_t d18 = U32(e2, 0x1C);
		int32_t dx = (int16_t)(uint16_t)((uint16_t)d10 - (uint16_t)d18);
		int32_t dy = (int16_t)(uint16_t)((uint16_t)(d10 >> 16) - (uint16_t)(d18 >> 16));
		int32_t dz = (int16_t)(uint16_t)((uint16_t)d14 - (uint16_t)U32(e2, 0x20));
		int32_t sq = add32(add32(mul32(dx, dx), mul32(dy, dy)), mul32(dz, dz));
		uint32_t dist = x::Sqrt((uint32_t)sq);
		st = MEM<uint32_t>(G(G_258FB78));
		U32(st, 0) = dist;
		return 0;
	}

	uint32_t __cdecl a_73FC20(void)
	{
		uint32_t st = MEM<uint32_t>(G(G_258FB78));
		int32_t slot = S16(st, 0x1E);
		const uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)mul32(slot, 0x9C);
		alignas(4) uint8_t pos[8] = {};
		const uint32_t pp = P(pos);
		if (U16(st, 0x24) == 2)
		{
			uint32_t fixed = MEM<uint32_t>(G(G_1533010));
			U32(pp, 0) = U32(fixed, 0);
			U32(pp, 4) = U32(fixed, 4);
		}
		else
		{
			x::GetEffectSpawnPosition(ent, 0xF1, 0, pp);
			st = MEM<uint32_t>(G(G_258FB78));
		}
		U16(pp, 2) = U16(ent, 0x24);
		S32(st, 0x1C4) = shl32(S16(pp, 0), 16);
		S32(st, 0x1C8) = shl32(S16(pp, 2), 16);
		S32(st, 0x1CC) = shl32(S16(pp, 4), 16);
		U32(st, 0x1D4) = U32(st, 0x1C4);
		U32(st, 0x1D8) = 0;
		U32(st, 0x1DC) = U32(st, 0x1CC);
		if (U16(st, 0x24) == 2)
		{
			uint32_t fixed = MEM<uint32_t>(G(G_1533010));
			U32(pp, 0) = U32(fixed, 0);
			U32(pp, 4) = U32(fixed, 4);
		}
		else
		{
			x::GetEffectSpawnPosition(ent, 0xF1, 0, pp);
			st = MEM<uint32_t>(G(G_258FB78));
		}
		S32(st, 0x1E4) = shl32(S16(pp, 0), 16);
		S32(st, 0x1E8) = shl32(S16(pp, 2), 16);
		S32(st, 0x1EC) = shl32(S16(pp, 4), 16);
		if (U16(st, 0x24) == 2)
		{
			uint32_t fixed = MEM<uint32_t>(G(G_1533010));
			U32(pp, 0) = U32(fixed, 0);
			U32(pp, 4) = U32(fixed, 4);
		}
		else
		{
			x::GetEffectSpawnPosition(ent, 0xF0, 0, pp);
			st = MEM<uint32_t>(G(G_258FB78));
		}
		S32(st, 0x1F4) = shl32(S16(pp, 0), 16);
		S32(st, 0x1F8) = shl32(S16(pp, 2), 16);
		S32(st, 0x1FC) = shl32(S16(pp, 4), 16);
		uint32_t sx = U32(st, 0x1C4);
		int32_t h = shl32(S16(ent, 0x3C), 16);
		U32(st, 0x204) = sx;
		S32(st, 0x208) = h;
		U32(st, 0x20C) = U32(st, 0x1CC);
		return 0;
	}

	uint32_t __cdecl a_73FE90(uint32_t a1)
	{
		const uint32_t st = MEM<uint32_t>(G(G_258FB78));
		if (S16(st, 0x1C) <= 0)
			return 0;
		int32_t px = shl32(S16(a1, 0x288), 16);
		int32_t pz = shl32(S16(a1, 0x28C), 16);
		int32_t py = shl32(S16(a1, 0x28A), 16);
		const uint32_t src = st + 0x1A4;
		uint32_t base = st + 0x114;
		int32_t i = 0;
		static const int32_t dst_off[5] = { 0x40, 0, -0x40, -0x80, -0xC0 };
		do
		{
			S32(src, 0) = px;
			S32(st, 0x1A8) = py;
			S32(st, 0x1AC) = pz;
			for (int k = 0; k < 5; k++)
			{
				uint32_t dst = base + dst_off[k];
				U32(dst, 0) = U32(src, 0);
				U32(dst, 4) = U32(src, 4);
				U32(dst, 8) = U32(src, 8);
				U32(dst, 0xC) = U32(src, 0xC);
			}
			base += 0x10;
			i++;
		} while (i < S16(st, 0x1C));
		return 0;
	}

	uint32_t __cdecl a_73FFB0(void)
	{
		uint32_t st = MEM<uint32_t>(G(G_258FB78));
		if (S16(st, 0x1C) > 0)
		{
			uint32_t fixed = MEM<uint32_t>(G(G_1533010));
			uint32_t off = 0;
			uint32_t sofs = 4;
			int32_t i = 0;
			alignas(4) uint8_t pos[8] = {};
			const uint32_t pp = P(pos);
			do
			{
				uint32_t slot = U32(sofs + st, 0);
				const uint32_t ent = game_addr(game::BattleEntitySlotData) + slot * 0x9C;
				if (U16(st, 0x24) == 2)
				{
					U32(pp, 0) = U32(fixed, 8);
					U32(pp, 4) = U32(fixed, 0xC);
				}
				else
				{
					x::GetEffectSpawnPosition(ent, 0xF1, 0, pp);
					st = MEM<uint32_t>(G(G_258FB78));
					fixed = MEM<uint32_t>(G(G_1533010));
				}
				U16(pp, 2) = U16(ent, 0x24);
				uint32_t e = off + st;
				S32(e, 0x54) = shl32(S16(pp, 0), 16);
				S32(e, 0x58) = shl32(S16(pp, 2), 16);
				S32(e, 0x5C) = shl32(S16(pp, 4), 16);
				U32(e, 0x94) = U32(e, 0x54);
				U32(e, 0x98) = 0;
				U32(e, 0x9C) = U32(e, 0x5C);
				if (U16(st, 0x24) == 2)
				{
					U32(pp, 0) = U32(fixed, 8);
					U32(pp, 4) = U32(fixed, 0xC);
				}
				else
				{
					x::GetEffectSpawnPosition(ent, 0xF1, 0, pp);
					st = MEM<uint32_t>(G(G_258FB78));
					fixed = MEM<uint32_t>(G(G_1533010));
				}
				e = off + st;
				S32(e, 0xD4) = shl32(S16(pp, 0), 16);
				S32(e, 0xD8) = shl32(S16(pp, 2), 16);
				S32(e, 0xDC) = shl32(S16(pp, 4), 16);
				if (U16(st, 0x24) == 2)
				{
					U32(pp, 0) = U32(fixed, 8);
					U32(pp, 4) = U32(fixed, 0xC);
				}
				else
				{
					x::GetEffectSpawnPosition(ent, 0xF0, 0, pp);
					st = MEM<uint32_t>(G(G_258FB78));
					fixed = MEM<uint32_t>(G(G_1533010));
				}
				e = off + st;
				S32(e, 0x114) = shl32(S16(pp, 0), 16);
				sofs += 4;
				S32(e, 0x118) = shl32(S16(pp, 2), 16);
				S32(e, 0x11C) = shl32(S16(pp, 4), 16);
				U32(e, 0x154) = U32(e, 0x54);
				S32(e, 0x158) = shl32(S16(ent, 0x3C), 16);
				U32(e, 0x15C) = U32(e, 0x5C);
				off += 0x10;
				i++;
			} while (i < S16(st, 0x1C));
		}
		int32_t n = S16(st, 0x1C);
		int32_t min_x = 0, max_x = 0, min_z = 0, max_z = 0;
		if (n > 0)
		{
			uint32_t p = st + 0x5C;
			uint32_t left = (uint32_t)n;
			uint32_t idx = 0;
			do
			{
				if ((uint16_t)idx == 0)
				{
					max_x = S32(p, -8);
					max_z = S32(p, 0);
					min_x = max_x;
					min_z = max_z;
				}
				else
				{
					int32_t vx = S32(p, -8);
					if (vx < min_x)
						min_x = vx;
					else if (vx > max_x)
						max_x = vx;
					int32_t vz = S32(p, 0);
					if (vz < min_z)
						min_z = vz;
					else if (vz > max_z)
						max_z = vz;
				}
				idx++;
				p += 0x10;
			} while (--left);
		}
		S32(st, 0x194) = add32(max_x, min_x) / 2;
		S32(st, 0x198) = 0;
		S32(st, 0x19C) = add32(max_z, min_z) / 2;
		return 0;
	}

	uint32_t __cdecl a_740210(uint32_t a1)
	{
		const uint32_t base = a1;
		const uint32_t st = MEM<uint32_t>(G(G_258FB78));
		const uint32_t tab0 = U32(st, 0x224);
		for (int t = 0; t < 4; t++)
		{
			uint32_t tab = U32(st, 0x224 + 4 * t);
			for (int k = 0; k < 16; k++)
			{
				uint32_t v = U32(tab, 4 * k);
				if (v != 0)
					U32(tab, 4 * k) = v + base;
			}
		}
		for (int k = 0; k < 16; k++)
		{
			uint32_t e = U32(tab0, 4 * k);
			if (e == 0)
				continue;
			for (int j = 0; j < 32; j++)
				U32(e, 0xC8 + 4 * j) += base;
		}
		return 0;
	}

	uint32_t __cdecl a_740700(void)
	{
		uint32_t act = U32(MEM<uint32_t>(G(G_258FB78)), 0x2C);
		if (act == 0)
			return 0;
		do
		{
			if (U16(act, 8) == 1)
			{
				uint32_t st = MEM<uint32_t>(G(G_258FB78));
				int32_t b = S8(act, 0x1D6);
				uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
				uint8_t kind = U8(bone, 0x16);
				if ((kind == 0 || kind == 1 || kind == 2) && U32(act, 0x170) != 0)
				{
					int8_t n = S8(act, 0x1D8);
					if (n == 1)
					{
						a_7407C0(act);
					}
					else if (n > 0)
					{
						int32_t i = 0;
						uint32_t sub = act + 0x19E;
						do
						{
							S32(act, 0xC0) = S16(sub, -2);
							S32(act, 0xC4) = S16(sub, 0);
							S32(act, 0xC8) = S16(sub, 2);
							a_7407C0(act);
							i++;
							sub += 8;
						} while (i < S8(act, 0x1D8));
					}
				}
			}
			act = U32(act, 4);
		} while (act != 0);
		return 0;
	}

	uint32_t __cdecl a_7407C0(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t hdr = MEM<uint32_t>(G(G_258FB68)) - 0xB4;
		MEM<uint32_t>(G(G_258FB68)) = hdr;
		const uint32_t mtx = act + 0xAC;
		x::GTE_SetRotMatrix(mtx);
		x::GTE_SetTransVector(mtx);
		uint32_t seq = U32(act, 0x170);
		uint32_t cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		int16_t variant = (int16_t)S8(act, 0x1D0);
		U32(hdr, 0) = seq;
		uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		S16(hdr, 4) = variant;
		U16(hdr, 0x24) = 0;
		uint32_t r = x::InitEffectSequenceFromData_c19(hdr, ot, 2, cursor);
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = r;
		MEM<uint32_t>(G(G_258FB68)) = MEM<uint32_t>(G(G_258FB68)) + 0xB4;
		return 0;
	}

	uint32_t __cdecl a_740880(uint32_t a1)
	{
		const uint32_t act = a1;
		uint32_t st = MEM<uint32_t>(G(G_258FB78));
		int32_t b = S8(act, 0x1D6);
		const uint32_t bone = U32(U32(st, 0x224) + (uint32_t)b * 4, 0);
		int32_t phase = S16(act, 0x1CC);
		if (phase == 0)
		{
			a_741050(act, bone);
			if (a_7419C0(act, bone) == 0)
			{
				a_740910(act, bone);
				a_741120(act, bone);
				U16(act, 0x1CA) = (uint16_t)(U16(act, 0x1CA) + 1);
				return 0;
			}
			U16(act, 0x1CC) = (uint16_t)(U16(act, 0x1CC) + 1);
			U32(act, 0x170) = 0;
			return 0;
		}
		if (phase == 1)
		{
			a_742CD0(act);
			st = MEM<uint32_t>(G(G_258FB78));
			U8(act, 0x1D7) = 0;
			U16(st, 0x16) = (uint16_t)(U16(st, 0x16) - 1);
		}
		return 0;
	}

	uint32_t __cdecl a_740910(uint32_t a1, uint32_t a2)
	{
		const uint32_t act = a1;
		const uint32_t ent = a2;
		alignas(4) uint8_t loc[0x38 + 127 * 0x10] = {};
		const uint32_t vec = P(loc) + 0x08;
		const uint32_t mat = P(loc) + 0x18;
		const uint32_t out = P(loc) + 0x38;

		uint32_t b194 = U32(act, 0x194);
		uint32_t b198 = U32(act, 0x198);
		U32(act, 0xCC) = b194;
		U32(act, 0xD0) = b198;
		int32_t fo = shl32(S16(act, 0x1CA), 1);
		uint16_t d;
		d = U16(U32(ent, 0xD8) + (uint32_t)fo, 0);
		U16(act, 0xCC) = (uint16_t)(U16(act, 0xCC) + d);
		d = U16(U32(ent, 0xDC) + (uint32_t)fo, 0);
		U16(act, 0xCE) = (uint16_t)(U16(act, 0xCE) + d);
		d = U16(U32(ent, 0xE0) + (uint32_t)fo, 0);
		U16(act, 0xD0) = (uint16_t)(U16(act, 0xD0) + d);
		U16(act, 0xCC) &= 0xFFF;
		U16(act, 0xCE) &= 0xFFF;
		U16(act, 0xD0) &= 0xFFF;
		for (int k = 0; k < 4; k++)
			U32(act, 0x11C + 4 * k) = U32(act, 0xDC + 4 * k);

		int32_t mode = S8(ent, 0x27);
		if (mode == 1)
		{
			if (S8(act, 0x1D8) > 0)
			{
				int32_t i = 0;
				uint32_t off = act + 0x12C;
				do
				{
					S32(off, 0) = shl32(S16(U32(ent, 0x13C) + (uint32_t)S16(act, 0x1CA) * 2, 0), 16);
					S32(off, 4) = shl32(S16(U32(ent, 0x140) + (uint32_t)S16(act, 0x1CA) * 2, 0), 16);
					S32(off, 8) = shl32(S16(U32(ent, 0x144) + (uint32_t)S16(act, 0x1CA) * 2, 0), 16);
					uint32_t model = U32(act, 0x1BC);
					if (model != 0)
						for (int k = 0; k < 8; k++)
							U32(mat, 4 * k) = U32(model + 0x2C, 4 * k);
					x::MAG_022_sub_8DD770(mat);
					x::TransformVectorBy3x3Matrix(mat, off, off);
					i++;
					off += 0x10;
				} while (i < S8(act, 0x1D8));
			}
		}
		else if (mode == 0)
		{
			if (S8(act, 0x1D8) > 0)
			{
				int32_t i = 0;
				uint32_t rows = act + 0x0C;
				uint32_t v = act + 0x220;
				uint32_t spd = act + 0x174;
				do
				{
					if (act + 0x184 != 0 || act + 0x174 != 0)
					{
						S32(spd, 0) = add32(S32(spd, 0), S32(spd, 0x10));
						uint16_t hook = U16(act, 0x1DA);
						if (hook != 0)
							a_740F70(e3_sx16(hook), spd);
						uint32_t s = U32(spd, 0);
						U32(vec, 0) = 0;
						U32(vec, 8) = 0;
						U32(vec, 4) = 0u - s;
						x::TransformVectorBy3x3Matrix(rows, vec, mat);
						S32(v, -0xF4) = add32(S32(v, -0xF4), S32(mat, 0));
						S32(v, -0xF0) = add32(S32(v, -0xF0), S32(mat, 4));
						S32(v, -0xEC) = add32(S32(v, -0xEC), S32(mat, 8));
					}
					S32(v, 0) = add32(S32(v, 0), S32(v, 0x40));
					S32(v, 4) = add32(S32(v, 4), S32(v, 0x44));
					S32(v, 8) = add32(S32(v, 8), S32(v, 0x48));
					S32(v, -0x40) = add32(S32(v, -0x40), S32(v, 0));
					S32(v, -0x3C) = add32(S32(v, -0x3C), S32(v, 4));
					S32(v, -0x38) = add32(S32(v, -0x38), S32(v, 8));
					S32(v, -0x3C) = add32(S32(v, -0x3C), S32(act, 0x1DC));
					const uint32_t pos = v - 0x40;
					uint16_t hook = U16(act, 0x1DA);
					if (hook != 0)
						a_740FC0(e3_sx16(hook), pos);
					S32(v, -0xF4) = add32(S32(v, -0xF4), S32(pos, 0));
					spd += 4;
					S32(v, -0xF0) = add32(S32(v, -0xF0), S32(v, -0x3C));
					S32(v, -0xEC) = add32(S32(v, -0xEC), S32(v, -0x38));
					v += 0x10;
					i++;
					rows += 0x20;
				} while (i < S8(act, 0x1D8));
			}
		}

		if (U8(ent, 0x26) == 1)
		{
			int32_t sel = S8(ent, 0x1D);
			uint32_t model = U32(act, 0x1BC);
			switch ((uint32_t)sel)
			{
			case 0:
				x::MAG_022_sub_8DD770(mat);
				if (model != 0)
					for (int k = 0; k < 8; k++)
						U32(mat, 4 * k) = U32(model + 0x2C, 4 * k);
				if (U16(act, 0xD0) != 0) x::sub_8DD960(mat, e3_sx16(U16(act, 0xD0)));
				if (U16(act, 0xCC) != 0) x::sub_8DD7E0(mat, e3_sx16(U16(act, 0xCC)));
				if (U16(act, 0xCE) != 0) x::MAG_022_sub_8DD8A0(mat, e3_sx16(U16(act, 0xCE)));
				break;
			case 1:
				x::MAG_022_sub_8DD770(mat);
				if (model != 0)
					for (int k = 0; k < 8; k++)
						U32(mat, 4 * k) = U32(model + 0x2C, 4 * k);
				if (U16(act, 0xCE) != 0) x::MAG_022_sub_8DD8A0(mat, e3_sx16(U16(act, 0xCE)));
				if (U16(act, 0xCC) != 0) x::sub_8DD7E0(mat, e3_sx16(U16(act, 0xCC)));
				if (U16(act, 0xD0) != 0) x::sub_8DD960(mat, e3_sx16(U16(act, 0xD0)));
				break;
			case 2:
				x::UnpackRotationMatrix(MEM<uint32_t>(G(G_257F9B0)), mat);
				if (U16(act, 0xD0) != 0) x::sub_8DD960(mat, e3_sx16(U16(act, 0xD0)));
				if (U16(act, 0xCC) != 0) x::sub_8DD7E0(mat, e3_sx16(U16(act, 0xCC)));
				if (U16(act, 0xCE) != 0) x::MAG_022_sub_8DD8A0(mat, e3_sx16(U16(act, 0xCE)));
				break;
			case 3:
				x::UnpackRotationMatrix(MEM<uint32_t>(G(G_257F9B0)), mat);
				if (U16(act, 0xCE) != 0) x::MAG_022_sub_8DD8A0(mat, e3_sx16(U16(act, 0xCE)));
				if (U16(act, 0xCC) != 0) x::sub_8DD7E0(mat, e3_sx16(U16(act, 0xCC)));
				if (U16(act, 0xD0) != 0) x::sub_8DD960(mat, e3_sx16(U16(act, 0xD0)));
				break;
			default:
				break;
			}
			if (S8(act, 0x1D8) > 0)
			{
				int32_t i = 0;
				uint32_t o = out, src = act + 0x12C;
				do
				{
					x::TransformVectorBy3x3Matrix(mat, src, o);
					i++;
					o += 0x10;
					src += 0x10;
				} while (i < S8(act, 0x1D8));
			}
		}
		else
		{
			int32_t n = S8(act, 0x1D8);
			if (n > 0)
			{
				uint32_t dwords = ((uint32_t)n & 0xFFFFFFF) << 2;
				for (uint32_t k = 0; k < dwords; k++)
					U32(out, 4 * k) = U32(act + 0x12C, 4 * k);
			}
		}

		int32_t place = S8(ent, 0x22);
		if (place == 1)
		{
			uint32_t model = U32(act, 0x1BC);
			if (model != 0 && S8(act, 0x1D8) > 0)
			{
				const uint32_t mpos = model + 0x4C;
				int32_t i = 0;
				uint32_t o = out, dst = act + 0xDC;
				do
				{
					U32(dst, 0) = U32(mpos, 0);
					U32(dst, 4) = U32(mpos, 4);
					U32(dst, 8) = U32(mpos, 8);
					U32(dst, 0xC) = U32(mpos, 0xC);
					S32(dst, 0) = add32(S32(dst, 0), S32(o, 0));
					S32(dst, 4) = add32(S32(dst, 4), S32(o, 4));
					S32(dst, 8) = add32(S32(dst, 8), S32(o, 8));
					i++;
					o += 0x10;
					dst += 0x10;
				} while (i < S8(act, 0x1D8));
			}
		}
		else if (place == 0)
		{
			if (S8(act, 0x1D8) > 0)
			{
				int32_t i = 0;
				uint32_t o = out, dst = act + 0xDC;
				do
				{
					i++;
					U32(dst, 0) = U32(o, 0);
					U32(dst, 4) = U32(o, 4);
					U32(dst, 8) = U32(o, 8);
					U32(dst, 0xC) = U32(o, 0xC);
					o += 0x10;
					dst += 0x10;
				} while (i < S8(act, 0x1D8));
			}
		}

		if (U8(ent, 0x1B) == 1 && S8(act, 0x1D8) > 0)
		{
			int32_t i = 0;
			uint32_t y = act + 0xE0;
			do
			{
				U32(y, 0) = 0;
				i++;
				y += 0x10;
			} while (i < S8(act, 0x1D8));
		}
		return 0;
	}

	namespace
	{
		inline int32_t e4_div256(int32_t v) { return v / 256; }
		inline int32_t e4_div65536(int32_t v) { return v / 65536; }

		inline int32_t e4_decay(uint32_t p, int32_t t)
		{
			int32_t v = S32(p, 0);
			int32_t d = e4_div256(mul32(e4_div256(v), t));
			S32(p, 0) = sub32(v, d);
			return d;
		}

		void e4_world_to_view(uint32_t node, uint32_t src)
		{
			U32(node, 0xA0) = (uint32_t)e4_div65536(S32(src, 0));
			U32(node, 0xA4) = (uint32_t)e4_div65536(S32(src, 4));
			U32(node, 0xA8) = (uint32_t)e4_div65536(S32(src, 8));
			x::GTE_SetRotMatrix(U32(G(G_257F9B0), 0));
			x::GTE_LoadIRFromMatrixColumn(node + 0x8C);
			x::GTE_MVMVA_RotIR();
			x::GTE_StoreIRToMatrixColumn(node + 0xAC);
			x::GTE_LoadIRFromMatrixColumn(node + 0x8E);
			x::GTE_MVMVA_RotIR();
			x::GTE_StoreIRToMatrixColumn(node + 0xAE);
			x::GTE_LoadIRFromMatrixColumn(node + 0x90);
			x::GTE_MVMVA_RotIR();
			x::GTE_StoreIRToMatrixColumn(node + 0xB0);
			x::GTE_SetTransVector(U32(G(G_257F9B0), 0));
			x::GTE_LoadV0FromDwords(node + 0xA0);
			x::GTE_MVMVA_RotV0_Tr();
			x::GTE_ReadMAC123(node + 0xC0);
		}

		void e4_build_rot(uint32_t node, int32_t sub, uint32_t parent)
		{
			uint32_t m = node + 0x8C;
			switch ((uint32_t)sub)
			{
				case 0:
					if (parent != 0)
						ff8fx::copy<(32)>((void *)m, (void *)(parent + 0x2C));
					else
						x::MAG_022_sub_8DD770(m);
					if (U16(node, 0xD0) != 0) x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0xD0));
					if (U16(node, 0xCC) != 0) x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(node, 0xCC));
					if (U16(node, 0xCE) != 0) x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(node, 0xCE));
					break;
				case 1:
					if (parent != 0)
						ff8fx::copy<(32)>((void *)m, (void *)(parent + 0x2C));
					else
						x::MAG_022_sub_8DD770(m);
					if (U16(node, 0xCE) != 0) x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(node, 0xCE));
					if (U16(node, 0xCC) != 0) x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(node, 0xCC));
					if (U16(node, 0xD0) != 0) x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0xD0));
					break;
				case 2:
					x::UnpackRotationMatrix(U32(G(G_257F9B0), 0), m);
					if (U16(node, 0xD0) != 0) x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0xD0));
					if (U16(node, 0xCC) != 0) x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(node, 0xCC));
					if (U16(node, 0xCE) != 0) x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(node, 0xCE));
					break;
				case 3:
					x::UnpackRotationMatrix(U32(G(G_257F9B0), 0), m);
					if (U16(node, 0xCE) != 0) x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(node, 0xCE));
					if (U16(node, 0xCC) != 0) x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(node, 0xCC));
					if (U16(node, 0xD0) != 0) x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0xD0));
					break;
				default:
					break;
			}
		}

		inline void e4_load_offset(uint8_t *v, uint32_t desc)
		{
			*(int16_t *)(v + 0) = (int16_t)e4_div65536(S32(desc, 0));
			*(int16_t *)(v + 2) = (int16_t)e4_div65536(S32(desc, 4));
			*(int16_t *)(v + 4) = (int16_t)e4_div65536(S32(desc, 8));
		}
	}

	uint32_t __cdecl a_740F70(uint32_t a1, uint32_t a2)
	{
		int32_t t = (int32_t)a1;
		if (t > 0x10000)
			t = 0x10000;
		else if (t < 0)
			t = 0;
		return (uint32_t)e4_decay(a2, t);
	}

	uint32_t __cdecl a_740FC0(uint32_t a1, uint32_t a2)
	{
		int32_t t = (int32_t)a1;
		if (t > 0x10000)
			t = 0x10000;
		else if (t < 0)
			t = 0;
		e4_decay(a2, t);
		e4_decay(a2 + 4, t);
		return (uint32_t)e4_decay(a2 + 8, t);
	}

	uint32_t __cdecl a_741050(uint32_t a1, uint32_t a2)
	{
		int32_t mode = (int32_t)S8(a2, 0x16);
		if (mode == 0)
		{
			int8_t cnt = (int8_t)(U8(a1, 0x1D0) + 1);
			int8_t last = S8(a1, 0x1D1);
			U8(a1, 0x1D0) = (uint8_t)cnt;
			if (cnt > last)
			{
				U8(a1, 0x1D0) = 0;
				U8(a1, 0x1D2) = 1;
			}
			return a1;
		}
		if (mode == 1)
		{
			int8_t cnt = (int8_t)(U8(a1, 0x1D0) + 1);
			int8_t last = S8(a1, 0x1D1);
			U8(a1, 0x1D0) = (uint8_t)cnt;
			if (cnt > last)
				U8(a1, 0x1D0) = 0;
			return a1;
		}
		if (mode == 2)
		{
			int8_t cnt = (int8_t)(U8(a1, 0x1D0) + 1);
			int8_t loop_end = S8(a1, 0x1D4);
			U8(a1, 0x1D0) = (uint8_t)cnt;
			if (cnt > loop_end)
			{
				int8_t left = S8(a1, 0x1D5);
				if (left > 0)
				{
					U8(a1, 0x1D5) = (uint8_t)(left - 1);
					U8(a1, 0x1D0) = U8(a1, 0x1D3);
				}
			}
			if (S8(a1, 0x1D0) > S8(a1, 0x1D1))
			{
				U8(a1, 0x1D0) = 0;
				U8(a1, 0x1D2) = 1;
			}
			return a1;
		}
		return (uint32_t)(mode - 2);
	}

	uint32_t __cdecl a_741120(uint32_t a1, uint32_t a2)
	{
		uint32_t node = a1;
		uint32_t desc = a2;
		uint8_t mode = U8(desc, 0x16);
		if (mode == 4 || mode == 5)
		{
			int32_t frame = (int32_t)S16(node, 0x1CA);
			uint32_t tex = U8(U32(desc, 0x12C) + frame, 0);
			uint32_t dir = U32(G(G_258FB78), 0);
			U32(node, 0x170) = U32(U32(dir, 0x22C) + tex * 4, 0);
			if (mode == 5)
				callp(F(F_7418B0), node, desc);
		}

		int32_t rot_mode = (int32_t)S8(desc, 0x1B);
		uint32_t m = node + 0x8C;
		switch ((uint32_t)rot_mode)
		{
			case 0:
				x::UnpackRotationMatrix(U32(G(G_257F9B0), 0), m);
				if (U16(node, 0xD0) != 0)
					x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0xD0));
				break;
			case 1:
				x::MAG_022_sub_8DD770(m);
				x::sub_8DD7E0(m, 0x400);
				break;
			case 2:
			{
				uint32_t yaw = x::sub_8DD7B0(U32(G(G_257F9B0), 0));
				x::MAG_022_sub_8DD770(m);
				if ((uint16_t)yaw != 0)
					x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)(int16_t)yaw);
				break;
			}
			case 3:
			case 4:
			{
				int32_t sub = (int32_t)S8(desc, 0x1D);
				uint32_t parent = U32(node, 0x1BC);
				e4_build_rot(node, sub, parent);
				break;
			}
			default:
				break;
		}

		mode = U8(desc, 0x16);
		if (mode == 4 || mode == 5)
		{
			int32_t frame = (int32_t)S16(node, 0x1CA);
			U8(node, 0x16C) = U8(U32(desc, 0xF0) + frame, 0);
			U8(node, 0x16D) = U8(U32(desc, 0xF4) + frame, 0);
			U8(node, 0x16E) = U8(U32(desc, 0xF8) + frame, 0);
			U16(node, 0x1CE) = U16(U32(desc, 0xFC) + frame * 2, 0);
			U16(node, 0xD4) = U16(U32(desc, 0xE4) + frame * 2, 0);
			U16(node, 0xD6) = U16(U32(desc, 0xE8) + frame * 2, 0);
			uint16_t sz = U16(U32(desc, 0xEC) + frame * 2, 0);
			int32_t scale[4];
			scale[0] = (int32_t)S16(node, 0xD4);
			scale[1] = (int32_t)S16(node, 0xD6);
			U16(node, 0xD8) = sz;
			scale[2] = (int32_t)(int16_t)sz;
			scale[3] = 0;
			x::scale3DMatrix(m, P(scale));
		}

		uint8_t count = U8(node, 0x1D8);
		if (count == 1)
		{
			e4_world_to_view(node, node + 0xDC);
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
				e4_world_to_view(node, src);
				U16(dst, 0) = U16(node, 0xC0);
				U16(dst, 2) = U16(node, 0xC4);
				uint16_t z = (uint16_t)(U16(node, 0xC8) + U16(desc, 0x38));
				i++;
				src += 0x10;
				U16(dst, 4) = z;
				dst += 8;
			} while (i < (int32_t)S8(node, 0x1D8));
		}
		return 0;
	}

	uint32_t __cdecl a_7419C0(uint32_t a1, uint32_t a2)
	{
		int32_t kind = (int32_t)S8(a2, 0x1E);
		if (kind == 2)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) < 0)
			{
				a_741A40(a1, a2);
				return 1;
			}
		}
		else if (kind == 1)
		{
			U16(a1, 0x1C8) = (uint16_t)(U16(a1, 0x1C8) - 1);
			if (S16(a1, 0x1C8) >= 0)
				return 0;
			a_741A40(a1, a2);
			return 1;
		}
		else if (kind != 0)
		{
			return 0;
		}
		if (U8(a1, 0x1D2) == 1)
		{
			a_741A40(a1, a2);
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl a_741A40(uint32_t a1, uint32_t a2)
	{
		if (U8(a2, 0x30) == 1)
		{
			uint32_t desc_index = (a2 & 0xFFFF0000u) | (uint16_t)(int16_t)S8(a2, 0x31);
			uint32_t anchor = (uint16_t)(int16_t)S8(a1, 0x1D9);
			return a_741A70(a1, desc_index, anchor);
		}
		return a1;
	}

	uint32_t __cdecl a_741A70(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t pool = U32(G(G_258EC54), 0);
		int32_t idx = (int32_t)S16(G(G_2585AE0), 0);
		uint32_t slot = 0;
		int32_t tries = 0;
		do
		{
			if (U8(pool + (uint32_t)(idx * 27) * 4, 0x69) == 0)
			{
				slot = pool + (uint32_t)(idx * 27) * 4;
				x::MAG_007_sub_8DCC00(slot, 0x6C);
				U8(slot, 0x6A) = (uint8_t)a2;
				uint32_t dir = U32(G(G_258FB78), 0);
				U8(slot, 0x69) = 1;
				U16(dir, 0x14) = (uint16_t)(U16(dir, 0x14) + 1);
				U32(slot, 0x5C) = a1;
				U8(slot, 0x6B) = (uint8_t)a3;
				a_742CA0(slot, 0);
				break;
			}
			idx++;
			if (idx >= 0x27)
				idx = 0;
			tries++;
		} while (tries < 0x28);
		idx++;
		if (idx < 0x27)
			U16(G(G_2585AE0), 0) = (uint16_t)idx;
		else
			U16(G(G_2585AE0), 0) = 0;
		return slot;
	}

	uint32_t __cdecl a_741B60(uint32_t a1)
	{
		U16(a1, 0x64) = (uint16_t)(U16(a1, 0x64) - 1);
		if (S16(a1, 0x64) > 0)
			return 0;
		a_742CD0(a1);
		uint32_t dir = U32(G(G_258FB78), 0);
		U8(a1, 0x69) = 0;
		U16(dir, 0x14) = (uint16_t)(U16(dir, 0x14) - 1);
		return 0;
	}

	uint32_t __cdecl a_742290(uint32_t a1, uint32_t a2)
	{
		int32_t lo = (int32_t)a1;
		int32_t hi = (int32_t)a2;
		if (lo == hi)
			return (uint32_t)lo;
		int32_t range = sub32(hi, lo);
		int32_t r = (int32_t)x::CrtRand();
		int32_t rem = r % range;
		if (range < 0)
			return (uint32_t)sub32(lo, rem);
		return (uint32_t)add32(rem, lo);
	}

	uint32_t __cdecl a_7422C0(uint32_t a1, uint32_t a2)
	{
		int32_t lo = (int32_t)a1;
		int32_t hi = (int32_t)a2;
		if (lo == hi)
			return (uint32_t)lo;
		int32_t range = sub32(hi, lo);
		int32_t r1 = (int32_t)x::CrtRand();
		int32_t r2 = (int32_t)x::CrtRand();
		int32_t prod = mul32(r2, r1);
		int32_t rem = prod % range;
		if (range < 0)
			return (uint32_t)sub32(lo, rem);
		return (uint32_t)add32(rem, lo);
	}

	uint32_t __cdecl a_742300(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t r = a_7422C0(U32(a2, 0), U32(a3, 0));
		uint32_t hi1 = U32(a3, 4);
		U32(a1, 0) = r;
		r = a_7422C0(U32(a2, 4), hi1);
		uint32_t hi2 = U32(a3, 8);
		uint32_t lo2 = U32(a2, 8);
		U32(a1, 4) = r;
		r = a_7422C0(lo2, hi2);
		U32(a1, 8) = r;
		return r;
	}

	uint32_t __cdecl a_742350(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t r = a_7423A0(U16(a2, 0), U16(a3, 0));
		uint16_t rng1 = U16(a3, 2);
		U16(a1, 0) = (uint16_t)r;
		r = a_7423A0(U16(a2, 2), rng1);
		uint16_t rng2 = U16(a3, 4);
		uint16_t base2 = U16(a2, 4);
		U16(a1, 2) = (uint16_t)r;
		r = a_7423A0(base2, rng2);
		U16(a1, 4) = (uint16_t)r;
		return r;
	}

	uint32_t __cdecl a_7423A0(uint32_t a1, uint32_t a2)
	{
		int32_t spread = (int32_t)(int16_t)a2;
		int32_t half = spread / 2;
		uint32_t r = a_742290((uint32_t)(-half), (uint32_t)half);
		uint32_t v = (uint32_t)add32((int32_t)r, (int32_t)(int16_t)a1);
		if ((int32_t)v < 0)
		{
			uint32_t k = ((0x1000u - v) >> 12) << 12;
			return v + k;
		}
		if ((int32_t)v < 0x1000)
			return v;
		uint32_t k = (0u - (v >> 12)) << 12;
		return v + k;
	}

	uint32_t __cdecl a_7424B0(uint32_t a1)
	{
		return ((a1 & 0xFFFF0000u) | U16(a1, 8)) - 1;
	}

	uint32_t __cdecl a_7424C0(uint32_t a1)
	{
		uint32_t dir = U32(G(G_258FB78), 0);
		uint32_t table = U32(dir, 0x224);
		int32_t di = (int32_t)S8(a1, 0x6A);
		uint32_t owner = U32(a1, 0x5C);
		uint32_t desc = U32(table + di * 4, 0);
		if (owner == 0)
			return 0;
		if (U8(desc, 0x35) == 1)
		{
			U32(a1, 0x4C) = (uint32_t)(add32(S32(owner, 0x11C), S32(owner, 0xDC)) / 2);
			U32(a1, 0x50) = (uint32_t)(add32(S32(owner, 0x120), S32(owner, 0xE0)) / 2);
			U32(a1, 0x54) = (uint32_t)(add32(S32(owner, 0x124), S32(owner, 0xE4)) / 2);
		}
		else
		{
			uint32_t s = owner + 0xDC;
			U32(a1, 0x4C) = U32(s, 0);
			U32(a1, 0x50) = U32(s, 4);
			U32(a1, 0x54) = U32(s, 8);
			U32(a1, 0x58) = U32(s, 0xC);
		}
		if (U8(desc, 0x1C) != 3)
			return 0;
		ff8fx::copy<(32)>((void *)(a1 + 0x2C), (void *)(owner + 0x8C));
		uint8_t local[0x10] = {};
		e4_load_offset(local, desc);
		x::GTE_SetRotMatrix(a1 + 0x2C);
		x::GTE_LoadV0(P(local));
		x::GTE_MVMVA_RotV0();
		x::GTE_StoreIR123(P(local + 8));
		int32_t ir1 = *(int16_t *)(local + 8);
		int32_t ir2 = *(int16_t *)(local + 0xA);
		int32_t ir3 = *(int16_t *)(local + 0xC);
		uint32_t px = U32(a1, 0x4C);
		U32(a1, 0x50) = (uint32_t)add32(S32(a1, 0x50), shl32(ir2, 16));
		U32(a1, 0x4C) = (uint32_t)add32((int32_t)px, shl32(ir1, 16));
		U32(a1, 0x54) = (uint32_t)add32(S32(a1, 0x54), shl32(ir3, 16));
		return 0;
	}

	uint32_t __cdecl a_742CA0(uint32_t a1, uint32_t a2)
	{
		U16(a1, 8) = (uint16_t)a2;
		uint32_t dir = U32(G(G_258FB78), 0);
		if (U32(dir, 0x2C) == 0)
		{
			U32(dir, 0x2C) = a1;
			U32(dir, 0x30) = a1;
			return a1;
		}
		uint32_t tail = U32(dir, 0x30);
		U32(dir, 0x30) = a1;
		U32(a1, 0) = tail;
		U32(tail, 4) = a1;
		return a1;
	}

	uint32_t __cdecl a_742CD0(uint32_t a1)
	{
		uint32_t dir = U32(G(G_258FB78), 0);
		uint32_t prev = U32(a1, 0);
		uint32_t next = U32(a1, 4);
		if (prev == 0)
			U32(dir, 0x2C) = next;
		else
			U32(prev, 4) = next;
		if (next == 0)
			U32(dir, 0x30) = prev;
		else
			U32(next, 0) = prev;
		return prev;
	}

	uint32_t __cdecl a_742D00(uint32_t a1)
	{
		uint32_t dir = U32(G(G_258FB78), 0);
		uint32_t table = U32(dir, 0x224);
		int32_t di = (int32_t)S8(a1, 0x6A);
		uint32_t owner = U32(a1, 0x5C);
		uint32_t desc = U32(table + di * 4, 0);
		U8(a1, 0x68) = U8(desc, 0x11);
		uint32_t m = a1 + 0x2C;
		switch ((uint32_t)(int32_t)S8(desc, 0x1C))
		{
			case 0:
				x::MAG_022_sub_8DD770(m);
				break;
			case 1:
			{
				x::MAG_022_sub_8DD770(m);
				int32_t k = shl32((int32_t)S8(a1, 0x6B), 4);
				uint32_t d = U32(G(G_258FB78), 0);
				uint32_t e = (uint32_t)k + d;
				int32_t dx = sub32(S32(d, 0x1C4), S32(e, 0x54));
				int32_t dz = sub32(S32(d, 0x1CC), S32(e, 0x5C));
				uint32_t ang = x::CartesianToGameAngle((uint32_t)dx, (uint32_t)dz);
				if (ang != 0)
					x::MAG_022_sub_8DD8A0(m, ang);
				break;
			}
			case 2:
			{
				x::MAG_022_sub_8DD770(m);
				int32_t k = shl32((int32_t)S8(a1, 0x6B), 4);
				uint32_t d = U32(G(G_258FB78), 0);
				int32_t dx = sub32(S32((uint32_t)k + d, 0x54), S32(d, 0x1C4));
				int32_t dz = sub32(S32((uint32_t)k + d, 0x5C), S32(d, 0x1CC));
				uint32_t ang = x::CartesianToGameAngle((uint32_t)dx, (uint32_t)dz);
				if (ang != 0)
					x::MAG_022_sub_8DD8A0(m, ang);
				break;
			}
			case 3:
				ff8fx::copy<(32)>((void *)m, (void *)(owner + 0x8C));
				break;
			case 4:
			{
				x::MAG_022_sub_8DD770(m);
				uint32_t d = U32(G(G_258FB78), 0);
				int32_t ent = (int32_t)S16(d, 0x1E);
				int32_t yaw = (int32_t)S16(game_addr(game::unk_1D972CE) + (uint32_t)(ent * 39) * 4, 0);
				if (yaw != 0)
					x::MAG_022_sub_8DD8A0(m, (uint32_t)yaw);
				break;
			}
			default:
				break;
		}
		switch ((uint32_t)(int32_t)S8(a1, 0x68))
		{
			case 0: a_742EB0(a1); break;
			case 1: a_742FF0(a1); break;
			case 2:
			case 3: a_7424C0(a1); break;
			case 4: callp(F(F_742610), a1); break;
			default: break;
		}
		U16(a1, 0x64) = (uint16_t)(int16_t)S8(desc, 0x12);
		return 0;
	}

	uint32_t __cdecl a_742EB0(uint32_t a1)
	{
		uint32_t dir = U32(G(G_258FB78), 0);
		uint32_t table = U32(dir, 0x224);
		uint32_t desc = U32(table + (int32_t)S8(a1, 0x6A) * 4, 0);
		uint32_t base = 0;
		bool has = true;
		switch ((uint32_t)(int32_t)S8(desc, 0x25))
		{
			case 0: base = 0x94; break;
			case 1: base = 0xD4; break;
			case 2: base = 0x114; break;
			case 3: base = 0x154; break;
			default: has = false; break;
		}
		if (has)
		{
			uint32_t src = (uint32_t)shl32((int32_t)S8(a1, 0x6B), 4) + dir + base;
			U32(a1, 0x4C) = U32(src, 0);
			U32(a1, 0x50) = U32(src, 4);
			uint32_t s8 = U32(src, 8);
			uint32_t sc = U32(src, 0xC);
			U32(a1, 0x54) = s8;
			U32(a1, 0x58) = sc;
		}
		uint8_t local[0x30] = {};
		e4_load_offset(local, desc);
		ff8fx::copy<(32)>(local + 0x10, (void *)(a1 + 0x2C));
		x::GTE_SetRotMatrix(P(local + 0x10));
		x::GTE_LoadV0(P(local));
		x::GTE_MVMVA_RotV0();
		x::GTE_StoreIR123(P(local + 8));
		int32_t ir1 = *(int16_t *)(local + 8);
		int32_t ir2 = *(int16_t *)(local + 0xA);
		int32_t ir3 = *(int16_t *)(local + 0xC);
		U32(a1, 0x4C) = (uint32_t)add32(S32(a1, 0x4C), shl32(ir1, 16));
		U32(a1, 0x50) = (uint32_t)add32(S32(a1, 0x50), shl32(ir2, 16));
		U32(a1, 0x54) = (uint32_t)add32(S32(a1, 0x54), shl32(ir3, 16));
		return 0;
	}

	uint32_t __cdecl a_742FF0(uint32_t a1)
	{
		uint32_t dir = U32(G(G_258FB78), 0);
		uint32_t table = U32(dir, 0x224);
		uint32_t desc = U32(table + (int32_t)S8(a1, 0x6A) * 4, 0);
		uint32_t base = 0;
		bool has = true;
		switch ((uint32_t)(int32_t)S8(desc, 0x25))
		{
			case 0: base = 0x1D4; break;
			case 1: base = 0x1E4; break;
			case 2: base = 0x1F4; break;
			case 3: base = 0x204; break;
			default: has = false; break;
		}
		if (has)
		{
			uint32_t src = dir + base;
			U32(a1, 0x4C) = U32(src, 0);
			U32(a1, 0x50) = U32(src, 4);
			uint32_t s8 = U32(src, 8);
			uint32_t sc = U32(src, 0xC);
			U32(a1, 0x54) = s8;
			U32(a1, 0x58) = sc;
		}
		uint8_t local[0x10] = {};
		e4_load_offset(local, desc);
		x::GTE_SetRotMatrix(a1 + 0x2C);
		x::GTE_LoadV0(P(local));
		x::GTE_MVMVA_RotV0();
		x::GTE_StoreIR123(P(local + 8));
		int32_t ir1 = *(int16_t *)(local + 8);
		int32_t ir2 = *(int16_t *)(local + 0xA);
		int32_t ir3 = *(int16_t *)(local + 0xC);
		U32(a1, 0x4C) = (uint32_t)add32(S32(a1, 0x4C), shl32(ir1, 16));
		U32(a1, 0x50) = (uint32_t)add32(S32(a1, 0x50), shl32(ir2, 16));
		U32(a1, 0x54) = (uint32_t)add32(S32(a1, 0x54), shl32(ir3, 16));
		return 0;
	}

	uint32_t __cdecl a_743100(uint32_t a1, uint32_t a2)
	{
		const uint32_t obj = a1;
		const uint32_t desc = a2;
		if (U8(desc, 0x28) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2C);
			a_741A70(obj, id, variant);
		}
		if (U8(desc, 0x29) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2D);
			a_741A70(obj, id, variant);
		}
		if (U8(desc, 0x2A) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2E);
			a_741A70(obj, id, variant);
		}
		if (U8(desc, 0x2B) == 1)
		{
			uint32_t variant = (uint16_t)(int16_t)S8(obj, 0x1D9);
			uint32_t id = (uint16_t)(int16_t)S8(desc, 0x2F);
			a_741A70(obj, id, variant);
		}
		return 0;
	}

	uint32_t __cdecl a_743190(void)
	{
		uint32_t dir = MEM<uint32_t>(G(G_258FB78));
		for (int32_t slot = 0; slot < 0x10; slot++)
		{
			uint32_t table = U32(dir, 0x224);
			uint32_t entry = U32(table, slot * 4);
			if (entry == 0)
				continue;
			uint32_t bit = (uint32_t)shl32(1, slot);
			uint32_t mask = U16(dir, 0x22);
			if (bit & mask)
				continue;
			uint8_t kind = U8(entry, 0x11);
			if (kind != 0 && kind != 1 && kind != 4)
				continue;
			int16_t group = (int16_t)S8(entry, 0x13);
			if (group != S16(dir, 0x18))
				continue;
			if (U8(entry, 0x37) == 1)
			{
				int32_t n = 0;
				if (S16(dir, 0x1C) <= 0)
					continue;
				do
				{
					a_741A70(0, (uint32_t)slot, (uint32_t)n);
					dir = MEM<uint32_t>(G(G_258FB78));
					n++;
				} while (n < (int32_t)S16(dir, 0x1C));
			}
			else
			{
				a_741A70(0, (uint32_t)slot, 0);
				dir = MEM<uint32_t>(G(G_258FB78));
			}
		}
		return 0;
	}

	uint32_t __cdecl a_7435E0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t node = a1;
		const uint32_t data = U32(node, 0);
		U16(node, 0x26) = U16(data, 8);
		uint32_t seq = U16(node, 4);
		uint32_t offs_tab = data + 10;
		uint32_t offs = U16(offs_tab, seq * 2);
		U16(node, 0x28) = U16(offs_tab, seq * 2 + 2);
		uint32_t list = offs + data;
		int32_t count = S32(list, 0);
		list += 4;
		S32(node, 0x30) = count;
		U32(node, 0x2C) = list;
		if (count < 0)
		{
			count &= 0x7FFFFFFF;
			U8(node, 0x25) |= 1;
			S32(node, 0x30) = count;
		}
		const uint32_t cam = node + 0x84;
		U16(node, 0x60) = 0;
		U16(node, 0x58) = 0;
		U16(node, 0x50) = 0;
		U16(node, 0x48) = 0;
		x::CopyCameraStateToBuffer(cam);
		uint16_t fl = U16(node, 0x24);
		if (fl & 1)
		{
			int32_t roll = S16(game_addr(game::g_BattleCam_Roll), 0);
			x::ApplyZRotation((uint32_t)sub32(S32(node, 8), roll), cam);
		}
		else if (!(fl & 0x200))
		{
			int32_t roll = S16(game_addr(game::g_BattleCam_Roll), 0);
			x::ApplyZRotation((uint32_t)sub32(0, roll), cam);
		}
		if (U8(node, 0x24) & 2)
		{
			U32(node, 0x14) = 0x1000;
			x::ScaleMatrix3x3(cam, node + 0xC);
		}
		fl = U16(node, 0x24);
		if (!(fl & 4))
			U32(node, 0x1C) = 0x808080;
		if (!(fl & 0x10))
			U8(node, 0x20) = U8(data, 4);
		if (!(fl & 0x20))
			U8(node, 0x21) = U8(data, 5);
		if (!(fl & 0x40))
			U8(node, 0x22) = U8(data, 6);
		if (!(fl & 0x80))
			U8(node, 0x23) = U8(data, 7);
		if (fl & 8)
			U32(node, 0xB0) = 1;
		else
			U32(node, 0xB0) = 0;
		U32(node, 0xAC) = 0;
		U32(node, 0xA4) = 0x1000;
		U32(node, 0xA8) = 0;
		U32(node, 0x3C) = 0xFFFFFFFF;
		return a_743720(node, a2, a3, a4);
	}

	uint32_t __cdecl a_743720(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t node = a1;
		uint32_t pkt = a4;
		const int32_t count = S32(node, 0x30);
		uint32_t ent = U32(node, 0x2C);
		uint32_t ot_slot = a3;
		uint32_t a4slot = a4;
		if (count <= 0)
			return pkt;
		for (int32_t i = 0; i < count; i++, ent += 0x14)
		{
			const int32_t flags = S16(ent, 0xE);
			U16(node, 0x72) = 0;
			U16(node, 0x70) = 0;
			U16(node, 0x6E) = 0;
			U16(node, 0x6A) = 0;
			U16(node, 0x68) = 0;
			U16(node, 0x66) = 0;
			U16(node, 0x74) = 0x1000;
			if (flags & 0xC00)
			{
				int32_t ang = S16(ent, 0xC);
				if (ang == 0)
				{
					U32(node, 0xA4) = 0x1000;
					U32(node, 0xA8) = 0;
					U32(node, 0xAC) = 0;
				}
				else if (ang != S32(node, 0xAC))
				{
					uint32_t c = x::computeCosine((uint32_t)ang);
					U32(node, 0xA4) = c;
					uint32_t s = x::computeSin((uint32_t)ang);
					U32(node, 0xA8) = s;
					S32(node, 0xAC) = ang;
				}
				int32_t sx = S16(ent, 0x10);
				int32_t c = S32(node, 0xA4);
				U16(node, 0x64) = (uint16_t)(mul32(c, sx) >> 12);
				int32_t s = S32(node, 0xA8);
				int32_t m10 = mul32(s, sx);
				int32_t sy = S16(ent, 0x12);
				int32_t m01 = mul32(s, sy);
				int32_t m11 = mul32(c, sy);
				m01 = sub32(0, m01);
				m10 >>= 12;
				m01 >>= 12;
				m11 >>= 12;
				U16(node, 0x6A) = (uint16_t)m10;
				U16(node, 0x66) = (uint16_t)m01;
				U16(node, 0x6C) = (uint16_t)m11;
			}
			else
			{
				U16(node, 0x6C) = 0x1000;
				U16(node, 0x64) = 0x1000;
			}
			const uint32_t w = U8(ent, 0);
			const uint32_t h = U8(ent, 2);
			a4slot = w;
			const uint32_t hl = h;
			uint32_t w8 = w << 3;
			U16(node, 0x5C) = (uint16_t)w8;
			U16(node, 0x54) = (uint16_t)(0u - w8);
			U16(node, 0x44) = (uint16_t)(0u - w8);
			uint32_t h8 = h << 3;
			U16(node, 0x4C) = (uint16_t)w8;
			U16(node, 0x4E) = (uint16_t)(0u - h8);
			U16(node, 0x46) = (uint16_t)(0u - h8);
			int32_t cx = add32(shl32(S16(ent, 8), 4), (int32_t)w8);
			U16(node, 0x5E) = (uint16_t)h8;
			int32_t cy = add32(shl32(S16(ent, 0xA), 4), (int32_t)h8);
			U16(node, 0x56) = (uint16_t)h8;
			S32(node, 0x7C) = cy;
			S32(node, 0x78) = cx;
			U32(node, 0x80) = 0;
			x::GTE_SetRotMatrix(node + 0x84);
			const uint32_t rot = node + 0x64;
			x::GTE_LoadIRFromMatrixColumn(rot);
			x::GTE_MVMVA_RotIR();
			uint32_t d0 = U32(ent, 0);
			uint32_t d1 = U32(ent, 4);
			U32(pkt, 0) = 0x9000000;
			U32(pkt, 4) = d0;
			U32(pkt, 0xC) = d1;
			x::GTE_StoreIRToMatrixColumn(rot);
			x::GTE_LoadIRFromMatrixColumn(node + 0x66);
			x::GTE_MVMVA_RotIR();
			{
				uint8_t in = U8(pkt, 5);
				uint32_t cr = U8(node, 0x1C);
				U8(pkt, 6) = in;
				U8(pkt, 4) = (uint8_t)(((uint32_t)in * cr) >> 7);
			}
			x::GTE_StoreIRToMatrixColumn(node + 0x66);
			x::GTE_LoadIRFromMatrixColumn(node + 0x68);
			x::GTE_MVMVA_RotIR();
			{
				uint32_t in = U8(pkt, 5);
				uint32_t cg = U8(node, 0x1D);
				uint32_t g = in * cg;
				uint8_t u = U8(ent, 4);
				U8(pkt, 5) = (uint8_t)(g >> 7);
				U8(pkt, 0x1C) = u;
			}
			x::GTE_StoreIRToMatrixColumn(node + 0x68);
			x::GTE_SetTransVector(node + 0x84);
			x::GTE_LoadV0FromDwords(node + 0x78);
			x::GTE_MVMVA_RotV0_Tr();
			{
				uint32_t cb = U8(node, 0x1E);
				uint32_t in = U8(pkt, 6);
				uint32_t b = cb * in;
				uint8_t v = U8(ent, 5);
				U8(pkt, 6) = (uint8_t)(b >> 7);
				U8(pkt, 0x15) = v;
			}
			x::GTE_ReadMAC123(node + 0x78);
			x::GTE_SetRotMatrix(rot);
			x::GTE_SetTransVector(rot);
			x::GTE_LoadV012(node + 0x44, node + 0x4C, node + 0x54);
			x::GTE_RTPT();
			{
				int32_t u = add32(sub32((int32_t)U8(ent, 4), S32(node, 0xB0)), (int32_t)a4slot);
				if (u >= 0x100)
					u = 0xFF;
				U8(pkt, 0x24) = (uint8_t)u;
				U8(pkt, 0x14) = (uint8_t)u;
			}
			{
				uint32_t pal_idx = ((uint32_t)flags & 0xFFFF) >> 12;
				int16_t pal = (int16_t)S8(node + pal_idx, 0x20);
				U16(pkt, 0xE) = (uint16_t)(U16(pkt, 0xE) + (uint16_t)(pal << 6));
			}
			x::GTE_ReadFLAG(node + 0x40);
			if (U32(node, 0x40) & 0x60000)
				continue;
			U16(pkt, 0x16) = (uint16_t)(flags & 0x1FF);
			x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
			x::GTE_LoadV0(node + 0x5C);
			x::GTE_RTPS();
			{
				int32_t v = add32(sub32((int32_t)U8(ent, 5), S32(node, 0xB0)), (int32_t)hl);
				if (v >= 0x100)
					v = 0xFF;
				U8(pkt, 0x25) = (uint8_t)v;
				U8(pkt, 0x1D) = (uint8_t)v;
			}
			x::GTE_ReadSXY2(pkt + 0x20);
			const uint32_t ioff = (uint32_t)i * 4;
			int32_t z = S32(node, 0x3C);
			if (z >= 0)
			{
				z = add32(z, S16(node, 0xB4));
				S32(node, 0x3C) = z;
				if (z < 0)
					S32(node, 0x3C) = 0;
				uint32_t k3 = MEM<uint32_t>(game_addr(game::GTE_SZ3)) + ioff;
				uint32_t k2 = MEM<uint32_t>(game_addr(game::GTE_SZ2)) + ioff;
				uint32_t k1 = MEM<uint32_t>(game_addr(game::GTE_SZ1)) + ioff;
				uint32_t k0 = MEM<uint32_t>(game_addr(game::GTE_SZ0)) + ioff;
				x::SSIGPU_InsertPrimDepthKeys(a2 + ot_slot * 4, pkt, k0, k1, k2, k3);
				pkt += 0x28;
			}
			else
			{
				x::GTE_AVSZ4();
				x::GTE_ReadOTZ(node + 0x3C);
				z = add32(S32(node, 0x3C), S16(node, 0xB4));
				S32(node, 0x3C) = z;
				if (z < 0)
					S32(node, 0x3C) = 0;
				int32_t depth = S32(node, 0x3C) >> (ot_slot & 31);
				ot_slot = (uint32_t)depth;
				if (U8(node, 0x25) & 1)
				{
					uint32_t mode = pkt + 0x28;
					uint32_t ot = a2 + (uint32_t)depth * 4;
					U32(mode, 0) = 0x1000000;
					U32(mode, 4) = 0xE1000220;
					a4slot = ot;
					x::SSIGPU_InsertPrimAutoDepth(ot, mode);
					uint32_t k3 = MEM<uint32_t>(game_addr(game::GTE_SZ3)) + ioff;
					uint32_t k2 = MEM<uint32_t>(game_addr(game::GTE_SZ2)) + ioff;
					uint32_t k1 = MEM<uint32_t>(game_addr(game::GTE_SZ1)) + ioff;
					uint32_t k0 = MEM<uint32_t>(game_addr(game::GTE_SZ0)) + ioff;
					x::SSIGPU_InsertPrimDepthKeys(a4slot, pkt, k0, k1, k2, k3);
					pkt = mode + 0xC;
				}
				else
				{
					uint32_t k3 = MEM<uint32_t>(game_addr(game::GTE_SZ3)) + ioff;
					uint32_t k2 = MEM<uint32_t>(game_addr(game::GTE_SZ2)) + ioff;
					uint32_t k1 = MEM<uint32_t>(game_addr(game::GTE_SZ1)) + ioff;
					uint32_t k0 = MEM<uint32_t>(game_addr(game::GTE_SZ0)) + ioff;
					x::SSIGPU_InsertPrimDepthKeys(a2 + (uint32_t)depth * 4, pkt, k0, k1, k2, k3);
					pkt += 0x28;
				}
			}
		}
		return pkt;
	}

	uint32_t __cdecl a_743C00(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl a_743C20(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x50) = (uint16_t)(U16(node, 0x50) + 1);
		int16_t cur = S16(node, 0x50);
		int16_t lim = S16(node, 0x52);
		if (cur > lim)
		{
			U8(node, 0x26) |= 4;
			S16(node, 0x50) = lim;
			return 1;
		}
		return 0;
	}

	uint32_t __cdecl a_7458E0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (U8(node, 0x26) & 4)
			return 0;
		uint32_t mbuf[8] = {};
		const uint32_t m = P(mbuf);
		x::MAG_022_sub_8DD770(m);
		x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(node, 0x46));
		x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(node, 0x44));
		x::sub_8DD960(m, (uint32_t)(int32_t)S16(node, 0x48));
		int32_t tx = S16(node, 0x1C);
		int32_t ty = S16(node, 0x1E);
		int32_t tz = S16(node, 0x20);
		S32(m, 0x14) = tx;
		S32(m, 0x18) = ty;
		S32(m, 0x1C) = tz;
		x::scale3DMatrix(m, node + 0x30);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), m, m);
		x::GTE_SetRotMatrix_W(m);
		x::GTE_SetTransVector_W(m);
		uint32_t prm = x::Field_Alloc(0x58);
		uint32_t model = U32(node, 0x4C);
		uint32_t v40 = U32(node, 0x40);
		U32(prm, 0) = model;
		U32(prm, 8) = v40;
		int32_t v50 = S16(node, 0x50);
		uint32_t cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		S32(prm, 0xC) = v50;
		uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U32(prm, 0x1C) = 0xF0;
		uint32_t next = x::Effect_RenderPrimModel(prm, ot, 2, cursor);
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = next;
		x::Field_Free(0x58);
		return 0;
	}

	uint32_t __cdecl a_746980(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x1C) = 0x600;
		uint32_t p = game_addr(game::unk_1D989BA);
		for (int k = 0; k < 4; k++)
		{
			U16(p, -0x28) = 0x600;
			U8(p, 0) = 0;
			U8(p, -1) = 0;
			U8(p, -2) = 0;
			p += 0x2C;
		}
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl a_746A10(void)
	{
		uint32_t p = game_addr(game::unk_1D98991);
		for (int k = 0; k < 4; k++)
		{
			uint8_t b = U8(p, 0);
			p += 0x2C;
			U8(p, -0x2C) = (uint8_t)(b & 0xFD);
		}
		return 0;
	}

	uint32_t __cdecl a_746AE0(void)
	{
		uint32_t p = game_addr(game::unk_1D98991);
		for (int k = 0; k < 4; k++)
		{
			uint8_t b = U8(p, 0);
			p += 0x2C;
			uint8_t bit = (uint8_t)((b >> 1) & 2);
			uint8_t keep = (uint8_t)(b & 0xFD);
			U8(p, -0x2C) = (uint8_t)(bit | keep);
		}
		return 0;
	}

	uint32_t __cdecl a_746C10(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t mdl = a1 + 0x30;
		const uint32_t ws = x::Field_Alloc(0x4C);
		const uint32_t mat = mdl + 0x40;
		const uint32_t skel = mdl + 0x60;
		x::ComposeZYXRotationMatrix(mdl + 0xC, mat);
		int32_t px = S16(mdl, 0x1C);
		int32_t py = S16(mdl, 0x1E);
		int32_t pz = S16(mdl, 0x20);
		S32(mdl, 0x54) = px;
		S32(mdl, 0x58) = py;
		S32(mdl, 0x5C) = pz;
		x::scale3DMatrix(mat, a1 + 0x114);
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), mat, ws);
		x::BS_ComputeBonesWorldMatrices(skel, ws);
		uint32_t v28 = U32(mdl, 0x28);
		U32(ws, 0x24) = a2;
		uint8_t bright = U8(mdl, 7);
		U8(ws, 0x4A) = bright;
		U8(ws, 0x49) = bright;
		U8(ws, 0x48) = bright;
		uint32_t v7c = U32(mdl, 0x7C);
		U32(ws, 0x3C) = v28;
		U16(ws, 0x34) = 0;
		U16(ws, 0x36) = 0;
		U16(ws, 0x44) = 0;
		uint32_t g969a8 = MEM<uint32_t>(G(G_1D969A8));
		U32(ws, 0x40) = v7c;
		uint32_t otbase = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase));
		U16(ws, 0x38) = 0x140;
		U16(ws, 0x3A) = 0xD8;
		U32(ws, 0x30) = g969a8;
		uint32_t geom = U32(skel, 4);
		uint32_t cursor = x::RenderGeometry(geom, ws + 0x20, otbase + 0x44, 4, a3);
		x::BattleModel_BuildBoneMatricesFromPose(skel);
		x::sub_5086F0(mdl);
		uint16_t shrink = U16(a1, 0x13E);
		if (shrink != 0x1000)
		{
			int32_t b3a = S16(mdl, 0x3A);
			int32_t b34 = S16(mdl, 0x34);
			int32_t midA = (b3a + b34) / 2;
			int32_t b38 = S16(mdl, 0x38);
			int32_t midB = (S16(mdl, 0x3E) + b38) / 2;
			int32_t cA = (int16_t)midA;
			int32_t s = (int16_t)shrink;
			S16(mdl, 0x34) = (int16_t)add32(mul32(b34 - cA, s) / 4096, cA);
			S16(mdl, 0x3A) = (int16_t)add32(mul32(b3a - cA, s) / 4096, cA);
			int32_t cB = (int16_t)midB;
			S16(mdl, 0x38) = (int16_t)add32(mul32(S16(mdl, 0x38) - cB, s) / 4096, cB);
			S16(mdl, 0x3E) = (int16_t)add32(mul32(S16(mdl, 0x3E) - cB, s) / 4096, cB);
		}
		if (!(U8(mdl, 0) & 0x20))
		{
			uint32_t ot2 = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4064;
			cursor = x::sub_5088A0(mdl, ot2, 0x10, cursor);
		}
		x::Field_Free(0x4C);
		return cursor;
	}

	uint32_t __cdecl a_747400(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		if (a_73B7E0(6) != 0)
		{
			uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 5;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl a_747440(uint32_t a1)
	{
		uint32_t d = MEM<uint32_t>(G(G_1533010));
		if (U16(d, 0x48) != 1)
			return 0;
		const uint32_t node = a1;
		int32_t n = 0;
		int32_t act = S8(node, 0x2A);
		uint32_t ctx = U32(node, 0xC);
		uint32_t entry = U32(ctx, 4) + (uint32_t)(act * 5) * 4;
		if (U8(entry, 0x10) != 0)
		{
			uint32_t off = 0;
			do
			{
				uint32_t rec = U32(entry, 8) + off;
				x::ApplyActionResultToTarget(rec);
				act = S8(node, 0x2A);
				n++;
				off += 0x18;
				ctx = U32(node, 0xC);
				entry = U32(ctx, 4) + (uint32_t)(act * 5) * 4;
			} while (n < (int32_t)U8(entry, 0x10));
		}
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl a_7474B0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (U8(node, 0x28) != 0)
		{
			uint32_t d = MEM<uint32_t>(G(G_1533010));
			if (U16(d, 0x4C) != 1)
				return 0;
		}
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl a_7474D0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t root = U32(node, 0x10);
		U8(node, 0x26) |= 1;
		U8(root, 0x63) = 0;
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl a_747500(uint32_t a1)
	{
		const uint32_t node = a1;
		if (U8(node, 0x63) != 0)
			return 0;
		uint8_t idx = U8(node, 0x2A);
		if ((int16_t)(int8_t)idx < S16(node, 0x58))
		{
			uint8_t loops = U8(node, 0x2E);
			U8(node, 0x2A) = (uint8_t)(idx + 1);
			uint8_t st = U8(node, 0x29);
			U8(node, 0x2E) = (uint8_t)(loops + 1);
			U8(node, 0x29) = (uint8_t)(st - 1);
			return 0;
		}
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl a_747550(uint32_t a1)
	{
		const uint32_t node = a1;
		if (U16(node, 0x5E) == 0)
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl a_747560(uint32_t a1)
	{
		uint32_t g = MEM<uint32_t>(G(G_257F8A0));
		const uint32_t node = a1;
		x::sub_508630(g, node + 0x62);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl a_747590(uint32_t a1)
	{
		const uint32_t node = a1;
		if (U8(node, 0x62) != 0)
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl a_7475A0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint8_t st = U8(node, 0x29);
		U8(node, 0x26) |= 1;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	namespace
	{
		inline void e6_copy16(uint32_t dst, uint32_t src)
		{
			U32(dst, 0) = U32(src, 0);
			U32(dst, 4) = U32(src, 4);
			U32(dst, 8) = U32(src, 8);
			U32(dst, 12) = U32(src, 12);
		}

		inline void e6_rotate_zxy(uint32_t m, uint32_t ang)
		{
			x::MAG_022_sub_8DD770(m);
			if (U16(ang, 4) != 0) x::sub_8DD960(m, (uint32_t)(int32_t)S16(ang, 4));
			if (U16(ang, 0) != 0) x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(ang, 0));
			if (U16(ang, 2) != 0) x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(ang, 2));
		}

		inline void e6_rotate_down_vector(uint32_t sp)
		{
			uint32_t v = sp + 0x20;
			U16(sp, 0x22) = 0xF000;
			U16(sp, 0x24) = 0;
			U16(v, 0) = 0;
			x::GTE_SetRotMatrix(sp);
			x::GTE_LoadV0(v);
			x::GTE_MVMVA_RotV0();
			x::GTE_StoreIR123(v);
		}

		inline void e6_rand_mod(uint32_t sp, int32_t off)
		{
			if (U16(sp, off) != 0)
			{
				int32_t r = (int32_t)x::CrtRand();
				U16(sp, off) = (uint16_t)(r % (int32_t)S16(sp, off));
			}
		}
	}

	uint32_t __cdecl a_737FD0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(G(G_258FB68), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(G(G_258FB68), 0) = sp;
		uint32_t node = a_7387B0(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(G(G_258FB78), 0);
			int32_t k = S8(a1, 0x6A);
			uint32_t v = U32(U32(ctx, 0x228), shl32(k, 2));
			U32(node, 0x170) = v;
			U8(node, 0x1D1) = (uint8_t)a_7424B0(v);
			if (U8(desc, 0x16) == 2)
			{
				uint8_t b17 = U8(desc, 0x17);
				uint8_t b18 = U8(desc, 0x18);
				int32_t s1a = S8(desc, 0x1A);
				U8(node, 0x1D3) = b17;
				int32_t s19 = S8(desc, 0x19);
				U8(node, 0x1D4) = b18;
				U8(node, 0x1D5) = (uint8_t)a_742290((uint32_t)s19, (uint32_t)s1a);
			}
		}
		U16(node, 0x1DA) = U16(desc, 0x64);
		if ((int32_t)a3 > 0)
		{
			uint32_t src = a1 + 0x4C;
			uint32_t emat = a1 + 0x2C;
			uint32_t out_speed = node + 0x184;
			uint32_t mat = node + 0x0C;
			uint32_t pos = node + 0x12C;
			for (int32_t n = (int32_t)a3; n != 0; n--)
			{
				e6_copy16(pos - 0x50, src);
				if (U8(desc, 0x22) == 0)
					e6_copy16(pos, src);
				uint8_t spread = U8(desc, 0x34);
				if (spread == 1)
				{
					U16(sp, 0x28) = (uint16_t)(x::CrtRand() & 0xFFF);
					U16(sp, 0x2A) = (uint16_t)(x::CrtRand() & 0xFFF);
					U16(sp, 0x2C) = (uint16_t)(x::CrtRand() & 0xFFF);
					e6_rotate_zxy(sp, sp + 0x28);
					int32_t i2 = shl32(S16(sp, 0x4E), 1);
					if (U8(desc, 0x33) != 0)
					{
						U16(sp, 0x48) = U16(U32(desc, 0xCC), i2);
						U16(sp, 0x4A) = U16(U32(desc, 0xD0), i2);
						U16(sp, 0x4C) = U16(U32(desc, 0xD4), i2);
					}
					else
					{
						U16(sp, 0x48) = U16(U32(desc, 0xCC), i2);
						U16(sp, 0x4A) = U16(U32(desc, 0xD0), i2);
						U16(sp, 0x4C) = U16(U32(desc, 0xD4), i2);
						e6_rand_mod(sp, 0x48);
						e6_rand_mod(sp, 0x4A);
						e6_rand_mod(sp, 0x4C);
					}
					e6_rotate_down_vector(sp);
					int32_t ex = mul32(S16(sp, 0x48), S16(sp, 0x20));
					int32_t ey = mul32(S16(sp, 0x4A), S16(sp, 0x22));
					int32_t ez = mul32(S16(sp, 0x4C), S16(sp, 0x24));
					int32_t old = S32(pos, 0);
					ex = shl32(ex, 4);
					S32(sp, 0x38) = ex;
					S32(pos, 0) = add32(old, ex);
					old = S32(pos, 4);
					ey = shl32(ey, 4);
					S32(sp, 0x3C) = ey;
					S32(pos, 4) = add32(old, ey);
					old = S32(pos, 8);
					ez = shl32(ez, 4);
					S32(sp, 0x40) = ez;
					S32(pos, 8) = add32(old, ez);
				}
				else if (spread == 2)
				{
					U16(sp, 0x28) = (uint16_t)(x::CrtRand() & 0xFFF);
					U16(sp, 0x2A) = 0;
					U16(sp, 0x2C) = 0x400;
					e6_rotate_zxy(sp, sp + 0x28);
					int32_t i2 = shl32(S16(sp, 0x4E), 1);
					if (S8(desc, 0x33) != 1)
					{
						U16(sp, 0x48) = U16(U32(desc, 0xCC), i2);
						U16(sp, 0x4A) = U16(U32(desc, 0xD0), i2);
						U16(sp, 0x4C) = U16(U32(desc, 0xD4), i2);
						e6_rand_mod(sp, 0x48);
						e6_rand_mod(sp, 0x4A);
						e6_rand_mod(sp, 0x4C);
					}
					else
					{
						U16(sp, 0x48) = U16(U32(desc, 0xCC), i2);
						U16(sp, 0x4A) = U16(U32(desc, 0xD0), i2);
						e6_rand_mod(sp, 0x4A);
						int32_t e = S16(sp, 0x4E);
						U16(sp, 0x4C) = U16(U32(desc, 0xD4), shl32(e, 1));
					}
					if ((x::CrtRand() & 1) != 0)
						U16(sp, 0x4A) = (uint16_t)(0 - U16(sp, 0x4A));
					e6_rotate_down_vector(sp);
					int32_t ex = mul32(S16(sp, 0x48), S16(sp, 0x20));
					int32_t ey = shl32(S16(sp, 0x4A), 16);
					int32_t oldx = S32(pos, 0);
					S32(sp, 0x3C) = ey;
					int32_t ez = mul32(S16(sp, 0x4C), S16(sp, 0x24));
					ex = shl32(ex, 4);
					S32(sp, 0x38) = ex;
					S32(pos, 0) = add32(oldx, ex);
					int32_t ey2 = S32(sp, 0x3C);
					S32(pos, 4) = add32(S32(pos, 4), ey2);
					int32_t oldz = S32(pos, 8);
					ez = shl32(ez, 4);
					S32(sp, 0x40) = ez;
					S32(pos, 8) = add32(oldz, ez);
				}
				int32_t rmode = S8(desc, 0x21);
				if (rmode == 0)
					a_742350(sp + 0x30, desc + 0x3C, desc + 0x44);
				else if (rmode == 1)
				{
					uint32_t c0 = U32(sp, 0x28);
					uint32_t c1 = U32(sp, 0x2C);
					U32(sp, 0x30) = c0;
					U32(sp, 0x34) = c1;
				}
				e6_rotate_zxy(mat, sp + 0x30);
				x::GTE_SetRotMatrix(emat);
				x::GTE_LoadIRFromMatrixColumn(mat);
				x::GTE_MVMVA_RotIR();
				x::GTE_StoreIRToMatrixColumn(mat);
				x::GTE_LoadIRFromMatrixColumn(mat + 2);
				x::GTE_MVMVA_RotIR();
				x::GTE_StoreIRToMatrixColumn(mat + 2);
				x::GTE_LoadIRFromMatrixColumn(mat + 4);
				x::GTE_MVMVA_RotIR();
				x::GTE_StoreIRToMatrixColumn(mat + 4);
				if (U8(desc, 0x27) == 0)
				{
					uint32_t r1 = a_7422C0(U32(desc, 0x4C), U32(desc, 0x50));
					U32(out_speed, -0x10) = r1;
					uint32_t r2 = a_7422C0(U32(desc, 0x54), U32(desc, 0x58));
					U32(out_speed, 0) = r2;
				}
				out_speed += 4;
				pos += 0x10;
				mat += 0x20;
			}
		}
		a_742350(node + 0x194, desc + 0x148, desc + 0x150);
		uint8_t m1e = U8(desc, 0x1E);
		if (m1e == 1 || m1e == 2)
		{
			int32_t s20 = S8(desc, 0x20);
			int32_t s1f = S8(desc, 0x1F);
			U16(node, 0x1C8) = (uint16_t)a_742290((uint32_t)s1f, (uint32_t)s20);
		}
		if (U8(desc, 0x27) == 0)
		{
			U32(node, 0x1DC) = a_7422C0(U32(desc, 0x5C), U32(desc, 0x60));
			if ((int32_t)a3 > 0)
			{
				uint32_t q = node + 0x220;
				for (int32_t n = (int32_t)a3; n != 0; n--)
				{
					a_742300(q - 0x40, desc + 0x68, desc + 0x78);
					a_742300(q, desc + 0x88, desc + 0x98);
					a_742300(q + 0x40, desc + 0xA8, desc + 0xB8);
					q += 0x10;
				}
			}
		}
		a_743100(node, desc);
		U32(G(G_258FB68), 0) = U32(G(G_258FB68), 0) + 0x50;
		return 0;
	}
}
}

