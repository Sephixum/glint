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

typedef struct utf_decode
{
	u32 codepoint;
	u32 advance;
} utf_decode;

internal utf_decode utf8_decode(u8* str, u64 max);
internal u32		utf8_encode(u8* dst, u32 codepoint);

internal utf_decode utf16_decode(u16* str, u64 max);
internal u32		utf16_encode(u16* dst, u32 codepoint);

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
internal b8	 string_char_match(u8 a, u8 b, string_match_flags flags);
internal b8	 string_match_wildcard(string str, string pattern, string_match_flags flags);
internal u64 string_find_needle(string str, u64 start_pos, string needle, string_match_flags flags);
internal u64 string_find_needle_reverse(string str, u64 start_pos, string needle, string_match_flags flags);
internal b8	 string_is_before(string a, string b);

internal string		 string_from_memory_size(arena* a, u64 size);
internal string		 string_from_count(arena* a, u64 count);
internal string		 string_from_bits_u32(arena* a, u32 x);
internal string		 string_from_bits_u64(arena* a, u64 x);
internal string		 string_from_u64(arena* a, u64 u64_value, u32 radix, u8 min_digits, u8 digit_group_separator);
internal string		 string_from_i64(arena* a, i64 s64_value, u32 radix, u8 min_digits, u8 digit_group_separator);
internal string		 string_from_f64(arena* a, f64 f64_value);
internal string_list string_split(arena* a, string str, string delim, string_split_flags flags);

#define s(S) string_lit_comp(S)
#define string_lit(S) string_create((u8*)(S), sizeof(S) - 1)
#define string_lit_comp(S) (string){(u8*)(S), sizeof(S) - 1}
#define string_lit_cstr(S) string_create((u8*)(S), sizeof(S))
#define string_varg(S) (int)((S).size), ((S).str)

internal string_node* string_list_push_node(string_list* list, string_node* node);
internal string_node* string_list_push_node_set_string(string_list* list, string_node* node, string str);
internal string_node* string_list_push_node_front(string_list* list, string_node* node);
internal string_node* string_list_push_node_front_set_string(string_list* list, string_node* node, string str);
internal string_node* string_list_push(arena* a, string_list* list, string str);
internal string_node* string_list_push_front(arena* a, string_list* list, string str);
internal void		  string_list_concat_in_place(string_list* list, string_list* to_push);
internal string_node* string_list_push_aligner(arena* a, string_list* list, u64 min, u64 align);
internal string_node* string_list_push_f(arena* a, string_list* list, char* fmt, ...);
internal string_node* string_list_push_front_f(arena* a, string_list* list, char* fmt, ...);
internal string_node* string_list_pop_front(string_list* list);
internal string_list  string_list_copy(arena* a, string_list* list);
internal string_list  string_list_substr(arena* a, string_list list, u64 s, u64 f);
#define string_list_first(list) ((list)->first ? (list)->first->string : string_zero())

internal string_array string_array_zero(void);
internal string_array String_array_from_list(arena* a, string_list* list);
internal string_array String_array_reserve(arena* a, u64 count);
internal string_array String_array_copy(arena* a, string_array array);
internal char**		  cstr_from_string_array(arena* a, string_array array);

internal string		string_strip_trailing_slashes(string path);
internal path_style string_path_get_style(string path);
internal b8			string_path_is_absolute(string path);
internal b8			string_path_is_relative(string path);
internal b8			string_path_has_root(string path);
internal string		string_path_dir_name(string path);
internal string		string_path_base_name(string path);
internal string		string_path_ext(string path);
internal string		string_path_stem(string path);
internal string		string_path_join(arena* a, string base, string relative);
internal string		string_path_to_system(arena* a, string path);
internal string		string_path_to_unix(arena* a, string path);

internal string16 string16_from_string(arena* a, string str);
internal string	  string_from_string16(arena* a, string16 str16);

internal inline force_inline string string_from_g_operating_system(operating_system os)
{
	switch (os)
	{
	case operating_system_null:
		return string_lit_comp("null");
	case operating_system_windows:
		return string_lit_comp("windows");
	case operating_system_linux:
		return string_lit_comp("linux");
	case operating_system_mac:
		return string_lit_comp("mac");
	case operating_system_COUNT:
		return string_lit_comp("COUNT");
	default:
		return string_lit_comp("unknown");
	}
}

internal string string_from_arena_flags(arena* a, arena_flags flags);
internal string string_from_string_match_flags(arena* a, string_match_flags flags);
internal string string_from_string_split_flags(arena* a, string_split_flags flags);
internal string string_from_path_style(arena* a, path_style style);

#endif // GLINT_BASE_STRING_STRING_H
