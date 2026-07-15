#include "g_arena.h"
#include "../os/g_systme_info.h"
#include "../os/g_os_memory.h"
#include "../util/g_util.h"
#include "../util/g_linked_list.h"
#include "../util/g_memory.h"

#include <memory.h>

internal g_arena* g_arena_create(g_arena_params* params)
{
	u64 reserve_size = params->reserve_size;
	u64 commit_size	 = params->commit_size;

	void* base = params->optional_backing_buffer;
	if (base == 0)
	{
		// Round up the reserve/commit sizes
		if (params->flags & g_arena_flag_large_pages)
		{
			reserve_size = g_align_pow2(reserve_size, g_os_get_system_info()->large_page_size);
			commit_size	 = g_align_pow2(commit_size, g_os_get_system_info()->large_page_size);
		}
		else
		{
			reserve_size = g_align_pow2(reserve_size, g_os_get_system_info()->page_size);
			commit_size	 = g_align_pow2(commit_size, g_os_get_system_info()->page_size);
		}

		if (params->flags & g_arena_flag_large_pages)
		{
			base = g_os_memory_reserve_large(reserve_size);
			g_os_memory_commit_large(base, commit_size);
		}
		else
		{
			base = g_os_memory_reserve(reserve_size);
			g_os_memory_commit(base, commit_size);
		}
	}

	g_arena* arena				= base;
	arena->current				= arena;
	arena->flags				= params->flags;
	arena->commit_size			= commit_size;
	arena->reserve_size			= reserve_size;
	arena->base_position		= 0;
	arena->position				= g_arena_header_size;
	arena->commit				= commit_size;
	arena->reserve				= reserve_size;
	arena->allocation_site_file = params->allocation_site_file;
	arena->allocation_site_line = params->allocation_site_line;
	arena->name					= params->name;
	arena->free_last			= 0;

	return arena;
}

internal void g_arena_destroy(g_arena* arena)
{
	for (g_arena *n = arena->current, *prev = 0; n != 0; n = prev)
	{
		prev = n->prev;
		g_os_memory_release(n, n->reserve);
	}
}

internal void* g_arena_push(g_arena* arena, u64 size, u64 align, b8 zero)
{
	g_arena* current = arena->current;
	u64		 pos_pre = g_align_pow2(current->position, align);
	u64		 pos_pst = pos_pre + size;

	u64 size_to_zero = 0;
	if (zero)
	{
		size_to_zero = g_min(current->commit, pos_pst) - pos_pre;
	}

	// Chain If Needed
	if (current->reserve < pos_pst && !(arena->flags & g_arena_flag_no_chain))
	{
		g_arena* new_block = 0;
		{
			g_arena* prev_block = 0;
			for (new_block = arena->free_last, prev_block = 0; new_block != 0;
				 prev_block = new_block, new_block = new_block->prev)
			{
				if (new_block->reserve >= g_align_pow2(new_block->position, align) + size)
				{
					if (prev_block != 0)
					{
						prev_block->prev = new_block->prev;
					}
					else
					{
						arena->free_last = new_block->prev;
					}
					break;
				}
			}
		}

		if (new_block == 0)
		{
			u64 reserve_size = current->reserve_size;
			u64 commit_size	 = current->commit_size;
			if (size + g_arena_header_size > reserve_size)
			{
				reserve_size = g_align_pow2(size + g_arena_header_size, align);
				commit_size	 = g_align_pow2(size + g_arena_header_size, align);
			}
			new_block = g_arena_create(&(g_arena_params){.reserve_size		   = reserve_size,
														 .commit_size		   = commit_size,
														 .flags				   = current->flags,
														 .allocation_site_file = current->allocation_site_file,
														 .allocation_site_line = current->allocation_site_line});

			size_to_zero = 0;
		}
		else
		{
			size_to_zero = size;
		}

		new_block->base_position = current->base_position + current->reserve;
		g_single_ll_stack_push_n(arena->current, new_block, prev);

		current = new_block;
		pos_pre = g_align_pow2(current->position, align);
		pos_pst = pos_pre + size;
	}

	// Commit New Pages If Needed
	if (current->commit < pos_pst)
	{
		u64 commit_pst_aligned = pos_pst + current->commit_size - 1;
		commit_pst_aligned -= commit_pst_aligned % current->commit_size;
		u64 commit_pst_clamped = g_clamp_top(commit_pst_aligned, current->reserve);
		u64 commit_size		   = commit_pst_clamped - current->commit;
		u8* commit_ptr		   = (u8*)current + current->commit;
		if (current->flags & g_arena_flag_large_pages)
		{
			g_os_memory_commit_large(commit_ptr, commit_size);
		}
		else
		{
			g_os_memory_commit(commit_ptr, commit_size);
		}
		current->commit = commit_pst_clamped;
	}

	void* result = 0;
	if (current->commit >= pos_pst)
	{
		result			  = (u8*)current + pos_pre;
		current->position = pos_pst;
		g_memory_zero(result, size_to_zero);
	}

	return result;
}

internal u64 g_arena_pos(g_arena* arena)
{
	g_arena* current = arena->current;
	u64		 pos	 = current->base_position + current->position;
	return pos;
}

internal void g_arena_pop_to(g_arena* arena, u64 pos)
{
	u64		 big_pos = g_clamp_bot(g_arena_header_size, pos);
	g_arena* current = arena->current;

	for (g_arena* prev = 0; current->base_position >= big_pos; current = prev)
	{
		prev			  = current->prev;
		current->position = g_arena_header_size;
		g_single_ll_stack_push_n(arena->free_last, current, prev);
	}

	arena->current = current;
	u64 new_pos	   = big_pos - current->base_position;
	g_assert(new_pos <= current->position);
	current->position = new_pos;
}

internal void g_arena_clear(g_arena* arena)
{
	g_arena_pop_to(arena, 0);
}

internal void g_arena_pop(g_arena* arena, u64 amt)
{
	u64 pos_old = g_arena_pos(arena);
	u64 pos_new = pos_old;
	if (amt < pos_old)
	{
		pos_new = pos_old - amt;
	}
	g_arena_pop_to(arena, pos_new);
}
