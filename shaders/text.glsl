#ifdef VERT /////////////////////////////////////

#include "include/global_uniforms.glsl"

layout(location = 0) in vec2 l;
layout(location = 1) in vec2 u;
layout(location = 2) in vec2 lt;
layout(location = 3) in vec2 ut;
layout(location = 4) in vec4 c;

layout(location = 0) out vec2 uv;
layout(location = 1) out vec4 color;

const vec2 positions[4] = vec2[](vec2(1.f, 1.f),
                                 vec2(0.f, 1.f),
                                 vec2(0.f, 0.f),
                                 vec2(1.f, 0.f));

void main()
{
    vec2 s = positions[gl_VertexIndex];
    gl_Position.xy = mix(l, u, s);
    gl_Position.z = 0.0;
    gl_Position.w = 1.0;
    uv = mix(lt, ut, s);
    color = c;
}

#endif //////////////////////////////////////////
#ifdef FRAG /////////////////////////////////////

layout(location = 0) out vec4 frag_color;

layout(binding = 1) uniform sampler2D packed_font;

layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 color;

void main()
{
    frag_color = color;
    frag_color.a *= texture(packed_font, uv).r;
}

#endif //////////////////////////////////////////
