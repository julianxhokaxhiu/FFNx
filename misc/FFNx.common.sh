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

vec3 ApplyKawaiColorOffset(vec3 color, vec4 offset)
{
	if (offset.w > 0.0) return saturate(color + offset.rgb);
	return color;
}

vec3 ExpandPSXWorldColor(vec3 color5)
{
	return toLinear((color5 * 8.0 + floor(color5 / 4.0)) / 255.0);
}

float GetPSXWorldDither(vec2 screenPosition, vec4 settings)
{
	if (settings.w < 0.5 || settings.w > 1.5) return 0.0;
	if (settings.z > 0.0) screenPosition.y = settings.z - screenPosition.y;
	vec2 pixel = mod(floor(screenPosition / settings.xy), vec2_splat(4.0));
	vec4 row = pixel.y < 1.0 ? vec4(-4.0, 0.0, -3.0, 1.0) :
		pixel.y < 2.0 ? vec4(2.0, -2.0, 3.0, -1.0) :
		pixel.y < 3.0 ? vec4(-3.0, 1.0, -4.0, 0.0) : vec4(3.0, -1.0, 2.0, -2.0);
	return row[int(pixel.x)] / 8.0;
}

vec3 ApplyPSXWorldColorPrecision(vec3 encodedColor, float dither)
{
	return ExpandPSXWorldColor(clamp(floor(saturate(encodedColor) * (255.0 / 8.0) + dither + 0.00001),
		vec3_splat(0.0), vec3_splat(31.0)));
}

vec3 ApplyPSXWorldAverageDither(vec3 encodedColor)
{
	// Each value from -4 to 3 occurs twice in PSX's 4x4 matrix.
	vec3 color8 = vec3_splat(0.0);
	for (int d = -4; d < 4; ++d)
	{
		vec3 color5 = clamp(floor(saturate(encodedColor) * (255.0 / 8.0) + float(d) / 8.0 + 0.00001),
			vec3_splat(0.0), vec3_splat(31.0));
		color8 += color5 * 8.0 + floor(color5 / 4.0);
	}
	return toLinear(color8 / (8.0 * 255.0));
}

vec3 ApplyPSXWorldModulation(vec3 lightingColor, vec3 textureColor, float dither, float ditherMode, bool fractionalTexture)
{
	// Recover the 5-bit texel before modulation; expanded 8-bit colors change the product.
	// Generated terrain mips store linear texture5 * 8 and retain filtered fractional values.
	vec3 texture5 = fractionalTexture ? textureColor * (255.0 / 8.0) :
		floor(floor(toGamma(textureColor) * 255.0 + 0.5) / 8.0);
	vec3 lighting8 = floor(saturate(lightingColor) * 255.0 + 0.0001);
	if (ditherMode > 1.5)
		return ApplyPSXWorldAverageDither(texture5 * lighting8 / (16.0 * 255.0));
	vec3 color5 = clamp(floor(texture5 * lighting8 / 128.0 + dither), vec3_splat(0.0), vec3_splat(31.0));
	return ExpandPSXWorldColor(color5);
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

// bgfx stores a uniform array's element count in 8 bits, so 255 matrices is the largest array it supports.
// Keep in sync with src/renderer.h
#define MAX_BONE_MATRICES 255
