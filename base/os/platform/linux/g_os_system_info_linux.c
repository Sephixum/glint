// Define feature test macros BEFORE any includes
#if !defined(_POSIX_C_SOURCE)
#	define _POSIX_C_SOURCE 200809L  // For POSIX.1-2008
#endif
#if !defined(_GNU_SOURCE) && defined(__GNUC__)
#	define _GNU_SOURCE  // For GNU extensions like get_nprocs()
#endif

#include "../../g_systme_info.h"
#include "../../../util/g_util.h"
#include "../../../core/g_platform_and_compiler_defines.h"

#include <unistd.h>

// Use POSIX sysconf for standard C, or GNU extensions if available
#if GLINT_GNU_EXTENSIONS
	#include <sys/sysinfo.h>  // For get_nprocs()
#endif

internal system_info* os_get_system_info(void)
{
	local_persist system_info info   = {0};
	local_persist b8			cached = 0;

	if (!cached)
	{
		// Processor count
		#if GLINT_GNU_EXTENSIONS
			// GNU extension: get_nprocs()
			info.logical_processor_count = (u32)get_nprocs();
		#elif GLINT_HAS_SYSCONF
			// POSIX standard: sysconf
			long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
			info.logical_processor_count = (nprocs > 0) ? (u32)nprocs : 1;
		#else
			// Fallback
			info.logical_processor_count = 1;
		#endif
		
		// Page size
		#if GLINT_GNU_EXTENSIONS
			// GNU extension: getpagesize()
			info.page_size = (u64)getpagesize();
		#elif GLINT_HAS_SYSCONF
			// POSIX standard: sysconf
			long page_size = sysconf(_SC_PAGESIZE);
			info.page_size = (page_size > 0) ? (u64)page_size : 4096;
		#else
			// Fallback: common page size
			info.page_size = 4096;
		#endif
		
		info.allocation_granularity = info.page_size;
		info.large_page_size		 = g_mb(2);
		
		// Hostname
		#if GLINT_POSIX
			gethostname(info.machine_name, g_array_count(info.machine_name));
		#else
			// Fallback: empty string
			info.machine_name[0] = '\0';
		#endif
		
		cached = 1;
	}

	return &info;
}
