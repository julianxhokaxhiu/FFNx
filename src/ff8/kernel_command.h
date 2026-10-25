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

class ff8_kernel_file;

// Added battle commands and their effect data share the kernel loader and dispatcher.
bool ff8_kernel_command_read(const ff8_kernel_file &file);
bool ff8_kernel_command_section_may_grow(int section);
bool ff8_kernel_command_owns(uint8_t command);
int ff8_kernel_command_execute(int attacker_slot, int command, int id, int variant, int target_slot, int target_mask, int linked);

struct ff8_kernel_battle_command;
const ff8_kernel_battle_command *ff8_kernel_command_get(int command);

bool ff8_kernel_command_removed(uint8_t command);
