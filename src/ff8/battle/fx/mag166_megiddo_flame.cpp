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
	struct D166
	{
		static uint32_t code() { return 0; }
		static uint32_t glob() { return 0; }
		static uint32_t data() { return 0; }
		static constexpr game::Id CAMERA = game::MAG166_MEGIDDO_FLAME_OMEGA_WEAPON_cameraAnim;
		static constexpr int32_t BONE = 0x25;
		static constexpr int32_t RMODE = 3;
		static constexpr int EFFECT = 166;
	};
}
}

namespace ff8fx
{
	void register_mag166_megiddo_flame()
	{
		static const char *const names[19] = { "166 Root", "166 Restore", "166 Director", "166 Glow", "166 Spin", "166 Ring", "166 BoltDirector",
			"166 Arc", "166 Sheet", "166 Ribbon", "166 Column", "166 ColumnFlash", "166 Pillar", "166 Flame", "166 Sparks", "166 Debris",
			"166 KnockDown", "166 Throw", "166 Ring2" };
		megiddo::Megiddo<megiddo::D166>::Register(names);
	}
}

