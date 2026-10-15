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

// AddMoreAbility: support kernel.bin files whose ability sections (12-18) hold
// more than the vanilla 116 entries between them. The seven sections are one
// contiguous array of 8-byte entries to the exe, indexed by a unified ability
// id 0..115; growing any of them shifts the ids of every group behind it. The
// savemap's per-GF learned mask is 128 bits, so the array can reach 128
// entries - 12 more than vanilla - with no save format change.
//
// Any of the seven may grow, as long as the total stays within 128 and the GF
// ability group still starts at id 64 or above. Completely inert while the
// loaded kernel.bin is vanilla.
//
// Shares AddMoreMagic's kernel.bin load hook: the exe is handed a
// vanilla-layout image, and the full ability array is kept FFNx-side.
void ff8_kernel_ability_init();

// Called by the kernel.bin load hook with the freshly read file. Returns true
// when the ability sections no longer have their vanilla layout and this
// feature wants the vanilla-layout image to be built. Fills the FFNx-side
// ability table and records where each group now starts.
bool ff8_kernel_ability_read(const char *stash, const uint32_t *offsets, int size);

// True for the data sections this feature allows to have a non-vanilla size,
// so the load hook does not warn about them.
bool ff8_kernel_ability_section_may_grow(int section);

// Applies the patches, once, after a grown kernel.bin has been read.
void ff8_kernel_ability_arm();
