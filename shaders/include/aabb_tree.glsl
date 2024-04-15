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

#include "texture_types.h"

float noise(vec2 p, float t)
{
    vec2 i = floor(p);
    vec2 f = p-i;
    float v = 0.0;
    vec2 s = f*f*(3.0-2.0*f);
    vec2 e = vec2(0.0, 1.0);
    v = mix(mix(hash(i+e.xx, t), hash(i+e.yx, t), s.x),
            mix(hash(i+e.xy, t), hash(i+e.yy, t), s.x), s.y);
    return v;
}

float fbm(vec2 p, float t)
{
    float v = 0.0;
    float s = 0.5;
    mat2 scale = 2.0*mat2(0.8,-0.6,0.6,0.8);
    for(int i = 0; i < 5; i++) {
        v += s*noise(p, t);
        s *= 0.5;
        p *= scale;
    }
    return v;
}

vec3 do_proc_texture(vec2 p, int texture_type) {
    switch(texture_type) {
        default:
        case TEX_PLAIN: {
            return vec3(1.0);
        } break;
        case TEX_FIREBALL: {
            return vec3(1.0, fbm(10.0*p, 10.0*time), 1.0);
        } break;
    }
}

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
    int texture_type;
};

#ifndef CUSTOM_SSBO
layout(std140, binding = 2) readonly buffer ssbo {
    int n_primitives;
    primitive_t primitives[65536];
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

vec3 get_floor_albedo(vec2 p)
{
    return vec3(0.0);
}

vec3 get_floor_emission(vec2 p)
{
    vec3 color = vec3(0.6+0.2*sin(10.0*time),0.0,0.0);

    float s = 10000.0;
    float r = length(p);
    float R = 12.0;
    if(r > R+5.5) return vec3(0.0);
    float a = R*cos(0.4*pi);
    float b = a/cos(0.2*pi);
    float T = 0.25;
    s = min(s, abs(r-R-0.8)-T);
    s = min(s, abs(r-R+0.8)-T);
    // s = min(s, abs(r-0.5*a)-T);
    for(float i = 0.0; i < 4.5; i++) {
        float theta = i*pi*0.4;
        vec2 n = vec2(cos(theta), sin(theta));
        float l = dot(n, p);
        s = min(s, max(abs(l-a)-T, r-R));
    }
    {
        float i = round(atan(p.y, p.x)/(0.4*pi));
        if(i < 0.0) i += 5.0;
        float theta = i*pi*0.4;
        vec2 c = R*vec2(cos(theta), sin(theta));

        float t = abs(length(p-c)-3.0)-T;
        if(t < 0.0) {
            if(i == float(selected)) color = mix(color, vec3(0.8), 0.5+0.5*sin(5.0*time));
            if(i == float(hovered)) color = mix(color, vec3(1.0), 0.5);
            s = min(s, t);
        }
        s = max(s, 3.0-T-length(p-c));
        // if(i == 0.0)
        //     s = min(s, length(p-c)-T);
    }
    {
        int i = 6;
        vec2 c = vec2(0.0);
        float t = abs(length(p-c)-4.5)-T;
        if(t < 0.0) {
            if(i == selected) color = mix(color, vec3(0.8), 0.5+0.5*sin(5.0*time));
            if(i == hovered)  color = mix(color, vec3(1.0), 0.5);
            s = min(s, t);
        }
        s = max(s, 4.5-T-length(p-c));
    }
    return color*step(0.0, -s);
}

bool raycast_floor(vec3 p, vec3 d, inout float hit_t, inout vec3 hit_pos, inout vec3 normal, inout vec3 albedo, out vec3 emission)
{
    float t = -p.z/d.z;
    if(t > 0.0 && t < hit_t) {
        hit_t = t;
        hit_pos = p+t*d;
        normal = vec3(0.0,0.0,1.0);
        albedo = get_floor_albedo(hit_pos.xy);
        emission = get_floor_emission(hit_pos.xy);
        return true;
    }
    return false;
}

bool cast_ray(vec3 p, vec3 d, float max_t, int ignore_primitive, out float hit_t, out vec3 hit_pos, out vec3 normal, inout vec3 albedo, inout vec3 emission)
{
    hit_t = max_t;

    bool did_hit = false;

    did_hit = raycast_floor(p, d, hit_t, hit_pos, normal, albedo, emission);

    for(int i = 0; i < n_primitives; i++) {
        if(i != ignore_primitive) {
            bool hit_primitive = raycast_primitive(i, p, d, hit_t, hit_pos, normal);
            if(hit_primitive) {
                albedo = primitives[i].albedo;
                emission = primitives[i].emission;
            }
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
