#ifndef WIN32_WORK_SYSTEM
#define WIN32_WORK_SYSTEM

#define AUDIO_BUFFER_MAX_LEN (50)
#define AUDIO_BUFFER_MAX_SIZE (2*AUDIO_BUFFER_MAX_LEN)

#include "context.h"

struct work_task
{
    void (*func)(context_t*, void*);
    void* data;
};

volatile work_task* work_stack;
volatile int n_work_entries; //number of unstarted tasks
volatile int n_remaining_tasks; //number of incomplete tasks
volatile int n_ready_threads;
HANDLE work_semephore;
const int n_max_workers = 7; //TODO: make this adjust to processor count
const int n_threads = n_max_workers+1;

bool pop_work(context_t* context)
{
    // int work_index = InterlockedDecrement((volatile long*) &n_work_entries);
    int work_index = n_work_entries;
    work_task work;
    if(work_index > 0)
    {
        work.func = work_stack[work_index-1].func;
        work.data = work_stack[work_index-1].data;
        _ReadBarrier();
        if(work_index != InterlockedCompareExchange((volatile long*) &n_work_entries, work_index-1, work_index))
            return true;
        work.func(context, work.data);
        InterlockedDecrement((volatile long*) &n_remaining_tasks);
        return true;
    }
    return false;
}

DWORD WINAPI thread_proc(void* param)
{
    int* thread_number = (int*) param;
    context_t* context = create_context(*thread_number);
    _WriteBarrier();
    InterlockedIncrement((volatile long*) &n_ready_threads);
    for ever
    {
        if(!pop_work(context)) WaitForSingleObjectEx(work_semephore, -1, false);
    }
}

void push_work(void (*func)(context_t*, void*), void* data)
{
    int work_index;
    _ReadWriteBarrier();
    do {
        work_index = n_work_entries;
        _ReadBarrier();
        work_stack[work_index].func = func;
        work_stack[work_index].data = data;
        _WriteBarrier();
    } while(work_index != InterlockedCompareExchange((volatile long*) &n_work_entries, work_index+1, work_index));
    InterlockedIncrement((volatile long*) &n_remaining_tasks);

    ReleaseSemaphore(work_semephore, 1, NULL);
}

void busy_work(int i, void* data)
{
    // log_output("thread ", i, ": ", (char*) data);
    printf("thread %i: %s", i, (char*) data);
}

#endif //WIN32_WORK_SYSTEM
