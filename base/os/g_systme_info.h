#ifndef GLINT_BASE_OS_OS_SYSTEM_INFO_H
#define GLINT_BASE_OS_OS_SYSTEM_INFO_H

#include "../core/g_types.h"

typedef struct system_info
{
	u32	 logical_processor_count;
	u64	 page_size;
	u64	 large_page_size;
	u64	 allocation_granularity;
	char machine_name[32];
} system_info;

internal system_info* os_get_system_info(void);

#endif // GLINT_BASE_OS_OS_SYSTEM_INFO_H
