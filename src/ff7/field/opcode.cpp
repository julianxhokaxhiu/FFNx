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
#include "../../globals.h"
#include "../../ff7.h"
#include "../../common.h"
#include "../../field.h"
#include "opcode.h"
#include "utils.h"

#include <set>

namespace ff7::field
{
    std::set<field_bank_address> field_bank_address_to_be_fixed = {{14, 6}};
    field_bank_address mvief_bank_address;

    int call_original_opcode_function(byte opcode)
    {
        if(original_opcode_table[opcode])
            return ((int(*)())original_opcode_table[opcode])();
        else
        {
            ffnx_error("Initialization error: original opcode table empty in position %d\n", opcode);
            return 0;
        }
    }

    short ff7_opcode_multiply_get_bank_value(short bank, short address)
    {
        int16_t ret = ff7_externals.get_bank_value(bank, address);
        if(is_fps_running_more_than_original())
            ret *= get_frame_multiplier();
        return ret;
    }

    short ff7_opcode_divide_get_bank_value(short bank, short address)
    {
        int16_t ret = ff7_externals.get_bank_value(bank, address);
        if(is_fps_running_more_than_original())
        {
            if(abs(ret) >= get_frame_multiplier())
                ret /= get_frame_multiplier();
        }
        return ret;
    }

    int opcode_script_partial_animation_wrapper()
    {
        field_event_data* event_data = *ff7_externals.field_event_data_ptr;
        field_animation_data* animation_data = *ff7_externals.field_animation_data_ptr;
        WORD total_number_of_frames = -1;
        int frame_multiplier = get_frame_multiplier();

        byte curr_opcode = get_field_parameter<byte>(-1);
        byte curr_model_id = ff7_externals.field_model_id_array[*ff7_externals.current_entity_id];
        byte previous_animation_id = event_data[curr_model_id].animation_id;
        byte speed = get_field_parameter<byte>(3);
        short previous_current_frame = event_data[curr_model_id].currentFrame;
        short first_frame = 16 * get_field_parameter<byte>(1) * frame_multiplier / ((curr_opcode == CANIM1 || curr_opcode == CANIM2) ? speed : 1);
        short last_frame = (get_field_parameter<byte>(2) * frame_multiplier + 1) / speed;
        char animation_type = ff7_externals.animation_type_array[curr_model_id];

        int ret = call_original_opcode_function(curr_opcode);

        if(curr_model_id != 255)
        {
            switch(animation_type)
            {
            case 0:
            case 1:
            case 3:
                if(animation_data)
                    total_number_of_frames = animation_data[curr_model_id].anim_frame_object[2] - 1;

                if(last_frame > total_number_of_frames)
                    last_frame = total_number_of_frames;

                // Since last frame is increased by 1, there might be cases where first frame is greater than previous current frame
                if(previous_animation_id == event_data[curr_model_id].animation_id && previous_current_frame == first_frame + event_data[curr_model_id].animation_speed)
                    first_frame = previous_current_frame;

                event_data[curr_model_id].currentFrame = first_frame;
                event_data[curr_model_id].lastFrame = last_frame;
                break;
            default:
                break;
            }
        }

        return ret;
    }

    int opcode_script_SHAKE()
    {
        byte type = get_field_parameter<byte>(2);
        auto *field_global_data_ptr = *ff7_externals.field_global_object_ptr;
        if ( (type & 1) != 0 )
        {
            field_global_data_ptr->shake_bg_x.do_shake = 1;
            field_global_data_ptr->shake_bg_x.shake_amplitude = ff7_externals.get_char_bank_value(1, 4);
            field_global_data_ptr->shake_bg_x.shake_n_steps = ff7_externals.get_char_bank_value(2, 5);

            if(is_fps_running_more_than_original())
                field_global_data_ptr->shake_bg_x.shake_n_steps *= get_frame_multiplier();
        }
        else
        {
            field_global_data_ptr->shake_bg_x.do_shake = 0;
        }
        if ( (type & 2) != 0 )
        {
            field_global_data_ptr->shake_bg_y.do_shake = 1;
            field_global_data_ptr->shake_bg_y.shake_amplitude = ff7_externals.get_char_bank_value(3, 6);
            field_global_data_ptr->shake_bg_y.shake_n_steps = ff7_externals.get_char_bank_value(4, 7);

            if(is_fps_running_more_than_original())
                field_global_data_ptr->shake_bg_y.shake_n_steps *= get_frame_multiplier();
        }
        else
        {
            field_global_data_ptr->shake_bg_y.do_shake = 0;
        }
        ff7_externals.field_curr_script_position[*ff7_externals.current_entity_id] += 8;
        return 0;
    }

    int opcode_script_WAIT()
    {
        int result = 0;

        WORD frames_left = ff7_externals.wait_frames_ptr[*ff7_externals.current_entity_id];
        if (frames_left)
        {
            if (frames_left == 1)
            {
                ff7_externals.wait_frames_ptr[*ff7_externals.current_entity_id] = 0;
                ff7_externals.field_curr_script_position[*ff7_externals.current_entity_id] += 3;
                result = 0;
            }
            else
            {
                --ff7_externals.wait_frames_ptr[*ff7_externals.current_entity_id];
                result = 1;
            }
        }
        else
        {
            ff7_externals.wait_frames_ptr[*ff7_externals.current_entity_id] = get_field_parameter<WORD>(0);

            if(is_fps_running_more_than_original())
                ff7_externals.wait_frames_ptr[*ff7_externals.current_entity_id] *= get_frame_multiplier();

            if (!ff7_externals.wait_frames_ptr[*ff7_externals.current_entity_id])
                ff7_externals.field_curr_script_position[*ff7_externals.current_entity_id] += 3;
            result = 1;
        }

        return result;
    }

    int opcode_script_MVIEF()
    {
        mvief_bank_address = {get_field_parameter<byte>(0), get_field_parameter<byte>(1)};

        return call_original_opcode_function(MVIEF);
    }

    int opcode_script_BGMOVIE()
    {
        is_movie_bgfield = get_field_parameter<byte>(0);

        return call_original_opcode_function(BGMOVIE);
    }

    uint8_t opcode_IFSW_compare_sub()
    {
        int16_t left_value = ff7_externals.get_bank_value(1, 2);
        int16_t right_value = ff7_externals.get_bank_value(2, 4);
        byte compare_type = get_field_parameter<byte>(5);

        field_bank_address current_mvief_bank_address = {get_field_bank_value(0), (byte)get_field_parameter<WORD>(1)};

        // Movie fix
        if (is_overlapping_movie_playing() && movie_fps_ratio > 1)
        {
            if (current_mvief_bank_address == mvief_bank_address)
                right_value *= movie_fps_ratio;
        }
        else
        {
            if(ff7_fps_limiter == FPS_LIMITER_60FPS)
            {
                if(field_bank_address_to_be_fixed.contains(current_mvief_bank_address))
                    right_value *= common_frame_multiplier;
            }
        }

        switch(compare_type)
        {
        case 0:
            return (left_value == right_value);
        case 1:
            return (left_value != right_value);
        case 2:
            return (left_value > right_value);
        case 3:
            return (left_value < right_value);
        case 4:
            return (left_value >= right_value);
        case 5:
            return (left_value <= right_value);
        case 6:
            return (right_value & left_value);
        case 7:
            return (right_value ^ left_value);
        case 8:
            return (right_value | left_value);
        case 9:
            return ((1 << right_value) & left_value);
        case 10:
            return ((uint8_t)((1 << right_value) & left_value) == 0);
        default:
            return 0;
        }
    }

    int opcode_script_FADE()
    {
        int ret = ((int(*)())ff7_externals.opcode_fade)();

        if(is_fps_running_more_than_original())
        {
            if((*ff7_externals.field_global_object_ptr)->fade_speed >= get_frame_multiplier())
                (*ff7_externals.field_global_object_ptr)->fade_speed /= get_frame_multiplier();
        }

        return ret;
    }
}

