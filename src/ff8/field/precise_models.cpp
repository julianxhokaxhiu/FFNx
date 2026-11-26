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

// The GTE projection (RTPT) of field models rounds each vertex's camera space position to whole units before the
// divide by depth: one unit is about 0.65 game pixel in a field, so every vertex of a face lands up to a pixel away
// from the others and faces look warped. The game's projection still runs (depth, flags); only the screen positions
// it gives are replaced by the same projection without that rounding.

#include "precise_models.h"

#include "../../ff8.h"
#include "../../globals.h"
#include "../../log.h"
#include "../../patch.h"
#include "../remaster.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

// Eighths of a pixel: rounding moves a vertex by less than one pixel, anything farther means another projection
#define PRECISE_MODELS_MAX_ERROR 16
#define PRECISE_MODELS_SCREEN_MIN -8192
#define PRECISE_MODELS_SCREEN_MAX 8184

static bool precise_models_enabled = true;

static int precise_models_screen(double position)
{
    const long value = lround(position);
    return value < PRECISE_MODELS_SCREEN_MIN ? PRECISE_MODELS_SCREEN_MIN : (value > PRECISE_MODELS_SCREEN_MAX ? PRECISE_MODELS_SCREEN_MAX : int(value));
}

void ff8_field_project_model_vertices()
{
    if (ff8_remastered_edition && ff8_remaster_field_model_scaled())
    {
        field_model_vertices_scale();
        return;
    }

    ((void (*)())ff8_externals.model_vertices_scale_sub_45FE10)();

    if (!precise_models_enabled)
    {
        return;
    }

    const float *rotation = ff8_externals.flt_1CA9234;
    const int *translation = ff8_externals.dword_1CA9290;
    const int zoom = ff8_externals.camera_zoom_dword_1CA92E4[0];
    const int center_x = 8 * ff8_externals.word_1CA92DE[0], center_y = 8 * ff8_externals.word_1CA92DE[2];
    const int16_t *vertex = ff8_externals.dword_1CA8A10;
    uint32_t *screen = (uint32_t *)ff8_externals.dword_1CA8A30 + 4;
    uint32_t precise_screen[3];

    for (int i = 0; i < 3; i++, vertex += 4)
    {
        const double camera_x = translation[0] + double(rotation[0]) * vertex[2] + double(rotation[1]) * vertex[1] + double(rotation[2]) * vertex[0];
        const double camera_y = translation[1] + double(rotation[3]) * vertex[2] + double(rotation[4]) * vertex[1] + double(rotation[5]) * vertex[0];
        const double camera_z = translation[2] + double(rotation[6]) * vertex[2] + double(rotation[7]) * vertex[1] + double(rotation[8]) * vertex[0];
        // Same depth tests as the game: 16-bit depth, then at least half the zoom
        const long depth = lround(camera_z);
        double divisor = camera_z;
        if (depth < 0 || depth > 0xFFFF)
        {
            divisor = depth < 0 ? 0.0 : double(0xFFFF);
        }
        if (divisor <= 0.0 || (depth <= 0xFFFF && depth <= zoom / 2))
        {
            divisor = zoom / 2;
        }

        const int x = precise_models_screen(center_x + 8.0 * camera_x * zoom / divisor);
        const int y = precise_models_screen(center_y + 8.0 * camera_y * zoom / divisor);
        const int game_x = int16_t(screen[i] & 0xFFFF), game_y = int16_t(screen[i] >> 16);
        if (abs(x - game_x) > PRECISE_MODELS_MAX_ERROR || abs(y - game_y) > PRECISE_MODELS_MAX_ERROR)
        {
            precise_models_enabled = false;
            ffnx_warning("%s: precise and game projections disagree (%d, %d) vs (%d, %d), precise field models turned off\n", __func__, x, y, game_x, game_y);
            return;
        }
        precise_screen[i] = uint32_t(uint16_t(x)) | (uint32_t(uint16_t(y)) << 16);
    }

    for (int i = 0; i < 3; i++)
    {
        screen[i] = precise_screen[i];
    }
}

void ff8_field_precise_models_init()
{
    replace_call(ff8_externals.sub_533C30 + 0x44, (void *)ff8_field_project_model_vertices);
}
