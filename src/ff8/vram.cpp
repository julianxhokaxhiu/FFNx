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
#include "vram.h"

#include "../ff8.h"
#include "../patch.h"

char nextTextureName[MAX_PATH] = "";
int next_psxvram_x = -1;
int next_psxvram_y = -1;
int next_scale = 1;

int ff8_upload_vram(int16_t *pos_and_size, uint8_t *texture_buffer)
{
    const int x = pos_and_size[0];
    const int y = pos_and_size[1];
    const int w = pos_and_size[2];
    const int h = pos_and_size[3];

    if (trace_all || trace_vram) ffnx_trace("%s x=%d y=%d w=%d h=%d texture_buffer=0x%X\n", __func__, x, y, w, h, texture_buffer);

    texturePacker.setTexture(nextTextureName, texture_buffer, x, y, w, h);

    ff8_externals.sub_464850(x, y, x + w - 1, h + y - 1);

    *nextTextureName = '\0';

    return 1;
}

int op_on_psxvram_sub_4675B0_parent_call1(int a1, int structure, int x, int y, int w, int h, int bpp, int rel_pos, int a9, uint8_t *target)
{
    if (trace_all || trace_vram) ffnx_trace("%s: x=%d y=%d w=%d h=%d bpp=%d rel_pos=(%d, %d) a9=%d target=%X\n", __func__, x, y, w, h, bpp, rel_pos & 0xF, rel_pos >> 4, a9, target);

    next_psxvram_x = (x >> (2 - bpp)) + ((rel_pos & 0xF) << 6);
    next_psxvram_y = y + (((rel_pos >> 4) & 1) << 8);

    int ret = ff8_externals.sub_464F70(a1, structure, x, y, w, h, bpp, rel_pos, a9, target);

    next_psxvram_x = -1;
    next_psxvram_y = -1;

    return ret;
}

int op_on_psxvram_sub_4675B0_parent_call2(texture_page *tex_page, int rel_pos, int a3)
{
    if (trace_all || trace_vram) ffnx_trace("%s: x=%d y=%d color_key=%d rel_pos=(%d, %d)\n", __func__, tex_page->x, tex_page->y, tex_page->color_key, rel_pos & 0xF, rel_pos >> 4);

    next_psxvram_x = (tex_page->x >> (2 - tex_page->color_key)) + ((rel_pos & 0xF) << 6);
    next_psxvram_y = tex_page->y + (((rel_pos >> 4) & 1) << 8);

    int ret = ((int(*)(texture_page *, int, int))ff8_externals.sub_4653B0)(tex_page, rel_pos, a3);

    next_psxvram_x = -1;
    next_psxvram_y = -1;

    return ret;
}

void read_vram_to_buffer1(uint8_t *vram, int vram_w_2048, uint8_t *target, int target_w, signed int w, int h, int bpp)
{
    if (trace_all || trace_vram) ffnx_trace("%s: vram_pos=(%d, %d) target=%X target_w=%d w=%d h=%d bpp=%d\n", __func__, next_psxvram_x, next_psxvram_y, target, target_w, w, h, bpp);

    if (next_psxvram_x == -1)
    {
        ffnx_warning("%s: cannot detect VRAM position\n", __func__);
    }
    else
    {
        texturePacker.registerTiledTex(target, next_psxvram_x, next_psxvram_y, w, h);
    }

    ff8_externals.sub_4675C0(vram, vram_w_2048, target, target_w, w, h, bpp);
}

uint32_t ff8_credits_open_texture(char *fileName, char *buffer)
{
    if (trace_all || trace_vram) ffnx_trace("%s: %s\n", __func__, fileName);

    // {name}.lzs
    strncpy(nextTextureName, strrchr(fileName, '\\') + 1, MAX_PATH);

    return ff8_externals.credits_open_file(fileName, buffer);
}

void vram_init()
{
    texturePacker.setVram((uint8_t *)ff8_externals.psxvram_buffer);

    replace_call(ff8_externals.open_lzs_image + 0x72, ff8_credits_open_texture);

    replace_function(ff8_externals.upload_psx_vram, ff8_upload_vram);

    replace_call(ff8_externals.sub_464BD0 + 0x53, op_on_psxvram_sub_4675B0_parent_call1);
    replace_call(ff8_externals.sub_464BD0 + 0xED, op_on_psxvram_sub_4675B0_parent_call1);
    replace_call(ff8_externals.sub_464BD0 + 0x1A1, op_on_psxvram_sub_4675B0_parent_call1);
    replace_call(ff8_externals.ssigpu_tx_select_2_sub_465CE0 + 0x281, op_on_psxvram_sub_4675B0_parent_call1);

    replace_call(ff8_externals.sub_464BD0 + 0x79, op_on_psxvram_sub_4675B0_parent_call2);
    replace_call(ff8_externals.sub_464BD0 + 0x1B5, op_on_psxvram_sub_4675B0_parent_call2);

    replace_call(uint32_t(ff8_externals.sub_464F70) + 0x2C5, read_vram_to_buffer1);
    replace_call(ff8_externals.sub_4653B0 + 0x9D, read_vram_to_buffer1);
}
