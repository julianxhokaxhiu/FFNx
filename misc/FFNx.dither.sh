/****************************************************************************/
//    Copyright (C) 2026 ChthonVII                                             //
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


// Dithering ---------------------------------------------

// This function is used for
//  - bit depth increase for tv->pc range expansion for movies
//  - bit depth increase for sRGB->HDR
//  - LUT operations

// Apply Martin Roberts' quasirandom dithering scaled below a specified level of precision.
// See https://extremelearning.com.au/unreasonable-effectiveness-of-quasirandom-sequences/
// pixelval: float (range 0-1) pixel color trio.
// coords: float (range 0-1) pixel coordinates, i.e., v_texcoord0.xy
// ydims: integer dimensions of first channel's texture
// udims & vdims: integer dimensions of first second and third channels' textures (may differ from ydim for various yuv formats)
// scale_divisors: step size divisors to scale the dithering to fit within. E.g., use 256.0 for dithering 8-bit values. One for each channel.
// xyoffset: value to add to x & y coords. Should be at least 1 to avoid x=0 and y=0. Should be different if the same input is dithered twice.
vec3 QuasirandomDither(vec3 pixelval, vec2 coords, ivec2 ydims, ivec2 udims, ivec2 vdims, vec3 scale_divisors, float xyoffset)
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
	dither = dither / scale_divisors;
	// add to input
	vec3 tempout = saturate(pixelval + dither);

	// Don't dither colors so close to 0 or 1 that dithering would be asymmetric.
	// But do dither values >1.0 in the special case that NTSC-J color correction simulation produces them in HDR mode.
  vec3 halfsteps = vec3_splat(0.5) / scale_divisors;
	bvec3 highcutoff = greaterThan(pixelval, vec3_splat(1.0) - halfsteps);
	bvec3 lowcutoff = lessThan(pixelval, halfsteps);
	vec3 outcolor = mix(tempout, pixelval, highcutoff);
	outcolor = mix(outcolor, pixelval, lowcutoff);
	return outcolor;
}
