// Copyright (C) 2026 HobbitDur
// SPDX-License-Identifier: GPL-3.0-or-later

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
