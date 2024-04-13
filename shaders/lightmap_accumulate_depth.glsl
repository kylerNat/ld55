#include "include/header.glsl"
#include "include/global_uniforms.glsl"
#include "include/aabb_tree.glsl"

#define NO_LIGHTMAP_SAMPLING
#include "include/lightmap.glsl"

layout (local_size_x = LIGHTMAP_ACCUMULATE_DEPTH_LOCAL_W, local_size_y = LIGHTMAP_ACCUMULATE_DEPTH_LOCAL_H, local_size_z = 1) in;

layout(binding = 1, rg32f) uniform writeonly image2D lightmap_depth_out;
layout(binding = 4) uniform sampler2D lightmap_depth;

layout(push_constant) uniform constants {
    int frame_number;
};

struct cast_result {
    vec3 color;
    float depth;
};

layout(std140, binding = 5) buffer lightmap_casts {
    cast_result casts[];
};

const float epsilon = 1e-6;

void main() {
    ivec2 o = ivec2(gl_GlobalInvocationID.xy % LIGHTPROBE_DEPTH_RESOLUTION);
    ivec2 probe_coord = ivec2(gl_GlobalInvocationID.xy / LIGHTPROBE_DEPTH_RESOLUTION);
    ivec2 tex_coord = probe_coord*LIGHTPROBE_DEPTH_PADDED_RESOLUTION+1;
    int probe_index = probe_coord.x+probe_coord.y*LIGHTMAP_LAYOUT_W;

    vec2 pixel_oct = (2.0/LIGHTPROBE_DEPTH_RESOLUTION)*(vec2(o)+0.5)-1.0;
    vec3 pixel_dir = oct_to_vec(pixel_oct);

    vec2 texel_depth = texelFetch(lightmap_depth, tex_coord+o, 0).rg;
    vec3 new_depth = vec3(0.0);

    for(int i = 0; i < LIGHTPROBE_CASTS_PER_FRAME; i++) {
        int cast_index = probe_index*LIGHTPROBE_CASTS_PER_FRAME+i;

        if(casts[cast_index].depth < 0.0) continue;

        vec2 oct = 2.0*hash2d(cast_index+frame_number*N_LIGHTPROBES*LIGHTPROBE_CASTS_PER_FRAME)-1.0;
        vec3 dir = oct_to_vec(oct);

        vec3 depth;
        depth.r = casts[cast_index].depth;
        depth.g = sq(casts[cast_index].depth);
        depth.b = 1.0;

        float weight = (1.0/LIGHTPROBE_CASTS_PER_FRAME)*pow(max(0.0, dot(pixel_dir, dir)), 51.0);
        if(weight > epsilon) {
            new_depth += weight*depth;
        }
    }

    float decay_fraction = 0.01;
    if(new_depth.z > epsilon) {
        texel_depth = mix(texel_depth, new_depth.xy/new_depth.z, decay_fraction);
    }

    imageStore(lightmap_depth_out, tex_coord+o, vec4(texel_depth, 0.0, 1.0));

    ivec2 o1 = ivec2(o.x, LIGHTPROBE_DEPTH_RESOLUTION-1-o.y);
    ivec2 o2 = ivec2(LIGHTPROBE_DEPTH_RESOLUTION-1-o.x, o.y);
    ivec2 o3 = ivec2(LIGHTPROBE_DEPTH_RESOLUTION-1-o.x, LIGHTPROBE_DEPTH_RESOLUTION-1-o.y);

    if(o.x == 0) {o1.x--; o3.x++;}
    if(o2.x == 0) {o1.x++; o3.x--;}
    if(o1.x != o.x) imageStore(lightmap_depth_out, tex_coord+o1, vec4(texel_depth, 0.0, 1.0));

    if(o.y == 0) {o2.y--; o3.y++;}
    if(o1.y == 0) {o2.y++; o3.y--;}
    if(o2.y != o.y) imageStore(lightmap_depth_out, tex_coord+o2, vec4(texel_depth, 0.0, 1.0));

    if(o1.x != o.x && o2.y != o.y) imageStore(lightmap_depth_out, tex_coord+o3, vec4(texel_depth, 0.0, 1.0));
}
