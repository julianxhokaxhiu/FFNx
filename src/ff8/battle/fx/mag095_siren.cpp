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

// Effect 095: Siren - Silent Voice

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
	uint32_t __cdecl s_73A0F0(uint32_t a1);
	uint32_t __cdecl s_73A130(void);
	uint32_t __cdecl s_73A220(uint32_t a1);
	uint32_t __cdecl s_73A270(void);
	uint32_t __cdecl s_73A6E0(void);
	uint32_t __cdecl s_73A7B0(uint32_t a1);
	uint32_t __cdecl s_73A8E0(uint32_t a1);
	uint32_t __cdecl s_73A990(uint32_t a1);
	uint32_t __cdecl s_73A9E0(uint32_t a1);
	uint32_t __cdecl s_73AAA0(uint32_t a1);
	uint32_t __cdecl s_73B2F0(uint32_t a1);
	uint32_t __cdecl s_73B320(uint32_t a1);
	uint32_t __cdecl s_73B380(uint32_t a1);
	uint32_t __cdecl s_73B480(uint32_t a1);
	uint32_t __cdecl s_73B670(uint32_t a1);
	uint32_t __cdecl s_73B6B0(uint32_t a1);
	uint32_t __cdecl s_73B6D0(uint32_t a1);
	uint32_t __cdecl s_73B6F0(uint32_t a1);
	uint32_t __cdecl s_73B730(uint32_t a1);
	uint32_t __cdecl s_73B7B0(uint32_t a1);
	uint32_t __cdecl s_73B800(uint32_t a1);
	uint32_t __cdecl s_73B840(uint32_t a1);
	uint32_t __cdecl s_73B880(uint32_t a1);
	uint32_t __cdecl s_73B900(uint32_t a1);
	uint32_t __cdecl s_73B950(uint32_t a1);
	uint32_t __cdecl s_73B9A0(uint32_t a1);
	uint32_t __cdecl s_73B9D0(uint32_t a1);
	uint32_t __cdecl s_73BA30(uint32_t a1);
	uint32_t __cdecl s_73BA50(uint32_t a1);
	uint32_t __cdecl s_73BA70(uint32_t a1);
	uint32_t __cdecl s_73BAD0(uint32_t a1);
	uint32_t __cdecl s_73BB20(uint32_t a1);
	uint32_t __cdecl s_73BBB0(uint32_t a1);
	uint32_t __cdecl s_73BBF0(uint32_t a1);
	uint32_t __cdecl s_73BC10(uint32_t a1);
	uint32_t __cdecl s_73BC60(uint32_t a1);
	uint32_t __cdecl s_73BC90(uint32_t a1);
	uint32_t __cdecl s_73BCF0(uint32_t a1);
	uint32_t __cdecl s_73C150(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl s_73C1A0(uint32_t a1);
	uint32_t __cdecl s_73C220(uint32_t a1);
	uint32_t __cdecl s_73C3B0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl s_73C790(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl s_73CD70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl s_73D1F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl s_73E1A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl s_73E7F0(uint32_t a1);
	uint32_t __cdecl s_73EB10(uint32_t a1);
	uint32_t __cdecl s_73EE10(uint32_t a1);
	uint32_t __cdecl s_73EE50(uint32_t a1);
	uint32_t __cdecl s_73EEC0(uint32_t a1);
	uint32_t __cdecl s_73EF20(uint32_t a1);
	uint32_t __cdecl s_73EF60(uint32_t a1);
	uint32_t __cdecl s_73EFE0(uint32_t a1);
	uint32_t __cdecl s_73F030(uint32_t a1);
	uint32_t __cdecl s_73F070(uint32_t a1);
	uint32_t __cdecl s_73F0C0(uint32_t a1);
	uint32_t __cdecl s_73F180(uint32_t a1);
	uint32_t __cdecl s_73F1D0(uint32_t a1);
	uint32_t __cdecl s_73F1E0(uint32_t a1);
	uint32_t __cdecl s_73F220(uint32_t a1);
	uint32_t __cdecl s_73F250(uint32_t a1);
	uint32_t __cdecl s_73F2B0(uint32_t a1);
	uint32_t __cdecl s_73F300(uint32_t a1);
	uint32_t __cdecl s_73F310(uint32_t a1);
	uint32_t __cdecl s_73F3E0(uint32_t a1);
	uint32_t __cdecl s_73F430(uint32_t a1);
	uint32_t __cdecl s_73F440(uint32_t a1);
	uint32_t __cdecl s_73F4A0(uint32_t a1);
	uint32_t __cdecl s_73F570(uint32_t a1);
	uint32_t __cdecl s_73F5C0(uint32_t a1);
	uint32_t __cdecl s_73F5D0(uint32_t a1);
	uint32_t __cdecl s_73F640(uint32_t a1);
	uint32_t __cdecl s_73F6A0(uint32_t a1);
	uint32_t __cdecl s_73F710(uint32_t a1);
	uint32_t __cdecl s_73F760(uint32_t a1);
	uint32_t __cdecl s_73F770(uint32_t a1);
	uint32_t __cdecl s_73F790(uint32_t a1);
	uint32_t __cdecl s_73F7C0(uint32_t a1);
	uint32_t __cdecl s_73F860(uint32_t a1);
	uint32_t __cdecl s_73F8D0(uint32_t a1);
	uint32_t __cdecl s_73F960(uint32_t a1);
	uint32_t __cdecl s_740470(uint32_t a1);
	uint32_t __cdecl s_740500(void);
	uint32_t __cdecl s_740840(void);
	uint32_t __cdecl s_741B10(uint32_t a1);
	uint32_t __cdecl s_741B90(uint32_t a1);
	uint32_t __cdecl s_741C10(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl s_7423F0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl s_743220(uint32_t a1);
	uint32_t __cdecl s_7432C0(uint32_t a1);
	uint32_t __cdecl s_7433C0(uint32_t a1);
	uint32_t __cdecl s_7434C0(uint32_t a1);
	uint32_t __cdecl s_743B90(uint32_t a1);
	uint32_t __cdecl s_743C70(uint32_t a1);
	uint32_t __cdecl s_743CD0(uint32_t a1);
	uint32_t __cdecl s_743D10(uint32_t a1);
	uint32_t __cdecl s_743DD0(uint32_t a1);
	uint32_t __cdecl s_743FD0(uint32_t a1);
	uint32_t __cdecl s_744010(uint32_t a1);
	uint32_t __cdecl s_744060(uint32_t a1);
	uint32_t __cdecl s_7440C0(uint32_t a1);
	uint32_t __cdecl s_744100(uint32_t a1);
	uint32_t __cdecl s_7441D0(uint32_t a1);
	uint32_t __cdecl s_7443C0(uint32_t a1);
	uint32_t __cdecl s_744400(uint32_t a1);
	uint32_t __cdecl s_744450(uint32_t a1);
	uint32_t __cdecl s_7444B0(uint32_t a1);
	uint32_t __cdecl s_7444F0(uint32_t a1);
	uint32_t __cdecl s_744570(uint32_t a1);
	uint32_t __cdecl s_744730(uint32_t a1);
	uint32_t __cdecl s_744770(uint32_t a1);
	uint32_t __cdecl s_744820(uint32_t a1);
	uint32_t __cdecl s_7448A0(uint32_t a1);
	uint32_t __cdecl s_744AA0(uint32_t a1);
	uint32_t __cdecl s_744BA0(uint32_t a1);
	uint32_t __cdecl s_744BE0(uint32_t a1);
	uint32_t __cdecl s_744C30(uint32_t a1);
	uint32_t __cdecl s_744D30(uint32_t a1, uint32_t a2);
	uint32_t __cdecl s_744E80(uint32_t a1);
	uint32_t __cdecl s_744ED0(uint32_t a1);
	uint32_t __cdecl s_744F30(uint32_t a1);
	uint32_t __cdecl s_744F90(uint32_t a1);
	uint32_t __cdecl s_744FF0(uint32_t a1);
	uint32_t __cdecl s_745010(uint32_t a1);
	uint32_t __cdecl s_745030(uint32_t a1);
	uint32_t __cdecl s_7451E0(uint32_t a1);
	uint32_t __cdecl s_745210(uint32_t a1);
	uint32_t __cdecl s_7452B0(uint32_t a1);
	uint32_t __cdecl s_7452F0(uint32_t a1);
	uint32_t __cdecl s_745380(uint32_t a1);
	uint32_t __cdecl s_745490(uint32_t a1);
	uint32_t __cdecl s_7455B0(uint32_t a1);
	uint32_t __cdecl s_7455F0(uint32_t a1);
	uint32_t __cdecl s_745610(uint32_t a1);
	uint32_t __cdecl s_745650(uint32_t a1);
	uint32_t __cdecl s_7456A0(uint32_t a1);
	uint32_t __cdecl s_745700(uint32_t a1);
	uint32_t __cdecl s_745730(uint32_t a1);
	uint32_t __cdecl s_745740(uint32_t a1);
	uint32_t __cdecl s_745840(uint32_t a1);
	uint32_t __cdecl s_7459D0(uint32_t a1);
	uint32_t __cdecl s_745A20(uint32_t a1);
	uint32_t __cdecl s_745A60(uint32_t a1);
	uint32_t __cdecl s_745A80(uint32_t a1);
	uint32_t __cdecl s_745AD0(uint32_t a1);
	uint32_t __cdecl s_745B40(uint32_t a1);
	uint32_t __cdecl s_745BB0(uint32_t a1);
	uint32_t __cdecl s_745BF0(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl s_745E80(uint32_t a1);
	uint32_t __cdecl s_745F50(uint32_t a1, uint32_t a2);
	uint32_t __cdecl s_7466A0(uint32_t a1);
	uint32_t __cdecl s_746760(uint32_t a1);
	uint32_t __cdecl s_7467B0(uint32_t a1);
	uint32_t __cdecl s_746800(uint32_t a1);
	uint32_t __cdecl s_746870(uint32_t a1);
	uint32_t __cdecl s_7469C0(uint32_t a1);
	uint32_t __cdecl s_746AA0(uint32_t a1);
	uint32_t __cdecl s_746B10(uint32_t a1);
	uint32_t __cdecl s_746B60(uint32_t a1);
	uint32_t __cdecl s_746DF0(uint32_t a1);
	uint32_t __cdecl s_746E50(uint32_t a1);
	uint32_t __cdecl s_746E80(uint32_t a1);
	uint32_t __cdecl s_746ED0(uint32_t a1);
	uint32_t __cdecl s_746F40(uint32_t a1);
	uint32_t __cdecl s_746FD0(uint32_t a1);
	uint32_t __cdecl s_747110(uint32_t a1);
	uint32_t __cdecl s_747160(uint32_t a1);
	uint32_t __cdecl s_747220(uint32_t a1);
	uint32_t __cdecl s_747320(uint32_t a1);
	uint32_t __cdecl s_747370(uint32_t a1);
	uint32_t __cdecl s_7473D0(uint32_t a1);

	namespace
	{
		inline uint32_t s1_inc_state_al(uint32_t node, uint32_t eax_before)
		{
			uint8_t v = (uint8_t)(U8(node, 0x29) + 1);
			U8(node, 0x29) = v;
			return (eax_before & 0xFFFFFF00u) | v;
		}

		inline uint32_t s1_action_rec(uint32_t node)
		{
			int32_t act_idx = S8(node, 0x2A);
			uint32_t actions = U32(U32(node, 0x0C), 4);
			return actions + (uint32_t)(act_idx * 5) * 4;
		}
	}

	uint32_t __cdecl s_73A0F0(uint32_t a1)
	{
		uint32_t node = a1;
		if (U8(node, 0x28) != 0) return 0;
		uint32_t cur = U32(game_addr(game::MAG_095_FileArenaCursor), 0);
		U32(game_addr(game::MAG_095_ParticlePoolPtr), 0) = cur;
		cur += 0x10E0;
		U32(game_addr(game::MAG_095_ActorPoolPtr), 0) = cur;
		cur += 0xC240;
		U32(game_addr(game::MAG_095_FileArenaCursor), 0) = cur;
		uint32_t r = s_73A130();
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return r;
	}

	uint32_t __cdecl s_73A130(void)
	{
		x::MAG_007_sub_8DCC00(U32(game_addr(game::MAG_095_ParticlePoolPtr), 0), 0x10E0);
		x::MAG_007_sub_8DCC00(U32(game_addr(game::MAG_095_ActorPoolPtr), 0), 0xC240);
		U16(game_addr(game::MAG_095_ParticleCursor), 0) = 0;
		U16(game_addr(game::MAG_095_ActorCursor), 0) = 0;
		U16(game_addr(game::unk_258E89C), 0) = 0;
		U16(game_addr(game::unk_257F8A8), 0) = 0;
		return 0;
	}

	uint32_t __cdecl s_73A220(uint32_t a1)
	{
		uint32_t node = a1;
		s_73A6E0();
		s_73A270();
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_LoadScriptQueue), game_addr(game::ActEng_FileLoadScriptTask), 0x30, node);
		uint32_t r = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_DirectorQueue), game_addr(game::Siren_DirectorTask), 0x48, node);
		return s1_inc_state_al(node, r);
	}

	uint32_t __cdecl s_73A270(void)
	{
		U32(game_addr(game::MAG_095_LoadScriptCursorPtr), 0) = game_addr(game::unk_1533850);
		U32(game_addr(game::MAG_095_AssetTablePtr), 0) = game_addr(game::MAG_095_AssetTable);
		x::MAG_007_sub_8DCC00(game_addr(game::MAG_095_AssetTable), 0x16C);
		uint32_t tab = U32(game_addr(game::MAG_095_AssetTablePtr), 0);
		uint32_t cur = U32(game_addr(game::MAG_095_FileArenaCursor), 0);
		const uint32_t edx = game_addr(game::unk_19F1018);
		U32(tab, 0x100) = cur;
		cur += 0xA000;
		U32(game_addr(game::MAG_095_FileArenaCursor), 0) = cur;
		const uint32_t ecx = game_addr(game::unk_19D1018);
		U32(tab, 0x114) = ecx;
		U32(tab, 0x118) = ecx;
		U32(tab, 0x11C) = ecx;
		U32(tab, 0x120) = ecx;
		U32(tab, 0x104) = ecx;
		U32(tab, 0x108) = game_addr(game::unk_19DFCC4);
		U32(tab, 0x10C) = edx;
		U32(tab, 0x110) = ecx;
		U32(tab, 0x124) = ecx;
		U32(tab, 0x128) = edx;
		U32(tab, 0x12C) = game_addr(game::unk_19E10F8);
		U32(tab, 0x130) = game_addr(game::unk_19EBF30);
		U32(tab, 0x134) = game_addr(game::unk_19ED200);
		U32(tab, 0x138) = game_addr(game::unk_19EE4D0);
		U32(tab, 0x13C) = game_addr(game::unk_19F19BC);
		U32(tab, 0x140) = ecx;
		U32(tab, 0x144) = game_addr(game::unk_19E0F58);
		U32(tab, 0x148) = game_addr(game::unk_19E1028);
		U32(tab, 0x14C) = ecx;
		U32(tab, 0x150) = ecx;
		U32(tab, 0x154) = ecx;
		U32(tab, 0x158) = ecx;
		U32(tab, 0x15C) = ecx;
		U32(tab, 0x160) = ecx;
		U16(tab, 0x164) = 0;
		U16(tab, 0x166) = 0;
		return tab;
	}

	uint32_t __cdecl s_73A6E0(void)
	{
		x::MAG_007_sub_8DCC00(U32(game_addr(game::unk_1533010), 0), 0x54);
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		U16(blk, 0) = 0;
		U16(blk, 2) = 0;
		U16(blk, 4) = 0xCC0;
		uint32_t r = a_73A720(blk + 8);
		uint32_t blk2 = U32(game_addr(game::unk_1533010), 0);
		U16(blk2, 8) = 0;
		return r;
	}

	uint32_t __cdecl s_73A7B0(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t states[28];
		states[0] = game_addr(game::MAG_095_Siren_Director_State0_ArmLoadScriptStep);
		states[1] = game_addr(game::MAG_095_Siren_Director_State1_SpawnCameraScript);
		states[2] = game_addr(game::MAG_095_Siren_Director_State2_WaitDirGate1);
		states[3] = game_addr(game::MAG_095_Siren_Director_State3_PlaySoundAtCut1);
		states[4] = game_addr(game::MAG_095_Siren_Director_State4_ToggleUntilLoaded);
		states[5] = game_addr(game::MAG_095_Siren_Director_State5_WaitCutFrame1E);
		states[6] = game_addr(game::MAG_095_Siren_Director_State6_WaitDirGate2);
		states[7] = game_addr(game::MAG_095_Siren_Director_State7_WaitDirPhase2);
		states[8] = game_addr(game::MAG_095_Siren_Director_State8_ArmLoadScriptStep);
		states[9] = game_addr(game::MAG_095_Siren_Director_State9_SpawnScreenTint);
		states[10] = game_addr(game::MAG_095_Siren_Director_State10_CountdownToggle);
		states[11] = game_addr(game::MAG_095_Siren_Director_State11_RequestCut3PlaySound);
		states[12] = game_addr(game::MAG_095_Siren_Director_State12_HideMonstersAtCut3);
		states[13] = game_addr(game::MAG_095_Siren_Director_State13_WaitCutFrame2D);
		states[14] = game_addr(game::MAG_095_Siren_Director_State14_RequestCut4);
		states[15] = game_addr(game::MAG_095_Siren_Director_State15_RestoreMonstersAtCut5);
		states[16] = game_addr(game::MAG_095_Siren_Director_State16_PlaySoundAtCut6);
		states[17] = game_addr(game::MAG_095_Siren_Director_State17_ArmLoadScriptStep);
		states[18] = game_addr(game::MAG_095_Siren_Director_State18_PauseUntilLoaded);
		states[19] = game_addr(game::MAG_095_Siren_Director_State19_WaitCutFrame1E);
		states[20] = game_addr(game::MAG_095_Siren_Director_State20_WaitDirGate7);
		states[21] = game_addr(game::MAG_095_Siren_Director_State21_WaitCut7);
		states[22] = game_addr(game::MAG_095_Siren_Director_State22_SetEndFlagAtFrame1);
		states[23] = game_addr(game::MAG_095_Siren_Director_State23_CountdownTimer);
		states[24] = game_addr(game::MAG_095_Siren_Director_State24_RequestCut8ReleaseVoice);
		states[25] = game_addr(game::MAG_095_Siren_Director_State25_WaitCut8);
		states[26] = game_addr(game::MAG_095_Siren_Director_State26_WaitDirGate9Finish);
		states[27] = game_addr(game::MAG_095_Siren_Director_State27_Empty);
		s_73A8E0(node);
		int32_t st = S8(node, 0x29);
		callp(states[st], node);
		uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((status & 1) && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl s_73A8E0(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		U16(blk, 0x46) = (uint16_t)(U16(blk, 0x46) + 1);
		uint16_t req = U16(blk, 0x42);
		if (U16(blk, 0x40) != req)
		{
			U16(blk, 0x40) = req;
			U16(blk, 0x46) = 0;
			s_73BCF0(node);
			blk = U32(game_addr(game::unk_1533010), 0);
		}
		uint16_t pend = U16(blk, 0x44);
		if (U16(blk, 0x42) != pend)
		{
			U16(blk, 0x42) = pend;
			a_73A6D0();
		}
		return blk;
	}

	uint32_t __cdecl s_73A990(uint32_t a1)
	{
		uint32_t node = a1;
		if (S16(node, 0x24) < 4) return 0;
		uint32_t slot = x::BdSound_ClaimVoiceSlot(game_addr(game::unk_16A3FB0), 1, 0x80);
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		U16(blk, 0x52) = (uint16_t)slot;
		uint32_t r = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::Siren_CameraScriptTask), 0xE0, node);
		return s1_inc_state_al(node, r);
	}

	uint32_t __cdecl s_73A9E0(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t states[15];
		states[0] = game_addr(game::MAG_095_Siren_CameraScript_State0_StartPathAtCut1);
		int32_t st = S8(node, 0x29);
		states[1] = game_addr(game::MAG_095_Siren_CameraScript_State1_CamKeyStep);
		states[2] = game_addr(game::MAG_095_Siren_CameraScript_State2_StartPathAtCut2);
		states[3] = game_addr(game::MAG_095_Siren_CameraScript_State3_StepThenPathAtCut3);
		states[4] = game_addr(game::MAG_095_Siren_CameraScript_State4_CamKeyStepThenKeyAtStep4);
		states[5] = game_addr(game::MAG_095_Siren_CameraScript_State5_StepThenCamSequence);
		states[6] = game_addr(game::MAG_095_Siren_CameraScript_State6_StepCamSequence);
		states[7] = game_addr(game::MAG_095_Siren_CameraScript_State7_WaitDirGate5);
		states[8] = game_addr(game::MAG_095_Siren_CameraScript_State8_StartPathAtCut5);
		states[9] = game_addr(game::MAG_095_Siren_CameraScript_State9_StepCountdown);
		states[10] = game_addr(game::MAG_095_Siren_CameraScript_State10_StepRequestCut6);
		states[11] = game_addr(game::MAG_095_Siren_CameraScript_State11_StepThenPathAtCut6);
		states[12] = game_addr(game::MAG_095_Siren_CameraScript_State12_StepUntilCut8);
		states[13] = game_addr(game::MAG_095_Siren_CameraScript_State13_FinishNode);
		states[14] = game_addr(game::MAG_095_Siren_CameraScript_State14_Empty);
		callp(states[st], node);
		uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((status & 1) && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl s_73AAA0(uint32_t a1)
	{
		uint32_t r = a_73AAE0(1);
		if (r == 0) return 0;
		a_73AB00();
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		a_73AB20(game_addr(game::unk_1533C40), blk + 8, 0);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73B2F0(uint32_t a1)
	{
		uint32_t r = a_73AAE0(2);
		if (r == 0) return 0;
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		a_73AB20(game_addr(game::unk_1533C88), blk + 8, 0);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73B320(uint32_t a1)
	{
		a_73AE10();
		uint32_t r = a_73AAE0(3);
		if (r == 0) return 0;
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		a_73AB20(game_addr(game::unk_1533CD0), blk, 0);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73B380(uint32_t a1)
	{
		uint32_t r = a_73AE10();
		if (r != 1) return r;
		a_73B3B0(game_addr(game::unk_1533C30));
		U16(a1, 0xDC) = 0x68;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73B480(uint32_t a1)
	{
		a_73B4B0();
		U16(a1, 0xDC) = (uint16_t)(U16(a1, 0xDC) - 1);
		if (S16(a1, 0xDC) <= 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73B670(uint32_t a1)
	{
		uint32_t r = a_73AAE0(5);
		if (r == 0) return 0;
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		a_73AB20(game_addr(game::unk_1533D60), blk, 0);
		U16(a1, 0xDC) = 0x20;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73B6B0(uint32_t a1)
	{
		a_73AE10();
		U16(a1, 0xDC) = (uint16_t)(U16(a1, 0xDC) - 1);
		if (S16(a1, 0xDC) <= 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73B6D0(uint32_t a1)
	{
		a_73AE10();
		uint32_t r = a_73B640(6);
		if (r == 0) return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73B6F0(uint32_t a1)
	{
		a_73AE10();
		uint32_t r = a_73AAE0(6);
		if (r == 0) return 0;
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		a_73AB20(game_addr(game::unk_1533DA8), blk + 8, 0);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73B730(uint32_t a1)
	{
		a_73AE10();
		uint32_t r = a_73AAE0(8);
		if (r == 0) return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73B7B0(uint32_t a1)
	{
		uint32_t r = a_73B7E0(1);
		if (r == 0) return 0;
		x::BdPlaySE(game_addr(game::unk_1533014), 1, 0x80);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73B800(uint32_t a1)
	{
		uint32_t tab = U32(game_addr(game::MAG_095_AssetTablePtr), 0);
		if (U16(tab, 0x166) == 0)
		{
			s_73B840(0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return a1;
		}
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		if (S16(blk, 0x46) < 0x1D) return tab;
		return s_73B840(1);
	}

	uint32_t __cdecl s_73B840(uint32_t a1)
	{
		uint32_t cur = U32(game_addr(game::MAG_095_PauseToggle), 0);
		if (a1 == 0)
		{
			if (cur == 1)
			{
				U32(game_addr(game::MAG_095_PauseToggle), 0) = 0;
				U32(game_addr(game::MAG_095_PrimPlayerPause), 0) = 0;
			}
			return 0;
		}
		if (cur != 0) return 0;
		U32(game_addr(game::MAG_095_PauseToggle), 0) = 1;
		U32(game_addr(game::MAG_095_PrimPlayerPause), 0) = 1;
		return 1;
	}

	uint32_t __cdecl s_73B880(uint32_t a1)
	{
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		if (S16(blk, 0x46) < 0x1E) return blk;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73B900(uint32_t a1)
	{
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		if (S16(blk, 0x46) <= 0x19) return blk;
		uint32_t node = a1;
		U16(node, 0x44) = 6;
		uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_ScreenTintQueue), game_addr(game::Effect_ScreenTintTask), 0x40, node);
		U32(t, 0x34) = game_addr(game::unk_1533E10);
		U16(t, 0x38) = 0;
		return s1_inc_state_al(node, t);
	}

	uint32_t __cdecl s_73B950(uint32_t a1)
	{
		uint32_t node = a1;
		U16(node, 0x44) = (uint16_t)(U16(node, 0x44) - 1);
		uint16_t cnt = U16(node, 0x44);
		if ((int16_t)cnt > 0) return cnt;
		uint32_t tab = U32(game_addr(game::MAG_095_AssetTablePtr), 0);
		if (U16(tab, 0x166) == 0)
		{
			uint32_t r = s_73B840(0);
			return s1_inc_state_al(node, r);
		}
		return s_73B840(1);
	}

	uint32_t __cdecl s_73B9A0(uint32_t a1)
	{
		uint32_t r = a_73B640(3);
		if (r == 0) return 0;
		x::BdPlaySE(game_addr(game::unk_1533018), 1, 0x80);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73B9D0(uint32_t a1)
	{
		uint32_t r = a_73B7E0(3);
		if (r == 0) return 0;
		a_73B9F0();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73BA30(uint32_t a1)
	{
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		if (S16(blk, 0x46) <= 0x2D) return blk;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73BA50(uint32_t a1)
	{
		uint32_t r = a_73B640(4);
		if (r == 0) return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73BA70(uint32_t a1)
	{
		uint32_t r = a_73B7E0(5);
		if (r == 0) return 0;
		a_73BA90();
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73BAD0(uint32_t a1)
	{
		uint32_t r = a_73B7E0(6);
		if (r == 0) return 0;
		x::BdPlaySE(game_addr(game::unk_1533020), 1, 0x80);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73BB20(uint32_t a1)
	{
		uint32_t tab = U32(game_addr(game::MAG_095_AssetTablePtr), 0);
		if (U16(tab, 0x166) == 0)
		{
			a_73BB60(0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			return a1;
		}
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		if (S16(blk, 0x46) < 0x1D) return tab;
		return a_73BB60(1);
	}

	uint32_t __cdecl s_73BBB0(uint32_t a1)
	{
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		if (S16(blk, 0x46) <= 0x1E) return blk;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73BBF0(uint32_t a1)
	{
		uint32_t r = a_73B7E0(7);
		if (r == 0) return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73BC10(uint32_t a1)
	{
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		if (S16(blk, 0x46) <= 1) return blk;
		U16(blk, 0x48) = 1;
		U16(a1, 0x44) = 8;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73BC60(uint32_t a1)
	{
		uint32_t r = a_73B640(8);
		if (r == 0) return 0;
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		x::sub_4A2940((uint32_t)(int32_t)S16(blk, 0x52));
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73BC90(uint32_t a1)
	{
		uint32_t r = a_73B7E0(8);
		if (r == 0) return 0;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	uint32_t __cdecl s_73BCF0(uint32_t a1)
	{
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		int32_t cut = S16(blk, 0x40);
		switch (cut)
		{
		case 1:
		{
			uint32_t node = a1;
			s_73C150(node, game_addr(game::MAG_095_Cut1ModelEmitterTask), game_addr(game::unk_16A5380), 0x94, 0, 0);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_OrbitSparkleSpawnerTask), 0xE0, node);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_BurstSparkleSpawnerTask), 0xE0, node);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_StageWobbleInTask), 0xE0, node);
			break;
		}
		case 2:
		{
			uint32_t tab = U32(game_addr(game::MAG_095_AssetTablePtr), 0);
			uint32_t node = a1;
			s_73C150(node, game_addr(game::MAG_095_ModelTaskA), U32(tab, 0x128), 0x48, 1, 0);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_KeyPathSparkleSpawnerTask), 0xE0, node);
			uint32_t rec = s1_action_rec(node);
			if (U8(rec, 0x10) == 0) break;
			uint32_t i = 0;
			uint32_t off = 0;
			do
			{
				uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_TargetRiseSpawnerTask), 0xE0, node);
				rec = s1_action_rec(node);
				i++;
				off += 0x18;
				uint32_t targets = U32(rec, 8);
				U8(t, 0x2D) = U8(targets + off - 0x18, 0);
			} while ((int32_t)i < (int32_t)U8(rec, 0x10));
			break;
		}
		case 3:
		{
			uint32_t node = a1;
			uint32_t actor = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_CreatureQueue), game_addr(game::GF_095Siren_CreatureActorTask), 0x140, node);
			uint32_t tab = U32(game_addr(game::MAG_095_AssetTablePtr), 0);
			x::Effect_BindModelContainerSetAnim(actor, U32(tab, 0x114), 0);
			tab = U32(game_addr(game::MAG_095_AssetTablePtr), 0);
			s_73C150(node, game_addr(game::MAG_095_ModelTaskB), U32(tab, 0x12C), 0x228, 2, 0);
			tab = U32(game_addr(game::MAG_095_AssetTablePtr), 0);
			s_73C150(node, game_addr(game::MAG_095_ModelTaskC), U32(tab, 0x130), 0x4A0, 3, 0);
			tab = U32(game_addr(game::MAG_095_AssetTablePtr), 0);
			s_73C150(node, game_addr(game::MAG_095_ModelTaskG), U32(tab, 0x138), 0xEC, 5, 0);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_SpawnScriptTask), 0xE0, node);
			break;
		}
		case 4:
		{
			uint32_t tab = U32(game_addr(game::MAG_095_AssetTablePtr), 0);
			uint32_t node = a1;
			s_73C150(node, game_addr(game::MAG_095_ModelTaskD), U32(tab, 0x134), 0x4A0, 4, 0);
			tab = U32(game_addr(game::MAG_095_AssetTablePtr), 0);
			s_73C150(node, game_addr(game::MAG_095_ModelTaskE), U32(tab, 0x134), 0x4A0, 4, 0);
			tab = U32(game_addr(game::MAG_095_AssetTablePtr), 0);
			s_73C150(node, game_addr(game::MAG_095_ModelTaskF), U32(tab, 0x134), 0x4A0, 4, 0);
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueB), game_addr(game::MAG_095_OrbitSpriteSpawnerTask), 0x64, node);
			for (uint32_t i = 0; (int32_t)i < 4; i++)
			{
				uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::Siren_WaterSurfaceTask), 0xE0, node);
				U16(t, 0xDE) = (uint16_t)i;
			}
			break;
		}
		case 6:
		{
			uint32_t node = a1;
			uint32_t rec = s1_action_rec(node);
			if (U8(rec, 0x10) != 0)
			{
				uint32_t i = 0;
				uint32_t off = 0;
				do
				{
					uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueB), game_addr(game::MAG_095_TargetSparkSpawnerTask), 0x64, node);
					rec = s1_action_rec(node);
					i++;
					off += 0x18;
					uint32_t targets = U32(rec, 8);
					U8(t, 0x2D) = U8(targets + off - 0x18, 0);
				} while ((int32_t)i < (int32_t)U8(rec, 0x10));
			}
			rec = s1_action_rec(node);
			if (U8(rec, 0x10) == 0) break;
			uint32_t i = 0;
			uint32_t off = 0;
			do
			{
				uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_BoxSparkleSpawnerTask), 0xE0, node);
				rec = s1_action_rec(node);
				i++;
				uint32_t targets = U32(rec, 8);
				off += 0x18;
				U8(t, 0x2D) = U8(targets + off - 0x18, 0);
			} while ((int32_t)i < (int32_t)U8(rec, 0x10));
			break;
		}
		case 7:
		{
			uint32_t node = a1;
			uint32_t rec = s1_action_rec(node);
			if (U8(rec, 0x10) != 0)
			{
				uint32_t i = 0;
				uint32_t off = 0;
				do
				{
					uint32_t tab = U32(game_addr(game::MAG_095_AssetTablePtr), 0);
					uint32_t t = s_73C150(node, game_addr(game::MAG_095_TargetModelTask), U32(tab, 0x13C), 0x140, 6, 0);
					rec = s1_action_rec(node);
					i++;
					off += 0x18;
					uint32_t targets = U32(rec, 8);
					U8(t, 0x2D) = U8(targets + off - 0x18, 0);
				} while ((int32_t)i < (int32_t)U8(rec, 0x10));
			}
			uint32_t tab = U32(game_addr(game::MAG_095_AssetTablePtr), 0);
			a_73C100(node, game_addr(game::MAG_095_ParticleSystemTask), U32(tab, 0x10C), 0, 0x2D, 0);
			break;
		}
		case 8:
			x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_StageWobbleOutTask), 0xE0, a1);
			break;
		default:
			break;
		}
		return 0;
	}

	uint32_t __cdecl s_73C150(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_ModelEmitterQueue), a2, 0x534, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		S32(t, 0x78) = (int32_t)(int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl s_73C1A0(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_095_Cut1ModelEmitter_State0_InitDraw);
		int32_t st = S8(node, 0x29);
		states[1] = game_addr(game::MAG_095_Cut1ModelEmitter_State1_DrawUntilCut2);
		states[2] = game_addr(game::MAG_095_Cut1ModelEmitter_State2_Nop);
		callp(states[st], node);
		uint8_t u = U8(node, 0x8C);
		uint8_t v = U8(node, 0x90);
		uint8_t status = U8(node, 0x26);
		uint32_t nu = (uint32_t)(uint8_t)(u + 4) & 0x7F;
		uint32_t nv = (uint32_t)(uint8_t)(v - 6) & 0x7F;
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		U16(node, 0x8C) = (uint16_t)nu;
		U16(node, 0x90) = (uint16_t)nv;
		if ((status & 1) && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl s_73C220(uint32_t a1)
	{
		uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		uint32_t blk = U32(game_addr(game::unk_1533010), 0);
		uint32_t p0 = U32(blk, 8);
		uint32_t p1 = U32(blk, 0xC);
		U32(node, 0x1C) = p0;
		U32(node, 0x20) = p1;
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + 0xE000);
		U16(node, 0x1E) = 0;
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		uint32_t r = a_73C280(node);
		return s1_inc_state_al(node, r);
	}

	namespace
	{
		uint32_t s2_twin(uint32_t ctx, int32_t b)
		{
			uint32_t ecx = (uint32_t)(U8(ctx, b + 2) & 0xF8) | 0xFFFE2000u;
			uint32_t edx = (uint32_t)(U8(ctx, b) & 0xF8);
			ecx <<= 5;
			ecx |= edx;
			edx = U16(ctx, b + 6);
			ecx <<= 5;
			edx = ~(edx - 1) & 0xF8;
			ecx |= edx;
			edx = U16(ctx, b + 4);
			ecx <<= 2;
			edx = (uint32_t)((int32_t)~(edx - 1) >> 3) & 0x1F;
			ecx |= edx;
			return ecx;
		}
	}

	uint32_t __cdecl s_73C3B0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		const uint32_t obj = a2;
		const uint32_t env = a3;
		if (((int32_t)S16(obj, 0x1C) | S32(obj, 0x18)) == 0)
			return 0;
		if (S16(obj, 0x24) >= 0x1000 && U32(obj, 0x20) == 0)
			return 0;

		const uint32_t ctx = x::Field_Alloc(0x68);
		uint32_t model = U32(a1, 0);
		const int32_t mdl = S16(obj, 2);
		model = model + U32(model + (uint32_t)mdl * 4u + 8, 0);
		const uint16_t frB = U16(obj, 0x2A);
		const uint16_t frA = U16(obj, 0x28);
		U32(ctx, 0) = model;
		auto frame_ptr = [&](uint16_t f) -> uint32_t {
			if (f == 0)
				return model + 0xC;
			return model + (uint32_t)mul32(S32(model, 4), (int32_t)(int16_t)f) * 8u + 0xC;
		};
		if (frA == frB)
			U32(ctx, 4) = frame_ptr(frA);
		else
		{
			const uint16_t lerp = U16(obj, 0x26);
			if (lerp == 0)
				U32(ctx, 4) = frame_ptr(frA);
			else if (lerp == 0x1000)
				U32(ctx, 4) = frame_ptr(frB);
			else
			{
				x::MAG_017_sub_701390(model, (uint32_t)(int32_t)(int16_t)frA, (uint32_t)(int32_t)(int16_t)frB,
					(uint32_t)(int32_t)S16(obj, 0x26), U32(env, 0x48));
				U32(ctx, 4) = U32(env, 0x48);
			}
		}

		alignas(4) uint8_t stk[0x48] = {};
		auto L = [&](int32_t off) -> uint32_t { return P(stk) + (uint32_t)(off - 0x10); };
		const uint32_t mat = L(0x38);

		x::MAG_017_sub_701310(obj + 0x10, mat);
		const uint16_t px = U16(obj, 8);
		const uint16_t py = U16(obj, 0xA);
		const uint16_t pz = U16(obj, 0xC);
		U16(L(0x10), 0) = px;
		U16(L(0x12), 0) = py;
		U16(L(0x14), 0) = pz;
		int32_t tx, ty, tz;
		if (U8(obj, 5) & 2)
		{
			tx = (int16_t)px;
			ty = (int16_t)py;
			tz = (int16_t)pz;
		}
		else
		{
			x::GTE_SetRotMatrix(env);
			x::GTE_LoadV0(L(0x10));
			x::GTE_MVMVA_RotV0();
			x::GTE_ReadMAC123(L(0x4C));
			x::GTE_MatrixMultiply(env, mat);
			tx = S32(L(0x4C), 0);
			ty = S32(L(0x50), 0);
			tz = S32(L(0x54), 0);
		}
		S32(L(0x4C), 0) = add32(tx, S32(env, 0x14));
		S32(L(0x50), 0) = add32(ty, S32(env, 0x18));
		S32(L(0x54), 0) = add32(tz, S32(env, 0x1C));
		if (!(U32(obj, 0x18) == 0x10001000u && U16(obj, 0x1C) == 0x1000))
		{
			if (U32(obj, 4) & 0x100)
			{
				U16(L(0x18), 0) = U16(obj, 0x18);
				U16(L(0x20), 0) = U16(obj, 0x1A);
				const uint16_t sz = U16(obj, 0x1C);
				U16(L(0x28), 0) = sz;
				U16(L(0x22), 0) = 0;
				U16(L(0x1C), 0) = 0;
				U16(L(0x26), 0) = 0;
				U16(L(0x1A), 0) = 0;
				U16(L(0x24), 0) = 0;
				U16(L(0x1E), 0) = 0;
				x::sub_56C220(mat, L(0x18));
			}
			else
			{
				S32(L(0x18), 0) = S16(obj, 0x18);
				S32(L(0x1C), 0) = S16(obj, 0x1A);
				S32(L(0x20), 0) = S16(obj, 0x1C);
				x::scale3DMatrix(mat, L(0x18));
			}
		}
		x::GTE_SetRotMatrix_W(mat);
		x::GTE_SetTransVector_W(mat);

		const uint32_t ofl = U32(obj, 4);
		if (ofl & 0x4000)
			U32(ctx, 0x14) = 0x2000;
		else
			U32(ctx, 0x14) = 0x2030;
		if (ofl & 0x2000)
			U32(ctx, 0x14) = U32(ctx, 0x14) | 0xC;
		const int32_t fade = S16(obj, 0x24);
		S32(ctx, 0xC) = fade;
		if (fade != 0)
		{
			const uint32_t f = U32(ctx, 0x14);
			U32(ctx, 8) = U32(obj, 0x20);
			U32(ctx, 0x14) = f | 0xC0;
		}
		else
		{
			const uint32_t ef = U32(env, 0x44);
			if (ef != 0)
			{
				U32(ctx, 0xC) = ef;
				U32(ctx, 0x14) = U32(ctx, 0x14) | 0xC0;
				U32(ctx, 8) = U32(env, 0x38);
			}
		}

		const int32_t mode = S16(env, 0x56);
		S32(ctx, 0x10) = S16(env, 0x58);
		bool tail = true;
		switch (mode)
		{
		case 0:
			if (U16(obj, 0) == 0)
				U16(ctx, 0x18) = U16(env, 0x4C);
			else
				U16(ctx, 0x18) = U16(env, 0x50);
			break;
		case 1:
			U16(ctx, 0x18) = U16(env, 0x4C);
			break;
		case 2:
			if (U16(obj, 0) == 3)
				U16(ctx, 0x18) = U16(env, 0x4C);
			else
				tail = false;
			break;
		default:
			tail = false;
			break;
		}
		if (tail)
		{
			U16(ctx, 0x1A) = 0;
			U16(ctx, 0x1E) = 0;
			U16(ctx, 0x1C) = 0;
			U16(ctx, 0x22) = 0x100;
			U16(ctx, 0x20) = 0x100;
			U16(ctx, 0x26) = 0;
			U16(ctx, 0x24) = 0;
			U16(ctx, 0x28) = 0x80;
			U16(ctx, 0x2A) = 0x100;
		}
		else
		{
			U16(ctx, 0x18) = 0;
			U16(ctx, 0x1A) = 0;
			U16(ctx, 0x1E) = 0;
			U16(ctx, 0x1C) = 0;
			U16(ctx, 0x22) = 0x100;
			U16(ctx, 0x20) = 0x100;
			U16(ctx, 0x26) = 0;
			U16(ctx, 0x24) = 0;
			U16(ctx, 0x2A) = 0x100;
			U16(ctx, 0x28) = 0x100;
		}

		const uint32_t cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		const uint32_t newCursor = s_73C790(ctx, ot, 2, cursor);
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = newCursor;

		if (S16(env, 0x56) == 0 && U16(obj, 0) == 0)
		{
			MEM<uint16_t>(game_addr(game::unk_258FB80)) = U16(obj, 8);
			MEM<uint16_t>(game_addr(game::unk_258FB82)) = U16(obj, 0xA);
			MEM<uint16_t>(game_addr(game::unk_258FB84)) = U16(obj, 0xC);
			MEM<int32_t>(game_addr(game::unk_258EC60)) = S16(obj, 0x18);
			MEM<int32_t>(game_addr(game::unk_258EC64)) = S16(obj, 0x1A);
			MEM<int32_t>(game_addr(game::unk_258EC68)) = S16(obj, 0x1C);
			s_73EB10(ctx);
		}
		if (U16(env, 0x56) == 1)
		{
			MEM<uint16_t>(game_addr(game::unk_258FB80)) = U16(obj, 8);
			MEM<uint16_t>(game_addr(game::unk_258FB82)) = U16(obj, 0xA);
			MEM<uint16_t>(game_addr(game::unk_258FB84)) = U16(obj, 0xC);
			MEM<int32_t>(game_addr(game::unk_258EC60)) = S16(obj, 0x18);
			MEM<int32_t>(game_addr(game::unk_258EC64)) = S16(obj, 0x1A);
			MEM<int32_t>(game_addr(game::unk_258EC68)) = S16(obj, 0x1C);
			s_73E7F0(ctx);
		}
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl s_73C790(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		if (!(U32(ctx, 0x14) & 0x2000))
			U32(ctx, 4) = U32(ctx, 0) + 8;
		const uint32_t model = U32(ctx, 0);
		const uint32_t fb = U8(ctx, 0xA);
		const uint32_t lists = U32(model, 0) + model;
		const uint32_t fg = U8(ctx, 9);
		U32(ctx, 0x2C) = lists;
		const uint32_t fr = U8(ctx, 8);
		x::someCameraWork_45DD60(fr, fg, fb);

		uint32_t cur = a4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = callo(game_addr(game::MAG_095_PrimList_FlatTri), ctx, a2, a3, a4);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = callo(game_addr(game::MAG_095_PrimList_FlatQuad), ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = s_73CD70(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = s_73D1F0(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = a_73D770(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = a_73D9C0(ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			cur = callo(game_addr(game::MAG_095_PrimList_GT3Scroll), ctx, a2, a3, cur);
		else
			U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		if (U32(U32(ctx, 0x2C), 0) != 0)
			return s_73E1A0(ctx, a2, a3, cur);
		U32(ctx, 0x2C) = U32(ctx, 0x2C) + 4;
		return cur;
	}

	uint32_t __cdecl s_73CD70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t rec = U32(ctx, 0x2C);
		int32_t count = S32(rec, 0);
		rec += 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = rec;
		if (count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return pkt;
		}
		uint32_t pSXY2 = pkt + 0x18;
		uint32_t pSXY1 = pkt + 0x10;
		uint32_t pCode = pkt + 4;
		uint32_t pSXY0 = pkt + 8;
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4u, vbase + (uint32_t)U16(rec, 6) * 4u,
				vbase + (uint32_t)U16(rec, 8) * 4u);
			x::GTE_RTPT();
			{
				const uint32_t flags = U32(ctx, 0x14);
				const uint32_t code = U32(rec, 0);
				U32(pkt, 0) = 0x7000000;
				U32(pCode, 0) = code;
				if (flags & 1)
					U32(pCode, 0) = code | 0x2000000;
				if (flags & 4)
					U32(pCode, 0) = U32(pCode, 0) & 0xFDFFFFFFu;
				const uint32_t uv2 = U32(rec, 8) >> 16;
				U32(pkt, 0xC) = U32(rec, 0xC);
				U32(pkt, 0x14) = U32(rec, 0x10);
				U32(pkt, 0x1C) = uv2;
			}
			x::GTE_ReadFLAG(ctx + 0x3C);
			if ((U32(ctx, 0x3C) & 0x60000) != 0)
				goto next;
			x::GTE_NCLIP();
			{
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) < 0 && !(U8(ctx, 0x14) & 0x10))
					goto next;
				x::GTE_ReadSXY012_Split(pSXY0, pSXY1, pSXY2);
				x::GTE_AVSZ3();
				int16_t v = S16(pSXY0, 0);
				if (v < 0 || v > 0xA00)
					clip = 1;
				v = S16(pSXY1, 0);
				if (v < 0 || v > 0xA00)
					clip |= 2;
				v = S16(pSXY2, 0);
				if (v < 0 || v > 0xA00)
					clip |= 4;
				v = S16(pkt, 0xA);
				if (v < 0 || v > 0x6C0)
					clip |= 0x10;
				v = S16(pkt, 0x12);
				if (v < 0 || v > 0x6C0)
					clip |= 0x20;
				v = S16(pkt, 0x1A);
				if (v < 0 || v > 0x6C0)
					clip |= 0x40;
				if ((clip & 7) == 7)
					goto next;
				if ((clip & 0x70) == 0x70)
					goto next;
			}
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x40)
			{
				x::set_unk_1CA8A28(pCode);
				x::set_dword_1CA8A30(U32(ctx, 0xC));
				x::sub_45F270();
				x::set_param_with_dword_1CA8A68(pCode);
			}
			{
				const int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
				S32(ctx, 0x38) = z;
				if (z < 0)
					S32(ctx, 0x38) = 0;
				const uint32_t ot = a2 + (uint32_t)(S32(ctx, 0x38) >> (a3 & 31)) * 4u;
				const uint16_t add0 = U16(ctx, 0x18);
				const uint16_t add1 = U16(ctx, 0x1A);
				if ((uint16_t)(add0 | add1) == 0)
				{
					x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
					pSXY0 += 0x20;
					pkt += 0x20;
					pSXY1 += 0x20;
					pSXY2 += 0x20;
					pCode += 0x20;
				}
				else
				{
					if (add0 != 0)
					{
						const uint32_t a = add0;
						const uint32_t c2 = U8(pkt, 0x1C) + a;
						const uint32_t c1 = U8(pkt, 0x14) + a;
						const uint32_t c0 = U8(pkt, 0xC) + a;
						if ((int32_t)(c2 | c1 | c0) > 0xFF)
						{
							const uint8_t s = U8(ctx, 0x28);
							U8(pkt, 0xC) = (uint8_t)(c0 - s);
							U8(pkt, 0x14) = (uint8_t)(c1 - s);
							U8(pkt, 0x1C) = (uint8_t)(c2 - s);
						}
						else
						{
							U8(pkt, 0xC) = (uint8_t)c0;
							U8(pkt, 0x14) = (uint8_t)c1;
							U8(pkt, 0x1C) = (uint8_t)c2;
						}
					}
					const uint16_t add1b = U16(ctx, 0x1A);
					if (add1b != 0)
					{
						const uint32_t a = add1b;
						const uint32_t c2 = U8(pkt, 0x1D) + a;
						const uint32_t c1 = U8(pkt, 0x15) + a;
						const uint32_t c0 = U8(pkt, 0xD) + a;
						if ((int32_t)(c2 | c1 | c0) > 0xFF)
						{
							const uint8_t s = U8(ctx, 0x2A);
							U8(pkt, 0xD) = (uint8_t)(c0 - s);
							U8(pkt, 0x15) = (uint8_t)(c1 - s);
							U8(pkt, 0x1D) = (uint8_t)(c2 - s);
						}
						else
						{
							U8(pkt, 0xD) = (uint8_t)c0;
							U8(pkt, 0x15) = (uint8_t)c1;
							U8(pkt, 0x1D) = (uint8_t)c2;
						}
					}
					const uint32_t poly = pkt;
					pSXY1 += 0x2C;
					pSXY2 += 0x2C;
					pCode += 0x2C;
					pSXY0 += 0x2C;
					uint32_t prim = pkt + 0x20;
					pkt += 0x2C;
					U32(prim, 0) = 0x2000000;
					uint32_t tw = 0;
					if (ctx + 0x1C != 0)
						tw = s2_twin(ctx, 0x1C);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
					pSXY0 += 0xC;
					pSXY1 += 0xC;
					prim = pkt;
					pkt += 0xC;
					pSXY2 += 0xC;
					pCode += 0xC;
					U32(prim, 0) = 0x2000000;
					tw = 0;
					if (ctx + 0x24 != 0)
						tw = s2_twin(ctx, 0x24);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
				}
			}
		next:
			rec += 0x14;
			--count;
		} while (count != 0);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl s_73D1F0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t rec = U32(ctx, 0x2C);
		int32_t count = S32(rec, 0);
		rec += 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = rec;
		if (count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return pkt;
		}
		uint32_t pSXY3 = pkt + 0x20;
		uint32_t pSXY2 = pkt + 0x18;
		uint32_t pSXY1 = pkt + 0x10;
		uint32_t pCode = pkt + 4;
		uint32_t pSXY0 = pkt + 8;
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4u, vbase + (uint32_t)U16(rec, 6) * 4u,
				vbase + (uint32_t)U16(rec, 8) * 4u);
			x::GTE_RTPT();
			{
				const uint32_t flags = U32(ctx, 0x14);
				const uint32_t code = U32(rec, 0);
				U32(pkt, 0) = 0x9000000;
				U32(pCode, 0) = code;
				if (flags & 1)
					U32(pCode, 0) = code | 0x2000000;
				if (flags & 4)
					U32(pCode, 0) = U32(pCode, 0) & 0xFDFFFFFFu;
				U32(pkt, 0xC) = U32(rec, 0xC);
				const uint32_t uv1 = U32(rec, 0x10);
				const uint32_t uv23 = U32(rec, 0x14);
				U32(pkt, 0x1C) = uv23;
				U32(pkt, 0x14) = uv1;
				U32(pkt, 0x24) = uv23 >> 16;
			}
			x::GTE_ReadFLAG(ctx + 0x3C);
			if ((U32(ctx, 0x3C) & 0x60000) != 0)
				goto next;
			x::GTE_NCLIP();
			{
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) < 0 && !(U8(ctx, 0x14) & 0x10))
					goto next;
				x::GTE_ReadSXY012_Split(pSXY0, pSXY1, pSXY2);
				x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0xA) * 4u);
				x::GTE_RTPS();
				int16_t v = S16(pSXY0, 0);
				if (v < 0 || v > 0xA00)
					clip = 1;
				v = S16(pSXY1, 0);
				if (v < 0 || v > 0xA00)
					clip |= 2;
				v = S16(pSXY2, 0);
				if (v < 0 || v > 0xA00)
					clip |= 4;
				v = S16(pkt, 0xA);
				if (v < 0 || v > 0x6C0)
					clip |= 0x10;
				v = S16(pkt, 0x12);
				if (v < 0 || v > 0x6C0)
					clip |= 0x20;
				v = S16(pkt, 0x1A);
				if (v < 0 || v > 0x6C0)
					clip |= 0x40;
				x::GTE_ReadSXY2(pSXY3);
				x::GTE_AVSZ4();
				v = S16(pSXY3, 0);
				if (v < 0 || v > 0xA00)
					clip |= 8;
				v = S16(pkt, 0x22);
				if (v < 0 || v > 0x6C0)
					clip |= 0x80;
				if ((clip & 0xF) == 0xF)
					goto next;
				if ((clip & 0xF0) == 0xF0)
					goto next;
			}
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x40)
			{
				x::set_unk_1CA8A28(pCode);
				x::set_dword_1CA8A30(U32(ctx, 0xC));
				x::sub_45F270();
				x::set_param_with_dword_1CA8A68(pCode);
			}
			{
				const int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
				S32(ctx, 0x38) = z;
				if (z < 0)
					S32(ctx, 0x38) = 0;
				const uint32_t ot = a2 + (uint32_t)(S32(ctx, 0x38) >> (a3 & 31)) * 4u;
				const uint16_t add0 = U16(ctx, 0x18);
				const uint16_t add1 = U16(ctx, 0x1A);
				if ((uint16_t)(add0 | add1) == 0)
				{
					x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
					pSXY0 += 0x28;
					pSXY1 += 0x28;
					pkt += 0x28;
					pSXY2 += 0x28;
					pSXY3 += 0x28;
					pCode += 0x28;
				}
				else
				{
					if (add0 != 0)
					{
						const uint32_t a = add0;
						const uint32_t c0 = U8(pkt, 0xC) + a;
						const uint32_t c1 = U8(pkt, 0x14) + a;
						const uint32_t c2 = U8(pkt, 0x1C) + a;
						const uint32_t c3 = U8(pkt, 0x24) + a;
						if ((int32_t)(c3 | c2 | c1 | c0) > 0xFF)
						{
							const uint8_t s = U8(ctx, 0x28);
							U8(pkt, 0xC) = (uint8_t)(c0 - s);
							U8(pkt, 0x14) = (uint8_t)(c1 - s);
							U8(pkt, 0x1C) = (uint8_t)(c2 - s);
							U8(pkt, 0x24) = (uint8_t)(c3 - s);
						}
						else
						{
							U8(pkt, 0x14) = (uint8_t)c1;
							U8(pkt, 0xC) = (uint8_t)c0;
							U8(pkt, 0x1C) = (uint8_t)c2;
							U8(pkt, 0x24) = (uint8_t)c3;
						}
					}
					const uint16_t add1b = U16(ctx, 0x1A);
					if (add1b != 0)
					{
						const uint32_t a = add1b;
						const uint32_t c0 = U8(pkt, 0xD) + a;
						const uint32_t c1 = U8(pkt, 0x15) + a;
						const uint32_t c2 = U8(pkt, 0x1D) + a;
						const uint32_t c3 = U8(pkt, 0x25) + a;
						if ((int32_t)(c3 | c2 | c1 | c0) > 0xFF)
						{
							const uint8_t s = U8(ctx, 0x2A);
							U8(pkt, 0xD) = (uint8_t)(c0 - s);
							U8(pkt, 0x15) = (uint8_t)(c1 - s);
							U8(pkt, 0x1D) = (uint8_t)(c2 - s);
							U8(pkt, 0x25) = (uint8_t)(c3 - s);
						}
						else
						{
							U8(pkt, 0x1D) = (uint8_t)c2;
							U8(pkt, 0xD) = (uint8_t)c0;
							U8(pkt, 0x15) = (uint8_t)c1;
							U8(pkt, 0x25) = (uint8_t)c3;
						}
					}
					pSXY0 += 0x34;
					pSXY3 += 0x34;
					pSXY2 += 0x34;
					const uint32_t poly = pkt;
					pCode += 0x34;
					pSXY1 += 0x34;
					uint32_t prim = pkt + 0x28;
					pkt += 0x34;
					U32(prim, 0) = 0x2000000;
					uint32_t tw = 0;
					if (ctx + 0x1C != 0)
						tw = s2_twin(ctx, 0x1C);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
					pSXY0 += 0xC;
					pSXY1 += 0xC;
					pSXY2 += 0xC;
					prim = pkt;
					pkt += 0xC;
					pSXY3 += 0xC;
					pCode += 0xC;
					U32(prim, 0) = 0x2000000;
					tw = 0;
					if (ctx + 0x24 != 0)
						tw = s2_twin(ctx, 0x24);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
				}
			}
		next:
			rec += 0x18;
			--count;
		} while (count != 0);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	namespace
	{
		uint32_t s3_texwin(uint32_t ctx, int32_t o)
		{
			uint32_t c = ((uint32_t)U8(ctx, o + 2) & 0xF8) | 0xFFFE2000u;
			c = (c << 5) | ((uint32_t)U8(ctx, o) & 0xF8);
			c = (c << 5) | (~((uint32_t)U16(ctx, o + 6) - 1u) & 0xF8);
			c = (c << 2) | ((~((uint32_t)U16(ctx, o + 4) - 1u) >> 3) & 0x1F);
			return c;
		}

		void s3_scroll_uv(uint32_t pkt, int32_t o, uint32_t d, uint32_t ctx, int32_t wo)
		{
			int32_t a = (int32_t)(U8(pkt, o) + d);
			int32_t c = (int32_t)(U8(pkt, o + 0xC) + d);
			int32_t e = (int32_t)(U8(pkt, o + 0x18) + d);
			int32_t b = (int32_t)(U8(pkt, o + 0x24) + d);
			if ((b | e | c | a) > 0xFF)
			{
				uint8_t w = U8(ctx, wo);
				U8(pkt, o) = (uint8_t)(a - w);
				U8(pkt, o + 0xC) = (uint8_t)(c - w);
				U8(pkt, o + 0x18) = (uint8_t)(e - w);
				U8(pkt, o + 0x24) = (uint8_t)(b - w);
			}
			else
			{
				U8(pkt, o) = (uint8_t)a;
				U8(pkt, o + 0xC) = (uint8_t)c;
				U8(pkt, o + 0x18) = (uint8_t)e;
				U8(pkt, o + 0x24) = (uint8_t)b;
			}
		}

		inline bool s3_off(int16_t v, int16_t lim) { return v < 0 || v > lim; }

		uint32_t s3_quad(uint32_t ctx, uint32_t data, uint32_t vbase, uint32_t ot_base, uint32_t shift, uint32_t pkt)
		{
			x::GTE_LoadV012(vbase + ((uint32_t)U16(data, 4) << 2), vbase + ((uint32_t)U16(data, 6) << 2), vbase + ((uint32_t)U16(data, 8) << 2));
			x::GTE_RTPT();
			uint32_t flags = U32(ctx, 0x14);
			uint32_t col0 = U32(data, 0);
			U32(pkt, 0) = 0x0C000000;
			U32(pkt, 4) = col0;
			if (flags & 2)
			{
				col0 |= 0x2000000;
				U32(pkt, 4) = col0;
			}
			if (flags & 8)
				U32(pkt, 4) &= 0xFDFFFFFF;
			uint32_t uv0 = U32(data, 0xC);
			uint32_t uv1 = U32(data, 0x10);
			U32(pkt, 0xC) = uv0;
			uint32_t uv23 = U32(data, 0x14);
			U32(pkt, 0x24) = uv23;
			U32(pkt, 0x18) = uv1;
			U32(pkt, 0x30) = uv23 >> 16;
			x::GTE_ReadFLAG(ctx + 0x3C);
			if (U32(ctx, 0x3C) & 0x60000)
				return pkt;
			x::GTE_NCLIP();
			uint32_t clip = 0;
			x::GTE_ReadMAC0(ctx + 0x30);
			if (S32(ctx, 0x30) < 0 && !(U8(ctx, 0x14) & 0x20))
				return pkt;
			x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x14, pkt + 0x20);
			x::GTE_LoadV0(vbase + ((uint32_t)U16(data, 0xA) << 2));
			x::GTE_RTPS();
			if (s3_off(S16(pkt, 0x8), 0xA00)) clip = 1;
			if (s3_off(S16(pkt, 0x14), 0xA00)) clip |= 2;
			if (s3_off(S16(pkt, 0x20), 0xA00)) clip |= 4;
			if (s3_off(S16(pkt, 0xA), 0x6C0)) clip |= 0x10;
			if (s3_off(S16(pkt, 0x16), 0x6C0)) clip |= 0x20;
			if (s3_off(S16(pkt, 0x22), 0x6C0)) clip |= 0x40;
			x::GTE_ReadSXY2(pkt + 0x2C);
			x::GTE_AVSZ4();
			uint32_t c = clip;
			if (s3_off(S16(pkt, 0x2C), 0xA00)) c |= 8;
			if (s3_off(S16(pkt, 0x2E), 0x6C0)) c |= 0x80;
			if ((c & 0xF) == 0xF)
				return pkt;
			if ((c & 0xF0) == 0xF0)
				return pkt;
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x80)
			{
				x::sub_45E120(data + 0x18, data + 0x1C, data + 0x20);
				x::set_dword_1CA8A30(U32(ctx, 0xC));
				x::sub_45F4C0();
				x::sub_45E370(pkt + 0x10, pkt + 0x1C, pkt + 0x28);
				x::set_unk_1CA8A28(pkt + 4);
				x::sub_45F270();
				x::set_param_with_dword_1CA8A68(pkt + 4);
			}
			else
			{
				uint32_t c1 = U32(data, 0x18);
				uint32_t c2 = U32(data, 0x1C);
				U32(pkt, 0x10) = c1;
				uint32_t c3 = U32(data, 0x20);
				U32(pkt, 0x1C) = c2;
				U32(pkt, 0x28) = c3;
			}
			int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
			S32(ctx, 0x38) = z;
			if (z < 0)
				S32(ctx, 0x38) = 0;
			uint32_t ot = ot_base + ((uint32_t)(S32(ctx, 0x38) >> (shift & 31)) << 2);
			uint16_t du = U16(ctx, 0x18);
			uint16_t dv = U16(ctx, 0x1A);
			if ((uint16_t)(du | dv) == 0)
			{
				x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
				return pkt + 0x34;
			}
			if (du != 0)
				s3_scroll_uv(pkt, 0xC, du, ctx, 0x28);
			uint16_t dv2 = U16(ctx, 0x1A);
			if (dv2 != 0)
				s3_scroll_uv(pkt, 0xD, dv2, ctx, 0x2A);
			uint32_t prim = pkt;
			uint32_t twa = pkt + 0x34;
			U32(twa, 0) = 0x2000000;
			uint32_t w = (ctx + 0x1C != 0) ? s3_texwin(ctx, 0x1C) : 0;
			U32(twa, 4) = w;
			U32(twa, 8) = 0;
			x::SSIGPU_InsertPrimAutoDepth(ot, twa);
			x::SSIGPU_InsertPrimAutoDepth(ot, prim);
			uint32_t twb = pkt + 0x40;
			U32(twb, 0) = 0x2000000;
			w = (ctx + 0x24 != 0) ? s3_texwin(ctx, 0x24) : 0;
			U32(twb, 4) = w;
			U32(twb, 8) = 0;
			x::SSIGPU_InsertPrimAutoDepth(ot, twb);
			return pkt + 0x4C;
		}

		inline uint32_t s3_task_end(uint32_t n, uint8_t status)
		{
			if ((status & 1) && U8(n, 0x28) == 0)
			{
				x::Effect_ReleaseLinkedTask(n);
				return 2;
			}
			return 0;
		}
	}

	uint32_t __cdecl s_73E1A0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t data = U32(ctx, 0x2C);
		int32_t count = S32(data, 0);
		data += 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = data;
		if (count <= 0)
		{
			U32(ctx, 0x2C) = data;
			return pkt;
		}
		do
		{
			pkt = s3_quad(ctx, data, vbase, a2, a3, pkt);
			data += 0x24;
		} while (--count);
		U32(ctx, 0x2C) = data;
		return pkt;
	}

	uint32_t __cdecl s_73E7F0(uint32_t a1)
	{
		const uint32_t src = U32(a1, 4);
		for (int i = 0; i < 24; i++)
		{
			MEM<uint32_t>(game_addr(game::MAG_095_KeyTable25) + 8 * i) = U32(src, 0x88 + 0x10 * i);
			MEM<uint32_t>(game_addr(game::unk_258BAA4) + 8 * i) = U32(src, 0x8C + 0x10 * i);
		}
		MEM<uint32_t>(game_addr(game::unk_258BB60)) = U32(src, 0x200);
		MEM<uint32_t>(game_addr(game::unk_258BB64)) = U32(src, 0x204);

		alignas(4) uint8_t loc[0x28];
		const uint32_t L = P(loc);
		U32(L, 0) = MEM<uint32_t>(game_addr(game::unk_258BB00));
		U32(L, 4) = MEM<uint32_t>(game_addr(game::unk_258BB04));
		x::MAG_022_sub_8DD770(L + 8);
		x::MAG_022_sub_8DD8A0(L + 8, 0x800);
		x::scale3DMatrix(L + 8, game_addr(game::unk_258EC60));
		int32_t tx = MEM<int16_t>(game_addr(game::unk_258FB80));
		int32_t ty = MEM<int16_t>(game_addr(game::unk_258FB82));
		uint32_t g = MEM<uint32_t>(game_addr(game::unk_1533010));
		S32(L, 0x1C) = tx;
		S32(L, 0x20) = ty;
		int32_t gz = S16(g, 0xC);
		int32_t oz = MEM<int16_t>(game_addr(game::unk_258FB84));
		S32(L, 0x24) = add32(add32(gz, oz), -0x1000);
		x::GTE_SetRotMatrix_W(L + 8);
		x::GTE_SetTransVector_W(L + 8);
		x::GTE_LoadV0(L);
		x::GTE_MVMVA_RotV0_Tr_2();
		x::GTE_StoreIR123(game_addr(game::MAG_095_GfPosTransformed));
		return 0;
	}

	uint32_t __cdecl s_73EB10(uint32_t a1)
	{
		const uint32_t src = U32(a1, 4);
		for (int i = 0; i < 16; i++)
		{
			MEM<uint32_t>(game_addr(game::MAG_095_KeyTableA) + 8 * i) = U32(src, 0x48 + 0x10 * i);
			MEM<uint32_t>(game_addr(game::unk_258E9A4) + 8 * i) = U32(src, 0x4C + 0x10 * i);
		}
		MEM<uint32_t>(game_addr(game::unk_258EA20)) = U32(src, 0);
		MEM<uint32_t>(game_addr(game::unk_258EA24)) = U32(src, 4);
		for (int i = 0; i < 16; i++)
		{
			MEM<uint32_t>(game_addr(game::MAG_095_KeyTableB) + 8 * i) = U32(src, 0x40 + 0x10 * i);
			MEM<uint32_t>(game_addr(game::unk_258E8A4) + 8 * i) = U32(src, 0x44 + 0x10 * i);
		}
		MEM<uint32_t>(game_addr(game::unk_258E920)) = U32(src, 8);
		MEM<uint32_t>(game_addr(game::unk_258E924)) = U32(src, 0xC);
		return 0;
	}

	uint32_t __cdecl s_73EE10(uint32_t a1)
	{
		if (a_73B7E0(2))
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
			return 0;
		}
		a_73C280(a1);
		return 0;
	}

	uint32_t __cdecl s_73EE50(uint32_t a1)
	{
		const uint32_t tab[3] = { game_addr(game::MAG_095_ModelTaskA_State0_InitAtGfPos), game_addr(game::MAG_095_ModelTaskA_State1_DrawUntilPhase3), game_addr(game::MAG_095_ModelTaskA_State2_Nop) };
		callp(tab[S8(a1, 0x29)], a1);
		uint8_t b = U8(a1, 0x8C);
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24)++;
		U16(a1, 0x8C) = (uint16_t)((uint8_t)(b - 4) & 0x7F);
		return s3_task_end(a1, status);
	}

	uint32_t __cdecl s_73EEC0(uint32_t a1)
	{
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		uint32_t g = MEM<uint32_t>(game_addr(game::unk_1533010));
		uint32_t p0 = U32(g, 8);
		uint32_t p1 = U32(g, 0xC);
		U32(a1, 0x1C) = p0;
		U32(a1, 0x20) = p1;
		U16(a1, 0x20) = (uint16_t)(U16(a1, 0x20) + 0xF000);
		U16(a1, 0x1E) = 0;
		U32(a1, 0x58) = 0x1000;
		U32(a1, 0x54) = 0x1000;
		U32(a1, 0x50) = 0x1000;
		a_73C280(a1);
		U8(a1, 0x29)++;
		return 0;
	}

	uint32_t __cdecl s_73EF20(uint32_t a1)
	{
		if (a_73B7E0(3))
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
			return 0;
		}
		a_73C280(a1);
		return 0;
	}

	uint32_t __cdecl s_73EF60(uint32_t a1)
	{
		const uint32_t tab[5] = { game_addr(game::MAG_095_ModelTaskB_State0_Init), game_addr(game::MAG_095_ModelTaskB_State1_DrawUntilPhase5), game_addr(game::MAG_095_ModelTaskB_State2_FadeUntilPhase8), game_addr(game::MAG_095_ModelTaskB_State3_FadeOutFinish), game_addr(game::MAG_095_ModelTaskB_State4_Nop) };
		callp(tab[S8(a1, 0x29)], a1);
		uint8_t b = U8(a1, 0x8C);
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24)++;
		U16(a1, 0x8C) = (uint16_t)((uint8_t)(b - 1) & 0x7F);
		return s3_task_end(a1, status);
	}

	uint32_t __cdecl s_73EFE0(uint32_t a1)
	{
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		U16(a1, 0x1C) = 0;
		U16(a1, 0x1E) = 0;
		U16(a1, 0x20) = 0;
		U32(a1, 0x58) = 0x1000;
		U32(a1, 0x54) = 0x1000;
		U32(a1, 0x50) = 0x1000;
		a_73C280(a1);
		U8(a1, 0x29)++;
		return 0;
	}

	uint32_t __cdecl s_73F030(uint32_t a1)
	{
		if (a_73C280(a1) == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
			return 0;
		}
		if (a_73B7E0(5))
			U8(a1, 0x29)++;
		return 0;
	}

	uint32_t __cdecl s_73F070(uint32_t a1)
	{
		int32_t v = add32(S32(a1, 0x7C), 0x100);
		S32(a1, 0x7C) = v;
		if (v >= 0x800)
			S32(a1, 0x7C) = 0x800;
		if (a_73C280(a1) == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
			return 0;
		}
		if (a_73B7E0(8))
			U8(a1, 0x29)++;
		return 0;
	}

	uint32_t __cdecl s_73F0C0(uint32_t a1)
	{
		if (a_73C280(a1) != 0)
		{
			int32_t v = add32(S32(a1, 0x7C), 0x100);
			S32(a1, 0x7C) = v;
			if (v < 0x1000)
				return 0;
			S32(a1, 0x7C) = 0x1000;
		}
		U8(a1, 0x26) |= 1;
		U8(a1, 0x29)++;
		return 0;
	}

	uint32_t __cdecl s_73F180(uint32_t a1)
	{
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		U32(a1, 0x58) = 0x3000;
		U32(a1, 0x54) = 0x3000;
		U32(a1, 0x50) = 0x3000;
		uint8_t st = (uint8_t)(U8(a1, 0x29) + 1);
		U16(a1, 0x1C) = 0;
		U16(a1, 0x1E) = 0xF800;
		U16(a1, 0x20) = 0xCC0;
		U16(a1, 0x88) = 0xFF80;
		U8(a1, 0x29) = st;
		return 0;
	}

	uint32_t __cdecl s_73F1D0(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0x18)
			U8(a1, 0x29)++;
		return 0;
	}

	uint32_t __cdecl s_73F1E0(uint32_t a1)
	{
		if (a_73B7E0(4))
		{
			U16(a1, 0x1E) = 0xFC00;
			U32(a1, 0x58) = 0x600;
			U32(a1, 0x54) = 0x600;
			U32(a1, 0x50) = 0x600;
			U8(a1, 0x29)++;
		}
		a_73C280(a1);
		return 0;
	}

	uint32_t __cdecl s_73F220(uint32_t a1)
	{
		if (a_73C280(a1) == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl s_73F250(uint32_t a1)
	{
		const uint32_t tab[4] = { game_addr(game::MAG_095_ModelTaskD_State0_Init), game_addr(game::MAG_095_ModelTaskD_State1_WaitFrame32), game_addr(game::MAG_095_ModelTaskD_State2_FaceCameraUntilPhase5), game_addr(game::MAG_095_ModelTaskD_State3_Nop) };
		callp(tab[S8(a1, 0x29)], a1);
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24)++;
		return s3_task_end(a1, status);
	}

	uint32_t __cdecl s_73F2B0(uint32_t a1)
	{
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		U32(a1, 0x58) = 0x600;
		U32(a1, 0x54) = 0x600;
		U32(a1, 0x50) = 0x600;
		uint8_t st = (uint8_t)(U8(a1, 0x29) + 1);
		U16(a1, 0x1C) = 0;
		U16(a1, 0x1E) = 0xFC00;
		U16(a1, 0x20) = 0xCC0;
		U16(a1, 0x88) = 0xFF80;
		U8(a1, 0x29) = st;
		return 0;
	}

	uint32_t __cdecl s_73F300(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0x32)
			U8(a1, 0x29)++;
		return 0;
	}

	uint32_t __cdecl s_73F310(uint32_t a1)
	{
		uint32_t yaw = x::sub_8DD7B0(game_addr(game::unk_1D97778));
		U16(a1, 0x62) = (uint16_t)((yaw - 0x800) & 0xFFF);
		if (a_73B7E0(5) != 0 || a_73C280(a1) == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl s_73F3E0(uint32_t a1)
	{
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		U32(a1, 0x58) = 0x600;
		U32(a1, 0x54) = 0x600;
		U32(a1, 0x50) = 0x600;
		uint8_t st = (uint8_t)(U8(a1, 0x29) + 1);
		U16(a1, 0x1C) = 0;
		U16(a1, 0x1E) = 0xFC00;
		U16(a1, 0x20) = 0xCC0;
		U16(a1, 0x88) = 0xFF80;
		U8(a1, 0x29) = st;
		return 0;
	}

	uint32_t __cdecl s_73F430(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0x50)
			U8(a1, 0x29)++;
		return 0;
	}

	uint32_t __cdecl s_73F440(uint32_t a1)
	{
		uint32_t yaw = x::sub_8DD7B0(game_addr(game::unk_1D97778));
		U16(a1, 0x62) = (uint16_t)((yaw - 0x800) & 0xFFF);
		if (a_73B7E0(5))
		{
			U16(a1, 0x1C) = 0;
			U32(a1, 0x58) = 0x1000;
			U32(a1, 0x54) = 0x1000;
			U32(a1, 0x50) = 0x1000;
			uint8_t st = (uint8_t)(U8(a1, 0x29) + 1);
			U16(a1, 0x1E) = 0xF800;
			U16(a1, 0x20) = 0xCC0;
			U8(a1, 0x29) = st;
		}
		a_73C280(a1);
		return 0;
	}

	uint32_t __cdecl s_73F4A0(uint32_t a1)
	{
		uint32_t yaw = x::sub_8DD7B0(game_addr(game::unk_1D97778));
		U16(a1, 0x62) = (uint16_t)((yaw - 0x800) & 0xFFF);
		if (a_73B7E0(6) != 0 || a_73C280(a1) == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl s_73F570(uint32_t a1)
	{
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		U32(a1, 0x58) = 0x1000;
		U32(a1, 0x54) = 0x1000;
		U32(a1, 0x50) = 0x1000;
		uint8_t st = (uint8_t)(U8(a1, 0x29) + 1);
		U16(a1, 0x1C) = 0;
		U16(a1, 0x1E) = 0xF800;
		U16(a1, 0x20) = 0xCC0;
		U16(a1, 0x88) = 0xFF80;
		U8(a1, 0x29) = st;
		return 0;
	}

	uint32_t __cdecl s_73F5C0(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0x82)
			U8(a1, 0x29)++;
		return 0;
	}

	uint32_t __cdecl s_73F5D0(uint32_t a1)
	{
		uint32_t yaw = x::sub_8DD7B0(game_addr(game::unk_1D97778));
		U16(a1, 0x62) = (uint16_t)((yaw - 0x800) & 0xFFF);
		if (a_73B7E0(6))
		{
			uint32_t g = MEM<uint32_t>(game_addr(game::unk_1533010));
			uint32_t p0 = U32(g, 8);
			uint32_t p1 = U32(g, 0xC);
			U32(a1, 0x1C) = p0;
			U32(a1, 0x58) = 0x2000;
			U32(a1, 0x54) = 0x2000;
			U32(a1, 0x50) = 0x2000;
			uint8_t st = (uint8_t)(U8(a1, 0x29) + 1);
			U32(a1, 0x20) = p1;
			U16(a1, 0x1E) = 0xFA00;
			U8(a1, 0x29) = st;
		}
		a_73C280(a1);
		return 0;
	}

	uint32_t __cdecl s_73F640(uint32_t a1)
	{
		uint32_t yaw = x::sub_8DD7B0(game_addr(game::unk_1D97778));
		U16(a1, 0x62) = (uint16_t)((yaw - 0x800) & 0xFFF);
		if (a_73B7E0(9) != 0 || a_73C280(a1) == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl s_73F6A0(uint32_t a1)
	{
		const uint32_t tab[6] = { game_addr(game::MAG_095_ModelTaskG_State0_Init), game_addr(game::MAG_095_ModelTaskG_State1_WaitFrame1E), game_addr(game::MAG_095_ModelTaskG_State2_DrawUntilEnd), game_addr(game::MAG_095_ModelTaskG_State3_RestartAtFrame89), game_addr(game::MAG_095_ModelTaskG_State4_DrawUntilPhase6), game_addr(game::MAG_095_ModelTaskG_State5_Nop) };
		callp(tab[S8(a1, 0x29)], a1);
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24)++;
		return s3_task_end(a1, status);
	}

	uint32_t __cdecl s_73F710(uint32_t a1)
	{
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		U16(a1, 0x1C) = 0;
		U16(a1, 0x1E) = 0;
		U16(a1, 0x20) = 0;
		U32(a1, 0x58) = 0x1000;
		U32(a1, 0x54) = 0x1000;
		U32(a1, 0x50) = 0x1000;
		U8(a1, 0x29)++;
		return 0;
	}

	uint32_t __cdecl s_73F760(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0x1E)
			U8(a1, 0x29)++;
		return 0;
	}

	uint32_t __cdecl s_73F770(uint32_t a1)
	{
		if (a_73C280(a1) == 0)
			U8(a1, 0x29)++;
		return 0;
	}

	uint32_t __cdecl s_73F790(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0x89)
		{
			x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl s_73F7C0(uint32_t a1)
	{
		if (a_73C280(a1) == 0 || a_73B7E0(6) != 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl s_73F860(uint32_t a1)
	{
		uint32_t slot = U8(a1, 0x2D);
		uint32_t ent = game_addr(game::BattleEntitySlotData) + slot * 0x9C;
		x::Effect_DecodeModelPrimLayout(U32(a1, 0x74), a1 + 0x94, U32(a1, 0x78));
		x::GetEffectSpawnPosition(ent, 0xF1, 0, a1 + 0x1C);
		U32(a1, 0x58) = 0x1000;
		U32(a1, 0x54) = 0x1000;
		U32(a1, 0x50) = 0x1000;
		a_73C280(a1);
		U8(a1, 0x29)++;
		return 0;
	}

	uint32_t __cdecl s_73F8D0(uint32_t a1)
	{
		if (a_73C280(a1) == 0)
		{
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl s_73F960(uint32_t a1)
	{
		if (S16(a1, 0x24) >= S16(a1, 0x298))
		{
			a_73F990(a1);
			s_740470(a1);
			U8(a1, 0x29)++;
		}
		return 0;
	}

	uint32_t __cdecl s_740470(uint32_t a1)
	{
		uint32_t sys = a1 + 0x34;
		uint32_t done = 0;
		MEM<uint32_t>(game_addr(game::MAG_095_ActorStatePtr)) = sys;
		uint16_t phase = U16(sys, 0x20);
		switch ((int16_t)phase)
		{
		case 0:
			U16(sys, 0x20) = (uint16_t)(phase + 1);
			break;
		case 1:
			a_743190();
			s_740840();
			a_740700();
			s_740500();
			sys = MEM<uint32_t>(game_addr(game::MAG_095_ActorStatePtr));
			U16(sys, 0x18)++;
			if (S16(sys, 0x18) >= S16(sys, 0x1A) && U16(sys, 0x14) == 0 && U16(sys, 0x16) == 0)
				U16(sys, 0x20)++;
			break;
		case 2:
			done = 1;
			break;
		default:
			break;
		}
		uint16_t n0 = U16(sys, 0x14);
		uint16_t n1 = U16(sys, 0x16);
		MEM<uint16_t>(game_addr(game::MAG_095_LiveCountA)) = (uint16_t)(MEM<uint16_t>(game_addr(game::MAG_095_LiveCountA)) + n0);
		MEM<uint16_t>(game_addr(game::MAG_095_LiveCountB)) = (uint16_t)(MEM<uint16_t>(game_addr(game::MAG_095_LiveCountB)) + n1);
		return done;
	}

	uint32_t __cdecl s_740500(void)
	{
		uint32_t e = U32(MEM<uint32_t>(game_addr(game::MAG_095_ActorStatePtr)), 0x2C);
		if (e == 0)
			return 0;
		do
		{
			if (U16(e, 8) == 1)
			{
				uint32_t sys = MEM<uint32_t>(game_addr(game::MAG_095_ActorStatePtr));
				int32_t k = S8(e, 0x1D6);
				uint32_t desc = U32(U32(sys, 0x224), k * 4);
				uint8_t kind = U8(desc, 0x16);
				if ((kind == 4 || kind == 5) && U32(e, 0x170) != 0)
					callo(game_addr(game::MAG_095_Emitter_SetupSequence), e, desc);
			}
			e = U32(e, 4);
		} while (e != 0);
		return 0;
	}

	uint32_t __cdecl s_740840(void)
	{
		uint32_t e = U32(MEM<uint32_t>(game_addr(game::MAG_095_ActorStatePtr)), 0x2C);
		if (e == 0)
			return 0;
		do
		{
			int32_t type = S16(e, 8);
			if (type == 0)
				s_741B10(e);
			else if (type == 1)
				a_740880(e);
			e = U32(e, 4);
		} while (e != 0);
		return 0;
	}

	uint32_t __cdecl s_741B10(uint32_t a1)
	{
		if (U16(a1, 0x62) == 0)
		{
			a_742D00(a1);
			U16(a1, 0x62)++;
		}
		int32_t mode = S8(a1, 0x68);
		if (mode == 3)
			a_7424C0(a1);
		else if (mode == 4)
			callo(game_addr(game::MAG_095_Actor_InterpolatePosition), a1);
		s_741B90(a1);
		U16(a1, 0x60)++;
		return a_741B60(a1);
	}

	uint32_t __cdecl s_741B90(uint32_t a1)
	{
		uint32_t sys = MEM<uint32_t>(game_addr(game::MAG_095_ActorStatePtr));
		uint32_t table = U32(sys, 0x224);
		int32_t k = S8(a1, 0x6A);
		uint32_t desc = U32(table, k * 4);
		int16_t age = S16(a1, 0x60);
		if (age > 0x27)
			return 0;
		int32_t count = U8(U32(desc, 0xC8), (int32_t)age);
		if (count == 0)
			return 0;
		int32_t batch = S8(desc, 0x36);
		int32_t q = count / batch;
		int32_t r = count % batch;
		if (q > 0)
		{
			do
			{
				s_741C10(a1, desc, (uint32_t)(int32_t)S8(desc, 0x36));
			} while (--q);
		}
		if (r > 0)
			s_741C10(a1, desc, (uint32_t)r);
		return 0;
	}

	namespace
	{
		inline void s4_copy16(uint32_t dst, uint32_t src)
		{
			U32(dst, 0) = U32(src, 0);
			U32(dst, 4) = U32(src, 4);
			U32(dst, 8) = U32(src, 8);
			U32(dst, 12) = U32(src, 12);
		}

		inline void s4_rotate_zxy(uint32_t m, uint32_t ang)
		{
			x::MAG_022_sub_8DD770(m);
			if (U16(ang, 4) != 0) x::sub_8DD960(m, (uint32_t)(int32_t)S16(ang, 4));
			if (U16(ang, 0) != 0) x::sub_8DD7E0(m, (uint32_t)(int32_t)S16(ang, 0));
			if (U16(ang, 2) != 0) x::MAG_022_sub_8DD8A0(m, (uint32_t)(int32_t)S16(ang, 2));
		}

		inline void s4_rotate_down_vector(uint32_t sp)
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

		inline void s4_rand_mod(uint32_t sp, int32_t off)
		{
			if (U16(sp, off) != 0)
			{
				int32_t r = (int32_t)x::CrtRand();
				U16(sp, off) = (uint16_t)(r % (int32_t)S16(sp, off));
			}
		}

		inline void s4_orbit_matrix(uint32_t m, int32_t ty_bias, int32_t tz_bias)
		{
			x::MAG_022_sub_8DD770(m);
			x::MAG_022_sub_8DD8A0(m, 0x800);
			x::scale3DMatrix(m, game_addr(game::unk_258EC60));
			int32_t tx = S16(game_addr(game::unk_258FB80), 0);
			int32_t ty = sub32(S16(game_addr(game::unk_258FB82), 0), ty_bias);
			uint32_t root = U32(game_addr(game::unk_1533010), 0);
			S32(m, 0x14) = tx;
			S32(m, 0x18) = ty;
			int32_t tz = sub32(add32(S16(root, 0x0C), S16(game_addr(game::unk_258FB84), 0)), tz_bias);
			S32(m, 0x1C) = tz;
			x::GTE_SetRotMatrix_W(m);
			x::GTE_SetTransVector_W(m);
		}

		inline uint16_t s4_key_lerp(uint32_t tab, int32_t k8, int32_t comp, int32_t t)
		{
			uint16_t a = U16(tab + comp, k8);
			int32_t d = sub32(S16(tab + comp + 8, k8), (int16_t)a);
			int32_t q = mul32(d, t) / 16;
			return (uint16_t)(q + a);
		}

		uint32_t s4_orbit_sprites_task(uint32_t node, uint32_t keys, int32_t ty_bias, uint32_t table_hi)
		{
			uint8_t m[0x20];
			s4_orbit_matrix(P(m), ty_bias, 0x2000);
			uint32_t pt = node + 0xD4;
			uint32_t ob = node + 0xAE;
			for (int32_t n = 4; n != 0; n--)
			{
				int32_t k8 = shl32(S16(pt, -8), 3);
				int32_t t = S16(pt, 0);
				int16_t v[4];
				v[3] = (int16_t)table_hi;
				uint16_t vx = s4_key_lerp(keys, k8, 0, t);
				vx = (uint16_t)(vx + U16(ob, -2));
				uint16_t vy = s4_key_lerp(keys, k8, 2, t);
				vy = (uint16_t)(vy + U16(ob, 0));
				uint16_t vz = s4_key_lerp(keys, k8, 4, t);
				vz = (uint16_t)(vz + U16(ob, 2));
				v[0] = (int16_t)vx;
				v[1] = (int16_t)vy;
				v[2] = (int16_t)vz;
				x::GTE_LoadV0(P(v));
				x::GTE_MVMVA_RotV0_Tr_2();
				x::GTE_StoreIR123(ob - 0x42);
				pt += 2;
				ob += 8;
			}
			uint32_t pos = node + 0x6C;
			for (int32_t n = 4; n != 0; n--)
			{
				U32(node, 0x1C) = U32(pos, 0);
				U32(node, 0x20) = U32(pos, 4);
				s_7434C0(node);
				pos += 8;
			}
			uint8_t status = U8(node, 0x26);
			U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
			if ((status & 1) != 0 && U8(node, 0x28) == 0)
			{
				x::Effect_ReleaseLinkedTask(node);
				return 2;
			}
			return 0;
		}

		uint32_t s4_state_task(uint32_t node, const uint32_t *tab)
		{
			callp(tab[S8(node, 0x29)], node);
			uint8_t status = U8(node, 0x26);
			U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
			if ((status & 1) != 0 && U8(node, 0x28) == 0)
			{
				x::Effect_ReleaseLinkedTask(node);
				return 2;
			}
			return 0;
		}

		inline void s4_anchor_to_root(uint32_t node, uint16_t dz)
		{
			uint32_t root = U32(game_addr(game::unk_1533010), 0);
			uint32_t d0 = U32(root, 8);
			uint32_t d1 = U32(root, 0xC);
			U32(node, 0x1C) = d0;
			U32(node, 0x20) = d1;
			uint8_t st = U8(node, 0x29);
			U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + dz);
			U16(node, 0x1E) = 0;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}

		void s4_fill_orbit_keys(uint32_t node)
		{
			uint32_t po = node + 0xB0;
			uint32_t pk = node + 0xD4;
			for (int32_t n = 4; n != 0; n--)
			{
				int32_t r = (int32_t)x::CrtRand();
				U16(pk, -8) = (uint16_t)(r % 16);
				U16(pk, 0) = (uint16_t)(x::CrtRand() & 0xF);
				int32_t ang = (int16_t)(uint16_t)(x::CrtRand() & 0xFFF);
				int32_t s = (int32_t)x::computeSin((uint32_t)ang);
				U16(po, -2) = (uint16_t)(s / 32);
				int32_t c = (int32_t)x::computeCosine((uint32_t)ang);
				pk += 2;
				U16(po, 0) = (uint16_t)(c / 32);
				po += 8;
			}
		}

		inline uint32_t s4_entity_of(uint32_t node)
		{
			uint32_t slot = U8(node, 0x2D);
			return game_addr(game::BattleEntitySlotData) + slot * 0x9C;
		}
	}

	uint32_t __cdecl s_741C10(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		uint32_t sp = U32(game_addr(game::MAG_095_ScratchSP), 0) - 0x50;
		int16_t kind = (int16_t)S8(a1, 0x6A);
		U32(game_addr(game::MAG_095_ScratchSP), 0) = sp;
		uint32_t node = s_7423F0(a1, (uint16_t)kind);
		uint32_t desc = a2;
		uint8_t a3_lo = (uint8_t)a3;
		uint16_t w60 = U16(a1, 0x60);
		uint8_t mode = U8(desc, 0x16);
		U8(node, 0x1D8) = a3_lo;
		U16(sp, 0x4E) = w60;
		if (mode == 0 || mode == 1 || mode == 2)
		{
			uint32_t ctx = U32(game_addr(game::MAG_095_ActorStatePtr), 0);
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
				s4_copy16(pos - 0x50, src);
				if (U8(desc, 0x22) == 0)
					s4_copy16(pos, src);
				uint8_t spread = U8(desc, 0x34);
				if (spread == 1)
				{
					U16(sp, 0x28) = (uint16_t)(x::CrtRand() & 0xFFF);
					U16(sp, 0x2A) = (uint16_t)(x::CrtRand() & 0xFFF);
					U16(sp, 0x2C) = (uint16_t)(x::CrtRand() & 0xFFF);
					s4_rotate_zxy(sp, sp + 0x28);
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
						s4_rand_mod(sp, 0x48);
						s4_rand_mod(sp, 0x4A);
						s4_rand_mod(sp, 0x4C);
					}
					s4_rotate_down_vector(sp);
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
					s4_rotate_zxy(sp, sp + 0x28);
					int32_t i2 = shl32(S16(sp, 0x4E), 1);
					if (S8(desc, 0x33) != 1)
					{
						U16(sp, 0x48) = U16(U32(desc, 0xCC), i2);
						U16(sp, 0x4A) = U16(U32(desc, 0xD0), i2);
						U16(sp, 0x4C) = U16(U32(desc, 0xD4), i2);
						s4_rand_mod(sp, 0x48);
						s4_rand_mod(sp, 0x4A);
						s4_rand_mod(sp, 0x4C);
					}
					else
					{
						U16(sp, 0x48) = U16(U32(desc, 0xCC), i2);
						U16(sp, 0x4A) = U16(U32(desc, 0xD0), i2);
						s4_rand_mod(sp, 0x4A);
						int32_t e = S16(sp, 0x4E);
						U16(sp, 0x4C) = U16(U32(desc, 0xD4), shl32(e, 1));
					}
					if ((x::CrtRand() & 1) != 0)
						U16(sp, 0x4A) = (uint16_t)(0 - U16(sp, 0x4A));
					s4_rotate_down_vector(sp);
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
				s4_rotate_zxy(mat, sp + 0x30);
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
		U32(game_addr(game::MAG_095_ScratchSP), 0) = U32(game_addr(game::MAG_095_ScratchSP), 0) + 0x50;
		return 0;
	}

	uint32_t __cdecl s_7423F0(uint32_t a1, uint32_t a2)
	{
		uint32_t pool = U32(game_addr(game::MAG_095_ActorPoolPtr), 0);
		int32_t cur = S16(game_addr(game::MAG_095_ActorCursor), 0);
		uint32_t rec = 0;
		int32_t tries = 0;
		for (;;)
		{
			if (U8(pool + (uint32_t)mul32(cur, 0x2A0), 0x1D7) == 0)
			{
				rec = (uint32_t)mul32(cur, 0x2A0) + pool;
				x::MAG_007_sub_8DCC00(rec, 0x2A0);
				uint8_t kind = (uint8_t)a2;
				U32(rec, 0x1BC) = a1;
				uint8_t b6b = U8(a1, 0x6B);
				uint32_t ctx = U32(game_addr(game::MAG_095_ActorStatePtr), 0);
				U8(rec, 0x1D7) = 1;
				U16(ctx, 0x16) = (uint16_t)(U16(ctx, 0x16) + 1);
				U8(rec, 0x1D6) = kind;
				U8(rec, 0x1D9) = b6b;
				a_742CA0(rec, 1);
				break;
			}
			cur++;
			if (cur >= 0x49)
				cur = 0;
			tries++;
			if (tries >= 0x4A)
				break;
		}
		cur++;
		if (cur < 0x49)
			U16(game_addr(game::MAG_095_ActorCursor), 0) = (uint16_t)cur;
		else
			U16(game_addr(game::MAG_095_ActorCursor), 0) = 0;
		return rec;
	}

	uint32_t __cdecl s_743220(uint32_t a1)
	{
		if (s_740470(a1) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl s_7432C0(uint32_t a1)
	{
		uint32_t ent = s4_entity_of(a1);
		uint32_t node = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_BoxSparkleTask), 0xE0, a1);
		uint32_t p = node + 0x6E;
		for (int32_t n = 4; n != 0; n--)
		{
			uint16_t lo = U16(ent, 0x34);
			int32_t r = (int32_t)x::CrtRand();
			int32_t q = mul32(r & 0xFFF, sub32(S16(ent, 0x3A), (int16_t)lo)) / 4096;
			lo = (uint16_t)(q + lo);
			uint16_t lo_y = U16(ent, 0x36);
			U16(p, -2) = (uint16_t)(lo + U16(ent, 0x1C));
			r = (int32_t)x::CrtRand();
			q = mul32(r & 0xFFF, sub32(S16(ent, 0x3C), (int16_t)lo_y)) / 4096;
			lo_y = (uint16_t)(q + lo_y);
			uint16_t lo_z = U16(ent, 0x38);
			U16(p, 0) = (uint16_t)(lo_y + U16(ent, 0x1E));
			r = (int32_t)x::CrtRand();
			q = mul32(r & 0xFFF, sub32(S16(ent, 0x3E), (int16_t)lo_z)) / 4096;
			p += 8;
			lo_z = (uint16_t)(q + lo_z);
			U16(p, -6) = (uint16_t)(lo_z + U16(ent, 0x20));
		}
		if (a_73B7E0(7) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
			return a1;
		}
		return 0;
	}

	uint32_t __cdecl s_7433C0(uint32_t a1)
	{
		uint32_t tab[3] = { game_addr(game::MAG_095_BoxSparkle_State0_PickSprite), game_addr(game::MAG_095_BoxSparkle_State1_CounterFinish), game_addr(game::MAG_095_BoxSparkle_State2_Nop) };
		callp(tab[S8(a1, 0x29)], a1);
		uint32_t v = a1 + 0x8E;
		for (int32_t n = 4; n != 0; n--)
		{
			int16_t c = S16(v, -2);
			U16(v, -2) = (uint16_t)((uint16_t)c - (int32_t)c / 4);
			c = S16(v, 0);
			U16(v, 0) = (uint16_t)((uint16_t)c - (int32_t)c / 4);
			c = S16(v, 2);
			U16(v, 2) = (uint16_t)((uint16_t)c - (int32_t)c / 4);
			U16(v, -0x22) = (uint16_t)(U16(v, -0x22) + (int32_t)S16(v, -2) / 16);
			U16(v, -0x20) = (uint16_t)(U16(v, -0x20) + (int32_t)S16(v, 0) / 16);
			U16(v, -0x1E) = (uint16_t)(U16(v, -0x1E) + (int32_t)S16(v, 2) / 16);
			U32(a1, 0x1C) = U32(v, -0x22);
			U32(a1, 0x20) = U32(v, -0x1E);
			s_7434C0(a1);
			v += 8;
		}
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		if ((status & 1) != 0 && U8(a1, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(a1);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl s_7434C0(uint32_t a1)
	{
		if ((U8(a1, 0x26) & 4) != 0)
			return 0;
		uint32_t hdr = x::Field_Alloc(0xD8);
		uint32_t m = hdr + 0xB8;
		x::UnpackRotationMatrix(game_addr(game::unk_1D97778), m);
		int32_t px = S16(a1, 0x1C);
		int32_t py = S16(a1, 0x1E);
		int32_t pz = S16(a1, 0x20);
		uint32_t vec = hdr + 0xCC;
		S32(hdr, 0xD0) = py;
		S32(hdr, 0xD4) = pz;
		S32(vec, 0) = px;
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(m);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(m);
		x::GTE_LoadIRFromMatrixColumn(hdr + 0xBA);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(hdr + 0xBA);
		x::GTE_LoadIRFromMatrixColumn(hdr + 0xBC);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(hdr + 0xBC);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(vec);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(vec);
		x::GTE_SetRotMatrix(m);
		x::GTE_SetTransVector(m);
		uint32_t data = U32(a1, 0x4C);
		uint16_t frame = U16(a1, 0x50);
		uint16_t depth = U16(a1, 0x56);
		U32(hdr, 0) = data;
		uint32_t cursor = U32(game_addr(game::BATTLE_PACKET_WRITE_CURSOR), 0);
		U16(hdr, 4) = frame;
		uint32_t ot = U32(game_addr(game::g_Battle_FrameRenderListBase), 0) + 0x44;
		U16(hdr, 0x24) = 0;
		U16(hdr, 0xB4) = depth;
		U32(game_addr(game::BATTLE_PACKET_WRITE_CURSOR), 0) = a_7435E0(hdr, ot, 2, cursor);
		x::Field_Free(0xD8);
		return 0;
	}

	uint32_t __cdecl s_743B90(uint32_t a1)
	{
		U16(a1, 0x56) = 0xFFF0;
		int32_t r = (int32_t)x::CrtRand();
		int32_t pick = r % 3;
		if (pick == 0)
			U32(a1, 0x4C) = game_addr(game::unk_15331D0);
		else if (pick == 1)
		{
			uint8_t st = U8(a1, 0x29);
			U32(a1, 0x4C) = game_addr(game::unk_1533328);
			U16(a1, 0x52) = 0xA;
			U8(a1, 0x29) = (uint8_t)(st + 1);
			return 0;
		}
		else if (pick == 2)
		{
			uint8_t st = U8(a1, 0x29);
			U32(a1, 0x4C) = game_addr(game::unk_15334A0);
			U16(a1, 0x52) = 0xA;
			U8(a1, 0x29) = (uint8_t)(st + 1);
			return 0;
		}
		uint8_t st = U8(a1, 0x29);
		U16(a1, 0x52) = 0xA;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl s_743C70(uint32_t a1)
	{
		uint32_t tab[4] = { game_addr(game::MAG_095_OrbitSparkleSpawner_State0_Anchor), game_addr(game::MAG_095_OrbitSparkleSpawner_State1_Advance), game_addr(game::MAG_095_OrbitSparkleSpawner_State2_SpawnOddFrames), game_addr(game::MAG_095_OrbitSparkleSpawner_State3_Nop) };
		return s4_state_task(a1, tab);
	}

	uint32_t __cdecl s_743CD0(uint32_t a1)
	{
		s4_anchor_to_root(a1, 0xE000);
		return a1;
	}

	uint32_t __cdecl s_743D10(uint32_t a1)
	{
		if ((U8(a1, 0x24) & 1) != 0)
		{
			uint32_t node = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_OrbitSparkleTask), 0xE0, a1);
			s4_fill_orbit_keys(node);
		}
		if (a_73B7E0(2) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl s_743DD0(uint32_t a1)
	{
		uint32_t tab[3] = { game_addr(game::MAG_095_OrbitSparkle_State0_Init), game_addr(game::MAG_095_OrbitSparkle_State1_UntilEndOrPhase2), game_addr(game::MAG_095_OrbitSparkle_State2_Nop) };
		callp(tab[S8(a1, 0x29)], a1);
		return s4_orbit_sprites_task(a1, game_addr(game::MAG_095_KeyTableA), 0x80, game_addr(game::MAG_095_OrbitSparkle_State1_UntilEndOrPhase2) >> 16);
	}

	uint32_t __cdecl s_743FD0(uint32_t a1)
	{
		U32(a1, 0x4C) = game_addr(game::unk_1533024);
		U16(a1, 0x56) = 0xFFD0;
		uint32_t r = x::CrtRand() & 7;
		U16(a1, 0x50) = (uint16_t)r;
		U16(a1, 0x52) = 0xF;
		if (a_73B7E0(2) != 0)
			U8(a1, 0x26) |= 4;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_744010(uint32_t a1)
	{
		if (a_743C20(a1) != 0 || a_73B7E0(2) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 5;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl s_744060(uint32_t a1)
	{
		uint32_t tab[4] = { game_addr(game::MAG_095_BurstSparkleSpawner_State0_Anchor), game_addr(game::MAG_095_BurstSparkleSpawner_State1_Advance), game_addr(game::MAG_095_BurstSparkleSpawner_State2_Spawn5), game_addr(game::MAG_095_BurstSparkleSpawner_State3_Nop) };
		return s4_state_task(a1, tab);
	}

	uint32_t __cdecl s_7440C0(uint32_t a1)
	{
		s4_anchor_to_root(a1, 0xE000);
		return a1;
	}

	uint32_t __cdecl s_744100(uint32_t a1)
	{
		for (int32_t n = 5; n != 0; n--)
		{
			uint32_t node = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_BurstSparkleTask), 0xE0, a1);
			s4_fill_orbit_keys(node);
		}
		if (a_73B7E0(2) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
			return a1;
		}
		return 0;
	}

	uint32_t __cdecl s_7441D0(uint32_t a1)
	{
		uint32_t tab[3] = { game_addr(game::MAG_095_BurstSparkle_State0_Init), game_addr(game::MAG_095_BurstSparkle_State1_UntilEndOrPhase2), game_addr(game::MAG_095_BurstSparkle_State2_Nop) };
		callp(tab[S8(a1, 0x29)], a1);
		return s4_orbit_sprites_task(a1, game_addr(game::MAG_095_KeyTableB), 0, game_addr(game::MAG_095_BurstSparkle_State1_UntilEndOrPhase2) >> 16);
	}

	uint32_t __cdecl s_7443C0(uint32_t a1)
	{
		U32(a1, 0x4C) = game_addr(game::unk_1533024);
		uint32_t r = x::CrtRand() & 7;
		U16(a1, 0x50) = (uint16_t)r;
		U16(a1, 0x56) = 0xFFC0;
		U16(a1, 0x52) = 0xF;
		if (a_73B7E0(2) != 0)
			U8(a1, 0x26) |= 4;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_744400(uint32_t a1)
	{
		if (a_743C20(a1) != 0 || a_73B7E0(2) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 5;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl s_744450(uint32_t a1)
	{
		uint32_t tab[4] = { game_addr(game::MAG_095_KeyPathSparkleSpawner_State0_Anchor), game_addr(game::MAG_095_KeyPathSparkleSpawner_State1_Advance), game_addr(game::MAG_095_KeyPathSparkleSpawner_State2_Spawn3), game_addr(game::MAG_095_KeyPathSparkleSpawner_State3_Nop) };
		return s4_state_task(a1, tab);
	}

	uint32_t __cdecl s_7444B0(uint32_t a1)
	{
		s4_anchor_to_root(a1, 0xF000);
		return a1;
	}

	uint32_t __cdecl s_7444F0(uint32_t a1)
	{
		for (int32_t k = 3; k != 0; k--)
		{
			uint32_t node = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_KeyPathSparkleTask), 0xE0, a1);
			uint32_t pk = node + 0xD4;
			for (int32_t n = 4; n != 0; n--)
			{
				int32_t r = (int32_t)x::CrtRand();
				U16(pk, -8) = (uint16_t)(r % 0x18);
				U16(pk, 0) = (uint16_t)(x::CrtRand() & 0xF);
				pk += 2;
			}
		}
		if (a_73B7E0(3) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl s_744570(uint32_t a1)
	{
		uint32_t tab[3] = { game_addr(game::MAG_095_KeyPathSparkle_State0_Init), game_addr(game::MAG_095_KeyPathSparkle_State1_UntilEndOrPhase3), game_addr(game::MAG_095_KeyPathSparkle_State2_Nop) };
		callp(tab[S8(a1, 0x29)], a1);
		uint8_t m[0x20];
		s4_orbit_matrix(P(m), 0, 0x1000);
		uint32_t out = a1 + 0x6C;
		uint32_t pt = a1 + 0xD4;
		for (int32_t n = 4; n != 0; n--)
		{
			int32_t k8 = shl32(S16(pt, -8), 3);
			int32_t t = S16(pt, 0);
			int16_t v[4];
			v[3] = (int16_t)(game_addr(game::MAG_095_KeyPathSparkle_State1_UntilEndOrPhase3) >> 16);
			v[0] = (int16_t)s4_key_lerp(game_addr(game::MAG_095_KeyTable25), k8, 0, t);
			v[1] = (int16_t)s4_key_lerp(game_addr(game::MAG_095_KeyTable25), k8, 2, t);
			v[2] = (int16_t)s4_key_lerp(game_addr(game::MAG_095_KeyTable25), k8, 4, t);
			x::GTE_LoadV0(P(v));
			x::GTE_MVMVA_RotV0_Tr_2();
			x::GTE_StoreIR123(out);
			pt += 2;
			out += 8;
		}
		uint32_t pos = a1 + 0x6C;
		for (int32_t n = 4; n != 0; n--)
		{
			U32(a1, 0x1C) = U32(pos, 0);
			U32(a1, 0x20) = U32(pos, 4);
			s_7434C0(a1);
			pos += 8;
		}
		uint8_t status = U8(a1, 0x26);
		U16(a1, 0x24) = (uint16_t)(U16(a1, 0x24) + 1);
		if ((status & 1) != 0 && U8(a1, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(a1);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl s_744730(uint32_t a1)
	{
		U32(a1, 0x4C) = game_addr(game::unk_1533024);
		U16(a1, 0x56) = 0xFFF0;
		uint32_t r = x::CrtRand() & 7;
		U16(a1, 0x50) = (uint16_t)r;
		U16(a1, 0x52) = 0xF;
		if (a_73B7E0(3) != 0)
			U8(a1, 0x26) |= 4;
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_744770(uint32_t a1)
	{
		if (a_743C20(a1) != 0 || a_73B7E0(3) != 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 5;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl s_744820(uint32_t a1)
	{
		uint32_t ent = s4_entity_of(a1);
		if (S16(ent, 0x3C) <= (int16_t)0xFE00)
		{
			U8(a1, 0x26) |= 1;
			return 0;
		}
		if (S16(ent, 0x20) > S16(game_addr(game::unk_258FB64), 0))
			return 0;
		if ((U8(ent, 8) & 2) == 0)
		{
			int32_t r = (int32_t)x::CrtRand();
			int32_t pick = r % 3;
			if (pick == 0)
				x::queueChainTransformationConditional(ent, 4);
			else if (pick == 1)
				x::queueChainTransformationConditional(ent, 5);
			else if (pick == 2)
				x::queueChainTransformationConditional(ent, 6);
		}
		uint8_t st = U8(a1, 0x29);
		U16(a1, 0xDC) = 8;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl s_7448A0(uint32_t a1)
	{
		if ((U8(a1, 0x24) & 1) != 0)
		{
			uint32_t ent = s4_entity_of(a1);
			uint32_t node = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_RisingSparkleTask), 0xE0, a1);
			uint32_t p = node + 0x6E;
			for (int32_t n = 4; n != 0; n--)
			{
				uint16_t lo = U16(ent, 0x34);
				int32_t r = (int32_t)x::CrtRand();
				int32_t q = mul32(r & 0xFFF, sub32(S16(ent, 0x3A), (int16_t)lo)) / 4096;
				U16(p, -2) = (uint16_t)(q + lo + U16(ent, 0x1C));
				r = (int32_t)x::CrtRand();
				uint16_t lo_z = U16(ent, 0x38);
				U16(p, 0) = (uint16_t)(0 - (r & 0x1FF));
				r = (int32_t)x::CrtRand();
				q = mul32(r & 0xFFF, sub32(S16(ent, 0x3E), (int16_t)lo_z)) / 4096;
				U16(p, 2) = (uint16_t)(q + U16(ent, 0x20) + lo_z);
				r = (int32_t)x::CrtRand();
				p += 8;
				U16(p, 0x18) = (uint16_t)(0 - (r & 0x3FF));
			}
		}
		{
			uint32_t ent = s4_entity_of(a1);
			uint32_t node = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_SplashSparkleTask), 0xE0, a1);
			uint32_t p = node + 0x6E;
			for (int32_t n = 4; n != 0; n--)
			{
				uint16_t lo = U16(ent, 0x34);
				int32_t r = (int32_t)x::CrtRand();
				int32_t q = mul32(r & 0xFFF, sub32(S16(ent, 0x3A), (int16_t)lo)) / 4096;
				U16(p, -2) = (uint16_t)(q + U16(ent, 0x1C) + lo);
				r = (int32_t)x::CrtRand();
				uint16_t lo_z = U16(ent, 0x38);
				U16(p, 0) = (uint16_t)(0 - (r & 0x1FF));
				r = (int32_t)x::CrtRand();
				q = mul32(r & 0xFFF, sub32(S16(ent, 0x3E), (int16_t)lo_z)) / 4096;
				U16(p, 2) = (uint16_t)(q + U16(ent, 0x20) + lo_z);
				r = (int32_t)x::CrtRand();
				U16(p, 0x1E) = (uint16_t)((r & 0x7FF) - 0x400);
				r = (int32_t)x::CrtRand();
				U16(p, 0x22) = (uint16_t)((r & 0x7FF) - 0x400);
				r = (int32_t)x::CrtRand();
				U16(p, 0x20) = (uint16_t)(0xFFFFFA00u - (uint32_t)(r & 0x7FF));
				p += 8;
			}
		}
		U16(a1, 0xDC) = (uint16_t)(U16(a1, 0xDC) - 1);
		if (a_73B7E0(3) != 0 || S16(a1, 0xDC) <= 0)
		{
			uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	namespace
	{
		inline uint32_t s5_task_tail(uint32_t node)
		{
			uint8_t status = U8(node, 0x26);
			U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
			if ((status & 1) != 0 && U8(node, 0x28) == 0)
			{
				x::Effect_ReleaseLinkedTask(node);
				return 2;
			}
			return 0;
		}

		inline void s5_next_state(uint32_t node, uint8_t flags)
		{
			U8(node, 0x26) = (uint8_t)(U8(node, 0x26) | flags);
			S8(node, 0x29) = (int8_t)(S8(node, 0x29) + 1);
		}
	}

	uint32_t __cdecl s_744AA0(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t states[3] = { game_addr(game::MAG_095_RisingSparkle_State0_Init), game_addr(game::MAG_095_RisingSparkle_State1_Play), game_addr(game::MAG_095_RisingSparkle_State2_Nop) };
		callp(states[S8(node, 0x29)], node);
		uint32_t p = node + 0x8E;
		for (int n = 4; n != 0; --n)
		{
			int16_t v = S16(p, -2);
			U16(p, -2) = (uint16_t)(v - (int16_t)(v / 4));
			v = S16(p, 0);
			U16(p, 0) = (uint16_t)(v - (int16_t)(v / 4));
			v = S16(p, 2);
			U16(p, 2) = (uint16_t)(v - (int16_t)(v / 4));
			U16(p, -0x22) = (uint16_t)(U16(p, -0x22) + (uint16_t)((int32_t)S16(p, -2) / 16));
			U16(p, -0x20) = (uint16_t)(U16(p, -0x20) + (uint16_t)((int32_t)S16(p, 0) / 16));
			U16(p, -0x1E) = (uint16_t)(U16(p, -0x1E) + (uint16_t)((int32_t)S16(p, 2) / 16));
			U32(node, 0x1C) = U32(p, -0x22);
			U32(node, 0x20) = U32(p, -0x1E);
			s_7434C0(node);
			p += 8;
		}
		return s5_task_tail(node);
	}

	uint32_t __cdecl s_744BA0(uint32_t a1)
	{
		uint32_t node = a1;
		U32(node, 0x4C) = game_addr(game::unk_1533024);
		U16(node, 0x56) = 0xFFE0;
		U16(node, 0x52) = 0xF;
		if (a_73B7E0(3) != 0)
			U8(node, 0x26) = (uint8_t)(U8(node, 0x26) | 4);
		S8(node, 0x29) = (int8_t)(S8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_744BE0(uint32_t a1)
	{
		if (a_73B7E0(3) != 0)
		{
			s5_next_state(a1, 5);
			return 0;
		}
		uint32_t node = a1;
		if (a_743C20(node) != 0)
			s5_next_state(node, 5);
		return 0;
	}

	uint32_t __cdecl s_744C30(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t states[4] = { game_addr(game::MAG_095_SplashSparkle_State0_Init), game_addr(game::MAG_095_SplashSparkle_State1_LoopRepeat), game_addr(game::MAG_095_SplashSparkle_State2_PlayLast), game_addr(game::MAG_095_SplashSparkle_State3_Nop) };
		callp(states[S8(node, 0x29)], node);
		uint32_t p = node + 0x8C;
		for (int32_t i = 0; i < 4; ++i)
		{
			U16(p, 2) = (uint16_t)(U16(p, 2) + 0xC0);
			int16_t v = S16(p, 0);
			U16(p, 0) = (uint16_t)(v - (int16_t)(v / 16));
			v = S16(p, 4);
			U16(p, 4) = (uint16_t)(v - (int16_t)(v / 16));
			U16(p, -0x20) = (uint16_t)(U16(p, -0x20) + (uint16_t)((int32_t)S16(p, 0) / 16));
			U16(p, -0x1E) = (uint16_t)(U16(p, -0x1E) + (uint16_t)((int32_t)S16(p, 2) / 16));
			U16(p, -0x1C) = (uint16_t)(U16(p, -0x1C) + (uint16_t)((int32_t)S16(p, 4) / 16));
			uint32_t xy = U32(p, -0x20);
			U32(node, 0x1C) = xy;
			uint32_t z = U32(p, -0x1C);
			U32(node, 0x20) = z;
			if (i < 1)
				s_744D30(node, 0x3000);
			p += 8;
		}
		return s5_task_tail(node);
	}

	uint32_t __cdecl s_744D30(uint32_t a1, uint32_t a2)
	{
		uint32_t node = a1;
		if ((U8(node, 0x26) & 4) != 0)
			return 0;
		uint32_t s = x::Field_Alloc(0xE8);
		uint32_t mat = s + 0xB8;
		x::UnpackRotationMatrix(game_addr(game::unk_1D97778), mat);
		uint16_t scale = (uint16_t)a2;
		if (scale != 0x1000)
		{
			int32_t sc = (int16_t)scale;
			U32(s, 0xE0) = (uint32_t)sc;
			U32(s, 0xDC) = (uint32_t)sc;
			U32(s, 0xD8) = (uint32_t)sc;
			x::scale3DMatrix(mat, s + 0xD8);
		}
		int32_t px = S16(node, 0x1C);
		int32_t py = S16(node, 0x1E);
		int32_t pz = S16(node, 0x20);
		uint32_t pos = s + 0xCC;
		U32(s, 0xD0) = (uint32_t)py;
		U32(s, 0xD4) = (uint32_t)pz;
		U32(pos, 0) = (uint32_t)px;
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(mat);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(mat);
		x::GTE_LoadIRFromMatrixColumn(s + 0xBA);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(s + 0xBA);
		x::GTE_LoadIRFromMatrixColumn(s + 0xBC);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(s + 0xBC);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(pos);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(pos);
		x::GTE_SetRotMatrix(mat);
		x::GTE_SetTransVector(mat);
		uint32_t seq = U32(node, 0x4C);
		uint16_t frame = U16(node, 0x50);
		uint16_t w56 = U16(node, 0x56);
		U32(s, 0) = seq;
		uint32_t cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		U16(s, 4) = frame;
		uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(s, 0x24) = 0;
		U16(s, 0xB4) = w56;
		uint32_t r = a_7435E0(s, ot, 2, cursor);
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = r;
		x::Field_Free(0xE8);
		return 0;
	}

	uint32_t __cdecl s_744E80(uint32_t a1)
	{
		uint32_t node = a1;
		U32(node, 0x4C) = game_addr(game::unk_15335CC);
		U16(node, 0x56) = 0xFFE0;
		U16(node, 0x52) = 8;
		if (a_73B7E0(3) != 0)
			U8(node, 0x26) = (uint8_t)(U8(node, 0x26) | 4);
		uint32_t r = x::CrtRand();
		U16(node, 0xDC) = (uint16_t)(r & 7);
		S8(node, 0x29) = (int8_t)(S8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_744ED0(uint32_t a1)
	{
		if (a_73B7E0(3) != 0)
		{
			s5_next_state(a1, 5);
			return 0;
		}
		uint32_t node = a1;
		a_743C20(node);
		if (S16(node, 0x50) > 1)
		{
			U16(node, 0xDC) = (uint16_t)(U16(node, 0xDC) - 1);
			bool more = S16(node, 0xDC) > 0;
			U16(node, 0x50) = 0;
			if (!more)
				S8(node, 0x29) = (int8_t)(S8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl s_744F30(uint32_t a1)
	{
		if (a_73B7E0(3) != 0)
		{
			s5_next_state(a1, 5);
			return 0;
		}
		uint32_t node = a1;
		if (a_743C20(node) != 0)
			s5_next_state(node, 5);
		return 0;
	}

	uint32_t __cdecl s_744F90(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t states[4] = { game_addr(game::MAG_095_SpawnScript_State0_Origin), game_addr(game::MAG_095_SpawnScript_State1_ResetCursor), game_addr(game::MAG_095_SpawnScript_State2_RunScript), game_addr(game::MAG_095_SpawnScript_State3_Nop) };
		callp(states[S8(node, 0x29)], node);
		return s5_task_tail(node);
	}

	uint32_t __cdecl s_744FF0(uint32_t a1)
	{
		uint32_t obj = MEM<uint32_t>(game_addr(game::unk_1533010));
		uint32_t xy = U32(obj, 8);
		uint32_t z = U32(obj, 0xC);
		U32(a1, 0x1C) = xy;
		U32(a1, 0x20) = z;
		S8(a1, 0x29) = (int8_t)(S8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_745010(uint32_t a1)
	{
		U16(a1, 0xDC) = 0;
		S8(a1, 0x29) = (int8_t)(S8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_745030(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t entry = (uint32_t)add32(mul32((int32_t)S16(node, 0xDC), 10), game_addr(game::unk_15339D8));
		if (a_73B7E0(6) != 0)
		{
			s5_next_state(node, 1);
			return 0;
		}
		int16_t frame = S16(entry, 0);
		if ((uint16_t)frame == 0x7FFF)
		{
			s5_next_state(node, 1);
			return 0;
		}
		if (frame > S16(node, 0x24))
			return 0;
		do
		{
			uint32_t r = x::CrtRand();
			uint32_t ang = r & 0x3F;
			ang = (ang & 0xFFFF0000u) | (uint16_t)((uint16_t)ang + U16(entry, 2));
			ang = (uint32_t)sub32((int32_t)ang, 0x20) & 0xFFF;
			uint32_t asset;
			if (S16(entry, 8) == 0)
				asset = game_addr(game::unk_16A40EC);
			else
				asset = U32(MEM<uint32_t>(game_addr(game::MAG_095_AssetTablePtr)), 0x108);
			uint32_t t = a_73C100(node, game_addr(game::MAG_095_ScriptParticleSystemTask), asset, 0, 0xA, 3);
			if (t != 0)
			{
				int32_t a = (int16_t)ang;
				U16(t, 0x288) = 0;
				U16(t, 0x28A) = 0xFE80;
				U16(t, 0x28C) = 0xCC0;
				int32_t sn = (int32_t)x::computeSin((uint32_t)a);
				int32_t v = mul32(sn, S16(entry, 6)) / 4096;
				U16(t, 0x288) = (uint16_t)(U16(t, 0x288) + (uint16_t)(0u - (uint32_t)v));
				int32_t cs = (int32_t)x::computeCosine((uint32_t)a);
				v = mul32(cs, S16(entry, 6)) / 4096;
				U16(t, 0x28C) = (uint16_t)(U16(t, 0x28C) + (uint16_t)(0u - (uint32_t)v));
				uint32_t r2 = x::CrtRand();
				uint16_t h = (uint16_t)(U16(entry, 4) - (uint16_t)(r2 & 0x3F));
				U16(t, 0x28A) = (uint16_t)(U16(t, 0x28A) + h);
			}
			U16(node, 0xDC) = (uint16_t)(U16(node, 0xDC) + 1);
			entry = (uint32_t)add32(mul32((int32_t)S16(node, 0xDC), 10), game_addr(game::unk_15339D8));
		} while (S16(entry, 0) <= S16(node, 0x24));
		return 0;
	}

	uint32_t __cdecl s_7451E0(uint32_t a1)
	{
		uint32_t node = a1;
		if (S16(node, 0x24) >= S16(node, 0x298))
		{
			a_73F990(node);
			s_740470(node);
			S8(node, 0x29) = (int8_t)(S8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl s_745210(uint32_t a1)
	{
		uint32_t node = a1;
		if (s_740470(node) != 0)
			s5_next_state(node, 1);
		return 0;
	}

	uint32_t __cdecl s_7452B0(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueB), game_addr(game::MAG_095_OrbitSpriteTask), 0x64, node);
		uint16_t idx = U16(node, 0x60);
		U16(t, 0x60) = idx;
		U16(node, 0x60) = (uint16_t)(idx + 1);
		uint32_t r = x::CrtRand();
		U16(node, 0x5C) = (uint16_t)((r & 3) + 4);
		S8(node, 0x29) = (int8_t)(S8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_7452F0(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t states[5] = { game_addr(game::MAG_095_OrbitSprite_State0_Init), game_addr(game::MAG_095_OrbitSprite_State1_FadeIn), game_addr(game::MAG_095_OrbitSprite_State2_Hold), game_addr(game::MAG_095_OrbitSprite_State3_FadeOut), game_addr(game::MAG_095_OrbitSprite_State4_Nop) };
		callp(states[S8(node, 0x29)], node);
		uint16_t spin = U16(node, 0x5E);
		uint16_t dx = U16(node, 0x54);
		spin = (uint16_t)(spin + U16(node, 0x48));
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + dx);
		U16(node, 0x48) = (uint16_t)(spin & 0xFFF);
		s_745380(node);
		return s5_task_tail(node);
	}

	uint32_t __cdecl s_745380(uint32_t a1)
	{
		uint32_t node = a1;
		if ((U8(node, 0x26) & 4) != 0)
			return 0;
		uint8_t m[0x20] = {};
		uint32_t L = P(m);
		x::MAG_022_sub_8DD770(L);
		x::MAG_022_sub_8DD8A0(L, (uint32_t)(int32_t)S16(node, 0x46));
		x::sub_8DD7E0(L, (uint32_t)(int32_t)S16(node, 0x44));
		x::sub_8DD960(L, (uint32_t)(int32_t)S16(node, 0x48));
		int32_t px = S16(node, 0x1C);
		int32_t py = S16(node, 0x1E);
		int32_t pz = S16(node, 0x20);
		U32(L, 0x14) = (uint32_t)px;
		U32(L, 0x18) = (uint32_t)py;
		U32(L, 0x1C) = (uint32_t)pz;
		x::scale3DMatrix(L, node + 0x30);
		x::GTE_SetRotMatrix_W(L);
		x::GTE_SetTransVector_W(L);
		uint32_t d = x::Field_Alloc(0x68);
		U32(d, 0) = U32(node, 0x4C);
		U32(d, 8) = U32(node, 0x40);
		U32(d, 0xC) = (uint32_t)(int32_t)S16(node, 0x50);
		U16(d, 0x18) = 0;
		U16(d, 0x1A) = 0;
		U16(d, 0x1E) = 0;
		U16(d, 0x1C) = 0;
		U16(d, 0x26) = 0;
		U16(d, 0x24) = 0;
		U32(d, 0x10) = (uint32_t)(int32_t)S16(node, 0x52);
		uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase));
		U16(d, 0x22) = 0x100;
		U16(d, 0x20) = 0x100;
		U16(d, 0x2A) = 0x100;
		U16(d, 0x28) = 0x100;
		uint32_t cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		U32(d, 0x14) = 0xF0;
		uint32_t r = s_73C790(d, ot + 0x44, 2, cursor);
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = r;
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl s_745490(uint32_t a1)
	{
		uint32_t tab = MEM<uint32_t>(game_addr(game::MAG_095_AssetTablePtr));
		uint32_t node = a1;
		U32(node, 0x4C) = U32(tab, 0x144);
		U16(node, 0x52) = 0xFE00;
		int32_t r = (int32_t)x::CrtRand();
		if ((r & 1) != 0)
		{
			r = (int32_t)x::CrtRand();
			U16(node, 0x1C) = (uint16_t)(r % 600 - 300);
			r = (int32_t)x::CrtRand();
			int32_t y = r % 30 + 0x78;
			U16(node, 0x1E) = (uint16_t)y;
			if ((U8(node, 0x60) & 1) != 0)
				U16(node, 0x1E) = (uint16_t)(0u - (uint32_t)y);
		}
		else
		{
			r = (int32_t)x::CrtRand();
			int32_t xx = r % 150 + 0x96;
			U16(node, 0x1C) = (uint16_t)xx;
			if ((U8(node, 0x60) & 1) != 0)
				U16(node, 0x1C) = (uint16_t)(0u - (uint32_t)xx);
			r = (int32_t)x::CrtRand();
			U16(node, 0x1E) = (uint16_t)(r % 240 - 0x78);
		}
		uint32_t obj = MEM<uint32_t>(game_addr(game::unk_15339D0));
		U16(node, 0x20) = (uint16_t)(U16(obj, 0xE0) << 1);
		r = (int32_t)x::CrtRand();
		U16(node, 0x54) = (uint16_t)(r % 3 + 2);
		r = (int32_t)x::CrtRand();
		if ((r & 1) != 0)
			U16(node, 0x54) = (uint16_t)(0 - U16(node, 0x54));
		r = (int32_t)x::CrtRand();
		uint32_t sc = (uint32_t)(r % 0xFFF + 0x400);
		U32(node, 0x38) = sc;
		U32(node, 0x34) = sc;
		U32(node, 0x30) = sc;
		r = (int32_t)x::CrtRand();
		U16(node, 0x48) = (uint16_t)(r & 0xFFF);
		r = (int32_t)x::CrtRand();
		U16(node, 0x50) = 0x1000;
		U16(node, 0x5E) = (uint16_t)((r & 0xF) + 0x10);
		S8(node, 0x29) = (int8_t)(S8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_7455B0(uint32_t a1)
	{
		uint32_t node = a1;
		U16(node, 0x50) = (uint16_t)(U16(node, 0x50) + 0xFE00);
		if (S16(node, 0x50) <= 0)
		{
			U16(node, 0x50) = 0;
			uint32_t r = x::CrtRand();
			U16(node, 0x5C) = (uint16_t)((r & 3) + 4);
			S8(node, 0x29) = (int8_t)(S8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl s_7455F0(uint32_t a1)
	{
		U16(a1, 0x5C) = (uint16_t)(U16(a1, 0x5C) - 1);
		if (S16(a1, 0x5C) <= 0)
			S8(a1, 0x29) = (int8_t)(S8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_745610(uint32_t a1)
	{
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0x200);
		if (S16(a1, 0x50) >= 0x1000)
		{
			int8_t st = S8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U16(a1, 0x50) = 0x1000;
			S8(a1, 0x29) = (int8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl s_745650(uint32_t a1)
	{
		uint32_t node = a1;
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) - 1);
		if (a_73B7E0(7) != 0)
		{
			s5_next_state(node, 1);
			return 0;
		}
		if (S16(node, 0x5C) <= 0)
			S8(node, 0x29) = (int8_t)(S8(node, 0x29) - 1);
		return 0;
	}

	uint32_t __cdecl s_7456A0(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t states[4] = { game_addr(game::MAG_095_TargetSparkSpawner_State0_AtTarget), game_addr(game::MAG_095_TargetSparkSpawner_State1_WaitFrame16), game_addr(game::MAG_095_TargetSparkSpawner_State2_Spawn), game_addr(game::MAG_095_TargetSparkSpawner_State3_Nop) };
		callp(states[S8(node, 0x29)], node);
		return s5_task_tail(node);
	}

	uint32_t __cdecl s_745700(uint32_t a1)
	{
		uint32_t slot = U8(a1, 0x2D);
		uint32_t ent = game_addr(game::BattleEntitySlotData) + slot * 39 * 4;
		U32(a1, 0x1C) = U32(ent, 0x1C);
		int8_t st = S8(a1, 0x29);
		U32(a1, 0x20) = U32(ent, 0x20);
		S8(a1, 0x29) = (int8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl s_745730(uint32_t a1)
	{
		if (S16(a1, 0x24) > 0xF)
			S8(a1, 0x29) = (int8_t)(S8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_745740(uint32_t a1)
	{
		uint32_t node = a1;
		uint8_t slot = U8(node, 0x2D);
		uint8_t ph = (uint8_t)(U8(node, 0x24) + slot);
		if ((ph & 3) == 0)
		{
			uint32_t ent = game_addr(game::BattleEntitySlotData) + (uint32_t)slot * 39 * 4;
			uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueB), game_addr(game::MAG_095_TargetSparkTask), 0x64, node);
			uint16_t lo = U16(ent, 0x34);
			uint32_t r = x::CrtRand();
			int32_t v = mul32((int32_t)(r & 0xFFF), sub32(S16(ent, 0x3A), (int16_t)lo)) / 4096;
			uint16_t w = (uint16_t)((uint16_t)v + U16(ent, 0x1C));
			w = (uint16_t)(w + lo);
			lo = U16(ent, 0x36);
			U16(t, 0x1C) = w;
			r = x::CrtRand();
			v = mul32((int32_t)(r & 0xFFF), sub32(S16(ent, 0x3C), (int16_t)lo)) / 4096;
			w = (uint16_t)((uint16_t)v + U16(ent, 0x1E));
			w = (uint16_t)(w + lo);
			lo = U16(ent, 0x38);
			U16(t, 0x1E) = w;
			r = x::CrtRand();
			v = mul32((int32_t)(r & 0xFFF), sub32(S16(ent, 0x3E), (int16_t)lo)) / 4096;
			w = (uint16_t)((uint16_t)v + U16(ent, 0x20));
			w = (uint16_t)(w + lo);
			U16(t, 0x20) = w;
		}
		if (a_73B7E0(7) != 0)
			s5_next_state(node, 1);
		return 0;
	}

	uint32_t __cdecl s_745840(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t states[5] = { game_addr(game::MAG_095_TargetSpark_State0_Init), game_addr(game::MAG_095_TargetSpark_State1_FadeIn), game_addr(game::MAG_095_TargetSpark_State2_Hold), game_addr(game::MAG_095_TargetSpark_State3_FadeOut), game_addr(game::MAG_095_TargetSpark_State4_Nop) };
		callp(states[S8(node, 0x29)], node);
		uint16_t spin = (uint16_t)(U16(node, 0x5E) + U16(node, 0x48));
		U16(node, 0x48) = (uint16_t)(spin & 0xFFF);
		uint32_t sc = U32(node, 0x38) + 0x100;
		U32(node, 0x38) = sc;
		U32(node, 0x34) = sc;
		U32(node, 0x30) = sc;
		a_7458E0(node);
		return s5_task_tail(node);
	}

	uint32_t __cdecl s_7459D0(uint32_t a1)
	{
		uint32_t tab = MEM<uint32_t>(game_addr(game::MAG_095_AssetTablePtr));
		uint32_t node = a1;
		U32(node, 0x4C) = U32(tab, 0x148);
		U32(node, 0x38) = 0x100;
		U32(node, 0x34) = 0x100;
		U32(node, 0x30) = 0x100;
		uint32_t r = x::CrtRand();
		U16(node, 0x48) = (uint16_t)(r & 0xFFF);
		r = x::CrtRand();
		U16(node, 0x50) = 0x1000;
		U16(node, 0x5E) = (uint16_t)((r & 0xF) + 0x10);
		S8(node, 0x29) = (int8_t)(S8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_745A20(uint32_t a1)
	{
		uint32_t node = a1;
		U16(node, 0x50) = (uint16_t)(U16(node, 0x50) + 0xFC00);
		if (S16(node, 0x50) <= 0)
		{
			U16(node, 0x50) = 0;
			uint32_t r = x::CrtRand();
			U16(node, 0x5C) = (uint16_t)((r & 3) + 4);
			S8(node, 0x29) = (int8_t)(S8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl s_745A60(uint32_t a1)
	{
		U16(a1, 0x5C) = (uint16_t)(U16(a1, 0x5C) - 1);
		if (S16(a1, 0x5C) <= 0)
			S8(a1, 0x29) = (int8_t)(S8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_745A80(uint32_t a1)
	{
		U16(a1, 0x50) = (uint16_t)(U16(a1, 0x50) + 0x200);
		if (S16(a1, 0x50) >= 0x1000)
		{
			int8_t st = S8(a1, 0x29);
			U8(a1, 0x26) = (uint8_t)(U8(a1, 0x26) | 1);
			U16(a1, 0x50) = 0x1000;
			S8(a1, 0x29) = (int8_t)(st + 1);
		}
		return 0;
	}

	namespace
	{
		inline uint32_t s6_surface(uint32_t node)
		{
			int32_t idx = S16(node, 0xde);
			int32_t k = add32(idx, mul32(idx, 2));
			k = add32(k, mul32(k, 8));
			k = shl32(k, 1);
			k = sub32(k, idx);
			return (uint32_t)add32(mul32(k, 4), game_addr(game::MAG_095_WaterSurfaceBlocks));
		}

		inline uint32_t s6_texwin(uint32_t edi, int32_t o)
		{
			uint32_t c = ((uint32_t)U8(edi, o + 2) & 0xf8) | 0xfffe2000u;
			c = (uint32_t)shl32((int32_t)c, 5);
			c |= (uint32_t)U8(edi, o) & 0xf8;
			c = (uint32_t)shl32((int32_t)c, 5);
			c |= (~((uint32_t)U16(edi, o + 6) - 1u)) & 0xf8;
			c = (uint32_t)shl32((int32_t)c, 2);
			uint32_t d = (uint32_t)((int32_t)(~((uint32_t)U16(edi, o + 4) - 1u)) >> 3) & 0x1f;
			return c | d;
		}
	}

	uint32_t __cdecl s_745AD0(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t states[6] = { game_addr(game::MAG_095_Siren_WaterSurface_State0_PlaceAndWait), game_addr(game::MAG_095_Siren_WaterSurface_State1_InitBlock), game_addr(game::MAG_095_Siren_WaterSurface_State2_Grow), game_addr(game::MAG_095_Siren_WaterSurface_State3_Hold), game_addr(game::MAG_095_Siren_WaterSurface_State4_Fade), game_addr(game::MAG_095_Siren_WaterSurface_State5_Empty) };
		int32_t st = S8(node, 0x29);
		callp(states[st], node);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((U8(node, 0x26) & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl s_745B40(uint32_t a1)
	{
		uint32_t script = MEM<uint32_t>(game_addr(game::unk_1533010));
		uint16_t z = U16(script, 4);
		uint32_t node = a1;
		U16(node, 0x1c) = 0x80;
		uint32_t idx = (uint32_t)(int32_t)S16(node, 0xde);
		U16(node, 0x1e) = 0xfb00;
		U16(node, 0x20) = z;
		if (idx > 3)
			return 0;
		switch (idx)
		{
		case 0:
			if (S16(node, 0x24) >= 0x2d)
				U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			break;
		case 1:
			if (S16(node, 0x24) >= 0x41)
				U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			break;
		case 2:
			if (S16(node, 0x24) >= 0x55)
				U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			if (S16(node, 0x24) >= 0x69)
				U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			break;
		case 3:
			if (S16(node, 0x24) >= 0x69)
				U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			break;
		}
		return 0;
	}

	uint32_t __cdecl s_745BB0(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t which = (uint32_t)U16(node, 0xde);
		s_745BF0(node, s6_surface(node), which);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_745BF0(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		(void)a1;
		uint32_t b = a2;
		x::MAG_007_sub_8DCC00(b, 0xd4);
		U16(b, 0x56) = 0x100;
		U16(b, 0x54) = 0x100;
		U16(b, 0x0e) = 0x3e54;
		U8(b, 0x0d) = 0x80;
		U8(b, 0x31) = 0xff;
		U8(b, 0x25) = 0xff;
		U8(b, 0x0c) = 0;
		uint32_t eax = U32(b, 0x0c);
		U8(b, 0x0c) = 0x2b;
		U32(b, 0x60) = eax;
		U32(b, 0x6c) = eax;
		eax = U32(b, 0x0c);
		U16(b, 0x1a) = 0xbc;
		U8(b, 0x19) = 0x80;
		U8(b, 0x30) = 0x2b;
		uint32_t edx = U32(b, 0x30);
		U8(b, 0x18) = 0x2b;
		uint32_t ecx = U32(b, 0x18);
		U8(b, 0x24) = 0;
		uint32_t edi = U32(b, 0x24);
		U32(b, 0x64) = eax;
		U32(b, 0x70) = eax;
		U32(b, 0x78) = ecx;
		U32(b, 0xa8) = edx;
		U32(b, 0x84) = ecx;
		U32(b, 0xb4) = edx;
		U8(b, 0x30) = 0x55;
		edx = U32(b, 0x30);
		U8(b, 0x18) = 0x55;
		ecx = U32(b, 0x18);
		U32(b, 0x90) = edi;
		U32(b, 0x9c) = edi;
		U8(b, 0x24) = 0x2b;
		edi = U32(b, 0x24);
		U8(b, 0x0c) = 0x55;
		U8(b, 0x30) = 0x7f;
		U8(b, 0x18) = 0x7f;
		eax = U32(b, 0x0c);
		U32(b, 0x7c) = ecx;
		U32(b, 0x88) = ecx;
		ecx = U32(b, 0x18);
		U16(b, 0x52) = 0;
		U16(b, 0x50) = 0;
		U16(b, 0x58) = 0;
		U16(b, 0x5a) = 0x80;
		U16(b, 0x5c) = 0x80;
		U16(b, 0x5e) = 0x80;
		U32(b, 0x00) = 0x0c000000;
		U8(b, 0x07) = 0x3e;
		U8(b, 0x06) = 0x80;
		U8(b, 0x05) = 0x80;
		U8(b, 0x04) = 0x80;
		U8(b, 0x12) = 0x80;
		U8(b, 0x11) = 0x80;
		U8(b, 0x10) = 0x80;
		U8(b, 0x1e) = 0x80;
		U8(b, 0x1d) = 0x80;
		U8(b, 0x1c) = 0x80;
		U8(b, 0x2a) = 0x80;
		U8(b, 0x29) = 0x80;
		U8(b, 0x28) = 0x80;
		U32(b, 0x94) = edi;
		U32(b, 0xac) = edx;
		U32(b, 0xa0) = edi;
		U32(b, 0xb8) = edx;
		U8(b, 0x24) = 0x55;
		U32(b, 0x68) = eax;
		edx = U32(b, 0x24);
		edi = U32(b, 0x30);
		U32(b, 0x80) = ecx;
		U32(b, 0x8c) = ecx;
		U32(b, 0x98) = edx;
		U32(b, 0xb0) = edi;
		U32(b, 0x74) = eax;
		U32(b, 0xa4) = edx;
		U32(b, 0xbc) = edi;
		U8(b, 0x4f) = 0x3e;
		U8(b, 0x4e) = 0x80;
		U8(b, 0x4d) = 0x80;
		U8(b, 0x4c) = 0x80;
		U8(b, 0x4b) = 0x3e;
		U8(b, 0x4a) = 0x40;
		U8(b, 0x49) = 0x40;
		U8(b, 0x48) = 0x40;
		U8(b, 0x47) = 0x3e;
		U8(b, 0x46) = 0;
		U8(b, 0x45) = 0;
		U8(b, 0x44) = 0;

		uint32_t v = game_addr(game::MAG_095_SurfaceBaseGrid);
		int32_t width = 0xc0;
		for (int32_t row = 0; row < 0xb; row++)
		{
			int32_t w16 = (int16_t)width;
			int32_t half = add32(w16, mul32(w16, 2)) / 2;
			int32_t z = shl32(-row, 0xa);
			for (int32_t col = 0; col < 4; col++)
			{
				int32_t xx = sub32(half, mul32(col, width));
				U16(v, 0) = (uint16_t)xx;
				U16(v, 2) = 0;
				U16(v, 4) = (uint16_t)z;
				v += 8;
			}
			width = add32(width, 0x60);
		}

		uint32_t r = x::CrtRand();
		U16(b, 0xce) = (uint16_t)(r & 0xfff);
		r = x::CrtRand();
		U16(b, 0xd0) = (uint16_t)(r & 0xfff);

		uint32_t which = (uint32_t)(int32_t)(int16_t)a3;
		switch (which)
		{
		case 0: U16(b, 0x3c) = 0x40; U16(b, 0x3e) = 0x180; U16(b, 0x40) = 0; break;
		case 1: U16(b, 0x3c) = 0x40; U16(b, 0x3e) = 0x80;  U16(b, 0x40) = 0; break;
		case 2: U16(b, 0x3c) = 0x40; U16(b, 0x3e) = 0xf80; U16(b, 0x40) = 0; break;
		case 3: U16(b, 0x3c) = 0x40; U16(b, 0x3e) = 0xe80; U16(b, 0x40) = 0; break;
		default: break;
		}
		return 0;
	}

	uint32_t __cdecl s_745E80(uint32_t a1)
	{
		uint32_t node = a1;
		s_745F50(node, s6_surface(node));
		if (a_73B7E0(7) != 0)
		{
			U16(node, 0xdc) = 0;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			return 0;
		}
		int32_t idx = S16(node, 0xde);
		if (U16(s6_surface(node), 0xcc) != 0xa)
			return 0;
		uint32_t r;
		switch ((uint32_t)idx)
		{
		case 0:
			r = x::CrtRand();
			U16(node, 0xdc) = (uint16_t)((r & 3) + 2);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			break;
		case 1:
			U16(node, 0xdc) = 0;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			break;
		case 2:
			r = x::CrtRand();
			U16(node, 0xdc) = (uint16_t)((r & 3) + 4);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			break;
		case 3:
			r = x::CrtRand();
			U16(node, 0xdc) = (uint16_t)((r & 3) + 6);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			break;
		default:
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
			break;
		}
		return 0;
	}

	uint32_t __cdecl s_745F50(uint32_t a1, uint32_t a2)
	{
		alignas(4) uint8_t frame[0x70] = {};
		const uint32_t F0 = P(frame);
		U32(F0, 0x68) = a1;
		U32(F0, 0x6c) = a2;

		uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase));
		uint32_t edi = U32(F0, 0x6c);
		uint32_t ebx = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		U32(F0, 0x2c) = ot;
		U16(edi, 0xcc) = (uint16_t)(U16(edi, 0xcc) + 1);
		if (S16(edi, 0xcc) > 0xa)
			U16(edi, 0xcc) = 0xa;

		uint32_t vs = (uint32_t)sub32((int32_t)U32(edi, 0xc0), 0x1000) & 0x7f00;
		U32(edi, 0xc0) = vs;
		U32(edi, 0xc4) = vs + 0x4000;
		U32(edi, 0xc8) = vs + 0x8000;
		for (uint32_t k = 0; k < 3; k++)
		{
			const uint32_t m = 0xffff00ffu;
			uint32_t b = edi + 0x78 + 4 * k;
			U32(b, -0x18) &= m;
			U32(b, 0) &= m;
			U32(b, 0x18) &= m;
			U32(b, 0x30) &= m;
			U32(b, -0xc) &= m;
			U32(b, 0xc) &= m;
			U32(b, 0x24) &= m;
			U32(b, 0x3c) &= m;
			U32(b, -0x18) |= U32(edi, 0xc0);
			U32(b, 0) |= U32(edi, 0xc0);
			U32(b, 0x18) |= U32(edi, 0xc4);
			U32(b, 0x30) |= U32(edi, 0xc4);
			U32(b, -0xc) |= U32(edi, 0xc4);
			U32(b, 0xc) |= U32(edi, 0xc4);
			U32(b, 0x24) |= U32(edi, 0xc8);
			U32(b, 0x3c) |= U32(edi, 0xc8);
		}

		uint16_t ph1 = (uint16_t)((U16(edi, 0xce) + 0x80) & 0xfff);
		uint16_t ph2 = (uint16_t)((U16(edi, 0xd0) + 0x80) & 0xfff);
		U16(edi, 0xce) = ph1;
		U16(edi, 0xd0) = ph2;
		if (a_73B7E0(5) != 0)
		{
			U32(F0, 0x3c) = 0x1000;
			U32(F0, 0x38) = 0x1000;
			U32(F0, 0x34) = 0x1000;
			U32(F0, 0x28) = 0xc0;
		}
		else
		{
			U32(F0, 0x28) = 0x60;
			U32(F0, 0x3c) = 0x200;
			U32(F0, 0x38) = 0x200;
			U32(F0, 0x34) = 0x200;
		}
		uint16_t cx = U16(edi, 0xce);
		uint16_t dx = U16(edi, 0xd0);
		uint32_t src = game_addr(game::MAG_095_SurfaceBaseGrid);
		U32(F0, 0x6c) = 0;
		U32(F0, 0x18) = 0;
		uint32_t dst = game_addr(game::MAG_095_SurfaceVertexGrid);
		U16(F0, 0x1c) = cx;
		U16(F0, 0x20) = dx;
		x::MAG_022_sub_8DD770(F0 + 0x44);
		x::MAG_022_sub_8DD8A0(F0 + 0x44, (uint32_t)(int32_t)S16(edi, 0x3e));
		x::sub_8DD7E0(F0 + 0x44, (uint32_t)(int32_t)S16(edi, 0x3c));
		x::scale3DMatrix(F0 + 0x44, F0 + 0x34);
		x::GTE_SetRotMatrix(F0 + 0x44);

		U32(F0, 0x10) = 0;
		if (S16(edi, 0xcc) + 1 > 0)
		{
			do
			{
				U32(F0, 0x24) = 4;
				do
				{
					uint32_t w0 = U32(src, 0);
					uint32_t w1 = U32(src, 4);
					U32(dst, 0) = w0;
					U32(dst, 4) = w1;
					int32_t s = (int32_t)x::computeSin((uint32_t)(int32_t)S16(F0, 0x1c));
					int32_t t = mul32(s, S16(F0, 0x6c));
					U16(dst, 0) = (uint16_t)(U16(dst, 0) + (uint16_t)(t / 4096));
					s = (int32_t)x::computeSin((uint32_t)(int32_t)S16(F0, 0x20));
					t = mul32(s, S16(F0, 0x18));
					U16(dst, 2) = (uint16_t)(U16(dst, 2) + (uint16_t)(t / 4096));
					x::GTE_LoadV0(dst);
					x::GTE_MVMVA_RotV0();
					x::GTE_StoreIR123(dst);
					uint32_t node = U32(F0, 0x68);
					src += 8;
					dst += 8;
					uint16_t ox = U16(node, 0x1c);
					uint16_t oy = U16(node, 0x1e);
					U16(dst, -8) = (uint16_t)(U16(dst, -8) + ox);
					uint16_t oz = U16(node, 0x20);
					U16(dst, -6) = (uint16_t)(U16(dst, -6) + oy);
					U16(dst, -4) = (uint16_t)(U16(dst, -4) + oz);
					U32(F0, 0x24) = U32(F0, 0x24) - 1;
				} while (U32(F0, 0x24) != 0);
				U32(F0, 0x6c) = (uint32_t)add32((int32_t)U32(F0, 0x6c), (int32_t)U32(F0, 0x28));
				U32(F0, 0x18) = (uint32_t)add32((int32_t)U32(F0, 0x18), 0x40);
				U32(F0, 0x1c) = (U32(F0, 0x1c) + 0x100) & 0xfff;
				U32(F0, 0x20) = (U32(F0, 0x20) + 0x200) & 0xfff;
				U32(F0, 0x10) = U32(F0, 0x10) + 1;
			} while ((int32_t)U32(F0, 0x10) < S16(edi, 0xcc) + 1);
		}

		{
			int32_t n = S16(edi, 0xcc);
			uint32_t out = game_addr(game::MAG_095_SurfaceVertexColours);
			for (int32_t r = 0; r < n + 1; r++)
			{
				for (int32_t c = 0; c < 4; c++)
				{
					uint32_t col = 0;
					bool mid = false;
					if (r == 0)
						col = U32(edi, 0x44);
					else if (r == 1)
					{
						if (c == 0) col = U32(edi, 0x44);
						else mid = true;
					}
					else if (r == n)
						col = U32(edi, 0x44);
					else if (r == n - 1)
					{
						if (c == 0) col = U32(edi, 0x44);
						else mid = true;
					}
					else
					{
						if (c == 0) col = U32(edi, 0x44);
						else if (c == 3) col = U32(edi, 0x44);
						else col = U32(edi, 0x4c);
					}
					if (mid)
						col = (c == 3) ? U32(edi, 0x44) : U32(edi, 0x48);
					U32(out, 0) = col;
					out += 4;
				}
			}
		}

		U32(F0, 0x6c) = game_addr(game::MAG_095_SurfaceVertexGrid);
		uint32_t colp = game_addr(game::MAG_095_SurfaceVertexColours);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		U32(F0, 0x10) = 0;
		if (S16(edi, 0xcc) > 0)
		{
			do
			{
				U32(F0, 0x24) = U32(F0, 0x10) & 1;
				U32(F0, 0x20) = U32(F0, 0x6c) + 8;
				U32(F0, 0x1c) = edi + 0x78;
				U32(F0, 0x28) = 3;
				do
				{
					uint32_t uv = U32(F0, 0x1c);
					uint32_t vp = U32(F0, 0x20);
					uint32_t odd = U32(F0, 0x24);
					U32(ebx, 0x00) = U32(edi, 0);
					U32(ebx, 0x04) = U32(colp, 0);
					U32(ebx, 0x10) = U32(colp, 4);
					U32(ebx, 0x1c) = U32(colp, 0x10);
					uint32_t d = U32(colp, 0x14);
					U32(ebx, 0x28) = d;
					uint32_t c;
					if (odd != 0)
					{
						c = U32(uv, -0x18);
						d = U32(uv, 0);
						U32(ebx, 0x0c) = c;
						c = U32(uv, 0x18);
						U32(ebx, 0x18) = d;
						d = U32(uv, 0x30);
						U32(ebx, 0x24) = c;
					}
					else
					{
						c = U32(uv, -0xc);
						d = U32(uv, 0xc);
						U32(ebx, 0x0c) = c;
						c = U32(uv, 0x24);
						U32(ebx, 0x18) = d;
						d = U32(uv, 0x3c);
						U32(ebx, 0x24) = c;
					}
					uint32_t v0 = U32(F0, 0x6c);
					U32(ebx, 0x30) = d;
					x::GTE_LoadV012(v0, vp, vp + 0x18);
					x::GTE_RTPT();
					x::GTE_StoreSXY012_PolyGT3_2(ebx);
					vp += 0x20;
					x::GTE_LoadV0(vp);
					x::GTE_RTPS();
					x::GTE_ReadSXY2(ebx + 0x2c);
					x::GTE_AVSZ4();
					x::GTE_ReadOTZ(F0 + 0x14);
					int32_t z = (int32_t)U32(F0, 0x14) >> 2;
					uint32_t pk = ebx + 0x34;
					uint32_t tw = edi + 0x50;
					U32(F0, 0x14) = (uint32_t)z;
					U32(pk, 0) = 0x2000000;
					uint32_t cmd = (tw != 0) ? s6_texwin(edi, 0x50) : 0;
					uint32_t otb = U32(F0, 0x2c);
					U32(pk, 4) = cmd;
					U32(pk, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(otb + U32(F0, 0x14) * 4, pk);
					uint32_t zz = U32(F0, 0x14);
					uint32_t poly = ebx;
					ebx = U32(F0, 0x2c);
					x::SSIGPU_InsertPrimAutoDepth(ebx + zz * 4, poly);
					pk += 0xc;
					tw = edi + 0x58;
					U32(pk, 0) = 0x2000000;
					cmd = (tw != 0) ? s6_texwin(edi, 0x58) : 0;
					U32(pk, 4) = cmd;
					U32(pk, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ebx + U32(F0, 0x14) * 4, pk);
					uint32_t e20 = U32(F0, 0x20);
					uint32_t e1c = U32(F0, 0x1c);
					ebx = pk + 0xc;
					uint32_t e6c = U32(F0, 0x6c) + 8;
					e20 += 8;
					uint32_t cnt = U32(F0, 0x28);
					colp += 4;
					e1c += 4;
					cnt--;
					U32(F0, 0x6c) = e6c;
					U32(F0, 0x20) = e20;
					U32(F0, 0x1c) = e1c;
					U32(F0, 0x28) = cnt;
				} while (U32(F0, 0x28) != 0);
				U32(F0, 0x6c) = U32(F0, 0x6c) + 8;
				colp += 4;
				U32(F0, 0x10) = U32(F0, 0x10) + 1;
			} while ((int32_t)U32(F0, 0x10) < S16(edi, 0xcc));
		}

		uint32_t node = U32(F0, 0x68);
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = ebx;
		uint8_t par = (uint8_t)(U8(node, 0xde) + U8(node, 0x24));
		if ((par & 1) == 0 && S16(edi, 0xcc) >= 4 && U8(edi, 0x4c) >= 0x40)
		{
			uint32_t task = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_SurfaceSplashTask), 0xe0, node);
			uint32_t pt = task + 0x6e;
			U32(F0, 0x68) = 4;
			do
			{
				uint32_t r = x::CrtRand();
				int32_t col = (int32_t)r % 4;
				r = x::CrtRand();
				int32_t den = S16(edi, 0xcc) + 1;
				int32_t rowr = (int32_t)r % den;
				int32_t row;
				if (a_73B7E0(5) != 0)
					row = S16(edi, 0xcc) - rowr / 2;
				else
					row = rowr / 2;
				int32_t idx = add32(col, mul32(row, 4));
				uint32_t vp = (uint32_t)add32(mul32(idx, 8), game_addr(game::MAG_095_SurfaceVertexGrid));
				uint32_t q0 = U32(vp, 0);
				uint32_t q1 = U32(vp, 4);
				U32(F0, 0x2c) = q0;
				U32(F0, 0x30) = q1;

				r = x::CrtRand();
				int32_t dlt = S16(vp, 8) - S16(vp, 0);
				int32_t t = mul32((int32_t)(r & 0xf), dlt);
				uint16_t px = (uint16_t)(U16(F0, 0x2c) + (uint16_t)(t / 16));

				r = x::CrtRand();
				dlt = S16(vp, 0x22) - S16(vp, 2);
				t = mul32((int32_t)(r & 0xf), dlt);
				U16(F0, 0x2e) = (uint16_t)(U16(F0, 0x2e) + (uint16_t)(t / 16));

				r = x::CrtRand();
				dlt = S16(vp, 0x24) - S16(vp, 4);
				t = mul32((int32_t)(r & 0xf), dlt);
				U16(pt, -2) = px;
				uint16_t py = U16(F0, 0x2e);
				uint16_t pz = (uint16_t)(U16(F0, 0x30) + (uint16_t)(t / 16));
				U16(pt, 0) = py;
				U16(pt, 2) = pz;
				if (a_73B7E0(5) != 0)
				{
					r = x::CrtRand();
					U16(pt, 0) = (uint16_t)(U16(pt, 0) + (uint16_t)((r & 0x3ff) - 0x200));
				}
				pt += 8;
				U32(F0, 0x68) = U32(F0, 0x68) - 1;
			} while (U32(F0, 0x68) != 0);
		}
		return 0;
	}

	uint32_t __cdecl s_7466A0(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t states[3] = { game_addr(game::MAG_095_SurfaceSplash_State0_Init), game_addr(game::MAG_095_SurfaceSplash_State1_PlayUntilEnd), game_addr(game::MAG_095_SurfaceSplash_State2_Nop) };
		int32_t st = S8(node, 0x29);
		callp(states[st], node);
		for (int32_t i = 0; i < 4; i++)
		{
			uint32_t p0 = U32(node, i * 8 + 0x6c);
			U32(node, 0x1c) = p0;
			uint32_t p1 = U32(node, i * 8 + 0x70);
			U32(node, 0x20) = p1;
			if (i == 0)
				U32(node, 0x4c) = game_addr(game::unk_15334A0);
			else if (i == 1)
				U32(node, 0x4c) = game_addr(game::unk_1533328);
			else
				U32(node, 0x4c) = game_addr(game::unk_15331D0);
			if (a_73B7E0(5) != 0)
				s_7434C0(node);
			else
				s_744D30(node, 0x400);
		}
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((U8(node, 0x26) & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl s_746760(uint32_t a1)
	{
		uint32_t node = a1;
		uint8_t st = U8(node, 0x29);
		U16(node, 0x56) = 0xfff0;
		st++;
		U16(node, 0x52) = 0xa;
		U8(node, 0x29) = st;
		return 0;
	}

	uint32_t __cdecl s_7467B0(uint32_t a1)
	{
		uint32_t node = a1;
		s_745F50(node, s6_surface(node));
		U16(node, 0xdc) = (uint16_t)(U16(node, 0xdc) - 1);
		uint32_t r = a_73B7E0(7);
		if (r != 0 || !(S16(node, 0xdc) > 0))
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_746800(uint32_t a1)
	{
		uint32_t node = a1;
		s_745F50(node, s6_surface(node));
		uint32_t r = s_746870(s6_surface(node));
		if (r != 1)
			return 0;
		if (a_73B7E0(7) != 0)
		{
			uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		else
			U8(node, 0x29) = 1;
		return 0;
	}

	uint32_t __cdecl s_746870(uint32_t a1)
	{
		uint32_t b = a1;
		if (U8(b, 0x4c) == 0)
			return 1;
		uint8_t step;
		if (a_73B7E0(7) != 0)
		{
			uint8_t d = (uint8_t)(U8(b, 0x4c) + 0xf8);
			uint8_t c = (uint8_t)(U8(b, 0x4d) + 0xf8);
			U8(b, 0x4c) = d;
			d = (uint8_t)(U8(b, 0x4e) + 0xf8);
			U8(b, 0x4d) = c;
			U8(b, 0x4e) = d;
			step = 0xfc;
		}
		else
		{
			uint8_t d = (uint8_t)(U8(b, 0x4c) + 0xfc);
			uint8_t c = (uint8_t)(U8(b, 0x4d) + 0xfc);
			U8(b, 0x4c) = d;
			d = (uint8_t)(U8(b, 0x4e) + 0xfc);
			U8(b, 0x4d) = c;
			U8(b, 0x4e) = d;
			step = 0xfe;
		}
		uint8_t c = (uint8_t)(U8(b, 0x48) + step);
		uint8_t d = (uint8_t)(U8(b, 0x49) + step);
		U8(b, 0x48) = c;
		c = (uint8_t)(U8(b, 0x4a) + step);
		uint8_t crest = U8(b, 0x4c);
		U8(b, 0x49) = d;
		U8(b, 0x4a) = c;
		if (crest > 0xe0)
		{
			U8(b, 0x4a) = 0;
			U8(b, 0x49) = 0;
			U8(b, 0x48) = 0;
			U8(b, 0x4e) = 0;
			U8(b, 0x4d) = 0;
			U8(b, 0x4c) = 0;
		}
		return 0;
	}

	uint32_t __cdecl s_7469C0(uint32_t a1)
	{
		if (a_73B7E0(3) == 0)
			return 0;
		uint32_t node = a1;
		uint32_t p = game_addr(game::unk_1D98992);
		U16(node, 0x1c) = 0x1000;
		for (int32_t i = 0; i < 4; i++)
		{
			U16(p, 0) = 0x1000;
			p += 0x2c;
		}
		a_746A10();
		uint8_t st = U8(node, 0x29);
		U8(node, 0x26) |= 1;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl s_746AA0(uint32_t a1)
	{
		a_746AE0();
		uint32_t node = a1;
		uint32_t p = game_addr(game::unk_1D989BA);
		U16(node, 0x1c) = 0x800;
		for (int32_t i = 0; i < 4; i++)
		{
			U16(p, -0x28) = 0x800;
			U8(p, 0) = 0;
			U8(p, -1) = 0;
			U8(p, -2) = 0;
			p += 0x2c;
		}
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_746B10(uint32_t a1)
	{
		uint32_t node = a1;
		U16(node, 0x1c) = (uint16_t)(U16(node, 0x1c) + 0xfe00);
		if (!(S16(node, 0x1c) > 0))
		{
			uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			st++;
			U16(node, 0x1c) = 0;
			U8(node, 0x29) = st;
		}
		uint16_t val = U16(node, 0x1c);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int32_t i = 0; i < 4; i++)
		{
			U16(p, 0) = val;
			p += 0x2c;
		}
		return 0;
	}

	uint32_t __cdecl s_746B60(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t states[8] = { game_addr(game::MAG_095_CreatureActor_State0_Init), game_addr(game::MAG_095_CreatureActor_State1_LoopUntilPhase4), game_addr(game::MAG_095_CreatureActor_State2_SoundStreamAtFrame18), game_addr(game::MAG_095_CreatureActor_State3_DropletsBubbles), game_addr(game::MAG_095_CreatureActor_State4_OneShotAnim), game_addr(game::MAG_095_CreatureActor_State5_BubblesUntilPhase5), game_addr(game::MAG_095_CreatureActor_State6_AnimUntilDirPhase6), game_addr(game::MAG_095_CreatureActor_State7_Empty) };
		int32_t st = S8(node, 0x29);
		callp(states[st], node);
		if ((U8(node, 0x26) & 4) == 0)
		{
			uint32_t cur = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = a_746C10(node, game_addr(game::MAG_095_CreatureModel), cur);
		}
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((U8(node, 0x26) & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl s_746DF0(uint32_t a1)
	{
		uint32_t node = a1;
		U8(node, 0x5a) = 0x18;
		U8(node, 0x59) = 0x18;
		U8(node, 0x58) = 0x18;
		uint32_t scl = node + 0x114;
		U16(node, 0x4c) = 0;
		U16(node, 0x4e) = 0xfe20;
		U16(node, 0x50) = 0xcc0;
		U32(node, 0x60) = scl;
		U32(node, 0x11c) = 0x1000;
		U32(node, 0x118) = 0x1000;
		U32(scl, 0) = 0x1000;
		x::au_re_Battle_ReadAnimation_7(node, 0);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_746E50(uint32_t a1)
	{
		uint32_t node = a1;
		x::sub_8DD1C0(node);
		if (a_73B7E0(4) != 0)
		{
			U8(node, 0x5a) = 0x20;
			U8(node, 0x59) = 0x20;
			U8(node, 0x58) = 0x20;
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl s_746E80(uint32_t a1)
	{
		uint32_t script = MEM<uint32_t>(game_addr(game::unk_1533010));
		if (S16(script, 0x46) < 0x12)
			return 0;
		x::BdPlaySE(game_addr(game::unk_153301C), 1, 0x80);
		x::BdPlaySummonStream(0x80, 1, 0x7f);
		uint32_t node = a1;
		x::au_re_Battle_ReadAnimation_7(node, 1);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl s_746ED0(uint32_t a1)
	{
		uint32_t node = a1;
		int16_t fr = S16(node, 0x136);
		if (fr >= 0xa && fr <= 0x37)
			s_746F40(node);
		if (S16(node, 0x136) >= 0x1e)
			s_747160(node);
		x::sub_8DD1C0(node);
		uint8_t c = (uint8_t)(U8(node, 0x58) + 4);
		U8(node, 0x58) = c;
		if (c >= 0x80)
		{
			uint8_t st = U8(node, 0x29);
			U8(node, 0x58) = 0x80;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		uint8_t v = U8(node, 0x58);
		U8(node, 0x5a) = v;
		U8(node, 0x59) = v;
		return 0;
	}

	uint32_t __cdecl s_746F40(uint32_t a1)
	{
		for (int32_t ofs = 0; ofs < 0x20; )
		{
			uint32_t r = x::CrtRand();
			int32_t sel = (int32_t)r % 0xc;
			r = x::CrtRand();
			uint32_t ang = r & 0xfff;
			uint32_t task = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_DropletTask), 0xe0, a1);
			int32_t tabv = S16((uint32_t)add32(mul32((int16_t)sel, 2), game_addr(game::unk_1533DF0)), 0);
			x::GetEffectSpawnPosition(a1 + 0x30, (uint32_t)tabv, (uint32_t)(int32_t)(int16_t)ang, ofs + task + 0x6c);
			r = x::CrtRand();
			uint32_t spd = (r & 0xff) + 0x100;
			ofs += 8;
			U16(task, ofs + 0x86) = (uint16_t)spd;
		}
		return 0;
	}

	uint32_t __cdecl s_746FD0(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t states[3] = { game_addr(game::MAG_095_Droplet_State0_Init), game_addr(game::MAG_095_Droplet_State1_PlayUntilEnd), game_addr(game::MAG_095_Droplet_State2_Nop) };
		int32_t st = S8(node, 0x29);
		callp(states[st], node);
		for (int32_t i = 0; i < 4; i++)
		{
			int32_t o = i * 8;
			for (int32_t k = 0; k < 3; k++)
			{
				uint16_t w = U16(node, o + 0x8c + 2 * k);
				U16(node, o + 0x8c + 2 * k) = (uint16_t)(w - (uint16_t)((int32_t)(int16_t)w / 4));
			}
			U16(node, o + 0x6c) = (uint16_t)(U16(node, o + 0x6c) + (uint16_t)((int32_t)S16(node, o + 0x8c) / 16));
			U16(node, o + 0x6e) = (uint16_t)(U16(node, o + 0x6e) + (uint16_t)((int32_t)S16(node, o + 0x8e) / 16));
			U16(node, o + 0x70) = (uint16_t)(U16(node, o + 0x70) + (uint16_t)((int32_t)S16(node, o + 0x90) / 16));
			uint32_t p0 = U32(node, o + 0x6c);
			U32(node, 0x1c) = p0;
			uint32_t p1 = U32(node, o + 0x70);
			U32(node, 0x20) = p1;
			U32(node, 0x4c) = (i == 0) ? 0x15336c4u : game_addr(game::unk_15334A0);
			s_744D30(node, 0x400);
		}
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((U8(node, 0x26) & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl s_747110(uint32_t a1)
	{
		uint32_t node = a1;
		uint8_t st = U8(node, 0x29);
		U16(node, 0x56) = 0xfff0;
		st++;
		U16(node, 0x52) = 0xa;
		U8(node, 0x29) = st;
		return 0;
	}

	uint32_t __cdecl s_747160(uint32_t a1)
	{
		for (int32_t ofs = 0; ofs < 0x10; )
		{
			uint32_t r = x::CrtRand();
			int32_t sel = (int32_t)r % 4;
			r = x::CrtRand();
			uint32_t ang = r & 0xfff;
			uint32_t task = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_095_EffectQueueA), game_addr(game::MAG_095_BubbleTask), 0xe0, a1);
			int32_t tabv = S16((uint32_t)add32(mul32((int16_t)sel, 2), game_addr(game::unk_1533E08)), 0);
			x::GetEffectSpawnPosition(a1 + 0x30, (uint32_t)tabv, (uint32_t)(int32_t)(int16_t)ang, ofs + task + 0x6c);
			r = x::CrtRand();
			U16(task, ofs + 0x8c) = (uint16_t)((r & 0x3ff) - 0x200);
			r = x::CrtRand();
			U16(task, ofs + 0x8e) = (uint16_t)((r & 0x3ff) - 0x200);
			r = x::CrtRand();
			ofs += 8;
			U16(task, ofs + 0x88) = (uint16_t)((r & 0x3ff) - 0x200);
		}
		return 0;
	}

	uint32_t __cdecl s_747220(uint32_t a1)
	{
		uint32_t node = a1;
		uint32_t states[3] = { game_addr(game::MAG_095_Bubble_State0_Init), game_addr(game::MAG_095_Bubble_State1_PlayUntilEnd), game_addr(game::MAG_095_Bubble_State2_Nop) };
		int32_t st = S8(node, 0x29);
		callp(states[st], node);
		uint32_t v = node + 0x8e;
		for (int32_t n = 4; n != 0; n--)
		{
			uint16_t w = U16(v, -2);
			U16(v, -2) = (uint16_t)(w - (uint16_t)((int32_t)(int16_t)w / 4));
			w = U16(v, 0);
			U16(v, 0) = (uint16_t)(w - (uint16_t)((int32_t)(int16_t)w / 4));
			w = U16(v, 2);
			U16(v, 2) = (uint16_t)(w - (uint16_t)((int32_t)(int16_t)w / 4));
			U16(v, -0x22) = (uint16_t)(U16(v, -0x22) + (uint16_t)((int32_t)S16(v, -2) / 16));
			U16(v, -0x20) = (uint16_t)(U16(v, -0x20) + (uint16_t)((int32_t)S16(v, 0) / 16));
			U16(v, -0x1e) = (uint16_t)(U16(v, -0x1e) + (uint16_t)((int32_t)S16(v, 2) / 16));
			uint32_t p0 = U32(v, -0x22);
			U32(node, 0x1c) = p0;
			uint32_t p1 = U32(v, -0x1e);
			U32(node, 0x20) = p1;
			s_744D30(node, 0x800);
			v += 8;
		}
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((U8(node, 0x26) & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl s_747320(uint32_t a1)
	{
		uint32_t node = a1;
		uint8_t st = U8(node, 0x29);
		U32(node, 0x4c) = game_addr(game::unk_15334A0);
		st++;
		U16(node, 0x56) = 0xfff0;
		U16(node, 0x52) = 0xa;
		U8(node, 0x29) = st;
		return 0;
	}

	uint32_t __cdecl s_747370(uint32_t a1)
	{
		uint32_t node = a1;
		int16_t fr = S16(node, 0x136);
		if (fr >= 0xa && fr <= 0x37)
			s_746F40(node);
		if (S16(node, 0x136) >= 0x1e)
			s_747160(node);
		if (x::au_re_Battle_ReadAnimation_8(node) == 1)
		{
			x::au_re_Battle_ReadAnimation_7(node, 2);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl s_7473D0(uint32_t a1)
	{
		uint32_t node = a1;
		s_747160(node);
		x::sub_8DD1C0(node);
		if (a_73B7E0(5) != 0)
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

namespace siren
{
	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort PORTS[] = {
		{ game::MAG_095_Sequence_State2_CarveScratchArenas, (void *)s_73A0F0, "095 MAG_095_sub_73A0F0" },
		{ game::MAG_095_ClearScratchArenas, (void *)s_73A130, "095 MAG_095_sub_73A130" },
		{ game::MAG_095_ActEng_Emitter_State0_SpawnFileLoadScript, (void *)s_73A220, "095 MAG_095_sub_73A220" },
		{ game::MAG_095_BuildAssetTable, (void *)s_73A270, "095 MAG_095_sub_73A270" },
		{ game::MAG_095_InitDirectorScriptBlock, (void *)s_73A6E0, "095 MAG_095_sub_73A6E0" },
		{ game::Siren_DirectorTask, (void *)s_73A7B0, "095 MAG_095_sub_73A7B0" },
		{ game::Siren_CutScriptTick, (void *)s_73A8E0, "095 sub_73A8E0" },
		{ game::MAG_095_Siren_Director_State1_SpawnCameraScript, (void *)s_73A990, "095 sub_73A990" },
		{ game::Siren_CameraScriptTask, (void *)s_73A9E0, "095 sub_73A9E0" },
		{ game::MAG_095_Siren_CameraScript_State0_StartPathAtCut1, (void *)s_73AAA0, "095 sub_73AAA0" },
		{ game::MAG_095_Siren_CameraScript_State2_StartPathAtCut2, (void *)s_73B2F0, "095 sub_73B2F0" },
		{ game::MAG_095_Siren_CameraScript_State3_StepThenPathAtCut3, (void *)s_73B320, "095 sub_73B320" },
		{ game::MAG_095_Siren_CameraScript_State5_StepThenCamSequence, (void *)s_73B380, "095 sub_73B380" },
		{ game::MAG_095_Siren_CameraScript_State6_StepCamSequence, (void *)s_73B480, "095 sub_73B480" },
		{ game::MAG_095_Siren_CameraScript_State8_StartPathAtCut5, (void *)s_73B670, "095 sub_73B670" },
		{ game::MAG_095_Siren_CameraScript_State9_StepCountdown, (void *)s_73B6B0, "095 sub_73B6B0" },
		{ game::MAG_095_Siren_CameraScript_State10_StepRequestCut6, (void *)s_73B6D0, "095 sub_73B6D0" },
		{ game::MAG_095_Siren_CameraScript_State11_StepThenPathAtCut6, (void *)s_73B6F0, "095 sub_73B6F0" },
		{ game::MAG_095_Siren_CameraScript_State12_StepUntilCut8, (void *)s_73B730, "095 sub_73B730" },
		{ game::MAG_095_Siren_Director_State3_PlaySoundAtCut1, (void *)s_73B7B0, "095 sub_73B7B0" },
		{ game::MAG_095_Siren_Director_State4_ToggleUntilLoaded, (void *)s_73B800, "095 sub_73B800" },
		{ game::MAG_095_SetSirenToggle, (void *)s_73B840, "095 sub_73B840" },
		{ game::MAG_095_Siren_Director_State5_WaitCutFrame1E, (void *)s_73B880, "095 sub_73B880" },
		{ game::MAG_095_Siren_Director_State9_SpawnScreenTint, (void *)s_73B900, "095 sub_73B900" },
		{ game::MAG_095_Siren_Director_State10_CountdownToggle, (void *)s_73B950, "095 sub_73B950" },
		{ game::MAG_095_Siren_Director_State11_RequestCut3PlaySound, (void *)s_73B9A0, "095 sub_73B9A0" },
		{ game::MAG_095_Siren_Director_State12_HideMonstersAtCut3, (void *)s_73B9D0, "095 sub_73B9D0" },
		{ game::MAG_095_Siren_Director_State13_WaitCutFrame2D, (void *)s_73BA30, "095 sub_73BA30" },
		{ game::MAG_095_Siren_Director_State14_RequestCut4, (void *)s_73BA50, "095 sub_73BA50" },
		{ game::MAG_095_Siren_Director_State15_RestoreMonstersAtCut5, (void *)s_73BA70, "095 sub_73BA70" },
		{ game::MAG_095_Siren_Director_State16_PlaySoundAtCut6, (void *)s_73BAD0, "095 sub_73BAD0" },
		{ game::MAG_095_Siren_Director_State18_PauseUntilLoaded, (void *)s_73BB20, "095 sub_73BB20" },
		{ game::MAG_095_Siren_Director_State19_WaitCutFrame1E, (void *)s_73BBB0, "095 sub_73BBB0" },
		{ game::MAG_095_Siren_Director_State21_WaitCut7, (void *)s_73BBF0, "095 sub_73BBF0" },
		{ game::MAG_095_Siren_Director_State22_SetEndFlagAtFrame1, (void *)s_73BC10, "095 sub_73BC10" },
		{ game::MAG_095_Siren_Director_State24_RequestCut8ReleaseVoice, (void *)s_73BC60, "095 sub_73BC60" },
		{ game::MAG_095_Siren_Director_State25_WaitCut8, (void *)s_73BC90, "095 sub_73BC90" },
		{ game::Siren_CutSpawner, (void *)s_73BCF0, "095 sub_73BCF0" },
		{ game::MAG_095_SpawnModelEmitterTask, (void *)s_73C150, "095 sub_73C150" },
		{ game::MAG_095_Cut1ModelEmitterTask, (void *)s_73C1A0, "095 sub_73C1A0" },
		{ game::MAG_095_Cut1ModelEmitter_State0_InitDraw, (void *)s_73C220, "095 sub_73C220" },
		{ game::Siren_RenderModelInstance, (void *)s_73C3B0, "095 sub_73C3B0" },
		{ game::MAG_095_RenderPrimModel, (void *)s_73C790, "095 sub_73C790" },
		{ game::MAG_095_PrimList_TexTri, (void *)s_73CD70, "095 sub_73CD70" },
		{ game::MAG_095_PrimList_TexQuad, (void *)s_73D1F0, "095 sub_73D1F0" },
		{ game::MAG_095_PrimList_GT4, (void *)s_73E1A0, "095 sub_73E1A0" },
		{ game::MAG_095_CopyRecordToKeyTable25, (void *)s_73E7F0, "095 sub_73E7F0" },
		{ game::MAG_095_CopyRecordToKeyTables16, (void *)s_73EB10, "095 sub_73EB10" },
		{ game::MAG_095_Cut1ModelEmitter_State1_DrawUntilCut2, (void *)s_73EE10, "095 sub_73EE10" },
		{ game::MAG_095_ModelTaskA, (void *)s_73EE50, "095 sub_73EE50" },
		{ game::MAG_095_ModelTaskA_State0_InitAtGfPos, (void *)s_73EEC0, "095 sub_73EEC0" },
		{ game::MAG_095_ModelTaskA_State1_DrawUntilPhase3, (void *)s_73EF20, "095 sub_73EF20" },
		{ game::MAG_095_ModelTaskB, (void *)s_73EF60, "095 sub_73EF60" },
		{ game::MAG_095_ModelTaskB_State0_Init, (void *)s_73EFE0, "095 sub_73EFE0" },
		{ game::MAG_095_ModelTaskB_State1_DrawUntilPhase5, (void *)s_73F030, "095 sub_73F030" },
		{ game::MAG_095_ModelTaskB_State2_FadeUntilPhase8, (void *)s_73F070, "095 sub_73F070" },
		{ game::MAG_095_ModelTaskB_State3_FadeOutFinish, (void *)s_73F0C0, "095 sub_73F0C0" },
		{ game::MAG_095_ModelTaskC_State0_Init, (void *)s_73F180, "095 sub_73F180" },
		{ game::MAG_095_ModelTaskC_State1_WaitFrame18, (void *)s_73F1D0, "095 sub_73F1D0" },
		{ game::MAG_095_ModelTaskC_State2_ResizeAtPhase4, (void *)s_73F1E0, "095 sub_73F1E0" },
		{ game::MAG_095_ModelTaskC_State3_DrawUntilEnd, (void *)s_73F220, "095 sub_73F220" },
		{ game::MAG_095_ModelTaskD, (void *)s_73F250, "095 sub_73F250" },
		{ game::MAG_095_ModelTaskD_State0_Init, (void *)s_73F2B0, "095 sub_73F2B0" },
		{ game::MAG_095_ModelTaskD_State1_WaitFrame32, (void *)s_73F300, "095 sub_73F300" },
		{ game::MAG_095_ModelTaskD_State2_FaceCameraUntilPhase5, (void *)s_73F310, "095 sub_73F310" },
		{ game::MAG_095_ModelTaskE_State0_Init, (void *)s_73F3E0, "095 sub_73F3E0" },
		{ game::MAG_095_ModelTaskE_State1_WaitFrame50, (void *)s_73F430, "095 sub_73F430" },
		{ game::MAG_095_ModelTaskE_State2_FaceCameraResizeAtPhase5, (void *)s_73F440, "095 sub_73F440" },
		{ game::MAG_095_ModelTaskE_State3_FaceCameraUntilPhase6, (void *)s_73F4A0, "095 sub_73F4A0" },
		{ game::MAG_095_ModelTaskF_State0_Init, (void *)s_73F570, "095 sub_73F570" },
		{ game::MAG_095_ModelTaskF_State1_WaitFrame82, (void *)s_73F5C0, "095 sub_73F5C0" },
		{ game::MAG_095_ModelTaskF_State2_MoveToGfAtPhase6, (void *)s_73F5D0, "095 sub_73F5D0" },
		{ game::MAG_095_ModelTaskF_State3_FaceCameraUntilPhase9, (void *)s_73F640, "095 sub_73F640" },
		{ game::MAG_095_ModelTaskG, (void *)s_73F6A0, "095 sub_73F6A0" },
		{ game::MAG_095_ModelTaskG_State0_Init, (void *)s_73F710, "095 sub_73F710" },
		{ game::MAG_095_ModelTaskG_State1_WaitFrame1E, (void *)s_73F760, "095 sub_73F760" },
		{ game::MAG_095_ModelTaskG_State2_DrawUntilEnd, (void *)s_73F770, "095 sub_73F770" },
		{ game::MAG_095_ModelTaskG_State3_RestartAtFrame89, (void *)s_73F790, "095 sub_73F790" },
		{ game::MAG_095_ModelTaskG_State4_DrawUntilPhase6, (void *)s_73F7C0, "095 sub_73F7C0" },
		{ game::MAG_095_TargetModel_State0_InitAtTarget, (void *)s_73F860, "095 sub_73F860" },
		{ game::MAG_095_TargetModel_State1_DrawUntilEnd, (void *)s_73F8D0, "095 sub_73F8D0" },
		{ game::MAG_095_ParticleSystem_State0_StartAtFrame, (void *)s_73F960, "095 sub_73F960" },
		{ game::MAG_095_ActSys_Step, (void *)s_740470, "095 sub_740470" },
		{ game::MAG_095_ActSys_DrawPass, (void *)s_740500, "095 sub_740500" },
		{ game::MAG_095_ActSys_UpdatePass, (void *)s_740840, "095 sub_740840" },
		{ game::MAG_095_ActSys_UpdateEmitter, (void *)s_741B10, "095 sub_741B10" },
		{ game::MAG_095_ActSys_EmitterSpawnStep, (void *)s_741B90, "095 sub_741B90" },
		{ game::MAG_095_ActSys_SpawnParticles, (void *)s_741C10, "095 sub_741C10" },
		{ game::MAG_095_ActSys_AllocParticleSlot, (void *)s_7423F0, "095 sub_7423F0" },
		{ game::MAG_095_ParticleSystem_State1_StepUntilDone, (void *)s_743220, "095 sub_743220" },
		{ game::MAG_095_BoxSparkleSpawner_State1_Spawn, (void *)s_7432C0, "095 sub_7432C0" },
		{ game::MAG_095_BoxSparkleTask, (void *)s_7433C0, "095 sub_7433C0" },
		{ game::MAG_095_DrawSpriteNode, (void *)s_7434C0, "095 sub_7434C0" },
		{ game::MAG_095_BoxSparkle_State0_PickSprite, (void *)s_743B90, "095 sub_743B90" },
		{ game::MAG_095_OrbitSparkleSpawnerTask, (void *)s_743C70, "095 sub_743C70" },
		{ game::MAG_095_OrbitSparkleSpawner_State0_Anchor, (void *)s_743CD0, "095 sub_743CD0" },
		{ game::MAG_095_OrbitSparkleSpawner_State2_SpawnOddFrames, (void *)s_743D10, "095 sub_743D10" },
		{ game::MAG_095_OrbitSparkleTask, (void *)s_743DD0, "095 sub_743DD0" },
		{ game::MAG_095_OrbitSparkle_State0_Init, (void *)s_743FD0, "095 sub_743FD0" },
		{ game::MAG_095_OrbitSparkle_State1_UntilEndOrPhase2, (void *)s_744010, "095 sub_744010" },
		{ game::MAG_095_BurstSparkleSpawnerTask, (void *)s_744060, "095 sub_744060" },
		{ game::MAG_095_BurstSparkleSpawner_State0_Anchor, (void *)s_7440C0, "095 sub_7440C0" },
		{ game::MAG_095_BurstSparkleSpawner_State2_Spawn5, (void *)s_744100, "095 sub_744100" },
		{ game::MAG_095_BurstSparkleTask, (void *)s_7441D0, "095 sub_7441D0" },
		{ game::MAG_095_BurstSparkle_State0_Init, (void *)s_7443C0, "095 sub_7443C0" },
		{ game::MAG_095_BurstSparkle_State1_UntilEndOrPhase2, (void *)s_744400, "095 sub_744400" },
		{ game::MAG_095_KeyPathSparkleSpawnerTask, (void *)s_744450, "095 sub_744450" },
		{ game::MAG_095_KeyPathSparkleSpawner_State0_Anchor, (void *)s_7444B0, "095 sub_7444B0" },
		{ game::MAG_095_KeyPathSparkleSpawner_State2_Spawn3, (void *)s_7444F0, "095 sub_7444F0" },
		{ game::MAG_095_KeyPathSparkleTask, (void *)s_744570, "095 sub_744570" },
		{ game::MAG_095_KeyPathSparkle_State0_Init, (void *)s_744730, "095 sub_744730" },
		{ game::MAG_095_KeyPathSparkle_State1_UntilEndOrPhase3, (void *)s_744770, "095 sub_744770" },
		{ game::MAG_095_TargetRiseSpawner_State0_WaitTargetHeight, (void *)s_744820, "095 sub_744820" },
		{ game::MAG_095_TargetRiseSpawner_State1_SpawnRisingAndSplash, (void *)s_7448A0, "095 sub_7448A0" },
		{ game::MAG_095_RisingSparkleTask, (void *)s_744AA0, "095 sub_744AA0" },
		{ game::MAG_095_RisingSparkle_State0_Init, (void *)s_744BA0, "095 sub_744BA0" },
		{ game::MAG_095_RisingSparkle_State1_Play, (void *)s_744BE0, "095 sub_744BE0" },
		{ game::MAG_095_SplashSparkleTask, (void *)s_744C30, "095 sub_744C30" },
		{ game::MAG_095_DrawSequenceSprite, (void *)s_744D30, "095 sub_744D30" },
		{ game::MAG_095_SplashSparkle_State0_Init, (void *)s_744E80, "095 sub_744E80" },
		{ game::MAG_095_SplashSparkle_State1_LoopRepeat, (void *)s_744ED0, "095 sub_744ED0" },
		{ game::MAG_095_SplashSparkle_State2_PlayLast, (void *)s_744F30, "095 sub_744F30" },
		{ game::MAG_095_SpawnScriptTask, (void *)s_744F90, "095 sub_744F90" },
		{ game::MAG_095_SpawnScript_State0_Origin, (void *)s_744FF0, "095 sub_744FF0" },
		{ game::MAG_095_SpawnScript_State1_ResetCursor, (void *)s_745010, "095 sub_745010" },
		{ game::MAG_095_SpawnScript_State2_RunScript, (void *)s_745030, "095 sub_745030" },
		{ game::MAG_095_ScriptParticleSystem_State0_StartAtFrame, (void *)s_7451E0, "095 sub_7451E0" },
		{ game::MAG_095_ScriptParticleSystem_State1_StepUntilDone, (void *)s_745210, "095 sub_745210" },
		{ game::MAG_095_OrbitSpriteSpawner_State0_Spawn, (void *)s_7452B0, "095 sub_7452B0" },
		{ game::MAG_095_OrbitSpriteTask, (void *)s_7452F0, "095 sub_7452F0" },
		{ game::MAG_095_OrbitSprite_Draw, (void *)s_745380, "095 sub_745380" },
		{ game::MAG_095_OrbitSprite_State0_Init, (void *)s_745490, "095 sub_745490" },
		{ game::MAG_095_OrbitSprite_State1_FadeIn, (void *)s_7455B0, "095 sub_7455B0" },
		{ game::MAG_095_OrbitSprite_State2_Hold, (void *)s_7455F0, "095 sub_7455F0" },
		{ game::MAG_095_OrbitSprite_State3_FadeOut, (void *)s_745610, "095 sub_745610" },
		{ game::MAG_095_OrbitSpriteSpawner_State1_WaitLoopUntilPhase7, (void *)s_745650, "095 sub_745650" },
		{ game::MAG_095_TargetSparkSpawnerTask, (void *)s_7456A0, "095 sub_7456A0" },
		{ game::MAG_095_TargetSparkSpawner_State0_AtTarget, (void *)s_745700, "095 sub_745700" },
		{ game::MAG_095_TargetSparkSpawner_State1_WaitFrame16, (void *)s_745730, "095 sub_745730" },
		{ game::MAG_095_TargetSparkSpawner_State2_Spawn, (void *)s_745740, "095 sub_745740" },
		{ game::MAG_095_TargetSparkTask, (void *)s_745840, "095 sub_745840" },
		{ game::MAG_095_TargetSpark_State0_Init, (void *)s_7459D0, "095 sub_7459D0" },
		{ game::MAG_095_TargetSpark_State1_FadeIn, (void *)s_745A20, "095 sub_745A20" },
		{ game::MAG_095_TargetSpark_State2_Hold, (void *)s_745A60, "095 sub_745A60" },
		{ game::MAG_095_TargetSpark_State3_FadeOut, (void *)s_745A80, "095 sub_745A80" },
		{ game::Siren_WaterSurfaceTask, (void *)s_745AD0, "095 sub_745AD0" },
		{ game::MAG_095_Siren_WaterSurface_State0_PlaceAndWait, (void *)s_745B40, "095 sub_745B40" },
		{ game::MAG_095_Siren_WaterSurface_State1_InitBlock, (void *)s_745BB0, "095 sub_745BB0" },
		{ game::MAG_095_WaterSurface_InitBlock, (void *)s_745BF0, "095 sub_745BF0" },
		{ game::MAG_095_Siren_WaterSurface_State2_Grow, (void *)s_745E80, "095 sub_745E80" },
		{ game::Siren_WaterSurfaceUpdateDraw, (void *)s_745F50, "095 sub_745F50" },
		{ game::MAG_095_SurfaceSplashTask, (void *)s_7466A0, "095 sub_7466A0" },
		{ game::MAG_095_SurfaceSplash_State0_Init, (void *)s_746760, "095 sub_746760" },
		{ game::MAG_095_Siren_WaterSurface_State3_Hold, (void *)s_7467B0, "095 sub_7467B0" },
		{ game::MAG_095_Siren_WaterSurface_State4_Fade, (void *)s_746800, "095 sub_746800" },
		{ game::MAG_095_WaterSurface_FadeRows, (void *)s_746870, "095 sub_746870" },
		{ game::MAG_095_StageWobbleIn_State1_SetAtPhase3, (void *)s_7469C0, "095 sub_7469C0" },
		{ game::MAG_095_StageWobbleOut_State0_Set800, (void *)s_746AA0, "095 sub_746AA0" },
		{ game::MAG_095_StageWobbleOut_State1_FadeOut, (void *)s_746B10, "095 sub_746B10" },
		{ game::GF_095Siren_CreatureActorTask, (void *)s_746B60, "095 GF_095Siren_CreatureActorTask" },
		{ game::MAG_095_CreatureActor_State0_Init, (void *)s_746DF0, "095 sub_746DF0" },
		{ game::MAG_095_CreatureActor_State1_LoopUntilPhase4, (void *)s_746E50, "095 sub_746E50" },
		{ game::MAG_095_CreatureActor_State2_SoundStreamAtFrame18, (void *)s_746E80, "095 sub_746E80" },
		{ game::MAG_095_CreatureActor_State3_DropletsBubbles, (void *)s_746ED0, "095 sub_746ED0" },
		{ game::MAG_095_SpawnDroplets, (void *)s_746F40, "095 sub_746F40" },
		{ game::MAG_095_DropletTask, (void *)s_746FD0, "095 sub_746FD0" },
		{ game::MAG_095_Droplet_State0_Init, (void *)s_747110, "095 sub_747110" },
		{ game::MAG_095_SpawnBubbles, (void *)s_747160, "095 sub_747160" },
		{ game::MAG_095_BubbleTask, (void *)s_747220, "095 sub_747220" },
		{ game::MAG_095_Bubble_State0_Init, (void *)s_747320, "095 sub_747320" },
		{ game::MAG_095_CreatureActor_State4_OneShotAnim, (void *)s_747370, "095 sub_747370" },
		{ game::MAG_095_CreatureActor_State5_BubblesUntilPhase5, (void *)s_7473D0, "095 sub_7473D0" },
		{ {}, nullptr, nullptr }
	};
}
}
	void register_mag095_siren()
	{
		act::register_module(95);
		for (const act::siren::ModPort *p = act::siren::PORTS; p->addr; p++)
			act::register_module_port(95, p->addr, p->port, p->name);
	}
}

