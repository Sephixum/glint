#include "../g_test_common.h"
#include "../../base/core/g_platform_and_compiler_defines.h"
#include "../../base/string/g_string.h"
#include <stdlib.h>

static void test_char_basic(void)
{
	GLINT_TEST_SECTION("char basic");
	GLINT_TEST_CHECK(char_is_space(' '));
	GLINT_TEST_CHECK(!char_is_space('a'));

	GLINT_TEST_CHECK(char_is_upper('A'));
	GLINT_TEST_CHECK(!char_is_upper('a'));

	GLINT_TEST_CHECK(char_is_lower('a'));
	GLINT_TEST_CHECK(!char_is_lower('B'));

	GLINT_TEST_CHECK(char_is_slash('\\'));
	GLINT_TEST_CHECK(char_is_slash('/'));
	GLINT_TEST_CHECK(!char_is_slash('2'));

	GLINT_TEST_CHECK(char_is_alpha('a'));
	GLINT_TEST_CHECK(!char_is_alpha('2'));
	GLINT_TEST_CHECK(!char_is_alpha('!'));

	GLINT_TEST_CHECK(char_is_digit('2', 10));
	GLINT_TEST_CHECK(char_is_digit('0', 10));
	GLINT_TEST_CHECK(char_is_digit('a', 16));
	GLINT_TEST_CHECK(char_is_digit('7', 8));
	GLINT_TEST_CHECK(!char_is_digit('a', 10));
	GLINT_TEST_CHECK(!char_is_digit('C', 2));

	GLINT_TEST_CHECK(char_to_lower('C') == 'c');
	GLINT_TEST_CHECK(char_to_lower('a') == 'a');

	GLINT_TEST_CHECK(!(char_to_upper('C') == 'c'));
	GLINT_TEST_CHECK(char_to_upper('d') == 'D');

	GLINT_TEST_CHECK(char_to_correct_slash('\\') == '/');
	GLINT_TEST_CHECK(char_to_correct_slash('/') == '/');

	{
		char const static_string[] = "123456789";
		u8*		   buf			   = malloc(10 * sizeof(u8));
		memcpy(buf, static_string, sizeof(static_string));

		// sizeof(static_string) includes null terminator, cstr_length does not
		GLINT_TEST_CHECK(cstr_length(buf) == sizeof(static_string) - 1);

		free(buf);
	}
}

static void test_cstr_basic(void)
{
	GLINT_TEST_SECTION("cstr basic");
	
	char const* hello = "hello";
	GLINT_TEST_CHECK_EQ_INT(cstr_length((u8*)hello), 5);
	
	char const* empty = "";
	GLINT_TEST_CHECK_EQ_INT(cstr_length((u8*)empty), 0);
}

static void test_string_basic(void)
{
	GLINT_TEST_SECTION("string basic");
	
	arena* a = arena_create_default();
	
	// Test string creation
	string str1 = string_lit_comp("hello");
	GLINT_TEST_CHECK_EQ_INT(str1.size, 5);
	GLINT_TEST_CHECK(str1.str[0] == 'h');
	
	// Test string_from_cstr
	string str2 = string_from_cstr("world");
	GLINT_TEST_CHECK_EQ_INT(str2.size, 5);
	
	// Test string_copy
	string str3 = string_copy(a, str1);
	GLINT_TEST_CHECK_EQ_INT(str3.size, str1.size);
	GLINT_TEST_CHECK(string_match(str1, str3, 0));
	
	arena_destroy(a);
}

static void test_string_edge_cases(void)
{
	GLINT_TEST_SECTION("string edge cases");
	
	// Test empty string
	string empty = string_zero();
	GLINT_TEST_CHECK_EQ_INT(empty.size, 0);
	GLINT_TEST_CHECK(empty.str == 0);
	
	// Test string with null bytes
	string str_with_null = string_lit_comp("hello\0world");
	GLINT_TEST_CHECK_EQ_INT(str_with_null.size, 11);
}

static void test_string_match(void)
{
	GLINT_TEST_SECTION("string match");
	
	string a = string_lit_comp("hello");
	string b = string_lit_comp("hello");
	string c = string_lit_comp("HELLO");
	string d = string_lit_comp("world");
	
	// Test exact match
	GLINT_TEST_CHECK(string_match(a, b, 0));
	GLINT_TEST_CHECK(!string_match(a, c, 0));
	GLINT_TEST_CHECK(!string_match(a, d, 0));
	
	// Test case insensitive
	GLINT_TEST_CHECK(string_match(a, c, string_match_flag_case_insensitive));
}

static void test_string_numeric_conversions(void)
{
	GLINT_TEST_SECTION("string numeric conversions");
	
	arena* a = arena_create_default();
	
	// Test u64_from_string
	string num_str = string_lit_comp("12345");
	u64 num = u64_from_string(num_str, 10);
	GLINT_TEST_CHECK_EQ_INT(num, 12345);
	
	// Test i64_from_string
	string neg_str = string_lit_comp("-42");
	i64 neg_num = i64_from_string(neg_str, 10);
	GLINT_TEST_CHECK_EQ_INT(neg_num, -42);
	
	// Test string_from_u64
	string result = string_from_u64(a, 999, 10, 0, 0);
	GLINT_TEST_CHECK(string_match(result, string_lit_comp("999"), 0));
	
	// Test hex
	string hex_str = string_lit_comp("FF");
	u64 hex_val = u64_from_string(hex_str, 16);
	GLINT_TEST_CHECK_EQ_INT(hex_val, 255);
	
	arena_destroy(a);
}

static void register_str_tests(void) __attribute__((constructor));
static void register_str_tests(void)
{
	GLINT_TEST_REGISTER(test_char_basic);
	GLINT_TEST_REGISTER(test_cstr_basic);
	GLINT_TEST_REGISTER(test_string_basic);
	GLINT_TEST_REGISTER(test_string_edge_cases);
	GLINT_TEST_REGISTER(test_string_match);
	GLINT_TEST_REGISTER(test_string_numeric_conversions);
}

// Include platform-specific OS implementations first
#if GLINT_OS_LINUX
	#include "../../base/os/platform/linux/g_os_memory_linux.c"
	#include "../../base/os/platform/linux/g_os_system_info_linux.c"
#elif GLINT_OS_WINDOWS
	#include "../../base/os/platform/windows/g_os_memory_windows.c"
	#include "../../base/os/platform/windows/g_os_system_info_windows.c"
#elif GLINT_OS_MAC
	// macOS implementations would go here
	#error "macOS OS implementations not yet added"
#else
	#error "Unsupported OS"
#endif

#include "../../base/memory/g_arena.c"
#include "../../base/string/g_string.c"
