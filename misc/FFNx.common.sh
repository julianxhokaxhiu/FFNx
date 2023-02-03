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

vec3 toLinear(vec3 _rgb)
{
	bvec3 cutoff = lessThan(_rgb.rgb, vec3_splat(0.04045));
	vec3 higher = pow((_rgb.rgb + vec3_splat(0.055)) / vec3_splat(1.055), vec3_splat(2.4));
	vec3 lower = _rgb.rgb / vec3_splat(12.92);

	return mix(higher, lower, cutoff);
}

vec3 toLinearSMPTE170M(vec3 _rgb)
{
	bvec3 cutoff = lessThan(_rgb.rgb, vec3_splat(0.081));
	vec3 higher = pow((_rgb.rgb + vec3_splat(0.099)) / vec3_splat(1.099), (vec3_splat(1.0) / vec3_splat(0.45)));
	vec3 lower = _rgb.rgb / vec3_splat(4.5);

	return mix(higher, lower, cutoff);
}

vec3 toGamma(vec3 _rgb)
{
	bvec3 cutoff = lessThan(_rgb.rgb, vec3_splat(0.0031308));
	vec3 higher = vec3_splat(1.055) * pow(_rgb.rgb, vec3_splat(1.0/2.4)) - vec3_splat(0.055);
	vec3 lower = _rgb.rgb * vec3_splat(12.92);

	return mix(higher, lower, cutoff);
}

// See https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Shaders/ColorSpaceUtility.hlsli#L120
vec3 REC709toREC2020(vec3 _rgb)
{
	mat3 toRec2020 = mat3(
		0.627402, 0.329292, 0.043306,
		0.069095, 0.919544, 0.011360,
		0.016394, 0.088028, 0.895578
	);
	return mul(toRec2020, _rgb);
}

// See https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Shaders/ColorSpaceUtility.hlsli#L75
vec3 ApplyREC2084Curve(vec3 _color)
{
	// reference PQ OETF will yield reference OOTF when
	// displayed on  a reference monitor employing EOTF

	float m1 = 2610.0 / 4096.0 * 1.0 / 4;
	float m2 = 2523.0 / 4096.0 * 128;
	float c1 = 3424.0 / 4096.0;
	float c2 = 2413.0 / 4096.0 * 32;
	float c3 = 2392.0 / 4096.0 * 32;

	vec3 Lp = pow(_color * (1.0/10000.0), vec3_splat(m1));
	return pow((c1 + c2 * Lp) / (vec3_splat(1.0) + c3 * Lp), vec3_splat(m2));
}
