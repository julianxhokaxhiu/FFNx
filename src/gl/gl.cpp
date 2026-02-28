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

#include "../renderer.h"
#include "../cfg.h"
#include "../gl.h"
#include "../macro.h"
#include "../log.h"
#include "../matrix.h"

#include "../ff7/widescreen.h"
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

void gl_draw_without_lighting(struct indexed_primitive* ip, struct polygon_data *polydata, struct light_data* lightdata, uint32_t clip)
{
	if (enable_external_mesh && polydata->field_48)
	{
		auto externalMesh = reinterpret_cast<ExternalMesh*>(polydata->field_48);
		gl_draw_external_mesh(externalMesh, lightdata);
		return;
	}
	
	static std::vector<vector3<float>> normals;
	if (!ff8 && lightdata != nullptr && game_lighting != GAME_LIGHTING_ORIGINAL)
	{
		gl_calculate_normals(&normals, ip, polydata, lightdata);
		gl_draw_indexed_primitive(ip->primitivetype, ip->vertextype, ip->vertices, normals.data(), ip->vertexcount, ip->indices, ip->indexcount, 0, 0, lightdata, clip, true);
	} else gl_draw_indexed_primitive(ip->primitivetype, ip->vertextype, ip->vertices, nullptr, ip->vertexcount, ip->indices, ip->indexcount, 0, 0, lightdata, clip, true);
}

// draw a set of primitives with lighting
void gl_draw_with_lighting(struct indexed_primitive *ip, struct polygon_data *polydata, struct light_data* lightdata, uint32_t clip)
{
	if (enable_external_mesh && polydata->field_48)
	{
		auto externalMesh = reinterpret_cast<ExternalMesh*>(polydata->field_48);
		gl_draw_external_mesh(externalMesh, lightdata);
		return;
	}

	static std::vector<vector3<float>> normals;
	if (!ff8)
	{
		gl_calculate_normals(&normals, ip, polydata, lightdata);
		gl_draw_indexed_primitive(ip->primitivetype, ip->vertextype, ip->vertices, normals.data(), ip->vertexcount, ip->indices, ip->indexcount, 0, polydata->boundingboxdata, lightdata, clip, true);
	}
	else gl_draw_indexed_primitive(ip->primitivetype, ip->vertextype, ip->vertices, nullptr, ip->vertexcount, ip->indices, ip->indexcount, 0, polydata->boundingboxdata, lightdata, clip, true);
}

// main rendering routine, draws a set of primitives according to the current render state
void gl_draw_indexed_primitive(uint32_t primitivetype, uint32_t vertextype, struct nvertex *vertices, struct vector3<float>* normals, uint32_t vertexcount, WORD *indices, uint32_t count, struct graphics_object *graphics_object, struct boundingbox* boundingbox, struct light_data* lightdata, uint32_t clip, uint32_t mipmap)
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
		ffnx_unexpected_once("vertextype > TLVERTEX\n");
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
	else if(gl_defer_draw(primitivetype, vertextype, vertices, normals, vertexcount, indices, count, boundingbox, lightdata, clip, mipmap))
	{
		current_state.texture_filter = saved_texture_filter;
		return;
	}

	bool isLightingEnabledTexture = true;

	// If we're attaching a texture, and it is an external one, then use a better blending that considers the alpha channel
	if (current_state.texture_set)
	{
		VOBJ(texture_set, texture_set, current_state.texture_set);

		if (VREF(texture_set, ogl.external)) newRenderer.isExternalTexture(true);

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
	newRenderer.isFBTexture(current_state.fb_texture);

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

void gl_draw_external_mesh(ExternalMesh* externalMesh, struct light_data* lightdata)
{	
	if(gl_defer_external_mesh(externalMesh, lightdata)) return;

	byte *level_data = *ff7_externals.field_level_data_pointer;
	if (!level_data)
	{
		return;
	}

	uint32_t model_loader_offset = *(uint32_t *)(level_data + 0x0E);
	auto pScale = (short*)(level_data + model_loader_offset + 0x8);
	auto scale = static_cast<float>((*pScale)) / 128.0f;

	std::array<struct matrix, MAX_BONE_MATRICES> matrix_palette;
    if (externalMesh->skins.size() > 0)
	{
		auto skin = externalMesh->skins[0];
		auto current_frame =  skin.current_frame;
	
		if(externalMesh->animations.contains(skin.current_anim))
		{
			auto anim = externalMesh->animations[skin.current_anim];		

			auto jointCount = skin.joints.size();

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
			auto jointCount = skin.joints.size();
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

	if(!ff8 && lightdata != nullptr && game_lighting != GAME_LIGHTING_ORIGINAL) 
	{
		newRenderer.setGameLightData(lightdata);
	} else newRenderer.setGameLightData(nullptr);

	if (externalMesh->skins.size())
	{
		newRenderer.setSmoothSkinningBoneMatrices(&matrix_palette);

		newRenderer.isSmoothSkinning(true);
		newRenderer.setSmoothSkinningUniforms();
	}

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

	newRenderer.setCommonUniforms();
	if (enable_lighting) newRenderer.setLightingUniforms();

	auto shapeCount = externalMesh->shapes.size();
	int vertexOffset = 0;
	int indexOffset = 0;

	for (int i = 0; i < shapeCount; ++i)
	{
		auto& shape = externalMesh->shapes[i];

		newRenderer.setCullMode(shape.isDoubleSided ? RendererCullMode::DISABLED : RendererCullMode::FRONT);

		externalMesh->bindField3dVertexBuffer(vertexOffset, shape.vertices.size());
		externalMesh->bindField3dIndexBuffer(indexOffset, shape.indices.size());

		if(shape.pMaterial != nullptr)
		{
			if(shape.pMaterial->baseColorTexHandles.size() > 0)
			{
				auto baseColorTexHandle = shape.pMaterial->baseColorTexHandles[shape.pMaterial->texIndex];
				if(bgfx::isValid(baseColorTexHandle))
					newRenderer.useTexture(baseColorTexHandle.idx, RendererTextureSlot::TEX_Y);
				else newRenderer.useTexture(0, RendererTextureSlot::TEX_Y);
			}

			if(shape.pMaterial->normalTexHandles.size() > 0)
			{
				auto normalTexHandle = shape.pMaterial->normalTexHandles[0];
				if(bgfx::isValid(normalTexHandle))
					newRenderer.useTexture(normalTexHandle.idx, RendererTextureSlot::TEX_NML);
				else newRenderer.useTexture(0, RendererTextureSlot::TEX_NML);
			}

			if(shape.pMaterial->pbrTexHandles.size() > 0)
			{
				auto pbrTexHandle = shape.pMaterial->pbrTexHandles[0];
				if(bgfx::isValid(pbrTexHandle))
					newRenderer.useTexture(pbrTexHandle.idx, RendererTextureSlot::TEX_PBR);
				else newRenderer.useTexture(0, RendererTextureSlot::TEX_PBR);
			}

			newRenderer.bindTextures();      
		}

 		if (enable_lighting)
		{
			newRenderer.drawToShadowMap(true, true);
			newRenderer.drawWithLighting(true, true, true);
		}
		newRenderer.draw(true, true, true);

		vertexOffset += shape.vertices.size();
		indexOffset += shape.indices.size();
	}
	
	newRenderer.discardAllBindings();
}
