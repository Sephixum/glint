#include "g_string.h"
#include "../util/g_memory.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

internal b8 char_is_space(u8 c)
{
	return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

internal b8 char_is_upper(u8 c)
{
	return ('A' <= c && c <= 'Z');
}

internal b8 char_is_lower(u8 c)
{
	return ('a' <= c && c <= 'z');
}

internal b8 char_is_alpha(u8 c)
{
	return (char_is_lower(c) || char_is_upper(c));
}

internal b8 char_is_slash(u8 c)
{
	return (c == '/' || c == '\\');
}

internal b8 char_is_digit(u8 c, u32 base)
{
	b8 result = 0;
	if (0 < base && base <= 16)
	{
		u8 val = integer_symbol_reverse[c];
		if (val < base)
		{
			result = 1;
		}
	}
	return result;
}

internal u8 char_to_lower(u8 c)
{
	if (char_is_upper(c))
	{
		c += ('a' - 'A');
	}
	return c;
}

internal u8 char_to_upper(u8 c)
{
	if (char_is_lower(c))
	{
		c += ('A' - 'a');
	}
	return c;
}

internal u8 char_to_correct_slash(u8 c)
{
	if (char_is_slash(c))
	{
		c = '/';
	}
	return c;
}

internal u64 cstr_length(u8* c)
{
	u64 length = 0;
	if (c)
	{
		u8* p = c;
		while (*p != 0)
		{
			p += 1;
		}
		length = (u64)(p - c);
	}
	return length;
}

internal u64 cstr16_length(u16* c)
{
	u64 length = 0;
	if (c)
	{
		u16* p = c;
		while (*p != 0)
		{
			p += 1;
		}
		length = (u64)(p - c);
	}
	return length;
}

internal string string_create(u8* str, u64 size)
{
	return (string){str, size};
}

internal string string_range(u8* first, u8* one_past_last)
{
	return string_create(first, one_past_last - first);
}

internal string string_zero(void)
{
	return string_create(0, 0);
}

internal string string_from_cstr(char* c)
{
	string result = string_create((u8*)c, cstr_length((u8*)c));
	return result;
}
internal u8* cstr_from_string(arena* a, string str)
{
	u8* buf = arena_push_array(a, u8, str.size + 1);
	g_memory_copy(buf, str.str, str.size);
	buf[str.size] = 0;
	return buf;
}

internal string string_cstr_capped(void* cstr, void* cap)
{
	char* ptr = (char*)cstr;
	char* opl = (char*)cap;
	for (; ptr < opl && *ptr != 0; ptr += 1)
	{
	}
	u64	   size	  = (u64)(ptr - (char*)cstr);
	string result = string_create((u8*)cstr, size);
	return result;
}

internal string string_cstr_capped_reverse(void* raw_start, void* raw_cap)
{
	u8* start = raw_start;
	u8* ptr	  = raw_cap;
	for (; ptr > start;)
	{
		ptr -= 1;
		if (*ptr == 0)
		{
			break;
		}
	}
	u64	   size	  = (u64)(ptr - start);
	string result = string_create(start, size);
	return result;
}

internal string string_concat(arena* a, string s1, string s2)
{
	string str = string_zero();
	str.size   = s1.size + s2.size;
	str.str	   = arena_push_array_nozero(a, u8, str.size + 1);
	g_memory_copy(str.str, s1.str, s1.size);
	g_memory_copy(str.str + s1.size, s2.str, s2.size);
	str.str[str.size] = 0;
	return str;
}

internal string string_copy(arena* a, string s)
{
	string str = string_zero();
	str.size   = s.size;
	str.str	   = arena_push_array_nozero(a, u8, str.size + 1);
	g_memory_copy(str.str, s.str, s.size);
	str.str[str.size] = 0;
	return str;
}

internal string string_fv(arena* a, char* fmt, va_list args)
{
	va_list args2;
	va_copy(args2, args);
	u32	   needed_bytes		= vsnprintf(0, 0, fmt, args) + 1;
	string result			= string_zero();
	result.str				= arena_push_array_nozero(a, u8, needed_bytes);
	result.size				= vsnprintf((char*)result.str, needed_bytes, fmt, args2);
	result.str[result.size] = 0;
	va_end(args2);
	return result;
}

internal string string_f(arena* a, char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	string result = string_fv(a, fmt, args);
	va_end(args);
	return result;
}

internal string string_substr(string str, u64 start, u64 finish)
{
	start  = g_clamp_top(start, str.size);
	finish = g_clamp_top(finish, str.size);
	str.str += start;
	str.size = g_dim_u64(start, finish);
	return str;
}

internal string string_prefix(string str, u64 size)
{
	str.size = g_clamp_top(size, str.size);
	return str;
}

internal string string_skip(string str, u64 amount)
{
	amount = g_clamp_top(amount, str.size);
	str.str += amount;
	str.size -= amount;
	return str;
}

internal string string_postfix(string str, u64 size)
{
	size	 = g_clamp_top(size, str.size);
	str.str	 = (str.str + str.size) - size;
	str.size = size;
	return str;
}

internal string string_chop(string str, u64 amount)
{
	amount = g_clamp_top(amount, str.size);
	str.size -= amount;
	return str;
}
internal string string_skip_chop_whitespace(string string)
{
	u8* first = string.str;
	u8* opl	  = first + string.size;
	for (; first < opl; first += 1)
	{
		if (!char_is_space(*first))
		{
			break;
		}
	}
	for (; opl > first;)
	{
		opl -= 1;
		if (!char_is_space(*opl))
		{
			opl += 1;
			break;
		}
	}

	struct string result = string_range(first, opl);
	return result;
}

internal string string_skip_chop_slashes(string string)
{
	u8* first = string.str;
	u8* opl	  = first + string.size;
	for (; first < opl; first += 1)
	{
		if (!char_is_slash(*first))
		{
			break;
		}
	}
	for (; opl > first;)
	{
		opl -= 1;
		if (!char_is_slash(*opl))
		{
			opl += 1;
			break;
		}
	}
	struct string result = string_range(first, opl);
	return result;
}

internal i64 sign_from_string(string str, string* string_tail)
{
	// count negative signs
	u64 neg_count = 0;
	u64 i		  = 0;
	for (; i < str.size; i += 1)
	{
		if (str.str[i] == '-')
		{
			neg_count += 1;
		}
		else if (str.str[i] != '+')
		{
			break;
		}
	}

	// output part of string after signs
	*string_tail = string_skip(str, i);

	// output integer sign
	i64 sign = (neg_count & 1) ? -1 : +1;
	return sign;
}
internal b8 string_is_integer(string str, u32 radix)
{
	b8 result = 0;
	if (str.size > 0)
	{
		if (1 < radix && radix <= 16)
		{
			result = 1;
			for (u64 i = 0; i < str.size; i += 1)
			{
				u8 c = str.str[i];
				if (!(c < 0x80) || integer_symbol_reverse[c] >= radix)
				{
					result = 0;
					break;
				}
			}
		}
	}
	return result;
}

internal b8 string_is_integer_signed(string str, u32 radix)
{
	b8	   result = 0;
	string sign	  = string_prefix(str, 1);
	if (string_match(sign, s("-"), 0))
	{
		result = string_is_integer(string_skip(str, 1), radix);
	}
	else
	{
		result = string_is_integer(str, radix);
	}
	return result;
}

internal u64 u64_from_string(string str, u32 radix)
{
	u64 x = 0;
	if (1 < radix && radix <= 16)
	{
		for (u64 i = 0; i < str.size; i += 1)
		{
			x *= radix;
			x += integer_symbol_reverse[str.str[i] & 0x7F];
		}
	}
	return x;
}

internal i64 i64_from_string(string str, u32 radix)
{
	i64 sign = sign_from_string(str, &str);
	i64 x	 = (i64)u64_from_string(str, radix) * sign;
	return x;
}

internal u32 u32_from_string(string str, u32 radix)
{
	u64 x64 = u64_from_string(str, radix);
	g_assert_always(x64 <= max_u32);
	u32 x32 = (u32)x64;
	return x32;
}

internal i32 i32_from_string(string str, u32 radix)
{
	i64 x64 = i64_from_string(str, radix);
	g_assert_always(x64 <= max_i32);
	i32 x32 = (i32)x64;
	return x32;
}

internal f64 f64_from_string(string str)
{
	f64 result = 0;
	if (str.size > 0)
	{
		// find starting pos of numeric string, as well as sign
		f64 sign = +1.0;
		if (str.str[0] == '-')
		{
			sign = -1.0;
		}
		else if (str.str[0] == '+')
		{
			sign = 1.0;
		}

		// gather numerics
		u64	 num_valid_chars = 0;
		char buffer[64];
		b32	 exp = 0;
		for (u64 idx = 0; idx < str.size && num_valid_chars < sizeof(buffer) - 1; idx += 1)
		{
			if (char_is_digit(str.str[idx], 10) || str.str[idx] == '.' || str.str[idx] == 'e' ||
				(exp && (str.str[idx] == '+' || str.str[idx] == '-')))
			{
				buffer[num_valid_chars] = str.str[idx];
				num_valid_chars += 1;
				exp = 0;
				exp = (str.str[idx] == 'e');
			}
		}

		buffer[num_valid_chars] = 0;
		result					= sign * atof(buffer);
	}
	return result;
}

internal b8 try_u64_string_from_c_rules(string str, u64* x)
{
	u64 radix		= 0;
	u64 prefix_size = 0;
	{
		// hex
		if (string_match(string_prefix(str, 2), string_lit("0x"), string_match_flag_case_insensitive))
		{
			radix = 0x10, prefix_size = 2;
		}
		// binary
		else if (string_match(string_prefix(str, 2), string_lit("0b"), string_match_flag_case_insensitive))
		{
			radix = 2, prefix_size = 2;
		}
		// octal
		else if (string_match(string_prefix(str, 1), string_lit("0o"), string_match_flag_case_insensitive) &&
				 str.size > 1)
		{
			radix = 010, prefix_size = 1;
		}
		// decimal
		else
		{
			radix = 10, prefix_size = 0;
		}
	}

	string integer	  = string_skip(str, prefix_size);
	b8	   is_integer = string_is_integer(integer, radix);
	if (is_integer)
	{
		*x = u64_from_string(integer, radix);
	}

	return is_integer;
}

internal b8 try_i64_string_from_c_rules(string str, i64* x)
{
	string string_tail = string_zero();
	i64	   sign		   = sign_from_string(str, &string_tail);
	u64	   x_u64	   = 0;
	b32	   is_integer  = try_u64_string_from_c_rules(string_tail, &x_u64);
	*x				   = x_u64 * sign;
	return is_integer;
}

internal b8 string_match(string a, string b, string_match_flags flags)
{
	g_not_implemented;
}
internal b8 string_match_wildcard(string str, string pattern, string_match_flags flags)
{
	g_not_implemented;
}
internal u64 string_find_needle(string str, u64 start_pos, string needle, string_match_flags flags)
{
	g_not_implemented;
}
internal u64 string_find_needle_reverse(string str, u64 start_pos, string needle, string_match_flags flags)
{
	g_not_implemented;
}
internal b8 string_is_before(string a, string b)
{
	g_not_implemented;
}
internal b8 string_char_match(u8 a, u8 b, string_match_flags flags)
{
	g_not_implemented;
}

internal string string_from_memory_size(arena* a, u64 size)
{
	g_not_implemented;
}

internal string string_fromCount(arena* a, u64 count)
{
	g_not_implemented;
}

internal string string_fromBitsU32(arena* a, u32 x)
{
	g_not_implemented;
}

internal string string_fromBitsU64(arena* a, u64 x)
{
	g_not_implemented;
}

internal string string_fromU64(arena* a, u64 u64, u32 radix, u8 min_digits, u8 digit_group_separator)
{
	g_not_implemented;
}

internal string string_fromS64(arena* a, i64 s64, u32 radix, u8 min_digits, u8 digit_group_separator)
{
	g_not_implemented;
}

internal string string_fromF64(arena* a, f64)
{
	g_not_implemented;
}

internal string_list string_split(arena* a, string str, string delim, string_split_flags flags)
{
	g_not_implemented;
}
