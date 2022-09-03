/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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

#pragma once

#include "../../common.h"
#include "../../globals.h"

namespace ff7::field
{
    void ff7_field_hook_init();
    void field_load_textures(struct ff7_game_obj *game_object, struct struc_3 *struc_3);
    void field_layer1_pick_tiles(short x_offset, short y_offset);
    void field_layer2_pick_tiles(short x_offset, short y_offset);
    void field_layer3_pick_tiles(short x_offset, short y_offset);
    void field_layer4_pick_tiles(short x_offset, short y_offset);
    void ff7_field_clip_with_camera_range(vector2<short>* point);
    void ff7_field_layer3_clip_with_camera_range(field_trigger_header* trigger_header, vector2<short>* point);
    uint32_t field_open_flevel_siz();
    void field_init_scripted_bg_movement();
    void field_update_scripted_bg_movement();
    bool ff7_field_do_draw_3d_model(short x, short y);
    void ff7_field_set_fade_quad_size(int x, int y, int width, int height);
}