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

#include "../../movies.h"
#include "../../cfg.h"
#include "../../globals.h"
#include "../widescreen.h"

namespace ff7::field
{
    bool is_fps_running_more_than_original()
    {
        if(is_overlapping_movie_playing())
            return movie_fps_ratio > 1;
        else
            return ff7_fps_limiter == FPS_LIMITER_60FPS;
    }

    int get_frame_multiplier()
    {
        if(is_overlapping_movie_playing())
            return movie_fps_ratio;
        else
            return common_frame_multiplier;
    }

    bool is_fieldmap_wide()
    {
        return widescreen_enabled && widescreen.getMode() != WM_DISABLED;
    }

    float field_get_linear_interpolated_value_float(float initial_value, float final_value, int n_steps, int step_idx)
    {
        return std::lerp(initial_value, final_value, step_idx / (float)n_steps);
    }

    float field_get_smooth_interpolated_value_float(float initial_value, float final_value, int n_steps, int step_idx)
    {
        float delta = final_value - initial_value;
        return initial_value + delta * (0.5f + sin(-M_PI/2.f + M_PI * (step_idx / (float)n_steps)) / 2.f);
    }

    int engine_apply_matrix_product_float_66307D(vector3<float> *input_vector, vector2<float> *output_vector, int *dummy1, int *dummy2)
    {
        int ret;
        float matrix[16];
        vector3<float> output_temp, vector_temp;
        vector3<float> input_vector_copy = *input_vector;
        ff7_game_engine_data* global_game_data = *ff7_externals.global_game_engine_data;

        ff7_externals.engine_convert_psx_matrix_to_float_matrix_row_version_661465(&global_game_data->rot_matrix, matrix);
        ff7_externals.engine_apply_matrix_product_to_vector_66CF7E(matrix, &input_vector_copy, &output_temp);
        vector_temp.x = (double)global_game_data->rot_matrix.position[0] + output_temp.x;
        vector_temp.y = (double)global_game_data->rot_matrix.position[1] + output_temp.y;
        vector_temp.z = (double)global_game_data->rot_matrix.position[2] + output_temp.z;
        if (vector_temp.z == 0.f)
        {
            ret = 0;
        }
        else
        {
            output_vector->x = vector_temp.x * global_game_data->scale / vector_temp.z + global_game_data->float_delta_x;
            output_vector->y = vector_temp.y * global_game_data->scale / vector_temp.z + global_game_data->float_delta_y;
            ret = (vector_temp.z * 0.25f);
        }
        *dummy1 = 0;
        *dummy2 = 0;
        return ret;
    }

    int field_apply_2D_translation_float_64314F(vector3<float> *input_vector, vector2<float> *output_vector)
    {
        int dummy_1, dummy_2;
        int ret;

        ff7_externals.engine_set_game_engine_rot_matrix_663673(ff7_externals.field_camera_rotation_matrix_CFF3D8);
        ff7_externals.engine_set_game_engine_position_663707(ff7_externals.field_camera_rotation_matrix_CFF3D8);
        ff7_externals.engine_set_game_engine_delta_values_661976(ff7_externals.field_viewport_xy_CFF204->x, ff7_externals.field_viewport_xy_CFF204->y);
        ret = engine_apply_matrix_product_float_66307D(input_vector, output_vector, &dummy_1, &dummy_2);
        ff7_externals.engine_set_game_engine_delta_values_661976(ff7_externals.field_max_half_viewport_width_height_CFF1F4->x, ff7_externals.field_max_half_viewport_width_height_CFF1F4->y);
        return ret;
    }
}