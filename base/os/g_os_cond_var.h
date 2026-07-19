#ifndef GLINT_BASE_OS_OS_COND_VAR_H
#define GLINT_BASE_OS_OS_COND_VAR_H

#include "../../base/core/g_types.h"
#include "g_os_mutex.h"

typedef struct os_cond_var
{
    void* opaque[1];
} os_cond_var;

internal os_cond_var os_cond_var_create(void);
internal void        os_cond_var_destroy(os_cond_var cv);
internal void        os_cond_var_wait(os_cond_var cv, os_mutex m);
internal b8          os_cond_var_timed_wait(os_cond_var cv, os_mutex m, u64 timeout_ms);
internal void        os_cond_var_signal(os_cond_var cv);
internal void        os_cond_var_broadcast(os_cond_var cv);

#endif // GLINT_BASE_OS_OS_COND_VAR_H