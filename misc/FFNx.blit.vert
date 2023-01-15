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

$input a_position, a_texcoord0
$output v_texcoord0

#include <bgfx/bgfx_shader.sh>

void main()
{
    vec4 pos = a_position;

    pos.w = 1.0 / pos.w;
    pos.xyz *= pos.w;
    pos = mul(u_proj, pos);

    gl_Position = pos;
    v_texcoord0 = a_texcoord0;
}

