#ifndef RECTANGLE_PACKER
#define RECTANGLE_PACKER

#include "bounding_box_2.h"

struct rectangle_space
{
    int_2 max_size;

    bounding_box_2* free_regions;
    int n_free_regions;
};

bounding_box_2 add_region(rectangle_space* s, int_2 size)
{
    for(int r = 0; r < s->n_free_regions; r++)
    {
        bounding_box_2 b = s->free_regions[r];
        int_2 region_size = b.u-b.l;
        if(size.x <= region_size.x && size.y <= region_size.y)
        {
            s->free_regions[r] = s->free_regions[--s->n_free_regions];

            int_2 excess_size = region_size-size;
            int largest_axes[2] = {0};
            for(int i = 1; i < 2; i++)
            {
                if(excess_size[i] > excess_size[largest_axes[0]]) largest_axes[0] = i;
            }
            int i1 = (largest_axes[0]+1)%2;

            int_2 upper = b.u;
            for(int i = 0; i < 2; i++)
            {
                if(excess_size[largest_axes[i]] > 0)
                {
                    bounding_box_2 new_box = {b.l, upper};
                    new_box.l[largest_axes[i]] += size[largest_axes[i]];
                    upper[largest_axes[i]] -= excess_size[largest_axes[i]];
                    // log_output("adding free box: ", new_box.l, ", ", new_box.u, "\n");
                    s->free_regions[s->n_free_regions++] = new_box;
                }
                else break;
            }

            bounding_box_2 out = {b.l, b.l+size};
            return out;
        }
    }

    log_output("no regions found for box of size ", size, "\n");
    for(int r = 0; r < s->n_free_regions; r++)
    {
        bounding_box_2 b = s->free_regions[r];
        log_output(b.l, ", ", b.u, "\n");
    }
    return {{0,0},{0,0}};
}

void free_region(rectangle_space* s, bounding_box_2 b)
{
    bool merged = false;
    do
    {
        merged = false;
        for(int r = 0; r < s->n_free_regions; r++)
        {
            bounding_box_2 region = s->free_regions[r];
            bool ll[2];
            bool uu[2];
            bool lu[2];
            bool ul[2];
            for(int i = 0; i < 2; i++)
            {
                ll[i] = region.l[i] == b.l[i];
                uu[i] = region.u[i] == b.u[i];
                lu[i] = region.l[i] == b.u[i];
                ul[i] = region.u[i] == b.l[i];
            }

            for(int i = 0; i < 2; i++)
            {
                int i1 = (i+1)%2;
                int i2 = (i+2)%2;
                if(ll[i1] && uu[i1] && ll[i2] && uu[i2] && (lu[i] || ul[i]))
                {
                    if(lu[i]) b.u[i] = region.u[i];
                    else      b.l[i] = region.l[i];

                    s->free_regions[r] = s->free_regions[--s->n_free_regions];
                    merged = true;
                    // log_output("merge\n");
                    break;
                }
            }
            if(merged) break;
        }
    }
    while(merged);

    s->free_regions[s->n_free_regions++] = b;
}

#endif //RECTANGLE_PACKER
