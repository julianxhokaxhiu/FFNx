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

#include "wmset.h"
#include "../../image/tim.h"
#include "../../saveload.h"
#include "../../log.h"

std::vector<WmsetSection17Texture> ff8_world_wmset_wave_animations_parse(const uint8_t *wmset_section17_data, size_t size)
{
	std::vector<WmsetSection17Texture> textures;

	const uint32_t *header = (const uint32_t *)wmset_section17_data;
	size_t max = size / 4, i = 0;

	for (int i = 0; i < max; ++i) {
		uint32_t pos = header[i];

		if (pos == 0 || pos + 12 >= size) {
			break;
		}

		max = std::min(max, pos / 4);

		WmsetSection17Texture texture = WmsetSection17Texture();
		const uint16_t *vram_positions = (const uint16_t *)(wmset_section17_data + pos + 4);

		texture.x = vram_positions[0];
		texture.y = vram_positions[1];

		const uint32_t *secondary_header = (const uint32_t *)(wmset_section17_data + pos + 8);
		size_t max2 = (size - pos - 8) / 4;
		uint32_t previous_frame_pos = 0;

		for (int j = 0; j < max2; ++j) {
			uint32_t frame_pos = secondary_header[j];

			if (frame_pos == previous_frame_pos || pos + 8 + frame_pos >= size - 8 || pos + 8 + frame_pos == header[i + 1]) {
				break;
			}

			max2 = std::min(max2, frame_pos / 4);
			previous_frame_pos = frame_pos;

			texture.textureFramePositions.push_back(wmset_section17_data + pos + 8 + frame_pos);
		}

		textures.push_back(texture);
	}

	return textures;
}

bool ff8_world_wmset_wave_animations_save_texture(const std::vector<WmsetSection17Texture> &textures, int texture_id, const char *dirname, const Tim &baseTexture)
{
	if (trace_all || trace_vram) ffnx_trace("%s: %s\n", __func__, dirname);

	if (texture_id >= textures.size()) {
		return false;
	}

	int frame_id = 0;
	std::vector<const uint8_t *> positions = textures.at(texture_id).textureFramePositions;
	for (const uint8_t *texture_pointer: positions) {
		char name[MAX_PATH] = {};
		snprintf(name, sizeof(name), "%s/texture%d", dirname, texture_id);
		Tim tim = Tim::fromTimData(texture_pointer); // Indexed Tim without palette

		ff8_tim mergedTim = ff8_tim();
		mergedTim.img_x = tim.imageX();
		mergedTim.img_y = tim.imageY();
		mergedTim.img_w = tim.imageWidth();
		mergedTim.img_h = tim.imageHeight();
		mergedTim.img_data = tim.imageData();
		// Use palette from baseTexture
		mergedTim.pal_x = baseTexture.paletteX();
		mergedTim.pal_y = baseTexture.paletteY();
		mergedTim.pal_w = baseTexture.paletteWidth();
		mergedTim.pal_h = baseTexture.paletteHeight();
		mergedTim.pal_data = baseTexture.paletteData();

		if (!Tim(tim.bpp(), mergedTim).save(name, frame_id, true)) {
			return false;
		}
		++frame_id;
	}

	return true;
}

std::unordered_map<uint32_t, WmsetSection41Texture> ff8_world_wmset_palette_animations_parse(const uint8_t *wmset_section41_data, size_t size)
{
	std::unordered_map<uint32_t, WmsetSection41Texture> textures;

	const uint32_t *header = (const uint32_t *)wmset_section41_data;
	size_t max = size / 4, i = 0;

	for (int i = 0; i < max; ++i) {
		uint32_t pos = header[i];

		if (pos == 0 || pos + 12 >= size) {
			break;
		}

		max = std::min(max, pos / 4);

		WmsetSection41Texture texture = WmsetSection41Texture();
		const uint16_t *vram_positions = (const uint16_t *)(wmset_section41_data + pos + 4);

		texture.height = wmset_section41_data[pos + 2];
		texture.srcX = vram_positions[0];
		texture.srcY = vram_positions[1];
		texture.x = vram_positions[2];
		texture.y = vram_positions[3];

		const uint32_t *toc = (const uint32_t *)(wmset_section41_data + pos + 12);

		for (int j = 0; j < texture.height; ++j) {
			texture.palettePositions.push_back((const uint16_t *)(wmset_section41_data + pos + 12 + toc[j] + 20));
		}

		ffnx_info("%s: height=%d src=(%d, %d) pos=(%d, %d)\n", __func__, texture.height, texture.srcX, texture.srcY, texture.x, texture.y);

		textures[uint32_t(texture.srcX) | (uint32_t(texture.srcY) << 16)] = texture;
	}

	return textures;
}

bool ff8_world_wmset_palette_animations_save_texture(const WmsetSection41Texture &texture, const char *dirname, const Tim &baseTexture)
{
	if (trace_all || trace_vram) ffnx_trace("%s: %s\n", __func__, dirname);

	std::vector<const uint16_t *> positions = texture.palettePositions;
	int palette_id = 0;
	for (const uint16_t *texture_pointer: positions) {
		ff8_tim mergedTim = ff8_tim();
		mergedTim.img_x = baseTexture.imageX();
		mergedTim.img_y = baseTexture.imageY();
		mergedTim.img_w = baseTexture.imageWidth();
		mergedTim.img_h = baseTexture.imageHeight();
		mergedTim.img_data = baseTexture.imageData();
		// Use palette from baseTexture
		mergedTim.pal_x = texture.x;
		mergedTim.pal_y = texture.y;
		mergedTim.pal_w = 256;
		mergedTim.pal_h = 1;
		mergedTim.pal_data = (uint16_t *)texture_pointer;

		if (!Tim(baseTexture.bpp(), mergedTim).save(dirname, palette_id, true)) {
			return false;
		}
		++palette_id;
	}

	return true;
}
