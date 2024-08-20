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

#pragma once

#include "ff7.h"

#include <array>

namespace ff7::field
{
    struct external_field_model_data
    {
        int moveFrameIndex;
        vector3<int> initialPosition;
        vector3<int> finalPosition;
        int wasNotCollidingWithTarget;
        int updateMovementReturnValue;
        int prevCollisionRadius;

        int rotationMoveFrameIndex;

        int blinkFrameIndex;
    };

    constexpr int MAX_FIELD_MODELS = 32;
    constexpr int BLINKING_FRAMES = 4;

    std::array<external_field_model_data, MAX_FIELD_MODELS> external_model_data;

    bool ff7_field_do_draw_3d_model(short x, short y);
    void ff7_field_update_models_position(int key_input_status);
    int ff7_field_update_player_model_position(short model_id);
    int ff7_field_update_single_model_position(short model_id);
    int ff7_field_check_collision_with_target(field_event_data* field_event_model, short target_collision_radius);
    void ff7_field_update_models_rotation_new();
    void ff7_field_blink_3d_model(field_animation_data* anim_data, field_model_blink_data* blink_data);
    void ff7_field_update_model_animation_frame(short model_id);
    int ff7_field_models_eye_to_model(char* model_name);
}
