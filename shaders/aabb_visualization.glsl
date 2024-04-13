#ifdef VERT /////////////////////////////////////

#include "include/header.glsl"
#include "include/global_uniforms.glsl"
#define CUSTOM_SSBO
#include "include/aabb_tree.glsl"

layout(location = 0) out vec3 pos;
layout(location = 1) flat out int nid;

#define CUSTOM_SSBO
layout(std140, binding = 2) readonly buffer ssbo {
    int n_primitives;
    primitive_t primitives[2048];

    int n_nodes;
    aabb_t nodes[4096];
};


void main() {
    nid = gl_InstanceIndex;
    aabb_t node = nodes[nid];
    vec3 center = 0.5*(node.u+node.l);
    vec3 size   = 0.5*(node.u-node.l);
    vec3 dir = vec3(0.0);
    int i = gl_VertexIndex;
    dir[i/8] = 1.0;
    vec3 position = (i%2==0?-1.0:1.0)*dir + ((i/2)%2==0?-1.0:1.0)*dir.yzx + ((i/4)%2==0?-1.0:1.0)*dir.zxy;
    pos = center + size*position*sign(camera_pos-center);
    gl_Position = camera*vec4(pos, 1.0);
}

#endif //////////////////////////////////////////
#ifdef FRAG /////////////////////////////////////

#include "include/header.glsl"
#include "include/global_uniforms.glsl"
#include "include/aabb_tree.glsl"

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec3 pos;
layout(location = 1) flat in int nid;

void main() {
    frag_color.rgb = vec3(cos(nid), cos(nid+2.0*pi/3), cos(nid+4.0*pi/3.0));
    frag_color.a = 0.4;
}

#endif //////////////////////////////////////////
