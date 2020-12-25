/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2020 Marcin Gomulak                                     //
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

// FF8 game mode definitions
static struct game_mode ff8_modes[] = {
	{FF8_MODE_CREDITS,       "MODE_CREDITS",       MODE_CREDITS,       true },
	{FF8_MODE_FIELD,         "MODE_FIELD",         MODE_FIELD,         true },
	{FF8_MODE_WORLDMAP,      "MODE_WORLDMAP",      MODE_WORLDMAP,      true },
	{FF8_MODE_SWIRL,         "MODE_SWIRL",         MODE_SWIRL,         true },
	{FF8_MODE_AFTER_BATTLE,  "MODE_AFTER_BATTLE",  MODE_AFTER_BATTLE,  true },
	{FF8_MODE_5,             "MODE_5",             MODE_UNKNOWN,       true },
	{FF8_MODE_MENU,          "MODE_MENU",          MODE_MENU,          true },
	{FF8_MODE_7,             "MODE_7",             MODE_UNKNOWN,       true },
	{FF8_MODE_CARDGAME,      "MODE_CARDGAME",      MODE_CARDGAME,      true },
	{FF8_MODE_9,             "MODE_9",             MODE_UNKNOWN,       true },
	{FF8_MODE_TUTO,          "MODE_TUTO",          MODE_UNKNOWN,       true },
	{FF8_MODE_11,            "MODE_11",            MODE_UNKNOWN,       true },
	{FF8_MODE_INTRO,         "MODE_INTRO",         MODE_INTRO,         true },
	{FF8_MODE_100,           "MODE_100",           MODE_UNKNOWN,       true },
	{FF8_MODE_BATTLE,        "MODE_BATTLE",        MODE_BATTLE,        true },
};

void ff8_set_main_loop(uint32_t driver_mode, uint32_t main_loop)
{
	uint32_t i;

	for(i = 0; i < num_modes; i++) if(ff8_modes[i].driver_mode == driver_mode) ff8_modes[i].main_loop = main_loop;
}

void ff8_find_externals()
{
	common_externals.diff_time = get_relative_call(common_externals.winmain, 0x41E);

	ff8_externals.sub_401ED0 = version == VERSION_FF8_12_JP ? 0x402290 : 0x401ED0;
	ff8_externals.pubintro_init = get_absolute_value(ff8_externals.sub_401ED0, 0x158);
	ff8_externals.sub_467C00 = get_relative_call(ff8_externals.pubintro_init, 0xB5);
	ff8_externals.sub_468810 = get_relative_call(ff8_externals.sub_467C00, 0x59);
	ff8_externals.sub_468BD0 = get_relative_call(ff8_externals.sub_468810, 0x5B);
	common_externals.dinput_hack1 = ff8_externals.sub_468BD0 + 0x64;

	ff8_externals.pubintro_main_loop = get_absolute_value(ff8_externals.sub_401ED0, 0x180);
	ff8_externals.credits_main_loop = get_absolute_value(ff8_externals.pubintro_main_loop, 0x6D);

	ff8_set_main_loop(MODE_CREDITS, ff8_externals.credits_main_loop);

	ff8_externals.sub_52F300 = get_relative_call(ff8_externals.credits_main_loop, 0xBF);
	ff8_externals.credits_loop_state = (DWORD*)get_absolute_value(ff8_externals.sub_52F300, 0x7);
	ff8_externals.credits_counter = (DWORD *)get_absolute_value(ff8_externals.sub_52F300, 0x59);
	ff8_externals.sub_470520 = get_absolute_value(ff8_externals.credits_main_loop, 0xE2);
	ff8_externals.sub_4A24B0 = get_absolute_value(ff8_externals.sub_470520, 0x2B);
	ff8_externals.sub_470630 = get_absolute_value(ff8_externals.sub_4A24B0, 0xE4);
	ff8_externals.main_loop = get_absolute_value(ff8_externals.sub_470630, 0x24);

	ff8_externals.swirl_main_loop = get_absolute_value(ff8_externals.main_loop, 0x4A3);

	ff8_set_main_loop(MODE_SWIRL, ff8_externals.swirl_main_loop);

	ff8_externals.battle_main_loop = get_absolute_value(ff8_externals.swirl_main_loop, 0x50);
	ff8_externals.is_window_active = get_relative_call(ff8_externals.battle_main_loop, 0x15B);
	ff8_externals.is_window_active_sub1 = get_relative_call(ff8_externals.is_window_active, 0x16);
	ff8_externals.is_window_active_sub2 = get_relative_call(ff8_externals.is_window_active, 0x1B);

	ff8_set_main_loop(MODE_BATTLE, ff8_externals.battle_main_loop);

	ff8_externals.sub_47CF60 = get_absolute_value(ff8_externals.main_loop, 0x340);
	ff8_externals.sub_47CCB0 = get_relative_call(ff8_externals.sub_47CF60, 0x1B3);
	ff8_externals.sub_534640 = get_relative_call(ff8_externals.sub_47CCB0, 0xF1);
	ff8_externals.sub_4972A0 = get_relative_call(ff8_externals.sub_534640, 0x51);
	ff8_externals.load_fonts = get_relative_call(ff8_externals.sub_4972A0, 0x16);

	ff8_externals.fonts = (font_object **)get_absolute_value(ff8_externals.load_fonts, 0x16);

	common_externals.assert_malloc = (void* (*)(uint32_t, const char*, uint32_t))get_relative_call(ff8_externals.load_fonts, 0x2A);

	common_externals._mode = (WORD *)get_absolute_value(ff8_externals.main_loop, 0x115);

	ff8_externals.pubintro_enter_main = get_absolute_value(ff8_externals.sub_401ED0, 0x16C);
	common_externals.prepare_movie = get_relative_call(ff8_externals.pubintro_enter_main, 0x12);
	common_externals.release_movie_objects = get_relative_call(common_externals.prepare_movie, 0x19E);
	common_externals.start_movie = get_relative_call(ff8_externals.pubintro_enter_main, 0x1A);

	ff8_externals.field_main_loop = get_absolute_value(ff8_externals.main_loop, 0x144);

	ff8_set_main_loop(MODE_FIELD, ff8_externals.field_main_loop);

	ff8_externals.sub_471F70 = get_relative_call(ff8_externals.field_main_loop, 0x148);
	ff8_externals.sub_4767B0 = get_relative_call(ff8_externals.sub_471F70, 0x4FE);
	ff8_externals.sub_4789A0 = get_relative_call(ff8_externals.sub_4767B0, 0x40F);
	ff8_externals.sub_47CA90 = get_relative_call(ff8_externals.sub_4789A0, 0x68B);
	ff8_externals.battle_trigger_field = ff8_externals.sub_47CA90 + 0x15;
	ff8_externals.sub_52B3A0 = (int (*)())get_relative_call(ff8_externals.battle_trigger_field, 0);

	common_externals.update_movie_sample = get_relative_call(ff8_externals.sub_4767B0, 0x5A5);

	ff8_externals.draw_movie_frame = get_relative_call(ff8_externals.sub_4767B0, 0xB84);
	common_externals.stop_movie = get_relative_call(common_externals.update_movie_sample, 0x3E2);

	ff8_externals.sub_529FF0 = get_relative_call(ff8_externals.sub_4767B0, 0x14E);
	common_externals.get_movie_frame = get_relative_call(ff8_externals.sub_529FF0, 0x26);

	common_externals.execute_opcode_table = (uint32_t*)get_absolute_value(ff8_externals.sub_529FF0, 0x65A);
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

	ff8_externals.movie_object = (ff8_movie_obj *)get_absolute_value(common_externals.prepare_movie, 0xDB);

	common_externals.debug_print = get_relative_call(common_externals.update_movie_sample, 0x141);

	ff8_externals._load_texture = get_relative_call(ff8_externals.load_fonts, 0x197);
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

	ff8_externals.swirl_sub_56D1D0 = get_relative_call(ff8_externals.sub_47CF60, 0x285);
	ff8_externals.swirl_sub_56D390 = get_relative_call(ff8_externals.swirl_sub_56D1D0, 0x2A);
	ff8_externals.swirl_texture1 = (ff8_graphics_object **)get_absolute_value(ff8_externals.swirl_sub_56D1D0, 0x1);

	ff8_externals.load_credits_image = get_relative_call(ff8_externals.credits_main_loop, 0xBF);
	ff8_externals.sub_52FE80 = get_relative_call(ff8_externals.load_credits_image, 0xA4);
	ff8_externals.sub_45D610 = get_relative_call(ff8_externals.sub_52FE80, 0x90);
	ff8_externals.sub_45D080 = get_relative_call(ff8_externals.sub_45D610, 0x5);
	ff8_externals.sub_464BD0 = get_relative_call(ff8_externals.sub_45D080, 0x208);
	ff8_externals.sub_4653B0 = get_relative_call(ff8_externals.sub_464BD0, 0x79);
	ff8_externals.sub_465720 = get_relative_call(ff8_externals.sub_464BD0, 0xAF);

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
	ff8_externals.get_character_width = (uint32_t (*)(uint32_t))get_relative_call(ff8_externals.menu_draw_text, 0x1D0);

	ff8_externals.open_lzs_image = get_relative_call(ff8_externals.load_credits_image, 0x27);
	ff8_externals.upload_psx_vram = get_relative_call(ff8_externals.open_lzs_image, 0xB9);
	ff8_externals.psxvram_buffer = (WORD *)get_absolute_value(ff8_externals.upload_psx_vram, 0x34);
	ff8_externals.sub_464850 = (void (*)(uint32_t, uint32_t, uint32_t, uint32_t))get_relative_call(ff8_externals.upload_psx_vram, 0x8A);

	ff8_externals.psx_texture_pages = (struc_51 *)get_absolute_value(ff8_externals.sub_464BD0, 0x10);

	ff8_externals.read_field_data = get_relative_call(ff8_externals.sub_471F70, 0x23A);
	ff8_externals.upload_mim_file = get_relative_call(ff8_externals.read_field_data, 0x729);
	ff8_externals.field_filename = (char *)get_absolute_value(ff8_externals.read_field_data, 0xF0);

	ff8_externals.load_field_models = get_relative_call(ff8_externals.read_field_data, 0xF0F);

	ff8_externals.worldmap_main_loop = get_absolute_value(ff8_externals.main_loop, 0x2D0);

	ff8_set_main_loop(MODE_WORLDMAP, ff8_externals.worldmap_main_loop);

	ff8_externals.worldmap_enter_main = get_absolute_value(ff8_externals.main_loop, 0x2C0);
	ff8_externals.worldmap_sub_53F310 = get_relative_call(ff8_externals.worldmap_enter_main, 0xA7);

	ff8_externals.show_vram_window = (void (*)())get_relative_call(ff8_externals.worldmap_main_loop, 0xA0);
	ff8_externals.refresh_vram_window = (void (*)())get_relative_call(ff8_externals.worldmap_main_loop, 0xA5);
	ff8_externals.wm_upload_psx_vram = get_relative_call(ff8_externals.load_field_models, 0xB72);

	ff8_externals.check_active_window = get_relative_call(ff8_externals.pubintro_main_loop, 0x4);
	ff8_externals.sub_467D10 = get_relative_call(ff8_externals.check_active_window, 0x16);

	common_externals.get_keyboard_state = get_relative_call(ff8_externals.sub_467D10, 0x11);
	ff8_externals.dinput_init_gamepad = get_relative_call(ff8_externals.sub_468810, 0xB4);
	ff8_externals.dinput_sub_4692B0 = get_relative_call(ff8_externals.sub_467D10, 0x1B);
	ff8_externals.dinput_gamepad_device = (LPDIRECTINPUTDEVICE8A)get_absolute_value(ff8_externals.dinput_sub_4692B0, 0x16);
	ff8_externals.dinput_gamepad_state = (LPDIJOYSTATE2)get_absolute_value(ff8_externals.dinput_sub_4692B0, 0x1B);

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

	// Search battle sound function to find play/stop midi related methods
	ff8_externals.sm_battle_sound = get_relative_call(ff8_externals.main_loop, 0x69D);

	ff8_externals.sdmusicplay = get_relative_call(ff8_externals.sm_battle_sound, 0x164);
	ff8_externals.sd_music_play = (uint32_t(*)(uint32_t, char*, uint32_t))get_relative_call(ff8_externals.sdmusicplay, 0x17);
	ff8_externals.current_music_ids = (uint32_t*)get_absolute_value(uint32_t(ff8_externals.sd_music_play), 0x1AA);
	common_externals.play_wav = get_relative_call(uint32_t(ff8_externals.sd_music_play), 0x1DC);
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

	// Pause/Resume functions
	ff8_externals.sub_500900 = get_relative_call(ff8_externals.sub_47CCB0, 0x98D);
	ff8_externals.sub_501B60 = get_relative_call(ff8_externals.sub_500900, -0x2A2);
	ff8_externals.sub_46B3A0 = get_relative_call(ff8_externals.sub_501B60, 0x54);
	common_externals.pause_wav = get_relative_call(ff8_externals.sub_46B3A0, 0xF);
	common_externals.pause_midi = get_relative_call(ff8_externals.sub_46B3A0, 0x17);
	ff8_externals.sub_46B3E0 = get_relative_call(ff8_externals.sub_501B60, 0xB3);
	common_externals.restart_wav = get_relative_call(ff8_externals.sub_46B3E0, 0xF);
	common_externals.restart_midi = get_relative_call(ff8_externals.sub_46B3E0, 0x17);

	// Search DirectSoundBuffer initilization
	ff8_externals.sub_4A6680 = get_relative_call(ff8_externals.sub_47CCB0, 0xF6);
	ff8_externals.sub_4A6660 = get_absolute_value(ff8_externals.sub_4A6680, 0x36F + 1);
	ff8_externals.sub_4A3D20 = get_relative_call(ff8_externals.sub_4A6660, 0x00);
	ff8_externals.sub_4A3EE0 = get_relative_call(ff8_externals.sub_4A3D20, 0x12A);
	ff8_externals.sub_469C60 = get_relative_call(ff8_externals.sub_4A3EE0, 0x418);
	ff8_externals.sub_46DDC0 = get_relative_call(ff8_externals.sub_469C60, 0x22A);
	common_externals.directsound_buffer_flags_1 = ff8_externals.sub_46DDC0 + 0x34 - 2;

	ff8_externals.sub_5304B0 = (void (*)())get_relative_call(common_externals.update_movie_sample, 0x3D9);

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

		ff8_externals.sub_53FAC0 = get_relative_call(ff8_externals.worldmap_main_loop, 0x134);
		ff8_externals.sub_54B460 = get_relative_call(ff8_externals.sub_53FAC0, 0x5D7);

		ff8_externals.sub_549E80 = get_relative_call(ff8_externals.sub_53FAC0, 0x1D5);
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

		ff8_externals.sub_53FAC0 = get_relative_call(ff8_externals.worldmap_main_loop, 0x137);
		ff8_externals.sub_54B460 = get_relative_call(ff8_externals.sub_53FAC0, 0x5D9);

		ff8_externals.sub_549E80 = get_relative_call(ff8_externals.sub_53FAC0, 0x1D6);
		ff8_externals.sub_53BB90 = get_relative_call(ff8_externals.sub_53FAC0, 0x2D5);
		ff8_externals.sub_53E2A0 = get_relative_call(ff8_externals.sub_53BB90, 0x336);
		ff8_externals.sub_53E6B0 = get_relative_call(ff8_externals.sub_53E2A0, 0x39A);
		ff8_externals.sub_4023D0 = get_relative_call(ff8_externals.sub_53BB90, 0xAE5);
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
