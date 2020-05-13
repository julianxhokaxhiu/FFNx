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
 * cfg.h - configuration variable definitions
 */

#pragma once

#include <confuse.h>

#include "types.h"
#include "log.h"

extern char *mod_path;
extern cfg_bool_t use_external_movie;
extern char* external_movie_ext;
extern cfg_bool_t use_external_music;
extern char* external_music_path;
extern char* external_music_ext;
extern char* winamp_in_plugin;
extern char* winamp_out_plugin;
extern cfg_bool_t save_textures;
extern cfg_bool_t trace_all;
extern cfg_bool_t trace_movies;
extern cfg_bool_t trace_music;
extern cfg_bool_t trace_fake_dx;
extern cfg_bool_t trace_direct;
extern cfg_bool_t trace_files;
extern cfg_bool_t trace_loaders;
extern cfg_bool_t trace_lights;
extern cfg_bool_t vertex_log;
extern cfg_bool_t uniform_log;
extern cfg_bool_t show_renderer_backend;
extern cfg_bool_t show_fps;
extern cfg_bool_t show_stats;
extern cfg_bool_t show_version;
extern long window_size_x;
extern long window_size_y;
extern long internal_resolution_scale;
extern cfg_bool_t preserve_aspect;
extern cfg_bool_t fullscreen;
extern long refresh_rate;
extern cfg_bool_t enable_vsync;
extern cfg_bool_t linear_filter;
extern cfg_bool_t transparent_dialogs;
extern cfg_bool_t mdef_fix;
extern cfg_bool_t fancy_transparency;
extern long enable_antialiasing;
extern cfg_bool_t enable_anisotropic;
extern cfg_bool_t skip_frames;
extern cfg_bool_t ff7_more_debug;
extern cfg_bool_t show_applog;
extern cfg_bool_t show_missing_textures;
extern cfg_bool_t show_error_popup;
extern cfg_bool_t movie_sync_debug;
extern char *renderer_backend;
extern cfg_bool_t renderer_debug;
extern cfg_bool_t create_crash_dump;
extern char* steam_game_userdata;
extern char* hext_patching_path;
extern char* override_path;
extern char* direct_mode_path;

void read_cfg();
