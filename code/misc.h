#ifndef MY_MISC
#define MY_MISC

size_t index_2(int_2 pos, int_2 size)
{
    return pos.x+size.x*pos.y;
}

size_t index_2(int_2 pos, int size)
{
    return pos.x+size*pos.y;
}

size_t index_2(int_2 pos, bounding_box_2 region)
{
    return index_2(pos-region.l, dim(region));
}

size_t index_3(int_3 pos, int_3 size)
{
    return pos.x+size.x*(pos.y+size.y*pos.z);
}

size_t index_3(int_3 pos, int size)
{
    return pos.x+size*(pos.y+size*pos.z);
}

const real_2x2 from_hex = {
    1.0f,0.0f,
    0.5f,0.5f*sqrt(3.0f),
};

const real_2x2 to_hex = inverse(from_hex);

int_2 hex_cell(real_2 x)
{
    real_2 p = to_hex*x;
    real_3 cube = {p.x, p.y, -p.x-p.y};
    real_3 rounded = round_per_axis(cube);
    int_3 int_cube = int_cast(rounded);
    real_3 diffs = abs_per_axis(rounded-cube);
    if(diffs[0] > diffs[1] && diffs[0] > diffs[2]) int_cube.x = -int_cube.y-int_cube.z;
    else if(diffs[1] > diffs[2]) int_cube.y = -int_cube.x-int_cube.z;
    return int_cube.xy;
}

uint32 str_to_id(char* str)
{
    union
    {
        uint32 id;
        char id_str[4];
    };
    assert(str[4] == 0, "id string must be 4 characters");
    memcpy(id_str, str, 4);
    return id;
}
#endif //MY_MISC
