/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2024 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
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

#include "common.h"
#include "log.h"
#include "utils.h"
#include "patch.h"
#include "saveload.h"

uint8_t *ff8_exe_scan_texts = nullptr;

bool ff8_get_battle_scan_texts_filename(char *filename)
{
    snprintf(filename, MAX_PATH, "%s/exe/battle_scans.msd", direct_mode_path.c_str());
    normalize_path(filename);

    return fileExists(filename);
}

void ff8_dump_battle_scan_texts()
{
    uint8_t *data = (uint8_t *)ff8_externals.scan_text_data;
    uint16_t *offsets_rel_to_data = (uint16_t *)ff8_externals.scan_text_positions;
    int text_count = int((uint16_t *)data - offsets_rel_to_data);
    uint16_t data_offset = text_count * sizeof(uint32_t);
    uint32_t offsets_rel_to_start[0x200] = {};
    int higher_offset = 0;

    for (int i = 0; i < text_count; ++i) {
        offsets_rel_to_start[i] = offsets_rel_to_data[i] + data_offset;
        if (offsets_rel_to_data[i] > higher_offset) {
            higher_offset = offsets_rel_to_data[i];
        }
    }

    for (int i = higher_offset; i < higher_offset + 1024; ++i) {
        if (data[i] == '\0') {
            higher_offset = i + 1;
            break;
        }
    }

    char filename[MAX_PATH] = {};
    if (ff8_get_battle_scan_texts_filename(filename)) {
        ffnx_warning("Save exe file skipped because the file [ %s ] already exists.\n", filename);

        return;
    }

    FILE *f = fopen(filename, "wb");

    if (f == nullptr) {
        return;
    }

    fwrite(offsets_rel_to_start, data_offset, 1, f);
    fwrite(data, higher_offset, 1, f);
    fclose(f);
}

uint8_t *ff8_override_battle_scans()
{
    if (ff8_exe_scan_texts != nullptr) {
        return ff8_exe_scan_texts;
    }

    char filename[MAX_PATH] = {};
    if (! ff8_get_battle_scan_texts_filename(filename)) {
        if (trace_all || trace_direct) ffnx_warning("Direct file not found %s\n", filename);

        return nullptr;
    }

    if (trace_all || trace_direct) ffnx_info("Direct file using %s\n", filename);

    FILE *f = fopen(filename, "rb");

    if (f == nullptr) {
        return nullptr;
    }

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    ff8_exe_scan_texts = (uint8_t *)driver_malloc(file_size); // Allocated once, never freed

    if (ff8_exe_scan_texts == nullptr) {
        return nullptr;
    }

    fread(ff8_exe_scan_texts, file_size, 1, f);
    fclose(f);

    return ff8_exe_scan_texts;
}

uint8_t *ff8_battle_get_scan_text(uint8_t target_id)
{
    uint8_t *direct_data_msd = ff8_override_battle_scans();

    if (direct_data_msd != nullptr) {
        uint32_t *positions = (uint32_t *)direct_data_msd;
        uint8_t *entities = (uint8_t *)ff8_externals.battle_entities_1D27BCB;

        if (trace_all) ffnx_trace("%s: get scan text target_id=%d entity_id=%d\n", __func__, target_id, entities[208 * target_id]);

        return direct_data_msd + positions[entities[208 * target_id]];
    }

    return ((uint8_t*(*)(uint8_t))ff8_externals.scan_get_text_sub_B687C0)(target_id);
}

void dump_exe_data()
{
    char dirname[MAX_PATH] = {};
    snprintf(dirname, sizeof(dirname), "%s/exe/", direct_mode_path.c_str());

    normalize_path(dirname);
    make_path(dirname);

    if (ff8)
    {
        ff8_dump_battle_scan_texts();
    }
}

void exe_data_init()
{
    if (save_exe_data)
    {
        dump_exe_data();
    }

    if (ff8)
    {
        replace_call(ff8_externals.sub_84F8D0 + 0x88, ff8_battle_get_scan_text);
    }
}
