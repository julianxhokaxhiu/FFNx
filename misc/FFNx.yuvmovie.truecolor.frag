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

// This shader is always used for YUV movies if NTSC-J mode is enabled.

$input v_color0, v_texcoord0, v_position0, v_normal0

#include <bgfx/bgfx_shader.sh>
#include "FFNx.common.sh"

SAMPLER2D(tex_0, 0);
SAMPLER2D(tex_1, 1);
SAMPLER2D(tex_2, 2);

uniform mat4 invViewMatrix;

uniform vec4 FSMovieFlags;
uniform vec4 FSMoreMovieFlags;
uniform vec4 TimeColor;
uniform vec4 TimeData;


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

#define YhorizontalCropScaleFactor FSMoreMovieFlags.y
#define UVhorizontalCropScaleFactor FSMoreMovieFlags.z

#define isTimeEnabled TimeData.x > 0.0
#define isTimeFilterEnabled TimeData.x > 0.0 && TimeData.y > 0.0


void main()
{
    // This variable is clobbered for YUV movies.
    vec4 color = v_color0;

    // At this juncture, ffmpeg has decoded our video file,
    // and the metadata we need has been passed as uniforms.
    // Now we need to do the following:
    //  0. Correct for chroma sampling location
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

    // Video bitstreams might be padded out to a multiple of blocksize or whatever.
    // So we may need to crop off the resulting green crap.
    vec2 Ycoord = v_texcoord0.xy;
    Ycoord.x = min(Ycoord.x * YhorizontalCropScaleFactor, YhorizontalCropScaleFactor);
    vec2 UVcoord = v_texcoord0.xy + chromaoffset;
    UVcoord.x = min(UVcoord.x * UVhorizontalCropScaleFactor, UVhorizontalCropScaleFactor);

    // fetch YUV from 3 textures
    vec3 yuv = vec3(
        texture2D(tex_0, Ycoord).r,
        texture2D(tex_1, UVcoord).r,
        texture2D(tex_2, UVcoord).r
    );

    // Convert YUV to RGB
    if (isFullRange){
        // center UV at 128
        yuv.g = yuv.g - (128.0/255.0);
        yuv.b = yuv.b - (128.0/255.0);
        // fix uv scale b/c 128 isn't quite halfway between 0 and 255
        float uscalar = (yuv.g < 0.0) ? 255.0/256.0 : 255.0/254.0;
        float vscalar = (yuv.b < 0.0) ? 255.0/256.0 : 255.0/254.0;
        yuv.g = clamp(yuv.g * uscalar, -0.5, 0.5);
        yuv.b = clamp(yuv.b * vscalar, -0.5, 0.5);
    }
    else {
        // if not full range, expand to full range

        // dither
        ivec2 ydimensions = textureSize(tex_0, 0);
        ivec2 udimensions = textureSize(tex_1, 0);
        ivec2 vdimensions = textureSize(tex_2, 0);
        yuv = QuasirandomDither(yuv, v_texcoord0.xy, ydimensions, udimensions, vdimensions, 256.0, 1.0);
        // subtract 16 from Y
        yuv.r = saturate(yuv.r - (16.0/255.0));
        // scale Y
        // Common video standard for Y range is 16-235, but bink uses 16-234
        float yscalar = (isBinkColorMatrix) ? 255.0/218.0 : 255.0/219.0;
        yuv.r = saturate(yuv.r * yscalar);
        // center UV at 128
        yuv.g = yuv.g - (128.0/255.0);
        yuv.b = yuv.b - (128.0/255.0);
        // scale UV
        yuv.g = clamp(yuv.g * (255.0/224.0), -0.5, 0.5);
        yuv.b = clamp(yuv.b * (255.0/224.0), -0.5, 0.5);
    } // end else (not full range)
    // matrix YUV to RGB
    if (isBT601ColorMatrix){
        color.rgb = toRGB_bt601_fullrange(yuv);
    }
    else if (isBT709ColorMatrix){
        color.rgb = toRGB_bt709_fullrange(yuv);
    }
    else if (isBinkColorMatrix){
        color.rgb = toRGB_bink_fullrange(yuv);
    }




    // Convert gamma to CRT gamma (BT1886 Appendix 1) and gamut to NTSC-J

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
