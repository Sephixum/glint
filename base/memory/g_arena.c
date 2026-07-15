#include "g_arena.h"
#include "../os/g_systme_info.h"
#include "../os/g_os_memory.h"
#include "../util/g_util.h"
#include "../util/g_linked_list.h"
#include "../util/g_memory.h"

#include <memory.h>

internal arena* arena_create(arena_params* params)
{
	u64 reserve_size = params->reserve_size;
	u64 commit_size	 = params->commit_size;

	void* base = params->optional_backing_buffer;
	if (base == 0)
	{
		// Round up the reserve/commit sizes
		if (params->flags & arena_flag_large_pages)
		{
			reserve_size = g_align_pow2(reserve_size, g_os_get_system_info()->large_page_size);
			commit_size	 = g_align_pow2(commit_size, g_os_get_system_info()->large_page_size);
		}
		else
		{
			reserve_size = g_align_pow2(reserve_size, g_os_get_system_info()->page_size);
			commit_size	 = g_align_pow2(commit_size, g_os_get_system_info()->page_size);
		}

		if (params->flags & arena_flag_large_pages)
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

	arena* arena				= base;
	arena->current				= arena;
	arena->flags				= params->flags;
	arena->commit_size			= commit_size;
	arena->reserve_size			= reserve_size;
	arena->base_position		= 0;
	arena->position				= arena_header_size;
	arena->commit				= commit_size;
	arena->reserve				= reserve_size;
	arena->allocation_site_file = params->allocation_site_file;
	arena->allocation_site_line = params->allocation_site_line;
	arena->name					= params->name;
	arena->free_last			= 0;

	return arena;
}

internal void arena_destroy(arena* a)
{
	for (arena *n = a->current, *prev = 0; n != 0; n = prev)
	{
		prev = n->prev;
		g_os_memory_release(n, n->reserve);
	}
}

internal void* arena_push(arena* a, u64 size, u64 align, b8 zero)
{
	arena* current = a->current;
	u64	   pos_pre = g_align_pow2(current->position, align);
	u64	   pos_pst = pos_pre + size;

	u64 size_to_zero = 0;
	if (zero)
	{
		size_to_zero = g_min(current->commit, pos_pst) - pos_pre;
	}

	// Chain If Needed
	if (current->reserve < pos_pst && !(a->flags & arena_flag_no_chain))
	{
		arena* new_block = 0;
		{
			arena* prev_block = 0;
			for (new_block = a->free_last, prev_block = 0; new_block != 0;
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
						a->free_last = new_block->prev;
					}
					break;
				}
			}
		}

		if (new_block == 0)
		{
			u64 reserve_size = current->reserve_size;
			u64 commit_size	 = current->commit_size;
			if (size + arena_header_size > reserve_size)
			{
				reserve_size = g_align_pow2(size + arena_header_size, align);
				commit_size	 = g_align_pow2(size + arena_header_size, align);
			}
			new_block = arena_create(&(arena_params){.reserve_size		   = reserve_size,
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
		g_single_ll_stack_push_n(a->current, new_block, prev);

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
		if (current->flags & arena_flag_large_pages)
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

internal u64 arena_pos(arena* a)
{
	arena* current = a->current;
	u64	   pos	   = current->base_position + current->position;
	return pos;
}

internal void arena_pop_to(arena* a, u64 pos)
{
	u64	   big_pos = g_clamp_bot(arena_header_size, pos);
	arena* current = a->current;

	for (arena* prev = 0; current->base_position >= big_pos; current = prev)
	{
		prev			  = current->prev;
		current->position = arena_header_size;
		g_single_ll_stack_push_n(a->free_last, current, prev);
	}

	a->current	= current;
	u64 new_pos = big_pos - current->base_position;
	g_assert(new_pos <= current->position);
	current->position = new_pos;
}

internal void arena_clear(arena* a)
{
	arena_pop_to(a, 0);
}

internal void arena_pop(arena* a, u64 amt)
{
	u64 pos_old = arena_pos(a);
	u64 pos_new = pos_old;
	if (amt < pos_old)
	{
		pos_new = pos_old - amt;
	}
	arena_pop_to(a, pos_new);
}
