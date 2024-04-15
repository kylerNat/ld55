#ifndef GAME_COMMON
#define GAME_COMMON

const real game_dt = 1.0/(120.0);

#include <time.h>

#include "gui.h"

#include "translations.h"

#include "bounding_box_2.h"

#include "misc.h"

#include "aabb_tree.h"

#include "wordnet.h"

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

//creatures are stored in the balls
struct ball {
    real r;
    real_3 base;
    real_3 p;
};

struct limb_t {
    real r;
    real segment_length;
    real_3 base;
    real_3 points[3];
    int n_points;
};

struct tentacle_t {
    real r;
    real segment_length;
    real_3 base;
    real_3 dbase;
    real_3 normal_points[8];
    real_3 points[8];
    real_3 dpoints[8];
    real attack_dir;
    int n_points;
};

enum DAMAGE_SOURCE {
    SRC_NONE,
    SRC_PLAYER,
    SRC_BOSS,
    N_DAMAGE_SOURCES
};

enum ELEMENT {
    ELE_FIRE,
    ELE_WATER,
    // ELE_EARTH,
    // ELE_AIR,
    ELE_POISON,
    ELE_LIFE,
    ELE_DEATH,
    // ELE_LIGHTNING,
    ELE_PHYSICAL,
    ELE_CHAOS,
    N_ELEMENTS
};

char element_names[][256] = {
    "fire",
    "water",
    // "earth",
    // "air",
    "poison",
    "life",
    "death",
    // "lightning",
    "physical",
    "chaos",
};

struct creature_t;

typedef void (*ability_fn)(creature_t* c);

struct ability_t {
    char name[256];
    int element;
    char description[256];
    ability_fn func;
    real cooldown;
};

enum BUFF {
    BUFF_DOUBLE,
    BUFF_HASTE,
    BUFF_CORPSE_EXPLOSION,
    BUFF_FIRE,
    BUFF_POISON,
    BUFF_FREEZE,
    BUFF_MASOCHISM,
    BUFF_ONE_SHOT_PROTECTION,
    BUFF_REFLECT,
    N_BUFFS,
};

char buff_names[][256] = {
    "again",
    "haste",
    "corpse explosion",
    "fire",
    "poison",
    "masochism",
    "one shot protection",
    "damage reflect",
};

char buff_descriptions[][256] = {
    "buffee instantly repeats their ability",
    "doubles cooldown recovery rate",
    "will explode on death",
    "this is fine",
    "deals damage proportional to the number of\nstacks every 0.25s, then removes a stack",
    "damage dealt to adjacent allies will be redirected",
    "prevents single hits from dealing more than 90% of max health",
    "direct damage taken by enemies will also be dealt back",
};

struct creature_t {
    char name[256]; //should this have a limit?

    int slot;
    real_3 p;
    quaternion orientation;

    int n_legs;
    int n_arms;
    // int n_tentacles;
    int n_eyes;

    limb_t legs[8];
    limb_t arms[8];
    real_3 arm_target_p[8];
    real_3 arm_target_v[8];
    int current_arm;
    // limb_t tentacles[8];
    ball body[2];
    ball head;
    ball eyes[4];

    real stance;
    real upright;

    real_3 albedo;
    real_3 emission;

    real speed;
    real strength;
    real tease;
    real vitality;

    real health;
    real affinities[N_ELEMENTS];

    int buffs[N_BUFFS];

    ability_t abilities[2];
    real cooldowns[2];
};

enum PROJECTILE_TYPE {
    PROJ_FIREBALL,
    PROJ_POISON,
    PROJ_FROSTBOLT,
    PROJ_HEAL,
    N_PROJECTILE_TYPES
};

struct projectile_t {
    int type;
    real strength;
    real_3 p;
    real_3 v;
    int origin;
};

struct boss_t {
    char name[256];

    bool dead;

    real r;
    real vitality;
    real health;

    quaternion orientation;

    real_3 albedo;
    real_3 emission;

    ball eyes[32];
    int n_eyes;

    tentacle_t tentacles[32];
    int n_tentacles;

    real affinities[N_ELEMENTS];

    int buffs[N_BUFFS];

    real_3 attack_point;
    real attack_t;
    real cooldown;
    real cooldown_length;

    real difficulty;
};

struct damage_number_t {
    real damage;
    real_3 p;
    real life;
    real_4 color;
};

struct explosion_t {
    real r;
    real_3 p;
    real life;
};

struct game_t {
    bool replay_mode;
    bool pause_menu;
    bool paused;
    bool gameplay_paused;

    real show_defeat;
    real show_victory;

    bool do_draw_lightprobes;
    bool do_draw_aabb;

    real_3 pos;
    real phi;
    real phi_target;
    real_3 shake;
    real_3 dshake;

    bool boss_selected;
    int selected;
    int hovered;

    bool boss_spawned;

    real filled_warning;
    real no_units_warning;

    entity* entities;
    int n_entities;

    damage_number_t* damage_numbers;
    int n_damage_numbers;

    explosion_t* explosions;
    int n_explosions;

    creature_t* creatures;
    int n_creatures;

    boss_t boss;

    projectile_t* projectiles;
    int n_projectiles;

    projectile_t* boss_projectiles;
    int n_boss_projectiles;

    creature_t* slots[5];

    creature_t spawning;

    uint seed;
    int frame_number;

    char name_string[1024];
    int n_name_string;
    char boss_string[1024];
    int n_boss_string;
    char* current_string;
    int* n_current_string;
    int key_repeat_frames;

    int speed_multiplier;
};

game_t game = {};

real_4 damage_color(int element, real damage) {
    real_4 damage_color = {1,0,0,1};
    if(element == ELE_FIRE) damage_color = {1.0f, randf(&game.seed), 0.0f, 1.0f};
    if(element == ELE_POISON) damage_color = {0.0f, 1.0f, 0.0f, 1.0f};
    damage_color.w *= clamp(0.5f+0.5f*damage, 0.5f, 1.0f);
    return damage_color;
}

void damage_boss(int element, real damage, real_3 p)
{
    damage /= game.boss.affinities[element];
    game.boss.health -= damage;
    real_4 color = damage_color(element, damage);
    color.w *= clamp(0.5f+0.5f*damage, 0.5f, 1.0f);
    game.damage_numbers[game.n_damage_numbers++] = {
        damage,
        game.boss.r*normalize(p),
        1.0f,
        color,
    };
}

void deal_damage(int source, creature_t* c, int element, real damage)
{
    creature_t* masochists[2];
    int n_masochists = 0;
    if(c->buffs[BUFF_MASOCHISM] == 0) {
        int slot = c->slot;
        int n[] = {
            (slot+1)%5,
            (slot+4)%5,
        };
        for(int i = 0; i < len(n); i++) {
            if(game.slots[n[i]]) {
                if(game.slots[n[i]]->buffs[BUFF_MASOCHISM] > 0) {
                    masochists[n_masochists++] = game.slots[n[i]];
                }
            }
        }
        for(int i = 0; i < n_masochists; i++) {
            deal_damage(source, masochists[i], element, damage/n_masochists);
        }
    }
    if(n_masochists == 0) {
        if(source == SRC_BOSS && c->buffs[BUFF_REFLECT] > 0)
            damage_boss(element, damage*c->affinities[element], c->p);
        damage /= max(0.25f, c->affinities[element]);
        if(c->buffs[BUFF_ONE_SHOT_PROTECTION] > 0) damage = min(damage, 0.9*c->vitality);
        game.damage_numbers[game.n_damage_numbers++] = {
            damage,
            c->p+0.3*rand_normal_3(&game.seed),
            1.0f,
            damage_color(element, damage),
        };
        c->health -= damage;
    }
}

#include "abilities.h"

void draw_limb(global_ssbo* ssbo, creature_t* c, limb_t* l) {
    for(int i = 1; i < l->n_points; i++) {
        real_3 u = l->points[i]-l->points[i-1];
        real_3 d = normalize(u);
        real_3 rot = {0,atan2(norm(d.xy), d.z),0};
        quaternion o = axis_to_quaternion(rot);
        rot = {0,0,atan2(d.y, d.x)};
        o = axis_to_quaternion(rot)*o;
        ssbo->primitives[ssbo->n_primitives++]  = {
            .p = apply_rotation(c->orientation, 0.5*(l->points[i-1]+l->points[i]))+c->p,
            .r = l->r,
            .size = {0,0,0.5*norm(u)},
            .type = PRIM_CAPSULE,
            .orientation = c->orientation*o,
            .albedo = c->albedo,
            .emission = {},
        };
    }
}

void draw_tentacle(global_ssbo* ssbo, boss_t* c, tentacle_t* l, real_3 status_emission) {
    for(int i = 1; i < l->n_points; i++) {
        real_3 u = l->points[i]-l->points[i-1];
        real_3 d = normalize(u);
        real_3 rot = {0,atan2(norm(d.xy), d.z),0};
        quaternion o = axis_to_quaternion(rot);
        rot = {0,0,atan2(d.y, d.x)};
        o = axis_to_quaternion(rot)*o;
        ssbo->primitives[ssbo->n_primitives++]  = {
            .p = 0.5*(l->points[i-1]+l->points[i]),
            .r = l->r,
            .size = {0,0,0.5*norm(u)},
            .type = PRIM_CAPSULE,
            .orientation = o,
            .albedo = c->albedo,
            .emission = status_emission,
        };
    }
}

void draw_boss(global_ssbo* ssbo)
{
    if(!game.boss_spawned) return;

    boss_t* b = &game.boss;

    real_3 status_emission = {};
    if(b->buffs[BUFF_POISON] > 0) status_emission += {0,0.003f,0};
    if(b->buffs[BUFF_FIRE]   > 0) status_emission += {0.003f,0.003f*(0.5f+0.5f*sin(10.0f*vkon.time)),0.0f};
    if(b->buffs[BUFF_FREEZE]   > 0) status_emission += {0.001f, 0.004f, 0.005f};

    ssbo->primitives[ssbo->n_primitives++]  = {
        .p = {},
        .r = b->r,
        .size = {0,0,0},
        .type = PRIM_SPHERE,
        .orientation = {1,0,0,0},
        .albedo = b->albedo,
        .emission = status_emission,
    };

    for(int i = 0; i < b->n_eyes; i++) {
        ball* e = b->eyes+i;
        ssbo->primitives[ssbo->n_primitives++]  = {
            .p = e->p,
            .r = e->r,
            .size = {0,0,0},
            .type = PRIM_SPHERE,
            .orientation = {1,0,0,0},
            .albedo = {},
            .emission = b->emission,
        };
    }

    for(int i = 0; i < b->n_tentacles; i++) {
        draw_tentacle(ssbo, b, b->tentacles+i, status_emission);
    }
}

void draw_creature(global_ssbo* ssbo, creature_t* c) {
    real phi = (real) c->slot*(0.4*pi);
    c->p = 12.0*(real_3){cos(phi), sin(phi)};
    c->orientation = axis_to_quaternion({0,0,phi+pi});
    if(c->n_legs > 0) {
        c->p.z = -c->body[0].r*c->legs[0].base.z+c->legs[0].segment_length*2.0f*c->upright;
        c->p.z *= 0.8f+0.2f*sin(c->speed*0.05f*game.frame_number);
        c->p.z = max(c->p.z, c->body[0].r);
    } else
        c->p.z = c->body[0].r;

    c->head.p = c->head.base;
    c->body[0].p = c->body[0].base;
    c->body[1].p = c->body[1].base;

    for(int i = 0; i < c->n_eyes; i++) {
        c->eyes[i].p = c->eyes[i].base + c->head.p;
    }
    for(int i = 0; i < c->n_legs; i++) {
        limb_t* l = c->legs+i;
        real length = 0.0;
        real_3 radial = l->base;
        radial.z = 0.0f;
        radial.xy = normalize_or_zero(radial.xy);
        l->points[0] = l->base*c->body[1].r + c->body[0].p;
        for(int j = 1; j < l->n_points-1; j++) {
            l->points[j] = l->segment_length*j*radial + l->points[0];
            l->points[j].z += l->segment_length;
        }

        real leg_length = 2.0f*l->segment_length;
        l->points[2] = l->points[0];
        l->points[2].xy += (clamp(c->stance, 0.0, 1.0)
                            *sqrt(sq(leg_length)-sq(leg_length*c->upright-l->r)))*radial.xy;
        l->points[2].z = -c->p.z+l->r;
        //dumb but easy ik loop
        for(int i = 0; i < 8; i++) {
            l->points[1] = lerp(l->points[1],
                                l->segment_length*normalize(l->points[1]-l->points[0])+l->points[0],
                                0.25);
            l->points[1] = lerp(l->points[1],
                                l->segment_length*normalize(l->points[1]-l->points[2])+l->points[2],
                                0.25);
        }
    }

    for(int i = 0; i < c->n_arms; i++) {
        limb_t* l = c->arms+i;
        real total_length = l->segment_length*l->n_points;
        for(int j = 0; j < l->n_points; j++) {
            l->points[j] = (l->segment_length*j+c->body[1].r)*l->base + c->body[1].p;
        }
        real_3 d = c->arm_target_p[i]-l->points[0];
        d = radius_min(d, total_length);
        l->points[2] = l->points[0]+d;
        //dumb but easy ik loop
        for(int i = 0; i < 8; i++) {
            l->points[1] = lerp(l->points[1],
                                l->segment_length*normalize(l->points[1]-l->points[0])+l->points[0],
                                0.25);
            l->points[1] = lerp(l->points[1],
                                l->segment_length*normalize(l->points[1]-l->points[2])+l->points[2],
                                0.25);
        }
    }

    ssbo->primitives[ssbo->n_primitives++]  = {
        .p = apply_rotation(c->orientation, c->head.p)+c->p,
        .r = c->head.r,
        .size = {0,0,0},
        .type = PRIM_SPHERE,
        .orientation = {1,0,0,0},
        .albedo = c->albedo,
        .emission = {},
    };
    for(int i = 0; i < len(c->body); i++) {
        ssbo->primitives[ssbo->n_primitives++]  = {
            .p = apply_rotation(c->orientation, c->body[i].p)+c->p,
            .r = c->body[i].r,
            .size = {0,0,0},
            .type = PRIM_SPHERE,
            .orientation = {1,0,0,0},
            .albedo = c->albedo,
            .emission = {},
        };
    }
    for(int i = 0; i < c->n_eyes; i++) {
        ssbo->primitives[ssbo->n_primitives++]  = {
            .p = apply_rotation(c->orientation, c->eyes[i].p)+c->p,
            .r = c->eyes[i].r,
            .size = {0,0,0},
            .type = PRIM_SPHERE,
            .orientation = {1,0,0,0},
            .albedo = {},
            .emission = c->emission,
        };
    }
    for(int i = 0; i < c->n_legs; i++) {
        draw_limb(ssbo, c, c->legs+i);
    }
    for(int i = 0; i < c->n_arms; i++) {
        draw_limb(ssbo, c, c->arms+i);
    }
}

void init_creature(creature_t* c, char* name) {
    *c = {};
    memcpy(c->name, name, 256);

    c->speed = max(get_resonance(name, "speed"), 0.05f);
    c->strength = max(get_resonance(name, "strength"), 0.05f);
    c->tease = max(get_resonance(name, "tease"), 0.05f);
    c->vitality = max(100.0f*get_resonance(name, "vitality"), 1.0f);
    c->health = c->vitality;

    real wide = clamp(0.4*get_resonance(name, "wide"), 0.1f, 1.5f);
    real thicc = clamp(0.1f*get_resonance(name, "thick"), 0.1f, 0.5f);
    real tall = wide+clamp(0.8f*get_resonance(name, "tall"), 0.1f, 1.5f);
    real gangly = clamp(get_resonance(name, "gangly"), 0.5f, 1.5f);
    real lanky = clamp(get_resonance(name, "lanky"), 0.5f, 1.5f);
    real head = clamp(0.5f*get_resonance(name, "head"), 0.1f, 1.0f);
    real eye = clamp(0.05f*get_resonance(name, "eye"), 0.1f, 0.3f*head);
    real eye_spacing = clamp(0.1f*get_resonance(name, "pupilary"), 0.2f, 3.0f);
    c->stance = clamp(0.5f*get_resonance(name, "stance"), 0.1f, 1.0f);
    c->upright = clamp(0.2f*get_resonance(name, "upright"), 0.5f, 1.0f);

    c->n_eyes = clamp_int((2.0f*get_resonance(name, "eyes")), 1, len(c->eyes));
    if(get_resonance(name, "eyeless") > 3.0) c->n_eyes = 0;
    c->n_legs = clamp_int((3.0f*get_resonance(name, "legs")), 2, len(c->legs));
    c->n_arms = clamp_int((2.0f*get_resonance(name, "arms")), 0, len(c->arms));
    if(c->n_arms%2 == 1) c->n_arms--;

    c->body[0]  = {
        .r = wide,
        .base = {},
    };

    c->body[1]  = {
        .r = 0.5*max(tall-wide,0.01f),
        .base = {0,0,0.5*(wide+tall)},
    };

    c->head = {
        .r = head,
        .base = {0,0,tall},
    };

    for(int i = 0; i < c->n_eyes; i++) {
        real vertical = 2.0f*eye_spacing*(i/2)-((c->n_eyes-1)/2)*eye_spacing;
        real horizontal = i%2 ? -eye_spacing : eye_spacing;
        if(i == c->n_eyes-1 && i%2 == 0) horizontal = 0.0f;
        real_3 eye_dir = normalize((real_3){1.0f, horizontal, vertical});
        c->eyes[i] = {
            .r = eye,
            .base = head*eye_dir,
        };
    }

    real arm_spacing = 1.0f/max(c->n_arms/2, 1);
    for(int i = 0; i < c->n_arms; i++) {
        limb_t* l = c->arms+i;

        real vertical = arm_spacing*(i/2)-((c->n_arms-1)/2)*0.5f*arm_spacing;
        real horizontal = i%2 ? -1.0f : 1.0f;

        if(i == c->n_arms-1 && i%2 == 0)
            l->base = {1.0f, 0.0f, 0.0f};
        else
            l->base = {0.0f, horizontal, vertical};
        l->base = normalize(l->base);
        l->r = clamp(0.2f*c->strength, 0.1f, 0.8f);
        l->segment_length = gangly;
        l->n_points = 3;
    }

    real leg_spacing = 0.5f;
    if(c->n_legs == 1) leg_spacing = 0.0f;
    for(int i = 0; i < c->n_legs; i++) {
        limb_t* l = c->legs+i;
        real phi = i*2.0f*pi/c->n_legs;
        if(c->n_legs%2 == 0) phi += 0.5*pi;
        else                 phi += pi;
        l->base = {leg_spacing*cos(phi), leg_spacing*sin(phi), -1.0f};
        l->base = normalize(l->base);
        l->r = clamp(0.2f*c->strength, 0.1f, 0.8f);
        l->segment_length = lanky;
        l->n_points = 3;
    }

    char color_names[][256] = {
        "white",
        "black",
        "red",
        "orange",
        "yellow",
        "green",
        "teal",
        "blue",
        "violet",
        "pink",
    };
    real_3 colors[] = {
        {1.0,1.0,1.0},
        {0.0,0.0,0.0},
        {1.0,0.0,0.0},
        {0.75,0.25,0.0},
        {1.0,1.0,0.0},
        {0.0,1.0,0.0},
        {0.0,0.5,0.5},
        {0.0,0.0,1.0},
        {0.5,0.0,1.0},
        {1.0,0.0,1.0},
    };
    assert(len(colors) == len(color_names));
    real_4 color = {};
    for(int i = 0; i < len(color_names); i++) {
        real weight = exp(get_resonance(name, color_names[i]));
        // log_output(name, "'s ", color_names[i], " weight = ", weight, "\n")
        color += pad_4(weight*colors[i], weight);
    }

    c->emission = {1,1,1};
    if(color.w > 0.0)
        c->albedo = color.xyz/color.w;
    else
        c->albedo = {};
    // log_output(name, "'s color = ", c->albedo, "\n");

    for(int i = 0; i < N_ELEMENTS; i++) {
        c->affinities[i] = get_resonance(name, element_names[i]);
    }
    c->affinities[ELE_PHYSICAL] *= c->strength*(c->n_arms/2);

    real highest_ability_score = 0.0;
    int ability_index = 0;
    real highest_ability_score2 = 0.0;
    int ability_index2 = 0;
    for(int i = 0; i < len(ability_list); i++) {
        real score = exp(2.0f*c->affinities[ability_list[i].element])*get_resonance(name, ability_list[i].name);
        if(score > highest_ability_score) {
            highest_ability_score2 = highest_ability_score;
            ability_index2 = ability_index;
            highest_ability_score = score;
            ability_index = i;
        } else if(score > highest_ability_score2) {
            highest_ability_score2 = score;
            ability_index2 = i;
        }
    }
    c->abilities[0] = ability_list[ability_index];
    c->abilities[1] = ability_list[ability_index2];
    update_cooldowns(c);
}

void init_boss(char* name)
{
    memcpy(game.boss.name, name, 256);

    game.boss.dead = false;

    uint32 seed = djb2(name);
    game.boss.r = 4.0;
    game.boss.albedo = randf_3(&seed, {0,0,0}, {1,1,1});
    game.boss.emission = randf_3(&seed, {0.0f,0.0f,0.0f}, {1,1,1});
    game.boss.cooldown_length = clamp(80.0f+20.0f*rand_normal(&seed), 40.0f, 120.0f);
    game.boss.n_eyes = 16;
    game.boss.n_tentacles = 16;
    for(int i = 0; i < game.boss.n_eyes; i++) {
        game.boss.eyes[i] = {
            .r = randf(&seed, 0.1f, 0.5f),
            .p = game.boss.r*normalize_or_zero(rand_normal_3(&seed)),
        };
    }
    for(int i = 0; i < game.boss.n_tentacles; i++) {
        tentacle_t* e = game.boss.tentacles+i;
        *e = {
            .r = randf(&seed, 0.1f, 0.3f),
            .segment_length = randf(&seed, 0.5f, 1.0f),
            .base = game.boss.r*normalize_or_zero(rand_normal_3(&seed)),
            .n_points = 8,
        };
        // e->points[0] = e->base;
        // for(int j = 1; j < e->n_points; j++) {
        //     e->points[j] = e->points[j-1]+e->base;
        // };
    }

    for(int i = 0; i < N_BUFFS; i++) {
        game.boss.buffs[i] = 0;
    }

    for(int i = 0; i < N_ELEMENTS; i++) {
        game.boss.affinities[i] = clamp(100.0f+50.0f*rand_normal(&seed), 50.0f, 200.0f)/100.0f;
    }
    game.boss.vitality = clamp(25000.0f+25000.0f*rand_normal(&seed), 5000.0f, 50000.0f);
    game.boss.health   = game.boss.vitality;

    game.boss.difficulty = 1.0f;
    for(int i = 0; i < N_ELEMENTS; i++) {
        game.boss.difficulty += game.boss.affinities[i];
    }
    game.boss.difficulty *= game.boss.vitality;
    game.boss.difficulty /= game.boss.cooldown_length;
}

void update_boss()
{
    if(!game.boss_spawned) return;
    boss_t* b = &game.boss;

    real slow = 1.0f;
    for(int i = 0; i < N_BUFFS; i++) {
        if(b->buffs[i] > 0) {
            if(i == BUFF_FIRE) {
                damage_boss(ELE_FIRE, 1.0f, 0.2f*rand_normal_3(&game.seed)+(real_3){0,0,1.0f});
            }
            if(i == BUFF_POISON) {
                if(game.frame_number%30 != 0) continue;
                damage_boss(ELE_POISON, 0.1f*b->buffs[BUFF_POISON], {0,0,1});
            }
            if(i == BUFF_FREEZE) {
                b->cooldown += 0.75f;
                slow = 0.25f;
            }
            b->buffs[i]--;
        }
    }

    for(int i = 0; i < b->n_eyes; i++) {
        ball* e = b->eyes+i;
        e->base += 0.002f*rand_normal_3(&game.seed);
        if(e->p.z < 0.0f) e->base.z += 0.01f;
        e->base = radius_min(e->base, 0.02f);
        e->base *= 0.999f;
        e->p += slow*e->base;
        e->p = b->r*normalize_or_zero(e->p);
    }

    real_3 attack_dir = normalize(b->attack_point);

    bool attacking = false;
    if(!b->dead) {
        b->cooldown -= 1.0f;
        if(b->cooldown <= 0.0f) {
            b->cooldown += b->cooldown_length;
            real total_weight = 0.0;
            for(int i = 0; i < 5; i++) {
                if(game.slots[i]) {
                    total_weight += game.slots[i]->tease;
                }
            }
            real sweight = randf(&game.seed, 0.0f, total_weight);
            total_weight = 0.0f;
            for(int i = 0; i < 5; i++) {
                if(game.slots[i]) {
                    total_weight += game.slots[i]->tease;
                    if(sweight < total_weight) {
                        b->attack_point = game.slots[i]->p;
                        b->attack_t = 20.0f;
                        int element = rand(&game.seed, 0, N_ELEMENTS-1); //don't do chaos
                        real damage = 20.0f*b->affinities[element];
                        deal_damage(SRC_BOSS, game.slots[i], element, damage);
                        play_sound(&tentacle_hit_sound, 1.0);
                        break;
                    }
                }
            }
        }
        attacking = b->attack_t>0.0f;
        b->attack_t = max(b->attack_t-1.0f, 0.0f);
    }

    for(int i = 0; i < b->n_tentacles; i++) {
        tentacle_t* e = b->tentacles+i;
        if(!b->dead) {
            e->dbase += 0.001f*rand_normal_3(&game.seed);
        }
        if(e->base.z < 0.0f) e->dbase.z += 0.01f;
        e->dbase = radius_min(e->dbase, 0.01f);
        e->dbase *= 0.999f;
        e->base += slow*e->dbase;
        real_3 n = normalize_or_zero(e->base);
        e->base = b->r*n;
        e->points[0] = e->base;
        e->normal_points[0] = e->base;
        e->dpoints[0] = e->dbase;
        for(int j = 1; j < e->n_points; j++) {
            // e->normal_points[j] = e->normal_points[j-1]+normalize_or_zero(e->normal_points[j])*e->segment_length*j;
            if(!b->dead) {
                e->dpoints[j] += 0.002f*rand_normal_3(&game.seed);
                e->dpoints[j] += 0.00002f*n;
            } else {
                if(e->normal_points[j].z > 0.0f) e->dpoints[j].z -= 0.002f;
                e->dpoints[j] += 0.00002f*n;
            }
            e->dpoints[j] = radius_min(e->dpoints[j], 0.1f);
            e->dpoints[j] = lerp(e->dpoints[j], e->dpoints[j-1], 0.05f);
            if(j < e->n_points-1) e->dpoints[j] = lerp(e->dpoints[j], e->dpoints[j+1], 0.05f);
            e->normal_points[j] += slow*e->dpoints[j];
            e->normal_points[j] = e->segment_length*normalize_or_zero(e->normal_points[j]-e->normal_points[j-1])+e->normal_points[j-1];

            real_3 target = e->normal_points[j];
            if(attacking) target = lerp(target, b->attack_point, max(dot(n, attack_dir), 0.0f));
            e->points[j] = lerp(e->points[j], target, 0.1f+0.1f*attacking);

            e->points[j] = e->segment_length*normalize_or_zero(e->points[j]-e->points[j-1])+e->points[j-1];
        }
    }
}

void update_creatures()
{
    for(int i = 0; i < 5; i++) {
        game.slots[i] = 0;
    }
    for(int i = 0; i < game.n_creatures; i++) {
        creature_t* c = game.creatures+i;
        if(c->slot >= 0 && c->slot < 5)
            game.slots[c->slot] = c;
    }

    for(int i = 0; i < game.n_creatures; i++) {
        creature_t* c = game.creatures+i;
        for(int j = 0; j < 2; j++) {
            c->cooldowns[j] -= 1.0f;
            while(c->cooldowns[j] <= 0.0f) {
                if(c->abilities[j].func) {
                    c->abilities[j].func(c);
                    c->cooldowns[j] += c->abilities[j].cooldown;
                }
            }
        }
    }

    for(int i = game.n_creatures-1; i >= 0; i--) {
        creature_t* c = game.creatures+i;
        if(c->health <= 0.0f) {
            if(c->buffs[BUFF_CORPSE_EXPLOSION] > 0) {
                real damage = 0.05f*c->buffs[BUFF_CORPSE_EXPLOSION]*c->vitality;
                damage_boss(ELE_DEATH, damage, c->p);
                real r = clamp(0.1f*damage, 4.0f, 7.0f);
                for(int i = 0; i < 10; i++) {
                    game.explosions[game.n_explosions++] = {
                        .r = 1.0+r*randf(&game.seed),
                        .p = c->p+0.5f*r*rand_normal_3(&game.seed),
                        .life = 1.0f,
                    };
                }
                play_sound(&explosion2_sound, 1.0);
                game.dshake += r*rand_normal_3(&game.seed);
            }
            *c = game.creatures[--game.n_creatures];
        }
    }

    for(int i = 0; i < game.n_creatures; i++) {
        creature_t* c = game.creatures+i;
        for(int j = 0; j < c->n_arms; j++) {
            limb_t* l = c->arms+j;
            real_3 p = (l->segment_length*1.0+c->body[1].r)*l->base + c->body[1].p;
            p.x += l->segment_length;
            c->arm_target_p[j] += c->arm_target_v[j];
            c->arm_target_v[j] += 0.01*(p-c->arm_target_p[j]);
            c->arm_target_v[j] *= 0.9;
        }

        if(c->health > c->vitality)
            c->health = 0.995f*(c->health-c->vitality)+c->vitality;

        for(int j = 0; j < N_BUFFS; j++) {
            if(c->buffs[j] > 0) {
                if(j == BUFF_HASTE) for(int k = 0; k < 2; k++)
                                        c->cooldowns[k] -= 1.0f;
                if(j == BUFF_DOUBLE) for(int k = 0; k < 2; k++)
                                         c->cooldowns[k] -= c->abilities[k].cooldown;
                if(j == BUFF_FIRE) deal_damage(SRC_NONE, c, ELE_FIRE, 0.1f);
                c->buffs[j]--;
            }
        }
    }

    for(int i = game.n_projectiles-1; i >= 0; i--) {
        projectile_t* p = game.projectiles+i;
        p->p += p->v;
        switch(p->type) {
            case PROJ_FIREBALL: {
                if(normsq(p->p) < sq(game.boss.r)) {
                    damage_boss(ELE_FIRE, p->strength, p->p);
                    game.boss.buffs[BUFF_FIRE] = max(game.boss.buffs[BUFF_FIRE], (int) (300*p->strength));
                    for(int i = 0; i < 10; i++) {
                        game.explosions[game.n_explosions++] = {
                            .r = 1.0+1.0f*p->strength*randf(&game.seed),
                            .p = p->p+0.3f*p->strength*rand_normal_3(&game.seed),
                            .life = 1.0f,
                        };
                    }
                    play_sound(&explosion_sound, 1.0f);
                    *p = game.projectiles[--game.n_projectiles];
                };
            } break;
            case PROJ_POISON: {
                if(normsq(p->p) < sq(game.boss.r)) {
                    game.boss.buffs[BUFF_POISON] += 10*p->strength;
                    *p = game.projectiles[--game.n_projectiles];
                };
            } break;
            case PROJ_FROSTBOLT: {
                if(normsq(p->p) < sq(game.boss.r)) {
                    game.boss.buffs[BUFF_FREEZE] += 120*p->strength;
                    *p = game.projectiles[--game.n_projectiles];
                    play_sound(&freeze_sound, 1.0f);
                };
            } break;
            case PROJ_HEAL: {
                for(int j = 0; j < 5; j++) {
                    if(j != p->origin && game.slots[j] && normsq(game.slots[j]->p.xy-p->p.xy) < sq(1.0)) {
                        game.slots[j]->health += 50.0f*p->strength;
                        *p = game.projectiles[--game.n_projectiles];
                        play_sound(&heal_sound, 1.0f);
                        break;
                    };
                }
            } break;
        }
    }

    update_boss();
}

void draw_projectile(global_ssbo* ssbo, projectile_t* p) {
    switch(p->type) {
        case PROJ_FIREBALL: {
            real radius = pow(p->strength, 1.0f/3.0f);
            ssbo->primitives[ssbo->n_primitives++]  = {
                .p = p->p,
                .r = radius,
                .size = {},
                .type = PRIM_SPHERE,
                .orientation = {1,0,0,0},
                .albedo = {0,0,0},
                .emission = {1.0,1.0,0.0},
                .texture_type = TEX_FIREBALL,
            };
        } break;
        case PROJ_POISON: {
            real radius = pow(p->strength, 1.0f/3.0f);
            ssbo->primitives[ssbo->n_primitives++]  = {
                .p = p->p,
                .r = radius,
                .size = {},
                .type = PRIM_SPHERE,
                .orientation = {1,0,0,0},
                .albedo = {0.2,0.5,0.2},
                .emission = {0.0,0.2+0.2*cos(vkon.time),0.0},
                .texture_type = TEX_FIREBALL,
            };
        } break;
        case PROJ_FROSTBOLT: {
            real radius = pow(p->strength, 1.0f/3.0f);
            ssbo->primitives[ssbo->n_primitives++]  = {
                .p = p->p,
                .r = radius,
                .size = {},
                .type = PRIM_SPHERE,
                .orientation = {1,0,0,0},
                .albedo = {0.2,0.5,0.5},
                .emission = {0.0,0.5+0.2*cos(vkon.time),1.0},
                // .texture_type = TEX_FIREBALL,
            };
        } break;
        case PROJ_HEAL: {
            real radius = pow(p->strength, 1.0f/3.0f);
            ssbo->primitives[ssbo->n_primitives++]  = {
                .p = p->p,
                .r = radius,
                .size = {},
                .type = PRIM_SPHERE,
                .orientation = {1,0,0,0},
                .albedo = {0.1,0.5,0.7},
                .emission = (real_3){0.2f,0.9f,1.0f}*(0.7+0.2*cos(vkon.time)),
                // .texture_type = TEX_FIREBALL,
            };
        } break;
        default: {
            real radius = pow(p->strength, 1.0f/3.0f);
            ssbo->primitives[ssbo->n_primitives++]  = {
                .p = p->p,
                .r = radius,
                .size = {},
                .type = PRIM_SPHERE,
                .orientation = {1,0,0,0},
                .albedo = {0,0,0},
                .emission = {1.0,1.0,1.0},
            };
        } break;
    }
}

void draw_explosion(global_ssbo* ssbo, explosion_t* e) {
    real_3 color = {};
    color = lerp(color, ((real_3){1,1,1}), smoothstep(0.0f, 0.8f, e->life));
    color = lerp(color, ((real_3){1,0,0}), smoothstep(0.8f, 1.0f, e->life));
    real radius = e->r*e->life;
    ssbo->primitives[ssbo->n_primitives++]  = {
        .p = e->p,
        .r = radius,
        .size = {},
        .type = PRIM_SPHERE,
        .orientation = {1,0,0,0},
        .albedo = {0.1,0.1,0.1},
        .emission = color,
        // .texture_type = TEX_FIREBALL,
    };
}

real_2 project_to_screen(real_3 p)
{
    real_3 d = p-vkon.camera_pos;
    d = d*vkon.camera_axes;
    real screen_dist = 1.0f/tan(0.5f*vkon.fov);
    return d.xy*(screen_dist/d.z);
}

void do_game_gui(user_input* input)
{
    if(!game.boss_spawned && game.n_creatures == 0) {
        real_2 p = {0.0,0.0};
        char text[] = "Type a True Name to summon";
        draw_text(text, p, gui.foreground_color, {0,0}, vkon.default_font);
    }

    char tooltip_text[1024] = "";
    real_2 tooltip_pos = {};
    real_2 tooltip_align = {};
    char text[1024] = "";
    int slot = game.selected;
    if(game.hovered >= 0 && game.slots[game.hovered]) slot = game.hovered;
    if(game.boss_selected) slot = 6;
    creature_t* c = 0;
    if(0 <= slot && slot < 5) c = game.slots[slot];
    if(c == 0 && slot != 6 && game.n_name_string > 0) c = &game.spawning;
    real spacing = 0.12;
    if(c) {
        real_2 p = {vkon.aspect_ratio-0.1, -0.9};
        draw_text(c->name, p, gui.foreground_color, {1,0}, vkon.default_font);
        p.y += spacing;
        sprintf(text, "health: %.1f/%.1f", c->health, c->vitality);
        draw_text(text, p, gui.foreground_color, {1,0}, vkon.default_font);
        p.y += spacing;
        sprintf(text, "speed: %.2f", c->speed);
        draw_text(text, p, gui.foreground_color, {1,0}, vkon.default_font);
        p.y += spacing;
        sprintf(text, "taunt: %.2f", c->tease);
        draw_text(text, p, gui.foreground_color, {1,0}, vkon.default_font);
        // p.y += spacing;
        // sprintf(text, "strength: %.2f", c->strength);
        // draw_text(text, p, gui.foreground_color, {1,0}, vkon.default_font);
        p.y += 1.5*spacing;
        sprintf(text, "abilities - cooldown:");
        draw_text(text, p, gui.foreground_color, {1,0}, vkon.default_font);
        p.y += 1.5*spacing;
        for(int i = 0; i < 2; i++) {
            sprintf(text, "%s - %.1fs", c->abilities[i].name, c->abilities[i].cooldown/120.0f);
            draw_text(text, p, gui.foreground_color, {1,0}, vkon.default_font);
            real_2 s = 0.5f*get_text_size(text, vkon.default_font);
            real_2 d = p-input->mouse;
            d.x -= s.x;
            d = abs_per_axis(d);
            if(d.x < s.x && d.y < s.y) {
                real_2 q = input->mouse;
                q.y += 0.15;
                sprintf(tooltip_text, "%s", c->abilities[i].description);
                tooltip_pos = q;
                tooltip_align = {1,0};
            }
            p.y += spacing;
        }

        p.y += spacing;
        draw_text("status effects:", p, gui.foreground_color, {1,0}, vkon.default_font);
        p.y += 1.5*spacing;
        int hovered_buff = -1;
        for(int i = 0; i < N_BUFFS; i++) {
            if(c->buffs[i] > 0) {
                sprintf(text, "%s: %.0fs", buff_names[i], c->buffs[i]/120.0f);
                draw_text(text, p, gui.foreground_color, {1,0}, vkon.default_font);
                real_2 s = 0.5f*get_text_size(text, vkon.default_font);
                real_2 d = p-input->mouse;
                d.x -= s.x;
                d = abs_per_axis(d);
                if(d.x < s.x && d.y < s.y) {
                    real_2 q = input->mouse;
                    q.x -= 0.15;
                    sprintf(tooltip_text, "%s", buff_descriptions[i]);
                    tooltip_pos = q;
                    tooltip_align = {1,0};
                }

                p.y += spacing;
            }
        }

        p = {-vkon.aspect_ratio+0.1, -0.9};

        draw_text("affinities:", p, gui.foreground_color, {-1,0}, vkon.default_font);
        real_2 s = 0.5f*get_text_size("affinities:", vkon.default_font);
        real_2 d = p-input->mouse;
        d.x += s.x;
        d = abs_per_axis(d);
        if(d.x < s.x && d.y < s.y) {
            real_2 q = input->mouse;
            q.x += 0.15;
            sprintf(tooltip_text, "affinities affect the potency of matching\nabilites and resistance against damage types");
            tooltip_pos = q;
            tooltip_align = {-1,0};
        }

        p.y += 1.5*spacing;
        p.x += 0.5*spacing;
        for(int i = 0; i < N_ELEMENTS; i++) {
            sprintf(text, "%s: %.2f", element_names[i], c->affinities[i]);
            draw_text(text, p, gui.foreground_color, {-1,0}, vkon.default_font);
            p.y += spacing;
        }
        p.x -= 0.5*spacing;
        p.y += spacing;

        //do tooltips last
        if(tooltip_text[0] != 0) {
            real_2 t = 0.5f*get_text_size(tooltip_text, vkon.default_font);
            tooltip_pos.y += t.y;
            real_2 r = tooltip_pos;
            r.x -= t.x*tooltip_align.x;
            draw_rectangle(pad_3(r), t+0.06, gui.foreground_color);
            draw_rectangle(pad_3(r), t+0.05, gui.background_color);
            draw_text(tooltip_text, tooltip_pos, gui.foreground_color, tooltip_align, vkon.default_font);
        }
    }

    if(slot == 6 && game.boss.name[0]) {
        boss_t* c = &game.boss;
        real_2 p = {vkon.aspect_ratio-0.1, -0.9};
        draw_text(c->name, {0.0, -0.9}, gui.foreground_color, {0,0}, vkon.symbols_font);
        p.y += spacing;
        sprintf(text, "health: %.1f/%.1f", c->health, c->vitality);
        draw_text(text, p, gui.foreground_color, {1,0}, vkon.default_font);
        p.y += spacing;
        sprintf(text, "attack cooldown: %.2f", c->cooldown_length);
        draw_text(text, p, gui.foreground_color, {1,0}, vkon.default_font);
        p.y += 1.5f*spacing;
        sprintf(text, "overall difficulty: %.0f", c->difficulty/10.0f);
        draw_text(text, p, gui.foreground_color, {1,0}, vkon.default_font);

        p = {-vkon.aspect_ratio+0.1, -0.9};

        draw_text("affinities:", p, gui.foreground_color, {-1,0}, vkon.default_font);
        real_2 s = 0.5f*get_text_size("affinities:", vkon.default_font);
        real_2 d = p-input->mouse;
        d.x += s.x;
        d = abs_per_axis(d);
        if(d.x < s.x && d.y < s.y) {
            real_2 q = input->mouse;
            q.x += 0.15;
            sprintf(tooltip_text, "affinities affect the potency of matching\nabilites and resistance against damage types");
            tooltip_pos = q;
            tooltip_align = {-1,0};
        }

        p.y += 1.5*spacing;
        p.x += 0.5*spacing;
        for(int i = 0; i < N_ELEMENTS; i++) {
            sprintf(text, "%s: %.2f", element_names[i], c->affinities[i]);
            draw_text(text, p, gui.foreground_color, {-1,0}, vkon.default_font);
            p.y += spacing;
        }
        p.x -= 0.5*spacing;
        p.y += spacing;

        //do tooltips last
        if(tooltip_text[0] != 0) {
            real_2 t = 0.5f*get_text_size(tooltip_text, vkon.default_font);
            tooltip_pos.y += t.y;
            real_2 r = tooltip_pos;
            r.x -= t.x*tooltip_align.x;
            draw_rectangle(pad_3(r), t+0.06, gui.foreground_color);
            draw_rectangle(pad_3(r), t+0.05, gui.background_color);
            draw_text(tooltip_text, tooltip_pos, gui.foreground_color, tooltip_align, vkon.default_font);
        }
    }

    //health bars
    for(int i = 0; i < game.n_creatures; i++) {
        creature_t* c = game.creatures+i;
        real_3 uv = {};
        uv.xy = project_to_screen(c->p+c->head.p);
        uv.y -= 0.1f;
        real health = max(c->health, 0.0f);
        draw_rectangle(uv, {0.001*health, 0.01}, {0,0.5,0.7,1});
        draw_rectangle(uv, {0.001*c->vitality, 0.01}, {1,0,0,1});
        draw_rectangle(uv, {0.001*min(health, c->vitality), 0.01}, {1,1,1,1});

        if(!game.boss_spawned) {
            real_3 feet = c->p;
            feet.z = 0.0f;
            real_2 p = project_to_screen(feet);
            p.y += 0.1f;
            draw_text(c->name, p, {1,1,1,1}, {0,0}, vkon.default_font);
        }
    }

    if(game.boss.name[0]) {
        boss_t* c = &game.boss;
        real_3 uv = {};
        real_3 p = {0,0,c->r};
        uv.xy = project_to_screen(p);
        uv.y -= 0.1f;
        real health = max(c->health, 0.0f);
        draw_rectangle(uv, {0.00002*health, 0.01}, {0,0.5,0.7,1});
        draw_rectangle(uv, {0.00002*c->vitality, 0.01}, {1,0,0,1});
        draw_rectangle(uv, {0.00002*min(health, c->vitality), 0.01}, {1,1,1,1});
    }

    //damage numbers
    draw_text_batched_start();
    for(int i = game.n_damage_numbers-1; i >= 0; i--) {
        damage_number_t* dn = game.damage_numbers+i;
        real_2 p = project_to_screen(dn->p);
        sprintf(text, "%.1f", dn->damage);
        if(dn->color == (real_4){}) dn->color = {1,0,0,1};
        draw_text_batched(text, p, dn->color*dn->life, {0,0}, vkon.default_font);
    }
    draw_text_batched_end();

    if(game.filled_warning > 0.0f) {
        float alpha = smoothstep(0.0, 0.2, game.filled_warning);
        char text[] = "can't summon into a full slot";
        real_2 s = get_text_size(text, vkon.default_font);
        real_2 p = {0.0,0.6};
        draw_rectangle(pad_3(p), s+0.05f, {0,0,0,0.1});
        draw_text(text, p, gui.foreground_color*alpha, {0,0}, vkon.default_font);
        game.filled_warning -= vkon.frame_time;
    }

    if(game.filled_warning > 0.0f) {
        float alpha = smoothstep(0.0, 0.2, game.filled_warning);
        char text[] = "can't summon into a full slot";
        real_2 s = get_text_size(text, vkon.default_font);
        real_2 p = {0.0,0.6};
        draw_rectangle(pad_3(p), s+0.05f, {0,0,0,0.1});
        draw_text(text, p, gui.foreground_color*alpha, {0,0}, vkon.default_font);
        game.filled_warning -= vkon.frame_time;
    }

    if(game.no_units_warning > 0.0f) {
        float alpha = smoothstep(0.0, 0.2, game.no_units_warning);
        char text[] = "you must summon at least one fighter\nbefore summoning any eldritch entities";
        // real_2 s = get_text_size(text, vkon.default_font);
        real_2 p = {0.0,0.0};
        // draw_rectangle(pad_3(p), s+0.05f, {0,0,0,0.5});
        draw_text(text, p, gui.foreground_color*alpha, {0,0}, vkon.default_font);
        game.no_units_warning -= vkon.frame_time;
    }

    if(game.show_defeat > 0.0f) {
        float alpha = smoothstep(0.0, 0.2, game.show_defeat);
        game.show_defeat -= vkon.frame_time;
        real_2 p = {0.0,0.0};
        draw_rectangle(pad_3(p), {vkon.aspect_ratio, 1.0}, {0,0,0,0.8*alpha});
        char text[] = "Defeat";
        draw_text(text, p, gui.foreground_color*alpha, {0,0}, vkon.default_font);
        if(is_down(M1, input) || is_down(VK_RETURN, input) || is_down(VK_LEFT, input) || is_down(VK_RIGHT, input) || is_down(VK_UP, input) || is_down(VK_DOWN, input))
            game.show_defeat = 0.0f;
        if(game.show_defeat <= 0.0f) game.boss_spawned = false;
    }

    if(game.show_victory > 0.0f) {
        float alpha = smoothstep(0.0, 0.2, game.show_victory);
        real_2 p = {0.0,0.0};
        draw_rectangle(pad_3(p), {vkon.aspect_ratio, 1.0}, {0,0,0,0.8*alpha});
        char text[] = "Victory! vs.";
        draw_text(text, p, gui.foreground_color*alpha, {0,0}, vkon.default_font);
        p.y += 0.12;
        sprintf(text, "%s", game.boss.name);
        draw_text(text, p, gui.foreground_color*alpha, {0,0}, vkon.symbols_font);
        if(is_down(M1, input) || is_down(VK_RETURN, input) || is_down(VK_LEFT, input) || is_down(VK_RIGHT, input) || is_down(VK_UP, input) || is_down(VK_DOWN, input))
            game.show_victory = 0.0f;
        if(game.show_victory <= 0.0f) game.boss_spawned = false;
    }

    if(game.speed_multiplier != 1) {
        real_2 p = {0.0,-0.8};
        char text[256];
        sprintf(text, "%dx speed", game.speed_multiplier);
        draw_text(text, p, gui.foreground_color, {0,0}, vkon.default_font);
    }
}

#endif //GAME_COMMON
