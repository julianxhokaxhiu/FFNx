/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2021 Julian Xhokaxhiu                                   //
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

#include "renderer.h"
#include "cfg.h"
#include "ff7.h"
#include "gl.h"
#include "patch.h"
#include "movies.h"
#include "music.h"
#include "ff7/defs.h"
#include "ff7_data.h"

unsigned char midi_fix[] = {0x8B, 0x4D, 0x14};
WORD snowboard_fix[] = {0x0F, 0x10, 0x0F};

void ff7_init_hooks(struct game_obj *_game_object)
{
	struct ff7_game_obj *game_object = (struct ff7_game_obj *)_game_object;

	common_externals.add_texture_format = game_object->externals->add_texture_format;
	common_externals.assert_calloc = game_object->externals->assert_calloc;
	common_externals.assert_malloc = game_object->externals->assert_malloc;
	common_externals.assert_free = game_object->externals->assert_free;
	common_externals.create_palette_for_tex = game_object->externals->create_palette_for_tex;
	common_externals.create_texture_format = game_object->externals->create_texture_format;
	common_externals.create_texture_set = game_object->externals->create_texture_set;
	common_externals.generic_light_polygon_set = game_object->externals->generic_light_polygon_set;
	common_externals.generic_load_group = game_object->externals->generic_load_group;
	common_externals.get_game_object = game_object->externals->get_game_object;
	ff7_externals.sub_6A2865 = game_object->externals->sub_6A2865;
	common_externals.make_pixelformat = game_object->externals->make_pixelformat;

	ff7_data(game_object);

	if (game_width == 1280)
		MessageBoxA(gameHwnd, "Using this driver with the old high-res patch is NOT recommended, there will be glitches.", "Warning", 0);

	game_object->d3d2_flag = 1;
	game_object->nvidia_fix = 0;

	// DirectInput hack, try to reacquire on any error
	memset_code(ff7_externals.dinput_getdata2 + 0x65, 0x90, 9);
	memset_code((uint32_t)common_externals.dinput_acquire_keyboard + 0x31, 0x90, 5);

	// Allow mouse cursor to be shown
	replace_function(ff7_externals.dinput_createdevice_mouse, noop);

	if (ff7_more_debug)
		replace_function(common_externals.debug_print2, external_debug_print2);

	// TODO: Comment this if Chocobo's not visible in race
	// replace_function(ff7_externals.draw_3d_model, draw_3d_model);

	// sub_6B27A9 hack, replace d3d code
	memset_code((uint32_t)ff7_externals.sub_6B27A9 + 25, 0x90, 6);
	replace_function(ff7_externals.sub_6B26C0, draw_single_triangle);
	replace_function(ff7_externals.sub_6B2720, sub_6B2720);

	// replace framebuffer access routine with our own version
	replace_function(ff7_externals.sub_673F5C, sub_673F5C);

	replace_function(ff7_externals.destroy_d3d2_indexed_primitive, destroy_d3d2_indexed_primitive);

	replace_function(ff7_externals.load_animation, load_animation);
	replace_function(ff7_externals.read_battle_hrc, read_battle_hrc);
	replace_function(common_externals.destroy_tex_header, destroy_tex_header);
	replace_function(ff7_externals.load_p_file, load_p_file);
	replace_function(common_externals.load_tex_file, load_tex_file);

	replace_function(ff7_externals.field_load_textures, field_load_textures);
	replace_function(ff7_externals.field_layer2_pick_tiles, field_layer2_pick_tiles);
	patch_code_byte(ff7_externals.field_draw_everything + 0xE2, 0x1D);
	patch_code_byte(ff7_externals.field_draw_everything + 0x353, 0x1D);
	replace_function(ff7_externals.open_flevel_siz, field_open_flevel_siz);

	replace_function(ff7_externals.get_equipment_stats, get_equipment_stats);

	replace_function(common_externals.open_file, open_file);
	replace_function(common_externals.read_file, read_file);
	replace_function(common_externals.__read_file, __read_file);
	replace_function(ff7_externals.__read, __read);
	replace_function(common_externals.write_file, write_file);
	replace_function(common_externals.close_file, close_file);
	replace_function(common_externals.get_filesize, get_filesize);
	replace_function(common_externals.tell_file, tell_file);
	replace_function(common_externals.seek_file, seek_file);
	replace_function(ff7_externals.open_lgp_file, open_lgp_file);
	replace_function(ff7_externals.lgp_chdir, lgp_chdir);
	replace_function(ff7_externals.lgp_open_file, lgp_open_file);
	replace_function(ff7_externals.lgp_read, lgp_read);
	replace_function(ff7_externals.lgp_read_file, lgp_read_file);
	replace_function(ff7_externals.lgp_get_filesize, lgp_get_filesize);
	replace_function(ff7_externals.lgp_seek_file, lgp_seek_file);

	replace_function(ff7_externals.magic_thread_start, magic_thread_start);

	replace_function(ff7_externals.kernel2_reset_counters, kernel2_reset_counters);
	replace_function(ff7_externals.kernel2_add_section, kernel2_add_section);
	replace_function(ff7_externals.kernel2_get_text, kernel2_get_text);
	patch_code_uint(ff7_externals.kernel_load_kernel2 + 0x1D, 20 * 65536);

	// prevent FF7 from stopping the movie when the window gets unfocused
	replace_function(ff7_externals.wm_activateapp, ff7_wm_activateapp);

	// required for the soft reset
	replace_function(ff7_externals.engine_exit_game_mode_sub_666C78, ff7_engine_exit_game_mode);

	// required to fix missing gameover music and broken menu sound after playing it
	replace_call_function(ff7_externals.on_gameover_enter, ff7_on_gameover_enter);
	replace_call_function(ff7_externals.on_gameover_exit, ff7_on_gameover_exit);

	// ##################################
	// bugfixes to enhance game stability
	// ##################################

	// chocobo crash fix
	memset_code(ff7_externals.chocobo_fix - 12, 0x90, 36);

	// midi transition crash fix
	memcpy_code(ff7_externals.midi_fix, midi_fix, sizeof(midi_fix));
	memset_code(ff7_externals.midi_fix + sizeof(midi_fix), 0x90, 18 - sizeof(midi_fix));

	// snowboard crash fix
	memcpy(ff7_externals.snowboard_fix, snowboard_fix, sizeof(snowboard_fix));

	// coaster aim fix
	patch_code_byte(ff7_externals.coaster_sub_5EE150 + 0x129, 5);
	patch_code_byte(ff7_externals.coaster_sub_5EE150 + 0x14A, 5);
	patch_code_byte(ff7_externals.coaster_sub_5EE150 + 0x16D, 5);
	patch_code_byte(ff7_externals.coaster_sub_5EE150 + 0x190, 5);

	// #####################
	// new timer calibration
	// #####################

	// replace time diff
	replace_function(common_externals.diff_time, qpc_diff_time);

	if (ff7_fps_limiter >= FF7_LIMITER_DEFAULT)
	{
		// replace rdtsc timing
		replace_function(common_externals.get_time, qpc_get_time);

		// override the timer calibration
		QueryPerformanceFrequency((LARGE_INTEGER *)&game_object->_countspersecond);
		game_object->countspersecond = (double)game_object->_countspersecond;

		replace_function(ff7_externals.fps_limiter_swirl, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_battle, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_coaster, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_condor, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_field, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_highway, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_snowboard, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_worldmap, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_chocobo, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_submarine, ff7_limit_fps);
		replace_function(ff7_externals.fps_limiter_credits, ff7_limit_fps);

		switch(ff7_fps_limiter)
		{
		case FF7_LIMITER_30FPS:
			*ff7_externals.battle_fps_menu_multiplier /= 2;
			break;
		case FF7_LIMITER_60FPS:
			*ff7_externals.battle_fps_menu_multiplier /= 4;
			break;
		}
	}

	// #####################
	// battle toggle
	// #####################

	replace_call_function(ff7_externals.field_battle_toggle, ff7_toggle_battle_field);
	replace_call_function(ff7_externals.worldmap_battle_toggle, ff7_toggle_battle_worldmap);

	// #####################
	// gamepad
	// #####################
	replace_function(ff7_externals.get_gamepad, ff7_get_gamepad);
	replace_function(ff7_externals.update_gamepad_status, ff7_update_gamepad_status);

	//######################
	// menu rendering fix
	//######################
	replace_call_function(ff7_externals.timer_menu_sub + 0x72F, ff7_menu_sub_6F5C0C);
	replace_call_function(ff7_externals.timer_menu_sub + 0xD77, ff7_menu_sub_6FAC38);
}

struct ff7_gfx_driver *ff7_load_driver(void* _game_object)
{
	struct ff7_gfx_driver *ret = (ff7_gfx_driver *)external_calloc(1, sizeof(*ret));

	ret->init = common_init;
	ret->cleanup = common_cleanup;
	ret->lock = common_lock;
	ret->unlock = common_unlock;
	ret->flip = common_flip;
	ret->clear = common_clear;
	ret->clear_all= common_clear_all;
	ret->setviewport = common_setviewport;
	ret->setbg = common_setbg;
	ret->prepare_polygon_set = common_prepare_polygon_set;
	ret->load_group = common_load_group;
	ret->setmatrix = common_setmatrix;
	ret->unload_texture = common_unload_texture;
	ret->load_texture = common_load_texture;
	ret->palette_changed = common_palette_changed;
	ret->write_palette = common_write_palette;
	ret->blendmode = common_blendmode;
	ret->light_polygon_set = common_externals.generic_light_polygon_set;
	ret->field_64 = common_field_64;
	ret->setrenderstate = common_setrenderstate;
	ret->_setrenderstate = common_setrenderstate;
	ret->__setrenderstate = common_setrenderstate;
	ret->field_74 = common_field_74;
	ret->field_78 = common_field_78;
	ret->draw_deferred = common_draw_deferred;
	ret->field_80 = common_field_80;
	ret->field_84 = common_field_84;
	ret->begin_scene = common_begin_scene;
	ret->end_scene = common_end_scene;
	ret->field_90 = common_field_90;
	ret->setrenderstate_flat2D = common_setrenderstate_2D;
	ret->setrenderstate_smooth2D = common_setrenderstate_2D;
	ret->setrenderstate_textured2D = common_setrenderstate_2D;
	ret->setrenderstate_paletted2D = common_setrenderstate_2D;
	ret->_setrenderstate_paletted2D = common_setrenderstate_2D;
	ret->draw_flat2D = common_draw_2D;
	ret->draw_smooth2D = common_draw_2D;
	ret->draw_textured2D = common_draw_2D;
	ret->draw_paletted2D = common_draw_paletted2D;
	ret->setrenderstate_flat3D = common_setrenderstate_3D;
	ret->setrenderstate_smooth3D = common_setrenderstate_3D;
	ret->setrenderstate_textured3D = common_setrenderstate_3D;
	ret->setrenderstate_paletted3D = common_setrenderstate_3D;
	ret->_setrenderstate_paletted3D = common_setrenderstate_3D;
	ret->draw_flat3D = common_draw_3D;
	ret->draw_smooth3D = common_draw_3D;
	ret->draw_textured3D = common_draw_3D;
	ret->draw_paletted3D = common_draw_paletted3D;
	ret->setrenderstate_flatlines = common_setrenderstate_2D;
	ret->setrenderstate_smoothlines = common_setrenderstate_2D;
	ret->draw_flatlines = common_draw_lines;
	ret->draw_smoothlines = common_draw_lines;
	ret->field_EC = common_field_EC;

	return ret;
}
