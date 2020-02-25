$input v_color0, v_texcoord0

#include <bgfx/bgfx_shader.sh>

SAMPLER2D(tex, 0);
SAMPLER2D(tex_u, 1);
SAMPLER2D(tex_v, 2);

uniform vec4 VSFlags;
uniform vec4 FSAlphaFlags;
uniform vec4 FSMiscFlags;

#define isFBTexture VSFlags.z > 0.0
#define isTexture VSFlags.w > 0.0
// ---
#define inAlphaRef FSAlphaFlags.x
#define inAlphaFunc FSAlphaFlags.y
#define doAlphaTest FSAlphaFlags.z > 0.0
// ---
#define isFullRange FSMiscFlags.x > 0.0
#define isYUV FSMiscFlags.y > 0.0
#define modulateAlpha FSMiscFlags.z > 0.0
#define isMovie FSMiscFlags.w > 0.0

void main()
{
	mediump vec4 color = v_color0;
    
    if (isYUV)
    {
        const mat4 mpeg_rgb_transform = mat4(
            +1.164, +1.164, +1.164, +0.000,
            +0.000, -0.392, +2.017, +0.000,
            +1.596, -0.813, +0.000, +0.000,
            +0.000, +0.000, +0.000, +1.000
        );

        const mat4 jpeg_rgb_transform = mat4(
            +1.000, +1.000, +1.000, +0.000,
            +0.000, -0.343, +1.765, +0.000,
            +1.400, -0.711, +0.000, +0.000,
            +0.000, +0.000, +0.000, +1.000
        );

        vec4 yuv = vec4(
            texture2D(tex, v_texcoord0.xy).r - (1.0 / 16.0),
            texture2D(tex_u, v_texcoord0.xy).r - 0.5,
            texture2D(tex_v, v_texcoord0.xy).r - 0.5,
            1.0
        );

#if BGFX_SHADER_LANGUAGE_SPIRV || BGFX_SHADER_LANGUAGE_HLSL
        if (isFullRange) color = mul(yuv, jpeg_rgb_transform);
        else color = mul(yuv, mpeg_rgb_transform);
#else
        if (isFullRange) color = mul(jpeg_rgb_transform, yuv);
        else color = mul(mpeg_rgb_transform, yuv);
#endif

        color.a = 1.0f;
    }
    else
    {
        if (doAlphaTest)
        {
            // ALPHA TEST
            if ( inAlphaFunc == 0.0) //NEVER
                discard;
            else if ( inAlphaFunc == 1.0) //LESS
                if (!(color.a < inAlphaRef)) discard;
            else if ( inAlphaFunc == 2.0) //EQUAL
                if (!(color.a == inAlphaRef)) discard;
            else if ( inAlphaFunc == 3.0) //LEQUAL
                if (!(color.a <= inAlphaRef)) discard;
            else if ( inAlphaFunc == 4.0) //GREATER
                if (!(color.a > inAlphaRef)) discard;
            else if ( inAlphaFunc == 5.0) //NOTEQUAL
                if (!(color.a != inAlphaRef)) discard;
            else if ( inAlphaFunc == 6.0) //GEQUAL
                if (!(color.a >= inAlphaRef)) discard;
        }

        if (isTexture)
        {
            mediump vec4 texture_color = texture2D(tex, v_texcoord0.xy);

            if (isFBTexture && all(equal(texture_color.rgb,vec3_splat(0.0)))) discard;

            if (isMovie) texture_color.a = 1.0;

            if (texture_color.a == 0.0) discard;

            color *= texture_color;

            if (modulateAlpha) color.a = texture_color.a;
        }
    }

	gl_FragColor = color;
}
