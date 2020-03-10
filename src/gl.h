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
 * gl.h - definitions used by OpenGL renderer
 */

#pragma once

#include "types.h"
#include "common.h"
#include "matrix.h"

#define VERTEX 1
#define LVERTEX 2
#define TLVERTEX 3

struct driver_state
{
	struct texture_set *texture_set;
	uint texture_handle;
	uint blend_mode;
	uint viewport[4];
	uint fb_texture;
	uint wireframe;
	uint texture_filter;
	uint cullface;
	uint nocull;
	uint depthtest;
	uint depthmask;
	uint shademode;
	uint alphatest;
	uint alphafunc;
	uint alpharef;
	struct matrix world_matrix;
	struct matrix d3dprojection_matrix;
};

struct deferred_draw
{
	uint primitivetype;
	uint vertextype;
	uint vertexcount;
	uint count;
	struct nvertex *vertices;
	word *indices;
	uint clip;
	uint mipmap;
	struct driver_state state;
	double z;
	uint drawn;
};

struct gl_texture_set
{
	uint textures;
	uint force_filter;
	uint force_zsort;
};

extern struct matrix d3dviewport_matrix;

extern struct driver_state current_state;

extern uint current_program;

extern int max_texture_size;

void gl_draw_movie_quad();
void gl_save_state(struct driver_state *dest);
void gl_load_state(struct driver_state *src);
uint gl_defer_draw(uint primitivetype, uint vertextype, struct nvertex *vertices, uint vertexcount, word *indices, uint count, uint clip, uint mipmap);
void gl_draw_deferred();
void gl_check_deferred(struct texture_set *texture_set);
void gl_cleanup_deferred();
uint gl_special_case(uint primitivetype, uint vertextype, struct nvertex *vertices, uint vertexcount, word *indices, uint count, struct graphics_object *graphics_object, uint clip, uint mipmap);
void gl_draw_with_lighting(struct indexed_primitive *ip, uint clip, struct matrix *model_matrix);
void gl_draw_indexed_primitive(uint, uint, struct nvertex *, uint, word *, uint, struct graphics_object *, uint clip, uint mipmap);
void gl_set_world_matrix(struct matrix *matrix);
void gl_set_d3dprojection_matrix(struct matrix *matrix);
void gl_set_blend_func(uint);
void gl_check_texture_dimensions(uint width, uint height, char *source);
uint gl_create_texture(void *data, uint width, uint height, uint format, uint internalformat, uint size, uint generate_mipmaps);
void *gl_get_pixel_buffer(uint size);
uint gl_commit_pixel_buffer(void *data, uint width, uint height, uint format, uint generate_mipmaps);
uint gl_compress_pixel_buffer(void *data, uint width, uint height, uint format);
uint gl_commit_compressed_buffer(void *data, uint width, uint height, uint format, uint size);
void gl_replace_texture(struct texture_set *texture_set, uint palette_index, uint new_texture);
void gl_upload_texture(struct texture_set *texture_set, uint palette_index, void *image_data, uint format);
void gl_bind_texture_set(struct texture_set *);
void gl_set_texture(uint);
uint gl_draw_text(uint x, uint y, uint color, uint alpha, char *fmt, ...);
