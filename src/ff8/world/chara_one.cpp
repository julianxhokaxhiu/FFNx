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

#include "chara_one.h"
#include "../../image/tim.h"
#include "../../log.h"

std::vector<CharaOneModelTextures> ff8_world_chara_one_parse_models(const uint8_t *chara_one_data, size_t size)
{
	std::vector<CharaOneModelTextures> models;

	const uint8_t *cur = chara_one_data + size, *min_cur = chara_one_data + 4;

	// The header is at the end of the file
	uint32_t count;
	cur -= 4;
	memcpy(&count, cur, 4);

	for (uint32_t i = 0; i < count && cur >= min_cur; ++i) {
		CharaOneModelTextures textures;

		uint32_t tim_offset;

		while (cur >= min_cur) {
			cur -= 4;
			memcpy(&tim_offset, cur, 4);

			if (int32_t(tim_offset) < 0) {
				break;
			}

			textures.push_back(tim_offset & 0xFFFFFFF);
		}

		models.push_back(textures);

		cur -= 4; // Skipping offset to the model data
	}

	return models;
}

bool ff8_world_chara_one_model_save_textures(const std::vector<CharaOneModelTextures> &models, const uint8_t *chara_one_model_data, const char *dirname)
{
	if (trace_all || trace_vram) ffnx_trace("%s: %s\n", __func__, dirname);

	int model_id = 0;
	for (const CharaOneModelTextures &textures: models) {
		int texture_id = 0;
		for (uint32_t texture_pointer: textures) {
			char name[MAX_PATH] = {};
			snprintf(name, sizeof(name), "%s/model%d-%d", dirname, model_id, texture_id);
			Tim tim = Tim::fromTimData(chara_one_model_data + texture_pointer);

			if (!tim.save(name, 0, 0, true)) {
				return false;
			}
			++texture_id;
		}
		++model_id;
	}

	return true;
}
