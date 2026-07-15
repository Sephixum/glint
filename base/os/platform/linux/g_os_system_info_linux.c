#include "../../g_systme_info.h"
#include "../../../util/g_util.h"

#include <sys/sysinfo.h>
#include <unistd.h>

internal g_system_info* g_os_get_system_info(void)
{
	local_persist g_system_info info   = {0};
	local_persist b8			cached = 0;

	if (!cached)
	{
		info.logical_processor_count = (u32)get_nprocs();
		info.page_size				 = (u64)getpagesize();
		info.allocation_granularity	 = info.page_size;
		info.large_page_size		 = g_mb(2);
		gethostname(info.machine_name, g_array_count(info.machine_name));
		cached = 1;
	}

	return &info;
}
