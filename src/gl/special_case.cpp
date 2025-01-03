/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
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

#define _USE_MATH_DEFINES
#include <math.h>

#include "../gl.h"
#include "../cfg.h"
#include "../macro.h"
#include "../globals.h"

#include "../ff7/widescreen.h"

#define SAFE_GFXOBJ_CHECK(X, Y) ((X) && (X) == (struct graphics_object *)(Y))

// rendering special cases, returns true if the draw call has been handled in
// some way and should not be rendered normally
// it is generally not safe to modify source data directly, a copy should be
// made and rendered separately
uint32_t gl_special_case(uint32_t primitivetype, uint32_t vertextype, struct nvertex *vertices, uint32_t vertexcount, WORD *indices, uint32_t count, struct graphics_object *graphics_object, uint32_t clip, uint32_t mipmap)
{
	uint32_t mode = getmode_cached()->driver_mode;
	VOBJ(texture_set, texture_set, current_state.texture_set);
	uint32_t defer = false, force_defer = false;

	// modpath textures rendered in 3D should always be filtered
	if(vertextype != TLVERTEX && current_state.texture_set && VREF(texture_set, ogl.external)) current_state.texture_filter = true;

	// modpath textures in menu should always be filtered
	if((mode == MODE_MENU || mode == MODE_MAIN_MENU) && current_state.texture_set && VREF(texture_set, ogl.external)) current_state.texture_filter = true;

	// some modpath textures have filtering forced on
	if(current_state.texture_set && VREF(texture_set, ogl.gl_set->force_filter) && VREF(texture_set, ogl.external)) current_state.texture_filter = true;

	// Texture filtering mostly does not work well in FF8
	if(ff8) current_state.texture_filter = enable_bilinear && vertextype != TLVERTEX && current_state.texture_set;
	else if (enable_bilinear && (vertextype != TLVERTEX || mode == MODE_MENU || mode == MODE_MAIN_MENU || (current_state.texture_set && VREF(texture_set, ogl.gl_set->force_filter)))) current_state.texture_filter = true;

	// some modpath textures have z-sort forced on
	if(current_state.texture_set && VREF(texture_set, ogl.gl_set->force_zsort) && VREF(texture_set, ogl.external)) defer = true;

	// z-sort by default in menu, unnecessary sorting will be avoided by defer logic
	if(mode == MODE_MENU || mode == MODE_MAIN_MENU) defer = true;

	if(!ff8)
	{
		if(SAFE_GFXOBJ_CHECK(graphics_object, ff7_externals.menu_objects->buster_tex))
		{
			// stretch main menu to fullscreen if it is a modpath texture
			if(VREF(texture_set, ogl.external) && vertexcount == 4)
			{
				float texture_ratio = VREF(texture_set, ogl.width) / (float)VREF(texture_set, ogl.height);
				bool use_wide_vertices = abs(texture_ratio - 16 / (aspect_ratio == AR_WIDESCREEN_16X10 ? 10.f : 9.f)) <= 0.01 && widescreen_enabled;
				float x = use_wide_vertices ? wide_viewport_x : 0.0f;
				float y = 0.0f;
				float width = use_wide_vertices ? wide_viewport_width : game_width;
				float height = game_height;
				vertices[0]._.x = x;
				vertices[0]._.y = y;
				vertices[0]._.z = 1.0f;
				vertices[1]._.x = x;
				vertices[1]._.y = y + height;
				vertices[1]._.z = 1.0f;
				vertices[2]._.x = x + width;
				vertices[2]._.y = y;
				vertices[2]._.z = 1.0f;
				vertices[3]._.x = x + width;
				vertices[3]._.y = y + height;
				vertices[3]._.z = 1.0f;
				vertices[0].u = 0.0f;
				vertices[0].v = 0.0f;
				vertices[1].u = 0.0f;
				vertices[1].v = 1.0f;
				vertices[2].u = 1.0f;
				vertices[2].v = 0.0f;
				vertices[3].u = 1.0f;
				vertices[3].v = 1.0f;
			}
		}

		if(current_state.texture_set && VREF(texture_set, tex_header))
		{
			VOBJ(tex_header, tex_header, VREF(texture_set, tex_header));

			if((uint32_t)VREF(tex_header, file.pc_name) > 32)
			{
				// avoid filtering window borders
				if(!_strnicmp(VREF(tex_header, file.pc_name), "menu/btl_win_c_", strlen("menu/btl_win_c_") - 1) && VREF(texture_set, palette_index) == 0) current_state.texture_filter = false;
			}
		}

		// z-sort select menu elements everywhere
		if(SAFE_GFXOBJ_CHECK(graphics_object, ff7_externals.menu_objects->menu_fade)) defer = true;
		if(SAFE_GFXOBJ_CHECK(graphics_object, ff7_externals.menu_objects->blend_window_bg)) defer = true;

		if(mode == MODE_FIELD)
		{
			// always z-sort vanilla messages
			if(SAFE_GFXOBJ_CHECK(graphics_object, ff7_externals.menu_objects->window_bg)) force_defer = true;

			// fix timer messages when window is normal
			if(SAFE_GFXOBJ_CHECK(graphics_object, ff7_externals.menu_objects->_btl_win)) force_defer = true;
			if(SAFE_GFXOBJ_CHECK(graphics_object, ff7_externals.menu_objects->btl_win_a)) force_defer = true;
			if(SAFE_GFXOBJ_CHECK(graphics_object, ff7_externals.menu_objects->btl_win_b)) force_defer = true;
			if(SAFE_GFXOBJ_CHECK(graphics_object, ff7_externals.menu_objects->btl_win_c)) force_defer = true;
			if(SAFE_GFXOBJ_CHECK(graphics_object, ff7_externals.menu_objects->btl_win_d)) force_defer = true;
		}

		if(mode == MODE_BATTLE)
		{
			// z-sort some GUI elements in battle
			if(SAFE_GFXOBJ_CHECK(graphics_object, ff7_externals.menu_objects->unknown2)) defer = true; // Limit and barrier bar (necessary for ESUI)
			if(SAFE_GFXOBJ_CHECK(graphics_object, ff7_externals.menu_objects->unknown3)) defer = true; // Limit and barrier bar (necessary for ESUI)
			if(SAFE_GFXOBJ_CHECK(graphics_object, ff7_externals.menu_objects->unknown5)) defer = true; // Limit box (necessary for ESUI)
		}
	}

	if((defer || force_defer) && !ff8) return gl_defer_sorted_draw(primitivetype, vertextype, vertices, vertexcount, indices, count, clip, mipmap, force_defer);

	return false;
}
