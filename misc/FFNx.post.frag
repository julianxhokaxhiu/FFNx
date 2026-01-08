/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
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
#include "FFNx.common.sh"

SAMPLER2D(tex_0, 0);

uniform vec4 FSHDRFlags;
uniform vec4 FSMovieFlags;

#define isHDR FSHDRFlags.x > 0.0
#define monitorNits FSHDRFlags.y

#define isOverallSRGBColorGamut abs(FSMovieFlags.w - 0.0) < 0.00001
#define isOverallNTSCJColorGamut abs(FSMovieFlags.w - 1.0) < 0.00001

void main()
{
	vec4 color = texture2D(tex_0, v_texcoord0.xy);

	if (isHDR) {
		// back to linear for gamut conversion and PQ gamma curve
		if (isOverallNTSCJColorGamut){
			color.rgb = CRTSimulation(color.rgb); // CRT gamma-space in, linear out
			color.rgb = convertGamut_NTSCJtoREC2020(color.rgb);
		}
		else {
			color.rgb = toLinear(color.rgb);
			color.rgb = convertGamut_SRGBtoREC2020(color.rgb);
		}
		color.rgb = ApplyREC2084Curve(color.rgb, monitorNits);
		// dither because we increased bit depth
		// dither in gamma space so dither step size is proportional to quantization step size
		// TODO: If/when a full 10-bit pathway is available for 10-bit FMVs, don't dither those
		ivec2 dimensions = textureSize(tex_0, 0);
		color.rgb = QuasirandomDither(color.rgb, v_texcoord0.xy, dimensions, dimensions, dimensions, 256.0, 2160.0);
	}
	else if (isOverallNTSCJColorGamut){
		color.rgb = GamutLUT(color.rgb, true, false);
		color.rgb = toGamma(color.rgb);
		ivec2 dimensions = textureSize(tex_0, 0);
		color.rgb = QuasirandomDither(color.rgb, v_texcoord0.xy, dimensions, dimensions, dimensions, 256.0, 2160.0);
	}
	// If not HDR mode nor NTSC-J mode, we should already be in gamma-space sRGB.

	gl_FragColor = color;
}
