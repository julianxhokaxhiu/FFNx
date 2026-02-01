/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

#pragma once

#include "../../ff7.h"

namespace ff7::field
{
    constexpr float INVALID_VALUE = -1000000;

    vector2<float>
        field_curr_delta_world_pos,
        last_valid_scripted_field_delta_world_pos,
        field_3d_world_pos,
        bg_main_layer_pos,
        bg_layer3_pos,
        bg_layer4_pos,
        cursor_position;

    void ff7_field_update_background();
    void ff7_field_set_world_coordinate_640EB7();
    void ff7_field_submit_draw_arrow(field_arrow_graphics_data* arrow_data);
    void ff7_field_submit_draw_cursor(field_arrow_graphics_data* arrow_data);
    void draw_gray_quads_sub_644E90();
    inline bool is_position_valid(vector2<float> position) {
        return position.x != INVALID_VALUE && position.y != INVALID_VALUE;
    }

}
