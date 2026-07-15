#ifndef GLINT_BASE_OS_OS_MEMORY_H
#define GLINT_BASE_OS_OS_MEMORY_H

#include "../core/g_types.h"

internal void *os_memory_reserve(u64 size);
internal b8	   os_memory_commit(void *ptr, u64 size);
internal void  os_memory_decommit(void *ptr, u64 size);
internal void  os_memory_release(void *ptr, u64 size);
internal void *os_memory_reserve_large(u64 size);
internal b32   os_memory_commit_large(void *ptr, u64 size);

#endif // GLINT_BASE_OS_OS_MEMORY_H
