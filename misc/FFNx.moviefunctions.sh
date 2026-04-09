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

// Y'UV to R'G'B' ---------------------------------------------------------

// Get Y'UV from full-range 0.0 to 1.0 values into range 0.0 to 1.0 for Y' and -0.5 to 0.5 for U and V.
// See https://www.itu.int/dms_pubrec/itu-r/rec/bt/r-rec-bt.601-7-201103-i!!pdf-e.pdf
// The standard assumes 10bit = 8bit x 4, so the constants for 10bit calculations are just 4x the 8bit constants.
// (The standard doesn't care about leaving 1021-2023 empty because it's a limited-range standard.
// Presumably the encoder did care and used the full range.)
vec3 normalizefullrangeYUV(vec3 yuv){
	// center UV at 128(x4)
	yuv.g = yuv.g - (512.0/1023.0);
	yuv.b = yuv.b - (512.0/1023.0);
	// fix UV scale b/c 128 isn't quite halfway between 0 and 255
  // nor is 512 exactly halfway between 0 and 1023
	float uscalar = (yuv.g < 0.0) ? 1023.0/1024.0 : 1023.0/1022.0;
	float vscalar = (yuv.b < 0.0) ? 1023.0/1024.0 : 1023.0/1022.0;
	yuv.g = clamp(yuv.g * uscalar, -0.5, 0.5);
	yuv.b = clamp(yuv.b * vscalar, -0.5, 0.5);
	return yuv;
}
/* 8 bit version
vec3 normalizefullrangeYUV(vec3 yuv){
	// center UV at 128
	yuv.g = yuv.g - (128.0/255.0);
	yuv.b = yuv.b - (128.0/255.0);
	// fix UV scale b/c 128 isn't quite halfway between 0 and 255
	float uscalar = (yuv.g < 0.0) ? 255.0/256.0 : 255.0/254.0;
	float vscalar = (yuv.b < 0.0) ? 255.0/256.0 : 255.0/254.0;
	yuv.g = clamp(yuv.g * uscalar, -0.5, 0.5);
	yuv.b = clamp(yuv.b * vscalar, -0.5, 0.5);
	return yuv;
}
*/

// Get Y'UV from limited-range 0.0 to 1.0 values into range 0.0 to 1.0 for Y' and -0.5 to 0.5 for U and V.
// See https://www.itu.int/dms_pubrec/itu-r/rec/bt/r-rec-bt.601-7-201103-i!!pdf-e.pdf
// The standard assumes 10bit = 8bit x 4, so the constants for 10bit calculations are just 4x the 8bit constants.
// (The standard doesn't care about leaving 1021-2023 empty because it's a limited-range standard.)
vec3 normalizelimitedrangeYUV(vec3 yuv, bool isBink){
	// subtract 16(x4) from Y'
	yuv.r = saturate(yuv.r - (64.0/1023.0));
	// scale Y'
	// 601/709 standard for limited Y range is 16-235 (x4), but bink uses 16-234 (x4)
	float yscalar = (isBink) ? 1023.0/872.0 : 1023.0/876.0;
	yuv.r = saturate(yuv.r * yscalar);
	// center UV at 128 (x4)
	yuv.g = yuv.g - (512.0/1023.0);
	yuv.b = yuv.b - (512.0/1023.0);
	// scale UV
	yuv.g = clamp(yuv.g * (1023.0/896.0), -0.5, 0.5);
	yuv.b = clamp(yuv.b * (1023.0/896.0), -0.5, 0.5);
	return yuv;
}
/* 8-bit version
vec3 normalizelimitedrangeYUV(vec3 yuv, bool isBink){
	// subtract 16 from Y'
	yuv.r = saturate(yuv.r - (16.0/255.0));
	// scale Y'
	// 601/709 standard for limited Y range is 16-235, but bink uses 16-234
	float yscalar = (isBink) ? 255.0/218.0 : 255.0/219.0;
	yuv.r = saturate(yuv.r * yscalar);
	// center UV at 128
	yuv.g = yuv.g - (128.0/255.0);
	yuv.b = yuv.b - (128.0/255.0);
	// scale UV
	yuv.g = clamp(yuv.g * (255.0/224.0), -0.5, 0.5);
	yuv.b = clamp(yuv.b * (255.0/224.0), -0.5, 0.5);
	return yuv;
}
*/

// matrix Y'UV to R'G'B' using rec601 matrix
vec3 toRGB_bt601_fullrange(vec3 yuv_input)
{
	// https://www.itu.int/dms_pubrec/itu-r/rec/bt/r-rec-bt.601-7-201103-i!!pdf-e.pdf
	mat3 jpeg_rgb_transform = mtxFromCols(
		vec3(+1.000, +1.000, +1.000),
		vec3(+0.000, -(0.114 * 1.772) / 0.587, +1.772),
		vec3(+1.402, -(0.299 * 1.402) / 0.587, +0.000)
	);
	return saturate(mul(jpeg_rgb_transform, yuv_input));
}

// matrix Y'UV to R'G'B' using rec709 matrix
vec3 toRGB_bt709_fullrange(vec3 yuv_input)
{
	// https://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.709-6-201506-I!!PDF-E.pdf
	mat3 bt709full_rgb_transform = mtxFromCols(
		vec3(+1.000, +1.000, +1.000),
		vec3(+0.000, -(0.0722 * 1.8556) / 0.7152, +1.8556),
		vec3(+1.5748, -(0.2126 * 1.5748) / 0.7152 , +0.000)
	);
	return saturate(mul(bt709full_rgb_transform, yuv_input));
}

// matrix Y'UV to R'G'B' using bink matrix (FF8 PC2000 and Steam videos)
vec3 toRGB_bink_fullrange(vec3 yuv_input)
{
	mat3 bink_yuv_rgb_transform = mtxFromCols(
		vec3(+1.000, +1.000, +1.000),
		vec3(+0.000, -(0.299 * 1.402) / 0.587, +1.402),
		vec3(+1.772, -(0.114 * 1.772) / 0.587, +0.000)
	);
	return saturate(mul(bink_yuv_rgb_transform, yuv_input));
}

// Gamma functions ------------------------------------------------

// commonly used for rec601 and rec709 movies
vec3 toLinearSMPTE170M(vec3 _rgb)
{
	bvec3 cutoff = lessThan(_rgb.rgb, vec3_splat(0.0812));
	vec3 higher = pow((_rgb.rgb + vec3_splat(0.099)) / vec3_splat(1.099), (vec3_splat(1.0) / vec3_splat(0.45)));
	vec3 lower = _rgb.rgb / vec3_splat(4.5);

	return saturate(mix(higher, lower, cutoff));
}
