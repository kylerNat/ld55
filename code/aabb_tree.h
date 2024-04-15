#ifndef AABB
#define AABB

#define MAX_AABB_NODES 0
#define MAX_AABB_PRIMITIVES 65536

struct aabb_t {
    alignas(16) real_3 l;
    alignas(16) real_3 u;
};

aabb_t contain(aabb_t a, aabb_t b) {
    return {min_per_axis(a.l, b.l), max_per_axis(a.u, b.u)};
}

real_3 dim(aabb_t a) {
    return a.u-a.l;
}

real area(aabb_t a) {
    real_3 s = dim(a);
    return 2.0*(s.y*s.z+s.x*s.z+s.x*s.y);
}

real area_change(aabb_t a, aabb_t b) {
    return area(contain(a, b)) - area(a);
};

bool does_intersect(aabb_t a, aabb_t b) {
    return (max(a.l.x, b.l.x) < min(a.u.x, b.u.x))
        && (max(a.l.y, b.l.y) < min(a.u.y, b.u.y))
        && (max(a.l.z, b.l.z) < min(a.u.z, b.u.z));
}

real_3 aabb_center(aabb_t box) {
    return 0.5*(box.u+box.l);
}

real_3 aabb_size(aabb_t box) {
    return 0.5*(box.u-box.l);
}

enum PRIMITIVE_TYPE {
    PRIM_NONE,
    PRIM_SPHERE,
    PRIM_CAPSULE,
    PRIM_DISC,
    PRIM_BOX,
    N_PRIMITIVE_TYPES
};

#include "../shaders/include/texture_types.h"

#pragma pack(push, 1)
struct aabb_node {
    //positive children are non-leaf nodes, 1 => aabb_node[0], 2 => aabb_node[1], ...
    //negative children are leaves, -1 => primitive[0], -2 =>primitive[1], ...
    //zero means empty
    int children[2];
    int parent;
    int leaf; //the primitive id, if this is a leaf
    int entity_id;
    aabb_t aabb;
};

struct primitive_t {
    aabb_t aabb; //tight aabb, the aabb of the node is expanded for motion
    int node;
    alignas(16) real_3 p;
    real r;
    alignas(16) real_3 size;
    uint32 type;
    alignas(16) quaternion orientation;
    alignas(16) real_3 v;
    alignas(16) real_3 omega;
    alignas(16) real_3 albedo;
    alignas(16) real_3 emission;
    int texture_type;
};

#define which_child(pid) (pid < 0)

struct aabb_tree {
    int n_nodes;
    int n_primitives;
    int root;
    alignas(16) aabb_node nodes[MAX_AABB_NODES];
    alignas(16) primitive_t primitives[MAX_AABB_PRIMITIVES];
};
#pragma pack(pop)

struct pq_element {
    real cost; //lower is better
    int element;
};

struct priority_queue {
    pq_element* queue;
    int n;
};

int pq_push(priority_queue* q, pq_element e)
{
    int i = q->n++;
    q->queue[i] = e;
    for(;;) {
        int p = (i-1)/2;
        if(i == 0 || q->queue[i].cost > q->queue[p].cost) break;
        swap(q->queue[i], q->queue[p])
        i = p;
    }
    return i;
}

pq_element pq_pop(priority_queue* q)
{
    pq_element e = q->queue[0];
    q->queue[0] = q->queue[--q->n];
    int i = 0;
    for(;;) {
        int l = 2*i+1;
        int r = 2*i+2;
        if(l >= q->n) break;
        int c = l;
        if(r < q->n && q->queue[r].cost < q->queue[l].cost) {
            c = r;
        }
        if(q->queue[c].cost > q->queue[i].cost) break;
        swap(q->queue[i], q->queue[c]);
        i = c;
    }

    return e;
}

void update_aabb(aabb_tree* tree, aabb_node* node)
{
    if(!node->leaf)
        node->aabb = contain(tree->nodes[node->children[0]-1].aabb,
                             tree->nodes[node->children[1]-1].aabb);
}

//aabb's up from node need to be updated after this
void maybe_rotate(aabb_tree* tree, aabb_node* node)
{
    if(node->parent == 0) return;
    aabb_node* p = tree->nodes+abs(node->parent)-1;
    int schild = 1-which_child(node->parent);
    if(p->parent == 0) return;
    aabb_node* s = tree->nodes+p->children[schild]-1;
    aabb_node* gp = tree->nodes+abs(p->parent)-1;
    int uchild = 1-which_child(p->parent);
    int uid = gp->children[uchild];
    aabb_node* u = tree->nodes+uid-1;

    real rot_area = area(contain(s->aabb, u->aabb));
    real cur_area = area(contain(s->aabb, node->aabb));
    if(rot_area < cur_area) {
        swap(gp->children[uchild], p->children[which_child(node->parent)]);
        swap(u->parent, node->parent);
    }
}

aabb_node* new_leaf(aabb_tree* tree, aabb_t aabb, int primitive_id)
{
    assert(primitive_id, "attempted to insert null primitive_id");
    if(tree->n_nodes >= MAX_AABB_NODES) {
        log_warning("AABB tree has no space for new nodes");
        return 0;
    }

    int id = ++tree->n_nodes;
    aabb_node* node = tree->nodes+id-1;
    *node = {};
    aabb_node* out = node;
    node->aabb = aabb;
    node->leaf = primitive_id;

    //if there are 0 nodes then we don't need to do anything else
    if(tree->n_nodes == 1) {
        tree->root = id;
        return out;
    }

    priority_queue q = {};
    q.queue = stalloc_typed(tree->n_nodes, pq_element);
    q.queue[q.n++] = {0.0f, tree->root}; //add the root to the search queue

    float best_cost = INF;
    int best_sibling = 0;
    aabb_node* s = 0;
    while(q.n > 0 && best_cost > q.queue[0].cost) {
        pq_element e = pq_pop(&q);
        aabb_node* n = tree->nodes+e.element-1;
        real cost = e.cost+area_change(n->aabb, aabb);
        if(n->leaf) {
            if(cost < best_cost) {
                best_sibling = e.element;
                s = n;
                best_cost = cost;
            }
        } else {
            for(int i = 0; i < 2; i++) {
                pq_push(&q, {cost, n->children[i]});
            }
        }
    }
    stunalloc(q.queue);

    assert(best_sibling, "no sibling found while new aabb node");

    //make a new parent to contain the sibling and the inserted node
    int pid = ++tree->n_nodes;
    aabb_node* p = tree->nodes+pid-1;
    *p = {};
    p->parent = s->parent;
    if(s->parent) {
        aabb_node* gp = tree->nodes+abs(s->parent)-1;
        gp->children[which_child(s->parent)] = pid;
    } else { //the root needs to be the first in the list, so we need to swap p and s
        tree->root = pid;
    }
    p->children[0] = best_sibling;
    p->children[1] = id;
    s->parent = pid;
    node->parent = -pid;

    //walk up to update aabb's & rebalance
    while(id > 0) {
        update_aabb(tree, node);
        id = abs(node->parent);
        maybe_rotate(tree, node);
        node = tree->nodes+id-1;
    }

    assert(tree->nodes[tree->root-1].parent == 0);
    return out;
}

void remove_leaf(aabb_tree* tree, int id)
{
    //can't delete root node, so 1 is invalid
    assert(1 < id && id <= tree->n_nodes, "invalid id ", id, " passed to remove_leaf");
    aabb_node* node = tree->nodes+id-1;
    assert(node->leaf, "attempted to delete non-leaf ", id, " with remove_leaf");
    int pid = abs(node->parent);
    aabb_node* p = tree->nodes+pid-1;
    int sid = p->children[1-which_child(node->parent)];
    aabb_node* s = tree->nodes+sid-1;
    s->parent = p->parent;
    //if the parent is the root node, then we skip updating the grandparent
    //the sibling should become the new root
    if(p->parent != 0) {
        aabb_node* gp = tree->nodes+abs(p->parent)-1;
        gp->children[which_child(p->parent)] = sid;
    } else {
        tree->root = sid;
    }

    //walk up to update aabb's & rebalance
    aabb_node* w = s;
    int wid = sid;
    while(wid > 0) {
        update_aabb(tree, w);
        wid = abs(w->parent);
        maybe_rotate(tree, w);
        w = tree->nodes+wid-1;
    }

    aabb_node old_node = *node;
    aabb_node old_p = *p;
    //need to be careful about moving nodes before doing the walk

    //we can delete node, and p
    //move last two nodes in list to keep memory contiguous
    //make sure we delete in order of highest to lowest index,
    //so that we don't fill the lower hole with the higher one
    aabb_node* del[2] = {};
    if(p>=node) {
        del[0] = p;
        del[1] = node;
    } else {
        del[0] = node;
        del[1] = p;
    }
    for(int j = 0; j < len(del); j++) {
        aabb_node* n = del[j];
        *n = tree->nodes[--tree->n_nodes];
        if(n-tree->nodes < tree->n_nodes) {
            if(n->parent) tree->nodes[abs(n->parent)-1].children[which_child(n->parent)] = n-tree->nodes+1;
            else tree->root = n-tree->nodes+1;
            if(n->leaf) tree->primitives[n->leaf-1].node = n-tree->nodes+1;
            else for(int i = 0; i < 2; i++) {
                    tree->nodes[n->children[i]-1].parent = (n-tree->nodes+1)*(i?-1:1);
                }
        }
    }

    assert(tree->nodes[tree->root-1].parent == 0);
}

void calculate_aabb(primitive_t* p)
{
    switch(p->type) {
        case PRIM_SPHERE: {
            p->aabb = {p->p - p->r, p->p + p->r};
        } break;
        case PRIM_CAPSULE: {
            real_3 axis = apply_rotation(p->orientation, (real_3){0,0,p->size.z});
            real_3 size = max_per_axis(-axis, axis)+p->r;
            p->aabb = {p->p - size, p->p + size};
        } break;
        case PRIM_DISC: { //currently just a box, but it could be smaller
            real_3x3 axes = real_identity_3(p->size.x);
            axes[2][2] = 0.0;
            axes = apply_rotation(p->orientation, axes);
            axes = abs(axes);
            real_3 size = axes[0]+axes[1]+p->r;
            p->aabb = {p->p - size, p->p + size};
        } break;
        case PRIM_BOX: {
            real_3x3 axes = real_identity_3(1.0);
            axes = apply_rotation(p->orientation, axes);
            real_3 size = abs(axes)*p->size+p->r;
            p->aabb = {p->p - size, p->p + size};
        } break;
        default:
            p->aabb = {p->p - p->r - p->size, p->p + p->r + p->size};
    };
}

aabb_t enlarged_aabb(primitive_t* p)
{
    aabb_t aabb = p->aabb;
    aabb.l += min_per_axis(p->v, {})-0.5*abs_per_axis(p->v);
    aabb.u += max_per_axis(p->v, {})+0.5*abs_per_axis(p->v);
    return aabb;
}

// void insert_primitive(aabb_tree* tree, int primitive_id)
// {
//     primitive_t* p = tree->primitives+primitive_id-1;
//     calculate_aabb(p);
//     aabb_node* node = new_leaf(tree, enlarged_aabb(p), primitive_id);
//     p->node = (node-tree->nodes)+1;
//     node->entity_id = p->entity_id;
// }

#endif //AABB
