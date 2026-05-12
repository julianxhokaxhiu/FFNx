/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
//    Copyright (C) 2026 ChthonVII                                          //
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

// This shader is always used for YUV movies if NTSC-J mode is disabled.

$input v_color0, v_texcoord0, v_position0, v_normal0

#include <bgfx/bgfx_shader.sh>
#include "FFNx.common.sh"
#include "FFNx.moviefunctions.sh"
#include "FFNx.dither.sh"

SAMPLER2D(tex_0, 0);
SAMPLER2D(tex_1, 1);
SAMPLER2D(tex_2, 2);

uniform mat4 invViewMatrix;

uniform vec4 FSMovieFlags;
uniform vec4 FSMoreMovieFlags;
uniform vec4 TimeColor;
uniform vec4 TimeData;
uniform vec4 FSHDRFlags;


// ---
#define isFullRange FSMovieFlags.w > 0.0


#define isBT601ColorMatrix abs(FSMovieFlags.x - 0.0) < 0.00001
#define isBT709ColorMatrix abs(FSMovieFlags.x - 1.0) < 0.00001
#define isBinkColorMatrix abs(FSMovieFlags.x - 2.0) < 0.00001

#define isSRGBColorGamut abs(FSMovieFlags.y - 0.0) < 0.00001
#define isNTSCJColorGamut abs(FSMovieFlags.y - 1.0) < 0.00001
#define isSMPTECColorGamut abs(FSMovieFlags.y - 2.0) < 0.00001
#define isRawP22ColorGamut abs(FSMovieFlags.y - 3.0) < 0.00001

#define isSRGBGamma abs(FSMovieFlags.z - 0.0) < 0.00001
#define is170MGamma abs(FSMovieFlags.z - 1.0) < 0.00001
#define isCRTGamma abs(FSMovieFlags.z - 2.0) < 0.00001

#define chromaLocation FSMoreMovieFlags.x

#define isTimeEnabled TimeData.x > 0.0
#define isTimeFilterEnabled TimeData.x > 0.0 && TimeData.y > 0.0

#define isHDR FSHDRFlags.x > 0.0


void main()
{
    // This variable is clobbered for YUV movies.
    vec4 color = v_color0;

    // At this juncture, ffmpeg has decoded our video file,
    // and the metadata we need has been passed as uniforms.
    // Now we need to do the following:
    //  1. Correct for chroma sampling location
    //  2. Crop any green crap from a padded bitstream
    //  3. Normalize Y'UV ranges and, if limited (tv) range, expand to full (pc) range
    //  2. Convert Y'UV to gamma-space R'G'B'
    //  3. Convert gamma-space R'G'B' to linear RGB.
    //  4. Convert from the video's gamut to our working gamut.
    //  5. Convert from linear R'G'B' to our working gamma-space.

    // In sRGB mode, our working gamut and gamma is sRGB.
    // Also, in sRGB mode, we simply ignore the video's gamma and gamut and treat them as if they were sRGB.
    // This is wrong in almost every case, but simple, fast, and consistent with how 2D/3D assets are rendered in sRGB mode.

    // We need to account for chroma sampling location.
    // Since U & V should always be set to BGFX_SAMPLER_XXX_ANISOTROPIC,
    // all we need do is move our sampling coordinates the opposite of the chroma sampling location's offset from center,
    // and the texture sampler's blending should do the rest.
    // (This would be more clear as a const array of vec2, but bgfx's shader language doesn't like that.)
    float chromaxoff = (mod(chromaLocation, 2.0) < 0.5) ? 0.25 : 0.0;
    float chromayoff = (chromaLocation < 1.1) ? 0.25 : -2.5;
    chromayoff = (chromaLocation < 3.1) ? 0.0 : chromayoff;
    vec2 uvdxdy = vec2_splat(1.0) / textureSize(tex_1, 0);
    vec2 chromaoffset = uvdxdy * vec2(chromaxoff, chromayoff);

    // fetch Y'UV from 3 textures
    vec3 yuv = vec3(
        texture2D(tex_0, v_texcoord0.xy).r,
        texture2D(tex_1, v_texcoord0.xy + chromaoffset).r,
        texture2D(tex_2, v_texcoord0.xy + chromaoffset).r
    );

    // swscale guarantees our input pixel format is YUV420P10
    // scale the 10 bits of data to fill up the 16 bits it's stored in
    yuv *= vec3_splat(65535/1023);
    // dither because we increased bit depth
    // (unless HDR, since we will dither later)
    if (!(isHDR)){
      ivec2 ydimensions = textureSize(tex_0, 0);
      ivec2 udimensions = textureSize(tex_1, 0);
      ivec2 vdimensions = textureSize(tex_2, 0);
      yuv = QuasirandomDither(yuv, v_texcoord0.xy, ydimensions, udimensions, vdimensions, vec3_splat(1024.0), 1.0);
    }

    // Convert Y'UV to R'G'B'
    if (isFullRange){
        yuv = normalizefullrangeYUV(yuv);
    }
    else {
        // we already dithered out to 16 bits, so we don't need another dither for range expansion
        yuv = normalizelimitedrangeYUV(yuv, isBinkColorMatrix);
    }
    // matrix Y'UV to R'G'B'
    if (isBT601ColorMatrix){
        color.rgb = toRGB_bt601_fullrange(yuv);
    }
    else if (isBT709ColorMatrix){
        color.rgb = toRGB_bt709_fullrange(yuv);
    }
    else if (isBinkColorMatrix){
        color.rgb = toRGB_bink_fullrange(yuv);
    }


    // for sRGB mode:
    //    ignore the movie's gamma and use sRGB instead
    //    ignore the movie's gamut and use sRGB instead

    // don't forget to set alpha
    color.a = 1.0;

    // Do we really want time filter applying to movies at all?
    // (currently this is set in gl_draw_movie_quad_common() in gl.cpp)
    if (isTimeFilterEnabled){
      color.rgb = toLinear(color.rgb);
      color.rgb *= TimeColor.rgb;
      color.rgb = toGamma(color.rgb);
    }

    // Don't dither here.
    // NTSC-J mode will always get dithered in post, as will sRGB+HDR mode. And sRGB+SDR mode doesn't need it.

    gl_FragColor = color;
}
