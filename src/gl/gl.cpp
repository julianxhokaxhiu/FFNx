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

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>

#include "../renderer.h"

#include "../cfg.h"
#include "../gl.h"
#include "../macro.h"
#include "../log.h"
#include "../matrix.h"

struct matrix d3dviewport_matrix = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

struct driver_state current_state;

int max_texture_size;

extern uint32_t nodefer;

// draw a fullscreen quad, respect aspect ratio of source image
void gl_draw_movie_quad_common(uint32_t width, uint32_t height)
{
	struct game_obj *game_object = common_externals.get_game_object();
	float ratio = game_width / (float)width;
	float movieHeight = ratio * height;
	float movieWidth = ratio * width;
	float movieOffsetY = (game_height - movieHeight) / 2.0f;

	/*  y0    y2
	 x0 +-----+ x2
		|    /|
		|   / |
		|  /  |
		| /   |
		|/    |
	 x1 +-----+ x3
		y1    y3
	*/

	// 0
	float x0 = 0.0f;
	float y0 = movieOffsetY;
	float u0 = 0.0f;
	float v0 = 0.0f;
	// 1
	float x1 = x0;
	float y1 = movieHeight + movieOffsetY;
	float u1 = u0;
	float v1 = 1.0f;
	// 2
	float x2 = movieWidth;
	float y2 = y0;
	float u2 = 1.0f;
	float v2 = v0;
	// 3
	float x3 = x2;
	float y3 = y1;
	float u3 = u2;
	float v3 = v1;

	struct nvertex vertices[] = {
		{x0, y0, 1.0f, 1.0f, 0xffffffff, 0, u0, v0},
		{x1, y1, 1.0f, 1.0f, 0xffffffff, 0, u1, v1},
		{x2, y2, 1.0f, 1.0f, 0xffffffff, 0, u2, v2},
		{x3, y3, 1.0f, 1.0f, 0xffffffff, 0, u3, v3},
	};
	WORD indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	current_state.texture_filter = true;

	internal_set_renderstate(V_NOCULL, 1, game_object);
	internal_set_renderstate(V_DEPTHTEST, 0, game_object);
	internal_set_renderstate(V_DEPTHMASK, 0, game_object);

	newRenderer.setInterpolationQualifier(RendererInterpolationQualifier::SMOOTH);
	newRenderer.bindVertexBuffer(vertices, 4);
	newRenderer.bindIndexBuffer(indices, 6);

	newRenderer.isTLVertex(true);
	newRenderer.doTextureFiltering(current_state.texture_filter);

	newRenderer.draw();
}

// draw movie frame
void gl_draw_movie_quad(uint32_t width, uint32_t height)
{
	struct driver_state saved_state;

	gl_save_state(&saved_state);

	gl_draw_movie_quad_common(width, height);

	gl_load_state(&saved_state);
}

// save complete rendering state to memory
void gl_save_state(struct driver_state *dest)
{
	memcpy(dest, &current_state, sizeof(current_state));
}

// restore complete rendering state from memory
void gl_load_state(struct driver_state *src)
{
	memcpy(&current_state, src, sizeof(current_state));

	gl_bind_texture_set(src->texture_set);
	gl_set_texture(src->texture_handle);
	current_state.texture_set = src->texture_set;
	common_setviewport(src->viewport[0], src->viewport[1], src->viewport[2], src->viewport[3], 0);
	gl_set_blend_func(src->blend_mode);
	internal_set_renderstate(V_WIREFRAME, src->wireframe, 0);
	// setting V_LINEARFILTER has no side effects
	internal_set_renderstate(V_CULLFACE, src->cullface, 0);
	internal_set_renderstate(V_NOCULL, src->nocull, 0);
	internal_set_renderstate(V_DEPTHTEST, src->depthtest, 0);
	internal_set_renderstate(V_DEPTHMASK, src->depthmask, 0);
	internal_set_renderstate(V_ALPHATEST, src->alphatest, 0);
	internal_set_renderstate(V_ALPHAFUNC, src->alphafunc, 0);
	internal_set_renderstate(V_ALPHAREF, src->alpharef, 0);
	internal_set_renderstate(V_SHADEMODE, src->shademode, 0);
	gl_set_world_matrix(&src->world_matrix);
	gl_set_d3dprojection_matrix(&src->d3dprojection_matrix);
}

// draw a set of primitives with a known model->world transformation
// interesting for real-time lighting, maps to normal rendering routine for now
void gl_draw_with_lighting(struct indexed_primitive *ip, uint32_t clip, struct matrix *model_matrix)
{
	gl_draw_indexed_primitive(ip->primitivetype, ip->vertextype, ip->vertices, ip->vertexcount, ip->indices, ip->indexcount, 0, clip, true);
}

// main rendering routine, draws a set of primitives according to the current render state
void gl_draw_indexed_primitive(uint32_t primitivetype, uint32_t vertextype, struct nvertex *vertices, uint32_t vertexcount, WORD *indices, uint32_t count, struct graphics_object *graphics_object, uint32_t clip, uint32_t mipmap)
{
	FILE *log;
	uint32_t i;
	uint32_t mode = getmode_cached()->driver_mode;
	// filter setting can change inside this function, we don't want that to
	// affect the global rendering state so save & restore it
	uint32_t saved_texture_filter = current_state.texture_filter;

	// should never happen, broken 3rd-party models cause this
	if(!count) return;

	// scissor test is used to emulate D3D viewports
	if (clip) newRenderer.doScissorTest(true);
	else newRenderer.doScissorTest(false);

	if(vertextype > TLVERTEX)
	{
		unexpected_once("vertextype > TLVERTEX\n");
		return;
	}

	newRenderer.setInterpolationQualifier(current_state.shademode ? RendererInterpolationQualifier::SMOOTH : RendererInterpolationQualifier::FLAT);

	// handle some special cases, see special_case.c
	if(gl_special_case(primitivetype, vertextype, vertices, vertexcount, indices, count, graphics_object, clip, mipmap))
	{
		// special cases can signal back to this function that the draw call has
		// been handled in some other manner
		current_state.texture_filter = saved_texture_filter;
		return;
	}

	// use mipmaps if available
	if (current_state.texture_filter && enable_anisotropic && current_state.texture_set)
	{
		VOBJ(texture_set, texture_set, current_state.texture_set);

		if (VREF(texture_set, ogl.external)) newRenderer.isExternalTexture(true);
	}

	// OpenGL treats texture filtering as a per-texture parameter, we need it
	// to be consistent with our global render state
	newRenderer.doTextureFiltering(current_state.texture_filter);

	if(vertextype != TLVERTEX)
	{
		newRenderer.setD3DProjection(&current_state.d3dprojection_matrix);
		newRenderer.setD3DViweport(&d3dviewport_matrix);
	}

	newRenderer.isTLVertex(vertextype == TLVERTEX);
	newRenderer.isFBTexture(current_state.fb_texture);

	if (ff8) newRenderer.doModulateAlpha(false);
	else newRenderer.doModulateAlpha(true);

	//// upload vertex data
	newRenderer.bindVertexBuffer(vertices, vertexcount);
	newRenderer.bindIndexBuffer(indices, count);
	newRenderer.setPrimitiveType(RendererPrimitiveType(primitivetype));

	newRenderer.draw();

	stats.vertex_count += count;

	current_state.texture_filter = saved_texture_filter;
}

void gl_set_world_matrix(struct matrix *matrix)
{
	newRenderer.setWorldView(matrix);
	memcpy(&current_state.world_matrix, matrix, sizeof(struct matrix));
}

void gl_set_d3dprojection_matrix(struct matrix *matrix)
{
	memcpy(&current_state.d3dprojection_matrix, matrix, sizeof(struct matrix));
}

// apply blend mode to OpenGL state
void gl_set_blend_func(uint32_t blend_mode)
{
	if(trace_all) trace("set blend mode %i\n", blend_mode);

	current_state.blend_mode = blend_mode;

	newRenderer.setBlendMode(RendererBlendMode(blend_mode));
}

// draw text on screen using the game font
uint32_t gl_draw_text(uint32_t x, uint32_t y, uint32_t color, uint32_t alpha, char *fmt, ...)
{
	char text[4096];
	va_list args;

	va_start(args, fmt);
	vsnprintf(text, 4096, fmt, args);
	newRenderer.printText(x, y, color, text);
	va_end(args);

	return true;
}
