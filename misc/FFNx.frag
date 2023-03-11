/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
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

$input v_color0, v_texcoord0

#include <bgfx/bgfx_shader.sh>
#include "FFNx.common.sh"

SAMPLER2D(tex_0, 0);
SAMPLER2D(tex_1, 1);
SAMPLER2D(tex_2, 2);

uniform vec4 VSFlags;
uniform vec4 FSAlphaFlags;
uniform vec4 FSMiscFlags;
uniform vec4 FSHDRFlags;
uniform vec4 FSTexFlags;
uniform vec4 FSMovieFlags;
uniform vec4 TimeColor;
uniform vec4 TimeData;

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

#define isTimeEnabled TimeData.x > 0.0
#define isTimeFilterEnabled TimeData.x > 0.0 && TimeData.y > 0.0

void main()
{
	vec4 color = vec4(toLinear(v_color0.rgb), v_color0.a);

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
                yuv = QuasirandomDither(yuv, v_texcoord0.xy, ydimensions, udimensions, vdimensions);
                // clamp back to tv range
                yuv = clamp(yuv, vec3_splat(16.0/255.0), vec3(235.0/255.0, 240.0/255.0, 240.0/255.0));
            }
#endif
            
            if (isBT601ColorMatrix){
                yuv.g = yuv.g - (128.0/255.0);
                yuv.b = yuv.b - (128.0/255.0);
                if (isFullRange){
                    // BT601 full-range YUV->RGB conversion
                    const mat3 jpeg_rgb_transform = mat3(
                        vec3(+1.000, +1.000, +1.000),
                        vec3(+0.000, -0.202008 / 0.587, +1.772),
                        vec3(+1.402, -0.419198 / 0.587, +0.000)
                    );
                    color.rgb = saturate(instMul(jpeg_rgb_transform, yuv));
                }
                else {
                    yuv.r = saturate(yuv.r - (16.0/255.0));
                    // BT601 TV-range YUV->RGB conversion
                    // (includes implict range conversion)
                    const mat3 mpeg_rgb_transform = mat3(
                        vec3(+255.0 / 219.0, +255.0 / 219.0, +255.0 / 219.0),
                        vec3(+0.000, -25.75602 / 65.744 , +225.93 / 112.0),
                        vec3(+178.755 / 112.0, -53.447745 / 65.744 , +0.000)
                    );
                    color.rgb = saturate(instMul(mpeg_rgb_transform, yuv));
                }
            
            }
            else if (isBT709ColorMatrix){
                yuv.g = yuv.g - (128.0/255.0);
                yuv.b = yuv.b - (128.0/255.0);
                if (isFullRange){
                    // BT709 full-range YUV->RGB conversion
                    const mat3 bt709full_rgb_transform = mat3(
                        vec3(+1.000, +1.000, +1.000),
                        vec3(+0.000, -0.13397432 / 0.7152, +1.8556),
                        vec3(+1.5748, -0.33480248 / 0.7152 , +0.000)
                    );
                    color.rgb = saturate(instMul(bt709full_rgb_transform, yuv));
                }
                else {
                    yuv.r = saturate(yuv.r - (16.0/255.0));
                    // BT709 tv-range YUV->RGB conversion
                    // (includes implict range conversion)
                    const mat3 bt709tv_rgb_transform = mat3(
                        vec3(+255.0 / 219.0, +255.0 / 219.0, +255.0 / 219.0),
                        vec3(+0.000, -17.0817258 / 80.1024 , +236.589 / 112.0),
                        vec3(+200.787 / 112.0, -42.6873162 / 80.1024 , +0.000)
                    );
                    color.rgb = saturate(instMul(bt709tv_rgb_transform, yuv));
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
                color.rgb = saturate(toLinearToelessSRGB(color.rgb));
            }
            else if (is2pt2Gamma){
                color.rgb = saturate(toLinear2pt2(color.rgb));
            }
            else if (is170MGamma){
                color.rgb = saturate(toLinearSMPTE170M(color.rgb));
            }
            else if (is2pt8Gamma){
                color.rgb = saturate(toLinear2pt8(color.rgb));
            }
            else {
                color.rgb = saturate(toLinear(color.rgb));
            }
            
            // Convert gamut to BT709/SRGB.
            // For SDR, we should do this to match the output device's gamut.
            // For HDR, we should do this so we have BT709 input to feed to REC709toREC2020()
            // Use of NTSC-J as the source gamut  for the original videos and their derivatives is a *highly* probable guess:
            // It looks correct, is consistent with the PS1's movie decoder chip's known use of BT601 color matrix, and conforms with Japanese TV standards of the time.
            if (isNTSCJColorGamut){
                const mat3 NTSCJ_to_bt709_gamut_transform = mat3(
                    vec3(+1.42849423843304, -0.028230868456879, -0.026451048534459),
                    vec3(-0.343794575385404, +0.937886666562635, -0.04977408617468),
                    vec3(-0.084699613295359, +0.09034421347425, +1.07622507193376)
                );
                color.rgb = saturate(instMul(NTSCJ_to_bt709_gamut_transform, color.rgb));
            }
            else if (isSMPTECColorGamut){
                const mat3 SMPTEC_to_bt709_gamut_transform = mat3(
                    vec3(+0.93954641805697, +0.0177731581035936, -0.0016219287899825),
                    vec3(+0.0501790284093381, +0.965794379088605, -0.00437041275295984),
                    vec3(+0.0102745535336914, +0.016432462807801, +1.00599234154294)
                );
                color.rgb = saturate(instMul(SMPTEC_to_bt709_gamut_transform, color.rgb));
            }
            else if (isEBUColorGamut){
                const mat3 EBU_to_bt709_gamut_transform = mat3(
                    vec3(+1.04404109596867, +0.000, -0.000),
                    vec3(-0.0440410959686678, +1.0, +0.0117951493631932),
                    vec3(+0.000, +0.000, +0.988204850636807)
                );
                color.rgb = saturate(instMul(EBU_to_bt709_gamut_transform, color.rgb));
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

    if (isTimeFilterEnabled) color.rgb *= TimeColor.rgb;

    if (!(isHDR)) {
        // SDR screens require the Gamma output to properly render light scenes
        color.rgb = toGamma(color.rgb);
    }

    gl_FragColor = color;
}
