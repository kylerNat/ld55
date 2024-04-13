#ifndef AABB
#define AABB

#include "maths.glsl"

struct aabb_t {
    vec3 l;
    vec3 u;
};

#define PRIM_NONE    0
#define PRIM_SPHERE  1
#define PRIM_CAPSULE 2
#define PRIM_DISC    3
#define PRIM_BOX     4

struct primitive_t {
    aabb_t aabb;
    int node;
    vec3 p;
    float r;
    vec3 size;
    uint type;
    vec4 orientation;
    vec3 v;
    vec3 omega;

    //TODO: probably just want to have material id with a separate material table
    vec3 albedo;
    vec3 emission;
};

#ifndef CUSTOM_SSBO
layout(std140, binding = 2) readonly buffer ssbo {
    int n_primitives;
    primitive_t primitives[1024];
};

///////////////raycast code///////////////////

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

// axis aligned box centered at the origin, with dimensions "size" and extruded by "rad"
float roundedbox_intersect( in vec3 ro, in vec3 rd, in vec3 size, in float rad )
{
    // bounding box
    vec3 m = 1.0/rd;
    vec3 n = m*ro;
    vec3 k = abs(m)*(size+rad);
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );
    if( tN>tF || tF<0.0) return -1.0;
    float t = tN;

    // convert to first octant
    vec3 pos = ro+t*rd;
    vec3 s = sign(pos);
    ro  *= s;
    rd  *= s;
    pos *= s;

    // faces
    pos -= size;
    pos = max( pos.xyz, pos.yzx );
    if( min(min(pos.x,pos.y),pos.z) < 0.0 ) return t;

    // some precomputation
    vec3 oc = ro - size;
    vec3 dd = rd*rd;
    vec3 oo = oc*oc;
    vec3 od = oc*rd;
    float ra2 = rad*rad;

    t = 1e20;

    // corner
    {
    float b = od.x + od.y + od.z;
    float c = oo.x + oo.y + oo.z - ra2;
    float h = b*b - c;
    if( h>0.0 ) t = -b-sqrt(h);
    }
    // edge X
    {
    float a = dd.y + dd.z;
    float b = od.y + od.z;
    float c = oo.y + oo.z - ra2;
    float h = b*b - a*c;
    if( h>0.0 )
    {
        h = (-b-sqrt(h))/a;
        if( h>0.0 && h<t && abs(ro.x+rd.x*h)<size.x ) t = h;
    }
    }
    // edge Y
    {
    float a = dd.z + dd.x;
    float b = od.z + od.x;
    float c = oo.z + oo.x - ra2;
    float h = b*b - a*c;
    if( h>0.0 )
    {
        h = (-b-sqrt(h))/a;
        if( h>0.0 && h<t && abs(ro.y+rd.y*h)<size.y ) t = h;
    }
    }
    // edge Z
    {
    float a = dd.x + dd.y;
    float b = od.x + od.y;
    float c = oo.x + oo.y - ra2;
    float h = b*b - a*c;
    if( h>0.0 )
    {
        h = (-b-sqrt(h))/a;
        if( h>0.0 && h<t && abs(ro.z+rd.z*h)<size.z ) t = h;
    }
    }

    if( t>1e19 ) t=-1.0;

    return t;
}

// normal of a rounded box
vec3 roundedbox_normal( in vec3 pos, in vec3 siz, in float rad )
{
    return sign(pos)*normalize(max(abs(pos)-siz,0.0));
}

bool does_intersect_precomp(vec3 m, vec3 r, vec3 boxSize, float max_t)
{
    vec3 n = m*r;
    vec3 k = abs(m)*boxSize;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );
    return !( tN>tF || tF<0.0 || tF<-max_t);
}

bool does_intersect(in vec3 ro, in vec3 rd, vec3 boxSize, float max_t)
{
    vec3 m = 1.0/rd; // can precompute if traversing a set of aligned boxes
    vec3 n = m*ro;   // can precompute if traversing a set of aligned boxes
    vec3 k = abs(m)*boxSize;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );
    return !( tN>tF || tF<0.0 || tF<-max_t);
}

//end of stolen iq code

bool raycast_primitive(int i, vec3 p, vec3 d, inout float hit_t, inout vec3 hit_pos, inout vec3 normal) {
        switch(primitives[i].type) {
            case PRIM_SPHERE: {
                vec3 r = primitives[i].p-p;
                float dist = dot(r, d);
                float bsq = dot(r,r)-sq(dist);
                float rsq = sq(primitives[i].r);
                if(dot(r,r) < rsq) {
                    hit_t = 0.0;
                    hit_pos = p;
                    normal = vec3(0.,0.,1.);
                    return true;
                }
                if(bsq < rsq) {
                    float t = dist-sqrt(rsq-bsq);
                    if(t > 0.0 && t < hit_t) {
                        hit_t = t;
                        hit_pos = p+t*d;
                        normal = normalize(hit_pos-primitives[i].p);
                        return true;
                    }
                }
            } break;
            case PRIM_CAPSULE: {
                vec3 r = primitives[i].p-p;
                float h = primitives[i].size.z;
                r = apply_rotation(conjugate(primitives[i].orientation), r);
                vec3 raw_d = d;
                d = apply_rotation(conjugate(primitives[i].orientation), d);
                d = normalize(d);

                float hh = h*h;
                float R = primitives[i].r;
                float ddxy = 1.0-d.z*d.z;//dot(d.xy, d.xy);
                //d.xy is not normalized so this has a factor of 1/|d.xy|
                float dist = dot(r.xy, d.xy);
                float rsq = sq(R);
                float l = sq(dist)+(rsq-dot(r.xy, r.xy))*ddxy;
                if(dist > 0.0 && l < 0.0) return false;
                float t = (dist-sqrt(l))/ddxy;
                if(t < hit_t) {
                    float z = t*d.z-r.z;
                    if(abs(z) < h) {
                        vec3 c = vec3(0.0, 0.0, z);
                        hit_t = t;
                        hit_pos = p+t*raw_d;
                        normal = normalize(hit_pos-primitives[i].p
                                           -apply_rotation(primitives[i].orientation, c));
                        return true;
                    } else {
                        vec3 c = vec3(0.0, 0.0, h*sign(z));
                        r.z += c.z;
                        dist = dot(r, d);
                        float bsq = dot(r, r)-sq(dist);
                        if(bsq < rsq) {
                            float t = dist-sqrt(rsq-bsq);
                            if(t > 0.0 && t < hit_t) {
                                hit_t = t;
                                hit_pos = p+t*raw_d;
                                normal = normalize(hit_pos-primitives[i].p
                                                   -apply_rotation(primitives[i].orientation, c));
                                return true;
                            }
                        }
                    }
                }
            } break;
            case PRIM_DISC: {
                vec3 r = p-primitives[i].p;
                float R = primitives[i].size.x;
                r = apply_rotation(conjugate(primitives[i].orientation), r);
                vec3 raw_d = d;
                d = apply_rotation(conjugate(primitives[i].orientation), d);
                //fuck it we raymarch
                float m;
                float t = 0.0;
                vec3 n;
                vec2 q;
                for(int i = 0;; i++) {
                    if(i >= 64) return false;
                    q = r.xy;
                    if(dot(q,q) > sq(R)) q = R*normalize(q);
                    n = r;
                    n.xy -= q;
                    m = length(n)-primitives[i].r;
                    if(m <= 0.01) break;
                    r += m*d;
                    t += m;
                }
                n = normalize(n);
                hit_t = t;
                hit_pos = p+t.x*raw_d;
                normal = apply_rotation(primitives[i].orientation, n);
                return true;
            } break;
            case PRIM_BOX: {
                vec3 r = p-primitives[i].p;
                r = apply_rotation(conjugate(primitives[i].orientation), r);
                vec3 rot_d = apply_rotation(conjugate(primitives[i].orientation), d);
                vec3 n;
                vec2 t;
                if(primitives[i].r <= 0.0)
                    t = box_intersection(r, rot_d, primitives[i].size, n);
                else {
                    t.x = roundedbox_intersect(r, rot_d, primitives[i].size, primitives[i].r);
                    t.y = -1.0;
                    n = roundedbox_normal(r, rot_d, primitives[i].r);
                }
                if(t.x < 0.0 && t.y > 0) {
                    hit_t = 0.0;
                    hit_pos = p;
                    normal = vec3(0.,0.,1.);
                    return true;
                }
                if(t.x > 0.0 && t.x < hit_t) {
                    hit_t = t.x;
                    hit_pos = p+t.x*d;
                    normal = apply_rotation(primitives[i].orientation, n);
                    return true;
                }
            } break;
            default: break;
        }
        return false;
}

float shadow_primitive(int i, vec3 p, vec3 d, inout float hit_t, in float k) {
        switch(primitives[i].type) {
            case PRIM_SPHERE: {
                vec3 r = primitives[i].p-p;
                float dist = dot(r, d);
                if(dist > 0.0 && dist < hit_t) {
                    float bsq = dot(r,r)-sq(dist);
                    if(bsq < sq(primitives[i].r)) return 0.0;
                    return k*(sqrt(max(bsq, 0.0)) - primitives[i].r)/dist;
                }
            } break;
            case PRIM_BOX: {
                vec3 r = p-primitives[i].p;
                r = apply_rotation(conjugate(primitives[i].orientation), r);
                vec3 rot_d = apply_rotation(conjugate(primitives[i].orientation), d);
                vec3 n;
                vec2 t = box_intersection(r, rot_d, primitives[i].size, n);
                if(t.x > 0.0 && t.x < hit_t) {
                    hit_t = t.x;
                    return 0.0;
                }
            } break;
            default: break;
        }
        return 1.0;
}


bool is_inside(vec3 p, vec3 size)
{
    return all(lessThan(abs(p), size));
}

bool is_inside(vec3 p, aabb_t box)
{
    return all(lessThan(box.l, p)) && all(lessThan(p, box.u));
}

vec3 aabb_center(aabb_t box) {
    return 0.5*(box.u+box.l);
}
vec3 aabb_size(aabb_t box) {
    return 0.5*(box.u-box.l);
}

bool cast_ray(vec3 p, vec3 d, float max_t, int ignore_primitive, out float hit_t, out vec3 hit_pos, out vec3 normal, out int hit_index)
{
    hit_t = max_t;

    bool did_hit = false;

    for(int i = 0; i < n_primitives; i++) {
        if(i != ignore_primitive) {
            bool hit_primitive = raycast_primitive(i, p, d, hit_t, hit_pos, normal);
            if(hit_primitive) hit_index = i;
            did_hit = hit_primitive || did_hit;
        }
    }
    return did_hit;
}

float shadow(vec3 p, vec3 d, float max_t, int ignore_primitive, out float hit_t, out vec3 hit_pos, out vec3 normal)
{
    hit_t = max_t;

    float sh = 1.0;

    for(int i = 0; i < n_primitives; i++) {
        if(i != ignore_primitive) {
            sh = min(sh, shadow_primitive(i, p, d, hit_t, 16.0));
            if(sh <= 0.0) break;
        }
    }
    return sh;
}

#endif

#endif //AABB
