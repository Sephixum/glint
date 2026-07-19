#ifndef GLINT_BASE_OS_OS_THREAD_H
#define GLINT_BASE_OS_OS_THREAD_H

#include "../../base/string/g_string.h"

// Thread state enum - tracks lifecycle
typedef enum os_thread_state
{
    os_thread_state_created,    // Created but not started
    os_thread_state_running,    // Started and executing
    os_thread_state_joined,     // Joined (join() completed)
    os_thread_state_detached,   // Detached (detach() called, can't join)
} os_thread_state;

typedef struct os_thread
{
    void* opaque[1];
} os_thread;

typedef void* (*PFN_os_thread_proc)(void* arg);

internal os_thread      os_thread_create(string name, PFN_os_thread_proc proc, void* user);
internal void           os_thread_start(os_thread t);
internal void*          os_thread_join(os_thread t);
internal void           os_thread_detach(os_thread t);
internal void           os_thread_destroy(os_thread t);
internal void           os_thread_attach_current(os_thread t);

internal os_thread      os_thread_get_current(void);

internal b8             os_thread_equal(os_thread a, os_thread b);
internal os_thread_state os_thread_get_state(os_thread t);
internal b8             os_thread_is_running(os_thread t);
internal string         os_thread_get_name(os_thread t);

internal u64            os_thread_get_id(os_thread t);

internal void           os_thread_sleep(u32 milliseconds);
internal void           os_thread_yield(void);
internal void           os_thread_exit(void* retval);

#endif // GLINT_BASE_OS_OS_THREAD_H