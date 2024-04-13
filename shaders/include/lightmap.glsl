#include "lightmap_constants.h"

vec2 vec_to_oct(vec3 p)
{
    vec3 sign_p = sign_not_zero(p);
    vec2 oct = p.xy * (1.0f/dot(p, sign_p));
    return (p.z > 0) ? oct : (1.0f-abs(oct.yx))*sign_p.xy;
}

vec3 oct_to_vec(vec2 oct)
{
    vec2 sign_oct = sign(oct);
    vec3 p = vec3(oct.xy, 1.0-dot(oct, sign_oct));
    if(p.z < 0) p.xy = (1.0f-abs(p.yx))*sign_oct.xy;
    return normalize(p);
}

//cdf of a gaussian, approximated
float cdf(float x)
{
    return 0.5f*tanh(0.797884560803f*(x+0.044715f*x*x*x))+0.5f;
}

#ifndef NO_LIGHTMAP_SAMPLING
vec3 sample_lightmap_color(vec3 pos, vec3 normal, vec2 sample_oct, out vec2 depth)
{
    // pos = pos+8*normal; //bias away from surfaces
    vec4 irradiance = vec4(0);
    vec4 irradiance_no_cheb = vec4(0);
    depth = vec2(0);
    ivec3 base_probe_pos = ivec3(floor((pos-LIGHTPROBE_OFFSET)/LIGHTPROBE_SPACING));

    for(int pz = 0; pz <= 1; pz++)
        for(int py = 0; py <= 1; py++)
            for(int px = 0; px <= 1; px++)
            {
                vec3 p = vec3(px, py, pz);
                ivec3 ip = ivec3(px, py, pz);
                ivec3 probe_pos = base_probe_pos+ip;
                if(any(lessThan(probe_pos, vec3(0)))
                   || any(greaterThanEqual(probe_pos, vec3(LIGHTPROBE_GRID_W, LIGHTPROBE_GRID_H, LIGHTPROBE_GRID_D)))) continue;
                int probe_index = int(dot(probe_pos, ivec3(1,LIGHTPROBE_GRID_W,LIGHTPROBE_GRID_W*LIGHTPROBE_GRID_H)));
                ivec2 probe_coord = ivec2(probe_index%LIGHTMAP_LAYOUT_W, probe_index/LIGHTMAP_LAYOUT_W);

                vec2 sample_coord = vec2(LIGHTPROBE_COLOR_PADDED_RESOLUTION*probe_coord+1)+LIGHTPROBE_COLOR_RESOLUTION*clamp(0.5f*sample_oct+0.5f,0,1);

                sample_coord += 0.5;
                vec2 t = trunc(sample_coord);
                vec2 f = fract(sample_coord);
                f = f*f*f*(f*(f*6.0-15.0)+10.0);
                // f = f*f*(-2*f+3);
                sample_coord = t+f-0.5;

                sample_coord *= vec2(1.0f/LIGHTMAP_COLOR_TEXTURE_RESOLUTION, 1.0f/LIGHTMAP_COLOR_TEXTURE_RESOLUTION);

                vec3 probe_x = LIGHTPROBE_SPACING*vec3(probe_pos)+LIGHTPROBE_OFFSET;

                vec4 probe_color = texture(lightmap_color, sample_coord);

                probe_color.a = 1;

                float normal_bias = 0.1;
                vec3 dist = pos+(normal)*normal_bias-probe_x; //NOTE: Majercjk adds 3*view dir to the normal here
                float r = max(length(dist), 0.0001f);
                vec3 dir = dist*(1.0f/r);

                vec2 dir_oct = vec_to_oct(dir);
                vec2 depth_sample_coord = vec2(LIGHTPROBE_DEPTH_PADDED_RESOLUTION*probe_coord+1.0)+LIGHTPROBE_DEPTH_RESOLUTION*clamp(0.5f*dir_oct+0.5f,0,1);
                depth_sample_coord *= vec2(1.0f/LIGHTMAP_DEPTH_TEXTURE_RESOLUTION, 1.0f/LIGHTMAP_DEPTH_TEXTURE_RESOLUTION);
                vec2 probe_depth = texture(lightmap_depth, depth_sample_coord).rg;

                float weight = 1.0;

                vec3 true_dir = normalize(probe_x-pos); //direction without bias
                weight *= sq(0.5*dot(normal, true_dir)+0.5)+0.0;

                vec3 base_dist = pos-probe_x;

                vec3 trilinear_weights = clamp(1.0-(1.0-2.0*p)*(1.0f/LIGHTPROBE_SPACING)*base_dist,
                                               0.0, 1.0);
                weight *= trilinear_weights.x*trilinear_weights.y*trilinear_weights.z+0.001;

                irradiance_no_cheb += weight*sqrt(probe_color);

                //Chebychev's inequality, upper bound for an arbitrary distribution
                if(r > probe_depth.r)
                {
                    float variance = abs(probe_depth.g-(probe_depth.r*probe_depth.r));
                    float cheb_weight = variance/max(variance+sq(r-probe_depth.r), 0.001);
                    cheb_weight = max(sq(cheb_weight)*cheb_weight, 0);
                    weight *= cheb_weight;
                }

                // //This assumes Gaussian distribution
                // float variance = abs(probe_depth.g-sq(probe_depth.r));
                // float x = (probe_depth.r-r)*inversesqrt(variance);
                // weight *= cdf(x);

                //this smoothly kills low values
                const float threshold = 0.02;
                if(weight < threshold)
                    weight *= sq(weight)/sq(threshold);

                irradiance += weight*sqrt(probe_color);
                depth += weight*(probe_depth);
            }
    irradiance.rgb = mix(sq(irradiance_no_cheb.rgb*(1.0f/irradiance_no_cheb.a)),
                          sq(irradiance.rgb*(1.0f/irradiance.a)), min(irradiance.a, 1.0));
    // irradiance.rgb = sq(irradiance.rgb*(1.0f/irradiance.a));

    depth *= (1.0f/irradiance.a);
    return irradiance.rgb;
}
#endif
