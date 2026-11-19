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

#include "mag_common.h"

namespace ff8fx
{
namespace griever344
{
	using namespace eng;
	using namespace magc;

	inline CastContext *&Ctx() { return var<CastContext *>(game_addr(game::MAG_344_Griever368_CastContext)); }
	inline uint32_t &CasterSlot() { return var<uint32_t>(game_addr(game::MAG_344_Griever368_CasterSlot)); }
	inline uint8_t &FanfareDone() { return var<uint8_t>(game_addr(game::MAG_344_Griever368_FanfareDone)); }
	inline uint32_t &PacketCursor() { return var<uint32_t>(game_addr(game::MAG_344_Griever368_PacketCursor)); }
	inline TaskQueue *QCue() { return (TaskQueue *)game_addr(game::MAG_344_Griever368_CueQueue); }

	static constexpr game::Id ORIG_RootTask = game::MAG_344_Griever368_RootTask;
	static constexpr game::Id ORIG_CueTask = game::MAG_344_Griever368_CueTask;
	static constexpr game::Id ARENA_A = game::unk_24E8BC8;
	static constexpr game::Id ARENA_B = game::MAG_344_Griever368_PacketArenas;
	static constexpr game::Id FANFARE_DATA = game::unk_DEE388;
	static constexpr game::Id CAMERA_ANIM = game::MAG344_FINAL_BATTLE_MUSIC_cameraAnim;

	inline void Music_SetVolumeTrans(uint32_t a1, uint32_t a2, uint32_t a3) { fn<void (__cdecl *)(uint32_t, uint32_t, uint32_t)>(game_addr(game::Music_SetVolumeTrans))(a1, a2, a3); }
	inline void QueueChainTransformation(uint8_t *entity, uint32_t chain) { fn<void (__cdecl *)(uint8_t *, uint32_t)>(game_addr(game::QueueChainTransformation))(entity, chain); }
	inline void BS_R0win_QueueVictoryFanfare(uint32_t data, uint8_t *done) { fn<void (__cdecl *)(uint32_t, uint8_t *)>(game_addr(game::BS_R0win_QueueVictoryFanfare))(data, done); }
	inline void BS_Music_SetChannelVolumeCommand(uint32_t a1, uint32_t a2) { fn<void (__cdecl *)(uint32_t, uint32_t)>(game_addr(game::BS_Music_SetChannelVolumeCommand))(a1, a2); }

#pragma pack(push, 1)
	struct RootNode
	{
		TaskNode hdr;
		int16_t counter;
		uint16_t pad0E;
	};
	struct CueNode
	{
		TaskNode hdr;
		int16_t counter;
		uint8_t pad0E[0x16];
	};
#pragma pack(pop)
	static_assert(sizeof(RootNode) == 0x10 && sizeof(CueNode) == 0x24, "344 nodes");
}
}

namespace ff8fx
{
namespace griever344
{
	static uint32_t __cdecl CueTask(TaskNode *n)
	{
		CueNode *c = (CueNode *)n;
		if (UpdateFlags() & 0x201) return 0;
		if (c->counter == 0) Music_SetVolumeTrans(0, 0x78, 0);
		if (c->counter == 0x1E) BS_R0win_QueueVictoryFanfare(game_addr(FANFARE_DATA), &FanfareDone());
		if (FanfareDone() == 0 && c->counter == 0x1F) return 0;
		if (c->counter == 0x20)
		{
			fn<void (__cdecl *)(const void *)>(game_addr(game::Battle_PlayCameraAnimation))((const void *)game_addr(CAMERA_ANIM));
			QueueChainTransformation(Entity(CasterSlot()), 0x14);
		}
		if (c->counter == 0x22)
		{
			BS_Music_SetChannelVolumeCommand(0, 0x7F);
			if (c->counter == 0x22) ApplyActionResultToTarget(Ctx()->actions->targets);
		}
		c->counter++;
		return c->counter > 0x23 ? TASK_END : 0;
	}

	static uint32_t __cdecl RootTask(TaskNode *n)
	{
		RootNode *r = (RootNode *)n;
		PacketCursor() = game_addr(ARENA_A);
		if (!(r->counter & 1)) PacketCursor() = game_addr(ARENA_B);
		const int left = ExecuteTaskQueue(QCue());
		r->counter++;
		return left ? 0 : TASK_END;
	}
}

	void register_mag344_griever_368()
	{
		register_port(griever344::ORIG_RootTask, (void *)griever344::RootTask, "344 RootTask", 344);
		register_port(griever344::ORIG_CueTask, (void *)griever344::CueTask, "344 CueTask", 344);
	}
}

