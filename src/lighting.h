/****************************************************************************/
//    Copyright (C) 2021 Cosmos                                             //
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

#include "matrix.h"
#include "common.h"

#include <vector>

enum TextureDebugOutput
{
    TEXTURE_DEBUG_OUTPUT_DISABLED = 0,
    TEXTURE_DEBUG_OUTPUT_COLOR,
    TEXTURE_DEBUG_OUTPUT_NORMALMAP,
    TEXTURE_DEBUG_OUTPUT_ROUGHNESS,
    TEXTURE_DEBUG_OUTPUT_METALNESS,
};

struct walkmeshEdge
{
    int v0;                      // edge vertex index 0
    int v1;                      // edge vertex index 1
    int ov;                      // opposite vertex index
    int prevEdge;                // previous vertex index
    int nextEdge;	             // next vertex index
    bool isBorder;               // border edge flag
    struct point3d perpDir;      // perpendicular direction
};

struct LightingState
{
    float lightViewMatrix[16];
    float lightProjMatrix[16];
    float lightInvViewProjTexMatrix[16];
    float lightViewProjMatrix[16];
    float lightViewProjTexMatrix[16];

    float lightingSettings[4] = { 1.0, 0.0, 0.0, 0.0 };
    float lightDirData[4] = { 0.3, -1.0, -0.3, 0.0 };
    float lightData[4] = { 1.0, 1.0, 1.0, 4.0 };
    float ambientLightData[4] = { 1.0, 1.0, 1.0, 2.0 };
    float materialData[4] = { 0.3, 0.3, 1.0, 1.0 };
    float shadowData[4] = { 0.001, 0.0, 0.0, 2048.0 };
    float fieldShadowData[4] = { 0.3, 200.0, 100.0, 0.0 };

    float lightingDebugData[4] = { 0.0, 0.0, 0.0, 0.0 };

    // Light Direction
    struct point3d worldLightRot = { 60.0, 60.0, 0.0 };

    // Shadowmap face culling
    bool isShadowMapFaceCullingEnabled = true;

    // Battle shadowmap frustum parameters
    float shadowMapArea = 20000.0f;
    float shadowMapNearFarSize = 20000.0f;

    // Field shadowmap frustum parameters
    float fieldShadowMapArea = 3000.0f;
    float fieldShadowMapNearFarSize = 3000.0f;

    // Field Shadow walkmesh parameters
    float walkMeshExtrudeSize = 20.0;
    float walkMeshPosOffset = -11.0;
};

class Lighting 
{
private:  
    LightingState lightingState;

private:
    void updateLightMatrices(struct boundingbox* sceneAabb);

    void ff7_get_field_view_matrix(struct matrix* outViewMatrix);
    void ff7_create_walkmesh(std::vector<struct nvertex>& vertices, std::vector<WORD>& indices, std::vector<struct walkmeshEdge>& edges);

    void createFieldWalkmesh(std::vector<struct nvertex>& vertices, std::vector<WORD>& indices, float extrudeSize);
    void extractWalkmeshBorderData(std::vector<struct nvertex>& vertices, std::vector<struct walkmeshEdge>& edges);
    void createWalkmeshBorderExtrusionData(std::vector<struct nvertex>& vertices, std::vector<struct walkmeshEdge>& edges);
    void createWalkmeshBorder(std::vector<struct nvertex>& vertices, std::vector<WORD>& indices, std::vector<struct walkmeshEdge>& edges, float extrudeSize);
    struct boundingbox calcFieldSceneAabb(struct boundingbox* sceneAbb, struct matrix* viewMatrix);

public:
    void draw(struct game_obj* game_object);
    void drawFieldShadow();

    const LightingState& getLightingState();

    // Lighting
    void Lighting::setPbrTextureEnabled(bool isEnabled);
    bool Lighting::isPbrTextureEnabled();
    void setWorldLightDir(float dirX, float dirY, float dirZ);
    struct point3d getWorldLightDir();
    void setLightIntensity(float intensity);
    float getLightIntensity();
    void setLightColor(float r, float g, float b);
    point3d getLightColor();
    void setAmbientIntensity(float intensity);
    float getAmbientIntensity();
    void setAmbientLightColor(float r, float g, float b);
    point3d getAmbientLightColor();

    // Material
    void setRoughness(float roughness);
    float getRoughness();
    void setMetalness(float metalness);
    float getMetalness();
    void setRoughnessScale(float roughness);
    float getRoughnessScale();
    void setMetalnessScale(float metalness);
    float getMetalnessScale();

    // Shadow (common)
    void setShadowFaceCullingEnabled(bool isEnabled);
    bool isShadowFaceCullingEnabled();
    void setShadowMapResolution(int size);
    int getShadowMapResolution();
    void setShadowConstantBias(float bias);
    float getShadowConstantBias();

    // Battle Shadow
    void setShadowMapArea(float area);
    float getShadowMapArea();
    void setShadowMapNearFarSize(float size);
    float getShadowMapNearFarSize();

    // Field Shadow
    void setFieldShadowMapArea(float area);
    float getFieldShadowMapArea();
    void setFieldShadowMapNearFarSize(float size);
    float getFieldShadowMapNearFarSize();
    void setFieldShadowOcclusion(float value);
    float getFieldShadowOcclusion();
    void setFieldShadowFadeStartDistance(float value);
    float getFieldShadowFadeStartDistance();
    void setFieldShadowFadeRange(float value);
    float getFieldShadowFadeRange();
    void setWalkmeshExtrudeSize(float size);
    float getWalkmeshExtrudeSize();
    void setWalkmeshPosOffset(float size);
    float getWalkmeshPosOffset();

    // Lighting Debug
    void setHide2dEnabled(bool isEnabled);
    bool isHide2dEnabled();
    void setShowWalkmeshEnabled(bool isEnabled);
    bool isShowWalkmeshEnabled();
    void setTextureDebugOutput(TextureDebugOutput output);
    TextureDebugOutput GetTextureDebugOutput();
};

extern Lighting lighting;
