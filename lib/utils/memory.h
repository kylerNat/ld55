#ifndef MEMORY
#define MEMORY

#include <utils/misc.h>
#include <utils/logging.h>
#include <stdio.h>
#include <cstring>

#ifndef platform_big_alloc
#include <stdlib.h>
#define platform_big_alloc(size) malloc(size)
#endif //platform_big_alloc

#ifndef block_size
#define block_size (1*gigabyte)
#endif //block_size

struct memory_block
{
    byte* memory;
    size_t available;
    size_t used;

    bool is_reserved; //set if the block is being used temporarily

    memory_block* next;
};

struct memory_manager
{
    memory_block * first;
    memory_block * current;
};

memory_block* allocate_new_block(size_t size)
{
    memory_block* new_block = (memory_block*) platform_big_alloc(size);
    new_block->memory = (byte*) new_block;
    new_block->available = block_size;
    new_block->used = sizeof(memory_block);
    new_block->next = 0;
    return new_block;
}

//TODO: handle case where size is > block_size
byte* permalloc(memory_manager* manager, size_t size)
{
    assert(!manager->current->is_reserved, "tried to allocate permanent memory while current memory block is reserved");
    if(manager->current->used+size > manager->current->available)
    {
        assert(block_size >= size,
               "tried to allocate ", size, " bytes, but permalloc cannot yet handle allocations larger than ", block_size);
        manager->current->next = allocate_new_block(block_size);
        manager->current = manager->current->next;
    }
    byte* out = manager->current->memory + manager->current->used;
    manager->current->used += size;
    return out;
}

byte* permalloc_clear(memory_manager* manager, size_t size)
{
    byte* out = permalloc(manager, size);
    memset(out, 0, size);
    return out;
}

void allocate_reserved(memory_manager* manager, size_t size)
{
    assert(!manager->current->is_reserved, "tried to allocate permanent memory while current memory block is reserved");
    manager->current->used += size;
    assert(manager->current->used <= manager->current->available, "overflowed reserved block");
}

//reserves a block for temporary use with at least size bytes unused
byte* reserve_block(memory_manager* manager, size_t size)
{
    assert(!manager->current->is_reserved, "tried to reserve block that is already reserved");
    if(manager->current->used+size > manager->current->available)
    {
        assert(block_size >= size);
        manager->current->next = allocate_new_block(block_size);
        manager->current = manager->current->next;
    }
    manager->current->is_reserved = true;
    return manager->current->memory + manager->current->used;
}

size_t current_block_unused(memory_manager* manager)
{
    return manager->current->available - manager->current->used;
}

void unreserve_block(memory_manager* manager)
{
    assert(manager->current->is_reserved, "tried to unreserve block that is not reserved");
    manager->current->is_reserved = false;
}

#ifdef USE_DEFAULT_MEMORY
static memory_block* default_first_block = allocate_new_block(block_size);
memory_manager default_manager = {default_first_block, default_first_block};

byte*   permalloc            (size_t size) { return permalloc            (&default_manager, size); }
byte*   permalloc_clear      (size_t size) { return permalloc_clear      (&default_manager, size); }
void    allocate_reserved    (size_t size) { return allocate_reserved    (&default_manager, size); }
byte*   reserve_block        (size_t size) { return reserve_block        (&default_manager, size); }
size_t  current_block_unused ()            { return current_block_unused (&default_manager);       }
void    unreserve_block      ()            { return unreserve_block      (&default_manager);       }

#endif //USE_DEFAULT_MEMORY

#endif //MEMORY
