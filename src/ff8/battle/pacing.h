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

#pragma once

#include <stdint.h>

// Battles at 60 fps with the original battle pace (ff8_fps_limiter = 4): the battle UI and the
// pad run every frame, like on the PlayStation, while the battle itself still advances 15
// times per second. See docs/ff8/battle_60fps_ui.md
void ff8_battle_pacing_init();

// Frame limiter integration (ff8_limit_fps): called at the end of every frame
void ff8_battle_pacing_frame_end(uint32_t driver_mode);
