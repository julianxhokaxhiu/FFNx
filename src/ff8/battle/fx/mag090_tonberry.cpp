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

// Effect 090: Tonberry - Chef's Knife

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
	namespace xm
	{
		static inline uint32_t BdPlayStream(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t, uint32_t, uint32_t)>(game_addr(game::BdPlayStream))(a1, a2, a3, a4); }
		static inline uint32_t sub_8DE160(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::sub_8DE160))(a1, a2); }
		static inline uint32_t sub_8DE1F0(uint32_t a1, uint32_t a2) { return fn<uint32_t (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::sub_8DE1F0))(a1, a2); }
	}
	uint32_t __cdecl t_7624D0(uint32_t a1);
	uint32_t __cdecl t_762650(uint32_t a1);
	uint32_t __cdecl t_762710(uint32_t a1);
	uint32_t __cdecl t_762BB0(uint32_t a1);
	uint32_t __cdecl t_762C60(uint32_t a1);
	uint32_t __cdecl t_762D10(uint32_t a1);
	uint32_t __cdecl t_762D60(uint32_t a1);
	uint32_t __cdecl t_762E60(uint32_t a1);
	uint32_t __cdecl t_762E90(uint32_t a1);
	uint32_t __cdecl t_763690(uint32_t a1);
	uint32_t __cdecl t_763790(uint32_t a1);
	uint32_t __cdecl t_763970(uint32_t a1);
	uint32_t __cdecl t_7639E0(uint32_t a1);
	uint32_t __cdecl t_763A30(uint32_t a1);
	uint32_t __cdecl t_763AA0(uint32_t a1);
	uint32_t __cdecl t_763AC0(uint32_t a1);
	uint32_t __cdecl t_763B10(uint32_t a1);
	uint32_t __cdecl t_763B90(uint32_t a1);
	uint32_t __cdecl t_763BD0(uint32_t a1);
	uint32_t __cdecl t_763C90(uint32_t a1);
	uint32_t __cdecl t_763CC0(uint32_t a1);
	uint32_t __cdecl t_763CF0(uint32_t a1);
	uint32_t __cdecl t_763D60(uint32_t a1);
	uint32_t __cdecl t_763E60(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl t_763EB0(uint32_t a1);
	uint32_t __cdecl t_763F80(uint32_t a1);
	uint32_t __cdecl t_764020(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl t_764070(uint32_t a1);
	uint32_t __cdecl t_764100(uint32_t a1);
	uint32_t __cdecl t_764260(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl t_764540(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl t_764670(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl t_764890(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl t_764B20(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl t_765010(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl t_765B30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl t_766090(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl t_766760(uint32_t a1);
	uint32_t __cdecl t_7667B0(uint32_t a1);
	uint32_t __cdecl t_766830(uint32_t a1, uint32_t a2);
	uint32_t __cdecl t_766EB0(uint32_t a1);
	uint32_t __cdecl t_766F00(uint32_t a1);
	uint32_t __cdecl t_766F20(uint32_t a1);
	uint32_t __cdecl t_766FB0(uint32_t a1);
	uint32_t __cdecl t_767020(uint32_t a1);
	uint32_t __cdecl t_767070(uint32_t a1);
	uint32_t __cdecl t_767090(uint32_t a1);
	uint32_t __cdecl t_767150(uint32_t a1);
	uint32_t __cdecl t_767190(uint32_t a1);
	uint32_t __cdecl t_7671D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
	uint32_t __cdecl t_767290(uint32_t a1);
	uint32_t __cdecl t_7672F0(uint32_t a1);
	uint32_t __cdecl t_767410(uint32_t a1);
	uint32_t __cdecl t_767430(uint32_t a1);
	uint32_t __cdecl t_767460(uint32_t a1);
	uint32_t __cdecl t_767480(uint32_t a1);
	uint32_t __cdecl t_7674E0(uint32_t a1);
	uint32_t __cdecl t_7677F0(uint32_t a1);
	uint32_t __cdecl t_767870(uint32_t a1);
	uint32_t __cdecl t_7678B0(uint32_t a1, uint32_t a2);
	uint32_t __cdecl t_7679D0(uint32_t a1);
	uint32_t __cdecl t_767A40(uint32_t a1);
	uint32_t __cdecl t_767BA0(uint32_t a1);
	uint32_t __cdecl t_767C00(uint32_t a1);
	uint32_t __cdecl t_767C70(uint32_t a1);
	uint32_t __cdecl t_767CD0(uint32_t a1);
	uint32_t __cdecl t_767D40(uint32_t a1);
	uint32_t __cdecl t_767DF0(uint32_t a1);
	uint32_t __cdecl t_767E20(uint32_t a1);
	uint32_t __cdecl t_767E60(uint32_t a1);
	uint32_t __cdecl t_767F20(uint32_t a1);
	uint32_t __cdecl t_767FE0(uint32_t a1);
	uint32_t __cdecl t_768030(uint32_t a1);
	uint32_t __cdecl t_768080(uint32_t a1);
	uint32_t __cdecl t_7680B0(uint32_t a1);
	uint32_t __cdecl t_7680D0(uint32_t a1);
	uint32_t __cdecl t_768130(uint32_t a1);
	uint32_t __cdecl t_768210(uint32_t a1);
	uint32_t __cdecl t_768250(uint32_t a1);
	uint32_t __cdecl t_768300(uint32_t a1);
	uint32_t __cdecl t_768320(uint32_t a1);
	uint32_t __cdecl t_768370(uint32_t a1);

	namespace
	{
		inline uint32_t t1_data() { return MEM<uint32_t>(game_addr(game::unk_1547168)); }
		inline void t1_next_state(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }
		inline uint32_t t1_task_end(uint32_t node, uint8_t status)
		{
			if ((status & 1) != 0 && U8(node, 0x28) == 0)
			{
				x::Effect_ReleaseLinkedTask(node);
				return 2;
			}
			return 0;
		}
		inline bool t1_out_x(int16_t v) { return v < 0 || v > 0xA00; }
		inline bool t1_out_y(int16_t v) { return v < 0 || v > 0x6C0; }
	}

	uint32_t __cdecl t_7624D0(uint32_t a1)
	{
		set_mod_by_code(U32(a1, 8));

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::unk_25A4C08)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_259EFB8)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_090_Tonberry_Master_State0_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_090_Tonberry_Master_State1_Advance);
		states[2] = game_addr(game::MAG_090_Tonberry_Master_State2_WaitNoChildren);
		states[3] = game_addr(game::MAG_090_Tonberry_Master_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_090_Tonberry_Master_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_090_Tonberry_Master_State5_Advance);
		states[6] = game_addr(game::MAG_090_Tonberry_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_090_Tonberry_Master_State7_RequestBattleSync);
		states[8] = game_addr(game::MAG_090_Tonberry_Master_State8_WaitBattleSyncDone);
		states[9] = game_addr(game::MAG_090_Tonberry_Master_State9_Finish);
		states[10] = game_addr(game::MAG_090_Tonberry_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_25A2CF4));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_25A2B74));
			MEM<uint32_t>(game_addr(game::MAG_090_PacketCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_259EFB4)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_25A2CF0));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_25A2B70));
			MEM<uint32_t>(game_addr(game::MAG_090_PacketCursor)) = v1;
			MEM<uint32_t>(game_addr(game::unk_259EFB4)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		static constexpr game::Id queues[6] = { game::MAG_090_EmitterQueue, game::MAG_090_DirectorQueue, game::MAG_090_LoadScriptQueue, game::MAG_090_CreatureQueue, game::MAG_090_TaskQueue, game::MAG_090_PrimModelQueue };
		for (int i = 0; i < 6; i++)
		{
			const uint32_t n = x::ExecuteTaskQueue(game_addr(queues[i]));
			U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)n);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return t1_task_end(node, status);
	}

	uint32_t __cdecl t_762650(uint32_t a1)
	{
		if (U8(a1, 0x28) == 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl t_762710(uint32_t a1)
	{
		t_762BB0(a1);
		a_7322A0();
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_090_LoadScriptQueue), game_addr(game::MAG_090_FileLoadScriptTask), 0x30, a1);
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_090_DirectorQueue), game_addr(game::Tonberry_DirectorTask), 0x48, a1);
		t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl t_762BB0(uint32_t a1)
	{
		x::MAG_007_sub_8DCC00(t1_data(), 0x54);
		{
			const uint32_t spawn = t1_data() + 8;
			const uint32_t slot = U8(a1, 0x2D);
			x::GetEffectSpawnPosition(game_addr(game::BattleEntitySlotData) + slot * 0x9C, 0xF0, 0, spawn);
		}
		const uint32_t d = t1_data();
		U16(d, 0xA) = 0;
		U32(d, 0) = U32(d, 8);
		U32(d, 4) = U32(d, 0xC);
		U16(d, 4) = (uint16_t)(U16(d, 4) + 0xC00);
		const uint16_t dx = (uint16_t)(U16(d, 8) - U16(d, 0));
		const uint16_t dz = (uint16_t)(U16(d, 0xC) - U16(d, 4));
		const uint32_t ang = x::CartesianToGameAngle((uint32_t)(int32_t)(int16_t)dx, (uint32_t)(int32_t)(int16_t)dz);
		const uint32_t d2 = t1_data();
		U16(d2, 0x4A) = (uint16_t)((ang - 0x800) & 0xFFF);
		const uint32_t slot = U8(a1, 0x2D);
		const uint16_t w = MEM<uint16_t>(game_addr(game::unk_1D972E6) + slot * 0x9C);
		U16(d2, 0x4E) = w;
		return 0;
	}

	uint32_t __cdecl t_762C60(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[12];
		states[0] = game_addr(game::MAG_090_Tonberry_Director_State0_ClaimVoiceSpawnScript);
		states[1] = game_addr(game::MAG_090_Tonberry_Director_State1_WaitDirGate1);
		states[2] = game_addr(game::MAG_090_Tonberry_Director_State2_WaitDirPhase1);
		states[3] = game_addr(game::MAG_090_Tonberry_Director_State3_WaitDirPhase3);
		states[4] = game_addr(game::MAG_090_Tonberry_Director_State4_ArmLoadScriptStep);
		states[5] = game_addr(game::MAG_090_Tonberry_Director_State5_WaitLoadScriptIdle);
		states[6] = game_addr(game::MAG_090_Tonberry_Director_State6_WaitDirGate5);
		states[7] = game_addr(game::MAG_090_Tonberry_Director_State7_WaitAtCue7);
		states[8] = game_addr(game::MAG_090_Tonberry_Director_State8_CountdownSetEndFlag);
		states[9] = game_addr(game::MAG_090_Tonberry_Director_State9_ReleaseVoiceAtCue9);
		states[10] = game_addr(game::MAG_090_Tonberry_Director_State10_Finish);
		states[11] = game_addr(game::MAG_090_Tonberry_Director_State11_Empty);
		t_762D10(node);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return t1_task_end(node, status);
	}

	uint32_t __cdecl t_762D10(uint32_t a1)
	{
		uint32_t d = t1_data();
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			t_763D60(a1);
			d = t1_data();
		}
		const uint16_t nxt = U16(d, 0x44);
		if (U16(d, 0x42) != nxt)
		{
			U16(d, 0x42) = nxt;
			a_73A6D0();
		}
		return 0;
	}

	uint32_t __cdecl t_762D60(uint32_t a1)
	{
		const uint32_t slot = x::BdSound_ClaimVoiceSlot(game_addr(game::unk_16D7CD0), 1, 0x80);
		const uint32_t d = t1_data();
		U16(d, 0x52) = (uint16_t)slot;
		x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_090_TaskQueue), game_addr(game::MAG_090_CameraScriptTask), 0x7C, a1);
		t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl t_762E60(uint32_t a1)
	{
		a_73AB00();
		t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl t_762E90(uint32_t a1)
	{
		if (a_73AAE0(2) != 0)
		{
			a_73AB20(game_addr(game::unk_154E9E8), t1_data(), 0);
			t1_next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl t_763690(uint32_t a1)
	{
		if (a_73AE10() == 1)
		{
			a_73B3B0(game_addr(game::unk_154E9D8));
			U16(a1, 0x74) = 0x63;
			t1_next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl t_763790(uint32_t a1)
	{
		a_73B4B0();
		U16(a1, 0x74) = (uint16_t)(U16(a1, 0x74) - 1);
		if (S16(a1, 0x74) <= 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl t_763970(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(3) != 0)
		{
			x::BdPlaySE(game_addr(game::unk_1547170), 0, 0x80);
			a_73AB20(game_addr(game::unk_154EAC0), t1_data(), 0);
			t1_next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl t_7639E0(uint32_t a1)
	{
		a_73AE10();
		if (a_73B640(4) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl t_763A30(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(6) != 0)
		{
			const uint32_t d = t1_data();
			if (S16(d, 0x4E) < 0x1000)
				a_73AB20(game_addr(game::unk_154EB50), d + 8, 0);
			else
				a_73AB20(game_addr(game::unk_154EB98), d + 8, 0);
			t1_next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl t_763AA0(uint32_t a1)
	{
		a_73AE10();
		if (a_73B640(7) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl t_763AC0(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(7) != 0)
		{
			const uint32_t d = t1_data();
			if (S16(d, 0x4E) < 0x1000)
				a_73AB20(game_addr(game::unk_154EBE0), d + 8, 0);
			else
				a_73AB20(game_addr(game::unk_154EC28), d + 8, 0);
			t1_next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl t_763B10(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(9) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl t_763B90(uint32_t a1)
	{
		if (a_73B7E0(1) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl t_763BD0(uint32_t a1)
	{
		if (a_73B7E0(3) != 0)
			t1_next_state(a1);
		return 0;
	}

	uint32_t __cdecl t_763C90(uint32_t a1)
	{
		if (a_73B7E0(7) != 0)
		{
			U16(a1, 0x44) = 0x2C;
			t1_next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl t_763CC0(uint32_t a1)
	{
		U16(a1, 0x44) = (uint16_t)(U16(a1, 0x44) - 1);
		if (S16(a1, 0x44) <= 0)
		{
			U16(t1_data(), 0x48) = 1;
			t1_next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl t_763CF0(uint32_t a1)
	{
		if (a_73B640(9) != 0)
		{
			x::sub_4A2940((uint32_t)(int32_t)S16(t1_data(), 0x52));
			t1_next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl t_763D60(uint32_t a1)
	{
		const uint32_t k = (uint32_t)((int32_t)S16(t1_data(), 0x40) - 1);
		switch (k)
		{
		case 0:
			t_763E60(a1, game_addr(game::Tonberry_ParticleTask), game_addr(game::unk_16D7DEC), 0x48C, 0, 0);
			return x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_090_TaskQueue), game_addr(game::MAG_090_EntityFadeInTask), 0x7C, a1);
		case 1:
		{
			const uint32_t creature = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_090_CreatureQueue), game_addr(game::GF_090Tonberry_CreatureActorTask), 0x148, a1);
			x::Effect_BindModelContainerSetAnim(creature, game_addr(game::MAG090_Tonberry_ModelContainer), 3);
			return x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_090_TaskQueue), game_addr(game::MAG_090_RingSpawnerTask), 0x7C, a1);
		}
		case 5:
			return x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_090_TaskQueue), game_addr(game::MAG_090_PrimModelRiseTask), 0x7C, a1);
		case 6:
			return x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_090_TaskQueue), game_addr(game::Tonberry_PrimModelActorTask), 0x7C, a1);
		case 7:
			return x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_090_TaskQueue), game_addr(game::MAG_090_EntityFadeOutTask), 0x7C, a1);
		default:
			return k;
		}
	}

	uint32_t __cdecl t_763E60(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_090_PrimModelQueue), a2, 0x520, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		U32(t, 0x78) = (uint32_t)(int32_t)(int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl t_763EB0(uint32_t a1)
	{
		uint32_t states[5];
		const uint32_t d = t1_data();
		states[0] = game_addr(game::MAG_090_Tonberry_Particle_State0_InitFromFrame6);
		states[1] = game_addr(game::MAG_090_Tonberry_Particle_State0_WaitDirGate2);
		states[2] = game_addr(game::MAG_090_Tonberry_Particle_State1_PlayAtCue2);
		const uint16_t ticks = U16(d, 0x46);
		states[3] = game_addr(game::MAG_090_Tonberry_Particle_State2_WaitCue3OrEnd);
		states[4] = game_addr(game::MAG_090_Tonberry_Particle_State3_Empty);
		if (ticks == 0x1E)
			MEM<uint32_t>(game_addr(game::MAG_090_ModelDepthOffsetTable)) = game_addr(game::unk_1547468);
		else if (ticks == 0x5A)
			MEM<uint32_t>(game_addr(game::MAG_090_ModelDepthOffsetTable)) = game_addr(game::unk_154747C);
		else if (ticks == 0x64)
			MEM<uint32_t>(game_addr(game::MAG_090_ModelDepthOffsetTable)) = game_addr(game::unk_1547490);
		else if (ticks == 0x69)
			MEM<uint32_t>(game_addr(game::MAG_090_ModelDepthOffsetTable)) = game_addr(game::unk_15474A4);
		else if (ticks == 0x71)
			MEM<uint32_t>(game_addr(game::MAG_090_ModelDepthOffsetTable)) = game_addr(game::unk_15474B8);
		const uint32_t node = a1;
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return t1_task_end(node, status);
	}

	uint32_t __cdecl t_763F80(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 6)
			return 0;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		alignas(4) uint16_t off[4] = {};
		const uint32_t d = t1_data();
		const uint32_t xy = U32(d, 0);
		const uint32_t zp = U32(d, 4);
		U32(node, 0x1C) = xy;
		const uint32_t yaw = (xy & 0xFFFF0000u) | U16(node, 0x62);
		U32(node, 0x20) = zp;
		U16(node, 0x1E) = 0;
		t_764020(yaw, 0x180, P(off));
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + off[1]);
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + off[0]);
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + off[2]);
		MEM<uint32_t>(game_addr(game::MAG_090_ModelDepthOffsetTable)) = game_addr(game::unk_1547454);
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		t_764070(node);
		t1_next_state(node);
		return 0;
	}

	uint32_t __cdecl t_764020(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		alignas(4) uint8_t loc[0x28] = {};
		const uint32_t L = P(loc);
		const uint32_t M = L + 8;
		U16(L, 0) = 0;
		U16(L, 2) = 0;
		U16(L, 4) = (uint16_t)a2;
		x::MAG_022_sub_8DD770(M);
		x::MAG_022_sub_8DD8A0(M, (uint32_t)(int32_t)(int16_t)a1);
		return x::matrixMultiplyVector(M, L, a3);
	}

	uint32_t __cdecl t_764070(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t data = U32(node, 0x74);
		const int32_t v = S32(data, 0x24);
		const uint32_t sec = data + (uint32_t)shl32(v / 4, 2);
		alignas(4) int16_t box[6];
		box[2] = 0x7FFF;
		box[1] = 0x7FFF;
		box[0] = 0x7FFF;
		box[5] = (int16_t)0x8001;
		box[4] = (int16_t)0x8001;
		box[3] = (int16_t)0x8001;
		xm::sub_8DE160(sec, P(box));
		return xm::sub_8DE1F0(node + 0x1C, P(box));
	}

	uint32_t __cdecl t_764100(uint32_t a1)
	{
		if (a_73B7E0(2) != 0)
		{
			a_73C280(a1);
			t1_next_state(a1);
		}
		return 0;
	}

	uint32_t __cdecl t_764260(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		alignas(4) uint8_t loc[0x48] = {};
		const uint32_t L = P(loc);
		const uint32_t V = L + 0x10;
		const uint32_t S = L + 0x18;
		const uint32_t M = L + 0x28;
		const uint32_t rec = a2;
		if (((uint32_t)(int32_t)S16(rec, 0x1C) | U32(rec, 0x18)) == 0)
			return 0;
		if (S16(rec, 0x24) >= 0x1000 && U32(rec, 0x20) == 0)
			return 0;

		const uint32_t hdr = x::Field_Alloc(0x68);
		const int32_t model = S16(rec, 2);
		const uint32_t data = U32(a1, 0);
		const uint32_t blk = a3;
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

		x::MAG_017_sub_701310(rec + 0x10, M);
		const uint16_t vx = U16(rec, 8);
		const uint16_t vy = U16(rec, 0xA);
		const uint16_t vz = U16(rec, 0xC);
		U16(V, 0) = vx;
		U16(V, 2) = vy;
		U16(V, 4) = vz;
		int32_t t0, t1, t2;
		if ((U8(rec, 5) & 2) != 0)
		{
			t0 = (int16_t)vx;
			t1 = (int16_t)vy;
			t2 = (int16_t)vz;
		}
		else
		{
			x::GTE_SetRotMatrix(blk);
			x::GTE_LoadV0(V);
			x::GTE_MVMVA_RotV0();
			x::GTE_ReadMAC123(M + 0x14);
			x::GTE_MatrixMultiply(blk, M);
			t2 = S32(M, 0x1C);
			t1 = S32(M, 0x18);
			t0 = S32(M, 0x14);
		}
		S32(M, 0x14) = add32(t0, S32(blk, 0x14));
		S32(M, 0x18) = add32(t1, S32(blk, 0x18));
		const uint32_t sxy = U32(rec, 0x18);
		S32(M, 0x1C) = add32(t2, S32(blk, 0x1C));
		if (sxy != 0x10001000 || U16(rec, 0x1C) != 0x1000)
		{
			S32(S, 0) = S16(rec, 0x18);
			S32(S, 4) = S16(rec, 0x1A);
			S32(S, 8) = S16(rec, 0x1C);
			x::scale3DMatrix(M, S);
		}
		x::GTE_SetRotMatrix_W(M);
		x::GTE_SetTransVector_W(M);
		const uint32_t flags = U32(rec, 4);
		U32(hdr, 0x14) = (flags & 0x4000) != 0 ? 0x2000u : 0x2030u;
		if ((flags & 0x2000) != 0)
			U32(hdr, 0x14) |= 0xC;
		const int32_t alpha = S16(rec, 0x24);
		U32(hdr, 0xC) = (uint32_t)alpha;
		if (alpha != 0)
		{
			U32(hdr, 0x14) |= 0xC0;
			U32(hdr, 8) = U32(rec, 0x20);
		}
		U32(hdr, 0x10) = (uint32_t)(int32_t)S16(blk, 0x58);
		uint32_t cursor, ot;
		if (U16(blk, 0x56) == 0)
		{
			const int32_t mi = S16(rec, 2);
			const uint32_t tab = MEM<uint32_t>(game_addr(game::MAG_090_ModelDepthOffsetTable));
			U16(hdr, 0x18) = 0;
			U16(hdr, 0x1A) = 0;
			U16(hdr, 0x1E) = 0;
			const int32_t dz = S16(tab, mi * 2);
			cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
			U16(hdr, 0x22) = 0x100;
			U16(hdr, 0x20) = 0x100;
			U16(hdr, 0x2A) = 0x100;
			U16(hdr, 0x28) = 0x100;
			ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
			U32(hdr, 0x10) = (uint32_t)dz;
			U16(hdr, 0x1C) = 0;
			U16(hdr, 0x26) = 0;
			U16(hdr, 0x24) = 0;
		}
		else
		{
			cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
			ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
			U16(hdr, 0x18) = 0;
			U16(hdr, 0x1A) = 0;
			U16(hdr, 0x1E) = 0;
			U16(hdr, 0x1C) = 0;
			U16(hdr, 0x22) = 0x100;
			U16(hdr, 0x20) = 0x100;
			U16(hdr, 0x26) = 0;
			U16(hdr, 0x24) = 0;
			U16(hdr, 0x2A) = 0x100;
			U16(hdr, 0x28) = 0x100;
		}
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = t_764540(hdr, ot, 2, cursor);
		x::Field_Free(0x68);
		return 0;
	}

	uint32_t __cdecl t_764540(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t hdr = a1;
		if ((U32(hdr, 0x14) & 0x2000) == 0)
			U32(hdr, 4) = U32(hdr, 0) + 8;
		{
			const uint32_t model = U32(hdr, 0);
			const uint32_t b = U8(hdr, 0xA);
			U32(hdr, 0x2C) = U32(model, 0) + model;
			const uint32_t g = U8(hdr, 9);
			const uint32_t r = U8(hdr, 8);
			x::someCameraWork_45DD60(r, g, b);
		}
		uint32_t cursor = a4;
		typedef uint32_t (__cdecl *Draw)(uint32_t, uint32_t, uint32_t, uint32_t);
		static const Draw lists[8] = { t_764670, t_764890, t_764B20, t_765010, a_73D770, a_73D9C0, t_765B30, t_766090 };
		for (int i = 0; i < 8; i++)
		{
			const uint32_t list = U32(hdr, 0x2C);
			if (U32(list, 0) != 0)
				cursor = lists[i](hdr, a2, a3, cursor);
			else
				U32(hdr, 0x2C) = list + 4;
		}
		return cursor;
	}

	uint32_t __cdecl t_764670(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t cursor = a4;
		const uint32_t list = U32(ctx, 0x2C);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		U32(ctx, 0x2C) = rec;
		const uint32_t vbase = U32(ctx, 4);
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return a4;
		}
		uint32_t pkt = cursor + 0xC;
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			const uint32_t flags = U32(ctx, 0x14);
			uint32_t code = U32(rec, 0);
			U32(cursor, 0) = 0x4000000;
			U32(pkt, -8) = code;
			if (flags & 1)
			{
				code |= 0x2000000;
				U32(pkt, -8) = code;
			}
			if (flags & 4)
				U32(pkt, -8) &= 0xFDFFFFFF;
			x::GTE_ReadFLAG(ctx + 0x3C);
			if (!(U32(ctx, 0x3C) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) >= 0 || (U8(ctx, 0x14) & 0x10))
				{
					x::GTE_ReadSXY012_Split(pkt - 4, pkt, pkt + 4);
					x::GTE_AVSZ3();
					if (t1_out_x(S16(pkt, -4))) clip = 1;
					if (t1_out_x(S16(pkt, 0))) clip |= 2;
					if (t1_out_x(S16(pkt, 4))) clip |= 4;
					if (t1_out_y(S16(pkt, -2))) clip |= 0x10;
					if (t1_out_y(S16(pkt, 2))) clip |= 0x20;
					if (t1_out_y(S16(pkt, 6))) clip |= 0x40;
					if ((uint8_t)(clip & 7) != 7 && (uint8_t)(clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x38);
						if (U8(ctx, 0x14) & 0x40)
						{
							x::set_unk_1CA8A28(pkt - 8);
							x::set_dword_1CA8A30(U32(ctx, 0x0C));
							x::sub_45F270();
							x::set_param_with_dword_1CA8A68(pkt - 8);
						}
						int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
						S32(ctx, 0x38) = z;
						if (z < 0)
							S32(ctx, 0x38) = 0;
						z = S32(ctx, 0x38) >> (a3 & 31);
						x::SSIGPU_InsertPrimAutoDepth(a2 + (uint32_t)z * 4, cursor);
						cursor += 0x14;
						pkt += 0x14;
					}
				}
			}
			rec += 0xC;
		} while (--count);
		U32(ctx, 0x2C) = rec;
		return cursor;
	}

	namespace
	{
		uint32_t t2_twin(uint32_t ctx, int32_t b)
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

		inline bool t2_outx(uint32_t p) { const int16_t v = S16(p, 0); return v < 0 || v > 0xA00; }
		inline bool t2_outy(uint32_t p) { const int16_t v = S16(p, 0); return v < 0 || v > 0x6C0; }

		inline void t2_depth_cue(uint32_t ctx, uint32_t pCode)
		{
			x::set_unk_1CA8A28(pCode);
			x::set_dword_1CA8A30(U32(ctx, 0xC));
			x::sub_45F270();
			x::set_param_with_dword_1CA8A68(pCode);
		}

		inline uint32_t t2_ot(uint32_t ctx, uint32_t a2, uint32_t a3)
		{
			const int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
			S32(ctx, 0x38) = z;
			if (z < 0)
				S32(ctx, 0x38) = 0;
			return a2 + (uint32_t)(S32(ctx, 0x38) >> (a3 & 31)) * 4u;
		}
	}

	uint32_t __cdecl t_764890(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		uint32_t rec = U32(ctx, 0x2C);
		int32_t count = S32(rec, 0);
		rec += 4;
		U32(ctx, 0x2C) = rec;
		const uint32_t vbase = U32(ctx, 4);
		if (count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return pkt;
		}
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4u, vbase + (uint32_t)U16(rec, 6) * 4u,
				vbase + (uint32_t)U16(rec, 8) * 4u);
			x::GTE_RTPT();
			{
				const uint32_t flags = U32(ctx, 0x14);
				const uint32_t code = U32(rec, 0);
				U32(pkt, 0) = 0x5000000;
				U32(pkt, 4) = code;
				if (flags & 1)
					U32(pkt, 4) = code | 0x2000000;
				if (flags & 4)
					U32(pkt, 4) = U32(pkt, 4) & 0xFDFFFFFFu;
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
				x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0xC, pkt + 0x10);
				x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0xA) * 4u);
				x::GTE_RTPS();
				if (t2_outx(pkt + 8))
					clip = 1;
				if (t2_outx(pkt + 0xC))
					clip |= 2;
				if (t2_outx(pkt + 0x10))
					clip |= 4;
				if (t2_outy(pkt + 0xA))
					clip |= 0x10;
				if (t2_outy(pkt + 0xE))
					clip |= 0x20;
				if (t2_outy(pkt + 0x12))
					clip |= 0x40;
				x::GTE_ReadSXY2(pkt + 0x14);
				x::GTE_AVSZ4();
				if (t2_outx(pkt + 0x14))
					clip |= 8;
				if (t2_outy(pkt + 0x16))
					clip |= 0x80;
				if ((clip & 0xF) == 0xF)
					goto next;
				if ((clip & 0xF0) == 0xF0)
					goto next;
			}
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x40)
				t2_depth_cue(ctx, pkt + 4);
			{
				const uint32_t ot = t2_ot(ctx, a2, a3);
				x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
				pkt += 0x18;
			}
		next:
			rec += 0xC;
			--count;
		} while (count != 0);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl t_764B20(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4u, vbase + (uint32_t)U16(rec, 6) * 4u,
				vbase + (uint32_t)U16(rec, 8) * 4u);
			x::GTE_RTPT();
			{
				const uint32_t flags = U32(ctx, 0x14);
				const uint32_t code = U32(rec, 0);
				U32(pkt, 0) = 0x7000000;
				U32(pkt, 4) = code;
				if (flags & 1)
					U32(pkt, 4) = code | 0x2000000;
				if (flags & 4)
					U32(pkt, 4) = U32(pkt, 4) & 0xFDFFFFFFu;
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
				x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
				x::GTE_AVSZ3();
				if (t2_outx(pkt + 8))
					clip = 1;
				if (t2_outx(pkt + 0x10))
					clip |= 2;
				if (t2_outx(pkt + 0x18))
					clip |= 4;
				if (t2_outy(pkt + 0xA))
					clip |= 0x10;
				if (t2_outy(pkt + 0x12))
					clip |= 0x20;
				if (t2_outy(pkt + 0x1A))
					clip |= 0x40;
				if ((clip & 7) == 7)
					goto next;
				if ((clip & 0x70) == 0x70)
					goto next;
			}
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x40)
				t2_depth_cue(ctx, pkt + 4);
			{
				const uint32_t ot = t2_ot(ctx, a2, a3);
				const uint16_t add0 = U16(ctx, 0x18);
				const uint16_t add1 = U16(ctx, 0x1A);
				if ((uint16_t)(add0 | add1) != 0)
				{
					if (add0 != 0)
					{
						const uint32_t a = add0;
						const uint32_t c2 = U8(pkt, 0x1C) + a;
						const uint32_t c0 = U8(pkt, 0xC) + a;
						const uint32_t c1 = U8(pkt, 0x14) + a;
						uint8_t s = 0;
						if ((int32_t)(c2 | c1 | c0) > 0xFF)
							s = U8(ctx, 0x28);
						U8(pkt, 0xC) = (uint8_t)(c0 - s);
						U8(pkt, 0x14) = (uint8_t)(c1 - s);
						U8(pkt, 0x1C) = (uint8_t)(c2 - s);
					}
					const uint16_t add1b = U16(ctx, 0x1A);
					if (add1b != 0)
					{
						const uint32_t a = add1b;
						const uint32_t c2 = U8(pkt, 0x1D) + a;
						const uint32_t c0 = U8(pkt, 0xD) + a;
						const uint32_t c1 = U8(pkt, 0x15) + a;
						uint8_t s = 0;
						if ((int32_t)(c2 | c1 | c0) > 0xFF)
							s = U8(ctx, 0x2A);
						U8(pkt, 0xD) = (uint8_t)(c0 - s);
						U8(pkt, 0x15) = (uint8_t)(c1 - s);
						U8(pkt, 0x1D) = (uint8_t)(c2 - s);
					}
					const uint32_t poly = pkt;
					uint32_t prim = pkt + 0x20;
					pkt += 0x2C;
					U32(prim, 0) = 0x2000000;
					uint32_t tw = 0;
					if (ctx + 0x1C != 0)
						tw = t2_twin(ctx, 0x1C);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
					prim = pkt;
					pkt += 0xC;
					U32(prim, 0) = 0x2000000;
					tw = 0;
					if (ctx + 0x24 != 0)
						tw = t2_twin(ctx, 0x24);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
				}
				else
				{
					const uint32_t poly = pkt;
					const uint32_t mode = pkt + 0x20;
					pkt += 0x2C;
					const uint32_t tpage = x::sub_45C690(0, 1, 0, 0);
					x::sub_45BFC0(mode, 0, 0, tpage & 0xFFFF, 0);
					x::SSIGPU_InsertPrimAutoDepth(ot, mode);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
				}
			}
		next:
			rec += 0x14;
			--count;
		} while (count != 0);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl t_765010(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
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
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4u, vbase + (uint32_t)U16(rec, 6) * 4u,
				vbase + (uint32_t)U16(rec, 8) * 4u);
			x::GTE_RTPT();
			{
				const uint32_t flags = U32(ctx, 0x14);
				const uint32_t code = U32(rec, 0);
				U32(pkt, 0) = 0x9000000;
				U32(pkt, 4) = code;
				if (flags & 1)
					U32(pkt, 4) = code | 0x2000000;
				if (flags & 4)
					U32(pkt, 4) = U32(pkt, 4) & 0xFDFFFFFFu;
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
				x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x10, pkt + 0x18);
				x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0xA) * 4u);
				x::GTE_RTPS();
				if (t2_outx(pkt + 8))
					clip = 1;
				if (t2_outx(pkt + 0x10))
					clip |= 2;
				if (t2_outx(pkt + 0x18))
					clip |= 4;
				if (t2_outy(pkt + 0xA))
					clip |= 0x10;
				if (t2_outy(pkt + 0x12))
					clip |= 0x20;
				if (t2_outy(pkt + 0x1A))
					clip |= 0x40;
				x::GTE_ReadSXY2(pkt + 0x20);
				x::GTE_AVSZ4();
				if (t2_outx(pkt + 0x20))
					clip |= 8;
				if (t2_outy(pkt + 0x22))
					clip |= 0x80;
				if ((clip & 0xF) == 0xF)
					goto next;
				if ((clip & 0xF0) == 0xF0)
					goto next;
			}
			x::GTE_ReadOTZ(ctx + 0x38);
			if (U8(ctx, 0x14) & 0x40)
				t2_depth_cue(ctx, pkt + 4);
			{
				const uint32_t ot = t2_ot(ctx, a2, a3);
				const uint16_t add0 = U16(ctx, 0x18);
				const uint16_t add1 = U16(ctx, 0x1A);
				if ((uint16_t)(add0 | add1) != 0)
				{
					if (add0 != 0)
					{
						const uint32_t a = add0;
						const uint32_t c0 = U8(pkt, 0xC) + a;
						const uint32_t c1 = U8(pkt, 0x14) + a;
						const uint32_t c2 = U8(pkt, 0x1C) + a;
						const uint32_t c3 = U8(pkt, 0x24) + a;
						uint8_t s = 0;
						if ((int32_t)(c3 | c2 | c1 | c0) > 0xFF)
							s = U8(ctx, 0x28);
						U8(pkt, 0xC) = (uint8_t)(c0 - s);
						U8(pkt, 0x14) = (uint8_t)(c1 - s);
						U8(pkt, 0x1C) = (uint8_t)(c2 - s);
						U8(pkt, 0x24) = (uint8_t)(c3 - s);
					}
					const uint16_t add1b = U16(ctx, 0x1A);
					if (add1b != 0)
					{
						const uint32_t a = add1b;
						const uint32_t c0 = U8(pkt, 0xD) + a;
						const uint32_t c1 = U8(pkt, 0x15) + a;
						const uint32_t c2 = U8(pkt, 0x1D) + a;
						const uint32_t c3 = U8(pkt, 0x25) + a;
						uint8_t s = 0;
						if ((int32_t)(c3 | c2 | c1 | c0) > 0xFF)
							s = U8(ctx, 0x2A);
						U8(pkt, 0xD) = (uint8_t)(c0 - s);
						U8(pkt, 0x15) = (uint8_t)(c1 - s);
						U8(pkt, 0x1D) = (uint8_t)(c2 - s);
						U8(pkt, 0x25) = (uint8_t)(c3 - s);
					}
					const uint32_t poly = pkt;
					uint32_t prim = pkt + 0x28;
					pkt += 0x34;
					U32(prim, 0) = 0x2000000;
					uint32_t tw = 0;
					if (ctx + 0x1C != 0)
						tw = t2_twin(ctx, 0x1C);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
					prim = pkt;
					pkt += 0xC;
					U32(prim, 0) = 0x2000000;
					tw = 0;
					if (ctx + 0x24 != 0)
						tw = t2_twin(ctx, 0x24);
					U32(prim, 4) = tw;
					U32(prim, 8) = 0;
					x::SSIGPU_InsertPrimAutoDepth(ot, prim);
				}
				else
				{
					const uint32_t poly = pkt;
					const uint32_t mode = pkt + 0x28;
					pkt += 0x34;
					const uint32_t tpage = x::sub_45C690(0, 1, 0, 0);
					x::sub_45BFC0(mode, 0, 0, tpage & 0xFFFF, 0);
					x::SSIGPU_InsertPrimAutoDepth(ot, mode);
					x::SSIGPU_InsertPrimAutoDepth(ot, poly);
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
		inline bool t3_out_x(int16_t v) { return v < 0 || v > 0xA00; }
		inline bool t3_out_y(int16_t v) { return v < 0 || v > 0x6C0; }

		inline uint32_t t3_texwin(uint32_t p)
		{
			uint32_t t = ((uint32_t)U8(p, 2) & 0xF8) | 0xFFFE2000u;
			t = (t << 5) | ((uint32_t)U8(p, 0) & 0xF8);
			t = (t << 5) | (~((uint32_t)U16(p, 6) - 1) & 0xF8);
			t = (t << 2) | ((~((uint32_t)U16(p, 4) - 1) >> 3) & 0x1F);
			return t;
		}

		inline void t3_scroll(uint32_t pkt, const int32_t *off, int n, uint16_t d, uint8_t wrap)
		{
			uint32_t s[4];
			uint32_t any = 0;
			for (int i = 0; i < n; i++)
			{
				s[i] = (uint32_t)U8(pkt, off[i]) + (uint32_t)d;
				any |= s[i];
			}
			const bool over = (int32_t)any > 0xFF;
			for (int i = 0; i < n; i++)
				U8(pkt, off[i]) = over ? (uint8_t)(s[i] - wrap) : (uint8_t)s[i];
		}
	}

	uint32_t __cdecl t_765B30(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		static const int32_t U_OFF[3] = { 0x0C, 0x18, 0x24 };
		static const int32_t V_OFF[3] = { 0x0D, 0x19, 0x25 };
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		const uint32_t list = U32(ctx, 0x2C);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = rec;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return pkt;
		}
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			const uint32_t flags = U32(ctx, 0x14);
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
			const uint32_t uv0 = U32(rec, 0x0C);
			const uint32_t w8 = U32(rec, 8);
			const uint32_t uv1 = U32(rec, 0x10);
			U32(pkt, 0x0C) = uv0;
			U32(pkt, 0x18) = uv1;
			U32(pkt, 0x24) = w8 >> 16;
			x::GTE_ReadFLAG(ctx + 0x3C);
			if (!(U32(ctx, 0x3C) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) >= 0 || (U8(ctx, 0x14) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x14, pkt + 0x20);
					x::GTE_AVSZ3();
					if (t3_out_x(S16(pkt, 8))) clip = 1;
					if (t3_out_x(S16(pkt, 0x14))) clip |= 2;
					if (t3_out_x(S16(pkt, 0x20))) clip |= 4;
					if (t3_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (t3_out_y(S16(pkt, 0x16))) clip |= 0x20;
					if (t3_out_y(S16(pkt, 0x22))) clip |= 0x40;
					if ((uint8_t)(clip & 7) != 7 && (uint8_t)(clip & 0x70) != 0x70)
					{
						x::GTE_ReadOTZ(ctx + 0x38);
						if (U8(ctx, 0x14) & 0x80)
						{
							x::sub_45E120(rec + 0x14, rec + 0x18, pkt + 4);
							x::set_dword_1CA8A30(U32(ctx, 0x0C));
							x::sub_45F4C0();
							x::sub_45E370(pkt + 0x10, pkt + 0x1C, pkt + 4);
						}
						else
						{
							const uint32_t c1 = U32(rec, 0x14), c2 = U32(rec, 0x18);
							U32(pkt, 0x10) = c1;
							U32(pkt, 0x1C) = c2;
						}
						int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
						S32(ctx, 0x38) = z;
						if (z < 0)
							S32(ctx, 0x38) = 0;
						z = S32(ctx, 0x38) >> (a3 & 31);
						const uint32_t ot = a2 + (uint32_t)z * 4;
						const uint16_t du = U16(ctx, 0x18);
						const uint16_t dv = U16(ctx, 0x1A);
						if ((uint16_t)(du | dv) != 0)
						{
							if (du != 0)
								t3_scroll(pkt, U_OFF, 3, du, U8(ctx, 0x28));
							const uint16_t dv2 = U16(ctx, 0x1A);
							if (dv2 != 0)
								t3_scroll(pkt, V_OFF, 3, dv2, U8(ctx, 0x2A));
							const uint32_t w1 = pkt + 0x28;
							U32(w1, 0) = 0x2000000;
							const uint32_t t1 = (ctx + 0x1C) != 0 ? t3_texwin(ctx + 0x1C) : 0;
							U32(w1, 4) = t1;
							U32(w1, 8) = 0;
							x::SSIGPU_InsertPrimAutoDepth(ot, w1);
							x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
							const uint32_t w2 = pkt + 0x34;
							U32(w2, 0) = 0x2000000;
							const uint32_t t2 = (ctx + 0x24) != 0 ? t3_texwin(ctx + 0x24) : 0;
							U32(w2, 4) = t2;
							U32(w2, 8) = 0;
							x::SSIGPU_InsertPrimAutoDepth(ot, w2);
							pkt += 0x40;
						}
						else
						{
							const uint32_t dm = pkt + 0x28;
							const uint32_t tp = x::sub_45C690(0, 1, 0, 0);
							x::sub_45BFC0(dm, 0, 0, tp & 0xFFFF, 0);
							x::SSIGPU_InsertPrimAutoDepth(ot, dm);
							x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
							pkt += 0x34;
						}
					}
				}
			}
			rec += 0x1C;
		} while (--count);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl t_766090(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		static const int32_t U_OFF[4] = { 0x0C, 0x18, 0x24, 0x30 };
		static const int32_t V_OFF[4] = { 0x0D, 0x19, 0x25, 0x31 };
		const uint32_t ctx = a1;
		uint32_t pkt = a4;
		const uint32_t list = U32(ctx, 0x2C);
		uint32_t count = U32(list, 0);
		uint32_t rec = list + 4;
		const uint32_t vbase = U32(ctx, 4);
		U32(ctx, 0x2C) = rec;
		if ((int32_t)count <= 0)
		{
			U32(ctx, 0x2C) = rec;
			return pkt;
		}
		do
		{
			x::GTE_LoadV012(vbase + (uint32_t)U16(rec, 4) * 4, vbase + (uint32_t)U16(rec, 6) * 4, vbase + (uint32_t)U16(rec, 8) * 4);
			x::GTE_RTPT();
			const uint32_t flags = U32(ctx, 0x14);
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
			const uint32_t uv0 = U32(rec, 0x0C);
			const uint32_t uv1 = U32(rec, 0x10);
			const uint32_t uv23 = U32(rec, 0x14);
			U32(pkt, 0x0C) = uv0;
			U32(pkt, 0x24) = uv23;
			U32(pkt, 0x18) = uv1;
			U32(pkt, 0x30) = uv23 >> 16;
			x::GTE_ReadFLAG(ctx + 0x3C);
			if (!(U32(ctx, 0x3C) & 0x60000))
			{
				x::GTE_NCLIP();
				uint32_t clip = 0;
				x::GTE_ReadMAC0(ctx + 0x30);
				if (S32(ctx, 0x30) >= 0 || (U8(ctx, 0x14) & 0x20))
				{
					x::GTE_ReadSXY012_Split(pkt + 8, pkt + 0x14, pkt + 0x20);
					x::GTE_LoadV0(vbase + (uint32_t)U16(rec, 0x0A) * 4);
					x::GTE_RTPS();
					if (t3_out_x(S16(pkt, 8))) clip = 1;
					if (t3_out_x(S16(pkt, 0x14))) clip |= 2;
					if (t3_out_x(S16(pkt, 0x20))) clip |= 4;
					if (t3_out_y(S16(pkt, 0x0A))) clip |= 0x10;
					if (t3_out_y(S16(pkt, 0x16))) clip |= 0x20;
					if (t3_out_y(S16(pkt, 0x22))) clip |= 0x40;
					x::GTE_ReadSXY2(pkt + 0x2C);
					x::GTE_AVSZ4();
					if (t3_out_x(S16(pkt, 0x2C))) clip |= 8;
					if (t3_out_y(S16(pkt, 0x2E))) clip |= 0x80;
					if ((uint8_t)(clip & 0xF) != 0xF && (uint8_t)(clip & 0xF0) != 0xF0)
					{
						x::GTE_ReadOTZ(ctx + 0x38);
						if (U8(ctx, 0x14) & 0x80)
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
							const uint32_t c1 = U32(rec, 0x18), c2 = U32(rec, 0x1C), c3 = U32(rec, 0x20);
							U32(pkt, 0x10) = c1;
							U32(pkt, 0x1C) = c2;
							U32(pkt, 0x28) = c3;
						}
						int32_t z = add32(S32(ctx, 0x38), S32(ctx, 0x10));
						S32(ctx, 0x38) = z;
						if (z < 0)
							S32(ctx, 0x38) = 0;
						z = S32(ctx, 0x38) >> (a3 & 31);
						const uint32_t ot = a2 + (uint32_t)z * 4;
						const uint16_t du = U16(ctx, 0x18);
						const uint16_t dv = U16(ctx, 0x1A);
						if ((uint16_t)(du | dv) != 0)
						{
							if (du != 0)
								t3_scroll(pkt, U_OFF, 4, du, U8(ctx, 0x28));
							const uint16_t dv2 = U16(ctx, 0x1A);
							if (dv2 != 0)
								t3_scroll(pkt, V_OFF, 4, dv2, U8(ctx, 0x2A));
							const uint32_t w1 = pkt + 0x34;
							U32(w1, 0) = 0x2000000;
							const uint32_t t1 = (ctx + 0x1C) != 0 ? t3_texwin(ctx + 0x1C) : 0;
							U32(w1, 4) = t1;
							U32(w1, 8) = 0;
							x::SSIGPU_InsertPrimAutoDepth(ot, w1);
							x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
							const uint32_t w2 = pkt + 0x40;
							U32(w2, 0) = 0x2000000;
							const uint32_t t2 = (ctx + 0x24) != 0 ? t3_texwin(ctx + 0x24) : 0;
							U32(w2, 4) = t2;
							U32(w2, 8) = 0;
							x::SSIGPU_InsertPrimAutoDepth(ot, w2);
							pkt += 0x4C;
						}
						else
						{
							const uint32_t dm = pkt + 0x34;
							const uint32_t tp = x::sub_45C690(0, 1, 0, 0);
							x::sub_45BFC0(dm, 0, 0, tp & 0xFFFF, 0);
							x::SSIGPU_InsertPrimAutoDepth(ot, dm);
							x::SSIGPU_InsertPrimAutoDepth(ot, pkt);
							pkt += 0x40;
						}
					}
				}
			}
			rec += 0x24;
		} while (--count);
		U32(ctx, 0x2C) = rec;
		return pkt;
	}

	uint32_t __cdecl t_766760(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_73B7E0(3) != 0)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(st + 1);
			return node;
		}
		const uint32_t r = a_73C280(node);
		if (r == 0)
		{
			const uint8_t st = (uint8_t)(U8(node, 0x29) + 1);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = st;
			return st;
		}
		return r;
	}

	uint32_t __cdecl t_7667B0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[5];
		states[0] = game_addr(game::MAG_090_Tonberry_PrimModelActor_State0_Init);
		states[1] = game_addr(game::MAG_090_Tonberry_PrimModelActor_State1_ShowAtFrame14);
		states[2] = game_addr(game::MAG_090_Tonberry_PrimModelActor_State2_LoopFrames);
		states[3] = game_addr(game::MAG_090_Tonberry_PrimModelActor_State3_CounterFinish);
		states[4] = game_addr(game::MAG_090_Tonberry_PrimModelActor_State4_Empty);
		callp(states[S8(node, 0x29)], node);
		t_766830(node, 0x100);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		if ((status & 1) && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}

	uint32_t __cdecl t_766830(uint32_t a1, uint32_t a2)
	{
		const uint32_t node = a1;
		if (U8(node, 0x26) & 4)
			return 0;
		const uint32_t blk = x::Field_Alloc(0xE8);
		const uint32_t M = blk + 0xB8;
		x::MAG_022_sub_8DD770(M);
		const uint16_t scale = (uint16_t)a2;
		if (scale != 0x1000)
		{
			const int32_t s = (int16_t)scale;
			S32(blk, 0xE0) = s;
			S32(blk, 0xDC) = s;
			S32(blk, 0xD8) = s;
			x::scale3DMatrix(M, blk + 0xD8);
		}
		const int32_t px = S16(node, 0x1C);
		const int32_t py = S16(node, 0x1E);
		const int32_t pz = S16(node, 0x20);
		S32(blk, 0xCC) = px;
		S32(blk, 0xD0) = py;
		S32(blk, 0xD4) = pz;
		x::GTE_SetRotMatrix(M);
		x::GTE_SetTransVector(M);
		const uint32_t model = U32(node, 0x4C);
		const uint16_t w50 = U16(node, 0x50);
		const uint16_t w56 = U16(node, 0x56);
		U32(blk, 0) = model;
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::MAG_090_PacketCursor));
		U16(blk, 4) = w50;
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(blk, 0x24) = 0;
		U16(blk, 0xB4) = w56;
		MEM<uint32_t>(game_addr(game::MAG_090_PacketCursor)) = a_7435E0(blk, ot, 2, cursor);
		x::Field_Free(0xE8);
		return 0;
	}

	namespace
	{
		inline uint32_t t4_task_end(uint32_t node, uint8_t status)
		{
			if ((status & 1) != 0 && U8(node, 0x28) == 0)
			{
				x::Effect_ReleaseLinkedTask(node);
				return 2;
			}
			return 0;
		}
		inline void t4_next_state(uint32_t node) { U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1); }
		inline int16_t t4_div16(int16_t v) { return (int16_t)((int32_t)v / 16); }
		inline int16_t t4_div8(int16_t v) { return (int16_t)((int32_t)v / 8); }
	}

	uint32_t __cdecl t_766EB0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t src = MEM<uint32_t>(game_addr(game::unk_1547510));
		const uint16_t z = U16(src, 0xE0);
		const uint8_t st = U8(node, 0x29);
		U8(node, 0x26) |= 4;
		U16(node, 0x1C) = 0x40;
		U16(node, 0x1E) = 0xFFE4;
		U16(node, 0x20) = z;
		U32(node, 0x4C) = game_addr(game::unk_1547174);
		U16(node, 0x56) = 0xFF00;
		U16(node, 0x52) = 5;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl t_766F00(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) >= 0x14)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) &= 0xFB;
			U16(node, 0x74) = 4;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl t_766F20(uint32_t a1)
	{
		const uint32_t node = a1;
		a_743C20(node);
		if (S16(node, 0x50) > 4)
		{
			U16(node, 0x74) = (uint16_t)(U16(node, 0x74) - 1);
			const int16_t left = S16(node, 0x74);
			U16(node, 0x50) = 3;
			if (left <= 0)
				t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl t_766FB0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_090_PrimModelRise_State0_Init);
		states[1] = game_addr(game::MAG_090_PrimModelRise_State1_Show);
		states[2] = game_addr(game::MAG_090_PrimModelRise_State2_RiseUntilCue7);
		states[3] = game_addr(game::MAG_090_PrimModelRise_State3_Nop);
		callp(states[S8(node, 0x29)], node);
		t_766830(node, 0x100);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return t4_task_end(node, status);
	}

	uint32_t __cdecl t_767020(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t src = MEM<uint32_t>(game_addr(game::unk_1547510));
		const uint16_t z = U16(src, 0xE0);
		const uint8_t st = U8(node, 0x29);
		U8(node, 0x26) |= 4;
		U16(node, 0x1C) = 0xFFC8;
		U16(node, 0x1E) = 0xFFA8;
		U16(node, 0x20) = z;
		U32(node, 0x4C) = game_addr(game::unk_1547258);
		U16(node, 0x56) = 0xFF00;
		U16(node, 0x52) = 0;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl t_767070(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) >= 0)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) &= 0xFB;
			U16(node, 0x6E) = 0x18;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl t_767090(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + t4_div16(S16(node, 0x6E)));
		if (a_73B7E0(7) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl t_767150(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) >= 0x29)
		{
			const uint32_t d = MEM<uint32_t>(game_addr(game::unk_1547168));
			U16(node, 0x78) = 0x40;
			U16(node, 0x74) = 0;
			const uint32_t xy = U32(d, 0);
			const uint32_t zp = U32(d, 4);
			U32(node, 0x1C) = xy;
			U32(node, 0x20) = zp;
			const uint8_t st = U8(node, 0x29);
			U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + 0x120);
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl t_767190(uint32_t a1)
	{
		const uint32_t node = a1;
		if (t_7671D0(node, game_addr(game::unk_154EC70), U16(node, 0x74), 0x20) == 0)
			t4_next_state(node);
		U16(node, 0x78) = (uint16_t)(U16(node, 0x78) + 8);
		const int16_t r = S16(node, 0x78);
		U16(node, 0x74) = (uint16_t)(U16(node, 0x74) + 1);
		if (r > 0xC0)
			U16(node, 0x78) = 0xC0;
		return 0;
	}

	uint32_t __cdecl t_7671D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
	{
		const uint32_t spawner = a1;
		const uint16_t count16 = U8(a2 + (uint32_t)(int32_t)(int16_t)a3, 0);
		if (count16 == 0x7F)
			return 0;
		int32_t count = (int32_t)(int16_t)count16;
		if (count > 0)
		{
			do
			{
				const uint32_t p = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_090_TaskQueue), game_addr(game::TonRingParticle_Task), 0x7C, spawner);
				const uint32_t r1 = x::CrtRand();
				const uint32_t ang = r1 & 0xFFF;
				const uint32_t r2 = x::CrtRand();
				const int32_t rem = (int32_t)r2 % (int32_t)(int16_t)a4;
				const uint32_t xy = U32(spawner, 0x1C);
				const uint32_t zp = U32(spawner, 0x20);
				const int32_t angs = (int32_t)(int16_t)ang;
				U32(p, 0x1C) = xy;
				U32(p, 0x20) = zp;
				const int32_t radius = (int16_t)(uint16_t)((uint32_t)rem + U16(spawner, 0x78));
				const int32_t s = (int32_t)x::computeSin((uint32_t)angs);
				U16(p, 0x1C) = (uint16_t)(U16(p, 0x1C) + (int16_t)(mul32(s, radius) / 4096));
				const int32_t c = (int32_t)x::computeCosine((uint32_t)angs);
				U16(p, 0x20) = (uint16_t)(U16(p, 0x20) + (int16_t)(mul32(c, radius) / 4096));
			} while (--count != 0);
		}
		return 1;
	}

	uint32_t __cdecl t_767290(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_090_TonRingParticle_State0_Init);
		states[1] = game_addr(game::MAG_090_TonRingParticle_State1_Animate);
		states[2] = game_addr(game::MAG_090_TonRingParticle_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		t_7672F0(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return t4_task_end(node, status);
	}

	uint32_t __cdecl t_7672F0(uint32_t a1)
	{
		const uint32_t node = a1;
		if ((U8(node, 0x26) & 4) != 0)
			return 0;
		const uint32_t blk = x::Field_Alloc(0xD8);
		const uint32_t mat = blk + 0xB8;
		x::UnpackRotationMatrix(game_addr(game::unk_1D97778), mat);
		const int32_t px = S16(node, 0x1C);
		const int32_t py = S16(node, 0x1E);
		const int32_t pz = S16(node, 0x20);
		const uint32_t tr = blk + 0xCC;
		S32(blk, 0xD0) = py;
		S32(blk, 0xD4) = pz;
		S32(tr, 0) = px;
		x::GTE_SetRotMatrix(game_addr(game::unk_1D97778));
		x::GTE_LoadIRFromMatrixColumn(mat);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(mat);
		x::GTE_LoadIRFromMatrixColumn(blk + 0xBA);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(blk + 0xBA);
		x::GTE_LoadIRFromMatrixColumn(blk + 0xBC);
		x::GTE_MVMVA_RotIR();
		x::GTE_StoreIRToMatrixColumn(blk + 0xBC);
		x::GTE_SetTransVector(game_addr(game::unk_1D97778));
		x::GTE_LoadV0FromDwords(tr);
		x::GTE_MVMVA_RotV0_Tr();
		x::GTE_ReadMAC123(tr);
		x::GTE_SetRotMatrix(mat);
		x::GTE_SetTransVector(mat);
		U32(blk, 0) = U32(node, 0x4C);
		const uint16_t frame = U16(node, 0x50);
		const uint16_t colour = U16(node, 0x56);
		U16(blk, 4) = frame;
		const uint32_t cursor = MEM<uint32_t>(game_addr(game::MAG_090_PacketCursor));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(blk, 0x24) = 0;
		U16(blk, 0xB4) = colour;
		MEM<uint32_t>(game_addr(game::MAG_090_PacketCursor)) = a_7435E0(blk, ot, 2, cursor);
		x::Field_Free(0xD8);
		return 0;
	}

	uint32_t __cdecl t_767410(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint8_t st = U8(node, 0x29);
		U32(node, 0x4C) = game_addr(game::unk_1547280);
		U16(node, 0x56) = 0;
		U16(node, 0x52) = 6;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl t_767430(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl t_767460(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) >= 0x52)
		{
			const uint8_t st = U8(node, 0x29);
			U16(node, 0x78) = 0x80;
			U16(node, 0x74) = 0;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl t_767480(uint32_t a1)
	{
		const uint32_t node = a1;
		const int16_t f = S16(node, 0x24);
		if (f >= 0x5C && f <= 0x64)
			U16(node, 0x20) = (uint16_t)(U16(node, 0x20) - 0x10);
		if (t_7671D0(node, game_addr(game::unk_154EC90), U16(node, 0x74), 0x80) == 0)
		{
			U8(node, 0x26) |= 1;
			t4_next_state(node);
		}
		U16(node, 0x74) = (uint16_t)(U16(node, 0x74) + 1);
		return 0;
	}

	uint32_t __cdecl t_7674E0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[14];
		states[0] = game_addr(game::TonCreature_S0_Init);
		states[1] = game_addr(game::MAG_090_CreatureActor_State1_ShowAtFrame1C);
		states[2] = game_addr(game::MAG_090_CreatureActor_State2_FadeInStartAnim);
		states[3] = game_addr(game::TonCreature_S3_Walk);
		states[4] = game_addr(game::MAG_090_CreatureActor_State4_AnimUntilCue3);
		states[5] = game_addr(game::MAG_090_CreatureActor_State5_SaveVelocity);
		states[6] = game_addr(game::MAG_090_CreatureActor_State6_StandUntilCue6);
		states[7] = game_addr(game::TonCreature_S7_Approach);
		states[8] = game_addr(game::MAG_090_CreatureActor_State8_StopAnim2);
		states[9] = game_addr(game::MAG_090_CreatureActor_State9_KnifeStab);
		states[10] = game_addr(game::MAG_090_CreatureActor_State10_WaitCue8);
		states[11] = game_addr(game::MAG_090_CreatureActor_State11_CountdownTimer);
		states[12] = game_addr(game::MAG_090_CreatureActor_State12_FadeOut);
		states[13] = game_addr(game::MAG_090_CreatureActor_State13_Empty);
		callp(states[S8(node, 0x29)], node);
		const uint32_t model = node + 0x30;
		U16(model, 0x1C) = (uint16_t)(U16(model, 0x1C) + t4_div16(S16(node, 0x124)));
		U16(model, 0x1E) = (uint16_t)(U16(model, 0x1E) + t4_div16(S16(node, 0x126)));
		U16(model, 0x20) = (uint16_t)(U16(model, 0x20) + t4_div16(S16(node, 0x128)));
		if ((U8(node, 0x26) & 4) == 0)
		{
			const uint32_t cursor = MEM<uint32_t>(game_addr(game::MAG_090_PacketCursor));
			const uint32_t next = a_746C10(node, game_addr(game::unk_25A2D00), cursor);
			MEM<uint32_t>(game_addr(game::MAG_090_PacketCursor)) = next;
			x::GetEffectSpawnPosition(model, 0x18, 0x9FC, game_addr(game::MAG_090_CreatureBonePos));
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return t4_task_end(node, status);
	}

	uint32_t __cdecl t_7677F0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_1547168));
		const uint32_t xy = U32(d, 0);
		const uint32_t zp = U32(d, 4);
		U8(node, 0x26) |= 4;
		U8(node, 0x30) |= 0x20;
		U32(node, 0x4C) = xy;
		U32(node, 0x50) = zp;
		MEM<uint16_t>(game_addr(game::MAG_090_SparkColour)) = 0;
		U32(node, 0x60) = node + 0x114;
		U32(node, 0x11C) = 0x1000;
		U32(node, 0x118) = 0x1000;
		U32(node, 0x114) = 0x1000;
		x::au_re_Battle_ReadAnimation_7(node, 3);
		x::BdPlaySE(game_addr(game::unk_154716C), 0, 0x80);
		xm::BdPlayStream(game_addr(game::unk_16D7DE8), 0x80, 1, 0x7F);
		t4_next_state(node);
		return 0;
	}

	uint32_t __cdecl t_767870(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) >= 0x1C)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) &= 0xFB;
			U16(node, 0x13A) = 0x1000;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		U16(node, 0x13C) = U16(node, 0x13A);
		t_7678B0(node, 0);
		return 0;
	}

	uint32_t __cdecl t_7678B0(uint32_t a1, uint32_t a2)
	{
		const uint32_t node = a1;
		a_733950(node + 0x30, U16(node, 0x13A), a2);
		return 0;
	}

	uint32_t __cdecl t_7679D0(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) >= 0x28)
		{
			U16(node, 0x13A) = 0;
			x::au_re_Battle_ReadAnimation_7(node, 0);
			t4_next_state(node);
		}
		else
		{
			U16(node, 0x13A) = (uint16_t)(U16(node, 0x13A) + 0xFEAB);
			if (S16(node, 0x13A) < 0)
				U16(node, 0x13A) = 0;
		}
		U16(node, 0x13C) = U16(node, 0x13A);
		t_7678B0(node, 0);
		return 0;
	}

	uint32_t __cdecl t_767A40(uint32_t a1)
	{
		const uint32_t node = a1;
		const int16_t af = S16(node, 0x136);
		if (af >= 0x4D && af <= 0x54)
		{
			const uint32_t r0 = x::CrtRand();
			const uint32_t bone = r0 & 0xFFF;
			const uint32_t p = x::Effect_AddTaskAndInitFromCtx(game_addr(game::MAG_090_TaskQueue), game_addr(game::TonSpark_Task), 0x7C, node);
			const uint32_t dst = p + 0x1C;
			const uint32_t r1 = x::CrtRand();
			U16(dst, 0) = (uint16_t)((r1 & 0xFF) - 0x80);
			const uint32_t r2 = x::CrtRand();
			U16(p, 0x1E) = (uint16_t)((r2 & 0xFF) - 0x80);
			const uint32_t r3 = x::CrtRand();
			U16(p, 0x20) = (uint16_t)((r3 & 0xFF) - 0x80);
			x::GetEffectSpawnPosition(node + 0x30, 0xD, (uint32_t)(int32_t)(int16_t)bone, dst);
		}
		if (U16(node, 0x136) == 0x5A)
		{
			MEM<uint16_t>(game_addr(game::MAG_090_SparkColour)) = 0xFF00;
			t_763E60(node, game_addr(game::MAG_090_SparkFollowTask), game_addr(game::unk_16F0F40), 0x14, 1, 0);
		}
		if (x::au_re_Battle_ReadAnimation_8(node) == 1)
		{
			t_764020(U16(node, 0x3E), 0xFFFFFED0, node + 0x124);
			x::au_re_Battle_ReadAnimation_7(node, 1);
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl t_767BA0(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t lay_b = U32(node, 0x78);
		const uint32_t lay_a = U32(node, 0x74);
		x::Effect_DecodeModelPrimLayout(lay_a, node + 0x94, lay_b);
		const uint32_t zp = MEM<uint32_t>(game_addr(game::unk_25A4C14));
		const uint16_t col = MEM<uint16_t>(game_addr(game::MAG_090_SparkColour));
		U16(node, 0x1E) = 0;
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		U32(node, 0x1C) = MEM<uint32_t>(game_addr(game::MAG_090_CreatureBonePos));
		U32(node, 0x20) = zp;
		U16(node, 0x88) = col;
		a_73C280(node);
		t4_next_state(node);
		return 0;
	}

	uint32_t __cdecl t_767C00(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t xy = MEM<uint32_t>(game_addr(game::MAG_090_CreatureBonePos));
		const uint32_t zp = MEM<uint32_t>(game_addr(game::unk_25A4C14));
		const uint16_t col = MEM<uint16_t>(game_addr(game::MAG_090_SparkColour));
		U32(node, 0x1C) = xy;
		U32(node, 0x20) = zp;
		U16(node, 0x88) = col;
		if ((uint16_t)xy == 0x7FFF)
		{
			U8(node, 0x26) |= 1;
			t4_next_state(node);
		}
		if (a_73C280(node) == 0)
		{
			U8(node, 0x26) |= 1;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl t_767C70(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_090_TonSpark_State0_Init);
		states[1] = game_addr(game::MAG_090_TonSpark_State1_FlyAnimate);
		states[2] = game_addr(game::MAG_090_TonSpark_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		t_7672F0(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return t4_task_end(node, status);
	}

	uint32_t __cdecl t_767CD0(uint32_t a1)
	{
		const uint32_t node = a1;
		U32(node, 0x4C) = game_addr(game::unk_1547344);
		U16(node, 0x56) = 0;
		U16(node, 0x52) = 9;
		const uint32_t r1 = x::CrtRand();
		U16(node, 0x6C) = (uint16_t)((int32_t)r1 % 0x300 - 0x180);
		const uint32_t r2 = x::CrtRand();
		U16(node, 0x6E) = (uint16_t)(0xFFFFFD80u - (uint32_t)((int32_t)r2 % 0x300));
		const uint32_t r3 = x::CrtRand();
		const uint8_t st = U8(node, 0x29);
		U16(node, 0x70) = (uint16_t)((int32_t)r3 % 0x300 + 0x280);
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl t_767D40(uint32_t a1)
	{
		const uint32_t node = a1;
		uint16_t vx = U16(node, 0x6C);
		uint16_t vy = U16(node, 0x6E);
		vy = (uint16_t)(vy + 0x60);
		uint16_t vz = U16(node, 0x70);
		vx = (uint16_t)(vx - t4_div8((int16_t)vx));
		U16(node, 0x6C) = vx;
		vy = (uint16_t)(vy - t4_div8((int16_t)vy));
		U16(node, 0x6E) = vy;
		vz = (uint16_t)(vz - t4_div8((int16_t)vz));
		U16(node, 0x70) = vz;
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + t4_div16((int16_t)vx));
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + t4_div16((int16_t)vy));
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + t4_div16((int16_t)vz));
		if (a_743C20(node) != 0)
		{
			U8(node, 0x26) |= 5;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl t_767DF0(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		if (a_73B7E0(3) != 0)
		{
			U16(node, 0x30) &= 0xFFDF;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl t_767E20(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		if (S16(node, 0x138) >= 3)
		{
			const uint32_t v0 = U32(node, 0x124);
			const uint32_t v1 = U32(node, 0x128);
			U32(node, 0x140) = v0;
			U32(node, 0x144) = v1;
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl t_767E60(uint32_t a1)
	{
		const uint32_t node = a1;
		if (a_73B640(6) != 0)
		{
			x::sub_8DD1C0(node);
			a_73BB60(0);
			const uint32_t v0 = U32(node, 0x140);
			const uint32_t v1 = U32(node, 0x144);
			U32(node, 0x124) = v0;
			const uint8_t st = U8(node, 0x29);
			U32(node, 0x128) = v1;
			U8(node, 0x29) = (uint8_t)(st + 1);
			return 0;
		}
		a_73BB60(1);
		U16(node, 0x128) = 0;
		U16(node, 0x126) = 0;
		U16(node, 0x124) = 0;
		return 0;
	}

	uint32_t __cdecl t_767F20(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		if (a_73B7E0(6) == 0)
			return 0;
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_1547168));
		const uint32_t xy = U32(d, 8);
		const uint16_t facing = U16(d, 0x4A);
		const uint32_t zp = U32(d, 0xC);
		U32(node, 0x4C) = xy;
		U16(node, 0x3E) = facing;
		U32(node, 0x50) = zp;
		t_764020(facing, 0xFFFFFED0, node + 0x124);
		int16_t off[4] = {0, 0, 0, 0};
		t_764020(U16(node, 0x3E), 0xFFFFFE80, P(off));
		U16(node, 0x4C) = (uint16_t)(U16(node, 0x4C) - (uint16_t)off[0]);
		U16(node, 0x4E) = (uint16_t)(U16(node, 0x4E) - (uint16_t)off[1]);
		const uint16_t facing2 = U16(node, 0x3E);
		U16(node, 0x50) = (uint16_t)(U16(node, 0x50) - (uint16_t)off[2]);
		t_764020(facing2, 0xFFFFFE84, P(off));
		U16(node, 0x50) = (uint16_t)(U16(node, 0x50) - (uint16_t)off[2]);
		const uint8_t st = U8(node, 0x29);
		U16(node, 0x4C) = (uint16_t)(U16(node, 0x4C) - (uint16_t)off[0]);
		U16(node, 0x4E) = (uint16_t)(U16(node, 0x4E) - (uint16_t)off[1]);
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl t_767FE0(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		if (S16(node, 0x138) >= 4)
		{
			U16(node, 0x128) = 0;
			U16(node, 0x126) = 0;
			U16(node, 0x124) = 0;
			x::au_re_Battle_ReadAnimation_7(node, 2);
			t4_next_state(node);
		}
		return 0;
	}

	uint32_t __cdecl t_768030(uint32_t a1)
	{
		const uint32_t node = a1;
		if (U16(node, 0x136) == 0x1E)
			x::BdPlaySummonStream(0x80, 1, 0x7F);
		if (x::au_re_Battle_ReadAnimation_8(node) == 1)
		{
			const uint8_t st = U8(node, 0x29);
			U16(node, 0x134) = 0x3C;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl t_768080(uint32_t a1)
	{
		if (a_73B640(8) != 0)
		{
			const uint32_t node = a1;
			const uint8_t st = U8(node, 0x29);
			U16(node, 0x134) = 0xA;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl t_7680B0(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x134) = (uint16_t)(U16(node, 0x134) - 1);
		if (S16(node, 0x134) <= 0)
			t4_next_state(node);
		return 0;
	}

	uint32_t __cdecl t_7680D0(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x13A) = (uint16_t)(U16(node, 0x13A) + 0x100);
		if (S16(node, 0x13A) >= 0x1000)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 5;
			U16(node, 0x13A) = 0x1000;
			MEM<uint16_t>(game_addr(game::MAG_090_CreatureBonePos)) = 0x7FFF;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		U16(node, 0x13C) = U16(node, 0x13A);
		t_7678B0(node, 1);
		t_768130(node);
		return 0;
	}

	uint32_t __cdecl t_768130(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t amb = MEM<uint32_t>(game_addr(game::CURRENT_BS_SHADOW_PALETTE));
		const int32_t level = S16(node, 0x13C);
		const uint8_t c0 = (uint8_t)(amb & 0xFF);
		const uint8_t c1 = (uint8_t)((amb >> 8) & 0xFF);
		const int32_t d0 = mul32((int32_t)c0, level) / 4096;
		U8(node, 0x5C) = (uint8_t)(c0 - (uint8_t)d0);
		const int32_t d1 = mul32((int32_t)c1, level) / 4096;
		U8(node, 0x5D) = (uint8_t)(c1 - (uint8_t)d1);
		const uint8_t c2 = MEM<uint8_t>(game_addr(game::unk_B8B9AA));
		const int32_t d2 = mul32((int32_t)c2, level) / 4096;
		U8(node, 0x5E) = (uint8_t)(c2 - (uint8_t)d2);
		return 0;
	}

	uint32_t __cdecl t_768210(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x76) = 0;
		uint32_t p = game_addr(game::unk_1D989BA);
		for (int k = 4; k != 0; --k)
		{
			MEM<uint16_t>(p - 0x28) = 0;
			MEM<uint8_t>(p) = 0;
			MEM<uint8_t>(p - 1) = 0;
			MEM<uint8_t>(p - 2) = 0;
			p += 0x2C;
		}
		t4_next_state(node);
		return 0;
	}

	uint32_t __cdecl t_768250(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x76) = (uint16_t)(U16(node, 0x76) + 0x100);
		if (S16(node, 0x76) >= 0x400)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U16(node, 0x76) = 0x400;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(node, 0x76);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int k = 4; k != 0; --k)
		{
			MEM<uint16_t>(p) = level;
			p += 0x2C;
		}
		return 0;
	}

	uint32_t __cdecl t_768300(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) >= 0x1E)
		{
			const uint8_t st = U8(node, 0x29);
			U16(node, 0x76) = 0x400;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl t_768320(uint32_t a1)
	{
		const uint32_t node = a1;
		U16(node, 0x76) = (uint16_t)(U16(node, 0x76) + 0xFF00);
		if (S16(node, 0x76) <= 0)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U16(node, 0x76) = 0;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(node, 0x76);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int k = 4; k != 0; --k)
		{
			MEM<uint16_t>(p) = level;
			p += 0x2C;
		}
		return 0;
	}

	uint32_t __cdecl t_768370(uint32_t a1)
	{
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_1547168));
		if (U16(d, 0x48) != 1)
			return 0;
		const uint32_t node = a1;
		const int32_t act = S8(node, 0x2A);
		const uint32_t ctx = U32(node, 0xC);
		const int32_t sub = S8(node, 0x2B);
		const uint32_t results = U32(ctx, 4);
		const uint32_t arr = U32(results + (uint32_t)(act * 20), 8);
		x::ApplyActionResultToTarget(arr + (uint32_t)(sub * 24));
		t4_next_state(node);
		return 0;
	}

namespace tonberry
{
	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort PORTS[] = {
		{ game::Tonberry_MasterTask, (void *)t_7624D0, "090 MAG_090_sub_7624D0" },
		{ game::MAG_090_Tonberry_Master_State2_WaitNoChildren, (void *)t_762650, "090 MAG_090_sub_762650" },
		{ game::Tonberry_Master_Init, (void *)t_762710, "090 MAG_090_sub_762710" },
		{ game::Tonberry_InitDirectorData, (void *)t_762BB0, "090 MAG_090_sub_762BB0" },
		{ game::Tonberry_DirectorTask, (void *)t_762C60, "090 MAG_090_sub_762C60" },
		{ game::Tonberry_Director_PhaseStep, (void *)t_762D10, "090 sub_762D10" },
		{ game::MAG_090_Tonberry_Director_State0_ClaimVoiceSpawnScript, (void *)t_762D60, "090 sub_762D60" },
		{ game::MAG_090_CameraScript_State0_ResetCamera, (void *)t_762E60, "090 sub_762E60" },
		{ game::MAG_090_CameraScript_State1_StartScriptAtCue2, (void *)t_762E90, "090 sub_762E90" },
		{ game::MAG_090_CameraScript_State2_StartCamScriptAtEnd, (void *)t_763690, "090 sub_763690" },
		{ game::MAG_090_CameraScript_State3_StepCamScript, (void *)t_763790, "090 sub_763790" },
		{ game::MAG_090_CameraScript_State5_PlaySEAtCue3, (void *)t_763970, "090 sub_763970" },
		{ game::MAG_090_CameraScript_State7_WaitCue4, (void *)t_7639E0, "090 sub_7639E0" },
		{ game::MAG_090_CameraScript_State9_StartScriptAtCue6, (void *)t_763A30, "090 sub_763A30" },
		{ game::MAG_090_CameraScript_State11_WaitCue7, (void *)t_763AA0, "090 sub_763AA0" },
		{ game::MAG_090_CameraScript_State12_StartScriptAtCue7, (void *)t_763AC0, "090 sub_763AC0" },
		{ game::MAG_090_CameraScript_State13_WaitCue9, (void *)t_763B10, "090 sub_763B10" },
		{ game::MAG_090_Tonberry_Director_State2_WaitDirPhase1, (void *)t_763B90, "090 sub_763B90" },
		{ game::MAG_090_Tonberry_Director_State3_WaitDirPhase3, (void *)t_763BD0, "090 sub_763BD0" },
		{ game::MAG_090_Tonberry_Director_State7_WaitAtCue7, (void *)t_763C90, "090 sub_763C90" },
		{ game::MAG_090_Tonberry_Director_State8_CountdownSetEndFlag, (void *)t_763CC0, "090 sub_763CC0" },
		{ game::MAG_090_Tonberry_Director_State9_ReleaseVoiceAtCue9, (void *)t_763CF0, "090 sub_763CF0" },
		{ game::MAG_090_SpawnTonberryCreature, (void *)t_763D60, "090 MAG_090_SpawnTonberryCreature" },
		{ game::Tonberry_SpawnModelTask, (void *)t_763E60, "090 sub_763E60" },
		{ game::Tonberry_ParticleTask, (void *)t_763EB0, "090 sub_763EB0" },
		{ game::MAG_090_Tonberry_Particle_State0_InitFromFrame6, (void *)t_763F80, "090 sub_763F80" },
		{ game::Tonberry_RotateYVector, (void *)t_764020, "090 sub_764020" },
		{ game::MAG_090_Particle_ComputeModelBounds, (void *)t_764070, "090 sub_764070" },
		{ game::MAG_090_Tonberry_Particle_State1_PlayAtCue2, (void *)t_764100, "090 sub_764100" },
		{ game::Tonberry_PrimDrawCallback, (void *)t_764260, "090 sub_764260" },
		{ game::Tonberry_RenderPrimModel, (void *)t_764540, "090 sub_764540" },
		{ game::Tonberry_PrimList_FlatTri, (void *)t_764670, "090 sub_764670" },
		{ game::TonberryModel_DrawFlatQuadList, (void *)t_764890, "090 sub_764890" },
		{ game::TonberryModel_DrawTexTriList, (void *)t_764B20, "090 sub_764B20" },
		{ game::TonberryModel_DrawTexQuadList, (void *)t_765010, "090 sub_765010" },
		{ game::Tonberry_DrawPrimBlock_GT3Scroll, (void *)t_765B30, "090 sub_765B30" },
		{ game::Tonberry_DrawPrimBlock_GT4Scroll, (void *)t_766090, "090 sub_766090" },
		{ game::MAG_090_Tonberry_Particle_State2_WaitCue3OrEnd, (void *)t_766760, "090 sub_766760" },
		{ game::Tonberry_PrimModelActorTask, (void *)t_7667B0, "090 sub_7667B0" },
		{ game::Tonberry_DrawPrimModelActor, (void *)t_766830, "090 sub_766830" },
		{ game::MAG_090_Tonberry_PrimModelActor_State0_Init, (void *)t_766EB0, "090 sub_766EB0" },
		{ game::MAG_090_Tonberry_PrimModelActor_State1_ShowAtFrame14, (void *)t_766F00, "090 sub_766F00" },
		{ game::MAG_090_Tonberry_PrimModelActor_State2_LoopFrames, (void *)t_766F20, "090 sub_766F20" },
		{ game::MAG_090_PrimModelRiseTask, (void *)t_766FB0, "090 sub_766FB0" },
		{ game::MAG_090_PrimModelRise_State0_Init, (void *)t_767020, "090 sub_767020" },
		{ game::MAG_090_PrimModelRise_State1_Show, (void *)t_767070, "090 sub_767070" },
		{ game::MAG_090_PrimModelRise_State2_RiseUntilCue7, (void *)t_767090, "090 sub_767090" },
		{ game::MAG_090_RingSpawner_State0_SetupAtFrame29, (void *)t_767150, "090 sub_767150" },
		{ game::MAG_090_RingSpawner_State1_SpawnRing, (void *)t_767190, "090 sub_767190" },
		{ game::TonRing_SpawnRing, (void *)t_7671D0, "090 sub_7671D0" },
		{ game::TonRingParticle_Task, (void *)t_767290, "090 sub_767290" },
		{ game::TonPrim_Draw, (void *)t_7672F0, "090 sub_7672F0" },
		{ game::MAG_090_TonRingParticle_State0_Init, (void *)t_767410, "090 sub_767410" },
		{ game::MAG_090_TonRingParticle_State1_Animate, (void *)t_767430, "090 sub_767430" },
		{ game::MAG_090_RingSpawner_State2_WidenAtFrame52, (void *)t_767460, "090 sub_767460" },
		{ game::MAG_090_RingSpawner_State3_LowerAndSpawn, (void *)t_767480, "090 sub_767480" },
		{ game::GF_090Tonberry_CreatureActorTask, (void *)t_7674E0, "090 GF_090Tonberry_CreatureActorTask" },
		{ game::TonCreature_S0_Init, (void *)t_7677F0, "090 sub_7677F0" },
		{ game::MAG_090_CreatureActor_State1_ShowAtFrame1C, (void *)t_767870, "090 sub_767870" },
		{ game::MAG_090_Creature_ApplyFadeTint, (void *)t_7678B0, "090 sub_7678B0" },
		{ game::MAG_090_CreatureActor_State2_FadeInStartAnim, (void *)t_7679D0, "090 sub_7679D0" },
		{ game::TonCreature_S3_Walk, (void *)t_767A40, "090 sub_767A40" },
		{ game::MAG_090_SparkFollow_State0_Init, (void *)t_767BA0, "090 sub_767BA0" },
		{ game::MAG_090_SparkFollow_State1_FollowCreature, (void *)t_767C00, "090 sub_767C00" },
		{ game::TonSpark_Task, (void *)t_767C70, "090 sub_767C70" },
		{ game::MAG_090_TonSpark_State0_Init, (void *)t_767CD0, "090 sub_767CD0" },
		{ game::MAG_090_TonSpark_State1_FlyAnimate, (void *)t_767D40, "090 sub_767D40" },
		{ game::MAG_090_CreatureActor_State4_AnimUntilCue3, (void *)t_767DF0, "090 sub_767DF0" },
		{ game::MAG_090_CreatureActor_State5_SaveVelocity, (void *)t_767E20, "090 sub_767E20" },
		{ game::MAG_090_CreatureActor_State6_StandUntilCue6, (void *)t_767E60, "090 sub_767E60" },
		{ game::TonCreature_S7_Approach, (void *)t_767F20, "090 sub_767F20" },
		{ game::MAG_090_CreatureActor_State8_StopAnim2, (void *)t_767FE0, "090 sub_767FE0" },
		{ game::MAG_090_CreatureActor_State9_KnifeStab, (void *)t_768030, "090 sub_768030" },
		{ game::MAG_090_CreatureActor_State10_WaitCue8, (void *)t_768080, "090 sub_768080" },
		{ game::MAG_090_CreatureActor_State11_CountdownTimer, (void *)t_7680B0, "090 sub_7680B0" },
		{ game::MAG_090_CreatureActor_State12_FadeOut, (void *)t_7680D0, "090 sub_7680D0" },
		{ game::TonCreature_DarkenColour, (void *)t_768130, "090 sub_768130" },
		{ game::MAG_090_EntityFadeIn_State0_Clear, (void *)t_768210, "090 sub_768210" },
		{ game::MAG_090_EntityFadeIn_State1_RampUp, (void *)t_768250, "090 sub_768250" },
		{ game::MAG_090_EntityFadeOut_State0_SetLevelAtFrame1E, (void *)t_768300, "090 sub_768300" },
		{ game::MAG_090_EntityFadeOut_State1_FadeOut, (void *)t_768320, "090 sub_768320" },
		{ game::TonDamage_ApplyResult, (void *)t_768370, "090 MAG_090_sub_768370" },
		{ {}, nullptr, nullptr }
	};
}
}
	void register_mag090_tonberry()
	{
		act::register_module(90);
		for (const act::tonberry::ModPort *p = act::tonberry::PORTS; p->addr; p++)
			act::register_module_port(90, p->addr, p->port, p->name);
	}
}

