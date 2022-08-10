/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Tang-Tang Zhou                                     //
//    Copyright (C) 2022 Cosmos                                             //
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

#include "widescreen.h"
#include "../patch.h"
#include "../ff7.h"
#include "cmath"

int viewport_width_plus_x_widescreen_fix = 750;
int swirl_framebuffer_offset_x_widescreen_fix = 106;
int swirl_framebuffer_offset_y_widescreen_fix = 64;

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
    replace_function((uint32_t)ff7_externals.field_clip_with_camera_range_6438F6, ff7_field_clip_with_camera_range);
    replace_function(ff7_externals.field_layer3_clip_with_camera_range_643628, ff7_field_layer3_clip_with_camera_range);
    replace_function(ff7_externals.field_culling_model_639252, ff7_field_do_draw_3d_model);
    replace_call_function(ff7_externals.field_sub_63AC66 + 0xD5, ff7_field_set_fade_quad_size);
    patch_code_dword(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x6A, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x80, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x94, (uint32_t)&wide_viewport_x);
    patch_code_dword(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x9C, (uint32_t)&wide_viewport_x);
    patch_code_int(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x64, wide_viewport_width / 2);
    patch_code_int(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x7A, wide_viewport_width / 2);
    memset_code(ff7_externals.field_submit_draw_pointer_hand_60D572 + 0x39, 0x90, 12); // Remove useless culling cursor
    patch_code_int(ff7_externals.field_init_viewport_values + 0xBE, wide_viewport_width + wide_viewport_x - 60);
    patch_code_int(ff7_externals.field_init_viewport_values + 0xC8, 18);

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