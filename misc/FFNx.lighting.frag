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

$input v_color0, v_texcoord0, v_position0, v_shadow0, v_normal0

#include <bgfx/bgfx_shader.sh>
#include "FFNx.lighting.sh"

// TEX_YUV
SAMPLER2D(tex_0, 0); // Y
SAMPLER2D(tex_1, 1); // U
SAMPLER2D(tex_2, 2); // V
// TEX_NML
SAMPLER2D(tex_5, 5);
// TEX_PBR
SAMPLER2D(tex_6, 6);

uniform vec4 VSFlags;
uniform vec4 FSAlphaFlags;
uniform vec4 FSMiscFlags;
uniform vec4 FSHDRFlags;
uniform vec4 FSTexFlags;
uniform vec4 FSMovieFlags;

uniform vec4 lightingSettings;
uniform vec4 lightingDebugData;
uniform vec4 materialData;
uniform vec4 materialScaleData;
uniform vec4 iblData;

#define isTLVertex VSFlags.x > 0.0
#define isFBTexture VSFlags.z > 0.0
#define isTexture VSFlags.w > 0.0
// ---
#define inAlphaRef FSAlphaFlags.x

#define isAlphaNever abs(FSAlphaFlags.y - 0.0) < 0.00001
#define isAlphaLess abs(FSAlphaFlags.y - 1.0) < 0.00001
#define isAlphaEqual abs(FSAlphaFlags.y - 2.0) < 0.00001
#define isAlphaLEqual abs(FSAlphaFlags.y - 3.0) < 0.00001
#define isAlphaGreater abs(FSAlphaFlags.y - 4.0) < 0.00001
#define isAlphaNotEqual abs(FSAlphaFlags.y - 5.0) < 0.00001
#define isAlphaGEqual abs(FSAlphaFlags.y - 6.0) < 0.00001

#define doAlphaTest FSAlphaFlags.z > 0.0

// ---
#define isFullRange FSMiscFlags.x > 0.0
#define isYUV FSMiscFlags.y > 0.0
#define modulateAlpha FSMiscFlags.z > 0.0
#define isMovie FSMiscFlags.w > 0.0

#define isHDR FSHDRFlags.x > 0.0
#define monitorNits FSHDRFlags.y


#define isBT601ColorMatrix abs(FSMovieFlags.x - 0.0) < 0.00001
#define isBT709ColorMatrix abs(FSMovieFlags.x - 1.0) < 0.00001
#define isBRG24ColorMatrix abs(FSMovieFlags.x - 2.0) < 0.00001

#define isSRGBColorGamut abs(FSMovieFlags.y - 0.0) < 0.00001
#define isSMPTECColorGamut abs(FSMovieFlags.y - 1.0) < 0.00001
#define isNTSCJColorGamut abs(FSMovieFlags.y - 2.0) < 0.00001
#define isEBUColorGamut abs(FSMovieFlags.y - 3.0) < 0.00001

#define isSRGBGamma abs(FSMovieFlags.z - 0.0) < 0.00001
#define is2pt2Gamma abs(FSMovieFlags.z - 1.0) < 0.00001
#define is170MGamma abs(FSMovieFlags.z - 2.0) < 0.00001
#define isToelessSRGBGamma abs(FSMovieFlags.z - 3.0) < 0.00001
#define is2pt8Gamma abs(FSMovieFlags.z - 4.0) < 0.00001

// ---
#define debugOutput lightingDebugData.z
#define DEBUG_OUTPUT_DISABLED 0
#define DEBUG_OUTPUT_COLOR 1
#define DEBUG_OUTPUT_NORMAL 2
#define DEBUG_OUTPUT_ROUGHNESS 3
#define DEBUG_OUTPUT_METALLIC 4
#define DEBUG_OUTPUT_AO 5
#define DEBUG_OUTPUT_SPECULAR 6
#define DEBUG_OUTPUT_IBL_SPECULAR 7
#define DEBUG_OUTPUT_IBL_DIFFUSE 8

#define isPbrTextureEnabled lightingSettings.x > 0.0
#define isEnvironmentLightingEnabled lightingSettings.y > 0.0

#define isNmlTextureLoaded FSTexFlags.x > 0.0
#define isPbrTextureLoaded FSTexFlags.y > 0.0
#define isIblTextureLoaded FSTexFlags.z > 0.0

void main()
{
    vec4 color = vec4(toLinear(v_color0.rgb), v_color0.a);
    // when !isTexture, this is used for directly rendering polygons with solid colors or gradients
    // when isTexture, this is used for used for multiplicative blend adjustments to textures
    // (In the latter case, not entirely sure this should be treated as a "color" (and thus linearized and gamut converted), rather than a straight "multiplier")
    
    // gamut convert v_color0 if needed
    if (isTexture){ // used for multiplicative blend adjustments to textures
        if (isHDR){
            if (!(isFBTexture)){ //FB needs no conversion b/c it's made up of things that were already converted
                if (isNTSCJColorGamut){ //until such time as texture metadata is supported, this will always be false 
                    color.rgb = convertGamut_NTSCJtoREC2020(color.rgb);
                }
                else {
                     color.rgb = convertGamut_SRGBtoREC2020(color.rgb);
                }
            }
        }
        else {
            if (!(isFBTexture)){ //FB needs no conversion b/c it's made up of things that were already converted
                if (isNTSCJColorGamut){ //until such time as texture metadata is supported, this will always be false 
                    color.rgb = convertGamut_NTSCJtoSRGB(color.rgb);
                }
            }
        }
    }
    else { // used for directly rendering polygons with solid colors or gradients
        if (isHDR){
            color.rgb = convertGamut_NTSCJtoREC2020(color.rgb);
        }
        else{
            color.rgb = convertGamut_NTSCJtoSRGB(color.rgb);
        }
    }
    
    vec4 color_nml = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 color_pbr = vec4(0.0, 0.0, 0.0, 0.0);

    if (isTexture)
    {
        if (isYUV)
        {
            vec3 yuv = vec3(
                texture2D(tex_0, v_texcoord0.xy).r,
                texture2D(tex_1, v_texcoord0.xy).r,
                texture2D(tex_2, v_texcoord0.xy).r
            );

// d3d9 doesn't support textureSize()
#if BGFX_SHADER_LANGUAGE_HLSL > 300 || BGFX_SHADER_LANGUAGE_GLSL || BGFX_SHADER_LANGUAGE_SPIRV
            if (!(isFullRange)){
                // dither prior to range conversion
                ivec2 ydimensions = textureSize(tex_0, 0);
                ivec2 udimensions = textureSize(tex_1, 0);
                ivec2 vdimensions = textureSize(tex_2, 0);
                yuv = QuasirandomDither(yuv, v_texcoord0.xy, ydimensions, udimensions, vdimensions, 255.0, 1.0);
                // clamp back to tv range
                yuv = clamp(yuv, vec3_splat(16.0/255.0), vec3(235.0/255.0, 240.0/255.0, 240.0/255.0));
            }
#endif
            
            if (isBT601ColorMatrix){
                yuv.g = yuv.g - (128.0/255.0);
                yuv.b = yuv.b - (128.0/255.0);
                if (isFullRange){
                    color.rgb = toRGB_bt601_fullrange(yuv);
                }
                else {
                    yuv.r = saturate(yuv.r - (16.0/255.0));       
                    color.rgb = toRGB_bt601_tvrange(yuv);
                }
            
            }
            else if (isBT709ColorMatrix){
                yuv.g = yuv.g - (128.0/255.0);
                yuv.b = yuv.b - (128.0/255.0);
                if (isFullRange){
                    color.rgb = toRGB_bt709_fullrange(yuv);
                }
                else {
                    yuv.r = saturate(yuv.r - (16.0/255.0));
                    color.rgb = toRGB_bt709_tvrange(yuv);
                }
            
            }
            else if (isBRG24ColorMatrix){
                color.rgb = yuv;
            }
            // default should be unreachable
            else {
                color.rgb = vec3_splat(0.5);
            }

            // Use a different inverse gamma function depending on the FMV's metadata
            if (isToelessSRGBGamma){
                color.rgb = toLinearToelessSRGB(color.rgb);
            }
            else if (is2pt2Gamma){
                color.rgb = toLinear2pt2(color.rgb);
            }
            else if (is170MGamma){
                color.rgb = toLinearSMPTE170M(color.rgb);
            }
            else if (is2pt8Gamma){
                color.rgb = toLinear2pt8(color.rgb);
            }
            else {
                color.rgb = toLinear(color.rgb);
            }
            
            // Convert gamut
            // For SDR, convert to sRGB(same as bt709)
            // For HDR, convert to req2020
            // Use of NTSC-J as the source gamut  for the original videos and their derivatives is a *highly* probable guess:
            // It looks correct, is consistent with the PS1's movie decoder chip's known use of BT601 color matrix, and conforms with Japanese TV standards of the time.
            if (isHDR){
                if (isNTSCJColorGamut){
                color.rgb = convertGamut_NTSCJtoREC2020(color.rgb);
                }
                else if (isSMPTECColorGamut){
                    color.rgb = convertGamut_SMPTECtoREC2020(color.rgb);
                }
                else if (isEBUColorGamut){
                    color.rgb = convertGamut_EBUtoREC2020(color.rgb);
                }
                else {
                    color.rgb = convertGamut_SRGBtoREC2020(color.rgb);
                }
            }
            else {
                if (isNTSCJColorGamut){
                    color.rgb = convertGamut_NTSCJtoSRGB(color.rgb);
                }
                else if (isSMPTECColorGamut){
                    color.rgb = convertGamut_SMPTECtoSRGB(color.rgb);
                }
                else if (isEBUColorGamut){
                    color.rgb = convertGamut_EBUtoSRGB(color.rgb);
                }
            }
            
            color.a = 1.0;
        }
        else
        {
            vec4 texture_color = texture2D(tex_0, v_texcoord0.xy);
            // used for ordinary textures and framebuffer textures.
            // framebuffer textures are used for things like the FF7 battle swirl
            // note: BGFX already linearized ordinary textures for us
            
            // gamut convert ordinary textures if needed (FB needs no conversion b/c it's made up of things that were already converted)
            if (!(isFBTexture)){
                if (isHDR){
                    if (isNTSCJColorGamut){ //until such time as texture metadata is supported, this will always be false 
                        texture_color.rgb = convertGamut_NTSCJtoREC2020(texture_color.rgb);
                    }
                    else {
                        texture_color.rgb = convertGamut_SRGBtoREC2020(texture_color.rgb);
                    }
                }
                else {
                    if (isNTSCJColorGamut){ //until such time as texture metadata is supported, this will always be false 
                        texture_color.rgb = convertGamut_NTSCJtoSRGB(texture_color.rgb);
                    }
                }
            }

            if (isNmlTextureLoaded) color_nml = texture2D(tex_5, v_texcoord0.xy);
            if (isPbrTextureLoaded) color_pbr = texture2D(tex_6, v_texcoord0.xy);

            if (doAlphaTest)
            {
                //NEVER
                if (isAlphaNever) discard;

                //LESS
                if (isAlphaLess)
                {
                    if (!(texture_color.a < inAlphaRef)) discard;
                }

                //EQUAL
                if (isAlphaEqual)
                {
                    if (!(texture_color.a == inAlphaRef)) discard;
                }

                //LEQUAL
                if (isAlphaLEqual)
                {
                    if (!(texture_color.a <= inAlphaRef)) discard;
                }

                //GREATER
                if (isAlphaGreater)
                {
                    if (!(texture_color.a > inAlphaRef)) discard;
                }

                //NOTEQUAL
                if (isAlphaNotEqual)
                {
                    if (!(texture_color.a != inAlphaRef)) discard;
                }

                //GEQUAL
                if (isAlphaGEqual)
                {
                    if (!(texture_color.a >= inAlphaRef)) discard;
                }
            }

            if (isFBTexture)
            {
                if(all(equal(texture_color.rgb,vec3_splat(0.0)))) discard;

                if (!(isHDR)) {
                    texture_color.rgb = toLinear(texture_color.rgb);
                }
            }

            if (isMovie) texture_color.a = 1.0;

            if (texture_color.a == 0.0) discard;

            if (modulateAlpha) color *= texture_color;
            else
            {
                color.rgb *= texture_color.rgb;
			    color.a = texture_color.a;
            }
        }
    }

    if(isTLVertex)
    {
        gl_FragColor = color;
        if(isTimeFilterEnabled)
        {
            if (isHDR){
                gl_FragColor.rgb *= convertGamut_SRGBtoREC2020(TimeColor.rgb);
            }
            else {
                gl_FragColor.rgb *= TimeColor.rgb;
            }
        }
    }
    else
    {
        // Shadow UV
        vec3 shadowUv = v_shadow0.xyz / v_shadow0.w;

        // View Direction
        vec3 viewDir = normalize(-v_position0.xyz);

        // Normal
        vec3 normal = normalize(v_normal0);
        if(isNmlTextureLoaded && isPbrTextureEnabled) normal = perturb_normal(normal, v_position0.xyz, color_nml.rgb, v_texcoord0.xy );

        // Roughness
        float perceptualRoughness = materialData.x;
        if(isPbrTextureLoaded && isPbrTextureEnabled) perceptualRoughness = color_pbr.r * materialScaleData.x;
        float roughness = min(max(0.001, perceptualRoughness * perceptualRoughness), 1.0);

        // Metallic
        float metallic = materialData.y;
        if(isPbrTextureLoaded && isPbrTextureEnabled) metallic = color_pbr.g * materialScaleData.y;
        metallic = min(1.0, metallic);

        // Specular (dielectric)
        float specular = materialData.z;
        if(isPbrTextureLoaded && isPbrTextureEnabled) specular = color_pbr.b * materialScaleData.z;
        specular = min(1.0, specular);

        // Ambient Occlusion
        float ao = 1.0;
        if(isPbrTextureLoaded && isPbrTextureEnabled) ao = color_pbr.a;

        // Luminance
        vec3 luminance = calcLuminance(color.rgb, v_position0.xyz, viewDir, normal, perceptualRoughness, roughness, metallic, specular, shadowUv, isHDR);

        // Indirect Luminance
        vec3 indirectLuminance = vec3_splat(0.0);
        vec3 specularIbl = vec3_splat(0.0);
        vec3 diffuseIbl = vec3_splat(0.0);
        if(isIblTextureLoaded && isEnvironmentLightingEnabled)
        {
            // Specular IBL
            vec3 R = mul(invViewMatrix, vec4(reflect(-viewDir, normal), 0)).xyz;
            float iblMipCount = iblData.x;
            float iblLod = CalcMipmapFromRoughness(roughness, iblMipCount);
            specularIbl = textureCubeLod(tex_7, R, iblLod).rgb;

            // Diffuse IBL
            vec3 worldNormal = mul(invViewMatrix, vec4(normal, 0)).xyz;
            diffuseIbl = textureCube(tex_8, worldNormal).rgb;

            indirectLuminance = CalcIblIndirectLuminance(color.rgb, specularIbl, diffuseIbl, viewDir, normal, roughness, metallic, specular, ao, isHDR);
        }
        else
        {
            indirectLuminance = CalcConstIndirectLuminance(color.rgb, isHDR);
        }

        if(debugOutput == DEBUG_OUTPUT_COLOR)
        {
            gl_FragColor = color;
        }
        else if(debugOutput == DEBUG_OUTPUT_NORMAL)
        {
            gl_FragColor = vec4(0.5 * normal + 0.5, 1.0);
        }
        else if(debugOutput == DEBUG_OUTPUT_ROUGHNESS)
        {
            gl_FragColor = vec4(vec3_splat(perceptualRoughness), 1.0);
        }
        else if(debugOutput == DEBUG_OUTPUT_METALLIC)
        {
            gl_FragColor = vec4(vec3_splat(metallic), 1.0);
        }
        else if(debugOutput == DEBUG_OUTPUT_AO)
        {
            gl_FragColor = vec4(vec3_splat(ao), 1.0);
        }
        else if(debugOutput == DEBUG_OUTPUT_SPECULAR)
        {
            gl_FragColor = vec4(vec3_splat(specular), 1.0);
        }
        else if(debugOutput == DEBUG_OUTPUT_IBL_SPECULAR)
        {
            gl_FragColor = vec4(specularIbl, 1.0);
        }
        else if(debugOutput == DEBUG_OUTPUT_IBL_DIFFUSE)
        {
            gl_FragColor = vec4(diffuseIbl, 1.0);
        }
        else
        {
            gl_FragColor = vec4(luminance + indirectLuminance, color.a);
        }
    }

    if (!(isHDR)) {
        // SDR screens require the Gamma output to properly render light scenes
        gl_FragColor.rgb = toGamma(gl_FragColor.rgb);
    }
}
