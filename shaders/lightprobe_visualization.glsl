#define RADIUS 0.05

#ifdef VERT /////////////////////////////////////

#include "include/header.glsl"
#include "include/global_uniforms.glsl"
#include "include/aabb_tree.glsl"

#include "include/lightmap_constants.h"

layout(location = 0) out vec3 pos;
layout(location = 1) flat out int probe_index;

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
    probe_index = gl_InstanceIndex;

    vec3 probe_pos = LIGHTPROBE_SPACING*vec3(probe_index%LIGHTPROBE_GRID_W, (probe_index/LIGHTPROBE_GRID_W)%LIGHTPROBE_GRID_H, (probe_index/(LIGHTPROBE_GRID_W*LIGHTPROBE_GRID_H)))+LIGHTPROBE_OFFSET;

    if(is_inside(camera_pos-probe_pos, vec3(RADIUS,RADIUS,RADIUS))) {
        float screen_dist = 1.0/tan(0.5*fov);
        vec2 uv = inside_positions[gl_VertexIndex];
        pos = camera_pos+camera_axes*vec3(uv.x*aspect_ratio, uv.y, screen_dist);
        gl_Position = vec4(uv, 0.0, 1.0);
    } else {
        pos = probe_pos + RADIUS*positions[gl_VertexIndex]*sign_not_zero(camera_pos-probe_pos);
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
layout(location = 1) flat in int probe_index;

layout(binding = 3) uniform sampler2D lightmap_color;
layout(binding = 4) uniform sampler2D lightmap_depth;

#include "include/lightmap.glsl"

void main() {
    vec3 p = camera_pos;
    vec3 d = pos-p;
    d = normalize(d);

    vec3 probe_pos = LIGHTPROBE_SPACING*vec3(probe_index%LIGHTPROBE_GRID_W, (probe_index/LIGHTPROBE_GRID_W)%LIGHTPROBE_GRID_H, (probe_index/(LIGHTPROBE_GRID_W*LIGHTPROBE_GRID_H)))+LIGHTPROBE_OFFSET;

    bool hit = false;
    float hit_t = 0.0;
    vec3 hit_pos = vec3(0.0);
    vec3 normal = vec3(0.0,0.0,1.0);

    vec3 r = probe_pos-p;
    float dist = dot(r, d);
    float bsq = dot(r,r)-sq(dist);
    float rsq = sq(RADIUS);
    if(dot(r, r) < rsq) {
        float t = dist+sqrt(rsq-bsq);
        hit_t = t;
        hit_pos = p+t*d;
        normal = normalize(hit_pos-probe_pos);
        hit = true;
    } else if(bsq < rsq) {
        float t = dist-sqrt(rsq-bsq);
        if(t > 0.0) {
            hit_t = t;
            hit_pos = p+t*d;
            normal = normalize(hit_pos-probe_pos);
            hit = true;
        }
    }
    if(!hit) {
        discard;
        return;
    }

    ivec2 probe_coord = ivec2(probe_index%LIGHTMAP_LAYOUT_W, probe_index/LIGHTMAP_LAYOUT_W);
    vec2 sample_oct = vec_to_oct(normal);
    vec2 sample_coord = vec2(LIGHTPROBE_COLOR_PADDED_RESOLUTION*probe_coord+1)+LIGHTPROBE_COLOR_RESOLUTION*clamp(0.5f*sample_oct+0.5f,0.0,1.0);

    sample_coord += 0.5;
    vec2 t = trunc(sample_coord);
    vec2 f = fract(sample_coord);
    f = f*f*f*(f*(f*6.0-15.0)+10.0);
    // f = f*f*(-2*f+3);
    sample_coord = t+f-0.5;

    sample_coord *= vec2(1.0f/LIGHTMAP_COLOR_TEXTURE_RESOLUTION, 1.0f/LIGHTMAP_COLOR_TEXTURE_RESOLUTION);

    vec4 probe_color = texture(lightmap_color, sample_coord);

    vec2 depth_sample_coord = vec2(LIGHTPROBE_DEPTH_PADDED_RESOLUTION*probe_coord+1.0)+LIGHTPROBE_DEPTH_RESOLUTION*clamp(0.5f*sample_oct+0.5f,0.0,1.0);
    depth_sample_coord *= vec2(1.0f/LIGHTMAP_DEPTH_TEXTURE_RESOLUTION, 1.0f/LIGHTMAP_DEPTH_TEXTURE_RESOLUTION);
    vec2 probe_depth = texture(lightmap_depth, depth_sample_coord).rg;

    frag_color.rgb = probe_color.rgb;
    frag_color.a = 1.0;

    {
        float n = 0.1;
        float f = 1000.0;
        gl_FragDepth = (f*(hit_t-n)/(hit_t*(f-n)));
    }
}

#endif //////////////////////////////////////////
