/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
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

#include "../../log.h"
#include "../../common.h"
#include "../../globals.h"
#include "../widescreen.h"

#include "../../renderer.h"

#include "background.h"
#include "defs.h"
#include "utils.h"
#include "camera.h"

#include <functional>

namespace ff7::field
{
    constexpr float MIN_STEP_INVERSE = 10.f;

    // ##################################################################
    // ----------------- DRAW GRAPHICS RELATED --------------------------
    // ##################################################################

    void field_layer1_pick_tiles(short bg_position_x, short bg_position_y)
    {
        int field_bg_multiplier = *ff7_externals.field_bg_multiplier;
        vector2<float> bg_position, initial_pos, tile_position;
        field_tile* layer1_tiles = *ff7_externals.field_layer1_tiles;

        bg_position.x = bg_position_x;
        bg_position.y = bg_position_y;
        if(ff7_fps_limiter >= FPS_LIMITER_30FPS)
        {
            if(is_position_valid(bg_main_layer_pos))
            {
                bg_position.x = bg_main_layer_pos.x;
                bg_position.y = bg_main_layer_pos.y;
            }
        }

        initial_pos.x = field_bg_multiplier * (320 - bg_position.x);
        initial_pos.y = field_bg_multiplier * ((ff7_field_center ? 232 : 224) - bg_position.y);

        if(*ff7_externals.field_special_y_offset > 0 && bg_position.y <= 6)
            initial_pos.y -= field_bg_multiplier * (*ff7_externals.field_special_y_offset);

        for(int i = 0; i < *ff7_externals.field_layer1_tiles_num; i++)
        {
            uint32_t tile_index = (*ff7_externals.field_layer1_palette_sort)[i];
            layer1_tiles[tile_index].field_1044 = 1;

            tile_position.x = initial_pos.x + field_bg_multiplier * layer1_tiles[tile_index].x;
            tile_position.y = initial_pos.y + field_bg_multiplier * layer1_tiles[tile_index].y;
            ff7_externals.add_page_tile(tile_position.x, tile_position.y, 0.9997, layer1_tiles[tile_index].u,
                                        layer1_tiles[tile_index].v, layer1_tiles[tile_index].palette_index, layer1_tiles[tile_index].page);
        }
    }

    void field_layer2_pick_tiles(short bg_position_x, short bg_position_y)
    {
        int field_bg_multiplier = *ff7_externals.field_bg_multiplier;
        field_tile *layer2_tiles = *ff7_externals.field_layer2_tiles;
        vector2<float> bg_position, initial_pos;

        bg_position.x = bg_position_x;
        bg_position.y = bg_position_y;
        if(ff7_fps_limiter >= FPS_LIMITER_30FPS)
        {
            if(is_position_valid(bg_main_layer_pos))
            {
                bg_position.x = bg_main_layer_pos.x;
                bg_position.y = bg_main_layer_pos.y;
            }
        }

        initial_pos.x = (320 - bg_position.x) * field_bg_multiplier;
        initial_pos.y = ((ff7_field_center ? 232 : 224) - bg_position.y) * field_bg_multiplier;
        if(*ff7_externals.field_special_y_offset > 0 && bg_position.y <= 8)
            initial_pos.y -= (*ff7_externals.field_special_y_offset) * field_bg_multiplier;

        for(int i = 0; i < *ff7_externals.field_layer2_tiles_num; i++)
        {
            uint32_t tile_index = (*ff7_externals.field_layer2_palette_sort)[i];
            vector2<float> tile_position;

            char anim_group = layer2_tiles[tile_index].anim_group;
            if(anim_group && !(ff7_externals.modules_global_object->background_sprite_layer[anim_group] & layer2_tiles[tile_index].anim_bitmask))
                continue;

            layer2_tiles[tile_index].field_1040 = 1;

            tile_position.x = layer2_tiles[tile_index].x * field_bg_multiplier + initial_pos.x;
            tile_position.y = layer2_tiles[tile_index].y * field_bg_multiplier + initial_pos.y;

            uint32_t page = (layer2_tiles[tile_index].use_fx_page) ? layer2_tiles[tile_index].fx_page : layer2_tiles[tile_index].page;

            if(layer2_tiles[tile_index].use_fx_page && layer2_tiles[tile_index].blend_mode == 2) page += 14;
            if(layer2_tiles[tile_index].use_fx_page && layer2_tiles[tile_index].blend_mode == 3) page += 18;

            ff7_externals.add_page_tile(tile_position.x, tile_position.y, layer2_tiles[tile_index].z, layer2_tiles[tile_index].u,
                                        layer2_tiles[tile_index].v, layer2_tiles[tile_index].palette_index, page);
        }
    }

    void field_layer3_shift_tile_position(vector2<float>* tile_position, vector2<float>* bg_position, int layer3_width, int layer3_height)
    {
        const int left_offset = 352 + (is_fieldmap_wide() ? abs(wide_viewport_x) : 0);
        const int right_offset = is_fieldmap_wide() ? abs(wide_viewport_x) : 0;
        const int top_offset = 256 + (widescreen_enabled ? 8 : 0);
        const int bottom_offset = widescreen_enabled ? 8 : 0;
        const int half_width = is_fieldmap_wide() ? ceil(wide_viewport_width / 4) : 160;
        const int half_height = widescreen_enabled ? 120 : 112;

        if(tile_position->x <= bg_position->x - left_offset || tile_position->x >= bg_position->x + right_offset)
            tile_position->x += (tile_position->x >= bg_position->x - half_width) ? -layer3_width : layer3_width;

        if(tile_position->y <= bg_position->y - top_offset || tile_position->y >= bg_position->y + bottom_offset)
            tile_position->y += (tile_position->y >= bg_position->y - half_height) ? -layer3_height : layer3_height;
    }

    void field_layer3_pick_tiles(short bg_position_x, short bg_position_y)
    {
        if(!*ff7_externals.do_draw_layer3_CFFE3C)
            return;

        float z_value;
        int field_bg_multiplier = *ff7_externals.field_bg_multiplier;
        field_tile *layer3_tiles = *ff7_externals.field_layer3_tiles;
        vector2<float> bg_position, initial_pos;

        bg_position.x = bg_position_x;
        bg_position.y = bg_position_y;
        if(ff7_fps_limiter >= FPS_LIMITER_30FPS)
        {
            if(is_position_valid(bg_layer3_pos))
            {
                bg_position.x = bg_layer3_pos.x;
                bg_position.y = bg_layer3_pos.y;
            }
        }

        initial_pos.x = (320 - bg_position.x) * field_bg_multiplier;
        initial_pos.y = ((ff7_field_center ? 232 : 224) - bg_position.y) * field_bg_multiplier;
        if(ff7_externals.modules_global_object->field_B0 < 0xFFF)
            z_value = ff7_externals.field_layer_sub_623C0F(ff7_externals.field_camera_rotation_matrix_CFF3D8, ff7_externals.modules_global_object->field_B0, 0, 0);
        else
            z_value = 0.9998;

        const bool do_increase_height = widescreen_enabled;
        const bool do_increase_width = is_fieldmap_wide() && (*ff7_externals.field_triggers_header)->bg3_width < ceil(wide_viewport_width / 2);
        const int layer3_width = (*ff7_externals.field_triggers_header)->bg3_width * (do_increase_width ? 2 : 1);
        const int layer3_height = (*ff7_externals.field_triggers_header)->bg3_height * (do_increase_height ? 2 : 1);
        const int left_offset = 352 + (is_fieldmap_wide() ? abs(wide_viewport_x) : 0);
        const int right_offset = is_fieldmap_wide() ? abs(wide_viewport_x) : 0;
        const int top_offset = 256 + (widescreen_enabled ? 8 : 0);
        const int bottom_offset = widescreen_enabled ? 8 : 0;

        for(int i = 0; i < *ff7_externals.field_layer3_tiles_num; i++)
        {
            uint32_t tile_index = (*ff7_externals.field_layer3_palette_sort)[i];
            vector2<float> tile_position = {
                static_cast<float>(layer3_tiles[tile_index].x),
                static_cast<float>(layer3_tiles[tile_index].y)
            };

            field_layer3_shift_tile_position(&tile_position, &bg_position, layer3_width, layer3_height);

            char anim_group = layer3_tiles[tile_index].anim_group;
            if(tile_position.x <= bg_position.x - left_offset || tile_position.x >= bg_position.x + right_offset ||
                tile_position.y <= bg_position.y - top_offset || tile_position.y >= bg_position.y + bottom_offset ||
                (anim_group && !(ff7_externals.modules_global_object->background_sprite_layer[anim_group] & layer3_tiles[tile_index].anim_bitmask)))
                continue;

            layer3_tiles[tile_index].field_1040 = 1;
            tile_position.x = tile_position.x * field_bg_multiplier + initial_pos.x;
            tile_position.y = tile_position.y * field_bg_multiplier + initial_pos.y;

            uint32_t page = (layer3_tiles[tile_index].use_fx_page) ? layer3_tiles[tile_index].fx_page : layer3_tiles[tile_index].page;

            ff7_externals.add_page_tile(tile_position.x, tile_position.y, z_value, layer3_tiles[tile_index].u,
                                        layer3_tiles[tile_index].v, layer3_tiles[tile_index].palette_index, page);
        }

        if(widescreen_enabled)
        {
            // Apply repeat x-y for background layer 4 tiles
            std::vector<vector2<int>> tile_offsets;
            if(do_increase_height)
                tile_offsets.push_back(vector2<int>{0, layer3_height /2});

            if(do_increase_width){
                tile_offsets.push_back(vector2<int>{layer3_width / 2, 0});
                tile_offsets.push_back(vector2<int>{layer3_width / 2, layer3_height / 2});
            }

            for(vector2<int> tile_offset: tile_offsets)
            {
                for(int i = 0; i < *ff7_externals.field_layer3_tiles_num; i++)
                {
                    uint32_t tile_index = (*ff7_externals.field_layer3_palette_sort)[i];
                    vector2<float> tile_position = {
                        static_cast<float>(layer3_tiles[tile_index].x + tile_offset.x),
                        static_cast<float>(layer3_tiles[tile_index].y + tile_offset.y)
                    };

                    field_layer3_shift_tile_position(&tile_position, &bg_position, layer3_width, layer3_height);

                    char anim_group = layer3_tiles[tile_index].anim_group;
                    if(tile_position.x <= bg_position.x - left_offset || tile_position.x >= bg_position.x + right_offset ||
                        tile_position.y <= bg_position.y - top_offset || tile_position.y >= bg_position.y + bottom_offset ||
                        (anim_group && !(ff7_externals.modules_global_object->background_sprite_layer[anim_group] & layer3_tiles[tile_index].anim_bitmask)))
                        continue;

                    layer3_tiles[tile_index].field_1040 = 1;
                    tile_position.x = tile_position.x * field_bg_multiplier + initial_pos.x;
                    tile_position.y = tile_position.y * field_bg_multiplier + initial_pos.y;

                    uint32_t page = (layer3_tiles[tile_index].use_fx_page) ? layer3_tiles[tile_index].fx_page : layer3_tiles[tile_index].page;

                    ff7_externals.add_page_tile(tile_position.x, tile_position.y, z_value, layer3_tiles[tile_index].u,
                                                layer3_tiles[tile_index].v, layer3_tiles[tile_index].palette_index, page);
                }
            }
        }
        *ff7_externals.field_layer3_flag_CFFE40 = 1;
    }

    void field_layer4_shift_tile_position(vector2<float>* tile_position, vector2<float>* bg_position, int layer4_width, int layer4_height)
    {
        const int left_offset = 352 + (is_fieldmap_wide() ? abs(wide_viewport_x) : 0);
        const int right_offset = is_fieldmap_wide() ? abs(wide_viewport_x) : 0;
        const int top_offset = 256 + (widescreen_enabled ? 8 : 0);
        const int bottom_offset = widescreen_enabled ? 8 : 0;
        const int half_width = is_fieldmap_wide() ? ceil(wide_viewport_width / 4) : 160;

        if(tile_position->x <= bg_position->x - left_offset || tile_position->x >= bg_position->x + right_offset)
            tile_position->x += (tile_position->x >= bg_position->x - half_width) ? -layer4_width : layer4_width;

        if(tile_position->y <= bg_position->y - top_offset || tile_position->y >= bg_position->y + bottom_offset)
            tile_position->y += (tile_position->y >= bg_position->y + bottom_offset) ? -layer4_height : layer4_height;

        if(widescreen_enabled && is_fieldmap_wide())
        {
            tile_position->x -= widescreen.getHorizontalOffset();
        }
    }

    void field_layer4_pick_tiles(short bg_position_x, short bg_position_y)
    {
        if(*ff7_externals.do_draw_layer4_CFFEA4)
        {
            int field_bg_multiplier = *ff7_externals.field_bg_multiplier;
            field_tile *layer4_tiles = *ff7_externals.field_layer4_tiles;
            vector2<float> bg_position, initial_pos;

            bg_position.x = bg_position_x;
            bg_position.y = bg_position_y;
            if(ff7_fps_limiter >= FPS_LIMITER_30FPS)
            {
                if(is_position_valid(bg_layer4_pos))
                {
                    bg_position.x = bg_layer4_pos.x;
                    bg_position.y = bg_layer4_pos.y;
                }
            }

            initial_pos.x = (320 - bg_position.x) * field_bg_multiplier;
            initial_pos.y = ((ff7_field_center ? 232 : 224) - bg_position.y) * field_bg_multiplier;
            float z_value = ff7_externals.field_layer_sub_623C0F(ff7_externals.field_camera_rotation_matrix_CFF3D8, ff7_externals.modules_global_object->field_AE, 0, 0);

            const bool do_increase_height = widescreen_enabled;
            const bool do_increase_width = is_fieldmap_wide() && (*ff7_externals.field_triggers_header)->bg4_width < ceil(wide_viewport_width / 2);
            const int layer4_width = (*ff7_externals.field_triggers_header)->bg4_width * (do_increase_width ? 2 : 1);
            const int layer4_height = (*ff7_externals.field_triggers_header)->bg4_height * (do_increase_height ? 2 : 1);
            const int left_offset = 352 + (is_fieldmap_wide() ? abs(wide_viewport_x) : 0);
            const int right_offset = is_fieldmap_wide() ? abs(wide_viewport_x) : 0;
            const int top_offset = 256 + (widescreen_enabled ? 8 : 0);
            const int bottom_offset = widescreen_enabled ? 8 : 0;

            for(int i = 0; i < *ff7_externals.field_layer4_tiles_num; i++)
            {
                uint32_t tile_index = (*ff7_externals.field_layer4_palette_sort)[i];
                vector2<float> tile_position = {
                    static_cast<float>(layer4_tiles[tile_index].x),
                    static_cast<float>(layer4_tiles[tile_index].y)
                };

                field_layer4_shift_tile_position(&tile_position, &bg_position, layer4_width, layer4_height);

                char anim_group = layer4_tiles[tile_index].anim_group;
                if(tile_position.x <= bg_position.x - left_offset || tile_position.x >= bg_position.x + right_offset ||
                    tile_position.y <= bg_position.y - top_offset || tile_position.y >= bg_position.y + bottom_offset ||
                    (anim_group && !(ff7_externals.modules_global_object->background_sprite_layer[anim_group] & layer4_tiles[tile_index].anim_bitmask)))
                    continue;

                layer4_tiles[tile_index].field_1040 = 1;
                tile_position.x = tile_position.x * field_bg_multiplier + initial_pos.x;
                tile_position.y = tile_position.y * field_bg_multiplier + initial_pos.y;

                if(!*ff7_externals.field_layer_CFF1D8 || layer4_tiles[tile_index].palette_index != (*ff7_externals.field_palette_D00088) + 1)
                {
                    uint32_t page = (layer4_tiles[tile_index].use_fx_page) ? layer4_tiles[tile_index].fx_page : layer4_tiles[tile_index].page;
                    ff7_externals.add_page_tile(tile_position.x, tile_position.y, z_value, layer4_tiles[tile_index].u,
                                                layer4_tiles[tile_index].v, layer4_tiles[tile_index].palette_index, page);
                }
            }

            if(widescreen_enabled)
            {
                // Apply repeat x-y for background layer 4 tiles
                std::vector<vector2<int>> tile_offsets;
                if(do_increase_height)
                    tile_offsets.push_back(vector2<int>{0, layer4_height /2});

                if(do_increase_width){
                    tile_offsets.push_back(vector2<int>{layer4_width / 2, 0});
                    tile_offsets.push_back(vector2<int>{layer4_width / 2, layer4_height / 2});
                }
                for(vector2<int> tile_offset: tile_offsets){
                    for(int i = 0; i < *ff7_externals.field_layer4_tiles_num; i++)
                    {
                        uint32_t tile_index = (*ff7_externals.field_layer4_palette_sort)[i];
                        vector2<float> tile_position = {
                            static_cast<float>(layer4_tiles[tile_index].x + tile_offset.x),
                            static_cast<float>(layer4_tiles[tile_index].y + tile_offset.y)
                        };

                        field_layer4_shift_tile_position(&tile_position, &bg_position, layer4_width, layer4_height);

                        char anim_group = layer4_tiles[tile_index].anim_group;
                        if(tile_position.x <= bg_position.x - left_offset || tile_position.x >= bg_position.x + right_offset ||
                            tile_position.y <= bg_position.y - top_offset || tile_position.y >= bg_position.y + bottom_offset ||
                            (anim_group && !(ff7_externals.modules_global_object->background_sprite_layer[anim_group] & layer4_tiles[tile_index].anim_bitmask)))
                            continue;

                        layer4_tiles[tile_index].field_1040 = 1;
                        tile_position.x = tile_position.x * field_bg_multiplier + initial_pos.x;
                        tile_position.y = tile_position.y * field_bg_multiplier + initial_pos.y;

                        if(!*ff7_externals.field_layer_CFF1D8 || layer4_tiles[tile_index].palette_index != (*ff7_externals.field_palette_D00088) + 1)
                        {
                            uint32_t page = (layer4_tiles[tile_index].use_fx_page) ? layer4_tiles[tile_index].fx_page : layer4_tiles[tile_index].page;
                            ff7_externals.add_page_tile(tile_position.x, tile_position.y, z_value, layer4_tiles[tile_index].u,
                                                        layer4_tiles[tile_index].v, layer4_tiles[tile_index].palette_index, page);
                        }
                    }
                }
            }

            *ff7_externals.field_layer4_flag_CFFEA8 = 1;
        }
    }


    void ff7_field_submit_draw_arrow(field_arrow_graphics_data* arrow_data)
    {
        // Add delta world position lost due to non-float calculation
        if(is_position_valid(field_3d_world_pos))
        {
            int view_multiplier = *ff7_externals.field_bg_multiplier;
            vector2<float> delta;
            delta.x = field_3d_world_pos.x - *ff7_externals.field_world_pos_x * view_multiplier;
            delta.y = field_3d_world_pos.y - *ff7_externals.field_world_pos_y * view_multiplier;
            for(int i = 0; i < 4; i++)
            {
                arrow_data->vertices[i].x += delta.x;
                arrow_data->vertices[i].y += delta.y;
            }
        }

        ff7_externals.field_submit_draw_arrow_63A171(arrow_data);
    }

    void ff7_field_submit_draw_cursor(field_arrow_graphics_data* arrow_data)
    {
        // Add delta position lost due to non-float calculation
        if(is_position_valid(cursor_position))
        {
            vector2<float> delta;
            delta.x = cursor_position.x - *ff7_externals.field_cursor_pos_x;
            delta.y = cursor_position.y - *ff7_externals.field_cursor_pos_y;
            for(int i = 0; i < 4; i++)
            {
                arrow_data->vertices[i].x += delta.x;
                arrow_data->vertices[i].y += delta.y;
            }
        }

        ff7_externals.field_submit_draw_arrow_63A171(arrow_data);
    }

    // ##################################################################
    // ##################################################################
    // ##################################################################

    void field_clip_with_camera_range_float(vector2<float>* point)
    {
        field_trigger_header* field_triggers_header_ptr = *ff7_externals.field_triggers_header;
        float half_width = 160;
        auto camera_range = field_triggers_header_ptr->camera_range;

        if(widescreen_enabled && is_fieldmap_wide())
        {
            camera_range = widescreen.getCameraRange();

            // Adjustment to prevent scrolling stopping one pixel too early
            camera_range.left += 1;
            camera_range.right -= 1;

            // This centers the background if necessary
            int cameraRangeSize = camera_range.right - camera_range.left;
            half_width = 160 + std::min(53, cameraRangeSize / 2 - 160);

            point->x += widescreen.getHorizontalOffset();
            if(widescreen.isResetVerticalPos()) point->y = 0;
            point->y += widescreen.getVerticalOffset();
        }

        if (point->x > camera_range.right - half_width)
            point->x = camera_range.right - half_width;
        if (point->x < camera_range.left + half_width)
            point->x = camera_range.left + half_width;
        if (point->y > camera_range.bottom - 120)
            point->y = camera_range.bottom - 120;
        if (point->y < camera_range.top + 120)
            point->y = camera_range.top + 120;

        if (enable_analogue_controls)
        {
            float accelCoeff = 0.1f / static_cast<float>(common_frame_multiplier);
            float maxScroll = 120;
            float maxScrollY = maxScroll * camera.getScrollingDirY();
            float maxScrollX = maxScroll * camera.getScrollingDirX();

            if (maxScrollX > 0.0) maxScrollX = std::min(maxScrollX, (camera_range.right - half_width - point->x));
            else maxScrollX = std::max(maxScrollX, camera_range.left + half_width - point->x);
            if (maxScrollY > 0.0) maxScrollY =  std::min(maxScrollY, (camera_range.bottom - 120 - point->y));
            else maxScrollY =  std::max(maxScrollY, camera_range.top + 120 - point->y);

            camera.setScrollingOffset(camera.getScrollingOffsetX() + accelCoeff * (maxScrollX - camera.getScrollingOffsetX()),
                                      camera.getScrollingOffsetY() + accelCoeff * (maxScrollY - camera.getScrollingOffsetY()));

            point->x += camera.getScrollingOffsetX();
            point->y += camera.getScrollingOffsetY();

            if (point->x > camera_range.right - half_width)
                point->x = camera_range.right - half_width;
            if (point->x < camera_range.left + half_width)
                point->x = camera_range.left + half_width;
            if (point->y > camera_range.bottom - 120)
                point->y = camera_range.bottom - 120;
            if (point->y < camera_range.top + 120)
                point->y = camera_range.top + 120;
        }
    }

    void float_sub_643628(field_trigger_header *trigger_header, vector2<float> *delta_position)
    {
        float half_width = 160;
        auto camera_range = trigger_header->camera_range;

        if(widescreen_enabled && is_fieldmap_wide())
        {
            camera_range = widescreen.getCameraRange();

            // This centers the background if necessary
            int cameraRangeSize = camera_range.right - camera_range.left;
            half_width = 160 + std::min(53, cameraRangeSize / 2 - 160);
        }

        if (trigger_header->field_14[0] == 1)
        {
            float diff_top_bottom = camera_range.bottom - 120 - (camera_range.top + 120);
            float diff_right_left = camera_range.right - half_width - (camera_range.left + half_width);
            float temp_1 = -(diff_top_bottom * (camera_range.top + 120 - delta_position->y) + diff_right_left * (camera_range.left + half_width - delta_position->x));
            float temp_square_value = (diff_top_bottom * diff_top_bottom + diff_right_left * diff_right_left) / 256.f;
            delta_position->x = ((diff_right_left * temp_1 / temp_square_value) / 256.f) + camera_range.left + half_width;
            delta_position->y = ((diff_top_bottom * temp_1 / temp_square_value) / 256.f) + camera_range.top + 120;
        }
        if (trigger_header->field_14[0] == 2)
        {
            float diff_bottom_top = camera_range.top + 120 - (camera_range.bottom - 120);
            float diff_right_left = camera_range.right - half_width - (camera_range.left + half_width);
            float temp_1 = -((diff_bottom_top) * (camera_range.bottom - 120 - delta_position->y) + diff_right_left * (camera_range.left + half_width - delta_position->x));
            float temp_square_value = (diff_bottom_top * diff_bottom_top + diff_right_left * diff_right_left) / 256.f;
            delta_position->x = ((diff_right_left * temp_1 / temp_square_value) / 256.f) + camera_range.left + half_width;
            delta_position->y = ((diff_bottom_top * temp_1 / temp_square_value) / 256.f) + camera_range.bottom - 120;
        }
    }

    void ff7_field_clip_with_camera_range(vector2<short>* point)
    {
        vector2<float> proxy_point = {(float)point->x, (float)point->y};
        field_clip_with_camera_range_float(&proxy_point);
        point->x = round(proxy_point.x);
        point->y = round(proxy_point.y);
    }

    void ff7_field_layer3_clip_with_camera_range(field_trigger_header* trigger_header, vector2<short>* point)
    {
        vector2<float> proxy_point = {(float)point->x, (float)point->y};
        float_sub_643628(*ff7_externals.field_triggers_header, &proxy_point);
        point->x = round(proxy_point.x);
        point->y = round(proxy_point.y);
    }

    void field_widescreen_width_clip_with_camera_range(vector2<short>* point)
    {
        if(!widescreen.isScriptedClipEnabled())
        {
            return;
        }

        auto camera_range = widescreen.getCameraRange();
        
        // Adjustment to prevent scrolling stopping one pixel too early
        camera_range.left += 1;
        camera_range.right -= 1;

        // This centers the background if necessary
        int cameraRangeSize = camera_range.right - camera_range.left;
        float half_width = 160 + std::min(53, cameraRangeSize / 2 - 160);

        point->x += widescreen.getHorizontalOffset();
        point->y += widescreen.getVerticalOffset();

        if (point->x > camera_range.right - half_width)
            point->x = camera_range.right - half_width;
        if (point->x < camera_range.left + half_width)
            point->x = camera_range.left + half_width;

        if(widescreen.isScriptedVerticalClipEnabled())
        {
            if (point->y > camera_range.bottom - 120)
                point->y = camera_range.bottom - 120;
            if (point->y < camera_range.top + 120)
                point->y = camera_range.top + 120;
        }
    }

    void engine_set_game_engine_world_coord_float_661B23(int field_world_x, int field_world_y)
    {
        ff7_externals.engine_set_game_engine_world_coord_661B23(field_world_x, field_world_y);

        // Override field_9A8 and field_9AC values with accurate field world coordinates position when possible
        ff7_game_obj* game_obj = (ff7_game_obj*)common_externals.get_game_object();
        if(game_obj)
        {
            if(ff7_fps_limiter >= FPS_LIMITER_30FPS)
            {
                if(is_position_valid(field_3d_world_pos))
                {
                    *(float*)&game_obj->field_9A8 = field_3d_world_pos.x;
                    *(float*)&game_obj->field_9AC = field_3d_world_pos.y;
                }
            }
        }
    }

    void engine_sub_661B68(int field_world_x, int field_world_y)
    {
        ff7_game_obj* game_obj = (ff7_game_obj*)common_externals.get_game_object();
        if(game_obj)
        {
            engine_set_game_engine_world_coord_float_661B23(field_world_x, field_world_y);
            ff7_externals.engine_sub_67CCDE(*(float*)&game_obj->field_99C, *(float*)&game_obj->field_9A0, *(float*)&game_obj->field_9A4,
                *(float*)&game_obj->field_9A8, *(float*)&game_obj->field_9AC, (float)(int)game_obj->_res_w, (float)(int)game_obj->_res_h, game_obj);
        }
    }

    void ff7_field_set_world_coordinate_640EB7()
    {
        if(is_position_valid(field_3d_world_pos) || *ff7_externals.field_world_pos_x != *ff7_externals.field_prev_world_pos_x || *ff7_externals.field_world_pos_y != *ff7_externals.field_prev_world_pos_y)
        {
            *ff7_externals.field_prev_world_pos_x = *ff7_externals.field_world_pos_x;
            *ff7_externals.field_prev_world_pos_y = *ff7_externals.field_world_pos_y;
            engine_sub_661B68((*ff7_externals.field_bg_multiplier) * (*ff7_externals.field_world_pos_x), (*ff7_externals.field_bg_multiplier) * (*ff7_externals.field_world_pos_y));
        }
    }

    void field_apply_player_position_2D_translation_float(vector2<float>* point, int model_id)
    {
        vector3<float> player_position;
        field_event_data* field_event_data_ptr = *ff7_externals.field_event_data_ptr;

        player_position.x = field_event_data_ptr[model_id].model_pos.x / 4096.f;
        player_position.y = field_event_data_ptr[model_id].model_pos.y / 4096.f;
        player_position.z = ff7_externals.modules_global_object->field_16 + (field_event_data_ptr[model_id].model_pos.z / 4096.f);
        field_apply_2D_translation_float_64314F(&player_position, point);
        point->x -= ff7_externals.field_viewport_xy_CFF204->x;
        point->y -= ff7_externals.field_viewport_xy_CFF204->y;
    }

    void field_init_scripted_bg_movement()
    {
        vector2<short> world_pos;
        if ( !ff7_externals.modules_global_object->world_move_status )
        {
            last_valid_scripted_field_delta_world_pos = {INVALID_VALUE, INVALID_VALUE};

            switch ( ff7_externals.modules_global_object->world_move_mode )
            {
            case 0:
                *ff7_externals.field_bg_flag_CC15E4 = 0;
                *ff7_externals.field_curr_delta_world_pos_x = 0;
                *ff7_externals.field_curr_delta_world_pos_y = 0;
                ff7_externals.modules_global_object->world_move_status = 2;

                last_valid_scripted_field_delta_world_pos = field_curr_delta_world_pos;
                break;
            case 1:
                *ff7_externals.field_bg_flag_CC15E4 = 1;
                ff7_externals.modules_global_object->world_move_status = 1;
                break;
            case 2:
            case 3:
                *ff7_externals.field_bg_flag_CC15E4 = 1;
                *ff7_externals.scripted_world_move_n_steps = ff7_externals.modules_global_object->field_20;
                *ff7_externals.scripted_world_move_step_index = 0;
                world_pos = {-(*ff7_externals.field_curr_delta_world_pos_x), -(*ff7_externals.field_curr_delta_world_pos_y)};

                if(is_fieldmap_wide())
                    field_widescreen_width_clip_with_camera_range(&world_pos);

                *ff7_externals.scripted_world_initial_pos_x = -world_pos.x;
                *ff7_externals.scripted_world_initial_pos_y = -world_pos.y;
                ff7_externals.modules_global_object->world_move_status = 1;
                break;
            case 4:
                *ff7_externals.field_bg_flag_CC15E4 = 1;

                world_pos = {-(ff7_externals.modules_global_object->field_A), -(ff7_externals.modules_global_object->field_C)};
                if(is_fieldmap_wide())
                    field_widescreen_width_clip_with_camera_range(&world_pos);

                *ff7_externals.field_curr_delta_world_pos_x = -world_pos.x;
                *ff7_externals.field_curr_delta_world_pos_y = -world_pos.y;
                ff7_externals.modules_global_object->world_move_status = 2;
                break;
            case 5:
            case 6:
                *ff7_externals.field_bg_flag_CC15E4 = 1;
                *ff7_externals.scripted_world_move_n_steps = ff7_externals.modules_global_object->field_20;
                *ff7_externals.scripted_world_move_step_index = 0;

                world_pos = {(-*ff7_externals.field_curr_delta_world_pos_x), -(*ff7_externals.field_curr_delta_world_pos_y)};
                if(is_fieldmap_wide())
                    field_widescreen_width_clip_with_camera_range(&world_pos);

                *ff7_externals.scripted_world_initial_pos_x = -world_pos.x;
                *ff7_externals.scripted_world_initial_pos_y = -world_pos.y;

                world_pos = {-(ff7_externals.modules_global_object->field_A), -(ff7_externals.modules_global_object->field_C)};
                if(is_fieldmap_wide())
                    field_widescreen_width_clip_with_camera_range(&world_pos);

                *ff7_externals.scripted_world_final_pos_x = -world_pos.x;
                *ff7_externals.scripted_world_final_pos_y = -world_pos.y;
                ff7_externals.modules_global_object->world_move_status = 1;
                break;
            default:
                return;
            }
        }
    }

    void field_update_scripted_bg_movement()
    {
        vector2<short> world_pos;
        vector2<float> world_pos_float;

        field_curr_delta_world_pos = {INVALID_VALUE, INVALID_VALUE};
        if(ff7_externals.modules_global_object->world_move_status == 1)
        {
            switch(ff7_externals.modules_global_object->world_move_mode)
            {
            case 1:
                ff7_externals.set_world_pos_based_on_player_pos_643C86(&world_pos);
                ff7_field_clip_with_camera_range(&world_pos);
                *ff7_externals.field_curr_delta_world_pos_x = -world_pos.x;
                *ff7_externals.field_curr_delta_world_pos_y = -world_pos.y;

                // Smooth background movement with floating point
                if(ff7_fps_limiter >= FPS_LIMITER_30FPS)
                {
                    field_apply_player_position_2D_translation_float(&world_pos_float, ff7_externals.modules_global_object->world_move_follow_model_id);
                    field_clip_with_camera_range_float(&world_pos_float);
                    field_curr_delta_world_pos.x = -world_pos_float.x;
                    field_curr_delta_world_pos.y = -world_pos_float.y;
                }
                break;
            case 2:
            case 3:
                if(*ff7_externals.scripted_world_move_n_steps)
                {
                    ff7_externals.set_world_pos_based_on_player_pos_643C86(&world_pos);
                    ff7_field_clip_with_camera_range(&world_pos);
                    std::function<int(int, int, int, int)> field_get_interpolated_value = ff7_externals.modules_global_object->world_move_mode == 2 ?
                        ff7_externals.field_get_linear_interpolated_value : ff7_externals.field_get_smooth_interpolated_value;
                    *ff7_externals.field_curr_delta_world_pos_x = field_get_interpolated_value(
                        *ff7_externals.scripted_world_initial_pos_x,
                        -world_pos.x,
                        *ff7_externals.scripted_world_move_n_steps,
                        *ff7_externals.scripted_world_move_step_index
                    );
                    *ff7_externals.field_curr_delta_world_pos_y = field_get_interpolated_value(
                        *ff7_externals.scripted_world_initial_pos_y,
                        -world_pos.y,
                        *ff7_externals.scripted_world_move_n_steps,
                        *ff7_externals.scripted_world_move_step_index
                    );

                    // Smooth background movement with floating point
                    if(ff7_fps_limiter >= FPS_LIMITER_30FPS)
                    {
                        field_apply_player_position_2D_translation_float(&world_pos_float, ff7_externals.modules_global_object->world_move_follow_model_id);
                        field_clip_with_camera_range_float(&world_pos_float);
                        std::function<float(float, float, int, int)> field_get_interpolated_value = ff7_externals.modules_global_object->world_move_mode == 2 ?
                            field_get_linear_interpolated_value_float : field_get_smooth_interpolated_value_float;
                        field_curr_delta_world_pos.x = field_get_interpolated_value(
                            *ff7_externals.scripted_world_initial_pos_x,
                            -world_pos_float.x,
                            *ff7_externals.scripted_world_move_n_steps,
                            *ff7_externals.scripted_world_move_step_index
                        );
                        field_curr_delta_world_pos.y = field_get_interpolated_value(
                            *ff7_externals.scripted_world_initial_pos_y,
                            -world_pos_float.y,
                            *ff7_externals.scripted_world_move_n_steps,
                            *ff7_externals.scripted_world_move_step_index
                        );
                    }

                    if(*ff7_externals.scripted_world_move_n_steps == *ff7_externals.scripted_world_move_step_index)
                        ff7_externals.modules_global_object->world_move_status = 2;
                    else
                        (*ff7_externals.scripted_world_move_step_index)++;
                }
                else
                {
                    ff7_externals.modules_global_object->world_move_status = 2;
                }
                break;
            case 5:
            case 6:
                if(*ff7_externals.scripted_world_move_n_steps)
                {
                    if(is_fieldmap_wide())
                    {
                        world_pos = {-(*ff7_externals.scripted_world_final_pos_x), -(*ff7_externals.scripted_world_final_pos_y)};
                        field_widescreen_width_clip_with_camera_range(&world_pos);
                        *ff7_externals.scripted_world_final_pos_x = -world_pos.x;

                        world_pos = {-(*ff7_externals.scripted_world_initial_pos_x), -(*ff7_externals.scripted_world_initial_pos_y)};
                        field_widescreen_width_clip_with_camera_range(&world_pos);    
                        *ff7_externals.scripted_world_initial_pos_x = -world_pos.x;                    
                    }

                    std::function<int(int, int, int, int)> field_get_interpolated_value = ff7_externals.modules_global_object->world_move_mode == 5 ?
                        ff7_externals.field_get_linear_interpolated_value : ff7_externals.field_get_smooth_interpolated_value;
                    *ff7_externals.field_curr_delta_world_pos_x = field_get_interpolated_value(
                        *ff7_externals.scripted_world_initial_pos_x,
                        *ff7_externals.scripted_world_final_pos_x,
                        *ff7_externals.scripted_world_move_n_steps,
                        *ff7_externals.scripted_world_move_step_index
                    );
                    *ff7_externals.field_curr_delta_world_pos_y = field_get_interpolated_value(
                        *ff7_externals.scripted_world_initial_pos_y,
                        *ff7_externals.scripted_world_final_pos_y,
                        *ff7_externals.scripted_world_move_n_steps,
                        *ff7_externals.scripted_world_move_step_index
                    );

                    // Smooth background movement with floating point
                    if(ff7_fps_limiter >= FPS_LIMITER_30FPS)
                    {
                        std::function<float(float, float, int, int)> field_get_interpolated_value = ff7_externals.modules_global_object->world_move_mode == 5 ?
                            field_get_linear_interpolated_value_float : field_get_smooth_interpolated_value_float;
                        field_curr_delta_world_pos.x = field_get_interpolated_value(
                            *ff7_externals.scripted_world_initial_pos_x,
                            *ff7_externals.scripted_world_final_pos_x,
                            *ff7_externals.scripted_world_move_n_steps,
                            *ff7_externals.scripted_world_move_step_index
                        );
                        field_curr_delta_world_pos.y = field_get_interpolated_value(
                            *ff7_externals.scripted_world_initial_pos_y,
                            *ff7_externals.scripted_world_final_pos_y,
                            *ff7_externals.scripted_world_move_n_steps,
                            *ff7_externals.scripted_world_move_step_index
                        );
                    }

                    if(*ff7_externals.scripted_world_move_n_steps == *ff7_externals.scripted_world_move_step_index)
                        ff7_externals.modules_global_object->world_move_status = 2;
                    else
                        (*ff7_externals.scripted_world_move_step_index)++;
                }
                else
                {
                    ff7_externals.modules_global_object->world_move_status = 2;
                }
                break;
            default:
                break;
            }

            if(is_fieldmap_wide())
            {
                world_pos = {-(*ff7_externals.field_curr_delta_world_pos_x), -(*ff7_externals.field_curr_delta_world_pos_y)};
                field_widescreen_width_clip_with_camera_range(&world_pos);
                *ff7_externals.field_curr_delta_world_pos_x = -world_pos.x;
            }
        }

        if(is_position_valid(field_curr_delta_world_pos))
        {
            last_valid_scripted_field_delta_world_pos = field_curr_delta_world_pos;
        }
    }

    void set_world_and_background_positions(vector2<float> delta_position, bool use_camdat_pan)
    {
        field_trigger_header* field_triggers_header_ptr = *ff7_externals.field_triggers_header;
        int field_bg_multiplier = *ff7_externals.field_bg_multiplier;

        field_3d_world_pos.x = ff7_externals.modules_global_object->shake_bg_x.shake_curr_value + ff7_externals.field_bg_offset->x - delta_position.x - 160;
        field_3d_world_pos.x += (use_camdat_pan) ? -(*ff7_externals.field_camera_data)->pan_x : 0;
        field_3d_world_pos.x *= field_bg_multiplier;
        field_3d_world_pos.y = ff7_externals.modules_global_object->shake_bg_y.shake_curr_value + ff7_externals.field_bg_offset->y - delta_position.y - 120;
        field_3d_world_pos.y += (use_camdat_pan) ? (*ff7_externals.field_camera_data)->pan_y : 0;
        field_3d_world_pos.y *= field_bg_multiplier;
        bg_main_layer_pos.x = delta_position.x + 320 - ff7_externals.field_bg_offset->x - ff7_externals.modules_global_object->shake_bg_x.shake_curr_value;
        bg_main_layer_pos.y = delta_position.y + 232 - ff7_externals.field_bg_offset->y - ff7_externals.modules_global_object->shake_bg_y.shake_curr_value;
        bg_layer3_pos.x = (field_triggers_header_ptr->bg3_pos_x / 16.f) + ((field_triggers_header_ptr->bg3_speed_x * delta_position.x) / 256.f);
        bg_layer3_pos.x = remainder(bg_layer3_pos.x, field_triggers_header_ptr->bg3_width);
        bg_layer3_pos.x = bg_layer3_pos.x + 320 - ff7_externals.field_bg_offset->x - ff7_externals.modules_global_object->shake_bg_x.shake_curr_value;
        bg_layer3_pos.y = (field_triggers_header_ptr->bg3_pos_y / 16.f) + ((field_triggers_header_ptr->bg3_speed_y * delta_position.y) / 256.f);
        bg_layer3_pos.y = remainder(bg_layer3_pos.y, field_triggers_header_ptr->bg3_height);
        bg_layer3_pos.y = bg_layer3_pos.y + 232 - ff7_externals.field_bg_offset->y - ff7_externals.modules_global_object->shake_bg_y.shake_curr_value;
        bg_layer4_pos.x = (field_triggers_header_ptr->bg4_pos_x / 16.f) + ((field_triggers_header_ptr->bg4_speed_x * delta_position.x) / 256.f);
        bg_layer4_pos.x = remainder(bg_layer4_pos.x, field_triggers_header_ptr->bg4_width);
        bg_layer4_pos.x = bg_layer4_pos.x + 320 - ff7_externals.field_bg_offset->x - ff7_externals.modules_global_object->shake_bg_x.shake_curr_value;
        bg_layer4_pos.y = (field_triggers_header_ptr->bg4_pos_y / 16.f) + ((field_triggers_header_ptr->bg4_speed_y * delta_position.y) / 256.f);
        bg_layer4_pos.y = remainder(bg_layer4_pos.y, field_triggers_header_ptr->bg4_height);
        bg_layer4_pos.y = bg_layer4_pos.y + 232 - ff7_externals.field_bg_offset->y - ff7_externals.modules_global_object->shake_bg_y.shake_curr_value;

        // Round the position to steps of 1/MIN_STEP_INVERSE due to visual glitches between tiles
        field_3d_world_pos.x = round(field_3d_world_pos.x * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
        field_3d_world_pos.y = round(field_3d_world_pos.y * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
        bg_main_layer_pos.x = round(bg_main_layer_pos.x * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
        bg_main_layer_pos.y = round(bg_main_layer_pos.y * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
        bg_layer3_pos.x = round(bg_layer3_pos.x * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
        bg_layer3_pos.y = round(bg_layer3_pos.y * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
        bg_layer4_pos.x = round(bg_layer4_pos.x * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
        bg_layer4_pos.y = round(bg_layer4_pos.y * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
    }

    void compute_pointer_hand_position(vector2<float> field_3d_world_coord, int model_id)
    {
        int view_multiplier = *ff7_externals.field_bg_multiplier;
        field_event_data* event_data_ptr = *ff7_externals.field_event_data_ptr;
        vector3<float> position;
        vector2<float> cursor_delta_pos;

        position.x = event_data_ptr[model_id].model_pos.x / 4096.f + event_data_ptr[model_id].offset_position_x;
        position.y = event_data_ptr[model_id].model_pos.y / 4096.f + event_data_ptr[model_id].offset_position_y;
        position.z = event_data_ptr[model_id].model_pos.z / 4096.f + event_data_ptr[model_id].offset_position_z + (ff7_externals.modules_global_object->field_10 << 7 >> 9);
        field_apply_2D_translation_float_64314F(&position, &cursor_delta_pos);
        cursor_position.x = field_3d_world_coord.x + view_multiplier * cursor_delta_pos.x + ff7_externals.field_curr_half_viewport_width_height_CFF1FC->x;
        cursor_position.y = field_3d_world_coord.y + view_multiplier * (cursor_delta_pos.y - 8) + ff7_externals.field_curr_half_viewport_width_height_CFF1FC->y;


        int viewport_x = is_fieldmap_wide() ? wide_viewport_x : ff7_externals.field_viewport_xy_CFF204->x;
        int viewport_width = is_fieldmap_wide() ? wide_viewport_width / 2 : 320;
        if(cursor_position.x > viewport_x + viewport_width * view_multiplier)
            cursor_position.x = viewport_x + viewport_width * view_multiplier;
        if(cursor_position.x < viewport_x)
            cursor_position.x = viewport_x;
        if(cursor_position.y > ff7_externals.field_viewport_xy_CFF204->y + 224 * view_multiplier)
            cursor_position.y = ff7_externals.field_viewport_xy_CFF204->y + 224 * view_multiplier;
        if(cursor_position.y < ff7_externals.field_viewport_xy_CFF204->y - 32)
            cursor_position.y = ff7_externals.field_viewport_xy_CFF204->y - 32;
    }

    void ff7_field_update_background()
    {
        ff7_externals.field_update_background_positions();

        int player_model_id = *ff7_externals.field_player_model_id;
        field_3d_world_pos = {INVALID_VALUE, INVALID_VALUE};
        bg_main_layer_pos = {INVALID_VALUE, INVALID_VALUE};
        bg_layer3_pos = {INVALID_VALUE, INVALID_VALUE};
        bg_layer4_pos = {INVALID_VALUE, INVALID_VALUE};
        cursor_position = {INVALID_VALUE, INVALID_VALUE};
        if ( *ff7_externals.word_CC1638 && !ff7_externals.modules_global_object->BGMOVIE_flag)
        {
            if(ff7_externals.modules_global_object->MVCAM_flag == 1 && is_position_valid(field_curr_delta_world_pos))
            {
                int field_bg_multiplier = *ff7_externals.field_bg_multiplier;
                field_3d_world_pos.x = (field_curr_delta_world_pos.x + ff7_externals.field_bg_offset->x - 160) * field_bg_multiplier;
                field_3d_world_pos.y = (field_curr_delta_world_pos.y + ff7_externals.field_bg_offset->y - 120) * field_bg_multiplier;
                field_3d_world_pos.x = round(field_3d_world_pos.x * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
                field_3d_world_pos.y = round(field_3d_world_pos.y * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
            }
        }
        else if(*ff7_externals.field_bg_flag_CC15E4)
        {
            if (enable_analogue_controls)
            {
                vector2<float> bg_delta_position = {0.0f, 0.0f};
                bool use_camdat_pan = true;
                if(is_position_valid(field_curr_delta_world_pos))
                {
                    bg_delta_position.x = -field_curr_delta_world_pos.x;
                    bg_delta_position.y = -field_curr_delta_world_pos.y;
                    use_camdat_pan = true;
                    set_world_and_background_positions(bg_delta_position, true);
                }
                else if(is_position_valid(last_valid_scripted_field_delta_world_pos))
                {
                    bg_delta_position.x = -last_valid_scripted_field_delta_world_pos.x;
                    bg_delta_position.y = -last_valid_scripted_field_delta_world_pos.y;
                    
                    field_clip_with_camera_range_float(&bg_delta_position);
                    field_curr_delta_world_pos.x = -bg_delta_position.x;
                    field_curr_delta_world_pos.y = -bg_delta_position.y;

                    set_world_and_background_positions(bg_delta_position, true);
                }
            }
            else
            {
                if(is_position_valid(field_curr_delta_world_pos))
                    set_world_and_background_positions({-field_curr_delta_world_pos.x, -field_curr_delta_world_pos.y}, true);
            }           
            

            if((*ff7_externals.field_event_data_ptr)[player_model_id].field_62)
                compute_pointer_hand_position(field_3d_world_pos, player_model_id);
        }
        else
        {
            vector2<float> bg_delta_position;
            field_apply_player_position_2D_translation_float(&bg_delta_position, player_model_id);
            field_clip_with_camera_range_float(&bg_delta_position);
            float_sub_643628(*ff7_externals.field_triggers_header, &bg_delta_position);
            field_curr_delta_world_pos.x = -bg_delta_position.x;
            field_curr_delta_world_pos.y = -bg_delta_position.y;
            set_world_and_background_positions(bg_delta_position, false);

            compute_pointer_hand_position(field_3d_world_pos, player_model_id);
        }
    }

    // This function should be called at each frame after drawing backgrounds and 3d models
    void draw_gray_quads_sub_644E90()
    {
        ff7_externals.field_draw_gray_quads_644E90();

        if (widescreen_enabled) widescreen.zoomBackground();

        newRenderer.setTimeFilterEnabled(false);
    }
}
