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

$input v_color0, v_texcoord0, v_texcoord1, v_texcoord2, v_texcoord3, v_position0, v_shadow0, v_normal0

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
uniform vec4 FSTexFlags;

uniform vec4 lightingSettings;
uniform vec4 lightingDebugData;
uniform vec4 materialData;

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

// ---
#define textureDebugOutput lightingDebugData.z
#define TEXTURE_DEBUG_OUTPUT_DISABLED 0
#define TEXTURE_DEBUG_OUTPUT_COLOR 1
#define TEXTURE_DEBUG_OUTPUT_NORMAL_MAP 2
#define TEXTURE_DEBUG_OUTPUT_ROUGHNESS 3
#define TEXTURE_DEBUG_OUTPUT_METALNESS 4

#define isNmlTextureLoaded FSTexFlags.x > 0.0
#define isPbrTextureLoaded FSTexFlags.y > 0.0

void main()
{
	vec4 color = v_color0;
    vec4 param = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 param2 = vec4(0.0, 0.0, 0.0, 0.0);

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

            color.a = 1.0f;
        }
        else
        {
            vec2 color_uv = vec2(0.0, 0.0);
            if(isTLVertex)
            {
                color_uv = v_texcoord0.xy;
            }
            else
            {
                color_uv = v_texcoord1.xy;
                param = texture2D(tex_0, v_texcoord2.xy);
                param2 = texture2D(tex_0, v_texcoord3.xy);
            }

            vec4 texture_color = texture2D(tex_0, color_uv);

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

            if (isFBTexture && all(equal(texture_color.rgb,vec3_splat(0.0)))) discard;

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
    }
    else
    {
        if(textureDebugOutput == TEXTURE_DEBUG_OUTPUT_COLOR)
        {
            gl_FragColor = color;
        }
        else if(textureDebugOutput == TEXTURE_DEBUG_OUTPUT_NORMAL_MAP)
        {
            gl_FragColor = vec4(param.rgb, 1.0);
        }
        else if(textureDebugOutput == TEXTURE_DEBUG_OUTPUT_ROUGHNESS)
        {
            gl_FragColor = vec4(param2.r, param2.r, param2.r, 1.0);
        }
        else if(textureDebugOutput == TEXTURE_DEBUG_OUTPUT_METALNESS)
        {
            gl_FragColor = vec4(param2.g, param2.g, param2.g, 1.0);
        }
        else
        {
            // Shadow UV
            vec3 shadowUv = v_shadow0.xyz / v_shadow0.w;

            // View Direction
            vec3 viewDir = normalize(v_position0.xyz);

            // Normal
            vec3 normal = normalize(v_normal0);
            if(isNmlTextureLoaded) normal = perturb_normal(normal, -v_position0.xyz, param.rgb, v_texcoord2.xy );

            // Roughness
            float roughness = materialData.x;
            if(isPbrTextureLoaded) roughness = param2.r * materialData.z;
            float roughnessClamped = max(0.001, roughness);

            // Metalness
            float metalness = materialData.y;
            if(isPbrTextureLoaded) metalness = param2.g * materialData.w;
            float metalnessClamped = min(1.0, metalness);

            // Ambient Occlusion
            float ao = 1.0;
            if(isPbrTextureLoaded) ao = param2.b;

            // Luminance
            vec3 luminance = calcLuminance(color.rgb, v_position0.xyz, viewDir, normal, roughnessClamped, metalnessClamped, ao, shadowUv);

            gl_FragColor = vec4(luminance, color.a);
        }
    }
}
