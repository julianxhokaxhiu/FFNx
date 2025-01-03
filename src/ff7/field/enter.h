/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

#pragma once

#include "model.h"
#include "background.h"
#include "../widescreen.h"

namespace ff7::field
{
    inline void ff7_field_initialize_variables()
    {
        ((void(*)())ff7_externals.field_initialize_variables)();

        field_3d_world_pos = {INVALID_VALUE, INVALID_VALUE};
        bg_main_layer_pos = {INVALID_VALUE, INVALID_VALUE};
        bg_layer3_pos = {INVALID_VALUE, INVALID_VALUE};
        bg_layer4_pos = {INVALID_VALUE, INVALID_VALUE};
        field_curr_delta_world_pos = {INVALID_VALUE, INVALID_VALUE};
        cursor_position = {INVALID_VALUE, INVALID_VALUE};

        // reset movement frame index for all models
        for(auto &external_data : external_model_data){
            external_data.moveFrameIndex = 0;
            external_data.rotationMoveFrameIndex = 0;
            external_data.prevCollisionRadius = 0;

            external_data.blinkFrameIndex = BLINKING_FRAMES;
        }

        if(widescreen_enabled || enable_uncrop) widescreen.initParamsFromConfig();
    }
}
