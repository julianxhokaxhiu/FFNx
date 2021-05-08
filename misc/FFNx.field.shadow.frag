/****************************************************************************/
//    Copyright (C) 2021 Cosmos                                             //
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

$input v_position0, v_color0, v_shadow0

#include <bgfx/bgfx_shader.sh>
#define FIELD_SHADOW 1
#include "FFNx.pcf.sh"

uniform vec4 lightingDebugData;

// ---
#define isShowWalkmeshEnabled lightingDebugData.y > 0.0

void main()
{
    // Shadow UV
    vec4 shadowUv = v_shadow0  / v_shadow0.w;

    // Shadow Factor
    float shadowFactor = sampleShadowMapPCF7x7(shadowUv.xyz, v_position0.xyz);
    float shadowOcclusion = fieldShadowData.x;
    shadowFactor = shadowOcclusion + (1.0 - shadowOcclusion) * shadowFactor;

    if(isShowWalkmeshEnabled)
    {
        gl_FragColor = vec4(v_color0.rgb * shadowFactor, 1.0);
    }
    else
    {
        gl_FragColor = vec4(0.0, 0.0, 0.0, mix(0.0, 1.0 -  shadowFactor, v_color0.a));
    }

#if BGFX_SHADER_LANGUAGE_HLSL > 400
    // Offsets depth to prevent some weird occlusion problems with field 2D tiles
    float depthOffset = 0.0075;
    gl_FragDepth =  (1.0 + depthOffset) * gl_FragCoord.z - depthOffset;
#endif
}
