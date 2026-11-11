/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include <math.h>

#include "../../globals.h"
#include "../../patch.h"
#include "../../sfx.h"
#include "../../log.h"
#include "../../macro.h"

#include "cfg.h"
#include "defs.h"
#include "world.h"
#include "utils.h"
#include "renderer.h"
#include "gl.h"
#include "../field/kawai.h"
#include "../defs.h"

namespace ff7::world
{
    World world;

    std::map<uint32_t, bool> do_decrease_wait_frames;

    namespace
    {
        namespace gte = field::kawai::gte;

        gte::Matrix terrain_colors = {{{1774, 0, 0}, {1774, 0, 0}, {1774, 0, 0}}};
        bool sky_dither = false;
        bool inside_light_core = false;
        bool loading_terrain_texture = false;

        ff7_graphics_object* load_terrain_graphics_object(uint32_t type, uint32_t polytype,
            struc_3* info, char* name, void* context)
        {
            const bool saved = loading_terrain_texture;
            loading_terrain_texture = true;
            auto* object = ff7_externals._load_texture(type, polytype, info, name, context);
            loading_terrain_texture = saved;
            return object;
        }

        int get_distance_to_light_point(vector4<int>* position, world_light_point* point)
        {
            const int distance = ff7_externals.world_get_distance_to_light_point(position, point);
            inside_light_core |= distance < static_cast<int64_t>(point->core_range) * 1024;
            return distance;
        }

        int set_terrain_light_color(const int16_t* colors)
        {
            sky_dither = true;
            for (size_t i = 0; i < 3; ++i) terrain_colors[i][0] = colors[3 * i];
            return ff7_externals.world_set_terrain_light_color(colors);
        }

        int16_t update_terrain_lighting(vector4<int>* position)
        {
            terrain_colors = {{{1774, 0, 0}, {1774, 0, 0}, {1774, 0, 0}}};
            sky_dither = false;
            inside_light_core = false;
            const auto result = ff7_externals.world_update_terrain_lighting(position);
            // This regional draw mode controls the sky, not the terrain's always-dithered pass.
            sky_dither &= !inside_light_core;
            return result;
        }

        struct ModelLighting
        {
            std::vector<uint32_t> colors;
            std::vector<vector3<float>> normals;
            ff7_light* light;
        };

        std::map<const indexed_primitive*, ModelLighting> model_lighting;
        std::map<const indexed_primitive*, field::kawai::Vector> model_color_offsets;

        int add_model_color(const bgra_color* color, hrc_data* model)
        {
            const auto result = ff7_externals.world_add_model_color(color, model);
            if (!model) return result;
            const field::kawai::Vector offset = {
                std::round(color->r * 255), std::round(color->g * 255), std::round(color->b * 255)};
            for (uint32_t i = 0; i < model->num_bones; ++i)
            {
                const auto& bone = model->bones[i];
                if (!bone.rsd_array) continue;
                for (uint32_t part = 0; part < bone.num_rsd; ++part)
                {
                    auto* rsd = bone.rsd_array[part].rsd_data;
                    auto* set = rsd ? rsd->polygon_set : nullptr;
                    if (!set || !set->light || !set->indexed_primitives) continue;
                    for (uint32_t group = 0; group < set->numgroups; ++group)
                    {
                        auto* primitive = set->indexed_primitives[group];
                        if (primitive) model_color_offsets[primitive] = offset;
                    }
                }
            }
            return result;
        }

        void prepare_model_lighting(hrc_data* model, anim_header* animation)
        {
            if (!model || !animation || !animation->num_frames || !animation->anim_frames || !model->bone_list ||
                (animation->use_matrix_array && !animation->matrix_array))
            {
                ffnx_unexpected_once("Cannot prepare world model lighting: missing initial animation, matrices or skeleton\n");
                return;
            }
            auto* frame = &animation->anim_frames[0];
            const bool alternate = model->field_A4 && *model->field_A4;
            auto root_animation = alternate ? ff7_externals._root_animation : ff7_externals.root_animation;
            auto frame_animation = alternate ? ff7_externals._frame_animation : ff7_externals.frame_animation;
            std::vector<matrix> stack(1);
            if (animation->use_matrix_array) stack.back() = animation->matrix_array[0];
            else root_animation(&stack.back(), frame, animation, model);
            list_node* node;
            LIST_FOR_EACH(node, model->bone_list)
            {
                auto* entry = reinterpret_cast<bone_list_member*>(&node->object);
                if (entry->bone_type == 2)
                {
                    stack.pop_back();
                    continue;
                }
                if (entry->bone_type != 1) continue;
                auto* bone = &model->bones[entry->bone_index];
                vector3<float> zero{};
                auto* rotation = entry->bone_index < animation->num_bones ? &frame->data[entry->bone_index] : &zero;
                matrix local, posed;
                if (animation->use_matrix_array)
                    local = animation->matrix_array[entry->bone_index + 1];
                else
                    frame_animation(entry->bone_index, &local, rotation, frame, animation, bone, model);
                multiply_matrix(&local, &stack.back(), &posed);
                stack.push_back(posed);
                if (!bone->rsd_array) continue;
                for (uint32_t part = 0; part < bone->num_rsd; ++part)
                {
                    auto* rsd = bone->rsd_array[part].rsd_data;
                    auto* set = rsd ? rsd->polygon_set : nullptr;
                    if (!set || !set->polygon_data || !set->indexed_primitives) continue;
                    auto* polygon = set->polygon_data;
                    field::kawai::Effect effect;
                    auto* light = set->light;
                    gte::Matrix directions{};
                    if (light)
                    {
                        const auto ambient = light->global_light_color_abgr_norm;
                        effect.color = {std::round(ambient.r * 255), std::round(ambient.g * 255), std::round(ambient.b * 255)};
                        const std::array lights = {light->color_1, light->color_2, light->color_3};
                        for (size_t i = 0; i < lights.size(); ++i)
                        {
                            const auto point = lights[i]->point;
                            // Undo the native world-axis conversion before lighting the original model pose.
                            effect.light_directions[i] = {-point.x, point.z, -point.y};
                            const auto color = lights[i]->d3dcol;
                            effect.light_colors[i] = {std::round(color.r * 255), std::round(color.g * 255), std::round(color.b * 255)};
                        }
                        gte::Affine bind_pose;
                        for (size_t column = 0; column < 3; ++column)
                        {
                            vector3<float> axis = {posed.m[0][column], posed.m[1][column], posed.m[2][column]};
                            normalize_vector(&axis);
                            const auto fixed = field::kawai::fixed_normal({axis.x, axis.y, axis.z});
                            for (size_t row = 0; row < 3; ++row) bind_pose.rotation[column][row] = fixed[row];
                        }
                        directions = gte::compose({field::kawai::light_directions(effect), {}}, bind_pose).rotation;
                    }
                    for (uint32_t group = 0; group < set->numgroups; ++group)
                    {
                        auto* primitive = set->indexed_primitives[group];
                        if (!primitive || model_lighting.contains(primitive)) continue;
                        ModelLighting lighting;
                        lighting.light = light;
                        lighting.normals.resize(primitive->vertexcount);
                        if (!polygon->normaldata) gl_calculate_normals(&lighting.normals, primitive, polygon, nullptr);
                        const auto offset = polygon->groupdata[group].offvert;
                        for (uint32_t i = 0; i < primitive->vertexcount; ++i)
                        {
                            const auto index = offset + i;
                            if (polygon->normaldata)
                                lighting.normals[i] = polygon->normaldata[polygon->has_normindextable ?
                                    polygon->normindextabledata[index] : index];
                            if (light)
                            {
                                const auto& normal = lighting.normals[i];
                                const auto source = polygon->vertexcolordata ? polygon->vertexcolordata[index] :
                                    primitive->vertices[i].color.color;
                                lighting.colors.push_back(field::kawai::light_color(source, effect,
                                    field::kawai::fixed_normal({normal.x, normal.y, normal.z}), directions));
                            }
                        }
                        model_lighting.emplace(primitive, std::move(lighting));
                    }
                }
            }
        }

        void draw_world_model(uint32_t frame, anim_header* animation, struc_110* instance,
            hrc_data* model, ff7_game_obj* game)
        {
            anim_header** animations = nullptr;
            if (model == *ff7_externals.world_player_hrc)
                animations = *ff7_externals.world_player_animations;
            else
                for (int i = 0; i < ff7_externals.world_get_model_count(); ++i)
                {
                    const auto& entry = ff7_externals.world_other_model_animations[i];
                    if (entry.model == model)
                    {
                        animations = entry.animations;
                        break;
                    }
                }
            // PSX lights animation zero/frame zero once, not the moving character or camera.
            prepare_model_lighting(model, animations ? animations[0] : nullptr);
            reinterpret_cast<void(*)(uint32_t, anim_header*, struc_110*, hrc_data*, ff7_game_obj*)>(
                ff7_externals.draw_3d_model)(frame, animation, instance, model, game);
        }
    }

    void forget_model_lighting(const indexed_primitive* primitive)
    {
        model_lighting.erase(primitive);
        model_color_offsets.erase(primitive);
    }

    bool draw_model_with_psx_lighting(indexed_primitive* primitive, polygon_data* polygon, uint32_t clip)
    {
        if (enable_worldmap_external_mesh || getmode_cached()->driver_mode != MODE_WORLDMAP) return false;
        const auto found = model_lighting.find(primitive);
        if (found == model_lighting.end()) return false;
        auto& lighting = found->second;
        std::vector<nvertex> vertices(primitive->vertices, primitive->vertices + primitive->vertexcount);
        // Static objects use their authored colors, including any native scripted changes.
        if (lighting.light)
        {
            const auto tint = lighting.light->color;
            const auto offset = model_color_offsets.find(primitive);
            for (size_t i = 0; i < vertices.size(); ++i)
            {
                vertices[i].color.color = lighting.colors[i];
                // World recoloring adds signed RGB offsets after the initial-pose lighting.
                if (offset != model_color_offsets.end())
                    vertices[i].color.color = field::kawai::add_color(vertices[i].color.color, offset->second);
                if (lighting.light->flags & 4)
                {
                    vertices[i].color.r = vertices[i].color.r * tint.r / 255;
                    vertices[i].color.g = vertices[i].color.g * tint.g / 255;
                    vertices[i].color.b = vertices[i].color.b * tint.b / 255;
                }
            }
        }
        const bool saved_world = current_state.psx_world;
        const bool saved_radar = current_state.psx_world_radar;
        const bool saved_meteor = current_state.psx_world_meteor;
        const auto saved_dither = current_state.psx_world_dither;
        current_state.psx_world = true;
        current_state.psx_world_radar = false;
        current_state.psx_world_meteor = false;
        current_state.psx_world_dither = is_sky_dithering_enabled() ?
            PSXWorldDitherMode::Native : PSXWorldDitherMode::Off;
        gl_draw_indexed_primitive(primitive->primitivetype, primitive->vertextype, vertices.data(),
            lighting.normals.data(), primitive->vertexcount, primitive->indices,
            primitive->indexcount, nullptr, polygon->boundingboxdata, nullptr, clip, true);
        current_state.psx_world = saved_world;
        current_state.psx_world_radar = saved_radar;
        current_state.psx_world_meteor = saved_meteor;
        current_state.psx_world_dither = saved_dither;
        return true;
    }

    bool world_has_old_highwind()
    {
        return (*ff7_externals.insertedCD) <= 2;
    }

    void world_update_model_movement(int delta_position_x, int delta_position_z)
    {
        int player_model_id = ff7_externals.world_get_player_model_id();
        constexpr float distance_threshold = 10.0f;
        float distance = std::sqrt(std::pow(delta_position_x, 2) + std::pow(delta_position_z, 2));

        // For Highwind sound
        if(player_model_id == 3)
        {
            int key_input_status = ff7_externals.world_get_current_key_input_status();
            bool is_highwind_moving = is_key_pressed(key_input_status, enable_analogue_controls ? CIRCLE | R1 : CIRCLE);
            sfx_process_wm_highwind(world_has_old_highwind(), is_highwind_moving);
        }
        // For worldmap footsteps
        else if((player_model_id >= 0 && player_model_id <= 2) || player_model_id == 4 || player_model_id == 19) // Cloud, Tifa, Cid, and Chocobo
        {
            // TODO: Fix footsteps when player is not moving at all. Here delta position is not clipped by collision detection (Need to find collision detection)
            if(distance > distance_threshold)
            {
                sfx_process_wm_footstep(player_model_id, ff7_externals.world_get_player_walkmap_type());
            }
        }

        ff7_externals.world_update_model_movement_762E87(delta_position_x, delta_position_z);
    }

    void world_init_variables(short param_1)
    {
        do_decrease_wait_frames.clear();

        ((void(*)(short))ff7_externals.world_init_variables_74E1E9)(param_1);
    }

    void world_snake_compute_delta_position(vector3<short>* delta_position, short z_value)
    {
        ff7_externals.world_sub_753D00(delta_position, z_value);
        if(delta_position)
        {
            delta_position->x /= common_frame_multiplier;
            delta_position->y /= common_frame_multiplier;
            delta_position->z /= common_frame_multiplier;
        }
    }

    int run_world_script_system_operations(WORD opcode)
    {
        int ret = 0;
        if (opcode == 0x306)
        {
            vector3<short> delta_position;
            if (*ff7_externals.is_wait_frames_zero_E39BC0)
            {
                --(*ff7_externals.world_event_current_entity_ptr_E3A7CC)->curr_script_position;
                return 1;
            }
            else
            {
                if(do_decrease_wait_frames[(uint32_t)*ff7_externals.world_event_current_entity_ptr_E3A7CC])
                    (*ff7_externals.world_event_current_entity_ptr_E3A7CC)->wait_frames--;
                do_decrease_wait_frames[(uint32_t)*ff7_externals.world_event_current_entity_ptr_E3A7CC] = !do_decrease_wait_frames[(uint32_t)*ff7_externals.world_event_current_entity_ptr_E3A7CC];

                if ((*ff7_externals.world_event_current_entity_ptr_E3A7CC)->wait_frames)
                    (*ff7_externals.world_event_current_entity_ptr_E3A7CC)->curr_script_position--;
                else
                    *ff7_externals.is_wait_frames_zero_E39BC0 = 1;
                delta_position.x = 0;
                delta_position.y = 0;
                delta_position.z = (*ff7_externals.world_event_current_entity_ptr_E39AD8)->movement_speed << (4 * (((*ff7_externals.world_event_current_entity_ptr_E39AD8)->animation_is_loop_mask & 0x40) != 0));
                ff7_externals.world_sub_753D00(&delta_position, (*ff7_externals.world_event_current_entity_ptr_E39AD8)->direction);
                ff7_externals.world_update_model_movement_762E87(delta_position.x, delta_position.z);
                (*ff7_externals.world_event_current_entity_ptr_E39AD8)->offset_y -= (*ff7_externals.world_event_current_entity_ptr_E39AD8)->vertical_speed;
                (*ff7_externals.world_event_current_entity_ptr_E39AD8)->position.y += (*ff7_externals.world_event_current_entity_ptr_E39AD8)->vertical_speed_2;
                return (*ff7_externals.world_event_current_entity_ptr_E3A7CC)->wait_frames != 0;
            }
        }
        else
        {
            ret = ((int (*)(WORD))ff7_externals.run_world_event_scripts_system_operations)(opcode);
        }
        return ret;
    }

    int pop_world_stack_multiply_wrapper()
    {
        int ret = ff7_externals.pop_world_script_stack();
        return ret * common_frame_multiplier;
    }

    int pop_world_stack_divide_wrapper()
    {
        int ret = ff7_externals.pop_world_script_stack();
        return ret / common_frame_multiplier;
    }

    int get_world_encounter_rate()
    {
        int encounter_rate = ff7_externals.get_world_encounter_rate();
        return encounter_rate / common_frame_multiplier;
    }

    bgra_color_ui8* calculate_terrain_lighting(matrix*, vector3<float>* normal,
        bgra_color*, bgra_color_ui8* source_color, bgra_color_ui8* out_color)
    {
        // PSX NCT uses untransformed normals; distance fading is already in the texture palette.
        const bool underwater = *ff7_externals.world_map_type_E045E8 == UNDERWATER;
        const gte::Matrix directions = underwater ?
            gte::Matrix{{{0, -4096, 0}, {0, 0, 0}, {0, 0, 0}}} :
            gte::Matrix{{{-4096, -4096, -4096}, {0, 0, 0}, {0, 0, 0}}};
        const gte::LongVector ambient = underwater ?
            gte::LongVector{32 * 16, 32 * 16, 48 * 16} : gte::LongVector{64 * 16, 64 * 16, 64 * 16};
        const auto light = gte::transform_ir({directions, {}},
            field::kawai::fixed_normal({normal->x, normal->y, normal->z}), true);
        const auto color = gte::transform_ir({terrain_colors, ambient}, light, true);
        out_color->r = static_cast<uint8_t>(std::clamp(color[0] >> 4, 0, 255));
        out_color->g = static_cast<uint8_t>(std::clamp(color[1] >> 4, 0, 255));
        out_color->b = static_cast<uint8_t>(std::clamp(color[2] >> 4, 0, 255));
        out_color->a = source_color->a;
        return out_color;
    }

    bool is_terrain_graphics_object(const graphics_object* object)
    {
        for (uint32_t i = 0; i < *ff7_externals.world_terrain_graphics_object_count; ++i)
        {
            if (static_cast<const void*>(object) == ff7_externals.world_terrain_graphics_objects[i])
                return true;
        }
        return false;
    }

    bool is_sky_dithering_enabled()
    {
        return sky_dither;
    }

    bool is_loading_terrain_texture()
    {
        return loading_terrain_texture;
    }

    void buildPSXWorldMipmaps(uint8_t* destination, const uint8_t* source, uint32_t width, uint32_t height)
    {
        // Linear UNORM stores texture5 * 8: mip interpolation retains fractional PSX texels.
        for (size_t pixel = 0; pixel < size_t(width) * height; ++pixel)
        {
            for (uint32_t channel = 0; channel < 3; ++channel)
                destination[pixel * 4 + channel] = source[pixel * 4 + channel] & 0xF8;
            destination[pixel * 4 + 3] = source[pixel * 4 + 3];
        }

        while (width > 1 || height > 1)
        {
            const uint32_t nextWidth = std::max(1u, width / 2);
            const uint32_t nextHeight = std::max(1u, height / 2);
            uint8_t* next = destination + size_t(width) * height * 4;
            if (width > 1 && height > 1 && width % 2 == 0 && height % 2 == 0)
            {
                bimg::imageRgba8Downsample2x2(next, width, height, 1, width * 4, nextWidth * 4, destination, false);
            }
            else
            {
                // bimg's 2x2 helper does not handle one-dimensional tails or odd-sized edges.
                const uint64_t area = uint64_t(width) * height;
                for (uint32_t y = 0; y < nextHeight; ++y)
                    for (uint32_t x = 0; x < nextWidth; ++x)
                    {
                        uint64_t sums[4] = {};
                        for (uint32_t sy = y * height / nextHeight; sy < ((y + 1) * height + nextHeight - 1) / nextHeight; ++sy)
                            for (uint32_t sx = x * width / nextWidth; sx < ((x + 1) * width + nextWidth - 1) / nextWidth; ++sx)
                            {
                                const uint32_t weightX = std::min((x + 1) * width, (sx + 1) * nextWidth) - std::max(x * width, sx * nextWidth);
                                const uint32_t weightY = std::min((y + 1) * height, (sy + 1) * nextHeight) - std::max(y * height, sy * nextHeight);
                                const uint8_t* pixel = destination + (size_t(sy) * width + sx) * 4;
                                for (uint32_t channel = 0; channel < 4; ++channel)
                                    sums[channel] += uint64_t(pixel[channel]) * weightX * weightY;
                            }
                        for (uint32_t channel = 0; channel < 4; ++channel)
                            next[(size_t(y) * nextWidth + x) * 4 + channel] = uint8_t((sums[channel] + area / 2) / area);
                    }
            }
            destination = next;
            width = nextWidth;
            height = nextHeight;
        }
    }

    void world_hook_init()
    {
        if (!enable_worldmap_external_mesh)
        {
            // Texture upload precedes graphics-object registration; exclude the snow-particle load.
            replace_call_function(ff7_externals.world_init_load_map_meshes_graphics_objects_75A283 + 0x181, load_terrain_graphics_object);
            replace_call_function(ff7_externals.world_init_load_map_meshes_graphics_objects_75A283 + 0x25B, load_terrain_graphics_object);
            replace_call_function(ff7_externals.world_mode_loop_sub_74DB8C + 0x3FA, update_terrain_lighting);
            replace_call_function((uint32_t)ff7_externals.world_update_terrain_lighting + 0x147, set_terrain_light_color);
            replace_call_function((uint32_t)ff7_externals.world_update_terrain_lighting + 0x67, get_distance_to_light_point);
            replace_call_function(ff7_externals.world_draw_all_3d_model_74C6B0 + 0x46, draw_world_model);
            replace_call_function(ff7_externals.world_draw_all_3d_model_74C6B0 + 0x103, draw_world_model);
            replace_call_function(ff7_externals.world_recolor_model + 0xA9, add_model_color);
            replace_call_function(ff7_externals.world_submit_draw_bg_meshes_75F68C + 0xEB3, calculate_terrain_lighting);
            replace_call_function(ff7_externals.world_submit_draw_bg_meshes_75F68C + 0xEDE, calculate_terrain_lighting);
            replace_call_function(ff7_externals.world_submit_draw_bg_meshes_75F68C + 0xF09, calculate_terrain_lighting);
        }

        if(ff7_fps_limiter == FPS_LIMITER_60FPS)
        {
            // World init
            replace_call_function(ff7_externals.world_mode_loop_sub_74DB8C + 0x108, world_init_variables);

            // Movement related fix
            patch_divide_code<DWORD>(ff7_externals.world_init_variables_74E1E9 + 0x15D, common_frame_multiplier);
            replace_call_function(ff7_externals.run_world_event_scripts_system_operations + 0x172, pop_world_stack_divide_wrapper);
            replace_call_function(ff7_externals.run_world_event_scripts_system_operations + 0x14B, pop_world_stack_divide_wrapper);
            replace_call_function(ff7_externals.run_world_event_scripts_system_operations + 0x55D, pop_world_stack_divide_wrapper);
            replace_call_function(ff7_externals.run_world_event_scripts_system_operations + 0x584, pop_world_stack_divide_wrapper);

            // Camera related fix
            replace_call_function(ff7_externals.run_world_event_scripts_system_operations + 0x7DC, pop_world_stack_divide_wrapper);

            // Midgar zolom (snake) movement fix
            replace_call_function(ff7_externals.update_world_snake_position_7564CD + 0x26, world_snake_compute_delta_position);
            replace_call_function(ff7_externals.update_world_snake_position_7564CD + 0x195, world_snake_compute_delta_position);
            replace_call_function(ff7_externals.update_world_snake_position_7564CD + 0x2B4, world_snake_compute_delta_position);

            // World Encounter rate fix
            replace_call_function(ff7_externals.world_sub_767641 + 0x110, get_world_encounter_rate);

            // Text box message fix
            patch_code_byte(ff7_externals.world_text_box_window_paging_769C02 + 0xF6, 0x5 + common_frame_multiplier / 2);
            patch_divide_code<byte>(ff7_externals.world_text_box_window_paging_769C02 + 0xF9, common_frame_multiplier);
            patch_divide_code<WORD>(ff7_externals.world_text_box_window_paging_769C02 + 0x10A, common_frame_multiplier);
            patch_code_byte(ff7_externals.world_text_box_window_paging_769C02 + 0x13A, 0x4 + common_frame_multiplier / 2);
            patch_code_byte(ff7_externals.world_text_box_window_opening_769A66 + 0x3D, 0x2 + common_frame_multiplier / 2);
            patch_code_byte(ff7_externals.world_text_box_window_opening_769A66 + 0xD2, 0x2 + common_frame_multiplier / 2);
            patch_code_byte(ff7_externals.world_text_box_window_closing_76ADF7 + 0x67, 0x2 + common_frame_multiplier / 2);
            patch_code_byte(ff7_externals.world_text_box_window_closing_76ADF7 + 0xC2, 0x2 + common_frame_multiplier / 2);
            patch_divide_code<short>(ff7_externals.world_text_box_reverse_paging_76ABE9 + 0x42, common_frame_multiplier);
            patch_divide_code<short>(ff7_externals.world_opcode_message + 0x1AC, common_frame_multiplier);
            patch_divide_code<short>(ff7_externals.world_opcode_message + 0x2CF, common_frame_multiplier);
            patch_divide_code<short>(ff7_externals.world_opcode_ask + 0x1AC, common_frame_multiplier);
            patch_divide_code<byte>(ff7_externals.world_opcode_ask + 0x3CC, common_frame_multiplier);

            // Wait frames decrease delayed
            replace_call_function(ff7_externals.run_world_event_scripts + 0xC7, run_world_script_system_operations);
        }

        if (enable_time_cycle)
        {
            replace_call_function((uint32_t)ff7_externals.world_wm0_overworld_draw_all_74C179 + 0x175, ff7::world::wm0_draw_minimap_quad_graphics_object);
            replace_call_function((uint32_t)ff7_externals.world_wm0_overworld_draw_all_74C179 + 0x1BE, ff7::world::wm0_draw_world_effects_1_graphics_object);
            replace_call_function((uint32_t)ff7_externals.world_wm0_overworld_draw_all_74C179 + 0x208, ff7::world::wm0_draw_minimap_points_graphics_object);
        }

        if (enable_worldmap_external_mesh)
        {
            // Replace player and camera update functions
            if (enable_analogue_controls)
            {
                replace_function(ff7_externals.world_update_camera_74E8CE, ff7::world::update_world_camera);
                replace_function(ff7_externals.world_update_player_74EA48, ff7::world::update_player_and_handle_input);

                // Disable Z-axis camera rotation
                replace_function(ff7_externals.world_sub_74D319, ff7::world::get_camera_rotation_z);
            }            
           
            replace_call_function(ff7_externals.world_mode_loop_sub_74DB8C + 0x296, ff7::world::init_load_wm_bot_blocks);
            replace_call_function(ff7_externals.world_exit_74BD77 + 0x11, ff7::world::destroy_graphics_objects);

            // Expand meteor quad size to render the top part
            patch_code_char(ff7_externals.world_submit_draw_clouds_and_meteor_7547A6 + 0x205, 0x7F);
            patch_code_char(ff7_externals.world_submit_draw_clouds_and_meteor_7547A6 + 0x217, 0x0);

            // Replace original world rendering
            patch_code_dword(ff7_externals.world_init_load_map_meshes_graphics_objects_75A283 + 0xA7, (DWORD)&ff7::world::wm0_overworld_draw_all);
            replace_call_function((uint32_t)ff7_externals.world_wm0_overworld_draw_all_74C179 + 0xF8, ff7::world::wm0_overworld_draw_clouds);
            replace_call_function((uint32_t)ff7_externals.world_wm0_overworld_draw_all_74C179 + 0x10A, ff7::world::wm0_overworld_draw_meteor);
            patch_code_dword(ff7_externals.world_init_load_map_meshes_graphics_objects_75A283 + 0xDF, (DWORD)&ff7::world::wm2_underwater_draw_all);
            patch_code_dword(ff7_externals.world_init_load_map_meshes_graphics_objects_75A283 + 0x117, (DWORD)&ff7::world::wm3_snowstorm_draw_all);

            // Disable original world spherical transformation
            memset_code(ff7_externals.world_sub_75F0AD + 0xFA, 0x90, 3);
            memset_code(ff7_externals.world_sub_75F0AD + 0x187, 0x90, 3);
            memset_code(ff7_externals.world_sub_75F0AD + 0x1A5, 0x90, 3);

            // Disable world models vertical movement
            replace_function(ff7_externals.world_sub_762F9A, ff7::world::world_sub_762F9A);

            // Draw snake with new spherical world transformation		
            replace_function(ff7_externals.animate_world_snake_75692A, ff7::world::animate_world_snake);	

            // Modify projection matrix to increase view frustrum far plane
            replace_call_function((uint32_t)ff7_externals.engine_apply_4x4_matrix_product_with_game_obj_matrix_67D2BF + 0x16, ff7::world::engine_apply_4x4_matrix_product_between_matrices);
            
            // Add same spherical world transformation as done in the shader to particles too
            replace_function(ff7_externals.world_sub_75C0FD, ff7::world::world_draw_effects);		

            // Remove world meshes draw calls by jumping to end of loop
            patch_code_byte((uint32_t)ff7_externals.world_wm0_overworld_draw_all_74C179 + 0x32, 0x76);
            patch_code_byte((uint32_t)ff7_externals.world_wm2_underwater_draw_all_74C3F0 + 0x32, 0x2D);
            patch_code_byte((uint32_t)ff7_externals.world_wm3_snowstorm_draw_all_74C589 + 0x32, 0x2D);

            // Disable model culling
            // TODO: try to find a way to make to game update models even when player is far away
            //memset_code(ff7_externals.world_draw_all_3d_model_74C6B0 + 0x84, 0x90, 6);
           
            // Disable shadow draw call
            // TODO: fix shadow rendering when lighting not enabled
            //if (enable_lighting)
            {
                replace_call_function((uint32_t)ff7_externals.world_wm0_overworld_draw_all_74C179 + 0x13B, ff7::world::draw_shadow);
                replace_call_function((uint32_t)ff7_externals.world_wm3_snowstorm_draw_all_74C589 + 0xB8, ff7::world::draw_shadow);
            }
        }
    }    
}
