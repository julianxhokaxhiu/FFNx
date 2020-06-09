/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
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

#include <Shlwapi.h>
#include <shlobj.h>
#include <psapi.h>
#include <mmsystem.h>

#include <time.h>
#include <malloc.h>
#include <stdio.h>

#define DIRECTINPUT_VERSION         0x0800 // DirectX >= 8 ( required by FF8 )
#include <dinput.h>
#include <dsound.h>

#include "matrix.h"
#include "common_imports.h"

// all known OFFICIAL versions of FF7 & FF8 released for the PC
#define VERSION_FF7_102_US          1
#define VERSION_FF7_102_FR          2
#define VERSION_FF7_102_DE          3
#define VERSION_FF7_102_SP          4
#define VERSION_FF8_12_US           5
#define VERSION_FF8_12_US_NV        6
#define VERSION_FF8_12_FR           7
#define VERSION_FF8_12_FR_NV        8
#define VERSION_FF8_12_DE           9
#define VERSION_FF8_12_DE_NV       10
#define VERSION_FF8_12_SP          11
#define VERSION_FF8_12_SP_NV       12
#define VERSION_FF8_12_IT          13
#define VERSION_FF8_12_IT_NV       14
#define VERSION_FF8_12_US_EIDOS    15
#define VERSION_FF8_12_US_EIDOS_NV 16
#define VERSION_FF8_12_JP          17

#define NV_VERSION (!(version & 1))

// FF8 does not support BLUE text!
enum
{
	TEXTCOLOR_GRAY,
	TEXTCOLOR_BLUE,
	TEXTCOLOR_RED,
	TEXTCOLOR_PINK,
	TEXTCOLOR_GREEN,
	TEXTCOLOR_LIGHT_BLUE,
	TEXTCOLOR_YELLOW,
	TEXTCOLOR_WHITE,
	NUM_TEXTCOLORS
};

enum game_modes
{
	MODE_FIELD = 0,
	MODE_BATTLE,
	MODE_WORLDMAP,
	MODE_MENU,
	MODE_HIGHWAY,
	MODE_CHOCOBO,
	MODE_SNOWBOARD,
	MODE_CONDOR,
	MODE_SUBMARINE,
	MODE_COASTER,
	MODE_CDCHECK,
	MODE_EXIT,
	MODE_SWIRL,
	MODE_GAMEOVER,
	MODE_CREDITS,
	MODE_INTRO,
	MODE_CARDGAME,
	MODE_UNKNOWN,
	MODE_AFTER_BATTLE,
};

// popup lifetime in frames
#define POPUP_TTL_MAX 10000

// dummy TEX version for framebuffer textures
#define FB_TEX_VERSION 100

struct game_mode
{
	uint mode;
	char *name;
	uint driver_mode;
	uint trace;
	uint main_loop;
};

gfx_init common_init;
gfx_cleanup common_cleanup;
gfx_lock common_lock;
gfx_unlock common_unlock;
gfx_flip common_flip;
gfx_clear common_clear;
gfx_clear_all common_clear_all;
gfx_setviewport common_setviewport;
gfx_setbg common_setbg;
gfx_prepare_polygon_set common_prepare_polygon_set;
gfx_load_group common_load_group;
gfx_setmatrix common_setmatrix;
gfx_unload_texture common_unload_texture;
gfx_load_texture common_load_texture;
gfx_palette_changed common_palette_changed;
gfx_write_palette common_write_palette;
gfx_blendmode common_blendmode;
gfx_light_polygon_set ff7gl_light_polygon_set;
gfx_field_64 common_field_64;
gfx_setrenderstate common_setrenderstate;
gfx_field_74 common_field_74;
gfx_field_78 common_field_78;
gfx_draw_deferred common_draw_deferred;
gfx_field_80 common_field_80;
gfx_field_84 common_field_84;
gfx_begin_scene common_begin_scene;
gfx_end_scene common_end_scene;
gfx_field_90 common_field_90;
gfx_polysetrenderstate common_setrenderstate_2D;
gfx_draw_vertices common_draw_2D;
gfx_draw_vertices common_draw_paletted2D;
gfx_polysetrenderstate common_setrenderstate_3D;
gfx_draw_vertices common_draw_3D;
gfx_draw_vertices common_draw_paletted3D;
gfx_draw_vertices common_draw_lines;
gfx_field_EC common_field_EC;

/* 
 * This structure holds memory addresses and function pointers of the original
 * engine used within this program. Not all of them are currently used for both
 * games, MIDI functions for example are only used for FF7 but they could
 * concievably be used for FF8 aswell.
 */

struct common_externals
{
	word *_mode;
	uint dinput_hack1;
	gfx_load_group *generic_load_group;
	gfx_light_polygon_set *generic_light_polygon_set;
	void *(*assert_free)(void *, const char *, uint);
	void *(*assert_malloc)(uint, const char *, uint);
	void *(*assert_calloc)(uint, uint, const char *, uint);
	IDirectSound **directsound;
	uint directsound_release;
	struct palette *(*create_palette_for_tex)(uint, struct tex_header *, struct texture_set *);
	struct game_obj *(*get_game_object)();
	struct texture_format *(*create_texture_format)();
	void (*add_texture_format)(struct texture_format *, struct game_obj *);
	void (*make_pixelformat)(uint, uint, uint, uint, uint, struct texture_format *);
	struct texture_set *(*create_texture_set)();
	uint debug_print;
	uint debug_print2;
	uint prepare_movie;
	uint release_movie_objects;
	uint start_movie;
	uint update_movie_sample;
	uint stop_movie;
	uint get_movie_frame;
	struct tex_header *(*create_tex_header)();
	uint get_time;
	uint midi_init;
	char *(*get_midi_name)(uint);
	uint use_midi;
	uint play_midi;
	uint stop_midi;
	uint cross_fade_midi;
	uint pause_midi;
	uint restart_midi;
	uint midi_status;
	uint set_master_midi_volume;
	uint set_midi_volume;
	uint set_midi_volume_trans;
	uint set_midi_tempo;
	uint remember_midi_playing_time;
	uint draw_graphics_object;
	char *font_info;
	uint build_dialog_window;
	uint write_file;
	uint close_file;
	uint open_file;
	uint read_file;
	uint __read_file;
	uint get_filesize;
	uint tell_file;
	uint seek_file;
	void *(*alloc_read_file)(uint, uint, struct file *);
	void *(*alloc_get_file)(struct file_context *, uint *, char *);
	void (*destroy_tex)(struct tex_header *);
	uint destroy_tex_header;
	uint start;
	uint winmain;
	uint load_tex_file;
	uint directsound_buffer_flags_1;
	uint (*play_sfx)(uint);
	uint play_sfx_on_channel;
	uint (*set_sfx_volume)(uint, uint);
	uint *master_sfx_volume;
};

// heap allocation wrappers
// driver_* functions are to be used for data internal to the driver, memory which is never allocated or free'd by the game
// external_* functions must be used for memory which could be allocated or free'd by the game
#ifndef NO_EXT_HEAP
#define external_free(x) common_externals.assert_free(x, "", 0)
#define external_malloc(x) common_externals.assert_malloc(x, "", 0)
#define external_calloc(x, y) common_externals.assert_calloc(x, y, "", 0)
#else
void ext_free(void *ptr, const char *file, uint line);
void *ext_malloc(uint size, const char *file, uint line);
void *ext_calloc(uint size, uint num, const char *file, uint line);

#define external_free(x) ext_free(x, "", 0)
#define external_malloc(x) ext_malloc(x, "", 0)
#define external_calloc(x, y) ext_calloc(x, y, "", 0)
#endif

#ifndef HEAP_DEBUG
#define driver_malloc(x) malloc(x)
#define driver_calloc(x, y) calloc(x, y)
#define driver_free(x) free(x)
#define driver_realloc(x, y) realloc(x, y)
#else
void *driver_malloc(uint size);
void *driver_calloc(uint size, uint num);
void driver_free(void *ptr);
void *driver_realloc(void *ptr, uint size);
#endif

// profiling routines
#ifdef PROFILE
#define PROFILE_START() qpc_get_time(&profile_start)
#define PROFILE_END() { qpc_get_time(&profile_end); profile_total += profile_end - profile_start; }

extern time_t profile_start;
extern time_t profile_end;
extern time_t profile_total;
#endif PROFILE

struct driver_stats
{
	uint texture_count;
	uint external_textures;
	uint ext_cache_size;
	uint texture_reloads;
	uint palette_writes;
	uint palette_changes;
	uint vertex_count;
	uint deferred;
	time_t timer;
};

void qpc_get_time(time_t *dest);
uint get_version();
struct game_mode *getmode();
struct game_mode *getmode_cached();
struct tex_header *make_framebuffer_tex(uint tex_w, uint tex_h, uint x, uint y, uint w, uint h, uint color_key);
void internal_set_renderstate(uint state, uint option, struct game_obj *game_object);

void get_data_lang_path(PCHAR buffer);
void get_userdata_path(PCHAR buffer, size_t bufSize, bool isSavegameFile);
