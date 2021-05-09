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

#include "FFNx.pcf.sh"

uniform vec4 lightDirData;
uniform vec4 lightData;
uniform vec4 ambientLightData;
uniform vec4 materialData;

#define INV_PI 0.31831

vec3 fresnelSchlick(vec3 f0, float cosine)
{
    return f0 + (1.0 - f0) * pow(1.0 - cosine, 5.0);
}

float normalDistributionGgx(vec3 N, vec3 H, float roughness)
{
    float roughness2 = roughness * roughness;
    float dotNH = max(0.0, dot(N, H));
    float a = (1.0 + (roughness2 - 1.0) * dotNH * dotNH);
    return roughness2 * INV_PI / (a * a);
}

float geometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = geometrySchlickGGX(NdotV, k);
    float ggx2 = geometrySchlickGGX(NdotL, k);

    return ggx1 * ggx2;
}

vec3 specularCookTorranceBrdf(vec3 F0, vec3 N, vec3 V, vec3 L, float roughness)
{
    vec3 H = normalize(V + L);
    float dotNV = saturate(dot(N, V));
    float dotNL = saturate(dot(N, L));
    float dotVH = saturate(dot(V, H));
    float d = normalDistributionGgx(N, H, roughness);
    float g = geometrySmith(N, V, L, roughness);
    vec3 f = fresnelSchlick(F0, dotVH);
    return d  * g * f / max(0.001, 4.0 * dotNV * dotNL);
}

// Calculates luminance using Physically-Based Rendering (PBR)
// https://learnopengl.com/PBR/Theory
vec3 calcLuminance(vec3 albedo, vec3 viewSpacePosition, vec3 normal, vec3 shadowUv)
{
    float shadowFactor = sampleShadowMapPCF7x7(shadowUv.xyz, viewSpacePosition.xyz);

    // Ambient
    vec3 ambientLightColor = ambientLightData.rgb;
    float ambientLightIntensity = ambientLightData.w;
    vec3 ambient = ambientLightIntensity * ambientLightColor * INV_PI * albedo.rgb;

    // Material
    float roughness = max(0.001, materialData.x);
    float metalness = materialData.y;

    // Diffuse
    vec3 diffuse = (1.0 - metalness) * INV_PI * albedo;

    // Specular
    vec3 viewDir = normalize(viewSpacePosition.xyz);
    vec3 lightDir = normalize(lightDirData.xyz);
    float power = materialData.x;
    vec3 F0 = vec3(0.04, 0.04, 0.04);
    F0 = mix(F0, albedo, metalness);
    vec3 specular = metalness * specularCookTorranceBrdf(F0, normal, viewDir, -lightDir, roughness);

    // Light
    float lightIntensity = lightData.w;
    vec3 lightColor = lightData.rgb;

    // Lambert cosine
    float NdotL = max(0.0, dot(normal, -lightDir));

    return ambient + shadowFactor * lightIntensity * lightColor * (diffuse + specular) * NdotL;
}