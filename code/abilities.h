#ifndef ABILITIES
#define ABILITIES

void update_cooldowns(creature_t* c) {
    for(int i = 0; i < 2; i++) {
        c->abilities[i].cooldown /= c->speed;
        if(c->abilities[i].element == ELE_PHYSICAL) c->abilities[i].cooldown /= c->n_arms;
        // c->cooldowns[i] = c->abilities[i].cooldown;
        c->cooldowns[i] = 0;
    }
}

void ability_fireball(creature_t* c) {
    real_3 v = -c->p;
    v.xy = c->speed*0.05*normalize(v.xy);
    v.z = 0.0;
    game.projectiles[game.n_projectiles++] = (projectile_t) {
        .type = PROJ_FIREBALL,
        .strength = c->affinities[ELE_FIRE],
        .p = c->p,
        .v = v,
    };
}

void ability_poison(creature_t* c) {
    real_3 v = -c->p;
    v.xy = c->speed*0.05*normalize(v.xy);
    v.z = 0.0;
    game.projectiles[game.n_projectiles++] = (projectile_t) {
        .type = PROJ_POISON,
        .strength = c->affinities[ELE_POISON],
        .p = c->p,
        .v = v,
    };
}

void ability_frostbolt(creature_t* c) {
    real_3 v = -c->p;
    v.xy = c->speed*0.05*normalize(v.xy);
    v.z = 0.0;
    game.projectiles[game.n_projectiles++] = (projectile_t) {
        .type = PROJ_FROSTBOLT,
        .strength = c->affinities[ELE_WATER],
        .p = c->p,
        .v = v,
    };
}

void ability_punch(creature_t* c) {
    if(c->n_arms == 0) return; //can't punch without arms
    int i = c->current_arm;
    c->current_arm = (c->current_arm+1)%c->n_arms;
    c->arm_target_v[i] += 1.0f*c->speed*(real_3){1,0,0};
    real damage = c->affinities[ELE_PHYSICAL];
    damage_boss(ELE_PHYSICAL, damage, c->p+apply_rotation(c->orientation, c->arms[i].points[2]));
}

//buffs should be infintely stackable with enough looped buffs
void ability_double(creature_t* c) {
    int slot = c->slot;
    int n[] = {
        (slot+1)%5,
        (slot+4)%5,
    };
    for(int i = 0; i < len(n); i++) {
        if(game.slots[n[i]]) {
            game.slots[n[i]]->buffs[BUFF_DOUBLE] = 1;
        }
    }
}

// void ability_haste(creature_t* c) {
//     int slot = c->slot;
//     int n[] = {
//         (slot+1)%5,
//         (slot+4)%5,
//     };
//     for(int i = 0; i < len(n); i++) {
//         if(game.slots[n[i]]) {
//             game.slots[n[i]]->buffs[BUFF_HASTE] += 300*c->affinities[ELE_AIR];
//         }
//     }
// }

void ability_haste_fire(creature_t* c) {
    int slot = c->slot;
    int n[] = {
        (slot+1)%5,
        (slot+4)%5,
    };
    for(int i = 0; i < len(n); i++) {
        if(game.slots[n[i]]) {
            game.slots[n[i]]->buffs[BUFF_HASTE] += 300*c->affinities[ELE_FIRE];
            game.slots[n[i]]->buffs[BUFF_FIRE]  += 300*c->affinities[ELE_FIRE];
        }
    }
}

void ability_sadism(creature_t* c) {
    int slot = c->slot;
    int n[] = {
        (slot+1)%5,
        (slot+4)%5,
    };
    for(int i = 0; i < len(n); i++) {
        if(game.slots[n[i]]) {
            game.slots[n[i]]->buffs[BUFF_MASOCHISM] += 800*c->affinities[ELE_DEATH];
        }
    }
}

void ability_one_shot_protection(creature_t* c) {
    int slot = c->slot;
    int n[] = {
        (slot+1)%5,
        (slot+4)%5,
    };
    for(int i = 0; i < len(n); i++) {
        if(game.slots[n[i]]) {
            game.slots[n[i]]->buffs[BUFF_ONE_SHOT_PROTECTION] += 200*c->affinities[ELE_LIFE];
        }
    }
}

void ability_corpse_explosion(creature_t* c) {
    int slot = c->slot;
    for(int i = 0; i < 5; i++) {
        if(i != slot && game.slots[i]) {
            game.slots[i]->buffs[BUFF_CORPSE_EXPLOSION] = 50*c->affinities[ELE_DEATH];
        }
    }
}

void ability_reflect(creature_t* c) {
    c->buffs[BUFF_REFLECT] = 50;
}

void ability_heal(creature_t* c) {
    real lowest_health = INF;
    creature_t* lowest = 0;
    for(int j = 0; j < 5; j++) {
        int i = (j+c->slot)%5;
        if(i == c->slot) continue;
        if(!game.slots[i]) continue;
        if(game.slots[i]->health < lowest_health) {
            lowest_health = game.slots[i]->health;
            lowest = game.slots[i];
        }
    }
    if(lowest == 0) return;

    real_3 v = lowest->p - c->p;
    v.xy = c->speed*0.25*normalize(v.xy);
    v.z = 0.0;
    game.projectiles[game.n_projectiles++] = (projectile_t) {
        .type = PROJ_HEAL,
        .strength = c->affinities[ELE_LIFE],
        .p = c->p,
        .v = v,
        .origin = c->slot,
    };
}

void ability_chaos(creature_t* c);

ability_t ability_list[] = {
    {"chaos", ELE_CHAOS, "performs a random ability", ability_chaos, 600.0}, //put chaos first so it's used when the search gets all zero weights
    {"fireball", ELE_FIRE, "fires a fireball", ability_fireball, 600.0},
    {"punch", ELE_PHYSICAL, "pow", ability_punch, 600.0},
    // {"again", ELE_LIFE, "immediately triggers the abilities of adjacent allies, ignoring cooldown", ability_double, 1200.0}, //too busted
    {"fast fire", ELE_FIRE, "casts a buff on adjacent allies that\ndoubles their cooldown recovery rate\nbut also lights them on fire", ability_haste_fire, 300.0},
    {"heal", ELE_LIFE, "heals the ally with the least health\ncannot self heal", ability_heal, 600.0},
    {"poison", ELE_POISON, "fires a ball of poison", ability_poison, 900.0},
    {"frostbolt", ELE_WATER, "fires a bolt of frost that slows down enemies", ability_frostbolt, 900.0},
    {"corpse explosion", ELE_DEATH, "causes allies to explode on death", ability_corpse_explosion, 1.0},
    {"sadism", ELE_DEATH, "gives adjacent allies masochism, causing them to\ntake damage in place of thier adjacent allies", ability_sadism, 600.0},
    {"one shot protection", ELE_LIFE, "casts a buff on adjacent allies that prevents them\nfrom taking more than 90% of their max health per hit", ability_one_shot_protection, 100.0},
    {"reflect", ELE_CHAOS, "gives self a buff that mirrors incoming damage\ndamage is mutliplied by affinities", ability_reflect, 1.0},
};

void ability_chaos(creature_t* c) {
    int i = rand(&game.seed, 0, len(ability_list));
    ability_list[i].func(c);
}

#endif //ABILITIES
