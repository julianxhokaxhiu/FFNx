/****************************************************************************/
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

#include "FFNx.pcf.sh"
#include "FFNx.common.sh"

// Specular IBL cubemap
SAMPLERCUBE(tex_7, 7);
// Diffuse IBL cubemap
SAMPLERCUBE(tex_8, 8);
// BRDF
SAMPLER2D(tex_9, 9);

uniform vec4 lightDirData;
uniform vec4 lightData;
uniform vec4 ambientLightData;
uniform vec4 TimeColor;
uniform vec4 TimeData;
uniform vec4 FSMovieFlags;

#define isTimeEnabled TimeData.x > 0.0
#define isTimeFilterEnabled TimeData.x > 0.0 && TimeData.y > 0.0

#define INV_PI 0.31831

#define isOverallNTSCJColorGamut abs(FSMovieFlags.w - 1.0) < 0.00001

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

vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
    float Fc = pow( 1 - cosTheta, 5 );
	return saturate( 50.0 * F0.g ) * Fc + (1 - Fc) * F0;
}

float normalDistributionGgx(vec3 N, vec3 H, float perceptualRoughness)
{
    float a2     = perceptualRoughness*perceptualRoughness;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = M_PI * denom * denom;

    return num / denom;
}

float visiblilitySchlick(float perceptualRoughness, float NoV, float NoL)
{
	float k = (perceptualRoughness + 1) / 8.0;
	float visSchlickV = NoV * (1 - k) + k;
	float visSchlickL = NoL * (1 - k) + k;
	return 0.25 / (visSchlickV * visSchlickL);
}

vec3 specularCookTorranceBrdf(vec3 F0, vec3 N, vec3 V, vec3 L, float perceptualRoughness, float roughness)
{
    vec3 H = normalize(V + L);
    float dotNV = saturate(dot(N, V)) + 1e-5;
    float dotNL = saturate(dot(N, L));
    float dotVH = saturate(dot(V, H));
    float d = normalDistributionGgx(N, H, roughness);
    float g = visiblilitySchlick(perceptualRoughness, dotNV, dotNL);
    vec3 f = fresnelSchlick(F0, dotVH);
    return d * g * f;
}

float CalcMipmapFromRoughness(float roughness, float mipCount)
{
	float level = 3.0 - 1.15 * log2( roughness );
	return mipCount - 1.0 - level;
}

// Calculates luminance using Physically-Based Rendering (PBR)
// https://learnopengl.com/PBR/Theory
vec3 calcLuminance(vec3 albedo, vec3 viewSpacePosition, vec3 viewDir, vec3 normal, float perceptualRoughness, float roughness, float metallic, float specular, vec3 shadowUv)
{
    float shadowFactor = sampleShadowMapPCF7x7(shadowUv.xyz, viewSpacePosition.xyz);

    // Light
    float lightIntensity = lightData.w;
    vec3 lightColor = toSomeLinearRGB(lightData.rgb, isOverallNTSCJColorGamut);
    if(isTimeEnabled)
    {
        lightColor *= TimeColor.rgb;
    }

    vec3 lightDir = normalize(lightDirData.xyz);

    vec3 F0 = mix(specular * vec3_splat(0.08), albedo, metallic);

    vec3 H = normalize(viewDir + lightDir);
    float dotVH = saturate(dot(viewDir, H));

    // Diffuse
    vec3 diffuseLuminance = (1.0 - metallic) * INV_PI * albedo;

    // Specular
    vec3 specularLuminance = specularCookTorranceBrdf(F0, normal, viewDir, lightDir, perceptualRoughness, roughness);

    // Lambert cosine
    float NdotL = max(0.0, dot(normal, lightDir));

    return shadowFactor * lightIntensity * lightColor * (diffuseLuminance + specularLuminance) * NdotL;
}

vec3 CalcIblIndirectLuminance(vec3 albedo, vec3 specularIbl, vec3 diffuseIbl, vec3 V, vec3 N, float roughness, float metallic, float specular, float ao)
{
    float dotNV = saturate(dot(N, V));
    vec2 envBRDF = texture2D(tex_9, vec2(dotNV, 1.0 - roughness)).xy;

    vec3 F0 = mix(specular * vec3_splat(0.08), albedo, metallic);
    vec3 indirectSpecular = specularIbl * (F0 * envBRDF.x + envBRDF.y);

    vec3 diffuse = diffuseIbl * albedo;
    vec3 indirectDiffuse = (1.0 - metallic) * diffuse;

    vec3 ambientLightColor = toSomeLinearRGB(ambientLightData.rgb, isOverallNTSCJColorGamut);
    float ambientLightIntensity = ambientLightData.w;

    return (indirectDiffuse + indirectSpecular) * ambientLightColor * ambientLightIntensity * ao;
}

vec3 CalcConstIndirectLuminance(vec3 albedo)
{
    // Ambient
    vec3 ambientLightColor = toSomeLinearRGB(ambientLightData.rgb, isOverallNTSCJColorGamut);
    if(isTimeEnabled)
    {
        ambientLightColor *= TimeColor.rgb;
    }

    float ambientLightIntensity = ambientLightData.w;
    vec3 ambient = ambientLightIntensity * ambientLightColor * INV_PI * albedo.rgb;

    return ambient;
}
