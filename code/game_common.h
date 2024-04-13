#ifndef GAME_COMMON
#define GAME_COMMON

#include <time.h>

#include "gui.h"

#include "translations.h"

#include "bounding_box_2.h"

#include "misc.h"

#include "aabb_tree.h"

struct index_table_entry {
    int id;
    int index;
};

struct rational {
    int n;
    int d;
};

struct entity {
    real_3 p;
    real_3 v;
    quaternion orientation;
    real_3 omega;

    int primitive_id; //associated collision primitive in the aabb tree
};

struct game_t {
    bool replay_mode;
    bool pause_menu;
    bool paused;

    bool do_draw_lightprobes;
    bool do_draw_aabb;

    real_3 pos;

    aabb_tree* tree;

    entity* entities;
    int n_entities;

    uint seed;
    int frame_number;
};

game_t game;

entity* get_entity(int id)
{
    return game.entities+id-1;
}

//collision detection and handling
struct collision_point {
    real_3 p;
    real_3 n;
    real d;
};

struct minkowski_point {
    real_3 p;
    real_3 a;
    real_3 b;
};

struct support_t {
    minkowski_point p;
    real d;
};

void handle_collision(int entity_id, collision_point point)
{
    entity* e = get_entity(entity_id);

    real m = 1.0;
    real invm = 1.0/m;
    real_3x3 invI = real_identity_3(10.0);

    real_3 r = point.p-e->p;
    real_3x3 r_dual = {
        0   ,+r.z,-r.y,
        -r.z, 0  ,+r.x,
        +r.y,-r.x, 0
    };
    real_3 u = e->v+cross(e->omega, r);
    real_3 normal = point.n;

    real COF = 0.5;
    real COR = 0.0;

    real_3x3 K = real_identity_3(invm) - r_dual*invI*r_dual;
    real_3x3 invK = inverse(K);

    real u_n = dot(u, normal);
    real_3 impulse = invK*(-COR*u_n*normal-u);

    real normal_force = dot(impulse, normal);

    real_3 u_t = u-u_n*normal;
    real_3 tangent = normalize_or_zero(u_t);

    //if static friction impulse is outside of the friction cone, compute kinetic impulse
    //NOTE could warm start here
    if(normsq(impulse) > (1+sq(COF))*sq(normal_force) || normal_force < 0.0) {
        real_3 impulse_dir = normal;
        real effective_COR = COR;
        normal_force = max(normal_force, 0.0f);
        real_3 tangent_force = impulse-normal_force*normal;
        impulse_dir += COF*normalize_or_zero(tangent_force);
        impulse = normal_force*impulse_dir;
    }
    e->v += invm*impulse;
    e->omega += invI*cross(r, impulse);

    { //position correction
        real depth = -point.d;
        // real_3 deltax = 0.05f*max(depth, 0.0f)*normal;
        real_3 psuedo_impulse = invK*(0.0625f*max(depth, 0.0f)*normal);
        e->p += psuedo_impulse;
        real_3 pseudo_omega = m*invI*cross(r, psuedo_impulse);
        e->orientation = axis_to_quaternion(pseudo_omega)*e->orientation;
    }
}

struct signed_distance {
    real d;
    real_3 n;
};

//sdf in local coordinates, ignoring the radius part
signed_distance sdf_local_core(primitive_t* a, real_3 r)
{
    switch(a->type) {
        case PRIM_SPHERE: {
            real l = norm(r);
            return {l, r/l};
        } break;
        case PRIM_CAPSULE: {
            r.z -= clamp(r.z, -a->size.z, a->size.z);
            real l = norm(r);
            return {l, r/l};
        } break;
        case PRIM_DISC: {
            r.xy -= radius_min(r.xy, a->size.x);
            real l = norm(r);
            return {l, r/l};
        } break;
        case PRIM_BOX: {
            real_3 q = abs_per_axis(r);
            real_3 v = q - a->size;
           if(v.x <= 0 && v.y <= 0 && v.z <= 0) {
                if(v.x > v.y && v.x > v.z)
                    return {v.x, {sign_not_zero(r.x),0,0}};
                else if(v.y > v.z)
                    return {v.y, {0,sign_not_zero(r.y),0}};
                else
                    return {v.z, {0,0,sign_not_zero(r.z)}};
            } else {
               v = max_per_axis(v, {});
               real l = norm(v);
               return {l, (v*sign_not_zero_per_axis(r))/l};
            }
        } break;
        default: { //fallback, just treat it like a sphere
            real l = norm(r);
            return {l, r/l};
        } break;
    }
}

real_3 support(primitive_t* a, real_3 d)
{
    switch(a->type) {
        case PRIM_SPHERE: {
            return a->p+a->r*d;
        } break;
        case PRIM_BOX: {
            d = apply_rotation(conjugate(a->orientation), d);
            real_3 s = sign_not_zero_per_axis(d);
            return a->p+apply_rotation(a->orientation, s*a->size);
        } break;
        default: { //fallback just treat it like a sphere with radius r+|size|
            return a->p+(a->r+norm(a->size))*d;
        } break;
    }
}

minkowski_point support(primitive_t* a, primitive_t* b, real_3 d)
{
    minkowski_point point;
    point.a = support(a, d);
    point.b = support(b, -d);
    point.p = point.a-point.b;
    return point;
}

int find_collision_manifold(primitive_t* a, primitive_t* b, collision_point* points) {
    assert(a->type <= b->type);

    int n_points = 0;
    switch(a->type) {
        case PRIM_SPHERE: {
            real_3 r = apply_rotation(conjugate(b->orientation), a->p - b->p);
            signed_distance s = sdf_local_core(b, r);
            real depth = s.d-b->r-a->r;
            if(depth > 0.0) return 0;
            real_3 normal = apply_rotation(b->orientation, s.n);
            points[n_points++] = {
                .p = a->p-0.5*(s.d-b->r+a->r)*normal,
                .n = normal,
                .d = depth,
            };
            return n_points;
        } break;
        case PRIM_CAPSULE: {
            //NOTE: could try to detect parallel collisions and generate two points
            //      for more stable contact generation
            real_3 axis = apply_rotation(a->orientation, (real_3){0,0,1.0f});
            real_3 axis_b_local = apply_rotation(conjugate(b->orientation), axis);
            real_3 r = apply_rotation(conjugate(b->orientation), a->p - b->p);
            real t = 0.0;
            real_3 q = r;
            signed_distance s = {};
            real scale = a->size.z;
            // binary search for closest point, haiku:
            // assumes sdf
            // is a convex function of
            // distance on axis
            for(int i = 0;; i++) {
                s = sdf_local_core(b, q);
                if(i >= 16) break;
                real dir = sign(dot(s.n, axis_b_local));
                if(i == 1 && dir*sign(t) <= 0.0) break;
                t -= scale*dir;
                scale *= 0.5;
                q = r+t*axis_b_local;
            }
            real depth = s.d - a->r - b->r;
            if(depth > 0.0) return 0;
            real_3 normal = apply_rotation(b->orientation, s.n);
            points[n_points++] = {
                .p = a->p + t*axis - 0.5*(s.d-b->r+a->r)*normal,
                .n = normal,
                .d = depth,
            };
            return n_points;
        } break;
        case PRIM_DISC: {
            real_3 axis_b_local = apply_rotation(conjugate(b->orientation)*a->orientation, (real_3) {0,0,1.0f});
            real_3 r = apply_rotation(conjugate(b->orientation), a->p - b->p);
            real_3 q = {};
            signed_distance s = {};
            //NOTE: this does not give ideal points for deep collisions
            //      where the disc is inside the core of the other shape
            for(int i = 0;; i++) {
                s = sdf_local_core(b, r+q);
                if(s.d <= a->r + b->r) break;
                if(i >= 24) break;
                q -= (s.d+0.01)*rej(s.n, axis_b_local); //slight bias to make sure it goes inside when they're colliding
                q = radius_min(q, a->size.x);
            }
            real depth = s.d - a->r - b->r;
            if(depth > 0.0) return 0;
            real_3 normal = apply_rotation(b->orientation, s.n);
            q = apply_rotation(b->orientation, q);
            points[n_points++] = {
                .p = a->p + q - 0.5*(s.d-b->r+a->r)*normal,
                .n = normal,
                .d = depth,
            };
            return n_points;
        } break;
        case PRIM_BOX: {
            real_3x3 axes_b_local = apply_rotation(conjugate(b->orientation)*a->orientation, real_identity_3(1.0f));
            real_3 r = apply_rotation(conjugate(b->orientation), a->p - b->p);
            real_3 q = {};
            signed_distance s = {};
            //NOTE: this does not give ideal points for deep collisions
            //      where the box is inside the core of the other shape, that's notable bad for the box box case with out rounded corners
            for(int i = 0;; i++) {
                s = sdf_local_core(b, r+q);
                if(s.d <= a->r + b->r) break;
                if(i >= 24) break;
                q -= (s.d+0.01)*s.n; //slight bias to make sure it goes inside when they're colliding
                q = q*axes_b_local;
                q = clamp_per_axis(q, -a->size, a->size);
                q = axes_b_local*q;
            }
            real depth = s.d - a->r - b->r;
            if(depth > 0.0) return 0;
            real_3 normal = apply_rotation(b->orientation, s.n);
            q = apply_rotation(b->orientation, q);
            points[n_points++] = {
                .p = a->p + q - 0.5*(s.d-b->r+a->r)*normal,
                .n = normal,
                .d = depth,
            };
            return n_points;
        } break;
    }
    return 0;
}

#if 0 //non-sdf version, incomplete
int find_collision_manifold(primitive_t* a, primitive_t* b, collision_point* points) {
    switch(a->type+N_PRIMITIVE_TYPES*b->type) {
        case PRIM_SPHERE+N_PRIMITIVE_TYPES*PRIM_SPHERE: {
            real_3 r = b->p-a->p;
            if(normsq(r) > sq(a->r+b->r)) return 0;
            real_3 n = normalize(r);
            points[0].p = a->p+n*0.5*(a->r + norm(r) - b->r);
            points[0].n = n;
            return 1;
        } break;
        case PRIM_SPHERE+N_PRIMITIVE_TYPES*PRIM_BOX: {
            real_3 r = b->p-a->p;
            r = apply_rotation(conjugate(b->orientation), r);
            real_3 q = clamp_per_axis(r, -b->size, b->size);
            if(normsq(r-q) > sq(a->r)) return 0;
            points[0].p = b->p-apply_rotation(b->orientation, q);
            points[0].n = apply_rotation(b->orientation, normalize(r-q));
            return 1;
        } break;
        case PRIM_BOX+N_PRIMITIVE_TYPES*PRIM_SPHERE: {
            real_3 r = b->p-a->p;
            r = apply_rotation(conjugate(a->orientation), r);
            real_3 q = clamp_per_axis(r, -a->size, a->size);
            if(normsq(r-q) > sq(b->r)) return 0;
            points[0].p = a->p-apply_rotation(a->orientation, q);
            points[0].n = apply_rotation(a->orientation, normalize(r-q));
            return 1;
        } break;
    }

    //fallback to gjk
#define do_support(d) { minkowski_point s = support(a, b, d);   \
//         if(dot(s.p, d) < 0.0) return 0;                         \
//         simplex[n_simplex++] = s; }

    minkowski_point simplex[4];
    int n_simplex = 0;
    do_support(b->p - a->p);

    int n_points = 0;

    real_3 v[3];
    real_3 f[3];

    float face_sign = 1.0;

    for(int steps = 0;; steps++) {
        if(steps > 20) return 0;
        switch(n_simplex) {
            case 1: {
                do_support(-simplex[0].p);
            } break;
            case 2: {
                real_3 v = simplex[0].p-simplex[1].p;
                real_3 dir = cross(v, cross(v, simplex[0].p));
                do_support(dir);
            } break;
            case 3: {
                v[0] = simplex[0].p-simplex[2].p;
                v[1] = simplex[1].p-simplex[2].p;

                f[2] = cross(v[0], v[1]); //normal of triangle
            gjk_triangle: //this part is the same when the 4 case reduces to a triangle
                f[0] = cross(f[2], v[1]); //normal of 20 edge
                f[1] = cross(v[0], f[2]); //normal of 12 edge

                for(int i = 0; i < 2; i++) {
                    if(dot(f[i], simplex[2].p) < 0) {
                        simplex[0] = simplex[--n_simplex]; //the opposite vertex is on the wrong side, so drop it
                        if(dot(v[i], simplex[2].p) > 0) --n_simplex;
                        goto next_simplex;
                    }
                }
                //inside the triangle
                face_sign = dot(f[2], simplex[2].p) < 0.0 ? 1.0 : -1.0;
                do_support(face_sign*f[2]);
            } break;
            case 4: {
                for(int i = 0; i < 3; i++) {
                    v[i] = simplex[i].p-simplex[3].p;
                }
                for(int i = 0; i < 3; i++) {
                    int j = (i+1)%3;
                    int k = (i+2)%3;
                    real_3 face_cross = cross(v[j], v[k]);
                    real_3 face = face_sign*face_cross;

                    if(dot(face, simplex[3].p) < 0) {
                        real_3 v0 = v[j];
                        real_3 v1 = v[k];
                        minkowski_point s0 = simplex[j];
                        minkowski_point s1 = simplex[k];
                        minkowski_point s2 = simplex[3];
                        n_simplex = 3;
                        simplex[0] = s0;
                        simplex[1] = s1;
                        simplex[2] = s2;
                        v[0] = v0;
                        v[1] = v1;
                        f[2] = face_cross;
                        goto gjk_triangle; //calculation is identical to triangle case from here
                    }
                }
                points[0] = {simplex[0].a, {0,0,1}}; //TODO: make this better
                points[1] = {simplex[1].a, {0,0,1}}; //TODO: make this better
                points[2] = {simplex[2].a, {0,0,1}}; //TODO: make this better
                points[3] = {simplex[3].a, {0,0,1}}; //TODO: make this better
                return 4;
            } break;
        }
    next_simplex:;
    }
#undef do_support
    return 0;
}
#endif

//broadphase collision detection
void do_collisions(aabb_tree* tree)
{
    int max_collisions = (tree->n_primitives*(tree->n_primitives-1))/2;
    int_2* collision_ids = stalloc_typed(max_collisions, int_2);
    int n_collisions = 0;
    int max_pairs = (tree->n_nodes*(tree->n_nodes-1))/2;
    int_2* pairs = stalloc_typed(max_pairs, int_2);
    int n_pairs = 0;
    pairs[n_pairs++] = {tree->root,tree->root};
    while(n_pairs > 0) {
        int_2 pair = pairs[--n_pairs];
        aabb_node* a = tree->nodes+pair[0]-1;
        aabb_node* b = tree->nodes+pair[1]-1;
        if(does_intersect(a->aabb, b->aabb)) {
            if(a->leaf && b->leaf) {
                if(a != b && (a->entity_id || b->entity_id))
                    collision_ids[n_collisions++] = {a->leaf, b->leaf};
            } else if(a->leaf){
                pairs[n_pairs++] = {pair[0], b->children[0]};
                pairs[n_pairs++] = {pair[0], b->children[1]};
            } else if(b->leaf) {
                pairs[n_pairs++] = {pair[1], a->children[0]};
                pairs[n_pairs++] = {pair[1], a->children[1]};
            }  else {
                pairs[n_pairs++] = {a->children[0], b->children[0]};
                pairs[n_pairs++] = {a->children[1], b->children[1]};
                pairs[n_pairs++] = {a->children[0], b->children[1]};
                if(a != b)
                    pairs[n_pairs++] = {a->children[1], b->children[0]};
            }
        }

        assert(n_pairs <= max_pairs);
        assert(n_collisions <= max_collisions);
    }

    for(int i = 0; i < n_collisions; i++) {
        int j = rand(&game.seed, i, n_collisions);
        swap(collision_ids[i], collision_ids[j]);

        primitive_t* a = tree->primitives+collision_ids[i][0]-1;
        primitive_t* b = tree->primitives+collision_ids[i][1]-1;

        if(a->type > b->type) swap(a, b);
        collision_point points[16];
        int n_points = find_collision_manifold(a, b, points);

        if(n_points <= 0) continue;

        // a->emission.y = 1;
        // b->emission.y = 1;

        // for(int i = 0; i < n_points; i++) {
        //     draw_sphere(points[i].p, 0.02f, {1.0, 1.0, 1.0, 1.0});
        // }

        if(a->entity_id && b->entity_id) {
            //handle collision
        } else if(a->entity_id) {
            handle_collision(a->entity_id, points[0]);
        } else if(b->entity_id) {
            collision_point bp = points[0];
            bp.n = -bp.n;
            handle_collision(b->entity_id, bp);
        }
    }

    stunalloc(pairs);
    stunalloc(collision_ids);
}

void update_entities(user_input* input)
{
    for(int i = 0; i < game.tree->n_primitives; i++) {
        game.tree->primitives[i].emission.y = 0.0;
    }
    for(int i = 0; i < game.n_entities; i++) {
        entity* e = game.entities+i;
        e->p += e->v;
        e->v.z -= 0.001;
        e->orientation = axis_to_quaternion(e->omega)*e->orientation;

        int pid = e->primitive_id;
        remove_leaf(game.tree, game.tree->primitives[pid-1].node);
        game.tree->primitives[pid-1].p = e->p;
        game.tree->primitives[pid-1].orientation = e->orientation;
        game.tree->primitives[pid-1].v = e->v;
        game.tree->primitives[pid-1].omega = e->omega;
        insert_primitive(game.tree, pid);
    }

    {
        static bool play = true;
        int pid;
        static real time = 0.0;
        pid = game.tree->n_primitives-1;
        real light_time = fmod(time, 9.0);
        if(light_time < 3.0) game.tree->primitives[pid-1].emission = {1,0,0};
        else if(light_time < 6.0) game.tree->primitives[pid-1].emission = {0,1,0};
        else game.tree->primitives[pid-1].emission = {0,0,1};
        game.tree->primitives[pid-1].orientation = axis_to_quaternion({0.0,0.2*time,0.0});
        remove_leaf(game.tree, game.tree->primitives[pid-1].node);
        insert_primitive(game.tree, pid);

        play = (play != is_pressed('F', input));
        if(play) time += vkon.frame_time;
    }

    for(int i = 0; i < 10; i++) {
        do_collisions(game.tree);
    }
}

#endif //GAME_COMMON
