$input v_color0, v_texcoord0

#include <bgfx/bgfx_shader.sh>

SAMPLER2D(tex, 0);

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
#define inheritTextureAlpha FSMiscFlags.z > 0.0

void main()
{
	gl_FragColor = texture2D(tex, v_texcoord0.xy);
}
