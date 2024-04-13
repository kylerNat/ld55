#ifndef BOUNDING_BOX_2
#define BOUNDING_BOX_2

struct bounding_box_2
{
    int_2 l;
    int_2 u;
};

bool operator==(bounding_box_2 a, bounding_box_2 b)
{
    return (a.l==b.l && a.u==b.u);
}

bool is_inside(int_2 p, bounding_box_2 b)
{
    return (b.l.x <= p.x && p.x < b.u.x &&
            b.l.y <= p.y && p.y < b.u.y);
}

bool is_intersecting(bounding_box_2 a, bounding_box_2 b)
{
    //working with coordinates doubled to avoid rounding errors from integer division
    int_2 a_center = (a.l+a.u);
    int_2 a_diag = (a.u-a.l);
    int_2 b_center = (b.l+b.u);
    int_2 b_diag = (b.u+b.l);
    int_2 closest_a_point_to_b = a_center+multiply_components(a_diag, sign_per_axis(b_center-a_center));
    int_2 closest_b_point_to_a = b_center+multiply_components(b_diag, sign_per_axis(b_center-a_center));
    return is_inside(closest_a_point_to_b, {2*b.l, 2*b.u}) || is_inside(closest_b_point_to_a, {2*a.l, 2*a.u});
}

bounding_box_2 expand_to(bounding_box_2 b, int_2 p)
{
    b.l = min_per_axis(b.l, p);
    b.u = max_per_axis(b.u, p+(int_2){1,1});
    return b;
}

int_2 dim(bounding_box_2 b)
{
    return b.u-b.l;
}

#endif //BOUNDING_BOX_2
