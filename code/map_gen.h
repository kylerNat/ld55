#ifndef MAP_GEN
#define MAP_GEN

#define halfsqrt3 0.86602540
#define invsqrt3 0.57735026
#define tau 6.28318530

#include "misc.h"

float hash1D(real_2 x)
{
    // hash by Inigo Quilez, Integer Hash - III, 2017
    uint_2 q = uint_cast(x * 65536.0f);
    q = {1103515245u * ((q.x >> 1u) ^ q.y), 1103515245u * ((q.y >> 1u) ^ q.x)};
    uint n = 1103515245u * (q.x ^ (q.y >> 3u));
    return float(n) * (1.0f / float(0xffffffffu));
}

float hash(real_2 x, float phase)
{
    return 0.5+0.5*sin(tau*hash1D(abs_per_axis(x))+phase);
}

real_2 hash2D(real_2 x)
{
    // based on: Inigo Quilez, Integer Hash - III, 2017
    uint_2 q2 = uint_cast(x * 65536.0f);
    uint_4 q = {q2.x, q2.y, q2.y + 3115245u, q2.x + 3115245u};
    q = {
        1103515245u * ((q.x >> 1u) ^ q.y),
        1103515245u * ((q.y >> 1u) ^ q.x),
        1103515245u * ((q.z >> 1u) ^ q.w),
        1103515245u * ((q.w >> 1u) ^ q.z),
    };
    uint_2 n = {
        1103515245u * (q.x ^ (q.y >> 3u)),
        1103515245u * (q.z ^ (q.w >> 3u)),
    };
    return real_cast(n) * (1.0f / float(0xffffffffu));
}

real_2 hash2d(real_2 x, float phase)
{
    return (real_2){0.5, 0.5}+0.5*sin_per_axis(tau*hash2D(abs_per_axis(x))+(real_2){phase, phase});
}


//value noise on a triangular lattice
real_3 tri_noise(real_2 p, float phase)
{
    real_2 q = {p.x-p.y*invsqrt3, p.y*2.0f*invsqrt3};
    real_2 iq = floor_per_axis(q);
    real_2 fq = q-iq;
    real_3 v = {};

    float h = fq.x+fq.y > 1.0f ? 1.0f : 0.0f; //which half of the unit cell does this triangle lie in
    real_2 c = iq+(real_2){h,h};
    real_2 r = p-(real_2){c.x+0.5*c.y, halfsqrt3*c.y};
    r *= 1.0f-2.0f*h;

    //compute barycentric coordinates
    real_3 lambda2 = {2.0f*invsqrt3*r.y, 0.0f, 2.0f*invsqrt3};
    real_3 lambda0 = {1.0f-r.x-invsqrt3*r.y, -1.0f, -invsqrt3};
    real_3 lambda1 = (real_3){1.0f,0.0f,0.0f}-lambda2-lambda0;

    v += lambda0*hash(c, phase);
    v += lambda1*hash(iq+(real_2){1.0f-h,h}, phase);
    v += lambda2*hash(iq+(real_2){h,1.0f-h}, phase);
    return v;
}

real_3 fbm(real_2 p, float time, int octaves, float decay, float time_factor)
{
    real_4 v = {};
    float weight = 1.0f;
    for(int i = 0; i < octaves; i++)
    {
        //time = mod(time, tau);
        real_3 n = tri_noise(p, time);
        v += weight*(real_4){n.x, n.y, n.z, 1.0f};
        p = p*(2.0f*(real_2x2){4.0f/5.0f, -3.0f/5.0f, 3.0f/5.0f, 4.0f/5.0f});
        weight *= decay;
        time *= time_factor;
    }
    return v.xyz/v.w;
}

//value noise on a triangular lattice, with gradients
real_3 tri_noised(real_2 p, float time)
{
    real_2 q = (real_2){p.x-p.y*invsqrt3, p.y*2.0f*invsqrt3};
    real_2 iq = floor_per_axis(q);
    real_2 fq = q-iq;
    real_3 v = (real_3){};

    float h = fq.x+fq.y>=1.0f?1.0f:0.0f; //which half of the unit cell does this triangle lie in
    real_2 c = iq+(real_2){h,h};
    real_2 r = p-(real_2){c.x+0.5*c.y, halfsqrt3*c.y};
    float s = 1.0f-2.0f*h;
    r *= s;

    //compute barrycentric coordinates
    real_3 lambda = (real_3){1.0f-r.x-invsqrt3*r.y, r.x-invsqrt3*r.y, 2.0f*invsqrt3*r.y};
    real_3 dlambda_dx = (real_3){-1.0f,1.0f,0.0f}*s;
    real_3 dlambda_dy = (real_3){-invsqrt3,-invsqrt3,2.0f*invsqrt3}*s;

    //quintic////////////////////
    real_3 lambda2 = lambda*lambda;
    real_3 a = 15.0f*lambda2*lambda2.zxy()*lambda.yzx();

    real_3 da_dlambda_xyz = 30.0f*lambda*lambda2.zxy()*lambda.yzx();
    real_3 da_dlambda_zxy = 30.0f*lambda2*lambda.zxy()*lambda.yzx();
    real_3 da_dlambda_yzx = 15.0f*lambda2*lambda2.zxy();
    real_3 da_dx = da_dlambda_xyz*dlambda_dx + da_dlambda_zxy*dlambda_dx.zxy() + da_dlambda_yzx*dlambda_dx.yzx();
    real_3 da_dy = da_dlambda_xyz*dlambda_dy + da_dlambda_zxy*dlambda_dy.zxy() + da_dlambda_yzx*dlambda_dy.yzx();

    //weights set to be quintic smoothstep along edges, with extra terms to set gradients in the normal direction to 0
    //these magically add up to 1 without correction
    real_3 w = lambda*lambda2*((real_3){10.0f,10.0f,10.0f}-15.0f*lambda+6.0f*lambda2)+a+a.yzx();

    //derivatives
    real_3 dw_dx = 30.0f*dlambda_dx*lambda2*((real_3){1,1,1}-lambda)*((real_3){1,1,1}-lambda) + da_dx+da_dx.yzx();
    real_3 dw_dy = 30.0f*dlambda_dy*lambda2*((real_3){1,1,1}-lambda)*((real_3){1,1,1}-lambda) + da_dy+da_dy.yzx();

    v += (real_3){w.x, dw_dx.x, dw_dy.x}*hash(c, time);
    v += (real_3){w.y, dw_dx.y, dw_dy.y}*hash(iq+(real_2){1.0f-h,h}, time);
    v += (real_3){w.z, dw_dx.z, dw_dy.z}*hash(iq+(real_2){h,1.0f-h}, time);

    return v;
}

real_3 fbmd(real_2 p, float time, int octaves, float decay, float time_factor)
{
    real_4 v = (real_4){0.0f};
    float weight = 1.0f;
    for(int i = 0; i < octaves; i++)
    {
        //time = mod(time, tau);
        real_3 n = tri_noised(p, time);
        v += weight*(real_4){n.x, n.y, n.z, 1.0f};
        p = p*(2.0f*(real_2x2){4.0f/5.0f, -3.0f/5.0f, 3.0f/5.0f, 4.0f/5.0f});
        weight *= decay;
        time *= time_factor;
    }
    return v.xyz/v.w;
}

struct wall_t
{
    real dist;
    real_2 gradient;
    real_2 flow;
};

wall_t operator*(wall_t a, real b)
{
    return {a.dist*b, a.gradient*b, a.flow};
}

wall_t operator*(real b, wall_t a)
{
    return {a.dist*b, a.gradient*b, a.flow};
}

wall_t operator*=(wall_t& a, real b)
{
    return a = a*b;
}

wall_t smin(wall_t a, wall_t b, real k)
{
    real h = max(k-abs(a.dist-b.dist), 0.0);
    real m = 0.25*h*h/k;
    real n = 0.50*  h/k;
    return {
        min(a.dist,  b.dist) - m,
        lerp(a.gradient,b.gradient,(a.dist<b.dist)?n:1.0-n),
        a.dist < b.dist ? a.flow : b.flow,
    };
}

wall_t smax(wall_t a, wall_t b, real k)
{
    real h = max(k-abs(a.dist-b.dist), 0.0);
    real m = 0.25*h*h/k;
    real n = 0.50*  h/k;
    return {
        max(a.dist,  b.dist) + m,
        lerp(a.gradient,b.gradient,(a.dist>b.dist)?n:1.0-n),
        a.dist > b.dist ? a.flow : b.flow,
    };
}

const real map_scale = 0.005f;
const real border_wall_value = -4.0f;

struct biome_core
{
    int biome;
    int_2 p;
    int size;

    int neighbors[16];
    int_2 neighbor_points[16];
    int n_neighbors;
};

struct map_t
{
    uint32 seed;
    bounding_box_2 map_range;

    biome_core* cores;
    int n_cores;

    real* wall_values;
    real_2* flow;
    int* biomes;
    real* light;
};

int get_biome(map_t* m, real_2 x)
{
    int_2 p = hex_cell(x*map_scale);
    if(is_inside(p, m->map_range))
    {
        return m->biomes[index_2(p-m->map_range.l, dim(m->map_range))];
    }
    return 0;
}

real wall_value(map_t* m, real_2 c)
{
    int_2 ic = int_cast(floor_per_axis(c));
    if(is_inside(ic, m->map_range))
    {
        return m->wall_values[index_2(ic-m->map_range.l, dim(m->map_range))];
    }
    return border_wall_value;
}

real_2 flow_raw(map_t* m, real_2 c)
{
    int_2 ic = int_cast(floor_per_axis(c));
    if(is_inside(ic, m->map_range))
    {
        return m->flow[index_2(ic-m->map_range.l, dim(m->map_range))];
    }
    return {};
}

real light_raw(map_t* m, real_2 c)
{
    int_2 ic = int_cast(floor_per_axis(c));
    if(is_inside(ic, m->map_range))
    {
        return m->light[index_2(ic-m->map_range.l, dim(m->map_range))];
    }
    return 0.0f;
}

real light_value(map_t* m, real_2 p)
{
    p = p*map_scale;
    real_2 q = (real_2){p.x-p.y*invsqrt3, p.y*2.0f*invsqrt3};
    real_2 iq = floor_per_axis(q);
    real_2 fq = q-iq;
    real v = 0.0f;

    float h = fq.x+fq.y>=1.0f?1.0f:0.0f; //which half of the unit cell does this triangle lie in
    real_2 c = iq+(real_2){h,h};
    real_2 r = p-(real_2){c.x+0.5*c.y, halfsqrt3*c.y};
    float s = 1.0f-2.0f*h;
    r *= s;

    //compute barrycentric coordinates
    real_3 lambda = (real_3){1.0f-r.x-invsqrt3*r.y, r.x-invsqrt3*r.y, 2.0f*invsqrt3*r.y};

    //quintic////////////////////
    real_3 lambda2 = lambda*lambda;
    real_3 a = 15.0f*lambda2*lambda2.zxy()*lambda.yzx();

    //weights set to be quintic smoothstep along edges, with extra terms to set gradients in the normal direction to 0
    //these magically add up to 1 without correction
    real_3 w = lambda*lambda2*((real_3){10.0f,10.0f,10.0f}-15.0f*lambda+6.0f*lambda2)+a+a.yzx();

    v += w.x*light_raw(m, c);
    v += w.y*light_raw(m, iq+(real_2){1.0f-h,h});
    v += w.z*light_raw(m, iq+(real_2){h,1.0f-h});

    return v;
}

//wall value with derivatives
wall_t wall_valued(map_t* m, real_2 p)
{
    real_2 q = (real_2){p.x-p.y*invsqrt3, p.y*2.0f*invsqrt3};
    real_2 iq = floor_per_axis(q);
    real_2 fq = q-iq;
    real_3 v = (real_3){};

    float h = fq.x+fq.y>=1.0f?1.0f:0.0f; //which half of the unit cell does this triangle lie in
    real_2 c = iq+(real_2){h,h};
    real_2 r = p-(real_2){c.x+0.5*c.y, halfsqrt3*c.y};
    float s = 1.0f-2.0f*h;
    r *= s;

    //compute barrycentric coordinates
    real_3 lambda = (real_3){1.0f-r.x-invsqrt3*r.y, r.x-invsqrt3*r.y, 2.0f*invsqrt3*r.y};
    real_3 dlambda_dx = (real_3){-1.0f,1.0f,0.0f}*s;
    real_3 dlambda_dy = (real_3){-invsqrt3,-invsqrt3,2.0f*invsqrt3}*s;

    //quintic////////////////////
    real_3 lambda2 = lambda*lambda;
    real_3 a = 15.0f*lambda2*lambda2.zxy()*lambda.yzx();

    real_3 da_dlambda_xyz = 30.0f*lambda*lambda2.zxy()*lambda.yzx();
    real_3 da_dlambda_zxy = 30.0f*lambda2*lambda.zxy()*lambda.yzx();
    real_3 da_dlambda_yzx = 15.0f*lambda2*lambda2.zxy();
    real_3 da_dx = da_dlambda_xyz*dlambda_dx + da_dlambda_zxy*dlambda_dx.zxy() + da_dlambda_yzx*dlambda_dx.yzx();
    real_3 da_dy = da_dlambda_xyz*dlambda_dy + da_dlambda_zxy*dlambda_dy.zxy() + da_dlambda_yzx*dlambda_dy.yzx();

    //weights set to be quintic smoothstep along edges, with extra terms to set gradients in the normal direction to 0
    //these magically add up to 1 without correction
    real_3 w = lambda*lambda2*((real_3){10.0f,10.0f,10.0f}-15.0f*lambda+6.0f*lambda2)+a+a.yzx();

    //derivatives
    real_3 dw_dx = 30.0f*dlambda_dx*lambda2*((real_3){1,1,1}-lambda)*((real_3){1,1,1}-lambda) + da_dx+da_dx.yzx();
    real_3 dw_dy = 30.0f*dlambda_dy*lambda2*((real_3){1,1,1}-lambda)*((real_3){1,1,1}-lambda) + da_dy+da_dy.yzx();

    v += (real_3){w.x, dw_dx.x, dw_dy.x}*wall_value(m, c);
    v += (real_3){w.y, dw_dx.y, dw_dy.y}*wall_value(m, iq+(real_2){1.0f-h,h});
    v += (real_3){w.z, dw_dx.z, dw_dy.z}*wall_value(m, iq+(real_2){h,1.0f-h});

    real_2 f = {};
    f += w.x*flow_raw(m, c);
    f += w.y*flow_raw(m, iq+(real_2){1.0f-h,h});
    f += w.z*flow_raw(m, iq+(real_2){h,1.0f-h});

    return {v.x, v.yz, f};
}

wall_t wall_map(map_t* m, real_2 p)
{
    real_2 x = {};
    real r = 1000.0f;
    real d = 2.0f*r;
    real_2 xm = round_per_axis(p/d)*d;

    wall_t wall = wall_valued(m, map_scale*p);
    wall.gradient *= map_scale;
    real nom = norm(wall.gradient);
    if(nom > 0.0f) wall *= 1.0/nom;

    if(wall.dist < 1.0f)
    {
        real_3 noise = 5.0f*tri_noised(0.1f*p, 0.0f);
        noise = 5.0*smoothstep(1.0, 0.8, wall.dist)*tri_noised(0.1*p, 0.0f);
        noise.yz *= 0.1f;

        wall.dist += noise.x;
        wall.gradient += noise.yz;
    }

    real_2 n = normalize_or_zero(wall.gradient);
    real smoth = smoothstep(5.0, 0.0, wall.dist);
    wall.flow -= (dot(wall.flow, n)*smoth)*n;
    wall.flow *= 1.0f-0.5f*smoth;

    return wall;
}

void draw_circle(map_t* m, real_2 p, real r, int biome)
{
    int_2 range_size = dim(m->map_range);

    // real_2 hex_p = to_hex*p*map_scale;
    int_2 hex_p = hex_cell(p*map_scale);
    real_2 rounded_p = from_hex*real_cast(hex_p)/map_scale;
    int hex_r = ceil(r*(2.0*invsqrt3*map_scale));
    int_2 l = hex_p-(int_2){hex_r, hex_r};
    int_2 u = hex_p+(int_2){hex_r, hex_r};

    for(int y = l.y; y < u.y; y++)
        for(int x = l.x; x < u.x; x++)
        {
            int_2 h = (int_2){x, y};
            real_2 hp = from_hex*real_cast(h)/map_scale;
            if(is_inside(h, m->map_range))
            {
                int index = index_2(h-m->map_range.l, dim(m->map_range));
                real dist = norm(hp-rounded_p);
                real d = r-dist;
                m->wall_values[index] = max(m->wall_values[index], d);
                if(d > 0)
                {
                    m->biomes[index] = biome;
                }
            }
        }
}

typedef void (*biome_fn)(map_t* m, biome_core* bc);

#define N_MAX_BIOME_TYPES 512

int n_biome_types;
struct biome_type
{
    uint32 id;
    real light;
    biome_fn func;
};
biome_type biome_types[N_MAX_BIOME_TYPES];
int n_biomes_types;

int get_biome_index(int biome_id)
{
    for(int i = 0; i < n_biome_types; i++)
        if(biome_types[i].id == biome_id)
        {
            return i;
        }
    union
    {
        char id_string[5];
        int id_int;
    };
    id_int = biome_id;
    id_string[4] = 0;
    log_warning("could not find biome: " , id_string, " (", id_int, ")\n");
    return -1;
}

int get_biome_index(char* biome_id)
{
    return get_biome_index(str_to_id(biome_id));
}

#include "biomes.h"

int add_biome(map_t* m, int biome, int_2 target_p, int_2 dir, int size, int* neighbors, int n_neighbors)
{
    int bi = get_biome_index(biome);

    int current_biome = 0;
    int_2 p = target_p+1000*dir;
    while(p != target_p && current_biome == 0)
    {
        p -= dir;
        if(is_inside(p, m->map_range))
        {
            int index = index_2(p-m->map_range.l, dim(m->map_range));
            current_biome = m->biomes[index];
        }
    }
    p += max(1, (int) pow((real) size, 0.5f))*dir;
    int core_index = m->n_cores++;
    biome_core* bc = m->cores+core_index;
    *bc = {biome, p, size};
    memcpy(bc->neighbors, neighbors, n_neighbors*sizeof(int));
    bc->n_neighbors = n_neighbors;
    for(int i = 0; i < bc->n_neighbors; i++) bc->neighbor_points[i] = p;
    biome_types[bi].func(m, bc);

    for(int i = 0; i < bc->n_neighbors; i++)
    {
        biome_core* nbc = m->cores+bc->neighbors[i];
        p = bc->neighbor_points[i];
        biome_snake(m, bc, 1000, p, 1, nbc->p, true);
    }
    return core_index;
}

void generate_map(map_t* m)
{
    m->seed = 318591829;

    m->map_range = {{-500, -500}, {501, 501}};
    int_2 range_size = dim(m->map_range);
    int n_values = axes_product(range_size);

    const int MAX_BIOME_CORES = 32;
    m->cores = (biome_core*) dynamic_realloc(m->wall_values, MAX_BIOME_CORES*sizeof(biome_core));

    m->wall_values = (real*) dynamic_realloc(m->wall_values, n_values*sizeof(real));
    m->biomes = (int*) dynamic_realloc(m->biomes, n_values*sizeof(int));
    m->flow = (real_2*) dynamic_realloc(m->flow, n_values*sizeof(real_2));
    m->light = (real*) dynamic_realloc(m->light, n_values*sizeof(real));
    // memset(m->wall_values, 0, n_values*sizeof(real));
    memset(m->biomes, 0, n_values*sizeof(int));
    memset(m->flow, 0, n_values*sizeof(real_2));

    for(int i = 0; i < n_values; i++)
    {
        // m->wall_values[i] = border_wall_value;
        m->wall_values[i] = border_wall_value-0.8f*border_wall_value*randf(&m->seed);
        m->light[i] = 0.5f;
    }

    int start_biome;
    {
        int neighbors[] = {};
        start_biome = add_biome(m, str_to_id("STRT"), {0, 0}, {}, 5000, neighbors, len(neighbors));
    }

    int dark_biome;
    {
        int neighbors[] = {start_biome};
        dark_biome = add_biome(m, str_to_id("DARK"), {0, 0}, {0,1}, 4000, neighbors, len(neighbors));
    }

    int test_biome3;
    {
        int neighbors[] = {dark_biome};
        test_biome3 = add_biome(m, str_to_id("TEST"), {0, 0}, {0,1}, 1000, neighbors, len(neighbors));
    }

    int test_biome;
    {
        int neighbors[] = {start_biome, dark_biome};
        test_biome = add_biome(m, str_to_id("TEST"), {0, 0}, {1,0}, 1000, neighbors, len(neighbors));
    }

    int test_biome2;
    {
        int neighbors[] = {start_biome, dark_biome};
        test_biome2 = add_biome(m, str_to_id("TST2"), {0, 0}, {-1,0}, 1000, neighbors, len(neighbors));
    }

    //spread biomes 1 tile into walls
    int* biomes = (int*) stalloc_clear(n_values*sizeof(int));
    memcpy(biomes, m->biomes, n_values*sizeof(int));
    for(int i = 0; i < n_values; i++)
    {
        int_2 p = {i%range_size.x + m->map_range.l.x, i/range_size.x + m->map_range.l.y};
        if(biomes[i] == 0)
        {
            int neighbor = 0;
            for(int d = 0; d < 6; d++)
            {
                int_2 dpos = (d>=3?-1:1)*(int_2){(1-(d%3)), ((d%3)>=1)};
                if(is_inside(p+dpos, m->map_range)) neighbor = max(neighbor, biomes[index_2(p+dpos-m->map_range.l, dim(m->map_range))]);
            }
            m->biomes[i] = neighbor;
            if(neighbor) {
                m->light[i] = biome_types[get_biome_index(neighbor)].light;
            }
        }
        else m->biomes[i] = biomes[i];
    }

    stunalloc(biomes);
}

#endif //MAP_GEN
