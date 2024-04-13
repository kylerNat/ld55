vec3 wall_valued(vec2 p)
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

    v += vec3(w.x, dw_dx.x, dw_dy.x)*texture(wall_values, inv_map_size*(0.5f+c-vec2(wall_map_min))).r;
    v += vec3(w.y, dw_dx.y, dw_dy.y)*texture(wall_values, inv_map_size*(0.5f+iq+vec2(1.0-h,h)-vec2(wall_map_min))).r;
    v += vec3(w.z, dw_dx.z, dw_dy.z)*texture(wall_values, inv_map_size*(0.5f+iq+vec2(h,1.0-h)-vec2(wall_map_min))).r;
    // v += vec3(w.x, dw_dx.x, dw_dy.x)*texelFetch(wall_values, ivec2(c)-wall_map_min, 0).r;
    // v += vec3(w.y, dw_dx.y, dw_dy.y)*texelFetch(wall_values, ivec2(iq+vec2(1.0-h,h))-wall_map_min, 0).r;
    // v += vec3(w.z, dw_dx.z, dw_dy.z)*texelFetch(wall_values, ivec2(iq+vec2(h,1.0-h))-wall_map_min, 0).r;

    return v;
}

vec3 wall_map(vec2 p, out vec3 noise)
{
    vec2 x = vec2(0.0);
    float r = 1000.0;
    float d = 2.0*r;
    vec2 xm = floor(p/d+0.5)*d;

    vec3 wall = (wall_valued(map_scale*p));
    wall.yz *= map_scale;
    wall /= length(wall.yz);

    // wall = smin(wall, vec3(10000.0f-length(p), -normalize(p)), 100.0f);

    if(wall.x < 1.0)
    {
        // noise = 5.0*fbmd(0.1*p, 0.0, 6, 0.5, 1.0);
        noise = 5.0*smoothstep(1.0, 0.8, wall.x)*tri_noised(0.1*p, 0.0f);
        noise.yz *= 0.1;
        // noise.x /= length(noise.yz);

        wall.x += noise.x;
        wall.yz += noise.yz;
    }

    // wall /= length(wall.yz);

    return wall;
}
