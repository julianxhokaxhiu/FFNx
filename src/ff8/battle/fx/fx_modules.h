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

// Memory owned by each effect, for the verifier

#pragma once

#include <stdint.h>

#include "fx_address.h"

namespace ff8fx::mod
{
	struct Region { game::Id addr; uint32_t size; const char *name; };

	struct Module
	{
		int effect_id;
		game::Id data_lo, data_hi;
		game::Id extra; uint32_t extra_size;
		bool lookahead;
		const char *name;
		const Region *streams;
		int nstreams;
		// memory found through live pointers (e.g. a model the effect writes flags into), added every tick
		void (*dyn_regions)(void (*add)(uint32_t addr, uint32_t size, const char *name)) = nullptr;
	};

	struct ExtSite { game::Id addr; int arity; const char *name; };

	extern const Module modules[];
	extern const int module_count;
	extern const Region engine_regions[];
	extern const int engine_region_count;
	extern const ExtSite ext_sites[];
	extern const int ext_site_count;

	const uint32_t DATA_MAX = 0x40000;
	const uint32_t STREAMS_MAX = 0x80000;

	const Module *find_module(int effect_id);
	uint32_t streams_bytes(const Module &m);
	inline bool fits(const Module &m)
	{
		return game_addr(m.data_hi) >= game_addr(m.data_lo) && game_addr(m.data_hi) - game_addr(m.data_lo) <= DATA_MAX && streams_bytes(m) <= STREAMS_MAX
			&& (m.extra_size <= 16);
	}

	const game::Id SSIGPU_ARENA = game::ssigpu_execution_start;
	const uint32_t SSIGPU_ARENA_SIZE = 0x60000;
	const game::Id RENDER_LIST_BASE_PTR = game::g_Battle_FrameRenderListBase;
	const uint32_t OT_SPAN_WORDS = 17 + 4386;
	const game::Id FRAME_PACKET_CUR_PTR = game::BATTLE_PACKET_WRITE_CURSOR;
	const game::Id FRAME_PACKET_END_PTR = game::BATTLE_PACKET_BUFFER_END;
	const game::Id FIELD_ALLOC_PTR = game::GLOBAL_MEMORY_POOL;
	const uint32_t FIELD_ALLOC_SCRATCH = 0x1000;
	const game::Id CRT_TLS_FN = game::getptd_560578;
	const game::Id EFFECT_ID_PTR = game::MAGIC_EFFECT_INDEX;
}
