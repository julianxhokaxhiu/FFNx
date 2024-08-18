/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Tang-Tang Zhou                                     //
//    Copyright (C) 2023 Cosmos                                             //
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

#include "cmath"

#include "../ff7.h"
#include "../cfg.h"
#include "../renderer.h"
#include "../video/movies.h"
#include "../movies.h"
#include "../gl.h"
#include "../globals.h"
#include "../patch.h"

#include "widescreen.h"
#include "field/defs.h"
#include "field/background.h"

int viewport_width_plus_x_widescreen_fix = 750;
int swirl_framebuffer_offset_x_widescreen_fix = 106;
int swirl_framebuffer_offset_y_widescreen_fix = 64;

Widescreen widescreen;

// This function should be called at each frame after drawing backgrounds and 3d models
void Widescreen::zoomBackground()
{
    if(gl_defer_zoom()) return;

    bool is_movie_playing = *ff7_externals.word_CC1638 && !ff7_externals.modules_global_object->BGMOVIE_flag;
    int width = 0;
    if(is_movie_playing )
    {
        if(widescreen.getMovieMode() == WM_ZOOM) width = 640;
    }
    else if(widescreen.getMode() == WM_ZOOM)
    {
        auto camera_range = widescreen.getCameraRange();
        width = 2 * (camera_range.right - camera_range.left);
    }

    if(width == 0) return;

    int zoomed_x = (wide_viewport_width - width) / 2;
    float vOffset = (480 - 9 * width / 16) / 2;

    uint16_t newX = newRenderer.getInternalCoordX(zoomed_x);
    uint16_t newWidth = newRenderer.getInternalCoordX(width);
    uint16_t newY = newRenderer.getInternalCoordY(vOffset);
    uint16_t newHeight = newRenderer.getInternalCoordY(game_height - 2 * vOffset);

    if(is_movie_playing)
    {
        int frame = ffmpeg_get_movie_frame();
        auto keyPair = widescreen.getMovieKeyPair(frame);

        uint16_t newY1 = newRenderer.getInternalCoordY(vOffset + keyPair.first.v_offset);
        uint16_t newY2 = newRenderer.getInternalCoordY(vOffset + keyPair.second.v_offset);

        float t = 0.0;
        auto seqFrameCount = static_cast<float>(keyPair.second.frame - keyPair.first.frame);
        if(seqFrameCount > 0)
            t =  static_cast<float>(frame - keyPair.first.frame) / static_cast<float>(keyPair.second.frame - keyPair.first.frame);
        t = std::max(std::min(t, 1.0f), 0.0f);
        int newVOffset = std::round(std::lerp(newY1, newY2, t));
        newY = newVOffset;
    }

    newRenderer.zoomBackendFrameBuffer(newX, newY, newWidth, newHeight);
}

void ifrit_first_wave_effect_widescreen_fix_sub_66A47E(int wave_data_pointer) {
	int viewport_width_1_fix = ceil(255.f / game_width * wide_viewport_width) - 255;
	*(short*)(wave_data_pointer + 8) += wide_viewport_x;
	*(short*)(wave_data_pointer + 16) += wide_viewport_x + viewport_width_1_fix * 2;
	*(short*)(wave_data_pointer + 24) += wide_viewport_x;
	*(short*)(wave_data_pointer + 32) += wide_viewport_x + viewport_width_1_fix * 2;

    ff7_externals.engine_draw_sub_66A47E(wave_data_pointer);
}

void ifrit_second_third_wave_effect_widescreen_fix_sub_66A47E(int wave_data_pointer) {
	int viewport_width_1_fix = ceil(255.f / game_width * wide_viewport_width) - 255;
	int viewport_width_2_fix = ceil(65.f / game_width * wide_viewport_width) - 65;
	*(short*)(wave_data_pointer + 8) += wide_viewport_x + viewport_width_1_fix * 2;
	*(short*)(wave_data_pointer + 16) += wide_viewport_x + (viewport_width_1_fix + viewport_width_2_fix) * 2;
	*(short*)(wave_data_pointer + 24) += wide_viewport_x + viewport_width_1_fix * 2;
	*(short*)(wave_data_pointer + 32) += wide_viewport_x + (viewport_width_1_fix + viewport_width_2_fix) * 2;

    ff7_externals.engine_draw_sub_66A47E(wave_data_pointer);
}

void pollensalta_cold_breath_atk_white_dot_effect()
{
    effect100_data* effect_data = &ff7_externals.effect100_array_data[*ff7_externals.effect100_array_idx];
    ff7_externals.pollensalta_cold_breath_atk_draw_white_dots_547E75(*ff7_externals.pollensalta_cold_breath_white_dot_rgb_scalar);
    if (!*ff7_externals.g_is_battle_paused)
    {
        for (int i = 0; i < 400; i++)
        {
            short offset = 2 * (i % 2) + 2;
            ff7_externals.pollensalta_cold_breath_white_dots_pos[i].x = (offset + ff7_externals.pollensalta_cold_breath_white_dots_pos[i].x);
            if (ff7_externals.pollensalta_cold_breath_white_dots_pos[i].x < wide_viewport_x)
                ff7_externals.pollensalta_cold_breath_white_dots_pos[i].x = ff7_externals.pollensalta_cold_breath_white_dots_pos[i].x + wide_viewport_width;
            if (ff7_externals.pollensalta_cold_breath_white_dots_pos[i].x > wide_viewport_width + wide_viewport_x)
                ff7_externals.pollensalta_cold_breath_white_dots_pos[i].x = ff7_externals.pollensalta_cold_breath_white_dots_pos[i].x - wide_viewport_width;

            ff7_externals.pollensalta_cold_breath_white_dots_pos[i].y = (offset + ff7_externals.pollensalta_cold_breath_white_dots_pos[i].y) % wide_viewport_height;
        }

        if (effect_data->field_2 < 8)
            *ff7_externals.pollensalta_cold_breath_white_dot_rgb_scalar += 1;
        if (effect_data->field_2 > 42)
            *ff7_externals.pollensalta_cold_breath_white_dot_rgb_scalar -= 1;
        if (++effect_data->field_2 == 50)
            effect_data->field_0 = 0xFFFF;

    }
}

void ff7_widescreen_fix_chocobo_submit_quad_graphics_object(int x, int y, int width, int height, int color, int unknown, float z_value, DWORD* pointer)
{
	if(width == 640) // Replace only quad related to water effect
	{
		x = wide_viewport_x;
		width = wide_viewport_width;
		height = wide_viewport_height;
	}
	ff7_externals.generic_submit_quad_graphics_object_671D2A(x, y, width, height, color, unknown, z_value, pointer);
}

void ff7_widescreen_hook_init() {
    // Field fix
    replace_function((uint32_t)ff7_externals.field_clip_with_camera_range_6438F6, ff7::field::ff7_field_clip_with_camera_range);
    replace_function(ff7_externals.field_layer3_clip_with_camera_range_643628, ff7::field::ff7_field_layer3_clip_with_camera_range);
    replace_function(ff7_externals.field_culling_model_639252, ff7::field::ff7_field_do_draw_3d_model);
    replace_call_function(ff7_externals.field_sub_63AC66 + 0xD5, ff7::field::ff7_field_set_fade_quad_size);
    patch_code_dword(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x6A, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x80, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x94, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x9C, (uint32_t)&wide_viewport_x);
    patch_code_int(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x64, wide_viewport_width / 2);
    patch_code_int(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x7A, wide_viewport_width / 2);
    memset_code(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x39, 0x90, 12); // Remove useless culling cursor
    patch_code_int(ff7_externals.field_init_viewport_values + 0xBE, wide_viewport_width + wide_viewport_x - 60);
    patch_code_int(ff7_externals.field_init_viewport_values + 0xC8, 18);
    // For zoom field maps
    replace_call_function(ff7_externals.field_draw_everything + 0x360, ff7::field::draw_gray_quads_sub_644E90);

    // Swirl fix
    patch_code_dword(ff7_externals.swirl_loop_sub_4026D4 + 0x335, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.swirl_enter_sub_401810 + 0x21, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.swirl_enter_40164E + 0xEE, (uint32_t)&swirl_framebuffer_offset_x_widescreen_fix);
    patch_code_dword(ff7_externals.swirl_enter_40164E + 0x112, (uint32_t)&swirl_framebuffer_offset_x_widescreen_fix);
    patch_code_dword(ff7_externals.swirl_enter_40164E + 0xFB, (uint32_t)&swirl_framebuffer_offset_y_widescreen_fix);
    patch_code_dword(ff7_externals.swirl_enter_40164E + 0x11F, (uint32_t)&swirl_framebuffer_offset_y_widescreen_fix);
    patch_code_int(ff7_externals.swirl_enter_40164E + 0xE8, 85);

    // Battle fix
    patch_code_dword(ff7_externals.battle_sub_5BD050 + 0x4B, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.battle_sub_5BD050 + 0x68, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.battle_sub_5BD050 + 0x8B, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.battle_sub_5BD050 + 0xB4, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.battle_sub_5BD050 + 0x105, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.battle_sub_5BD050 + 0x122, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.battle_sub_5BD050 + 0x141, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.battle_sub_5BD050 + 0x16A, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.battle_sub_5BD050 + 0x19F, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.battle_sub_5BD050 + 0x1BB, (uint32_t)&wide_viewport_x);
    patch_code_int(ff7_externals.battle_enter + 0x1E8, wide_viewport_y);
    patch_code_int(ff7_externals.battle_enter + 0x21A, wide_viewport_height);
    patch_code_dword(ff7_externals.battle_enter + 0x229, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.battle_enter + 0x22F, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.battle_draw_quad_5BD473 + 0xDA, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.battle_draw_quad_5BD473 + 0x112, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.battle_sub_58ACB9 + 0x55, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.battle_sub_58ACB9 + 0x65, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.display_battle_damage_5BB410 + 0x23F, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.display_battle_damage_5BB410 + 0x24C, (uint32_t)&wide_viewport_x);
    patch_code_int(ff7_externals.shadow_flare_draw_white_bg_57747E + 0x18, wide_viewport_x);
    patch_code_int(ff7_externals.shadow_flare_draw_white_bg_57747E + 0x1F, wide_viewport_width / 2);
    patch_code_int(ff7_externals.pollensalta_cold_breath_atk_enter_sub_5474F0 + 0x83, wide_viewport_width);
    patch_code_int(ff7_externals.pollensalta_cold_breath_atk_enter_sub_5474F0 + 0x8D, wide_viewport_height);
    patch_code_short(ff7_externals.pollensalta_cold_breath_atk_main_loop_5476B0 + 0x191, wide_viewport_x - 200);
    patch_code_dword(ff7_externals.pollensalta_cold_breath_atk_main_loop_5476B0 + 0x39, (uint32_t)&pollensalta_cold_breath_atk_white_dot_effect);
    patch_code_int(ff7_externals.pandora_box_skill_draw_bg_flash_effect_568371 + 0x3D, wide_viewport_x - 170);

    // Battle summon fix
    replace_call_function(ff7_externals.ifrit_sub_595A05 + 0x930, ifrit_first_wave_effect_widescreen_fix_sub_66A47E);
    replace_call_function(ff7_externals.ifrit_sub_595A05 + 0xAEC, ifrit_second_third_wave_effect_widescreen_fix_sub_66A47E);
    replace_call_function(ff7_externals.ifrit_sub_595A05 + 0xCC0, ifrit_second_third_wave_effect_widescreen_fix_sub_66A47E);
    patch_code_int(ff7_externals.neo_bahamut_effect_sub_490F2A + 0x58, wide_viewport_width / 4);
    patch_code_dword(ff7_externals.neo_bahamut_effect_sub_490F2A + 0x5D, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.neo_bahamut_effect_sub_490F2A + 0x6A, (uint32_t)&wide_viewport_x);
    patch_code_int(ff7_externals.neo_bahamut_effect_sub_490F2A + 0x88, wide_viewport_width / 4);
    patch_code_dword(ff7_externals.neo_bahamut_effect_sub_490F2A + 0x1A2, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.neo_bahamut_effect_sub_490F2A + 0x1AF, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.run_bahamut_neo_main_48C2A1 + 0x140, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.run_bahamut_neo_main_48C2A1 + 0x15B, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.run_bahamut_neo_main_48C2A1 + 0x19B, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.run_bahamut_neo_main_48C2A1 + 0x1D1, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.run_bahamut_neo_main_48C2A1 + 0x20E, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.run_bahamut_neo_main_48C2A1 + 0x243, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.run_bahamut_neo_main_48C2A1 + 0x28A, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.run_bahamut_neo_main_48C2A1 + 0x2C0, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.run_bahamut_neo_main_48C2A1 + 0x2FC, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.run_bahamut_neo_main_48C2A1 + 0x332, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.odin_gunge_effect_sub_4A3A2E + 0x38, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.odin_gunge_effect_sub_4A3A2E + 0x53, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.odin_gunge_effect_sub_4A4BE6 + 0x36, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.odin_gunge_effect_sub_4A4BE6 + 0x51, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.typhoon_effect_sub_4D7044 + 0x1B, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.typhoon_effect_sub_4D7044 + 0x36, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.typhoon_effect_sub_4DB15F + 0x22, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.typhoon_effect_sub_4DB15F + 0x3D, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.barret_limit_3_1_sub_4700F7 + 0x1B, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.barret_limit_3_1_sub_4700F7 + 0x36, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.fat_chocobo_sub_5096F3 + 0x4A, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.fat_chocobo_sub_5096F3 + 0x5F, (uint32_t)&wide_viewport_width);
    // Makes bahamut small stars background to 512x512 quad size and a different positioning of the 6 image patches
    patch_code_short(ff7_externals.bahamut_zero_draw_bg_effect_sub_4859AA + 0x20F, -512);
    patch_code_short(ff7_externals.bahamut_zero_draw_bg_effect_sub_4859AA + 0x23F, 1024);
    patch_code_short(ff7_externals.bahamut_zero_draw_bg_effect_sub_4859AA + 0x26F, 512);
    patch_code_short(ff7_externals.bahamut_zero_draw_bg_effect_sub_4859AA + 0x29E, 512);
    patch_code_short(ff7_externals.bahamut_zero_draw_bg_effect_sub_4859AA + 0x2CE, 512);
    patch_code_short(ff7_externals.bahamut_zero_bg_star_graphics_data_7F6748 + 0x8, 512);
    patch_code_short(ff7_externals.bahamut_zero_bg_star_graphics_data_7F6748 + 0xA, 512);

    // Battle fading animation fix
    patch_code_short(ff7_externals.battle_sub_5BCF9D + 0x3A, 30);
    patch_code_byte(ff7_externals.battle_sub_5BCF9D + 0x69, 120);
    patch_code_byte(ff7_externals.battle_sub_5BD050 + 0x46, 72);
    patch_code_byte(ff7_externals.battle_sub_5BD050 + 0xA5, 72);
    patch_code_byte(ff7_externals.battle_sub_5BD050 + 0x87, 48);
    patch_code_byte(ff7_externals.battle_sub_5BD050 + 0xDC, 48);
    patch_code_byte(ff7_externals.battle_sub_5BD050 + 0x100, 48);
    patch_code_byte(ff7_externals.battle_sub_5BD050 + 0x15C, 48);
    patch_code_byte(ff7_externals.battle_sub_5BD050 + 0x186, 48);

    // Worldmap fix
    patch_code_dword(ff7_externals.world_draw_fade_quad_75551A + 0x12, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.world_draw_fade_quad_75551A + 0x1A3, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.world_draw_fade_quad_75551A + 0x2BE, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.world_draw_fade_quad_75551A + 0x331, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.world_draw_fade_quad_75551A + 0x4E8, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.world_draw_fade_quad_75551A + 0x54A, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.world_culling_bg_meshes_75F263 + 0xE, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.world_culling_bg_meshes_75F263 + 0x20, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.world_culling_bg_meshes_75F263 + 0x26, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.world_submit_draw_bg_meshes_75F68C + 0xAE, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.world_submit_draw_bg_meshes_75F68C + 0xB6, (uint32_t)&wide_viewport_x);
    memset_code(ff7_externals.world_sub_751EFC + 0xC89, 0x90, 6);

    patch_code_short(ff7_externals.world_compute_skybox_data_754100 + 0x174, -wide_viewport_width / 4 - 20);
    patch_code_short(ff7_externals.world_compute_skybox_data_754100 + 0x1D1, wide_viewport_width / 4 + 20);
    patch_code_short(ff7_externals.world_compute_skybox_data_754100 + 0x22E, -wide_viewport_width / 4 - 20);
    patch_code_short(ff7_externals.world_compute_skybox_data_754100 + 0x282, wide_viewport_width / 4 + 20);
    patch_code_byte(ff7_externals.world_compute_skybox_data_754100 + 0x180, 44);
    patch_code_byte(ff7_externals.world_compute_skybox_data_754100 + 0x1DD, 44);
    patch_code_short(ff7_externals.world_compute_skybox_data_754100 + 0x237, 20);
    patch_code_short(ff7_externals.world_compute_skybox_data_754100 + 0x28B, 20);
    patch_code_short(ff7_externals.world_submit_draw_clouds_and_meteor_7547A6 + 0x132, -256);
    patch_code_int(ff7_externals.world_submit_draw_clouds_and_meteor_7547A6 + 0x144, 256);
    patch_code_byte(ff7_externals.world_submit_draw_clouds_and_meteor_7547A6 + 0x15C, 0);
    patch_code_short(ff7_externals.world_submit_draw_clouds_and_meteor_7547A6 + 0x16D, 256);
    patch_code_int(ff7_externals.world_submit_draw_clouds_and_meteor_7547A6 + 0x2DC, 0);
    patch_code_int(ff7_externals.world_submit_draw_clouds_and_meteor_7547A6 + 0x40F, 0);
    patch_code_dword(ff7_externals.world_submit_draw_clouds_and_meteor_7547A6 + 0x5A5, (uint32_t)&wide_viewport_x); // Meteor avoid culling
    patch_code_int(ff7_externals.world_submit_draw_clouds_and_meteor_7547A6 + 0x5B5, wide_viewport_width / 2); // Meteor avoid culling

    // Gameover fix
    patch_code_int(ff7_externals.enter_gameover + 0xA4, wide_viewport_x);
    patch_code_int(ff7_externals.enter_gameover + 0xB8, wide_viewport_width);

    // CDCheck fix
    patch_code_int(ff7_externals.cdcheck_enter_sub + 0xB9, wide_viewport_x);
    patch_code_int(ff7_externals.cdcheck_enter_sub + 0xCD, wide_viewport_width);

    // Credits fix
    patch_code_dword(ff7_externals.credits_submit_draw_fade_quad_7AA89B + 0x99, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.credits_submit_draw_fade_quad_7AA89B + 0xE6, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.credits_submit_draw_fade_quad_7AA89B + 0x133, (uint32_t)&viewport_width_plus_x_widescreen_fix);
    patch_code_dword(ff7_externals.credits_submit_draw_fade_quad_7AA89B + 0x180, (uint32_t)&viewport_width_plus_x_widescreen_fix);

    // Highway fix
    patch_code_int(ff7_externals.highway_submit_fade_quad_659532 + 0x5F, wide_viewport_width);
    patch_code_char(ff7_externals.highway_submit_fade_quad_659532 + 0x66, wide_viewport_x);

    // Chocobo fix
    patch_code_int(ff7_externals.chocobo_init_viewport_values_76D320 + 0x1F, wide_viewport_height);
    patch_code_char(ff7_externals.chocobo_init_viewport_values_76D320 + 0x29, wide_viewport_y);
    patch_code_int(ff7_externals.chocobo_init_viewport_values_76D320 + 0x62, wide_viewport_y);
    patch_code_dword((uint32_t)ff7_externals.chocobo_submit_draw_fade_quad_77B1CE + 0x99, (uint32_t)&wide_viewport_x);
    patch_code_int((uint32_t)ff7_externals.chocobo_fade_quad_data_97A498 + 0x28, wide_viewport_width / 2);
    replace_call_function(ff7_externals.chocobo_submit_draw_water_quad_77A7D0 + 0x9F, ff7_widescreen_fix_chocobo_submit_quad_graphics_object);

    // Snowboard fix
    patch_code_int(ff7_externals.snowboard_draw_sky_and_mountains_72DAF0 + 0xCC, ceil(wide_viewport_width / 4));
    patch_code_int(ff7_externals.snowboard_draw_sky_and_mountains_72DAF0 + 0x140, ceil(wide_viewport_width / 4));
    patch_code_float((uint32_t)ff7_externals.snowboard_sky_quad_pos_x_7B7DB8, wide_viewport_x);
    patch_code_int(ff7_externals.snowboard_submit_draw_sky_quad_graphics_object_72E31F + 0x173, wide_viewport_width + wide_viewport_x);
    patch_code_int(ff7_externals.snowboard_submit_draw_sky_quad_graphics_object_72E31F + 0x225, wide_viewport_width + wide_viewport_x);
    patch_code_int(ff7_externals.snowboard_submit_draw_black_quad_graphics_object_72DD94 + 0x27, wide_viewport_width);
    patch_code_char(ff7_externals.snowboard_submit_draw_black_quad_graphics_object_72DD94 + 0x2E, wide_viewport_x);
    patch_code_int(ff7_externals.snowboard_submit_draw_white_fade_quad_graphics_object_72DD53 + 0x27, wide_viewport_width);
    patch_code_char(ff7_externals.snowboard_submit_draw_white_fade_quad_graphics_object_72DD53 + 0x2E, wide_viewport_x);
    patch_code_int(ff7_externals.snowboard_submit_draw_opaque_quad_graphics_object_72DDD5 + 0x1F, wide_viewport_width);
    patch_code_char(ff7_externals.snowboard_submit_draw_opaque_quad_graphics_object_72DDD5 + 0x26, wide_viewport_x);

    // Menu, endbattle menu, ... fixes
    patch_code_dword(ff7_externals.menu_submit_draw_fade_quad_6CD64E + 0x50, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.menu_submit_draw_fade_quad_6CD64E + 0xA8, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.menu_submit_draw_fade_quad_6CD64E + 0x105, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.menu_submit_draw_fade_quad_6CD64E + 0x163, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.menu_submit_draw_fade_quad_6CD64E + 0x111, (uint32_t)&wide_viewport_width);
    patch_code_dword(ff7_externals.menu_submit_draw_fade_quad_6CD64E + 0x16F, (uint32_t)&wide_viewport_width);
}

void Widescreen::loadConfig()
{
    char _fullpath[MAX_PATH];
    sprintf(_fullpath, "%s/%s/config.toml", basedir, external_widescreen_path.c_str());

    try
    {
        config = toml::parse_file(_fullpath);
    }
    catch (const toml::parse_error &err)
    {
        config = toml::parse("");
    }
}

void Widescreen::loadMovieConfig()
{
    char _fullpath[MAX_PATH];
    sprintf(_fullpath, "%s/%s/movie_config.toml", basedir, external_widescreen_path.c_str());

    try
    {
        movie_config = toml::parse_file(_fullpath);
    }
    catch (const toml::parse_error &err)
    {
        movie_config = toml::parse("");
    }
}

void Widescreen::init()
{
    loadConfig();
    loadMovieConfig();
    if (aspect_ratio == AR_WIDESCREEN_16X10)
    {
        wide_viewport_x = -64;
        wide_viewport_width = 768;
        wide_game_width = 768;
        viewport_width_plus_x_widescreen_fix = 704;
        swirl_framebuffer_offset_x_widescreen_fix = 64;
    }
}

void Widescreen::initParamsFromConfig()
{
    field_trigger_header* field_triggers_header_ptr = *ff7_externals.field_triggers_header;
    camera_range.left = field_triggers_header_ptr->camera_range.left;
    camera_range.right = field_triggers_header_ptr->camera_range.right;
    camera_range.bottom = field_triggers_header_ptr->camera_range.bottom;
    camera_range.top = field_triggers_header_ptr->camera_range.top;
    if(camera_range.right - camera_range.left >= game_width / 2 + abs(wide_viewport_x))
        widescreen_mode = WM_EXTEND_WIDE;
    else
        widescreen_mode = WM_DISABLED;
    h_offset = 0;
    v_offset = 0;
    is_reset_vertical_pos = false;
    is_scripted_clip_enabled = true;
    is_scripted_vertical_clip_enabled = false;
    movie_v_offset.clear();

    auto pName = get_current_field_name();
    if(pName == 0) return;

    std::string _name(pName);
    auto node = config[_name];
    if(node)
    {
        if(auto leftNode = node["left"]) camera_range.left = leftNode.value_or(0);
        if(auto rightNode = node["right"]) camera_range.right = rightNode.value_or(0);
        if(auto bottomNode = node["bottom"])camera_range.bottom = bottomNode.value_or(0);
        if(auto topNode = node["top"]) camera_range.top = topNode.value_or(0);
        if(auto hOffsetNode = node["h_offset"]) h_offset = hOffsetNode.value_or(0);
        if(auto vOffsetNode = node["v_offset"]) v_offset = vOffsetNode.value_or(0);
        if(auto vResetVerticalPosNode = node["reset_vertical_pos"]) is_reset_vertical_pos = vResetVerticalPosNode.value_or(false);
        if(auto vScriptedClipNode = node["scripted_clip"]) is_scripted_clip_enabled = vScriptedClipNode.value_or(true);
        if(auto vScriptedVerticalClipNode = node["scripted_vertical_clip"]) is_scripted_vertical_clip_enabled = vScriptedVerticalClipNode.value_or(false);

        if(auto modeNode = node["mode"]) widescreen_mode = static_cast<WIDESCREEN_MODE>(modeNode.value_or(0));

        if(widescreen_mode == WM_ZOOM)
        {
            int verticalRangeOffset = 9 * (camera_range.right - camera_range.left) / 16 - 240;
            camera_range.bottom -= verticalRangeOffset / 2;
            camera_range.top += verticalRangeOffset / 2;
        }
    }
}

void Widescreen::initMovieParamsFromConfig(char *name)
{
    widescreen_movie_mode = WM_DISABLED;
    movie_v_offset.clear();

    if(name == 0) return;

    std::string _name(name);
    auto node = movie_config[_name];
    if(node)
    {
        if(auto modeNode = node["mode"]) widescreen_movie_mode = static_cast<WIDESCREEN_MODE>(modeNode.value_or(0));

        if(auto movie_v_offset_node = node["movie_v_offset"])
        {
            auto array = movie_v_offset_node.as_array();
            auto size = array->size();
            movie_v_offset.resize(size);
            for (int i = 0; i < size; ++i)
            {
                auto keyframeArray = array->get(i)->as_array();
                movie_v_offset[i].frame = keyframeArray->get(0)->value<int>().value_or(0);
                movie_v_offset[i].v_offset = keyframeArray->get(1)->value<int>().value_or(0);
            }
        }
    }
}

KeyPair Widescreen::getMovieKeyPair(int frame)
{
    KeyPair ret;

    int keyCount = movie_v_offset.size();
    for(int keyIndex = 0; keyIndex < keyCount; ++keyIndex)
    {
        auto key = movie_v_offset[keyIndex];
        if(keyIndex == keyCount-1)
        {
            ret.first.frame = key.frame * movie_fps_ratio;
            ret.first.v_offset = key.v_offset;
            ret.second.frame = key.frame * movie_fps_ratio;
            ret.second.v_offset = key.v_offset;
            break;
        }

        auto nextKey = movie_v_offset[keyIndex + 1];
        auto keyFrame = key.frame * movie_fps_ratio;
        auto nextkeyFrame = nextKey.frame * movie_fps_ratio;
        if(frame >= keyFrame && frame < nextkeyFrame)
        {
            auto nextKey = movie_v_offset[keyIndex + 1];
            ret.first.frame = keyFrame;
            ret.first.v_offset = key.v_offset;
            ret.second.frame = nextkeyFrame;
            ret.second.v_offset = nextKey.v_offset;
            break;
        }
    }

    return ret;
}
