/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

#include "patch.h"

// FF7 game mode definitions
static struct game_mode ff7_modes[] = {
	{FF7_MODE_FIELD,       "MODE_FIELD",       MODE_FIELD,       false},
	{FF7_MODE_BATTLE,      "MODE_BATTLE",      MODE_BATTLE,      false},
	{FF7_MODE_WORLDMAP,    "MODE_WORLDMAP",    MODE_WORLDMAP,    false},
	{FF7_MODE_UNKNOWN4,    "MODE_UNKNOWN4",    MODE_UNKNOWN,     true },
	{FF7_MODE_MENU,        "MODE_MENU",        MODE_MENU,        false},
	{FF7_MODE_HIGHWAY,     "MODE_HIGHWAY",     MODE_HIGHWAY,     false},
	{FF7_MODE_CHOCOBO,     "MODE_CHOCOBO",     MODE_CHOCOBO,     false},
	{FF7_MODE_SNOWBOARD,   "MODE_SNOWBOARD",   MODE_SNOWBOARD,   false},
	{FF7_MODE_CONDOR,      "MODE_CONDOR",      MODE_CONDOR,      false},
	{FF7_MODE_SUBMARINE,   "MODE_SUBMARINE",   MODE_SUBMARINE,   false},
	{FF7_MODE_COASTER,     "MODE_COASTER",     MODE_COASTER,     false},
	{FF7_MODE_CDCHECK,     "MODE_CDCHECK",     MODE_CDCHECK,     false},
	{FF7_MODE_UNKNOWN13,   "MODE_UNKNOWN13",   MODE_UNKNOWN,     true },
	{FF7_MODE_SNOWBOARD2,  "MODE_SNOWBOARD2",  MODE_SNOWBOARD,   false},
	{FF7_MODE_UNKNOWN15,   "MODE_UNKNOWN15",   MODE_UNKNOWN,     true },
	{FF7_MODE_UNKNOWN16,   "MODE_UNKNOWN16",   MODE_UNKNOWN,     true },
	{FF7_MODE_BATTLE_MENU, "MODE_BATTLE_MENU", MODE_MENU,        false},
	{FF7_MODE_UNKNOWN18,   "MODE_UNKNOWN18",   MODE_UNKNOWN,     true },
	{FF7_MODE_EXIT,        "MODE_EXIT",        MODE_EXIT,        false},
	{FF7_MODE_MAIN_MENU,   "MODE_MAIN_MENU",   MODE_MENU,        false},
	{FF7_MODE_UNKNOWN21,   "MODE_UNKNOWN21",   MODE_UNKNOWN,     true },
	{FF7_MODE_UNKNOWN22,   "MODE_UNKNOWN22",   MODE_UNKNOWN,     true },
	{FF7_MODE_SWIRL,       "MODE_SWIRL",       MODE_SWIRL,       false},
	{FF7_MODE_UNKNOWN24,   "MODE_UNKNOWN24",   MODE_UNKNOWN,     true },
	{FF7_MODE_UNKNOWN25,   "MODE_UNKNOWN25",   MODE_UNKNOWN,     true },
	{FF7_MODE_GAMEOVER,    "MODE_GAMEOVER",    MODE_GAMEOVER,    false},
	{FF7_MODE_CREDITS,     "MODE_CREDITS",     MODE_CREDITS,     false},
	{FF7_MODE_UNKNOWN28,   "MODE_UNKNOWN28",   MODE_UNKNOWN,     true },
};

void ff7_set_main_loop(uint32_t driver_mode, uint32_t main_loop)
{
	uint32_t i;

	for(i = 0; i < num_modes; i++) if(ff7_modes[i].driver_mode == driver_mode) ff7_modes[i].main_loop = main_loop;
}

void ff7_find_externals()
{
	uint32_t main_loop = ff7_externals.cdcheck + 0xF3;
	uint32_t field_main_loop;
	uint32_t battle_main_loop;
	uint32_t menu_main_loop;
	uint32_t worldmap_main_loop;
	uint32_t cdcheck_main_loop;
	uint32_t coaster_main_loop;
	uint32_t swirl_main_loop;
	uint32_t movie_module;
	uint32_t file_module;

	if(*((uint32_t *)main_loop) != 0x81EC8B55) unexpected("odd main loop prologue\n");

	common_externals.update_movie_sample = get_relative_call(main_loop, 0x67);

	movie_module = common_externals.update_movie_sample - 0x3039;

	ff7_externals.movie_sub_415231 = (void (*)(char*))(movie_module + 0x331);
	common_externals.prepare_movie = movie_module + 0x1A95;
	common_externals.release_movie_objects = movie_module + 0x2859;
	common_externals.start_movie = movie_module + 0x2BB0;
	common_externals.stop_movie = movie_module + 0x2CB2;
	common_externals.get_movie_frame = movie_module + 0x3713;

	ff7_externals.movie_object = (struct movie_obj *)(get_absolute_value(common_externals.prepare_movie, 0x42) - 0xC);

	common_externals._mode = (WORD*)get_absolute_value(main_loop, 0x8C);

	ff7_set_main_loop(MODE_GAMEOVER, get_absolute_value(main_loop, 0x1FE));
	swirl_main_loop = get_absolute_value(main_loop, 0x25B);
	ff7_set_main_loop(MODE_SWIRL, swirl_main_loop);
	cdcheck_main_loop = get_absolute_value(main_loop, 0x397);
	ff7_set_main_loop(MODE_CDCHECK, cdcheck_main_loop);
	ff7_set_main_loop(MODE_CREDITS, get_absolute_value(main_loop, 0x4CA));
	menu_main_loop = get_absolute_value(main_loop, 0x62E);
	ff7_set_main_loop(MODE_MENU, menu_main_loop);
	battle_main_loop = get_absolute_value(main_loop, 0x89A);
	ff7_set_main_loop(MODE_BATTLE, battle_main_loop);
	field_main_loop = get_absolute_value(main_loop, 0x8F8);
	ff7_set_main_loop(MODE_FIELD, field_main_loop);
	worldmap_main_loop = get_absolute_value(main_loop, 0x977);
	ff7_set_main_loop(MODE_WORLDMAP, worldmap_main_loop);
	ff7_set_main_loop(MODE_CHOCOBO, get_absolute_value(main_loop, 0x9C5));
	ff7_set_main_loop(MODE_CONDOR, get_absolute_value(main_loop, 0xA13));
	ff7_set_main_loop(MODE_HIGHWAY, get_absolute_value(main_loop, 0xA61));
	coaster_main_loop = get_absolute_value(main_loop, 0xAAF);
	ff7_set_main_loop(MODE_COASTER, coaster_main_loop);
	ff7_set_main_loop(MODE_SUBMARINE, get_absolute_value(main_loop, 0xAFD));
	ff7_set_main_loop(MODE_SNOWBOARD, get_absolute_value(main_loop, 0xB3E));

	ff7_externals.sub_5F4A47 = get_absolute_value(main_loop, 0xA13);
	ff7_externals.reset_game_obj_sub_5F4971 = (void (*)(struct game_obj*))get_relative_call(ff7_externals.sub_5F4A47, 0x5B);
	ff7_externals.engine_exit_game_mode_sub_666C78 = get_relative_call(common_externals.winmain, 0x217);
	ff7_externals.sub_666C13 = (void* (*)(struct game_obj*))get_relative_call(ff7_externals.engine_exit_game_mode_sub_666C78, 0x35);
	ff7_externals.sub_670F9B = (void* (*)(void*))get_relative_call(ff7_externals.engine_exit_game_mode_sub_666C78, 0x47);
	ff7_externals.byte_CC0D89 = (BYTE*)get_absolute_value(main_loop, 0x71E);

	ff7_externals.destroy_field_bk = get_relative_call(field_main_loop, 0x222);
	ff7_externals.destroy_field_tiles = get_relative_call(ff7_externals.destroy_field_bk, 0x1E6);
	ff7_externals.field_layers = (field_layer **)get_absolute_value(ff7_externals.destroy_field_tiles, 0x46);

	ff7_externals.num_field_entities = (WORD *)(((uint32_t)ff7_externals.field_layers) - 0xC);
	ff7_externals.field_objects = (field_object **)(((uint32_t)ff7_externals.field_layers) - 0x10);

	ff7_externals.open_field_file = get_relative_call(field_main_loop, 0x331);
	ff7_externals.field_file_name = (char *)get_absolute_value(ff7_externals.open_field_file, 0x77);
	ff7_externals.read_field_file = get_relative_call(ff7_externals.open_field_file, 0xCF);

	ff7_externals.battle_loop = get_relative_call(battle_main_loop, 0x1C8);
	ff7_externals.battle_sub_429AC0 = get_absolute_value(ff7_externals.battle_loop, 0x79);
	ff7_externals.battle_b3ddata_sub_428B12 = get_relative_call(ff7_externals.battle_sub_429AC0, 0x71);
	ff7_externals.graphics_render_sub_68A638 = get_relative_call(ff7_externals.battle_b3ddata_sub_428B12, 0x10A);
	ff7_externals.create_dx_sfx_something = get_relative_call(ff7_externals.graphics_render_sub_68A638, 0xD3);
	ff7_externals.load_p_file = get_relative_call(ff7_externals.create_dx_sfx_something, 0x144);

	ff7_externals.create_polygon_data = (polygon_data* (*)(uint32_t, uint32_t))get_relative_call(ff7_externals.load_p_file, 0x17);
	ff7_externals.create_polygon_lists = (void (*)(polygon_data*))get_relative_call(ff7_externals.load_p_file, 0x35B);
	ff7_externals.free_polygon_data = (void (*)(polygon_data*))get_relative_call(ff7_externals.load_p_file, 0x3C4);

	common_externals.open_file = get_relative_call(ff7_externals.load_p_file, 0x3A);

	file_module = common_externals.open_file - 0xE2;
	common_externals.close_file = file_module + 0xA1;
	common_externals.read_file = file_module + 0x611;
	common_externals.__read_file = file_module + 0x6A7;
	common_externals.write_file = file_module + 0x735;
	common_externals.alloc_read_file = (void* (*)(uint32_t, uint32_t, struct file*))(file_module + 0x830);
	common_externals.get_filesize = file_module + 0x84B;
	common_externals.tell_file = file_module + 0x8A1;
	common_externals.seek_file = file_module + 0x90A;
	common_externals.alloc_get_file = (void* (*)(file_context*, uint32_t*, char*))(file_module + 0xA0E);

	common_externals.destroy_tex = (void (*)(tex_header*))get_relative_call(common_externals.load_tex_file, 0x16D);
	common_externals.destroy_tex_header = get_relative_call((uint32_t)common_externals.destroy_tex, 0x78);

	ff7_externals.battle_sub_42A0E7 = get_relative_call(ff7_externals.battle_sub_429AC0, 0xA4);
	ff7_externals.load_battle_stage = get_relative_call(ff7_externals.battle_sub_42A0E7, 0x78);
	ff7_externals.load_battle_stage_pc = get_relative_call(ff7_externals.load_battle_stage, 0x151);
	ff7_externals.read_battle_hrc = get_relative_call(ff7_externals.load_battle_stage_pc, 0x25);

	ff7_externals.battle_regular_chdir = (void (*)(battle_chdir_struc*))get_relative_call(ff7_externals.read_battle_hrc, 0x16);
	ff7_externals.battle_context_chdir = (void (*)(file_context*, battle_chdir_struc*))get_relative_call(ff7_externals.read_battle_hrc, 0x2B);
	ff7_externals.swap_extension = (void (*)(char*, char*, char*))get_relative_call(ff7_externals.read_battle_hrc, 0x43);
	ff7_externals.destroy_battle_hrc = (void (*)(uint32_t, battle_hrc_header*))get_relative_call(ff7_externals.read_battle_hrc, 0xB3);
	ff7_externals.battle_regular_olddir = (void (*)(battle_chdir_struc*))get_relative_call(ff7_externals.read_battle_hrc, 0xD2);
	ff7_externals.battle_context_olddir = (void (*)(file_context*, battle_chdir_struc*))get_relative_call(ff7_externals.read_battle_hrc, 0xE7);

	ff7_externals.field_sub_60DCED = get_relative_call(field_main_loop, 0x37A);
	ff7_externals.field_load_models = get_relative_call(ff7_externals.field_sub_60DCED, 0x168);
	ff7_externals.field_load_animation = get_relative_call(ff7_externals.field_load_models, 0x8DF);
	ff7_externals.load_animation = get_relative_call(ff7_externals.field_load_animation, 0x16D);
	ff7_externals.destroy_animation = (void (*)(anim_header*))get_relative_call(ff7_externals.load_animation, 0x162);

	ff7_externals.load_lgp = get_relative_call(main_loop, 0x450);
	ff7_externals.open_lgp_file = get_relative_call(ff7_externals.load_lgp, 0x1C);
	ff7_externals.__read = get_relative_call(common_externals.read_file, 0x4A);

	ff7_externals.lgp_open_file = get_relative_call((uint32_t)common_externals.open_file, 0x234);
	ff7_externals.lgp_seek_file = get_relative_call((uint32_t)common_externals.open_file, 0x265);
	ff7_externals.lgp_read = get_relative_call((uint32_t)common_externals.read_file, 0x2E);
	ff7_externals.lgp_get_filesize = get_relative_call((uint32_t)ff7_externals.read_field_file, 0x71);
	ff7_externals.lgp_read_file = get_relative_call((uint32_t)ff7_externals.read_field_file, 0xDD);

	ff7_externals.lgp_fds = (FILE **)get_absolute_value(ff7_externals.lgp_seek_file, 0x17);

	ff7_externals.context_chdir = get_relative_call((uint32_t)ff7_externals.battle_context_chdir, 0x3C);
	ff7_externals.lgp_chdir = get_relative_call(ff7_externals.context_chdir, 0x2A);

	ff7_externals.lgp_lookup_tables = (lookup_table_entry **)get_absolute_value(ff7_externals.lgp_open_file, 0x194);
	ff7_externals.lgp_tocs = (lgp_toc_entry **)get_absolute_value(ff7_externals.lgp_open_file, 0x233);
	ff7_externals.lgp_folders = (lgp_folders *)get_absolute_value(ff7_externals.lgp_open_file, 0x42C);

	ff7_externals.battle_sub_437DB0 = get_absolute_value(ff7_externals.battle_loop, 0x8D);
	ff7_externals.sub_5CB2CC = get_relative_call(ff7_externals.battle_sub_437DB0, 0x43);

	ff7_externals.midi_volume_control = (uint32_t *)get_absolute_value(common_externals.midi_init, 0x706);
	ff7_externals.midi_initialized = (uint32_t *)get_absolute_value(common_externals.midi_init, 0x3A);

	ff7_externals.menu_sub_6CDA83 = get_relative_call(menu_main_loop, 0x112);
	ff7_externals.menu_sub_6CBD43 = get_relative_call(ff7_externals.menu_sub_6CDA83, 0xAF);
	ff7_externals.menu_sub_701EE4 = get_relative_call(ff7_externals.menu_sub_6CBD43, 0x7);
	ff7_externals.phs_menu_sub = get_relative_call(ff7_externals.menu_sub_701EE4, 0xE3);

	if(version == VERSION_FF7_102_US) ff7_externals.menu_draw_party_member_stats = get_relative_call(ff7_externals.phs_menu_sub, 0x8FF);
	else ff7_externals.menu_draw_party_member_stats = get_relative_call(ff7_externals.phs_menu_sub, 0x8F5);

	ff7_externals.party_member_to_char_map = (uint32_t *)get_absolute_value(ff7_externals.menu_draw_party_member_stats, 0x14);

	ff7_externals.menu_start = get_absolute_value(main_loop, 0x627);
	ff7_externals.menu_sub_6CB56A = get_relative_call(ff7_externals.menu_sub_6CDA83, 0xDE);
	ff7_externals.menu_subs_call_table = (uint32_t *)get_absolute_value(ff7_externals.menu_sub_6CB56A, 0x2EC);
	ff7_externals.status_menu_sub = ff7_externals.menu_subs_call_table[5];
	ff7_externals.menu_sound_slider_loop = ff7_externals.menu_subs_call_table[8];
	ff7_externals.draw_status_limit_level_stats = get_relative_call(ff7_externals.status_menu_sub, 0x8E);

	ff7_externals.get_kernel_text = (char* (*)(uint32_t, uint32_t, uint32_t))get_relative_call(ff7_externals.draw_status_limit_level_stats, 0x10C);

	ff7_externals.sub_5CF282 = get_relative_call(ff7_externals.sub_5CB2CC, 0x4E);
	ff7_externals.get_equipment_stats = get_relative_call(ff7_externals.sub_5CF282, 0x2F0);

	ff7_externals.weapon_data_array = (weapon_data *)(get_absolute_value(ff7_externals.get_equipment_stats, 0x50) - 4);
	ff7_externals.armor_data_array = (armor_data *)(get_absolute_value(ff7_externals.get_equipment_stats, 0x78) - 2);

	ff7_externals.field_sub_6388EE = get_relative_call(field_main_loop, 0xFF);
	ff7_externals.field_draw_everything = get_relative_call(ff7_externals.field_sub_6388EE, 0x11);
	ff7_externals.field_pick_tiles_make_vertices = get_relative_call(ff7_externals.field_draw_everything, 0xC9);
	ff7_externals.field_layer2_pick_tiles = get_relative_call(ff7_externals.field_pick_tiles_make_vertices, 0x48);
	ff7_externals.field_special_y_offset = (uint32_t *)get_absolute_value(ff7_externals.field_layer2_pick_tiles, 0x43);
	ff7_externals.field_layer2_tiles_num = (uint32_t *)get_absolute_value(ff7_externals.field_layer2_pick_tiles, 0x8C);
	ff7_externals.field_layer2_palette_sort = (uint32_t **)get_absolute_value(ff7_externals.field_layer2_pick_tiles, 0xA3);
	ff7_externals.field_layer2_tiles = (field_tile **)get_absolute_value(ff7_externals.field_layer2_pick_tiles, 0xC0);
	ff7_externals.field_anim_state = (char *)get_absolute_value(ff7_externals.field_layer2_pick_tiles, 0x1A4);
	ff7_externals.add_page_tile = (void (*)(float, float, float, float, float, uint32_t, uint32_t))get_relative_call(ff7_externals.field_layer2_pick_tiles, 0x327);

	ff7_externals.field_load_textures = get_relative_call(ff7_externals.field_sub_60DCED, 0x107);
	ff7_externals.field_convert_type2_layers = (void (*)())get_relative_call(ff7_externals.field_load_textures, 0xD);
	ff7_externals.make_struc3 = (void (*)(uint32_t, struc_3*))get_relative_call(ff7_externals.field_load_textures, 0xAC);
	ff7_externals.make_field_tex_header_pal = (void (*)(ff7_tex_header*))get_relative_call(ff7_externals.field_load_textures, 0x21F);
	ff7_externals.make_field_tex_header = (void (*)(ff7_tex_header*))get_relative_call(ff7_externals.field_load_textures, 0x23C);
	ff7_externals._load_texture = (ff7_graphics_object* (*)(uint32_t, uint32_t, struc_3*, char*, void*))get_relative_call(ff7_externals.field_load_textures, 0x2F8);

	ff7_externals.read_field_background_data = get_relative_call(ff7_externals.field_sub_60DCED, 0x8B);
	ff7_externals.layer2_end_page = (WORD *)get_absolute_value(ff7_externals.read_field_background_data, 0x788);

	ff7_externals.create_d3d2_indexed_primitive = get_relative_call((uint32_t)common_externals.generic_load_group, 0x22);
	ff7_externals.destroy_d3d2_indexed_primitive = get_relative_call(ff7_externals.create_d3d2_indexed_primitive, 0x290);

	ff7_externals.enter_main = get_absolute_value(worldmap_main_loop, 0x2AE);

	ff7_externals.kernel_init = get_relative_call(ff7_externals.enter_main, 0xF1);
	ff7_externals.kernel_load_kernel2 = get_relative_call(ff7_externals.kernel_init, 0x1FD);
	ff7_externals.kernel2_reset_counters = get_relative_call(ff7_externals.kernel_load_kernel2, 0x33);

	ff7_externals.sub_4012DA = get_absolute_value(ff7_externals.kernel_init, 0x136);
	ff7_externals.kernel2_add_section = get_relative_call(ff7_externals.sub_4012DA, 0x4D);
	ff7_externals.kernel2_get_text = get_relative_call((uint32_t)ff7_externals.get_kernel_text, 0xF7);

	ff7_externals.draw_3d_model = get_relative_call(ff7_externals.field_draw_everything, 0x17F);
	ff7_externals.stack_push = (void (*)(struct stack*))get_relative_call(ff7_externals.draw_3d_model, 0x8E);
	ff7_externals.stack_top = (void* (*)(struct stack*))get_relative_call(ff7_externals.draw_3d_model, 0x9A);
	ff7_externals.stack_pop = (void (*)(struct stack*))get_relative_call(ff7_externals.draw_3d_model, 0x8FD);
	ff7_externals._root_animation = (void (*)(matrix*, anim_frame*, anim_header*, hrc_data*))get_absolute_value(ff7_externals.draw_3d_model, 0xD4);
	ff7_externals._frame_animation = (void (*)(uint32_t, matrix*, point3d*, anim_frame*, anim_header*, hrc_bone*, hrc_data*))get_absolute_value(ff7_externals.draw_3d_model, 0xDB);
	ff7_externals.root_animation = (void (*)(matrix*, anim_frame*, anim_header*, hrc_data*))get_absolute_value(ff7_externals.draw_3d_model, 0xE7);
	ff7_externals.frame_animation = (void (*)(uint32_t, matrix*, point3d*, anim_frame*, anim_header*, hrc_bone*, hrc_data*))get_absolute_value(ff7_externals.draw_3d_model, 0xEE);
	ff7_externals.model_mode = (uint32_t *)get_absolute_value(ff7_externals.draw_3d_model, 0x2A7);

	ff7_externals.name_menu_sub_6CBD32 = get_relative_call(ff7_externals.menu_sub_6CDA83, 0x9A);
	ff7_externals.name_menu_sub_719C08 = get_relative_call(ff7_externals.name_menu_sub_6CBD32, 0x7);

	if(version == VERSION_FF7_102_FR)
	{
		ff7_externals.menu_sub_71894B = get_relative_call(ff7_externals.name_menu_sub_719C08, 0x2B);
		ff7_externals.menu_sub_718DBE = get_relative_call(ff7_externals.name_menu_sub_719C08, 0x77);
		ff7_externals.menu_sub_719B81 = get_relative_call(ff7_externals.name_menu_sub_719C08, 0xCC);
		
		ff7_externals.set_default_input_settings_save = get_relative_call(ff7_externals.menu_sub_71894B, 0x189);
	}
	else
	{
		ff7_externals.menu_sub_71894B = get_relative_call(ff7_externals.name_menu_sub_719C08, 0x2A);
		ff7_externals.menu_sub_718DBE = get_relative_call(ff7_externals.name_menu_sub_719C08, 0x76);
		ff7_externals.menu_sub_719B81 = get_relative_call(ff7_externals.name_menu_sub_719C08, 0xCB);
		
		ff7_externals.set_default_input_settings_save = get_relative_call(ff7_externals.menu_sub_71894B, 0x188);
	}

	ff7_externals.keyboard_name_input = get_relative_call(ff7_externals.menu_sub_718DBE, 0x99);
 	ff7_externals.restore_input_settings = get_relative_call(ff7_externals.menu_sub_719B81, 0x80);
 	
	ff7_externals.dinput_getdata2 = get_relative_call(ff7_externals.keyboard_name_input, 0x1C);
	common_externals.get_keyboard_state = get_relative_call(ff7_externals.keyboard_name_input, 0x6);

	ff7_externals.init_game = get_absolute_value(ff7_externals.init_stuff, 0x336);
	ff7_externals.sub_41A1B0 = get_relative_call(ff7_externals.init_game, 0x85);
	ff7_externals.init_directinput = get_relative_call(ff7_externals.sub_41A1B0, 0x34);
	ff7_externals.dinput_createdevice_mouse = get_relative_call(ff7_externals.init_directinput, 0x48);

	common_externals.dinput_acquire_keyboard = (int (*)())get_relative_call(common_externals.get_keyboard_state, 0x4F);
	common_externals.keyboard_device = (IDirectInputDeviceA**)get_absolute_value(common_externals.get_keyboard_state, 0x06);
	common_externals.keyboard_connected = (uint32_t*)get_absolute_value(common_externals.get_keyboard_state, 0x47);

	ff7_externals.sub_69C69F = (void (*)(matrix*, ff7_light*))get_relative_call(ff7_externals.draw_3d_model, 0x882);

	ff7_externals.coaster_sub_5E9051 = get_relative_call(coaster_main_loop, 0xC6);
	ff7_externals.coaster_sub_5EE150 = get_relative_call(ff7_externals.coaster_sub_5E9051, 0x3);

	ff7_externals.cleanup_game = get_absolute_value(ff7_externals.init_stuff, 0x350);
	ff7_externals.cleanup_midi = get_relative_call(ff7_externals.cleanup_game, 0x72);

	ff7_externals.sub_60DF96 = get_relative_call(ff7_externals.init_game, 0x42B);
	ff7_externals.sub_60EEB2 = get_relative_call(ff7_externals.sub_60DF96, 0x26);
	ff7_externals.open_flevel_siz = get_relative_call(ff7_externals.sub_60EEB2, 0x79F);
	ff7_externals.field_map_infos = get_absolute_value(ff7_externals.open_flevel_siz, 0xAF) - 0xBC;

	ff7_externals.sound_operation = get_relative_call(ff7_externals.enter_main, 0xE4);
	common_externals.play_sfx_on_channel = get_relative_call(ff7_externals.sound_operation, 0x2AB);
	common_externals.set_sfx_volume_on_channel = (uint32_t(*)(uint32_t, uint32_t))get_relative_call(ff7_externals.sound_operation, 0x3B3);
	common_externals.dsound_volume_table = (uint32_t*)get_absolute_value(uint32_t(common_externals.set_sfx_volume_on_channel), 0xCC);
	common_externals.play_sfx = (uint32_t(*)(uint32_t))get_relative_call(ff7_externals.sound_operation, 0x703);
	ff7_externals.sfx_fill_buffer_from_audio_dat = get_relative_call(uint32_t(common_externals.play_sfx), 0x4D);
	ff7_externals.sound_states = (ff7_field_sfx_state*)get_absolute_value(common_externals.play_sfx_on_channel, 0x28);
	common_externals.master_sfx_volume = (uint32_t*)get_absolute_value(common_externals.play_sfx_on_channel, 0x342);

	ff7_externals.battle_clear_sound_flags = get_relative_call(ff7_externals.battle_sub_429AC0, 0x6C);
	ff7_externals.swirl_sound_effect = get_relative_call(swirl_main_loop, 0x8B);

	ff7_externals.field_initialize_variables = get_relative_call(ff7_externals.field_sub_60DCED, 0x178);
	ff7_externals.music_lock_clear_fix = ff7_externals.field_initialize_variables + 0x2B8;
	ff7_externals.field_init_event_sub_63BCA7 = get_relative_call(ff7_externals.field_initialize_variables, 0x29D);
	ff7_externals.field_init_event = get_relative_call(ff7_externals.field_init_event_sub_63BCA7, 0x8);
	ff7_externals.execute_opcode = get_relative_call(ff7_externals.field_init_event, 0x80);

	common_externals.execute_opcode_table = (uint32_t*)get_absolute_value(ff7_externals.execute_opcode, 0x10D);
	ff7_externals.opcode_akao2 = common_externals.execute_opcode_table[0xDA];
	ff7_externals.opcode_akao = common_externals.execute_opcode_table[0xF2];
	ff7_externals.opcode_gameover = common_externals.execute_opcode_table[0xFF];

	ff7_externals.enter_gameover = get_absolute_value(main_loop, 0x1F7);
	ff7_externals.exit_gameover = get_absolute_value(main_loop, 0x213);
	ff7_externals.start_gameover = (void* (*)())get_relative_call(ff7_externals.enter_gameover, 0xC6);
	ff7_externals.gameover_sub_6C12B1 = (void* (*)())get_relative_call(ff7_externals.exit_gameover, 0x21);
	ff7_externals.on_gameover_enter = ff7_externals.enter_gameover + 0xC6;
	ff7_externals.on_gameover_exit = ff7_externals.exit_gameover + 0x21;
}

void ff7_data()
{
	num_modes = sizeof(ff7_modes) / sizeof(ff7_modes[0]);

	ff7_find_externals();

	memcpy(modes, ff7_modes, sizeof(ff7_modes));

	text_colors[TEXTCOLOR_GRAY] = 0x08;
	text_colors[TEXTCOLOR_BLUE] = 0x01;
	text_colors[TEXTCOLOR_RED] = 0x04;
	text_colors[TEXTCOLOR_PINK] = 0x05;
	text_colors[TEXTCOLOR_GREEN] = 0x02;
	text_colors[TEXTCOLOR_LIGHT_BLUE] = 0x09;
	text_colors[TEXTCOLOR_YELLOW] = 0x0E;
	text_colors[TEXTCOLOR_WHITE] = 0x0F;
}
