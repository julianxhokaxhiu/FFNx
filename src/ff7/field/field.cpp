/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

/*
 * This file contains the changes necessary to support subtractive and 25%
 * blending modes in field backgrounds. Texture pages where these blending
 * modes are used are duplicated and the tile data modified to point to these
 * new pages which have the correct blending mode set.
 */

#include "../../log.h"
#include "../../patch.h"
#include "../../common.h"
#include "../../gl.h"
#include "../widescreen.h"
#include "../defs.h"

#include "opcode.h"
#include "background.h"
#include "defs.h"
#include "enter.h"
#include "model.h"
#include "kawai.h"

#include <bit>

namespace ff7::field
{
    static int (*field_draw_effect_pages_original)(ff7_game_obj*, uint16_t, uint16_t);

    static int field_draw_effect_pages(ff7_game_obj* game_object, uint16_t first_page, uint16_t end_page)
    {
        const char* field_name = get_current_field_name();
        if (!field_name || strcmp(field_name, "mogu_1") != 0 || first_page != 15 || end_page != 26)
            return field_draw_effect_pages_original(game_object, first_page, end_page);

        // PSX draws the water at depth 4094 before the screen lighting at depth 1.
        const int result = field_draw_effect_pages_original(game_object, 24, end_page);
        const uint32_t depthtest = current_state.depthtest;
        const uint32_t depthmask = current_state.depthmask;
        internal_set_renderstate(V_DEPTHTEST, false, (game_obj*)game_object);
        internal_set_renderstate(V_DEPTHMASK, false, (game_obj*)game_object);
        field_draw_effect_pages_original(game_object, first_page, 24);
        internal_set_renderstate(V_DEPTHTEST, depthtest, (game_obj*)game_object);
        internal_set_renderstate(V_DEPTHMASK, depthmask, (game_obj*)game_object);
        return result;
    }

    // helper function initializes page dst, copies texture from src and applies
    // blend_mode
    void field_load_textures_helper(struct ff7_game_obj *game_object, struct struc_3 *struc_3, uint32_t src, uint32_t dst, uint32_t blend_mode)
    {
        struct ff7_tex_header *tex_header;

        ff7_externals.make_struc3(blend_mode, struc_3);

        tex_header = (struct ff7_tex_header *)common_externals.create_tex_header();

        ff7_externals.field_layers[dst]->tex_header = tex_header;

        if(ff7_externals.field_layers[src]->type == 1) ff7_externals.make_field_tex_header_pal(tex_header);
        if(ff7_externals.field_layers[src]->type == 2) {
            ff7_externals.make_field_tex_header(tex_header);
            tex_header->color_key = 3;
        }

        struc_3->tex_header = tex_header;

        if(src != dst)
        {
            ff7_externals.field_layers[dst]->image_data = external_malloc(256 * 256);
            memcpy(ff7_externals.field_layers[dst]->image_data, ff7_externals.field_layers[src]->image_data, 256 * 256);
        }

        tex_header->image_data = (unsigned char*)ff7_externals.field_layers[dst]->image_data;

        tex_header->file.pc_name = (char*)external_malloc(1024);
        sprintf(tex_header->file.pc_name, "field/%s/%s_%02i", strchr(ff7_externals.field_file_name, '\\') + 1, strchr(ff7_externals.field_file_name, '\\') + 1, src);

        ff7_externals.field_layers[dst]->graphics_object = ff7_externals._load_texture(1, PT_S2D, struc_3, 0, game_object->dx_sfx_something);
        ff7_externals.field_layers[dst]->present = true;
    }

    void field_load_textures(struct ff7_game_obj *game_object, struct struc_3 *struc_3)
    {
        uint32_t i;

        ff7_externals.field_convert_type2_layers();

        for(i = 0; i < 29; i++)
        {
            uint32_t blend_mode = 4;

            if(!ff7_externals.field_layers[i]->present) continue;

            if(ff7_externals.field_layers[i]->type == 1)
            {
                if(i >= 24) blend_mode = 0;
                else if(i >= 15) blend_mode = 1;

                // fr_e map uses this blend mode for these specific texture slots
                if(i >= 15 && i <= 18 && *common_externals.current_field_id == 347) blend_mode = 2;
            }
            else if(ff7_externals.field_layers[i]->type == 2)
            {
                if(i >= 40) blend_mode = 0;
                else if(i >= 33) blend_mode = 1;
            }
            else ffnx_glitch("unknown field layer type %i\n", ff7_externals.field_layers[i]->type);

            field_load_textures_helper(game_object, struc_3, i, i, blend_mode);

            // these magic numbers have been gleaned from original source data
            // the missing blend modes in question are used in exactly these pages
            // and copying them in this manner does not risk overwriting any other
            // data
            if(i >= 15 && i <= 18 && ff7_externals.field_layers[i]->type == 1) field_load_textures_helper(game_object, struc_3, i, i + 14, 2);
            if(i >= 15 && i <= 20 && ff7_externals.field_layers[i]->type == 1) field_load_textures_helper(game_object, struc_3, i, i + 18, 3);
        }

        *ff7_externals.layer2_end_page += 18;
    }

    uint32_t field_open_flevel_siz()
    {
        struct lgp_file *f = lgp_open_file("flevel.siz", 1);

        if (0 == f) {
            return 0;
        }

        uint32_t size = lgp_get_filesize(f, 1);
        char* buffer = new char[size];

        lgp_read_file(f, 1, buffer, size);

        // Increase from 787 (field map count) to 1200
        const uint32_t max_map_count = 1200;
        uint32_t* uncompressed_sizes = reinterpret_cast<uint32_t*>(buffer);
        uint32_t count = size / sizeof(uint32_t);
        uint32_t* flevel_sizes = reinterpret_cast<uint32_t*>(ff7_externals.field_map_infos + 0xBC);

        if (count > max_map_count) {
            count = max_map_count;
        }

        for (uint32_t i = 0; i < count; ++i) {
            flevel_sizes[i * 0x34] = uncompressed_sizes[i] + 4000000; // +2 MB compared to the original implementation
        }

        // Force a value if not specified by the flevel.siz
        for (uint32_t i = count; i < max_map_count; ++i) {
            flevel_sizes[i * 0x34] = 4000000;
        }

        delete[] buffer;

        return 1;
    }

    void ff7_field_set_fade_quad_size(int x, int y, int width, int height)
    {
        if(widescreen_enabled)
        {
            x -= abs(wide_viewport_x);
            width += (wide_viewport_width - game_width);
        }
        if(enable_uncrop)
        {
            y -= ff7_field_center ? 16 : 0;
            height += 32;
        }
        ff7_externals.field_sub_63AC3F(x, y, width, height);
    }

    void ff7_field_evaluate_encounter_rate()
    {
        field_event_data* field_event_data_array = (*ff7_externals.field_event_data_ptr);
        int original_movement_speed = field_event_data_array[*ff7_externals.field_player_model_id].movement_speed;
        field_event_data_array[*ff7_externals.field_player_model_id].movement_speed = original_movement_speed / common_frame_multiplier;
        ff7_externals.field_evaluate_encounter_rate_60B2C6();
        field_event_data_array[*ff7_externals.field_player_model_id].movement_speed = original_movement_speed;
    }

    void field_animate_3d_models()
    {
        ff7_externals.field_animate_3d_models_6392BB();
        advance_kawai_effects(common_frame_multiplier);
    }

    int ff7_apply_KAWAI_op_code(int sub_code, ff7_hrc_polygon_data *ff7_hrc_polygon_data, ff7_kawai_opcode_params *opcode_params, int model_pos_xy, int model_pos_z, int model_id, int *sub_code_ret)
    {
        ff7::field::ff7_kawai_current_model_id = model_id;

        int complete = 0;
        if (apply_kawai_effect(model_id, sub_code, ff7_hrc_polygon_data, sub_code_ret, &complete))
            return complete;
        return ff7_externals.field_apply_kawai_op_64A070(sub_code, ff7_hrc_polygon_data, opcode_params, model_pos_xy, model_pos_z, model_id, sub_code_ret);
    }

    static int (*native_below_level_color)(ff7_hrc_polygon_data*, int16_t*);

    static void field_draw_kawai_model(uint32_t frame, anim_header* animation, struc_110* instance,
        hrc_data* model, ff7_game_obj* game)
    {
        KawaiModelScope scope(instance);
        ((void (*)(uint32_t, anim_header*, struc_110*, hrc_data*, ff7_game_obj*))ff7_externals.draw_3d_model)
            (frame, animation, instance, model, game);
    }

    static int splash_below_level_color(ff7_hrc_polygon_data* model, int16_t* params)
    {
        // KAWAI 09's particles still use the game's splash renderer.
        return has_kawai_splash_color() ? 1 : native_below_level_color(model, params);
    }

    static bool recalculate_background_depths = false;

    void field_load_tut_file(ff7_game_obj* game_object)
    {
        ff7_externals.field_load_tut_file_60DAD4(game_object);

        // The loader must copy the original near plane to the foreground-depth threshold first.
        const float near_plane = std::bit_cast<float>(game_object->field_9A0);
        recalculate_background_depths = near_plane > 50.0f;
        if (recalculate_background_depths)
        {
            game_object->field_9A0 = std::bit_cast<uint32_t>(50.0f);
        }
    }

    void field_initialize_background_depths(int world_x, int world_y)
    {
        ((void (*)(int, int))ff7_externals.field_sub_661B68)(world_x, world_y);

        if (recalculate_background_depths)
        {
            // PC field files contain baked Z values; rebuild them from the retained raw tile depths.
            field_tile* tiles = *ff7_externals.field_layer2_tiles;
            for (int i = 0; i < *ff7_externals.field_layer2_tiles_num; ++i)
            {
                tiles[i].z = ff7_externals.field_layer_sub_623C0F(
                    ff7_externals.field_camera_rotation_matrix_CFF3D8, tiles[i].flags,
                    2 * tiles[i].x + 320, 2 * tiles[i].y + 240);
            }
        }
    }

    void ff7_field_hook_init()
    {
        field_draw_effect_pages_original = (decltype(field_draw_effect_pages_original))get_relative_call(ff7_externals.field_draw_everything, 0x348);
        replace_call_function(ff7_externals.field_draw_everything + 0x348, field_draw_effect_pages);

        // Fix camera model clipping in various fields
        replace_function(ff7_externals.field_culling_model_639252, ff7_field_do_draw_3d_model);
        replace_call_function(ff7_externals.open_field_file + 0x9E, field_load_tut_file);
        replace_call_function(ff7_externals.enter_field + 0xB5, field_load_tut_file);
        replace_call_function(ff7_externals.enter_field + 0x2CA, field_load_tut_file);
        replace_call_function(ff7_externals.field_sub_60DCED + 0x94, field_initialize_background_depths);

        std::copy(common_externals.execute_opcode_table, &common_externals.execute_opcode_table[OPCODE_COUNT - 1], &original_opcode_table[0]);
        // Init stuff
        replace_call_function(ff7_externals.field_sub_60DCED + 0x178, ff7_field_initialize_variables);

        // Model movement (walk, run) fps fix + allow footstep sfx
        replace_call_function(ff7_externals.field_loop_sub_63C17F + 0x5DD, ff7_field_update_models_position);
        replace_call_function(ff7_externals.field_update_models_positions + 0x8BC, ff7_field_update_player_model_position);
        replace_call_function(ff7_externals.field_update_models_positions + 0x9E8, ff7_field_update_single_model_position);
        replace_call_function(ff7_externals.field_update_models_positions + 0x9AA, ff7_field_check_collision_with_target);
        replace_call_function(common_externals.execute_opcode_table[OFST] + 0x46, ff7_opcode_multiply_get_bank_value);

        // Model rotation
        byte jump_to_OFST_update[] = {0xE9, 0xE6, 0x01, 0x00, 0x00};
        replace_call_function(ff7_externals.field_update_models_positions + 0x7C, ff7_field_update_models_rotation_new);
        memcpy_code(ff7_externals.field_update_models_positions + 0x81, jump_to_OFST_update, sizeof(jump_to_OFST_update));

        // woa_* background animation fix
        replace_call_function(ff7_externals.field_loop_sub_63C17F + 0x1A6, ff7_field_update_background_original);

        if(ff7_fps_limiter >= FPS_LIMITER_30FPS)
        {
            if(ff7_fps_limiter == FPS_LIMITER_60FPS)
            {
                // Partial animation fps fix
                patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANMX1], (DWORD)&opcode_script_partial_animation_wrapper);
                patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANMX2], (DWORD)&opcode_script_partial_animation_wrapper);
                patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANIM1], (DWORD)&opcode_script_partial_animation_wrapper);
                patch_code_dword((uint32_t)&common_externals.execute_opcode_table[CANIM2], (DWORD)&opcode_script_partial_animation_wrapper);

                // Model movement fps fix for ladder and jump
                patch_code_byte(ff7_externals.field_update_models_positions + 0x1041, 0x2 - common_frame_multiplier / 2);
                patch_code_byte(ff7_externals.field_update_models_positions + 0x189A, 0x2 - common_frame_multiplier / 2);
                replace_call_function(common_externals.execute_opcode_table[JUMP] + 0x1F1, ff7_opcode_multiply_get_bank_value);
                patch_divide_code<int>(ff7_externals.field_update_models_positions + 0xC89, common_frame_multiplier * 2);
                patch_divide_code<int>(ff7_externals.field_update_models_positions + 0xE48, common_frame_multiplier * 2);

                // Encounter rate fix
                replace_call_function(ff7_externals.field_update_models_positions + 0x90F, ff7_field_evaluate_encounter_rate);

                // Text box message fix
                patch_code_byte((uint32_t)ff7_externals.field_text_box_window_paging_631945 + 0xFD, 0x5 + common_frame_multiplier / 2);
                patch_divide_code<byte>((uint32_t)ff7_externals.field_text_box_window_paging_631945 + 0x100, common_frame_multiplier);
                patch_divide_code<WORD>((uint32_t)ff7_externals.field_text_box_window_paging_631945 + 0x111, common_frame_multiplier);
                patch_code_byte((uint32_t)ff7_externals.field_text_box_window_paging_631945 + 0x141, 0x4 + common_frame_multiplier / 2);
                patch_code_byte((uint32_t)ff7_externals.field_text_box_window_opening_6317A9 + 0x3D, 0x2 + common_frame_multiplier / 2);
                patch_code_byte((uint32_t)ff7_externals.field_text_box_window_opening_6317A9 + 0xD2, 0x2 + common_frame_multiplier / 2);
                patch_code_byte(ff7_externals.field_text_box_window_closing_632EB8 + 0x64, 0x2 + common_frame_multiplier / 2);
                patch_code_byte(ff7_externals.field_text_box_window_closing_632EB8 + 0xBF, 0x2 + common_frame_multiplier / 2);
                patch_divide_code<short>(ff7_externals.field_text_box_window_reverse_paging_632CAA + 0x42, common_frame_multiplier);
                patch_divide_code<short>(ff7_externals.field_opcode_message_update_loop_630D50 + 0x1AC, common_frame_multiplier);
                patch_divide_code<short>(ff7_externals.field_opcode_message_update_loop_630D50 + 0x2CF, common_frame_multiplier);
                patch_divide_code<short>((uint32_t)ff7_externals.field_opcode_ask_update_loop_6310A1 + 0x1AC, common_frame_multiplier);
                patch_divide_code<byte>((uint32_t)ff7_externals.field_opcode_ask_update_loop_6310A1 + 0x3CC, common_frame_multiplier);

                // Fade in and fade out screen transitions
                patch_divide_code<short>(ff7_externals.field_initialize_variables + 0x123, common_frame_multiplier);
                patch_code_byte(ff7_externals.field_handle_screen_fading + 0x210, 25 * common_frame_multiplier);
                patch_code_int(ff7_externals.field_handle_screen_fading + 0x240, 25 * common_frame_multiplier - 1);
            }

            // Smooth background movement for both 30 fps mode and 60 fps mode
            replace_call_function(ff7_externals.field_draw_everything + 0x34, ff7_field_set_world_coordinate_640EB7);
            replace_call_function(ff7_externals.field_loop_sub_63C17F + 0x1A6, ff7_field_update_background_smooth);
            replace_call_function(ff7_externals.compute_and_submit_draw_gateways_arrows_64DA3B + 0x357, ff7_field_submit_draw_arrow);
            replace_call_function(ff7_externals.compute_and_submit_draw_gateways_arrows_64DA3B + 0x63C, ff7_field_submit_draw_arrow);
            replace_call_function(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x284, ff7_field_submit_draw_cursor);
        }

        // Movie model animation fps fix
        replace_call_function(ff7_externals.field_update_models_positions + 0x68D, ff7_field_update_model_animation_frame);
        replace_call_function(ff7_externals.field_update_models_positions + 0x919, ff7_field_update_model_animation_frame);
        replace_call_function(ff7_externals.field_update_models_positions + 0xA2B, ff7_field_update_model_animation_frame);
        replace_call_function(ff7_externals.field_update_models_positions + 0xE8C, ff7_field_update_model_animation_frame);

        // Background scroll fps fix
        replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x34, ff7_opcode_divide_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x4D, ff7_opcode_divide_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x68, ff7_opcode_divide_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[BGSCR] + 0x81, ff7_opcode_divide_get_bank_value);
        replace_function(ff7_externals.opcode_shake, opcode_script_SHAKE);

        // Camera fps fix
        replace_call_function(common_externals.execute_opcode_table[SCRLC] + 0x3B, ff7_opcode_multiply_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[SCRLA] + 0x72, ff7_opcode_multiply_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[SCR2DC] + 0x3C, ff7_opcode_multiply_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[SCR2DL] + 0x3C, ff7_opcode_multiply_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[SCRLP] + 0xA7, ff7_opcode_multiply_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[NFADE] + 0x89, ff7_opcode_divide_get_bank_value);
        replace_call_function(common_externals.execute_opcode_table[VWOFT] + 0xCC, ff7_opcode_multiply_get_bank_value);
        patch_code_dword((uint32_t)&common_externals.execute_opcode_table[FADE], (DWORD)&opcode_script_FADE);

        // Movie fps fix
        patch_code_dword((uint32_t)&common_externals.execute_opcode_table[MVIEF], (DWORD)&opcode_script_MVIEF);
        patch_code_dword((uint32_t)&common_externals.execute_opcode_table[BGMOVIE], (DWORD)&opcode_script_BGMOVIE);

        // Others fps fix
        patch_code_dword((uint32_t)&common_externals.execute_opcode_table[WAIT], (DWORD)&opcode_script_WAIT);
        replace_function(ff7_externals.sub_611BAE, opcode_IFSW_compare_sub);

        // Fix run emulation when using the analogue key for NPCs
        patch_code_dword((uint32_t)&common_externals.execute_opcode_table[IFKEY], (DWORD)&opcode_script_IFKEY);

        // Fix KAWAI LIGHT opcode animation
        replace_call_function(ff7_externals.field_main_loop + 0xF6, field_animate_3d_models);
        replace_call_function((uint32_t)ff7_externals.field_animate_3d_models_6392BB + 0x726, ff7_apply_KAWAI_op_code);
        replace_call_function(ff7_externals.field_draw_everything + 0x17F, field_draw_kawai_model);
        replace_call_function(ff7_externals.field_draw_everything + 0x2EC, field_draw_kawai_model);
        const auto splash = get_relative_call((uint32_t)ff7_externals.field_apply_kawai_op_64A070, 0x594);
        native_below_level_color = (int (*)(ff7_hrc_polygon_data*, int16_t*))get_relative_call(splash, 0x27F);
        replace_call_function(splash + 0x27F, splash_below_level_color);

        // Fix FF7 2026 rerelease crash after battle
        patch_code_dword((uint32_t)&common_externals.execute_opcode_table[VISI], (DWORD)&opcode_script_VISI);
    }
}
