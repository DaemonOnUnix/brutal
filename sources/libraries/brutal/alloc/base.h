#pragma once

#include <brutal/base/macros.h>
#include <brutal/base/std.h>
#include <brutal/mem.h>

typedef struct alloc Alloc;

void alloc_no_op(Alloc *self, void *ptr);

typedef void *AllocAcquire(Alloc *self, size_t size);

typedef void *AllocResize(Alloc *self, void *ptr, size_t new_size);

typedef void AllocRelease(Alloc *self, void *ptr);

struct alloc
{
    AllocAcquire *acquire;
    AllocResize *resize;
    AllocRelease *release;
};

#define alloc_acquire(SELF, SIZE) \
    ((SELF)->acquire((SELF), (SIZE)))

#define alloc_resize(SELF, PTR, NEW_SIZE) \
    ((SELF)->resize((SELF), (PTR), (NEW_SIZE)))

#define alloc_release(SELF, PTR) \
    ((SELF)->release((SELF), (PTR)))

#define alloc_make(SELF, T) (                    \
    {                                            \
        T *ptr = alloc_acquire(SELF, sizeof(T)); \
        mem_set(ptr, 0, sizeof(T));              \
        ptr;                                     \
    })

#define alloc_make_array(SELF, T, COUNT) \
    ((T *)alloc_calloc(SELF, sizeof(T), COUNT))

/* --- Vanilla Allocation Functions ----------------------------------------- */

void *alloc_malloc(Alloc *self, size_t size);

void alloc_free(Alloc *self, void *ptr);

void *alloc_calloc(Alloc *self, size_t size, size_t count);