#ifndef GLINT_BASE_H
#define GLINT_BASE_H

/**
 * @file base.h
 * Unity build header - includes all base/ library headers
 *
 * Include this file in your project to get the entire base library.
 * For unity builds, also include base.c in your build to compile all implementations.
 *
 * Usage:
 *   #include "base/base.h"
 */

/* Core types and compiler definitions */
#include "core/g_platform_and_compiler_defines.h"
#include "core/g_types.h"

/* Memory allocators */
#include "memory/g_arena.h"

/* String library */
#include "string/g_string.h"

/* OS abstraction layer */
#include "os/g_os_system_info.h"
#include "os/g_os_memory.h"
#include "os/g_os_thread.h"
#include "os/g_os_mutex.h"
#include "os/g_os_cond_var.h"
#include "os/g_os_semaphore.h"
#include "os/g_os_barrier.h"
#include "os/g_os_file.h"
#include "os/g_os_library.h"
#include "os/g_os_time.h"

/* Utility libraries */
#include "util/g_memory.h"
#include "util/g_linked_list.h"
#include "util/g_util.h"

/* Math library */
#include "math/g_math.h"

/* Logging */
#include "log/g_log.h"

/* Duration/time spans */
#include "duration/g_duration.h"

#endif /* GLINT_BASE_H */
