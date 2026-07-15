#include "../../g_os_memory.h"

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

	res = madvise(ptr, size, MADV_POPULATE_READ | MADV_POPULATE_WRITE);
	if (res < 0)
	{
		return 0;
	}

	return 1;
}

internal void os_memory_decommit(void *ptr, u64 size)
{
	madvise(ptr, size, MADV_DONTNEED);
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
	u32 res = 0;

	res = mprotect(ptr, size, PROT_READ | PROT_WRITE);
	if (res < 0)
	{
		return 0;
	}

	res = madvise(ptr, size, MADV_POPULATE_READ | MADV_POPULATE_WRITE);
	if (res < 0)
	{
		return 0;
	}

	return 1;
}
