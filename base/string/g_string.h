#ifndef GLINT_BASE_STRING_STRING_H
#define GLINT_BASE_STRING_STRING_H

#include "../core/g_types.h"
#include "../util/g_util.h"
#include "../memory/g_arena.h"
#include <stdarg.h>

typedef struct string
{
	u8* str;
	u64 size;
} string;

typedef struct string16
{
	u16* str;
	u64	 size;
} string16;

typedef struct string_node
{
	struct string_node* next;
	string				string;
} string_node;

typedef struct string_list
{
	string_node* first;
	string_node* last;
	u64			 node_count;
	u64			 total_size;
} string_list;

typedef struct string_array
{
	string* data;
	u64		count;
	u64		total_size;
} string_array;

typedef enum string_match_flags
{
	string_match_flag_case_insensitive	= g_bit(1),
	string_match_flag_right_side_sloppy = g_bit(2),
	string_match_flag_slash_insensitive = g_bit(3)
} string_match_flags;

typedef enum string_split_flags
{
	string_split_flag_keep_empties = g_bit(1)
} string_split_flags;

typedef enum path_style
{
	path_style_null				= g_bit(1),
	path_style_relative			= g_bit(2),
	path_style_windows_absolute = g_bit(3),
	path_style_unix_absolute	= g_bit(4),
#if GLINT_OS_WINDOWS
	path_style_system_absolute = path_style_windows_absolute
#elif GLINT_OS_LINUX || GLINT_OS_MAC
	path_style_system_absolute = path_style_unix_absolute
#else
#	error "Absolute path style is undefined for this OS."
#endif
} path_style;

internal b8 char_is_space(u8 c);
internal b8 char_is_upper(u8 c);
internal b8 char_is_lower(u8 c);
internal b8 char_is_alpha(u8 c);
internal b8 char_is_slash(u8 c);
internal b8 char_is_digit(u8 c, u32 base);
internal u8 char_to_lower(u8 c);
internal u8 char_to_upper(u8 c);
internal u8 char_to_correct_slash(u8 c);

internal u64 cstr_length(u8* c);
internal u64 cstr16_length(u16* c);

internal string string_create(u8* str, u64 size);
internal string string_range(u8* first, u8* one_past_last);
internal string string_zero(void);

internal string string_from_cstr(char* c);
internal u8*	cstr_from_string(arena* a, string str);
internal string string_cstr_capped(void* cstr, void* cap);
internal string string_cstr_capped_reverse(void* raw_start, void* raw_cap);

internal string string_concat(arena* a, string s1, string s2);
internal string string_copy(arena* a, string s);
internal string string_fv(arena* a, char* fmt, va_list args);
internal string string_f(arena* a, char* fmt, ...);

internal string string_substr(string str, u64 start, u64 finish);
internal string string_prefix(string str, u64 size);
internal string string_skip(string str, u64 amount);
internal string string_postfix(string str, u64 size);
internal string string_chop(string str, u64 amount);
internal string string_skip_chop_whitespace(string string);
internal string string_skip_chop_slashes(string string);

internal i64 sign_from_string(string str, string* string_tail);
internal b8	 string_is_integer(string str, u32 radix);
internal b8	 string_is_integer_signed(string str, u32 radix);
internal u64 u64_from_string(string str, u32 radix);
internal i64 i64_from_string(string str, u32 radix);
internal u32 u32_from_string(string str, u32 radix);
internal i32 i32_from_string(string str, u32 radix);
internal f64 f64_from_string(string str);
internal b8	 try_u64_string_from_c_rules(string str, u64* x);
internal b8	 try_i64_string_from_c_rules(string str, i64* x);

internal b8	 string_match(string a, string b, string_match_flags flags);
internal b8	 string_match_wildcard(string str, string pattern, string_match_flags flags);
internal u64 string_find_needle(string str, u64 start_pos, string needle, string_match_flags flags);
internal u64 string_find_needle_reverse(string str, u64 start_pos, string needle, string_match_flags flags);
internal b8	 string_is_before(string a, string b);
internal b8	 string_char_match(u8 a, u8 b, string_match_flags flags);

internal string		 string_from_memory_size(arena* a, u64 size);
internal string		 string_fromCount(arena* a, u64 count);
internal string		 string_fromBitsU32(arena* a, u32 x);
internal string		 string_fromBitsU64(arena* a, u64 x);
internal string		 string_fromU64(arena* a, u64 u64, u32 radix, u8 min_digits, u8 digit_group_separator);
internal string		 string_fromS64(arena* a, i64 s64, u32 radix, u8 min_digits, u8 digit_group_separator);
internal string		 string_fromF64(arena* a, f64);
internal string_list string_split(arena* a, string str, string delim, string_split_flags flags);

#define s(S) string_lit_comp(S)
#define string_lit(S) string_create((u8*)(S), sizeof(S) - 1)
#define string_lit_comp(S) (string){(u8*)(S), sizeof(S) - 1}
#define string_lit_cstr(S) string_create((u8*)(S), sizeof(S))
#define string_varg(S) (int)((S).size), ((S).str)

#endif // GLINT_BASE_STRING_STRING_H
