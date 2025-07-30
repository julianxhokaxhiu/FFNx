/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 myst6re                                            //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Tang-Tang Zhou                                     //
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

#include <stdint.h>
#include "../globals.h"
#include "../patch.h"
#include "../log.h"

bool fonts_initialized = false;
ff8_font *fonts_fieldtdw_even = nullptr;
ff8_font *fonts_fieldtdw_odd = nullptr;
ff8_font *fonts_sysevn = nullptr;
ff8_font *fonts_sysodd = nullptr;
ff8_graphics_object *graphic_object_font8_even = nullptr;
ff8_graphics_object *graphic_object_font8_odd = nullptr;
uint8_t font_character_width_local_field[452];

ff8_font *malloc_ff8_font_structure()
{
    ff8_font *font = (ff8_font *)external_malloc(sizeof(ff8_font));
    font->graphics_object48 = nullptr;
    font->graphics_object4C = nullptr;
    font->graphics_object50 = nullptr;
    font->graphics_object54 = nullptr;

    return font;
}

ff8_graphics_object *ff8_create_font_graphic_object(const char *path, ff8_create_graphic_object *create_graphics_object_infos, bool isTmp = false)
{
    char buffer[MAX_PATH] = {};

    if (!isTmp) {
        if (create_graphics_object_infos->file_container != nullptr) {
            sprintf(buffer, "c:%s%s", ff8_externals.archive_path_prefix_menu, path);
        } else {
            sprintf(buffer, "%s%s", ff8_externals.archive_path_prefix_menu, path);
        }
    } else {
        strcpy(buffer, path);
    }

    void *dword_1D29F5C = *(void **)0x1D29F5C; // TODO

    return ((ff8_graphics_object*(*)(int,int,ff8_create_graphic_object*,char*,void*))(ff8_externals._load_texture))(1, 12, create_graphics_object_infos, buffer, dword_1D29F5C);
}

void free_font_graphics_object(ff8_font *font)
{
    if (font->graphics_object48 != nullptr) {
        ff8_externals.free_graphics_object(font->graphics_object48);
        font->graphics_object48 = nullptr;
    }
    if (font->graphics_object4C != nullptr) {
        ff8_externals.free_graphics_object(font->graphics_object4C);
        font->graphics_object4C = nullptr;
    }
    if (font->graphics_object50 != nullptr) {
        ff8_externals.free_graphics_object(font->graphics_object50);
        font->graphics_object50 = nullptr;
    }
    if (font->graphics_object54 != nullptr) {
        ff8_externals.free_graphics_object(font->graphics_object54);
        font->graphics_object54 = nullptr;
    }
}

void fill_font_structure(ff8_font *font, int width, int height, int ratio)
{
    if (font->graphics_object48 == nullptr) {
        return;
    }

    font->field_4 = float(width * ratio);
    font->field_8 = float(height * ratio);
    font->field_38 = font->field_34;
    font->field_3D = 0;

    font->field_0 = 0;
    font->field_14 = 4;
    font->field_3E = ((ff8_tex_header *)(((ff8_texture_set *)(font->graphics_object48->hundred_data->texture_set))->tex_header))->field_DC;
    font->field_40 = ((ff8_tex_header *)(((ff8_texture_set *)(font->graphics_object48->hundred_data->texture_set))->tex_header))->field_E0;
    font->field_C = 1.0 / font->field_4;
    font->field_10 = 1.0 / font->field_8;
    font->field_18 = float(width);
    font->field_1C = float(height);
    font->field_20 = 1.0 / font->field_18;
    font->field_24 = 1.0 / font->field_1C;
    font->field_28 = font->field_4 / font->field_18;
    font->field_2C = font->field_8 / font->field_1C;
}

void create_graphics_object_info_structure_for_font(ff8_create_graphic_object *create_graphics_object_infos)
{
    ff8_externals.create_graphics_object_info_structure(4, create_graphics_object_infos);
    create_graphics_object_infos->field_7C |= 0x80u;
    create_graphics_object_infos->flags |= 0x11u;
    create_graphics_object_infos->field_18 = *(uint32_t *)0x1D29F60; // graphics_instance TODO
}

void ff8_load_fonts_field(char *tdw_tim_data, char *path)
{
    ffnx_trace("%s %s\n", __func__, path);
    size_t element_count;
    ff8_create_graphic_object create_graphics_object_infos;
    int *tim = (int *)tdw_tim_data;
    char *tim_img_header;

    if ((tdw_tim_data[4] & 8) != 0) { // paletted
        tim_img_header = &tdw_tim_data[*((DWORD *)tdw_tim_data + 2) + 8];
    } else {
        tim_img_header = tdw_tim_data + 8;
    }
    int16_t width = *((int16_t *)tim_img_header + 4), height = *((int16_t *)tim_img_header + 5);

    if (*(DWORD *)tim_img_header == 12) {
        return;
    }

    create_graphics_object_info_structure_for_font(&create_graphics_object_infos);
    create_graphics_object_infos.field_8 = 1;

    if (fonts_fieldtdw_even == nullptr) {
        fonts_fieldtdw_even = malloc_ff8_font_structure();
    } else {
        free_font_graphics_object(fonts_fieldtdw_even);
    }
    if (fonts_fieldtdw_odd == nullptr) {
        fonts_fieldtdw_odd = malloc_ff8_font_structure();
    } else {
        free_font_graphics_object(fonts_fieldtdw_odd);
    }
    bool use_low_res = true;

    if (*(uint32_t *)0xB86C80 == 2 && *(uint8_t *)0xB85E40) { // high res
        ff8_file_container *file_container = ((ff8_file_container*(*)(const char*))0x51ADC0)("\\MENU\\");
        // Remove extension
        path[strlen(path) - 4] = '\0';
        // Get filename
        const char *field_name = strrchr(path, '\\');
        if (field_name == nullptr) {
            field_name = path;
        } else {
            field_name += 1;
        }
        char filename[MAX_PATH] = {};
        snprintf(filename, sizeof(filename), "%shires\\fieldtdw\\%s00.dat", ff8_externals.archive_path_prefix_menu, field_name);
        void *buffer = nullptr;
        ((int(*)(void*,char*))0x4B8EA0)(&buffer, filename);
        if (buffer) {
            void *buffer2 = nullptr;
            ((void(*)(int*,int**,unsigned int*))0x4B9260)((int *)buffer, (int **)&buffer2, &element_count); // Malloc
            ((size_t(*)(void*,LPCSTR,size_t))0x4B8FB0)(buffer, "temp_evn.tim", element_count); // Write to file
            ((size_t(*)(void*,LPCSTR,size_t))0x4B8FB0)(buffer2, "temp_odd.tim", element_count); // Write to file
            external_free(buffer);
            external_free(buffer2);
            buffer = nullptr;
            fonts_fieldtdw_even->graphics_object48 = ff8_create_font_graphic_object("temp_evn.tim", &create_graphics_object_infos, true);
            fonts_fieldtdw_odd->graphics_object48 = ff8_create_font_graphic_object("temp_odd.tim", &create_graphics_object_infos, true);
            use_low_res = false;
        }
        snprintf(filename, sizeof(filename), "%shires\\fieldtdw\\%s01.dat", ff8_externals.archive_path_prefix_menu, field_name);
        ((int(*)(void*,char*))0x4B8EA0)(&buffer, filename);
        if (buffer) {
            void *buffer2 = nullptr;
            ((void(*)(int*,int**,unsigned int*))0x4B9260)((int *)buffer, (int **)&buffer2, &element_count); // Malloc
            ((size_t(*)(void*,LPCSTR,size_t))0x4B8FB0)(buffer, "temp_evn1.tim", element_count); // Write to file
            ((size_t(*)(void*,LPCSTR,size_t))0x4B8FB0)(buffer2, "temp_odd1.tim", element_count); // Write to file
            external_free(buffer);
            external_free(buffer2);
            buffer = nullptr;
            fonts_fieldtdw_even->graphics_object4C = ff8_create_font_graphic_object("temp_evn1.tim", &create_graphics_object_infos, true);
            fonts_fieldtdw_odd->graphics_object4C = ff8_create_font_graphic_object("temp_odd1.tim", &create_graphics_object_infos, true);
        }
        ff8_externals.free_file_container(file_container);
        fonts_fieldtdw_even->field_1 = 1;
        fonts_fieldtdw_even->field_30 = 2;
        fonts_fieldtdw_even->field_34 = 1;
        fonts_fieldtdw_even->field_3C = 0;
        fonts_fieldtdw_odd->field_1 = 1;
        fonts_fieldtdw_odd->field_30 = 2;
        fonts_fieldtdw_odd->field_34 = 1;
        fonts_fieldtdw_odd->field_3C = 0;
    }

    if (use_low_res) {
        void *buffer2 = nullptr;
        ((void(*)(int*,int**,unsigned int*))0x4B9260)(tim, (int **)&buffer2, &element_count); // Malloc
        ((size_t(*)(void*,LPCSTR,size_t))0x4B8FB0)(tim, "temp_evn.tim", element_count); // Write to file
        ((size_t(*)(void*,LPCSTR,size_t))0x4B8FB0)(buffer2, "temp_odd.tim", element_count); // Write to file
        external_free(buffer2);

        fonts_fieldtdw_even->graphics_object48 = ff8_create_font_graphic_object("temp_evn.tim", &create_graphics_object_infos, true);
        fonts_fieldtdw_even->field_1 = 0;
        fonts_fieldtdw_even->field_30 = 1;
        fonts_fieldtdw_even->field_34 = 1;
        fonts_fieldtdw_even->field_3C = 0;
        fonts_fieldtdw_odd->graphics_object48 = ff8_create_font_graphic_object("temp_odd.tim", &create_graphics_object_infos, true);
        fonts_fieldtdw_odd->field_1 = 0;
        fonts_fieldtdw_odd->field_30 = 1;
        fonts_fieldtdw_odd->field_34 = 1;
        fonts_fieldtdw_odd->field_3C = 0;
    }

    fill_font_structure(fonts_sysevn, width, height, fonts_sysevn->field_30);
    fill_font_structure(fonts_sysodd, width, height, fonts_sysodd->field_30);
}

void reset_graphics_object_field_58(ff8_graphics_object *graphic_object)
{
    if (graphic_object != nullptr) graphic_object->field_58 = 0;
}

void reset_font_graphics_object_field_58(ff8_font *font)
{
    reset_graphics_object_field_58(font->graphics_object48);
    reset_graphics_object_field_58(font->graphics_object4C);
    reset_graphics_object_field_58(font->graphics_object50);
    reset_graphics_object_field_58(font->graphics_object54);
}

void ff8_fonts_reset_field_58()
{
    ((void(*)())0x4B3080)();

    reset_font_graphics_object_field_58(fonts_fieldtdw_even);
    reset_font_graphics_object_field_58(fonts_fieldtdw_odd);

    reset_font_graphics_object_field_58(fonts_sysevn);
    reset_font_graphics_object_field_58(fonts_sysodd);

    reset_graphics_object_field_58(graphic_object_font8_even);
    reset_graphics_object_field_58(graphic_object_font8_odd);
}

void draw_graphics_object(ff8_graphics_object *graphic_object, game_obj *game_object)
{
    if (graphic_object != nullptr) ((void(*)(ff8_graphics_object*,game_obj*))0x4178D7)(graphic_object, game_object);
}

void draw_font(ff8_font *font, game_obj *game_object)
{
    draw_graphics_object(font->graphics_object48, game_object);
    draw_graphics_object(font->graphics_object4C, game_object);
    draw_graphics_object(font->graphics_object50, game_object);
    draw_graphics_object(font->graphics_object54, game_object);
}

void ff8_fonts_draw()
{
    game_obj *game_object = common_externals.get_game_object();

    draw_font(fonts_fieldtdw_even, game_object);
    draw_font(fonts_fieldtdw_odd, game_object);

    draw_font(fonts_sysevn, game_object);
    draw_font(fonts_sysodd, game_object);

    draw_graphics_object(graphic_object_font8_even, game_object);
    draw_graphics_object(graphic_object_font8_odd, game_object);

    ((void(*)())0x4B3000)();
}

char *pointer_to_iterate_to = nullptr;

void before_loop_fonts_sub_49F3D0()
{
    ffnx_trace("%s\n", __func__);
    ((void(*)())0x49AB90)(); // TODO

    char *v7 = (char *)0x1D6B940; // TODO

    for (int i = 0; i < 9; ++i) {
        if (*v7 != 1) {
            break;
        }
        ++v7;
    }

    pointer_to_iterate_to = v7;
}

void jp_fonts_with_font8c(ff8_draw_menu_sprite_texture_infos_short *texture_infos_short)
{
    ffnx_trace("%s\n", __func__);
    int v4 = (texture_infos_short->field_E / 64) - fonts_sysevn->field_40;

    texture_infos_short->field_E = (fonts_sysevn->field_3E >> 4) & 0x3F | ((fonts_sysevn->field_40 + uint16_t(v4 / 2)) << 6);
    ff8_font *fonts = (v4 & 1) != 0 ? fonts_sysodd : fonts_sysevn;

    // font8 support
    /* if (16 * (texture_infos_short->field_E & 0x3F) != fonts->field_3E
            && texture_infos_short->field_C >= 128u && texture_infos_short->field_D >= 152u && texture_infos_short->field_D < 200u) {
        ff8_graphics_object *graphic_object = (v4 & 1) != 0 ? graphic_object_font8_odd : graphic_object_font8_even;
        if (graphic_object != nullptr) {
            texture_infos_short->field_C += 128;
            texture_infos_short->field_D += 104;
            texture_infos_short->field_E = (((ff8_tex_header *)(((ff8_texture_set *)(graphic_object->hundred_data->texture_set))->tex_header))->field_DC >> 4) & 0x3F | ((LOWORD(((ff8_tex_header *)(((ff8_texture_set *)(graphic_object->hundred_data->texture_set))->tex_header))->field_E0) + uint16_t(v4 / 2)) << 6);

            return ((void(*)(ff8_draw_menu_sprite_texture_infos_short*,ff8_graphics_object*))0x49AE10)(texture_infos_short, graphic_object); // TODO
        }
    } */

    ((void(*)(ff8_draw_menu_sprite_texture_infos_short*,ff8_font*))0x49D190)(texture_infos_short, fonts);

    /* if (fonts->graphics_object48 != nullptr && fonts->graphics_object48->field_74 != nullptr) {
        fonts->graphics_object48->field_74[0].field_0 += 0.5;
        fonts->graphics_object48->field_74[0].field_4 += 0.5;
        fonts->graphics_object48->field_74[1].field_0 += 0.5;
        fonts->graphics_object48->field_74[1].field_4 += 0.5;
        fonts->graphics_object48->field_74[2].field_0 += 0.5;
        fonts->graphics_object48->field_74[2].field_4 += 0.5;
    }
    if (fonts->graphics_object4C != nullptr && fonts->graphics_object4C->field_74 != nullptr) {
        fonts->graphics_object4C->field_74[0].field_0 += 0.5;
        fonts->graphics_object4C->field_74[0].field_4 += 0.5;
        fonts->graphics_object4C->field_74[1].field_0 += 0.5;
        fonts->graphics_object4C->field_74[1].field_4 += 0.5;
        fonts->graphics_object4C->field_74[2].field_0 += 0.5;
        fonts->graphics_object4C->field_74[2].field_4 += 0.5;
    }
    if (fonts->graphics_object50 != nullptr && fonts->graphics_object50->field_74 != nullptr) {
        fonts->graphics_object50->field_74[0].field_0 += 0.5;
        fonts->graphics_object50->field_74[0].field_4 += 0.5;
        fonts->graphics_object50->field_74[1].field_0 += 0.5;
        fonts->graphics_object50->field_74[1].field_4 += 0.5;
        fonts->graphics_object50->field_74[2].field_0 += 0.5;
        fonts->graphics_object50->field_74[2].field_4 += 0.5;
    }
    if (fonts->graphics_object54 != nullptr && fonts->graphics_object54->field_74 != nullptr) {
        fonts->graphics_object54->field_74[0].field_0 += 0.5;
        fonts->graphics_object54->field_74[0].field_4 += 0.5;
        fonts->graphics_object54->field_74[1].field_0 += 0.5;
        fonts->graphics_object54->field_74[1].field_4 += 0.5;
        fonts->graphics_object54->field_74[2].field_0 += 0.5;
        fonts->graphics_object54->field_74[2].field_4 += 0.5;
    } */
}

void fonts_with_font8c_1_sub_49D190(ff8_draw_menu_sprite_texture_infos_short *texture_infos, ff8_font *fonts)
{
    ffnx_trace("%s\n", __func__);

    // Add missing information to texture_infos
    struc_kernel_sysfont *kernel_sysfont = (struc_kernel_sysfont *)0x1D2B730; // TODO
    *((uint16_t *)&(texture_infos->field_C) + 1) += kernel_sysfont[*pointer_to_iterate_to++].field_1;

    texture_infos->field_E = ((texture_infos->field_E - 0x3812) << 1) + 0x3812;

    return jp_fonts_with_font8c(texture_infos);
}

void fonts_with_font8c_2_sub_4A1CF0(ff8_draw_menu_sprite_texture_infos_short *texture_infos, ff8_font *fonts)
{
    ffnx_trace("%s: field_C=%X field_E=%X\n", __func__, *(uint16_t *)&texture_infos->field_C, texture_infos->field_E);

    return jp_fonts_with_font8c(texture_infos);
}

void fonts_with_font8c_3_sub_4A1CF0(ff8_draw_menu_sprite_texture_infos_short *texture_infos, ff8_font *fonts)
{
    ffnx_trace("%s\n", __func__);

    uint16_t field_c_divided_by_12 = *(uint16_t *)&texture_infos->field_C / 12;
    int a3 = (field_c_divided_by_12 >> 8) * 21 + (field_c_divided_by_12 & 0xFF);

    *(uint16_t *)&texture_infos->field_C = 12 * (((a3 >> 1) % 21) | (((a3 >> 1) / 21) << 8));

    texture_infos->field_E = ((a3 & 1) ? 14418 : 14354) + ((texture_infos->field_E - 14354) << 1);

    ffnx_trace("%s: field_C=%X field_E=%X\n", __func__, *(uint16_t *)&texture_infos->field_C, texture_infos->field_E);

    return jp_fonts_with_font8c(texture_infos);
}

void fonts_field_with_font8c_sub_49C3D0(ff8_draw_menu_sprite_texture_infos_short *texture_infos_short, ff8_font *fonts)
{
    ffnx_trace("%s\n", __func__);

    if (fonts_fieldtdw_odd->graphics_object48 != nullptr && fonts_fieldtdw_even->graphics_object48 != nullptr) {
        int v4 = (texture_infos_short->field_E >> 6) - fonts_fieldtdw_even->field_40;
        texture_infos_short->field_E = ((texture_infos_short->field_E >> 6) << 6) | ((fonts_fieldtdw_even->field_3E >> 4) & 0x3F);

        fonts = (v4 & 1) != 0 ? fonts_fieldtdw_odd : fonts_fieldtdw_even;

        ((void(*)(ff8_draw_menu_sprite_texture_infos_short*,ff8_font*))0x49D190)(texture_infos_short, fonts);

        /* if (fonts->graphics_object48 != nullptr && fonts->graphics_object48->field_74 != nullptr) {
            fonts->graphics_object48->field_74[0].field_0 += 0.5;
            fonts->graphics_object48->field_74[0].field_4 += 0.5;
            fonts->graphics_object48->field_74[1].field_0 += 0.5;
            fonts->graphics_object48->field_74[1].field_4 += 0.5;
            fonts->graphics_object48->field_74[2].field_0 += 0.5;
            fonts->graphics_object48->field_74[2].field_4 += 0.5;
        }
        if (fonts->graphics_object4C != nullptr && fonts->graphics_object4C->field_74 != nullptr) {
            fonts->graphics_object4C->field_74[0].field_0 += 0.5;
            fonts->graphics_object4C->field_74[0].field_4 += 0.5;
            fonts->graphics_object4C->field_74[1].field_0 += 0.5;
            fonts->graphics_object4C->field_74[1].field_4 += 0.5;
            fonts->graphics_object4C->field_74[2].field_0 += 0.5;
            fonts->graphics_object4C->field_74[2].field_4 += 0.5;
        }
        if (fonts->graphics_object50 != nullptr && fonts->graphics_object50->field_74 != nullptr) {
            fonts->graphics_object50->field_74[0].field_0 += 0.5;
            fonts->graphics_object50->field_74[0].field_4 += 0.5;
            fonts->graphics_object50->field_74[1].field_0 += 0.5;
            fonts->graphics_object50->field_74[1].field_4 += 0.5;
            fonts->graphics_object50->field_74[2].field_0 += 0.5;
            fonts->graphics_object50->field_74[2].field_4 += 0.5;
        }
        if (fonts->graphics_object54 != nullptr && fonts->graphics_object54->field_74 != nullptr) {
            fonts->graphics_object54->field_74[0].field_0 += 0.5;
            fonts->graphics_object54->field_74[0].field_4 += 0.5;
            fonts->graphics_object54->field_74[1].field_0 += 0.5;
            fonts->graphics_object54->field_74[1].field_4 += 0.5;
            fonts->graphics_object54->field_74[2].field_0 += 0.5;
            fonts->graphics_object54->field_74[2].field_4 += 0.5;
        } */

        return;
    }

    return ((void(*)(ff8_draw_menu_sprite_texture_infos_short*,ff8_font*))0x49D190)(texture_infos_short, fonts);
}

int get_character_width(int character)
{
    uint8_t *font_char_width = (uint8_t *)0x1D2B4F0; // TODO

    if ((character & 0x400) != 0) {
        character &= 0x3FF;
        font_char_width = font_character_width_local_field;
    }
    uint8_t width = font_char_width[character >> 1];
    if ((character & 1) != 0) {
        width >>= 4;
    }
    return width & 0xF;
}

uint8_t *kernel_bin_get_section_sub_482220(int section_id)
{
    ffnx_trace("%s\n", __func__);

    struc_kernel_sysfont *kernel_sysfont_byte_2231B44 = (struc_kernel_sysfont *)0x1D2B730;
    struc_kernel_sysfont *kernel_sysfont = kernel_sysfont_byte_2231B44 + 1;
    for (int i = 0; i < 10; ++i) {
        int character = ((uint8_t*(*)(int))0x47EC60)(section_id)[1] + i - 32;
        int space = get_character_width(character);
        kernel_sysfont->field_0 ^= (space ^ kernel_sysfont->field_0) & 0xF;
        if (space <= 8) {
            kernel_sysfont->field_0 = kernel_sysfont->field_0 & 0xF | (16 * ((8 - space) / 2));
        } else {
            kernel_sysfont->field_0 = kernel_sysfont->field_0 & 0xF;
        }
        ++kernel_sysfont;
    }

    return ((uint8_t*(*)(int))0x47EC60)(section_id);
}

int fill_texture_infos_for_font(int a1, ff8_draw_menu_sprite_texture_infos *texture_infos, int x, int y, int character, int current_color, uint32_t *field8)
{
    ffnx_trace("%s\n", __func__);

    bool is_extended_font = (character & 0x400) != 0;

    texture_infos->field_0 = 0x5000000;
    // << 7 only in jp version + 14418 only in jp version
    texture_infos->field_12 = ((current_color & 7) << 7) + ((character & 1) ? 14418 : 14354);
    texture_infos->field_8 = (current_color & 0xFFFFFFF8) == 0 ? *field8 : *(field8 + 1);
    texture_infos->field_4 = is_extended_font ? 0xE100041D : 0xE100041F;
    texture_infos->x_related = uint16_t(x);
    texture_infos->y_related = uint16_t(y);
    int character2 = (is_extended_font ? character & 0x3FF : character) >> 1; // >> 1 only in jp version
    texture_infos->field_14 = 0xC000C;
    texture_infos->field_10 = 12 * ((character2 % 21) | (int16_t(char(character2) / 21) << 8));

    if (is_extended_font) {
        return ((int(*)(int,ff8_draw_menu_sprite_texture_infos*))0x49C3D0)(a1, texture_infos);
    }

    return ((int(*)(int,ff8_draw_menu_sprite_texture_infos*))0x49C390)(a1, texture_infos);
}

ff8_draw_menu_sprite_texture_infos *fill_texture_infos_for_icon(int *a1, ff8_draw_menu_sprite_texture_infos *texture_infos, int &x, int y, uint8_t icon_param)
{
    int icon_id = 0;
    if (icon_param >= 64) {
        uint16_t *icon_id_word_B86CCC = (uint16_t *)0xB86CCC;
        icon_id = icon_id_word_B86CCC[icon_param];
    } else if (icon_param < 32 || icon_param > 47) {
        if (icon_param >= 48 && icon_param <= 63) { // Maybe sometimes icon_param <= 63 is not wanted?
            icon_id = icon_param + 80;
        }
    } else {
        int key_from_key_id = ((int(*)(int))0x4A2760)(icon_param - 32);
        if (key_from_key_id >= 0) {
            icon_id = key_from_key_id + 128;
        }
    }
    int dword_1D2B1EC = *(int *)0x1D2B1EC;
    if (a1 != nullptr || texture_infos != nullptr) {
        texture_infos = ((ff8_draw_menu_sprite_texture_infos*(*)(int*,ff8_draw_menu_sprite_texture_infos*,void*,int,uint16_t,uint16_t,int))0x4B6F20)(a1, texture_infos, ((void*(*)())ff8_externals.get_icon_sp1_data)(), icon_id, x, y, dword_1D2B1EC);
    }
    x += uint8_t(((uint16_t(*)(void*,int))0x4B6D60)(((void*(*)())ff8_externals.get_icon_sp1_data)(), icon_id)) + 1;

    return texture_infos;
}

int text_related_sub_4A0680(uint8_t *text_data, bool continue_on_new_line)
{
    ffnx_trace("%s\n", __func__);

    int x = 0, y = 12, max_x = 0, max_y = 12;

    for (;;) {
        uint8_t current_byte = *text_data++;

        if (current_byte == 0) {
            break;
        }

        if (current_byte == 2 || current_byte == 1 || current_byte == 7) { // New line, New page, ???
            if (current_byte == 2) { // New line
                y += 16;
            } else {
                y = 12;
            }
            if (max_y < y) {
                max_y = y;
            }
            if (max_x < x) {
                max_x = x;
            }
            x = 0;
            if (!continue_on_new_line) {
                return max_x | (max_y << 16);
            }
        } else if (current_byte == 5) { // Icon
            fill_texture_infos_for_icon(nullptr, nullptr, x, 0, *text_data++);
        } else if (current_byte <= 15) {
            ++text_data;
        } else if (current_byte >= 24) {
            int character;
            if (current_byte < 32) { // two-bytes
                if (current_byte > 27) { // Field extended font
                    character = int(*text_data + 224 * current_byte - 6304) | 0x400;
                } else {
                    character = *text_data + 224 * current_byte - 5408;
                }
                text_data++;
            } else {
                character = current_byte - 32;
            }
            x += get_character_width(character);
        }
    }
    if (max_x < x) {
        max_x = x;
    }
    if (max_y < y) {
        max_y = y;
    }
    return max_x | (max_y << 16);
}

ff8_draw_menu_sprite_texture_infos *input_get_command_keycodes_sub_4A0990(
    int *a1,
    ff8_draw_menu_sprite_texture_infos *texture_infos,
    int x,
    int y,
    uint8_t *text_data,
    int current_color
) {
    ffnx_trace("%s\n", __func__);

    int x_orig = x;

    if (text_data == nullptr || y > 256 || y < -8) {
        return texture_infos;
    }

    for (;;) {
        uint8_t current_byte = *text_data++;

        if (current_byte == 2) { // new line
            x = x_orig;
            y += 16;

            continue;
        }

        if (current_byte <= 24 || x > 384) {
            break;
        }

        int character;
        if (current_byte < 32) { // two-bytes
            if (current_byte > 27) { // Field extended font
                character = int(*text_data + 224 * current_byte - 6304) | 0x400;
            } else {
                character = *text_data + 224 * current_byte - 5408;
            }
            text_data++;
        } else {
            character = current_byte - 32;
        }
        *a1 = fill_texture_infos_for_font(*a1, texture_infos, x, y, character, current_color, (uint32_t *)0x1D2ADD8);
        x += get_character_width(character);
        ++texture_infos;
    }

    return texture_infos;
}

void menu_parse_and_render_text_sub_4A0B70(int *a1, int x, int y, uint8_t *text_data)
{
    ffnx_trace("%s\n", __func__);

    int x_orig = x;
    int *dword_1D762E0 = (int *)0x1D762E0;
    int dword_227C6F0_orig = *dword_1D762E0;
    int battle_struct = ((int(*)(int))0x403E00)(0);
    uint8_t *output = (uint8_t *)(battle_struct + 768);
    *dword_1D762E0 = battle_struct + 896;
    int current_color = 7;
    uint8_t *text_it = text_data;
    ff8_draw_menu_sprite_texture_infos *texture_infos = ((ff8_draw_menu_sprite_texture_infos*(*)())0x49A650)();
    uint8_t *last_color_iterate_text_byte_1D762E4 = (uint8_t *)0x1D762E4;

    while (text_it) {
        ((void(*)(uint8_t*,uint8_t*,int))0x4B84A0)(text_it, output, -1); // Expand text with names
        *last_color_iterate_text_byte_1D762E4 = current_color;
        text_it = ((uint8_t*(*)(uint8_t*))0x4B8430)(text_it); // To next line
        text_data = output;
        for (;;) {
            int current_byte = *text_data++;

            if (current_byte == 0 || current_byte == 1 || current_byte == 7) { // end of text, New page, ???
                text_it = nullptr; // Stop
                break;
            }

            if (current_byte == 2) { // New line
                x = x_orig;
                y += 16;
                break;
            }

            if (current_byte == 5) { // Icons
                // x is modified
                texture_infos = fill_texture_infos_for_icon(a1, texture_infos, x, y, *text_data++);
            } else if (current_byte == 6) { // Color
                current_color = (*text_data++) & 0xF;
            } else if (current_byte <= 15) {
                ++text_data;
            } else if (current_byte > 24) {
                int character;
                if (current_byte < 32) { // two-bytes
                    if (current_byte > 27) { // Field extended font
                        character = int(*text_data + 224 * current_byte - 6304) | 0x400;
                    } else {
                        character = *text_data + 224 * current_byte - 5408;
                    }
                    text_data++;
                } else {
                    character = current_byte - 32;
                }
                *a1 = fill_texture_infos_for_font(*a1, texture_infos, x, y, character, current_color, (uint32_t *)0x1D2B1EC);
                x += get_character_width(character);
                ++texture_infos;
            }
        }
    }

    ((void(*)(ff8_draw_menu_sprite_texture_infos*))0x49A670)(texture_infos);
    *dword_1D762E0 = dword_227C6F0_orig;
}

void window_parse_for_render_text_sub_4A0EE0(int *arg0, ff8_win_obj *win)
{
    ffnx_trace("%s\n", __func__);

    ((void(*)())0x49ABC0)();
    uint8_t **dword_1D762E0 = (uint8_t **)0x1D762E0;
    uint8_t *output = *dword_1D762E0;
    uint8_t *text_it = (uint8_t *)win->text_data1;
    *dword_1D762E0 += 128;
    int first_answer_line = win->first_question, last_anwser_line = win->last_question;
    ff8_draw_menu_sprite_texture_infos *texture_infos = ((ff8_draw_menu_sprite_texture_infos*(*)())0x49A650)();
    int x = win->field_30 + 2, y = win->field_32 - win->field_12 + 2;
    int current_line = 0;
    int current_color = win->current_color >> 4;
    if (first_answer_line <= 0) {
        x += 32;
    }
    uint8_t *last_color_iterate_text_byte_1D762E4 = (uint8_t *)0x1D762E4;
    *last_color_iterate_text_byte_1D762E4 = current_color;

    while (y < -16) {
        if (!text_it) {
            ((void(*)(ff8_draw_menu_sprite_texture_infos*))0x49A670)(texture_infos);
            dword_1D762E0 -= 128;
            ((void(*)())0x49ABE0)();

            return;
        }
        text_it = ((uint8_t*(*)(uint8_t*))0x4B8430)(text_it); // To next line
        current_color = *last_color_iterate_text_byte_1D762E4 & 0xF;
        y += 16;
        ++current_line;
        if (y >= -16) {
            break;
        }
    }

    while (text_it) {
        // Expand text with names
        ((void(*)(uint8_t*,uint8_t*,int))0x4B84A0)(text_it, output, current_line >= win->text_data1_line ? win->text_data1_offset : -1);
        *last_color_iterate_text_byte_1D762E4 = current_color;
        text_it = ((uint8_t*(*)(uint8_t*))0x4B8430)(text_it); // To next line
        ++current_line;
        uint8_t *text_data = output;
        for (;;) {
            int current_byte = *text_data++;

            if (current_byte == 0 || current_byte == 1 || current_byte == 7) { // end of text, New page, ???
                text_it = nullptr; // Stop
                break;
            }

            if (current_byte == 2) { // New line
                x = win->field_30 + 2;
                if (first_answer_line <= current_line && last_anwser_line >= current_line) {
                    x += 32;
                }
                y += 16;
                break;
            }

            if (current_byte == 5) { // Icons
                // x is modified
                texture_infos = fill_texture_infos_for_icon(arg0, texture_infos, x, y, *text_data++);
            } else if (current_byte == 6) { // Color
                current_color = (*text_data++) & 0xF;
            } else if (current_byte <= 15) {
                ++text_data;
            } else if (current_byte > 24) {
                int character;
                if (current_byte < 32) { // two-bytes
                    if (current_byte > 27) { // Field extended font
                        character = int(*text_data + 224 * current_byte - 6304) | 0x400;
                    } else {
                        character = *text_data + 224 * current_byte - 5408;
                    }
                    text_data++;
                } else {
                    character = current_byte - 32;
                }
                *arg0 = fill_texture_infos_for_font(*arg0, texture_infos, x, y, character, current_color, (uint32_t *)0x1D2B1EC);
                x += get_character_width(character);
            }
        }
    }

    ((void(*)(ff8_draw_menu_sprite_texture_infos*))0x49A670)(texture_infos);
    dword_1D762E0 -= 128;
    ((void(*)())0x49ABE0)();
}

ff8_draw_menu_sprite_texture_infos *battle_text_parse_common(
    int *a1,
    ff8_draw_menu_sprite_texture_infos *texture_infos,
    int x,
    int y,
    uint8_t *text_data,
    int16_t current_color,
    uint32_t *field8
) {
    ffnx_trace("%s\n", __func__);

    if (text_data == nullptr) {
        return texture_infos;
    }

    ((void(*)())0x49AB90)(); // TODO

    int x_orig = x;

    for (;;) {
        uint8_t current_byte = *text_data++;

        if (current_byte == 2) { // new line
            x = x_orig;
            y += 16;

            continue;
        }

        if (current_byte <= 24) {
            break;
        }

        int character;
        if (current_byte < 32) { // two-bytes
            if (current_byte > 27) { // Field extended font
                character = current_byte; // undefined behavior
            } else {
                character = *text_data + 224 * current_byte - 5408;
            }
            text_data++;
        } else {
            character = current_byte - 32;
        }
        *a1 = fill_texture_infos_for_font(*a1, texture_infos, x, y, character, current_color & 7, field8);
        x += get_character_width(character);
        ++texture_infos;
    }

    ((void(*)())0x49AB90)(); // TODO

    return texture_infos;
}

ff8_draw_menu_sprite_texture_infos *battle_text_parse_display_related_sub_4A6BC0(
    int *a1,
    ff8_draw_menu_sprite_texture_infos *texture_infos,
    int x,
    int y,
    uint8_t *text_data,
    int16_t current_color
) {
    ffnx_trace("%s\n", __func__);

    return battle_text_parse_common(a1, texture_infos, x, y, text_data, current_color, ((uint32_t*(*)(int))0x403E00)(0) + 228);
}

ff8_draw_menu_sprite_texture_infos *battle_text_parse_display_related_sub_4B0400(
    int *a1,
    ff8_draw_menu_sprite_texture_infos *texture_infos,
    int x,
    int y,
    uint8_t *text_data,
    int16_t current_color
) {
    ffnx_trace("%s\n", __func__);

    DWORD *aicon_sp1_data = ((DWORD*(*)())0x4B6D40)();
    DWORD *battle_input_dword_1D6D168 = (DWORD *)0x1D6D168;

    uint32_t v14 = *(DWORD *)((char *)aicon_sp1_data + uint16_t(aicon_sp1_data[*((uint16_t *)battle_input_dword_1D6D168 + 34) + 1]));
    uint32_t field8 = *battle_input_dword_1D6D168 | (((v14 >> 26) & 2) << 24);

    return battle_text_parse_common(a1, texture_infos, x, y, text_data, current_color, &field8);
}

int32_t ff8_open_tdw_field(char *id_path, void *data)
{
    ffnx_trace("%s\n", __func__);

    char tdw_path[MAX_PATH] = {};

    strncpy(tdw_path, id_path, strnlen(id_path, MAX_PATH) - 2);
    strcat(tdw_path, "tdw");
    ffnx_trace("%s %s\n", __func__, tdw_path);

    if (ff8_externals.sm_pc_read(tdw_path, data) != 8) {
        uint32_t *tdw_header = (uint32_t *)data;

        if (tdw_header[1]) {
            ff8_load_fonts_field((char *)data + tdw_header[1], tdw_path);
            memcpy(font_character_width_local_field, (char *)data + tdw_header[0], sizeof(font_character_width_local_field));
            ((void(*)())0x49EFB0)(); // TODO
        }
    }

    return ff8_externals.sm_pc_read(id_path, data);
}

void convert_ascii_to_ff8_encoding_jp(char *data)
{
    ffnx_trace("%s: %s\n", __func__, data);

    size_t i = 0, len = strlen(data);

    for (; i < len; ++i) {
        char c = data[i];
        if (c >= 'A' && c <= 'Z') {
            c -= 0x73;
        } else if (c >= '0' && c <= '9') {
            c += 0x23;
        } else if (c >= 'a' && c <= 'z') {
            c += 0x6D;
        } else {
            c = 0x5F;
        }
        data[i] = c;
    }

    data[i] = 0;
}

void fonts_init_2()
{
    ffnx_trace("%s: fonts_initialized=%d is_japanese_font_loaded=%d\n", __func__, fonts_initialized, fonts_sysevn->graphics_object48 != nullptr);

    if (JP_VERSION || fonts_initialized || fonts_sysevn->graphics_object48 == nullptr) {
        return;
    }

    fonts_initialized = true;

    replace_call(0x497D90 + 0x7, ff8_fonts_reset_field_58);
    replace_call(0x497CA0 + 0xCD, ff8_fonts_draw);

    replace_call(0x4A2D70 + 0x5C, before_loop_fonts_sub_49F3D0);
    replace_call(0x49C090 + 0xB, fonts_with_font8c_1_sub_49D190);
    replace_call(0x49C390 + 0xE, fonts_with_font8c_2_sub_4A1CF0);
    replace_call(0x49C3B0 + 0xB, fonts_with_font8c_3_sub_4A1CF0);

    replace_call(0x49C3D0 + 0xE, fonts_field_with_font8c_sub_49C3D0);

    // Open tdw in field
    replace_call(0x471000 + 0x88B, ff8_open_tdw_field);

    replace_call(0x49EFB0 + 0xD2, kernel_bin_get_section_sub_482220);
    replace_function(0x4A0640, get_character_width);
    replace_function(0x4A0680, text_related_sub_4A0680);
    replace_function(0x4A0990, input_get_command_keycodes_sub_4A0990);
    replace_function(0x4A0B70, menu_parse_and_render_text_sub_4A0B70);
    replace_function(0x4A0EE0, window_parse_for_render_text_sub_4A0EE0);
    replace_function(0x4A6BC0, battle_text_parse_display_related_sub_4A6BC0);
    replace_function(0x4B0400, battle_text_parse_display_related_sub_4B0400);

    /* patch_code_word(0x4A2890 + 0x28 + 1, 0x73EAu);
    patch_code_word(0x4A2890 + 0x37 + 1, 0x23C2u);
    patch_code_word(0x4A2890 + 0x46 + 1, 0x6DC2u);
    patch_code_byte(0x4A2890 + 0x4B + 1, 0x5Fu); */

    // Convert ASCII to ff8 encoding
    replace_function(0x4A2890, convert_ascii_to_ff8_encoding_jp);

    /* uint8_t jp_get_char_patch[] = {
        //0xF6,0xC4,0x04,           // test    ah, 4
        //0x74,0x0C,                // jz      short loc_4A53FB
        //0x25,0xFF,0x03,0x00,0x00, // and     eax, 3FFh
        //0xB9,0x00,0x00,0x00,0x00, // mov     ecx, offset font_character_with_local_field_unk_2231984
        //0xEB,0x05,                // jmp     short loc_4A5400
        // loc_4A53FB:
        0xB9,0x00,0x00,0x00,0x00, // mov     ecx, offset font_character_width_global_byte_22317C0
        // loc_4A5400:
        0x8B,0xD0,                // mov     edx, eax
        0xD1,0xFA,                // sar     edx, 1
        0xA8,0x01,                // test    al, 1
        0x8A,0x0C,0x0A,           // mov     cl, [edx+ecx]
        0x74,0x03,                // jz      short loc_4A540E
        0xC0,0xE9,0x04            // shr     cl, 4
        // loc_4A540E:
    };

    memcpy_code(0x4A0680 + 0x10A, jp_get_char_patch, sizeof(jp_get_char_patch));
    // Fill with NOP
    memset_code(0x4A0680 + 0x10A + sizeof(jp_get_char_patch), 0x90, 55 - sizeof(jp_get_char_patch));
    // Update addresses
    //patch_code_dword(0x4A0680 + 0x10A + 11, 0x2231984);
    patch_code_dword(0x4A0680 + 0x10A + /* 18 /* 1, 0x1D2B4F0);

    patch_code_byte(0x4A0990 + 0x181, 0x53); // push ebx
    replace_call_function(0x4A0680 + 0x181 + 1, get_character_width); // call get_character_width
    uint8_t add_esp_4[] = {0x83, 0xC4, 0x04};
    memcpy_code(0x4A0680 + 0x181 + 1 + 5, add_esp_4, sizeof(add_esp_4)); // add esp, 4
    memset_code(0x4A0680 + 0x181 + 1 + 5 + sizeof(add_esp_4), 0x90, 55 - 1 - 5 - sizeof(add_esp_4)); // nop
    */
}

void ff8_load_fonts(ff8_file_container *file_container, int is_exit_menu)
{
    ((void(*)(ff8_file_container*,int))ff8_externals.load_fonts)(file_container, is_exit_menu);

    ff8_create_graphic_object create_graphics_object_infos;
    bool is_flfifs_opened_locally = false;

    if (fonts_fieldtdw_even == nullptr) {
        fonts_fieldtdw_even = malloc_ff8_font_structure();
    }
    if (fonts_fieldtdw_odd == nullptr) {
        fonts_fieldtdw_odd = malloc_ff8_font_structure();
    }

    int is_exit_menu_or_just_allocated = is_exit_menu;

    if (fonts_sysevn == nullptr) {
        fonts_sysevn = malloc_ff8_font_structure();
        fonts_sysodd = malloc_ff8_font_structure();

        is_exit_menu_or_just_allocated = 1;
    }

    free_font_graphics_object(fonts_sysevn);
    free_font_graphics_object(fonts_sysodd);

    if (graphic_object_font8_even != nullptr) {
        ff8_externals.free_graphics_object(graphic_object_font8_even);
        graphic_object_font8_even = nullptr;
    }
    if (graphic_object_font8_odd != nullptr) {
        ff8_externals.free_graphics_object(graphic_object_font8_odd);
        graphic_object_font8_odd = nullptr;
    }

    create_graphics_object_info_structure_for_font(&create_graphics_object_infos);

    if (file_container == nullptr) {
        file_container = ((ff8_file_container*(*)(const char*))0x51ADC0)("\\MENU\\"); // menu fifls struct
        is_flfifs_opened_locally = true;
    }
    create_graphics_object_infos.file_container = file_container;
    if (*(uint32_t *)0xB86C80 == 2 && *(uint8_t *)0xB85E40) { // high res
        if (is_exit_menu_or_just_allocated) {
            fonts_sysevn->field_1 = 1;
            fonts_sysevn->field_3C = 0;
            fonts_sysodd->field_1 = 1;
            fonts_sysodd->field_3C = 0;
        } else {
            fonts_sysevn->field_1 = 0;
            fonts_sysevn->field_3C = 1;
            fonts_sysodd->field_1 = 0;
            fonts_sysodd->field_3C = 1;
        }
        fonts_sysevn->graphics_object48 = ff8_create_font_graphic_object("hires\\sysevn00.tim", &create_graphics_object_infos);
        fonts_sysevn->graphics_object4C = ff8_create_font_graphic_object("hires\\sysevn01.tim", &create_graphics_object_infos);
        fonts_sysevn->graphics_object50 = ff8_create_font_graphic_object("hires\\sysevn02.tim", &create_graphics_object_infos);
        fonts_sysevn->graphics_object54 = ff8_create_font_graphic_object("hires\\sysevn03.tim", &create_graphics_object_infos);
        fonts_sysevn->field_30 = 4;
        fonts_sysevn->field_34 = 2;
        fonts_sysodd->graphics_object48 = ff8_create_font_graphic_object("hires\\sysodd00.tim", &create_graphics_object_infos);
        fonts_sysodd->graphics_object4C = ff8_create_font_graphic_object("hires\\sysodd01.tim", &create_graphics_object_infos);
        fonts_sysodd->graphics_object50 = ff8_create_font_graphic_object("hires\\sysodd02.tim", &create_graphics_object_infos);
        fonts_sysodd->graphics_object54 = ff8_create_font_graphic_object("hires\\sysodd03.tim", &create_graphics_object_infos);
        fonts_sysodd->field_30 = 4;
        fonts_sysodd->field_34 = 2;
    } else { // low res
        fonts_sysevn->graphics_object48 = ff8_create_font_graphic_object("sysfnt_even.tim", &create_graphics_object_infos);
        fonts_sysevn->field_1 = 0;
        fonts_sysevn->field_30 = 1;
        fonts_sysevn->field_34 = 1;
        fonts_sysevn->field_3C = 0;
        fonts_sysodd->graphics_object48 = ff8_create_font_graphic_object("sysfnt_odd.tim", &create_graphics_object_infos);
        fonts_sysodd->field_1 = 0;
        fonts_sysodd->field_30 = 1;
        fonts_sysodd->field_34 = 1;
        fonts_sysodd->field_3C = 0;
    }

    fill_font_structure(fonts_sysevn, 256, 252, fonts_sysodd->field_34);
    fill_font_structure(fonts_sysodd, 256, 252, fonts_sysodd->field_34);

    graphic_object_font8_even = ff8_create_font_graphic_object("font8_even.tim", &create_graphics_object_infos);
    graphic_object_font8_odd = ff8_create_font_graphic_object("font8_odd.tim", &create_graphics_object_infos);

    if (is_flfifs_opened_locally) {
        ff8_externals.free_file_container(file_container);
    }

    fonts_init_2();
}

void ff8_cleanup_fonts()
{
    if (fonts_fieldtdw_even != nullptr) {
        free_font_graphics_object(fonts_fieldtdw_even);
        external_free(fonts_fieldtdw_even);
        fonts_fieldtdw_even = nullptr;
    }
    if (fonts_fieldtdw_odd != nullptr) {
        free_font_graphics_object(fonts_fieldtdw_odd);
        external_free(fonts_fieldtdw_odd);
        fonts_fieldtdw_odd = nullptr;
    }
    if (fonts_sysevn != nullptr) {
        free_font_graphics_object(fonts_sysevn);
        external_free(fonts_sysevn);
        fonts_sysevn = nullptr;
    }
    if (fonts_sysodd != nullptr) {
        free_font_graphics_object(fonts_sysodd);
        external_free(fonts_sysodd);
        fonts_sysodd = nullptr;
    }
    if (graphic_object_font8_even != nullptr) {
        ff8_externals.free_graphics_object(graphic_object_font8_even);
        graphic_object_font8_even = nullptr;
    }
    if (graphic_object_font8_odd != nullptr) {
        ff8_externals.free_graphics_object(graphic_object_font8_odd);
        graphic_object_font8_odd = nullptr;
    }

    ((void(*)())ff8_externals.pubintro_cleanup_textures_menu)();
}

int fonts_sysoddeven_sub_4A0E70(int a1, ff8_draw_menu_sprite_texture_infos_short *texture_infos)
{
    ffnx_trace("%s: field_C=%X field_E=%X\n", __func__, *(uint16_t *)&texture_infos->field_C, texture_infos->field_E);

    return ((int(*)(int,ff8_draw_menu_sprite_texture_infos_short*))0x4A0E70)(a1, texture_infos);
}

int fonts_sysoddeven_sub_4A0E00(int a1, ff8_draw_menu_sprite_texture_infos *texture_infos)
{
    ffnx_trace("%s: field_C=%X field_E=%X\n", __func__, texture_infos->field_10, texture_infos->field_12);

    return ((int(*)(int,ff8_draw_menu_sprite_texture_infos*))0x4A0E00)(a1, texture_infos);
}

void fonts_init()
{
    if (JP_VERSION) {
        replace_call(0x4C23C0 + 0x1CF, fonts_sysoddeven_sub_4A0E70);
        replace_call(0x4A5B40 + 0x26E, fonts_sysoddeven_sub_4A0E00);
    }

    if (JP_VERSION || !ff8_enable_japanese_font) {
        return;
    }

    replace_call(ff8_externals.menu_enter2 + 0x16, ff8_load_fonts);
    replace_call(ff8_externals.sub_4972A0 + 0x16, ff8_load_fonts);
    replace_call(ff8_externals.sub_497F20 + 0xB3, ff8_load_fonts);
    replace_call(ff8_externals.pubintro_cleanup_textures + 0x0, ff8_cleanup_fonts);
}
