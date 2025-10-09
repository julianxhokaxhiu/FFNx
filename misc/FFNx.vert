/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
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

$input a_position, a_color0, a_texcoord0, a_normal
$output v_color0, v_texcoord0, v_position0, v_normal0

#include <bgfx/bgfx_shader.sh>
#include "FFNx.common.sh"

uniform mat4 d3dViewport;
uniform mat4 d3dProjection;
uniform mat4 worldView;
uniform mat4 normalMatrix;
uniform mat4 invViewMatrix;
uniform vec4 gameLightingFlags;
uniform vec4 gameGlobalLightColor;
uniform vec4 gameLightColor1;
uniform vec4 gameLightColor2;
uniform vec4 gameLightColor3;
uniform vec4 gameLightDir1;
uniform vec4 gameLightDir2;
uniform vec4 gameLightDir3;
uniform vec4 gameScriptedLightColor;

uniform vec4 VSFlags;
uniform vec4 WMFlags;

#define isTLVertex VSFlags.x > 0.0
#define blendMode VSFlags.y
#define isFBTexture VSFlags.z > 0.0

#define isApplySphericalWorld WMFlags.x > 0.0
#define sphericaWorldRadiusScale  WMFlags.x

#define gameLightingMode gameLightingFlags.x
#define GAME_LIGHTING_PER_VERTEX 1

void main()
{
    vec4 pos = a_position;
    vec4 color = a_color0;
    vec2 coords = a_texcoord0;

    color.rgb = color.bgr;

    if (isTLVertex)
    {
        pos.w = 1.0 / pos.w;
        pos.xyz *= pos.w;
        pos = mul(u_proj, pos);
    }
    else
    {
        v_position0 = mul(worldView, vec4(pos.xyz, 1.0));

        if (isApplySphericalWorld) pos.xyz = ApplySphericalWorld(v_position0.xyz, sphericaWorldRadiusScale);
        else pos = v_position0;

        pos = mul(mul(d3dViewport, d3dProjection), vec4(pos.xyz, 1.0));
        v_normal0 = mul(normalMatrix, vec4(a_normal, 0.0)).xyz;

        // In this default shader, lighting is applied in gamma space so that it does better match the original lighting
        if (gameLightingMode == GAME_LIGHTING_PER_VERTEX)
        {
            vec3 worldNormal = mul(invViewMatrix, vec4(v_normal0, 0)).xyz;
            float dotLight1 = saturate(dot(worldNormal, gameLightDir1.xyz));
            float dotLight2 = saturate(dot(worldNormal, gameLightDir2.xyz));
            float dotLight3 = saturate(dot(worldNormal, gameLightDir3.xyz));
            vec3 light1Ambient = gameLightColor1.rgb * dotLight1 * dotLight1;
            vec3 light2Ambient = gameLightColor2.rgb * dotLight2 * dotLight2;
            vec3 light3Ambient = gameLightColor3.rgb * dotLight3 * dotLight3;
            vec3 lightAmbient = gameScriptedLightColor.rgb * (gameGlobalLightColor.rgb + light1Ambient + light2Ambient + light3Ambient);
            color.rgb *= gameGlobalLightColor.w * lightAmbient;
        }

        if (color.a > 0.5) color.a = 0.5;
    }

    if (blendMode == 4.0) color.a = 1.0;
    else if (blendMode == 3.0) color.a = 0.25;

#if BGFX_SHADER_LANGUAGE_HLSL
#else
    #if BGFX_SHADER_LANGUAGE_SPIRV
    #else
        if (isFBTexture) coords.y = 1.0 - coords.y;
    #endif
#endif

    gl_Position = pos;
    v_color0 = color;
    v_texcoord0 = coords;
}

