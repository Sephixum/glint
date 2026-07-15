#ifndef GLINT_OS_OS_SYSTEM_INFO_H
#define GLINT_OS_OS_SYSTEM_INFO_H

#include "../core/g_types.h"

typedef struct g_system_info
{
	u32	 logical_processor_count;
	u64	 page_size;
	u64	 large_page_size;
	u64	 allocation_granularity;
	char machine_name[32];
} g_system_info;

internal g_system_info* g_os_get_system_info(void);

#endif // GLINT_OS_OS_SYSTEM_INFO_H
