/****************************************************************************/
//    Copyright (C) 2022 Cosmos                                             //
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

$input a_position, a_color0
$output v_position0, v_color0, v_shadow0

#include <bgfx/bgfx_shader.sh>

uniform mat4 d3dViewport;
uniform mat4 d3dProjection;
uniform mat4 worldView;
uniform mat4 lightViewProjTexMatrix;

void main()
{
    v_position0 = vec4(a_position.xyz, 1.0);
    v_color0 = a_color0;
    v_shadow0 = mul(mul(lightViewProjTexMatrix, worldView), v_position0);
    gl_Position = mul(mul(d3dViewport, mul(d3dProjection, worldView)), v_position0);
}
