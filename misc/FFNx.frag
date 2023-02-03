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

#define isTimeEnabled TimeData.x > 0.0
#define isTimeFilterEnabled TimeData.x > 0.0 && TimeData.y > 0.0

void main()
{
	vec4 color = vec4(toLinear(v_color0.rgb), v_color0.a);

    if (isTexture)
    {
        if (isYUV)
        {
            const mat3 mpeg_rgb_transform = mat3(
                vec3(+1.164, +1.164, +1.164),
                vec3(+0.000, -0.392, +2.017),
                vec3(+1.596, -0.813, +0.000)
            );

            const mat3 jpeg_rgb_transform = mat3(
                vec3(+1.000, +1.000, +1.000),
                vec3(+0.000, -0.343, +1.765),
                vec3(+1.400, -0.711, +0.000)
            );

            vec3 yuv = vec3(
                texture2D(tex_0, v_texcoord0.xy).r - (1.0 / 16.0),
                texture2D(tex_1, v_texcoord0.xy).r - 0.5,
                texture2D(tex_2, v_texcoord0.xy).r - 0.5
            );

            if (isFullRange) color.rgb = instMul(jpeg_rgb_transform, yuv);
            else color.rgb = instMul(mpeg_rgb_transform, yuv);

            if (isMovie){
		color.rgb = toLinearSMPTE170M(color.rgb);
	    }
	    else {
		color.rgb = toLinear(color.rgb);
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
