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

layout(location = 0) in vec2 uv;

void main()
{
    frag_color = texture(image, uv);
}

#endif //////////////////////////////////////////
