/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2024 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include <shlwapi.h>

#include "common.h"
#include "log.h"
#include "utils.h"
#include "patch.h"
#include "saveload.h"
#include "ff8/file.h"

constexpr char *FF8_EXE_BATTLE_SCANS = "battle_scans";
constexpr char *FF8_EXE_CARD_NAMES = "card_names";
constexpr char *FF8_EXE_DRAW_POINT = "draw_point";
constexpr char *FF8_EXE_CARD_TEXTS = "card_texts";

uint8_t *ff8_exe_scan_texts = nullptr;
bool ff8_exe_scan_texts_file_absent = false;
uint8_t *ff8_exe_card_names = nullptr;
bool ff8_exe_card_names_file_absent = false;
uint8_t *ff8_exe_draw_point = nullptr;
bool ff8_exe_draw_point_file_absent = false;
uint8_t *ff8_exe_card_texts = nullptr;
bool ff8_exe_card_texts_file_absent = false;

uint8_t *ff8_remastered_scan_texts = nullptr;
uint8_t *ff8_remastered_card_names = nullptr;
uint8_t *ff8_remastered_draw_point = nullptr;
uint8_t *ff8_remastered_card_texts = nullptr;

bool ff8_get_exe_path(const char *name, char *target_filename)
{
    char langPath[16] = "";
    ff8_fs_lang_string(langPath);

    snprintf(target_filename, MAX_PATH, "%s/%s/%s/exe/%s.msd", basedir, direct_mode_path.c_str(), langPath, name);
    normalize_path(target_filename);

    if (fileExists(target_filename)) {
        return true;
    }

    if (trace_all || trace_direct) ffnx_warning("Direct file not found %s\n", target_filename);

    // Retry without lang
    snprintf(target_filename, MAX_PATH, "%s/%s/exe/%s.msd", basedir, direct_mode_path.c_str(), name);
    normalize_path(target_filename);

    if (fileExists(target_filename)) {
        return true;
    }

    if (trace_all || trace_direct) ffnx_warning("Direct file not found %s\n", target_filename);

    return false;
}

uint8_t *ff8_remastered_open_exe_file(const char *name, int32_t *out_size = nullptr)
{
    char target_filename[MAX_PATH] = {};
    get_data_lang_path(target_filename, false);
    PathAppendA(target_filename, name);
    strcat(target_filename, "_");
    concat_lang_str(target_filename);
    strcat(target_filename, ".dat");

    ff8_file_context context = ff8_file_context();
    int32_t file_size = 0;
    // Use FF8 filesystem
    uint8_t *buffer = ff8_externals.open_read_close_file(&context, 0, &file_size, target_filename);

    if (out_size != nullptr) {
        *out_size = file_size;
    }

    return buffer;
}

void ff8_dump_msd(const char *filename, uint8_t *data)
{
    uint32_t *offsets = (uint32_t *)data;
    uint32_t first_offset = *offsets;
    int text_count = first_offset / 4;
    int higher_offset = 0;

    for (int i = 0; i < text_count; ++i) {
        if (offsets[i] > higher_offset) {
            higher_offset = offsets[i];
        }
    }

    for (int i = higher_offset; i < higher_offset + 1024; ++i) {
        if (data[i] == '\0') {
            higher_offset = i + 1;
            break;
        }
    }

    FILE *f = fopen(filename, "wb");

    if (f == nullptr) {
        return;
    }

    fwrite(data, first_offset + higher_offset, 1, f);
    fclose(f);
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
    if (ff8_get_exe_path(FF8_EXE_BATTLE_SCANS, filename)) {
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

void ff8_dump_card_names()
{
    uint8_t *data = *(uint8_t **)ff8_externals.card_name_positions;
    uint16_t *positions = *(uint16_t **)ff8_externals.card_name_positions;
    constexpr int text_count = 110;
    uint16_t old_data_offset = (1 + text_count) * sizeof(uint16_t), new_data_offset = text_count * sizeof(uint32_t);
    uint32_t offsets_rel_to_start[text_count] = {};
    int higher_offset = 0;

    for (int i = 0; i < text_count; ++i) {
        offsets_rel_to_start[i] = positions[i + 1] - old_data_offset + new_data_offset;
        if (positions[i + 1] > higher_offset) {
            higher_offset = positions[i + 1];
        }
    }

    for (int i = higher_offset; i < higher_offset + 1024; ++i) {
        if (data[i] == '\0') {
            higher_offset = i + 1;
            break;
        }
    }

    char filename[MAX_PATH] = {};
    if (ff8_get_exe_path(FF8_EXE_CARD_NAMES, filename)) {
        ffnx_warning("Save exe file skipped because the file [ %s ] already exists.\n", filename);

        return;
    }

    FILE *f = fopen(filename, "wb");

    if (f == nullptr) {
        return;
    }

    fwrite(offsets_rel_to_start, new_data_offset, 1, f);
    fwrite(data + old_data_offset, higher_offset - old_data_offset, 1, f);
    fclose(f);
}

void ff8_dump_card_texts()
{
    uint8_t *data = *ff8_externals.card_texts_off_B96968;
    uint16_t *offsets = (uint16_t *)(data + 2);
    uint32_t first_offset = *offsets;
    int text_count = first_offset / 4;
    int higher_offset = 0;
    uint32_t offsets_rel_to_start[0x200] = {};

    for (int i = 0; i < text_count; ++i) {
        offsets_rel_to_start[i] = offsets[i * 2];
        if (offsets[i * 2] > higher_offset) {
            higher_offset = offsets[i * 2];
        }
    }

    for (int i = higher_offset; i < higher_offset + 1024; ++i) {
        if (data[i] == '\0') {
            higher_offset = i + 1;
            break;
        }
    }

    char filename[MAX_PATH] = {};
    if (ff8_get_exe_path(FF8_EXE_CARD_TEXTS, filename)) {
        ffnx_warning("Save exe file skipped because the file [ %s ] already exists.\n", filename);

        return;
    }

    FILE *f = fopen(filename, "wb");

    if (f == nullptr) {
        return;
    }

    fwrite(offsets_rel_to_start, text_count * 4, 1, f);
    fwrite(data + first_offset, higher_offset - first_offset, 1, f);
    fclose(f);
}

uint8_t *ff8_open_msd(char *filename, int *data_size = nullptr)
{
    if (trace_all || trace_direct) ffnx_info("Direct file using %s\n", filename);

    if (data_size != nullptr) {
        *data_size = 0;
    }

    FILE *f = fopen(filename, "rb");

    if (f == nullptr) {
        return nullptr;
    }

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t *target_data = (uint8_t *)driver_malloc(file_size); // Allocated once, never freed

    if (target_data == nullptr) {
        return nullptr;
    }

    fread(target_data, file_size, 1, f);
    fclose(f);

    if (data_size != nullptr) {
        *data_size = file_size;
    }

    return target_data;
}

uint8_t *ff8_override_msd_data(const char *name, uint8_t *&msd_texts, bool &file_is_absent, int *data_size = nullptr)
{
    if (msd_texts != nullptr || file_is_absent) {
        return msd_texts;
    }

    char filename[MAX_PATH] = {};
    if (! ff8_get_exe_path(name, filename)) {
        file_is_absent = true;

        return nullptr;
    }

    msd_texts = ff8_open_msd(filename, data_size);

    return msd_texts;
}

uint8_t *ff8_override_card_texts()
{
    int data_size = 0;
    uint8_t *ff8_exe_card_texts_msd = nullptr;
    ff8_exe_card_texts_msd = ff8_override_msd_data(FF8_EXE_CARD_TEXTS, ff8_exe_card_texts_msd, ff8_exe_card_texts_file_absent, &data_size);

    if (ff8_exe_card_texts_msd == nullptr) {
        return nullptr;
    }

    ff8_exe_card_texts = (uint8_t *)driver_malloc(data_size + 16); // Allocated once, never freed

    if (ff8_exe_card_texts == nullptr) {
        driver_free(ff8_exe_card_texts_msd);

        return nullptr;
    }

    uint32_t *msd_offsets = (uint32_t *)ff8_exe_card_texts_msd;
    *(uint16_t *)ff8_exe_card_texts = *(uint16_t *)*ff8_externals.card_texts_off_B96968;
    int text_count = 256;

    for (int i = 0; i < text_count; ++i) {
        if (msd_offsets[i] / 4 < text_count) {
            text_count = msd_offsets[i] / 4;
        }
    }

    // We shift the positions (+2)
    memcpy(ff8_exe_card_texts + 2, ff8_exe_card_texts_msd, text_count * 4);
    // But not the texts!
    memcpy(ff8_exe_card_texts + text_count * 4, ff8_exe_card_texts_msd + text_count * 4, data_size - text_count * 4);

    driver_free(ff8_exe_card_texts_msd);

    return ff8_exe_card_texts;
}

uint8_t *ff8_battle_get_scan_text(uint8_t target_id)
{
    uint8_t *direct_data_msd = ff8_override_msd_data(FF8_EXE_BATTLE_SCANS, ff8_exe_scan_texts, ff8_exe_scan_texts_file_absent);
    uint8_t *entities = (uint8_t *)ff8_externals.battle_entities_1D27BCB;

    if (direct_data_msd != nullptr) {
        uint32_t *positions = (uint32_t *)direct_data_msd;

        if (trace_all) ffnx_trace("%s: get scan text target_id=%d entity_id=%d\n", __func__, target_id, entities[208 * target_id]);

        return direct_data_msd + positions[entities[208 * target_id]];
    }

    if (ff8_remastered_edition) {
        if (ff8_remastered_scan_texts == nullptr) {
            ff8_remastered_scan_texts = ff8_remastered_open_exe_file("desc_scan");
        }
        if (ff8_remastered_scan_texts != nullptr) {
            uint16_t *positions = (uint16_t *)ff8_remastered_scan_texts;

            return ff8_remastered_scan_texts + 320 + positions[entities[208 * target_id]];
        }
    }

    return ((uint8_t*(*)(uint8_t))ff8_externals.scan_get_text_sub_B687C0)(target_id);
}

char *ff8_get_card_name(int32_t card_id)
{
    if (card_id >= 110) {
        return nullptr;
    }

    uint8_t *direct_data_msd = ff8_override_msd_data(FF8_EXE_CARD_NAMES, ff8_exe_card_names, ff8_exe_card_names_file_absent);
    if (direct_data_msd != nullptr) {
        uint32_t *positions = (uint32_t *)direct_data_msd;

        if (trace_all) ffnx_trace("%s: get card name card_id=%d\n", __func__, card_id);

        return (char *)direct_data_msd + positions[card_id];
    }

    if (ff8_remastered_edition) {
        if (ff8_remastered_card_names == nullptr) {
            ff8_remastered_card_names = ff8_remastered_open_exe_file("off_cards_names");
        }
        if (ff8_remastered_card_names != nullptr) {
            uint16_t *positions = (uint16_t *)ff8_remastered_card_names;

            return (char *)ff8_remastered_card_names + positions[card_id + 1];
        }
    }

    uint16_t *positions = *(uint16_t **)ff8_externals.card_name_positions;

    return *(char **)ff8_externals.card_name_positions + positions[card_id + 1];
}

void dump_exe_data()
{
    char dirname[MAX_PATH] = {};
    snprintf(dirname, sizeof(dirname), "%s/%s/exe/", basedir, direct_mode_path.c_str());

    normalize_path(dirname);
    make_path(dirname);

    if (ff8)
    {
        ff8_dump_battle_scan_texts();
        ff8_dump_card_names();

        if (!ff8_get_exe_path(FF8_EXE_DRAW_POINT, dirname)) {
            ff8_dump_msd(dirname, *(uint8_t **)ff8_externals.drawpoint_messages);
        }

        ff8_dump_card_texts();
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
        replace_function(ff8_externals.get_card_name, ff8_get_card_name);
        uint8_t *msd = ff8_override_msd_data(FF8_EXE_DRAW_POINT, ff8_exe_draw_point, ff8_exe_draw_point_file_absent);
        if (msd == nullptr && ff8_remastered_edition) {
            msd = ff8_remastered_open_exe_file("off_text_draw_point");
        }
        if (msd != nullptr) {
            patch_code_uint(ff8_externals.drawpoint_messages, uint32_t(msd));
        }
        msd = ff8_override_card_texts();
        if (msd == nullptr && ff8_remastered_edition) {
            msd = ff8_remastered_open_exe_file("byte_card_texts");
        }
        if (msd != nullptr) {
            patch_code_uint(uint32_t(ff8_externals.card_texts_off_B96504), uint32_t(msd));
            patch_code_uint(uint32_t(ff8_externals.card_texts_off_B96968), uint32_t(msd));
        }
    }
}
