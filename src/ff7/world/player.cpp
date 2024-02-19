/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Cosmos                                             //
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

#include "world.h"
#include "camera.h"
#include "utils.h"

#include <math.h>
#include "defs.h"
#include "../../patch.h"

namespace ff7::world {

    int get_player_movement_speed(int model_id)
    {
        int movement_multiplier = *ff7_externals.world_movement_multiplier_DFC480;
        switch(model_id)
        {
            case HIGHWIND: // Highwind
                return 120 * movement_multiplier;
            case SUBMARINE:
                return 45 * movement_multiplier;
            case WILD_CHOCOBO:
            case TINY_BRONCO:
            case CHOCOBO:
            case 41:
            case 42:
                // Chocobos and Tiny Bronco
                return 60 * movement_multiplier;
            case BUGGY: // Buggy
                return 45 * movement_multiplier;
            default:
                return 30 * movement_multiplier;
        }
    }

    short get_player_direction_on_key_input(int current_key_status)
    {
        short direction = INVALID_DIRECTION;

        auto joyDir = ff7::world::world.GetJoystickDirection();
        float inputDirLength = vector_length(&joyDir);
	    if(inputDirLength > 0.0f)
        {
            float angle = atan2( joyDir.x, - joyDir.y);
            if (angle < 0) { angle += 2 * M_PI; }
            direction = static_cast<short>(std::max(0.0, std::min(4096.0, 4096.0 * angle / (2.0* M_PI))));
        }
        else
        {
            if(is_key_pressed(current_key_status, LEFT))
                direction = -1024;
            if(is_key_pressed(current_key_status, RIGHT))
                direction = 1024;
            if(is_key_pressed(current_key_status, UP))
            {
                if(direction == INVALID_DIRECTION)
                    direction = 2048;
                else
                    direction += direction / 2;
            }
            if(is_key_pressed(current_key_status, DOWN))
            {
                if(direction == INVALID_DIRECTION)
                    direction = 0;
                else
                    direction -= direction / 2;
            }
        }

        return direction;
    }

    void update_player_and_handle_input()
    {
        static float rotationSpeedInterp = 0.0f;

        const int worldmap_type = *ff7_externals.world_map_type_E045E8;
        const int movement_multiplier = *ff7_externals.world_movement_multiplier_DFC480;

        vector3<int> delta_movement = {0, 0, 0};
        ff7_externals.world_set_current_entity_to_player_entity();
        const int player_model_id = ff7_externals.world_get_player_model_id();

        if(*ff7_externals.world_is_control_enabled_DE6B5C && ff7_externals.world_get_unknown_flag_75335C())
        {
            int current_key_input = ff7_externals.world_get_current_key_input_status();
            int prev_key_input = *ff7_externals.world_prev_key_input_status_DFC470;

            // Limit key input
            if(player_model_id == TINY_BRONCO && is_key_pressed(current_key_input, CROSS))
                current_key_input &= SELECT | UNK1 | UNK2;

            int movement_speed = get_player_movement_speed(player_model_id);
            short player_direction = get_player_direction_on_key_input(current_key_input);
            if(player_direction != INVALID_DIRECTION)
            {
                delta_movement.x = cos((player_direction / 2048.f) * M_PI - M_PI / 2) * movement_speed;
                delta_movement.z = -sin((player_direction / 2048.f) * M_PI - M_PI / 2) * movement_speed;
            }

            int camera_view_type = *ff7_externals.world_camera_viewtype_DFC4B4;
            if(enable_analogue_controls || camera_view_type == FRONT_VIEW || camera_view_type == HIGHWIND_VIEW || worldmap_type == SNOWSTORM)
            {
                update_world_camera_front(current_key_input, prev_key_input);

                auto joyDir = ff7::world::world.GetJoystickDirection();

                static float deltaMovement = 0.0f;
                static int horizontalDeltaInterp = 0;
                
                bool highwind_view_and_square_pressed = camera_view_type == HIGHWIND_VIEW && is_key_pressed(current_key_input, SQUARE | L1);

                int horizontal_delta = 0;
                if(!highwind_view_and_square_pressed)
                {
                if(std::abs(joyDir.x) > 0.0f)
                    horizontal_delta = -2048 * joyDir.x;
                else if(is_key_pressed(current_key_input, LEFT))
                    horizontal_delta = 2048;
                else if(is_key_pressed(current_key_input, RIGHT))
                    horizontal_delta = -2048;
                }

                horizontalDeltaInterp = (15 * horizontalDeltaInterp + horizontal_delta) / 16;                
                if(camera_view_type == HIGHWIND_VIEW || worldmap_type == UNDERWATER || player_model_id == TINY_BRONCO || player_model_id == BUGGY || player_model_id == SUBMARINE)
                {
                    bool is_movement_key_pressed = is_key_pressed(current_key_input, highwind_view_and_square_pressed ? ANY_DIRECTIONAL_KEY: CIRCLE | R1);
                    bool is_back_movement_key_pressed = is_key_pressed(current_key_input, SQUARE | L1);

                    if(is_back_movement_key_pressed && (player_model_id == BUGGY || player_model_id == SUBMARINE))
                    {
                        if(!is_movement_key_pressed)
                        {                   
                            movement_speed = -movement_speed * 0.75f;
                        } else movement_speed = 0;

                        is_movement_key_pressed = true;
                    }

                    if (!is_movement_key_pressed || highwind_view_and_square_pressed) movement_speed = 0;
                    
                    deltaMovement = (deltaMovement * 31.0f + movement_speed) / 32.0f;

                    if(!is_movement_key_pressed && std::abs(deltaMovement) < 10.0f) deltaMovement = 0.0f;
                    *ff7_externals.world_special_delta_movement_DE6A18 = deltaMovement;

                    short last_valid_player_direction = (*ff7_externals.world_event_current_entity_ptr_E39AD8)->direction;
                    if(highwind_view_and_square_pressed && std::abs(deltaMovement) < 10)
                    {
                        auto hightwindSpeed = get_player_movement_speed(player_model_id);
                        if(std::abs(joyDir.x) > 0.0f)
                            delta_movement.x = joyDir.x * hightwindSpeed;
                        else if(is_key_pressed(current_key_input, LEFT))
                            delta_movement.x = -hightwindSpeed;
                        else if(is_key_pressed(current_key_input, RIGHT))
                            delta_movement.x = hightwindSpeed;

                        if(std::abs(joyDir.y) > 0.0f)
                            delta_movement.z = -joyDir.y * hightwindSpeed;
                        else if(is_key_pressed(current_key_input, UP))
                            delta_movement.z = -hightwindSpeed;
                        else if(is_key_pressed(current_key_input, DOWN))
                            delta_movement.z = hightwindSpeed;

                        delta_movement.x *= 0.25f;
                        delta_movement.z *= 0.25f;
                    }
                    else
                    {
                        const float rotSpeedXMax = worldmap_type == UNDERWATER || player_model_id == SUBMARINE ? 24.0f : 32.0f;
                        float rotSpeedX = 0.0f;
                        if (std::abs(joyDir.x) > 0.0)
                            rotSpeedX = rotSpeedXMax * joyDir.x / common_frame_multiplier;
                        else if(is_key_pressed(current_key_input, LEFT))
                            rotSpeedX = -rotSpeedXMax / common_frame_multiplier;
                        else if(is_key_pressed(current_key_input, RIGHT))
                            rotSpeedX = rotSpeedXMax / common_frame_multiplier;

                        if ( player_model_id == BUGGY )
                        {
                            rotationSpeedInterp = rotSpeedX;
                        }
                        else
                        {
                            rotationSpeedInterp = (rotationSpeedInterp * 15 + rotSpeedX) / 16.0f;
                        }
                        short delta = -static_cast<short>(rotationSpeedInterp);

                        if (player_model_id == BUGGY )
                        {                                
                            auto maxSpeed = static_cast<float>(get_player_movement_speed(player_model_id));
                            if (is_back_movement_key_pressed) maxSpeed *= 0.5f;
                            delta *= std::abs(deltaMovement) / maxSpeed;
                        }

                        last_valid_player_direction += delta;
                        

                        if(last_valid_player_direction > 2048)
                            last_valid_player_direction -= 4096;
                        if(last_valid_player_direction < -2048)
                            last_valid_player_direction += 4096;

                        player_direction = last_valid_player_direction + *ff7_externals.world_camera_front_DFC484 - 4096;

                        delta_movement.x = cos((player_direction / 2048.f) * M_PI - M_PI / 2) * (deltaMovement);
                        delta_movement.z = -sin((player_direction / 2048.f) * M_PI - M_PI / 2) * (deltaMovement);
                    }
                }
                else if(player_direction != INVALID_DIRECTION)
                    ff7_externals.world_set_facing_and_direction_to_current_entity(player_direction - *ff7_externals.world_camera_front_DFC484);

                // Deflect delta movement with camera front direction
                vector3<short> copy_delta_movement = {(short)delta_movement.x, 0, (short)delta_movement.z};
                vector3<short> rotation = {0, (short)-*ff7_externals.world_camera_front_DFC484, 0};
                vector3<int> output_delta = {0, 0, 0};
                int dummy;
                rotation_matrix matrix;
                ff7_externals.engine_apply_rotation_to_transform_matrix_6628DE(&rotation, &matrix);
                matrix.position[0] = 0;
                matrix.position[1] = 0;
                matrix.position[2] = 0;
                ff7_externals.engine_set_game_engine_rot_matrix_663673(&matrix);
                ff7_externals.engine_set_game_engine_position_663707(&matrix);
                ff7_externals.engine_apply_translation_with_delta_662ECC(&copy_delta_movement, &output_delta, &dummy);
                delta_movement.x = output_delta.x;
                delta_movement.z = output_delta.z;

                static float verticalDeltaInterp = 0.0;
                int vertical_delta = 0;
                static float verticalSpeedinterp = 0.0;
                float verticalSpeed = 0.0;
                if(camera_view_type == HIGHWIND_VIEW && !highwind_view_and_square_pressed)
                {                    
                    vector4<int> player_highwind_position;
                    ff7_externals.world_copy_player_pos_to_param_762798(&player_highwind_position);

                    auto joyDir = ff7::world::world.GetJoystickDirection();
                    if((joyDir.y > 0 || is_key_pressed(current_key_input, UP)) && player_highwind_position.y > 500 && *ff7_externals.world_y_player_pos_flag_DE6A14 &&
                       ff7_externals.world_get_player_walkmap_type() != 27)
                    {
                        if(joyDir.y > 0)
                        {
                            verticalSpeed = -joyDir.y * 50 * movement_multiplier;
                            vertical_delta = joyDir.y * 10.0;
                        }
                        else
                        {
                            verticalSpeed = -50 * movement_multiplier;
                            vertical_delta = 10;
                        }
                    }
                    else if((joyDir.y < 0 || is_key_pressed(current_key_input, DOWN)) && player_highwind_position.y < UINT16_MAX / 2 - 50 * movement_multiplier - 2000)
                    {
                        if (joyDir.y < 0)
                        {
                            verticalSpeed =-joyDir.y * 50 * movement_multiplier;
                            vertical_delta = joyDir.y * 10.0;
                        }
                        else
                        {
                            verticalSpeed = 50 * movement_multiplier;
                            vertical_delta = -10;
                        }
                    }
                    verticalSpeedinterp = (verticalSpeedinterp * 7.0f + verticalSpeed) / 8.0f;
                    ff7_externals.world_add_y_pos_to_current_entity_761F22(verticalSpeedinterp);
                }

                if(worldmap_type == UNDERWATER)
                {
                    vector4<int> player_submarine_position;
                    ff7_externals.world_copy_player_pos_to_param_762798(&player_submarine_position);

                    if((joyDir.y > 0 || is_key_pressed(current_key_input, UP)) && player_submarine_position.y > -5000 && *ff7_externals.world_y_player_pos_flag_DE6A14)
                    {
                        if (joyDir.y > 0)
                        {
                            verticalSpeed = -joyDir.y * 15 * movement_multiplier;
                            vertical_delta = joyDir.y * 10.0;
                        }
                        else
                        {
                            verticalSpeed = -15 * movement_multiplier;
                            vertical_delta = 10;
                        }
                    }

                    if((joyDir.y < 0 || is_key_pressed(current_key_input, DOWN)) && player_submarine_position.y <-1000)
                    {
                        if (joyDir.y > 0)
                        {
                            verticalSpeed = -joyDir.y * 15 * movement_multiplier;
                            vertical_delta = joyDir.y * 10.0;
                        }
                        else
                        {
                            verticalSpeed = 15 * movement_multiplier;
                            vertical_delta = -10;
                        }
                    }

                    verticalSpeedinterp = (verticalSpeedinterp * 7.0f + verticalSpeed) / 8.0f;
                    ff7_externals.world_add_y_pos_to_current_entity_761F22(verticalSpeedinterp);
                }

                verticalDeltaInterp = (verticalDeltaInterp * 15.0f + vertical_delta) / 16.0f;

                if(camera_view_type == HIGHWIND_VIEW)
                {
                    int is_player_moving = ff7_externals.world_is_current_entity_animated_761F44() &&
                                           abs(*ff7_externals.world_camera_rotation_z_DE6B70) >= 16 || horizontalDeltaInterp;
                    delta_movement.y = ((movement_multiplier == 1) | (2 * is_player_moving)) + 3;
                    *ff7_externals.world_camera_rotation_z_DE6B70 = ((1 << (delta_movement.y - 1)) + horizontalDeltaInterp
                                                                    + *ff7_externals.world_camera_rotation_z_DE6B70 * (1 << delta_movement.y - 1)) >> delta_movement.y;
                    *ff7_externals.world_unk_rotation_value_E045E0 = (verticalDeltaInterp);
                    ff7_externals.world_sub_762F75(*ff7_externals.world_unk_rotation_value_E045E0 + 4, *ff7_externals.world_camera_rotation_z_DE6B70 * 4, *ff7_externals.world_camera_rotation_z_DE6B70 * -2);
                }

                // Reset world camera rotation Z
                if(!horizontalDeltaInterp && abs(*ff7_externals.world_camera_rotation_z_DE6B70) <= 4)
                    *ff7_externals.world_camera_rotation_z_DE6B70 = 0;

                if(worldmap_type != SNOWSTORM)
                {
                    if(player_direction == INVALID_DIRECTION)
                        player_direction = *ff7_externals.previous_player_direction_DF5434;

                    if(player_direction != INVALID_DIRECTION)
                        *ff7_externals.previous_player_direction_DF5434 = player_direction;

                    player_direction += 2048;
                    if(player_direction > 2048)
                        player_direction -= 4096;

                    if(camera_view_type != HIGHWIND_VIEW && worldmap_type != UNDERWATER && player_model_id != SUBMARINE && player_model_id != TINY_BRONCO)
                    {
                        if(delta_movement.x != 0 || delta_movement.z != 0.0f)
                        {
                            int current_direction = player_direction;
                            ff7_externals.world_set_facing_and_direction_to_current_entity(current_direction + 2048 - *ff7_externals.world_camera_front_DFC484);
                        }
                    }
                    else
                    {
                        if(!highwind_view_and_square_pressed)
                        {
                            int current_direction = player_direction;
                            ff7_externals.world_set_facing_and_direction_to_current_entity(current_direction + 2048 - *ff7_externals.world_camera_front_DFC484);
                        }
                    }
                }
            }
            else
            {
                *ff7_externals.world_camera_rotation_z_DE6B70 /= 2;
                if(player_direction != INVALID_DIRECTION)
                    ff7_externals.world_set_facing_and_direction_to_current_entity(player_direction);
            }

            if(player_model_id == BUGGY) {
                if(is_key_pressed_first_time(current_key_input, prev_key_input, ANY_DIRECTIONAL_KEY))
                    ff7_externals.world_music_set_frequency_all_channels_75E6A8(8u, 32);

                if(is_key_released(current_key_input, prev_key_input, ANY_DIRECTIONAL_KEY))
                    ff7_externals.world_music_set_frequency_all_channels_75E6A8(8u, 0);
            }

            if(player_model_id == SUBMARINE && is_key_pressed_first_time(current_key_input, prev_key_input, CROSS))
            {
                if(worldmap_type == UNDERWATER && ff7_externals.world_get_player_walkmap_type() == 3)
                    ff7_externals.world_sub_74D6BB();
                else if(ff7_externals.world_get_player_walkmap_type() == 3)
                    ff7_externals.world_sub_74D6F6();
            }

            if(is_key_pressed_first_time(current_key_input, prev_key_input, START) ||
               is_key_pressed_first_time(current_key_input, prev_key_input, SELECT))
            {
                if(*ff7_externals.world_map_type_E045E8 == OVERWORLD)
                    ff7_externals.world_set_minimap_mask((ff7_externals.world_get_minimap_mask() + 1) % 3);
                else
                    ff7_externals.world_set_minimap_mask(ff7_externals.world_get_minimap_mask() == 0 ? 2 : 0);
            }

            if(is_key_pressed_first_time(current_key_input, prev_key_input, TRIANGLE) && *ff7_externals.world_mode_E045E4 == 1)
            {
                if(player_model_id == HIGHWIND)
                    ff7_externals.world_run_special_opcode_7640BC(6);
                else if((player_model_id ==  CLOUD || player_model_id ==  TIFA || player_model_id ==  CID || player_model_id == BUGGY) && ff7_externals.world_get_player_walkmap_type() != 14)
                {
                    ff7_externals.world_set_camera_fade_speed_755B97(16);
                    ff7_externals.world_set_world_control_lock_74D438(0, 1);
                    *ff7_externals.world_mode_E045E4 = 2;
                    ff7_externals.world_sub_74C980(0);
                }
            }

            *ff7_externals.world_prev_key_input_status_DFC470 = current_key_input;
            if(ff7_externals.world_get_player_walkmap_type() == 14)
                ff7_externals.world_add_delta_movement_due_to_bridge_7591C2(&delta_movement.x, &delta_movement.z);
            if(player_model_id == TINY_BRONCO)
                ff7_externals.world_sfx_play_or_stop_75E6CC((delta_movement.z | delta_movement.x) != 0 ? 493 : -493);

            if(ff7_footsteps)
                world_update_model_movement(delta_movement.x, delta_movement.z);
            else
                ff7_externals.world_update_model_movement_762E87(delta_movement.x, delta_movement.z);
            ff7_externals.world_current_entity_model_collision_detection_with_other_models_76296E();
        }

        if(!ff7_externals.world_is_control_enabled_DE6B5C && ff7_externals.world_get_player_walkmap_type() == 14)
        {
            ff7_externals.world_add_delta_movement_due_to_bridge_7591C2(&delta_movement.x, &delta_movement.z);
            world_update_model_movement(delta_movement.x, delta_movement.z);
            ff7_externals.world_current_entity_model_collision_detection_with_other_models_76296E();
        }

        update_world_camera_rotation_y();
    }

}