#include "g_string.h"
#include "../core/g_platform_and_compiler_defines.h"
#include "../util/g_memory.h"
#include "../util/g_linked_list.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GLINT_UTF_REPLACEMENT_CODEPOINT 0xFFFDu
#define GLINT_UTF_MAX_CODEPOINT 0x10FFFFu

internal u32 utf8_seq_len(u8 lead)
{
	if ((lead & 0x80) == 0x00)
	{
		return 1; // 0xxxxxxx
	}
	if ((lead & 0xE0) == 0xC0)
	{
		return 2; // 110xxxxx
	}
	if ((lead & 0xF0) == 0xE0)
	{
		return 3; // 1110xxxx
	}
	if ((lead & 0xF8) == 0xF0)
	{
		return 4; // 11110xxx
	}
	return 0;
}

internal utf_decode utf8_decode(u8* str, u64 max)
{
	utf_decode result = {0};
	result.codepoint  = GLINT_UTF_REPLACEMENT_CODEPOINT;
	result.advance	  = 1;

	if (max == 0)
	{
		return result;
	}

	u8	lead = str[0];
	u32 len	 = utf8_seq_len(lead);

	if (len == 0 || len > max)
	{
		// invalid leading byte, or truncated at end of buffer
		return result;
	}

	u32 codepoint	  = 0;
	u32 min_codepoint = 0;

	switch (len)
	{
	case 1:
	{
		codepoint	  = lead;
		min_codepoint = 0;
	}
	break;
	case 2:
	{
		codepoint	  = lead & 0x1F;
		min_codepoint = 0x80;
	}
	break;
	case 3:
	{
		codepoint	  = lead & 0x0F;
		min_codepoint = 0x800;
	}
	break;
	case 4:
	{
		codepoint	  = lead & 0x07;
		min_codepoint = 0x10000;
	}
	break;
	}

	for (u64 i = 1; i < len; i++)
	{
		u8 cont = str[i];
		if ((cont & 0xC0) != 0x80)
		{
			// not a valid continuation byte -- bail, treat lead byte alone as invalid
			return result;
		}
		codepoint = (codepoint << 6) | (cont & 0x3F);
	}

	// reject overlong encodings, surrogate-range codepoints (invalid in utf8),
	// and anything past the valid Unicode range
	if (codepoint < min_codepoint || (codepoint >= 0xD800 && codepoint <= 0xDFFF) ||
		codepoint > GLINT_UTF_MAX_CODEPOINT)
	{
		return result;
	}

	result.codepoint = codepoint;
	result.advance	 = len;
	return result;
}

internal u32 utf8_encode(u8* dst, u32 codepoint)
{
	if (codepoint > GLINT_UTF_MAX_CODEPOINT || (codepoint >= 0xD800 && codepoint <= 0xDFFF))
	{
		codepoint = GLINT_UTF_REPLACEMENT_CODEPOINT;
	}

	if (codepoint <= 0x7F)
	{
		dst[0] = (u8)codepoint;
		return 1;
	}
	else if (codepoint <= 0x7FF)
	{
		dst[0] = (u8)(0xC0 | (codepoint >> 6));
		dst[1] = (u8)(0x80 | (codepoint & 0x3F));
		return 2;
	}
	else if (codepoint <= 0xFFFF)
	{
		dst[0] = (u8)(0xE0 | (codepoint >> 12));
		dst[1] = (u8)(0x80 | ((codepoint >> 6) & 0x3F));
		dst[2] = (u8)(0x80 | (codepoint & 0x3F));
		return 3;
	}
	else
	{
		dst[0] = (u8)(0xF0 | (codepoint >> 18));
		dst[1] = (u8)(0x80 | ((codepoint >> 12) & 0x3F));
		dst[2] = (u8)(0x80 | ((codepoint >> 6) & 0x3F));
		dst[3] = (u8)(0x80 | (codepoint & 0x3F));
		return 4;
	}
}

internal utf_decode utf16_decode(u16* str, u64 max)
{
	utf_decode result = {0};
	result.codepoint  = GLINT_UTF_REPLACEMENT_CODEPOINT;
	result.advance	  = 1;

	if (max == 0)
	{
		return result;
	}

	u16 unit0 = str[0];

	if (unit0 < 0xD800 || unit0 > 0xDFFF)
	{
		result.codepoint = unit0;
		result.advance	 = 1;
		return result;
	}

	if (unit0 >= 0xDC00)
	{
		return result;
	}

	if (max < 2)
	{
		return result;
	}

	u16 unit1 = str[1];
	if (unit1 < 0xDC00 || unit1 > 0xDFFF)
	{
		return result;
	}

	u32 codepoint	 = 0x10000 + (((u32)(unit0 - 0xD800)) << 10) + ((u32)(unit1 - 0xDC00));
	result.codepoint = codepoint;
	result.advance	 = 2;
	return result;
}

internal u32 utf16_encode(u16* dst, u32 codepoint)
{
	if (codepoint > GLINT_UTF_MAX_CODEPOINT || (codepoint >= 0xD800 && codepoint <= 0xDFFF))
	{
		codepoint = GLINT_UTF_REPLACEMENT_CODEPOINT;
	}

	if (codepoint <= 0xFFFF)
	{
		dst[0] = (u16)codepoint;
		return 1;
	}
	else
	{
		u32 v  = codepoint - 0x10000;
		dst[0] = (u16)(0xD800 + (v >> 10));
		dst[1] = (u16)(0xDC00 + (v & 0x3FF));
		return 2;
	}
}

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

internal string os_time_to_string(arena* a, os_time t)
{
	os_date d = os_time_to_date(t);

	char buf[32];
	int	 n = snprintf(buf,
					  sizeof(buf),
					  "%04u-%02u-%02u %02u:%02u:%02u.%03llu",
					  d.year,
					  d.month,
					  d.day,
					  d.hour,
					  d.minute,
					  d.second,
					  (unsigned long long)(d.nanosecond / 1'000'000));
	if (n < 0) return string_zero();

	return string_copy(a, (string){(u8*)buf, (u64)n});
}

internal string os_date_to_string(arena* a, os_date d)
{
	char buf[48];
	int	 n = snprintf(buf,
					  sizeof(buf),
					  "%04u-%02u-%02u %02u:%02u:%02u.%03llu %s",
					  d.year,
					  d.month,
					  d.day,
					  d.hour,
					  d.minute,
					  d.second,
					  (unsigned long long)(d.nanosecond / 1'000'000),
					  string_from_os_weekday(d.weekday).str);
	if (n < 0) return string_zero();

	return string_copy(a, (string){(u8*)buf, (u64)n});
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
			radix = 0b10, prefix_size = 2;
		}
		// octal
		else if (string_match(string_prefix(str, 2), string_lit("0o"), string_match_flag_case_insensitive) &&
				 str.size > 1)
		{
			radix = 0o10, prefix_size = 2;
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
	b8 result = 0;
	if (a.size == b.size && flags == 0)
	{
		result = g_memory_match(a.str, b.str, b.size);
	}
	else if (a.size == b.size || (flags & string_match_flag_right_side_sloppy))
	{
		b8	case_insensitive  = (b8)(flags & string_match_flag_case_insensitive);
		b8	slash_insensitive = (b8)(flags & string_match_flag_slash_insensitive);
		u64 size			  = g_min(a.size, b.size);
		result				  = 1;
		g_for_each_index(i, size)
		{
			u8 at = a.str[i];
			u8 bt = b.str[i];
			if (case_insensitive)
			{
				at = char_to_upper(at);
				bt = char_to_upper(bt);
			}
			if (slash_insensitive)
			{
				at = char_to_correct_slash(at);
				bt = char_to_correct_slash(bt);
			}
			if (at != bt)
			{
				result = 0;
				break;
			}
		}
	}
	return result;
}

internal b8 string_char_match(u8 a, u8 b, string_match_flags flags)
{
	u8 at = a;
	u8 bt = b;
	if (flags & string_match_flag_case_insensitive)
	{
		at = char_to_upper(at);
		bt = char_to_upper(bt);
	}
	if (flags & string_match_flag_slash_insensitive)
	{
		at = char_to_correct_slash(at);
		bt = char_to_correct_slash(bt);
	}
	return (at == bt);
}

internal b8 string_match_wildcard(string str, string pattern, string_match_flags flags)
{
	b8 matched = 0;

	u64 pattern_cursor = 0;
	u64 string_cursor  = 0;

	u64 pattern_start = max_u64;
	u64 string_start  = 0;

	for (;;)
	{
		if (pattern_cursor == pattern.size)
		{
			if (string_cursor == str.size || (flags & string_match_flag_right_side_sloppy))
			{
				matched = 1;
				break;
			}
		}

		if (string_cursor == str.size)
		{
			while (pattern_cursor < pattern.size && pattern.str[pattern_cursor] == '*')
			{
				pattern_cursor += 1;
			}
			matched = (pattern_cursor == pattern.size);
			break;
		}

		if (pattern_cursor < pattern.size && pattern.str[pattern_cursor] == '*')
		{
			pattern_start = pattern_cursor;
			string_start  = string_cursor;
			pattern_cursor += 1;
			continue;
		}

		if (pattern_cursor < pattern.size &&
			(pattern.str[pattern_cursor] == '?' ||
			 string_char_match(str.str[string_cursor], pattern.str[pattern_cursor], flags)))
		{
			string_cursor += 1;
			pattern_cursor += 1;
			continue;
		}

		if (pattern_start != max_u64)
		{
			pattern_cursor = pattern_start + 1;
			string_start += 1;
			string_cursor = string_start;
			continue;
		}

		break;
	}

	return matched;
}

internal u64 string_find_needle(string str, u64 start_pos, string needle, string_match_flags flags)
{
	u8* p			= str.str + start_pos;
	u64 stop_offset = g_max(str.size + 1, needle.size) - needle.size;
	u8* stop_p		= str.str + stop_offset;
	if (needle.size > 0)
	{
		u8*				   string_opl				  = str.str + str.size;
		string			   needle_tail				  = string_skip(needle, 1);
		string_match_flags adjusted_flags			  = flags | string_match_flag_right_side_sloppy;
		u8				   needle_first_char_adjusted = needle.str[0];
		if (adjusted_flags & string_match_flag_case_insensitive)
		{
			needle_first_char_adjusted = char_to_upper(needle_first_char_adjusted);
		}
		for (; p < stop_p; p += 1)
		{
			u8 haystack_char_adjusted = *p;
			if (adjusted_flags & string_match_flag_case_insensitive)
			{
				haystack_char_adjusted = char_to_upper(haystack_char_adjusted);
			}
			if (haystack_char_adjusted == needle_first_char_adjusted)
			{
				if (string_match(string_range(p + 1, string_opl), needle_tail, adjusted_flags))
				{
					break;
				}
			}
		}
	}
	u64 result = str.size;
	if (p < stop_p)
	{
		result = (u64)(p - str.str);
	}
	return result;
}

internal b8 string_contains(string str, string needle, string_match_flags flags)
{
	// string_find_needle returns str.size when the needle isn't present.
	return string_find_needle(str, 0, needle, flags) != str.size;
}

internal u64 string_find_needle_reverse(string str, u64 start_pos, string needle, string_match_flags flags)
{
	u64 result = 0;
	for (i64 i = str.size - start_pos - needle.size; i >= 0; --i)
	{
		string haystack = string_substr(str, i, i + needle.size);
		if (string_match(haystack, needle, flags))
		{
			result = (u64)i + needle.size;
			break;
		}
	}
	return result;
}

internal b8 string_is_before(string a, string b)
{
	b8 result = 0;
	{
		u64 common_size = g_min(a.size, b.size);
		g_for_each_index(off, common_size)
		{
			if (a.str[off] < b.str[off])
			{
				result = 1;
				break;
			}
			else if (a.str[off] > b.str[off])
			{
				result = 0;
				break;
			}
			else if (off + 1 == common_size)
			{
				result = (a.size < b.size);
			}
		}
	}
	return result;
}

internal string string_from_memory_size(arena* a, u64 size)
{
	string result = string_zero();
	{
		if (size < g_kb(1))
		{
			result = string_f(a, "%llu byte%s", size, size == 1 ? "" : "s");
		}
		else if (size < g_mb(1))
		{
			result = string_f(a, "%llu.%02llu KiB", size / g_kb(1), ((size * 100) / g_kb(1)) % 100);
		}
		else if (size < g_gb(1))
		{
			result = string_f(a, "%llu.%02llu MiB", size / g_mb(1), ((size * 100) / g_mb(1)) % 100);
		}
		else if (size < g_tb(1))
		{
			result = string_f(a, "%llu.%02llu GiB", size / g_gb(1), ((size * 100) / g_gb(1)) % 100);
		}
		else
		{
			result = string_f(a, "%llu.%02llu TiB", size / g_tb(1), ((size * 100) / g_tb(1)) % 100);
		}
	}
	return result;
}

internal string string_from_count(arena* a, u64 count)
{
	string result = string_zero();
	{
		if (count < 1 * 1000)
		{
			result = string_f(a, "%llu", count);
		}
		else if (count < 1000000)
		{
			u64 const frac = ((count * 100) / 1000) % 100;
			if (frac > 0)
			{
				result = string_f(a, "%llu.%02lluK", count / 1000, frac);
			}
			else
			{
				result = string_f(a, "%lluK", count / 1000);
			}
		}
		else if (count < 1000000000)
		{
			u64 const frac = ((count * 100) / 1000000) % 100;
			if (frac > 0)
			{
				result = string_f(a, "%llu.%02lluM", count / 1000000, frac);
			}
			else
			{
				result = string_f(a, "%lluM", count / 1000000);
			}
		}
		else
		{
			u64 const frac = ((count * 100) * 1000000000) % 100;
			if (frac > 0)
			{
				result = string_f(a, "%llu.%02lluB", count / 1000000000, frac);
			}
			else
			{
				result = string_f(a, "%lluB", count / 1000000000, frac);
			}
		}
	}
	return result;
}

internal string string_from_bits_u32(arena* a, u32 x)
{
	u8 const c0		= 'a' + ((x >> 28) & 0xf);
	u8 const c1		= 'a' + ((x >> 24) & 0xf);
	u8 const c2		= 'a' + ((x >> 20) & 0xf);
	u8 const c3		= 'a' + ((x >> 16) & 0xf);
	u8 const c4		= 'a' + ((x >> 12) & 0xf);
	u8 const c5		= 'a' + ((x >> 8) & 0xf);
	u8 const c6		= 'a' + ((x >> 4) & 0xf);
	u8 const c7		= 'a' + ((x >> 0) & 0xf);
	string	 result = string_f(a, "%c%c%c%c%c%c%c%c", c0, c1, c2, c3, c4, c5, c6, c7);
	return result;
}

internal string string_from_bits_u64(arena* a, u64 x)
{
	u8 const c0 = 'a' + ((x >> 60) & 0xf);
	u8 const c1 = 'a' + ((x >> 56) & 0xf);
	u8 const c2 = 'a' + ((x >> 52) & 0xf);
	u8 const c3 = 'a' + ((x >> 48) & 0xf);
	u8 const c4 = 'a' + ((x >> 44) & 0xf);
	u8 const c5 = 'a' + ((x >> 40) & 0xf);
	u8 const c6 = 'a' + ((x >> 36) & 0xf);
	u8 const c7 = 'a' + ((x >> 32) & 0xf);
	u8 const c8 = 'a' + ((x >> 28) & 0xf);
	u8 const c9 = 'a' + ((x >> 24) & 0xf);
	u8 const ca = 'a' + ((x >> 20) & 0xf);
	u8 const cb = 'a' + ((x >> 16) & 0xf);
	u8 const cc = 'a' + ((x >> 12) & 0xf);
	u8 const cd = 'a' + ((x >> 8) & 0xf);
	u8 const ce = 'a' + ((x >> 4) & 0xf);
	u8 const cf = 'a' + ((x >> 0) & 0xf);
	string	 result =
		string_f(a, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd, ce, cf);
	return result;
}

internal string string_from_u64(arena* a, u64 u64_value, u32 radix, u8 min_digits, u8 digit_group_separator)
{
	string result = string_zero();
	{
		// prefix
		string prefix = string_zero();
		switch (radix)
		{
		case 16:
		{
			prefix = string_lit("0x");
		}
		break;
		case 8:
		{
			prefix = string_lit("0o");
		}
		break;
		case 2:
		{
			prefix = string_lit("0b");
		}
		break;
		}

		// determine nmbr of chars between separators
		u8 digit_group_size = 3;
		switch (radix)
		{
		default:
			break;
		case 2:
		case 8:
		case 16:
		{
			digit_group_size = 4;
		}
		break;
		}

		// prep
		u64 needed_leading_0s = 0;
		{
			u64 needed_digits = 1;
			{
				u64 u64_reduce = u64_value;
				for (;;)
				{
					u64_reduce /= radix;
					if (u64_reduce == 0)
					{
						break;
					}
					needed_digits += 1;
				}
			}
			needed_leading_0s	  = (min_digits > needed_digits) ? min_digits - needed_digits : 0;
			u64 needed_separators = 0;
			if (digit_group_separator != 0)
			{
				needed_separators = (needed_digits + needed_leading_0s) / digit_group_size;
				if (needed_separators > 0 && (needed_digits + needed_leading_0s) % digit_group_size == 0)
				{
					needed_separators -= 1;
				}
			}
			result.size				= prefix.size + needed_leading_0s + needed_separators + needed_digits;
			result.str				= arena_push_array_nozero(a, u8, result.size + 1);
			result.str[result.size] = 0;
		}

		// fill contents
		{
			u64 u64_reduce			   = u64_value;
			u64 digits_until_separator = digit_group_size;
			g_for_each_index(idx, result.size)
			{
				if (digits_until_separator == 0 && digit_group_separator != 0)
				{
					result.str[result.size - idx - 1] = digit_group_separator;
					digits_until_separator			  = digit_group_size + 1;
				}
				else
				{
					result.str[result.size - idx - 1] = char_to_lower(integer_symbols[u64_reduce % radix]);
					u64_reduce /= radix;
				}
				digits_until_separator -= 1;
				if (u64_reduce == 0)
				{
					break;
				}
			}
			g_for_each_index(leading_0_idx, needed_leading_0s)
			{
				result.str[prefix.size + leading_0_idx] = '0';
			}
		}

		// fill prefix
		if (prefix.size != 0)
		{
			g_memory_copy(result.str, prefix.str, prefix.size);
		}
	}
	return result;
}

internal string string_from_i64(arena* a, i64 i64_value, u32 radix, u8 min_digits, u8 digit_group_separator)
{
	string result = string_zero();
	if (i64_value < 0)
	{
		temp_arena scratch = arena_get_scratch(&a, 1);
		string	   numeric_part =
			string_from_u64(scratch.arena, (u64)(-i64_value), radix, min_digits, digit_group_separator);
		result = string_f(a, "-%s", numeric_part);
		arena_release_scratch(scratch);
	}
	else
	{
		result = string_from_u64(a, (u64)i64_value, radix, min_digits, digit_group_separator);
	}
	return result;
}

internal string string_from_f64(arena* a, f64 f64_value)
{
	u8* buf	 = arena_push_array(a, u8, 64);
	u64 size = snprintf((char*)buf, 64, "%f", f64_value);
	return string_create(buf, size);
}

internal string string_from_duration(arena* a, duration d)
{
	u8* buf = arena_push_array(a, u8, 64);
	u64 ns  = d.nanos;
	u64 size;

	if (ns < 1000)
	{
		size = (u64)snprintf((char*)buf, 64, "%lluns", (unsigned long long)ns);
	}
	else if (ns < 1000000)
	{
		f64 us = (f64)ns / 1000.0;
		size   = (u64)snprintf((char*)buf, 64, "%.2fus", us);
	}
	else if (ns < 1000000000)
	{
		f64 ms = (f64)ns / 1000000.0;
		size   = (u64)snprintf((char*)buf, 64, "%.2fms", ms);
	}
	else if (ns < 60000000000)
	{
		f64 s = (f64)ns / 1000000000.0;
		size  = (u64)snprintf((char*)buf, 64, "%.2fs", s);
	}
	else
	{
		duration_extended de = duration_break(d);
		if (de.hours > 0)
		{
			size = (u64)snprintf((char*)buf, 64, "%lluh %llum %llus",
								 (unsigned long long)de.hours,
								 (unsigned long long)de.minutes,
								 (unsigned long long)de.seconds);
		}
		else
		{
			size = (u64)snprintf((char*)buf, 64, "%llum %llus",
								 (unsigned long long)de.minutes,
								 (unsigned long long)de.seconds);
		}
	}

	return string_create(buf, size);
}

internal string_list string_split(arena* a, string str, string delim, string_split_flags flags)
{
	string_list list		 = {0};
	b8			keep_empties = (flags & string_split_flag_keep_empties);
	u8*			ptr			 = str.str;
	u8*			opl			 = str.str + str.size;

	while (ptr < opl)
	{
		u8* first = ptr;
		for (; ptr < opl; ++ptr)
		{
			u8 c		= *ptr;
			b8 is_split = 0;

			g_for_each_index(i, delim.size)
			{
				if (delim.str[i] == c)
				{
					is_split = 1;
					break;
				}
			}

			if (is_split)
			{
				break;
			}
		}

		string string = string_range(first, ptr);
		if (keep_empties || string.size > 0)
		{
			string_list_push(a, &list, string);
		}

		++ptr;
	}

	return list;
}

internal string_node* string_list_push_node(string_list* list, string_node* node)
{
	g_single_ll_queue_push(list->first, list->last, node);
	list->node_count += 1;
	list->total_size += node->string.size;
	return node;
}

internal string_node* string_list_push_node_set_string(string_list* list, string_node* node, string str)
{
	g_single_ll_queue_push(list->first, list->last, node);
	list->node_count += 1;
	list->total_size += node->string.size;
	node->string = str;
	return node;
}

internal string_node* string_list_push_node_front(string_list* list, string_node* node)
{
	g_single_ll_queue_push_front(list->first, list->last, node);
	list->node_count += 1;
	list->total_size += node->string.size;
	return node;
}

internal string_node* string_list_push_node_front_set_string(string_list* list, string_node* node, string str)
{
	g_single_ll_queue_push_front(list->first, list->last, node);
	list->node_count += 1;
	list->total_size += node->string.size;
	node->string = str;
	return node;
}

internal string_node* string_list_push(arena* a, string_list* list, string str)
{
	string_node* node = arena_push_array_nozero(a, string_node, 1);
	string_list_push_node_front_set_string(list, node, str);
	return node;
}

internal string_node* string_list_push_front(arena* a, string_list* list, string str)
{
	string_node* node = arena_push_array_nozero(a, string_node, 1);
	string_list_push_node_front_set_string(list, node, str);
	return node;
}

internal void string_list_concat_in_place(string_list* list, string_list* to_push)
{
	if (to_push->node_count != 0)
	{
		if (list->last)
		{
			list->node_count += to_push->node_count;
			list->total_size += to_push->total_size;
			list->last->next = to_push->first;
			list->last		 = to_push->last;
		}
		else
		{
			*list = *to_push;
		}
		g_memory_zero_struct(to_push);
	}
}

internal string_node* string_list_push_aligner(arena* a, string_list* list, u64 min, u64 align)
{
	read_only local_persist u8 zeroes[64] = {0};
	g_assert_always(g_is_pow2_or_zero(align));
	u64 pad = g_max(min, g_align_pad_pow2(list->total_size, align));
	if (pad < sizeof(zeroes))
	{
		return string_list_push(a, list, string_create(zeroes, pad));
	}
	else
	{
		return string_list_push(a, list, string_create(arena_push_array(a, u8, pad), pad));
	}
}

internal string_node* string_list_push_f(arena* a, string_list* list, char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	string		 string = string_fv(a, fmt, args);
	string_node* result = string_list_push(a, list, string);
	va_end(args);
	return result;
}

internal string_node* string_list_push_front_f(arena* a, string_list* list, char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	string		 string = string_fv(a, fmt, args);
	string_node* result = string_list_push_front(a, list, string);
	va_end(args);
	return result;
}

internal string_node* string_list_pop_front(string_list* list)
{
	string_node* node = 0;
	if (list->node_count)
	{
		node = list->first;
		g_assert_always(list->total_size >= list->first->string.size);
		list->node_count -= 1;
		list->total_size -= list->first->string.size;
		g_single_ll_queue_pop(list->first, list->last);
	}
	return node;
}

internal string_list string_list_copy(arena* a, string_list* list)
{
	string_list result = {0};
	for (string_node* node = list->first; node != 0; node = node->next)
	{
		string_node* new_node	= arena_push_array_nozero(a, string_node, 1);
		string		 new_string = string_copy(a, node->string);
		string_list_push_node_set_string(&result, new_node, new_string);
	}
	return result;
}

internal string_list string_list_substr(arena* a, string_list list, u64 s, u64 f)
{
	string_list	 result = {0};
	string_node* n		= list.first;

	u64 front_min = 0;
	{
		u64 cursor = 0;
		for (; n != 0; cursor += n->string.size, n = n->next)
		{
			if (cursor + n->string.size > s)
			{
				front_min = s - cursor;
				break;
			}
		}
	}

	if (front_min > 0)
	{
		u64 front_max = front_min + g_min(g_dim_u64(s, f), n->string.size);
		string_list_push(a, &result, string_substr(n->string, front_min, front_max));
		n = n->next;
	}

	for (; n != 0; n = n->next)
	{
		if (result.total_size >= g_dim_u64(s, f))
		{
			break;
		}
		u64 copy_max  = g_dim_u64(s, f) - result.total_size;
		u64 copy_size = g_min(copy_max, n->string.size);
		string_list_push(a, &result, string_substr(n->string, 0, copy_size));
	}

    return result;
}

internal string_array string_array_zero(void)
{
	g_not_implemented;
}

internal string_array string_array_from_list(arena* a, string_list* list)
{
	g_not_implemented;
}

internal string_array string_array_reserve(arena* a, u64 count)
{
	g_not_implemented;
}
internal string_array string_array_copy(arena* a, string_array array)
{
	g_not_implemented;
}
internal char** cstr_from_string_array(arena* a, string_array array)
{
	g_not_implemented;
}

// Internal helper: byte index of the last path separator ('/' or '\') in `path`,
// or `path.size` when there is none. Backs the path-slicing primitives below.
internal u64 string_path__last_sep(string path)
{
	u64 sep = path.size;
	for (u64 i = path.size; i-- > 0;)
	{
		if (char_is_slash(path.str[i]))
		{
			sep = i;
			break;
		}
	}
	return sep;
}
internal string string_strip_trailing_slashes(string path)
{
	while (path.size > 0 && char_is_slash(path.str[path.size - 1]))
	{
		path.size -= 1;
	}
	return path;
}
internal path_style string_path_get_style(string path)
{
	g_not_implemented;
}
internal b8 string_path_is_absolute(string path)
{
	g_not_implemented;
}
internal b8 string_path_is_relative(string path)
{
	g_not_implemented;
}
internal b8 string_path_has_root(string path)
{
	g_not_implemented;
}
internal string string_path_dir_name(string path)
{
	u64 sep = string_path__last_sep(path);
	if (sep >= path.size)
	{
		return string_lit_comp(".");
	}
	if (sep == 0)
	{
		return string_lit_comp("/");
	}
	return string_prefix(path, sep);
}
internal string string_path_base_name(string path)
{
	u64 sep = string_path__last_sep(path);
	if (sep >= path.size)
	{
		return path;
	}
	return string_skip(path, sep + 1);
}
internal string string_path_ext(string path)
{
	string base = string_path_base_name(path);
	u64		dot  = base.size;
	for (u64 i = base.size; i-- > 0;)
	{
		if (base.str[i] == '.')
		{
			dot = i;
			break;
		}
	}
	// No dot, or a leading dot (dotfile like ".bashrc") → no extension.
	if (dot == 0 || dot >= base.size)
	{
		return string_zero();
	}
	return string_skip(base, dot);
}
internal string string_path_stem(string path)
{
	string base = string_path_base_name(path);
	string ext  = string_path_ext(path);
	if (ext.size == 0)
	{
		return base;
	}
	return string_chop(base, ext.size);
}
internal string string_path_join(arena* a, string base, string relative)
{
	g_not_implemented;
}
internal string string_path_to_system(arena* a, string path)
{
	g_not_implemented;
}
internal string string_path_to_unix(arena* a, string path)
{
	g_not_implemented;
}

internal string16 string16_from_string(arena* a, string str)
{
	u16* buf   = arena_push_array(a, u16, str.size * 2); // worst case
	u64	 buf_i = 0;
	u64	 i	   = 0;
	while (i < str.size)
	{
		utf_decode dec = utf8_decode(str.str + i, str.size - i);
		buf_i += utf16_encode(buf + buf_i, dec.codepoint);
		i += dec.advance;
	}
	return (string16){buf, buf_i};
}

internal string string_from_string16(arena* a, string16 str16)
{
	u8* buf	  = arena_push_array(a, u8, str16.size * 4);
	u64 buf_i = 0;
	u64 i	  = 0;

	while (i < str16.size)
	{
		utf_decode dec = utf16_decode(str16.str + i, str16.size - i);
		buf_i += utf8_encode(buf + buf_i, dec.codepoint);
		i += dec.advance;
	}

	return (string){buf, buf_i};
}

// Enum to string functions

internal string string_from_arena_flags(arena* a, arena_flags flags)
{
	if (flags == 0)
	{
		return string_lit_comp("none");
	}
	
	string_list list = {0};
	
	if (flags & arena_flag_no_chain)
	{
		string_node* node = arena_push_array_nozero(a, string_node, 1);
		string_list_push_node_set_string(&list, node, string_lit_comp("no_chain"));
	}
	if (flags & arena_flag_large_pages)
	{
		string_node* node = arena_push_array_nozero(a, string_node, 1);
		string_list_push_node_set_string(&list, node, string_lit_comp("large_pages"));
	}
	
	// Join with ", "
	string result = string_zero();
	if (list.first)
	{
		u64 total_size = 0;
		u64 count = 0;
		for (string_node* node = list.first; node != 0; node = node->next)
		{
			total_size += node->string.size;
			count++;
		}
		if (count > 1)
		{
			total_size += (count - 1) * 2; // for ", " separators
		}
		
		u8* buffer = arena_push_array_nozero(a, u8, total_size + 1);
		u64 pos = 0;
		for (string_node* node = list.first; node != 0; node = node->next)
		{
			if (node != list.first)
			{
				buffer[pos++] = ',';
				buffer[pos++] = ' ';
			}
			g_memory_copy(buffer + pos, node->string.str, node->string.size);
			pos += node->string.size;
		}
		buffer[total_size] = 0;
		result = (string){buffer, total_size};
	}
	
	return result;
}

internal string string_from_string_match_flags(arena* a, string_match_flags flags)
{
	if (flags == 0)
	{
		return string_lit_comp("none");
	}
	
	string_list list = {0};
	
	if (flags & string_match_flag_case_insensitive)
	{
		string_node* node = arena_push_array_nozero(a, string_node, 1);
		string_list_push_node_set_string(&list, node, string_lit_comp("case_insensitive"));
	}
	if (flags & string_match_flag_right_side_sloppy)
	{
		string_node* node = arena_push_array_nozero(a, string_node, 1);
		string_list_push_node_set_string(&list, node, string_lit_comp("right_side_sloppy"));
	}
	if (flags & string_match_flag_slash_insensitive)
	{
		string_node* node = arena_push_array_nozero(a, string_node, 1);
		string_list_push_node_set_string(&list, node, string_lit_comp("slash_insensitive"));
	}
	
	// Join with ", "
	string result = string_zero();
	if (list.first)
	{
		u64 total_size = 0;
		u64 count = 0;
		for (string_node* node = list.first; node != 0; node = node->next)
		{
			total_size += node->string.size;
			count++;
		}
		if (count > 1)
		{
			total_size += (count - 1) * 2; // for ", " separators
		}
		
		u8* buffer = arena_push_array_nozero(a, u8, total_size + 1);
		u64 pos = 0;
		for (string_node* node = list.first; node != 0; node = node->next)
		{
			if (node != list.first)
			{
				buffer[pos++] = ',';
				buffer[pos++] = ' ';
			}
			g_memory_copy(buffer + pos, node->string.str, node->string.size);
			pos += node->string.size;
		}
		buffer[total_size] = 0;
		result = (string){buffer, total_size};
	}
	
	return result;
}

internal string string_from_string_split_flags(arena* a, string_split_flags flags)
{
	if (flags == 0)
	{
		return string_lit_comp("none");
	}
	
	if (flags & string_split_flag_keep_empties)
	{
		return string_lit_comp("keep_empties");
	}
	
	return string_lit_comp("none");
}

internal string string_from_path_style(arena* a, path_style style)
{
	if (style == 0)
	{
		return string_lit_comp("none");
	}
	
	string_list list = {0};
	
	if (style & path_style_null)
	{
		string_node* node = arena_push_array_nozero(a, string_node, 1);
		string_list_push_node_set_string(&list, node, string_lit_comp("null"));
	}
	if (style & path_style_relative)
	{
		string_node* node = arena_push_array_nozero(a, string_node, 1);
		string_list_push_node_set_string(&list, node, string_lit_comp("relative"));
	}
	if (style & path_style_windows_absolute)
	{
		string_node* node = arena_push_array_nozero(a, string_node, 1);
		string_list_push_node_set_string(&list, node, string_lit_comp("windows_absolute"));
	}
	if (style & path_style_unix_absolute)
	{
		string_node* node = arena_push_array_nozero(a, string_node, 1);
		string_list_push_node_set_string(&list, node, string_lit_comp("unix_absolute"));
	}
	
	// Join with ", "
	string result = string_zero();
	if (list.first)
	{
		u64 total_size = 0;
		u64 count = 0;
		for (string_node* node = list.first; node != 0; node = node->next)
		{
			total_size += node->string.size;
			count++;
		}
		if (count > 1)
		{
			total_size += (count - 1) * 2; // for ", " separators
		}
		
		u8* buffer = arena_push_array_nozero(a, u8, total_size + 1);
		u64 pos = 0;
		for (string_node* node = list.first; node != 0; node = node->next)
		{
			if (node != list.first)
			{
				buffer[pos++] = ',';
				buffer[pos++] = ' ';
			}
			g_memory_copy(buffer + pos, node->string.str, node->string.size);
			pos += node->string.size;
		}
		buffer[total_size] = 0;
		result = (string){buffer, total_size};
	}
	
	return result;
}
