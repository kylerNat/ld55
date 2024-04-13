#ifndef CONTEXT_IMPL
#define CONTEXT_IMPL

#include "context.h"

local context_t* context_list[8] = {};

context_t* create_context(int i)
{
    memory_stack* stack = allocate_new_stack(memory_stack_size);
    memory_manager* manager = (memory_manager*) (stack->memory + stack->used);
    stack->used += sizeof(memory_manager);
    manager->first = stack;
    manager->current = manager->first;
    manager->n_stallocs = 0;

    context_t* context = (context_t*) (stack->memory+stack->used);
    stack->used += sizeof(context_t);

    context->thread_id = GetCurrentThreadId();
    context->manager = manager;

    context_list[i] = context;

    return context;
}

context_t* main_context;

context_t * get_context()
{
    int thread_id = GetCurrentThreadId();
    for(int i = 0; i < len(context_list); i++)
    {
        if(context_list[i] && thread_id == context_list[i]->thread_id) return context_list[i];
    }
    return 0;
}

#endif //CONTEXT_IMPL
