#ifndef GLINT_BASE_OS_OS_FILE_H
#define GLINT_BASE_OS_OS_FILE_H

#include "base/core/g_types.h"
#include "base/string/g_string.h"
#include "base/util/g_util.h"
#include "base/memory/g_arena.h"
#include "base/os/g_os_time.h"

// ── OS File ───────────────────────────────────────────────────────────────

typedef enum os_file_mode
{
	os_file_mode_read	= g_bit(0),
	os_file_mode_write	= g_bit(1),
	os_file_mode_append = g_bit(2),
} os_file_mode;

typedef struct os_file
{
	void* opaque[1];
} os_file;

// Open / close
internal os_file os_file_open(string path, os_file_mode mode);
internal void	 os_file_close(os_file f);

// Read / write
internal u64	os_file_write(os_file f, void const* data, u64 data_size);
internal string os_file_read(arena* a, os_file f, u64 max_read);
internal string os_file_read_entire(arena* a, os_file f);
internal b8		os_file_flush(os_file f);
internal b8		os_file_valid(os_file f);

// Query
internal u64 os_file_size(os_file f);

// Symlink operations
internal b8 os_symlink_create(string target, string path);
internal b8 os_symlink_remove(string path);
internal b8 os_symlink_remove_recursive(string path);

// Directory operations
internal b8			 os_dir_create(string path);
internal b8			 os_dir_create_recursive(string path);
internal b8			 os_dir_remove(string path);
internal b8			 os_dir_remove_recursive(string path);
internal string_list os_dir_list(arena* a, string path);

// ── Path query and info ───────────────────────────────────────────────────

typedef enum os_path_type
{
	os_path_type_none,
	os_path_type_file,
	os_path_type_dir,
	os_path_type_symlink,
	os_path_type_other,
} os_path_type;

typedef struct os_path_info
{
	b8		exists;
	b8		is_dir;
	b8		is_symlink;
	u64		size_bytes;
	os_time modified_time;
} os_path_info;

internal os_path_type os_path_get_type(string path);
internal os_path_info os_path_get_info(string path);

// ── Error handling ────────────────────────────────────────────────────────

internal string os_file_error(void);

#endif // GLINT_BASE_OS_OS_FILE_H
