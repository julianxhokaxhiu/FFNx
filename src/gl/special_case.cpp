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

#define _USE_MATH_DEFINES
#include <math.h>
#include "../renderer.h"

#include "../gl.h"
#include "../cfg.h"
#include "../macro.h"
#include "../log.h"
#include "../ff8.h"
#include "../saveload.h"

#define SAFE_GFXOBJ_CHECK(X, Y) ((X) && (X) == (struct graphics_object *)(Y))

// rendering special cases, returns true if the draw call has been handled in
// some way and should not be rendered normally
// it is generally not safe to modify source data directly, a copy should be
// made and rendered separately
uint32_t gl_special_case(uint32_t primitivetype, uint32_t vertextype, struct nvertex *vertices, uint32_t vertexcount, WORD *indices, uint32_t count, struct graphics_object *graphics_object, uint32_t clip, uint32_t mipmap)
{
	uint32_t mode = getmode_cached()->driver_mode;
	VOBJ(texture_set, texture_set, current_state.texture_set);
	uint32_t defer = false;

	if(fancy_transparency && current_state.texture_set && current_state.blend_mode == BLEND_NONE)
	{
		// restore original blend mode for non-modpath textures
		if (!VREF(texture_set, ogl.external))
		{
			newRenderer.useFancyTransparency(false);
		}
	}

	// modpath textures rendered in 3D should always be filtered
	if(vertextype != TLVERTEX && current_state.texture_set && VREF(texture_set, ogl.external)) current_state.texture_filter = true;

	// modpath textures in menu should always be filtered
	if(mode == MODE_MENU && current_state.texture_set && VREF(texture_set, ogl.external)) current_state.texture_filter = true;

	// some modpath textures have filtering forced on
	if(current_state.texture_set && VREF(texture_set, ogl.gl_set->force_filter) && VREF(texture_set, ogl.external)) current_state.texture_filter = true;

	// some modpath textures have z-sort forced on
	if(current_state.texture_set && VREF(texture_set, ogl.gl_set->force_zsort) && VREF(texture_set, ogl.external)) defer = true;

	// z-sort by default in menu, unnecessary sorting will be avoided by defer logic
	if(mode == MODE_MENU) defer = true;

	if(!ff8)
	{
		if(SAFE_GFXOBJ_CHECK(graphics_object, ff7_externals.menu_objects->buster_tex))
		{
			// stretch main menu to fullscreen if it is a modpath texture
			if(VREF(texture_set, ogl.external) && vertexcount == 4)
			{
				vertices[0]._.x = 0.0f;
				vertices[0]._.y = 0.0f;
				vertices[0]._.z = 1.0f;
				vertices[1]._.x = 0.0f;
				vertices[1]._.y = (float)game_height;
				vertices[1]._.z = 1.0f;
				vertices[2]._.x = (float)game_width;
				vertices[2]._.y = 0.0f;
				vertices[2]._.z = 1.0f;
				vertices[3]._.x = (float)game_width;
				vertices[3]._.y = (float)game_height;
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

		if(mode == MODE_BATTLE && vertextype == TLVERTEX)
		{
			// z-sort all obvious GUI elements in battle
			if(!(current_state.viewport[2] == 640 && (current_state.viewport[3] == 332 || current_state.viewport[3] == 480)) || vertexcount == 4) defer = true;
		}
	}

	if(defer && fancy_transparency) return gl_defer_draw(primitivetype, vertextype, vertices, vertexcount, indices, count, clip, mipmap);

	return false;
}
