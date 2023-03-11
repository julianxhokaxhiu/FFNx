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
	bvec3 cutoff = lessThan(_rgb.rgb, vec3_splat(0.0812));
	vec3 higher = pow((_rgb.rgb + vec3_splat(0.099)) / vec3_splat(1.099), (vec3_splat(1.0) / vec3_splat(0.45)));
	vec3 lower = _rgb.rgb / vec3_splat(4.5);

	return mix(higher, lower, cutoff);
}

vec3 toLinear2pt2(vec3 _rgb)
{
	return pow(_rgb.rgb, vec3_splat(2.2));
}

// Microsoft says PAL uses a pure 2.8 gamma curve. See: https://learn.microsoft.com/en-us/windows/win32/api/mfobjects/ne-mfobjects-mfvideotransferfunction
// ffmpeg thinks there *should* be a linear toe slope, but uses a pure curve since they cannot find any documentation for it. See: https://github.com/FFmpeg/FFmpeg/blob/master/libavfilter/vf_colorspace.c#L162
// In any event Poynton says 2.8 is "unrealistically high" and PAL CRT units did not really behave like that
// PAL switched to the SMPTE170M function in 2005 (see BT1700)
vec3 toLinear2pt8(vec3 _rgb)
{
	return pow(_rgb.rgb, vec3_splat(2.8));
}


vec3 toLinearToelessSRGB(vec3 _rgb)
{
    vec3 twoPtwo = toLinear2pt2(_rgb);
    vec3 sRGB = toLinear(_rgb);
    bvec3 useSRGB = lessThan(sRGB, twoPtwo);
    vec3 proportion = pow(_rgb / vec3_splat(0.389223), vec3_splat(1.0 / 2.2));
    vec3 merged = mix(twoPtwo, sRGB, proportion);
    return mix(merged, sRGB, useSRGB);
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

// Apply Martin Roberts' quasirandom dithering below 8 bits of precision.
// See https://extremelearning.com.au/unreasonable-effectiveness-of-quasirandom-sequences/
// pixelval: float (range 0-1) pixel color trio, assumed to have been read in from a trio of 8-bit values.
// coords: float (range 0-1) pixel coordinates, i.e., v_texcoord0.xy
// ydims: integer dimensions of first channel's texture
// udims & vdims: integer dimensions of first second and third channels' textures (may differ from ydim for various yuv formats)
vec3 QuasirandomDither(vec3 pixelval, vec2 coords, ivec2 ydims, ivec2 udims, ivec2 vdims)
{
	// get integer range x,y coords for this pixel
	// invert one axis for u and the other axis for v to decouple dither patterns across channels
	// see https://blog.kaetemi.be/2015/04/01/practical-bayer-dithering/
	// add 1 to avoid x=0 and y=0
	vec3 xpos = vec3(
		round(float(ydims.x) * coords.x)  + 1.0,
		round(float(udims.x) * (1.0 - coords.x)) + 1.0,
		round(float(vdims.x) * coords.x)  + 1.0
	);
	vec3 ypos = vec3(
		round(float(ydims.y) * coords.y) + 1.0,
		round(float(udims.y) * coords.y) + 1.0,
		round(float(vdims.y) * (1.0 - coords.y)) + 1.0
	);
	// R series magic
	vec3 dither = fract((xpos * vec3_splat(0.7548776662)) + (ypos * vec3_splat(0.56984029)));
	// triangular wave function
	// if exactly 0.5, then pass through so we don't get a 1.0
	bvec3 smallcutoff = lessThan(dither, vec3_splat(0.5));
	bvec3 bigcutoff = greaterThan(dither, vec3_splat(0.5));
	dither = mix(dither, dither * vec3_splat(2.0), smallcutoff);
	dither = mix(dither, vec3_splat(2.0) - (dither * vec3_splat(2.0)), bigcutoff);
	// shift down by half
	dither = dither - vec3_splat(0.5);
	// scale down below the precision of the 8-bit input
	dither = dither / vec3_splat(255.0);
	// add to input
	return saturate(pixelval + dither);
}
