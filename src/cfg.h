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

#include <string>
#include "log.h"

#define RENDERER_BACKEND_AUTO 0
#define RENDERER_BACKEND_OPENGL 1
#define RENDERER_BACKEND_DIRECT3D9 2
#define RENDERER_BACKEND_DIRECT3D11 3
#define RENDERER_BACKEND_DIRECT3D12 4
#define RENDERER_BACKEND_VULKAN 5

extern std::string mod_path;
extern bool enable_ffmpeg_videos;
extern std::string ffmpeg_video_ext;
extern bool use_external_sfx;
extern std::string external_sfx_path;
extern std::string external_sfx_ext;
extern bool use_external_music;
extern bool external_music_resume;
extern std::string external_music_path;
extern std::string external_music_ext;
extern std::string he_bios_path;
extern std::string external_voice_path;
extern std::string external_voice_ext;
extern bool enable_voice_music_fade;
extern long external_voice_music_fade_volume;
extern bool save_textures;
extern bool trace_all;
extern bool trace_renderer;
extern bool trace_movies;
extern bool trace_music;
extern bool trace_sfx;
extern bool trace_fake_dx;
extern bool trace_direct;
extern bool trace_files;
extern bool trace_loaders;
extern bool trace_lights;
extern bool trace_opcodes;
extern bool trace_voice;
extern bool vertex_log;
extern bool uniform_log;
extern bool show_renderer_backend;
extern bool show_fps;
extern bool show_stats;
extern bool show_version;
extern long window_size_x;
extern long window_size_y;
extern long internal_resolution_scale;
extern bool preserve_aspect;
extern bool fullscreen;
extern long refresh_rate;
extern bool enable_vsync;
extern bool linear_filter;
extern bool mdef_fix;
extern bool fancy_transparency;
extern long enable_antialiasing;
extern bool enable_anisotropic;
extern bool skip_frames;
extern bool ff7_more_debug;
extern bool ff8_ssigpu_debug;
extern bool show_applog;
extern bool show_missing_textures;
extern bool show_error_popup;
extern bool movie_sync_debug;
extern long renderer_backend;
extern bool renderer_debug;
extern bool create_crash_dump;
extern std::string steam_game_userdata;
extern std::string hext_patching_path;
extern std::string override_path;
extern std::string direct_mode_path;
extern std::string save_path;
extern bool enable_debug_ui;
extern long debug_ui_hotkey;
extern bool ff8_keep_game_running_in_background;
extern double speedhack_step;
extern double speedhack_max;
extern double speedhack_min;
extern bool enable_animated_textures;

void read_cfg();
