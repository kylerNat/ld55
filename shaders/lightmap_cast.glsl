#include "include/header.glsl"
#define UBO_BINDING 6
#include "include/global_uniforms.glsl"
#include "include/aabb_tree.glsl"

layout(binding = 3) uniform sampler2D lightmap_color;
layout(binding = 4) uniform sampler2D lightmap_depth;

#include "include/lightmap.glsl"

layout (local_size_x = LIGHTMAP_CAST_LOCAL_SIZE, local_size_y = 1, local_size_z = 1) in;

struct cast_result {
    vec3 color;
    float depth;
};

layout(std140, binding = 5) buffer lightmap_casts {
    cast_result casts[];
};

void main() {
    ivec2 probe_coord = ivec2(gl_WorkGroupID.xy);
    int probe_index = probe_coord.x+probe_coord.y*LIGHTMAP_LAYOUT_W;
    vec3 probe_pos = LIGHTPROBE_SPACING*vec3(probe_index%LIGHTPROBE_GRID_W, (probe_index/LIGHTPROBE_GRID_W)%LIGHTPROBE_GRID_H, (probe_index/(LIGHTPROBE_GRID_W*LIGHTPROBE_GRID_H)))+LIGHTPROBE_OFFSET;

    int cast_index = int(probe_index*LIGHTPROBE_CASTS_PER_FRAME + gl_LocalInvocationID.x);
    vec2 oct = 2.0*hash2d(cast_index+frame_number*N_LIGHTPROBES*LIGHTPROBE_CASTS_PER_FRAME)-1.0;
    vec3 dir = oct_to_vec(oct);
    float hit_t;
    vec3 hit_pos;
    vec3 normal;
    vec3 color = vec3(0.0);
    int pid;
    vec3 albedo = vec3(0.0);
    vec3 emission = vec3(0.0);
    bool hit = cast_ray(probe_pos, dir, 100000.0, -1, hit_t, hit_pos, normal, albedo, emission);
    if(!hit) {
        color = vec3(0.01+0.01*dir.z);
    } else {
        vec2 depth;
        color = albedo*sample_lightmap_color(hit_pos, normal, vec_to_oct(normal), depth);
        color = clamp(color, 0.0, 1.0);
        color += emission;
    }
    hit_t = min(hit_t, LIGHTPROBE_SPACING);

    //TODO: get emission color and sample old probes around hit point

    casts[cast_index].color = color;
    casts[cast_index].depth = hit_t;
}
