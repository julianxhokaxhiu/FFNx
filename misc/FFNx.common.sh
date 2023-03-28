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

// YUV to RGB ---------------------------------------------------------
// tv-range functions include implicit range expansion

vec3 toRGB_bt601_fullrange(vec3 yuv_input)
{
	const mat3 jpeg_rgb_transform = mat3(
		vec3(+1.000, +1.000, +1.000),
		vec3(+0.000, -0.202008 / 0.587, +1.772),
		vec3(+1.402, -0.419198 / 0.587, +0.000)
	);
	return saturate(instMul(jpeg_rgb_transform, yuv_input));
}

vec3 toRGB_bt601_tvrange(vec3 yuv_input)
{
	const mat3 mpeg_rgb_transform = mat3(
		vec3(+255.0 / 219.0, +255.0 / 219.0, +255.0 / 219.0),
		vec3(+0.000, -25.75602 / 65.744 , +225.93 / 112.0),
		vec3(+178.755 / 112.0, -53.447745 / 65.744 , +0.000)
	);
	return saturate(instMul(mpeg_rgb_transform, yuv_input));
}

vec3 toRGB_bt709_fullrange(vec3 yuv_input)
{
	const mat3 bt709full_rgb_transform = mat3(
		vec3(+1.000, +1.000, +1.000),
		vec3(+0.000, -0.13397432 / 0.7152, +1.8556),
		vec3(+1.5748, -0.33480248 / 0.7152 , +0.000)
	);
	return saturate(instMul(bt709full_rgb_transform, yuv_input));
}

vec3 toRGB_bt709_tvrange(vec3 yuv_input)
{
	const mat3 bt709tv_rgb_transform = mat3(
		vec3(+255.0 / 219.0, +255.0 / 219.0, +255.0 / 219.0),
		vec3(+0.000, -17.0817258 / 80.1024 , +236.589 / 112.0),
		vec3(+200.787 / 112.0, -42.6873162 / 80.1024 , +0.000)
	);
	return saturate(instMul(bt709tv_rgb_transform, yuv_input));
}


// Gamma functions ------------------------------------------------

// gamma encoded --> linear:

// sRGB
vec3 toLinear(vec3 _rgb)
{
	bvec3 cutoff = lessThan(_rgb.rgb, vec3_splat(0.04045));
	vec3 higher = pow((_rgb.rgb + vec3_splat(0.055)) / vec3_splat(1.055), vec3_splat(2.4));
	vec3 lower = _rgb.rgb / vec3_splat(12.92);

	return saturate(mix(higher, lower, cutoff));
}

vec3 toLinearSMPTE170M(vec3 _rgb)
{
	bvec3 cutoff = lessThan(_rgb.rgb, vec3_splat(0.0812));
	vec3 higher = pow((_rgb.rgb + vec3_splat(0.099)) / vec3_splat(1.099), (vec3_splat(1.0) / vec3_splat(0.45)));
	vec3 lower = _rgb.rgb / vec3_splat(4.5);

	return saturate(mix(higher, lower, cutoff));
}

vec3 toLinear2pt2(vec3 _rgb)
{
	return saturate(pow(_rgb.rgb, vec3_splat(2.2)));
}

// Microsoft says PAL uses a pure 2.8 gamma curve. See: https://learn.microsoft.com/en-us/windows/win32/api/mfobjects/ne-mfobjects-mfvideotransferfunction
// ffmpeg thinks there *should* be a linear toe slope, but uses a pure curve since they cannot find any documentation for it. See: https://github.com/FFmpeg/FFmpeg/blob/master/libavfilter/vf_colorspace.c#L162
// In any event Poynton says 2.8 is "unrealistically high" and PAL CRT units did not really behave like that
// PAL switched to the SMPTE170M function in 2005 (see BT1700)
vec3 toLinear2pt8(vec3 _rgb)
{
	return saturate(pow(_rgb.rgb, vec3_splat(2.8)));
}


vec3 toLinearToelessSRGB(vec3 _rgb)
{
    vec3 twoPtwo = toLinear2pt2(_rgb);
    vec3 sRGB = toLinear(_rgb);
    bvec3 useSRGB = lessThan(sRGB, twoPtwo);
    vec3 proportion = pow(_rgb / vec3_splat(0.389223), vec3_splat(1.0 / 2.2));
    vec3 merged = mix(twoPtwo, sRGB, proportion);
    return saturate(mix(merged, sRGB, useSRGB));
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


// Gamut conversions ---------------------------------------------
// These should be done using linear RGB input

vec3 convertGamut_NTSCJtoSRGB(vec3 rgb_input)
{
	const mat3 NTSCJ_to_bt709_gamut_transform = mat3(
		vec3(+1.34756301456925, -0.031150036968175, -0.024443490594835),
		vec3(-0.276463760747096, +0.956512223260545, -0.048150182045316),
		vec3(-0.071099263267176, +0.074637860817515, +1.07259361295816)
	);
	return saturate(instMul(NTSCJ_to_bt709_gamut_transform, rgb_input));
}

vec3 convertGamut_SMPTECtoSRGB(vec3 rgb_input)
{
	const mat3 SMPTEC_to_bt709_gamut_transform = mat3(
		vec3(+0.93954641805697, +0.0177731581035936, -0.0016219287899825),
		vec3(+0.0501790284093381, +0.965794379088605, -0.00437041275295984),
		vec3(+0.0102745535336914, +0.016432462807801, +1.00599234154294)
	);
	return saturate(instMul(SMPTEC_to_bt709_gamut_transform, rgb_input));
}

vec3 convertGamut_EBUtoSRGB(vec3 rgb_input)
{
	const mat3 EBU_to_bt709_gamut_transform = mat3(
		vec3(+1.04404109596867, +0.000, -0.000),
		vec3(-0.0440410959686678, +1.0, +0.0117951493631932),
		vec3(+0.000, +0.000, +0.988204850636807)
	);
	return saturate(instMul(EBU_to_bt709_gamut_transform, rgb_input));
}

// See https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Shaders/ColorSpaceUtility.hlsli#L120
vec3 convertGamut_SRGBtoREC2020(vec3 rgb_input)
{
	mat3 toRec2020 = mat3(
		vec3(+0.628252390228217, +0.069018748494509, +0.016358741846493),
		vec3(+0.329243684863216, +0.9191169021082, +0.087837787397663),
		vec3(+0.042503924908568, +0.011864349397292, +0.895803470755843)
	);
	return saturate(instMul(toRec2020, rgb_input));
}

vec3 convertGamut_NTSCJtoREC2020(vec3 rgb_input)
{
	mat3 NTSCJtoRec2020 = mat3(
		vec3(+0.835314787642499, +0.064086581191406, -0.00258827855966),
		vec3(+0.139190018780176, +0.859494098117681, +0.036362217824334),
		vec3(+0.025495200617381, +0.076419362630435, +0.966226011255509)
	);
	return saturate(instMul(NTSCJtoRec2020, rgb_input));
}

vec3 convertGamut_SMPTECtoREC2020(vec3 rgb_input)
{
	mat3 NTSCJtoRec2020 = mat3(
		vec3(+0.596055044600838, +0.081162684813783, +0.015478022749295),
		vec3(+0.349321035033339, +0.891089379419002, +0.081739076199362),
		vec3(+0.054623920365823, +0.027747935767214, +0.902782901051343)
	);
	return saturate(instMul(NTSCJtoRec2020, rgb_input));
}

vec3 convertGamut_EBUtoREC2020(vec3 rgb_input)
{
	mat3 NTSCJtoRec2020 = mat3(
		vec3(+0.655921314038802, +0.072058409820593, +0.017079198766081),
		vec3(+0.302076101195449, +0.916217182555354, +0.097683466215707),
		vec3(+0.042002584765749, +0.011724407624053, +0.885237335018211)
	);
	return saturate(instMul(NTSCJtoRec2020, rgb_input));
}

// Dithering ---------------------------------------------

// Apply Martin Roberts' quasirandom dithering below 8 bits of precision.
// See https://extremelearning.com.au/unreasonable-effectiveness-of-quasirandom-sequences/
// pixelval: float (range 0-1) pixel color trio, assumed to have been read in from a trio of 8-bit values.
// coords: float (range 0-1) pixel coordinates, i.e., v_texcoord0.xy
// ydims: integer dimensions of first channel's texture
// udims & vdims: integer dimensions of first second and third channels' textures (may differ from ydim for various yuv formats)
// scale_divisor: step size divisor to scale the dithering to fit within. E.g., use 255.0 for dithering 8-bit values.
vec3 QuasirandomDither(vec3 pixelval, vec2 coords, ivec2 ydims, ivec2 udims, ivec2 vdims, float scale_divisor)
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
	// scale down below the specified step size
	dither = dither / vec3_splat(scale_divisor);
	// add to input
	return saturate(pixelval + dither);
}
