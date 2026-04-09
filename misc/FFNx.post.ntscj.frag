/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

$input v_color0, v_texcoord0

#include <bgfx/bgfx_shader.sh>
#include "FFNx.colorfunctions.sh" //includes FFNx.common.sh
#include "FFNx.dither.sh"

SAMPLER2D(tex_0, 0);

uniform vec4 FSHDRFlags;

#define isHDR FSHDRFlags.x > 0.0
#define monitorNits FSHDRFlags.y

void main()
{
	vec4 color = texture2D(tex_0, v_texcoord0.xy);

	if (isHDR) {
		// Dither because we will increase effective bit depth.
		// 256 is correct step size for everything except movies, which already have higher precision.
		// But unfortunately there's no way at this point to segregate movies, from other stuff, from other stuff drawn on top of movies.
		// So we just have to use 256 for everything.
		// Dither in gamma space so dither step size is proportional to quantization step size.
		// Can't dither in rec2084 gamma space because our max signal only occupies a small space near the bottom of the range.
		// It's not ideal to dither ahead of CRT color correction simulation, but doing it after is gnarly due to out-of-bounds red.
		ivec2 dimensions = textureSize(tex_0, 0);
		color.rgb = QuasirandomDither(color.rgb, v_texcoord0.xy, dimensions, dimensions, dimensions, vec3_splat(256.0), 2160.0);
		// simulate CRT color correction and gamma
		color.rgb = CRTSimulation(color.rgb); // CRT gamma-space in, linear out
		color.rgb = convertGamut_NTSCJtoREC2020(color.rgb);
		color.rgb = ApplyREC2084Curve(color.rgb, monitorNits);
	}
	else {
		color.rgb = GamutLUT(color.rgb); // AssignGamutLUT() in renderer.cpp should have bound the correct LUT
		color.rgb = toGamma(color.rgb);
    // dither with step size 256 because the LUT only has 8 bits of precision
		ivec2 dimensions = textureSize(tex_0, 0);
		color.rgb = QuasirandomDither(color.rgb, v_texcoord0.xy, dimensions, dimensions, dimensions, vec3_splat(256.0), 2160.0);
	}
	gl_FragColor = color;
}
