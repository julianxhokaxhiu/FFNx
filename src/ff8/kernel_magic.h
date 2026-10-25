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

const char *ff8_get_battle_command_name(int command_id);

// AddMoreMagic: support kernel.bin files whose Magic section (section 2 in the wiki/editor)
// holds more than the vanilla 57 spells, unlocking spell ids 57-63 and
// 96-255 (64-95 stay reserved for GFs - 16 used today, 16 free for a future
// 32-GF exe patch). Supports EN/FR/DE/SP/IT/JP/JP_NV retail 1.2; other
// versions are skipped. Call once from ff8_init_hooks(). Completely inert
// while the loaded kernel.bin is vanilla-sized, except for the shared battle
// dispatcher hook, which forwards unchanged until an extension is active.
void ff8_kernel_magic_init();

// Existing magic-name anchor, shared with the ability extension.
uint32_t ff8_kernel_magic_name_getter();

// Existing draw-visibility anchor, shared with the command extension.
uint32_t ff8_kernel_magic_spell_visibility_fn();
