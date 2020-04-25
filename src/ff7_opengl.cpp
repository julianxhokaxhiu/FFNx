/* 
 * FFNx - Complete OpenGL replacement of the Direct3D renderer used in 
 * the original ports of Final Fantasy VII and Final Fantasy VIII for the PC.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * ff7_opengl.c - FF7 specific code and loading routine
 */

#include "compile_cfg.h"

#include "types.h"
#include "cfg.h"
#include "ff7.h"
#include "common.h"
#include "globals.h"
#include "gl.h"
#include "log.h"
#include "patch.h"
#include "movies.h"
#include "music.h"
#include "ff7/defs.h"

#include "ff7_data.h"

unsigned char midi_fix[] = {0x8B, 0x4D, 0x14};
word snowboard_fix[] = {0x0F, 0x10, 0x0F};

static uint noop() { return 0; }

struct ff7_gfx_driver *ff7_load_driver(struct ff7_game_obj *game_object)
{
	struct ff7_gfx_driver *ret;

	common_externals.add_texture_format        = game_object->externals->add_texture_format;
	common_externals.assert_calloc             = game_object->externals->assert_calloc;
	common_externals.assert_malloc             = game_object->externals->assert_malloc;
	common_externals.assert_free               = game_object->externals->assert_free;
	common_externals.create_palette_for_tex    = game_object->externals->create_palette_for_tex;
	common_externals.create_texture_format     = game_object->externals->create_texture_format;
	common_externals.create_texture_set        = game_object->externals->create_texture_set;
	common_externals.generic_light_polygon_set = game_object->externals->generic_light_polygon_set;
	common_externals.generic_load_group        = game_object->externals->generic_load_group;
	common_externals.get_game_object           = game_object->externals->get_game_object;
	ff7_externals.sub_6A2865                   = game_object->externals->sub_6A2865;
	common_externals.make_pixelformat          = game_object->externals->make_pixelformat;

	ff7_data();

	if(game_width == 1280) MessageBoxA(hwnd, "Using this driver with the old high-res patch is NOT recommended, there will be glitches.", "Warning", 0);

	game_object->d3d2_flag = 1;
	game_object->nvidia_fix = 0;

	game_object->window_title = "Final Fantasy VII";

	// DirectInput hack, try to reacquire on any error
	memset_code(ff7_externals.dinput_getdata2 + 0x65, 0x90, 9);
	memset_code(ff7_externals.dinput_getstate2 + 0x3C, 0x90, 9);
	memset_code(ff7_externals.dinput_getstate2 + 0x7E, 0x90, 5);
	memset_code(ff7_externals.dinput_acquire_keyboard + 0x31, 0x90, 5);

	replace_function(ff7_externals.dinput_createdevice_mouse, noop);

	if(more_ff7_debug) replace_function(common_externals.debug_print2, external_debug_print2);

	// TODO: Comment this if Chocobo's not visible in race
	// replace_function(ff7_externals.draw_3d_model, draw_3d_model);

	// sub_6B27A9 hack, replace d3d code
	memset_code((uint)ff7_externals.sub_6B27A9 + 25, 0x90, 6);
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

	if(transparent_dialogs) memset_code(common_externals.build_dialog_window + 0x1842, 0x90, 6);

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

	// chocobo crash fix
	memset_code(ff7_externals.chocobo_fix - 12, 0x90, 36);

	// midi transition crash fix
	memcpy_code(ff7_externals.midi_fix, midi_fix, sizeof(midi_fix));
	memset_code(ff7_externals.midi_fix + sizeof(midi_fix), 0x90, 18 - sizeof(midi_fix));

	// prevent FF7 from trying to cleanup the built-in midi player if we're going to replace it
	if(use_external_music) replace_function(ff7_externals.cleanup_midi, noop);

	// snowboard crash fix
	memcpy(ff7_externals.snowboard_fix, snowboard_fix, sizeof(snowboard_fix));

	// coaster aim fix
	patch_code_byte(ff7_externals.coaster_sub_5EE150 + 0x129, 5);
	patch_code_byte(ff7_externals.coaster_sub_5EE150 + 0x14A, 5);
	patch_code_byte(ff7_externals.coaster_sub_5EE150 + 0x16D, 5);
	patch_code_byte(ff7_externals.coaster_sub_5EE150 + 0x190, 5);

	if (ff7_center_fields)
	{
		// vertically center fields
		switch (version)
		{
		case VERSION_FF7_102_US:
			patch_code_byte(0x60D86C, 0x10);
			patch_code_byte(0x60D9CB, 0x08);
			patch_code_byte(0x640C78, 0xE8);
			patch_code_byte(0x640FD5, 0xE8);
			patch_code_byte(0x641398, 0xE8);
			patch_code_byte(0x60D8A5, 0xF0);
			break;
		case VERSION_FF7_102_FR:
		case VERSION_FF7_102_SP:
			patch_code_byte(0x60D80C, 0x10);
			patch_code_byte(0x60D96B, 0x08);
			patch_code_byte(0x640C18, 0xE8);
			patch_code_byte(0x640F75, 0xE8);
			patch_code_byte(0x641338, 0xE8);
			patch_code_byte(0x60D845, 0xF0);
			break;
		case VERSION_FF7_102_DE:
			patch_code_byte(0x60D7FC, 0x10);
			patch_code_byte(0x60D95B, 0x08);
			patch_code_byte(0x640C48, 0xE8);
			patch_code_byte(0x640FA5, 0xE8);
			patch_code_byte(0x641368, 0xE8);
			patch_code_byte(0x60D835, 0xF0);
			break;
		}
	}

	if (ff7_battle_fullscreen)
	{
		// fullscreen battles
		// Kudos to satsuki and Chrysalis for the offsets!
		switch (version)
		{
		case VERSION_FF7_102_US:
			patch_code_byte(0x41B51A, 0x70);
			patch_code_byte(0x41B4E8, 0x4C);
			patch_code_byte(0x6CF639, 0xF0);
			patch_code_byte(0x6CF6CB, 0xF0);
			patch_code_byte(0x6DD49A, 0x80);
			patch_code_byte(0x6DD44D, 0x80);
			patch_code_byte(0x6DD0BE, 0x77);
			patch_code_byte(0x6DD14F, 0x77);
			patch_code_byte(0x6DD0E0, 0x77);
			patch_code_byte(0x6DD105, 0x77);
			patch_code_byte(0x6DD12A, 0x77);
			patch_code_byte(0x6DD096, 0x77);
			patch_code_byte(0x6DD06F, 0x77);
			patch_code_byte(0x6DD3BA, 0x80);
			patch_code_byte(0x6DD539, 0x84);
			patch_code_byte(0x6DD584, 0x84);
			patch_code_byte(0x6DD5D6, 0x84);
			patch_code_byte(0x6DD603, 0x82);
			patch_code_byte(0x6DD6BD, 0x84);
			patch_code_byte(0x6DD6EB, 0x82);
			patch_code_byte(0x6DD750, 0x84);
			patch_code_byte(0x6DD78F, 0x8E);
			patch_code_byte(0x6DD7C0, 0x82);
			patch_code_byte(0x6DC991, 0x92);
			patch_code_byte(0x6DCA16, 0x92);
			patch_code_byte(0x6DD86F, 0x82);
			patch_code_byte(0x6DD8A4, 0x82);
			patch_code_byte(0x6DD8D4, 0x82);
			patch_code_byte(0x6DD956, 0x82);
			patch_code_byte(0x6DD9C3, 0x82);
			patch_code_byte(0x6DD9F8, 0x82);
			patch_code_byte(0x6DE71C, 0x78);
			patch_code_byte(0x6DE5E1, 0x78);
			patch_code_byte(0x6DE66D, 0x78);
			patch_code_byte(0x6DE6B4, 0x78);
			patch_code_byte(0x6CF934, 0x82);
			patch_code_byte(0x6DFB51, 0x7E);
			patch_code_byte(0x6DFC1C, 0x84);
			patch_code_byte(0x6DFA14, 0x76);
			patch_code_byte(0x6DFC46, 0x76);
			patch_code_byte(0x6CF977, 0x82);
			patch_code_byte(0x6E080C, 0x7E);
			patch_code_byte(0x6E0723, 0x76);
			patch_code_byte(0x6E0823, 0x74);
			patch_code_byte(0x6CF9D5, 0x82);
			patch_code_byte(0x6E0B2D, 0x7E);
			patch_code_byte(0x6E0A15, 0x76);
			patch_code_byte(0x6E0B54, 0x76);
			patch_code_byte(0x6CF8D7, 0x82);
			patch_code_byte(0x6DEECB, 0x7E);
			patch_code_byte(0x6DEC56, 0x76);
			patch_code_byte(0x6DED78, 0x7A);
			patch_code_byte(0x6DEDDE, 0x82);
			patch_code_byte(0x6DEDA3, 0x84);
			patch_code_byte(0x6DEDA9, 0xA0);
			patch_code_byte(0x6DEDE4, 0xB0);
			patch_code_byte(0x6E0A0F, 0xF6);
			patch_code_byte(0x6E0B59, 0x1A);
			patch_code_byte(0x6E2014, 0x7E);
			patch_code_byte(0x6E0E18, 0x76);
			patch_code_byte(0x6E0D79, 0x82);
			patch_code_byte(0x6E0D74, 0xA6);
			patch_code_byte(0x6E0D97, 0x82);
			patch_code_byte(0x6E0D92, 0xC2);
			patch_code_byte(0x6E0D3B, 0x79);
			patch_code_byte(0x6E0D40, 0x70);
			patch_code_byte(0x6E0DDB, 0xAA);
			patch_code_byte(0x6E0DFD, 0xC6);
			patch_code_byte(0x6E0DB3, 0x8E);
			patch_code_byte(0x6DE7BC, 0x7A);
			patch_code_byte(0x6E1891, 0x76);
			patch_code_byte(0x6E13A1, 0x82);
			patch_code_byte(0x6E18D4, 0x76);
			patch_code_byte(0x6E148C, 0x82);
			patch_code_byte(0x6E14C1, 0x82);
			patch_code_byte(0x6E14F1, 0x82);
			patch_code_byte(0x6E1535, 0x82);
			patch_code_byte(0x6E1555, 0x94);
			patch_code_byte(0x6E18FB, 0x76);
			patch_code_byte(0x6E162B, 0x82);
			patch_code_byte(0x6E1660, 0x82);
			patch_code_byte(0x6E1690, 0x82);
			patch_code_byte(0x6E16D4, 0x82);
			patch_code_byte(0x6E16F4, 0x94);
			patch_code_byte(0x6E1863, 0x82);
			patch_code_byte(0x6DF492, 0x86);
			patch_code_byte(0x6DF4BA, 0x77);
			patch_code_byte(0x6DF533, 0x73);
			patch_code_byte(0x6DF4E2, 0x77);
			patch_code_byte(0x6CF852, 0x1A);
			patch_code_byte(0x6D7B0F, 0x0F);
			patch_code_byte(0x6D7A8F, 0x0D);
			patch_code_byte(0x6E3C59, 0x9A);
			patch_code_byte(0x6E3892, 0x80);
			patch_code_byte(0x6E38AD, 0xA6);
			patch_code_byte(0x6E38F9, 0x80);
			patch_code_byte(0x6E3918, 0x9C);
			patch_code_byte(0x6E3B8D, 0x9A);
			patch_code_byte(0x6E3964, 0x76);
			patch_code_byte(0x6E3C31, 0x8A);
			patch_code_byte(0x6E3BBB, 0xAC);
			patch_code_byte(0x6E3A35, 0x08);
			patch_code_byte(0x6E316D, 0x76);
			patch_code_byte(0x6E325A, 0x58);
			patch_code_byte(0x6E2189, 0x76);
			patch_code_byte(0x6E2429, 0x18);
			patch_code_byte(0x91C342, 0x70);
			patch_code_byte(0x91C3DA, 0x70);
			patch_code_byte(0x91C63A, 0x70);
			patch_code_byte(0x91C6D2, 0x70);
			patch_code_byte(0x91C76A, 0x70);
			patch_code_byte(0x91C5A2, 0x70);
			patch_code_byte(0x91CE8A, 0x70);
			patch_code_byte(0x91CF22, 0x70);
			patch_code_byte(0x91CF38, 0x70);
			patch_code_byte(0x91D34A, 0x70);
			patch_code_byte(0x91D2B2, 0x70);
			patch_code_byte(0x91CFBA, 0x70);
			patch_code_byte(0x91D0EA, 0x40);
			patch_code_byte(0x91E7FC, 0x54);
			patch_code_byte(0x91D182, 0x70);
			patch_code_byte(0x91C50A, 0x70);
			patch_code_byte(0x91C472, 0x70);
			patch_code_byte(0x91D3E2, 0x70);
			memcpy_code(0x6D0B45, "\x90\x90\x90\x90\x90", 0x5);
			memcpy_code(0x91C5BA, "\x10\x02", 0x2);
			memcpy_code(0x91C5B6, "\x10\x02", 0x2);
			memcpy_code(0x91E990, "\x48\x6A", 0x2);
			break;
		case VERSION_FF7_102_FR:
			patch_code_byte(0x41B52A, 0x70);
			patch_code_byte(0x41B4F8, 0x4C);
			patch_code_byte(0x75A4E9, 0xF4);
			patch_code_byte(0x75A57B, 0xF4);
			patch_code_byte(0x76834A, 0x80);
			patch_code_byte(0x7682FD, 0x80);
			patch_code_byte(0x767F6E, 0x77);
			patch_code_byte(0x767FFF, 0x77);
			patch_code_byte(0x767F90, 0x77);
			patch_code_byte(0x767FB5, 0x77);
			patch_code_byte(0x767FDA, 0x77);
			patch_code_byte(0x767F46, 0x77);
			patch_code_byte(0x767F1F, 0x77);
			patch_code_byte(0x76826A, 0x80);
			patch_code_byte(0x7683E9, 0x84);
			patch_code_byte(0x768434, 0x84);
			patch_code_byte(0x768486, 0x84);
			patch_code_byte(0x7684B3, 0x82);
			patch_code_byte(0x76856D, 0x84);
			patch_code_byte(0x76859B, 0x82);
			patch_code_byte(0x768600, 0x84);
			patch_code_byte(0x76863F, 0x8E);
			patch_code_byte(0x768670, 0x82);
			patch_code_byte(0x767841, 0x92);
			patch_code_byte(0x7678C6, 0x92);
			patch_code_byte(0x76871F, 0x82);
			patch_code_byte(0x768754, 0x82);
			patch_code_byte(0x768784, 0x82);
			patch_code_byte(0x768806, 0x82);
			patch_code_byte(0x768873, 0x82);
			patch_code_byte(0x7688A8, 0x82);
			patch_code_byte(0x7695CC, 0x78);
			patch_code_byte(0x769491, 0x78);
			patch_code_byte(0x76951D, 0x78);
			patch_code_byte(0x769564, 0x78);
			patch_code_byte(0x75A7E4, 0x82);
			patch_code_byte(0x76AA01, 0x7E);
			patch_code_byte(0x76AACC, 0x84);
			patch_code_byte(0x76A8C4, 0x76);
			patch_code_byte(0x76AAF6, 0x76);
			patch_code_byte(0x75A827, 0x82);
			patch_code_byte(0x76B6BC, 0x7E);
			patch_code_byte(0x76B5D3, 0x76);
			patch_code_byte(0x76B6D3, 0x74);
			patch_code_byte(0x75A885, 0x82);
			patch_code_byte(0x76B9DD, 0x7E);
			patch_code_byte(0x76B8C5, 0x76);
			patch_code_byte(0x76BA04, 0x76);
			patch_code_byte(0x75A787, 0x82);
			patch_code_byte(0x769D7B, 0x7E);
			patch_code_byte(0x769B06, 0x76);
			patch_code_byte(0x769C28, 0x7A);
			patch_code_byte(0x769C8E, 0x82);
			patch_code_byte(0x769C53, 0x84);
			patch_code_byte(0x769C59, 0xA0);
			patch_code_byte(0x769C94, 0xB0);
			patch_code_byte(0x76B8BF, 0xF6);
			patch_code_byte(0x76BA09, 0x1A);
			patch_code_byte(0x76CEC4, 0x7E);
			patch_code_byte(0x76BCC8, 0x76);
			patch_code_byte(0x76BC29, 0x82);
			patch_code_byte(0x76BC24, 0xA6);
			patch_code_byte(0x76BC47, 0x82);
			patch_code_byte(0x76BC42, 0xC2);
			patch_code_byte(0x76BBEB, 0x79);
			patch_code_byte(0x76BBF0, 0x70);
			patch_code_byte(0x76BC8B, 0xAA);
			patch_code_byte(0x76BCAD, 0xC6);
			patch_code_byte(0x76BC63, 0x8E);
			patch_code_byte(0x76966C, 0x7A);
			patch_code_byte(0x76C741, 0x76);
			patch_code_byte(0x76C251, 0x82);
			patch_code_byte(0x76C784, 0x76);
			patch_code_byte(0x76C33C, 0x82);
			patch_code_byte(0x76C371, 0x82);
			patch_code_byte(0x76C3A1, 0x82);
			patch_code_byte(0x76C3E5, 0x82);
			patch_code_byte(0x76C405, 0x94);
			patch_code_byte(0x76C7AB, 0x76);
			patch_code_byte(0x76C4DB, 0x82);
			patch_code_byte(0x76C510, 0x82);
			patch_code_byte(0x76C540, 0x82);
			patch_code_byte(0x76C584, 0x82);
			patch_code_byte(0x76C5A4, 0x94);
			patch_code_byte(0x76C713, 0x82);
			patch_code_byte(0x7695D6, 0x68);
			patch_code_byte(0x769676, 0x68);
			patch_code_byte(0x76956E, 0x68);
			patch_code_byte(0x769527, 0x68);
			patch_code_byte(0x75A64C, 0x68);
			patch_code_byte(0x76A342, 0x86);
			patch_code_byte(0x76A36A, 0x77);
			patch_code_byte(0x76A3E3, 0x73);
			patch_code_byte(0x76A392, 0x77);
			patch_code_byte(0x75A702, 0x1A);
			patch_code_byte(0x7629BF, 0x0B);
			patch_code_byte(0x76293F, 0x0E);
			patch_code_byte(0x76EB09, 0x9A);
			patch_code_byte(0x76E742, 0x80);
			patch_code_byte(0x76E75D, 0xA6);
			patch_code_byte(0x76E7A9, 0x80);
			patch_code_byte(0x76E7C8, 0x9C);
			patch_code_byte(0x76EA3D, 0x9A);
			patch_code_byte(0x76E814, 0x76);
			patch_code_byte(0x76EAE1, 0x8A);
			patch_code_byte(0x76EA6B, 0xAC);
			patch_code_byte(0x76E8E5, 0x08);
			patch_code_byte(0x76E01D, 0x76);
			patch_code_byte(0x76E10A, 0x58);
			patch_code_byte(0x76D039, 0x76);
			patch_code_byte(0x76D2D9, 0x18);
			patch_code_byte(0x99313A, 0x70);
			patch_code_byte(0x9931D2, 0x70);
			patch_code_byte(0x993432, 0x70);
			patch_code_byte(0x9934CA, 0x70);
			patch_code_byte(0x993562, 0x70);
			patch_code_byte(0x99339A, 0x70);
			patch_code_byte(0x993C82, 0x70);
			patch_code_byte(0x993D1A, 0x70);
			patch_code_byte(0x993D30, 0x70);
			patch_code_byte(0x994142, 0x70);
			patch_code_byte(0x9940AA, 0x70);
			patch_code_byte(0x993DB2, 0x70);
			patch_code_byte(0x993EE2, 0x40);
			patch_code_byte(0x9955F4, 0x54);
			patch_code_byte(0x9955F0, 0x82);
			patch_code_byte(0x9955F2, 0xEF);
			patch_code_byte(0x9955F4, 0x54);
			patch_code_byte(0x9955F6, 0xB0);
			patch_code_byte(0x99327E, 0x70);
			patch_code_byte(0x993F7A, 0x70);
			patch_code_byte(0x993302, 0x70);
			patch_code_byte(0x99326A, 0x70);
			patch_code_byte(0x993316, 0x50);
			patch_code_byte(0x9941DA, 0x70);
			memcpy_code(0x75B9F5, "\x90\x90\x90\x90\x90", 0x5);
			memcpy_code(0x9933B2, "\x10\x02", 0x2);
			memcpy_code(0x9933AE, "\x10\x02", 0x2);
			memcpy_code(0x9958C8, "\x48\x6A", 0x2);
			break;
		case VERSION_FF7_102_DE:
			patch_code_byte(0x41B51A, 0x70);
			patch_code_byte(0x41B4E8, 0x4C);
			patch_code_byte(0x75A219, 0xF4);
			patch_code_byte(0x75A2AB, 0xF4);
			patch_code_byte(0x75A514, 0x82);
			patch_code_byte(0x75A557, 0x82);
			patch_code_byte(0x75A5B5, 0x82);
			patch_code_byte(0x75A4B7, 0x82);
			patch_code_byte(0x75A37C, 0x68);
			patch_code_byte(0x75A432, 0x1A);
			patch_code_byte(0x76806A, 0x80);
			patch_code_byte(0x76801D, 0x80);
			patch_code_byte(0x767C8E, 0x77);
			patch_code_byte(0x767D1F, 0x77);
			patch_code_byte(0x767CB0, 0x77);
			patch_code_byte(0x767CD5, 0x77);
			patch_code_byte(0x767CFA, 0x77);
			patch_code_byte(0x767C66, 0x77);
			patch_code_byte(0x767C3F, 0x77);
			patch_code_byte(0x767F8A, 0x80);
			patch_code_byte(0x768109, 0x84);
			patch_code_byte(0x768154, 0x84);
			patch_code_byte(0x7681A6, 0x84);
			patch_code_byte(0x7681D3, 0x82);
			patch_code_byte(0x76828D, 0x84);
			patch_code_byte(0x7682BB, 0x82);
			patch_code_byte(0x768320, 0x84);
			patch_code_byte(0x76835F, 0x8E);
			patch_code_byte(0x768390, 0x82);
			patch_code_byte(0x767561, 0x92);
			patch_code_byte(0x7675E6, 0x92);
			patch_code_byte(0x76843F, 0x82);
			patch_code_byte(0x768474, 0x82);
			patch_code_byte(0x7684A4, 0x82);
			patch_code_byte(0x768526, 0x82);
			patch_code_byte(0x768593, 0x82);
			patch_code_byte(0x7685C8, 0x82);
			patch_code_byte(0x7692EC, 0x78);
			patch_code_byte(0x7691B1, 0x78);
			patch_code_byte(0x76923D, 0x78);
			patch_code_byte(0x769284, 0x78);
			patch_code_byte(0x76A721, 0x7E);
			patch_code_byte(0x76A7EC, 0x84);
			patch_code_byte(0x76A5E4, 0x76);
			patch_code_byte(0x76A816, 0x76);
			patch_code_byte(0x76B3DC, 0x7E);
			patch_code_byte(0x76B2F3, 0x76);
			patch_code_byte(0x76B3F3, 0x74);
			patch_code_byte(0x76B6FD, 0x7E);
			patch_code_byte(0x76B5E5, 0x76);
			patch_code_byte(0x76B724, 0x76);
			patch_code_byte(0x769A9B, 0x7E);
			patch_code_byte(0x769826, 0x76);
			patch_code_byte(0x769948, 0x7A);
			patch_code_byte(0x7699AE, 0x82);
			patch_code_byte(0x769973, 0x84);
			patch_code_byte(0x769979, 0xA0);
			patch_code_byte(0x7699B4, 0xB0);
			patch_code_byte(0x76B5DF, 0xF6);
			patch_code_byte(0x76B729, 0x1A);
			patch_code_byte(0x76CBE4, 0x7E);
			patch_code_byte(0x76B9E8, 0x76);
			patch_code_byte(0x76B949, 0x82);
			patch_code_byte(0x76B944, 0xA6);
			patch_code_byte(0x76B967, 0x82);
			patch_code_byte(0x76B962, 0xC2);
			patch_code_byte(0x76B90B, 0x79);
			patch_code_byte(0x76B910, 0x70);
			patch_code_byte(0x76B9AB, 0xAA);
			patch_code_byte(0x76B9CD, 0xC6);
			patch_code_byte(0x76B983, 0x8E);
			patch_code_byte(0x76938C, 0x7A);
			patch_code_byte(0x76C461, 0x76);
			patch_code_byte(0x76BF71, 0x82);
			patch_code_byte(0x76C4A4, 0x76);
			patch_code_byte(0x76C05C, 0x82);
			patch_code_byte(0x76C091, 0x82);
			patch_code_byte(0x76C0C1, 0x82);
			patch_code_byte(0x76C105, 0x82);
			patch_code_byte(0x76C125, 0x94);
			patch_code_byte(0x76C4CB, 0x76);
			patch_code_byte(0x76C1FB, 0x82);
			patch_code_byte(0x76C230, 0x82);
			patch_code_byte(0x76C260, 0x82);
			patch_code_byte(0x76C2A4, 0x82);
			patch_code_byte(0x76C2C4, 0x94);
			patch_code_byte(0x76C433, 0x82);
			patch_code_byte(0x7692F6, 0x68);
			patch_code_byte(0x769396, 0x68);
			patch_code_byte(0x76928E, 0x68);
			patch_code_byte(0x769247, 0x68);
			patch_code_byte(0x76A062, 0x86);
			patch_code_byte(0x76A08A, 0x77);
			patch_code_byte(0x76A103, 0x73);
			patch_code_byte(0x76A0B2, 0x77);
			patch_code_byte(0x7626DF, 0x0B);
			patch_code_byte(0x76265F, 0x0E);
			patch_code_byte(0x76E829, 0x9A);
			patch_code_byte(0x76E462, 0x80);
			patch_code_byte(0x76E47D, 0xA6);
			patch_code_byte(0x76E4C9, 0x80);
			patch_code_byte(0x76E4E8, 0x9C);
			patch_code_byte(0x76E75D, 0x9A);
			patch_code_byte(0x76E534, 0x76);
			patch_code_byte(0x76E801, 0x8A);
			patch_code_byte(0x76E78B, 0xAC);
			patch_code_byte(0x76E605, 0x08);
			patch_code_byte(0x76DD3D, 0x76);
			patch_code_byte(0x76DE2A, 0x58);
			patch_code_byte(0x76CD59, 0x76);
			patch_code_byte(0x76CFF9, 0x18);
			patch_code_byte(0x992DAA, 0x70);
			patch_code_byte(0x992E42, 0x70);
			patch_code_byte(0x9930A2, 0x70);
			patch_code_byte(0x99313A, 0x70);
			patch_code_byte(0x9931D2, 0x70);
			patch_code_byte(0x99300A, 0x70);
			patch_code_byte(0x9938F2, 0x70);
			patch_code_byte(0x99398A, 0x70);
			patch_code_byte(0x9939A0, 0x70);
			patch_code_byte(0x993DB2, 0x70);
			patch_code_byte(0x993D1A, 0x70);
			patch_code_byte(0x993A22, 0x70);
			patch_code_byte(0x993B52, 0x40);
			patch_code_byte(0x995264, 0x54);
			patch_code_byte(0x995260, 0x82);
			patch_code_byte(0x995262, 0xEF);
			patch_code_byte(0x995264, 0x54);
			patch_code_byte(0x995266, 0xB0);
			patch_code_byte(0x992EEE, 0x70);
			patch_code_byte(0x993BEA, 0x70);
			patch_code_byte(0x992F72, 0x70);
			patch_code_byte(0x992EDA, 0x70);
			patch_code_byte(0x992F86, 0x50);
			patch_code_byte(0x993E4A, 0x70);
			memcpy_code(0x75B722, "\x90\x90\x90\x90\x90", 0x5);
			memcpy_code(0x993022, "\x10\x02", 0x2);
			memcpy_code(0x99301E, "\x10\x02", 0x2);
			memcpy_code(0x995478, "\x48\x6A", 0x2);
			break;
		case VERSION_FF7_102_SP:
			patch_code_byte(0x41B52A, 0x70);
			patch_code_byte(0x41B4F8, 0x4C);
			patch_code_byte(0x75A5F9, 0xF4);
			patch_code_byte(0x75A68B, 0xF4);
			patch_code_byte(0x75A8F4, 0x82);
			patch_code_byte(0x75A937, 0x82);
			patch_code_byte(0x75A995, 0x82);
			patch_code_byte(0x75A897, 0x82);
			patch_code_byte(0x75A75C, 0x68);
			patch_code_byte(0x75A812, 0x1A);
			patch_code_byte(0x76845A, 0x80);
			patch_code_byte(0x76840D, 0x80);
			patch_code_byte(0x76807E, 0x77);
			patch_code_byte(0x76810F, 0x77);
			patch_code_byte(0x7680A0, 0x77);
			patch_code_byte(0x7680C5, 0x77);
			patch_code_byte(0x7680EA, 0x77);
			patch_code_byte(0x768056, 0x77);
			patch_code_byte(0x76802F, 0x77);
			patch_code_byte(0x76837A, 0x80);
			patch_code_byte(0x7684F9, 0x84);
			patch_code_byte(0x768544, 0x84);
			patch_code_byte(0x768596, 0x84);
			patch_code_byte(0x7685C3, 0x82);
			patch_code_byte(0x76867D, 0x84);
			patch_code_byte(0x7686AB, 0x82);
			patch_code_byte(0x768710, 0x84);
			patch_code_byte(0x76874F, 0x8E);
			patch_code_byte(0x768780, 0x82);
			patch_code_byte(0x767951, 0x92);
			patch_code_byte(0x7679D6, 0x92);
			patch_code_byte(0x76882F, 0x82);
			patch_code_byte(0x768864, 0x82);
			patch_code_byte(0x768894, 0x82);
			patch_code_byte(0x768916, 0x82);
			patch_code_byte(0x768983, 0x82);
			patch_code_byte(0x7689B8, 0x82);
			patch_code_byte(0x7696DC, 0x78);
			patch_code_byte(0x7695A1, 0x78);
			patch_code_byte(0x76962D, 0x78);
			patch_code_byte(0x769674, 0x78);
			patch_code_byte(0x769E8B, 0x7E);
			patch_code_byte(0x769C16, 0x76);
			patch_code_byte(0x769D38, 0x7A);
			patch_code_byte(0x769D9E, 0x82);
			patch_code_byte(0x769D63, 0x84);
			patch_code_byte(0x769D69, 0xA0);
			patch_code_byte(0x769DA4, 0xB0);
			patch_code_byte(0x76AB11, 0x7E);
			patch_code_byte(0x76ABDC, 0x84);
			patch_code_byte(0x76A9D4, 0x76);
			patch_code_byte(0x76AC06, 0x76);
			patch_code_byte(0x76977C, 0x7A);
			patch_code_byte(0x7696E6, 0x68);
			patch_code_byte(0x769786, 0x68);
			patch_code_byte(0x76967E, 0x68);
			patch_code_byte(0x769637, 0x68);
			patch_code_byte(0x76A452, 0x86);
			patch_code_byte(0x76A47A, 0x77);
			patch_code_byte(0x76A4F3, 0x73);
			patch_code_byte(0x76A4A2, 0x77);
			patch_code_byte(0x762ACF, 0x0B);
			patch_code_byte(0x762A4F, 0x0E);
			patch_code_byte(0x76B7CC, 0x7E);
			patch_code_byte(0x76B6E3, 0x76);
			patch_code_byte(0x76B7E3, 0x74);
			patch_code_byte(0x76BAED, 0x7E);
			patch_code_byte(0x76B9D5, 0x76);
			patch_code_byte(0x76BB14, 0x76);
			patch_code_byte(0x76B9CF, 0xF6);
			patch_code_byte(0x76BB19, 0x1A);
			patch_code_byte(0x76CFD4, 0x7E);
			patch_code_byte(0x76BDD8, 0x76);
			patch_code_byte(0x76BD39, 0x82);
			patch_code_byte(0x76BD34, 0xA6);
			patch_code_byte(0x76BD57, 0x82);
			patch_code_byte(0x76BD52, 0xC2);
			patch_code_byte(0x76BCFB, 0x79);
			patch_code_byte(0x76BD00, 0x70);
			patch_code_byte(0x76BD9B, 0xAA);
			patch_code_byte(0x76BDBD, 0xC6);
			patch_code_byte(0x76BD73, 0x8E);
			patch_code_byte(0x76C851, 0x76);
			patch_code_byte(0x76C361, 0x82);
			patch_code_byte(0x76C894, 0x76);
			patch_code_byte(0x76C44C, 0x82);
			patch_code_byte(0x76C481, 0x82);
			patch_code_byte(0x76C4B1, 0x82);
			patch_code_byte(0x76C4F5, 0x82);
			patch_code_byte(0x76C515, 0x94);
			patch_code_byte(0x76C8BB, 0x76);
			patch_code_byte(0x76C5EB, 0x82);
			patch_code_byte(0x76C620, 0x82);
			patch_code_byte(0x76C650, 0x82);
			patch_code_byte(0x76C694, 0x82);
			patch_code_byte(0x76C6B4, 0x94);
			patch_code_byte(0x76C823, 0x82);
			patch_code_byte(0x76EC19, 0x9A);
			patch_code_byte(0x76E852, 0x80);
			patch_code_byte(0x76E86D, 0xA6);
			patch_code_byte(0x76E8B9, 0x80);
			patch_code_byte(0x76E8D8, 0x9C);
			patch_code_byte(0x76EB4D, 0x9A);
			patch_code_byte(0x76E924, 0x76);
			patch_code_byte(0x76EBF1, 0x8A);
			patch_code_byte(0x76EB7B, 0xAC);
			patch_code_byte(0x76E9F5, 0x08);
			patch_code_byte(0x76E12D, 0x76);
			patch_code_byte(0x76E21A, 0x58);
			patch_code_byte(0x76D149, 0x76);
			patch_code_byte(0x76D3E9, 0x18);
			patch_code_byte(0x99402A, 0x70);
			patch_code_byte(0x9940C2, 0x70);
			patch_code_byte(0x994322, 0x70);
			patch_code_byte(0x9943BA, 0x70);
			patch_code_byte(0x994452, 0x70);
			patch_code_byte(0x99428A, 0x70);
			patch_code_byte(0x994B72, 0x70);
			patch_code_byte(0x994C0A, 0x70);
			patch_code_byte(0x994C20, 0x70);
			patch_code_byte(0x995032, 0x70);
			patch_code_byte(0x994F9A, 0x70);
			patch_code_byte(0x994CA2, 0x70);
			patch_code_byte(0x994DD2, 0x40);
			patch_code_byte(0x9964E4, 0x54);
			patch_code_byte(0x9964E0, 0x82);
			patch_code_byte(0x9964E2, 0xEF);
			patch_code_byte(0x9964E4, 0x54);
			patch_code_byte(0x9964E6, 0xB0);
			patch_code_byte(0x99416E, 0x70);
			patch_code_byte(0x994E6A, 0x70);
			patch_code_byte(0x9941F2, 0x70);
			patch_code_byte(0x99415A, 0x70);
			patch_code_byte(0x994206, 0x50);
			patch_code_byte(0x9950CA, 0x70);
			memcpy_code(0x75BB05, "\x90\x90\x90\x90\x90", 0x5);
			memcpy_code(0x9942A2, "\x10\x02", 0x2);
			memcpy_code(0x99429E, "\x10\x02", 0x2);
			memcpy_code(0x996678, "\x48\x6A", 0x2);
			break;
		}
	}

	ret = (ff7_gfx_driver*)external_calloc(1, sizeof(*ret));

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

void ff7_post_init()
{
	movie_init();
	if(use_external_music) music_init();
}
