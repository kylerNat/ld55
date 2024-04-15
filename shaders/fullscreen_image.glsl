#ifdef VERT /////////////////////////////////////

layout(location = 0) out vec2 uv;

const vec2 positions[4] = vec2[](vec2(1.f, 1.f),
                                 vec2(0.f, 1.f),
                                 vec2(0.f, 0.f),
                                 vec2(1.f, 0.f));

void main()
{
    uv = positions[gl_VertexIndex];
    gl_Position = vec4(2.0*uv-1.0, 0.0, 1.0);
}

#endif //////////////////////////////////////////
#ifdef FRAG /////////////////////////////////////

layout(location = 0) out vec4 frag_color;

layout(binding = 0) uniform sampler2D image;
layout(binding = 1) uniform sampler2D ui_image;

layout(location = 0) in vec2 uv;

void main()
{
    vec2 size = vec2(textureSize(image, 0));
    vec2 s = uv*size-0.5;
    vec2 f = fract(s);
    float b = 0.5*fwidth(s.x);
    vec2 t = trunc(s) + smoothstep(0.5-b, 0.5+b, f)+0.5;
    frag_color = texture(image, t/size);
    vec4 ui_color = texture(ui_image, uv);
    frag_color = frag_color*(1.0-ui_color.a)+ui_color;
    frag_color.a = 1.0;
}

#endif //////////////////////////////////////////
