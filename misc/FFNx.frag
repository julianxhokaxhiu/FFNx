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

$input v_color0, v_texcoord0, v_position0, v_normal0

#include <bgfx/bgfx_shader.sh>
#include "FFNx.common.sh"

SAMPLER2D(tex_0, 0);
SAMPLER2D(tex_1, 1);
SAMPLER2D(tex_2, 2);

uniform mat4 invViewMatrix;

uniform vec4 VSFlags;
uniform vec4 FSAlphaFlags;
uniform vec4 FSMiscFlags;
uniform vec4 FSHDRFlags;
uniform vec4 FSTexFlags;
uniform vec4 WMFlags;
uniform vec4 FSMovieFlags;
uniform vec4 TimeColor;
uniform vec4 TimeData;
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
#define isOverallNTSCJColorGamut abs(FSMovieFlags.w - 1.0) < 0.00001

#define isTimeEnabled TimeData.x > 0.0
#define isTimeFilterEnabled TimeData.x > 0.0 && TimeData.y > 0.0

#define gameLightingMode gameLightingFlags.x
#define GAME_LIGHTING_PER_PIXEL 2

#define isFogEnabled WMFlags.y > 0.0

void main()
{
    vec4 color = vec4(toLinearBT1886Appx1Fast(v_color0.rgb), v_color0.a);

    if (isTexture)
    {
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
                yuv = QuasirandomDither(yuv, v_texcoord0.xy, ydimensions, udimensions, vdimensions, 255.0, 1.0);
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
            if (isCRTGamma){
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

            // Convert gamut to BT709/SRGB or NTSC-J, depending on what we're going to do in post.
            // We need to do this here, because we may draw things on top of the video, and so we need to match the gamut of the things we're going to draw.
            // This approach has the unfortunate drawback of resulting in two gamut conversions for some inputs.
            // But I see no alternative.
            if (isOverallNTSCJColorGamut){
                // do nothing for NTSC-J
                // for other gamuts, backwards convert so that final NTSCJ-to-sRGB conversion will be a round trip
                if ((isSRGBColorGamut) || (isSMPTECColorGamut) || (isEBUColorGamut)){
                    color.rgb = GamutLUT(color.rgb, false, false, true); // linear in, linear out; LUT contains gamma-space data, but LUT function will linearize it while interpolating
                    // dither after the LUT operation
                    ivec2 dimensions = textureSize(tex_0, 0);
                    color.rgb = QuasirandomDither(color.rgb, v_texcoord0.xy, dimensions, dimensions, dimensions, 255.0, 4320.0);
                }
                // Note: Bring back matrix-based conversions for HDR *if* we can find a way to left potentially out-of-bounds values linger until post processing.
            }
            // overall sRGB
            else {
                // do nothing for sRGB
                // take NTSC-J back to gamma-space, then full-on conversion
                // straight linear-to-linear conversion for SMPTE-C and EBU
                if ((isNTSCJColorGamut) || (isSMPTECColorGamut) || (isEBUColorGamut)){
                    if (isNTSCJColorGamut){
                      // go back to gamma space
                      color.rgb = toGammaBT1886Appx1Fast(color.rgb);
                    }
                    color.rgb = GamutLUT(color.rgb, isNTSCJColorGamut, true, false); // variable in, linear out; LUT contains sRGB gamma-space data, but LUT function will linearize it while interpolating
                    // dither after the LUT operation
                    ivec2 dimensions = textureSize(tex_0, 0);
                    color.rgb = QuasirandomDither(color.rgb, v_texcoord0.xy, dimensions, dimensions, dimensions, 255.0, 4320.0);
                }
                // Note: Bring back matrix-based conversions for HDR *if* we can find a way to left potentially out-of-bounds values linger until post processing.
            }

            color.a = 1.0;
        }
        else
        {
            vec4 texture_color = texture2D(tex_0, v_texcoord0.xy);

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

                // This was previously in gamma space, so linearize again.
                //texture_color.rgb = toLinearBT1886Appx1Fast(texture_color.rgb);

            }

            // Use CRT gamma for all textures (no longer using BGFX's built-in sRGB linearization)
            texture_color.rgb = toLinearBT1886Appx1Fast(texture_color.rgb);

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

    if (isTimeFilterEnabled) color.rgb *= TimeColor.rgb;

    if (!(isTLVertex) && isFogEnabled) color.rgb = ApplyWorldFog(color.rgb, v_position0.xyz);

    // return to gamma space so we can do alpha blending the same way FF7/8 did.
    color.rgb = toGammaBT1886Appx1Fast(color.rgb);

    // In this default shader, lighting is applied in gamma space so that it does better match the original lighting
    if (gameLightingMode == GAME_LIGHTING_PER_PIXEL)
    {
        vec3 normal = normalize(v_normal0);
        vec3 worldNormal = mul(invViewMatrix, vec4(normal, 0)).xyz;
        float dotLight1 = saturate(dot(worldNormal, gameLightDir1.xyz));
        float dotLight2 = saturate(dot(worldNormal, gameLightDir2.xyz));
        float dotLight3 = saturate(dot(worldNormal, gameLightDir3.xyz));
        vec3 light1Ambient = gameLightColor1.rgb * dotLight1 * dotLight1;
        vec3 light2Ambient = gameLightColor2.rgb * dotLight2 * dotLight2;
        vec3 light3Ambient = gameLightColor3.rgb * dotLight3 * dotLight3;
        vec3 lightAmbient = gameScriptedLightColor.rgb * (gameGlobalLightColor.rgb + light1Ambient + light2Ambient + light3Ambient);
        color.rgb *= gameGlobalLightColor.w * lightAmbient;
    }
    
    gl_FragColor = color;
}
