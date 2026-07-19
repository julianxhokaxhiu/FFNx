/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                    //
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

// AddMoreMagic: support kernel.bin files whose Magic section (section 1)
// holds more than the vanilla 57 spells, unlocking spell ids 57-63 and
// 96-255 (64-95 stay reserved for GFs - 16 used today, 16 free for a future
// 32-GF exe patch). Call once from ff8_init_hooks(). Completely inert while
// the loaded kernel.bin is vanilla-sized.
void ff8_kernel_magic_init();
