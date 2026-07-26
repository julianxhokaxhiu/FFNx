/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 myst6re                                            //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Tang-Tang Zhou                                     //
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

#include "../globals.h"
#include "../log.h"
#include "../patch.h"
#include "remaster.h"
#include "vram.h"

Zzz g_FF8ZzzArchiveMain;
Zzz g_FF8ZzzArchiveOther;
uint8_t *extended_memory = nullptr;

struct ff8_remastered_model_divisor {
    char modelId[5];
    uint16_t divisor;
};

std::unordered_map<uint32_t, CharaOneModel> field_model_map;
double next_field_model_divisor = 0.0;
constexpr int model_divisors_size = 142;
ff8_remastered_model_divisor model_divisors[model_divisors_size] = {
    {"d000", 104},
    {"d001", 75},
    {"d002", 75},
    {"d003", 75},
    {"d005", 75},
    {"d006", 75},
    {"d007", 75},
    {"d009", 75},
    {"d010", 75},
    {"d011", 75},
    {"d012", 75},
    {"d014", 75},
    {"d015", 74},
    {"d016", 74},
    {"d017", 74},
    {"d018", 100},
    {"d019", 100},
    {"d020", 100},
    {"d021", 100},
    {"d022", 104},
    {"d023", 104},
    {"d024", 104},
    {"d025", 104},
    {"d026", 104},
    {"d027", 104},
    {"d028", 104},
    {"d029", 104},
    {"d030", 104},
    {"d032", 70},
    {"d033", 70},
    {"d034", 70},
    {"d037", 70},
    {"d040", 74},
    {"d041", 74},
    {"d043", 75},
    {"d044", 75},
    {"d045", 80},
    {"d046", 80},
    {"d047", 47},
    {"d048", 47},
    {"d049", 75},
    {"d050", 100},
    {"d051", 104},
    {"d052", 104},
    {"d054", 500},
    {"d055", 500},
    {"d056", 500},
    {"d057", 500},
    {"d058", 500},
    {"d059", 840},
    {"d060", 150},
    {"d061", 150},
    {"d062", 150},
    {"d065", 70},
    {"d066", 150},
    {"d067", 150},
    {"d068", 150},
    {"d069", 150},
    {"d070", 150},
    {"d071", 75},
    {"d072", 80},
    {"d073", 47},
    {"d074", 74},
    {"d075", 104},
    {"n002", 19},
    {"n010", 55},
    {"n029", 7},
    {"o028", 300},
    {"o029", 300},
    {"p001", 150},
    {"p002", 150},
    {"p004", 150},
    {"p005", 150},
    {"p006", 150},
    {"p007", 150},
    {"p008", 150},
    {"p010", 150},
    {"p011", 150},
    {"p012", 6},
    {"p013", 150},
    {"p015", 150},
    {"p017", 150},
    {"p018", 62},
    {"p021", 150},
    {"p022", 150},
    {"p024", 150},
    {"p025", 150},
    {"p028", 150},
    {"p030", 150},
    {"p031", 150},
    {"p034", 150},
    {"p037", 150},
    {"p042", 150},
    {"p043", 150},
    {"p044", 150},
    {"p045", 150},
    {"p047", 150},
    {"p048", 300},
    {"p049", 150},
    {"p056", 150},
    {"p058", 300},
    {"p061", 150},
    {"p063", 150},
    {"p064", 150},
    {"p070", 150},
    {"p074", 150},
    {"p078", 150},
    {"p086", 150},
    {"p087", 150},
    {"p090", 150},
    {"p094", 150},
    {"p101", 150},
    {"p105", 150},
    {"p106", 150},
    {"p107", 150},
    {"p119", 150},
    {"p120", 150},
    {"p121", 150},
    {"p122", 500},
    {"p123", 32},
    {"p124", 150},
    {"p125", 75},
    {"p133", 150},
    {"p137", 67},
    {"p138", 150},
    {"p142", 45},
    {"p143", 150},
    {"p144", 150},
    {"p147", 150},
    {"p161", 150},
    {"p162", 150},
    {"p163", 150},
    {"p164", 150},
    {"p168", 150},
    {"p169", 150},
    {"p170", 150},
    {"p171", 150},
    {"p172", 150},
    {"p173", 45},
    {"p180", 150},
    {"p183", 150}
};

void field_model_vertices_scale()
{
    double unzoom_model = next_field_model_divisor;

    if (unzoom_model == 1.0) {
        return ((void(*)())ff8_externals.model_vertices_scale_sub_45FE10)();
    }

    int flags = 0;
    int v1 = ff8_externals.camera_zoom_dword_1CA92E4[0] >> 1;
    ff8_externals.dword_1CA8A50[0] = ff8_externals.dword_1CA8A50[3];
    double v30 = (double)ff8_externals.camera_zoom_dword_1CA92E4[0]; // Float in the original implementation
    int *v2 = ff8_externals.dword_1CA8A50 + 1;
    uint32_t *v22 = (uint32_t *)ff8_externals.dword_1CA8A30 + 4;
    uint32_t v31 = 8 * ff8_externals.word_1CA92DE[0];
    uint32_t v32 = 8 * ff8_externals.word_1CA92DE[2];
    int32_t x_related;
    int32_t y_related;
    uint32_t z_related;
    double v11;
    int16_t *v3 = ff8_externals.dword_1CA8A10;

    for (int i = 0; i < 3; ++i) {
        double v4 = double(v3[0]), v6 = double(v3[1]), v7 = double(v3[2]);
        v3 += 4;

        x_related = ff8_externals.dword_1CA9290[0] + round(double(ff8_externals.flt_1CA9234[0] * v7 + ff8_externals.flt_1CA9234[1] * v6 + ff8_externals.flt_1CA9234[2] * v4) / unzoom_model);
        y_related = ff8_externals.dword_1CA9290[1] + round(double(ff8_externals.flt_1CA9234[3] * v7 + ff8_externals.flt_1CA9234[4] * v6 + ff8_externals.flt_1CA9234[5] * v4) / unzoom_model);
        z_related = ff8_externals.dword_1CA9290[2] + round(double(ff8_externals.flt_1CA9234[6] * v7 + ff8_externals.flt_1CA9234[7] * v6 + ff8_externals.flt_1CA9234[8] * v4) / unzoom_model);

        uint32_t z_related2 = z_related;
        if (z_related2 > 0xFFFF) {
            z_related2 = 0xFFFF;
            flags |= 0x80040000;
        }
        *v2 = uint16_t(z_related2);
        if (*v2 <= v1) {
            if (!*v2) {
                flags |= 0x80040000;
            }
            z_related2 = v1;
            flags |= 0x80020000;
        }
        v11 = v30 / z_related2;
        double v20 = round(8.0 * x_related * v11);
        int v12 = v31 + int(v20);
        if (v12 < -8192) {
            v12 = -8192;
            flags |= 0x80004000;
        } else if (v12 > 8184) {
            v12 = 8184;
            flags |= 0x80004000;
        }

        double v21 = round(8.0 * y_related * v11);
        int v14 = v32 + int(v21);
        if (v14 < -8192) {
            v14 = -8192;
            flags |= 0x80002000;
        } else if (v14 > 8184) {
            v14 = 8184;
            flags |= 0x80002000;
        }

        *v22 = (uint32_t(uint16_t(v14)) << 16) | uint16_t(v12);
        ++v22;
        ++v2;
    }

    double v16 = v11 * (double)*(int16_t *)(ff8_externals.camera_zoom_dword_1CA92E4 + 2) * 65536.0 + (double)*(int32_t *)(ff8_externals.camera_zoom_dword_1CA92E4 + 8);
    double v33 = v16 * 0.00024414062;
    int v17 = int(round(v33));
    if (v17 > 0xFFF) {
        v17 = 0xFFF;
        flags |= 0x1000u;
    }
    ff8_externals.calc_model_poly_condition_result_dword_1CA8A70[0] = int(round(v16));
    ff8_externals.dword_1CA8A30[0] = v17;
    ff8_externals.calc_model_poly_condition_result_dword_1CA8A70[1] = x_related;
    if (x_related >= -32768) {
        if (x_related <= 0x7FFF) {
            ff8_externals.dword_1CA8A30[1] = x_related;
        } else {
            flags |= 0x81000000;
            ff8_externals.dword_1CA8A30[1] = 0x7FFF;
        }
    } else {
        flags |= 0x81000000;
        ff8_externals.dword_1CA8A30[1] = -32768;
    }
    ff8_externals.calc_model_poly_condition_result_dword_1CA8A70[2] = y_related;
    if (y_related >= -32768) {
        if (y_related <= 0x7FFF) {
            ff8_externals.dword_1CA8A30[2] = y_related;
        } else {
            flags |= 0x80800000;
            ff8_externals.dword_1CA8A30[2] = 0x7FFF;
        }
    } else {
        flags |= 0x80800000;
        ff8_externals.dword_1CA8A30[2] = -32768;
    }
    ff8_externals.calc_model_poly_condition_result_dword_1CA8A70[3] = z_related;
    if (z_related >= -32768) {
        if (z_related <= 0x7FFF) {
            ff8_externals.dword_1CA8A30[3] = z_related;
        } else {
            ff8_externals.dword_1CA8A30[3] = 0x7FFF;
            flags |= 0x400000;
        }
    } else {
        ff8_externals.dword_1CA8A30[3] = -32768;
        flags |= 0x400000;
    }

    *ff8_externals.dword_1CA92F8 = flags;
}

double get_model_divisor(uint32_t addr)
{
    auto it = field_model_map.find(addr);
    if (it == field_model_map.end()) {
        return 1.0;
    }

    for (int i = 0; i < model_divisors_size; ++i) {
        char *modelId = model_divisors[i].modelId;

        if (*(uint32_t *)modelId == *(uint32_t *)it->second.name) {
            return double(model_divisors[i].divisor);
        }
    }

    return 1.0;
}

void field_sub_530C30(int a1, int a2, int **a3)
{
    next_field_model_divisor = get_model_divisor(uint32_t(*a3));

    ((void(*)(int,int,int**))ff8_externals.sub_530C30)(a1, a2, a3);

    next_field_model_divisor = 1.0;
}

int field_open_chara_one(
    int current_data_pointer,
    void *models_infos,
    void *palette_vram_positions,
    void *texture_vram_positions,
    const char *dir_name,
    int field_data_pointer,
    int allocated_size,
    int pcb_data_size
) {
    int ret = ((int(*)(int,void*,void*,void*,const char*,int,int,int))ff8_externals.load_field_models)(current_data_pointer, models_infos, palette_vram_positions, texture_vram_positions, dir_name, field_data_pointer, allocated_size, pcb_data_size);

    std::map<uint32_t, CharaOneModel> models_ordered;

    for (auto model: chara_one_models) {
        models_ordered.insert(std::pair<uint32_t, CharaOneModel>(model.second.modelId, model.second));
    }

    field_model_map.clear();

    for (int i = 0; i < *ff8_externals.field_state_other_count; ++i) {
        int16_t model_id = (*ff8_externals.field_state_others)[i].model_id;

        if (model_id >= 0) {
            field_model_map.insert(std::pair<uint32_t, CharaOneModel>(((uint32_t *)ff8_externals.dword_1DCB340)[i], models_ordered.at(model_id)));
        }
    }

    return ret;
}

void ff8_remaster_init()
{
    replace_call(ff8_externals.sub_533C30 + 0x44, field_model_vertices_scale);
    replace_call(ff8_externals.sub_533CD0 + 0x28E, field_sub_530C30);
    replace_call(ff8_externals.read_field_data + (JP_VERSION ? 0xFA2 : 0xF0F), field_open_chara_one);

    char fullpath[MAX_PATH];

    snprintf(fullpath, sizeof(fullpath), "%s/main.zzz", ff8_externals.app_path);
    errno_t err = g_FF8ZzzArchiveMain.open(fullpath);

    if (err != 0) {
        ffnx_error("%s: cannot open %s (error code %d)\n", __func__, fullpath, err);
        exit(1);
    }

    snprintf(fullpath, sizeof(fullpath), "%s/other.zzz", ff8_externals.app_path);

    err = g_FF8ZzzArchiveOther.open(fullpath);

    if (err != 0) {
        ffnx_error("%s: cannot open %s (error code %d)\n", __func__, fullpath, err);
        exit(1);
    }
}
