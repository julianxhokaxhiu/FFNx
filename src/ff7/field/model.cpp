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

#include "../../globals.h"
#include "../../sfx.h"
#include "../../movies.h"
#include "../../utils.h"
#include "../../log.h"
#include "../widescreen.h"

#include "utils.h"
#include "model.h"

namespace ff7::field
{
    bool ff7_field_do_draw_3d_model(short x, short y)
    {
        if(*ff7_externals.field_bg_flag_CC15E4)
            return 1;
        int left_offset_x = 40 + (widescreen_enabled ? abs(wide_viewport_x) - 50 : 0);
        int right_offset_x = 400 + (widescreen_enabled ? abs(wide_viewport_x) - 50 : 0);
        return x > ff7_externals.field_viewport_xy_CFF204->x - left_offset_x && x < ff7_externals.field_viewport_xy_CFF204->x + right_offset_x &&
            y > ff7_externals.field_viewport_xy_CFF204->y - 120 && y < ff7_externals.field_viewport_xy_CFF204->y + 460;
    }

    void ff7_field_update_models_position(int key_input_status)
    {
        bool emulate_run = !(ff7_externals.modules_global_object->current_key_input_status & 0x40) && gamepad_analogue_intent == INTENT_RUN;

        if (emulate_run)
        {
            key_input_status |= 0x40;
            ff7_externals.modules_global_object->current_key_input_status |= 0x40;
        }

        ((void(*)(int))ff7_externals.field_update_models_positions)(key_input_status);

        if (emulate_run)
        {
            key_input_status &= ~0x40;
            ff7_externals.modules_global_object->current_key_input_status &= ~0x40;
        }

        for(int model_idx = 0; model_idx < (int)(*ff7_externals.field_n_models); model_idx++)
        {
            // Reset movement frame index for all models if they are not walking/running
            if((*ff7_externals.field_event_data_ptr)[model_idx].movement_type != 1)
            {
                external_model_data[model_idx].moveFrameIndex = 0;
            }

            // Reset rotation movement frame index for all models if they are not rotating
            byte rotation_type = (*ff7_externals.field_event_data_ptr)[model_idx].rotation_steps_type;
            if(rotation_type == 0 || rotation_type == 3)
            {
                external_model_data[model_idx].rotationMoveFrameIndex = 0;
            }
        }
    }

    int ff7_field_update_player_model_position(short model_id)
    {
        field_event_data* field_event_data_array = (*ff7_externals.field_event_data_ptr);
        int original_movement_speed = field_event_data_array[model_id].movement_speed;
        int frame_multiplier = get_frame_multiplier();
        if(is_fps_running_more_than_original())
        {
            field_event_data_array[model_id].movement_speed = original_movement_speed / frame_multiplier;
        }

        int is_player_moving = ff7_externals.field_update_single_model_position(model_id);
        field_event_data_array[model_id].movement_speed = original_movement_speed;

        // Allow footsteps to be detected correctly
        if(ff7_footsteps)
            sfx_process_footstep(is_player_moving);

        return is_player_moving;
    }

    int ff7_field_update_single_model_position(short model_id)
    {
        int ret;
        int frame_multiplier = get_frame_multiplier();
        field_event_data* field_event_data_array = (*ff7_externals.field_event_data_ptr);

        if(is_fps_running_more_than_original() && *ff7_externals.field_id == 748)
        {
            // Fix softlock related to this discussion https://github.com/julianxhokaxhiu/FFNx/discussions/569. 
            // The issue is due to the fact that when Cloud climb up, there is a mini auto movement which triggers a line making Cloud jump below.
            // This script that makes Cloud jump below does not end well, which makes the next script overlapping with this.
            // If the next script is to climb back up, there is a sort of race condition where the player movability is activated.
            // Then if the player taps the DOWN button, it will go into a softlock.
            // 
            // This logic is very fragile (might cause other softlock), so, the solution is to write another logic only for this map
            int interpolationStep = external_model_data[model_id].moveFrameIndex + 1;
            if(external_model_data[model_id].moveFrameIndex == 0)
            {
                external_model_data[model_id].initialPosition = field_event_data_array[model_id].model_pos;
                ret = ff7_externals.field_update_single_model_position(model_id);
                external_model_data[model_id].updateMovementReturnValue = ret;
                external_model_data[model_id].finalPosition = field_event_data_array[model_id].model_pos;
                external_model_data[model_id].prevCollisionRadius = field_event_data_array[model_id].collision_radius;
                field_event_data_array[model_id].model_pos.x = external_model_data[model_id].initialPosition.x + ((external_model_data[model_id].finalPosition.x - external_model_data[model_id].initialPosition.x) * interpolationStep) / frame_multiplier;
                field_event_data_array[model_id].model_pos.y = external_model_data[model_id].initialPosition.y + ((external_model_data[model_id].finalPosition.y - external_model_data[model_id].initialPosition.y) * interpolationStep) / frame_multiplier;
                field_event_data_array[model_id].model_pos.z = external_model_data[model_id].initialPosition.z + ((external_model_data[model_id].finalPosition.z - external_model_data[model_id].initialPosition.z) * interpolationStep) / frame_multiplier;
                field_event_data_array[model_id].collision_radius = 0;
            }
            else
            {
                ret = external_model_data[model_id].updateMovementReturnValue;
                field_event_data_array[model_id].model_pos.x = external_model_data[model_id].initialPosition.x + ((external_model_data[model_id].finalPosition.x - external_model_data[model_id].initialPosition.x) * interpolationStep) / frame_multiplier;
                field_event_data_array[model_id].model_pos.y = external_model_data[model_id].initialPosition.y + ((external_model_data[model_id].finalPosition.y - external_model_data[model_id].initialPosition.y) * interpolationStep) / frame_multiplier;
                field_event_data_array[model_id].model_pos.z = external_model_data[model_id].initialPosition.z + ((external_model_data[model_id].finalPosition.z - external_model_data[model_id].initialPosition.z) * interpolationStep) / frame_multiplier;
            }

            if((external_model_data[model_id].moveFrameIndex + 1) % frame_multiplier == 0)
            {
                field_event_data_array[model_id].collision_radius = external_model_data[model_id].prevCollisionRadius;
            }

            external_model_data[model_id].moveFrameIndex = (external_model_data[model_id].moveFrameIndex + 1) % frame_multiplier;
        }
        else if(is_fps_running_more_than_original())
        {
            int interpolationStep = external_model_data[model_id].moveFrameIndex + 1;
            if(external_model_data[model_id].moveFrameIndex == 0)
            {
                external_model_data[model_id].initialPosition = field_event_data_array[model_id].model_pos;
                ret = ff7_externals.field_update_single_model_position(model_id);
                external_model_data[model_id].updateMovementReturnValue = ret;
                external_model_data[model_id].finalPosition = field_event_data_array[model_id].model_pos;
                field_event_data_array[model_id].model_pos.x = external_model_data[model_id].initialPosition.x + ((external_model_data[model_id].finalPosition.x - external_model_data[model_id].initialPosition.x) * interpolationStep) / frame_multiplier;
                field_event_data_array[model_id].model_pos.y = external_model_data[model_id].initialPosition.y + ((external_model_data[model_id].finalPosition.y - external_model_data[model_id].initialPosition.y) * interpolationStep) / frame_multiplier;
                field_event_data_array[model_id].model_pos.z = external_model_data[model_id].initialPosition.z + ((external_model_data[model_id].finalPosition.z - external_model_data[model_id].initialPosition.z) * interpolationStep) / frame_multiplier;
            }
            else
            {
                ret = external_model_data[model_id].updateMovementReturnValue;
                field_event_data_array[model_id].model_pos.x = external_model_data[model_id].initialPosition.x + ((external_model_data[model_id].finalPosition.x - external_model_data[model_id].initialPosition.x) * interpolationStep) / frame_multiplier;
                field_event_data_array[model_id].model_pos.y = external_model_data[model_id].initialPosition.y + ((external_model_data[model_id].finalPosition.y - external_model_data[model_id].initialPosition.y) * interpolationStep) / frame_multiplier;
                field_event_data_array[model_id].model_pos.z = external_model_data[model_id].initialPosition.z + ((external_model_data[model_id].finalPosition.z - external_model_data[model_id].initialPosition.z) * interpolationStep) / frame_multiplier;
            }
            external_model_data[model_id].moveFrameIndex = (external_model_data[model_id].moveFrameIndex + 1) % frame_multiplier;
        }
        else
        {
            ret = ff7_externals.field_update_single_model_position(model_id);
        }

        return ret;
    }

    int ff7_field_check_collision_with_target(field_event_data* field_event_model, short target_collision_radius)
    {
        int ret;
        int frame_multiplier = get_frame_multiplier();
        int model_id = std::distance(*ff7_externals.field_event_data_ptr, field_event_model);

        if(is_fps_running_more_than_original())
        {
            if(external_model_data[model_id].moveFrameIndex == 0)
            {
                ret = ff7_externals.field_check_collision_with_target(field_event_model, target_collision_radius);
                external_model_data[model_id].wasNotCollidingWithTarget = ret;
            }
            else
            {
                ret = external_model_data[model_id].wasNotCollidingWithTarget;
            }
        }
        else
        {
            ret = ff7_externals.field_check_collision_with_target(field_event_model, target_collision_radius);
        }

        return ret;
    }

    void ff7_field_update_models_rotation_new()
    {
        for(int model_idx = 0; model_idx < *ff7_externals.field_n_models; model_idx++)
        {
            auto &field_event_data = (*ff7_externals.field_event_data_ptr)[model_idx];
            byte rotation_type = field_event_data.rotation_steps_type;
            if(rotation_type)
            {
                // Legacy code still works, but when in 60FPS, the steps index and number of steps are modified
                uint32_t rotation_n_steps = field_event_data.rotation_n_steps;
                uint32_t rotation_steps_idx = field_event_data.rotation_step_idx;

                if(is_fps_running_more_than_original())
                {
                    rotation_n_steps *= get_frame_multiplier();
                    rotation_steps_idx = rotation_steps_idx * get_frame_multiplier() + external_model_data[model_idx].rotationMoveFrameIndex;
                }

                if(rotation_type == 1)
                {
                    field_event_data.rotation_curr_value = ff7_externals.field_get_linear_interpolated_value(
                        field_event_data.rotation_initial,
                        field_event_data.rotation_final,
                        rotation_n_steps,
                        rotation_steps_idx
                    );

                    if(field_event_data.rotation_step_idx == field_event_data.rotation_n_steps)
                        field_event_data.rotation_steps_type = 3;
                    else
                    {
                        if(is_fps_running_more_than_original())
                        {
                            external_model_data[model_idx].rotationMoveFrameIndex = (external_model_data[model_idx].rotationMoveFrameIndex + 1) % get_frame_multiplier();
                            if(external_model_data[model_idx].rotationMoveFrameIndex == 0)
                                field_event_data.rotation_step_idx++;
                        }
                        else
                        {
                            field_event_data.rotation_step_idx++;
                        }
                    }
                }
                else if(rotation_type == 2)
                {
                    field_event_data.rotation_curr_value = ff7_externals.field_get_smooth_interpolated_value(
                        field_event_data.rotation_initial,
                        field_event_data.rotation_final,
                        rotation_n_steps,
                        rotation_steps_idx
                    );

                    if(field_event_data.rotation_step_idx == field_event_data.rotation_n_steps)
                        field_event_data.rotation_steps_type = 3;
                    else
                    {
                        if(is_fps_running_more_than_original())
                        {
                            external_model_data[model_idx].rotationMoveFrameIndex = (external_model_data[model_idx].rotationMoveFrameIndex + 1) % get_frame_multiplier();
                            if(external_model_data[model_idx].rotationMoveFrameIndex == 0)
                                field_event_data.rotation_step_idx++;
                        }
                        else
                        {
                            field_event_data.rotation_step_idx++;
                        }
                    }
                }
            }
        }
    }

    void ff7_handle_KAWAI_reset()
    {
        static WORD last_field_id = 0;

        if (last_field_id != *common_externals.current_field_id)
        {
            last_field_id = *common_externals.current_field_id;

            // Reset eyes blinking
            ff7_externals.field_model_blink_data_D000C8->blink_left_eye_mode = 0;
            ff7_externals.field_model_blink_data_D000C8->blink_right_eye_mode = 0;

            for(int i = 0; i < FF7_MAX_NUM_MODEL_ENTITIES; i++)
            {
                // Reset mouths
                ff7_model_data[i].current_mouth_idx = 0;
                if (ff7_model_data[i].mouth_tex) ff7_externals.field_unload_model_tex(ff7_model_data[i].mouth_tex);
                ff7_model_data[i].mouth_tex = NULL;

                // Reset KAWAI state
                ff7_model_data[i].is_kawai_active = false;
                ff7_model_data[i].do_kawai_repeat = false;
                ff7_model_data[i].init_kawai_opcode = 0x0;
                ff7_model_data[i].init_kawai_params = nullptr;
                ff7_model_data[i].exec_kawai_opcode = 0x0;
                ff7_model_data[i].exec_kawai_params = nullptr;
            }
        }
    }

    int ff7_internal_blink_eye(field_animation_data *field_anim_data, field_model_blink_data *blink_data)
    {
        int i;
        ff7_polygon_set *polygon_set = nullptr;
        struct hrc_bone *bones;
        hrc_data *hrc_data;
        int blink_left_eye_mode;
        int blink_right_eye_mode;

        if ( field_anim_data->anim_frame_object )
        {
            hrc_data = field_anim_data->anim_frame_object->hrc_data;
            if ( hrc_data )
            {
                int bone_idx = 0;
                bones = field_anim_data->anim_frame_object->hrc_data->bones;
                if ( (char)field_anim_data->eye_texture_idx >= 33 )
                    return 1;
                while ( bone_idx < (signed int)hrc_data->num_bones && _strcmpi(bones->bone_name, "head") )
                {
                    ++bone_idx;
                    ++bones;
                }
                if ( bone_idx == hrc_data->num_bones ) return 0;
                polygon_set = bones->rsd_array->rsd_data->polygon_set;
                ff7_externals.field_sub_6A2736(polygon_set);
                // Mouth replacement logic
                if (ff7_advanced_blinking && polygon_set && ff7_model_data[blink_data->model_id].mouth_tex)
                {
                    if (polygon_set->hundred_data_group_array[3] != NULL)
                    {
                        polygon_set->hundred_data_group_array[3] = ff7_model_data[blink_data->model_id].mouth_tex;
                        polygon_set->per_group_hundreds = 1;
                    }
                }
                blink_left_eye_mode = blink_data->blink_left_eye_mode;
                for ( i = 0; i < (signed int)polygon_set->numgroups && !polygon_set->hundred_data[i].texture_set; ++i );
                if ( i == polygon_set->numgroups )return 0;
                if ( blink_left_eye_mode == 1 )
                {
                    if (ff7_advanced_blinking && ff7_model_data[blink_data->model_id].left_eye_tex) polygon_set->hundred_data_group_array[1] = ff7_model_data[blink_data->model_id].left_eye_tex;
                    ff7_externals.field_sub_6A2782(i, &polygon_set->hundred_data[i], polygon_set);
                }
                else
                {
                    if ( blink_left_eye_mode != 2 )
                    {
                        if (ff7_advanced_blinking && !ff7_model_data[blink_data->model_id].has_mouth) polygon_set->per_group_hundreds = 0;
                        return 0;
                    }
                    polygon_set->per_group_hundreds = 1;
                    if ( !field_anim_data->static_left_eye_tex )
                    goto LABEL_22;
                    ff7_externals.field_sub_6A2782(i, (p_hundred *)field_anim_data->static_left_eye_tex, polygon_set);
                }
                blink_right_eye_mode = blink_data->blink_right_eye_mode;
                if ( ++i == polygon_set->numgroups ) return 0;
                if ( blink_right_eye_mode == 1 )
                {
                    if (ff7_advanced_blinking && ff7_model_data[blink_data->model_id].right_eye_tex) polygon_set->hundred_data_group_array[2] = ff7_model_data[blink_data->model_id].right_eye_tex;
                    ff7_externals.field_sub_6A2782(i, &polygon_set->hundred_data[i], polygon_set);
                    return 1;
                }
                if ( blink_right_eye_mode != 2 )
                {
                    if (ff7_advanced_blinking && !ff7_model_data[blink_data->model_id].has_mouth) polygon_set->per_group_hundreds = 0;
                    return 0;
                }
                polygon_set->per_group_hundreds = 1;
                if ( field_anim_data->static_right_eye_tex )
                {
                    ff7_externals.field_sub_6A2782(i, (p_hundred *)field_anim_data->static_right_eye_tex, polygon_set);
                    return 1;
                }
            LABEL_22:
                ff7_externals.field_sub_6A2782(i, &polygon_set->hundred_data[i], polygon_set);
                return 0;
            }
        }

        return 0;
    }

    int ff7_field_blink_eye_sub_649B50(field_animation_data *field_anim_data, field_model_blink_data *blink_data)
    {
        int ret = 0;

        // Custom eyes + mouth fetching
        byte curr_entity_id = *ff7_externals.current_entity_id;
        byte curr_model_id = blink_data->model_id;
        byte curr_eye_index = MAXBYTE;
        static char curr_model_name[10]{0};
        bool is_npc = false;

        if (ff7_advanced_blinking && curr_model_id != MAXBYTE)
        {
            byte left_eye_index = blink_data->blink_left_eye_mode;
            byte right_eye_index = blink_data->blink_right_eye_mode;
            byte mouth_index = ff7_model_data[curr_model_id].current_mouth_idx;
            curr_eye_index = field_anim_data->eye_texture_idx;

            _splitpath((const char*)(*ff7_externals.field_models_data + (10380 * curr_model_id) + 512), NULL, NULL, curr_model_name, NULL);

            if (trace_all || trace_opcodes) ffnx_trace("field_blink_eye_sub: curr_entity_id=%u,curr_model_id=%u,curr_eye_index=%u,curr_mouth_index=%u,curr_model_name=%s\n", curr_entity_id, curr_model_id, curr_eye_index, mouth_index, curr_model_name);

            if (curr_eye_index < 10)
            {
                char directpath[MAX_PATH + sizeof(basedir)];
                char filename[10];
                char ext[4];
                bool ext_left_eye_found = false, ext_right_eye_found = false;

                // NPCs always default on Cloud eyes/mouth
                if (curr_eye_index == 9)
                {
                    curr_eye_index = 0;
                    is_npc = true;
                }

                if (ff7_externals.field_models_eye_blink_buffer[curr_eye_index].has_eyes)
                {
                    // LEFT EYE
                    _splitpath(ff7_externals.field_models_eye_blink_buffer[curr_eye_index].static_left_eye_filename, NULL, NULL, filename, ext);

                    _snprintf(directpath, sizeof(directpath), "%s/%s/flevel/eye_%s_%d.TEX", basedir, direct_mode_path.c_str(), curr_model_name, left_eye_index);
                    if (ext_left_eye_found = fileExists(directpath))
                        _snprintf(ff7_model_data[curr_model_id].left_eye_tex_filename, 1024, "eye_%s_%d%s", curr_model_name, left_eye_index, ext);
                    else
                    {
                        if (left_eye_index > 2 && (trace_all || trace_direct || trace_opcodes)) ffnx_trace("subcode[EYETX]: Custom left eye texture not found: %s\n", directpath);

                        // If it is an NPC use the generic NPC name instead of Cloud
                        if (is_npc) _snprintf(filename, sizeof(filename), "npc_eye2");

                        _snprintf(directpath, sizeof(directpath), "%s/%s/flevel/%s_%d.TEX", basedir, direct_mode_path.c_str(), filename, left_eye_index);
                        if (ext_left_eye_found = fileExists(directpath))
                            _snprintf(ff7_model_data[curr_model_id].left_eye_tex_filename, 1024, "%s_%d%s", filename, left_eye_index, ext);
                        else
                        {
                            if (left_eye_index > 2 && (trace_all || trace_direct || trace_opcodes)) ffnx_trace("subcode[EYETX]: Custom left eye texture not found: %s\n", directpath);

                            // If it is an NPC, and nothing was found so far, switch back to Cloud eye
                            if (is_npc) _splitpath(ff7_externals.field_models_eye_blink_buffer[curr_eye_index].static_left_eye_filename, NULL, NULL, filename, ext);
                            _snprintf(ff7_model_data[curr_model_id].left_eye_tex_filename, 1024, "%s%s", filename, ext);
                        }
                    }

                    // RIGHT EYE
                    _splitpath(ff7_externals.field_models_eye_blink_buffer[curr_eye_index].static_right_eye_filename, NULL, NULL, filename, ext);

                    _snprintf(directpath, sizeof(directpath), "%s/%s/flevel/eye_%sr_%d.TEX", basedir, direct_mode_path.c_str(), curr_model_name, right_eye_index);
                    if (ext_right_eye_found = fileExists(directpath))
                        _snprintf(ff7_model_data[curr_model_id].right_eye_tex_filename, 1024, "eye_%sr_%d%s", curr_model_name, right_eye_index, ext);
                    else
                    {
                        if (right_eye_index > 2 && (trace_all || trace_direct || trace_opcodes)) ffnx_trace("subcode[EYETX]: Custom right eye texture not found: %s\n", directpath);

                        // If it is an NPC use the generic NPC name instead of Cloud
                        if (is_npc) _snprintf(filename, sizeof(filename), "npc_eye2r");

                        _snprintf(directpath, sizeof(directpath), "%s/%s/flevel/%s_%d.TEX", basedir, direct_mode_path.c_str(), filename, right_eye_index);
                        if (ext_right_eye_found = fileExists(directpath))
                            _snprintf(ff7_model_data[curr_model_id].right_eye_tex_filename, 1024, "%s_%d%s", filename, right_eye_index, ext);
                        else
                        {
                            if (right_eye_index > 2 && (trace_all || trace_direct || trace_opcodes)) ffnx_trace("subcode[EYETX]: Custom right eye texture not found: %s\n", directpath);

                            // If it is an NPC, and nothing was found so far, switch back to Cloud eye
                            if (is_npc) _splitpath(ff7_externals.field_models_eye_blink_buffer[curr_eye_index].static_right_eye_filename, NULL, NULL, filename, ext);
                            _snprintf(ff7_model_data[curr_model_id].right_eye_tex_filename, 1024, "%s%s", filename, ext);
                        }
                    }

                    // Reload TEX data in memory
                    if(field_anim_data->static_left_eye_tex) ff7_externals.field_unload_model_tex(field_anim_data->static_left_eye_tex);
                    if(field_anim_data->static_right_eye_tex) ff7_externals.field_unload_model_tex(field_anim_data->static_right_eye_tex);
                    curr_model_data.has_eyes = 1;
                    curr_model_data.static_left_eye_filename = ff7_model_data[curr_model_id].left_eye_tex_filename;
                    curr_model_data.static_right_eye_filename = ff7_model_data[curr_model_id].right_eye_tex_filename;
                    ff7_externals.field_load_model_eye_tex(&curr_model_data, field_anim_data);
                    ff7_model_data[curr_model_id].left_eye_tex = field_anim_data->static_left_eye_tex;
                    ff7_model_data[curr_model_id].right_eye_tex = field_anim_data->static_right_eye_tex;

                    // Restore original curr_eye_index
                    curr_eye_index = field_anim_data->eye_texture_idx;
                }

                // MOUTH
                char* char_name = strtok(filename, "_");

                _snprintf(directpath, sizeof(directpath), "%s/%s/flevel/mouth_%s_%d.TEX", basedir, direct_mode_path.c_str(), curr_model_name, mouth_index);
                if (ff7_model_data[curr_model_id].has_mouth = fileExists(directpath))
                    _snprintf(ff7_model_data[curr_model_id].mouth_tex_filename, 1024, "mouth_%s_%d%s", curr_model_name, mouth_index, ext);
                else
                {
                    if (mouth_index > 0 && (trace_all || trace_direct || trace_opcodes)) ffnx_trace("subcode[EYETX]: Custom mouth texture not found: %s\n", directpath);

                    // NPC only
                    if (is_npc)
                    {
                        _snprintf(directpath, sizeof(directpath), "%s/%s/flevel/npc_mouth_%d.TEX", basedir, direct_mode_path.c_str(), mouth_index);
                        if (ff7_model_data[curr_model_id].has_mouth = fileExists(directpath))
                            _snprintf(ff7_model_data[curr_model_id].mouth_tex_filename, 1024, "npc_mouth_%d%s", mouth_index, ext);
                        else if (mouth_index > 0 && (trace_all || trace_direct || trace_opcodes)) ffnx_trace("subcode[EYETX]: Custom mouth texture not found: %s\n", directpath);
                    }
                    else
                    {
                        _snprintf(directpath, sizeof(directpath), "%s/%s/flevel/%s_mouth_%d.TEX", basedir, direct_mode_path.c_str(), filename, mouth_index);
                        if (ff7_model_data[curr_model_id].has_mouth = fileExists(directpath))
                            _snprintf(ff7_model_data[curr_model_id].mouth_tex_filename, 1024, "%s_mouth_%d%s", char_name, mouth_index, ext);
                        else if (mouth_index > 0 && (trace_all || trace_direct || trace_opcodes)) ffnx_trace("subcode[EYETX]: Custom mouth texture not found: %s\n", directpath);
                    }
                }

                // Prepare mouth tex object
                if (ff7_model_data[curr_model_id].has_mouth)
                {
                    struc_3 tex_mouth_info;
                    ff7_externals.create_struc_3_info_sub_67455E(&tex_mouth_info);
                    tex_mouth_info.base_directory = (uint32_t)ff7_externals.field_unk_909288;
                    tex_mouth_info.file_context.use_lgp = 1;
                    tex_mouth_info.file_context.lgp_num = 1;
                    tex_mouth_info.file_context.name_mangler = 0;
                    ff7_model_data[curr_model_id].mouth_tex = ff7_externals.field_load_model_tex(0, 0, ff7_model_data[curr_model_id].mouth_tex_filename, &tex_mouth_info, common_externals.get_game_object());
                }
                else
                {
                    if (ff7_model_data[curr_model_id].mouth_tex) ff7_externals.field_unload_model_tex(ff7_model_data[curr_model_id].mouth_tex);
                    ff7_model_data[curr_model_id].mouth_tex = NULL;
                }

                // Index is also treated as blink mode, if higher than 2 then "fake a closed eyes" in order to reload textures
                if (left_eye_index <= 2 || right_eye_index <= 2)
                {
                    blink_data->blink_left_eye_mode = left_eye_index;
                    blink_data->blink_right_eye_mode = right_eye_index;
                }
                else if (ext_left_eye_found || ext_right_eye_found)
                {
                    blink_data->blink_left_eye_mode = 2;
                    blink_data->blink_right_eye_mode = 2;
                }
                blink_data->model_id = curr_model_id;
            }
        }

        // Original code
        ret = ff7_internal_blink_eye(field_anim_data, blink_data);

        // 60FPS patch: wait time and blink time
        if(ff7_fps_limiter == FPS_LIMITER_60FPS)
        {
            if(blink_data->blink_left_eye_mode == 2 && blink_data->blink_right_eye_mode == 2)
            {
                auto &field_event_data = (*ff7_externals.field_event_data_ptr)[blink_data->model_id];
                if(external_model_data[blink_data->model_id].blinkFrameIndex > 0)
                {
                    field_event_data.blink_wait_frames = 0;
                    external_model_data[blink_data->model_id].blinkFrameIndex--;
                }
                else
                {
                    field_event_data.blink_wait_frames = 64 * common_frame_multiplier + getRandomInt(0, 32 * common_frame_multiplier);
                    external_model_data[blink_data->model_id].blinkFrameIndex = BLINKING_FRAMES;
                }
            }
        }

        return ret;
    }

    void ff7_field_update_model_animation_frame(short model_id)
    {
        field_event_data &model_event_data = (*ff7_externals.field_event_data_ptr)[model_id];
        const int original_animation_speed = model_event_data.animation_speed;
        if (is_overlapping_movie_playing())
        {
            if(movie_fps_ratio == 1 && ff7_fps_limiter == FPS_LIMITER_60FPS)
                model_event_data.animation_speed *= common_frame_multiplier;
            else if(movie_fps_ratio > 1 && ff7_fps_limiter < FPS_LIMITER_60FPS)
                model_event_data.animation_speed /= movie_fps_ratio;
            else if(movie_fps_ratio > 2 && ff7_fps_limiter == FPS_LIMITER_60FPS)
                model_event_data.animation_speed /= (movie_fps_ratio / 2);
        }

        ff7_externals.field_update_model_animation_frame(model_id);

        model_event_data.animation_speed = original_animation_speed;
    }

    int ff7_field_models_eye_to_model(char* model_name)
    {
        // Cloud
        if ( !_strcmpi(model_name, "AAAA") )
            return 0;
        if ( !_strcmpi(model_name, "AFIE") )
            return 0;
        if ( !_strcmpi(model_name, "BHFF") )
            return 0;
        if ( !_strcmpi(model_name, "BUGE") )
            return 0;
        if ( !_strcmpi(model_name, "DLFB") )
            return 0;
        if ( !_strcmpi(model_name, "EIHD") )
            return 0;
        if ( !_strcmpi(model_name, "EKBF") )
            return 0;
        if ( !_strcmpi(model_name, "ENAB") )
            return 0;
        if ( !_strcmpi(model_name, "HTJE") )
            return 0;

        // Tifa
        if ( !_strcmpi(model_name, "AAGB") )
            return 1;
        if ( !_strcmpi(model_name, "AXJA") )
            return 1;
        if ( !_strcmpi(model_name, "EQIB") )
            return 1;
        if ( !_strcmpi(model_name, "BIDB") )
            return 1;
        if ( !_strcmpi(model_name, "AGGB") )
            return 1;
        if ( !_strcmpi(model_name, "BUAC") )
            return 1;

        // Aerith
        if ( !_strcmpi(model_name, "AUFF") )
            return 2;
        if ( !_strcmpi(model_name, "CAHC") )
            return 2;
        if ( !_strcmpi(model_name, "AZBB") )
            return 2;
        if ( !_strcmpi(model_name, "CQGA") )
            return 2;
        if ( !_strcmpi(model_name, "DIFF") )
            return 2;
        if ( !_strcmpi(model_name, "CPJF") ) // Ifalna - shares the same eyes
            return 2;

        // Barret
        if ( !_strcmpi(model_name, "ACGD") )
            return 3;
        if ( !_strcmpi(model_name, "FQCB") )
            return 3;
        if ( !_strcmpi(model_name, "AYFB") )
            return 3;
        if ( !_strcmpi(model_name, "AIBA") )
            return 3;

        // Red XIII
        if ( !_strcmpi(model_name, "ADDA") )
            return 4;
        if ( !_strcmpi(model_name, "HVJF") )
            return 4;

        // Cid
        if ( !_strcmpi(model_name, "ABDA") )
            return 5;
        if ( !_strcmpi(model_name, "AIHB") )
            return 5;

        // Vincent
        if ( !_strcmpi(model_name, "AEHD") )
            return 6;
        if ( !_strcmpi(model_name, "BIJD") )
            return 6;

        // Yuffie
        if ( !_strcmpi(model_name, "ABJB") )
            return 7;
        if ( !_strcmpi(model_name, "FEEA") )
            return 7;
        if ( !_strcmpi(model_name, "AHDF") )
            return 7;

        // Cait Sith
        if ( !_strcmpi(model_name, "AEBC") )
            return 8;

        return 9; // Defaults to Cloud eye
    }
}
