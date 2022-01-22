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

#include "common.h"

#define VERTEX 1
#define LVERTEX 2
#define TLVERTEX 3

enum DrawOrder
{
	DRAW_ORDER_0 = 0,
	DRAW_ORDER_1,
	DRAW_ORDER_2,

	DRAW_ORDER_COUNT
};

struct driver_state
{
	struct texture_set *texture_set;
	uint32_t texture_handle;
	uint32_t blend_mode;
	uint32_t viewport[4];
	uint32_t fb_texture;
	uint32_t wireframe;
	uint32_t texture_filter;
	uint32_t cullface;
	uint32_t nocull;
	uint32_t depthtest;
	uint32_t depthmask;
	uint32_t shademode;
	uint32_t alphatest;
	uint32_t alphafunc;
	uint32_t alpharef;
	struct matrix world_view_matrix;
	struct matrix d3dprojection_matrix;
};

struct deferred_draw
{
	uint32_t primitivetype;
	uint32_t vertextype;
	uint32_t vertexcount;
	uint32_t count;
	uint32_t draworder;
	struct nvertex* vertices;
	vector3<float>* normals;
	WORD *indices;
	struct boundingbox* boundingbox;
	uint32_t clip;
	uint32_t mipmap;
	struct driver_state state;
};

struct deferred_sorted_draw
{
	deferred_draw deferred_draw;
	double z;
	uint32_t drawn;
};

struct gl_texture_set
{
	uint32_t textures;
	uint32_t force_filter;
	uint32_t force_zsort;
	uint32_t disable_lighting;
	// ANIMATED TEXTURES
	uint32_t is_animated;
	std::map<std::string, uint32_t> animated_textures;
	// ADDITIONAL TEXTURES
	std::map<uint16_t, uint32_t> additional_textures;
};

extern struct matrix d3dviewport_matrix;

extern struct driver_state current_state;

extern uint32_t current_program;

extern int max_texture_size;

void gl_draw_movie_quad(uint32_t width, uint32_t height);
void gl_save_state(struct driver_state *dest);
void gl_load_state(struct driver_state *src);
uint32_t gl_defer_draw(uint32_t primitivetype, uint32_t vertextype, struct nvertex* vertices, vector3<float>* normals, uint32_t vertexcount, WORD* indices, uint32_t count, struct boundingbox* boundingbox, uint32_t clip, uint32_t mipmap);
uint32_t gl_defer_sorted_draw(uint32_t primitivetype, uint32_t vertextype, struct nvertex *vertices, uint32_t vertexcount, WORD *indices, uint32_t count, uint32_t clip, uint32_t mipmap);
void gl_draw_deferred(bool isDrawOrderEnabled = false, DrawOrder draworder = DRAW_ORDER_0);
void gl_set_projection_viewport_matrices();
struct boundingbox calculateSceneAabb();
void gl_draw_sorted_deferred();
void gl_check_deferred(struct texture_set *texture_set);
void gl_cleanup_deferred();
uint32_t gl_special_case(uint32_t primitivetype, uint32_t vertextype, struct nvertex *vertices, uint32_t vertexcount, WORD *indices, uint32_t count, struct graphics_object *graphics_object, uint32_t clip, uint32_t mipmap);
void gl_draw_without_lighting(struct indexed_primitive* ip, uint32_t clip);
void gl_draw_with_lighting(struct indexed_primitive *ip, struct polygon_data *polydata, uint32_t clip);
void gl_draw_indexed_primitive(uint32_t, uint32_t, struct nvertex *, vector3<float>* normals, uint32_t, WORD *, uint32_t, struct graphics_object *, struct boundingbox* boundingbox, uint32_t clip, uint32_t mipmap);
void gl_set_worldview_matrix(struct matrix *matrix);
void gl_set_d3dprojection_matrix(struct matrix *matrix);
void gl_set_blend_func(uint32_t);
bool gl_check_texture_dimensions(uint32_t width, uint32_t height, char *source);
void gl_replace_texture(struct texture_set *texture_set, uint32_t palette_index, uint32_t new_texture);
void gl_upload_texture(struct texture_set *texture_set, uint32_t palette_index, void *image_data, uint32_t format);
void gl_bind_texture_set(struct texture_set *);
void gl_set_texture(uint32_t texture, struct gl_texture_set* gl_set);
uint32_t gl_draw_text(uint32_t x, uint32_t y, uint32_t color, uint32_t alpha, char *fmt, ...);
