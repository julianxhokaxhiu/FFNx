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

#include <stdint.h>
#include "../ff8.h"

inline uint32_t fromR5G5B5Color(uint16_t color)
{
    uint8_t r = color & 31,
            g = (color >> 5) & 31,
            b = (color >> 10) & 31;

    return (0xffu << 24) |
        ((((r << 3) + (r >> 2)) & 0xffu) << 16) |
        ((((g << 3) + (g >> 2)) & 0xffu) << 8) |
        (((b << 3) + (b >> 2)) & 0xffu);
}

bool tim_to_rgba32(uint32_t *target, uint8_t bpp, uint8_t *img_data, uint16_t img_w, uint16_t img_h, uint16_t *pal_data = nullptr);
void save_tim(const char *fileName, uint8_t bpp, ff8_tim *tim_infos, uint32_t palette_index = 0);
void save_lzs(const char *fileName, uint8_t *uncompressed_data);
