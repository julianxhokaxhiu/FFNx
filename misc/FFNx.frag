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

// This shader is always used for 2D elements.
// This shader is used for 3D elements when advanced lighting is disabled.

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
#define isNotFBTexture VSFlags.z < 0.00001
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
#define isRawP22ColorGamut abs(FSMovieFlags.y - 3.0) < 0.00001

#define isSRGBGamma abs(FSMovieFlags.z - 0.0) < 0.00001
#define is170MGamma abs(FSMovieFlags.z - 1.0) < 0.00001
#define isCRTGamma abs(FSMovieFlags.z - 2.0) < 0.00001


#define isOverallSRGBColorGamut abs(FSMovieFlags.w - 0.0) < 0.00001
#define isOverallNTSCJColorGamut abs(FSMovieFlags.w - 1.0) < 0.00001

#define isTimeEnabled TimeData.x > 0.0
#define isTimeFilterEnabled TimeData.x > 0.0 && TimeData.y > 0.0

#define gameLightingMode gameLightingFlags.x
#define GAME_LIGHTING_PER_PIXEL 2

#define isFogEnabled WMFlags.y > 0.0

void main()
{
    // v_color0 is used for solid 2D colors (e.g., textbox backgrounds), solid-color polygon faces (e.g., all original FF7 models), and colorizing textures
    // This variable is clobbered for YUV movies.
    vec4 color = v_color0;
    color.rgb = toLinear(color.rgb);

    if (isTexture)
    {
        // TODO: relocate this stanza to a separate shader for YUVmovies
        // All movies except non-steam FF8 use YUV plumbing. (Including FF7 original movies.)
        if (isYUV)
        {
            // At this juncture, ffmpeg has decoded our video file,
            // and the metadata we need has been passed as uniforms.
            // Now we need to do the following:
            //  1. If limited (tv) range, expand to full (pc) range
            //  2. Convert YUV to gamma-space R'G'B'
            //  3. Convert gamma-space R'G'B' to linear RGB.
            //  4. Convert from the video's gamut to our working gamut.
            //  5. Convert from linear R'G'B' to our working gamma-space.
            // In NTSC-J mode, our working gamut and gamma is uncorrected NTSC-J (i.e., before the CRT's color correction circuit).
            // Temporary: Until this shader gets split off, we may need an extra gamma conversion
            //    to get in the right space ahead of the toGamma() at the bottom of this shader.
            //    (After the split, that toGamma() can be changed.)
            // In sRGB mode, our working gamut and gamma is sRGB.
            // Also, in sRGB mode, we simply ignore the video's gamma and gamut and treat them as if they were sRGB.
            // This is wrong in almost every case, but our project maintainer insists upon it.

            // fetch YUV from 3 textures
            // TODO: Look at the feasibility of passing chroma position as a uniform so we can resample YUV 4:2:0 etc properly in the shader instead of on the CPU in swscale
            vec3 yuv = vec3(
                texture2D(tex_0, v_texcoord0.xy).r,
                texture2D(tex_1, v_texcoord0.xy).r,
                texture2D(tex_2, v_texcoord0.xy).r
            );

            // If the video is limited range, dither ahead of increasing the effective bit depth
            if (!(isFullRange)){
                ivec2 ydimensions = textureSize(tex_0, 0);
                ivec2 udimensions = textureSize(tex_1, 0);
                ivec2 vdimensions = textureSize(tex_2, 0);
                yuv = QuasirandomDither(yuv, v_texcoord0.xy, ydimensions, udimensions, vdimensions, 256.0, 1.0);
                // clamp back to tv range
                yuv = clamp(yuv, vec3_splat(16.0/255.0), vec3(235.0/255.0, 240.0/255.0, 240.0/255.0));
            }

            // Convert YUV to linear R'G'B', expanding range if needed
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
            else { //isBRG24ColorMatrix
                // This is a special case where we converted the BRG24 movies from the PC98 edition of FF7 to planar RGB in order to pass them through the YUV plumbing.
                color.rgb = yuv;
            }

            // TODO: Split into two separate shaders, one with this stanza, and one without
            if (isOverallNTSCJColorGamut){
                // Convert gamma to CRT gamma (BT1886 Appenddix 1) and gamut to NTSC-J

                // Do nothing for NTSC-J gamut; it's already correct. (CRT gamma is implied.)

                // For uncorrected P22, multiply by the inverse of the matrix for the CRT color correction circuit,
                // and the final NTSC-J-to-sRGB/rec2020 conversion will roundtrip that,
                // leading to the desired overall result
                if (isRawP22ColorGamut){
                    color.rgb = CRTUncorrect(color.rgb);
                    // (CRT gamma is implied.)
                }
                // Otherwise we need to do a full conversion
                else if ((isSRGBColorGamut) || (isSMPTECColorGamut)) {
                    if (isCRTGamma){
                        color.rgb = toLinearBT1886Appx1Fast(color.rgb);
                    }
                    else if (is170MGamma){
                        color.rgb = toLinearSMPTE170M(color.rgb);
                    }
                    else {
                        color.rgb = toLinear(color.rgb);
                    }
                    // This LUT goes backards from linear RGB to uncorrected gamma-space NTSC-J
                    // AssignGamutLUT() in renderer.cpp should have bound the correct LUT
                    color.rgb = GamutLUTBackwards(color.rgb);
                }
            }
            // for sRGB mode:
            //    ignore the movie's gamma and use sRGB instead
            //    ignore the movie's gamut and use sRGB instead

            // temporary!!!
            // linearize as sRGB to roundtrip the toGamma() at the bottom of this shader
            color.rgb = toLinear(color.rgb);

            // don't forget to set alpha
            color.a = 1.0;
        }
        // This stanza pertains to 2D textures (aside from YUV movies) and textures on 3D objects if advanced lighting is disabled
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

            // check for some discard conditions
            if (isMovie) texture_color.a = 1.0;
            if (isNotFBTexture && texture_color.a == 0.0) discard;
            if (isFBTexture)
            {
                if(all(equal(texture_color.rgb,vec3_splat(0.0)))) discard;

                // This was previously in gamma space, so linearize again.
                texture_color.rgb = toLinear(texture_color.rgb);
            }

            // multiply by v_color0
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

    // return to sRGB gamma space so we can do alpha blending the same way FF7/8 did.
    color.rgb = toGamma(color.rgb);

    // In this default shader, lighting is applied in gamma space so that it does better match the original lighting
    if ((gameLightingMode == GAME_LIGHTING_PER_PIXEL))
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
    
    // TODO: relocate this stanza to a separate shader for YUVmovies
    // if we did a movie gamut conversion, and won't dither later, then dither now
    // do this in gamma space so that dither step size is proportional to quantization step size
    if (isTexture && !(isOverallNTSCJColorGamut) && isYUV && !(isSRGBColorGamut))
    {
      ivec2 dimensions = textureSize(tex_0, 0);
      color.rgb = QuasirandomDither(color.rgb, v_texcoord0.xy, dimensions, dimensions, dimensions, 256.0, 4320.0);
    }

    gl_FragColor = color;
}
