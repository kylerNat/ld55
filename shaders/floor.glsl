#ifdef VERT /////////////////////////////////////

#include "include/header.glsl"
#include "include/global_uniforms.glsl"
#include "include/aabb_tree.glsl"

layout(location = 0) out vec3 pos;

const vec2 inside_positions[4] = vec2[](vec2( 1.f,  1.f),
                                        vec2(-1.f,  1.f),
                                        vec2(-1.f, -1.f),
                                        vec2( 1.f, -1.f));

void main() {
    float screen_dist = 1.0/tan(0.5*fov);
    vec2 uv = inside_positions[gl_VertexIndex];
    pos = camera_pos+camera_axes*vec3(uv.x*aspect_ratio, uv.y, screen_dist);
    gl_Position = vec4(uv, 0.0, 1.0);
}

#endif //////////////////////////////////////////
#ifdef FRAG /////////////////////////////////////

#include "include/header.glsl"
#include "include/global_uniforms.glsl"
#include "include/aabb_tree.glsl"

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec3 pos;

layout(binding = 3) uniform sampler2D lightmap_color;
layout(binding = 4) uniform sampler2D lightmap_depth;

#include "include/lightmap.glsl"

void main() {
    vec3 p = camera_pos;
    vec3 d = pos-p;
    d = normalize(d);
    float t = 10000.0;

    vec3 hit_pos = p;
    vec3 normal = vec3(0.0,0.0,1.0);
    vec3 albedo = vec3(0.0);
    vec3 emission = vec3(0.0);
    bool hit = raycast_floor(p, d, t, hit_pos, normal, albedo, emission);
    if(!hit) {
        discard;
        return;
    }
    vec3 light = vec3(1.0);
    // if(ln > 0.0) {
    //     light *= shadow(hit_pos, l, length(lr), pid, light_t, hit_pos, normal2);
    // }
    vec2 depth;
    light = albedo*sample_lightmap_color(hit_pos, normal, vec_to_oct(normal), depth);
    light += emission;
    float t2 = 0.0;
    for(int i = 0; i < 64; i++) {
        float s = 0.1+0.05*t2;
        hit_pos -= s*d;
        t2 += s;
        vec3 emis = get_floor_emission(hit_pos.xy);
        light += 0.001*(0.5+0.5*emis.g)*emis/hit_pos.z;
    }
    frag_color.rgb = light;

    frag_color.a = 1.0;

    float n = 0.1;
    float f = 1000.0;
    gl_FragDepth = (f*(t-n)/(t*(f-n)));
}

#endif //////////////////////////////////////////
