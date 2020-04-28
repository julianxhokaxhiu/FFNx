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
 * cfg.c - configuration file parser based on libconfuse
 */

#include <stdio.h>
#include <string.h>
#include "cfg.h"

#define FFNX_CFG_FILE "FFNx.cfg"

// configuration variables with their default values
char *mod_path = nullptr;
cfg_bool_t use_external_movie = cfg_bool_t(true);
char* external_movie_ext = nullptr;
long use_external_music = FFNX_MUSIC_VGMSTREAM;
char* external_music_path = nullptr;
char* external_music_ext = nullptr;
char* in_plugin_dll_name = nullptr;
cfg_bool_t save_textures = cfg_bool_t(false);
cfg_bool_t trace_all = cfg_bool_t(false);
cfg_bool_t trace_movies = cfg_bool_t(false);
cfg_bool_t trace_fake_dx = cfg_bool_t(false);
cfg_bool_t trace_direct = cfg_bool_t(false);
cfg_bool_t trace_files = cfg_bool_t(false);
cfg_bool_t trace_loaders = cfg_bool_t(false);
cfg_bool_t trace_lights = cfg_bool_t(false);
cfg_bool_t vertex_log = cfg_bool_t(false);
cfg_bool_t uniform_log = cfg_bool_t(false);
cfg_bool_t show_renderer_backend = cfg_bool_t(true);
cfg_bool_t show_fps = cfg_bool_t(false);
cfg_bool_t show_stats = cfg_bool_t(false);
cfg_bool_t show_version = cfg_bool_t(false);
long window_size_x = 0;
long window_size_y = 0;
long internal_resolution_scale = 4;
cfg_bool_t preserve_aspect = cfg_bool_t(true);
cfg_bool_t fullscreen = cfg_bool_t(false);
long refresh_rate = 0;
cfg_bool_t enable_vsync = cfg_bool_t(true);
cfg_bool_t linear_filter = cfg_bool_t(false);
cfg_bool_t transparent_dialogs = cfg_bool_t(false);
cfg_bool_t mdef_fix = cfg_bool_t(true);
cfg_bool_t fancy_transparency = cfg_bool_t(true);
cfg_bool_t enable_anisotropic = cfg_bool_t(true);
cfg_bool_t skip_frames = cfg_bool_t(false);
cfg_bool_t ff7_more_debug = cfg_bool_t(false);
cfg_bool_t show_applog = cfg_bool_t(true);
cfg_bool_t show_missing_textures = cfg_bool_t(false);
cfg_bool_t show_error_popup = cfg_bool_t(false);
cfg_bool_t movie_sync_debug = cfg_bool_t(false);
char *renderer_backend = nullptr;
cfg_bool_t renderer_debug = cfg_bool_t(false);
cfg_bool_t create_crash_dump = cfg_bool_t(false);
char* steam_game_userdata = nullptr;
char* hext_patching_path = nullptr;
char* override_path = nullptr;
char* direct_mode_path = nullptr;

cfg_opt_t opts[] = {
		CFG_SIMPLE_STR("mod_path", &mod_path),
		CFG_SIMPLE_BOOL("use_external_movie", &use_external_movie),
		CFG_SIMPLE_STR("external_movie_ext", &external_movie_ext),
		CFG_SIMPLE_INT("use_external_music", &use_external_music),
		CFG_SIMPLE_STR("external_music_path", &external_music_path),
		CFG_SIMPLE_STR("external_music_ext", &external_music_ext),
		CFG_SIMPLE_STR("in_plugin_dll_name", &in_plugin_dll_name),
		CFG_SIMPLE_BOOL("save_textures", &save_textures),
		CFG_SIMPLE_BOOL("trace_all", &trace_all),
		CFG_SIMPLE_BOOL("trace_movies", &trace_movies),
		CFG_SIMPLE_BOOL("trace_fake_dx", &trace_fake_dx),
		CFG_SIMPLE_BOOL("trace_direct", &trace_direct),
		CFG_SIMPLE_BOOL("trace_files", &trace_files),
		CFG_SIMPLE_BOOL("trace_loaders", &trace_loaders),
		CFG_SIMPLE_BOOL("trace_lights", &trace_lights),
		CFG_SIMPLE_BOOL("vertex_log", &vertex_log),
		CFG_SIMPLE_BOOL("uniform_log", &uniform_log),
		CFG_SIMPLE_BOOL("show_renderer_backend", &show_renderer_backend),
		CFG_SIMPLE_BOOL("show_fps", &show_fps),
		CFG_SIMPLE_BOOL("show_stats", &show_stats),
		CFG_SIMPLE_BOOL("show_version", &show_version),
		CFG_SIMPLE_INT("window_size_x", &window_size_x),
		CFG_SIMPLE_INT("window_size_y", &window_size_y),
		CFG_SIMPLE_INT("internal_resolution_scale", &internal_resolution_scale),
		CFG_SIMPLE_BOOL("preserve_aspect", &preserve_aspect),
		CFG_SIMPLE_BOOL("fullscreen", &fullscreen),
		CFG_SIMPLE_INT("refresh_rate", &refresh_rate),
		CFG_SIMPLE_BOOL("enable_vsync", &enable_vsync),
		CFG_SIMPLE_BOOL("linear_filter", &linear_filter),
		CFG_SIMPLE_BOOL("transparent_dialogs", &transparent_dialogs),
		CFG_SIMPLE_BOOL("mdef_fix", &mdef_fix),
		CFG_SIMPLE_BOOL("fancy_transparency", &fancy_transparency),
		CFG_SIMPLE_BOOL("enable_anisotropic", &enable_anisotropic),
		CFG_SIMPLE_BOOL("skip_frames", &skip_frames),
		CFG_SIMPLE_BOOL("show_applog", &show_applog),
		CFG_SIMPLE_BOOL("show_missing_textures", &show_missing_textures),
		CFG_SIMPLE_BOOL("show_error_popup", &show_error_popup),
		CFG_SIMPLE_BOOL("movie_sync_debug", &movie_sync_debug),
		CFG_SIMPLE_STR("renderer_backend", &renderer_backend),
		CFG_SIMPLE_BOOL("renderer_debug", &renderer_debug),
		CFG_SIMPLE_BOOL("create_crash_dump", &create_crash_dump),
		CFG_SIMPLE_STR("steam_game_userdata", &steam_game_userdata),
		CFG_SIMPLE_BOOL("ff7_more_debug", &ff7_more_debug),
		CFG_SIMPLE_STR("hext_patching_path", &hext_patching_path),
		CFG_SIMPLE_STR("override_path", &override_path),
		CFG_SIMPLE_STR("direct_mode_path", &direct_mode_path),

		CFG_END()
};

void error_callback(cfg_t *cfg, const char *fmt, va_list ap)
{
	char config_error_string[4096];
	char display_string[4096];

	vsnprintf(config_error_string, sizeof(config_error_string), fmt, ap);

	error("parse error in config file\n");
	error("%s\n", config_error_string);
	sprintf(display_string, "You have an error in your config file, some options may not have been parsed.\n(%s)", config_error_string);
	MessageBoxA(hwnd, display_string, "Warning", 0);
}

void read_cfg()
{
	cfg_t* cfg;

	use_external_movie = cfg_bool_t(!ff8);

	if (_access(FFNX_CFG_FILE, 0) == 0)
	{
		cfg = cfg_init(opts, 0);

		cfg_set_error_function(cfg, error_callback);

		cfg_parse(cfg, FFNX_CFG_FILE);

		cfg_free(cfg);
	}

	if (ff8)
	{
		// Reset some internal flags as they are not compatible with FF8
		fancy_transparency = cfg_bool_t(false);
	}

	// Internal scale of 1 is not allowed
	if (internal_resolution_scale < 2) internal_resolution_scale = 2;

	// #############
	// SAFE DEFAULTS
	// #############

	// HEXT PATCHING

	if (hext_patching_path == nullptr)
	{
		hext_patching_path = (char*)calloc(12, sizeof(char));
		PathAppendA(hext_patching_path, "hext");
	}

	if (ff8)
		PathAppendA(hext_patching_path, "ff8");
	else
		PathAppendA(hext_patching_path, "ff7");

	switch (version)
	{
	case VERSION_FF7_102_US:
		if (ff7_japanese_edition)
		{
			PathAppendA(hext_patching_path, "ja");
		}
		else
		{
			PathAppendA(hext_patching_path, "en");
		}
		break;
	case VERSION_FF7_102_FR:
		PathAppendA(hext_patching_path, "fr");
		break;
	case VERSION_FF7_102_DE:
		PathAppendA(hext_patching_path, "de");
		break;
	case VERSION_FF7_102_SP:
		PathAppendA(hext_patching_path, "sp");
		break;
	case VERSION_FF8_12_US:
		PathAppendA(hext_patching_path, "en");
		break;
	case VERSION_FF8_12_US_NV:
		PathAppendA(hext_patching_path, "en_nv");
		break;
	case VERSION_FF8_12_FR:
		PathAppendA(hext_patching_path, "fr");
		break;
	case VERSION_FF8_12_FR_NV:
		PathAppendA(hext_patching_path, "fr_nv");
		break;
	case VERSION_FF8_12_DE:
		PathAppendA(hext_patching_path, "de");
		break;
	case VERSION_FF8_12_DE_NV:
		PathAppendA(hext_patching_path, "de");
		break;
	case VERSION_FF8_12_SP:
		PathAppendA(hext_patching_path, "sp");
		break;
	case VERSION_FF8_12_SP_NV:
		PathAppendA(hext_patching_path, "sp_nv");
		break;
	case VERSION_FF8_12_IT:
		PathAppendA(hext_patching_path, "it");
		break;
	case VERSION_FF8_12_IT_NV:
		PathAppendA(hext_patching_path, "it_nv");
		break;
	case VERSION_FF8_12_US_EIDOS:
		PathAppendA(hext_patching_path, "en_eidos");
		break;
	case VERSION_FF8_12_US_EIDOS_NV:
		PathAppendA(hext_patching_path, "en_eidos_nv");
		break;
	case VERSION_FF8_12_JP:
		PathAppendA(hext_patching_path, "jp");
		break;
	}

	//OVERRIDE PATH
	if (override_path == nullptr)
		override_path = R"(override)";

	// DIRECT MODE PATH
	if (direct_mode_path == nullptr)
		direct_mode_path = R"(direct)";

	// RENDERER
	if (renderer_backend == nullptr)
		renderer_backend = "OpenGL";

	// EXTERNAL MOVIE EXTENSION
	if (external_movie_ext == nullptr)
		external_movie_ext = "avi";

	// EXTERNAL MUSIC EXTENSION
	if (external_music_ext == nullptr)
		external_music_ext = "ogg";

	// MOD PATH
	if (mod_path == nullptr)
		mod_path = "mods/Textures";

#ifdef SINGLE_STEP
	window_size_x = 0;
	window_size_y = 0;
	fullscreen = cfg_bool_t(false);
#endif
}
