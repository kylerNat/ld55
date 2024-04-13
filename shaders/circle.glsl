#ifdef VERT /////////////////////////////////////

#include "include/global_uniforms.glsl"

layout(location = 0) in vec3 p;
layout(location = 1) in float r;
layout(location = 2) in vec4 c;

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 uv;

const vec2 positions[4] = vec2[](vec2( 1.f,  1.f),
                                 vec2(-1.f,  1.f),
                                 vec2(-1.f, -1.f),
                                 vec2( 1.f, -1.f));

void main() {
    uv = positions[gl_VertexIndex];
    gl_Position = vec4(p.xy+r*uv, p.z, 1.0);
    gl_Position.x /= aspect_ratio;
    // gl_Position.xy += r*uv;
    color = c;
}

#endif //////////////////////////////////////////
#ifdef FRAG /////////////////////////////////////

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec4 color;
layout(location = 1) in vec2 uv;

void main() {
    float r = length(uv);
    float s = fwidth(r);
    float a = smoothstep(1.0, 1.0-s, r);
    frag_color = color;
    frag_color.a *= a;
}

#endif //////////////////////////////////////////
