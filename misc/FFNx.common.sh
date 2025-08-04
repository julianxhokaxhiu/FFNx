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
// These constants correspond to a mid-90s Sony Trinitron CRT with the brightness turned pretty far up and the contrast turned pretty far down.
// Do not change them blindly. If you change black or white level, then B, K, S, and I need to be recalculated.
// And LUTs will need to be recalculated.
// (https://github.com/ChthonVII/gamutthingy can calculate them for you.)
#define crtBlackLevel 0.0018
#define crtWhiteLevel 1.5
#define crtConstantB 0.1042361441620798
#define crtConstantK 1.1591247176412838
#define crtConstantS 1.3711574400867077
#define crtConstantI 0.1489575626482313

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
// They had the same gamma function as every other CRT (reasonably well approximated by BT1886 Appendix 1), because the underlying physics so dictated.
// Notwithstanding the behavior of the CRTs, digital video encoded for PAL/EBU might have followe dthe 2.8 gamma in the spec.
// PAL switched to the SMPTE170M function in 2005 (see BT1700)
vec3 toLinear2pt8(vec3 _rgb)
{
	return saturate(pow(_rgb.rgb, vec3_splat(2.8)));
}

// EOTF Function from BT1886 Appendix 1 (https://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.1886-0-201103-I!!PDF-E.pdf)
// Approximates the gamma behavior of a CRT (more accurately than the crummy Annex 1 function)
// Constants have been selected to match a mid-90s Sony Trinitron CRT with the brightness turned pretty far up and the contrast turned pretty far down.
// Assumes input in range 0-1, output in range 0-1.
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

// Inverse EOTF Function from BT1886 Appendix 1 (https://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.1886-0-201103-I!!PDF-E.pdf)
// Approximates the inverse of gamma behavior of a CRT (more accurately than the crummy Annex 1 function)
// Constants have been selected to match a mid-90s Sony Trinitron CRT with the brightness turned pretty far up.
vec3 toGammaBT1886Appx1Fast(vec3 _rgb)
{
	// undo the chop and normalization post-processing
	_rgb = _rgb * vec3_splat(crtWhiteLevel - crtBlackLevel);
	_rgb = _rgb + vec3_splat(crtBlackLevel);

	// Inverse EOTF
	bvec3 cutoff = lessThan(_rgb.rgb, vec3_splat(crtConstantI));
	vec3 higher = pow(vec3_splat(1.0/crtConstantK) * _rgb, vec3_splat(1.0/2.6));
	vec3 lower = pow(vec3_splat(1.0/crtConstantK) * vec3_splat(1.0/crtConstantS) * _rgb, vec3_splat(1.0/3.0));
	vec3 outcolor = mix(higher, lower, cutoff);

	//unshift
	outcolor = outcolor - vec3_splat(crtConstantB);

	return saturate(outcolor);
}

// Gamut conversions ---------------------------------------------
// For final conversion in NTSC-J mode + NTSC-J movie in uncorrected mode:
// 		Start from gamma-space R'G'B'
// 		Simulate color correction circuit
// 		Linearize with BT1886 Appendix 1 EOTF function
// 		Chromatic adaption to convert whitepoint to D65
// 		Gamut conversion to sRGB (or REC2020)
// 		Gamut compression to make result fit inside sRGB gamut (not needed for REC2020)
// 		Output linear RGB in sRGB gamut
// For 709/SMPTEC/EBU in NTSC-J mode:
// 		Do the inverse of the above so that the final conversion will round-trip
// 		E.g., linear 709(=sRBG) to gamma-space NTSC-J
// For SMPTEC/EBU movie in uncorrected mode:
// 		Do linear-to-linear gamut conversion + compression.

// Note: sRGB is the same gamut as rec709 video.

// Note: High precision values are used for the "D65" whitepoint. (x=0.312713, y=0.329016)

// To rec2020:
// See https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Shaders/ColorSpaceUtility.hlsli#L120
vec3 convertGamut_SRGBtoREC2020(vec3 rgb_input)
{
	mat3 toRec2020 = mat3(
		vec3(+0.6274191906, +0.0691004930, +0.0163944170),
		vec3(+0.3292752691, +0.9195389158, +0.0880250760),
		vec3(+0.0433055403, +0.0113605912, +0.8955805070)
	);
	return saturate(instMul(toRec2020, rgb_input));
}

// NTSC-J (actually P22) to REC2020 (linear RGB input, linear RGB output)
// Converts P22 primaries (gamutthingy's "P22_trinitron_mixandmatch" with 9300K+8MPCD whitepoint to REC2020 primaries with D65 whitepoint)
// (flip matrix b/c shader languages are column-major)
vec3 convertGamut_NTSCJtoREC2020(vec3 rgb_input)
{
	mat3 NTSCJtoRec2020 = mat3(
		vec3(+0.6117882897, +0.0797156500, +0.0072571191),
		vec3(+0.3142775896, +0.8673198799, +0.0632222078),
		vec3(+0.0739341207, +0.0529644702, +0.9295206731)
	);
	// clamp low in case of floating point errors; don't clamp high
	// out-of-bounds red will get scaled down to something sane inside ApplyREC2084Curve()
	// end result should be a pure red that's stronger than the red component of white (but still easily in bounds for rec2020)
	return max(instMul(NTSCJtoRec2020, rgb_input), vec3_splat(0.0));
}

// Simulate CRT behavior (gamma-space R'G'B' input, linear RGB output)
// Some outputs **WILL** be > 1.0! Make sure subsequent code can handle that.
vec3 CRTSimulation(vec3 rgb_input){
	// Apply the color correction matrix
	// Matrices can be derived from the chip datasheets through the inverse of the method explained in
	// Parker, Norman W. "An Analysis of the Necessary Decoder Corrections for Color Receiver Operation with Non-Standard Receiver Primaries." IEEE Transactions on Consumer Electronics, Vol. CE-28, No. 1, pp. 74-83. February 1982. (Reprint of 1966 original.)
	// This matrix corresponds to the CXA2060BS in JP mode.
	// With the saturation knob turned up a bit, also rolled into the matrix.
	// (flip matrix b/c shader languages are column-major)
	mat3 crtMatrix = mat3(
		vec3(+1.329241997338724, +0.002013691127971472, -0.004560000000000004),
		vec3(-0.2171413781028256, +0.9972707680691105, -0.02348000000000002),
		vec3(-0.1128712957634457, +0.0007533473126427508, +1.03544)
	);
	vec3 corrected = instMul(crtMatrix, rgb_input);

	// It does not appear that CRTs low clamped the output from color correction.
	// But it was implicitly clamped by the fact electron guns don't run in reverse.

	// Whether CRTs ever high clamped the output from color correction (and, if so, how high) is a great mystery.
	// The scant evidence available suggests no clamping at all.
	// But that could be wrong, or it could vary by model.

	// linearize with a slightly modified version of BT1886 that accepts out-of-bounds input
	// (which we need due to not clamping the color correction circuit)
	// add B
	corrected = corrected + vec3_splat(crtConstantB);
	// clamp low at zero light emission (electron gun doesn't go in reverse)
	corrected = max(corrected, vec3_splat(0.0));
	// EOTF
	bvec3 cutoff = lessThan(corrected.rgb, vec3_splat(0.35 + crtConstantB));
	vec3 higher = pow(corrected, vec3_splat(2.6)) * vec3_splat(crtConstantK);
	vec3 lower = pow(corrected, vec3_splat(3.0)) * vec3_splat(crtConstantK) * vec3_splat(crtConstantS);
	vec3 outcolor = mix(higher, lower, cutoff);
	// renormalize
	outcolor = outcolor - vec3_splat(crtBlackLevel);
	outcolor = outcolor / vec3_splat(crtWhiteLevel - crtBlackLevel);
	// clamp low in case of floating point errors; don't clamp high
	return max(outcolor, vec3_splat(0.0));
}

// This is a generic 3D LUT function.
// We're using it to do gamut conversions when a gamut compression mapping algorithm is necessary to avoid losing detail to clipping.
// Since that's waaaay too compute heavy, we precompute it, then use a LUT.
// Renderer::AssignGamutLUT() in renderer.cpp is in charge of making sure the correct LUT is bound.
// Expects:
// - coords 0,0 in the upper left corner
// - 4096x64 dimensions (FYI: BGFX cannot handle 16384x128)
// - stores values as linear RGB, or sRGB gamma-space, or CRT gamma-space as indicated by srgblut and crtlut parameters.
// 		(do not let BGFX linearize this texture when loading it)
// - black in the upper left corner
// - green on the vertical axis
// - red on the small horizontal axis
// - blue on the large horizontal axis
// - input is linear or CRT gamma-space, as indicated by crttypeinput parameter
// output is always linearized (regardless of how it was stored)

vec3 GamutLUT(vec3 rgb_input, bool crttypeinput, bool srgblut, bool crtlut)
{
	vec3 temp = saturate(rgb_input) * vec3_splat(63.0);
	vec3 floors = floor(temp);
	vec3 ceils = ceil(temp);
	vec3 ceilweights = saturate(temp - floors);

	// If the input is in CRT gamma space, we need to get it, and the LUT indices, into linear RGB to compute interpolation weights.
	// This two-corner method is slightly wrong because the gamma-space indices don't form a perfect cube in linear space.
	// But it should be close enough that the error is smaller than quantization error.
	if (crttypeinput){
		vec3 temp_linear = CRTSimulation(rgb_input);
		vec3 floors_linear = CRTSimulation(floors/vec3_splat(63.0));
		vec3 ceils_linear = CRTSimulation(ceils/vec3_splat(63.0));
		ceilweights = saturate((temp_linear - floors_linear) / (ceils_linear - floors_linear));
	}

	// driver might not correctly sample a 1.0 coordinate (and/or might not honor clamp-to-edge)
	// so we are going to add a just-under-half-step offset to red and green, then increase their divisors by 1
	// This should get us a slightly lower coordinate within the same pixel
	floors = floors + vec3(0.4999, 0.4999, 0.0);
	ceils = ceils + vec3(0.4999, 0.4999, 0.0);
	floors = floors / vec3(4096.0, 64.0, 64.0);
	ceils = ceils / vec3(4096.0, 64.0, 64.0);

	// take 8 samples
	vec3 RfGfBf = (texture2D(tex_10, vec2(floors.b + floors.r, floors.g))).xyz;
	vec3 RfGfBc = (texture2D(tex_10, vec2(ceils.b + floors.r, floors.g))).xyz;
	vec3 RfGcBf = (texture2D(tex_10, vec2(floors.b + floors.r, ceils.g))).xyz;
	vec3 RfGcBc = (texture2D(tex_10, vec2(ceils.b + floors.r, ceils.g))).xyz;
	vec3 RcGfBf = (texture2D(tex_10, vec2(floors.b + ceils.r, floors.g))).xyz;
	vec3 RcGfBc = (texture2D(tex_10, vec2(ceils.b + ceils.r, floors.g))).xyz;
	vec3 RcGcBf = (texture2D(tex_10, vec2(floors.b + ceils.r, ceils.g))).xyz;
	vec3 RcGcBc = (texture2D(tex_10, vec2(ceils.b + ceils.r, ceils.g))).xyz;

	// if the LUT is stored with sRGB gamma, need to linearize for interpolation
	if (srgblut){
		RfGfBf = toLinear(RfGfBf);
		RfGfBc = toLinear(RfGfBc);
		RfGcBf = toLinear(RfGcBf);
		RfGcBc = toLinear(RfGcBc);
		RcGfBf = toLinear(RcGfBf);
		RcGfBc = toLinear(RcGfBc);
		RcGcBf = toLinear(RcGcBf);
		RcGcBc = toLinear(RcGcBc);
	}
	// or if the LUT stores CRT gamma-space values, linearize those
	// (this is slightly wrong b/c we're skipping color correction simulation,
	// but we must skip it since it's not cleanly invertible (which is something we need));
	// and the error should be smaller than the quantization error anyway,
	// and we're only doing this in one narrow case: non-NTSC-J movies in NTSC-J mode)
	else if (crtlut){
		RfGfBf = toLinearBT1886Appx1Fast(RfGfBf);
		RfGfBc = toLinearBT1886Appx1Fast(RfGfBc);
		RfGcBf = toLinearBT1886Appx1Fast(RfGcBf);
		RfGcBc = toLinearBT1886Appx1Fast(RfGcBc);
		RcGfBf = toLinearBT1886Appx1Fast(RcGfBf);
		RcGfBc = toLinearBT1886Appx1Fast(RcGfBc);
		RcGcBf = toLinearBT1886Appx1Fast(RcGcBf);
		RcGcBc = toLinearBT1886Appx1Fast(RcGcBc);
	}

	// merge down to 4 samples along blue axis
	vec3 RfGf = mix(RfGfBf, RfGfBc, vec3_splat(ceilweights.b));
	vec3 RfGc = mix(RfGcBf, RfGcBc, vec3_splat(ceilweights.b));
	vec3 RcGf = mix(RcGfBf, RcGfBc, vec3_splat(ceilweights.b));
	vec3 RcGc = mix(RcGcBf, RcGcBc, vec3_splat(ceilweights.b));

	// merge down to 2 samples along green axis
	vec3 Rf = mix(RfGf, RfGc, vec3_splat(ceilweights.g));
	vec3 Rc = mix(RcGf, RcGc, vec3_splat(ceilweights.g));

	// merge down to one color along red axis
	vec3 outcolor = mix(Rf, Rc, vec3_splat(ceilweights.r));

	return saturate(outcolor);
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
	vec3 tempout = pixelval + dither; // Don't clamp. NTSC-J color correction simulation produces values >1.0 in HDR mode. Have faith that input was clamped in other cases.

	// Don't dither colors so close to 0 or 1 that dithering would be asymmetric.
	// But do dither values >1.0 in the special case that NTSC-J color correction simulation produces them in HDR mode.
	bvec3 highcutoff = greaterThan(pixelval, vec3_splat(1.0 - (0.5 / scale_divisor)));
	bvec3 superhighcutoff = greaterThan(pixelval, vec3_splat(1.0 + (0.5 / scale_divisor)));
	bvec3 lowcutoff = lessThan(pixelval, vec3_splat(0.5 / scale_divisor));
	vec3 outcolor = mix(tempout, pixelval, highcutoff);
	outcolor = mix(outcolor, tempout, superhighcutoff);
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
