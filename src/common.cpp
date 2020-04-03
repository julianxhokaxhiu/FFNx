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
 * common.c - common code shared between both games
 */

#include <windows.h>
#include <stdio.h>
#include <sys/timeb.h>

#include "renderer.h"

#include "types.h"
#include "log.h"
#include "crashdump.h"
#include "macro.h"
#include "common.h"
#include "ff7.h"
#include "ff8.h"
#include "patch.h"
#include "gl.h"
#include "movies.h"
#include "music.h"
#include "saveload.h"
#include "matrix.h"

// global FF7/FF8 flag, available after version check
uint ff8 = false;

uint ff8_currentdisk = 0;

// global FF7/FF8 flag, check if is steam edition
uint steam_edition = false;

// window dimensions requested by the game, normally 640x480
uint game_width;
uint game_height;

// offset from screen edge to start of content, for aspect correction
uint x_offset = 0;
uint y_offset = 0;

// device context & window handles
HDC hDC = 0;
HWND hwnd = 0;

// game-specific data, see ff7_data.h/ff8_data.h
uint text_colors[NUM_TEXTCOLORS];
struct game_mode modes[64];
uint num_modes;

// memory locations, replaced functions or patching offsets
// some addresses in FF7 are sourced from static tables in the
// externals_102_xx.h files but most of them are computed at runtime,
// see ff7_data.h/ff8_data.h
struct common_externals common_externals;
struct ff7_externals ff7_externals;
struct ff8_externals ff8_externals;

// various statistics, collected for display purposes only EXCEPT for the
// external cache size
struct driver_stats stats;

// on-screen popup messages
char popup_msg[1024];
uint popup_ttl = 0;
uint popup_color;

// scene stack used to save render state when the game calls begin/end scene
struct driver_state scene_stack[8];
uint scene_stack_pointer = 0;

// global frame counter
uint frame_counter = 0;

// default 32-bit BGRA texture format presented to the game
struct texture_format *texture_format;

// install directory for the current game
char basedir[BASEDIR_LENGTH];

uint version;

// global data used for profiling macros, see compile_cfg.h for more info
#ifdef PROFILE
time_t profile_start;
time_t profile_end;
time_t profile_total;
#endif PROFILE

// support code for the HEAP_DEBUG option, see compile_cfg.h for more info
#ifdef HEAP_DEBUG
uint allocs = 0;

void *driver_malloc(uint size)
{
	void *tmp = malloc(size);
	trace("%i: malloc(%i) = 0x%x\n", ++allocs, size, tmp);
	return tmp;
}

void *driver_calloc(uint size, uint num)
{
	void *tmp = calloc(size, num);
	trace("%i: calloc(%i, %i) = 0x%x\n", ++allocs, size, num, tmp);
	return tmp;
}

void driver_free(void *ptr)
{
	if(!ptr) return;

	trace("%i: free(0x%x)\n", --allocs, ptr);
	free(ptr);
}

void *driver_realloc(void *ptr, uint size)
{
	void *tmp = realloc(ptr, size);
	trace("%i: realloc(0x%x, %i) = 0x%x\n", allocs, ptr, size, tmp);
	return tmp;
}
#endif

// support code for the NO_EXT_HEAP option, see compile_cfg.h for more info
#ifdef NO_EXT_HEAP

void ext_free(void *ptr, const char *file, uint line)
{
	driver_free(ptr);
}

void *ext_malloc(uint size, const char *file, uint line)
{
	return driver_malloc(size);
}

void *ext_calloc(uint size, uint num, const char *file, uint line)
{
	return driver_calloc(size, num);
}
#endif

// figure out which game module is currently running by looking at the game's
// own mode variable and the address of the current main function
struct game_mode *getmode()
{
	static uint last_mode = 0;
	VOBJ(game_obj, game_object, common_externals.get_game_object());
	uint i;

	// find exact match, mode and main loop both match
	for(i = 0; i < num_modes; i++)
	{
		struct game_mode *m = &modes[i];

		if(m->main_loop == (uint)VREF(game_object, main_obj_A0C).main_loop && m->mode == *common_externals._mode)
		{
			if(last_mode != m->mode)
			{
				if(m->trace) trace("%s\n", m->name);
				last_mode = m->mode;
			}

			if (trace_all) trace("getmode: exact match - driver_mode: %u - mode: %u - name: %s\n", m->driver_mode, m->mode, m->name);

			return m;
		}
	}

	// if there is no exact match, try to find a match by main loop only
	for(i = 0; i < num_modes; i++)
	{
		struct game_mode *m = &modes[i];

		if(m->main_loop && m->main_loop == (uint)VREF(game_object, main_obj_A0C).main_loop)
		{
			if(last_mode != m->mode)
			{
				if(m->mode != *common_externals._mode && m->trace)
				{
					uint j;
					struct game_mode *_m = NULL;

					for(j = 0; j < num_modes - 1; j++)
					{
						_m = &modes[j];

						if(_m->mode == *common_externals._mode) break;
					}

					if (trace_all) trace("getmode: mismatched mode, %s -> %s\n", _m->name, m->name);
				}
				if(m->trace) trace("%s\n", m->name);
				last_mode = m->mode;
			}

			if (trace_all) trace("getmode: no exact match, found by main loop - driver_mode: %u - mode: %u - name: %s\n", m->driver_mode, m->mode, m->name);

			return m;
		}
	}

	// finally, ignore main loop and try to match by mode only
	for(i = 0; i < num_modes; i++)
	{
		struct game_mode *m = &modes[i];

		if(m->mode == *common_externals._mode)
		{
			if(last_mode != m->mode)
			{
				if(m->trace) trace("%s\n", m->name);
				last_mode = m->mode;
			}

			if (trace_all) trace("getmode: ignore main loop, match by mode only - driver_mode: %u - mode: %u - name: %s\n", m->driver_mode, m->mode, m->name);

			return m;
		}
	}

	if(*common_externals._mode != last_mode)
	{
		unexpected("unknown mode (%i, 0x%x)\n", *common_externals._mode, (uint)VREF(game_object, main_obj_A0C).main_loop);
		last_mode = *common_externals._mode;
	}

	if(!ff8) return &modes[4];
	else return &modes[11];
}

// game mode usually doesn't change in the middle of a frame and even if it
// does we usually don't care until the next frame so we can safely cache it to
// avoid constant lookups
struct game_mode *getmode_cached()
{
	static uint last_frame = -1;
	static struct game_mode *last_mode;

	if(frame_counter != last_frame) 
	{
		last_mode = getmode();
		last_frame = frame_counter;
	}

	return last_mode;
}

// called by the game before rendering starts, after the driver object has been
// created, we use this opportunity to initialize our default OpenGL render
// state
uint common_init(struct game_obj *game_object)
{
	if(trace_all) trace("dll_gfx: init\n");

	newRenderer.setBlendMode(RendererBlendMode::BLEND_NONE);

	texture_format = common_externals.create_texture_format();
	common_externals.make_pixelformat(32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000, texture_format);
	common_externals.add_texture_format(texture_format, game_object);

	return true;
}

// called by the game just before it exits, we need to make sure the game
// doesn't crash after we're gone
void common_cleanup(struct game_obj *game_object)
{
	if(trace_all) trace("dll_gfx: cleanup\n");

	if (!ff8) {
		ff7_release_movie_objects();
		if (use_external_music) music_cleanup();
	}

	gl_cleanup_deferred();

	unreplace_functions();

	newRenderer.shutdown();
}

// unused and unnecessary
uint common_lock(uint surface)
{
	if(trace_all) trace("dll_gfx: lock %i\n", surface);

	return true;
}

// unused and unnecessary
uint common_unlock(uint surface)
{
	if(trace_all) trace("dll_gfx: unlock %i\n", surface);

	return true;
}

// called by the game at the end of each frame to swap the front and back
// buffers
void common_flip(struct game_obj *game_object)
{
	if (trace_all) trace("dll_gfx: flip (%i)\n", frame_counter);

	VOBJ(game_obj, game_object, game_object);
	static time_t last_gametime;
	static struct timeb last_frame;
	static uint fps_counters[3] = {0, 0, 0};
	time_t last_seconds = last_frame.time;
	struct game_mode *mode = getmode();

	// draw any z-sorted content now that we're done drawing everything else
	gl_draw_deferred();

	if (fullscreen)
	{
		static uint col = 4;
		uint row = 1;

		if (show_version)
		{
			gl_draw_text(col, row++, text_colors[TEXTCOLOR_GRAY], 255, "VERSION: " VERSION);
		}

		if (show_renderer_backend)
		{
			gl_draw_text(col, row++, text_colors[TEXTCOLOR_GREEN], 255, "RENDERER: %s", renderer_backend);
		}

		if (show_fps)
		{
			// average last two seconds and round up for our FPS counter
			gl_draw_text(col, row++, text_colors[TEXTCOLOR_YELLOW], 255, "FPS: %2i", (fps_counters[1] + fps_counters[2] + 1) / 2);
		}

		if (show_stats)
		{
			static uint color = text_colors[TEXTCOLOR_PINK];

#ifdef HEAP_DEBUG
			gl_draw_text(col, row++, color, 255, "Allocations: %u", allocs);
#endif
#ifdef PROFILE
			gl_draw_text(col, row++, color, 255, "Profiling: %I64u us", (time_t)((profile_total * 1000000.0) / VREF(game_object, countspersecond)));
#endif
			gl_draw_text(col, row++, color, 255, "RAM usage: %uMB", get_ram_size() / (1024 * 1024));
			gl_draw_text(col, row++, color, 255, "textures: %u", stats.texture_count);
			gl_draw_text(col, row++, color, 255, "external textures: %u", stats.external_textures);
			gl_draw_text(col, row++, color, 255, "texture reloads: %u", stats.texture_reloads);
			gl_draw_text(col, row++, color, 255, "palette writes: %u", stats.palette_writes);
			gl_draw_text(col, row++, color, 255, "palette changes: %u", stats.palette_changes);
			gl_draw_text(col, row++, color, 255, "zsort layers: %u", stats.deferred);
			gl_draw_text(col, row++, color, 255, "vertices: %u", stats.vertex_count);
			gl_draw_text(col, row++, color, 255, "timer: %I64u", stats.timer);
		}
	}
	else
	{
		char newWindowTitle[1024];

		strcpy_s(newWindowTitle, 1024, VREF(game_object, window_title));

		// Append chosen rendering engine
		if (show_renderer_backend)
		{
			char tmp[64];
			sprintf_s(tmp, 64, " (%s)", renderer_backend);
			strcat_s(newWindowTitle, 1024, tmp);
		}

		if (show_version)
		{
			char tmp[16];
			sprintf_s(tmp, 16, " " VERSION);
			strcat_s(newWindowTitle, 1024, tmp);
		}

		if (show_stats)
		{
			char tmp[768];
			sprintf_s(tmp, 768, " | RAM: %u MB | nTex: %u | nExt.Tex: %u", get_ram_size() / (1024 * 1024), stats.texture_count, stats.external_textures);
			strcat_s(newWindowTitle, 1024, tmp);
		}

		if (show_fps)
		{
			char tmp[64];
			sprintf_s(tmp, 64, " | FPS: %2i", (fps_counters[1] + fps_counters[2] + 1) / 2);
			strcat_s(newWindowTitle, 1024, tmp);
		}

		SetWindowText(hwnd, newWindowTitle);
	}

	if(show_fps)
	{
		fps_counters[0]++;
		ftime(&last_frame);

		if (last_seconds != last_frame.time)
		{
			fps_counters[2] = fps_counters[1];
			fps_counters[1] = fps_counters[0];
			fps_counters[0] = 0;
		}
	}		

	// if there is an active popup message, display it
	if(popup_ttl > 0)
	{
		if(gl_draw_text(8, 75, popup_color, (popup_ttl * 255) / POPUP_TTL_MAX, popup_msg))
		{
			uint diff = (POPUP_TTL_MAX - popup_ttl) / 10;

			if(diff == 0) popup_ttl--;
			else if(diff > popup_ttl) popup_ttl = 0;
			else popup_ttl -= diff;
		}
	}

#ifdef PRERELEASE
	gl_draw_text(8, 79, text_colors[TEXTCOLOR_RED], 255, PRERELEASE_WARNING);
#endif

	// reset per-frame stats
	stats.texture_reloads = 0;
	stats.palette_writes = 0;
	stats.palette_changes = 0;
	stats.vertex_count = 0;
	stats.deferred = 0;

	newRenderer.show();

	current_state.texture_filter = true;
	current_state.fb_texture = false;

	// new framelimiter, not based on vsync
	if(!ff8 && use_new_timer)
	{
		time_t gametime;
		double framerate = mode->framerate;

		if(framerate == 0.0) framerate = 60.0;

		do qpc_get_time(&gametime); while(gametime > last_gametime && gametime - last_gametime < VREF(game_object, countspersecond * (1.0 / framerate)));

		last_gametime = gametime;
	}

	if(!fullscreen) ShowCursor(true);

	// fix unresponsive quit menu
	if(!ff8 && VREF(game_object, field_A54))
	{
		MSG msg;

		if(PeekMessageA(&msg, 0, 0, 0, 1))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	frame_counter++;

	// FF8 does not clear the screen properly in the card game module
	if(ff8 && mode->driver_mode == MODE_CARDGAME) common_clear_all(0);
}

// called by the game to clear an aspect of the back buffer, mostly called from
// clear_all below
void common_clear(uint clear_color, uint clear_depth, uint unknown, struct game_obj *game_object)
{
	uint mode = getmode()->driver_mode;

	if(trace_all) trace("dll_gfx: clear %i %i %i\n", clear_color, clear_depth, unknown);

	newRenderer.setClearFlags(
		clear_color || mode == MODE_MENU || mode == MODE_CONDOR,
		clear_depth
	);
}

// called by the game to clear the entire back buffer
void common_clear_all(struct game_obj *game_object)
{
	if(trace_all) trace("dll_gfx: clear_all\n");

	common_clear(true, true, true, game_object);
}

// called by the game to setup a viewport inside the game window, allowing it
// to clip drawing to the requested area
void common_setviewport(uint _x, uint _y, uint _w, uint _h, struct game_obj *game_object)
{
	uint mode = getmode()->driver_mode;

	if(trace_all) trace("dll_gfx: setviewport %i %i %i %i\n", _x, _y, _w, _h);

	current_state.viewport[0] = _x;
	current_state.viewport[1] = _y;
	current_state.viewport[2] = _w;
	current_state.viewport[3] = _h;

	newRenderer.setScissor(
		_x,
		_y,
		_w,
		_h
	);

	// emulate the transformation applied by an equivalent Direct3D viewport
	d3dviewport_matrix._11 = (float)_w / (float)game_width;
	// no idea why this is necessary
	if(!ff8 && mode == MODE_BATTLE) d3dviewport_matrix._22 = 1.0f;
	else d3dviewport_matrix._22 = (float)_h / (float)game_height;
	d3dviewport_matrix._41 = (((float)_x + (float)_w / 2.0f) - (float)game_width / 2.0f) / ((float)game_width / 2.0f);
	d3dviewport_matrix._42 = -(((float)_y + (float)_h / 2.0f) - (float)game_height / 2.0f) / ((float)game_height / 2.0f);
}

// called by the game to set the background color which the back buffer will be
// cleared to
void common_setbg(struct bgra_color *color, struct game_obj *game_object)
{
	if(trace_all) trace("dll_gfx: setbg\n");

	newRenderer.setBackgroundColor(
		color->r,
		color->g,
		color->b,
		ff8 ? color->a : 0.0f
	);
}

// called by the game to initialize a polygon_set structure
// we don't really need to do anything special here
uint common_prepare_polygon_set(struct polygon_set *polygon_set)
{
	VOBJ(polygon_set, polygon_set, polygon_set);

	if(VPTR(polygon_set)) VRASS(polygon_set, indexed_primitives, (indexed_primitive**)external_calloc(VREF(polygon_set, numgroups), 4));

	return true;
}

// called by the game to load a group from a .p file into a renderable format
uint common_load_group(uint group_num, struct matrix_set *matrix_set, struct p_hundred *hundred_data, struct p_group *group_data, struct polygon_data *polygon_data, struct polygon_set *polygon_set, struct game_obj *game_object)
{
	if(!ff8) return ff7gl_load_group(group_num, matrix_set, hundred_data, group_data, polygon_data, (struct ff7_polygon_set *)polygon_set, (struct ff7_game_obj *)game_object);
	else return common_externals.generic_load_group(group_num, matrix_set, hundred_data, group_data, polygon_data, polygon_set, game_object);
}

// called by the game to update one of the matrices in a matrix_set structure
void common_setmatrix(uint unknown, struct matrix *matrix, struct matrix_set *matrix_set, struct game_obj *game_object)
{
	if(trace_all) trace("dll_gfx: setmatrix\n");

	switch(unknown)
	{
		case 0:
			if(!matrix_set->matrix_world) matrix_set->matrix_world = matrix;
			else memcpy(matrix_set->matrix_world, matrix, sizeof(*matrix));
			break;

		case 1:
			if(!matrix_set->matrix_view) matrix_set->matrix_view = matrix;
			else memcpy(matrix_set->matrix_view, matrix, sizeof(*matrix));
			break;

		case 2:
			if(!matrix_set->matrix_projection) matrix_set->matrix_projection = matrix;
			else memcpy(matrix_set->matrix_projection, matrix, sizeof(*matrix));
			break;
	}
}

// called by the game to unload a texture
void common_unload_texture(struct texture_set *texture_set)
{
	uint i;
	VOBJ(texture_set, texture_set, texture_set);

	if(trace_all) trace("dll_gfx: unload_texture 0x%x\n", VPTR(texture_set));

	if(!VPTR(texture_set)) return;
	if(!VREF(texture_set, texturehandle)) return;
	if(!VREF(texture_set, ogl.gl_set)) return;

	for (uint idx = 0; idx < VREF(texture_set, ogl.gl_set->textures); idx++)
		newRenderer.deleteTexture(VREF(texture_set, texturehandle[idx]));

	external_free(VREF(texture_set, texturehandle));
	external_free(VREF(texture_set, ogl.gl_set));

	VRASS(texture_set, texturehandle, 0);
	VRASS(texture_set, ogl.gl_set, 0);

	stats.texture_count--;

	if(VREF(texture_set, ogl.external)) stats.external_textures--;

	// remove any other references to this texture
	gl_check_deferred(VPTRCAST(texture_set, texture_set));

	for(i = 0; i < scene_stack_pointer; i++)
	{
		if(scene_stack[i].texture_set == VPTR(texture_set)) scene_stack[i].texture_set = 0;
	}

	if(current_state.texture_set == VPTR(texture_set)) current_state.texture_set = 0;

	if(ff8) ff8_unload_texture(VPTRCAST(ff8_texture_set, texture_set));
}

// create a texture from an area of the framebuffer, source rectangle is encoded into tex header
// with our fictional version FB_TEXT_VERSION
// return true to short-circuit texture loader
uint load_framebuffer_texture(struct texture_set *texture_set, struct tex_header *tex_header)
{
	VOBJ(texture_set, texture_set, texture_set);
	VOBJ(tex_header, tex_header, tex_header);
	uint texture;

	if(VREF(tex_header, version) != FB_TEX_VERSION) return false;

	if(trace_all) trace("load_framebuffer_texture: XY(%u,%u) %ux%u\n", VREF(tex_header, fb_tex.x), VREF(tex_header, fb_tex.y), VREF(tex_header, fb_tex.w), VREF(tex_header, fb_tex.h));

	texture = newRenderer.blitTexture(
		VREF(tex_header, fb_tex.x),
		VREF(tex_header, fb_tex.y),
		VREF(tex_header, fb_tex.w),
		VREF(tex_header, fb_tex.h)
	);	

	VRASS(texture_set, texturehandle[0], texture);

	return true;
}

// load modpath texture for tex file, returns true if successful
uint load_external_texture(struct texture_set *texture_set, struct tex_header *tex_header)
{
	VOBJ(texture_set, texture_set, texture_set);
	VOBJ(tex_header, tex_header, tex_header);
	uint texture = 0;
	struct gl_texture_set *gl_set = VREF(texture_set, ogl.gl_set);

	if(save_textures) return false;

	if((uint)VREF(tex_header, file.pc_name) > 32)
	{
		uint use_compression = true;

		if(trace_all || trace_loaders) trace("texture file name: %s\n", VREF(tex_header, file.pc_name));

		if(!_strnicmp(VREF(tex_header, file.pc_name), "field", strlen("field") - 1)) use_compression = false;

		texture = load_texture(VREF(tex_header, file.pc_name), VREF(tex_header, palette_index), VREFP(texture_set, ogl.width), VREFP(texture_set, ogl.height), use_compression);

		if(!_strnicmp(VREF(tex_header, file.pc_name), "world", strlen("world") - 1)) gl_set->force_filter = true;

		if(!_strnicmp(VREF(tex_header, file.pc_name), "menu/usfont", strlen("menu/usfont") - 1))
		{
			gl_set->force_filter = true;
			gl_set->force_zsort = true;
		}

		if(!_strnicmp(VREF(tex_header, file.pc_name), "menu/btl_win", strlen("menu/btl_win") - 1)) gl_set->force_zsort = true;

		if(!_strnicmp(VREF(tex_header, file.pc_name), "flevel/hand_1", strlen("flevel/hand_1") - 1)) gl_set->force_filter = true;
	}

	if(texture)
	{
		gl_replace_texture(texture_set, VREF(tex_header, palette_index), texture);

		if(!VREF(texture_set, ogl.external)) stats.external_textures++;
		VRASS(texture_set, ogl.external, true);

		return true;
	}

	return false;
}

// convert a single 8-bit paletted pixel to 32-bit BGRA format
_inline uint pal2bgra(uint pixel, uint *palette, uint palette_offset, uint color_key, uint reference_alpha)
{
	if(color_key && pixel == 0) return 0;

	else
	{
		uint color = palette[palette_offset + pixel];
		// FF7 uses a form of alpha keying to emulate PSX blending
		if(BGRA_A(color) == 0xFE) color = (color & 0xFFFFFF) | reference_alpha;
		return color;
	}
}

// convert an entire image from its native format to 32-bit BGRA
void convert_image_data(unsigned char *image_data, uint *converted_image_data, uint w, uint h, struct texture_format *tex_format, uint invert_alpha, uint color_key, uint palette_offset, uint reference_alpha)
{
	uint i, j, o = 0, c = 0;

	// invalid texture in FF8, do not attempt to convert
	if(ff8 && tex_format->bytesperpixel == 0) return;

	// paletted source data (4-bit palettes are expanded to 8-bit by the game)
	if(tex_format->bytesperpixel == 1)
	{
		if(!tex_format->use_palette)
		{
			glitch("unsupported texture format\n");
			return;
		}

		for(i = 0; i < w; i++)
		{
			for(j = 0; j < h; j++)
			{
				if(image_data[o] > tex_format->palette_size)
				{
					glitch("texture conversion error\n");
					return;
				}

				converted_image_data[c++] = pal2bgra(image_data[o++], tex_format->palette_data, palette_offset, color_key, reference_alpha);
			}
		}
	}
	// RGB(A) source data
	else
	{
		if(tex_format->use_palette)
		{
			glitch("unsupported texture format\n");
			return;
		}

		for(i = 0; i < w; i++)
		{
			for(j = 0; j < h; j++)
			{
				uint pixel = 0;
				uint color = 0;

				switch(tex_format->bytesperpixel)
				{
					// 16-bit RGB(A)
					case 2:
						pixel = *((word *)(&image_data[o]));
						break;
					// 24-bit RGB
					case 3:
						pixel = image_data[o] | image_data[o + 1] << 8 | image_data[o + 2] << 16;
						break;
					// 32-bit RGBA or RGBX
					case 4:
						pixel = *((uint *)(&image_data[o]));
						break;

					default:
						glitch("unsupported texture format\n");
						return;
				}

				o += tex_format->bytesperpixel;

				// PSX style mask bit
				if(color_key && (pixel & ~tex_format->alpha_mask) == 0)
				{
					converted_image_data[c++] = 0;
					continue;
				}

				// convert source data to 8 bits per channel
				color = tex_format->blue_max > 0 ? ((((pixel & tex_format->blue_mask) >> tex_format->blue_shift) * 255) / tex_format->blue_max) : 0;
				color |= (tex_format->green_max > 0 ? ((((pixel & tex_format->green_mask) >> tex_format->green_shift) * 255) / tex_format->green_max) : 0) << 8;
				color |= (tex_format->red_max > 0 ? ((((pixel & tex_format->red_mask) >> tex_format->red_shift) * 255) / tex_format->red_max) : 0) << 16;
				
				// special case to deal with poorly converted PSX images in FF7
				if(invert_alpha && pixel != 0x8000) color |= (tex_format->alpha_max > 0 ? (255 - ((((pixel & tex_format->alpha_mask) >> tex_format->alpha_shift) * 255) / tex_format->alpha_max)) : 255) << 24;
				else color |= (tex_format->alpha_max > 0 ? ((((pixel & tex_format->alpha_mask) >> tex_format->alpha_shift) * 255) / tex_format->alpha_max) : 255) << 24;

				converted_image_data[c++] = color;
			}
		}
	}
}

// called by the game to load a texture
// can be called under a wide variety of circumstances, we must figure out what the game wants
struct texture_set *common_load_texture(struct texture_set *_texture_set, struct tex_header *_tex_header, struct texture_format *texture_format)
{
	VOBJ(game_obj, game_object, common_externals.get_game_object());
	VOBJ(texture_set, texture_set, _texture_set);
	VOBJ(tex_header, tex_header, _tex_header);
	struct palette *palette = 0;
	uint color_key = false;
	struct texture_format *tex_format = VREFP(tex_header, tex_format);

	if(trace_all) trace("dll_gfx: load_texture 0x%x\n", _texture_set);

	// no existing texture set, create one
	if(!VPTR(texture_set)) VASS(texture_set, texture_set, common_externals.create_texture_set());

	// allocate space for our private data
	if(!VREF(texture_set, ogl.gl_set)) VRASS(texture_set, ogl.gl_set, (gl_texture_set*)external_calloc(sizeof(struct gl_texture_set), 1));

	// texture handle array may not have been initialized
	if(!VREF(texture_set, texturehandle))
	{
		// allocate some more textures just in case, there could be more palettes we don't know about yet
		// FF8 likes to change its mind about just how many palettes a texture has
		VRASS(texture_set, ogl.gl_set->textures, VREF(tex_header, palettes) > 0 ? VREF(tex_header, palettes) * 2 : 1);
		VRASS(texture_set, texturehandle, (uint*)external_calloc(VREF(texture_set, ogl.gl_set->textures), sizeof(uint)));

		if(ff8 && VREF(tex_header, version) != FB_TEX_VERSION)
		{
			external_free(VREF(tex_header, old_palette_data));
			VRASS(tex_header, old_palette_data, 0);
		}

		stats.texture_count++;
	}

	// number of palettes has changed, reload the texture completely
	if(VREF(texture_set, ogl.gl_set->textures) != VREF(tex_header, palettes) * 2 && !(VREF(tex_header, palettes) == 0 && VREF(texture_set, ogl.gl_set->textures) == 1))
	{
		common_unload_texture(VPTRCAST(texture_set, texture_set));

		return common_load_texture(VPTRCAST(texture_set, texture_set), VPTRCAST(tex_header, tex_header), texture_format);
	}

	// make sure the information in the texture set is consistent
	VRASS(texture_set, tex_header, VPTRCAST(tex_header, tex_header));
	VRASS(texture_set, texture_format, texture_format);

	// check if this is suppposed to be a framebuffer texture, we may not have to do anything
	if(load_framebuffer_texture(VPTRCAST(texture_set, texture_set), VPTRCAST(tex_header, tex_header))) return VPTRCAST(texture_set, texture_set);

	// initialize palette index to a sane value if it hasn't been set
	if(VREF(tex_header, palettes) > 0)
	{
		if(VREF(texture_set, palette_index) == -1)
		{
			VRASS(tex_header, palette_index, 0);
		}
		else
		{
			VRASS(tex_header, palette_index, VREF(texture_set, palette_index));
		}
	}
	else VRASS(tex_header, palette_index, 0);

	if(VREF(tex_header, palette_index) >= VREF(texture_set, ogl.gl_set->textures))
	{
		unexpected("tried to use non-existent palette (%i, %i)\n", VREF(tex_header, palette_index), VREF(texture_set, ogl.gl_set->textures));
		VRASS(tex_header, palette_index, 0);
		return VPTRCAST(texture_set, texture_set);
	}

	// create palette structure if it doesn't exist already
	if(VREF(tex_header, palettes) > 1 && VREF(texture_set, palette) == 0) palette = common_externals.create_palette_for_tex(texture_format->bitsperpixel, VPTRCAST(tex_header, tex_header), VPTRCAST(texture_set, texture_set));

	if(tex_format->palettes == 0) tex_format->palettes = VREF(tex_header, palette_entries);

	// convert texture data from source format and load it
	if(texture_format != 0 && VREF(tex_header, image_data) != 0)
	{
		// detect changes in palette data for FF8, we can't trust it to notify us
		if(ff8 && VREF(tex_header, palettes) > 0 && VREF(tex_header, version) != FB_TEX_VERSION)
		{
			if(!VREF(tex_header, old_palette_data))
			{
				VRASS(tex_header, old_palette_data, (unsigned char*)external_malloc(4 * tex_format->palette_size));
			}

			if(memcmp(VREF(tex_header, old_palette_data), tex_format->palette_data, 4 * tex_format->palette_size))
			{
				for (uint idx = 0; idx < VREF(texture_set, ogl.gl_set->textures); idx++)
					newRenderer.deleteTexture(VREF(texture_set, texturehandle[idx]));

				memset(VREF(texture_set, texturehandle), 0, VREF(texture_set, ogl.gl_set->textures) * sizeof(uint));

				memcpy(VREF(tex_header, old_palette_data), tex_format->palette_data, 4 * tex_format->palette_size);
			}
		}

		// the texture handle for the current palette is missing, convert & load it
		if(!VREF(texture_set, texturehandle[VREF(tex_header, palette_index)]))
		{
			uint c = 0;
			uint w = VREF(tex_header, version) == FB_TEX_VERSION ? VREF(tex_header, fb_tex.w) : tex_format->width;
			uint h = VREF(tex_header, version) == FB_TEX_VERSION ? VREF(tex_header, fb_tex.h) : tex_format->height;
			uint invert_alpha = false;
			uint *image_data;
			// pre-calculate some useful data for palette conversion
			uint palette_offset = VREF(tex_header, palette_index) * VREF(tex_header, palette_entries);
			uint reference_alpha = (VREF(tex_header, reference_alpha) & 0xFF) << 24;

			// detect 16-bit PSX 5551 format with mask bit
			if(tex_format->bitsperpixel == 16 && tex_format->alpha_mask == 0x8000)
			{
				// correct incomplete texture format in FF7
				if(!ff8)
				{
					tex_format->blue_mask =  0x001F;
					tex_format->green_mask = 0x03E0;
					tex_format->red_mask =   0x7C00;
					tex_format->blue_shift =  0;
					tex_format->green_shift = 5;
					tex_format->red_shift =  10;
					tex_format->blue_max =  31;
					tex_format->green_max = 31;
					tex_format->red_max =   31;
				}

				invert_alpha = true;
			}

			// check if this texture can be loaded from the modpath, we may not have to do any conversion
			if(load_external_texture(VPTRCAST(texture_set, texture_set), VPTRCAST(tex_header, tex_header))) return VPTRCAST(texture_set, texture_set);

			// allocate PBO
			image_data = (uint*)gl_get_pixel_buffer(w * h * 4);

			if(!ff8)
			{
				// find out if color keying is enabled for this texture
				color_key = VREF(tex_header, color_key);

				// find out if color keying is enabled for this particular palette
				if(VREF(tex_header, use_palette_colorkey)) color_key = VREF(tex_header, palette_colorkey[VREF(tex_header, palette_index)]);
			}

			// convert source data
			convert_image_data(VREF(tex_header, image_data), image_data, w, h, tex_format, invert_alpha, color_key, palette_offset, reference_alpha);

			// save texture to modpath if save_textures is enabled
			if(save_textures && (uint)VREF(tex_header, file.pc_name) > 32)
			{
				if(!save_texture(image_data, w, h, VREF(tex_header, palette_index), VREF(tex_header, file.pc_name))) error("save_texture failed\n");
			}

			// commit PBO and populate texture set
			gl_upload_texture(VPTRCAST(texture_set, texture_set), VREF(tex_header, palette_index), image_data, RendererTextureType::BGRA);

			// free the memory buffer
			driver_free(image_data);
		}
		else return VPTRCAST(texture_set, texture_set);
	}
	else unexpected("no texture format specified or no source data\n");

	return VPTRCAST(texture_set, texture_set);
}

// called by the game to indicate when a texture has switched to using another palette
uint common_palette_changed(uint unknown1, uint unknown2, uint unknown3, struct palette *palette, struct texture_set *texture_set)
{
	VOBJ(texture_set, texture_set, texture_set);

	if(trace_all) trace("dll_gfx: palette_changed 0x%x %i\n", texture_set, VREF(texture_set, palette_index));

	if(palette == 0 || texture_set == 0) return false;

	// unset current texture
	newRenderer.useTexture(0);

	// texture loader logic handles missing palettes, just make sure the new palette has been loaded
	texture_set = common_load_texture(texture_set, VREF(texture_set, tex_header), VREF(texture_set, texture_format));

	// re-bind texture set to make sure the new palette is active
	gl_bind_texture_set(texture_set);

	stats.palette_changes++;

	return true;
}

// called by the game to write new color data to a palette
// sometimes called just to indicate that the palette has already been changed
// return value?
uint common_write_palette(uint source_offset, uint size, void *source, uint dest_offset, struct palette *palette, struct texture_set *texture_set)
{
	uint palette_index;
	uint palettes;
	VOBJ(texture_set, texture_set, texture_set);
	VOBJ(tex_header, tex_header, VREF(texture_set, tex_header));

	if(trace_all) trace("dll_gfx: write_palette 0x%x, %i, %i, %i, 0x%x, 0x%x\n", texture_set, source_offset, dest_offset, size, source, palette->palette_entry);

	if(palette == 0) return false;

	// if the tex header and texture set are not consistent we shouldn't be touching
	// anything before the texture is reloaded
	if(VREF(texture_set, ogl.gl_set->textures) != VREF(tex_header, palettes) * 2 && !(VREF(tex_header, palettes) == 0 && VREF(texture_set, ogl.gl_set->textures) == 1)) return true;

	palette_index = dest_offset / VREF(tex_header, palette_entries);
	palettes = size / VREF(tex_header, palette_entries);

	if(!ff8)
	{
		// FF7 writes to one palette at a time
		if(palettes > 1) unexpected("multipalette write\n");

		if(palette_index >= VREF(texture_set, ogl.gl_set->textures))
		{
			unexpected("palette write outside valid palette area (%i, %i)\n", palette_index, VREF(texture_set, ogl.gl_set->textures));
			return false;
		}

		// make sure the palette actually changed to avoid redundant texture reloads
		if(memcmp(((uint *)VREF(tex_header, tex_format.palette_data)) + dest_offset, ((uint *)source + source_offset), size * 4))
		{
			memcpy(((uint *)VREF(tex_header, tex_format.palette_data)) + dest_offset, ((uint *)source + source_offset), size * 4);

			if(!VREF(texture_set, ogl.external))
			{
				newRenderer.deleteTexture(VREF(texture_set, texturehandle[palette_index]));

				VRASS(texture_set, texturehandle[palette_index], 0);
			}

			stats.texture_reloads++;
		}
	}
	else
	{
		// FF8 writes multiple palettes in one swath but it always writes whole palettes
		if(palettes > 1 && size % VREF(tex_header, palette_entries)) unexpected("unaligned multipalette write\n");

		if(!VREF(tex_header, old_palette_data)) return false;

		// since FF8 may have already modified the palette itself we need to compare the new data to our backup
		if(memcmp(((uint *)VREF(tex_header, old_palette_data)) + dest_offset, ((uint *)source + source_offset), size * 4))
		{
			memcpy(((uint *)VREF(tex_header, old_palette_data)) + dest_offset, ((uint *)source + source_offset), size * 4);
			memcpy(((uint *)VREF(tex_header, tex_format.palette_data)) + dest_offset, ((uint *)source + source_offset), size * 4);

			// limit write to the palettes that we are aware of
			if(palette_index >= VREF(texture_set, ogl.gl_set->textures)) palettes = 0;
			else if(palette_index + palettes > VREF(texture_set, ogl.gl_set->textures)) palettes = VREF(texture_set, ogl.gl_set->textures) - palette_index;

			if(dest_offset + size > VREF(tex_header, tex_format.palette_size))
			{
				unexpected("palette write outside advertised palette area (0x%x + 0x%x, 0x%x)\n", dest_offset, size, VREF(tex_header, tex_format.palette_size));
			}

			// if there's anything left at this point, reload the affected textures
			if(palettes && !VREF(texture_set, ogl.external))
			{
				for (uint idx = 0; idx < palettes; idx++)
					newRenderer.deleteTexture(VREF(texture_set, texturehandle[palette_index]));

				memset(VREFP(texture_set, texturehandle[palette_index]), 0, palettes * sizeof(uint));
			}

			stats.texture_reloads++;
		}
	}

	// modpath textures don't have palettes, these writes are ignored
	// TODO: fancy palette cycling emulation?
	if(VREF(texture_set, ogl.external))
	{
		if((uint)VREF(tex_header, file.pc_name) > 32)
		{
			glitch_once("missed palette write to external texture %s\n", VREF(tex_header, file.pc_name));
		}
		else glitch_once("missed palette write to external texture\n");
	}

	stats.palette_writes++;

	return true;
}

// blend mode parameters, identical to Direct3D driver
struct blend_mode blend_modes[5] = {      // PSX blend mode:
	{1, 1, 0x80, 5, 0x10, 6, 0x20, 0, 0}, // average
	{1, 0, 0xFF, 2, 2,    2, 2,    0, 0}, // additive blending
	{1, 0, 0xFF, 4, 8,    2, 2,    0, 0}, // subtractive blending
	{1, 0, 0x40, 5, 0x10, 2, 2,    0, 0}, // 25%? incoming color
	{1, 0, 0xFF, 2, 2,    1, 1,    0, 0}, // 
};

// called by the game to retrieve blend mode parameters
// only z-sort and vertex alpha are really relevant to us
struct blend_mode *common_blendmode(uint unknown, struct game_obj *game_object)
{
	if(trace_all) trace("dll_gfx: blendmode %i\n", unknown);

	switch(unknown)
	{
		case 0:
			return &blend_modes[0];
		case 1:
			return &blend_modes[1];
		case 2:
			return &blend_modes[2];
		case 3:
			return &blend_modes[3];
		case 4:
			if(!ff8) unexpected("blend mode 4 requested\n");
			return &blend_modes[4];
	}

	unexpected("invalid blendmode (%i)\n", unknown);

	return 0;
}

// helper function to set simple render states (single parameter)
void internal_set_renderstate(uint state, uint option, struct game_obj *game_object)
{
	VOBJ(game_obj, game_object, game_object);

	switch(state)
	{
		// wireframe rendering, not used?
		case V_WIREFRAME:
			if (option) newRenderer.setWireframeMode(true);
			else newRenderer.setWireframeMode(false);
			current_state.wireframe = option;
			break;

		// texture filtering, can be disabled globally via config file
		case V_LINEARFILTER:
			if((option && !VREF(game_object, field_988)) && linear_filter) current_state.texture_filter = true;
			else current_state.texture_filter = false;
			break;

		// perspective correction should never be turned off
		case V_PERSPECTIVE:
			// noop
			break;

		// color keying is done when textures are converted, not when rendering
		case V_COLORKEY:
			// noop
			break;

		// no dithering necessary in 32-bit color mode
		case V_DITHER:
			// noop
			break;

		// alpha test is used in many places in FF8 instead of color keying
		case V_ALPHATEST:
			if (option) newRenderer.doAlphaTest(true);
			else newRenderer.doAlphaTest(false);
			current_state.alphatest = option;
			break;
		
		// cull face, does this ever change?
		case V_CULLFACE:
			if (option) newRenderer.setCullMode(RendererCullMode::FRONT);
			else newRenderer.setCullMode(RendererCullMode::BACK);
			current_state.cullface = option;
			break;

		// turn off culling completely, once again unsure if its ever used
		case V_NOCULL:
			if (option) newRenderer.setCullMode(RendererCullMode::DISABLED);
			else newRenderer.setCullMode(RendererCullMode::BACK);
			current_state.nocull = option;
			break;

		// turn depth testing on/off
		case V_DEPTHTEST:
			if (option) newRenderer.doDepthTest(true);
			else newRenderer.doDepthTest(false);
			current_state.depthtest = option;
			break;

		// depth mask, enable/disable writing to the Z-buffer
		case V_DEPTHMASK:
			if (option) newRenderer.doDepthWrite(true);
			else newRenderer.doDepthWrite(false);
			current_state.depthmask = option;
			break;

		// no idea what this is supposed to do
		case V_TEXADDR:
			// noop
			break;

		// function and reference values for alpha test
		case V_ALPHAFUNC:
		case V_ALPHAREF:
			if(state == V_ALPHAFUNC) current_state.alphafunc = option;
			else current_state.alpharef = option;

			switch(current_state.alphafunc)
			{
			case 0: newRenderer.setAlphaRef(RendererAlphaFunc::NEVER, current_state.alpharef / 255.0f); break;
			case 1: newRenderer.setAlphaRef(RendererAlphaFunc::ALWAYS, current_state.alpharef / 255.0f); break;
			case 2: newRenderer.setAlphaRef(RendererAlphaFunc::LESS, current_state.alpharef / 255.0f); break;
			case 3: newRenderer.setAlphaRef(RendererAlphaFunc::LEQUAL, current_state.alpharef / 255.0f); break;
			case 4: newRenderer.setAlphaRef(RendererAlphaFunc::EQUAL, current_state.alpharef / 255.0f); break;
			case 5: newRenderer.setAlphaRef(RendererAlphaFunc::GEQUAL, current_state.alpharef / 255.0f); break;
			case 6: newRenderer.setAlphaRef(RendererAlphaFunc::GREATER, current_state.alpharef / 255.0f); break;
			case 7: newRenderer.setAlphaRef(RendererAlphaFunc::NOTEQUAL, current_state.alpharef / 255.0f); break;
			default: newRenderer.setAlphaRef(RendererAlphaFunc::LEQUAL, current_state.alpharef / 255.0f); break;
			}
			break;
		default:
			break;
	}
}

// called by the game to set a simple render state
void common_field_64(uint unknown1, uint unknown2, struct game_obj *game_object)
{
	if(trace_all) trace("dll_gfx: field_64 %i %i\n", unknown1, unknown2);

	internal_set_renderstate(unknown1, unknown2, game_object);
}

// called by the game to apply a set of render states
void common_setrenderstate(struct p_hundred *hundred_data, struct game_obj *game_object)
{
	uint features;
	uint options;

	VOBJ(game_obj, game_object, game_object);

	if(hundred_data == 0) return;

	features = hundred_data->field_C;
	options = hundred_data->field_8;

	if(trace_all) trace("dll_gfx: setrenderstate 0x%x 0x%x\n", features, options);

// helper macro to check if a bit is set
// to be able to tell which bits we haven't handled, this macro will also clear
// a bit after checking it, be extremely careful not to copy/paste any
// invocation of this macro, the second invocation will not work!
#define CHECK_BIT(X, Y) ((X) & BIT((Y))) && (((X &= ~BIT((Y))) || true))

	if(CHECK_BIT(features, V_WIREFRAME)) internal_set_renderstate(V_WIREFRAME, CHECK_BIT(options, V_WIREFRAME), game_object);
	if(CHECK_BIT(features, V_TEXTURE)) gl_bind_texture_set(hundred_data->texture_set);
	if(CHECK_BIT(features, V_LINEARFILTER)) internal_set_renderstate(V_LINEARFILTER, CHECK_BIT(options, V_LINEARFILTER), game_object);
	if(CHECK_BIT(features, V_PERSPECTIVE)) internal_set_renderstate(V_PERSPECTIVE, CHECK_BIT(options, V_PERSPECTIVE), game_object);
	if(CHECK_BIT(features, V_COLORKEY)) internal_set_renderstate(V_COLORKEY, CHECK_BIT(options, V_COLORKEY), game_object);
	if(CHECK_BIT(features, V_DITHER)) internal_set_renderstate(V_DITHER, CHECK_BIT(options, V_DITHER), game_object);
	if(CHECK_BIT(features, V_ALPHABLEND))
	{
		if(CHECK_BIT(options, V_ALPHABLEND))
		{
			if(VREF(game_object, field_93C))
			{
				if(VREF(game_object, current_hundred)) gl_set_blend_func(VREF(game_object, current_hundred->blend_mode));
				else gl_set_blend_func(BLEND_NONE);
			}
			else gl_set_blend_func(hundred_data->blend_mode);
		}
		else gl_set_blend_func(BLEND_NONE);
	}
	if(CHECK_BIT(features, V_ALPHATEST)) internal_set_renderstate(V_ALPHATEST, CHECK_BIT(options, V_ALPHATEST), game_object);
	if(CHECK_BIT(features, V_CULLFACE)) internal_set_renderstate(V_CULLFACE, CHECK_BIT(options, V_CULLFACE), game_object);
	if(CHECK_BIT(features, V_NOCULL)) internal_set_renderstate(V_NOCULL, CHECK_BIT(options, V_NOCULL), game_object);
	if(CHECK_BIT(features, V_DEPTHTEST)) internal_set_renderstate(V_DEPTHTEST, CHECK_BIT(options, V_DEPTHTEST), game_object);
	if(CHECK_BIT(features, V_DEPTHMASK)) internal_set_renderstate(V_DEPTHMASK, CHECK_BIT(options, V_DEPTHMASK), game_object);
	if(CHECK_BIT(features, V_SHADEMODE))
	{
		if(CHECK_BIT(options, V_SHADEMODE) && !VREF(game_object, field_92C))
		{
			if(hundred_data->shademode == 1)
			{
				// TODO: OPENGL
				//glShadeModel(GL_FLAT);
				current_state.shademode = false;
			}
			else if(hundred_data->shademode == 2)
			{
				//glShadeModel(GL_SMOOTH);
				current_state.shademode = true;
			}
			else glitch("missing shade mode %i\n", hundred_data->shademode);
		}

		else
		{
			//glShadeModel(GL_FLAT);
			current_state.shademode = false;
		}
	}

	// any bits still set in the features and options variables at this point
	// are features that we do not currently handle
}

// called by the game to apply a predetermined set of render states
// one for each blend mode? not sure what this is used for exactly
void common_field_74(uint unknown, struct game_obj *game_object)
{
	VOBJ(game_obj, game_object, game_object);

	if(trace_all) trace("dll_gfx: field_74\n");

	if(unknown > 4) return;

	common_setrenderstate(VREF(game_object, hundred_array[unknown]), game_object);
}

// called by the game to render a polygon set
// in FF7 this is where most of the 3D rendering happens
// in FF8 this function doesn't do any rendering at all
void common_field_78(struct polygon_set *polygon_set, struct game_obj *game_object)
{
	if(!ff8) ff7gl_field_78((struct ff7_polygon_set *)polygon_set, (struct ff7_game_obj *)game_object);
	else ff8gl_field_78((struct ff8_polygon_set *)polygon_set, (struct ff8_game_obj *)game_object);
}

// called by the game to render an instance that has been deferred by the above
// function, this is a feature of the original game, not to be confused with
// our own deferred rendering!
void common_draw_deferred(struct struc_77 *struc_77, struct game_obj *game_object)
{
	VOBJ(polygon_set, polygon_set, struc_77->polygon_set);
	struct p_hundred *hundred_data = struc_77->hundred_data;
	struct indexed_primitive *ip;
	struct matrix *model_matrix = 0;

	if(trace_all) trace("dll_gfx: draw_deferred\n");

	if(!VREF(polygon_set, indexed_primitives)) return;

	ip = VREF(polygon_set, indexed_primitives[struc_77->current_group]);

	if(!ip) return;

	common_setrenderstate(hundred_data, game_object);

	if(struc_77->use_matrix) gl_set_world_matrix(&struc_77->matrix);
	if(struc_77->use_matrix_pointer) gl_set_world_matrix(struc_77->matrix_pointer);

	if(VREF(polygon_set, matrix_set)) model_matrix = VREF(polygon_set, matrix_set)->matrix_view;

	gl_draw_with_lighting(ip, VREF(polygon_set, field_4), model_matrix);
}

// called by the game to render a graphics object, basically a wrapper for
// field_78
void common_field_80(struct graphics_object *graphics_object, struct game_obj *game_object)
{
	VOBJ(graphics_object, graphics_object, graphics_object);

	if(trace_all) trace("dll_gfx: field_80\n");

	if(!VPTR(graphics_object)) return;

	common_field_78(VREF(graphics_object, polygon_set), game_object);
}

// called by the game to draw some predefined polygon sets, no idea what this
// is really used for
void common_field_84(uint unknown, struct game_obj *game_object)
{
	VOBJ(game_obj, game_object, game_object);
	VOBJ(polygon_set, polygon_set_2EC, VREF(game_object, polygon_set_2EC));
	VOBJ(polygon_set, polygon_set_2F0, VREF(game_object, polygon_set_2F0));

	if(trace_all) trace("dll_gfx: field_84\n");

	if(!VREF(game_object, in_scene)) return;

	VRASS(game_object, field_928, unknown);

	if(!unknown)
	{
		VRASS(polygon_set_2EC, field_0, true);
		VRASS(polygon_set_2F0, field_0, false);
		common_field_78(VPTRCAST(polygon_set, polygon_set_2EC), game_object);
	}

	else
	{
		VRASS(polygon_set_2EC, field_0, false);
		VRASS(polygon_set_2F0, field_0, true);
		common_field_78(VPTRCAST(polygon_set, polygon_set_2F0), game_object);
	}
}

// called by the game to setup a new scene for rendering
// scenes are not stacked in FF7
// FF8 relies on the ability to stack scenes, saving and later reverting to a previous render state
uint common_begin_scene(uint unknown, struct game_obj *game_object)
{
	VOBJ(game_obj, game_object, game_object);

	if(trace_all) trace("dll_gfx: begin_scene\n");

	if(scene_stack_pointer == sizeof(scene_stack) / sizeof(scene_stack[0])) glitch("scene stack overflow\n");
	else gl_save_state(&scene_stack[scene_stack_pointer++]);

	VRASS(game_object, in_scene, VREF(game_object, in_scene) + 1);
	
	common_field_84(unknown, game_object);

	return true;
}

// called by the game to end a scene previously setup by the above function
// render state will be restored to what it was before the scene was created
void common_end_scene(struct game_obj *game_object)
{
	VOBJ(game_obj, game_object, game_object);

	if(trace_all) trace("dll_gfx: end_scene\n");

	if(!scene_stack_pointer) glitch("scene stack underflow\n");
	else gl_load_state(&scene_stack[--scene_stack_pointer]);

	if(VREF(game_object, in_scene)) VRASS(game_object, in_scene, VREF(game_object, in_scene) - 1);
}

// noop
void common_field_90(uint unknown)
{
	glitch_once("dll_gfx: field_90 (not implemented)\n");
}

// helper function used to draw a set of triangles without palette data
void generic_draw(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object, uint vertextype)
{
	VOBJ(polygon_set, polygon_set, polygon_set);
	VOBJ(indexed_vertices, iv, iv);

	gl_draw_indexed_primitive(RendererPrimitiveType::PT_TRIANGLES, vertextype, VREF(iv, vertices), VREF(iv, vertexcount), VREF(iv, indices), VREF(iv, indexcount), UNSAFE_VREF(graphics_object, iv, graphics_object), VREF(polygon_set, field_4), true);
}

// helper function used to draw a set of triangles with palette data
void generic_draw_paletted(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object, uint vertextype)
{
	VOBJ(polygon_set, polygon_set, polygon_set);
	VOBJ(indexed_vertices, iv, iv);
	uint count = VREF(iv, count);
	unsigned char *palettes = VREF(iv, palettes);
	struct p_hundred *hundred_data = VREF(polygon_set, hundred_data);
	struct nvertex *vertices;
	struct nvertex *_vertices = VREF(iv, vertices);
	word *indices = VREF(iv, indices);

	if(!VREF(polygon_set, field_2C)) return;

	while(count > 0)
	{
		VOBJ(graphics_object, graphics_object, UNSAFE_VREF(graphics_object, iv, graphics_object));
		VOBJ(texture_set, texture_set, hundred_data->texture_set);
		uint palette_index = *palettes++;
		uint var30 = 1;
		uint vertexcount = VREF(graphics_object, vertices_per_shape);
		uint indexcount = VREF(graphics_object, indices_per_shape);

		vertices = _vertices;

		VRASS(texture_set, palette_index, palette_index);

		common_palette_changed(0, 0, 0, VREF(texture_set, palette), VPTRCAST(texture_set, texture_set));

		while(var30 < count)
		{
			if(*palettes != palette_index) break;

			palettes++;

			vertexcount += VREF(graphics_object, vertices_per_shape);
			indexcount += VREF(graphics_object, indices_per_shape);

			var30++;
		}

		_vertices = &_vertices[VREF(graphics_object, vertices_per_shape) * var30];

		count -= var30;

		gl_draw_indexed_primitive(RendererPrimitiveType::PT_TRIANGLES, vertextype, vertices, vertexcount, VREF(iv, indices), indexcount, UNSAFE_VREF(graphics_object, iv, graphics_object), VREF(polygon_set, field_4), true);
	}
}

// called by the game to set the render state for a set of 2D triangles
void common_setrenderstate_2D(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object)
{
	VOBJ(polygon_set, polygon_set, polygon_set);

	if(trace_all) trace("dll_gfx: setrenderstate_2D\n");

	if(!VREF(polygon_set, field_2C)) return;

	common_setrenderstate(VREF(polygon_set, hundred_data), game_object);
}

// called by the game to draw a set of 2D triangles without palette data
void common_draw_2D(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object)
{
	if(trace_all) trace("dll_gfx: draw_2D\n");

	generic_draw(polygon_set, iv, game_object, TLVERTEX);
}

// called by the game to draw a set of 2D triangles with palette data
void common_draw_paletted2D(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object)
{
	if(trace_all) trace("dll_gfx: draw_paletted2D\n");

	generic_draw_paletted(polygon_set, iv, game_object, TLVERTEX);
}

// called by the game to set the render state for a set of 3D triangles
void common_setrenderstate_3D(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object)
{
	VOBJ(polygon_set, polygon_set, polygon_set);
	VOBJ(indexed_vertices, iv, iv);
	VOBJ(graphics_object, graphics_object, UNSAFE_VREF(graphics_object, iv, graphics_object));

	if(trace_all) trace("dll_gfx: setrenderstate_3D\n");

	if(!VREF(polygon_set, field_2C)) return;

	common_setrenderstate(VREF(polygon_set, hundred_data), game_object);

	if(VREF(graphics_object, use_matrix_pointer)) gl_set_world_matrix(VREF(graphics_object, matrix_pointer));
	else gl_set_world_matrix(VREFP(graphics_object, matrix));
}

// called by the game to draw a set of 3D triangles without palette data
void common_draw_3D(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object)
{
	if(trace_all) trace("dll_gfx: draw_3D\n");

	generic_draw(polygon_set, iv, game_object, LVERTEX);
}

// called by the game to draw a set of 3D triangles with palette data
void common_draw_paletted3D(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object)
{
	if(trace_all) trace("dll_gfx: draw_paletted3D\n");

	generic_draw_paletted(polygon_set, iv, game_object, LVERTEX);
}

// called by the game to draw a set of lines
void common_draw_lines(struct polygon_set *polygon_set, struct indexed_vertices *iv, struct game_obj *game_object)
{
	VOBJ(polygon_set, polygon_set, polygon_set);
	VOBJ(indexed_vertices, iv, iv);

	if(trace_all) trace("dll_gfx: draw_lines\n");

	gl_draw_indexed_primitive(RendererPrimitiveType::PT_LINES, TLVERTEX, VREF(iv, vertices), VREF(iv, vertexcount), VREF(iv, indices), VREF(iv, indexcount), UNSAFE_VREF(graphics_object, iv, graphics_object), VREF(polygon_set, field_4), true);
}

// noop
void common_field_EC(struct game_obj *game_object)
{
	glitch_once("dll_gfx: field_EC (not implemented)\n");
}

// create a suitable tex header to be processed by the framebuffer texture loader
struct tex_header *make_framebuffer_tex(uint tex_w, uint tex_h, uint x, uint y, uint w, uint h, uint color_key)
{
	VOBJ(tex_header, tex_header, common_externals.create_tex_header());

	VRASS(tex_header, bpp, 32);
	VRASS(tex_header, color_key, color_key);
	memcpy(VREFP(tex_header, tex_format), texture_format, sizeof(struct texture_format));

	VRASS(tex_header, tex_format.alpha_max, 0);

	VRASS(tex_header, tex_format.width, tex_w);
	VRASS(tex_header, tex_format.height, tex_h);

	VRASS(tex_header, version, FB_TEX_VERSION);

	VRASS(tex_header, fb_tex.x, x);
	VRASS(tex_header, fb_tex.y, y);
	VRASS(tex_header, fb_tex.w, w);
	VRASS(tex_header, fb_tex.h, h);

	return VPTRCAST(tex_header, tex_header);
}

SIZE_T get_ram_size()
{
	PROCESS_MEMORY_COUNTERS_EX pmc;

	pmc.cb = sizeof(pmc);

	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	return pmc.WorkingSetSize;
}

void qpc_get_time(time_t *dest)
{
	QueryPerformanceCounter((LARGE_INTEGER *)dest);

	stats.timer = *dest;
}

// version check reads from a given offset in memory
uint version_check(uint offset)
{
	return (*(uint *)(offset));
}

// figure out if we are running in FF7 or FF8 and detect which version
uint get_version()
{
	uint version_check1 = version_check(0x401004);
	uint version_check2 = version_check(0x401404);

	trace("v1: 0x%X, v2: 0x%X\n", version_check1, version_check2);

	if(version_check1 == 0x99CE0805)
	{
		info("Auto-detected version: FF7 1.02 US English\n");
		return VERSION_FF7_102_US;
	}
	else if(version_check1 == 0x99EBF805)
	{
		info("Auto-detected version: FF7 1.02 French\n");
		return VERSION_FF7_102_FR;
	}
	else if(version_check1 == 0x99DBC805)
	{
		info("Auto-detected version: FF7 1.02 German\n");
		return VERSION_FF7_102_DE;
	}
	else if(version_check1 == 0x99F65805)
	{
		info("Auto-detected version: FF7 1.02 Spanish\n");
		return VERSION_FF7_102_SP;
	}
	else if(version_check1 == 0x3885048D && version_check2 == 0x159618)
	{
		info("Auto-detected version: FF8 1.2 US English\n");
		return VERSION_FF8_12_US;
	}
	else if(version_check1 == 0x3885048D && version_check2 == 0x1597C8)
	{
		info("Auto-detected version: FF8 1.2 US English (Nvidia)\n");
		return VERSION_FF8_12_US_NV;
	}
	else if(version_check1 == 0x1085048D && version_check2 == 0x159B48)
	{
		info("Auto-detected version: FF8 1.2 French\n");
		return VERSION_FF8_12_FR;
	}
	else if(version_check1 == 0x1085048D && version_check2 == 0x159CF8)
	{
		info("Auto-detected version: FF8 1.2 French (Nvidia)\n");
		return VERSION_FF8_12_FR_NV;
	}
	else if(version_check1 == 0xA885048D && version_check2 == 0x159C48)
	{
		info("Auto-detected version: FF8 1.2 German\n");
		return VERSION_FF8_12_DE;
	}
	else if(version_check1 == 0xA885048D && version_check2 == 0x159DF8)
	{
		info("Auto-detected version: FF8 1.2 German (Nvidia)\n");
		return VERSION_FF8_12_DE_NV;
	}
	else if(version_check1 == 0x9085048D && version_check2 == 0x159C58)
	{
		info("Auto-detected version: FF8 1.2 Spanish\n");
		return VERSION_FF8_12_SP;
	}
	else if(version_check1 == 0x8085048D && version_check2 == 0x159DE8)
	{
		info("Auto-detected version: FF8 1.2 Spanish (Nvidia)\n");
		return VERSION_FF8_12_SP_NV;
	}
	else if(version_check1 == 0xB885048D && version_check2 == 0x159BC8)
	{
		info("Auto-detected version: FF8 1.2 Italian\n");
		return VERSION_FF8_12_IT;
	}
	else if(version_check1 == 0xB885048D && version_check2 == 0x159D78)
	{
		info("Auto-detected version: FF8 1.2 Italian (Nvidia)\n");
		return VERSION_FF8_12_IT_NV;
	}
	else if(version_check1 == 0x2885048D && version_check2 == 0x159598)
	{
		info("Auto-detected version: FF8 1.2 US English (Eidos Patch)\n");
		return VERSION_FF8_12_US_EIDOS;
	}
	else if(version_check1 == 0x2885048D && version_check2 == 0x159748)
	{
		info("Auto-detected version: FF8 1.2 US English (Eidos Patch) (Nvidia)\n");
		return VERSION_FF8_12_US_EIDOS_NV;
	}
	else if(version_check1 == 0x1B6E9CC && version_check2 == 0x7C8DFFC9)
	{
		info("Auto-detected version: FF8 1.2 Japanese\n");
		return VERSION_FF8_12_JP;
	}

	return 0;
}

void get_data_lang_path(PCHAR buffer)
{
	PathAppendA(buffer, R"(data\lang-)");
	switch (version)
	{
	case VERSION_FF7_102_US:
	case VERSION_FF8_12_US_NV:
		strcat(buffer, "en");
		break;
	case VERSION_FF7_102_FR:
	case VERSION_FF8_12_FR_NV:
		strcat(buffer, "fr");
		break;
	case VERSION_FF7_102_DE:
	case VERSION_FF8_12_DE_NV:
		strcat(buffer, "de");
		break;
	case VERSION_FF7_102_SP:
	case VERSION_FF8_12_SP_NV:
		strcat(buffer, "sp");
		break;
	case VERSION_FF8_12_IT_NV:
		strcat(buffer, "it");
		break;
	case VERSION_FF8_12_JP:
		strcat(buffer, "jp");
		break;
	}
}

void get_userdata_path(PCHAR buffer, size_t bufSize, bool isSavegameFile)
{
	PWSTR outPath = NULL;

	HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, NULL, &outPath);

	if (SUCCEEDED(hr))
	{
		wcstombs(buffer, outPath, bufSize);

		CoTaskMemFree(outPath);

		if (ff8)
			PathAppendA(buffer, R"(Square Enix\FINAL FANTASY VIII Steam)");
		else
			PathAppendA(buffer, R"(Square Enix\FINAL FANTASY VII Steam)");

		if (isSavegameFile)
		{
			if (steam_game_userdata != nullptr)
			{
				// Directly use the given userdata
				PathAppendA(buffer, steam_game_userdata);
			}
			else
			{
				// Search for the first "user_" match in the game path
				CHAR searchPath[260];
				WIN32_FIND_DATA pathFound;
				HANDLE hFind;

				strcpy(searchPath, buffer);
				strcat(searchPath, R"(\user_*)");
				if (hFind = FindFirstFileA(searchPath, &pathFound))
				{
					PathAppendA(buffer, pathFound.cFileName);
					FindClose(hFind);
				}
			}
		}
	}
}

// cd check
uint ff7_get_inserted_cd(void) {
	int requiredCD = -1;

	requiredCD = *(uint8_t*)(0xDC0BDC);
	if (requiredCD == 0)
		requiredCD = 1;

	return requiredCD;
}

MCIERROR __stdcall dotemuMciSendCommandA(MCIDEVICEID mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	DWORD mciStatusRet;

	switch (uMsg)
	{
	case MCI_OPEN:
		((LPDWORD)dwParam2)[2] = (DWORD)"waveaudio";
		dwParam1 = 8704;
		((LPDWORD)dwParam2)[3] = (DWORD)R"(Data\Music\eyes_on_me.wav)";
		return mciSendCommandA(mciId, uMsg, dwParam1, dwParam2);
	case MCI_SET:
		((LPDWORD)dwParam2)[1] = 0;
		break;
	case MCI_STATUS:
		mciStatusRet = 0;
		((LPDWORD)dwParam2)[1] = (DWORD)&mciStatusRet;
		return 0;
	case MCI_PLAY:
		((LPDWORD)dwParam2)[2] = 339000;
		break;
	}

	return mciSendCommandA(mciId, uMsg, dwParam1, dwParam2);
}

#if defined(__cplusplus)
extern "C" {
#endif

// main entry point, called by the game to create a graphics driver object
__declspec(dllexport) void *new_dll_graphics_driver(void *game_object)
{
	void *ret;
	VOBJ(game_obj, game_object, game_object);
	DEVMODE dmScreenSettings;

	if(version == VERSION_FF7_102_US)
	{
		#include "externals_102_us.h"
	}
	else if(version == VERSION_FF7_102_FR)
	{
		#include "externals_102_fr.h"
	}
	else if(version == VERSION_FF7_102_DE)
	{
		#include "externals_102_de.h"
	}
	else if(version == VERSION_FF7_102_SP)
	{
		#include "externals_102_sp.h"
	}

	if(!version)
	{
		error("no compatible version found\n");
		MessageBoxA(hwnd, "Your ff7.exe or ff8.exe is incompatible with this driver and will exit after this message.\n"
			"Possible reasons for this error:\n"
			" - You have the faulty \"1.4 XP Patch\" for FF7.\n"
			" - You have FF7 retail 1.00 version (you need the 1.02 patch).\n"
			" - You have an unsupported translation of FF7. (US English, French, German and Spanish versions are currently supported)\n"
			" - You have FF8 retail 1.0 version (you need the 1.2 patch).\n"
			" - You have FF8 1.2 from Eidos (you need the newer 1.2 patch from Squaresoft).\n"
			" - You have an unsupported translation of FF8. (US English, French, German, Spanish and Italian versions are currently supported)\n"
			" - You have a conflicting patch applied.\n\n"
			, "Error", 0);
		exit(1);
	}

	// try to prevent screensavers from going off
	SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);

	// game-specific initialization
	if(!ff8)
		ret = ff7_load_driver(VPTRCAST(ff7_game_obj, game_object));
	else
		ret = ff8_load_driver(VPTRCAST(ff8_game_obj, game_object));

	// catch all applog messages
	replace_function(common_externals.debug_print, external_debug_print);

#ifdef NO_EXT_HEAP
	replace_function((uint)common_externals.assert_free, ext_free);
	replace_function((uint)common_externals.assert_malloc, ext_malloc);
	replace_function((uint)common_externals.assert_calloc, ext_calloc);
#endif

	// read original resolution
	game_width = VREF(game_object, res_w);
	game_height = VREF(game_object, res_h);

	// steal window handle
	hwnd = VREF(game_object, hwnd);

	// fetch current user screen settings
	EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dmScreenSettings);

	if(window_size_x == 0 || window_size_y == 0)
	{
		if(fullscreen)
		{
			window_size_x = dmScreenSettings.dmPelsWidth;
			window_size_y = dmScreenSettings.dmPelsHeight;
		}
		else
		{
			// default window mode is original resolution
			window_size_x = game_width;
			window_size_y = game_height;
		}
	}
	else
	{
		if (fullscreen)
		{
			dmScreenSettings.dmPelsWidth = window_size_x;
			dmScreenSettings.dmPelsHeight = window_size_y;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		}

		if(refresh_rate)
		{
			dmScreenSettings.dmDisplayFrequency = refresh_rate;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
		}
	}

	if(fullscreen)
	{
		if(ChangeDisplaySettingsEx(0, &dmScreenSettings, 0, CDS_FULLSCREEN | CDS_RESET, 0) != DISP_CHANGE_SUCCESSFUL)
		{
			MessageBoxA(hwnd, "Failed to set the requested fullscreen mode, reverting to original resolution window mode.\n", "Error", 0);
			error("failed to set fullscreen mode\n");
			fullscreen = cfg_bool_t(false);
			window_size_x = game_width;
			window_size_y = game_height;
		}

		MoveWindow(hwnd, 0, 0, window_size_x, window_size_y, false);
		SetWindowText(hwnd, VREF(game_object, window_title));
	}

	if(!fullscreen)
	{
		RECT tmp;
		uint w, h;

		tmp.left = 0;
		tmp.top = 0;
		tmp.right = window_size_x;
		tmp.bottom = window_size_y;

		uint32_t initialWindowsPositionOffsetX = (dmScreenSettings.dmPelsWidth / 2) - (window_size_x / 2);
		uint32_t initialWindowsPositionOffsetY = (dmScreenSettings.dmPelsHeight / 2) - (window_size_y / 2);

		// in windowed mode we need to create our own window with the proper decorations
		DestroyWindow(hwnd);

		if(!AdjustWindowRectEx(&tmp, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, false, 0)) windows_error(0);

		w = tmp.right - tmp.left;
		h = tmp.bottom - tmp.top;

		if(!(hwnd = CreateWindowEx(0, VREF(game_object, window_class), VREF(game_object, window_title), WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, initialWindowsPositionOffsetX, initialWindowsPositionOffsetY, w, h, 0, 0, VREF(game_object, hinstance), 0)))
		{
			error("couldn't create new window: ");
			windows_error(0);
		}

		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(VREF(game_object, hinstance), MAKEINTRESOURCE(101)));

		ShowWindow(hwnd, SW_SHOW);

		VRASS(game_object, hwnd, hwnd);
	}

	// Init renderer
	newRenderer.init();

	max_texture_size = newRenderer.getCaps()->limits.maxTextureSize;
	info("Max texture size: %ix%i\n", max_texture_size, max_texture_size);

	// aspect correction
	if(preserve_aspect && window_size_x * 3 != window_size_y * 4)
	{
		if(window_size_y * 4 > window_size_x * 3)
		{
			y_offset = window_size_y - (window_size_x * 3) / 4;
		}
		else if(window_size_x * 3 > window_size_y * 4)
		{
			x_offset = (window_size_x - (window_size_y * 4) / 3) / 2;
		}
	}

	/*
	 * Default to original game resolution if none given
	 */

	if (window_size_x == 0) window_size_x = game_width;
	if (window_size_y == 0) window_size_y = game_height;

	info("Original resolution %ix%i, New resolution %ix%i\n", game_width, game_height, window_size_x, window_size_y);

	// perform any additional initialization that requires the rendering environment to be set up
	if(!ff8) ff7_post_init();
	else ff8_post_init();

	return ret;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// Enable the following block if you wish to use VS2019 ReAttach extension
		/*
		while (!IsDebuggerPresent())
		{
			Sleep(100);
		}
		__debugbreak();
		*/

		GetCurrentDirectoryA(BASEDIR_LENGTH, basedir);

		// install crash handler
		open_applog("FFNx.log");
		SetUnhandledExceptionFilter(ExceptionHandler);

		info("FFNx driver version " VERSION PRERELEASE_WARNING "\n");
		version = get_version();

		if (version >= VERSION_FF8_12_US) ff8 = true;

		read_cfg();

		CHAR parentName[1024];

		GetModuleFileNameA(NULL, parentName, sizeof(parentName));

		_strlwr(parentName);

		if (!ff8 &&
			(
				strstr(parentName, "ff7.exe") != NULL ||
				strstr(parentName, "ff7_en.exe") != NULL ||
				strstr(parentName, "ff7_de.exe") != NULL ||
				strstr(parentName, "ff7_fr.exe") != NULL ||
				strstr(parentName, "ff7_sp.exe") != NULL
			)
		)
		{
			replace_function(0x404A7D, ff7_get_inserted_cd);

			if (strstr(parentName, "ff7_en.exe") != NULL ||
				strstr(parentName, "ff7_de.exe") != NULL ||
				strstr(parentName, "ff7_fr.exe") != NULL ||
				strstr(parentName, "ff7_sp.exe") != NULL)
			{
				steam_edition = true;

				// Steam edition has music files under a different path by default
				external_music_path = "data/music_ogg";
			}
		}
		else if (ff8 &&
			(
				strstr(parentName, "ff8_en.exe") != NULL ||
				strstr(parentName, "ff8_fr.exe") != NULL ||
				strstr(parentName, "ff8_de.exe") != NULL ||
				strstr(parentName, "ff8_it.exe") != NULL ||
				strstr(parentName, "ff8_sp.exe") != NULL ||
				strstr(parentName, "ff8_jp.exe") != NULL
			)
		)
		{
			steam_edition = true;

			DWORD offset = version == VERSION_FF8_12_JP ? 0x402320 : 0x401F60;
			DWORD offset2; // No idea what is this required
			DWORD offset3; // Eyes on me patch

			// Calculate offset2
			switch (version)
			{
			case VERSION_FF8_12_US_NV:
				offset2 = 0xB86014;
				offset3 = 0xB69388;
				break;
			case VERSION_FF8_12_FR_NV:
			case VERSION_FF8_12_DE_NV:
			case VERSION_FF8_12_SP_NV:
			case VERSION_FF8_12_IT_NV:
				offset2 = 0xB85F74;
				offset3 = 0xB69388;
				break;
			case VERSION_FF8_12_JP:
				offset2 = 0xD89654;
				offset3 = 0x2CA3DC8;
				break;
			}

			/*
			Patch 1,2,3 are required to inject this DLL into the main process
			Patch 4 is done as well by the official Steam driver, but no idea why.
			*/
			// 1
			patch_code_int(offset, 0x000001BA);
			patch_code_int(offset + 0x4, -0x2F793900);
			patch_code_word(offset + 0x8, 0x000B);
			patch_code_byte(offset + 0xA, 0x00);
			// 2
			patch_code_dword(offset + 0xB, (DWORD)new_dll_graphics_driver);
			// 3
			patch_code_int(offset + 0xF, 0x0001E0B8);
			patch_code_word(offset + 0x13, -0x7000);
			// 4
			patch_code_int(offset2, ((DWORD*)offset2)[11]);
			// 5
			patch_code_dword(offset3, (DWORD)dotemuMciSendCommandA);

			// Steam edition contains movies unpacked
			use_external_movie = cfg_bool_t(true);
		}
	}

	return TRUE;
}

// Steam compatibility
__declspec(dllexport) LSTATUS __stdcall dotemuRegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
	return ERROR_SUCCESS;
}

__declspec(dllexport) LSTATUS __stdcall dotemuRegCloseKey(HKEY hKey)
{
	return ERROR_SUCCESS;
}

__declspec(dllexport) LSTATUS __stdcall dotemuRegFlushKey(HKEY hKey)
{
	return ERROR_SUCCESS;
}

__declspec(dllexport) LSTATUS __stdcall dotemuRegDeleteValueA(HKEY hKey, LPCSTR lpValueName)
{
	return ERROR_SUCCESS;
}

__declspec(dllexport) LSTATUS __stdcall dotemuRegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, LPBYTE lpData, DWORD cbData)
{
	if (ff8)
	{
		if (strcmp(lpValueName, "SFXVolume") == 0 || strcmp(lpValueName, "MusicVolume") == 0)
		{
			if (lpData[0] > 0x64)
				lpData[0] = 0x64;
		}
	}

	return ERROR_SUCCESS;
}

__declspec(dllexport) LSTATUS __stdcall dotemuRegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	LSTATUS ret = ERROR_SUCCESS;

	LPSTR buf = new CHAR[*lpcbData]{ 0 };

	/* FF7 */
	// General
	if (strcmp(lpValueName, "AppPath") == 0)
	{
		GetCurrentDirectory(*lpcbData, buf);
		strcat(buf, R"(\)");

		strcpy((CHAR*)lpData, buf);
	}
	else if (strcmp(lpValueName, "DataPath") == 0)
	{
		GetCurrentDirectory(*lpcbData, buf);
		strcat(buf, R"(\data\)");

		strcpy((CHAR*)lpData, buf);
	}
	else if (strcmp(lpValueName, "DataDrive") == 0)
	{
		strcpy((CHAR*)lpData, "CD:");
	}
	else if (strcmp(lpValueName, "MoviePath") == 0)
	{
		GetCurrentDirectory(*lpcbData, buf);
		strcat(buf, R"(\data\movies\)");

		strcpy((CHAR*)lpData, buf);
	}
	else if (strcmp(lpValueName, "FullInstall") == 0)
	{
		lpData[0] = 0x1;
	}
	// Audio
	else if (strcmp(lpValueName, "DD_GUID") == 0 || strcmp(lpValueName, "Sound_GUID") == 0)
	{
		memcpy(lpData, buf, *lpcbData);
	}
	else if (strcmp(lpValueName, "MIDI_DeviceID") == 0)
	{
		lpData[0] = 0x0;
	}
	else if (strcmp(lpValueName, "Sound") == 0 || strcmp(lpValueName, "Midi") == 0 || strcmp(lpValueName, "wave_music") == 0)
	{
		ret = 2;
	}
	else if (strcmp(lpValueName, "SFXVolume") == 0 || strcmp(lpValueName, "MusicVolume") == 0)
	{
		lpData[0] = 0x64;
	}
	// Graphics
	else if (strcmp(lpValueName, "Driver") == 0)
	{
		lpData[0] = 0x3;
	}
	else if (strcmp(lpValueName, "DriverPath") == 0)
	{
		strcpy((CHAR*)lpData, R"(AF3DN.P)");
	}
	else if (strcmp(lpValueName, "Mode") == 0)
	{
		lpData[0] = 0x2;

		// Steam release is somehow requesting this key multiple times.
		// Returning 1 will set 2 internally in the engine
		if (*lpcbData == 256) ret = 1;
	}
	else if (strcmp(lpValueName, "Options") == 0)
	{
		lpData[0] = 0x12;
	}
	/* FF8 */
	else if (strcmp(lpValueName, "GraphicsGUID") == 0 || strcmp(lpValueName, "SoundGUID") == 0 || strcmp(lpValueName, "MIDIGUID") == 0)
	{
		memcpy(lpData, buf, 16);
	}
	else if (strcmp(lpValueName, "SoundOptions") == 0)
	{
		lpData[0] = 0x00000000;
	}
	else if (strcmp(lpValueName, "InstallOptions") == 0)
	{
		lpData[0] = 0x000000ff;
	}
	else if (strcmp(lpValueName, "MidiOptions") == 0)
	{
		lpData[0] = 0x00000001;
	}
	else if (strcmp(lpValueName, "Graphics") == 0)
	{
		lpData[0] = 0x10100021;
	}

	delete[] buf;

	return ret;
}

__declspec(dllexport) HANDLE __stdcall dotemuCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	HANDLE ret = INVALID_HANDLE_VALUE;

	if (strstr(lpFileName, "CD:") != NULL)
	{
		CHAR newPath[260]{ 0 };
		uint8_t requiredDisk = *(uint8_t*)(*(DWORD*)ff8_externals.requiredDisk + 0xCC);
		CHAR diskAsChar[2];

		itoa(requiredDisk, diskAsChar, 10);

		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		if (strstr(lpFileName, "DISK1") != NULL || strstr(lpFileName, "DISK2") != NULL || strstr(lpFileName, "DISK3") != NULL || strstr(lpFileName, "DISK4") != NULL)
		{
			PathAppendA(newPath, R"(data\disk)");
			PathAppendA(newPath, pos);

			if (strstr(lpFileName, diskAsChar) != NULL)
			{
				ff8_currentdisk = requiredDisk;

				ret = CreateFileA(newPath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
			}
		}
	}
	else if (strstr(lpFileName, "app.log"))
	{
		CHAR newPath[260]{ 0 };

		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		get_userdata_path(newPath, 260, false);
		PathAppendA(newPath, pos);

		ret = CreateFileA(newPath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	else if (strstr(lpFileName, "temp.fi") || strstr(lpFileName, "temp.fl") || strstr(lpFileName, "temp.fs") || strstr(lpFileName, "temp_evn.") || strstr(lpFileName, "temp_odd."))
	{
		CHAR newPath[260]{ 0 };

		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		get_userdata_path(newPath, 260, false);
		PathAppendA(newPath, pos);

		ret = CreateFileA(newPath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	else if (strstr(lpFileName, ".fi") != NULL || strstr(lpFileName, ".fl") != NULL || strstr(lpFileName, ".fs") != NULL)
	{
		CHAR newPath[260]{ 0 };

		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		get_data_lang_path(newPath);
		PathAppendA(newPath, pos);

		ret = CreateFileA(newPath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	else if (strstr(lpFileName, R"(SAVE\)") != NULL)
	{
		CHAR newPath[260]{ 0 };
		CHAR saveFileName[50]{ 0 };

		// Search for the next character pointer after "SAVE\"
		const char* pos = strstr(lpFileName, R"(SAVE\)") + 5;
		strcpy(saveFileName, pos);
		_strlwr(saveFileName);
		*(strstr(saveFileName, R"(\)")) = 95;
		strcat(saveFileName, R"(.ff8)");

		get_userdata_path(newPath, 260, true);
		PathAppendA(newPath, saveFileName);

		ret = CreateFileA(newPath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	else
		ret = CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	return ret;
}

__declspec(dllexport) UINT __stdcall dotemuGetDriveTypeA(LPCSTR lpRootPathName)
{
	UINT ret;

	if (strcmp(lpRootPathName, "CD:") == 0)
	{
		ret = DRIVE_CDROM;
	}
	else
	{
		ret = GetDriveTypeA(lpRootPathName);
	}

	return ret;
}

__declspec(dllexport) BOOL __stdcall dotemuDeleteFileA(LPCSTR lpFileName)
{
	BOOL ret = false;

	if (strstr(lpFileName, "app.log"))
	{
		CHAR newPath[260]{ 0 };

		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		get_userdata_path(newPath, 260, false);
		PathAppendA(newPath, pos);

		ret = DeleteFileA(newPath);
	}
	else if (strstr(lpFileName, "temp.fi") || strstr(lpFileName, "temp.fl") || strstr(lpFileName, "temp.fs") || strstr(lpFileName, "temp_evn.") || strstr(lpFileName, "temp_odd."))
	{
		CHAR newPath[260]{ 0 };

		// Search for the last '\' character and get a pointer to the next char
		const char* pos = strrchr(lpFileName, 92) + 1;

		get_userdata_path(newPath, 260, false);
		PathAppendA(newPath, pos);

		ret = DeleteFileA(newPath);
	}
	else
		ret = DeleteFileA(lpFileName);

	return ret;
}

#if defined(__cplusplus)
}
#endif
