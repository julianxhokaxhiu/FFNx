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
 * gl/special_case.c - rendering special cases, quirks or improved effects
 */

#define _USE_MATH_DEFINES
#include <math.h>
#include "../renderer.h"

#include "../types.h"
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
uint gl_special_case(uint primitivetype, uint vertextype, struct nvertex *vertices, uint vertexcount, word *indices, uint count, struct graphics_object *graphics_object, uint clip, uint mipmap)
{
	uint mode = getmode_cached()->driver_mode;
	VOBJ(texture_set, texture_set, current_state.texture_set);
	uint defer = false;

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

	// z-sort by default in menu and condor battle, unnecessary sorting will be
	// avoided by defer logic
	if(mode == MODE_MENU) defer = true;
	if(mode == MODE_CONDOR) defer = true;

	if(!ff8)
	{
		if(current_state.texture_set && VREF(texture_set, tex_header))
		{
			VOBJ(tex_header, tex_header, VREF(texture_set, tex_header));

			if((uint)VREF(tex_header, file.pc_name) > 32)
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
