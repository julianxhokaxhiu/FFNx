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

#include "save_data.h"
#include <span>

class ff8_kernel_file;

// Returns false for an invalid/unsupported item extension, before the native
// kernel image is changed. changed requests the shared compatibility loader.
bool ff8_kernel_item_read(const ff8_kernel_file &file, bool &changed);
bool ff8_kernel_item_section_may_grow(int section);
bool ff8_kernel_item_active();
std::span<savemap_ff8_item> ff8_player_items();
int ff8_kernel_item_add(int id, int quantity);
