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

#include "../renderer.h"

#include "../gl.h"
#include "../macro.h"
#include "../log.h"

uint32_t nodefer = false;

#define DEFERRED_MAX 1024

struct deferred_draw *deferred_draws;
uint32_t num_deferred;

struct deferred_sorted_draw *deferred_sorted_draws;
uint32_t num_sorted_deferred;

// save a draw call for later processing
uint32_t gl_defer_draw(uint32_t primitivetype, uint32_t vertextype, struct nvertex* vertices, vector3<float>* normals, uint32_t vertexcount, WORD* indices, uint32_t count, struct boundingbox* boundingbox, uint32_t clip, uint32_t mipmap)
{
	if (trace_all) ffnx_trace("gl_defer_draw: call with primitivetype: %u - vertextype: %u - vertexcount: %u - count: %u - clip: %d - mipmap: %d\n", primitivetype, vertextype, vertexcount, count, clip, mipmap);

	if (!deferred_draws) deferred_draws = (deferred_draw*)driver_calloc(sizeof(*deferred_draws), DEFERRED_MAX);

	if (ff8 || !enable_lighting)
	{
		return false;
	}

	// global disable
	if (nodefer) {
		if (trace_all) ffnx_trace("gl_defer_draw: nodefer true\n");
		return false;
	}

	if (num_deferred + 1 > DEFERRED_MAX)
	{
		if (trace_all) ffnx_trace("gl_defer_draw: deferred draw queue overflow - num_deferred: %u - count: %u - DEFERRED_MAX: %u\n", num_deferred, count, DEFERRED_MAX);
		return false;
	}

	uint32_t defer = num_deferred;

	deferred_draws[defer].count = count;
	deferred_draws[defer].clip = clip;
	deferred_draws[defer].mipmap = mipmap;
	deferred_draws[defer].primitivetype = primitivetype;
	deferred_draws[defer].vertextype = vertextype;
	deferred_draws[defer].vertexcount = vertexcount;
	deferred_draws[defer].indices = (WORD*)driver_malloc(sizeof(*indices) * count);
	deferred_draws[defer].vertices = (nvertex*)driver_malloc(sizeof(*vertices) * vertexcount);
	gl_save_state(&deferred_draws[defer].state);

	memcpy(deferred_draws[defer].indices, indices, sizeof(*indices) * count);
	memcpy(deferred_draws[defer].vertices, vertices, sizeof(*vertices) * vertexcount);

	if (boundingbox)
	{
		deferred_draws[defer].boundingbox = (struct boundingbox*)driver_malloc(sizeof(struct boundingbox));

		deferred_draws[defer].boundingbox->min_x = boundingbox->min_x;
		deferred_draws[defer].boundingbox->min_y = boundingbox->min_y;
		deferred_draws[defer].boundingbox->min_z = boundingbox->min_z;

		deferred_draws[defer].boundingbox->max_x = boundingbox->max_x;
		deferred_draws[defer].boundingbox->max_y = boundingbox->max_y;
		deferred_draws[defer].boundingbox->max_z = boundingbox->max_z;
	}
	else // calculate AABB if no bounding box found
	{
		deferred_draws[defer].boundingbox = (struct boundingbox*)driver_malloc(sizeof(struct boundingbox));

		deferred_draws[defer].boundingbox->min_x = FLT_MAX;
		deferred_draws[defer].boundingbox->min_y = FLT_MAX;
		deferred_draws[defer].boundingbox->min_z = FLT_MAX;

		deferred_draws[defer].boundingbox->max_x = FLT_MIN;
		deferred_draws[defer].boundingbox->max_y = FLT_MIN;
		deferred_draws[defer].boundingbox->max_z = FLT_MIN;

		for (int i = 0; i < vertexcount; ++i)
		{
			float vx = vertices[i]._.x;
			float vy = vertices[i]._.y;
			float vz = vertices[i]._.z;

			deferred_draws[defer].boundingbox->min_x = std::min(deferred_draws[defer].boundingbox->min_x, vx);
			deferred_draws[defer].boundingbox->min_y = std::min(deferred_draws[defer].boundingbox->min_y, vy);
			deferred_draws[defer].boundingbox->min_z = std::min(deferred_draws[defer].boundingbox->min_z, vz);

			deferred_draws[defer].boundingbox->max_x = std::max(deferred_draws[defer].boundingbox->max_x, vx);
			deferred_draws[defer].boundingbox->max_y = std::max(deferred_draws[defer].boundingbox->max_y, vy);
			deferred_draws[defer].boundingbox->max_z = std::max(deferred_draws[defer].boundingbox->max_z, vz);
		}
	}

	if (normals)
	{
		deferred_draws[defer].normals = (vector3<float>*)driver_malloc(sizeof(*normals) * vertexcount);
		memcpy(deferred_draws[defer].normals, normals, sizeof(*normals) * vertexcount);
	}

	num_deferred++;

	if (trace_all) ffnx_trace("gl_defer_draw: return true\n");

	return true;
}

// re-order and save a draw call for later processing
uint32_t gl_defer_sorted_draw(uint32_t primitivetype, uint32_t vertextype, struct nvertex *vertices, uint32_t vertexcount, WORD *indices, uint32_t count, uint32_t clip, uint32_t mipmap)
{
	uint32_t tri;
	uint32_t mode = getmode_cached()->driver_mode;
	uint32_t *tri_deferred;
	float *tri_z;
	uint32_t defer_index = 0;

	if (trace_all) ffnx_trace("gl_defer_sorted_draw: call with primitivetype: %u - vertextype: %u - vertexcount: %u - count: %u - clip: %d - mipmap: %d\n", primitivetype, vertextype, vertexcount, count, clip, mipmap);

	if(!deferred_sorted_draws) deferred_sorted_draws = (deferred_sorted_draw*)driver_calloc(sizeof(*deferred_sorted_draws), DEFERRED_MAX);

	// global disable
	if (nodefer) {
		if (trace_all) ffnx_trace("gl_defer_sorted_draw: nodefer true\n");
		return false;
	}

	// output will not be consistent if depth testing is disabled, this call
	// cannot be re-ordered
	if(!current_state.depthtest)
	{
		if (trace_all) ffnx_trace("gl_defer_sorted_draw: depthtest false\n");
		return false;
	}

	// framebuffer textures should not be re-ordered
	if(current_state.fb_texture)
	{
		if (trace_all) ffnx_trace("gl_defer_sorted_draw: fb_texture true\n");
		return false;
	}

	if(current_state.blend_mode != BLEND_NONE)
	{
		if (trace_all) ffnx_trace("gl_defer_sorted_draw: blend_mode != BLEND_NONE - blend_mode: %u\n", current_state.blend_mode);
		if(current_state.blend_mode != BLEND_AVG)
		{
			if (trace_all) ffnx_trace("gl_defer_sorted_draw: blend_mode != BLEND_AVG - blend_mode: %u\n", current_state.blend_mode);
			// be conservative with non-standard blending modes
			if (mode != MODE_MENU && mode != MODE_BATTLE) {
				if (trace_all) ffnx_trace("gl_defer_sorted_draw: mode != MODE_MENU && mode != MODE_BATTLE - mode: %u\n", mode);
				return false;
			}
		}
	}
	else
	{
		if (!current_state.texture_set)
		{
			if (trace_all) ffnx_trace("gl_defer_sorted_draw: texture_set false\n");
			return false;
		}
		else
		{
			VOBJ(texture_set, texture_set, current_state.texture_set);
			VOBJ(tex_header, tex_header, VREF(texture_set, tex_header));

			if (trace_all) ffnx_trace("gl_defer_sorted_draw: texture_set true for texture %s%d\n", VREF(tex_header, file.pc_name), VREF(tex_header, palette_index));

			// texture format does not support alpha, re-order is not necessary
			if (!VREF(texture_set, ogl.external) && VREF(tex_header, tex_format.alpha_bits) < 2) {
				if (trace_all) ffnx_trace("gl_defer_sorted_draw: texture format does not support alpha, re-order is not necessary\n");
				return false;
			}
		}
	}

	// quads are used for some GUI elements, we do not need to re-order these
	if (primitivetype != RendererPrimitiveType::PT_TRIANGLES) {
		if (trace_all) ffnx_trace("gl_defer_sorted_draw: primitivetype != TRIANGLES\n");
		return false;
	}

	if(num_sorted_deferred + count / 3 > DEFERRED_MAX)
	{
		if (trace_all) ffnx_trace("gl_defer_sorted_draw: deferred draw queue overflow - num_sorted_deferred: %u - count: %u - DEFERRED_MAX: %u\n", num_sorted_deferred, count, DEFERRED_MAX);
		return false;
	}

	tri_deferred = (uint32_t*)driver_calloc(sizeof(*tri_deferred), count / 3);
	tri_z = (float*)driver_calloc(sizeof(*tri_z), count / 3);

	// calculate screen space average Z coordinate for each triangle
	for(tri = 0; tri < count / 3; tri++)
	{
		uint32_t i;

		for(i = 0; i < 3; i++)
		{
			if(vertextype == TLVERTEX) tri_z[tri] += vertices[indices[tri * 3 + i]]._.z;
			else
			{
				struct point4d world;
				struct point4d proj;
				struct point4d view;
				transform_point_w(&current_state.world_view_matrix, &vertices[indices[tri * 3 + i]]._, &world);
				transform_point4d(&current_state.d3dprojection_matrix, &world, &proj);
				transform_point4d(&d3dviewport_matrix, &proj, &view);
				tri_z[tri] += view.z / view.w;
			}
		}

		tri_z[tri] /= 3.0f;
	}

	// arrange triangles into layers based on Z coordinates calculated above
	// each layer will be drawn separately
	while(defer_index < count / 3)
	{
		float z = tri_z[defer_index];
		uint32_t tri_num = 0;
		uint32_t defer = num_sorted_deferred;
		uint32_t vert_index = 0;

		for(tri = 0; tri < count / 3; tri++) if(tri_z[tri] == z) tri_num++;

		deferred_sorted_draws[defer].deferred_draw.count = tri_num * 3;
		deferred_sorted_draws[defer].deferred_draw.clip = clip;
		deferred_sorted_draws[defer].deferred_draw.mipmap = mipmap;
		deferred_sorted_draws[defer].deferred_draw.primitivetype = primitivetype;
		deferred_sorted_draws[defer].deferred_draw.vertextype = vertextype;
		deferred_sorted_draws[defer].deferred_draw.vertexcount = tri_num * 3;
		deferred_sorted_draws[defer].deferred_draw.indices = (WORD*)driver_malloc(sizeof(*indices) * tri_num * 3);
		deferred_sorted_draws[defer].deferred_draw.vertices = (nvertex*)driver_malloc(sizeof(*vertices) * tri_num * 3);
		gl_save_state(&deferred_sorted_draws[defer].deferred_draw.state);
		deferred_sorted_draws[defer].drawn = false;
		deferred_sorted_draws[defer].z = z;

		for(tri = 0; tri < count / 3 && vert_index < tri_num * 3; tri++)
		{
			if(tri_z[tri] == z)
			{
				memcpy(&deferred_sorted_draws[defer].deferred_draw.vertices[vert_index + 0], &vertices[indices[tri * 3 + 0]], sizeof(*vertices));
				memcpy(&deferred_sorted_draws[defer].deferred_draw.vertices[vert_index + 1], &vertices[indices[tri * 3 + 1]], sizeof(*vertices));
				memcpy(&deferred_sorted_draws[defer].deferred_draw.vertices[vert_index + 2], &vertices[indices[tri * 3 + 2]], sizeof(*vertices));
				deferred_sorted_draws[defer].deferred_draw.indices[vert_index + 0] = vert_index + 0;
				deferred_sorted_draws[defer].deferred_draw.indices[vert_index + 1] = vert_index + 1;
				deferred_sorted_draws[defer].deferred_draw.indices[vert_index + 2] = vert_index + 2;

				vert_index += 3;

				tri_deferred[tri] = true;
			}
		}

		if(vert_index < tri_num * 3) ffnx_error("deferred draw z mismatch\n");

		num_sorted_deferred++;

		while(defer_index < count / 3 && tri_deferred[defer_index]) defer_index++;
	}

	driver_free(tri_deferred);
	driver_free(tri_z);

	if (trace_all) ffnx_trace("gl_defer_sorted_draw: return true\n");

	return true;
}

// draw deferred models
void gl_draw_deferred(draw_field_shadow_callback shadow_callback)
{
	struct driver_state saved_state;

	bool isFieldShadowDrawn = false;

	if (num_deferred == 0) {
		if (trace_all) ffnx_trace("gl_draw_deferred: num_deferred == 0\n");
		return;
	}

	gl_save_state(&saved_state);

	nodefer = true;

	for (int i = 0; i < num_deferred; ++i)
	{
		if (deferred_draws[i].vertices == nullptr)
		{
			continue;
		}

		if (shadow_callback != nullptr && !isFieldShadowDrawn && deferred_draws[i].vertextype != TLVERTEX)
		{
			(*shadow_callback)();
			isFieldShadowDrawn = true;
		}

		gl_load_state(&deferred_draws[i].state);

		gl_draw_indexed_primitive(deferred_draws[i].primitivetype,
			deferred_draws[i].vertextype,
			deferred_draws[i].vertices,
			deferred_draws[i].normals,
			deferred_draws[i].vertexcount,
			deferred_draws[i].indices,
			deferred_draws[i].count,
			0,
			deferred_draws[i].boundingbox,
			deferred_draws[i].clip,
			deferred_draws[i].mipmap
		);

		++stats.deferred;

		driver_free(deferred_draws[i].vertices);
		deferred_draws[i].vertices = nullptr;
		driver_free(deferred_draws[i].indices);
		deferred_draws[i].indices = nullptr;
		driver_free(deferred_draws[i].normals);
		deferred_draws[i].normals = nullptr;
		driver_free(deferred_draws[i].boundingbox);
		deferred_draws[i].boundingbox = nullptr;
	}

	num_deferred = 0;

	nodefer = false;

	gl_load_state(&saved_state);
}

void gl_set_projection_viewport_matrices()
{
	struct driver_state saved_state;

	gl_save_state(&saved_state);

	for (int i = 0; i < num_deferred; ++i)
	{
		if (deferred_draws[i].vertextype != TLVERTEX)
		{
			gl_load_state(&deferred_draws[i].state);

			newRenderer.setD3DProjection(&current_state.d3dprojection_matrix);
			newRenderer.setD3DViweport(&d3dviewport_matrix);
			break;
		}
	}

	gl_load_state(&saved_state);
}

struct boundingbox calculateSceneAabb()
{
	struct boundingbox sceneAabb;
	sceneAabb.min_x = FLT_MAX;
	sceneAabb.min_y = FLT_MAX;
	sceneAabb.min_z = FLT_MAX;
	sceneAabb.max_x = FLT_MIN;
	sceneAabb.max_y = FLT_MIN;
	sceneAabb.max_z = FLT_MIN;
	for (int i = 0; i < num_deferred; ++i)
	{
		if (deferred_draws[i].vertextype == TLVERTEX)
		{
			continue;
		}
		if (deferred_draws[i].normals == nullptr || deferred_draws[i].count == 3)
		{
			continue;
		}

		struct boundingbox* bb = deferred_draws[i].boundingbox;
		if (bb)
		{
			vector3<float> corners[8] = { {bb->min_x, bb->min_y, bb->min_z},
									   {bb->min_x, bb->min_y, bb->max_z},
			                           {bb->min_x, bb->max_y, bb->min_z},
			                           {bb->min_x, bb->max_y, bb->max_z},
			                           {bb->max_x, bb->min_y, bb->min_z},
			                           {bb->max_x, bb->min_y, bb->max_z},
			                           {bb->max_x, bb->max_y, bb->min_z},
			                           {bb->max_x, bb->max_y, bb->max_z} };

			struct matrix worldViewMatrix = deferred_draws[i].state.world_view_matrix;
			for (int j = 0; j < 8; ++j)
			{
				vector3<float> cornerViewSpace;
				transform_point(&worldViewMatrix, &corners[j], &cornerViewSpace);

				sceneAabb.min_x = std::min(sceneAabb.min_x, cornerViewSpace.x);
				sceneAabb.min_y = std::min(sceneAabb.min_y, cornerViewSpace.y);
				sceneAabb.min_z = std::min(sceneAabb.min_z, cornerViewSpace.z);

				sceneAabb.max_x = std::max(sceneAabb.max_x, cornerViewSpace.x);
				sceneAabb.max_y = std::max(sceneAabb.max_y, cornerViewSpace.y);
				sceneAabb.max_z = std::max(sceneAabb.max_z, cornerViewSpace.z);
			}
		}
	}

	return sceneAabb;
}

// draw all the layers we've accumulated in the correct order and reset queue
void gl_draw_sorted_deferred()
{
	struct driver_state saved_state;

	if (num_sorted_deferred == 0) {
		if (trace_all) ffnx_trace("gl_draw_sorted_deferred: num_sorted_deferred == 0\n");
		return;
	}

	gl_save_state(&saved_state);

	nodefer = true;

	stats.deferred += num_sorted_deferred;

	while(true)
	{
		uint32_t i;
		double z = -1.0;
		uint32_t next = -1;

		for(i = 0; i < num_sorted_deferred; i++)
		{
			if(deferred_sorted_draws[i].z > z && !deferred_sorted_draws[i].drawn)
			{
				next = i;
				z = deferred_sorted_draws[i].z;
			}
		}

		if(next == -1) break;

		gl_load_state(&deferred_sorted_draws[next].deferred_draw.state);
		internal_set_renderstate(V_DEPTHTEST, 1, 0);
		internal_set_renderstate(V_DEPTHMASK, 1, 0);

		gl_draw_indexed_primitive(deferred_sorted_draws[next].deferred_draw.primitivetype,
								  deferred_sorted_draws[next].deferred_draw.vertextype,
								  deferred_sorted_draws[next].deferred_draw.vertices,
								  0,
								  deferred_sorted_draws[next].deferred_draw.vertexcount,
								  deferred_sorted_draws[next].deferred_draw.indices,
								  deferred_sorted_draws[next].deferred_draw.count,
								  0,
								  0,
								  deferred_sorted_draws[next].deferred_draw.clip,
								  deferred_sorted_draws[next].deferred_draw.mipmap
								  );

		driver_free(deferred_sorted_draws[next].deferred_draw.vertices);
		driver_free(deferred_sorted_draws[next].deferred_draw.indices);
		deferred_sorted_draws[next].drawn = true;
	}

	num_sorted_deferred = 0;

	nodefer = false;

	gl_load_state(&saved_state);
}

// a texture is being unloaded, invalidate any pending draw calls associated
// with it and perform the necessary cleanup
void gl_check_deferred(struct texture_set *texture_set)
{
	uint32_t i;

	for (i = 0; i < num_deferred; i++)
	{
		if (deferred_draws[i].state.texture_set == texture_set)
		{
			driver_free(deferred_draws[i].vertices);
			deferred_draws[i].vertices = nullptr;
			driver_free(deferred_draws[i].indices);
			deferred_draws[i].indices = nullptr;
			driver_free(deferred_draws[i].normals);
			deferred_draws[i].normals = nullptr;
			driver_free(deferred_draws[i].boundingbox);
			deferred_draws[i].boundingbox = nullptr;
		}
	}

	for(i = 0; i < num_sorted_deferred; i++)
	{
		if(deferred_sorted_draws[i].deferred_draw.state.texture_set == texture_set)
		{
			driver_free(deferred_sorted_draws[i].deferred_draw.vertices);
			driver_free(deferred_sorted_draws[i].deferred_draw.indices);
			deferred_sorted_draws[i].drawn = true;
		}
	}
}

void gl_cleanup_deferred()
{
	driver_free(deferred_draws);
	driver_free(deferred_sorted_draws);
}

