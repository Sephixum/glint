#include "core/g_platform_and_compiler_defines.h"

#include "base/os/g_os_memory.h"
#include "base/os/g_os_system_info.h"
#include "base/core/g_platform_and_compiler_defines.h"

internal system_info *os_get_system_info(void)
{
	g_not_implemented;
}

internal void *os_memory_reserve(u64 size)
{
	g_not_implemented;
}

internal b8 os_memory_commit(void *ptr, u64 size)
{
	g_not_implemented;
}

internal void os_memory_decommit(void *ptr, u64 size)
{
	g_not_implemented;
}

internal void os_memory_release(void *ptr, u64 size)
{
	g_not_implemented;
}

internal void *os_memory_reserve_large(u64 size)
{
	g_not_implemented;
}

internal b32 os_memory_commit_large(void *ptr, u64 size)
{
	g_not_implemented;
}
