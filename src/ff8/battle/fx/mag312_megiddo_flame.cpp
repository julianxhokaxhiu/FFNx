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

#include "fx_megiddo.h"

namespace ff8fx
{
namespace megiddo
{
	struct D312
	{
		// A copy of Megiddo Flame 166: its code, globals and data are at the same distance from 166's in every release
		static uint32_t code() { return game_addr(game::MAG_166_MegiddoFlame_Root) - game_addr(game::MAG_312_MegiddoFlame_Root); }
		static uint32_t glob() { return game_addr(game::unk_23BF038) - game_addr(game::unk_223CCA0); }
		static uint32_t data() { return game_addr(game::unk_D78048) - game_addr(game::unk_CDF658); }
		static constexpr game::Id CAMERA = game::MAG312_MEGIDDO_FLAME_MOBILE_TYPE_8_cameraAnim;
		static constexpr int32_t BONE = 0xA;
		static constexpr int32_t RMODE = 2;
		static constexpr int EFFECT = 312;
	};
}
}

namespace ff8fx
{
	void register_mag312_megiddo_flame()
	{
		static const char *const names[19] = { "312 Root", "312 Restore", "312 Director", "312 Glow", "312 Spin", "312 Ring", "312 BoltDirector",
			"312 Arc", "312 Sheet", "312 Ribbon", "312 Column", "312 ColumnFlash", "312 Pillar", "312 Flame", "312 Sparks", "312 Debris",
			"312 KnockDown", "312 Throw", "312 Ring2" };
		megiddo::Megiddo<megiddo::D312>::Register(names);
	}
}

