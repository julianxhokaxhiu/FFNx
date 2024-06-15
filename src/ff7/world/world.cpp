/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
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

#include "defs.h"
#include "world.h"
#include "utils.h"
#include "renderer.h"

namespace ff7::world
{
    World world;

    std::map<uint32_t, bool> do_decrease_wait_frames;

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

    void world_hook_init()
    {
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