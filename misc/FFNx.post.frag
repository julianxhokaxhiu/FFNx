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

SAMPLER2D(tex_0, 0);

uniform vec4 FSHDRFlags;

#define isHDR FSHDRFlags.x > 0.0
#define monitorNits FSHDRFlags.y

vec3 toXyzFromSrgb(vec3 _rgb)
{
	mat3 toXYZ = mat3(
		0.4125564, 0.3575761, 0.1804375,
		0.2126729, 0.7151522, 0.0721750,
		0.0193339, 0.1191920, 0.9503041
	);
	return mul(toXYZ, _rgb);
}

vec3 toRec2020FromXyz(vec3 _xyz)
{
	mat3 toRec2020 = mat3(
		1.7166512, -0.3556708, -0.2533663,
	   -0.6666844,  1.6164812,  0.0157685,
	    0.0176399, -0.0427706,  0.9421031
	);
	return mul(toRec2020, _xyz);
}

vec3 toPqOetf(vec3 _color)
{
	// reference PQ OETF will yield reference OOTF when
	// displayed on  a reference monitor employing EOTF

	float m1 = 0.1593017578125;
	float m2 = 78.84375;
	float c1 = 0.8359375;
	float c2 = 18.8515625;
	float c3 = 18.6875;

	vec3 Ym1 = pow(_color.xyz * (1.0/10000.0), vec3_splat(m1) );
	_color = pow((c1 + c2*Ym1) / (vec3_splat(1.0) + c3*Ym1), vec3_splat(m2) );
	return _color;
}

void main()
{
	vec4 color = texture2D(tex_0, v_texcoord0.xy);

	if (isHDR) {
		// change primaries from sRGB/rec709 to rec2020 and remap the white point on top of the current monitor nits value
		color.rgb = toPqOetf(toRec2020FromXyz(toXyzFromSrgb(color.xyz)) * monitorNits);
	}

	gl_FragColor = color;
}
