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

#include "stage.h"
#include "../../image/tim.h"
#include "../../saveload.h"
#include "../../log.h"

#include <set>
#include <unordered_map>

const uint8_t *ff8_battle_stage_search_model(const uint8_t *stage_data, size_t stage_data_size, std::list<uint32_t> &model_offsets)
{
    const uint32_t *stage_data_32 = reinterpret_cast<const uint32_t *>(stage_data);
    const uint32_t *stage_data_end = reinterpret_cast<const uint32_t *>(stage_data + stage_data_size);
    bool ok = false;

    // Looking for 00010001
    while (stage_data_32 < stage_data_end) {
        if (*stage_data_32 == 0x00010001) {
            const uint32_t *stage_data_32_2 = stage_data_32 - 1;

            // Parse model offsets list
            do {
                model_offsets.push_front(*stage_data_32_2);

                if (*stage_data_32_2 == (model_offsets.size() + 1) * 4 && *(stage_data_32_2 - 1) == model_offsets.size()) {
                    ok = true;
                    break;
                }

                --stage_data_32_2;
            } while (*stage_data_32_2 != 0 && *stage_data_32_2 >= *(stage_data_32_2 - 1) &&  *stage_data_32_2 < stage_data_size);

            if (ok) {
                break;
            }
        }

        ++stage_data_32;
    }

    if (!ok) {
        ffnx_warning("%s: models not found\n", __func__);

        return nullptr;
    }

    return reinterpret_cast<const uint8_t *>(stage_data_32 - model_offsets.size() - 1);
}

bool ff8_battle_stage_parse_geometry(const uint8_t *stage_data, size_t stage_data_size, Stage &stage)
{
    std::list<uint32_t> model_offsets;

    const uint8_t *models_section_start = ff8_battle_stage_search_model(stage_data + 0x500, stage_data_size - 0x500, model_offsets);

    if (models_section_start == nullptr) {
        return false;
    }

    do {
        const uint8_t *after_vertices = nullptr;

        for (uint32_t offset: model_offsets) {
            const uint8_t *model_section_start = models_section_start + offset;
            uint16_t vertice_count = *(uint16_t *)(model_section_start + 4);
            after_vertices = model_section_start + 6 + vertice_count * 6 + 4;
            uint32_t unknown1 = *(uint32_t *)(after_vertices - 4);

            // Padding
            after_vertices += (after_vertices - stage_data) % 4;
            uint16_t triangles_count = *(uint16_t *)after_vertices, quads_count = *(((uint16_t *)after_vertices) + 1);
            uint32_t unknown2 = *(uint32_t *)(after_vertices + 4);

            after_vertices += 8;

            for (int triangle_id = 0; triangle_id < triangles_count; ++triangle_id) {
                FF8StageTriangle triangle = FF8StageTriangle();

                memcpy(&triangle, after_vertices, sizeof(FF8StageTriangle));

                stage.triangles.push_back(triangle);

                after_vertices += sizeof(FF8StageTriangle);
            }

            for (int quad_id = 0; quad_id < quads_count; ++quad_id) {
                FF8StageQuad quad = FF8StageQuad();

                memcpy(&quad, after_vertices, sizeof(FF8StageQuad));

                stage.quads.push_back(quad);

                after_vertices += sizeof(FF8StageQuad);
            }
        }

        if (after_vertices == nullptr) {
            break;
        }

        models_section_start = nullptr;

        uint32_t next_models_count = *(uint32_t *)(after_vertices + 0x98);

        if (next_models_count < 65535) {
            const uint8_t *pos_next_model = after_vertices + 0x9C + next_models_count * sizeof(uint32_t);

            if (uint32_t(pos_next_model) > uint32_t(after_vertices) && pos_next_model - stage_data < stage_data_size && *(uint32_t *)pos_next_model == 0x00010001)
            {
                model_offsets.clear();
                models_section_start = ff8_battle_stage_search_model(pos_next_model, stage_data_size - (pos_next_model - stage_data), model_offsets);
            }
        }
    } while (models_section_start != nullptr);

    return true;
}

bool ff8_battle_state_save_texture(const Stage &stage, const Tim &tim, const char *filename)
{
    std::unordered_map<uint8_t, std::set<uint8_t> > palsPerTexture;

    // Group palettes by texture ids
    for (const FF8StageTriangle &triangle: stage.triangles) {
        palsPerTexture[triangle.tex_id & 0xF].insert((triangle.pal_id >> 6) & 0xF);
    }

    for (const FF8StageQuad &quad: stage.quads) {
        palsPerTexture[quad.tex_id & 0xF].insert((quad.pal_id >> 6) & 0xF);
    }

    std::vector<TimRect> rectangles;

    for (const std::pair<uint8_t, std::set<uint8_t> > &pair: palsPerTexture) {
        uint8_t texId = pair.first;

        if (pair.second.size() > 1) {
            // List areas with palette ids
            std::set<TimRect> rects;

            for (const FF8StageTriangle &triangle: stage.triangles) {
                if (texId != (triangle.tex_id & 0xF)) {
                    continue;
                }

                uint32_t x1, y1, x2, y2;

                x1 = std::min(std::min(triangle.u1, triangle.u2), triangle.u3);
                y1 = std::min(std::min(triangle.v1, triangle.v2), triangle.v3);
                x2 = std::max(std::max(triangle.u1, triangle.u2), triangle.u3);
                y2 = std::max(std::max(triangle.v1, triangle.v2), triangle.v3);

                uint8_t palId = (triangle.pal_id >> 6) & 0xF;
                TimRect rect(palId, x1, y1, x2, y2);
                if (rect.isValid()) {
                    rects.insert(rect);
                }
            }

            for (const FF8StageQuad &quad: stage.quads) {
                if (texId != (quad.tex_id & 0xF)) {
                    continue;
                }

                uint32_t x1, y1, x2, y2;

                x1 = std::min(std::min(std::min(quad.u1, quad.u2), quad.u3), quad.u4);
                y1 = std::min(std::min(std::min(quad.v1, quad.v2), quad.v3), quad.v4);
                x2 = std::max(std::max(std::max(quad.u1, quad.u2), quad.u3), quad.u4);
                y2 = std::max(std::max(std::max(quad.v1, quad.v2), quad.v3), quad.v4);

                uint8_t palId = (quad.pal_id >> 6) & 0xF;
                TimRect rect(palId, x1, y1, x2, y2);
                if (rect.isValid()) {
                    rects.insert(rect);
                }
            }

            // Merge areas together
            std::set<TimRect> rectsMerged;
            TimRect previousRect;

            while (true) {
                for (const TimRect &rect: rects) {
                    if (! previousRect.isValid()) {
                        previousRect = rect;

                        continue;
                    }

                    if (previousRect.palIndex == rect.palIndex
                            && previousRect.x2 == rect.x1
                            && previousRect.y1 == rect.y1 && previousRect.y2 == rect.y2) { // Same height, vertically aligned
                        previousRect.x2 = rect.x2; // Increase rect horizontally
                    } else if (previousRect.palIndex == rect.palIndex
                            && previousRect.x1 == rect.x1 && previousRect.x2 == rect.x2
                            && previousRect.y2 == rect.y1) { // Same width, horizontally aligned
                        previousRect.y2 = rect.y2; // Increase rect vertically
                    } else if (previousRect.palIndex == rect.palIndex
                            && previousRect.x1 <= rect.x1 && previousRect.x2 >= rect.x2
                            && previousRect.y1 <= rect.y1 && previousRect.y2 >= rect.y2) {
                        // Rect inside another
                    } else if (previousRect.palIndex == rect.palIndex
                            && previousRect.x1 >= rect.x1 && previousRect.x2 <= rect.x2
                            && previousRect.y1 >= rect.y1 && previousRect.y2 <= rect.y2) { // Rect inside another
                        previousRect = rect; // Replace by bigger rect
                    } else {
                        // Flush
                        rectsMerged.insert(previousRect);
                        previousRect = rect;
                    }
                }

                if (previousRect.isValid()) {
                    rectsMerged.insert(previousRect);
                }

                if (rects.size() == rectsMerged.size()) {
                    break;
                }

                rects = rectsMerged;
                rectsMerged.clear();
            }

            for (const TimRect &rect: rectsMerged) {
                rectangles.push_back(TimRect(rect.palIndex, texId * 128 + rect.x1, rect.y1, texId * 128 + rect.x2, rect.y2));
            }
        }

        // Use first palette by default
        rectangles.push_back(TimRect(*(pair.second.begin()), texId * 128, 0, texId * 128 + 128, 255));
    }

    tim.saveMultiPaletteTrianglesAndQuads(filename, rectangles, true);

    return true;
}
