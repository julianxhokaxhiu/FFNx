/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <array>

#include "../renderer.h"
#include "../cfg.h"
#include "../gl.h"
#include "../macro.h"
#include "../log.h"
#include "../matrix.h"

#include "../ff7/widescreen.h"
#include "../ff7/world/world.h"
#include "../ff7/field/kawai_eyetx.h"
#include "../ff8/texture_packer.h"
#include "../ff8/uv_patch.h"
#include "../ff8/vram.h"
#include "external_mesh.h"

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
	uint32_t cur_game_width = game_width;
	if (!ff8)
	{
		if (widescreen.getMovieMode() == WM_EXTEND_ONLY)
		{
			cur_game_width = wide_game_width;
		}
	}

	float ratio = cur_game_width / (float)width;
	float movieHeight = ratio * height;
	float movieWidth = ratio * width;
	float movieOffsetY = (game_height - movieHeight) / 2.0f;

	if (!ff8 && enable_time_cycle) newRenderer.setTimeFilterEnabled(true);

	if (!ff8 && !ff7_field_center) movieOffsetY = 0.0f;

	float movie_quad_x = 0.0f;
	float movie_quad_y = movieOffsetY;
	float movie_quad_height = movieHeight + movieOffsetY;
	float movie_quad_width = movieWidth;
	if (!ff8)
	{
		if (widescreen.getMovieMode() == WM_FILL)
		{
			movie_quad_x = wide_viewport_x;
			movie_quad_y = 0.0f;
			movie_quad_height = movieHeight + 2 * movieOffsetY;
			movie_quad_width = wide_game_width;
		} else if (widescreen.getMovieMode() == WM_EXTEND_ONLY)
		{
			movie_quad_x = (game_width - movieWidth) / 2.0f;
		}
	}

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
	float x0 = movie_quad_x;
	float y0 = movie_quad_y;
	float u0 = 0.0f;
	float v0 = 0.0f;
	// 1
	float x1 = x0;
	float y1 = movie_quad_height;
	float u1 = u0;
	float v1 = 1.0f;
	// 2
	float x2 = x0 + movie_quad_width;
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

	newRenderer.setYUVMovieBackend();
	newRenderer.bindVertexBuffer(vertices, 0, 4);
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
void gl_load_state(struct driver_state *src, bool bind_textures)
{
	if (bind_textures)
	{
		VOBJ(texture_set, texture_set, src->texture_set);

		memcpy(&current_state, src, sizeof(current_state));

		gl_bind_texture_set(src->texture_set);
		gl_set_texture(src->texture_handle, src->texture_set ? VREF(texture_set, ogl.gl_set) : NULL);
		current_state.texture_set = src->texture_set;
	}

	current_state.kawai_effect = src->kawai_effect;
	current_state.kawai_camera_matrix = src->kawai_camera_matrix;
	current_state.psx_world = src->psx_world;
	current_state.psx_world_radar = src->psx_world_radar;
	current_state.psx_world_meteor = src->psx_world_meteor;
	current_state.psx_world_dither = src->psx_world_dither;
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
	gl_set_worldview_matrix(&src->world_view_matrix);
	gl_set_d3dprojection_matrix(&src->d3dprojection_matrix);
}

void gl_calculate_normals(std::vector<vector3<float>>* pNormals, struct indexed_primitive* ip, struct polygon_data *polydata, struct light_data* lightdata)
{
	bool has_model_data = false;
	static vector3<float> zero = { 0.0f, 0.0f, 0.0f };
	auto& normals = *pNormals;

	normals.resize(ip->vertexcount);
	std::fill(normals.begin(), normals.end(), zero);

	// User wants to attempt to load model data
	if (!prefer_lighting_cpu_calculations)
	{
		// If models do provide normal data, use it
		if (polydata->normaldata != NULL)
		{
			for (uint32_t idx = 0; idx < ip->vertexcount; idx++)
			{
				normals[idx] = polydata->has_normindextable ? polydata->normaldata[polydata->normindextabledata[idx]] : polydata->normaldata[idx];
			}

			has_model_data = true;
		}
	}

	// If the previous code was not able to fetch the model normal data, we have to calculate it on the CPU
	if (!has_model_data)
	{
		vector3<float> e12, e13, triNormal;

		// Calculate vertex normals by averaging adjacent triangle normals
		// Vertex normals are calculated here because battle models dont seem to include normals
		for (uint32_t idx = 0; idx < ip->indexcount; idx+=3)
		{
			e12 = e13 = triNormal = zero;

			int vId0 = ip->indices[idx];
			int vId1 = ip->indices[idx + 1];
			int vId2 = ip->indices[idx + 2];

			auto v1 = &ip->vertices[vId0]._;
			auto v2 = &ip->vertices[vId1]._;
			auto v3 = &ip->vertices[vId2]._;

			subtract_vector(v2, v1, &e12);
			subtract_vector(v3, v1, &e13);
			cross_product(&e13, &e12, &triNormal);

			add_vector(&normals[vId0], &triNormal, &normals[vId0]);
			add_vector(&normals[vId1], &triNormal, &normals[vId1]);
			add_vector(&normals[vId2], &triNormal, &normals[vId2]);
		}

		for (uint32_t idx = 0; idx < ip->vertexcount; idx++)
		{
			normalize_vector(&normals[idx]);
		}
	}
}

namespace
{
	FF8WorldMapStrip ff8_strip_texture()
	{
		if (!current_state.texture_set || current_state.fb_texture) return FF8WorldMapStrip::None;
		VOBJ(texture_set, texture_set, current_state.texture_set);
		if (!VREF(texture_set, tex_header)) return FF8WorldMapStrip::None;
		VOBJ(tex_header, tex_header, VREF(texture_set, tex_header));
		const auto tiled = texturePacker.getTiledTex(VREF(tex_header, image_data));
		if (!tiled.isValid() || tiled.bpp() != Tim::Bpp4 || tiled.x() != 13 * 64 || tiled.y() != 256)
			return FF8WorldMapStrip::None;
		const auto palette = tiled.palette(VREF(texture_set, palette_index));
		if (!palette.isValid() || (palette.x() != 0 && palette.x() != 16) || palette.y() != 240)
			return FF8WorldMapStrip::None;
		const auto strip = palette.x() == 0 ? FF8WorldMapStrip::Rail : FF8WorldMapStrip::Road;
		const int tile_x = tiled.x() + (strip == FF8WorldMapStrip::Road ? 8 : 0);
		for (const auto &texture : texturePacker.matchTextures(tiled))
		{
			const auto &image = texture.texture();
			if (texture.name().starts_with("world/dat/wmset/section39/") &&
				image.x() <= tile_x && image.x() + image.w() >= tile_x + 8 &&
				image.y() <= tiled.y() && image.y() + image.h() >= tiled.y() + 16)
				return strip;
		}
		return FF8WorldMapStrip::None;
	}

	struct PSXWorldDrawScope
	{
		bool saved = current_state.psx_world;
		bool saved_radar = current_state.psx_world_radar;
		bool saved_meteor = current_state.psx_world_meteor;
		PSXWorldDitherMode saved_dither = current_state.psx_world_dither;

		explicit PSXWorldDrawScope(graphics_object* object)
		{
			// Native z-sorting outlives the world draw function; identify terrain at the actual draw.
			// FFNx's own deferred draws have no object and already carry the saved world flags.
			if (object)
			{
				const bool vanilla_world = !ff8 && !enable_worldmap_external_mesh &&
					getmode_cached()->driver_mode == MODE_WORLDMAP;
				current_state.psx_world = vanilla_world && ff7::world::is_terrain_graphics_object(object);
				current_state.psx_world_radar = vanilla_world &&
					static_cast<const void*>(object) == *ff7_externals.world_radar_graphics_object;
				current_state.psx_world_meteor = vanilla_world &&
					static_cast<const void*>(object) == *ff7_externals.world_meteor_graphics_object;
				current_state.psx_world_dither = PSXWorldDitherMode::Off;
				if (current_state.psx_world)
					current_state.psx_world_dither = PSXWorldDitherMode::Average;
				else if (vanilla_world && ff7::world::is_sky_dithering_enabled() &&
					static_cast<const void*>(object) == *ff7_externals.world_sky_graphics_object)
					current_state.psx_world_dither = PSXWorldDitherMode::Average;
			}
		}

		~PSXWorldDrawScope()
		{
			current_state.psx_world = saved;
			current_state.psx_world_radar = saved_radar;
			current_state.psx_world_meteor = saved_meteor;
			current_state.psx_world_dither = saved_dither;
		}
	};

	struct KawaiDrawScope
	{
		ff7::field::kawai::Effect effect = current_state.kawai_effect;
		matrix camera = current_state.kawai_camera_matrix;

		explicit KawaiDrawScope(polygon_data* polygon)
		{
			ff7::field::bind_kawai_effect(ff8 ? nullptr : polygon);
		}

		~KawaiDrawScope()
		{
			current_state.kawai_effect = effect;
			current_state.kawai_camera_matrix = camera;
		}
	};

	bool draw_field_facial_texture(indexed_primitive* ip, polygon_data* polygon, uint32_t clip)
	{
		if (ff8 || !ff7::field::ff7_field_is_facial_primitive(polygon, ip)) return false;

		std::vector<nvertex> vertices(ip->vertices, ip->vertices + ip->vertexcount);
		std::vector<uint32_t> colors;
		colors.reserve(vertices.size());
		for (auto& vertex : vertices)
		{
			vertex.color.r = vertex.color.g = vertex.color.b = 255;
			colors.push_back(vertex.color.color);
		}
		auto facial = *ip;
		facial.vertices = vertices.data();

		// The enclosing KawaiDrawScope restores this; retain non-lighting script effects.
		auto& kind = current_state.kawai_effect.kind;
		if (kind == ff7::field::kawai::Kind::lighting || kind == ff7::field::kawai::Kind::positional_lighting)
			kind = ff7::field::kawai::Kind::none;
		if (!ff7::field::draw_kawai_model(polygon, &facial, {}, nullptr, clip, colors))
			gl_draw_indexed_primitive(facial.primitivetype, facial.vertextype, facial.vertices, nullptr,
				facial.vertexcount, facial.indices, facial.indexcount, nullptr, nullptr, nullptr, clip, true);
		return true;
	}
}

void gl_draw_without_lighting(struct indexed_primitive* ip, struct polygon_data *polydata, struct light_data* lightdata, uint32_t clip)
{
	KawaiDrawScope kawai_scope(polydata);
	if (enable_external_mesh && polydata->field_48)
	{
		auto externalMesh = reinterpret_cast<ExternalMesh*>(polydata->field_48);
		gl_draw_external_mesh(externalMesh, lightdata);
		return;
	}
	if (draw_field_facial_texture(ip, polydata, clip)) return;
	if (!ff8 && ff7::world::draw_model_with_psx_lighting(ip, polydata, clip)) return;
	
	static std::vector<vector3<float>> normals;
	if (!ff8 && current_state.kawai_effect.kind != ff7::field::kawai::Kind::none)
	{
		gl_calculate_normals(&normals, ip, polydata, lightdata);
		ff7::field::draw_kawai_model(polydata, ip, normals, lightdata, clip);
		return;
	}
	if (!ff8 && lightdata != nullptr && game_lighting != GAME_LIGHTING_ORIGINAL)
	{
		gl_calculate_normals(&normals, ip, polydata, lightdata);
		gl_draw_indexed_primitive(ip->primitivetype, ip->vertextype, ip->vertices, normals.data(), ip->vertexcount, ip->indices, ip->indexcount, 0, 0, lightdata, clip, true);
	} else gl_draw_indexed_primitive(ip->primitivetype, ip->vertextype, ip->vertices, nullptr, ip->vertexcount, ip->indices, ip->indexcount, 0, 0, lightdata, clip, true);
}

// draw a set of primitives with lighting
void gl_draw_with_lighting(struct indexed_primitive *ip, struct polygon_data *polydata, struct light_data* lightdata, uint32_t clip)
{
	KawaiDrawScope kawai_scope(polydata);
	if (enable_external_mesh && polydata->field_48)
	{
		auto externalMesh = reinterpret_cast<ExternalMesh*>(polydata->field_48);
		gl_draw_external_mesh(externalMesh, lightdata);
		return;
	}
	if (draw_field_facial_texture(ip, polydata, clip)) return;
	if (!ff8 && ff7::world::draw_model_with_psx_lighting(ip, polydata, clip)) return;

	static std::vector<vector3<float>> normals;
	if (!ff8)
	{
		gl_calculate_normals(&normals, ip, polydata, lightdata);
		if (ff7::field::draw_kawai_model(polydata, ip, normals, lightdata, clip)) return;
		gl_draw_indexed_primitive(ip->primitivetype, ip->vertextype, ip->vertices, normals.data(), ip->vertexcount, ip->indices, ip->indexcount, 0, polydata->boundingboxdata, lightdata, clip, true);
	}
	else gl_draw_indexed_primitive(ip->primitivetype, ip->vertextype, ip->vertices, nullptr, ip->vertexcount, ip->indices, ip->indexcount, 0, polydata->boundingboxdata, lightdata, clip, true);
}

// main rendering routine, draws a set of primitives according to the current render state
void gl_draw_indexed_primitive(uint32_t primitivetype, uint32_t vertextype, struct nvertex *vertices, struct vector3<float>* normals, uint32_t vertexcount, WORD *indices, uint32_t count, struct graphics_object *graphics_object, struct boundingbox* boundingbox, struct light_data* lightdata, uint32_t clip, uint32_t mipmap)
{
	PSXWorldDrawScope world_scope(graphics_object);
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
		ffnx_unexpected_once("vertextype > TLVERTEX\n");
		return;
	}

	std::array<nvertex, 8> sky_vertices;
	std::array<WORD, 18> sky_indices = {0, 1, 2, 1, 3, 2, 2, 3, 4, 3, 5, 4, 4, 5, 6, 5, 7, 6};
	if (!ff8 && !enable_worldmap_external_mesh && (widescreen_enabled || enable_uncrop) &&
		mode == MODE_WORLDMAP && graphics_object &&
		static_cast<const void*>(graphics_object) == *ff7_externals.world_sky_graphics_object)
	{
		const int horizon = *ff7_externals.world_sky_horizon;
		if (vertextype != TLVERTEX || vertexcount != 4 || count != 6 || normals || horizon <= -24)
		{
			ffnx_unexpected_once("Unexpected FF7 world sky gradient geometry\n");
		}
		else
		{
			// Widescreen extends [-24, horizon] to [-44, horizon + 20].
			// Keep that coverage, but place the authored colors at the original edges.
			const float inset = 20.0f / (horizon + 64.0f);
			const auto inset_vertex = [inset](const nvertex& from, const nvertex& to)
			{
				nvertex vertex = from;
				vertex._.x += (to._.x - from._.x) * inset;
				vertex._.y += (to._.y - from._.y) * inset;
				vertex._.z += (to._.z - from._.z) * inset;
				return vertex;
			};
			for (size_t column = 0; column < 2; ++column)
			{
				sky_vertices[column] = vertices[column];
				sky_vertices[2 + column] = inset_vertex(vertices[column], vertices[2 + column]);
				sky_vertices[4 + column] = inset_vertex(vertices[2 + column], vertices[column]);
				sky_vertices[6 + column] = vertices[2 + column];
			}
			vertices = sky_vertices.data();
			indices = sky_indices.data();
			vertexcount = static_cast<uint32_t>(sky_vertices.size());
			count = static_cast<uint32_t>(sky_indices.size());
		}
	}

	newRenderer.setInterpolationQualifier(current_state.shademode ? RendererInterpolationQualifier::SMOOTH : RendererInterpolationQualifier::FLAT);

	std::vector<nvertex> strip_vertices;
	if (ff8 && mode == MODE_WORLDMAP && vertextype == TLVERTEX && primitivetype == PT_TRIANGLES &&
		ff8_worldmap_adjust_strip_uvs(vertices, vertexcount, indices, count, ff8_strip_texture(), strip_vertices))
		vertices = strip_vertices.data();

	// handle some special cases, see special_case.c
	if(gl_special_case(primitivetype, vertextype, vertices, vertexcount, indices, count, graphics_object, clip, mipmap))
	{
		// special cases can signal back to this function that the draw call has
		// been handled in some other manner
		current_state.texture_filter = saved_texture_filter;
		return;
	}
	else if(gl_defer_draw(primitivetype, vertextype, vertices, normals, vertexcount, indices, count, boundingbox, lightdata, clip, mipmap))
	{
		current_state.texture_filter = saved_texture_filter;
		return;
	}

	bool isLightingEnabledTexture = true;
	bool framebufferColorKey = false;

	// If we're attaching a texture, and it is an external one, then use a better blending that considers the alpha channel
	if (current_state.texture_set)
	{
		VOBJ(texture_set, texture_set, current_state.texture_set);

		if (VREF(texture_set, ogl.external)) newRenderer.isExternalTexture(true);

		if (current_state.fb_texture)
		{
			VOBJ(tex_header, tex_header, VREF(texture_set, tex_header));
			framebufferColorKey = VREF(tex_header, color_key) != 0;
		}

		if(enable_lighting && VREF(texture_set, ogl.gl_set->disable_lighting))
		{
			isLightingEnabledTexture = false;
		}
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
	newRenderer.isFBTexture(current_state.fb_texture, framebufferColorKey);

	newRenderer.doModulateAlpha(true);

	newRenderer.isSmoothSkinning(false);

	//// upload vertex data
	newRenderer.bindVertexBuffer(vertices, normals, vertexcount);
	newRenderer.bindIndexBuffer(indices, count);
	newRenderer.setPrimitiveType(RendererPrimitiveType(primitivetype));

	if(!ff8 && lightdata != nullptr && normals != nullptr && game_lighting != GAME_LIGHTING_ORIGINAL)
	{
		newRenderer.setGameLightData(lightdata);
	} else newRenderer.setGameLightData(nullptr);

	if (!ff8 && enable_lighting && normals != nullptr && isLightingEnabledTexture)
	{
		newRenderer.drawToShadowMap();
		newRenderer.drawWithLighting(true, true);
	}
	else newRenderer.draw();

	stats.vertex_count += count;

	current_state.texture_filter = saved_texture_filter;
}

void gl_set_worldview_matrix(struct matrix *matrix)
{
	newRenderer.setWorldViewMatrix(matrix);
	memcpy(&current_state.world_view_matrix, matrix, sizeof(struct matrix));
}

void gl_set_d3dprojection_matrix(struct matrix *matrix)
{
	memcpy(&current_state.d3dprojection_matrix, matrix, sizeof(struct matrix));
}

// apply blend mode to OpenGL state
void gl_set_blend_func(uint32_t blend_mode)
{
	if(trace_all) ffnx_trace("set blend mode %i\n", blend_mode);

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

// The current field's model scale (from its model loader data), or 0 when no field is loaded
float gl_get_field_model_scale()
{
	byte *level_data = *ff7_externals.field_level_data_pointer;
	if (!level_data) return 0.0f;

	uint32_t model_loader_offset = *(uint32_t *)(level_data + 0x0E);
	auto pScale = (short*)(level_data + model_loader_offset + 0x8);
	return static_cast<float>((*pScale)) / 128.0f;
}

// Every texture slot is set for every part, so a part never samples the previous part's textures.
// A part without a base color texture is drawn with its material color instead of being discarded.
static void gl_use_external_mesh_textures(const Shape& shape)
{
	auto pMaterial = shape.pMaterial;
	bgfx::TextureHandle baseColorTexHandle = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle normalTexHandle = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle pbrTexHandle = BGFX_INVALID_HANDLE;
	if(pMaterial != nullptr)
	{
		if(pMaterial->baseColorTexHandles.size() > 0) baseColorTexHandle = pMaterial->baseColorTexHandles[pMaterial->texIndex];
		if(pMaterial->normalTexHandles.size() > 0) normalTexHandle = pMaterial->normalTexHandles[0];
		if(pMaterial->pbrTexHandles.size() > 0) pbrTexHandle = pMaterial->pbrTexHandles[0];
	}
	newRenderer.useTexture(bgfx::isValid(baseColorTexHandle) ? baseColorTexHandle.idx : 0, RendererTextureSlot::TEX_Y);
	newRenderer.useTexture(bgfx::isValid(normalTexHandle) ? normalTexHandle.idx : 0, RendererTextureSlot::TEX_NML);
	newRenderer.useTexture(bgfx::isValid(pbrTexHandle) ? pbrTexHandle.idx : 0, RendererTextureSlot::TEX_PBR);
}

void gl_draw_external_mesh(ExternalMesh* externalMesh, struct light_data* lightdata)
{
	auto shapeCount = externalMesh->shapes.size();
	if (shapeCount == 0) return;

	if(gl_defer_external_mesh(externalMesh, lightdata)) return;
	const bool kawai_active = current_state.kawai_effect.kind != ff7::field::kawai::Kind::none;

	auto scale = gl_get_field_model_scale();
	if (scale == 0.0f)
	{
		return;
	}

	std::array<struct matrix, MAX_BONE_MATRICES> matrix_palette;
	size_t jointCount = 0;
    if (externalMesh->skins.size() > 0)
	{
		auto skin = externalMesh->skins[0];
		auto current_frame =  skin.current_frame;

		// Joints past the bone limit are ignored instead of overflowing matrix_palette
		jointCount = std::min(skin.joints.size(), static_cast<size_t>(MAX_BONE_MATRICES));

		if(externalMesh->animations.contains(skin.current_anim))
		{
			auto anim = externalMesh->animations[skin.current_anim];

			for(int i = 0; i < jointCount; ++i)
			{
				auto& joint = skin.joints[i];

				auto frame = std::min(current_frame, static_cast<int>(anim.keyFrames[i].rotation.size()) - 1);

				float parentMatrix[16];
				bx::mtxScale(parentMatrix, scale);

				if(joint.parentJointIndex != -1)
				{
					auto& parentBone = skin.joints[joint.parentJointIndex];
					memcpy(parentMatrix, parentBone.calculatedMatrix, sizeof(float) * 16);
				}

				float currentTranslationMatrix[16];
				auto currentTranslation = anim.keyFrames[i].translation[frame];
				bx::mtxTranslate(currentTranslationMatrix, currentTranslation.x, currentTranslation.y, currentTranslation.z);

				float currentRotationMatrix[16];
				auto currentRotation = anim.keyFrames[i].rotation[frame];
				bx::Quaternion rotationQuaternion = {currentRotation.x, currentRotation.y, currentRotation.z, -currentRotation.w};
				bx::mtxFromQuaternion(currentRotationMatrix, rotationQuaternion);

				float currentMatrix[16];
				bx::mtxMul(currentMatrix, currentRotationMatrix, currentTranslationMatrix);
			
				float currentGlobalMatrix[16];
				bx::mtxMul(currentGlobalMatrix, currentMatrix, parentMatrix);
				
				float boneMatrix[16];
				bx::mtxMul(boneMatrix, joint.inverseBindPoseMatrix, currentGlobalMatrix);

				memcpy(joint.calculatedMatrix, currentGlobalMatrix, sizeof(float) * 16);
				
				float identityMatrix[16];
				bx::mtxIdentity(identityMatrix);

				memcpy(matrix_palette[i].m[0], boneMatrix, sizeof(float) * 16);
			}
		}
		else
		{
			for(int i = 0; i < jointCount; ++i)
			{			
				float parentMatrix[16];
				bx::mtxScale(parentMatrix, scale);
				memcpy(matrix_palette[i].m[0], parentMatrix, sizeof(float) * 16);
			}
		}
	}
	else
	{
		for(int i = 0; i < MAX_BONE_MATRICES; ++i)
		{			
			float identityMatrix[16];
			bx::mtxIdentity(identityMatrix);
			memcpy(matrix_palette[i].m[0], identityMatrix, sizeof(float) * 16);
		}
	}	

	if(!ff8 && lightdata != nullptr && game_lighting != GAME_LIGHTING_ORIGINAL &&
		(!kawai_active || current_state.kawai_effect.kind == ff7::field::kawai::Kind::clip ||
			ff7::field::kawai::gpu_color_offset(current_state.kawai_effect, true)))
	{
		newRenderer.setGameLightData(lightdata);
	} else newRenderer.setGameLightData(nullptr);

	if (externalMesh->skins.size() && !kawai_active)
	{
		newRenderer.setSmoothSkinningBoneMatrices(&matrix_palette, jointCount);

		newRenderer.isSmoothSkinning(true);
		newRenderer.setSmoothSkinningUniforms();
	}
	else newRenderer.isSmoothSkinning(false);

	newRenderer.setInterpolationQualifier(SMOOTH);
	newRenderer.setPrimitiveType();
	newRenderer.isTLVertex(false);
	newRenderer.doTextureFiltering(true);
	newRenderer.isExternalTexture(true);
	newRenderer.isTexture(true);
	newRenderer.doDepthTest(true);
	newRenderer.doDepthWrite(true);

	struct matrix* pProjMatrix = nullptr;
	if(!ff8)
	{
		struct ff7_game_obj *game_object = (ff7_game_obj *)common_externals.get_game_object();
		if (game_object)
		{
			auto polygon_set = (ff7_polygon_set*)game_object->polygon_set_2EC;
			if(polygon_set)
			{
				auto matrix_set = polygon_set->matrix_set;
				pProjMatrix = matrix_set->matrix_projection;
			}

		}

		if(pProjMatrix != nullptr)
		{
			newRenderer.setD3DProjection(pProjMatrix);
			newRenderer.setD3DViweport(&d3dviewport_matrix);
		}
	}

	// The common and lighting uniforms are set once per model, with the first part's textures picked
	gl_use_external_mesh_textures(externalMesh->shapes[0]);
	newRenderer.setCommonUniforms();
	if (enable_lighting) newRenderer.setLightingUniforms();

	int vertexOffset = 0;
	int indexOffset = 0;

	for (int i = 0; i < shapeCount; ++i)
	{
		auto& shape = externalMesh->shapes[i];

		// drawWithLighting() turns the program into its lighting variant, so every part starts from SMOOTH again
		newRenderer.setInterpolationQualifier(SMOOTH);
		newRenderer.setCullMode(shape.isDoubleSided ? RendererCullMode::DISABLED : RendererCullMode::FRONT);

		ff7::field::KawaiGeometry kawai_geometry;
		if (kawai_active)
		{
			auto vertices = shape.vertices;
			auto normals = shape.normals;
			if (jointCount != 0)
			{
				if (shape.joints.size() != vertices.size() || shape.weights.size() != vertices.size())
				{
					ffnx_error("KAWAI: skinned mesh has inconsistent joint/weight arrays\n");
					continue;
				}
				bool valid = true;
				for (size_t v = 0; v < vertices.size() && valid; ++v)
				{
					const auto& j = shape.joints[v];
					const auto& w = shape.weights[v];
					const std::array<float, 4> joints = {j.x, j.y, j.z, j.w};
					const std::array<float, 4> weights = {w.x, w.y, w.z, w.w};
					vector3<float> position{}, normal{};
					for (size_t component = 0; component < 4; ++component)
					{
						if (!std::isfinite(weights[component]))
						{
							valid = false;
							break;
						}
						if (weights[component] == 0) continue;
						if (!std::isfinite(joints[component]) || joints[component] != std::trunc(joints[component]) ||
							joints[component] < 0 || joints[component] >= jointCount)
						{
							valid = false;
							break;
						}
						auto bone = matrix_palette[static_cast<size_t>(joints[component])];
						vector3<float> transformed;
						transform_point(&bone, &shape.vertices[v]._, &transformed);
						position.x += weights[component] * transformed.x;
						position.y += weights[component] * transformed.y;
						position.z += weights[component] * transformed.z;
						if (v < normals.size())
						{
							bone._41 = bone._42 = bone._43 = 0;
							auto source_normal = shape.normals[v];
							transform_point(&bone, &source_normal, &transformed);
							normal.x += weights[component] * transformed.x;
							normal.y += weights[component] * transformed.y;
							normal.z += weights[component] * transformed.z;
						}
					}
					vertices[v]._ = position;
					if (v < normals.size()) normals[v] = normal;
				}
				if (!valid)
				{
					ffnx_error("KAWAI: skinned mesh has invalid weights or joint indices\n");
					continue;
				}
			}
			std::vector<uint32_t> source_colors;
			source_colors.reserve(shape.vertices.size());
			for (const auto& vertex : shape.vertices) source_colors.push_back(vertex.color.color);
			kawai_geometry = ff7::field::process_kawai_geometry(vertices, normals, shape.indices, source_colors, lightdata);
			if (ff7::field::kawai::gpu_color_offset(current_state.kawai_effect, game_lighting != GAME_LIGHTING_ORIGINAL))
			{
				newRenderer.setGameLightData(kawai_geometry.normals.empty() ? nullptr : lightdata);
				newRenderer.setCommonUniforms();
			}
		}
		else
		{
			externalMesh->bindField3dVertexBuffer(vertexOffset, shape.vertices.size());
			externalMesh->bindField3dIndexBuffer(indexOffset, shape.indices.size());
		}

		gl_use_external_mesh_textures(shape);
		newRenderer.bindTextures();

		// The first part's flags were set with the common uniforms; the others update the ones that depend on textures
		if (i > 0) newRenderer.setTextureFlagsUniforms();

		auto draw_shape = []()
		{
			if (enable_lighting)
			{
				newRenderer.drawToShadowMap(true, true);
				newRenderer.drawWithLighting(true, true, true);
			}
			else newRenderer.draw(true, true, true);
		};
		if (kawai_active)
		{
			for (size_t start = 0; start < kawai_geometry.vertices.size(); start += 65535)
			{
				const auto count = static_cast<uint32_t>(std::min<size_t>(65535, kawai_geometry.vertices.size() - start));
				std::vector<WORD> indices(count);
				for (uint32_t index = 0; index < count; ++index) indices[index] = static_cast<WORD>(index);
				newRenderer.bindVertexBuffer(kawai_geometry.vertices.data() + start,
					kawai_geometry.normals.empty() ? nullptr : kawai_geometry.normals.data() + start, count);
				newRenderer.bindIndexBuffer(indices.data(), count);
				newRenderer.setInterpolationQualifier(SMOOTH);
				draw_shape();
			}
		}
		else draw_shape();

		vertexOffset += shape.vertices.size();
		indexOffset += shape.indices.size();
	}

	newRenderer.discardAllBindings();

	// Don't let the skinned programs leak into whatever is drawn next
	newRenderer.isSmoothSkinning(false);
}
