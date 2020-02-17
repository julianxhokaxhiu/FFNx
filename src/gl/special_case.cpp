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
#include "../bgfx.h"

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
			newRenderer.setBlendMode(RendererBlendMode::BLEND_NONE);
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

	if(ff8)
	{
		// fix cerberus line effect
		if(mode == MODE_SWIRL && primitivetype == RendererPrimitiveType::PT_LINES && vertices[0].color.color == 0 && vertices[1].color.color == 0xFFFF0000 && vertexcount <= 64)
		{
			uint i, j;

			gl_set_blend_func(2);

			for(i = 0; i < vertexcount; i += 2)
			{
				uint color1 = vertices[i].color.color;
				uint color2 = vertices[i + 1].color.color;
				float z = vertices[i]._.z;
				double x = vertices[i]._.x;
				double factor = (double)window_size_x / (double)game_width;
				double inv_factor = 0.5 / factor;
				uint new_count = ((uint)(4 * factor)) * 2;
				struct nvertex *_vertices = (struct nvertex*)driver_malloc(new_count * sizeof(*_vertices));
				word *_indices = (word*)driver_malloc(new_count * sizeof(*_indices));

				for(j = 0; j < new_count; j += 2)
				{
					_vertices[j]._.x = (float)x;
					_vertices[j]._.y = vertices[i]._.y;
					_vertices[j]._.z = z;
					_vertices[j].color.w = 1.0f;
					_vertices[j].color.color = 0x80000000;

					_indices[j] = j;

					_vertices[j + 1]._.x = (float)x;
					_vertices[j + 1]._.y = vertices[i + 1]._.y;
					_vertices[j + 1]._.z = z;
					_vertices[j + 1].color.w = 1.0f;
					_vertices[j + 1].color.color = 0x8000FFFF;

					_indices[j + 1] = j + 1;

					x += inv_factor;
				}

				gl_draw_indexed_primitive(primitivetype, vertextype, _vertices, new_count, _indices, new_count, graphics_object, false, true);

				driver_free(_vertices);
				driver_free(_indices);
			}

			return true;
		}

		// fix FF8 battle swirl zoom effect
		if(*ff8_externals.swirl_texture1)
		{
			if((*ff8_externals.swirl_texture1)->hundred_data)
			{
				if(current_state.texture_set && current_state.texture_set == (*ff8_externals.swirl_texture1)->hundred_data->texture_set)
				{
					// normal battle swirl
					if(vertexcount == 4 && current_state.blend_mode == 1)
					{
						float offset = vertices[1]._.x - 640.0f;

						offset = (1.0f - cosf((offset / 160.0f) * (M_PI / 2.0f))) * 160.0f;

						vertices[0]._.x = offset;
						vertices[1]._.x = 640.0f + offset;
						vertices[2]._.x = offset;
						vertices[3]._.x = 640.0f + offset;

						vertices[0]._.y = 16.0f;
						vertices[1]._.y = 16.0f;
						vertices[2]._.y = 464.0f;
						vertices[3]._.y = 464.0f;

						vertices[0].color.color = 0xFF404040;
						vertices[1].color.color = 0xFF101010;
						vertices[2].color.color = 0xFF404040;
						vertices[3].color.color = 0xFF101010;
					}

					// boss swirl
					if(vertexcount == 16)
					{
						vertices[0]._.x = -vertices[4]._.x;
						vertices[1]._.x = 640.0f - (vertices[5]._.x - 640.0f);
						vertices[2]._.x = -vertices[6]._.x;
						vertices[3]._.x = 640.0f - (vertices[7]._.x - 640.0f);
						vertices[8]._.x = -vertices[12]._.x;
						vertices[9]._.x = 640.0f - (vertices[13]._.x - 640.0f);
						vertices[10]._.x = -vertices[14]._.x;
						vertices[11]._.x = 640.0f - (vertices[15]._.x - 640.0f);

						vertices[0]._.y = 16.0f - (vertices[8]._.y - 16.0f);
						vertices[1]._.y = 16.0f - (vertices[9]._.y - 16.0f);
						vertices[2]._.y = 464.0f - (vertices[10]._.y - 464.0f);
						vertices[3]._.y = 464.0f - (vertices[11]._.y - 464.0f);
						vertices[4]._.y = 16.0f - (vertices[12]._.y - 16.0f);
						vertices[5]._.y = 16.0f - (vertices[13]._.y - 16.0f);
						vertices[6]._.y = 464.0f - (vertices[14]._.y - 464.0f);
						vertices[7]._.y = 464.0f - (vertices[15]._.y - 464.0f);
					}
				}
			}
		}

		// fix FF8 battle swirl fade effect
		if(mode == MODE_SWIRL && primitivetype == RendererPrimitiveType::PT_LINES && vertexcount == 896)
		{
			uint i;
			uint color1 = vertices[0].color.color;
			uint color2 = vertices[1].color.color;
			float z = vertices[0]._.z;
			double y = vertices[0]._.y;
			double factor = (double)window_size_y / (double)game_height;
			double inv_factor = 0.5 / factor;
			uint new_count = ((uint)((vertexcount + 2) * factor)) * 2;
			struct nvertex *_vertices = (struct nvertex*)driver_malloc(new_count * sizeof(*_vertices));
			word *_indices = (word*)driver_malloc(new_count * sizeof(*_indices));

			for(i = 0; i < new_count; i += 2)
			{
				uint xvert = ((uint)(i / factor) / 4) * 2 + 1;
				float x = vertices[(xvert < 895 ? xvert : 895)]._.x;

				_vertices[i]._.x = 0.0f;
				_vertices[i]._.y = (float)y;
				_vertices[i]._.z = z;
				_vertices[i].color.w = 1.0f;
				_vertices[i].color.color = color1;

				_indices[i] = i;

				_vertices[i + 1]._.x = x;
				_vertices[i + 1]._.y = (float)y;
				_vertices[i + 1]._.z = z;
				_vertices[i + 1].color.w = 1.0f;
				_vertices[i + 1].color.color = 0xFF000000;

				_indices[i + 1] = i + 1;

				y += inv_factor;
			}

			gl_draw_indexed_primitive(primitivetype, vertextype, _vertices, new_count, _indices, new_count, graphics_object, false, true);

			driver_free(_vertices);
			driver_free(_indices);

			return true;
		}
	}
	else
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
