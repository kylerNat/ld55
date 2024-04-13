#ifndef MEMORY
#define MEMORY

#include <utils/misc.h>
#include <stdio.h>

// #define memory_stack_size (1*gigabyte/2)
#define memory_stack_size (100*megabyte)

struct memory_stack
{
    byte* memory;
    size_t available;
    size_t used;

    memory_stack* next;
};

struct stack_allocation
{
    void* data;
    memory_stack* stack;
};

#define N_MAX_STALLOCS 4096

struct memory_manager
{
    memory_stack * first;
    memory_stack * current;

    stack_allocation stallocs[N_MAX_STALLOCS];
    int n_stallocs;
};

memory_stack* allocate_new_stack(size_t size);

byte* stalloc(size_t size);

byte* stalloc_clear(size_t size);

void stunalloc(void * memory);

byte* dynamic_alloc(size_t size)
{
    byte* out = (byte*) malloc(size);
    assert(out, "malloc failed");
    return out;
}

byte* dynamic_alloc_clear(size_t size)
{
    byte* out = (byte*) malloc(size);
    assert(out, "malloc failed");
    memset(out, 0, size);
    return out;
}

byte* dynamic_realloc(void* data, size_t size)
{
    byte* out = (byte*) realloc(data, size);
    assert(out, "realloc failed");
    return out;
}

void dynamic_unalloc(void * data)
{
    free(data);
}

#define stalloc_typed(size, type) ((type*) stalloc((size)*sizeof(type)))
#define dynamic_alloc_typed(size, type) ((type*) dynamic_alloc((size)*sizeof(type)))
#define dynamic_realloc_typed(size, type) ((type*) dynamic_realloc((size)*sizeof(type)))

#endif //MEMORY
