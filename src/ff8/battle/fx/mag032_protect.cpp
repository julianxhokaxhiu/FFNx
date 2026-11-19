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

// Effect 32: Protect: a copy of the actor/heal effect library

#include "act_engine.h"

namespace ff8fx
{
namespace act
{
namespace protect
{
	static const Mod MOD_032 = { "protect", 32, game::MAG_032_PROTECT_FL, game::MAG_031_HEARTBREAK_FL,
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::g_Protect_EmitterQueue, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} },
		{ {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, game::MAG_032_PROTECT_EmitterTask, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {} } };

	static constexpr game::Id PACKET_CURSOR = game::g_Protect_PacketCursor;
	static constexpr game::Id Q_EMITTER = game::g_Protect_EmitterQueue;
	static constexpr game::Id Q_FLASH = game::g_Protect_FlashQueue;
	static constexpr game::Id Q_PARTICLE = game::g_Protect_ParticleQueue;
	static constexpr game::Id Q_SHIELD = game::g_Protect_ShieldQueue;
	static constexpr game::Id Q_STAGE = game::g_Protect_StageQueue;
	static constexpr game::Id BLEND_BUFFER = game::g_Protect_BlendBuffer;

	static constexpr game::Id ORIG_Master = game::MAG_032_PROTECT_Tick;
	static constexpr game::Id ORIG_Emitter = game::MAG_032_PROTECT_EmitterTask;
	static constexpr game::Id ORIG_Flash = game::MAG_032_PROTECT_ScreenFlashTask;
	static constexpr game::Id ORIG_ShieldPlayer = game::MAG_032_PROTECT_ShieldPlayerTask;
	static constexpr game::Id ORIG_ShieldDraw = game::MAG_032_PROTECT_ShieldDrawCallback;
	static constexpr game::Id ORIG_Spawner = game::MAG_032_PROTECT_ParticleSpawnerTask;
	static constexpr game::Id ORIG_Particle = game::MAG_032_PROTECT_ParticleTask;
	static const void *SOUND_Protect() { return (const void *)game_addr(game::unk_15F3B20); }

	static inline uint32_t TargetEntity(uint32_t node)
	{
		return (uint32_t)U8(node, 0x2D) * 0x9C + game_addr(game::BattleEntitySlotData);
	}

	static inline uint32_t task_end(uint32_t node, uint8_t status)
	{
		if ((status & 1) != 0 && U8(node, 0x28) == 0)
		{
			x::Effect_ReleaseLinkedTask(node);
			return 2;
		}
		return 0;
	}
}
}
}

namespace ff8fx
{
namespace act
{
namespace protect
{
	static uint32_t __cdecl Master(uint32_t a1)
	{
		g_mod = &MOD_032;

		uint32_t states[11];
		ff8fx::copy<(8 * 4)>((void *)game_addr(game::Effect_SharedCamMatrixSnapshot), (const void *)game_addr(game::unk_1D97778));
		const uint32_t node = a1;
		MEM<uint32_t>(game_addr(game::unk_26CDCBC)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		MEM<uint32_t>(game_addr(game::unk_26C3974)) = game_addr(game::Effect_SharedCamMatrixSnapshot);
		states[0] = game_addr(game::MAG_032_PROTECT_Master_State0_Advance);
		const uint8_t parity = U8(node, 0x5C);
		states[1] = game_addr(game::MAG_032_PROTECT_Master_State1_Advance);
		states[2] = game_addr(game::MAG_032_PROTECT_StartWobbleIn);
		states[3] = game_addr(game::MAG_032_PROTECT_SpawnEmitter);
		states[4] = game_addr(game::MAG_032_PROTECT_ActionLoop);
		states[5] = game_addr(game::MAG_032_PROTECT_StartWobbleOut);
		states[6] = game_addr(game::MAG_032_PROTECT_Master_State6_WaitQueuesEmpty);
		states[7] = game_addr(game::MAG_032_PROTECT_Master_State7_Advance);
		states[8] = game_addr(game::MAG_032_PROTECT_Master_State8_Advance);
		states[9] = game_addr(game::MAG_032_PROTECT_Master_State9_Finish);
		states[10] = game_addr(game::MAG_032_PROTECT_Master_State10_Empty);
		if ((parity & 1) != 0)
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_26CDAEC));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_26CD6E4));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_26C3970)) = v2;
		}
		else
		{
			const uint32_t v1 = MEM<uint32_t>(game_addr(game::unk_26CDAE8));
			const uint32_t v2 = MEM<uint32_t>(game_addr(game::unk_26CD6E0));
			MEM<uint32_t>(game_addr(PACKET_CURSOR)) = v1;
			MEM<uint32_t>(game_addr(game::unk_26C3970)) = v2;
		}
		x::Effect_UpdateTargetPosFromBones(node);
		callp(states[S8(node, 0x29)], node);
		U16(node, 0x5E) = 0;
		static constexpr game::Id queues[5] = { Q_EMITTER, Q_FLASH, Q_PARTICLE, Q_SHIELD, Q_STAGE };
		for (int i = 0; i < 5; i++)
			U16(node, 0x5E) = (uint16_t)(U16(node, 0x5E) + (uint16_t)x::ExecuteTaskQueue(game_addr(queues[i])));
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x5C) = (uint16_t)(U16(node, 0x5C) + 1);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl StartWobbleIn(uint32_t a1)
	{
		if (U8(a1, 0x28) != 0)
			return 0;
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_STAGE), game_addr(game::MAG_032_PROTECT_WobbleInTask), 0x40, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl WobbleIn(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0x100);
		if (S16(a1, 0x1C) >= 0x400)
		{
			const uint8_t st = U8(a1, 0x29);
			U8(a1, 0x26) |= 1;
			U16(a1, 0x1C) = 0x400;
			U8(a1, 0x29) = (uint8_t)(st + 1);
		}
		const uint16_t level = U16(a1, 0x1C);
		uint32_t p = game_addr(game::unk_1D98992);
		for (int i = 4; i != 0; i--, p += 0x2C)
			MEM<uint16_t>(p) = level;
		return 0;
	}

	static uint32_t __cdecl Emitter(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_032_PROTECT_SpawnShieldAndParticles);
		states[1] = game_addr(game::MAG_032_PROTECT_ReleaseMaster);
		states[2] = game_addr(game::MAG_032_PROTECT_ApplyResult);
		states[3] = game_addr(game::MAG_032_PROTECT_Emitter_State3_Empty);
		x::MAG_001_CURE_Emitter_UpdatePos(node);
		x::MAG_001_CURE_Emitter_ComputeModelBounds(node);
		callp(states[S8(node, 0x29)], node);
		if (U16(node, 0x24) == 0)
			x::BdPlaySE(P(SOUND_Protect()), 0, 0x80);
		if (U16(node, 0x24) == 0x16)
		{
			const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_FLASH), game_addr(ORIG_Flash), 0x3C, node);
			U32(t, 0x30) = game_addr(game::g_Protect_FlashScript);
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static void FlashDraw(uint32_t colour)
	{
		const uint32_t bucket = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x1C;
		uint32_t pkt = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		U32(pkt, 4) = colour;
		U32(pkt, 0) = 0x5000000;
		U8(pkt, 7) = 0x2A;
		U32(pkt, 8) = 0;
		U32(pkt, 0xC) = 0x140;
		U32(pkt, 0x10) = 0xD80000;
		U32(pkt, 0x14) = 0xD80140;
		x::SSIGPU_InsertPrimAltViewport(bucket, pkt);
		pkt += 0x18;
		const uint32_t tpage = x::sub_45C690(0, 1, 0x280, 0) & 0xFFFF;
		x::sub_45BFC0(pkt, 0, 0, tpage, 0);
		x::SSIGPU_InsertPrimAutoDepth(bucket, pkt);
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = pkt + 0xC;
	}

	static uint32_t __cdecl Flash(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint16_t idx = U16(node, 0x38);
		const uint32_t script = U32(node, 0x30);
		const uint32_t entry = U32(script, (int32_t)(int16_t)idx * 4);
		const uint16_t hi = (uint16_t)(entry >> 16);
		if ((hi >> 8) == 0xFF)
			U8(node, 0x26) |= 1;
		else
		{
			if ((hi >> 8) == 0xFE)
			{
				if (S16(node, 0x24) >= (int16_t)(hi & 0xFF))
				{
					const uint16_t next = (uint16_t)(idx + 1);
					U16(node, 0x38) = next;
					U32(node, 0x34) = U32(script, (int32_t)(int16_t)next * 4);
				}
			}
			else
			{
				U32(node, 0x34) = entry;
				U16(node, 0x38) = (uint16_t)(idx + 1);
			}
			FlashDraw(U32(node, 0x34));
		}
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t SpawnPlayer(uint32_t parent, uint32_t fn_addr, uint32_t model, uint32_t anim, uint32_t w80, uint32_t w82)
	{
		const uint32_t t = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_SHIELD), fn_addr, 0x504, parent);
		U16(t, 0x80) = (uint16_t)w80;
		U32(t, 0x74) = model;
		S32(t, 0x78) = (int16_t)anim;
		U16(t, 0x82) = (uint16_t)w82;
		return t;
	}

	static uint32_t __cdecl SpawnShieldAndParticles(uint32_t a1)
	{
		SpawnPlayer(a1, game_addr(ORIG_ShieldPlayer), game_addr(game::g_Protect_ShieldModel), 0x470, 0, 0);
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PARTICLE), game_addr(ORIG_Spawner), 0x88, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl ShieldInit(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t anim = U32(node, 0x78);
		const uint32_t entity = TargetEntity(node);
		x::Effect_DecodeModelPrimLayout(U32(node, 0x74), node + 0x94, anim);
		x::GetEffectSpawnPosition(entity, 0xF1, 0, node + 0x1C);
		if (S16(node, 0x1E) > -0x200)
			S16(node, 0x1E) = -0x200;
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static void ShieldMatrix(uint32_t node, uint32_t blk)
	{
		const uint32_t entity = TargetEntity(node);
		x::MAG_022_sub_8DD770(blk);
		S32(blk, 0x14) = S16(node, 0x1C);
		S32(blk, 0x18) = S16(node, 0x1E);
		S32(blk, 0x1C) = S16(node, 0x20);
		x::MAG_022_sub_8DD8A0(blk, (uint32_t)(int32_t)S16(entity, 0xE));
		x::ComposeAffineTransform(game_addr(game::unk_1D97778), blk, blk);
	}

	static uint32_t __cdecl RenderShield(uint32_t a1)
	{
		const uint32_t node = a1;
		alignas(4) uint8_t fr[0x5C] = {};
		const uint32_t L = P(fr);
		ShieldMatrix(node, L);
		U32(L, 0x48) = game_addr(BLEND_BUFFER);
		if (prim_play(node + 0x94, game_addr(ORIG_ShieldDraw), L, 0) == 0)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	static uint32_t __cdecl ShieldDraw(uint32_t a1, uint32_t a2, uint32_t a3)
	{
		alignas(4) uint8_t loc[0x38] = {};
		const uint32_t L = P(loc);
		const uint32_t M = L + 0x18;
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
				x::MAG_017_sub_701390(base, (uint32_t)(int32_t)f0, (uint32_t)(int32_t)f1, (uint32_t)(int32_t)t, U32(blk, 0x48));
				U32(hdr, 4) = U32(blk, 0x48);
			}
		}

		x::MAG_017_sub_701310(rec + 0x10, M);
		U16(L, 2) = U16(rec, 0xA);
		const uint32_t flags = U32(rec, 4);
		U16(L, 0) = U16(rec, 8);
		U16(L, 4) = U16(rec, 0xC);
		if ((flags & 0x800) != 0)
			x::matrixMultiplyVector(M, L, L);
		x::GTE_SetRotMatrix(blk);
		x::GTE_LoadV0(L);
		x::GTE_MVMVA_RotV0();
		x::GTE_ReadMAC123(M + 0x14);
		if ((flags & 0x8000) == 0)
			x::GTE_MatrixMultiply(blk, M);
		S32(M, 0x14) = add32(S32(M, 0x14), S32(blk, 0x14));
		S32(M, 0x18) = add32(S32(M, 0x18), S32(blk, 0x18));
		S32(M, 0x1C) = add32(S32(M, 0x1C), S32(blk, 0x1C));
		if (U32(rec, 0x18) != 0x10001000 || U16(rec, 0x1C) != 0x1000)
		{
			S32(L, 0x8) = S16(rec, 0x18);
			S32(L, 0xC) = S16(rec, 0x1A);
			S32(L, 0x10) = S16(rec, 0x1C);
			x::scale3DMatrix(M, L + 0x8);
		}
		x::GTE_SetRotMatrix_W(M);
		x::GTE_SetTransVector_W(M);
		const int32_t fade = S16(rec, 0x24);
		U32(hdr, 0x1C) = 0x2030;
		U32(hdr, 0xC) = (uint32_t)fade;
		if (fade != 0)
		{
			U32(hdr, 0x1C) = 0x20F0;
			U32(hdr, 8) = U32(rec, 0x20);
		}
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = x::Effect_RenderPrimModel(hdr, ot, 2, MEM<uint32_t>(game_addr(PACKET_CURSOR)));
		x::Field_Free(0x58);
		return 0;
	}

	static uint32_t __cdecl Spawner(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[4];
		states[0] = game_addr(game::MAG_032_PROTECT_SpawnerInit);
		states[1] = game_addr(game::MAG_032_PROTECT_SpawnerWait);
		states[2] = game_addr(game::MAG_032_PROTECT_SpawnerEmit);
		states[3] = game_addr(game::MAG_032_PROTECT_ParticleSpawner_State3_Empty);
		callp(states[S8(node, 0x29)], node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl SpawnerInit(uint32_t a1)
	{
		const uint32_t node = a1;
		const uint32_t entity = TargetEntity(node);
		x::GetEffectSpawnPosition(entity, 0xF1, 0, node + 0x1C);
		if (S16(node, 0x1E) > -0x200)
			S16(node, 0x1E) = -0x200;
		x::MAG_022_sub_8DD770(node + 0x58);
		x::MAG_022_sub_8DD8A0(node + 0x58, (uint32_t)(int32_t)S16(entity, 0xE));
		S32(node, 0x6C) = S16(node, 0x1C);
		S32(node, 0x70) = S16(node, 0x1E);
		S32(node, 0x74) = S16(node, 0x20);
		U8(node, 0x29) = (uint8_t)(U8(node, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl SpawnerWait(uint32_t a1)
	{
		if (S16(a1, 0x24) >= 0x14)
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return a1;
	}

	static uint32_t __cdecl SpawnerEmit(uint32_t a1)
	{
		const uint32_t node = a1;
		alignas(4) uint8_t loc[0x20] = {};
		const uint32_t H = P(loc);
		int32_t count = (int16_t)(uint16_t)MEM<uint8_t>(game_addr(game::g_Protect_ParticleCountTable) + S16(node, 0x24));
		if (count > 0)
		{
			const uint32_t mat = node + 0x58;
			do
			{
				const uint32_t p = x::Effect_AddTaskAndInitFromCtx(game_addr(Q_PARTICLE), game_addr(ORIG_Particle), 0x88, node);
				x::MAG_022_sub_8DD770(H);
				S32(H, 0x14) = 0;
				S32(H, 0x18) = -0x280;
				S32(H, 0x1C) = -0x280;
				const int32_t angle = (int16_t)(x::CrtRand() & 0xFFF);
				x::sub_8DD960(mat, (uint32_t)angle);
				x::ComposeAffineTransform(mat, H, H);
				U16(p, 0x1C) = U16(H, 0x14);
				U16(p, 0x1E) = U16(H, 0x18);
				U16(p, 0x20) = U16(H, 0x1C);
				x::sub_8DD7E0(H, 0x100);
				const int32_t speed = (int16_t)((x::CrtRand() & 0x1FF) + 0x10);
				U16(p, 0x78) = (uint16_t)((int32_t)S16(H, 0x4) * speed / 4096);
				U16(p, 0x7A) = (uint16_t)((int32_t)S16(H, 0xA) * speed / 4096);
				U16(p, 0x7C) = (uint16_t)((int32_t)S16(H, 0x10) * speed / 4096);
			} while (--count != 0);
		}
		if (S16(node, 0x24) > 0x18)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	static void ParticleDraw(uint32_t node)
	{
		if ((U8(node, 0x26) & 4) != 0)
			return;
		const uint32_t hdr = x::Field_Alloc(0xB4);
		TransformCameraByShadowRotation((const void *)(node + 0x1C), 0x1000, S16(node, 0x54));
		U32(hdr, 0) = U32(node, 0x4C);
		const uint32_t cursor = MEM<uint32_t>(game_addr(PACKET_CURSOR));
		const uint32_t ot = MEM<uint32_t>(game_addr(game::g_Battle_FrameRenderListBase)) + 0x44;
		U16(hdr, 4) = U16(node, 0x50);
		U16(hdr, 0x24) = 0;
		MEM<uint32_t>(game_addr(PACKET_CURSOR)) = InitEffectSequenceFromData((void *)hdr, ot, 2, cursor);
		x::Field_Free(0xB4);
	}

	static uint32_t __cdecl Particle(uint32_t a1)
	{
		const uint32_t node = a1;
		uint32_t states[3];
		states[0] = game_addr(game::MAG_032_PROTECT_ParticleInit);
		states[1] = game_addr(game::MAG_032_PROTECT_ParticleMove);
		states[2] = game_addr(game::MAG_032_PROTECT_Particle_State2_Empty);
		callp(states[S8(node, 0x29)], node);
		ParticleDraw(node);
		const uint8_t status = U8(node, 0x26);
		U16(node, 0x24) = (uint16_t)(U16(node, 0x24) + 1);
		return task_end(node, status);
	}

	static uint32_t __cdecl ParticleInit(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U32(a1, 0x4C) = game_addr(game::g_Protect_ParticleFlipbook);
		U16(a1, 0x52) = 0xB;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	static inline int32_t DivMinus5(int32_t v)
	{
		int32_t hi = (int32_t)(((int64_t)v * (int64_t)(int32_t)0x99999999) >> 32);
		hi >>= 1;
		return hi + (int32_t)((uint32_t)hi >> 31);
	}

	static void ParticleStep(uint32_t node)
	{
		const int32_t dx = DivMinus5(S16(node, 0x78));
		const uint16_t vx = (uint16_t)(U16(node, 0x78) + (uint16_t)dx);
		const int32_t dy = DivMinus5(S16(node, 0x7A));
		const uint16_t vy = (uint16_t)(U16(node, 0x7A) + (uint16_t)dy);
		U16(node, 0x1C) = (uint16_t)(U16(node, 0x1C) + vx);
		U16(node, 0x1E) = (uint16_t)(U16(node, 0x1E) + vy);
		U16(node, 0x80) = (uint16_t)dx;
		const int32_t dz = DivMinus5(S16(node, 0x7C));
		const uint16_t vz = (uint16_t)(U16(node, 0x7C) + (uint16_t)dz);
		U16(node, 0x82) = (uint16_t)dy;
		U16(node, 0x20) = (uint16_t)(U16(node, 0x20) + vz);
		U16(node, 0x84) = (uint16_t)dz;
		U16(node, 0x78) = vx;
		U16(node, 0x7A) = vy;
		U16(node, 0x7C) = vz;
	}

	static uint32_t __cdecl ParticleMove(uint32_t a1)
	{
		const uint32_t node = a1;
		ParticleStep(node);
		if (a_743C20(node) != 0)
		{
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x26) |= 1;
			U8(node, 0x29) = (uint8_t)(st + 1);
		}
		return 0;
	}

	static uint32_t __cdecl ReleaseMaster(uint32_t a1)
	{
		if (U16(a1, 0x24) == 0x12)
		{
			U8(U32(a1, 0x10), 0x63) = 0;
			U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		}
		return a1;
	}

	static uint32_t __cdecl ActionResult(uint32_t a1)
	{
		const uint32_t node = a1;
		if (S16(node, 0x24) < 0x12)
			return 0;
		const int32_t action = S8(node, 0x2A);
		const int32_t target = S8(node, 0x2B);
		const uint32_t acts = U32(U32(node, 0xC), 4);
		const uint32_t targets = U32(acts + (uint32_t)(action * 20), 8);
		x::ApplyActionResultToTarget(targets + (uint32_t)(target * 24));
		const uint8_t st = U8(node, 0x29);
		U8(node, 0x26) |= 1;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	static uint32_t __cdecl ActionLoop(uint32_t a1)
	{
		const uint32_t node = a1;
		if (U8(node, 0x63) != 0)
			return 0;
		const uint8_t idx = U8(node, 0x2A);
		if ((int16_t)(int8_t)idx < S16(node, 0x58))
		{
			const uint8_t loops = U8(node, 0x2E);
			U8(node, 0x2A) = (uint8_t)(idx + 1);
			const uint8_t st = U8(node, 0x29);
			U8(node, 0x2E) = (uint8_t)(loops + 1);
			U8(node, 0x29) = (uint8_t)(st - 1);
			return 0;
		}
		const uint8_t st = U8(node, 0x29);
		U16(node, 0x60) = 7;
		U8(node, 0x29) = (uint8_t)(st + 1);
		return 0;
	}

	static uint32_t __cdecl StartWobbleOut(uint32_t a1)
	{
		U16(a1, 0x60) = (uint16_t)(U16(a1, 0x60) - 1);
		if (S16(a1, 0x60) > 0)
			return 0;
		x::Effect_AddTaskAndInitFromCtx(game_addr(Q_STAGE), game_addr(game::MAG_032_PROTECT_WobbleOutTask), 0x40, a1);
		U8(a1, 0x29) = (uint8_t)(U8(a1, 0x29) + 1);
		return 0;
	}

	static uint32_t __cdecl WobbleOutInit(uint32_t a1)
	{
		const uint8_t st = U8(a1, 0x29);
		U16(a1, 0x1C) = 0x400;
		U8(a1, 0x29) = (uint8_t)(st + 1);
		return a1;
	}

	static uint32_t __cdecl WobbleOut(uint32_t a1)
	{
		U16(a1, 0x1C) = (uint16_t)(U16(a1, 0x1C) + 0xFF00);
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

	struct ModPort { game::Id addr; void *port; const char *name; };
	static const ModPort ENGINE_PORTS[] = {
		{ game::MAG_032_PROTECT_Master_State0_Advance, (void *)a_73A0D0, "032 MAG_032_sub_86BC50" },
		{ game::MAG_032_PROTECT_Master_State1_Advance, (void *)a_73A0D0, "032 MAG_032_sub_86BC60" },
		{ game::MAG_032_PROTECT_WobbleInTask, (void *)a_73A380, "032 MAG_032_sub_86BCA0" },
		{ game::MAG_032_PROTECT_WobbleClear, (void *)a_7335D0, "032 MAG_032_sub_86BD00" },
		{ game::MAG_032_PROTECT_WobbleIn_State2_Empty, (void *)a_73A6D0, "032 nullsub_1662" },
		{ game::MAG_032_PROTECT_SpawnEmitter, (void *)a_73A170, "032 MAG_032_sub_86BD90" },
		{ game::MAG_032_PROTECT_ShieldPlayerTask, (void *)a_73A380, "032 MAG_032_sub_86C000" },
		{ game::MAG_032_PROTECT_ShieldPlayer_State2_Empty, (void *)a_73A6D0, "032 nullsub_1665" },
		{ game::MAG_032_PROTECT_StepFlipbook, (void *)a_743C20, "032 sub_86C760" },
		{ game::MAG_032_PROTECT_Particle_State2_Empty, (void *)a_73A6D0, "032 nullsub_1666" },
		{ game::MAG_032_PROTECT_ParticleSpawner_State3_Empty, (void *)a_73A6D0, "032 nullsub_1667" },
		{ game::MAG_032_PROTECT_Emitter_State3_Empty, (void *)a_73A6D0, "032 nullsub_1663" },
		{ game::MAG_032_PROTECT_WobbleOutTask, (void *)a_73A380, "032 MAG_032_sub_86C890" },
		{ game::MAG_032_PROTECT_WobbleOut_State2_Empty, (void *)a_73A6D0, "032 nullsub_1668" },
		{ game::MAG_032_PROTECT_Master_State6_WaitQueuesEmpty, (void *)a_747550, "032 MAG_032_sub_86C960" },
		{ game::MAG_032_PROTECT_Master_State7_Advance, (void *)a_73A0D0, "032 MAG_032_sub_86C970" },
		{ game::MAG_032_PROTECT_Master_State8_Advance, (void *)a_73A0D0, "032 MAG_032_sub_86C980" },
		{ game::MAG_032_PROTECT_Master_State9_Finish, (void *)a_7475A0, "032 MAG_032_sub_86C990" },
		{ game::MAG_032_PROTECT_Master_State10_Empty, (void *)a_73A6D0, "032 nullsub_1664" },
		{ {}, nullptr, nullptr }
	};
	static const ModPort PORTS[] = {
		{ ORIG_Master, (void *)Master, "032 Protect Master" },
		{ game::MAG_032_PROTECT_StartWobbleIn, (void *)StartWobbleIn, "032 StartWobbleIn" },
		{ game::MAG_032_PROTECT_WobbleIn, (void *)WobbleIn, "032 WobbleIn" },
		{ ORIG_Emitter, (void *)Emitter, "032 Emitter" },
		{ ORIG_Flash, (void *)Flash, "032 Flash" },
		{ game::MAG_032_PROTECT_SpawnShieldAndParticles, (void *)SpawnShieldAndParticles, "032 SpawnShieldAndParticles" },
		{ game::MAG_032_PROTECT_ShieldInit, (void *)ShieldInit, "032 ShieldInit" },
		{ game::MAG_032_PROTECT_RenderShield, (void *)RenderShield, "032 RenderShield" },
		{ ORIG_ShieldDraw, (void *)ShieldDraw, "032 ShieldDraw" },
		{ ORIG_Spawner, (void *)Spawner, "032 Spawner" },
		{ game::MAG_032_PROTECT_SpawnerInit, (void *)SpawnerInit, "032 SpawnerInit" },
		{ game::MAG_032_PROTECT_SpawnerWait, (void *)SpawnerWait, "032 SpawnerWait" },
		{ game::MAG_032_PROTECT_SpawnerEmit, (void *)SpawnerEmit, "032 SpawnerEmit" },
		{ ORIG_Particle, (void *)Particle, "032 Particle" },
		{ game::MAG_032_PROTECT_ParticleInit, (void *)ParticleInit, "032 ParticleInit" },
		{ game::MAG_032_PROTECT_ParticleMove, (void *)ParticleMove, "032 ParticleMove" },
		{ game::MAG_032_PROTECT_ReleaseMaster, (void *)ReleaseMaster, "032 ReleaseMaster" },
		{ game::MAG_032_PROTECT_ApplyResult, (void *)ActionResult, "032 ActionResult" },
		{ game::MAG_032_PROTECT_ActionLoop, (void *)ActionLoop, "032 ActionLoop" },
		{ game::MAG_032_PROTECT_StartWobbleOut, (void *)StartWobbleOut, "032 StartWobbleOut" },
		{ game::MAG_032_PROTECT_WobbleOutInit, (void *)WobbleOutInit, "032 WobbleOutInit" },
		{ game::MAG_032_PROTECT_WobbleOut, (void *)WobbleOut, "032 WobbleOut" },
		{ {}, nullptr, nullptr }
	};
}
}

	void register_mag032_protect()
	{
		act::register_module(32);
		for (const act::protect::ModPort *p = act::protect::ENGINE_PORTS; p->addr; p++)
			act::register_module_port(32, p->addr, p->port, p->name);
		for (const act::protect::ModPort *p = act::protect::PORTS; p->addr; p++)
			act::register_module_port(32, p->addr, p->port, p->name);
	}
}

