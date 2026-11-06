/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

namespace ff7::field
{
    void ff7_field_initialize_eyetx();
    void ff7_field_reset_blink_frames();
    void ff7_field_set_mouth_index(byte model_id, byte mouth_index);
    bool ff7_field_is_facial_primitive(const polygon_data *polygon, const indexed_primitive *primitive);
    void ff7_handle_KAWAI_reset();
    int ff7_field_models_eye_to_model(char* model_name);
    int ff7_field_blink_eye_sub_649B50(field_animation_data *field_anim_data, field_model_blink_data *blink_data);
}
