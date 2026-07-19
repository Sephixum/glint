#ifndef GLINT_BASE_LOG_LOG_H
#define GLINT_BASE_LOG_LOG_H

#include "../core/g_types.h"
#include "../string/g_string.h"
#include "../memory/g_arena.h"
#include "../util/g_util.h"
#include "../os/g_os_file.h"
#include "g_platform_and_compiler_defines.h"

// Preprocessor-evaluable level bits. The enum below and the compile-time
// `#if` gates both reference these. The enum members cannot be used in the
// gates: `#if` treats identifiers that are not preprocessor macros (enum
// constants are not) as 0, which is what previously made the TRACE/DEBUG/INFO
// gates permanently dead.
#define GLINT_LOG_LEVEL_TRACE_BIT g_bit(1)
#define GLINT_LOG_LEVEL_DEBUG_BIT g_bit(2)
#define GLINT_LOG_LEVEL_INFO_BIT g_bit(3)
#define GLINT_LOG_LEVEL_WARN_BIT g_bit(4)
#define GLINT_LOG_LEVEL_ERROR_BIT g_bit(5)
#define GLINT_LOG_LEVEL_FATAL_BIT g_bit(6)

typedef enum log_level
{
	log_level_trace = GLINT_LOG_LEVEL_TRACE_BIT,
	log_level_debug = GLINT_LOG_LEVEL_DEBUG_BIT,
	log_level_info	= GLINT_LOG_LEVEL_INFO_BIT,
	log_level_warn	= GLINT_LOG_LEVEL_WARN_BIT,
	log_level_error = GLINT_LOG_LEVEL_ERROR_BIT,
	log_level_fatal = GLINT_LOG_LEVEL_FATAL_BIT,
} log_level;

#define GLINT_LOG_LEVEL_ALL                                                                                            \
	(GLINT_LOG_LEVEL_TRACE_BIT | GLINT_LOG_LEVEL_DEBUG_BIT | GLINT_LOG_LEVEL_INFO_BIT | GLINT_LOG_LEVEL_WARN_BIT |     \
	 GLINT_LOG_LEVEL_ERROR_BIT | GLINT_LOG_LEVEL_FATAL_BIT)
#define GLINT_LOG_LEVEL_NONE 0

#ifndef GLINT_LOG_ENABLE
#	define GLINT_LOG_ENABLE GLINT_LOG_LEVEL_ALL
#endif

typedef struct logger logger;

internal logger* log_get(string name);
internal void	 log_set_level(logger* log, log_level level);
internal string	 log_name(logger* log);
internal void	 log_open_file(string path);
internal void	 log_close_file(void);

void log_write(logger* log, log_level level, char* file, u32 line, char const* func, char const* fmt, ...);

#if (GLINT_LOG_LEVEL_TRACE_BIT & GLINT_LOG_ENABLE)
#	define GLINT_LOG_TRACE(l, ...) log_write(l, log_level_trace, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#	define GLINT_LOG_TRACE(l, ...) g_no_op
#endif

#if (GLINT_LOG_LEVEL_DEBUG_BIT & GLINT_LOG_ENABLE)
#	define GLINT_LOG_DEBUG(l, ...) log_write(l, log_level_debug, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#	define GLINT_LOG_DEBUG(l, ...) g_no_op
#endif

#if (GLINT_LOG_LEVEL_INFO_BIT & GLINT_LOG_ENABLE)
#	define GLINT_LOG_INFO(l, ...) log_write(l, log_level_info, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#	define GLINT_LOG_INFO(l, ...) g_no_op
#endif

#define GLINT_LOG_WARN(l, ...) log_write(l, log_level_warn, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define GLINT_LOG_ERROR(l, ...) log_write(l, log_level_error, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define GLINT_LOG_FATAL(l, ...) log_write(l, log_level_fatal, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#endif // GLINT_BASE_LOG_LOG_H
