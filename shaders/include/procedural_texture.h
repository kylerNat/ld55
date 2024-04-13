#define halfsqrt3 0.86602540
#define invsqrt3 0.57735026
#define sqrt2 1.41421356237309504880
#define tau 6.28318530
#define pi 3.14159265358979323846264338327950

float hash1D(vec2 x)
{
    // hash by Inigo Quilez, Integer Hash - III, 2017
    uvec2 q = uvec2(x * 65536.0);
    q = 1103515245u * ((q >> 1u) ^ q.yx);
    uint n = 1103515245u * (q.x ^ (q.y >> 3u));
    return float(n) * (1.0 / float(0xffffffffu));
}

vec2 hash2D(vec2 x)
{
    // based on: Inigo Quilez, Integer Hash - III, 2017
    uvec4 q = uvec2(x * 65536.0).xyyx + uvec2(0u, 3115245u).xxyy;
    q = 1103515245u * ((q >> 1u) ^ q.yxwz);
    uvec2 n = 1103515245u * (q.xz ^ (q.yw >> 3u));
    return vec2(n) * (1.0 / float(0xffffffffu));
}

float hash(vec2 x, float phase)
{
    return 0.5+0.5*sin(tau*hash1D(abs(x))+phase);
}

vec2 hash2d(vec2 x, float phase)
{
    return 0.5+0.5*sin(tau*hash2D(abs(x))+phase);
}

float noise1d(float x, float phase)
{
    float ix = floor(x);
    float fx = x-ix;
    float h0 = hash(vec2(ix,0.0), phase);
    float h1 = hash(vec2(ix+1.0,0.0), phase);
    return h0 + (h1-h0)*fx*fx*(3.0-2.0*fx);
}

vec2 noised1d(float x, float phase)
{
    float ix = floor(x);
    float fx = x-ix;
    float h0 = hash(vec2(ix,0.0), phase);
    float h1 = hash(vec2(ix+1.0,0.0), phase);
    return vec2(h0 + (h1-h0)*fx*fx*(3.0-2.0*fx), h0 + 6.0*(h1-h0)*fx*(1.0-fx));
}

vec2 fbmd1d(float p, float time, int octaves, float decay, float time_factor)
{
    vec3 v = vec3(0.0);
    float weight = 1.0;
    for(int i = 0; i < octaves; i++)
    {
        //time = mod(time, tau);
        v += weight*vec3(noised1d(p, time), 1.0);
        p *= 2.0;
        weight *= decay;
        time *= time_factor;
    }
    return v.xy/v.z;
}


//value noise on a triangular lattice
float tri_noise(vec2 p, float phase)
{
    vec2 q = vec2(p.x-p.y*invsqrt3, p.y*2.0*invsqrt3);
    vec2 iq = floor(q);
    vec2 fq = fract(q);
    float v = 0.0;

    float h = step(1.0, fq.x+fq.y); //which half of the unit cell does this triangle lie in
    vec2 c = iq+h;
    vec2 r = p-vec2(c.x+0.5*c.y, halfsqrt3*c.y);
    r *= 1.0-2.0*h;

    //compute barrycentric coordinates
    float lambda2 = 2.0*invsqrt3*r.y;
    float lambda0 = 1.0-r.x-invsqrt3*r.y;
    float lambda1 = 1.0-lambda2-lambda0;

    v += lambda0*hash(abs(c), phase);
    v += lambda1*hash(abs(iq+vec2(1.0-h,h)), phase);
    v += lambda2*hash(abs(iq+vec2(h,1.0-h)), phase);
    return v;
}

float fbm(vec2 p, float time, int octaves, float decay, float time_factor)
{
    vec2 v = vec2(0.0);
    float weight = 1.0;
    for(int i = 0; i < octaves; i++)
    {
        //time = mod(time, tau);
        v += weight*vec2(tri_noise(p, time), 1.0);
        p *= 2.0*mat2(4.0/5.0, -3.0/5.0, 3.0/5.0, 4.0/5.0);
        weight *= decay;
        time *= time_factor;
    }
    return v.x/v.y;
}

float electric_fbm(vec2 p, float time, int octaves, float decay, float time_factor)
{
    vec2 v = vec2(0.0);
    float weight = 1.0;
    for(int i = 0; i < octaves; i++)
    {
        //time = mod(time, tau);
        v += weight*vec2(tri_noise(p, floor(time*4.0)*0.25), 1.0);
        p *= 2.0*mat2(4.0/5.0, -3.0/5.0, 3.0/5.0, 4.0/5.0);
        weight *= decay;
        time *= time_factor;
    }
    return v.x/v.y;
}

//value noise on a triangular lattice, with gradients
vec3 tri_noised(vec2 p, float time)
{
    vec2 q = vec2(p.x-p.y*invsqrt3, p.y*2.0*invsqrt3);
    vec2 iq = floor(q);
    vec2 fq = fract(q);
    vec3 v = vec3(0.0);

    float h = step(1.0, fq.x+fq.y); //which half of the unit cell does this triangle lie in
    vec2 c = iq+h;
    vec2 r = p-vec2(c.x+0.5*c.y, halfsqrt3*c.y);
    float s = 1.0-2.0*h;
    r *= s;

    //compute barrycentric coordinates
    vec3 lambda = vec3(1.0-r.x-invsqrt3*r.y, r.x-invsqrt3*r.y, 2.0*invsqrt3*r.y);
    vec3 dlambda_dx = vec3(-1.0,1.0,0.0)*s;
    vec3 dlambda_dy = vec3(-invsqrt3,-invsqrt3,2.0*invsqrt3)*s;

    //smooth interpolation based on https://www.sciencedirect.com/science/article/pii/0021904573900208
    // with smoothsteps along the edges and normal derivatives set to 0

    //quintic////////////////////
    vec3 lambda2 = lambda*lambda;
    vec3 a = 15.0*lambda2*lambda2.zxy*lambda.yzx;

    vec3 da_dlambda_xyz = 30.0*lambda*lambda2.zxy*lambda.yzx;
    // vec3 da_dlambda_zxy = 30.0*lambda2*lambda.zxy*lambda.yzx;
    vec3 da_dlambda_zxy = da_dlambda_xyz.yzx;
    vec3 da_dlambda_yzx = 15.0*lambda2*lambda2.zxy;
    vec3 da_dx = da_dlambda_xyz*dlambda_dx + da_dlambda_zxy*dlambda_dx.zxy + da_dlambda_yzx*dlambda_dx.yzx;
    vec3 da_dy = da_dlambda_xyz*dlambda_dy + da_dlambda_zxy*dlambda_dy.zxy + da_dlambda_yzx*dlambda_dy.yzx;

    //weights set to be quintic smoothstep along edges, with extra terms to set gradients in the normal direction to 0
    //these magically add up to 1 without correction
    vec3 w = lambda*lambda2*(10.0-15.0*lambda+6.0*lambda2)+a+a.yzx;

    //derivatives
    vec3 dw_dx = 30.0*dlambda_dx*lambda2*(1.0-lambda)*(1.0-lambda) + da_dx+da_dx.yzx;
    vec3 dw_dy = 30.0*dlambda_dy*lambda2*(1.0-lambda)*(1.0-lambda) + da_dy+da_dy.yzx;

    v += vec3(w.x, dw_dx.x, dw_dy.x)*hash(c, time);
    v += vec3(w.y, dw_dx.y, dw_dy.y)*hash(iq+vec2(1.0-h,h), time);
    v += vec3(w.z, dw_dx.z, dw_dy.z)*hash(iq+vec2(h,1.0-h), time);

    return v;
}

vec3 fbmd(vec2 p, float time, int octaves, float decay, float time_factor)
{
    vec4 v = vec4(0.0);
    float weight = 1.0;
    for(int i = 0; i < octaves; i++)
    {
        //time = mod(time, tau);
        v += weight*vec4(tri_noised(p, time), 1.0);
        p *= 2.0*mat2(4.0/5.0, -3.0/5.0, 3.0/5.0, 4.0/5.0);
        weight *= decay;
        time *= time_factor;
    }
    return v.xyz/v.w;
}

//value noise on a triangular lattice
vec2 tri_noise2d(vec2 p, float phase)
{
    vec2 q = vec2(p.x-p.y*invsqrt3, p.y*2.0*invsqrt3);
    vec2 iq = floor(q);
    vec2 fq = fract(q);
    vec2 v = vec2(0.0);

    float h = step(1.0, fq.x+fq.y); //which half of the unit cell does this triangle lie in
    vec2 c = iq+h;
    vec2 r = p-vec2(c.x+0.5*c.y, halfsqrt3*c.y);
    r *= 1.0-2.0*h;

    //compute barrycentric coordinates
    float lambda2 = 2.0*invsqrt3*r.y;
    float lambda0 = 1.0-r.x-invsqrt3*r.y;
    float lambda1 = 1.0-lambda2-lambda0;

    v += lambda0*hash2d(abs(c), phase);
    v += lambda1*hash2d(abs(iq+vec2(1.0-h,h)), phase);
    v += lambda2*hash2d(abs(iq+vec2(h,1.0-h)), phase);
    return v;
}

vec2 fbm2d(vec2 p, float time, int octaves, float decay, float time_factor)
{
    vec3 v = vec3(0.0);
    float weight = 1.0;
    for(int i = 0; i < octaves; i++)
    {
        //time = mod(time, tau);
        v += weight*vec3(tri_noise2d(p, time), 1.0);
        p *= 2.0*mat2(4.0/5.0, -3.0/5.0, 3.0/5.0, 4.0/5.0);
        weight *= decay;
        time *= time_factor;
    }
    return v.xy/v.z;
}

vec3 dot_params(vec2 tx, float i, float time)
{
    return vec3(hash(tx+vec2(42.0*i, 69.0*i), time), hash(tx+vec2(55.0+98.0*i, 12.0+12.0*i), time), 0.25*hash(tx+vec2(93.0+92.0*i, 74.0+23.0*i), time));
    // return vec3(0.0, 0.0, 0.1);
}

vec3 single_dot(vec2 x, vec2 tx, float i, float time)
{
    vec3 params = dot_params(tx, i, time);
    vec2 c = params.xy;
    float radius = params.z;

    vec2 r = x-c;
    return vec3(smoothstep(-sq(radius), -sq(radius)+0.01, -dot(r,r)));
}

//TODO: should be on a triangular grid
vec3 dots(vec2 x, float time)
{
    x *= 8.0;
    vec3 color = vec3(0.0);
    vec2 fx = fract(x);
    vec2 tx = floor(x);
    vec2 d = vec2(0.0);
    //TODO: I think I can size things so I only need to check 4 cells instead of 9
    for(d.y = -1.0; d.y <= 1.0; d.y += 1.0)
        for(d.x = -1.0; d.x <= 1.0; d.x += 1.0)
        {
            color += single_dot(fx-d, tx+d, 0, time);
        }
    return color;
}

float voronoi(vec2 x, float time)
{
    x *= 8.0;
    vec2 fx = fract(x);
    vec2 tx = floor(x);
    vec2 d = vec2(0.0);
    float color = 0.0;
    float distsq = 10000;
    for(d.y = -1.0; d.y <= 1.0; d.y += 1.0)
        for(d.x = -1.0; d.x <= 1.0; d.x += 1.0)
        {
            vec3 params = dot_params(tx+d, 0, time);
            vec2 c = params.xy;
            float radius = params.z;
            vec2 r = fx-d-c;
            float dsq = dot(r, r);
            if(dsq < distsq)
            {
                color = 4.0*radius;
                distsq = dsq;
            }
        }
    return color;
}

vec3 smin(vec3 a, vec3 b, float k)
{
    float h = max(k-abs(a.x-b.x),0.0);
    float m = 0.25*h*h/k;
    float n = 0.50*  h/k;
    return vec3(min(a.x,  b.x) - m,
                mix(a.yz,b.yz,(a.x<b.x)?n:1.0-n));
}

vec3 smax(vec3 a, vec3 b, float k)
{
    float h = max(k-abs(a.x-b.x),0.0);
    float m = 0.25*h*h/k;
    float n = 0.50*  h/k;
    return vec3(max(a.x,  b.x) + m,
                mix(a.yz,b.yz,(a.x>b.x)?n:1.0-n));
}

float smin(float a, float b, float k)
{
    float h = max(k-abs(a-b),0.0);
    float m = 0.25*h*h/k;
    float n = 0.50*  h/k;
    return min(a.x,  b.x) - m;
}

float smax(float a, float b, float k)
{
    float h = max(k-abs(a-b),0.0);
    float m = 0.25*h*h/k;
    float n = 0.50*  h/k;
    return max(a.x,  b.x) + m;
}
