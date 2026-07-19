#include "g_log.h"

#include "base/core/g_platform_and_compiler_defines.h"
#include "base/util/g_memory.h"
#include "base/util/g_linked_list.h"
#include "base/util/g_util.h"
#include "g_string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_BUF_SIZE 4096

typedef struct logger_entity
{
	struct logger_entity* next;
	logger*				  log;
} logger_entity;

typedef struct logger_state
{
	arena*		   a;
	arena*		   entity_a;
	logger_entity* entity_free;
	logger_entity* active_list;
	os_file		   log_file;
} logger_state;

global logger_state* glint_log_state;

internal const char* log_level_label(u32 level)
{
	switch (level)
	{
	case g_bit(1):
		return "TRACE";
	case g_bit(2):
		return "DEBUG";
	case g_bit(3):
		return "INFO";
	case g_bit(4):
		return "WARN";
	case g_bit(5):
		return "ERROR";
	case g_bit(6):
		return "FATAL";
	default:
		return "?????";
	}
}

internal const char* log_level_color(u32 level)
{
	switch (level)
	{
	case g_bit(1):
		return "\033[2m"; // dim
	case g_bit(2):
		return "\033[36m"; // cyan
	case g_bit(3):
		return "\033[32m"; // green
	case g_bit(4):
		return "\033[33m"; // yellow
	case g_bit(5):
		return "\033[31m"; // red
	case g_bit(6):
		return "\033[1;31m"; // bold red
	default:
		return "\033[0m";
	}
}

#define GLINT_COLOR_RESET "\033[0m"

struct logger
{
	string name;
	u32	   filter;
};

// -- state init (matches OS entity pool pattern) --

internal void log_ensure_state(void)
{
	if (glint_log_state)
		return;

	arena* main_a				 = arena_create_default();
	glint_log_state				 = arena_push_struct(main_a, logger_state);
	glint_log_state->a			 = main_a;
	glint_log_state->entity_a	 = arena_create_default();
	glint_log_state->active_list = 0;
	g_memory_zero_struct(&glint_log_state->log_file);

	// pre-allocate 64 entities
	logger_entity* pool = arena_push_array(glint_log_state->entity_a, logger_entity, 64);
	g_for_each_index(i, 63)
	{
		pool[i].next = &pool[i + 1];
	}
	pool[63].next				 = 0;
	glint_log_state->entity_free = pool;
}

internal logger_entity* log_entity_create(void)
{
	log_ensure_state();

	logger_entity* e = glint_log_state->entity_free;
	if (e)
	{
		glint_log_state->entity_free = e->next;
	}
	else
	{
		e = arena_push_struct(glint_log_state->entity_a, logger_entity);
	}
	g_memory_zero_struct(e);
	return e;
}

internal void log_entity_destroy(logger_entity* e)
{
	if (!e)
		return;
	g_single_ll_stack_push(glint_log_state->entity_free, e);
}

// -- public API --

internal logger* log_get(string name)
{
	log_ensure_state();

	g_for_each_node(ent, logger_entity, glint_log_state->active_list)
	{
		if (string_match(ent->log->name, name, 0))
		{
			return ent->log;
		}
	}

	logger_entity* ent			 = log_entity_create();
	ent->log					 = arena_push_struct(glint_log_state->entity_a, logger);
	ent->log->name				 = name;
	ent->log->filter			 = GLINT_LOG_LEVEL_ALL;
	ent->next					 = glint_log_state->active_list;
	glint_log_state->active_list = ent;

	return ent->log;
}

internal void log_set_level(logger* log, u32 level)
{
	if (log)
	{
		log->filter = level;
	}
}

internal string log_name(logger* log)
{
	if (log)
	{
		return log->name;
	}
	return string_zero();
}

internal void log_open_file(string path)
{
	log_ensure_state();
	glint_log_state->log_file = os_file_open(path, os_file_mode_write | os_file_mode_append);
}

internal void log_close_file(void)
{
	if (!glint_log_state || !glint_log_state->log_file.opaque[0])
		return;

	os_file_flush(glint_log_state->log_file);
	os_file_close(glint_log_state->log_file);
	g_memory_zero_struct(&glint_log_state->log_file);
}

// -- timestamp --

internal u64 log_timestamp(char* buf, u64 buf_size)
{
	os_date d = os_time_to_date(os_time_now());
	return (u64)snprintf(buf, buf_size, "%02u:%02u:%02u.%03u", d.hour, d.minute, d.second, d.nanosecond / 1'000'000);
}

// -- core write --

void log_write(logger* log, u32 level, char* file, u32 line, char const* func, char const* fmt, ...)
{
	if (!log || !(log->filter & level))
	{
		return;
	}

	char ts[32];
	log_timestamp(ts, sizeof(ts));

	char buf[LOG_BUF_SIZE];
	u64	 pos = 0;

	pos += (u64)snprintf(buf + pos, LOG_BUF_SIZE - pos, "[%s] ", ts);
	pos += (u64)snprintf(buf + pos,
						 LOG_BUF_SIZE - pos,
						 "%s[%-5s]" GLINT_COLOR_RESET " ",
						 log_level_color(level),
						 log_level_label(level));
	pos += (u64)snprintf(buf + pos,
						 LOG_BUF_SIZE - pos,
						 "\033[36m[%.*s]" GLINT_COLOR_RESET " ",
						 (int)log->name.size,
						 log->name.str);

	va_list args;
	g_defer_scope(va_start(args, fmt), va_end(args))
	{
		pos += (u64)vsnprintf(buf + pos, LOG_BUF_SIZE - pos, fmt, args);
	}

	pos += (u64)snprintf(buf + pos,
						 LOG_BUF_SIZE - pos,
						 "  \033[2m%.*s:%u (%s)\033[0m",
						 string_varg(string_path_base_name(string_create((u8*)file, cstr_length((u8*)file)))),
						 line,
						 func);
	buf[LOG_BUF_SIZE - 1] = '\0';
	fprintf(stderr, "%s\n", buf);

	// plain file output (no colors)
	if (glint_log_state && glint_log_state->log_file.opaque[0])
	{
		char fbuf[LOG_BUF_SIZE];
		u64	 fp = 0;

		fp += (u64)snprintf(fbuf + fp,
							LOG_BUF_SIZE - fp,
							"[%s] [%-5s] [%.*s] ",
							ts,
							log_level_label(level),
							(int)log->name.size,
							log->name.str);

		g_defer_scope(va_start(args, fmt), va_end(args))
		{
			fp += (u64)vsnprintf(fbuf + fp, LOG_BUF_SIZE - fp, fmt, args);
		};
		fp += (u64)snprintf(fbuf + fp,
							LOG_BUF_SIZE - fp,
							"  %.*s:%u (%s)\n",
							string_varg(string_path_stem(string_from_cstr(file))),
							line,
							func);
		fbuf[LOG_BUF_SIZE - 1] = '\0';
		os_file_write(glint_log_state->log_file, fbuf, fp);
	}

	if (level == log_level_error || level == log_level_fatal)
	{
		fflush(stderr);
	}

	if (level == log_level_fatal)
	{
		abort();
	}
}
