/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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

uniform vec4 FSHDRFlags;

#define isHDR FSHDRFlags.x > 0.0
#define monitorNits FSHDRFlags.y

void main()
{
	vec4 color = texture2D(tex_0, v_texcoord0.xy);

	if (isHDR) {
		// change primaries from sRGB/rec709 to rec2020 and remap the white point on top of the current monitor nits value
		color.rgb = ApplyREC2084Curve(REC709toREC2020(color.rgb) * monitorNits);
	}

	gl_FragColor = color;
}
