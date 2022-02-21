/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Tang-Tang Zhou                                     //
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

#include "../ff7.h"
#include "../patch.h"
#include "../sfx.h"

std::map<uint32_t, bool> do_decrease_wait_frames;

// For worldmap footsteps
void ff7_world_update_model_movement(int delta_position_x, int delta_position_z)
{
    constexpr int distance_threshold = 10;
    float distance = std::sqrt(std::pow(delta_position_x, 2) + std::pow(delta_position_z, 2));

    // TODO: Fix footsteps when player is not moving at all. Here delta position is not clipped by collision detection (Need to find collision detection)
    if(*ff7_externals.world_event_current_entity_ptr_E39AD8 && distance > distance_threshold)
    {
        int player_model_id = ff7_externals.world_get_player_model_id();
        if(player_model_id >= 0 && player_model_id <= 2 || player_model_id == 4 || player_model_id == 19) // Cloud, Tifa, Cid, and Chocobo
        {
            sfx_play_wm_footstep(player_model_id, ff7_externals.world_get_player_walkmap_type());
        }
    }

    ff7_externals.world_update_model_movement_762E87(delta_position_x, delta_position_z);
}

void ff7_world_init_variables(short param_1)
{
    do_decrease_wait_frames.clear();

    ((void(*)(short))ff7_externals.world_init_variables_74E1E9)(param_1);
}

void ff7_world_snake_compute_delta_position(short* delta_position, short z_value)
{
    ff7_externals.world_compute_delta_position_753D00(delta_position, z_value);
    if(delta_position)
    {
        delta_position[0] /= common_frame_multiplier;
        delta_position[1] /= common_frame_multiplier;
        delta_position[2] /= common_frame_multiplier;
    }
}

int ff7_run_world_script_system_operations(WORD opcode)
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

int ff7_pop_world_stack_multiply_wrapper()
{
    int ret = ff7_externals.pop_world_script_stack();
    return ret * common_frame_multiplier;
}

int ff7_pop_world_stack_divide_wrapper()
{
    int ret = ff7_externals.pop_world_script_stack();
    return ret / common_frame_multiplier;
}

int ff7_get_world_encounter_rate()
{
    int encounter_rate = ff7_externals.get_world_encounter_rate();
    return encounter_rate / common_frame_multiplier;
}

void ff7_world_hook_init()
{
    // World init
    replace_call_function(ff7_externals.world_update_sub_74DB8C + 0x108, ff7_world_init_variables);

    // Movement related fix
    patch_divide_code<DWORD>(ff7_externals.world_init_variables_74E1E9 + 0x15D, common_frame_multiplier);
    replace_call_function(ff7_externals.run_world_event_scripts_system_operations + 0x172, ff7_pop_world_stack_divide_wrapper);
    replace_call_function(ff7_externals.run_world_event_scripts_system_operations + 0x14B, ff7_pop_world_stack_divide_wrapper);
    replace_call_function(ff7_externals.run_world_event_scripts_system_operations + 0x55D, ff7_pop_world_stack_divide_wrapper);
    replace_call_function(ff7_externals.run_world_event_scripts_system_operations + 0x584, ff7_pop_world_stack_divide_wrapper);

    // Camera related fix
    replace_call_function(ff7_externals.run_world_event_scripts_system_operations + 0x7DC, ff7_pop_world_stack_divide_wrapper);

    // Midgar zolom (snake) movement fix
    replace_call_function(ff7_externals.update_world_snake_position_7564CD + 0x26, ff7_world_snake_compute_delta_position);
    replace_call_function(ff7_externals.update_world_snake_position_7564CD + 0x195, ff7_world_snake_compute_delta_position);
    replace_call_function(ff7_externals.update_world_snake_position_7564CD + 0x2B4, ff7_world_snake_compute_delta_position);

    // World Encounter rate fix
    replace_call_function(ff7_externals.world_sub_767641 + 0x110, ff7_get_world_encounter_rate);

    // Message speed fix
    patch_code_byte(ff7_externals.world_opcode_message_update_text_769C02 + 0xF6, 0x5 + common_frame_multiplier / 2);
    patch_divide_code<byte>(ff7_externals.world_opcode_message_update_text_769C02 + 0xF9, common_frame_multiplier);
    patch_divide_code<WORD>(ff7_externals.world_opcode_message_update_text_769C02 + 0x10A, common_frame_multiplier);
    patch_code_byte(ff7_externals.world_opcode_message_update_text_769C02 + 0x13A, 0x4 + common_frame_multiplier / 2);

    // Wait frames decrease delayed
    replace_call_function(ff7_externals.run_world_event_scripts + 0xC7, ff7_run_world_script_system_operations);
}