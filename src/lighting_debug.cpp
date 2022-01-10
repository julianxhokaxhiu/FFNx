/****************************************************************************/
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

#include "lighting_debug.h"
#include "lighting.h"
#include "cfg.h"

#include <imgui.h>
#include <math.h>

void lighting_debug(bool* isOpen)
{
    if (!ImGui::Begin("Lighting Debug", isOpen, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    bool isLightingEnabled = enable_lighting;
    if (ImGui::Checkbox("Enable Lighting", &isLightingEnabled))
    {
        enable_lighting = isLightingEnabled;
    }
    bool isPbrTexturesEnabled = lighting.isPbrTextureEnabled();
    if (ImGui::Checkbox("Enable PBR Textures", &isPbrTexturesEnabled))
    {
        lighting.setPbrTextureEnabled(isPbrTexturesEnabled);
    }
    bool isEnvironmentLightingEnabled = lighting.isEnvironmentLightingEnabled();
    if (ImGui::Checkbox("Enable Environment Lighting", &isEnvironmentLightingEnabled))
    {
        lighting.setEnvironmentLightingEnabled(isEnvironmentLightingEnabled);
    }
    if (ImGui::CollapsingHeader("Direct Lighting", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth))
    {
        point3d lightDirVector = lighting.getWorldLightDir();
        float lightRotation[3] = { lightDirVector.x , lightDirVector.y };
        if (ImGui::DragFloat2("Light Rotation", lightRotation, 1.0f))
        {
            lightRotation[0] = std::max(0.0f, std::min(180.0f, lightRotation[0]));
            lightRotation[1] = std::max(0.0f, std::min(360.0f, lightRotation[1]));
            lighting.setWorldLightDir(lightRotation[0], lightRotation[1], 0.0f);
        }
        float lightIntensity = lighting.getLightIntensity();
        if (ImGui::DragFloat("Intensity##0", &lightIntensity, 0.01f, 0.0f, 100.0f))
        {
            lighting.setLightIntensity(lightIntensity);
        }
        struct point3d lightColorPoint3d = lighting.getLightColor();
        float lightColor[3] = { lightColorPoint3d.x, lightColorPoint3d.y, lightColorPoint3d.z };
        if (ImGui::ColorEdit3("Color##0", lightColor))
        {
            lighting.setLightColor(lightColor[0], lightColor[1], lightColor[2]);
        }
    }
    if (ImGui::CollapsingHeader("Indirect Lighting", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth))
    {
        float ambientIntensity = lighting.getAmbientIntensity();
        if (ImGui::DragFloat("Intensity##1", &ambientIntensity, 0.01f, 0.0f, 100.0f))
        {
            lighting.setAmbientIntensity(ambientIntensity);
        }

        struct point3d ambientLightColorPoint3d = lighting.getAmbientLightColor();
        float ambientLightColor[3] = { ambientLightColorPoint3d.x, ambientLightColorPoint3d.y, ambientLightColorPoint3d.z };
        if (ImGui::ColorEdit3("Color##1", ambientLightColor))
        {
            lighting.setAmbientLightColor(ambientLightColor[0], ambientLightColor[1], ambientLightColor[2]);
        }
    }
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth))
    {
        float roughness = lighting.getRoughness();
        if (ImGui::DragFloat("Roughness", &roughness, 0.01f, 0.0f, 1.0f))
        {
            lighting.setRoughness(roughness);
        }
        float metallic = lighting.getMetallic();
        if (ImGui::DragFloat("Metallic", &metallic, 0.01f, 0.0f, 1.0f))
        {
            lighting.setMetallic(metallic);
        }
        float specular = lighting.getSpecular();
        if (ImGui::DragFloat("Specular", &specular, 0.01f, 0.0f, 1.0f))
        {
            lighting.setSpecular(specular);
        }
        float roughnessScale = lighting.getRoughnessScale();
        if (ImGui::DragFloat("Roughness Scale", &roughnessScale, 0.01f, 0.0f, 2.0f))
        {
            lighting.setRoughnessScale(roughnessScale);
        }
        float metallicScale = lighting.getMetallicScale();
        if (ImGui::DragFloat("Metallic Scale", &metallicScale, 0.01f, 0.0f, 2.0f))
        {
            lighting.setMetallicScale(metallicScale);
        }
        float specularScale = lighting.getSpecularScale();
        if (ImGui::DragFloat("Specular Scale", &specularScale, 0.01f, 0.0f, 2.0f))
        {
            lighting.setSpecularScale(specularScale);
        }
    }
    if (ImGui::CollapsingHeader("Shadow map (common)", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth))
    {
        bool isShadowFaceCullingEnabled = lighting.isShadowFaceCullingEnabled();
        if (ImGui::Checkbox("Face culling", &isShadowFaceCullingEnabled))
        {
            lighting.setShadowFaceCullingEnabled(isShadowFaceCullingEnabled);
        }
        int shadowMapResolution = lighting.getShadowMapResolution();
        if (ImGui::SliderInt("Resolution", &shadowMapResolution, 512, 4096))
        {
            lighting.setShadowMapResolution(shadowMapResolution);
        }
        float shadowConstantBias = lighting.getShadowConstantBias();
        if (ImGui::DragFloat("Constant Bias", &shadowConstantBias, 0.001f, 0.0f, 1.0f))
        {
            lighting.setShadowConstantBias(shadowConstantBias);
        }
    }
    if (ImGui::CollapsingHeader("Battle shadow map", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth))
    {
        float shadowMapArea = lighting.getShadowMapArea();
        if (ImGui::DragFloat("Area##0", &shadowMapArea, 10.0f, 0.0f, 100000.0f))
        {
            lighting.setShadowMapArea(shadowMapArea);
        }
        float shadowMapNearFarSize = lighting.getShadowMapNearFarSize();
        if (ImGui::DragFloat("Near/far size##0", &shadowMapNearFarSize, 10.0f, 0.0f, 100000.0f))
        {
            lighting.setShadowMapNearFarSize(shadowMapNearFarSize);
        }
    }
    if (ImGui::CollapsingHeader("Field shadow map", ImGuiTreeNodeFlags_DefaultOpen| ImGuiTreeNodeFlags_SpanAvailWidth))
    {
        float fieldShadowOcclusion = lighting.getFieldShadowOcclusion();
        if (ImGui::DragFloat("Occlusion", &fieldShadowOcclusion, 0.01f, 0.0f, 1.0f))
        {
            lighting.setFieldShadowOcclusion(fieldShadowOcclusion);
        }
        float fieldShadowMapArea = lighting.getFieldShadowMapArea();
        if (ImGui::DragFloat("Area##1", &fieldShadowMapArea, 10.0f, 0.0f, 100000.0f))
        {
            lighting.setFieldShadowMapArea(fieldShadowMapArea);
        }
        float fieldShadowMapNearFarSize = lighting.getFieldShadowMapNearFarSize();
        if (ImGui::DragFloat("Near/far size##1", &fieldShadowMapNearFarSize, 10.0f, 0.0f, 100000.0f))
        {
            lighting.setFieldShadowMapNearFarSize(fieldShadowMapNearFarSize);
        }
        float fieldShadowDistance = lighting.getFieldShadowFadeStartDistance();
        if (ImGui::DragFloat("Fade Start Distance", &fieldShadowDistance, 1.0f, 0.0f, 1000.0f))
        {
            lighting.setFieldShadowFadeStartDistance(fieldShadowDistance);
        }
        float fieldShadowFadeRange = lighting.getFieldShadowFadeRange();
        if (ImGui::DragFloat("Fade Range", &fieldShadowFadeRange, 1.0f, 0.0f, 1000.0f))
        {
            lighting.setFieldShadowFadeRange(fieldShadowFadeRange);
        }
        float walkMeshExtrudeSize = lighting.getWalkmeshExtrudeSize();
        if (ImGui::DragFloat("Walkmesh extrude size", &walkMeshExtrudeSize, 0.01f, 0.0f, 100.0f))
        {
            lighting.setWalkmeshExtrudeSize(walkMeshExtrudeSize);
        }
        float offset = lighting.getWalkmeshPosOffset();
        if (ImGui::DragFloat("Walkmesh offset", &offset, 0.01f, -100.0f, 100.0f))
        {
            lighting.setWalkmeshPosOffset(offset);
        }
    }
    if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth))
    {
        int debugOutput = lighting.GetDebugOutput();
        if (ImGui::Combo("Debug Output", &debugOutput, "Disabled\0Color\0Normal\0Roughness\0Metallic\0AO\0Specular\0IBL (Specular)\0IBL (Diffuse)\0"))
        {
            lighting.setDebugOutput(static_cast<DebugOutput>(debugOutput));
        }
        bool isHide2dEnabled = lighting.isHide2dEnabled();
        if (ImGui::Checkbox("Hide 2D", &isHide2dEnabled))
        {
            lighting.setHide2dEnabled(isHide2dEnabled);
        }
        bool isShowWalkmeshEnabled = lighting.isShowWalkmeshEnabled();
        if (ImGui::Checkbox("Show walkmesh", &isShowWalkmeshEnabled))
        {
            lighting.setShowWalkmeshEnabled(isShowWalkmeshEnabled);
        }
    }
    ImGui::End();
}
