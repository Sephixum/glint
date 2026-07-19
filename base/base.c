#include "memory/g_arena.c"
#include "string/g_string.c"

#if GLINT_OS_LINUX
#	include "os/platform/linux/g_os_impl_linux.c"
#elif GLITNT_OS_WINDOWS
#	include "os/platform/windows/g_os_impl_windows.c"
#else
#	error "Unsupported platform - only Linux and Windows are supported"
#endif

#include "log/g_log.c"
#include "duration/g_duration.c"
#include "math/g_math.c"
