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

vec3 toLinear(vec3 _rgb)
{
	bvec3 cutoff = lessThan(_rgb.rgb, vec3_splat(0.04045));
	vec3 higher = pow((_rgb.rgb + vec3_splat(0.055)) / vec3_splat(1.055), vec3_splat(2.4));
	vec3 lower = _rgb.rgb / vec3_splat(12.92);

	return mix(higher, lower, cutoff);
}

vec3 toGamma(vec3 _rgb)
{
	bvec3 cutoff = lessThan(_rgb.rgb, vec3_splat(0.0031308));
	vec3 higher = vec3_splat(1.055) * pow(_rgb.rgb, vec3_splat(1.0/2.4)) - vec3_splat(0.055);
	vec3 lower = _rgb.rgb * vec3_splat(12.92);

	return mix(higher, lower, cutoff);
}

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
