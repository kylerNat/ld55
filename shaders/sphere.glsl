#ifdef VERT /////////////////////////////////////

#include "include/header.glsl"
#include "include/global_uniforms.glsl"
#include "include/maths.glsl"

layout(location = 0) in vec3 p;
layout(location = 1) in float r;
layout(location = 2) in vec4 c;

layout(location = 0) out vec3 pos;
layout(location = 1) out vec3 center;
layout(location = 2) out float radius;
layout(location = 3) out vec4 color;

const vec3 positions[8] = vec3[](vec3( 1.f,  1.f, 1.f),
                                 vec3(-1.f,  1.f, 1.f),
                                 vec3(-1.f, -1.f, 1.f),
                                 vec3( 1.f, -1.f, 1.f),

                                 vec3( 1.f, -1.f, -1.f),
                                 vec3( 1.f,  1.f, -1.f),

                                 vec3(-1.f,  1.f, -1.f),
                                 vec3(-1.f,  1.f,  1.f));

const vec2 inside_positions[8] = vec2[](vec2( 1.f,  1.f),
                                        vec2(-1.f,  1.f),
                                        vec2(-1.f, -1.f),
                                        vec2( 1.f, -1.f),
                                        vec2(nan),
                                        vec2(nan),
                                        vec2(nan),
                                        vec2(nan));

void main() {
    vec3 d = camera_pos-p;
    if(dot(d, d) < r*r) {
        float screen_dist = 1.0/tan(0.5*fov);
        vec2 uv = inside_positions[gl_VertexIndex];
        pos = camera_pos+camera_axes*vec3(uv.x*aspect_ratio, uv.y, screen_dist);
        gl_Position = vec4(uv, 0.0, 1.0);
    } else {
        pos = p + r*positions[gl_VertexIndex]*sign_not_zero(d);
        gl_Position = camera*vec4(pos, 1.0);
    }
    center = p;
    radius = r;
    color = c;
}

#endif //////////////////////////////////////////
#ifdef FRAG /////////////////////////////////////

#include "include/header.glsl"
#include "include/global_uniforms.glsl"
#include "include/aabb_tree.glsl"

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 center;
layout(location = 2) in float radius;
layout(location = 3) in vec4 color;

layout(binding = 3) uniform sampler2D lightmap_color;
layout(binding = 4) uniform sampler2D lightmap_depth;

#include "include/lightmap.glsl"

void main() {
    vec3 p = camera_pos;
    vec3 d = pos-p;
    d = normalize(d);

    bool hit = false;
    float hit_t = 0.0;
    vec3 hit_pos = vec3(0.0);
    vec3 normal = vec3(0.0,0.0,1.0);

    vec3 r = center-p;
    float dist = dot(r, d);
    float bsq = dot(r,r)-sq(dist);
    float rsq = sq(radius);
    if(dot(r, r) < rsq) {
        float t = dist+sqrt(rsq-bsq);
        hit_t = t;
        hit_pos = p+t*d;
        normal = normalize(hit_pos-pos);
        hit = true;
    } else
    if(bsq < rsq) {
        float t = dist-sqrt(rsq-bsq);
        if(t > 0.0) {
            hit_t = t;
            hit_pos = p+t*d;
            normal = normalize(hit_pos-pos);
            hit = true;
        }
    }
    if(!hit) {
        discard;
        return;
    }

    frag_color = color;

    // {
    //     float n = 0.1;
    //     float f = 1000.0;
    //     gl_FragDepth = (f*(hit_t-n)/(hit_t*(f-n)));
    // }
    gl_FragDepth = 0.0;
}
#endif //////////////////////////////////////////
