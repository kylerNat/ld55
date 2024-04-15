#include "include/header.glsl"
#define UBO_BINDING 6
#include "include/global_uniforms.glsl"
#include "include/aabb_tree.glsl"

#define NO_LIGHTMAP_SAMPLING
#include "include/lightmap.glsl"

layout (local_size_x = LIGHTMAP_ACCUMULATE_COLOR_LOCAL_W, local_size_y = LIGHTMAP_ACCUMULATE_COLOR_LOCAL_H, local_size_z = 1) in;

layout(binding = 0, rgba32f) uniform writeonly image2D lightmap_color_out;
layout(binding = 3) uniform sampler2D lightmap_color;

struct cast_result {
    vec3 color;
    float depth;
};

layout(std140, binding = 5) buffer lightmap_casts {
    cast_result casts[];
};

const float epsilon = 1e-6;

void main() {
    ivec2 o = ivec2(gl_GlobalInvocationID.xy % LIGHTPROBE_COLOR_RESOLUTION);
    ivec2 probe_coord = ivec2(gl_GlobalInvocationID.xy / LIGHTPROBE_COLOR_RESOLUTION);
    ivec2 tex_coord = probe_coord*LIGHTPROBE_COLOR_PADDED_RESOLUTION+1;
    int probe_index = probe_coord.x+probe_coord.y*LIGHTMAP_LAYOUT_W;

    vec2 pixel_oct = (2.0/LIGHTPROBE_COLOR_RESOLUTION)*(vec2(o)+0.5)-1.0;
    vec3 pixel_dir = oct_to_vec(pixel_oct);

    vec4 texel_color = texelFetch(lightmap_color, tex_coord+o, 0);
    vec4 new_color = vec4(0.0);

    for(int i = 0; i < LIGHTPROBE_CASTS_PER_FRAME; i++) {
        int cast_index = probe_index*LIGHTPROBE_CASTS_PER_FRAME+i;

        if(casts[cast_index].depth < 0.0) continue;

        vec2 oct = 2.0*hash2d(cast_index+frame_number*N_LIGHTPROBES*LIGHTPROBE_CASTS_PER_FRAME)-1.0;
        vec3 dir = oct_to_vec(oct);

        vec4 color = vec4(casts[cast_index].color, 1.0);

        float weight = (1.0/LIGHTPROBE_CASTS_PER_FRAME)*max(0.0, dot(pixel_dir, dir));

        if(weight > epsilon) {
            new_color += weight*color;
        }
    }

    float decay_fraction = 0.01;
    if(new_color.a > epsilon) {
        texel_color.rgb = mix(texel_color.rgb, new_color.rgb/new_color.a, decay_fraction);
    }

    imageStore(lightmap_color_out, tex_coord+o, texel_color);

    ivec2 o1 = ivec2(o.x, LIGHTPROBE_COLOR_RESOLUTION-1-o.y);
    ivec2 o2 = ivec2(LIGHTPROBE_COLOR_RESOLUTION-1-o.x, o.y);
    ivec2 o3 = ivec2(LIGHTPROBE_COLOR_RESOLUTION-1-o.x, LIGHTPROBE_COLOR_RESOLUTION-1-o.y);

    if(o.x == 0) {o1.x--; o3.x++;}
    if(o2.x == 0) {o1.x++; o3.x--;}
    if(o1.x != o.x) imageStore(lightmap_color_out, tex_coord+o1, texel_color);

    if(o.y == 0) {o2.y--; o3.y++;}
    if(o1.y == 0) {o2.y++; o3.y--;}
    if(o2.y != o.y) imageStore(lightmap_color_out, tex_coord+o2, texel_color);

    if(o1.x != o.x && o2.y != o.y) imageStore(lightmap_color_out, tex_coord+o3, texel_color);
}
