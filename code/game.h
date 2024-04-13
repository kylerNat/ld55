#ifndef GAME                    //
#define GAME

#include <maths/maths.h>
#include "game_common.h"
#include "memory.h"

const real game_dt = 1.0/(120.0);

void init_world()
{
    game.seed = 3192859812;
    game.frame_number = 0;

    game.entities = dynamic_alloc_typed(1024, entity);
    game.n_entities = 0;

    game.tree = dynamic_alloc_typed(1, aabb_tree);
    game.tree->n_nodes = 0;
    game.tree->n_primitives = 0;

    {
        for(int i = 0; i < 10; i++)
        {
            game.tree->primitives[game.tree->n_primitives++] = (primitive_t) {
                .p = randf_3(&game.seed, {-5,-5,-5}, {5,5,5}),
                .r = 0.1f,
                .size = {0,0,0},
                .type = PRIM_SPHERE,
                .orientation = {1,0,0,0},
                .albedo = {1,1,1},
                .emission = {0,0,0},
            };
            insert_primitive(game.tree, game.tree->n_primitives);
        }

        game.tree->primitives[game.tree->n_primitives++] = (primitive_t) {
            .p = {3,3,0},
            .r = 0.0f,
            .size = {1.1f,1.5f,1.2f},
            .type = PRIM_BOX,
            .orientation = axis_to_quaternion({1.0,1.0,1.0}),
            .albedo = {1,1,1},
            .emission = {0,0,0},
        };
        insert_primitive(game.tree, game.tree->n_primitives);

        game.tree->primitives[game.tree->n_primitives++] = (primitive_t) {
            .p = {3.0f,0.0f,-2.0f},
            .r = 0.0f,
            .size = {2.1f,5.5f,0.2f},
            .type = PRIM_BOX,
            .orientation = {1.0,0.0,0.0,0.0},
            .albedo = {1,1,1},
            .emission = {0,0,0},
        };
        insert_primitive(game.tree, game.tree->n_primitives);

        game.tree->primitives[game.tree->n_primitives++] = (primitive_t) {
            .p = {5.0f,0.0f,2.0f},
            .r = 0.0f,
            .size = {1.0f,0.2f,4.0f},
            .type = PRIM_BOX,
            .orientation = {1.0,0.0,0.0,0.0},
            .albedo = {1,1,1},
            .emission = {0,0,0},
        };
        insert_primitive(game.tree, game.tree->n_primitives);

        game.tree->primitives[game.tree->n_primitives++] = (primitive_t) {
            .p = {1.0f,0.0f,2.0f},
            .r = 0.0f,
            .size = {1.0f,0.2f,4.0f},
            .type = PRIM_BOX,
            .orientation = {1.0,0.0,0.0,0.0},
            .albedo = {1,1,1},
            .emission = {0,0,0},
        };
        insert_primitive(game.tree, game.tree->n_primitives);

        game.tree->primitives[game.tree->n_primitives++] = (primitive_t) {
            .p = {3.0f,-5.0f,2.0f},
            .r = 0.0f,
            .size = {1.5f,0.2f,4.0f},
            .type = PRIM_BOX,
            .orientation = {1.0,0.0,0.0,0.0},
            .albedo = {0,0,0},
            .emission = {1,0,0},
        };
        insert_primitive(game.tree, game.tree->n_primitives);
    }

    int eid = ++game.n_entities;
    entity* e = game.entities+eid-1;
    *e = {};
    // e->p = {0.0f, 4.0f, 0.0f};
    // e->orientation = axis_to_quaternion(1.5*(real_3){1.0f,1.0f,1.0f});
    e->p = {4.568948, -0.373175, -0.801927};
    e->orientation = {0.056979, 0.896681, 0.318195, 0.301852};

    game.tree->primitives[game.tree->n_primitives++] = (primitive_t) {
        .p = e->p,
        .r = 0.2f,
        .size = {0.2f,0.2f,0.2f},
        .type = PRIM_CAPSULE,
        .orientation = e->orientation,
        .albedo = {0.5,0.2,0.5},
        .emission = {0.0,0,0.0},
        .entity_id = eid,
    };
    insert_primitive(game.tree, game.tree->n_primitives);
    e->primitive_id = game.tree->n_primitives;
}

void create_world()
{
    init_world();
}

void update_game(user_input* input)
{
    game.do_draw_lightprobes = game.do_draw_lightprobes != is_pressed(VK_F4, input);
    game.do_draw_aabb = game.do_draw_aabb != is_pressed(VK_F3, input);

    static real phi = 0.0;
    static real theta = 0.5*pi;

    real sens = 1.0;
    if(!is_down('E', input) || !is_down(M1, input)) {
        phi -= sens*input->dmouse.x;
        theta += sens*input->dmouse.y;
    }
    else {
        game.entities[0].orientation = axis_to_quaternion(-input->dmouse.x*vkon.camera_axes[1]+input->dmouse.y*vkon.camera_axes[0])*game.entities[0].orientation;

    }

    vkon.camera_axes[0] = {cos(phi), sin(phi), 0};
    vkon.camera_axes[2] = {-sin(phi)*sin(theta), cos(phi)*sin(theta), cos(theta)};
    vkon.camera_axes[1] = cross(vkon.camera_axes[2], vkon.camera_axes[0]);

    vkon.camera_pos = game.pos;

    real speed = (1.0+1.0*is_down(VK_SHIFT, input))*vkon.frame_time;
    game.pos += (is_down('W', input)-is_down('S', input))*speed*vkon.camera_axes[2];
    game.pos += (is_down('D', input)-is_down('A', input))*speed*vkon.camera_axes[0];
    static real hold_dist = 1.0;
    if(is_down('E', input)) {
        real_3 new_p = game.pos + hold_dist*vkon.camera_axes[2];
        game.entities[0].v = new_p-game.entities[0].p;
        // game.entities[0].p = new_p;
        game.entities[0].omega = {};
        hold_dist *= pow(1.01, input->mouse_wheel);
        if(is_pressed('E', input)) game.entities[0].v = {};
    }
    if(is_released('E', input)) {
        log_output("p: ", game.entities[0].p, ", o: ", game.entities[0].orientation, "\n");
    }

    update_entities(input);

}

void render_game(user_input* input)
{
    if(game.replay_mode) {
        do_replay_export_menu(input, vkon.frame_time);
        return;
    }
    if(game.pause_menu) {
        if(do_pause_menu(input, vkon.frame_time)) game.pause_menu = false;
        return;
    }

    static real Deltat = 0;
    Deltat += vkon.frame_time;
    real gif_frame_time = 0.01f*vkon.replay_centiseconds;
    if(Deltat >= gif_frame_time) {
        vkon.do_gif_frame = 1;
        Deltat -= gif_frame_time;
    }

    vkon.time += vkon.frame_time;

    uint32 f = vkon.current_frame;
    global_ssbo* ssbo = (global_ssbo*) vkon.shader_storage_buffers_mapped[f];
    memcpy(ssbo->primitives, game.tree->primitives, game.tree->n_primitives*sizeof(primitive_t));
    ssbo->n_primitives = game.tree->n_primitives;

    draw_world();

#ifdef DEV_CHEATS
    game.do_draw_lightprobes = false;
    if(game.do_draw_lightprobes) {
        draw_lightprobes();
    }

    game.do_draw_aabb = false;
    if(game.do_draw_aabb) {
        uint32 f = vkon.current_frame;
        global_ssbo* ssbo = (global_ssbo*) vkon.shader_storage_buffers_mapped[f];
        for(int i = 0; i < game.tree->n_nodes; i++) {
            ssbo->nodes[i] = game.tree->nodes[i].aabb;
        }
        ssbo->n_nodes = game.tree->n_nodes;
        draw_aabb();
    }
#endif

    draw_circle(pad_3(input->mouse), 0.01, {1,1,1,1});

    static real smoothed_frame_time = 1.0;
    smoothed_frame_time = lerp(smoothed_frame_time, vkon.frame_time, 0.03);
    if(settings.show_fps)
    {
        char frame_time_text[1024];
        sprintf(frame_time_text, "%2.1f ms\n%2.1f fps", smoothed_frame_time*1000.0f, 1.0f/smoothed_frame_time);

        draw_text(frame_time_text, {-vkon.aspect_ratio+0.05, -0.95}, {1,1,1,1}, {-1,-1}, vkon.default_font);
    }
}

#endif //GAME
