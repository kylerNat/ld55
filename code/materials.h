#ifndef MATERIALS
#define MATERIALS

#define N_MAX_MATERIALS 2048
//material_id's only go up to 256, but materials for cell type's can have different properties for each body

#define BASE_CELL_MAT 128
#define ROOM_TEMP 100

struct material_t
{
    uint32 id;

    real cost;
    real growth_rate;
    real max_health;
    real transfer_rate;
    real regen;

    bool is_brain;

    //physical properties
    real density;
    real sharpness;
    real hardness;
    real max_radial_force;
    real max_angular_force;

    real radial_compliance;
    real angular_compliance;
    real plasticity;

    real friction;
    real restitution;
    real drag;

    real movement_force;

    //electrical properties
    real conductivity;
    real leak_conductivity;
    real capacitance;
    real work_function;
    real max_voltage;

    //visual properties
    real_4 base_color;
    real light_radius;
    real_3 emission;

    int texture_type;

    cell_fn physics_update_fn;
    cell_die_fn destroyed_fn;
};

GLuint material_visual_properties_texture;
GLuint material_physical_properties_texture;

material_t materials_list[N_MAX_MATERIALS];
int n_materials;

material_t base_material = {
    .id = 0,

    .cost = 1.0f,
    .growth_rate = 0.005f,

    .max_health = 1,
    .transfer_rate = 0.1f,

    .density = 1.0f,
    .sharpness = 0.0f,
    .hardness = 1.0f,
    .max_radial_force = 4.0f,
    .max_angular_force = 4.0f,

    .radial_compliance = 1.0f,
    .angular_compliance = 1.0f,
    .plasticity = 0.1f,

    .friction = 2.0f,
    .restitution = 0.5f,
    .drag = 0.01f,

    .movement_force = 0.0005f,

    .conductivity = 0.0f,
    .leak_conductivity = 0.001f,
    .capacitance = 1.0f,
    .work_function = 10.0f,
    .max_voltage = 10.0f,

    .base_color = {1, 1, 1, 1},
    .emission = {0,0,0},

    .texture_type = 1,
};

int get_material_index(uint32 material_id)
{
    for(int i = 0; i < N_MAX_MATERIALS; i++)
        if(materials_list[i].id == material_id)
        {
            return i;
        }
    union
    {
        char id_string[5];
        int id_int;
    };
    id_int = material_id;
    id_string[4] = 0;
    log_warning("could not find material: " , id_string, " (", id_int, ")\n");
    return -1;
}

int get_material_index(char* material_id)
{
    return get_material_index(str_to_id(material_id));
}

char* get_description(material_t* mat)
{
    char key[10];
    sprintf(key, "%.4s_desc", (char*) &mat->id);
    return get_translation(key);
}

char* get_name(material_t* mat)
{
    char key[10];
    sprintf(key, "%.4s_name", (char*) &mat->id);
    return get_translation(key);
}

struct drop_pool{
    int materials[N_MAX_MATERIALS];
    real weight_sums[N_MAX_MATERIALS];
    int n_materials;
};

#define N_TIERS 4

drop_pool drop_pools[N_TIERS] = {};

void add_drop_chance(int material_index, int tier, real weight)
{
    int i = drop_pools[tier].n_materials++;
    drop_pools[tier].materials[i] = material_index;
    drop_pools[tier].weight_sums[i] = weight;
    if(i > 0) drop_pools[tier].weight_sums[i] += drop_pools[tier].weight_sums[i-1];
}

int get_from_pool(uint32 * seed, int tier)
{
    drop_pool* dp = drop_pools+tier;

    real r = randf(seed)*dp->weight_sums[dp->n_materials-1];

    //binary search
    int a = 0;
    int b = dp->n_materials-1;
    while(a < b)
    {
        int i = (a+b)/2;
        if(r > dp->weight_sums[i]) a = i+1;
        else b = i;
    }
    return dp->materials[b];
}

#endif //MATERIALS
