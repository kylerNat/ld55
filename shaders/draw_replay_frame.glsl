#ifdef VERT /////////////////////////////////////

#include "include/global_uniforms.glsl"

layout(location = 0) in vec2 p;
layout(location = 1) in float scale;
layout(location = 2) in uint image_index;

layout(location = 0) out vec2 uv;
layout(location = 1) flat out uint index;

const vec2 positions[4] = vec2[](vec2(1.f, 1.f),
                                 vec2(0.f, 1.f),
                                 vec2(0.f, 0.f),
                                 vec2(1.f, 0.f));

void main()
{
    uv = positions[gl_VertexIndex];
    gl_Position = vec4(vec2(p.x/aspect_ratio, p.y)+scale*(2.0*uv-1.0), 0.0, 1.0);
    index = image_index;
}

#endif //////////////////////////////////////////
#ifdef FRAG /////////////////////////////////////

layout(location = 0) out vec4 frag_color;

layout(constant_id = 0) const uint max_frames = 60;

layout(set = 1, binding = 0) uniform sampler2D frames[max_frames];

layout(location = 0) in vec2 uv;
layout(location = 1) flat in uint index;

void main()
{
    vec2 size = vec2(textureSize(frames[index], 0));
    vec2 s = uv*size-0.5;
    vec2 f = fract(s);
    float b = 0.5*fwidth(s.x);
    vec2 t = trunc(s) + smoothstep(0.5-b, 0.5+b, f)+0.5;
    frag_color = texture(frames[index], t/size);
}

#endif //////////////////////////////////////////
