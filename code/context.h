#ifndef CONTEXT
#define CONTEXT

#include "memory.h"

struct context_t
{
    int thread_id;
    memory_manager* manager;
    uint32 seed;
};

context_t * get_context();

#endif //CONTEXT
