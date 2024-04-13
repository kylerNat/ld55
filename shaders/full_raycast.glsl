#ifdef VERT /////////////////////////////////////

#include "include/global_uniforms.glsl"

layout(location = 0) out vec2 uv;

const vec2 positions[4] = vec2[](vec2( 1.f,  1.f),
                                 vec2(-1.f,  1.f),
                                 vec2(-1.f, -1.f),
                                 vec2( 1.f, -1.f));

void main() {
    uv = positions[gl_VertexIndex];
    gl_Position = vec4(uv, 0.0, 1.0);
}

#endif //////////////////////////////////////////
#ifdef FRAG /////////////////////////////////////

#include "include/header.glsl"
#include "include/global_uniforms.glsl"
#include "include/aabb_tree.glsl"

layout(location = 0) out vec4 frag_color;

layout(location = 0) in vec2 uv;

//from https://iquilezles.org/articles/intersectors/
vec2 box_intersection( in vec3 ro, in vec3 rd, vec3 boxSize, out vec3 outNormal )
{
    vec3 m = 1.0/rd; // can precompute if traversing a set of aligned boxes
    vec3 n = m*ro;   // can precompute if traversing a set of aligned boxes
    vec3 k = abs(m)*boxSize;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );
    if( tN>tF || tF<0.0) return vec2(-1.0); // no intersection
    outNormal = (tN>0.0) ? step(vec3(tN),t1) : // ro ouside the box
                           step(t2,vec3(tF));  // ro inside the box
    outNormal *= -sign(rd);
    return vec2( tN, tF );
}

bool does_intersect(in vec3 ro, in vec3 rd, vec3 boxSize)
{
    vec3 m = 1.0/rd; // can precompute if traversing a set of aligned boxes
    vec3 n = m*ro;   // can precompute if traversing a set of aligned boxes
    vec3 k = abs(m)*boxSize;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );
    return !( tN>tF || tF<0.0);
}

void main() {
    float screen_dist = 2.0/tan(0.5*fov);
    vec3 p = camera_pos;
    vec3 d = camera_axes*vec3(aspect_ratio*uv.x, uv.y, screen_dist);
    d = normalize(d);
    float t = 10000.0;

    vec3 boxes = vec3(0.0);

#if 1
    int stack[MAX_AABB_NODES];
    stack[0] = 1;
    int n_stack = 1;

    int primitive_ids[1024];
    int n_primitives = 0;

    int iteration = 0;
    while(n_stack > 0) {
        if(++iteration > tree.n_nodes) break;
        int i = stack[--n_stack]-1;
        vec3 bp = 0.5*(tree.nodes[i].aabb.u+tree.nodes[i].aabb.l);
        vec3 r = p-bp;
        vec3 size = 0.5*(tree.nodes[i].aabb.u-tree.nodes[i].aabb.l);
        if(does_intersect(r, d, size)) {
            if(tree.nodes[i].leaf != 0) {
                primitive_ids[n_primitives++] = tree.nodes[i].leaf;
            } else {
                stack[n_stack++] = tree.nodes[i].children.x;
                stack[n_stack++] = tree.nodes[i].children.y;
            }
            boxes += 0.01;
        }
    }
    for(int j = 0; j < n_primitives; j++) {
        int i = primitive_ids[j]-1;
        switch(tree.primitives[i].type) {
            case PRIM_BOXSPHERE: {
                vec3 r = tree.primitives[i].p-p;
                float dist = dot(r, d);
                float bsq = dot(r,r)-sq(dist);
                float rsq = sq(tree.primitives[i].r);
                if(bsq < rsq)
                    t = min(t, dist-sqrt(rsq-bsq));
            } break;
            default: break;
        }
    }

#else

    // for(int i = 0; i < tree.n_nodes; i++) {
    //     vec3 bp = 0.5*(tree.nodes[i].aabb.u+tree.nodes[i].aabb.l);
    //     vec3 r = p-bp;
    //     vec3 size = 0.5*(tree.nodes[i].aabb.u-tree.nodes[i].aabb.l);
    //     vec3 n;
    //     if(box_intersection(r, d, size, n).x > 0.0)
    //         boxes += 0.01*abs(n);
    // }

    for(int i = 0; i < tree.n_primitives; i++) {
        switch(tree.primitives[i].type) {
            case PRIM_BOXSPHERE: {
                vec3 r = tree.primitives[i].p-p;
                float dist = dot(r, d);
                float bsq = dot(r,r)-sq(dist);
                float rsq = sq(tree.primitives[i].r);
                if(bsq < rsq)
                    t = min(t, dist-sqrt(rsq-bsq));
            } break;
            default: break;
        }
    }
#endif

    frag_color.rgb = vec3(1.0/max(t, 1.0))+boxes;
    frag_color.a = 1.0;
}

#endif //////////////////////////////////////////
