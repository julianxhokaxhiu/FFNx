/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

std::unordered_map<uint32_t, CharaOneModel> ff8_chara_one_parse_models(const uint8_t *chara_one_data, size_t size)
{
	std::unordered_map<uint32_t, CharaOneModel> models;

	const uint8_t *cur = chara_one_data;

	uint32_t count;
	memcpy(&count, cur, 4);
	cur += 4;

	for (uint32_t i = 0; i < count && cur - chara_one_data < size - 16; ++i) {
		uint32_t offset;
		memcpy(&offset, cur, 4);
		cur += 4;

		if (offset == 0) {
			break;
		}

		uint32_t section_size;
		memcpy(&section_size, cur, 4);
		cur += 4;

		uint32_t flag;
		memcpy(&flag, cur, 4);
		cur += 4;

		if (flag == section_size) {
			memcpy(&flag, cur, 4);
			cur += 4;
		}

		CharaOneModel model = CharaOneModel();
		model.modelId = i;

		if (flag >> 24 != 0xd0) { // NPCs (not main characters)
			uint32_t tim_offset;

			if (trace_all) ffnx_info("%s: %d %d %d\n", __func__, i, int(cur - chara_one_data), size);

			if ((flag & 0xFFFFFF) == 0) {
				model.texturesData.push_back(0);
			}

			while (cur - chara_one_data < size) {
				memcpy(&tim_offset, cur, 4);
				cur += 4;

				if (tim_offset == 0xFFFFFFFF) {
					break;
				}

				model.texturesData.push_back(tim_offset & 0xFFFFFF);
			}
		} else {
			model.isMch = true;
		}

		if (cur - chara_one_data < 16) {
			break;
		}

		cur += 4;
		char name[5] = "";
		for (uint8_t j = 0; j < 4; ++j) {
			if (cur[j] > 'z' || cur[j] < '0') {
				name[j] = '\0';
				break;
			}

			name[j] = cur[j];
		}
		strncpy(model.name, name, 4);

		models[offset + 4] = model;

		cur += 12;
	}

	return models;
}

void ff8_parse_pcb(const uint8_t *pcb_data, int pcb_data_size, std::unordered_map<uint32_t, CharaOneModel> &models)
{
	int count = std::min(pcb_data_size / 8, 64);
	std::unordered_map<uint32_t, uint32_t> model_light_colors;
	uint32_t default_light_color = 0x80808080;

	for (int i = 0; i < count; ++i)
	{
		char name[5] = "";
		for (uint8_t j = 0; j < 4; ++j)
		{
			name[j] = pcb_data[i * 8 + j];

			if (name[j] > 'z' || name[j] < '0')
			{
				name[j] = '\0';
				break;
			}
		}

		model_light_colors[*(uint32_t *)name] = *(uint32_t *)(pcb_data + i * 8 + 4);

		if (i == 0) {
			default_light_color = *(uint32_t *)(pcb_data + i * 8 + 4);
		}
	}

	// Set color modifiers to models
	std::unordered_map<uint32_t, CharaOneModel> ret;

	for (const auto pair: models)
	{
		CharaOneModel model = pair.second;
		auto it = model_light_colors.find(*(uint32_t *)model.name);
		uint32_t raw_color = it == model_light_colors.end() ? default_light_color : (*it).second;
		uint8_t *color_modifier = (uint8_t *)&raw_color;

		model.rgbaModifier[0] = int(color_modifier[1]) - 128;
		model.rgbaModifier[1] = int(color_modifier[2]) - 128;
		model.rgbaModifier[2] = int(color_modifier[3]) - 128;
		model.rgbaModifier[3] = 0;

		ret[pair.first] = model;
	}

	models = ret;
}

void ff8_mch_parse_model(CharaOneModel &model, const uint8_t *mch_data, size_t size)
{
	if(size < 0x100) {
		ffnx_warning("%s: empty MCH\n", __func__);
		return;
	}

	const uint8_t *cur = mch_data;
	uint32_t tim_offset = 0;

	while (cur - mch_data < 0x100 - 4) {
		memcpy(&tim_offset, cur, 4);
		cur += 4;

		if(tim_offset == 0xFFFFFFFF) {
			break;
		}

		model.texturesData.push_back(tim_offset & 0xFFFFFF);
	}
}

bool ff8_chara_one_model_save_textures(const CharaOneModel &model, const uint8_t *chara_one_model_data, const char *dirname)
{
	if (trace_all || trace_vram) ffnx_trace("%s: %s\n", __func__, dirname);

	int texture_id = 0;
	for (uint32_t texture_pointer: model.texturesData) {
		char name[MAX_PATH] = {};
		snprintf(name, sizeof(name), "%s/%s-%d", dirname, model.name, texture_id);
		Tim tim = Tim::fromTimData(chara_one_model_data + texture_pointer);

		if (!tim.save(name, 0, 0, true)) {
			return false;
		}
		++texture_id;
	}

	return true;
}
