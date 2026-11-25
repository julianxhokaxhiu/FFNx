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

// Battle models drawn from full precision bone matrices.
//
// The battle bone builder (sub_508C90) scales the root bone rotation by the skeleton's
// model_scale (party models and their weapons: 127 out of 4096), and every child bone
// inherits it through 12-bit fixed point matrix products. Each bone matrix then keeps
// about 7 bits of precision and each bone origin is rounded to about 4096 / model_scale
// (~32) raw units. As a pose moves slowly every bone snaps on its own, so a thin weapon
// held in a hand shivers against it by a pixel or more (for eg. Selphie's nunchaku seen
// from behind). This happens at the vanilla 15 fps too.
//
// The engine's own matrices are never changed (effects, shadows, attach points and targets
// read them). Only the vertices drawn by RenderGeometry (sub_5099D0) use a precise copy:
// - sub_508C90 (bone build): the same engine build is run first with model_scale forced
//   to 4096 (32x the precision), kept aside scaled back to the true size, then the normal
//   build runs.
// - sub_5095B0 (bones x camera/entity matrix, in place): the same composition is done in
//   doubles from the precise copy and checked against the engine's integer result. If they
//   disagree beyond rounding, the feature turns itself off for the session.
// - sub_509B30 (GTE load of a bone matrix, called from RenderGeometry): notes which bone is
//   current.
// - sub_50F900 (ParseVertices): the vertices of a recognised bone are projected from the
//   precise matrix and translation, with the engine's own projection, near clip, clip flags
//   and depth. Anything not recognised (a matrix the engine changed since, a model with
//   model_scale >= 4096, the stage) goes through the engine's own code.

#include "precise_bones.h"

#include "../../ff8.h"
#include "../../cfg.h"
#include "../../log.h"
#include "../../patch.h"
#include "../../globals.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PRECISE_BONES_SLOTS 48
#define PRECISE_BONES_MAX_BONES 96

// Skeleton header: bone count (u8) at +0, model_scale (s16) at +2, bones from +16.
#define SKELETON_BONES_OFFSET 16
#define SKELETON_BONE_SIZE 48
// Bone: rotation matrix (9 x s16) at +0x10, translation (3 x s32) at +0x24.
#define BONE_MATRIX_OFFSET 0x10
#define BONE_TRANSLATION_OFFSET 0x24
// Camera/entity matrix passed to sub_5095B0: 9 x s16, translation (3 x s32) at +0x14.
#define WORLD_MATRIX_TRANSLATION_OFFSET 0x14

// Engine rounding: a few units on a matrix element, a few units of bone origin down a chain.
#define MAX_MATRIX_ERROR 8.0
#define MAX_ORIGIN_ERROR 64.0

struct precise_skeleton
{
    uint8_t *skeleton;
    uint32_t used;
    int bone_count;
    bool valid, world;
    double rotation[PRECISE_BONES_MAX_BONES][9], translation[PRECISE_BONES_MAX_BONES][3];             // precise local pose (true size, 4096 = 1)
    int32_t local_ints[PRECISE_BONES_MAX_BONES][12];                                                  // the engine's local ints after its build
    double world_rotation[PRECISE_BONES_MAX_BONES][9], world_translation[PRECISE_BONES_MAX_BONES][3]; // precise world (camera space)
    int32_t world_ints[PRECISE_BONES_MAX_BONES][12];                                                  // the engine's world ints
};

static precise_skeleton precise_skeletons[PRECISE_BONES_SLOTS];
static uint32_t precise_clock = 0;
static bool precise_enabled = false;
static uint32_t build_replace_id = 0, world_replace_id = 0, parse_replace_id = 0;

static const double *current_rotation = nullptr, *current_translation = nullptr;
static const int32_t *current_ints = nullptr;

static uint8_t *anim_header_skeleton(void *anim_header)
{
    uint8_t *com_file_data = *(uint8_t **)((uint8_t *)anim_header + 4);
    return com_file_data ? *(uint8_t **)com_file_data : nullptr;
}

static inline uint8_t *skeleton_bone(uint8_t *skeleton, int bone)
{
    return skeleton + SKELETON_BONES_OFFSET + SKELETON_BONE_SIZE * bone;
}

static void read_bone_ints(uint8_t *skeleton, int bone, int32_t out[12])
{
    const uint8_t *b = skeleton_bone(skeleton, bone);
    const int16_t *m = (const int16_t *)(b + BONE_MATRIX_OFFSET);
    const int32_t *v = (const int32_t *)(b + BONE_TRANSLATION_OFFSET);
    for (int i = 0; i < 9; i++)
        out[i] = m[i];
    for (int i = 0; i < 3; i++)
        out[9 + i] = v[i];
}

static precise_skeleton *find_skeleton(uint8_t *skeleton, bool create)
{
    precise_skeleton *oldest = &precise_skeletons[0];
    for (int i = 0; i < PRECISE_BONES_SLOTS; i++)
    {
        if (precise_skeletons[i].skeleton == skeleton)
            return &precise_skeletons[i];
        if (precise_skeletons[i].used < oldest->used)
            oldest = &precise_skeletons[i];
    }
    if (!create)
        return nullptr;
    oldest->skeleton = skeleton;
    oldest->valid = oldest->world = false;
    return oldest;
}

static void precise_bones_disable(const char *why)
{
    if (!precise_enabled)
        return;
    precise_enabled = false;
    for (int i = 0; i < PRECISE_BONES_SLOTS; i++)
        precise_skeletons[i].valid = precise_skeletons[i].world = false;
    current_rotation = current_translation = nullptr;
    current_ints = nullptr;
    ffnx_warning("%s: battle precise bones turned off (%s)\n", __func__, why);
}

static void call_build_bones(void *anim_header)
{
    ((void(__cdecl *)(void *))ff8_externals.battle_build_bone_matrices_sub_508C90)(anim_header);
}

void __cdecl ff8_battle_precise_build_bones(void *anim_header)
{
    uint8_t *skeleton = anim_header_skeleton(anim_header);
    const int bone_count = skeleton ? skeleton[0] : 0;
    int16_t *model_scale = skeleton ? (int16_t *)(skeleton + 2) : nullptr;
    const int scale = model_scale ? *model_scale : 0;
    const bool precise = precise_enabled && bone_count > 0 && bone_count <= PRECISE_BONES_MAX_BONES && scale > 0 && scale < 4096;
    precise_skeleton *entry = skeleton ? find_skeleton(skeleton, precise) : nullptr;

    unreplace_function(build_replace_id);
    if (entry && precise)
    {
        // The engine's own build at full precision, scaled back to the true size
        *model_scale = 4096;
        call_build_bones(anim_header);
        *model_scale = (int16_t)scale;
        const double k = scale / 4096.0;
        for (int b = 0; b < bone_count; b++)
        {
            int32_t big[12];
            read_bone_ints(skeleton, b, big);
            for (int i = 0; i < 9; i++)
                entry->rotation[b][i] = big[i] * k;
            for (int i = 0; i < 3; i++)
                entry->translation[b][i] = big[9 + i] * k;
        }
        // Then the normal build, the engine's matrices stay exactly as vanilla
        call_build_bones(anim_header);
        for (int b = 0; b < bone_count; b++)
            read_bone_ints(skeleton, b, entry->local_ints[b]);
        entry->bone_count = bone_count;
        entry->valid = true;
        entry->world = false;
        entry->used = ++precise_clock;
    }
    else
    {
        call_build_bones(anim_header);
        if (entry)
            entry->valid = entry->world = false;
    }
    rereplace_function(build_replace_id);
}

void __cdecl ff8_battle_precise_compute_bones_world(void *anim_header, void *matrix)
{
    uint8_t *skeleton = anim_header_skeleton(anim_header);
    precise_skeleton *entry = (precise_enabled && skeleton) ? find_skeleton(skeleton, false) : nullptr;
    bool ok = entry && entry->valid && entry->bone_count == skeleton[0];
    // The local pose must be the one the precise copy was built with
    for (int b = 0; ok && b < entry->bone_count; b++)
    {
        int32_t cur[12];
        read_bone_ints(skeleton, b, cur);
        ok = memcmp(cur, entry->local_ints[b], sizeof(cur)) == 0;
    }

    unreplace_function(world_replace_id);
    ((void(__cdecl *)(void *, void *))ff8_externals.battle_compute_bones_world_matrices_sub_5095B0)(anim_header, matrix);
    rereplace_function(world_replace_id);

    if (!entry)
        return;
    entry->world = false;
    if (!ok)
        return;

    const int16_t *m = (const int16_t *)matrix;
    const int32_t *mt = (const int32_t *)((const uint8_t *)matrix + WORLD_MATRIX_TRANSLATION_OFFSET);
    double worst_rotation = 0.0, worst_translation = 0.0;
    for (int b = 0; b < entry->bone_count; b++)
    {
        const double *r = entry->rotation[b], *t = entry->translation[b];
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
                entry->world_rotation[b][3 * i + j] = (m[3 * i] * r[j] + m[3 * i + 1] * r[3 + j] + m[3 * i + 2] * r[6 + j]) / 4096.0;
            entry->world_translation[b][i] = (m[3 * i] * t[0] + m[3 * i + 1] * t[1] + m[3 * i + 2] * t[2]) / 4096.0 + mt[i];
        }
        read_bone_ints(skeleton, b, entry->world_ints[b]);
        for (int i = 0; i < 9; i++)
            worst_rotation = fmax(worst_rotation, fabs(entry->world_rotation[b][i] - entry->world_ints[b][i]));
        for (int i = 0; i < 3; i++)
            worst_translation = fmax(worst_translation, fabs(entry->world_translation[b][i] - entry->world_ints[b][9 + i]));
    }

    if (worst_rotation > MAX_MATRIX_ERROR || worst_translation > MAX_ORIGIN_ERROR)
    {
        char why[128];
        snprintf(why, sizeof(why), "precise and engine bones disagree: matrix %.1f, origin %.1f", worst_rotation, worst_translation);
        precise_bones_disable(why);
        return;
    }
    entry->world = true;
}

int __cdecl ff8_battle_precise_gte_load_transform(void *transform)
{
    const int ret = ((int(__cdecl *)(void *))ff8_externals.gte_load_transform_sub_509B30)(transform);

    current_rotation = current_translation = nullptr;
    current_ints = nullptr;
    if (!precise_enabled)
        return ret;

    uint8_t *p = (uint8_t *)transform;
    for (int i = 0; i < PRECISE_BONES_SLOTS; i++)
    {
        precise_skeleton *entry = &precise_skeletons[i];
        if (!entry->world || !entry->skeleton)
            continue;
        uint8_t *first = entry->skeleton + SKELETON_BONES_OFFSET + BONE_MATRIX_OFFSET;
        if (p < first || p >= first + SKELETON_BONE_SIZE * entry->bone_count || (p - first) % SKELETON_BONE_SIZE)
            continue;
        const int b = (int)((p - first) / SKELETON_BONE_SIZE);
        int32_t cur[12];
        read_bone_ints(entry->skeleton, b, cur);
        // The engine changed that bone since: draw it the engine's way
        if (memcmp(cur, entry->world_ints[b], sizeof(cur)) != 0)
            return ret;
        current_rotation = entry->world_rotation[b];
        current_translation = entry->world_translation[b];
        current_ints = entry->world_ints[b];
        return ret;
    }
    return ret;
}

// GTE float rotation mirror (matrix / 4096), stored column-reversed per row: flt_1CA9234[3 * row + 2 - col]
static inline float gte_rotation(int row, int col)
{
    return ff8_externals.flt_1CA9234[3 * row + 2 - col];
}

int __cdecl ff8_battle_precise_parse_vertices(uint8_t *context, int16_t **vertex_stream, uint32_t **output_cursor)
{
    const int32_t *wi = current_ints;
    bool mine = precise_enabled && wi != nullptr;
    // The GTE must still hold that bone's matrix (nothing reloaded it since)
    for (int i = 0; mine && i < 3; i++)
        mine = ff8_externals.dword_1CA9290[i] == wi[9 + i];
    for (int row = 0; mine && row < 3; row++)
        for (int col = 0; mine && col < 3; col++)
            mine = fabsf(gte_rotation(row, col) - (float)(wi[3 * row + col] / 4096.0)) < 1e-4f;

    if (!mine)
    {
        unreplace_function(parse_replace_id);
        const int ret = ((int(__cdecl *)(void *, void *, void *))ff8_externals.world_calc_zoom_sub_50F900)(context, vertex_stream, output_cursor);
        rereplace_function(parse_replace_id);
        return ret;
    }

    const double *R = current_rotation, *T = current_translation;
    int16_t *v = *vertex_stream;
    uint32_t *out = *output_cursor;
    const int n = *v;
    if (n == 0)
        return 0;
    v++;

    // Clip rectangle from the render context, in 1/8 subpixels
    const int x0 = 8 * *(const int16_t *)(context + 0x14), y0 = 8 * *(const int16_t *)(context + 0x16);
    const int x1 = x0 + 8 * *(const int16_t *)(context + 0x18), y1 = y0 + 8 * *(const int16_t *)(context + 0x1A);
    const int h = ff8_externals.camera_zoom_dword_1CA92E4[0], half = h >> 1;
    const int ofx = 8 * ff8_externals.word_1CA92DE[0], ofy = 8 * ff8_externals.word_1CA92DE[2];

    for (int i = 0; i < n; i++, v += 3)
    {
        const double x = v[0], y = v[1], z = v[2];
        const double cx8 = 8.0 * ((R[0] * x + R[1] * y + R[2] * z) / 4096.0 + T[0]);
        const double cy8 = 8.0 * ((R[3] * x + R[4] * y + R[5] * z) / 4096.0 + T[1]);
        const double cz = (R[6] * x + R[7] * y + R[8] * z) / 4096.0 + T[2];
        const int zr = (int)lrint(cz);
        uint32_t flags = 0;
        int sx, sy;
        if (zr > half)
        {
            const double inv = h / cz;
            sx = (int)lrint(inv * cx8) + ofx;
            sy = (int)lrint(inv * cy8) + ofy;
        }
        else
        {
            // Near clip, same as the engine
            flags = 0x20000;
            sx = ofx + 2 * (int)lrint(cx8);
            sy = ofy + 2 * (int)lrint(cy8);
        }
        if (x0 || y0)
        {
            if (sx < x0)
                flags |= 0x1100000;
            else if (sx > x1)
                flags |= 0x1200000;
            if (sy < y0)
                flags |= 0x2400000;
            else if (sy > y1)
                flags |= 0x2800000;
        }
        else
        {
            if ((uint32_t)sx > (uint32_t)x1)
                flags |= 0x1000000;
            if ((uint32_t)sy > (uint32_t)y1)
                flags |= 0x2000000;
        }
        out[0] = ((uint32_t)sx & 0xFFFF) | ((uint32_t)sy << 16);
        out[1] = flags | ((uint32_t)zr & 0xFFFF);
        out += 2;
    }
    *vertex_stream = v;
    *output_cursor = out;
    return n;
}

void ff8_battle_precise_bones_init()
{
    if (!ff8_battle_precise_bones)
        return;

    // The call chains below were only checked on the English 1.2 / Steam 2013 executable
    if (!FF8_US_VERSION)
    {
        ffnx_info("%s: battle precise bones is only available on the English version of the game\n", __func__);
        return;
    }

    // Sanity checks: the second call site of each function must agree with the first one
    const bool layout_ok = get_relative_call(ff8_externals.battle_render_entity_sub_502D40, 0x11B) == ff8_externals.sub_5099D0                                        // RenderGeometry (body)
                           && get_relative_call(ff8_externals.battle_render_entity_sub_502D40, 0x149) == ff8_externals.battle_compute_bones_world_matrices_sub_5095B0 // weapon bones
                           && get_relative_call(ff8_externals.battle_rebuild_entity_bones_sub_508F60, 0x1A) == ff8_externals.battle_build_bone_matrices_sub_508C90;   // weapon build
    if (!layout_ok)
    {
        ffnx_warning("%s: unexpected battle code layout, battle precise bones disabled\n", __func__);
        return;
    }

    precise_enabled = true;
    build_replace_id = replace_function(ff8_externals.battle_build_bone_matrices_sub_508C90, (void *)ff8_battle_precise_build_bones);
    world_replace_id = replace_function(ff8_externals.battle_compute_bones_world_matrices_sub_5095B0, (void *)ff8_battle_precise_compute_bones_world);
    // Only the bone loads of RenderGeometry, right before its ParseVertices call
    replace_call(ff8_externals.sub_5099D0 + 0xBD, (void *)ff8_battle_precise_gte_load_transform);
    // A function replace, not a call replace: the widescreen hook of the ParseVertices call
    // in RenderGeometry calls the original function, so both compose
    parse_replace_id = replace_function(uint32_t(ff8_externals.world_calc_zoom_sub_50F900), (void *)ff8_battle_precise_parse_vertices);

    ffnx_info("%s: battle models drawn from full precision bone matrices\n", __func__);
}
