/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
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

#include <unordered_map>

bool ff8_background_tiles_looks_alike(const Tile &tile, const Tile &other)
{
	return tile.texID == other.texID
		&& tile.palID == other.palID
		&& tile.srcX == other.srcX
		&& tile.srcY == other.srcY
		&& tile.blendType == other.blendType;
}

std::vector<Tile> ff8_background_parse_tiles(const uint8_t *map_data, int *maxW)
{
	std::vector<Tile> tiles;

	*maxW = 0;

	while (true) {
		Tile tile;

		memcpy(&tile, map_data, sizeof(Tile));

		if (tile.x == 0x7fff) {
			break;
		}

		uint8_t texture_id = tile.texID & 0xF;
		int maxX = (texture_id + 1) * TEXTURE_WIDTH_BPP16;
		if (maxX > *maxW) {
			*maxW = maxX;
		}

		if (trace_all || trace_vram) {
			Tim::Bpp bpp = Tim::Bpp((tile.texID >> 7) & 3);
			uint8_t pal_id = (tile.palID >> 6) & 0xF;

			ffnx_info("tile %d dst %d %d %d src %d %d texid %d bpp %d palId %d blendType %d param %d %d\n", tiles.size(), tile.x, tile.y, tile.z, tile.srcX, tile.srcY, texture_id, int(bpp), pal_id, tile.blendType, tile.parameter, tile.state);
		}

		tiles.push_back(tile);

		map_data += sizeof(Tile);
	}

	return tiles;
}

void ff8_background_tiles_to_map(const std::vector<Tile> &tiles, uint8_t *map_data)
{
	for (const Tile &tile: tiles) {
		memcpy(map_data, (const void *)&tile, sizeof(Tile));

		map_data += sizeof(Tile);
	}

	*(uint16_t *)map_data = 0x7fff;
}

void ff8_background_draw_tile(const Tile &tile, uint32_t *target, const uint16_t target_width, const uint8_t* const textures_data, const uint16_t* const palettes_data)
{
	Tim::Bpp bpp = Tim::Bpp((tile.texID >> 7) & 3);
	uint8_t texture_id = tile.texID & 0xF;
	uint8_t pal_id = (tile.palID >> 6) & 0xF;
	const uint8_t *texture_data_start = textures_data + texture_id * TEXTURE_WIDTH_BYTES + tile.srcY * MIM_DATA_WIDTH_BYTES;
	const uint16_t *palette_data_start = bpp == Tim::Bpp16 ? nullptr : palettes_data + pal_id * PALETTE_SIZE;

	if (bpp == Tim::Bpp16) {
		const uint16_t *texture_data = reinterpret_cast<const uint16_t *>(texture_data_start) + tile.srcX;

		for (int y = 0; y < TILE_SIZE; ++y) {
			for (int x = 0; x < TILE_SIZE; ++x) {
				*(target + x) = fromR5G5B5Color(*(texture_data + x), true);
			}

			target += target_width;
			texture_data += MIM_DATA_WIDTH_BYTES / 2;
		}
	} else if (bpp == Tim::Bpp8) {
		const uint8_t *texture_data = texture_data_start + tile.srcX;

		for (int y = 0; y < TILE_SIZE; ++y) {
			for (int x = 0; x < TILE_SIZE; ++x) {
				*(target + x) = fromR5G5B5Color(palette_data_start[*(texture_data + x)], true);
			}

			target += target_width;
			texture_data += MIM_DATA_WIDTH_BYTES;
		}
	} else {
		const uint8_t *texture_data = texture_data_start + tile.srcX / 2;

		for (int y = 0; y < TILE_SIZE; ++y) {
			for (int x = 0; x < TILE_SIZE / 2; ++x) {
				uint8_t index = *(texture_data + x);
				*(target + x * 2) = fromR5G5B5Color(palette_data_start[index & 0xF], true);
				*(target + x * 2 + 1) = fromR5G5B5Color(palette_data_start[index >> 4], true);
			}

			target += target_width;
			texture_data += MIM_DATA_WIDTH_BYTES;
		}
	}
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
		uint8_t row = tile_id / cols_count, col = tile_id % cols_count;
		uint32_t *target = image_data_start + row * TILE_SIZE * width + col * TILE_SIZE;

		ff8_background_draw_tile(tile, target, width, textures_data, palettes_data);

		++tile_id;
	}

	save_texture(image_data_start, image_data_size, width, TEXTURE_HEIGHT, uint32_t(-1), filename, false);

	delete[] image_data_start;

	return true;
}

bool ff8_background_save_textures_legacy(const std::vector<Tile> &tiles, const uint8_t *mim_data, const char *filename)
{
	if (trace_all || trace_vram) ffnx_trace("%s %s\n", __func__, filename);

	std::unordered_map<uint16_t, Tile> tiles_per_position_in_texture, pal_conflicts;
	std::unordered_map<uint8_t, std::vector<uint8_t>> texture_ids;

	const uint16_t *palettes_data = reinterpret_cast<const uint16_t *>(mim_data + 0x1000);
	const uint8_t *textures_data = mim_data + 0x3000;

	for (const Tile &tile: tiles) {
		uint8_t texture_id = tile.texID & 0xF, pal_id = (tile.texID >> 6) & 0xF;
		Tim::Bpp bpp = Tim::Bpp((tile.texID >> 7) & 3);

		ffnx_info("dst %d %d %d src %d %d texid %d bpp %d palid %d blendType %d\n", tile.x, tile.y, tile.z, tile.srcX, tile.srcY, texture_id, int(bpp), pal_id, tile.blendType);
		texture_ids.insert(std::pair<uint8_t, std::vector<uint8_t>>(texture_id, std::vector<uint8_t>()));
		texture_ids[texture_id].push_back(0xFF);

		uint16_t key = texture_id | ((tile.srcX / 16) << 4) | ((tile.srcY / 16) << 8);
		if (tiles_per_position_in_texture.contains(key)) {
			if (tiles_per_position_in_texture[key].palID == pal_id || pal_conflicts.contains(key | (pal_id << 12))) {
				ffnx_warning("Tile conflict\n");
			} else {
				pal_conflicts[key | (pal_id << 12)] = tile;
				texture_ids[texture_id].push_back(pal_id);
			}
		} else {
			tiles_per_position_in_texture[key] = tile;
		}
	}

	uint32_t* const image_data_start = new uint32_t[TEXTURE_WIDTH_BPP4 * TEXTURE_HEIGHT];

	if (image_data_start == nullptr) {
		return false;
	}

	const uint32_t image_data_size = TEXTURE_WIDTH_BPP4 * TEXTURE_HEIGHT * sizeof(uint32_t);

	// Save textures
	for (const std::pair<uint8_t, std::vector<uint8_t>> pair: texture_ids) {
		const uint8_t texture_id = pair.first;

		for (const uint8_t pal_id: pair.second) {
			const std::unordered_map<uint16_t, Tile> &tiles = pal_id == 0xFF ? tiles_per_position_in_texture : pal_conflicts;
			const uint16_t key = texture_id | (pal_id == 0xFF ? 0 : (pal_id << 12));

			// Fill with zeroes (transparent image)
			memset(image_data_start, 0, image_data_size);

			for (uint8_t row = 0; row < 16; ++row) {
				for (uint8_t col = 0; col < 16; ++col) {
					uint32_t *target = image_data_start + row * TILE_SIZE * TEXTURE_WIDTH_BPP4 + col * TILE_SIZE;
					auto it = tiles.find(key | (col << 4) | (row << 8));
					if (it == tiles.end()) {
						if (pal_id == 0xFF) {
							ffnx_info("texture_id=%d row=%d col=%d pal_id=%d not found\n", texture_id, row, col, pal_id);
							continue;
						}

						it = tiles_per_position_in_texture.find(texture_id | (col << 4) | (row << 8));
						if (it == tiles_per_position_in_texture.end()) {
							ffnx_info("texture_id=%d row=%d col=%d pal_id=%d not found\n", texture_id, row, col, pal_id);
							continue;
						}
					}

					ff8_background_draw_tile(it->second, target, TEXTURE_WIDTH_BPP4, textures_data, palettes_data);

					ffnx_info("texture_id=%d row=%d col=%d pal_id=%d\n", texture_id, row, col, pal_id);
				}
			}

			char filename_tex[MAX_PATH] = {};

			if (pal_id == 0xFF) {
				snprintf(filename_tex, sizeof(filename_tex), "%s_%d", filename, texture_id);
			} else {
				snprintf(filename_tex, sizeof(filename_tex), "%s_%d_0_%d", filename, texture_id, pal_id + 240);
			}
			save_texture(image_data_start, image_data_size, TEXTURE_WIDTH_BPP4, TEXTURE_HEIGHT, -1, filename_tex, false);
		}
	}

	delete[] image_data_start;

	return true;
}
