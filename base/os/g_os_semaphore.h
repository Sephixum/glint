#ifndef GLINT_BASE_OS_OS_SEMAPHORE_H
#define GLINT_BASE_OS_OS_SEMAPHORE_H

#include "../../base/core/g_types.h"

typedef struct os_semaphore
{
    void* opaque[1];
} os_semaphore;

internal os_semaphore os_semaphore_create(u32 initial_count, u32 max_count);
internal void         os_semaphore_destroy(os_semaphore s);
internal void         os_semaphore_wait(os_semaphore s);
internal b8           os_semaphore_try_wait(os_semaphore s);
internal void         os_semaphore_post(os_semaphore s);

#endif // GLINT_BASE_OS_OS_SEMAPHORE_H