#ifndef GLINT_BASE_MEMORY_ARENA_H
#define GLINT_BASE_MEMORY_ARENA_H

#include "../util/g_util.h"
#include "../core/g_types.h"

typedef enum g_arena_flags
{
	g_arena_flag_no_chain	 = g_bit(1),
	g_arena_flag_large_pages = g_bit(2),
} g_arena_flags;

global constexpr u64		   g_arena_header_size			= 128;
global constexpr u64		   g_arena_default_reserve_size = g_mb(64);
global constexpr u64		   g_arena_default_commit_size	= g_mb(64);
global constexpr g_arena_flags g_arena_default_flags		= 0;

typedef struct g_arena_params
{
	g_arena_flags flags;
	u64			  reserve_size;

	u64	  commit_size;
	void* optional_backing_buffer;

	char* allocation_site_file;
	int	  allocation_site_line;

	char* name;
} g_arena_params;

typedef struct g_arena
{
	struct g_arena* prev;
	struct g_arena* current;

	g_arena_flags flags;

	u64 commit_size;
	u64 reserve_size;
	u64 base_position;

	u64 position;
	u64 commit;
	u64 reserve;

	char* allocation_site_file;
	int	  allocation_site_line;
	char* name;

	struct g_arena* free_last;
} g_arena;
static_assert(sizeof(g_arena) <= g_arena_header_size, "g_arena size is bigger than g_arena_header_size");

internal g_arena* g_arena_create(g_arena_params* params);
#define g_arena_create_default(...)                                                                                    \
	g_arena_create(&(g_arena_params){.reserve_size		   = g_arena_default_reserve_size,                             \
									 .commit_size		   = g_arena_default_commit_size,                              \
									 .flags				   = g_arena_default_flags,                                    \
									 .allocation_site_file = __FILE__,                                                 \
									 .allocation_site_line = __LINE__,                                                 \
									 __VA_ARGS__})
internal void  g_arena_destroy(g_arena* arena);
internal void* g_arena_push(g_arena* arena, u64 size, u64 align, b8 zero);
internal u64   g_arena_pos(g_arena* arena);
internal void  g_arena_pop_to(g_arena* arena, u64 pos);
internal void  g_arena_clear(g_arena* arena);
internal void  g_arena_pop(g_arena* arena, u64 amt);

#define g_arena_push_array_nozero_aligned(arena, type, count, align)                                                   \
	(type*)(g_arena_push((arena), sizeof(type) * (count), (align), 0))
#define g_arena_push_array_aligned(arena, type, count, align)                                                          \
	(type*)(g_arena_push((arena), sizeof(type) * (count), (align), 1))
#define g_arena_push_array_nozero(arena, type, count)                                                                  \
	g_arena_push_array_nozero_aligned(arena, type, count, g_max(8, alignof(type)))
#define g_arena_push_array(arena, type, count) g_arena_push_array_aligned(arena, type, count, g_max(8, alignof(type)))
#define g_arena_push_struct(arena, type) g_arena_push_array(arena, type, 1)

typedef struct g_temp_arena
{
	g_arena* arena;
	u64		 pos;
} g_temp_arena;

internal g_temp_arena g_temp_arena_begin(g_arena* arena);
internal void		  g_temp_arena_end(g_temp_arena temp);

internal g_temp_arena g_arena_get_scratch(g_arena** conflicts, u64 conflict_count);
#define g_arena_release_scratch(temp) g_arena_temp_end(temp)

#endif // GLINT_BASE_MEMORY_ARENA_H
