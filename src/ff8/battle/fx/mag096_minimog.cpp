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

// Effect 096: MiniMog - Moogle Dance

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
	uint32_t __cdecl m_731F70(uint32_t a1);
	uint32_t __cdecl m_732250(uint32_t a1);
	uint32_t __cdecl m_7326F0(void);
	uint32_t __cdecl m_7327B0(uint32_t a1);
	uint32_t __cdecl m_7328B0(uint32_t a1);
	uint32_t __cdecl m_732950(uint32_t a1);
	uint32_t __cdecl m_732AC0(uint32_t a1);
	uint32_t __cdecl m_7332C0(uint32_t a1);
	uint32_t __cdecl m_733310(uint32_t a1);
	uint32_t __cdecl m_733340(uint32_t a1);
	uint32_t __cdecl m_733370(uint32_t a1);
	uint32_t __cdecl m_7333A0(uint32_t a1);
	uint32_t __cdecl m_7333D0(uint32_t a1);
	uint32_t __cdecl m_733400(uint32_t a1);
	uint32_t __cdecl m_733430(uint32_t a1);
	uint32_t __cdecl m_733460(uint32_t a1);
	uint32_t __cdecl m_733490(uint32_t a1);
	uint32_t __cdecl m_733510(uint32_t a1);
	uint32_t __cdecl m_733610(uint32_t a1);
	uint32_t __cdecl m_733700(uint32_t a1);
	uint32_t __cdecl m_733730(uint32_t a1);
	uint32_t __cdecl m_733780(uint32_t a1);
	uint32_t __cdecl m_7337D0(uint32_t a1);
	uint32_t __cdecl m_733870(uint32_t a1);
	uint32_t __cdecl m_733890(uint32_t a1);
	uint32_t __cdecl m_733910(uint32_t a1, uint32_t a2);
	uint32_t __cdecl m_733B20(uint32_t a1);
	uint32_t __cdecl m_733B90(uint32_t a1);
	uint32_t __cdecl m_733C40(uint32_t a1);
	uint32_t __cdecl m_733C90(uint32_t a1);
	uint32_t __cdecl m_733D40(uint32_t a1);
	uint32_t __cdecl m_733DA0(uint32_t a1);
	uint32_t __cdecl m_733E10(uint32_t a1);
	uint32_t __cdecl m_733E90(uint32_t a1);
	uint32_t __cdecl m_733F70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);
	uint32_t __cdecl m_733FC0(uint32_t a1);
	uint32_t __cdecl m_734400(uint32_t a1);
	uint32_t __cdecl m_734450(uint32_t a1);
	uint32_t __cdecl m_734580(uint32_t a1, uint32_t a2, uint32_t a3);
	uint32_t __cdecl m_734810(uint32_t a1);
	uint32_t __cdecl m_739610(uint32_t a1);
	uint32_t __cdecl m_739A50(uint32_t a1);
	uint32_t __cdecl m_739B00(uint32_t a1);
	uint32_t __cdecl m_739B70(uint32_t a1);
	uint32_t __cdecl m_739BB0(uint32_t a1);

	namespace
	{
		inline uint32_t m1_add_task(uint32_t queue, uint32_t fn, uint32_t size, uint32_t parent)
		{
			return x::Effect_AddTaskAndInitFromCtx(queue, fn, size, parent);
		}
		inline uint32_t m1_task_end(uint32_t node, uint8_t status)
		{
			if ((status & 1) != 0 && U8(node, 0x28) == 0)
			{
				x::Effect_ReleaseLinkedTask(node);
				return 2;
			}
			return 0;
		}
		inline void m1_script_chain(uint32_t a1, uint32_t script)
		{
			if (a_73AE10() == 1)
			{
				a_73AB20(script, MEM<uint32_t>(game_addr(game::unk_152BB90)), 0);
				U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
			}
		}
		inline uint32_t m1_section(uint32_t data, int32_t off)
		{
			const int32_t v = S32(data, off);
			return data + (uint32_t)shl32(v / 4, 2);
		}
	}

	uint32_t __cdecl m_731F70(uint32_t a1)
	{
		set_mod_by_code(U32(a1, 8));

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::unk_257F71C)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_257B850)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_096_MiniMog_Master_State0_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_096_MiniMog_Master_State1_Advance);
		states[2] = game_addr(game::MAG_096_MiniMog_Master_State2_CarveWorkBuffers);
		states[3] = game_addr(game::MAG_096_MiniMog_Master_State3_SpawnEmitter);
		states[4] = game_addr(game::MAG_096_MiniMog_Master_State4_NextTargetLoop);
		states[5] = game_addr(game::MAG_096_MiniMog_Master_State5_Advance);
		states[6] = game_addr(game::MAG_096_MiniMog_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_096_MiniMog_Master_State7_RequestBattleSync);
		states[8] = game_addr(game::MAG_096_MiniMog_Master_State8_WaitBattleSyncDone);
		states[9] = game_addr(game::MAG_096_MiniMog_Master_State9_Finish);
		states[10] = game_addr(game::MAG_096_MiniMog_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_257E3DC));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_257DA64));
			MEM<uint32_t>(game_addr(game::unk_257B744)) = v1;
			MEM<uint32_t>(game_addr(game::unk_257B84C)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_257E3D8));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_257DA60));
			MEM<uint32_t>(game_addr(game::unk_257B744)) = v1;
			MEM<uint32_t>(game_addr(game::unk_257B84C)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		MEM<uint16_t>(game_addr(game::unk_257F704)) = 0;
		MEM<uint16_t>(game_addr(game::unk_257F688)) = 0;
		static constexpr game::Id queues[8] = { game::unk_257E268, game::MAG_096_DirectorQueue, game::MAG_096_LoadScriptQueue, game::MAG_096_CreatureQueue, game::unk_257E258, game::MAG_096_ParticleModelQueue, game::MAG_096_TaskQueue, game::unk_257CF28 };
		for (int i = 0; i < 8; i++)
		{
			const uint32_t n = x::ExecuteTaskQueue(game_addr(queues[i]));
			U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)n);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return m1_task_end(node, status);
	}

	uint32_t __cdecl m_732250(uint32_t a1)
	{
		m_7326F0();
		a_7322A0();
		m1_add_task(game_addr(game::MAG_096_LoadScriptQueue), game_addr(game::MAG_096_FileLoadScriptTask), 0x30, a1);
		m1_add_task(game_addr(game::MAG_096_DirectorQueue), game_addr(game::MiniMog_DirectorTask), 0x48, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl m_7326F0(void)
	{
		x::MAG_007_sub_8DCC00(MEM<uint32_t>(game_addr(game::unk_152BB90)), 0x54);
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_152BB90));
		U16(d, 0) = 0;
		U16(d, 2) = 0xFF00;
		U16(d, 4) = 0;
		a_73A720(d + 8);
		return 0;
	}

	uint32_t __cdecl m_7327B0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[15];
		states[0] = game_addr(game::MAG_096_MiniMog_Director_State0_WaitDirGate1);
		states[1] = game_addr(game::MAG_096_MiniMog_Director_State1_SpawnTasksAtCue1);
		states[2] = game_addr(game::MAG_096_MiniMog_Director_State2_CountdownSpawnEntityFade);
		states[3] = game_addr(game::MAG_096_MiniMog_Director_State3_ArmLoadScriptStep);
		states[4] = game_addr(game::MAG_096_MiniMog_Director_State4_WaitLoadScriptIdle);
		states[5] = game_addr(game::MAG_096_MiniMog_Director_State5_Wait20Frames);
		states[6] = game_addr(game::MAG_096_MiniMog_Director_State6_WaitDirGate2);
		states[7] = game_addr(game::MAG_096_MiniMog_Director_State7_WaitAtCue2);
		states[8] = game_addr(game::MAG_096_MiniMog_Director_State8_CountdownTimer);
		states[9] = game_addr(game::MAG_096_MiniMog_Director_State9_WaitDirGate3);
		states[10] = game_addr(game::MAG_096_MiniMog_Director_State10_SpawnFadesAtCue3);
		states[11] = game_addr(game::MAG_096_MiniMog_Director_State11_CountdownTimer);
		states[12] = game_addr(game::MAG_096_MiniMog_Director_State12_ReleaseVoiceAtCue4);
		states[13] = game_addr(game::MAG_096_MiniMog_Director_State13_Finish);
		states[14] = game_addr(game::MAG_096_MiniMog_Director_State14_Empty);
		m_7328B0(node);
		callp(states[S8(node, 0x29)], node);
		const uint32_t bucket = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x4484;
		const uint32_t pkt = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
		U32(pkt, 0) = 2;
		U32(pkt, 4) = 0xE1000020;
		U32(pkt, 8) = 0;
		x::SSIGPU_InsertPrimAutoDepth(bucket, pkt);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = pkt + 0xC;
		return m1_task_end(node, status);
	}

	uint32_t __cdecl m_7328B0(uint32_t a1)
	{
		uint32_t d = MEM<uint32_t>(game_addr(game::unk_152BB90));
		U16(d, 0x46) = (uint16_t)(U16(d, 0x46) + 1);
		const uint16_t req = U16(d, 0x42);
		if (U16(d, 0x40) != req)
		{
			U16(d, 0x40) = req;
			U16(d, 0x46) = 0;
			m_733E90(a1);
			d = MEM<uint32_t>(game_addr(game::unk_152BB90));
		}
		const uint16_t next = U16(d, 0x44);
		if (U16(d, 0x42) != next)
		{
			U16(d, 0x42) = next;
			a_73A6D0();
		}
		return 0;
	}

	uint32_t __cdecl m_732950(uint32_t a1)
	{
		if (a_73B7E0(1) == 0)
			return 0;
		x::BdPlaySE(game_addr(game::unk_152BB94), 0, 0x80);
		const uint32_t node = a1;
		m1_add_task(game_addr(game::MAG_096_TaskQueue), game_addr(game::MAG_096_CameraScriptTask), 0x70, node);
		m1_add_task(game_addr(game::MAG_096_TaskQueue), game_addr(game::MAG_096_StageFadeInTask), 0x70, node);
		m1_add_task(game_addr(game::MAG_096_TaskQueue), game_addr(game::MAG_096_MusicFadeTask), 0x70, node);
		const uint32_t slot = x::BdSound_ClaimVoiceSlot(game_addr(game::unk_1692140), 1, 0x80);
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_152BB90));
		U16(node, 0x44) = 0xC;
		U16(d, 0x52) = (uint16_t)slot;
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl m_732AC0(uint32_t a1)
	{
		a_73AB00();
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_152BB90));
		a_73AB20(game_addr(game::unk_1532C68), d + 8, 0);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl m_7332C0(uint32_t a1)
	{
		a_73AE10();
		if (a_73AAE0(2) != 0)
		{
			a_73AB20(game_addr(game::unk_1532CB0), MEM<uint32_t>(game_addr(game::unk_152BB90)), 0);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl m_733310(uint32_t a1) { m1_script_chain(a1, game_addr(game::unk_1532CF8)); return 0; }
	uint32_t __cdecl m_733340(uint32_t a1) { m1_script_chain(a1, game_addr(game::unk_1532D40)); return 0; }
	uint32_t __cdecl m_733370(uint32_t a1) { m1_script_chain(a1, game_addr(game::unk_1532D88)); return 0; }
	uint32_t __cdecl m_7333A0(uint32_t a1) { m1_script_chain(a1, game_addr(game::unk_1532DD0)); return 0; }
	uint32_t __cdecl m_7333D0(uint32_t a1) { m1_script_chain(a1, game_addr(game::unk_1532E18)); return 0; }
	uint32_t __cdecl m_733400(uint32_t a1) { m1_script_chain(a1, game_addr(game::unk_1532E60)); return 0; }
	uint32_t __cdecl m_733430(uint32_t a1) { m1_script_chain(a1, game_addr(game::unk_1532EA8)); return 0; }
	uint32_t __cdecl m_733460(uint32_t a1) { m1_script_chain(a1, game_addr(game::unk_1532EF0)); return 0; }
	uint32_t __cdecl m_733490(uint32_t a1) { m1_script_chain(a1, game_addr(game::unk_1532F38)); return 0; }

	uint32_t __cdecl m_733510(uint32_t a1)
	{
		if (a_73AAE0(4) != 0)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl m_733610(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0x100);
		if (S16(a1, 0x1C) >= 0x1000)
		{
			U16(a1, 0x1C) = 0x1000;
			a_746A10();
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int i = 4; i != 0; i--, p += 0x2C)
			MEM<uint16_t>(p) = level;
		return 0;
	}

	uint32_t __cdecl m_733700(uint32_t a1)
	{
		if (a_73B7E0(2) != 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0x1C) = 0x7F;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl m_733730(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) - 8);
		if (S16(a1, 0x1C) <= 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0x1C) = 0;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		x::Music_SetVolumeImmediate(0, (uint32_t)(int32_t)S16(a1, 0x1C));
		return 0;
	}

	uint32_t __cdecl m_733780(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + (int8_t)MEM<uint8_t>(game_addr(game::sub_733788)));
		if (S16(a1, 0x1C) >= 0x7F)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0x7F;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		x::Music_SetVolumeImmediate(0, (uint32_t)(int32_t)S16(a1, 0x1C));
		return 0;
	}

	uint32_t __cdecl m_7337D0(uint32_t a1)
	{
		U16(a1, 0x44) = (uint16_t)(U16(a1, 0x44) - 1);
		if (S16(a1, 0x44) <= 0)
		{
			m1_add_task(game_addr(game::MAG_096_TaskQueue), game_addr(game::MAG_096_EntityFadeInTask), 0x70, a1);
			U16(a1, 0x44) = U16(a1, 0x24);
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl m_733870(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x1C) = 0;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl m_733890(uint32_t a1)
	{
		uint32_t eax_hi = (uint32_t)(int32_t)S8(a1, 0x29) & 0xFFFF0000u;
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0x100);
		if (S16(a1, 0x1C) >= 0x1000)
		{
			U16(a1, 0x1C) = 0x1000;
			eax_hi = a_73B9F0() & 0xFFFF0000u;
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		m_733910(eax_hi | U16(a1, 0x1C), 3);
		return 0;
	}

	uint32_t __cdecl m_733910(uint32_t a1, uint32_t a2)
	{
		for (uint32_t ent = game_addr(game::unk_1D97494); ent < game_addr(game::CAMERA_FLAG_RELATED); ent += 0x9C)
		{
			uint16_t flags = U16(ent, 0);
			if ((flags & 2) != 0)
			{
				flags = (uint16_t)(flags | 0x800);
				U16(ent, 0) = flags;
				a_733950(ent, a1, a2);
			}
		}
		return 0;
	}

	uint32_t __cdecl m_733B20(uint32_t a1)
	{
		if (a_73B7E0(2) != 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U16(a1, 0x44) = 0x90;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl m_733B90(uint32_t a1)
	{
		if (a_73B7E0(3) == 0)
			return 0;
		m1_add_task(game_addr(game::MAG_096_TaskQueue), game_addr(game::MAG_096_StageFadeOutTask), 0x70, a1);
		m1_add_task(game_addr(game::MAG_096_TaskQueue), game_addr(game::MAG_096_EntityFadeOutTask), 0x70, a1);
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x44) = MEM<uint16_t>(game_addr(game::sub_733BD3));
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl m_733C40(uint32_t a1)
	{
		a_746AE0();
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x1C) = 0x1000;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl m_733C90(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + MEM<uint16_t>(game_addr(game::sub_733C98)));
		if (S16(a1, 0x1C) <= 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int i = 4; i != 0; i--, p += 0x2C)
			MEM<uint16_t>(p) = level;
		return 0;
	}

	uint32_t __cdecl m_733D40(uint32_t a1)
	{
		a_73BA90();
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x1C) = 0x1000;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl m_733DA0(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + MEM<uint16_t>(game_addr(game::sub_733DA8)));
		if (S16(a1, 0x1C) <= 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		m_733910((a1 & 0xFFFF0000u) | U16(a1, 0x1C), 3);
		return 0;
	}

	uint32_t __cdecl m_733E10(uint32_t a1)
	{
		if (a_73B640(4) == 0)
			return 0;
		x::sub_4A2940((uint32_t)(int32_t)S16(MEM<uint32_t>(game_addr(game::unk_152BB90)), 0x52));
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_152BB90));
		const uint8_t st = U8(a1, 0x29);
		U16(d, 0x48) = 1;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	uint32_t __cdecl m_733E90(uint32_t a1)
	{
		const int32_t diff = (int32_t)S16(MEM<uint32_t>(game_addr(game::unk_152BB90)), 0x40) - 2;
		if (diff != 0)
			return (uint32_t)diff;
		a_73C100(a1, game_addr(game::MAG_096_ActorSystemTask), game_addr(game::unk_169227C), 0, 0x2D, 2);
		m_733F70(a1, game_addr(game::MAG_096_ParticleModelTask), game_addr(game::unk_169A1BC), 0x488, 0, 0);
		const uint32_t creature = m1_add_task(game_addr(game::MAG_096_CreatureQueue), game_addr(game::MiniMog_CreatureActorTask), 0x140, a1);
		x::Effect_BindModelContainerSetAnim(creature, game_addr(game::MAG096_Mog_ModelContainer), 3);
		return a_73C100(a1, game_addr(game::MAG_096_ActorSystemTask), game_addr(game::unk_1696DE8), 0x6E, 0x2D, 2);
	}

	uint32_t __cdecl m_733F70(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
	{
		const uint32_t t = m1_add_task(game_addr(game::MAG_096_ParticleModelQueue), a2, 0x51C, a1);
		U16(t, 0x80) = (uint16_t)a5;
		U32(t, 0x74) = a3;
		U32(t, 0x78) = (uint32_t)(int32_t)(int16_t)a4;
		U16(t, 0x82) = (uint16_t)a6;
		return t;
	}

	uint32_t __cdecl m_733FC0(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_096_ParticleModel_State0_Init);
		states[1] = game_addr(game::MAG_096_ParticleModel_State1_Play);
		states[2] = game_addr(game::MAG_096_ParticleModel_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		a_7340A0(m1_section(U32(node, 0x74), 0x3C), 2);
		a_7340A0(m1_section(U32(node, 0x74), 0x40), 2);
		a_7340A0(m1_section(U32(node, 0x74), 0x44), 2);
		a_7340A0(m1_section(U32(node, 0x74), 0x54), 2);
		a_7340A0(m1_section(U32(node, 0x74), 0x58), 2);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return m1_task_end(node, status);
	}

	uint32_t __cdecl m_734400(uint32_t a1)
	{
		const uint32_t node = a1;
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, U32(node, 0x78));
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_152BB90));
		const uint32_t xy = U32(d, 0);
		const uint32_t zp = U32(d, 4);
		U32(node, 0x1C) = xy;
		U32(node, 0x20) = zp;
		U16(node, 0x1E) = 0;
		U32(node, 0x58) = 0x1000;
		U32(node, 0x54) = 0x1000;
		U32(node, 0x50) = 0x1000;
		m_734450(node);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl m_734450(uint32_t a1)
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
		U32(L, 0x48) = MEM<uint32_t>(game_addr(game::MAG_096_VertexLerpScratchPtr));
		U16(L, 0x54) = U16(node, 0x86);
		const uint32_t v7c = U32(node, 0x7C);
		U32(L, 0x38) = U32(node, 0x70);
		U16(L, 0x4C) = U16(node, 0x8C);
		const uint16_t w90 = U16(node, 0x90);
		const uint16_t w92 = U16(node, 0x92);
		U32(L, 0x44) = v7c;
		const uint16_t w8e = U16(node, 0x8E);
		U16(L, 0x50) = w90;
		const uint32_t paused = MEM<uint32_t>(game_addr(game::MAG_096_PrimPlayerPause));
		U16(L, 0x52) = w92;
		U16(L, 0x4E) = w8e;
		U16(L, 0x56) = U16(node, 0x80);
		return prim_play(node + 0x94, game_addr(game::MiniMog_Prim_DrawObject), L, paused);
	}

	uint32_t __cdecl m_734580(uint32_t a1, uint32_t a2, uint32_t a3)
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

		if ((U32(rec, 4) & 0x400) != 0)
			x::MAG_117_sub_701430(rec + 0x10, M);
		else
			x::MAG_017_sub_701310(rec + 0x10, M);
		const uint16_t vx = U16(rec, 8);
		const uint16_t vy = U16(rec, 0xA);
		const uint16_t vz = U16(rec, 0xC);
		U16(L, 0) = vx;
		U16(L, 2) = vy;
		U16(L, 4) = vz;
		x::GTE_SetRotMatrix(blk);
		x::GTE_LoadV0(L);
		x::GTE_MVMVA_RotV0();
		x::GTE_ReadMAC123(M + 0x14);
		x::GTE_MatrixMultiply(blk, M);
		{
			const int32_t bx = S32(blk, 0x14);
			const int32_t t0 = S32(M, 0x14);
			const int32_t t1 = S32(M, 0x18);
			const int32_t t2 = S32(M, 0x1C);
			S32(M, 0x14) = add32(t0, bx);
			const int32_t by = S32(blk, 0x18);
			S32(M, 0x18) = add32(t1, by);
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
		if ((flags & 0x2000) != 0)
			U32(hdr, 0x1C) |= 0xC;
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
		x::Field_Free(0x58);
		return 0;
	}

	uint32_t __cdecl m_734810(uint32_t a1)
	{
		if (m_734450(a1) == 0)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	uint32_t __cdecl m_739610(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[6];
		states[0] = game_addr(game::MAG_096_MiniMog_CreatureActor_State0_Init);
		states[1] = game_addr(game::MAG_096_MiniMog_CreatureActor_State1_SwitchTexAnimAnim1);
		states[2] = game_addr(game::MAG_096_MiniMog_CreatureActor_State2_WaitModelAnimDone);
		states[3] = game_addr(game::MAG_096_MiniMog_CreatureActor_State3_Anim2);
		states[4] = game_addr(game::MAG_096_MiniMog_CreatureActor_State4_FinishAtFrame1F);
		states[5] = game_addr(game::MAG_096_MiniMog_CreatureActor_State5_Empty);
		callp(states[S8(node, 0x29)], node);
		if ((U8(node, 0x26) & 4) == 0)
		{
			const uint32_t cursor = MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR));
			MEM<uint32_t>(game_addr(game::BATTLE_PACKET_WRITE_CURSOR)) = a_746C10(node, game_addr(game::MAG_096_CreatureOtList), cursor);
			a_739890(node);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return m1_task_end(node, status);
	}

	uint32_t __cdecl m_739A50(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t d = MEM<uint32_t>(game_addr(game::unk_152BB90));
		const uint32_t xy = U32(d, 0);
		const uint32_t zp = U32(d, 4);
		U32(node, 0x4C) = xy;
		U32(node, 0x50) = zp;
		U32(node, 0x60) = node + 0x114;
		U32(node, 0x11C) = 0x1000;
		U32(node, 0x118) = 0x1000;
		U32(node, 0x114) = 0x1000;
		x::au_re_Battle_ReadAnimation_7(node, 0);
		a_739AC0(node, game_addr(game::unk_1532FC8), 1);
		x::BdPlaySE(game_addr(game::unk_152BB98), 1, 0x80);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	uint32_t __cdecl m_739B00(uint32_t a1)
	{
		const uint32_t node = a1;
		if (U16(node, 0x24) == 0x14)
			a_739AC0(node, game_addr(game::unk_1532FD8), 0);
		if (x::au_re_Battle_ReadAnimation_8(node) == 1)
		{
			x::au_re_Battle_ReadAnimation_7(node, 1);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl m_739B70(uint32_t a1)
	{
		const uint32_t node = a1;
		if (x::au_re_Battle_ReadAnimation_8(node) == 1)
		{
			x::au_re_Battle_ReadAnimation_7(node, 2);
			a_739AC0(node, game_addr(game::unk_1532FE8), 0);
			U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		}
		return 0;
	}

	uint32_t __cdecl m_739BB0(uint32_t a1)
	{
		const uint32_t node = a1;
		x::sub_8DD1C0(node);
		if (S16(node, 0x136) >= 0x1F)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 5;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

namespace minimog
{
	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort PORTS[] = {
		{ game::MiniMog_MasterTask, (void *)m_731F70, "096 MAG_096_sub_731F70" },
		{ game::MAG_096_Emitter_State0_InitDirectorAndTasks, (void *)m_732250, "096 MAG_096_sub_732250" },
		{ game::MAG_096_InitDirectorData, (void *)m_7326F0, "096 MAG_096_sub_7326F0" },
		{ game::MiniMog_DirectorTask, (void *)m_7327B0, "096 MAG_096_sub_7327B0" },
		{ game::MAG_096_Director_PhaseBookkeeping, (void *)m_7328B0, "096 sub_7328B0" },
		{ game::MAG_096_MiniMog_Director_State1_SpawnTasksAtCue1, (void *)m_732950, "096 sub_732950" },
		{ game::MAG_096_CameraScript_State0_StartScript, (void *)m_732AC0, "096 sub_732AC0" },
		{ game::MAG_096_CameraScript_State1_StepNextAtCue2, (void *)m_7332C0, "096 sub_7332C0" },
		{ game::MAG_096_CameraScript_State2_NextScriptAtEnd, (void *)m_733310, "096 sub_733310" },
		{ game::MAG_096_CameraScript_State3_NextScriptAtEnd, (void *)m_733340, "096 sub_733340" },
		{ game::MAG_096_CameraScript_State4_NextScriptAtEnd, (void *)m_733370, "096 sub_733370" },
		{ game::MAG_096_CameraScript_State5_NextScriptAtEnd, (void *)m_7333A0, "096 sub_7333A0" },
		{ game::MAG_096_CameraScript_State6_NextScriptAtEnd, (void *)m_7333D0, "096 sub_7333D0" },
		{ game::MAG_096_CameraScript_State7_NextScriptAtEnd, (void *)m_733400, "096 sub_733400" },
		{ game::MAG_096_CameraScript_State8_NextScriptAtEnd, (void *)m_733430, "096 sub_733430" },
		{ game::MAG_096_CameraScript_State9_NextScriptAtEnd, (void *)m_733460, "096 sub_733460" },
		{ game::MAG_096_CameraScript_State10_NextScriptAtEnd, (void *)m_733490, "096 sub_733490" },
		{ game::MAG_096_CameraScript_State13_WaitCue4, (void *)m_733510, "096 sub_733510" },
		{ game::MAG_096_StageFadeIn_State1_FadeIn, (void *)m_733610, "096 sub_733610" },
		{ game::MAG_096_MusicFade_State0_SetVolumeAtCue2, (void *)m_733700, "096 sub_733700" },
		{ game::MAG_096_MusicFade_State1_FadeOut, (void *)m_733730, "096 sub_733730" },
		{ game::MAG_096_MusicFade_State3_FadeIn, (void *)m_733780, "096 sub_733780" },
		{ game::MAG_096_MiniMog_Director_State2_CountdownSpawnEntityFade, (void *)m_7337D0, "096 sub_7337D0" },
		{ game::MAG_096_EntityFadeIn_State0_ClearLevel, (void *)m_733870, "096 sub_733870" },
		{ game::MAG_096_EntityFadeIn_State1_FadeIn, (void *)m_733890, "096 sub_733890" },
		{ game::MAG_096_ApplyEntityFadeLevel, (void *)m_733910, "096 sub_733910" },
		{ game::MAG_096_MiniMog_Director_State7_WaitAtCue2, (void *)m_733B20, "096 sub_733B20" },
		{ game::MAG_096_MiniMog_Director_State10_SpawnFadesAtCue3, (void *)m_733B90, "096 sub_733B90" },
		{ game::MAG_096_StageFadeOut_State0_Set1000, (void *)m_733C40, "096 sub_733C40" },
		{ game::MAG_096_StageFadeOut_State1_FadeOut, (void *)m_733C90, "096 sub_733C90" },
		{ game::MAG_096_EntityFadeOut_State0_RestoreSet1000, (void *)m_733D40, "096 sub_733D40" },
		{ game::MAG_096_EntityFadeOut_State1_FadeOut, (void *)m_733DA0, "096 sub_733DA0" },
		{ game::MAG_096_MiniMog_Director_State12_ReleaseVoiceAtCue4, (void *)m_733E10, "096 sub_733E10" },
		{ game::MAG_096_SpawnMogCreature, (void *)m_733E90, "096 MAG_096_SpawnMogCreature" },
		{ game::MAG_096_SpawnModelTask, (void *)m_733F70, "096 sub_733F70" },
		{ game::MAG_096_ParticleModelTask, (void *)m_733FC0, "096 sub_733FC0" },
		{ game::MAG_096_ParticleModel_State0_Init, (void *)m_734400, "096 sub_734400" },
		{ game::MAG_096_ParticleModel_PlayStep, (void *)m_734450, "096 sub_734450" },
		{ game::MiniMog_Prim_DrawObject, (void *)m_734580, "096 sub_734580" },
		{ game::MAG_096_ParticleModel_State1_Play, (void *)m_734810, "096 sub_734810" },
		{ game::MiniMog_CreatureActorTask, (void *)m_739610, "096 sub_739610" },
		{ game::MAG_096_MiniMog_CreatureActor_State0_Init, (void *)m_739A50, "096 sub_739A50" },
		{ game::MAG_096_MiniMog_CreatureActor_State1_SwitchTexAnimAnim1, (void *)m_739B00, "096 sub_739B00" },
		{ game::MAG_096_MiniMog_CreatureActor_State3_Anim2, (void *)m_739B70, "096 sub_739B70" },
		{ game::MAG_096_MiniMog_CreatureActor_State4_FinishAtFrame1F, (void *)m_739BB0, "096 sub_739BB0" },
		{ {}, nullptr, nullptr }
	};
}
}
	void register_mag096_minimog()
	{
		act::register_module(96);
		for (const act::minimog::ModPort *p = act::minimog::PORTS; p->addr; p++)
			act::register_module_port(96, p->addr, p->port, p->name);
	}
}

