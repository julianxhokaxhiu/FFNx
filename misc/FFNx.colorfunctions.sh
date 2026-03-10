/****************************************************************************/
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

#include "FFNx.common.sh" // need sRGB gamma functions

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

// Inverse EOTF Function from BT1886 Appendix 1 (https://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.1886-0-201103-I!!PDF-E.pdf)
// Approximates the inverse of gamma behavior of a CRT (more accurately than the crummy Annex 1 function)
// Constants have been selected to match a mid-90s Sony Trinitron CRT with the brightness turned pretty far up.
// Not used - retained for posterity, possible use in future
/*
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
*/

// Gamut conversions ---------------------------------------------
// For NTSC-J:
// 		Start from gamma-space R'G'B'
// 		Simulate color correction circuit
// 		Linearize with BT1886 Appendix 1 EOTF function
// 		Chromatic adaption to convert whitepoint to D65
// 		Gamut conversion to sRGB
// 		Gamut compression to make result fit inside sRGB gamut
// 		Output linear RGB in sRGB gamut
// For movies in other gamuts:
// 		Linearize with appropriate gamma function
// 		Reverse LUT from linear back to gamma-space R'G'B'

// Note: sRGB is the same gamut as rec709 video.

// Note: High precision values are used for the "D65" whitepoint. (x=0.312713, y=0.329016)

// NTSC-J (actually P22) to REC2020 (linear RGB input, linear RGB output)
// Converts P22 primaries (gamutthingy's "P22_trinitron_mixandmatch" with 9300K+8MPCD whitepoint to REC2020 primaries with D65 whitepoint)
// (flip matrix b/c shader languages are column-major)
vec3 convertGamut_NTSCJtoREC2020(vec3 rgb_input)
{
	mat3 NTSCJtoRec2020 = mtxFromCols(
		vec3(+0.6117882897, +0.0797156500, +0.0072571191),
		vec3(+0.3142775896, +0.8673198799, +0.0632222078),
		vec3(+0.0739341207, +0.0529644702, +0.9295206731)
	);
	// clamp low in case of floating point errors; don't clamp high
	// out-of-bounds red will get scaled down to something sane inside ApplyREC2084Curve()
	// end result should be a pure red that's stronger than the red component of white (but still easily in bounds for rec2020)
	return max(mul(NTSCJtoRec2020, rgb_input), vec3_splat(0.0));
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
	mat3 crtMatrix = mtxFromCols(
		vec3(+1.3302672030, +0.0020136150, -0.0045265039),
		vec3(-0.2173088529, +0.9972330662, -0.0233075243),
		vec3(-0.1129583501, +0.0007533188, +1.0278340282)
	);
	vec3 corrected = mul(crtMatrix, rgb_input);

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

// Inverse of CRTSimulation()
// Accepts inputs >1.0; output is clamped 0-1.
vec3 InverseCRTSimulation(vec3 rgb_input){

	///// start by inverting gamma function

	// undo the chop and normalization post-processing
	rgb_input = rgb_input * vec3_splat(crtWhiteLevel - crtBlackLevel);
	rgb_input = rgb_input + vec3_splat(crtBlackLevel);

	// clamp any value that's still negative
	rgb_input = max(rgb_input, vec3_splat(0.0));

	// Inverse EOTF
	bvec3 cutoff = lessThan(rgb_input.rgb, vec3_splat(crtConstantI));
	vec3 higher = pow(vec3_splat(1.0/crtConstantK) * rgb_input, vec3_splat(1.0/2.6));
	vec3 lower = pow(vec3_splat(1.0/crtConstantK) * vec3_splat(1.0/crtConstantS) * rgb_input, vec3_splat(1.0/3.0));
	vec3 outcolor = mix(higher, lower, cutoff);

	//unshift
	outcolor = outcolor - vec3_splat(crtConstantB);

	//// apply inverse of color correction matrix
	mat3 inverseCRTMatrix = mtxFromCols(
		vec3(+0.7517584969, -0.0015204271, +0.0032762102),
		vec3(+0.1657451679, +1.0024222152, +0.0234612064),
		vec3(+0.0824963352, -0.0009017881, +0.9732625834)
	);
	outcolor = mul(inverseCRTMatrix, outcolor);

	//// clamp
	// strictly clamp to 0-1 because this corresponds to the console's R'G'B' output, which is defined by integer math
	// also, clamping here is a substitute for clamping linear RGB operations we couldn't clamp earlier due to CRTSimulation()'s outputs >1.0.
	return saturate(outcolor);
}

// Apply the inverse of the CRT color correction matrix. (gamma-space R'G'B' input and output)
// When used together with NTSC-J mode, the overall result is what you'd get from a CRT television
// with P22 phosphors, 9300K+8MPCD whitepoint, and no color correction circuit --
// which basically describes a mid-90s CRT computer monitor.
vec3 CRTUncorrect(vec3 rgb_input){
	//// apply inverse of color correction matrix
	mat3 inverseCRTMatrix = mtxFromCols(
		vec3(+0.7517584969, -0.0015204271, +0.0032762102),
		vec3(+0.1657451679, +1.0024222152, +0.0234612064),
		vec3(+0.0824963352, -0.0009017881, +0.9732625834)
	);
	vec3 outcolor = mul(inverseCRTMatrix, rgb_input);
	// clamp
	return saturate(outcolor);
}

// This is a generic 3D LUT function.
// We're using it to do gamut conversions when a gamut compression mapping algorithm is necessary to avoid losing detail to clipping.
// Since that's waaaay too compute heavy, we precompute it, then use a LUT.
// Renderer::AssignGamutLUT() in renderer.cpp is in charge of making sure the correct LUT is bound.
// Expects:
// - coords 0,0 in the upper left corner
// - 4096x64 dimensions (FYI: BGFX cannot handle 16384x128)
// - Stores values as gamma-space sRGB. (They will be linearized.)
// - black in the upper left corner
// - green on the vertical axis
// - red on the small horizontal axis
// - blue on the large horizontal axis
// - input is CRT gamma-space R'G'B' (before the CRT's color correction circuit)
// - output is always linearized
vec3 GamutLUT(vec3 rgb_input)
{
	vec3 temp = saturate(rgb_input) * vec3_splat(63.0); // use 63 b/c LUT is created with left-of-bin DAC to make interpolation here easier
	vec3 floors = floor(temp);
	vec3 ceils = ceil(temp);
	// If the input is in CRT gamma space, we need to get it, and the LUT indices, into linear RGB to compute interpolation weights.
	// This two-corner method is slightly wrong because the gamma-space indices don't form a perfect cube in linear space.
	// But it should be close enough that the error is smaller than quantization error.
	vec3 temp_linear = CRTSimulation(rgb_input);
	vec3 floors_linear = CRTSimulation(floors/vec3_splat(63.0));
	vec3 ceils_linear = CRTSimulation(ceils/vec3_splat(63.0));
	vec3 ceilweights = saturate((temp_linear - floors_linear) / max((ceils_linear - floors_linear), vec3_splat(0.0000000001))); // avoid div0

	// multiply blue by 64, as it is the horizontal "major" axis
	floors = floors * vec3(1.0, 1.0, 64.0);
	ceils = ceils * vec3(1.0, 1.0, 64.0);

	// take 8 samples
	// LUT is always stored in gamma-space sRGB, so linearize them
	vec3 RfGfBf = toLinear(texelFetch(tex_10, ivec2(floors.b + floors.r, floors.g), 0).xyz);
	vec3 RfGfBc = toLinear(texelFetch(tex_10, ivec2(ceils.b + floors.r, floors.g), 0).xyz);
	vec3 RfGcBf = toLinear(texelFetch(tex_10, ivec2(floors.b + floors.r, ceils.g), 0).xyz);
	vec3 RfGcBc = toLinear(texelFetch(tex_10, ivec2(ceils.b + floors.r, ceils.g), 0).xyz);
	vec3 RcGfBf = toLinear(texelFetch(tex_10, ivec2(floors.b + ceils.r, floors.g), 0).xyz);
	vec3 RcGfBc = toLinear(texelFetch(tex_10, ivec2(ceils.b + ceils.r, floors.g), 0).xyz);
	vec3 RcGcBf = toLinear(texelFetch(tex_10, ivec2(floors.b + ceils.r, ceils.g), 0).xyz);
	vec3 RcGcBc = toLinear(texelFetch(tex_10, ivec2(ceils.b + ceils.r, ceils.g), 0).xyz);

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

// This is the same as GamutLUT() except that:
//   - The input is linear RGB
//   - The output is CRT gamma-space R'G'B' (before the CRT's color correction circuit)
//   - The stored values are CRT gamma-space R'G'B' (before the CRT's color correction circuit)
vec3 GamutLUTBackwards(vec3 rgb_input)
{
	vec3 temp = saturate(rgb_input) * vec3_splat(63.0); // use 63 b/c LUT is created with left-of-bin DAC to make interpolation here easier
	vec3 floors = floor(temp);
	vec3 ceils = ceil(temp);
	vec3 ceilweights = saturate(temp - floors);

	// multiply blue by 64, as it is the horizontal "major" axis
	floors = floors * vec3(1.0, 1.0, 64.0);
	ceils = ceils * vec3(1.0, 1.0, 64.0);

	// take 8 samples
	// LUT is always stored as CRT gamma-space R'G'B', so run CRT simulation to linearize them
	vec3 RfGfBf = CRTSimulation(texelFetch(tex_10, ivec2(floors.b + floors.r, floors.g), 0).xyz);
	vec3 RfGfBc = CRTSimulation(texelFetch(tex_10, ivec2(ceils.b + floors.r, floors.g), 0).xyz);
	vec3 RfGcBf = CRTSimulation(texelFetch(tex_10, ivec2(floors.b + floors.r, ceils.g), 0).xyz);
	vec3 RfGcBc = CRTSimulation(texelFetch(tex_10, ivec2(ceils.b + floors.r, ceils.g), 0).xyz);
	vec3 RcGfBf = CRTSimulation(texelFetch(tex_10, ivec2(floors.b + ceils.r, floors.g), 0).xyz);
	vec3 RcGfBc = CRTSimulation(texelFetch(tex_10, ivec2(ceils.b + ceils.r, floors.g), 0).xyz);
	vec3 RcGcBf = CRTSimulation(texelFetch(tex_10, ivec2(floors.b + ceils.r, ceils.g), 0).xyz);
	vec3 RcGcBc = CRTSimulation(texelFetch(tex_10, ivec2(ceils.b + ceils.r, ceils.g), 0).xyz);

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

	return InverseCRTSimulation(outcolor);
}
