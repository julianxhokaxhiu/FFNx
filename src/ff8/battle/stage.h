/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
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

#pragma once

#include "../../common.h"
#include "../../image/tim.h"

#include <vector>

struct FF8StageTriangle {
    uint16_t faceA, faceB, faceC;
    uint8_t u1, v1;
    uint8_t u2, v2;
    uint16_t pal_id; // 6 bits = Always 15 | 4 bits = PaletteID | 6 bits = Always 0
    uint8_t u3, v3;
    uint8_t tex_id;
    uint8_t hide;
    uint8_t red, green, blue;
    uint8_t instruction;
};

struct FF8StageQuad {
    uint16_t faceA, faceB, faceC, faceD;
    uint8_t u1, v1;
    uint16_t pal_id; // 6 bits = Always 15 | 4 bits = PaletteID | 6 bits = Always 0
    uint8_t u2, v2;
    uint8_t tex_id;
    uint8_t hide;
    uint8_t u3, v3;
    uint8_t u4, v4;
    uint8_t red, green, blue;
    uint8_t instruction;
};

struct Stage {
    std::vector<FF8StageTriangle> triangles;
    std::vector<FF8StageQuad> quads;
};

bool ff8_battle_stage_parse_geometry(const uint8_t *stage_data, size_t stage_data_size, Stage &stage);
bool ff8_battle_state_save_texture(const Stage &stage, const Tim &tim, const char *filename);
