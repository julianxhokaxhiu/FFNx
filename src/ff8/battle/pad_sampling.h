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

// Battle pad read 60 times per second like on the PlayStation, battle unchanged at 15 fps.
// See docs/ff8/battle_input_60hz.md
void ff8_battle_pad_sampling_init();

// Frame limiter integration (ff8_limit_fps). Called at the end of every frame with the time
// the frame took so far: returns how many extra pad readings to take evenly spread over the
// frame (0 outside battle), each one taken with ff8_battle_pad_sampling_read.
int ff8_battle_pad_sampling_frame_end(uint32_t driver_mode, double frame_ms);
void ff8_battle_pad_sampling_read(int index, double frame_ms);
