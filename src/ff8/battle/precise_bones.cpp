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

// The bone builder scales the root bone by the skeleton's model_scale (party models and weapons: 127 of 0x1000)
// and every child inherits it through 12-bit fixed point products, so each bone snaps on its own and weapons
// shake in the hand. The game's matrices stay untouched; only the GTE gets a precise copy before a bone is drawn.

#include "precise_bones.h"

#include "../../ff8.h"
#include "../../log.h"
#include "../../patch.h"
#include "../../globals.h"

#include <math.h>
#include <stdint.h>

#define PRECISE_BONES_SLOTS 24
#define PRECISE_BONES_MAX_BONES 96
// Engine rounding: a few units on a matrix element, a few units of bone origin down a chain
#define PRECISE_BONES_MAX_MATRIX_ERROR 8.0
#define PRECISE_BONES_MAX_ORIGIN_ERROR 64.0

struct precise_bone
{
    float rotation[3][3]; // local pose at true size, 0x1000 = 1
    float translation[3];
    ff8_psx_transform local;  // the game's local transform after its build
    float gte_rotation[3][3]; // camera space, 1 = 1
    int32_t gte_translation[3];
    ff8_psx_transform world; // the game's camera space transform
};

struct precise_skeleton
{
    ff8_battle_skeleton *skeleton;
    uint32_t used;
    int bone_count;
    bool valid, world;
    precise_bone bones[PRECISE_BONES_MAX_BONES];
};

static precise_skeleton precise_skeletons[PRECISE_BONES_SLOTS];
static uint32_t precise_clock = 0;
static bool precise_enabled = true;

static bool same_transform(const ff8_psx_transform &a, const ff8_psx_transform &b)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (a.matrix[i][j] != b.matrix[i][j])
            {
                return false;
            }
        }
        if (a.translation[i] != b.translation[i])
        {
            return false;
        }
    }
    return true;
}

static ff8_battle_skeleton *header_skeleton(ff8_battle_anim_header *header)
{
    return header->com_file_data ? (ff8_battle_skeleton *)header->com_file_data->skeleton : nullptr;
}

static precise_skeleton *find_skeleton(ff8_battle_skeleton *skeleton, bool create)
{
    precise_skeleton *oldest = &precise_skeletons[0];
    for (int i = 0; i < PRECISE_BONES_SLOTS; i++)
    {
        if (precise_skeletons[i].skeleton == skeleton)
        {
            return &precise_skeletons[i];
        }
        if (precise_skeletons[i].used < oldest->used)
        {
            oldest = &precise_skeletons[i];
        }
    }
    if (!create)
    {
        return nullptr;
    }
    oldest->skeleton = skeleton;
    oldest->valid = oldest->world = false;
    return oldest;
}

static void precise_bones_disable(double matrix_error, double origin_error)
{
    precise_enabled = false;
    for (int i = 0; i < PRECISE_BONES_SLOTS; i++)
    {
        precise_skeletons[i].valid = precise_skeletons[i].world = false;
    }
    ffnx_warning("%s: precise and game bones disagree (matrix %.1f, origin %.1f), precise battle bones turned off\n", __func__, matrix_error, origin_error);
}

// Bone build: the game's own build first run at full scale (32x the precision for party models), kept aside at the true size, then the normal build
void ff8_battle_precise_build_bones(ff8_battle_anim_header *header)
{
    ff8_battle_skeleton *skeleton = header_skeleton(header);
    const int scale = skeleton ? skeleton->model_scale : 0;
    const bool precise = precise_enabled && skeleton && skeleton->bone_count > 0 && skeleton->bone_count <= PRECISE_BONES_MAX_BONES && scale > 0 && scale < 0x1000;
    precise_skeleton *entry = skeleton ? find_skeleton(skeleton, precise) : nullptr;

    if (!precise || entry == nullptr)
    {
        ff8_externals.battle_build_bone_matrices_sub_508C90(header);
        if (entry)
        {
            entry->valid = entry->world = false;
        }
        return;
    }

    skeleton->model_scale = 0x1000;
    ff8_externals.battle_build_bone_matrices_sub_508C90(header);
    skeleton->model_scale = int16_t(scale);
    const float k = scale / 4096.0f;
    for (int b = 0; b < skeleton->bone_count; b++)
    {
        const ff8_psx_transform &full = skeleton->bones[b].transform;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                entry->bones[b].rotation[i][j] = full.matrix[i][j] * k;
            }
            entry->bones[b].translation[i] = full.translation[i] * k;
        }
    }

    ff8_externals.battle_build_bone_matrices_sub_508C90(header);
    for (int b = 0; b < skeleton->bone_count; b++)
    {
        entry->bones[b].local = skeleton->bones[b].transform;
    }
    entry->bone_count = skeleton->bone_count;
    entry->valid = true;
    entry->world = false;
    entry->used = ++precise_clock;
}

// Bones x camera/entity transform: the same product from the precise pose, checked against the game's result
void ff8_battle_precise_compute_bones_world(ff8_battle_anim_header *header, ff8_psx_transform *transform)
{
    ff8_battle_skeleton *skeleton = header_skeleton(header);
    precise_skeleton *entry = precise_enabled && skeleton ? find_skeleton(skeleton, false) : nullptr;
    bool ok = entry && entry->valid && entry->bone_count == skeleton->bone_count;
    for (int b = 0; ok && b < entry->bone_count; b++)
    {
        ok = same_transform(skeleton->bones[b].transform, entry->bones[b].local);
    }

    ff8_externals.battle_compute_bones_world_matrices_sub_5095B0(header, transform);

    if (entry == nullptr)
    {
        return;
    }
    entry->world = false;
    if (!ok)
    {
        return;
    }

    double worst_matrix = 0.0, worst_origin = 0.0;
    for (int b = 0; b < entry->bone_count; b++)
    {
        precise_bone &bone = entry->bones[b];
        bone.world = skeleton->bones[b].transform;
        for (int i = 0; i < 3; i++)
        {
            const double m0 = transform->matrix[i][0], m1 = transform->matrix[i][1], m2 = transform->matrix[i][2];
            for (int j = 0; j < 3; j++)
            {
                const double value = m0 * bone.rotation[0][j] + m1 * bone.rotation[1][j] + m2 * bone.rotation[2][j];
                worst_matrix = fmax(worst_matrix, fabs(value / 4096.0 - bone.world.matrix[i][j]));
                bone.gte_rotation[i][j] = float(value / (4096.0 * 4096.0));
            }
            const double origin = (m0 * bone.translation[0] + m1 * bone.translation[1] + m2 * bone.translation[2]) / 4096.0 + transform->translation[i];
            worst_origin = fmax(worst_origin, fabs(origin - bone.world.translation[i]));
            bone.gte_translation[i] = int32_t(lround(origin));
        }
    }

    if (worst_matrix > PRECISE_BONES_MAX_MATRIX_ERROR || worst_origin > PRECISE_BONES_MAX_ORIGIN_ERROR)
    {
        precise_bones_disable(worst_matrix, worst_origin);
        return;
    }
    entry->world = true;
}

// GTE load of a bone in RenderGeometry: the vertices that follow are projected with the precise transform
int ff8_battle_precise_gte_load_transform(ff8_psx_transform *transform)
{
    const int ret = ff8_externals.gte_load_transform_sub_509B30(transform);

    if (!precise_enabled)
    {
        return ret;
    }

    for (int i = 0; i < PRECISE_BONES_SLOTS; i++)
    {
        precise_skeleton &entry = precise_skeletons[i];
        if (!entry.world)
        {
            continue;
        }
        const uintptr_t first = uintptr_t(&entry.skeleton->bones[0].transform);
        const uintptr_t offset = uintptr_t(transform) - first;
        if (uintptr_t(transform) < first || offset % sizeof(ff8_battle_skeleton_bone) != 0 || offset / sizeof(ff8_battle_skeleton_bone) >= uintptr_t(entry.bone_count))
        {
            continue;
        }
        const precise_bone &bone = entry.bones[offset / sizeof(ff8_battle_skeleton_bone)];
        // Changed by the game since: keep its transform
        if (!same_transform(*transform, bone.world))
        {
            return ret;
        }
        // GTE float rotation mirror, each row stored from the last column to the first
        for (int row = 0; row < 3; row++)
        {
            for (int col = 0; col < 3; col++)
            {
                ff8_externals.flt_1CA9234[3 * row + 2 - col] = bone.gte_rotation[row][col];
            }
            ff8_externals.dword_1CA9290[row] = bone.gte_translation[row];
        }
        return ret;
    }
    return ret;
}

void ff8_battle_precise_bones_init()
{
    replace_call(ff8_externals.battle_rebuild_entity_bones_sub_508F60 + 0x9, (void *)ff8_battle_precise_build_bones);
    replace_call(ff8_externals.battle_rebuild_entity_bones_sub_508F60 + 0x1A, (void *)ff8_battle_precise_build_bones);
    replace_call(ff8_externals.battle_read_animation_sub_508F90 + 0x2F3, (void *)ff8_battle_precise_build_bones);
    replace_call(ff8_externals.battle_render_entity_sub_502D40 + 0xB2, (void *)ff8_battle_precise_compute_bones_world);
    replace_call(ff8_externals.battle_render_entity_sub_502D40 + 0x149, (void *)ff8_battle_precise_compute_bones_world);
    replace_call(ff8_externals.sub_5099D0 + 0xBD, (void *)ff8_battle_precise_gte_load_transform);
}
