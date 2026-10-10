/****************************************************************************/
//    Copyright (C) 2026 Jakob Dowdle                                       //
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

// SetBlendMode hook + Ifrit CIRCLE.RSD data patch (#654).
//
// The original SetBlendMode does not write the PSX blend index to
// p_hundred->blend_mode (+0x44), which is the field common_setrenderstate
// reads. This hook propagates the index after the original runs.

#include <windows.h>
#include <cstdint>
#include <cstring>

#include "../ff7.h"
#include "../patch.h"
#include "../log.h"
#include "../common_imports.h"

#include "blend.h"

namespace ff7
{

typedef void (*SetBlendMode_t)(uint32_t update_flag, uint32_t blend_mode, struct p_hundred *state);

static SetBlendMode_t call_original_set_blend_mode = nullptr;

// SetBlendMode's prologue is PUSH EBP; MOV EBP,ESP; SUB ESP,8 — three
// instructions totalling 6 bytes. A 5-byte trampoline would split SUB ESP,8.
static constexpr size_t TRAMPOLINE_COPY_BYTES = 6;

static SetBlendMode_t build_set_blend_mode_trampoline(uint32_t target)
{
    uint8_t *tramp = static_cast<uint8_t *>(
        VirtualAlloc(nullptr, 16, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!tramp)
        return nullptr;

    std::memcpy(tramp, reinterpret_cast<void *>(target), TRAMPOLINE_COPY_BYTES);

    tramp[TRAMPOLINE_COPY_BYTES] = 0xE9;
    int32_t rel = static_cast<int32_t>(
        (target + TRAMPOLINE_COPY_BYTES) -
        (reinterpret_cast<uint32_t>(tramp) + TRAMPOLINE_COPY_BYTES + 5));
    std::memcpy(tramp + TRAMPOLINE_COPY_BYTES + 1, &rel, 4);

    return reinterpret_cast<SetBlendMode_t>(tramp);
}

static void ff7_set_blend_mode(uint32_t update_flag, uint32_t blend_mode, struct p_hundred *state)
{
    call_original_set_blend_mode(update_flag, blend_mode, state);

    // Mode 6 means "restore saved mode" — the original re-applies
    // state->blend_mode, so the field already holds the correct index.
    if (blend_mode == 6)
        return;

    if (state != nullptr)
        state->blend_mode = blend_mode;
}

// Ifrit summon's descriptor table holds CIRCLE.RSD (the ground hole) at
// offset 0 with blend mode 1 (additive). The PSX renders it with mode 2
// (subtractive). 0xb1 -> 0xb2 flips the mode while preserving flag bits.
static constexpr uint8_t IFRIT_CIRCLE_FLAGS_SUB = 0xb2;

void blend_hook_init()
{
    if (!ff7_externals.set_blend_mode)
    {
        ffnx_warning("blend_hook_init: set_blend_mode address not set for this version, skipping\n");
        return;
    }

    call_original_set_blend_mode = build_set_blend_mode_trampoline(ff7_externals.set_blend_mode);
    if (!call_original_set_blend_mode)
    {
        ffnx_error("blend_hook_init: VirtualAlloc failed, cannot build SetBlendMode trampoline\n");
        return;
    }

    replace_function(ff7_externals.set_blend_mode, ff7_set_blend_mode);

    patch_code_byte(ff7_externals.ifrit_summon_render_descriptors_8BFEE8, IFRIT_CIRCLE_FLAGS_SUB);
}

} // namespace ff7
