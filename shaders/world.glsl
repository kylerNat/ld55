#ifdef VERT /////////////////////////////////////

#include "include/header.glsl"
#include "include/global_uniforms.glsl"
#include "include/aabb_tree.glsl"

layout(location = 0) out vec3 pos;
layout(location = 1) flat out int pid;

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
    pid = gl_InstanceIndex;
    primitive_t prim = primitives[pid];
    if(is_inside(camera_pos, prim.aabb)) {
        float screen_dist = 1.0/tan(0.5*fov);
        vec2 uv = inside_positions[gl_VertexIndex];
        pos = camera_pos+camera_axes*vec3(uv.x*aspect_ratio, uv.y, screen_dist);
        gl_Position = vec4(uv, 0.0, 1.0);
    } else {
        vec3 center = 0.5*(prim.aabb.u+prim.aabb.l);
        vec3 size   = 0.5*(prim.aabb.u-prim.aabb.l);
        pos = center + size*positions[gl_VertexIndex]*sign_not_zero(camera_pos-center);
        gl_Position = camera*vec4(pos, 1.0);
    }
}

#endif //////////////////////////////////////////
#ifdef FRAG /////////////////////////////////////

#include "include/header.glsl"
#include "include/global_uniforms.glsl"
#include "include/aabb_tree.glsl"

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec3 pos;
layout(location = 1) flat in int pid;

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
    bool hit = raycast_primitive(pid, p, d, t, hit_pos, normal);
    if(!hit) {
        discard;
        return;
    }
    // hit_pos += 0.1*l;
    vec3 light = vec3(1.0);
    // if(ln > 0.0) {
    //     light *= shadow(hit_pos, l, length(lr), pid, light_t, hit_pos, normal2);
    // }
    vec2 depth;
    light = primitives[pid].albedo*sample_lightmap_color(hit_pos, normal, vec_to_oct(normal), depth);
    light += primitives[pid].emission;
    frag_color.rgb = light*do_proc_texture(vec_to_oct(normal), primitives[pid].texture_type);

    frag_color.a = 1.0;

    float n = 0.1;
    float f = 1000.0;
    gl_FragDepth = (f*(t-n)/(t*(f-n)));
}

#endif //////////////////////////////////////////
