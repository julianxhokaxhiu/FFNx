/****************************************************************************/
//    Copyright (C) 2023 Cosmos                                             //
//    Copyright (C) 2026 ChthonVII                                          //
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

// Gamma functions ------------------------------------------------
// sRGB and rec2084 gamma functions.
// Other gamma functions used for movies and color correction are in FFNx.moviefunctions.sh and FFNx.colorfunctions.sh

// gamma encoded --> linear:

// sRGB
vec3 toLinear(vec3 _rgb)
{
	bvec3 cutoff = lessThan(_rgb.rgb, vec3_splat(0.04045));
	vec3 higher = pow((_rgb.rgb + vec3_splat(0.055)) / vec3_splat(1.055), vec3_splat(2.4));
	vec3 lower = _rgb.rgb / vec3_splat(12.92);

	return saturate(mix(higher, lower, cutoff));
}

// linear --> gamma encoded:

// sRGB
vec3 toGamma(vec3 _rgb)
{
	bvec3 cutoff = lessThan(_rgb.rgb, vec3_splat(0.0031308));
	vec3 higher = vec3_splat(1.055) * pow(_rgb.rgb, vec3_splat(1.0/2.4)) - vec3_splat(0.055);
	vec3 lower = _rgb.rgb * vec3_splat(12.92);

	return saturate(mix(higher, lower, cutoff));
}

// See https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Shaders/ColorSpaceUtility.hlsli#L75
// max_nits should be "the brightness level that SDR 'white' is rendered at within an HDR monitor" (probably 100-200ish)
// Google Chrome uses a default of 200 if autodetection fails.
vec3 ApplyREC2084Curve(vec3 _color, float max_nits)
{
	// reference PQ OETF will yield reference OOTF when
	// displayed on  a reference monitor employing EOTF

	float m1 = 2610.0 / 4096.0 * 1.0 / 4;
	float m2 = 2523.0 / 4096.0 * 128;
	float c1 = 3424.0 / 4096.0;
	float c2 = 2413.0 / 4096.0 * 32;
	float c3 = 2392.0 / 4096.0 * 32;

	vec3 Lp = pow(_color * (vec3_splat(max_nits)/vec3_splat(10000.0)), vec3_splat(m1));
	return saturate(pow((c1 + c2 * Lp) / (vec3_splat(1.0) + c3 * Lp), vec3_splat(m2)));
}

// Note: High precision values are used for the "D65" whitepoint. (x=0.312713, y=0.329016)

// To rec2020:
// See https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Shaders/ColorSpaceUtility.hlsli#L120
// (This is here, rather than FFNx.colorfunctions.sh to maintain segregation of NTSC-J mode code.)
vec3 convertGamut_SRGBtoREC2020(vec3 rgb_input)
{
	mat3 toRec2020 = mtxFromCols(
		vec3(+0.6274191906, +0.0691004930, +0.0163944170),
		vec3(+0.3292752691, +0.9195389158, +0.0880250760),
		vec3(+0.0433055403, +0.0113605912, +0.8955805070)
	);
	return saturate(mul(toRec2020, rgb_input));
}

// Fog ---------------------------------------------
vec3 ApplyWorldFog(vec3 color, vec3 viewPosition)
{
	float d = sqrt(dot(viewPosition, viewPosition));
	float s0 = 0;
	float e0 = 10000;

	float density = 0.00025;
	float t = 1 / exp (d * density);

	vec3 fogColor0 = vec3(0.1, 0.1, 0.2);
	vec3 outColor = mix(color * fogColor0, color, t);

	float e1 = 15000;
	float t2 = 1.0 - saturate((d - e0) / e1);
	outColor *= t2;

	return outColor;
}

// Spherical world  ---------------------------------------------
#define cplx vec2
#define cplx_new(re, im) vec2(re, im)
#define cplx_re(z) z.x
#define cplx_im(z) z.y
#define cplx_exp(z) (exp(z.x) * cplx_new(cos(z.y), sin(z.y)))
#define cplx_scale(z, scalar) (z * scalar)
#define cplx_abs(z) (sqrt(z.x * z.x + z.y * z.y))

vec3 ApplySphericalWorld(vec3 viewPosition, float radiusScale)
{
	vec3 outResult = vec3(0.0, 0.0, 0.0);

	float rp = -250000 * radiusScale;

	vec2 planedir = normalize(vec2(viewPosition.x, viewPosition.z));
	cplx plane = cplx_new(viewPosition.y, sqrt((viewPosition.x) * (viewPosition.x) + (viewPosition.z) * (viewPosition.z)));
	cplx circle = rp * cplx_exp(cplx_scale(plane, 1.0 / rp)) - cplx_new(rp, 0);
	outResult.x = cplx_im(circle) * planedir.x;
	outResult.z = cplx_im(circle) * planedir.y;
	outResult.y = cplx_re(circle);

	return outResult;
}

#define MAX_BONE_MATRICES 128
