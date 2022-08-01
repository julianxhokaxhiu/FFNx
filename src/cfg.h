/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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

#pragma once

#include <string>
#include <vector>
#include "log.h"

#define RENDERER_BACKEND_AUTO 0
#define RENDERER_BACKEND_OPENGL 1
#define RENDERER_BACKEND_DIRECT3D9 2
#define RENDERER_BACKEND_DIRECT3D11 3
#define RENDERER_BACKEND_DIRECT3D12 4
#define RENDERER_BACKEND_VULKAN 5

#define FF7_LIMITER_ORIGINAL 0
#define FF7_LIMITER_DEFAULT 1
#define FF7_LIMITER_30FPS 2
#define FF7_LIMITER_60FPS 3

extern std::string mod_path;
extern std::vector<std::string> mod_ext;
extern long enable_ffmpeg_videos;
extern std::string ffmpeg_video_ext;
extern std::vector<std::string> external_movie_audio_ext;
extern bool use_external_sfx;
extern std::string external_sfx_path;
extern std::vector<std::string> external_sfx_ext;
extern bool external_sfx_always_centered;
extern bool use_external_music;
extern bool external_music_resume;
extern bool external_music_sync;
extern std::string external_music_path;
extern std::vector<std::string> external_music_ext;
extern std::string he_bios_path;
extern std::string external_voice_path;
extern std::vector<std::string> external_voice_ext;
extern std::string external_ambient_path;
extern std::vector<std::string> external_ambient_ext;
extern std::string external_lighting_path;
extern bool enable_voice_music_fade;
extern long external_voice_music_fade_volume;
extern bool enable_voice_auto_text;
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
extern bool trace_vram;
extern bool trace_lights;
extern bool trace_opcodes;
extern bool trace_voice;
extern bool trace_ambient;
extern bool trace_gamepad;
extern bool trace_achievement;
extern bool trace_battle_animation;
extern bool trace_battle_text;
extern bool vertex_log;
extern bool uniform_log;
extern bool show_renderer_backend;
extern bool show_fps;
extern bool show_stats;
extern bool show_version;
extern long window_size_x;
extern long window_size_y;
extern long internal_resolution_scale;
extern long aspect_ratio;
extern bool fullscreen;
extern bool borderless;
extern long refresh_rate;
extern bool enable_vsync;
extern bool mdef_fix;
extern long enable_antialiasing;
extern bool enable_anisotropic;
extern bool enable_lighting;
extern bool prefer_lighting_cpu_calculations;
extern bool ff7_external_opening_music;
extern bool more_debug;
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
extern bool enable_devtools;
extern long devtools_hotkey;
extern double speedhack_step;
extern double speedhack_max;
extern double speedhack_min;
extern bool enable_animated_textures;
extern std::vector<std::string> disable_animated_textures_on_field;
extern long ff7_fps_limiter;
extern bool ff7_footsteps;
extern bool enable_analogue_controls;
extern bool enable_inverted_camera_controls;
extern double right_analog_stick_deadzone;
extern double left_analog_trigger_deadzone;
extern double right_analog_trigger_deadzone;
extern std::string external_vibrate_path;
extern bool enable_steam_achievements;
extern bool steam_achievements_debug_mode;
extern double hdr_max_nits;
extern long external_audio_number_of_channels;
extern long external_audio_sample_rate;
extern bool ff8_worldmap_internal_highres_textures;
extern std::string app_path;
extern std::string data_drive;

void read_cfg();
