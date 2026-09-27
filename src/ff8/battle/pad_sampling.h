/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

// FF8 battle pad sampling at 60 Hz, battle unchanged at 15 fps.
//
// The PC battle runs at 15 fps and ticks its UI 4 times back to back per frame (60 UI ticks
// per second, like the PlayStation), but reads the pad only once per frame: 3 of the 4 UI
// ticks see a stale pad, so presses shorter than a frame are merged or lost (GF Boost,
// Renzokuken, Zell's Duel, quick menu inputs). The PlayStation read the pad every vsync.
// Here the pad is also read at 1/4, 2/4 and 3/4 of every battle frame, and the 3 catch-up UI
// ticks of the next frame each get one of those readings, in order: every UI tick sees its
// own pad state, 60 readings per second. Nothing else changes: frame rate, battle speed,
// UI timing, and the game's own once-per-frame pad read are exactly vanilla.
//
// FF8 2000 / Steam, English 1.2 only (FF8_EN.exe addresses); install verifies the code first.

// Installs the hooks. Returns false (nothing patched) on an unsupported executable.
bool ff8_battle_pad_sampling_init();
bool ff8_battle_pad_sampling_enabled();

// Frame limiter integration (ff8_limit_fps): number of extra pad readings to spread evenly
// over the current frame wait (3 in battle, 0 elsewhere), and the reading itself. Times are
// milliseconds since the frame started (reported in FFNx.log with the GF Boost statistics).
int ff8_battle_pad_sampling_extra_reads(uint32_t driver_mode);
void ff8_battle_pad_sampling_frame_work_done(double frame_ms);
void ff8_battle_pad_sampling_read(int index, double frame_ms);
