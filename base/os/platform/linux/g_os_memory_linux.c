// Define feature test macros BEFORE any includes
#if !defined(_GNU_SOURCE)
#	define _GNU_SOURCE  // For madvise() and MADV_* constants
#endif

#include "../../g_os_memory.h"
#include "../../../core/g_platform_and_compiler_defines.h"

#include <sys/mman.h>

internal void *os_memory_reserve(u64 size)
{
	if (size == 0)
	{
		return 0;
	}

	void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (result == MAP_FAILED)
	{
		result = 0;
	}

	return result;
}

internal b8 os_memory_commit(void *ptr, u64 size)
{
	i32 res = 0;

	res = mprotect(ptr, size, PROT_READ | PROT_WRITE);
	if (res < 0)
	{
		return 0;
	}

	// madvise() to populate pages (Linux 5.14+)
	// These constants may not be available on older kernels
	#if defined(MADV_POPULATE_READ) && defined(MADV_POPULATE_WRITE)
		res = madvise(ptr, size, MADV_POPULATE_READ | MADV_POPULATE_WRITE);
		if (res < 0)
		{
			// Fallback: just return success, pages will fault in on access
			// This is not an error on older kernels
		}
	#else
		// MADV_POPULATE_* not available, pages will fault in on first access
		(void)res;  // Suppress unused warning
	#endif

	return 1;
}

internal void os_memory_decommit(void *ptr, u64 size)
{
	#if defined(MADV_DONTNEED)
		madvise(ptr, size, MADV_DONTNEED);
	#endif
	mprotect(ptr, size, PROT_NONE);
}

internal void os_memory_release(void *ptr, u64 size)
{
	munmap(ptr, size);
}

internal void *os_memory_reserve_large(u64 size)
{
	void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
	if (result == MAP_FAILED)
	{
		result = 0;
	}
	return result;
}

internal b32 os_memory_commit_large(void *ptr, u64 size)
{
	i32 res = 0;

	res = mprotect(ptr, size, PROT_READ | PROT_WRITE);
	if (res < 0)
	{
		return 0;
	}

	// madvise() to populate huge pages (Linux 5.14+)
	#if defined(MADV_POPULATE_READ) && defined(MADV_POPULATE_WRITE)
		res = madvise(ptr, size, MADV_POPULATE_READ | MADV_POPULATE_WRITE);
		if (res < 0)
		{
			// Fallback: not a critical error for huge pages
		}
	#else
		(void)res;  // Suppress unused warning
	#endif

	return 1;
}
