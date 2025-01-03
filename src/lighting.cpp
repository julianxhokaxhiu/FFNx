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

#include "lighting.h"
#include "gl.h"
#include "globals.h"
#include "renderer.h"
#include "macro.h"
#include "cfg.h"
#include "utils.h"

Lighting lighting;

std::string Lighting::getConfigGroup()
{
	const struct game_mode *mode = getmode_cached();
	std::string ret;
	char *field_name = nullptr;

	switch(mode->driver_mode)
	{
		case MODE_FIELD:
			ret.append("field_");
			field_name = get_current_field_name();
			if (field_name) ret.append(field_name);
			break;
		case MODE_BATTLE:
			ret.append("bat_");
			ret.append(std::to_string(ff7_externals.modules_global_object->battle_id));
			break;
	}

	return ret;
}

auto Lighting::getConfigEntry(char *key)
{
	std::string groupKey = getConfigGroup();

	if (!groupKey.empty() && config.contains(groupKey))
	{
		auto ret = config[groupKey][key];
		if (ret) return ret;
	}

	return config[key];
}

void Lighting::loadConfig()
{
	try
	{
		if (enable_devtools && fileExists(configDevToolsPath))
			config = toml::parse_file(configDevToolsPath);
		else
			config = toml::parse_file(configPath);
	}
	catch (const toml::parse_error &err)
	{
		config = toml::parse("");
	}
}

void Lighting::initParamsFromConfig()
{
	struct game_mode *mode = getmode_cached();

	float lightRotationV = getConfigEntry("light_rotation_vertical").value_or(60.0);
	lightRotationV = std::max(0.0f, std::min(180.0f, lightRotationV));
	float lightRotationH = getConfigEntry("light_rotation_horizontal").value_or(60.0);
	lightRotationH = std::max(0.0f, std::min(360.0f, lightRotationH));
	lighting.setWorldLightDir(lightRotationV, lightRotationH, 0.0f);

	float lightIntensity = getConfigEntry("light_intensity").value_or(4.0);
	lighting.setLightIntensity(lightIntensity);

	toml::array *lightColorArray = getConfigEntry("light_color").as_array();
	if (lightColorArray != nullptr && lightColorArray->size() == 3)
	{
		float r = lightColorArray->get(0)->value<float>().value_or(1.0);
		float g = lightColorArray->get(1)->value<float>().value_or(1.0);
		float b = lightColorArray->get(2)->value<float>().value_or(1.0);
		lighting.setLightColor(r, g, b);
	}

	float ambientLightIntensity = getConfigEntry("ambient_light_intensity").value_or(1.0);
	lighting.setAmbientIntensity(ambientLightIntensity);

	toml::array *ambientLightColorArray = getConfigEntry("ambient_light_color").as_array();
	if (ambientLightColorArray != nullptr && ambientLightColorArray->size() == 3)
	{
		float r = ambientLightColorArray->get(0)->value<float>().value_or(1.0);
		float g = ambientLightColorArray->get(1)->value<float>().value_or(1.0);
		float b = ambientLightColorArray->get(2)->value<float>().value_or(1.0);
		lighting.setAmbientLightColor(r, g, b);
	}

	float roughness = config["material_roughness"].value_or(0.7);
	lighting.setRoughness(roughness);

	float metallic = config["material_metallic"].value_or(0.5);
	lighting.setMetallic(metallic);

	float specular = config["material_specular"].value_or(0.1);
	lighting.setSpecular(specular);

	int shadowMapResolution = config["shadowmap_resolution"].value_or(2048);
	shadowMapResolution = std::max(0, std::min(16384, shadowMapResolution));
	lighting.setShadowMapResolution(shadowMapResolution);

	if (mode->driver_mode == MODE_FIELD)
	{
		float occlusion = getConfigEntry("shadowmap_occlusion").value_or(0.3);
		lighting.setFieldShadowOcclusion(occlusion);

		float area = getConfigEntry("shadowmap_area").value_or(3000.0);
		lighting.setFieldShadowMapArea(area);

		float nearFarSize = getConfigEntry("shadowmap_near_far_size").value_or(3000.0);
		lighting.setFieldShadowMapNearFarSize(nearFarSize);

		float fadeStartDistance = getConfigEntry("shadowmap_fade_start_distance").value_or(1000.0);
		lighting.setFieldShadowFadeStartDistance(fadeStartDistance);

		float fadeStartRange = getConfigEntry("shadowmap_fade_range").value_or(100.0);
		lighting.setFieldShadowFadeRange(fadeStartRange);
	}
}

void Lighting::updateLightMatrices(const vector3<float>& center)
{
	struct game_mode *mode = getmode_cached();

	float rotMatrix[16];
	float degreeToRadian = M_PI / 180.0f;
	bx::mtxRotateXYZ(rotMatrix, lightingState.worldLightRot.x * degreeToRadian,
									 lightingState.worldLightRot.y * degreeToRadian, lightingState.worldLightRot.z * degreeToRadian);

	const float forwardVector[4] = {0.0f, 0.0f, -1.0f, 0.0};
	float worldSpaceLightDir[4] = {0.0f, 0.0f, 0.0f, 0.0};
	bx::vec4MulMtx(worldSpaceLightDir, forwardVector, rotMatrix);

	float area = lightingState.shadowMapArea;
	float nearFarSize = lightingState.shadowMapNearFarSize;

	if (mode->driver_mode == MODE_FIELD)
	{
		// In Field mode the z axis is the up vector so we swap y and z axis
		float tmp = worldSpaceLightDir[1];
		worldSpaceLightDir[1] = worldSpaceLightDir[2];
		worldSpaceLightDir[2] = -tmp;

		area = lightingState.fieldShadowMapArea;
		nearFarSize = lightingState.fieldShadowMapNearFarSize;
	}
	if (mode->driver_mode == MODE_WORLDMAP)
    {
		worldSpaceLightDir[1] *= -1.0f;
        worldSpaceLightDir[2] *= -1.0f;
		worldSpaceLightDir[3] *= -1.0f;
	}

	// Transform light direction into view space
	float viewSpaceLightDir[4];
	bx::vec4MulMtx(viewSpaceLightDir, worldSpaceLightDir, newRenderer.getViewMatrix());

    const bx::Vec3 at = { center.x, center.y, center.z };
    const bx::Vec3 eye = { center.x + viewSpaceLightDir[0],
                           center.y + viewSpaceLightDir[1],
                           center.z + viewSpaceLightDir[2] };

	bx::Vec3 up = {0, 1, 0};
	const bx::Vec3 viewDir = {worldSpaceLightDir[0], worldSpaceLightDir[1], worldSpaceLightDir[2]};
	if (bx::abs(bx::dot(viewDir, up)) > 0.999)
	{
		up = {1.0, 0.0, 0.0};
	}

	// Light view matrix
	bx::mtxLookAt(lightingState.lightViewMatrix, eye, at, up);

	// Light projection matrix
	bx::mtxOrtho(lightingState.lightProjMatrix, -area, area, -area, area,
							 -nearFarSize, nearFarSize, 0.0f, bgfx::getCaps()->homogeneousDepth);

	// Light view projection matrix
	bx::mtxMul(lightingState.lightViewProjMatrix, lightingState.lightViewMatrix, lightingState.lightProjMatrix);

	// Matrix for converting from NDC to texture coordinates
	const float sy = bgfx::getCaps()->originBottomLeft ? 0.5f : -0.5f;
	const float sz = bgfx::getCaps()->homogeneousDepth ? 0.5f : 1.0f;
	const float tz = bgfx::getCaps()->homogeneousDepth ? 0.5f : 0.0f;
	const float mtxCrop[16] =
			{
					0.5f,
					0.0f,
					0.0f,
					0.0f,
					0.0f,
					sy,
					0.0f,
					0.0f,
					0.0f,
					0.0f,
					sz,
					0.0f,
					0.5f,
					0.5f,
					tz,
					1.0f,
			};

	float mtxTmp[16];
	bx::mtxMul(mtxTmp, lightingState.lightProjMatrix, mtxCrop);
	bx::mtxMul(lightingState.lightViewProjTexMatrix, lightingState.lightViewMatrix, mtxTmp);

	// Inverse of all light transformations above
	bx::mtxInverse(lightingState.lightInvViewProjTexMatrix, lightingState.lightViewProjTexMatrix);
}

void Lighting::ff7_load_ibl()
{
	struct game_mode *mode = getmode_cached();
	static uint32_t prev_mode = -1;
	static char filename[64]{0};
	static char specularFullpath[MAX_PATH];
	static char diffuseFullpath[MAX_PATH];
	static WORD last_field_id = 0, last_battle_id = 0;

	switch (mode->driver_mode)
	{
	case MODE_BATTLE:
		if (mode->driver_mode != prev_mode || last_battle_id != ff7_externals.modules_global_object->battle_id)
		{
			last_battle_id = ff7_externals.modules_global_object->battle_id;

			sprintf(filename, "bat_%d", last_battle_id);
			sprintf(specularFullpath, "%s/%s/ibl/%s_s.dds", basedir, external_lighting_path.c_str(), filename);
			sprintf(diffuseFullpath, "%s/%s/ibl/%s_d.dds", basedir, external_lighting_path.c_str(), filename);

			newRenderer.prepareSpecularIbl(specularFullpath);
			newRenderer.prepareDiffuseIbl(diffuseFullpath);
		}
		break;
	case MODE_FIELD:
		if (mode->driver_mode != prev_mode || last_field_id != *ff7_externals.field_id)
		{
			last_field_id = *ff7_externals.field_id;

			sprintf(filename, "field_%d", last_field_id);
			sprintf(specularFullpath, "%s/%s/ibl/%s_s.dds", basedir, external_lighting_path.c_str(), filename);
			sprintf(diffuseFullpath, "%s/%s/ibl/%s_d.dds", basedir, external_lighting_path.c_str(), filename);

			newRenderer.prepareSpecularIbl(specularFullpath);
			newRenderer.prepareDiffuseIbl(diffuseFullpath);
		}
		break;
	default:
		break;
	}

	prev_mode = mode->driver_mode;
}

void Lighting::ff7_create_walkmesh(std::vector<struct walkmeshEdge> &edges)
{
	byte *level_data = *ff7_externals.field_level_data_pointer;
	if (!level_data)
	{
		return;
	}

	uint32_t walkmesh_offset = *(uint32_t *)(level_data + 0x16);

	WORD numTris = *(WORD *)(level_data + walkmesh_offset + 4);

	for (int i = 0; i < numTris; ++i)
	{
		vertex_3s *triangle_data = (vertex_3s *)(level_data + walkmesh_offset + 8 + 24 * i);

		for (int j = 0; j < 3; ++j)
		{
			struct nvertex vertex;
			vertex._.x = triangle_data[j].x;
			vertex._.y = triangle_data[j].y;
			vertex._.z = triangle_data[j].z;
			vertex.color.w = 1.0f;
			vertex.color.r = 0xff;
			vertex.color.g = 0xff;
			vertex.color.b = 0xff;
			vertex.color.a = 0xff;

			vertex.u = 0.0;
			vertex.v = 0.0;

			walkMeshVertices.push_back(vertex);
			walkMeshIndices.push_back(walkMeshIndices.size());
		}

		int vId0 = walkMeshVertices.size() - 3;
		int vId1 = walkMeshVertices.size() - 3 + 1;
		int vId2 = walkMeshVertices.size() - 3 + 2;
		walkmeshEdge e0;
		e0.v0 = vId0;
		e0.v1 = vId1;
		e0.ov = vId2;
		e0.prevEdge = -1;
		e0.nextEdge = -1;
		e0.isBorder = false;
		e0.perpDir = {0.0f, 0.0f, 0.0f};
		walkmeshEdge e1;
		e1.v0 = vId1;
		e1.v1 = vId2;
		e1.ov = vId0;
		e1.prevEdge = -1;
		e1.nextEdge = -1;
		e1.isBorder = false;
		e1.perpDir = {0.0f, 0.0f, 0.0f};
		walkmeshEdge e2;
		e2.v0 = vId2;
		e2.v1 = vId0;
		e2.ov = vId1;
		e2.prevEdge = -1;
		e2.nextEdge = -1;
		e2.isBorder = false;
		e2.perpDir = {0.0f, 0.0f, 0.0f};
		edges.push_back(e0);
		edges.push_back(e1);
		edges.push_back(e2);
	}
}

// creates the field walkmesh for rendering
void Lighting::createFieldWalkmesh(float extrudeSize)
{
	static WORD last_field_id = 0;

	if (*ff7_externals.field_id == last_field_id)
	{
		return;
	}

	walkMeshVertices.clear();
	walkMeshIndices.clear();

	std::vector<struct walkmeshEdge> edges;

	// Get the walkmesh triangles and edges
	ff7_create_walkmesh(edges);

	// Detect triangle edges that are external borders of the walkmesh
	// Border edges will be use to extrude a small area where field shadows will fade out
	// This is done to prevent sharp discontinuities at the walkmesh borders
	extractWalkmeshBorderData(edges);

	// Extract previous and next adjacent border edges
	// Calculate extrude direction for each border edge
	createWalkmeshBorderExtrusionData(edges);

	// Create triangles for the border extrusion
	createWalkmeshBorder(edges, extrudeSize);

	last_field_id = *ff7_externals.field_id;
}

void Lighting::extractWalkmeshBorderData(std::vector<struct walkmeshEdge> &edges)
{
	int numEdges = edges.size();
	for (int i = 0; i < numEdges; ++i)
	{
		auto &e = edges[i];
		vector3<float> pos0 = walkMeshVertices[e.v0]._;
		vector3<float> pos1 = walkMeshVertices[e.v1]._;

		bool isBorder = true;
		for (int j = 0; j < numEdges; ++j)
		{
			if (j == i)
			{
				continue;
			}

			auto &other_e = edges[j];
			vector3<float> other_pos0 = walkMeshVertices[other_e.v0]._;
			vector3<float> other_pos1 = walkMeshVertices[other_e.v1]._;

			float errorMargin = 0.001f;
			if (std::abs(pos0.x - other_pos0.x) < errorMargin && std::abs(pos0.y - other_pos0.y) < errorMargin && std::abs(pos0.z - other_pos0.z) < errorMargin &&
					std::abs(pos1.x - other_pos1.x) < errorMargin && std::abs(pos1.y - other_pos1.y) < errorMargin && std::abs(pos1.z - other_pos1.z) < errorMargin)
			{
				isBorder = false;
			}

			if (std::abs(pos1.x - other_pos0.x) < errorMargin && std::abs(pos1.y - other_pos0.y) < errorMargin && std::abs(pos1.z - other_pos0.z) < errorMargin &&
					std::abs(pos0.x - other_pos1.x) < errorMargin && std::abs(pos0.y - other_pos1.y) < errorMargin && std::abs(pos0.z - other_pos1.z) < errorMargin)
			{
				isBorder = false;
			}
		}
		e.isBorder = isBorder;

		if (isBorder)
		{
			auto &v0 = walkMeshVertices[e.v0];
			auto &v1 = walkMeshVertices[e.v1];
		}
	}
}

void Lighting::createWalkmeshBorderExtrusionData(std::vector<struct walkmeshEdge> &edges)
{
	int numEdges = edges.size();
	for (int i = 0; i < numEdges; ++i)
	{
		auto &e = edges[i];
		if (!e.isBorder)
		{
			continue;
		}

		vector3<float> pos0 = walkMeshVertices[e.v0]._;
		vector3<float> pos1 = walkMeshVertices[e.v1]._;

		for (int j = 0; j < numEdges; ++j)
		{
			if (j == i)
			{
				continue;
			}

			auto &other_e = edges[j];
			if (!other_e.isBorder)
			{
				continue;
			}

			vector3<float> other_pos0 = walkMeshVertices[other_e.v0]._;
			vector3<float> other_pos1 = walkMeshVertices[other_e.v1]._;

			float errorMargin = 0.1f;
			;
			if ((std::abs(pos0.x - other_pos0.x) < errorMargin && std::abs(pos0.y - other_pos0.y) < errorMargin && std::abs(pos0.z - other_pos0.z) < errorMargin) ||
					(std::abs(pos0.x - other_pos1.x) < errorMargin && std::abs(pos0.y - other_pos1.y) < errorMargin && std::abs(pos0.z - other_pos1.z) < errorMargin))
			{
				e.prevEdge = j;
			}

			if ((std::abs(pos1.x - other_pos0.x) < errorMargin && std::abs(pos1.y - other_pos0.y) < errorMargin && std::abs(pos1.z - other_pos0.z) < errorMargin) ||
					(std::abs(pos1.x - other_pos1.x) < errorMargin && std::abs(pos1.y - other_pos1.y) < errorMargin && std::abs(pos1.z - other_pos1.z) < errorMargin))
			{
				e.nextEdge = j;
			}

			vector3<float> pos0 = walkMeshVertices[e.v0]._;
			vector3<float> pos1 = walkMeshVertices[e.v1]._;
			vector3<float> ovPos = walkMeshVertices[e.ov]._;

			vector3<float> triCenter;
			add_vector(&pos0, &pos1, &triCenter);
			add_vector(&triCenter, &ovPos, &triCenter);
			divide_vector(&triCenter, 2.0f, &triCenter);

			vector3<float> edgeDir0;
			subtract_vector(&pos1, &pos0, &edgeDir0);
			normalize_vector(&edgeDir0);
			vector3<float> edgeDir1;
			subtract_vector(&ovPos, &pos0, &edgeDir1);
			normalize_vector(&edgeDir1);
			vector3<float> normal;
			cross_product(&edgeDir0, &edgeDir1, &normal);

			vector3<float> perpDir;
			cross_product(&edgeDir0, &normal, &perpDir);
			normalize_vector(&perpDir);

			vector3<float> ovDir0;
			subtract_vector(&pos0, &ovPos, &ovDir0);
			normalize_vector(&ovDir0);

			vector3<float> ovDir1;
			subtract_vector(&pos1, &ovPos, &ovDir1);
			normalize_vector(&ovDir1);

			if (dot_product(&ovDir0, &perpDir) < 0.0)
			{
				multiply_vector(&perpDir, -1.0f, &perpDir);
			}

			e.perpDir = perpDir;
		}
	}
}

void Lighting::createWalkmeshBorder(std::vector<struct walkmeshEdge> &edges, float extrudeSize)
{
	int numEdges = edges.size();
	for (int i = 0; i < numEdges; ++i)
	{
		auto &e = edges[i];
		if (e.isBorder == false)
		{
			continue;
		}

		vector3<float> pos0 = walkMeshVertices[e.v0]._;
		vector3<float> pos1 = walkMeshVertices[e.v1]._;

		if (e.prevEdge == -1 || e.nextEdge == -1)
			continue;

		auto &prevEdge = edges[e.prevEdge];
		auto &nextEdge = edges[e.nextEdge];

		vector3<float> capExtrudeDir0;
		add_vector(&e.perpDir, &prevEdge.perpDir, &capExtrudeDir0);
		normalize_vector(&capExtrudeDir0);

		vector3<float> capExtrudeDir1;
		add_vector(&e.perpDir, &nextEdge.perpDir, &capExtrudeDir1);
		normalize_vector(&capExtrudeDir1);

		// Extrude triangle 0
		vector3<float> extrudePos0;
		{
			vector3<float> perpDir = {capExtrudeDir0.x, capExtrudeDir0.y, capExtrudeDir0.z};
			float cos = dot_product(&e.perpDir, &capExtrudeDir0);

			vector3<float> extrudeOffset;
			multiply_vector(&perpDir, extrudeSize / cos, &extrudeOffset);

			add_vector(&pos0, &extrudeOffset, &extrudePos0);

			struct nvertex v0;
			v0._.x = pos1.x;
			v0._.y = pos1.y;
			v0._.z = pos1.z;
			v0.color.w = 1.0f;
			v0.color.r = 0xff;
			v0.color.g = 0x00;
			v0.color.b = 0x00;
			v0.color.a = 0xff;
			walkMeshVertices.push_back(v0);
			walkMeshIndices.push_back(walkMeshIndices.size());

			struct nvertex v1;
			v1._.x = pos0.x;
			v1._.y = pos0.y;
			v1._.z = pos0.z;
			v1.color.w = 1.0f;
			v1.color.r = 0xff;
			v1.color.g = 0x00;
			v1.color.b = 0x00;
			v1.color.a = 0xff;

			walkMeshVertices.push_back(v1);
			walkMeshIndices.push_back(walkMeshIndices.size());

			struct nvertex v2;
			v2._.x = extrudePos0.x;
			v2._.y = extrudePos0.y;
			v2._.z = extrudePos0.z;
			v2.color.w = 1.0f;
			v2.color.r = 0xff;
			v2.color.g = 0x00;
			v2.color.b = 0x00;
			v2.color.a = 0x00;

			walkMeshVertices.push_back(v2);
			walkMeshIndices.push_back(walkMeshIndices.size());
		}

		// Extrude triangle 1
		vector3<float> extrudePos1;
		{
			vector3<float> perpDir = {capExtrudeDir1.x, capExtrudeDir1.y, capExtrudeDir1.z};
			float cos = dot_product(&e.perpDir, &capExtrudeDir1);

			vector3<float> extrudeOffset;
			multiply_vector(&perpDir, extrudeSize / cos, &extrudeOffset);

			add_vector(&pos1, &extrudeOffset, &extrudePos1);

			struct nvertex v0;
			v0._.x = extrudePos1.x;
			v0._.y = extrudePos1.y;
			v0._.z = extrudePos1.z;
			v0.color.w = 1.0f;
			v0.color.r = 0xff;
			v0.color.g = 0x00;
			v0.color.b = 0x00;
			v0.color.a = 0x00;

			walkMeshVertices.push_back(v0);
			walkMeshIndices.push_back(walkMeshIndices.size());

			struct nvertex v1;
			v1._.x = pos1.x;
			v1._.y = pos1.y;
			v1._.z = pos1.z;
			v1.color.w = 1.0f;
			v1.color.r = 0xff;
			v1.color.g = 0x00;
			v1.color.b = 0x00;
			v1.color.a = 0xff;

			walkMeshVertices.push_back(v1);
			walkMeshIndices.push_back(walkMeshIndices.size());

			struct nvertex v2;
			v2._.x = extrudePos0.x;
			v2._.y = extrudePos0.y;
			v2._.z = extrudePos0.z;
			v2.color.w = 1.0f;
			v2.color.r = 0xff;
			v2.color.g = 0x00;
			v2.color.b = 0x00;
			v2.color.a = 0x00;

			walkMeshVertices.push_back(v2);
			walkMeshIndices.push_back(walkMeshIndices.size());
		}
	}
}

struct boundingbox Lighting::calcFieldSceneAabb(struct boundingbox *sceneAabb)
{
	byte *level_data = *ff7_externals.field_level_data_pointer;
	if (!level_data)
	{
		return *sceneAabb;
	}

	uint32_t walkmesh_offset = *(uint32_t *)(level_data + 0x16);

	std::vector<struct nvertex> vertices;
	std::vector<WORD> indices;
	std::vector<struct walkmeshEdge> edges;

	WORD numTris = *(WORD *)(level_data + walkmesh_offset + 4);

	vector3<float> boundingMin = {FLT_MAX, FLT_MAX, FLT_MAX};
	vector3<float> boundingMax = {FLT_MIN, FLT_MIN, FLT_MIN};

	// Calculates walkmesh AABB
	for (int i = 0; i < numTris; ++i)
	{
		vertex_3s *triangle_data = (vertex_3s *)(level_data + walkmesh_offset + 8 + 24 * i);

		for (int j = 0; j < 3; ++j)
		{
			boundingMin.x = std::min(boundingMin.x, static_cast<float>(triangle_data[j].x));
			boundingMin.y = std::min(boundingMin.y, static_cast<float>(triangle_data[j].y));
			boundingMin.z = std::min(boundingMin.z, static_cast<float>(triangle_data[j].z));
			boundingMax.x = std::max(boundingMax.x, static_cast<float>(triangle_data[j].x));
			boundingMax.y = std::max(boundingMax.y, static_cast<float>(triangle_data[j].y));
			boundingMax.z = std::max(boundingMax.z, static_cast<float>(triangle_data[j].z));
		}
	}

	// Calculates walkmesh AABB in view space
	struct boundingbox bb;
	bb.min_x = FLT_MAX;
	bb.min_y = FLT_MAX;
	bb.min_z = FLT_MAX;
	bb.max_x = FLT_MIN;
	bb.max_y = FLT_MIN;
	bb.max_z = FLT_MIN;

	vector3<float> corners[8] = {{boundingMin.x, boundingMin.y, boundingMin.z},
															 {boundingMin.x, boundingMin.y, boundingMax.z},
															 {boundingMin.x, boundingMax.y, boundingMin.z},
															 {boundingMin.x, boundingMax.y, boundingMax.z},
															 {boundingMax.x, boundingMin.y, boundingMin.z},
															 {boundingMax.x, boundingMin.y, boundingMax.z},
															 {boundingMax.x, boundingMax.y, boundingMin.z},
															 {boundingMax.x, boundingMax.y, boundingMax.z}};

	for (int j = 0; j < 8; ++j)
	{
		vector3<float> cornerViewSpace;
		struct matrix viewMatrix;
		::memcpy(&viewMatrix.m[0][0], newRenderer.getViewMatrix(), sizeof(viewMatrix.m));
		transform_point(&viewMatrix, &corners[j], &cornerViewSpace);

		bb.min_x = std::min(bb.min_x, cornerViewSpace.x);
		bb.min_y = std::min(bb.min_y, cornerViewSpace.y);
		bb.min_z = std::min(bb.min_z, cornerViewSpace.z);

		bb.max_x = std::max(bb.max_x, cornerViewSpace.x);
		bb.max_y = std::max(bb.max_y, cornerViewSpace.y);
		bb.max_z = std::max(bb.max_z, cornerViewSpace.z);
	}

	bb.min_x = std::min(bb.min_x, sceneAabb->min_x);
	bb.min_y = std::min(bb.min_y, sceneAabb->min_y);
	bb.min_z = std::min(bb.min_z, sceneAabb->min_z);

	bb.max_x = std::max(bb.max_x, sceneAabb->max_x);
	bb.max_y = std::max(bb.max_y, sceneAabb->max_y);
	bb.max_z = std::max(bb.max_z, sceneAabb->max_z);

	return bb;
}

void Lighting::init()
{
	sprintf(configPath, "%s/%s/config.toml", basedir, external_lighting_path.c_str());
	sprintf(configDevToolsPath, "%s/%s/config.devtools.toml", basedir, external_lighting_path.c_str());

	reload();
}

void Lighting::reload()
{
	loadConfig();
	initParamsFromConfig();
}

void Lighting::save()
{
	std::ofstream ofs(configDevToolsPath);
	ofs << config << std::endl;
}

void Lighting::draw(struct game_obj *game_object)
{
	VOBJ(game_obj, game_object, game_object);
	struct game_mode *mode = getmode_cached();
	static WORD last_field_id = 0, last_battle_id = 0;

	ff7_load_ibl();

	struct boundingbox sceneAabb = calculateSceneAabb();

	switch(mode->driver_mode)
	{
		case MODE_FIELD:
		{
			// Refresh the configuration if we're on a new field
			if (last_field_id != *ff7_externals.field_id)
			{
				last_field_id = *ff7_externals.field_id;
				initParamsFromConfig();
			}

			struct boundingbox fieldSceneAabb = calcFieldSceneAabb(&sceneAabb);

			// Light view frustum pointing to scene AABB center
			vector3<float> center = {
				0.5f * (fieldSceneAabb.min_x + fieldSceneAabb.max_x),
				0.5f * (fieldSceneAabb.min_y + fieldSceneAabb.max_y),
				0.5f * (fieldSceneAabb.min_z + fieldSceneAabb.max_z) };

				float viewSpaceCenter[4];
	float centerFloat[4];
	centerFloat[0] = center.x;
	centerFloat[1] = center.y;
	centerFloat[2] = center.z;
	centerFloat[3] = 1;
	bx::vec4MulMtx(viewSpaceCenter, centerFloat, newRenderer.getViewMatrix());
			updateLightMatrices(center);
			gl_draw_deferred(&drawFieldShadowCallback);
			break;
		}
		case MODE_WORLDMAP:
		{
			int world_pos_x = ff7_externals.world_player_pos_E04918->x;
			int world_pos_y = ff7_externals.world_player_pos_E04918->y;
			int world_pos_z = ff7_externals.world_player_pos_E04918->z;

			// Light view frustum pointing to player position
			vector3<float> center = {
				static_cast<float>(world_pos_x),
				static_cast<float>(world_pos_y),
				static_cast<float>(world_pos_z)};

			struct matrix viewMatrix;
    		::memcpy(&viewMatrix.m[0][0], newRenderer.getViewMatrix(), sizeof(viewMatrix.m));
			vector3<float> centerViewSpace;
			transform_point(&viewMatrix, &center, &centerViewSpace);

			updateLightMatrices(centerViewSpace);
			gl_draw_deferred(nullptr);
		}
		break;
		case MODE_BATTLE:
		{
			// Refresh the configuration if we're on a new battle
			if (last_battle_id != ff7_externals.modules_global_object->battle_id)
			{
				last_battle_id = ff7_externals.modules_global_object->battle_id;
				initParamsFromConfig();
			}
		}
		default:
		{
			// Light view frustum pointing to scene AABB center
			vector3<float> center = {
				0.5f * (sceneAabb.min_x + sceneAabb.max_x),
				0.5f * (sceneAabb.min_y + sceneAabb.max_y),
				0.5f * (sceneAabb.min_z + sceneAabb.max_z) };

			updateLightMatrices(center);
			gl_draw_deferred(nullptr);
			break;
		}
	}
}

void drawFieldShadowCallback()
{
	lighting.createFieldWalkmesh(lighting.getWalkmeshExtrudeSize());

	auto walkMeshVertices = lighting.getWalkmeshVertices();
	auto walkMeshIndices = lighting.getWalkmeshIndices();

	newRenderer.bindVertexBuffer(walkMeshVertices.data(), 0, walkMeshVertices.size());
	newRenderer.bindIndexBuffer(walkMeshIndices.data(), walkMeshIndices.size());

	newRenderer.setPrimitiveType();
	newRenderer.isTLVertex(false);
	newRenderer.setCullMode(RendererCullMode::BACK);
	newRenderer.setBlendMode(RendererBlendMode::BLEND_AVG);
	newRenderer.isFBTexture(false);
	newRenderer.doDepthTest(true);
	newRenderer.doDepthWrite(false);

	// Create a world matrix
	struct matrix worldMatrix;
	identity_matrix(&worldMatrix);

	// WalkMesh offset to adjust vertical position of walkmesh so that it is just under the character feets
	worldMatrix._43 = lighting.getWalkmeshPosOffset();

	// View matrix
	struct matrix viewMatrix;
	memcpy(&viewMatrix.m[0][0], newRenderer.getViewMatrix(), sizeof(viewMatrix.m));

	// Create a world view matrix
	struct matrix worldViewMatrix;
	multiply_matrix(&worldMatrix, &viewMatrix, &worldViewMatrix);
	newRenderer.setWorldViewMatrix(&worldViewMatrix);

	newRenderer.drawFieldShadow();
}

const LightingState &Lighting::getLightingState()
{
	return lightingState;
}

void Lighting::setPbrTextureEnabled(bool isEnabled)
{
	lightingState.lightingSettings[0] = isEnabled;
}

bool Lighting::isPbrTextureEnabled()
{
	return lightingState.lightingSettings[0];
}

void Lighting::setEnvironmentLightingEnabled(bool isEnabled)
{
	lightingState.lightingSettings[1] = isEnabled;
}

bool Lighting::isEnvironmentLightingEnabled()
{
	return lightingState.lightingSettings[1];
}

void Lighting::setWorldLightDir(float dirX, float dirY, float dirZ)
{
	lightingState.worldLightRot.x = dirX;
	lightingState.worldLightRot.y = dirY;
	lightingState.worldLightRot.z = dirZ;
}

vector3<float> Lighting::getWorldLightDir()
{
	return lightingState.worldLightRot;
}

void Lighting::setLightIntensity(float intensity)
{
	lightingState.lightData[3] = intensity;
}

float Lighting::getLightIntensity()
{
	return lightingState.lightData[3];
}

void Lighting::setLightColor(float r, float g, float b)
{
	lightingState.lightData[0] = r;
	lightingState.lightData[1] = g;
	lightingState.lightData[2] = b;
}

vector3<float> Lighting::getLightColor()
{
	vector3<float> color = {lightingState.lightData[0],
													lightingState.lightData[1],
													lightingState.lightData[2]};
	return color;
}

void Lighting::setAmbientIntensity(float intensity)
{
	lightingState.ambientLightData[3] = intensity;
}

float Lighting::getAmbientIntensity()
{
	return lightingState.ambientLightData[3];
}

void Lighting::setAmbientLightColor(float r, float g, float b)
{
	lightingState.ambientLightData[0] = r;
	lightingState.ambientLightData[1] = g;
	lightingState.ambientLightData[2] = b;
}

vector3<float> Lighting::getAmbientLightColor()
{
	vector3<float> color = {lightingState.ambientLightData[0],
													lightingState.ambientLightData[1],
													lightingState.ambientLightData[2]};
	return color;
}

void Lighting::setIblMipCount(int mipCount)
{
	lightingState.iblData[0] = mipCount;
}

bool Lighting::isDisabledLightingTexture(const std::string &textureName)
{
	toml::array *disabledTextures = config["disable_lighting_textures"].as_array();
	if (disabledTextures && !disabledTextures->empty() && disabledTextures->is_homogeneous(toml::node_type::string))
	{
		int count = disabledTextures->size();
		for (int i = 0; i < count; ++i)
		{
			auto disabledTextureName = disabledTextures->get(i)->value<std::string>();
			if (disabledTextureName.has_value() && textureName == disabledTextureName.value())
			{
				return true;
			}
		}
	}

	return false;
}

void Lighting::setRoughness(float roughness)
{
	lightingState.materialData[0] = roughness;
}

float Lighting::getRoughness()
{
	return lightingState.materialData[0];
}

void Lighting::setMetallic(float metallic)
{
	lightingState.materialData[1] = metallic;
}

float Lighting::getMetallic()
{
	return lightingState.materialData[1];
}

void Lighting::setSpecular(float specular)
{
	lightingState.materialData[2] = specular;
}

float Lighting::getSpecular()
{
	return lightingState.materialData[2];
}

void Lighting::setRoughnessScale(float scale)
{
	lightingState.materialScaleData[0] = scale;
}

float Lighting::getRoughnessScale()
{
	return lightingState.materialScaleData[0];
}

void Lighting::setMetallicScale(float scale)
{
	lightingState.materialScaleData[1] = scale;
}

float Lighting::getMetallicScale()
{
	return lightingState.materialScaleData[1];
}

void Lighting::setSpecularScale(float scale)
{
	lightingState.materialScaleData[2] = scale;
}

float Lighting::getSpecularScale()
{
	return lightingState.materialScaleData[2];
}

void Lighting::setShadowFaceCullingEnabled(bool isEnabled)
{
	lightingState.isShadowMapFaceCullingEnabled = isEnabled;
}

bool Lighting::isShadowFaceCullingEnabled()
{
	return lightingState.isShadowMapFaceCullingEnabled;
}

void Lighting::setShadowMapResolution(int resolution)
{
	lightingState.shadowData[3] = resolution;
	newRenderer.prepareShadowMap();
}

int Lighting::getShadowMapResolution()
{
	return lightingState.shadowData[3];
}

void Lighting::setShadowConstantBias(float bias)
{
	lightingState.shadowData[0] = bias;
}

float Lighting::getShadowConstantBias()
{
	return lightingState.shadowData[0];
}

void Lighting::setShadowMapArea(float area)
{
	lightingState.shadowMapArea = area;
}

float Lighting::getShadowMapArea()
{
	return lightingState.shadowMapArea;
}

void Lighting::setShadowMapNearFarSize(float size)
{
	lightingState.shadowMapNearFarSize = size;
}

float Lighting::getShadowMapNearFarSize()
{
	return lightingState.shadowMapNearFarSize;
}

void Lighting::setFieldShadowMapArea(float area)
{
	lightingState.fieldShadowMapArea = area;
}

float Lighting::getFieldShadowMapArea()
{
	return lightingState.fieldShadowMapArea;
}

void Lighting::setFieldShadowMapNearFarSize(float size)
{
	lightingState.fieldShadowMapNearFarSize = size;
}

float Lighting::getFieldShadowMapNearFarSize()
{
	return lightingState.fieldShadowMapNearFarSize;
}

void Lighting::setFieldShadowOcclusion(float value)
{
	lightingState.fieldShadowData[0] = value;
}

float Lighting::getFieldShadowOcclusion()
{
	return lightingState.fieldShadowData[0];
}

void Lighting::setFieldShadowFadeStartDistance(float value)
{
	lightingState.fieldShadowData[1] = value;
}

float Lighting::getFieldShadowFadeStartDistance()
{
	return lightingState.fieldShadowData[1];
}

void Lighting::setFieldShadowFadeRange(float value)
{
	lightingState.fieldShadowData[2] = value;
}

float Lighting::getFieldShadowFadeRange()
{
	return lightingState.fieldShadowData[2];
}

void Lighting::setWalkmeshExtrudeSize(float size)
{
	lightingState.walkMeshExtrudeSize = size;
}

float Lighting::getWalkmeshExtrudeSize()
{
	return lightingState.walkMeshExtrudeSize;
}

void Lighting::setWalkmeshPosOffset(float offset)
{
	lightingState.walkMeshPosOffset = offset;
}

float Lighting::getWalkmeshPosOffset()
{
	return lightingState.walkMeshPosOffset;
}

const std::vector<nvertex> &Lighting::getWalkmeshVertices()
{
	return walkMeshVertices;
}

const std::vector<WORD> &Lighting::getWalkmeshIndices()
{
	return walkMeshIndices;
}

void Lighting::setHide2dEnabled(bool isEnabled)
{
	lightingState.lightingDebugData[0] = isEnabled;
}

bool Lighting::isHide2dEnabled()
{
	return lightingState.lightingDebugData[0];
}

void Lighting::setShowWalkmeshEnabled(bool isEnabled)
{
	lightingState.lightingDebugData[1] = isEnabled;
}

bool Lighting::isShowWalkmeshEnabled()
{
	return lightingState.lightingDebugData[1];
}

void Lighting::setDebugOutput(DebugOutput output)
{
	lightingState.lightingDebugData[2] = output;
}

DebugOutput Lighting::GetDebugOutput()
{
	return static_cast<DebugOutput>(lightingState.lightingDebugData[2]);
}
