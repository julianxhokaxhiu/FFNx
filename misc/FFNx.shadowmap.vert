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

$input a_position, a_color0, a_texcoord0, a_normal, a_indices, a_weight
$output v_color0, v_texcoord0, v_position0, v_shadow0, v_normal0

#include <bgfx/bgfx_shader.sh>
#include "FFNx.common.sh"

uniform mat4 worldView;
uniform mat4 lightViewProjMatrix;

uniform mat4 boneMatrices[MAX_BONE_MATRICES];
uniform vec4 skinningFlags;

#define isSmoothSkinning skinningFlags.x > 0.0

void main()
{
	vec4 pos = a_position;
    vec4 color = a_color0;
    vec2 coords = a_texcoord0;

    color.rgba = color.bgra;

    if (color.a > 0.5) color.a = 0.5;
    else if(color.r + color.g + color.b == 0.0)
    {
        color.a = -1;
    }

    if (isSmoothSkinning)
    {
        int boneIndex = a_indices.x;
        vec3 avgPos = vec3(0.0, 0.0, 0.0);
        avgPos += a_weight.x * mul(boneMatrices[a_indices.x], vec4(pos.xyz, 1.0)).xyz;
        avgPos += a_weight.y * mul(boneMatrices[a_indices.y], vec4(pos.xyz, 1.0)).xyz;
        avgPos += a_weight.z * mul(boneMatrices[a_indices.z], vec4(pos.xyz, 1.0)).xyz;
        avgPos += a_weight.w * mul(boneMatrices[a_indices.w], vec4(pos.xyz, 1.0)).xyz;

        pos = vec4(avgPos, 1.0);
    }
    
    pos = mul(mul(lightViewProjMatrix, worldView), vec4(pos.xyz, 1.0));

    gl_Position = pos;
    v_color0 = color;
    v_texcoord0 = coords;
}
