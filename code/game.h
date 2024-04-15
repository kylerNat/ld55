#ifndef GAME                    //
#define GAME

#include <time.h>
#include <maths/maths.h>
#include "game_common.h"
#include "memory.h"

void init_world()
{
    time_t seconds;
    time(&seconds);
    game.seed = seconds;
    randui(&game.seed);
    randui(&game.seed);
    randui(&game.seed);
    game.frame_number = 0;

    game.n_entities = 0;
    game.n_creatures = 0;
    game.n_projectiles = 0;
    game.n_boss_projectiles = 0;
    game.n_damage_numbers = 0;
    game.n_explosions = 0;

    game.boss_spawned = false;

    game.pos = {24.0f, 24.0f, 24.0f};

    game.speed_multiplier = 1;
}

void create_world()
{
    game = {};

    game.entities = dynamic_alloc_typed(1024, entity);
    game.creatures = dynamic_alloc_typed(1024, creature_t);
    game.projectiles = dynamic_alloc_typed(65536, projectile_t);
    game.boss_projectiles = dynamic_alloc_typed(65536, projectile_t);
    game.damage_numbers = dynamic_alloc_typed(65536, damage_number_t);
    game.explosions = dynamic_alloc_typed(65536, explosion_t);

    init_world();
}

void update_game(user_input* input)
{
    game.do_draw_lightprobes = game.do_draw_lightprobes != is_pressed(VK_F4, input);
    game.do_draw_aabb = game.do_draw_aabb != is_pressed(VK_F3, input);

    // static real phi = 0.0;
    // static real theta = 0.5*pi;

    // real sens = 1.0;
    // if(!is_down('E', input) || !is_down(M1, input)) {
    //     phi -= sens*input->dmouse.x;
    //     theta += sens*input->dmouse.y;
    // }
    // else {
    //     game.entities[0].orientation = axis_to_quaternion(-input->dmouse.x*vkon.camera_axes[1]+input->dmouse.y*vkon.camera_axes[0])*game.entities[0].orientation;

    // }

    // vkon.camera_axes[0] = {cos(phi), sin(phi), 0};
    // vkon.camera_axes[2] = {-sin(phi)*sin(theta), cos(phi)*sin(theta), cos(theta)};
    // vkon.camera_axes[1] = cross(vkon.camera_axes[2], vkon.camera_axes[0]);

    // real speed = (10.0+10.0*is_down(VK_SHIFT, input))*vkon.frame_time;
    // game.pos += (is_down('W', input)-is_down('S', input))*speed*vkon.camera_axes[2];
    // game.pos += (is_down('D', input)-is_down('A', input))*speed*vkon.camera_axes[0];
    // game.pos += (is_down(' ', input)-is_down(VK_SHIFT, input))*speed*vkon.camera_axes[1];
    // static real hold_dist = 1.0;
    // if(is_down('E', input)) {
    //     real screen_dist = 1.0f/tan(0.5f*vkon.fov);
    //     real_3 d = vkon.camera_axes*(real_3){input->mouse.x, input->mouse.y, screen_dist};
    //     // d = normalize(d);
    //     real t = -(game.pos.z-1.0)/d.z;
    //     real_3 new_p = game.pos+d*t;

    //     game.entities[0].v = new_p-game.entities[0].p;
    //     // game.entities[0].p = new_p;
    //     game.entities[0].omega = {};
    //     hold_dist *= pow(1.01, input->mouse_wheel);
    //     if(is_pressed('E', input)) game.entities[0].v = {};
    // }
    // if(is_released('E', input)) {
    //     log_output("p: ", game.entities[0].p, ", o: ", game.entities[0].orientation, "\n");
    // }

    real screen_dist = 1.0f/tan(0.5f*vkon.fov);
    real_3 d = vkon.camera_axes*(real_3){input->mouse.x, input->mouse.y, screen_dist};
    real t = -(game.pos.z)/d.z;
    real_2 p = game.pos.xy+d.xy*t;
    real i = round(atan2(p.y, p.x)/(0.4*pi));
    if(i < 0.0) i += 5.0;
    int ii = (int) i;
    real R = 12.0;
    real phi_circle = i*pi*0.4;
    real_2 c = R*(real_2){cos(phi_circle), sin(phi_circle)};
    if(normsq(p-c) < sq(3.25f)) {
        if(is_pressed(M1, input)) {
            game.selected = ii;
            game.boss_selected = false;
            game.dshake += 0.3f*rand_normal_3(&game.seed);
            // auto c = game.slots[game.selected]; //click to explode, just for testing but lots of fun
            // if(c) c->health = 0.0f;
        }
        game.hovered = ii;
    } else game.hovered = -1;

    if(is_pressed(VK_LEFT, input)) {
        game.selected += 4;
        game.dshake += 0.3f*rand_normal_3(&game.seed);
    }
    if(is_pressed(VK_RIGHT, input)) {
        game.selected += 1;
        game.dshake += 0.3f*rand_normal_3(&game.seed);
    }

    if(normsq(p) < sq(5.0)) {
        if(is_pressed(M1, input)) {
            game.boss_selected = true;
            game.dshake += 0.5f*rand_normal_3(&game.seed);
        }
        game.hovered = 6;
    }
    if(is_pressed(VK_UP, input)) {
        if(game.boss_selected) game.selected += 2;
        game.boss_selected = !game.boss_selected;
        game.dshake += 0.3f*rand_normal_3(&game.seed);
    }
    if(is_pressed(VK_DOWN, input)) {
        game.boss_selected = false;
        game.dshake += 0.3f*rand_normal_3(&game.seed);
    }

    game.selected = game.selected%5;
    game.phi_target = game.selected*(pi*0.4);
    if(game.phi - game.phi_target > pi)
        game.phi -= 2.0*pi;
    if(game.phi - game.phi_target < -pi)
        game.phi += 2.0*pi;
    game.phi = lerp(game.phi, game.phi_target, 0.1);

    real camera_dist = 48.0;
    game.pos = {camera_dist*cos(game.phi), camera_dist*sin(game.phi), camera_dist};

    vkon.camera_axes[2] = -normalize(game.pos);
    vkon.camera_axes[0] = normalize(cross(vkon.camera_axes[2], {0,0,1}));
    vkon.camera_axes[1] = cross(vkon.camera_axes[2], vkon.camera_axes[0]);

    vkon.camera_pos = game.pos+game.shake;
    game.shake += game.dshake;
    game.dshake -= 0.8*game.shake;
    game.dshake *= 0.8;

    real shake_amount = 0.2f;

    if(is_pressed('1', input)) game.speed_multiplier = 1;
    if(is_pressed('2', input)) game.speed_multiplier = 2;
    if(is_pressed('3', input)) game.speed_multiplier = 4;

    if(game.boss_selected) {
        game.current_string = game.boss_string;
        game.n_current_string = &game.n_boss_string;
    } else {
        game.current_string = game.name_string;
        game.n_current_string = &game.n_name_string;
    }

    bool name_changed = false;
    bool spawned = false;
    if(!game.boss_spawned) {
        for(int i = 0; i < input->n_typed; i++) {
            name_changed = true;

            real shake = shake_amount;
            char c = input->typed[i];
            if(is_uppercase(c)) c += 'a'-'A';
            else if(c == VK_SPACE) c = ' ';
            else if(c == VK_BACK || c == VK_DELETE) {
                c = 0;
                *game.n_current_string = max((*game.n_current_string)-1, 0);
            } else if(c == VK_RETURN) {
                c = 0;
                game.current_string[*game.n_current_string] = 0;
                if(*game.n_current_string > 0) {
                    if(game.boss_selected) { //summon boss
                        if(game.n_creatures > 0) {
                            game.boss_spawned = true;
                            spawned = true;
                        } else
                            game.no_units_warning = 1.0f;
                    } else { //absolute spagetti
                        creature_t* existing = 0;
                        creature_t* filling = 0;
                        //check if it's already on the board
                        for(int i = 0; i < game.n_creatures; i++) {
                            if(strcmp(game.current_string, game.creatures[i].name) == 0) {
                                existing = game.creatures+i;
                            }
                            if(game.creatures[i].slot == game.selected) {
                                filling = game.creatures+i;
                            }
                        }
                        if(existing && filling) filling->slot = existing->slot;
                        if(existing) {
                            existing->slot = game.selected;
                            *game.n_current_string = 0;
                        } else {
                            if(filling) *filling = game.creatures[--game.n_creatures];
                            creature_t* cr = game.creatures+(game.n_creatures++);
                            init_creature(cr, game.current_string);
                            cr->slot = game.selected;
                            shake = 1.0f;
                            *game.n_current_string = 0;
                        }
                        spawned = true;
                    }
                }
            } else {
                shake = 0.0f;
                c = 0;
            }
            if(game.boss_selected &&
               (c == 'g' || c == 'j' || c == 'm' || c == 'w')) c += 1;
            if(c && *game.n_current_string < 50) {
                game.current_string[(*game.n_current_string)++] = c;
            }
            game.dshake += shake*rand_normal_3(&game.seed);
        }
        if(is_down(VK_BACK, input) || is_down(VK_DELETE, input)) {
            game.key_repeat_frames += 1;
            if(game.key_repeat_frames >= 30 && game.key_repeat_frames%5 == 0) {
                if(*game.n_current_string > 0)
                    game.dshake += shake_amount*rand_normal_3(&game.seed);
                *game.n_current_string = max((*game.n_current_string)-1, 0);
                name_changed = true;
            }
        } else {
            game.key_repeat_frames = 0;
        }
        game.current_string[*game.n_current_string] = 0;

        // if(name_changed && !spawned) play_sound(&key_sound, 1.0);
        // else
        if(spawned) {
            if(game.boss_selected) play_sound(&explosion2_sound, 1.0);
            else play_sound(&explosion_sound, 1.0);
        }

        if(name_changed && !game.boss_selected) {
            init_creature(&game.spawning, game.name_string);
        } else {
            init_boss(game.boss_string);
        }
        game.n_projectiles = 0;
        game.n_boss_projectiles = 0;
        for(int i = 0; i < game.n_creatures; i++) {
            creature_t* c = game.creatures+i;
            c->health = c->vitality;
            for(int j = 0; j < N_BUFFS; j++) {
                c->buffs[j] = 0;
            }
            for(int j = 0; j < 2; j++) {
                // c->cooldowns[j] = c->abilities[j].cooldown;
                c->cooldowns[j] = 0;
            }
        }
        for(int i = 0; i < 5; i++) {
            game.slots[i] = 0;
        }
        for(int i = 0; i < game.n_creatures; i++) {
            creature_t* c = game.creatures+i;
            if(c->slot >= 0 && c->slot < 5)
                game.slots[c->slot] = c;
        }
    } else if(!game.gameplay_paused) {
        update_creatures();
        game.frame_number++;
    }

    //damage numbers
    for(int i = game.n_damage_numbers-1; i >= 0; i--) {
        damage_number_t* dn = game.damage_numbers+i;
        dn->p.z += 0.05;
        dn->life *= 0.99f;//-0.01f/max(dn->damage, 0.1f);
        if(dn->life < 0.01f) *dn = game.damage_numbers[--game.n_damage_numbers];
    }

    //explosions
    for(int i = game.n_explosions-1; i >= 0; i--) {
        explosion_t* e = game.explosions+i;
        e->p.z += 0.05;
        e->life *= 0.95f;
        if(e->life < 0.01f) *e = game.explosions[--game.n_explosions];
    }

    if(game.n_creatures <= 0 && game.boss_spawned) {
        game.show_defeat = 5.0f;
    }
    if(game.n_creatures > 0 && game.boss.health <= 0.0f && game.boss_spawned) {
        game.boss.dead = true;
        game.show_victory = 5.0f;
    }
}

void render_game(user_input* input)
{
    if(game.replay_mode) {
        return;
    }
    if(game.pause_menu) {
        // return;
    }

    static real Deltat = 0;
    Deltat += vkon.frame_time;
    real gif_frame_time = 0.01f*vkon.replay_centiseconds;
    if(Deltat >= gif_frame_time) {
        vkon.do_gif_frame = 1;
        Deltat -= gif_frame_time;
    }

    uint32 f = vkon.current_frame;
    global_ssbo* ssbo = (global_ssbo*) vkon.shader_storage_buffers_mapped[f];
    ssbo->n_primitives = 0;
    for(int i = 0; i < game.n_creatures; i++) {
        creature_t* c = game.creatures+i;
        draw_creature(ssbo, c);
    }
    for(int i = 0; i < game.n_projectiles; i++) {
        projectile_t* p = game.projectiles+i;
        draw_projectile(ssbo, p);
    }
    for(int i = 0; i < game.n_explosions; i++) {
        explosion_t* e = game.explosions+i;
        draw_explosion(ssbo, e);
    }
    draw_boss(ssbo);
    for(int i = 0; i < ssbo->n_primitives; i++) {
        calculate_aabb(ssbo->primitives+i);
    }
    draw_world();
    draw_floor();

#ifdef DEV_CHEATS
    if(game.do_draw_lightprobes) {
        draw_lightprobes();
    }

    // if(game.do_draw_aabb) {
    //     uint32 f = vkon.current_frame;
    //     global_ssbo* ssbo = (global_ssbo*) vkon.shader_storage_buffers_mapped[f];
    //     for(int i = 0; i < game.tree->n_nodes; i++) {
    //         ssbo->nodes[i] = game.tree->nodes[i].aabb;
    //     }
    //     ssbo->n_nodes = game.tree->n_nodes;
    //     draw_aabb();
    // }
#endif
}

void render_ui(user_input* input)
{
    if(game.replay_mode) {
        do_replay_export_menu(input, vkon.frame_time);
        return;
    }
    if(game.pause_menu) {
        if(do_pause_menu(input, vkon.frame_time)) game.pause_menu = false;
        return;
    }

    vkon.time += vkon.frame_time;

    draw_circle(pad_3(input->mouse), 0.01, {1,1,1,1});

    if(!game.boss_spawned) {
        font_info font = vkon.default_font;
        if(game.boss_selected) font = vkon.symbols_font;
        draw_text(game.current_string, {0,0.85}, {1,1,1,1}, {0,0}, font);
    }

    do_game_gui(input);

    static real smoothed_frame_time = 1.0;
    smoothed_frame_time = lerp(smoothed_frame_time, vkon.frame_time, 0.03);
    if(settings.show_fps)
    {
        char frame_time_text[1024];
        sprintf(frame_time_text, "%2.1f ms\n%2.1f fps", smoothed_frame_time*1000.0f, 1.0f/smoothed_frame_time);

        draw_text(frame_time_text, {-vkon.aspect_ratio+0.05, 0.95}, {1,1,1,1}, {-1,1}, vkon.default_font);
    }
}

void update_uniform_buffer()
{
    uint32 f = vkon.current_frame;
    static int frame_number = 0;
    global_uniform_buffer uniforms = {
        .t = vkon.camera,
        .camera_axes = {pad_4(vkon.camera_axes[0]), pad_4(vkon.camera_axes[1]), pad_4(vkon.camera_axes[2])},
        .camera_pos = vkon.camera_pos,
        .fov = vkon.fov,
        .aspect_ratio = vkon.aspect_ratio,
        .time = vkon.time,
        .frame_number = frame_number,
        .selected = game.boss_selected ? 6 : game.selected,
        .hovered = game.hovered,
    };
    frame_number++;
    memcpy(vkon.uniform_buffers_mapped[f], &uniforms, sizeof(uniforms));
}

#endif //GAME
