#ifndef GLINT_BASE_OS_OS_BARRIER_H
#define GLINT_BASE_OS_OS_BARRIER_H

#include "../../base/core/g_types.h"

typedef struct os_barrier
{
    void* opaque[1];
} os_barrier;

internal os_barrier os_barrier_create(u32 thread_count);
internal void       os_barrier_destroy(os_barrier b);
internal b8         os_barrier_wait(os_barrier b);

#endif // GLINT_BASE_OS_OS_BARRIER_H