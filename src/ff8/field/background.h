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

#include <vector>

constexpr int TEXTURE_WIDTH_BYTES = 128; // Real texture width depends on the texture depth (bpp4 => 256, bpp8 => 128, bpp16 => 64)
constexpr int TEXTURE_WIDTH_BPP16 = 64;
constexpr int TEXTURE_WIDTH_BPP8 = 128;
constexpr int TEXTURE_WIDTH_BPP4 = 256;
constexpr int TEXTURE_HEIGHT = 256;
constexpr int VRAM_PAGE_MIM_MAX_COUNT = 13;
constexpr int MIM_DATA_WIDTH_BYTES = TEXTURE_WIDTH_BYTES * VRAM_PAGE_MIM_MAX_COUNT;
constexpr int MIM_DATA_HEIGHT = TEXTURE_HEIGHT;
constexpr int TILE_SIZE = 16;
constexpr int PALETTE_SIZE = 256;

struct Tile {
	int16_t x, y, z;
	uint16_t texID; // 2 bits = depth | 2 bits = blend | 1 bit = draw | 4 bits = textureID
	uint16_t palID; // 6 bits = Always 30 | 4 bits = PaletteID | 6 bits = Always 0
	uint8_t srcX, srcY;
	uint8_t layerID; // 0-7
	uint8_t blendType; // 0-4
	uint8_t parameter, state;
};

// A tile looks like another if it uses the same texture with the same palette and uses the same blending
bool ff8_background_tiles_looks_alike(const Tile &tile, const Tile &other);

std::vector<Tile> ff8_background_parse_tiles(const uint8_t *map_data);
bool ff8_background_save_textures(const std::vector<Tile> &tiles, const uint8_t *mim_data, const char *filename);
bool ff8_background_save_textures_legacy(const std::vector<Tile> &tiles, const uint8_t *mim_data, const char *filename);
