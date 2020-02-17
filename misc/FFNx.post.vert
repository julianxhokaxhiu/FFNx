$input a_position, a_color0, a_texcoord0
$output v_color0, v_texcoord0

#include <bgfx/bgfx_shader.sh>

uniform highp mat4 d3dViewport;
uniform highp mat4 d3dProjection;
uniform highp mat4 worldView;

uniform vec4 VSFlags;
#define isTLVertex VSFlags.x > 0.0
#define blendMode VSFlags.y
#define isFBTexture VSFlags.z > 0.0

void main()
{
	highp vec4 pos = a_position;
    lowp vec4 color = a_color0;
    mediump vec2 coords = a_texcoord0;

    color.rgba = color.bgra;

    pos.w = 1.0 / pos.w;
    pos.xyz *= pos.w;
    pos = mul(u_proj, pos);

    gl_Position = pos;
    v_color0 = color;
    v_texcoord0 = coords;
}

