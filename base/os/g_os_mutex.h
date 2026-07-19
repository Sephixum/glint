#ifndef GLINT_BASE_OS_OS_MUTEX_H
#define GLINT_BASE_OS_OS_MUTEX_H

#include "../../base/core/g_types.h"

typedef struct os_mutex
{
    void* opaque[1];
} os_mutex;

internal os_mutex os_mutex_create(void);
internal void     os_mutex_destroy(os_mutex m);
internal void     os_mutex_lock(os_mutex m);
internal b8       os_mutex_try_lock(os_mutex m);
internal void     os_mutex_unlock(os_mutex m);

#endif // GLINT_BASE_OS_OS_MUTEX_H