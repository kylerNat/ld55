#ifndef MATERIALS_LIST
#define MATERIALS_LIST

#include "cell_funcs.h"

void init_materials_list()
{
    memset(drop_pools, 0, sizeof(drop_pools));
    n_materials = 0;

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("NONE");

        //physical
        mat->density = 0.0;
        mat->friction = 0;
        mat->restitution = 0.0;

        //visual
        mat->base_color = {0,0,0,0};
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("BODY");
        add_drop_chance(material_index, 0, 1.0f);

        mat->cost = 1.0f;
        mat->growth_rate *= 2.5f;

        //visual
        mat->base_color = {0.2, 0.2, 0.2, 1.0};
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("SWIM");
        add_drop_chance(material_index, 0, 0.5f);

        mat->cost = 5.0f;

        //physical

        mat->movement_force *= 2;

        //visual
        mat->base_color = {0.5, 0.7, 0.4, 1.0};

        // mat->physics_update_fn = &cell_swim;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("SEEK");
        add_drop_chance(material_index, 0, 0.1f);

        mat->cost = 5.0f;

        //physical
        mat->restitution = 0.0f;
        mat->drag *= 0.1f;
        mat->movement_force = 0;

        mat->radial_compliance = 0.5f;
        mat->angular_compliance = 32.0f;
        mat->plasticity = 0.0f;

        //visual
        mat->base_color = {0.8, 0.2, 0.9, 1.0};

        mat->physics_update_fn = &cell_seek;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("HARD");
        add_drop_chance(material_index, 1, 0.5f);

        mat->cost = 10.0f;
        mat->growth_rate *= 0.625f;

        //physical
        mat->density = 2.0f;
        mat->hardness = 10.0f;
        mat->friction = 10.0f;
        mat->restitution = 0.9f;
        mat->max_radial_force = 8.0f;
        mat->max_angular_force = 8.0f;
        mat->drag *= 10.0f;

        mat->radial_compliance = 0.0f;
        mat->angular_compliance = 0.0f;
        mat->plasticity *= 0.5f;

        //visual
        mat->base_color = {0.9, 0.9, 0.9, 1.0};
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("SLIP");
        add_drop_chance(material_index, 0, 0.3f);

        mat->cost = 2.0f;
        mat->growth_rate *= 2.0f;

        //physical
        mat->density = 0.25f;
        mat->hardness = 0.5f;
        mat->friction = 0.0f;
        mat->restitution = 0.0f;
        mat->max_radial_force = 2.0f;
        mat->drag = 0.0f;

        mat->radial_compliance = 2.0f;
        mat->angular_compliance = 8.0f;

        //visual
        mat->base_color = {0.3, 0.4, 0.8, 1.0};
        mat->texture_type = 0;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("FLEX");
        add_drop_chance(material_index, 0, 0.3f);

        mat->cost = 2.0f;
        mat->growth_rate *= 2.5f;

        //physical
        mat->density = 0.5f;
        mat->friction *= 0.5f;
        mat->restitution = 0.0f;
        mat->drag = 0.0f;
        mat->movement_force = 0;

        mat->radial_compliance = 0.0f;
        mat->angular_compliance = 64.0f;
        mat->plasticity = 0.0f;

        //visual
        mat->base_color = {0.8, 0.2, 0.7, 1.0};
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("TRG0");

        mat->cost = 5.0f;
        add_drop_chance(material_index, 1, 0.05f);

        mat->conductivity = 100.0f;
        mat->capacitance = 1000.0f;
        mat->leak_conductivity = 10.0f;

        //visual
        mat->base_color = {1.0, 0.2, 0.2, 1.0};

        mat->physics_update_fn = cell_trigger_ability_0;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("TRG1");
        add_drop_chance(material_index, 1, 0.05f);

        mat->cost = 5.0f;

        mat->conductivity = 0.0f;
        mat->capacitance = 1000.0f;
        mat->leak_conductivity = 10.0f;

        //visual
        mat->base_color = {1.0, 0.2, 0.2, 1.0};

        mat->physics_update_fn = cell_trigger_ability_1;
    }

    //TODO: expansion and contraction muscles?
    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("MUSL");
        add_drop_chance(material_index, 1, 0.1f);

        mat->cost = 10.0f;
        mat->growth_rate = 0.00125f;

        mat->density = 2.0f;

        mat->conductivity = 1.0f;
        mat->capacitance = 1.0f;
        mat->leak_conductivity = 0.001f;

        mat->radial_compliance = 0.25;
        mat->angular_compliance = 4.0f;

        //visual
        mat->base_color = {0.9, 0.4, 0.4, 1.0};
        mat->physics_update_fn = cell_muscle;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("HART");

        mat->cost = 25.0f;
        mat->hardness = 0.0f;
        mat->growth_rate *= 0.125f;
        mat->max_health = 100.0f;
        mat->regen = 0.002f; //actual rate
        // mat->regen = 0.01f; //for faster testing

        //visual
        mat->base_color = {0.9, 0.1, 0.1, 1.0};
        mat->light_radius = 5.0f;
        mat->emission = {0.9, 0.1, 0.1};

        mat->texture_type = 2;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("LIGT");
        add_drop_chance(material_index, 0, 0.01f);

        mat->cost = 5.0f;

        //visual
        mat->base_color = {0.9, 0.9, 0.9, 1.0};
        mat->light_radius = 100.0f;
        mat->emission = {0.9, 0.9, 0.9};

        // mat->texture_type = 0;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("WIRE");
        add_drop_chance(material_index, 1, 0.1f);

        mat->cost = 5.0f;

        mat->capacitance = 10.0f;
        mat->conductivity = 100.0f;
        mat->leak_conductivity = 0.001f;
        mat->max_voltage = 100.0f;

        //visual
        mat->base_color = {0.7, 0.7, 0.8, 1.0};
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("AAMP");
        add_drop_chance(material_index, 2, 0.1f);

        mat->cost = 5.0f;

        // mat->max_health = 10.0f;
        mat->transfer_rate = 1.0f,
            mat->capacitance = 1000.0f;
        mat->conductivity = 100.0f;
        mat->leak_conductivity = 10.0f;
        mat->max_voltage = 100.0f;

        //visual
        mat->base_color = {0.6, 0.2, 1.0, 1.0};

        mat->physics_update_fn = cell_amp;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("SPIK");
        add_drop_chance(material_index, 0, 0.1f);

        mat->max_radial_force = 16.0f;
        mat->max_angular_force = 16.0f;

        mat->radial_compliance = 0.1f;
        mat->angular_compliance = 0.1f;

        mat->cost = 5.0f;

        mat->sharpness = 15.0f;

        //visual
        mat->base_color = {0.6, 0.6, 0.6, 1.0};
        mat->texture_type = 3;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("BOMB");
        add_drop_chance(material_index, 1, 0.05f);

        mat->cost = 10.0f;
        mat->growth_rate *= 0.25f;

        mat->conductivity = 100.0f;
        mat->leak_conductivity = 0.001f;

        //visual
        mat->base_color = {1.0, 0.3, 0.0, 1.0};
        mat->light_radius = 3.0f;
        mat->emission = {0.8, 0.4, 0.1};
        mat->texture_type = 1;

        mat->physics_update_fn = cell_explode;
        mat->destroyed_fn = cell_die_explode;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("FEEL");
        add_drop_chance(material_index, 2, 0.05f);

        mat->cost = 5.0f;

        mat->conductivity = 100.0f;
        mat->capacitance = 1000.0f;
        mat->leak_conductivity = 10.0f;

        //visual
        mat->base_color = {0.8, 0.2, 0.4, 1.0};

        mat->physics_update_fn = cell_contact_sensor;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("GRND");
        add_drop_chance(material_index, 1, 0.2f);

        mat->cost = 5.0f;

        mat->conductivity = 100.0f;
        mat->capacitance = 1000.0f;
        mat->leak_conductivity = 100.0f;
        mat->max_voltage = 100.0f;

        //visual
        mat->base_color = {0.5, 0.5, 0.5, 1.0};
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("ONNN");
        add_drop_chance(material_index, 1, 0.1f);

        mat->cost = 5.0f;

        mat->conductivity = 100.0f;
        mat->capacitance = 500.0f;
        mat->leak_conductivity = 10.0f;

        //visual
        mat->base_color = {0.4, 0.3, 0.2, 1.0};

        mat->physics_update_fn = cell_on_switch;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("STIK");
        add_drop_chance(material_index, 1, 0.1f);

        mat->cost = 5.0f;

        mat->conductivity = 100.0f;

        //physical
        mat->restitution = 0.0f;
        mat->drag *= 2.0f;

        mat->radial_compliance = 0.5f;
        mat->angular_compliance = 32.0f;

        //visual
        mat->base_color = {0.2, 0.6, 0.3, 1.0};

        mat->physics_update_fn = &cell_sticky;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("GATE");
        add_drop_chance(material_index, 2, 0.1f);

        mat->cost = 2.0f;

        mat->conductivity = 100.0f;

        //physical

        //visual
        mat->base_color = {0.8, 0.5, 0.5, 1.0};

        mat->physics_update_fn = &cell_gate;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("BREK");
        add_drop_chance(material_index, 2, 0.1f);

        mat->cost = 1.0f;

        mat->conductivity = 100.0f;

        //physical

        //visual
        mat->base_color = {0.7, 0.7, 0.5, 1.0};

        mat->physics_update_fn = &cell_breakaway;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("HDET");
        add_drop_chance(material_index, 2, 0.1f);

        mat->cost = 1.0f;

        mat->cost = 5.0f;

        mat->conductivity = 100.0f;
        mat->capacitance = 1000.0f;
        mat->leak_conductivity = 10.0f;

        //physical

        //visual
        mat->base_color = {0.9, 0.2, 0.2, 1.0};

        mat->physics_update_fn = &cell_hp_detector;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("JETT");
        add_drop_chance(material_index, 1, 0.1f);

        mat->cost = 10.0f;

        //physical

        mat->conductivity = 100.0f;

        //visual
        mat->base_color = {0.7, 0.9, 0.4, 1.0};

        mat->physics_update_fn = &cell_jet;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("LECH");
        add_drop_chance(material_index, 2, 0.2f);
        //TODO: might want to make this unlocked by a special mutation

        mat->cost = 10.0f;

        //physical
        mat->restitution = 0.0f;

        mat->radial_compliance = 0.5f;
        mat->angular_compliance = 32.0f;

        //visual
        mat->base_color = {0.4, 0.0, 0.0, 1.0};

        mat->physics_update_fn = &cell_leech;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("VENM");
        add_drop_chance(material_index, 2, 0.2f);

        mat->cost = 10.0f;

        //physical
        mat->restitution = 0.0f;

        mat->radial_compliance = 0.5f;
        mat->angular_compliance = 32.0f;

        //visual
        mat->base_color = {0.0, 0.4, 0.0, 1.0};

        mat->physics_update_fn = &cell_venom;
    }

    {
        int material_index = n_materials++;
        material_t* mat = materials_list+material_index;
        *mat = base_material;

        mat->id = str_to_id("ANEU");
        add_drop_chance(material_index, 2, 0.01f);

        mat->cost = 10.0f;

        mat->is_brain = true;

        //physical
        mat->restitution = 0.0f;

        mat->radial_compliance = 0.5f;
        mat->angular_compliance = 32.0f;

        //visual
        mat->base_color = {0.4, 0.2, 0.0, 1.0};

        mat->physics_update_fn = &cell_offensive_brain;
    }
}

#endif //MATERIALS_LIST
