/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2020 Marcin Gomulak                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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

#include "ff8_data.h"

#include "patch.h"

void ff8_set_main_loop(uint32_t driver_mode, uint32_t main_loop)
{
	uint32_t i;

	for(i = 0; i < num_modes; i++) if(ff8_modes[i].driver_mode == driver_mode) ff8_modes[i].main_loop = main_loop;
}

void ff8_find_externals()
{
	common_externals.winmain = get_relative_call(ff8_externals.start, 0xDB);
	common_externals.create_window = get_relative_call(common_externals.winmain, 0x114);
	common_externals.engine_wndproc = (WNDPROC)get_absolute_value(common_externals.create_window, 0x34);

	ff8_externals.main_entry = get_relative_call(common_externals.winmain, 0x4D);

	if (JP_VERSION)
	{
		ff8_externals.main_entry = get_relative_call(ff8_externals.main_entry, 0x0);
	}

	ff8_externals.archive_path_prefix = (char *)get_absolute_value(ff8_externals.main_entry, 0x2E);
	common_externals.diff_time = (uint64_t (*)(uint64_t*,uint64_t*,uint64_t*))get_relative_call(common_externals.winmain, 0x41E);
	ff8_externals.init_config = get_relative_call(ff8_externals.main_entry, 0x73);
	ff8_externals.pubintro_init = get_absolute_value(ff8_externals.main_entry, 0x158);

	if (JP_VERSION)
	{
		ff8_externals.init_config = get_relative_call(ff8_externals.init_config, 0x0);
		ff8_externals.pubintro_init = get_relative_call(ff8_externals.pubintro_init, 0x0);
	}

	ff8_externals.sub_467C00 = get_relative_call(ff8_externals.pubintro_init, 0xB5);
	ff8_externals.input_init = get_relative_call(ff8_externals.pubintro_init, 0xBA);
	ff8_externals.ff8input_cfg_read = get_relative_call(ff8_externals.input_init, 0x5);
	ff8_externals.sub_468810 = get_relative_call(ff8_externals.sub_467C00, 0x59);
	ff8_externals.sub_468BD0 = get_relative_call(ff8_externals.sub_468810, 0x5B);
	common_externals.dinput_hack1 = ff8_externals.sub_468BD0 + 0x64;

	ff8_externals.pubintro_exit = get_absolute_value(ff8_externals.main_entry, 0x176);
	ff8_externals.pubintro_main_loop = get_absolute_value(ff8_externals.main_entry, 0x180);
	ff8_externals.credits_main_loop = get_absolute_value(ff8_externals.pubintro_main_loop, 0x6D);

	ff8_set_main_loop(MODE_CREDITS, ff8_externals.credits_main_loop);

	ff8_externals.load_credits_image = get_relative_call(ff8_externals.credits_main_loop, 0xBF);

	ff8_externals.credits_loop_state = (DWORD*)get_absolute_value(ff8_externals.load_credits_image, 0x7);
	ff8_externals.credits_counter = (DWORD *)get_absolute_value(ff8_externals.load_credits_image, 0x59);
	ff8_externals.sub_470520 = get_absolute_value(ff8_externals.credits_main_loop, 0xE2);
	ff8_externals.sub_4A24B0 = get_absolute_value(ff8_externals.sub_470520, 0x2B);
	ff8_externals.sub_470630 = get_absolute_value(ff8_externals.sub_4A24B0, 0xE4);
	ff8_externals.main_loop = get_absolute_value(ff8_externals.sub_470630, 0x24);

	ff8_externals.reg_get_data_drive = (uint32_t(*)(char*, DWORD))get_relative_call(ff8_externals.init_config, 0x21);
	ff8_externals.get_disk_number = get_relative_call(ff8_externals.main_loop, 0x1A);
	ff8_externals.disk_data_path = (char*)get_absolute_value(ff8_externals.get_disk_number, 0xF);
	ff8_externals.set_game_paths = (void (*)(int, char*, const char*))get_relative_call(ff8_externals.init_config, 0x3E);

	ff8_externals.savemap = (uint32_t**)get_absolute_value(ff8_externals.main_loop, 0x21);

	if (JP_VERSION)
	{
		ff8_externals.sm_pc_read = (uint32_t(*)(char*, void*))get_relative_call(ff8_externals.main_loop, 0x9C + 3);
		ff8_externals.cdcheck_main_loop = get_absolute_value(ff8_externals.main_loop, 0xBB + 3);
		common_externals._mode = (WORD *)get_absolute_value(ff8_externals.main_loop, 0x115 + 3);
		ff8_externals.field_main_loop = get_absolute_value(ff8_externals.main_loop, 0x144 + 3);
		common_externals.current_field_id = (WORD*)get_absolute_value(ff8_externals.main_loop, 0x21F + 6);
		ff8_externals.worldmap_enter_main = get_absolute_value(ff8_externals.main_loop, 0x2C0 + 4);
		ff8_externals.worldmap_main_loop = get_absolute_value(ff8_externals.main_loop, 0x2D0 + 4);
		ff8_externals.battle_main_loop = get_absolute_value(ff8_externals.main_loop, 0x340 + 4);
		// Search battle sound function to find play/stop midi related methods
		ff8_externals.sm_battle_sound = get_relative_call(ff8_externals.main_loop, 0x487 + 5);
		ff8_externals.swirl_main_loop = get_absolute_value(ff8_externals.main_loop, 0x4A3 + 5);
		ff8_externals.sub_470250 = get_relative_call(ff8_externals.main_loop, 0x6E7 - 15);
	}
	else
	{
		ff8_externals.sm_pc_read = (uint32_t(*)(char*, void*))get_relative_call(ff8_externals.main_loop, 0x9C);
		ff8_externals.cdcheck_main_loop = get_absolute_value(ff8_externals.main_loop, 0xBB);
		common_externals._mode = (WORD *)get_absolute_value(ff8_externals.main_loop, 0x115);
		ff8_externals.field_main_loop = get_absolute_value(ff8_externals.main_loop, 0x144);
		common_externals.current_field_id = (WORD*)get_absolute_value(ff8_externals.main_loop, 0x21F);
		ff8_externals.worldmap_enter_main = get_absolute_value(ff8_externals.main_loop, 0x2C0);
		ff8_externals.worldmap_main_loop = get_absolute_value(ff8_externals.main_loop, 0x2D0);
		ff8_externals.battle_main_loop = get_absolute_value(ff8_externals.main_loop, 0x340);
		// Search battle sound function to find play/stop midi related methods
		ff8_externals.sm_battle_sound = get_relative_call(ff8_externals.main_loop, 0x487);
		ff8_externals.swirl_main_loop = get_absolute_value(ff8_externals.main_loop, 0x4A3);
		ff8_externals.sub_470250 = get_relative_call(ff8_externals.main_loop, 0x6E7);
	}

	common_externals.debug_print2 = get_relative_call(uint32_t(ff8_externals.sm_pc_read), 0x16);
	ff8_externals.moriya_filesytem_open = get_relative_call(uint32_t(ff8_externals.sm_pc_read), 0x21);
	ff8_externals.moriya_filesytem_seek = get_relative_call(uint32_t(ff8_externals.sm_pc_read), 0x77);
	ff8_externals.moriya_filesytem_read = get_relative_call(uint32_t(ff8_externals.sm_pc_read), 0xB7);
	ff8_externals.moriya_filesytem_close = get_relative_call(uint32_t(ff8_externals.sm_pc_read), 0xDD);
	ff8_externals.free_file_container = (void(*)(ff8_file_container*))get_relative_call(ff8_externals.moriya_filesytem_close, 0x1F);

	ff8_externals.cdcheck_sub_52F9E0 = get_relative_call(ff8_externals.cdcheck_main_loop, 0x95);

	ff8_set_main_loop(MODE_CDCHECK, ff8_externals.cdcheck_main_loop);
	ff8_set_main_loop(MODE_SWIRL, ff8_externals.swirl_main_loop);
	ff8_set_main_loop(MODE_BATTLE, ff8_externals.battle_main_loop);
	ff8_set_main_loop(MODE_FIELD, ff8_externals.field_main_loop);
	ff8_set_main_loop(MODE_WORLDMAP, ff8_externals.worldmap_main_loop);

	ff8_externals.sub_47CCB0 = get_relative_call(ff8_externals.battle_main_loop, 0x1B3);
	ff8_externals.sub_534640 = get_relative_call(ff8_externals.sub_47CCB0, 0xF1);
	ff8_externals.sub_4972A0 = get_relative_call(ff8_externals.sub_534640, 0x51);
	ff8_externals.load_fonts = get_relative_call(ff8_externals.sub_4972A0, 0x16);

	ff8_externals.fonts = (font_object **)get_absolute_value(ff8_externals.load_fonts, JP_VERSION ? 0x17 : 0x16);

	common_externals.assert_malloc = (void* (*)(uint32_t, const char*, uint32_t))get_relative_call(ff8_externals.load_fonts, JP_VERSION ? 0x29 : 0x2A);

	ff8_externals.sub_471F70 = get_relative_call(ff8_externals.field_main_loop, 0x148);

	if (JP_VERSION)
	{
		ff8_externals.sub_4767B0 = get_relative_call(ff8_externals.sub_471F70, 0x4FE - 2);
		common_externals.update_field_entities = get_relative_call(ff8_externals.sub_4767B0, 0x14E + 1);
		ff8_externals.sub_4789A0 = get_relative_call(ff8_externals.sub_4767B0, 0x40F + 3);
		ff8_externals.stop_cdrom = (uint32_t(*)())get_relative_call(ff8_externals.sub_4767B0, 0xB46 - 13);
		ff8_externals.stop_cdrom_field_call = ff8_externals.sub_4767B0 + 0xB46 - 0xD;
		ff8_externals.sub_47CA90 = get_relative_call(ff8_externals.sub_4789A0, 0x674);
	}
	else
	{
		ff8_externals.sub_4767B0 = get_relative_call(ff8_externals.sub_471F70, 0x4FE);
		common_externals.update_field_entities = get_relative_call(ff8_externals.sub_4767B0, 0x14E);
		ff8_externals.sub_4789A0 = get_relative_call(ff8_externals.sub_4767B0, 0x40F);
		ff8_externals.stop_cdrom = (uint32_t(*)())get_relative_call(ff8_externals.sub_4767B0, 0xB46);
		ff8_externals.stop_cdrom_field_call = ff8_externals.sub_4767B0 + 0xB46;
		ff8_externals.sub_47CA90 = get_relative_call(ff8_externals.sub_4789A0, 0x68B);
	}

	ff8_externals.battle_trigger_field = ff8_externals.sub_47CA90 + 0x15;
	ff8_externals.sub_52B3A0 = (int (*)())get_relative_call(ff8_externals.battle_trigger_field, 0);
	ff8_externals.check_game_is_paused = (int32_t(*)(int32_t))get_relative_call(ff8_externals.field_main_loop, 0x16C);
	ff8_externals.pause_menu = (int(*)(int))get_relative_call(uint32_t(ff8_externals.check_game_is_paused), 0x88);
	ff8_externals.init_pause_menu = get_relative_call(uint32_t(ff8_externals.check_game_is_paused), 0xE2);
	ff8_externals.pause_menu_with_vibration = (int(*)(int))(ff8_externals.init_pause_menu - 0x290);
	ff8_externals.vibration_apply = get_relative_call(uint32_t(ff8_externals.pause_menu_with_vibration), 0xB4);
	ff8_externals.get_keyon = (int(*)(int, int))get_relative_call(uint32_t(ff8_externals.pause_menu_with_vibration), 0xC9);
	ff8_externals.get_vibration_capability = get_relative_call(uint32_t(ff8_externals.pause_menu_with_vibration), 0xE3);
	ff8_externals.set_vibration = get_relative_call(uint32_t(ff8_externals.pause_menu_with_vibration), 0x26A);
	ff8_externals.gamepad_vibration_states = (ff8_gamepad_vibration_state *)(get_absolute_value(ff8_externals.get_vibration_capability, 0xE + 3) - 0xB);
	ff8_externals.vibration_objects = (ff8_vibrate_struc *)get_absolute_value(ff8_externals.set_vibration, 0x2 + 1);
	ff8_externals.vibration_clear_intensity = get_relative_call(ff8_externals.sub_471F70, 0x276);
	ff8_externals.open_battle_vibrate_vib = get_relative_call(ff8_externals.pubintro_exit, 0x18);
	ff8_externals.vibrate_data_battle = (uint8_t **)get_absolute_value(ff8_externals.open_battle_vibrate_vib, 0x6);

	common_externals.get_movie_frame = get_relative_call(common_externals.update_field_entities, 0x26);

	common_externals.execute_opcode_table = (uint32_t*)get_absolute_value(common_externals.update_field_entities, 0x65A);
	ff8_externals.opcode_effectplay2 = common_externals.execute_opcode_table[0x21];
	ff8_externals.opcode_mes = common_externals.execute_opcode_table[0x47];
	ff8_externals.opcode_ask = common_externals.execute_opcode_table[0x4A];
	ff8_externals.opcode_movie = common_externals.execute_opcode_table[0x4F];
	ff8_externals.opcode_moviesync = common_externals.execute_opcode_table[0x50];
	ff8_externals.opcode_spuready = common_externals.execute_opcode_table[0x56];
	ff8_externals.opcode_movieready = common_externals.execute_opcode_table[0xA3];
	ff8_externals.opcode_musicload = common_externals.execute_opcode_table[0xB5];
	ff8_externals.opcode_crossmusic = common_externals.execute_opcode_table[0xBA];
	ff8_externals.opcode_dualmusic = common_externals.execute_opcode_table[0xBB];
	ff8_externals.opcode_musicvoltrans = common_externals.execute_opcode_table[0xC1];
	ff8_externals.opcode_musicvolfade = common_externals.execute_opcode_table[0xC2];
	ff8_externals.opcode_choicemusic = common_externals.execute_opcode_table[0x135];
	ff8_externals.opcode_musicskip = common_externals.execute_opcode_table[0x144];
	ff8_externals.opcode_musicvolsync = common_externals.execute_opcode_table[0x149];
	ff8_externals.opcode_getmusicoffset = common_externals.execute_opcode_table[0x16F];

	common_externals.cross_fade_midi = get_relative_call(ff8_externals.opcode_crossmusic, 0x5C);
	ff8_externals.music_load = get_relative_call(ff8_externals.opcode_musicload, 0x8C);

	common_externals.prepare_movie = get_relative_call(ff8_externals.opcode_movieready, 0x99);
	common_externals.release_movie_objects = get_relative_call(common_externals.prepare_movie, 0x19E);
	common_externals.start_movie = get_relative_call(ff8_externals.opcode_movie, 0xC3);
	common_externals.update_movie_sample = get_relative_call(common_externals.start_movie, 0x74);
	ff8_externals.draw_movie_frame = get_relative_call(ff8_externals.opcode_moviesync, 0x1C);
	common_externals.stop_movie = get_relative_call(common_externals.update_movie_sample, 0x3E2);
	ff8_externals.movie_object = (ff8_movie_obj *)get_absolute_value(common_externals.prepare_movie, 0xDB);

	common_externals.debug_print = get_relative_call(common_externals.update_movie_sample, 0x141);

	ff8_externals._load_texture = get_relative_call(ff8_externals.load_fonts, JP_VERSION ? 0x31B : 0x197);
	ff8_externals.sub_4076B6 = get_relative_call(ff8_externals._load_texture, 0x16D);
	ff8_externals.sub_41AC34 = get_relative_call(ff8_externals.sub_4076B6, 0x46);
	ff8_externals.load_texture_data = get_relative_call(ff8_externals.sub_41AC34, 0x168);
	common_externals.load_tex_file = get_relative_call(ff8_externals.load_texture_data, 0x103);
	common_externals.create_tex_header = (tex_header* (*)())get_relative_call(common_externals.load_tex_file, 0xD);
	common_externals.assert_calloc = (void* (*)(uint32_t, uint32_t, const char*, uint32_t))get_relative_call((uint32_t)common_externals.create_tex_header, 0x15);
	common_externals.open_file = get_relative_call(common_externals.load_tex_file, 0x27);
	common_externals.read_file = get_relative_call(common_externals.load_tex_file, 0x49);
	common_externals.alloc_read_file = (void* (*)(uint32_t, uint32_t, struct file *))get_relative_call(common_externals.load_tex_file, 0xB3);
	common_externals.close_file = get_relative_call(common_externals.load_tex_file, 0x15B);
	common_externals.destroy_tex = (void (*)(tex_header*))get_relative_call(common_externals.load_tex_file, 0x16D);
	common_externals.destroy_tex_header = get_relative_call((uint32_t)common_externals.destroy_tex, 0x78);
	common_externals.assert_free = (void* (*)(void*, const char*, uint32_t))get_relative_call(common_externals.destroy_tex_header, 0x21);
	common_externals.get_game_object = (game_obj* (*)())get_relative_call((uint32_t)common_externals.destroy_tex, 0x6);

	ff8_externals.fs_archive_search_filename = (int(*)(const char *, ff8_file_fi_infos *, const ff8_file_container *))get_relative_call(common_externals.open_file, 0x28D);
	ff8_externals.ff8_fs_archive_search_filename2 = (int(*)(const char *, ff8_file_fi_infos *, const ff8_file_container *))get_relative_call(uint32_t(ff8_externals.fs_archive_search_filename), 0x10);
	ff8_externals.fs_archive_get_fl_filepath = (char *(*)(int, const ff8_file_fl *))get_relative_call(uint32_t(ff8_externals.ff8_fs_archive_search_filename2), 0x40);
	ff8_externals._open = get_relative_call(common_externals.open_file, 0x2CE);
	ff8_externals._sopen = (int(*)(const char*, int, int, ...))get_relative_call(ff8_externals._open, 0xE);
	ff8_externals.fopen = get_relative_call(ff8_externals.ff8input_cfg_read, 0x33);
	ff8_externals._fsopen = (FILE *(*)(const char*, const char*, int))get_relative_call(ff8_externals.fopen, 0xA);
	ff8_externals.strcpy_with_malloc = (char*(*)(const char*))get_relative_call(common_externals.open_file, 0x2F2);

	ff8_externals.dd_d3d_start = get_relative_call(ff8_externals.pubintro_init, 0x75);
	ff8_externals.create_d3d_gfx_driver = get_relative_call(ff8_externals.dd_d3d_start, 0x88);
	ff8_externals.d3d_init = get_absolute_value(ff8_externals.create_d3d_gfx_driver, 0x1B);
	ff8_externals.sub_40BFEB = get_absolute_value(ff8_externals.d3d_init, 0x1370);
	common_externals.create_texture_format = (struct texture_format* (*)())get_relative_call(ff8_externals.sub_40BFEB, 0x2B);

	ff8_externals.tim2tex = get_relative_call(ff8_externals.sub_41AC34, 0xFC);
	ff8_externals.sub_41BC76 = get_relative_call(ff8_externals.tim2tex, 0x72);
	common_externals.make_pixelformat = (void (*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, struct texture_format*))get_relative_call(ff8_externals.sub_41BC76, 0x102);

	common_externals.add_texture_format = (void (*)(struct texture_format*, game_obj*))get_relative_call(ff8_externals.sub_40BFEB, 0xBF);

	ff8_externals.d3d_load_texture = get_absolute_value(ff8_externals.create_d3d_gfx_driver, 0x9D);
	common_externals.create_texture_set = (texture_set* (*)())get_relative_call(ff8_externals.d3d_load_texture, 0x6B);

	common_externals.create_palette_for_tex = (palette* (*)(uint32_t, tex_header*, texture_set*))get_relative_call(ff8_externals.d3d_load_texture, 0x316);

	ff8_externals.movie_hack1 = common_externals.update_movie_sample + 0xA5;
	ff8_externals.movie_hack2 = common_externals.update_movie_sample + 0x1DF;

	ff8_externals.sub_559910 = get_relative_call(ff8_externals.swirl_main_loop, 0x1A);

	ff8_externals.swirl_sub_56D1D0 = get_relative_call(ff8_externals.battle_main_loop, 0x285);
	ff8_externals.swirl_sub_56D390 = get_relative_call(ff8_externals.swirl_sub_56D1D0, 0x2A);
	ff8_externals.swirl_texture1 = (ff8_graphics_object **)get_absolute_value(ff8_externals.swirl_sub_56D1D0, 0x1);

	ff8_externals.sub_52FE80 = get_relative_call(ff8_externals.load_credits_image, 0xA4);
	ff8_externals.sub_45D610 = get_relative_call(ff8_externals.sub_52FE80, 0x90);
	ff8_externals.sub_45D080 = get_relative_call(ff8_externals.sub_45D610, 0x5);
	ff8_externals.sub_464BD0 = get_relative_call(ff8_externals.sub_45D080, 0x208);
	ff8_externals.sub_4653B0 = get_relative_call(ff8_externals.sub_464BD0, 0x79);
	ff8_externals.sub_465720 = get_relative_call(ff8_externals.sub_464BD0, 0xAF);

	ff8_externals.ssigpu_callbacks_1 = (uint32_t *)get_absolute_value(ff8_externals.sub_45D080, 0x21E);
	ff8_externals.sub_462AD0 = get_relative_call(ff8_externals.ssigpu_callbacks_1[116], 0x13);
	ff8_externals.sub_462DF0 = get_relative_call(ff8_externals.sub_462AD0, 0x62);
	ff8_externals.ssigpu_tx_select_2_sub_465CE0 = get_relative_call(ff8_externals.sub_462DF0, 0x33);
	ff8_externals.sub_464F70 = (int(*)(int,int,int,int,int,int,int,int,int,uint8_t*))get_relative_call(ff8_externals.ssigpu_tx_select_2_sub_465CE0, 0x281);
	ff8_externals.sub_4675C0 = (void(*)(uint8_t*,int,uint8_t*,int,signed int,int,int))get_relative_call(uint32_t(ff8_externals.sub_464F70), 0x2C5);

	ff8_externals.sub_559E40 = get_relative_call(ff8_externals.swirl_main_loop, 0x28);
	ff8_externals.sub_559F30 = get_relative_call(ff8_externals.sub_559E40, 0xC1);

	if(NV_VERSION)
	{
		ff8_externals.nvidia_hack1 = get_absolute_value(ff8_externals.sub_559F30, 0x3E);
		ff8_externals.nvidia_hack2 = get_absolute_value(ff8_externals.sub_559F30, 0xAC);
	}

	ff8_externals.menu_viewport = (sprite_viewport *)(get_absolute_value(ff8_externals.sub_4972A0, 0x12) - 0x20);

	ff8_externals.sub_497380 = get_relative_call(ff8_externals.sub_4A24B0, 0xAA);
	ff8_externals.sub_4B3410 = get_relative_call(ff8_externals.sub_497380, 0xAC);
	ff8_externals.sub_4BE4D0 = get_relative_call(ff8_externals.sub_4B3410, 0x68);
	ff8_externals.sub_4BECC0 = get_relative_call(ff8_externals.sub_4BE4D0, 0x39);
	ff8_externals.menu_draw_text = get_relative_call(ff8_externals.sub_4BECC0, 0x127);
	ff8_externals.get_character_width = (uint32_t (*)(uint32_t))get_relative_call(ff8_externals.menu_draw_text, JP_VERSION ? 0x1E1 : 0x1D0);

	ff8_externals.open_lzs_image = get_relative_call(ff8_externals.load_credits_image, 0x27);
	ff8_externals.credits_open_file = (uint32_t (*)(char*,char*))get_relative_call(ff8_externals.open_lzs_image, 0x72);
	ff8_externals.upload_psx_vram = get_relative_call(ff8_externals.open_lzs_image, 0xB9);
	ff8_externals.psxvram_buffer = (WORD *)get_absolute_value(ff8_externals.upload_psx_vram, 0x34);
	ff8_externals.sub_464850 = (void (*)(uint32_t, uint32_t, uint32_t, uint32_t))get_relative_call(ff8_externals.upload_psx_vram, 0x8A);

	ff8_externals.psx_texture_pages = (struc_51 *)get_absolute_value(ff8_externals.sub_464BD0, 0x10);

	ff8_externals.read_field_data = get_relative_call(ff8_externals.sub_471F70, 0x23A);
	ff8_externals.upload_mim_file = get_relative_call(ff8_externals.read_field_data, JP_VERSION ? 0x723 : 0x729);
	ff8_externals.field_filename = (char *)get_absolute_value(ff8_externals.read_field_data, 0xF0);

	ff8_externals.load_field_models = get_relative_call(ff8_externals.read_field_data, JP_VERSION ? 0xFA2 : 0xF0F);

	ff8_externals.worldmap_sub_53F310 = get_relative_call(ff8_externals.worldmap_enter_main, 0xA7);

	ff8_externals.wm_upload_psx_vram = get_relative_call(ff8_externals.load_field_models, 0xB72);

	ff8_externals.engine_eval_process_input = get_relative_call(ff8_externals.pubintro_main_loop, 0x4);
	ff8_externals.engine_eval_keyboard_gamepad_input = get_relative_call(ff8_externals.engine_eval_process_input, 0x16);
	ff8_externals.has_keyboard_gamepad_input = get_relative_call(ff8_externals.engine_eval_process_input, 0x1B);
	ff8_externals.engine_gamepad_button_pressed = (BYTE*)get_absolute_value(ff8_externals.has_keyboard_gamepad_input, 0x22);
	ff8_externals.engine_mapped_buttons = (DWORD*)get_absolute_value(ff8_externals.engine_eval_keyboard_gamepad_input, 0xB9);

	common_externals.get_keyboard_state = get_relative_call(ff8_externals.engine_eval_keyboard_gamepad_input, 0x11);
	ff8_externals.dinput_init_gamepad = get_relative_call(ff8_externals.sub_468810, 0xB4);
	ff8_externals.dinput_update_gamepad_status = get_relative_call(ff8_externals.engine_eval_keyboard_gamepad_input, 0x1B);
	ff8_externals.dinput_gamepad_device = (LPDIRECTINPUTDEVICE8A)get_absolute_value(ff8_externals.dinput_update_gamepad_status, 0x16);
	ff8_externals.dinput_gamepad_state = (LPDIJOYSTATE2)get_absolute_value(ff8_externals.dinput_update_gamepad_status, 0x1B);

	common_externals.dinput_acquire_keyboard = (int (*)())get_relative_call(common_externals.get_keyboard_state, 0x34);
	common_externals.keyboard_device = (IDirectInputDeviceA**)get_absolute_value(common_externals.get_keyboard_state, 0x01);
	common_externals.keyboard_connected = (uint32_t*)get_absolute_value(common_externals.get_keyboard_state, 0x2C);

	ff8_externals.initialize_sound = get_relative_call(ff8_externals.pubintro_init, 0xD8); // sub_469640
	common_externals.directsound_create = get_relative_call(ff8_externals.initialize_sound, 0x5D); // sub_46DBF0
	common_externals.directsound = (LPLPDIRECTSOUND)get_absolute_value(common_externals.directsound_create, 0x26);
	common_externals.directsound_release = get_relative_call(ff8_externals.initialize_sound, 0xD5);

	common_externals.midi_init = get_relative_call(ff8_externals.pubintro_init, 0x130);
	ff8_externals.music_path = (char*)get_absolute_value(ff8_externals.pubintro_init, 0x136);
	common_externals.midi_cleanup = get_relative_call(ff8_externals.pubintro_init, 0x1B4);
	ff8_externals.sounds_cleanup = get_relative_call(ff8_externals.pubintro_init, 0x1B9);
	common_externals.wav_cleanup = get_relative_call(ff8_externals.sounds_cleanup, 0x0);
	ff8_externals.volume_update = get_relative_call(ff8_externals.field_main_loop, 0x28C);
	ff8_externals.volume_music_update = get_relative_call(ff8_externals.volume_update, 0x6);

	ff8_externals.sdmusicplay = get_relative_call(ff8_externals.sm_battle_sound, 0x164);
	ff8_externals.sd_music_play = (uint32_t(*)(uint32_t, char*, uint32_t))get_relative_call(ff8_externals.sdmusicplay, 0x17);
	ff8_externals.current_music_ids = (uint32_t*)get_absolute_value(uint32_t(ff8_externals.sd_music_play), 0x1AA);
	ff8_externals.play_wav = get_relative_call(uint32_t(ff8_externals.sd_music_play), 0x1DC);
	common_externals.play_wav = get_relative_call(ff8_externals.play_wav, 0x73);

	common_externals.play_midi = get_relative_call(uint32_t(ff8_externals.sd_music_play), 0x20C);

	ff8_externals.dmusic_segment_connect_to_dls = get_relative_call(common_externals.play_midi, 0x247);
	ff8_externals.load_midi_segment = get_relative_call(common_externals.midi_init, 0xC8);
	ff8_externals.choice_music = get_relative_call(ff8_externals.opcode_choicemusic, 0x5D);
	ff8_externals.sd_music_play_at = get_relative_call(ff8_externals.opcode_musicskip, 0x46);
	ff8_externals.load_and_play_midi_segment = get_relative_call(ff8_externals.choice_music, 0x99);
	ff8_externals.load_midi_segment_from_id = get_relative_call(ff8_externals.choice_music, 0xD0);
	ff8_externals.stop_midi_segments = get_relative_call(ff8_externals.load_and_play_midi_segment, 0xB);
	ff8_externals.play_midi_segments = get_relative_call(ff8_externals.choice_music, 0x172);

	common_externals.get_midi_name = (char* (*)(uint32_t))get_relative_call(common_externals.play_midi, 0x21C);

	ff8_externals.sub_46B800 = get_relative_call(ff8_externals.sm_battle_sound, 0x52);
	ff8_externals.stop_music = get_relative_call(ff8_externals.sub_46B800, 0xB);
	common_externals.set_midi_volume_trans = get_relative_call(ff8_externals.opcode_musicvoltrans, 0x49); // Formally music_volume_trans
	common_externals.set_midi_volume_fade = get_relative_call(ff8_externals.opcode_musicvolfade, 0x59); // Formally music_volume_fade
	ff8_externals.set_midi_volume = get_relative_call(ff8_externals.stop_music, 0x22);
	common_externals.set_midi_volume = get_relative_call(ff8_externals.sm_battle_sound, 0x173); // Formally set_music_volume

	common_externals.stop_wav = get_relative_call(ff8_externals.stop_music, 0x14);
	common_externals.stop_midi = get_relative_call(ff8_externals.stop_music, 0x2A);

	ff8_externals.sub_46C050 = get_relative_call(ff8_externals.sm_battle_sound, 0x5C);
	common_externals.remember_midi_playing_time = get_relative_call(ff8_externals.sub_46C050, 0x00);

	common_externals.midi_status = get_relative_call(ff8_externals.sm_battle_sound, 0x14);

	ff8_externals.load_cdrom = (uint32_t(*)())get_relative_call(ff8_externals.opcode_spuready, 0x4B);
	ff8_externals.load_cdrom_call = ff8_externals.opcode_spuready + 0x4B;
	ff8_externals.play_cdrom = (uint32_t(*)(uint32_t, uint32_t, uint32_t))get_relative_call(ff8_externals.opcode_spuready, 0x60);
	ff8_externals.play_cdrom_call = ff8_externals.opcode_spuready + 0x60;
	ff8_externals.stop_cdrom_cleanup_call = ff8_externals.pubintro_init + 0x183;

	// Pause/Resume functions
	ff8_externals.sub_500900 = get_relative_call(ff8_externals.sub_47CCB0, 0x98D);
	ff8_externals.sub_501B60 = get_relative_call(ff8_externals.sub_500900, -0x2A2);
	ff8_externals.pause_music_and_sfx = get_relative_call(ff8_externals.sub_501B60, 0x54);
	common_externals.pause_wav = get_relative_call(ff8_externals.pause_music_and_sfx, 0xF);
	common_externals.pause_midi = get_relative_call(ff8_externals.pause_music_and_sfx, 0x17);
	ff8_externals.restart_music_and_sfx = get_relative_call(ff8_externals.sub_501B60, 0xB3);
	common_externals.restart_wav = get_relative_call(ff8_externals.restart_music_and_sfx, 0xF);
	common_externals.restart_midi = get_relative_call(ff8_externals.restart_music_and_sfx, 0x17);

	// Search DirectSoundBuffer initilization
	ff8_externals.sub_4A6680 = get_relative_call(ff8_externals.sub_47CCB0, 0xF6);
	ff8_externals.sub_4A6660 = get_absolute_value(ff8_externals.sub_4A6680, 0x36F + 1);
	ff8_externals.sub_4A3D20 = get_relative_call(ff8_externals.sub_4A6660, 0x00);
	ff8_externals.sub_4A3EE0 = get_relative_call(ff8_externals.sub_4A3D20, 0x12A);
	ff8_externals.sub_469C60 = get_relative_call(ff8_externals.sub_4A3EE0, 0x418);
	ff8_externals.sub_46DDC0 = get_relative_call(ff8_externals.sub_469C60, 0x22A);
	common_externals.directsound_buffer_flags_1 = ff8_externals.sub_46DDC0 + 0x34 - 2;

	ff8_externals.sub_5304B0 = (int (*)())get_relative_call(common_externals.update_movie_sample, 0x3D9);

	ff8_externals.enable_framelimiter = (uint32_t *)get_absolute_value(common_externals.stop_movie, 0x49);

	ff8_externals.byte_1CE4907 = (unsigned char *)get_absolute_value(common_externals.update_movie_sample, 0x363);
	ff8_externals.byte_1CE4901 = (unsigned char *)get_absolute_value(common_externals.update_movie_sample, 0x37D);
	ff8_externals.byte_1CE490D = (unsigned char *)get_absolute_value(common_externals.update_movie_sample, 0x3BE);

	ff8_externals.sub_45B310 = get_relative_call(ff8_externals.pubintro_init, 0x91);
	ff8_externals.sub_45B460 = get_relative_call(ff8_externals.sub_45B310, 0x0);
	ff8_externals.ssigpu_init = get_relative_call(ff8_externals.sub_45B460, 0x26);
	ff8_externals.d3dcaps = (uint32_t *)get_absolute_value(ff8_externals.ssigpu_init, 0x6C);

	if(version == VERSION_FF8_12_US || version == VERSION_FF8_12_US_NV || version == VERSION_FF8_12_US_EIDOS || version == VERSION_FF8_12_US_EIDOS_NV)
	{
		ff8_externals.worldmap_sub_53F310_loc_53F7EE = ff8_externals.worldmap_sub_53F310 + 0x4DE;

		ff8_externals.show_vram_window = (void (*)())get_relative_call(ff8_externals.worldmap_main_loop, 0xA0);
		ff8_externals.refresh_vram_window = (void (*)())get_relative_call(ff8_externals.worldmap_main_loop, 0xA5);
		ff8_externals.sub_53FAC0 = get_relative_call(ff8_externals.worldmap_main_loop, 0x134);
		ff8_externals.sub_54B460 = get_relative_call(ff8_externals.sub_53FAC0, 0x5D7);

		ff8_externals.sub_549E80 = get_relative_call(ff8_externals.sub_53FAC0, 0x1D5);
		ff8_externals.sub_550070 = get_relative_call(ff8_externals.sub_53FAC0, 0x278);
		ff8_externals.vibrate_data_world = (uint8_t *)get_absolute_value(ff8_externals.sub_550070, 0xA82);
		ff8_externals.sub_53BB90 = get_relative_call(ff8_externals.sub_53FAC0, 0x2D4);
		ff8_externals.sub_53E2A0 = get_relative_call(ff8_externals.sub_53BB90, 0x327);
		ff8_externals.sub_53E6B0 = get_relative_call(ff8_externals.sub_53E2A0, 0x36B);
		ff8_externals.sub_4023D0 = get_relative_call(ff8_externals.sub_53BB90, 0xAB1);
		ff8_externals.sub_53C750 = get_relative_call(ff8_externals.sub_53FAC0, 0x2DB);
		ff8_externals.sub_54FDA0 = get_relative_call(ff8_externals.sub_53FAC0, 0x375);
		ff8_externals.sub_54D7E0 = get_relative_call(ff8_externals.sub_53FAC0, 0x3C2);
		ff8_externals.sub_544630 = get_relative_call(ff8_externals.sub_53FAC0, 0x3D2);
		ff8_externals.sub_545EA0 = get_relative_call(ff8_externals.sub_53FAC0, 0x4BF);

		ff8_externals.sub_545F10 = get_relative_call(ff8_externals.sub_545EA0, 0x20);

		ff8_externals.sub_546100 = get_relative_call(ff8_externals.sub_545F10, 0x58);

		ff8_externals.battle_trigger_worldmap = ff8_externals.sub_53FAC0 + 0x4E6;
	}
	else
	{
		ff8_externals.worldmap_sub_53F310_loc_53F7EE = ff8_externals.worldmap_sub_53F310 + 0x507;

		ff8_externals.show_vram_window = (void (*)())get_relative_call(ff8_externals.worldmap_main_loop, 0xA3);
		ff8_externals.refresh_vram_window = (void (*)())get_relative_call(ff8_externals.worldmap_main_loop, 0xA8);
		ff8_externals.sub_53FAC0 = get_relative_call(ff8_externals.worldmap_main_loop, 0x137);
		ff8_externals.sub_54B460 = get_relative_call(ff8_externals.sub_53FAC0, 0x5D9);

		ff8_externals.sub_549E80 = get_relative_call(ff8_externals.sub_53FAC0, 0x1D6);
		ff8_externals.sub_550070 = get_relative_call(ff8_externals.sub_53FAC0, 0x279);
		ff8_externals.vibrate_data_world = (uint8_t *)get_absolute_value(ff8_externals.sub_550070, 0xAFA);
		ff8_externals.sub_53BB90 = get_relative_call(ff8_externals.sub_53FAC0, 0x2D5);
		ff8_externals.sub_53E2A0 = get_relative_call(ff8_externals.sub_53BB90, 0x336);
		ff8_externals.sub_53E6B0 = get_relative_call(ff8_externals.sub_53E2A0, 0x39A);
		ff8_externals.sub_4023D0 = get_relative_call(ff8_externals.sub_53BB90, 0xAE5);
		if (JP_VERSION)
		{
			ff8_externals.sub_4023D0 = get_relative_call(ff8_externals.sub_4023D0, 0x0);
		}
		ff8_externals.sub_53C750 = get_relative_call(ff8_externals.sub_53FAC0, 0x2DC);
		ff8_externals.sub_54FDA0 = get_relative_call(ff8_externals.sub_53FAC0, 0x376);
		ff8_externals.sub_54D7E0 = get_relative_call(ff8_externals.sub_53FAC0, 0x3C4);
		ff8_externals.sub_544630 = get_relative_call(ff8_externals.sub_53FAC0, 0x3D5);
		ff8_externals.sub_545EA0 = get_relative_call(ff8_externals.sub_53FAC0, 0x4C1);

		ff8_externals.sub_545F10 = get_relative_call(ff8_externals.sub_545EA0, 0x1C);

		ff8_externals.sub_546100 = get_relative_call(ff8_externals.sub_545F10, 0x54);

		ff8_externals.sub_54A0D0 = 0x54A0D0;

		ff8_externals.battle_trigger_worldmap = ff8_externals.sub_53FAC0 + 0x4EA;
	}

	ff8_externals.sub_548080 = get_relative_call(ff8_externals.worldmap_sub_53F310_loc_53F7EE, 0x9B);
	ff8_externals.sub_541C80 = (int (*)(int))get_relative_call(ff8_externals.battle_trigger_worldmap, 0);

	ff8_externals.sub_558D70 = get_relative_call(ff8_externals.sub_54B460, 0x3F3);

	common_externals.current_field_name = (char*)get_absolute_value(ff8_externals.opcode_effectplay2, 0x75);
	common_externals.previous_field_id = (WORD*)get_absolute_value(ff8_externals.sub_470250, 0x13);
	common_externals.update_entities_call = common_externals.update_field_entities + 0x657;

	ff8_externals.field_get_dialog_string = get_relative_call(ff8_externals.opcode_mes, 0x5D);
	ff8_externals.set_window_object = get_relative_call(ff8_externals.opcode_mes, 0x66);
	ff8_externals.windows = (ff8_win_obj*)get_absolute_value(ff8_externals.set_window_object, 0x11);

	ff8_externals.sub_470440 = get_absolute_value(ff8_externals.credits_main_loop, 0xD2);
	ff8_externals.sub_49ACD0 = get_relative_call(ff8_externals.sub_470440, JP_VERSION ? 0x9C : 0x98);
	ff8_externals.sub_4A0880 = get_relative_call(ff8_externals.sub_49ACD0, 0x58);
	ff8_externals.sub_4A0C00 = get_absolute_value(ff8_externals.sub_4A0880, 0x33);
	ff8_externals.show_dialog = (char(*)(int32_t, uint32_t, int16_t))get_relative_call(ff8_externals.sub_4A0C00, 0x5F);

	// Required by Steam edition
	switch (version)
	{
	case VERSION_FF8_12_US_NV:
		ff8_externals.requiredDisk = 0xB8EE90;
		break;
	case VERSION_FF8_12_FR_NV:
		ff8_externals.requiredDisk = 0xB8EDB8;
		break;
	case VERSION_FF8_12_DE_NV:
		ff8_externals.requiredDisk = 0xB8EDC0;
		break;
	case VERSION_FF8_12_SP_NV:
		ff8_externals.requiredDisk = 0xB8EDC0;
		break;
	case VERSION_FF8_12_IT_NV:
		ff8_externals.requiredDisk = 0xB8EDB8;
		break;
	case VERSION_FF8_12_JP:
		ff8_externals.requiredDisk = 0xD92BB0;
		break;
	}
}

void ff8_data()
{
	num_modes = sizeof(ff8_modes) / sizeof(ff8_modes[0]);

	ff8_find_externals();

	memcpy(modes, ff8_modes, sizeof(ff8_modes));

	text_colors[TEXTCOLOR_GRAY] = 0x08;
	text_colors[TEXTCOLOR_BLUE] = 0x01;
	text_colors[TEXTCOLOR_RED] = 0x04;
	text_colors[TEXTCOLOR_PINK] = 0x05;
	text_colors[TEXTCOLOR_GREEN] = 0x02;
	text_colors[TEXTCOLOR_LIGHT_BLUE] = 0x09;
	text_colors[TEXTCOLOR_YELLOW] = 0x0E;
	text_colors[TEXTCOLOR_WHITE] = 0x0F;
}
