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
#include <unordered_map>

struct WmsetSection17Texture {
    uint16_t x, y;
    std::vector<const uint8_t *> textureFramePositions;
};

struct WmsetSection41Texture {
    uint16_t srcX, srcY, x, y;
    uint8_t height;
    std::vector<const uint16_t *> palettePositions;
};

std::vector<WmsetSection17Texture> ff8_world_wmset_wave_animations_parse(const uint8_t *wmset_section17_data, size_t size);
bool ff8_world_wmset_wave_animations_save_texture(const std::vector<WmsetSection17Texture> &textures, int texture_id, const char *dirname, const Tim &baseTexture);

std::unordered_map<uint32_t, WmsetSection41Texture> ff8_world_wmset_palette_animations_parse(const uint8_t *wmset_section41_data, size_t size);
bool ff8_world_wmset_palette_animations_save_texture(const WmsetSection41Texture &texture, const char *dirname, const Tim &baseTexture);
