#ifndef GLINT_BASE_MEMORY_ARENA_H
#define GLINT_BASE_MEMORY_ARENA_H

#include "../util/g_util.h"
#include "../core/g_types.h"

typedef enum arena_flags
{
	arena_flag_no_chain	   = g_bit(1),
	arena_flag_large_pages = g_bit(2),
} arena_flags;

global constexpr u64		 arena_header_size			= 128;
global constexpr u64		 arena_default_reserve_size = g_mb(64);
global constexpr u64		 arena_default_commit_size	= g_mb(64);
global constexpr arena_flags arena_default_flags		= 0;

typedef struct arena_params
{
	arena_flags flags;
	u64			reserve_size;

	u64	  commit_size;
	void* optional_backing_buffer;

	char* allocation_site_file;
	int	  allocation_site_line;

	char* name;
} arena_params;

typedef struct arena
{
	struct arena* prev;
	struct arena* current;

	arena_flags flags;

	u64 commit_size;
	u64 reserve_size;
	u64 base_position;

	u64 position;
	u64 commit;
	u64 reserve;

	char* allocation_site_file;
	int	  allocation_site_line;
	char* name;

	struct arena* free_last;
} arena;
static_assert(sizeof(arena) <= arena_header_size, "g_arena size is bigger than g_arena_header_size");

internal arena* arena_create(arena_params* params);
#define arena_create_default(...)                                                                                      \
	arena_create(&(arena_params){.reserve_size		   = arena_default_reserve_size,                                   \
								 .commit_size		   = arena_default_commit_size,                                    \
								 .flags				   = arena_default_flags,                                          \
								 .allocation_site_file = __FILE__,                                                     \
								 .allocation_site_line = __LINE__,                                                     \
								 __VA_ARGS__})
internal void  arena_destroy(arena* arena);
internal void* arena_push(arena* arena, u64 size, u64 align, b8 zero);
internal u64   arena_pos(arena* arena);
internal void  arena_pop_to(arena* arena, u64 pos);
internal void  arena_clear(arena* arena);
internal void  arena_pop(arena* arena, u64 amt);

#define arena_push_array_nozero_aligned(arena, type, count, align)                                                     \
	(type*)(arena_push((arena), sizeof(type) * (count), (align), 0))
#define arena_push_array_aligned(arena, type, count, align)                                                            \
	(type*)(arena_push((arena), sizeof(type) * (count), (align), 1))
#define arena_push_array_nozero(arena, type, count)                                                                    \
	arena_push_array_nozero_aligned(arena, type, count, g_max(8, alignof(type)))
#define arena_push_array(arena, type, count) arena_push_array_aligned(arena, type, count, g_max(8, alignof(type)))
#define arena_push_struct(arena, type) arena_push_array(arena, type, 1)

typedef struct temp_arena
{
	arena* arena;
	u64	   pos;
} temp_arena;

internal temp_arena temp_arena_begin(arena* arena);
internal void		temp_arena_end(temp_arena temp);

internal temp_arena arena_get_scratch(arena** conflicts, u64 conflict_count);
#define arena_release_scratch(temp) temp_arena_end(temp)

#endif // GLINT_BASE_MEMORY_ARENA_H
