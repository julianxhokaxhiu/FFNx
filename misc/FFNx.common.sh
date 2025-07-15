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

// Constants for BT1886 Appendix 1 EOTF Function
// These constants correspond to a properly calibrated mid-90s Sony Trinitron CRT
// Do not change them blindly. If you change black or white level, then B, K, and S need to be recalculated.
// (https://github.com/ChthonVII/gamutthingy can calculate them for you.)
#define crtBlackLevel 0.002
#define crtWhiteLevel 1.71
#define crtConstantB 0.1032367172184046
#define crtConstantK 1.324516800483618
#define crtConstantS 1.372366050214779

// Gamut LUT
SAMPLER2D(tex_10, 10);

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
// In any event, Poynton says 2.8 is "unrealistically high" and PAL CRT units did not really behave like that.
// PAL switched to the SMPTE170M function in 2005 (see BT1700)
vec3 toLinear2pt8(vec3 _rgb)
{
	return saturate(pow(_rgb.rgb, vec3_splat(2.8)));
}

// EOTF Function from BT1886 Appendix 1 (https://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.1886-0-201103-I!!PDF-E.pdf)
// Approximates the gamma behavior of a CRT (more accurately than the crummy Annex 1 function)
// Constants have been selected to match a properly calibrated mid-90s Sony Trinitron CRT.
vec3 toLinearBT1886Appx1Fast(vec3 _rgb)
{
	// add B
	_rgb = _rgb + vec3_splat(crtConstantB);
	// EOTF
	bvec3 cutoff = lessThan(_rgb.rgb, vec3_splat(0.35 + crtConstantB));
	vec3 higher = pow(_rgb, vec3_splat(2.6)) * vec3_splat(crtConstantK);
	vec3 lower = pow(_rgb, vec3_splat(3.0)) * vec3_splat(crtConstantK) * vec3_splat(crtConstantS);
	vec3 outcolor = mix(higher, lower, cutoff);
	// renormalize
	outcolor = outcolor - vec3_splat(crtBlackLevel);
	outcolor = outcolor / vec3_splat(crtWhiteLevel - crtBlackLevel);
	return saturate(outcolor);
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


// Gamut conversions ---------------------------------------------
// These functions all take a linear RGB input and produce a linear RGB output.
// Mathematically, they are equivalent to:
//   1. Convert linear RGB to XYZ using the source gamut's red/green/blue points
//   2. Do a gamut conversion from the source gamut to the destination gamut
//   3. Covert XYZ to linear RGB using the destination gamut's red/green/blue points
// But all of that has been pre-computed into a single matrix multiply operation.

// Note: sRGB is the same gamut as rec709 video.

// Note: High precision values are used for the "D65" whitepoint. (x=0.312713, y=0.329016)

// Note: There are (at least) three different whitepoints that are all referred to as "D93"/"9300K."
// The one used here is 9300K+27mpcd (x=0.281, y=0.311), which is what NTSC-J television sets used.

// Most of the gamut conversion matrices have been replacved with LUTs.
// We will want to bring them back for HDR *if* we can find a way to left potentially out-of-bounds values linger until post processing.

// To rec2020:
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


// This is a generic 3D LUT function.
// We're using it to do gamut conversions when a gamut compression mapping algorithm is necessary to avoid losing detail to clipping.
// Since that's waaaay too compute heavy, we precompute it, then use a LUT.
// Renderer::AssignGamutLUT() in renderer.cpp is in charge of making sure the correct LUT is bound.
// Expects:
// - coords 0,0 in the upper left corner
// - 4096x64 dimensions
// - linear rgb (unlike most other textures BGFX is NOT doing a linearize for us; we expect the image is linear to start with)
// - black in the upper left corner
// - green on the vertical axis
// - red on the small horizontal axis
// - blue on the large horizontal axis

vec3 GamutLUT(vec3 rgb_input)
{
	vec3 temp = saturate(rgb_input) * vec3_splat(63.0);
	vec3 floors = floor(temp);
	vec3 ceils = ceil(temp);
	vec3 ceilweights = saturate(temp - floors);

	// driver might not correctly sample a 1.0 coordinate
	// so we are going to add a just-under-half-step offset to red and green, then increase their divisors by 1
	// This should get us a slightly lower coordinate within the same pixel
	floors = floors + vec3(0.4999, 0.4999, 0.0);
	ceils = ceils + vec3(0.4999, 0.4999, 0.0);
	floors = floors / vec3(4096.0, 64.0, 64.0);
	ceils = ceils / vec3(4096.0, 64.0, 64.0);

	vec3 RfGfBf = (texture2D(tex_10, vec2(floors.b + floors.r, floors.g))).xyz;
	vec3 RfGfBc = (texture2D(tex_10, vec2(ceils.b + floors.r, floors.g))).xyz;
	vec3 RfGcBf = (texture2D(tex_10, vec2(floors.b + floors.r, ceils.g))).xyz;
	vec3 RfGcBc = (texture2D(tex_10, vec2(ceils.b + floors.r, ceils.g))).xyz;
	vec3 RcGfBf = (texture2D(tex_10, vec2(floors.b + ceils.r, floors.g))).xyz;
	vec3 RcGfBc = (texture2D(tex_10, vec2(ceils.b + ceils.r, floors.g))).xyz;
	vec3 RcGcBf = (texture2D(tex_10, vec2(floors.b + ceils.r, ceils.g))).xyz;
	vec3 RcGcBc = (texture2D(tex_10, vec2(ceils.b + ceils.r, ceils.g))).xyz;

	vec3 RfGf = mix(RfGfBf, RfGfBc, vec3_splat(ceilweights.b));
	vec3 RfGc = mix(RfGcBf, RfGcBc, vec3_splat(ceilweights.b));
	vec3 RcGf = mix(RcGfBf, RcGfBc, vec3_splat(ceilweights.b));
	vec3 RcGc = mix(RcGcBf, RcGcBc, vec3_splat(ceilweights.b));

	vec3 Rf = mix(RfGf, RfGc, vec3_splat(ceilweights.g));
	vec3 Rc = mix(RcGf, RcGc, vec3_splat(ceilweights.g));

	vec3 outcolor = mix(Rf, Rc, vec3_splat(ceilweights.r));

	return outcolor;
}

// Dithering ---------------------------------------------

// Apply Martin Roberts' quasirandom dithering scaled below a specified level of precision.
// See https://extremelearning.com.au/unreasonable-effectiveness-of-quasirandom-sequences/
// pixelval: float (range 0-1) pixel color trio.
// coords: float (range 0-1) pixel coordinates, i.e., v_texcoord0.xy
// ydims: integer dimensions of first channel's texture
// udims & vdims: integer dimensions of first second and third channels' textures (may differ from ydim for various yuv formats)
// scale_divisor: step size divisor to scale the dithering to fit within. E.g., use 255.0 for dithering 8-bit values.
// xyoffset: value to add to x & y coords. Should be at least 1 to avoid x=0 and y=0. Should be different if the same input is dithered twice.
// (This function will be used twice if TV-range video is dithered for range expansion, then again for HDR bit depth increase.)
vec3 QuasirandomDither(vec3 pixelval, vec2 coords, ivec2 ydims, ivec2 udims, ivec2 vdims, float scale_divisor, float xyoffset)
{
	// get integer range x,y coords for this pixel
	// invert one axis for u and the other axis for v to decouple dither patterns across channels
	// see https://blog.kaetemi.be/2015/04/01/practical-bayer-dithering/
	// add 1 to avoid x=0 and y=0
	vec3 xpos = vec3(
		round(float(ydims.x) * coords.x)  + xyoffset,
		round(float(udims.x) * (1.0 - coords.x)) + xyoffset,
		round(float(vdims.x) * coords.x)  + xyoffset
	);
	vec3 ypos = vec3(
		round(float(ydims.y) * coords.y) + xyoffset,
		round(float(udims.y) * coords.y) + xyoffset,
		round(float(vdims.y) * (1.0 - coords.y)) + xyoffset
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
	vec3 tempout = saturate(pixelval + dither);

	// don't dither colors so close to 0 or 1 that dithering is asymmetric
	bvec3 highcutoff = greaterThan(pixelval, vec3_splat(1.0 - (0.5 / scale_divisor)));
	bvec3 lowcutoff = lessThan(pixelval, vec3_splat(0.5 / scale_divisor));
	vec3 outcolor = mix(tempout, pixelval, highcutoff);
	outcolor = mix(outcolor, pixelval, lowcutoff);
	return outcolor;
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
