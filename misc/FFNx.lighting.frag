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

// This shader is never used for 2D elements.
// This shader is used for 3D elements when advanced lighting is enabled.

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
uniform vec4 WMFlags;
//uniform vec4 FSMovieFlags;

uniform vec4 lightingSettings;
uniform vec4 lightingDebugData;
uniform vec4 materialData;
uniform vec4 materialScaleData;
uniform vec4 iblData;
uniform vec4 gameLightingFlags;
uniform vec4 gameGlobalLightColor;
uniform vec4 gameLightColor1;
uniform vec4 gameLightColor2;
uniform vec4 gameLightColor3;
uniform vec4 gameLightDir1;
uniform vec4 gameLightDir2;
uniform vec4 gameLightDir3;
uniform vec4 gameScriptedLightColor;

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
#define isNTSCJColorGamut abs(FSMovieFlags.y - 1.0) < 0.00001
#define isSMPTECColorGamut abs(FSMovieFlags.y - 2.0) < 0.00001
#define isEBUColorGamut abs(FSMovieFlags.y - 3.0) < 0.00001

#define isSRGBGamma abs(FSMovieFlags.z - 0.0) < 0.00001
#define is2pt2Gamma abs(FSMovieFlags.z - 1.0) < 0.00001
#define is170MGamma abs(FSMovieFlags.z - 2.0) < 0.00001
#define isCRTGamma abs(FSMovieFlags.z - 3.0) < 0.00001
#define is2pt8Gamma abs(FSMovieFlags.z - 4.0) < 0.00001

#define isOverallSRGBColorGamut abs(FSMovieFlags.w - 0.0) < 0.00001
//#define isOverallNTSCJColorGamut abs(FSMovieFlags.w - 1.0) < 0.00001 // already defined in included FFNx.lighting.sh

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

#define isFogEnabled WMFlags.y > 0.0

#define isNmlTextureLoaded FSTexFlags.x > 0.0
#define isPbrTextureLoaded FSTexFlags.y > 0.0
#define isIblTextureLoaded FSTexFlags.z > 0.0

#define gameLightingMode gameLightingFlags.x
#define GAME_LIGHTING_PER_PIXEL 2

void main()
{
    // v_color0 is used for solid-color polygon faces (e.g., all original FF7 models) and colorizing textures
    // (Not used for solid-color 2D elements b/c this shader isn't used for 2D elements.)
    // This variable is clobbered for YUV movies.
    vec4 color = v_color0; //previously linearized in vertex shader
    vec4 color_nml = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 color_pbr = vec4(0.0, 0.0, 0.0, 0.0);

    if (isTexture)
    {
        // All movies except non-steam FF8 use YUV plumbing. (Including FF7 original movies.)
        // (But this shader is never used for YUV movies.)
        if (isYUV)
        {
            vec3 yuv = vec3(
                texture2D(tex_0, v_texcoord0.xy).r,
                texture2D(tex_1, v_texcoord0.xy).r,
                texture2D(tex_2, v_texcoord0.xy).r
            );

            if (!(isFullRange)){
                // dither prior to range conversion
                ivec2 ydimensions = textureSize(tex_0, 0);
                ivec2 udimensions = textureSize(tex_1, 0);
                ivec2 vdimensions = textureSize(tex_2, 0);
                yuv = QuasirandomDither(yuv, v_texcoord0.xy, ydimensions, udimensions, vdimensions, 256.0, 1.0);
                // clamp back to tv range
                yuv = clamp(yuv, vec3_splat(16.0/255.0), vec3(235.0/255.0, 240.0/255.0, 240.0/255.0));
            }

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
            // special case -- assume NTSC-J gamut always implies BT1886 Appx1 gamma (and how we deal with that depends on our target gamut)
            if (isNTSCJColorGamut)
            {
                if (isOverallNTSCJColorGamut){
                    color.rgb = CRTSimulation(color.rgb);
                }
                else{
                    color.rgb = GamutLUT(color.rgb, true, false);
                }
            }
            else if (isCRTGamma){
                color.rgb = toLinearBT1886Appx1Fast(color.rgb);
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

            // We need to get everything into linear RGB in our working gamut
            // (We may draw objects over the top of the movie, so we need to make things consistent **NOW**)
            if (isOverallNTSCJColorGamut){
                // do nothing for NTSC-J
                if ((isSRGBColorGamut) || (isSMPTECColorGamut) || (isEBUColorGamut)){
                    color.rgb = GamutLUT(color.rgb, false, true);
                }
            }
            // overall sRGB
            else {
                // do nothing for sRGB(/bt709) -- nothing to be done
                // do nothing for NTSC-J -- already done above
                if ((isSMPTECColorGamut) || (isEBUColorGamut)){
                    color.rgb = GamutLUT(color.rgb, false, false);
                }
            }

            color.a = 1.0;
        }
        // This stanza pertains to textures on 3D objects if advanced lighting is enabled
        else
        {
            vec4 texture_color = texture2D(tex_0, v_texcoord0.xy);

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

            // check for some discard conditions
            if (isMovie) texture_color.a = 1.0;
            if (texture_color.a == 0.0) discard;
            if (isFBTexture)
            {
                if(all(equal(texture_color.rgb,vec3_splat(0.0)))) discard;
            }

            // linearize, possibly with gamut conversion
            texture_color.rgb = toSomeLinearRGB(texture_color.rgb, isOverallNTSCJColorGamut);

            // multiply by v_color0
            if (modulateAlpha) color *= texture_color;
            else
            {
                color.rgb *= texture_color.rgb;
                color.a = texture_color.a;
            }

        }
    }

    vec3 normal = vec3(0.0, 0.0, 0.0);
    if(isTLVertex)
    {
        gl_FragColor = color;
        if(isTimeFilterEnabled)
        {
            gl_FragColor.rgb *= TimeColor.rgb;
        }
    }
    else
    {
        // Shadow UV
        vec3 shadowUv = v_shadow0.xyz / v_shadow0.w;

        // View Direction
        vec3 viewDir = normalize(-v_position0.xyz);

        // Normal
        normal = normalize(v_normal0);
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
        vec3 luminance = calcLuminance(color.rgb, v_position0.xyz, viewDir, normal, perceptualRoughness, roughness, metallic, specular, shadowUv);

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

            indirectLuminance = CalcIblIndirectLuminance(color.rgb, specularIbl, diffuseIbl, viewDir, normal, roughness, metallic, specular, ao);
        }
        else
        {
            indirectLuminance = CalcConstIndirectLuminance(color.rgb);
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

        if (isFogEnabled && debugOutput == DEBUG_OUTPUT_DISABLED ) gl_FragColor.rgb = ApplyWorldFog(gl_FragColor.rgb, v_position0.xyz);
    }

    if(!(isTLVertex) && gameLightingMode == GAME_LIGHTING_PER_PIXEL && debugOutput == DEBUG_OUTPUT_DISABLED)
    {
        vec3 worldNormal = mul(invViewMatrix, vec4(normal, 0)).xyz;
        float dotLight1 = saturate(dot(worldNormal, gameLightDir1.xyz));
        float dotLight2 = saturate(dot(worldNormal, gameLightDir2.xyz));
        float dotLight3 = saturate(dot(worldNormal, gameLightDir3.xyz));


        vec3 light1Ambient = toSomeLinearRGB(gameLightColor1.rgb, isOverallNTSCJColorGamut) * dotLight1 * dotLight1;
        vec3 light2Ambient = toSomeLinearRGB(gameLightColor2.rgb, isOverallNTSCJColorGamut) * dotLight2 * dotLight2;
        vec3 light3Ambient = toSomeLinearRGB(gameLightColor3.rgb, isOverallNTSCJColorGamut) * dotLight3 * dotLight3;
        vec3 lightAmbient = toSomeLinearRGB(gameScriptedLightColor.rgb, isOverallNTSCJColorGamut) * (toSomeLinearRGB(gameGlobalLightColor.rgb, isOverallNTSCJColorGamut) + light1Ambient + light2Ambient + light3Ambient);

        gl_FragColor.rgb *= gameGlobalLightColor.w * lightAmbient;
    }

    // return to sRGB gamma space so we can do alpha blending the same way FF7/8 did.
    gl_FragColor.rgb = toSomeGammaRGB(gl_FragColor.rgb, isOverallNTSCJColorGamut);

    // if we did a movie gamut conversion, and won't dither later, then dither now
    // do this in gamma space so that dither step size is proportional to quantization step size

    if (isTexture && !(isOverallNTSCJColorGamut) && isYUV && !(isSRGBColorGamut))
    {
      ivec2 dimensions = textureSize(tex_0, 0);
      gl_FragColor.rgb = QuasirandomDither(gl_FragColor.rgb, v_texcoord0.xy, dimensions, dimensions, dimensions, 256.0, 4320.0);
    }


}
