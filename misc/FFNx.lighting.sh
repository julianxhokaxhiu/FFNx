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

#define INV_PI 0.31831

// Normal Mapping Without Precomputed Tangents
// http://www.thetenthplanet.de/archives/1180
mat3 cotangent_frame( vec3 N, vec3 p, vec2 uv ) 
{ 
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p ); 
    vec3 dp2 = dFdy( p ); 
    vec2 duv1 = dFdx( uv ); 
    vec2 duv2 = dFdy( uv );
    
    // solve the linear system
    vec3 dp2perp = cross( dp2, N ); 
    vec3 dp1perp = cross( N, dp1 ); 
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;  
    
    // construct a scale-invariant frame 
    float invmax = inversesqrt(max( dot(T,T), dot(B,B)));

    return transpose(mat3(T * invmax, B * invmax, N)); 
}

vec3 perturb_normal(vec3 N, vec3 V, vec3 normalmap, vec2 texcoord)
{ 
    // assume N, the interpolated vertex normal and 
    // V, the view vector (vertex to eye)
    vec3 normalMapRemapped = normalmap * 255.0/127.0 - 128.0/127.0;

    // Blend tangent space normal at uv mirror edges to prevent discontinuities
    float blendRange = 0.1;
    float borderCoeff_u = fract(texcoord.x);
    float borderCoeff_v = fract(texcoord.y);
    borderCoeff_u = borderCoeff_u < 0.5 ? borderCoeff_u : 1.0 - borderCoeff_u;
    borderCoeff_v = borderCoeff_v < 0.5 ? borderCoeff_v : 1.0 - borderCoeff_v;
    float t_u = min(1.0, borderCoeff_u / blendRange);                
    float t_v = min(1.0, borderCoeff_v / blendRange);
    float newNormalMapX = mix(0.0, normalMapRemapped.x, smoothstep(0.0, 1.0, t_u));
    float newNormalMapY = mix(0.0, normalMapRemapped.y, smoothstep(0.0, 1.0, t_v));
    normalMapRemapped = vec3(newNormalMapX, newNormalMapY, sqrt(1.0 - newNormalMapX * newNormalMapX + newNormalMapY * newNormalMapY));

    // Build matrix to transform from tangent to view space
    mat3 TBN = cotangent_frame(N, -V, texcoord);

    return normalize(mul(TBN, normalMapRemapped)); 
}

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
vec3 calcLuminance(vec3 albedo, vec3 viewSpacePosition, vec3 viewDir, vec3 normal, float roughness, float metalness, float ao, vec3 shadowUv)
{
    float shadowFactor = sampleShadowMapPCF7x7(shadowUv.xyz, viewSpacePosition.xyz);

    // Ambient
    vec3 ambientLightColor = ambientLightData.rgb;
    float ambientLightIntensity = ambientLightData.w;
    vec3 ambient = ambientLightIntensity * ambientLightColor * INV_PI * albedo.rgb;

    // Diffuse
    vec3 diffuse = (1.0 - metalness) * INV_PI * albedo;

    // Specular
    vec3 lightDir = normalize(lightDirData.xyz);
    vec3 F0 = vec3(0.04, 0.04, 0.04);
    F0 = mix(F0, albedo, metalness);
    vec3 specular = metalness * specularCookTorranceBrdf(F0, normal, viewDir, -lightDir, roughness);

    // Light
    float lightIntensity = lightData.w;
    vec3 lightColor = lightData.rgb;

    // Lambert cosine
    float NdotL = max(0.0, dot(normal, -lightDir));

    return ao * ambient + shadowFactor * lightIntensity * lightColor * (diffuse + specular) * NdotL;
}