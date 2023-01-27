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

#include "background.h"
#include "../../image/tim.h"
#include "../../saveload.h"
#include "../../log.h"

bool ff8_background_tiles_looks_alike(const Tile &tile, const Tile &other)
{
	return tile.texID == other.texID
		&& tile.palID == other.palID
		&& tile.srcX == other.srcX
		&& tile.srcY == other.srcY
		&& tile.blendType == other.blendType;
}

std::vector<Tile> ff8_background_parse_tiles(const uint8_t *map_data)
{
	std::vector<Tile> tiles;

	while (true) {
		Tile tile;

		memcpy(&tile, map_data, sizeof(Tile));

		if (tile.x == 0x7fff) {
			break;
		}

		uint8_t texture_id = tile.texID & 0xF;
		Tim::Bpp bpp = Tim::Bpp((tile.texID >> 7) & 3);
		uint8_t pal_id = (tile.palID >> 6) & 0xF;

		if (trace_all || trace_vram) ffnx_info("tile %d dst %d %d %d src %d %d texid %d bpp %d palId %d blendType %d param %d %d\n", tiles.size(), tile.x, tile.y, tile.z, tile.srcX, tile.srcY, texture_id, int(bpp), pal_id, tile.blendType, tile.parameter, tile.state);

		tiles.push_back(tile);

		map_data += sizeof(Tile);
	}

	return tiles;
}

bool ff8_background_save_textures(const std::vector<Tile> &tiles, const uint8_t *mim_data, const char *filename)
{
	if (trace_all || trace_vram) ffnx_trace("%s %s\n", __func__, filename);

	const uint16_t* const palettes_data = reinterpret_cast<const uint16_t *>(mim_data + 0x1000);
	const uint8_t* const textures_data = mim_data + 0x3000;

	const uint8_t cols_count = tiles.size() / (TEXTURE_HEIGHT / TILE_SIZE) + int(tiles.size() % (TEXTURE_HEIGHT / TILE_SIZE) != 0);
	const uint16_t width = cols_count * TILE_SIZE;
	const uint32_t image_data_size = width * TEXTURE_HEIGHT * sizeof(uint32_t);

	uint32_t* const image_data_start = new uint32_t[width * TEXTURE_HEIGHT];

	if (image_data_start == nullptr) {
		return false;
	}

	// Fill with zeroes (transparent image)
	memset(image_data_start, 0, image_data_size);

	uint32_t tile_id = 0;

	for (const Tile &tile: tiles) {
		Tim::Bpp bpp = Tim::Bpp((tile.texID >> 7) & 3);
		uint8_t texture_id = tile.texID & 0xF;
		uint8_t pal_id = (tile.palID >> 6) & 0xF;
		const uint8_t *texture_data_start = textures_data + texture_id * TEXTURE_WIDTH_BYTES + tile.srcY * MIM_DATA_WIDTH_BYTES;
		const uint16_t *palette_data_start = bpp == Tim::Bpp16 ? nullptr : palettes_data + pal_id * PALETTE_SIZE;
		uint8_t row = tile_id / cols_count, col = tile_id % cols_count;
		uint32_t *target = image_data_start + row * TILE_SIZE * width;

		if (bpp == Tim::Bpp16) {
			const uint16_t *texture_data = reinterpret_cast<const uint16_t *>(texture_data_start) + tile.srcX;
			target += col * TILE_SIZE;

			for (int y = 0; y < TILE_SIZE; ++y) {
				for (int x = 0; x < TILE_SIZE; ++x) {
					*(target + x) = fromR5G5B5Color(*(texture_data + x), true);
				}

				target += width;
				texture_data += MIM_DATA_WIDTH_BYTES / 2;
			}
		} else if (bpp == Tim::Bpp8) {
			const uint8_t *texture_data = texture_data_start + tile.srcX;
			target += col * TILE_SIZE;

			for (int y = 0; y < TILE_SIZE; ++y) {
				for (int x = 0; x < TILE_SIZE; ++x) {
					*(target + x) = fromR5G5B5Color(palette_data_start[*(texture_data + x)], true);
				}

				target += width;
				texture_data += MIM_DATA_WIDTH_BYTES;
			}
		} else {
			const uint8_t *texture_data = texture_data_start + tile.srcX / 2;
			target += col * TILE_SIZE;

			for (int y = 0; y < TILE_SIZE; ++y) {
				for (int x = 0; x < TILE_SIZE / 2; ++x) {
					uint8_t index = *(texture_data + x);
					*(target + x * 2) = fromR5G5B5Color(palette_data_start[index & 0xF], true);
					*(target + x * 2 + 1) = fromR5G5B5Color(palette_data_start[index >> 4], true);
				}

				target += width;
				texture_data += MIM_DATA_WIDTH_BYTES;
			}
		}

		++tile_id;
	}

	save_texture(image_data_start, image_data_size, width, TEXTURE_HEIGHT, uint32_t(-1), filename, false);

	delete[] image_data_start;

	return true;
}
