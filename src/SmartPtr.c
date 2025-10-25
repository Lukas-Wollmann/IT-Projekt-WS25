#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>

typedef void (*destructor_fn)(void*);

typedef struct ControlBlock {
    atomic_size_t ref_count;
    destructor_fn dtor;
} control_block;

typedef struct {
    void *ptr;
    control_block *control_block;
} shared_ptr;


shared_ptr make_shared(size_t size, destructor_fn dtor)
{
    control_block *control_block = malloc(sizeof(control_block) + size);

    atomic_init(&control_block->ref_count, 1);
    control_block->dtor = dtor;

    return (shared_ptr){ (void*)(control_block + 1), control_block };
}

shared_ptr shared_retain(shared_ptr *sp) 
{
    atomic_fetch_add_explicit(&sp->control_block->ref_count, 1, memory_order_relaxed);

    return *sp;
}

void shared_release(shared_ptr *sp) 
{
    control_block *c = sp->control_block;
    
    int prev = atomic_fetch_sub_explicit(&c->ref_count, 1, memory_order_acq_rel);

    if (prev == 1) 
    {
        atomic_thread_fence(memory_order_acquire);
        
        if (c->dtor)
        {
            c->dtor(sp->ptr);
        }

        free(c);
    }
}

void int_destructor(void *p) 
{
    printf("freeing int payload\n");
}

int main(void) 
{
    shared_ptr sp = make_shared(sizeof(int), int_destructor);
    *(int*)sp.ptr = 123;

    shared_release(&sp);
}