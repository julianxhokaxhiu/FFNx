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

$input v_color0, v_texcoord0, v_position0, v_shadow0, v_normal0

#include <bgfx/bgfx_shader.sh>

SAMPLER2D(tex_0, 0);

uniform vec4 VSFlags;
uniform vec4 FSAlphaFlags;
uniform vec4 FSTexFlags;

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

void main()
{
	vec4 color = v_color0;

    if (isTexture)
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

        if (texture_color.a == 0.0) discard;

    }
    else if(color.a < 0.0 && color.r + color.g + color.b == 0.0)
	{
        discard;
    }

    gl_FragColor = vec4_splat(0.0);
}
